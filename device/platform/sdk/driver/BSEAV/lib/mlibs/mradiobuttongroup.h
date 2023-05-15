/***************************************************************************
 *     Copyright (c) 2002-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mradiobuttongroup.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 5/11/06 2:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mradiobuttongroup.h $
 * 
 * 2   5/11/06 2:55p erickson
 * PR20951: don't hardcode textColor in radio button
 * 
 ********************************************************************/
#ifndef MRADIOBUTTONGROUP_H
#define MRADIOBUTTONGROUP_H

#include "mbutton.h"

BEGIN_EV(MRadioButtonGroup)
	EV_METHOD(MRadioButtonGroup, Click)
END_EV()

class MRadioButtonGroup : public MWidget, public MButtonEventHandler {
public:
	MRadioButtonGroup(MWidget *parent, const MRect &rect, const char *name = NULL);

	/**
	* Set how high each radiobutton item is. -1 (default) means evenly
	* spaced over the entire height of the group.
	**/
	void setItemHeight(int itemHeight);

	void click(int index);
	/**
	* Find first enabled radio button and click it.
	*/
	void clickDefault();
	int checked();
	bool itemEnabled(int index);

	bool focus();
	/**
	* Calling setEnabled on the entire group wipes out any individual
	* setEnabled calls you may have done.
	*/
	void setEnabled(bool enabled);
	void setEnabled(int index, bool enabled);

	void add(const char *text);
	void clear() {deleteChildren();}
	int total() const {return totalChildren();}

	//event
	void onClick(MButton *event);

protected:
	int _itemHeight;

	// overrides
	void setGeometry(const MRect &rect);

	void layout();

	SUPPORT_EV(MRadioButtonGroup)
	SUPPORT_EV_METHOD(Click)
};

#endif //MRADIOBUTTONGROUP_H
