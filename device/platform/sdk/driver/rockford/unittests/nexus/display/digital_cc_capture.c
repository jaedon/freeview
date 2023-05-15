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
 * $brcm_Workfile: digital_cc_capture.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 8/18/11 3:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/digital_cc_capture.c $
 * 
 * Hydra_Software_Devel/8   8/18/11 3:31p jrubio
 * SW7346-11: make sure frontend is default to ON
 * 
 * Hydra_Software_Devel/7   7/8/11 12:18p jtna
 * SW7405-5336: added basic sanity check for incoming streamer input
 * 
 * Hydra_Software_Devel/6   10/25/10 5:35p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/5   7/29/10 10:49a calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/4   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/3   8/4/08 11:40a erickson
 * PR45280: update tests for current 3556
 *
 * Hydra_Software_Devel/2   2/29/08 10:27a jgarrett
 * PR 39435: Fixing includes
 *
 * Hydra_Software_Devel/1   2/4/08 10:39a erickson
 * PR38679: update for 740x
 *
 * 4   1/24/08 12:52p vsilyaev
 * PR 38682: fixed return type of NEXUS_Platform_GetStreamerInputBand
 *
 * 3   1/23/08 12:38p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_video_input_vbi.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_video_window.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_composite_output.h"
#include "nexus_parser_band.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

int main(void)
{
/* without composite outputs, this test has no meaning. */
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStatus videoDecoderStatus;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display0, display1;
    NEXUS_VideoWindowHandle window0, window1;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayVbiSettings displayVbiSettings;
#if CAPTURE_CC
    NEXUS_VideoInputVbiSettings videoInputVbiSettings;
    FILE *foutput;
#endif
    NEXUS_VideoInput videoInput;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif
    
    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
#ifdef IP_STREAMER_SUBST
    IpsAddStcSettings(ips, &stcSettings);
#endif
    videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* bring up display */
#if NEXUS_DTV_PLATFORM
    display0 = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display0, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    window0 = NEXUS_VideoWindow_Open(display0, 0);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    displaySettings.displayType = NEXUS_DisplayType_eBypass;
    display1 = NEXUS_Display_Open(1, &displaySettings);
    window1 = NEXUS_VideoWindow_Open(display1, 0);
    NEXUS_Display_AddOutput(display1, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display0 = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display0, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window0 = NEXUS_VideoWindow_Open(display0, 0);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display1 = NEXUS_Display_Open(1, &displaySettings);
    window1 = NEXUS_VideoWindow_Open(display1, 0);
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display1, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#endif

#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = platformConfig.outputs.audioDacs[0];
    ipsTimebaseSettings.display = display0;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
    ipsTimebaseSettings.display = display1;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    vdecode = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    videoInput = NEXUS_VideoDecoder_GetConnector(vdecode);
    NEXUS_VideoWindow_AddInput(window0, videoInput);
    NEXUS_VideoWindow_AddInput(window1, videoInput);

#define ENCODE_CC 1

#if CAPTURE_CC
    /* enable Read capture */
    NEXUS_VideoInput_GetVbiSettings(videoInput, &videoInputVbiSettings);
    videoInputVbiSettings.closedCaptionEnabled = true;
    NEXUS_VideoInput_SetVbiSettings(videoInput, &videoInputVbiSettings);
#endif

#if ENCODE_CC
    /* enable routing to displays */
    NEXUS_Display_GetVbiSettings(display0, &displayVbiSettings);
    displayVbiSettings.vbiSource = videoInput;
    displayVbiSettings.closedCaptionRouting = true;
    displayVbiSettings.closedCaptionEnabled = true;
    NEXUS_Display_SetVbiSettings(display0, &displayVbiSettings);

    NEXUS_Display_GetVbiSettings(display1, &displayVbiSettings);
    displayVbiSettings.vbiSource = videoInput;
    displayVbiSettings.closedCaptionRouting = true;
    displayVbiSettings.closedCaptionEnabled = true;
    NEXUS_Display_SetVbiSettings(display1, &displayVbiSettings);
#endif

#ifdef IP_STREAMER_SUBST
        ipsStartSettings.vDecoder = vdecode;
        ipsStartSettings.aDecoder = NULL;
        ipsStartSettings.stcChannel = videoProgram.stcChannel;
        IpsStart(ips, &ipsStartSettings);
#endif

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);

#if CAPTURE_CC
    foutput = fopen("vbi.dat", "w+");
    for (;;) {
        NEXUS_ClosedCaptionData ccData;
        unsigned num;
        NEXUS_VideoInput_ReadClosedCaption(videoInput, &ccData, 1, &num);
        if (num) {
            fwrite(ccData.data, 2, 1, foutput);
            continue;
        }

        fflush(foutput);
        BKNI_Sleep(200);
    }
#else
    BKNI_Sleep(10000);
#endif

    NEXUS_VideoDecoder_GetStatus(vdecode, &videoDecoderStatus);
    BDBG_ASSERT(videoDecoderStatus.numDecoded);

    NEXUS_VideoDecoder_Stop(vdecode);
#if CAPTURE_CC
    fclose(foutput);
#endif
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    NEXUS_VideoWindow_RemoveAllInputs(window0);
    NEXUS_VideoWindow_RemoveAllInputs(window1);
    NEXUS_VideoInput_Shutdown(videoInput);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_Display_RemoveAllOutputs(display0);
    NEXUS_Display_RemoveAllOutputs(display1);
    NEXUS_VideoWindow_Close(window0);
    NEXUS_VideoWindow_Close(window1);
    NEXUS_Display_Close(display0);
    NEXUS_Display_Close(display1);
    NEXUS_StcChannel_Close(videoProgram.stcChannel);
    NEXUS_PidChannel_Close(videoProgram.pidChannel);
    NEXUS_Platform_Uninit();

#endif /* NEXUS_NUM_COMPOSITE_OUTPUTS */
    return 0;
}
