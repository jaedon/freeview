/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: timebase.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 6/28/12 3:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/timebase.c $
 * 
 * Hydra_Software_Devel/7   6/28/12 3:55p mward
 * SW7401-4521: use NEXUS_TransportCapabilities.stcPrescaleMax and
 * .stcIncrementMax
 * 
 * Hydra_Software_Devel/6   6/27/12 6:11p mward
 * SW7401-3748: inc and prescale off by one.
 * 
 * Hydra_Software_Devel/5   6/27/12 12:24p mward
 * SW7401-3748: Added runtime param check for PCR_INC and PRESCALE.  Added
 * a flag NEXUS_TIMEBASE_HAS_32BIT_PRESCALE which can be defined for
 * chips in which BXPT_HAS_32BIT_PRESCALE.
 * 
 * Hydra_Software_Devel/4   2/16/12 2:47p katrep
 * SW7231-317:autotest failure
 * 
 * Hydra_Software_Devel/3   1/18/12 5:20p erickson
 * SW7425-2193: extend test for chips with NEXUS_NUM_HDMI_INPUTS defined
 * but not NEXUS_NUM_HDDVI_INPUTS defined. add tests for timebase
 * "setframerate" functions.
 * 
 * Hydra_Software_Devel/2   11/2/11 10:11a erickson
 * SW7231-391: adapt unittests to enum/handle multiplex
 * 
 * Hydra_Software_Devel/1   7/29/11 10:33a erickson
 * SW7425-1029: add timebase unittest
 * 
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_timebase.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

BDBG_MODULE(timebase);

static void pcr_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
}

static void pcr_error_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
}

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_TimebaseSettings settings;
    NEXUS_TimebaseStatus status;
    NEXUS_Error rc;
    unsigned loops = 5000;
    bool expect_failure;
    NEXUS_PidChannelHandle pcrPidChannel;
    
    srand(time(NULL));
    
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    
    pcrPidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 0x11, NULL);
    BDBG_ASSERT(pcrPidChannel);
    
    BDBG_CASSERT(NEXUS_NUM_TIMEBASES <= NEXUS_Timebase_eMax);
    
    while (loops--) {
        NEXUS_Timebase timebase = NEXUS_Timebase_e0 + (rand() % NEXUS_NUM_TIMEBASES);
        NEXUS_TransportCapabilities capabilities;
        switch (rand()%5) {
        case 0:
            expect_failure = false;
            NEXUS_Timebase_GetSettings(timebase, &settings);
            settings.pcrCallback.callback = rand()%2?pcr_callback:NULL;
            settings.pcrErrorCallback.callback = rand()%2?pcr_error_callback:NULL;
            settings.freeze = rand()%2;
            settings.sourceType = rand()%NEXUS_TimebaseSourceType_eMax;
            BDBG_MSG(("settings.sourceType %d", settings.sourceType));
            switch (settings.sourceType) {
            case NEXUS_TimebaseSourceType_ePcr:
                settings.sourceSettings.pcr.trackRange = rand()%NEXUS_TimebaseTrackRange_eMax;
                settings.sourceSettings.pcr.maxPcrError = rand();
                settings.sourceSettings.pcr.pidChannel = pcrPidChannel;
                break;
            case NEXUS_TimebaseSourceType_eFreeRun:
                NEXUS_GetTransportCapabilities(&capabilities);
                settings.sourceSettings.freeRun.trackRange = rand()%NEXUS_TimebaseTrackRange_eMax;
                settings.sourceSettings.freeRun.centerFrequency = rand();
                settings.sourceSettings.freeRun.prescale = rand()%capabilities.stcPrescaleMax;
                settings.sourceSettings.freeRun.inc = rand()%capabilities.stcIncMax;
                settings.sourceSettings.freeRun.loopDirectPathGain = rand();
                settings.sourceSettings.freeRun.loopGain = rand();
                settings.sourceSettings.freeRun.loopIntegratorLeak = rand();
                break;
            case NEXUS_TimebaseSourceType_eAnalog:
#ifndef NEXUS_NUM_ANALOG_AUDIO_DECODERS
#define NEXUS_NUM_ANALOG_AUDIO_DECODERS 0
#endif
                expect_failure = !NEXUS_NUM_ANALOG_AUDIO_DECODERS;
                settings.sourceSettings.vdec.index = NEXUS_NUM_ANALOG_AUDIO_DECODERS ? rand()%NEXUS_NUM_ANALOG_AUDIO_DECODERS : 0;
                settings.sourceSettings.vdec.format = rand()%NEXUS_VideoFormat_eMax;
                settings.sourceSettings.vdec.frameRate = rand()%NEXUS_VideoFrameRate_eMax;
                settings.sourceSettings.vdec.trackRange = rand()%NEXUS_TimebaseTrackRange_eMax;
                break;
            case NEXUS_TimebaseSourceType_eHdDviIn:
#ifndef NEXUS_NUM_HDDVI_INPUTS
#define NEXUS_NUM_HDDVI_INPUTS 0
#endif
#ifndef NEXUS_NUM_HDMI_INPUTS
#define NEXUS_NUM_HDMI_INPUTS 0
#endif
#if NEXUS_NUM_HDDVI_INPUTS
                if (NEXUS_NUM_HDDVI_INPUTS) {
                    settings.sourceSettings.hdDvi.index = rand()%NEXUS_NUM_HDDVI_INPUTS;
                }
                else 
#endif
#if NEXUS_NUM_HDMI_INPUTS
                if (NEXUS_NUM_HDMI_INPUTS) {
                    settings.sourceSettings.hdDvi.index = rand()%NEXUS_NUM_HDMI_INPUTS;
                }
                else 
#endif
                {
                    settings.sourceSettings.hdDvi.index = 0;
                    expect_failure = true;
                }
                settings.sourceSettings.hdDvi.vertSyncClock = rand();
                settings.sourceSettings.hdDvi.format = rand()%NEXUS_VideoFormat_eMax;
                /* TODO: NEXUS_VideoFrameRate_e14_985 and NEXUS_VideoFrameRate_e7_493 not supported yet */
                settings.sourceSettings.hdDvi.frameRate = rand()%NEXUS_VideoFrameRate_e14_985;
                settings.sourceSettings.hdDvi.trackRange = rand()%NEXUS_TimebaseTrackRange_eMax;
                break;
            case NEXUS_TimebaseSourceType_eCcir656In:
#ifndef NEXUS_NUM_656_INPUTS
#define NEXUS_NUM_656_INPUTS 0
#endif
                settings.sourceSettings.ccir656.index = NEXUS_NUM_656_INPUTS ? rand()%NEXUS_NUM_656_INPUTS : 0;
                settings.sourceSettings.ccir656.format = rand()%NEXUS_VideoFormat_eMax;
                settings.sourceSettings.ccir656.trackRange = rand()%NEXUS_TimebaseTrackRange_eMax;
                break;
            case NEXUS_TimebaseSourceType_eI2sIn:
#ifndef NEXUS_NUM_I2S_INPUTS
#define NEXUS_NUM_I2S_INPUTS 0
#endif
                expect_failure = !NEXUS_NUM_I2S_INPUTS;
                settings.sourceSettings.i2s.index = NEXUS_NUM_I2S_INPUTS ? rand()%NEXUS_NUM_I2S_INPUTS : 0;
                switch (rand()%3) {
                case 0: settings.sourceSettings.i2s.sampleRate = 48000; break;
                case 1: settings.sourceSettings.i2s.sampleRate = 44100; break;
                case 2: settings.sourceSettings.i2s.sampleRate = 32000; break;
                }
                settings.sourceSettings.i2s.trackRange = rand()%NEXUS_TimebaseTrackRange_eMax;
                break;
            case NEXUS_TimebaseSourceType_eSpdifIn:
#ifndef NEXUS_NUM_SPDIF_INPUTS
#define NEXUS_NUM_SPDIF_INPUTS 0
#endif
                /* no settings */
                expect_failure = !NEXUS_NUM_SPDIF_INPUTS;
                break;
            default:
                break;    
            }
            rc = NEXUS_Timebase_SetSettings(timebase, &settings);
            if (expect_failure) 
                BDBG_ASSERT(rc);
            else        
                BDBG_ASSERT(!rc);
            break;
        case 1:
            rc = NEXUS_Timebase_GetStatus(timebase, &status);
            BDBG_ASSERT(!rc);
            break;
        case 2:
            rc = NEXUS_Timebase_ResetStatus(timebase);
            BDBG_ASSERT(!rc);
            break;
        case 3:
            /* TODO: NEXUS_VideoFrameRate_e14_985 and NEXUS_VideoFrameRate_e7_493 not supported yet */
            rc = NEXUS_Timebase_SetHdDviFrameRate(timebase, rand()%NEXUS_VideoFrameRate_e14_985);
            BDBG_ASSERT(!rc);
            break;
        case 4:
            /* TODO: NEXUS_VideoFrameRate_e14_985 and NEXUS_VideoFrameRate_e7_493 not supported yet */
            rc = NEXUS_Timebase_SetVdecFrameRate(timebase, rand()%NEXUS_VideoFrameRate_e14_985);
            BDBG_ASSERT(!rc);
            break;
        }
        if (loops % 100 == 0) BDBG_WRN(("remaining loops %d", loops));
    }
    
    NEXUS_PidChannel_Close(pcrPidChannel);
    NEXUS_Platform_Uninit();

    return 0;
}
