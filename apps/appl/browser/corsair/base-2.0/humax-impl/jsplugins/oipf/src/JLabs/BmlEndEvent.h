// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __BML_END_EVENT
#define __BML_END_EVENT

#include "Event.h"

namespace Ooif
{
	class BMLEndEvent : public Event
	{
	private:
		int		cause;
		char	*param;
		
	public:
		BMLEndEvent(jsplugin_obj *c, int cause, const char *param);
		~BMLEndEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __BML_END_EVENT