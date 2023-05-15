// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "DRMSystemStatusChangeEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

DRMSystemStatusChangeEvent::~DRMSystemStatusChangeEvent()
{
	ENTRY;
	HLIB_STD_MemFree(DRMSystemID);
}

DRMSystemStatusChangeEvent::DRMSystemStatusChangeEvent(jsplugin_obj *c, const char *DRMSystemID)
	: Event(c, "DRMSystemStatusChange")
{
	this->DRMSystemID = HLIB_STD_StrDup(DRMSystemID);
}

int DRMSystemStatusChangeEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_STRING("DRMSystemID", DRMSystemID, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

//virtual
int DRMSystemStatusChangeEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = DRMSystemID;
	return 1;
}
#endif // OIPF
