// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef	CONFIG_OP_FREESAT
#ifdef OIPF

#include "HMXOperatorFreesatMhegUpdateEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HMXOperatorFreesatMhegUpdateEvent::HMXOperatorFreesatMhegUpdateEvent
	(jsplugin_obj *context, OOIFNumber eType, const char *_BridgeUrl)
  : Event(context, "FreesatMhegEvent")
{
	ENTRY;

	this->m_Type = eType;
	this->m_MhegBridgeUrl = (_BridgeUrl !=NULL) ? HLIB_STD_StrDup(_BridgeUrl) : NULL;
}

HMXOperatorFreesatMhegUpdateEvent::~HMXOperatorFreesatMhegUpdateEvent()
{
	ENTRY;
	if(this->m_MhegBridgeUrl != NULL)
	{
		HLIB_STD_MemFree(this->m_MhegBridgeUrl);
	}
}

int HMXOperatorFreesatMhegUpdateEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("type", m_Type, JSP_GET_VALUE_CACHE);
	GET_STRING("bridgeUrl", m_MhegBridgeUrl, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HMXOperatorFreesatMhegUpdateEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HMXOperatorFreesatMhegUpdateEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_Type;

	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = m_MhegBridgeUrl;

	return 2;
}
#endif // OIPF
#endif


