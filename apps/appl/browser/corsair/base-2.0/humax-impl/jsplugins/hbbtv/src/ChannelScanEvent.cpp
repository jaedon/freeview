/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "ChannelScanEvent.h"
#include "macros.h"
#include <string.h>

using namespace Ooif;

ChannelScanEvent::ChannelScanEvent(jsplugin_obj *c, OOIFNumber type, OOIFNumber progress, OOIFNumber frequency, OOIFNumber signalStrength,
                                   OOIFNumber channelNumber, OOIFNumber channelType, OOIFNumber channelCount, OOIFNumber transponderCount) : Event(c, "ChannelScan")
	, m_type(type)
	, m_progress(progress)
	, m_frequency(frequency)
	, m_signalStrength(signalStrength)
	, m_channelNumber(channelNumber)
	, m_channelType(channelType)
	, m_channelCount(channelCount)
	, m_transponderCount(transponderCount)
{
	ENTRY;
}

ChannelScanEvent::~ChannelScanEvent()
{
	ENTRY;
}

int ChannelScanEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("type", m_type, JSP_GET_VALUE_CACHE);
	GET_NUMBER("progress", m_progress, JSP_GET_VALUE_CACHE);
	GET_NUMBER("frequency", m_frequency, JSP_GET_VALUE_CACHE);
	GET_NUMBER("signalStrength", m_signalStrength, JSP_GET_VALUE_CACHE);
	GET_NUMBER("channelNumber", m_channelNumber, JSP_GET_VALUE_CACHE);
	GET_NUMBER("channelType", m_channelType, JSP_GET_VALUE_CACHE);
	GET_NUMBER("channelCount", m_channelCount, JSP_GET_VALUE_CACHE);
	GET_NUMBER("transponderCount", m_transponderCount, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int ChannelScanEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int ChannelScanEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[8];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_type;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number =m_progress;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = m_frequency;

	result[3].type = JSP_TYPE_NUMBER;
	result[3].u.number = m_signalStrength;

	result[4].type = JSP_TYPE_NUMBER;
	result[4].u.number = m_channelNumber;

	result[5].type = JSP_TYPE_NUMBER;
	result[5].u.number = m_channelType;

	result[6].type = JSP_TYPE_NUMBER;
	result[6].u.number = m_channelCount;

	result[7].type = JSP_TYPE_NUMBER;
	result[7].u.number = m_transponderCount;

	return 8;
}
#endif // OIPF
