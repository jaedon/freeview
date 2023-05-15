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
	if(content_id)
		contentID = HLIB_STD_StrDup(content_id);
	else
		contentID = NULL;
	if(drm_system_id)
		DRMSystemID = HLIB_STD_StrDup(drm_system_id);
	else
		DRMSystemID = NULL;

	if(rights_issuer_url)
		rightsIssuerURL = HLIB_STD_StrDup(rights_issuer_url);
	else
		rightsIssuerURL = NULL;
}

DRMRightsErrorEventBroadcastEvent::~DRMRightsErrorEventBroadcastEvent()
{
	ENTRY;

	if(contentID)
		HLIB_STD_MemFree(contentID);
	if(DRMSystemID)
		HLIB_STD_MemFree(DRMSystemID);
	if(rightsIssuerURL)
		HLIB_STD_MemFree(rightsIssuerURL);

}

int DRMRightsErrorEventBroadcastEvent::getter(jsplugin_obj *obj, const char *name,
        jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("errorState", errorState, JSP_GET_VALUE);
	if(contentID)
	{
		GET_STRING("contentID", contentID, JSP_GET_VALUE);
	}
	else
	{
		GET_NULL("contentID");
	}
	if(DRMSystemID)
	{
		GET_STRING("DRMSystemID", DRMSystemID, JSP_GET_VALUE);
	}
	else
	{
		GET_NULL("DRMSystemID");
	}
	if(rightsIssuerURL)
	{
	    	GET_STRING("rightsIssuerURL", rightsIssuerURL, JSP_GET_VALUE);
	}
	else
	{
		GET_NULL("rightsIssuerURL");
	}
	return JSP_GET_NOTFOUND;
}

//virtual
int DRMRightsErrorEventBroadcastEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[4];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = errorState;
	if(contentID)
	{
		result[1].type = JSP_TYPE_STRING;
		result[1].u.string = contentID;
	}
	else
	{
		result[1].type = JSP_TYPE_NULL;
	}
	if(DRMSystemID)
	{
		result[2].type = JSP_TYPE_STRING;
		result[2].u.string = DRMSystemID;
	}
	else
	{
		result[2].type = JSP_TYPE_NULL;
	}
	if(rightsIssuerURL)
	{
		result[3].type = JSP_TYPE_STRING;
		result[3].u.string = rightsIssuerURL;
	}
	else
	{
		result[3].type = JSP_TYPE_NULL;
	}
	return 4;

}
