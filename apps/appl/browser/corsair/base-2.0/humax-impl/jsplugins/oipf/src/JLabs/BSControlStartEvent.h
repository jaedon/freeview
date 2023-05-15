// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __BS_CONTROL_STARTED_EVENT
#define __BS_CONTROL_STARTED_EVENT

#include "Event.h"

namespace Ooif
{
	class BSControlStartEvent : public Event
	{
	public:
		BSControlStartEvent(jsplugin_obj *c);

		~BSControlStartEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __CA_REPLACE_EVENT


