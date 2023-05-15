// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2012 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef JLABS

#include "PowerStateChangeErrorEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

PowerStateChangeErrorEvent::~PowerStateChangeErrorEvent()
{
}

PowerStateChangeErrorEvent::PowerStateChangeErrorEvent(jsplugin_obj *context, OOIFNumber reason)
	: Event(context, "PowerStateChangeError")
{
	ENTRY;
	this->reason = reason;
}

int PowerStateChangeErrorEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("reason", reason, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int PowerStateChangeErrorEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int PowerStateChangeErrorEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = reason;
	return 1;
}
#endif // OIPF