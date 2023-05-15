// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/*
  This is the OIPF plugin - "OOIF"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "OOIFUtils.h"
#include "ObjectFactory.h"
#include "DownloadManager.h"
#include "DrmAgent.h"
#include "NativeDownload.h"
#include "NativeDrmAgent.h"
#include "ApplicationManager.h"
#include "Application.h"
#include "MasterConfiguration.h"
#include "DrmAgent.h"
#include "DownloadTrigger.h"
#include "Capabilities.h"
#include "RecordingScheduler.h"
#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "SearchManager.h"
#endif // OIPF || HBBTV_1_2_1
#include "Exception.h"
#include "VideoBroadcast.h"
#include "ParentalControlManager.h"
#include "ObjectInstance.h"
#include "VideoOnDemand.h"
#include "AsyncHandler.h"
#ifdef OIPF
#include "ChannelConfig.h"
#endif // OIPF
#include "NativeVideoOnDemand.h"
#include "NativeVideoBroadcast.h"
#include "NativeSecurity.h"
#include "NativeDebug.h"
#include "LocalSystem.h"

#include "oplappmgr.h"

namespace Ooif
{
static jsplugin_cap cap;

static const char* global_names[] = { "OipfObjectFactory", "oipfObjectFactory", "application", "debug", NULL };
static const char* default_object_types[] = { "application/oipfApplicationManager",
        "application/oipfConfiguration",
        "application/oipfCapabilities",
        "application/oipfDrmAgent",
        "application/oipfParentalControlManager",
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
        "application/oipfDownloadManager",
        "application/oipfDownloadTrigger",
#endif // HBBTV_DOWNLOAD || OIPF
#if defined(HBBTV_PVR) || defined(OIPF)
        "application/oipfRecordingScheduler",
#endif // HBBTV_PVR || OIPF
#if defined(OIPF) || defined(HBBTV_1_2_1)
        "application/oipfSearchManager",
#endif // OIPF || HBBTV_1_2_1
        "video/broadcast",
        NULL
                                            };
static int default_object_types_count = sizeof(default_object_types) / sizeof(default_object_types[0]);
static char** vod_object_types = NULL;
static int vod_object_types_count = 0;
const char** handled_object_types = NULL;

/** global variable for jsplugin module callbacks */
jsplugin_callbacks *g_opera_callbacks;
std::list<jsplugin_obj*> g_proteges;

void firstInitializeOOIF();

/** flag set when listeners are initialized */
bool initialized = false;

//static
int debug(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	NATIVE_WARN_NOK(native_debug(STRING_VALUE(argv[0])));
	return JSP_CALL_NO_VALUE;
}

static void videoVisible(jsplugin_obj *obj, int visibility, int unloading)
{
	VisualObject * o = EXTRACT_OBJECT<VisualObject>(obj);
	if (o)
	{
		o->videoVisible(visibility, unloading);
	}
	else
	{
		OOIF_LOG_WARNING("add_tv_visual videoVisible called with unknown object\n");
	}
}

static void videoPosition(jsplugin_obj *obj, int x, int y, int w, int h)
{
	VisualObject * o = EXTRACT_OBJECT<VisualObject>(obj);
	if (o)
	{
		o->videoPosition(x, y, w, h);
	}
	else
	{
		OOIF_LOG_DEBUG("WARNING: add_tv_visual VideoPosition called with unknown object\n");
	}
}

static int oipf_global_getter(jsplugin_obj *ctx, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("oipfObjectFactory", ctx, new OipfObjectFactory(), JSP_GET_VALUE_CACHE);
	GET_FUNCTION(debug, ctx, "s", JSP_GET_VALUE_CACHE);

	// window.application is a cheat function, which returns the
	// Application jsplugin object corresponding to the current window
	// This is stored in the static application map in the
	// ApplicationManager. If necessary, this function will create a
	// new Application, so we can say that they are initialized
	// lazily.
	if (!strcmp(name, "application"))
	{
		long int window_identifier = OOIFUtils::getWindow(ctx);
		char *host = OOIFUtils::getHost(ctx);
		OOIF_LOG_DEBUG("Need to get the application for: %ld\n", window_identifier);
		ApplicationIdentifier appId;
		jsplugin_obj *this_obj = ctx; // macro magic...
		NATIVE_CALL(native_AppManGetApplicationId(window_identifier, host, &appId));

		jsplugin_obj *app = ApplicationManager::getApplicationById(appId, ctx);
		if (!app)
			RETURN_NULL(JSP_GET_VALUE);

		result->u.object = app;
		result->type = JSP_TYPE_OBJECT;
		return JSP_GET_VALUE;
	}

	return JSP_GET_NOTFOUND;
}

static void oipf_global_gc_trace(jsplugin_obj *this_obj)
{
	for (list<jsplugin_obj*>::iterator it=g_proteges.begin(); it!=g_proteges.end(); it++)
	{
		jsplugin_obj *obj = *it;
//			OOIF_LOG_DEBUG("Global protecting %p", obj);
		g_opera_callbacks->gcmark(obj);
	}
#if defined(CONFIG_DEBUG) && defined(OBJ_INSTANCE_COUNTER)
    static int test_cnt = 0;
    if ( test_cnt == 0 )
    {
        ObjectInstance::debugPrintObjectInstanceCount();
    }

    test_cnt++;
    if ( test_cnt > 10 ) test_cnt = 0;
#endif    
}

int isVodObject(const char *type)
{
	for (int i = 0; i < vod_object_types_count; i++)
		if (strcasecmp(vod_object_types[i], type) == 0)
			return true;
	return false;
}

int handle_object(jsplugin_obj *global_obj, jsplugin_obj *object_obj, int attrs_count, jsplugin_attr *attrs,
                  jsplugin_getter **getter, jsplugin_setter **setter, jsplugin_destructor **destructor,
                  jsplugin_gc_trace **trace, jsplugin_notify **inserted, jsplugin_notify **removed)
{
	ENTRY;

	// We have to dig our way through the attrs to find the mime type
	const char *type = NULL;
	for (int i=0; i<attrs_count; i++)
	{
		if (!strcmp(attrs[i].name, "type"))
		{
			type = attrs[i].value;
		}
	}

	if (!type)
	{
		return JSP_OBJECT_ERROR;
	}

	OOIF_LOG_INFO("Handling object for MIME type: %s\n", type);

	const char *data = NULL;
	if (isVodObject(type))
	{
		for (int i=0; i<attrs_count; i++)
		{
			if (!strcmp(attrs[i].name, "data"))
			{
				data = attrs[i].value;
			}
		}
		if (data)
			OOIF_LOG_DEBUG("Received data=\"%s\" in object tag for VOD object (type=\"%s\")\n", data, type);
		else
			OOIF_LOG_DEBUG("No data received in object tag for VOD object (type=\"%s\")\n", type);

		NATIVE_HANDLE_OBJECT(type, OipfObjectFactory::createVideoOnDemand(global_obj, type, data, &RETVAL), JSP_OBJECT_VISUAL);
	}

	HANDLE_OBJECT("application/oipfApplicationManager", new ApplicationManager(), JSP_OBJECT_OK);
	HANDLE_OBJECT("application/oipfConfiguration", new MasterConfiguration(), JSP_OBJECT_OK);
	HANDLE_OBJECT("application/oipfCapabilities", new Capabilities(), JSP_OBJECT_OK);

	NATIVE_HANDLE_OBJECT("application/oipfDrmAgent", OipfObjectFactory::createDrmAgent(global_obj, &RETVAL), JSP_OBJECT_OK);
	NATIVE_HANDLE_OBJECT("application/oipfParentalControlManager", OipfObjectFactory::createParentalControlManager(global_obj, &RETVAL), JSP_OBJECT_OK);

	NATIVE_HANDLE_OBJECT("video/broadcast", OipfObjectFactory::createVideoBroadcast(global_obj, &RETVAL), JSP_OBJECT_VISUAL);
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
	NATIVE_HANDLE_OBJECT("application/oipfDownloadTrigger", OipfObjectFactory::createDownloadTrigger(global_obj, &RETVAL), JSP_OBJECT_OK);
	NATIVE_HANDLE_OBJECT("application/oipfDownloadManager", OipfObjectFactory::createDownloadManager(global_obj, &RETVAL), JSP_OBJECT_OK);
#endif // HBBTV_DOWNLOAD || OIPF
#if defined(HBBTV_PVR) || defined(OIPF)
	NATIVE_HANDLE_OBJECT("application/oipfRecordingScheduler", OipfObjectFactory::createRecordingScheduler(global_obj, &RETVAL), JSP_OBJECT_OK);
#endif // HBBTV_PVR || OIPF
#if defined(OIPF) || defined(HBBTV_1_2_1)
	NATIVE_HANDLE_OBJECT("application/oipfSearchManager", OipfObjectFactory::createSearchManager(global_obj, &RETVAL), JSP_OBJECT_OK);
#endif // OIPF || HBBTV_1_2_1

	return JSP_OBJECT_ERROR;
}

void ooif_init(jsplugin_obj *global_context)
{
	if (!initialized)
	{
		firstInitializeOOIF();
		initialized = true;
	}
	OOIFUtils::registerGlobalContext(global_context);
}

void ooif_destroy(jsplugin_obj *global_context)
{
	OOIFUtils::unregisterGlobalContext(global_context);
	OOIFUtils::freeTmpBuffer();
}

int ooif_allow_access(const char *protocol, const char *hostname, int port, const char *url)
{
	OOIFBoolean retval = FALSE;
#if 0
	//NATIVE_WARN_NOK(native_SecurityAllowAccess(protocol, hostname, port, &retval));
	bool hbbtv_override = false;
	bool oipf_override = false;
#ifndef	DISABLE_ALLOW_ACCESS_OVERRIDE
	hbbtv_override = strstr(url, "_hbbtv_");
	oipf_override = strstr(url, "_oipf_");
#endif
#ifdef OIPF
	retval = oipf_override || (!hbbtv_override && (!strcmp(protocol, "file") || !strcmp(protocol, "widget")));
#endif
#ifdef HBBTV
	retval = hbbtv_override || (!oipf_override && (!strcmp(protocol, "http") || !strcmp(protocol, "https")));
#endif
	return retval;

#else
#if 0
	if ( (strcmp(protocol, "http" ) == 0
		|| strcmp(protocol, "https" ) == 0
		|| strcmp(protocol, "dvb" ) == 0)
		&& OplOIPFApplicationManager::findApplication(hostname) == false )
#else
	if ( (strcmp(protocol, "http" ) == 0
		|| strcmp(protocol, "https" ) == 0
		|| strcmp(protocol, "dvb" ) == 0)
		&& OplApplicationManager::getApplicationWindow() == OPLWINDOW_HBBTV )
#endif
	{
		OOIF_LOG_PRINT("##########################################################\n");
		OOIF_LOG_PRINT("HbbTV ooif_allow_access: protocol=%s, hostname=%s\n", protocol, hostname);
		OOIF_LOG_PRINT("##########################################################\n");
		retval = TRUE;
	}
	else
	{
		retval = FALSE;
	}

	return retval;
#endif
}

void firstInitializeOOIF()
{
	NATIVE_WARN_NOK(native_DrmAgentSetListener(DrmAgent::onDrmAgentEvent));
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
	NATIVE_WARN_NOK(native_DownloadSetListener(DownloadManager::onDownloadEvent));
#endif // HBBTV_DOWNLOAD || OIPF
	NATIVE_WARN_NOK(native_ApplicationManagerSetListener(ApplicationManager::onApplicationManagerEvent));
	NATIVE_WARN_NOK(native_ApplicationManagerSetAppDestroyedListener(ApplicationManager::onApplicationDestroyed));
#ifdef OIPF
	NATIVE_WARN_NOK(native_ApplicationManagerSetKeyListener(ApplicationManager::receiveKey));
#endif // OIPF
#ifdef OIPF
	NATIVE_WARN_NOK(native_LocalSystemSetListener(LocalSystem::onLocalSystemEvent));
#if defined(HBBTV_PVR) || defined(OIPF)
	NATIVE_WARN_NOK(native_RecordingSchedulerSetListener(RecordingScheduler::onRecordingSchedulerEvent));
#endif // HBBTV_PVR || OIPF
#endif // OIPF
#if defined(OIPF) || defined(HBBTV_1_2_1)
	NATIVE_WARN_NOK(native_SearchManagerSetListener(SearchManager::onMetadataEvent));
#endif // OIPF || HBBTV_1_2_1
	NATIVE_WARN_NOK(native_VodSetListener(VideoOnDemand::onVideoOnDemandEvent));
	NATIVE_WARN_NOK(native_BroadcastSetListener(VideoBroadcast::onVideoBroadcastEvent));

#ifdef OOIF_ASYNC
	NATIVE_WARN_NOK(native_AsyncSetResumer(AsyncHandler::resume));
	NATIVE_WARN_NOK(native_AsyncSetTokenGetter(AsyncHandler::getResumeToken));
	AsyncHandler::setResume(g_opera_callbacks->resume);
#endif // OOIF_ASYNC
#ifdef OIPF
	NATIVE_WARN_NOK(native_ApplicationManagerInitializeWidgets());
	NATIVE_WARN_NOK(native_ApplicationManagerSetWidgetListener(ApplicationManager::onApplicationManagerWidgetEvent));
	NATIVE_WARN_NOK(native_ApplicationSetListener(Application::onApplicationEvent));
	NATIVE_WARN_NOK(native_ChannelConfigSetListener(ChannelConfig::onChannelConfigEvent));
#endif // OIPF

	OOIF_LOG_INFO("OOIF event listeners registered\n");

	ApplicationManager::initializeApplicationManager();
}

} /* namespace Ooif */

using namespace Ooif;

/* Someone may want to compile and link under Windows */
# if defined (WIN32) || defined (WINCE)	// {
#  define DECLSPEC __declspec(dllexport)
# else
#  define DECLSPEC
# endif

extern "C" {

	static void final_free_object_types()
	{
		int i;
		if (handled_object_types)
		{
			HLIB_STD_MemFree(handled_object_types);
			handled_object_types = NULL;
		}

		if (vod_object_types)
		{
			for (i = 0; i < vod_object_types_count; i++)
			{
				HLIB_STD_MemFree(vod_object_types[i]);
			}
			HLIB_STD_MemFree(vod_object_types);
			vod_object_types = NULL;
		}
	}

	/* the global plugin hook. */
	DECLSPEC int jsplugin_capabilities(jsplugin_cap **result, jsplugin_callbacks *c)
	{
		OOIF_LOG_FPRINT("====== HBBTV plugin loaded ======\n");
		NATIVE_WARN_NOK(native_VodGetSupportedMimetypes(&vod_object_types, &vod_object_types_count));
		handled_object_types = (const char**) HLIB_STD_MemAlloc((vod_object_types_count + default_object_types_count) * sizeof(const char*));
		atexit(final_free_object_types);
		memcpy(handled_object_types, vod_object_types, vod_object_types_count * sizeof(const char*));
		memcpy(handled_object_types + vod_object_types_count, default_object_types, default_object_types_count * sizeof(const char*));

		memset(&cap, 0, sizeof(jsplugin_cap));
		cap.global_names = global_names;
		cap.object_types = handled_object_types;

		cap.global_getter = oipf_global_getter;
		cap.global_setter = NULL;
		cap.init = ooif_init;
		cap.destroy = ooif_destroy;
		cap.handle_object = handle_object;
		cap.allow_access = ooif_allow_access;
		cap.gc_trace = oipf_global_gc_trace;

		*result = &cap;

		c->add_tv_visual(videoVisible, videoPosition);

		g_opera_callbacks = c;

		return 0;
	}

} // extern "C"