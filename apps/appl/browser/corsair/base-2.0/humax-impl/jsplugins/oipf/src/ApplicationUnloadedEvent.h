// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __APPLICATION_UNLOADED_EVENT
#define __APPLICATION_UNLOADED_EVENT

#include "Event.h"

namespace Ooif
{
class ApplicationUnloadedEvent : public Event
{
private:
	jsplugin_obj *application;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	ApplicationUnloadedEvent(jsplugin_obj *app, jsplugin_obj *c);
	virtual ~ApplicationUnloadedEvent();

	virtual int serialize(jsplugin_value **values);
};
} /* namespace Ooif */
#endif // !__APPLICATION_UNLOADED_EVENT
