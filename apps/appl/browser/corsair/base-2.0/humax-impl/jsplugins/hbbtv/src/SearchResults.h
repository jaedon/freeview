// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifndef __SEARCHRESULTS
#define __SEARCHRESULTS

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeSearch.h"

namespace Ooif
{
	/*
	  SearchResults is one of the few collection classes which does not
	  inherit from Collection. The nature of these results is that there
	  is no use in caching them, it is better to always retrieve them from
	  the native layer.
	*/
	class SearchResults : public ObjectInstance, public WrapperObject<SearchResults>
	{
		int getItem(jsplugin_obj* obj, int index, jsplugin_value* result);
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	public:
		SearchResults(SearchResultsHandle i);
		~SearchResults();

		// Specific functions for this interface
		static int item(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getResults(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int abort(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
		static int update(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
		static void releaseNative(SearchResultsHandle id);
		static const ClassType class_name = CLASS_SEARCHRESULT;
	};
} /* namespace Ooif */

#endif // __SEARCHRESULTS
#endif // OIPF || HBBTV_1_2_1
