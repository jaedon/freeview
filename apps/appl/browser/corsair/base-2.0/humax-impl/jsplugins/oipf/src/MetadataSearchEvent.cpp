// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "MetadataSearchEvent.h"
#ifdef HMX_WEBUI
#include "MetadataSearch.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

MetadataSearchEvent::MetadataSearchEvent(jsplugin_obj *c, int _state, MetadataSearchHandle _search)
	: Event(c, "MetadataSearch"), state(_state), search(_search)
{
}

int MetadataSearchEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("state", state, JSP_GET_VALUE_CACHE);
#ifdef HBBTV_1_2_1
	GET_NUMBER("status", state, JSP_GET_VALUE_CACHE);
#endif

#if defined( HMX_WEBUI ) ||defined( JLABS )
	GET_OBJECT("search", obj, new MetadataSearch((MetadataSearchHandle)search), JSP_GET_VALUE);
#else
	GET_OBJECT("search", obj, new MetadataSearch(search), JSP_GET_VALUE);
#endif
	return JSP_GET_NOTFOUND;
}

//virtual
int MetadataSearchEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new MetadataSearch(search), 0);
	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = state;
	return 2;
}
#endif // OIPF || HBBTV_1_2_1
