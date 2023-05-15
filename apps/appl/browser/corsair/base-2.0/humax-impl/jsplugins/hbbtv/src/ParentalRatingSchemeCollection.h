// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/*
  Since ParentalRatingSchemeCollection has two additional functions
  beyond normal Collections, it is necessary to create a special
  subclass of Collection<ParentalRatingScheme*> to implement it.
 */

#ifndef __PRSCOLLECTION
#define __PRSCOLLECTION

#include "jsplugin.h"

#include "macros.h"
#include "Collection.h"
#include "ParentalRatingScheme.h"

namespace Ooif
{
class ParentalRatingSchemeCollection : public Collection<ParentalRatingScheme>
{
public:
	ParentalRatingSchemeCollection();
	virtual ~ParentalRatingSchemeCollection();

	// Javascript functions
#ifdef OIPF
	static int addParentalRatingScheme(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
	static int getParentalRatingScheme(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static const ClassType class_name = CLASS_PARENTAL_RATING_SCHEME_COLLECTION;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
};
} /* namespace Ooif */

#endif // __PRSCOLLECTION
