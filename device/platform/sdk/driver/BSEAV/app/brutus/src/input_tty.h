/***************************************************************************
 *     Copyright (c) 2004-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_tty.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 6/10/05 2:01p $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_tty.h $
 * 
 * 2   6/10/05 2:01p erickson
 * PR15512: added '?' help, removed output modification so that n is
 * normal
 * 
 * 1   2/7/05 8:13p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   7/6/04 3:56p erickson
 * PR11771: settop api dataflow redesign
 *
 * Irvine_BSEAVSW_Devel/2   4/16/04 4:17p erickson
 * PR9883: added 'a' key to toggle between apps in DUAL_OUTPUT mode
 *
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 *****************************************************************/
#ifndef INPUT_TTY_H__
#define INPUT_TTY_H__

#include "inputdevice.h"
#include "mtimer.h"

/**
Summary:
Single key control over a TTY (telnet or console) terminal.

Description:
This is only intended for experts. The interactive script interface (--exec command line)
is better documented and more straightforward.
**/
class TTYInput : public InputDevice, MTimerEventHandler {
public:
	TTYInput(int appIndex, MApplication *app, InputEngine *ie);
	~TTYInput();

	void process(uint32_t data);
	void onTimeout(MTimer *timer);
	void printUsage();

protected:
	int _appIndex;
	MTimer _timer;
};

#endif //INPUT_TTY_H__
