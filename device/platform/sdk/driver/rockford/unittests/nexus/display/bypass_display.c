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
 * $brcm_Workfile: bypass_display.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 4/13/09 12:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/bypass_display.c $
 * 
 * Hydra_Software_Devel/6   4/13/09 12:52p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/5   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/4   10/6/08 10:22p erickson
 * PR40799: extended test
 *
 * Hydra_Software_Devel/3   8/4/08 11:40a erickson
 * PR45280: update tests for current 3556
 *
 * Hydra_Software_Devel/2   7/9/08 12:08p erickson
 * PR42679: update Makefile
 *
 * Hydra_Software_Devel/1   5/14/08 2:30p erickson
 * PR39257: add bypass_display test
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_analog_video_decoder.h"
#else
#include "nexus_video_decoder.h"
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
#include "nexus_component_output.h"
#else
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#endif
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
/* if there's no composite, this test has no meaning */
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_DisplayHandle display, bypassDisplay;
    NEXUS_DisplaySettings displayCfg;
    NEXUS_VideoWindowHandle window0, window1;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
#else
    NEXUS_VideoDecoderHandle videoDecoder;
#endif
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoInput videoInput;
    NEXUS_Error rc;
    unsigned loops2 = 3;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displayCfg);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    displayCfg.displayType = NEXUS_DisplayType_eAuto;
    displayCfg.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displayCfg);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#else
    display = NEXUS_Display_Open(0, &displayCfg);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#endif

    /* Bring up bypass display for simultaneous composite output of analog source.
    Please note that the composite output is routed to the Y-component output on the 93563. */
    NEXUS_Display_GetDefaultSettings(&displayCfg);
    displayCfg.displayType = NEXUS_DisplayType_eBypass;
    displayCfg.format = NEXUS_VideoFormat_eNtsc;
    bypassDisplay = NEXUS_Display_Open(1, &displayCfg);
    BDBG_ASSERT(bypassDisplay);
    rc = NEXUS_Display_AddOutput(bypassDisplay, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);

    window0 = NEXUS_VideoWindow_Open(display, 0);
    window1 = NEXUS_VideoWindow_Open(bypassDisplay, 0);

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);
    videoInput = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);
#else
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    videoInput = NEXUS_VideoDecoder_GetConnector(videoDecoder);
#endif

    while (--loops2) {
        unsigned loops = 10;
        rc = NEXUS_VideoWindow_AddInput(window0, videoInput);
        BDBG_ASSERT(!rc);
        rc = NEXUS_VideoWindow_AddInput(window1, videoInput);
        BDBG_ASSERT(!rc);

        while (--loops) {
            printf("NTSC\n");
            NEXUS_Display_GetSettings(bypassDisplay, &displayCfg);
            displayCfg.format = NEXUS_VideoFormat_eNtsc;
            rc = NEXUS_Display_SetSettings(bypassDisplay, &displayCfg);
            BDBG_ASSERT(!rc);

            printf("PAL\n");
            NEXUS_Display_GetSettings(bypassDisplay, &displayCfg);
            displayCfg.format = NEXUS_VideoFormat_ePal;
            rc = NEXUS_Display_SetSettings(bypassDisplay, &displayCfg);
            BDBG_ASSERT(!rc);

#if BCHP_CHIP == 3548 || BCHP_CHIP == 3556
            printf("SECAM\n");
            NEXUS_Display_GetSettings(bypassDisplay, &displayCfg);
            displayCfg.format = NEXUS_VideoFormat_eSecam;
            rc = NEXUS_Display_SetSettings(bypassDisplay, &displayCfg);
            BDBG_ASSERT(!rc);
#endif
        }

        NEXUS_VideoWindow_RemoveAllInputs(window0);
        NEXUS_VideoWindow_RemoveAllInputs(window1);
    }

#endif
    return 0;
}

