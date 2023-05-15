// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_CAS_ACTION_IR_EMM_FORCED_OTA
#define __H_CAS_ACTION_IR_EMM_FORCED_OTA 

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeHConfiguration.h"

#include "opllocalsystem.h"


#define ____HMX_CasActionIrEmmForcedOta_functions____


typedef void (*hCasActionIrEmmForcedOtaEventListener)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);


OOIFReturnCode native_HCasActionIrEmmForcedOta_setListener(hCasActionIrEmmForcedOtaEventListener listener);
OOIFReturnCode native_HCasActionIrEmmForcedOta_releaseListener(hCasActionIrEmmForcedOtaEventListener listener);

OOIFReturnCode native_HCasActionIrEmmForcedOtaEventSetListener(hCasActionIrEmmForcedOtaEventListener listener);
void native_HCasActionIrEmmForcedOtaEventListener(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);


namespace Ooif
{
	class HCasActionIrEmmForcedOta : public EventTarget
	{
	private:
		static list<HCasActionIrEmmForcedOta*> 	instances; // for hadling events.


	public:
		HCasActionIrEmmForcedOta();
		~HCasActionIrEmmForcedOta();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int setListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int releaseListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static void onCasActionIrEmmForcedOtaEvent(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);	

		static const ClassType class_name = CLASS_CAS_ACTION_IR_EMM_FORCED_OTA;

	};
} /* namespace Ooif */

#endif // __H_CAS_ACTION_IR_EMM_FORCED_OTA
#endif // OIPF


