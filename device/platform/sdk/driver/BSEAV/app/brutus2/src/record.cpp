/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: record.cpp $
* $brcm_Revision: 9 $
* $brcm_Date: 10/11/12 5:25p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/record.cpp $
* 
* 9   10/11/12 5:25p tokushig
* SW7231-749: clear current video pointer when stopping record
* 
* 8   10/11/12 4:10p jrubio
* SW7346-1004: fix debug issues
* 
* 7   10/11/12 3:22p tokushig
* SW7231-749: add member variable initializations
* 
* 6   10/11/12 2:37p jrubio
* SW7346-1004: rename functions
* 
* 5   10/10/12 5:31p tokushig
* SW7231-749: adjust debug output for release
* 
* 4   9/25/12 12:45p jrubio
* SW7346-1004: record needs no arguments
* 
* 3   9/18/12 5:44p jrubio
* SW7346-1004: move addRecordPids out of record
* 
* 2   9/17/12 10:27a jrubio
* SW7346-1004: modify record functions
* 
* 1   8/29/12 10:46a jrubio
* SW7346-1004: add record files
* 
* 
* 
*********************************************************************************/
#include "band.h"
#include "avtypes.h"
#include "xmltags.h"
#include "record.h"
#include "mxmlparser.h"
#include <unistd.h> //file i/o
#include <fcntl.h>

BDBG_MODULE(brutus_record);


CRecord::CRecord(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CResource(name, number, eBoardResource_record, pCfg),
    _file(NULL),
    _stickyFile(NULL),
    _customFile(NULL),
    _recpump(NULL),
    _record(NULL),
    _pParserBand(NULL),
    _currentVideo(NULL)
{
    
}


CRecord::~CRecord()
{
   NEXUS_Record_Destroy(_record);
   NEXUS_Recpump_Close(_recpump);
   _record= NULL;
   _recpump= NULL;
}

void CRecord::dump(void)
{
   BDBG_ERR(("record number %d",_number));
}

static void endOfStreamCallback(void *context, int param)
{
    CRecord * record = (CRecord *) context;
    BSTD_UNUSED(context);
    if (param) {
        BDBG_WRN(("end of stream"));
    }
    else { 
        
        BDBG_WRN(("beginning of stream"));
    }
    return;
}

eRet CRecord::initialize()
{
    BDBG_MSG(("INIT!"));
    _recpump = NEXUS_Recpump_Open(_number, NULL);
    if (!_recpump)
    {
        BDBG_ERR(("Cannot Open Record"));
        return eRet_ExternalError;
    }
    _record = NEXUS_Record_Create();
    if (!_record)
    {
        BDBG_ERR(("Cannot Open Playback"));
        NEXUS_Recpump_Close(_recpump);
        return eRet_ExternalError;
    }


    NEXUS_Record_GetSettings(_record, &_recordSettings);
    _recordSettings.recpump = _recpump;
    NEXUS_Record_SetSettings(_record, &_recordSettings);
    
    BDBG_MSG(("added record/recpump #%d",_number));
    return eRet_Ok;
}


/* LUA record */
eRet CRecord::start()
{
    
    eRet ret = eRet_ExternalError;
    NEXUS_Error rc = NEXUS_SUCCESS;
    MString fullFilePath;
    MString fullIndexPath;
    
    if (_currentVideo == NULL || _currentVideo->_path == NULL ) 
    {
        BDBG_ERR(("Please setVideo(CVideo *Video) before calling %s ",__FUNCTION__));
        return  eRet_ExternalError;    
    }

    if(_currentVideo->_isRecordActive == true)
    {
        BDBG_WRN(("Record is Active already continue to Record"));
        return eRet_Ok;
    }

    
    BDBG_WRN(("Record START"));
    /* IndexName will always be the same as the filename except with .nav extension.*/
    fullFilePath = _currentVideo->_path + _currentVideo->_mediaName;
    fullIndexPath = _currentVideo->_path + _currentVideo->_indexName;

    BDBG_ERR(("Opening files: '%s' '%s'",fullFilePath.s(), fullIndexPath.s()));

    _file = NEXUS_FileRecord_OpenPosix(fullFilePath.s(),fullIndexPath.s());
    if (!_file) 
    {
        BDBG_ERR(("can't open files: '%s' '%s'",fullFilePath.s(), fullIndexPath.s()));
        goto error;
    }

    
    /* Start record */
    rc = NEXUS_Record_Start(_record, _file);
    CHECK_NEXUS_ERROR_GOTO("record start failed", ret, rc, error);

    _currentVideo->_isRecordActive = true;

    ret = notifyObservers(eNotify_RecordStarted, this);
    CHECK_ERROR_GOTO("error notifying observers", ret, error);

    BDBG_MSG(("Record Started"));

    return eRet_Ok;
error:
    return ret;
}


/* Is Active */
bool CRecord::isActive(void)
{
   if (_currentVideo != NULL && _currentVideo->_isRecordActive == true ) 
      return true;
   else
      return false;

}

eRet CRecord::stop()
{
    eRet ret = eRet_Ok;

    BDBG_MSG((" Record stop"));

    if (_currentVideo->_isRecordActive == false || _currentVideo == NULL) 
    {
        BDBG_WRN((" Record is not active"));
        ret =eRet_ExternalError;
        goto error;
    }

    /* Close all Pids associate with this plyaback and close the PID channels */
    NEXUS_Record_Stop(_record);
    NEXUS_FileRecord_Close(_file);
    _currentVideo->_pidMgr.closeRecordPidChannels(this);
    _currentVideo->closeVideo();

    _currentVideo->_isRecordActive = false;

    ret = notifyObservers(eNotify_RecordStopped, this);
    CHECK_ERROR_GOTO("error notifying observers", ret, error);

    _currentVideo = NULL;
error:
    return ret;
}




void CRecord::closePids(void)
{
    _currentVideo->_pidMgr.closeRecordPids(this);
}

NEXUS_RecordSettings CRecord::getSettings(void)
{
    NEXUS_Record_GetSettings(_record, &_recordSettings);
    return _recordSettings;
}

void CRecord::setSettings(NEXUS_RecordSettings recordSettings)
{
    _recordSettings = recordSettings;
    if (NEXUS_Record_SetSettings(_record, &_recordSettings)!= NEXUS_SUCCESS)
    {
        BDBG_ERR(("Cannot SetSettings"));
    }
}

