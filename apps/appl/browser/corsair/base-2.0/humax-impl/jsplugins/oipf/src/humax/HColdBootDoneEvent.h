// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __COLD_BOOT_DONE_NOTIFIER_EVENT
#define __COLD_BOOT_DONE_NOTIFIER_EVENT

#include "Event.h"

namespace Ooif
{
	class HColdBootDoneEvent : public Event
	{
	private:


	public:
		HColdBootDoneEvent(jsplugin_obj *context);
		~HColdBootDoneEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __COLD_BOOT_DONE_NOTIFIER_EVENT
#endif // OIPF

