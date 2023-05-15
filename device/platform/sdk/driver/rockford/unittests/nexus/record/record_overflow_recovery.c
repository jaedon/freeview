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
 * $brcm_Workfile: record_overflow_recovery.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 2/1/12 4:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/record_overflow_recovery.c $
 * 
 * Hydra_Software_Devel/6   2/1/12 4:03p erickson
 * SW7405-5599: increase timeout for more test results
 * 
 * Hydra_Software_Devel/5   1/18/11 5:18p vsilyaev
 * SW7420-1113: Disabled HW flow control for 'overflow' unit-tests
 * 
 * Hydra_Software_Devel/4   12/15/10 11:18a jtna
 * SW7125-716: make test more overflow-prone
 * 
 * Hydra_Software_Devel/3   12/2/10 12:39p jtna
 * SW7125-716: rework overflow recovery unittests
 * 
 * Hydra_Software_Devel/2   11/10/09 12:40p jtna
 * SWDEPRECATED-3451: update
 * 
 * Hydra_Software_Devel/1   12/11/08 6:20p jtna
 * PR48779: initial version
 *
 *****************************************************************************/

/* This is an automated unit test for record overflow recovery.

a) Induce a record overflow and continue recording for a specified amount of time.
b) Stop the record, then restart it without overflow. Continue recording for a specified amount of time.
c) Stop the record and check the indices of the NAV file against the data file for correctness.

*/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_recpump.h"
#if NEXUS_HAS_RECORD
#include "nexus_record.h"
#endif
#include "nexus_memory.h"
#include "examples_lib.h"
#include "check_index_lib.h"

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

BDBG_MODULE(record_overflow_recovery);

bool has_overflowed = false;
bool is_normal_record = false;
unsigned now, then;

#define OVERFLOW_RECORD_TIME (5*1000)  /* time to continue recording after first overflow detected */
#define NORMAL_RECORD_TIME   (10*1000) /* time to record for after overflow recovery */

static int time_diff(unsigned past, unsigned future)
{
    unsigned result;
    if (future >= past) {
        result = future - past;
    }
    else { /* wraparound */
        result = ((unsigned)(-1)) - past + future;
    }
    return result;
}

/* callback when space becomes available */
static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BDBG_WRN(("overflow"));

    if (is_normal_record) {
        BDBG_ERR(("Overflow detected during normal record"));
        BDBG_ASSERT(0);
    }
    
    if (has_overflowed == false) {
        then = b_get_time(); /* time of first overflow */
        has_overflowed = true;
    }
    else {
        now = b_get_time();
        if (time_diff(then, now) > OVERFLOW_RECORD_TIME) {
            BKNI_SetEvent((BKNI_EventHandle)context);
        }
    }
}

#define NUM_RECORDS 16
#define MAX_PLAYPUMP_READ (188*1024)

int main(void) {
#if NEXUS_HAS_RECORD
    NEXUS_PidChannelHandle pidChannel[NUM_RECORDS];
    NEXUS_FileRecordHandle fileRecord[NUM_RECORDS];
    NEXUS_RecpumpHandle recpump[NUM_RECORDS];
    NEXUS_RecordHandle record[NUM_RECORDS];   
    NEXUS_RecordPidChannelSettings pidChannelSettings;
    NEXUS_RecordSettings recordSettings;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;

    BKNI_EventHandle play_event, overflow_event;
    unsigned i;
    int result = 0;
    unsigned char *pMem = NULL;
    unsigned sleeptime = 0;

    char data[NUM_RECORDS][64];
    char indx[NUM_RECORDS][64];

    /* create the file names */
    for (i=0; i<NUM_RECORDS; i++) {
        BKNI_Snprintf(data[i], 64, "videos/record_overflow%d.mpg", i);
        BKNI_Snprintf(indx[i], 64, "videos/record_overflow%d.nav", i);
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    #define NUMPACKETS_TO_ALLOC 1024
    #define NUMPACKETS_PER_RAP 128
    BDBG_CASSERT(NUMPACKETS_TO_ALLOC%NUMPACKETS_PER_RAP==0);
    #define TEST_PID 0x11

    /* create some data consisting of TS packets with random payload. 
    this has to be sufficiently large to induce a NEXUS_Record overflow */
    pMem = BKNI_Malloc(188*NUMPACKETS_TO_ALLOC);
    generate_payload(pMem, TEST_PID, NUMPACKETS_TO_ALLOC, NUMPACKETS_PER_RAP);

    BKNI_CreateEvent(&play_event);
    BKNI_CreateEvent(&overflow_event);

    playpump = NEXUS_Playpump_Open(0, NULL);
    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = play_event;
    rc = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Playpump_Start(playpump);
    BDBG_ASSERT(!rc);

    for (i=0; i<NUM_RECORDS; i++) {
        recpump[i] = NEXUS_Recpump_Open(i, NULL);
        BDBG_ASSERT(recpump[i]);
        record[i] = NEXUS_Record_Create();
        BDBG_ASSERT(record[i]);

        NEXUS_Record_GetSettings(record[i], &recordSettings);
        recordSettings.recpump = recpump[i];
        recordSettings.overflowCallback.callback = overflow_callback; /* one callback function for all N records */
        recordSettings.overflowCallback.context = overflow_event;
        recordSettings.overflowCallback.param = 0;
        recordSettings.stopOnOverflow = false;
        recordSettings.recpumpSettings.bandHold = NEXUS_RecpumpFlowControl_eDisable;
        rc = NEXUS_Record_SetSettings(record[i], &recordSettings);
        BDBG_ASSERT(!rc);

        fileRecord[i] = NEXUS_FileRecord_OpenPosix(data[i], indx[i]);
        BDBG_ASSERT(fileRecord[i]);

        NEXUS_Record_GetDefaultPidChannelSettings(&pidChannelSettings);
        pidChannelSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
        pidChannelSettings.recpumpSettings.pidTypeSettings.video.index = true;
        pidChannelSettings.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
        pidChannel[i] = NEXUS_Playpump_OpenPidChannel(playpump, TEST_PID, NULL);

        rc = NEXUS_Record_AddPidChannel(record[i], pidChannel[i], &pidChannelSettings);
        BDBG_ASSERT(!rc);
    }

    for (i=0; i<NUM_RECORDS; i++) {
        rc = NEXUS_Record_Start(record[i], fileRecord[i]); 
        BDBG_ASSERT(!rc);
    }

    /* record data until overflow occurs. after initial overflow, keep recording for a specified amount of time */
    then = b_get_time();
    
    BDBG_WRN(("Record started. Overflow should occur"));
    while (BKNI_WaitForEvent(overflow_event, 0)) {
        void *buffer;
        size_t buffer_size;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
            break;
        if (buffer_size == 0) {
            BKNI_WaitForEvent(play_event, BKNI_INFINITE);
            continue;
        }

        /* The first call to get_buffer will return the entire playback buffer.
        If we use it, we're going to have to wait until the descriptor is complete,
        and then we're going to underflow. So use a max size. */
        BDBG_CASSERT(MAX_PLAYPUMP_READ <= 188*NUMPACKETS_TO_ALLOC); /* we can't read more than we allocated */
        if (buffer_size > MAX_PLAYPUMP_READ)
            buffer_size = MAX_PLAYPUMP_READ;

        buffer_size -= buffer_size%188;
        BKNI_Memcpy(buffer, pMem, buffer_size);
        rc = NEXUS_Playpump_WriteComplete(playpump, 0, buffer_size);
        BDBG_ASSERT(!rc);

        now = b_get_time();

        if (has_overflowed == false) {
            if (time_diff(then, now) > 180*1000) { /* assert if we haven't overflown within the timeout */
                BDBG_ERR(("Record should have overflown, but did not"));
                BDBG_ASSERT(0);
            }
        }
    }

    /* stop all records */
    for (i=0; i<NUM_RECORDS; i++) {
        NEXUS_Record_Stop(record[i]);
        NEXUS_FileRecord_Close(fileRecord[i]);
    }
    NEXUS_Playpump_Flush(playpump);
    BKNI_Sleep(1000);

    sleeptime = MAX_PLAYPUMP_READ * 8 * NUM_RECORDS / (5*1000); /* throughput of ~5Mbps */
    BDBG_WRN(("Completed record with overflow. Now starting record without overflow"));

    /* start new records */
    for (i=0; i<NUM_RECORDS; i++) {
        fileRecord[i] = NEXUS_FileRecord_OpenPosix(data[i], indx[i]);
        BDBG_ASSERT(fileRecord[i]);

        rc = NEXUS_Record_Start(record[i], fileRecord[i]);
        BDBG_ASSERT(!rc);
    }

    /* record for a specified amount of time. fail if overflow occurs */
    then = b_get_time();
    is_normal_record = true;
    while (1) {
        void *buffer;
        size_t buffer_size;
        now = b_get_time();

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size)) {
            break;
        }
        if (buffer_size == 0) {
            BKNI_WaitForEvent(play_event, BKNI_INFINITE);
            continue;
        }

        if (buffer_size > MAX_PLAYPUMP_READ) {
            buffer_size = MAX_PLAYPUMP_READ;
        }

        buffer_size -= buffer_size%188;
        BKNI_Memcpy(buffer, pMem, buffer_size);

        rc = NEXUS_Playpump_WriteComplete(playpump, 0, buffer_size);
        BDBG_ASSERT(!rc);

        BKNI_Sleep(sleeptime);

        if (time_diff(then, now) > NORMAL_RECORD_TIME) {
            BKNI_Sleep(sleeptime);
            break;
        }
    }

    /* unwind */
    for (i=0; i<NUM_RECORDS; i++) {
        NEXUS_Record_Stop(record[i]);
    }
    NEXUS_Playpump_Stop(playpump);
    BKNI_DestroyEvent(play_event);
    BKNI_DestroyEvent(overflow_event);

    for (i=0; i<NUM_RECORDS; i++) {
        NEXUS_Record_RemoveAllPidChannels(record[i]);
        NEXUS_Record_Destroy(record[i]);
        NEXUS_Recpump_Close(recpump[i]);
        NEXUS_Playpump_ClosePidChannel(playpump, pidChannel[i]);
        NEXUS_FileRecord_Close(fileRecord[i]);
    }
    NEXUS_Playpump_Close(playpump);
    BKNI_Free(pMem);

    for (i=0; i<NUM_RECORDS; i++) {
        result |= check_nav_index(data[i], indx[i]);
    }
    BDBG_WRN(("Test completed. rc %d", result));

    NEXUS_Platform_Uninit();
    return result;
    
#endif /* NEXUS_HAS_RECORD */
    printf(("NEXUS_HAS_RECORD not defined. Test not run\n"));
    return 1;
}
