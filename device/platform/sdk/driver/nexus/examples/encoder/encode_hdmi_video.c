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
 * $brcm_Workfile: encode_hdmi_video.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 8/31/12 1:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/encoder/encode_hdmi_video.c $
 * 
 * 4   8/31/12 1:13p hongtaoz
 * SW7429-254: fixed encoder examples compile error for non-ViCE2 SoCs;
 * 
 * 3   9/29/11 12:39p erickson
 * SW7425-1320: rely on NEXUS_ENCODER_DISPLAY_IDX defined in
 *  nexus_platform_features.h
 * 
 * 2   8/17/11 3:53p hongtaoz
 * SW7425-651: added new API to configure video encode delay;
 * 
 * 6   3/28/11 12:08p vsilyaev
 * SW7425-94, SW7425-102: added transcoder AV sync end-to-end delay
 *  settings
 * 
 * 5   3/3/11 11:22a vsilyaev
 * SW7425-137: Removed target and min bitrates
 * 
 * SW7425-137/1   3/2/11 11:43p nilesh
 * SW7425-137: Removed uiMin and uiTarget bitrate settings
 * 
 * 4   2/9/11 2:12p vsilyaev
 * SW7425-41: Merged updated examples
 * 
 * SW7425_bringup/2   2/8/11 5:23p hongtaoz
 * SW7425-39, SW7425-40: adapted to the new API changes;
 * 
 * SW7425_bringup/1   1/2/11 5:24p hongtaoz
 * SW7425-40: added hdmi video encode test;
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
#if NEXUS_HAS_VIDEO_ENCODER
#include "nexus_video_encoder_output.h"
#include "nexus_file.h"
#include "nexus_video_encoder.h"
#endif
#if NEXUS_NUM_HDMI_INPUTS
#include "nexus_hdmi_input.h"
#endif

#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

#ifndef NEXUS_ENCODER_DISPLAY_IDX
#define NEXUS_ENCODER_DISPLAY_IDX   3
#endif

int main(void)  {
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_VIDEO_ENCODER
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;

    NEXUS_DisplayHandle displayTranscode;
    NEXUS_VideoWindowHandle windowTranscode;
    NEXUS_VideoEncoderHandle videoEncoder;
    NEXUS_VideoEncoderSettings videoEncoderConfig;
    NEXUS_VideoEncoderStartSettings videoEncoderStartConfig;
    NEXUS_VideoEncoderDelayRange delayRange;
    NEXUS_VideoEncoderStatus videoEncoderStatus;
    FILE *fout;
    size_t bytes;
    
#if NEXUS_NUM_HDMI_INPUTS
    NEXUS_HdmiInputHandle hdmiInput;
    NEXUS_HdmiInputSettings hdmiInputSettings;
#endif

    NEXUS_DisplaySettings displaySettings;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

#if NEXUS_NUM_HDMI_INPUTS
    NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
    hdmiInputSettings.timebase = NEXUS_Timebase_e0;
    hdmiInput = NEXUS_HdmiInput_Open(0, &hdmiInputSettings);
    if(!hdmiInput) {
        fprintf(stderr, "Can't get hdmi input\n");
        return -1;
    }
#endif

    /* NOTE: must open video encoder before display; otherwise open will init ViCE2 core
     * which might cause encoder display GISB error since encoder display would
     * trigger RDC to program mailbox registers in ViCE2;
     */
    videoEncoder = NEXUS_VideoEncoder_Open(0, NULL);
    assert(videoEncoder);

    /* Bring up video encoder display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eEncoder;
    displaySettings.format = NEXUS_VideoFormat_e720p24hz;/* bring up 480p first */
    displayTranscode = NEXUS_Display_Open(NEXUS_ENCODER_DISPLAY_IDX, &displaySettings);
    assert(displayTranscode);

    windowTranscode = NEXUS_VideoWindow_Open(displayTranscode, 0);
    assert(windowTranscode);

    /* connect same decoder to encoder display 
     * This simul mode is for video encoder bringup only; audio path may have limitation
     * for simul display+transcode mode;
     */
    NEXUS_VideoWindow_AddInput(windowTranscode, NEXUS_HdmiInput_GetVideoConnector(hdmiInput));


    NEXUS_VideoEncoder_GetSettings(videoEncoder, &videoEncoderConfig);
    videoEncoderConfig.variableFrameRate = true;
    videoEncoderConfig.bitrateMax = 6*1000*1000;
    videoEncoderConfig.streamStructure.framesP = 29;
    videoEncoderConfig.streamStructure.framesB = 0;

    NEXUS_VideoEncoder_GetDefaultStartSettings(&videoEncoderStartConfig);
    videoEncoderStartConfig.codec = NEXUS_VideoCodec_eH264;
    videoEncoderStartConfig.profile = NEXUS_VideoCodecProfile_eBaseline;
    videoEncoderStartConfig.level = NEXUS_VideoCodecLevel_e31;
    videoEncoderStartConfig.input = displayTranscode;
    videoEncoderStartConfig.stcChannel = stcChannel;

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

	NEXUS_VideoEncoder_GetDelayRange(videoEncoder, &videoEncoderConfig, &videoEncoderStartConfig, &delayRange);
	printf("\t encoder A2P delay = %u ms\n", delayRange.min/27000);
    videoEncoderConfig.encoderDelay = delayRange.min;
    NEXUS_VideoEncoder_SetSettings(videoEncoder, &videoEncoderConfig);
    NEXUS_VideoEncoder_Start(videoEncoder, &videoEncoderStartConfig);
    NEXUS_VideoEncoder_GetStatus(videoEncoder, &videoEncoderStatus);

    fout = fopen("videos/hdmi.mpg","wb");
    assert(fout);

    for(bytes=0;;) {
        size_t size[2];
        const NEXUS_VideoEncoderDescriptor *desc[2];
        unsigned i,j;
        unsigned descs;

        
        NEXUS_VideoEncoder_GetBuffer(videoEncoder, &desc[0], &size[0], &desc[1], &size[1]);
        if(size[0]==0 && size[1]==0) {
            fflush(fout);
            fprintf(stderr, "written %u bytes....\t\r", bytes);
            BKNI_Sleep(30);
            continue;
        }
        for(descs=0,j=0;j<2;j++) {
            descs+=size[j];
            for(i=0;i<size[j];i++) {
                fwrite((const uint8_t *)videoEncoderStatus.bufferBase + desc[j][i].offset, desc[j][i].length, 1, fout);
                bytes+= desc[j][i].length;
            }
        }
        NEXUS_VideoEncoder_ReadComplete(videoEncoder, descs);
    }
    fclose(fout);

    /* Bring down system */
    NEXUS_VideoInput_Shutdown(NEXUS_HdmiInput_GetVideoConnector(hdmiInput));
    NEXUS_StcChannel_Close(stcChannel);

    NEXUS_Display_Close(displayTranscode);
    NEXUS_VideoEncoder_Close(videoEncoder);
    
    NEXUS_Platform_Uninit();

#endif
    return 0;
}


