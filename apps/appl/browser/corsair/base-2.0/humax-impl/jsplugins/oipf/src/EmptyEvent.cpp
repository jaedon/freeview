// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "EmptyEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

EmptyEvent::~EmptyEvent()
{
	ENTRY;
}

EmptyEvent::EmptyEvent(jsplugin_obj *c, const char* name)
	: Event(c, name)
{
}

int EmptyEvent::getter(jsplugin_obj *obj, const char *name,
                       jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

//virtual
int EmptyEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[0];
	return 0;
}
