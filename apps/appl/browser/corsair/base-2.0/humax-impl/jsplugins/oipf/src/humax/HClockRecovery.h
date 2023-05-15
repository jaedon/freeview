// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_CLOCK_RECOVERY
#define __H_CLOCK_RECOVERY

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeHConfiguration.h"

#include "opllocalsystem.h"


#define ____HMX_ClockRecovery_functions____


typedef void (*hClockRecoveryEventListener)(HUINT32 batchType, HUINT32 batchProcType, HUINT32 err);

OOIFReturnCode native_HClockRecovery_startClockRecovery(hClockRecoveryEventListener listener);
OOIFReturnCode native_HClockRecoverySetListener(hClockRecoveryEventListener listener);

void native_HClockRecoveryEventListener(HUINT32 batchType, HUINT32 batchProcType, HUINT32 err);


namespace Ooif
{
	class HClockRecovery : public EventTarget
	{
	private:
		static list<HClockRecovery*> 	instances; // for hadling events.


	public:
		HClockRecovery();
		~HClockRecovery();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int startClockRecovery(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static void onClockRecoveryEvent(HUINT32 batchType, HUINT32 batchProcType, HUINT32 err);

		static const ClassType class_name = CLASS_CLOCK_RECOVERY;

	};
} /* namespace Ooif */

#endif // __H_CLOCK_RECOVERY
#endif // OIPF


