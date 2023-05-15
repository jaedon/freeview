/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ledengine.cpp $
 * $brcm_Revision: 8 $
 * $brcm_Date: 4/27/11 4:43p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/ledengine.cpp $
 * 
 * 8   4/27/11 4:43p jrubio
 * SW7346-158: fix frontpanel for 7346
 * 
 * 7   4/28/10 7:04p mward
 * SW7125-33: Set LEDs on  BCM97019.
 * 
 * 6   8/10/09 1:39p jrubio
 * PR55232: add 7340/7342
 * 
 * 5   2/4/09 10:27a jrubio
 * PR51629: add 7336 support
 * 
 * 4   5/28/08 3:52p jrubio
 * PR42353: fix typo, for 7325
 * 
 * 3   5/23/08 11:56a jrubio
 * PR42353: remove 7335/7325 usage of the last LED
 * 
 * 2   3/17/08 3:17p erickson
 * PR40307: remove old, unused code before adding new
 * 
 * 1   2/7/05 8:14p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/14   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 * 
 * Irvine_BSEAVSW_Devel/13   9/15/04 1:53p erickson
 * PR9683: convert from B_CHIP to BCHP_CHIP in brutus
 * 
 * Irvine_BSEAVSW_Devel/12   4/21/04 2:55p erickson
 * PR10065: set 97038 leds for play/record
 * 
 * Irvine_BSEAVSW_Devel/11   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/10   10/13/03 11:47a erickson
 * 7320 only has 3
 * 
 * Irvine_BSEAVSW_Devel/9   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 * 
 * Irvine_BSEAVSW_Devel/8   9/17/03 12:31p erickson
 * converted from original debug interface to magnum debug interface
 * 
 * Irvine_BSEAVSW_Devel/7   9/10/03 3:54p erickson
 * 7328 support
 * 
 * Irvine_BSEAVSW_Devel/6   8/21/03 9:43a erickson
 * venom2 settop api work
 * 
 * Irvine_BSEAVSW_Devel/5   8/19/03 11:55a erickson
 * conversion to settop api in progress
 * 
 * Irvine_BSEAVSW_Devel/4   7/16/03 8:53a erickson
 * 7328 support
 * 
 * Irvine_BSEAVSW_Devel/3   6/16/03 10:59a erickson
 * display led MSG even for led-enabled platforms
 * 
 * SanJose_Linux_Devel/13   4/9/03 11:21a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#include "ledengine.h"
#include "bstd.h"
BDBG_MODULE(brutus_ledengine);

LedEngine::LedEngine(MApplication *app, int enabled) : _timer(app) {
	if (enabled) {
		_uo = buser_output_open(B_ID(0));
		_msecPerChar = 300;
		_timer.addHandler(this);
		setLeds(1,0,0,0);
	}
	else
		_uo = NULL;
}

LedEngine::~LedEngine() {
	if (_uo)
		buser_output_close(_uo);
}

int LedEngine::write(const char *msg) {
	BDBG_MSG(("LEDS: %s", msg));
	if (_uo) {
		char buf[5] = "    ";
		int len = strlen(msg);
		if (len > 4)
			len = 4;
		memcpy(buf, msg, len);
		buf[len] = 0;
		return buser_output_display_message(_uo, buf);
	}
	return 0;
}

void LedEngine::scroll(const char *msg, bool continuous) {
	if (_uo) {
		_scrollMsg = msg;
		_scrollIndex = 0;
		_scrollContinuous = continuous;
		// do one immediate
		onTimeout(NULL);
		_timer.start(_msecPerChar, MTimer::eForever);
	}
}

void LedEngine::onTimeout(MTimer *sender) {
	write(_scrollMsg.mid(_scrollIndex));
	if (++_scrollIndex >= _scrollMsg.length()) {
		if (_scrollContinuous)
			_scrollIndex = 0;
		else
			_timer.stop();
	}
}

int LedEngine::setLeds(int power, int play, int rec, int msg) {
	if (!_uo)
		return -1;
#if NEXUS_PLATFORM==97019
	buser_output_set_led(_uo, 7, power);	/* green */
	buser_output_set_led(_uo, 5, msg);		/* red */
#else
	buser_output_set_led(_uo, 0, power);
	buser_output_set_led(_uo, 1, play);
	buser_output_set_led(_uo, 2, rec);
    /* Led not working on 7325/7335 platform */
#if (BCHP_CHIP != 7335) && ( BCHP_CHIP != 7325 ) && (BCHP_CHIP!=7340) && (BCHP_CHIP!=7342) && (BCHP_CHIP!=7344) && (BCHP_CHIP!=7346)
     buser_output_set_led(_uo, 3, msg);
#endif
#endif
	return 0;
}
