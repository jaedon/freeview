// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __BOOKMARK
#define __BOOKMARK

#include "NativeBookmark.h"

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

namespace Ooif
{
/*
  The Bookmark object on this level is really only a single int
  identifying it to the native layer. In order to get the relevant
  information, it needs to ask the native layer.
 */

class Bookmark : public ObjectInstance, public WrapperObject<Bookmark>
{
private:
	void init();
protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
public:
	Bookmark(BookmarkHandle i);
	Bookmark(const Bookmark &original);
	~Bookmark();

	static void releaseNative(BookmarkHandle id);
	static const ClassType class_name = CLASS_BOOKMARK;
};
} /* namespace Ooif */

#endif // __BOOKMARK
#endif // OIPF
