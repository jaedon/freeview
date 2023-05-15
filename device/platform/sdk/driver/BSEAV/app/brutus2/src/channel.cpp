/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel.cpp $
* $brcm_Revision: 15 $
* $brcm_Date: 12/5/12 7:02p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel.cpp $
* 
* 15   12/5/12 7:02p tokushig
* SW7231-1086: fix comparison for duplicate channels during scan with
* append option set.  comparison was only comparing pids and ignoring
* freq, bandwidth, and other frontend specific parameters.
* 
* 14   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 13   10/25/12 3:04p tokushig
* SW7231-749: add pid channel debug printout
* 
* 12   10/19/12 5:43p tokushig
* SW7231-749: ensure tuners are properly checked in/out during tune/scan.
* 
* 11   10/16/12 3:47p tokushig
* SW7231-749: fix tuner issue with 97231FBTSFF board
* 
* 10   10/12/12 12:15a tokushig
* SW7231-749: change copy constructor to NOT copy unique resources
* 
* 9   10/11/12 5:27p tokushig
* SW7231-749: fix channel copy constructor and add initialization for
* _pRecord
* 
* 8   9/18/12 5:42p jrubio
* SW7346-1004: add isRecording bool
* 
* 7   9/17/12 10:25a jrubio
* SW7346-1004: channel needs record attributes
* 
* 6   8/27/12 9:54a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 5   5/25/12 12:24a tokushig
* SW7231-749: changed channel dump from WRN to MSG
* 
* 4   4/11/12 1:15p jrubio
* SW7346-644: add CPlayback Class, remove CModelSource Class and Replace
* with CPidMgr, add CPidMgr to base Channel Class
* 
* 3   3/27/12 3:29p tokushig
* SW7405-5581: channel channel dump printout to warning so it will always
* print
* 
* 2   3/19/12 4:25p tokushig
* SW7405-5581: Add support for filtering channel list during scan.
* Ignore encrypted channels, channels with invalid pcr pids, and audio
* only channels.
* 
* 1   3/13/12 4:00p tokushig
* SW7405-5581: more CModelChannel base class out of channelmgr file and
* into channel file
* 
* 
*********************************************************************************/
#include "channel.h"
#include "band.h"
#include "record.h"

#include "nexus_parser_band.h"


BDBG_MODULE(brutus_channel);


CModelChannel::CModelChannel(eBoardResource type) :
    _type(type),
    _major(0),
    _minor(0),
    _programNum(0),
    _pInputBand(NULL),
    _tuned(false),
#if NEXUS_HAS_FRONTEND
    _pTuner(NULL),
#endif
    _pRecord(NULL)
{
}

CModelChannel::CModelChannel(const CModelChannel & ch) :
     CMvcModel(),
    _type(ch._type),
    _major(ch._major),
    _minor(ch._minor),
    _programNum(ch._programNum),
    _pInputBand(NULL),
    _tuned(false),
    _pParserBand(NULL),
    _transportType(ch._transportType),
    _pidMgr(ch._pidMgr),
#if NEXUS_HAS_FRONTEND
    _pTuner(NULL),
#endif
    _pRecord(NULL)
{
}
    

CModelChannel::~CModelChannel()
{
}

#if NEXUS_HAS_FRONTEND
eRet CModelChannel::mapInputBand(CInputBand * pInputBand)
{
    eRet                           ret        = eRet_Ok;
    NEXUS_Error                    nerror     = NEXUS_SUCCESS;
    NEXUS_ParserBandSettings       settings;
    NEXUS_ParserBand               band;
    NEXUS_FrontendUserParameters   userParams;

    BDBG_ASSERT(NULL != _pParserBand);
    BDBG_ASSERT(NULL != pInputBand);

    nerror = NEXUS_Frontend_GetUserParameters(_pTuner->getFrontend(), &userParams);
    CHECK_NEXUS_ERROR_GOTO("unable to get frontend user parameters", ret, nerror, error);

    band = _pParserBand->getBand();
    NEXUS_ParserBand_GetSettings(band, &settings);
    if (true == userParams.isMtsif)
    {
        settings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
        settings.sourceTypeSettings.mtsif = _pTuner->getFrontendConnector();
    }
    else
    {
        settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        settings.sourceTypeSettings.inputBand = pInputBand->getBand();
    }
    BDBG_ASSERT((NEXUS_TransportType_eTs     == _transportType) ||
                (NEXUS_TransportType_eDssEs  == _transportType) ||
                (NEXUS_TransportType_eDssPes == _transportType));
    settings.transportType = _transportType;

    nerror = NEXUS_ParserBand_SetSettings(band, &settings);
    CHECK_NEXUS_ERROR_GOTO("error setting parser band settings", ret, nerror, error);

error:
    return ret;
}
#else
eRet CModelChannel::mapInputBand(CInputBand * pInputBand)
{
    eRet                           ret        = eRet_Ok;
    NEXUS_Error                    nerror     = NEXUS_SUCCESS;
    NEXUS_ParserBandSettings       settings;
    NEXUS_ParserBand               band;

    BDBG_ASSERT(NULL != _pParserBand);
    BDBG_ASSERT(NULL != pInputBand);

    band = _pParserBand->getBand();
    NEXUS_ParserBand_GetSettings(band, &settings);
    settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    settings.sourceTypeSettings.inputBand = pInputBand->getBand();

    BDBG_ASSERT((NEXUS_TransportType_eTs     == _transportType) ||
                (NEXUS_TransportType_eDssEs  == _transportType) ||
                (NEXUS_TransportType_eDssPes == _transportType));
    settings.transportType = _transportType;

    nerror = NEXUS_ParserBand_SetSettings(band, &settings);
    CHECK_NEXUS_ERROR_GOTO("error setting parser band settings", ret, nerror, error);

error:
    return ret;
}
#endif /* NEXUS_HAS_FRONTEND */

void CModelChannel::dump()
{
    CPid * pPid = NULL;

    BDBG_MSG(("channel - type:%d, number:%d.%d, tuned:%d", _type, _major, _minor, _tuned));
    for (int i = 0; NULL != (pPid = _pidMgr.getPid(i, ePidType_Video)); i++)
    {
        BDBG_MSG(("     video pid:0x%2x", pPid->getPid()));
    }
    for (int i = 0; NULL != (pPid =  _pidMgr.getPid(i, ePidType_Audio)); i++)
    {
        BDBG_MSG(("     audio pid:0x%2x", pPid->getPid()));
    }
    for (int i = 0; NULL != (pPid = _pidMgr.getPid(i, ePidType_Ancillary)); i++)
    {
        BDBG_MSG(("     ancillary pid:0x%2x", pPid->getPid()));
    }
    pPid = _pidMgr.getPid(0, ePidType_Pcr);
    if (NULL != pPid)
    {
        BDBG_MSG(("     pcr   pid:0x%2x", pPid->getPid()));
    }
}

eRet CModelChannel::readXML(MXmlElement * xmlElemChannel)
{
    eRet    ret       = eRet_Ok;
    MString strMajor;
    MString strMinor;

    BDBG_ASSERT(NULL != xmlElemChannel);

    strMajor = xmlElemChannel->attrValue(XML_ATT_MAJOR);
    if (false == strMajor.isEmpty())
    {
        setMajor(strMajor.toInt());
    }

    strMinor = xmlElemChannel->attrValue(XML_ATT_MINOR);
    if (false == strMinor.isEmpty())
    {
        setMinor(strMinor.toInt());
    }

    return ret;
}

void CModelChannel::writeXML(MXmlElement * xmlElemChannel)
{
    BDBG_ASSERT(NULL != xmlElemChannel);

    xmlElemChannel->addAttr(XML_ATT_MAJOR, MString(getMajor()));
    xmlElemChannel->addAttr(XML_ATT_MINOR, MString(getMinor()));
}


bool CModelChannel::isRecording(void)
{
    if (_pRecord == NULL || _pRecord->isActive() == false)
    {
        return false;
    } else
        return true;
}

bool CModelChannel::operator==(CModelChannel &other)
{
    /* check base class equivalency first */
    //if (false == CModelSource::operator==(other))
    if (other._pidMgr == _pidMgr)
    {
         if (other.getType() == getType())
         {
             return true;
         }
    }

    return false;
}


/* get PSI channel info for current tuned channel */
eRet CModelChannel::getChannelInfo(CHANNEL_INFO_T * pChanInfo, bool bScanning)
{
    BERR_Code     err            = BERR_SUCCESS;
    CParserBand * pBand          = getParserBand();
    int           patTimeout     = 50; /* in tsPsi_setTimeout2() this is 500msecs */
    int           patTimeoutOrig = 0;
    int           pmtTimeout     = 50; /* in tsPsi_setTimeout2() this is 500msecs */
    int           pmtTimeoutOrig = 0;

    if (true == bScanning)
    {
        /* adjust pat/pmt timeouts for faster scanning */
        tsPsi_getTimeout2(&patTimeoutOrig, &pmtTimeoutOrig);
        tsPsi_setTimeout2(patTimeout, pmtTimeout);
    }

    err = tsPsi_getChannelInfo2(pChanInfo, pBand->getBand());

    if (true == bScanning)
    {
        /* restore default pat/pmt timeouts */
        tsPsi_setTimeout2(patTimeoutOrig, pmtTimeoutOrig);
    }

    return ((BERR_SUCCESS == err) ? eRet_Ok : eRet_ExternalError);
}

/* take given tuned pChannel and search psi data for programs.  call callback with any found channels.
   returns the number of found programs.  this method assumes the major channel number has already been
   set (it will set minor channel numbers).  the given channel will also be updated with the first found
   valid (as determined by callback) psi program info.*/
int CModelChannel::addPsiPrograms(CTunerScanCallback addChannelCallback, void * context)
{
    eRet               ret          = eRet_Ok;
    int                minor        = 1;
    CHANNEL_INFO_T     chanInfo;

    BDBG_ASSERT(NULL != addChannelCallback);
    memset(&chanInfo, 0, sizeof(chanInfo));

    /* get PSI channel info for current tuned channel */
    ret = getChannelInfo(&chanInfo, true);
    CHECK_ERROR_GOTO("PSI channel info retrieval failed, skipping to next channel to be scanned", ret, error);

    /* channel info received */
    /* printinfo(&chanInfo);*/   /* debug */

    /* PSI info retrieved - populate new channel objects and give to callback */
    for (int i = 0; i < chanInfo.num_programs; i++) 
    {
        CModelChannel * pChNew = NULL;

        /* since all these channels share a common frequency, we'll assign minor
           channel numbers here (minor numbers start at 1) */
        initialize(&chanInfo.program_info[i]);
        setMinor(minor);

        /* give new channel to callback */
        if (true == addChannelCallback(this, context))
        {
            minor++;
        }
    }

error:
    return (minor - 1);
}
