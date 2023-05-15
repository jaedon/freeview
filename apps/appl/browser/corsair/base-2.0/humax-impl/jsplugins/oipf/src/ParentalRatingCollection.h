// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/*
  Since ParentalRatingCollection has an additional function beyond
  normal Collections, it is necessary to create a special subclass of
  Collection<ParentalRatingScheme*> to implement it.
 */

#ifndef __PRCOLLECTION
#define __PRCOLLECTION

#include "jsplugin.h"

#include "macros.h"
#include "Collection.h"
#include "ParentalRating.h"

namespace Ooif
{

class ParentalRatingCollection : public Collection<ParentalRating>
{
private:
	/** Used to identify owner instance when adding a new rating. */
	void *CollectionOwner;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	ParentalRatingCollection(void *owner);
	virtual ~ParentalRatingCollection();

	// Javascript functions
#if defined(HBBTV_PVR) || defined(OIPF)
	static int addParentalRating(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // HBBTV_PVR || OIPF
	static const ClassType class_name = CLASS_PARENTALRATINGCOLLECTION;

};
} /* namespace Ooif */

#endif // __PRCOLLECTION
