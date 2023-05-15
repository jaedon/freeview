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
 * $brcm_Workfile: playpump_scatter_gather.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/14/11 12:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/playpump_scatter_gather.c $
 * 
 * Hydra_Software_Devel/2   9/14/11 12:12p jtna
 * SW7420-2054: enforce boundsHeap for scatter-gather
 * 
 * Hydra_Software_Devel/1   7/21/11 2:31p jtna
 * SW7125-1015: added NEXUS_Playpump_SubmitScatterGatherDescriptor
 * unittests
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_playpump.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#define VERIFY_DESCRIPTORS 1

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(void)  {
    FILE *file;
    NEXUS_PlaypumpOpenSettings playpumpOpenSettings;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    const char *fname = "videos/cnnticker.mpg";
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_MemoryAllocationSettings memAllocSettings;
#define TOTAL_BUFFERS 10
    NEXUS_PlaypumpScatterGatherDescriptor sgDesc[TOTAL_BUFFERS];
    void *buf[TOTAL_BUFFERS];
    size_t buf_size = 128*1024;
    unsigned cur_buf;
    unsigned i;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

    NEXUS_Playpump_GetDefaultOpenSettings(&playpumpOpenSettings);
#if VERIFY_DESCRIPTORS
    /* specifying boundsHeap will do bounds check on every descriptor submitted. 
       you can verify that it's working by specifying different heaps for NEXUS_PlaypumpOpenSettings.boundsHeap 
       and NEXUS_MemoryAllocationSettings.heap below */
    playpumpOpenSettings.boundsHeap = platformConfig.heap[0];
#endif
    playpump = NEXUS_Playpump_Open(0, &playpumpOpenSettings);

    /* use stdio for file I/O to keep the example simple. */
    file = fopen(fname, "rb");
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        goto error;
    }

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_Playpump_Start(playpump);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0x21, NULL);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    /* Bring up video decoder and display. No audio to keep the example simple. */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if (platformConfig.outputs.component[0]) {
        NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    }
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);

    /* buffers must be from the nexus heap to be used by playpump; therefore use NEXUS_Memory_Allocate */
    NEXUS_Memory_GetDefaultAllocationSettings(&memAllocSettings);
    memAllocSettings.heap = platformConfig.heap[0];
    for (i=0;i<TOTAL_BUFFERS;i++) {
        NEXUS_Memory_Allocate(buf_size, &memAllocSettings, &buf[i]);
    }

    for(cur_buf=0;;) {
        int n;
        NEXUS_Error rc;
        NEXUS_PlaypumpStatus status;
        unsigned numBufRead, numCompleted, numToRead;

        rc = NEXUS_Playpump_GetStatus(playpump, &status);
        BDBG_ASSERT(!rc);
        if(status.descFifoDepth == TOTAL_BUFFERS) {
            /* every buffer is in use */
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            continue;
        }

        /* do a bunch of reads and build up the descriptors */
        numToRead = TOTAL_BUFFERS-status.descFifoDepth - rand()%(TOTAL_BUFFERS-status.descFifoDepth+1);
        for (numBufRead=0; numBufRead<numToRead; numBufRead++) {
            n = fread(buf[cur_buf], 1, buf_size, file);
            if (n < 0) goto error;
            if (n == 0) break;
            sgDesc[numBufRead].addr = buf[cur_buf];
            sgDesc[numBufRead].length = n;
            cur_buf = (cur_buf + 1)%TOTAL_BUFFERS; /* use the next buffer */
        }

        BKNI_Printf("queue %d\n", numBufRead);
        rc = NEXUS_Playpump_SubmitScatterGatherDescriptor(playpump, sgDesc, numBufRead, &numCompleted);
        BDBG_ASSERT(!rc);
        BDBG_ASSERT(numCompleted==numBufRead); /* we've already checked that there are enough descriptors available */
        
        if (n == 0) {
            /* wait for the decoder to reach the end of the content before looping */
            while (1) {
                NEXUS_VideoDecoderStatus status;
                NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                if (!status.queueDepth) break;
            }
            fseek(file, 0, SEEK_SET);
            NEXUS_Playpump_Flush(playpump);
        }
    }
    return 0;

error:
    return 1;
}
