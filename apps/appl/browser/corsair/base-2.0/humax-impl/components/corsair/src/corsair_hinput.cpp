/**************************************************************************************/
/**
 * @file corsair_hinput.cpp
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com)
 * @date    2012/03/17
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include <string>
#include <iostream>

#include <directfb_keyboard.h>
#include <directfb_keynames.h>

#include <hlib.h>

#include "oipf_api.h"
#include "input_api.h"
#include "humax_custom_key.h"

#include "corsair.h"
#include "corsair_hinput.h"
#include "corsair_gfx.h"
#include "corsair_utils.h"
#include "corsair_tools.h"
#include "corsair_input.h"
#include "corsair_web_surface.h"
#include "corsair_hbbtv_surface.h"
#include "corsair_hbbtv_manager.h"
#ifdef ENABLE_OPERA_TVSTORE
#include "corsair_otvs.h"
#endif
#include "oipf_surface.h"

using namespace std;

extern Corsair* g_corsair;

/*******************************************************************************
 *	class CorsairInput
 ******************************************************************************/
CorsairInput::CorsairInput(int appId)
{
//	DFBCHECK(m_window->CreateEventBuffer(m_window, &m_input));
	m_skip_keyup_event = false;
	m_dfb_window_id = appId;
}

CorsairInput::~CorsairInput()
{
//	DFBCHECK(m_input->Release(m_input));
}

void CorsairInput::readInput(int delay)
{
    HInputEvent_t		hEvent;

	while (HAPI_GetInputEvent((HWINID_e)m_dfb_window_id, &hEvent, 0) == TRUE)
	{
		switch (hEvent.type)
		{
			case DIET_KEYPRESS:
		    case DIET_KEYRELEASE:
				return inputProcess(hEvent);
		}
	}
}

void CorsairInput::consume(HInputEvent_t evt)
{
    if ( evt.type == DIET_KEYPRESS )
    {
        m_forced_consume = true;
        m_last_presskey = evt;
    }
    else if ( evt.type == DIET_KEYRELEASE )
    {
        m_forced_consume = false;
    }
    
	HAPI_ReturnKeyEvent((HWINID_e)m_dfb_window_id, eHINPUTRET_CONSUMED, &evt);
}

void CorsairInput::pass(HInputEvent_t evt)
{
	HAPI_ReturnKeyEvent((HWINID_e)m_dfb_window_id, eHINPUTRET_PASS, &evt);
}

/*******************************************************************************
 *	class CorsairWebInput
 ******************************************************************************/
CorsairWebInput::CorsairWebInput(int appId)
	: CorsairInput(appId)
{
}

CorsairWebInput::~CorsairWebInput()
{
}

void CorsairWebInput::inputProcess(HInputEvent_t evt)
{
	GOGI_KEYBOARD_EVENT gogi_evt;
	char       key_value[16] = {0,};
	DFBInputEventType   type;

	type = (DFBInputEventType)evt.type;
	dfb_to_gogi_key_symbol(type, evt.key_symbol,
			evt.key_code, evt.key_id,
			evt.modifiers, gogi_evt, key_value, evt.flags);

	g_corsair->showOpenWebSurface();

	GogiOperaWindow* focused_window = g_corsair->getFocusedWindow();
	if ( !focused_window ) return pass(evt);

 	if ( gogi_keyboard_evt(&gogi_evt) != GOGI_STATUS_OK )
	{
		return pass(evt);
	}
    
	return consume(evt);
}

/*******************************************************************************
 *	class CorsairHbbTVInput
 ******************************************************************************/
CorsairHbbTVInput::CorsairHbbTVInput(int appId)
	: CorsairInput(appId)
{
}

CorsairHbbTVInput::~CorsairHbbTVInput()
{
}

void CorsairHbbTVInput::inputProcess(HInputEvent_t evt)
{
	char       key_value[16] = {0,};
	DFBInputEventType   type;
	GOGI_KEYBOARD_EVENT gogi_evt;
    GogiOperaWindow* hbbtv_window = NULL;
    
	type = (DFBInputEventType)evt.type;
	dfb_to_gogi_key_symbol( type, evt.key_symbol,
							evt.key_code, evt.key_id,
							evt.modifiers, gogi_evt, key_value, evt.flags);
	gogi_evt.key_repeat = 0;
	gogi_evt.key_value = key_value;
	
	LOG_DEBUG("CorsairHbbTVInput:: got the %s : %d \n",(gogi_evt.key_type == GOGI_KEYDOWN ? "KEYDOWN" : "KEYUP" ),gogi_evt.key);
		
	if ( gogi_evt.key_type == GOGI_KEYDOWN )
	{
	    hbbtv_window = g_corsair->getHbbTVSurface()->getWindow();
	    
	    if ( !hbbtv_window ) 
	    {
	        LOG_DEBUG("CorsairHbbTVInput:: There is no HbbTV window\n");
	        return pass(evt);
	    }
	    CorsairHbbTVManager *hbbtv_manager = g_corsair->getHbbTVManager();
	    if ( hbbtv_manager == NULL ) return pass(evt);
	    
		if ( gogi_evt.key == GOGI_KEY_ESCAPE )
		{
		    LOG_DEBUG("CorsairHbbTVInput:: EXIT handled by corsair, HbbTV exit keyscinario\n");
			if ( hbbtv_manager->isRunningApplication() )
			{
				hbbtv_manager->action(new CorsairHbbTVDoReactivateAction(hbbtv_manager));
				hbbtv_manager->action(new CorsairHbbTVExitAction(hbbtv_manager));
			}
			return consume(evt);
		}

		/* opera_hbbtv_check_key_input well check the gogi_evt is available for the hbbtv window without performing. */
		/* opera recommand that we should not change the focused window during keydown and keyup cycle.  */
		if ( opera_hbbtv_check_key_input( hbbtv_window, &gogi_evt ) == false )
		{
		    if ( hbbtv_manager->getHbbTVApplicationFromHumax() && \ 
                    true == isAllowedKeyByUX( gogi_evt.key ) )
		    {
		        LOG_DEBUG("CorsairHbbTVInput:: Ch+/- Key consume becasue OIPF don't want to this key during running BI\n");
		        return consume(evt);
		    }
		    else
		    {
		        LOG_DEBUG("CorsairHbbTVInput:: pass key to OIPF\n");
		        return pass(evt);
		    }
		}
		
		if ( g_corsair->getFocusedWindow() != NULL && g_corsair->getFocusedWindow() != hbbtv_window )
		{
			LOG_DEBUG("CorsairHbbTVInput:: This is OIPF window call showSurface\n");
			g_corsair->showHbbTVSurface();
		}
		
		if ( opera_hbbtv_key_input( hbbtv_window, &gogi_evt) == true )
        {
    		LOG_DEBUG("CorsairHbbTVInput:: this key has been consumed.\n");
	        return consume(evt);
        }
	}
	else if ( gogi_evt.key_type == GOGI_KEYUP )
	{
	    LOG_DEBUG("CorsairHbbTVInput:: %s \n",(m_forced_consume ? "make pair key event for hama" : "ignored"));
	    if ( m_forced_consume )
	    {
	        hbbtv_window = g_corsair->getHbbTVSurface()->getWindow();
	        if ( hbbtv_window )
	        {
	            LOG_DEBUG("CorsairHbbTVInput:: passed to 0x%x \n",hbbtv_window);
	            opera_hbbtv_key_input( hbbtv_window, &gogi_evt);
	        }
	        else
	        {
	            LOG_DEBUG("CorsairHbbTVInput:: not passed to HbbTV Window, but call consume for Hama\n");
	        }
	        return consume(evt);
	    }
	}

	LOG_DEBUG("CorsairHbbTVInput:: this key will be passed to OIPFInput.\n");

	return pass(evt);
}

bool CorsairHbbTVInput::isAllowedKeyByUX( GOGI_VIRTUAL_KEY key )
{
    bool retval = false;
    switch( key )
    {
        case GOGI_KEY_MENU: // home key
        case GOGI_KEY_SEARCH:
        case GOGI_KEY_GUIDE:
        case GOGI_KEY_HOME: // portal
        case GOGI_KEY_VOLUME_UP:
        case GOGI_KEY_VOLUME_DOWN:
        case GOGI_KEY_POWER:
        case GOGI_KEY_MUTE:
        case GOGI_KEY_ESCAPE:
        case GOGI_KEY_NETFLIX:
            break;
            
        default:
            retval = true;
            break;
    }
    return retval;
}
	
/*******************************************************************************
 *	class CorsairOipfInput
 ******************************************************************************/
CorsairOipfInput::CorsairOipfInput(int appId)
	: CorsairInput(appId)
{
}

CorsairOipfInput::~CorsairOipfInput()
{
}

void CorsairOipfInput::inputProcess(HInputEvent_t evt)
{
	char       key_value[16] = {0,};
	DFBInputEventType   type;
	GOGI_KEYBOARD_EVENT gogi_evt;

	type = (DFBInputEventType)evt.type;
	memset( key_value, 0x00, sizeof ( key_value ) );
	dfb_to_gogi_key_symbol(type, evt.key_symbol,
			evt.key_code, evt.key_id,
			evt.modifiers, gogi_evt, key_value, evt.flags);
	gogi_evt.key_repeat = 0;
	gogi_evt.key_value = key_value;

	if ( g_corsair == NULL )
	{
		LOG_DEBUG("CorsairOipfInput:: this is not the Corsair Handle\n");
		return pass(evt);
	}

	LOG_DEBUG("CorsairOipfInput:: got the %s : %d \n",(gogi_evt.key_type == GOGI_KEYDOWN ? "keydown" : "keyup" ),gogi_evt.key);

	GogiOperaWindow* focused_window = g_corsair->getFocusedWindow();
	if ( !focused_window )
	{
		g_corsair->showOipfSurface();
		if(!g_corsair->getFocusedWindow())
		{
			LOG_DEBUG("CorsairOipfInput:: There is not the focused window\n");
			return pass(evt);
		}
	}

	// condition : 	If this fucntion is called when the focused window isn't OIPF window,
	//			explicitly call showOipfSurface to set focus on HbbTV window before send key input.
	//			Because this key input is already passed from HbbTV window
	if ( gogi_evt.key_type == GOGI_KEYDOWN ) // this routine should be called only once when pressing the key.
	{
		if( !opera_oipf_is_oipf_window(opera_hbbtv_get_window_identifier(focused_window)))
		{
			LOG_DEBUG("CorsairOipfInput:: This is HbbTV window call showSurface\n");
			g_corsair->showOipfSurface();
		}
	}

	if ( gogi_keyboard_evt(&gogi_evt) != GOGI_STATUS_OK )
	{
		LOG_DEBUG("CorsairOipfInput:: this key will be passed.\n");
		return pass(evt);
	}

	LOG_DEBUG("CorsairOipfInput:: this key has been consumed.\n");

	return consume(evt);
}

/*******************************************************************************
 *	class CorsairOTVSInput
 ******************************************************************************/
#ifdef ENABLE_OPERA_TVSTORE
CorsairOTVSInput::CorsairOTVSInput(int appId)
	: CorsairInput(appId)
{
}

CorsairOTVSInput::~CorsairOTVSInput()
{
}

void CorsairOTVSInput::inputProcess(HInputEvent_t evt)
{
	char       key_value[16] = {0,};
	DFBInputEventType   type;
	GOGI_KEYBOARD_EVENT gogi_evt;

	type = (DFBInputEventType)evt.type;
	memset( key_value, 0x00, sizeof ( key_value ) );
	dfb_to_gogi_key_symbol(type, evt.key_symbol,
			evt.key_code, evt.key_id,
			evt.modifiers, gogi_evt, key_value, evt.flags);
	gogi_evt.key_repeat = 0;
	gogi_evt.key_value = key_value;

	if ( g_corsair == NULL || !g_corsair->getOTVSHandle() )
	{
		LOG_DEBUG("CorsairOTVSInput:: this is not the Corsair Handle\n");
		return pass(evt);
	}

	if ( m_skip_keyup_event == true && gogi_evt.key_type == GOGI_KEYUP )
	{
		LOG_DEBUG("CorsairOTVSInput:: this key up is consumed to make one keyevent set for one window\n");
		m_skip_keyup_event = false;
		return consume(evt);
	}

	if ( g_corsair->getOTVSHandle()->getFocusedWindow() == NULL )
	{
		LOG_DEBUG("CorsairOTVSInput:: this key will be passed.(none window)\n");
		return pass(evt);
	}

	/* this is for the case that some notification of Prism Web app get a focus althoug OTVS pages are displayed */
	if( g_corsair->getOTVSHandle()->isOTVSWindow(g_corsair->getFocusedWindow()) == false && gogi_evt.key_type== GOGI_KEYDOWN )
	{
		g_corsair->showOTVSSurface();
	}


	/* key filter */
	if ( gogi_evt.key == GOGI_KEY_VOLUME_UP || gogi_evt.key == GOGI_KEY_VOLUME_DOWN || gogi_evt.key == GOGI_KEY_POWER || gogi_evt.key == GOGI_KEY_MUTE )
	{
		return pass(evt);
	}

	if ( gogi_evt.key == GOGI_KEY_MENU )
	{
		return pass(evt);
	}

	if( gogi_evt.key == GOGI_KEY_ESCAPE )
	{
		/* in the 331 line, we checked already g_corsair->getOTVSHandle()->getFocusedWindow(), so consume keyup and keydown for exit key
		   keydown is only consumed, and some data handling will be done with keyup event. */
		if( gogi_evt.key_type== GOGI_KEYUP )
		{
			if ( g_corsair->getOTVSHandle()->getOTVSApplicationWindow() == g_corsair->getOTVSHandle()->getFocusedWindow() )
			{
				g_corsair->getOTVSHandle()->stopOTVSApplication();
	   		}
	   		if ( g_corsair->getOTVSHandle()->getErrorPageWindow() == g_corsair->getOTVSHandle()->getFocusedWindow() )
			{
				g_corsair->getOTVSHandle()->stopErrorPage();
			}
			if ( g_corsair->getOTVSHandle()->getOTVSPageWindow() == g_corsair->getOTVSHandle()->getFocusedWindow() )
			{
				g_corsair->getOTVSHandle()->stopOTVSPage();
			}

			if ( !g_corsair->getOTVSHandle()->getFocusedWindow() )
			{
				g_corsair->showOipfSurface();
				g_corsair->showGogiWindow( g_corsair->getFocusedWindow());
			}
			else
			{
				g_corsair->getOTVSHandle()->showOTVS();
			}
		}
		return consume(evt);
	}

	if ( g_corsair->getOTVSHandle()->getErrorPageWindow() != NULL )
	{
		if( gogi_evt.key_type== GOGI_KEYDOWN && ( gogi_evt.key == GOGI_KEY_ENTER || gogi_evt.key == GOGI_KEY_BACK ))
		{
			g_corsair->getOTVSHandle()->stopErrorPage();
			m_skip_keyup_event = true;
			return consume(evt);
		}
	}

	replaceKey(&gogi_evt);

	if ( gogi_keyboard_evt(&gogi_evt) != GOGI_STATUS_OK )
	{
		LOG_DEBUG("CorsairOTVSInput:: this key will be passed.\n");
		return pass(evt);
	}

	LOG_DEBUG("CorsairOTVSInput:: this key has been consumed.\n");

	return consume(evt);
}

void CorsairOTVSInput::replaceKey(GOGI_KEYBOARD_EVENT *key_event)
{
	switch(key_event->key)
	{
		case GOGI_KEY_BACK:
		case GOGI_KEY_ESCAPE:
			key_event->key = GOGI_KEY_BACKSPACE;
			break;
	}
}

#endif
