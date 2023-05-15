// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __JLABS_PB_PROGRAMME
#define __JLABS_PB_PROGRAMME

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeJlabsPbProgramme.h"
#include "Collection.h"
#include "ParentalRatingCollection.h"

namespace Ooif
{
	class jlabsPbProgramme : public ObjectInstance, public WrapperObject<jlabsPbProgramme>
	{
	private:
		void init();
		ParentalRatingCollection *getParentalRatings(int count, ParentalRatingHandle *handles);

	public:
		~jlabsPbProgramme();
		jlabsPbProgramme(jlabsPbProgrammeHandle i);
		jlabsPbProgramme(const jlabsPbProgramme &original);

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static int getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getComponents(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void releaseNative(jlabsPbProgrammeHandle id);
		
		static const ClassType class_name = CLASS_JLABS_PBPROGRAMME;
	};
} /* namespace Ooif */


#endif // jlabsPbProgramme
