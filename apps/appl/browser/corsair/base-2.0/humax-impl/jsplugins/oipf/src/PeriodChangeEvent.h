// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __PERIOD_CHANGE_EVENT
#define __PERIOD_CHANGE_EVENT

#include "Event.h"
#include "Collection.h"
#include "NativeVideoOnDemand.h"

namespace Ooif
{
class PeriodChangeEvent : public Event
{
private:
	NumberCollection *_bandwidths;
	OOIFNumber _position;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	PeriodChangeEvent(jsplugin_obj *c, const char *name, NumberCollection *bandwidths, OOIFNumber position);
	virtual ~PeriodChangeEvent();

	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __PERIOD_CHANGE_EVENT
