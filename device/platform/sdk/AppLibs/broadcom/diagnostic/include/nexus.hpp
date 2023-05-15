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
 * $brcm_Workfile: nexus.hpp $
 * $brcm_Revision: 24 $
 * $brcm_Date: 12/6/11 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 ******************************************************************************/
#ifndef __NEXUS_HPP__
#define __NEXUS_HPP__

#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_core_utils.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#if NEXUS_HAS_STREAM_MUX
#include "nexus_video_adj.h"
#include "nexus_video_encoder.h"
#include "nexus_audio_encoder.h"
#include "nexus_stream_mux.h"
#include "nexus_recpump.h"
#include "nexus_record.h"
#endif
#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel.h"
#endif
#include "nexus_audio_capture.h"
#include "bdbg.h"

using namespace std;

namespace broadcom {
namespace broadband {
namespace diagnostic {

class StcChannel {
	unsigned idx;
	NEXUS_StcChannelHandle stcChannel;
	NEXUS_StcChannelSettings stcSettings;

public:
	StcChannel(unsigned index) :
			idx(index), stcChannel(NULL) {
		NEXUS_StcChannel_GetDefaultSettings(index, &stcSettings);
	}

	virtual ~StcChannel() {
		if (stcChannel)
			NEXUS_StcChannel_Close(stcChannel);
	}

	NEXUS_StcChannelSettings *settings() {
		return &stcSettings;
	}

	NEXUS_Error update(NEXUS_StcChannelSettings *settings = NULL) {
		return NEXUS_StcChannel_SetSettings(*this, settings ? settings : &stcSettings);
	}

	operator NEXUS_StcChannelHandle() {
		if (!stcChannel)
			stcChannel = NEXUS_StcChannel_Open(idx, &stcSettings);
		return stcChannel;
	}
};

class Window {
	friend class Display;

	NEXUS_VideoWindowHandle window;
	NEXUS_VideoWindowSettings videoWindowSettings;
//	vector<NEXUS_VideoInput> connectorTypes;

	Window(NEXUS_DisplayHandle display, unsigned index = 0) {
		window = NEXUS_VideoWindow_Open(display, index);
		NEXUS_VideoWindow_GetSettings(window, &videoWindowSettings);
	}

public:
	virtual ~Window() {
//		for (vector<NEXUS_VideoInput>::iterator i = connectorTypes.begin(); i < connectorTypes.end(); i++)
//			NEXUS_VideoInput_Shutdown(*i); // for some reason, crash at shutdown

		NEXUS_VideoWindow_Close(window);
	}

	NEXUS_VideoWindowSettings *settings() {
		return &videoWindowSettings;
	}

	NEXUS_Error update(NEXUS_VideoWindowSettings *settings = NULL) {
		return NEXUS_VideoWindow_SetSettings(window, settings ? settings : &videoWindowSettings);
	}

	operator NEXUS_VideoWindowHandle() {
		return window;
	}

	NEXUS_Error connect(NEXUS_VideoInput input) {
//		connectorTypes.push_back(input);
		return NEXUS_VideoWindow_AddInput(window, input);
	}
};

class Display {
	unsigned idx;
	NEXUS_DisplayHandle display;
	NEXUS_DisplaySettings displaySettings;
	map<unsigned, Window*> windows;

public:
	Display(unsigned index) :
			idx(index), display(NULL) {
		NEXUS_Display_GetDefaultSettings(&displaySettings);
	}

	virtual ~Display() {
		for (map<unsigned, Window*>::iterator i = windows.begin(); i != windows.end(); ++i)
			delete i->second;

		if (display) {
			NEXUS_Display_RemoveAllOutputs(display);
			NEXUS_Display_Close(display);
		}
	}

	NEXUS_DisplaySettings *settings() {
		return &displaySettings;
	}

	NEXUS_Error update(NEXUS_DisplaySettings *settings = NULL) {
		return NEXUS_Display_SetSettings(*this, settings ? settings : &displaySettings);
	}

	operator NEXUS_DisplayHandle() {
		if (!display)
			display = NEXUS_Display_Open(idx, &displaySettings);
		return display;
	}

	Window* open(unsigned index = 0) {
		if (windows.count(index) < 1)
			windows[index] = new Window(*this, index);
		return windows[index];
	}

	NEXUS_Error addOutput(NEXUS_VideoOutput output) {
		return NEXUS_Display_AddOutput(*this, output);
	}

	void setFrameRate(uint16_t videoWidth, uint16_t videoHeight, bool interlaced, float frameRate) {
		if (videoWidth <= 720 && videoHeight <= 480) {
			displaySettings.format = interlaced ? NEXUS_VideoFormat_eNtsc : NEXUS_VideoFormat_e480p;
		}
		else if (videoWidth <= 1280 && videoHeight <= 720) {
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
		else if (videoWidth == 1920 && videoHeight == 1080 && !interlaced) {
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
		update();
	}

	void setFrameRate3D(uint16_t videoWidth, uint16_t videoHeight, bool interlaced, float frameRate) {
		if (videoWidth <= 720 && videoHeight <= 480) {
			displaySettings.format = NEXUS_VideoFormat_e3D_1080p_24Hz;
		}
		else if (videoWidth <= 1280 && videoHeight <= 720) {
			if (frameRate >= 59)
				displaySettings.format = NEXUS_VideoFormat_e3D_720p;
			else if (frameRate >= 49)
				displaySettings.format = NEXUS_VideoFormat_e3D_720p_50Hz;
			else if (frameRate >= 29)
				displaySettings.format = NEXUS_VideoFormat_e3D_720p_30Hz;
			else if (frameRate >= 23)
				displaySettings.format = NEXUS_VideoFormat_e3D_720p_24Hz;
			else
				displaySettings.format = NEXUS_VideoFormat_e3D_720p;
		}
		else if (videoWidth == 1920 && videoHeight == 1080 && !interlaced) {
			if (frameRate >= 29)
				displaySettings.format = NEXUS_VideoFormat_e3D_1080p_30Hz;
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
		update();
	}
};

class PidChannel {
protected:
	int pid;
	NEXUS_PidChannelHandle pidChannel;

	PidChannel(int pid) :
			pid(pid), pidChannel(NULL) {
	}

public:
	virtual ~PidChannel() {
	}

	virtual operator NEXUS_PidChannelHandle() {
		return pidChannel;
	}
};

class PlaypumpPidChannel: PidChannel {
	friend class Playpump;

	NEXUS_PlaypumpHandle playpump;
	NEXUS_PlaypumpOpenPidChannelSettings *playpumpPidSettings;

	PlaypumpPidChannel(NEXUS_PlaypumpHandle playpump, int pid) :
			PidChannel(pid), playpump(playpump), playpumpPidSettings(NULL) {
	}

public:
	virtual ~PlaypumpPidChannel() {
		if (pidChannel)
			NEXUS_Playpump_ClosePidChannel(playpump, pidChannel);
	}

	NEXUS_PlaypumpOpenPidChannelSettings *settings() {
		if (!playpumpPidSettings) {
			playpumpPidSettings = new NEXUS_PlaypumpOpenPidChannelSettings();
			NEXUS_Playpump_GetDefaultOpenPidChannelSettings(playpumpPidSettings);
		}
		return playpumpPidSettings;
	}

	NEXUS_Error update(NEXUS_PlaypumpOpenPidChannelSettings* settings = NULL) {
		if (settings) {
			if (!playpumpPidSettings)
				playpumpPidSettings = new NEXUS_PlaypumpOpenPidChannelSettings();
			*playpumpPidSettings = *settings;
		}

		if (pidChannel) {
			NEXUS_Playpump_ClosePidChannel(playpump, pidChannel);
			pidChannel = NEXUS_Playpump_OpenPidChannel(playpump, pid, playpumpPidSettings);
		}
		return 0;
	}

	operator NEXUS_PidChannelHandle() {
		if (!pidChannel)
			pidChannel = NEXUS_Playpump_OpenPidChannel(playpump, pid, playpumpPidSettings);
		return pidChannel;
	}
};

class Playpump {
	NEXUS_PlaypumpHandle playpump;
	map<int, PlaypumpPidChannel*> pidChannels;

public:
	Playpump(unsigned index = NEXUS_ANY_ID, NEXUS_PlaypumpOpenSettings *playpumpConfig = NULL) {
		playpump = NEXUS_Playpump_Open(index, playpumpConfig);
		BDBG_ASSERT(playpump);
	}

	virtual ~Playpump() {
		closePids();
		NEXUS_Playpump_Close(playpump);
	}

	operator NEXUS_PlaypumpHandle() {
		return playpump;
	}

	PlaypumpPidChannel* openPid(int pid) {
		if (pidChannels.count(pid) < 1)
			pidChannels[pid] = new PlaypumpPidChannel(*this, pid);
		return pidChannels[pid];
	}

	/**
	 * Need to close pid channels before closing decoder, else will seg fault.
	 */
	void closePids() {
		for (map<int, PlaypumpPidChannel*>::iterator i = pidChannels.begin(); i != pidChannels.end(); ++i)
			delete i->second;
		pidChannels.clear();
	}
};

class File {
	std::string path;
	NEXUS_FilePlayHandle file;

public:
	File(const char* path) :
			path(path), file(NULL) {
	}

	virtual ~File() {
		close();
	}

	operator NEXUS_FilePlayHandle() {
		if (!file)
			open();
		return file;
	}

	bool open() {
		close();
		file = NEXUS_FilePlay_OpenPosix(path.c_str(),
				path.substr(path.length() - 4, 4) == ".mp4" ? path.c_str() : NULL);
		return file != NULL;
	}

	void close() {
		if (file) {
			NEXUS_FilePlay_Close(file);
			file = NULL;
		}
	}
};

class PlaybackPidChannel: PidChannel {
	friend class Playback;

	NEXUS_PlaybackHandle playback;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;

	PlaybackPidChannel(NEXUS_PlaybackHandle playback, int pid) :
			PidChannel(pid), playback(playback) {
		NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
	}

public:
	virtual ~PlaybackPidChannel() {
		if (pidChannel)
			NEXUS_Playback_ClosePidChannel(playback, pidChannel);
	}

	NEXUS_PlaybackPidChannelSettings *settings() {
		return &playbackPidSettings;
	}

	NEXUS_Error update(NEXUS_PlaybackPidChannelSettings* settings = NULL) {
		return NEXUS_Playback_SetPidChannelSettings(playback, *this, settings ? settings : &playbackPidSettings);
	}

	operator NEXUS_PidChannelHandle() {
		if (!pidChannel)
			pidChannel = NEXUS_Playback_OpenPidChannel(playback, pid, &playbackPidSettings);
		return pidChannel;
	}
};

class Playback {
	NEXUS_PlaybackHandle playback;
	NEXUS_PlaybackSettings playbackSettings;
	Playpump pump;
	map<int, PlaybackPidChannel*> pidChannels;
    NEXUS_PlaybackStatus status;

public:
	Playback() :
			playback(NEXUS_Playback_Create()) {
		NEXUS_Playback_GetSettings(playback, &playbackSettings);
		playbackSettings.playpump = pump;
	}

	virtual ~Playback() {
		closePids();
		NEXUS_Playback_Destroy(playback);
	}

	NEXUS_PlaybackSettings *settings() {
		return &playbackSettings;
	}

	NEXUS_Error update(NEXUS_PlaybackSettings *settings = NULL) {
		return NEXUS_Playback_SetSettings(playback, settings ? settings : &playbackSettings);
	}

	operator NEXUS_PlaybackHandle() {
		return playback;
	}

	NEXUS_Error play(File& file, const NEXUS_PlaybackStartSettings *pStartSettings = NULL) {
		update();
		return NEXUS_Playback_Start(playback, file, pStartSettings);
	}

    NEXUS_Error pause() {
        return NEXUS_Playback_Pause(playback);
    }

    NEXUS_PlaybackStatus *getStatus() {
        NEXUS_Playback_GetStatus(playback, &status);
        return &status;
    }

	void stop() {
		NEXUS_Playback_Stop(playback);
	}

	PlaybackPidChannel* openPid(int pid) {
		if (pidChannels.count(pid) < 1)
			pidChannels[pid] = new PlaybackPidChannel(*this, pid);
		return pidChannels[pid];
	}

	/**
	 * Need to close pid channels before closing decoder, else will seg fault.
	 */
	void closePids() {
		for (map<int, PlaybackPidChannel*>::iterator i = pidChannels.begin(); i != pidChannels.end(); ++i)
			delete i->second;
		pidChannels.clear();
	}
};

class VideoDecoder {
	NEXUS_VideoDecoderHandle videoDecoder;
	NEXUS_VideoDecoderSettings videoDecoderSettings;
	NEXUS_VideoDecoderStartSettings startSettings;
	NEXUS_VideoDecoderStatus videoStatus;

	static void sourceChanged(void *data, int unused) {
		BSTD_UNUSED(unused);
		BKNI_SetEvent((BKNI_EventHandle) data);
	}

public:
	VideoDecoder(unsigned index, const NEXUS_VideoDecoderOpenSettings *videoDecoderOpenSettings = NULL) {
		videoDecoder = NEXUS_VideoDecoder_Open(index, videoDecoderOpenSettings);
		NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
		NEXUS_VideoDecoder_GetDefaultStartSettings(&startSettings);
	}

	virtual ~VideoDecoder() {
//		NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
		NEXUS_VideoDecoder_Close(videoDecoder);
	}

	NEXUS_VideoDecoderSettings *settings() {
		return &videoDecoderSettings;
	}

	NEXUS_Error update(NEXUS_VideoDecoderSettings *settings = NULL) {
		return NEXUS_VideoDecoder_SetSettings(videoDecoder, settings ? settings : &videoDecoderSettings);
	}

	operator NEXUS_VideoDecoderHandle() {
		return videoDecoder;
	}

	operator NEXUS_VideoInput() {
		return NEXUS_VideoDecoder_GetConnector(videoDecoder);
	}

	NEXUS_Error start(NEXUS_VideoDecoderStartSettings *pStartSettings = NULL) {
		if (pStartSettings)
			startSettings = *pStartSettings;
		return NEXUS_VideoDecoder_Start(videoDecoder, &startSettings);
	}

	void stop() {
		NEXUS_VideoDecoder_Stop(videoDecoder);
	}

	bool operator !=(Display& display) {
		BKNI_EventHandle sourceChangedEvent;
		BKNI_CreateEvent(&sourceChangedEvent);
		videoDecoderSettings.sourceChanged.callback = sourceChanged;
		videoDecoderSettings.sourceChanged.context = sourceChangedEvent;
		update();

		BERR_Code bc = BKNI_WaitForEvent(sourceChangedEvent, 10 * 1000);
		BKNI_DestroyEvent(sourceChangedEvent);
		videoDecoderSettings.sourceChanged.callback = NULL;
		videoDecoderSettings.sourceChanged.context = NULL;
		update();

		if (bc == BERR_TIMEOUT) {
			cout << "Warning: Timeout (10 sec) waiting for source change event. " << endl;
//			return false; // no source change for 10 sec
		}
		else if (bc == BERR_OS_ERROR) {
			cout << "Warning: wait for source change encountered os error. " << endl;
//			return false;
		}

		NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
		cout << "Decoder format enum: " << videoStatus.format << ". Display format enum: " << display.settings()->format << endl;

		return videoStatus.format != display.settings()->format;
	}

	/*
	 * Adjust display format to video decoder format.
	 * @return: NEXUS_VideoDecoderStatus
	 * @note: Must call != display before calling this function!
	 */
	NEXUS_VideoDecoderStatus adjustFormat(Display& display) {
		if (videoStatus.format != display.settings()->format) {
			display.settings()->format = videoStatus.format;
			display.update();
			cout << "New Display format enum: " << display.settings()->format << endl;
		}
		return videoStatus;
	}

	static NEXUS_VideoFrameRate convert(float frameRate) {
		if (frameRate > 60)
			return NEXUS_VideoFrameRate_eMax;
		else if (frameRate > 59.95)
			return NEXUS_VideoFrameRate_e60;
		else if (frameRate > 50)
			return NEXUS_VideoFrameRate_e59_94;
		else if (frameRate > 30)
			return NEXUS_VideoFrameRate_e50;
		else if (frameRate > 29.98)
			return NEXUS_VideoFrameRate_e30;
		else if (frameRate > 25)
			return NEXUS_VideoFrameRate_e29_97;
		else if (frameRate > 24)
			return NEXUS_VideoFrameRate_e25;
		else if (frameRate > 23.98)
			return NEXUS_VideoFrameRate_e24;
		else if (frameRate > 14.99)
			return NEXUS_VideoFrameRate_e23_976;
		else if (frameRate > 7.95)
			return NEXUS_VideoFrameRate_e14_985;
		else
			return NEXUS_VideoFrameRate_eUnknown;
	}
};

class AudioDecoder {
	NEXUS_AudioDecoderHandle audioDecoder;
	NEXUS_AudioDecoderStartSettings startSettings;
	NEXUS_AudioDecoderSettings *audioDecoderSettings; // use pointer here because we don't always need to change the settings
	map<NEXUS_AudioConnectorType, NEXUS_AudioOutput> outputs;

public:
	AudioDecoder(unsigned index, const NEXUS_AudioDecoderOpenSettings *audioDecoderOpenSettings = NULL) :
			audioDecoderSettings(NULL) {
		audioDecoder = NEXUS_AudioDecoder_Open(index, audioDecoderOpenSettings);
		NEXUS_AudioDecoder_GetDefaultStartSettings(&startSettings);
	}

	virtual ~AudioDecoder() {
		if (audioDecoderSettings)
			delete audioDecoderSettings;
		disconnect();
		NEXUS_AudioDecoder_Close(audioDecoder);
	}

	NEXUS_AudioDecoderSettings *settings() {
		if (!audioDecoderSettings) {
			audioDecoderSettings = new NEXUS_AudioDecoderSettings();
			NEXUS_AudioDecoder_GetSettings(audioDecoder, audioDecoderSettings);
		}
		return audioDecoderSettings;
	}

	NEXUS_Error update(NEXUS_AudioDecoderSettings *settings = NULL) {
		if (!settings && !audioDecoderSettings)
			return 0;
		return NEXUS_AudioDecoder_SetSettings(audioDecoder, settings ? settings : audioDecoderSettings);
	}

	operator NEXUS_AudioDecoderHandle() {
		return audioDecoder;
	}

	NEXUS_Error start(NEXUS_AudioDecoderStartSettings *pStartSettings = NULL) {
		if (pStartSettings)
			startSettings = *pStartSettings;
		if (startSettings.pidChannel)
			return NEXUS_AudioDecoder_Start(audioDecoder, &startSettings);
		return 0;
	}

	void stop() {
		if (startSettings.pidChannel)
			NEXUS_AudioDecoder_Stop(audioDecoder);
	}

	NEXUS_AudioInput operator[](NEXUS_AudioConnectorType connectorType) {
		if (outputs.count(connectorType) == 0)
			outputs[connectorType] = NULL;
		return NEXUS_AudioDecoder_GetConnector(audioDecoder, connectorType);
	}

    NEXUS_Error connect(NEXUS_AudioOutput output, NEXUS_AudioConnectorType connectorType, NEXUS_AudioInput *audioConnector = NULL)
	{
		if (outputs.count(connectorType) > 0 && outputs[connectorType]) {
			NEXUS_AudioOutput_RemoveInput(outputs[connectorType], NEXUS_AudioDecoder_GetConnector(audioDecoder, connectorType));
		}
        outputs[connectorType] = output;
		
		if(audioConnector)
			return NEXUS_AudioOutput_AddInput(output, *audioConnector);
		else
			return NEXUS_AudioOutput_AddInput(output, NEXUS_AudioDecoder_GetConnector(audioDecoder, connectorType));
	}

	void disconnect() {
		for (map<NEXUS_AudioConnectorType, NEXUS_AudioOutput>::iterator i = outputs.begin(); i != outputs.end(); ++i) {
			if (i->second) {
//				cout << "Removing input " << i->first << " from audio decoder." << endl;
				NEXUS_AudioOutput_RemoveInput(i->second, NEXUS_AudioDecoder_GetConnector(audioDecoder, i->first));
			}
//			cout << "Shutdown input " << i->first << " from audio decoder." << endl;
			NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, i->first));
			if (i->second)
				NEXUS_AudioOutput_Shutdown(i->second);
		}
		outputs.clear();
	}
};

class AudioCapture {
	NEXUS_AudioCaptureHandle capture;
	NEXUS_AudioInput input;

public:
	AudioCapture(unsigned index = 0, const NEXUS_AudioCaptureOpenSettings *pSettings = NULL) :
			input(NULL) {
		capture = NEXUS_AudioCapture_Open(index, pSettings);
	}

	virtual ~AudioCapture() {
		disconnect();
		NEXUS_AudioCapture_Close(capture);
	}

	operator NEXUS_AudioCaptureHandle() {
		return capture;
	}

	operator NEXUS_AudioOutput() {
		return NEXUS_AudioCapture_GetConnector(capture);
	}

	NEXUS_Error start(const NEXUS_AudioCaptureStartSettings *pSettings) {
		return NEXUS_AudioCapture_Start(capture, pSettings);
	}

	void stop() {
		NEXUS_StopCallbacks(capture);
		NEXUS_AudioCapture_Stop(capture);
	}

	NEXUS_Error connect(NEXUS_AudioInput input) {
		if (this->input)
			NEXUS_AudioOutput_RemoveInput(*this, this->input);
		this->input = input;
		return NEXUS_AudioOutput_AddInput(*this, input);
	}

	NEXUS_Error disconnect() {
		NEXUS_Error rc = 0;
		if (input) {
			rc = NEXUS_AudioOutput_RemoveInput(*this, input);
			input = NULL;
		}
		return rc;
	}
};

#if NEXUS_HAS_STREAM_MUX
class VideoEncoder {
	NEXUS_VideoEncoderHandle videoEncoder;
	NEXUS_VideoEncoderSettings videoEncoderSettings;
	NEXUS_VideoEncoderStartSettings startSettings;

public:
	VideoEncoder(unsigned index, const NEXUS_VideoEncoderOpenSettings *videoEncoderOpenSettings = NULL) {
		videoEncoder = NEXUS_VideoEncoder_Open(index, videoEncoderOpenSettings);
		NEXUS_VideoEncoder_GetSettings(videoEncoder, &videoEncoderSettings);
		NEXUS_VideoEncoder_GetDefaultStartSettings(&startSettings);
	}

	virtual ~VideoEncoder() {
		NEXUS_VideoEncoder_Close(videoEncoder);
	}

	NEXUS_VideoEncoderSettings *settings() {
		return &videoEncoderSettings;
	}

	NEXUS_Error update(NEXUS_VideoEncoderSettings *settings = NULL) {
		return NEXUS_VideoEncoder_SetSettings(videoEncoder, settings ? settings : &videoEncoderSettings);
	}

	operator NEXUS_VideoEncoderHandle() {
		return videoEncoder;
	}

	NEXUS_Error start(NEXUS_VideoEncoderStartSettings *pStartSettings = NULL) {
		if (pStartSettings)
			startSettings = *pStartSettings;
		return NEXUS_VideoEncoder_Start(videoEncoder, &startSettings);
	}

	void stop() {
		NEXUS_VideoEncoder_Stop(videoEncoder, NULL);
	}
};

class AudioEncoder {
	NEXUS_AudioEncoderHandle audioEncoder;
	NEXUS_AudioEncoderSettings encoderSettings;
	NEXUS_AudioInput input;
	NEXUS_AudioInput output; // output from this encoder, which is an input to mux, etc. Need to shut it down because we created it.

public:
	AudioEncoder() :
			audioEncoder(NULL), input(NULL), output(NULL) {
		NEXUS_AudioEncoder_GetDefaultSettings(&encoderSettings);
	}

	virtual ~AudioEncoder() {
		if (audioEncoder) {
			disconnect();
			NEXUS_AudioEncoder_Close(audioEncoder);
		}
	}

	NEXUS_AudioEncoderSettings *settings() {
		return &encoderSettings;
	}

	NEXUS_Error update(NEXUS_AudioEncoderSettings *settings = NULL) {
		return NEXUS_AudioEncoder_SetSettings(*this, settings ? settings : &encoderSettings);
	}

	operator NEXUS_AudioEncoderHandle() {
		if (!audioEncoder)
			audioEncoder = NEXUS_AudioEncoder_Open(&encoderSettings);
		return audioEncoder;
	}

	operator NEXUS_AudioInput() {
		if (!output)
			output = NEXUS_AudioEncoder_GetConnector(*this);
		return output;
	}

	NEXUS_Error connect(NEXUS_AudioInput input) {
		if (this->input)
			NEXUS_AudioEncoder_RemoveInput(audioEncoder, this->input);
		this->input = input;
		return NEXUS_AudioEncoder_AddInput(*this, input);
	}

	NEXUS_Error disconnect() {
		NEXUS_Error rc = 0;
		if (input) {
//			cout << "Removing input " << input << " from audio encoder." << endl;
			rc = NEXUS_AudioEncoder_RemoveInput(audioEncoder, input);
			input = NULL;
		}
		if (output) {
//			cout << "Shutdown output " << output << " from AudioEncoder." << endl;
			NEXUS_AudioInput_Shutdown(output);
			output = NULL;
		}
		return rc;
	}
};

class AudioMuxOutput {
	NEXUS_AudioMuxOutputHandle audioMuxOutput;
	NEXUS_AudioInput input;

public:
	AudioMuxOutput(const NEXUS_AudioMuxOutputCreateSettings *pSettings = NULL) :
			input(NULL) {
		audioMuxOutput = NEXUS_AudioMuxOutput_Create(pSettings);
	}

	virtual ~AudioMuxOutput() {
		disconnect();
		NEXUS_AudioMuxOutput_Destroy(audioMuxOutput);
	}

	operator NEXUS_AudioMuxOutputHandle() {
		return audioMuxOutput;
	}

	NEXUS_Error start(const NEXUS_AudioMuxOutputStartSettings *pSettings = NULL) {
		return NEXUS_AudioMuxOutput_Start(audioMuxOutput, pSettings);
	}

	void stop() {
		NEXUS_AudioMuxOutput_Stop(audioMuxOutput);
	}

	operator NEXUS_AudioOutput() {
		return NEXUS_AudioMuxOutput_GetConnector(audioMuxOutput);
	}

	NEXUS_Error connect(NEXUS_AudioInput input) {
		if (this->input)
			NEXUS_AudioOutput_RemoveInput(*this, this->input);
		this->input = input;
		return NEXUS_AudioOutput_AddInput(*this, input);
	}

	NEXUS_Error disconnect() {
		NEXUS_Error rc = 0;
		if (input) {
//			cout << "Removing input " << input << " from audioMuxOutput." << endl;
			rc = NEXUS_AudioOutput_RemoveInput(*this, input);
			NEXUS_AudioOutput_Shutdown(*this);
			input = NULL;
		}
		return rc;
	}
};

class StreamMux {
	NEXUS_StreamMuxHandle streamMux;
	NEXUS_StreamMuxOutput muxOutput;
	BKNI_EventHandle finishEvent;
	NEXUS_StreamMuxCreateSettings createSettings;

	static void transcoderFinishCallback(void *context, int param) {
		BKNI_EventHandle finishEvent = (BKNI_EventHandle) context;
		BSTD_UNUSED(param);
//		BDBG_WRN(("Finish callback invoked, now stop the stream mux."));
		BKNI_SetEvent(finishEvent);
	}

public:
	StreamMux(NEXUS_StreamMuxCreateSettings *pSettings = NULL) {
		BKNI_CreateEvent(&finishEvent);
		if (!pSettings) {
			NEXUS_StreamMux_GetDefaultCreateSettings(&createSettings);
			pSettings = &createSettings;
		}
		pSettings->finished.callback = transcoderFinishCallback;
		pSettings->finished.context = finishEvent;
		streamMux = NEXUS_StreamMux_Create(pSettings);
	}

	virtual ~StreamMux() {
		BKNI_DestroyEvent(finishEvent);
		NEXUS_StreamMux_Destroy(streamMux);
	}

	operator NEXUS_StreamMuxHandle() {
		return streamMux;
	}

	NEXUS_Error start(const NEXUS_StreamMuxStartSettings *pSettings) {
		return NEXUS_StreamMux_Start(streamMux, pSettings, &muxOutput);
	}

	NEXUS_StreamMuxOutput* output() {
		return &muxOutput;
	}

	void waitForFinish(int timeoutMsec) {
		NEXUS_StreamMux_Finish(streamMux);

		BERR_Code bc = BKNI_WaitForEvent(finishEvent, timeoutMsec);
		if (bc == BERR_TIMEOUT)
			cout << "Warning: stream mux not finished within " << timeoutMsec / 1000 << " seconds. " << endl;
		else if (bc == BERR_OS_ERROR)
			cout << "Warning: wait for stream mux finish encountered os error. " << endl;
	}

	void stop() {
		NEXUS_StreamMux_Stop(streamMux);
	}
};

class Recpump {
	NEXUS_RecpumpHandle recpump;

public:
	Recpump(unsigned index = NEXUS_ANY_ID, const NEXUS_RecpumpOpenSettings *pSettings = NULL) {
		recpump = NEXUS_Recpump_Open(index, pSettings);
	}

	virtual ~Recpump() {
		NEXUS_Recpump_Close(recpump);
	}

	operator NEXUS_RecpumpHandle() {
		return recpump;
	}
};

class Record {
	NEXUS_RecordHandle record;
	Recpump *pRecpump;
	NEXUS_RecordSettings recordSettings;

public:
	Record(NEXUS_RecpumpHandle recpump = NULL) :
			record(NEXUS_Record_Create()), pRecpump(NULL) {
		NEXUS_Record_GetSettings(record, &recordSettings);
		if (!recpump) {
			pRecpump = new Recpump();
			recordSettings.recpump = *pRecpump;
		}
		else
			recordSettings.recpump = recpump;
		update();
	}

	virtual ~Record() {
		NEXUS_Record_Destroy(record);
		if (pRecpump)
			delete pRecpump;
	}

	operator NEXUS_RecordHandle() {
		return record;
	}

	NEXUS_RecordSettings* settings() {
		return &recordSettings;
	}

	NEXUS_Error update() {
		return NEXUS_Record_SetSettings(record, &recordSettings);
	}

	NEXUS_Error addPid(NEXUS_PidChannelHandle pidChannel, const NEXUS_RecordPidChannelSettings *pSettings = NULL) {
		return NEXUS_Record_AddPidChannel(record, pidChannel, pSettings);
	}

	void closePids() {
		NEXUS_Record_RemoveAllPidChannels(record);
	}

	NEXUS_Error start(NEXUS_FileRecordHandle fileTranscode) {
		update();
		return NEXUS_Record_Start(record, fileTranscode);
	}

	void stop() {
		NEXUS_Record_Stop(record);
	}
};
#endif

#if NEXUS_HAS_SYNC_CHANNEL
class SyncChannel {
	NEXUS_SyncChannelSettings syncChannelSettings;
	NEXUS_SyncChannelHandle syncChannel;
public:
	SyncChannel() {
		NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
		syncChannel = NEXUS_SyncChannel_Create(&syncChannelSettings);
	}

	virtual ~SyncChannel() {
		if (syncChannel)
			NEXUS_SyncChannel_Destroy(syncChannel);
	}

	operator NEXUS_SyncChannelHandle() {
		return syncChannel;
	}

	NEXUS_SyncChannelSettings* settings() {
		return &syncChannelSettings;
	}

	NEXUS_Error update() {
		if (syncChannel)
			return NEXUS_SyncChannel_SetSettings(syncChannel, &syncChannelSettings);
		return NEXUS_NOT_INITIALIZED;
	}
};
#endif

}
}
}
#endif
