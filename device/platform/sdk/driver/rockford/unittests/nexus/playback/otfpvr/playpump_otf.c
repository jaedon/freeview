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
 * $brcm_Workfile: playpump_otf.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 2/27/12 4:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/otfpvr/playpump_otf.c $
 * 
 * Hydra_Software_Devel/5   2/27/12 4:25p erickson
 * SW7425-2459: add hdmi
 * 
 * Hydra_Software_Devel/4   2/15/12 1:33p erickson
 * SW7425-2358: check return codes if OTF not enabled, memset
 * BOTF_MarkerParams to avoid uninitialized member
 * 
 * Hydra_Software_Devel/3   4/16/09 12:07p vsilyaev
 * PR 50887: Fixed typo
 * 
 * Hydra_Software_Devel/2   4/1/09 3:56p vsilyaev
 * PR 50887: Added more test modes
 * 
 * Hydra_Software_Devel/1   3/31/09 7:28p vsilyaev
 * PR 50887: OTFPVR support for the Nexus video decoder
 * 
 *
 *****************************************************************************/
/* Nexus otfpvr unit test app: playback and trickmoes */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_playpump.h"


#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "botf_marker.h"

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

#define MAX_READ (188*256)

static void 
print_status(NEXUS_VideoDecoderHandle videoDecoder, off_t position, const char *state)
{
    NEXUS_Error rc;
    NEXUS_VideoDecoderStatus status;

    rc=NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
    if(rc==NEXUS_SUCCESS) {
        fprintf(stderr, "\rPTS: %10u(%u)", status.pts, status.ptsType);
    }
    fprintf(stderr, " file: %10u: %-15s\t\t\r", (unsigned)position, state);
    return;
}

static void
wait_for_drain(NEXUS_PlaypumpHandle playpump, NEXUS_VideoDecoderHandle videoDecoder, off_t position)
{
    NEXUS_Error rc;
    NEXUS_PlaypumpStatus playpumpStatus;
    NEXUS_VideoDecoderStatus decoderStatus;

    for(;;) {
        rc=NEXUS_Playpump_GetStatus(playpump, &playpumpStatus);
        if(rc!=NEXUS_SUCCESS) {
            break;
        }
        if(playpumpStatus.fifoDepth==0) {
            break;
        }
        /* fprintf(stderr, "---P:%u\n", playpumpStatus.fifoDepth); */
        BKNI_Sleep(100);
        print_status(videoDecoder, position, "drain playpump");
    }
    for(;;) {
        rc=NEXUS_VideoDecoder_GetStatus(videoDecoder, &decoderStatus);
        if(rc!=NEXUS_SUCCESS) {
            break;
        }
        if(decoderStatus.queueDepth==0) {
            break;
        }
        /* fprintf(stderr, "---D:%u\n", decoderStatus.queueDepth); */
        BKNI_Sleep(100);
        print_status(videoDecoder, position, "drain decoder");
    }
    return;
}

static int
copy_data(FILE *file, size_t bytes, NEXUS_PlaypumpHandle playpump, BKNI_EventHandle event, NEXUS_VideoDecoderHandle videoDecoder, off_t position)
{
    while(bytes>0) {
        void *buffer;
        size_t buffer_size;
        int n;
        NEXUS_Error rc;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size)) {
            fprintf(stderr, "NEXUS_Playpump_GetBuffer failed");
            return -1;
        }
        if (buffer_size == 0) {
            rc = BKNI_WaitForEvent(event, 5*1000);
            if(rc==BERR_TIMEOUT) {
                fprintf(stderr, "copy_data:timeout\n");
            }
            print_status(videoDecoder, position, "copy_data");
            continue;
        }
        if (buffer_size > MAX_READ) {
            buffer_size = MAX_READ;
        }
        if(buffer_size>bytes) {
            buffer_size = bytes;
        }
        n = fread(buffer, 1, buffer_size, file);
        if (n!=(int)buffer_size ) {
            fprintf(stderr, "fread failed\n");
            return -1;
        }
        position += n;
        rc = NEXUS_Playpump_ReadComplete(playpump, 0, n);
        BDBG_ASSERT(!rc);
        bytes-=buffer_size;
    }
    return 0;
}

static int
send_data(const uint8_t *buf, size_t bytes, NEXUS_PlaypumpHandle playpump, BKNI_EventHandle event, NEXUS_VideoDecoderHandle videoDecoder, off_t position)
{
    while(bytes>0) {
        void *buffer;
        size_t buffer_size;
        NEXUS_Error rc;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size)) {
            fprintf(stderr, "NEXUS_Playpump_GetBuffer failed");
            return -1;
        }
        if (buffer_size == 0) {
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            if(rc==BERR_TIMEOUT) {
                fprintf(stderr, "send_data:timeout\n");
            }
            print_status(videoDecoder, position, "send_data");
            continue;
        }
        if(buffer_size>bytes) {
            buffer_size = bytes;
        }
        /* fprintf(stderr, "buffer_size %u\n", buffer_size); */
        BKNI_Memcpy(buffer, buf, buffer_size);
        rc = NEXUS_Playpump_ReadComplete(playpump, 0, buffer_size);
        BDBG_ASSERT(!rc);
        bytes-=buffer_size;
        buf += buffer_size;
    }
    return 0;
}

typedef enum {
    file_mode_sequential, 
    file_mode_forward,
    file_mode_backward
} file_mode;

typedef struct {
    unsigned mode;
    size_t chunk;
    size_t stride;
    off_t max_size;
    off_t start;
} send_desc;

static void send_desc_init(send_desc *desc)
{
    desc->mode = file_mode_sequential;
    desc->chunk = 3*188*1024;
    desc->stride = desc->chunk;
    desc->max_size = 0;
    desc->start = 0;
    return;
}

static int
send_file(FILE *file, NEXUS_PlaypumpHandle playpump, BKNI_EventHandle event, unsigned pid, const send_desc *desc, NEXUS_VideoDecoderHandle videoDecoder)
{
    unsigned i;
    off_t next_off;
    off_t file_size;
    off_t data_left;
    off_t position;

    fseeko(file, 0, SEEK_END);
    file_size = ftello(file);
    if(file_size<desc->chunk) {
       file_size = desc->chunk;
    }

    next_off = desc->start;
    if(desc->mode==file_mode_backward && next_off==0) {
        next_off = file_size - desc->chunk;
    } 
    if(desc->max_size) {
        data_left = desc->max_size;
    } else {
        data_left = file_size;
    }
    for(i=0;data_left>0;i++) {
        int rc;
        BOTF_MarkerParams marker;
        uint8_t packet[188*2];
        unsigned len;
        size_t block_size = desc->chunk;
       
        position=next_off;
     
        fseeko(file, next_off, SEEK_SET);
        print_status(videoDecoder, position, "send_file"); 

        BKNI_Memset(&marker, 0, sizeof(marker));
        marker.ChunkCntr = i;
        marker.OffsetHi = (uint32_t)(next_off>>32);
        marker.OffsetLow = (uint32_t)next_off;
        marker.Pid = pid;
        rc = BOTF_BuildMarkerTS( &marker, packet, sizeof(packet), &len);
        if (rc) return BERR_TRACE(rc);
        switch(desc->mode) {
        case file_mode_sequential:
            next_off += desc->chunk;
            data_left -= desc->chunk;
            break;
        case file_mode_forward:
            rc = send_data(packet, len, playpump, event, videoDecoder, position);
            if(rc<0) {
                break;
            }
            next_off+=desc->stride;
            data_left-=desc->stride;
            break;
        case file_mode_backward:
            rc = send_data(packet, len, playpump, event, videoDecoder, position);
            if(rc<0) {
                break;
            }
            if(next_off<desc->chunk) {
                if(next_off>0) {
                    block_size = next_off;
                } else {
                    break;
                }
            }
            next_off-=desc->stride;
            data_left-=desc->stride;
            break;
        }
        rc = copy_data(file, block_size, playpump, event, videoDecoder, position);
        if(rc<0) {
            break;
        }
    }
    wait_for_drain(playpump, videoDecoder, position);
    return 0;
}

#define SLOW_MO 1

int main(int argc, char **argv)  {
    FILE *file;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    const char *fname = "videos/ed_ipbb24_2mbps.ts";
    NEXUS_VideoDecoderTrickState trickState, defaultTrickState;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    uint16_t videoPid;
    unsigned i;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    int rc;

    if (argc > 1) {
        fname = argv[1];
    }
    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

    playpump = NEXUS_Playpump_Open(0, NULL);
    file = fopen(fname, "rb");
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        goto error;
    }

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    /* playpumpSettings.transportType = NEXUS_TransportType_eEs; */
    playpumpSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_Playpump_Start(playpump);
    
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Bring up video decoder and display. No audio to keep the example simple. */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings); /* take default capabilities */
    if(argc>4) {
        videoDecoderOpenSettings.fifoSize = 1024*strtol(argv[4],NULL,0);
    }
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings); 
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    /* BKNI_Sleep(500); */

    videoPid = 0x100;
    if(argc>2) {
        videoPid = strtol(argv[2],NULL,0);
    }

    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, videoPid, NULL);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    NEXUS_VideoDecoder_GetTrickState(videoDecoder, &trickState);
    defaultTrickState = trickState;
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);

    for(i=0;i<24;i++) {
        send_desc desc;
        send_desc_init(&desc);

        if(argc>3) {
            desc.chunk = 1024*strtol(argv[3],NULL,0);
            desc.stride = desc.chunk;
        }

        desc.max_size = ((uint64_t)1<<i) * 1024 * 1024 * 2;
        fprintf(stderr, "\n\nUsing up-to %u MBytes of data, block %u Kbytes, CDB %u Kbytes\n", (unsigned)(desc.max_size/(1024*1024)), desc.chunk/1024, videoDecoderOpenSettings.fifoSize/1024 );

        trickState = defaultTrickState;
        fprintf(stderr, "PLAYBACK WIHOUT OTF\n");
        trickState.reorderingMode = NEXUS_VideoDecoderReorderingMode_eNone;
        trickState.tsmEnabled = true;
        trickState.topFieldOnly = false;
        trickState.reverseFields = false;
        trickState.rate = NEXUS_NORMAL_PLAY_SPEED;
        NEXUS_Playpump_Flush(playpump);
        rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trickState);
        if (rc) {rc = BERR_TRACE(rc); goto err_videodecoder_otf;}
        send_file(file, playpump, event, videoPid, &desc, videoDecoder);


        fprintf(stderr, "\n\nENABLE OTF\n\n");
        trickState.reorderingMode = NEXUS_VideoDecoderReorderingMode_eSequential;
        trickState.tsmEnabled = false;
        trickState.reverseFields = false;
        trickState.rate = NEXUS_NORMAL_PLAY_SPEED/SLOW_MO;
        NEXUS_Playpump_Flush(playpump);
        rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trickState);
        if (rc) {rc = BERR_TRACE(rc); goto err_videodecoder_otf;}
        send_file(file, playpump, event, videoPid, &desc, videoDecoder);

        fprintf(stderr, "\n\nOTF 1xRew\n\n");
        trickState.reorderingMode = NEXUS_VideoDecoderReorderingMode_eInterleaved;
        trickState.tsmEnabled = false;
        trickState.rate = NEXUS_NORMAL_PLAY_SPEED/SLOW_MO;
        trickState.reverseFields = true;
        trickState.topFieldOnly = false;
        NEXUS_Playpump_Flush(playpump);
        rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trickState);
        if (rc) {rc = BERR_TRACE(rc); goto err_videodecoder_otf;}
        desc.mode = file_mode_backward;
        desc.stride = desc.chunk/2;
        send_file(file, playpump, event, videoPid, &desc, videoDecoder);
        desc.stride = desc.chunk;

        fprintf(stderr, "\n\nOTF Simple FF\n\n");
        trickState.reorderingMode = NEXUS_VideoDecoderReorderingMode_eChunkForward;
        trickState.tsmEnabled = false;
        trickState.rate = NEXUS_NORMAL_PLAY_SPEED/SLOW_MO;
        trickState.topFieldOnly = false;
        trickState.reverseFields = false;
        NEXUS_Playpump_Flush(playpump);
        rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trickState);
        if (rc) {rc = BERR_TRACE(rc); goto err_videodecoder_otf;}
        desc.mode = file_mode_forward;
        send_file(file, playpump, event, videoPid, &desc, videoDecoder);

        fprintf(stderr, "\n\nOTF Simple Rew\n\n");
        trickState.reorderingMode = NEXUS_VideoDecoderReorderingMode_eChunkBackward;
        trickState.tsmEnabled = false;
        trickState.rate = NEXUS_NORMAL_PLAY_SPEED/SLOW_MO;
        trickState.reverseFields = true;
        trickState.topFieldOnly = false;
        NEXUS_Playpump_Flush(playpump);
        rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trickState);
        if (rc) {rc = BERR_TRACE(rc); goto err_videodecoder_otf;}
        desc.mode = file_mode_backward;
        send_file(file, playpump, event, videoPid, &desc, videoDecoder);


        fprintf(stderr, "\n\nOTF 4xFastFF\n\n");
        trickState.reorderingMode = NEXUS_VideoDecoderReorderingMode_eChunkForward;
        trickState.tsmEnabled = false;
        trickState.rate = 4*NEXUS_NORMAL_PLAY_SPEED/SLOW_MO;
        trickState.topFieldOnly = false;
        trickState.reverseFields = false;
        NEXUS_Playpump_Flush(playpump);
        rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trickState);
        if (rc) {rc = BERR_TRACE(rc); goto err_videodecoder_otf;}
        desc.mode = file_mode_forward;
        send_file(file, playpump, event, videoPid, &desc, videoDecoder);

        fprintf(stderr, "\n\nOTF 4xFastRew\n\n");
        trickState.reorderingMode = NEXUS_VideoDecoderReorderingMode_eChunkBackward;
        trickState.tsmEnabled = false;
        trickState.rate = 4*NEXUS_NORMAL_PLAY_SPEED/SLOW_MO;
        trickState.reverseFields = true;
        trickState.topFieldOnly = false;
        NEXUS_Playpump_Flush(playpump);
        rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trickState);
        if (rc) {rc = BERR_TRACE(rc); goto err_videodecoder_otf;}
        desc.mode = file_mode_backward;
        send_file(file, playpump, event, videoPid, &desc, videoDecoder);

        fprintf(stderr, "\n\nOTF VeryFastFF\n\n");
        trickState.reorderingMode = NEXUS_VideoDecoderReorderingMode_eChunkForward;
        trickState.decodeMode = NEXUS_VideoDecoderDecodeMode_eI;
        trickState.tsmEnabled = false;
        trickState.rate = 40*NEXUS_NORMAL_PLAY_SPEED/SLOW_MO;
        trickState.topFieldOnly = false;
        trickState.reverseFields = false;
        NEXUS_Playpump_Flush(playpump);
        rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trickState);
        if (rc) {rc = BERR_TRACE(rc); goto err_videodecoder_otf;}
        desc.stride = desc.chunk*4;
        desc.mode = file_mode_forward;
        send_file(file, playpump, event, videoPid, &desc, videoDecoder);

        fprintf(stderr, "\n\nOTF VeryFastRew\n\n");
        trickState.reorderingMode = NEXUS_VideoDecoderReorderingMode_eChunkBackward;
        trickState.tsmEnabled = false;
        trickState.rate = 40*NEXUS_NORMAL_PLAY_SPEED/SLOW_MO;
        trickState.reverseFields = true;
        trickState.topFieldOnly = false;
        trickState.decodeMode = NEXUS_VideoDecoderDecodeMode_eI;
        NEXUS_Playpump_Flush(playpump);
        rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trickState);
        if (rc) {rc = BERR_TRACE(rc); goto err_videodecoder_otf;}
        desc.stride = desc.chunk*4;
        desc.mode = file_mode_backward;
        send_file(file, playpump, event, videoPid, &desc, videoDecoder);
        
    }

    /* loops forever */
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_Playpump_Stop(playpump);
    NEXUS_Playpump_Close(playpump); 
    NEXUS_Platform_Uninit();
    return 0;

error:
    return 1;
    
err_videodecoder_otf:
    printf("VideoDecoder not configured for OTF PVR. build with export OTFPVR_SUPPORT=y.\n");
    return -1;
}
