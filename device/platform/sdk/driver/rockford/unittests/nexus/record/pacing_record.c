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
 * $brcm_Workfile: pacing_record.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 7/29/10 11:55a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/pacing_record.c $
 * 
 * Hydra_Software_Devel/4   7/29/10 11:55a calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/3   5/10/10 6:58p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 * 
 * Hydra_Software_Devel/2   6/9/09 4:11p jtna
 * PR55767: update NEXUS_ParserBandSettings.timestampType =>
 * NEXUS_RecpumpSettings.timestampType
 * 
 * Hydra_Software_Devel/1   3/10/09 10:54a erickson
 * PR52946: add example
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
    NEXUS_RecordPidChannelSettings pidSettings;
    NEXUS_RecordSettings recordSettings;
    const char *fname = "videos/pacing_stream.mpg";
    const char *index = "videos/pacing_stream.nav";

#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    ExampleDecode_GetDefaultSettings(&decodeSettings);
    decodeSettings.pcrPid = 0x11;
    decodeSettings.videoPid = 0x11;
    decodeSettings.videoCodec = NEXUS_VideoCodec_eMpeg2;
    decodeSettings.audioPid = 0x14;
    decodeSettings.audioCodec = NEXUS_AudioCodec_eAc3;
    decodeSettings.parserBand = parserBand;
    StartExampleDecode(&decodeSettings);

    recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(record, &recordSettings);
    recordSettings.recpump = recpump;
    recordSettings.recpumpSettings.timestampType = NEXUS_TransportTimestampType_eMod300; /* This enables timestamp record */
    NEXUS_Record_SetSettings(record, &recordSettings);

    printf("starting record session\n");
    file = NEXUS_FileRecord_OpenPosix(fname,  index);

    NEXUS_Record_GetDefaultPidChannelSettings(&pidSettings);
    pidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    pidSettings.recpumpSettings.pidTypeSettings.video.index = true;
    pidSettings.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;

    NEXUS_Record_AddPidChannel(record, NEXUS_PidChannel_Open(parserBand, 0x11, NULL), &pidSettings);
    NEXUS_Record_AddPidChannel(record, NEXUS_PidChannel_Open(parserBand, 0x14, NULL), NULL);
    NEXUS_Record_Start(record, file);

    BKNI_Sleep(20 * 1000);

    NEXUS_Record_Stop(record);
    NEXUS_Record_RemoveAllPidChannels(record);
    NEXUS_FileRecord_Close(file);
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif 

#endif
    return 0;
}
