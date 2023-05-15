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
#include "Recording.h"

namespace Ooif
{
	class RecordingScheduler : public EventTarget
	{
	private:
		static list<RecordingScheduler*> recordingSchedulers;
#ifdef OIPF
        ScheduledRecordingCollection *getAllRecordings(int recordingsCount, ScheduledRecordingHandle *handles);
#else /* OIPF */
        RecordingCollection *getAllRecordings(int recordingsCount, RecordingHandle *handles);
#endif /* OIPF */
#if !defined(JLABS) && defined(HMX_WEBUI)
		static void onRecordingFileOperationEvent(int ticket, int result);
#endif /* #if !defined(JLABS) && defined(HMX_WEBUI) */

#ifdef OIPF
	protected:
		virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);
#endif /* OIPF */

	public:
		RecordingScheduler();
		~RecordingScheduler();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		// Javascript functions
		static int record(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int recordAt(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int recordCrid(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int removeSeries(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getScheduledRecordings(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getChannelConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int createProgrammeObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
		static int getRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int refresh(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF

		static int getCurrentRecordings(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		// Static member functions
#ifdef OIPF
		static void onRecordingEvent(RecordingHandle handle, OOIFNumber state);
#ifdef HMX_WEBUI
		static void onScheduleEvent(OOIFNumber state, ScheduledRecordingHandle handle, OOIFNumber error, ScheduledRecordingHandle *conclicts, int count, ScheduledRecordingHandle *alternatives, int alt_count, ProgrammeHandle *altevents, int alt_evt_count);
		static void onRecordEvent(OOIFNumber state, ScheduledRecordingHandle handle, OOIFNumber error);

		static int startSatRecord(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setDoNotDelete(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int recordConfirm(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int update(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int copyRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int moveRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int removeRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stopFileOp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setSeriesLock(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getCountScheduleList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getCountRecordingList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getScheduleList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getRecordingList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif //HMX_WEBUI
#endif // OIPF
#ifdef JLABS
		static int getScheduledRecordingConflictInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int copy(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getRecordings(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setDefaultDMS(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getDefaultDMSDeviceHandle(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getDefaultDMSRecordDestinationId(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void onHNReservationFinishedEvent(ScheduledRecordingHandle recording, OOIFNumber errorCode);
		static void onHNRemoveResultEvent(char *strRemove, OOIFNumber errorCode);
		static HDiscInfoCollection	*getDiscs(int count, HDiscInfoHandle *handles);
#endif // JLABS
		static const ClassType class_name = CLASS_RECORDINGSCHEDULER;
	};
} /* namespace Ooif */

#endif // __RECORDINGSCHEDULER
#endif // HBBTV_PVR || OIPF
