// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __MHEG_TUNE_EVENT
#define __MHEG_TUNE_EVENT

#include "Event.h"
#include "NativeChannel.h"
#include "Channel.h"

namespace Ooif
{
	class MhegTuneEvent : public Event
	{
	private:
		Channel* m_channel;
		OOIFNumber m_ulSvcUid;
		OOIFNumber m_ulTuneMode;
		
	public:
		MhegTuneEvent(jsplugin_obj *context, ChannelHandle channel, OOIFNumber ulSvcUid, OOIFNumber ulTuneMode);
		~MhegTuneEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __MHEG_TUNE_EVENT
#endif // OIPF
