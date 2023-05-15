// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(HBBTV_PVR) || defined(OIPF)
#ifndef __RECORDINGSCHEDULER
#define __RECORDINGSCHEDULER

#include "jsplugin.h"

#include "EventTarget.h"
#include "Collection.h"
#include "NativeRecording.h"
#include "PVR.h"

namespace Ooif
{
class RecordingScheduler : public EventTarget
{
private:
	static list<RecordingScheduler*> recordingSchedulers;
	static PVRCollection *getAllRecordings(int recordingsCount, PVRHandle *handles);
protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
public:
	RecordingScheduler();
	~RecordingScheduler();
	// Javascript functions
	static int record(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int recordAt(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getScheduledRecordings(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getChannelConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createProgrammeObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
	static int stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int refresh(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void onRecordingSchedulerEvent(RecordingSchedulerEvent event, RecordingSchedulerEventInfo info);
#endif // OIPF
	static const ClassType class_name = CLASS_RECORDINGSCHEDULER;
};
} /* namespace Ooif */

#endif // __RECORDINGSCHEDULER
#endif // HBBTV_PVR || OIPF
