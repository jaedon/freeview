// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "BmlEndEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

BMLEndEvent::BMLEndEvent(jsplugin_obj *c, int cause, const char *param)
	: Event(c, "BMLEnd")
{
	ENTRY;
	// to guarantee a 'release' callback to integration without the
	// getter being called on the channel in case of a non-intrinsic
	// event, we need to create a channel here
	this->cause = cause;
	this->param = HLIB_STD_StrDup(param);

}

BMLEndEvent::~BMLEndEvent()
{
	ENTRY;
	HLIB_STD_MemFree(this->param);
}

int BMLEndEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("cause", cause, JSP_GET_VALUE_CACHE);
	GET_STRING("param", param, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int BMLEndEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int BMLEndEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = cause;

	if (param) {
		result[1].type = JSP_TYPE_STRING;
		result[1].u.string = param;
	} else {
		result[1].type = JSP_TYPE_NULL;
	}
	return 2;
}
