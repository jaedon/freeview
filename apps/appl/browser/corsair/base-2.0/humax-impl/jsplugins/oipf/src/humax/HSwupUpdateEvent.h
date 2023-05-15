// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __SWUPDATE_UPDATE_EVENT
#define __SWUPDATE_UPDATE_EVENT

#include "Event.h"

namespace Ooif
{
	class HSwupdateUpdateEvent : public Event
	{
	private:
		OOIFNumber m_Source;
		OOIFNumber m_State;
		OOIFNumber m_Progress;
		OOIFNumber m_Progressmax;
		OOIFNumber m_Err;

	public:
		HSwupdateUpdateEvent(jsplugin_obj *context, OOIFNumber _source, OOIFNumber _state, OOIFNumber _progress, OOIFNumber _progressmax, OOIFNumber _err);
		~HSwupdateUpdateEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __SWUPDATE_UPDATE_EVENT
#endif // OIPF




