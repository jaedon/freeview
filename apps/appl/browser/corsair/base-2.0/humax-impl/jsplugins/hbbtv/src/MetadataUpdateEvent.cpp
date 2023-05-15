// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "MetadataUpdateEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "Channel.h"
#include "Programme.h"

using namespace Ooif;

MetadataUpdateEvent::MetadataUpdateEvent(jsplugin_obj *c, int _action, int _info, void *object, SearchItemType type)
	: Event(c, "MetadataUpdate"), action(_action), info(_info), programme(NULL), channel(NULL),  objectType(type)
{
	switch (objectType)
	{
	case SEARCH_PROGRAMME_ITEM:
		programme = new Programme(object);
		break;
	case SEARCH_CHANNEL_ITEM:
		channel = new Channel(object);
		break;
	}
}

MetadataUpdateEvent::~MetadataUpdateEvent()
{
	delete programme;
	delete channel;
}

int MetadataUpdateEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("action", action, JSP_GET_VALUE_CACHE);
	GET_NUMBER("info", info, JSP_GET_VALUE_CACHE);
	// Now return either a Programme or a Channel ...
	if (!strcmp("object", name))
	{
		switch (objectType)
		{
		case SEARCH_PROGRAMME_ITEM:
			if (!programme) return JSP_GET_ERROR;

			RETURN_OBJECT(obj, new Programme(*programme), result, JSP_GET_VALUE);
			break;
		case SEARCH_CHANNEL_ITEM:
			if (!channel) return JSP_GET_ERROR;

			RETURN_OBJECT(obj, new Channel(*channel), result, JSP_GET_VALUE);
			break;
		default:
			OOIF_LOG_ERROR("Invalid object type");
		}
	}

	return JSP_GET_NOTFOUND;
}

//virtual
int MetadataUpdateEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[3];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = action;
	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = info;

	switch (objectType)
	{
	case SEARCH_CHANNEL_ITEM:
		if (!channel) return 0;

		SET_SERIALIZED_OBJECT(new Channel(*channel), 2);
		break;
	case SEARCH_PROGRAMME_ITEM:
		if (!programme) return 0;

		SET_SERIALIZED_OBJECT(new Programme(*programme), 2);
		break;
	default:
		OOIF_LOG_ERROR("Invalid object type");
	}

	return 3;
}
#endif // OIPF
