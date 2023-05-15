/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: addinput_manualpowerstate.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/6/12 2:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/addinput_manualpowerstate.c $
 * 
 * Hydra_Software_Devel/1   9/6/12 2:11p erickson
 * SW7405-3276: add test for addinput/removeinput and manualPowerState
 * 
 * 11   1/26/12 11:23a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 10   6/24/11 10:57a erickson
 * SW7231-199: add hdmi output
 * 
 * 9   1/7/11 3:26p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 8   2/4/09 3:50p erickson
 * PR51841: refactor
 *
 * 7   10/16/08 9:31a erickson
 * PR47967: add panel support
 *
 * 6   6/19/08 2:33p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 5   6/7/08 1:39p jgarrett
 * PR 43318: Enabling display on DTV platforms without composite outputs.
 *
 * 4   2/25/08 9:34a jgarrett
 * PR 39435: Fixing includes
 *
 * 3   1/24/08 12:52p vsilyaev
 * PR 38682: fixed return type of NEXUS_Platform_GetStreamerInputBand
 *
 * 2   1/23/08 12:38p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 *
******************************************************************************/
/* Nexus example app: single live video decode from an input band */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>

BDBG_MODULE(decode_video);

/* the following define the input and its characteristics -- these will vary by input */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define VIDEO_PID 0x11

#define TOTAL_LOOPS 10

void b_getchar(void)
{
#if 0
    getchar();
#else
    BKNI_Sleep(200);
#endif
}

int main(void)
{
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Error rc;
    unsigned loops;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return rc;
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
    videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    
    /* scenario 1: start/stop video with default manualPowerState. */
    for (loops=0;loops<TOTAL_LOOPS;loops++) {
        static bool stopBeforeRemove = true;

        /* must call AddInput before Start */        
        rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
        BDBG_ASSERT(!rc);
        rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
        BDBG_ASSERT(!rc);
    
        b_getchar();
        
        /* if !manualPowerState, we are automatically stopped on RemoveInput */        
        if (stopBeforeRemove) {
            BDBG_WRN(("stop before remove"));
            NEXUS_VideoDecoder_Stop(vdecode);
        }
        else {
            BDBG_WRN(("don't stop before remove"));
        }
        rc = NEXUS_VideoWindow_RemoveInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
        BDBG_ASSERT(!rc);
        /* calling Stop after RemoveInput is harmless, but puts an ERR on the console because decoder is already stopped */
        stopBeforeRemove != stopBeforeRemove;
        
        b_getchar();
    }
    
    /* scenario 2: start/stop video with manualPowerState = true. */
    {
        NEXUS_VideoDecoderSettings settings;
        NEXUS_VideoDecoder_GetSettings(vdecode, &settings);
        settings.manualPowerState = true;
        rc = NEXUS_VideoDecoder_SetSettings(vdecode, &settings);
        BDBG_ASSERT(!rc);
    }
    for (loops=0;loops<TOTAL_LOOPS;loops++) {
        static bool stopBeforeRemove = true;

        /* must addinput before start, even with manualPowerState */
        rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
        BDBG_ASSERT(!rc);
        rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
        BDBG_ASSERT(!rc);
    
        b_getchar();

        /* if manualPowerState, we are not automatically stopped. we can stop in either order, but we must stop. */
        if (stopBeforeRemove) {
            BDBG_WRN(("stop before remove"));
            NEXUS_VideoDecoder_Stop(vdecode);
        }
        rc = NEXUS_VideoWindow_RemoveInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
        BDBG_ASSERT(!rc);
        if (!stopBeforeRemove) {
            BDBG_WRN(("stop after remove"));
            NEXUS_VideoDecoder_Stop(vdecode);
        }
        stopBeforeRemove = !stopBeforeRemove;
        
        b_getchar();
    }
    
    /* scenario 2: add/remove input with manualPowerState = true. no stop/start needed. */
    {
        NEXUS_VideoDecoderSettings settings;
        NEXUS_VideoDecoder_GetSettings(vdecode, &settings);
        settings.manualPowerState = true;
        rc = NEXUS_VideoDecoder_SetSettings(vdecode, &settings);
        BDBG_ASSERT(!rc);
    }
    /* another option with manualPowerState is to not stop/start. just add/remove input */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    for (loops=0;loops<TOTAL_LOOPS;loops++) {
        b_getchar();
        rc = NEXUS_VideoWindow_RemoveInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
        BDBG_ASSERT(!rc);
        b_getchar();
        rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
        BDBG_ASSERT(!rc);
    }
    
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_PidChannel_Close(videoProgram.pidChannel);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();    
    
    return 0;
}
