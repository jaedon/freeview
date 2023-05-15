// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __REQUEST_PINCODE_EVENT
#define __REQUEST_PINCODE_EVENT

#include "Event.h"

namespace Ooif
{
	class RequestPincodeEvent : public Event
	{
	private:
		char *message;

	public:
		RequestPincodeEvent(jsplugin_obj *c, const char *_message);
		~RequestPincodeEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif //__REQUEST_PINCODE_EVENT

/* EOF */


