// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ParentalRatingErrorEvent.h"
#include "macros.h"

#include "string.h"

using namespace Ooif;

ParentalRatingErrorEvent::ParentalRatingErrorEvent(jsplugin_obj* c
	, const char* contentID
#if defined(OIPF) || defined(HBBTV_1_2_1)
	, ParentalRatingHandle* ratings
	, int count
#else
	, ParentalRatingHandle rating
#endif
	, const char* DRMSystemID /*!< use NULL for JavaScripts's null */
	): Event(c, "ParentalRatingError")
#if defined(OIPF) || defined(HBBTV_1_2_1)
	, _ratings(ratings)
	, _count(count)
#else
	, _rating(rating)
#endif
{
	ENTRY;

	_contentID = (contentID ? HLIB_STD_StrDup(contentID) : NULL);
	_DRMSystemID = (DRMSystemID ? HLIB_STD_StrDup(DRMSystemID) : NULL);
}

ParentalRatingErrorEvent::~ParentalRatingErrorEvent()
{
	ENTRY;

	if (_contentID)
		HLIB_STD_MemFree(_contentID);
	if (_DRMSystemID)
		HLIB_STD_MemFree(_DRMSystemID);
#if defined(OIPF) || defined(HBBTV_1_2_1)
	if (_ratings)
		HLIB_STD_MemFree(_ratings);
#endif // OIPF || HBBTV_1_2_1
}

int ParentalRatingErrorEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_STRING("contentID", _contentID, JSP_GET_VALUE_CACHE);
#if defined(OIPF) || defined(HBBTV_1_2_1)
	GET_OBJECT("ratings", obj,  getParentalRatingCollection(_count, _ratings), JSP_GET_VALUE_CACHE);
#else
	GET_OBJECT("rating", obj, new ParentalRating(_rating), JSP_GET_VALUE_CACHE);
#endif

	GET_STRING("DRMSystemID", _DRMSystemID, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

//virtual
int ParentalRatingErrorEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[3];
	jsplugin_value *result = *argv;

	if (_contentID)
	{
		result[0].type = JSP_TYPE_STRING;
		result[0].u.string = _contentID;
	}
	else
		result[0].type = JSP_TYPE_NULL;

#if defined(OIPF) || defined(HBBTV_1_2_1)
	SET_SERIALIZED_OBJECT(getParentalRatingCollection(_count, _ratings), 1);
#else
	SET_SERIALIZED_OBJECT(new ParentalRating(_rating), 1);
#endif // HBBTV

	if (_DRMSystemID)
	{
		result[2].type = JSP_TYPE_STRING;
		result[2].u.string = _DRMSystemID;
	}
	else
		result[2].type = JSP_TYPE_NULL;

	return 3;
}

#if defined(OIPF) || defined(HBBTV_1_2_1)
ParentalRatingCollection* ParentalRatingErrorEvent::getParentalRatingCollection(int count, ParentalRatingHandle *ratings)
{
	ParentalRatingCollection *collection = new ParentalRatingCollection(NULL);
	for (int i=0; i<count; i++)
	{
		collection->addItem(new ParentalRating(ratings[i]));
	}
	return collection;
}
#endif // OIPF || HBBTV_1_2_1
