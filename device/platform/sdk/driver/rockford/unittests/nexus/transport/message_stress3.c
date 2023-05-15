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
 * $brcm_Workfile: message_stress3.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 11/14/11 12:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/message_stress3.c $
 * 
 * Hydra_Software_Devel/9   11/14/11 12:10p erickson
 * SW7400-2955: NEXUS_Message_ReadComplete may fail because msg filter is
 * stopped and restarted. don't assert in this case.
 * 
 * Hydra_Software_Devel/8   8/8/11 10:56a erickson
 * SW7400-2955: added stop_all() to prevent livelock on linux 2.6.18
 * 
 * Hydra_Software_Devel/7   6/20/11 5:16p mward
 * SW7400-2955:  Don't sleep in the callback until at least one message
 * has been counted.
 * 
 * Hydra_Software_Devel/6   5/25/11 5:09p jtna
 * SW7405-5336: add simple test to determine unittest pass/fail for
 * automated testing
 * 
 * Hydra_Software_Devel/5   4/8/11 4:22p mward
 * SW7400-2955:  Add a timeout to work around a potential livelock due to
 * the behavior of the O(1) scheduler in the 2.6.18 Linux kernel.
 * 
 * Hydra_Software_Devel/4   7/29/10 12:24p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/3   5/10/10 7:00p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 * 
 * Hydra_Software_Devel/2   3/8/10 2:05p erickson
 * SW7405-4011: add error callbacks
 *
 * Hydra_Software_Devel/1   9/25/09 11:32a erickson
 * SWDEPRECATED-3717: add tests that require XPT MSG PID2BUF
 *
 * Hydra_Software_Devel/3   5/8/09 2:28p erickson
 * PR53390: require that NEXUS_Message_GetBuffer succeed. shorten test for
 * automation.
 *
 * Hydra_Software_Devel/2   5/4/09 5:12p erickson
 * PR42679: update
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

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

BDBG_MODULE(message);

/**
This app tests actual parsing of messages from randomized use of filters
in a single group.
this app assumes you can have multiple filters per pid, one buffer per filter, no buffer sharing (i.e. no required filter groups)
**/

unsigned g_numCallbacks = 0, g_numMsgs = 0;

#define MAX_MESSAGES 7
struct msgs_t {
    NEXUS_MessageHandle msg;
    bool started;
    unsigned count;
} g_msgs[MAX_MESSAGES];

#define MAX_PIDS 1
struct pidch_t {
    NEXUS_PidChannelHandle pidch;
} g_pidch[MAX_PIDS];

/* These programs are based on PMT info in pid 0x21 of estarFeed_20050804_2.5min.mpg */

struct program_t {
    unsigned short mask;
    unsigned short coeff;
} g_program[MAX_MESSAGES] = {
    {0x0000, 0x26f7}, /* 28 byte */
    {0x0000, 0x26f8}, /* 32 byte */
    {0x000F, 0x26f9}, /* 32 byte + 28 byte */
    {0x0000, 0x26fa}, /* 32 byte */
    {0x0000, 0x2700}, /* 32 byte */
    {0x0000, 0x2701}, /* 32 byte */
    {0x0000, 0x2702}
    };

void message_callback(void *context, int param)
{
    const void *buffer;
    size_t size;
    NEXUS_MessageHandle message = g_msgs[param].msg;
    NEXUS_Error rc;
    
    g_numCallbacks++;

    BSTD_UNUSED(context);
    
    if (g_msgs[param].started) {
        rc = NEXUS_Message_GetBuffer(message, &buffer, &size);
        BDBG_ASSERT(!rc || !g_msgs[param].started); /* may have been stopped */
        if (!size) goto done;

        /* complete between 1 and all bytes */
        size -= (rand() % size);

        rc = NEXUS_Message_ReadComplete(message, size);
        /* may have been stopped, or stopped and restarted, so ignore all errors */
        if (!rc) {
            g_msgs[param].count++;
        }
    }

done:
    BKNI_Sleep(rand() % 10);
}

/* on linux 2.6.18, NEXUS_StopCallbacks may be starved if any other message filter
is still running. it is firing NEXUS_IsrCallback_Fire_isr, and that causes the queue entry to be 
armed, and the linux scheduler never switches to the thread that is waiting on Acquire/Release in
StopCallbacks.
with linux 2.6.31, the "completely fair scheduler" prevents this from happening.
so, a simple app fix is to stop all message filters before either calling NEXUS_StopCallbacks
or calling NEXUS_Message_Close on any one message filter.
 */
void stop_all(void)
{
    unsigned i;
    /* linux scheduler may not stop servicing all other callbacks, so we need them to sleep a bit. */
    for (i=0;i<MAX_MESSAGES;i++) {
        if (g_msgs[i].msg) {
            g_msgs[i].started = false;
            NEXUS_Message_Stop(g_msgs[i].msg);
        }
    }
}

void error_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    printf("error callback\n");
}

int main(int argc, char **argv) {
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidChannelSettings;
    unsigned loops = 5000;
    NEXUS_Error rc;
    unsigned i;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    if (argc > 1) loops = atoi(argv[1]);

    NEXUS_Platform_Init(NULL);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    for (i=0;i<MAX_PIDS;i++) {
        NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
        pidChannelSettings.requireMessageBuffer = true;
        g_pidch[i].pidch = NEXUS_PidChannel_Open(parserBand, 0x21, &pidChannelSettings);
    }

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    while (--loops) {
#if 0
/* SW7400-2955 - to test the linux 2.6.18 live lock bug, enable this short circuit then run with: 
    time while [ 1 ]; do nexus message_stress3 || break; done 
*/
        if (g_numCallbacks && g_numMsgs) break; /* this test is going to pass, so short circuit */
#endif
        
        if (loops % 100 == 0) {
            unsigned j;
            printf("Callbacks: ");
            for (j=0;j<MAX_MESSAGES;j++) {
                printf("%3d ", g_msgs[j].count);
                g_numMsgs += g_msgs[j].count;
            }
            printf("\n");
        }

        i = rand() % MAX_MESSAGES;
        if (!g_msgs[i].msg) {
            NEXUS_MessageSettings settings;
            NEXUS_Message_GetDefaultSettings(&settings);
            settings.dataReady.callback = message_callback;
            settings.dataReady.param = i;
            settings.psiLengthError.callback = error_callback;
            settings.crcError.callback = error_callback;
            settings.pesLengthError.callback = error_callback;
            settings.pesStartCodeError.callback = error_callback;
            g_msgs[i].msg = NEXUS_Message_Open(&settings);
            BDBG_ASSERT(g_msgs[i].msg);
        }
        else if (!g_msgs[i].started && (rand()%2)) {
            NEXUS_MessageStartSettings startSettings;
            unsigned pidno = rand() % MAX_PIDS;

            NEXUS_Message_GetDefaultStartSettings(g_msgs[i].msg, &startSettings);
            startSettings.pidChannel = g_pidch[pidno].pidch;

            /* these fields must match */
            startSettings.filter.mask[0] = 0x00;
            startSettings.filter.mask[2] = (g_program[i].mask & 0xFF00) >> 8;
            startSettings.filter.mask[3] = g_program[i].mask & 0xFF;

            /* they must match these values */
            startSettings.filter.coefficient[0] = 0x02;
            startSettings.filter.coefficient[2] = (g_program[i].coeff & 0xFF00) >> 8;
            startSettings.filter.coefficient[3] = g_program[i].coeff & 0xFF;

            rc = NEXUS_Message_Start(g_msgs[i].msg, &startSettings);
            BDBG_ASSERT(!rc);
            g_msgs[i].started = true;
            NEXUS_StartCallbacks(g_msgs[i].msg);

            BKNI_Sleep(100); /* allow some data to flow */
        }
        else if (g_msgs[i].started && (rand()%20) == 0) {
            /* don't call StopCallbacks before Stop. see stop_all(). */    
            g_msgs[i].started = false;
            NEXUS_Message_Stop(g_msgs[i].msg);
        }
        else if (rand()%20 == 0) {
            stop_all();            
            NEXUS_Message_Close(g_msgs[i].msg);
            g_msgs[i].msg = NULL;
        }

        if (rand() % 5 == 0 ) BKNI_Sleep(rand() % 50);
    }

    /* helpful check for automated testing */
    if (!g_numCallbacks || !g_numMsgs) {
        BDBG_ERR(("No callbacks received or no messages parsed during unittest"));
        BDBG_ERR(("callbacks %u, messages %u. Asserting!", g_numCallbacks, g_numMsgs));
        BKNI_Fail();
    }
    BDBG_WRN(("DONE: callbacks %u, messages %u.", g_numCallbacks, g_numMsgs));

    /* must stop all messages first, before closing, to prevent runaway interrupts from starving us */
    stop_all();
    /* now it's quiet, so we can close */
    for (i=0;i<MAX_MESSAGES;i++) {
        if (g_msgs[i].msg) {
            NEXUS_Message_Close(g_msgs[i].msg);
            g_msgs[i].msg = NULL;
        }
    }

    for (i=0;i<MAX_PIDS;i++) {
        NEXUS_PidChannel_Close(g_pidch[i].pidch);
    }

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    NEXUS_Platform_Uninit();
    return 0;
}
