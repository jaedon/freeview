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
 * $brcm_Workfile: message_group3.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 5/8/09 2:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/message_group3.c $
 * 
 * Hydra_Software_Devel/3   5/8/09 2:27p erickson
 * PR42679: shorten test
 *
 * Hydra_Software_Devel/2   5/4/09 5:12p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   4/9/09 1:39p erickson
 * PR54038: added test for bad NEXUS_Message_ReadComplete calls. system
 * should survive.
 *
 * Hydra_Software_Devel/1   3/18/09 4:56p erickson
 * PR53390: added filter group stress test
 *
 * Hydra_Software_Devel/1   11/20/08 5:00p erickson
 * PR48848: added message_group test
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_core_utils.h"
#include "nexus_parser_band.h"
#include "nexus_message.h"
#include "nexus_memory.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(message);

/**
This app tests actual parsing of messages from randomized use of filters
in a single group.
But the parsing is intentionally done wrong. The app will get wrong data, but nexus should not crash or hang.
**/

#define MAX_MESSAGES 7
struct {
    NEXUS_MessageHandle msg;
    bool started;
} g_msgs[MAX_MESSAGES];

#define MAX_PIDS 1
struct {
    NEXUS_PidChannelHandle pidch;
    void *buffer;
} g_pidch[MAX_PIDS];

/* These programs are based on PMT info in pid 0x21 of estarFeed_20050804_2.5min.mpg */

unsigned short g_program[MAX_MESSAGES] = {0x26f7, 0x26f8, 0x26f9, 0x26fa, 0x2700, 0x2701, 0x2702};

void message_callback(void *context, int param)
{
    const void *buffer;
    size_t size;
    NEXUS_MessageHandle message = g_msgs[param].msg;
    NEXUS_Error rc;

    BSTD_UNUSED(context);

    if (g_msgs[param].started) {
        unsigned message_length;

        rc = NEXUS_Message_GetBuffer(message, &buffer, &size);
        if (rc || !size) goto done;

#define TS_READ_16( BUF ) ((uint16_t)((BUF)[0]<<8|(BUF)[1]))
#define TS_PSI_GET_SECTION_LENGTH( BUF )    (uint16_t)(TS_READ_16( &(BUF)[1] ) & 0x0FFF)
        message_length = TS_PSI_GET_SECTION_LENGTH((const uint8_t *)buffer) + 3;

        BDBG_MSG(("message_length %d, msg %d, size %d", message_length, param, size));

        /* intentionally mess up the parsing */
{
    static int count = 0;
    if (++count < 100) {
        if (rand() % 10 == 0) {
            message_length += (rand() % 50) - 25;
        }
    }
}

        /* we may fail on the ReadComplete. it should keep going. */
        (void)NEXUS_Message_ReadComplete(message, message_length);
    }

done:
    BKNI_Sleep(rand() % 10);
}

int main(void) {
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidChannelSettings;
    NEXUS_MemoryAllocationSettings allocSettings;
    unsigned loops = 5000;
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
        g_pidch[i].pidch = NEXUS_PidChannel_Open(parserBand, 0x21, &pidChannelSettings);

        NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
        allocSettings.alignment = 1024;
        rc = NEXUS_Memory_Allocate(bufferSize, &allocSettings, &g_pidch[i].buffer);
        BDBG_ASSERT(!rc);
    }

    while (--loops) {
        if (loops % 25 == 0) printf("countdown %d...\n", loops);

        i = rand() % MAX_MESSAGES;
        if (!g_msgs[i].msg) {
            NEXUS_MessageSettings settings;
            NEXUS_Message_GetDefaultSettings(&settings);
            settings.dataReady.callback = message_callback;
            settings.dataReady.param = i;
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

            /* these fields must match */
            startSettings.filter.mask[0] = 0x00;
            startSettings.filter.mask[2] = 0x00;
            startSettings.filter.mask[3] = 0x00;

            /* they must match these values */
            startSettings.filter.coefficient[0] = 0x02;
            startSettings.filter.coefficient[2] = (g_program[i] & 0xFF00) >> 8;
            startSettings.filter.coefficient[3] = g_program[i] & 0xFF;

            rc = NEXUS_Message_Start(g_msgs[i].msg, &startSettings);
            BDBG_ASSERT(!rc);
            g_msgs[i].started = true;
            NEXUS_StartCallbacks(g_msgs[i].msg);

            BKNI_Sleep(100); /* allow some data to flow */
        }
        else if (g_msgs[i].started && (rand()%2)) {
            NEXUS_StopCallbacks(g_msgs[i].msg);
            NEXUS_Message_Stop(g_msgs[i].msg);
            g_msgs[i].started = false;
        }
        else {
            NEXUS_Message_Close(g_msgs[i].msg);
            g_msgs[i].started = false;
            g_msgs[i].msg = NULL;
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
    }
    for (i=0;i<MAX_PIDS;i++) {
        NEXUS_PidChannel_Close(g_pidch[i].pidch);
    }

    NEXUS_Platform_Uninit();
    return 0;
}
