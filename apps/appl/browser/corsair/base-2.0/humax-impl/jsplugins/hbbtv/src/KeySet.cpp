// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "KeySet.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "NativeApplicationManager.h"
#include "Application.h"
#include "OOIFUtils.h"

using namespace Ooif;

KeySet::KeySet(ApplicationIdentifier appId) : _appId(appId)
{
	ENTRY;
	classType = CLASS_KEYSET;
}

KeySet::~KeySet()
{
	ENTRY;
}

int KeySet::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	// Constants
	GET_NUMBER("RED", KEY_RED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("GREEN", KEY_GREEN, JSP_GET_VALUE_CACHE);
	GET_NUMBER("YELLOW", KEY_YELLOW, JSP_GET_VALUE_CACHE);
	GET_NUMBER("BLUE", KEY_BLUE, JSP_GET_VALUE_CACHE);
	GET_NUMBER("NAVIGATION", KEY_NAVIGATION, JSP_GET_VALUE_CACHE);
	GET_NUMBER("VCR", KEY_VCR, JSP_GET_VALUE_CACHE);
	GET_NUMBER("SCROLL", KEY_SCROLL, JSP_GET_VALUE_CACHE);
	GET_NUMBER("INFO", KEY_INFO, JSP_GET_VALUE_CACHE);
	GET_NUMBER("NUMERIC", KEY_NUMERIC, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ALPHA", KEY_ALPHA, JSP_GET_VALUE_CACHE);
#if defined(OIPF) || defined(HBBTV_1_2_1)
	GET_NUMBER("OTHER", KEY_OTHER, JSP_GET_VALUE_CACHE);
#endif // OIPF || HBBTV_1_2_1
	NATIVE_GET_NUMBER("value", native_KeySetGetValue(_appId, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("maximumValue", native_KeySetGetMaximumValue(_appId, &RETVAL), JSP_GET_VALUE);
#ifdef OIPF
	NATIVE_GET_NUMBERS_COLLECTION("otherKeys", obj, native_KeySetOtherKeys(_appId, &SIZE, &NATIVE_ARRAY), getNumberCollection(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_NUMBERS_COLLECTION("maximumOtherKeys", obj, native_KeySetMaximumOtherKeys(_appId, &SIZE, &NATIVE_ARRAY), getNumberCollection(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
#endif // OIPF
	GET_FUNCTION(setValue, obj, "ns", JSP_GET_VALUE_CACHE);
#if 1 // OIPF 2.3 added
    GET_BOOLEAN("supportsPointer", false, JSP_GET_VALUE_CACHE);
    GET_FUNCTION(getKeyIcon, obj, "n", JSP_GET_VALUE_CACHE);    
#endif
	return JSP_GET_NOTFOUND;
}

int KeySet::setValue(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#ifdef OIPF
	CHECK_ARGUMENT_COUNT_MIN_MAX(1, 2);
#else
	CHECK_ARGUMENT_COUNT(1);
#endif // OIPF

	KeySet *keyset = EXTRACT_OBJECT_OR_THROW(keyset, this_obj, KeySet);
	OOIFReturnCode ret_ = OOIF_RETURN_ERROR;
	OOIFNumber retval;

	int otherKeysCount = 0;
	OOIFNumber *otherKeys = NULL;
#ifdef OIPF
	if (argc == 2)
	{
		int r = OOIFUtils::deserializeIntegerArray(STRING_VALUE(argv[1]), &otherKeysCount, &otherKeys);
		if (r < 0)
			THROW_OOM_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}
#endif

	ret_ = native_KeySetSetValue(keyset->_appId, NUMBER_VALUE(argv[0]), otherKeysCount, otherKeys, &retval);
	if (otherKeys)
		HLIB_STD_MemFree(otherKeys);

	NATIVE_CALL(ret_);
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

NumberCollection *KeySet::getNumberCollection(int count, OOIFNumber *values)
{
	NumberCollection *collection = new NumberCollection();
	for (int i = 0; i < count; i++)
	{
		collection->addItem(values[i]);
	}
	HLIB_STD_MemFree(values);
	return collection;
}

#if 1 // OIPF 2.3 added
int KeySet::getKeyIcon(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	KeySet *keyset = EXTRACT_OBJECT_OR_THROW(keyset, this_obj, KeySet);

	const char *retval;
	NATIVE_CALL(native_KeySetGetKeyIcon(keyset->_appId, NUMBER_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}
#endif
