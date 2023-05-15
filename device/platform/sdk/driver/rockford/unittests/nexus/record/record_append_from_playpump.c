/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: record_append_from_playpump.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 4/5/12 3:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/record_append_from_playpump.c $
 * 
 * Hydra_Software_Devel/8   4/5/12 3:30p katrep
 * SW7231-317:test fails when the original and recorded file size is the
 * same
 * 
 * Hydra_Software_Devel/7   2/27/12 6:50p randyjew
 * SW7125-195:  Trying 350 ms. sleep for autotest 97344.
 * 
 * Hydra_Software_Devel/6   2/21/12 12:38p erickson
 * SW7425-2282: add BNAV_Player_Settings.changingBounds
 * 
 * Hydra_Software_Devel/5   10/18/11 2:20p gmullen
 * SW7425-1383: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1383/1   10/14/11 4:13p gmullen
 * SW7425-1383: Updated allPass usages to use new API
 * 
 * Hydra_Software_Devel/4   1/18/11 5:24p vsilyaev
 * SW7420-1113: Improved debug output
 * 
 * Hydra_Software_Devel/3   3/25/10 4:51p mward
 * SW7125-195:  Trying 300 ms. sleep for autotest 97125.
 * 
 * Hydra_Software_Devel/2   3/24/10 3:24p mward
 * SW7400-2726: Empirically chosen BKNI_Sleep(200) for pacing is marginal
 * on some platforms, use BKNI_Sleep(250).
 * 
 * Hydra_Software_Devel/1   7/24/09 11:43a jtna
 * PR56980: added record_append_from_playpump
 *
 *****************************************************************************/
/* unittest to record a stream over multiple segments using record_append */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#include "nexus_record.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* for truncate() */
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bcmplayer.h"
#include <sys/types.h>
#include <sys/stat.h>

BDBG_MODULE(record_append_from_playpump);

int binary_compare(const char* src, const char* dst)
{
    FILE *file1, *file2;
    int rc = 0;
    unsigned total, last_print;
    uint8_t buf1[1024], buf2[1024];
    printf("starting binary comparison\n");

    file1 = fopen(src, "r");
    file2 = fopen(dst, "r");
    BDBG_ASSERT(file1);
    BDBG_ASSERT(file2);

    total = last_print = 0;
    while (fread(buf2, sizeof(buf2), 1, file2)==1 && fread(buf1, sizeof(buf1), 1, file1)==1 ) {
        if (BKNI_Memcmp(buf1, buf2, sizeof(buf1))) {
            BDBG_ERR(("discrepancy at %lld", ftello(file1)));
            rc = -1;
        }
        total += sizeof(buf1);
        if (total/1024/1024 > last_print) {
            last_print = total/1024/1024;
            printf("compared %u MB\n", last_print);
        }
    }

    BDBG_ASSERT(feof(file2)!=0); /* the record output will always be equal to or smaller than 
                                    the source due to dropped data to satisfy alignment */


    /* test that the total amount of data dropped is less than atomSize */
    fseek(file1, 0, SEEK_END);
    fseek(file2, 0, SEEK_END);
    if (ftello(file1)-ftello(file2) > 4096) { /* TODO: this is changeable via recpump settings */
        BDBG_ERR(("file1(%lld) - file2(%lld) = %lld > 4096 ", ftello(file1), ftello(file2), ftello(file1)-ftello(file2)));
        rc = -1;
    }

    return rc;
}

uint64_t get_filesize(const char* fname)
{
    struct stat buf;
    if (stat(fname, &buf)) {
        return BERR_TRACE(-1);
    }
    else {
        return (uint64_t)buf.st_size;
    }
}

void truncate_file(const char* fname, off_t oldsize, off_t newsize)
{
    int rc;
    BDBG_ASSERT(oldsize>=newsize);
    if (oldsize > newsize) {
        BDBG_WRN(("truncate %s %llu -> %llu", fname, oldsize, newsize));
    }
    rc = truncate(fname, newsize);
    BDBG_ASSERT(!rc);
}

/**
Reading the existing file size inside NEXUS_Record is easy.
Reading the timestamp is difficult. It would require punching through several layers.
Instead, to support the appended record feature, we will require the app to pass in both the file size and the timestamp.
They are relatively easy to get outside of nexus. */
int get_stats_from_previous_recording(const char *fname, const char *index, uint64_t *offset, uint64_t *truncated, unsigned *timestamp)
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
    truncate_file(fname, fsize, fsize-trunc);
    *offset = fsize-trunc;
    *truncated = trunc;
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
    cfg.changingBounds = true;
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
            /* this indicates a nav entry with all fields set to 0. this is a symptom of the record source not being 
               "paced" correctly. see comment below the NEXUS_Playpump_ReadComplete() call */
            BDBG_ASSERT(position.timestamp!=0);
            break;
        }
        else if (++trunc) {
            BDBG_WRN(("removing nav entry %d with offset %u", position.index, position.offsetLo));
        }
    }

    BNAV_Player_Close(bcmplayer);
    fclose(file);

    truncate_file(index, fsize, fsize-trunc*32);
    return 0;
}

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

/* TODO: is allpass record with index a valid testcase? */
#define ALLPASS_RECORD 0

int main(int argc, char **argv) {
    NEXUS_FileRecordHandle recfile;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_RecordPidChannelSettings pidCfg;
    NEXUS_RecordSettings recordCfg;
    NEXUS_PlaypumpHandle playpump;
    const char *recfname = "videos/record_append_from_playpump.mpg"; /* record output */
    const char *recindex = "videos/record_append_from_playpump.nav";

    FILE* playfile;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpOpenPidChannelSettings playpumpPidSettings;
    NEXUS_PidChannelHandle playpumpPidChannel0, playpumpPidChannel1;
    BKNI_EventHandle event;
#if 1
    /* playback file used as record source */
    const char *playfname = "videos/discoveryAvcHD.mpg";
    const int VPID = 0x1522;
    const int APID = 0x1523;
    const NEXUS_VideoCodec VCODEC = NEXUS_VideoCodec_eH264;
#else
    /* a smaller file */
    const char *playfname = "videos/batman_avc_480i.mpg";
    const int VPID = 0x1722;
    const int APID = 0x1723;
    const NEXUS_VideoCodec VCODEC = NEXUS_VideoCodec_eH264;
#endif
    const unsigned playfsize = (unsigned)get_filesize(playfname);

    unsigned i;
    NEXUS_Error rc;
    unsigned feed_total = 0;
    unsigned feed_last_print = 0;
    unsigned splits = 10; /* number of segments to record the file over */

    if (argc > 1) splits = atoi(argv[1]);
    
    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);

    /* use stdio for file I/O to keep the example simple */
    playfile = fopen(playfname, "rb");
    if (!playfile) {
        fprintf(stderr, "can't open file:%s\n", playfname);
        goto error;
    }

    /* set up a playpump to be used as record source */
    playpump = NEXUS_Playpump_Open(0, NULL);

    BKNI_CreateEvent(&event);
    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
#if ALLPASS_RECORD
    playpumpSettings.allPass = true;
    playpumpSettings.acceptNullPackets = false;
#endif
    playpumpSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&playpumpPidSettings);
#if ALLPASS_RECORD
    NEXUS_Playpump_GetAllPassPidChannelIndex(playpump, (unsigned *) &playpumpPidSettings.pidSettings.pidChannelIndex );
#endif
    playpumpPidChannel0 = NEXUS_Playpump_OpenPidChannel(playpump, VPID, &playpumpPidSettings);
#if (!ALLPASS_RECORD)
    playpumpPidChannel1 = NEXUS_Playpump_OpenPidChannel(playpump, APID, NULL);
#endif 

    for (i=0;i<splits;i++) {
        printf("record session %d\n", i);

        if (i>0) {
            /* this is a good sanity check. if this assert fails, data is being dropped somewhere.
               you may need to increase writeAllTimeout, or slow down the rate at which the record source is feeding data.
               see the comment below the NEXUS_Playpump_ReadComplete() call */
            if(feed_total != (unsigned)get_filesize(recfname)) {
                printf("feed_total:%u filesize:%u \n", (unsigned)feed_total, (unsigned)get_filesize(recfname));
                BDBG_ASSERT(feed_total == (unsigned)get_filesize(recfname));
            }
        }

        recpump = NEXUS_Recpump_Open(0, NULL);
        record = NEXUS_Record_Create();
        NEXUS_Record_GetSettings(record, &recordCfg);
        recordCfg.recpump = recpump;
        recordCfg.writeAllTimeout = 100; /* allow time to finish writing out data */
        NEXUS_Record_SetSettings(record, &recordCfg);
        
        NEXUS_Record_GetDefaultPidChannelSettings(&pidCfg);
        pidCfg.recpumpSettings.pidType = NEXUS_PidType_eVideo;
        pidCfg.recpumpSettings.pidTypeSettings.video.index = true;
        pidCfg.recpumpSettings.pidTypeSettings.video.codec = VCODEC;
        NEXUS_Record_AddPidChannel(record, playpumpPidChannel0, &pidCfg);
#if (!ALLPASS_RECORD)
        NEXUS_Record_AddPidChannel(record, playpumpPidChannel1, NULL);
#endif

        if (i==0) {
            recfile = NEXUS_FileRecord_OpenPosix(recfname, recindex);
            rc = NEXUS_Record_Start(record, recfile);
            BDBG_ASSERT(!rc);
        }
        else {
            NEXUS_RecordAppendSettings appendSettings;
            uint64_t trunc_amount;

            NEXUS_Record_GetDefaultAppendSettings(&appendSettings);
            rc = get_stats_from_previous_recording(recfname, recindex, &appendSettings.startingOffset, &trunc_amount, &appendSettings.timestamp);
            BDBG_ASSERT(!rc);
            BDBG_WRN(("offset %lld, timestamp %d", appendSettings.startingOffset, appendSettings.timestamp));

            /* feed_total must be updated */
            feed_total -= (unsigned)trunc_amount;
            
            /* seek to location where recording was stopped previously, to resume playpump from there */
            fseek(playfile, appendSettings.startingOffset, SEEK_SET);

            recfile = NEXUS_FileRecord_AppendPosix(recfname, recindex);
            rc = NEXUS_Record_StartAppend(record, recfile, &appendSettings);
            BDBG_ASSERT(!rc);
        }

        NEXUS_Playpump_Start(playpump);
        while (feed_total < (playfsize*(i+1))/splits) {
            void *buffer;
            size_t buffer_size;
            int n;
            NEXUS_Error rc;
    
            if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
                break;
            if (buffer_size == 0) {
                BKNI_WaitForEvent(event, BKNI_INFINITE);
                continue;
            }
    
            /* The first call to get_buffer will return the entire playback buffer.
            If we use it, we're going to have to wait until the descriptor is complete,
            and then we're going to underflow. So use a max size. */
    #define MAX_READ (188*1024)
            if (buffer_size > MAX_READ)
                buffer_size = MAX_READ;

            n = fread(buffer, 1, buffer_size, playfile);
            if (n < 0) goto error;
            if (n == 0) {
                BDBG_ASSERT(0); /* this shouldn't occur for this unittest */
                fseek(playfile, 0, SEEK_SET);
                NEXUS_Playpump_Flush(playpump);
            }
            else {
                rc = NEXUS_Playpump_ReadComplete(playpump, 0, n);
                BDBG_ASSERT(!rc);
                /* you cannot feed playpump as quickly as you can; the record source must be "paced" properly 
                   or index file corruption may occur without warning. BKNI_Sleep is crude way to pace this. 
                   the sleep time was chosen empirically and will differ from system to system. 
                   if recpump is used, this can be controlled better */
                BKNI_Sleep(350); 
            }
            feed_total += n;

            if (feed_total/1024/1024 > feed_last_print) {
                feed_last_print = feed_total/1024/1024;
                printf("%3d/%3d MB completed\n", feed_last_print, playfsize/1024/1024);
            }
        }

        NEXUS_Playpump_Stop(playpump);
        NEXUS_Record_Stop(record);

        NEXUS_Record_RemoveAllPidChannels(record);
        NEXUS_FileRecord_Close(recfile);

        NEXUS_Record_Destroy(record);
        NEXUS_Recpump_Close(recpump);
    }

    rc = binary_compare(playfname, recfname);
    BDBG_ASSERT(!rc);

    BDBG_WRN(("test completed successfully\n"));
    return 0;

error:
    printf("Unexpected error encountered during test\n");
    return -1;
}

/* once record is complete, use nexus/utils/playback to try out playback and trickmodes, eg.
$ nexus playback -probe videos/record_append_from_playpump.mpg videos/record_append_from_playpump.nav 

> rate(4) 
*/
