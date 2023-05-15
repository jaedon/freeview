// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifndef __QUERY
#define __QUERY

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeSearch.h"
#include "Collection.h"
#include "ParentalRatingCollection.h"

namespace Ooif
{
class Query : public ObjectInstance, public WrapperObject<Query>
{

public:
	~Query();
	Query(QueryHandle i);
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	// Specific functions for this interface
	static int _and(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int _or(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int _not(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void releaseNative(QueryHandle id);
	static const ClassType class_name = CLASS_QUERY;
};
} /* namespace Ooif */

#endif // __QUERY
#endif // OIPF || HBBTV_1_2_1
