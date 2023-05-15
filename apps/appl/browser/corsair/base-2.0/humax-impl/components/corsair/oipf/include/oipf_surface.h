/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Daniel Blizniak
 *
 */


#ifndef __OIPF_SURFACE_H
#define	__OIPF_SURFACE_H

#include "corsair_oipf.h"

class CorsairHbbTVSurface;

/**
   Surface class for windows running OIPF applications.

   This class allow (gogi) windows to be requested for use in the oipf scope.

   See \ref CorsairHbbTVSurface for usage of the class.
*/
class OipfSurface : public CorsairHbbTVSurface, public IOipfSurface
{
private:
	class WindowEqual
	{
	public:
		WindowEqual(const GogiOperaWindow* gogi_window) : m_gogi_window(gogi_window) { }
		bool operator()(const HbbTVWindowData &windowData) const { return windowData.gogi_window == m_gogi_window; }
	private:
		const GogiOperaWindow* m_gogi_window;
	};

public:
	OipfSurface(int x, int y, int w, int h);
	OipfSurface(int x, int y, int w, int h, int appId);
	virtual ~OipfSurface() { }
	
#ifdef OIPF_WIDGETS
	/** Register an externally created gogi window to this surface.
	 * This will allow the external OIPF gogi window to be refreshed and focused using the normal OIPF window handling strategy.
	 *
	 * @param gogi_window, the gogi window to register
	 */
	virtual void registerWindow(GogiOperaWindow* gogi_window);
	virtual void unregisterWindow(GogiOperaWindow* gogi_window);
#endif // OIPF_WIDGETS

	/**
	 * Request a gogi window.
	 *
	 * @return the created gogi window
	 */
	virtual GogiOperaWindow* requestWindow();

	void resizeGogiWindow(GogiOperaWindow* gogi_window, GogiRect& rect);
	bool releaseWindow(GogiOperaWindow* gogi_window);

	/** returns the focused OIPF applications window (this application is the primary receiver) */
	virtual HbbTVWindowData* getFocusedWindowData();

#ifdef OIPF_WIDGETS
	virtual GogiScreenHandle getScreenHandle() const { return m_screen; }
#endif // OIPF_WIDGETS

};

#endif	/* __OIPF_SURFACE_H */

