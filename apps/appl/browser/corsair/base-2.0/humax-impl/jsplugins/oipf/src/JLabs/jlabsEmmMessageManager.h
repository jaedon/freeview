// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined OIPF && defined JLABS

#ifndef __JLABS_EMMMESSAGEMANAGER
#define __JLABS_EMMMESSAGEMANAGER

#include "jsplugin.h"
#include "ObjectInstance.h"

namespace Ooif
{
	class jlabsEmmMessageManager : public ObjectInstance
	{
	public:
		jlabsEmmMessageManager();
		~jlabsEmmMessageManager();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		// JS functions

		static int stopAutoDisplayMessage (jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getAllEmmMessage (jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getAllSelfMessage (jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getAllBoardMessage (jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static const ClassType class_name = CLASS_JLABS_EMM_MESSAGE_MANAGER;
	};
} /* namespace Ooif */

#endif // __JLABS_EMMMESSAGEMANAGER
#endif

