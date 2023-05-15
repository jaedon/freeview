/** **********************************************************************************************************
	@file 		corsair_web_surface.cpp
	
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

#include <iostream>

#include "gogi_opera_api.h"

#include "corsair_web_surface.h"
#include "corsair.h"
#include "corsair_gfx.h"
#include "corsair_config.h"

using namespace std;


/**
 * CorsairWebSurface
 *
 * Contains a GOGI window and a DirectFB surface and can be used for
 * Internet browsing.
 */

CorsairWebSurface::CorsairWebSurface(int x, int y, int w, int h, string url)
	: CorsairSurface(x, y, w, h),
	  m_window(NULL),
	  m_url(url)
{
}

CorsairWebSurface::CorsairWebSurface(int x, int y, int w, int h, string url, int appId)
	: CorsairSurface(x, y, w, h, appId),
	  m_window(NULL),
	  m_url(url)
{
}


CorsairWebSurface::~CorsairWebSurface()
{
	CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
	if (m_window && corsair_gfx)
		op_close_window(corsair_gfx->getOpera(), m_window);
}

bool CorsairWebSurface::init(GOGI_FORMAT gogi_format, gogi_flush_callback_t flush_buffer)
{
	if ( CorsairSurface::init( gogi_format, flush_buffer ) == true )
	// Create the main opera window and open the speed dial
	{
		CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
		if (op_create_window(corsair_gfx->getOpera(), &m_window, NULL, m_screen) != GOGI_STATUS_OK)
		{
			LOG_ERR("Failed to create an Opera window.\n");
			return false;
		}

		GogiRect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = m_width;
		rect.h = m_height;

		if (op_set_window_rect(m_window, &rect) != GOGI_STATUS_OK)
			LOG_ERR("Failed to set the size of the Opera window.\n");

		GogiColor col;
		col.alpha = 0;
		col.red = 0;
		col.green = 0;
		col.blue = 0;
		op_set_default_background_color(m_window, &col);

		op_enable_highlight_rectangle(m_window);
		op_set_window_transparent(m_window, true);
#if !defined(CONFIG_DEBUG)
		op_hide_scrollbars(m_window);
#endif

#if 0 /** STB-Component : Opera default setting to show speed dial page */
		open();

		op_open_url(m_window, "file:/usr/browser/webapps/default.html");
		// Open Opera speeddial as start page
		if (m_url.empty())
		{
			if (op_open_url(m_window, "opera:speeddial") != GOGI_STATUS_OK)
				LOG_ERR("Failed to open Open Web start page.\n");
			int idx = 0;
			op_speed_dial_set_url(idx++, "http://www.opera.com");
			op_speed_dial_set_url(idx++, "http://www.google.com");
		}
		else
			op_open_url(m_window, m_url.c_str());
#endif
	}

	return true;
}


void CorsairWebSurface::showSurface()
{
	setVisibility(true);
	gogi_set_default_main_screen(m_screen);

	op_set_focus(m_window);
	op_show_window(m_window);
	setScreenInvalid();
}

void CorsairWebSurface::hideSurface()
{
	setVisibility(false);
	op_hide_window(m_window);
}

void CorsairWebSurface::setUrl(const char *url)
{
	if ( url ) m_url = url;
}

void CorsairWebSurface::open(const char *url)
{
	op_set_focus(m_window);
	( url ) ? op_open_url(m_window, url) : op_open_url(m_window, m_url.c_str());
}
