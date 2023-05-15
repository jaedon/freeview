// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "HCasActionIrEmmForcedOtaEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HCasActionIrEmmForcedOtaEvent::~HCasActionIrEmmForcedOtaEvent()
{
}

HCasActionIrEmmForcedOtaEvent::HCasActionIrEmmForcedOtaEvent(
	jsplugin_obj *context, OOIFNumber __param1, OOIFNumber __param2, OOIFNumber __param3)
	: Event(context,"CasActionIrEmmForcedOta")
{
	ENTRY;
	this->m_param1		= __param1;
	this->m_param2		= __param2;
	this->m_param3		= __param3;
}

int HCasActionIrEmmForcedOtaEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("param1", m_param1, JSP_GET_VALUE_CACHE);
	GET_NUMBER("param2", m_param2, JSP_GET_VALUE_CACHE);
	GET_NUMBER("param3", m_param3, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HCasActionIrEmmForcedOtaEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HCasActionIrEmmForcedOtaEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_param1;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_param2;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = m_param3;

	return 3;
}
#endif // OIPF


