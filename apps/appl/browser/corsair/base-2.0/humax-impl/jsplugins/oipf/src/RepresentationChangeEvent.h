// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __REPRESENTATION_CHANGE_EVENT
#define __REPRESENTATION_CHANGE_EVENT

#include "Event.h"
#include "NativeVideoOnDemand.h"

namespace Ooif
{
class RepresentationChangeEvent : public Event
{
private:
	OOIFNumber _bandwidth;
	OOIFNumber _position;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	RepresentationChangeEvent(jsplugin_obj *c, const char *name, OOIFNumber bandwidth, OOIFNumber position);
	virtual ~RepresentationChangeEvent();

	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __REPRESENTATION_CHANGE_EVENT
