// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __PARENTALRATINGSCHEME
#define __PARENTALRATINGSCHEME

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeParentalControlManager.h"

namespace Ooif
{
	class ParentalRatingScheme : public StringCollection, public WrapperObject<ParentalRatingScheme>
{
private:
	void init();

protected:
		virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	ParentalRatingScheme(ParentalRatingSchemeHandle i);
	ParentalRatingScheme(const ParentalRatingScheme &original);
	virtual ~ParentalRatingScheme();

	// Specific functions for this interface
	static int indexOf(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int iconUri(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#if (defined(JLABS_JCOM) || defined(HMX_WEBUI))
	/*
	 * This function originate from JLABS, But HUMAX WEB UI will be use, also.
	 */
	static int changeThreshold(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif
	static void releaseNative(ParentalRatingSchemeHandle id);
	static const ClassType class_name = CLASS_PARENTALRATINGSCHEME;
};
} /* namespace Ooif */

#endif // __PARENTALRATINGSCHEME
