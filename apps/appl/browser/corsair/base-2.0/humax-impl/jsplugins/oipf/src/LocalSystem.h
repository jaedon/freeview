// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
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
	NetworkInterfaceCollection *getNetworkInterfaces(int count, NetworkInterfaceHandle *handles);
	AVOutputCollection *getAVOutputs(int count, AVOutputHandle *handles);
	TunerCollection *getTuners(int count, TunerHandle *handles);

protected:
	virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	LocalSystem();
	virtual ~LocalSystem();

	// Javascript functions
	static int setScreenSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setPvrSupport(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setPowerState(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setDigestCredentials(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int clearDigestCredentials(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	// Event Listener
	static void onLocalSystemEvent(LocalSystemEvent event, LocalSystemEventInfo* info);
	static const ClassType class_name = CLASS_LOCAL_SYSTEM;
};
} /* namespace Ooif */

#endif // __LOCALSYSTEM
#endif // OIPF
