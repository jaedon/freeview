// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ApplicationPrivateData.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "NativeApplicationManager.h"
#include "NativeSecurity.h"
#include "Application.h"
#include "KeySet.h"
#include "Channel.h"

using namespace Ooif;

ApplicationPrivateData::ApplicationPrivateData(ApplicationIdentifier appId) : _appId(appId)
{
	ENTRY;
	classType = CLASS_APPLICATIONPRIVATEDATA;
}

ApplicationPrivateData::~ApplicationPrivateData()
{
	ENTRY;
}

int ApplicationPrivateData::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_OBJECT("keyset", obj, new KeySet(_appId), JSP_GET_VALUE);

#ifdef HBBTV_1_2_1
#ifdef HBBTV_SECURITY
	if (!strcmp(name, "currentChannel"))
		SECURITY_CHECK(obj, CHECK_BROADCAST, RETURN_NULL(JSP_GET_VALUE));
#endif // HBBTV_SECURITY
#endif // HBBTV_1_2_1
	NATIVE_GET_OBJECT("currentChannel", obj, native_AppPrivateDataGetCurrentChannel(_appId, &RETVAL), new Channel(RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(getFreeMem, obj, "", JSP_GET_VALUE_CACHE);

#ifdef OIPF
	NATIVE_GET_BOOLEAN("wakeupApplication", native_AppPrivateDataGetWakeupApplication(_appId, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("wakeupOITF", native_AppPrivateDataGetWakeupOITF(_appId, &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(prepareWakeupApplication, obj, "ss-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(prepareWakeupOITF, obj, "-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(prepareWakeupTvtvUpdate, obj, "-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(clearWakeupToken, obj, "", JSP_GET_VALUE_CACHE);
#endif // OIPF

#ifdef HMX_WEBUI
	NATIVE_GET_NUMBER("wakeupRepeatDays", native_AppPrivateDataGetWakeupRepeatDays(_appId, &RETVAL), JSP_GET_VALUE);
	if (!strcmp(name, "wakeupChannel")) {
		const char 		*ccid = NULL;
		ChannelHandle	channelHandle = NULL;

		NATIVE_GET(native_AppPrivateDataGetWakeupChannelID(_appId, &ccid));
		NATIVE_GET(native_ChannelGetByCCID(ccid, &channelHandle));
		RETURN_OBJECT(obj, new Channel(channelHandle), result, JSP_GET_VALUE);
	}
	if (!strcmp(name, "wakeupTime")) {
		return returnWakeupTime(obj, result);
	}
	if (!strcmp(name, "tvtvWakeupTime")) {
		return returnTvtvWakeupTime(obj, result);
	}
	NATIVE_GET_BOOLEAN("isTvtvStandbyEnable", native_AppPrivateDataGetTvtvIsStandbyEnable(_appId, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("wakeupVolume", native_AppPrivateDataGetWakeupVolume(_appId, &RETVAL), JSP_GET_VALUE);
	if (!strcmp(name,"widgetContentUrl")) {
		const char * retval = Application::m_ContentUrl.c_str();
		RETURN_STRING(retval, JSP_GET_VALUE);
	}
#endif
	return JSP_GET_NOTFOUND;
}

//static
int ApplicationPrivateData::getFreeMem(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ApplicationPrivateData *apd = EXTRACT_OBJECT_OR_THROW(apd, this_obj, ApplicationPrivateData);

	OOIFNumber retval;
	NATIVE_CALL(native_AppPrivateDataGetFreeMem(apd->_appId, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

#ifdef OIPF
//static
int ApplicationPrivateData::prepareWakeupApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ApplicationPrivateData *apd = EXTRACT_OBJECT_OR_THROW(apd, this_obj, ApplicationPrivateData);

	OOIFBoolean retval;
	NATIVE_CALL(native_AppPrivateDataPrepareWakeupApplication(apd->_appId, STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), STRING_VALUE(argv[2]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

// code from KAYO-453
typedef struct parse_date_userdata
{
	jsplugin_obj *global_context;
	jsplugin_obj *this_obj;
	ApplicationPrivateData *apd;
	OOIFNumber		repeatDays;
	char 			*channelID;
	OOIFNumber		volume;
	OOIFBoolean     isTvtvStandbyEnable;
} parse_date_userdata;

//static
void ApplicationPrivateData::getDateResults(int status, jsplugin_value *return_value, void *user_data)
{
	ENTRY;
	parse_date_userdata *ud = (parse_date_userdata*)(user_data);
	ApplicationPrivateData* apd = reinterpret_cast<ApplicationPrivateData*>(ud->apd);

	if (status == JSP_CALLBACK_OK && return_value->type == JSP_TYPE_NUMBER)
	{
		OOIF_LOG_DEBUG("getDateResults : returning value: %lf", return_value->u.number);

		OOIFBoolean retval = FALSE;
	#ifdef HMX_WEBUI
		NATIVE_WARN_NOK(native_AppPrivateDataPrepareWakeupOITFExtended(
							apd->_appId
							, return_value->u.number / 1000
							, ud->repeatDays
							, ud->channelID
							, ud->volume
							, &retval
						));
		if (ud->channelID)
			HLIB_STD_MemFree(ud->channelID);
	#else
		NATIVE_WARN_NOK(native_AppPrivateDataPrepareWakeupOITFWithUnixtime(
							apd->_appId
							, return_value->u.number / 1000
							, &retval
						));
	#endif
		apd->retvalPrepareWakeupOITF = retval;
	}
	else
	{
		OOIF_LOG_DEBUG("getDateResults : Callback failed! Error code: %d", status);
	}
	HLIB_STD_MemFree(ud);
	g_opera_callbacks->resume(apd->getHost());
}

void ApplicationPrivateData::getTvtvDateResults(int status, jsplugin_value *return_value, void *user_data)
{
	ENTRY;
	parse_date_userdata *ud = (parse_date_userdata*)(user_data);
	ApplicationPrivateData* apd = reinterpret_cast<ApplicationPrivateData*>(ud->apd);

	if (status == JSP_CALLBACK_OK && return_value->type == JSP_TYPE_NUMBER)
	{
		OOIF_LOG_DEBUG("getTvtvDateResults : returning value: %lf", return_value->u.number);

		OOIFBoolean retval = FALSE;

		NATIVE_WARN_NOK(native_AppPrivateDataPrepareWakeupTvtvUpdate(
							apd->_appId
							, return_value->u.number / 1000
							, ud->isTvtvStandbyEnable
							, &retval
						));
		apd->retvalPrepareWakeupTvtvUpdate = retval;
	}
	else
	{
		OOIF_LOG_DEBUG("getTvtvDateResults : Callback failed! Error code: %d", status);
	}
	HLIB_STD_MemFree(ud);
	g_opera_callbacks->resume(apd->getHost());
}

//static
void ApplicationPrivateData::getDateFunctionResultObj(int status, jsplugin_value *return_value, void *user_data)
{
	ENTRY;
	parse_date_userdata *ud = (parse_date_userdata*)(user_data);
	if (status == JSP_CALLBACK_OK && return_value->type == JSP_TYPE_NATIVE_OBJECT)
	{
		g_opera_callbacks->call_function(ud->global_context, ud->this_obj, return_value->u.object, 0, 0, user_data, &ApplicationPrivateData::getDateResults);
        g_opera_callbacks->resume(ud->apd->getHost());
    }
	else
	{
		OOIF_LOG_DEBUG("getDateFunctionResultObj : Callback failed! Error code: %d", status);
		HLIB_STD_MemFree(ud);
	}

}

void ApplicationPrivateData::getTvtvDateFunctionResultObj(int status, jsplugin_value *return_value, void *user_data)
{
	ENTRY;
	parse_date_userdata *ud = (parse_date_userdata*)(user_data);
	if (status == JSP_CALLBACK_OK && return_value->type == JSP_TYPE_NATIVE_OBJECT)
	{
		g_opera_callbacks->call_function(ud->global_context, ud->this_obj, return_value->u.object, 0, 0, user_data, &ApplicationPrivateData::getTvtvDateResults);
		g_opera_callbacks->resume(ud->apd->getHost());
	}
	else
	{
		OOIF_LOG_DEBUG("getTvtvDateFunctionResultObj : Callback failed! Error code: %d", status);
		HLIB_STD_MemFree(ud);
	}

}


#ifdef	HMX_WEBUI
int ApplicationPrivateData::returnWakeupTime(jsplugin_obj *obj, jsplugin_value *result)
{
	const char *time;

	NATIVE_GET(native_AppPrivateDataGetWakeupTime(_appId, &time));

	char *expr = OOIFUtils::getTmpBuffer(128);
	if (!expr)
		return JSP_GET_NOMEM;
	//*retval = "Tue, 25 Aug 2009 08:20:00 GMT";

	snprintf(expr, 128, "new Date(\'%s\')", time);

	result->u.string = expr;
	result->type = JSP_TYPE_EXPRESSION;
	return JSP_GET_VALUE;
}

int ApplicationPrivateData::returnTvtvWakeupTime(jsplugin_obj *obj, jsplugin_value *result)
{
	const char *time;

	NATIVE_GET(native_AppPrivateDataGetTvtvWakeupTime(_appId, &time));

	char *expr = OOIFUtils::getTmpBuffer(128);
	if (!expr)
		return JSP_GET_NOMEM;
	//*retval = "Tue, 25 Aug 2009 08:20:00 GMT";

	snprintf(expr, 128, "new Date(\'%s\')", time);

	result->u.string = expr;
	result->type = JSP_TYPE_EXPRESSION;
	return JSP_GET_VALUE;
}

//static
int ApplicationPrivateData::prepareWakeupOITF(jsplugin_obj *this_obj,
        jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv,
        jsplugin_value *result)
{
	ENTRY;

	ApplicationPrivateData *apd = EXTRACT_OBJECT_OR_THROW(apd, this_obj, ApplicationPrivateData);

	// KAYO-453
	if (argc < 0)
	{
		RETURN_BOOLEAN(apd->retvalPrepareWakeupOITF, JSP_GET_VALUE);
	}

	jsplugin_obj *	time;
	OOIFNumber		repeatDays = 0;
	Channel *		channel    = NULL;
	OOIFNumber		volume     = 0;

	if (argc == 1)
	{
		if (argv[0].type == JSP_TYPE_NULL)// CANCEL
		{
			OOIFBoolean retval = FALSE;
			NATIVE_CALL(native_AppPrivateDataPrepareWakeupOITFExtended(apd->_appId, 0, 0, NULL, 0, &retval));
			RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
		}
		CHECK_ARGUMENT(0, JSP_TYPE_NATIVE_OBJECT);

		time = argv[0].u.object;
	}
	else if (argc == 4)
	{
		CHECK_ARGUMENT(0, JSP_TYPE_NATIVE_OBJECT);
		CHECK_ARGUMENT(1, JSP_TYPE_NUMBER);
		CHECK_ARGUMENT(2, JSP_TYPE_OBJECT);
		CHECK_ARGUMENT(3, JSP_TYPE_NUMBER);

		ObjectInstance *obj = OBJECT_VALUE_OR_THROW(obj, argv[2], ObjectInstance);
		if (obj->getType() != CLASS_CHANNEL)
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);

		time       = argv[0].u.object;
		repeatDays = NUMBER_VALUE(argv[1]);
		channel    = static_cast<Channel*>(obj);
		volume     = NUMBER_VALUE(argv[3]);
	}
	else THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);

	long int win_id = OOIFUtils::getWindow(this_obj);
	jsplugin_obj* context = OOIFUtils::getGlobalContext(win_id);

	apd->retvalPrepareWakeupOITF = FALSE;

	parse_date_userdata *ud = (parse_date_userdata*)HLIB_STD_MemAlloc(sizeof(parse_date_userdata));
	ud->global_context = context;
	ud->apd = apd;
	ud->this_obj   = time;
	ud->repeatDays = repeatDays;
	ud->volume     = volume;

	if (channel && channel->data_wrapper && channel->data_wrapper->data.ccid)
		ud->channelID = HLIB_STD_StrDup(channel->data_wrapper->data.ccid);
	else
		ud->channelID = NULL;

	int res = g_opera_callbacks->get_property(context, time, "getTime", ud, &ApplicationPrivateData::getDateFunctionResultObj);
	if (res != JSP_GET_VALUE)
	{
		OOIF_LOG_DEBUG("parseDate : requesting Date.getTime() failed: %d", res);
		RETURN_BOOLEAN(FALSE, JSP_CALL_VALUE);
	}

	result->type = JSP_TYPE_NULL;
	return JSP_CALL_DELAYED;
}

int ApplicationPrivateData::prepareWakeupTvtvUpdate(jsplugin_obj *this_obj,
        jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv,
        jsplugin_value *result)
{
	ENTRY;

	ApplicationPrivateData *apd = EXTRACT_OBJECT_OR_THROW(apd, this_obj, ApplicationPrivateData);

	if (argc < 0)
	{
		RETURN_BOOLEAN(apd->retvalPrepareWakeupTvtvUpdate, JSP_GET_VALUE);
	}

	jsplugin_obj *	time;
	OOIFBoolean     isTvtvStandbyEnable;

	if (argc == 2)
	{
		if (argv[0].type == JSP_TYPE_NULL)// CANCEL
		{
			OOIFBoolean retval = FALSE;
			NATIVE_CALL(native_AppPrivateDataPrepareWakeupTvtvUpdate(apd->_appId, 0, false, &retval));
			RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
		}
		CHECK_ARGUMENT(0, JSP_TYPE_NATIVE_OBJECT);
		time       = argv[0].u.object;
		isTvtvStandbyEnable = BOOLEAN_VALUE(argv[1]);
	}
	else THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);

	long int win_id = OOIFUtils::getWindow(this_obj);
	jsplugin_obj* context = OOIFUtils::getGlobalContext(win_id);

	apd->retvalPrepareWakeupTvtvUpdate = FALSE;

	parse_date_userdata *ud = (parse_date_userdata*)HLIB_STD_MemAlloc(sizeof(parse_date_userdata));
	ud->global_context = context;
	ud->apd = apd;
	ud->this_obj   = time;
	ud->isTvtvStandbyEnable = isTvtvStandbyEnable;

	int res = g_opera_callbacks->get_property(context, time, "getTime", ud, &ApplicationPrivateData::getTvtvDateFunctionResultObj);
	if (res != JSP_GET_VALUE)
	{
		OOIF_LOG_DEBUG("parseDate : requesting Date.getTime() failed: %d", res);
		RETURN_BOOLEAN(FALSE, JSP_CALL_VALUE);
	}

	result->type = JSP_TYPE_NULL;
	return JSP_CALL_DELAYED;
}
#else
//static
int ApplicationPrivateData::prepareWakeupOITF(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ApplicationPrivateData *apd = EXTRACT_OBJECT_OR_THROW(apd, this_obj, ApplicationPrivateData);

	OOIFBoolean retval;
	NATIVE_CALL(native_AppPrivateDataPrepareWakeupOITF(apd->_appId, STRING_VALUE(argv[0]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
#endif // HMX_WEBUI

//static
int ApplicationPrivateData::clearWakeupToken(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ApplicationPrivateData *apd = EXTRACT_OBJECT_OR_THROW(apd, this_obj, ApplicationPrivateData);

	const char *retval;
	NATIVE_CALL(native_AppPrivateDataClearWakeupToken(apd->_appId, &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}
#endif //OIPF

