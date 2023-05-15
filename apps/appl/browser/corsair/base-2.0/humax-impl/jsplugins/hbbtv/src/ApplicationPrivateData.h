// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __APPLICATIONPRIVATEDATA
#define __APPLICATIONPRIVATEDATA

#include "ObjectInstance.h"
#include "Application.h"

namespace Ooif
{
	class ApplicationPrivateData : public ObjectInstance
	{
	private:
		ApplicationIdentifier _appId;

	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

	public:
		ApplicationPrivateData(ApplicationIdentifier appId);
		~ApplicationPrivateData();

		// Specific functions for this interface
		static int getFreeMem(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
		static int prepareWakeupApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int prepareWakeupOITF(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int clearWakeupToken(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
		static const ClassType class_name = CLASS_APPLICATIONPRIVATEDATA;
	};
} /* namespace Ooif */

#endif // __APPLICATIONPRIVATEDATA
