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
 * $brcm_Workfile: playback_trick_loop.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 8/8/11 11:52a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/playback_trick_loop.c $
 * 
 * Hydra_Software_Devel/7   8/8/11 11:52a erickson
 * SWDEPRECATED-3044: speed up tests
 * 
 * Hydra_Software_Devel/6   10/21/10 1:22p gmohile
 * SW7125-480 : Check for video cdb depth
 * 
 * Hydra_Software_Devel/5   9/3/10 11:43a erickson
 * SWDEPRECATED-3044: clarify meaning of BDBG_ASSERT
 *
 * Hydra_Software_Devel/4   5/26/10 1:01p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 *
 * Hydra_Software_Devel/3   5/6/09 2:27p erickson
 * PR51821: make check status sleep much longer than wrap around time to
 * avoid false positives
 *
 * Hydra_Software_Devel/2   5/6/09 11:34a erickson
 * PR51821: reduce run time to 4 minutes for automation, allow option to
 * extend run time
 *
 * Hydra_Software_Devel/1   5/5/09 4:58p erickson
 * PR51821: create unittest for this PR
 *
 * Hydra_Software_Devel/3   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/2   1/30/09 10:31a erickson
 * PR42679: added cleanup
 *
 * Hydra_Software_Devel/1   1/20/09 1:22p erickson
 * PR42679: added playback_start_stop test
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_playback.h"
#include "nexus_file.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(playback_stop_start);

int main(int argc, char **argv) {
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    const char *fname = "videos/cnnticker.mpg";
    const char *indexname = "videos/cnnticker.nav";
    NEXUS_Error rc;
    NEXUS_PlaybackPidChannelSettings playbackPidCfg;
    unsigned loops = 45; /* loop for 3 minutes */
    unsigned i;
    NEXUS_PlaybackStatus lastStatus;
    NEXUS_PlaybackTrickModeSettings trick;
    NEXUS_VideoDecoderStatus lastVideostatus;

    if (argc > 1) loops = atoi(argv[1]);

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    file = NEXUS_FilePlay_OpenPosix(fname, indexname);
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidCfg);
    playbackPidCfg.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidCfg.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    playbackPidCfg.pidTypeSettings.video.decoder = videoDecoder;
    playbackPidCfg.pidTypeSettings.video.index = true;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x21, &playbackPidCfg);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    /* Bring up video display and outputs */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);

    /* Start playback */
    rc = NEXUS_Playback_Start(playback, file, NULL);
    BDBG_ASSERT(!rc);

    /* Start a 5x rewind */
    NEXUS_Playback_GetDefaultTrickModeSettings(&trick);
    trick.rate = -5 * NEXUS_NORMAL_PLAY_SPEED;
    rc = NEXUS_Playback_TrickMode(playback, &trick);
    BDBG_ASSERT(!rc);

    /* Then monitor it over time. Fail if it stalls. */
    rc = NEXUS_Playback_GetStatus(playback, &lastStatus);
    BDBG_ASSERT(!rc);
    BKNI_Sleep(4000);
    for (i=0;i<loops;i++) {
        NEXUS_PlaybackStatus status;
	NEXUS_VideoDecoderStatus vstatus;

	rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &vstatus);
	BDBG_ASSERT(!rc);
        rc = NEXUS_Playback_GetStatus(playback, &status);
        BDBG_ASSERT(!rc);
        printf("status %d bytes played, playback %d/%d, video %u/%u\n", (int)status.bytesPlayed, status.fifoDepth, status.fifoSize, vstatus.fifoDepth, vstatus.fifoSize);

        /* if ==, then playback (or something downstream, like a decoder) has stalled. this is a bug.
        if <, then playback is returning bad status. bytesPlayed should only increment after Start. */
        BDBG_ASSERT((status.bytesPlayed > lastStatus.bytesPlayed) || (vstatus.fifoDepth != lastVideostatus.fifoDepth));

        lastStatus = status;
	lastVideostatus = vstatus;
        BKNI_Sleep(4000);
    }

    printf("cleaning up\n");
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_Display_Close(display);
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_Playpump_Close(playpump);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_Platform_Uninit();

    return 0;
}
