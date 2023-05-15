// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef _HMX_DETACHED_MICROPHONE_EVENT_
#define _HMX_DETACHED_MICROPHONE_EVENT_

#include "Event.h"

namespace Ooif
{
	class HMXDetachedMicrophoneEvent : public Event
	{
	private:
		OOIFNumber devId;

	public:
		HMXDetachedMicrophoneEvent(jsplugin_obj *context, OOIFNumber _devId);
		~HMXDetachedMicrophoneEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // _HMX_DETACHED_MICROPHONE_EVENT_