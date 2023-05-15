/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: packetsub_callback.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 4/9/12 3:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/transport/packetsub_callback.c $
 * 
 * 8   4/9/12 3:26p jtna
 * SW7425-2555: add comment about using NEXUS_PidChannelSettings.enabled =
 *  false to avoid discarding pids
 * 
 * 7   1/30/12 11:53a erickson
 * SW7405-5325: set multiple packets, add mutex to serialize
 *  GetBuffer/WriteComplete, add boolean to short circuit on exit, add #if
 *  0'd test code for flush
 *
 * 6   1/24/12 4:56p erickson
 * SW7405-5325: update comments
 *
 * 5   1/24/12 3:07p erickson
 * SW7405-5325: fix cont_count, have app do looping, skip opening frontend
 *
 * 4   1/13/12 8:53p erickson
 * SW7405-5325: don't filter on pid 0x0 just because we're inserting pid
 *  0x0.
 *
 * 3   12/16/11 11:42a erickson
 * SW7405-5325: remove stop/start when adding a new descriptor
 *
 * 2   5/13/11 1:41p erickson
 * SW7405-5311: don't stop packetsub in the finished callback. the data
 *  has not been fed into the stream yet.
 *
 * 1   5/13/11 12:18p erickson
 * SW7420-1873: remove shutdown attribute
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_packetsub.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h" /* for verification */

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>

BDBG_MODULE(packetsub);

static void dataready_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
}

unsigned g_counter = 0;
unsigned g_pid = 0;
unsigned g_cont_count = 0;
BKNI_MutexHandle mutex;
bool g_started = false;

static void set_packetsub(NEXUS_PacketSubHandle packetSub, unsigned char data)
{
    int rc;
    uint8_t *pkt;
    unsigned size;

    BDBG_WRN(("setting packetsub with %d", data));

again:
    /* a mutex is required to ensure the GetBuffer/WriteComplete is atomic between the callback
    and the main thread. */
    BKNI_AcquireMutex(mutex);
    rc = NEXUS_PacketSub_GetBuffer(packetSub, (void **)&pkt, &size);
    BDBG_ASSERT(!rc);
    if (size < 188) {
        if (size) {
            rc = NEXUS_PacketSub_WriteCompleteWithSkip(packetSub, size, 0);
            BDBG_ASSERT(!rc);
        }
        BKNI_ReleaseMutex(mutex);
        if (!g_started) return; /* app is exiting */
        BKNI_Sleep(1); /* instead of wait for dataCallback */
        goto again;
    }

    /* for example, insert a new PAT */
    pkt[0] = 0x47;   /* SYNC BYTE */
    pkt[1] = (g_pid >> 8) & 0x1f;
    pkt[2] = g_pid & 0xff;  /* PID */
    pkt[3] = 0x30 | g_cont_count; /* not scrambled, adaptation_field then payload, 0 continuity counter */
    if (++g_cont_count > 0xF) g_cont_count = 0;
    pkt[4] = 188-5-4; /* leave only 4 bytes of payload */
    BKNI_Memset(&pkt[5], 0, 188-5-4); /* zero out adaptation field */
    pkt[184] = data;
    pkt[185] = data;
    pkt[186] = data;
    pkt[187] = data;

    /* NOTE: user can feed more than one packet a single WriteComplete. user can also call GetBuffer/WriteComplete more than once.
    user is responsible for setting the continuity count of all packets. */

    rc = NEXUS_PacketSub_WriteCompleteWithSkip(packetSub, 0, 188);
    BDBG_ASSERT(!rc);

    BKNI_ReleaseMutex(mutex);

#if 0
    if (rand() % 4 == 0) {
        /* random insert a flush and keep the pipeline going */
        BDBG_WRN(("flush"));
        BKNI_AcquireMutex(mutex);
        NEXUS_PacketSub_Flush(packetSub);
        BKNI_ReleaseMutex(mutex);
        set_packetsub(packetSub, ++g_counter); /* keep it going */
    }
#endif
}

static void packetsub_finished(void *context, int param)
{
    NEXUS_PacketSubHandle packetSub = context;
    BSTD_UNUSED(param);
    BDBG_WRN(("packetsub %p finished", context));
    set_packetsub(packetSub, ++g_counter);
    set_packetsub(packetSub, ++g_counter);
    set_packetsub(packetSub, ++g_counter);
    set_packetsub(packetSub, ++g_counter);
    set_packetsub(packetSub, ++g_counter);
}

int main(void) {
    NEXUS_PlatformSettings platformSettings;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelHandle pidChannel;
    NEXUS_PidChannelSettings pidChannelSettings;
    NEXUS_PacketSubHandle packetSub;
    NEXUS_PacketSubOpenSettings packetSubOpenSettings;
    NEXUS_PacketSubSettings packetSubSettings;
    NEXUS_Error rc;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecpumpSettings recpumpSettings;
    unsigned total_data = 0;
    FILE *data_file;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    BKNI_CreateMutex(&mutex);
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    /* this pid channel is the insertion channel, not the substitution channel.
    so, even though we are inserting pid 0x0's, we don't need to filter for the incoming pid 0x0's that will be discarded.
    we can either disable the pid channel, or just pick any unused pid value so we can get a unused pid channel. 
    either way will work. */
    pidChannelSettings.enabled = false;
    pidChannel = NEXUS_PidChannel_Open(parserBand, 0x1ffe, &pidChannelSettings);

    NEXUS_PacketSub_GetDefaultOpenSettings(&packetSubOpenSettings);
    packetSubOpenSettings.fifoSize = 1024;

    packetSub = NEXUS_PacketSub_Open(0, &packetSubOpenSettings);

    NEXUS_PacketSub_GetSettings(packetSub, &packetSubSettings);
    packetSubSettings.pidChannel = pidChannel;
    packetSubSettings.loop = false; /* can only do one Submit if looping. also, for correct TS data
        it must be 16 packets with continuity count of 0x0..0xF. so, this example does not use packetsub
        looping. instead, after a packet is submitted, the finished callback submits the next packet. */
    packetSubSettings.finished.callback = packetsub_finished;
    packetSubSettings.finished.context = packetSub;
    rc = NEXUS_PacketSub_SetSettings(packetSub, &packetSubSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_PacketSub_Start(packetSub);
    BDBG_ASSERT(!rc);
    g_started = true;

    /* app can add more than one at a time */
    set_packetsub(packetSub, ++g_counter);
    set_packetsub(packetSub, ++g_counter);
    set_packetsub(packetSub, ++g_counter);
    set_packetsub(packetSub, ++g_counter);
    set_packetsub(packetSub, ++g_counter);

    /**
    Use a recpump to record the result for verification
    **/
    recpump = NEXUS_Recpump_Open(0, NULL);
    data_file = fopen("packetsub_results.ts", "wb");
    NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = dataready_callback;
    NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);
    rc = NEXUS_Recpump_AddPidChannel(recpump, pidChannel, NULL);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Recpump_Start(recpump);
    BDBG_ASSERT(!rc);
    while (total_data < 32 * 1024) {
        const void *buffer;
        unsigned size;

        rc = NEXUS_Recpump_GetDataBuffer(recpump, &buffer, &size);
        BDBG_ASSERT(!rc);
        if (!size) {
            BKNI_Sleep(1);
            continue;
        }
        fwrite(buffer, size, 1, data_file);
        rc = NEXUS_Recpump_DataReadComplete(recpump, size);
        BDBG_ASSERT(!rc);
        printf("writing %d bytes\n", size);
        total_data += size;
    }
    NEXUS_Recpump_Stop(recpump);
    NEXUS_Recpump_RemoveAllPidChannels(recpump);
    NEXUS_Recpump_Close(recpump);
    fclose(data_file);

    NEXUS_PacketSub_Stop(packetSub);
    g_started = false;
    NEXUS_PacketSub_Close(packetSub);
    NEXUS_PidChannel_Close(pidChannel);
    BKNI_DestroyMutex(mutex);
    NEXUS_Platform_Uninit();
    return 0;
}
