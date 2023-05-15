// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __APPLICATION
#define __APPLICATION

#include "jsplugin.h"
#include "EventTarget.h"

#include "ApplicationManager.h"

#ifdef HMX_WEBUI
#include <string>
#endif

namespace Ooif
{
class Application : public EventTarget
{
private:
	void init();

	/** Is the Application still usable from JavaScript? Sometimes a native representation of the
	 * object can be removed/invalidated, while the JavaScript representation is still being
	 * held alive by some scripts. In such case the object can't be forcefully removed and
	 * should be kept in a state that will prevent scripts for doing any harm. Any function
	 * call or property access on this object will trigger an exception.
	 */
	bool valid;

protected:
	ApplicationIdentifier appId;
#ifdef OIPF
	jsplugin_value domWindow;
#endif

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	Application(ApplicationIdentifier id);
	Application(const Application &original);
	~Application();

	void markInvalid()
	{
		valid = false;
	}

	ApplicationIdentifier getAppId() const
	{
		return appId;
	}

	/** Check if the application runs in the given JS context.
	 *
	 * \param context JavaScript context to check
	 * \return true if the given context is the one in which the application runs in,
	 *         false otherwise
	 */
	static bool checkAppContext(jsplugin_obj *context);

	// Specific functions for this interface
	static int show(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int hide(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int destroyApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
	static int activateInput(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int deactivateInput(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int startWidget(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int stopWidget(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void onApplicationEvent(ApplicationEvent event, ApplicationIdentifier appId);
#endif // OIPF

#ifdef OIPF
	static void getWindowAsyncCallback(int status, jsplugin_value *return_value, void *user_data);
	static const char *GetEventName(ApplicationEvent event);
#endif
#ifdef HMX_WEBUI
	static std::string m_ContentUrl; 
#endif
	static const ClassType class_name = CLASS_APPLICATION;

};
} /* namespace Ooif */

#endif // __APPLICATION
