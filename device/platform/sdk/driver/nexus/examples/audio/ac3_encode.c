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
 * $brcm_Workfile: ac3_encode.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 1/26/12 11:31a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/audio/ac3_encode.c $
 * 
 * 7   1/26/12 11:31a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 6   11/28/11 5:14p jgarrett
 * SW7411-2108: Setting copyright indicator (CP=0) on SPDIF
 * 
 * 5   1/7/11 3:17p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 4   3/22/10 2:27a randyjew
 * SW7468-143: Support for boards without component and/or composite
 * 
 * 3   2/4/09 3:51p erickson
 * PR51841: refactor
 *
 * 2   10/4/08 9:57a jgarrett
 * PR 47509: Adding Ac3Encode
 *
******************************************************************************/
/* Nexus example app: single live a/v decode from an input band */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_ac3_encode.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

/* the following define the input and its characteristics -- these will vary by input */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eH264
#define AUDIO_CODEC NEXUS_AudioCodec_eAacPlusLoas
#define VIDEO_PID 33
#define AUDIO_PID 32

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderOpenSettings audioOpenSettings;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_Ac3EncodeHandle ac3Encode;
    NEXUS_SpdifOutputSettings spdifSettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, AUDIO_PID, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = AUDIO_CODEC;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioOpenSettings);
    audioOpenSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_e5_1;
    audioDecoder = NEXUS_AudioDecoder_Open(0, &audioOpenSettings);
    ac3Encode = NEXUS_Ac3Encode_Open(NULL);

    /* Send stereo output to DAC and HDMI */
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif
    /* Send multichannel output to AC3 encoder */
    NEXUS_Ac3Encode_AddInput(ac3Encode,
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eMultichannel));
    /* Send compressed AC3 data to SPDIF */
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                               NEXUS_Ac3Encode_GetConnector(ac3Encode));

    /* Mark the SPDIF data as copyrighted */
    NEXUS_SpdifOutput_GetSettings(platformConfig.outputs.spdif[0], &spdifSettings);
    spdifSettings.channelStatusInfo.swCopyRight = true;
    NEXUS_SpdifOutput_SetSettings(platformConfig.outputs.spdif[0], &spdifSettings);

    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
		}
    }
#endif

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

    /* Print status while decoding */
    for (;;) {
        NEXUS_VideoDecoderStatus status;
        NEXUS_AudioDecoderStatus audioStatus;
        uint32_t stc;

        NEXUS_VideoDecoder_GetStatus(vdecode, &status);
        NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);
        printf("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference, status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0);
        NEXUS_AudioDecoder_GetStatus(audioDecoder, &audioStatus);
        printf("audio             pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
        BKNI_Sleep(1000);
    }

    return 0;
}
