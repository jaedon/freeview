/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: record.h $
* $brcm_Revision: 5 $
* $brcm_Date: 10/11/12 4:10p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/record.h $
* 
* 5   10/11/12 4:10p jrubio
* SW7346-1004: fix debug issues
* 
* 4   10/11/12 2:37p jrubio
* SW7346-1004: rename functions
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

#ifndef RECORD_H__
#define RECORD_H__

#include "nexus_types.h"
#include "nexus_record.h"
#include "nexus_file.h"

#include "resource.h"
#include "pid.h"
#include "mvc.h"
#include "pidmgr.h"
#include "playback.h"
#include "videolist.h"
#include "mlist.h"
#include "tspsimgr2.h"
#include "mstring.h"
#include "mstringlist.h"
#include "mxmlelement.h"
#include "bmedia_probe.h"


#ifdef __cplusplus
extern "C" {
#endif

class CControl;
class CVideo;
class CParserBand;

class CRecordData
{
public:
    CRecordData(const char * strFileName = "filename.mpg", const char * strPath = NULL,CVideo * video=NULL) :
        _strFileName(strFileName),
        _strPath(strPath), 
        _video(video) {};

public:
    MString  _strFileName;
    MString  _strIndexName;
    MString  _strPath;
    CVideo * _video;
};

class CRecord : public CResource
{
public:

    CRecord(const char * name, const uint16_t number, CConfiguration * pCfg);
    
    ~CRecord(void);
    eRet initialize();
    

    eRet start();
    eRet stop();
    
    eRet parseInfo(const char * filename);
    void closePids(void);
    NEXUS_RecordHandle getRecord(void) {return _record; };
    NEXUS_RecordSettings getSettings(void);
    void setSettings(NEXUS_RecordSettings recordSettings);

    void setVideo(CVideo * video) { _currentVideo = video; };
    CVideo * getVideo(void) { return _currentVideo; };
    CPid * getPid(uint16_t index, ePidType type){return _currentVideo->_pidMgr.getPid(index,type); };
    CParserBand * getBand(void) { return _pParserBand;}
    void setBand(CParserBand * pParserBand) { _pParserBand = pParserBand;}
    eRet createVideo(MString fileName, MString path);

    bool hasIndex() {return _currentVideo->hasIndex();};
    bool isActive(void);
    
    void dump(void);
    void printPids(void) {_currentVideo->_pidMgr.print();};
    
protected:
    NEXUS_FileRecordHandle _file;
    NEXUS_FileRecordHandle _customFile;
    NEXUS_FileRecordHandle _stickyFile;
    NEXUS_RecpumpHandle    _recpump;
    NEXUS_RecordHandle     _record;
    NEXUS_RecordSettings   _recordSettings;
    CParserBand          * _pParserBand;
    
    CVideo               * _currentVideo;
            
};



#ifdef __cplusplus
}
#endif

#endif /* RECORD_H__ */
