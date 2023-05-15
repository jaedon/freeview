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
 * $brcm_Workfile: dtv_pip.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 7/29/10 10:55a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/dtv_pip.c $
 * 
 * Hydra_Software_Devel/11   7/29/10 10:55a calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/10   5/10/10 6:55p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 * 
 * Hydra_Software_Devel/9   6/5/09 11:30a erickson
 * PR55427: fix app
 *
 * Hydra_Software_Devel/8   4/13/09 12:52p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/7   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/6   1/30/09 2:59p erickson
 * PR44764: remove external VDB support. nexus now supports VDB and
 * dynamic RTS internally.
 *
 * Hydra_Software_Devel/5   10/14/08 3:28p rgreen
 * PR46176: Power management support; Move frontend information inside of
 * hdm object
 *
 * Hydra_Software_Devel/4   7/18/08 3:24p erickson
 * PR44764: add more formats, add source pending, fix input types
 *
 * Hydra_Software_Devel/3   7/17/08 10:07a erickson
 * PR44764: impl PIP and PBP, decoder/component/hdmi 0 and 1
 *
 * Hydra_Software_Devel/2   7/11/08 4:04p erickson
 * PR44764: fix use of BVDB_RtsTable.ulId, remove workaround
 *
 * Hydra_Software_Devel/1   7/11/08 3:51p erickson
 * PR44764: initial checkin
 *
******************************************************************************/
#include "nexus_platform.h"

#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_core_utils.h"

#if NEXUS_NUM_VIDEO_DECODERS
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#endif
#if NEXUS_NUM_HDMI_INPUTS
#include "nexus_hdmi_input.h"
#endif
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_analog_video_decoder.h"
#include "nexus_component_input.h"
#endif

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

#ifdef IP_STREAMER_SUBST
IpsHandle ips;
#endif

BDBG_MODULE(dtv_pip);

#define MAX_INPUTS 4
NEXUS_VideoDecoderHandle videoDecoder[1];
NEXUS_VideoDecoderStartSettings videoProgram[1];
NEXUS_DisplayHandle display;
NEXUS_VideoWindowHandle window0, window1;
struct input_t {
    NEXUS_VideoInput videoInput;
    const char *videoInputType;
} input[MAX_INPUTS];

#if NEXUS_NUM_HDMI_INPUTS
NEXUS_HdmiInputHandle hdmiInput[2];
#endif
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
#endif
int g_mainSource, g_pipSource;

NEXUS_VideoInput open_video_decoder(int index, unsigned short pid)
{
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;

    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
#ifdef IP_STREAMER_SUBST
    IpsAddParserBandSettings(ips, &parserBandSettings);
#endif    
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[index]);
    videoProgram[index].codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram[index].pidChannel = NEXUS_PidChannel_Open(parserBand, pid, NULL);

    NEXUS_StcChannel_GetDefaultSettings(index, &stcSettings);
    stcSettings.timebase = (NEXUS_Timebase) (NEXUS_Timebase_e0 + index);
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram[index].pidChannel; /* PCR on video pid */
#ifdef IP_STREAMER_SUBST
    IpsAddStcSettings(ips, &stcSettings);
#endif
    videoProgram[index].stcChannel = NEXUS_StcChannel_Open(index, &stcSettings);

    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    videoDecoder[index] = NEXUS_VideoDecoder_Open(index, &videoDecoderOpenSettings);

    return NEXUS_VideoDecoder_GetConnector(videoDecoder[index]);
}

NEXUS_VideoInput open_hdmi_input(int index)
{
#if NEXUS_NUM_HDMI_INPUTS
    NEXUS_HdmiInputSettings hdmiInputSettings;

    NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
    hdmiInputSettings.timebase = NEXUS_Timebase_e0;

    hdmiInput[index] = NEXUS_HdmiInput_Open(index, &hdmiInputSettings);
    BDBG_ASSERT(hdmiInput[index]);

    return NEXUS_HdmiInput_GetVideoConnector(hdmiInput[index]);
#else
    BSTD_UNUSED(index);
    return NULL;
#endif
}

NEXUS_VideoInput open_component_input(int index)
{
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;

    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);

    NEXUS_Platform_GetConfiguration(&platformConfig);
    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_ComponentInput_GetConnector(platformConfig.inputs.component[index]), &analogVideoDecoderSettings);
    NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);

    return NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);
#else
    BSTD_UNUSED(index);
    return NULL;
#endif
}

void set_sources(unsigned mainSource, unsigned pipSource)
{
    NEXUS_Error rc;

    g_mainSource = mainSource;
    g_pipSource = pipSource;

    if (input[mainSource].videoInput) {
        rc = NEXUS_VideoWindow_AddInput(window0, input[mainSource].videoInput);
        BDBG_ASSERT(!rc);
    }
    if (input[pipSource].videoInput) {
        rc = NEXUS_VideoWindow_AddInput(window1, input[pipSource].videoInput);
        BDBG_ASSERT(!rc);
    }

    if (mainSource == 0 || pipSource == 0) {
        rc = NEXUS_VideoDecoder_Start(videoDecoder[0], &videoProgram[0]);
        BDBG_ASSERT(!rc);
    }
}

void unset_sources(void)
{
    NEXUS_VideoDecoder_Stop(videoDecoder[0]);
    NEXUS_VideoWindow_RemoveAllInputs(window0);
    NEXUS_VideoWindow_RemoveAllInputs(window1);
}

void wait_for_next(void)
{
#if 0
    /* interactive mode */
    printf("Press ENTER to try another configuration\n");
    getchar();
#else
    printf("Next configuration in 5 seconds...\n");
    BKNI_Sleep(5000);
#endif
}

int main(void)
{
#if NEXUS_NUM_VIDEO_WINDOWS > 1
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoFormatInfo displayFormat;
    NEXUS_DisplaySettings displaySettings;
    unsigned mode;
    NEXUS_Error rc;
    unsigned i;
#ifdef IP_STREAMER_SUBST
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
    IpsOpenSettings ipsOpenSettings;
#endif

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* bring up display */
    display = NEXUS_Display_Open(0, NULL);
    window0 = NEXUS_VideoWindow_Open(display, 0);
    window1 = NEXUS_VideoWindow_Open(display, 1);
    BDBG_ASSERT(window1);
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif

#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = NULL;
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    /* get format */
    NEXUS_Display_GetSettings(display, &displaySettings);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormat);

#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* bring up inputs */
    input[0].videoInput = open_video_decoder(0, 0x31);
    input[0].videoInputType = "decoder";

    input[1].videoInput = open_component_input(0);
    input[1].videoInputType = "component";

    input[2].videoInput = open_hdmi_input(0);
    input[2].videoInputType = "hdmi0";

    input[3].videoInput = open_hdmi_input(1);
    input[3].videoInputType = "hdmi1";


#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    for (mode=0; mode<2; mode++) {
        NEXUS_VideoWindowSettings window0Settings, window1Settings;

        NEXUS_VideoWindow_GetSettings(window0, &window0Settings);
        NEXUS_VideoWindow_GetSettings(window1, &window1Settings);
        switch (mode%2) {
        case 0: /* PIP */
            window0Settings.position.x = 0;
            window0Settings.position.y = 0;
            window0Settings.position.width = displayFormat.width;
            window0Settings.position.height = displayFormat.height;
            window1Settings.position.x = displayFormat.width/2;
            window1Settings.position.y = 0;
            window1Settings.position.width = displayFormat.width/2;
            window1Settings.position.height = displayFormat.height/2;
            break;
        case 1: /* side-by-side */
            window0Settings.position.x = 0;
            window0Settings.position.y = 0;
            window0Settings.position.width = displayFormat.width/2;
            window0Settings.position.height = displayFormat.height;
            window1Settings.position.x = displayFormat.width/2;
            window1Settings.position.y = 0;
            window1Settings.position.width = displayFormat.width/2;
            window1Settings.position.height = displayFormat.height;
            break;
        }
        rc = NEXUS_VideoWindow_SetSettings(window0, &window0Settings);
        BDBG_ASSERT(!rc);
        rc = NEXUS_VideoWindow_SetSettings(window1, &window1Settings);
        BDBG_ASSERT(!rc);

        set_sources(0, 1);
        wait_for_next();
        unset_sources();

        set_sources(1, 0);
        wait_for_next();
        unset_sources();

        set_sources(2, 0);
        wait_for_next();
        unset_sources();

        set_sources(0, 2);
        wait_for_next();
        unset_sources();

        set_sources(1, 2);
        wait_for_next();
        unset_sources();

        set_sources(2, 1);
        wait_for_next();
        unset_sources();

        set_sources(2, 3);
        wait_for_next();
        unset_sources();

        set_sources(3, 2);
        wait_for_next();
        unset_sources();
    }

    printf("Exiting...\n");

    for (i=0;i<4;i++) {
        if (input[i].videoInput) {
            NEXUS_VideoInput_Shutdown(input[i].videoInput);
        }
    }
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
#endif
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(videoDecoder[0]);
#if NEXUS_NUM_HDMI_INPUTS
    NEXUS_HdmiInput_Close(hdmiInput[0]);
    NEXUS_HdmiInput_Close(hdmiInput[1]);
#endif
    NEXUS_PidChannel_Close(videoProgram[0].pidChannel);
    NEXUS_StcChannel_Close(videoProgram[0].stcChannel);

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    NEXUS_Platform_Uninit();
#endif
    return 0;
}

