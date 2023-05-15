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
 * $brcm_Workfile: decoder_trick.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 5/26/10 1:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/decoder_trick.c $
 * 
 * Hydra_Software_Devel/3   5/26/10 1:03p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/2   12/8/08 2:39p erickson
 * PR49361: added status, removed unused audio code
 *
 * Hydra_Software_Devel/1   10/28/08 4:50p erickson
 * PR48406: added decoder_trick test
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_playback.h"
#include "nexus_file.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

void print_status(NEXUS_VideoDecoderHandle videoDecoder)
{
    NEXUS_VideoDecoderStatus status;
    NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
    printf("status %d decoded, %d display, %d decode drops, %d received\n",
        status.numDecoded,
        status.numDisplayed,
        status.numDecodeDrops,
        status.numPicturesReceived);
}

int main(void) {
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
    NEXUS_Error rc;
    NEXUS_VideoDecoderTrickState trick;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    file = NEXUS_FilePlay_OpenPosix(fname, NULL);
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x21, NULL);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    display = NEXUS_Display_Open(0, NULL);
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

    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* Start decoders */
    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);

    /* Start playback */
    printf("Normal play\n");
    NEXUS_Playback_Start(playback, file, NULL);
    BKNI_Sleep(5000);
    print_status(videoDecoder);

    printf("Decode I\n");
    NEXUS_VideoDecoder_GetTrickState(videoDecoder, &trick);
    trick.decodeMode = NEXUS_VideoDecoderDecodeMode_eI;
    trick.tsmEnabled = false; /* required because decoder is intentionally not keeping up with STC. */
    rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trick);
    BDBG_ASSERT(!rc);
    BKNI_Sleep(5000);
    print_status(videoDecoder);

    printf("Decode IP\n");
    NEXUS_VideoDecoder_GetTrickState(videoDecoder, &trick);
    trick.decodeMode = NEXUS_VideoDecoderDecodeMode_eIP;
    trick.tsmEnabled = false; /* required because decoder is intentionally not keeping up with STC. */
    rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trick);
    BDBG_ASSERT(!rc);
    BKNI_Sleep(5000);
    print_status(videoDecoder);

    printf("Decode All\n");
    NEXUS_VideoDecoder_GetTrickState(videoDecoder, &trick);
    trick.decodeMode = NEXUS_VideoDecoderDecodeMode_eAll;
    trick.tsmEnabled = false; /* required because decoder is intentionally not keeping up with STC. */
    rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trick);
    BDBG_ASSERT(!rc);
    BKNI_Sleep(5000);
    print_status(videoDecoder);

    printf("Decode 2x slow\n");
    NEXUS_VideoDecoder_GetTrickState(videoDecoder, &trick);
    trick.rate = NEXUS_NORMAL_DECODE_RATE / 2;
    trick.tsmEnabled = false; /* required because decoder is intentionally not keeping up with STC. */
    trick.topFieldOnly = true; /* required to prevent bounce on interlaced content */
    rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trick);
    BDBG_ASSERT(!rc);
    BKNI_Sleep(5000);
    print_status(videoDecoder);

    printf("Decode normal\n");
    NEXUS_VideoDecoder_GetTrickState(videoDecoder, &trick);
    trick.rate = NEXUS_NORMAL_DECODE_RATE;
    trick.tsmEnabled = true;
    trick.topFieldOnly = false;
    rc = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &trick);
    BDBG_ASSERT(!rc);
    BKNI_Sleep(5000);
    print_status(videoDecoder);

    /* Bring down system */
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    return 0;
}
