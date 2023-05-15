// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __JLABSNETWORKSERVICE_EVENT__
#define __JLABSNETWORKSERVICE_EVENT__

#include "Event.h"

namespace Ooif
{
	/**
	   Generic class for events with no arguments.
	*/

	class jlabsNetworkServiceEventResult: public Event
	{
	public:
		~jlabsNetworkServiceEventResult();
		jlabsNetworkServiceEventResult(jsplugin_obj *c, int nStatus);

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	private :
		int m_nStatus;

	};
} /* namespace Ooif */

#endif // __JLABSNETWORKSERVICE_EVENT__
