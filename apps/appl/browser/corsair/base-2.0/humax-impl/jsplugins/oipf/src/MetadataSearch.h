// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifndef __METADATASEARCH
#define __METADATASEARCH

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeSearch.h"
#include "Collection.h"
#include "ParentalRatingCollection.h"

namespace Ooif
{
class MetadataSearch : public ObjectInstance, public WrapperObject<MetadataSearch>
{
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	~MetadataSearch();
	MetadataSearch(MetadataSearchHandle i);

		static int setQuery(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
	static int addChannelConstraint(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int orderBy(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int addCurrentRatingConstraint(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int addRatingConstraint(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef	HMX_WEBUI
	static int addConstraint(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif
#endif // OIPF
	static int createQuery(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int findProgrammesFromStream(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void releaseNative(MetadataSearchHandle id);

	static const ClassType class_name = CLASS_METADATASEARCH;
};
} /* namespace Ooif */

#endif // __METADATASEARCH
#endif // OIPF || HBBTV_1_2_1
