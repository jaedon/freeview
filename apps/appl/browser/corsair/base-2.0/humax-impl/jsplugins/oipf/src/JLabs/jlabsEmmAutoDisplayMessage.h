// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __JLABSEMMAUTODISPLAYMESSAGE__
#define __JLABSEMMAUTODISPLAYMESSAGE__

#if defined OIPF && defined JLABS

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "NativeJlabsEmmAutoDisplayMessage.h"

namespace Ooif
{
	class jlabsEmmAutoDisplayMessage : public ObjectInstance, public WrapperObject<jlabsEmmAutoDisplayMessage>
	{
	private:
		void init();
	public:
		jlabsEmmAutoDisplayMessage(jlabsEmmAutoDisplayMessageHandle i);
		jlabsEmmAutoDisplayMessage(const jlabsEmmAutoDisplayMessage &original);
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative(jlabsEmmAutoDisplayMessageHandle id);
		static const ClassType class_name = CLASS_JLABS_EMM_AUTO_DISPLAY_MESSAGE;
		// JS functions
	};
} /* namespace Ooif */

#endif

#endif // __JLABSEMMAUTODISPLAYMESSAGE__
