// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ApplicationUnloadedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

ApplicationUnloadedEvent::~ApplicationUnloadedEvent()
{
	ENTRY;
	if (application)
		removeProtege(application);
}

ApplicationUnloadedEvent::ApplicationUnloadedEvent(jsplugin_obj *app, jsplugin_obj *c)
	: Event(c, "ApplicationUnloaded"), application(app)
{
	if (application)
		addProtege(application);
}

int ApplicationUnloadedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	// FIXME: Macroify?
	if (!strcmp(name, "appl"))
	{
		if (!application)
		{
			RETURN_NULL(JSP_GET_VALUE);
		}
		result->type = JSP_TYPE_OBJECT;
		result->u.object = application;
		return JSP_GET_VALUE;
	}

	return JSP_GET_NOTFOUND;
}

//virtual
int ApplicationUnloadedEvent::serialize(jsplugin_value **argv)
{
	if (!application)
		return 0;

	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_OBJECT;
	result[0].u.object = application;
	return 1;
}
