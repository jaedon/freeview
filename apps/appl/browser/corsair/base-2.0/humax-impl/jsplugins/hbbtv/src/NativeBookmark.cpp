// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "NativeBookmark.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

OOIFReturnCode native_BookmarkGetTime(BookmarkHandle identifier, OOIFNumber *retval)
{
	*retval = static_cast<OOIFNumber>(101 + reinterpret_cast<long>(identifier));
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BookmarkGetName(BookmarkHandle identifier, const char **retval)
{
	*retval = "Dummy Bookmark Name";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BookmarkReleaseHandle(BookmarkHandle id)
{
	return OOIF_RETURN_OK;
}
#endif // OIPF
