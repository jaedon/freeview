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
#include "WidgetUninstallationEvent.h"
#include "macros.h"
#include "NativeApplicationManager.h"

using namespace Ooif;

WidgetUninstallationEvent::WidgetUninstallationEvent(jsplugin_obj *c, WidgetDescriptorHandle handle,
        ApplicationManagerWidgetEventState state)
	: Event(c, "WidgetUninstallation")
	, m_handle(handle)
	, m_state(state)
{
	ENTRY;
}

WidgetUninstallationEvent::~WidgetUninstallationEvent()
{
	ENTRY;
}

int WidgetUninstallationEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_OBJECT("wd", obj, new WidgetDescriptor(m_handle), JSP_GET_VALUE_CACHE);
	GET_NUMBER("state", OOIFNumber(m_state), JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int WidgetUninstallationEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new WidgetDescriptor(m_handle), 0);

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = OOIFNumber(m_state);

	return 2;
}

#endif //OIPF
