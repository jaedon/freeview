// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)

#include "DownloadTrigger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NativeDownload.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include "jsplugin.h"
#include "Exception.h"
#include "macros.h"
#include "OOIFUtils.h"

using namespace Ooif;

int DownloadTrigger::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY

	GET_FUNCTION(registerDownload, obj, "ss", JSP_GET_VALUE);
	GET_FUNCTION(registerDownloadURL, obj, "sss", JSP_GET_VALUE);
#ifdef OIPF
	GET_FUNCTION(checkDownloadPossible, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(registerDownloadFromCRID, obj, "sss", JSP_GET_VALUE_CACHE);
#endif // OIPF

	return JSP_GET_NOTFOUND;
}

//static
int DownloadTrigger::registerDownload(jsplugin_obj *this_obj,
                                      jsplugin_obj *function_obj,
                                      int argc, jsplugin_value *argv,
                                      jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT_MIN_MAX(1, 2);

	const char* downloadStart;
	//we need special handling of string "null" since we tell jsplugin
	//module to read it as a string (because of to Date)
	if (argc >= 2 && strcmp("null", STRING_VALUE(argv[1])) != 0)
		downloadStart = STRING_VALUE(argv[1]);
	else
		downloadStart = NULL;

	long int window = OOIFUtils::getWindow(this_obj);
	char *host = OOIFUtils::getHost(this_obj);
	const char* retval;
	NATIVE_CALL(native_DownloadRegisterDownload(STRING_VALUE(argv[0]),
	            downloadStart,
	            window, host,
	            &retval));

	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int DownloadTrigger::registerDownloadURL(jsplugin_obj *this_obj,
        jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv,
        jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT_MIN_MAX(2, 3);

	const char* downloadStart;
	if (argc >= 3 && strcmp("null", STRING_VALUE(argv[2])) != 0)
		downloadStart = STRING_VALUE(argv[2]);
	else
		downloadStart = NULL;
	long int window = OOIFUtils::getWindow(this_obj);
	char *host = OOIFUtils::getHost(this_obj);
	const char* retval;
	NATIVE_CALL(native_DownloadRegisterDownloadURL(STRING_VALUE(argv[0]),
	            STRING_VALUE(argv[1]),
	            downloadStart,
	            window, host,
	            &retval));

	RETURN_STRING(retval, JSP_CALL_VALUE);
}

#ifdef OIPF
//static
int DownloadTrigger::registerDownloadFromCRID(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT_MIN_MAX(2, 3);

	const char* downloadStart;
	if (argc >= 3 && strcmp("null", STRING_VALUE(argv[2])) != 0)
		downloadStart = STRING_VALUE(argv[2]);
	else
		downloadStart = NULL;
	long int window = OOIFUtils::getWindow(this_obj);
	char *host = OOIFUtils::getHost(this_obj);
	const char* retval;
	NATIVE_CALL(native_DownloadRegisterDownloadFromCRID(STRING_VALUE(argv[0]),
	            STRING_VALUE(argv[1]),
	            downloadStart,
	            window, host,
	            &retval));

	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int DownloadTrigger::checkDownloadPossible(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	OOIFNumber retval;
	NATIVE_CALL(native_DownloadCheckDownloadPossible(NUMBER_VALUE(argv[0]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}
#endif // OIPF
#endif // HBBTV_DOWNLOAD || OIPF
