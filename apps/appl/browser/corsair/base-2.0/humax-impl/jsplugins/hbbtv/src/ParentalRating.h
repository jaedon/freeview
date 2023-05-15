// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __PARENTALRATING
#define __PARENTALRATING

#include "NativeParentalControlManager.h"

#include "ObjectInstance.h"
#include "WrapperObject.h"

namespace Ooif
{
class ParentalRating : public ObjectInstance, public WrapperObject<ParentalRating>
{
public:
	ParentalRating(ParentalRatingHandle i) : WrapperObject<ParentalRating>(i) {};
	ParentalRating(const ParentalRating &original) : WrapperObject<ParentalRating>(original.identifier) {};
	virtual ~ParentalRating();

	static void releaseNative(ParentalRatingHandle id);

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
};
} /* namespace Ooif */

#endif // __PARENTALRATING
