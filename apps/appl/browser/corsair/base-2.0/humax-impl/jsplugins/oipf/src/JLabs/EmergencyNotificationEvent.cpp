// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "EmergencyNotificationEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "Channel.h"
#include "Tuner.h"


using namespace Ooif;

EmergencyNotificationEvent::EmergencyNotificationEvent(jsplugin_obj *c, ChannelHandle chId, TunerHandle tunerId)
	: Event(c, "EmergencyNotification"), channel(NULL), tuner(NULL)
{
	ENTRY;
	// to guarantee a 'release' callback to integration without the
	// getter being called on the channel in case of a non-intrinsic
	// event, we need to create a channel here
	if (chId != 0)
		channel = new Channel(chId);

	if ((int)tunerId != -1)
		tuner = new Tuner(tunerId);
}

EmergencyNotificationEvent::~EmergencyNotificationEvent()
{
	ENTRY;
	delete channel;
	delete tuner;
}

int EmergencyNotificationEvent::getter(jsplugin_obj *obj, const char *name,
											   jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	if (strcmp(name, "channel") == 0) {
		if (channel) {
			RETURN_OBJECT(obj, new Channel(channel->getIdentifier()), result, JSP_GET_VALUE_CACHE);
		} else {
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
	}
	if (strcmp(name, "tuner") == 0) {
		if (tuner) {
			RETURN_OBJECT(obj, new Tuner(channel->getIdentifier()), result, JSP_GET_VALUE_CACHE);
		} else {
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
	}

	return JSP_GET_NOTFOUND;
}

int EmergencyNotificationEvent::setter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int EmergencyNotificationEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	if (channel) {
		SET_SERIALIZED_OBJECT( new Channel(channel->getIdentifier()), 0);
	} else {
		result[0].type = JSP_TYPE_NULL;
	}
	if (tuner) {
		SET_SERIALIZED_OBJECT( new Tuner(tuner->getIdentifier()), 1);
	} else {
		result[1].type = JSP_TYPE_NULL;
	}
	return 2;
}