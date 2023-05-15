// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_LOCALSYSTEM
#define __H_LOCALSYSTEM

#include "jsplugin.h"
#include "EventTarget.h"
#include "NativeHConfiguration.h"
#include "Collection.h"
#include "ObjectInstance.h"

typedef void (*hLocalSystemClockRecoveryEventListener)(int batchType, int batchProcType, int err);
OOIFReturnCode native_HLocalSystem_startClockRecovery(hLocalSystemClockRecoveryEventListener listener);
OOIFReturnCode native_HLocalSystemClockRecoverySetListener(hLocalSystemClockRecoveryEventListener listener);
void native_HLocalSystemClockRecoveryEventListener(int batchType, int batchProcType, int err);

namespace Ooif
{
	class HLocalSystem : public EventTarget
	{
	private:
		static list<HLocalSystem*> instances;
		HNetworkInterfaceCollection *getNetworkInterfaces(int count, HNetworkInterfaceHandle *handles);
		AVOutputCollection *getAVOutputs(int count, AVOutputHandle *handles);
#if defined(HMX_WEBUI)
		TunerCollection		*getTuners(int count, TunerHandle *handles);
		HMX_OutputCollection *getHmxOutputs(int count, HMX_OutputHandle *handles);
		HMX_AudioCollection *getHmxAudios(int count, HMX_AudioHandle *handles);
#endif

	protected:
		virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);

	public:
		HLocalSystem();
		~HLocalSystem();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		// Javascript functions
		static int setScreenSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setPvrSupport(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setPowerState(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setDigestCredentials(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int clearDigestCredentials(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setApplicationReady(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int showMsgbox(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int showKeyboard(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		// Event Listener
		static void onLocalSystemEvent(HLocalSystemEvent event, HLocalSystemEventInfo *info);
		static const ClassType class_name = CLASS_LOCAL_SYSTEM;
#if defined(HMX_WEBUI)
		static int getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int registerEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int unregisterEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int checkFileExistence(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif
		// Opera TV Browser
		static int startTVBrowser(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getStatusOfTVBrowser(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

	};
} /* namespace Ooif */

#endif // __H_LOCALSYSTEM
#endif // OIPF
