// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ParentalRating.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

using namespace Ooif;

ParentalRating::~ParentalRating()
{
	ENTRY;
}

int ParentalRating::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("name", native_ParentalRatingGetName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("scheme", native_ParentalRatingGetScheme(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("value", native_ParentalRatingGetValue(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("labels", native_ParentalRatingGetLabels(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("region", native_ParentalRatingGetRegion(identifier, &RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int ParentalRating::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	SET_READONLY("name");
	SET_READONLY("scheme");
	SET_READONLY("value");
	SET_READONLY("labels");
	SET_READONLY("region");
	return JSP_PUT_NOTFOUND;
}

void ParentalRating::releaseNative(ParentalRatingHandle id)
{
	NATIVE_WARN_NOK(native_ParentalRatingReleaseHandle(id));
}
