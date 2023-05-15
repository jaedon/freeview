/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_keypad.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:12p $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_keypad.h $
 * 
 * 1   2/7/05 8:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 *****************************************************************/
#ifndef INPUT_KEYPAD_H__
#define INPUT_KEYPAD_H__

#include "inputdevice.h"
class Control; /* Control is needed in order to make keypad commands context sensitive */

/**
Summary:
Settop frontpanel buttons.
**/
class KeypadInput : public InputDevice {
public:
	/**
	Summary:
	Control is needed because KeypadInput is context sensitive in order to
	maximize the functionality.
	**/
	KeypadInput(Control *control, MApplication *app, InputEngine *ie) : InputDevice(app, ie, 3)
	{
		_control = control;
	}

	void process(uint32_t data);
	
protected:
	Control *_control;
};

#endif // INPUT_KEYPAD_H__
