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

#include "oplbookmark.h"
#include "macros.h"

OOIFReturnCode native_BookmarkGetTime(BookmarkHandle identifier, OOIFNumber *retval)
{
	OPL_Bookmark_t	*pstBookmark = (OPL_Bookmark_t*)identifier;
	ENTRY;

	if (pstBookmark)
	{
		*retval = pstBookmark->time;
		return OOIF_RETURN_OK;
	}

	return OOIF_RETURN_VALUE_NULL;
}

OOIFReturnCode native_BookmarkGetName(BookmarkHandle identifier, const char **retval)
{
	OPL_Bookmark_t	*pstBookmark = (OPL_Bookmark_t*)identifier;
	ENTRY;

	if (pstBookmark)
	{
		if (pstBookmark->name)
		{
			*retval = pstBookmark->name;
			return OOIF_RETURN_OK;
		}
	}

	return OOIF_RETURN_VALUE_NULL;
}

OOIFReturnCode native_BookmarkReleaseHandle(BookmarkHandle id)
{
	OPL_Bookmark_t	*pstBookmark = (OPL_Bookmark_t*)id;
	ENTRY;

	if (pstBookmark)
	{
#if 0
		OPL_Bookmark_Delete(pstBookmark);
#else
		if (pstBookmark->name)
		{
			HLIB_STD_MemFree(pstBookmark->name);
			pstBookmark->name = NULL;
		}
		HLIB_STD_MemFree(pstBookmark);
#endif

		return OOIF_RETURN_OK;
	}

	return OOIF_RETURN_VALUE_NULL;
}
#endif // OIPF
