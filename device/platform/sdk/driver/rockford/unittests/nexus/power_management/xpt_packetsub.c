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
 * $brcm_Workfile: xpt_packetsub.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/15/11 6:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/power_management/xpt_packetsub.c $
 * 
 * Hydra_Software_Devel/2   2/15/11 6:00p jtna
 * SW7420-1320: update
 * 
 * Hydra_Software_Devel/1   1/18/11 4:17p jtna
 * SW7420-1320: added xpt_remux.c and xpt_packetsub.c
 * 
 *****************************************************************************/
 
#include "nexus_platform.h"
#include "nexus_packetsub.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(packetsub);

#define AUTOMATED_TEST 1
#define MAX_SLEEP_TIME 4000
#define LOOPS 5000
#define EXTRA_LOGS 1 /* extra debug logs */

void dataready_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
}

int main(void) {
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelHandle pidChannel;
    NEXUS_PidChannelSettings pidChannelSettings;
    NEXUS_PacketSubHandle packetSub;
    NEXUS_PacketSubOpenSettings packetSubOpenSettings;
    NEXUS_PacketSubSettings packetSubSettings;
    unsigned pid = 0x0;
    uint8_t *pkt;
    unsigned size;
    NEXUS_Error rc;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecpumpSettings recpumpSettings;
    unsigned total_data = 0;
    FILE *data_file;
    unsigned sleep, loops;

    NEXUS_Platform_Init(NULL);

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    NEXUS_PacketSub_GetDefaultOpenSettings(&packetSubOpenSettings);
    packetSubOpenSettings.fifoSize = 1024;
    packetSub = NEXUS_PacketSub_Open(0, &packetSubOpenSettings);

    data_file = fopen("packetsub.ts", "wb");
    recpump = NEXUS_Recpump_Open(0, NULL);

    NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = dataready_callback; /* dataready callback is required */
    NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);

    loops = LOOPS;
    while (loops--) {
        NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
        pidChannel = NEXUS_PidChannel_Open(parserBand, pid, &pidChannelSettings);

        NEXUS_PacketSub_GetSettings(packetSub, &packetSubSettings);
        packetSubSettings.pidChannel = pidChannel;
        packetSubSettings.loop = true;
        packetSubSettings.outputRate = 1000000;
        rc = NEXUS_PacketSub_SetSettings(packetSub, &packetSubSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_PacketSub_GetBuffer(packetSub, (void **)&pkt, &size);
        BDBG_ASSERT(!rc);
        BDBG_ASSERT(size >= 188);

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

        rc = NEXUS_PacketSub_WriteComplete(packetSub, 188);
        BDBG_ASSERT(!rc);

        rc = NEXUS_PacketSub_Start(packetSub);
        BDBG_ASSERT(!rc);

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
        total_data = 0;

#if AUTOMATED_TEST
        sleep = rand()%MAX_SLEEP_TIME;
        BKNI_Sleep(sleep);
#else
        printf("Press ENTER to stop\n");
        getchar();
#endif

        NEXUS_PacketSub_Stop(packetSub); /* this calls BXPT_PacketSub_StopChannel */
        NEXUS_PacketSub_Flush(packetSub);
#if EXTRA_LOGS
        printf("Packetsub stop\n");
#endif

        NEXUS_Recpump_Stop(recpump); /* this calls BXPT_Rave_DisableContext */
        NEXUS_Recpump_RemoveAllPidChannels(recpump);
#if EXTRA_LOGS
        printf("Recpump stop\n");
#endif

        NEXUS_PidChannel_Close(pidChannel); /* this calls BXPT_SetParserEnable */
#if EXTRA_LOGS
        printf("Pidchannel close\n");
#endif

        /* XMEMIF should be off here */
        printf("Test: power down (loops %4d)\n", loops);

#if AUTOMATED_TEST
        sleep = rand()%MAX_SLEEP_TIME;
        BKNI_Sleep(sleep);
#else
        printf("Press ENTER to restart\n");
        getchar();
#endif       
    }
    
    NEXUS_Recpump_Close(recpump);
    fclose(data_file);

    NEXUS_PacketSub_Stop(packetSub);
    NEXUS_PacketSub_Close(packetSub);
    NEXUS_Platform_Uninit();
    return 0;
}
