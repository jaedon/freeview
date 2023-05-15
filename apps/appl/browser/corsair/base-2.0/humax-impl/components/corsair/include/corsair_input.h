/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 */

#ifndef __CORSAIR_INPUT_H__
#define __CORSAIR_INPUT_H__

#include <directfb.h>
#include "corsair_config.h"
#include "gogi_opera_api.h"


void corsair_send_keypress(DFBInputEventType type,
			               DFBInputDeviceKeySymbol symbol,
			               DFBInputDeviceKeyIdentifier id,
			               DFBInputDeviceModifierMask modifiers);

void corsair_read_input(int delay);
bool corsair_process_key(GOGI_KEYBOARD_EVENT& key);
void corsair_input_init();
void corsair_input_shutdown();

void dfb_to_gogi_key_symbol(unsigned int  type,
							unsigned int symbol,
							int keycode,
							unsigned int id,
							unsigned int modifiers,
							GOGI_KEYBOARD_EVENT &gogi_evt,
							char* key_value, unsigned int flags);
void corsair_unicode_to_utf8(unsigned int input, char* output, unsigned int& output_len);

#endif // __CORSAIR_INPUT_H__
