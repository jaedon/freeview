// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __USER_ACTION_IDLE_EVENT
#define __USER_ACTION_IDLE_EVENT

#include "Event.h"

namespace Ooif
{
	class HUserActionIdleEvent : public Event
	{
	private:
	public:
		HUserActionIdleEvent(jsplugin_obj *context);
		~HUserActionIdleEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __USER_ACTION_IDLE_EVENT
#endif // OIPF
