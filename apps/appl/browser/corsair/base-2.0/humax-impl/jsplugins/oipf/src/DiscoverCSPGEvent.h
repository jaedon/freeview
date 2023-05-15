// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __DISCOVER_CSPG_EVENT
#define __DISCOVER_CSPG_EVENT

#include "Event.h"

namespace Ooif
{
class DiscoverCSPGEvent : public Event
{
public:
	~DiscoverCSPGEvent();
	DiscoverCSPGEvent(jsplugin_obj *c);

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

	virtual int serialize(jsplugin_value **values);
};
} /* namespace Ooif */
#endif // !__DISCOVER_CSPG_EVENT