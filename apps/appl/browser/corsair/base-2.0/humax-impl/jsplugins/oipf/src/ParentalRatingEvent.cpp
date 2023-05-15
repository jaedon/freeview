// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ParentalRatingEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "ParentalRating.h"


using namespace Ooif;

ParentalRatingEvent::ParentalRatingEvent(jsplugin_obj *c, const char *name, const char *contentID, ParentalRatingHandle rating, const char *DRMSystemID) :
	Event(c, name), rating(rating)
{
	ENTRY;

	this->contentID = contentID ? HLIB_STD_StrDup(contentID) : NULL;
	this->DRMSystemID = DRMSystemID ? HLIB_STD_StrDup(DRMSystemID) : NULL;
}

ParentalRatingEvent::~ParentalRatingEvent()
{
	ENTRY;
	if(contentID)
		HLIB_STD_MemFree(contentID);
	if(DRMSystemID)
		HLIB_STD_MemFree(DRMSystemID);
}

int ParentalRatingEvent::getter(jsplugin_obj *obj, const char *name,
                                jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	if( contentID )
	{
		GET_STRING("contentID", contentID, JSP_GET_VALUE_CACHE);
	}
	else
	{
		GET_NULL("contentID");
	}
	GET_OBJECT("rating", obj, new ParentalRating(rating), JSP_GET_VALUE_CACHE);
	if(DRMSystemID)
	{
		GET_STRING("DRMSystemID", DRMSystemID, JSP_GET_VALUE_CACHE);
	}
	else
	{
		GET_NULL("DRMSystemID");
	}

	return JSP_GET_NOTFOUND;
}

//virtual
int ParentalRatingEvent::serialize(jsplugin_value **argv)
{

	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;
	if (contentID)
	{
		result[0].type = JSP_TYPE_STRING;
		result[0].u.string = contentID;
	}
	else
	{
		result[0].type = JSP_TYPE_NULL;
	}

	SET_SERIALIZED_OBJECT(new ParentalRating(rating), 1);

	if (DRMSystemID)
	{
		result[2].type = JSP_TYPE_STRING;
		result[2].u.string = DRMSystemID;
	}
	else
	{
		result[2].type = JSP_TYPE_NULL;
	}
	return 3;
}
