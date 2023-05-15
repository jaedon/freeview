// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "PlayPositionChangedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

PlayPositionChangedEvent::PlayPositionChangedEvent(jsplugin_obj *c, OOIFBoolean has_pos, OOIFNumber pos)
	: Event(c, "PlayPositionChanged"), has_position(has_pos), position(pos)
{
	ENTRY;
}

PlayPositionChangedEvent::~PlayPositionChangedEvent()
{
	ENTRY;
}

int PlayPositionChangedEvent::getter(jsplugin_obj *obj, const char *name,
                                     jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	if (has_position)
	{
		GET_NUMBER("position", position, JSP_GET_VALUE);
	}
	else
	{
		GET_UNDEFINED("position");
	}

	return JSP_GET_NOTFOUND;
}

//virtual
int PlayPositionChangedEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	if (has_position)
	{
		result[0].type = JSP_TYPE_NUMBER;
		result[0].u.number = position;
	}
	else
	{
		result[0].type = JSP_TYPE_UNDEFINED;
	}
	return 1;
}
