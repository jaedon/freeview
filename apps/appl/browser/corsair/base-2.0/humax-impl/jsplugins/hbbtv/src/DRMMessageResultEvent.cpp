// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "DRMMessageResultEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

DRMMessageResultEvent::~DRMMessageResultEvent()
{
	ENTRY;
	HLIB_STD_MemFree(msgID);
	HLIB_STD_MemFree(resultMsg);
}

DRMMessageResultEvent::DRMMessageResultEvent(jsplugin_obj *c, const char *msgID, const char *resultMsg, int resultCode)
	: Event(c, "DRMMessageResult")
{
	this->msgID = HLIB_STD_StrDup(msgID);
	this->resultMsg = HLIB_STD_StrDup(resultMsg);
	this->resultCode = resultCode;
}

int DRMMessageResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_STRING("msgID", msgID, JSP_GET_VALUE_CACHE);
	GET_STRING("resultMsg", resultMsg, JSP_GET_VALUE_CACHE);
	GET_NUMBER("resultCode", resultCode, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

//virtual
int DRMMessageResultEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[3];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = msgID;
	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = resultMsg;
	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = resultCode;
	return 3;
}
