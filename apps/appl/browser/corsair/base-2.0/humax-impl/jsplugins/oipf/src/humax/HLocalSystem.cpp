// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "HLocalSystem.h"

#include "EmptyEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "NativeHConfiguration.h"
#include "Exception.h"
#include "HNetworkInterface.h"
#include "AVOutput.h"
#include "Tuner.h"
#include "PowerStateChangeEvent.h"

#if defined(HMX_WEBUI)
#include "humax/HMX_Output.h"
#include "humax/HMX_Audio.h"
#include "AntennaInfoCollection.h"
#include "HCamStateChangeEvent.h"
#include "HSmartcardStateChangedEvent.h"
#include "HFrontPanel.h"
#include "HSoftwareUpdate.h"
#include "HChannelImportExport.h"
#include "HColdBootDone.h"
#include "HCasActionIrEmmForcedOta.h"
#include "HLcnUpdate.h"
#include "HClockRecovery.h"
#include "HUserActionIdleEvent.h"
#include "HKeyboardResultEvent.h"
#include "HMXAttachedMicrophoneEvent.h"
#include "HMXDetachedMicrophoneEvent.h"
#include "HTrdConflict.h"
#include "HUsbWirelessStateChangedEvent.h"

#endif // HMX_WEBUI

using namespace Ooif;

//static
list<HLocalSystem*> HLocalSystem::instances;

HLocalSystem::HLocalSystem()
{
	ENTRY;
	classType = class_name;
	HLocalSystem::instances.push_back(this);
}

HLocalSystem::~HLocalSystem()
{
	ENTRY;
	HLocalSystem::instances.remove(this);
}



int HLocalSystem::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

#if defined(HMX_SECURITY)
	NATIVE_SECURITY_CHECK(obj);
#endif

	GET_NUMBER("OFF",	0, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ON",	1, JSP_GET_VALUE_CACHE);
	GET_NUMBER("PASSIVE_STANDBY",	2, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ACTIVE_STANDBY",	3, JSP_GET_VALUE_CACHE);
	GET_NUMBER("PASSIVE_STANDBY_HIBERATE",	4, JSP_GET_VALUE_CACHE);

	NATIVE_GET_STRING("deviceID", native_HLocalSystemGetDeviceID(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_BOOLEAN("systemReady", native_HLocalSystemGetSystemReady(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("vendorName", native_HLocalSystemGetVendorName(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("modelName", native_HLocalSystemGetModelName(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("updatedList", native_HLocalSystemGetUpdatedList(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("softwareVersion", native_HLocalSystemGetSoftwareVersion(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("packageVersion", native_HLocalSystemGetPackageVersion(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("hardwareVersion", native_HLocalSystemGetHardwareVersion(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("serialNumber", native_HLocalSystemGetSerialNumber(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("pvrEnabled", native_HLocalSystemGetPvrEnabled(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("standbyState", native_HLocalSystemGetStandbyState(&RETVAL), JSP_GET_VALUE);

	NATIVE_GET_NUMBER("releaseVersion", native_HLocalSystemGetReleaseVersion(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("majorVersion", native_HLocalSystemGetMajorVersion(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("minorVersion", native_HLocalSystemGetMinorVersion(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("oipfProfile", native_HLocalSystemGetOipfProfile(&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_BOOLEAN("ciplusEnabled", native_HLocalSystemGetCiplusEnabled(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("powerState", native_HLocalSystemGetPowerState(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("previousPowerState", native_HLocalSystemGetPreviousPowerState(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("timeCurrentPowerState", native_HLocalSystemGetTimeCurrentPowerState(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("chipId", native_HLocalSystemGetChipId(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("hcrdata", native_HLocalSystemGetHCRData(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("checkHcrData", native_HLocalSystemCheckHCRData(&RETVAL), JSP_GET_VALUE);


	NATIVE_GET_NUMBER("volume", native_HLocalSystemGetVolume(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("mute", native_HLocalSystemGetMute(&RETVAL), JSP_GET_VALUE);
	// HUMAX EXTENSION will not be used!!!!!!
#if 0
	NATIVE_GET_VOIDP_COLLECTION("outputs", obj,
								native_HLocalSystemGetOutputs(&SIZE, &NATIVE_ARRAY),
								getAVOutputs(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
#endif
	NATIVE_GET_VOIDP_COLLECTION("networkInterfaces", obj,
								native_HLocalSystemGetNetworkInterfaces(&SIZE, &NATIVE_ARRAY),
								getNetworkInterfaces(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("tuners", obj,
								native_HLocalSystemGetTuners(&SIZE, &NATIVE_ARRAY),
								getTuners(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("tvStandard", native_HLocalSystemGetTvStandard(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrSupport", native_HLocalSystemGetPvrSupport(&RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(setApplicationReady, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setScreenSize, obj, "nn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setPvrSupport, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setPowerState, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setDigestCredentials, obj, "ssss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(clearDigestCredentials, obj, "ss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(showMsgbox, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(showKeyboard, obj, "", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(PowerStateChange);
	INTRINSIC_EVENT_GET(UserActionIdle);
	INTRINSIC_EVENT_GET(KeyboardResult);

#if defined(HMX_WEBUI)
	NATIVE_GET_VOIDP_COLLECTION("hmx_outputs", obj,
								native_HLocalSystemGetHmxOutputs(&SIZE, &NATIVE_ARRAY),
								getHmxOutputs(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("hmx_audios", obj,
								native_HLocalSystemGetHmxAudios(&SIZE, &NATIVE_ARRAY),
								getHmxAudios(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);

	NATIVE_GET_BOOLEAN("boxFirstBootStatus",
								native_HLocalSystemGetBoxFirstBootStatus(&RETVAL), JSP_GET_VALUE);

	NATIVE_GET_NUMBER("boxLaunchAppMode",
								native_HLocalSystemGetLaunchAppMode(&RETVAL), JSP_GET_VALUE);

	NATIVE_GET_BOOLEAN("boxAntennaPowerStatus",
								native_HLocalSystemGetAntennaPowerStatus(&RETVAL), JSP_GET_VALUE);

	NATIVE_GET_BOOLEAN("boxServiceUpdateFlag",
								native_HLocalSystemGetServiceUpdateFlag(&RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(getField, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setField, obj, "ss", JSP_GET_VALUE_CACHE);

	GET_OBJECT("networkManager", obj, new HMX_NetworkManager(), JSP_GET_VALUE );
	GET_OBJECT("frontPanel", obj, new HFrontPanel(), JSP_GET_VALUE);
	GET_OBJECT("swupdate", obj, new HSoftwareUpdate(), JSP_GET_VALUE);
	GET_OBJECT("channelImportExport", obj, new HChannelImportExport(), JSP_GET_VALUE);
	GET_OBJECT("coldBootDone", obj, new HColdBootDone(), JSP_GET_VALUE);
	GET_OBJECT("casactionIrEmmForcedOta", obj, new HCasActionIrEmmForcedOta(), JSP_GET_VALUE);
	GET_OBJECT("lcnupdate", obj, new HLcnUpdate(), JSP_GET_VALUE);
	GET_OBJECT("clockRecovery", obj, new HClockRecovery(), JSP_GET_VALUE);
	GET_OBJECT("trdconflict", obj, new HTrdConflict(), JSP_GET_VALUE);

	if (strcmp(name, "antennaInfoLists") == 0)
	{
		RETURN_OBJECT(obj, new AntennaInfoCollection(), result, JSP_GET_VALUE);
	}

	INTRINSIC_EVENT_GET(CamStateChange);
	INTRINSIC_EVENT_GET(SmartcardStateChanged);
	INTRINSIC_EVENT_GET(ThermalProtectionOccured);

	GET_FUNCTION(registerEventListener, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unregisterEventListener, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(checkFileExistence, obj, "s", JSP_GET_VALUE);
	NATIVE_GET_NUMBER("microphoneVolume", native_HLocalSystemGetMicrophoneVolume(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("microphoneEnabled", native_HLocalSystemGetMicrophoneEnabled(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("connectedMicrophone", native_HLocalSystemGetConnectedMicrophone(&RETVAL), JSP_GET_VALUE);
   	NATIVE_GET_NUMBER("automaticPowerdown", native_HLocalSystemGetAutomaticPowerdown(&RETVAL), JSP_GET_VALUE);

    NATIVE_GET_NUMBER("priorityOutputs",native_HLocalSystemGetPriorityOutputs(&RETVAL), JSP_GET_VALUE);

	INTRINSIC_EVENT_GET(AttachedMicrophone);
	INTRINSIC_EVENT_GET(DetachedMicrophone);
	INTRINSIC_EVENT_GET(UsbWirelessStateChanged);
#endif
	GET_FUNCTION(startTVBrowser, obj, "s", JSP_GET_VALUE);
	GET_FUNCTION(getStatusOfTVBrowser, obj, "", JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HLocalSystem::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	NATIVE_SET("standbyState", native_HLocalSystemSetStandbyState(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("volume", native_HLocalSystemSetVolume(NUMBER_VALUE(value[0])));
	NATIVE_SET("mute", native_HLocalSystemSetMute(BOOLEAN_VALUE(value[0])));

	INTRINSIC_EVENT(PowerStateChange);
	INTRINSIC_EVENT(UserActionIdle);
	INTRINSIC_EVENT(KeyboardResult);

#if defined(HMX_WEBUI)
	INTRINSIC_EVENT(AttachedMicrophone);
	INTRINSIC_EVENT(DetachedMicrophone);
	INTRINSIC_EVENT(CamStateChange);
	INTRINSIC_EVENT(SmartcardStateChanged);
	INTRINSIC_EVENT(UsbWirelessStateChanged);
	INTRINSIC_EVENT(ThermalProtectionOccured);
	NATIVE_SET("boxFirstBootStatus", native_HLocalSystemSetBoxFirstBootStatus(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("boxAntennaPowerStatus", native_HLocalSystemSetAntennaPowerStatus(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("boxServiceUpdateFlag", native_HLocalSystemSetServiceUpdateFlag(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("microphoneVolume", native_HLocalSystemSetMicrophoneVolume(NUMBER_VALUE(value[0])));
	NATIVE_SET("microphoneEnabled", native_HLocalSystemSetMicrophoneEnabled(BOOLEAN_VALUE(value[0])));
    NATIVE_SET("automaticPowerdown", native_HLocalSystemSetAutomaticPowerdown(NUMBER_VALUE(value[0])));
    NATIVE_SET("priorityOutputs", native_HLocalSystemSetPriorityOutputs(NUMBER_VALUE(value[0])));
#endif // HMX_WEBUI

	return JSP_PUT_NOTFOUND;
}

//static
int HLocalSystem::setScreenSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);
	CHECK_ARGUMENT(1, JSP_TYPE_NUMBER);

	OOIFBoolean retval;
	NATIVE_CALL(native_HLocalSystemSetScreenSize(NUMBER_VALUE(argv[0]),
												NUMBER_VALUE(argv[1]),
												&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int HLocalSystem::setPvrSupport(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);

	OOIFNumber retval;
	NATIVE_CALL(native_HLocalSystemSetPvrSupport(NUMBER_VALUE(argv[0]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int	HLocalSystem::setPowerState(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);

	OOIFBoolean retval;
	NATIVE_CALL(native_HLocalSystemSetPowerState(NUMBER_VALUE(argv[0]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

HNetworkInterfaceCollection *HLocalSystem::getNetworkInterfaces(int count, NetworkInterfaceHandle *handles)
{
	ENTRY;

	HNetworkInterfaceCollection *collection = new HNetworkInterfaceCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new HNetworkInterface(handles[i]));
	}
	delete []handles;
	return collection;
}

AVOutputCollection *HLocalSystem::getAVOutputs(int count, AVOutputHandle *handles)
{
	ENTRY;
	AVOutputCollection *collection = new AVOutputCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new AVOutput(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}


TunerCollection *HLocalSystem::getTuners(int count, TunerHandle *handles)
{
	ENTRY;
	TunerCollection *collection = new TunerCollection;

	for (int i = 0; i < count; i++)
	{
		collection->addItem(new Tuner(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

//static
int HLocalSystem::setApplicationReady(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;
	NATIVE_CALL(native_LocalSystemSetApplicationReady(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int HLocalSystem::setDigestCredentials(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
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
int HLocalSystem::clearDigestCredentials(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
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
void HLocalSystem::onLocalSystemEvent(HLocalSystemEvent event, HLocalSystemEventInfo* info)
{
	ENTRY;

	// Iterate over all instances and dispatch the event
	for (list<HLocalSystem*>::iterator it=instances.begin(); it!=instances.end(); it++) {
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();

            if(context == NULL)
                continue;
		switch (event) {
		case HLOCAL_SYSTEM_EVENT_POWER_STATE_CHANGE:
		{
#if 1
			fflush(NULL);
			OOIF_LOG_PRINT("\n\n\n\n\n[%s:%d] HLOCAL_SYSTEM_EVENT_POWER_STATE_CHANGE\n\n\n\n\n",__FUNCTION__,__LINE__);
			fflush(NULL);
#endif

			OOIF_LOG_DEBUG("PowerStateChange event received\n");
			OOIFNumber state = info->power_state_change_data.powerState;
			e = new PowerStateChangeEvent(context, state);
			break;
		}
#if defined(HMX_WEBUI)
		case HLOCAL_SYSTEM_EVENT_CAM_STATE_CHANGE:
			OOIF_LOG_DEBUG("CamStateChange event received\n");
			e = new HCamStateChangeEvent(context, info->cam_state_change_data.slotNumber, info->cam_state_change_data.camState);
			break;

		case HLOCAL_SYSTEM_EVENT_SC_STATE_CHANGED:
			OOIF_LOG_DEBUG("SmartcardStateChanged event received\n");
			e = new HSmartcardStateChangedEvent(context, info->sc_state_change_data.slotNumber, info->sc_state_change_data.scState);
			break;
		case HLOCAL_SYSTEM_EVENT_MIC_ATTACHED:
			OOIF_LOG_DEBUG("AttachedMicrophone event received\n");
			e = new HMXAttachedMicrophoneEvent(context, info->ext_dev_info_data.devId);
			break;
		case HLOCAL_SYSTEM_EVENT_MIC_DETACHED:
			OOIF_LOG_DEBUG("DetachedMicrophone event received\n");
			e = new HMXDetachedMicrophoneEvent(context, info->ext_dev_info_data.devId);
			break;
		case HLOCAL_SYSTEM_EVENT_USB_WIRELESS_STATE_CHANGE:
			OOIF_LOG_DEBUG("WirelessStateChange event received\n");
			e = new HUsbWirelessStateChangedEvent(context, info->ext_dev_info_data.devId, info->ext_dev_info_data.devState);
			break;
		case HLOCAL_SYSTEM_EVENT_THERMAL_PROTECTION_OCCURED:
			OOIF_LOG_DEBUG("ThermalProtectionOccured event received");
			e = new EmptyEvent(context, "ThermalProtectionOccured");
			break;
#endif

		case HLOCAL_SYSTEM_EVENT_USER_ACTION_IDLE:
			OOIF_LOG_DEBUG("UserActionIdle event received\n");
			e = new HUserActionIdleEvent(context);
			break;

		case HLOCAL_SYSTEM_EVENT_KEYBOARD_RESULT:
			OOIF_LOG_DEBUG("KeyboardResult event received\n");
			e = new HKeyboardResultEvent(context, info->keyboard_result_data.resultStr);
			break;

		default:
			OOIF_LOG_WARNING("Unknown event received\n");
			return;
		}
		(*it)->dispatchEvent(e);
	}
}

OOIFReturnCode HLocalSystem::checkListenerAccessAllowed(const char *eventName)
{
	jsplugin_obj *this_obj = getHost();
	char *host = OOIFUtils::getHost(this_obj);
	long int window = OOIFUtils::getWindow(this_obj);
	OOIFReturnCode ret = native_LocalSystemOnListenerAccessed(window, host, eventName);
	return ret;
}

//static
int HLocalSystem::showMsgbox(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	OOIFBoolean retval;
	NATIVE_CALL(native_HLocalSystemShowMsgbox(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int HLocalSystem::showKeyboard(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(3);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);
	CHECK_ARGUMENT(1, JSP_TYPE_BOOLEAN);
	CHECK_ARGUMENT(2, JSP_TYPE_STRING);

	OOIFBoolean retval;
	NATIVE_CALL(native_HLocalSystemShowKeyboard(&retval, NUMBER_VALUE(argv[0]), BOOLEAN_VALUE(argv[1]), STRING_VALUE(argv[2])));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


#if defined(HMX_WEBUI)
HMX_OutputCollection *HLocalSystem::getHmxOutputs(int count, HMX_OutputHandle *handles)
{
	HMX_OutputCollection *collection = new HMX_OutputCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new HMX_Output(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

HMX_AudioCollection *HLocalSystem::getHmxAudios(int count, HMX_AudioHandle *handles)
{
	HMX_AudioCollection *collection = new HMX_AudioCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new HMX_Audio(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

//static
int HLocalSystem::getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	const char *retval;
	NATIVE_CALL(native_HLocalSystemGetField(STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int HLocalSystem::setField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	NATIVE_CALL(native_HLocalSystemSetField(STRING_VALUE(argv[0]), STRING_VALUE(argv[1])));
	return JSP_CALL_NO_VALUE;
}

//static
int HLocalSystem::registerEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	NATIVE_WARN_NOK(native_HLocalSystemSetListener(HLocalSystem::onLocalSystemEvent));
	return JSP_CALL_NO_VALUE;
}

//static
int HLocalSystem::unregisterEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	NATIVE_WARN_NOK(native_HLocalSystemUnsetListener());
	return JSP_CALL_NO_VALUE;
}

// static
int HLocalSystem::checkFileExistence(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval;
	NATIVE_CALL(native_HLocalSystemCheckFileExistence(STRING_VALUE(argv[0]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

#endif // #if defined(HMX_WEBUI)


// static
int HLocalSystem::startTVBrowser(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	OOIFBoolean retval = true;

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

// static
int HLocalSystem::getStatusOfTVBrowser(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval = false;

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

#endif // OIPF
