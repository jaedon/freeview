// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __KEYBOARD_RESULT_EVENT
#define __KEYBOARD_RESULT_EVENT

#include "Event.h"

namespace Ooif
{
	class HKeyboardResultEvent : public Event
	{
	private:
		char* resultStr;
	public:
		HKeyboardResultEvent(jsplugin_obj *context, char* resultStr);
		~HKeyboardResultEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __KEYBOARD_RESULT_EVENT
#endif // OIPF
