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
 * $brcm_Workfile: recpump_itb_svc.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/4/11 6:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/recpump_itb_svc.c $
 * 
 * Hydra_Software_Devel/1   3/4/11 6:18p vsilyaev
 * SW7422-14: SVC record case
 * 
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#include "nexus_playpump.h"

#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
BDBG_MODULE(recpump_itb_svc);

static void dataready_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    printf("overflow %s\n", (const char *)context);
}

static void print_status(NEXUS_RecpumpHandle recpump)
{
    NEXUS_RecpumpStatus status;
    NEXUS_Recpump_GetStatus(recpump, &status);
    printf("status: \n");
    printf("  data:  %u total bytes \tfifo %u/%u\n", (unsigned)status.data.bytesRecorded, status.data.fifoDepth, status.data.fifoSize);
    printf("  index: %u total bytes \tfifo %u/%u\n", (unsigned)status.index.bytesRecorded, status.index.fifoDepth, status.index.fifoSize);
}



static void play_read_data(NEXUS_PlaypumpHandle playpump, FILE *fin, unsigned count)
{
    for(;count>0;count--) {
        void *buffer;
        size_t buffer_size;
        NEXUS_Error rc;
        int n;

        rc = NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size);
        BDBG_ASSERT(rc==NEXUS_SUCCESS);
        if (buffer_size == 0) {
           break;
        }
#define MAX_READ (188*1024)
        if (buffer_size > MAX_READ) {
            buffer_size = MAX_READ;
        }
        n = fread(buffer, 1, buffer_size, fin);
        BDBG_WRN(("reading %d bytes(%u)", n, buffer_size));
        BDBG_ASSERT(n>=0);
        if (n == 0) {
            BDBG_WRN(("End of File"));
            break;
        }
        rc = NEXUS_Playpump_WriteComplete(playpump, 0, n);
        BDBG_ASSERT(rc==NEXUS_SUCCESS);
    }
    return;
}


static void play_callback(void *context, int param)
{
    play_read_data(context, (FILE *)param, 2);
    return;
}

static void play_file(const char *fname, NEXUS_PlaypumpHandle playpump)
{
    FILE *fin = fopen(fname, "rb");
    NEXUS_PlaypumpSettings playpumpSettings;

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = playpump;
    playpumpSettings.dataCallback.param = (int)fin;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
    NEXUS_Playpump_Start(playpump);
    return;
}

int main(void) {
    FILE *index_file;
    FILE *data_file;
    const char *index_fname = "videos/stream.stc";
    const char *data_fname = "videos/stream.mpg";
    const char *source_file = "videos/test_sequenceB.ts";
    BKNI_EventHandle event;
    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_RecpumpAddPidChannelSettings addPidChannelSettings;
    NEXUS_RecpumpHandle recpump;
    NEXUS_PidChannelHandle pidChannel[2];
    NEXUS_PlaypumpHandle playpump;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);

    BKNI_CreateEvent(&event);

    recpump = NEXUS_Recpump_Open(0, NULL);

    index_file = fopen(index_fname, "wb");
    if (!index_file) {
        fprintf(stderr, "can't open index file:%s\n", index_fname);
        goto error;
    }
    data_file = fopen(data_fname, "wb");
    if (!data_file) {
        fprintf(stderr, "can't open data file:%s\n", data_fname);
        goto error;
    }

    playpump = NEXUS_Playpump_Open(0, NULL);
    play_file(source_file, playpump);

    NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = dataready_callback;
    recpumpSettings.data.dataReady.context = event;
    recpumpSettings.index.dataReady.callback = dataready_callback; /* same callback */
    recpumpSettings.index.dataReady.context = event; /* same event */
    recpumpSettings.data.overflow.callback = overflow_callback;
    recpumpSettings.data.overflow.context = "data";
    recpumpSettings.index.overflow.callback = overflow_callback;
    recpumpSettings.index.overflow.context = "index";
    NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);

    NEXUS_Recpump_GetDefaultAddPidChannelSettings(&addPidChannelSettings);
    addPidChannelSettings.pidType = NEXUS_PidType_eVideo;
    addPidChannelSettings.pidTypeSettings.video.index = true;
    addPidChannelSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH264;
    addPidChannelSettings.pidTypeSettings.video.pid = 0x31;
    pidChannel[0] = NEXUS_Playpump_OpenPidChannel(playpump, addPidChannelSettings.pidTypeSettings.video.pid, NULL);
    NEXUS_Recpump_AddPidChannel(recpump, pidChannel[0], &addPidChannelSettings);

    addPidChannelSettings.pidTypeSettings.video.index = true;
    addPidChannelSettings.pidTypeSettings.video.pid = 0x32;
    addPidChannelSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH264_Svc;
    pidChannel[1] = NEXUS_Playpump_OpenPidChannel(playpump, addPidChannelSettings.pidTypeSettings.video.pid, NULL);
    NEXUS_Recpump_AddPidChannel(recpump, pidChannel[1], &addPidChannelSettings);

    NEXUS_Recpump_Start(recpump);
    BKNI_WaitForEvent(event, BKNI_INFINITE);

    while (1) {
        const void *data_buffer, *index_buffer;
        size_t data_buffer_size, index_buffer_size;
        int n;

        if (NEXUS_Recpump_GetDataBuffer(recpump, &data_buffer, &data_buffer_size))
            break;
        if (NEXUS_Recpump_GetIndexBuffer(recpump, &index_buffer, &index_buffer_size))
            break;
        if (data_buffer_size == 0 && index_buffer_size == 0) {
            fflush(data_file);
            fflush(index_file);
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            continue;
        }

        if (data_buffer_size) {
            n = fwrite(data_buffer, 1, data_buffer_size, data_file);
            if (n < 0) goto error;
            NEXUS_Recpump_DataReadComplete(recpump, data_buffer_size);
        }
        if (index_buffer_size) {
            n = fwrite(index_buffer, 1, index_buffer_size, index_file);
            if (n < 0) goto error;
            NEXUS_Recpump_IndexReadComplete(recpump, n);
        }
        printf("wrote %d data, %d index\n", data_buffer_size, index_buffer_size);
        print_status(recpump);
    }
    /* loops forever */

error:
    return 1;
}
