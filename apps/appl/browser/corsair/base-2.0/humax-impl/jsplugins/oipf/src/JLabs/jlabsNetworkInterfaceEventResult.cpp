// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "jlabsNetworkInterfaceEventResult.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

jlabsNetworkInterfaceEventResult::~jlabsNetworkInterfaceEventResult()
{
	ENTRY;
}

jlabsNetworkInterfaceEventResult::jlabsNetworkInterfaceEventResult(jsplugin_obj *c, const char* name)
  : Event(c, "LinkStatusChanged"), m_EvnetType(name)
{
}

int jlabsNetworkInterfaceEventResult::getter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsNetworkInterfaceEventResult::setter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int jlabsNetworkInterfaceEventResult::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = m_EvnetType;

	return 1;
}
