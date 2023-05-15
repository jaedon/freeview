// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Channel.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "Exception.h"
#include "macros.h"

using namespace Ooif;
void ChannelDataWrapper::init(void *id)
{
	memset(&data, 0, sizeof(ChannelData));
	data.dsd_len = -1;
	NATIVE_WARN_NOK(native_ChannelInitialize(id, &data));

	if (data.dsd)
	{
		if (data.dsd_len < 0)
		{
			// Backwards-compatibility: assume null-terminated string
			data.dsd_len = strlen((const char*)data.dsd);
		}

		// wrap the dsd (which is raw data) into a utf-8 string
		int dsd_utf8_len = OOIFUtils::getLengthOfBinaryDataInUTF8(data.dsd, data.dsd_len);
		char* dsd_utf8 = (char*)HLIB_STD_MemAlloc(dsd_utf8_len + 1);
		if (dsd_utf8)
		{
			OOIFUtils::wrapBinaryDataInUTF8(dsd_utf8, dsd_utf8_len, data.dsd, data.dsd_len);

			// null terminate to be on the safe side
			dsd_utf8[dsd_utf8_len] = 0;
		}

		// keep the utf-8 string instead of the raw data
		DATA_FREE_STRING(dsd);
		data.dsd = (unsigned char*)dsd_utf8;
		data.dsd_len = dsd_utf8 ? dsd_utf8_len : 0;
	}
}

ChannelDataWrapper::~ChannelDataWrapper()
{
	DATA_FREE_STRING(ccid);
#ifdef OIPF
	DATA_FREE_STRING(tunerID);
#endif /* OIPF */
	DATA_FREE_STRING(name);
	DATA_FREE_STRING(dsd);
#ifdef OIPF
	DATA_FREE_STRING(ipBroadcastID);
	DATA_FREE_STRING(longName);
	DATA_FREE_STRING(description);
	DATA_FREE_STRING(logoURL);
#endif /* OIPF */
}

Channel::~Channel()
{
	ENTRY;
}

void Channel::init()
{
	classType = CLASS_CHANNEL;
}

Channel::Channel(ChannelHandle i) : DataObject<Channel,ChannelDataWrapper>(i)
{
	init();
}

Channel::Channel(const Channel &original) : DataObject<Channel,ChannelDataWrapper>(original)
{
	init();
}

int Channel::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_BROADCAST);
#endif // HBBTV_SECURITY

	GET_NUMBER("TYPE_TV", CHANNEL_TYPE_TV, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TYPE_RADIO", CHANNEL_TYPE_RADIO, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TYPE_OTHER", CHANNEL_TYPE_OTHER, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_ANALOG", CHANNEL_ID_ANALOG, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_DVB_C", CHANNEL_ID_DVB_C, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_DVB_S", CHANNEL_ID_DVB_S, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_DVB_T", CHANNEL_ID_DVB_T, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_DVB_SI_DIRECT", CHANNEL_ID_DVB_SI_DIRECT, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_DVB_C2", CHANNEL_ID_DVB_C2, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_DVB_S2", CHANNEL_ID_DVB_S2, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_DVB_T2", CHANNEL_ID_DVB_T2, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_ISDB_C", CHANNEL_ID_ISDB_C, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_ISDB_S", CHANNEL_ID_ISDB_S, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_ISDB_T", CHANNEL_ID_ISDB_T, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_ATSC_T", CHANNEL_ID_ATSC_T, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_IPTV_SDS", CHANNEL_ID_IPTV_SDS, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_IPTV_URI", CHANNEL_ID_IPTV_URI, JSP_GET_VALUE_CACHE);

	DATA_GET_NUMBER(channelType);
	DATA_GET_STRING(ccid);
	// Manually fetch the dsd member, since it requires casting.
	//DATA_GET_STRING_WITH_LENGTH(dsd, dsd_len);
	if (!data_wrapper)
	{
		GET_NULL("dsd");
	}
	else
	{
		GET_STRING_WITH_LENGTH("dsd", (const char*)data_wrapper->data.dsd, data_wrapper->data.dsd_len, JSP_GET_VALUE_CACHE);
	}
	DATA_GET_NUMBER_OR_UNDEFINED(onid);
	DATA_GET_NUMBER(tsid);
	DATA_GET_NUMBER(sid);
#ifdef HBBTV_1_2_1
	DATA_GET_STRING_OR_UNDEFINED(name);
	DATA_GET_NUMBER_OR_UNDEFINED(nid);
#else
	DATA_GET_STRING(name);
#endif // HBBTV_1_2_1

#ifdef OIPF
	DATA_GET_NUMBER(idType);
	DATA_GET_NUMBER(sourceID);
	DATA_GET_NUMBER(freq);
	DATA_GET_NUMBER(cni);
	DATA_GET_NUMBER(majorChannel);
	DATA_GET_NUMBER(minorChannel);
	DATA_GET_NUMBER(channelMaxBitRate);
	DATA_GET_NUMBER(channelTTR);

	DATA_GET_BOOLEAN(favourite);
	DATA_GET_BOOLEAN(locked);
	DATA_GET_BOOLEAN(manualBlock);
	DATA_GET_BOOLEAN(authorised);
	DATA_GET_BOOLEAN(hidden);

	DATA_GET_STRING(tunerID);
	DATA_GET_STRING(ipBroadcastID);
	DATA_GET_STRING(longName);
	DATA_GET_STRING(description);
	DATA_GET_STRING(logoURL);

	NATIVE_GET_STRING_COLLECTION("favIDs", obj,
	                             native_ChannelGetFavIDs(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("genre", obj,
	                             native_ChannelGetGenre(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);

	GET_FUNCTION(getField, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getLogo, obj, "nn", JSP_GET_VALUE_CACHE);
#endif // OIPF

	return JSP_GET_NOTFOUND;
}

int Channel::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

#ifdef OIPF
//static
int Channel::getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                      int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Channel *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Channel);

	const char *retval;
	NATIVE_CALL(native_ChannelGetField(instance->getIdentifier(), STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int Channel::getLogo(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                     int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	Channel *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Channel);

	const char *retval;
	NATIVE_CALL(native_ChannelGetLogo(instance->getIdentifier(), NUMBER_VALUE(argv[0]),
	                                  NUMBER_VALUE(argv[1]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}
#endif // OIPF

void Channel::releaseNative(ChannelHandle id)
{
	NATIVE_WARN_NOK(native_ChannelReleaseHandle(id));
}
