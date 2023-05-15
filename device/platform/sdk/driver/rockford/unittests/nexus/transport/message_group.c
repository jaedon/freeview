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
 * $brcm_Workfile: message_group.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/24/09 10:00a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/message_group.c $
 * 
 * Hydra_Software_Devel/2   3/24/09 10:00a erickson
 * PR53516: added NEXUS_Message_GetStatus test
 *
 * Hydra_Software_Devel/1   11/20/08 5:00p erickson
 * PR48848: added message_group test
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_message.h"
#include "nexus_memory.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(message);

void message_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BSTD_UNUSED(context);
    /* this will cause Close to block */
    BKNI_Sleep(rand() % 100);
}

#define MAX_MESSAGES 10
struct {
    NEXUS_MessageHandle msg;
    bool started;
} g_msgs[MAX_MESSAGES];

#define MAX_PIDS 5
struct {
    NEXUS_PidChannelHandle pidch;
    void *buffer;
} g_pidch[MAX_PIDS];

int main(void) {
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidChannelSettings;
    NEXUS_MemoryAllocationSettings allocSettings;
    unsigned loops = 10000;
    NEXUS_Error rc;
    unsigned bufferSize = 4096;
    unsigned i;

    NEXUS_Platform_Init(NULL);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);

    for (i=0;i<MAX_PIDS;i++) {
        NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
        pidChannelSettings.requireMessageBuffer = true;
        g_pidch[i].pidch = NEXUS_PidChannel_Open(parserBand, i, &pidChannelSettings);

        NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
        allocSettings.alignment = 1024;
        rc = NEXUS_Memory_Allocate(bufferSize, &allocSettings, &g_pidch[i].buffer);
        BDBG_ASSERT(!rc);
    }


    while (--loops) {
        if (loops % 100 == 0) printf("countdown %d...\n", loops);

        i = rand() % MAX_MESSAGES;
        if (!g_msgs[i].msg) {
            NEXUS_MessageSettings settings;
            NEXUS_Message_GetDefaultSettings(&settings);
            settings.dataReady.callback = message_callback;
            settings.bufferSize = 0;
            g_msgs[i].msg = NEXUS_Message_Open(&settings);
            BDBG_ASSERT(g_msgs[i].msg);
        }
        else if (!g_msgs[i].started && (rand()%2)) {
            NEXUS_MessageStartSettings startSettings;
            unsigned pidno = rand() % MAX_PIDS;

            NEXUS_Message_GetDefaultStartSettings(g_msgs[i].msg, &startSettings);
            startSettings.pidChannel = g_pidch[pidno].pidch;
            startSettings.buffer = g_pidch[pidno].buffer;
            startSettings.bufferSize = bufferSize;
            rc = NEXUS_Message_Start(g_msgs[i].msg, &startSettings);
            BDBG_ASSERT(!rc);
            g_msgs[i].started = true;
        }
        else if (g_msgs[i].started && (rand()%2)) {
            NEXUS_Message_Stop(g_msgs[i].msg);
            g_msgs[i].started = false;
        }
        else {
            NEXUS_Message_Close(g_msgs[i].msg);
            g_msgs[i].started = false;
            g_msgs[i].msg = NULL;
        }

        if (rand() % 50 == 0 && g_msgs[i].msg) {
            NEXUS_MessageStatus status;
            rc = NEXUS_Message_GetStatus(g_msgs[i].msg, &status);
            BDBG_ASSERT(!rc);
            printf("message[%d] status: %d members, master? %c\n",
                i, status.groupMembers, status.isGroupMaster?'y':'n');
        }

        if (rand() % 5 == 0 ) BKNI_Sleep(rand() % 50);
    }

    for (i=0;i<MAX_MESSAGES;i++) {
        if (g_msgs[i].msg) {
            NEXUS_Message_Close(g_msgs[i].msg);
            g_msgs[i].msg = NULL;
        }
    }

    for (i=0;i<MAX_PIDS;i++) {
        NEXUS_Memory_Free(g_pidch[i].buffer);
        NEXUS_PidChannel_Close(g_pidch[i].pidch);
    }
    NEXUS_Platform_Uninit();
    return 0;
}
