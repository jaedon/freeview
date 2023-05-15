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
 * $brcm_Workfile: playback_list.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 3/7/12 10:49a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/playback_list.c $
 * 
 * Hydra_Software_Devel/7   3/7/12 10:49a erickson
 * SW7425-2507: some chips don't support DSS, so allow certain failure
 * 
 * Hydra_Software_Devel/6   3/2/12 11:52a erickson
 * SW7425-2507: extend unittest so it would have caught this error
 *
 * Hydra_Software_Devel/5   5/10/10 6:57p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 *
 * Hydra_Software_Devel/4   4/3/09 12:58p erickson
 * PR53824: set required pid channel settings.
 *
 * Hydra_Software_Devel/3   1/30/09 10:27a erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/2   11/3/08 3:19p erickson
 * PR48517: fix shutdown of app
 *
 * Hydra_Software_Devel/1   10/29/08 5:22p erickson
 * PR48517: added playback_list test
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
#include "nexus_video_input.h"
#include "nexus_core_utils.h"
#include "nexus_component_output.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "nexus_playback.h"

BDBG_MODULE(playback_list);

struct teststreams_t {
    const char *filename;
    unsigned short video_pid;
    NEXUS_VideoCodec video_codec;
    unsigned short audio_pid;
    NEXUS_AudioCodec audio_codec;
    NEXUS_TransportType transportType;
} g_teststreams[] =
{
    {"videos/dss_ppv_preview.mpg", 0x78, NEXUS_VideoCodec_eMpeg2, 0x70, NEXUS_AudioCodec_eMpeg, NEXUS_TransportType_eDssEs},
    {"videos/discoveryAvcHD.mpg", 0x1522, NEXUS_VideoCodec_eH264, 0x1523, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eTs},
    {"videos/cnnticker.mpg", 0x21, NEXUS_VideoCodec_eMpeg2, 0x22, NEXUS_AudioCodec_eMpeg, NEXUS_TransportType_eTs},
    {"videos/herbie1AvcHD.mpg", 0x1222, NEXUS_VideoCodec_eH264, 0x1223, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eTs},
    {"videos/riddick_avc_720p.mpg", 0x1422, NEXUS_VideoCodec_eH264, 0x1423, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eTs}
};

static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_Error rc;
    unsigned i;
    unsigned loop;
    BKNI_EventHandle firstPtsPassed, endOfStream;

    srand(time(NULL));

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&firstPtsPassed);
    BKNI_CreateEvent(&endOfStream);

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
    playbackSettings.endOfStreamCallback.callback = complete;
    playbackSettings.endOfStreamCallback.context = endOfStream;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    rc = NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    BDBG_ASSERT(!rc);

    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.firstPtsPassed.callback = complete;
    videoDecoderSettings.firstPtsPassed.context = firstPtsPassed;
    rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    BDBG_ASSERT(!rc);

    for (loop=0;loop<5;loop++) {
        rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
        BDBG_ASSERT(!rc);

        for (i=0;i<sizeof(g_teststreams)/sizeof(g_teststreams[0]);i++) {
            NEXUS_PlaybackPidChannelSettings playbackPidCfg;

            NEXUS_Playback_GetSettings(playback, &playbackSettings);
            playbackSettings.playpumpSettings.transportType = g_teststreams[i].transportType;
            rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
            BDBG_ASSERT(!rc);

            NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
            stcSettings.modeSettings.Auto.transportType = g_teststreams[i].transportType;
            rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
            BDBG_ASSERT(!rc);

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

            BKNI_ResetEvent(firstPtsPassed);
            BKNI_ResetEvent(endOfStream);
            rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
            BDBG_ASSERT(!rc);
            rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
            BDBG_ASSERT(!rc);

            printf("opening %s\n", g_teststreams[i].filename);
            file = NEXUS_FilePlay_OpenPosix(g_teststreams[i].filename, NULL);
            BDBG_ASSERT(file);

            rc = NEXUS_Playback_Start(playback, file, NULL);
            if (rc && NEXUS_IS_DSS_MODE(playbackSettings.playpumpSettings.transportType)) {
                /* some platforms don't support DSS, so allow this failure */
                goto close_file;
            }
            BDBG_ASSERT(!rc);

            switch (loop) {
            case 0:
                printf("play for 5 seconds, with no end of stream\n");
                rc = BKNI_WaitForEvent(endOfStream, 5000);
                BDBG_ASSERT(rc == NEXUS_TIMEOUT);
                break;
            case 1:
                printf("immediate stop\n");
                break;
            case 2:
                printf("random stop\n");
                BKNI_Sleep(rand() % 5000);
                break;
            case 3:
                {
                NEXUS_VideoDecoderTrickState trick;
                
                printf("go in/out of trick modes\n");
                NEXUS_VideoDecoder_GetTrickState(videoDecoder, &trick);
                trick.hostTrickModesEnabled = true;
                trick.tsmEnabled = false;
                trick.reverseFields = true;
                rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trick);
                BDBG_ASSERT(!rc);

                BKNI_Sleep(1);
                                
                NEXUS_VideoDecoder_GetNormalPlay(&trick);
                rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trick);
                BDBG_ASSERT(!rc);
                
                BKNI_Sleep(1);
                
                NEXUS_VideoDecoder_Flush(videoDecoder);
                
                BKNI_Sleep(1);
                }
                break;
            case 4:
                {
                unsigned oneSecond = NEXUS_IS_DSS_MODE(g_teststreams[i].transportType)?27000000:45000;
                NEXUS_VideoDecoderStatus status;
                unsigned currentPts;
                unsigned count = 20;

                printf("PTS test\n");
                rc = BKNI_WaitForEvent(firstPtsPassed, 2000);
                BDBG_ASSERT(!rc);
                rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                BDBG_ASSERT(!rc);
                currentPts = status.pts;
#define ABS(X) (((X)>0)?(X):-(X))
                while (1) {
                    unsigned testPts;
                    BKNI_Sleep(1000);
                    rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                    BDBG_ASSERT(!rc);
                    BDBG_WRN(("pts %#x", status.pts));
                    testPts = currentPts + oneSecond;
                    if (ABS((int)(status.pts-testPts) > (int)oneSecond*1/8)) {
                        BDBG_ERR(("bad PTS: expected %#x, got %#x", testPts, status.pts));
                        BKNI_Fail();
                    }
                    currentPts = status.pts;

                    if (!--count) break;
                }
                }
                break;
            }

            NEXUS_Playback_Stop(playback);
close_file:
            NEXUS_FilePlay_Close(file);
            NEXUS_AudioDecoder_Stop(audioDecoder);
            NEXUS_VideoDecoder_Stop(videoDecoder);

            NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
            NEXUS_Playback_ClosePidChannel(playback, audioPidChannel);

        }
        NEXUS_VideoWindow_RemoveAllInputs(window);
    }

    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    BKNI_DestroyEvent(firstPtsPassed);
    BKNI_DestroyEvent(endOfStream);
    NEXUS_Platform_Uninit();

    return 0;
}
