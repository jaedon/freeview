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
 * $brcm_Workfile: mosaic_transition_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/23/12 12:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/mosaic_transition_stress.c $
 * 
 * Hydra_Software_Devel/1   3/23/12 12:37p jtna
 * SW7346-725: add mosaic_transition_stress.c
 * 
******************************************************************************/
/* test transitions between mosaic and non-mosaic modes */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif

#include "nexus_mosaic_display.h"
#include "nexus_core_utils.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

/* the following define the input file and its characteristics -- these will vary by input file */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define VIDEO_PID 0x31

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle parentWindow;
    NEXUS_VideoWindowHandle window[2]; /* mosaic windows */
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_VideoWindowSettings windowSettings[2];
    NEXUS_VideoWindowMosaicSettings mosaicWindowSettings;
    NEXUS_VideoFormatInfo displayFormatInfo;
    NEXUS_Rect windowRect[2];
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif
    bool started = false;
    unsigned loops = 100;

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
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = videoPidChannel;

    /* Bring up display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif

#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    }
#endif
    parentWindow = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

    /* required for mosaic mode */
    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.manualPowerState = true; 
    rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    BDBG_ASSERT(!rc);

    while (loops--) {
        NEXUS_VideoWindow_AddInput(parentWindow, NEXUS_VideoDecoder_GetConnector(videoDecoder));

        if (!started) {
            rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
            BDBG_ASSERT(!rc);
            started = true;
        }

        BKNI_Printf("in non-mosaic mode...\n");
        BKNI_Sleep(rand()%2000);

        /* tear down main window */
        NEXUS_VideoWindow_RemoveInput(parentWindow, NEXUS_VideoDecoder_GetConnector(videoDecoder));
        if (rand()%2) {
            /* randomly skip shutdown. if skipped, NEXUS_VideoWindow_AddInput() will print a warning, but should succeed */
            BKNI_Printf("skip video input shutdown\n");
            NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
        }

        if (rand()%2) {
            /* randomly flush CC. this function used to create a VDC source, which would cause problems */
            BKNI_Printf("flush cc\n");
            NEXUS_VideoInput_FlushClosedCaption(NEXUS_VideoDecoder_GetConnector(videoDecoder));
        }

        windowRect[0].x = 0;
        windowRect[0].y = 0;
        windowRect[0].width = displayFormatInfo.width/2;
        windowRect[0].height = displayFormatInfo.height;
        windowRect[1] = windowRect[0];
        windowRect[1].x = windowRect[1].x + displayFormatInfo.width/2;

        window[0] = NEXUS_VideoWindow_OpenMosaic(parentWindow, 0);
        NEXUS_VideoWindow_GetSettings(window[0], &windowSettings[0]);
        windowSettings[0].position = windowRect[0];
        rc = NEXUS_VideoWindow_SetSettings(window[0], &windowSettings[0]);
        BDBG_ASSERT(!rc);

        /* mosaic 0 uses the left half of the video */
        NEXUS_VideoWindow_GetMosaicSettings(window[0], &mosaicWindowSettings);
        mosaicWindowSettings.backendMosaic.enabled = true;
        mosaicWindowSettings.backendMosaic.clipRect.x = 0;
        mosaicWindowSettings.backendMosaic.clipRect.y = 0;
        mosaicWindowSettings.backendMosaic.clipRect.width = 1920/2;
        mosaicWindowSettings.backendMosaic.clipRect.height = 1080;
        mosaicWindowSettings.backendMosaic.clipBase.width = 1920;
        mosaicWindowSettings.backendMosaic.clipBase.height = 1080;
        rc = NEXUS_VideoWindow_SetMosaicSettings(window[0], &mosaicWindowSettings);
        BDBG_ASSERT(!rc);

        window[1] = NEXUS_VideoWindow_OpenMosaic(parentWindow, 1);
        NEXUS_VideoWindow_GetSettings(window[1], &windowSettings[1]);
        windowSettings[1].position = windowRect[1];
        rc = NEXUS_VideoWindow_SetSettings(window[1], &windowSettings[1]);
        BDBG_ASSERT(!rc);

        /* mosaic 1 uses the right half of the video */
        NEXUS_VideoWindow_GetMosaicSettings(window[1], &mosaicWindowSettings);
        mosaicWindowSettings.backendMosaic.enabled = true;
        mosaicWindowSettings.backendMosaic.clipRect.x = 1920/2;
        mosaicWindowSettings.backendMosaic.clipRect.y = 0;
        mosaicWindowSettings.backendMosaic.clipRect.width = 1920/2;
        mosaicWindowSettings.backendMosaic.clipRect.height = 1080;
        mosaicWindowSettings.backendMosaic.clipBase.width = 1920;
        mosaicWindowSettings.backendMosaic.clipBase.height = 1080;
        rc = NEXUS_VideoWindow_SetMosaicSettings(window[1], &mosaicWindowSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_VideoWindow_AddInput(window[0], NEXUS_VideoDecoder_GetConnector(videoDecoder));
        BDBG_ASSERT(!rc);
        rc = NEXUS_VideoWindow_AddInput(window[1], NEXUS_VideoDecoder_GetConnector(videoDecoder));
        BDBG_ASSERT(!rc);

        BKNI_Printf("in mosaic mode...\n");
        BKNI_Sleep(rand()%2000);

        NEXUS_VideoWindow_RemoveInput(window[0], NEXUS_VideoDecoder_GetConnector(videoDecoder));
        NEXUS_VideoWindow_RemoveInput(window[1], NEXUS_VideoDecoder_GetConnector(videoDecoder));
        NEXUS_VideoWindow_Close(window[0]);
        NEXUS_VideoWindow_Close(window[1]);
    }

    NEXUS_VideoWindow_Close(parentWindow);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_Display_RemoveAllOutputs(display);
    NEXUS_Display_Close(display);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_Platform_Uninit();

    return 0;
}
