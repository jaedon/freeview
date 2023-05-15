/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Marek Sieradzki
 *
 */

#ifndef CORSAIR_OIPF_H
#define CORSAIR_OIPF_H

typedef void* HBBTV_WindowIdentifier;
typedef struct GOGI_OperaWindow GogiOperaWindow;
typedef struct GOGI_Rect GogiRect;
typedef void* GogiScreenHandle;

struct IAsteriks
{
	virtual bool requestOipfWindow(HBBTV_WindowIdentifier *id) = 0;
	virtual bool releaseOipfWindow(HBBTV_WindowIdentifier id) = 0;
	virtual void resizeOipfWindow(GogiOperaWindow* window, GogiRect& rect) = 0;
};

IAsteriks* opera_oipf_get_asteriks();

struct IOipfSurface
{
	virtual void registerWindow(GogiOperaWindow *w) = 0;
	virtual void unregisterWindow(GogiOperaWindow *w) = 0;

	virtual GogiScreenHandle getScreenHandle() const = 0;
};

IOipfSurface* opera_oipf_get_oipf_surface();

#endif
