// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "CAReplaceEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

CAReplaceEvent::CAReplaceEvent(jsplugin_obj *c, const char *_message, ChannelHandle id)
	: Event(c, "CAReplace"), message(NULL), channel(NULL)
{
	ENTRY;
	if(_message != NULL)
	{
		// to guarantee a 'release' callback to integration without the
		// getter being called on the channel in case of a non-intrinsic
		// event, we need to create a channel here
		if(strcmp(_message, "NULL") == 0)
		{
			message = NULL;
		}
		else
		{
			message = HLIB_STD_StrDup(_message);
		}
	}
	if (id != 0)
		channel = new Channel(id);
}

CAReplaceEvent::~CAReplaceEvent()
{
	ENTRY;
	if(message != NULL)
	{
		HLIB_STD_MemFree(message);
	}
	delete channel;
}

int CAReplaceEvent::getter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	if (strcmp(name, "message") == 0) {
		if(message == NULL)
		{
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
		else
		{
			GET_STRING("message", message, JSP_GET_VALUE_CACHE);
		}
	}
	if (strcmp(name, "channel") == 0) {
		if (channel) {
			RETURN_OBJECT(obj, new Channel(channel->getIdentifier()), result, JSP_GET_VALUE_CACHE);
		} else {
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
	}

	return JSP_GET_NOTFOUND;
}

int CAReplaceEvent::setter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int CAReplaceEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	if(message == NULL)
	{
		result[0].type = JSP_TYPE_NULL;
	}
	else
	{
		result[0].type = JSP_TYPE_STRING;
		result[0].u.string = message;
	}
	if (channel) {
		SET_SERIALIZED_OBJECT( new Channel(channel->getIdentifier()), 1);
	} else {
		result[1].type = JSP_TYPE_NULL;
	}
	return 2;
}

