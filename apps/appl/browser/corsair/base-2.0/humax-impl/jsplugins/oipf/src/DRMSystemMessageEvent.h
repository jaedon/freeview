// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifndef __DRM_SYSTEM_MESSAGE_EVENT
#define __DRM_SYSTEM_MESSAGE_EVENT

#include "Event.h"

namespace Ooif
{
class DRMSystemMessageEvent : public Event
{
private:
	char *msg;
	char *DRMSystemID;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	DRMSystemMessageEvent(jsplugin_obj *c, const char *msg, const char *DRMSystemID);
	virtual ~DRMSystemMessageEvent();

	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __DRM_SYSTEM_MESSAGE_EVENT
#endif // OIPF || HBBTV_1_2_1
