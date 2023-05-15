/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Erik Lindström
 *
 */

#ifndef __CORSAIR_SURFACE_H_
#define __CORSAIR_SURFACE_H_

#include <sys/time.h>
#include <directfb.h>
#include "gogi_opera_api.h"

#ifdef USE_DIRECTFBGL		
class CorsairOpenglFbo;
#endif

class CorsairSurface
{
	public:
		CorsairSurface(int x, int y, int w, int h);
		CorsairSurface(int x, int y, int w, int h, int appId);
		virtual ~CorsairSurface();
		virtual void* getDataBuffer();
		virtual void drawRect(DFBRectangle* rect);

		virtual void showSurface() = 0;
		virtual void hideSurface() = 0;

		IDirectFBSurface* getDfbSurface() { return m_dfb_surface; }
		IDirectFBWindow* getDfbWindow() { return m_dfb_window; }

		int getCurrentXPos() { return m_x_pos; }
		int getCurrentYPos() { return m_y_pos; }

		int getWidth() { return m_width; }
		int getHeight() { return m_height; }

		int getAppId() { return m_app_id; }

		bool isVisible() { return m_visible; }
		bool isOnScreen() { return true;}

		bool intersects(DFBRectangle* rect);

		void setVisibility(bool visibility) { m_visible = visibility; }
		void updateScreen(bool force = false);
		void setScreenInvalid() { m_screen_invalid = true; }
		void flip(DFBRegion *region);
		virtual bool init(GOGI_FORMAT gogi_format, gogi_flush_callback_t flush_buffer);

	protected:
		virtual void updateScreenInternal();
		void clearSurface();
		void initDfbWindow(int x, int y, int w, int h);
		
	protected:
		IDirectFBSurface *m_dfb_surface;
		IDirectFBWindow *m_dfb_window;

        GogiScreenHandle m_screen;
        
		int m_x_pos;
		int m_y_pos;
		int m_width;
		int m_height;

		int m_app_id;

		bool m_visible;
		bool m_screen_invalid;
		timeval m_next_screen_update;
#ifdef USE_DIRECTFBGL
		CorsairOpenglFbo	*m_egl_object;
#endif
};


#endif //__CORSAIR_SURFACE_H_
