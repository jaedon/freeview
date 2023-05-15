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
 * $brcm_Workfile: pacing_playpump.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 11/2/09 11:17a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/pacing_playpump.c $
 * 
 * Hydra_Software_Devel/5   11/2/09 11:17a erickson
 * SW7405-3315: remove playbackOverride. use NEXUS_StcChannelMode instead.
 * 
 * Hydra_Software_Devel/4   5/4/09 5:12p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/3   12/16/08 12:41p erickson
 * PR42679: use ePcr TSM for pacing
 *
 * Hydra_Software_Devel/2   11/4/08 4:38p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   10/27/08 4:14p erickson
 * PR42679: added pacing testse
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_playpump.h"
#include "nexus_display.h"
#include "nexus_video_decoder.h"
#include "nexus_video_window.h"
#include "nexus_stc_channel.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif


#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

void print_status(NEXUS_PlaypumpHandle playpump, NEXUS_StcChannelHandle stcChannel, NEXUS_VideoDecoderHandle vdecode)
{
    NEXUS_PlaypumpStatus status;
    NEXUS_VideoDecoderStatus vstatus;
    uint32_t stc;

    NEXUS_Playpump_GetStatus(playpump, &status);
    NEXUS_VideoDecoder_GetStatus(vdecode, &vstatus);
    NEXUS_StcChannel_GetStc(stcChannel, &stc);
    printf("state playpump=%d%%, video=%d%%/pts=%#x/pics=%d, stc=%#x\n",
        status.fifoSize?status.fifoDepth*100/status.fifoSize:0,
        vstatus.fifoSize?vstatus.fifoDepth*100/vstatus.fifoSize:0, vstatus.pts, vstatus.queueDepth,
        stc);
}

int main(void) {
    FILE *file;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    const char fname[] = "videos/pacing_stream.mpg";
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_Error rc;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    unsigned i;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

    playpump = NEXUS_Playpump_Open(0, NULL);
    file = fopen(fname, "rb");
    if (!file) {
        printf("Unable to open file %s\n", fname);
        return -1;
    }

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eMod300;
    playpumpSettings.timestamp.pacing = true;
    rc = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Playpump_Start(playpump);
    BDBG_ASSERT(!rc);

#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, NULL);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);
    rc = NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    BDBG_ASSERT(!rc);
#else
    display = NEXUS_Display_Open(0, NULL);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
    vdecode = NEXUS_VideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
    BDBG_ASSERT(!rc);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0x11, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr;
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel;
    videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    BDBG_ASSERT(!rc);

    while (1) {
        void *buffer;
        size_t buffer_size;
        int n;
        NEXUS_Error rc;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
            break;
        if (buffer_size == 0) {
            BKNI_WaitForEvent(event, 1000);
            print_status(playpump, videoProgram.stcChannel, vdecode);
            continue;
        }

        /* The first call to get_buffer will return the entire playback buffer.
        If we use it, we're going to have to wait until the descriptor is complete,
        and then we're going to underflow. So use a max size. */
#define MAX_READ (188*1024)
        if (buffer_size > MAX_READ)
            buffer_size = MAX_READ;

        n = fread(buffer, 1, buffer_size, file);
        BDBG_ASSERT(n >= 0);
        if (n == 0) {
            break;
        }
        else {
            rc = NEXUS_Playpump_ReadComplete(playpump, 0, n);
            BDBG_ASSERT(!rc);
        }
        /* TODO: devise way to verify pacing is working correctly. */
    }

    /* simple wait for end of decode */
    for (i=0;i<30;i++) {
        print_status(playpump, videoProgram.stcChannel, vdecode);
        BKNI_Sleep(1000);
    }

    NEXUS_VideoDecoder_Stop(vdecode);
    NEXUS_StcChannel_Close(videoProgram.stcChannel);
    NEXUS_Playpump_ClosePidChannel(playpump, videoProgram.pidChannel);
    NEXUS_Playpump_Close(playpump);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(vdecode);
    BKNI_DestroyEvent(event);
    NEXUS_Platform_Uninit();

    return 0;
}
