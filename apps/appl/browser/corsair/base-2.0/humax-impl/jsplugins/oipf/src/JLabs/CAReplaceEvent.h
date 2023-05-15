// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __CA_REPLACE_EVENT
#define __CA_REPLACE_EVENT

#include "Event.h"
#include "NativeChannel.h"
#include "Channel.h"

namespace Ooif
{
	class CAReplaceEvent : public Event
	{
	private:
		char *message;
		Channel *channel;

	public:
		CAReplaceEvent(jsplugin_obj *c, const char *_message, ChannelHandle id);
		~CAReplaceEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __CA_REPLACE_EVENT

