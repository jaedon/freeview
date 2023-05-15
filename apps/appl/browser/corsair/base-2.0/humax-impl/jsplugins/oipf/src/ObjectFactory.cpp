// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ObjectFactory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "Exception.h"

#include "DrmAgent.h"
#include "NativeDrmAgent.h"
#include "MasterConfiguration.h"
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
#include "DownloadTrigger.h"
#include "DownloadManager.h"
#endif // HBBTV_DOWNLOAD || OIPF
#include "ApplicationManager.h"
#include "Capabilities.h"
#include "ParentalControlManager.h"
#if defined(HBBTV_PVR) || defined(OIPF)
#include "RecordingScheduler.h"
#endif // HBBTV_PVR || OIPF
#ifdef OIPF
#include "ChannelConfig.h"
#endif // OIPF
#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "SearchManager.h"
#endif // OIPF || HBBTV_1_2_1
#include "VideoOnDemand.h"
#include "NativeVideoOnDemand.h"
#include "VideoBroadcast.h"
#include "NativeVideoBroadcast.h"

#if defined(HMX_WEBUI)
//#include "humax/HMX_Media.h"
#include "humax/HMasterConfiguration.h"
#include "humax/HMXStorageUtil.h"
#include "humax/HCasUi.h"
#include "humax/HMXOperatorFeature.h"
#include "humax/HMXOperatorFreesat.h"
#include "humax/HBluetoothManager.h"
#include "humax/HMXApplicationManager.h"

#include "humax/HDlna.h"
#include "humax/HWoon.h"
#include "humax/HContentManager.h"
#include "humax/HSamba.h"
#include "humax/HFtpManager.h"
#include "humax/HDial.h"
#include "humax/HSatip.h"
#include "humax/HAirplay.h" 
#include "humax/HAlexaHelper.h" 

#if defined(USE_VIDEOHANDLE)
#include "humax/HVideoBroadcast.h"
#include "humax/HVideoOnDemand.h"
#endif
#endif

#ifdef JLABS
#include "jlabsEmmMessageManager.h"
#include "jlabsCDSSearcher.h"
#include "NativeJlabsCDSSearcher.h"
#include "jlabsRemoteControlFunction.h"
#include "jlabsDMRController.h"
#endif // JLABS

using namespace Ooif;

int OipfObjectFactory::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(isObjectSupported, obj, "s", JSP_GET_VALUE);

	GET_FUNCTION(createApplicationManagerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createCapabilitiesObject, obj, "", JSP_GET_VALUE);
#if JLABS
	GET_FUNCTION(createConfigurationObject, obj, "", JSP_GET_VALUE);
#endif
	GET_FUNCTION(createDrmAgentObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createParentalControlManagerObject, obj, "", JSP_GET_VALUE);
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
#if !defined(JLABS)
	GET_FUNCTION(createDownloadTriggerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createDownloadManagerObject, obj, "", JSP_GET_VALUE);
#endif
#endif // HBBTV_DOWNLOAD || OIPF
#if defined(HBBTV_PVR) || defined(OIPF)
	GET_FUNCTION(createRecordingSchedulerObject, obj, "", JSP_GET_VALUE);
#endif // HBBTV_PVR || OIPF
#ifdef OIPF
	GET_FUNCTION(createChannelConfig, obj, "", JSP_GET_VALUE);
#endif // OIPF
#if defined(OIPF) || defined(HBBTV_1_2_1)
	GET_FUNCTION(createSearchManagerObject, obj, "", JSP_GET_VALUE);
#endif // OIPF || HBBTV_1_2_1
#if defined(HMX_WEBUI)
   	GET_FUNCTION(createAirplayManagerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createDlnaManagerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createSambaManagerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createFtpManagerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createWoonManagerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createContentManagerObject, obj, "", JSP_GET_VALUE);
//	GET_FUNCTION(createMediaSearchManagerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createHumaxConfigurationObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createHumaxApplicationManagerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createHumaxStorageUtilObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createHumaxCasUtil, obj, "", JSP_GET_VALUE);
    GET_FUNCTION(createBluetoothManagerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createHumaxOperatorFeatureObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createDialManagerObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createSatipManagerObject, obj, "", JSP_GET_VALUE);
    GET_FUNCTION(createAlexaHelperManagerObject, obj, "", JSP_GET_VALUE);
#if defined(USE_VIDEOHANDLE)
	GET_FUNCTION(createHumaxVideoBroadcastObject, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createHumaxVideoOnDemandObject, obj, "", JSP_GET_VALUE);
#endif
#endif
#ifdef CONFIG_OP_FREESAT
	GET_FUNCTION(createHumaxOperatorFreesatObject, obj, "", JSP_GET_VALUE);
#endif
#ifdef JLABS
	GET_FUNCTION(createJlabsEmmMessageManager, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createJlabsCDSSearcher, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(createJlabsDMRController, obj, "", JSP_GET_VALUE);
#endif // JLABS

	result->type = JSP_TYPE_BOOLEAN;
	result->u.boolean = 1;

	return JSP_GET_VALUE;
}

//static
int OipfObjectFactory::isObjectSupported(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	const char* mimeType = STRING_VALUE(argv[0]);
	OOIFBoolean supported = FALSE;

	NATIVE_CALL(native_ObjectFactoryIsObjectSupported(mimeType, &supported));
	RETURN_BOOLEAN(supported, JSP_CALL_VALUE);
}

//static
int OipfObjectFactory::createDrmAgentObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	CREATE_OBJECT(createDrmAgent(this_obj, &OBJ));
}

//static
int OipfObjectFactory::createConfigurationObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;
	RETURN_OBJECT(this_obj, new MasterConfiguration(), result, JSP_CALL_VALUE);
}

#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
//static
int OipfObjectFactory::createDownloadTriggerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	CREATE_OBJECT(createDownloadTrigger(this_obj, &OBJ));
}

//static
int OipfObjectFactory::createDownloadManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	CREATE_OBJECT(createDownloadManager(this_obj, &OBJ));
}
#endif // HBBTV_DOWNLOAD || OIPF

//static
int OipfObjectFactory::createApplicationManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new ApplicationManager(), result, JSP_CALL_VALUE);
}

//static
int OipfObjectFactory::createCapabilitiesObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new Capabilities(), result, JSP_CALL_VALUE);
}

//static
int OipfObjectFactory::createParentalControlManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	CREATE_OBJECT(createParentalControlManager(this_obj, &OBJ));
}

#if defined(HBBTV_PVR) || defined(OIPF)
//static
int OipfObjectFactory::createRecordingSchedulerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	CREATE_OBJECT(createRecordingScheduler(this_obj, &OBJ));
}
#endif // HBBTV_PVR || OIPF

#ifdef OIPF
//static
int OipfObjectFactory::createChannelConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new ChannelConfig(DEFAULT), result, JSP_CALL_VALUE);
}
#endif // OIPF

#if defined(OIPF) || defined(HBBTV_1_2_1)
//static
int OipfObjectFactory::createSearchManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	CREATE_OBJECT(createSearchManager(this_obj, &OBJ));
}

//static
OOIFReturnCode OipfObjectFactory::createSearchManager(jsplugin_obj* global_obj, ObjectInstance **result)
{
	SearchManagerHandle id;
	NATIVE_CALL_HANDLE_OBJ(native_SearchManagerCreate(&id, WINDOW, HOST), new SearchManager(id), global_obj);
}
#endif // OIPF || HBBTV_1_2_1

OOIFReturnCode OipfObjectFactory::createDrmAgent(jsplugin_obj *global_obj, ObjectInstance **result)
{
	DrmAgentHandle id;
#ifdef OIPF
	NATIVE_CALL_HANDLE_OBJ(native_DrmAgentCreate(&id, WINDOW, HOST), new DrmAgent(id), global_obj);
#else
	NATIVE_CALL_HANDLE_OBJ(native_DrmAgentCreate(&id, WINDOW, HOST), new DrmAgent(id), NULL);
#endif
}

OOIFReturnCode OipfObjectFactory::createParentalControlManager(jsplugin_obj *global_obj, ObjectInstance **result)
{
	NATIVE_CALL_HANDLE_OBJ(native_ParentalControlManagerOnCreate(WINDOW, HOST), new ParentalControlManager(), global_obj);
}

OOIFReturnCode OipfObjectFactory::createVideoBroadcast(jsplugin_obj *global_obj, ObjectInstance **result)
{
	VideoBroadcastHandle id;
	NATIVE_CALL_HANDLE_OBJ(native_BroadcastCreate(&id, WINDOW, HOST), new VideoBroadcast(id), global_obj);
}

OOIFReturnCode OipfObjectFactory::createVideoOnDemand(jsplugin_obj *global_obj, const char *mime_type, const char *data, ObjectInstance **result)
{
	VodHandle id;
	NATIVE_CALL_HANDLE_OBJ(native_VodCreate(&id, WINDOW, HOST, mime_type, data), new VideoOnDemand(id), global_obj);
}
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
OOIFReturnCode OipfObjectFactory::createDownloadTrigger(jsplugin_obj *global_obj, ObjectInstance **result)
{
	NATIVE_CALL_HANDLE_OBJ(native_DownloadTriggerOnCreate(WINDOW, HOST), new DownloadTrigger(), global_obj);
}

OOIFReturnCode OipfObjectFactory::createDownloadManager(jsplugin_obj *global_obj, ObjectInstance **result)
{
	NATIVE_CALL_HANDLE_OBJ(native_DownloadManagerOnCreate(WINDOW, HOST), new DownloadManager(), global_obj);
}
#endif // HBBTV_DOWNLOAD || OIPF
#if defined(HBBTV_PVR) || defined(OIPF)
OOIFReturnCode OipfObjectFactory::createRecordingScheduler(jsplugin_obj *global_obj, ObjectInstance **result)
{
	NATIVE_CALL_HANDLE_OBJ(native_RecordingSchedulerOnCreate(WINDOW, HOST), new RecordingScheduler(), global_obj);
}
#endif // HBBTV_PVR || OIPF

#if defined(HMX_WEBUI)
int OipfObjectFactory::createAirplayManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new HAirplay(), result, JSP_CALL_VALUE);
}

int OipfObjectFactory::createWoonManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new HWoon(), result, JSP_CALL_VALUE);
}

int OipfObjectFactory::createSambaManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new HSambaManager(), result, JSP_CALL_VALUE); 
}

int OipfObjectFactory::createFtpManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new HFtpManager(), result, JSP_CALL_VALUE);

}

int OipfObjectFactory::createContentManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new HContentManager(), result, JSP_CALL_VALUE);

}

int OipfObjectFactory::createDlnaManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new HDlna(), result, JSP_CALL_VALUE);
}

int OipfObjectFactory::createDialManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new HDial(), result, JSP_CALL_VALUE);
}

int OipfObjectFactory::createSatipManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new HSatip(), result, JSP_CALL_VALUE);
}

int OipfObjectFactory::createAlexaHelperManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new HAlexaHelper(), result, JSP_CALL_VALUE);
}

//static
#if 0
int OipfObjectFactory::createMediaSearchManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;
	RETURN_OBJECT(this_obj, new HMX_Media(), result, JSP_CALL_VALUE);
}
#endif

int OipfObjectFactory::createHumaxConfigurationObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;
	RETURN_OBJECT(this_obj, new HMasterConfiguration(), result, JSP_CALL_VALUE);
}
int OipfObjectFactory::createHumaxApplicationManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;
	RETURN_OBJECT(this_obj, new HMXApplicationManager(), result, JSP_CALL_VALUE);
}
int OipfObjectFactory::createHumaxStorageUtilObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;
	RETURN_OBJECT(this_obj, new HMXStorageUtil(), result, JSP_CALL_VALUE);
}
int OipfObjectFactory::createHumaxCasUtil(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;
	RETURN_OBJECT(this_obj, new HCasUi(), result, JSP_CALL_VALUE);
}
int OipfObjectFactory::createHumaxOperatorFeatureObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;
	RETURN_OBJECT(this_obj, new HMXOperatorFeature(), result, JSP_CALL_VALUE);
}
int OipfObjectFactory::createBluetoothManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(0);
    ENTRY;
    RETURN_OBJECT(this_obj, new HBluetoothManager(), result, JSP_CALL_VALUE);
}
#if defined(USE_VIDEOHANDLE)
OOIFReturnCode OipfObjectFactory::createHumaxVideoBroadcast(jsplugin_obj *global_obj, ObjectInstance **result)
{
	*result = HVideoBroadcast::create();
	return OOIF_RETURN_OK;
}
OOIFReturnCode OipfObjectFactory::createHumaxVideoOnDemand(jsplugin_obj *global_obj, ObjectInstance **result)
{
	*result = HVideoOnDemand::create();
	return OOIF_RETURN_OK;
}
int OipfObjectFactory::createHumaxVideoBroadcastObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;
	ObjectInstance *humaxVideoBroadcastObject = NULL;
	createHumaxVideoBroadcast(this_obj, &humaxVideoBroadcastObject);
	RETURN_OBJECT(this_obj, humaxVideoBroadcastObject, result, JSP_CALL_VALUE);
}
int OipfObjectFactory::createHumaxVideoOnDemandObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;
	ObjectInstance *humaxVideoOnDemandObject = NULL;
	createHumaxVideoOnDemand(this_obj, &humaxVideoOnDemandObject);
	RETURN_OBJECT(this_obj, humaxVideoOnDemandObject, result, JSP_CALL_VALUE);
}
#endif // USE_VIDEOHANDLE
#endif
#ifdef CONFIG_OP_FREESAT
int OipfObjectFactory::createHumaxOperatorFreesatObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;
	RETURN_OBJECT(this_obj, new HMXOperatorFreesat(), result, JSP_CALL_VALUE);
}
#endif
#ifdef JLABS
//static
int OipfObjectFactory::createJlabsEmmMessageManager(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new jlabsEmmMessageManager(), result, JSP_CALL_VALUE);
}

//static
int OipfObjectFactory::createJlabsCDSSearcher(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new jlabsCDSSearcher(), result, JSP_CALL_VALUE);
}

OOIFReturnCode OipfObjectFactory::createRemoteControlFunctionObject(jsplugin_obj *global_obj, ObjectInstance **result)
{
	ENTRY;
	jlabsRemoteControlFunctionHandle id;
	NATIVE_CALL_HANDLE_OBJ(native_jlabsRemoteControlFunctionCreate(&id, WINDOW, HOST), new jlabsRemoteControlFunction(id), global_obj);
}

int OipfObjectFactory::createJlabsDMRController(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new jlabsDMRController(), result, JSP_CALL_VALUE);
}
#endif // JLABS
