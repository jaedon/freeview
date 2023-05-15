/**************************************************************************************/
/**
 * @file jlabsContentResults.cpp
 *
 * jlabsContentResults  module
 *
 * @author  hae-kwang jeong(hkjeong@humaxdigital.com)
 * @date    2012/03/20
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/


//#if defined OIPF && defined JLABS
#if defined OIPF 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"

#include "jlabsContentResults.h"
#include "NativeJlabsContentResults.h"
#include "WrapperObject.h"

//#include "jlabsCDSRecording.h"


using namespace Ooif;

jlabsContentResults::jlabsContentResults(jlabsContentResultsHandle i) : WrapperObject<jlabsContentResults>(i)
{
	classType = CLASS_JLABS_SEARCHRESULT;
}

jlabsContentResults::~jlabsContentResults()
{
	ENTRY;
}

int jlabsContentResults::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_NUMBER("length", native_jlabsContentResultsGetLength(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("offset", native_jlabsContentResultsGetOffset(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("totalSize", native_jlabsContentResultsGetTotalSize(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(item, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getResults, obj, "nn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(abort, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(update, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsContentResults::item(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	jlabsContentResults *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsContentResults);
	void *res;
	NATIVE_CALL(native_jlabsContentResultsItem(instance->getIdentifier(),
				NUMBER_VALUE(argv[0]), &res));
	return OOIF_RETURN_OK;
}

int jlabsContentResults::getResults(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	jlabsContentResults *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsContentResults);

	OOIFBoolean retval;
	NATIVE_CALL(native_jlabsContentResultsGetResults(instance->getIdentifier(),
				NUMBER_VALUE(argv[0]),
				NUMBER_VALUE(argv[1]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsContentResults::abort(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
		jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	jlabsContentResults *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsContentResults);
	NATIVE_CALL(native_jlabsContentResultsAbort(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

int jlabsContentResults::update(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
		jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	jlabsContentResults *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsContentResults);

	NATIVE_CALL(native_jlabsContentResultsUpdate(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

void jlabsContentResults::releaseNative(jlabsContentResultsHandle id)
{
	NATIVE_WARN_NOK(native_jlabsContentResultsReleaseHandle(id));
}

#endif // OIPF


