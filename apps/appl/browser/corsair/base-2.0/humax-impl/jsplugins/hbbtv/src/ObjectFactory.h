// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __OBJECTFACTORY
#define __OBJECTFACTORY

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "NativeOOIFTypes.h"

namespace Ooif
{
class OipfObjectFactory : public ObjectInstance
{
protected:
	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	// Specific functions for this interface
	// Some of the functions from OipfObjectFactory are implemented in ooif.js:
	// - createVideoBroadcastObject()
	// - createVideoMpegObject()
	static int isObjectSupported(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createDrmAgentObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createConfigurationObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
	static int createDownloadTriggerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createDownloadManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // HBBTV_DOWNLOAD || OIPF
	static int createApplicationManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createCapabilitiesObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createParentalControlManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#if defined(HBBTV_PVR) || defined(OIPF)
	static int createRecordingSchedulerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // HBBTV_PVR || OIPF
#ifdef OIPF
	static int createChannelConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
#if defined(OIPF) || defined(HBBTV_1_2_1)
	static int createSearchManagerObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static OOIFReturnCode createSearchManager(jsplugin_obj* global_obj, ObjectInstance **result);
#endif // OIPF || HBBTV_1_2_1
	static OOIFReturnCode createDrmAgent(jsplugin_obj *global_obj, ObjectInstance **result);
	static OOIFReturnCode createParentalControlManager(jsplugin_obj *global_obj, ObjectInstance **result);
	static OOIFReturnCode createVideoBroadcast(jsplugin_obj *global_obj, ObjectInstance **result);
	static OOIFReturnCode createVideoOnDemand(jsplugin_obj *global_obj, const char *mime_type, const char *data, ObjectInstance **result);
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
	static OOIFReturnCode createDownloadTrigger(jsplugin_obj *global_obj, ObjectInstance **result);
	static OOIFReturnCode createDownloadManager(jsplugin_obj *global_obj, ObjectInstance **result);
#endif // HBBTV_DOWNLOAD || OIPF
#if defined(HBBTV_PVR) || defined(OIPF)
	static OOIFReturnCode createRecordingScheduler(jsplugin_obj *global_obj, ObjectInstance **result);
#endif // HBBTV_PVR || OIPF
};
} /* namespace Ooif */

#endif // __OBJECTFACTORY
