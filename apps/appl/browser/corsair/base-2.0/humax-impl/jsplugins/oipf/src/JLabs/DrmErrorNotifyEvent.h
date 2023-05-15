// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __DRM_ERROR_NOTIFY_EVENT
#define __DRM_ERROR_NOTIFY_EVENT

#include "Event.h"

namespace Ooif
{
	class DrmErrorNotifyEvent : public Event
	{
	private:
		int error_number;
		char *drm_system_id;
		
	public:
		DrmErrorNotifyEvent(jsplugin_obj *c, int _error_number, const char *_drm_system_id);
		~DrmErrorNotifyEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __DRM_ERROR_NOTIFY_EVENT