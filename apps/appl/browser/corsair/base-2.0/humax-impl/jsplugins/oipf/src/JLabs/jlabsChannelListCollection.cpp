// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined OIPF && defined JLABS
#include "jlabsChannelListCollection.h"
#include "NativeJlabsChannel.h"
#include "jlabsDTTChannelList.h"
#include "jlabsBSCATVChannelList.h"

#include "Exception.h"
#include "macros.h"

#include "jsplugin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Ooif;

jlabsChannelListCollection::jlabsChannelListCollection(int networkType)
{
	ENTRY;
	classType = CLASS_JLABS_CHANNEL_LIST_COLLECTION;
	networkTypeName = networkType;
}

jlabsChannelListCollection::jlabsChannelListCollection()
{
	ENTRY;
	classType = CLASS_JLABS_CHANNEL_LIST_COLLECTION;
}

jlabsChannelListCollection::~jlabsChannelListCollection()
{
	ENTRY;
}

int jlabsChannelListCollection::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	int ret = Collection<ChannelList>::getter(obj, name, result);
	if (ret != JSP_GET_NOTFOUND)
		return ret;

	GET_NUMBER("JLABS_DTT_NETWORK", JLABS_DTT_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_BS_NETWORK", JLABS_BS_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_CATV_NETWORK", JLABS_CATV_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_ALL_NETWORK", JLABS_ALL_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_CS1_NETWORK", JLABS_CS1_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_CS2_NETWORK", JLABS_CS2_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_JCHITS_NETWORK", JLABS_JCHITS_NETWORK, JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("networkTypeName", native_jlabsChannelListCollectionGetNetworkTypeName((jlabsChannelListNetwork)networkTypeName, &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("lastUpdateDate",native_jlabsChannelListCollectionGetLastUpdateDate((jlabsChannelListNetwork)networkTypeName, &RETVAL),JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int jlabsChannelListCollection::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int jlabsChannelListCollection::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	ENTRY;

#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif
	jlabsChannelListCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsChannelListCollection);
	ChannelList *channelList =  instance->internalGetItem(index);

//	ObjectInstance *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, ObjectInstance);

	if (channelList->getType() == CLASS_JLABS_DTT_CHANNEL_LIST)
	{
		RETURN_OBJECT(this_obj, new jlabsDTTChannelList((jlabsDTTChannelList*)(this->internalGetItem(index))), result, return_type);
	}
	else if(channelList->getType() == CLASS_JLABS_BS_CATV_CHANNEL_LIST)
	{
		RETURN_OBJECT(this_obj, new jlabsBSCATVChannelList((jlabsBSCATVChannelList*)(this->internalGetItem(index))), result, return_type);
	}
	else
	{
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}
}
#endif /* OIPF && JLABS */
