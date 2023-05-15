// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "TsrStateChangedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

TsrStateChangedEvent::TsrStateChangedEvent(
	jsplugin_obj *context, OOIFBoolean isPaused)
		: Event(context,"TsrStateChanged"),
		m_bIsPaused(isPaused)
{
	ENTRY;
}

TsrStateChangedEvent::~TsrStateChangedEvent()
{
	ENTRY;
}

int TsrStateChangedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_BOOLEAN("isPaused", m_bIsPaused, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int TsrStateChangedEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int TsrStateChangedEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_BOOLEAN;
	result[0].u.number = m_bIsPaused;

	return 1;
}
#endif // OIPF


