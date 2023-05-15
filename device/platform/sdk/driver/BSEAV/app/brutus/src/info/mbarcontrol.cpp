/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mbarcontrol.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/mbarcontrol.cpp $
 * 
 * 1   2/7/05 8:11p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   11/23/04 9:54a erickson
 * PR13336: added MBarControl for picture control ui
 *
 ****************************************************************************/
#include "mbarcontrol.h"

/**
TODO: If this control proves useful, we'll want to make it more general.
1) configurable colors
2) configurable sizes
3) configurable button layouts
**/

/**
This is a generic drawing routine used by the Control and Button.
**/
static void drawBar(MPainter &ptr, const MRect &rect, const MBarRange *range)
{
#define CURSOR_WIDTH 3
	int x; // x position of cursor
	if (range->high - range->low)
		x = (rect.width()-CURSOR_WIDTH) * (range->value - range->low) / (range->high - range->low);
	else
		x = 0;
		
	/* Hardcoded colors */
	ptr.setBrush(0xFF999999);
	ptr.fillRect(rect);
	ptr.setBrush(0xFF4040b0);
	ptr.fillRect(MRect(rect.x()+x,rect.y(),CURSOR_WIDTH,rect.height()));
}

////////////////////////////

MBarControl::MBarControl(MWidget *parent, const MRect &rect, const char *text) :
	MButton(parent, rect, text)
{
	setFocusable(true);
	_range.low = _range.high = _range.value = 0;
	_inc = 0;
	_callback = NULL;
}

void MBarControl::setCallback(MBarControlCallback callback, void *context)
{
	_callback = callback;
	_context = context;
}

void MBarControl::set(const MBarRange *range, int increment)
{
	_range = *range;
	_inc = increment;
	repaint();
}

void MBarControl::setValue(int value)
{
	if (value < _range.low)
		value = _range.low;
	else if (value > _range.high)
		value = _range.high;
	_range.value = value;

	if (_callback)
		_callback(this, _context);

	repaint();
}

bool MBarControl::keyDown(int ascii, int modifiers, int scancode)
{
	switch (ascii) {
	case MWKEY_LEFT:
		setValue(_range.value - _inc);
		return true;
	case MWKEY_RIGHT:
		setValue(_range.value + _inc);
		return true;
	default: return MButton::keyDown(ascii, modifiers, scancode);
	}
}

void MBarControl::draw(const MRect &cliprect)
{
	/* this is a simple "blur" test */
	if (!hasFocus()) {
		hide();
		return;
	}

	MPainter ptr(this, cliprect);
	int br = 5;
	int textHeight = 20;

	ptr.setPen(textColor());
	ptr.setFont(currentFont());
	ptr.drawText(MRect(br,br,width()-br*2,textHeight), text(), -1, MPainter::alCenter);

	drawBar(ptr, MRect(br,br+textHeight,width()-br*2,height()-br*2-textHeight), &_range);
}

/////////////////////////////////////////////

MBarButton::MBarButton(MWidget *parent, const MRect &rect, const char *text) :
	MPushButton(parent, rect, text)
{
	setBevelWidth(2);
}

void MBarButton::draw(const MRect &cliprect)
{
	MPainter ptr(this, cliprect);
	MRect barRect(0,5,width()/3,height()-10);

	if (!hasFocus()) {
		/* Gray bevel */
		ptr.drawBevel(barRect, _bevel,
			0xFF777777,
			0xFF777777,
			0xFF222222,
			0xFF222222);
	}
	else {
		/* Draw focus as a yellow colored bevel. */
		ptr.drawBevel(barRect, _bevel,
			style()->color(MStyle::FOCUS),
			style()->color(MStyle::FOCUS),
			style()->color(MStyle::FOCUS),
			style()->color(MStyle::FOCUS));
	}

	drawBar(ptr, MRect(barRect.x()+_bevel,barRect.y()+_bevel,
		barRect.width()-_bevel*2,barRect.height()-_bevel*2),
		&_range);

	/* draw text */
	ptr.setFont(currentFont());
	ptr.setPen(textColor());
	ptr.drawText(MRect(width()/3+5, 0, width()*2/3-5, height()), _text, _textlen,
		MPainter::alLeft, _val);
}

void MBarButton::set(const MBarRange *range)
{
	_range = *range;
	repaint();
}
