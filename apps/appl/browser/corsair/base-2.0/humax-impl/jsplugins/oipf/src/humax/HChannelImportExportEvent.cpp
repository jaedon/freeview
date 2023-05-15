// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "HChannelImportExportEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HChannelImportExportEvent::~HChannelImportExportEvent()
{
}

HChannelImportExportEvent::HChannelImportExportEvent(
	jsplugin_obj *context, OOIFNumber _type, OOIFNumber _result)
	: Event(context,"ChannelImportExport")
{
	ENTRY;

	this->m_Type		= _type;
	this->m_Result		= _result;
}

int HChannelImportExportEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("type", m_Type, JSP_GET_VALUE_CACHE);
	GET_NUMBER("result", m_Result, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HChannelImportExportEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HChannelImportExportEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_Type;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_Result;

	return 2;
}
#endif // OIPF


