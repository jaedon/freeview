/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 */

#ifndef __CORSAIR_WEB_SURFACE_H
#define __CORSAIR_WEB_SURFACE_H

#include <directfb.h>
#include "corsair_surface.h"
#include "gogi_opera_api.h"

/**
   This is a surface for allowing an Opera window to be used for open
   Internet browsing. This is a simple integration that renders an Opera
   browser window with the Opera speed dial as start page.

   In Corsair, there is only one Opera window, hence this surface only
   support 1 window. In an extended version, it would be possible to
   add several Opera windows (e.g., in tabs) to this
   surface.

   Navigation bar and dialog handling are other features that can be
   added.
*/
class CorsairWebSurface : public CorsairSurface
{
	public:
		CorsairWebSurface(int x, int y, int w, int h, std::string url);
		CorsairWebSurface(int x, int y, int w, int h, std::string url, int appId);
		virtual ~CorsairWebSurface();

		virtual bool init(GOGI_FORMAT gogi_format, gogi_flush_callback_t flush_buffer);
		GogiOperaWindow* getWindow() { return m_window; }

		/** surface visability */
		virtual void showSurface();
		virtual void hideSurface();

		void setUrl(const char *url);
		void open(const char *url=NULL);
	
	private:
		GogiScreenHandle m_screen;
		GogiOperaWindow* m_window;
		std::string m_url;
};


#endif //__CORSAIR_WEB_SURFACE_H
