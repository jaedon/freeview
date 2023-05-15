// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "RecordingEventBroadcastEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

RecordingEventBroadcastEvent::RecordingEventBroadcastEvent(jsplugin_obj *c, OOIFNumber _state, OOIFBoolean _has_error, OOIFNumber _error, const char *_recordingID) :
	Event(c, "RecordingEvent"),
	state(_state),
	has_error(_has_error),
	error(_error),
	recordingID( HLIB_STD_StrDup( _recordingID ) )
{
	ENTRY;
}

RecordingEventBroadcastEvent::~RecordingEventBroadcastEvent()
{
	ENTRY;
	HLIB_STD_MemFree( recordingID );
}

int RecordingEventBroadcastEvent::getter(jsplugin_obj *obj, const char *name,
        jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("state", state, JSP_GET_VALUE_CACHE);
	if( has_error )
	{
		GET_NUMBER("error", error, JSP_GET_VALUE_CACHE);
	}
	else
	{
		GET_UNDEFINED("error");
	}
	GET_STRING("recordingID", recordingID, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int RecordingEventBroadcastEvent::setter(jsplugin_obj *obj, const char *name,
        jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int RecordingEventBroadcastEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[3];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = state;
	if( has_error )
	{
		result[1].type = JSP_TYPE_NUMBER;
		result[1].u.number = error;
	}
	else
	{
		result[1].type = JSP_TYPE_UNDEFINED;
	}
	result[2].type = JSP_TYPE_STRING;
	result[2].u.string = recordingID;
	return 3;
}
