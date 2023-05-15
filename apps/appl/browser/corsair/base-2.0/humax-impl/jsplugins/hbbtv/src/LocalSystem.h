// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/** 
 * @ToDo originally, localSystem property is not supported for HbbTV requirement. 
   but the Juke Box application require the property to login, so we remove OIPF condition.
   so I marked which code are fixed due to above issue with JUKEBOX SUPPORT as a comment.
 */
//#ifdef OIPF //JUKEBOX_SUPPORT
#ifndef __LOCALSYSTEM
#define __LOCALSYSTEM

#include "jsplugin.h"
#include "NativeConfiguration.h"
#include "EventTarget.h"
#include "Collection.h"

namespace Ooif
{
class LocalSystem : public EventTarget
{
private:
	static list<LocalSystem*> instances;
#ifdef OIPF //JUKEBOX_SUPPORT
	NetworkInterfaceCollection *getNetworkInterfaces(int count, NetworkInterfaceHandle *handles);
	AVOutputCollection *getAVOutputs(int count, AVOutputHandle *handles);
	TunerCollection *getTuners(int count, TunerHandle *handles);
#endif
protected:
#ifdef OIPF //JUKEBOX_SUPPORT
	virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);
#endif
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	LocalSystem();
	virtual ~LocalSystem();

#ifdef OIPF //JUKEBOX_SUPPORT
	// Javascript functions
	static int setScreenSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setPvrSupport(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setPowerState(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setDigestCredentials(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int clearDigestCredentials(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	// Event Listener
	static void onLocalSystemEvent(LocalSystemEvent event, LocalSystemEventInfo* info);
#endif
	static const ClassType class_name = CLASS_LOCAL_SYSTEM;
};
} /* namespace Ooif */

#endif // __LOCALSYSTEM
//#endif // OIPF
