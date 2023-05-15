// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "BmlErrorEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

BmlErrorEvent::BmlErrorEvent(jsplugin_obj *c, int _error_number)
	: Event(c, "BmlError"), error_number(_error_number)
{
	ENTRY;
	// to guarantee a 'release' callback to integration without the
	// getter being called on the channel in case of a non-intrinsic
	// event, we need to create a channel here

}

BmlErrorEvent::~BmlErrorEvent()
{
	ENTRY;
}

int BmlErrorEvent::getter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("errorNumber", error_number, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int BmlErrorEvent::setter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int BmlErrorEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = error_number;
	return 1;
}
