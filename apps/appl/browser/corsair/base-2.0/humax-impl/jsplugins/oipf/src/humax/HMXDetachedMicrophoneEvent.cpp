// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "HMXDetachedMicrophoneEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HMXDetachedMicrophoneEvent::~HMXDetachedMicrophoneEvent()
{
}

HMXDetachedMicrophoneEvent::HMXDetachedMicrophoneEvent(jsplugin_obj *context, OOIFNumber _devId)
	: Event(context, "DetachedMicrophone"), devId(_devId)
{
	ENTRY;
	this->devId = _devId;
}

int HMXDetachedMicrophoneEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("id", devId, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HMXDetachedMicrophoneEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HMXDetachedMicrophoneEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = devId;

	return 1;
}

#endif // OIPF
