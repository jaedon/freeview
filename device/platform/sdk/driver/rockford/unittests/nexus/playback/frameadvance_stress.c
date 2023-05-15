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
 * $brcm_Workfile: frameadvance_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 10/31/12 9:13a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/frameadvance_stress.c $
 * 
 * Hydra_Software_Devel/12   10/31/12 9:13a erickson
 * SW7400-3052: fix logic for determining direction of frameadvance
 * 
 * Hydra_Software_Devel/11   10/18/11 8:59a erickson
 * SW7400-3052: do one frameadvance before reading first PTS. the decoder
 * requires forward frame advance to start at the beginning of the GOP,
 * which might be a hop from our current location.
 *
 * Hydra_Software_Devel/10   8/24/11 6:53p mward
 * SW7400-3052:  Stop test immediately when failure is detected.
 *
 * Hydra_Software_Devel/9   8/23/11 3:39p mward
 * SW7400-3052:  Don't advance past ends of the file.
 *
 * Hydra_Software_Devel/8   8/8/11 11:52a erickson
 * SWDEPRECATED-3044: speed up tests
 *
 * Hydra_Software_Devel/7   5/6/09 10:40a erickson
 * PR42679: give frame advance minimum sleep time to prevent XVD errors
 *
 * Hydra_Software_Devel/6   5/4/09 4:47p erickson
 * PR42679: shorten run time
 *
 * Hydra_Software_Devel/5   4/6/09 3:33p erickson
 * PR42679: fix looping
 *
 * Hydra_Software_Devel/4   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/3   12/8/08 11:18a erickson
 * PR49930: verify test
 *
 * Hydra_Software_Devel/2   10/24/08 10:59a erickson
 * PR42679: fix for loops, reduce sleep
 *
 * Hydra_Software_Devel/1   10/24/08 10:34a erickson
 * PR48340: update
 *
 * Hydra_Software_Devel/2   5/22/08 1:22p erickson
 * PR42679: fix return codes
 *
 * Hydra_Software_Devel/1   5/7/08 3:51p erickson
 * PR42504: add playback unittests
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_playpump.h"
#include "nexus_playback.h"
#include "examples_lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"
BDBG_MODULE(frameadvance_stress);

extern NEXUS_VideoDecoderHandle g_videoDecoder;

int main(void) {
    NEXUS_PlatformSettings platformSettings;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    const char *fname = "videos/cnnticker.mpg";
    const char *index = "videos/cnnticker.nav";
    ExampleDecodeSettings decodeSettings;
    NEXUS_FilePosition firstPosition,lastPosition;
    unsigned i, j;
    NEXUS_Error rc;
    int apprc = 0;

    srand(time(NULL));
    
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);

    file = NEXUS_FilePlay_OpenPosix(fname, index);
    BDBG_ASSERT(file);

    BDBG_ASSERT(NEXUS_SUCCESS == NEXUS_FilePlay_GetBounds(file,&firstPosition,&lastPosition));

    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.beginningOfStreamAction = NEXUS_PlaybackLoopMode_ePause;
    playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePause;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    for(i=0;i<15;i++) {
        unsigned j_limit, i_limit;
        NEXUS_PlaybackStatus playback_status;

        ExampleDecode_GetDefaultSettings(&decodeSettings);
        decodeSettings.videoPid = 0x21;
        decodeSettings.videoCodec = NEXUS_VideoCodec_eMpeg2;
        decodeSettings.audioPid = 0x22;
        decodeSettings.audioCodec = NEXUS_AudioCodec_eMpeg;
        decodeSettings.playback = playback;
        decodeSettings.playpump = playpump;
        rc = StartExampleDecode(&decodeSettings);
        BDBG_ASSERT(!rc);

        BDBG_WRN(("start playback %d", i));
        rc = NEXUS_Playback_Start(playback, file, NULL);
        BDBG_ASSERT(!rc);

        BKNI_Sleep(rand() % 1500);

        rc = NEXUS_Playback_Pause(playback);
        BDBG_ASSERT(!rc);

        j_limit = rand()%6;
        for (j=0;j<j_limit;j++) {
            NEXUS_VideoDecoderStatus startStatus, endStatus;
            bool direction = rand()%2;
            unsigned diff, i;

            i_limit = rand()%40 + 20;

            /* go forward one to get a PTS */
            rc = NEXUS_Playback_FrameAdvance(playback, true);
            BDBG_ASSERT(!rc);
            
            /* now get status to decide what we will do */
            rc = NEXUS_VideoDecoder_GetStatus(g_videoDecoder, &startStatus);
            BDBG_ASSERT(!rc);
            rc = NEXUS_Playback_GetStatus(playback,&playback_status);
            BDBG_ASSERT(!rc);
            
            if (playback_status.position < 40*i_limit) {
                direction = true; /* forward to avoid beginning of file */
            }
            else if (playback_status.position > lastPosition.timestamp-(40*i_limit)) {
                direction = false; /* reverse to avoid end of file */
            }
            BDBG_WRN(("frameadvance %s %d times from pos=%d,pts=%#x with bound %d,%d", direction?"forward":"backward", i_limit, 
                playback_status.position, startStatus.pts, playback_status.first, playback_status.last));

            for (i=0;i<i_limit;i++) {
                rc = NEXUS_Playback_FrameAdvance(playback, direction);
                BDBG_ASSERT(!rc);
                /* frameadvance requires a minimum amount of time, otherwise video decoder may not be ready */
                BKNI_Sleep(34 + (rand() % 70));
            }
            rc = NEXUS_VideoDecoder_GetStatus(g_videoDecoder, &endStatus);
            BDBG_ASSERT(!rc);
            diff = direction ? endStatus.pts-startStatus.pts : startStatus.pts-endStatus.pts;
            if (diff < i_limit*1500/2) {
                BDBG_ERR(("frame advance didn't go far enough: %#x -> %#x, diff %d, expecting at least %d",
                    startStatus.pts, endStatus.pts, diff,
                    i_limit*1500/2));
                apprc = -1;
                break;
            }
        }

        NEXUS_Playback_Stop(playback);
        StopExampleDecode();
        if (apprc) {
            break;
        }
    }

    NEXUS_Playback_Destroy(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playpump_Close(playpump);
    return apprc;
}
