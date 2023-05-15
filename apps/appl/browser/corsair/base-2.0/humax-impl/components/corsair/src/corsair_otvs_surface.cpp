/** **********************************************************************************************************
	@file 		corsair_otvs_surface.cpp
	
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

#include "gogi_opera_api.h"

#include "corsair_otvs_surface.h"
#include "corsair.h"
#include "corsair_gfx.h"
#include "corsair_config.h"

using namespace std;

/**
 * CorsairOTVSSurface
 *
 * Contains a GOGI window and a DirectFB surface and can be used for
 * HbbTV content.
 */

CorsairOTVSSurface::CorsairOTVSSurface(int x, int y, int w, int h)
	: CorsairSurface(x, y, w, h),
	  m_gogi_otvsapplication_window(NULL),
	  m_gogi_otvspage_window(NULL),
	  m_gogi_errorpage_window(NULL),
	  m_gogi_otvstemp_window(NULL),
	  m_last_focused_window(NULL)
{
}

CorsairOTVSSurface::CorsairOTVSSurface(int x, int y, int w, int h, int appId)
	: CorsairSurface(x, y, w, h, appId),
	  m_gogi_otvsapplication_window(NULL),
	  m_gogi_otvspage_window(NULL),
	  m_gogi_errorpage_window(NULL),
	  m_gogi_otvstemp_window(NULL),
	  m_last_focused_window(NULL)
{
}


CorsairOTVSSurface::~CorsairOTVSSurface()
{
	if ( m_gogi_otvsapplication_window )
		releaseWindow( m_gogi_otvsapplication_window );
	if ( m_gogi_errorpage_window )
		releaseWindow( m_gogi_errorpage_window );
	if ( m_gogi_otvspage_window )
		releaseWindow( m_gogi_otvspage_window );
}

GogiOperaWindow* CorsairOTVSSurface::createGogiWindow()
{
	GogiOperaWindow* window = NULL;

	CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
	int status = op_create_window(corsair_gfx->getOpera(), &window, NULL, m_screen);
	if (GOGI_STATUS_OK != status)
	{
		LOG_ERR("Failed to create opera window.\n");
		return NULL;
	}

	GogiRect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = m_width;
	rect.h = m_height;
	op_set_window_rect(window, &rect);

	op_hide_window(window);

	op_set_window_transparent(window, true);

	/* the default color is black for background(refer the document "implementation guide" of Opera) */
	GogiColor col;
	col.alpha = 255;
	col.red = 0;
	col.green = 0;
	col.blue = 0;
	op_set_default_background_color(window, &col);
	op_hide_scrollbars(window);
	return window;
}


void CorsairOTVSSurface::destroyGogiWindow(GogiOperaWindow* window)
{
	if (window)
	{
		LOG_DEBUG("==== destroyed window 0x%x \n",window);
		CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
		op_close_window(corsair_gfx->getOpera(), window);
	}
}

void CorsairOTVSSurface::showSurface() 
{
	GogiOperaWindow* focused_window = getFocusedWindow();
	
	if ( focused_window )
	{
		if ( focused_window == getOTVSPageWindow() )
		{
			if ( getOTVSApplicationWindow() != NULL )
				op_hide_window(getOTVSApplicationWindow());
			if ( getOTVSErrorPageWindow() != NULL )
				op_hide_window(getOTVSErrorPageWindow());
		}
		else if ( focused_window == getOTVSApplicationWindow() )
		{
			if ( getOTVSPageWindow() != NULL )
				op_hide_window(getOTVSPageWindow());
			if ( getOTVSErrorPageWindow() != NULL )
				op_hide_window(getOTVSErrorPageWindow());
		}
		else if ( focused_window == getOTVSErrorPageWindow() )
		{
			if ( getOTVSPageWindow() != NULL )
				op_hide_window(getOTVSPageWindow());
			if ( getOTVSApplicationWindow() != NULL )
				op_hide_window(getOTVSApplicationWindow());
		}
		setVisibility(true);
		gogi_set_default_main_screen(m_screen);
		op_show_window(focused_window);
		op_set_focus(focused_window);
	}
}

void CorsairOTVSSurface::hideSurface()
{
	setVisibility(false);
	if ( m_gogi_otvsapplication_window )
		op_hide_window(m_gogi_otvsapplication_window);
	if ( m_gogi_otvspage_window )
		op_hide_window(m_gogi_otvspage_window);
	if ( m_gogi_errorpage_window )
		op_hide_window(m_gogi_errorpage_window);
}


GogiOperaWindow* CorsairOTVSSurface::requestOTVSApplicationWindow()
{
	m_gogi_otvsapplication_window = requestWindow(m_gogi_otvsapplication_window);
	return m_gogi_otvsapplication_window;
		}
		
GogiOperaWindow* CorsairOTVSSurface::requestOTVSErrorPageWindow()
{
	m_gogi_errorpage_window = requestWindow(m_gogi_errorpage_window);
	return m_gogi_errorpage_window;
}

GogiOperaWindow* CorsairOTVSSurface::requestOTVSPageWindow()
{
	m_gogi_otvspage_window = requestWindow(m_gogi_otvspage_window);
	return m_gogi_otvspage_window;
}
	
GogiOperaWindow* CorsairOTVSSurface::requestWindow( GogiOperaWindow* &window)
{
	if(window)
	{
		LOG_DEBUG(" There is already an window.\n");
		return window;
	}

	window = createGogiWindow();

	op_set_focus(window);

	return window;
}

bool CorsairOTVSSurface::releaseWindow( GogiOperaWindow* &window )
{
	if(!window)
	{
		LOG_ERR(" There is not an window.\n");
		return m_gogi_otvsapplication_window;
	}
	
	op_lose_focus(window);
	destroyGogiWindow(window);
	window = NULL;
	return true;
}

bool CorsairOTVSSurface::releaseOTVSApplicationWindow() { 
	releaseWindow( m_gogi_otvsapplication_window );
		setFocusedWindow(NULL);
	return 0;
	}

bool CorsairOTVSSurface::releaseOTVSErrorPageWindow() { 
	releaseWindow( m_gogi_errorpage_window );
	setFocusedWindow(NULL);
	return 0;
}

bool CorsairOTVSSurface::releaseOTVSPageWindow(){ 
	releaseWindow( m_gogi_otvspage_window );
		setFocusedWindow(NULL);
	return 0;
}

int CorsairOTVSSurface::setFocusedWindow( GogiOperaWindow* focused_window)
{
	LOG_DEBUG("====setFocusedWindow 0x%x \n",focused_window);
	if ( focused_window )
	{
		m_last_focused_window = focused_window;
	}
	else
	{
		m_last_focused_window = NULL;
		
		if ( getOTVSPageWindow() )
		{
			LOG_DEBUG("====setFocusedWindow getOTVSPageWindow 0x%x \n",getOTVSPageWindow());
			m_last_focused_window = getOTVSPageWindow();
		}
		if ( getOTVSApplicationWindow() )
		{
			LOG_DEBUG("====setFocusedWindow getOTVSApplicationWindow 0x%x \n",getOTVSApplicationWindow());
			m_last_focused_window = getOTVSApplicationWindow();
		}
		if ( getOTVSErrorPageWindow() )
		{
			LOG_DEBUG("====setFocusedWindow getOTVSErrorPageWindow 0x%x \n",getOTVSErrorPageWindow());
			m_last_focused_window = getOTVSErrorPageWindow();
		}
	}

	if ( m_last_focused_window )
		showSurface();
		
	return 0;
}

GogiOperaWindow* CorsairOTVSSurface::getFocusedWindow()
{
	LOG_DEBUG("====getFocusedWindow 0x%x \n",m_last_focused_window);
	return m_last_focused_window;
}