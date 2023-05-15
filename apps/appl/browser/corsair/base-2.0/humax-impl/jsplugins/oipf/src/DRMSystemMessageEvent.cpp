// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "DRMSystemMessageEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

DRMSystemMessageEvent::~DRMSystemMessageEvent()
{
	ENTRY;
	HLIB_STD_MemFree(msg);
	HLIB_STD_MemFree(DRMSystemID);
}

DRMSystemMessageEvent::DRMSystemMessageEvent(jsplugin_obj *c, const char *msg, const char *DRMSystemID)
	: Event(c, "DRMSystemMessage")
{
	this->msg = HLIB_STD_StrDup(msg);
	this->DRMSystemID = HLIB_STD_StrDup(DRMSystemID);
}

int DRMSystemMessageEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_STRING("msg", msg, JSP_GET_VALUE_CACHE);
	GET_STRING("DRMSystemID", DRMSystemID, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

//virtual
int DRMSystemMessageEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = msg;
	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = DRMSystemID;
	return 2;
}
#endif // OIPF || HBBTV_1_2_1
