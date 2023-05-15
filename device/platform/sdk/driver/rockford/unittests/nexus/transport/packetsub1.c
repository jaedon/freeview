/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: packetsub1.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 4/5/12 4:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/packetsub1.c $
 * 
 * Hydra_Software_Devel/3   4/5/12 4:39p erickson
 * SW7425-2147: change to forcedInsertion
 * 
 * Hydra_Software_Devel/2   2/3/12 11:58a erickson
 * SW7405-5598: use transportCapabilities.numPacketSubs
 * 
 * Hydra_Software_Devel/1   12/16/11 11:44a erickson
 * SW7405-5325: add packetsub1 test
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_packetsub.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h" /* for verification */
#include "nexus_transport_capabilities.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(packetsub1);

static void complete(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

#include <sys/time.h>
static unsigned b_get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

int write_packet(NEXUS_PacketSubHandle packetSub, BKNI_EventHandle dataEvent, unsigned pid)
{
    uint8_t *pkt;
    unsigned size;
    int rc;

again:
    rc = NEXUS_PacketSub_GetBuffer(packetSub, (void **)&pkt, &size);
    BDBG_ASSERT(!rc);
    if (size < 188) {
        if (dataEvent) {
            if (size) {
                rc = NEXUS_PacketSub_WriteCompleteWithSkip(packetSub, size, 0);
                BDBG_ASSERT(!rc);
            }
            else {
                rc = BKNI_WaitForEvent(dataEvent, 3000);
                BDBG_ASSERT(!rc);
            }
            goto again;
        }
        return -1;
    }

    /* create new packet to replace the PAT */
    pkt[0] = 0x47;   /* SYNC BYTE */
    pkt[1] = (pid >> 8) & 0x1f;
    pkt[2] = pid & 0xff;  /* PID */
    pkt[3] = 0x30; /* not scrambled, adaptation_field then payload, 0 continuity counter */
    pkt[4] = 188-5-4; /* leave only 4 bytes of payload */
    BKNI_Memset(&pkt[5], 0, 188-5-4); /* zero out adaptation field */
    pkt[184] = 0xde;
    pkt[185] = 0xad;
    pkt[186] = 0xbe;
    pkt[187] = 0xef;

    BDBG_WRN(("size %d", size));
    rc = NEXUS_PacketSub_WriteCompleteWithSkip(packetSub, 0, 188);
    BDBG_ASSERT(!rc);
    return 0;
}

#define NEXUS_MAX_PACKET_SUB 16

int main(void) {
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelHandle pidChannel;
    NEXUS_PidChannelSettings pidChannelSettings;
    struct {
        NEXUS_PacketSubHandle handle;
        bool started;
        BKNI_EventHandle finishedEvent, dataEvent;
        unsigned cnt;
    } packetSub[NEXUS_MAX_PACKET_SUB];
    NEXUS_PacketSubOpenSettings packetSubOpenSettings;
    NEXUS_PacketSubSettings packetSubSettings;
    NEXUS_Error rc;
    unsigned i;
    unsigned starttime;
    unsigned timeout = 120;
    NEXUS_TransportCapabilities transportCapabilities;

    rc = NEXUS_Platform_Init(NULL);
    if (rc) return -1;

    NEXUS_GetTransportCapabilities(&transportCapabilities);
    BDBG_ASSERT(transportCapabilities.numPacketSubs <= NEXUS_MAX_PACKET_SUB);
    
    BKNI_Memset(packetSub, 0, sizeof(packetSub));

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannel = NEXUS_PidChannel_Open(parserBand, 0x0, &pidChannelSettings);

    starttime = b_get_time();
    
    while (timeout && b_get_time()-starttime<timeout*1000) {
        unsigned index = rand()%transportCapabilities.numPacketSubs;

        switch (rand()%5) {
        case 0:
            if (!packetSub[index].handle) {
                NEXUS_PacketSub_GetDefaultOpenSettings(&packetSubOpenSettings);
                packetSubOpenSettings.fifoSize = rand()%packetSubOpenSettings.fifoSize;
                if (packetSubOpenSettings.fifoSize < 4) {
                    packetSubOpenSettings.fifoSize = 188; /* min size */
                }
                packetSubOpenSettings.numDescriptors = 1 + rand()%20;
                /* TODO: XPT PI not preventing duplicate opens */
                packetSub[index].handle = NEXUS_PacketSub_Open(index, rand()%2?NULL:&packetSubOpenSettings);
                BDBG_ASSERT(packetSub[index].handle);

                BKNI_CreateEvent(&packetSub[index].finishedEvent);
                BKNI_CreateEvent(&packetSub[index].dataEvent);

                NEXUS_PacketSub_GetSettings(packetSub[index].handle, &packetSubSettings);
                packetSubSettings.pidChannel = pidChannel;
                packetSubSettings.loop = false;
                packetSubSettings.finished.callback = complete;
                packetSubSettings.finished.context = packetSub[index].finishedEvent;
                packetSubSettings.dataCallback.callback = complete;
                packetSubSettings.dataCallback.context = packetSub[index].dataEvent;
                packetSubSettings.forcedInsertion = rand()%2;
                packetSubSettings.highPriorityDma = rand()%2;
                packetSubSettings.fullRateOutput = rand()%2;
                rc = NEXUS_PacketSub_SetSettings(packetSub[index].handle, &packetSubSettings);
                BDBG_ASSERT(!rc);
            }
            break;
        case 1:
            if (packetSub[index].handle && rand()%50==0) {
                NEXUS_PacketSub_Close(packetSub[index].handle);
                BKNI_DestroyEvent(packetSub[index].finishedEvent);
                BKNI_DestroyEvent(packetSub[index].dataEvent);
                packetSub[index].handle = NULL;
                packetSub[index].started = false;
            }
            break;
        case 2:
            if (packetSub[index].handle && !packetSub[index].started) {
                rc = NEXUS_PacketSub_Start(packetSub[index].handle);
                BDBG_ASSERT(!rc);
                packetSub[index].started = true;
            }
            break;
        case 3:
            if (packetSub[index].started && rand()%30==0) {
                NEXUS_PacketSub_Stop(packetSub[index].handle);
                packetSub[index].started = false;
            }
            break;
        case 4:
            if (packetSub[index].started) {
                unsigned size;
                void *pkt;
                rc = NEXUS_PacketSub_GetBuffer(packetSub[index].handle, &pkt, &size);
                BDBG_ASSERT(!rc);
                
                /* don't allow a big buffer too often. we want more HW/SW transitions */
                if (size > 2000 && rand()%20 != 0) {
                    size = 2000;
                }
                
                if (size) {
                    unsigned skip, amount;
                    /* random skip */
                    skip = rand()%size;
                    skip -= skip%4; /* must be 4 byte aligned */

                    /* complete a random amount of the remainder */
                    amount = size - skip;
                    amount = rand()%amount;
                    amount -= amount%188; /* packet aligned */

                    if (!amount && !skip) {
                        skip = size; /* skip it all so we don't get stuck on a wrap around */
                    }

                    rc = NEXUS_PacketSub_WriteCompleteWithSkip(packetSub[index].handle, skip, amount);
                    BDBG_ASSERT(!rc);
                    if (++packetSub[index].cnt%1000==0) BDBG_WRN(("psub[%d]: %d writes", index, packetSub[index].cnt));
                }
            }
            break;
        }
    }

    for (i=0;i<NEXUS_MAX_PACKET_SUB;i++) {
        if (packetSub[i].handle) {
            NEXUS_PacketSub_Close(packetSub[i].handle);
            BKNI_DestroyEvent(packetSub[i].finishedEvent);
            BKNI_DestroyEvent(packetSub[i].dataEvent);
        }
    }
    NEXUS_PidChannel_Close(pidChannel);
    NEXUS_Platform_Uninit();
    return 0;
}
