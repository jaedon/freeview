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
 * $brcm_Workfile: decode.c $
 * $brcm_Revision: 17 $
 * $brcm_Date: 8/31/12 11:30a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/utils/decode.c $
 * 
 * 17   8/31/12 11:30a erickson
 * SW7429-253: print video bufferLatency
 * 
 * 16   1/31/12 5:07p rjlewis
 * SW7425-1136: Added HDMI support (and some cleanup).
 * 
 * 15   3/23/11 5:41p vsilyaev
 * SW7422-406: Unified parsing of command line parameters
 * 
 * 14   1/7/11 3:30p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 13   12/13/10 4:07p erickson
 * SW35230-179: add video bitrate using
 *  NEXUS_VideoDecoderStatus.numBytesDecoded
 * 
 * 12   9/8/10 12:05p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * SW7468-129/1   3/5/10 7:31p vsilyaev
 * SW7468-129: Added hooks for soft video decoder
 * 
 * 11   12/30/09 2:23p erickson
 * SW7550-128: add closed caption feature (-cc on)
 *
 * 10   8/18/09 4:36p vsilyaev
 * PR 56809: Added option to control handling of video errors
 *
 * 9   6/8/09 7:06a erickson
 * PR55617: add user-specific aspectRatio
 *
 * 8   3/18/09 10:26a erickson
 * PR52350: add wxga/fha support with 50/60 hz option
 *
 * 7   3/6/09 9:33a erickson
 * PR51743: added -ar and -graphics options, default DTV apps to panel
 *  output
 *
 * 6   2/5/09 2:08p erickson
 * PR51151: update
 *
 * 5   1/23/09 5:12p erickson
 * PR48944: update
 *
 * 4   1/22/09 7:48p vsilyaev
 * PR 50848: Don't use globals for the command line options
 *
 * 3   11/20/08 12:50p erickson
 * PR48944: update
 *
 * 2   11/17/08 2:19p erickson
 * PR48944: update
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_surface.h"
#include "nexus_core_utils.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#if NEXUS_HAS_ASTM
#include "nexus_astm.h"
#endif
#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel.h"
#endif

#include "cmdline_args.h"

BDBG_MODULE(decode);

static unsigned b_get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

#if NEXUS_NUM_HDMI_OUTPUTS
/* registered HDMI hotplug handler -- changes the format (to monitor's default) if monitor doesn't support current format */
static void hotplug_callback(void *pParam, int iParam)
{
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_HdmiOutputHandle hdmi = pParam;
    NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)iParam;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_Error rc;

    rc = NEXUS_HdmiOutput_GetStatus(hdmi, &hdmiStatus);
    /*fprintf(stderr, "HDMI event: %s\n", hdmiStatus.connected?"connected":"not connected");*/

    /* the app can choose to switch to the preferred format, but it's not required. */
    if ( !rc && hdmiStatus.connected )
    {
        NEXUS_DisplaySettings displaySettings;
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] )
        {
            fprintf(stderr, "Current format not supported by attached monitor -- switching to preferred format (%d)\n", hdmiStatus.preferredVideoFormat);
            if (hdmiStatus.preferredVideoFormat >= NEXUS_VideoFormat_e480p) 
                fprintf(stderr, "Warning: This format may disable composite output!\n");
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    }
}
#endif

int main(int argc, const char *argv[])
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle pcrPidChannel, videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder, audioPassthroughDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
#if NEXUS_HAS_ASTM
    NEXUS_AstmSettings astmSettings;
    NEXUS_AstmHandle astm;
#endif
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelSettings syncChannelSettings;
    NEXUS_SyncChannelHandle syncChannel;
#endif
    struct util_opts_t opts;
    NEXUS_Error rc;
    unsigned start;

    if (cmdline_parse(argc, argv, &opts)) {
        return 0;
    }
    if (cmdline_probe(&opts.common, opts.filename, &opts.indexname)) {
        return 1;
    }
    /* They MUST include a video and audio PID for the decode */
    if (!opts.common.videoPid && !opts.common.audioPid) {
        BDBG_ERR(("Missing video or audio pid specification; See usage."));
        print_usage(argv[0]);
        return 1;
    }

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    rc = NEXUS_Platform_GetStreamerInputBand(0, &inputBand);
    BDBG_ASSERT(!rc);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = opts.common.transportType;
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);

    /* Open the audio and video pid channels */
    pcrPidChannel = NEXUS_PidChannel_Open(parserBand, opts.common.pcrPid?opts.common.pcrPid:opts.common.videoPid, NULL);
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, opts.common.videoPid, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, opts.common.audioPid, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = pcrPidChannel;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

#if NEXUS_HAS_SYNC_CHANNEL
    if (opts.sync) {
        NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
        syncChannel = NEXUS_SyncChannel_Create(&syncChannelSettings);
    }
#endif

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = opts.common.videoCodec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    videoProgram.frameRate = opts.common.videoFrameRate;
    videoProgram.aspectRatio = opts.common.aspectRatio;
    videoProgram.sampleAspectRatio.x = opts.common.sampleAspectRatio.x;
    videoProgram.sampleAspectRatio.y = opts.common.sampleAspectRatio.y;
    videoProgram.errorHandling = opts.videoErrorHandling;

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = opts.common.audioCodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    rc = NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    BDBG_ASSERT(!rc);

    /* TODO: param for compressed passthrough of same or different pid */
    audioPassthroughDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    rc = NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(audioPassthroughDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    BDBG_ASSERT(!rc);

#if NEXUS_NUM_HDMI_OUTPUTS
    if (opts.common.useHdmiOutput ) {
        NEXUS_HdmiOutputStatus hdmiStatus;
        rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
        /* If its connected we'll try to use compressed */
        if ( !rc && hdmiStatus.connected )
        {
            if (( audioProgram.codec == NEXUS_AudioCodec_eAc3 ) && ( hdmiStatus.audioCodecSupported[NEXUS_AudioCodec_eAc3] ))
            {
                NEXUS_AudioOutput_AddInput(
                    NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
                    NEXUS_AudioDecoder_GetConnector(audioPassthroughDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
            }
            else
            {
                NEXUS_AudioOutput_AddInput(
                    NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
                    NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
            }
        }
        else
        {
            /* if not connected we'll default to stereo on hotplug */
            NEXUS_AudioOutput_AddInput(
                NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
                NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
        }
    }
#endif

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = opts.common.displayType;
    displaySettings.format = opts.common.displayFormat;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if (opts.common.useCompositeOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
        BDBG_ASSERT(!rc);
    }
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if (opts.common.useComponentOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
        BDBG_ASSERT(!rc);
    }
#endif
#if NEXUS_NUM_PANEL_OUTPUTS
    if (opts.common.usePanelOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
        BDBG_ASSERT(!rc);
        NEXUS_BoardCfg_ConfigurePanel(true, true, true);
        printf("Panel on\n");
    }
#endif
#if NEXUS_NUM_SCART_INPUTS
    if (opts.common.useScart1CompositeOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
        BDBG_ASSERT(!rc);
        printf("Turning on: scart 1 composite output\n");
    }
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    if (opts.common.useHdmiOutput) {
        NEXUS_HdmiOutputSettings hdmiSettings;

        rc = NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
        BDBG_ASSERT(!rc);

        /* Install hotplug callback -- video only for now */
        NEXUS_HdmiOutput_GetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
        hdmiSettings.hotplugCallback.callback = hotplug_callback;
        hdmiSettings.hotplugCallback.context = platformConfig.outputs.hdmi[0];
        hdmiSettings.hotplugCallback.param = (int)display;
        NEXUS_HdmiOutput_SetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);

        /* Force a hotplug to switch to preferred format */
        hotplug_callback(platformConfig.outputs.hdmi[0], (int)display);
    }
#endif

    if (opts.graphics) {
        NEXUS_SurfaceHandle framebuffer;
        NEXUS_SurfaceCreateSettings surfaceCreateSettings;
        NEXUS_SurfaceMemory mem;
        NEXUS_GraphicsSettings graphicsSettings;
        NEXUS_VideoFormatInfo videoFormatInfo;
        unsigned i,j;

        NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);

        NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
        surfaceCreateSettings.width = 720;
        surfaceCreateSettings.height = videoFormatInfo.height;
        framebuffer = NEXUS_Surface_Create(&surfaceCreateSettings);
        NEXUS_Surface_GetMemory(framebuffer, &mem);
        for (i=0;i<surfaceCreateSettings.height;i++) {
            for (j=0;j<surfaceCreateSettings.width;j++) {
                /* create checker board */
                ((uint32_t*)((uint8_t*)mem.buffer + i*mem.pitch))[j] = (((i/50)%2) ^ ((j/50)%2)) ? 0x00000000 : 0xFFFFFFFF;
            }
        }

        NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
        graphicsSettings.enabled = true;
        graphicsSettings.clip.width = surfaceCreateSettings.width;
        graphicsSettings.clip.height = surfaceCreateSettings.height;
        rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
        BDBG_ASSERT(!rc);
    }

    window = NEXUS_VideoWindow_Open(display, 0);

    NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    windowSettings.contentMode = opts.common.contentMode;
    rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
    BDBG_ASSERT(!rc);

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(opts.common.videoDecoder, NULL); /* take default capabilities */
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
    displayVbiSettings.vbiSource = NEXUS_VideoDecoder_GetConnector(videoDecoder);
    displayVbiSettings.closedCaptionEnabled = opts.closedCaptionEnabled;
    displayVbiSettings.closedCaptionRouting = opts.closedCaptionEnabled;
    rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
    BDBG_ASSERT(!rc);

#if NEXUS_HAS_ASTM
    if (opts.astm) {
        /* create an astm instance and connect */
        NEXUS_Astm_GetDefaultSettings(&astmSettings);
        astmSettings.stcChannel = stcChannel;
        astmSettings.videoDecoder = videoDecoder;
        astmSettings.audioDecoder[0] = audioDecoder;
        astmSettings.audioDecoder[1] = audioPassthroughDecoder;
        astmSettings.stcMaster = audioDecoder; /* audio master is preferred */
        astm = NEXUS_Astm_Create(&astmSettings);
    }
#endif

#if NEXUS_HAS_SYNC_CHANNEL
    if (opts.sync) {
        NEXUS_SyncChannel_GetSettings(syncChannel, &syncChannelSettings);
        syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(videoDecoder);
        syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
        rc = NEXUS_SyncChannel_SetSettings(syncChannel, &syncChannelSettings);
        BDBG_ASSERT(!rc);
    }
#endif

    /* Start Decoders */
    start = b_get_time();
    if (opts.common.videoPid) {
        rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
        BDBG_ASSERT(!rc);
    }
    if (opts.common.audioPid) {
        rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
        BDBG_ASSERT(!rc);
        rc = NEXUS_AudioDecoder_Start(audioPassthroughDecoder, &audioProgram);
        BDBG_ASSERT(!rc);
    }

#if NEXUS_HAS_ASTM
    if (opts.astm) {
        NEXUS_Astm_Start(astm);
    }
#endif

#if 1
    /* Print status while decoding */
    for (;;) {
        NEXUS_VideoDecoderStatus status;
        NEXUS_AudioDecoderStatus audioStatus;
        uint32_t stc;
        unsigned duration = (b_get_time() - start) / 1000;
        unsigned bitrate = 0;
        
        NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
        if (duration) {
            bitrate = status.numBytesDecoded/duration*8; /* bps */
        }
        NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);
        printf("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo=%d%% %d.%01dMbps, latency=%dmsec\n",
            status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference, status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0,
#define MEGABYTE (1024*1024)
            bitrate/MEGABYTE,(bitrate%MEGABYTE)*10/MEGABYTE,
            status.bufferLatency);
        NEXUS_AudioDecoder_GetStatus(audioDecoder, &audioStatus);
        printf("audio0            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
        BKNI_Sleep(1000);
    }
#else
    printf("Press ENTER to stop decode\n");
    getchar();

    /* example shutdown */
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}

/*
************************************************

examples / test cases

# basic decode of streamer input
nexus decode -video 0x31 -audio 0x34 -video_type mpeg -audio_type ac3

************************************************
*/

