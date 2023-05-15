// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __EVENTTARGET
#define __EVENTTARGET

#include "jsplugin.h"

#include <queue>
#include <list>
#include <pthread.h>

#include "ObjectInstance.h"
#include "NativeOOIFTypes.h"
#include "Event.h"

using namespace std;

namespace Ooif
{

/**
	Super class for event listeners in OOIF. Implements the generic
	DOM listeners. Example:
		video.addEventListener("PlayStateChange", listener, false);
		video.removeEventListener("PlayStateChange", listener);

	Multiple listeners can be registered on an event on an object,
	hence an event must be identified on listener object as well as
	  event name.
 */
class EventListener
{
protected:
	jsplugin_obj *listener_obj;
	char *event_name;

public:
	typedef enum {
		GENERIC_EVENT = 1
		, INTRINSIC_EVENT = 2
		, STREAM_EVENT = 3
	} EventTargetType;

	// unique identifier for the class, override in sub classes with new unique value!
	virtual EventTargetType type() { return GENERIC_EVENT; }
	EventListener(jsplugin_obj *listener, const char* eventName);
	virtual ~EventListener();

	virtual bool overwrite();
	virtual bool matchEvent(Event* event);
	virtual bool matchListener(EventListener* listener);
	virtual void dispatch(Event* event, jsplugin_obj *context);

	jsplugin_obj *getListenerObj()  { return listener_obj; }
	const char *getEventName()  { return event_name; }
};

/**
	Intrinsic listeners are registered as attributes on an
	object. Example:
		video.onPlayStateChange = listener;
	There can only be one listener per event, hence the
	listener needs to be overwritten if a new listener is set.
 */
class IntrinsicEventListener : public EventListener
{
public:
	// unique identifier of sub class
	EventTargetType type() { return INTRINSIC_EVENT; }
	IntrinsicEventListener(jsplugin_obj *listener, const char* eventName): EventListener(listener, eventName) {}
	virtual ~IntrinsicEventListener() {}

	bool overwrite();
	bool matchListener(EventListener* listener);
	void dispatch(Event* event, jsplugin_obj *context);
};

/**
   Intrinsic "eval" listeners are registered as attributes on an
   object inside the document or using setAttribute call. Example:

   <object type="video/broadcast" onfullscreenchange="alert('hello!');" />
   The value is a string which is a javascript code to be evaluated
   when event is fired. These events overwrite each other with other
   intrinsic events of any kind.
   This is a special example of IntrinsicEventListener and is interchangable
   and inter-replaceable, so it possesses the same unique identifier / type().
*/
class IntrinsicEvalEventListener : public IntrinsicEventListener
{
protected:
	char* listener;
public:
	IntrinsicEvalEventListener(const char* listener, const char* eventName);
	~IntrinsicEvalEventListener();

	void dispatch(Event* event, jsplugin_obj *context);
};

class EventTarget : public ObjectInstance
{
private:
	pthread_mutex_t ooif_mutex;
	queue<Event*> event_queue;
	list<EventListener*> listeners;
	static list<EventTarget*> targets;

	void dispatchEvents();

	// If there are any event targets, one of them is always the
	// polling context, making sure that we always receive polls to
	// dispatch events. If the polling context is unloaded or deleted,
	// a new one is appointed and a new polling is started
	static EventTarget *polling_context;
	static void findNewPollingContext();

protected:
	// list access and GC protect
	void addListener(EventListener *listener);
	void removeListener(EventListener *listener);
	void removeAllListeners(const char* eventName=NULL);

	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

	// help functions
	void addNewListener(EventListener *listener);
	EventListener *getMatchingListener(EventListener &listener);

	OOIFReturnCode internalAddEventListener(const char *eventName, jsplugin_value *listener, bool intrinsic);
	OOIFReturnCode internalRemoveEventListener(const char *eventName, jsplugin_value *listener);
	OOIFReturnCode removeIntrinsicListener(const char *eventName);
	OOIFReturnCode findIntrinsicListener(const char *eventName, jsplugin_obj** retval);

	virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);

public:
	EventTarget();
	virtual ~EventTarget();

	// Overrides ObjectInstance::setHost
	void setHost(jsplugin_obj *h);
	// add a new event to be sent
	void dispatchEvent(Event *event);

	static int dispatchAllEvents(const jsplugin_obj *global_context);
	static void async_callback(int status, jsplugin_value *return_value, void *user_data);

	// Javascript functions
	static int addEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int removeEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
};
} /* namespace Ooif */

#endif // __EVENTTARGET
