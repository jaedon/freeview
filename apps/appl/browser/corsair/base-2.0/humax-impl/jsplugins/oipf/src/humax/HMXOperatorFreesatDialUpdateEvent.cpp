// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef	CONFIG_OP_FREESAT
#ifdef OIPF

#include "HMXOperatorFreesatDialUpdateEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HMXOperatorFreesatDialUpdateEvent::HMXOperatorFreesatDialUpdateEvent
	(jsplugin_obj *context, OOIFNumber eType, const char *_DialUrl,const char *_DialAppName)
  : Event(context, "FreesatDialEvent")
{
	ENTRY;

	this->m_Type = eType;
	this->m_DialUrl = (_DialUrl !=NULL) ? HLIB_STD_StrDup(_DialUrl) : NULL;
	this->m_DialAppName = (_DialAppName !=NULL) ? HLIB_STD_StrDup(_DialAppName) : NULL;

}

HMXOperatorFreesatDialUpdateEvent::~HMXOperatorFreesatDialUpdateEvent()
{
	ENTRY;
	if(this->m_DialUrl != NULL)
	{
		HLIB_STD_MemFree(this->m_DialUrl);
	}
	if(this->m_DialAppName != NULL)
	{
		HLIB_STD_MemFree(this->m_DialAppName);
	}

}

int HMXOperatorFreesatDialUpdateEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("type", m_Type, JSP_GET_VALUE_CACHE);
	GET_STRING("DialUrl", m_DialUrl, JSP_GET_VALUE_CACHE);
	GET_STRING("DialAppName", m_DialAppName, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HMXOperatorFreesatDialUpdateEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HMXOperatorFreesatDialUpdateEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_Type;

	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = m_DialUrl;

	result[2].type = JSP_TYPE_STRING;
	result[2].u.string = m_DialAppName;

	return 3;
}

#endif // OIPF
#endif


