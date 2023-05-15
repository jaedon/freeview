// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_PVR) || defined(OIPF)

#include "RecordingScheduler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "OOIFUtils.h"

#include "NativeRecording.h"
#include "NativeProgramme.h"
#include "NativeCapabilities.h"
#include "NativeDisc.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include "PVR.h"
#include "Collection.h"
#include "ChannelConfig.h"
#include "DiscInfo.h"
#include "PVREvent.h"
#include "Programme.h"

using namespace Ooif;

list<RecordingScheduler*> RecordingScheduler::recordingSchedulers;

RecordingScheduler::RecordingScheduler()
{
	ENTRY;
	classType = CLASS_RECORDINGSCHEDULER;
	recordingSchedulers.push_back(this);
}

RecordingScheduler::~RecordingScheduler()
{
	ENTRY;
	RecordingScheduler::recordingSchedulers.remove(this);
}

int RecordingScheduler::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY

	GET_FUNCTION(record, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(recordAt, obj, "iiis", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getScheduledRecordings, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getChannelConfig, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(remove, obj, "-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createProgrammeObject, obj, "", JSP_GET_VALUE_CACHE);

	NATIVE_GET_VOIDP_COLLECTION("recordings", obj,
	                            native_RecordingSchedulerGetRecordings(OOIFUtils::getHost(obj), &SIZE, &NATIVE_ARRAY),
	                            getAllRecordings(SIZE, NATIVE_ARRAY),
	                            JSP_GET_VALUE);

#ifdef OIPF
	NATIVE_GET_OBJECT("discInfo", obj, native_DiscInfoOnCreate(), new DiscInfo(), JSP_GET_VALUE);
	GET_FUNCTION(stop, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(refresh, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getRecording, obj, "s", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(PVREvent);

	return EventTarget::getter(obj, name, result);
#else
	return JSP_GET_NOTFOUND;
#endif // OIPF
}

int RecordingScheduler::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
#ifdef OIPF
	INTRINSIC_EVENT(PVREvent);
#endif  // OIPF
	return JSP_PUT_NOTFOUND;
}

//static
int RecordingScheduler::recordAt(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                 int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(4);

	PVRHandle pvri;
	NATIVE_CALL(native_RecordingSchedulerRecordAt(NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]),
	            NUMBER_VALUE(argv[2]), STRING_VALUE(argv[3]),
	            OOIFUtils::getHost(this_obj), &pvri));
	RETURN_OBJECT(this_obj, new PVR(pvri), result, JSP_CALL_VALUE);
}

//static
int RecordingScheduler::record(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	Programme *p = OBJECT_VALUE_OR_THROW(p, argv[0], Programme);

	PVRHandle pvri;
	NATIVE_CALL(native_RecordingSchedulerRecord(p->getIdentifier(), OOIFUtils::getWindow(this_obj),
	            OOIFUtils::getHost(this_obj), &pvri));
	RETURN_OBJECT(this_obj, new PVR(pvri), result, JSP_CALL_VALUE);
}

//static
int RecordingScheduler::remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);

	PVR *pvr = OBJECT_VALUE_OR_THROW(pvr, argv[0], PVR);
	NATIVE_CALL(native_RecordingSchedulerRemove(pvr->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int RecordingScheduler::getScheduledRecordings(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	PVRHandle *handles = NULL;
	int recordingCount;
	NATIVE_CALL(native_RecordingSchedulerGetScheduledRecordings(OOIFUtils::getHost(this_obj),
	            &recordingCount, &handles));
	RETURN_OBJECT(this_obj, getAllRecordings(recordingCount, handles), result, JSP_CALL_VALUE);
}

//static
int RecordingScheduler::getChannelConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
        jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new ChannelConfig(RECORDING), result, JSP_CALL_VALUE);
}

//
int RecordingScheduler::createProgrammeObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	ProgrammeHandle id;
	NATIVE_CALL(native_ProgrammeCreate(&id, OOIFUtils::getWindow(this_obj),
	                                   OOIFUtils::getHost(this_obj)));
	RETURN_OBJECT(this_obj, new Programme(id), result, JSP_CALL_VALUE);
}

//static
PVRCollection *RecordingScheduler::getAllRecordings(int recordingsCount, PVRHandle *handles)
{
	PVRCollection *collection = new PVRCollection();
	for (int i=0; i<recordingsCount; i++)
	{
		collection->addItem(new PVR(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

#ifdef OIPF
//static
int RecordingScheduler::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                             int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	PVR *pvr = OBJECT_VALUE_OR_THROW(pvr, argv[0], PVR);
	NATIVE_CALL(native_RecordingSchedulerStop(pvr->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int RecordingScheduler::refresh(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	NATIVE_CALL(native_RecordingSchedulerRefresh());
	return JSP_CALL_NO_VALUE;
}

//static
int RecordingScheduler::getRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                     int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	PVRHandle pvri;
	NATIVE_CALL(native_RecordingSchedulerGetRecording(STRING_VALUE(argv[0]), &pvri));
	RETURN_OBJECT(this_obj, new PVR(pvri), result, JSP_CALL_VALUE);
}

//static
void RecordingScheduler::onRecordingSchedulerEvent(RecordingSchedulerEvent event, RecordingSchedulerEventInfo info)
{
	for (list<RecordingScheduler*>::iterator it=recordingSchedulers.begin(); it!=recordingSchedulers.end(); it++)
	{
		RecordingScheduler *rs = *it;
		Event* e = NULL;
		switch (event)
		{
		case RECORDING_SCHEDULER_PVR_EVENT:
		{
			e = new PVREvent(rs->getHost(), info.pvr_event_data.state, info.pvr_event_data.id);
			break;
		}
		default:
			return;
		}
		rs->dispatchEvent(e);
	}
}
#endif // OIPF
#endif // HBBTV_PVR || OIPF
