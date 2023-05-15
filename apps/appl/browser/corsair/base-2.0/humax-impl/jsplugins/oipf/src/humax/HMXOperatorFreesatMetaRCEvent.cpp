// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef	CONFIG_OP_FREESAT

#ifdef OIPF

#include "HMXOperatorFreesatMetaRCEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HMXOperatorFreesatRCEvent::HMXOperatorFreesatRCEvent
	(jsplugin_obj *context, OOIFNumber eType, const char *_rcHeader,const char *_rcBody)
  : Event(context, "FreesatRCEvent")
{
	ENTRY;

	this->m_Type = eType;
	this->m_RCHeader = (_rcHeader !=NULL) ? HLIB_STD_StrDup(_rcHeader) : NULL;
	this->m_RCBody = (_rcBody !=NULL) ? HLIB_STD_StrDup(_rcBody) : NULL;
}

HMXOperatorFreesatRCEvent::~HMXOperatorFreesatRCEvent()
{
	ENTRY;
	if(this->m_RCHeader != NULL)
	{
		HLIB_STD_MemFree(this->m_RCHeader);
	}
	if(this->m_RCBody != NULL)
	{
		HLIB_STD_MemFree(this->m_RCBody);
	}

}

int HMXOperatorFreesatRCEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("type", m_Type, JSP_GET_VALUE_CACHE);
	GET_STRING("rc_header", m_RCHeader, JSP_GET_VALUE_CACHE);
	GET_STRING("rc_body", m_RCBody, JSP_GET_VALUE_CACHE);



	return JSP_GET_NOTFOUND;
}

int HMXOperatorFreesatRCEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HMXOperatorFreesatRCEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_Type;

	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = m_RCHeader;
	
	result[2].type = JSP_TYPE_STRING;
	result[2].u.string = m_RCBody;

	return 3;
}
#endif // OIPF
#endif


