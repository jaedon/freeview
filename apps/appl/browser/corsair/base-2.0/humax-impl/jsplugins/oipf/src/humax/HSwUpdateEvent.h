// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __SWUPDATE_EVENT
#define __SWUPDATE_EVENT

#include "Event.h"
#include "HSoftwareUpdate.h"
#include "oplswupdate.h"

namespace Ooif
{
	class HSwUpdateEvent : public Event
	{
	private:
		DxSwUpdate_EventData_t	*m_pstSwUpdateEvent;

	public:
		HSwUpdateEvent(jsplugin_obj *context, DxSwUpdate_EventData_t *pstSwEvent);
		~HSwUpdateEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __SWUPDATE_EVENT
#endif // OIPF



