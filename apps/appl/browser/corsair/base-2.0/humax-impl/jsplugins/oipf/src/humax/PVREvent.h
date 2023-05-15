// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __HUMAX_PVR_EVENT
#define __HUMAX_PVR_EVENT

#if defined(OIPF) && defined(HMX_WEBUI)
#include <PVREvent.h>

namespace humax
{
	class PVREvent : public Ooif::PVREvent
	{
	private:
		int 	error;

		Ooif::ScheduledRecording			*schedule;
		Ooif::Recording						*recording;
		Ooif::ScheduledRecordingCollection	*conflicts;
		Ooif::NumberCollection				*conflictGroupInfo;
		Ooif::ScheduledRecordingCollection	*alternatives;
		Ooif::ProgrammeCollection			*altevents;
		/**
		 * SchedueldRecording과 Recording이 상속관계아 아니라, 두 개로 나눠 관리하지만,
		 * 나중에 오페라에서 고쳐주면, 하나로 통합 관리해야 함
		 */
		Ooif::RecordingCollection				*conflictRecs;

	public:
		~PVREvent();

		PVREvent(int state, ScheduledRecordingHandle recording, int error, ScheduledRecordingHandle *conflicts, int count, ScheduledRecordingHandle *alternatives, int alt_count, ProgrammeHandle *altevents,int alt_evt_count, jsplugin_obj *c);

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		int serialize(jsplugin_value **argv);

	private:
		Ooif::ScheduledRecordingCollection*	getScheduledRecordingConflicts ();
		Ooif::ScheduledRecordingCollection*	getScheduledRecordingAlternatives ();
		Ooif::ProgrammeCollection*			getScheduledRecordingAltEvents ();
		Ooif::RecordingCollection*			getRecordingConflicts ();
		Ooif::NumberCollection*				getScheduledRecordingGroupInfo();
	};
}// namespace humax

#endif // OIPF

#endif // !__HUMAX_PVR_EVENT
