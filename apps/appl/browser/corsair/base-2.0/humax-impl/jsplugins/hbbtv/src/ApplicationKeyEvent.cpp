// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2012 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined OIPF || (defined HBBTV && defined KEY_HBBTV_TO_OIPF)

#include "NativeApplicationManager.h"
#include "ApplicationKeyEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctype.h"

#include "macros.h"
#include "ApplicationManager.h"
#include "jsplugin.h"

using namespace Ooif;


ApplicationKeyEvent::ApplicationKeyEvent(jsplugin_obj *context, const char* event_name, int _keyCode, ApplicationIdentifier _app)
	: Event(context, event_name),
		keyCode(_keyCode),
		cancelBubble(false),
		keyEventType(HLIB_STD_StrDup(event_name)),
		eventSentToNextApp(false),
		appId(_app),
		listenerCounter(0),
		manager(NULL)
{
	ENTRY;
	//we are assuming that event_name is lower-case string (except 1st and 4th chars)
	if (eventName && strlen(eventName) > 3)
	{
		// 1st and 4th letters should be upper ones, the rest are lowercase (according to DAE specs)
		eventName[0] = toupper(eventName[0]);
		eventName[3] = toupper(eventName[3]);
	}
	if (keyEventType && strlen(keyEventType) > 3)
	{
		// 'keyevent.type' should be lowercase string
		keyEventType[0] = tolower(keyEventType[0]);
		keyEventType[3] = tolower(keyEventType[3]);
	}
}

ApplicationKeyEvent::ApplicationKeyEvent(const ApplicationKeyEvent& event, ApplicationManager *am)
	: Event(event.context, event.eventName),
		keyCode(event.keyCode),
		cancelBubble(false),
		keyEventType(HLIB_STD_StrDup(event.keyEventType)),
		eventSentToNextApp(false),
		appId(event.appId),
		listenerCounter(0),
		manager(am)
{
	ENTRY;
}

ApplicationKeyEvent::~ApplicationKeyEvent()
{
	ENTRY;
	HLIB_STD_MemFree(keyEventType);
}

int ApplicationKeyEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_BOOLEAN("cancelBubble", cancelBubble, JSP_GET_VALUE_CACHE);
	GET_BOOLEAN("cancelable",	FALSE, JSP_GET_VALUE_CACHE);
	GET_BOOLEAN("bubbles",		TRUE, JSP_GET_VALUE_CACHE);

	GET_NUMBER("which",		keyCode, JSP_GET_VALUE_CACHE);
	GET_NUMBER("keyCode",	keyCode, JSP_GET_VALUE_CACHE);
	GET_STRING("type",		keyEventType, JSP_GET_VALUE_CACHE);

	GET_FUNCTION(stopPropagation, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int ApplicationKeyEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	SET_BOOLEAN("cancelBubble",	cancelBubble, JSP_PUT_OK);
	SET_NUMBER("which",			keyCode, JSP_PUT_OK);
	SET_NUMBER("keyCode",		keyCode, JSP_PUT_OK);

	return JSP_PUT_NOTFOUND;
}

int ApplicationKeyEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	// we strongly assume that serialize is only used for IntrinsicEvents
	listenerCounter++;

	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_OBJECT;
	result[0].u.object = this->getHost();


	setWaitForGC();

	return 1;
}

//static
int ApplicationKeyEvent::stopPropagation(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
											int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	ApplicationKeyEvent *keyEvent = EXTRACT_OBJECT<ApplicationKeyEvent>(this_obj);
	if (!keyEvent)
		return JSP_CALL_ERROR;

	keyEvent->cancelBubble = true;

	return JSP_CALL_NO_VALUE;
}

// static
void ApplicationKeyEvent::async_callback(int status, jsplugin_value *return_value, void *user_data)
{
	ENTRY;
	if (status != JSP_CALLBACK_OK || !user_data)
	{
		// we should assume that event may not be propagated to next app
		return;
	}

	if (ApplicationKeyEvent* keyevent = static_cast<ApplicationKeyEvent*>(user_data))
	{
		if ((--keyevent->listenerCounter) == 0)
		{
			keyevent->propagateEventInAppStack();
		}
	}
}

void ApplicationKeyEvent::propagateEventInAppStack()
{
	ENTRY;
	if (manager)
		manager->removeProtege(this->getHost());
	if (cancelBubble || eventSentToNextApp)
		return;

	eventSentToNextApp = true;

	//check App context since we should propagate the event only from original application
	if (!Application::checkAppContext(ApplicationManager::getApplicationById(appId, context)))
		return;

	//it's time to send it to next active application
	ApplicationIdentifier nextActiveAppId;
	OOIFReturnCode ret = native_AppManGetNextActivatedApplication(appId, &nextActiveAppId);
	if (ret == OOIF_RETURN_OK && nextActiveAppId)
	{
		ApplicationKeyEvent* tmpEvent = new ApplicationKeyEvent(*this, NULL);
		if (!tmpEvent)
			return;

		tmpEvent->appId = nextActiveAppId;
		tmpEvent->eventSentToNextApp = false;

		ApplicationManager::sendKeyEventToApp(nextActiveAppId, tmpEvent);

		delete tmpEvent;
	}
}

#endif // OIPF || (HBBTV && KEY_HBBTV_TO_OIPF)
