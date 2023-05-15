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
 * $brcm_Workfile: message_group2.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 7/29/10 12:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/message_group2.c $
 * 
 * Hydra_Software_Devel/6   7/29/10 12:19p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/5   4/19/10 2:51p erickson
 * SW7335-703: add NEXUS_Message_UpdateFilter test
 *
 * Hydra_Software_Devel/4   4/19/10 2:03p erickson
 * SW7420-624: revise stress test
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
**/

#define MAX_MESSAGES 7
struct {
    bool added;
    unsigned filterNum;
} g_msgs[MAX_MESSAGES];

/* These programs are based on PMT info in pid 0x21 of estarFeed_20050804_2.5min.mpg */

unsigned short g_program[MAX_MESSAGES] = {0x26f7, 0x26f8, 0x26f9, 0x26fa, 0x2700, 0x2701, 0x2702};

NEXUS_MessageHandle msg = NULL;

#define ADD_XPT_MESSAGE_PAD(message_length) \
    (((message_length) % 4) ? ((message_length) + 4 - ((message_length) % 4)) : (message_length))

void message_callback(void *context, int param)
{
    const void *buffer;
    size_t size;
    NEXUS_Error rc;
    unsigned message_length;

    BSTD_UNUSED(param);

    rc = NEXUS_Message_GetBuffer(msg, &buffer, &size);
    BDBG_ASSERT(!rc);
    if (!size) goto done;

#define TS_READ_16( BUF ) ((uint16_t)((BUF)[0]<<8|(BUF)[1]))
#define TS_PSI_GET_SECTION_LENGTH( BUF )    (uint16_t)(TS_READ_16( &(BUF)[1] ) & 0x0FFF)

    message_length = TS_PSI_GET_SECTION_LENGTH((const uint8_t *)buffer) + 3;
    message_length = ADD_XPT_MESSAGE_PAD(message_length);
    BDBG_MSG(("message_length %d, msg %p, buffer %p, size %d", message_length, context, buffer, size));

#if 0
    {
        unsigned i;
        for (i=0;i<message_length;i++) BKNI_Printf("%02x", ((const uint8_t *)buffer)[i]);
        BKNI_Printf("\n");
    }
#endif

    rc = NEXUS_Message_ReadComplete(msg, message_length);
    BDBG_ASSERT(!rc);

done:
    BKNI_Sleep(rand() % 10);
}

void setfilter(NEXUS_MessageFilter *pFilter, unsigned i)
{
    NEXUS_Message_GetDefaultFilter(pFilter);

    /* these fields must match */
    pFilter->mask[0] = 0x00;
    pFilter->mask[2] = 0x00;
    pFilter->mask[3] = 0x00;

    /* they must match these values */
    pFilter->coefficient[0] = 0x02;
    pFilter->coefficient[2] = (g_program[i] & 0xFF00) >> 8;
    pFilter->coefficient[3] = g_program[i] & 0xFF;
}

int main(void) {
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelHandle pidChannel;
    unsigned loops = 5000;
    NEXUS_Error rc;
    unsigned i;
    bool started = false;
    NEXUS_PidChannelSettings pidChannelSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    srand(time(NULL));

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

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.requireMessageBuffer = true;
    pidChannel = NEXUS_PidChannel_Open(parserBand, 0x21, &pidChannelSettings);

#ifdef IP_STREAMER_SUBST
        ipsStartSettings.vDecoder = NULL;
        ipsStartSettings.aDecoder = NULL;
        ipsStartSettings.stcChannel = NULL;
        IpsStart(ips, &ipsStartSettings);
#endif

    while (--loops) {
        if (loops % 25 == 0) printf("countdown %d...\n", loops);

        i = rand() % MAX_MESSAGES;

        switch (rand() % 7) {
        case 0:
            if (!msg) {
                NEXUS_MessageSettings settings;
                NEXUS_Message_GetDefaultSettings(&settings);
                settings.dataReady.callback = message_callback;
                msg = NEXUS_Message_Open(&settings);
                BDBG_ASSERT(msg);
            }
            break;
        case 1:
            if (msg && !started) {
                NEXUS_MessageStartSettings startSettings;

                NEXUS_Message_GetDefaultStartSettings(msg, &startSettings);
                startSettings.pidChannel = pidChannel;
                startSettings.filterGroup = true;
                setfilter(&startSettings.filter, i);

                rc = NEXUS_Message_Start(msg, &startSettings);
                BDBG_ASSERT(!rc);
                started = true;
                NEXUS_StartCallbacks(msg);

                BKNI_Sleep(100); /* allow some data to flow */
            }
            break;
        case 2:
            if (started && (rand()%40==0)) {
                NEXUS_StopCallbacks(msg);
                NEXUS_Message_Stop(msg);
                started = false;
                BKNI_Memset(g_msgs, 0, sizeof(g_msgs));
            }
            break;
        case 3:
            if (msg && (rand()%40==0)) {
                NEXUS_Message_Close(msg);
                started = false;
                BKNI_Memset(g_msgs, 0, sizeof(g_msgs));
                msg = NULL;
            }
            break;
        case 4:
            if (started && !g_msgs[i].added) {
                NEXUS_MessageFilter filter;
                setfilter(&filter, i);
                rc = NEXUS_Message_AddFilter(msg, &filter, &g_msgs[i].filterNum);
                BDBG_ASSERT(!rc);
                g_msgs[i].added = true;
            }
            break;
        case 5:
            if (started && g_msgs[i].added) {
                rc = NEXUS_Message_RemoveFilter(msg, g_msgs[i].filterNum);
                BDBG_ASSERT(!rc);
                g_msgs[i].added = false;
            }
            break;
        case 6:
            if (started) {
                unsigned filterNum;
                unsigned temp_i = rand() % MAX_MESSAGES; /* get a random filter again */
                NEXUS_MessageFilter filter;
                setfilter(&filter, temp_i);

                if (g_msgs[i].added) {
                    filterNum = g_msgs[i].filterNum;
                }
                else {
                    filterNum = NEXUS_MESSAGE_MAIN_FILTER_NUM;
                }

                /* printf("switch %d from filter %d to %d\n", filterNum, i, temp_i); */
                rc = NEXUS_Message_UpdateFilter(msg, filterNum, &filter);
                BDBG_ASSERT(!rc);
            }
            break;
        }

        if (rand() % 5 == 0 ) BKNI_Sleep(rand() % 50);
    }

    NEXUS_Message_Close(msg);
    NEXUS_PidChannel_Close(pidChannel);
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    NEXUS_Platform_Uninit();
    return 0;
}
