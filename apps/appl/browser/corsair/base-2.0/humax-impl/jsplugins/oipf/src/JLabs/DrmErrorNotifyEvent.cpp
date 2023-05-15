// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "DrmErrorNotifyEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

DrmErrorNotifyEvent::DrmErrorNotifyEvent(jsplugin_obj *c, int _error_number, const char *_drm_system_id)
	: Event(c, "DRMErrorNotify"), error_number(_error_number), drm_system_id(NULL)
{
	ENTRY;
	if(_drm_system_id != NULL)
	{
		// to guarantee a 'release' callback to integration without the
		// getter being called on the channel in case of a non-intrinsic
		// event, we need to create a system id here
		drm_system_id = HLIB_STD_StrDup(_drm_system_id);
	}
}

DrmErrorNotifyEvent::~DrmErrorNotifyEvent()
{
	ENTRY;
	if(drm_system_id != NULL)
	{
		HLIB_STD_MemFree(drm_system_id);
	}
}

int DrmErrorNotifyEvent::getter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("errorNumber", error_number, JSP_GET_VALUE_CACHE);
	if(drm_system_id)
	{
		GET_STRING("DRMSystemID", drm_system_id, JSP_GET_VALUE);
	}
	else
	{
		GET_NULL("DRMSystemID");
	}

	return JSP_GET_NOTFOUND;
}

int DrmErrorNotifyEvent::setter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int DrmErrorNotifyEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = error_number;
	if(drm_system_id)
	{
		result[1].type = JSP_TYPE_STRING;
		result[1].u.string = drm_system_id;
	}
	else
	{
		result[1].type = JSP_TYPE_NULL;
	}
	return 2;
}
