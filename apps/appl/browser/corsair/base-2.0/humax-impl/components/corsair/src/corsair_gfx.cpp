/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Erik Lindström
 */

#include <iostream>
#include "corsair_gfx.h"
#include "corsair_utils.h"
#include "corsair_surface.h"
#include "corsair_hbbtv_surface.h"
#ifdef ENABLE_OPERA_TVSTORE
#include "corsair_otvs_surface.h"
#endif
#include "corsair_web_surface.h"
#include "oipf_surface.h"

#ifdef USE_DIRECTFBGL
#include "corsair_gfx_egl.h"
#endif

using namespace std;


// CorsairGfx class
bool CorsairGfx::isShutdown = false;

CorsairGfx::CorsairGfx()
	: m_dfb(NULL),
	  m_primary_surface(NULL),
	  m_surface(NULL),
#ifdef CORSAIR_USE_DFB_WINDOW
	  m_layer(NULL),
	  m_window(NULL),
#endif // CORSAIR_USE_DFB_WINDOW
	  m_dfb_format(DSPF_ARGB),
	  m_gogi_format(GOGI_FORMAT_VEGA_BGRA32),
	  m_screen_width(SCREEN_WIDTH),
	  m_screen_height(SCREEN_HEIGHT)
{
	m_opera_bpp = CorsairUtils::get_bpp(m_gogi_format);
}


void CorsairGfx::freeSurfaces()
{
	SurfaceVector::iterator it;
	for (it = m_surfaces.begin(); it != m_surfaces.end(); ++it)
		delete *it;
	m_surfaces.clear();
}


CorsairGfx::~CorsairGfx()
{
	freeSurfaces();

#ifdef CORSAIR_USE_DFB_WINDOW
	if (m_primary_surface)
		DFBCHECK(m_primary_surface->Release(m_primary_surface));

	if (m_window)
		DFBCHECK(m_window->Release(m_window));

	if (m_layer)
		DFBCHECK(m_layer->Release(m_layer));
#else
	if (m_primary_surface)
		DFBCHECK(m_primary_surface->Release(m_primary_surface));
#endif // CORSAIR_USE_DFB_WINDOW

	if (m_dfb)
		DFBCHECK(m_dfb->Release(m_dfb));
}





CorsairGfx* CorsairGfx::getInstance()
{
	if (isShutdown == true)
		return NULL;
	static CorsairGfx* instance = NULL;
	if (!instance)
		instance = new CorsairGfx();
	return instance;
}

void CorsairGfx::initGogiScreen(void)
{
	for (SurfaceVector::iterator iter = m_surfaces.begin(); iter != m_surfaces.end(); ++iter)
	{
		CorsairSurface *surface = (*iter);
		if (!surface || !surface->init(m_gogi_format, m_flush_buffer))
		{
			//create or init failed
			if (surface)
				delete surface;
		}
	}
}
CorsairHbbTVSurface* CorsairGfx::createHbbTVSurface(int x, int y, int width, int height, int appId)
{
	CorsairHbbTVSurface* surface = new CorsairHbbTVSurface(x, y, width, height, appId);

#ifdef USE_VEGA_DFB
	if (!surface || !surface->init(m_gogi_format, m_flush_buffer))
	{
		//create or init failed
		if (surface)
			delete surface;
		return NULL;
	}
#endif
	HAPI_SetWindowStyle((HWINID_e)appId, eHWS_WindowInputEnable, TRUE); // for arm core issue
	m_surfaces.push_back(surface);
	return surface;
}

#ifdef OIPF
OipfSurface* CorsairGfx::createOipfSurface(int x, int y, int width, int height, int appId)
{
	OipfSurface* surface = new OipfSurface(x, y, width, height, appId);
#ifdef USE_VEGA_DFB
	if (!surface || !surface->init(m_gogi_format, m_flush_buffer))
	{
		//create or init failed
		if (surface)
			delete surface;
		return NULL;
	}
#endif

	HAPI_SetWindowStyle((HWINID_e)appId, eHWS_WindowInputEnable, TRUE); // for arm core issue

	m_surfaces.push_back(surface);
	return surface;
}
#endif // OIPF
#ifdef ENABLE_OPERA_TVSTORE
CorsairOTVSSurface* CorsairGfx::createOTVSSurface(int x, int y, int width, int height, int appId)
{
	CorsairOTVSSurface* surface = new CorsairOTVSSurface(x, y, width, height, appId);
#ifdef USE_VEGA_DFB

	if (!surface || !surface->init(m_gogi_format, m_flush_buffer))
	{
		//create or init failed
		if (surface)
			delete surface;
		return NULL;
	}
#endif
	m_surfaces.push_back(surface);
	return surface;
}
#endif

CorsairWebSurface* CorsairGfx::createWebSurface(int x, int y, int width, int height, string url, int appId)
{
	CorsairWebSurface* surface = new CorsairWebSurface(x, y, width, height, url, appId);
#ifdef USE_VEGA_DFB
	if (!surface || !surface->init(m_gogi_format, m_flush_buffer))
	{
		//create or init failed
		if (surface)
			delete surface;
		return NULL;
	}
#endif
	m_surfaces.push_back(surface);
	return surface;
}


void CorsairGfx::clearRect(IDirectFBSurface* surface, DFBRectangle* rect)
{
	if (!rect)
		return;

	DFBCHECK(surface->SetDrawingFlags(surface, DSDRAW_NOFX));
	DFBCHECK(surface->SetColor(surface, 0, 0, 0, 0));
	DFBCHECK(surface->FillRectangle(surface, rect->x, rect->y, rect->w, rect->h));
	DFBCHECK(surface->SetDrawingFlags(surface, DSDRAW_BLEND));
}


void CorsairGfx::updateScreens(bool force)
{
	for (SurfaceVector::iterator iter = m_surfaces.begin(); iter != m_surfaces.end(); ++iter)
		(*iter)->updateScreen(force);
}


void CorsairGfx::drawIntersection(CorsairSurface* surface, DFBRectangle* rect, bool clear)
{
	bool rect_cleared = false;

	for (SurfaceVector::iterator iter = m_surfaces.begin(); iter != m_surfaces.end(); ++iter)
	{
		if (!(*iter)->isVisible() || !(*iter)->isOnScreen())
			continue;

		if (clear && !rect_cleared)
		{
			clearRect(m_surface, rect);
			rect_cleared = true;
		}


		if (surface == *iter || (*iter)->intersects((DFBRectangle*)rect))
		{
			DFBRectangle surfaceRect;
			surfaceRect.x = rect->x - (*iter)->getCurrentXPos();
			surfaceRect.y = rect->y - (*iter)->getCurrentYPos();
			surfaceRect.w = rect->w;
			surfaceRect.h = rect->h;
			(*iter)->drawRect(&surfaceRect);
		}
	}
}


void CorsairGfx::flip(DFBRegion* region)
{
#ifdef CORSAIR_USE_DFB_WINDOW
	DFBCHECK(m_primary_surface->Flip(m_primary_surface, region, DSFLIP_ONSYNC));
#else
#ifdef NOFLIP
	DFBCHECK(m_primary_surface->Flip(m_primary_surface, region, DSFLIP_NONE));
#else
	DFBCHECK(m_primary_surface->Flip(m_primary_surface, region, DSFLIP_BLIT));
#endif //NOFLIP
#endif // CORSAIR_USE_DFB_WINDOW
}


void CorsairGfx::shutdown()
{
	isShutdown = true;

#ifdef USE_DIRECTFBGL
	CorsairGfxEgl::deinit();
#endif
	if (m_layer)
	{
		m_layer->Release(m_layer);
		m_layer = NULL;
	}

	if (m_dfb)
	{
		m_dfb->Release(m_dfb);
		m_dfb = NULL;
	}
	delete this;
}


void CorsairGfx::init(int argc, char** argv, gogi_flush_callback_t flush_buffer)
{
	DFBCHECK(DirectFBInit(&argc, &argv));
	DFBCHECK(DirectFBCreate(&m_dfb));
#if 1
	DFBGraphicsDeviceDescription	gdesc;
	IDirectFBDisplayLayer			*layer;
	DFBWindowDescription			desc;
	IDirectFBWindow					*window;

	m_dfb->GetDeviceDescription(m_dfb, &gdesc);
	DFBCHECK(m_dfb->GetDisplayLayer(m_dfb, DLID_PRIMARY, &m_layer));
	m_layer->SetLevel(m_layer, 0);
	m_layer->SetCooperativeLevel(m_layer, DLSCL_ADMINISTRATIVE);
#ifdef USE_DIRECTFBGL
	CorsairGfxEgl::init();
#endif
#else
#ifdef CORSAIR_USE_DFB_WINDOW
	DFBDisplayLayerConfig layerConfig;

	DFBCHECK(m_dfb->GetDisplayLayer(m_dfb, DLID_PRIMARY, &m_layer));
	DFBCHECK(m_layer->SetCooperativeLevel(m_layer, DLSCL_ADMINISTRATIVE));
	DFBCHECK(m_layer->GetConfiguration(m_layer, &layerConfig));
	DFBCHECK(m_layer->SetBackgroundColor(m_layer, 0, 0, 0, 0));

	layerConfig.flags = (DFBDisplayLayerConfigFlags)(layerConfig.flags | DLCONF_WIDTH | DLCONF_HEIGHT | DLCONF_PIXELFORMAT | DLCONF_BUFFERMODE | DLCONF_OPTIONS);
	layerConfig.width = m_screen_width;
	layerConfig.height = m_screen_height;
	layerConfig.buffermode = DLBM_BACKVIDEO;
// 	layerConfig.options = DLOP_ALPHACHANNEL;	// Abbe has this as part of his flickering work, but for us DirectFB won't start with it.
	layerConfig.pixelformat = DSPF_ARGB;

	DFBCHECK(m_layer->SetConfiguration(m_layer, &layerConfig));
	DFBCHECK(m_layer->EnableCursor(m_layer, 0));

	DFBWindowDescription  desc;
	desc.flags = (DFBWindowDescriptionFlags) (DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_PIXELFORMAT | DWDESC_SURFACE_CAPS | DWDESC_CAPS);
#if DIRECTFB_MAJOR_VERSION >= 1 && DIRECTFB_MINOR_VERSION >= 1
	desc.flags |= DWDESC_OPTIONS;
#endif
	desc.posx = -(m_screen_width); // make the window offscreen
	desc.posy = -(m_screen_height);
	desc.width = m_screen_width;
	desc.height = m_screen_height;
	desc.pixelformat = DSPF_ARGB;
	desc.surface_caps = (DFBSurfaceCapabilities) (DSCAPS_PREMULTIPLIED | DSCAPS_VIDEOONLY);
	desc.caps = (DFBWindowCapabilities) (DWCAPS_NODECORATION | DWCAPS_ALPHACHANNEL);
#if DIRECTFB_MAJOR_VERSION >= 1 && DIRECTFB_MINOR_VERSION >= 1
	desc.options = (DFBWindowOptions) (DWOP_KEEP_POSITION | DWOP_KEEP_SIZE | DWOP_KEEP_STACKING | DWOP_INDESTRUCTIBLE | DWOP_ALPHACHANNEL);
#endif
	DFBCHECK(m_layer->CreateWindow(m_layer, &desc, &m_window));
	DFBCHECK(m_window->GetSurface(m_window, &m_surface));

	// check if we had the video memory
	{
		void* surf_data;
		int pitch;
		if(m_surface->Lock(m_surface, (DFBSurfaceLockFlags)(DSLF_WRITE), &surf_data, &pitch) == DFB_OK)
			m_surface->Unlock(m_surface);
		else
		{
			LOG_ERR("Surface will not be created in videoonly memory...\n");
			m_window->Release(m_window);
			m_window = NULL;
			m_surface->Release(m_surface);
			m_surface = NULL;
			desc.surface_caps = DSCAPS_PREMULTIPLIED; // and not videoonly
			DFBCHECK(m_layer->CreateWindow(m_layer, &desc, &m_window));
			DFBCHECK(m_window->GetSurface(m_window, &m_surface));
		}
	}

	DFBSurfacePixelFormat dfb_format = DSPF_UNKNOWN;
	DFBCHECK(m_surface->GetPixelFormat(m_surface, &dfb_format));
	DFBCHECK((dfb_format == DSPF_ARGB) ? DFB_OK : DFB_FAILURE);

	DFBCHECK(m_window->MoveTo(m_window, 0, 0));
	DFBCHECK(m_window->SetStackingClass(m_window, DWSC_MIDDLE));

	DFBCHECK(m_window->SetOpacity(m_window, 0xFF));
	DFBCHECK(m_surface->SetBlittingFlags(m_surface, DSBLIT_BLEND_ALPHACHANNEL));
	DFBCHECK(m_surface->SetPorterDuff(m_surface, DSPD_SRC_OVER));
	DFBCHECK(m_surface->Clear(m_surface, 0xff, 0xff, 0xff, 0xff));
	DFBCHECK(m_surface->Flip(m_surface, NULL, DSFLIP_ONSYNC));

	m_primary_surface = m_surface;
#else
	DFBCHECK(m_dfb->SetCooperativeLevel (m_dfb, DFSCL_FULLSCREEN));

	DFBSurfaceDescription dsc;
#ifdef NOFLIP
	dsc.caps = (DFBSurfaceCapabilities)(DSCAPS_PRIMARY);
#else
	dsc.caps = (DFBSurfaceCapabilities)(DSCAPS_PRIMARY | DSCAPS_FLIPPING);
#endif // NOFLIP
	dsc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
#ifdef CORSAIR_USE_DFB_RESOURCE_ID
	dsc.flags = (DFBSurfaceDescriptionFlags)(dsc.flags | DSDESC_RESOURCE_ID);
	dsc.resource_id = CORSAIR_USE_DFB_RESOURCE_ID;
#endif // CORSAIR_USE_DFB_RESOURCE_ID

	dsc.pixelformat = m_dfb_format;
	dsc.height = m_screen_height;
	dsc.width = m_screen_width;
	DFBCHECK(m_dfb->CreateSurface(m_dfb, &dsc, &m_primary_surface));

	DFBCHECK(m_primary_surface->SetBlittingFlags(m_primary_surface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL)));
	DFBCHECK(m_primary_surface->SetDrawingFlags(m_primary_surface, DSDRAW_BLEND));
	DFBCHECK(m_primary_surface->SetColor(m_primary_surface, 0x0, 0x0, 0x0, 0xff));

	m_surface = m_primary_surface;
#endif // CORSAIR_USE_DFB_WINDOW
#endif
	m_flush_buffer = flush_buffer;
}


void CorsairGfx::destroySurface(CorsairSurface* surface)
{
	if (!surface)
		return;

	SurfaceVector::iterator it;
	for (it = m_surfaces.begin(); it != m_surfaces.end(); ++it)
		if ((*it) == surface)
			break;

	if (it == m_surfaces.end())
		return;

	m_surfaces.erase(it);
	delete surface;
	flip(NULL);
}


void CorsairGfx::drawRect(IDirectFBSurface* surface, DFBRectangle* rect, int curr_x, int curr_y)
{
 	if (!surface || !rect)
 		return;
 	DFBCHECK(m_surface->SetColor(m_surface, 0, 0, 0, 0xff));
 	DFBCHECK(m_surface->Blit(m_surface, surface, rect, rect->x + curr_x, rect->y + curr_y));
}
