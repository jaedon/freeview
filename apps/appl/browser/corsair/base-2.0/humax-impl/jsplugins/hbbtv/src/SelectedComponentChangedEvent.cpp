// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)

#include "SelectedComponentChangedEvent.h"

#include <stdlib.h>
#include <string.h>


using namespace Ooif;

SelectedComponentChangedEvent::SelectedComponentChangedEvent(jsplugin_obj *c, OOIFNumber componentType) : Event(c, "SelectedComponentChanged"),
	_componentType(componentType)
{
	ENTRY;
}

SelectedComponentChangedEvent::SelectedComponentChangedEvent(jsplugin_obj *c, OOIFNumber componentType, const char *className) : Event(c, className),
	_componentType(componentType)
{

	ENTRY;
}

//virtual
SelectedComponentChangedEvent::~SelectedComponentChangedEvent()
{
	ENTRY;
}

int SelectedComponentChangedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("componentType", _componentType, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

//virtual
int SelectedComponentChangedEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = _componentType;
	return 1;
}
#endif // OIPF || HBBTV_1_2_1
