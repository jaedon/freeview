// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __BML_START_EVENT
#define __BML_START_EVENT

#include "Event.h"

namespace Ooif
{
	class BMLStartEvent : public Event
	{
	private:
		OOIFBoolean autoFlag;
		
	public:
		BMLStartEvent(jsplugin_obj *c, OOIFBoolean autoFlag);
		~BMLStartEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __BML_START_EVENT