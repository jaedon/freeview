// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "HClockRecoveryEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HClockRecoveryEvent::~HClockRecoveryEvent()
{
}

HClockRecoveryEvent::HClockRecoveryEvent
	(jsplugin_obj *context, OOIFNumber _batchType, OOIFNumber _batchProcType, OOIFNumber _err)
  : Event(context, "ClockRecovery"),m_batchType(_batchType), m_batchProcType(_batchProcType), m_err(_err)
{
	ENTRY;

	this->m_batchType		= _batchType;
	this->m_batchProcType	= _batchProcType;
	this->m_err				= _err;
}

int HClockRecoveryEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("batchType", m_batchType, JSP_GET_VALUE_CACHE);
	GET_NUMBER("batchProcType", m_batchProcType, JSP_GET_VALUE_CACHE);
	GET_NUMBER("err", m_err, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HClockRecoveryEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HClockRecoveryEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_batchType;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_batchProcType;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = m_err;

	return 3;
}
#endif // OIPF



