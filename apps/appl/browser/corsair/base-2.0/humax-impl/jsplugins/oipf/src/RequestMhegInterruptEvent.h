// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __MHEG_INTERRUPT_EVENT
#define __MHEG_INTERRUPT_EVENT

#include "Event.h"
#include "NativeChannel.h"
#include "Channel.h"

namespace Ooif
{
	class MhegInterruptEvent : public Event
	{
	private:
		OOIFNumber m_ulInterruptibleFlag;
		
	public:
		MhegInterruptEvent(jsplugin_obj *context, OOIFNumber ulInterruptibleFlag);
		~MhegInterruptEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __MHEG_INTERRUPT_EVENT
#endif // OIPF
