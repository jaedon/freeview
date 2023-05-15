/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: pip_swap.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 2/21/12 12:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/pip_swap.c $
 * 
 * Hydra_Software_Devel/6   2/21/12 12:31p erickson
 * SWDEPRECATED-3044: fix printf
 * 
 * Hydra_Software_Devel/5   2/8/12 2:40p erickson
 * SWDEPRECATED-3044: fix test for dual digital decode platforms
 * 
 * Hydra_Software_Devel/4   5/26/10 12:59p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/3   5/12/10 11:51a spothana
 * SW7420-740: Fixing the compilation error.
 * 
 * Hydra_Software_Devel/2   2/2/10 11:15a mward
 * SW7125-195: Don't fail if platform doesn't have PIP. (if
 * NEXUS_NUM_VIDEO_WINDOWS < 2)
 * 
 * Hydra_Software_Devel/1   6/5/09 11:32a erickson
 * PR55427: add pip_swap test
 *
******************************************************************************/
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_core_utils.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>

BDBG_MODULE(pip_swap);

void wait_for_next(void)
{
#if 0
    /* interactive mode */
    printf("Press ENTER for next configuration\n");
    getchar();
#else
    printf("Next configuration in 5 seconds...\n");
    BKNI_Sleep(5000);
#endif
}

int main(void)
{
#if NEXUS_NUM_VIDEO_WINDOWS > 1 && NEXUS_NUM_VIDEO_DECODERS > 1
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Error rc;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand[2];
    NEXUS_VideoWindowSettings windowSettings[2];
    NEXUS_VideoDecoderHandle videoDecoder[2];
    NEXUS_VideoDecoderStartSettings videoProgram[2];
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window[2];
    NEXUS_VideoInput videoInput[2];
    unsigned index;
    NEXUS_Rect mainWindowRect, pipWindowRect;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    window[0] = NEXUS_VideoWindow_Open(display, 0);
    BDBG_ASSERT(window[0]);
    window[1] = NEXUS_VideoWindow_Open(display, 1);
    BDBG_ASSERT(window[1]);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);
    
    /* start digital */
    for (index = 0; index < 2; index++) {
        NEXUS_ParserBandSettings parserBandSettings;
        NEXUS_StcChannelSettings stcSettings;
        NEXUS_VideoDecoderSettings videoDecoderSettings;
        
        parserBand[index] = NEXUS_ParserBand_e0 + index;
        
        NEXUS_ParserBand_GetSettings(parserBand[index], &parserBandSettings);
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        parserBandSettings.sourceTypeSettings.inputBand = inputBand;
        parserBandSettings.transportType = NEXUS_TransportType_eTs;
        rc = NEXUS_ParserBand_SetSettings(parserBand[index], &parserBandSettings);
        BDBG_ASSERT(!rc);
        
        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[index]);
        videoProgram[index].codec = NEXUS_VideoCodec_eMpeg2;
        videoProgram[index].pidChannel = NEXUS_PidChannel_Open(parserBand[index], index?0x11:0x31, NULL);
        NEXUS_StcChannel_GetDefaultSettings(index, &stcSettings);
        stcSettings.timebase = (NEXUS_Timebase)((unsigned)NEXUS_Timebase_e0 + index);
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        stcSettings.modeSettings.pcr.pidChannel = videoProgram[index].pidChannel; /* PCR on video pid */
        videoProgram[index].stcChannel = NEXUS_StcChannel_Open(index, &stcSettings);
        videoDecoder[index] = NEXUS_VideoDecoder_Open(index, NULL); /* take default capabilities */
        BDBG_ASSERT(videoDecoder[index]);
        
        NEXUS_VideoDecoder_GetSettings(videoDecoder[index], &videoDecoderSettings);
        videoDecoderSettings.manualPowerState = true; /* allows swap w/o stop/start */
        rc = NEXUS_VideoDecoder_SetSettings(videoDecoder[index], &videoDecoderSettings);
        BDBG_ASSERT(!rc);
        
        videoInput[index] = NEXUS_VideoDecoder_GetConnector(videoDecoder[index]);
        BDBG_ASSERT(videoInput[index]);
        
        rc = NEXUS_VideoWindow_AddInput(window[index], videoInput[index]);
        BDBG_ASSERT(!rc);
        rc = NEXUS_VideoDecoder_Start(videoDecoder[index], &videoProgram[index]);
        BDBG_ASSERT(!rc);
        
        NEXUS_VideoWindow_GetSettings(window[index], &windowSettings[index]);
    }

    mainWindowRect = windowSettings[0].position;
    pipWindowRect = windowSettings[1].position;

    printf("Current mode: digital in main, analog in PIP.\n");
    wait_for_next();

    windowSettings[1].position = mainWindowRect;
    windowSettings[1].zorder = 0;
    windowSettings[0].position = pipWindowRect;
    windowSettings[0].zorder = 1;
    /* to swap zorder, we must defer the BVDC_ApplyChanges */
    NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eManual);
    rc = NEXUS_VideoWindow_SetSettings(window[0], &windowSettings[0]);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_SetSettings(window[1], &windowSettings[1]);
    BDBG_ASSERT(!rc);
    NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eAuto);

    printf("Current mode: zorder & position swap.\n");
    wait_for_next();

    windowSettings[0].position = mainWindowRect;
    windowSettings[0].zorder = 0;
    windowSettings[1].position = pipWindowRect;
    windowSettings[1].zorder = 1;
    NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eManual);
    rc = NEXUS_VideoWindow_SetSettings(window[0], &windowSettings[0]);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_SetSettings(window[1], &windowSettings[1]);
    BDBG_ASSERT(!rc);
    NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eAuto);
    printf("Current mode: no swap.\n");
    wait_for_next();

    /* must stop digital decode before switching inputs to windows */
    NEXUS_VideoWindow_RemoveAllInputs(window[0]);
    NEXUS_VideoWindow_RemoveAllInputs(window[1]);
    rc = NEXUS_VideoWindow_AddInput(window[0], videoInput[1]);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_AddInput(window[1], videoInput[0]);
    BDBG_ASSERT(!rc);
    printf("Current mode: videoInput swap.\n");
    wait_for_next();

    NEXUS_VideoWindow_RemoveAllInputs(window[0]);
    NEXUS_VideoWindow_RemoveAllInputs(window[1]);
    rc = NEXUS_VideoWindow_AddInput(window[0], videoInput[0]);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_AddInput(window[1], videoInput[1]);
    BDBG_ASSERT(!rc);
    printf("Current mode: no swap.\n");
    wait_for_next();

    for (index=0;index<2;index++) {
        NEXUS_VideoDecoder_Stop(videoDecoder[index]);
        NEXUS_VideoInput_Shutdown(videoInput[index]);
        NEXUS_VideoDecoder_Close(videoDecoder[index]);
        NEXUS_PidChannel_Close(videoProgram[index].pidChannel);
        NEXUS_StcChannel_Close(videoProgram[index].stcChannel);
        NEXUS_VideoWindow_Close(window[index]);
    }
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();
#else
    printf("can't test NEXUS_NUM_VIDEO_WINDOWS=%d NEXUS_NUM_VIDEO_DECODERS=%d\n", NEXUS_NUM_VIDEO_WINDOWS, NEXUS_NUM_VIDEO_DECODERS);
#endif /* NEXUS_NUM_VIDEO_WINDOWS > 1 */
    return 0;
}

