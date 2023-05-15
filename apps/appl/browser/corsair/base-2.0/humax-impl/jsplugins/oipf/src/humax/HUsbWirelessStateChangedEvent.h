// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __USBWLAN_STATE_CHANGE_EVENT
#define __USBWLAN_STATE_CHANGE_EVENT

#include "Event.h"

namespace Ooif
{
	class HUsbWirelessStateChangedEvent : public Event
	{
	private:
		OOIFNumber usbWlanDevId;
		OOIFNumber usbWlanState;

	public:
		HUsbWirelessStateChangedEvent(jsplugin_obj *context, OOIFNumber _usbWlanDevId, OOIFNumber _usbWlanState);
		~HUsbWirelessStateChangedEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */

#endif // __USBWLAN_STATE_CHANGE_EVENT