// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "PlaySpeedChangedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

PlaySpeedChangedEvent::PlaySpeedChangedEvent(jsplugin_obj *c, OOIFNumber s)
	: Event(c, "PlaySpeedChanged"), speed(s)
{
	ENTRY;
}

PlaySpeedChangedEvent::~PlaySpeedChangedEvent()
{
	ENTRY;
}

int PlaySpeedChangedEvent::getter(jsplugin_obj *obj, const char *name,
                                  jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("speed", speed, JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

//virtual
int PlaySpeedChangedEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = speed;
	return 1;
}
