/***************************************************************************
 *     (c)2008-2011 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: NexusCrc.cpp $
 * $brcm_Revision: 12 $
 * $brcm_Date: 11/3/11 4:30p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/diagnostic/crc/NexusCrc.cpp $
 * 
 * 12   11/3/11 4:30p jliang
 * SW7346-463: Made capture function public.
 * 
 * 11   11/1/11 10:20a jliang
 * SW7346-463: Change how CRC is enabled.
 * 
 * 10   10/13/11 12:47p jliang
 * SW7346-463: Add compare luma only flag.
 * 
 * 9   6/17/11 1:27p jliang
 * SW7346-103: Remove an un-needed function and changed search start to
 *  10% of reference crc size.
 * 
 * 8   6/2/11 1:26p jliang
 * SW7346-103: Make parameter length const.
 * 
 * 7   6/2/11 1:21p jliang
 * SW7346-103: Added AddReference array function.
 * 
 * 6   5/25/11 12:31p jliang
 * SW7346-103: Added AddReference function.
 * 
 * 5   4/14/11 2:19p jliang
 * SW7346-103: Changed ReadReference to return false if the ref. file
 *  cannot be opened.
 * 
 * 4   4/1/11 8:13p jliang
 * SW7346-103: Reset stop to false before start the thread.
 * 
 * 3   4/1/11 6:08p jliang
 * SW7346-103: add back isCaptureDone.
 * 
 * 2   4/1/11 5:46p jliang
 * SW7346-103: Remove display module init because cannot do it here.
 * 
 * 1   4/1/11 5:04p jliang
 * SW7346-103: Adding wrapper class NexusCRC and crc.h.
 * 
 *  NexusCrc.cpp
 *
 *  Created on: Mar 30, 2011
 ***************************************************************************/
#include <pthread.h>

#include "nexus_display.h"
#include "nexus_platform_features.h"
#include "nexus_gpio.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_hdmi_output.h"
#include "nexus_video_input_crc.h"

#include "bstd.h"
#include "bkni.h"

#include "NexusCrc.h"
#include "ReferenceCrc.h"

using namespace std;

namespace broadcom {
namespace broadband {
namespace diagnostic {

void* NexusCrc::start_capture(void* arg) {
	NexusCrc* crc = (NexusCrc*) (((arg)));
	crc->Capture();
	return 0;
}

NexusCrc::NexusCrc(bool autoDisable) :
		ref(new ReferenceCrc), thread(0), stop(false), callback(NULL), callbackContext(NULL), autoDisable(autoDisable) {
}

NexusCrc::~NexusCrc() {
	delete ref;
}

bool NexusCrc::ReadReference(const char* referenceFile) {
	return ref->ReadReference(referenceFile);
}

void NexusCrc::SetRecordFrames(const size_t numFrames) {
	ref->SetRecordFrames(numFrames);
}

void NexusCrc::SetCompareLumaOnly(bool value) const {
	if (value)
		cout << "\nComparing Luma only.\n";

	ref->CompareLumaOnly = value;
}
pthread_t NexusCrc::Start(void (*finishCallback)(void*), void* context) {
	callback = finishCallback;
	this->callbackContext = context;
	stop = false;
	pthread_create(&thread, NULL, start_capture, this);
	return thread;
}

void NexusCrc::Wait() {
	pthread_join(thread, NULL);
}

bool NexusCrc::IsCaptureDone() const {
	return ref->IsCaptureDone();
}

bool NexusCrc::Pass() {
	return ref->Pass();
}

string NexusCrc::Result(ostream& details) {
	return ref->Result(details);
}

void NexusCrc::DumpCaptured(const char* dumpFile) const {
	return ref->DumpCaptured(dumpFile);
}

NexusDisplayCrc::NexusDisplayCrc(void* display_handle, bool autoEnable, bool autoDisable, void* videoDecoder) :
		NexusCrc(autoDisable), display(display_handle), videoDecoder(videoDecoder) {
	cout << "\nGot nexus_display_handle = " << display << endl;
	if (autoEnable) {
		NEXUS_DisplaySettings displaySettings;
		NEXUS_Display_GetSettings((NEXUS_DisplayHandle)(((display))), &displaySettings);
		displaySettings.crcQueueSize = 600; /* This enables the CRC capture */
		NEXUS_Display_SetSettings((NEXUS_DisplayHandle)(((display))), &displaySettings);
	}
}

void NexusDisplayCrc::disable() {
	NEXUS_DisplaySettings displaySettings;
	NEXUS_Display_GetSettings((NEXUS_DisplayHandle)(((display))), &displaySettings);
	displaySettings.crcQueueSize = 0; /* This disables the CRC capture */
	NEXUS_Display_SetSettings((NEXUS_DisplayHandle)(((display))), &displaySettings);
}

NexusDisplayCrc::~NexusDisplayCrc() {
	if (autoDisable)
		disable();
}
void NexusDisplayCrc::AddReference(const void* data, const size_t length) {
	NEXUS_DisplayCrcData* crc = (NEXUS_DisplayCrcData*) (((data)));
	unsigned *luma = new unsigned[length];
	unsigned *cb = new unsigned[length];
	unsigned *cr = new unsigned[length];
	for (size_t i = 0; i < length; i++) {
		luma[i] = crc[i].cmp.luma;
		cb[i] = crc[i].cmp.cb;
		cr[i] = crc[i].cmp.cr;
	}
	ref->AddReference(luma, cb, cr, length);
	delete[] luma;
	delete[] cb;
	delete[] cr;
}

void NexusDisplayCrc::Capture() {
	NEXUS_DisplayCrcData data[16];
	unsigned num, i;
	//NEXUS_KEYPAD_AON_GPIO;
	do {
		NEXUS_Display_GetCrcData((NEXUS_DisplayHandle)(((display))), data, 16, &num);
		if (!num) {
			BKNI_Sleep(1000);
			continue;
		}
		for (i = 0; i < num; i++)
			ref->AddCaptured(data[i].cmp.luma, data[i].cmp.cb, data[i].cmp.cr);
	}
	while (!ref->IsCaptureDone() && !stop);
	if (callback)
		callback (callbackContext);

	if (autoDisable)
		disable();

	// clear the queue
	NEXUS_Error rc;
	do {
		rc = NEXUS_Display_GetCrcData((NEXUS_DisplayHandle) (((display))), data, 16, &num);
	}
	while (rc == NEXUS_SUCCESS && num);
}
NexusVideoDecoderCrc::NexusVideoDecoderCrc(void* videoDecoder, bool autoEnable, bool autoDisable) :
		NexusCrc(autoDisable), videoDecoder(videoDecoder) {
	cout << "\nGot videoDecoder = " << videoDecoder << endl;
	if (autoEnable) {
		NEXUS_VideoDecoderExtendedSettings settings;
		NEXUS_VideoDecoder_GetExtendedSettings((NEXUS_VideoDecoderHandle)(((videoDecoder))), &settings);
		settings.crcFifoSize = 300;
		NEXUS_VideoDecoder_SetExtendedSettings((NEXUS_VideoDecoderHandle)(((videoDecoder))), &settings);
	}
}

NexusVideoDecoderCrc::~NexusVideoDecoderCrc() {
	if (autoDisable)
		disable();
}
void NexusVideoDecoderCrc::disable() {
	NEXUS_VideoDecoderExtendedSettings settings;
	NEXUS_VideoDecoder_GetExtendedSettings((NEXUS_VideoDecoderHandle)(((videoDecoder))), &settings);
	settings.crcFifoSize = 0;
	NEXUS_VideoDecoder_SetExtendedSettings((NEXUS_VideoDecoderHandle)(((videoDecoder))), &settings);
}

void NexusVideoDecoderCrc::Capture() {
	NEXUS_VideoDecoderCrc data[16];
	unsigned num, i;
	do {
		NEXUS_VideoDecoder_GetCrcData((NEXUS_VideoDecoderHandle)(((videoDecoder))), data, 16, &num);
		if (!num) {
			BKNI_Sleep(1000);
			continue;
		}
		for (i = 0; i < num; i++)
			ref->AddCaptured(data[i].top.luma, data[i].top.cb, data[i].top.cr, data[i].bottom.luma, data[i].bottom.cb,
					data[i].bottom.cr);
	}
	while (!ref->IsCaptureDone() && !stop);
	if (callback)
		callback (callbackContext);

	if (autoDisable)
		disable();

	// clear the queue
	NEXUS_Error rc;
	do {
		rc = NEXUS_VideoDecoder_GetCrcData((NEXUS_VideoDecoderHandle)(((videoDecoder))), data, 16, &num);
	}
	while (rc == NEXUS_SUCCESS && num);
}

NexusHdmiCrc::NexusHdmiCrc(void* hdmiOutputHandle, bool autoEnable, bool autoDisable) :
		NexusCrc(autoDisable), hdmiOutput(hdmiOutputHandle) {
	cout << "\nGot HDMI output handle = " << hdmiOutput << endl;
	ref->CompareLumaOnly = true;
	if (autoEnable) {
		NEXUS_HdmiOutputSettings settings;
		NEXUS_HdmiOutput_GetSettings((NEXUS_HdmiOutputHandle)(((hdmiOutput))), &settings);
		settings.crcQueueSize = 300;
		NEXUS_HdmiOutput_SetSettings((NEXUS_HdmiOutputHandle)(((hdmiOutput))), &settings);
	}
}

NexusHdmiCrc::~NexusHdmiCrc() {
	if (autoDisable)
		disable();
}

void NexusHdmiCrc::disable() {
	NEXUS_HdmiOutputSettings settings;
	NEXUS_HdmiOutput_GetSettings((NEXUS_HdmiOutputHandle)(((hdmiOutput))), &settings);
	settings.crcQueueSize = 0;
	NEXUS_HdmiOutput_SetSettings((NEXUS_HdmiOutputHandle)(((hdmiOutput))), &settings);
}

void NexusHdmiCrc::Capture() {
	NEXUS_HdmiOutputCrcData data[16];
	unsigned num, i;
	do {
		NEXUS_HdmiOutput_GetCrcData((NEXUS_HdmiOutputHandle)(((hdmiOutput))), data, 16, &num);
		if (!num) {
			BKNI_Sleep(1000);
			continue;
		}
		for (i = 0; i < num; i++)
			ref->AddCaptured(data[i].crc, 0, 0);
	}
	while (!ref->IsCaptureDone() && !stop);
	if (callback)
		callback (callbackContext);

	if (autoDisable)
		disable();

	// clear the queue
	NEXUS_Error rc;
	do {
		rc = NEXUS_HdmiOutput_GetCrcData((NEXUS_HdmiOutputHandle)(((hdmiOutput))), data, 16, &num);
	}
	while (rc == NEXUS_SUCCESS && num);
}

NexusVideoInputCrc::NexusVideoInputCrc(void* videoInputHandle, bool autoEnable, bool autoDisable) :
		NexusCrc(autoDisable), videoInput(videoInputHandle) {
	cout << "\nGot Video Input handle = " << videoInput << endl;
	if (autoEnable) {
//		NEXUS_HdmiOutputSettings settings;
//		NEXUS_HdmiOutput_GetSettings((NEXUS_VideoInput)(((videoInputHandle))), &settings);
//		settings.crcQueueSize = 300;
//		NEXUS_HdmiOutput_SetSettings((NEXUS_VideoInput)(((videoInputHandle))), &settings);
	}
}

NexusVideoInputCrc::~NexusVideoInputCrc() {
	if (autoDisable)
		disable();
}

void NexusVideoInputCrc::disable() {
//	NEXUS_HdmiOutputSettings settings;
//	NEXUS_HdmiOutput_GetSettings((NEXUS_VideoInput)(((videoInputHandle))), &settings);
//	settings.crcQueueSize = 0;
//	NEXUS_HdmiOutput_SetSettings((NEXUS_VideoInput)(((videoInputHandle))), &settings);
}

void NexusVideoInputCrc::Capture() {
	NEXUS_VideoInputCrcData data[16];
	unsigned num, i;
	do {
		NEXUS_VideoInput_GetCrcData((NEXUS_VideoInput)(((videoInput))), data, 16, &num);
		if (!num) {
			BKNI_Sleep(1000);
			continue;
		}
		for (i = 0; i < num; i++)
			ref->AddCaptured(data[i].lumaCrc, data[i].chromaCrc, data[i].lumaCrc, data[i].chromaCrc);
	}
	while (!ref->IsCaptureDone() && !stop);
	if (callback)
		callback (callbackContext);

	if (autoDisable)
		disable();

	// clear the queue
	NEXUS_Error rc;
	do {
		rc = NEXUS_VideoInput_GetCrcData((NEXUS_VideoInput)(((videoInput))), data, 16, &num);
	}
	while (rc == NEXUS_SUCCESS && num);
}

NexusAudioDecoderCrc::NexusAudioDecoderCrc(void* audioDecoder, bool autoEnable, bool autoDisable,
		const unsigned ringBufferSize) :
		NexusCrc(autoDisable), audioDecoder(audioDecoder), ringBufferSize(ringBufferSize), computed(0), crc(0) {
	ref->CompareLumaOnly = true;
	cout << "\nGot audioDecoder = " << audioDecoder << ". Ring buffer size: " << this->ringBufferSize << endl;
}

NexusAudioDecoderCrc::~NexusAudioDecoderCrc() {
}

void NexusAudioDecoderCrc::Capture() {
	//	NEXUS_VideoDecoderCrc data[16];
	//	unsigned num, i;
	//
	do {
		//		NEXUS_VideoDecoder_GetCrcData((NEXUS_VideoDecoderHandle) videoDecoder, data, 16, &num);
		//		if (!num) {
		BKNI_Sleep(1000);
		//			continue;
		//		}
		//
		//		for (i = 0; i < num; i++)
		//			ref->AddCaptured(data[i].top.luma, data[i].top.cb, data[i].top.cr, data[i].bottom.luma, data[i].bottom.cb, data[i].bottom.cr);
		//
	}
	while (!ref->IsCaptureDone() && !stop);
	//
	if (callback)
		callback (callbackContext);
	//
	//	if (autoDisable) {
	//	    NEXUS_VideoDecoderExtendedSettings settings;
	//		NEXUS_VideoDecoder_GetExtendedSettings((NEXUS_VideoDecoderHandle) videoDecoder, &settings);
	//		settings.crcFifoSize = 0;
	//		NEXUS_VideoDecoder_SetExtendedSettings((NEXUS_VideoDecoderHandle) videoDecoder, &settings);
	//	}
}
unsigned short crc16(const unsigned char message, unsigned short crc = 0) {
	// x^16 + x^15 + x^2 + 1 lookup table
	static int table[] = { 0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780,
			0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
			0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00,
			0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1,
			0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081,
			0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740,
			0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01,
			0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0,
			0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580,
			0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
			0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501,
			0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0,
			0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80,
			0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541,
			0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 0x5000,
			0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1,
			0x9481, 0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81,
			0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841, 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
			0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701,
			0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
	        };
	return (crc >> 8) ^ table[(crc ^ message) & 0xff];
}

void NexusAudioDecoderCrc::AddCaptured(const unsigned char* data, const unsigned length) {
	for (unsigned i = 0; !ref->IsCaptureDone() && !stop; i++, computed++) {
		if (computed == ringBufferSize) {
			ref->AddCaptured(crc, 0, 0);
			computed = 0;
			crc = 0;
		}
		if (i < length)
			crc = crc16(data[i], crc);
		else
			break;
	}
}

}
}
}
