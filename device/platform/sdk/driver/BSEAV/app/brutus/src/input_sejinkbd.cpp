/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_sejinkbd.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:12p $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_sejinkbd.cpp $
 * 
 * 1   2/7/05 8:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new design, support for shift/control/capslock
 *
 *****************************************************************/
#include "input_sejinkbd.h"
#include <bstd.h>
BDBG_MODULE(brutus_input_sejinkbd);

SejinIRKeyboardInput::SejinIRKeyboardInput(MApplication *app) :
	InputDevice(app, NULL, 2) // 2 == sejin IR keyboard
{
	last = 0;
	shift = ctrl = capslock = false;
}

static unsigned long g_sejin_irkeyboard_map[128] = {
MWKEY_UNKNOWN, MWKEY_UNKNOWN, 'b', MWKEY_TAB, MWKEY_RIGHT,                              /* 0 */
' ', '\'', MWKEY_LSHIFT, '-', '[',
        /* 5 */
MWKEY_UNKNOWN, MWKEY_ENTER, MWKEY_INSERT, MWKEY_LEFT, MWKEY_UNKNOWN,    /* 10 */
MWKEY_UNKNOWN, MWKEY_UNKNOWN, MWKEY_UNKNOWN, 'a', 's',                                  /* 15 */
'd', 'f', 'j', MWKEY_UNKNOWN, 'k',
        /* 20 */
'l', MWKEY_UNKNOWN, ';', MWKEY_PAGEDOWN, MWKEY_END,                                     /* 25 */
MWKEY_LMETA /*WINDOWS*/, MWKEY_UNKNOWN, MWKEY_UNKNOWN, MWKEY_UNKNOWN, 'q',      /* 30 */
'w', 'e', 'r', 'u', MWKEY_UNKNOWN,
        /* 35 */
'i', 'o', MWKEY_UNKNOWN, 'p', ']',
        /* 40 */
MWKEY_PAUSE, MWKEY_UNKNOWN, MWKEY_UNKNOWN, MWKEY_UNKNOWN, MWKEY_UNKNOWN,        /* 45 */
'z', 'x', 'c', 'v', 'm',
                /* 50 */
MWKEY_LSHIFT, ',', '.', MWKEY_UNKNOWN, '/',                                                     /* 55 */
MWKEY_UNKNOWN, 'n', MWKEY_UNKNOWN, MWKEY_UNKNOWN, MWKEY_UNKNOWN,                /* 60 */
MWKEY_DELETE, MWKEY_F3, MWKEY_ESCAPE, MWKEY_F5, '5',                                    /* 65 */
'6', MWKEY_UNKNOWN, MWKEY_F6, MWKEY_F8, MWKEY_UNKNOWN,                                  /* 70 */
'=', MWKEY_BACKSPACE, MWKEY_HOME, MWKEY_UNKNOWN, MWKEY_UNKNOWN,
MWKEY_UNKNOWN, MWKEY_UNKNOWN, MWKEY_F2, MWKEY_F1, MWKEY_F4,
'g', 'h', MWKEY_UNKNOWN, MWKEY_F7, MWKEY_F9,
MWKEY_UNKNOWN, '\\', MWKEY_UP, MWKEY_PAGEUP, MWKEY_UNKNOWN,
MWKEY_LCTRL, MWKEY_UNKNOWN /*FUNCTION*/, MWKEY_UNKNOWN, '1', '2',
'3', '4', '7', MWKEY_UNKNOWN, '8',
'9', MWKEY_UNKNOWN, '0', MWKEY_DOWN, MWKEY_RMETA /*POPUP*/ ,
MWKEY_UNKNOWN, MWKEY_UNKNOWN, MWKEY_UNKNOWN, MWKEY_LALT, '`',
MWKEY_CAPSLOCK, MWKEY_NUMLOCK, 't', 'y', MWKEY_UNKNOWN,
MWKEY_F11, MWKEY_F10, MWKEY_UNKNOWN, MWKEY_F12, MWKEY_SYSREQ,
MWKEY_SCROLLOCK, MWKEY_UNKNOWN, MWKEY_UNKNOWN};

void SejinIRKeyboardInput::process(uint32_t data)
{
	bool keyup = data & 0x00800000;
	bool mousemove = data & 0x80000000;
	BDBG_MSG(("SejinKbd: %x", data));

	if (mousemove)
		return;

	// capture key code
	data = (data >> 8) & 0xFF;
	if (data == 0xFF) // termination code
		return;
	if (data >= sizeof(g_sejin_irkeyboard_map)/sizeof(g_sejin_irkeyboard_map[0])) {
		BDBG_ERR(("Invalid keyboard value: %x", data));
		return;
	}
	data = g_sejin_irkeyboard_map[data];

	if (data == MWKEY_LSHIFT || data == MWKEY_RSHIFT) {
		shift = !keyup;
	}
	if (data == MWKEY_LCTRL || data == MWKEY_RCTRL) {
		ctrl = !keyup;
	}

	// from this point on, keyup doesn't matter
	if (keyup) {
		if (last == data) // only clear if we're keyup on the actual last key
			last = 0;
		return;
	}
	// throw away duplicates
	if (data == last)
		return;
	last = data;

	if (data == MWKEY_CAPSLOCK)
		capslock = !capslock;

	if (capslock != shift) { /* allow capslock + shift to go lower case */
		/* capitalize */
		if (data >= 'a' && data <= 'z')
			data += 'A' - 'a';
	}
	else if (shift) {
		switch (data) {
		case '1': data = '!'; break;
		case '2': data = '@'; break;
		case '3': data = '#'; break;
		case '4': data = '$'; break;
		case '5': data = '%'; break;
		case '6': data = '^'; break;
		case '7': data = '&'; break;
		case '8': data = '*'; break;
		case '9': data = '('; break;
		case '0': data = ')'; break;
		case '-': data = '_'; break;
		case '=': data = '+'; break;
		case '[': data = '{'; break;
		case ']': data = '}'; break;
		case '\\': data = '|'; break;
		case ';': data = ':'; break;
		case '\'': data = '"'; break;
		case ',': data = '<'; break;
		case '.': data = '>'; break;
		case '/': data = '?'; break;
		case '`': data = '~'; break;
		}
	}

	BDBG_MSG(("  MWKEY %x(%c)", data, (char)data));
	app()->sendKeyDown(app()->focusedWidget(), data, 0, 0);
}
