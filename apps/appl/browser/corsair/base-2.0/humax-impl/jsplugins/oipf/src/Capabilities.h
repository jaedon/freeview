// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __CAPABILITIES
#define __CAPABILITIES

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "NativeCapabilities.h"

namespace Ooif
{
	class Capabilities : public ObjectInstance
	{
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	public:
		// Javascript functions
		static int hasCapability(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	private:
		int getXmlCapabilities(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	};
} /* namespace Ooif */

#endif // __CAPABILITIES
