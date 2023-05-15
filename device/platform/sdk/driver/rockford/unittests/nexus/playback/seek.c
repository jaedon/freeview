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
 * $brcm_Workfile: seek.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 6/12/12 4:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/seek.c $
 * 
 * Hydra_Software_Devel/8   6/12/12 4:44p mward
 * SW7435-56:  Use a different .avi file.  The FantasticFour.divx was not
 * compatible with the "finer threshold to detect not properly
 * multiplexed streams" introduced in SW7358-322.
 * 
 * Hydra_Software_Devel/7   2/21/12 12:23p erickson
 * SW7208-183: only require in-bounds seek to succeed
 * 
 * Hydra_Software_Devel/6   5/26/10 1:02p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/5   5/10/10 6:58p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 * 
 * Hydra_Software_Devel/4   5/6/09 9:51a erickson
 * PR42679: add index, reduce time
 *
 * Hydra_Software_Devel/3   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/2   1/12/09 4:28p erickson
 * PR49930: fix Seek params
 *
 * Hydra_Software_Devel/1   12/8/08 11:18a erickson
 * PR49930: add seek test
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#include "nexus_video_window.h"
#include "nexus_component_output.h"
#include "nexus_playpump.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#endif
#include <stdlib.h>

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
BDBG_MODULE(seek);

/* this is needed for avi seek right now. */
#define ADD_AUDIO_PIDCHANNEL 1

/* media framework does not fail out-of-bounds seek. bcmplayer does. */
#define TEST_SEEK_FAILURE 0

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
    {"videos/avi/RiverFrontStadiumGoesDown.divx", NULL, 1, NEXUS_VideoCodec_eMpeg4Part2, 2, NEXUS_AudioCodec_eMp3, NEXUS_TransportType_eAvi},
    {"videos/asf/Discoverers_1080ap.wmv", NULL, 2, NEXUS_VideoCodec_eVc1, 1, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eAsf},
    {"videos/discoveryAvcHD.mpg", "videos/discoveryAvcHD.nav", 0x1522, NEXUS_VideoCodec_eH264, 0x1523, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eTs},
    {"videos/cnnticker.mpg", "videos/cnnticker.nav", 0x21, NEXUS_VideoCodec_eMpeg2, 0x22, NEXUS_AudioCodec_eMpeg, NEXUS_TransportType_eTs},
    {"videos/herbie1AvcHD.mpg", "videos/herbie1AvcHD.nav", 0x1222, NEXUS_VideoCodec_eH264, 0x1223, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eTs},
    {"videos/riddick_avc_720p.mpg", "videos/riddick_avc_720p.nav", 0x1422, NEXUS_VideoCodec_eH264, 0x1423, NEXUS_AudioCodec_eAc3, NEXUS_TransportType_eTs},
    {NULL, NULL, 0, NEXUS_VideoCodec_eUnknown, 0, NEXUS_AudioCodec_eUnknown, NEXUS_TransportType_eUnknown}
};

int main(int argc, char **argv) {
#if NEXUS_HAS_PLAYBACK
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
#if ADD_AUDIO_PIDCHANNEL
    NEXUS_PidChannelHandle audioPidChannel;
#endif
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    unsigned index;
    NEXUS_Error rc;
    NEXUS_PlaybackPidChannelSettings playbackPidChannelSettings;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    playpump = NEXUS_Playpump_Open(0, NULL);
    stcChannel = NEXUS_StcChannel_Open(0, NULL);
    playback = NEXUS_Playback_Create();
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

    /* Bring up video display and outputs */
    display = NEXUS_Display_Open(0, NULL);
 #if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
    BDBG_ASSERT(!rc);

    for(index=0;g_teststreams[index].filename;index++) {
        unsigned loops = 25;

        if (argc > 1) loops = atoi(argv[1]);

        NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
        stcSettings.timebase = NEXUS_Timebase_e0;
        stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
        stcSettings.modeSettings.Auto.transportType = g_teststreams[index].transportType;
        stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;
        rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
        BDBG_ASSERT(!rc);

        NEXUS_Playback_GetSettings(playback, &playbackSettings);
        playbackSettings.playpump = playpump;
        playbackSettings.playpumpSettings.transportType = g_teststreams[index].transportType;
        playbackSettings.stcChannel = stcChannel;
        rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
        BDBG_ASSERT(!rc);

        file = NEXUS_FilePlay_OpenPosix(g_teststreams[index].filename,
            g_teststreams[index].indexname ? g_teststreams[index].indexname : g_teststreams[index].filename );
        if (!file) {
            BDBG_WRN(("unable to open w/ index"));
            file = NEXUS_FilePlay_OpenPosix(g_teststreams[index].filename, NULL);
            if (!file) {
                fprintf(stderr, "can't open file %s\n", g_teststreams[index].filename);
                goto error;
            }
        }

        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidChannelSettings);
        playbackPidChannelSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidChannelSettings.pidTypeSettings.video.codec = g_teststreams[index].video_codec;
        playbackPidChannelSettings.pidTypeSettings.video.decoder = vdecode;
        playbackPidChannelSettings.pidTypeSettings.video.index = true;
        videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, g_teststreams[index].video_pid, &playbackPidChannelSettings);

#if ADD_AUDIO_PIDCHANNEL
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidChannelSettings);
        playbackPidChannelSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidChannelSettings.pidTypeSettings.video.codec = g_teststreams[index].audio_codec;
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, g_teststreams[index].audio_pid, &playbackPidChannelSettings);
#endif
        
        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
        videoProgram.codec = g_teststreams[index].video_codec;
        videoProgram.pidChannel = videoPidChannel;
        videoProgram.stcChannel = stcChannel;

        /* Start Decoders */
        rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Playback_Start(playback, file, NULL);
        BDBG_ASSERT(!rc);

        while (--loops) {
            NEXUS_PlaybackStatus status;
            unsigned newpos;

            BKNI_Sleep(rand() % 1000);

            rc = NEXUS_Playback_GetStatus(playback, &status);
            BDBG_ASSERT(!rc);

#if TEST_SEEK_FAILURE
            if (rand()%2) {
                /* seek out of bounds */
                if (rand()%2 && status.first) {
                    newpos = rand()%status.first;
                }
                else {
                    newpos = status.last + rand();
                    if (newpos <= status.last) {
                        newpos = status.last+1;
                    }
                }
                printf("unsupported jump from %u to %u (bounds %u...%u)\n", (unsigned)status.position, newpos, (unsigned)status.first, (unsigned)status.last);
                rc = NEXUS_Playback_Seek(playback, newpos);
                BDBG_ASSERT(rc);
            }
            else 
#endif
            {
                /* seek in bounds */
                if (rand()%8==0) {
                    /* ensure we check boundaries */
                    newpos = status.first; break;
                }
                else if (rand()%8==0) {
                    /* ensure we check boundaries */
                    newpos = status.last; break;
                }
                else {
                    /* anywhere within bounds */
                    newpos = status.first + (rand() % (status.last-status.first+1));
                }
                printf("jumping from %u to %u (bounds %u...%u)\n", (unsigned)status.position, newpos, (unsigned)status.first, (unsigned)status.last);
                rc = NEXUS_Playback_Seek(playback, newpos);
                BDBG_ASSERT(!rc);
            }
        }

        NEXUS_Playback_Stop(playback);
        NEXUS_FilePlay_Close(file);
        NEXUS_VideoDecoder_Stop(vdecode);
#if ADD_AUDIO_PIDCHANNEL
        NEXUS_Playback_ClosePidChannel(playback, audioPidChannel);
#endif
        NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
    }

    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_Platform_Uninit();
    return 0;

error:
#endif
    return 1;
}
