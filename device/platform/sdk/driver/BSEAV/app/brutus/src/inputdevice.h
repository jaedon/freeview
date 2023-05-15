/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: inputdevice.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:13p $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/inputdevice.h $
 * 
 * 1   2/7/05 8:13p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   7/6/04 3:56p erickson
 * PR11771: settop api dataflow redesign
 * 
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 *****************************************************************/
#ifndef INPUTDEVICE_H__
#define INPUTDEVICE_H__

#include "miodevice.h"
#include "bsettop.h"
class InputEngine;

/**
Summary:
InputDevice is an abstract base class for managing input from mwidgets/bwin
into Brutus.

Description:
You should subclass this for a particular device, pass in the settopApiId and
implement a process() function to convert the device-dependent data into
device independent data and send into Brutus via the InputEngine.
**/
class InputDevice : public MIoDeviceEventHandler {
public:
	InputDevice(
		MApplication *app, /* The mwidgets instance into which the user input
								device should be inserted. */
		InputEngine *ie, /* Optional way of sending data into Brutus. */
		int settopApiId /* B_ID() to use when opening buser_input device. If -1
							then don't open a buser_input device. */
		);

	virtual ~InputDevice();

	void onReady(MIoDevice *dev);

	/**
	Summary:
	data comes in from InputDevice base class so that it can be converted
	from device dependent data into a device independent code.
	**/
	virtual void process(uint32_t data) = 0;

	MApplication *app() const {return _dev.app();}
protected:
	buser_input_t _ui;
	MIoDevice _dev;
	InputEngine *_inputEngine;
};

#endif //INPUTDEVICE_H__
