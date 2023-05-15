// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ApplicationManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <functional>
#include <algorithm>

#include "jsplugin.h"
#include "OOIFUtils.h"
#include "Application.h"
#include "NativeApplicationManager.h"

#include "EmptyEvent.h"
#if defined OIPF || ( defined HBBTV && defined KEY_HBBTV_TO_OIPF)

#include "ApplicationKeyEvent.h"
#include "ApplicationLoadedEvent.h"
#include "ApplicationUnloadedEvent.h"
#include "Collection.h"
#include "WidgetDescriptor.h"
#include "WidgetInstallationEvent.h"
#include "WidgetUninstallationEvent.h"
#endif // OIPF || (HBBTV && KEY_HBBTV_TO_OIPF)
#include "ApplicationLoadErrorEvent.h"

#include "macros.h"
#include "Exception.h"

#if defined(CONFIG_PROD_FVP4000T)
#include "humax/HumaxApplicationLoadedEvent.h"
#include "humax/HumaxApplicationUnloadedEvent.h"
#endif


#if defined(CONFIG_OP_JCOM)
#include "hlib.h"
#endif

namespace Ooif
{
extern std::list<jsplugin_obj*> g_proteges;
}

using namespace Ooif;


//static
ApplicationManager::ApplicationsMap ApplicationManager::applications;
//static
pthread_mutex_t ApplicationManager::applicationsMutex = PTHREAD_MUTEX_INITIALIZER;
//static
list<ApplicationManager*> ApplicationManager::instances;
//static
pthread_mutex_t ApplicationManager::instancesMutex = PTHREAD_MUTEX_INITIALIZER;

ApplicationManager::ApplicationManager()
{
	ENTRY;
	classType = CLASS_APPLICATIONMANAGER;
	// Initialize state
	pthread_mutex_lock(&instancesMutex);
#ifdef CONFIG_PROD_FVP4000T
	if( ApplicationManager::instances.size() == 0 ) /* if there are some app mgr instance, do not need to set a callback again */
		native_AppManSetHumaxApplicationListener(ApplicationManager::onHumaxApplicationManagerEvent);
#endif
	ApplicationManager::instances.push_back(this);
	pthread_mutex_unlock(&instancesMutex);
}

ApplicationManager::~ApplicationManager()
{
	pthread_mutex_lock(&instancesMutex);
	ApplicationManager::instances.remove(this);
#ifdef CONFIG_PROD_FVP4000T
	if( ApplicationManager::instances.size() == 0 ) /* if there is no more than one, now we can delete the callback. */
		native_AppManSetHumaxApplicationListener(NULL);
#endif
	pthread_mutex_unlock(&instancesMutex);
}

int ApplicationManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	INTRINSIC_EVENT_GET(LowMemory);
#ifdef OIPF
	INTRINSIC_EVENT_GET(ApplicationLoaded);
	INTRINSIC_EVENT_GET(ApplicationUnloaded);
	INTRINSIC_EVENT_GET(WidgetInstallation);
	INTRINSIC_EVENT_GET(WidgetUninstallation);
#endif // OIPF
	INTRINSIC_EVENT_GET(ApplicationLoadError);

	GET_FUNCTION(getOwnerApplication, obj, "o", JSP_GET_VALUE_CACHE);

#ifdef OIPF
	GET_NUMBER("WIDGET_INSTALLATION_STARTED", (OOIFNumber)WIDGET_INSTALLATION_STARTED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_INSTALLATION_COMPLETED", (OOIFNumber)WIDGET_INSTALLATION_COMPLETED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_INSTALLATION_FAILED", (OOIFNumber)WIDGET_INSTALLATION_FAILED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_UNINSTALLATION_STARTED", (OOIFNumber)WIDGET_UNINSTALLATION_STARTED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_UNINSTALLATION_COMPLETED", (OOIFNumber)WIDGET_UNINSTALLATION_COMPLETED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_UNINSTALLATION_FAILED", (OOIFNumber)WIDGET_UNINSTALLATION_FAILED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_ERROR_STORAGE_AREA_FULL", (OOIFNumber)WIDGET_ERROR_STORAGE_AREA_FULL, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_ERROR_DOWNLOAD", (OOIFNumber)WIDGET_ERROR_DOWNLOAD, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_ERROR_INVALID_ZIP_ARCHIVE", (OOIFNumber)WIDGET_ERROR_INVALID_ZIP_ARCHIVE, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_ERROR_INVALID_SIGNATURE", (OOIFNumber)WIDGET_ERROR_INVALID_SIGNATURE, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_ERROR_GENERIC", (OOIFNumber)WIDGET_ERROR_GENERIC, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WIDGET_ERROR_SIZE_EXCEEDED", (OOIFNumber)WIDGET_ERROR_SIZE_EXCEEDED, JSP_GET_VALUE_CACHE);

	GET_FUNCTION(getChildApplications, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getApplicationVisualizationMode, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(gc, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(installWidget, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(uninstallWidget, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getWidgetInstallProgress, obj, "s", JSP_GET_VALUE);

	// internal functions
	GET_OBJECT("widgets", obj, new WidgetDescriptorCollection(), JSP_GET_VALUE);
#endif // OIPF

#if (defined OIPF && defined KEY_HBBTV_TO_OIPF) || ( defined HBBTV && defined KEY_HBBTV_TO_OIPF)
	// internal functions
	GET_FUNCTION(__key, obj, "sn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(__triggerApplicationLoadedEvent, obj, "", JSP_GET_VALUE_CACHE);
#endif // (OIPF && KEY_HBBTV_TO_OIPF )|| (HBBTV && KEY_HBBTV_TO_OIPF)

#if defined(HMX_WEBUI)
	GET_FUNCTION(createPortalApplication, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(destroyPortalApplication, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(addOIPFApplication, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeOIPFApplication, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeAllOIPFApplications, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createHbbTVApplication, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(destroyHbbTVApplication, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(doTeletext, obj, "", JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("HBBTV_APP_TYPE_AUTOSTART", native_GetHbbTVApplicationType(name, &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("HBBTV_APP_TYPE_INDEPENDENT", native_GetHbbTVApplicationType(name, &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("HBBTV_APP_TYPE_DIGITALTELETEXT", native_GetHbbTVApplicationType(name, &RETVAL), JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getHumaxApplications, obj, "", JSP_GET_VALUE_CACHE);

#ifdef CONFIG_PROD_FVP4000T
	GET_FUNCTION(createHumaxApplication, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(destroyHumaxApplication, obj, "s", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(HumaxApplicationLoaded);
	INTRINSIC_EVENT_GET(HumaxApplicationUnloaded);
#endif

#endif

	return EventTarget::getter(obj, name, result);
}

int ApplicationManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	INTRINSIC_EVENT(LowMemory);
#ifdef OIPF
	INTRINSIC_EVENT(ApplicationLoaded);
	INTRINSIC_EVENT(ApplicationUnloaded);
	INTRINSIC_EVENT(WidgetInstallation);
	INTRINSIC_EVENT(WidgetUninstallation);
#endif // OIPF
	INTRINSIC_EVENT(ApplicationLoadError);


#ifdef CONFIG_PROD_FVP4000T
	INTRINSIC_EVENT(HumaxApplicationUnloaded);
	INTRINSIC_EVENT(HumaxApplicationLoaded);
#endif
	return JSP_PUT_NOTFOUND;
}

int ApplicationManager::destructor(jsplugin_obj *obj)
{
	ENTRY;
	OOIF_LOG_DEBUG("ApplicationManager have been destroyed: %p\n", this);
	return JSP_DESTROY_OK;
}

//static
int ApplicationManager::getOwnerApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	char *host = OOIFUtils::getHost(this_obj);
	long int window_identifier = OOIFUtils::getWindow(this_obj);
	ApplicationIdentifier appId;
	NATIVE_CALL(native_AppManGetApplicationId(window_identifier, host, &appId));

	jsplugin_obj *app = getApplicationById(appId, this_obj);
	if (!app)
		RETURN_NULL(JSP_CALL_VALUE);

	result->u.object = app;
	result->type = JSP_TYPE_OBJECT;
	return JSP_CALL_VALUE;
}

#ifdef OIPF
//static
int ApplicationManager::getChildApplications(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	Application *parent = OBJECT_VALUE_OR_THROW(parent, argv[0], Application);
	ApplicationIdentifier *children = NULL;
	int count;
	NATIVE_CALL(native_AppManGetChildApplications(parent->getAppId(), &count, &children));

	ApplicationCollection *childrenCollection = new ApplicationCollection;
	for (int i=0; i<count; i++)
	{
		Application *app = EXTRACT_OBJECT<Application>(getApplicationById(children[i], this_obj));
		if (app)
		{
			childrenCollection->addItem(app);
		}
		else
		{
			OOIF_LOG_WARNING("Child %d did not match an application\n", i);
		}
	}
	HLIB_STD_MemFree(children);
	RETURN_OBJECT(this_obj, childrenCollection, result, JSP_CALL_VALUE);
}

//static
int ApplicationManager::getApplicationVisualizationMode(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFNumber mode;
	NATIVE_CALL(native_AppManGetApplicationVisualizationMode(&mode));
	RETURN_NUMBER(mode, JSP_CALL_VALUE);
}

//static
int ApplicationManager::gc(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	return JSP_CALL_NO_VALUE;
}
#endif // OIPF

//static
bool ApplicationManager::isOipfApplication(ApplicationIdentifier appId)
{
    OOIFBoolean isOipfApp = false;
	return native_AppManIsOipfApplication(appId, &isOipfApp) == OOIF_RETURN_OK
		&& isOipfApp;
}

#if defined OIPF || ( defined HBBTV && defined KEY_HBBTV_TO_OIPF)
//static
int ApplicationManager::__key(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
					int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	CHECK_ARGUMENT(1, JSP_TYPE_NUMBER);

#if !(defined HBBTV && defined KEY_HBBTV_TO_OIPF)
	//check App context since we should only handle this event inside DAE application
	ApplicationIdentifier appId;

	if (OOIF_RETURN_OK != native_AppManGetApplicationId(OOIFUtils::getWindow(this_obj), OOIFUtils::getHost(this_obj), &appId) || !isOipfApplication(appId))
		THROW_GENERIC_ERR(this_obj, result, JSP_CALL_EXCEPTION);
#else
	/* keyEvents must be propagated even from HbbTV applications into OIPF Activation Stack
	 * so let's skip the OIPF DAE app check
	 */
#endif // !(KEY_HBBTV_TO_OIPF && HBBTV)

#if defined HBBTV && defined KEY_HBBTV_TO_OIPF
	if (!strcmp("keypress", argv[0].u.string) ||
			!strcmp("keyup", argv[0].u.string) ||
			!strcmp("keydown", argv[0].u.string))
	{
		native_AppManPropagateKeyEvent(argv[0].u.string, static_cast<int>(argv[1].u.number));
	}
#else // HBBTV && KEY_HBBTV_TO_OIPF

	ApplicationIdentifier nextActiveAppId;
	NATIVE_CALL(native_AppManGetNextActivatedApplication(NULL, &nextActiveAppId));
	if (!nextActiveAppId)
		return JSP_CALL_NO_VALUE;

	ApplicationKeyEvent* e = NULL;
	if (!strcmp("keypress", argv[0].u.string) ||
		!strcmp("keyup", argv[0].u.string) ||
		!strcmp("keydown", argv[0].u.string))
	{
		if ((e = new ApplicationKeyEvent(this_obj, argv[0].u.string, static_cast<int>(argv[1].u.number), nextActiveAppId)))
		{
			ApplicationManager::sendKeyEventToApp(nextActiveAppId, e);

			delete e;
		}
	}
#endif // HBBTV && KEY_HBBTV_TO_OIPF
	return JSP_CALL_NO_VALUE;
}

//static
int ApplicationManager::__triggerApplicationLoadedEvent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	ApplicationIdentifier appId;

	if (OOIF_RETURN_OK != native_AppManGetApplicationId(OOIFUtils::getWindow(this_obj), OOIFUtils::getHost(this_obj), &appId))
		THROW_GENERIC_ERR(this_obj, result, JSP_CALL_EXCEPTION);

#if !(defined HBBTV && defined KEY_HBBTV_TO_OIPF)
	//check App context since we should only handle this event inside DAE application
	if (!isOipfApplication(appId))
		THROW_GENERIC_ERR(this_obj, result, JSP_CALL_EXCEPTION);
#endif // !(HBBTV && KEY_HBBTV_TO_OIPF)

	// We should propagate this event over all OOIF plugins
	native_AppManPropagateEvent(MANAGER_EVENT_APPLICATION_LOADED, appId);

	return JSP_CALL_NO_VALUE;
}

//static
void ApplicationManager::sendKeyEventToApp(ApplicationIdentifier appId, ApplicationKeyEvent* keyevent)
{
	ENTRY;

	if (!keyevent)
		return;

	std::map<EventTarget*, Event*> targets;
	// Iterate over all instances and dispatch the event
	pthread_mutex_lock(&instancesMutex);
	for (ApplicationManagersList::iterator it=instances.begin(); it!=instances.end(); ++it)
	{
		jsplugin_obj *context = (*it)->getHost();

		ApplicationKeyEvent *e = new ApplicationKeyEvent(*keyevent, *it);
		if (!e)
			continue;

		e->setContext(context);
		EventTarget *target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));

		// We are inserting target to the map to get rid of duplicate deliveries
		// when application for the given id seems to be the same for more than
		// one application manager (when they share the same context).
		if (!targets.insert(std::make_pair(target, e)).second)
			delete e; // target was not added (because it is a duplicate), thus freeing mem.
		else
			(*it)->addProtege(e->getHost());
	}

	pthread_mutex_unlock(&instancesMutex);
	for (std::map<EventTarget*, Event*>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
        if(it->first)
    		it->first->dispatchEvent(it->second);
	}
}
#endif // OIPF || (HBBTV && KEY_HBBTV_TO_OIPF)

#ifdef OIPF

//static
int ApplicationManager::installWidget(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

#if defined(CONFIG_OP_JCOM)
	HLIB_CMD_CaptureLog((const HCHAR*)"WidgetInstallStart,uri=%s", STRING_VALUE(argv[0]));
#endif

	NATIVE_CALL(native_AppManInstallWidget(STRING_VALUE(argv[0])));

	return JSP_CALL_NO_VALUE;
}

//static
int ApplicationManager::uninstallWidget(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	WidgetDescriptor *descriptor = OBJECT_VALUE_OR_THROW(descriptor, argv[0], WidgetDescriptor);
	NATIVE_CALL(native_AppManUninstallWidget(descriptor->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

int ApplicationManager::getWidgetInstallProgress(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	OOIFNumber progress;
	NATIVE_CALL(native_AppManGetWidgetInstallProgress(STRING_VALUE(argv[0]), &progress));
	RETURN_NUMBER(progress, JSP_CALL_VALUE);
}
#endif // OIPF

//static
jsplugin_obj *ApplicationManager::getApplicationById(ApplicationIdentifier appId,
        jsplugin_obj *context)
{
	jsplugin_obj *globalContext = OOIFUtils::getGlobalContext(context);
	jsplugin_obj *app = NULL;

	pthread_mutex_lock(&applicationsMutex);
	if (!applications.count(appId) || (applications[appId].count(globalContext) == 0))
	{
		OOIF_LOG_DEBUG("(Lazy) initialization of application for %p\n", appId);
		// We need to instantiate an object for this window
		Application *application = new Application(appId);
		int r = g_opera_callbacks->create_object(context,
		        ObjectInstance::getter_proxy,
		        ObjectInstance::setter_proxy,
		        ObjectInstance::destructor_proxy,
		        ObjectInstance::gc_proxy,
		        &app);
		if (r != JSP_CREATE_OK)
		{
			OOIF_LOG_ERROR("creating Application failed\n");
			delete application;
			app = NULL;
		}
		else
		{
			app->plugin_private = static_cast<ObjectInstance*>(application);
			application->setHost(app);
			r = g_opera_callbacks->add_unload_listener(app, ObjectInstance::unload_proxy);
			if (r != JSP_CALLBACK_OK)
				OOIF_LOG_WARNING("Failed to add unload listener\n");

			applications[appId][globalContext] = app;
			g_proteges.push_back(app);
		}
	}
	else
	{
		OOIF_LOG_DEBUG("Picking up old application for appId %p\n", appId);
		app = applications[appId][globalContext];
	}
	pthread_mutex_unlock(&applicationsMutex);

	return app;
}

list<jsplugin_obj *> *ApplicationManager::getApplicationInstanceList(ApplicationIdentifier appId)
{
	list<jsplugin_obj *> *appLst = NULL;

	pthread_mutex_lock(&applicationsMutex);
	if (applications.count(appId) != 0)
	{
		appLst = new list<jsplugin_obj *>();

		ApplicationContextsMap::iterator it = applications[appId].begin();
		for (; it != applications[appId].end(); it++)
		{
			appLst->push_back(it->second);
		}
	}
	pthread_mutex_unlock(&applicationsMutex);

	return appLst;
}

//static
void ApplicationManager::initializeApplicationManager()
{
	// map should be more or less empty :)
}

//static
void ApplicationManager::removeApplication(jsplugin_obj *application)
{
	pthread_mutex_lock(&applicationsMutex);
	for (ApplicationsMap::iterator itApp=applications.begin(); itApp!=applications.end(); itApp++)
	{
		for (ApplicationContextsMap::iterator itCtx = itApp->second.begin(); itCtx != itApp->second.end(); itCtx++)
			if (itCtx->second == application)
			{
				OOIF_LOG_DEBUG("Removing application with appId %p and context %p)\n", itCtx->first, itCtx->second);
				g_proteges.remove(application);
				itApp->second.erase(itCtx->first);
				if (itApp->second.empty())
					applications.erase(itApp->first);
				pthread_mutex_unlock(&applicationsMutex);
				return;
			}
	}
	pthread_mutex_unlock(&applicationsMutex);
}

//static
void ApplicationManager::onApplicationManagerEvent(ApplicationManagerEvent event,
        ApplicationIdentifier appId)
{
    if ( isOipfApplication(appId) == FALSE )
    {
        OOIF_LOG_DEBUG("ApplicationManager event received %s, but it's hbbtv appId: %p\n", GetEventName(event), appId);
        return;
    }
    
	Event *e = NULL;
	std::map<EventTarget*, Event*> targets;

	OOIF_LOG_DEBUG("ApplicationManager event received: %s, appId: %p\n", GetEventName(event), appId);

	// Iterate over all instances and dispatch the event
	pthread_mutex_lock(&instancesMutex);
	for (ApplicationManagersList::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		jsplugin_obj *context = (*it)->getHost();
		EventTarget *target;

		switch (event)
		{
		case MANAGER_EVENT_LOW_MEMORY:
			e = new EmptyEvent(context, "LowMemory");
			target = *it;
			break;
#ifdef OIPF
		case MANAGER_EVENT_APPLICATION_LOADED:
			e = new ApplicationLoadedEvent(getApplicationById(appId, context), context);
			target = *it;
			break;
		case MANAGER_EVENT_APPLICATION_UNLOADED:
			e = new ApplicationUnloadedEvent(getApplicationById(appId, context), context);
			target = *it;
			break;
#endif // OIPF
		case MANAGER_EVENT_APPLICATION_LOAD_ERROR:
		{
			jsplugin_obj *app = getApplicationById(appId, context);
			e = new ApplicationLoadErrorEvent(app, context);
			target = *it;
			// application need to be notified to assure "empty"
			(EXTRACT_OBJECT<Application>(app))->markInvalid();
			break;
		}
#ifdef OIPF
		case MANAGER_EVENT_APPLICATION_ACTIVATED:
			e = new EmptyEvent(context, "ApplicationActivated");
			target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));
			OOIF_LOG_DEBUG("MANAGER_EVENT_APPLICATION_ACTIVATED received, context: %p appId: %p\n", context, appId);
			break;
		case MANAGER_EVENT_APPLICATION_DEACTIVATED:
			e = new EmptyEvent(context, "ApplicationDeactivated");
			target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));
			OOIF_LOG_DEBUG("MANAGER_EVENT_APPLICATION_DEACTIVATED received\n");
			break;
		case MANAGER_EVENT_APPLICATION_SHOWN:
			e = new EmptyEvent(context, "ApplicationShown");
			target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));
			OOIF_LOG_DEBUG("MANAGER_EVENT_APPLICATION_SHOWN received\n");
			break;
		case MANAGER_EVENT_APPLICATION_HIDDEN:
			e = new EmptyEvent(context, "ApplicationHidden");
			target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));
			OOIF_LOG_DEBUG("MANAGER_EVENT_APPLICATION_HIDDEN received\n");
			break;
		case MANAGER_EVENT_APPLICATION_PRIMARY_RECEIVER:
			e = new EmptyEvent(context, "ApplicationPrimaryReceiver");
			target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));
			OOIF_LOG_DEBUG("MANAGER_EVENT_APPLICATION_PRIMARY_RECEIVER received\n");
			break;
		case MANAGER_EVENT_APPLICATION_NOT_PRIMARY_RECEIVER:
			e = new EmptyEvent(context, "ApplicationNotPrimaryReceiver");
			target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));
			OOIF_LOG_DEBUG("MANAGER_EVENT_APPLICATION_NOT_PRIMARY_RECEIVER received\n");
			break;
		case MANAGER_EVENT_APPLICATION_TOPMOST:
			e = new EmptyEvent(context, "ApplicationTopmost");
			target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));
			OOIF_LOG_DEBUG("MANAGER_EVENT_APPLICATION_TOPMOST received\n");
			break;
		case MANAGER_EVENT_APPLICATION_NOT_TOPMOST:
			e = new EmptyEvent(context, "ApplicationNotTopmost");
			target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));
			OOIF_LOG_DEBUG("MANAGER_EVENT_APPLICATION_NOT_TOPMOST received\n");
			break;
		case MANAGER_EVENT_APPLICATION_DESTROY_REQUEST:
			e = new EmptyEvent(context, "ApplicationDestroyRequest");
			target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));
			OOIF_LOG_DEBUG("MANAGER_EVENT_APPLICATION_DESTROY_REQUEST received\n");
			break;
		case MANAGER_EVENT_APPLICATION_HIBERNATE_REQUEST:
			e = new EmptyEvent(context, "ApplicationHibernateRequest");
			target = EXTRACT_OBJECT<EventTarget>(getApplicationById(appId, context));
			OOIF_LOG_DEBUG("MANAGER_EVENT_APPLICATION_HIBERNATE_REQUEST received\n");
			break;

#endif
		default:
			pthread_mutex_unlock(&instancesMutex);
			return;
		}
		OOIF_LOG_DEBUG("Dispatching the above event to event target: %p\n", target);
		// We are inserting target to the map to get rid of duplicate deliveries
		// when application for the given id seems to be the same for more than
		// one application manager (when they share the same context).
		if (!targets.insert(std::make_pair(target, e)).second)
			delete e; // target was not added (because it is a duplicate), thus freeing mem.
	}
	for (std::map<EventTarget*, Event*>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
        if (it->first)
    		it->first->dispatchEvent(it->second);
	}
	pthread_mutex_unlock(&instancesMutex);

#if defined OIPF || (defined HBBTV && defined KEY_HBBTV_TO_OIPF)
	if (event == MANAGER_EVENT_APPLICATION_LOADED)
	{
		/* Dispatch this particular event immiediately
		 * to simulate firing ApplicationLoaded event before body.onload
		 */
		dispatchAllEvents(NULL);
	}
#endif // OIPF || (HBBTV && KEY_HBBTV_TO_OIPF)
}

//static
void ApplicationManager::onApplicationDestroyed(ApplicationIdentifier appId)
{
    if ( isOipfApplication(appId) == FALSE )
    {
        OOIF_LOG_DEBUG("onApplicationDestroyed event received , but it's hbbtv appId: %p\n", appId);
        return;
    }

	pthread_mutex_lock(&applicationsMutex);
	ApplicationsMap::iterator it = applications.find(appId);
	if (it != applications.end())
	{
		ApplicationContextsMap appCtxMap = it->second;
		for (ApplicationContextsMap::iterator itCtx = appCtxMap.begin(); itCtx != appCtxMap.end(); ++itCtx)
		{
			g_proteges.remove(itCtx->second);

			// Native representation of the Application object is dead. What is left in JavaScript should
			// be considered an invalid object. JS code should break all the references it has to this
			// Application object in the onApplicationUnloaded event handler. If it does not it has to
			// be prevented from doing any harm by still using this object.
			(EXTRACT_OBJECT<Application>(itCtx->second))->markInvalid();
		}

		appCtxMap.clear();
		applications.erase(appId);
	}
	else
	{
		OOIF_LOG_DEBUG("onApplicationDestroyed called with unrecognized appId %p (never accessed from script?)\n", appId);
	}
	pthread_mutex_unlock(&applicationsMutex);
}

#ifdef OIPF

void ApplicationManager::onApplicationManagerWidgetEvent(ApplicationManagerWidgetEvent event, WidgetDescriptorHandle handle,
        ApplicationManagerWidgetEventState state, ApplicationManagerWidgetEventStateReason reason)
{
	// Iterate over all instances and dispatch the event
	pthread_mutex_lock(&instancesMutex);
	for (ApplicationManagersList::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();
		switch (event)
		{
		case MANAGER_EVENT_WIDGET_INSTALLATION:
		{
			e = new WidgetInstallationEvent(context, handle, state, reason);
			OOIF_LOG_DEBUG("MANAGER_EVENT_WIDGET_INSTALLATION received\n");

#if defined(CONFIG_OP_JCOM)
				if(state == WIDGET_INSTALLATION_COMPLETED)
				{
					char *widgetName;
					native_WidgetDescriptorGetString(WIDGET_NAME, handle, (const char**)&widgetName);
					HLIB_CMD_CaptureLog((const HCHAR*)"WidgetInstallSuccess,WidgetName=%s", widgetName);
				}
				else if(state == WIDGET_INSTALLATION_FAILED)
				{
					HLIB_CMD_CaptureLog((const HCHAR*)"WidgetInstallFail,Reaseon=%d", (int)reason);
				}
#endif
			break;
		}
		case MANAGER_EVENT_WIDGET_UNINSTALLATION:
		{
			e = new WidgetUninstallationEvent(context, handle, state);
			OOIF_LOG_DEBUG("MANAGER_EVENT_WIDGET_UNINSTALLATION received\n");
			break;
		}
		default:
			OOIF_LOG_WARNING("Unknown event received\n");
			pthread_mutex_unlock(&instancesMutex);
			return;
		}
		(*it)->dispatchEvent(e);
	}
	pthread_mutex_unlock(&instancesMutex);
}
#endif // OIPF

//static
const char* ApplicationManager::GetEventName(ApplicationManagerEvent event)
{
	switch (event)
	{
	case MANAGER_EVENT_LOW_MEMORY:
		return "MANAGER_EVENT_LOW_MEMORY";
#ifdef OIPF
	case MANAGER_EVENT_APPLICATION_LOADED:
		return "MANAGER_EVENT_APPLICATION_LOADED";
	case MANAGER_EVENT_APPLICATION_UNLOADED:
		return "MANAGER_EVENT_APPLICATION_UNLOADED";
#endif // OIPF
#ifdef HBBTV
	case MANAGER_EVENT_APPLICATION_LOAD_ERROR:
		return "MANAGER_EVENT_APPLICATION_LOAD_ERROR";
#endif // HBBTV
#ifdef OIPF
	case MANAGER_EVENT_APPLICATION_ACTIVATED:
		return "MANAGER_EVENT_APPLICATION_ACTIVATED";
	case MANAGER_EVENT_APPLICATION_DEACTIVATED:
		return "MANAGER_EVENT_APPLICATION_DEACTIVATED";
	case MANAGER_EVENT_APPLICATION_SHOWN:
		return "MANAGER_EVENT_APPLICATION_SHOWN";
	case MANAGER_EVENT_APPLICATION_HIDDEN:
		return "MANAGER_EVENT_APPLICATION_HIDDEN";
	case MANAGER_EVENT_APPLICATION_PRIMARY_RECEIVER:
		return "MANAGER_EVENT_APPLICATION_PRIMARY_RECEIVER";
	case MANAGER_EVENT_APPLICATION_NOT_PRIMARY_RECEIVER:
		return "MANAGER_EVENT_APPLICATION_NOT_PRIMARY_RECEIVER";
	case MANAGER_EVENT_APPLICATION_TOPMOST:
		return "MANAGER_EVENT_APPLICATION_TOPMOST";
	case MANAGER_EVENT_APPLICATION_NOT_TOPMOST:
		return "MANAGER_EVENT_APPLICATION_NOT_TOPMOST";
	case MANAGER_EVENT_APPLICATION_DESTROY_REQUEST:
		return "MANAGER_EVENT_APPLICATION_DESTROY_REQUEST";
	case MANAGER_EVENT_APPLICATION_HIBERNATE_REQUEST:
		return "MANAGER_EVENT_APPLICATION_HIBERNATE_REQUEST";
#endif
	default:
		return "UNKNOWN";
	}
}

#ifdef OIPF
//static
void ApplicationManager::receiveKey(const char* event_name, int _keyCode)
{
	ENTRY;
	ApplicationIdentifier nextActiveAppId = 0;
	native_AppManGetNextActivatedApplication(NULL, &nextActiveAppId);
	if (!nextActiveAppId)
		return;

	jsplugin_obj *ctx = 0;
	pthread_mutex_lock(&applicationsMutex);
	if (applications.count(nextActiveAppId) && applications[nextActiveAppId].size())
	{
			ctx = applications[nextActiveAppId].begin()->second;
	}
	pthread_mutex_unlock(&applicationsMutex);
	if(!ctx)
		return;

	ApplicationKeyEvent* e = NULL;
	if (!strcmp("keypress", event_name) ||
			!strcmp("keyup", event_name) ||
			!strcmp("keydown", event_name))
	{

		if ((e = new ApplicationKeyEvent(ctx, event_name, _keyCode, nextActiveAppId)) != NULL)
		{
			ApplicationManager::sendKeyEventToApp(nextActiveAppId, e);
			delete e;
		}
	}
}
#endif

#if defined(HMX_WEBUI)
int ApplicationManager::createPortalApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	NATIVE_CALL(native_CreatePortalApplication());
	return JSP_CALL_NO_VALUE;
}

int ApplicationManager::destroyPortalApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	NATIVE_CALL(native_DestroyPortalApplication());
	return JSP_CALL_NO_VALUE;
}

int ApplicationManager::addOIPFApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);

	OOIFBoolean retval;
	NATIVE_CALL(native_AddOIPFApplication(argv[0].u.string, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int ApplicationManager::removeOIPFApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);

	OOIFBoolean retval;
	NATIVE_CALL(native_RemoveOIPFApplication(argv[0].u.string, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int ApplicationManager::removeAllOIPFApplications(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	NATIVE_CALL(native_RemoveAllOIPFApplications());
	return JSP_CALL_NO_VALUE;
}

int ApplicationManager::createHbbTVApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	NATIVE_CALL(native_CreateHbbTVApplication(argv[0].u.number));
	return JSP_CALL_NO_VALUE;
}

int ApplicationManager::destroyHbbTVApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	NATIVE_CALL(native_DestroyHbbTVApplication());
	return JSP_CALL_NO_VALUE;
}

int ApplicationManager::doTeletext(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	NATIVE_CALL(native_DoTeletext());
	return JSP_CALL_NO_VALUE;
}

int ApplicationManager::getHumaxApplications(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	char **apps = NULL;
	int count = 0;

	NATIVE_CALL(native_AppManGetHumaxApplications(&count, &apps));

	RETURN_OBJECT(this_obj, OOIFUtils::CreateStringCollection(count, apps), result, JSP_CALL_VALUE);
}

#ifdef CONFIG_PROD_FVP4000T
int ApplicationManager::createHumaxApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(2);

	NATIVE_CALL(native_AppManCreateHumaxApplication(STRING_VALUE(argv[0]), STRING_VALUE(argv[1])));

	return JSP_CALL_NO_VALUE;
	}

int ApplicationManager::destroyHumaxApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);

	NATIVE_CALL(native_AppManDestroyHumaxApplication(argv[0].u.string));

	return JSP_CALL_NO_VALUE;
}

void ApplicationManager::onHumaxApplicationManagerEvent(int event, const char *app_name)
{
	Event *e = NULL;
	std::map<EventTarget*, Event*> targets;

	OOIF_LOG_DEBUG("ApplicationManager event received: %s, app_name: %p\n", GetEventName((ApplicationManagerEvent)event), app_name);

	// Iterate over all instances and dispatch the event
	pthread_mutex_lock(&instancesMutex);

	for (ApplicationManagersList::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		jsplugin_obj *context = (*it)->getHost();
		EventTarget *target;

		switch (event)
		{
			case MANAGER_EVENT_APPLICATION_LOADED:
				e = new HumaxApplicationLoadedEvent(app_name, context);
				target = *it;
				break;
			case MANAGER_EVENT_APPLICATION_UNLOADED:
				e = new HumaxApplicationUnloadedEvent(app_name, context);
				target = *it;
				break;

			default:
				break;
		}
		if (!targets.insert(std::make_pair(target, e)).second)
			delete e; // target was not added (because it is a duplicate), thus freeing mem.

	}
	for (std::map<EventTarget*, Event*>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
        if(it->first)
    		it->first->dispatchEvent(it->second);
	}

	pthread_mutex_unlock(&instancesMutex);
}

#endif
#endif
