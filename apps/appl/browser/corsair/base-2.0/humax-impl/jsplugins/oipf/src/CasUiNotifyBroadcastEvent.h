// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __CASUI_NOTIFY_BROADCAST_EVENT
#define __CASUI_NOTIFY_BROADCAST_EVENT

#include "Event.h"

namespace Ooif
{
	class CasUiNotifyBroadcastEvent : public Event
	{
	private:
		OOIFNumber module;
		OOIFNumber slot;
		OOIFNumber session;

	public:
		CasUiNotifyBroadcastEvent(jsplugin_obj *c, OOIFNumber _module, OOIFNumber _slot, OOIFNumber _session);
		~CasUiNotifyBroadcastEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __CASUI_NOTIFY_BROADCAST_EVENT

/* EOF */

