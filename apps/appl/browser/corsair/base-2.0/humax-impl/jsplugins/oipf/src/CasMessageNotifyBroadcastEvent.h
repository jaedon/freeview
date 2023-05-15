// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __CASMESSAGE_NOTIFY_BROADCAST_EVENT
#define __CASMESSAGE_NOTIFY_BROADCAST_EVENT

#include "Event.h"

namespace Ooif
{
	class CasMessageNotifyBroadcastEvent : public Event
	{
	private:
		OOIFNumber module;
		OOIFNumber slot;
		OOIFNumber session;
		char *message;
		OOIFNumber message_length;

	public:
		CasMessageNotifyBroadcastEvent(jsplugin_obj *c, OOIFNumber _module, OOIFNumber _slot, OOIFNumber _session, const char *_message, OOIFNumber _message_length);
		~CasMessageNotifyBroadcastEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __CASMESSAGE_NOTIFY_BROADCAST_EVENT

/* EOF */
