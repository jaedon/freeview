// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "DiscoverAGEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

DiscoverAGEvent::~DiscoverAGEvent()
{
	ENTRY;
}

DiscoverAGEvent::DiscoverAGEvent(jsplugin_obj *c) : Event(c, "DiscoverAGEvent")
{
}

int DiscoverAGEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int DiscoverAGEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int DiscoverAGEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[0];
	return 0;
}
