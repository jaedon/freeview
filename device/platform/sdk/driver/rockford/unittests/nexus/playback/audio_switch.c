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
 * $brcm_Workfile: audio_switch.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 7/24/12 1:54p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/audio_switch.c $
 * 
 * Hydra_Software_Devel/20   7/24/12 1:54p jessem
 * SW7425-3568: Added NEXUS_AudioDecoder_Close
 *
 * Hydra_Software_Devel/19   2/10/12 10:57a mward
 * SW7400-3067:  Disable HD DivX files for 7400.  It supports only SD
 * DivX.
 *
 * Hydra_Software_Devel/18   8/10/10 3:37p randyjew
 * SW7208-105: Fix compile warning for implicit declaration for time
 *
 * Hydra_Software_Devel/17   6/8/10 11:31a mward
 * SW7125-470:  Add file name to "Playing" message.
 *
 * Hydra_Software_Devel/16   5/26/10 1:00p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 *
 * Hydra_Software_Devel/15   5/7/10 6:37p mward
 * SW7125-253:  Allow compilation with C++, eliminate "invalid conversion"
 * errors.
 *
 * Hydra_Software_Devel/14   5/6/09 10:18a erickson
 * PR42679: reduce execution time for automated test
 *
 * Hydra_Software_Devel/13   5/4/09 4:47p erickson
 * PR42679: shorten run time
 *
 * Hydra_Software_Devel/12   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/11   12/18/08 11:40a erickson
 * PR49930: fix shutdown
 *
 * Hydra_Software_Devel/10   12/10/08 4:43p erickson
 * PR49930: move accurate seek algorithm inside playback
 *
 * Hydra_Software_Devel/9   12/10/08 11:08a erickson
 * PR49930: update
 *
 * Hydra_Software_Devel/8   12/9/08 6:43p erickson
 * PR49930: update
 *
 * Hydra_Software_Devel/7   12/8/08 11:18a erickson
 * PR49930: add accurateSeek, seek playback for immediate resumption, add
 * list of playback files
 *
 * Hydra_Software_Devel/6   11/3/08 11:38a erickson
 * PR42679: clean up
 *
 * Hydra_Software_Devel/5   10/24/08 10:36a erickson
 * PR42679: shorten test
 *
 * Hydra_Software_Devel/4   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/3   5/22/08 1:22p erickson
 * PR42679: fix return codes
 *
 * Hydra_Software_Devel/2   5/13/08 10:33a erickson
 * PR42644: convert test to output switch test with random timing
 *
 * Hydra_Software_Devel/1   5/12/08 2:49p erickson
 * PR42627: add test app
 *
 * Hydra_Software_Devel/1   5/7/08 3:51p erickson
 * PR42504: add playback unittests
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#include "nexus_audio_input.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_playpump.h"
#include "nexus_playback.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
BDBG_MODULE(audio_switch);

struct teststreams_t {
    const char *filename;
    const char *indexname;
    unsigned short video_pid;
    NEXUS_VideoCodec video_codec;
    unsigned short audio_pid;
    NEXUS_AudioCodec audio_codec;
    NEXUS_TransportType transportType;
} g_teststreams[] =
{
	#if (BCHP_CHIP!=7400) /* No HD DivX Support on 7400 */
    {"videos/avi/F01_HD_1080p11_640x480_Xsub_01.divx", NULL, 1, NEXUS_VideoCodec_eMpeg4Part2, 2, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eAvi},
    {"videos/avi/F01_HD_1080p11_640x480_Xsub_01.divx", NULL, 1, NEXUS_VideoCodec_eMpeg4Part2, 2, NEXUS_AudioCodec_eMpeg, NEXUS_TransportType_eAvi}, /* no audio */
	#endif
    {"videos/avi/FantasticFour.divx", NULL, 1, NEXUS_VideoCodec_eMpeg4Part2, 2, NEXUS_AudioCodec_eMp3, NEXUS_TransportType_eAvi},
    {"videos/asf/Discoverers_1080ap.wmv", NULL, 2, NEXUS_VideoCodec_eVc1, 1, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eAsf},
    {"videos/cnnticker.mpg", "videos/cnnticker.nav", 0x21, NEXUS_VideoCodec_eMpeg2, 0x22, NEXUS_AudioCodec_eMpeg, NEXUS_TransportType_eTs},
    {"videos/riddick_avc_720p.mpg", "videos/riddick_avc_720p.nav", 0x1422, NEXUS_VideoCodec_eH264, 0x1423, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eTs},
    {NULL, NULL, 0, NEXUS_VideoCodec_eUnknown, 0, NEXUS_AudioCodec_eUnknown, NEXUS_TransportType_eUnknown}
};

#if 0
/* removed these files to reduce automated test time */
    {"videos/avi/RiverFrontStadiumGoesDown.divx", NULL, 1, NEXUS_VideoCodec_eMpeg4Part2, 2, NEXUS_AudioCodec_eMp3, NEXUS_TransportType_eAvi},
    {"videos/avi/RenoBalloom.divx", NULL, 2, NEXUS_VideoCodec_eMpeg4Part2, 1, NEXUS_AudioCodec_eMp3, NEXUS_TransportType_eAvi},
    {"videos/discoveryAvcHD.mpg", "videos/discoveryAvcHD.nav", 0x1522, NEXUS_VideoCodec_eH264, 0x1523, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eTs},
    {"videos/herbie1AvcHD.mpg", "videos/herbie1AvcHD.nav", 0x1222, NEXUS_VideoCodec_eH264, 0x1223, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eTs},
#endif

void print_status(NEXUS_PlaybackHandle playback, NEXUS_StcChannelHandle stcChannel, NEXUS_VideoDecoderHandle vdecode, NEXUS_AudioDecoderHandle pcmDecoder, NEXUS_AudioDecoderHandle compressedDecoder)
{
    NEXUS_PlaybackStatus status;
    NEXUS_VideoDecoderStatus vstatus;
    NEXUS_AudioDecoderStatus astatus0, astatus1;
    uint32_t stc;

    NEXUS_Playback_GetStatus(playback, &status);
    NEXUS_VideoDecoder_GetStatus(vdecode, &vstatus);
    NEXUS_AudioDecoder_GetStatus(pcmDecoder, &astatus0);
    NEXUS_StcChannel_GetStc(stcChannel, &stc);
    if (compressedDecoder) {
        NEXUS_AudioDecoder_GetStatus(compressedDecoder, &astatus1);
    }
    else {
        BKNI_Memset(&astatus1, 0, sizeof(astatus1));
    }
    printf("state playback=%d%%, video=%d%%/pts=%#x/pics=%d, audio0=%d%%/pts=%#x, audio1=%d%%/pts=%#x, stc=%#x\n",
        status.fifoSize?status.fifoDepth*100/status.fifoSize:0,
        vstatus.fifoSize?vstatus.fifoDepth*100/vstatus.fifoSize:0, vstatus.pts, vstatus.queueDepth,
        astatus0.fifoSize?astatus0.fifoDepth*100/astatus0.fifoSize:0, astatus0.pts,
        astatus1.fifoSize?astatus1.fifoDepth*100/astatus1.fifoSize:0, astatus1.pts,
        stc);
}

int main(int argc, char **argv) {
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder = NULL;
    NEXUS_AudioDecoderStartSettings audioProgram;
    unsigned i;
    unsigned loops = 8;
    bool compressedDecoderUsed = false;
    NEXUS_Error rc;
    unsigned index = 0;
    bool accurateSeek = true;
    NEXUS_PlaybackPidChannelSettings playbackPidChannelSettings;
    unsigned max_sleep = 5 * 1000;

    srand((unsigned)time(NULL));

    if (argc > 1) loops = atoi(argv[1]);
    if (argc > 2) index = atoi(argv[2]);
    if (argc > 3) accurateSeek = atoi(argv[3]);
    if (argc > 4) max_sleep = atoi(argv[4]);

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Bring up audio decoders and outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if (compressedDecoderUsed) {
        compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }

    /* Bring up video display and outputs */
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    display = NEXUS_Display_Open(0, NULL);
 #if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    stcChannel = NEXUS_StcChannel_Open(0, NULL);
    playpump = NEXUS_Playpump_Open(0, NULL);
    playback = NEXUS_Playback_Create();
    vdecode = NEXUS_VideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
    BDBG_ASSERT(!rc);

    for (;g_teststreams[index].filename;index++) {
        file = NEXUS_FilePlay_OpenPosix(g_teststreams[index].filename, g_teststreams[index].indexname?g_teststreams[index].indexname:g_teststreams[index].filename);
        if (!file) {
            fprintf(stderr, "can't open file %s\n", g_teststreams[index].filename);
            return -1;
        }

        NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
        stcSettings.timebase = NEXUS_Timebase_e0;
        stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
        stcSettings.modeSettings.Auto.transportType = g_teststreams[index].transportType;
        stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;
        rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);

        NEXUS_Playback_GetSettings(playback, &playbackSettings);
        playbackSettings.playpump = playpump;
        /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
        playbackSettings.playpumpSettings.transportType = g_teststreams[index].transportType;
        playbackSettings.stcChannel = stcChannel;
        playbackSettings.accurateSeek = accurateSeek; /* defaults to true for this app */
        NEXUS_Playback_SetSettings(playback, &playbackSettings);

        /* Open the audio and video pid channels */
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidChannelSettings);
        playbackPidChannelSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidChannelSettings.pidTypeSettings.video.codec = g_teststreams[index].video_codec;
        playbackPidChannelSettings.pidTypeSettings.video.decoder = vdecode;
        videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, g_teststreams[index].video_pid, &playbackPidChannelSettings);
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidChannelSettings);
        playbackPidChannelSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidChannelSettings.pidTypeSettings.audio.primary = pcmDecoder;
        if (compressedDecoderUsed) {
            playbackPidChannelSettings.pidTypeSettings.audio.secondary = compressedDecoder;
        }
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, g_teststreams[index].audio_pid, &playbackPidChannelSettings);

        /* Set up decoder Start structures now. We need to know the audio codec to properly set up
        the audio outputs. */
        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
        videoProgram.codec = g_teststreams[index].video_codec;
        videoProgram.pidChannel = videoPidChannel;
        videoProgram.stcChannel = stcChannel;
        NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
        audioProgram.codec = g_teststreams[index].audio_codec;
        audioProgram.pidChannel = audioPidChannel;
        audioProgram.stcChannel = stcChannel;

        /* Start Decoders */
        rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
        BDBG_ASSERT(!rc);
        rc = NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
        BDBG_ASSERT(!rc);
        if (compressedDecoderUsed) {
            rc = NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
            BDBG_ASSERT(!rc);
        }
        rc = NEXUS_Playback_Start(playback, file, NULL);
        BDBG_ASSERT(!rc);

        for(i=0;i<loops;i++) {
            NEXUS_PlaybackStatus status;
            unsigned duration = 2000 + (rand() % max_sleep); /* sleep for a random time up to a certain max */

            printf("playing %s for %d msec\n", g_teststreams[index].filename, duration);
            duration -= duration%250;
            while (duration > 0) {
                print_status(playback, stcChannel, vdecode, pcmDecoder, compressedDecoder);
                BKNI_Sleep(250);
                duration -= 250;
            }

            /* have playback seek backwards to our current location so that we can resume video in the same
            spot and hear audio right away */
            rc = NEXUS_Playback_Pause(playback);
            BDBG_ASSERT(!rc);
            rc = NEXUS_Playback_GetStatus(playback, &status);
            BDBG_ASSERT(!rc);
            printf("paused at pos %d\n", (unsigned)status.position);

            /* Reconfigure audio here... */
            if (compressedDecoderUsed) {
                NEXUS_AudioDecoder_Stop(compressedDecoder); /* must stop this first to avoid PCRlib errors */
            }
            NEXUS_AudioDecoder_Stop(pcmDecoder);
            /* do any reconfiguration needed here */
            rc = NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram); /* must start this first to avoid PCRlib errors */
            BDBG_ASSERT(!rc);
            if (compressedDecoderUsed) {
                rc = NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
                BDBG_ASSERT(!rc);
            }

            rc = NEXUS_Playback_Seek(playback, status.position);
            BDBG_ASSERT(!rc);

            /* accurateSeek logic will be invoked from inside Playback */
            rc = NEXUS_Playback_Play(playback);
            BDBG_ASSERT(!rc);
        }

        NEXUS_Playback_Stop(playback);
        NEXUS_FilePlay_Close(file);
        if (compressedDecoderUsed) {
            NEXUS_AudioDecoder_Stop(compressedDecoder);
        }
        NEXUS_AudioDecoder_Stop(pcmDecoder);
        NEXUS_VideoDecoder_Stop(vdecode);
        NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
        NEXUS_Playback_ClosePidChannel(playback, audioPidChannel);
    }

    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if (compressedDecoder) {
        NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
		NEXUS_AudioDecoder_Close(compressedDecoder);
    }
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_Platform_Uninit();
    return 0;
}
