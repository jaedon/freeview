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
 * $brcm_Workfile: sync_channel.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 1/26/12 4:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/lipsync/sync_channel.c $
 * 
 * 10   1/26/12 4:22p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 9   8/9/08 7:52a jgarrett
 * PR 43395: Fixing compilation with no component output
 * 
 * 8   6/23/08 4:07p jgarrett
 * PR 43395: Configuring HDSD_SIMUL based on num displays
 * 
 * 7   4/8/08 6:43p bandrews
 * PR41520: Allow NULL input handles to be passed in
 * 
 * 6   4/4/08 4:34p erickson
 * PR41381: added BSTD_UNUSED
 *
 * 5   4/3/08 10:53a erickson
 * PR36068: always compile sync_channel
 *
 * 4   2/28/08 10:16a erickson
 * PR36808: set frameRateMaster. allow optional cmdline pids.
 *
 * 3   2/28/08 9:28a erickson
 * PR36808: added HDSD_SIMUL
 *
 * 2   2/22/08 2:28p erickson
 * PR36808: api changes
 *
 * 1   2/4/08 11:43a erickson
 * PR36808: added sync_channel
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
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

#if NEXUS_NUM_DISPLAYS > 1
#define HDSD_SIMUL 1
#endif

/* the following define the input and its characteristics -- these will vary by input */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x11
#define AUDIO_PID 0x14

int main(int argc, char **argv)
{
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display0;
    NEXUS_VideoWindowHandle window0;
    #if HDSD_SIMUL
    NEXUS_DisplayHandle display1;
    NEXUS_VideoWindowHandle window1;
    #endif
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_SyncChannelSettings syncChannelSettings;
    NEXUS_SyncChannelHandle syncChannel;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif
    unsigned vidpid = VIDEO_PID;
    unsigned audpid = AUDIO_PID;

    if (argc > 1) {
        vidpid = strtoul(argv[1], NULL, 0);
    }
    if (argc > 2) {
        audpid = strtoul(argv[2], NULL, 0);
    }

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* create a sync channel */
    NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
    syncChannel = NEXUS_SyncChannel_Create(&syncChannelSettings);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, vidpid, NULL);
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = AUDIO_CODEC;
    audioProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, audpid, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
    audioProgram.stcChannel = videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Bring up audio decoders and connect to outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
                               NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                                   NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                                   NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif

    /* bring up HD/SD simul display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
#if NEXUS_NUM_COMPONENT_OUTPUTS || NEXUS_NUM_HDMI_OUTPUTS
    displaySettings.format = NEXUS_VideoFormat_e720p;
#elif NEXUS_NUM_COMPOSITE_OUTPUTS
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
#elif NEXUS_NUM_SCART_INPUTS
    displaySettings.format = NEXUS_VideoFormat_ePal;
#endif
    display0 = NEXUS_Display_Open(0, &displaySettings);
    window0 = NEXUS_VideoWindow_Open(display0, 0);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display0, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#elif NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display0, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#else
    #if NEXUS_NUM_SCART_INPUTS
    NEXUS_Display_AddOutput(display0, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
    #if NEXUS_NUM_SCART_INPUTS>1
    NEXUS_Display_AddOutput(display0, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
    #endif
    #endif
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display0, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display0, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display0, &displaySettings);
		}
    }
#endif

#if HDSD_SIMUL
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display1 = NEXUS_Display_Open(1, &displaySettings);
    window1 = NEXUS_VideoWindow_Open(display1, 0);
    NEXUS_Display_AddOutput(display1, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window0, NEXUS_VideoDecoder_GetConnector(vdecode));
#if HDSD_SIMUL
    NEXUS_VideoWindow_AddInput(window1, NEXUS_VideoDecoder_GetConnector(vdecode));
#endif

    /* set frame rate master */
    NEXUS_Display_GetSettings(display0, &displaySettings);
    displaySettings.frameRateMaster = NEXUS_VideoDecoder_GetConnector(vdecode);
    NEXUS_Display_SetSettings(display0, &displaySettings);

    /* connect sync channel */
    NEXUS_SyncChannel_GetSettings(syncChannel, &syncChannelSettings);
    syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(vdecode);
    syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
    syncChannelSettings.audioInput[1] = NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed);
    NEXUS_SyncChannel_SetSettings(syncChannel, &syncChannelSettings);

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }
    
    getchar();

	/* Stop decoders */
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Stop(compressedDecoder);
    }
    NEXUS_AudioDecoder_Stop(pcmDecoder);
    NEXUS_VideoDecoder_Stop(vdecode);

    /* disconnect sync channel */
    NEXUS_SyncChannel_GetSettings(syncChannel, &syncChannelSettings);
    syncChannelSettings.videoInput = NULL;
    syncChannelSettings.audioInput[0] = NULL;
    syncChannelSettings.audioInput[1] = NULL;
    NEXUS_SyncChannel_SetSettings(syncChannel, &syncChannelSettings);

#else
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);
	printf("This application is not supported on this platform!\n");
#endif
    return 0;
}
