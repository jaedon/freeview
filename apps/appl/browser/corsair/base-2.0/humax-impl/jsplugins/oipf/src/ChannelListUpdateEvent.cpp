/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ChannelListUpdateEvent.h"
#include "macros.h"
#include <string.h>

using namespace Ooif;

ChannelListUpdateEvent::ChannelListUpdateEvent(jsplugin_obj *c, const char * event) : Event(c, event)
{
	ENTRY;
}

#ifdef	HMX_WEBUI
ChannelListUpdateEvent::ChannelListUpdateEvent(ChannelList *list, jsplugin_obj *c, const char* event)
	: Event(c, event), updated(list)
{
	ENTRY;
}
#endif

ChannelListUpdateEvent::~ChannelListUpdateEvent()
{
	ENTRY;
#ifdef	HMX_WEBUI
	if (updated)
		delete updated;
#endif
}

int ChannelListUpdateEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

#ifdef	HMX_WEBUI
	if (updated)
		GET_OBJECT("channelList", obj, new ChannelList(updated), JSP_GET_VALUE);
	else
		GET_UNDEFINED("channelList");
#endif

	return JSP_GET_NOTFOUND;
}

int ChannelListUpdateEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int ChannelListUpdateEvent::serialize(jsplugin_value **argv)
{
	ENTRY;

#ifdef	HMX_WEBUI
	if (updated)
	{
		*argv = new jsplugin_value[1];
		jsplugin_value *result = *argv;

		SET_SERIALIZED_OBJECT(new ChannelList(updated), 0);
		return 1;
	}
#endif
	*argv = new jsplugin_value[0];

	return 0;
}
