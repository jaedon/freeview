// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __PVR_EVENT
#define __PVR_EVENT

#include "Event.h"
#include "NativeRecording.h"
#include "PVR.h"
#include "NativeOOIFTypes.h"

namespace Ooif
{
class PVREvent : public Event
{
private:
	OOIFNumber _state;
	PVRHandle _recordingHandle;
protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
public:
	PVREvent(jsplugin_obj *c, OOIFNumber state, PVRHandle recordingHandle);
	virtual ~PVREvent();
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */
#endif // __PVR_EVENT
#endif // OIPF
