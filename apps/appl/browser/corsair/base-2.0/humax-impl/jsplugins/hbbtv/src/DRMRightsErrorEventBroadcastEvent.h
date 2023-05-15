// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __DRM_RIGHTS_ERROR_EVENT_BROADCAST_EVENT
#define __DRM_RIGHTS_ERROR_EVENT_BROADCAST_EVENT

#include "Event.h"

namespace Ooif
{
class DRMRightsErrorEventBroadcastEvent : public Event
{
private:
	int errorState;
	char *contentID;
	char *DRMSystemID;
	char *rightsIssuerURL;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	/**
	   @param c object event occured on
	   @param s new speed
	 */
	DRMRightsErrorEventBroadcastEvent(jsplugin_obj *c, int error_state, const char* content_id,
	                                  const char* drm_system_id, const char* rights_issuer_url);
	virtual ~DRMRightsErrorEventBroadcastEvent();

	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __DRM_RIGHTS_ERROR_EVENT_BROADCAST_EVENT
