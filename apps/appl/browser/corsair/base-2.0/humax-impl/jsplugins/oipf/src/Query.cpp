// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "Query.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "Exception.h"
#include "macros.h"

#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

using namespace Ooif;

Query::Query(QueryHandle i) : WrapperObject<Query>(i)
{
	classType = CLASS_QUERY;
}

Query::~Query()
{
	ENTRY;
	// Let's assume QueryHandle is automatically collected (eg an int)
}

int Query::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("and", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("or", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("not", CHECK_BROADCAST);
#endif // HBBTV_SECURITY

	GET_RENAMED_FUNCTION("and", _and, obj, "o", JSP_GET_VALUE_CACHE);
	GET_RENAMED_FUNCTION("or", _or, obj, "o", JSP_GET_VALUE_CACHE);
	GET_RENAMED_FUNCTION("not", _not, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

//static
int Query::_and(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	Query *q1 = EXTRACT_OBJECT_OR_THROW(q1, this_obj, Query);
	Query *q2 = OBJECT_VALUE_OR_THROW(q2, argv[0], Query);

	QueryHandle id;
	NATIVE_CALL(native_QueryAnd(q1->getIdentifier(), q2->getIdentifier(), &id));
	RETURN_OBJECT(this_obj, new Query(id), result, JSP_CALL_VALUE);
}

//static
int Query::_or(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	Query *q1 = EXTRACT_OBJECT_OR_THROW(q1, this_obj, Query);
	Query *q2 = OBJECT_VALUE_OR_THROW(q2, argv[0], Query);

	QueryHandle id;
	NATIVE_CALL(native_QueryOr(q1->getIdentifier(), q2->getIdentifier(), &id));
	RETURN_OBJECT(this_obj, new Query(id), result, JSP_CALL_VALUE);
}

//static
int Query::_not(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	Query *q = EXTRACT_OBJECT_OR_THROW(q, this_obj, Query);

	QueryHandle id;
	NATIVE_CALL(native_QueryNot(q->getIdentifier(), &id));
	RETURN_OBJECT(this_obj, new Query(id), result, JSP_CALL_VALUE);
}

void Query::releaseNative(QueryHandle id)
{
	NATIVE_WARN_NOK(native_QueryReleaseHandle(id));
}
#endif // OIPF || HBBTV_1_2_1
