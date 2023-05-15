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
 * $brcm_Workfile: playback_mvc.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 6/25/12 3:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/video/playback_mvc.c $
 * 
 * 9   6/25/12 3:27p jtna
 * SW7422-170: decrease CDB size requirement
 * 
 * 8   3/8/12 5:53p jtna
 * SW7422-170: fix status printout message
 * 
 * 7   5/4/11 11:38a vsilyaev
 * SW7346-179, SW7425-416: Use different types for the video decoder and
 *  display orientation
 * 
 * 6   4/4/11 10:52a jtna
 * SW7422-170: make unneeded global variables local
 * 
 * 5   3/17/11 12:50p erickson
 * SW7422-333: always build mvc/svc examples, but have print err on
 *  runtime failure
 * 
 * 4   3/15/11 12:13p jtna
 * SW7422-221: make use of NEXUS_VideoFormatInfo.isFullRes3d
 * 
 * 3   3/15/11 10:49a jtna
 * SW7422-170: update comments
 * 
 * 2   2/23/11 3:19p jtna
 * SW7422-183: remove EOS wrap workaround
 * 
 * 1   1/19/11 4:48p jtna
 * SW7422-170: add playback_svc.c and playback_mvc.c
 * 
 *****************************************************************************/

/* Nexus example app: MVC (Multi View Video Coding) example 

   Usage Cases:
   ------------

   1. Decode and display MVC (left + right view) at the default 3D output resolution of 1080p24:
   # nexus playback_mvc filename

   2. Decode and display MVC (left view only) at the default 2D output resolution of 1080p24:
   # nexus playback_mvc filename avc

   * Note: The video PIDs are hardcoded to 0x31 (left view) and 0x32 (right view)  */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_hdmi_output.h"
#include "nexus_component_output.h"
#include "nexus_core_utils.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(playback_mvc);

int main(int argc, const char *argv[])  {
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel, enhancementVideoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_FilePlayHandle file;
    char fname[256];
    unsigned vpid, epid;
    bool avc_only = false;
    NEXUS_Error rc;

    vpid = 0x31; 
    epid = 0x32;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    if (argc < 2) {
        fprintf(stderr, "Usage: nexus %s filename (avc)\n", argv[0]);
        return -1;
    }
    if (argc==3) {
        if (strcmp(argv[2], "avc") == 0) {
            avc_only = true;
        }
    }
    sprintf(fname, "%s", argv[1]);

    BKNI_Printf("****************************************************************\n");
    BKNI_Printf("file: %s, pids: %#x %#x, mode: %s\n", fname, vpid, epid, avc_only?"AVC":"MVC");
    BKNI_Printf("****************************************************************\n");

    file = NEXUS_FilePlay_OpenPosix(fname, NULL);
    if (!file) {
        fprintf(stderr, "Can't open file: %s\n", fname);
        return -1;
    }

    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;
    if (avc_only) {
        displaySettings.format = NEXUS_VideoFormat_e1080p24hz;
    }
    else {
        displaySettings.format = NEXUS_VideoFormat_e3D_1080p_24Hz;
    }

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
    if (videoFormatInfo.isFullRes3d) {
        displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eHdmiDvo; /* HDMI master mode is required */
    }
    display = NEXUS_Display_Open(0, &displaySettings);
    if (!display) {
        BDBG_ERR(("feature not supported"));
        return -1;
    }

    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));

    window = NEXUS_VideoWindow_Open(display, 0);

    /* Bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    videoDecoderOpenSettings.fifoSize = 3 * 1024 * 1024; /* this is stream-dependent */
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    if (!avc_only) {
        NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
        videoDecoderSettings.customSourceOrientation = true;
        videoDecoderSettings.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightFullFrame;
        NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    }

    /* Open the pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = avc_only ? NEXUS_VideoCodec_eH264 : NEXUS_VideoCodec_eH264_Mvc;
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, vpid, &playbackPidSettings);
    enhancementVideoPidChannel = NEXUS_Playback_OpenPidChannel(playback, epid, &playbackPidSettings);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
    audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x33, &playbackPidSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = avc_only ? NEXUS_VideoCodec_eH264 : NEXUS_VideoCodec_eH264_Mvc;
    videoProgram.pidChannel = videoPidChannel;
    if (!avc_only) {
        videoProgram.enhancementPidChannel = enhancementVideoPidChannel;
    }
    videoProgram.stcChannel = stcChannel;

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Start decoders */
    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    if (rc) {
        BDBG_ERR(("feature not supported"));
        return -1;
    }
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

    /* Start playback */
    NEXUS_Playback_Start(playback, file, NULL);

#if 1
    while (1) {
        NEXUS_PlaybackStatus playbackStatus;
        NEXUS_VideoDecoderStatus videoStatus;
        NEXUS_Playback_GetStatus(playback, &playbackStatus);
        NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
        if (playbackStatus.fifoSize) {
            BKNI_Printf("pos %lu, last %lu, state %d, pfifo %d%%, vfifo %d%% pts %d, diff %d\n", playbackStatus.position, playbackStatus.last, playbackStatus.state,
                (playbackStatus.fifoDepth*100)/playbackStatus.fifoSize, (videoStatus.fifoDepth*100)/videoStatus.fifoSize,
                videoStatus.pts, videoStatus.ptsStcDifference);
        }
        BKNI_Sleep(1000);
    }

#else
    /* Playback state machine is driven from inside Nexus. */
    printf("Press ENTER to quit\n");
    getchar();

    /* Bring down system */
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_Platform_Uninit();
#endif

#endif
    return 0;
}
