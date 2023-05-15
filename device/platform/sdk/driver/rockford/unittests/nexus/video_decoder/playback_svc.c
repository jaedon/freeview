/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: playback_svc.c $
 * $brcm_Revision: Hydra_Software_Devel/17 $
 * $brcm_Date: 6/25/12 3:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/playback_svc.c $
 * 
 * Hydra_Software_Devel/17   6/25/12 3:29p jtna
 * SW7422-170: decrease CDB size requirement
 * 
 * Hydra_Software_Devel/16   3/8/12 5:55p jtna
 * SW7422-170: fix status printout message
 * 
 * Hydra_Software_Devel/15   10/12/11 3:19p katrep
 * SW7231-317:fixed the build issue, api change
 * 
 * Hydra_Software_Devel/14   3/16/11 11:55a jtna
 * SW7422-304: add comment on SVC 3D stream
 * 
 * Hydra_Software_Devel/13   3/7/11 11:37a jtna
 * SW7422-221: make use of NEXUS_VideoFormatInfo.isFullRes3d
 * 
 * Hydra_Software_Devel/12   3/7/11 11:04a jtna
 * SW7422-304: add 3D SVC decode support
 * 
 * Hydra_Software_Devel/11   2/23/11 3:20p jtna
 * SW7422-183: remove EOS wrap workaround
 * 
 * Hydra_Software_Devel/10   1/6/11 4:40p jtna
 * SW7422-170: remove code that reapplies XVD 3d setting after each video
 * decoder stop
 * 
 * Hydra_Software_Devel/9   1/5/11 6:38p jtna
 * SW7422-170: set default frame rate to 60 and remove hack of setting
 * moving content interpolation mode
 * 
 * Hydra_Software_Devel/8   1/3/11 4:09p jtna
 * SW7422-170: option to disable audio decoder
 * 
 * Hydra_Software_Devel/7   1/3/11 3:56p jtna
 * SW7422-170: option to disable audio decoder
 * 
 * Hydra_Software_Devel/6   1/3/11 2:19p jtna
 * SW7422-170: added 3d option
 * 
 * Hydra_Software_Devel/5   1/3/11 12:15p jtna
 * SW7422-170: set output format correctly
 * 
 * Hydra_Software_Devel/4   12/30/10 2:35p jtna
 * SW7422-170: workaround to force progressive scan-out
 * 
 * Hydra_Software_Devel/3   12/30/10 10:44a jtna
 * SW7422-170: added power key to exit key list
 * 
 * Hydra_Software_Devel/2   12/29/10 3:30p jtna
 * SW7422-170: update
 * 
 * Hydra_Software_Devel/1   12/28/10 8:14p jtna
 * SW7422-170: added playback_svc and playback_mvc
 * 
 *****************************************************************************/

/* sample streams: \\lbirv-nas-0001\streams\local\jtna\misc\CES2011\svc\ */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_extra.h"
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
#include "nexus_ir_input.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(playback_svc);

#define DISABLE_AUDIO 0
#if DISABLE_AUDIO
#define NEXUS_AudioDecoder_Open(x,y) NULL
#define NEXUS_AudioDecoder_Close(x)
#define NEXUS_AudioDecoder_Start(x,y)
#define NEXUS_AudioDecoder_Stop(x)
#define NEXUS_AudioOutput_AddInput(x,y)
#define NEXUS_AudioDecoder_GetConnector(x,y)
#define NEXUS_AudioInput_Shutdown(x)
#endif 


static NEXUS_IrInputHandle g_irHandle;

void ir_callback(void* pParam, int iParam)
{
    NEXUS_IrInputEvent irEvent;
    size_t numEvents = 1;
    bool overflow;
    unsigned int keypress;
    NEXUS_Error rc = 0;

    BSTD_UNUSED(iParam);
    BSTD_UNUSED(pParam);

    while (numEvents && rc==0) {
        rc = NEXUS_IrInput_GetEvents(g_irHandle, &irEvent, 1, &numEvents, &overflow);

        if (overflow) {
            printf("Overflow, rc: %d, numEvents:%d\n", rc, numEvents);
        }

        if (numEvents && !irEvent.repeat) {
            keypress = (irEvent.code & 0xffff);

            if (keypress == 53266 /* exit */ || keypress == 24586 /* power */) {
                exit(-1);
            }
            printf("Keypress = %d\n", keypress);
        }
    }
}

NEXUS_VideoDecoderHandle videoDecoder;
NEXUS_VideoDecoderStartSettings videoProgram;
NEXUS_AudioDecoderHandle audioDecoder;
NEXUS_AudioDecoderStartSettings audioProgram;
NEXUS_PlaybackHandle playback;
NEXUS_FilePlayHandle file;

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
    /*NEXUS_VideoDecoderHandle videoDecoder;*/
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    /*NEXUS_VideoDecoderStartSettings videoProgram;*/
    /*NEXUS_AudioDecoderHandle audioDecoder;*/
    /*NEXUS_AudioDecoderStartSettings audioProgram;*/
    /*NEXUS_FilePlayHandle file;*/
    NEXUS_PlaypumpHandle playpump;
    /*NEXUS_PlaybackHandle playback;*/
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_IrInputSettings irSettings;
    NEXUS_VideoFormat videoFormat;
    char videoFormatName[8];
    char fname[256];
    unsigned vpid, epid;
    int i;
    bool avc_only = false;
    bool svc_3d = false;

    vpid = 0x31; 
    epid = 0x32;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    videoFormat = NEXUS_VideoFormat_e1080p; /* SVC defaults to 1080p */
    sprintf(videoFormatName, "%s", "1080p");
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename (avc) (480p/720p/1080p) (3d)\n", argv[0]);
        return -1;
    }
    for (i=2;i<argc;i++) {
        if (strcmp(argv[i], "avc") == 0) {
            avc_only = true;
            videoFormat = NEXUS_VideoFormat_e720p; /* AVC defaults to 720p */
            sprintf(videoFormatName, "%s", "720p");
        }
        else if (strcmp(argv[i], "3D") == 0 || strcmp(argv[i], "3d") == 0) {
            svc_3d = true;
        }
        else if (strcmp(argv[i], "480p") == 0) {
            videoFormat = NEXUS_VideoFormat_e480p;
            sprintf(videoFormatName, "%s", "480p");
        }
        else if (strcmp(argv[i], "720p") == 0) {
            videoFormat = NEXUS_VideoFormat_e720p;
            sprintf(videoFormatName, "%s", "720p");
        }
        else if (strcmp(argv[i], "1080p") == 0) {
            videoFormat = NEXUS_VideoFormat_e1080p;
            sprintf(videoFormatName, "%s", "1080p");
        }
    }

    /* SVC 3D defaults to specific video formats for now */
    if (svc_3d) {
        /* for SVC 3D monsterhouse stream, the frame rate reported by vdecoder is 60hz in both cases. 
           however, there is no display when the video format is set to NEXUS_VideoFormat_e3D_1080p_30Hz, so set it to _24Hz for now */
        if (!avc_only) {
            videoFormat = NEXUS_VideoFormat_e3D_1080p_24Hz;
            sprintf(videoFormatName, "%s", "1080p3D");
        }
        else {
            videoFormat = NEXUS_VideoFormat_e1080p60hz;
            sprintf(videoFormatName, "%s", "1080p");
        }
    }
    sprintf(fname, "%s", argv[1]);

    BKNI_Printf("****************************************************************\n");
    BKNI_Printf("file: %s\n", fname);
    BKNI_Printf("pids: %#x %#x, mode: %s, output: %s(%d) %s\n", vpid, epid, avc_only?"AVC":"SVC", videoFormatName, videoFormat, svc_3d?"3D":"");
    BKNI_Printf("****************************************************************\n");

    /* Open IR module */
    NEXUS_IrInput_GetDefaultSettings(&irSettings);
    irSettings.mode = NEXUS_IrInputMode_eRemoteA;
    irSettings.dataReady.callback = ir_callback;
    g_irHandle = NEXUS_IrInput_Open(0, &irSettings);

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
    displaySettings.format = videoFormat;

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
    if (svc_3d && videoFormatInfo.isFullRes3d) {
        displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eHdmiDvo; /* HDMI master mode is required */
    }
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));

    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    videoDecoderOpenSettings.fifoSize = 3 * 1024 * 1024;
    videoDecoderOpenSettings.svc3dSupported = svc_3d;
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    if (svc_3d) {
        if (!avc_only) {
            NEXUS_VideoDecoderSettings videoDecoderSettings;
            NEXUS_VideoDecoderExtendedSettings videoDecoderExtendedSettings;
            NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
            videoDecoderSettings.customSourceOrientation = true;
            videoDecoderSettings.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_LeftRight;
            NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);

            NEXUS_VideoDecoder_GetExtendedSettings(videoDecoder, &videoDecoderExtendedSettings);
            videoDecoderExtendedSettings.svc3dEnabled = true;
            NEXUS_VideoDecoder_SetExtendedSettings(videoDecoder, &videoDecoderExtendedSettings);
        }
        else {
            NEXUS_HdmiOutputVendorSpecificInfoFrame vsi;
            NEXUS_HdmiOutput_GetVendorSpecificInfoFrame(platformConfig.outputs.hdmi[0], &vsi);
            vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
            vsi.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eSidexSideHalf;
            NEXUS_HdmiOutput_SetVendorSpecificInfoFrame(platformConfig.outputs.hdmi[0], &vsi);  
        }
    }

    /* Open the pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = avc_only ? NEXUS_VideoCodec_eH264 : NEXUS_VideoCodec_eH264_Svc;
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, vpid, &playbackPidSettings);
    enhancementVideoPidChannel = NEXUS_Playback_OpenPidChannel(playback, epid, &playbackPidSettings);

#if (!DISABLE_AUDIO)
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
    audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x33, &playbackPidSettings);
#else
    audioPidChannel = NULL;
#endif

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = avc_only ? NEXUS_VideoCodec_eH264 : NEXUS_VideoCodec_eH264_Svc;
    videoProgram.pidChannel = videoPidChannel;
    if (!avc_only) {
        videoProgram.enhancementPidChannel = enhancementVideoPidChannel;
    }
    videoProgram.stcChannel = stcChannel;
    /* Set the default framerate to 60. Otherwise, 1080p60 streams without a 
       coded frame rate won't be scanned out as progressive. See SW7422-175. 
       To verify, BVN_MIDDLE.SCL_0.SCR_PIC_SIZE must be 1920x1080, not 1920x540 */
    videoProgram.frameRate = NEXUS_VideoFrameRate_e60;

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Start decoders */
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

    /* Start playback */
    NEXUS_Playback_Start(playback, file, NULL);

    while (1) {
        NEXUS_PlaybackStatus playbackStatus;
        NEXUS_VideoDecoderStatus videoStatus;
        NEXUS_Playback_GetStatus(playback, &playbackStatus);
        NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
        if (playbackStatus.fifoSize) { /* fifoSize can be NULL due to a race condition with Playback_Stop and Playback_GetStatus */
            BKNI_Printf("pos %lu, last %lu, state %d, pfifo %d%%, vfifo %d%% pts %d, diff %d\n", playbackStatus.position, playbackStatus.last, playbackStatus.state,
                (playbackStatus.fifoDepth*100)/playbackStatus.fifoSize, (videoStatus.fifoDepth*100)/videoStatus.fifoSize,
                videoStatus.pts, videoStatus.ptsStcDifference);
        }
        BKNI_Sleep(1000);
    }

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
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_Platform_Uninit();

#endif
    return 0;
}
