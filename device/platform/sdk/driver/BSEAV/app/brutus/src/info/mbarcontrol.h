/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mbarcontrol.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/mbarcontrol.h $
 * 
 * 1   2/7/05 8:11p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   11/23/04 9:54a erickson
 * PR13336: added MBarControl for picture control ui
 *
 ****************************************************************************/
#ifndef MBARCONTROL_H__
#define MBARCONTROL_H__

#include "mbutton.h"

class MBarControl;

typedef void (*MBarControlCallback)(MBarControl *control, void *context);

/**
Describes an inclusive range and current value.
The following rule applies:
	value >= low && value <= high.
**/
struct MBarRange {
	int low;
	int high;
	int value;
};

/**
Widget which displays a range bar and indicator.
Allow right/left control of the indicator, based on an increment value.
**/
class MBarControl : public MButton {
public:
	MBarControl(MWidget *parent, const MRect &rect, const char *text);

	/* Set the range and value */
	void set(const MBarRange *range, int increment);
	/* Set the value */
	void setValue(int value);
	/* Get the current value */
	int value() const {return _range.value;}

	/**
	Set a callback which will be called when value is updated.
	**/
	void setCallback(MBarControlCallback callback, void *context);

protected:
	MBarRange _range;
	int _inc;
	MBarControlCallback _callback;
	void *_context;

	/* handle left/right changes */
	bool keyDown(int ascii, int modifiers, int scancode);
	void draw(const MRect &cliprect);
};

/**
Widget which displays a button with text, along with
a range bar and indicator.
Does not provide a UI to change the indicator.
**/
class MBarButton : public MPushButton {
public:
	MBarButton(MWidget *parent, const MRect &rect, const char *text);
	
	/* Set the range and value */
	void set(const MBarRange *range);

protected:
	MBarRange _range;
	void draw(const MRect &cliprect);
};

#endif //MBARCONTROL_H__
