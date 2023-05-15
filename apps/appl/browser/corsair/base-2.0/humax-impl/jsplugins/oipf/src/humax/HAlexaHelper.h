// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_ALEXA_HELPER
#define __H_ALEXA_HELPER

#include "jsplugin.h"
#include "EventTarget.h"
#include "Collection.h"
#include "ObjectInstance.h"

namespace Ooif
{
	class HAlexaHelper : public EventTarget
	{
	private:
		static list<HAlexaHelper*> instances;

	public:
		HAlexaHelper();
		virtual ~HAlexaHelper();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static const ClassType class_name = CLASS_HMX_ALEXA_HELPER;
		
		static int processData(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

	};
} /* namespace Ooif */

#endif // __H_ALEXA_HELPER
#endif // OIPF
