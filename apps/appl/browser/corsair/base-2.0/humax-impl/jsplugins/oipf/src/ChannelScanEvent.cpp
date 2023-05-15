/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ChannelScanEvent.h"
#include "macros.h"
#include <string.h>

using namespace Ooif;

ChannelScanEvent::ChannelScanEvent(jsplugin_obj *c,
			OOIFNumber type, OOIFNumber progress, OOIFNumber frequency,
			OOIFNumber signalStrength, OOIFNumber channelNumber, OOIFNumber channelType,
			OOIFNumber channelCount, OOIFNumber transponderCount,
			int _infoCount, humax::SearchedInfo_Data *_searchedInfos,
			humax::ChannelTuned_Data	*tunedInfo, const char * event) :
			Event(c, event)
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

	infoCount = 0;
	searchedInfos = NULL;
	if (_infoCount > 0)
	{
		infoCount = _infoCount;
		searchedInfos = (humax::SearchedInfo_Data*)HLIB_STD_MemAlloc(sizeof(humax::SearchedInfo_Data) * _infoCount);
		memcpy(searchedInfos, _searchedInfos, _infoCount * sizeof(humax::SearchedInfo_Data));
	}
	tunedInfoData = NULL;
	if (NULL != tunedInfo)
	{
		tunedInfoData = (humax::ChannelTuned_Data*)HLIB_STD_MemAlloc(sizeof(humax::ChannelTuned_Data));
		memcpy(tunedInfoData, tunedInfo, sizeof(humax::ChannelTuned_Data));
	}
}

ChannelScanEvent::~ChannelScanEvent()
{
	ENTRY;

	if (searchedInfos)
		HLIB_STD_MemFree(searchedInfos);
	if (tunedInfoData)
		HLIB_STD_MemFree(tunedInfoData);
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

#if !defined(JLABS)
	GET_OBJECT("searchedInfo", obj, new CSearchedInfo(infoCount, searchedInfos), JSP_GET_VALUE);
	GET_OBJECT("tunedInfo", obj, new ChannelScanParameters(tunedInfoData), JSP_GET_VALUE);
#endif

	return JSP_GET_NOTFOUND;
}

int ChannelScanEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

#if !defined(JLABS)
	#define NUM_OF_CHANNELSCANEVENTPARAM	10
#else
	#define NUM_OF_CHANNELSCANEVENTPARAM	8
#endif

int ChannelScanEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[NUM_OF_CHANNELSCANEVENTPARAM];

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

#if !defined(JLABS)
	SET_SERIALIZED_OBJECT(new CSearchedInfo(infoCount, searchedInfos), 8);
	SET_SERIALIZED_OBJECT(new ChannelScanParameters(tunedInfoData), 9);
#endif

	return NUM_OF_CHANNELSCANEVENTPARAM;
}

