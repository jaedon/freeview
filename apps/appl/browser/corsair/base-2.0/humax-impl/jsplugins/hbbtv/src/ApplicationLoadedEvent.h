// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __APPLICATION_LOADED_EVENT
#define __APPLICATION_LOADED_EVENT

#include "Event.h"

namespace Ooif
{
class ApplicationLoadedEvent : public Event
{
private:
	jsplugin_obj *application;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	ApplicationLoadedEvent(jsplugin_obj *app, jsplugin_obj *c);
	virtual ~ApplicationLoadedEvent();

	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */
#endif // !__APPLICATION_LOADED_EVENT
