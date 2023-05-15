// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#include "HUsbWirelessStateChangedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HUsbWirelessStateChangedEvent::~HUsbWirelessStateChangedEvent()
{
}

HUsbWirelessStateChangedEvent::HUsbWirelessStateChangedEvent(jsplugin_obj *context, OOIFNumber _usbWlanDevId, OOIFNumber _usbWlanState)
  : Event(context, "UsbWirelessStateChanged"), usbWlanDevId(_usbWlanDevId), usbWlanState(_usbWlanState)
{
	ENTRY;
	this->usbWlanDevId= _usbWlanDevId;
	this->usbWlanState = _usbWlanState;
}

int HUsbWirelessStateChangedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("usbWlanDevId", usbWlanDevId, JSP_GET_VALUE_CACHE);
	GET_NUMBER("usbWlanState", usbWlanState, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HUsbWirelessStateChangedEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HUsbWirelessStateChangedEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = usbWlanDevId;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = usbWlanState;

	return 2;
}

