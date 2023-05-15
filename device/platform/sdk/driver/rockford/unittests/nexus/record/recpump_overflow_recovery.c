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
 * $brcm_Workfile: recpump_overflow_recovery.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 1/18/11 5:18p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/record/recpump_overflow_recovery.c $
 * 
 * Hydra_Software_Devel/3   1/18/11 5:18p vsilyaev
 * SW7420-1113: Disabled HW flow control for 'overflow' unit-tests
 * 
 * Hydra_Software_Devel/2   12/2/10 12:39p jtna
 * SW7125-716: rework overflow recovery unittests
 * 
 * Hydra_Software_Devel/1   12/11/08 6:21p jtna
 * PR48779: initial version
 * 
 *****************************************************************************/

/* This is an automated unit test for recpump overflow recovery.
     
a) Induce a recpump overflow and continue recording for a specified amount of time.
b) Stop the record, then restart it without overflow. Continue recording for a specified amount of time.
c) Check the indices of the SCT6 file against the data file for correctness.
     
*/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#include "nexus_memory.h"
#include "examples_lib.h"
#include "check_index_lib.h"

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

BDBG_MODULE(recpump_overflow_recovery);

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

/* callback when buffer exceeds dataReadyThreshold */
static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

unsigned now, then;
bool has_overflowed = false;
bool is_normal_record = false;

#define OVERFLOW_RECORD_TIME (10*1000)  /* time to continue recording after first overflow detected */
#define NORMAL_RECORD_TIME   (10*1000) /* time to record for after overflow recovery */

static void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(param);
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

#define RECORD_FROM_STREAMER 0 /* if 1, record from streamer. if 0, record using generated data */

int main(void) {
    FILE *fdata0, *findx0, *fdata1, *findx1;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_RecpumpAddPidChannelSettings addPidChannelSettings;
    NEXUS_PidChannelHandle pidChannel;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlatformSettings platformSettings;
#if RECORD_FROM_STREAMER
    ExampleDecodeSettings decodeSettings;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
#endif
    NEXUS_Error rc;
    int result = 0;

    BKNI_EventHandle play_event, overflow_event;
    unsigned char *pMem = NULL;

    /* record with overflow */
    const char *data0 = "videos/recpump_overflow0.mpg";
    const char *indx0 = "videos/recpump_overflow0.idx";
    /* record without overflow */
    const char *data1 = "videos/recpump_overflow1.mpg";
    const char *indx1 = "videos/recpump_overflow1.idx";

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

#if (!RECORD_FROM_STREAMER)
    #define NUMPACKETS_TO_ALLOC 512
    #define NUMPACKETS_PER_RAP 128
    BDBG_CASSERT(NUMPACKETS_TO_ALLOC%NUMPACKETS_PER_RAP==0);
    #define TEST_PID 0x11

    pMem = BKNI_Malloc(188*NUMPACKETS_TO_ALLOC);
    generate_payload(pMem, TEST_PID, NUMPACKETS_TO_ALLOC, NUMPACKETS_PER_RAP);
#endif

    BKNI_CreateEvent(&play_event);
    BKNI_CreateEvent(&overflow_event);

#if RECORD_FROM_STREAMER
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    
    ExampleDecode_GetDefaultSettings(&decodeSettings);
    decodeSettings.videoPid = 0x31;
    decodeSettings.videoCodec = NEXUS_VideoCodec_eMpeg2;
    decodeSettings.audioPid = 0x34;
    decodeSettings.audioCodec = NEXUS_AudioCodec_eAc3;
    decodeSettings.parserBand = parserBand;
    StartExampleDecode(&decodeSettings);
#else
    playpump = NEXUS_Playpump_Open(0, NULL);
    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = play_event;
    rc = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Playpump_Start(playpump);
    BDBG_ASSERT(!rc);
#endif

    recpump = NEXUS_Recpump_Open(0, NULL);

    NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = play_callback;
    recpumpSettings.data.dataReady.context = play_event;
    recpumpSettings.index.dataReady.callback = play_callback;
    recpumpSettings.index.dataReady.context = play_event;
    recpumpSettings.data.overflow.callback = overflow_callback;
    recpumpSettings.data.overflow.context = overflow_event;
    recpumpSettings.index.overflow.callback = overflow_callback;
    recpumpSettings.index.overflow.context = overflow_event;
    recpumpSettings.bandHold = NEXUS_RecpumpFlowControl_eDisable;
    rc = NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);
    BDBG_ASSERT(!rc);

    fdata0 = fopen(data0, "wb"); BDBG_ASSERT(fdata0);
    findx0 = fopen(indx0, "wb"); BDBG_ASSERT(findx0);
    fdata1 = fopen(data1, "wb"); BDBG_ASSERT(fdata1);
    findx1 = fopen(indx1, "wb"); BDBG_ASSERT(findx1);

    NEXUS_Recpump_GetDefaultAddPidChannelSettings(&addPidChannelSettings);
    addPidChannelSettings.pidType = NEXUS_PidType_eVideo;
    addPidChannelSettings.pidTypeSettings.video.index = true;
    addPidChannelSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
#if RECORD_FROM_STREAMER
    pidChannel = NEXUS_PidChannel_Open(parserBand, 0x31, NULL);
    rc = NEXUS_Recpump_AddPidChannel(recpump, pidChannel, &addPidChannelSettings);
    BDBG_ASSERT(!rc);
#else
    pidChannel = NEXUS_Playpump_OpenPidChannel(playpump, TEST_PID, NULL);
    rc = NEXUS_Recpump_AddPidChannel(recpump, pidChannel, &addPidChannelSettings);
    BDBG_ASSERT(!rc);
#endif
/*goto normal;*/

    /* record data until overflow occurs. once overflow occurs, keep recording for a specified amount of time */
    then = b_get_time();

    BDBG_WRN(("Recpump started. Overflow should occur"));
    rc = NEXUS_Recpump_Start(recpump); 
    BDBG_ASSERT(!rc);

    while (BKNI_WaitForEvent(overflow_event, 0)) {
        const void *data_buffer, *index_buffer;
        size_t data_buffer_size, index_buffer_size;
        int n;

#if (!RECORD_FROM_STREAMER)
        void *buffer;
        size_t buffer_size;
        NEXUS_Error rc;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
            break;
        if (buffer_size == 0) {
            BKNI_WaitForEvent(play_event, BKNI_INFINITE);
            continue;
        }

        /* The first call to get_buffer will return the entire playback buffer.
        If we use it, we're going to have to wait until the descriptor is complete,
        and then we're going to underflow. So use a max size. */
        #define MAX_READ (188*512)
        BDBG_CASSERT(MAX_READ <= 188*NUMPACKETS_TO_ALLOC); /* we can't read more than we allocated */
        if (buffer_size > MAX_READ)
            buffer_size = MAX_READ;

        buffer_size -= buffer_size%188;
        BKNI_Memcpy(buffer, pMem, buffer_size);
        rc = NEXUS_Playpump_ReadComplete(playpump, 0, buffer_size);
        BDBG_ASSERT(!rc);
#endif

        if (NEXUS_Recpump_GetDataBuffer(recpump, &data_buffer, &data_buffer_size)) {
            break;
        }
        if (NEXUS_Recpump_GetIndexBuffer(recpump, &index_buffer, &index_buffer_size)) {
            break;
        }
        if (data_buffer_size == 0 && index_buffer_size == 0) {
            BKNI_WaitForEvent(play_event, BKNI_INFINITE);
            continue;
        }

        if (data_buffer_size) {
            n = fwrite(data_buffer, 1, data_buffer_size, fdata0);
            if (n < 0) goto error;
            NEXUS_Recpump_DataWriteComplete(recpump, n/4); /* consume data slowly. this induces an overflow */
        }
        if (index_buffer_size) {
            n = fwrite(index_buffer, 1, index_buffer_size, findx0);
            if (n < 0) goto error;
            NEXUS_Recpump_IndexWriteComplete(recpump, n/4);
        }
    }

    NEXUS_Recpump_Stop(recpump);
    rc = NEXUS_Playpump_Flush(playpump); 
    BDBG_ASSERT(!rc);
    BKNI_Sleep(1000);

    BDBG_WRN(("Completed record with overflow. Now starting record without overflow"));
    NEXUS_Recpump_Start(recpump);

    /* record for a specified amount of time. fail if overflow occurs */
    then = b_get_time();
    is_normal_record = true;
    while (1) {
        const void *data_buffer, *index_buffer;
        size_t data_buffer_size, index_buffer_size;
        int n;

#if (!RECORD_FROM_STREAMER)
        void *buffer;
        size_t buffer_size;
        NEXUS_Error rc;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size)) {
            break;
        }
        if (buffer_size == 0) {
            BKNI_WaitForEvent(play_event, BKNI_INFINITE);
            continue;
        }

        if (buffer_size > MAX_READ) {
            buffer_size = MAX_READ;
        }

        buffer_size -= buffer_size%188;
        BKNI_Memcpy(buffer, pMem, buffer_size);
        rc = NEXUS_Playpump_WriteComplete(playpump, 0, buffer_size);
        BDBG_ASSERT(!rc);
#endif

        now = b_get_time();
        if (NEXUS_Recpump_GetDataBuffer(recpump, &data_buffer, &data_buffer_size))
            break;
        if (NEXUS_Recpump_GetIndexBuffer(recpump, &index_buffer, &index_buffer_size))
            break;
        if (data_buffer_size == 0 && index_buffer_size == 0) {
            BKNI_WaitForEvent(play_event, BKNI_INFINITE);
            continue;
        }

        if (data_buffer_size) {
            n = fwrite(data_buffer, 1, data_buffer_size, fdata1);
            if (n < 0) goto error;
            NEXUS_Recpump_DataReadComplete(recpump, n);
        }
        if (index_buffer_size) {
            n = fwrite(index_buffer, 1, index_buffer_size, findx1);
            if (n < 0) goto error;
            NEXUS_Recpump_IndexReadComplete(recpump, n);
        }

        if (time_diff(then, now) > NORMAL_RECORD_TIME) {
            break;
        }
    }

    /* unwind */
    NEXUS_Recpump_Stop(recpump);
    NEXUS_Playpump_Stop(playpump);
    BKNI_DestroyEvent(play_event);
    BKNI_DestroyEvent(overflow_event);
    NEXUS_Recpump_RemoveAllPidChannels(recpump);
#if RECORD_FROM_STREAMER
    NEXUS_PidChannel_Close(pidChannel);
#else
    NEXUS_Playpump_ClosePidChannel(playpump, pidChannel);
#endif
    NEXUS_Recpump_Close(recpump);
    NEXUS_Playpump_Close(playpump);
    fclose(fdata0);
    fclose(findx0);
    fclose(fdata1);
    fclose(findx1);
    BKNI_Free(pMem);

    result = check_sct6_index(data1, indx1);
    BDBG_WRN(("Test completed. rc %d", result));

    NEXUS_Platform_Uninit();
    return result;
    
error:
    BDBG_WRN(("Test aborted due to unknown error"));
    return 1;
}
