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
 * $brcm_Workfile: audio_descriptors.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 1/26/12 11:32a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/audio/audio_descriptors.c $
 * 
 * 9   1/26/12 11:32a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 8   6/20/11 6:57p jgarrett
 * SW7425-406: Adding AD support via DSP mixer
 * 
 * 7   1/7/11 3:18p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 6   3/22/10 2:27a randyjew
 * SW7468-143: Support for boards without component and/or composite
 * 
 * 5   4/1/09 1:24p jgarrett
 * PR 53592: Changing default video format to 50Hz for dynamic RTS
 * 
 * 4   2/4/09 3:51p erickson
 * PR51841: refactor
 *
 * 3   10/4/08 6:21a jgarrett
 * PR 46799: Adding passthrough support
 *
 * 2   7/24/08 10:33a jgarrett
 * PR 39452: Removing DTV-specific headers
 *
 * 1   7/23/08 3:57p jgarrett
 * PR 39452: Adding audio descriptors example
 *
 * 14   7/14/08 3:29p erickson
 * PR44792: add sample shutdown code
 *
 * 13   7/7/08 2:40p erickson
 * PR44619: remove duplicate NEXUS_PanelOutput_Open calls.
 *
 * 12   6/20/08 5:48p vsilyaev
 * PR 40921: Use NTSC resolution if no component output
 *
 * 11   6/19/08 2:44p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 10   6/19/08 10:46a erickson
 * PR42678: switch to output_type=panel|component
 *
 * 9   6/7/08 1:38p jgarrett
 * PR 43318: Allowing display with no composite outputs
 *
 * 8   5/21/08 10:33a erickson
 * PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
 *
 * 7   4/23/08 11:35a erickson
 * PR35457: rearrange example for simpler explanation
 *
 * 6   4/1/08 11:39a erickson
 * PR38442: use is_panel_output() for consistent DVO output selection
 *
 * 5   2/25/08 9:28a jgarrett
 * PR 39435: fixing includes
 *
 * 4   1/24/08 12:52p vsilyaev
 * PR 38682: fixed return type of NEXUS_Platform_GetStreamerInputBand
 *
 * 3   1/23/08 12:38p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
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
#include "nexus_audio_mixer.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(audio_descriptors);

#define USE_SPDIF 0
#define USE_COMPRESSED_SPDIF 0

/* the following define the input and its characteristics -- these will vary by input */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eMpeg
#define VIDEO_PID 501
#define AUDIO_PID 502
#define DESC_PID  504

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel, descriptionPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle pcmDecoder, descriptorDecoder;
#if USE_COMPRESSED_SPDIF
    NEXUS_AudioDecoderHandle compressedDecoder;
#endif
    NEXUS_AudioDecoderStartSettings audioProgram, audioDescription;
    NEXUS_AudioDecoderOpenSettings audioOpenSettings;
    NEXUS_AudioDecoderSettings audioSettings;
    NEXUS_AudioMixerHandle mixer;
    NEXUS_AudioMixerSettings mixerSettings;
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
    descriptionPidChannel = NEXUS_PidChannel_Open(parserBand, DESC_PID, NULL);

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
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioDescription);
    audioDescription.codec = AUDIO_CODEC;
    audioDescription.pidChannel = descriptionPidChannel;
    audioDescription.stcChannel = stcChannel;

    /* Bring up the primary audio decoder */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);

    /* Open audio descriptor decoder */
    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioOpenSettings);
    audioOpenSettings.type = NEXUS_AudioDecoderType_eAudioDescriptor;
    descriptorDecoder = NEXUS_AudioDecoder_Open(1, &audioOpenSettings);

#if USE_COMPRESSED_SPDIF
    /* Open a decoder for compressed passthrough */
    compressedDecoder = NEXUS_AudioDecoder_Open(2, NULL);
#endif

    /* Link the descriptor and primary decoders */
    NEXUS_AudioDecoder_GetSettings(pcmDecoder, &audioSettings);
    audioSettings.descriptionDecoder = descriptorDecoder;
    NEXUS_AudioDecoder_SetSettings(pcmDecoder, &audioSettings);

    /* Open mixer to mix the description and primary audio */
    NEXUS_AudioMixer_GetDefaultSettings(&mixerSettings);
#if NEXUS_AUDIO_MODULE_FAMILY == 7422
    mixerSettings.mixUsingDsp = true;
#endif    
    mixer = NEXUS_AudioMixer_Open(&mixerSettings);

    /* Add both decoders to the mixer */
    NEXUS_AudioMixer_AddInput(mixer, NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioMixer_AddInput(mixer, NEXUS_AudioDecoder_GetConnector(descriptorDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

#if NEXUS_AUDIO_MODULE_FAMILY == 7422
    NEXUS_AudioMixer_GetSettings(mixer, &mixerSettings);
    mixerSettings.master = NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
    NEXUS_AudioMixer_SetSettings(mixer, &mixerSettings);
#endif    

    /* Add DAC and HDMI to the mixer output */
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioMixer_GetConnector(mixer));
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioMixer_GetConnector(mixer));
#endif

#if USE_COMPRESSED_SPDIF
    /* Use a passthrough decoder for spdif */
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
#elif USE_SPDIF
    /* Attach SPDIF to the mixed output */
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioMixer_GetConnector(mixer));
#endif

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

    BDBG_WRN(("Starting PCM"));
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    BDBG_WRN(("Starting Descriptor"));
    audioDescription.secondaryDecoder = true;
    NEXUS_AudioDecoder_Start(descriptorDecoder, &audioDescription);
#if USE_COMPRESSED_SPDIF
    NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
#endif

#if 1
    /* Print status while decoding */
    for (;;) {
        NEXUS_VideoDecoderStatus status;
        NEXUS_AudioDecoderStatus audioStatus;
        uint32_t stc;

        NEXUS_VideoDecoder_GetStatus(vdecode, &status);
        NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);
        printf("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference, status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0);
        NEXUS_AudioDecoder_GetStatus(pcmDecoder, &audioStatus);
        printf("audio            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
#if USE_COMPRESSED_SPDIF
        NEXUS_AudioDecoder_GetStatus(compressedDecoder, &audioStatus);
        if ( audioStatus.started )
        {
            printf("compressed        pts %#x, stc %#x (diff %d) fifo=%d%%\n",
                audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
        }
#endif
        NEXUS_AudioDecoder_GetStatus(descriptorDecoder, &audioStatus);
        if ( audioStatus.started )
        {
            printf("descriptor        pts %#x, stc %#x (diff %d) fifo=%d%%\n",
                audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
        }
        BKNI_Sleep(1000);
    }
#else
    printf("Press ENTER to stop decode\n");
    getchar();

    /* example shutdown */
    NEXUS_AudioDecoder_Close(pcmDecoder);
#if USE_COMPRESSED_SPDIF
    NEXUS_AudioDecoder_Close(compressedDecoder);
#endif
    NEXUS_AudioDecoder_Close(descriptionDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}
