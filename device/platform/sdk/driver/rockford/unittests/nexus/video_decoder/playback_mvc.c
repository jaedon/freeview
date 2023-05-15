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
 * $brcm_Workfile: playback_mvc.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 6/25/12 3:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/playback_mvc.c $
 * 
 * Hydra_Software_Devel/13   6/25/12 3:28p jtna
 * SW7422-170: decrease CDB size requirement
 * 
 * Hydra_Software_Devel/12   3/8/12 5:54p jtna
 * SW7422-170: fix status printout message
 * 
 * Hydra_Software_Devel/11   10/12/11 3:19p katrep
 * SW7231-317:fixed the build issue, api change
 * 
 * Hydra_Software_Devel/10   3/15/11 12:13p jtna
 * SW7422-221: make use of NEXUS_VideoFormatInfo.isFullRes3d
 * 
 * Hydra_Software_Devel/9   2/23/11 3:20p jtna
 * SW7422-183: remove EOS wrap workaround
 * 
 * Hydra_Software_Devel/8   1/6/11 4:39p jtna
 * SW7422-170: remove code that reapplies XVD 3d setting after each video
 * decoder stop
 * 
 * Hydra_Software_Devel/7   1/6/11 4:27p jtna
 * SW7422-181: removed workaround for pink/green garbage in MVC decode
 * 
 * Hydra_Software_Devel/6   1/6/11 2:14p jtna
 * SW7422-170: stop/start audio decoder in EOS callback
 * 
 * Hydra_Software_Devel/5   1/6/11 11:45a jtna
 * SW7422-170: option to disable audio
 * 
 * Hydra_Software_Devel/4   12/30/10 2:35p jtna
 * SW7422-170: better green-line issue workaround
 * 
 * Hydra_Software_Devel/3   12/30/10 10:44a jtna
 * SW7422-170: added power key to exit key list
 * 
 * Hydra_Software_Devel/2   12/29/10 3:29p jtna
 * SW7422-170: update
 * 
 * Hydra_Software_Devel/1   12/28/10 8:13p jtna
 * SW7422-170: added playback_svc and playback_mvc
 * 
 *****************************************************************************/

/* sample stream: \\lbirv-nas-0001\streams\local\jtna\misc\CES2011\mvc\monsterhouse.mvc.remux3.ts */

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

BDBG_MODULE(playback_mvc);

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
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    /*NEXUS_VideoDecoderStartSettings videoProgram;*/
    /*NEXUS_AudioDecoderHandle audioDecoder;*/
    /*NEXUS_AudioDecoderStartSettings audioProgram;*/
    /*NEXUS_FilePlayHandle file;*/
    NEXUS_PlaypumpHandle playpump;
    /*NEXUS_PlaybackHandle playback;*/
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_IrInputSettings irSettings;
    char fname[256];
    unsigned vpid, epid;
    bool avc_only = false;

    vpid = 0x31; 
    epid = 0x32;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename (avc)\n", argv[0]);
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
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));

    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    videoDecoderOpenSettings.fifoSize = 3 * 1024 * 1024;
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    if (!avc_only) {
        NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
        videoDecoderSettings.customSourceOrientation = true;
        videoDecoderSettings.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_LeftRight;
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
    BKNI_Printf("Press ENTER to quit\n");
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
