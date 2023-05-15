// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "EventRelayEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "Channel.h"


using namespace Ooif;

EventRelayEvent::EventRelayEvent(jsplugin_obj *c, ChannelHandle id)
	: Event(c, "EventRelay"), channel(NULL)
{
	ENTRY;
	// to guarantee a 'release' callback to integration without the
	// getter being called on the channel in case of a non-intrinsic
	// event, we need to create a channel here
	if (id != 0)
		channel = new Channel(id);
}

EventRelayEvent::~EventRelayEvent()
{
	ENTRY;
	delete channel;
}

int EventRelayEvent::getter(jsplugin_obj *obj, const char *name,
											   jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	if (strcmp(name, "channel") == 0) {
		if (channel) {
			RETURN_OBJECT(obj, new Channel(channel->getIdentifier()), result, JSP_GET_VALUE_CACHE);
		} else {
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
	}

	return JSP_GET_NOTFOUND;
}

int EventRelayEvent::setter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int EventRelayEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	if (channel) {
		SET_SERIALIZED_OBJECT( new Channel(channel->getIdentifier()), 0);
	} else {
		result[0].type = JSP_TYPE_NULL;
	}
	return 1;
}
