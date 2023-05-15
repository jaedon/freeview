/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_irremote.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 1/12/12 10:15a $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_irremote.h $
 * 
 * 4   1/12/12 10:15a jrubio
 * SW7346-400: add silver remote
 * 
 * 3   5/9/06 11:41a jrubio
 * PR18491: Fixed description of RemoteC
 * 
 * 2   1/16/06 4:32p jrubio
 * PR18491: added function proto for UHF legacy remote
 * 
 * 1   2/7/05 8:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 *****************************************************************/
#ifndef INPUT_IRREMOTE_H__
#define INPUT_IRREMOTE_H__

#include "inputdevice.h"

/**
Summary:
One flavor of remote control (aka GI remote)
**/
class RemoteAInput : public InputDevice {
public:
	RemoteAInput(MApplication *app, InputEngine *ie) : InputDevice(app, ie, 0) {}

	void process(uint32_t data);
};

/**
Summary:
One flavor of remote control (aka SA remote)
**/
class RemoteBInput : public InputDevice {
public:
	RemoteBInput(MApplication *app, InputEngine *ie) : InputDevice(app, ie, 1) {}

	void process(uint32_t data);
};

/**
Summary:
One flavor of remote control (aka IR echostar remote)**
***/
class RemoteCInput : public InputDevice {
public:
	RemoteCInput(MApplication *app, InputEngine *ie) : InputDevice(app, ie, 4) {}

	void process(uint32_t data);
};

/**
Summary:
One flavor of remote control NEC****
**/
class RemoteDInput : public InputDevice {
public:
	RemoteDInput(MApplication *app, InputEngine *ie) : InputDevice(app, ie, 8) {}

	void process(uint32_t data);
};

#endif //INPUT_IRREMOTE_H__
