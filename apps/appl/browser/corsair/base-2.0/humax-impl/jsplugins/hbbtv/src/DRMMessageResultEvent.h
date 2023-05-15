// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __DRM_MESSAGE_RESULT_EVENT
#define __DRM_MESSAGE_RESULT_EVENT

#include "Event.h"

namespace Ooif
{
class DRMMessageResultEvent : public Event
{
private:
	char *msgID;
	char *resultMsg;
	int resultCode;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	virtual ~DRMMessageResultEvent();
	DRMMessageResultEvent(jsplugin_obj *c, const char *msgID, const char *resultMsg, int resultCode);

	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __DRM_MESSAGE_RESULT_EVENT
