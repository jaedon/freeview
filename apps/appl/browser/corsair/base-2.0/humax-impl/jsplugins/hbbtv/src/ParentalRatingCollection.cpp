// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Exception.h"

#include "ParentalRatingCollection.h"
#include "NativeParentalControlManager.h"
#include "ParentalRating.h"

#include "jsplugin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Ooif;

ParentalRatingCollection::ParentalRatingCollection(void *owner)
	: CollectionOwner(owner)
{
	ENTRY;
	classType = CLASS_PARENTALRATINGCOLLECTION;
}

ParentalRatingCollection::~ParentalRatingCollection()
{
	ENTRY;
}

int ParentalRatingCollection::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	int ret = Collection<ParentalRating>::getter(obj, name, result);
	if (ret != JSP_GET_NOTFOUND)
		return ret;

#if defined(HBBTV_PVR) || defined(OIPF)
	GET_FUNCTION(addParentalRating, obj, "ssnn-", JSP_GET_VALUE_CACHE);
#endif //HBBTV_PVR || OIPF

	return JSP_GET_NOTFOUND;
}

int ParentalRatingCollection::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

#if defined(HBBTV_PVR) || defined(OIPF)
//static
int ParentalRatingCollection::addParentalRating(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT_MIN_MAX(4,5);

	ParentalRatingCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, ParentalRatingCollection);

	const char *scheme = STRING_VALUE(argv[0]);
	const char *name = STRING_VALUE(argv[1]);
	OOIFBoolean has_value = TYPE_NUMBER(argv[2]);
	OOIFNumber value = -1;
	if (has_value)
	{
		value = NUMBER_VALUE(argv[2]);
	}
	OOIFNumber labels = NUMBER_VALUE(argv[3]);
	const char *region = NULL;
	if (argc > 4 && TYPE_STRING(argv[4]))
	{
		region = STRING_VALUE(argv[4]);
	}

	ParentalRatingHandle pr;
	NATIVE_CALL(native_PRCollectionAddParentalRating(instance->CollectionOwner,
	            scheme, name,
	            has_value, value,
	            labels, region,
	            OOIFUtils::getWindow(this_obj),
	            OOIFUtils::getHost(this_obj),
	            &pr));
	instance->addItem(new ParentalRating(pr));
	return JSP_CALL_NO_VALUE;
}
#endif //HBBTV_PVR || OIPF
