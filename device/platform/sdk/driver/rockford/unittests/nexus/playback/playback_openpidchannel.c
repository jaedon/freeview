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
 * $brcm_Workfile: playback_openpidchannel.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 8/17/12 10:53a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/playback_openpidchannel.c $
 * 
 * Hydra_Software_Devel/4   8/17/12 10:53a jessem
 * SW7425-3701: Added 7346 to the conditional check for PB 13 and 14.
 *
 * Hydra_Software_Devel/3   8/16/12 12:46p jessem
 * SW7425-3701: Added failure check on PBs 13 and 14 for 7422, 7425, and
 * 7344.
 *
 * Hydra_Software_Devel/2   7/30/12 10:55a erickson
 * SW7358-266: add unittest for playpump/playback open pid channel options
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_playback.h"

int main(void)
{
    unsigned i;
    NEXUS_PlaypumpHandle playpump[NEXUS_NUM_PLAYPUMPS];
    NEXUS_PlaybackHandle playback[NEXUS_NUM_PLAYPUMPS];
    struct {
        bool used;
        unsigned pid;
        unsigned pbIndex;
    } pidChannel[NEXUS_NUM_PID_CHANNELS];
    unsigned loops = 20000;
    int rc;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);
    BKNI_Memset(playback, 0, sizeof(playback));
    BKNI_Memset(pidChannel, 0, sizeof(pidChannel));

    while (loops--) {
        unsigned pbIndex = rand() % NEXUS_NUM_PLAYPUMPS;
        switch (rand() % 4) {
        case 0:
            if (!playback[pbIndex]) {
                NEXUS_PlaybackSettings playbackSettings;
                printf("open playback %d\n", pbIndex);
                playpump[pbIndex] = NEXUS_Playpump_Open(pbIndex, NULL);

				/* See SW7425-3701 */
#if (BCHP_CHIP == 7425) || (BCHP_CHIP == 7422) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346)
				if (pbIndex == 13 || pbIndex == 14)
				{
					BDBG_ASSERT(!playpump[pbIndex]);
				}
				else
#endif
                {
                    BDBG_ASSERT(playpump[pbIndex]);

                    playback[pbIndex] = NEXUS_Playback_Create();
                    BDBG_ASSERT(playback[pbIndex]);

                    NEXUS_Playback_GetSettings(playback[pbIndex], &playbackSettings);
                    playbackSettings.playpump = playpump[pbIndex];
                    rc = NEXUS_Playback_SetSettings(playback[pbIndex], &playbackSettings);
                    BDBG_ASSERT(!rc);
                }
            }
            break;
        case 1:
            if (playback[pbIndex] && rand()%30 == 0) {
                printf("close playback %d\n", pbIndex);
                NEXUS_Playback_Destroy(playback[pbIndex]);
                playback[pbIndex] = NULL;
                NEXUS_Playpump_Close(playpump[pbIndex]);
                playpump[pbIndex] = NULL;
                for (i=0;i<NEXUS_NUM_PID_CHANNELS;i++) {
                    if (pidChannel[i].pbIndex == pbIndex) {
                        pidChannel[i].used = false;
                    }
                }
            }
            break;
        case 2:
            if (playback[pbIndex]) {
                NEXUS_PidChannelHandle pidCh;
                NEXUS_PidChannelStatus status;
                NEXUS_PlaybackPidChannelSettings settings;
                unsigned pid = rand()%0x1fff;

                NEXUS_Playback_GetDefaultPidChannelSettings(&settings);
                switch (rand()%4) {
                default:
                    /* try to find match */
                    for (i=0;i<NEXUS_NUM_PID_CHANNELS;i++) {
                        if (pidChannel[i].used && pidChannel[i].pbIndex == pbIndex) {
                            /* change pids so we can test this case */
                            pid = pidChannel[i].pid;
                            settings.pidSettings.pidSettings.pidChannelIndex = i;
                            break;
                        }
                    }
                    if (i<NEXUS_NUM_PID_CHANNELS) break;
                    /* fallthrough */
                case 0: settings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_ANY; break;
                case 1: settings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE; break;
                case 2: settings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE; break;
                }
                /* TODO: it's possible the test fails because we run out of pid channels. for now the randomization appears well within those limits. */
                pidCh = NEXUS_Playback_OpenPidChannel(playback[pbIndex], pid, NULL);
                BDBG_ASSERT(pidCh);

                rc = NEXUS_PidChannel_GetStatus(pidCh, &status);
                BDBG_ASSERT(!rc);
                printf("playback %d: open pid %#x, ch %d=>%d = %#x\n", pbIndex, pid, settings.pidSettings.pidSettings.pidChannelIndex, status.pidChannelIndex, (unsigned)pidCh);
                if (pidChannel[status.pidChannelIndex].used) {
                    BDBG_ASSERT(pidChannel[status.pidChannelIndex].pid == pid);
                    BDBG_ASSERT(pidChannel[status.pidChannelIndex].pbIndex == pbIndex);
                }
                else {
                    pidChannel[status.pidChannelIndex].used = true;
                    pidChannel[status.pidChannelIndex].pid = pid;
                    pidChannel[status.pidChannelIndex].pbIndex = pbIndex;
                }
            }
            break;
        case 3:
            if (playback[pbIndex] && rand()%30 == 0) {
                NEXUS_Playback_CloseAllPidChannels(playback[pbIndex]);
                for (i=0;i<NEXUS_NUM_PID_CHANNELS;i++) {
                    if (pidChannel[i].pbIndex == pbIndex) {
                        pidChannel[i].used = false;
                    }
                }
            }
            break;
        }
    }

    for (i=0;i<NEXUS_NUM_PLAYPUMPS;i++) {
        if (playback[i]) {
            NEXUS_Playback_Destroy(playback[i]);
            NEXUS_Playpump_Close(playpump[i]);
        }
    }
    NEXUS_Platform_Uninit();
    printf("Success\n");
    return 0;
}

