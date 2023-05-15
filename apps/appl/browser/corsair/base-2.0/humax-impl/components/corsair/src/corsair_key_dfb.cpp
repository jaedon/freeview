/**************************************************************************************/
/**
 * @file corsair_key.cpp
 *
 * Key event management
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com)
 * @date    2012/03/17
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "corsair_key_dfb.h"
#include "corsair_input.h"
#include "corsair_appmgr.h"
#include "corsair_web_surface.h"
#include "corsair_utils.h"
#include "corsair_gfx.h"
#include "corsair.h"

#include "oplbroadcast.h"
#include "hlib.h"

#include <cstdio>
#include <iostream>
#include <directfb_keynames.h>

using namespace std;

CorsairKeyManager::CorsairKeyManager()
{
}

CorsairKeyManager::~CorsairKeyManager()
{
}

void CorsairKeyManager::pushKey(DFBEvent event)
{
	push(event);
}

bool CorsairKeyManager::process()
{
	for ( int i=0; m_keyQueue.size(); ++i )
	{
		DFBEvent event = pop();
		GOGI_KEY_SYM gogi_sym = getGogiKeySym(event);

		if ( gogi_sym == GOGI_HBBTV_VK_UNDEFINED )
		{
			m_keyQueue.pop();
			continue;
		}

		if ( gogi_sym == GOGI_HBBTV_VK_TELETEXT )
		{
			CorsairApplicationManager::runTeletext();
			m_keyQueue.pop();
			continue;
		}

		(void)inputKey(event);

		m_keyQueue.pop();
	}

	return true;
}

//-----------------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------------
void CorsairKeyManager::push(DFBEvent event)
{
	m_keyQueue.push(event);
}

DFBEvent CorsairKeyManager::pop()
{
	if ( isEmpty() ) return DFBEvent();

	return m_keyQueue.front();
}

bool CorsairKeyManager::isEmpty()
{
	return m_keyQueue.empty();
}

bool CorsairKeyManager::inputHbbtvKey(DFBEvent event)
{
	GOGI_KEY_SYM gogi_sym = getGogiKeySym(event);

	if ( gogi_sym == GOGI_HBBTV_VK_ESCAPE )
	{
		CorsairApplicationManager::exitTVPortal();
	}
	else if ( gogi_sym == GOGI_HBBTV_VK_HOME )
	{
		CorsairApplicationManager::runTVPortal();
	}
	else
	{
		GogiOperaWindow* focused_window = Corsair::getFocusedWindow();
		if ( opera_hbbtv_key_input(focused_window, GOGI_KEYDOWN, gogi_sym, 0) == false
				|| opera_hbbtv_key_input(focused_window, GOGI_KEYUP, gogi_sym, 0) == false )
		{
			getCorsair()->showOpenWebSurface();
			inputOpenWebKey(event);
			getCorsair()->showHbbTVSurface();
			return false;
		}
	}

	returnKey(event, true);

	return true;
}

bool CorsairKeyManager::inputOpenWebKey(DFBEvent event)
{
	GOGI_KEY_SYM gogi_sym = getGogiKeySym(event);

#if 0 // This is temporary work around for Lost Key Problem.
	if ( gogi_sym == GOGI_HBBTV_VK_ESCAPE )
	{
		getCorsair()->getWebSurface()->open();
	}
	else
#endif
	if ( gogi_sym == GOGI_HBBTV_VK_HOME )
	{
		CorsairApplicationManager::runTVPortal();
	}
	else
	{
		if ( gogi_keyboard_evt(GOGI_KEYDOWN, gogi_sym, GOGI_KEYMOD_NONE) != GOGI_STATUS_OK
				|| gogi_keyboard_evt(GOGI_KEYUP, gogi_sym, GOGI_KEYMOD_NONE) != GOGI_STATUS_OK )
		{

			return inputNativeKey(event);
		}
	}

	returnKey(event, true);

	return true;
}

bool CorsairKeyManager::inputNativeKey(DFBEvent event)
{
	returnKey(event, false);

	return true;
}


bool CorsairKeyManager::inputKey(DFBEvent event)
{
	gogi_bool is_visible = false;

	GogiOperaWindow* focused_window = Corsair::getFocusedWindow();
	if ( !focused_window )
	{
		return inputNativeKey(event);
	}
	else if ( opera_hbbtv_is_hbbtv_window(focused_window) )
	{
		return inputHbbtvKey(event);
	}
	else if ( op_is_window_visible(focused_window, &is_visible) == GOGI_STATUS_OK
			&& is_visible == true )
	{
		return inputOpenWebKey(event);
	}
	else
	{
		return inputNativeKey(event);
	}

	return true;
}

void CorsairKeyManager::returnKey(DFBEvent event, bool consumed)
{
#if 0
	DFBWindowEvent  windowEvent;
	IDirectFBWindow *retWindow = NULL;
	CorsairGfx *corsair_gfx = CorsairGfx::getInstance();
	if ( corsair_gfx == NULL ) return;
	IDirectFBDisplayLayer *layer = corsair_gfx->getLayer();

	DFBWindowID fromWindowId = HxMACRO_GetDFBWindowIdFromEvent(event.window);
	DFBCHECK(layer->GetWindow(layer, fromWindowId, &retWindow));
	HxMACRO_SetDFBMessage(windowEvent, DFEC_WINDOW, DWET_KEYDOWN, DWEF_RETURNED,
			event.window.key_symbol, event.window.window_id, fromWindowId, consumed);

	retWindow->SendEvent(retWindow, &windowEvent);
#endif
}

GOGI_VIRTUAL_KEY CorsairKeyManager::getGogiKeySym(DFBEvent event)
{
	GOGI_KEYBOARD_EVENT gogi_evt;
	char       *key_value;
	DFBInputEventType   type;

	gogi_mode = GOGI_KEYMOD_NONE;
	type = (event.window.type == DWET_KEYDOWN) ? DIET_KEYPRESS : DIET_KEYRELEASE;

	dfb_to_gogi_key_symbol(type, event.window.key_symbol,
			event.window.key_code, event.window.key_id,
			event.window.modifiers, gogi_evt, key_value, 0);

	return gogi_evt.key;
}

