// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "Bookmark.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "NativeBookmark.h"

#include "macros.h"

using namespace Ooif;

void Bookmark::init()
{
	classType = CLASS_BOOKMARK;
}

Bookmark::Bookmark(BookmarkHandle i) : WrapperObject<Bookmark>(i)
{
	ENTRY;
	init();
}

Bookmark::Bookmark(const Bookmark &original) : WrapperObject<Bookmark>(original.identifier)
{
	ENTRY;
	init();
}

Bookmark::~Bookmark()
{
	ENTRY;
}

int Bookmark::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_NUMBER("time", native_BookmarkGetTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("name", native_BookmarkGetName(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

void Bookmark::releaseNative(BookmarkHandle id)
{
	NATIVE_WARN_NOK(native_BookmarkReleaseHandle(id));
}

#endif // OIPF
