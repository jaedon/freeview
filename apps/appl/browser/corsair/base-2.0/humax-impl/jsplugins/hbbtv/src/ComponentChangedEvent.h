// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

//HBBTV_2_0
#ifndef __COMPONENT_CHANGED_EVENT
#define __COMPONENT_CHANGED_EVENT

#include "Event.h"
#include "macros.h"

namespace Ooif
{
class ComponentChangedEvent : public Event
{
private:
	OOIFNumber _componentType;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	ComponentChangedEvent(jsplugin_obj *c, OOIFNumber componentType);
	virtual ~ComponentChangedEvent();
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __COMPONENT_CHANGED_EVENT

