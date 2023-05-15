// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ChannelChangeErrorBroadcastEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "Channel.h"


using namespace Ooif;

ChannelChangeErrorBroadcastEvent::ChannelChangeErrorBroadcastEvent(jsplugin_obj *c, ChannelHandle id, int error)
	: Event(c, "ChannelChangeError"), channel(NULL), errorState(error)
{
	ENTRY;
	// to guarantee a 'release' callback to integration without the
	// getter being called on the channel in case of a non-intrinsic
	// event, we need to create a channel here
	if (id != 0)
		channel = new Channel(id);
}

ChannelChangeErrorBroadcastEvent::~ChannelChangeErrorBroadcastEvent()
{
	ENTRY;
	delete channel;
}

int ChannelChangeErrorBroadcastEvent::getter(jsplugin_obj *obj, const char *name,
        jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	if (strcmp(name, "channel") == 0)
	{
		if (channel)
		{
			RETURN_OBJECT(obj, new Channel(channel->getIdentifier()), result, JSP_GET_VALUE_CACHE);
		}
		else
		{
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
	}
	GET_NUMBER("errorState", errorState, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int ChannelChangeErrorBroadcastEvent::setter(jsplugin_obj *obj, const char *name,
        jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int ChannelChangeErrorBroadcastEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	if (channel)
	{
		SET_SERIALIZED_OBJECT( new Channel(channel->getIdentifier()), 0);
	}
	else
	{
		result[0].type = JSP_TYPE_NULL;
	}
	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = errorState;
	return 2;
}
