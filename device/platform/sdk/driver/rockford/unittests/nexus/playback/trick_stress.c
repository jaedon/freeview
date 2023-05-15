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
 * $brcm_Workfile: trick_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 9/8/11 2:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/trick_stress.c $
 * 
 * Hydra_Software_Devel/7   9/8/11 2:09p katrep
 * SW7231-4:fixed the uninit
 * 
 * Hydra_Software_Devel/6   8/8/11 11:52a erickson
 * SWDEPRECATED-3044: speed up tests
 * 
 * Hydra_Software_Devel/5   6/1/09 12:09p erickson
 * PR42679: make finite
 *
 * Hydra_Software_Devel/4   5/4/09 4:47p erickson
 * PR42679: shorten run time
 *
 * Hydra_Software_Devel/3   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/2   11/6/08 2:13p erickson
 * PR42679: remove known failing case
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
#include "bstd.h"
#include "bkni.h"
BDBG_MODULE(trick_stress);

int main(void) {
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    const char *fname = "videos/cnnticker.mpg";
    const char *index = "videos/cnnticker.nav";
    ExampleDecodeSettings decodeSettings;
    unsigned i;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);

    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);

    file = NEXUS_FilePlay_OpenPosix(fname, index);
    BDBG_ASSERT(file);

    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    for(i=0;i<30;i++) {
        unsigned j;

        printf("iteration %d\n", i);

        ExampleDecode_GetDefaultSettings(&decodeSettings);
        decodeSettings.videoPid = 0x21;
        decodeSettings.videoCodec = NEXUS_VideoCodec_eMpeg2;
        decodeSettings.audioPid = 0x22;
        decodeSettings.audioCodec = NEXUS_AudioCodec_eMpeg;
        decodeSettings.playback = playback;
        decodeSettings.playpump = playpump;
        rc = StartExampleDecode(&decodeSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Playback_Start(playback, file, NULL);
        BDBG_ASSERT(!rc);

        BKNI_Sleep(rand() % 1000);

        for (j=0;j<5;j++) {
            NEXUS_PlaybackTrickModeSettings trick;

            NEXUS_Playback_GetDefaultTrickModeSettings(&trick);
            trick.rate = (int)(rand() % NEXUS_NORMAL_PLAY_SPEED*10) - NEXUS_NORMAL_PLAY_SPEED*5;

            /* special case: this rate is only supported with stc trick modes */
            if (trick.rate > NEXUS_NORMAL_PLAY_SPEED && trick.rate < NEXUS_NORMAL_PLAY_SPEED*2) {
                trick.rate = NEXUS_NORMAL_PLAY_SPEED * 2;
            }

            rc = NEXUS_Playback_TrickMode(playback, &trick);
            BDBG_ASSERT(!rc);

            BKNI_Sleep(rand() % 2000);
        }

        NEXUS_Playback_Stop(playback);
        StopExampleDecode();
    }

	UninitExampleDecode();
    NEXUS_Playback_Destroy(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playpump_Close(playpump);
    NEXUS_Platform_Uninit();
    return 0;
}
