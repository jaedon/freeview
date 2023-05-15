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
 * $brcm_Workfile: ReferenceCrc.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 10/13/11 12:47p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/diagnostic/crc/ReferenceCrc.h $
 * 
 * 11   10/13/11 12:47p jliang
 * SW7346-463: Add compare luma only flag.
 * 
 * 10   6/17/11 1:27p jliang
 * SW7346-103: Remove an un-needed function and changed search start to
 *  10% of reference crc size.
 * 
 * 9   6/2/11 1:26p jliang
 * SW7346-103: Make parameter length const.
 * 
 * 8   6/2/11 1:21p jliang
 * SW7346-103: Added AddReference array function.
 * 
 * 7   5/25/11 12:31p jliang
 * SW7346-103: Added AddReference function.
 * 
 * 6   4/14/11 2:27p jliang
 * SW7346-103: Check if reference crc exists or not, if not, return false.
 * 
 * 5   4/7/11 4:39p jliang
 * SW7346-103: Added work around for a bug in diff algorithm. Need to find
 *  and fix the bug!!!
 * 
 * 4   4/1/11 5:46p jliang
 * SW7346-103: Remove display module init because cannot do it here.
 * 
 * 3   4/1/11 5:04p jliang
 * SW7346-103: Creating libcrc-nexus.so.
 * 
 * 2   3/30/11 5:07p jliang
 * SW7346-103: enhanced diff result interpretation.
 * 
 * 1   3/29/11 12:36p jliang
 * SW7346-103: adding CRC app source code.
 * 
 *  ReferenceCrc.h
 *
 *  Created on: Mar 17, 2011
 ***************************************************************************/

#ifndef REFERENCECRC_H_
#define REFERENCECRC_H_

#include <stdio.h>

#include <vector>
#include <queue>
#include <string>
#include <iostream>

using namespace std;

namespace broadcom {
namespace broadband {
namespace diagnostic {
/*
 *
 */
class ReferenceCrc {
	typedef struct Crc {
		uint32_t luma;
		uint32_t cb;
		uint32_t cr;
		bool compareLumaOnly;
		bool noCr;

		Crc() :
			luma(0), cb(0), cr(0), compareLumaOnly(false), noCr(false) {
		}

		Crc(uint32_t luma, uint32_t cb, uint32_t cr) :
			luma(luma), cb(cb), cr(cr), compareLumaOnly(false), noCr(false) {
		}

		Crc(uint32_t luma, uint32_t cb, uint32_t cr, bool compareLumaOnly) :
			luma(luma), cb(cb), cr(cr), compareLumaOnly(compareLumaOnly), noCr(false) {
		}

//		Crc(uint32_t luma, uint32_t cb, uint32_t cr, bool compareLumaOnly, bool noCr) :
//			luma(luma), cb(cb), cr(cr), compareLumaOnly(compareLumaOnly), noCr(noCr) {
//		}

		Crc(uint32_t luma, uint32_t chroma, bool compareLumaOnly) :
			luma(luma), cb(chroma), cr(0), compareLumaOnly(compareLumaOnly), noCr(true) {
		}

		inline bool operator ==(const struct Crc &rhs) const {
			if (compareLumaOnly || rhs.compareLumaOnly)
				return luma == rhs.luma;
			if (noCr)
				return luma == rhs.luma && cb == rhs.cb;
			return luma == rhs.luma && cb == rhs.cb && cr == rhs.cr;
		}

		inline bool operator !=(const struct Crc &rhs) const {
			return !(*this == rhs);
		}

		inline string ToString() const {
			char s[65];
			if (compareLumaOnly)
				sprintf(s, "%08x", luma);
			else if (noCr)
				sprintf(s, "%08x %08x", luma, cb);
			else
				sprintf(s, "%08x %08x %08x", luma, cb, cr);
			return string(s);
		}

		friend ostream& operator <<(ostream &os, const struct Crc &obj) {
			return os << obj.ToString();
		}
	} Crc;

	typedef struct Frame : Crc {
		Crc* bottom;

		Frame() : bottom(NULL) {
		}

		Frame(const Crc top) : Crc(top), bottom(NULL) {
		}

		Frame(const Crc top, const Crc bottom) : Crc(top), bottom(new Crc) {
			*this->bottom = bottom;
		}

	    Frame(const struct Frame& frame) : Crc(frame), bottom(NULL) {
	    	if (frame.bottom) {
				this->bottom = new Crc;
				*this->bottom = *frame.bottom;
	    	}
		}

		inline Frame& operator=(const struct Frame& frame) {
			this->luma = frame.luma;
			this->cb = frame.cb;
			this->cr = frame.cr;
			this->compareLumaOnly = frame.compareLumaOnly;
			this->noCr = frame.noCr;
			if (frame.bottom) {
				if (!this->bottom)
					this->bottom = new Crc;
				*(this->bottom) = *frame.bottom;
			}
			return *this;
		};

		virtual ~Frame() {
		}

		inline bool operator ==(const struct Frame &rhs) const {
			return Crc::operator ==(rhs) && (bottom && rhs.bottom ? *bottom == *rhs.bottom : true);
		}

		inline bool operator !=(const struct Frame &rhs) const {
			return !(*this == rhs);
		}

		inline string ToString() const {
			return Crc::ToString() + (bottom ? " " + bottom->ToString() : "");
		}

		friend ostream& operator <<(ostream &os, const struct Frame &obj) {
			return os << obj.ToString();
		}
	} Frame;

	typedef vector<Frame> crc_sequence;

	static const size_t SEARCH_WINDOW = 5;

	crc_sequence reference;
	crc_sequence actual;

	bool found, loopback;
	int first;
	size_t current;
	size_t searched;
	size_t searchStart;
	bool hasError;
	size_t referenceSize;
	size_t recordFrames;

    void setup(size_t same);
    void setSearchStart();

public:
	ReferenceCrc();
	virtual ~ReferenceCrc();

public:
	bool ReadReference(const char* referenceFile);
	void AddReference(const uint32_t luma[], const uint32_t cb[], const uint32_t cr[], const size_t length);
	void AddCaptured(const size_t luma, const size_t cb, const size_t cr);
	void AddCaptured(const size_t luma, const size_t cb, const size_t cr, const size_t luma_bot, const size_t cb_bot, const size_t cr_bot);
	void AddCaptured(const size_t luma, const size_t chroma, const size_t luma_r, const size_t chroma_r);
	bool IsCaptureDone() const;
	bool Pass();
	string Result(ostream& details = cout);
	void DumpCaptured(const char* dumpFile) const;
	bool HasError() { return hasError; }
	bool CompareLumaOnly;
	void SetRecordFrames(const size_t numFrames) { recordFrames = numFrames; }

private:
	void search();
	void compare();
	void compute();
    void missed(queue<Frame>& del, queue< pair<Frame, bool> >& add, ostream& result, size_t& idx, size_t& misMatch, size_t& extra, size_t& repeat, size_t& drop) const;
    void mismatch(Frame& ref, Frame& cap, ostream& details, size_t& idx, size_t& misMatch) const;
};

}
}
}
#endif /* REFERENCECRC_H_ */
