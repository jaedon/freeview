/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel_vsb.cpp $
* $brcm_Revision: 9 $
* $brcm_Date: 12/5/12 7:03p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel_vsb.cpp $
* 
* 9   12/5/12 7:03p tokushig
* SW7231-1086: fix comparison for duplicate channels during scan with
* append option set.  comparison was only comparing pids and ignoring
* freq, bandwidth, and other frontend specific parameters.
* 
* 8   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 7   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 6   11/12/12 3:41p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 5   10/22/12 4:46p tokushig
* SW7231-749: modify tuner handling to check tuner status after lock
* callback only.  also change callback handling to only set even in
* nexus callback.
* 
* 3   8/27/12 9:50a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 2   4/11/12 1:16p jrubio
* SW7346-644: add CPlayback Class, remove CModelSource Class and Replace
* with CPidMgr, add CPidMgr to base Channel Class
* 
* 1   3/27/12 10:31a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 
*********************************************************************************/

#if NEXUS_HAS_FRONTEND

#include "channel_vsb.h"
#include "channelmgr.h"
#include "nexus_core_utils.h"
#include "tuner_vsb.h"

BDBG_MODULE(channel_vsb);

const char *g_nexusVsbModes[NEXUS_FrontendVsbMode_eMax] = {	/* index: Nexus Vsb Modes */
    "8",
    "16"
};

NEXUS_FrontendVsbMode CModelChannelVsb::stringToNexusVsbMode(const char * mode)
{
    int i = 0;
    
    i = atoi(mode);

    switch (i) {
    case 8: 
        return NEXUS_FrontendVsbMode_e8;
        break;
    case 16: 
        return NEXUS_FrontendVsbMode_e16;
        break;
    default: 
        BDBG_WRN(("Not a valid Mode, set this to MAX"));
        return NEXUS_FrontendVsbMode_eMax;
        break;
   }
}

const char * CModelChannelVsb::nexusVsbModeToString(NEXUS_FrontendVsbMode mode)
{
    if (mode >= NEXUS_FrontendVsbMode_eMax) 
    {
        BDBG_ERR((" Invalid mode %d ",mode));
        return NULL;
    }

    return g_nexusVsbModes[mode];
}

CModelChannelVsb::CModelChannelVsb(CTunerVsb * pTuner) :
    CModelChannel(eBoardResource_frontendVsb)
{
#if NEXUS_HAS_FRONTEND
    setTuner(pTuner);
#endif
    NEXUS_Frontend_GetDefaultVsbSettings(&_settings);
}

CModelChannelVsb::CModelChannelVsb(const CModelChannelVsb & chVsb) :
    CModelChannel(chVsb),
    _settings(chVsb._settings)
{
    /* copy constructor will not copy any unique objects like tuner */
}

CModelChannel * CModelChannelVsb::createCopy(CModelChannel * pChannel) 
{
    CModelChannelVsb * pChNew = NULL;

    pChNew = new CModelChannelVsb(*(CModelChannelVsb *)pChannel);
    return pChNew;
}

eRet CModelChannelVsb::readXML(MXmlElement * xmlElemChannel)
{
    eRet    ret           = eRet_Ok;
    MString strBandwidth;
    MString strSymRate;
    MString strAnnex;
    MString strFreq;

    CModelChannel::readXML(xmlElemChannel);

    setTransportType(NEXUS_TransportType_eTs);
    
    MString strMode = xmlElemChannel->attrValue(XML_ATT_MODE);
    if (strMode.isEmpty())
    {
        BDBG_WRN(("No Vsb Mode is not set."));
        _settings.mode = NEXUS_FrontendVsbMode_eMax;
        goto error;
    }  

    _settings.mode = stringToNexusVsbMode(strMode.s());
    if (_settings.mode == NEXUS_FrontendVsbMode_eMax) 
    {
        goto error;
    }

    strSymRate = xmlElemChannel->attrValue(XML_ATT_SYMRATE);
    if (strSymRate.isEmpty())
    {
        BDBG_MSG(("No Symbol rate defined set to default"));
        _settings.symbolRate = 0;
    } 
    else 
    {
        _settings.symbolRate = strSymRate.toInt();
    }
    
    strFreq = xmlElemChannel->attrValue(XML_ATT_FREQ);
    if(strFreq.isEmpty())
    {
        BDBG_MSG(("Freq is not set, default to 549Mhz"));
        _settings.frequency = 549 * 1000000;
    } 
    else
    {
        _settings.frequency =  strFreq.toInt();
        BDBG_MSG(("Freq is successfully set to %d",_settings.frequency));
    }

    _pidMgr.readPidXML(xmlElemChannel);

error:
    return ret;
}

void CModelChannelVsb::writeXML(MXmlElement * xmlElemChannel)
{
    BDBG_MSG((" Write Vsb channel"));
    CModelChannel::writeXML(xmlElemChannel);
    
    xmlElemChannel->addAttr(XML_ATT_TYPE,"vsb");
    xmlElemChannel->addAttr(XML_ATT_FREQ, MString(_settings.frequency));
    xmlElemChannel->addAttr(XML_ATT_MODE, nexusVsbModeToString(_settings.mode));
    xmlElemChannel->addAttr(XML_ATT_SYMRATE, MString(_settings.symbolRate));

    _pidMgr.writePidXML(xmlElemChannel);
}

CModelChannelVsb::~CModelChannelVsb()
{
}

eRet CModelChannelVsb::initialize(PROGRAM_INFO_T * pProgramInfo)
{
    eRet ret = eRet_Ok;

    BDBG_ASSERT(NULL != pProgramInfo);

    setProgramNum(pProgramInfo->program_number);
    _pidMgr.clearPids();
    ret = _pidMgr.createPids(pProgramInfo);
    CHECK_ERROR_GOTO("unable to create pids from program info", ret, error);

error:
    return ret;
}

eRet CModelChannelVsb::tune(void * id, CModelConfig * pModelConfig, bool bWaitForLock)
{
    eRet                           ret             = eRet_Ok;
    NEXUS_Error                    nerror          = NEXUS_SUCCESS;
    NEXUS_FrontendUserParameters   userParams;
    CInputBand                   * pInputBand      = NULL;
    CParserBand                  * pParserBand     = NULL;
    CTunerVsb                    * pTuner          = NULL;
    CBoardResources              * pBoardResources = NULL;
    CConfiguration               * pCfg            = NULL;

    BDBG_ASSERT(NULL != pModelConfig);
    pBoardResources = pModelConfig->getBoardResources();
    pCfg            = pModelConfig->getCfg();

    pTuner = (CTunerVsb *)getTuner();
    if (NULL == pTuner)
    {
        pTuner = (CTunerVsb *)pBoardResources->checkoutResource(id, eBoardResource_frontendVsb);
        CHECK_PTR_GOTO("unable to checkout Vsb tuner", pTuner, ret, eRet_NotAvailable, done);
        pTuner->setModelConfig(pModelConfig);
        setTuner(pTuner);
    }

    nerror = NEXUS_Frontend_GetUserParameters(_pTuner->getFrontend(), &userParams);
    CHECK_NEXUS_ERROR_GOTO("unable to get frontend user parameters", ret, nerror, done);

    /* checkout input band based on frontend user parameters (set by nexus platform) */
    pInputBand = (CInputBand *)pBoardResources->checkoutResource(id, eBoardResource_inputBand, userParams.param1);
    CHECK_PTR_GOTO("unable to checkout inputband", pInputBand, ret, eRet_NotAvailable, error);
    setInputBand(pInputBand);

    pParserBand = (CParserBand *)pBoardResources->checkoutResource(id, eBoardResource_parserBand);
    CHECK_PTR_GOTO("unable to checkout parser band", pParserBand, ret, eRet_NotAvailable, error);
    setParserBand(pParserBand);

    ret = mapInputBand(pInputBand);
    CHECK_ERROR_GOTO("error mapping input band to parser band", ret, error);

    BDBG_MSG(("CH NUMBER: %d.%d", getMajor(), getMinor()));
    BDBG_MSG(("mode:%d freq:%u iffreq:%u symbolRate:%u terr:%d",
              _settings.mode, _settings.frequency, _settings.ifFrequency, _settings.symbolRate, _settings.terrestrial));

    ret = pTuner->tune(_settings);
    CHECK_ERROR_GOTO("tuning error!", ret, error);

    _tuned = true;

    if (true == bWaitForLock)
    {
        B_Error                  berror     = B_ERROR_SUCCESS;
        int                      timeout    = GET_INT(pCfg, TUNE_VSB_TIMEOUT);
        NEXUS_FrontendLockStatus lockStatus = NEXUS_FrontendLockStatus_eUnknown;

        berror = B_Event_Wait(_pTuner->getWaitEvent(), timeout);
        CHECK_BOS_ERROR_GOTO("tune failed to lock", ret, berror, error);

        if ((false == pTuner->getLockState()) && (false == pTuner->isAcquireInProgress()))
        {
            /* wait event could have been set if unlocked but tuner is done searching
               so we will indicate a failed tune in the return code */
            ret = eRet_NotAvailable;
        }
    }
    goto done;

error:
    if (NULL != pTuner)
    {
        pBoardResources->checkinResource(pTuner);
        setTuner(NULL);
    }

    if (NULL != pParserBand)
    {
        pBoardResources->checkinResource(pParserBand);
    }
    if (NULL != pInputBand)
    {
        pBoardResources->checkinResource(pInputBand);
    }

    _tuned = false;

done:
    return ret;
}

eRet CModelChannelVsb::unTune(CModelConfig * pModelConfig, bool bCheckInTuner)
{
    eRet              ret             = eRet_Ok;
    CBoardResources * pBoardResources = NULL;
    BDBG_ASSERT(pModelConfig);
    pBoardResources = pModelConfig->getBoardResources();
    
    if ((NULL != _pParserBand) && (_pParserBand->isCheckedOut()))
    {
        pBoardResources->checkinResource(_pParserBand);
        setParserBand(NULL);
    }

    if ((NULL != _pInputBand) && (_pInputBand->isCheckedOut()))
    {
        pBoardResources->checkinResource(_pInputBand);
        setInputBand(NULL);
    }

    if ((NULL != _pTuner) && (_pTuner->isCheckedOut()))
    {
        CTuner * pTuner = getTuner();
        pTuner->unTune();

        if (true == bCheckInTuner)
        {
            pBoardResources->checkinResource(pTuner);
            setTuner(NULL);
        }
    }

    _tuned = false;

    return ret;
}


bool CModelChannelVsb::operator==(CModelChannel &other)
{
    CModelChannelVsb * pOtherVsb = (CModelChannelVsb *)&other;

    /* check base class equivalency first */
    if (true == CModelChannel::operator==(*pOtherVsb))
    {
         if ((pOtherVsb->getFrequency() == getFrequency()) &&
             (pOtherVsb->getMode()      == getMode()))
         {
             return true;
         }
    }

    return false;
}

#endif /* NEXUS_HAS_FRONTEND */
