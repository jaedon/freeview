// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_PVR) || defined(OIPF)

#include "NativeRecording.h"
#include "NativeProgramme.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oplprogramme.h"
#include "oplrecorder.h"
#include "oplscheduler.h"
#include "oplrecorditem.h"

#ifdef JLABS
#include "opldiscinfo.h"
#include "opldmp.h"
#endif

#if defined(HMX_WEBUI)
#include "oplfilemgr.h"
#include "oplstorageutil.h"
#endif //HMX_WEBUI

static RecordingConstants	__recording_ToRecordingConstants(int nativeState)
{
	RecordingConstants	oipfState = RECORDING_UNREALIZED;

	switch(nativeState)
	{
	case eOPL_SCHEDULER_EVENT_SCHEDULED:
		oipfState = RECORDING_SCHEDULED;
		break;

	case eOPL_SCHEDULER_EVENT_PRESTART:
		oipfState = RECORDING_REC_PRESTART;
		break;

	case eOPL_SCHEDULER_EVENT_ACQUIRING_RESOURCES:
		oipfState = RECORDING_REC_ACQUIRING_RESOURCES;
		break;

	case eOPL_SCHEDULER_EVENT_STARTED:
		oipfState = RECORDING_REC_STARTED;
		break;

	case eOPL_SCHEDULER_EVENT_UPDATED:
		oipfState = RECORDING_REC_UPDATED;
		break;

	case eOPL_SCHEDULER_EVENT_COMPLETED:
		oipfState = RECORDING_REC_COMPLETED;
		break;

	case eOPL_SCHEDULER_EVENT_PARTIALY_COMPLETED:
		oipfState = RECORDING_REC_PARTIALLY_COMPLETED;
		break;

	case eOPL_SCHEDULER_EVENT_DELETED:
		oipfState = RECORDING_DEL_SCHEDULED;
		break;

	case eOPL_SCHEDULER_EVENT_START_ERROR:
		oipfState = RECORDING_REC_START_ERROR;
		break;

	case eOPL_SCHEDULER_EVENT_START_CONFLICT:
		oipfState = RECORDING_REC_START_CONFLICT;
		break;

	case eOPL_SCHEDULER_EVENT_LOAD:
		oipfState = RECORDING_REC_LOAD;
		break;

	case eOPL_SCHEDULER_EVENT_ERROR:
		oipfState = RECORDING_ERROR;
		break;

	default:
		break;
	}

	return oipfState;
}

static DetailedRecordingErrorsCodes	__recording_ToDetailedRecordingError (int error)
{
	switch(error)
	{
	case eOPL_SCHEDULER_ERROR_RESOURCE_LIMITATION:
		return RECORDING_ERROR_REC_RESOURCE_LIMITATION;
	case eOPL_SCHEDULER_ERROR_INSUFFICIENT_STORAGE:
		return RECORDING_ERROR_INSUFFICIENT_STORAGE;
	case eOPL_SCHEDULER_ERROR_TUNER_CONFLICT:
		return RECORDING_ERROR_TUNER_CONFLICT;
	case eOPL_SCHEDULER_ERROR_DRM_RESTRICTION:
		return RECORDING_ERROR_REC_DRM_RESTRICTION;
	case eOPL_SCHEDULER_ERROR_HAVE_ALTERNATIVE:
		return RECORDING_ERROR_HAVE_ALTERNATIVE;
	case eOPL_SCHEDULER_ERROR_RFOTA:
		return RECORDING_ERROR_RFOTA;
	case eOPL_SCHEDULER_ERROR_NO_AV:
		return RECORDING_ERROR_NO_AV;
	case eOPL_SCHEDULER_ERROR_POWER_OFF:
		return RECORDING_ERROR_POWER_OFF;
	case eOPL_SCHEDULER_ERROR_UNKNOWN:
		return RECORDING_ERROR_REC_UNKNOWN;
	case eOPL_SCHEDULER_ERROR_CAS:
		return RECORDING_ERROR_REC_CAS;
	case eOPL_SCHEDULER_ERROR_HIGHER_PRIORITY:
		return RECORDING_ERROR_HIGHER_PRIORITY;
	case eOPL_SCHEDULER_ERROR_TRACKING_FAIL:
		return RECORDING_ERROR_TRACKING_FAIL;
	case eOPL_SCHEDULER_ERROR_OFF_AIR:
        return RECORDING_ERROR_OFF_AIR;
    case eOPL_SCHEDULER_ERROR_SATIP_FAILED:
        return RECORDING_ERROR_SATIP_FAILED;
    case eOPL_SCHEDULER_ERROR_MAX_RECORD_LIST:
        return RECORDING_ERROR_MAX_RECORD_LIST;
    case eOPL_SCHEDULER_ERROR_NONE:
    default:
        break;
    }
	return RECORDING_ERROR_REC_UNKNOWN;
}

#define __NATIVE_RECORDING_PUBLIC_START__

#ifdef OIPF
scheduleListener *global_scheduleListener = 0;
recordListener *global_recordListener = 0;

OOIFReturnCode native_SchedulerSetListener(scheduleListener listener)
{
	ENTRY;

	global_scheduleListener = listener;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordSetListener(recordListener listener)
{
	ENTRY;

	global_recordListener = listener;
	return OOIF_RETURN_OK;
}

static OPL_BOOL		scheduler_EventListener (
					  int state		///< @see enum OPL_SchedulerState
					, int error		///< @see enum OPL_SchedulerError
					, OPL_ScheduledItem_t *item
					, OPL_Collection_t    *conflict
					, OPL_Collection_t    *alternative
					, OPL_Collection_t    *altevent
				)
{
    ENTRY;

    if (global_scheduleListener == NULL || item == NULL)
    	return OPL_FALSE;

    RecordingConstants  oipfState = __recording_ToRecordingConstants(state);
    DetailedRecordingErrorsCodes    oipfError = __recording_ToDetailedRecordingError(error);
    ScheduledRecordingHandle    handle = NULL;
    ScheduledRecordingHandle    *list = NULL;
    int count = 0;
    ScheduledRecordingHandle    *alt_list = NULL;
    int alt_count = 0;
    ProgrammeHandle	    *alt_evt_list = NULL;
    int alt_evt_count = 0;

    if(conflict)
    {
    	list = (ScheduledRecordingHandle *)OPL_Collection_Dump(conflict, &count, NULL);
    }

    if (alternative)
    {
    	alt_list = (ScheduledRecordingHandle *)OPL_Collection_Dump(alternative, &alt_count, NULL);
    }

    if(altevent)
    {
    	alt_evt_list = (ProgrammeHandle *)OPL_Collection_Dump(altevent, &alt_evt_count, NULL);
    }

#ifdef JLABS
    if (OPL_ScheduledItem_InstanceOf(item, OPL_ScheduledItem_t))
    {
    	handle = (ScheduledRecordingHandle)OPL_Recording_ScheduledToRecording_Clone(item);
    }
    else
    {
    	handle = (ScheduledRecordingHandle)OPL_Recording_Clone((OPL_Recording_t *)item);
    }
#else

    handle = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(item);

#endif

    if ( handle )
    {
    	// onRecordingEvent(OOIFNumber state, ScheduledRecordingHandle handle, OOIFNumber error, ScheduledRecordingHandle *conflicts, int count)
    	// RecordingScheduler::onRecordingEvent()
    	global_scheduleListener(
    		  (OOIFNumber)oipfState
    		, handle
    		, (OOIFNumber)oipfError
    		, list
    		, count
    		, alt_list
    		, alt_count
    		, alt_evt_list
    		, alt_evt_count
    	);
    }

    if (list)
    	HLIB_STD_MemFree(list);

    if (alt_list)
    	HLIB_STD_MemFree(alt_list);

    if (alt_evt_list)
    	HLIB_STD_MemFree(alt_evt_list);

    return OPL_TRUE;
}
#endif /* OIPF */

#ifdef JLABS
hnReservationFinishedListener *global_hnReservationFinishedListener = 0;

OOIFReturnCode native_RecordingSetHNReservationFinishedListener(hnReservationFinishedListener listener)
{
	global_hnReservationFinishedListener = listener;
	return OOIF_RETURN_OK;
}

static OPL_BOOL		scheduler_EventhnReservationFinishedListener (
					 OPL_ScheduledItem_t *item
					, int error
				)
{
	ENTRY;

	if (global_hnReservationFinishedListener == NULL)
		return OPL_FALSE;

	ScheduledRecordingHandle		handle = NULL;

	if( item != NULL )
	{
		if (OPL_ScheduledItem_InstanceOf(item, OPL_ScheduledItem_t))
		{
			handle = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(item);
		}
		else
		{
			handle = (ScheduledRecordingHandle)OPL_Recording_Clone((OPL_Recording_t *)item);
		}
	}

	// onRecordingEvent(OOIFNumber state, ScheduledRecordingHandle handle, OOIFNumber error, ScheduledRecordingHandle *conflicts, int count)
	// RecordingScheduler::onRecordingEvent()
	global_hnReservationFinishedListener(handle, (OOIFNumber)error);

	return OPL_TRUE;
}

hnRemoveResultListener *global_hnRemoveResultListener = 0;

OOIFReturnCode native_RecordingSetHNRemoveResultListener(hnRemoveResultListener listener)
{
	global_hnRemoveResultListener = listener;
	return OOIF_RETURN_OK;
}

static void		scheduler_EventhnRemoveResultListener (
					 char *strRemove
					, int error
				)
{
	ENTRY;

	if (global_hnRemoveResultListener == NULL)
		return;

	// onRecordingEvent(OOIFNumber state, ScheduledRecordingHandle handle, OOIFNumber error, ScheduledRecordingHandle *conflicts, int count)
	// RecordingScheduler::onRecordingEvent()
	global_hnRemoveResultListener(strRemove, (OOIFNumber)error);

	return;
}
OOIFReturnCode native_SchedulerRemoveCDSRecording(jlabsCDSRecordingHandle identifier, char **retVal)
{
	ENTRY;

	OPL_CDSRecording_Remove((Opl_CDSRecording_t*)identifier, retVal);

	return OOIF_RETURN_OK;
}

#endif	// JLABS

OPL_BOOL	recordItem_EventListener(int state, int Err, OPL_Recording_t *item)
{

	ENTRY;
	//OOIF_LOG_PRINT("[%s:%d]state[%d],err[%d], filename[%s]\n",__FUNCTION__,__LINE__,state,Err,item->szFileName);

	if (global_recordListener == NULL || item == NULL)
		return OPL_FALSE;

	RecordingConstants				oipfState = __recording_ToRecordingConstants(state);
	DetailedRecordingErrorsCodes	oipfError = __recording_ToDetailedRecordingError(Err);

	global_recordListener(
		  (OOIFNumber)oipfState
		, item
		, (OOIFNumber)oipfError
		);
	return OPL_TRUE;
}

OOIFReturnCode native_RecordingSchedulerOnCreate(long int window, const char *host)
{
	ENTRY;
    OOIF_LOG_DEBUG("recording Scheduler On Create : %d (host: %s)", window, host);

	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();	// for initialization~!!

#ifdef OIPF
	OPL_Scheduler_AddEventListener(scheduler, scheduler_EventListener);
	OPL_RecordItem_AddEventListener(recordItem_EventListener);
#endif

#ifdef JLABS
	OPL_Scheduler_AddHNEventListener(scheduler, scheduler_EventhnReservationFinishedListener);
	OPL_REMOVE_RegisterListener(scheduler_EventhnRemoveResultListener);
#endif

#if defined(HMX_WEBUI)

#endif /* defined(HMX_WEBUI) */

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_SchedulerOnListenerAccessed(const char *eventName)
{
	ENTRY;
	// TODO: 이게 뭐냐??
	return OOIF_RETURN_OK;
}
#endif /*OIPF*/

#ifdef JLABS
OOIFReturnCode native_SchedulerRecord(ProgrammeHandle identifier, OOIFNumber factory, OOIFNumber recmode, const char *deviceStr, const char *destinationId, long int window_identifier, const char *host, ScheduledRecordingHandle *retval)
{
	ENTRY;
	OOIF_LOG_DEBUG("Creating scheduled recording (from Programme): %d (host: %s)", window_identifier, host);

	OPL_ScheduledItem_t	*sched = NULL;
	OPL_ReservationTryType	rsvTryType;
	int					RecType;
	int					nfac;

	rsvTryType = eOPL_SCHEDULER_TRY_TYPE_NORMAL;
	nfac = (int)factory;

	switch(nfac)
	{
		case 0:
			RecType = eOPL_SCHEDULED_PRO_RECORD;
			sched = OPL_ScheduledItem_jlabsNewWithProgramme(RecType, (OPL_ProgrammeHandle)identifier, (int)recmode, deviceStr, NULL, NULL);
			break;
		case 1:
			RecType = eOPL_SCHEDULED_PRO_WATCH;
			sched = OPL_ScheduledItem_jlabsNewWithProgramme(RecType, (OPL_ProgrammeHandle)identifier, 0, NULL, NULL, NULL);
			break;
		case 2:
			RecType = eOPL_SCHEDULED_PRO_DLNA;
			sched = OPL_ScheduledItem_jlabsNewWithProgramme(RecType, (OPL_ProgrammeHandle)identifier, 0, NULL, deviceStr, destinationId);
			break;
	}
	sched->isScheduledAsSeries = OPL_FALSE;

	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	sched = OPL_Scheduler_MakeSchedule(OPL_Scheduler_GetInstance(), sched, rsvTryType);
	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(sched);
	return OOIF_RETURN_OK;
}
#elif HMX_WEBUI
OOIFReturnCode native_SchedulerRecord(ProgrammeHandle identifier, OOIFNumber factory, OOIFNumber targetStorage, long int window_identifier, const char *host, ScheduledRecordingHandle *retval)
{
	ENTRY;
	OOIF_LOG_DEBUG("Creating scheduled recording (from Programme): %d (host: %s)", window_identifier, host);

	OPL_ScheduledItem_t *sched = NULL;
	OPL_ReservationTryType			rsvTryType = eOPL_SCHEDULER_TRY_TYPE_NORMAL;
	int				isScheduledAsSeries = 0;
	int				RecType = 0;

	switch ((int)factory)
	{
		case SCHEDULED_RECORDING_RECORDING:
			RecType = eOPL_SCHEDULED_PRO_RECORD;
			break;
		case SCHEDULED_RECORDING_VIEWING:
			RecType = eOPL_SCHEDULED_PRO_WATCH;
			break;
		case SCHEDULED_RECORDING_SERIES_RECORDING:
			RecType = eOPL_SCHEDULED_PRO_RECORD;
			isScheduledAsSeries = TRUE;
			break;
		case SCHEDULED_RECORDING_ALTERNATIVE_RECORDING:
			RecType = eOPL_SCHEDULED_PRO_RECORD;
			rsvTryType = eOPL_SCHEDULER_TRY_TYPE_FORCE;
			break;
		case SCHEDULED_RECORDING_ALTERNATIVE_SERIES_RECORDING:
			RecType = eOPL_SCHEDULED_PRO_RECORD;
			rsvTryType = eOPL_SCHEDULER_TRY_TYPE_FORCE;
			isScheduledAsSeries = TRUE;
			break;
		case SCHEDULED_RECORDING_SERIES_VIEWING:
			RecType = eOPL_SCHEDULED_PRO_WATCH;
			isScheduledAsSeries = TRUE;
			break;
		case SCHEDULED_RECORDING_ALTERNATIVE_VIEWING:
			RecType = eOPL_SCHEDULED_PRO_WATCH;
			rsvTryType = eOPL_SCHEDULER_TRY_TYPE_FORCE;
			break;
		case SCHEDULED_RECORDING_ALTERNATIVE_SERIES_VIEWING:
			RecType = eOPL_SCHEDULED_PRO_WATCH;
			rsvTryType = eOPL_SCHEDULER_TRY_TYPE_FORCE;
			isScheduledAsSeries = TRUE;
			break;
		default:
			RecType = eOPL_SCHEDULED_PRO_RECORD;
			break;
	}

	sched = OPL_ScheduledItem_NewWithProgramme(RecType, (OPL_ProgrammeHandle)identifier, NULL);
	if (sched == NULL)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	if(isScheduledAsSeries)
	{
		sched->isScheduledAsSeries = OPL_TRUE;
	}

	sched->targetStorage = targetStorage;
	sched = OPL_Scheduler_MakeSchedule(OPL_Scheduler_GetInstance(), sched, rsvTryType);
	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ScheduledRecordingHandle)sched; //OPL_ScheduledItem_Clone(sched);
	//need sched deinit
	return OOIF_RETURN_OK;
}
#else
OOIFReturnCode native_SchedulerRecord(ProgrammeHandle identifier, long int window_identifier, const char *host, ScheduledRecordingHandle *retval)
{
	ENTRY;
	OOIF_LOG_DEBUG("Creating scheduled recording (from Programme): %d (host: %s)", window_identifier, host);

	OPL_ScheduledItem_t *sched = OPL_Scheduler_Record(
										OPL_Scheduler_GetInstance(),
										(OPL_ProgrammeHandle)identifier
									);

	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ScheduledRecordingHandle)sched;
	return OOIF_RETURN_OK;
}
#endif

#ifdef HMX_WEBUI
OOIFReturnCode native_SchedulerRecordCrid(OOIFNumber factory, const char *crid, const char *title, const char *channelId, long int window_identifier, const char *host, ScheduledRecordingHandle *retval)
{
	ENTRY;
	OOIF_LOG_DEBUG("Creating scheduled recording (from crid): %d (host: %s)", window_identifier, host);

	OPL_ScheduledItem_t *sched = NULL;
	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();
	OPL_ReservationTryType			rsvTryType;
	int				RecType;
	int				isScheduledAsSeries = 0;

	switch((int)factory)
	{
		case SCHEDULED_RECORDING_RECORDING:
			RecType = eOPL_SCHEDULED_PRO_RECORD;
			isScheduledAsSeries = FALSE;
			break;

		case SCHEDULED_RECORDING_SERIES_RECORDING:
			RecType = eOPL_SCHEDULED_PRO_RECORD;
			isScheduledAsSeries = TRUE;
			break;

		default:
			return OOIF_RETURN_VALUE_NULL;
	}

	sched = OPL_ScheduledItem_NewWithCrid(RecType, isScheduledAsSeries, crid, title, channelId, NULL);
	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	rsvTryType = eOPL_SCHEDULER_TRY_TYPE_CRID;
	sched = OPL_Scheduler_MakeSchedule(scheduler, sched, rsvTryType);
	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(sched);
	return OOIF_RETURN_OK;

}
#endif

OOIFReturnCode native_ScheduledRecordingReleaseHandle(ScheduledRecordingHandle id)
{
	ENTRY;

	if (id)
	{
		OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;
		if (OPL_ScheduledItem_InstanceOf(item, OPL_ScheduledItem_t))
		{	// shceduled recording
			OPL_ScheduledItem_Delete((OPL_ScheduledItem_t *)id);
		}
		else if (OPL_ScheduledItem_InstanceOf(item, OPL_Recording_t))
		{	// recording
			OPL_Recording_Delete((OPL_Recording_t *)id);
		}
		else
		{	// what ??
			OOIF_LOG_DEBUG("What the FUXX not scheduled recording nor recording in scheduledRecordingRelease!!!\n");
		}
	}

	return OOIF_RETURN_OK;
}

#ifdef JLABS
OOIFReturnCode native_SchedulerRecordAt(OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, OOIFNumber factory, OOIFNumber recmode, const char *deviceStr, const char *destinationId, const char *host, ScheduledRecordingHandle *retval)

{
	ENTRY;

	OPL_ScheduledItem_t *sched = NULL;
	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();
	OPL_ReservationTryType	rsvTryType;
	int					RecType;
	int					nfac;

	rsvTryType = eOPL_SCHEDULER_TRY_TYPE_NORMAL;
	nfac = (int)factory;

	switch(nfac)
	{
		case 0:
			RecType = eOPL_SCHEDULED_PRO_RECORD;
			sched = OPL_ScheduledItem_jlabsNewWithTime(RecType, (unsigned long)startTime, (unsigned long)duration, (int)repeatDays, channelID, (int)recmode, deviceStr, NULL, NULL);
			break;
		case 1:
			RecType = eOPL_SCHEDULED_PRO_WATCH;
			sched = OPL_ScheduledItem_jlabsNewWithTime(RecType, (unsigned long)startTime, (unsigned long)duration, (int)repeatDays, channelID, (int)0, NULL, NULL, NULL);
			break;
		case 2:
			RecType = eOPL_SCHEDULED_PRO_DLNA;
			sched = OPL_ScheduledItem_jlabsNewWithTime(RecType, (unsigned long)startTime, (unsigned long)duration, (int)repeatDays, channelID, (int)recmode, NULL, deviceStr, destinationId);
			break;
	}

	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	sched = OPL_Scheduler_MakeSchedule(scheduler, sched, rsvTryType);
	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(sched);
	return OOIF_RETURN_OK;
}
#elif HMX_WEBUI
OOIFReturnCode native_SchedulerRecordAt(OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, OOIFNumber factory, OOIFNumber targetStorage, OOIFNumber volume, const char *host, ScheduledRecordingHandle *retval)

{
	ENTRY;

	OPL_ScheduledItem_t *sched = NULL;
	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();
	OPL_ReservationTryType			rsvTryType = eOPL_SCHEDULER_TRY_TYPE_NORMAL;
	int				RecType;
//	int				isForceRsv;
	int				Volume;

	Volume	= (int)volume;

	switch((int)factory)
	{
	case SCHEDULED_RECORDING_VIEWING: RecType = eOPL_SCHEDULED_PRO_WATCH;
			break;
	case SCHEDULED_RECORDING_POWER_ON_TIME: RecType = eOPL_SCHEDULED_PRO_POWER_ON_TIMER;
			break;
	case SCHEDULED_RECORDING_REGULAR_OTA: RecType = eOPL_SCHEDULED_PRO_REGULAR_OTA;
			break;
	case SCHEDULED_RECORDING_ASO_NOTIFY: RecType = eOPL_SCHEDULED_PRO_ASO_NOTIFY;
			break;
	default: RecType = eOPL_SCHEDULED_PRO_RECORD;
			break;
	}
	sched = OPL_ScheduledItem_NewWithTime(RecType, (unsigned long)startTime, (unsigned long)duration, (int)repeatDays, channelID, NULL,Volume);

	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	sched->targetStorage = targetStorage;
	sched = OPL_Scheduler_MakeSchedule(scheduler, sched, rsvTryType);
	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(sched);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerUpdate(RecordingHandle identifier, OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, OOIFNumber factory, OOIFNumber targetStorage, const char *host, RecordingHandle *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *sched = NULL;
	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();
	OPL_ReservationTryType			rsvTryType = eOPL_SCHEDULER_TRY_TYPE_NORMAL;;
	int				RecType;

	switch ((int)factory)
	{
		case SCHEDULED_RECORDING_VIEWING:
			RecType = eOPL_SCHEDULED_PRO_WATCH;
			break;
		case SCHEDULED_RECORDING_SAT_RECORDING:
			RecType = eOPL_SCHEDULED_PRO_SAT_RECORD;
			break;
		case SCHEDULED_RECORDING_RECORDING:
		default:
			RecType = eOPL_SCHEDULED_PRO_RECORD;
			break;
	}

	sched = OPL_ScheduledItem_NewWithTime(RecType, (unsigned long)startTime, (unsigned long)duration, (int)repeatDays, channelID, NULL,0);

	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	sched->targetStorage = targetStorage;
	sched = OPL_Scheduler_MakeSchedule(scheduler, sched, rsvTryType);
	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(sched);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerUpdateScheduled(ScheduledRecordingHandle identifier,
	OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, OOIFNumber factory, OOIFNumber targetStorage, const char *host, ScheduledRecordingHandle *retval)
{
	OOIFReturnCode			errCode = OOIF_RETURN_VALUE_UNDEFINED;
	OPL_ScheduledItem_t 	*item = NULL;
	OPL_ScheduledItem_t 	*newItem = NULL;
	unsigned long			newStartTime = (unsigned long)startTime;
	unsigned long			newDuration = (unsigned long)duration;
	int						newRepeatDays = (int)repeatDays;
	int						RecType = 0;

	OPL_BOOL				isSameStartTime = OPL_FALSE;
	OPL_BOOL				isSameDuration = OPL_FALSE;
	OPL_BOOL				isSameRepeatDay = OPL_FALSE;
	OPL_BOOL				isSameRecType = OPL_FALSE;
	OPL_BOOL				isSameChannelID = OPL_FALSE;

	ENTRY;

	*retval = NULL;
	if((int)factory & 0x0001)
	{
		RecType = eOPL_SCHEDULED_PRO_WATCH;
	}
	else
	{
		if ((int)factory & 0x0008)
			RecType = eOPL_SCHEDULED_PRO_SAT_RECORD;
		else
			RecType = eOPL_SCHEDULED_PRO_RECORD;
	}

	item = (OPL_ScheduledItem_t *)identifier;
	if(item == NULL)
	{
		errCode = OOIF_RETURN_VALUE_UNDEFINED;
	}
	else
	{
		if(item->startTime == newStartTime)
		{
			isSameStartTime = OPL_TRUE;
		}

		if(item->duration == newDuration)
		{
			isSameDuration = OPL_TRUE;
		}

		if(item->repeatDays == newRepeatDays)
		{
			isSameRepeatDay = OPL_TRUE;
		}

		if(item->type == RecType)
		{
			isSameRecType = OPL_TRUE;
		}

		if(strcmp(item->channelID,(const char *)channelID) == 0)
		{
			isSameChannelID = OPL_TRUE;
		}

		if( item->type != RecType &&
			(isSameStartTime && isSameDuration && isSameRepeatDay && isSameRecType && isSameChannelID))
		{
			// 다 같은경우...변경할 필요 없음...
			// 하지만 스케줄 타입을 변경한 경우는 허락함...
			// Series 에서 TBR 로 변경한경우..
		}
		else
		{
			newItem = OPL_ScheduledItem_Clone(item);
			if(newItem == NULL)
			{
				errCode = OOIF_RETURN_VALUE_UNDEFINED;
			}
			else
			{
				newItem->startTime = newStartTime;
				newItem->duration = newDuration;
				newItem->repeatDays = newRepeatDays;
				newItem->type = RecType;
				if(strcmp(item->channelID,(const char *)channelID))
				{
					if(newItem->channelID != NULL)
					{
						HLIB_STD_MemFree(newItem->channelID);
					}
					newItem->channelID = HLIB_STD_StrDup(channelID);
				}

				if(newItem->programmeID != NULL)
				{
					if(isSameStartTime && isSameDuration && isSameRepeatDay && !isSameRecType && isSameChannelID)
					{
						// Programme ID를 내려준다...remind, Record 타입만 변경한다면...
					}
					else
					{
						// REC TYPE만 다른 경우라면 PROGRAMME ID를 살릴 수 있다..
					HLIB_STD_MemFree(newItem->programmeID);
					newItem->programmeID = NULL;
				}
				}

				if (!OPL_Scheduler_UpdateProperty(OPL_Scheduler_GetInstance(), newItem))
				{
					OPL_ScheduledItem_Delete(newItem);
					errCode = OOIF_RETURN_VALUE_UNDEFINED;
				}
				else
				{
					*retval = newItem;
					errCode = OOIF_RETURN_OK;
				}
			}
		}
	}

	return errCode;
}

RecordingFileOperateListener	g_pRecFileOperationEventListener   = NULL;
static void native_SchedulerRecFileOpListener(int ticketKey, eOplFileManagerState state)
{
	ENTRY;
	RecordingSchedulerFileOpStateType fileOpState = RECORDING_SCHEDULER_FILEOP_STATE_UNKNOWN;
	eOplFileManagerBaseState base = eOPL_FILEMGR_BASE_UNKNOWN;

	if(NULL == g_pRecFileOperationEventListener)
	{
		OOIF_LOG_ERROR("[%s:%d]\n", __FUNCTION__, __LINE__);
		return;
	}

	OOIF_LOG_DEBUG("key 0x%x, state %d\n", ticketKey, state);
	switch(state&0x00FF0000)
	{
		case eOPL_CMD_COPY:
			base = eOPL_FILEMGR_BASE_COPY;
			break;
		case eOPL_CMD_REMOVE:
			base = eOPL_FILEMGR_BASE_REMOVE;
			break;
		case eOPL_CMD_MOVE:
			base = eOPL_FILEMGR_BASE_MOVE;
			break;
		case eOPL_CMD_STOP:
			base = eOPL_FILEMGR_BASE_STOP;
			break;
		default:
			break;
	}

	switch(base)
	{
		case eOPL_FILEMGR_BASE_UNKNOWN:
			break;
		case eOPL_FILEMGR_BASE_STOP:
			fileOpState = RECORDING_SCHEDULER_FILEOP_STATE_STOP;
			break;
		default:
			switch(state-base)
			{
				case eOPL_ING_INDEX:
					fileOpState = RECORDING_SCHEDULER_FILEOP_STATE_ING;
					break;
				case eOPL_COMPLETE_INDEX:
					fileOpState = RECORDING_SCHEDULER_FILEOP_STATE_COMPLETE;
					break;
				case eOPL_ERROR_INDEX:
					fileOpState = RECORDING_SCHEDULER_FILEOP_STATE_ERROR;
					break;
				default:
					break;
			}
			break;
	}

	if(fileOpState == RECORDING_SCHEDULER_FILEOP_STATE_UNKNOWN)
	{
		switch(state)
		{
		case eOPL_FILEMGR_STATE_LACK_SPACE:
			fileOpState = RECORDING_SCHEDULER_FILEOP_NOT_ENOUGH_SPACE;
			break;
		case eOPL_FILEMGR_STATE_4G_FAT32:
			fileOpState = RECORDING_SCHEDULER_FILEOP_STATE_SIZE_LIMIT_4G;
			break;
		default:
			break;
		}
	}

	g_pRecFileOperationEventListener(ticketKey, (int)fileOpState);
}

OOIFReturnCode 	native_SchedulerRegRecFileOpEventListener(RecordingFileOperateListener listener)
{
	ENTRY;
	g_pRecFileOperationEventListener = listener;

	OplRecFileManager::SetListener((OplRecFileManagerCallback)native_SchedulerRecFileOpListener);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_SchedulerUnRegRecFileOpEventListener(void)
{
	ENTRY;
	g_pRecFileOperationEventListener = NULL;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerCopyRecording(RecordingHandle identifier, OOIFNumber targetStorage, OOIFNumber *ticketNumber)
{
	int ticketKey = -1;
	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	HBOOL bSuccess = FALSE;
	HUINT32 ulDevId = (HUINT32)targetStorage;

	bSuccess = OplRecFileManager::CopyRecording((const HCHAR *)item->id, ulDevId, &ticketKey);
	if (bSuccess == FALSE)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	(*ticketNumber) = ticketKey;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_SchedulerMoveRecording(RecordingHandle identifier, OOIFNumber targetStorage, OOIFNumber *ticketNumber)
{
	int ticketKey = -1;
	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	HBOOL bSuccess = FALSE;
	HUINT32 ulDevId = (HUINT32)targetStorage;

	bSuccess = OplRecFileManager::MoveRecording((const HCHAR *)item->id, ulDevId, &ticketKey);
	if (bSuccess == FALSE)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	(*ticketNumber) = ticketKey;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_SchedulerRemoveRecording(RecordingHandle identifier, OOIFNumber *ticketNumber)
{
	OOIFReturnCode retCode = OOIF_RETURN_OK;
	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	int ticketKey = -1;
	HBOOL bRet = FALSE;

	bRet = OplRecFileManager::RemoveRecording(
		(const char *)item->id, (const char *)item->szPath,(const char *)item->szFileName, &ticketKey);
	if(bRet)
	{
		retCode = OOIF_RETURN_VALUE_UNDEFINED;
	}
	else
	{
		(*ticketNumber) = ticketKey;
	}
	return retCode;
}

OOIFReturnCode native_SchedulerStopFileOp(OOIFNumber ticketNumber, OOIFNumber *pRetTicketNumber)
{
	OOIFReturnCode retCode = OOIF_RETURN_OK;
	HBOOL bRet = FALSE;

	bRet = OplRecFileManager::stopRecordingFileOp((int)ticketNumber, (int *)pRetTicketNumber);
	if(bRet)
	{
		retCode = OOIF_RETURN_VALUE_UNDEFINED;
	}

	return retCode;
}

OOIFReturnCode native_SchedulerSetManualLock(const char *seriesID, OOIFBoolean lockState)
{
	OOIFReturnCode error = OOIF_RETURN_OK;
	OPL_BOOL bLock = OPL_FALSE;

	if(seriesID == NULL)
	{
		error = OOIF_RETURN_ERROR;
	}
	else
	{
		if(lockState == TRUE)
		{
			bLock = OPL_TRUE;
		}
		else
		{
			bLock = OPL_FALSE;
		}
		OPL_Scheduler_SetSeriesManualLock(OPL_Scheduler_GetInstance(), seriesID, bLock);
	}

	return error;
}

#else

OOIFReturnCode native_SchedulerRecordAt(OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, OOIFNumber volume,const char *host, ScheduledRecordingHandle *retval)
{
	ENTRY;
	OPL_ScheduledItem_t *sched;

	sched = OPL_Scheduler_RecordAt(OPL_Scheduler_GetInstance(),
					(unsigned long)startTime,
					(unsigned long)duration,
					(int)repeatDays,
					channelID,
					(int)volume,
				);

	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(sched);
	return OOIF_RETURN_OK;
}
#endif

#ifdef JLABS
OOIFReturnCode native_SchedulerCopy(OOIFNumber startTime, RecordingHandle identifier, const char *deviceId, const char *destinationId, ScheduledRecordingHandle *retval)

{
	ENTRY;

	OPL_ScheduledItem_t	*sched = NULL;
	OPL_HANDLE 			scheduler = OPL_Scheduler_GetInstance();
	int					RecType;

	RecType = eOPL_SCHEDULED_PRO_DUBBING;
	sched = OPL_ScheduledItem_jlabsNewWithRecording(RecType, (unsigned long)startTime, (OPL_RecordingHandle)identifier, deviceId, destinationId);
	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	sched = OPL_Scheduler_MakeSchedule(scheduler, sched, eOPL_SCHEDULER_TRY_TYPE_NORMAL);
	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(sched);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_SchedulerSetDefaultDMS(const char *deviceHandle, const char *destinationId, OOIFNumber *retval)
{
	OPL_BOOL	hErr;

	if (deviceHandle == NULL || destinationId == NULL )
		return OOIF_RETURN_ERROR;

	hErr = OPL_Scheduler_SetDefaultDMS(deviceHandle, destinationId);
	if( hErr == FALSE)
		return OOIF_RETURN_VALUE_NULL;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerGetDefaultDMSDeviceHandle(unsigned char **retval)
{
	OPL_BOOL	hErr;
	unsigned char *tmp;

	if (retval == NULL)
		return OOIF_RETURN_ERROR;

	hErr = OPL_Scheduler_GetDefaultDMSDeviceHandle(&tmp);
	if( hErr == FALSE)
		return OOIF_RETURN_VALUE_NULL;

	*retval = tmp;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerGetDefaultDMSRecordDestinationId(unsigned char **retval)
{
	OPL_BOOL	hErr;
	unsigned char *tmp;

	if (retval == NULL)
		return OOIF_RETURN_ERROR;

	hErr = OPL_Scheduler_GetDefaultDMSRecordDestinationId(&tmp);
	if( hErr == FALSE)
		return OOIF_RETURN_VALUE_NULL;

	*retval = tmp;

	return OOIF_RETURN_OK;
}
#endif

OOIFReturnCode native_SchedulerGetScheduledRecordings(const char *host, int *count, ScheduledRecordingHandle **handles)
{
	ENTRY;
	unsigned int ulCurrentTime;
	int	cnt;

	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();
	ulCurrentTime = OPL_Scheduler_GetSystemTime();
	OOIF_LOG_DEBUG("[%d] ulCurrentTime %d\n",__LINE__, ulCurrentTime);

	int	c = OPL_Scheduler_CountScheduledProgrammes(scheduler);
	if (c == 0)
	{
		*count  = c;
		*handles = NULL;
		return OOIF_RETURN_VALUE_NULL;
	}

	ScheduledRecordingHandle *array = (ScheduledRecordingHandle *) HLIB_STD_MemAlloc (sizeof(ScheduledRecordingHandle) * c);
	if (array == NULL)
	{
		return OOIF_RETURN_OOM_ERROR;
	}

	OPL_ScheduledItem_t *item;

	cnt = 0;
	for (int i = 0; i < c ; i++)
	{
		item = OPL_Scheduler_GetScheduledProgrammeAt(scheduler, i);
		if( (ulCurrentTime+EARLYSTARTTIME) < item->startTime)
		{
			OOIF_LOG_DEBUG("[%d] type %d\n",__LINE__, item->type);
			OOIF_LOG_DEBUG("[%d] name %s\n",__LINE__, item->name);
			OOIF_LOG_DEBUG("[%d] startTime %d\n",__LINE__, item->startTime);
			OOIF_LOG_DEBUG("[%d] duration %d\n",__LINE__, item->duration);
			OOIF_LOG_DEBUG("[%d] recMode %d\n",__LINE__, item->recMode);
			OOIF_LOG_DEBUG("[%d] storageID %s\n",__LINE__, item->storageID);
			OOIF_LOG_DEBUG("[%d] deviceHandle %s\n",__LINE__, item->deviceHandle);
			OOIF_LOG_DEBUG("[%d] destinationId %s\n",__LINE__, item->destinationId);

			if (item->type == eOPL_SCHEDULED_PRO_DUBBING)
			{
				array[cnt] = (RecordingHandle)OPL_Recording_ScheduledToRecording_Clone((OPL_ScheduledItem_t*)item);
			}
			else
			{
				array[cnt] = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(item);
			}
			cnt++;
		}
#if defined(CONFIG_DEBUG)
		else
		{
			OOIF_LOG_DEBUG("[%d] type %d\n",__LINE__, item->type);
			OOIF_LOG_DEBUG("[%d] name %s\n",__LINE__, item->name);
			OOIF_LOG_DEBUG("[%d] startTime %d\n",__LINE__, item->startTime);
			OOIF_LOG_DEBUG("[%d] duration %d\n",__LINE__, item->duration);
			OOIF_LOG_DEBUG("[%d] recMode %d\n",__LINE__, item->recMode);
			OOIF_LOG_DEBUG("[%d] storageID %s\n",__LINE__, item->storageID);
			OOIF_LOG_DEBUG("[%d] deviceHandle %s\n",__LINE__, item->deviceHandle);
			OOIF_LOG_DEBUG("[%d] destinationId %s\n",__LINE__, item->destinationId);
		}
		OOIF_LOG_DEBUG("[%d] CONFIG_DEBUG>?????\n",__LINE__);
#endif
	}

	if( cnt == 0 )
	{
		if(array)
			HLIB_STD_MemFree(array);
		*count  = 0;
		*handles = NULL;
		return OOIF_RETURN_VALUE_NULL;
	}

	*handles = array;
	*count   = cnt;

	return OOIF_RETURN_OK;
}

#if defined(HMX_WEBUI)
OOIFReturnCode native_SchedulerGetCountScheduleList(int *count)
{
	ENTRY;
	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();
	int	scheduleCount = 0;

	scheduleCount = OPL_Scheduler_CountScheduledProgrammes(scheduler);
	if (scheduleCount == 0)
	{
		*count  = 0;
		return OOIF_RETURN_VALUE_NULL;
	}

	*count   = scheduleCount;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerGetCountRecordingList(int *count)
{
	ENTRY;
	int	recordingCount = 0;

	recordingCount = OPL_RecordItem_CountRecordings();
	if (recordingCount == 0)
	{
		*count  = 0;
		return OOIF_RETURN_VALUE_NULL;
	}

	*count = recordingCount;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerGetScheduleList(int *count, ScheduledRecordingHandle **handles)
{
	ENTRY;
	OOIFReturnCode ret = OOIF_RETURN_OK;
	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();
	ScheduledRecordingHandle *array = NULL;
	OPL_ScheduledItem_t *sItem = NULL;
	int scheduleCount = 0;

	scheduleCount = OPL_Scheduler_CountScheduledProgrammes(scheduler);
	if (scheduleCount == 0)
	{
		*count  = 0;
		handles = NULL;
		ret = OOIF_RETURN_VALUE_NULL;
		goto END_FUNC;
	}

	array = (ScheduledRecordingHandle *) HLIB_STD_MemAlloc (sizeof(ScheduledRecordingHandle) * scheduleCount);
	if (array == NULL)
	{
		*count  = 0;
		handles = NULL;
		ret = OOIF_RETURN_OOM_ERROR;
		goto END_FUNC;
	}

	
	for (int i = 0 ; i < scheduleCount; i++)
	{
		sItem = OPL_Scheduler_GetScheduledProgrammeAt(scheduler, i);
		if (sItem != NULL)
		{
			array[i] = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(sItem);
		}
	}

	*handles = array;
	*count = scheduleCount;

	ret = OOIF_RETURN_OK;

END_FUNC:

    return ret;
}

OOIFReturnCode native_SchedulerGetRecordingList(int *count, ScheduledRecordingHandle **handles)
{
	ENTRY;
	OOIFReturnCode ret = OOIF_RETURN_OK;
	OPL_Collection_t  	*collection = NULL;
	ScheduledRecordingHandle *array = NULL;
	OPL_Recording_t 	*rItem = NULL;
	int recordingCount = 0;

	collection = OPL_RecordItem_GetRecordings();
	if (collection != NULL)
	{
		recordingCount = OPL_Collection_Length(collection);
	}
	if (recordingCount == 0)
	{
		*count	= 0;
		handles = NULL;
		ret = OOIF_RETURN_VALUE_NULL;
		goto END_FUNC;
	}
	
	array = (ScheduledRecordingHandle *) HLIB_STD_MemAlloc (sizeof(ScheduledRecordingHandle) * recordingCount);
	if (array == NULL)
	{
		*count  = 0;
		handles = NULL;
		ret = OOIF_RETURN_OOM_ERROR;
		goto END_FUNC;
	}

	for (int i = 0 ; i < recordingCount; i++)
	{
		rItem = (OPL_Recording_t*)OPL_Collection_ItemAt(collection,i);
		array[i] = (ScheduledRecordingHandle)OPL_Recording_Clone(rItem);
	}

	*handles = array;
	*count = recordingCount;

	ret = OOIF_RETURN_OK;

END_FUNC:

	if (collection != NULL)
	{
		OPL_Collection_Delete(collection);
	}
    return ret;
}

#endif

OOIFReturnCode native_SchedulerRemoveScheduled(ScheduledRecordingHandle identifier)
{
	ENTRY;

	OPL_Scheduler_Remove(OPL_Scheduler_GetInstance(), (OPL_ScheduledItem_t *)identifier);

	return OOIF_RETURN_OK;
}

#if defined(HMX_WEBUI)
OOIFReturnCode native_SchedulerRemoveFirstScheduled(ScheduledRecordingHandle identifier)
{
	ENTRY;

	OPL_Scheduler_RemoveFirstItem(OPL_Scheduler_GetInstance(), (OPL_ScheduledItem_t *)identifier);

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_SchedulerRemoveWholeSeries(ScheduledRecordingHandle identifier)
{
	OPL_Scheduler_RemoveWholeSeries(OPL_Scheduler_GetInstance(), (OPL_ScheduledItem_t *)identifier);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerGetGroupCRIDs(ScheduledRecordingHandle identifier, int *count,
											 char ***crids)
{
 	OPL_Collection_t *coll = NULL;

	coll = OPL_Scheduler_GetGroupCRIDs((OPL_ScheduledItem_t*)identifier);
	if (coll)
	{
		*crids = OPL_Collection_DumpString(coll, count);
		OPL_Collection_Delete(coll);
	}
	else
	{
		*crids = NULL;
	}
	
	if (*crids == NULL)
	{
		*count  = 0;
	}

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_ScheduledRecordingGetProgrammeCRID(ScheduledRecordingHandle id,const char **retval)
{
	*retval = OPL_Scheduler_GetProgrammeCRID((OPL_ScheduledItem_t*)id);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}



#endif
OOIFReturnCode native_getCurrentRecordings(int *count, RecordingHandle **handles)
{
	ENTRY;
	OPL_Collection_t *itemlist = NULL;
	int c = 0;

	OPL_HANDLE 			scheduler 	= OPL_Scheduler_GetInstance();

	itemlist = OPL_Scheduler_GetCurrentRecordings(scheduler);

    if (NULL == itemlist)
	{
		*count  = c;
		*handles = NULL;
		return OOIF_RETURN_VALUE_NULL;
	}

	c = OPL_Collection_Length(itemlist);

	RecordingHandle *array = (RecordingHandle *) HLIB_STD_MemAlloc (sizeof(RecordingHandle) * c);
	if (array == NULL)
	{
		OPL_Collection_Delete(itemlist);
		return OOIF_RETURN_OOM_ERROR;
	}

	OPL_Recording_t *item;

	for (int i = 0 ; i < c; i++)
	{
		item = (OPL_Recording_t*)OPL_Collection_ItemAt(itemlist, i);

		if (OPL_ScheduledItem_InstanceOf(item, OPL_ScheduledItem_t))
		{
			array[i] = (RecordingHandle)OPL_Recording_ScheduledToRecording_Clone((OPL_ScheduledItem_t*)item);
		}
		else
		{
			array[i] = (RecordingHandle)OPL_Recording_Clone(item);
		}
	}
	*handles = array;
	*count	 = c;

	OPL_Collection_Delete(itemlist);

	return OOIF_RETURN_OK;
}

#ifdef JLABS
OOIFReturnCode native_Jlabs_SchedulerGetRecordings(const char *discId, int *count, RecordingHandle **handles)
{
	ENTRY;

	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();
	int total_cnt = OPL_Scheduler_CountRecordings(scheduler);
	int	c = OPL_Scheduler_CountRecordingsByDiscId(scheduler, discId);
	if (c == 0)
	{
		*count  = c;
		handles = NULL;
		return OOIF_RETURN_VALUE_NULL;
	}

	RecordingHandle *array = (RecordingHandle *) HLIB_STD_MemAlloc (sizeof(RecordingHandle) * c);
	if (array == NULL)
	{
		return OOIF_RETURN_OOM_ERROR;
	}

	OPL_Recording_t *item;
	int i, j;

	for (i = 0, j = 0; i < total_cnt; i++)
	{
		item = OPL_Scheduler_GetRecordingAtDiscId(scheduler, i, discId);
		if(item != NULL)
		{
			array[j] = (RecordingHandle)OPL_Recording_Clone(item);
			j++;

			if(j > c)
			{
				OOIF_LOG_DEBUG("Item count error");
				return OOIF_RETURN_ERROR;
			}
		}
	}

	*handles = array;
	*count	 = c;

	return OOIF_RETURN_OK;
}
#endif

OOIFReturnCode native_SchedulerGetRecordings(int *count, ScheduledRecordingHandle **handles)
{	// scheduled and recorded
	ENTRY;
	OOIFReturnCode ret = OOIF_RETURN_OK;
	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();
	OPL_Recording_t 	*rItem = NULL;
	OPL_Collection_t  	*collection = NULL;
	ScheduledRecordingHandle *array = NULL;
	int sc = OPL_Scheduler_CountScheduledProgrammes(scheduler);
	int real_rc = 0, real_sc = 0;
	int tc;

	collection = OPL_RecordItem_GetRecordings();
	if (collection != NULL)
	{
		real_rc = OPL_Collection_Length(collection);
	}
	tc = real_rc + sc;
	if (tc == 0)
	{
		*count  = 0;
		handles = NULL;
		ret = OOIF_RETURN_VALUE_NULL;
		goto END_FUNC;
	}

	array = (ScheduledRecordingHandle *) HLIB_STD_MemAlloc (sizeof(ScheduledRecordingHandle) * tc);
	if (array == NULL)
	{
		*count  = 0;
		handles = NULL;
		ret = OOIF_RETURN_OOM_ERROR;
		goto END_FUNC;
	}

	for (int i = 0 ; i < real_rc; i++)
	{
		rItem = (OPL_Recording_t*)OPL_Collection_ItemAt(collection,i);
		array[i] = (ScheduledRecordingHandle)OPL_Recording_Clone(rItem);
	}

	OPL_ScheduledItem_t *sItem;
	for (int i = 0 ; i < sc; i++)
	{
		sItem = OPL_Scheduler_GetScheduledProgrammeAt(scheduler, i);
		if (sItem != NULL)
		{
			array[real_rc + real_sc] = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(sItem);
			real_sc++;
		}
	}

	*handles = array;
	*count	 = real_rc + real_sc;

	ret = OOIF_RETURN_OK;

END_FUNC:

	if (collection != NULL)
	{
		OPL_Collection_Delete(collection);
	}
    return ret;
}

OOIFReturnCode native_RecordingReleaseHandle(RecordingHandle id)
{
	ENTRY;

	if (id)
		OPL_Recording_Delete((OPL_Recording_t *)id);
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_SchedulerRemove(RecordingHandle identifier)
{
	return native_SchedulerRemoveScheduled((ScheduledRecordingHandle)identifier);
	//global_recordingListener(identifier, 0);
	//OOIF_LOG_DEBUG("Removing recording: %p", identifier);
	//return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerStop(ScheduledRecordingHandle identifier)
{
	ENTRY;

	OPL_Scheduler_Stop(OPL_Scheduler_GetInstance(), (OPL_Recording_t *)identifier);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerGetRecording(const char *id, RecordingHandle *retval)
{
	ENTRY;

	OPL_Recording_t *recording = OPL_RecordItem_GetRecordingByID(id);

	if (recording == NULL)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*retval = (RecordingHandle)recording;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SchedulerRefresh()
{
	ENTRY;

	OPL_Scheduler_Refresh(OPL_Scheduler_GetInstance());
	return OOIF_RETURN_OK;
}
#endif // OIPF

/*------------------------------------------
  ScheduledRecording
  ------------------------------------------*/
#ifdef JLABS
OOIFReturnCode native_SchedulerSetName(ScheduledRecordingHandle id, const char *name, OOIFBoolean *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	*retval = OPL_Scheduler_SetName(item, (unsigned char *)name);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetStorageId(ScheduledRecordingHandle id, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item->storageID)
	{
		*retval = item->storageID;
	}
	else
	{
		*retval = "";
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetFactoryType(ScheduledRecordingHandle id, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	switch (item->type)
	{
	case eOPL_SCHEDULED_PRO_RECORD:
		*retval = SCHEDULED_RECORDING_RECORDING;
		break;

	case eOPL_SCHEDULED_PRO_WATCH:
		*retval = SCHEDULED_RECORDING_VIEWING;
		break;

	case eOPL_SCHEDULED_PRO_DLNA:
		*retval = SCHEDULED_RECORDING_DLNA_UPLOAD;
		break;

	case eOPL_SCHEDULED_PRO_DUBBING:
		*retval = SCHEDULED_RECORDING_DUBBING;
		break;

	default:
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetRecMode(ScheduledRecordingHandle id, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->recMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetFromDap(ScheduledRecordingHandle id, OOIFBoolean *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->fromDap;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetGroupIndex(ScheduledRecordingHandle id, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->groupIndex;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetUserID(ScheduledRecordingHandle id, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item->userID)
	{
		*retval = item->userID;
	}
	else
	{
		*retval = "";
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetDeviceHandle(ScheduledRecordingHandle id, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item->deviceHandle)
	{
		*retval = item->deviceHandle;
	}
	else
	{
		*retval = "";
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetDestinationId(ScheduledRecordingHandle id, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item->destinationId)
	{
		*retval = item->destinationId;
	}
	else
	{
		*retval = "";
	}
	return OOIF_RETURN_OK;
}

#elif HMX_WEBUI
OOIFReturnCode native_ScheduledRecordingGetFactoryType(ScheduledRecordingHandle id, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;
	switch (item->type)
	{
	case eOPL_SCHEDULED_PRO_RECORD:
		*retval = SCHEDULED_RECORDING_RECORDING;
		break;
	case eOPL_SCHEDULED_PRO_SAT_RECORD:
		*retval = SCHEDULED_RECORDING_SAT_RECORDING;
		break;
	case eOPL_SCHEDULED_PRO_WATCH:
		*retval = SCHEDULED_RECORDING_VIEWING;
		break;
	case eOPL_SCHEDULED_PRO_POWER_ON_TIMER:
		*retval = SCHEDULED_RECORDING_POWER_ON_TIME;
		break;
	case eOPL_SCHEDULED_PRO_NETSTREAMING_LIVE:
		*retval = SCHEDULED_RECORDING_NETSTREAMING_LIVE;
		break;
	case eOPL_SCHEDULED_PRO_NETSTREAMING_FILE:
		*retval = SCHEDULED_RECORDING_NETSTREAMING_FILE;
		break;
	case eOPL_SCHEDULED_PRO_REGULAR_OTA:
		*retval = SCHEDULED_RECORDING_REGULAR_OTA;
		break;
	case eOPL_SCHEDULED_PRO_ASO_NOTIFY:
		*retval = SCHEDULED_RECORDING_ASO_NOTIFY;
		break;
	default:
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}
#endif

OOIFReturnCode native_ScheduledRecordingGetUID(ScheduledRecordingHandle id,OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->uid;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetStartPadding(ScheduledRecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->startPadding;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingSetStartPadding(ScheduledRecordingHandle identifier, OOIFNumber startPadding)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;
	unsigned long	value, backup;

	value = (unsigned long)startPadding;

	if (item->startPadding != value)
	{
		backup = item->startPadding;
		item->startPadding = value;

		if (!OPL_Scheduler_Update(OPL_Scheduler_GetInstance(), item))
		{
			item->startPadding = backup;
			return OOIF_RETURN_ERROR;
		}
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetEndPadding(ScheduledRecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->endPadding;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingSetEndPadding(ScheduledRecordingHandle identifier, OOIFNumber endPadding)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;
	unsigned long	value, backup;

	value = (unsigned long)endPadding;

	if (item->endPadding != value)
	{
		backup = item->endPadding;
		item->endPadding = value;

		if (!OPL_Scheduler_Update(OPL_Scheduler_GetInstance(), item))
		{
			item->endPadding = backup;
			return OOIF_RETURN_ERROR;
		}
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetRepeatDays(ScheduledRecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->repeatDays;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetName(ScheduledRecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->name)
	{
		*retval = item->name;
	}
	else if (OPL_ScheduledItem_InstanceOf(item, OPL_ScheduledItem_t)
			&& NULL == item->programmeID)
	{
		*retval = "";
	}
	else
	{
		*retval = "";
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetLongName(ScheduledRecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->longName == NULL)
	{
		return OOIF_RETURN_VALUE_NULL;
	}
	*retval = item->longName;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetDescription(ScheduledRecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->description == NULL)
	{
		return OOIF_RETURN_VALUE_NULL;
	}
	*retval = item->description;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetLongDescription(ScheduledRecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->longDescription == NULL)
	{
		return OOIF_RETURN_VALUE_NULL;
	}
	*retval = item->longDescription;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetStartTime(ScheduledRecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->startTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetDuration(ScheduledRecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->duration;
	return OOIF_RETURN_OK;
}

#ifdef HMX_WEBUI
OOIFReturnCode native_ScheduledRecordingSetDuration(ScheduledRecordingHandle identifier, OOIFNumber duration)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;
	unsigned long	value, backup;

	value = (unsigned long)duration;

	if (item->duration!= value)
	{
		backup = item->duration;
		item->duration= value;

		if (!OPL_Scheduler_Update(OPL_Scheduler_GetInstance(), item))
		{
			item->duration= backup;
		}
	}

	return OOIF_RETURN_OK;
}
#endif

OOIFReturnCode native_ScheduledRecordingGetChannel(ScheduledRecordingHandle identifier, ChannelHandle *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	if (item->channelID == NULL)
		return OOIF_RETURN_VALUE_NULL;

	OPL_ChannelManager_t chMgr = OPL_Channel_GetManager();

	OPL_Channel_t channel = OPL_Channel_FindChannel(chMgr, item->channelID);

	if (channel == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ChannelHandle)OPL_Channel_Clone(channel);
	return OOIF_RETURN_OK;
}

#if defined HMX_WEBUI
OOIFReturnCode native_ScheduledRecordingGetIsScheduledAsSeries(ScheduledRecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (item->isScheduledAsSeries) ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetSeriesID(ScheduledRecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->seriesID == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = item->seriesID;
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_ScheduledRecordingGetVolume(ScheduledRecordingHandle identifier,  OOIFNumber *volume)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->volume == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*volume = item->volume;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetStatus(ScheduledRecordingHandle identifier,  OOIFNumber *status)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->status == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*status = item->status;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingIsEventBased(ScheduledRecordingHandle id, OOIFBoolean *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->eventbased;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetDiseqcInput(ScheduledRecordingHandle identifier,  OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->diseqcInput;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetFrequency(ScheduledRecordingHandle identifier,  OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->frequency;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetPolarisation(ScheduledRecordingHandle identifier,  OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->polarisation;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetSymbolRate(ScheduledRecordingHandle identifier,  OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->symbolRate;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetSatType(ScheduledRecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->satType == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = item->satType;
	return OOIF_RETURN_OK;
}
#endif /* HMX_WEBUI */

OOIFReturnCode native_ScheduledRecordingGetProgrammeID(ScheduledRecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->programmeID == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = item->programmeID;
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_ScheduledRecordingGetProgrammeIDType(ScheduledRecordingHandle identifier, ScheduledRecordingProgrammeIDType *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->programmeID == NULL)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = (ScheduledRecordingProgrammeIDType)OPL_Programme_GetIDTypeFromProgrammeID(item->programmeID);

	return OOIF_RETURN_OK;
}

#if defined OIPF || defined OPT_HBBTV_SCHED_REC_SERIES
OOIFReturnCode native_ScheduledRecordingGetEpisode(ScheduledRecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->episode;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingGetTotalEpisodes(ScheduledRecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->totalEpisodes;

	return OOIF_RETURN_OK;
}
#endif /* OIPF || OPT_HBBTV_SCHED_REC_SERIES */

OOIFReturnCode native_ScheduledRecordingGetParentalRating(ScheduledRecordingHandle identifier, int *count, ParentalRatingHandle **handles)
{
	int	value;

	OPL_ScheduledItem_t	*scheditem;
	OPL_ParentalRating_t	*parentalRating;
	ParentalRatingHandle	*myHandles = (ParentalRatingHandle*)HLIB_STD_MemAlloc(sizeof(ParentalRatingHandle));

	scheditem = (OPL_ScheduledItem_t*)identifier;
	value = OPL_ScheduledItem_GetParentalRating(scheditem);
#ifdef JLABS
	if(value == 0)
		value = scheditem->Rating;
#endif
	parentalRating = OPL_PCMAN_PRC_AddParentalRating(NULL, NULL, NULL, value, 0, NULL);
	myHandles[0] = (ParentalRatingHandle)parentalRating;

	*handles = myHandles;
	*count = 1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ScheduledRecordingIsRecorded(ScheduledRecordingHandle id, OOIFBoolean *retval)
{
   	ENTRY;

	OPL_ScheduledItem_t	*item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	if (OPL_ScheduledItem_InstanceOf(item, OPL_ScheduledItem_t))
		*retval = FALSE;
	else
		*retval = TRUE;
    return OOIF_RETURN_OK;
}

/*------------------------------------------
  Recording
  ------------------------------------------*/

OOIFReturnCode native_RecordingGetState(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)__recording_ToRecordingConstants(item->state);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetID(RecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	if (item->id == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = item->id;
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_RecordingGetError(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	int oipfState = (OOIFNumber)__recording_ToRecordingConstants(item->state);

	if (RECORDING_ERROR != oipfState && RECORDING_REC_PARTIALLY_COMPLETED != oipfState)
	{	// only valid (RECORDING_ERROR or RECORDING_REC_PARTIALLY_COMPLETED)
		return OOIF_RETURN_VALUE_NULL;
	}

	*retval = (OOIFNumber)__recording_ToDetailedRecordingError(item->error);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetIsManual(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isManual ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetDoNotDelete(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->doNotDelete ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

#ifdef HMX_WEBUI
OOIFReturnCode native_RecordingGetWillBeDeleted(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->willBeDeleted ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}
#endif

OOIFReturnCode native_RecordingSetDoNotDelete(RecordingHandle identifier, OOIFBoolean doNotDelete)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	OPL_BOOL		value = doNotDelete ? OPL_TRUE : OPL_FALSE;
	OPL_BOOL		backup;

	{
		backup = item->doNotDelete;
		item->doNotDelete = value;

		if (!OPL_Scheduler_Update(OPL_Scheduler_GetInstance(), (OPL_ScheduledItem_t *)item))
		{
			item->doNotDelete = backup;
			return OOIF_RETURN_ERROR;
		}
	}
	return OOIF_RETURN_OK;

}

OOIFReturnCode native_RecordingGetSaveDays(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->saveDays;
	return OOIF_RETURN_OK;

}

OOIFReturnCode native_RecordingSetSaveDays(RecordingHandle identifier, OOIFNumber saveDays)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	int		value = (int)saveDays;
	int		backup;

	if (item->saveDays != value)
	{
		backup = item->saveDays;
		item->saveDays = (int)value;
		if (!OPL_Scheduler_Update(OPL_Scheduler_GetInstance(), (OPL_ScheduledItem_t *)item))
		{
			item->saveDays = backup;
			return OOIF_RETURN_ERROR;
		}
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetSaveEpisodes(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->saveEpisodes;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingSetSaveEpisodes(RecordingHandle identifier, OOIFNumber saveEpisodes)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	int		value = (int)saveEpisodes;
	int		backup;

	if (item->saveEpisodes != value)
	{
		backup = item->saveEpisodes;
		item->saveEpisodes = (int)value;
		if (!OPL_Scheduler_Update(OPL_Scheduler_GetInstance(), (OPL_ScheduledItem_t *)item))
		{
			item->saveEpisodes = backup;
			return OOIF_RETURN_ERROR;
		}
	}
	return OOIF_RETURN_OK;

}

OOIFReturnCode native_RecordingGetBlocked(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->blocked ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}
#endif // OIPF

OOIFReturnCode native_RecordingGetParentalRatings(RecordingHandle identifier, int *count, ParentalRatingHandle **handles)
{
	ENTRY;
#ifdef JLABS
	return native_RecordingGetParentalRating(identifier, count, handles);
#else
	return native_ScheduledRecordingGetParentalRating((ScheduledRecordingHandle)identifier, count, handles);
#endif
}

#ifdef OIPF
OOIFReturnCode native_RecordingGetShowType(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->showType;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetSubtitles(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->subtitles ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetSubtitleLanguages(RecordingHandle identifier, int *count, char ***languages)
{
	ENTRY;

	OPL_Recording_t 	*item = (OPL_Recording_t *)identifier;
	OPL_Collection_t	*collection = item->subtitleLanguages;

	if (NULL == collection)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*languages = OPL_Collection_DumpString(collection, count);
	if (*languages == NULL)
	{
		*count     = 0;
		*languages = (char **)HLIB_STD_MemAlloc(4);
	}


#if 0
	if (collection->length == 0)
		return OOIF_RETURN_VALUE_NULL;

	char **array = (char **)HLIB_STD_MemAlloc(sizeof(char *) * collection->length);
	if (array == NULL)
		return OOIF_RETURN_OOM_ERROR;

	int c = 0;
	for (size_t i = 0 ; i < collection->length ; i++)
	{
		if (collection->data[i])
		{
			array[c++] = HLIB_STD_StrDup((char *)collection->data[i]);
		}
	}
	if (c == 0)
	{
		HLIB_STD_MemFree(array);
		return OOIF_RETURN_VALUE_NULL;
	}

	*languages = array;
	*count = c;
#endif
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetIsHD(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isHD ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetIsWidescreen(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isWideScreen ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetAudioType(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->audioType;

	return OOIF_RETURN_OK;
}

#if defined(HMX_WEBUI)
OOIFReturnCode native_RecordingGetLastPlayPosition(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->lastPlayPosition;

	return OOIF_RETURN_OK;
}
#endif // #if defined(HMX_WEBUI)

OOIFReturnCode native_RecordingGetIsMultilingual(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isMultilingual ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetAudioLanguages(RecordingHandle identifier, int *count, char ***languages)
{
	ENTRY;

	OPL_Recording_t 	*item = (OPL_Recording_t *)identifier;
	OPL_Collection_t	*collection = item->audioLanguages;

	if (NULL == collection)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*languages = OPL_Collection_DumpString(collection, count);
	if (*languages == NULL)
	{
		*count     = 0;
		*languages = (char **)HLIB_STD_MemAlloc(4);
	}
#if 0
	if (collection->length == 0)
		return OOIF_RETURN_VALUE_NULL;

	char **array = (char **)HLIB_STD_MemAlloc(sizeof(char *) * collection->length);
	if (array == NULL)
		return OOIF_RETURN_OOM_ERROR;

	int c = 0;
	for (size_t i = 0 ; i < collection->length ; i++)
	{
		if (collection->data[i])
		{
			array[c++] = HLIB_STD_StrDup((char *)collection->data[i]);
		}
	}
	if (c == 0)
	{
		HLIB_STD_MemFree(array);
		return OOIF_RETURN_VALUE_NULL;
	}

	*languages = array;
	*count = c;
#endif
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetGenres(RecordingHandle identifier, int *count, char ***genres)
{
	ENTRY;

	OPL_Recording_t 	*item = (OPL_Recording_t *)identifier;
	OPL_Collection_t	*collection = item->genres;

	if (NULL == collection)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*genres = OPL_Collection_DumpString(collection, count);
	if (*genres == NULL)
	{
		*count  = 0;
		*genres = (char **)HLIB_STD_MemAlloc(4);
	}

#if 0
	if (collection->length == 0)
		return OOIF_RETURN_VALUE_NULL;

	char **array = (char **)HLIB_STD_MemAlloc(sizeof(char *) * collection->length);
	if (array == NULL)
		return OOIF_RETURN_OOM_ERROR;

	int c = 0;
	for (size_t i = 0 ; i < collection->length ; i++)
	{
		if (collection->data[i])
		{
			array[c++] = HLIB_STD_StrDup((char *)collection->data[i]);
		}
	}
	if (c == 0)
	{
		HLIB_STD_MemFree(array);
		return OOIF_RETURN_VALUE_NULL;
	}

	*genres = array;
	*count  = c;
#endif
	return OOIF_RETURN_OK;

}
#endif // OIPF

OOIFReturnCode native_RecordingGetRecordingStartTime(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->recordingStartTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetRecordingDuration(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->recordingDuration;

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_RecordingGetBookmarks(RecordingHandle identifier, int *count, BookmarkHandle **handles)
{
	ENTRY;

	OPL_Recording_t		*item		= (OPL_Recording_t *)identifier;
	OPL_Collection_t	*collection = item->bookmarks;

	if (NULL == collection)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*handles = (BookmarkHandle *)OPL_Collection_Dump(collection, count, NULL);
	if (*handles == NULL)
	{
		*handles = (BookmarkHandle *)HLIB_STD_MemAlloc(strlen("will be auto-freed") + 1);
		*count = 0;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetLocked(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->locked;

	return OOIF_RETURN_OK;
}

#if defined(HMX_WEBUI)
OOIFReturnCode native_RecordingGetIsManualLocked(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isManualLocked? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingSetIsManualLocked(RecordingHandle identifier, OOIFBoolean isManualLocked)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	OPL_BOOL		value = isManualLocked ? OPL_TRUE : OPL_FALSE;
	OPL_BOOL		backup;

	if (item->isManualLocked!= value)
	{
		backup = item->isManualLocked;
		item->isManualLocked = value;

		if (!OPL_Scheduler_Update(OPL_Scheduler_GetInstance(), (OPL_ScheduledItem_t *)item))
		{
			item->isManualLocked = backup;
			return OOIF_RETURN_ERROR;
		}
	}
	return OOIF_RETURN_OK;

}

OOIFReturnCode native_RecordingGetIsScrambled(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isScrambled? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetIsEncrypted(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isEncrypted? TRUE : FALSE;
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_RecordingGetCopyControl(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->copyControl;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetCasIDs(RecordingHandle identifier, int *count, OOIFNumber **casIDs)
{
	ENTRY;

	OPL_Recording_t 		*item = (OPL_Recording_t *)identifier;
	eOplRecordingCasType	*tmpcasIDs = NULL;


	*count = OPL_Recording_GetCasIDs(item, &tmpcasIDs);

	if ((*count > 0) && (tmpcasIDs != NULL))
    {
    	*casIDs = (OOIFNumber *)HLIB_STD_MemAlloc(sizeof(OOIFNumber) * (*count));
        if (*casIDs)
        {
            for (int i = 0 ; i < *count ; i++)
            {
                (*casIDs)[i] = (OOIFNumber)tmpcasIDs[i];
            }
        }
        else
        {
            *count = 0;
        }

		HLIB_STD_MemFree(tmpcasIDs);
    }
    else
    {
        *casIDs = NULL;
    }

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetExpiryDate(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->expiryDate;
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_RecordingGetPausedPoints(RecordingHandle identifier, int *count, BookmarkHandle **handles)
{
	ENTRY;

	OPL_Recording_t		*item		= (OPL_Recording_t *)identifier;
	OPL_Collection_t	*collection = item->pausedPoints;

	if (NULL == collection)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*handles = (BookmarkHandle *)OPL_Collection_Dump(collection, count, NULL);
	if (*handles == NULL)
	{
		*handles = (BookmarkHandle *)HLIB_STD_MemAlloc(strlen("will be auto-freed") + 1);
		*count = 0;
	}

	return OOIF_RETURN_OK;
}

#endif /* defined(HMX_WEBUI) */

OOIFReturnCode native_RecordingAddBookmark(RecordingHandle recording, OOIFNumber time, const char *name, BookmarkHandle *retval)
{
	ENTRY;
	OOIF_LOG_DEBUG("Adding bookmark: %f %s", time, name);

	*retval = OPL_Recording_Bookmark_Add(recording, time, name);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingRemoveBookmark(RecordingHandle recording, BookmarkHandle bookmark)
{
    ENTRY;
	OOIF_LOG_DEBUG("Removing bookmark: %p", bookmark);

	OPL_Recording_Bookmark_Remove(recording, bookmark);
	return OOIF_RETURN_OK;
}

//part of Recording interface inherited form the ScheduledRecording class

OOIFReturnCode native_RecordingGetStartPadding(RecordingHandle identifier, OOIFNumber *retval)
{
    ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->startPadding;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetEndPadding(RecordingHandle identifier, OOIFNumber *retval)
{
   ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->endPadding;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingSetEndPadding(RecordingHandle identifier, OOIFNumber endPadding)
{
    OOIF_LOG_DEBUG("Setting Recording endPadding %d", static_cast<int>(endPadding));
    return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetRepeatDays(RecordingHandle identifier, OOIFNumber *retval)
{
    ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = item->repeatDays;
    return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetName(RecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->name)
		*retval = item->name;
	else if (OPL_ScheduledItem_InstanceOf(item, OPL_Recording_t)
			&& ((OPL_Recording_t *)item)->isManual)
		*retval = "";
	else
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetLongName(RecordingHandle identifier, const char **retval)
{
   ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->longName == NULL)
	{
		return OOIF_RETURN_VALUE_NULL;
	}
	*retval = item->longName;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetDescription(RecordingHandle identifier, const char **retval)
{
    ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->description == NULL)
	{
		return OOIF_RETURN_VALUE_NULL;
	}
	*retval = item->description;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetLongDescription(RecordingHandle identifier, const char **retval)
{
    ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->longDescription == NULL)
	{
		return OOIF_RETURN_VALUE_NULL;
	}
	*retval = item->longDescription;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetStartTime(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->startTime;
    return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetDuration(RecordingHandle identifier, OOIFNumber *retval)
{
    ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->duration;
    return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetChannel(RecordingHandle identifier, ChannelHandle *retval)
{
	ENTRY;
	HBOOL				bHaveChannelId = FALSE, bHaveLcn = FALSE;
	OPL_ScheduledItem_t *item 	= (OPL_ScheduledItem_t *)identifier;
	OPL_Recording_t		*recItem= (OPL_Recording_t *)identifier;

	if (item == NULL)
	{
		return OOIF_RETURN_ERROR;
	}

	if (item->channelID)
		bHaveChannelId = TRUE;

	if (recItem->lcn)
		bHaveLcn = TRUE;

	if (FALSE == bHaveChannelId && FALSE == bHaveLcn)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	OPL_ChannelManager_t chMgr = OPL_Channel_GetManager();
	OPL_Channel_t 		channel= NULL;

	if (bHaveChannelId)
	{	// get the channcel use channel id
		channel = OPL_Channel_FindChannel(chMgr, item->channelID);
	}

#ifdef CONFIG_OP_FREESAT
	if (bHaveLcn && NULL == channel)
	{	// retry with lcn
		channel = OPL_Channel_FindChannelByNumber(chMgr, recItem->lcn, 0);
	}
#else //CONFIG_OP_FREESAT
	if (NULL == channel)	/* Factory Reset후 Ch DB가 없는 상태로 record list가 만들어진 상황에서 Ch가 검색되어 record list가 처리될 때의 예외 처리 */
	{
		channel = OPL_Channel_FindChannelByTriplet(chMgr, item->originalNetworkId, item->transportStreamId, item->serviceId);
	}
#endif //CONFIG_OP_FREESAT

	if (NULL == channel)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*retval = (ChannelHandle)OPL_Channel_Clone(channel);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetProgrammeID(RecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->programmeID == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = item->programmeID;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetProgrammeIDType(RecordingHandle identifier, ScheduledRecordingProgrammeIDType *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->programmeID == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (ScheduledRecordingProgrammeIDType)OPL_Programme_GetIDTypeFromProgrammeID(item->programmeID);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetParentalRating(RecordingHandle identifier, int *count, ParentalRatingHandle **handles)
{
#ifdef JLABS
	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	int	value = 0;
	OPL_ParentalRating_t *parentalRating;
	ParentalRatingHandle *myHandles = (ParentalRatingHandle*)HLIB_STD_MemAlloc(sizeof(ParentalRatingHandle));

	if (NULL != item)
	{
		value = item->parentalRating;
	}
	parentalRating = OPL_PCMAN_PRC_AddParentalRating(NULL, NULL, NULL, value, 0, NULL);
	myHandles[0] = (ParentalRatingHandle)parentalRating;

	*handles = myHandles;
	*count = 1;

	return OOIF_RETURN_OK;

#else

	return OOIF_RETURN_VALUE_UNDEFINED;

#endif
}

#endif // OIPF

#if defined OIPF || defined OPT_HBBTV_SCHED_REC_SERIES
OOIFReturnCode native_RecordingGetEpisode(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;
	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->episode;
    return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetTotalEpisodes(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->totalEpisodes;
	return OOIF_RETURN_OK;
}
#ifdef HMX_WEBUI
OOIFReturnCode native_RecordingGetSeason(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;
	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->season;
    return OOIF_RETURN_OK;
}
#endif
#endif // defined OIPF || defined OPT_HBBTV_SCHED_REC_SERIES

OOIFReturnCode native_SchedulerConfirmScheduled(ScheduledRecordingHandle identifier, OOIFNumber type, OOIFNumber check)
{
	ENTRY;

	OPL_Scheduler_Confirm(OPL_Scheduler_GetInstance(), (OPL_ScheduledItem_t *)identifier, type, check);

	return OOIF_RETURN_OK;
}


#if defined(HMX_WEBUI)
OOIFReturnCode native_RecordingGetThumbnail(RecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	*retval = OPL_Recording_GetThumbnail(item);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_RecordingGetIsScheduledAsSeries(ScheduledRecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (item->isScheduledAsSeries) ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetSeriesID(ScheduledRecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->seriesID == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = item->seriesID;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetField(RecordingHandle id, const char *fieldId,
										const char **retval)
{

 	OPL_Recording_t *recordingObj = static_cast<OPL_Recording_t *>(id);
	OPL_Collection_t	*collection;
	char	*data = NULL;

 	if ( !recordingObj )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	/**
	 * const char **retval, 러턴 값에 const가 붙은 경우, caller쪽에서 메모리를 책임지지 않습니다.
	 * 따라서 무조건 string 형태(char *)의 값만을 리턴해야 해요
	 * 아마도, getField라는 api는 아주아주 많이 추상화된 api이므로,
	 * return 값으로는 일반 string 혹은 json 형태의 string이 올라가야 할 것으로 보입니다.
	 * 밑은 example이여요.
	 * javascript에서는 JSON.parse()를 사용하면 되고요.
	 */
    	if (!strcmp(fieldId,(const char *)"extEventInfo"))
	{
		collection = recordingObj->extEventInfo;
		data = OPL_Collection_ToJSON((const char *)"extEventInfo", collection, NULL);
	}
#ifdef	CONFIG_OP_FREESAT
	else if (!strcmp(fieldId,(const char *)"freesat/guidance"))
	{
		collection = recordingObj->guidance;
		data = OPL_Collection_ToJSON((const char *)"guidance", collection, NULL);
	}
	else if(!strcmp(fieldId,(const char *)"freesat/hasAudioDescription"))
	{
		data =OPL_Recording_HasAudioDescription(recordingObj);
	}
	else if(!strcmp(fieldId,(const char *)"freesat/hasSigned"))
	{
		data = OPL_Recording_HasSigned(recordingObj);
	}
	else if(!strcmp(fieldId,(const char *)"freesat/hasAC3"))
	{
		data = OPL_Recording_HasAC3(recordingObj);
	}
#endif // CONFIG_OP_FREESAT
	else if (!strcmp(fieldId,(const char *)"freeview/guidance"))
	{
		data = HLIB_STD_StrDup(recordingObj->guidance);
	}
	else if (!strcmp(fieldId,(const char *)"freeview/hasAudioDescription"))
	{
		data = OPL_Recording_HasAudioDescription(recordingObj);
	}

	// 일케 안주면, 메모리 누수 ㅠㅠ
	*retval = (const char *)OPL_Recording_PutGarbage(recordingObj, (void *)data);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetGroupCRIDs(RecordingHandle id, int *count,
											 char ***crids)
{
 	OPL_Collection_t *coll;
	OPL_Recording_t *item = (OPL_Recording_t *)id;
	coll = item->seriesCRIDs;
	if (coll)
	{
		*crids = OPL_Collection_DumpString(coll, count);
	}
	else
	{
		*crids = NULL;
	}
	if (*crids == NULL)
	{
		*count  = 0;
		*crids = (char **)HLIB_STD_StrDup("will be freed");
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetProgrammeCRID(ProgrammeHandle id,const char **retval)
{
	OPL_Recording_t *item = (OPL_Recording_t *)id;

	if(HxSTD_StrEmpty(item->programmeCRID))
		*retval = "";
	else
		*retval = HLIB_STD_StrDup(item->programmeCRID);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetSeriesTitle(ProgrammeHandle id,const char **retval)
{
	OPL_Recording_t *item = (OPL_Recording_t *)id;

	if(HxSTD_StrEmpty(item->seriesTitle))
		*retval = "";
	else
		*retval = HLIB_STD_StrDup(item->seriesTitle);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetSubtitleTypes(RecordingHandle id, int *count, char ***types)
{
 	OPL_Collection_t *coll;
	OPL_Recording_t *item = (OPL_Recording_t *)id;
	coll = item->subtitletypes;
	if (coll)
	{
		*types = OPL_Collection_DumpString(coll, count);
	}
	else
	{
		*types = NULL;
	}
	if (*types == NULL)
	{
		*count  = 0;
		*types = (char **)HLIB_STD_StrDup("will be freed");
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetDoNotDeleteSetTime(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->doNotDeleteSetTime;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_RecordingStartSatRecord(void)
{
	OPL_Scheduler_StartSatRecord();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingSetName(RecordingHandle id, const char *name, OOIFBoolean *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	*retval = OPL_Scheduler_SetName(item, (unsigned char *)name);

	return OOIF_RETURN_OK;
}
#endif /* HMX_WEBUI */

#ifdef JLABS
OOIFReturnCode native_RecordingGetRecMode(RecordingHandle id, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->recMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetFromDap(RecordingHandle id, OOIFBoolean *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->fromDap;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetFactoryType(RecordingHandle id, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	switch (item->type)
	{
	case eOPL_SCHEDULED_PRO_RECORD:
		*retval = SCHEDULED_RECORDING_RECORDING;
		break;

	case eOPL_SCHEDULED_PRO_WATCH:
		*retval = SCHEDULED_RECORDING_VIEWING;
		break;

	case eOPL_SCHEDULED_PRO_DLNA:
		*retval = SCHEDULED_RECORDING_DLNA_UPLOAD;
		break;

	case eOPL_SCHEDULED_PRO_DUBBING:
		*retval = SCHEDULED_RECORDING_DUBBING;
		break;

	default:
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetDigitalCopyControl(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->digitalCopyControl;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetAnalogCopyControl(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->analogCopyControl;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetDigitalCopyCount(RecordingHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->digitalCopyCount;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetTranscoded(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->isTranscoded;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetWatched(RecordingHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->isWatched;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetDiscInfo(HDiscInfoHandle *handle)
{

	OplDiscInfo	Info;

	// Default PVR HDD의 info만 가지고 온다.
	if (OPL_Discinfo_GetDefaultDiscInfo(&Info) != BPL_STATUS_OK)
	{
		return OOIF_RETURN_ERROR;
	}

	*handle = (HDiscInfoHandle)OPL_DiscInfo_Clone(&Info);

	HLIB_STD_MemFree(Info.Name);
	HLIB_STD_MemFree(Info.DiscId);

	if (!(*handle))
		return OOIF_RETURN_VALUE_NULL;
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_RecordingGetStorages(int *count, HDiscInfoHandle **handles)
{
	HDiscInfoHandle *array = NULL;
	HUINT32 	cnt=0, i;
	OplDiscInfo *Info = NULL;

	OPL_Discinfo_GetDiscInfoList(&Info, &cnt, eDiscInfo_All);

	array = new HDiscInfoHandle[cnt];

	for(i = 0 ; i < cnt ; i++)
	{
		array[i] = (HDiscInfoHandle)OPL_DiscInfo_Clone(&Info[i]);
	}

	*count = cnt;
	*handles = array;
	array = NULL;

	delete []Info;
	return OOIF_RETURN_OK;

}

OOIFReturnCode native_RecordingGetThumbnailURL(RecordingHandle identifier, const char **retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	if (item->thumbnail== NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = item->thumbnail;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetGroupIndex(RecordingHandle id, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item == NULL)
		return OOIF_RETURN_ERROR;

	*retval = item->groupIndex;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetUserID(RecordingHandle id, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item->userID)
	{
		*retval = item->userID;
	}
	else
	{
		*retval = "";
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetDeviceHandle(RecordingHandle id, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item->deviceHandle)
	{
		*retval = item->deviceHandle;
	}
	else
	{
		*retval = "";
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetDestinationId(RecordingHandle id, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item->destinationId)
	{
		*retval = item->destinationId;
	}
	else
	{
		*retval = "";
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingGetChapters(RecordingHandle identifier, int *count, BookmarkHandle **handles)
{
	ENTRY;

	OPL_Recording_t		*item		= (OPL_Recording_t *)identifier;
	OPL_Collection_t	*collection = item->chapters;

	if (NULL == collection)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*handles = (BookmarkHandle *)OPL_Collection_Dump(collection, count, NULL);
	if (*handles == NULL)
	{
		*handles = (BookmarkHandle *)HLIB_STD_MemAlloc(strlen("will be auto-freed"));
		*count = 0;
	}

	return OOIF_RETURN_OK;
}

#endif // JLABS

#if (defined(JLABS) || defined(HMX_WEBUI))
OOIFReturnCode native_RecordingGetStorageId(RecordingHandle id, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)id;

	if (item->storageID)
	{
		*retval = item->storageID;
	}
	else
	{
		*retval = "";
	}
	return OOIF_RETURN_OK;
}
#endif //(defined(JLABS) || defined(HMX_WEBUI))

#endif // HBBTV_PVR || OIPF

/* EOF */

