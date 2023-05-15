/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Erik Lindström
 *
 */

#ifndef __CORSAIR_HBBTV_SURFACE_H
#define __CORSAIR_HBBTV_SURFACE_H

#include <directfb.h>
#include <vector>
#include "corsair_config.h"
#include "corsair_surface.h"
#include "gogi_opera_api.h"

/**
   Surface class for windows running HbbTV applications.

   This class allow (gogi) windows to be requested for use in the hbbtv scope.

   See \ref Corsair for usage of the class.
*/
class CorsairHbbTVSurface : public CorsairSurface
{
public:
	CorsairHbbTVSurface(int x, int y, int w, int h);
	CorsairHbbTVSurface(int x, int y, int w, int h, int appId);
	virtual ~CorsairHbbTVSurface();

	/** Gets the window that is currently focused in the surface */
	GogiOperaWindow* getWindow();

	/** Set surface visability */
	virtual void showSurface();
	virtual void hideSurface();

	/**
	 * Request a gogi window.
	 *
	 * @return the created gogi window
	 */
	virtual GogiOperaWindow* requestWindow();

	/**
	 * This is invoked from the HbbTV library to release a previously requested window.
	 * @return true if window was released, false if window was not found.
	 */
	virtual bool releaseWindow(GogiOperaWindow* window);

	/** Set gogi window visibility */
	void showWindow(GogiOperaWindow* window);
	void hideWindow(GogiOperaWindow* window);
	// We need to keep track of what window is "focused" in the
	// surface, i.a., what window is the one we will send input to and
	// to set as 'focused' in opera. In this implementation, the
	// focused window is always the latest requested window (the last
	// on in the vector). 'is_owned_by_corsair' flag indicates if
	// the gogi window is owned by Corsair or be an external
	// component (if owned by Corsair then Corsair will release it when needed)
	struct HbbTVWindowData
	{
		HbbTVWindowData(GogiOperaWindow* window = NULL, bool visible = false, bool owned_by_corsair = true);

		GogiOperaWindow* 	gogi_window;
		bool 				is_visible;
		bool 				is_owned_by_corsair;
		std::string 		url;
	};
	
	bool isFocusedWindow(GogiOperaWindow* gogi_window);
	virtual HbbTVWindowData* getFocusedWindowData();
	virtual bool findWindow( GogiOperaWindow* gogi_window );
	int getWindowSize() { return m_windows.size(); }

	void showWindowList();
	bool setWindowUrl(GogiOperaWindow* gogi_window, std::string url);

protected:
    GogiOperaWindow* createGogiWindow();
	void destroyGogiWindow(GogiOperaWindow* window);
    HbbTVWindowData* getWindowData(GogiOperaWindow* gogi_window);

protected:
	typedef std::vector<HbbTVWindowData> HbbTVWindowDataVector;
	HbbTVWindowDataVector m_windows;
};


#endif //__CORSAIR_HBBTV_SURFACE_H
