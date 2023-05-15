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
 * $brcm_Workfile: dqt_trick.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/24/12 1:54p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/dqt_trick.c $
 * 
 * Hydra_Software_Devel/2   7/24/12 1:54p jessem
 * SW7425-3568: Added NEXUS_FilePlay_Close.
 *
 * Hydra_Software_Devel/1   10/12/11 11:46a erickson
 * SW7425-1261: add dqt unittest
 *
 * 2   3/22/10 2:29a randyjew
 * SW7468-143: Support for boards without component and/or composite
 *
 * 1   7/13/09 3:57p jtna
 * PR56423: added dqt_trick.c
 *
 *****************************************************************************/
/* Nexus example app */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_playback.h"
#include "nexus_file.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(dqt_trick);

int main(void)  {
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    const char *fname = "videos/cnnticker.mpg";
    const char *index = "videos/cnnticker.nav";
    NEXUS_Error rc;
    unsigned loops = 10;

    srand(time(NULL));

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    file = NEXUS_FilePlay_OpenPosix(fname, index);
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* No audio decode to keep things simple */

    /* Bring up video display and outputs */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* Open the audio and video pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2; /* must be told codec for correct handling */
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x21, &playbackPidSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    rc = NEXUS_Playback_Start(playback, file, NULL);
    BDBG_ASSERT(!rc);

    while (loops--) {
        NEXUS_PlaybackTrickModeSettings playbackTrick;
        uint32_t prev_pts = 0;
        unsigned i, tests = 50, one_picture_steps = 0;

        NEXUS_Playback_GetDefaultTrickModeSettings(&playbackTrick);
        playbackTrick.skipControl = NEXUS_PlaybackSkipControl_eHost;
        playbackTrick.rateControl = NEXUS_PlaybackRateControl_eDecoder;
        playbackTrick.rate = NEXUS_NORMAL_PLAY_SPEED/-5; /* add 5x slow motion so we can poll */
        playbackTrick.mode = NEXUS_PlaybackHostTrickMode_ePlayGop;
        {
            unsigned gop_skip = rand()%3;
            unsigned frames_per_gop = rand()%5 + 7; /* at least 7. remember: 2 B's will be lost. */
            playbackTrick.mode_modifier = -1 * (100*gop_skip + frames_per_gop);
        }
        BDBG_WRN(("GOP trick %d, rate %d", playbackTrick.mode_modifier, playbackTrick.rate));
        rc = NEXUS_Playback_TrickMode(playback, &playbackTrick);
        BDBG_ASSERT(!rc);

        /* monitor PTS's */
        for (i=0;i<tests;i++) {
            uint32_t pts;
            unsigned poll_loops = 300;

            /* poll until PTS changes */
            while (poll_loops--) {
                NEXUS_VideoDecoderStatus status;
                rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                BDBG_ASSERT(!rc);
                pts = status.pts;
                if (pts && pts != prev_pts) break;
                BKNI_Sleep(10);
            }
            BDBG_ASSERT(poll_loops); /* if 0, we must have stuck for 3 seconds */

            /* is PTS for previous picture? this stream is 30fs, so its 1500. +200 for some tolerance. */
            BDBG_MSG(("%08x --> %08x", prev_pts, pts));
            if (pts < prev_pts && prev_pts - pts < 1700) {
                one_picture_steps++;
            }

            prev_pts = pts;
        }

        /* the number of one_picture_steps can never == tests because we skip GOP's and have stream
        loop around. but it should be a high percentage. */
        if (one_picture_steps < tests * 50/100) {
            BDBG_ERR(("only got %d out of %d one picture steps", one_picture_steps, tests));
            BKNI_Fail();
        }
        BDBG_WRN(("pass with %d out of %d one picture steps", one_picture_steps, tests));

        NEXUS_Playback_Play(playback);
    }

    NEXUS_VideoDecoder_Stop(videoDecoder);
	NEXUS_FilePlay_Close(file);
    NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_Platform_Uninit();

    return 0;
}
