// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __PARENTAL_RATING_EVENT
#define __PARENTAL_RATING_EVENT

#include "Event.h"
#include "NativeParentalControlManager.h"
#include "ParentalRating.h"

namespace Ooif
{
class ParentalRatingEvent : public Event
{
private:
	char *contentID;
	ParentalRating rating;
	char *DRMSystemID;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	ParentalRatingEvent(jsplugin_obj *c, const char *name, const char * contentID, ParentalRatingHandle rating, const char *DRMSystemID);
	virtual ~ParentalRatingEvent();

	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __PARENTAL_RATING_EVENT
