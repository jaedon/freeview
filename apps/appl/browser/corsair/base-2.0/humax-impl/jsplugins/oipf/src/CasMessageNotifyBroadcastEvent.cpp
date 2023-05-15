// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "CasMessageNotifyBroadcastEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

CasMessageNotifyBroadcastEvent::CasMessageNotifyBroadcastEvent(jsplugin_obj *c, OOIFNumber _module, OOIFNumber _slot, OOIFNumber _session, const char *_message, OOIFNumber _message_length)
    : Event(c, "CasMessageNotify")
    , module(_module)
    , slot(_slot)
    , session(_session)
{
	ENTRY;
	if ( _message!=NULL && _message_length > 0 )
    {
        message = (char*)HLIB_STD_MemDup(_message, _message_length);
        message_length = _message_length;        
    }
    else
    {
        message = NULL;
        message_length = 0;
    }
}

CasMessageNotifyBroadcastEvent::~CasMessageNotifyBroadcastEvent()
{
	ENTRY;
	if(this->message != NULL)
	{
		HLIB_STD_MemFree(this->message);
	}
}

int CasMessageNotifyBroadcastEvent::getter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("module", module, JSP_GET_VALUE_CACHE);
	GET_NUMBER("slot", slot, JSP_GET_VALUE_CACHE);
	GET_NUMBER("session", session, JSP_GET_VALUE_CACHE);
	GET_STRING_WITH_LENGTH("message", message, message_length, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int CasMessageNotifyBroadcastEvent::setter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int CasMessageNotifyBroadcastEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[4];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = module;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = slot;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = session;

	result[3].type = JSP_TYPE_STRING_WITH_LENGTH;
	result[3].u.s.string = message;
	result[3].u.s.len = message_length;

	return 4;
}

/* EOF */

