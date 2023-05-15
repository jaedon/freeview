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
	CHECK_ARGUMENT_COUNT(2);

	ObjectInstance *targetObj = OBJECT_VALUE(argv[1], ObjectInstance);
	if(!targetObj)
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);

	void * target;
	if (targetObj->getType() == CLASS_CHANNEL)
	{
		target = static_cast<Channel*>(targetObj)->getIdentifier();
	}
	else if (targetObj->getType() == CLASS_VIDEO_ON_DEMAND)
	{
		target = static_cast<VideoOnDemand*>(targetObj)->getIdentifier();
	}
	else if (targetObj->getType() == CLASS_VIDEO_BROADCAST)
	{
		target = static_cast<VideoBroadcast*>(targetObj)->getIdentifier();
	}
	else
	{
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}

	const char *pcPIN = STRING_VALUE(argv[0]);
	OOIFNumber retval;
	NATIVE_CALL(native_ParentalControlUnlockWithParentalControlPIN(pcPIN, target, &retval));
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
