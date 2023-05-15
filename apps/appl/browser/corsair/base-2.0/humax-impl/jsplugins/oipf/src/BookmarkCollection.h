// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/*
  Since BookmarkCollection has an additional function beyond normal
  Collections, it is necessary to create a special subclass of
  Collection<BookmarkScheme*> to implement it.
 */

#ifdef OIPF
#ifndef __BOOKMARKCOLLECTION
#define __BOOKMARKCOLLECTION

#include "jsplugin.h"

#include "NativeRecording.h"

#include "macros.h"
#include "Collection.h"
#include "Bookmark.h"

namespace Ooif
{
class BookmarkCollection : public Collection<Bookmark>
{
protected:
	void *sourceIdentifier;
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	// Get all bookmarks for a specific recording
	BookmarkCollection(void *sourceId, int count, BookmarkHandle *handles);
	BookmarkCollection(void);
	~BookmarkCollection();

	// Javascript functions
	static int addBookmark(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int removeBookmark(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static const ClassType class_name = CLASS_BOOKMARKCOLLECTION;
};
} /* namespace Ooif */

#endif // __BOOKMARKCOLLECTION
#endif // OIPF
