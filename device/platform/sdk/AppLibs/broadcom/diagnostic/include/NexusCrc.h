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
 * $brcm_Workfile: NexusCrc.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 11/3/11 4:29p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/diagnostic/include/NexusCrc.h $
 * 
 * 11   11/3/11 4:29p jliang
 * SW7346-463: Made capture function public.
 * 
 * 10   11/1/11 10:20a jliang
 * SW7346-463: Change how CRC is enabled.
 * 
 * 9   10/13/11 12:47p jliang
 * SW7346-463: Add compare luma only flag.
 * 
 * 8   6/17/11 1:27p jliang
 * SW7346-103: Remove an un-needed function and changed search start to
 *  10% of reference crc size.
 * 
 * 7   6/2/11 1:26p jliang
 * SW7346-103: Make parameter length const.
 * 
 * 6   6/2/11 1:21p jliang
 * SW7346-103: Added AddReference array function.
 * 
 * 5   5/25/11 12:31p jliang
 * SW7346-103: Added AddReference function.
 * 
 * 4   4/14/11 2:22p jliang
 * SW7346-103: Check if reference crc exists or not, if not, return false.
 * 
 * 3   4/1/11 6:08p jliang
 * SW7346-103: add back isCaptureDone.
 * 
 * 2   4/1/11 5:46p jliang
 * SW7346-103: Remove display module init because cannot do it here.
 * 
 * 1   4/1/11 5:03p jliang
 * SW7346-103: Adding wrapper class NexusCRC and crc.h.
 * 
 *  NexusCrc.h
 *
 *  Created on: Mar 30, 2011
 ***************************************************************************/

#ifndef NEXUSCRC_H_
#define NEXUSCRC_H_

#include <string>
#include <sstream>
#include <iostream>

/*
 *
 */
namespace broadcom {
namespace broadband {
namespace diagnostic {

class ReferenceCrc;

class NexusCrc {
protected:
	ReferenceCrc* ref;
	pthread_t thread;
	bool stop;
	void (*callback)(void*);
	void *callbackContext;
	bool autoDisable;

public:
	NexusCrc(bool autoDisable = true);
	virtual ~NexusCrc();

public:
	/*
	 * ReadReference returns false if the file cannot be opened.
	 */
	bool ReadReference(const char* referenceFile);
	void SetRecordFrames(const size_t numFrames);
	void SetCompareLumaOnly(bool value) const;

	virtual void Capture() = 0;
	pthread_t Start(void (*finishCallback)(void*) = NULL, void* context = NULL);
	void Wait();
	void Stop() { stop = true; }
	bool IsCaptureDone() const;

	bool Pass();
	std::string Result(std::ostream& details = std::cout);
	void DumpCaptured(const char* dumpFile) const;

private:
	static void* start_capture(void* arg);
};

class NexusDisplayCrc : public NexusCrc {
	void* display;
	void* videoDecoder;
	void disable();

public:
	NexusDisplayCrc(void* display_handle, bool autoEnable = true, bool autoDisable = true, void* videoDecoder = NULL);
	virtual ~NexusDisplayCrc();

public:
	void AddReference(const void* data, const size_t length);
	void Capture();
};

class NexusVideoDecoderCrc : public NexusCrc {
	void* videoDecoder;
	void disable();

public:
	NexusVideoDecoderCrc(void* videoDecoder, bool autoEnable = true, bool autoDisable = true);
	virtual ~NexusVideoDecoderCrc();

public:
	void Capture();
};

class NexusHdmiCrc : public NexusCrc {
	void* hdmiOutput;
	void disable();

public:
	NexusHdmiCrc(void* hdmiOutputHandle, bool autoEnable = true, bool autoDisable = true);
	virtual ~NexusHdmiCrc();

public:
	void Capture();
};

class NexusVideoInputCrc : public NexusCrc {
	void* videoInput;
	void disable();

public:
	NexusVideoInputCrc(void* videoInputHandle, bool autoEnable = true, bool autoDisable = true);
	virtual ~NexusVideoInputCrc();

public:
	void Capture();
};

class NexusAudioDecoderCrc : public NexusCrc {
	void* audioDecoder;
	unsigned ringBufferSize;

	unsigned computed;
	unsigned short crc;

public:
	NexusAudioDecoderCrc(void* audioDecoder, bool autoEnable = true, bool autoDisable = true, const unsigned ringBufferSize = 8192);
	virtual ~NexusAudioDecoderCrc();

public:
	void Capture();
	void AddCaptured(const unsigned char* data, const unsigned length);
};

}
}
}

#endif /* NEXUSCRC_H_ */
