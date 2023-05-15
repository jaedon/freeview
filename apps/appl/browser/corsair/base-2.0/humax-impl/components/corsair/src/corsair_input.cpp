/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Erik Lindström
 *
 * Implementation of the Opera HbbTV library's key input API.
 *
 */

#include <directfb.h>
#include <directfb_keynames.h>
#include <stdlib.h>
#include <iostream>

#include "input_api.h"

#include "corsair_utils.h"
#include "corsair_input.h"
#include "corsair_gfx.h"
#include "corsair.h"
#ifdef OIPF
# include "asteriks.h"
#endif// OIPF

//#include "NativeApplicationManager.h"

enum {
	KEY_RED = 0x1,
	KEY_GREEN = 0x2,
	KEY_YELLOW = 0x4,
	KEY_BLUE = 0x8,
	KEY_NAVIGATION = 0x10,
	KEY_VCR = 0x20,
	KEY_SCROLL = 0x40,
	KEY_INFO = 0x80,
	KEY_NUMERIC = 0x100,
	KEY_ALPHA = 0x200
};

IDirectFB* g_dfb_instance = NULL;
IDirectFBSurface* g_primary_surface;
IDirectFBEventBuffer *g_input = NULL;

/* ssyoo : KEY_SCROLL and KEY_INFO need to use the VK_INFO and VK_PAGEUP, VK_PAGEDOWN on Picasa and YouTube */
int g_max_key_mask = KEY_RED | KEY_GREEN | KEY_YELLOW | KEY_BLUE | KEY_NAVIGATION | KEY_VCR | KEY_SCROLL | KEY_INFO | KEY_NUMERIC|KEY_ALPHA;

using namespace std;
extern Corsair* g_corsair;


int hbbtv_keyset_get_supported_values()
{
	return g_max_key_mask;
}


void corsair_input_init()
{
	CorsairGfx *corsGfx = CorsairGfx::getInstance();
	if (corsGfx)
	{
		g_dfb_instance = corsGfx->getDfbInstance();
		g_primary_surface = corsGfx->getPrimarySurface();
	}
	DFBCHECK(g_dfb_instance->CreateInputEventBuffer(g_dfb_instance, DICAPS_ALL, DFB_TRUE, &g_input));
}


void corsair_input_shutdown()
{
	DFBCHECK(g_input->Release(g_input));
}


void corsair_send_keypress(DFBInputEventType type,
						   DFBInputDeviceKeySymbol symbol,
						   DFBInputDeviceKeyIdentifier id,
						   DFBInputDeviceModifierMask modifiers)
{
#if 0
	// Buffer to keep key value in utf8 format.
	char key_val[10] = {0,0,0,0,0,0,0,0,0,0};

	GOGI_KEYBOARD_EVENT gogi_evt;
	gogi_evt.key_repeat = 0;

	dfb_to_gogi_key_symbol(type, symbol, id, modifiers, gogi_evt, key_val);
	gogi_evt.key_value = key_val;

	if (gogi_evt.key_type == GOGI_KEYDOWN && corsair_process_key(gogi_evt))
		return;

	GogiOperaWindow* focused_window = g_corsair->getFocusedWindow();
	if (!focused_window)
	{
		LOG_DEBUG("input when no focused window\n");
		return;
	}
	else if (opera_hbbtv_is_hbbtv_window(focused_window))
	{
		bool consumed_key = opera_hbbtv_key_input(focused_window, &gogi_evt);
	}
	else
	{
		gogi_keyboard_evt(&gogi_evt);
	}
#endif
}


bool corsair_process_key(GOGI_KEYBOARD_EVENT& key_event)
{
	switch(key_event.key)
	{
		case GOGI_KEY_ESCAPE:
		{
			GogiOperaWindow* focused_window = g_corsair->getFocusedWindow();
			if (focused_window && opera_hbbtv_is_hbbtv_window(focused_window))
			{
				LOG_DEBUG("Terminate HbbTV application\n");
				// terminate running hbbtv application
				HBBTV_WindowIdentifier hbbtv_win = opera_hbbtv_get_window_identifier(focused_window);
				if (!hbbtv_win)
					return false;

				HBBTV_EventData* data = static_cast<HBBTV_EventData*>(malloc(sizeof(HBBTV_EventData)));
				data->app.hbbtv_window = hbbtv_win;
				opera_hbbtv_send_event(E_TERMINATE_APP, data);

				return true;
			}
			else
				return false;
		}


		case GOGI_KEY_F9:
		{
			static bool flip = false;
			if (flip)
				// Show HbbTV screen for running HbbTV applications
				g_corsair->showHbbTVSurface();
			else
				g_corsair->showOpenWebSurface();
			flip = !flip;

			return true;
		}

		case GOGI_KEY_F10:
		{
			static bool show_web = true;
			if (show_web)
			// Show Web screen for Internet surfing
			g_corsair->showOpenWebSurface();
#ifdef OIPF
			else
				// Show OIPF screen for running OIPF applications
				static_cast<Asteriks*>(g_corsair)->showOipfSurface();
			show_web = !show_web;
#endif // OIPF
			
			return true;
		}


		case GOGI_KEY_F12:
		{
			g_corsair->stop();
			return true;
		}

		default:
			return false;
	}
}


void increase_usecs(struct timeval &time, int usecs)
{
	time.tv_sec  += (time.tv_usec + usecs) / 1000000;
	time.tv_usec  = (time.tv_usec + usecs) % 1000000;
	return;
}

void corsair_read_input(int delay)
{
	struct timeval timeout;
	gettimeofday(&timeout, NULL);
	increase_usecs(timeout, delay*1000);

	// Calculate the number of milliseconds to the next timeout.
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	int msec_timeout = (timeout.tv_sec-currentTime.tv_sec)*1000
					 + (timeout.tv_usec-currentTime.tv_usec)/1000;

	// Wait for input events, but only at most as long as opera says
	// it has nothing to do.
	if (g_input->WaitForEventWithTimeout(g_input,
										 msec_timeout/1000,
										 msec_timeout%1000) == DFB_INTERRUPTED)
	{
		// If we have been woken up, just return immediately.
		return;
	}

	while (g_input->HasEvent(g_input) == DFB_OK)
	{
		DFBEvent evt;
		DFBCHECK(g_input->GetEvent(g_input, &evt));

		// Input event
		if (evt.clazz == DFEC_INPUT)
		{
			DFBInputEventType type = evt.input.type;

			if (type == DIET_KEYPRESS || type == DIET_KEYRELEASE)
			{
				corsair_send_keypress(type, evt.input.key_symbol, evt.input.key_id, evt.input.modifiers);
			}
		}
	}
	return;
}


// Convert the 4 byte unicode input to utf-8. The output should be a pre allocated
// character array at least 6 bytes wide. The output_len will be set to resulting
// utf-8 width.
void corsair_unicode_to_utf8(unsigned int input, char* output, unsigned int& output_len)
	{
	output_len = 0;

	// 0xxxxxxx
	if(input < 0x80)
			{
		*output++ = (unsigned char)input;
		output_len = 1;
			}
	// 110xxxxx 10xxxxxx
	else if(input < 0x800)
	{
		*output++  = (unsigned char)(0xc0 | input >> 6);
		*output++  = (unsigned char)(0x80 | input & 0x3f);
		output_len = 2;
	}
	// 1110xxxx 10xxxxxx 10xxxxxx
	else if(input < 0x10000)
	{
		*output++  = (unsigned char)(0xe0 | input >> 12);
		*output++  = (unsigned char)(0x80 | input >> 6 & 0x3f);
		*output++  = (unsigned char)(0x80 | input & 0x3f);
		output_len = 3;
	}
	// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	else if(input < 0x200000)
	{
		*output++  = (unsigned char)(0xf0 | input >> 18);
		*output++  = (unsigned char)(0x80 | input >> 12 & 0x3f);
		*output++  = (unsigned char)(0x80 | input >> 6 & 0x3f);
		*output++  = (unsigned char)(0x80 | input & 0x3f);
		output_len = 4;
	}
	// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	else if(input < 0x4000000)
	{
		*output++  = (unsigned char)(0xf8 | input >> 24);
		*output++  = (unsigned char)(0x80 | input >> 18 & 0x3f);
		*output++  = (unsigned char)(0x80 | input >> 12 & 0x3f);
		*output++  = (unsigned char)(0x80 | input >> 6 & 0x3f);
		*output++  = (unsigned char)(0x80 | input & 0x3f);
		output_len = 5;
	}
	// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	else if(input < 0x8000000)
	{
		*output++  = (unsigned char)(0xfc | input >> 30);
		*output++  = (unsigned char)(0x80 | input >> 24 & 0x3f);
		*output++  = (unsigned char)(0x80 | input >> 18 & 0x3f);
		*output++  = (unsigned char)(0x80 | input >> 12 & 0x3f);
		*output++  = (unsigned char)(0x80 | input >> 6 & 0x3f);
		*output++  = (unsigned char)(0x80 | input & 0x3f);
		output_len = 6;
	}
}


