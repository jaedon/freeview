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
	NATIVE_GET_OBJECT("currentChannel", obj, hmx_AppPrivateDataGetCurrentChannel(_appId, &RETVAL), new Channel(RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(getFreeMem, obj, "", JSP_GET_VALUE_CACHE);

#ifdef OIPF
	NATIVE_GET_BOOLEAN("wakeupApplication", native_AppPrivateDataGetWakeupApplication(_appId, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("wakeupOITF", native_AppPrivateDataGetWakeupOITF(_appId, &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(prepareWakeupApplication, obj, "ss-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(prepareWakeupOITF, obj, "-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(clearWakeupToken, obj, "", JSP_GET_VALUE_CACHE);
#endif // OIPF

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

//static
int ApplicationPrivateData::prepareWakeupOITF(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ApplicationPrivateData *apd = EXTRACT_OBJECT_OR_THROW(apd, this_obj, ApplicationPrivateData);

	OOIFBoolean retval;
	NATIVE_CALL(native_AppPrivateDataPrepareWakeupOITF(apd->_appId, STRING_VALUE(argv[0]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int ApplicationPrivateData::clearWakeupToken(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ApplicationPrivateData *apd = EXTRACT_OBJECT_OR_THROW(apd, this_obj, ApplicationPrivateData);

	const char *retval;
	NATIVE_CALL(native_AppPrivateDataClearWakeupToken(apd->_appId, &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}
#endif //OIPF
