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
 * $brcm_Workfile: dynamic_backlight.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 4/15/10 2:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/dynamic_backlight.c $
 * 
 * Hydra_Software_Devel/5   4/15/10 2:39p erickson
 * SW3548-2854: refactor backlight so it takes the min of the user value
 * and the dynamic contrast value
 *
 * Hydra_Software_Devel/4   2/16/10 4:41p erickson
 * SW3548-2776: extend test
 *
 * Hydra_Software_Devel/3   2/2/10 5:35p erickson
 * SWGIGGS2S-254: revise test
 *
 * Hydra_Software_Devel/2   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   9/5/08 1:11p erickson
 * PR45897: added test app
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
#include "nexus_video_input.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_picture_ctrl.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(dynamic_backlight);

int main(void)
{
#if NEXUS_DTV_PLATFORM
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PictureCtrlContrastStretch contrastStretch;
    NEXUS_Error rc;
    unsigned i;

    srand(time(NULL));

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
    videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* bring up display */
    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
    BDBG_ASSERT(!rc);

    /* Start Decoders */
    rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    BDBG_ASSERT(!rc);

    for (i=0;i<500;i++) {
        NEXUS_PanelOutputSettings panelSettings;

        switch (rand() % 3) {
        case 0:
            NEXUS_PictureCtrl_GetContrastStretch(window, &contrastStretch);
            contrastStretch.enabled = rand() % 2;
            contrastStretch.dynamicBacklightControl = rand() % 2;
            contrastStretch.dynamicBacklightScaleFactor = rand() % NEXUS_BACKLIGHT_SCALE_FACTOR;
            rc = NEXUS_PictureCtrl_SetContrastStretch(window, &contrastStretch);
            BDBG_ASSERT(!rc);
            BDBG_WRN(("set backlight control %d, scale %d/%d", contrastStretch.dynamicBacklightControl, contrastStretch.dynamicBacklightScaleFactor, NEXUS_BACKLIGHT_SCALE_FACTOR));
            break;
        case 1:
            /* user can reset during the middle */
            NEXUS_PanelOutput_GetSettings(platformConfig.outputs.panel[0], &panelSettings);
            panelSettings.backlightLevel = rand() % NEXUS_BACKLIGHT_SCALE_FACTOR;
            rc = NEXUS_PanelOutput_SetSettings(platformConfig.outputs.panel[0], &panelSettings);
            BDBG_ASSERT(!rc);
            BDBG_WRN(("set backlight scale %d/%d", panelSettings.backlightLevel, NEXUS_BACKLIGHT_SCALE_FACTOR));
            break;
        case 2:
            BKNI_Sleep(rand() % 100);
            break;
        }
    }

    NEXUS_VideoDecoder_Stop(vdecode);

    /* TODO: this is required for now because of an implementation shortcut */
    NEXUS_PictureCtrl_GetContrastStretch(window, &contrastStretch);
    contrastStretch.dynamicBacklightControl = false;
    rc = NEXUS_PictureCtrl_SetContrastStretch(window, &contrastStretch);
    BDBG_ASSERT(!rc);

    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(videoProgram.stcChannel);
    NEXUS_PidChannel_Close(videoProgram.pidChannel);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}
