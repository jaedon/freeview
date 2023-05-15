// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __SWUPDATE_DETECT_EVENT
#define __SWUPDATE_DETECT_EVENT

#include "Event.h"

namespace Ooif
{
	class HSwupdateDetectEvent : public Event
	{
	private:
		OOIFNumber m_Source;
		OOIFNumber m_State;
		char	m_Curver[128];
		char 	m_Foundver[128];
		OOIFNumber m_Err;

	public:
		HSwupdateDetectEvent(jsplugin_obj *context, OOIFNumber _source, OOIFNumber _state, const char *curver, const char *foundver, OOIFNumber _err);
		~HSwupdateDetectEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __SWUPDATE_DETECT_EVENT
#endif // OIPF



