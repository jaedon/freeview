// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef	CONFIG_OP_FREESAT
#ifdef OIPF

#include "HMXOperatorFreesatDialAppState.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HMXOperatorFreesatDialAppState::HMXOperatorFreesatDialAppState
	(jsplugin_obj *context, OOIFNumber eType, int appState, int userData)
  : Event(context, "FreesatDialAppState")
{
	ENTRY;

	this->m_Type = eType;
	this->m_appState = appState;
	this->m_userData = userData;
}

HMXOperatorFreesatDialAppState::~HMXOperatorFreesatDialAppState()
{
	ENTRY;
}

int HMXOperatorFreesatDialAppState::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("type", m_Type, JSP_GET_VALUE_CACHE);
	GET_NUMBER("appState", m_appState, JSP_GET_VALUE_CACHE);
	GET_NUMBER("userData", m_userData, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HMXOperatorFreesatDialAppState::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HMXOperatorFreesatDialAppState::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_Type;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_appState;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = m_userData;

	return 3;
}
#endif // OIPF
#endif


