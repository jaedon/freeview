// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __CLOCK_RECOVERY_EVENT
#define __CLOCK_RECOVERY_EVENT

#include "Event.h"

namespace Ooif
{
	class HClockRecoveryEvent : public Event
	{
	private:
		OOIFNumber m_batchType;
		OOIFNumber m_batchProcType;
		OOIFNumber m_err;

	public:
		HClockRecoveryEvent(jsplugin_obj *context, OOIFNumber _batchType, OOIFNumber _batchProcType, OOIFNumber _err);
		~HClockRecoveryEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __CLOCK_RECOVERY_EVENT
#endif // OIPF





