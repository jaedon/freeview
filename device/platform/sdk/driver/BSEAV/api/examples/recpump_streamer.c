/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: recpump_streamer.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/recpump_streamer.c $
 * 
 * 2   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

struct context {
    FILE *data;
    FILE *index;
    brecpump_t pump;
    BKNI_EventHandle data_event, index_event;
} g_context;

void data_write_callback(void *context_)
{
    struct context *context = (struct context *)context_;
    printf("data event\n");
    BKNI_SetEvent(context->data_event);
}

void index_write_callback(void *context_)
{
    struct context *context = (struct context *)context_;
    printf("index event\n");
    BKNI_SetEvent(context->index_event);
}

void *data_write(void *context_)
{
    struct context *context = (struct context *)context_;

    while (1) {
        const void *buffer;
        size_t buffer_size;
        int n;
        if (brecpump_data_get_buffer(context->pump, &buffer, &buffer_size))
            break;
        if (!buffer_size) {
            BKNI_WaitForEvent(context->data_event, BKNI_INFINITE);
            continue;
        }

        n = fwrite(buffer, 1, buffer_size, context->data);
        printf("data %d\n", n);
        if (n < 0 || brecpump_data_write_complete(context->pump, n))
            break;
    }
    printf("data done\n");
    return NULL;
}

void *index_write(void *context_)
{
    struct context *context = (struct context *)context_;

    while (1) {
        const void *buffer;
        size_t buffer_size;
        int n;
        if (brecpump_index_get_buffer(context->pump, &buffer, &buffer_size))
            break;
        if (!buffer_size) {
            BKNI_WaitForEvent(context->index_event, BKNI_INFINITE);
            continue;
        }

        n = fwrite(buffer, 1, buffer_size, context->index);
        printf("index %d\n", n);
        if (n < 0 || brecpump_index_write_complete(context->pump, n))
            break;
    }
    printf("index done\n");
    return NULL;
}

int main(void) {
    bresult rc = bsettop_init(BSETTOP_VERSION);
    bdecode_t decode = bdecode_open(B_ID(0));
    bdisplay_t display = bdisplay_open(B_ID(0));
    bdecode_window_t window = bdecode_window_open(B_ID(0), display);
    bstream_t stream;
    bstream_mpeg mpeg;
    bband_t band;
    brecpump_params params;
    pthread_t data_thread, index_thread;
    if (rc) goto error;

    bstream_mpeg_init(&mpeg);
    mpeg.video[0].pid = 0x31;
    mpeg.audio[0].pid = 0x34;
    mpeg.audio[0].format = baudio_format_ac3;
    mpeg.pcr_pid = 0x31;

    band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
    if (!band) goto error;

    stream = bstream_open(band, &mpeg);
    if (!stream) goto error;

    if (bdecode_start(decode, stream, window))
        goto error;


    printf("press to start record\n");
    getchar(); /* press enter to stop decode */


    g_context.data = fopen("stream.mpg", "w+");
    assert(g_context.data);
    g_context.index = fopen("stream.sct", "w+");
    assert(g_context.index);
    g_context.pump = brecpump_open(B_ID(0), NULL);


    BKNI_CreateEvent(&g_context.data_event);
    BKNI_CreateEvent(&g_context.index_event);

    brecpump_params_init(&params, g_context.pump);
    params.callback_context = &g_context;
    params.data_write_callback = data_write_callback;
    params.index_write_callback = index_write_callback;

    if (brecpump_start(g_context.pump, stream, &params))
        goto error;

    pthread_create(&data_thread, NULL, data_write, &g_context);
    pthread_create(&index_thread, NULL, index_write, &g_context);

    getchar(); /* press enter to stop record */

    brecpump_stop(g_context.pump);
    BKNI_SetEvent(g_context.data_event);
    BKNI_SetEvent(g_context.index_event);
    pthread_join(data_thread, NULL);
    pthread_join(index_thread, NULL);

    bsettop_uninit();

    return 0;
error:
    return 1;
}
