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
 * $brcm_Workfile: codec_mismatch.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 9/14/11 2:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/codec_mismatch.c $
 * 
 * Hydra_Software_Devel/21   9/14/11 2:42p erickson
 * SW7405-5478: cleanup
 *
 * Hydra_Software_Devel/20   9/9/11 10:58a mward
 * SW7405-5478: Compiler warning for unused stcSettings.
 *
 * Hydra_Software_Devel/19   9/8/11 1:40p mward
 * SW7405-5478: Don't close stcChannel unless it was opened.
 *
 * Hydra_Software_Devel/18   9/7/11 4:10p erickson
 * SW7405-5478: add --only-video and --only-audio options. disable TSM
 * because StcChannel's "TSM deadlock" detection might hide a bug. expand
 * timeout.
 *
 * Hydra_Software_Devel/17   9/7/11 12:33p erickson
 * SW7405-5478: clarify failure message
 *
 * Hydra_Software_Devel/16   8/31/11 11:28a erickson
 * SW7405-5478: rework test. actually test mismatch. add randomization and
 * time limit. verify no watchdogs after decode.
 *
 * Hydra_Software_Devel/15   5/10/10 6:56p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 *
 * Hydra_Software_Devel/14   10/27/09 4:46p mphillip
 * SW7405-3034: Properly terminate on default run
 *
 * Hydra_Software_Devel/13   10/16/09 6:54p mphillip
 * SW7405-3034: Only run one test by default (use --all to run all the
 * tests)
 *
 * Hydra_Software_Devel/12   9/23/09 10:43a mphillip
 * SW7405-3034: Fix warning on build, pending astm test implementation
 *
 * Hydra_Software_Devel/11   5/14/09 12:13p jtna
 * PR54883: use vsync mode for now
 *
 * Hydra_Software_Devel/10   5/12/09 11:40a jtna
 * PR42679: refactor to add runtime options
 *
 * Hydra_Software_Devel/9   5/6/09 4:23p jtna
 * PR42679: query both video and audio decoders
 *
 * Hydra_Software_Devel/8   5/6/09 2:56p jtna
 * PR42679: add check for stuck/idle PTS. disabled for now
 *
 * Hydra_Software_Devel/7   5/6/09 10:40a erickson
 * PR42679: give stcChannel to playback
 *
 * Hydra_Software_Devel/6   5/4/09 4:47p erickson
 * PR42679: shorten run time
 *
 * Hydra_Software_Devel/5   4/3/09 1:01p erickson
 * PR53824: update
 *
 * Hydra_Software_Devel/4   12/17/08 4:04p jtna
 * PR42679: update
 *
 * Hydra_Software_Devel/3   11/6/08 11:39a jtna
 * PR42679: fixed stream transition
 *
 * Hydra_Software_Devel/2   11/3/08 12:27p jtna
 * PR42679: update
 *
 * Hydra_Software_Devel/1   11/3/08 12:19p jtna
 * PR42679: initial version
 *
 *
 *****************************************************************************/
 /* mismatched codec test */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_component_output.h"
#include "nexus_astm.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "examples_lib.h"

BDBG_MODULE(codec_mismatch);
BDBG_OBJECT_ID(codec_mismatch);

typedef struct streaminfo {
    const char *filename;
    unsigned short video_pid;
    NEXUS_VideoCodec video_codec;
    unsigned short audio_pid;
    NEXUS_AudioCodec audio_codec;
} streaminfo;

streaminfo g_teststreams[] =
{
    {"videos/discoveryAvcHD.mpg", 0x1522, NEXUS_VideoCodec_eH264, 0x1523, NEXUS_AudioCodec_eAc3},
    {"videos/cnnticker.mpg", 0x21, NEXUS_VideoCodec_eMpeg2, 0x22, NEXUS_AudioCodec_eMpeg},
    {"videos/herbie1AvcHD.mpg", 0x1222, NEXUS_VideoCodec_eH264, 0x1223, NEXUS_AudioCodec_eAc3},
    {"videos/riddick_avc_720p.mpg", 0x1422, NEXUS_VideoCodec_eH264, 0x1423, NEXUS_AudioCodec_eAc3},
    {NULL, 0, NEXUS_VideoCodec_eUnknown, 0, NEXUS_AudioCodec_eUnknown}
};
#define NUMSTREAMS (sizeof(g_teststreams)/sizeof(*g_teststreams)-1)

/* The test time for a given codec combination on a particular stream is PTS_POLL_TIME * PTS_POLL_LIMIT.
some failures do not occur immediately. setting limit to 20 seconds. */
#define PTS_POLL_TIME 400 /* in ms */
#define PTS_POLL_LIMIT 37

#define IDLE_CNT_LIMIT 5 /* max no of consecutive idle PTS's we tolerate for a given codec combination
                            before deeming the test a failure. set this to > PTS_POLL_LIMIT to ignore idle PTS's */

int main(int argc, char **argv)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_VideoDecoderStatus videoStatus;
    NEXUS_AudioDecoderStatus audioStatus;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_Error rc;
    uint32_t lastPts = 0;
    unsigned idleCnt = 0;
    unsigned ptsPollCnt;
    unsigned j;
    unsigned timeout = 165*1000; /* 15 seconds less than 3 minutes (increases chances that we don't exceed 3 minutes) */
    unsigned start_time;
#if NEXUS_HAS_ASTM
    NEXUS_AstmSettings astmSettings;
    NEXUS_AstmHandle astm;
#endif
    bool files_done = false;
    int curarg = 1;
    bool test_audio = true;
    bool test_video = true;

    srand(time(NULL));
    start_time = b_get_time();

    /* runtime options */
    while (curarg < argc) {
        if (!strcmp(argv[curarg],"--all")) {
            /* This test was taking too long for the automated unit tests.
             * This way, only one stream is run for the automated tests by default, but
             * all of them can still be tested.*/
            timeout = 0;
        }
        else if (!strcmp(argv[curarg],"--only-video")) {
            /* only test video (i.e. disable audio test) */
            test_audio = false;
        }
        else if (!strcmp(argv[curarg],"--only-audio")) {
            /* only test audio (i.e. disable video test) */
            test_video = false;
        }
        curarg++;
    }

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Open the StcChannel to do lipsync with the PCR */
#if 0
/* we could enable TSM, but the stcchannel's "TSM deadlock" detection must be disabled. otherwise, it might
flush and cause a bug to be undetected. */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
    stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
    stcSettings.modeSettings.Auto.behavior  = NEXUS_StcChannelAutoModeBehavior_eFirstAvailable; /* required for TSM testing w/ bad codecs */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);
#else
    /* enable this to test vsync mode */
    stcChannel = NULL;
    BSTD_UNUSED(stcSettings);
#endif

    playpump = NEXUS_Playpump_Open(0, NULL);
    playback = NEXUS_Playback_Create();
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    /* Bring up video display and outputs */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

#if 0 /* TODO */
#if NEXUS_HAS_ASTM
    /* create an astm instance and connect */
    NEXUS_Astm_GetDefaultSettings(&astmSettings);
    astmSettings.stcChannel = stcChannel;
    astmSettings.videoDecoder = videoDecoder;
    astmSettings.audioDecoder[0] = audioDecoder;
    astmSettings.stcMaster = videoDecoder; /* select master mode */
    astm = NEXUS_Astm_Create(&astmSettings);

    NEXUS_Astm_Start(astm);
#endif
#else
    /* eliminate warning from commenting out that code */
    BSTD_UNUSED(astm);
    BSTD_UNUSED(astmSettings);
#endif

    /* cycle through files */
    while (!files_done) {
        NEXUS_PlaybackPidChannelSettings playbackPidCfg;
        bool codecs_done = false;
        NEXUS_VideoCodec videoCodec = 0;
        NEXUS_AudioCodec audioCodec = 0;

        if (timeout) {
            j = rand()%NUMSTREAMS;
        }

        /* Open the audio and video pid channels */
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidCfg);
        playbackPidCfg.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidCfg.pidTypeSettings.video.codec = g_teststreams[j].video_codec;
        playbackPidCfg.pidTypeSettings.video.decoder = videoDecoder;
        playbackPidCfg.pidTypeSettings.video.index = true;
        videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, g_teststreams[j].video_pid, &playbackPidCfg);

        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidCfg);
        playbackPidCfg.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidCfg.pidTypeSettings.audio.primary = audioDecoder;
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, g_teststreams[j].audio_pid, &playbackPidCfg);

        printf("\nOpening %s\n", g_teststreams[j].filename);
        file = NEXUS_FilePlay_OpenPosix(g_teststreams[j].filename, NULL);
        BDBG_ASSERT(file);

        rc = NEXUS_Playback_Start(playback, file, NULL);
        BDBG_ASSERT(!rc);

        while (!codecs_done) {
            if (timeout) {
                /* we don't have time to test all codecs. so randomly select some. after multiple runs of the autotest, we will eventually test all. */
                if ((rand() % 2) && test_audio) {
                    videoCodec = g_teststreams[j].video_codec;
                    audioCodec = rand() % NEXUS_AudioCodec_eMax;
                }
                else if (test_video) {
                    videoCodec = rand() % NEXUS_VideoCodec_eMax;
                    audioCodec = g_teststreams[j].audio_codec;
                }
            }

            NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
            videoProgram.pidChannel = videoPidChannel;
            videoProgram.stcChannel = stcChannel;
            videoProgram.codec = (videoCodec==NEXUS_VideoCodec_eMax || !test_video)?g_teststreams[j].video_codec:videoCodec;
            NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
            audioProgram.pidChannel = audioPidChannel;
            audioProgram.stcChannel = stcChannel;
            audioProgram.codec = (audioCodec==NEXUS_AudioCodec_eMax || !test_audio)?g_teststreams[j].audio_codec:audioCodec;

            BDBG_WRN((">Testing video codec %d (%s), audio codec %d (%s), %s",
                videoProgram.codec, videoProgram.codec==g_teststreams[j].video_codec?"right":"wrong",
                audioProgram.codec, audioProgram.codec==g_teststreams[j].audio_codec?"right":"wrong",
                g_teststreams[j].filename));

            /* Start Decoders */
            rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
            if (rc==NEXUS_INVALID_PARAMETER || rc==NEXUS_NOT_SUPPORTED) {
                BDBG_WRN(("Unsupported video codec. Aborting this test."));
                goto next;
            }
            BDBG_ASSERT(!rc);

            rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
            if (rc==NEXUS_INVALID_PARAMETER || rc==NEXUS_NOT_SUPPORTED) {
                BDBG_WRN(("Unsupported audio codec. Aborting this test."));
                NEXUS_VideoDecoder_Stop(videoDecoder);
                goto next;
            }
            BDBG_ASSERT(!rc);

            ptsPollCnt = 0;
            while (ptsPollCnt++ < PTS_POLL_LIMIT) {
                BKNI_Sleep(PTS_POLL_TIME);
                NEXUS_AudioDecoder_GetStatus(audioDecoder, &audioStatus);
                NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);

                if (videoProgram.codec==g_teststreams[j].video_codec) { /* cycle through audio codecs */
                    printf("[%3d] vPTS=0x%08x, vFIFO=%2d%% (aFIFO=%2d%%)", ptsPollCnt, videoStatus.pts,
                        videoStatus.fifoSize ? videoStatus.fifoDepth*100/videoStatus.fifoSize : 0,
                        audioStatus.fifoSize ? audioStatus.fifoDepth*100/audioStatus.fifoSize : 0);
                    /* check for idle/stuck PTS */
                    if (videoStatus.pts == lastPts) {
                        printf("\tvideo PTS idle (%d)\n", ++idleCnt);
                        if (idleCnt > IDLE_CNT_LIMIT) {
                            printf("Unittest failure because audio decoder stopped consuming bad data. Stuck video PTS is a symptom of that.\n");
                            return 1;
                        }
                    }
                    else {
                        printf("\n");
                        idleCnt=0;
                    }
                    lastPts = videoStatus.pts;
                }
                else { /* cycle through video codecs */
                    printf("[%3d] aPTS=0x%08x, aFIFO=%2d%% (vFIFO=%2d%%)", ptsPollCnt, audioStatus.pts,
                        audioStatus.fifoSize ? audioStatus.fifoDepth*100/audioStatus.fifoSize : 0,
                        videoStatus.fifoSize ? videoStatus.fifoDepth*100/videoStatus.fifoSize : 0);
                    /* check for idle/stuck PTS */
                    if (audioStatus.pts == lastPts) {
                        printf("\taudio PTS idle (%d)\n", ++idleCnt);
                        if (idleCnt > IDLE_CNT_LIMIT) {
                            printf("Unittest failure because video decoder stopped consuming bad data. Stuck audio PTS is a symptom of that.\n");
                            return 1;
                        }
                    }
                    else {
                        printf("\n");
                        idleCnt=0;
                    }
                    lastPts = audioStatus.pts;
                }
            }

            /* even with lots of bad data, we should not get a watchdog */
            rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
            BDBG_ASSERT(!rc);
            BDBG_ASSERT(!videoStatus.numWatchdogs);
            rc = NEXUS_AudioDecoder_GetStatus(audioDecoder, &audioStatus);
            BDBG_ASSERT(!rc);
            BDBG_ASSERT(!audioStatus.numWatchdogs);

            NEXUS_AudioDecoder_Stop(audioDecoder);
            NEXUS_VideoDecoder_Stop(videoDecoder);

next:
            lastPts = 0;

            /* next codec */
            if (!timeout) {
                /* cycle through codecs */
                if (videoCodec<NEXUS_VideoCodec_eMax && test_video) {
                    videoCodec++;
                }
                else if (audioCodec<NEXUS_AudioCodec_eMax && test_audio) {
                    audioCodec++;
                }
                else {
                    codecs_done = true;
                }
            }
            else {
                unsigned duration = b_get_time() - start_time;
                files_done = (duration >= timeout);
                codecs_done = true; /* always pick another file */
            }
        }

        NEXUS_Playback_Stop(playback);
        NEXUS_FilePlay_Close(file);
        NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
        NEXUS_Playback_ClosePidChannel(playback, audioPidChannel);
        if (!timeout) {
            if (++j == NUMSTREAMS) files_done = true;
        }
    }

    printf("> Test completed\n");

#if 0 /* TODO */
    NEXUS_Astm_Stop(astm);
    NEXUS_Astm_Destroy(astm);
#endif
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_AudioDecoder_Close(audioDecoder);
    if (stcChannel) {
        NEXUS_StcChannel_Close(stcChannel);
    }
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);

    NEXUS_Platform_Uninit();

    return 0;
}
