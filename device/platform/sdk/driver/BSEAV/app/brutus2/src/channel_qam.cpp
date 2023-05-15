/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel_qam.cpp $
* $brcm_Revision: 17 $
* $brcm_Date: 11/29/12 6:07p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel_qam.cpp $
* 
* 17   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 16   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 15   11/12/12 3:40p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 14   10/22/12 4:46p tokushig
* SW7231-749: modify tuner handling to check tuner status after lock
* callback only.  also change callback handling to only set even in
* nexus callback.
* 
* 13   10/19/12 5:43p tokushig
* SW7231-749: ensure tuners are properly checked in/out during tune/scan.
* 
* 12   10/10/12 5:31p tokushig
* SW7231-749: adjust debug output for release
* 
* 11   8/27/12 9:50a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 10   4/11/12 1:16p jrubio
* SW7346-644: add CPlayback Class, remove CModelSource Class and Replace
* with CPidMgr, add CPidMgr to base Channel Class
* 
* 9   3/12/12 4:42p tokushig
* SW7405-5581: after channel load, unnumbered channels will be auto-
* numbered starting with the last valid channel number in the channel
* list.  channel numbers can not be saved/restored in channel list
* save/load.  moved code to load channels from xml to the appropriate
* channel classes.
* 
* 8   3/9/12 3:45p jrubio
* SW7346-644: add qam write XML
* 
* 7   3/9/12 2:55p tokushig
* SW7405-5581: added support for channel list save.  also added initial
* code to handle tuner lock/unlock after first tune.
* 
* 6   3/7/12 10:09a jrubio
* SW7346-644: add some functions
* 
* 5   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#if NEXUS_HAS_FRONTEND

#include "channel_qam.h"
#include "channelmgr.h"
#include "nexus_core_utils.h"
#include "tuner_qam.h"

BDBG_MODULE(channel_qam);

const char *g_nexusQamModes[NEXUS_FrontendQamMode_eMax] = {	/* index: Nexus Qam Modes */
    "16",
    "32",
    "64",
    "128",
    "256",
    "512",
    "1024",
    "2048",
    "4096",
    "auto"
};

NEXUS_FrontendQamMode CModelChannelQam::stringToNexusQamMode(const char * mode)
{
    int i = 0;
    
    if (!strncmp(mode,"auto",4)) 
       return NEXUS_FrontendQamMode_eAuto_64_256;
    else 
    { 
        i = atoi(mode);

        switch (i) {
        case 16: 
            return NEXUS_FrontendQamMode_e16;
            break;
        case 32: 
            return NEXUS_FrontendQamMode_e32;
            break;
        case 64: 
            return NEXUS_FrontendQamMode_e64;
            break;
        case 128: 
            return NEXUS_FrontendQamMode_e128;
            break;
        case 256: 
            return NEXUS_FrontendQamMode_e256;
            break;
        case 512: 
            return NEXUS_FrontendQamMode_e512;
            break;
        case 1024: 
            return NEXUS_FrontendQamMode_e1024;
            break;
        case 2048: 
            return NEXUS_FrontendQamMode_e2048;
            break;
        case 4096:
            return NEXUS_FrontendQamMode_e4096;
            break;
        default: 
            BDBG_WRN(("Not a valid Mode, set this to MAX"));
            return NEXUS_FrontendQamMode_eMax;
            break;
       }
    }

}

const char * CModelChannelQam::nexusQamModeToString(NEXUS_FrontendQamMode mode)
{
    if (mode >= NEXUS_FrontendQamMode_eMax) 
    {
        BDBG_ERR((" Invalide mode %d ",mode));
        return NULL;
    }

    return g_nexusQamModes[mode];
}

CModelChannelQam::CModelChannelQam(CTunerQam * pTuner) :
    CModelChannel(eBoardResource_frontendQam)
{
#if NEXUS_HAS_FRONTEND
    setTuner(pTuner);
#endif
    NEXUS_Frontend_GetDefaultQamSettings(&_settings);
}

CModelChannelQam::CModelChannelQam(const CModelChannelQam & chQam) :
    CModelChannel(chQam),
    _settings(chQam._settings)
{
    /* copy constructor will not copy any unique objects like tuner */
}

CModelChannel * CModelChannelQam::createCopy(CModelChannel * pChannel) 
{
    CModelChannelQam * pChNew = NULL;

    pChNew = new CModelChannelQam(*(CModelChannelQam *)pChannel);
    return pChNew;
}

eRet CModelChannelQam::readXML(MXmlElement * xmlElemChannel)
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
        BDBG_WRN(("No QAM Mode is not set."));
        _settings.mode = NEXUS_FrontendQamMode_eMax;
        goto error;
    }  

    _settings.mode = stringToNexusQamMode(strMode.s());
    if (_settings.mode == NEXUS_FrontendQamMode_eMax) 
    {
        goto error;
    }

    strBandwidth = xmlElemChannel->attrValue(XML_ATT_BANDWIDTH);
    if (strBandwidth.isEmpty())
    {
        BDBG_MSG(("No Symbol rate defined set to default"));
        _settings.bandwidth = 0;
    } 
    else 
    {
        _settings.bandwidth = strBandwidth.toInt();
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
    
    strAnnex = xmlElemChannel->attrValue(XML_ATT_ANNEX);
    if(strAnnex.isEmpty())
    {
        BDBG_MSG(("Annex is not defined, Default to Annex B"));
        _settings.annex = NEXUS_FrontendQamAnnex_eB;
    }
    else
    { 
        /* set to Default Value First */
        _settings.annex = NEXUS_FrontendQamAnnex_eB;
        if (strAnnex == "A")
            _settings.annex = NEXUS_FrontendQamAnnex_eA;
        else
        if (strAnnex == "B")
            _settings.annex = NEXUS_FrontendQamAnnex_eB;
        else
        if (strAnnex == "C")
            _settings.annex = NEXUS_FrontendQamAnnex_eC;

        BDBG_MSG(("Annex is set to %s", strAnnex.s()));
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

void CModelChannelQam::writeXML(MXmlElement * xmlElemChannel)
{
    BDBG_MSG((" Write QAM channel"));
    CModelChannel::writeXML(xmlElemChannel);
    
    xmlElemChannel->addAttr(XML_ATT_TYPE,"qam");
    xmlElemChannel->addAttr(XML_ATT_FREQ, MString(_settings.frequency));
    xmlElemChannel->addAttr(XML_ATT_MODE, nexusQamModeToString(_settings.mode));
    xmlElemChannel->addAttr(XML_ATT_BANDWIDTH, MString(_settings.bandwidth));
    xmlElemChannel->addAttr(XML_ATT_SYMRATE, MString(_settings.symbolRate));

    switch(_settings.annex) 
    {
        case 0:
            xmlElemChannel->addAttr(XML_ATT_ANNEX,"A");
            break;
        case 2:
            xmlElemChannel->addAttr(XML_ATT_ANNEX,"C");
            break;
        case 1:
        default:
            xmlElemChannel->addAttr(XML_ATT_ANNEX,"B");
            break;
    }

    _pidMgr.writePidXML(xmlElemChannel);
}

CModelChannelQam::~CModelChannelQam()
{
}

eRet CModelChannelQam::initialize(PROGRAM_INFO_T * pProgramInfo)
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

uint32_t CModelChannelQam::mode2SymbolRate(NEXUS_FrontendQamMode mode)
{
    uint32_t symbolRate = 0;

    switch (mode) {
        default:
        case NEXUS_FrontendQamMode_e64: 
            symbolRate = 5056900; 
            break;
        case NEXUS_FrontendQamMode_e256: 
            symbolRate = 5360537; 
            break;
        case NEXUS_FrontendQamMode_e1024: 
            symbolRate = 0; 
            break;
    }

    return symbolRate;
}

eRet CModelChannelQam::tune(void * id, CModelConfig * pModelConfig, bool bWaitForLock)
{
    eRet                           ret             = eRet_Ok;
    NEXUS_Error                    nerror          = NEXUS_SUCCESS;
    NEXUS_FrontendUserParameters   userParams;
    CInputBand                   * pInputBand      = NULL;
    CParserBand                  * pParserBand     = NULL;
    CTunerQam                    * pTuner          = NULL;
    CBoardResources              * pBoardResources = NULL;
    CConfiguration               * pCfg            = NULL;

    BDBG_ASSERT(NULL != pModelConfig);
    pBoardResources = pModelConfig->getBoardResources();
    pCfg            = pModelConfig->getCfg();

    pTuner = (CTunerQam *)getTuner();
    if (NULL == pTuner)
    {
        pTuner = (CTunerQam *)pBoardResources->checkoutResource(id, eBoardResource_frontendQam);
        CHECK_PTR_GOTO("unable to checkout QAM tuner", pTuner, ret, eRet_NotAvailable, done);
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
    BDBG_MSG(("mode:%d annex:%d freq:%u iffreq:%u symbolRate:%u fastAcq:%d terr:%d",
              _settings.mode, _settings.annex, _settings.frequency, _settings.ifFrequency, _settings.symbolRate, _settings.fastAcquisition, _settings.terrestrial));

    _settings.symbolRate = mode2SymbolRate(_settings.mode);

    ret = pTuner->tune(_settings);
    CHECK_ERROR_GOTO("tuning error!", ret, error);

    _tuned = true;

    if (true == bWaitForLock)
    {
        B_Error                  berror     = B_ERROR_SUCCESS;
        int                      timeout    = GET_INT(pCfg, TUNE_QAM_TIMEOUT);
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
        pParserBand = NULL;
    }
    if (NULL != pInputBand)
    {
        pBoardResources->checkinResource(pInputBand);
        pInputBand = NULL;
    }

    _tuned = false;

done:
    return ret;
}

eRet CModelChannelQam::unTune(CModelConfig * pModelConfig, bool bCheckInTuner)
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


bool CModelChannelQam::operator==(CModelChannel &other)
{
    CModelChannelQam * pOtherQam = (CModelChannelQam *)&other;

    /* check base class equivalency first */
    if (true == CModelChannel::operator==(other))
    {
         if ((pOtherQam->getFrequency() == getFrequency()) &&
             (pOtherQam->getMode()      == getMode())      &&
             (pOtherQam->getAnnex()     == getAnnex()))
         {
             return true;
         }
    }

    return false;
}

#endif /* NEXUS_HAS_FRONTEND */
