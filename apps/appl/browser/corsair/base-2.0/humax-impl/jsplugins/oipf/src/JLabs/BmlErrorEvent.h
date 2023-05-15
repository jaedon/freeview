// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __BML_ERROR_EVENT
#define __BML_ERROR_EVENT

#include "Event.h"

namespace Ooif
{
	class BmlErrorEvent : public Event
	{
	private:
		int error_number;
		
	public:
		BmlErrorEvent(jsplugin_obj *c, int _error_number);
		~BmlErrorEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __BML_ERROR_EVENT