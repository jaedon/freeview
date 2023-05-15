/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Erik Lindström
 *
 * CorsairGfx could be said to represent the underlying graphical
 * components of an STB. It provides the Opera HbbTV library with a
 * surface to draw to.
 *
 */

#ifndef __CORSAIR_GFX_H_
#define __CORSAIR_GFX_H_


#include <vector>
#include <directfb.h>
#include "corsair_config.h"
#include "gogi_opera_api.h"
#include "corsair_utils.h"

#include "hapi.h"

class CorsairSurface;
class CorsairHbbTVSurface;
#ifdef ENABLE_OPERA_TVSTORE
class CorsairOTVSSurface;
#endif
class CorsairWebSurface;
class OipfSurface;
class Corsair;

using namespace std;
class CorsairGfx
{
	public:
		typedef std::vector<CorsairSurface*> SurfaceVector;

		static CorsairGfx* getInstance();
		static bool isShutdown;
		void init(int argc, char** argv, gogi_flush_callback_t flush_callback);
		void shutdown();
		void initGogiScreen(void);


		/** Create surface for running HbbTV applications */
#ifdef REMOVE_HBBTV_INPUT		
 		CorsairHbbTVSurface* createHbbTVSurface(int x, int y, int width, int height, int appId=eAWINDOW_OIPF);
#else
		CorsairHbbTVSurface* createHbbTVSurface(int x, int y, int width, int height, int appId=eAWINDOW_HBBTV);
#endif

#ifdef ENABLE_OPERA_TVSTORE
		CorsairOTVSSurface* createOTVSSurface(int x, int y, int width, int height, int appId=eAWINDOW_OTVS);
#endif
		/** Create surface for running OIPF applications */
		OipfSurface* createOipfSurface(int x, int y, int width, int height, int appId=eAWINDOW_OIPF);
		/** Cerate surface for open internet browsing */
		CorsairWebSurface* createWebSurface(int x, int y, int width, int height, std::string url = DEFAULT_APPLICATION_URI, int appId=0);

		void destroySurface(CorsairSurface* surface);

		void updateScreens(bool force = false);
		void drawIntersection(CorsairSurface* surface, DFBRectangle* rect, bool clear = true);
		void flip(DFBRegion* region);


		void setOpera(GogiOpera* opera) { m_opera = opera; }

		IDirectFB* getDfbInstance() { return m_dfb; }
		IDirectFBSurface* getPrimarySurface() { return m_primary_surface; }
		IDirectFBWindow* getWindow() { return m_window; }

		DFBSurfacePixelFormat getDfbFormat() { return m_dfb_format; }

		void drawRect(IDirectFBSurface* surface, DFBRectangle* rect,
		              int curr_x, int curr_y);

		int getOperaStride() { return m_opera_bpp * m_screen_width; }
		int getOperaBpp() { return m_opera_bpp; }
		GOGI_FORMAT getGogiFormat() { return m_gogi_format; }
		GogiOpera* getOpera() { return m_opera; }

		void clearRect(IDirectFBSurface* surface, DFBRectangle* rect);

    	void freeSurfaces();

		int getScreenWidth() const { return m_screen_width; }
		int getScreenHeight() const { return m_screen_height; }

	private:
		CorsairGfx();
		~CorsairGfx();

		void releaseSurface(IDirectFBSurface* surface);

		std::vector<CorsairSurface*> m_surfaces;

		IDirectFB *m_dfb;
		IDirectFBSurface *m_primary_surface;
		IDirectFBSurface *m_surface;
#ifdef CORSAIR_USE_DFB_WINDOW
		IDirectFBDisplayLayer *m_layer;
		IDirectFBWindow *m_window;
#endif // CORSAIR_USE_DFB_WINDOW

		DFBSurfacePixelFormat m_dfb_format;
		GOGI_FORMAT m_gogi_format;
		int m_opera_bpp;
		GogiOpera* m_opera;

		gogi_flush_callback_t m_flush_buffer;
		
		int m_screen_width;
		int m_screen_height;
};

#endif //__CORSAIR_GFX_H_
