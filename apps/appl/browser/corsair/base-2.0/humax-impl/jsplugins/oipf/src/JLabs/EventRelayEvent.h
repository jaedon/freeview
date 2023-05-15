// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __EVENT_RELAY_EVENT
#define __EVENT_RELAY_EVENT

#include "Event.h"
#include "NativeChannel.h"
#include "Channel.h"

namespace Ooif
{
	class EventRelayEvent : public Event
	{
	private:
		Channel *channel;
	public:
		EventRelayEvent(jsplugin_obj *c, ChannelHandle id);
		~EventRelayEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __EVENT_RELAY_EVENT