/***************************************************************************
 *     Copyright (c) 2004-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_btremote.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/13/08 4:54p $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_btremote.h $
 * 
 * 1   2/13/08 4:54p tokushig
 * PR39534: added bluetooth remote capabilities to brutus
 * 
 * SanDiego_Brutus_3D/2   1/30/08 2:20p tokushig
 * refactor demo bluetooth remote implementation for integration into main
 * line
 * 
 *****************************************************************/
#ifndef INPUT_BTREMOTE_H__
#define INPUT_BTREMOTE_H__

#include "inputdevice.h"

/**
Summary:
One flavor of remote control (aka Sony PS3 BT remote)**
***/
class BluetoothInput : public InputDevice {
public:
	BluetoothInput(MApplication *app, InputEngine *ie);
	void process(uint32_t data);
};

#endif //INPUT_BTREMOTE_H__
