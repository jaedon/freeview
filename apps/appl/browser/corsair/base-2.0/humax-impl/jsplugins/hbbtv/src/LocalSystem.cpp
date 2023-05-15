// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/** 
 * @ToDo originally, localSystem property is not supported for HbbTV requirement. 
   but the Juke Box application require the property to login, so we remove OIPF condition.
   so I marked which code are fixed due to above issue with JUKEBOX SUPPORT as a comment.
 */
//#ifdef OIPF //JUKEBOX_SUPPORT
#include "LocalSystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "NativeConfiguration.h"
#include "Exception.h"
#include "NetworkInterface.h"
#include "AVOutput.h"
#include "Tuner.h"
#include "PowerStateChangeEvent.h"

using namespace Ooif;

//static
list<LocalSystem*> LocalSystem::instances;

LocalSystem::LocalSystem()
{
	ENTRY;
	classType = class_name;
	LocalSystem::instances.push_back(this);
}

LocalSystem::~LocalSystem()
{
	ENTRY;
	LocalSystem::instances.remove(this);
}

int LocalSystem::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV //JUKEBOX_SUPPORT
	NATIVE_GET_STRING("deviceID", native_LocalSystemGetDeviceID(&RETVAL), JSP_GET_VALUE_CACHE);
#else
	GET_NUMBER("OFF", STATE_OFF, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ON", STATE_ON, JSP_GET_VALUE_CACHE);
	GET_NUMBER("PASSIVE_STANDBY", STATE_PASSIVE_STANDBY, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ACTIVE_STANDBY", STATE_ACTIVE_STANDBY, JSP_GET_VALUE_CACHE);
	GET_NUMBER("PASSIVE_STANDBY_HIBERNATE", STATE_PASSIVE_STANDBY_HIBERNATE, JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("deviceID", native_LocalSystemGetDeviceID(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_BOOLEAN("systemReady", native_LocalSystemGetSystemReady(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("vendorName", native_LocalSystemGetVendorName(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("modelName", native_LocalSystemGetModelName(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("softwareVersion", native_LocalSystemGetSoftwareVersion(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("hardwareVersion", native_LocalSystemGetHardwareVersion(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("serialNumber", native_LocalSystemGetSerialNumber(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("releaseVersion", native_LocalSystemGetReleaseVersion(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("majorVersion", native_LocalSystemGetMajorVersion(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("minorVersion", native_LocalSystemGetMinorVersion(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("oipfProfile", native_LocalSystemGetOipfProfile(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_BOOLEAN("pvrEnabled", native_LocalSystemGetPvrEnabled(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("ciplusEnabled", native_LocalSystemGetCiplusEnabled(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("standbyState", native_LocalSystemGetStandbyState(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("powerState", native_LocalSystemGetPowerState(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("previousPowerState", native_LocalSystemGetPreviousPowerState(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("timeCurrentPowerState", native_LocalSystemGetTimeCurrentPowerState(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("volume", native_LocalSystemGetVolume(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("mute", native_LocalSystemGetMute(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("outputs", obj,
	                            native_LocalSystemGetOutputs(&SIZE, &NATIVE_ARRAY),
	                            getAVOutputs(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("networkInterfaces", obj,
	                            native_LocalSystemGetNetworkInterfaces(&SIZE, &NATIVE_ARRAY),
	                            getNetworkInterfaces(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("tuners", obj,
	                            native_LocalSystemGetTuners(OOIFUtils::getWindow(obj), OOIFUtils::getHost(obj), &SIZE, &NATIVE_ARRAY),
	                            getTuners(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("tvStandard", native_LocalSystemGetTvStandard(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrSupport", native_LocalSystemGetPvrSupport(&RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(setScreenSize, obj, "nn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setPvrSupport, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setPowerState, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setDigestCredentials, obj, "ssss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(clearDigestCredentials, obj, "ss", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(PowerStateChange);
#endif
	return EventTarget::getter(obj, name, result);
}

int LocalSystem::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
#ifdef HBBTV //JUKEBOX_SUPPORT
	SET_READONLY("deviceID");
#else
	NATIVE_SET("standbyState", native_LocalSystemSetStandbyState(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("volume", native_LocalSystemSetVolume(NUMBER_VALUE(value[0])));
	NATIVE_SET("mute", native_LocalSystemSetMute(BOOLEAN_VALUE(value[0])));

	SET_READONLY("deviceID");
	SET_READONLY("systemReady");
	SET_READONLY("vendorName");
	SET_READONLY("modelName");
	SET_READONLY("softwareVersion");
	SET_READONLY("hardwareVersion");
	SET_READONLY("serialNumber");
	SET_READONLY("releaseVersion");
	SET_READONLY("majorVersion");
	SET_READONLY("minorVersion");
	SET_READONLY("oipfProfile");
	SET_READONLY("pvrEnabled");
	SET_READONLY("ciplusEnabled");
	SET_READONLY("powerState");
	SET_READONLY("previousPowerState");
	SET_READONLY("timeCurrentPowerState");
	SET_READONLY("outputs");
	SET_READONLY("networkInterfaces");
	SET_READONLY("tuners");
	SET_READONLY("tvStandard");
	SET_READONLY("pvrSupport");

	INTRINSIC_EVENT(PowerStateChange);
#endif
	return JSP_PUT_NOTFOUND;
}
#ifdef OIPF //JUKEBOX_SUPPORT
//static
int LocalSystem::setScreenSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);
	CHECK_ARGUMENT(1, JSP_TYPE_NUMBER);

	OOIFBoolean retval;
	NATIVE_CALL(native_LocalSystemSetScreenSize(NUMBER_VALUE(argv[0]),
	            NUMBER_VALUE(argv[1]),
	            &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int LocalSystem::setPvrSupport(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);

	OOIFNumber retval;
	NATIVE_CALL(native_LocalSystemSetPvrSupport(NUMBER_VALUE(argv[0]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int LocalSystem::setPowerState(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);

	OOIFBoolean retval;
	NATIVE_CALL(native_LocalSystemSetPowerState(NUMBER_VALUE(argv[0]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

NetworkInterfaceCollection *LocalSystem::getNetworkInterfaces(int count, NetworkInterfaceHandle *handles)
{
	ENTRY;
	NetworkInterfaceCollection *collection = new NetworkInterfaceCollection;
	for (int i=0; i<count; i++)
	{
		collection->addItem(new NetworkInterface(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

AVOutputCollection *LocalSystem::getAVOutputs(int count, AVOutputHandle *handles)
{
	ENTRY;
	AVOutputCollection *collection = new AVOutputCollection;
	for (int i=0; i<count; i++)
	{
		collection->addItem(new AVOutput(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

TunerCollection *LocalSystem::getTuners(int count, TunerHandle *handles)
{
	ENTRY;
	TunerCollection *collection = new TunerCollection;
	for (int i=0; i<count; i++)
	{
		collection->addItem(new Tuner(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

//static
int LocalSystem::setDigestCredentials(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(4);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	CHECK_ARGUMENT(1, JSP_TYPE_STRING);
	CHECK_ARGUMENT(2, JSP_TYPE_STRING);
	CHECK_ARGUMENT(3, JSP_TYPE_STRING);

	OOIFBoolean retval;
	NATIVE_CALL(native_LocalSystemSetDigestCredentials(STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), STRING_VALUE(argv[2]), STRING_VALUE(argv[3]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int LocalSystem::clearDigestCredentials(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	CHECK_ARGUMENT(1, JSP_TYPE_STRING);

	OOIFBoolean retval;
	NATIVE_CALL(native_LocalSystemClearDigestCredentials(STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
void LocalSystem::onLocalSystemEvent(LocalSystemEvent event, LocalSystemEventInfo* info)
{
	ENTRY;
	// Iterate over all instances and dispatch the event
	for (list<LocalSystem*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();
		switch (event)
		{
		case LOCAL_SYSTEM_EVENT_POWER_STATE_CHANGE:
		{
			OOIF_LOG_DEBUG("PowerStateChange event received");
			OOIFNumber state = info->power_state_change_data.powerState;
			e = new PowerStateChangeEvent(context, state);
			break;
		}
		default:
			OOIF_LOG_WARNING("Unknown event received");
			return;
		}
		(*it)->dispatchEvent(e);
	}
}

OOIFReturnCode LocalSystem::checkListenerAccessAllowed(const char *eventName)
{
	jsplugin_obj *this_obj = getHost();
	char *host = OOIFUtils::getHost(this_obj);
	long int window = OOIFUtils::getWindow(this_obj);
	OOIFReturnCode ret = native_LocalSystemOnListenerAccessed(window, host, eventName);
	return ret;
}
#endif //OIPF //JUKEBOX_SUPPORT line 137
//#endif // OIPF //JUKEBOX_SUPPORT
