// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __RECORDING_EVENT_BROADCAST_EVENT
#define __RECORDING_EVENT_BROADCAST_EVENT

#include "Event.h"

namespace Ooif
{
class RecordingEventBroadcastEvent : public Event
{
private:
	OOIFNumber state;
	OOIFBoolean has_error;
	OOIFNumber error;
	char *recordingID;
public:
	RecordingEventBroadcastEvent(jsplugin_obj *c, OOIFNumber _state, OOIFBoolean _has_error, OOIFNumber _error, const char *_recordingID);
	~RecordingEventBroadcastEvent();

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __RECORDING_EVENT_BROADCAST_EVENT
