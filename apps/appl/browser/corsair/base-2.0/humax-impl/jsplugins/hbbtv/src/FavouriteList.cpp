// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "FavouriteList.h"
#include "NativeParentalControlManager.h"
#include "ParentalRating.h"
#include "Channel.h"
#include "Exception.h"
#include "macros.h"

#include "jsplugin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Ooif;

void FavouriteList::init()
{
	int count;
	ChannelHandle *handles = NULL;
	NATIVE_WARN_NOK(native_FavouriteListGetChannels(getIdentifier(), &count, &handles));
	for (int i=0; i<count; i++)
	{
		addItem(new Channel(handles[i]));
	}
	HLIB_STD_MemFree(handles);
}

FavouriteList::FavouriteList(FavouriteListHandle id) : ChannelList(NONE), WrapperObject<FavouriteList>(id)
{
	ENTRY;
	init();
}

FavouriteList::FavouriteList(const FavouriteList &original) : ChannelList(NONE), WrapperObject<FavouriteList>(original.identifier)
{
	ENTRY;
	init();
}

FavouriteList::~FavouriteList()
{
	ENTRY;
}

int FavouriteList::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	int ret = ChannelList::getter(obj, name, result);
	if (ret != JSP_GET_NOTFOUND)
		return ret;

	NATIVE_GET_STRING("favID", native_FavouriteListGetFavID(getIdentifier(), &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("name", native_FavouriteListGetName(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(getChannel, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getChannelByTriplet, obj, "n-n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getChannelBySourceID, obj, "n", JSP_GET_VALUE_CACHE);

	GET_FUNCTION(insertBefore, obj, "ns", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(remove, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(commit, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

//static
int FavouriteList::getChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	FavouriteList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteList);
	ChannelHandle id = NULL;
	NATIVE_CALL(native_FavouriteListGetChannel(instance->getIdentifier(), STRING_VALUE(argv[0]), &id));
	RETURN_OBJECT(this_obj, new Channel(id), result, JSP_CALL_VALUE);
}

//static
int FavouriteList::getChannelByTriplet(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(3);
	FavouriteList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteList);
	ChannelHandle id = NULL;
	NATIVE_CALL(native_FavouriteListGetChannelByTriplet(instance->getIdentifier(),
	            NUMBER_VALUE(argv[0]),
	            TYPE_NUMBER(argv[1]),
	            NUMBER_VALUE(argv[1]),
	            NUMBER_VALUE(argv[2]),
	            &id));
	RETURN_OBJECT(this_obj, new Channel(id), result, JSP_CALL_VALUE);
}

//static
int FavouriteList::getChannelBySourceID(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	FavouriteList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteList);
	ChannelHandle id = NULL;
	NATIVE_CALL(native_FavouriteListGetChannelBySourceID(instance->getIdentifier(), NUMBER_VALUE(argv[0]), &id));
	RETURN_OBJECT(this_obj, new Channel(id), result, JSP_CALL_VALUE);
}

//static
int FavouriteList::insertBefore(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	FavouriteList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteList);

	OOIFNumber index = NUMBER_VALUE(argv[0]);
	if (index > instance->getSize() || index < 0)
		RETURN_BOOLEAN(0, JSP_CALL_VALUE);

	ChannelHandle id = NULL;
	NATIVE_WARN_NOK(native_ChannelGetByCCID(STRING_VALUE(argv[1]), &id));
	if (!id)
		RETURN_BOOLEAN(0, JSP_CALL_VALUE);

	Channel *newChannel = new Channel(id);
	instance->internalInsertAt(newChannel, index);
	RETURN_BOOLEAN(1, JSP_CALL_VALUE);
}

//static
int FavouriteList::remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(1);
    FavouriteList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteList);

    OOIFNumber index = NUMBER_VALUE(argv[0]);
    if (index<0 || index>instance->getSize()-1)
    	RETURN_BOOLEAN(0, JSP_CALL_VALUE);

    instance->internalRemoveIndex(index);
    RETURN_BOOLEAN(1, JSP_CALL_VALUE);
}

//static
int FavouriteList::commit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	FavouriteList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteList);
	FAV_LIST_COMMIT(instance);
	RETURN_BOOLEAN(1, JSP_CALL_VALUE);
}

void FavouriteList::releaseNative(FavouriteListHandle id)
{
	NATIVE_WARN_NOK(native_FavouriteListReleaseHandle(id));
}

#endif // OIPF
