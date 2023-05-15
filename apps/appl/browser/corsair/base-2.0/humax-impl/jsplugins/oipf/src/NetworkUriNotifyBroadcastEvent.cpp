// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "NetworkUriNotifyBroadcastEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

NetworkUriNotifyBroadcastEvent::NetworkUriNotifyBroadcastEvent(
	jsplugin_obj *context,const char * pszUri)
		: Event(context,"NetworkUriChanged")
{
	ENTRY;

	m_pNetworkUri = (pszUri!=NULL) ? HLIB_STD_StrDup(pszUri) : NULL;
}

NetworkUriNotifyBroadcastEvent::~NetworkUriNotifyBroadcastEvent()
{
	if(m_pNetworkUri != NULL)
	{
		HLIB_STD_MemFree(m_pNetworkUri);
	}
}

int NetworkUriNotifyBroadcastEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_STRING("m_pNetworkUri", m_pNetworkUri, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int NetworkUriNotifyBroadcastEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int NetworkUriNotifyBroadcastEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = m_pNetworkUri;


	return 1;
}
#endif // OIPF


