// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __CHANNEL_IMPORT_EXPORT_NOTIFIER_EVENT
#define __CHANNEL_IMPORT_EXPORT_NOTIFIER_EVENT

#include "Event.h"

namespace Ooif
{
	class HChannelImportExportEvent : public Event
	{
	private:
		OOIFNumber m_Type;
		OOIFNumber m_Result;

	public:
		HChannelImportExportEvent(jsplugin_obj *context, OOIFNumber _type, OOIFNumber _result);
		~HChannelImportExportEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __CHANNEL_IMPORT_EXPORT_NOTIFIER_EVENT
#endif // OIPF



