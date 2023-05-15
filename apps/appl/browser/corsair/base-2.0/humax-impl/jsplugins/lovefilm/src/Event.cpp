// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Event.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "Exception.h"
#include "EventTarget.h"

using namespace Lovefilm;

NJSEBoolean hasEventPostfix(const char* str)
{
	unsigned int strlen_event =strlen("Event");
	unsigned int strlen_str = strlen(str);

	return (strlen_str >= strlen_event &&
	        strcmp(str + strlen_str - strlen_event, "Event") == 0 );
}

Event::Event(jsplugin_obj *c, const char *name) : wait_for_gc(false), context(c)
{
	eventName = strdup(name);
	classType = CLASS_EVENT;

	if (hasEventPostfix(name))
	{
		stringRep = strdup(name);
	}
	else
	{
		stringRep = (char*)malloc(strlen(name) + strlen("Event") + 1);
		strcpy(stringRep, name);
		strcat(stringRep, "Event");
	}
}

void Event::initialize()
{
	jsplugin_obj *event;
	int r = g_opera_callbacks->create_object(context,
	        ObjectInstance::getter_proxy,
	        ObjectInstance::setter_proxy,
	        ObjectInstance::destructor_proxy,
	        ObjectInstance::gc_proxy,
	        &event);

	if (r != JSP_CREATE_OK)
	{
		NJSE_LOG_ERROR("Event object could not be created");
		return;
	}

	r = g_opera_callbacks->add_unload_listener(event, ObjectInstance::unload_proxy);
	if (r != JSP_CALLBACK_OK)
		NJSE_LOG_WARNING("Failed to add unload listener");
	event->plugin_private = static_cast<ObjectInstance*>(this);
	setHost(event);
}

jsplugin_obj *Event::getHost()
{
	if (!host)
		initialize();
	return host;
}

Event::~Event()
{
	free(eventName);
	free(stringRep);
}

//static
int Event::toString(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	Event *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Event);
	result->type = JSP_TYPE_STRING;
	result->u.string = instance->stringRep;
	return JSP_CALL_VALUE;
}

//static
int Event::valueOf(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	result->type = JSP_TYPE_OBJECT;
	result->u.object = this_obj;
	return JSP_CALL_VALUE;
}

jsplugin_async_callback* Event::getListenerCallback()
{
	return EventTarget::async_callback;
}

/*
//virtual
int Event::serialize(jsplugin_value **argv)
{
	NJSE_LOG_WARNING("This event has not implemented serialize()");
	return 0;
}
*/
