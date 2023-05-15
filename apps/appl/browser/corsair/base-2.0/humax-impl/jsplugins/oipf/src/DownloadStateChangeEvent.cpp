// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)

#include "DownloadStateChangeEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

DownloadStateChangeEvent::~DownloadStateChangeEvent()
{
}

DownloadStateChangeEvent::DownloadStateChangeEvent(jsplugin_obj *context, DownloadHandle item,
        OOIFNumber state, OOIFBoolean has_reason, OOIFNumber reason)
	: Event(context, "DownloadStateChange"), download(item)
{
	this->state = state;
	this->has_reason = has_reason;
	if (this->has_reason)
		this->reason = reason;
}

int DownloadStateChangeEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_OBJECT("item", obj, new Download(download), JSP_GET_VALUE_CACHE);
	GET_NUMBER("state", state, JSP_GET_VALUE_CACHE);
	if (has_reason)
	{
		GET_NUMBER("reason", reason, JSP_GET_VALUE_CACHE);
	}
	else
	{
		GET_UNDEFINED("reason");
	}

	return JSP_GET_NOTFOUND;
}

int DownloadStateChangeEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int DownloadStateChangeEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new Download(download), 0);

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = state;
	if (has_reason)
	{
		result[2].type = JSP_TYPE_NUMBER;
		result[2].u.number = reason;
	}
	else
	{
		result[2].type = JSP_TYPE_UNDEFINED;
	}
	return 3;
}
#endif // HBBTV_DOWNLOAD || OIPF
