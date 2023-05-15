/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_sejinkbd.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:12p $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_sejinkbd.h $
 * 
 * 1   2/7/05 8:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new design, support for shift/control/capslock
 *
 *****************************************************************/
#ifndef INPUT_SEJINKBD_H__
#define INPUT_SEJINKBD_H__

#include "inputdevice.h"

/**
Summary:
Implements one flavor of IR keyboard device.
**/
class SejinIRKeyboardInput : public InputDevice {
public:
	SejinIRKeyboardInput(MApplication *app);

	void process(uint32_t data);
	
protected:
	uint32_t last;
	bool shift, capslock, ctrl;
};

#endif //INPUT_SEJINKBD_H__
