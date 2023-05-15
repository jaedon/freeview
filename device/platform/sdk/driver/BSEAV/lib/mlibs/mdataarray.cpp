/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: mdataarray.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 10:05a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mdataarray.cpp $
 * 
 * 2   9/7/12 10:05a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "mdataarray.h"
#include <string.h>

#define PAD 10

#define ADD() \
	resize(_writeindex+sizeof(v)+PAD, true); \
	memcpy(&_array[_writeindex], &v, sizeof(v)); \
	_writeindex += sizeof(v)

void MDataArray::addChar(char v) {
	ADD();
}

void MDataArray::addInt(int v) {
	ADD();
}

void MDataArray::addShort(short v) {
	ADD();
}

void MDataArray::addFloat(float v) {
	ADD();
}

void MDataArray::addLong(long v) {
	ADD();
}

void MDataArray::addBool(bool v) {
	ADD();
}

void MDataArray::addString(const char *v) {
	// prefix a 0 because of LIFO mode
	addChar(0);

	int len = strlen(v);
	resize(_writeindex+len+1+PAD, true);
	memcpy(&_array[_writeindex], v, len+1);
	_writeindex += len+1;
}

#define GET(TYPE, DEFAULT) \
	if (_mode == FIFO) { \
		if (_readindex >= _size) \
			return DEFAULT; \
		else { \
			TYPE v; \
			memcpy(&v, &_array[_readindex], sizeof(v)); \
			_readindex += sizeof(v); \
			return v; \
		} \
	} \
	else { \
		TYPE v; \
		if (_writeindex - sizeof(v) < 0) \
			return DEFAULT; \
		else { \
			memcpy(&v, &_array[_writeindex-sizeof(v)], sizeof(v)); \
			_writeindex -= sizeof(v); \
			return v; \
		} \
	}

char MDataArray::getChar() {
	GET(char, 0);
}

int MDataArray::getInt() {
	GET(int, 0);
}

short MDataArray::getShort() {
	GET(short, 0);
}

float MDataArray::getFloat() {
	GET(float, 0.0);
}

long MDataArray::getLong() {
	GET(long, 0);
}

bool MDataArray::getBool() {
	GET(bool, false);
}

const char *MDataArray::getString() {
	if (_mode == FIFO) {
		if (getChar() != 0)
			return NULL;
		if (_readindex >= _size)
			return NULL;
		else {
			// remove 0 prefix
			const char *str = (const char *)&_array[_readindex];
			_readindex += strlen(str)+1;
			return str;
		}
	}
	else {
		if (_writeindex == 0)
			return NULL;
		else {
			const char *str = (const char *)&_array[_writeindex];
			// first, verify the postfixed 0
			if (*--str)
				return NULL;
			// search backwards to the prefixed 0
			while (*--str && str > (const char *)_array);
			_writeindex = str - (const char *)_array;
			if (str == (const char *)_array)
				return NULL;
			return str+1;
		}
	}
}

#define HAS_MAIN
#ifdef HAS_MAIN
#include <stdio.h>
int main() {
	MDataArray a(MDataArray::FIFO);
	a.addInt(1);
	a.addFloat(2.2);
	a.addBool(true);
	a.addString("this is a test");
	int v1 = a.getInt();
	float v2 = a.getFloat();
	bool v3 = a.getBool();
	const char *v4 = a.getString();
	printf("FIFO dataarray=%d,%f,%s,%s, arraysize=%d\n",
		v1,v2,v3?"TRUE":"FALSE",v4, a.size());

	a.setMode(MDataArray::LIFO);
	a.addInt(1);
	a.addFloat(2.2);
	a.addBool(true);
	a.addString("this is a test");
	int writeIndex = a.writeIndex();
	printf("LIFO dataarray=%d,%f,%s,%s, arraysize=%d\n",
		a.getInt(),
		a.getFloat(),
		a.getBool()?"TRUE":"FALSE",
		a.getString());
	a.setWriteIndex(writeIndex);
	printf("LIFO (2nd time) dataarray=%d,%f,%s,%s, arraysize=%d\n",
		a.getInt(),
		a.getFloat(),
		a.getBool()?"TRUE":"FALSE",
		a.getString());
	return 0;
}
#endif
