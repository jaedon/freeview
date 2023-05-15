/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: marrowbar.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 1/19/07 2:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/marrowbar.h $
 * 
 * 3   1/19/07 2:50p erickson
 * PR25913: added MArrowBar::getItem
 * 
 * 2   8/24/05 5:12p erickson
 * PR16819: added total()
 * 
 * 1   2/7/05 8:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   9/13/04 1:53p erickson
 * PR11081: factored MArrowBar into separate gui component
 *
 ****************************************************************************/
#ifndef MARROWBAR_H__
#define MARROWBAR_H__

#include "mbutton.h"
#include "mlabel.h"
#include "mstringlist.h"

/**
Like a combo-box, but without the drop own. Once it has focus,
left/right will scroll through the selections. Up/down will navigate out.
**/
class MArrowBar : public MPushButton {
public:
	MArrowBar(MWidget *parent, const MRect &rect);
	~MArrowBar();

	void addItem(const char *item);
	void setItem(int index, const char *item);
	// -1 returns current item
	const char *getItem(int index = -1);
	void clear();
	int current() const {return _current;}
	void setCurrent(int index);
	int total() const {return _items.total();}
	void left();
	void right();

	// overrides
	void setGeometry(const MRect &rect);
protected:
	MStringList _items;
	int _current;
	MLabel *_left, *_right;
	bool keyDown(int ascii, int modifiers, int scancode);
};

#endif // MARROWBAR_H__
