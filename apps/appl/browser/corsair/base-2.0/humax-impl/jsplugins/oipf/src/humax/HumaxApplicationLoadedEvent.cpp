// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "HumaxApplicationLoadedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HumaxApplicationLoadedEvent::~HumaxApplicationLoadedEvent()
{
	ENTRY;
}

HumaxApplicationLoadedEvent::HumaxApplicationLoadedEvent(const char *app, jsplugin_obj *c)
	: Event(c, "HumaxApplicationLoaded"), app_name(app)
{
	
}

int HumaxApplicationLoadedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_STRING("name", app_name.c_str(), JSP_GET_VALUE );
	
	return JSP_GET_NOTFOUND;
}

//virtual
int HumaxApplicationLoadedEvent::serialize(jsplugin_value **argv)
{
	if (!app_name.size())
		return 0;

	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = app_name.c_str();
	return 1;
}
