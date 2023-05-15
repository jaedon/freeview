// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "EventTarget.h"

#include <stdio.h>
#include <string.h>

#include "macros.h"
#include "Exception.h"

#include "Event.h"

using namespace Lovefilm;

/////////////////////////
//  EventListener (base class)
/////////////////////////

EventListener::EventListener(jsplugin_obj *listener, const char* eventName)
	: listener_obj(listener)
{
	event_name = strdup(eventName);
}


EventListener::~EventListener()
{
	free(event_name);
}


bool EventListener::overwrite()
{
	// no need to overwrite since the listener should be identical
	// with an already registered one, see: EventListener::matchListener.
	return false;
}


bool EventListener::matchEvent(Event* e)
{
	return (strcmp(e->eventName, event_name) == 0);
}


bool EventListener::matchListener(EventListener* l)
{
	// unique on: event_name + listener function;
	return ((l->type() == this->type()) &&
	        (strcmp(l->getEventName(), event_name) == 0) &&
	        l->getListenerObj() && listener_obj &&
	        (l->getListenerObj()->opera_private == listener_obj->opera_private));
}


void EventListener::dispatch(Event* event, jsplugin_obj *context)
{
	jsplugin_value argv[1];
	argv[0].type = JSP_TYPE_OBJECT;
	argv[0].u.object = event->getHost();

	if (!argv[0].u.object)
	{
		// we've failed to create_object due to OOM, give up on this event.
		NJSE_LOG_ERROR("failed to create event to dispatch");
		return;
	}

	int r = g_opera_callbacks->call_function(context, context,
	        getListenerObj(),
	        1, argv,
	        event->getUserData(),
	        event->getListenerCallback());
	if (r != JSP_CALL_NO_VALUE)
	{
		NJSE_LOG_ERROR("Dispatch event failed");
	}
	else
	{
		event->dispatched();
		// We will receive a GC call from jsapi later
		event->setWaitForGC();
	}
}


///////////////////////////
//  IntrinsicEventListener
///////////////////////////

bool IntrinsicEventListener::overwrite()
{
	// should be replaced with new listener if re-set since intrinsic
	// events can only have one listener
	return true;
}


bool IntrinsicEventListener::matchListener(EventListener* l)
{
	// unique on: event_name
	return ((l->type() == this->type()) &&
	        (strcmp(l->getEventName(), event_name) == 0));
}


void IntrinsicEventListener::dispatch(Event* event, jsplugin_obj *context)
{
	// Intrinsic events needs event parameters serialized
	jsplugin_value *argv;
	int argc = event->serialize(&argv);
	int r = g_opera_callbacks->call_function(context, context,
	        getListenerObj(),
	        argc, argv,
	        event->getUserData(),
	        event->getListenerCallback());
	if (r != JSP_CALL_NO_VALUE)
		NJSE_LOG_ERROR("Dispatch intrinsic event failed");
	delete[] argv;
}


IntrinsicEvalEventListener::IntrinsicEvalEventListener(const char* listener, const char* eventName)
			: IntrinsicEventListener(NULL, eventName), listener(NULL)
{
	if (listener)
		this->listener = strdup(listener);
}


IntrinsicEvalEventListener::~IntrinsicEvalEventListener()
{
	if (listener)
		free(listener);
}


void IntrinsicEvalEventListener::dispatch(Event* event, jsplugin_obj *context)
{
	int r = g_opera_callbacks->eval(context, listener, NULL, EventTarget::async_callback);

	if (r != JSP_CALLBACK_OK)
		NJSE_LOG_ERROR("Dispatch intrinsic string event failed");
}

/////////////////////////
//  EventTarget (base class)
/////////////////////////

//static
list<EventTarget*> EventTarget::targets;
//static
EventTarget *EventTarget::polling_context = NULL;

EventTarget::EventTarget()
{
	pthread_mutex_init(&lovefilm_mutex, NULL);
	EventTarget::targets.push_back(this);
}


EventTarget::~EventTarget()
{
	ENTRY;

	EventTarget::targets.remove(this);

	if (polling_context == this)
	{
		polling_context = NULL;
		findNewPollingContext();
	}

	removeAllListeners();

	pthread_mutex_lock( &lovefilm_mutex );
	Event *event;
	while (!event_queue.empty())
	{
		event = event_queue.front();
		event_queue.pop();
		delete event;
	}
	pthread_mutex_unlock( &lovefilm_mutex );
	pthread_mutex_destroy(&lovefilm_mutex);
}


void EventTarget::dispatchEvents()
{
	pthread_mutex_lock( &lovefilm_mutex );

	while (!event_queue.empty())
	{
		Event *event = event_queue.front();
		event_queue.pop();

		for (list<EventListener*>::iterator it = listeners.begin(); it!=listeners.end(); it++)
		{
			EventListener* listener = *it;
			if (listener->matchEvent(event))
				listener->dispatch(event, getHost());
		}

		// if an object representing the event was sent to JS script, the event will be deleted by GC instead.
		if (!event->waitForGC())
		{
			event->notFired();
			delete event;
		}
	}

	pthread_mutex_unlock( &lovefilm_mutex );
}


//static
void EventTarget::findNewPollingContext()
{
	// Just pick the first EventTarget in the list
	if (!targets.empty())
	{
		EventTarget *e = *targets.begin();
		polling_context = e;
		int r = g_opera_callbacks->set_poll_interval(e->getHost(), 50,
		        EventTarget::dispatchAllEvents);
		if (r != JSP_POLL_OK)
			NJSE_LOG_ERROR("Failed to set up polling callbacks");
	}
}


void EventTarget::addListener(EventListener *listener)
{
	addProtege(listener->getListenerObj());
	listeners.push_front(listener);
}


void EventTarget::removeListener(EventListener *listener)
{
	removeProtege(listener->getListenerObj());
	listeners.remove(listener);
	delete listener;
}


void EventTarget::removeAllListeners(const char* eventName)
{
	list<EventListener*>::iterator it = listeners.begin();
	while (it!=listeners.end())
	{
		EventListener *listener = *it;
		if (!eventName || (strcmp(eventName, listener->getEventName()) == 0))
		{
			removeProtege(listener->getListenerObj());
			it = listeners.erase(it);
			delete listener;
		}
		else
		{
			it++;
		}
	}
}


int EventTarget::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	// Event lister handling
	GET_FUNCTION(addEventListener, obj, "so-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, obj, "so-", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}


void EventTarget::addNewListener(EventListener *listener)
{
	ENTRY;

	for (list<EventListener*>::iterator it = listeners.begin(); it!=listeners.end(); it++)
	{
		EventListener* current = *it;
		if (listener->matchListener(current))
		{
			if (current->overwrite())
			{
				NJSE_LOG_DEBUG("replacing old event listener");
				removeListener(current);
				break;
			}
			else
			{
				NJSE_LOG_DEBUG("listener is a duplicate, not adding");
				delete listener;
				return;
			}
		}
	}
	NJSE_LOG_DEBUG("Adding new %s listener", listener->getEventName());
	addListener(listener);
}


EventListener *EventTarget::getMatchingListener(EventListener &listener)
{
	for (list<EventListener*>::iterator it = listeners.begin(); it!=listeners.end(); it++)
	{
		EventListener *current = *it;
		if (listener.matchListener(current))
		{
			return current;
		}
	}
	return NULL;
}


NJSEReturnCode EventTarget::internalAddEventListener(const char *eventName, jsplugin_value *listener, bool intrinsic)
{
	ENTRY;

	if (!listener)
	{
		NJSE_LOG_WARNING("Trying to add a NULL listener.");
		return NJSE_RETURN_OK;
	}

	NJSEReturnCode check = checkListenerAccessAllowed(eventName);
	if (check != NJSE_RETURN_OK)
		return check;

	EventListener *new_listener;
	if (listener->type == JSP_TYPE_STRING)
	{
		if (!intrinsic)
			return NJSE_RETURN_ERROR;
		new_listener = new IntrinsicEvalEventListener(listener->u.string, eventName);
	}
	else if (intrinsic)
	{
		new_listener = new IntrinsicEventListener(listener->u.object, eventName);
	}
	else
	{
		new_listener = new EventListener(listener->u.object, eventName);
	}

	if (!new_listener)
		return NJSE_RETURN_OOM_ERROR;

	addNewListener(new_listener);

	return NJSE_RETURN_OK;
}


NJSEReturnCode EventTarget::internalRemoveEventListener(const char *eventName,
											            jsplugin_value *listener)
{
	NJSEReturnCode check = checkListenerAccessAllowed(eventName);
	if (check != NJSE_RETURN_OK)
		return check;

	EventListener tmp_listener(listener->u.object, eventName);
	EventListener *matching_listener = getMatchingListener(tmp_listener);
	if (matching_listener)
	{
		NJSE_LOG_DEBUG("removing event listener on %s", eventName);
		removeListener(matching_listener);
	}

	return NJSE_RETURN_OK;
}


NJSEReturnCode EventTarget::removeIntrinsicListener(const char *eventName)
{
	NJSEReturnCode check = checkListenerAccessAllowed(eventName);
	if (check != NJSE_RETURN_OK)
		return check;

	IntrinsicEventListener tmp_listener(NULL, eventName);
	EventListener *matching_listener = getMatchingListener(tmp_listener);
	if (matching_listener)
	{
		NJSE_LOG_DEBUG("removing intrinsic listener on %s", eventName);
		removeListener(matching_listener);
	}

	return NJSE_RETURN_OK;
}


NJSEReturnCode EventTarget::findIntrinsicListener(const char *eventName, jsplugin_obj **retval)
{
	NJSEReturnCode check = checkListenerAccessAllowed(eventName);
	if (check != NJSE_RETURN_OK)
		return check;

	IntrinsicEventListener tmp_listener(NULL, eventName);
	EventListener *listener = getMatchingListener(tmp_listener);
	if (!listener)
		return NJSE_RETURN_VALUE_NULL;

	*retval= listener->getListenerObj();
	return NJSE_RETURN_OK;
}


NJSEReturnCode EventTarget::checkListenerAccessAllowed(const char *eventName)
{
	// Default behavior is to accept all access, this may be overridden in sub classes
	return NJSE_RETURN_OK;
}


void EventTarget::setHost(jsplugin_obj *h)
{
	// When the host is set - start polling
	ObjectInstance::setHost(h);
	if (!polling_context)
	{
		findNewPollingContext();
	}
}


void EventTarget::dispatchEvent(Event *event)
{
	ENTRY;
        
    if (event)
    {
	// All events are enqueued for thread safety
	pthread_mutex_lock( &lovefilm_mutex );
	event_queue.push(event);
	pthread_mutex_unlock( &lovefilm_mutex );
    }
}


//static
int EventTarget::dispatchAllEvents(const jsplugin_obj *obj)
{
	for (list<EventTarget*>::iterator it=targets.begin(); it!=targets.end(); it++)
	{
		(*it)->dispatchEvents();
	}
	return 1;
}


// static
void EventTarget::async_callback(int status, jsplugin_value *return_value, void *user_data)
{
	ENTRY;
}


// static
int EventTarget::addEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT_MIN_MAX(2, 3);
	if (argc != 2 && argc != 3)
	{
		NJSE_LOG_WARNING("Invalid argument count to addEventListener() %d "
		                 "instead of 2 or 3 - aborting", argc);
		return JSP_CALL_ERROR;
	}
	if (argv[0].type == JSP_TYPE_STRING && argv[1].type == JSP_TYPE_NATIVE_OBJECT)
	{
		EventTarget *et = EXTRACT_OBJECT_OR_THROW(et, this_obj, EventTarget);
		NATIVE_CALL(et->internalAddEventListener(argv[0].u.string, &(argv[1]), false));
		return JSP_CALL_NO_VALUE;
	}
	else
	{
		NJSE_LOG_WARNING("Invalid argument passed to addEventListener() - aborting");
		return JSP_CALL_ERROR;
	}
}

// static
int EventTarget::removeEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	if (argc != 2 && argc != 3)
	{
		NJSE_LOG_WARNING("Invalid argument count to removeEventListener() %d "
		                 "instead of 2 or 3 - aborting", argc);
		return JSP_CALL_ERROR;
	}
	if (argv[0].type == JSP_TYPE_STRING && argv[1].type == JSP_TYPE_NATIVE_OBJECT)
	{
		EventTarget *et = EXTRACT_OBJECT_OR_THROW(et, this_obj, EventTarget);
		NATIVE_CALL(et->internalRemoveEventListener(argv[0].u.string, &(argv[1])));
		return JSP_CALL_NO_VALUE;
	}
	else
	{
		NJSE_LOG_WARNING("Invalid argument passed to removeEventListener() - aborting");
		return JSP_CALL_ERROR;
	}
}
