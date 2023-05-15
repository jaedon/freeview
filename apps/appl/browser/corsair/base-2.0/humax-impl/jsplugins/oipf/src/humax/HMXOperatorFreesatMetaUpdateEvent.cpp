// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef	CONFIG_OP_FREESAT

#ifdef OIPF

#include "HMXOperatorFreesatMetaUpdateEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HMXOperatorFreesatMetaUpdateEvent::HMXOperatorFreesatMetaUpdateEvent
	(jsplugin_obj *context, OOIFNumber eType, const char *_metaName)
  : Event(context, "FreesatMetaFileUpdate")
{
	ENTRY;

	this->m_Type = eType;
	this->m_MetaName = (_metaName !=NULL) ? HLIB_STD_StrDup(_metaName) : NULL;
}

HMXOperatorFreesatMetaUpdateEvent::~HMXOperatorFreesatMetaUpdateEvent()
{
	ENTRY;
	if(this->m_MetaName != NULL)
	{
		HLIB_STD_MemFree(this->m_MetaName);
	}
}

int HMXOperatorFreesatMetaUpdateEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("type", m_Type, JSP_GET_VALUE_CACHE);
	GET_STRING("metaname", m_MetaName, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HMXOperatorFreesatMetaUpdateEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HMXOperatorFreesatMetaUpdateEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_Type;

	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = m_MetaName;

	return 2;
}
#endif // OIPF
#endif


