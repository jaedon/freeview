// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ParentalControlManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Collection.h"
#include "ParentalRatingScheme.h"
#include "ParentalRatingSchemeCollection.h"
#include "Exception.h"
#ifdef OIPF
#include "Channel.h"
#include "VideoOnDemand.h"
#include "VideoBroadcast.h"
#endif // OIPF

using namespace std;

using namespace Ooif;

int ParentalControlManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#if defined(HMX_SECURITY)
	NATIVE_SECURITY_CHECK(obj);
#endif

	GET_OBJECT("parentalRatingSchemes", obj, new ParentalRatingSchemeCollection(), JSP_GET_VALUE);
#ifndef OIPF
	GET_OBJECT("aParentalRatingScheme", obj, new ParentalRatingScheme((void*)1), JSP_GET_VALUE);
#endif // !OIPF

#ifdef OIPF
	NATIVE_GET_BOOLEAN("isPINEntryLocked", native_ParentalControlGetIsPINEntryLocked(&RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(setParentalControlStatus, obj, "sb", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getParentalControlStatus, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getBlockUnrated, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setParentalControlPIN, obj, "ss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unlockWithParentalControlPIN, obj, "so", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(verifyParentalControlPIN, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setBlockUnrated, obj, "sb", JSP_GET_VALUE_CACHE);
#ifdef HMX_WEBUI
	GET_FUNCTION(resetParentalControlPIN, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isNagraSmartCardInserted, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setNargaSmartCardPIN, obj, "ss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getNargaSmartCardPINAccessibleState, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getNargaSmartCardPINRetryCount, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getNargaSmartCardPINRemainTime, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(verifyNargaSmartCardPIN, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unlockAV, obj, "", JSP_GET_VALUE_CACHE);
#endif	/* HMX_WEBUI */
#endif /* OIPF */

	return JSP_GET_NOTFOUND;
}

int ParentalControlManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	SET_READONLY("parentalRatingSchemes");
#ifdef OIPF
	SET_READONLY("isPINEntryLocked");
#endif //OIPF
	return JSP_PUT_NOTFOUND;
}

#ifdef OIPF
//static
int ParentalControlManager::setParentalControlStatus(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlSetParentalControlStatus(STRING_VALUE(argv[0]),
	            BOOLEAN_VALUE(argv[1]),
	            &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int ParentalControlManager::getParentalControlStatus(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;
	NATIVE_CALL(native_ParentalControlGetParentalControlStatus(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int ParentalControlManager::getBlockUnrated(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;
	NATIVE_CALL(native_ParentalControlGetBlockUnrated(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int ParentalControlManager::setParentalControlPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlSetParentalControlPIN(STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int ParentalControlManager::unlockWithParentalControlPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int obj_type = -1;
	CHECK_ARGUMENT_COUNT(2);

	ObjectInstance *targetObj = OBJECT_VALUE(argv[1], ObjectInstance);
	if(!targetObj)
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);

	void * target;
	if (targetObj->getType() == CLASS_CHANNEL)
	{
		obj_type = 0;
		target = static_cast<Channel*>(targetObj)->getIdentifier();
	}
	else if (targetObj->getType() == CLASS_VIDEO_ON_DEMAND)
	{
		obj_type = 1;
		target = static_cast<VideoOnDemand*>(targetObj)->getIdentifier();
	}
	else if (targetObj->getType() == CLASS_VIDEO_BROADCAST)
	{
		obj_type = 2;
		target = static_cast<VideoBroadcast*>(targetObj)->getIdentifier();
	}
	else
	{
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}

	const char *pcPIN = STRING_VALUE(argv[0]);
	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlUnlockWithParentalControlPIN(pcPIN, obj_type, target, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int ParentalControlManager::verifyParentalControlPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlVerifyParentalControlPIN(STRING_VALUE(argv[0]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int ParentalControlManager::setBlockUnrated(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlSetBlockUnrated(STRING_VALUE(argv[0]), BOOLEAN_VALUE(argv[1]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}
#endif /* OIPF */


#ifdef HMX_WEBUI
//static
int ParentalControlManager::resetParentalControlPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlResetParentalControlPIN(STRING_VALUE(argv[0]),&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int ParentalControlManager::isNagraSmartCardInserted(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;
	NATIVE_CALL(native_ParentalControlIsNagraSmartCardInserted(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int ParentalControlManager::setNargaSmartCardPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	OOIFBoolean retval;
	NATIVE_CALL(native_ParentalControlSetNargaSmartCardPIN(STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int ParentalControlManager::getNargaSmartCardPINAccessibleState(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlGetNargaSmartCardPINAccessibleState(&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int ParentalControlManager::getNargaSmartCardPINRetryCount(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlGetNargaSmartCardPINRetryCount(&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int ParentalControlManager::getNargaSmartCardPINRemainTime(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlGetNargaSmartCardPINRemainTime(&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int ParentalControlManager::verifyNargaSmartCardPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlVerifyNargaSmartCardPIN(STRING_VALUE(argv[0]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int ParentalControlManager::unlockAV(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlUnlockAV(&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

#endif	/* HMX_WEBUI */

