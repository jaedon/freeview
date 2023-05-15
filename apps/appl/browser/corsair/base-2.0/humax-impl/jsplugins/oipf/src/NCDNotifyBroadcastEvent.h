// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __NCD_CHANGED_EVENT
#define __NCD_CHANGED_EVENT

#include "Event.h"

namespace Ooif
{
	class NCDNotifyBroadcastEvent : public Event
	{
	private:
		OOIFNumber m_ulStartTimeOfChange;
		OOIFNumber m_ulASODuration;
		char *m_pNcdMessage;
		
	public:
		NCDNotifyBroadcastEvent(jsplugin_obj *context, OOIFNumber ulStartTimeOfChange, OOIFNumber ulASODuration,const char *pszNcdMessage);
		~NCDNotifyBroadcastEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __NCD_CHANGED_EVENT
#endif // OIPF
