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
 * $brcm_Workfile: marray.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 10:05a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/marray.h $
 * 
 * 2   9/7/12 10:05a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#ifndef MARRAY_H
#define MARRAY_H

#include <stdlib.h>

template <class T>
class MArray {
public:
	MArray(int size = 0);
	virtual ~MArray() {clear();}

	MArray<T> copy() const {return *this;}

	int size() const {return _size;}

	void resize(int size, bool dontShrink = false);
	void clear() {resize(0);}
	bool fill(const T &v, int newsize = -1) {
		if (newsize != -1)
			resize(newsize);
		for (int i=0;i<_size;i++)
			_array[i] = v;
		return true;
	}

	T &operator[](int index) {
		if (index >= size())
			resize(index+10);
		return _array[index];
	}

protected:
	T *_array;
	int _size;
};

template <class T>
inline MArray<T>::MArray(int size) {
	_array = NULL;
	_size = 0;
	resize(size);
}

template <class T>
inline void MArray<T>::resize(int size, bool dontShrink) {
	if (!dontShrink && size != _size ||
		dontShrink && size > _size)
	{
		T *temp = size?new T[size]:NULL;
		if (_array && temp) {
			int cpysize;
			if (_size > size)
				cpysize = _size * sizeof(T);
			else
				cpysize = size * sizeof(T);
			memcpy(temp, _array, cpysize);
			delete _array;
		}
		_array = temp;
		_size = size;
	}
}

#endif
