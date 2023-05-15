/** **********************************************************************************************************
	@file 		corsair_hbbtv_surface.cpp
	
	@date		2012/09/25
	@author		Browser Part (psw_browser@humaxdigital.com)
	@breif		
	
	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. ("Humax") and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR 
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR 
	INABILITY TO USE THE SOFTWARE.
	
	This License is effective until terminated. You may terminate this License at any time 
	by destroying all copies of the Software including all documentation. This License will terminate 
	immediately without notice from Humax to you if you fail to comply with any provision of this License. 
	Upon termination, you must destroy all copies of the Software and all documentation.  
	
	The laws of the Republic of Korea will apply to any disputes arising out of or relating to 
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#include <cassert>
#include <iostream>

#include "gogi_opera_api.h"

#include "corsair_hbbtv_surface.h"
#include "corsair.h"
#include "corsair_gfx.h"
#include "corsair_config.h"

using namespace std;

CorsairHbbTVSurface::HbbTVWindowData::HbbTVWindowData(GogiOperaWindow* window, bool visible, bool owned_by_corsair)
	: gogi_window(window),
	  is_visible(visible),
	  is_owned_by_corsair(owned_by_corsair)
{ }


/**
 * CorsairHbbTVSurface
 *
 * Contains a GOGI window and a DirectFB surface and can be used for
 * HbbTV content.
 */

CorsairHbbTVSurface::CorsairHbbTVSurface(int x, int y, int w, int h)
	: CorsairSurface(x, y, w, h)
{
}

CorsairHbbTVSurface::CorsairHbbTVSurface(int x, int y, int w, int h, int appId)
	: CorsairSurface(x, y, w, h, appId)
{
}


CorsairHbbTVSurface::~CorsairHbbTVSurface()
{
	for (HbbTVWindowDataVector::iterator it = m_windows.begin(); it != m_windows.end(); ++it)
        if(it->is_owned_by_corsair)
			destroyGogiWindow((*it).gogi_window);

	m_windows.clear();
}

GogiOperaWindow* CorsairHbbTVSurface::createGogiWindow()
{
	GogiOperaWindow* window = NULL;

	CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
	int status = op_create_window(corsair_gfx->getOpera(), &window, NULL, m_screen);
	if (GOGI_STATUS_OK != status)
	{
		LOG_ERR("Failed to create opera window.[%d]\n",status);
		return NULL;
	}

	GogiRect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = m_width;
	rect.h = m_height;
	op_set_window_rect(window, &rect);

	op_hide_window(window);

	op_disable_highlight_rectangle(window);
	op_set_window_transparent(window, true);

	GogiColor col;
	col.alpha = 0;
	col.red = 255;
	col.green = 255;
	col.blue = 255;
	op_set_default_background_color(window, &col);
#if !defined(CONFIG_DEBUG)
	op_hide_scrollbars(window);
#endif

	return window;
}


void CorsairHbbTVSurface::destroyGogiWindow(GogiOperaWindow* window)
{
	assert(opera_hbbtv_get_window_identifier(window));

	if (window)
	{
		CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
		op_close_window(corsair_gfx->getOpera(), window);
	}
}


GogiOperaWindow* CorsairHbbTVSurface::getWindow()
{
	HbbTVWindowData* focused_window_data = getFocusedWindowData();
	return focused_window_data ? focused_window_data->gogi_window : NULL;
}


void CorsairHbbTVSurface::showSurface()
{
	setVisibility(true);
	gogi_set_default_main_screen(m_screen);

	for (HbbTVWindowDataVector::iterator it = m_windows.begin(); it != m_windows.end(); ++it)
	{
		if ((*it).is_visible)
		{
			assert(opera_hbbtv_get_window_identifier((*it).gogi_window));
			op_show_window((*it).gogi_window);
		}
	}

	HbbTVWindowData* focused_window_data = getFocusedWindowData();
	if (focused_window_data)
	{
		assert(opera_hbbtv_get_window_identifier(focused_window_data->gogi_window));
		op_set_focus(focused_window_data->gogi_window);
	}
}

void CorsairHbbTVSurface::hideSurface()
{
	setVisibility(false);

	for (HbbTVWindowDataVector::iterator it = m_windows.begin(); it != m_windows.end(); ++it)
	{
		assert(opera_hbbtv_get_window_identifier((*it).gogi_window));
		op_hide_window((*it).gogi_window);
	}
}

GogiOperaWindow* CorsairHbbTVSurface::requestWindow()
{
	GogiOperaWindow* gogi_window = createGogiWindow();
	if (gogi_window)
	{
		assert(opera_hbbtv_get_window_identifier(gogi_window));

		HbbTVWindowData window_data(gogi_window);
		m_windows.push_back(window_data);

		// focus always on latest requested window
		if (isVisible())
			op_set_focus(gogi_window);
	}
	return gogi_window;
}


bool CorsairHbbTVSurface::releaseWindow(GogiOperaWindow* gogi_window)
{
	assert(opera_hbbtv_get_window_identifier(gogi_window));

	HbbTVWindowDataVector::iterator it;
	for (it = m_windows.begin(); it != m_windows.end(); ++it)
	{
		if ((*it).gogi_window == gogi_window)
			break;
	}

	if (it == m_windows.end())
	{
		assert(!"trying to release window from wrong surface");
		return false;
	}

	bool is_focused_window = isFocusedWindow(gogi_window);

	m_windows.erase(it);
	destroyGogiWindow(gogi_window);

	if (is_focused_window && isVisible())
	{
		// reset focus on 'new' focused window if it exists
		HbbTVWindowData* focused_window_data = getFocusedWindowData();
		if (focused_window_data)
		{
			op_raise_window(focused_window_data->gogi_window);
		}
	}
	return true;
}


void CorsairHbbTVSurface::showWindow(GogiOperaWindow* gogi_window)
{
	assert(opera_hbbtv_get_window_identifier(gogi_window));

	HbbTVWindowData* window_data = getWindowData(gogi_window);
	if (window_data)
	{
		window_data->is_visible = true;

		if (isVisible())
		{
			op_show_window(gogi_window);
		}
	}
	else
		LOG_DEBUG("CorsairHbbTVSurface::showWindow called with unrecognized window.\n");
}


void CorsairHbbTVSurface::hideWindow(GogiOperaWindow* gogi_window)
{
	assert(opera_hbbtv_get_window_identifier(gogi_window));

	HbbTVWindowData* window_data = getWindowData(gogi_window);
	if (window_data)
	{
		window_data->is_visible = false;

		if (isVisible())
		{
			op_hide_window(gogi_window);
			if (isFocusedWindow(gogi_window))
				// reset focus on running app
				op_raise_window(gogi_window);
		}
	}
	else
		LOG_DEBUG("CorsairHbbTVSurface::hideWindow called with unrecognized window.\n");
}


CorsairHbbTVSurface::HbbTVWindowData* CorsairHbbTVSurface::getWindowData(GogiOperaWindow* gogi_window)
{
	for (HbbTVWindowDataVector::iterator it = m_windows.begin(); it != m_windows.end(); ++it)
	{
		if ((*it).gogi_window == gogi_window)
			return &(*it);
	}
	return NULL;
}


bool CorsairHbbTVSurface::isFocusedWindow(GogiOperaWindow* gogi_window)
{
	HbbTVWindowData* focused_window_data = getFocusedWindowData();
	return focused_window_data && gogi_window == focused_window_data->gogi_window;
}


CorsairHbbTVSurface::HbbTVWindowData* CorsairHbbTVSurface::getFocusedWindowData()
{
	return m_windows.empty() ? NULL : &m_windows.back();
}

bool CorsairHbbTVSurface::findWindow( GogiOperaWindow* gogi_window )
{
	HbbTVWindowDataVector::iterator it;
	for (it = m_windows.begin(); it != m_windows.end(); ++it)
	{
		if ( gogi_window ==  (*it).gogi_window )
		{
			return true;
		}
	}	
	return false;
}

void CorsairHbbTVSurface::showWindowList(void)
{
	CorsairHbbTVSurface::HbbTVWindowData *data = NULL;
	CorsairHbbTVSurface::HbbTVWindowData *focus = getFocusedWindowData();
	unsigned int index = 0;

	for (HbbTVWindowDataVector::iterator it = m_windows.begin(); it != m_windows.end(); ++it)
	{
		data = &(*it);
		LOG_DEBUG("index		: %d/%d \n",++index,m_windows.size());
		LOG_DEBUG("window     : 0x%x \n",data->gogi_window);
		LOG_DEBUG("visible    : %d \n",data->is_visible);
		LOG_DEBUG("by corsair : %d \n",data->is_owned_by_corsair);
		LOG_DEBUG("focus      : %d \n",(data == focus));
		LOG_DEBUG("url        : %s \n",data->url.c_str());
	}
}

bool CorsairHbbTVSurface::setWindowUrl(GogiOperaWindow* gogi_window, std::string url){
	HbbTVWindowDataVector::iterator it;
	CorsairHbbTVSurface::HbbTVWindowData *data = NULL;

	for (it = m_windows.begin(); it != m_windows.end(); ++it)
	{
		if ( gogi_window ==  (*it).gogi_window )
		{
			data = &(*it);
			data->url = url;
			return true;
		}
	}	

	return false;
}

