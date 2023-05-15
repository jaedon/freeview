// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __EMPTY_EVENT
#define __EMPTY_EVENT

#include "Event.h"

namespace Ooif
{
	/**
	   Generic class for events with no arguments.
	*/

	class jlabsNetworkInterfaceEventResult: public Event
	{
	public:
		const char *m_EvnetType;
		~jlabsNetworkInterfaceEventResult();
		jlabsNetworkInterfaceEventResult(jsplugin_obj *c, const char* name);

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */

#endif // __EMPTY_EVENT
