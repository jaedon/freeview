// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __JLABS_EMMMESSAGE
#define __JLABS_EMMMESSAGE

#include "NativeJlabsEmmMessage.h"

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

namespace Ooif
{
	class jlabsEmmMessage : public ObjectInstance, public WrapperObject<jlabsEmmMessage>
	{
	private:
		void init();
	public:
		jlabsEmmMessage(jlabsEmmMessageHandle i);
		jlabsEmmMessage(const jlabsEmmMessage &original);
		~jlabsEmmMessage();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static void releaseNative(jlabsEmmMessageHandle id);
		static const ClassType class_name = CLASS_JLABS_EMM_MESSAGE;
	};
} /* namespace Ooif */

#endif // __JLABS_EMMMESSAGE
#endif

