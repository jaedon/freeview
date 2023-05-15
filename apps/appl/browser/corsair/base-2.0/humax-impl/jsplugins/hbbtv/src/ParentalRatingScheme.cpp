// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ParentalRatingScheme.h"
#include "ParentalRating.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "Exception.h"
#include "macros.h"

using namespace Ooif;

void ParentalRatingScheme::init()
{
	classType = CLASS_PARENTALRATINGSCHEME;

	int count;
	const char **handles = NULL;
	NATIVE_WARN_NOK(native_ParentalRatingSchemeGetRatings(getIdentifier(), &count, &handles));
	for (int i=0; i<count; i++)
	{
		/**
		 * Strings are allocated using strdup function. The StringCollection class
		 * takes ownership of the memory.
		 */
		addItem(handles[i]);
	}
	HLIB_STD_MemFree(handles);
}

ParentalRatingScheme::ParentalRatingScheme(ParentalRatingSchemeHandle i) : WrapperObject<ParentalRatingScheme>(i)
{
	ENTRY;
	init();
}

ParentalRatingScheme::ParentalRatingScheme(const ParentalRatingScheme &original) : WrapperObject<ParentalRatingScheme>(original.identifier)
{
	ENTRY;
	init();
}

ParentalRatingScheme::~ParentalRatingScheme()
{
	ENTRY;
}

int ParentalRatingScheme::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("name", native_ParentalRatingSchemeGetName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_OBJECT("threshold", obj, native_ParentalRatingSchemeGetThreshold(identifier, &RETVAL), new ParentalRating(RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(indexOf, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(iconUri, obj, "n", JSP_GET_VALUE_CACHE);

	return StringCollection::getter(obj, name, result);
}

int ParentalRatingScheme::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	SET_READONLY("name");
	SET_READONLY("threshold");
	return JSP_PUT_NOTFOUND;
}

//static
int ParentalRatingScheme::indexOf(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	ParentalRatingScheme *prs = EXTRACT_OBJECT_OR_THROW(prs, this_obj, ParentalRatingScheme);

	for (int i=0; i < prs->getSize(); ++i)
	{
		if (strcmp(prs->items[i], STRING_VALUE(argv[0])) == 0)
			RETURN_NUMBER(i, JSP_GET_VALUE);
	}
	RETURN_NUMBER(-1, JSP_GET_VALUE); //not found, according to DAE spec.
}

//static
int ParentalRatingScheme::iconUri(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	ParentalRatingScheme *prs = EXTRACT_OBJECT_OR_THROW(prs, this_obj, ParentalRatingScheme);

	const char *retval;
	NATIVE_CALL(native_ParentalRatingSchemeIconUri(prs->getIdentifier(), NUMBER_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

void ParentalRatingScheme::releaseNative(ParentalRatingSchemeHandle id)
{
	NATIVE_WARN_NOK(native_ParentalRatingSchemeReleaseHandle(id));
}
