// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "SearchResults.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

#include "Exception.h"
#include "Query.h"
#include "Channel.h"
#include "ChannelList.h"
#include "ParentalRatingScheme.h"
#include "Programme.h"

#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

using namespace Ooif;

SearchResults::SearchResults(SearchResultsHandle i) : WrapperObject<SearchResults>(i)
{
	classType = CLASS_SEARCHRESULT;
}

SearchResults::~SearchResults()
{
	ENTRY;
	// Let's assume SearchResultsHandle is automatically collected (eg an int)
}

int SearchResults::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("length", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("offset", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("totalSize", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("item", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("getResults", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("abort", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_GET_NUMBER("length", native_SearchResultsGetLength(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("offset", native_SearchResultsGetOffset(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("totalSize", native_SearchResultsGetTotalSize(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(item, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getResults, obj, "nn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(abort, obj, "", JSP_GET_VALUE_CACHE);
#ifdef OIPF
	GET_FUNCTION(update, obj, "", JSP_GET_VALUE_CACHE);
#endif // OIPF

	// Array indexing (eg myCollection[0])
	int index = atoi(name);
	if (index > 0 || name[0] == '0')
	{
		return getItem(obj, index, result);
	}

	return JSP_GET_NOTFOUND;
}

int SearchResults::getItem(jsplugin_obj* obj, int index, jsplugin_value* result)
{
	//check if we are not exceeding array's bound
	OOIFNumber length;
	NATIVE_GET(native_SearchResultsGetLength(getIdentifier(), &length));
	if (index >= (int)length)
		return JSP_GET_NOTFOUND;

	//get a value from desired index
	SearchItemType type;
	void* res;
	NATIVE_GET(native_SearchResultsItem(getIdentifier(), index, &type, &res));

	//return the value to EcmaScript
	switch (type)
	{
	case SEARCH_PROGRAMME_ITEM:
	{
		RETURN_OBJECT(obj, new Programme(res), result, JSP_GET_VALUE);
		break;
	}
	case SEARCH_CHANNEL_ITEM:
	{
		RETURN_OBJECT(obj, new Channel(res), result, JSP_GET_VALUE);
		break;
	}
	default:
		return JSP_GET_ERROR;
	}
}

//static
int SearchResults::item(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                        int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);

	SearchResults *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, SearchResults);
	SearchItemType type;
	void *res;
	NATIVE_CALL(native_SearchResultsItem(instance->getIdentifier(),
	                                     NUMBER_VALUE(argv[0]), &type, &res));
	switch (type)
	{
	case SEARCH_PROGRAMME_ITEM:
		RETURN_OBJECT(this_obj, new Programme(res), result, JSP_CALL_VALUE);
		break;
	case SEARCH_CHANNEL_ITEM:
		RETURN_OBJECT(this_obj, new Channel(res), result, JSP_CALL_VALUE);
		break;
	default:
		return JSP_CALL_ERROR;
	}
}

//static
int SearchResults::getResults(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                              int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(2);
	SearchResults *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, SearchResults);

	OOIFBoolean retval;
	NATIVE_CALL(native_SearchResultsGetResults(instance->getIdentifier(),
	            NUMBER_VALUE(argv[0]),
	            NUMBER_VALUE(argv[1]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int SearchResults::abort(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
                         jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	SearchResults *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, SearchResults);
	NATIVE_CALL(native_SearchResultsAbort(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

#ifdef OIPF
//static
int SearchResults::update(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
                          jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	SearchResults *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, SearchResults);

	NATIVE_CALL(native_SearchResultsUpdate(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}
#endif // OIPF

void SearchResults::releaseNative(SearchResultsHandle id)
{
	NATIVE_WARN_NOK(native_SearchResultsReleaseHandle(id));
}
#endif // OIPF || HBBTV_1_2_1
