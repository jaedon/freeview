/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ledengine.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/17/08 3:17p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/ledengine.h $
 * 
 * 2   3/17/08 3:17p erickson
 * PR40307: remove old, unused code before adding new
 * 
 * 1   2/7/05 8:14p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/3   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/2   8/19/03 11:55a erickson
 * conversion to settop api in progress
 * 
 * SanJose_Linux_Devel/4   4/9/03 11:21a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#ifndef LEDENGINE_H
#define LEDENGINE_H

#include "mtimer.h"
#include "mstring.h"
#include "bsettop.h"

/**
* LedEngine allows you to write or scroll a message
* across the set-top's front panel leds.
*/
class LedEngine : public MTimerEventHandler {
public:
	LedEngine(MApplication *app, int enabled);
	virtual ~LedEngine();

	void clear() {write("    ");}
	int write(const char *msg);
	void setScrollSpeed(int msecPerChar) {_msecPerChar = msecPerChar;}
	void scroll(const char *msg, bool continuous = false);

	int setLeds(int power, int play, int rec, int msg);

	// event
	void onTimeout(MTimer *sender);

protected:
	buser_output_t _uo;
	int _msecPerChar;
	int _scrollIndex;
	MString _scrollMsg;
	bool _scrollContinuous;
	MTimer _timer;
};

#endif
