// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "jlabsLocalSystem.h"
#include "jlabsNetworkInterface.h"
#include "jlabsNetworkService.h"
#include "jlabsWiFiAccessPoint.h"
#if 1 /* HUMAX API for WiFi configuration */
#include "jlabsNetworkManager.h"
#endif
#include "jlabsMiscSetting.h"
#include "jlabsLocalsystemDiscAttachedEvent.h"
#include "jlabsLocalsystemDiscDetachedEvent.h"
#include "jlabsLocalsystemFinishDiscFormatEvent.h"
#include "jlabsLocalsystemResultPairingEvent.h"
#include "jlabsLocalsystemResidentAppFinishEvent.h"
#include "jlabsLocalsystemDiscLoadFinishedEvent.h"
#include "jlabsLocalsystemSetupInfoInsertFinishedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

using namespace Ooif;

//static
list<jlabsLocalSystem*> jlabsLocalSystem::instances;

jlabsLocalSystem::jlabsLocalSystem()
{
	ENTRY;
	classType = CLASS_JLABS_LOCAL_SYSTEM;	//class_name;
	instances.push_back(this);
}

jlabsLocalSystem::~jlabsLocalSystem()
{
	ENTRY;
	instances.remove(this);
}

int jlabsLocalSystem::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_VOIDP_COLLECTION("networkInterfaces", obj,
								native_jlabsLocalSystemGetNetworkInterfaces(&SIZE, &NATIVE_ARRAY),
								getNetworkInterfaces(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("networkService", obj,
								native_jlabsLocalSystemGetNetworkService(&SIZE, &NATIVE_ARRAY),
								getNetworkService(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("WiFiAccessPoint", obj,
								native_jlabsLocalSystemGetWiFiAccessPoint(&SIZE, &NATIVE_ARRAY),
								getWiFiAccessPoint(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);

	GET_OBJECT("misc", obj, new jlabsMiscSetting(), JSP_GET_VALUE);
	GET_OBJECT("systemInfo", obj, new jlabsSystemInfo(), JSP_GET_VALUE);

	NATIVE_GET_VOIDP_COLLECTION("ppvHistory", obj,
								native_jlabsLocalSystemGetPPVHistory(&SIZE, &NATIVE_ARRAY),
								getPPVHistory(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("scanComponent", obj,
								native_jlabsLocalSystemGetScanComponent(&SIZE, &NATIVE_ARRAY),
								getScanComponent(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);

	GET_FUNCTION(openBrowser, 	obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(ping, 			obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setFactoryReset,  obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(resetUserInfo,  obj, "", JSP_GET_VALUE_CACHE);

	NATIVE_GET_VOIDP_COLLECTION("discs", obj,
								native_jlabsLocalSystemGetStorages(&SIZE, &NATIVE_ARRAY),
								getDiscs(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);

	NATIVE_GET_STRING("defaultRecDisc", native_jlabsLocalSystemGetDefaultRecDisc(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("defaultFactoryForDap", native_jlabsLocalSystemGetDefaultFactoryForDap(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("defaultDiscForDap", native_jlabsLocalSystemGetDefaultDiscForDap(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("softwareDownloadState", native_jlabsLocalSystemGetSoftwareDownloadState(&RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(detachDisc, obj, "ob", JSP_GET_VALUE);
	GET_FUNCTION(formatDisc, obj, "o", JSP_GET_VALUE);

	INTRINSIC_EVENT_GET(FinishDiscFormat);
	INTRINSIC_EVENT_GET(DiscAttached);
	INTRINSIC_EVENT_GET(DiscDetached);
	INTRINSIC_EVENT_GET(DiscLoadFinished);

	GET_FUNCTION(startPairing, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(abortPairing, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(startResidentApp, obj, "ss", JSP_GET_VALUE);
	GET_FUNCTION(ResidentAppFinish, obj, "s", JSP_GET_VALUE);
	GET_FUNCTION(insertSetupInfo, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(loginApp, obj, "sn", JSP_GET_VALUE);
	GET_FUNCTION(logoffApp, obj, "", JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(ResultPairing);
	INTRINSIC_EVENT_GET(ResidentAppFinish);
	INTRINSIC_EVENT_GET(SetupInfoInsertFinished);

	return JSP_GET_NOTFOUND;
}

int jlabsLocalSystem::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(FinishDiscFormat);
	INTRINSIC_EVENT(DiscAttached);
	INTRINSIC_EVENT(DiscDetached);
	INTRINSIC_EVENT(DiscLoadFinished);

	NATIVE_SET("defaultRecDisc", native_jlabsLocalSystemSetDefaultRecDisc(STRING_VALUE(value[0])));
	NATIVE_SET("defaultFactoryForDap", native_jlabsLocalSystemSetDefaultFactoryForDap(NUMBER_VALUE(value[0])));
	NATIVE_SET("defaultDiscForDap", native_jlabsLocalSystemSetDefaultDiscForDap(STRING_VALUE(value[0])));

	INTRINSIC_EVENT(ResultPairing);
	INTRINSIC_EVENT(ResidentAppFinish);
	INTRINSIC_EVENT(SetupInfoInsertFinished);

	return JSP_PUT_NOTFOUND;
}

jlabsNetworkInterfaceCollection *jlabsLocalSystem::getNetworkInterfaces(int count, jlabsNetworkInterfaceHandle  *handles)
{
	ENTRY;
	jlabsNetworkInterfaceCollection *collection = new jlabsNetworkInterfaceCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new jlabsNetworkInterface(handles[i]));
	}

	if( handles )
	{
		HLIB_STD_MemFree(handles);
	}
	return collection;
}

jlabsNetworkServiceCollection *jlabsLocalSystem::getNetworkService(int count, jlabsNetworkServiceHandle *handles)
{
	ENTRY;
	jlabsNetworkServiceCollection *collection = new jlabsNetworkServiceCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new jlabsNetworkService(handles[i]));
	}

	if( handles )
	{
		HLIB_STD_MemFree(handles);
	}
	return collection;
}

jlabsWiFiAccessPointCollection *jlabsLocalSystem::getWiFiAccessPoint(int count, jlabsWiFiAccessPointHandle *handles)
{
	ENTRY;
	jlabsWiFiAccessPointCollection *collection = new jlabsWiFiAccessPointCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new jlabsWiFiAccessPoint(handles[i]));
	}

	if( handles )
	{
		HLIB_STD_MemFree(handles);
	}
	return collection;
}

jlabsPPVHistoryCollection *jlabsLocalSystem::getPPVHistory(int count, jlabsPPVHistoryHandle *handles)
{
	ENTRY;
	jlabsPPVHistoryCollection *collection = new jlabsPPVHistoryCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new jlabsPPVHistory(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}
jlabsScanningCollection	*jlabsLocalSystem::getScanComponent(int count, jlabsScanningHandle *handles)
{
	ENTRY;
	jlabsScanningCollection	*collection = new jlabsScanningCollection;
	stNativeJlabsLocalSystem_ScanSetting_t 	*pstSetting;

	for (int i=0; i<count; i++)
	{
		pstSetting = (stNativeJlabsLocalSystem_ScanSetting_t*)handles[i];
		collection->addItem(new jlabsScanning(handles[i]));
	}
	HLIB_STD_MemFree(handles);

	return collection;
}

//// Method

int jlabsLocalSystem::openBrowser(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);
	CHECK_ARGUMENT(1, JSP_TYPE_NUMBER);

	OOIF_LOG_PRINT("this is jlabsLocalSystem::openBrowser  function.\n");

	OOIFBoolean retval;
	NATIVE_CALL(native_LocalSystemSetScreenSize(NUMBER_VALUE(argv[0]),
												NUMBER_VALUE(argv[1]),
												&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsLocalSystem::ping(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);
	CHECK_ARGUMENT(1, JSP_TYPE_NUMBER);

	OOIF_LOG_PRINT("this is jlabsLocalSystem::ping function.\n");

	OOIFBoolean retval;
	NATIVE_CALL(native_LocalSystemSetScreenSize(NUMBER_VALUE(argv[0]),
												NUMBER_VALUE(argv[1]),
												&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsLocalSystem::setFactoryReset(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval;
	NATIVE_CALL(native_jlabsLocalSystemSetFactoryReset(&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int jlabsLocalSystem::resetUserInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval;

	NATIVE_CALL(native_jlabsLocalSystemResetUserInfo(&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

HDiscInfoCollection	*jlabsLocalSystem::getDiscs(int count, HDiscInfoHandle *handles)
{
	ENTRY;
	HDiscInfoCollection	*collection = new HDiscInfoCollection;

	for (int i=0; i<count; i++)
	{
		collection->addItem(new HDiscInfo(handles[i]));
	}
	HLIB_STD_MemFree(handles);

	return collection;
}

void jlabsLocalSystem::onJlabsLocalSystemEvent(JlabsLocalsystemEvent event, JlabsLocalsystemEventInfo* info)
{
	HBOOL	bConnection= FALSE;

	ENTRY;
	// Iterate over all instances and dispatch the event
	for (list<jlabsLocalSystem*>::iterator it=instances.begin(); it!=instances.end(); it++) {
		Event *e;
		jsplugin_obj *context = (*it)->getHost();
		switch (event) {
		case		JLABS_LOCALSYSTEM_EVENT_FINISH_DISC_FORMAT:
		{
			OOIF_LOG_DEBUG("JLABS_LOCALSYSTEM_EVENT_FINISH_DISC_FORMAT");
			e = new JlabsFinishDiscFormatEvent(context, info->format_data.handle, info->format_data.state);
			break;
		}
		case		JLABS_LOCALSYSTEM_EVENT_DISC_ATTACHED:
		{
			OOIF_LOG_DEBUG("JLABS_LOCALSYSTEM_EVENT_DISC_ATTACHED");
			e = new JlabsDiscAttachedEvent(context, info->attach_data.handle);
			break;
		}
		case		JLABS_LOCALSYSTEM_EVENT_DISC_DETACHED:
		{
			OOIF_LOG_DEBUG("JLABS_LOCALSYSTEM_EVENT_DISC_DETACHED");
			e = new JlabsDiscDetachedEvent(context, info->detach_data.discId, info->detach_data.discName);
			bConnection = TRUE;
			break;
		}
		case		JLABS_LOCALSYSTEM_EVENT_RESULT_PAIRING:
		{
			OOIF_LOG_DEBUG("JLABS_LOCALSYSTEM_EVENT_RESULT_PAIRING");
			e = new JlabsResultPairingEvent(context, info->result_pairing_data.result);
			break;
		}
		case		JLABS_LOCALSYSTEM_EVENT_RESIDENT_APP_FINISH:
		{
			OOIF_LOG_DEBUG("JLABS_LOCALSYSTEM_EVENT_ResidentAppFinish");
			e = new JlabsResidentAppFinishEvent(context, info->resident_app_finish_data.appName);
			break;
		}
		case		JLABS_LOCALSYSTEM_EVENT_DISC_LOAD_FINISHED:
		{
			OOIF_LOG_DEBUG("JLABS_LOCALSYSTEM_EVENT_DISC_SCAN_FINISH");
			e = new JlabsDiscLoadFinishedEvent(context, info->scan_data.handle);
			break;
		}
		case		JLABS_LOCALSYSTEM_EVENT_INSERT_SETUP_FINISHED:
		{
			OOIF_LOG_DEBUG("JLABS_LOCALSYSTEM_EVENT_INSERT_SETUP_FINISHED");
			e = new jlabsSetupInfoInsertFinishedEvent(context, info->insertResult_data.errorNumber);
			break;
		}
		default:
			OOIF_LOG_WARNING("Unknown event received");
			return;
		}
		(*it)->dispatchEvent(e);
	}

	// Free
	if(bConnection)
	{
		HLIB_STD_MemFree(info->detach_data.discId);
		HLIB_STD_MemFree(info->detach_data.discName);
	}
}

int jlabsLocalSystem::formatDisc(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	HDiscInfoHandle		info;
	HDiscInfo *d = OBJECT_VALUE_OR_THROW(d, argv[0], HDiscInfo);
	OOIFBoolean retval;

	info = d->getIdentifier();

	NATIVE_CALL(native_jlabsLocalSystemFormatDisc(info, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsLocalSystem::detachDisc(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	HDiscInfoHandle		info;
	HDiscInfo *d = OBJECT_VALUE_OR_THROW(d, argv[0], HDiscInfo);
	OOIFBoolean retval;

	CHECK_ARGUMENT(1, JSP_TYPE_BOOLEAN);	// force

	info = d->getIdentifier();
	NATIVE_CALL(native_jlabsLocalSystemDetachDisc(info, BOOLEAN_VALUE(argv[1]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


int jlabsLocalSystem::startPairing(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;

	NATIVE_CALL(native_jlabsLocalSystemStartPairing(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsLocalSystem::abortPairing(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;

	NATIVE_CALL(native_jlabsLocalSystemAbortPairing(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsLocalSystem::startResidentApp(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	OOIFBoolean retval;

	NATIVE_CALL(native_jlabsLocalSystemStartResidentApp(STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsLocalSystem::ResidentAppFinish(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	OOIFBoolean retval;

	NATIVE_CALL(native_jlabsLocalSystemResidentAppFinish(STRING_VALUE(argv[0]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsLocalSystem::insertSetupInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	OOIFNumber retval;

	NATIVE_CALL(native_jlabsLocalSystemInsertSetupInfo(&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int jlabsLocalSystem::loginApp(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	OOIFBoolean retval;

	NATIVE_CALL(native_jlabsLocalSystemLoginApp(STRING_VALUE(argv[0]), NUMBER_VALUE(argv[1]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsLocalSystem::logoffApp(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;

	NATIVE_CALL(native_jlabsLocalSystemLogoffApp(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


#endif // OIPF

