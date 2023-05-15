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
 * $brcm_Workfile: ms11_dual_ac3plus.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 2/13/12 11:39a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/audio/ms11_dual_ac3plus.c $
 * 
 * 6   2/13/12 11:39a jgarrett
 * SW7420-2230: Fixing memory leak on shutdown
 * 
 * 5   1/26/12 11:32a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 4   1/12/12 3:08p jgarrett
 * SW7231-581: Re-enabling secondary decode, adjusting default PIDs to
 *  match dolby reference stream
 * 
 * 3   6/18/11 12:23p jgarrett
 * SW7425-406: Changing behavior of useDspMixer flag to allow set at Open
 *  time
 * 
 * 2   5/25/11 5:15p jgarrett
 * SW7425-408: Adding 7425 support
 * 
 * 1   8/26/10 4:21p jgarrett
 * SW7420-790: Merging MS11 examples to main
 * 
 * SW7420-790/2   6/28/10 10:47a jgarrett
 * SW7420-790: Adding additional post processing branches for MS11
 * 
 * SW7420-790/1   6/11/10 5:00p jgarrett
 * SW7420-790: Adding MS11 Support
 * 
******************************************************************************/

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
#include "nexus_audio_input.h"
#include "nexus_dolby_digital_reencode.h"
#include "nexus_dolby_volume.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(ms11_dual_ac3plus);

/* Uncomment this to enable SPDIF re-encoding to AC3 */
#if 0
#define USE_COMPRESSED_SPDIF 1
#endif

/* Uncomment this to use Dolby Volume 258 on the mixed data */
#if 0
#define USE_DV258 1
#endif

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, mainPidChannel, secondaryPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle mainDecoder, secondaryDecoder;
    NEXUS_AudioDecoderStartSettings mainProgram, secondaryProgram;
    NEXUS_AudioDecoderOpenSettings audioOpenSettings;
    NEXUS_AudioMixerSettings mixerSettings;
    NEXUS_AudioMixerHandle mixer;
    NEXUS_DolbyDigitalReencodeHandle ddre;
#if USE_DV258
    NEXUS_DolbyVolume258Handle dv258;
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
#if NEXUS_AUDIO_MODULE_FAMILY != 7422
    platformSettings.audioModuleSettings.maximumProcessingBranches = 2;
#endif
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 33, NULL);
    mainPidChannel = NEXUS_PidChannel_Open(parserBand, 37, NULL);
    secondaryPidChannel = NEXUS_PidChannel_Open(parserBand, 39, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&mainProgram);
    mainProgram.codec = NEXUS_AudioCodec_eAc3Plus;
    mainProgram.pidChannel = mainPidChannel;
    mainProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&secondaryProgram);
    secondaryProgram.codec = NEXUS_AudioCodec_eAc3Plus;
    secondaryProgram.pidChannel = secondaryPidChannel;
    secondaryProgram.stcChannel = stcChannel;
    secondaryProgram.secondaryDecoder = true;   /* Indicate this is a secondary channel for STC Channel/PCRlib functions */

    /* Bring up the primary audio decoder */
    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioOpenSettings);
    audioOpenSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_e5_1;
    mainDecoder = NEXUS_AudioDecoder_Open(0, &audioOpenSettings);

    /* Open audio descriptor decoder */
    secondaryDecoder = NEXUS_AudioDecoder_Open(1, &audioOpenSettings);

    /* Open mixer to mix the description and primary audio */
    NEXUS_AudioMixer_GetDefaultSettings(&mixerSettings);
    mixerSettings.mixUsingDsp = true;
    mixer = NEXUS_AudioMixer_Open(&mixerSettings);

    /* Add both decoders to the mixer */
    NEXUS_AudioMixer_AddInput(mixer, NEXUS_AudioDecoder_GetConnector(mainDecoder, NEXUS_AudioDecoderConnectorType_eMultichannel));
    NEXUS_AudioMixer_AddInput(mixer, NEXUS_AudioDecoder_GetConnector(secondaryDecoder, NEXUS_AudioDecoderConnectorType_eMultichannel));

    /* Set the Mixer to use DSP mixing */
    NEXUS_AudioMixer_GetSettings(mixer, &mixerSettings);
    mixerSettings.master = NEXUS_AudioDecoder_GetConnector(mainDecoder, NEXUS_AudioDecoderConnectorType_eMultichannel);
    NEXUS_AudioMixer_SetSettings(mixer, &mixerSettings);

    /* Add the DDRE processor after mixing */
    ddre = NEXUS_DolbyDigitalReencode_Open(NULL);

#if USE_DV258
    /* Open DV258 and add between mixer and DDRE */
    dv258 = NEXUS_DolbyVolume258_Open(NULL);
    NEXUS_DolbyVolume258_AddInput(dv258, NEXUS_AudioMixer_GetConnector(mixer));
    NEXUS_DolbyDigitalReencode_AddInput(ddre, NEXUS_DolbyVolume258_GetConnector(dv258));
#else
    /* Just connect DDRE to the mixer output */
    NEXUS_DolbyDigitalReencode_AddInput(ddre, NEXUS_AudioMixer_GetConnector(mixer));
#endif

    /* Add DAC and SPDIF */
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_DolbyDigitalReencode_GetConnector(ddre, NEXUS_DolbyDigitalReencodeConnectorType_eStereo));
#if USE_COMPRESSED_SPDIF
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_DolbyDigitalReencode_GetConnector(ddre, NEXUS_DolbyDigitalReencodeConnectorType_eCompressed));
#else
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_DolbyDigitalReencode_GetConnector(ddre, NEXUS_DolbyDigitalReencodeConnectorType_eStereo));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_DolbyDigitalReencode_GetConnector(ddre, NEXUS_DolbyDigitalReencodeConnectorType_eStereo));
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

    BDBG_WRN(("Starting Main"));
    NEXUS_AudioDecoder_Start(mainDecoder, &mainProgram);

    BDBG_WRN(("Starting Secondary"));
    NEXUS_AudioDecoder_Start(secondaryDecoder, &secondaryProgram);

    printf("Press ENTER to stop decode\n");
    getchar();

    /* example shutdown */
    NEXUS_AudioDecoder_Stop(secondaryDecoder);
    NEXUS_AudioDecoder_Stop(mainDecoder);
    NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(mixer));
    NEXUS_AudioMixer_Close(mixer);
    NEXUS_DolbyDigitalReencode_Close(ddre);
    NEXUS_AudioDecoder_Close(secondaryDecoder);
    NEXUS_AudioDecoder_Close(mainDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(mainPidChannel);
    NEXUS_PidChannel_Close(secondaryPidChannel);
    NEXUS_Platform_Uninit();

    return 0;
}

