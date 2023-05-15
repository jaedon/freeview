// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "DRMRightsErrorEventBroadcastEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

DRMRightsErrorEventBroadcastEvent::DRMRightsErrorEventBroadcastEvent(jsplugin_obj *c,
        int error_state,
        const char* content_id,
        const char* drm_system_id,
        const char* rights_issuer_url)
	: Event(c, "DRMRightsError")
{
	ENTRY;

	errorState = error_state;
	contentID = HLIB_STD_StrDup(content_id);
	DRMSystemID = HLIB_STD_StrDup(drm_system_id);
	if(rights_issuer_url)
		rightsIssuerURL = HLIB_STD_StrDup(rights_issuer_url);
	else
		rightsIssuerURL = NULL;
}

DRMRightsErrorEventBroadcastEvent::~DRMRightsErrorEventBroadcastEvent()
{
	ENTRY;

	HLIB_STD_MemFree(contentID);
	HLIB_STD_MemFree(DRMSystemID);
	HLIB_STD_MemFree(rightsIssuerURL);

}

int DRMRightsErrorEventBroadcastEvent::getter(jsplugin_obj *obj, const char *name,
        jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("errorState", errorState, JSP_GET_VALUE);
	GET_STRING("contentID", contentID, JSP_GET_VALUE);
	GET_STRING("DRMSystemID", DRMSystemID, JSP_GET_VALUE);

	if (strcmp(name, "rightsIssuerURL") == 0)
	{
		if (rightsIssuerURL)
		{
			RETURN_STRING(rightsIssuerURL, JSP_GET_VALUE);
		}
		else
		{
			RETURN_NULL(JSP_GET_VALUE);
		}
	}

	return JSP_GET_NOTFOUND;
}

//virtual
int DRMRightsErrorEventBroadcastEvent::serialize(jsplugin_value **argv)
{
	int argc = rightsIssuerURL ? 4 : 3;
	*argv = new jsplugin_value[argc];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = errorState;

	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = contentID;

	result[2].type = JSP_TYPE_STRING;
	result[2].u.string = DRMSystemID;

	if (rightsIssuerURL)
	{
		result[3].type = JSP_TYPE_STRING;
		result[3].u.string = rightsIssuerURL;
	}

	return argc;
}
