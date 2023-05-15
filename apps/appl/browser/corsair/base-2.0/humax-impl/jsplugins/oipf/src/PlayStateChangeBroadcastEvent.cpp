// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "PlayStateChangeBroadcastEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

PlayStateChangeBroadcastEvent::PlayStateChangeBroadcastEvent(jsplugin_obj *c, OOIFNumber _state, OOIFBoolean _has_error, OOIFNumber _error )
	: Event(c, "PlayStateChange"), state(_state), has_error(_has_error), error(_error)
{
	ENTRY;
}

PlayStateChangeBroadcastEvent::~PlayStateChangeBroadcastEvent()
{
	ENTRY;
}

int PlayStateChangeBroadcastEvent::getter(jsplugin_obj *obj, const char *name,
        jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("state", state, JSP_GET_VALUE_CACHE);
	if(has_error)
	{
		GET_NUMBER("error", error, JSP_GET_VALUE_CACHE);
	}
	else
	{
		GET_UNDEFINED("error");
	}

	return JSP_GET_NOTFOUND;
}

int PlayStateChangeBroadcastEvent::setter(jsplugin_obj *obj, const char *name,
        jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int PlayStateChangeBroadcastEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = state;
	if(has_error)
	{
		result[1].type = JSP_TYPE_NUMBER;
		result[1].u.number = error;
	}
	else
	{
		result[1].type = JSP_TYPE_UNDEFINED;
	}
	return 2;
}
