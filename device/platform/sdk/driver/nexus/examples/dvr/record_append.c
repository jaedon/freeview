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
 * $brcm_Workfile: record_append.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 2/25/10 11:38a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/dvr/record_append.c $
 * 
 * 4   2/25/10 11:38a mphillip
 * SW7550-277: Guard record use for platforms lacking record
 * 
 * 3   7/24/09 11:41a jtna
 * PR56980: clarify role of application in record append
 * 
 * 2   1/7/09 11:10p erickson
 * PR49501: added options, print 64 bit offset, use ftello
 *
 * 1   1/7/09 9:50p erickson
 * PR49501: added record_append example
 *
 * Hydra_Software_Devel/1   5/7/08 4:16p erickson
 * PR42329: add record unittests
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#if NEXUS_HAS_RECORD
#include "nexus_recpump.h"
#include "nexus_record.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* for truncate() */
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bcmplayer.h"

BDBG_MODULE(record_append);

#if NEXUS_HAS_RECORD
/**
Reading the existing file size inside NEXUS_Record is easy.
Reading the timestamp is difficult. It would require punching through several layers.
Instead, to support the appended record feature, we will require the app to pass in both the file size and the timestamp.
They are relatively easy to get outside of nexus. */
int get_stats_from_previous_recording(const char *fname, const char *index, uint64_t *offset, unsigned *timestamp)
{
    BNAV_Player_Settings cfg;
    BNAV_Player_Handle bcmplayer;
    BNAV_Player_Position position;
    FILE *file;
    NEXUS_Error rc;
    uint64_t fsize, trunc;

    /* read data file size. this will allow indexing to resume with correct offsets. */
    file = fopen(fname, "r");
    if (!file) return BERR_TRACE(-1);
    fseek(file, 0, SEEK_END);
    fsize = ftello(file);
    fclose(file);

    /* data file must be truncated to a satisfy both packet (188) and direct I/O page (4096) alignment.
       see NEXUS_Record_StartAppend() header for why this is necessary */
    trunc = fsize%(uint64_t)(188/4*4096);
    rc = truncate(fname, fsize-trunc);
    if (rc) return BERR_TRACE(-1);
    BDBG_WRN(("truncate %s %llu -> %llu", fname, fsize, fsize-trunc));
    *offset = fsize-trunc;
    BDBG_ASSERT((*offset)%188==0);
    BDBG_ASSERT((*offset)%4096==0);


    /* read last timestamp from the existing index. this will allow indexing to resume with continuously incrementing timestamps */
    file = fopen(index, "r");
    if (!file) return BERR_TRACE(-1);
    fseek(file, 0, SEEK_END);
    fsize = ftello(file);

    BNAV_Player_GetDefaultSettings(&cfg);
    cfg.videoPid = 1; /* don't care */
    cfg.filePointer = file;
    cfg.readCb = (BP_READ_CB)fread;
    cfg.tellCb = (BP_TELL_CB)ftell;
    cfg.seekCb = (BP_SEEK_CB)fseek;
    rc = BNAV_Player_Open(&bcmplayer, &cfg);
    if (rc) return BERR_TRACE(rc);

    /* index file may contain an entry that points to an offset that we just truncated */
    trunc = 0;
    while (1) {
        BNAV_Player_GetSettings(bcmplayer, &cfg);
        rc = BNAV_Player_GetPositionInformation(bcmplayer, cfg.lastIndex-(long)trunc, &position);
        if (rc) return BERR_TRACE(rc);
        *timestamp = position.timestamp;
        if ((uint64_t)position.offsetLo < *offset) { 
            break; 
        }
        else if (++trunc) {
            BDBG_WRN(("removing nav entry %d with offset %u", position.index, position.offsetLo));
        }
    }

    BNAV_Player_Close(bcmplayer);
    fclose(file);

    rc = truncate(index, fsize-trunc*32);
    if (rc) return BERR_TRACE(-1);

    return 0;
}
#endif

int main(int argc, char **argv) {
#if NEXUS_HAS_RECORD
    NEXUS_FileRecordHandle file;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_RecordPidChannelSettings pidCfg;
    NEXUS_RecordSettings recordCfg;
    const char *fname = "videos/record_append.mpg";
    const char *index = "videos/record_append.nav";
    unsigned i;
    NEXUS_Error rc;
    unsigned loops = 10;
    bool start_with_append = false;

    if (argc > 1) loops = atoi(argv[1]);
    if (argc > 2) start_with_append = !strcasecmp(argv[2], "start_with_append");

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    for (i=0;i<loops;i++) {
        printf("record session %d\n", i);

        recpump = NEXUS_Recpump_Open(0, NULL);
        record = NEXUS_Record_Create();
        NEXUS_Record_GetSettings(record, &recordCfg);
        recordCfg.recpump = recpump;
        /* allow time to finish writing out data. 
           trailing data that is not aligned to recpumpSettings.atomSize will still be dropped. 
           see description in header file for more info. */
        recordCfg.writeAllTimeout = 100;

        NEXUS_Record_SetSettings(record, &recordCfg);

        NEXUS_Record_GetDefaultPidChannelSettings(&pidCfg);
        pidCfg.recpumpSettings.pidType = NEXUS_PidType_eVideo;
        pidCfg.recpumpSettings.pidTypeSettings.video.index = true;
        pidCfg.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
        NEXUS_Record_AddPidChannel(record, NEXUS_PidChannel_Open(parserBand, 0x31, NULL), &pidCfg);
        NEXUS_Record_AddPidChannel(record, NEXUS_PidChannel_Open(parserBand, 0x34, NULL), NULL);

        if (i == 0 && !start_with_append) {
            file = NEXUS_FileRecord_OpenPosix(fname, index);
            rc = NEXUS_Record_Start(record, file);
            BDBG_ASSERT(!rc);
        }
        else {
            NEXUS_RecordAppendSettings appendSettings;

            NEXUS_Record_GetDefaultAppendSettings(&appendSettings);
            get_stats_from_previous_recording(fname, index, &appendSettings.startingOffset, &appendSettings.timestamp);
            BDBG_WRN(("offset %lld, timestamp %d", appendSettings.startingOffset, appendSettings.timestamp));

            file = NEXUS_FileRecord_AppendPosix(fname, index);
            rc = NEXUS_Record_StartAppend(record, file, &appendSettings);
            BDBG_ASSERT(!rc);
        }

        BKNI_Sleep(15*1000);

        NEXUS_Record_Stop(record);
        NEXUS_Record_RemoveAllPidChannels(record);
        NEXUS_FileRecord_Close(file);

        NEXUS_Record_Destroy(record);
        NEXUS_Recpump_Close(recpump);
    }
#else
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);
#endif

    return 0;
}
