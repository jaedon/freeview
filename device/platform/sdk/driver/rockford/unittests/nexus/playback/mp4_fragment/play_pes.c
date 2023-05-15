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
 * $brcm_Workfile: play_pes.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 5/26/11 4:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/mp4_fragment/play_pes.c $
 * 
 * Hydra_Software_Devel/6   5/26/11 4:25p vsilyaev
 * SW7425-615: Added H264 stream
 * 
 * Hydra_Software_Devel/5   5/20/11 12:48p vsilyaev
 * SW7425-615: Really disabled scaler factor rounding
 * 
 * Hydra_Software_Devel/4   5/19/11 7:20p vsilyaev
 * SW7425-615: Disable 480p interlaced scanout from the decoder and use
 * SCL->CAP path
 * 
 * Hydra_Software_Devel/3   5/19/11 6:44p vsilyaev
 * SW7425-615: Disable deinterlacer
 * 
 * Hydra_Software_Devel/2   5/19/11 6:33p vsilyaev
 * SW7425-465: Set minimumSourceFormat and bandwidthEquationParams.delta
 * 
 * Hydra_Software_Devel/1   5/19/11 6:07p vsilyaev
 * SW7425-615: Helper application to debug transitions betwen different
 * source resolutions
 * 
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_audio_dac.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_video_adj.h"
#include "nexus_playback.h"
#include "nexus_file.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

static void print_status(NEXUS_VideoDecoderHandle videoDecoder)
{
    NEXUS_Error rc;
    NEXUS_VideoDecoderStatus videoStatus;
    size_t old_width=0;

    for(;;) {
        rc=NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
        if(old_width!=videoStatus.source.width) {
            old_width = videoStatus.source.width;
            fprintf(stderr, "%-3u video:%ux%u\n", (videoStatus.pts+45000/2)/45000, videoStatus.source.width, videoStatus.source.height);
        }
        BKNI_Sleep(10);
    }
    return;
}

int main(int argc, const char *argv[])
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_VideoWindowMadSettings madSettings;
    NEXUS_VideoWindowScalerSettings scalerSettings;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_FilePlayHandle file;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PlaybackSettings playbackSettings;
    const char *fname = "big_buck_1080p.pes";
    NEXUS_VideoCodec codec = NEXUS_VideoCodec_eVc1;

    if(argc>1) {
        fname = argv[1];
    }
    if(argc>2) {
        codec = atol(argv[2]);
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    /* displaySettings.format = NEXUS_VideoFormat_eNtsc; */
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if(platformConfig.outputs.component[0]){
        NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    }
#endif 
    if(displaySettings.format == NEXUS_VideoFormat_eNtsc) {
#if NEXUS_NUM_COMPOSITE_OUTPUTS
        NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS    
        if (platformConfig.outputs.svideo[0]) {
            NEXUS_Display_AddOutput(display, NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[0]));
        }
#endif    
    }
#endif /* NEXUS_DTV_PLATFORM */

    window = NEXUS_VideoWindow_Open(display, 0);
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    windowSettings.scaleFactorRounding.enabled = false;
    windowSettings.minimumSourceFormat = NEXUS_VideoFormat_e1080p;
    NEXUS_VideoWindow_SetSettings(window, &windowSettings);

    NEXUS_VideoWindow_GetScalerSettings(window, &scalerSettings);
    scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
    scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
    NEXUS_VideoWindow_SetScalerSettings(window, &scalerSettings);

    NEXUS_VideoWindow_GetMadSettings(window, &madSettings);
    madSettings.deinterlace = false;
    NEXUS_VideoWindow_SetMadSettings(window, &madSettings);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    file = NEXUS_FilePlay_OpenPosix(fname, NULL);
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }
    playpump = NEXUS_Playpump_Open(0, NULL);
    playback = NEXUS_Playback_Create();
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eMpeg2Pes;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = codec; /* must be told codec for correct handling */
    playbackPidSettings.pidTypeSettings.video.index = false;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0xE0, &playbackPidSettings);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = codec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    videoProgram.progressiveOverrideMode = NEXUS_VideoDecoderProgressiveOverrideMode_eDisable;
    videoProgram.timestampMode = codec == NEXUS_VideoCodec_eVc1?NEXUS_VideoDecoderTimestampMode_eDecode:NEXUS_VideoDecoderTimestampMode_eDisplay;

    BKNI_Sleep(2*1000);
    /* Start decoders */
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_Playback_Start(playback, file, NULL);
    print_status(videoDecoder);

    return 0;

}

