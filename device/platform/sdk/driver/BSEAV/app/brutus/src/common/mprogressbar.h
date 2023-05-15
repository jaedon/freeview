/***************************************************************************
 *     Copyright (c) 2003-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mprogressbar.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 12/20/05 6:23a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/mprogressbar.h $
 * 
 * 1   12/20/05 6:23a erickson
 * PR18256: moved progress bar code into reuseable file
 * 
 ***************************************************************************/
#ifndef MPROGRESSBAR_H__
#define MPROGRESSBAR_H__

#include "mlabel.h"

/**
The MProgressBar operates in two modes:
1) depth/size mode. This is the traditional progress bar.
2) diff mode. This shows a position or negative value around a center zero-bar.
**/
class MProgressBar : public MLabel {
public:
	MProgressBar(MWidget *parent, const MRect &rect, const char *label = NULL);
	
	void setColors(unsigned long barColor, unsigned long background) {
		_barColor = barColor;
		_bkColor = background;
		repaint();
	}
	
	void setDepth(unsigned long depth, unsigned long size) {
		_size = size;
		_depth = depth;
		_isDiff = false;
		repaint();
	}
	unsigned long size() const {return _size;}
	unsigned long depth() const {return _depth;}
	
	void setDiff(unsigned int maxdiff, int diff) {
		_maxdiff = maxdiff;
		_diff = diff;
		_isDiff = true;
		repaint();
	}
protected:
	unsigned long _barColor, _bkColor, _size, _depth;
	int _maxdiff, _diff;
	bool _isDiff;
	
	void draw(const MRect &cliprect);
};

#endif /* MPROGRESSBAR_H__ */
