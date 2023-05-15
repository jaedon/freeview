// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF

#include <string.h>

#include "WidgetDescriptor.h"
#include "WidgetInstallationEvent.h"
#include "macros.h"
#include "NativeApplicationManager.h"

using namespace Ooif;

WidgetInstallationEvent::WidgetInstallationEvent(jsplugin_obj *c, WidgetDescriptorHandle handle,
        ApplicationManagerWidgetEventState state, ApplicationManagerWidgetEventStateReason reason)
	: Event(c, "WidgetInstallation")
	, m_handle(handle)
	, m_state(state)
	, m_reason(reason)
{
	ENTRY;
}

WidgetInstallationEvent::~WidgetInstallationEvent()
{
	ENTRY;
}

int WidgetInstallationEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_OBJECT("wd", obj, new WidgetDescriptor(m_handle), JSP_GET_VALUE_CACHE);
	GET_NUMBER("state", OOIFNumber(m_state), JSP_GET_VALUE_CACHE);
	if(m_state == WIDGET_INSTALLATION_FAILED)
	{
		GET_NUMBER("reason", OOIFNumber(m_reason), JSP_GET_VALUE_CACHE);
	}
	else
	{
		GET_NULL("reason");
	}
	return JSP_GET_NOTFOUND;
}

int WidgetInstallationEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[3];
	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new WidgetDescriptor(m_handle), 0);

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = OOIFNumber(m_state);

	if(m_state == WIDGET_INSTALLATION_FAILED)
	{
		result[2].type = JSP_TYPE_NUMBER;
		result[2].u.number = OOIFNumber(m_reason);
	}
	else
	{
		result[2].type = JSP_TYPE_NULL;
	}

	return 3;
}

#endif //OIPF
