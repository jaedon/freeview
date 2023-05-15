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
#include "Recording.h"
#include "Programme.h"

namespace Ooif
{
class PVREvent : public Event
{
private:
	Recording recording;
#if !defined(HMX_WEBUI)
	int state;
#else
protected:
	int state;
#endif

public:
	virtual ~PVREvent();

	PVREvent(RecordingHandle recordingHandle, int state, jsplugin_obj *c);

	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */
#endif // __PVR_EVENT
#endif // OIPF
