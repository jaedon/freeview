// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __CAS_ACTION_IR_EMM_FORCED_OTA_EVENT
#define __CAS_ACTION_IR_EMM_FORCED_OTA_EVENT

#include "Event.h"

namespace Ooif
{
	class HCasActionIrEmmForcedOtaEvent : public Event
	{
	private:
		OOIFNumber m_param1;
		OOIFNumber m_param2;
		OOIFNumber m_param3;

	public:
		HCasActionIrEmmForcedOtaEvent (jsplugin_obj *context, OOIFNumber __param1, OOIFNumber __param2, OOIFNumber __param3);
		~HCasActionIrEmmForcedOtaEvent ();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __CAS_ACTION_IR_EMM_FORCED_OTA_EVENT
#endif // OIPF

