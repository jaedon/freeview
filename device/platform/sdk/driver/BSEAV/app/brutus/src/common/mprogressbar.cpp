/***************************************************************************
 *     Copyright (c) 2003-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mprogressbar.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 12/20/05 6:23a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/mprogressbar.cpp $
 * 
 * 1   12/20/05 6:23a erickson
 * PR18256: moved progress bar code into reuseable file
 * 
 ***************************************************************************/
#include "mprogressbar.h"

MProgressBar::MProgressBar(MWidget *parent, const MRect &rect, const char *label) :
	MLabel(parent, rect, label)
{
	_barColor = 0xFF00ee22;
	_bkColor = 0xFFe0e0e0;
	_size = 100;
	_depth = 0;
	_maxdiff = 0;
	_diff = 0;
	_isDiff = false;
	setTransparent(false);
}
	
void MProgressBar::draw(const MRect &cliprect) {
	MPainter ptr(this, cliprect);

	if (!_isDiff) {
		float f;
		if (_size) {
			f = (float)_depth; // avoid overflown and maintain accuracy
			f /= _size;
			f *= width();
		}
		else {
			f = 0;
		}
		int w = (int)f;
		ptr.setBrush(_barColor);
		ptr.fillRect(MRect(0,0,w,height()));
		ptr.setBrush(_bkColor);
		ptr.fillRect(MRect(w,0,width()-w,height()));
	}
	else {
		float f;
		if (_diff < 0 && -_diff > _maxdiff)
			_diff = -_maxdiff;
		else if (_diff > 0 && _diff > _maxdiff)
			_diff = _maxdiff;
		if (_maxdiff) {
			f = (float)_diff; // avoid overflow and maintain accuracy
			f /= _maxdiff * 2;
			f *= width();
		}
		else {
			f = 0;
		}
		int w = (int)f;
		int half = width()/2;
		ptr.setBrush(_barColor);
		if (w < 0) {
			ptr.fillRect(MRect(half+w,0,-w,height()));
			ptr.setBrush(_bkColor);
			ptr.fillRect(MRect(0,0,half+w,height()));
			ptr.fillRect(MRect(half,0,half,height()));
		}
		else {
			ptr.fillRect(MRect(half,0,w,height()));
			ptr.setBrush(_bkColor);
			ptr.fillRect(MRect(0,0,half+1,height()));
			ptr.fillRect(MRect(half+w,0,half-w,height()));
		}
		ptr.setBrush(0); // black
		ptr.fillRect(MRect(half-1,0,2,height()));
	}
}

