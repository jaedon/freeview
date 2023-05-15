// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
#ifndef __DOWNLOAD_STATE_CHANGE_EVENT
#define __DOWNLOAD_STATE_CHANGE_EVENT

#include "Event.h"

#include "Download.h"

namespace Ooif
{
class DownloadStateChangeEvent : public Event
{
private:
	Download download;
	OOIFNumber state;
	OOIFBoolean has_reason;
	OOIFNumber reason;

public:
	DownloadStateChangeEvent(jsplugin_obj *context, DownloadHandle item, OOIFNumber state, OOIFBoolean has_reason, OOIFNumber reason);
	~DownloadStateChangeEvent();

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

	virtual int serialize(jsplugin_value **values);
};
} /* namespace Ooif */
#endif // __DOWNLOAD_STATE_CHANGE_EVENT
#endif // HBBTV_DOWNLOAD || OIPF
