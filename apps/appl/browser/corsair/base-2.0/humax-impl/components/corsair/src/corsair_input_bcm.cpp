/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * Key binding implementation for the Broadcom 97335 device.
 *
 */

#ifdef CORSAIR_INPUT_BROADCOM

#include <iostream>
using namespace std;

#include <directfb.h>
#include <directfb_keynames.h>
#include <cstring>
#include "gogi_opera_api.h"
#include "input_api.h"
#include "corsair_input.h"
#include "corsair_log.h"
#include "humax_custom_key.h"

#define	GOGI_SYMBOL(diks, gogi, value) \
	case	diks: \
	gogi_evt.key = (GOGI_VIRTUAL_KEY)gogi; \
	gogi_evt.key_value = value; \
	gogi_evt.key_value_length = (gogi_evt.key_value) ? strlen(value) : 0; \
	break

// when flags is setting
void dfb_to_gogi_key_symbol_flags(unsigned int  type,
									unsigned int symbol,
									int keycode,
									unsigned int id,
									GOGI_KEYBOARD_EVENT &gogi_evt,
									char* key_value, unsigned int flags)
{
	switch (symbol)
	{
#if defined(CONFIG_PROD_HD9000I)
		GOGI_SYMBOL(DIKS_CHANNEL_UP,	GOGI_KEY_FRONT_UP, NULL);
		GOGI_SYMBOL(DIKS_CHANNEL_DOWN,	GOGI_KEY_FRONT_DOWN, NULL);
		GOGI_SYMBOL(DIKS_VOLUME_UP, 	GOGI_KEY_FRONT_RIGHT, NULL);
		GOGI_SYMBOL(DIKS_VOLUME_DOWN,	GOGI_KEY_FRONT_LEFT, NULL);
		GOGI_SYMBOL(DIKS_SELECT,		GOGI_KEY_ENTER, NULL);
#endif
        GOGI_SYMBOL(DIKS_CUSTOM67,		GOGI_KEY_FRONT_SW_REBOOT, NULL);			// NULSTIL on front
		default:
			break;
	}
}

void dfb_to_gogi_key_symbol_base(unsigned int  type,
									unsigned int symbol,
									int keycode,
									unsigned int id,
									GOGI_KEYBOARD_EVENT &gogi_evt,
									char* key_value, unsigned int flags)
{
	if (symbol == 0x0000)
	{
		unsigned char	decoded_code, inverse_code;

		decoded_code = keycode & 0x00FF;
		inverse_code = ~((keycode & 0xFF00) >> 8);

		switch (decoded_code)
		{
			case	0x84:		//	KEY STANDBY
				gogi_evt.key = GOGI_KEY_POWER;
				break;
			case	0x81:		//	UNIVERSAL TV
			case	0x80:		//	UNIVERSAL PVR
			default:
				gogi_evt.key = (GOGI_VIRTUAL_KEY)symbol;
				break;
		}
	}
	else
	{
		switch (symbol)
		{
			GOGI_SYMBOL(DIKS_INTERNET,		GOGI_KEY_HOME, NULL);
			GOGI_SYMBOL(DIKS_PLAYER,		GOGI_KEY_PLAYER, NULL);
			GOGI_SYMBOL(DIKS_RED,			GOGI_KEY_RED, NULL);
			GOGI_SYMBOL(DIKS_GREEN, 		GOGI_KEY_GREEN, NULL);
			GOGI_SYMBOL(DIKS_YELLOW,		GOGI_KEY_YELLOW, NULL);
			GOGI_SYMBOL(DIKS_BLUE,			GOGI_KEY_BLUE, NULL);
			GOGI_SYMBOL(DIKS_VOLUME_UP, 	GOGI_KEY_VOLUME_UP, NULL);
			GOGI_SYMBOL(DIKS_VOLUME_DOWN,	GOGI_KEY_VOLUME_DOWN, NULL);
			GOGI_SYMBOL(DIKS_OPTION,		GOGI_KEY_OPTION, NULL);
			GOGI_SYMBOL(DIKS_INFO,			GOGI_KEY_INFO, NULL);
			GOGI_SYMBOL(DIKS_CHANNEL_UP,	GOGI_KEY_CHANNEL_UP, NULL);
			GOGI_SYMBOL(DIKS_CHANNEL_DOWN,	GOGI_KEY_CHANNEL_DOWN, NULL);
			GOGI_SYMBOL(DIKS_EXIT,			GOGI_KEY_ESCAPE, NULL);
#if defined(CONFIG_PROD_HD9000I)
			GOGI_SYMBOL(DIKS_LAST,			GOGI_KEY_VOD, NULL);
#elif defined(CONFIG_PROD_CDIG2000C)
			GOGI_SYMBOL(DIKS_LAST,			GOGI_KEY_BACK, NULL);
#else
			GOGI_SYMBOL(DIKS_LAST,			GOGI_KEY_LAST, NULL);
#endif
			GOGI_SYMBOL(DIKS_CURSOR_DOWN,	GOGI_KEY_DOWN, NULL);
			GOGI_SYMBOL(DIKS_CURSOR_UP, 	GOGI_KEY_UP, NULL);
			GOGI_SYMBOL(DIKS_CURSOR_LEFT,	GOGI_KEY_LEFT, NULL);
			GOGI_SYMBOL(DIKS_CURSOR_RIGHT,	GOGI_KEY_RIGHT, NULL);
			GOGI_SYMBOL(DIKS_MENU,			GOGI_KEY_MENU, NULL);
			GOGI_SYMBOL(DIKS_HOME,			GOGI_KEY_MENU, NULL);
			GOGI_SYMBOL(DIKS_EPG,			GOGI_KEY_GUIDE, NULL);
			GOGI_SYMBOL(DIKS_PREVIOUS,		GOGI_KEY_TRACK_PREV, NULL);
			GOGI_SYMBOL(DIKS_STOP,			GOGI_KEY_STOP, NULL);
			GOGI_SYMBOL(DIKS_NEXT,			GOGI_KEY_TRACK_NEXT, NULL);
			GOGI_SYMBOL(DIKS_REWIND,		GOGI_KEY_REWIND, NULL);
			GOGI_SYMBOL(DIKS_PLAY,			GOGI_KEY_PLAY, NULL);
			GOGI_SYMBOL(DIKS_PAUSE, 		GOGI_KEY_PAUSE, NULL);
			GOGI_SYMBOL(DIKS_PLAYPAUSE, 	GOGI_KEY_PLAY_PAUSE, NULL);
			GOGI_SYMBOL(DIKS_FASTFORWARD,	GOGI_KEY_FAST_FWD, NULL);
			GOGI_SYMBOL(DIKS_MUTE,			GOGI_KEY_MUTE, NULL);
			GOGI_SYMBOL(DIKS_RECORD,		GOGI_KEY_RECORD, NULL);
			GOGI_SYMBOL(DIKS_CUSTOM4,		GOGI_KEY_LIST, NULL);			//	Channel List
			GOGI_SYMBOL(DIKS_TV,			GOGI_KEY_TVRADIO_TOGGLE, NULL);
			GOGI_SYMBOL(DIKS_ZOOM,			GOGI_KEY_SCREEN_MODE_NEXT, NULL);		//	Wide
			GOGI_SYMBOL(DIKS_TEXT,			GOGI_KEY_TELETEXT, NULL);
			GOGI_SYMBOL(DIKS_POWER, 		GOGI_KEY_POWER, NULL);
			GOGI_SYMBOL(DIKS_ENTER, 		GOGI_KEY_ENTER, NULL);
			GOGI_SYMBOL(DIKS_AUDIO, 		GOGI_KEY_SURROUND_MODE_NEXT, NULL);
			GOGI_SYMBOL(DIKS_SUBTITLE,		GOGI_KEY_SUBTITLE, NULL);
			GOGI_SYMBOL(DIKS_CUSTOM5,		GOGI_KEY_SETTING, NULL);
			GOGI_SYMBOL(DIKS_CUSTOM6,		GOGI_KEY_SEARCH, NULL);
			GOGI_SYMBOL(DIKS_CUSTOM7,		GOGI_KEY_MULTI_CUSTOM, NULL);
			GOGI_SYMBOL(DIKS_CUSTOM8,		GOGI_KEY_MULTI_CUSTOM_SUCCESS, NULL);
			GOGI_SYMBOL(DIKS_CUSTOM9,		GOGI_KEY_MULTI_CUSTOM_FAIL, NULL);
			// JAPAN
			GOGI_SYMBOL(DIKS_FAVORITES, 	GOGI_KEY_RECALL_FAVORITE_0, NULL);	// HKS_FAVORITES
			GOGI_SYMBOL(DIKS_CUSTOM61,		GOGI_KEY_DATA_BROADCAST, NULL); 	// HKS_DATA_BROADCAST
			GOGI_SYMBOL(DIKS_PROGRAM,		GOGI_KEY_PROGRAM, NULL);				// HKS_PROGRAM
			GOGI_SYMBOL(DIKS_DIGITS,		GOGI_KEY_DIGITS, NULL); 			// HKS_DIGITS
			GOGI_SYMBOL(DIKS_CUSTOM45,		GOGI_KEY_VOD, NULL);					// HKS_VOD
			GOGI_SYMBOL(DIKS_CUSTOM23,		GOGI_KEY_BROWSER, NULL);				// HKS_BROWSER, FULL_BROWSER
			GOGI_SYMBOL(DIKS_CUSTOM56,		GOGI_KEY_DTT, NULL);					// HKS_DTT
			GOGI_SYMBOL(DIKS_CUSTOM57,		GOGI_KEY_BS, NULL); 				// HKS_BS
			GOGI_SYMBOL(DIKS_CUSTOM58,		GOGI_KEY_CATV, NULL);				// HKS_CATV
			GOGI_SYMBOL(DIKS_CUSTOM10,		GOGI_KEY_11, NULL); 				// HKS_11
			GOGI_SYMBOL(DIKS_CUSTOM11,		GOGI_KEY_12, NULL); 				// HKS_12
			GOGI_SYMBOL(DIKS_BACK,			GOGI_KEY_BACK, NULL);				// HKS_BACK
			GOGI_SYMBOL(DIKS_SCREEN,		GOGI_KEY_SCREEN_MODE_NEXT, NULL);	// HKS_MODE
			GOGI_SYMBOL(DIKS_SETUP, 		GOGI_KEY_SETTING, NULL);				// HKS_SETUP
			GOGI_SYMBOL(DIKS_CUSTOM16,		GOGI_KEY_STORE_FAVORITE_0, NULL);	// HKS_PRESET_CH
			GOGI_SYMBOL(DIKS_CUSTOM54,		GOGI_KEY_RECOMMAND, NULL);			// HKS_RECOMMAND
			GOGI_SYMBOL(DIKS_CUSTOM53,		GOGI_KEY_ETC, NULL);					// HKS_ETC
			GOGI_SYMBOL(DIKS_SHIFT, 		GOGI_KEY_SHIFT, NULL);				// HKS_SHIFT
			GOGI_SYMBOL(DIKS_CLEAR, 		GOGI_KEY_CLEAR, NULL);				// HKS_CLEAR
			GOGI_SYMBOL(DIKS_CUSTOM59,		GOGI_KEY_BROADCAST_NETWORK, NULL);	// 방송전화(FRONT 버튼)
			GOGI_SYMBOL(DIKS_CUSTOM98,		GOGI_KEY_WPS, NULL);					// WPS(FRONT 버튼)
			GOGI_SYMBOL(DIKS_CUSTOM99,		GOGI_KEY_HIDDEN_MENU, NULL);			// Hidden(CH+, CH-) (FRONT 버튼)
			GOGI_SYMBOL(DIKS_CUSTOM65,		GOGI_KEY_REPLAY, NULL); 		// replay
			GOGI_SYMBOL(DIKS_CUSTOM66,		GOGI_KEY_SKIP, NULL);			// skip
			GOGI_SYMBOL(DIKS_CUSTOM68,      GOGI_KEY_NETFLIX, NULL);            // NULSTIL on front

			default:
#if 1
				if (DFB_KEY_TYPE(symbol) == DIKT_IDENTIFIER)
				{
					gogi_evt.key = (GOGI_VIRTUAL_KEY)((symbol&0x00ff)+0x40);
				}
				else if (DFB_KEY_TYPE(symbol) == DIKT_UNICODE)
				{
					gogi_evt.key = (GOGI_VIRTUAL_KEY)DFB_UPPER_CASE(symbol);
					corsair_unicode_to_utf8(symbol, key_value, gogi_evt.key_value_length);
				}
				else
				{
					gogi_evt.key = (GOGI_VIRTUAL_KEY)(symbol & 0xFF);
				}
#endif
				break;
		}
	}

}
void dfb_to_gogi_key_symbol(unsigned int  type,
							unsigned int symbol,
							int keycode,
							unsigned int id,
							unsigned int modifiers,
							GOGI_KEYBOARD_EVENT &gogi_evt,
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

	dfb_to_gogi_key_symbol_base(type, symbol, keycode,id, gogi_evt,key_value, flags);

	if(flags & DIEF_BUTTONS) //when front key is pressed , send DIEF_BUTTONS flags defined in directfb.h
	{
		dfb_to_gogi_key_symbol_flags(type, symbol, keycode,id, gogi_evt,key_value, flags);
	}
	LOG_DEBUG("## modifier:0x%x  keycode:0x%x  DFBKEY:0x%x  GOGIKEY:0x%x  gogi_evn:0x%x \n",modifiers,keycode,symbol,gogi_evt.key,gogi_evt.key_type);
}
#endif //CORSAIR_INPUT_BROADCOM
