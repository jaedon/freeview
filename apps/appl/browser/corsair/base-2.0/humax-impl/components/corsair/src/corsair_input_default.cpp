/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * This file contains platform specific key bindings.
 *
 *
 */

#ifdef CORSAIR_INPUT_DEFAULT

#include <directfb.h>
#include <directfb_keynames.h>

#include "corsair.h"
#include "corsair_input.h"
#include "gogi_opera_api.h"
#include "input_api.h"

extern Corsair* g_corsair;


void dfb_to_gogi_key_symbol(DFBInputEventType type,
							DFBInputDeviceKeySymbol symbol,
							int keycode,
							DFBInputDeviceKeyIdentifier id,
							DFBInputDeviceModifierMask modifiers,
							GOGI_KEYBOARD_EVENT& gogi_evt,
							char* key_value, unsigned int flags)
{
	gogi_evt.key_type = (type == DIET_KEYPRESS) ? GOGI_KEYDOWN : GOGI_KEYUP;
	gogi_evt.key_modifiers = GOGI_KEYMOD_NONE;
	gogi_evt.key_value_length = 0;

	if (modifiers & DIMM_SHIFT) {
		gogi_evt.key_modifiers |= GOGI_KEYMOD_SHIFT;
	}
	if (modifiers & DIMM_CONTROL) {
		gogi_evt.key_modifiers |= GOGI_KEYMOD_CTRL;
	}
	if (modifiers & DIMM_ALT) {
		gogi_evt.key_modifiers |= GOGI_KEYMOD_ALT;
	}
	if (modifiers & DIMM_META) {
		gogi_evt.key_modifiers |= GOGI_KEYMOD_META;
	}
	if (modifiers & DIMM_SUPER) {
		gogi_evt.key_modifiers |= GOGI_KEYMOD_SUPER;
	}

	if (DFB_KEY_TYPE(id) == DIKT_IDENTIFIER)
	{
		switch (id)
		{
			case DIKI_F1:
				gogi_evt.key = GOGI_KEY_RED;
				break;
			case DIKI_F2:
				gogi_evt.key = GOGI_KEY_GREEN;
				break;
			case DIKI_F3:
				gogi_evt.key = GOGI_KEY_YELLOW;
				break;
			case DIKI_F4:
				gogi_evt.key = GOGI_KEY_BLUE;
				break;
			case DIKI_F5:
				gogi_evt.key = GOGI_KEY_F5;
				break;
			case DIKI_F6:
				gogi_evt.key = GOGI_KEY_F6;
				break;
			case DIKI_F7:
				gogi_evt.key = GOGI_KEY_F7;
				break;
			case DIKI_F8:
				gogi_evt.key = GOGI_KEY_F8;
				break;
			case DIKI_F9:
				gogi_evt.key = GOGI_KEY_F9;
				break;
			case DIKI_F10:
				gogi_evt.key = GOGI_KEY_F10;
				break;
			case DIKI_F11:
				gogi_evt.key = GOGI_KEY_F11;
				break;
			case DIKI_F12:
				gogi_evt.key = GOGI_KEY_F12;
				break;
			case DIKI_HOME:
				gogi_evt.key = GOGI_KEY_HOME;
				break;
			case DIKI_END:
				gogi_evt.key = GOGI_KEY_END;
				break;
			case DIKI_PAGE_UP:
				gogi_evt.key = GOGI_KEY_PAGEUP;
				break;
			case DIKI_PAGE_DOWN:
				gogi_evt.key = GOGI_KEY_PAGEDOWN;
				break;
			case DIKI_UP:
				gogi_evt.key = GOGI_KEY_UP;
				break;
			case DIKI_DOWN:
				gogi_evt.key = GOGI_KEY_DOWN;
				break;
			case DIKI_LEFT:
				gogi_evt.key = GOGI_KEY_LEFT;
				break;
			case DIKI_RIGHT:
				gogi_evt.key = GOGI_KEY_RIGHT;
				break;
			case DIKI_ESCAPE:
				gogi_evt.key = GOGI_KEY_ESCAPE;
				break;
			case DIKI_KP_MINUS:
				gogi_evt.key = GOGI_KEY_SUBTRACT;
				break;
			case DIKI_KP_PLUS:
				gogi_evt.key = GOGI_KEY_ADD;
				break;
			case DIKI_KP_MULT:
				gogi_evt.key = GOGI_KEY_MULTIPLY;
				break;
			case DIKI_KP_DIV:
				gogi_evt.key = GOGI_KEY_DIVIDE;
				break;
			case DIKI_INSERT:
				gogi_evt.key = GOGI_KEY_INSERT;
				break;
			case DIKI_DELETE:
				gogi_evt.key = GOGI_KEY_DELETE;
				break;
			case DIKI_BACKSPACE:
				gogi_evt.key = GOGI_KEY_BACKSPACE;
				break;
			case DIKI_TAB:
				gogi_evt.key = GOGI_KEY_TAB;
				key_value[0] = '\t';
				gogi_evt.key_value_length = 1;
				break;
			case DIKI_SPACE:
				gogi_evt.key = GOGI_KEY_SPACE;
				break;
			case DIKI_KP_ENTER:
			case DIKI_ENTER:
				gogi_evt.key = GOGI_KEY_ENTER;
				break;
			case DIKI_ALT_L:
			case DIKI_ALT_R:
				gogi_evt.key = GOGI_KEY_ALT;
				break;
			case DIKI_SHIFT_L:
			case DIKI_SHIFT_R:
				gogi_evt.key = GOGI_KEY_SHIFT;
				break;
			case DIKI_CONTROL_L:
			case DIKI_CONTROL_R:
				gogi_evt.key = GOGI_KEY_CTRL;
				break;
			case DIKI_CAPS_LOCK:
				gogi_evt.key = GOGI_KEY_CAPS_LOCK;
				break;
			case DIKI_SCROLL_LOCK:
				gogi_evt.key = GOGI_KEY_SCROLL_LOCK;
				break;
			case DIKI_NUM_LOCK:
				gogi_evt.key = GOGI_KEY_NUM_LOCK;
				break;
			case DIKI_PRINT:
				gogi_evt.key = GOGI_KEY_PRINTSCREEN;
				break;
			case DIKI_PAUSE:
				gogi_evt.key = GOGI_KEY_PAUSE;
				break;
			default:
				if (DFB_KEY_TYPE(symbol) == DIKT_IDENTIFIER)
				{
					gogi_evt.key = (GOGI_VIRTUAL_KEY)((symbol&0x00ff)+0x40);
				}
				else if (DFB_KEY_TYPE(symbol) == DIKT_UNICODE)
				{
					gogi_evt.key = (GOGI_VIRTUAL_KEY)DFB_UPPER_CASE(symbol);
					corsair_unicode_to_utf8(symbol, key_value, gogi_evt.key_value_length);
				}
		}
	}

	else if (DFB_KEY_TYPE(symbol) == DIKT_UNICODE)
	{
		switch (symbol)
		{
		case DIKS_RETURN:
			gogi_evt.key = GOGI_KEY_ENTER;
			break;
		default:
			gogi_evt.key = (GOGI_VIRTUAL_KEY)DFB_UPPER_CASE(symbol);
			corsair_unicode_to_utf8(symbol, key_value, gogi_evt.key_value_length);
			break;
		}
	}

	else if ((symbol & 0xFF00) == DIKT_SPECIAL)
	{
		switch (symbol)
		{
			case DIKS_ENTER:
				gogi_evt.key = GOGI_KEY_ENTER;
				break;
			case DIKS_CURSOR_LEFT:
				gogi_evt.key = GOGI_KEY_LEFT;
				break;
			case DIKS_CURSOR_RIGHT:
				gogi_evt.key = GOGI_KEY_RIGHT;
				break;
			case DIKS_CURSOR_UP:
				gogi_evt.key = GOGI_KEY_UP;
				break;
			case DIKS_CURSOR_DOWN:
				gogi_evt.key = GOGI_KEY_DOWN;
				break;
			case DIKS_CLEAR:
				gogi_evt.key = GOGI_KEY_BACKSPACE;
				break;
			case DIKS_SELECT:
				// Added a dot to type ip adresses
				gogi_evt.key = (GOGI_VIRTUAL_KEY)'.';
				break;
			case DIKS_POWER:
			case DIKS_POWER2:
				g_corsair->stop();
				break;
			default:
				gogi_evt.key = (GOGI_VIRTUAL_KEY)(symbol & 0xFF);
				break;
		}
	}

	else
	{
		gogi_evt.key = (GOGI_VIRTUAL_KEY)(symbol & 0xFF);
	}
}
#endif //CORSAIR_INPUT_DEFAULT
