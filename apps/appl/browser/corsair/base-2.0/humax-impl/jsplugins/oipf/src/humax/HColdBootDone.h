// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_COLD_BOOT_DONE
#define __H_COLD_BOOT_DONE 

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeHConfiguration.h"

#include "opllocalsystem.h"


#define ____HMX_ColdBootDone_functions____


typedef void (*hColdBootDoneEventListener)(void);

OOIFReturnCode native_HColdBootDone_getColdBootDone(OOIFBoolean *retval, hColdBootDoneEventListener listener);
OOIFReturnCode native_HColdBootDone_releaseListener(void);
OOIFReturnCode native_HColdBootDoneSetListener(hColdBootDoneEventListener listener);

void native_HColdBootDoneEventListener(void);

namespace Ooif
{
	class HColdBootDone : public EventTarget
	{
	private:
		static list<HColdBootDone*> 	instances; // for hadling events.


	public:
		HColdBootDone();
		~HColdBootDone();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int getColdBootDone(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int releaseColdBootDoneListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static void onColdBootDoneEvent(void);			

		static const ClassType class_name = CLASS_COLD_BOOT_DONE;

	};
} /* namespace Ooif */

#endif // __H_COLD_BOOT_DONE
#endif // OIPF


