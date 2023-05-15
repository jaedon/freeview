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
 * $brcm_Workfile: b_playback_decode.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 4/12/11 11:42a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/utils/b_playback_decode.c $
 * 
 * 1   4/12/11 11:42a erickson
 * SW7420-1797: add nexus-based decode app using b_playback
 *
 *****************************************************************************/

/**
this example shows how to do nexus playback and decode using the b_playback wrapper around bcmplayer.
this direct-to-bcmplayer mode bypasses NEXUS_Playback and media framework which allows for customized low-level access.
**/

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_playpump.h"
#include "b_playback.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(int argc, char **argv)  {
    int rc;
    b_playback_t p;
    struct b_playback_start_settings start_settings;
    struct b_playback_trick_mode trick_mode;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoCodec videoCodec = NEXUS_VideoCodec_eMpeg2;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    p = b_playback_create(NULL);
    BDBG_ASSERT(p);

    b_playback_get_default_start_settings(&start_settings);
    if (argc > 1) {
        start_settings.datafile = argv[1];
    }
    if (argc > 2) {
        start_settings.indexfile = argv[2];
    }
    if (argc > 3) {
        start_settings.video_pid = strtoul(argv[3], NULL, 0);
    }
    if (argc > 4) {
        if (!strcasecmp(argv[4], "avc")) {
            videoCodec = NEXUS_VideoCodec_eH264;
        }
    }
    rc = b_playback_start(p, &start_settings);
    BDBG_ASSERT(!rc);

    BKNI_CreateEvent(&event);

    playpump = NEXUS_Playpump_Open(0, NULL);

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_Playpump_Start(playpump);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, start_settings.video_pid, NULL);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = videoCodec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);

#define TEST_TRICK 1
#if TEST_TRICK
    b_playback_get_default_trick_mode(&trick_mode);
    trick_mode.rate = B_PLAYBACK_NORMAL_RATE * 8;
    rc = b_playback_trick_mode(p, &trick_mode);
    BDBG_ASSERT(!rc);
    {
        NEXUS_VideoDecoderTrickState trickState;
        NEXUS_VideoDecoder_GetTrickState(videoDecoder, &trickState);
        trickState.hostTrickModesEnabled = true;
        trickState.tsmEnabled = false;
        rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trickState);
        BDBG_ASSERT(!rc);
    }
#endif

    while (1) {
        void *buffer, *input_buffer;
        size_t buffer_size, input_size;
        NEXUS_Error rc;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
            break;
        if (buffer_size == 0) {
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            continue;
        }

        rc = b_playback_get_buffer(p, &input_buffer, &input_size);
        BDBG_ASSERT(!rc);

        if (input_size == 0) {
            /* wait for the decoder to reach the end of the content before looping */
            while (1) {
                NEXUS_VideoDecoderStatus status;
                NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                if (!status.queueDepth) break;
            }
            b_playback_seek(p, 0, SEEK_SET);
            NEXUS_Playpump_Flush(playpump);
            continue;
        }

        /* if we don't read all, it will be still be waiting for us */
        if (input_size > buffer_size) {
            input_size = buffer_size;
        }
        /* TODO: use playpump scatter-gather in a more sophisticated app to avoid this memcpy */
        BKNI_Memcpy(buffer, input_buffer, input_size);

        rc = b_playback_read_complete(p, input_size);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Playpump_WriteComplete(playpump, 0, input_size);
        BDBG_ASSERT(!rc);
    }
    return 0;
}
