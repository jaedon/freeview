// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "StreamBroadcastEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

//  StreamEventListener
StreamEventListener::StreamEventListener(jsplugin_obj *listener, const char* streamEventName)
	:  EventListener(listener, StreamBroadcastEvent::STREAM_EVENT_NAME)
{
	stream_event_name = HLIB_STD_StrDup(streamEventName);
}


StreamEventListener::~StreamEventListener()
{
	HLIB_STD_MemFree(stream_event_name);
}


bool StreamEventListener::matchEvent(Event* e)
{
	if (!EventListener::matchEvent(e))
		return false;

	StreamBroadcastEvent *event = static_cast<StreamBroadcastEvent*>(e);
	// Match also listener ID:s, to ensure that events only go to correct listeners.
	// This is needed since stream events can have the same names in different stream event objects.
	return (strcmp(event->getStreamEventName(), stream_event_name) == 0 && event->getListenerID() == (void*)listener_obj->opera_private);
}


bool StreamEventListener::matchListener(EventListener* l)
{
	// unique on: event_name + listener function + stream_event_name
	if (!EventListener::matchListener(l))
		return false;

	StreamEventListener *listener = static_cast<StreamEventListener*>(l);
	return (strcmp(listener->getStreamEventName(), stream_event_name) == 0);
}


// StreamEvent
const char *StreamBroadcastEvent::STREAM_EVENT_NAME = "StreamEvent";

StreamBroadcastEvent::StreamBroadcastEvent(jsplugin_obj *c, const char *name, void *listener_id,
        const char *data, const char *text,
        const char *status)
	: Event(c, STREAM_EVENT_NAME)
{
	ENTRY;
	this->name = HLIB_STD_StrDup(name);
	this->listener_id = listener_id;
	this->data = HLIB_STD_StrDup(data);
	this->text = HLIB_STD_StrDup(text);
	this->status = HLIB_STD_StrDup(status);
}

StreamBroadcastEvent::~StreamBroadcastEvent()
{
	ENTRY;
	HLIB_STD_MemFree(name);
	HLIB_STD_MemFree(data);
	HLIB_STD_MemFree(text);
	HLIB_STD_MemFree(status);
}

int StreamBroadcastEvent::getter(jsplugin_obj *obj, const char *name,
                                 jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
#ifdef OIPF
	GET_STRING("eventName", this->name, JSP_GET_VALUE_CACHE);
#else
	GET_STRING("name", this->name, JSP_GET_VALUE_CACHE);
#endif // OIPF
	GET_STRING("data", this->data, JSP_GET_VALUE_CACHE);
	GET_STRING("text", this->text, JSP_GET_VALUE_CACHE);
	GET_STRING("status", this->status, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int StreamBroadcastEvent::setter(jsplugin_obj *obj, const char *name,
                                 jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int StreamBroadcastEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[4];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = name;
	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = data;
	result[2].type = JSP_TYPE_STRING;
	result[2].u.string = text;
	result[3].type = JSP_TYPE_STRING;
	result[3].u.string = status;
	return 4;
}

