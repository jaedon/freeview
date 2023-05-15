/******************************************************************************
 *    (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: recpump_buffer_threshold.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/27/12 6:41p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/recpump_buffer_threshold.c $
 * 
 * Hydra_Software_Devel/1   7/27/12 6:41p vsilyaev
 * SW7435-188: Test case to verify advertised behaviour of FIFO depth
 * callback notifications
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_playback.h"
#include "nexus_recpump.h"
#include "nexus_file.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(recpump_buffer_threshold);

#define LOG(x) BDBG_LOG(x)

static struct {
    unsigned dataready_index;
    unsigned dataready_data;
} status = {0,0};

static NEXUS_RecpumpOpenSettings recpumpOpenSettings;
static BKNI_EventHandle event;
static NEXUS_PlaybackHandle playback;

static void dataready_callback_index(void *context, int param)
{
    BSTD_UNUSED(param);
    status.dataready_index++;
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void dataready_callback_data(void *context, int param)
{
    BSTD_UNUSED(param);
    status.dataready_data++;
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BDBG_WRN(("overflow %s", (const char *)context));
}

static void print_status(NEXUS_RecpumpHandle recpump)
{
    NEXUS_Error rc;
    NEXUS_RecpumpStatus rstatus;
    NEXUS_PlaybackStatus pstatus;
    const void *data_buffer, *index_buffer;
    size_t data_buffer_size, index_buffer_size;

    rc = NEXUS_Recpump_GetStatus(recpump, &rstatus);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Playback_GetStatus(playback, &pstatus);
    BDBG_ASSERT(!rc);
    rc=NEXUS_Recpump_GetDataBuffer(recpump, &data_buffer, &data_buffer_size);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    rc=NEXUS_Recpump_GetIndexBuffer(recpump, &index_buffer, &index_buffer_size);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);

    LOG(("playback bytesPlayed %d", (unsigned)pstatus.bytesPlayed));
    LOG(("recpump data:  %u total bytes \tfifo %u/%u", (unsigned)rstatus.data.bytesRecorded, rstatus.data.fifoDepth, rstatus.data.fifoSize));
    LOG(("recpump index: %u total bytes \tfifo %u/%u", (unsigned)rstatus.index.bytesRecorded, rstatus.index.fifoDepth, rstatus.index.fifoSize));
    LOG(("dataready index:(%u,%u) data:(%u,%u)", index_buffer_size, status.dataready_index, data_buffer_size, status.dataready_data));

    return;
}

static void flush_data(NEXUS_RecpumpHandle recpump)
{
    const void *data_buffer;
    size_t data_buffer_size;
    NEXUS_Error rc;

    rc=NEXUS_Recpump_GetDataBuffer(recpump, &data_buffer, &data_buffer_size);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    if(data_buffer_size) {
        NEXUS_Recpump_DataReadComplete(recpump, data_buffer_size);
        rc=NEXUS_Recpump_GetDataBuffer(recpump, &data_buffer, &data_buffer_size);
        BDBG_ASSERT(rc==NEXUS_SUCCESS);
        if(data_buffer_size) {
            NEXUS_Recpump_DataReadComplete(recpump, data_buffer_size);
        }
    }
    return;
}

static void wait_for_itb(NEXUS_RecpumpHandle recpump, unsigned threshold)
{
    for(;;) {
        NEXUS_Error rc;
        const void *index_buffer;
        size_t index_buffer_size;

        rc=NEXUS_Recpump_GetIndexBuffer(recpump, &index_buffer, &index_buffer_size);
        BDBG_ASSERT(rc==NEXUS_SUCCESS);
        LOG(("wait_for_itb:%u,%u", threshold, index_buffer_size));
        if(index_buffer_size>=threshold) {
            break;
        }
        print_status(recpump);
        flush_data(recpump);
        BKNI_WaitForEvent(event, 100);
    }
    return;
}

static void consume_itb(NEXUS_RecpumpHandle recpump, unsigned bytes)
{
    NEXUS_Error rc;
    const void *index_buffer;
    size_t index_buffer_size;
    rc=NEXUS_Recpump_GetIndexBuffer(recpump, &index_buffer, &index_buffer_size);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    LOG(("consume_itb:%u,%u", bytes, index_buffer_size));
    BDBG_ASSERT(index_buffer_size>=bytes);
    rc=NEXUS_Recpump_IndexReadComplete(recpump, bytes);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    return;
}

int main(int argc, char **argv) {
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PidChannelHandle pidChannel;
    NEXUS_FilePlayHandle playbackFile;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_RecpumpAddPidChannelSettings pidCfg;
    NEXUS_Error rc;
    const char *fname = "videos/stream.mpg";
    NEXUS_PlaypumpHandle playpump;
    unsigned itb_atom;

    if (argc > 1) fname = argv[1];

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    BKNI_CreateEvent(&event);

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    playbackFile = NEXUS_FilePlay_OpenPosix(fname, NULL);
    if (!playbackFile) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.playpumpSettings.maxDataRate = 2 * 1000000; /* prevent record overflow because of limited disk i/o speed */
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    pidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x21, NULL);
    BDBG_ASSERT(pidChannel);

    NEXUS_Recpump_GetDefaultOpenSettings(&recpumpOpenSettings);
    recpump = NEXUS_Recpump_Open(0, &recpumpOpenSettings);
    BDBG_ASSERT(recpump);

    NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = dataready_callback_data;
    recpumpSettings.data.dataReady.context = event;
    recpumpSettings.index.dataReady.callback = dataready_callback_index; /* same callback */
    recpumpSettings.index.dataReady.context = event; /* same event */
    recpumpSettings.data.overflow.callback = overflow_callback;
    recpumpSettings.data.overflow.context = "data";
    recpumpSettings.index.overflow.callback = overflow_callback;
    recpumpSettings.index.overflow.context = "index";
    NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);

    NEXUS_Recpump_GetDefaultAddPidChannelSettings(&pidCfg);
    pidCfg.pidType = NEXUS_PidType_eVideo;
    pidCfg.pidTypeSettings.video.index = true;
    pidCfg.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH264;
    pidCfg.pidTypeSettings.video.pid = 0x21; 
    rc = NEXUS_Recpump_AddPidChannel(recpump, pidChannel, &pidCfg);
    BDBG_ASSERT(!rc);


    rc = NEXUS_Recpump_Start(recpump);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Playback_Start(playback, playbackFile, NULL);
    BDBG_ASSERT(!rc);
    
    itb_atom = recpumpOpenSettings.index.dataReadyThreshold;
    wait_for_itb(recpump, itb_atom*2);
    consume_itb(recpump, itb_atom);
    wait_for_itb(recpump, itb_atom*4);
    consume_itb(recpump, itb_atom);
    wait_for_itb(recpump, itb_atom*6);
    consume_itb(recpump, itb_atom);
    wait_for_itb(recpump, itb_atom*8);
    getchar();

    /* Bring down system */
    NEXUS_Recpump_Stop(recpump);
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(playbackFile);
    NEXUS_Recpump_RemoveAllPidChannels(recpump);
    NEXUS_Recpump_Close(recpump);
	NEXUS_Playback_Destroy(playback);
	NEXUS_Playpump_Close(playpump);
    BKNI_DestroyEvent(event);
    NEXUS_Platform_Uninit();

#endif
    return 0;
}
