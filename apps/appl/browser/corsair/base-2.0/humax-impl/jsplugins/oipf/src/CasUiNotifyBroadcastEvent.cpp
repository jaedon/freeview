// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "CasUiNotifyBroadcastEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

CasUiNotifyBroadcastEvent::CasUiNotifyBroadcastEvent(jsplugin_obj *c, OOIFNumber _module, OOIFNumber _slot, OOIFNumber _session)
  : Event(c, "CasUiNotify"),
  module(_module),
  slot(_slot),
  session(_session)
{
	ENTRY;
}

CasUiNotifyBroadcastEvent::~CasUiNotifyBroadcastEvent()
{
	ENTRY;
}

int CasUiNotifyBroadcastEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("module", module, JSP_GET_VALUE_CACHE);
	GET_NUMBER("slot", slot, JSP_GET_VALUE_CACHE);
	GET_NUMBER("session", session, JSP_GET_VALUE_CACHE);
	
	return JSP_GET_NOTFOUND;
}

int CasUiNotifyBroadcastEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int CasUiNotifyBroadcastEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = module;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = slot;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = session;

	return 3;
}


/* EOF */

