// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2012 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef JLABS
#ifndef __POWER_STATE_CHANGE_ERROR_EVENT
#define __POWER_STATE_CHANGE_ERROR_EVENT

#include "Event.h"

namespace Ooif
{
class PowerStateChangeErrorEvent : public Event
{
private:
	OOIFNumber reason;

public:
	PowerStateChangeErrorEvent(jsplugin_obj *context, OOIFNumber reason);
	~PowerStateChangeErrorEvent();

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

	virtual int serialize(jsplugin_value **values);
};
} /* namespace Ooif */
#endif // __POWER_STATE_CHANGE_ERROR_EVENT
#endif // OIPF
