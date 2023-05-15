// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "BookmarkCollection.h"
#include "NativeParentalControlManager.h"
#include "Bookmark.h"
#include "Exception.h"

#include "jsplugin.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Ooif;

BookmarkCollection::BookmarkCollection(void *sourceId, int count, BookmarkHandle *handles)
	: sourceIdentifier(sourceId)
{
	ENTRY;
	classType = CLASS_BOOKMARKCOLLECTION;
	for (int i=0; i<count; i++)
	{
		addItem(new Bookmark(handles[i]));
	}
	HLIB_STD_MemFree(handles);
}

BookmarkCollection::BookmarkCollection(void)
	: sourceIdentifier(0)
{
	ENTRY;
	classType = CLASS_BOOKMARKCOLLECTION;
}

BookmarkCollection::~BookmarkCollection()
{
	ENTRY;
}

int BookmarkCollection::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	int ret = Collection<Bookmark>::getter(obj, name, result);

	if (ret != JSP_GET_NOTFOUND)
		return ret;

	GET_FUNCTION(addBookmark, obj, "ns", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeBookmark, obj, "o", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}


//static
int BookmarkCollection::addBookmark(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                    int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(2);
	BookmarkCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, BookmarkCollection);
	BookmarkHandle handle;
	NATIVE_CALL(native_RecordingAddBookmark(instance->sourceIdentifier,
	                                  NUMBER_VALUE(argv[0]),
	                                  STRING_VALUE(argv[1]),
	                                  &handle));
	instance->addItem(new Bookmark(handle));
	RETURN_OBJECT(this_obj, new Bookmark(handle), result, JSP_CALL_VALUE);
}

//static
int BookmarkCollection::removeBookmark(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
    BookmarkCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, BookmarkCollection);

    CHECK_ARGUMENT_COUNT(1);
    Bookmark *bookmark = OBJECT_VALUE_OR_THROW(bookmark, argv[0], Bookmark);

    NATIVE_CALL(native_RecordingRemoveBookmark(instance->sourceIdentifier,
                                         bookmark->getIdentifier()));

    // The Bookmark object in the jsplugin object is not the same as
    // the one stored in the internal Collection, but the identifiers
    // should be the same.
    for (int i = 0; i< instance->getSize(); i++)
    {
    	if (instance->internalGetItem(i)->getIdentifier() == bookmark->getIdentifier())
    	{
    		instance->internalRemoveIndex(i);
    		break;
    	}
    }
    return JSP_CALL_NO_VALUE;
}
#endif // OIPF
