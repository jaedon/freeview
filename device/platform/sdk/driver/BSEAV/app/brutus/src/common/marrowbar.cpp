/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: marrowbar.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 1/19/07 2:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/marrowbar.cpp $
 * 
 * 3   1/19/07 2:50p erickson
 * PR25913: added MArrowBar::getItem
 * 
 * 2   8/24/05 11:08a erickson
 * PR16819: only show arrows if there are 2 or more options
 * 
 * 1   2/7/05 8:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   9/13/04 1:53p erickson
 * PR11081: factored MArrowBar into separate gui component
 *
 ****************************************************************************/
#include "marrowbar.h"

MArrowBar::MArrowBar(MWidget *parent, const MRect &rect) :
	MPushButton(parent, rect, NULL, NULL)
{
	setAlignment(MPainter::alCenter);
	setVAlignment(MPainter::valCenter);
	_current = -1;
	_left = new MLabel(this, MRect(0,0,10,10), "<");
	_left->setAlignment(MPainter::alCenter);
	_left->setVAlignment(MPainter::valCenter);
	_left->hide(); /* until something is added */
	_right = new MLabel(this, MRect(0,0,10,10), ">");
	_right->setAlignment(MPainter::alCenter);
	_right->setVAlignment(MPainter::valCenter);
	_right->hide();
	/* this sets the left/right geometry */
	setGeometry(rect);
}

MArrowBar::~MArrowBar()
{
	delete _left;
	delete _right;
}

void MArrowBar::setGeometry(const MRect &rect)
{
	MPushButton::setGeometry(rect);
	int h = height();
	_left->setGeometry(MRect(0,0,h,h));
	_right->setGeometry(MRect(width()-height(),0,h,h));
}

void MArrowBar::clear()
{
	_items.clear();
	_current = -1;
	_left->hide();
	_right->hide();
}

const char *MArrowBar::getItem(int index) //TODO: prefer a const version, but lack MStringList support
{
	if (index == -1) index = current();
	return _items[index];
}

void MArrowBar::setItem(int index, const char *item)
{
	if (index == -1 || index >= _items.total())
		_items.add(item);
	else
		_items[index] = item;
	if (_current < 0 || _current == index)
		setCurrent(0);
}

void MArrowBar::addItem(const char *item)
{
	_items.add(item);
	if (_current < 0)
		setCurrent(0);
	if (_items.total() > 1) {
		_left->show();
		_right->show();
	}
	else {
		_left->hide();
		_right->hide();
	}
}

void MArrowBar::setCurrent(int index)
{
	setText(_items[index]);
	_current = index;
}

void MArrowBar::left()
{
	int index = _current-1;
	if (index < 0)
		index = _items.total()-1;
	setCurrent(index);
}

void MArrowBar::right()
{
	int index = _current+1;
	if (index >= _items.total())
		index = 0;
	setCurrent(index);
}

bool MArrowBar::keyDown(int ascii, int modifiers, int scancode)
{
	switch (ascii) {
	case MWKEY_LEFT: left(); return true;
	case MWKEY_RIGHT: right(); return true;
//	case MWKEY_ENTER: select(); return true;
	default: return MPushButton::keyDown(ascii, modifiers, scancode);
	}
}
