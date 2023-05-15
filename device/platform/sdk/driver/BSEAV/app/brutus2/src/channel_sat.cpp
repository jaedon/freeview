/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel_sat.cpp $
* $brcm_Revision: 14 $
* $brcm_Date: 12/5/12 7:02p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel_sat.cpp $
* 
* 14   12/5/12 7:02p tokushig
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
* 11   11/12/12 3:40p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 10   10/22/12 4:46p tokushig
* SW7231-749: modify tuner handling to check tuner status after lock
* callback only.  also change callback handling to only set even in
* nexus callback.
* 
* 8   8/27/12 9:50a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 7   4/11/12 1:16p jrubio
* SW7346-644: add CPlayback Class, remove CModelSource Class and Replace
* with CPidMgr, add CPidMgr to base Channel Class
* 
*
* 
*********************************************************************************/

#if NEXUS_HAS_FRONTEND 

#include "channel_sat.h"
#include "channelmgr.h"
#include "nexus_core_utils.h"
#include "tuner_sat.h"

BDBG_MODULE(channel_sat);

const char *g_nexusSatModes[NEXUS_FrontendSatelliteMode_eMax] = {	/* index: Nexus Satellite Modes */
    "dvb",
    "dss",
    "dcii",
    "qpskturbo",
    "8pskturbo",
    "turbo",
    "8pskldpc",
    "qpskldpc",
    "ldpc",
    "blind"
};

NEXUS_FrontendSatelliteMode CModelChannelSat::stringToNexusSatMode(const char * mode)
{
    /* Helper Tuner functions */
      if (!strncmp(mode,"",1)) {
        BDBG_WRN(("No Satellite Mode set"));
        return NEXUS_FrontendSatelliteMode_eMax;
    }
    if (!strncmp(mode,"dvb",3))
        return NEXUS_FrontendSatelliteMode_eDvb;
    if (!strncmp(mode,"dss",3))
        return NEXUS_FrontendSatelliteMode_eDss;
    if (!strncmp(mode,"dcii",4))
        return NEXUS_FrontendSatelliteMode_eDcii;
    if (!strncmp(mode,"qpskturbo",9))
        return NEXUS_FrontendSatelliteMode_eQpskTurbo;
    if (!strncmp(mode,"8pskturbo",9))
        return NEXUS_FrontendSatelliteMode_e8pskTurbo;
    if (!strncmp(mode,"turbo",5))
        return NEXUS_FrontendSatelliteMode_eTurbo;
    if (!strncmp(mode,"8pskldpc",8))
        return NEXUS_FrontendSatelliteMode_e8pskLdpc;
    if (!strncmp(mode,"qpskldpc",8))
        return NEXUS_FrontendSatelliteMode_eQpskLdpc;
    if (!strncmp(mode,"ldpc",4))
        return NEXUS_FrontendSatelliteMode_eLdpc;
    if (!strncmp(mode,"blind",5))
        return NEXUS_FrontendSatelliteMode_eBlindAcquisition;

    BDBG_ERR(("Cannot find Satellite Mode %s", mode));
    return NEXUS_FrontendSatelliteMode_eMax;
}

const char * CModelChannelSat::nexusSatModeToString(NEXUS_FrontendSatelliteMode mode)
{
    if (mode >= NEXUS_FrontendSatelliteMode_eMax) 
        return NULL;

    return g_nexusSatModes[(int)mode];
}

CModelChannelSat::CModelChannelSat(CTunerSat * pTuner) :
    CModelChannel(eBoardResource_frontendSds)
{
#if NEXUS_HAS_FRONTEND
    setTuner(pTuner);
#endif
    NEXUS_Frontend_GetDefaultSatelliteSettings(&_settings.sat);
}

CModelChannelSat::~CModelChannelSat()
{
}

CModelChannelSat::CModelChannelSat(const CModelChannelSat & chSat) :
    CModelChannel(chSat),
    _settings(chSat._settings)
{
    /* copy constructor will not copy any unique objects like tuner */
}

CModelChannel * CModelChannelSat::createCopy(CModelChannel * pChannel) 
{
    CModelChannelSat * pChNew = NULL;

    pChNew = new CModelChannelSat(*(CModelChannelSat *)pChannel);
    return pChNew;
}

eRet CModelChannelSat::readXML(MXmlElement * xmlElemChannel)
{
    eRet    ret         = eRet_Ok;
    MString strMode;
    MString strSymRate;
    MString strDiseqc;
    MString strFreq;
    MString strTone;

    CModelChannel::readXML(xmlElemChannel);

    setTransportType(NEXUS_TransportType_eTs);
    
    strMode = xmlElemChannel->attrValue(XML_ATT_MODE);
    _settings.sat.mode = stringToNexusSatMode(strMode.s());
    if (_settings.sat.mode == NEXUS_FrontendSatelliteMode_eMax) 
        goto error;

    BDBG_MSG(("Mode is %d ", _settings.sat.mode));
    
    strSymRate = xmlElemChannel->attrValue(XML_ATT_SYMRATE);
    if (strSymRate.isEmpty())
    {
        BDBG_MSG(("No SymRate is set, Default to 20M"));
        _settings.sat.symbolRate = 20000000;
    } 
    else 
    {
        _settings.sat.symbolRate = strSymRate.toInt()*1000000;
    }
    
    strDiseqc = xmlElemChannel->attrValue(XML_ATT_DISEQC);
    if(strDiseqc.isEmpty())
    {
        BDBG_MSG(("Diseqc is not set, default to 13V"));
        _settings.diseqc.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
    } 
    else 
    {
        switch(strDiseqc.toInt()) {
            case 13:
                _settings.diseqc.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
                break;
            case 18:
                _settings.diseqc.voltage = NEXUS_FrontendDiseqcVoltage_e18v;
                break;
            default:
                 _settings.diseqc.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
                 BDBG_ERR(("Not Supported Voltage %d, set to Default 13V",strDiseqc.toInt()));
                 break;
        }
        BDBG_MSG(("Diseqc Voltage set to %dV",strDiseqc.toInt()));
    } 

    strFreq = xmlElemChannel->attrValue(XML_ATT_FREQ);
    if(strFreq.isEmpty())
    {
        BDBG_ERR(("Freq is not set, This channel will Fail"));
        goto error;
    } 
    else
    {
        _settings.sat.frequency =  strFreq.toFloat() * 1000000;
        BDBG_MSG(("Freq is successfully set to %d", _settings.sat.frequency));
    }

    strTone = xmlElemChannel->attrValue(XML_ATT_TONE);
    if(strTone == "true")
        _settings.diseqc.toneEnabled = true;
    else if (strTone =="false") 
        _settings.diseqc.toneEnabled= false;
    else 
    {
        BDBG_ERR(("Tone is not a correct value: %s",strTone.s()));
        goto error;
    }

    /* Get and Add Pids */
    _pidMgr.readPidXML(xmlElemChannel);

    return ret;

error:
    BDBG_ERR(("Cannot add Satellite Channel"));
    return eRet_InvalidParameter;
}

void CModelChannelSat::writeXML(MXmlElement * xmlElemChannel)
{
    BDBG_MSG((" Add SDS channel"));

    CModelChannel::writeXML(xmlElemChannel);

    xmlElemChannel->addAttr(XML_ATT_TYPE,"sds");
    xmlElemChannel->addAttr(XML_ATT_FREQ, MString(_settings.sat.frequency/1000000));
    xmlElemChannel->addAttr(XML_ATT_MODE, nexusSatModeToString(_settings.sat.mode));
    xmlElemChannel->addAttr(XML_ATT_SYMRATE, MString(_settings.sat.symbolRate/1000000));

    switch(_settings.diseqc.voltage) 
    {
        case 0:
            xmlElemChannel->addAttr(XML_ATT_DISEQC,"13");
            break;
        default:
            xmlElemChannel->addAttr(XML_ATT_DISEQC,"18");
            break;
    }

    if (_settings.diseqc.toneEnabled) 
        xmlElemChannel->addAttr(XML_ATT_TONE,"true");
    else
        xmlElemChannel->addAttr(XML_ATT_TONE,"false");

    _pidMgr.writePidXML(xmlElemChannel);
}

eRet CModelChannelSat::initialize(PROGRAM_INFO_T * pProgramInfo)
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

eRet CModelChannelSat::tune(void * id, CModelConfig * pModelConfig, bool bWaitForLock)
{
    eRet                           ret             = eRet_Ok;
    NEXUS_Error                    nerror          = NEXUS_SUCCESS;
    NEXUS_FrontendUserParameters   userParams;
    CInputBand                   * pInputBand      = NULL;
    CParserBand                  * pParserBand     = NULL;
    CTunerSat                    * pTuner          = NULL;
    CBoardResources              * pBoardResources = NULL;
    CConfiguration               * pCfg            = NULL;
    NEXUS_FrontendDiseqcSettings diseqcSettings;


    BDBG_ASSERT(NULL != pModelConfig);
    pBoardResources = pModelConfig->getBoardResources();
    pCfg            = pModelConfig->getCfg();

    pTuner = (CTunerSat *)getTuner();
    if (NULL == pTuner)
    {
        pTuner = (CTunerSat *)pBoardResources->checkoutResource(id, eBoardResource_frontendSds);
        CHECK_PTR_GOTO("unable to checkout SAT tuner", pTuner, ret, eRet_NotAvailable, done);
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

    diseqcSettings.toneEnabled  = _settings.diseqc.toneEnabled;
    diseqcSettings.voltage  = _settings.diseqc.voltage;

    NEXUS_Frontend_GetDiseqcSettings(pTuner->getFrontend(), &_settings.diseqc);
    _settings.diseqc.toneEnabled = diseqcSettings.toneEnabled;
    _settings.diseqc.voltage = diseqcSettings.voltage;

    BDBG_MSG(("CH NUMBER: %d.%d", getMajor(), getMinor()));
    BDBG_MSG(("mode:%d freq:%u  symbolRate:%u voltage:%d toneEnabled:%s",
              _settings.sat.mode,  _settings.sat.frequency,  _settings.sat.symbolRate, _settings.diseqc.voltage,(_settings.diseqc.toneEnabled)?"yes":"no" ));

    ret = pTuner->tune(_settings);
    CHECK_ERROR_GOTO("tuning error!", ret, error);

    _tuned = true;

    if (true == bWaitForLock)
    {
        B_Error                  berror     = B_ERROR_SUCCESS;
        int                      timeout    = GET_INT(pCfg, TUNE_SAT_TIMEOUT);
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

eRet CModelChannelSat::unTune(CModelConfig * pModelConfig, bool bCheckInTuner)
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


bool CModelChannelSat::operator==(CModelChannel &other)
{
    CModelChannelSat * pOtherSat = (CModelChannelSat *)&other;

    /* check base class equivalency first */
    if (true == CModelChannel::operator==(*pOtherSat))
    {
        if ((pOtherSat->getFrequency() == getFrequency()) &&
            (pOtherSat->getMode()      == getMode()))
        {
            return true;
        }
    }

    return false;
}

#endif /* NEXUS_HAS_FRONTEND */
