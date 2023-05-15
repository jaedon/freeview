// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __NETWORK_URI_CHANGED_EVENT
#define __NETWORK_URI_CHANGED_EVENT

#include "Event.h"

namespace Ooif
{
	class NetworkUriNotifyBroadcastEvent : public Event
	{
	private:
		char *m_pNetworkUri;

	public:
		NetworkUriNotifyBroadcastEvent(jsplugin_obj *context, const char *pszNetworkUriMessage);
		~NetworkUriNotifyBroadcastEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __NETWORK_URI_CHANGED_EVENT
#endif // OIPF
