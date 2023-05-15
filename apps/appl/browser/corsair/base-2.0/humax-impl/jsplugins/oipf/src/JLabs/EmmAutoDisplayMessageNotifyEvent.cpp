// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "EmmAutoDisplayMessageNotifyEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

EmmAutoDisplayMessageNotifyEvent::EmmAutoDisplayMessageNotifyEvent(jsplugin_obj *c, jlabsEmmAutoDisplayMessageHandle id)
	: Event(c, "EmmAutoDisplayMessageNotify"), adm_info(NULL)
{
	ENTRY;
	// to guarantee a 'release' callback to integration without the
	// getter being called on the channel in case of a non-intrinsic
	// event, we need to create a channel here
	if (id != 0)
		adm_info = new jlabsEmmAutoDisplayMessage(id);
}

EmmAutoDisplayMessageNotifyEvent::~EmmAutoDisplayMessageNotifyEvent()
{
	ENTRY;
	delete adm_info;
}

int EmmAutoDisplayMessageNotifyEvent::getter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	if (strcmp(name, "ADMinfo") == 0) {
		if (adm_info) {
			RETURN_OBJECT(obj, new jlabsEmmAutoDisplayMessage(adm_info->getIdentifier()), result, JSP_GET_VALUE_CACHE);
		} else {
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
	}

	return JSP_GET_NOTFOUND;
}

int EmmAutoDisplayMessageNotifyEvent::setter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int EmmAutoDisplayMessageNotifyEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	if (adm_info) {
		SET_SERIALIZED_OBJECT( new jlabsEmmAutoDisplayMessage(adm_info->getIdentifier()), 0);
	} else {
		result[0].type = JSP_TYPE_NULL;
	}

	return 1;
}