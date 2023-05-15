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
 * $brcm_Workfile: crc_mvc.cpp $
 * $brcm_Revision: 10 $
 * $brcm_Date: 12/6/11 6:06p $
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
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "nexus_core_utils.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <cmath>

#include "bstd.h"
#include "bkni.h"
#include "b_playback_ip_lib.h"

#include "crc.h"

using namespace std;
using namespace broadcom::broadband::diagnostic;

int gCRC = 0;
void finishCallback(void *) {
	__sync_fetch_and_sub(&gCRC, 1);
}

static void sourceChanged(void *data, int unused) {
	BSTD_UNUSED(unused);
	BKNI_SetEvent((BKNI_EventHandle) data);
}

static void play_callback(void *context, int param) {
	BSTD_UNUSED(param);
	BKNI_SetEvent((BKNI_EventHandle) context);
}

void setDisplayFrameRate3D(B_PlaybackIpPsiInfo & psi, NEXUS_DisplaySettings & displaySettings, bool interlaced,
		float frameRate) {
	if (psi.videoWidth <= 720 && psi.videoHeight <= 480) {
		displaySettings.format = NEXUS_VideoFormat_e3D_1080p_24Hz;
	}
	else if (psi.videoWidth <= 1280 && psi.videoHeight <= 720) {
		if (frameRate >= 59)
			displaySettings.format = NEXUS_VideoFormat_e3D_720p;

		else if (frameRate >= 49)
			displaySettings.format = NEXUS_VideoFormat_e3D_720p_50Hz;

		else if (frameRate >= 29)
			displaySettings.format = NEXUS_VideoFormat_e3D_720p_30Hz;

//		else if (frameRate >= 24.9)
//			displaySettings.format = NEXUS_VideoFormat_e720p25hz;

		else if (frameRate >= 23)
			displaySettings.format = NEXUS_VideoFormat_e3D_720p_24Hz;

		else
			displaySettings.format = NEXUS_VideoFormat_e3D_720p;

	}
	else if (psi.videoWidth == 1920 && psi.videoHeight == 1080 && !interlaced) {
//		if (frameRate >= 59)
//			displaySettings.format = NEXUS_VideoFormat_e1080p60hz;
//
//		else if (frameRate >= 49)
//			displaySettings.format = NEXUS_VideoFormat_e1080p50hz;

//		else
		if (frameRate >= 29)
			displaySettings.format = NEXUS_VideoFormat_e3D_1080p_30Hz;

//		else if (frameRate >= 24.9)
//			displaySettings.format = NEXUS_VideoFormat_e1080p25hz;

		else if (frameRate >= 23)
			displaySettings.format = NEXUS_VideoFormat_e3D_1080p_24Hz;

		else
			displaySettings.format = NEXUS_VideoFormat_e3D_1080p_24Hz;

	}
	else {
		if (frameRate == 50)
			displaySettings.format = NEXUS_VideoFormat_e3D_1080p_24Hz;

		else
			displaySettings.format = NEXUS_VideoFormat_e3D_1080p_30Hz;
	}

	cout << "Display format: " << displaySettings.format << endl;
}

void setDisplayFrameRate(B_PlaybackIpPsiInfo & psi, NEXUS_DisplaySettings & displaySettings, bool interlaced,
		float frameRate) {
	if (psi.videoWidth <= 720 && psi.videoHeight <= 480) {
		displaySettings.format = interlaced ? NEXUS_VideoFormat_eNtsc : NEXUS_VideoFormat_e480p;
	}
	else if (psi.videoWidth <= 1280 && psi.videoHeight <= 720) {
		if (frameRate >= 59)
			displaySettings.format = NEXUS_VideoFormat_e720p;

		else if (frameRate >= 49)
			displaySettings.format = NEXUS_VideoFormat_e720p50hz;

		else if (frameRate >= 29)
			displaySettings.format = NEXUS_VideoFormat_e720p30hz;

		else if (frameRate >= 24.9)
			displaySettings.format = NEXUS_VideoFormat_e720p25hz;

		else if (frameRate >= 23)
			displaySettings.format = NEXUS_VideoFormat_e720p24hz;

		else
			displaySettings.format = NEXUS_VideoFormat_e720p;
	}
	else if (psi.videoWidth == 1920 && psi.videoHeight == 1080 && !interlaced) {
		if (frameRate >= 59)
			displaySettings.format = NEXUS_VideoFormat_e1080p60hz;

		else if (frameRate >= 49)
			displaySettings.format = NEXUS_VideoFormat_e1080p50hz;

		else if (frameRate >= 29)
			displaySettings.format = NEXUS_VideoFormat_e1080p30hz;

		else if (frameRate >= 24.9)
			displaySettings.format = NEXUS_VideoFormat_e1080p25hz;

		else if (frameRate >= 23)
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

	cout << "Display format: " << displaySettings.format << endl;
}

static int printUsage(char* argv[]) {
	cout << "CRC Compare Usage:\n";
	cout << argv[0]
			<< " <path to stream> [path to reference crc] [output frame rate] [i or p] [-avd <path to avd reference crc>] [-hdmi <path to hdmi reference crc>] [-mfd <path to MFD reference crc>]"
			<< endl;
	return EXIT_FAILURE;
}

int main(int argc, char* argv[]) {
	if (argc < 3)
		return printUsage(argv);

	NEXUS_PlatformSettings platformSettings;
	NEXUS_PlatformConfiguration platformConfig;
	NEXUS_StcChannelHandle stcChannel;
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_PidChannelHandle videoPidChannel, audioPidChannel, enhancementVideoPidChannel;
	NEXUS_DisplayHandle display;
	NEXUS_DisplaySettings displaySettings;
	NEXUS_VideoWindowHandle window;
	NEXUS_VideoDecoderHandle videoDecoder;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_AudioDecoderHandle audioDecoder;
	NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_PlaypumpHandle playpump;
	FILE *file;
	NEXUS_PlaypumpSettings playpumpSettings;
	NEXUS_VideoWindowSettings videoWindowSettings;
	NEXUS_Error rc;
	BKNI_EventHandle sourceChangedEvent;
	NEXUS_VideoDecoderSettings videoDecoderSettings;
	BERR_Code bc;
	BKNI_EventHandle bufferReadyEvent;

	bool disp = false; // compare display crc if argv[2] is a ref crc file
	float output = 0;
	bool interlaced = true;
	for (int i = 2; i < argc; i++) {
		string s(argv[i]);
		if (s == "i" || s == "p") {
			interlaced = s == "i";
			if (!disp && i > 2)
				disp = true;
		}
		else if (output == 0) {
			output = strtof(argv[i], NULL);
			if (output && !disp && i > 2)
				disp = true;
		}
	}
	cout << "User defined frame rate: " << output << (interlaced ? ". Interlaced." : "") << endl;

	bool avd = false, hdmi = false, mfd = false;
	string avdRef, hdmiRef, mfdRef;
	for (int i = 2; i < argc; i++) {
		string s(argv[i]);
		if (s == "-avd") {
			if (argc > i) {
				avd = true;
				avdRef = string(argv[i + 1]);
				cout << "Using AVD ref file: " << avdRef << endl;
				if (!disp && i > 2 && !hdmi && !mfd)
					disp = true;
			}
			else
				return printUsage(argv);
		}
		else if (s == "-hdmi") {
			if (argc > i) {
				hdmi = true;
				hdmiRef = string(argv[i + 1]);
				cout << "Using HDMI ref file: " << hdmiRef << endl;
				if (!disp && i > 2 && !avd && !mfd)
					disp = true;
			}
			else
				return printUsage(argv);
		}
		else if (s == "-mfd") {
			if (argc > i) {
				mfd = true;
				mfdRef = string(argv[i + 1]);
				cout << "Using MFD ref file: " << mfdRef << endl;
				if (!disp && i > 2 && !avd && !hdmi)
					disp = true;
			}
			else
				return printUsage(argv);
		}
	}

	if (!disp)
		disp = !avd && !hdmi && !mfd;

	const char *fname = argv[1];

	NEXUS_Platform_GetDefaultSettings(&platformSettings);
	platformSettings.openFrontend = false;
	if (output != 0) {
		platformSettings.displayModuleSettings.dropFrame = abs(output - (int) (output)) >= 0.01; // false if output is integer
		if (platformSettings.displayModuleSettings.dropFrame)
			cout << "Drop frame true." << endl;
	}
	NEXUS_Platform_Init(&platformSettings);

	NEXUS_Platform_GetConfiguration(&platformConfig);
	/* call IP Applib to setup the streaming from Rave buffers */
	B_PlaybackIpFileStreamingOpenSettings fileStreamingSettings;
	memset(&fileStreamingSettings, 0, sizeof(fileStreamingSettings));
	fileStreamingSettings.mediaProbeOnly = true;
	strncpy(fileStreamingSettings.fileName, fname, strlen(fname));
	B_PlaybackIpFileStreamingHandle playbackIpFileHandle;
	if ((playbackIpFileHandle = B_PlaybackIp_FileStreamingOpen(&fileStreamingSettings)) == NULL) {
		cout << "ERROR: Failed to open File Streaming handle " << fname << endl;
		return EXIT_FAILURE;
	}
	B_PlaybackIpPsiInfo psi;
	B_PlaybackIp_FileStreamingGetMediaInfo(playbackIpFileHandle, &psi);
	B_PlaybackIp_FileStreamingClose(playbackIpFileHandle);
	cout << "Video pid found: " << hex << psi.videoPid << ". Extra video pid: " << psi.extraVideoPid << endl;

	file = fopen(fname, "rb");
	if (!file) {
		fprintf(stderr, "can't open file:%s\n", fname);
		return EXIT_FAILURE;
	}

	BKNI_CreateEvent(&bufferReadyEvent);
	playpump = NEXUS_Playpump_Open(0, NULL);
	NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
	playpumpSettings.dataCallback.callback = play_callback;
	playpumpSettings.dataCallback.context = bufferReadyEvent;
	playpumpSettings.transportType = psi.extraVideoPid ? NEXUS_TransportType_eTs : psi.mpegType;
	NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
	NEXUS_Playpump_Start(playpump);

	NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

	/* Bring up video display and outputs */
	NEXUS_Display_GetDefaultSettings(&displaySettings);
	displaySettings.displayType = NEXUS_DisplayType_eAuto;
	displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;
	cout << "Video size: " << dec << psi.videoWidth << "x" << psi.videoHeight << endl;

	if (disp)
		displaySettings.crcQueueSize = 600; /* This enables the CRC capture */

	float frameRate = output > 0 ? output : psi.videoFrameRate;
	if (psi.extraVideoPid)
		setDisplayFrameRate3D(psi, displaySettings, interlaced, frameRate);
	else
		setDisplayFrameRate(psi, displaySettings, interlaced, frameRate);

	if (output != 0)
		displaySettings.frameRateMaster = NULL;

	NEXUS_VideoFormatInfo videoFormatInfo;
	NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
	if (videoFormatInfo.isFullRes3d) {
		displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eHdmiDvo; /* HDMI master mode is required */
	}

	display = NEXUS_Display_Open(0, &displaySettings);

	if (platformConfig.outputs.hdmi[0]) {
		rc = NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
		BDBG_ASSERT(!rc);
	}
//	if (platformConfig.outputs.component[0]) {
//		NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
//	}
//	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
	window = NEXUS_VideoWindow_Open(display, 0);
	if (output != 0) {
		NEXUS_VideoWindow_GetSettings(window, &videoWindowSettings);
		videoWindowSettings.autoMaster = false;
		NEXUS_VideoWindow_SetSettings(window, &videoWindowSettings);
	}

	/* bring up decoder and connect to display */
	NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
	NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
	videoDecoderOpenSettings.fifoSize = 6 * 1024 * 1024; /* this is stream-dependent */
	videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);

	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

	/* register sourceChanged callback, set bufferReadyEvent, wait bufferReadyEvent */
	BKNI_CreateEvent(&sourceChangedEvent);
	NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
	videoDecoderSettings.sourceChanged.callback = sourceChanged;
	videoDecoderSettings.sourceChanged.context = sourceChangedEvent;
	/* setup 3D */
	if (psi.extraVideoPid) {
		videoDecoderSettings.customSourceOrientation = true;
		videoDecoderSettings.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightFullFrame;
	}
	NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);

	/* Open the audio and video pid channels */
	NEXUS_PlaypumpOpenPidChannelSettings pidSettings;
	NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidSettings);
	pidSettings.pidType = NEXUS_PidType_eVideo;
//	pidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH264_Mvc;//psi.videoCodec; /* must be told codec for correct handling */
//	pidSettings.video.index = true;
//	pidSettings.video.decoder = videoDecoder;
	videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, psi.videoPid, NULL);
	enhancementVideoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, psi.extraVideoPid, NULL);

	/* Bring up audio decoders and outputs */
	if (psi.audioPid) {
		audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
		NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
				NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
		NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
				NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

		NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidSettings);
		pidSettings.pidType = NEXUS_PidType_eAudio;
//	    pidSettings.pidTypeSettings.audio = psi.audioCodec;
//	    pidSettings.pidTypeSettings.audio.primary = audioDecoder;
		audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, psi.audioPid, NULL);
	}
	else {
		audioDecoder = 0; // to get rid of "may be used uninitialized in this function" warning
		audioPidChannel = 0;
	}

	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	 the audio outputs. */
	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	videoProgram.codec = psi.extraVideoPid ? NEXUS_VideoCodec_eH264_Mvc : psi.videoCodec;
	videoProgram.pidChannel = videoPidChannel;
	videoProgram.stcChannel = stcChannel;
	videoProgram.enhancementPidChannel = enhancementVideoPidChannel;
	if (output != 0 && psi.videoFrameRate != output) {
		cout << "Detected input frame rate is " << psi.videoFrameRate << ". Setting output frame rate to " << output
				<< "." << endl;
	}
	else
		cout << "Output frame rate is " << psi.videoFrameRate << "." << endl;

	frameRate = output > 0 ? output : psi.videoFrameRate;
	if (frameRate > 60)
		videoProgram.frameRate = NEXUS_VideoFrameRate_eMax;
	else if (frameRate > 59.95)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e60;
	else if (frameRate > 50)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e59_94;
	else if (frameRate > 30)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e50;
	else if (frameRate > 29.98)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e30;
	else if (frameRate > 25)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e29_97;
	else if (frameRate > 24)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e25;
	else if (frameRate > 23.98)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e24;
	else if (frameRate > 14.99)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e23_976;
	else if (frameRate > 7.95)
		videoProgram.frameRate = NEXUS_VideoFrameRate_e14_985;
//	else if (frameRate > 0)
//		videoProgram.frameRate = NEXUS_VideoFrameRate_e7_943;
	else
		videoProgram.frameRate = NEXUS_VideoFrameRate_eUnknown;

	cout << "video frame rate enum: " << videoProgram.frameRate << endl;
	if (psi.audioPid)
	{
		NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
		audioProgram.codec = psi.audioCodec;
		audioProgram.pidChannel = audioPidChannel;
		audioProgram.stcChannel = stcChannel;
	}

	/* Start decoders */
	NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
	if (psi.audioPid)
		NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

	/* set up CRC capture */
	char* lumaOnly = ::getenv("CRC_LUMA_ONLY");

	NexusDisplayCrc* ref = NULL;
	if (disp) {
		ref = new NexusDisplayCrc(display, false, true, videoDecoder);
		if (lumaOnly != NULL)
			ref->SetCompareLumaOnly(string(lumaOnly) == "y");
		bool hasRef = ref->ReadReference(argv[2]);
		if (!hasRef) {
			cout << "Error: Unable to read reference CRC file " << argv[2] << ". Default to AVD only." << endl;
			displaySettings.crcQueueSize = 0;
			NEXUS_Display_SetSettings(display, &displaySettings);
			delete ref;
			ref = NULL;
		}
	}

//	NEXUS_DisplayCrcData d[] = {
//			{{1, 2, 3}},
//			{{2, 3, 4}},
//			{{5, 6, 7}},
//			{{12, 13, 14}}
//	};
//	ref.AddReference(d, sizeof(d) / sizeof(NEXUS_DisplayCrcData));
//	cout << "Size of d: " << sizeof(d) / sizeof(NEXUS_DisplayCrcData) << endl;

	NexusVideoDecoderCrc* avdCrc = NULL;
	if (avd) {
		avdCrc = new NexusVideoDecoderCrc(videoDecoder);
		if (lumaOnly != NULL)
			avdCrc->SetCompareLumaOnly(string(lumaOnly) == "y");
		if (!avdCrc->ReadReference(avdRef.c_str()))
			cout << "Error: Unable to read reference AVD CRC file " << avdRef << endl;
	}

	NexusHdmiCrc* hdmiCrc = NULL;
#if NEXUS_NUM_HDMI_OUTPUTS
	if (hdmi) {
		hdmiCrc = new NexusHdmiCrc(platformConfig.outputs.hdmi[0]);
		if (!hdmiCrc->ReadReference(hdmiRef.c_str()))
			cout << "Error: Unable to read reference HDMI CRC file " << hdmiRef << endl;
	}
#endif
	NexusVideoInputCrc* mfdCrc = NULL;
	if (mfd) {
		NEXUS_VideoInput videoInput = NEXUS_VideoDecoder_GetConnector(videoDecoder);
		mfdCrc = new NexusVideoInputCrc(videoInput);
		if (!mfdCrc->ReadReference(mfdRef.c_str()))
			cout << "Error: Unable to read reference MFD CRC file " << mfdRef << endl;
	}

	if (ref)
		ref->Start(finishCallback);
	if (avdCrc)
		avdCrc->Start(finishCallback);
	if (hdmiCrc)
		hdmiCrc->Start(finishCallback);
	if (mfdCrc)
		mfdCrc->Start(finishCallback);

	gCRC = 0;
	if (ref)
		gCRC++;
	if (avdCrc)
		gCRC++;
	if (hdmiCrc)
		gCRC++;
	if (mfdCrc)
		gCRC++;

	/* Start playback */
	while (gCRC > 0) {
		void *buffer;
		size_t buffer_size;

		if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
			break;
		if (buffer_size == 0) {
			BKNI_WaitForEvent(bufferReadyEvent, BKNI_INFINITE);
			continue;
		}

		/* The first call to get_buffer will return the entire playback buffer.
		 If we use it, we're going to have to wait until the descriptor is complete,
		 and then we're going to underflow. So use a max size. */
#define MAX_READ (188*1024)
		if (buffer_size > MAX_READ)
			buffer_size = MAX_READ;

		int n = fread(buffer, 1, buffer_size, file);
		if (n < 0) {
			rc = BERR_TRACE(ferror(file));
			fprintf(stderr, "can't read file:%s\n", fname);
			return rc;
		}

		if (n == 0) {
			// add EOS
#define BMPEG2TS_PKT_LEN	    (188)
#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))

			uint8_t pkt[BMPEG2TS_PKT_LEN];
			pkt[0] = 0x47;
			pkt[1] = B_GET_BITS(psi.videoPid, 12, 8);
			pkt[2] = B_GET_BITS(psi.videoPid, 7, 0);
			pkt[3] = 0x10;
			/* create dummy startcodes */
			for (int i = 4; i < BMPEG2TS_PKT_LEN; i += 8) {
				pkt[i] = 0x00;
				pkt[i + 1] = 0x00;
				pkt[i + 2] = 0x00;
				pkt[i + 3] = 0x00;
				pkt[i + 4] = 0x00;
				pkt[i + 5] = 0x00;
				pkt[i + 6] = 0x01;
				pkt[i + 7] = 0x0A;
			}

			if (buffer_size >= BMPEG2TS_PKT_LEN) {
				cout << "End of file reached. Addding EOS." << endl;
				memcpy(buffer, pkt, BMPEG2TS_PKT_LEN);
				if (buffer_size >= 2 * BMPEG2TS_PKT_LEN) {
					memcpy(buffer + BMPEG2TS_PKT_LEN, pkt, BMPEG2TS_PKT_LEN);
					rc = NEXUS_Playpump_WriteComplete(playpump, 0, 2 * BMPEG2TS_PKT_LEN);
				}
				else
					rc = NEXUS_Playpump_WriteComplete(playpump, 0, BMPEG2TS_PKT_LEN);
				BDBG_ASSERT(!rc);
			}
			else {
				cerr << "Not enough buffer " << buffer_size << endl;
			}
			/* wait for the decoder to reach the end of the content before looping */
			while (gCRC > 0) {
				NEXUS_VideoDecoderStatus status;
				NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
				if (!status.queueDepth)
					break;
			}
			fseek(file, 0, SEEK_SET);
			NEXUS_Playpump_Flush(playpump);
		}
		else if (gCRC > 0) {
			rc = NEXUS_Playpump_WriteComplete(playpump, 0, n);
			BDBG_ASSERT(!rc);

			if (output == 0 && psi.videoFrameRate == 0) {
				bc = BKNI_WaitForEvent(sourceChangedEvent, 10 * 1000);
				if (bc == BERR_TIMEOUT)
					cout << "Info: no source change for 10 seconds. " << endl;
				else if (bc == BERR_OS_ERROR)
					cout << "Warning: wait for source change encountered os error. " << endl;

				NEXUS_VideoDecoderStatus videoStatus;
				NEXUS_DisplaySettings dispSettings;
				NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
				NEXUS_Display_GetSettings(display, &dispSettings);
				if (videoStatus.format != dispSettings.format)
		//		{
					cout << "Decoder format enum: " << videoStatus.format << ". Display format enum: "
							<< dispSettings.format << endl;

				videoProgram.frameRate = videoStatus.frameRate;
			//		if (videoStatus.frameRate != NEXUS_VideoFrameRate_eUnknown) {
				NEXUS_VideoDecoder_Stop(videoDecoder);
				if (psi.audioPid)
					NEXUS_AudioDecoder_Stop(audioDecoder);
				fseek(file, 0, SEEK_SET);
				NEXUS_Playpump_Flush(playpump);

				switch (videoStatus.frameRate) {
				case NEXUS_VideoFrameRate_e23_976:
					psi.videoFrameRate = 23.976;
					break;
				case NEXUS_VideoFrameRate_e24:
					psi.videoFrameRate = 24;
					break;
				case NEXUS_VideoFrameRate_e25:
					psi.videoFrameRate = 25;
					break;
				case NEXUS_VideoFrameRate_e29_97:
					psi.videoFrameRate = 29.97;
					break;
				case NEXUS_VideoFrameRate_e30:
					psi.videoFrameRate = 30;
					break;
				case NEXUS_VideoFrameRate_e50:
					psi.videoFrameRate = 50;
					break;
				case NEXUS_VideoFrameRate_e59_94:
					psi.videoFrameRate = 59.94;
					break;
				case NEXUS_VideoFrameRate_e60:
					psi.videoFrameRate = 60;
					break;
				case NEXUS_VideoFrameRate_e14_985:
					psi.videoFrameRate = 14.985;
					break;
					//			case NEXUS_VideoFrameRate_e7_943:
					//				psi.videoFrameRate = 7.943;
					//				break;
				case NEXUS_VideoFrameRate_eMax:
					psi.videoFrameRate = 120;
					break;
				default:
					psi.videoFrameRate = 60;
					break;
				}
				cout << "Decoder video frame rate: " << psi.videoFrameRate
						<< (videoStatus.interlaced ? ". Interlaced." : "") << endl;
				if (psi.extraVideoPid)
					setDisplayFrameRate3D(psi, dispSettings, videoStatus.interlaced, psi.videoFrameRate);
				else
					setDisplayFrameRate(psi, dispSettings, videoStatus.interlaced, psi.videoFrameRate);

				NEXUS_Display_SetSettings(display, &dispSettings);

				NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
				if (psi.audioPid)
					NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
			//		}
		//		}
			}
		}
	}

	if (ref) {
		stringstream details;
		cout << "CRC compare for " << fname << "\n" << ref->Result(details) << "\n";
		cout << details.str();
		if (!ref->Pass())
			ref->DumpCaptured((string(fname) + ".crc_disp").c_str());
		delete ref;
	}

	if (avdCrc) {
		cout << "AVD CRC compare for " << fname << "\n" << avdCrc->Result() << "\n";
		if (!avdCrc->Pass())
			avdCrc->DumpCaptured((string(fname) + ".crc_avd").c_str());
		delete avdCrc;
	}

	if (hdmiCrc) {
		cout << "HDMI CRC compare for " << fname << "\n" << hdmiCrc->Result() << "\n";
		if (!hdmiCrc->Pass())
			hdmiCrc->DumpCaptured((string(fname) + ".crc_hdmi").c_str());
		delete hdmiCrc;
	}

	if (mfdCrc) {
		cout << "MFD CRC compare for " << fname << "\n" << mfdCrc->Result() << "\n";
		if (!mfdCrc->Pass())
			mfdCrc->DumpCaptured((string(fname) + ".crc_mfd").c_str());
		delete mfdCrc;
	}

	/* Bring down system */
	NEXUS_VideoDecoder_Stop(videoDecoder);
	if (psi.audioPid)
		NEXUS_AudioDecoder_Stop(audioDecoder);
//	NEXUS_Playback_Stop(playback);
	fclose(file);
//	NEXUS_FilePlay_Close(file);
//	NEXUS_Playback_Destroy(playback);
	NEXUS_Playpump_Close(playpump);
#if NEXUS_NUM_HDMI_OUTPUTS
	if (platformConfig.outputs.hdmi[0])
		NEXUS_Display_RemoveOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif
	NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
	NEXUS_VideoDecoder_Close(videoDecoder);
	if (psi.audioPid) {
		NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
		NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
		NEXUS_AudioInput_Shutdown(
				NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
		NEXUS_AudioDecoder_Close(audioDecoder);
	}
	NEXUS_VideoWindow_Close(window);
	NEXUS_Display_Close(display);
	NEXUS_StcChannel_Close(stcChannel);
	BKNI_DestroyEvent(bufferReadyEvent);
	BKNI_DestroyEvent(sourceChangedEvent);
	NEXUS_Platform_Uninit();

	return EXIT_SUCCESS;
}