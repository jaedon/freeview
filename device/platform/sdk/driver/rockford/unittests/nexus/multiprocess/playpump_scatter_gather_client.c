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
 * $brcm_Workfile: playpump_scatter_gather_client.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 12/5/11 3:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/multiprocess/playpump_scatter_gather_client.c $
 * 
 * Hydra_Software_Devel/4   12/5/11 3:31p erickson
 * SW7420-2129: use NULL for default settings now that default heap can be
 * obtained via NEXUS_P_DefaultHeap
 * 
 * Hydra_Software_Devel/3   11/18/11 1:23p erickson
 * SW7420-1148: fix for untrusted client
 * 
 * Hydra_Software_Devel/2   10/12/11 12:25p erickson
 * SW7420-1135: api change
 * 
 * Hydra_Software_Devel/1   7/29/11 12:06p erickson
 * SW7420-1991: add multiprocess tests for nelem and nelem_out attributes
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_simple_video_decoder.h"
#include "nexus_component_output.h"
#include "nexus_playpump.h"
#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

BDBG_MODULE(playpump_scatter_gather_client);

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(void)  {
    FILE *file;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    const char *fname = "videos/cnnticker.mpg";
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_SimpleVideoDecoderHandle videoDecoder;
    NEXUS_SimpleVideoDecoderStartSettings videoProgram;
#define TOTAL_BUFFERS 10
    void *buf[TOTAL_BUFFERS];
    size_t buf_size = 128*1024;
    unsigned cur_buf;
    unsigned i;
    NEXUS_Error rc;
    unsigned loops = 3;

    NEXUS_Platform_Join();

    BKNI_CreateEvent(&event);

    playpump = NEXUS_Playpump_Open(0, NULL);

    /* use stdio for file I/O to keep the example simple. */
    file = fopen(fname, "rb");
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        goto error;
    }

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    /* setting mode = NEXUS_PlaypumpMode_eScatterGather is deprecated */
    rc = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Playpump_Start(playpump);
    BDBG_ASSERT(!rc);

    videoDecoder = NEXUS_SimpleVideoDecoder_Acquire(NEXUS_ANY_ID);
    if (!videoDecoder) {
        BDBG_ERR(("video decoder not available"));
        return -1;
    }
    
    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0x21, NULL);

    NEXUS_SimpleVideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.settings.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.settings.pidChannel = videoPidChannel;
    rc = NEXUS_SimpleVideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);

    /* buffers must be from the nexus heap to be used by playpump; therefore use NEXUS_Memory_Allocate */
    for (i=0;i<TOTAL_BUFFERS;i++) {
        NEXUS_MemoryAllocationSettings allocSettings;
        NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
        NEXUS_Memory_Allocate(buf_size, &allocSettings, &buf[i]);
    }

    for(cur_buf=0;;) {
        int n;
        NEXUS_Error rc;
        NEXUS_PlaypumpStatus status;

        rc = NEXUS_Playpump_GetStatus(playpump, &status);
        BDBG_ASSERT(!rc);
        if(status.descFifoDepth == TOTAL_BUFFERS) {
            /* every buffer is in use */
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            continue;
        }

        n = fread(buf[cur_buf], 1, buf_size, file);
        if (n < 0) goto error;
        if (n == 0) {
            if (--loops == 0) break; /* finite test */    
#if 0
            /* wait for the decoder to reach the end of the content before looping */
            while (1) {
                NEXUS_VideoDecoderStatus status;
                NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                if (!status.queueDepth) break;
            }
#else
            /* TODO */
            BKNI_Sleep(1000);    
#endif
            fseek(file, 0, SEEK_SET);
            NEXUS_Playpump_Flush(playpump);
        }
        else {
            NEXUS_PlaypumpScatterGatherDescriptor desc;
            unsigned numConsumed;
            desc.addr = buf[cur_buf];
            desc.length = n;
            rc = NEXUS_Playpump_SubmitScatterGatherDescriptor(playpump, &desc, 1, &numConsumed);
            BDBG_ASSERT(!rc);
            BDBG_ASSERT(numConsumed==1); /* we've already checked that there are descriptors available*/
            cur_buf = (cur_buf + 1)%TOTAL_BUFFERS; /* use the next buffer */
        }
    }
    return 0;

error:
    return 1;
}
