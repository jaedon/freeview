// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __HUMAXAPPLICATION_UNLOADED_EVENT
#define __HUMAXAPPLICATION_UNLOADED_EVENT

#include <string>
#include "Event.h"

namespace Ooif
{
class HumaxApplicationUnloadedEvent : public Event
{
private:
	std::string app_name;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	HumaxApplicationUnloadedEvent(const char* app, jsplugin_obj *c);
	virtual ~HumaxApplicationUnloadedEvent();

	virtual int serialize(jsplugin_value **values);
};
} /* namespace Ooif */
#endif // !__APPLICATION_UNLOADED_EVENT
