/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_timebase.c $
* $brcm_Revision: 72 $
* $brcm_Date: 10/5/12 4:57p $
*
* API Description:
*   Management of timebase (clock rate) blocks.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/transport/src/nexus_timebase.c $
* 
* 72   10/5/12 4:57p bandrews
* SW7425-4040: return NULL instead of eInvalid
* 
* 71   10/3/12 5:01p jtna
* SW7425-3782: refactor nexus tsmf impl
* 
* 70   8/15/12 11:04a vsilyaev
* SW7420-2085: Can't call NEXUS_OBJECT_UNSET in the Timebase finalizer
* 
* 69   7/26/12 2:29p erickson
* SW7420-2328: do not NEXUS_ASSERT_MODULE in isr context
* 
* 68   7/23/12 11:36a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 67   7/5/12 1:36p vsilyaev
* SW7420-2085: Use NEXUS_OBJECT_SET for timebase
* 
* 66   6/19/12 12:44p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 65   5/10/12 6:17p jtna
* SW7425-2752: rework MTSIF-frontend <-> host interface
* 
* 64   2/2/12 8:51p bandrews
* SW7425-2299: read-only calls (GetSettings/GetStatus) do not reserve the
*  resource if enum is used
* 
* 63   2/1/12 11:51a katrep
* SW7231-317:allow hddvi time base source for hddvi input
* 
* 62   1/18/12 5:20p erickson
* SW7425-2193: avoid div-by-zero crash in
*  NEXUS_Timebase_P_GetDpcrCoefficientsFromVideoFormat
* 
* 61   12/9/11 5:22p bandrews
* SW7550-772: merge to main
* 
* SW7550-772/1   12/8/11 8:58p bandrews
* SW7550-772: implement default timebase usage
* 
* 60   11/14/11 2:28p erickson
* SW7425-1448: add NEXUS_Timebase_SetVdecFrameRate and
*  NEXUS_Timebase_SetHdDviFrameRate
* 
* 59   11/7/11 4:00p bandrews
* SW7231-446: merge to main
* 
* SW7231-446/1   11/4/11 10:11p bandrews
* SW7231-446: updates for kernel mode
* 
* 58   10/31/11 7:48p bandrews
* SW7231-391: merge to main
* 
* SW7420-2078/5   10/25/11 7:26p bandrews
* SW7231-391: fix warnings
* 
* SW7420-2078/4   10/25/11 5:22p bandrews
* SW7231-391: update parser band and timebase implementations to use
*  handles everywhere, even for legacy enum usage
* 
* SW7420-2078/3   10/14/11 8:36p bandrews
* SW7231-391: update stc channel to work with client timebase
* 
* SW7420-2078/2   10/14/11 5:05p bandrews
* SW7231-391: timebase must be > eMax to check in objdb
* 
* SW7420-2078/1   10/11/11 8:26p bandrews
* SW7231-391: add support for parser band and timebase protected client
*  usage
* 
* 57   9/14/11 2:00p erickson
* SW7408-284: add
*  NEXUS_TimebaseSettings.sourceSettings.pcr.jitterCorrection
* 
* 56   5/11/11 3:50p akam
* SWDTV-7071: Merged SWDTV-5829 to main line.
* 
* SWDTV-5829/6   5/11/11 11:58a akam
* SWDTV-5829: Merged from main.
* 
* SWDTV-5829/5   3/30/11 10:43a petlee
* SWDTV-5829: Update calculation of inc and center freq again for
*  24/25/30/50/60 Hz
* 
* SWDTV-5829/4   3/29/11 4:46p petlee
* SWDTV-5829: Update calculation of inc and center freq
* 
* SWDTV-5829/3   3/28/11 9:53a rgreen
* SWDTV-5829: Update vdec and hddvi to also use video format to determine
*  inc and center freq
* 
* SWDTV-5829/2   3/21/11 5:13p petlee
* SWDTV-5829: Update vdec and hddvi to also use video format to determine
*  inc and center freq
* 
* SWDTV-5829/1   3/18/11 5:40p petlee
* Update Nexus to sync output frequency to input using hddvi vsync clock
* 
* 54   11/19/10 11:01a jtna
* SW7408-181: merge
* 
* 53   11/8/10 4:04p gmullen
* SW7422-20: Use Int IDs generated by BXPT
* 
* 52   10/28/10 5:09p erickson
* SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
*  macros)
* 
* 51   9/20/10 1:33p petlee
* SW35230-1156: Update vdec index for 35230
* 
* 50   7/21/10 4:42p jhaberf
* SW35230-694: changes for analog video PIP
* 
* 49   7/14/10 5:37p erickson
* SW3556-1145: add NEXUS_TimebaseTrackRange to all source type settings
*
* 48   5/3/10 11:15a erickson
* SW7335-706: add NEXUS_TimebaseStatus.lastValueLo
*
* 47   12/11/09 6:41p mphillip
* SW7550-112: Merge 7550 changes to main branch
*
* 46   12/1/09 6:30p randyjew
* SW7468-6: Add 7468 support
*
* 45   11/19/09 4:25p gmohile
* SW7408-1 : Add 7408 support
*
* 44   9/18/09 10:53a jhaberf
* SW35230-1: Added support for the 35230 DTV chip
*
* 43   8/24/09 3:03p jrubio
* SW7342-11: add timebase protection for DPCR1
*
* 42   8/19/09 4:04p mward
* PR55545: Support for 7125.
*
* 41   8/5/09 4:56p jrubio
* PR55232: add 7342/7340 support
*
* 40   7/17/09 7:13p bandrews
* PR49215: astm dvr support
*
* 39   6/25/09 11:16a erickson
* PR46889: change pcrError from MSG to WRN. this WRN will only print if
*  NEXUS_TimebaseSettings.pcrErrorCallback is set.
*
* 38   6/12/09 5:58p bandrews
* PR52472: Use phase error instead of computing PCR - STC directly
*
* 37   4/28/09 10:29a erickson
* PR54656: call BXPT_PCR_DirecTv_GetLastPcr in DSS mode
*
* 36   1/20/09 4:59p bandrews
* PR51021: Change warning to message
*
* 35   1/19/09 5:28p bandrews
* PR51021: merge
*
* PR51021/2   1/16/09 4:13p bandrews
* PR51021: Updated to separate permission from enable
*
* PR51021/1   1/16/09 12:18a bandrews
* PR51021: First stab at cleaning up ASTM playback blockage
*
* 34   12/19/08 10:15a jgarrett
* PR 49587: Fixing loop filter values to appropriate values by default
*
* 33   12/9/08 6:32p nickh
* PR48963: Fix compile errors for 7420 PCR changes
*
* 32   12/8/08 3:01p bandrews
* PR50016: ensure that even if playback is set before start ASTM we
*  default dependencies
*
* 31   12/3/08 3:18p nickh
* PR48963: Add support for 7420 PCR hardware which uses a PID channel as
*  opposed to PID value
*
* 30   12/2/08 3:14p jgarrett
* PR 49587: Merging to mainline
*
* Nexus_Devel/wince_7400/9   9/17/08 5:29p ptimariu
* PR47063: merge from nexus 3.2 mainline
*
* 29   11/24/08 11:45a erickson
* PR48846: move pcrErrorCallback outside substruct for kernel mode proxy
*  parsing
*
* 28   11/24/08 10:28a erickson
* PR48846: added pcrError callback
*
* 27   11/21/08 1:21p erickson
* PR49212: NEXUS_Timebase_SetSettings needs to be able to compare new
*  with current settings. added "force" option for sourceType changes.
*  preserved Astm logic for overriding sourceType in
*  NEXUS_Timebase_SetSettings.
*
* 26   11/18/08 9:38p bandrews
* PR49212: Corrected logic error for disabling ASTM settings during
*  playback
*
* 25   11/18/08 8:53p bandrews
* PR49212: Temporarily block ASTM during playback
*
* 24   11/18/08 7:45p bandrews
* PR49212: ASTM overrides user settings, but it should not destroy them
*  when it does so
*
* 23   10/24/08 4:42p erickson
* PR47232: added trackRange for freeRun
*
* 22   10/7/08 10:40p erickson
* PR47232: move centerFrequency to freeRun.centerFrequency'
*
* 21   10/6/08 2:35a erickson
* PR47232: added
*  NEXUS_TimebaseSettings.sourceSettings.pcr.centerFrequency
*
* 20   9/3/08 7:34p vishk
* PR 46315: Sample code for PID remapping
*
* 19   9/2/08 11:19a erickson
* PR46213: added NEXUS_TimebaseSettings.freeze
*
* 18   7/17/08 10:45a erickson
* PR44492: add NEXUS_TimebaseSettings.sourceSettings.i2s.sampleRate
*
* 17   7/14/08 12:04p erickson
* PR42739: added options for IPTV
*
* PR42739/1   7/10/08 10:26p ssood
* PR42739: Adding support to absorb high jitter for Live IP Channels
*
* 16   7/3/08 10:40a erickson
* PR44492: add i2s and spdif timebase selection
*
* 15   6/23/08 3:16p jgarrett
* PR 43995: Fixing astm monitor callbacks
*
* 14   5/1/08 2:55p erickson
* PR42391: remove duplicate and unused storage to prevent future bugs
*
* 13   4/28/08 11:53a erickson
* PR42197: remove NEXUS_ParserBand_ePlayback enums
*
* 12   4/8/08 1:53p erickson
* PR39453: add 3548 & 3556
*
* 11   4/1/08 3:11p erickson
* PR40716: move break out of #if B_HAS_DSS
*
* 10   3/31/08 2:01p erickson
* PR40421: allow callback Set with NULL to clear callback
*
***************************************************************************/
#include "nexus_transport_module.h"
#include "nexus_class_verification.h"

#define NEXUS_TIMEBASE_PCR_CLOCK    0x337f980 /* may be chip-dependent */
#define NEXUS_TIMEBASE_HDDVI_CLOCK  0xcdfe600 /* may be chip-depenedent */
#define NEXUS_TIMEBASE_FREQ_FACTOR  1000

BDBG_MODULE(nexus_timebase);

static NEXUS_Error NEXUS_Timebase_P_SetTwoPcrErrorMonitor(NEXUS_TimebaseHandle timebase, bool forceOff);

static NEXUS_TimebaseHandle NEXUS_Timebase_P_ResolveAcquire(NEXUS_Timebase timebase, bool acquire)
{
    NEXUS_TimebaseHandle out = NULL;
    
    NEXUS_ASSERT_MODULE();

    if (timebase != NEXUS_Timebase_eInvalid)
    {
        if ((unsigned)timebase < (unsigned)NEXUS_Timebase_eMax)
        {
            unsigned index = timebase - NEXUS_Timebase_e0; /* assumes continuous enums */
            
            if (index < NEXUS_NUM_TIMEBASES)
            {
                /* enum variant */
                if (acquire)
                {
#if BDBG_DEBUG_BUILD
                    if (!pTransport->timebase[index].acquired)
                    {
                        BDBG_MSG(("Allocating timebase %u to unprotected client", index));
                    }
#endif
                    pTransport->timebase[index].acquired = true;
                }
                out = &pTransport->timebase[index];
            }
            else
            {
                BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
        }
        else if ((unsigned)timebase > (unsigned)NEXUS_Timebase_eMax)
        {
            /* pointer variant */
            out = (NEXUS_TimebaseHandle)timebase;
            BDBG_OBJECT_ASSERT(out, NEXUS_Timebase);
        }
        else /*if (timebase == NEXUS_Timebase_eMax)*/
        {
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }
    else
    {
        /* eInvalid means use the default timebase, which is e0, but don't acquire it */
        out = &pTransport->timebase[0];
    }

    return out;
}

NEXUS_TimebaseHandle NEXUS_Timebase_Resolve_priv(NEXUS_Timebase timebase)
{
    return NEXUS_Timebase_P_ResolveAcquire(timebase, true);
}


void NEXUS_Timebase_GetDefaultSettings_priv(NEXUS_TimebaseSettings *pSettings)
{
    BXPT_PCR_TimebaseFreqRefConfig freqRefCfg;
    BERR_Code rc;

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->sourceType = NEXUS_TimebaseSourceType_eFreeRun;
    pSettings->sourceSettings.pcr.maxPcrError = 0xFF;
    pSettings->sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
    pSettings->sourceSettings.pcr.jitterCorrection = NEXUS_TristateEnable_eNotSet; /* this is "auto". the current HW implementation of DPCR jitter correction is a 
        global setting. so, if we default to auto, then any one timebase that sets it will enable it system-wide. */
    pSettings->sourceSettings.freeRun.centerFrequency = 0x400000;
    pSettings->sourceSettings.freeRun.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
    pSettings->sourceSettings.vdec.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
    pSettings->sourceSettings.hdDvi.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
    pSettings->sourceSettings.ccir656.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
    pSettings->sourceSettings.i2s.trackRange = NEXUS_TimebaseTrackRange_e61ppm;

    /* at init time, consult HW settings so that, by default, there is no change */
    /* hard code to use timebase zero, because we don't really need an actual timebase for this call */
    rc = BXPT_PCR_GetTimeBaseFreqRefDefaults(pTransport->timebase[0].pcr, BXPT_PCR_TimeRef_eInternal, &freqRefCfg);
    if (rc) {
        rc=BERR_TRACE(rc);
    }
    else {
        /* make SW state == HW state */
        pSettings->sourceSettings.freeRun.prescale = freqRefCfg.Prescale;
        pSettings->sourceSettings.freeRun.inc = freqRefCfg.Inc;
        pSettings->sourceSettings.freeRun.loopDirectPathGain = freqRefCfg.FiltA;
        pSettings->sourceSettings.freeRun.loopGain = freqRefCfg.FiltB;
        pSettings->sourceSettings.freeRun.loopIntegratorLeak = freqRefCfg.FiltC;
    }
}

/* 108MHz VDEC clock. */
NEXUS_Error NEXUS_Timebase_P_GetDpcrCoefficients(NEXUS_VideoFrameRate frameRate, unsigned *inc, unsigned *prescale)
{
    /* select clock coefficient, based on the source format */
    switch (frameRate) {
    case NEXUS_VideoFrameRate_e29_97:
    case NEXUS_VideoFrameRate_e59_94:
        *inc = 900900;
        break;
    case NEXUS_VideoFrameRate_e30:
    case NEXUS_VideoFrameRate_e60:
        *inc = 900000;
        break;
    case NEXUS_VideoFrameRate_e25:
    case NEXUS_VideoFrameRate_e50:
        *inc = 1080000;
        break;
    case NEXUS_VideoFrameRate_e23_976:
        *inc = 2252250;
        break;
    case NEXUS_VideoFrameRate_e24:
        *inc = 2250000;
        break;
    case NEXUS_VideoFrameRate_eUnknown:
        /* leave unchanged */
        break;
    default:
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    *prescale = 0;
    return 0;
}

NEXUS_Error NEXUS_Timebase_P_GetDpcrCoefficientsFromVideoFormat(NEXUS_VideoFormat videoFormat, NEXUS_VideoFrameRate frameRate, unsigned *inc, unsigned *prescale)
{
    NEXUS_VideoFormatInfo videoFmtInfo;
    uint32_t refreshRate; /* this is the refresh rate multiplied by a factor of 1000 */

    if (NEXUS_VideoFormat_eUnknown == videoFormat)
    {
        return NEXUS_Timebase_P_GetDpcrCoefficients(frameRate, inc, prescale);
    }

    NEXUS_VideoFormat_GetInfo(videoFormat, &videoFmtInfo);

    refreshRate = (2397 == videoFmtInfo.verticalFreq) ? 23976 : videoFmtInfo.verticalFreq * 10;
    if (refreshRate) {
        *inc = (uint32_t)((uint64_t)NEXUS_TIMEBASE_PCR_CLOCK * (uint64_t)NEXUS_TIMEBASE_FREQ_FACTOR / (uint64_t)refreshRate);
    }
    else {
        *inc = 0;
    }
    *prescale = 0;

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Timebase_P_GetIncAndCenterFreq(NEXUS_VideoFormat videoFormat, unsigned vertSyncClock, unsigned *inc, unsigned *centerFreq)
{
    NEXUS_VideoFormatInfo videoFmtInfo;
    uint32_t refreshRate = 0;
    uint32_t hddviFactor = 0;
    uint32_t pcrFactor = 0;

    if (0 == vertSyncClock || NEXUS_VideoFormat_eUnknown == videoFormat) return BERR_TRACE(NEXUS_INVALID_PARAMETER);

    NEXUS_VideoFormat_GetInfo(videoFormat, &videoFmtInfo);

    if ((2300 < videoFmtInfo.verticalFreq) && (2450 >= videoFmtInfo.verticalFreq)) {
        refreshRate = 24;
    } else if ((2450 < videoFmtInfo.verticalFreq) && (2600 >= videoFmtInfo.verticalFreq)) {
        refreshRate = 25;
    } else if ((2900 < videoFmtInfo.verticalFreq) && (3100 >= videoFmtInfo.verticalFreq)) {
        refreshRate = 30 ;
    } else if ((4900 < videoFmtInfo.verticalFreq) && (5100 >= videoFmtInfo.verticalFreq)) {
        refreshRate = 50;
    } else if ((5900 < videoFmtInfo.verticalFreq) && (6100 >= videoFmtInfo.verticalFreq)) {
        refreshRate = 60;
    } else {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    hddviFactor = NEXUS_TIMEBASE_HDDVI_CLOCK / refreshRate;
    pcrFactor = NEXUS_TIMEBASE_PCR_CLOCK / refreshRate;

    *inc = (uint32_t)((uint64_t)pcrFactor * (uint64_t)vertSyncClock / (uint64_t)hddviFactor);
    *centerFreq = (uint32_t)((uint64_t)0x400000 * (uint64_t)hddviFactor / (uint64_t)vertSyncClock);

    BDBG_MSG(("NEXUS_Timebase_P_GetIncAndCenterFreq: refreshRate=%u, vertSyncClock=%u, inc=%u, centerFreq=%u",
        refreshRate,
        vertSyncClock,
        *inc,
        *centerFreq
        ));


    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Timebase_P_SetSettings(NEXUS_TimebaseHandle timebase, const NEXUS_TimebaseSettings *pSettings)
{
    BERR_Code rc;
    BXPT_PCR_Handle pcr = NULL;
    BXPT_PCR_TimebaseFreqRefConfig freqRefConfig;
    BXPT_PCR_TimeRef timeref;
    NEXUS_TimebaseSourceType sourceType;
#if NEXUS_HAS_ASTM
    NEXUS_TimebaseAstmSettings * pAstmSettings;
#endif
    bool force;

    BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase);
    
    pcr = timebase->pcr;
    /* read sourceType out of the structure because Astm might change it. do not use pSettings->sourceType later in this function. */
    sourceType = pSettings->sourceType;

#if NEXUS_HAS_ASTM
    /* if we aren't attempting to set the source type to FreeRun from the user config, permit ASTM.
    Having the user set the sourceType to eFreeRun is equivalent to being in playback mode, and we don't support that in ASTM
    so disable ASTM */
    pAstmSettings = &timebase->astm.settings;

    if (pAstmSettings->enabled)
    {
        BDBG_MSG(("ASTM is setting the clock coupling for timebase %u to %s", timebase->hwIndex, pAstmSettings->clockCoupling == NEXUS_TimebaseClockCoupling_eInternalClock ? "internal" : "input"));
        if (pAstmSettings->clockCoupling == NEXUS_TimebaseClockCoupling_eInternalClock)
        {
            sourceType = NEXUS_TimebaseSourceType_eFreeRun;
        }
    }
#endif

    timebase->status.sourceType = sourceType;

    force = (sourceType != timebase->settings.sourceType);

    if ( sourceType == NEXUS_TimebaseSourceType_ePcr) {
        NEXUS_IsrCallback_Set(timebase->monitorCallback, &pSettings->pcrCallback);
    }
    else {
        NEXUS_IsrCallback_Set(timebase->monitorCallback, NULL);
    }

    timebase->isDss = false;
    switch (sourceType) {
    case NEXUS_TimebaseSourceType_ePcr:
        {
        BXPT_PCR_XptStreamPcrCfg pcrConfig;
#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
        BXPT_PCR_JitterCorrection jitterCorrection;
#endif
        NEXUS_PidChannelHandle pcrPidChannel = pSettings->sourceSettings.pcr.pidChannel;
        
        if (!pcrPidChannel) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        
        rc = BXPT_PCR_GetStreamPcrConfig(pcr, &pcrConfig);
        if (rc) {return BERR_TRACE(rc);}

#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
        /* transform nexus tristate to BXPT_PCR_JitterCorrection tristate */
        switch (pSettings->sourceSettings.pcr.jitterCorrection) {
        case NEXUS_TristateEnable_eDisable: jitterCorrection = BXPT_PCR_JitterCorrection_eDisable; break;
        case NEXUS_TristateEnable_eEnable:  jitterCorrection = BXPT_PCR_JitterCorrection_eEnable; break;
        case NEXUS_TristateEnable_eNotSet:  jitterCorrection = BXPT_PCR_JitterCorrection_eAuto; break;
        default: return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        if (jitterCorrection == BXPT_PCR_JitterCorrection_eAuto) {
            pcrConfig.JitterTimestamp = BXPT_PCR_JitterTimestampMode_eAuto;
        }
        else if ( pcrPidChannel->status.playback ) {
            /* for playback, nexus does not expose e28_4P or e32, so we can hardcode to e30_2U */
            pcrConfig.JitterTimestamp = BXPT_PCR_JitterTimestampMode_e30_2U;
        }
        else {
            /* for live, there is no HW capability for e30_2U or e28_4P, so we can hardcode to e32 */
            pcrConfig.JitterTimestamp = BXPT_PCR_JitterTimestampMode_e32;
        }
#endif
        
        if ( pcrPidChannel->status.playback )
        {
#if !BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
            pcrConfig.eStreamSelect = BXPT_DataSource_ePlayback;
            pcrConfig.WhichSource = pcrPidChannel->status.playbackIndex;
#endif
#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
            pcrConfig.PbJitterDisable = jitterCorrection;
#endif
        }
        else
        {
            NEXUS_ParserBandSettings parserSettings;

            NEXUS_ParserBand_P_GetSettings(pcrPidChannel->parserBand, &parserSettings);
            if ( parserSettings.sourceType == NEXUS_ParserBandSourceType_eInputBand || 
                 parserSettings.sourceType == NEXUS_ParserBandSourceType_eMtsif ||
                 parserSettings.sourceType == NEXUS_ParserBandSourceType_eTsmf )
            {
#if !BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
                pcrConfig.eStreamSelect = BXPT_DataSource_eInputBand;
                pcrConfig.WhichSource = parserSettings.sourceTypeSettings.inputBand - NEXUS_InputBand_e0;
#endif
            }
            else if ( parserSettings.sourceType == NEXUS_ParserBandSourceType_eRemux )
            {
                BDBG_ERR(("You cannot use a PCR pid channel from a remux looped back to a parser band. Instead, use a pid channel from the original input band."));
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
            else
            {
                BDBG_ERR(("Invalid parser band source (%d) for timebase input."));
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
            pcrConfig.LiveJitterDisable = jitterCorrection;
#endif
        }

        pcrConfig.MaxPcrError = pSettings->sourceSettings.pcr.maxPcrError;
#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
        /* 7420 PCR hardware uses the PID channel */
        pcrConfig.PidChannel = pcrPidChannel->status.pidChannelIndex;
#else
        pcrConfig.Pid = pcrPidChannel->status.pid;
#endif
        rc = BXPT_PCR_GetTimeBaseFreqRefDefaults(pcr, BXPT_PCR_TimeRef_eXpt, &pcrConfig.TimebaseCfg);
        if (rc) {return BERR_TRACE(rc);}

        rc = BXPT_PCR_SetStreamPcrConfig(pcr, &pcrConfig);
        if (rc) {return BERR_TRACE(rc);}

        timebase->isDss = NEXUS_IS_DSS_MODE(pcrPidChannel->status.transportType);

        /* Set the crystal tracking range */
        if (force || pSettings->sourceSettings.pcr.trackRange != timebase->settings.sourceSettings.pcr.trackRange) {
            BDBG_CASSERT(NEXUS_TimebaseTrackRange_e244ppm == (NEXUS_TimebaseTrackRange)BXPT_PCR_TrackRange_PPM_244);
            BXPT_PCR_SetTimeRefTrackRange(pcr, pSettings->sourceSettings.pcr.trackRange);
        }

        BDBG_MSG(("%s: PCR Mode Timebase (%u) Settings: track range %d, max pcr error %d\n",
                    __FUNCTION__, timebase->hwIndex, pSettings->sourceSettings.pcr.trackRange, pSettings->sourceSettings.pcr.maxPcrError));
        }
        break;

    case NEXUS_TimebaseSourceType_eFreeRun:
        rc = BXPT_PCR_GetTimeBaseFreqRefDefaults(pcr, BXPT_PCR_TimeRef_eInternal, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}

        freqRefConfig.FiltA = pSettings->sourceSettings.freeRun.loopDirectPathGain;
        freqRefConfig.FiltB = pSettings->sourceSettings.freeRun.loopGain;
        freqRefConfig.FiltC = pSettings->sourceSettings.freeRun.loopIntegratorLeak;

        freqRefConfig.Prescale = pSettings->sourceSettings.freeRun.prescale;
        freqRefConfig.Inc = pSettings->sourceSettings.freeRun.inc;
        rc = BXPT_PCR_ConfigNonStreamTimeBase(pcr, BXPT_PCR_TimeRef_eInternal, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}

        if (force || pSettings->sourceSettings.freeRun.centerFrequency != timebase->settings.sourceSettings.freeRun.centerFrequency) {
            BXPT_PCR_SetCenterFrequency(pcr, pSettings->sourceSettings.freeRun.centerFrequency);
        }
        /* Set the crystal tracking range */
        if (force || pSettings->sourceSettings.freeRun.trackRange != timebase->settings.sourceSettings.freeRun.trackRange) {
            BXPT_PCR_SetTimeRefTrackRange(pcr, pSettings->sourceSettings.freeRun.trackRange);
        }
        break;

    case NEXUS_TimebaseSourceType_eAnalog:
        /* select clock coefficient, based on the source format */
        /* TODO: vdec.index is needed to select the right enum here, but the user must also pass the timebase to the right NEXUS_AnalogVideoDecoder to set the right
        framerate. This could be mismatched. */
        switch (pSettings->sourceSettings.vdec.index) {
#if NEXUS_DTV_PLATFORM
#if NEXUS_DTV_PLATFORM < 3
        case 0: timeref = BXPT_PCR_TimeRef_eVDECVl; break;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS > 1
        case 1: timeref = BXPT_PCR_TimeRef_eVDEC_SEC_VL; break;
#endif
#else
        case 0: timeref = BXPT_PCR_TimeRef_eHD_DVI_V0; break;
        case 1: timeref = BXPT_PCR_TimeRef_eHD_DVI_V1; break;
#endif
#endif
        default: return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
#if NEXUS_DTV_PLATFORM
        rc = BXPT_PCR_GetTimeBaseFreqRefDefaults(pcr, timeref, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}

        /* For HD VDEC's, we must look up based on various refresh rates */
        rc = NEXUS_Timebase_P_GetDpcrCoefficientsFromVideoFormat(
            pSettings->sourceSettings.vdec.format,
            timebase->vdecFrameRate?timebase->vdecFrameRate:pSettings->sourceSettings.vdec.frameRate,
            &freqRefConfig.Inc,
            &freqRefConfig.Prescale
            );
        if (rc) {return BERR_TRACE(rc);}

        if (pSettings->sourceSettings.hdDvi.vertSyncClock)
        {
            uint32_t inc = 0;
            uint32_t centerFreq = 0;

            rc = NEXUS_Timebase_P_GetIncAndCenterFreq(
                     pSettings->sourceSettings.vdec.format,
                     pSettings->sourceSettings.hdDvi.vertSyncClock,
                     &inc,
                     &centerFreq);

            if (NEXUS_SUCCESS == rc)
            {
                freqRefConfig.Inc = inc;
                BXPT_PCR_SetCenterFrequency(pcr, centerFreq);
            }

            BDBG_WRN(("NEXUS_Timebase_SetSettings: eAnalog - vertSyncClock=%u, inc=%u, centerFreq=%u",
                pSettings->sourceSettings.hdDvi.vertSyncClock,
                freqRefConfig.Inc,
                centerFreq));
        }

        rc = BXPT_PCR_ConfigNonStreamTimeBase(pcr, timeref, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}

        /* Set the crystal tracking range */
        if (force || pSettings->sourceSettings.vdec.trackRange != timebase->settings.sourceSettings.vdec.trackRange) {
            BXPT_PCR_SetTimeRefTrackRange(pcr, pSettings->sourceSettings.vdec.trackRange);
        }
#endif
        break;

    case NEXUS_TimebaseSourceType_eCcir656In:
        timeref = BXPT_PCR_TimeRef_eI656_Vl;
        rc = BXPT_PCR_GetTimeBaseFreqRefDefaults(pcr, timeref, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}

        /* select clock coefficient, based on the source format. This is SD only, so we don't need framerate.
        TODO: likely these values are different for 3563/3548. */
        switch (pSettings->sourceSettings.ccir656.format) {
        case NEXUS_VideoFormat_eNtsc:
        case NEXUS_VideoFormat_eNtscJapan:
            freqRefConfig.Inc = 900900; break;
        default: /* PAL */
            freqRefConfig.Inc = 1080000; break;
        }

        freqRefConfig.Prescale = 0;
        rc = BXPT_PCR_ConfigNonStreamTimeBase(pcr, timeref, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}

        /* Set the crystal tracking range */
        if (force || pSettings->sourceSettings.ccir656.trackRange != timebase->settings.sourceSettings.ccir656.trackRange) {
            BXPT_PCR_SetTimeRefTrackRange(pcr, pSettings->sourceSettings.ccir656.trackRange);
        }
        break;

#if NEXUS_NUM_I2S_INPUTS > 0
    case NEXUS_TimebaseSourceType_eI2sIn:
        switch (pSettings->sourceSettings.i2s.index) {
        case 0: timeref = BXPT_PCR_TimeRef_eI2S0; break;
#if NEXUS_NUM_I2S_INPUTS > 1
        case 1: timeref = BXPT_PCR_TimeRef_eI2S1; break;
#endif
        default: return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        rc = BXPT_PCR_GetTimeBaseFreqRefDefaults(pcr, timeref, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}

        switch (pSettings->sourceSettings.i2s.sampleRate) {
        case 48000:
            freqRefConfig.Prescale = 1;
            freqRefConfig.Inc = 1125;
            break;
        case 44100:
            freqRefConfig.Prescale = 48;
            freqRefConfig.Inc = 30000;
            break;
        case 32000:
            freqRefConfig.Prescale = 3;
            freqRefConfig.Inc = 3375;
            break;
        default:
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        rc = BXPT_PCR_ConfigNonStreamTimeBase(pcr, timeref, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}

        /* Set the crystal tracking range */
        if (force || pSettings->sourceSettings.i2s.trackRange != timebase->settings.sourceSettings.i2s.trackRange) {
            BXPT_PCR_SetTimeRefTrackRange(pcr, pSettings->sourceSettings.i2s.trackRange);
        }
        break;
#endif

#if NEXUS_NUM_SPDIF_INPUTS > 0
    case NEXUS_TimebaseSourceType_eSpdifIn:
        timeref = BXPT_PCR_TimeRef_eSPDIF;

        rc = BXPT_PCR_GetTimeBaseFreqRefDefaults(pcr, timeref, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}

        rc = BXPT_PCR_ConfigNonStreamTimeBase(pcr, timeref, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}
        break;
#endif

#if NEXUS_NUM_HDMI_INPUTS || NEXUS_NUM_HDDVI_INPUTS
    case NEXUS_TimebaseSourceType_eHdDviIn:
        switch (pSettings->sourceSettings.hdDvi.index) {
        case 0: timeref = BXPT_PCR_TimeRef_eHD_DVI_V0; break;
        case 1: timeref = BXPT_PCR_TimeRef_eHD_DVI_V1; break;
        default: return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        /* Set XPT timebase */
        rc = BXPT_PCR_GetTimeBaseFreqRefDefaults(pcr, timeref, &freqRefConfig);
        if (rc) {return BERR_TRACE(rc);}

        rc = NEXUS_Timebase_P_GetDpcrCoefficientsFromVideoFormat(
            pSettings->sourceSettings.hdDvi.format,
            timebase->hdDviFrameRate?timebase->hdDviFrameRate:pSettings->sourceSettings.hdDvi.frameRate,
            &freqRefConfig.Inc,
            &freqRefConfig.Prescale
            );
        if (rc) {return BERR_TRACE(rc);}

        if (pSettings->sourceSettings.hdDvi.vertSyncClock)
        {
            uint32_t inc = 0;
            uint32_t centerFreq = 0;

            rc = NEXUS_Timebase_P_GetIncAndCenterFreq(
                     pSettings->sourceSettings.hdDvi.format,
                     pSettings->sourceSettings.hdDvi.vertSyncClock,
                     &inc,
                     &centerFreq);

             if (NEXUS_SUCCESS == rc)
            {
                freqRefConfig.Inc = inc;
                BXPT_PCR_SetCenterFrequency(pcr, centerFreq);
            }

            BDBG_MSG(("NEXUS_Timebase_SetSettings: eHdDviIn - vertSyncClock=%u, inc=%u, centerFreq=%u",
                pSettings->sourceSettings.hdDvi.vertSyncClock,
                freqRefConfig.Inc,
                centerFreq));
        }

        rc = BXPT_PCR_ConfigNonStreamTimeBase(pcr, timeref, &freqRefConfig);
        if (rc) BERR_TRACE(rc);

        /* Set the crystal tracking range */
        if (force || pSettings->sourceSettings.hdDvi.trackRange != timebase->settings.sourceSettings.hdDvi.trackRange) {
            BXPT_PCR_SetTimeRefTrackRange(pcr, pSettings->sourceSettings.hdDvi.trackRange);
        }
        break;
#endif

    default:
        BDBG_ERR(("unsupported timebase source type"));
        return NEXUS_INVALID_PARAMETER;
    }

#if B_HAS_DSS
    rc = BXPT_PCR_DirecTv_SetPcrMode(pcr, timebase->isDss?BXPT_PcrMode_eDirecTv:BXPT_PcrMode_eMpeg);
    if (rc) {return BERR_TRACE(rc);}
#endif

    BXPT_PCR_FreezeIntegrator(pcr, pSettings->freeze);

    timebase->settings = *pSettings;

    rc = NEXUS_Timebase_P_SetTwoPcrErrorMonitor(timebase, false);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

NEXUS_Error NEXUS_Timebase_SetSettings(NEXUS_Timebase timebase, const NEXUS_TimebaseSettings *pSettings)
{
    NEXUS_TimebaseHandle handle;

    handle = NEXUS_Timebase_Resolve_priv(timebase);

    if (handle)
    {
        return NEXUS_Timebase_P_SetSettings(handle, pSettings);
    }
    else
    {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
}

void NEXUS_Timebase_P_GetSettings(NEXUS_TimebaseHandle timebase, NEXUS_TimebaseSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase);
    *pSettings = timebase->settings;
}

void NEXUS_Timebase_GetSettings(NEXUS_Timebase timebase, NEXUS_TimebaseSettings *pSettings)
{
    NEXUS_TimebaseHandle handle;

    handle = NEXUS_Timebase_P_ResolveAcquire(timebase, false);

    if (handle)
    {
        NEXUS_Timebase_P_GetSettings(handle, pSettings);
    }
    else
    {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
}

static int NEXUS_Timebase_P_NextAvailable(void)
{
    int next = -1;
    unsigned i = 0;
    
    for (i = 0; i < NEXUS_NUM_TIMEBASES; i++) 
    {
        if (!pTransport->timebase[i].acquired) 
        {
            next = i;
            break;
        }
    }

    return next;
}

static NEXUS_TimebaseHandle NEXUS_Timebase_P_Open(unsigned index)
{
    NEXUS_TimebaseHandle handle = NULL;

    if (index == NEXUS_ANY_ID)
    {
        int next = NEXUS_Timebase_P_NextAvailable();
        if (next == -1)
        {
            BERR_TRACE(NEXUS_NOT_AVAILABLE);
            goto end;
        }
        else
        {
            index = (unsigned)next;
        }
    }
    
    if (index >= NEXUS_NUM_TIMEBASES) 
    {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto end;
    }

    if (!pTransport->timebase[index].acquired) 
    {
        pTransport->timebase[index].acquired = true;
        /* BDBG_MSG(("Allocating timebase %u to client %p", index, b_objdb_get_client())); */
        handle = &pTransport->timebase[index];
        NEXUS_OBJECT_SET(NEXUS_Timebase, handle);
    }
    else 
    {
        BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

end:
    return handle;
}

NEXUS_Timebase NEXUS_Timebase_Open(unsigned index)
{
    NEXUS_TimebaseHandle handle = NULL;
    handle = NEXUS_Timebase_P_Open(index);
    /* here we cast to the enum, but it is really a pointer */
    return handle?(NEXUS_Timebase)handle:(NEXUS_Timebase)NULL;
}

static void NEXUS_Timebase_P_Finalizer(NEXUS_TimebaseHandle timebase)
{
    NEXUS_TimebaseSettings settings;
    
    NEXUS_OBJECT_ASSERT(NEXUS_Timebase, timebase);

    /* revert to default known state */
    NEXUS_Timebase_GetDefaultSettings_priv(&settings);
    NEXUS_Timebase_P_SetSettings(timebase, &settings);

    /* this may unregister enum variant usage, if that usage was done
    in an unprotected client *after* a protected client already acquired
    the resource */
    timebase->acquired = false;
    /* can't call NEXUS_OBJECT_UNSET since there is a still internal API that uses object */

    /* BDBG_MSG(("Client %p releasing timebase %u", b_objdb_get_client(), timebase->hwIndex)); */
    return;
}

static NEXUS_OBJECT_CLASS_MAKE(NEXUS_Timebase, NEXUS_Timebase_P_Close);

void NEXUS_Timebase_Close(NEXUS_Timebase timebase)
{
    NEXUS_TimebaseHandle handle = NULL;

    handle = NEXUS_Timebase_Resolve_priv(timebase);

    if (handle)
    {
        NEXUS_Timebase_P_Close(handle);
    }
    else
    {
        BDBG_ERR(("You may be attempting to close the enum variant of this resource.  Please ensure you are passing the resource returned when you called open."));
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    return;
}


static void NEXUS_Timebase_P_Monitor_isr(void *context, int param)
{
    NEXUS_TimebaseHandle timebase = context;
    NEXUS_TimebaseStatus *pStatus = &timebase->status;
    BSTD_UNUSED(param);

    BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase);

    BDBG_MSG(("Timebase %u: received PCR", timebase->hwIndex));
    pStatus->pcrValid = true;
    pStatus->pcrCount++;
    NEXUS_IsrCallback_Fire_isr(timebase->monitorCallback);

#if NEXUS_HAS_ASTM
    if (timebase->astm.settings.enabled)
    {
        if (timebase->astm.settings.pcrReceived_isr)
        {
            timebase->astm.settings.pcrReceived_isr(timebase->astm.settings.callbackContext, timebase->hwIndex);
        }
    }
#endif
}

NEXUS_Error NEXUS_Timebase_P_StartMonitor(NEXUS_TimebaseHandle timebase)
{
    BERR_Code rc;
    BINT_Id pcr_int;

    BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase);
 
    BDBG_MSG(("start monitor %d", timebase->hwIndex));

    rc = BXPT_PCR_GetIntId( timebase->hwIndex, BXPT_PCR_IntName_ePhaseCompare, &pcr_int );
    if (rc) return BERR_TRACE(rc);

    rc = BINT_CreateCallback(&timebase->intMonitorCallback, g_pCoreHandles->bint, pcr_int, NEXUS_Timebase_P_Monitor_isr, timebase, 0);
    if (rc) return BERR_TRACE(rc);
    rc = BINT_EnableCallback(timebase->intMonitorCallback);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

void NEXUS_Timebase_P_StopMonitor(NEXUS_TimebaseHandle timebase)
{
     BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase);

    if (timebase->intMonitorCallback) {
        BINT_DisableCallback(timebase->intMonitorCallback);
        BINT_DestroyCallback(timebase->intMonitorCallback);
        timebase->intMonitorCallback = NULL;
    }

    /* clean up in case it was left on */
    (void)NEXUS_Timebase_P_SetTwoPcrErrorMonitor(timebase, true);
}

static void NEXUS_Timebase_P_TwoPcrError_isr( void *context, int param )
{
    NEXUS_TimebaseHandle timebase = context;
    BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase);
    BSTD_UNUSED(param);
    timebase->status.pcrErrors++;
    BDBG_WRN(("Timebase %u: pcrError", timebase->hwIndex));
    NEXUS_IsrCallback_Fire_isr(timebase->pcrErrorCallback);
}

static NEXUS_Error NEXUS_Timebase_P_SetTwoPcrErrorMonitor(NEXUS_TimebaseHandle timebase, bool forceOff)
{
    BINT_Id dpcrErrorIntId;
    bool install = false;

    BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase);

    install = (timebase->settings.sourceType == NEXUS_TimebaseSourceType_ePcr &&
               timebase->settings.sourceSettings.pcr.pidChannel &&
               timebase->settings.pcrErrorCallback.callback &&
               !forceOff);

    if ( install ) {
        NEXUS_IsrCallback_Set(timebase->pcrErrorCallback, &timebase->settings.pcrErrorCallback);
    }
    else {
        NEXUS_IsrCallback_Set(timebase->pcrErrorCallback, NULL);
    }

    if (install && !timebase->intPcrErrorCallback) {
        BERR_Code rc;
        BDBG_MSG(("Installing DPCR%u TWO_PCR_ERROR interrupt handler", timebase->hwIndex));

        rc = BXPT_PCR_GetIntId( timebase->hwIndex, BXPT_PCR_IntName_eTwoPcrErrors, &dpcrErrorIntId );
        if (rc) return BERR_TRACE(rc);

        rc = BINT_CreateCallback(&timebase->intPcrErrorCallback, g_pCoreHandles->bint, dpcrErrorIntId, NEXUS_Timebase_P_TwoPcrError_isr, timebase, 0);
        if (rc) return BERR_TRACE(rc);
        rc = BINT_EnableCallback(timebase->intPcrErrorCallback);
        if (rc) return BERR_TRACE(rc);
    }
    else if (!install && timebase->intPcrErrorCallback) {
        BINT_DisableCallback(timebase->intPcrErrorCallback);
        BINT_DestroyCallback(timebase->intPcrErrorCallback);
        timebase->intPcrErrorCallback = NULL;
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Timebase_GetStatus_priv_isr(NEXUS_TimebaseHandle timebase, NEXUS_TimebaseStatus *pStatus)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    uint32_t hi, lo;
    int32_t error;

    BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase);

    *pStatus = timebase->status;

    do
    {
#if B_HAS_DSS
        if (timebase->isDss)
        {
            BXPT_PCR_DirecTv_GetLastPcr(timebase->pcr, &hi);
            pStatus->lastValue = hi;
            pStatus->lastValueLo = 0;
        }
        else
#endif
        {
            BXPT_PCR_GetLastPcr_isr(timebase->pcr, &hi, &lo);
            pStatus->lastValue = hi;
            pStatus->lastValueLo = lo;
        }
        BXPT_PCR_GetPhaseError_isr(timebase->pcr, &error);
        /* read again to make sure that the phase goes with the correct PCR */
#if B_HAS_DSS
        if (timebase->isDss)
        {
            BXPT_PCR_DirecTv_GetLastPcr(timebase->pcr, &hi);
        }
        else
#endif
        {
            BXPT_PCR_GetLastPcr_isr(timebase->pcr, &hi, &lo);
        }
    } while (pStatus->lastValue != hi);

    pStatus->lastError = error;

    return rc;
}

NEXUS_Error NEXUS_Timebase_GetStatus(NEXUS_Timebase timebase, NEXUS_TimebaseStatus *pStatus)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_TimebaseHandle handle;

    handle = NEXUS_Timebase_P_ResolveAcquire(timebase, false);

    if (handle)
    {
        BKNI_EnterCriticalSection();
        rc = NEXUS_Timebase_GetStatus_priv_isr(handle, pStatus);
        BKNI_LeaveCriticalSection();
    }
    else
    {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    return rc;
}

NEXUS_Error NEXUS_Timebase_ResetStatus(NEXUS_Timebase timebase)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_TimebaseHandle handle;

    handle = NEXUS_Timebase_Resolve_priv(timebase);

    if (handle)
    {
        BKNI_Memset(&handle->status, 0, sizeof(handle->status));
        /* this sets count to 0 and valid to false. */
        handle->status.sourceType = handle->settings.sourceType;

#if NEXUS_HAS_ASTM
        if (handle->astm.settings.enabled)
        {
            if (handle->astm.settings.clockCoupling == NEXUS_TimebaseClockCoupling_eInternalClock)
            {
                handle->status.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
            }
        }
#endif
    }
    else
    {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    return rc;
}

#if NEXUS_HAS_ASTM
void NEXUS_Timebase_GetAstmSettings_priv(
    NEXUS_TimebaseHandle timebase,
    NEXUS_TimebaseAstmSettings * pAstmSettings  /* [out] */
)
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase); 

    *pAstmSettings = timebase->astm.settings;
}

NEXUS_Error NEXUS_Timebase_SetAstmSettings_priv(
    NEXUS_TimebaseHandle timebase,
    const NEXUS_TimebaseAstmSettings * pAstmSettings
)
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase);

    /* copy settings as-is, this way ASTM will always get what it set */
    timebase->astm.settings = *pAstmSettings;

    /* if ASTM is internally permitted, reapply settings */
    rc = NEXUS_Timebase_P_SetSettings(timebase, &timebase->settings);

    return rc;
}
#endif /* NEXUS_HAS_ASTM */

NEXUS_Error NEXUS_Timebase_GetIndex_priv(
    NEXUS_TimebaseHandle timebase,
    unsigned * pIndex
    )
{
    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(pIndex);
    BDBG_OBJECT_ASSERT(timebase, NEXUS_Timebase);
    *pIndex = timebase->hwIndex;
    return 0;
}

NEXUS_Error NEXUS_Timebase_SetHdDviFrameRate( NEXUS_Timebase timebase, NEXUS_VideoFrameRate frameRate )
{
    NEXUS_TimebaseHandle handle;
    handle = NEXUS_Timebase_Resolve_priv(timebase);
    if (!handle) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    
    handle->hdDviFrameRate = frameRate;
    return NEXUS_Timebase_P_SetSettings(handle, &handle->settings);
}

NEXUS_Error NEXUS_Timebase_SetVdecFrameRate( NEXUS_Timebase timebase, NEXUS_VideoFrameRate frameRate )
{
    NEXUS_TimebaseHandle handle;
    handle = NEXUS_Timebase_Resolve_priv(timebase);
    if (!handle) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    
    handle->vdecFrameRate = frameRate;
    return NEXUS_Timebase_P_SetSettings(handle, &handle->settings);
}
