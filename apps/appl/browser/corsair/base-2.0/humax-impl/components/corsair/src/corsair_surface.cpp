/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Erik Lindström
 */

#include <iostream>
#include <hapi.h>
#include <directfb.h>

#include "corsair.h"
#include "corsair_debug.h"
#include "corsair_gfx.h"
#include "corsair_surface.h"

#ifdef USE_DIRECTFBGL
#include "corsair_gfx_egl.h"
#include "corsair_opengl_fbo.h"
#endif

CorsairSurface::CorsairSurface(int x, int y, int w, int h)
	: m_screen(NULL)
	, m_x_pos(x)
	, m_y_pos(y)
	, m_width(w)
	, m_height(h)
	, m_visible(false)
	, m_screen_invalid(true)
	, m_app_id(0xff)
{
	initDfbWindow(x, y, w, h);
}

CorsairSurface::CorsairSurface(int x, int y, int w, int h, int appId)
	: m_screen(NULL)
	, m_x_pos(x)
	, m_y_pos(y)
	, m_width(w)
	, m_height(h)
	, m_visible(false)
	, m_screen_invalid(true)
	, m_app_id(appId)
{
	initDfbWindow(x, y, w, h);

	//	for LXC, add virtual window with root pid
	HAPI_AddVirtualWindow((HWINID_e)appId);
	m_dfb_window->SetApplicationID(m_dfb_window, appId);
}


CorsairSurface::~CorsairSurface()
{
	if (m_dfb_surface)
	{
		DFBCHECK(m_dfb_surface->Release(m_dfb_surface));
	}

	if (m_dfb_window)
	{
		DFBCHECK(m_dfb_window->Release(m_dfb_window));
	}
		
	if (m_screen)
	{
		gogi_delete_screen(m_screen);
    }
}

void CorsairSurface::updateScreenInternal()
{
	gogi_update_screen(m_screen);
}

void CorsairSurface::clearSurface()
{
	CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
	if (!corsair_gfx)
		return;
	DFBRectangle rect;
  rect.x = m_x_pos;
  rect.y = m_y_pos;
  rect.w = m_width;
  rect.h = m_height;

	corsair_gfx->clearRect(m_dfb_surface, &rect);
	corsair_gfx->drawIntersection(this, &rect, false);
	DFBRegion region;
	region.x1 = m_width+m_x_pos-1;
	region.y1 = m_height+m_y_pos-1;
	region.x2 = m_x_pos;
	region.y2 = m_y_pos;
	corsair_gfx->flip(&region);
}

void CorsairSurface::drawRect(DFBRectangle* rect)
{
	if (!isVisible())
		return;

	CorsairGfx *corsair_gfx = CorsairGfx::getInstance();
	if (!corsair_gfx)
		return;

	corsair_gfx->drawRect(m_dfb_surface, rect, m_x_pos, m_y_pos);
}


bool CorsairSurface::intersects(DFBRectangle* rect)
{
	return (rect->x >= getCurrentXPos() && rect->x < getCurrentXPos() + getWidth()
	&& rect->y >= getCurrentYPos() && rect->y < getCurrentYPos() + getHeight());
}

void* CorsairSurface::getDataBuffer()
{
#ifdef USE_DIRECTFBGL
	return m_egl_object->getFBO();
#else
#ifdef USE_VEGA_DFB
	if(Corsair::uses_vega)
		return (void*)m_dfb_surface;
#endif
	void *surface_data = NULL;
	int pitch;
	m_dfb_surface->Lock(m_dfb_surface, DSLF_WRITE, &surface_data, &pitch);
	m_dfb_surface->Unlock(m_dfb_surface);

	return surface_data;
#endif
}


void CorsairSurface::updateScreen(bool force)
{
	if (force || ((m_screen_invalid)))
	{
		m_screen_invalid=false;
		updateScreenInternal();
		gettimeofday(&m_next_screen_update, NULL);

		CorsairUtils::usec_increase(this->m_next_screen_update, SCREEN_UPDATE_DELAY);
	}
}

void CorsairSurface::flip(DFBRegion *region)
{
#ifdef USE_DIRECTFBGL
	m_egl_object->draw(region->x1, region->y1, region->x2 - region->x1, region->y2 - region->y1);
	m_egl_object->swap();
#else
#ifdef USE_VEGA_DFB
	DFBCHECK(m_dfb_surface->Flip(m_dfb_surface, region, DSFLIP_BLIT));
#endif
#endif
}

bool CorsairSurface::init(GOGI_FORMAT gogi_format, gogi_flush_callback_t flush_buffer)
{
	CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
	if (!corsair_gfx)
		return false;

	if (gogi_new_screen(m_width, m_height,
						corsair_gfx->getOperaStride(),
						gogi_format,
						getDataBuffer(),
						this, &m_screen) != GOGI_STATUS_OK)
	{
		return false;
	}

	if (gogi_set_flush_callback(flush_buffer, m_screen) != GOGI_STATUS_OK)
	{
		return false;
	}

	return true;
}

void CorsairSurface::initDfbWindow(int x, int y, int w, int h)
{
	CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
	if (!corsair_gfx)
		return;

	IDirectFBDisplayLayer *layer;
	DFBWindowDescription desc;

	desc.posx = 0;
	desc.posy = 0;
	desc.height = corsair_gfx->getScreenHeight();
	desc.width = corsair_gfx->getScreenWidth();

#ifdef USE_DIRECTFBGL
	desc.pixelformat = DSPF_ABGR;
	desc.surface_caps = DSCAPS_GL;
	desc.caps = (DFBWindowCapabilities)DWCAPS_ALPHACHANNEL;
	desc.options = DWOP_ALPHACHANNEL;
	desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_SURFACE_CAPS | DWDESC_PIXELFORMAT | DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_OPTIONS | DWDESC_CAPS);
#else
	desc.options = DWOP_ALPHACHANNEL;
	desc.caps = (DFBWindowCapabilities)DWCAPS_ALPHACHANNEL;
	desc.flags = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_OPTIONS);
#endif

#ifdef CORSAIR_USE_DFB_RESOURCE_ID
	desc.flags = (DFBWindowDescriptionFlags)(desc.flags | DWDESC_RESOURCE_ID);
	desc.resource_id = CORSAIR_USE_DFB_RESOURCE_ID;
#endif // CORSAIR_USE_DFB_RESOURCE_ID

	IDirectFB* dfb = corsair_gfx->getDfbInstance();
	DFBCHECK(dfb->GetDisplayLayer(dfb, DLID_PRIMARY, &layer));
	DFBCHECK(layer->CreateWindow(layer, &desc, &m_dfb_window));
	m_dfb_window->GetSurface(m_dfb_window, &m_dfb_surface);

	DFBCHECK(m_dfb_surface->SetDrawingFlags(m_dfb_surface, DSDRAW_NOFX));
	DFBCHECK(m_dfb_surface->SetColor(m_dfb_surface, 0x0, 0x0, 0x0, 0x00));
	DFBCHECK(m_dfb_surface->FillRectangle(m_dfb_surface, 0, 0, corsair_gfx->getScreenWidth(), corsair_gfx->getScreenHeight()));  // 1080 p
	DFBCHECK(m_dfb_surface->Flip(m_dfb_surface, NULL, (DFBSurfaceFlipFlags)0));

	m_dfb_window->SetOpacity(m_dfb_window, 0xFF);
	m_dfb_window->RequestFocus(m_dfb_window);

	gettimeofday(&m_next_screen_update, NULL);

#ifdef USE_DIRECTFBGL
	m_egl_object = new CorsairOpenglFbo(m_dfb_surface, m_width, m_height);
	if ( !m_egl_object ) return;

	m_egl_object->init();
#endif
}
