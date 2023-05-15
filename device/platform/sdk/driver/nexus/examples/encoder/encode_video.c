/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: encode_video.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 6/6/12 11:39a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/encoder/encode_video.c $
 * 
 * 6   6/6/12 11:39a hongtaoz
 * SW7425-3167: removed trackInput from NEXUS_VideoEncoderStreamStructure;
 * 
 * 5   4/17/12 3:34p vsilyaev
 * SW7425-2875: Use originalPts instead of originalPTS
 * 
 * 4   4/2/12 2:50p vsilyaev
 * SW7425-1473 : Merged raaga encoder support
 * 
 * SW7425-1473/1   3/29/12 2:32p gmohile
 * SW7425-1473 : Add raaga encoder examples
 * 
 * 3   9/29/11 12:39p erickson
 * SW7425-1320: rely on NEXUS_ENCODER_DISPLAY_IDX defined in
 *  nexus_platform_features.h
 * 
 * 2   8/17/11 3:53p hongtaoz
 * SW7425-651: added new API to configure video encode delay;
 * 
 * 10   3/3/11 11:22a vsilyaev
 * SW7425-137: Removed target and min bitrates
 * 
 * SW7425-137/1   3/2/11 11:43p nilesh
 * SW7425-137: Removed uiMin and uiTarget bitrate settings
 * 
 * 9   2/25/11 3:08p vsilyaev
 * SW7425-39, SW7425-40: Merged examples
 * 
 * SW7425_bringup/7   2/23/11 1:57p hongtaoz
 * SW7425-40: added end-to-end delay setting;
 * 
 * SW7425_bringup/6   2/8/11 10:40p hongtaoz
 * SW742-40: adapted to the new APIs;
 * 
 * SW7425_bringup/5   2/7/11 9:24p hongtaoz
 * SW7425-40: adapted to the new API;
 * 
 * SW7425_bringup/4   1/28/11 6:09p hongtaoz
 * SW7425-40: added STC channel config for video encoder;
 * 
 * SW7425_bringup/3   1/24/11 11:18a hongtaoz
 * SW7425-40: use 24p test stream; add video descriptors log file;
 * 
 * 7   1/17/11 4:46p vsilyaev
 * SW7425-65: Added StcChannel to the videoEncoder
 * 
 * 6   1/4/11 12:56p vsilyaev
 * SW7425-40: Fixed compile issue on non-encoder platforms
 * 
 * 5   12/28/10 6:37p vsilyaev
 * SW7425-40: Set defaul profile and level
 * 
 * 4   12/27/10 6:02p vsilyaev
 * SW7425-40: Removed primary display
 * 
 * 3   12/16/10 5:17p vsilyaev
 * SW7425-40: Updated video encoder configuration
 * 
 * 2   12/16/10 12:20p vsilyaev
 * SW7425-39, SW7425-40: Merged into mainline
 * 
 * SW7425_bringup/2   12/15/10 10:51p hongtaoz
 * SW7425-39, SW7425-40: updated examples with missed calls and re-ordered
 *  encoder/display open sequence;
 * 
 * SW7425_bringup/1   12/15/10 5:34p hongtaoz
 * SW7425-39, SW7425-40: updated default encoder display index and encoder
 *  settings for 7425 bringup;
 * 
 * 1   12/13/10 3:37p vsilyaev
 * SW7425-40: Video encoder examples
 * 
 *
 *****************************************************************************/
/* Nexus example app: playback and decode */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_video_adj.h"
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_VIDEO_ENCODER
#include "nexus_video_encoder_output.h"
#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_video_encoder.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"


BDBG_MODULE(encode_video);
int main(void)  {
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_VIDEO_ENCODER
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel, stcChannelEncoder;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;

    NEXUS_DisplayHandle displayTranscode;
    NEXUS_VideoWindowHandle windowTranscode;
    NEXUS_VideoEncoderHandle videoEncoder;
    NEXUS_VideoEncoderSettings videoEncoderConfig;
    NEXUS_VideoEncoderDelayRange videoDelay;
    NEXUS_VideoEncoderStartSettings videoEncoderStartConfig;
    NEXUS_VideoEncoderStatus videoEncoderStatus;

#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
    NEXUS_VideoWindowScalerSettings sclSettings;
    NEXUS_VideoWindowMadSettings madSettings;
    NEXUS_VideoWindowSettings windowSettings;
#endif

    FILE *fout;
    size_t bytes;


    NEXUS_DisplaySettings displaySettings;
    const char *fname = "videos/avatar_AVC_15M.ts";
    FILE *fdesc;

    fdesc = fopen("videos/video_desc.txt","w");
    fprintf(fdesc, "flags(h) origPts(h) pts(Lh)     escr(h) ticksPerBit(u) shr(d) offset(h) length(h)\n");

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

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);


    /* bring up decoder and connect to local display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

    /* NOTE: must open video encoder before display; otherwise open will init ViCE2 core
     * which might cause encoder display GISB error since encoder display would
     * trigger RDC to program mailbox registers in ViCE2;
     */
    videoEncoder = NEXUS_VideoEncoder_Open(0, NULL);
    assert(videoEncoder);

    /* Bring up video encoder display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
    displaySettings.format = NEXUS_VideoFormat_e720p;
    displayTranscode = NEXUS_Display_Open(0, &displaySettings);    
#else
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eEncoder;
    displaySettings.format = NEXUS_VideoFormat_e720p24hz;/* bring up 480p first */
    displayTranscode = NEXUS_Display_Open(NEXUS_ENCODER_DISPLAY_IDX, &displaySettings);
#endif
    assert(displayTranscode);


    windowTranscode = NEXUS_VideoWindow_Open(displayTranscode, 0);
    assert(windowTranscode);

#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
    NEXUS_VideoWindow_GetSettings(windowTranscode, &windowSettings);    
    windowSettings.position.width = 416;
    windowSettings.position.height = 224;
    windowSettings.pixelFormat = NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08;    
    windowSettings.visible = false;
    NEXUS_VideoWindow_SetSettings(windowTranscode, &windowSettings);
    
    NEXUS_VideoWindow_GetScalerSettings(windowTranscode, &sclSettings);
    sclSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
    sclSettings.bandwidthEquationParams.delta = 1000000;
    NEXUS_VideoWindow_SetScalerSettings(windowTranscode, &sclSettings);
    
    NEXUS_VideoWindow_GetMadSettings(windowTranscode, &madSettings);
    madSettings.deinterlace = true;
    madSettings.enable22Pulldown = true;
    madSettings.enable32Pulldown = true;
    NEXUS_VideoWindow_SetMadSettings(windowTranscode, &madSettings);
#endif

    /* connect same decoder to encoder display 
     * This simul mode is for video encoder bringup only; audio path may have limitation
     * for simul display+transcode mode;
     */
    NEXUS_VideoWindow_AddInput(windowTranscode, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* Open the video pid channel */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH264; /* must be told codec for correct handling */
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x101, &playbackPidSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eH264;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
 
    NEXUS_VideoEncoder_GetSettings(videoEncoder, &videoEncoderConfig);
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
    videoEncoderConfig.frameRate = NEXUS_VideoFrameRate_e29_97;
    videoEncoderConfig.bitrateMax = 400*1000;
#else 
    videoEncoderConfig.variableFrameRate = true;
    videoEncoderConfig.frameRate = NEXUS_VideoFrameRate_e24;
    videoEncoderConfig.bitrateMax = 6*1000*1000;
    videoEncoderConfig.streamStructure.framesP = 29; /* IPP GOP size = 30 */
    videoEncoderConfig.streamStructure.framesB = 0;
#endif
    /* Start decoders */
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);

    /* Start playback */
    NEXUS_Playback_Start(playback, file, NULL);

    BKNI_Sleep(1000);

    /* encoder requires different STC broadcast mode from decoder */
    NEXUS_StcChannel_GetDefaultSettings(1, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
    stcChannelEncoder = NEXUS_StcChannel_Open(1, &stcSettings);

    NEXUS_VideoEncoder_GetDefaultStartSettings(&videoEncoderStartConfig);
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
    videoEncoderStartConfig.input = displayTranscode;
    videoEncoderStartConfig.codec = NEXUS_VideoCodec_eH264;
    videoEncoderStartConfig.profile = NEXUS_VideoCodecProfile_eBaseline;
    videoEncoderStartConfig.level = NEXUS_VideoCodecLevel_e30;         
    videoEncoderStartConfig.bounds.inputDimension.max.width = 416;
    videoEncoderStartConfig.bounds.inputDimension.max.height = 224;
    videoEncoderStartConfig.stcChannel = stcChannelEncoder;
#else
    videoEncoderStartConfig.codec = NEXUS_VideoCodec_eH264;
    videoEncoderStartConfig.profile = NEXUS_VideoCodecProfile_eBaseline;
    videoEncoderStartConfig.level = NEXUS_VideoCodecLevel_e31;
    videoEncoderStartConfig.input = displayTranscode;
    videoEncoderStartConfig.stcChannel = stcChannelEncoder;

	/******************************************
	 * add configurable delay to video path 
	 */
	/* NOTE: ITFP is encoder feature to detect and lock on 3:2/2:2 cadence in the video content to help 
	 * efficient coding for interlaced formats; disabling ITFP will impact the bit efficiency but reduce the encode delay. */
	videoEncoderConfig.enableFieldPairing = true;

	/* 0 to use default 750ms rate buffer delay; TODO: allow user to adjust it to lower encode delay at cost of quality reduction! */
	videoEncoderStartConfig.rateBufferDelay = 0;

	/* to allow 23.976p passthru; TODO: allow user to configure minimum framerate to achieve lower delay!
	 * Note: lower minimum framerate means longer encode delay */
	videoEncoderStartConfig.bounds.inputFrameRate.min = NEXUS_VideoFrameRate_e23_976;

	/* to allow 24 ~ 60p dynamic frame rate coding TODO: allow user to config higher minimum frame rate for lower delay! */
	videoEncoderStartConfig.bounds.outputFrameRate.min = NEXUS_VideoFrameRate_e23_976;
	videoEncoderStartConfig.bounds.outputFrameRate.max = NEXUS_VideoFrameRate_e60;

	/* max encode size allows 1080p encode; TODO: allow user to choose lower max resolution for lower encode delay */
	videoEncoderStartConfig.bounds.inputDimension.max.width = 1920;
	videoEncoderStartConfig.bounds.inputDimension.max.height = 1088;
#endif

    /* NOTE: video encoder delay is in 27MHz ticks */
    NEXUS_VideoEncoder_GetDelayRange(videoEncoder, &videoEncoderConfig, &videoEncoderStartConfig, &videoDelay);
    BDBG_WRN(("\n\tVideo encoder end-to-end delay = [%u ~ %u] ms", videoDelay.min/27000, videoDelay.max/27000));
    videoEncoderConfig.encoderDelay = videoDelay.min;

    /* note the Dee is set by SetSettings */
    NEXUS_VideoEncoder_SetSettings(videoEncoder, &videoEncoderConfig);
    NEXUS_VideoEncoder_Start(videoEncoder, &videoEncoderStartConfig);
    NEXUS_VideoEncoder_GetStatus(videoEncoder, &videoEncoderStatus);

    fout = fopen("videos/encoder.mpg","wb");
    assert(fout);

    for(bytes=0;;) {
        size_t size[2];
        const NEXUS_VideoEncoderDescriptor *desc[2];
        unsigned i,j;
        unsigned descs;

        
        NEXUS_VideoEncoder_GetBuffer(videoEncoder, &desc[0], &size[0], &desc[1], &size[1]);
        if(size[0]==0 && size[1]==0) {
            NEXUS_VideoDecoderStatus vstatus;

            NEXUS_VideoDecoder_GetStatus(videoDecoder, &vstatus);
            fflush(fout);
            fprintf(stderr, "written %u bytes.... decode:%u\t\r", bytes, vstatus.pts);
            BKNI_Sleep(30);
            continue;
        }
        for(descs=0,j=0;j<2;j++) {
            descs+=size[j];
            for(i=0;i<size[j];i++) {
                if(desc[j][i].length > 0)
                {
                fwrite((const uint8_t *)videoEncoderStatus.bufferBase + desc[j][i].offset, desc[j][i].length, 1, fout);
                fprintf(fdesc, "%8x %8x   %x%08x %08x     %5u   %5d   %8x %8x\n", desc[j][i].flags, desc[j][i].originalPts, 
                    (uint32_t)(desc[j][i].pts>>32), (uint32_t)(desc[j][i].pts & 0xffffffff), desc[j][i].escr, 
                    desc[j][i].ticksPerBit, desc[j][i].shr, desc[j][i].offset, desc[j][i].length);
                bytes+= desc[j][i].length;
                if(desc[j][i].length > 0x100000)
                {
                    BDBG_ERR(("++++ desc[%d][%d] length = 0x%x, offset=0x%x", j,i, desc[j][i].length, desc[j][i].offset));
                }
                   
                }
            }
        }
        NEXUS_VideoEncoder_ReadComplete(videoEncoder, descs);
    }
    fclose(fout);

    /* Bring down system */
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_StcChannel_Close(stcChannel);

    NEXUS_Display_Close(displayTranscode);
    NEXUS_VideoEncoder_Close(videoEncoder);
    NEXUS_StcChannel_Close(stcChannelEncoder);
    
    NEXUS_Platform_Uninit();

#endif
    return 0;
}


