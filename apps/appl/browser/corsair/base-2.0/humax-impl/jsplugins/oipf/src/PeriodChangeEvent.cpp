// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "PeriodChangeEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

PeriodChangeEvent::PeriodChangeEvent(jsplugin_obj *c, const char *name,
                                     NumberCollection *bandwidths, OOIFNumber position)
	: Event(c, name), _bandwidths(bandwidths), _position(position)
{
	ENTRY;
}

PeriodChangeEvent::~PeriodChangeEvent()
{
	ENTRY;
	delete _bandwidths;
}

int PeriodChangeEvent::getter(jsplugin_obj *obj, const char *name,
                              jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	if (strcmp(name, "bandwidths") == 0)
	{
		RETURN_OBJECT(obj, _bandwidths, result, JSP_GET_VALUE_CACHE);
	}
	GET_NUMBER("position", _position, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

//virtual
int PeriodChangeEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_OBJECT;
	result[0].u.object = _bandwidths->getHost();

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = _position;

	return 2;
}
