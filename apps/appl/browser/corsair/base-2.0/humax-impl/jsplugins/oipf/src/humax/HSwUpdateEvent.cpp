// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "HSwUpdateEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HSwUpdateEvent::~HSwUpdateEvent()
{
	if (m_pstSwUpdateEvent)
		HLIB_STD_MemFree(m_pstSwUpdateEvent);
}

HSwUpdateEvent::HSwUpdateEvent(
	jsplugin_obj *context, DxSwUpdate_EventData_t *pstSwEvent)
	: Event(context,"UpdateEvent")
{
	ENTRY;
	m_pstSwUpdateEvent = (DxSwUpdate_EventData_t*)HLIB_STD_MemAlloc(sizeof(DxSwUpdate_EventData_t));
	memcpy(m_pstSwUpdateEvent, pstSwEvent, sizeof(DxSwUpdate_EventData_t));
}

int HSwUpdateEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_OBJECT("SwUpdateEventInfo", obj, new CSwUpdateInfo(m_pstSwUpdateEvent), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HSwUpdateEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HSwUpdateEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new CSwUpdateInfo(m_pstSwUpdateEvent), 0);

	return 1;
}
#endif // OIPF


