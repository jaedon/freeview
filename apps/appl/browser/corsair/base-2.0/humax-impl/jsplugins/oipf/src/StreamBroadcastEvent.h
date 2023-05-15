// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __STREAM_EVENT
#define __STREAM_EVENT

#include "Event.h"
#include "EventTarget.h"

namespace Ooif
{
/**
   Due to the nature of stream events, listeners needs to be
   groupt by an additional event name. Ref. HBBTV 8.2.1.1

   (Intrinsic listeners are not supported for stream events.)
 */
class StreamEventListener : public EventListener
{
private:
	char *stream_event_name;

public:
	// unique identifier of sub class
	EventTargetType type()
	{
		return STREAM_EVENT;
	}

	StreamEventListener(jsplugin_obj *listener, const char* streamEventName);
	~StreamEventListener();

	bool matchListener(EventListener* listener);
	bool matchEvent(Event* event);

	const char* getStreamEventName()
	{
		return stream_event_name;
	}
};

class StreamBroadcastEvent : public Event
{
private:
	char *name;
	void *listener_id;
	char *data;
	char *text;
	char *status;
public:
	static const char *STREAM_EVENT_NAME;

	StreamBroadcastEvent(jsplugin_obj *c, const char *name, void* listener_id, const char *data,
	                     const char *text, const char *status);
	virtual ~StreamBroadcastEvent();

	const char* getStreamEventName() { return name; }
	void* getListenerID() { return listener_id; }

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __STREAM_EVENT
