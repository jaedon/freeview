// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ChannelList.h"
#include "NativeChannel.h"
#include "ParentalRating.h"
#include "Channel.h"
#include "Exception.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include "jsplugin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Ooif;

ChannelList::ChannelList(ChannelListType type)
{
	ENTRY;

	classType = CLASS_CHANNELLIST;

	int count = 0;
	ChannelHandle *handles = NULL;

	switch (type)
	{
	case ALL:
	case RECORDABLE:
		NATIVE_WARN_NOK(native_ChannelConfigGetChannelList(&count, &handles));
		break;
	case NONE:
		break;
	default:
		OOIF_LOG_ERROR("Unknown Channel List Type");
	}

	if (handles)
	{
		for (int i=0; i<count; i++)
		{
			addItem(new Channel(handles[i]));
		}
		HLIB_STD_MemFree(handles);
	}
}

ChannelList::ChannelList(ChannelList *original)
{
	ENTRY;

	classType = CLASS_CHANNELLIST;

	for (int i=0; i<original->getSize(); i++)
	{
		addItem(new Channel(*(original->internalGetItem(i))));
	}
}

#ifdef OIPF
ChannelList::ChannelList(ChannelList *original, OOIFBoolean has_blocked, OOIFBoolean blocked, OOIFBoolean has_favourite, OOIFBoolean favourite, OOIFBoolean has_hidden, OOIFBoolean hidden)
{
	ENTRY;

	classType = CLASS_CHANNELLIST;

	Channel *c;

	for (int i=0; i<original->getSize(); i++)
	{
		c = new Channel(*(original->internalGetItem(i)));

		if ( ( (has_blocked && (c->data_wrapper->data.manualBlock == blocked)) || !has_blocked) &&
		        ( (has_favourite && (c->data_wrapper->data.favourite == favourite)) || !has_favourite) &&
		        ( (has_hidden && (c->data_wrapper->data.hidden)) || !has_hidden) )
		{
			addItem(c);
		}
		else
		{
			delete c;
		}
	}
}

ChannelList::ChannelList(const char* bdr)
{
	ENTRY;

	classType = CLASS_CHANNELLIST;

	int count = 0;
	ChannelHandle *handles = NULL;
	NATIVE_WARN_NOK(native_ChannelCreateChannelList(bdr, &count, &handles));

	if (handles)
	{
		for (int i=0; i<count; i++)
		{
			addItem(new Channel(handles[i]));
		}
		HLIB_STD_MemFree(handles);
	}
}
#endif // OIPF

ChannelList::~ChannelList()
{
	ENTRY;
}

int ChannelList::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_BROADCAST);
#endif // HBBTV_SECURITY

	int ret = Collection<Channel>::getter(obj, name, result);
	if (ret != JSP_GET_NOTFOUND)
		return ret;

	GET_FUNCTION(getChannel, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getChannelByTriplet, obj, "n-n", JSP_GET_VALUE_CACHE);
#ifdef OIPF
	GET_FUNCTION(getChannelBySourceID, obj, "n", JSP_GET_VALUE_CACHE);
#endif // OIPF

	return JSP_GET_NOTFOUND;
}

int ChannelList::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//static
int ChannelList::getChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);

	ChannelList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, ChannelList);
	// Alright let's search through the list for this ChannelList
	for (int i = 0; i < instance->getSize(); i++)
	{
		Channel *channel = instance->internalGetItem(i);
		if (channel->data_wrapper)
		{
			if (strcmp(STRING_VALUE(argv[0]), channel->data_wrapper->data.ccid) == 0
#ifdef OIPF
			        || strcmp(STRING_VALUE(argv[0]), channel->data_wrapper->data.ipBroadcastID) == 0
#endif // OIPF
			   )
			{
				RETURN_OBJECT(this_obj, new Channel(*channel), result, JSP_CALL_VALUE);
			}
		}
	}

	RETURN_NULL(JSP_CALL_VALUE);
}

//static
int ChannelList::getChannelByTriplet(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(3);

	ChannelList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, ChannelList);
	// Alright let's search through the list for this ChannelList
	for (int i = 0; i < instance->getSize(); i++)
	{
		Channel *channel = instance->internalGetItem(i);
		if (channel->data_wrapper)
		{
			if (NUMBER_VALUE(argv[0]) == channel->data_wrapper->data.onid &&
			        (!TYPE_NUMBER(argv[1]) || NUMBER_VALUE(argv[1]) == channel->data_wrapper->data.tsid)  &&
			        NUMBER_VALUE(argv[2]) == channel->data_wrapper->data.sid)
			{
				RETURN_OBJECT(this_obj, new Channel(*channel), result, JSP_CALL_VALUE);
			}
		}
	}

	RETURN_NULL(JSP_CALL_VALUE);
}

#ifdef OIPF
//static
int ChannelList::getChannelBySourceID(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	ChannelList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, ChannelList);
	// Alright let's search through the list for this ChannelList
	for (int i = 0; i< instance->getSize(); i++)
	{
		Channel *channel = instance->internalGetItem(i);
		if (NUMBER_VALUE(argv[0]) == channel->data_wrapper->data.sourceID)
		{
			RETURN_OBJECT(this_obj, new Channel(*channel), result, JSP_CALL_VALUE);
		}
	}

	RETURN_NULL(JSP_CALL_VALUE);
}
#endif // OIPF
