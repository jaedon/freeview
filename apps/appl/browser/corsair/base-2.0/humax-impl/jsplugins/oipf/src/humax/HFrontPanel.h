// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_FRONTPANEL
#define __H_FRONTPANEL

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeHConfiguration.h"


namespace Ooif
{
	class HFrontPanel : public EventTarget
	{

	public:
		HFrontPanel();
		~HFrontPanel();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int setString(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static int setLed(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		//static int setCurrentLiveInform(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		//static int setCurrentRecordingInform(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		// animation function
		static int setAnimation(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		// category set
		static int setCurrentMediaInform(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

	};
} /* namespace Ooif */

#endif // __H_FRONTPANEL
#endif // OIPF

