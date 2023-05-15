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
 * $brcm_Workfile: colorclip.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 4/6/09 2:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/colorclip.c $
 * 
 * Hydra_Software_Devel/4   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/3   3/23/09 1:31p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/2   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   9/22/08 4:44p erickson
 * PR47190: added colorclip app
 *
 * 15   7/18/08 11:31a jgarrett
 * PR 44953: Removing -Wstrict-prototypes warnings
 *
 * 14   7/14/08 3:29p erickson
 * PR44792: add sample shutdown code
 *
 * 13   7/7/08 2:40p erickson
 * PR44619: remove duplicate NEXUS_PanelOutput_Open calls.
 *
 * 12   6/20/08 5:48p vsilyaev
 * PR 40921: Use NTSC resolution if no component output
 *
 * 11   6/19/08 2:44p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 10   6/19/08 10:46a erickson
 * PR42678: switch to output_type=panel|component
 *
 * 9   6/7/08 1:38p jgarrett
 * PR 43318: Allowing display with no composite outputs
 *
 * 8   5/21/08 10:33a erickson
 * PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
 *
 * 7   4/23/08 11:35a erickson
 * PR35457: rearrange example for simpler explanation
 *
 * 6   4/1/08 11:39a erickson
 * PR38442: use is_panel_output() for consistent DVO output selection
 *
 * 5   2/25/08 9:28a jgarrett
 * PR 39435: fixing includes
 *
 * 4   1/24/08 12:52p vsilyaev
 * PR 38682: fixed return type of NEXUS_Platform_GetStreamerInputBand
 *
 * 3   1/23/08 12:38p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 *
******************************************************************************/
/* Nexus example app: single live a/v decode from an input band */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#include "nexus_picture_ctrl.h"
#endif

int main(void)
{
#if NEXUS_DTV_PLATFORM
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_PictureCtrlColorClipSettings colorClipSettings;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);

    NEXUS_PictureCtrl_GetColorClipSettings(display, &colorClipSettings);
    /* Default color clip settings for BT709
       [Note 1] bit shift to compensate for difference between HW and API */
    colorClipSettings.crYSlopeA = 0x263 << 8; /* [Note 1] */
    colorClipSettings.crYSlopeB = 0x0A7 << 8; /* [Note 1] */
    colorClipSettings.cbYSlopeA = 0x700 << 8; /* [Note 1] */
    colorClipSettings.cbYSlopeB = 0x08E << 8; /* [Note 1] */
    colorClipSettings.crJoint = 47;
    colorClipSettings.cbJoint = 16;
#if 0
    /* don't exist in VDC */
    colorClipSettings.extendedWhite = false;
    colorClipSettings.extendedBlack = false;
#endif
    colorClipSettings.colorClipMode = NEXUS_ColorClipMode_eNone;
    rc = NEXUS_PictureCtrl_SetColorClipSettings(display, &colorClipSettings);
    BDBG_ASSERT(!rc);

    BKNI_Sleep(10000);
#endif

    return 0;
}
