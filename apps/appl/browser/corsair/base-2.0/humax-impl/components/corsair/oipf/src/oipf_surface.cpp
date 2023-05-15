/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 */

#include <iostream>
#include <algorithm>
#include <cassert>

#include "gogi_opera_api.h"

#include "corsair_hbbtv_surface.h"
#include "oipf_api.h"
#include "oipf_surface.h"
#include "corsair_gfx.h"
#include "corsair_config.h"
#include "corsair.h"
using namespace std;

/**
 * OipfSurface
 *
 * Contains a GOGI window and a DirectFB surface and can be used for
 * OIPF content.
 */

OipfSurface::OipfSurface(int x, int y, int w, int h)
	: CorsairHbbTVSurface(x, y, w, h)
{
}

OipfSurface::OipfSurface(int x, int y, int w, int h, int appId)
	: CorsairHbbTVSurface(x, y, w, h, appId)
{
}

#ifdef OIPF_WIDGETS

void OipfSurface::registerWindow(GogiOperaWindow* gogi_window)
{
	HbbTVWindowData window_data(gogi_window, false, false);
	m_windows.push_back(window_data);
}

void OipfSurface::unregisterWindow(GogiOperaWindow* gogi_window)
{
	vector<HbbTVWindowData>::iterator i = find_if(m_windows.begin(), m_windows.end(), WindowEqual(gogi_window));

	assert(i != m_windows.end());
	if (i != m_windows.end())
	{
		m_windows.erase(i);
	}
}

#endif // OIPF_WIDGETS

GogiOperaWindow* OipfSurface::requestWindow()
{
	GogiOperaWindow* gogi_window = createGogiWindow();

	if (gogi_window)
	{
		HbbTVWindowData window_data(gogi_window);
		m_windows.push_back(window_data);
	}

	return gogi_window;
}

void OipfSurface::resizeGogiWindow(GogiOperaWindow* gogi_window, GogiRect& rect)
{
	assert(opera_hbbtv_get_window_identifier(gogi_window));

	GogiRect orig_rect;
	op_get_window_rect(gogi_window, &orig_rect);
	
	GogiRect new_rect = { rect.x >= 0 ? rect.x : orig_rect.x, rect.y >= 0 ? rect.y : orig_rect.y,
		rect.w >= 0 ? rect.w : orig_rect.w, rect.h >= 0 ? rect.h : orig_rect.h };
	
	op_set_window_rect(gogi_window, &new_rect);
}

bool OipfSurface::releaseWindow(GogiOperaWindow* gogi_window)
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
		return false;
	}

	if(it->is_owned_by_corsair)
		destroyGogiWindow(gogi_window);
	
	m_windows.erase(it);

	return true;
}

OipfSurface::HbbTVWindowData* OipfSurface::getFocusedWindowData()
{
	GogiOperaWindow *gogiWnd = opera_oipf_get_primary_receiver_window();

	if(!gogiWnd)
		return NULL;

	HbbTVWindowDataVector::iterator it = find_if(m_windows.begin(), m_windows.end(), WindowEqual(gogiWnd));

	return it != m_windows.end() ? &(*it) : NULL;
}
