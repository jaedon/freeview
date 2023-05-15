// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __UPLOAD_PPV_VIEWLOG_DATA_EVENT
#define __UPLOAD_PPV_VIEWLOG_DATA_EVENT

#include "Event.h"

namespace Ooif
{
	class UploadPPVViewlogDataEvent : public Event
	{
	private:
		OOIFNumber uploadResult;

	public:
		UploadPPVViewlogDataEvent(jsplugin_obj *c, OOIFNumber _uploadResult);
		~UploadPPVViewlogDataEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __UPLOAD_PPV_VIEWLOG_DATA_EVENT

