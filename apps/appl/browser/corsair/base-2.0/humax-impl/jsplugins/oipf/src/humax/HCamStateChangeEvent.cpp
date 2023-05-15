// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "HCamStateChangeEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HCamStateChangeEvent::~HCamStateChangeEvent()
{
}

HCamStateChangeEvent::HCamStateChangeEvent(jsplugin_obj *context, OOIFNumber _slotNumber, OOIFNumber _camState)
  : Event(context, "CamStateChange"), slotNumber(_slotNumber), camState(_camState)
{
	ENTRY;
	this->slotNumber = _slotNumber;
	this->camState = _camState;
}

int HCamStateChangeEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("slotNumber", slotNumber, JSP_GET_VALUE_CACHE);
	GET_NUMBER("camState", camState, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HCamStateChangeEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HCamStateChangeEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = slotNumber;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = camState;

	return 2;
}
#endif // OIPF
