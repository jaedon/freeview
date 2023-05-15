// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __JLABS_BOARDMESSAGE
#define __JLABS_BOARDMESSAGE

#include "NativeJlabsBoardMessage.h"

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

namespace Ooif
{
	class jlabsBoardMessage : public ObjectInstance, public WrapperObject<jlabsBoardMessage>
	{
	private:
		void init();
	public:
		jlabsBoardMessage(jlabsBoardMessageHandle i);
		jlabsBoardMessage(const jlabsBoardMessage &original);
		~jlabsBoardMessage();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static void releaseNative(jlabsBoardMessageHandle id);
		static const ClassType class_name = CLASS_JLABS_BOARD_MESSAGE;
	};
} /* namespace Ooif */

#endif // __JLABS_BOARDMESSAGE
#endif


