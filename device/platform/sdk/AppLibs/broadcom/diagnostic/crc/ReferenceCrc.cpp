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
 * $brcm_Workfile: ReferenceCrc.cpp $
 * $brcm_Revision: 19 $
 * $brcm_Date: 10/13/11 12:48p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/diagnostic/crc/ReferenceCrc.cpp $
 * 
 * 19   10/13/11 12:48p jliang
 * SW7346-463: Add compare luma only flag.
 * 
 * 18   6/17/11 1:27p jliang
 * SW7346-103: Remove an un-needed function and changed search start to
 *  10% of reference crc size.
 * 
 * 17   6/14/11 3:51p jliang
 * SW7346-103: Change search function to use ref crc if beginning if found
 *  when looking for loop back.
 * 
 * 16   6/10/11 3:50p jliang
 * SW7346-103: added \n to printf statement.
 * 
 * 15   6/10/11 3:29p jliang
 * SW7346-103: added some printf statements in IsCaptureDone.
 * 
 * 14   6/2/11 1:27p jliang
 * SW7346-103: Make parameter length const.
 * 
 * 13   6/2/11 1:22p jliang
 * SW7346-103: Added AddReference array function.
 * 
 * 12   5/25/11 2:59p jliang
 * SW7346-103: Fixed a bug in crc reference print output.
 * 
 * 11   5/25/11 2:11p jliang
 * SW7346-103: Fixed referenceSize in AddReference.
 * 
 * 10   5/25/11 12:31p jliang
 * SW7346-103: Added AddReference function.
 * 
 * 9   5/24/11 4:08p jliang
 * SW7346-103: Change IsCaptureDone to capture twice the ref file if
 *  loopback not found.
 * 
 * 8   4/14/11 2:27p jliang
 * SW7346-103: Check if reference crc exists or not, if not, return false.
 * 
 * 7   4/7/11 4:39p jliang
 * SW7346-103: Added work around for a bug in diff algorithm. Need to find
 *  and fix the bug!!!
 * 
 * 6   4/7/11 11:56a jliang
 * SW7346-103: Change first to 0 when loopback found but first not found.
 * 
 * 5   4/6/11 3:42p jliang
 * SW7346-103: fixed loopback detection for repeated patterns.
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
 * 1   3/29/11 12:35p jliang
 * SW7346-103: adding CRC app source code.
 * 
 *  ReferenceCrc.cpp
 *
 *  Created on: Mar 17, 2011
 ***************************************************************************/
#include <stdint.h>
#include <stdlib.h>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

//#include <boost/algorithm/string.hpp>
#include <dtl/dtl.hpp>

using namespace std;

#include "ReferenceCrc.h"

namespace broadcom {
namespace broadband {
namespace diagnostic {

ReferenceCrc::ReferenceCrc() :
	found(false), loopback(false), first(-1), current(0), searched(0), searchStart(0), hasError(false),
			referenceSize(0), recordFrames(0), CompareLumaOnly(false) {
	reference.reserve(referenceSize);
}

ReferenceCrc::~ReferenceCrc() {
}

void ReferenceCrc::setSearchStart() {
	if (searchStart < referenceSize * 0.1)
		searchStart = referenceSize * 0.1;

	if (searchStart < 120)
		searchStart = 120;

	if (searchStart > referenceSize)
		searchStart = referenceSize > SEARCH_WINDOW ? referenceSize - SEARCH_WINDOW : 0;

	cout << "Search Start: " << searchStart << "." << endl;
}

void ReferenceCrc::setup(size_t sameAtBeginning) {
	referenceSize = reference.size();
	cout << "Total of reference CRC: " << dec << referenceSize << ". ";

	if (sameAtBeginning >= referenceSize) {
		cout << "All frames are identical." << endl;
		searchStart = 0;
		actual.reserve(referenceSize);
		return;
	}

	if (sameAtBeginning > 1) {
		cout << "First " << sameAtBeginning << " frames' luma are identical." << endl;
		searchStart = sameAtBeginning - 2;
	}
	else
		searchStart = 0;
	setSearchStart();
	actual.reserve(referenceSize + searchStart);
}

/*
 * ReadReference returns false if the file cannot be opened.
 */
bool ReferenceCrc::ReadReference(const char *referenceFile) {
	size_t same = 0;
	Frame firstCrc;
	bool notTheSame = false;
	ifstream refile(referenceFile);
	if (refile.is_open()) {
		string line;
		while (!refile.eof()) {
			getline(refile, line);
			if (line[0] == '#')
				continue;

			size_t co = line.find(":");
			if (co != string::npos)
				line = line.substr(co + 1);

			vector<string> strs;
			istringstream iss(line);
			copy(istream_iterator<string> (iss), istream_iterator<string> (), back_inserter<vector<string> > (strs));
			//			boost::split(strs, line, boost::is_any_of(" ,"));
			int l = strs.size();
			if (l == 0)
				continue;
			int i = 0;
			bool is3D = false;
			if (l >= 6) // top and bottom
				i = l - 6;
			else if (l >= 4) { // 3D
				i = l - 4;
				is3D = true;
			}
			else if (l >= 3)
				i = l - 3;
			else if (l >= 2)
				i = l - 2;

			Frame crc;
			crc.luma = strtoll(strs[i].c_str(), NULL, 16);
			if (l >= 3) {
				crc.cb = strtoll(strs[i + 1].c_str(), NULL, 16);
				if (is3D) {
					crc.cr = 0;
					crc.noCr = true;
				}
				else
					crc.cr = strtoll(strs[i + 2].c_str(), NULL, 16);
			}
			else if (l >= 2) {
				crc.cb = strtoll(strs[i + 1].c_str(), NULL, 16);
				crc.cr = 0;
				crc.noCr = true;
			}
			else
				crc.compareLumaOnly = true;

			if (l >= 6) {
				crc.bottom = new Crc;
				crc.bottom->luma = strtoll(strs[i + 3].c_str(), NULL, 16);
				crc.bottom->cb = strtoll(strs[i + 4].c_str(), NULL, 16);
				crc.bottom->cr = strtoll(strs[i + 5].c_str(), NULL, 16);
			}
			else if (l >= 4) {
				crc.bottom = new Crc;
				crc.bottom->luma = strtoll(strs[i + 2].c_str(), NULL, 16);
				crc.bottom->cb = strtoll(strs[i + 3].c_str(), NULL, 16);
				crc.bottom->cr = 0;
				crc.bottom->noCr = true;
			}

			if (!notTheSame) {
				if (0 == same) {
					firstCrc = crc;
					same++;
				}
				else if (firstCrc.luma != crc.luma)
					notTheSame = true;
				else
					same++;
			}

			reference.push_back(crc);
			//			cout << crc.ToString() << endl;
		}
		refile.close();
	}
	else
		return false;
	setup(same);
	return true;
}

void ReferenceCrc::AddReference(const uint32_t luma[], const uint32_t cb[], const uint32_t cr[], const size_t length) {
	if (length == 0) return;
	size_t same = 0;
	Crc firstCrc(luma[0], cb[0], cr[0]);
	bool notTheSame = false;
	for (size_t i = 0; i < length; i++) {
		if (!notTheSame) {
			if (firstCrc.luma != luma[i])
				notTheSame = true;
			same++;
		}
		reference.push_back(Crc(luma[i], cb[i], cr[i]));
	}
	setup(same);
}

void ReferenceCrc::compute() {
	if (recordFrames) return;
	if (!found || !loopback)
		search();
	compare();
}

void ReferenceCrc::AddCaptured(const size_t luma, const size_t cb, const size_t cr) {
	actual.push_back(Crc(luma, cb, cr, CompareLumaOnly));
	compute(); // do it at real time
}

void ReferenceCrc::AddCaptured(const size_t luma, const size_t cb, const size_t cr, const size_t luma_bot, const size_t cb_bot, const size_t cr_bot) {
	if (reference[0].bottom)
		actual.push_back(Frame(Crc(luma, cb, cr, CompareLumaOnly), Crc(luma_bot, cb_bot, cr_bot, CompareLumaOnly)));
	else
		actual.push_back(Crc(luma, cb, cr, CompareLumaOnly));
	compute(); // do it at real time
}

void ReferenceCrc::AddCaptured(const size_t luma, const size_t chroma, const size_t luma_r, const size_t chroma_r) {
	actual.push_back(Frame(Crc(luma, chroma, CompareLumaOnly), Crc(luma_r, chroma_r, CompareLumaOnly)));
	compute(); // do it at real time
}

bool ReferenceCrc::IsCaptureDone() const {
	if (loopback)
		printf("Video has looped back.\n");
	if (!recordFrames && actual.size() >= referenceSize * 2 + searchStart + SEARCH_WINDOW)
		printf("Capture done, because we have captured %d frames.\n", actual.size());
	if (recordFrames && actual.size() >= recordFrames)
		printf("Capture done, because we have recorded %d frames.\n", actual.size());
	return loopback || (!recordFrames && actual.size() >= referenceSize * 2 + searchStart + SEARCH_WINDOW) // if loopback not found, capture twice the reference file
			|| (recordFrames && actual.size() >= recordFrames);
}

void ReferenceCrc::search() {
	if (actual.size() < SEARCH_WINDOW)
		return;

	for (; searched < actual.size() - SEARCH_WINDOW; searched++) {
		if (!found) {
			found = true;
			for (size_t i = 0; i < SEARCH_WINDOW; i++) {
				//				cout << dec << searched << ". searching " << hex << reference[i + searchStart].luma << ": " << actual[i
				//						+ searched].luma << endl;
				if (reference[i + searchStart].luma != actual[i + searched].luma) {
					found = false;
					break;
				}
			}
			if (found) {
				first = searched - searchStart;
				if (first < 0)
					first = 0;
			}
		}

		if (!loopback && searched - (found ? first : 0) >= referenceSize) {
			loopback = true;
			for (size_t i = 0; i < SEARCH_WINDOW; i++) {
				//				cout << dec << i + searched << ". loopback? " << actual[i + searched] << "," << actual[i + searchStart]
				//						<< endl;
				if (!found) {
					if (actual[i + searchStart].luma != actual[i + searched].luma) {
						loopback = false;
						break;
					}
					printf("Loopback compare frame %d with frame %d: %x = %x\n", i + searchStart, i + searched - (found ? first : 0),
							actual[i + searchStart].luma, actual[i + searched].luma);
				}
				else {
					if (reference[i + searchStart].luma != actual[i + searched].luma) {
						loopback = false;
						break;
					}
				}
			}
			if (loopback) {
				cout << "Loopback found at " << dec << searched << endl;
				if (!found) {
					found = true;
					first = 0;
				}
				break;
			}
		}
	}
}

void ReferenceCrc::compare() {
	if (!found || hasError)
		return;

	for (int i = first + current; current < referenceSize && i < (int) actual.size(); current++, i++) {
		if (i < 0 || reference[current] != actual[i]) {
			hasError = true;
		}
	}
}
bool ReferenceCrc::Pass() {
	compute();
	return first >= 0 && first + referenceSize <= actual.size() && !hasError;
}

void ReferenceCrc::missed(queue<Frame> & del, queue<pair<Frame, bool> > & add, ostream & result, size_t & idx,
		size_t & misMatch, size_t & repeat, size_t & extra, size_t & drop) const {
	while (!del.empty() && !add.empty()) {
		mismatch(del.front(), add.front().first, result, idx, misMatch);
		//		if (add.front().first != del.front()) {
		//			result << "Frame " << dec << idx++ << " mismatch. Captured CRC: " << add.front().first << ". Reference: "
		//					<< del.front() << endl;
		//			misMatch++;
		//		}
		//		else
		//			idx++;
		del.pop();
		add.pop();
	}
	while (!add.empty()) {
		result << "Frame " << dec << idx << (add.front().second ? " repeat" : " extra") << ". Captured CRC: "
				<< add.front().first << endl;
		if (add.front().second)
			repeat++;

		else
			extra++;

		add.pop();
	}
	while (!del.empty()) {
		result << "Frame " << dec << idx++ << " dropped. Reference CRC: " << del.front() << endl;
		del.pop();
		drop++;
	}
}

void ReferenceCrc::mismatch(Frame & ref, Frame & cap, ostream & details, size_t & idx, size_t & misMatch) const {
	if (ref != cap) {
		details << "Frame " << dec << idx++ << " mismatch. Captured CRC: " << cap << ". Reference: " << ref << endl;
		misMatch++;
	}
	else
		idx++;

}

string ReferenceCrc::Result(ostream & details) {
	stringstream result;
	if (Pass())
		result << "Passed!";
	else {
		result << "Failed. ";
		if (!found)
			result << " Unable to find the first frame.";
		else if (first + referenceSize > actual.size())
			result << " Not enough frames captured.";
		result << endl;

		if (found) {
			crc_sequence a(actual.begin() + first, actual.end());
			//			a.assign(actual.begin() + first, actual.end());
			dtl::Diff<Frame> d(reference, a);
			d.onHuge();
			d.compose();
			//			d.printSES();
			vector<pair<Frame, dtl::elemInfo> > info = d.getSes().getSequence();
			//			dtl::ChangePrinter<pair<Frame, dtl::elemInfo> , ostream> print(cout << endl);
			size_t misMatch = 0, repeat = 0, extra = 0, drop = 0;
			queue<Frame> del;
			queue<pair<Frame, bool> > add;
			size_t idx = 1;
			for (size_t i = 0; i < info.size() && idx <= referenceSize; i++) {
				if (info[i].second.type == dtl::SES_COMMON) {
					missed(del, add, details, idx, misMatch, repeat, extra, drop);
					idx++;
				}
				else if (info[i].second.type == dtl::SES_ADD) {
					if (!del.empty()) {
						mismatch(del.front(), info[i].first, details, idx, misMatch);
						//						if (del.front != info[i].first) {
						//							details << "Frame " << dec << idx++ << " mismatch. Captured CRC: " << info[i].first
						//									<< ". Reference: " << del.front() << endl;
						//							misMatch++;
						//						}
						//						else
						//							idx++;
						del.pop();
					}
					else if (i > 0) {
						add.push(pair<Frame, bool> (info[i].first, info[i].first == info[i - 1].first));
					}
				}
				else {
					if (!add.empty()) {
						mismatch(info[i].first, add.front().first, details, idx, misMatch);
						//						details << "Frame " << dec << idx++ << " mismatch. Captured CRC: " << add.front().first
						//								<< ". Reference: " << info[i].first << endl;
						add.pop();
						//						misMatch++;
					}
					else
						del.push(info[i].first);
				}

				//				cout << dec << info[i].second.beforeIdx << "," << info[i].second.afterIdx << ": ";
				//				print(info[i]);
			}
			//            for_each(info.begin(), info.end(), ChangePrinter< dtl::Ses<Frame> , ostream >(cout));
			if (!del.empty())
				missed(del, add, details, idx, misMatch, repeat, extra, drop);
			result << dec << "Mismatch: " << misMatch << ". repeat: " << repeat << ". extra: " << extra
					<< ". dropped: " << drop << endl;
		}
	}
	//	result << "\nFirst matching frame is " << first << endl;
	//	result << "\nTotal # of frames captured: ";
	//	result << actual.size();
	//	result << ". Total # of reference frames: ";
	//	result << referenceSize;
	return result.str();
}

void ReferenceCrc::DumpCaptured(const char *dumpFile) const {
	ofstream dump(dumpFile);
	if (dump.is_open()) {
		for (size_t i = 0; i < actual.size(); i++)
			dump << dec << (int) (((((i - first))))) << ": " << actual[i].ToString() << "\n";
	}
	dump.close();
}

}
}
} // name space
