// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined OIPF && defined JLABS
#ifndef __JLABS_CONTENTRESULTS
#define __JLABS_CONTENTRESULTS

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"
#include "NativeBookmark.h"

#include "NativeJlabsContentResults.h"
#include "Collection.h"
#include "Query.h"

namespace Ooif
{
	class jlabsContentResults: public ObjectInstance, public WrapperObject<jlabsContentResults>
	{

		private :
		public:
			~jlabsContentResults();
			jlabsContentResults(jlabsContentResultsHandle id);
			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

			static int item(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int getResults(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int abort(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int update(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static void releaseNative(jlabsContentResultsHandle id);
			static const ClassType class_name = CLASS_JLABS_SEARCHRESULT;
	};
} /* namespace Ooif */

#endif // __JLABS_CONTENTSRESULTS
#endif



