// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "EmmNotifyNewMessageEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

EmmNotifyNewMessageEvent::EmmNotifyNewMessageEvent(jsplugin_obj *c, int _type, int _msg_id, jlabsEmmMessageHandle id)
	: Event(c, "EmmNotifyNewMessage"), type(_type), msg_id(_msg_id), new_msg(NULL)
{
	ENTRY;
	// to guarantee a 'release' callback to integration without the
	// getter being called on the channel in case of a non-intrinsic
	// event, we need to create a channel here
	if (id != 0)
		new_msg = new jlabsEmmMessage(id);
}

EmmNotifyNewMessageEvent::~EmmNotifyNewMessageEvent()
{
	ENTRY;
	delete new_msg;
}

int EmmNotifyNewMessageEvent::getter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("type", type, JSP_GET_VALUE_CACHE);
	GET_NUMBER("msgId", msg_id, JSP_GET_VALUE_CACHE);
	if (strcmp(name, "newMsg") == 0) {
		if (new_msg) {
			RETURN_OBJECT(obj, new jlabsEmmMessage(new_msg->getIdentifier()), result, JSP_GET_VALUE_CACHE);
		} else {
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
	}

	return JSP_GET_NOTFOUND;
}

int EmmNotifyNewMessageEvent::setter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int EmmNotifyNewMessageEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[3];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = type;
	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = msg_id;
	if (new_msg) {
		SET_SERIALIZED_OBJECT( new jlabsEmmMessage(new_msg->getIdentifier()), 2);
	} else {
		result[2].type = JSP_TYPE_NULL;
	}
	
	return 3;
}
