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
 * $brcm_Workfile: record1.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 5/27/11 11:56a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/record1.c $
 * 
 * Hydra_Software_Devel/3   5/27/11 11:56a erickson
 * SW7405-691: update tests
 * 
 * Hydra_Software_Devel/2   7/29/10 12:08p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/1   5/7/08 4:16p erickson
 * PR42329: add record unittests
 *
 *****************************************************************************/
/* Nexus example app: recback and decode */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#if NEXUS_HAS_RECORD
#include "nexus_record.h"
#endif
#include "examples_lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

int main(void) {
#if NEXUS_HAS_RECORD
    NEXUS_FileRecordHandle file;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    ExampleDecodeSettings decodeSettings;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_RecordPidChannelSettings pidCfg;
    NEXUS_RecordSettings recordCfg;
    const char *fname = "videos/stream.mpg";
    const char *index = "videos/stream.nav";
    unsigned i;
    NEXUS_Error rc;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    srand(time(NULL));
    
    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    rc = IpsStart(ips, &ipsStartSettings);
    BDBG_ASSERT(!rc);
#endif

    ExampleDecode_GetDefaultSettings(&decodeSettings);
    decodeSettings.pcrPid = 0x11;
    decodeSettings.videoPid = 0x11;
    decodeSettings.videoCodec = NEXUS_VideoCodec_eMpeg2;
    decodeSettings.audioPid = 0x14;
    decodeSettings.audioCodec = NEXUS_AudioCodec_eAc3;
    decodeSettings.parserBand = parserBand;
    rc = StartExampleDecode(&decodeSettings);
    BDBG_ASSERT(!rc);

    recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(record, &recordCfg);
    recordCfg.recpump = recpump;
    NEXUS_Record_SetSettings(record, &recordCfg);

    for (i=0;i<200;i++) {
        NEXUS_PidChannelHandle pidChannel[2];
        
        if (i%10==0) printf("record session %d\n", i);
        
        file = NEXUS_FileRecord_OpenPosix(fname,  rand()%2?index:NULL);

        NEXUS_Record_GetDefaultPidChannelSettings(&pidCfg);
        switch (rand()%3) {
        case 0:    
            pidCfg.recpumpSettings.pidType = NEXUS_PidType_eVideo;
            pidCfg.recpumpSettings.pidTypeSettings.video.index = rand()%2;
            pidCfg.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
            break;
        case 1:    
            pidCfg.recpumpSettings.pidType = NEXUS_PidType_eOther;
            pidCfg.recpumpSettings.pidTypeSettings.other.index = rand()%2;
            break;
        case 2:
            break;    
        }

        pidChannel[0] = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
        pidChannel[1] = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
        rc = NEXUS_Record_AddPidChannel(record, pidChannel[0], &pidCfg);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Record_AddPidChannel(record, pidChannel[1], NULL);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Record_Start(record, file);
        BDBG_ASSERT(!rc);

        BKNI_Sleep(rand()%200);

        NEXUS_Record_Stop(record);
        NEXUS_Record_RemoveAllPidChannels(record);
        NEXUS_PidChannel_Close(pidChannel[0]);
        NEXUS_PidChannel_Close(pidChannel[1]);
        NEXUS_FileRecord_Close(file);
    }
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif 
    StopExampleDecode();
    UninitExampleDecode();
    NEXUS_Platform_Uninit();
#endif
    return 0;
}
