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
 * $brcm_Workfile: playback_stctrick.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 3/2/12 12:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/playback_stctrick.c $
 * 
 * Hydra_Software_Devel/9   3/2/12 12:05p randyjew
 * SW7468-6:Increase time to get first PTS 4000
 * 
 * Hydra_Software_Devel/8   5/26/10 1:01p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/7   5/10/10 6:57p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 * 
 * Hydra_Software_Devel/6   10/14/09 1:17p mphillip
 * SW7405-3034: Convert assert to error condition and alert
 * 
 * Hydra_Software_Devel/5   5/6/09 9:54a erickson
 * PR42679: remove 4x stc trick. this is not supported with AVC decode.
 *
 * Hydra_Software_Devel/4   4/3/09 12:57p erickson
 * PR53824: use Playback for stc trick. set required pid channel settings.
 *
 * Hydra_Software_Devel/3   10/29/08 10:06a erickson
 * PR48406: update
 *
 * Hydra_Software_Devel/2   10/27/08 3:14p erickson
 * PR48406: added multiple streams, explicit PTS/STC rate test
 *
 * Hydra_Software_Devel/1   9/8/08 11:33a erickson
 * PR42679: added stctrick test
 *
 *****************************************************************************/
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
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "examples_lib.h"

BDBG_MODULE(playback_stctrick);

struct teststreams_t{
    const char *filename;
    unsigned short video_pid;
    NEXUS_VideoCodec video_codec;
    unsigned short audio_pid;
    NEXUS_AudioCodec audio_codec;
} g_teststreams[] =
{
    {"videos/discoveryAvcHD.mpg", 0x1522, NEXUS_VideoCodec_eH264, 0x1523, NEXUS_AudioCodec_eAc3},
    {"videos/cnnticker.mpg", 0x21, NEXUS_VideoCodec_eMpeg2, 0x22, NEXUS_AudioCodec_eMpeg},
    {"videos/herbie1AvcHD.mpg", 0x1222, NEXUS_VideoCodec_eH264, 0x1223, NEXUS_AudioCodec_eAc3},
    {"videos/riddick_avc_720p.mpg", 0x1422, NEXUS_VideoCodec_eH264, 0x1423, NEXUS_AudioCodec_eAc3},
    {NULL, 0, NEXUS_VideoCodec_eUnknown, 0, NEXUS_AudioCodec_eUnknown}
};

NEXUS_StcChannelHandle stcChannel;
NEXUS_VideoDecoderHandle videoDecoder;
NEXUS_PlaybackHandle playback;
int g_failure = 0;

void stc_trick_test(unsigned num, unsigned denom)
{
    uint32_t start_stc, start_pts;
    uint32_t end_stc, end_pts;
    NEXUS_VideoDecoderStatus status;
    NEXUS_Error rc;
    float desired_rate, actual_rate, diff_rate;

    while (1) {
        NEXUS_PlaybackTrickModeSettings trickModeSettings;

        NEXUS_StcChannel_GetStc(stcChannel, &start_stc);
        rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
        BDBG_ASSERT(!rc);
        start_pts = status.pts;

        /* By using Playback's rate, it will also set both audio and video trick modes */
        NEXUS_Playback_GetDefaultTrickModeSettings(&trickModeSettings);
        trickModeSettings.rate = num * NEXUS_NORMAL_PLAY_SPEED / denom;
        rc = NEXUS_Playback_TrickMode(playback, &trickModeSettings);
        /* Changing this assert to an alert and exit -- not sure that this should fail?
        BDBG_ASSERT(!rc); */
        if (rc) {
            BDBG_ERR(("Unsupported rate: %d/%d",num,denom));
            break;
        }

#define TEST_TIME 3000
        BKNI_Sleep(TEST_TIME);

        NEXUS_StcChannel_GetStc(stcChannel, &end_stc);
        rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
        BDBG_ASSERT(!rc);
        end_pts = status.pts;

        /* just try again if there was a wrap. assumes stream is longer than TEST_TIME msec */
        if (end_pts < start_pts || end_stc < end_stc)
            continue;

        desired_rate = (float)num / (float)denom;
        printf("desired rate %d / %d = %0.2f\n", num, denom, desired_rate);

        actual_rate = (end_stc - start_stc) / (45.0 * TEST_TIME);
        printf("stc moved %#x (%#x..%#x) in %d seconds (%0.2f rate)\n",
            end_stc - start_stc, end_stc, start_stc,
            TEST_TIME/1000,
            actual_rate);
        diff_rate = actual_rate - desired_rate;
        if (diff_rate < 0) diff_rate = -diff_rate;
        if (diff_rate > 0.2) {
            BDBG_ERR(("Unable to achieve target STC rate."));
            /* don't assert. we want to test all streams. */
            g_failure = 1;
        }

        actual_rate = (end_pts - start_pts) / (45.0 * TEST_TIME);
        printf("pts moved %#x in %d seconds (%0.2f rate)\n",
            end_pts - start_pts,
            TEST_TIME/1000,
            actual_rate);
        diff_rate = actual_rate - desired_rate;
        if (diff_rate < 0) diff_rate = -diff_rate;
        if (diff_rate > 0.2) {
            BDBG_ERR(("Unable to achieve target STC rate."));
            /* don't assert. we want to test all streams. */
            g_failure = 1;
        }

        break;
    }
}

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_Error rc;
    unsigned i;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    for (i=0;g_teststreams[i].filename;i++) {
        NEXUS_PlaybackPidChannelSettings playbackPidCfg;

        printf("opening %s\n", g_teststreams[i].filename);
        file = NEXUS_FilePlay_OpenPosix(g_teststreams[i].filename, NULL);
        BDBG_ASSERT(file);

        videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
        audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);

        /* Open the StcChannel to do lipsync with the PCR */
        NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
        stcSettings.timebase = NEXUS_Timebase_e0;
        stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
        stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

        playpump = NEXUS_Playpump_Open(0, NULL);
        playback = NEXUS_Playback_Create();
        NEXUS_Playback_GetSettings(playback, &playbackSettings);
        playbackSettings.playpump = playpump;
        playbackSettings.stcChannel = stcChannel;
        playbackSettings.stcTrick = true;
        rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
        BDBG_ASSERT(!rc);

        /* Open the audio and video pid channels */
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidCfg);
        playbackPidCfg.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidCfg.pidTypeSettings.video.codec = g_teststreams[i].video_codec;
        playbackPidCfg.pidTypeSettings.video.decoder = videoDecoder;
        playbackPidCfg.pidTypeSettings.video.index = true;
        videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, g_teststreams[i].video_pid, &playbackPidCfg);

        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidCfg);
        playbackPidCfg.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidCfg.pidTypeSettings.audio.primary = audioDecoder;
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, g_teststreams[i].audio_pid, &playbackPidCfg);

        /* Set up decoder Start structures now. We need to know the audio codec to properly set up
        the audio outputs. */
        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
        videoProgram.codec = g_teststreams[i].video_codec;
        videoProgram.pidChannel = videoPidChannel;
        videoProgram.stcChannel = stcChannel;
        NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
        audioProgram.pidChannel = audioPidChannel;
        audioProgram.stcChannel = stcChannel;
        audioProgram.codec = g_teststreams[i].audio_codec;

        NEXUS_AudioOutput_AddInput(
            NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

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
        NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

        /* Start Decoders */
        rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
        BDBG_ASSERT(!rc);
        rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
        BDBG_ASSERT(!rc);

        printf("normal play\n");
        rc = NEXUS_Playback_Start(playback, file, NULL);
        BKNI_Sleep(4000); /* give it plenty of time to get first PTS */

        stc_trick_test(1, 1);

        printf("0.8x stc ffwd (audio trick)\n");
        stc_trick_test(8, 10);

        printf("1.2x stc ffwd (audio trick)\n");
        stc_trick_test(12, 10);

        printf("1.5x stc ffwd\n");
        stc_trick_test(2, 1);

        printf("2x stc ffwd\n");
        stc_trick_test(2, 1);

#if 0
/* cannot acheive 4x with AVC */
        printf("4x stc ffwd\n");
        stc_trick_test(4, 1);
#endif

        printf("normal play\n");
        stc_trick_test(1, 1);

        NEXUS_AudioDecoder_Stop(audioDecoder);
        NEXUS_VideoDecoder_Stop(videoDecoder);
        NEXUS_Playback_Stop(playback);
        NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
        NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

        NEXUS_Playback_Destroy(playback);
        NEXUS_Playpump_Close(playpump);
        NEXUS_AudioDecoder_Close(audioDecoder);
        NEXUS_VideoWindow_Close(window);
        NEXUS_Display_Close(display);
        NEXUS_VideoDecoder_Close(videoDecoder);
        NEXUS_StcChannel_Close(stcChannel);
        NEXUS_FilePlay_Close(file);
    }

    NEXUS_Platform_Uninit();

    return g_failure;
}
