// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Application.h"
#include "ApplicationManager.h"

#include "NativeApplicationManager.h"
#include "NativeSecurity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "EmptyEvent.h"
#include "ApplicationPrivateData.h"

#ifdef OIPF
#include "WidgetDescriptor.h"
#endif // OIPF

// Object validity
#define CHECK_OBJECT_VALID(context, app, result, returntype) \
	if (!app->valid) { \
		THROW_EXCEPTION(context, "Error", "This Application object has been unloaded. Please make sure " \
						"that your code breaks all the references to unloaded Applications.", \
						result, returntype); \
	}


using namespace Ooif;

void Application::init()
{
	classType = CLASS_APPLICATION;
	valid = true;
#ifdef OIPF
	domWindow.type = JSP_TYPE_UNDEFINED;
	domWindow.u.object = NULL;
#endif // OIPF
}

Application::Application(ApplicationIdentifier id) : appId(id)
{
	ENTRY;
	init();
}

Application::Application(const Application &original) : appId(original.appId)
{
	ENTRY;
	init();
}


Application::~Application()
{
	ENTRY;
	OOIF_LOG_DEBUG("Application removed, URL probably changed");
	ApplicationManager::removeApplication(host);
#ifdef OIPF
	if (domWindow.type == JSP_TYPE_OBJECT || domWindow.type == JSP_TYPE_NATIVE_OBJECT)
		removeProtege(domWindow.u.object);
#endif //OIPF
}

//static
bool Application::checkAppContext(jsplugin_obj *context)
{
	Application* app = EXTRACT_OBJECT<Application>(context);
	long int win_id = 0;
	if (OOIF_RETURN_OK == native_AppManGetWindowOfApplication(app->getAppId(), &win_id))
		return OOIFUtils::getWindow(context) == win_id;

	return false;
}

int Application::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	if (!valid)
	{
		// HbbTV Errata #1
		RETURN_UNDEFINED(JSP_GET_VALUE);
	}

	GET_FUNCTION(show, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(hide, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createApplication, obj, "sb", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(destroyApplication, obj, "", JSP_GET_VALUE_CACHE);

	if (!strcmp(name, "privateData"))
	{
		if (checkAppContext(obj))
			GET_OBJECT("privateData", obj, new ApplicationPrivateData(appId), JSP_GET_VALUE);
		else
			THROW_SECURITY_ERR(obj, result, JSP_GET_EXCEPTION);
	}

#ifdef OIPF
	NATIVE_GET_STRING_COLLECTION("permissions", obj,
	                             native_ApplicationGetPermissions(appId, &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("visible", native_ApplicationGetVisible(appId, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("active", native_ApplicationGetActive(appId, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isPrimaryReceiver", native_ApplicationGetIsPrimaryReceiver(appId, &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(activateInput, obj, "b", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(deactivateInput, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(startWidget, obj, "ob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopWidget, obj, "o", JSP_GET_VALUE_CACHE);
	
	GET_FUNCTION(addEventListener, obj, "sos", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, obj, "sos", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(ApplicationActivated);
	INTRINSIC_EVENT_GET(ApplicationDeactivated);
	INTRINSIC_EVENT_GET(ApplicationShown);
	INTRINSIC_EVENT_GET(ApplicationHidden);
	INTRINSIC_EVENT_GET(ApplicationPrimaryReceiver);
	INTRINSIC_EVENT_GET(ApplicationNotPrimaryReceiver);
	INTRINSIC_EVENT_GET(ApplicationTopmost);
	INTRINSIC_EVENT_GET(ApplicationNotTopmost);
	INTRINSIC_EVENT_GET(ApplicationDestroyRequest);
	INTRINSIC_EVENT_GET(ApplicationHibernateRequest);

	INTRINSIC_EVENT_GET(KeyPress);
	INTRINSIC_EVENT_GET(KeyUp);
	INTRINSIC_EVENT_GET(KeyDown);

	if (strcmp(name, "window") == 0)
	{
		Application* app = EXTRACT_OBJECT<Application>(obj);
		if (app->domWindow.type != JSP_TYPE_UNDEFINED)
		{
			*result = app->domWindow;
			return JSP_GET_VALUE;
		}
		else
		{
			long int win_id = 0;
			NATIVE_GET(native_AppManGetWindowOfApplication(app->appId, &win_id));
			jsplugin_obj* context = OOIFUtils::getGlobalContext(win_id);
			if (NULL == context)
			{
				THROW_GENERIC_ERR(obj, result, JSP_GET_EXCEPTION);
			}
			int res = g_opera_callbacks->eval(context, "window", app, &Application::getWindowAsyncCallback);
			if (JSP_CALLBACK_OK != res)
			{
				THROW_GENERIC_ERR(obj, result, JSP_GET_EXCEPTION);
			}
			OOIF_LOG_DEBUG("Requesting window object from DOM, initial status: %d", res);
			result->type = JSP_TYPE_NULL;
			return JSP_GET_DELAYED;
		}
	}

	return EventTarget::getter(obj, name, result);
#else
	return JSP_GET_NOTFOUND;
#endif // OIPF
}

int Application::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	CHECK_OBJECT_VALID(obj, this, value, JSP_PUT_EXCEPTION);

	SET_READONLY("privateData");
#ifdef OIPF
	INTRINSIC_EVENT(ApplicationActivated);
	INTRINSIC_EVENT(ApplicationDeactivated);
	INTRINSIC_EVENT(ApplicationShown);
	INTRINSIC_EVENT(ApplicationHidden);
	INTRINSIC_EVENT(ApplicationPrimaryReceiver);
	INTRINSIC_EVENT(ApplicationNotPrimaryReceiver);
	INTRINSIC_EVENT(ApplicationTopmost);
	INTRINSIC_EVENT(ApplicationNotTopmost);
	INTRINSIC_EVENT(ApplicationDestroyRequest);
	INTRINSIC_EVENT(ApplicationHibernateRequest);

	SET_READONLY("visible");
	SET_READONLY("active");
	SET_READONLY("isPrimaryReceiver");
	SET_READONLY("permissions");
	SET_READONLY("window");

	INTRINSIC_EVENT(KeyPress);
	INTRINSIC_EVENT(KeyUp);
	INTRINSIC_EVENT(KeyDown);
#endif // OIPF

	return JSP_PUT_NOTFOUND;
}

//static
int Application::show(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	Application *app = EXTRACT_OBJECT_OR_THROW(app, this_obj, Application);
	CHECK_OBJECT_VALID(this_obj, app, result, JSP_CALL_EXCEPTION);

	NATIVE_CALL(native_ApplicationShow(app->appId));
	return JSP_CALL_NO_VALUE;
}

int Application::hide(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	Application *app = EXTRACT_OBJECT_OR_THROW(app, this_obj, Application);
	CHECK_OBJECT_VALID(this_obj, app, result, JSP_CALL_EXCEPTION);

	NATIVE_CALL(native_ApplicationHide(app->appId));
	return JSP_CALL_NO_VALUE;
}

int Application::createApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	Application *app = EXTRACT_OBJECT_OR_THROW(app, this_obj, Application);
	CHECK_OBJECT_VALID(this_obj, app, result, JSP_CALL_EXCEPTION);
	bool createChild = BOOLEAN_VALUE(argv[1]);
#ifdef HBBTV_1_2_1
	if (createChild)
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
#endif // HBBTV_1_2_1

	ApplicationIdentifier id;
	NATIVE_CALL(native_ApplicationCreateApplication(STRING_VALUE(argv[0]), createChild, app->appId, &id));

	jsplugin_obj *app_obj = ApplicationManager::getApplicationById(id, this_obj);
	if (!app_obj)
		return JSP_CALL_ERROR;

	result->u.object = app_obj;
	result->type = JSP_TYPE_OBJECT;
	return JSP_CALL_VALUE;
}

//static
int Application::destroyApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	Application *app = EXTRACT_OBJECT_OR_THROW(app, this_obj, Application);
	CHECK_OBJECT_VALID(this_obj, app, result, JSP_CALL_EXCEPTION);

	native_HmxSetReactiveAit(true);

	NATIVE_CALL(native_ApplicationDestroyApplication(app->appId));

	return JSP_CALL_NO_VALUE;
}

#ifdef OIPF
//static
int Application::activateInput(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	//check App context since we should only handle this event inside OIPF/DAE application
	long int window_identifier = OOIFUtils::getWindow(this_obj);
	ApplicationIdentifier appId;
	if (OOIF_RETURN_OK != native_AppManGetApplicationId(window_identifier, OOIFUtils::getHost(this_obj), &appId))
		return JSP_CALL_NO_VALUE;

	OOIFBoolean isOipfApp = false;
	if (OOIF_RETURN_OK != native_AppManIsOipfApplication(appId, &isOipfApp))
		return JSP_CALL_NO_VALUE;

	if (isOipfApp)
	{
		CHECK_ARGUMENT_COUNT(1);
		Application *app = EXTRACT_OBJECT_OR_THROW(app, this_obj, Application);
		CHECK_OBJECT_VALID(this_obj, app, result, JSP_CALL_EXCEPTION);

		NATIVE_CALL(native_ApplicationActivateInput(app->appId, BOOLEAN_VALUE(argv[0])));
	}

	return JSP_CALL_NO_VALUE;
}

//static
int Application::deactivateInput(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	Application *app = EXTRACT_OBJECT_OR_THROW(app, this_obj, Application);
	CHECK_OBJECT_VALID(this_obj, app, result, JSP_CALL_EXCEPTION);

	NATIVE_CALL(native_ApplicationDeactivateInput(app->appId));

	return JSP_CALL_NO_VALUE;
}

//static
int Application::startWidget(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	Application* app = EXTRACT_OBJECT_OR_THROW(app, this_obj, Application);
	WidgetDescriptor* wd = OBJECT_VALUE_OR_THROW(wd, argv[0], WidgetDescriptor);
	ApplicationIdentifier id;
	CHECK_OBJECT_VALID(this_obj, app, result, JSP_CALL_EXCEPTION);

	NATIVE_CALL(native_ApplicationStartWidget(wd->getIdentifier(), BOOLEAN_VALUE(argv[1]), app->appId, &id));

	jsplugin_obj *app_obj = ApplicationManager::getApplicationById(id, this_obj);
	if (!app_obj)
		return JSP_CALL_ERROR;

	result->u.object = app_obj;
	result->type = JSP_TYPE_OBJECT;
	return JSP_CALL_VALUE;
}

//static
int Application::stopWidget(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	Application* app = EXTRACT_OBJECT_OR_THROW(app, this_obj, Application);
	WidgetDescriptor *wd OBJECT_VALUE_OR_THROW(wd, argv[0], WidgetDescriptor);
	CHECK_OBJECT_VALID(this_obj, app, result, JSP_CALL_EXCEPTION);

	NATIVE_CALL(native_ApplicationStopWidget(wd->getIdentifier(), app->appId));
	return JSP_CALL_NO_VALUE;
}

void Application::getWindowAsyncCallback(int status, jsplugin_value *return_value, void *user_data)
{
	ENTRY;
	OOIF_LOG_DEBUG("Returned from evaluating window object status: %d type: %d", status, return_value->type);
	Application* app = reinterpret_cast<Application*>(user_data);

	if (status == JSP_CALLBACK_OK)
	{
		app->domWindow = *return_value;
		if (app->domWindow.type == JSP_TYPE_OBJECT || app->domWindow.type == JSP_TYPE_NATIVE_OBJECT)
			app->addProtege(app->domWindow.u.object);
	}

	g_opera_callbacks->resume(app->getHost());
}

//static
void Application::onApplicationEvent(ApplicationEvent event,
                                     ApplicationIdentifier appId)
{
	const char *evtName = NULL;

	OOIF_LOG_DEBUG("Application event received: %s, appId: %p", GetEventName(event), appId);

	switch (event)
	{
	case APPLICATION_EVENT_ACTIVATED:
		evtName = "ApplicationActivated";
		break;
	case APPLICATION_EVENT_DEACTIVATED:
		evtName = "ApplicationDeactivated";
		break;
	case APPLICATION_EVENT_SHOWN:
		evtName = "ApplicationShown";
		break;
	case APPLICATION_EVENT_HIDDEN:
		evtName = "ApplicationHidden";
		break;
	case APPLICATION_EVENT_PRIMARY_RECEIVER:
		evtName = "ApplicationPrimaryReceiver";
		break;
	case APPLICATION_EVENT_NOT_PRIMARY_RECEIVER:
		evtName = "ApplicationNotPrimaryReceiver";
		break;
	case APPLICATION_EVENT_TOPMOST:
		evtName = "ApplicationTopmost";
		break;
	case APPLICATION_EVENT_NOT_TOPMOST:
		evtName = "ApplicationNotTopmost";
		break;
	case APPLICATION_EVENT_DESTROY_REQUEST:
		evtName = "ApplicationDestroyRequest";
		break;
	case APPLICATION_EVENT_HIBERNATE_REQUEST:
		evtName = "ApplicationHibernateRequest";
		break;
	default:
		break;
	}

	list<jsplugin_obj *> *apps = ApplicationManager::getApplicationInstanceList(appId);
	if (apps == NULL)
	{
		// JavaScript code does not hold any references to Application objects.
		return;
	}

	list<jsplugin_obj *>::iterator it = apps->begin();
	for (; it != apps->end(); it++)
	{
		EventTarget *target = EXTRACT_OBJECT<EventTarget>(*it);
		target->dispatchEvent(new EmptyEvent(*it, evtName));
	}

	apps->clear();
	delete apps;
}

//static
const char *Application::GetEventName(ApplicationEvent event)
{
	switch (event)
	{
	case APPLICATION_EVENT_ACTIVATED:
		return "APPLICATION_EVENT_ACTIVATED";
	case APPLICATION_EVENT_DEACTIVATED:
		return "APPLICATION_EVENT_DEACTIVATED";
	case APPLICATION_EVENT_SHOWN:
		return "APPLICATION_EVENT_SHOWN";
	case APPLICATION_EVENT_HIDDEN:
		return "APPLICATION_EVENT_HIDDEN";
	case APPLICATION_EVENT_PRIMARY_RECEIVER:
		return "APPLICATION_EVENT_PRIMARY_RECEIVER";
	case APPLICATION_EVENT_NOT_PRIMARY_RECEIVER:
		return "APPLICATION_EVENT_NOT_PRIMARY_RECEIVER";
	case APPLICATION_EVENT_TOPMOST:
		return "APPLICATION_EVENT_TOPMOST";
	case APPLICATION_EVENT_NOT_TOPMOST:
		return "APPLICATION_EVENT_NOT_TOPMOST";
	case APPLICATION_EVENT_DESTROY_REQUEST:
		return "APPLICATION_EVENT_DESTROY_REQUEST";
	case APPLICATION_EVENT_HIBERNATE_REQUEST:
		return "APPLICATION_EVENT_HIBERNATE_REQUEST";
	default:
		return "UNKNOWN";
	}
}
#endif // OIPF
