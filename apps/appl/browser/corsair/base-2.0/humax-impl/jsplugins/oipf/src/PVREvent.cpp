// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "PVREvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "Recording.h"

using namespace Ooif;

PVREvent::~PVREvent()
{
	ENTRY;
}

PVREvent::PVREvent(RecordingHandle recordingHandle, int state, jsplugin_obj *c) 
  : Event(c, "PVREvent"), recording(recordingHandle), state(state)
{
}

int PVREvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("state", state, JSP_GET_VALUE_CACHE);
	GET_OBJECT("recording", obj, new Recording(recording), JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int PVREvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int PVREvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new Recording(recording), 0);

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = state;
	return 2;
}
#endif // OIPF
