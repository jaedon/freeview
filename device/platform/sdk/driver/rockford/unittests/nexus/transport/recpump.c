/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: recpump.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 7/31/12 8:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/recpump.c $
 * 
 * Hydra_Software_Devel/7   7/31/12 8:50a erickson
 * SWDEPRECATED-3044: randomize close to expose bugs
 * 
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_recpump.h"
#include "nexus_parser_band.h"

/* use smaller #'s so the test cases don't get too sparse */
#define TOTAL_PIDCHANNELS 5
#define TOTAL_RECPUMPS 4

void dataReadyCallback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
}

int main(void)
{
    unsigned i;
    NEXUS_Error rc;
    NEXUS_RecpumpHandle recpump[TOTAL_RECPUMPS];
    NEXUS_PidChannelHandle pidChannel[TOTAL_PIDCHANNELS];
    bool pidChannelAssignment[TOTAL_RECPUMPS][TOTAL_PIDCHANNELS];
    bool started[TOTAL_RECPUMPS];
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_RecpumpSettings settings;
    NEXUS_RecpumpStatus status;
    unsigned loops = 10000;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);

    BKNI_Memset(recpump, 0, sizeof(recpump));
    BKNI_Memset(pidChannel, 0, sizeof(pidChannel));
    BKNI_Memset(pidChannelAssignment, 0, sizeof(pidChannelAssignment));
    BKNI_Memset(started, 0, sizeof(started));

    for (i=0;i<TOTAL_PIDCHANNELS;i++) {
        pidChannel[i] = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, rand() % 0x1ffe, NULL);
        BDBG_ASSERT(pidChannel[i]);
    }

    while (--loops) {
        unsigned recpumpIndex = rand() % TOTAL_RECPUMPS;
        unsigned pidChannelIndex = rand() % TOTAL_PIDCHANNELS;
        unsigned action = rand() % 8;
        unsigned n;

#if 0
        printf("%d: rec=%d, pid=%d, action=%d\n", loops, recpumpIndex, pidChannelIndex, action); /* verbose */
#else
        if (loops % 100 == 0) printf("loops %d\n", loops); /* minimum sign of life */
#endif

        switch (action) {
        case 0:
            if (!recpump[recpumpIndex]) {
                NEXUS_RecpumpOpenSettings openSettings;
                NEXUS_Recpump_GetDefaultOpenSettings(&openSettings);
                if (rand() % 2) {
                    openSettings.index.bufferSize = 0;
                }
                recpump[recpumpIndex] = NEXUS_Recpump_Open(recpumpIndex, &openSettings);
                BDBG_ASSERT(recpump[recpumpIndex]);
            }
            break;
        case 1:
            if (recpump[recpumpIndex]) {
                NEXUS_Recpump_Close(recpump[recpumpIndex]);
                recpump[recpumpIndex] = NULL;
                started[recpumpIndex] = false;
                for (n=0;n<TOTAL_PIDCHANNELS;n++) {
                    pidChannelAssignment[recpumpIndex][n] = false;
                }
            }
            break;
        case 2:
            if (recpump[recpumpIndex] && !pidChannelAssignment[recpumpIndex][pidChannelIndex]) {
                rc = NEXUS_Recpump_AddPidChannel(recpump[recpumpIndex], pidChannel[pidChannelIndex], NULL);
                BDBG_ASSERT(!rc);
                pidChannelAssignment[recpumpIndex][pidChannelIndex] = true;
            }
            break;
        case 3:
            if (recpump[recpumpIndex] && pidChannelAssignment[recpumpIndex][pidChannelIndex]) {
                rc = NEXUS_Recpump_RemovePidChannel(recpump[recpumpIndex], pidChannel[pidChannelIndex]);
                BDBG_ASSERT(!rc);
                pidChannelAssignment[recpumpIndex][pidChannelIndex] = false;
            }
            break;
        case 4:
            if (recpump[recpumpIndex]) {
                NEXUS_Recpump_RemoveAllPidChannels(recpump[recpumpIndex]);
                for (n=0;n<TOTAL_PIDCHANNELS;n++) {
                    pidChannelAssignment[recpumpIndex][n] = false;
                }
            }
            break;
        case 5:
            if (recpump[recpumpIndex]) {
                rc = NEXUS_Recpump_GetStatus(recpump[recpumpIndex], &status);
                BDBG_ASSERT(!rc);
                BDBG_ASSERT(status.data.fifoSize);
            }
            break;
        case 6:
            if (recpump[recpumpIndex] && !started[recpumpIndex]) {
                NEXUS_Recpump_GetSettings(recpump[recpumpIndex], &settings);
                settings.data.dataReady.callback = dataReadyCallback;
                rc = NEXUS_Recpump_SetSettings(recpump[recpumpIndex], &settings);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Recpump_Start(recpump[recpumpIndex]);
                BDBG_ASSERT(!rc);
                started[recpumpIndex] = true;
            }
            break;
        case 7:
            if (recpump[recpumpIndex] && started[recpumpIndex]) {
                NEXUS_Recpump_Stop(recpump[recpumpIndex]);
                started[recpumpIndex] = false;
            }
            break;
        }
    }

    /* free everything up, but in random order */
    {
        bool done;
        do {
            if (rand()%2) {
                i = rand()%TOTAL_PIDCHANNELS;
                if (pidChannel[i]) {
                    NEXUS_PidChannel_Close(pidChannel[i]);
                    pidChannel[i] = NULL;
                }
            }
            else {
                i = rand()%TOTAL_RECPUMPS;
                if (recpump[i]) {
                    NEXUS_Recpump_Close(recpump[i]);
                    recpump[i] = NULL;
                }
            }
            
            /* check if everything closed */
            done = true;
            for (i=0;i<TOTAL_PIDCHANNELS && done;i++) {
                if (pidChannel[i]) done = false;
            }
            for (i=0;i<TOTAL_RECPUMPS && done;i++) {
                if (recpump[i]) done = false;
            }
        } while (!done);
    }

    recpump[0] = NEXUS_Recpump_Open(0, NULL);
    BDBG_ASSERT(recpump[0]);

    /* pid channels from two different transport types is not ok */
#if NEXUS_NUM_PARSER_BANDS > 2
    NEXUS_ParserBand_GetSettings(NEXUS_ParserBand_e2, &parserBandSettings);
    parserBandSettings.transportType = NEXUS_TransportType_eDssEs;
    NEXUS_ParserBand_SetSettings(NEXUS_ParserBand_e2, &parserBandSettings);
    pidChannel[0] = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 0, NULL);
    BDBG_ASSERT(pidChannel[0]);
    rc = NEXUS_Recpump_AddPidChannel(recpump[0], pidChannel[0], NULL);
    BDBG_ASSERT(!rc);
    pidChannel[1] = NEXUS_PidChannel_Open(NEXUS_ParserBand_e2, 1, NULL);
    BDBG_ASSERT(pidChannel[1]);
#else
    NEXUS_ParserBand_GetSettings(NEXUS_ParserBand_e1, &parserBandSettings);
    parserBandSettings.transportType = NEXUS_TransportType_eDssEs;
    NEXUS_ParserBand_SetSettings(NEXUS_ParserBand_e1, &parserBandSettings);
    pidChannel[0] = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, 0, NULL);
    BDBG_ASSERT(pidChannel[0]);
    rc = NEXUS_Recpump_AddPidChannel(recpump[0], pidChannel[0], NULL);
    BDBG_ASSERT(!rc);
    pidChannel[1] = NEXUS_PidChannel_Open(NEXUS_ParserBand_e1, 1, NULL);
    BDBG_ASSERT(pidChannel[1]);
#endif 
    printf("Expected failure...\n");
    rc = NEXUS_Recpump_AddPidChannel(recpump[0], pidChannel[1], NULL);
    BDBG_ASSERT(rc);
    NEXUS_Recpump_RemoveAllPidChannels(recpump[0]);
    NEXUS_PidChannel_Close(pidChannel[0]);
    NEXUS_PidChannel_Close(pidChannel[1]);
    NEXUS_Recpump_Close(recpump[0]);

    /* invalid recpump */
    printf("Expected failure...\n");
    recpump[1] = NEXUS_Recpump_Open(500, NULL);
    BDBG_ASSERT(!recpump[1]);

    NEXUS_Platform_Uninit();
    printf("Success\n");
    return 0;
}
