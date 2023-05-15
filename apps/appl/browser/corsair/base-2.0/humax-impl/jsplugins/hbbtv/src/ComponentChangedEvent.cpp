// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

// defined HBBTV_2.0
#if defined(OIPF) || defined(HBBTV_1_2_1)

#include "ComponentChangedEvent.h"

#include <stdlib.h>
#include <string.h>


using namespace Ooif;

ComponentChangedEvent::ComponentChangedEvent(jsplugin_obj *c, OOIFNumber componentType) : Event(c, "ComponentChanged"),
	_componentType(componentType)
{
	ENTRY;
}

//virtual
ComponentChangedEvent::~ComponentChangedEvent()
{
	ENTRY;
}

int ComponentChangedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("componentType", _componentType, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

//virtual
int ComponentChangedEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = _componentType;

	return 1;
}
#endif // OIPF || HBBTV_1_2_1
