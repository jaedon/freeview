/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(HBBTV_PVR) || defined(OIPF)

#ifndef __NATIVE_RECORDING
#define __NATIVE_RECORDING

#include "NativeChannel.h"
#include "NativeParentalControlManager.h"
#ifdef OIPF
#include "NativeBookmark.h"
#endif /* OIPF */

#ifdef JLABS
#include "NativeDisc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   Forward declaration.
*/
typedef void* ProgrammeHandle;

/**
  \defgroup SchedRec Scheduled Recording APIs
  @{
   This is the native interface for the scheduled recordings APIs as
   defined in DAE 7.10.
*/

/**
   Opaque reference to a ScheduledRecording instace in
   the native platform as defined in DAE 7.10.2.
 */
typedef void* ScheduledRecordingHandle;

/**
   Opaque reference to a Recording instance in the native platform
   as defined in DAE 7.10.5.
 */
typedef void* RecordingHandle;

typedef void* jlabsCDSRecordingHandle;


#ifdef OIPF
#ifndef HMX_WEBUI
typedef void (scheduleListener)(RecordingHandle id, OOIFNumber state);
#else //HMX_WEBUI
typedef void (scheduleListener)(OOIFNumber state, ScheduledRecordingHandle id, OOIFNumber error, ScheduledRecordingHandle *conclicts, int count, ScheduledRecordingHandle *alternatives, int alt_count, ProgrammeHandle *altevents,  int alt_evt_count);
typedef void (recordListener)(OOIFNumber state, ScheduledRecordingHandle id, OOIFNumber error);
typedef void (*RecordingFileOperateListener)(int ticket, int result);
#endif	//HMX_WEBUI
OOIFReturnCode native_SchedulerSetListener(scheduleListener listener);
OOIFReturnCode native_RecordSetListener(recordListener listener);
#endif /* OIPF */
/*------------------------------------------
  Recording Scheduler
  ------------------------------------------*/

#ifdef JLABS
/**
	Recording Type constants as defined in JLabs SPEC-023 Annex B B.3.5.1.1
*/
typedef enum {
	RECORDING_SCHEDULER_USB_RECORDING = 0,
	RECORDING_SCHEDULER_VIEWING_RESERVATION,
	RECORDING_SCHEDULER_DLNA_UPLOAD,
	RECORDING_SCHEDULER_DLNA_COPY
} RecordingSchedulerJlabsConstants;

typedef void (hnReservationFinishedListener)(ScheduledRecordingHandle recording, OOIFNumber errorCode);
OOIFReturnCode native_RecordingSetHNReservationFinishedListener(hnReservationFinishedListener);
typedef void (hnRemoveResultListener)(char *strRemove, OOIFNumber errorCode);
OOIFReturnCode native_RecordingSetHNRemoveResultListener(hnRemoveResultListener);

#endif // JLABS

/**
   Called when a new RecordingScheduler instance is created. The return
   value may be used for capability and security checks.
   \param window The identifier of the window the create call originates from
   \param host The host URI of the application the create call originates from
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_RecordingSchedulerOnCreate(long int window, const char *host);

#ifdef OIPF
/**
 * Called when an event listener is added, removed, or accessed from Javascript
 * on the RecordingScheduler.  OOIF will handle it automatically, so no action
 * is needed from the integration layer. Nonetheless, this callback gives the
 * oportunity to prevent the listener from being added, removed, or accessed,
 * by returning an error code. This may be used to implement security checks,
 * as defined in section HbbTV 11.1, or capability checks as defined in DAE 7
 * and DAE 9.3
 * is being added, removed, or accessed.
 * \param eventName the name of the event
 * \return OOIF_RETURN_OK, or OOIF_RETURN_SECURITY_ERROR, or OOIF_RETURN_TYPE_ERROR
 */
OOIFReturnCode native_SchedulerOnListenerAccessed(const char *eventName);
#endif /*OIPF*/

#ifdef JLABS
typedef enum {
	SCHEDULED_RECORDING_RECORDING = 0,
	SCHEDULED_RECORDING_VIEWING = 1,
	SCHEDULED_RECORDING_DLNA_UPLOAD = 2,
	SCHEDULED_RECORDING_DUBBING = 3,

	SCHEDULED_RECORDING_SERIES_RECORDING = 10
} ScheduledRecordingFactoryType;

OOIFReturnCode native_SchedulerRecord(ProgrammeHandle identifier, OOIFNumber factory, OOIFNumber recmode, const char *deviceStr, const char *destinationId, long int window_identifier, const char *host, ScheduledRecordingHandle *retval);
OOIFReturnCode native_SchedulerRecordAt(OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, OOIFNumber factory, OOIFNumber recmode, const char *deviceStr, const char *destinationId, const char *host, ScheduledRecordingHandle *retval);

#elif defined(HMX_WEBUI)
typedef enum {
	SCHEDULED_RECORDING_RECORDING = 0,
	SCHEDULED_RECORDING_VIEWING = 1,
	SCHEDULED_RECORDING_NETSTREAMING_LIVE = 4,
	SCHEDULED_RECORDING_NETSTREAMING_FILE = 5,
	SCHEDULED_RECORDING_POWER_ON_TIME = 6,
	SCHEDULED_RECORDING_REGULAR_OTA = 7,
	SCHEDULED_RECORDING_SAT_RECORDING = 8,
	SCHEDULED_RECORDING_ASO_NOTIFY = 9,

	SCHEDULED_RECORDING_SERIES_RECORDING = 10,
	SCHEDULED_RECORDING_ALTERNATIVE_RECORDING = 11,
	SCHEDULED_RECORDING_ALTERNATIVE_SERIES_RECORDING = 12,

	SCHEDULED_RECORDING_SERIES_VIEWING = 13,
	SCHEDULED_RECORDING_ALTERNATIVE_VIEWING = 14,
	SCHEDULED_RECORDING_ALTERNATIVE_SERIES_VIEWING = 15,
} ScheduledRecordingFactoryType;

typedef enum {
	SCHEDULED_RECTARGET_MANUAL = 0,
	SCHEDULED_RECTARGET_INTHDD = 1,
	SCHEDULED_RECTARGET_INTSUBHDD = 2,
	SCHEDULED_RECGTAGET_EXTHDD = 3,
	SCHEDULED_RECTARGET_MAX = 4
}ScheduledRecordingTargetType;

typedef enum {
	RECORDING_SCHEDULER_FILEOP_STATE_ING = 0,
	RECORDING_SCHEDULER_FILEOP_STATE_COMPLETE = 1,
	RECORDING_SCHEDULER_FILEOP_STATE_ERROR = 2,
	RECORDING_SCHEDULER_FILEOP_STATE_UNKNOWN = 3,
	RECORDING_SCHEDULER_FILEOP_STATE_STOP = 4,

	RECORDING_SCHEDULER_FILEOP_NOT_ENOUGH_SPACE = 5,
	RECORDING_SCHEDULER_FILEOP_STATE_SIZE_LIMIT_4G = 6,
} RecordingSchedulerFileOpStateType;

OOIFReturnCode native_SchedulerRecord(ProgrammeHandle identifier, OOIFNumber factory, OOIFNumber targetStorage, long int window_identifier, const char *host, ScheduledRecordingHandle *retval);
OOIFReturnCode native_SchedulerRecordAt(OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, OOIFNumber factory, OOIFNumber targetStorage,OOIFNumber volume, const char *host, ScheduledRecordingHandle *retval);
OOIFReturnCode native_SchedulerRecordCrid(OOIFNumber factory, const char *crid, const char *title, const char *channelId, long int window_identifier, const char *host, ScheduledRecordingHandle *retval);
OOIFReturnCode native_SchedulerUpdate(RecordingHandle identifier, OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, OOIFNumber factory, OOIFNumber targetStorage, const char *host, RecordingHandle *retval);
OOIFReturnCode native_SchedulerUpdateScheduled(ScheduledRecordingHandle id, OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, OOIFNumber factory, OOIFNumber targetStorage, const char *host, ScheduledRecordingHandle *retval);


OOIFReturnCode native_SchedulerRegRecFileOpEventListener(RecordingFileOperateListener listener);
OOIFReturnCode native_SchedulerUnRegRecFileOpEventListener(void);

OOIFReturnCode native_SchedulerCopyRecording(RecordingHandle identifier, OOIFNumber targetStorage, OOIFNumber *ticketNumber);
OOIFReturnCode native_SchedulerMoveRecording(RecordingHandle identifier, OOIFNumber targetStorage, OOIFNumber *ticketNumber);
OOIFReturnCode native_SchedulerRemoveRecording(RecordingHandle identifier, OOIFNumber *ticketNumber);
OOIFReturnCode native_SchedulerStopFileOp(OOIFNumber ticketNumber, OOIFNumber *pRetTicketNumber);
OOIFReturnCode native_SchedulerSetManualLock(const char *seriesID, OOIFBoolean lockState);

#else
/**
   This function corresponds to the OIPF function record() as
   defined in DAE 7.10.1.1.
   \param programme As defined in DAE 7.10.1.1
   \param window_identifier The identifier of the window the create
   call originates from
   \param host Domain of the service that is issuing the request
   \param retval Retval as defined in DAE 7.10.1.1
   \return OOIF return code
*/
OOIFReturnCode native_SchedulerRecord(ProgrammeHandle programme, long int window_identifier, const char* host, ScheduledRecordingHandle *retval);

/**
   This function corresponds to the OIPF function recordAt() as
   defined in DAE 7.10.1.1.
   \param startTime As defined in DAE 7.10.1.1
   \param duration As defined in DAE 7.10.1.1
   \param repeatDays As defined in DAE 7.10.1.1
   \param channelID As defined in DAE 7.10.1.1
   \param host Domain of the service that is issuing the request
   \param retval Retval as defined in  7.10.1.1
   \return OOIF return code
*/
OOIFReturnCode native_SchedulerRecordAt(OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, const char *host, ScheduledRecordingHandle *retval);
#endif

/**
   This function corresponds to the OIPF function getScheduledRecordings() as
   defined in DAE 7.10.1.1.
   \param host Domain of the service that is issuing the request
   \param count Number of scheduled recording handles in the returned array
   \param handles Use this to return the composed scheduled recording
   handles array. The OOIF plugin will take over ownership of the
   array and will later call free() to de-allocate the memory
   \return OOIF return code
*/
OOIFReturnCode native_SchedulerGetScheduledRecordings(const char *host, int *count, ScheduledRecordingHandle **handles);
#ifdef HMX_WEBUI
OOIFReturnCode native_SchedulerGetCountScheduleList(int *count);
OOIFReturnCode native_SchedulerGetCountRecordingList(int *count);
OOIFReturnCode native_SchedulerGetScheduleList(int *count, ScheduledRecordingHandle **handles);
OOIFReturnCode native_SchedulerGetRecordingList(int *count, ScheduledRecordingHandle **handles);
#endif

/**
   This function corresponds to the OIPF function remove() as
   defined in DAE 7.10.1.1.
   \param id Scheduled Recording instance identifier
   \return OOIF return code
*/
OOIFReturnCode native_SchedulerRemoveScheduled(ScheduledRecordingHandle id);
#ifdef HMX_WEBUI
OOIFReturnCode native_SchedulerRemoveFirstScheduled(ScheduledRecordingHandle id);
OOIFReturnCode native_SchedulerRemoveWholeSeries(ScheduledRecordingHandle identifier);
#endif
OOIFReturnCode native_SchedulerConfirmScheduled(ScheduledRecordingHandle identifier, OOIFNumber type, OOIFNumber check);
/**
   Gets OIPF property recordings as defined in DAE 7.10.4.1.
   \param count Number of recording handles in the returned array
   \param handles Use this to return the recording handles array. The
   OOIF plugin will take over ownership of the array and will later
   call free() to de-allocate the memory
   \return OOIF return code
*/
#ifdef JLABS
OOIFReturnCode native_Jlabs_getScheduledRecordingConflictInfo(const char *host, int *count, ScheduledRecordingHandle **handles);
OOIFReturnCode native_Jlabs_SchedulerGetRecordings(const char *discId, int *count, RecordingHandle **handles);
OOIFReturnCode native_SchedulerRemoveCDSRecording(jlabsCDSRecordingHandle identifier, char **retVal);
#endif /* JLABS */

OOIFReturnCode native_SchedulerGetRecordings(int *count, ScheduledRecordingHandle **handles);

#ifdef OIPF
/** 7.10.4.2 */
OOIFReturnCode native_SchedulerRemove(RecordingHandle id);
/** 7.10.4.2 */
OOIFReturnCode native_SchedulerStop(RecordingHandle id);
/** 7.10.4.2 */
OOIFReturnCode native_SchedulerGetRecording(const char *id, RecordingHandle *retval);
/** 7.10.4.2 */
OOIFReturnCode native_SchedulerRefresh();
#endif /* OIPF */

OOIFReturnCode native_getCurrentRecordings(int *count, RecordingHandle **handles);


/*------------------------------------------
  ScheduledRecording
  ------------------------------------------*/
/** @name ScheduledRecording
	The ScheduledRecording class as defined in DAE 7.10.2.
	@{*/
/**
  Constants specified in DAE 7.10.2.1
*/
typedef enum {
#ifdef OIPF
	SCHEDULED_RECORDING_ID_TVA_CRID = 0,
#endif /* OIPF */
	SCHEDULED_RECORDING_ID_DVB_EVENT = 1,
#ifdef OIPF
	SCHEDULED_RECORDING_ID_TVA_GROUP_CRID = 2
#endif /* OIPF */
} ScheduledRecordingProgrammeIDType;

/**
  Constants defined in DAE 7.10.2.2, as possible values
  for the repeatDays properties
*/
typedef enum {
	SCHEDULED_RECORDING_SUNDAY = 0x1,
	SCHEDULED_RECORDING_MONDAY = 0x2,
	SCHEDULED_RECORDING_TUESDAY = 0x4,
	SCHEDULED_RECORDING_WEDNESDAY = 0x8,
	SCHEDULED_RECORDING_THURSDAY = 0x10,
	SCHEDULED_RECORDING_FRIDAY = 0x20,
	SCHEDULED_RECORDING_SATURDAY = 0x40,
} ScheduledRecordingDay;

/**
   Called when the ScheduledRecording instance is destroyed in the OOIF plugin.
   \param id ScheduledRecording instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_ScheduledRecordingReleaseHandle(ScheduledRecordingHandle id);

#ifdef JLABS
OOIFReturnCode native_SchedulerCopy(OOIFNumber startTime, RecordingHandle identifier, const char *deviceId, const char *destinationId, ScheduledRecordingHandle *retval);
OOIFReturnCode native_SchedulerSetDefaultDMS(const char *deviceHandle, const char *destinationId, OOIFNumber *retval);
OOIFReturnCode native_SchedulerGetDefaultDMSDeviceHandle(unsigned char **retval);
OOIFReturnCode native_SchedulerGetDefaultDMSRecordDestinationId(unsigned char **retval);
OOIFReturnCode native_SchedulerSetName(ScheduledRecordingHandle id, const char *name, OOIFBoolean *retval);
OOIFReturnCode native_ScheduledRecordingGetFactoryType(ScheduledRecordingHandle id, OOIFNumber *retval);
OOIFReturnCode native_ScheduledRecordingGetStorageId(ScheduledRecordingHandle id, const char **retval);
OOIFReturnCode native_ScheduledRecordingGetRecMode(ScheduledRecordingHandle id, OOIFNumber *retval);
OOIFReturnCode native_ScheduledRecordingGetFromDap(ScheduledRecordingHandle id, OOIFBoolean *retval);
OOIFReturnCode native_ScheduledRecordingGetGroupIndex(ScheduledRecordingHandle id, OOIFNumber *retval);
OOIFReturnCode native_ScheduledRecordingGetUserID(ScheduledRecordingHandle id, const char **retval);
OOIFReturnCode native_ScheduledRecordingGetDeviceHandle(ScheduledRecordingHandle id, const char **retval);
OOIFReturnCode native_ScheduledRecordingGetDestinationId(ScheduledRecordingHandle id, const char **retval);
#elif HMX_WEBUI
OOIFReturnCode native_ScheduledRecordingGetFactoryType(ScheduledRecordingHandle id, OOIFNumber *retval);
OOIFReturnCode native_ScheduledRecordingGetUID(ScheduledRecordingHandle id,OOIFNumber *retval);
OOIFReturnCode native_SchedulerGetGroupCRIDs(ScheduledRecordingHandle identifier, int *count, char ***crids);
OOIFReturnCode native_ScheduledRecordingGetProgrammeCRID(ScheduledRecordingHandle id,const char **retval);
OOIFReturnCode native_ScheduledRecordingIsEventBased(ScheduledRecordingHandle id, OOIFBoolean *retval);
OOIFReturnCode native_ScheduledRecordingGetDiseqcInput(ScheduledRecordingHandle identifier,  OOIFNumber *retval);
OOIFReturnCode native_ScheduledRecordingGetFrequency(ScheduledRecordingHandle identifier,  OOIFNumber *retval);
OOIFReturnCode native_ScheduledRecordingGetPolarisation(ScheduledRecordingHandle identifier,  OOIFNumber *retval);
OOIFReturnCode native_ScheduledRecordingGetSymbolRate(ScheduledRecordingHandle identifier,  OOIFNumber *retval);
OOIFReturnCode native_ScheduledRecordingGetSatType(ScheduledRecordingHandle identifier, const char **retval);
#endif

/**
   Gets OIPF property startPadding as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetStartPadding(ScheduledRecordingHandle id, OOIFNumber *retval);

/**
   Sets OIPF property startPadding as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param startPadding As defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingSetStartPadding(ScheduledRecordingHandle id, OOIFNumber startPadding);

/**
   Gets OIPF property endPadding as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetEndPadding(ScheduledRecordingHandle id, OOIFNumber *retval);

/**
   Sets OIPF property endPadding as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param endPadding As defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingSetEndPadding(ScheduledRecordingHandle id, OOIFNumber endPadding);

/**
   Gets OIPF property repeatDays as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetRepeatDays(ScheduledRecordingHandle id, OOIFNumber *retval);

/**
   Gets OIPF property name as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetName(ScheduledRecordingHandle id, const char **retval);

/**
   Gets OIPF property longName as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetLongName(ScheduledRecordingHandle id, const char **retval);

/**
   Gets OIPF property description as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetDescription(ScheduledRecordingHandle id, const char **retval);

/**
   Gets OIPF property longDescription as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetLongDescription(ScheduledRecordingHandle id, const char **retval);

/**
   Gets OIPF property startTime as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetStartTime(ScheduledRecordingHandle id, OOIFNumber *retval);

/**
   Gets OIPF property duration as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetDuration(ScheduledRecordingHandle id, OOIFNumber *retval);

/**
   Gets OIPF property channel as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
#ifdef HMX_WEBUI
OOIFReturnCode native_ScheduledRecordingSetDuration(ScheduledRecordingHandle identifier, OOIFNumber duration);
#endif
OOIFReturnCode native_ScheduledRecordingGetChannel(ScheduledRecordingHandle id, ChannelHandle *retval);

#if defined HMX_WEBUI
/**
   Gets property IsScheduledAsSeries (defined by HUMAX)
   \param id Scheduled Recording instance identifier
   \param retval Retval (true => shceudled as series / false => scheduled as Single Episode )
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetIsScheduledAsSeries(ScheduledRecordingHandle id, OOIFBoolean *retval);
/**
   Gets property seriesID (defined by HUMAX)
   \param id Scheduled Recording instance identifier
   \param retval Retval (series id)
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetSeriesID(ScheduledRecordingHandle id, const char **retval);
/**
   Gets property volume (defined by HUMAX)
   \param id Scheduled Recording instance identifier
   \param retval Retval (power on timer volume)
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetVolume(ScheduledRecordingHandle id,  OOIFNumber *volume);
/**
   Gets property schedule status (defined by HUMAX)
	  \param id Scheduled Recording instance identifier
	  \param retval Retval (power on timer volume)
	  \return OOIF return code
   */
OOIFReturnCode native_ScheduledRecordingGetStatus(ScheduledRecordingHandle id,  OOIFNumber *status);
 /**
   Gets property audio type (defined by HUMAX)
   \param id Scheduled Recording instance identifier
   \param retval Retval (power on timer volume)
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetAudioType(ScheduledRecordingHandle id, OOIFNumber *audioType);
#endif /* HMX_WEBUI */

/**
   Gets OIPF property programmeId as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetProgrammeID(ScheduledRecordingHandle id, const char **retval);

/**
   Gets OIPF property programmeIDType as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetProgrammeIDType(ScheduledRecordingHandle id, ScheduledRecordingProgrammeIDType *retval);

#if defined OIPF || defined OPT_HBBTV_SCHED_REC_SERIES
/**
   Gets OIPF property episode as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetEpisode(ScheduledRecordingHandle id, OOIFNumber *retval);

/**
   Gets OIPF property totalEpisodes as defined in DAE 7.10.2.2.
   \param id ScheduledRecording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetTotalEpisodes(ScheduledRecordingHandle id, OOIFNumber *retval);
#endif /* OOIF || OPT_HBBTV_SCHED_REC_SERIES */

/**
   Gets OIPF property parentalRating as defined in DAE 7.10.2.2.
   \param id ScheduledRecording instance identifier
   \param count Number of handles in the returned array
   \param handles Use this to return the parental rating handles
   array. The OOIF plugin will take over ownership of the array and
   will later call free() to de-allocate the memory
   \return Number of handles in the returned array
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingGetParentalRating(ScheduledRecordingHandle id, int *count, ParentalRatingHandle **handles);

/**
   Called to distinguish handle between ScheduledRecording and Recording.
   \param id ScheduledRecording instance identifier
   \return retval should be true if the recording was aready recorded
   \return OOIF return code
*/
OOIFReturnCode native_ScheduledRecordingIsRecorded(ScheduledRecordingHandle id, OOIFBoolean *retval);

/*@}*/

/*------------------------------------------
  Recording
  ------------------------------------------*/
/** @name Recording
	The Recording class as defined in DAE 7.10.5.
	@{*/


/**
  Constants defined in DAE 7.10.5.1, constants for recording and timeshift states.
*/
typedef enum {
    RECORDING_UNREALIZED = 0,
    RECORDING_SCHEDULED,
    RECORDING_DEL_SCHEDULED,
    RECORDING_REC_PRESTART,
    RECORDING_REC_ACQUIRING_RESOURCES,
    RECORDING_REC_STARTED,
    RECORDING_REC_UPDATED,
    RECORDING_REC_COMPLETED,
    RECORDING_REC_PARTIALLY_COMPLETED,
    RECORDING_TS_ACQUIRING_RESOURCES,
    RECORDING_TS_STARTED,
    RECORDING_ERROR,
    RECORDING_REC_START_ERROR,
    RECORDING_REC_START_CONFLICT,
    RECORDING_REC_LOAD
} RecordingConstants;

/**
  Constants defined in DAE 7.10.5.1, constants for detailed error codes
  Specification does not define values for these constants.
  Implementations may use any values as long as the value of each constant is unique.)
*/
typedef enum {
    RECORDING_ERROR_REC_RESOURCE_LIMITATION = 0,
    RECORDING_ERROR_INSUFFICIENT_STORAGE	= 1,
    RECORDING_ERROR_TUNER_CONFLICT			= 2,
    RECORDING_ERROR_REC_DRM_RESTRICTION		= 3,
    RECORDING_ERROR_REC_UNKNOWN				= 4,
    RECORDING_ERROR_TS_RESOURCE_LIMITATION	= 5,
    RECORDING_ERROR_TS_DRM_RESTRICTION		= 6,
    RECORDING_ERROR_HAVE_ALTERNATIVE		= 7,
    RECORDING_ERROR_TS_UNKNOWN				= 8,
    RECORDING_ERROR_RFOTA					= 9,
    RECORDING_ERROR_NO_AV					= 10,
    RECORDING_ERROR_POWER_OFF				= 11,
    RECORDING_ERROR_REC_CAS					= 12,
    RECORDING_ERROR_HIGHER_PRIORITY			= 13,
    RECORDING_ERROR_TRACKING_FAIL			= 14,
    RECORDING_ERROR_OFF_AIR					= 15,
    RECORDING_ERROR_SATIP_FAILED			= 16,
    RECORDING_ERROR_MAX_RECORD_LIST         = 17, 
} DetailedRecordingErrorsCodes;

/**
   Called when the Recording instance is destroyed in the OOIF plugin.
   \param id Recording instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_RecordingReleaseHandle(RecordingHandle id);

/**
   Gets OIPF property state as defined in DAE 7.10.5.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
 */
OOIFReturnCode native_RecordingGetState(RecordingHandle id, OOIFNumber *retval);

#ifdef OIPF
/**
   Gets OIPF property error as defined in DAE 7.10.5.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
 */
OOIFReturnCode native_RecordingGetError(RecordingHandle identifier, OOIFNumber *retval);
#endif /* OIPF */

/**
   Gets OIPF property id as defined in DAE 7.10.5.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
 */
OOIFReturnCode native_RecordingGetID(RecordingHandle id, const char **retval);


#ifdef OIPF
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetIsManual(RecordingHandle id, OOIFBoolean *retval);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetDoNotDelete(RecordingHandle id, OOIFBoolean *retval);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingSetDoNotDelete(RecordingHandle id, OOIFBoolean doNotDelete);

#if defined(HMX_WEBUI)
OOIFReturnCode native_RecordingGetWillBeDeleted(RecordingHandle id, OOIFBoolean *retval);
#endif // #if defined(HMX_WEBUI)

/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetSaveDays(RecordingHandle id, OOIFNumber *retval);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingSetSaveDays(RecordingHandle id, OOIFNumber saveDays);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetSaveEpisodes(RecordingHandle id, OOIFNumber *retval);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingSetSaveEpisodes(RecordingHandle id, OOIFNumber saveEpisodes);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetBlocked(RecordingHandle id, OOIFBoolean *retval);
#endif /* OIPF */

/**
   Gets OIPF property parentalRatings as defined in DAE 7.10.5.2.
   \param id Recording instance identifier
   \param count Number of handles in the returned array
   \param handles Use this to return the parental rating handles
   array. The OOIF plugin will take over ownership of the array and
   will later call free() to de-allocate the memory
   \return OOIF return code
 */
OOIFReturnCode native_RecordingGetParentalRatings(RecordingHandle id, int *count, ParentalRatingHandle **handles);

#ifdef OIPF
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetShowType(RecordingHandle id, OOIFNumber *retval);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetSubtitles(RecordingHandle id, OOIFBoolean *retval);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetSubtitleLanguages(RecordingHandle id, int *count, char ***languages);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetIsHD(RecordingHandle id, OOIFBoolean *retval);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetIsWidescreen(RecordingHandle id, OOIFBoolean *retval);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetAudioType(RecordingHandle id, OOIFNumber *retval);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetIsMultilingual(RecordingHandle id, OOIFBoolean *retval);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetAudioLanguages(RecordingHandle id, int *count, char ***languages);
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetGenres(RecordingHandle id, int *count, char ***genres);
// humax define
#if defined(HMX_WEBUI)
OOIFReturnCode native_RecordingGetLastPlayPosition(RecordingHandle identifier, OOIFNumber *retval);
#endif // HMX_WEBUI
#endif /* OIPF */

/**
   Gets OIPF property recordingStartTime as defined in DAE 7.10.5.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
 */
OOIFReturnCode native_RecordingGetRecordingStartTime(RecordingHandle id, OOIFNumber *retval);

/**
   Gets OIPF property recordingDuration as defined in DAE 7.10.5.2.
   \param id Recording instance identifier
   \param retval Retval as defined in DAE 7.10.5.2
   \return OOIF return code
 */
OOIFReturnCode native_RecordingGetRecordingDuration(RecordingHandle id, OOIFNumber *retval);

#ifdef OIPF
/** DAE 7.10.5.2 */
	OOIFReturnCode native_RecordingGetBookmarks(RecordingHandle id, int *count, BookmarkHandle **handles);
/* FIXME: This should really take a RecordingHandle */
/** DAE 7.10.5.2 */
OOIFReturnCode native_RecordingGetLocked(RecordingHandle id, OOIFBoolean *retval);
#if defined(HMX_WEBUI)
OOIFReturnCode native_RecordingGetIsManualLocked(RecordingHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_RecordingSetIsManualLocked(RecordingHandle identifier, OOIFBoolean isManualLocked);
OOIFReturnCode native_RecordingGetIsScrambled(RecordingHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_RecordingGetIsEncrypted(RecordingHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_RecordingGetCopyControl(RecordingHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_RecordingGetCasIDs(RecordingHandle identifier, int *count, OOIFNumber **casIDs);
OOIFReturnCode native_RecordingGetExpiryDate(RecordingHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_RecordingGetPausedPoints(RecordingHandle identifier, int *count, BookmarkHandle **handles);

#endif /* defined(HMX_WEBUI) */

/** DAE 7.10.9.2 */
OOIFReturnCode native_RecordingAddBookmark(RecordingHandle recording, OOIFNumber time, const char *name, BookmarkHandle *retval);
/** DAE 7.10.9.2 */
OOIFReturnCode native_RecordingRemoveBookmark(RecordingHandle recording, BookmarkHandle bookmark);

// part of the interface inherited from the ScheduledRecording class
/**
   Gets OIPF property startPadding as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetStartPadding(RecordingHandle id, OOIFNumber *retval);

/**
   Sets OIPF property startPadding as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param startPadding As defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingSetStartPadding(RecordingHandle id, OOIFNumber startPadding);

/**
   Gets OIPF property endPadding as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetEndPadding(RecordingHandle id, OOIFNumber *retval);

/**
   Sets OIPF property endPadding as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param endPadding As defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingSetEndPadding(RecordingHandle id, OOIFNumber endPadding);

/**
   Gets OIPF property repeatDays as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetRepeatDays(RecordingHandle id, OOIFNumber *retval);

/**
   Gets OIPF property name as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetName(RecordingHandle id, const char **retval);

/**
   Gets OIPF property longName as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetLongName(RecordingHandle id, const char **retval);

/**
   Gets OIPF property description as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetDescription(RecordingHandle id, const char **retval);

/**
   Gets OIPF property longDescription as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetLongDescription(RecordingHandle id, const char **retval);

/**
   Gets OIPF property startTime as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetStartTime(RecordingHandle id, OOIFNumber *retval);

/**
   Gets OIPF property duration as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetDuration(RecordingHandle id, OOIFNumber *retval);

/**
   Gets OIPF property channel as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetChannel(RecordingHandle id, ChannelHandle *retval);

#endif /* OIPF */

#if defined OIPF || defined OPT_HBBTV_SCHED_REC_SERIES

#endif /* OOIF || OPT_HBBTV_SCHED_REC_SERIES */

#if defined(HMX_WEBUI)
/**
   Gets OIPF property isScheduledAsSeries as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetThumbnail(RecordingHandle identifier, const char **retval);
OOIFReturnCode native_RecordingGetIsScheduledAsSeries(RecordingHandle id, OOIFBoolean *retval);
OOIFReturnCode native_RecordingGetSeriesID(ScheduledRecordingHandle identifier, const char **retval);
OOIFReturnCode native_RecordingGetField(RecordingHandle id, const char *fieldId, const char **retval);
OOIFReturnCode native_RecordingGetGroupCRIDs(RecordingHandle id, int *count,char ***crids);
OOIFReturnCode native_RecordingGetProgrammeCRID(ProgrammeHandle id,const char **retval);
OOIFReturnCode native_RecordingGetSeriesTitle(ProgrammeHandle id,const char **retval);
OOIFReturnCode native_RecordingGetSubtitleTypes(RecordingHandle id, int *count, char ***types);
OOIFReturnCode native_RecordingGetDoNotDeleteSetTime(RecordingHandle id, OOIFNumber *retval);
OOIFReturnCode native_RecordingStartSatRecord(void);
#endif

/**
   Gets OIPF property programmeId as defined in DAE 7.10.2.2.
   \param id Scheduled Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetProgrammeID(RecordingHandle id, const char **retval);

/**
   Gets OIPF property programmeIDType as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetProgrammeIDType(RecordingHandle id, ScheduledRecordingProgrammeIDType *retval);

#if defined OIPF || defined OPT_HBBTV_SCHED_REC_SERIES
/**
   Gets OIPF property episode as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetEpisode(RecordingHandle id, OOIFNumber *retval);

/**
   Gets OIPF property totalEpisodes as defined in DAE 7.10.2.2.
   \param id Recording instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetTotalEpisodes(RecordingHandle id, OOIFNumber *retval);
#ifdef HMX_WEBUI
OOIFReturnCode native_RecordingGetSeason(RecordingHandle id, OOIFNumber *retval);
OOIFReturnCode native_RecordingSetName(RecordingHandle id, const char *name, OOIFBoolean *retval);
#endif
#endif /* OOIF || OPT_HBBTV_SCHED_REC_SERIES */

/**
   Gets OIPF property parentalRating as defined in DAE 7.10.2.2.
   Note: This property was renamed to parentalRatings in release 2.
   \param id Recording instance identifier
   \param count Number of handles in the returned array
   \param handles Use this to return the parental rating handles
   array. The OOIF plugin will take over ownership of the array and
   will later call free() to de-allocate the memory
   \return Number of handles in the returned array
   \return OOIF return code
*/
OOIFReturnCode native_RecordingGetParentalRating(RecordingHandle id, int *count, ParentalRatingHandle **handles);

#ifdef JLABS
OOIFReturnCode native_RecordingGetRecMode(RecordingHandle id, OOIFNumber *retval);
OOIFReturnCode native_RecordingGetFromDap(RecordingHandle id, OOIFBoolean *retval);
OOIFReturnCode native_RecordingGetFactoryType(RecordingHandle id, OOIFNumber *retval);
OOIFReturnCode native_RecordingGetDigitalCopyControl(RecordingHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_RecordingGetAnalogCopyControl(RecordingHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_RecordingGetDigitalCopyCount(RecordingHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_RecordingGetTranscoded(RecordingHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_RecordingGetWatched(RecordingHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_RecordingGetDiscInfo(HDiscInfoHandle *handle);
OOIFReturnCode native_RecordingGetStorages(int *count, HDiscInfoHandle **handles);
OOIFReturnCode native_RecordingGetThumbnailURL(RecordingHandle identifier, const char **retval);
OOIFReturnCode native_RecordingGetGroupIndex(RecordingHandle id, OOIFNumber *retval);
OOIFReturnCode native_RecordingGetUserID(RecordingHandle id, const char **retval);
OOIFReturnCode native_RecordingGetDeviceHandle(RecordingHandle id, const char **retval);
OOIFReturnCode native_RecordingGetDestinationId(RecordingHandle id, const char **retval);
OOIFReturnCode native_RecordingGetChapters(RecordingHandle identifier, int *count, BookmarkHandle **handles);
#endif // JLABS

#if (defined(JLABS) || defined(HMX_WEBUI))
OOIFReturnCode native_RecordingGetStorageId(RecordingHandle id, const char **retval);
#endif //(defined(JLABS) || defined(HMX_WEBUI))

/*@}*/
/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_RECORDING */
#endif /* HBBTV_PVR || OIPF */
