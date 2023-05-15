/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __CHANNEL_LIST_UPDATE_EVENT
#define __CHANNEL_LIST_UPDATE_EVENT

#include "Event.h"
#ifdef	HMX_WEBUI
#include "ChannelList.h"
#endif

namespace Ooif
{
	class ChannelListUpdateEvent : public Event
	{
#ifdef	HMX_WEBUI
	private:
		ChannelList	*updated;
#endif

	public:
		ChannelListUpdateEvent(jsplugin_obj *c, const char * event = "ChannelListUpdate");
#ifdef	HMX_WEBUI
		ChannelListUpdateEvent(ChannelList *list, jsplugin_obj *c, const char * event = "ChannelListUpdate");
#endif
		virtual ~ChannelListUpdateEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __CHANNEL_LIST_UPDATE_EVENT
