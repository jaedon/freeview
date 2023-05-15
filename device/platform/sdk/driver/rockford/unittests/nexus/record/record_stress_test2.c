/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: record_stress_test2.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/26/12 3:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/record_stress_test2.c $
 * 
 * Hydra_Software_Devel/2   7/26/12 3:55p randyjew
 * SW7468-143: Modify NUM_RECORDS for 7468
 * 
 * Hydra_Software_Devel/1   9/15/11 10:32a erickson
 * SW7405-5489: add record_stress_test2
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#include "nexus_record.h"
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

BDBG_MODULE(record_stress_test2);
#if BCHP_CHIP == 7468
#define NUM_RECORDS 1
#else
#define NUM_RECORDS 5
#endif

int main(void) 
{
    NEXUS_InputBand inputBand;
    struct {
        NEXUS_FileRecordHandle file;
        NEXUS_RecpumpHandle recpump;
        NEXUS_RecordHandle record;
        NEXUS_ParserBand parserBand;
        NEXUS_PidChannelHandle pidChannel[2];
    } record[NUM_RECORDS];
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    unsigned long i;
    int rc;
    unsigned loops = 10000;
    
    srand(time(NULL));
    BDBG_CASSERT(NUM_RECORDS < NEXUS_NUM_RECPUMPS);
    BDBG_CASSERT(NUM_RECORDS < NEXUS_NUM_PARSER_BANDS);

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    BKNI_Memset(record, 0, sizeof(record));

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    for (i=0;i<NUM_RECORDS;i++) {
        NEXUS_ParserBandSettings parserBandSettings;
        record[i].parserBand = NEXUS_ParserBand_e0+i;
        NEXUS_ParserBand_GetSettings(record[i].parserBand, &parserBandSettings);
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        parserBandSettings.sourceTypeSettings.inputBand = inputBand;
        parserBandSettings.transportType = NEXUS_TransportType_eTs;
        rc = NEXUS_ParserBand_SetSettings(record[i].parserBand, &parserBandSettings);
        BDBG_ASSERT(!rc);
        
        record[i].pidChannel[0] = NEXUS_PidChannel_Open(record[i].parserBand, 0x11, NULL);
        BDBG_ASSERT(record[i].pidChannel[0]);
        record[i].pidChannel[1] = NEXUS_PidChannel_Open(record[i].parserBand, 0x14, NULL);
        BDBG_ASSERT(record[i].pidChannel[1]);
    }
    
    while (loops--) {
        unsigned index = rand() % NUM_RECORDS;
        
        if (loops % 100 == 0) printf("loops %d\n", loops);
        switch (rand() % 6) {
        case 0:
            if (!record[index].recpump) {
                record[index].recpump = NEXUS_Recpump_Open(index, NULL);
                BDBG_ASSERT(record[index].recpump);
            }
            break;
        case 1:    
            if (record[index].recpump && !record[index].record) {
                NEXUS_Recpump_Close(record[index].recpump);
                record[index].recpump = NULL;
            }
            break;
        case 2:    
            if (record[index].recpump && !record[index].record) {
                NEXUS_RecordSettings recordSettings;
                record[index].record = NEXUS_Record_Create();
                NEXUS_Record_GetSettings(record[index].record, &recordSettings);
                recordSettings.recpump = record[index].recpump;
                rc = NEXUS_Record_SetSettings(record[index].record, &recordSettings);
                BDBG_ASSERT(!rc);
            }
            break;  
        case 3:    
            if (record[index].record && !record[index].file) {
                NEXUS_Record_Destroy(record[index].record);
                record[index].record = NULL;
            }
            break;      
        case 4:
            if (record[index].record && !record[index].file) {
                NEXUS_RecordPidChannelSettings pidSettings;
                char datafile[256], indexfile[256];
                
                NEXUS_Record_GetDefaultPidChannelSettings(&pidSettings);
                if (rand() % 2) {
                    pidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
                    pidSettings.recpumpSettings.pidTypeSettings.video.index = rand()%2;
                    pidSettings.recpumpSettings.pidTypeSettings.video.codec = rand()%2?NEXUS_VideoCodec_eMpeg2:NEXUS_VideoCodec_eH264;
                }
                
                snprintf(datafile, 256, "videos/stream%d.mpg", index);
                snprintf(indexfile, 256, "videos/stream%d.nav", index);
                record[index].file = NEXUS_FileRecord_OpenPosix(datafile, indexfile);
                BDBG_ASSERT(record[index].file);
                
                rc = NEXUS_Record_AddPidChannel(record[index].record, record[index].pidChannel[0], &pidSettings);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Record_AddPidChannel(record[index].record, record[index].pidChannel[1], NULL);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Record_Start(record[index].record, record[index].file);
                BDBG_ASSERT(!rc);
            }
            break;
        case 5:    
            if (record[index].file) {
                NEXUS_Record_RemoveAllPidChannels(record[index].record);
                NEXUS_Record_Stop(record[index].record);
                NEXUS_FileRecord_Close(record[index].file);
                record[index].file = NULL;
            }
            break;
        }
        if (rand() % 10 == 0) {
            BKNI_Sleep(rand() % 200);
        }
    }             
    
    for (i=0;i<NUM_RECORDS;i++){
        if (record[i].file) {
            NEXUS_Record_Stop(record[i].record);
            NEXUS_Record_RemoveAllPidChannels(record[i].record);
            NEXUS_FileRecord_Close(record[i].file);
        }
        if (record[i].record) {
            NEXUS_Record_Destroy(record[i].record);
        }
        if (record[i].recpump) {
            NEXUS_Recpump_Close(record[i].recpump);
        }
        NEXUS_PidChannel_Close(record[i].pidChannel[0]);
        NEXUS_PidChannel_Close(record[i].pidChannel[1]);
    }

    NEXUS_Platform_Uninit();
    
    return 0;
}
