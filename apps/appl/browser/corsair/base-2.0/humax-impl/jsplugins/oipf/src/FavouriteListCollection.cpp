// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "FavouriteListCollection.h"
#include "NativeChannel.h"
#include "FavouriteList.h"
#include "Exception.h"
#include "Channel.h"

#include "jsplugin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Ooif;

FavouriteListCollection::FavouriteListCollection()
{
	ENTRY;
	classType = CLASS_FAVOURITELISTCOLLECTION;
	// There is really only one favourite list collection, global to
	// the entire terminal let's initialize the data here
	int count;
	FavouriteListHandle *handles = NULL;
	NATIVE_WARN_NOK(native_ChannelConfigGetFavouriteLists(&count, &handles));
	for (int i=0; i<count; i++)
	{
		addItem(new FavouriteList(handles[i]));
	}
	HLIB_STD_MemFree(handles);
}

FavouriteListCollection::~FavouriteListCollection()
{
	ENTRY;
}

int FavouriteListCollection::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	int ret = Collection<FavouriteList>::getter(obj, name, result);
	if (ret != JSP_GET_NOTFOUND)
		return ret;

	GET_FUNCTION(remove, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(commit, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getFavouriteList, obj, "s", JSP_GET_VALUE_CACHE);
#if defined(HMX_WEBUI)
	GET_FUNCTION(createFavouriteList, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(activateFavouriteList, obj, "s", JSP_GET_VALUE_CACHE);
#else
	GET_FUNCTION(createFavouriteList, obj, "", JSP_GET_VALUE_CACHE);
#endif

	return JSP_GET_NOTFOUND;
}

//static
int FavouriteListCollection::remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(1);

    FavouriteListCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteListCollection);

    OOIFNumber index = NUMBER_VALUE(argv[0]);
    if (index<0 || index>instance->getSize()-1)
    	RETURN_BOOLEAN(0, JSP_CALL_VALUE);
#if defined(HMX_WEBUI)
    FavouriteList *fl = instance->internalGetItem(index);
    OOIFBoolean	retval;
    NATIVE_WARN_NOK(native_FavouriteListRemove(fl->getIdentifier(), &retval));

    instance->internalRemoveIndex(index);
    RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
#else // opera original
    instance->internalRemoveIndex(index);
    RETURN_BOOLEAN(1, JSP_CALL_VALUE);
#endif
}

//static
int FavouriteListCollection::commit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	FavouriteListCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteListCollection);
	for ( int j = 0; j< instance->getSize(); j++)
	{
		FAV_LIST_COMMIT(instance->internalGetItem(j));
	}
	RETURN_BOOLEAN(1, JSP_CALL_VALUE);
}

//static
int FavouriteListCollection::getFavouriteList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

#if defined(HMX_WEBUI)
	FavouriteListHandle handle;
	NATIVE_CALL(native_FavouriteListGetFavouriteList(STRING_VALUE(argv[0]), &handle));
	RETURN_OBJECT(this_obj, new FavouriteList(handle), result, JSP_CALL_VALUE);
#else // opera original
	FavouriteListCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteListCollection);
	for (int i = 0; i < instance->getSize(); i++)
	{
		FavouriteList *fl = instance->internalGetItem(i);
		const char *favid;
		NATIVE_WARN_NOK(native_FavouriteListGetFavID(fl->getIdentifier(), &favid));
		if (!strcmp(favid, STRING_VALUE(argv[0])))
		{
			// FIXME: This FavouriteList object can be deleted or
			// unloaded, so this may crash. We probably need to
			// convert this into a pure wrapper class in order to be
			// safe, ie not keep the channels stored in the class, not
			// inherit from ChannelList or even collection at
			// all. Returning new FavouriteList(fl) will prevent the
			// crash, but calling commmit() on the
			// FavouriteListCollection would not work.
			RETURN_OBJECT(this_obj, new FavouriteList(fl->getIdentifier()), result, JSP_CALL_VALUE);
		}
	}

	RETURN_NULL(JSP_CALL_VALUE);
#endif
}

//static
int FavouriteListCollection::createFavouriteList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
#if defined(HMX_WEBUI)
	CHECK_ARGUMENT_COUNT(1);
	FavouriteListCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteListCollection);

	FavouriteListHandle handle;
	NATIVE_CALL(native_FavouriteListCreate(STRING_VALUE(argv[0]), NULL, &handle));
	FavouriteList *fl= new FavouriteList(handle);
	instance->addItem(fl);

	RETURN_OBJECT(this_obj, new FavouriteList(*fl), result, JSP_CALL_VALUE);
#else // opera original
	CHECK_ARGUMENT_COUNT(0);
	FavouriteListCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, FavouriteListCollection);

	FavouriteListHandle handle;
	NATIVE_CALL(native_FavouriteListCreate(&handle));
	instance->addItem(new FavouriteList(handle));

	const char *favid;
	NATIVE_WARN_NOK(native_FavouriteListGetFavID(handle, &favid));
	RETURN_STRING(favid, JSP_CALL_VALUE);
#endif // #if defined(HMX_WEBUI)
}
#endif // OIPF

#if defined(HMX_WEBUI)
int FavouriteListCollection::activateFavouriteList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	//	OIPF Spec Description :
	//	Active the favourite list from the collection. This method SHALL return true if the
	//	operation succeeded, or false if an invalid index was specified. A newly created
	//	favourite list has to be committed before it can be activated.

	//	We don't need function : all favourite list is activated

	OOIFBoolean	retval;

	NATIVE_CALL(native_FavouriteListActivateFavouriteList(STRING_VALUE(argv[0]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
#endif
