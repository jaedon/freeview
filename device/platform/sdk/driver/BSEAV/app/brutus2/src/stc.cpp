/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: stc.cpp $
* $brcm_Revision: 3 $
* $brcm_Date: 4/13/12 11:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/stc.cpp $
* 
* 3   4/13/12 11:46a jrubio
* SW7346-644: add playback to stc class
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "stc.h"

BDBG_MODULE(brutus_stc);


CStc::CStc(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CResource(name, number, eBoardResource_stcChannel, pCfg),
    _stcChannel(NULL),
    _timebase(NEXUS_Timebase_eInvalid),
    _stcType(eStcType_ParserBand)
{
}

CStc::~CStc()
{
}

eRet CStc::open()
{
    eRet                     ret        = eRet_Ok;
    NEXUS_StcChannelSettings settings;

    NEXUS_StcChannel_GetDefaultSettings(_number, &settings);
    settings.timebase = (NEXUS_Timebase)_number;    /* each decode has its own timebase */

    if (true == GET_BOOL(_pCfg, USE_FIRST_PTS))
    {
        BDBG_WRN(("PTS auto behavior default %d,changing to %d",
                  settings.modeSettings.Auto.behavior,
                  NEXUS_StcChannelAutoModeBehavior_eFirstAvailable));
        settings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eFirstAvailable;
    }

    _stcChannel = NEXUS_StcChannel_Open(_number, &settings);
    CHECK_PTR_GOTO("Stc channel open failure", _stcChannel, ret, eRet_ExternalError, error);

error:
    return ret;
}

void CStc::close()
{
    if (NULL != _stcChannel)
    {
        NEXUS_StcChannel_Close(_stcChannel);
    }
}

void CStc::doit(CPid * pPcrPid)
{
    eRet ret = eRet_Ok;
    NEXUS_StcChannelSettings settings;

    NEXUS_StcChannel_GetDefaultSettings(_number, &settings);
    settings.timebase = NEXUS_Timebase_e0;
    /*settings.mode = NEXUS_StcChannelMode_ePcr;
    settings.modeSettings.pcr.pidChannel = pPcrPid->getPidChannel();*/
    _stcChannel = NEXUS_StcChannel_Open(_number, &settings);
    CHECK_PTR_GOTO("Stc channel open failure", _stcChannel, ret, eRet_ExternalError, error);

    configure(pPcrPid);
    /*
    NEXUS_StcChannel_GetSettings(_stcChannel, &settings);
    settings.mode = NEXUS_StcChannelMode_ePcr;
    settings.modeSettings.pcr.pidChannel = pPcrPid->getPidChannel();
    NEXUS_StcChannel_SetSettings(_stcChannel, &settings);
    */

error:
    return;
}

eRet CStc::configure(CPid * pPcrPid)
{
    eRet                     ret                = eRet_Ok;
    NEXUS_Error              nerror             = NEXUS_SUCCESS;
    NEXUS_StcChannelSettings settings;
    BDBG_ASSERT(NULL != pPcrPid);

    NEXUS_StcChannel_GetSettings(_stcChannel, &settings);

    switch(_stcType)
    {
    case eStcType_ParserBand:
        settings.timebase = NEXUS_Timebase_e0;
        settings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        settings.autoConfigTimebase = true;
        settings.modeSettings.pcr.offsetThreshold = 8;
        settings.modeSettings.pcr.maxPcrError = 255;
        settings.modeSettings.pcr.disableTimestampCorrection = false;
        settings.modeSettings.pcr.disableJitterAdjustment = false;
        settings.modeSettings.pcr.pidChannel = pPcrPid->getPidChannel();
        _pPcrPid = pPcrPid;
        break;

    case eStcType_TtsPacing:
        BDBG_ASSERT("not supported yet");
        break;

    case eStcType_PcrPacing:
        BDBG_ASSERT("not supported yet");
        break;

    case eStcType_IpLivePlayback:
        BDBG_ASSERT("not supported yet");
        break;

    case eStcType_PvrPlayback:
        settings.timebase = NEXUS_Timebase_e0;
        settings.mode = NEXUS_StcChannelMode_eAuto;
        break;

    default:
        BDBG_ASSERT("not supported yet");
        break;
    }

    nerror = NEXUS_StcChannel_SetSettings(_stcChannel, &settings);
    CHECK_NEXUS_ERROR_GOTO("error setting stc channel settings", ret, nerror, error);

error:
    return ret;
}

