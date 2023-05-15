// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/*

  Should keep a map of jsplugin Applications

  map<appID, Application*>
  dispatchEvent(appID, ApplicationEventType);

  enum ApplicationEventType {
      ApplicationActivated
	  ApplicationDeactivated
	  ApplicationShown
	  ApplicationHidden
	  ApplicationPrimaryReceiver
	  ApplicationNotPrimaryReceiver
	  ApplicationTopmost
	  ApplicationNotTopmost
	  ApplicationDestroyRequest
	  LowMemory
  }

  getOwnerApplication(...) {
     Look up the appID from the native layer and return the applicatio n
	 Comparison between different applications, is that a problem?
  }

  Application {
      appID id;

	  // Returns false if there are destroy listeners, calls destroy
	  // itself after those are finished otherwise returns true and
	  // should be destroyed
	  bool destroyRequest();

	  window ?!
	  // Return a window proxy?! IMPOSSIBLE
	  // Store a reference to it somehow, when the object is created? Timing issues? Asynchronously return values. Security issues?!
	  //  ... There does not seem to be a way to do this while complying to the whole spec, especially security issues, hmm.

  }

  // This is not possible is it?
  // ... NO! We get internal WRONG_THIS_ERR, postMessage is only supposed to be run on the window object, makes sense.
  WindowProxy {
      // Filter out everything except postMessage
	  // But the postMessage getter should probably return a function pointer to window.postMessage
	  // So how do we get a pointer to postMessage, without timing issues?
	  // cb_eval(obj, "window.postMessage", 0, callback);
      postMessage();
  }
 */

#ifndef __APPLICATIONMANAGER
#define __APPLICATIONMANAGER

#include "jsplugin.h"
#include "EventTarget.h"

#include <map>
#include <pthread.h>

#include "NativeApplicationManager.h"
#ifdef OIPF
#include "WidgetDescriptorCollection.h"
#endif // OIPF

namespace Ooif
{
class ApplicationKeyEvent;

class ApplicationManager : public EventTarget
{
public:
	static bool isOipfApplication(ApplicationIdentifier appId);

	ApplicationManager();
	virtual ~ApplicationManager();
	// Specific functions for this interface
	static int getOwnerApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
	static int getChildApplications(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getApplicationVisualizationMode(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int gc(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int installWidget(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int uninstallWidget(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getWidgetInstallProgress(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF

#if defined OIPF || (defined HBBTV && defined KEY_HBBTV_TO_OIPF)
	static int __key(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int __triggerApplicationLoadedEvent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void sendKeyEventToApp(ApplicationIdentifier appId, ApplicationKeyEvent* keyevent);
#endif // OIPF || (HBBTV && KEY_HBBTV_TO_OIPF)
#ifdef OIPF
		static void receiveKey(const char* event_name, int _keyCode);
#endif

	// Non-JS functions
	static jsplugin_obj *getApplicationById(ApplicationIdentifier id, jsplugin_obj *context);
	static list<jsplugin_obj *> *getApplicationInstanceList(ApplicationIdentifier appId);
	static void initializeApplicationManager();
	static void removeApplication(jsplugin_obj *application);
	// Callbacks from platform
	static void onApplicationManagerEvent(ApplicationManagerEvent event, ApplicationIdentifier appId);
	static void onApplicationDestroyed(ApplicationIdentifier appId);
#ifdef OIPF
	static void onApplicationManagerWidgetEvent(ApplicationManagerWidgetEvent event, WidgetDescriptorHandle handle,
	        ApplicationManagerWidgetEventState state, ApplicationManagerWidgetEventStateReason reason);
#endif // OIPF
	static const ClassType class_name = CLASS_APPLICATIONMANAGER;

	// Types
	typedef map<jsplugin_obj*, jsplugin_obj*> ApplicationContextsMap;
	typedef map<ApplicationIdentifier, ApplicationContextsMap> ApplicationsMap;
	typedef list<ApplicationManager*> ApplicationManagersList;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	int destructor(jsplugin_obj *obj);

private:
	static const char *GetEventName(ApplicationManagerEvent event);
	// static map
	static ApplicationsMap applications;
	static pthread_mutex_t applicationsMutex;
	// static list of all ApplicationManager instances
	static ApplicationManagersList instances;
	//mutex protecting ApplicationManagers
	static pthread_mutex_t instancesMutex;

#if defined(HMX_WEBUI)
public:
	static int createPortalApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int destroyPortalApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int addOIPFApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int removeOIPFApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int removeAllOIPFApplications(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createHbbTVApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int destroyHbbTVApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int doTeletext(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getHumaxApplications(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

#ifdef CONFIG_PROD_FVP4000T
    static int createHumaxApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
    static int destroyHumaxApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void onHumaxApplicationManagerEvent(int event, const char *app_name);
#endif
#endif
};
} /* namespace Ooif */

#endif // __APPLICATIONMANAGER
