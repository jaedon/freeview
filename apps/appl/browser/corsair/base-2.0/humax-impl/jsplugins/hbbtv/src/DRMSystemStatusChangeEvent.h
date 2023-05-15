// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __DRM_SYSTEM_STATUS_CHANGE_EVENT
#define __DRM_SYSTEM_STATUS_CHANGE_EVENT

#include "Event.h"

namespace Ooif
{
class DRMSystemStatusChangeEvent : public Event
{
private:
	char *DRMSystemID;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	DRMSystemStatusChangeEvent(jsplugin_obj *c, const char *DRMSystemID);
	virtual ~DRMSystemStatusChangeEvent();

	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __DRM_SYSTEM_STATUS_CHANGE_EVENT
#endif // OIPF
