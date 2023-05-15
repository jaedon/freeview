/***************************************************************************
 *     Copyright (c) 2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_uhfremote.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/9/06 11:52a $
 *
 * Module Description: UHF remote header
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_uhfremote.h $
 * 
 * 1   5/9/06 11:52a jrubio
 * PR18491: new uhf remote file
 * 
 ***************************************************************************/

#ifndef INPUT_UHFREMOTE_H__
#define INPUT_UHFREMOTE_H__

#include "inputdevice.h"

/**
*Summary:
* One flavor of remote control This is DirecTV* remote
*
********/
class uhfDirecTV: public InputDevice {
public:
	uhfDirecTV(MApplication *app, InputEngine *ie) : InputDevice(app, ie, 5) {}

	void process(uint32_t data);
};

/**
*Summary:
* One flavor of remote control This is EchoStar remote
*
********/
class uhfEchostar : public InputDevice {
public:
	uhfEchostar(MApplication *app, InputEngine *ie) : InputDevice(app, ie, 6) {}

	void process(uint32_t data);
};



#endif //INPUT_UHFREMOTE_H__
