// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __EMERGENCY_NOTIFICATION_EVENT
#define __EMERGENCY_NOTIFICATION_EVENT

#include "Event.h"
#include "NativeChannel.h"
#include "Channel.h"
#include "Tuner.h"

namespace Ooif
{
	class EmergencyNotificationEvent : public Event
	{
	private:
		Channel 	*channel;
		Tuner	*tuner;
	public:
		EmergencyNotificationEvent(jsplugin_obj *c, ChannelHandle chId, TunerHandle tunerId);
		~EmergencyNotificationEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __EMERGENCY_NOTIFICATION_EVENT