// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "RequestMhegTuneEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

MhegTuneEvent::MhegTuneEvent(
	jsplugin_obj *context, ChannelHandle channel, OOIFNumber ulSvcUid, OOIFNumber ulTuneMode)
		: Event(context,"RequestMhegTune"),
		m_channel(NULL),
		m_ulSvcUid(ulSvcUid),
		m_ulTuneMode(ulTuneMode)
{
	ENTRY;
	m_channel = new Channel(channel);
}

MhegTuneEvent::~MhegTuneEvent()
{
}


int MhegTuneEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	if (strcmp(name, "m_channel") == 0)
	{
		if (m_channel)
		{
			RETURN_OBJECT(obj, new Channel(m_channel->getIdentifier()), result, JSP_GET_VALUE_CACHE);
		}
		else
		{
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
	}
	GET_NUMBER("m_ulSvcUid", m_ulSvcUid, JSP_GET_VALUE_CACHE);
	GET_NUMBER("m_ulTuneMode", m_ulTuneMode, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int MhegTuneEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int MhegTuneEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;

	if (m_channel) {
		SET_SERIALIZED_OBJECT( new Channel(m_channel->getIdentifier()), 0);
	} else {
		result[0].type = JSP_TYPE_NULL;
	}

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_ulSvcUid;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = m_ulTuneMode;

	return 3;
}
#endif // OIPF
