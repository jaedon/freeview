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
 * $brcm_Workfile: open.c $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 11/4/11 11:06a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/open.c $
 * 
 * Hydra_Software_Devel/14   11/4/11 11:06a erickson
 * SW7231-391: restore previous test, a few tweaks
 * 
 * Hydra_Software_Devel/12   2/11/11 11:55a randyjew
 * SW7420-1120:Use NEXUS_NUM_PARSER_BANDS for call of
 * NEXUS_PidChannel_Open.
 * 
 * Hydra_Software_Devel/11   2/10/11 11:01a erickson
 * SW7420-1120: update test for non-dup NEXUS_PidChannelHandle, add
 * randomize test
 *
 * Hydra_Software_Devel/10   5/10/10 7:01p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 *
 * Hydra_Software_Devel/9   5/3/10 6:48p randyjew
 * SW7468-6: Use NEXUS_NUM_PID_CHANNELS instead of hardcoded 256.
 *
 * Hydra_Software_Devel/8   2/2/10 3:28p mward
 * SW7125-195:  Due to SW7335-638: fix pid channel refcnt if
 * NEXUS_PidChannelSettings.pidChannelIndex is specified, pidChannel2 now
 * can, and should, be closed.
 *
 * Hydra_Software_Devel/7   6/11/09 4:32p jtna
 * PR55817: add testcase for opening same pid on same pidChannelIndex
 *
 *****************************************************************************/
/* Nexus test app:
flavors of transport Open/Close
*/

#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_playpump.h"

int main(void)
{
    unsigned i, j;
    NEXUS_Error rc;
    NEXUS_PidChannelSettings pidChannelSettings;
    NEXUS_PidChannelHandle pidChannel, pidChannel2;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);

    printf("open test\n");
    for (i=0;i<1000;i++) {
        NEXUS_PidChannelStatus pidChannelStatus, pidChannelStatus2;
        NEXUS_ParserBand parserBand = (NEXUS_ParserBand)(NEXUS_ParserBand_e0 + (rand()%NEXUS_NUM_PARSER_BANDS));
        unsigned short pid = rand() % 20;

        pidChannel = NEXUS_PidChannel_Open(parserBand, pid, NULL);
        BDBG_ASSERT(pidChannel);
        rc = NEXUS_PidChannel_GetStatus(pidChannel, &pidChannelStatus);
        BDBG_ASSERT(!rc);

        pidChannel = NEXUS_PidChannel_Open(parserBand, pid, NULL);
        BDBG_ASSERT(pidChannel);
        rc = NEXUS_PidChannel_GetStatus(pidChannel, &pidChannelStatus2);
        BDBG_ASSERT(!rc);
        BDBG_ASSERT(pidChannelStatus.pidChannelIndex == pidChannelStatus2.pidChannelIndex); /* same HW pid channel */

        pidChannel = NEXUS_PidChannel_Open(parserBand, pid, NULL);
        BDBG_ASSERT(pidChannel);
        rc = NEXUS_PidChannel_GetStatus(pidChannel, &pidChannelStatus2);
        BDBG_ASSERT(!rc);
        BDBG_ASSERT(pidChannelStatus.pidChannelIndex == pidChannelStatus2.pidChannelIndex); /* same HW pid channel */

        pidChannel = NEXUS_PidChannel_Open(parserBand, pid, NULL);
        BDBG_ASSERT(pidChannel);
        rc = NEXUS_PidChannel_GetStatus(pidChannel, &pidChannelStatus2);
        BDBG_ASSERT(!rc);
        BDBG_ASSERT(pidChannelStatus.pidChannelIndex == pidChannelStatus2.pidChannelIndex); /* same HW pid channel */

        NEXUS_PidChannel_CloseAll(parserBand);

        pidChannel = NEXUS_PidChannel_Open(parserBand, pid, NULL);
        BDBG_ASSERT(pidChannel);
        NEXUS_PidChannel_Close(pidChannel);

        NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
        pidChannel = NEXUS_PidChannel_Open(parserBand, pid, &pidChannelSettings);
        BDBG_ASSERT(pidChannel);
        NEXUS_PidChannel_Close(pidChannel);
    }

    printf("verify status\n");
    for (i=0;i<1000;i++) {
        NEXUS_PidChannelStatus pidChannelStatus;
        NEXUS_ParserBand parserBand = (NEXUS_ParserBand)(NEXUS_ParserBand_e0 + (rand()%NEXUS_NUM_PARSER_BANDS));
        unsigned short pid = rand() % 20;

        pidChannel = NEXUS_PidChannel_Open(parserBand, pid, NULL);
        BDBG_ASSERT(pidChannel);

        rc = NEXUS_PidChannel_GetStatus(pidChannel, &pidChannelStatus);
        BDBG_ASSERT(!rc);

        BDBG_ASSERT(pidChannelStatus.playback == false);
        BDBG_ASSERT(pidChannelStatus.remappedPid == pid);
        BDBG_ASSERT(pidChannelStatus.transportType == NEXUS_TransportType_eTs);

        NEXUS_PidChannel_ConsumerStarted(pidChannel);

        NEXUS_PidChannel_Close(pidChannel);
    }

    printf("close all\n");
    for (i=0;i<10;i++) {
        NEXUS_ParserBand parserBand = (NEXUS_ParserBand)(NEXUS_ParserBand_e0 + (rand()%NEXUS_NUM_PARSER_BANDS));

        printf("%d\n", i);
        for (j=0;j<2000;j++) {
            unsigned short pid = rand() % NEXUS_NUM_PID_CHANNELS; /* set highest pid to equal max # of pid channels */
            pidChannel = NEXUS_PidChannel_Open(parserBand, pid, NULL);
            BDBG_ASSERT(pidChannel);
        }

        NEXUS_PidChannel_CloseAll(parserBand);
    }

    /* when specifying the same pidChannelIndex, opening the same pid should succeed */
    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.pidChannelIndex = 0;
    pidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 1, &pidChannelSettings);
    BDBG_ASSERT(pidChannel);
    pidChannelSettings.pidChannelIndex = 0;
    pidChannel2 = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 1, &pidChannelSettings);
    BDBG_ASSERT(pidChannel2);
    BDBG_ASSERT(pidChannel!=pidChannel2); /* duplicate HW pid channel returns different SW pid channel */
    NEXUS_PidChannel_Close(pidChannel);
    NEXUS_PidChannel_Close(pidChannel2);

    /* when pidChannelIndex is left as -1, opening the same pid should return the same pidChannel */
    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    BDBG_ASSERT(pidChannelSettings.pidChannelIndex == -1);
    pidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 1, &pidChannelSettings);
    BDBG_ASSERT(pidChannel);
    pidChannel2 = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 1, &pidChannelSettings);
    BDBG_ASSERT(pidChannel2);
    BDBG_ASSERT(pidChannel!=pidChannel2);
    NEXUS_PidChannel_Close(pidChannel);
    NEXUS_PidChannel_Close(pidChannel2);

    /* when specifying the same pidChannelIndex, opening a different pid should fail */
    printf("Expected failure on invalid pid channel & pid number combo...\n");
    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.pidChannelIndex = 0;
    pidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 1, &pidChannelSettings);
    BDBG_ASSERT(pidChannel);
    pidChannelSettings.pidChannelIndex = 0;
    pidChannel2 = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 2, &pidChannelSettings);
    BDBG_ASSERT(!pidChannel2);
    NEXUS_PidChannel_Close(pidChannel);

    printf("Expected failure on invalid pid channel index...\n");
    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.pidChannelIndex = 10;
    pidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 1, &pidChannelSettings);
    BDBG_ASSERT(pidChannel);
    pidChannelSettings.pidChannelIndex = 11; /* mismatch of requested hw index w/ same pb and pid */
    pidChannel2 = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 1, &pidChannelSettings);
    BDBG_ASSERT(!pidChannel2);
    NEXUS_PidChannel_Close(pidChannel);

    /* run a randomization test */
    {
        NEXUS_PidChannelHandle pidCh[NEXUS_NUM_PID_CHANNELS];
        BKNI_Memset(pidCh, 0, sizeof(pidCh));
        for (i=0;i<100000;i++) {
            unsigned index = rand() % NEXUS_NUM_PID_CHANNELS;
            if (i%1000==0) printf("pidchannel loops %d\n", i);
            switch (rand() % 4) {
            case 0:
                if (!pidCh[index]) {
                    /* test limited (but >1) # of pb's and pids, so there's some dups */
                    pidCh[index] = NEXUS_PidChannel_Open(rand()%NEXUS_NUM_PARSER_BANDS, rand()%5, NULL);
                    BDBG_ASSERT(pidCh[index]);

                    /* verify no duplicates */
                    for (j=0;j<NEXUS_NUM_PID_CHANNELS;j++) {
                        if (j != index) BDBG_ASSERT(pidCh[j] != pidCh[index]);
                    }
                }
                break;
            case 1:
                if (pidCh[index]) {
                    NEXUS_PidChannel_Close(pidCh[index]);
                    pidCh[index] = NULL;
                }
                break;
            case 2:
                if (pidCh[index]) {
                    NEXUS_PidChannelStatus status;
                    rc = NEXUS_PidChannel_GetStatus(pidCh[index], &status);
                    BDBG_ASSERT(!rc);
                }
                break;
            case 3:
                if (pidCh[index]) {
                    rc = NEXUS_PidChannel_SetEnabled(pidCh[index], rand()%2);
                    BDBG_ASSERT(!rc);
                }
                break;
            }
        }
        for (i=0;i<NEXUS_NUM_PID_CHANNELS;i++) {
            if (pidCh[i]) NEXUS_PidChannel_Close(pidCh[i]);
        }
    }

    printf("Expected failure on invalid parser band...\n");
    pidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_eMax, i, NULL);
    BDBG_ASSERT(!pidChannel);

    {
    NEXUS_PlaypumpOpenSettings playpumpOpenSettings;
    NEXUS_PlaypumpHandle playpump;

    NEXUS_Playpump_GetDefaultOpenSettings(&playpumpOpenSettings);
    playpumpOpenSettings.fifoSize = 500 * 1024 * 1024;
    playpump = NEXUS_Playpump_Open(0, &playpumpOpenSettings);
    BDBG_ASSERT(!playpump);

    NEXUS_Playpump_GetDefaultOpenSettings(&playpumpOpenSettings);
    playpumpOpenSettings.fifoSize = 1 * 1024 * 1024;
    playpump = NEXUS_Playpump_Open(0, &playpumpOpenSettings);
    BDBG_ASSERT(playpump);

    NEXUS_Playpump_Close(playpump);
    }

    /* NOTE: uninit will print WRN's if pid channel or parser band refcnt's are non-zero. no return code for this. */
    NEXUS_Platform_Uninit();
    printf("Success\n");
    return 0;
}
