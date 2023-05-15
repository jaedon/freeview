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
 * $brcm_Workfile: record_encrypted_timestamps.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 12/8/11 3:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/dvr/record_encrypted_timestamps.c $
 * 
 * 3   12/8/11 3:13p mphillip
 * SW7346-481: 7346/7425 B0 support exposed a lack of
 *  NEXUS_Security_GetDefaultClearKey calls
 * 
 * 2   6/9/09 4:11p jtna
 * PR55767: update NEXUS_ParserBandSettings.timestampType =>
 *  NEXUS_RecpumpSettings.timestampType
 * 
 * 1   3/9/09 3:35p mphillip
 * PR52786: Add encrypted timestamp examples
 * 
 * 2   2/19/09 3:19p erickson
 * PR51841: remove examples_lib
 *
 * 6   2/4/09 1:59p erickson
 * PR51841: simplify
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#if NEXUS_HAS_RECORD
#include "nexus_record.h"
#endif
#include "nexus_security.h"

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"


int main(void) {
#if NEXUS_HAS_RECORD
    NEXUS_InputBand inputBand;
    NEXUS_FileRecordHandle file;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_RecordPidChannelSettings pidSettings;
    NEXUS_RecordSettings recordSettings;
    NEXUS_PidChannelHandle pidChannel[2];
    const char *fname = "videos/stream_3des.mpg";
    const char *index = "videos/stream_3des.nav";
    static const uint8_t keys[16] = { 0x4e, 0x61, 0xbc, 0x00, 0xa8, 0x7f, 0x39, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    NEXUS_KeySlotHandle keyHandle;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;
    NEXUS_DmaHandle dma;
    NEXUS_SecurityKeySlotSettings keySettings;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);

    dma = NEXUS_Dma_Open(0, NULL);

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!keyHandle) {
        printf("\nAllocate enc keyslot failed \n");
        return 1;
    }
    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
    algConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
    algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    algConfig.operation = NEXUS_SecurityOperation_eEncrypt;
    algConfig.enableTimestamps = true; /* enable timestamps */
    NEXUS_Security_ConfigAlgorithm(keyHandle, &algConfig);

    /* Load AV keys */
    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, sizeof(keys));
    if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    recpump = NEXUS_Recpump_Open(0, NULL);

    record = NEXUS_Record_Create();

    NEXUS_Record_GetSettings(record, &recordSettings);
    recordSettings.recpump = recpump;
    recordSettings.recpumpSettings.securityDma = dma;
    recordSettings.recpumpSettings.securityContext = keyHandle;
    recordSettings.recpumpSettings.timestampType = NEXUS_TransportTimestampType_eMod300; /* enable timestamps */
    NEXUS_Record_SetSettings(record, &recordSettings);

    file = NEXUS_FileRecord_OpenPosix(fname,  index);

    /* configure the video pid for indexing */
    NEXUS_Record_GetDefaultPidChannelSettings(&pidSettings);
    pidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    pidSettings.recpumpSettings.pidTypeSettings.video.index = true;
    pidSettings.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    pidChannel[0] = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[0], &pidSettings);

    /* the audio pid requires no special configuration */
    pidChannel[1] = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[1], NULL);

    NEXUS_Record_Start(record, file);

    /* Nexus is now recording to disk */

    printf("press ENTER to stop record\n");
    getchar();

    NEXUS_Record_Stop(record);
    NEXUS_Record_RemoveAllPidChannels(record);
    NEXUS_PidChannel_Close(pidChannel[0]);
    NEXUS_PidChannel_Close(pidChannel[1]);
    NEXUS_FileRecord_Close(file);
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);
    NEXUS_Platform_Uninit();

#endif
    return 0;
}
