/***************************************************************************
*     (c)2004-2008 Broadcom Corporation
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
* $brcm_Workfile: hdmi_input.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 5/14/08 10:07a $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/hdmi_input/hdmi_input.c $
* 
* Hydra_Software_Devel/2   5/14/08 10:07a erickson
* PR42679: eliminate duplicate code in unittest Makefiles
*
* Hydra_Software_Devel/1   4/24/08 10:27a erickson
* PR40410: added hdmi_input test
*
* PR40410/1   4/22/08 1:56p honglang
* PR40410: Add test code to verify HdDviSettings in HdmiInput interface
*
* 5   4/1/08 11:39a erickson
* PR38442: use is_dvo_mode() for consistent DVO output selection
*
* 4   3/26/08 10:51a erickson
* PR40742: added more status info
*
* 3   1/23/08 9:22p vobadm
* PR35457: update docs
*
* 2   1/23/08 12:39p erickson
* PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
*
* 1   1/18/08 2:13p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/11   12/11/07 4:13p jgarrett
* PR 38107: Removing non-portable strings.h
*
* Nexus_Devel/10   11/30/07 3:57p erickson
* PR34925: rename stc_channel
*
* Nexus_Devel/9   11/30/07 2:14p jgarrett
* PR 27938: Adding DTV specifics
*
* Nexus_Devel/8   11/30/07 12:22p jgarrett
* PR 37778: Using kylin variable name for dvo vs component
*
* Nexus_Devel/7   11/29/07 5:58p jgarrett
* PR 37778: Standardizing DTV apps
*
* Nexus_Devel/6   11/29/07 10:32a jgarrett
* PR 37471: Adding audio, refactoring to use platform config
*
* Nexus_Devel/5   11/19/07 1:20p erickson
* PR34925: split demux.h into separate interface files
*
* Nexus_Devel/4   11/16/07 3:32p erickson
* PR36814: added NEXUS_HdmiInput_SetHdcpKeySet call
*
* Nexus_Devel/3   11/15/07 9:31a erickson
* PR34662: added #include nexus_video_window. removed unnecessary
* windowSettings.visible = true.
*
* Nexus_Devel/2   11/9/07 3:16p erickson
* PR36814: added config of timebase
*
* Nexus_Devel/1   11/8/07 2:39p erickson
* PR36814: add hdmi_input
*
***************************************************************************/

/* Nexus example app: single live video decode from 3563 tuner */

#if NEXUS_DTV_PLATFORM
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_dvo_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_hdmi_input.h"
#include "nexus_video_hdmi_input.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_output.h"
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "nexus_platform_boardcfg.h"

void source_changed(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    printf("source changed\n");
}

void avmute_changed(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    printf("avmute changed\n");
}

bool is_dvo_mode()
{
    const char *pEnvVar = getenv("hdmi_dvo_port");
    return pEnvVar && (pEnvVar[0] == 'y' || pEnvVar[0] == 'Y');
}
#endif

int main(void)
{
#if NEXUS_DTV_PLATFORM
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displayCfg;
    NEXUS_VideoWindowHandle window;
    NEXUS_ComponentOutputHandle component;
    NEXUS_HdmiInputHandle hdmiInput;
    NEXUS_HdmiInputSettings hdmiInputSettings;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_StcChannelSettings stcSettings;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eHdDviIn;
    NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);

    NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
    hdmiInputSettings.timebase = NEXUS_Timebase_e0;
    hdmiInputSettings.sourceChanged.callback = source_changed;
    hdmiInputSettings.avMuteChanged.callback = avmute_changed;
    hdmiInput = NEXUS_HdmiInput_Open(0, &hdmiInputSettings);
    if(!hdmiInput) {
        fprintf(stderr, "Can't get hdmi input\n");
        return -1;
    }

    NEXUS_HdmiInput_SetHdcpKeySet(hdmiInput, NULL /* use reference key set for test */);

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displayCfg);
    if ( !is_dvo_mode() )
    {
        displayCfg.displayType = NEXUS_DisplayType_eAuto;
        displayCfg.format = NEXUS_VideoFormat_e720p;
    }

    displayCfg.background = 0xFF00FF00;
    display = NEXUS_Display_Open(0, &displayCfg);

    if ( is_dvo_mode() )
    {
        NEXUS_Display_AddOutput(display, NEXUS_DvoOutput_GetConnector(platformConfig.outputs.dvo[0]));
        #if NEXUS_DTV_PLATFORM
        NEXUS_BoardCfg_ConfigurePanel(true, true, true);
        #endif
    }
    else
    {
        component = platformConfig.outputs.component[0];
        NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(component));
    }

    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.input = NEXUS_HdmiInput_GetAudioConnector(hdmiInput);
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.autoConfigTimebase = false;
    audioProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
                               NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    fprintf(stderr, "Panel output ready\n");
    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoWindow_AddInput(window, NEXUS_HdmiInput_GetVideoConnector(hdmiInput));

    fprintf(stderr, "Starting Audio Decoder\n");
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

    while (1) {
        NEXUS_HdmiInputStatus status;

        NEXUS_HdmiInput_GetStatus(hdmiInput, &status);

        printf(
            "HdmiInput Status\n"
            "  original format: %d\n"
            "  interlaced:      %c\n"
            "  noSignal:        %c\n",
            status.originalFormat,
            status.interlaced?'y':'n',
            status.noSignal?'y':'n');


        BKNI_Sleep(1000);

        /*
            To customize the below test items :
            - auto format and video position
            - auto CSC
            The above features are not used in Kylin for now, but will be used by customers,
            so the test example is very important.
       */
        char cmdChar[5] = {0};
        int cmdInt;
        static bool flagVideoFormat = false;
        static bool flagCSC = false;
        static bool firstRun = true;

        if (firstRun)
        {
            firstRun = false;
            printf("Enable auto video format testing ? (Y to enable, or any other key to disable)\n");
            memset(cmdChar, 0, 5);
            read(0, cmdChar, 5);
            if ((cmdChar[0] == 'y') || (cmdChar[0] == 'Y')) flagVideoFormat = true;

            printf("Enable auto CSC testing ? (Y to enable, or any other key to disable)\n");
            memset(cmdChar, 0, 5);
            read(0, cmdChar, 5);
            if ((cmdChar[0] == 'y') || (cmdChar[0] == 'Y')) flagCSC = true;
        }

        /* test auto format and video position*/
        if (flagVideoFormat)
        {
            NEXUS_VideoHdmiInputSettings videoHdmiSettings;
            NEXUS_VideoFormatInfo hdmiVideoFmtInfo;

            /* auto format */
            printf("Disable auto format ? (Y to disable, or any other key to enable)\n");
            memset(cmdChar, 0, 5);
            read(0, cmdChar, 5);
            if ((cmdChar[0] == 'y') || (cmdChar[0] == 'Y'))
            {
                videoHdmiSettings.autoFormat = false;
            }
            else
            {
                videoHdmiSettings.autoFormat = true;
            }

            if (!videoHdmiSettings.autoFormat)
            {
                printf("Input format, refer to : (%d)480i (%d)480p (%d)576p (%d)720p (%d)1080i (%d)1080p (%d)1080p@50 (%d)1600x1200\n",
                    NEXUS_VideoFormat_eNtsc, NEXUS_VideoFormat_e480p, NEXUS_VideoFormat_e576p, NEXUS_VideoFormat_e720p,
                    NEXUS_VideoFormat_e1080i, NEXUS_VideoFormat_e1080p60hz, NEXUS_VideoFormat_e1080p50hz, NEXUS_VideoFormat_eVesa1600x1200p60hz);

                scanf("%d", &cmdInt);
                videoHdmiSettings.format = cmdInt;

                NEXUS_VideoFormat_GetInfo(cmdInt, &hdmiVideoFmtInfo);
                printf("Input format is %dx%d@%d\n",
                    hdmiVideoFmtInfo.digitalWidth, hdmiVideoFmtInfo.digitalHeight,hdmiVideoFmtInfo.verticalFreq);
            }

            /* auto position */
            printf("Disable auto position ? (Y to disable, or any other key to enable)\n");
            memset(cmdChar, 0, 5);
            read(0, cmdChar, 5);
            if ((cmdChar[0] == 'y') || (cmdChar[0] == 'Y'))
            {
                videoHdmiSettings.autoPosition = false;
            }
            else
            {
                videoHdmiSettings.autoPosition = true;
            }

            if (!videoHdmiSettings.autoPosition)
            {
                printf("Input hstart:\n");
                scanf("%d", &cmdInt);
                videoHdmiSettings.position.x = cmdInt;

                printf("Input vstart:\n");
                scanf("%d", &cmdInt);
                videoHdmiSettings.position.y = cmdInt;
            }

            NEXUS_VideoHdmiInput_SetSettings(NEXUS_HdmiInput_GetVideoConnector(hdmiInput), &videoHdmiSettings);
        }


        /* test auto CSC */
        if (flagCSC)
        {
            NEXUS_HdmiInputSettings hdmiSettings;

            printf("Disable auto CSC ? (Y to disable, or any other key to enable)\n");
            memset(cmdChar, 0, 5);
            read(0, cmdChar, 5);
            if ((cmdChar[0] == 'y') || (cmdChar[0] == 'Y'))
            {
                hdmiSettings.autoColorSpace = false;
            }
            else
            {
                hdmiSettings.autoColorSpace = true;
            }

            if (!hdmiSettings.autoColorSpace)
            {
                printf("Input CSC, refer to: (%d)RGB (%d)YCbCr444 (%d)YCbCr422\n",
                    NEXUS_ColorSpace_eRGB, NEXUS_ColorSpace_eYCbCr444, NEXUS_ColorSpace_eYCbCr422);
                scanf("%d", &cmdInt);
                hdmiSettings.colorSpace = cmdInt;
            }
            NEXUS_HdmiInput_SetSettings(hdmiInput, &hdmiSettings);
        }

    }

#endif
    return 0;
}

