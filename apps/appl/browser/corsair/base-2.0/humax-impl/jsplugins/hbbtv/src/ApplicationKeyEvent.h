// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2012 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef APPLICATIONKEYEVENT_H
#define	APPLICATIONKEYEVENT_H

#include <cstring>
#include "Event.h"
#include "Application.h"

class ApplicationManger;

namespace Ooif
{
	class ApplicationKeyEvent : public Event
	{
	private:
		int		keyCode;
		bool	cancelBubble;
		char*	keyEventType;
		
		bool	eventSentToNextApp;
		ApplicationIdentifier	appId;
		int		listenerCounter;
		ApplicationManager* manager;
		
		void propagateEventInAppStack();

	public:
		ApplicationKeyEvent(jsplugin_obj *context, const char* event_name, int _keyCode, ApplicationIdentifier _app);
		ApplicationKeyEvent(const ApplicationKeyEvent& event, ApplicationManager *am);
		~ApplicationKeyEvent();

		void setContext(jsplugin_obj* c) { context = c; }
		
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		int serialize(jsplugin_value **argv);
		
		static int stopPropagation(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		
		static bool AppKeyEventName(const char *name) { return !strcmp(name, "KeyUp") || !strcmp(name, "KeyPress") || !strcmp(name, "KeyDown"); }
		
		void	dispatched() { ++listenerCounter; }
		void*	getUserData() { return (void*)this; }
		static void async_callback(int status, jsplugin_value *return_value, void *user_data);
		jsplugin_async_callback* getListenerCallback() { return ApplicationKeyEvent::async_callback; }		
		void notFired() {if (listenerCounter == 0) propagateEventInAppStack();}
	};
} /* namespace Ooif */



#endif	/* APPLICATIONKEYEVENT_H */

