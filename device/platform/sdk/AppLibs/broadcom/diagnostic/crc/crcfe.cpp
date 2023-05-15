/******************************************************************************
 *    (c)2009-2011 Broadcom Corporation
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
 * $brcm_Workfile: crcfe.cpp $
 * $brcm_Revision: 4 $
 * $brcm_Date: 11/1/11 10:19a $
 *
 * Module Description:
 *
 * Revision History:
 ******************************************************************************/
#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
//#include "nexus_playback.h"
//#include "nexus_file.h"
#include "nexus_parser_band.h"
#include "nexus_frontend.h"

#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "bstd.h"
#include "bkni.h"

#include "crc.h"

using namespace std;
using namespace broadcom::broadband::diagnostic;
NEXUS_DisplayHandle nexus_display_handle0;

void finishCallback() {
	cout << "CRC callback here " << endl;
}

#define USE_DVB 1 /* else use DSS */

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendSatelliteStatus status;
    NEXUS_FrontendDiseqcStatus disqecStatus;

    BSTD_UNUSED(param);

    fprintf(stderr, "Frontend(%p) - lock callback\n", (void*)frontend);

    NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
    fprintf(stderr, "  demodLocked = %d\n", status.demodLocked);
    NEXUS_Frontend_GetDiseqcStatus(frontend, &disqecStatus);
    fprintf(stderr, "  diseqc tone = %d, voltage = %d\n", disqecStatus.toneEnabled, disqecStatus.voltage);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		// print usage
		cout << "CRC Compare Usage:\n";
		cout << argv[0] << " <path to reference crc> [output frame rate] [i or p]" << endl;
		return EXIT_FAILURE;
	}
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendDiseqcSettings diseqcSettings;

	NEXUS_PlatformSettings platformSettings;
	NEXUS_PlatformConfiguration platformConfig;
	NEXUS_StcChannelHandle stcChannel;
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
	NEXUS_DisplayHandle display;
	NEXUS_DisplaySettings displaySettings;
	NEXUS_VideoWindowHandle window;
	NEXUS_VideoDecoderHandle videoDecoder;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_AudioDecoderHandle audioDecoder;
	NEXUS_AudioDecoderStartSettings audioProgram;
//	NEXUS_FilePlayHandle file;
//	NEXUS_PlaypumpHandle playpump;
//	NEXUS_PlaybackHandle playback;
//	NEXUS_PlaybackSettings playbackSettings;
//	NEXUS_PlaybackPidChannelSettings playbackPidSettings;
	NEXUS_VideoWindowSettings videoWindoSettings;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    unsigned i;
	NEXUS_Error rc;

	float output = 0;
	bool interlaced = true;
	if (argc > 2) {
		if (argv[2][0] == 'p' || (argc > 3 && argv[3][0] == 'p'))
			interlaced = false;
		if (argv[2][0] != 'i' && argv[2][0] != 'p')
			output = strtof(argv[2], NULL);
		else if (argc > 3 && argv[3][0] != 'i' && argv[3][0] != 'p')
			output = strtof(argv[3], NULL);
	}

	NEXUS_Platform_GetDefaultSettings(&platformSettings);
	platformSettings.openFrontend = false;
	platformSettings.displayModuleSettings.crcQueueSize = 600; /* This enables the CRC capture */
	if (output != 0)
		platformSettings.displayModuleSettings.dropFrame = (output - (int) output) > 0.01; // false if output is integer
	NEXUS_Platform_Init(&platformSettings);
	NEXUS_Platform_GetConfiguration(&platformConfig);

    for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
    {
        NEXUS_FrontendCapabilities capabilities;
        frontend = platformConfig.frontend[i];
        if (frontend) {
            NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
            /* Does this frontend support satellite? */
            if ( capabilities.satellite )
            {
                break;
            }
        }
    }

    if (NULL == frontend )
    {
        fprintf(stderr, "Unable to find satellite-capable frontend\n");
        return 0;
    }

    NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
#ifdef USE_DVB
    satSettings.frequency = 1119000000;
    satSettings.mode = NEXUS_FrontendSatelliteMode_eDvb;
#else
    satSettings.frequency = 1396820000;
    satSettings.mode = NEXUS_FrontendSatelliteMode_eDss;
#endif
    satSettings.lockCallback.callback = lock_callback;
    satSettings.lockCallback.context = frontend;
    NEXUS_Frontend_GetUserParameters(frontend, &userParams);

    /* Map a parser band to the demod's input band. */
    parserBand = NEXUS_ParserBand_e2; /* USER: Change this value to match your platform */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    printf("Input Band is %d\n", userParams.param1);
#ifdef USE_DVB
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
#else
    parserBandSettings.transportType = NEXUS_TransportType_eDssEs;
#endif
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    NEXUS_Frontend_GetDiseqcSettings(frontend, &diseqcSettings);
#ifdef USE_DVB
    diseqcSettings.toneEnabled = true;
    diseqcSettings.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
#else
    diseqcSettings.toneEnabled = false;
    diseqcSettings.voltage = NEXUS_FrontendDiseqcVoltage_e18v;
#endif
    NEXUS_Frontend_SetDiseqcSettings(frontend, &diseqcSettings);
    printf("Set DiseqcSettings\n");

	/* Bring up audio decoders and outputs */
	audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
	NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
			NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
	NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
			NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

	/* Bring up video display and outputs */
	NEXUS_Display_GetDefaultSettings(&displaySettings);
	displaySettings.displayType = NEXUS_DisplayType_eAuto;
	displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e16x9;

	float frameRate = output > 0 ? output : 59.94;
//	if (psi.videoWidth <= 720 && psi.videoHeight <= 480) {
//		displaySettings.format = interlaced ? NEXUS_VideoFormat_eNtsc : NEXUS_VideoFormat_e480p;
//	}
//	else if (psi.videoWidth <= 1280 && psi.videoHeight <= 720) {
//		if (frameRate >= 59.93)
//			displaySettings.format = NEXUS_VideoFormat_e720p;
//		else if (frameRate >= 50)
//			displaySettings.format = NEXUS_VideoFormat_e720p50hz;
//		else if (frameRate >= 30)
//			displaySettings.format = NEXUS_VideoFormat_e720p30hz;
//		else if (frameRate >= 25)
//			displaySettings.format = NEXUS_VideoFormat_e720p25hz;
//		else if (frameRate >= 24)
//			displaySettings.format = NEXUS_VideoFormat_e720p24hz;
//		else
//			displaySettings.format = NEXUS_VideoFormat_e720p;
//	}
//	else if (psi.videoWidth == 1920 && psi.videoHeight == 1080 && !interlaced) {
	if (!interlaced) {
		if (frameRate >= 59.93)
			displaySettings.format = NEXUS_VideoFormat_e1080p60hz;
		else if (frameRate >= 50)
			displaySettings.format = NEXUS_VideoFormat_e1080p50hz;
		else if (frameRate >= 30)
			displaySettings.format = NEXUS_VideoFormat_e1080p30hz;
		else if (frameRate >= 25)
			displaySettings.format = NEXUS_VideoFormat_e1080p25hz;
		else if (frameRate >= 24)
			displaySettings.format = NEXUS_VideoFormat_e1080p24hz;
		else
			displaySettings.format = NEXUS_VideoFormat_e1080p;
	}
	else {
		if (frameRate == 50)
			displaySettings.format = NEXUS_VideoFormat_e1080i50hz;
		else
			displaySettings.format = NEXUS_VideoFormat_e1080i;
	}
cout << "display format: " << displaySettings.format << endl;
	if (output != 0)
		displaySettings.frameRateMaster = NULL;

	nexus_display_handle0 = display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_HDMI_OUTPUTS
	if (platformConfig.outputs.hdmi[0]) {
		rc = NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
		BDBG_ASSERT(!rc);
	}
#endif
#if NEXUS_DTV_PLATFORM
	NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
	NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
#if NEXUS_NUM_COMPONENT_OUTPUTS
	if (platformConfig.outputs.component[0]) {
		NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
	}
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#endif
	window = NEXUS_VideoWindow_Open(display, 0);

	if (output != 0) {
		NEXUS_VideoWindow_GetSettings(window, &videoWindoSettings);
		videoWindoSettings.autoMaster = false;
		NEXUS_VideoWindow_SetSettings(window, &videoWindoSettings);
	}

	/* bring up decoder and connect to display */
	videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

#ifdef USE_DVB
        videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x31, NULL);
        audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x34, NULL);
#else
        videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x78, NULL);
        audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x79, NULL);
#endif

	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	 the audio outputs. */
	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
	videoProgram.pidChannel = videoPidChannel;
	videoProgram.stcChannel = stcChannel;
	if (output != 0) {
		cout << "Setting output frame rate to " << output
				<< "." << endl;
	}

	frameRate = output > 0 ? output : 59.94;
	if (frameRate > 60)
		videoProgram.frameRate = NEXUS_VideoFrameRate_eMax;
	else if (frameRate > 59.95)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e60;
	else if (frameRate > 50)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e59_94;
	else if (frameRate > 30)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e50;
	else if (frameRate > 29.96)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e30;
	else if (frameRate > 25)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e29_97;
	else if (frameRate > 24)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e25;
	else if (frameRate > 23.976)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e24;
	else if (frameRate > 14.985)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e23_976;
	else if (frameRate > 7.943)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e14_985;
//	else if (frameRate > 0)
//		videoProgram.frameRate = NEXUS_VideoFrameRate_e7_943;
	else
		videoProgram.frameRate = NEXUS_VideoFrameRate_eUnknown;

cout << "video frame rate: " << videoProgram.frameRate << endl;

	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
	audioProgram.codec = NEXUS_AudioCodec_eAc3;
	audioProgram.pidChannel = audioPidChannel;
	audioProgram.stcChannel = stcChannel;

    rc = NEXUS_Frontend_TuneSatellite(frontend, &satSettings);
    BDBG_ASSERT(!rc);

    while (1)
    {
        NEXUS_FrontendSatelliteStatus satStatus;
        rc = NEXUS_Frontend_GetSatelliteStatus(frontend, &satStatus);
        if (rc) {
            printf("unable to read status\n");
        }
        else {
            printf(
                "Sat Status\n"
                "  symbolRate %d\n"
                "  locked  %d\n",
                    satStatus.settings.symbolRate,
                    satStatus.demodLocked);
            if(satStatus.demodLocked)
                break;
        }
        BKNI_Sleep(1000);
    }

	/* Start decoders */
	NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
	NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

	NexusCrc ref(display);
	bool hasRef = ref.ReadReference(argv[1]);
	if (!hasRef)
		cout << "Error: Unable to read reference CRC file " << argv[1] << endl;
	//	else {
	ref.Start(finishCallback);
	ref.Wait();

	/* Bring down system */
	NEXUS_VideoDecoder_Stop(videoDecoder);
	NEXUS_AudioDecoder_Stop(audioDecoder);
//	NEXUS_Playback_Stop(playback);
//	NEXUS_FilePlay_Close(file);
//	NEXUS_Playback_Destroy(playback);
//	NEXUS_Playpump_Close(playpump);
	NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
	NEXUS_VideoDecoder_Close(videoDecoder);
	NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
	NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
	NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
	NEXUS_AudioDecoder_Close(audioDecoder);
	NEXUS_VideoWindow_Close(window);
	NEXUS_Display_Close(display);
	NEXUS_StcChannel_Close(stcChannel);
	NEXUS_Platform_Uninit();

	//	if (hasRef) {
	cout << "CRC compare: " << ref.Result() << "\n";
	if (!ref.Pass())
		ref.DumpCaptured((string(argv[1]) + ".captured_crc").c_str());
	//	}

	return EXIT_SUCCESS;
}
