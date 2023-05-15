// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "MetadataSearch.h"

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
#include "SearchResults.h"

#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

using namespace Ooif;

MetadataSearch::MetadataSearch(MetadataSearchHandle i) : WrapperObject<MetadataSearch>(i)
{
	classType = CLASS_METADATASEARCH;
}

MetadataSearch::~MetadataSearch()
{
	ENTRY;
	// Let's assume MetadataSearchHandle is automatically collected (eg an int)
}

int MetadataSearch::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("searchTarget", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("result", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("setQuery", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("createQuery", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("findProgrammesFromStream", CHECK_BROADCAST);
#if defined(HMX_WEBUI)
	GET_GUARDED_SECURITY_CHECK("id", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("query", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("addChannelConstraint", CHECK_BROADCAST);
#endif
#endif // HBBTV_SECURITY

	NATIVE_GET_NUMBER("searchTarget", native_SearchGetSearchTarget(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_OBJECT("result", obj, native_SearchGetResult(getIdentifier(), &RETVAL), new SearchResults(RETVAL), JSP_GET_VALUE);
#if defined(HMX_WEBUI)
	NATIVE_GET_NUMBER("id", native_SearchGetId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif

#ifdef OIPF
	NATIVE_GET_OBJECT("query", obj, native_SearchGetQuery(getIdentifier(), &RETVAL), new Query(RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(addChannelConstraint, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(orderBy, obj, "sb", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(addCurrentRatingConstraint, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(addRatingConstraint, obj, "on", JSP_GET_VALUE_CACHE);
#endif // OIPF

	GET_FUNCTION(setQuery, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createQuery, obj, "sns", JSP_GET_VALUE_CACHE);

#ifdef HBBTV_1_2_1
	GET_FUNCTION(findProgrammesFromStream, obj, "on", JSP_GET_VALUE_CACHE);
#else
	GET_FUNCTION(findProgrammesFromStream, obj, "onn", JSP_GET_VALUE_CACHE);
#endif

#if defined(HMX_WEBUI)
	GET_FUNCTION(addConstraint, obj, "ss", JSP_GET_VALUE_CACHE);
#endif

	return JSP_GET_NOTFOUND;
}

int MetadataSearch::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	SET_READONLY("searchTarget");
	SET_READONLY("result");
#ifdef OIPF
	if (!strcmp(name, "query"))
	{
		Query *q = OBJECT_VALUE(*value, Query);
		if (!q)
		{
			THROW_WRONG_ARG_ERR(obj, value, JSP_PUT_EXCEPTION);
		}
		NATIVE_SET("query", native_SearchSetQuery(getIdentifier(), q->getIdentifier()));
	}
#endif // OIPF
	return JSP_PUT_NOTFOUND;
}

int MetadataSearch::setQuery(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                             int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	MetadataSearch *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, MetadataSearch);
	Query *q = OBJECT_VALUE_OR_THROW(q, argv[0], Query);
	NATIVE_CALL(native_SearchSetQueryF(instance->getIdentifier(), q->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

#ifdef OIPF
//static
int MetadataSearch::addChannelConstraint(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT_MIN_MAX(0, 1);
	MetadataSearch *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, MetadataSearch);

	if (argc == 0 || (argc == 1 && argv[0].type == JSP_TYPE_NULL))
	{
		NATIVE_CALL(native_SearchAddChannelConstraint(instance->getIdentifier(), 0));
		return JSP_CALL_NO_VALUE;
	}

	ObjectInstance *obj = OBJECT_VALUE_OR_THROW(obj, argv[0], ObjectInstance);
	if (obj->getType() == CLASS_CHANNEL
#ifdef JLABS
		|| obj->getType() == CLASS_JLABS_DTT_CHANNEL
		|| obj->getType() == CLASS_JLABS_BS_CATV_CHANNEL
#endif
	)
	{
		// Add channel
		Channel *c = static_cast<Channel*>(obj);
		NATIVE_CALL(native_SearchAddChannelConstraint(instance->getIdentifier(),
		            c->getIdentifier()));
		return JSP_CALL_NO_VALUE;
	}
	else if (obj->getType() == CLASS_CHANNELLIST
#ifdef JLABS
		|| obj->getType() == CLASS_JLABS_DTT_CHANNEL_LIST
		|| obj->getType() == CLASS_JLABS_BS_CATV_CHANNEL_LIST
#endif
	)
	{
		// Add all these channels
		ChannelList *cl = static_cast<ChannelList*>(obj);
		for (int i=0; i<cl->getSize(); i++)
		{
			Channel *c = cl->internalGetItem(i);
			NATIVE_CALL(native_SearchAddChannelConstraint(instance->getIdentifier(),
			            c->getIdentifier()));
		}
		return JSP_CALL_NO_VALUE;
	}
	else
	{
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}
}

//static
int MetadataSearch::orderBy(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                            int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	MetadataSearch *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, MetadataSearch);
	if (argv[0].type == JSP_TYPE_NULL)
	{
		NATIVE_CALL(native_SearchOrderBy(instance->getIdentifier(), NULL, BOOLEAN_VALUE(argv[1])));
		return JSP_CALL_NO_VALUE;
	}

	NATIVE_CALL(native_SearchOrderBy(instance->getIdentifier(), STRING_VALUE(argv[0]), BOOLEAN_VALUE(argv[1])));
	return JSP_CALL_NO_VALUE;
}

//static
int MetadataSearch::addCurrentRatingConstraint(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	MetadataSearch *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, MetadataSearch);

	NATIVE_CALL(native_SearchAddCurrentRatingConstraint(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int MetadataSearch::addRatingConstraint(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	MetadataSearch *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, MetadataSearch);

	ParentalRatingSchemeHandle scheme;
	if (argv[0].type == JSP_TYPE_NULL)
	{
		scheme = 0;
	}
	else
	{
		ParentalRatingScheme* prs = OBJECT_VALUE_OR_THROW(prs, argv[0], ParentalRatingScheme);
		scheme = prs->getIdentifier();
	}

	if (argv[1].type == JSP_TYPE_NULL)
	{
		NATIVE_CALL(native_SearchAddRatingConstraint(instance->getIdentifier(), scheme, TRUE, 0));
	}
	else
	{
		NATIVE_CALL(native_SearchAddRatingConstraint(instance->getIdentifier(), scheme, FALSE, NUMBER_VALUE(argv[1])));
	}
	return JSP_CALL_NO_VALUE;
}
#endif // OIPF

//static
int MetadataSearch::createQuery(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
#ifdef HBBTV_SECURITY
    CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY

    CHECK_ARGUMENT_COUNT_MIN_MAX(1, 3);

    MetadataSearch *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, MetadataSearch);
    QueryHandle id;

    // count == 1: (queryString)
    if (argc == 1) {
#ifdef	HMX_WEBUI
        NATIVE_CALL(native_SearchCreateQuery(instance->getIdentifier(), STRING_VALUE(argv[0]), (OOIFNumber)-1, (char*)"", &id));
#else
        NATIVE_CALL(native_SearchCreateQuery(STRING_VALUE(argv[0]), (OOIFNumber)-1, (char*)"", &id));
#endif
    }
    // count == 3: (field, comparison, value)
    else if (argc == 3) {
#ifdef	HMX_WEBUI
        NATIVE_CALL(native_SearchCreateQuery(instance->getIdentifier(), STRING_VALUE(argv[0]), NUMBER_VALUE(argv[1]),
                                             STRING_VALUE(argv[2]), &id));
#else
        NATIVE_CALL(native_SearchCreateQuery(STRING_VALUE(argv[0]), NUMBER_VALUE(argv[1]),
                                             STRING_VALUE(argv[2]), &id));
#endif
    }
    else {
        OOIF_LOG_WARNING("Invalid argument count: %d", argc);
        THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
    }

    RETURN_OBJECT(this_obj, new Query(id), result, JSP_CALL_VALUE);
}

//static
int MetadataSearch::findProgrammesFromStream(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_1_2_1
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(2);
#else // HBBTV_1_2_1
	CHECK_ARGUMENT_COUNT(3);
#endif // HBBTV_1_2_1

	MetadataSearch *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, MetadataSearch);
	Channel *c = OBJECT_VALUE_OR_THROW(c, argv[0], Channel);
#ifdef HBBTV_1_2_1
	if (argv[1].type == JSP_TYPE_NULL)
	{
		NATIVE_CALL(native_SearchFindProgrammesFromStream(instance->getIdentifier(),
		            c->getIdentifier(),
		            TRUE,
		            0));
	}
	else if (argv[1].type == JSP_TYPE_NUMBER)
	{
		NATIVE_CALL(native_SearchFindProgrammesFromStream(instance->getIdentifier(),
		            c->getIdentifier(),
		            FALSE,
		            NUMBER_VALUE(argv[1])));
	}
	return JSP_CALL_NO_VALUE;
#else // HBBTV_1_2_1
	OOIFBoolean retval = FALSE;
	if (argv[1].type == JSP_TYPE_NULL)
	{
		NATIVE_CALL(native_SearchFindProgrammesFromStream(instance->getIdentifier(),
		            c->getIdentifier(),
		            TRUE,
		            0,
		            NUMBER_VALUE(argv[2]),
		            &retval));
	}
	else if (argv[1].type == JSP_TYPE_NUMBER)
	{
		NATIVE_CALL(native_SearchFindProgrammesFromStream(instance->getIdentifier(),
		            c->getIdentifier(),
		            FALSE,
		            NUMBER_VALUE(argv[1]),
		            NUMBER_VALUE(argv[2]),
		            &retval));
	}
	RETURN_BOOLEAN(retval, JSP_GET_VALUE);
#endif // HBBTV_1_2_1
}

void MetadataSearch::releaseNative(MetadataSearchHandle id)
{
	NATIVE_WARN_NOK(native_MetadataSearchReleaseHandle(id));
}

#ifdef	HMX_WEBUI
//static
int MetadataSearch::addConstraint(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
											   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT_MIN_MAX(1, 2);

	MetadataSearch *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, MetadataSearch);

	const char *field = STRING_VALUE(argv[0]);
	const char *value;

	if (argc == 1)
		value = "";
	else
		value = STRING_VALUE(argv[1]);

	NATIVE_CALL(native_SearchAddConstraint(instance->getIdentifier(), field, value));
	return JSP_CALL_NO_VALUE;
}
#endif
#endif // OIPF || HBBTV_1_2_1
