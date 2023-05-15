// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __PROGRAMME
#define __PROGRAMME

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeProgramme.h"
#include "Collection.h"
#include "ParentalRatingCollection.h"

namespace Ooif
{
class Programme : public ObjectInstance, public WrapperObject<Programme>
{
private:
	void init();
	ParentalRatingCollection *getParentalRatings(int count, ParentalRatingHandle *handles);

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	virtual ~Programme();
	Programme(ProgrammeHandle i);
	Programme(const Programme &original);

	// Specific functions for this interface
	static int getSIDescriptors(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void releaseNative(ProgrammeHandle id);
	static const ClassType class_name = CLASS_PROGRAMME;
};
} /* namespace Ooif */


#endif // PROGRAMME
