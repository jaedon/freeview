// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Configuration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "NativeConfiguration.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "Channel.h"
#include "Programme.h"


using namespace Ooif;

int Configuration::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#if defined(HMX_SECURITY)
	NATIVE_SECURITY_CHECK(obj);
#endif

	NATIVE_GET_STRING("preferredAudioLanguage", native_ConfigGetPreferredAudioLanguage(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("preferredSubtitleLanguage", native_ConfigGetPreferredSubtitleLanguage(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("countryId", native_ConfigGetCountryId(&RETVAL), JSP_GET_VALUE);
#ifdef HBBTV_1_2_1
	NATIVE_GET_STRING("preferredUILanguage", native_ConfigGetPreferredUILanguage(&RETVAL), JSP_GET_VALUE);
#endif // HBBTV_1_2_1
#ifdef HBBTV
	NATIVE_GET_BOOLEAN("timeShiftSynchronized", native_ConfigGetTimeShiftSynchronized(&RETVAL), JSP_GET_VALUE);
#endif // HBBTV
#ifdef OIPF
	NATIVE_GET_NUMBER("preferredTimeShiftMode", native_ConfigGetPreferredTimeShiftMode(&RETVAL), JSP_GET_VALUE);
#endif // OIPF

	//optional part of interface for HbbTV / HbbTV 1.2.1 and mandatory for OIPF
	NATIVE_GET_NUMBER("regionId", native_ConfigGetRegionId(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrPolicy", native_ConfigGetPvrPolicy(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrSaveEpisodes", native_ConfigGetPvrSaveEpisodes(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrSaveDays", native_ConfigGetPvrSaveDays(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrStartPadding", native_ConfigGetPvrStartPadding(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrEndPadding", native_ConfigGetPvrEndPadding(&RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(getText, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setText, obj, "ss", JSP_GET_VALUE_CACHE);

	//optional part of interface for HbbTV
#if defined(HBBTV) && !defined(HBBTV_1_2_1)
	NATIVE_GET_BOOLEAN("isPINEntryLocked", native_ConfigGetIsPINEntryLocked(&RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(setParentalControlPIN, obj, "ss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setParentalControlPINEnable, obj, "sb", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getParentalControlPINEnable, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unlockWithParentalControlPIN, obj, "son", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(verifyParentalControlPIN, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setBlockUnrated, obj, "sb", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getBlockUnrated, obj, "", JSP_GET_VALUE_CACHE);
#endif // HBBTV && !HBBTV_1_2_1

	return JSP_GET_NOTFOUND;
}

int Configuration::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

#ifdef HBBTV
	SET_READONLY("preferredAudioLanguage");
	SET_READONLY("preferredSubtitleLanguage");
	SET_READONLY("countryId");
#endif // HBBTV
#ifdef HBBTV_1_2_1
	SET_READONLY("preferredUILanguage");
#endif // HBBTV_1_2_1
#ifdef OIPF
	NATIVE_SET("preferredTimeShiftMode", native_ConfigSetPreferredTimeShiftMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("preferredAudioLanguage", native_ConfigSetPreferedAudioLanguage(STRING_VALUE(value[0])));
	NATIVE_SET("preferredSubtitleLanguage", native_ConfigSetPreferredSubtitleLanguage(STRING_VALUE(value[0])));
	NATIVE_SET("countryId", native_ConfigSetCountryId(STRING_VALUE(value[0])));
#endif // OIPF

	//optional part for HbbTV / HbbTV 1.2.1 and mandatory for OIPF
	NATIVE_SET("regionId", native_ConfigSetRegionId(NUMBER_VALUE(value[0])));
	NATIVE_SET("pvrPolicy", native_ConfigSetPvrPolicy(NUMBER_VALUE(value[0])));
	NATIVE_SET("pvrSaveEpisodes", native_ConfigSetPvrSaveEpisodes(NUMBER_VALUE(value[0])));
	NATIVE_SET("pvrSaveDays", native_ConfigSetPvrSaveDays(NUMBER_VALUE(value[0])));
	NATIVE_SET("pvrStartPadding", native_ConfigSetPvrStartPadding(NUMBER_VALUE(value[0])));
	NATIVE_SET("pvrEndPadding", native_ConfigSetPvrEndPadding(NUMBER_VALUE(value[0])));

	//optional part of interface for HbbTV
#if defined(HBBTV) && !defined(HBBTV_1_2_1)
	SET_READONLY("isPINEntryLocked");
#endif // HBBTV && !HBBTV_1_2_1

	return JSP_PUT_NOTFOUND;
}

#if defined(HBBTV) && !defined(HBBTV_1_2_1)
//static
int Configuration::setParentalControlPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	OOIFNumber retval;
	NATIVE_CALL(native_ConfigSetParentalControlPIN(STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int Configuration::setParentalControlPINEnable(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	OOIFNumber retval;
	NATIVE_CALL(native_ConfigSetParentalControlPINEnable(STRING_VALUE(argv[0]), BOOLEAN_VALUE(argv[1]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int Configuration::getParentalControlPINEnable(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;
	NATIVE_CALL(native_ConfigGetParentalControlPINEnable(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int Configuration::unlockWithParentalControlPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(3);

	ObjectInstance *targetObj = OBJECT_VALUE(argv[1], ObjectInstance);
	if(!targetObj ||
	        (targetObj->getType() != CLASS_CHANNEL && targetObj->getType() != CLASS_PROGRAMME))
	{
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}
	void * target;
	if (targetObj->getType() == CLASS_CHANNEL)
		target = static_cast<Channel*>(targetObj)->getIdentifier();
	else // if (targetObj->getType() == CLASS_PROGRAMME)
		target = static_cast<Programme*>(targetObj)->getIdentifier();
	const char *pcPIN = STRING_VALUE(argv[0]);
	OOIFNumber duration = NUMBER_VALUE(argv[2]);
	OOIFNumber retval;
	NATIVE_CALL(native_ConfigUnlockWithParentalControlPIN(pcPIN, target, duration, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int Configuration::verifyParentalControlPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval;
	NATIVE_CALL(native_ConfigVerifyParentalControlPIN(STRING_VALUE(argv[0]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int Configuration::setBlockUnrated(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	OOIFNumber retval;
	NATIVE_CALL(native_ConfigSetBlockUnrated(STRING_VALUE(argv[0]), BOOLEAN_VALUE(argv[1]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int Configuration::getBlockUnrated(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;
	NATIVE_CALL(native_ConfigGetBlockUnrated(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
#endif // HBBTV && !HBBTV_1_2_1

//static
int Configuration::getText(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	const char *retval;
	NATIVE_CALL(native_ConfigGetText(STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int Configuration::setText(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	NATIVE_CALL(native_ConfigSetText(STRING_VALUE(argv[0]), STRING_VALUE(argv[1])));
	return JSP_CALL_NO_VALUE;
}

