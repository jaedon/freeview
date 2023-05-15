// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "jsplugin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ParentalRatingSchemeCollection.h"
#include "NativeParentalControlManager.h"

#include "ParentalRatingScheme.h"
#include "Exception.h"

using namespace Ooif;

ParentalRatingSchemeCollection::ParentalRatingSchemeCollection()
{
	ENTRY;
	classType = CLASS_PARENTAL_RATING_SCHEME_COLLECTION;
	// Get complete list of parental schemes available
	ParentalRatingSchemeHandle *schemes = NULL;
	int schemesCount;
	NATIVE_WARN_NOK(native_ParentalControlGetParentalRatingSchemes(&schemesCount, &schemes));
	for (int i=0; i<schemesCount; i++)
	{
		addItem(new ParentalRatingScheme(schemes[i]));
	}
	HLIB_STD_MemFree(schemes);
}

ParentalRatingSchemeCollection::~ParentalRatingSchemeCollection()
{
	ENTRY;
}

int ParentalRatingSchemeCollection::getter(jsplugin_obj *obj, const char *name,
        jsplugin_value *result)
{
	ENTRY;

	int ret = Collection<ParentalRatingScheme>::getter(obj, name, result);
	if (ret != JSP_GET_NOTFOUND)
		return ret;
#ifdef OIPF
	GET_FUNCTION(addParentalRatingScheme, obj, "ss", JSP_GET_VALUE_CACHE);
#endif // OIPF
	GET_FUNCTION(getParentalRatingScheme, obj, "s", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

#ifdef OIPF
//static
int ParentalRatingSchemeCollection::addParentalRatingScheme(jsplugin_obj *this_obj,
        jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv,
        jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	/*jlabs에서는 add ParentalRating사용하지 않음.*/
#if defined(JLABS_JCOM)
	RETURN_UNDEFINED(JSP_CALL_VALUE);
#else
	ParentalRatingSchemeCollection *prsc = EXTRACT_OBJECT_OR_THROW(prsc, this_obj, ParentalRatingSchemeCollection);
	ParentalRatingSchemeHandle id;
	NATIVE_CALL(native_PRSCollectionAddParentalRatingScheme(STRING_VALUE(argv[0]),
	            STRING_VALUE(argv[1]), &id));
	prsc->addItem(new ParentalRatingScheme(id));
	RETURN_OBJECT(this_obj, new ParentalRatingScheme(id), result, JSP_CALL_VALUE);
#endif
}
#endif // OIPF

//static
int ParentalRatingSchemeCollection::getParentalRatingScheme(jsplugin_obj *this_obj,
        jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv,
        jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	ParentalRatingSchemeCollection *prsc = EXTRACT_OBJECT_OR_THROW(prsc, this_obj, ParentalRatingSchemeCollection);
	for(std::vector<ParentalRatingScheme*>::iterator it = prsc->items.begin(); it != prsc->items.end(); ++it)
	{
		ParentalRatingScheme* prs = (*it);
		if (prs)
		{
			ParentalRatingSchemeHandle id = prs->getIdentifier();
			const char* name = NULL;
			NATIVE_CALL(native_ParentalRatingSchemeGetName(id, &name));
			if (strcmp(STRING_VALUE(argv[0]), name) == 0)
			{
				RETURN_OBJECT(this_obj, new ParentalRatingScheme(*prs), result, JSP_CALL_VALUE);
			}
		}
	}
	RETURN_UNDEFINED(JSP_CALL_VALUE);
}

