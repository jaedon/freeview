// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "HSwupUpdateEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HSwupdateUpdateEvent::~HSwupdateUpdateEvent()
{
}

HSwupdateUpdateEvent::HSwupdateUpdateEvent
	(jsplugin_obj *context, OOIFNumber _source, OOIFNumber _state, OOIFNumber _progress, OOIFNumber _progressmax, OOIFNumber _err)
  : Event(context, "UpdateProgress")
{
	ENTRY;

	this->m_Source		= _source;
	this->m_State		= _state;
	this->m_Progress	= _progress;
	this->m_Progressmax	= _progressmax;
	this->m_Err			= _err;
}

int HSwupdateUpdateEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("source", m_Source, JSP_GET_VALUE_CACHE);
	GET_NUMBER("state", m_State, JSP_GET_VALUE_CACHE);
	GET_NUMBER("progress", m_Progress, JSP_GET_VALUE_CACHE);
	GET_NUMBER("progressmax", m_Progressmax, JSP_GET_VALUE_CACHE);
	GET_NUMBER("err", m_Err, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HSwupdateUpdateEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HSwupdateUpdateEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[5];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_Source;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_State;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = m_Progress;

	result[3].type = JSP_TYPE_NUMBER;
	result[3].u.number = m_Progressmax;

	result[4].type = JSP_TYPE_NUMBER;
	result[4].u.number = m_Err;

	return 5;
}
#endif // OIPF


