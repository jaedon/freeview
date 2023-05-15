// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "NCDNotifyBroadcastEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

NCDNotifyBroadcastEvent::NCDNotifyBroadcastEvent(
	jsplugin_obj *context, OOIFNumber ulStartTimeOfChange, OOIFNumber ulASODuration,const char * pszMessage)
		: Event(context,"NCDChanged"),
		m_ulStartTimeOfChange(ulStartTimeOfChange),
		m_ulASODuration(ulASODuration)
{
	ENTRY;
	OOIF_LOG_PRINT("[NCDNotifyBroadcastEvent] pszMessage : %s\n",pszMessage);
	m_pNcdMessage = (pszMessage!=NULL) ? HLIB_STD_StrDup(pszMessage) : NULL;
}

NCDNotifyBroadcastEvent::~NCDNotifyBroadcastEvent()
{
	if(m_pNcdMessage != NULL)
	{
		HLIB_STD_MemFree(m_pNcdMessage);
	}
}

int NCDNotifyBroadcastEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("m_ulStartTimeOfChange", m_ulStartTimeOfChange, JSP_GET_VALUE_CACHE);
	GET_NUMBER("m_ulASODuration", m_ulASODuration, JSP_GET_VALUE_CACHE);
	GET_STRING("m_pNcdMessage", m_pNcdMessage, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int NCDNotifyBroadcastEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int NCDNotifyBroadcastEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_ulStartTimeOfChange;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_ulASODuration;

	result[2].type = JSP_TYPE_STRING;
	result[2].u.string = m_pNcdMessage;

	
	return 3;
}
#endif // OIPF


