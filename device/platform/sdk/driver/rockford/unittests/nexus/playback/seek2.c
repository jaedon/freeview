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
 * $brcm_Workfile: seek2.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 4/6/09 2:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/seek2.c $
 * 
 * Hydra_Software_Devel/3   4/6/09 2:20p erickson
 * PR42679: update
 * 
 * Hydra_Software_Devel/2   2/13/09 11:53a erickson
 * PR47221: fix shutdown
 *
 * Hydra_Software_Devel/1   2/13/09 10:59a erickson
 * PR47221: added test app for starting playback from a previous location
 *
 * Hydra_Software_Devel/2   1/12/09 4:28p erickson
 * PR49930: fix Seek params
 *
 * Hydra_Software_Devel/1   12/8/08 11:18a erickson
 * PR49930: add seek test
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#include "nexus_video_window.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_playpump.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#endif
#include <stdlib.h>

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
BDBG_MODULE(seek2);

int main(void) {
#if NEXUS_HAS_PLAYBACK
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_Error rc;
    NEXUS_PlaybackPidChannelSettings playbackPidChannelSettings;
    unsigned loops = 100;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    playpump = NEXUS_Playpump_Open(0, NULL);
    stcChannel = NEXUS_StcChannel_Open(0, NULL);
    playback = NEXUS_Playback_Create();
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

    /* Bring up video display and outputs */
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
    BDBG_ASSERT(!rc);

    NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
    rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.stcChannel = stcChannel;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    file = NEXUS_FilePlay_OpenPosix("videos/cnnticker.mpg", "videos/cnnticker.nav");
    if (!file) {
        fprintf(stderr, "can't open file %s\n", "videos/cnnticker.mpg");
        goto error;
    }

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidChannelSettings);
    playbackPidChannelSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidChannelSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    playbackPidChannelSettings.pidTypeSettings.video.decoder = vdecode;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x21, &playbackPidChannelSettings);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    /* Start Decoders */
    rc = NEXUS_Playback_Start(playback, file, NULL);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    BDBG_ASSERT(!rc);

    while (--loops) {
        NEXUS_PlaybackStatus status;
        unsigned long currentpos;

        /* playing */
        BKNI_Sleep(rand() % 5000);

        NEXUS_Playback_Pause(playback);

        rc = NEXUS_Playback_GetStatus(playback, &status);
        BDBG_ASSERT(!rc);
        currentpos = status.position;
        printf("current pos %lu\n", currentpos);

        NEXUS_Playback_Stop(playback);
        NEXUS_VideoDecoder_Stop(vdecode);

        NEXUS_Playback_GetSettings(playback, &playbackSettings);
        playbackSettings.startPaused = true;
        rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Playback_Start(playback, file, NULL);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Playback_Seek(playback, currentpos);
        BDBG_ASSERT(!rc);

        rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Playback_Play(playback);
        BDBG_ASSERT(!rc);
    }

    NEXUS_Playback_Stop(playback);
    NEXUS_VideoDecoder_Stop(vdecode);

    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);

    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_Platform_Uninit();
    return 0;

error:
#endif
    return 1;
}
