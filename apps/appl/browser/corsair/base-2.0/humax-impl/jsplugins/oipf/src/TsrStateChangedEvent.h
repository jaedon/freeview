// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __TSR_STATE_CHANGED_EVENT
#define __TSR_STATE_CHANGED_EVENT

#include "Event.h"

namespace Ooif
{
	class TsrStateChangedEvent : public Event
	{
	private:
		OOIFBoolean	m_bIsPaused;

	public:
		TsrStateChangedEvent(jsplugin_obj *context, OOIFBoolean isPaused);
		~TsrStateChangedEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __TSR_STATE_CHANGED_EVENT
#endif // OIPF
