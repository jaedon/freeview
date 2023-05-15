/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel_ofdm.cpp $
* $brcm_Revision: 14 $
* $brcm_Date: 12/5/12 7:03p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel_ofdm.cpp $
* 
* 14   12/5/12 7:03p tokushig
* SW7231-1086: fix comparison for duplicate channels during scan with
* append option set.  comparison was only comparing pids and ignoring
* freq, bandwidth, and other frontend specific parameters.
* 
* 13   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 12   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 11   11/12/12 3:41p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 10   11/7/12 5:54p jrubio
* SW7231-1060: remove mapInputBand Fuction, add handling for dvbt,dvbt2
* 
* 9   10/22/12 4:46p tokushig
* SW7231-749: modify tuner handling to check tuner status after lock
* callback only.  also change callback handling to only set even in
* nexus callback.
* 
* 8   10/19/12 5:44p tokushig
* SW7231-749: ensure tuners are properly checked in/out during tune/scan.
* 
* 7   10/10/12 8:25p tokushig
* SW7231-749: update mtsif setting
* 
* 6   8/27/12 9:53a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 6   8/27/12 9:50a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 5   7/31/12 5:29a jrubio
* SW7346-848: fix typo
* 
* 4   7/30/12 5:06p jrubio
* SW7346-848: ofdm channel read needs to be in MHz not Hz
* 
* 3   7/24/12 5:11p jrubio
* SW7346-848: add ofdm cabalities, new parserband function
* 
* 2   7/24/12 3:39p tokushig
* SW7231-749: fix ofdm channel creation
* 
* 1   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 
*********************************************************************************/

#if NEXUS_HAS_FRONTEND

#include "channel_ofdm.h"
#include "channelmgr.h"
#include "nexus_core_utils.h"
#include "tuner_ofdm.h"

#include "band.h"

#include "nexus_parser_band.h"

BDBG_MODULE(channel_ofdm);

const char *g_nexusOfdmModes[NEXUS_FrontendOfdmMode_eMax] = {	/* index: Nexus Ofdm Modes */
    "dvbt",
    "dvbt2",
    "dvbc2",
    "isdbt"
};

NEXUS_FrontendOfdmMode CModelChannelOfdm::stringToNexusOfdmMode(const char * mode)
{
    int i = 0;
    
    if (!strncmp(mode, g_nexusOfdmModes[NEXUS_FrontendOfdmMode_eDvbt], 4)) 
    {
       return NEXUS_FrontendOfdmMode_eDvbt;
    }
    else 
    if (!strncmp(mode, g_nexusOfdmModes[NEXUS_FrontendOfdmMode_eDvbt2], 5)) 
    {
       return NEXUS_FrontendOfdmMode_eDvbt2;
    }
    else 
    if (!strncmp(mode, g_nexusOfdmModes[NEXUS_FrontendOfdmMode_eIsdbt], 5)) 
    {
       return NEXUS_FrontendOfdmMode_eIsdbt;
    } else
    if (!strncmp(mode, g_nexusOfdmModes[NEXUS_FrontendOfdmMode_eDvbc2], 5))
    {
        return NEXUS_FrontendOfdmMode_eDvbc2;
    }


    BDBG_WRN(("Not a valid Mode, set this to MAX"));
    return NEXUS_FrontendOfdmMode_eMax;
}


const char * CModelChannelOfdm::nexusOfdmModeToString(NEXUS_FrontendOfdmMode mode)
{
    if (mode >= NEXUS_FrontendOfdmMode_eMax) 
    {
        BDBG_ERR((" Invalide mode %d ",mode));
        return NULL;
    }

    return g_nexusOfdmModes[mode];
}

CModelChannelOfdm::CModelChannelOfdm(CTunerOfdm * pTuner) :
    CModelChannel(eBoardResource_frontendOfdm)
{
#if NEXUS_HAS_FRONTEND
    setTuner(pTuner);
#endif
    NEXUS_Frontend_GetDefaultOfdmSettings(&_settings);
}

CModelChannelOfdm::CModelChannelOfdm(const CModelChannelOfdm & chOfdm) :
    CModelChannel(chOfdm),
    _settings(chOfdm._settings)
{
    /* copy constructor will not copy any unique objects like tuner */
}

CModelChannel * CModelChannelOfdm::createCopy(CModelChannel * pChannel) 
{
    CModelChannelOfdm * pChNew = NULL;

    pChNew = new CModelChannelOfdm(*(CModelChannelOfdm *)pChannel);
    return pChNew;
}

eRet CModelChannelOfdm::readXML(MXmlElement * xmlElemChannel)
{
    eRet    ret           = eRet_Ok;
    MString strBandwidth;
    MString strFreq;

    CModelChannel::readXML(xmlElemChannel);

    setTransportType(NEXUS_TransportType_eTs);
    
    NEXUS_Frontend_GetDefaultOfdmSettings(&_settings);    
    MString strMode = xmlElemChannel->attrValue(XML_ATT_MODE);
    if (strMode.isEmpty())
    {
        BDBG_WRN(("No Ofdm Mode is not set."));
        _settings.mode = NEXUS_FrontendOfdmMode_eMax;
        goto error;
    }  

    _settings.mode = stringToNexusOfdmMode(strMode.s());
    if (_settings.mode == NEXUS_FrontendOfdmMode_eMax) 
    {
        goto error;
    }

    strBandwidth = xmlElemChannel->attrValue(XML_ATT_BANDWIDTH);
    if (strBandwidth.isEmpty())
    {
        BDBG_MSG(("No bandwidth defined set to default"));
        _settings.bandwidth = 6000000;
    } 
    else 
    {
        _settings.bandwidth = strBandwidth.toInt()*1000000;
    }

    strFreq = xmlElemChannel->attrValue(XML_ATT_FREQ);
    if(strFreq.isEmpty())
    {
        BDBG_MSG(("Freq is not set, default to 549Mhz"));
        _settings.frequency = 549 * 1000000;
    } 
    else
    {
        _settings.frequency =  strFreq.toInt() * 1000000;
        BDBG_MSG(("Freq is successfully set to %d",_settings.frequency));
    }

    _pidMgr.readPidXML(xmlElemChannel);


error:
    return ret;
}

void CModelChannelOfdm::writeXML(MXmlElement * xmlElemChannel)
{
    BDBG_MSG((" Write Ofdm channel"));
    CModelChannel::writeXML(xmlElemChannel);
    
    xmlElemChannel->addAttr(XML_ATT_TYPE,"ofdm");
    xmlElemChannel->addAttr(XML_ATT_FREQ, MString(_settings.frequency/1000000));
    xmlElemChannel->addAttr(XML_ATT_MODE, nexusOfdmModeToString(_settings.mode));
    xmlElemChannel->addAttr(XML_ATT_BANDWIDTH, MString(_settings.bandwidth/1000000));

    _pidMgr.writePidXML(xmlElemChannel);
}

CModelChannelOfdm::~CModelChannelOfdm()
{
}

eRet CModelChannelOfdm::initialize(PROGRAM_INFO_T * pProgramInfo)
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


eRet CModelChannelOfdm::tune(void * id, CModelConfig * pModelConfig, bool bWaitForLock)
{
    eRet                           ret             = eRet_Ok;
    NEXUS_Error                    nerror          = NEXUS_SUCCESS;
    NEXUS_FrontendUserParameters   userParams;
    CInputBand                   * pInputBand      = NULL;
    CParserBand                  * pParserBand     = NULL;
    CTunerOfdm                   * pTuner          = NULL;
    CBoardResources              * pBoardResources = NULL;
    CConfiguration               * pCfg            = NULL;
    NEXUS_FrontendOfdmSettings     newSettings;

    BDBG_ASSERT(NULL != pModelConfig);
    pBoardResources = pModelConfig->getBoardResources();
    pCfg            = pModelConfig->getCfg();

    pTuner = (CTunerOfdm *)getTuner();
    if (NULL == pTuner)
    {
        pTuner = (CTunerOfdm *)pBoardResources->checkoutResource(id, eBoardResource_frontendOfdm);
        CHECK_PTR_GOTO("unable to checkout Ofdm tuner", pTuner, ret, eRet_NotAvailable, done);
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

    /* Default Settings to reset a couple of fields if OFDM Changes */
    NEXUS_Frontend_GetDefaultOfdmSettings(&newSettings);

    /* These should be set for all modes */
    _settings.acquisitionMode = NEXUS_FrontendOfdmAcquisitionMode_eAuto;
    _settings.terrestrial = true;
    _settings.spectrum = NEXUS_FrontendOfdmSpectrum_eAuto;

    if(_settings.mode == NEXUS_FrontendOfdmMode_eDvbt){
        _settings.manualTpsSettings = false;
        _settings.pullInRange = NEXUS_FrontendOfdmPullInRange_eWide;
        _settings.cciMode = NEXUS_FrontendOfdmCciMode_eNone;
        _settings.dvbt2Settings.plpMode = newSettings.dvbt2Settings.plpMode;
        _settings.dvbt2Settings.plpId = newSettings.dvbt2Settings.plpId;     
    }
    else if(_settings.mode == NEXUS_FrontendOfdmMode_eDvbt2){
        _settings.dvbt2Settings.plpMode = false;
        _settings.dvbt2Settings.plpId = 0;     
        _settings.manualTpsSettings = newSettings.manualTpsSettings;
        _settings.pullInRange = newSettings.pullInRange;
        _settings.cciMode = newSettings.cciMode;
    }
    else if(_settings.mode == NEXUS_FrontendOfdmMode_eIsdbt){

       _settings.manualTpsSettings = newSettings.manualTpsSettings;
       _settings.pullInRange = newSettings.pullInRange;
       _settings.cciMode = newSettings.cciMode;
       _settings.dvbt2Settings.plpMode = newSettings.dvbt2Settings.plpMode;
       _settings.dvbt2Settings.plpId = newSettings.dvbt2Settings.plpId;     

    }

    BDBG_MSG(("CH NUMBER: %d.%d", getMajor(), getMinor()));
    BDBG_MSG(("mode:%d freq:%u bandwidth:%d",
              _settings.mode, _settings.frequency, _settings.bandwidth));

    ret = pTuner->tune(_settings);
    CHECK_ERROR_GOTO("tuning error!", ret, error);

    _tuned = true;
    
    if (true == bWaitForLock)
    {
        B_Error                  berror     = B_ERROR_SUCCESS;
        int                      timeout    = GET_INT(pCfg, TUNE_OFDM_TIMEOUT);
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

eRet CModelChannelOfdm::unTune(CModelConfig * pModelConfig, bool bCheckInTuner)
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


bool CModelChannelOfdm::operator==(CModelChannel &other)
{
    CModelChannelOfdm * pOtherOfdm = (CModelChannelOfdm *)&other;

    /* check base class equivalency first */
    if (true == CModelChannel::operator==(*pOtherOfdm))
    {
         if ((pOtherOfdm->getFrequency() == getFrequency()) &&
             (pOtherOfdm->getMode()      == getMode()))
         {
             return true;
         }
    }

    return false;
}

#endif /* NEXUS_HAS_FRONTEND */
