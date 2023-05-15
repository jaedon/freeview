// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
/*
  Since ParentalRatingCollection has an additional function beyond
  normal Collections, it is necessary to create a special subclass of
  Collection<ParentalRatingScheme*> to implement it.
 */

#ifndef __FAVLISTCOLLECTION
#define __FAVLISTCOLLECTION

#include "jsplugin.h"

#include "macros.h"
#include "Collection.h"
#include "FavouriteList.h"

namespace Ooif
{
class FavouriteListCollection : public Collection<FavouriteList>
{
public:
	FavouriteListCollection();
	~FavouriteListCollection();
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	// Javascript functions
	static int commit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getFavouriteList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createFavouriteList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static const ClassType class_name = CLASS_FAVOURITELISTCOLLECTION;
};
} /* namespace Ooif */

#endif // __FAVLISTCOLLECTION
#endif // OIPF
