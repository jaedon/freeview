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
   Opaque reference to a ScheduledRecording or Recording
   instance in the native platform as defined in DAE 7.10.2 and
   DAE 7.10.5. One pointer is used for both interfaces, because
   ScheduledRecording becomes Recording if the time set
   for planned record has passed (the recording has started/finished)
*/
typedef void* PVRHandle;

/*------------------------------------------
  Recording Scheduler
  ------------------------------------------*/
/** @name RecSched Recording Scheduler
	This is the native interface for the oipfRecordingScheduler APIs as
	defined in DAE 7.10.1.
	@{*/

/**
   Called when a new oipfRecordingScheduler instance is created. The return
   value may be used for capability and security checks.
   \param window The identifier of the window the create call originates from
   \param host The host URI of the application the create call originates from
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_RecordingSchedulerOnCreate(long int window, const char *host);

#ifdef OIPF
/**
   oipfRecordingScheduler event types.
*/
typedef enum
{
    /** This event corresponds to the onPVREvent event defined in DAE 7.10.4.1 */
    RECORDING_SCHEDULER_PVR_EVENT = 0
} RecordingSchedulerEvent;

/**
   Union used to pass event context info in an event callback. What
   attribute to use is decided by the event type defined by the
   #RecordingSchedulerEvent.
*/
typedef union
{
	/** used for RECORDING_SCHEDULER_PVR_EVENT as described in DAE 7.10.4.1.*/
	struct
	{
		OOIFNumber state;
		PVRHandle id;
	} pvr_event_data;
} RecordingSchedulerEventInfo;

/**
   The callback function that the native platform should call to
   dispatch an oipfRecordingScheduler related event. The OOIF plugin will call the
   registered listener on the oipfRecordingScheduler instances
   \param event The event type
   \param info Event context data
   @see native_RecordingSchedulerSetListener
 */
typedef void (recordingSchedulerListener)(RecordingSchedulerEvent event, RecordingSchedulerEventInfo info);

/**
   Registers new listener on oipfRecordingScheduler related events.
   \param listener The listener
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_RecordingSchedulerSetListener(recordingSchedulerListener listener);
#endif /*OIPF*/

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
OOIFReturnCode native_RecordingSchedulerRecord(ProgrammeHandle programme, long int window_identifier, const char* host, PVRHandle *retval);

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
OOIFReturnCode native_RecordingSchedulerRecordAt(OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, const char *host, PVRHandle *retval);

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
OOIFReturnCode native_RecordingSchedulerGetScheduledRecordings(const char *host, int *count, PVRHandle **handles);

/**
   This function corresponds to the OIPF function remove() as
   defined in DAE 7.10.1.1
   \param id PVR instance identifier
   \return OOIF return code
*/
OOIFReturnCode native_RecordingSchedulerRemove(PVRHandle id);

/**
   Gets OIPF property recordings as defined in DAE 7.10.4.1.
   \param host Domain of the service that is issuing the request
   \param count Number of recording handles in the returned array
   \param handles Use this to return the recording handles array. The
   OOIF plugin will take over ownership of the array and will later
   call free() to de-allocate the memory
   \return OOIF return code
*/
OOIFReturnCode native_RecordingSchedulerGetRecordings(const char *host, int *count, PVRHandle **handles);

#ifdef OIPF
/**
   This function corresponds to the OIPF function getRecording() as defined in DAE 7.10.4.2
   \param id As defined in DAE 7.10.4.2
   \param retval Retval as defined in DAE 7.10.4.2
   \return OOIF return code
*/
OOIFReturnCode native_RecordingSchedulerGetRecording(const char* id, PVRHandle* retval);

/**
   This function corresponds to the OIPF function stop() as defined in DAE 7.10.4.2
   \param id As defined in DAE 7.10.4.2
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_RecordingSchedulerStop(PVRHandle id);

/**
   This function corresponds to the OIPF function refresh() as defined in DAE 7.10.4.2
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_RecordingSchedulerRefresh();
#endif /* OIPF */
/* @} */

/*------------------------------------------
  ScheduledRecording and Recording APIs
  ------------------------------------------*/
/** @name PVR

    The ScheduledRecording and Recording classes as defined in DAE
    7.10.2 and DAE 7.10.5.

	If a API specific to the Recording class is called with a PVR
    instance identifier that points to a ScheduledRecording object,
    the integration is expected to return the
    OOIF_RETURN_VALUE_UNDEFINED status code. This functionality allows
    the integration to transparently transform a ScheduledRecording
    instance to a Recoding instance during the object's lifetime in
    OOIF.
	@{*/

#ifndef HBBTV_1_2_1
/**
  Constants specified in DAE 7.10.2.1
*/
typedef enum {
#ifdef OIPF
	SCHEDULED_RECORDING_ID_TVA_CRID = 0 ,
	SCHEDULED_RECORDING_ID_TVA_GROUP_CRID = 2 ,
#endif /* OIPF */
	SCHEDULED_RECORDING_ID_DVB_EVENT = 1
} ScheduledRecordingProgrammeIDType;
#endif /* HBBTV_1_2_1 */

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
	SCHEDULED_RECORDING_SATURDAY = 0x40
} ScheduledRecordingDay;

#ifdef OIPF
/**
   Constans defined in DAE 7.10.5.1, their values
   are up to implementators, and each must be unique.
*/
typedef struct
{
	OOIFNumber RECORDING_UNREALIZED;
	OOIFNumber RECORDING_SCHEDULED;
	OOIFNumber RECORDING_DEL_SCHEDULED;
	OOIFNumber RECORDING_REC_PRESTART;
	OOIFNumber RECORDING_REC_ACQUIRING_RESOURCES;
	OOIFNumber RECORDING_REC_STARTED;
	OOIFNumber RECORDING_REC_UPDATED;
	OOIFNumber RECORDING_REC_COMPLETED;
	OOIFNumber RECORDING_REC_PARTIALLY_COMPLETED;
	OOIFNumber RECORDING_TS_ACQUIRING_RESOURCES;
	OOIFNumber RECORDING_TS_STARTED;
	OOIFNumber RECORDING_ERROR;

	OOIFNumber ERROR_REC_RESOURCE_LIMITATION;
	OOIFNumber ERROR_INSUFFICIENT_STORAGE;
	OOIFNumber ERROR_TUNER_CONFLICT;
	OOIFNumber ERROR_REC_DRM_RESTRICTION;
	OOIFNumber ERROR_REC_UNKNOWN;
	OOIFNumber ERROR_TS_RESOURCE_LIMITATION;
	OOIFNumber ERROR_TS_DRM_RESTRICTION;
	OOIFNumber ERROR_TS_UNKNOWN;
} PVRData;

/**
   Populate the PVRData struct with the appropriate values as
   defined in DAE 7.10.5.1 The OOIF plugin will handle all getter calls on
   these parameter. They are valid for all instances of the PVR interface
   during plugin runtime.
   \param data property struct to populate
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_PVRInitialize(PVRData* data);
#endif // OIPF

/**
   Called when the PVR instance is destroyed in the OOIF plugin.
   \param id PVR instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_PVRReleaseHandle(PVRHandle id);

/**
   Gets OIPF property startPadding as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetStartPadding(PVRHandle id, OOIFNumber *retval);

/**
   Sets OIPF property startPadding as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param startPadding As defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRSetStartPadding(PVRHandle id, OOIFNumber startPadding);

/**
   Gets OIPF property endPadding as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetEndPadding(PVRHandle id, OOIFNumber *retval);

/**
   Sets OIPF property endPadding as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param endPadding As defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRSetEndPadding(PVRHandle id, OOIFNumber endPadding);

/**
   Gets OIPF property name as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetName(PVRHandle id, const char **retval);

#ifndef HBBTV_1_2_1
/**
   Gets OIPF property repeatDays as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetRepeatDays(PVRHandle id, OOIFNumber *retval);

/**
   Gets OIPF property longName as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetLongName(PVRHandle id, const char **retval);

/**
   Gets OIPF property longDescription as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetLongDescription(PVRHandle id, const char **retval);
#endif /* !HBBTV_1_2_1 */

/**
   Gets OIPF property description as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetDescription(PVRHandle id, const char **retval);

/**
   Gets OIPF property startTime as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetStartTime(PVRHandle id, OOIFNumber *retval);

/**
   Gets OIPF property duration as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetDuration(PVRHandle id, OOIFNumber *retval);

/**
   Gets OIPF property channel as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetChannel(PVRHandle id, ChannelHandle *retval);

#if defined (HBBTV) || !defined(HBBTV_1_2_1)
/**
   Gets OIPF property isSeries as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetIsSeries(PVRHandle id, OOIFBoolean *retval);
#endif /* HBBTV && !HBBTV_1_2_1 */

#ifndef HBBTV_1_2_1
/**
   Gets OIPF property programmeId as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetProgrammeID(PVRHandle id, const char **retval);

/**
   Gets OIPF property programmeIDType as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetProgrammeIDType(PVRHandle id, ScheduledRecordingProgrammeIDType *retval);

/**
   Gets OIPF property episode as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetEpisode(PVRHandle id, OOIFNumber *retval);

/**
   Gets OIPF property totalEpisodes as defined in DAE 7.10.2.2.
   \param id PVR instance identifier
   \param retval Retval as defined in 7.10.2.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetTotalEpisodes(PVRHandle id, OOIFNumber *retval);
#endif /* !HBBTV_1_2_1 */

/**
   Gets OIPF property parentalRatings as defined in DAE 7.10.2.2 and DAE1.1 7.10.5.1.
   \param id PVR instance identifier
   \param count Number of handles in the returned array
   \param handles Use this to return the parental rating handles
   array. The OOIF plugin will take over ownership of the array and
   will later call free() to de-allocate the memory
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetParentalRatings(PVRHandle id, int *count, ParentalRatingHandle **handles);

/**
   Gets OIPF property state as defined in DAE1.1 7.10.5.1 / DAE2.1 7.10.5.2 / DAE1.2 7.10.2
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in DAE1.1 7.10.5.1 / DAE2.1 7.10.5.2 / DAE1.2 7.10.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetState(PVRHandle id, OOIFNumber *retval);

/**
   Gets OIPF property id as defined in DAE1.1 7.10.5
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in DAE1.1 7.10.5
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetID(PVRHandle id, const char **retval);

#ifdef OIPF
/**
   Gets OIPF property error as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetError(PVRHandle id, OOIFNumber *retval);

/**
   Gets OIPF property isManual as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetIsManual(PVRHandle id, OOIFBoolean *retval);

/**
   Gets OIPF property doNotDelete as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetDoNotDelete(PVRHandle id, OOIFBoolean *retval);

/**
   Sets OIPF property doNotDelete as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param doNotDelete As defined in DAE 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRSetDoNotDelete(PVRHandle id, OOIFBoolean doNotDelete);

/**
   Gets OIPF property saveDays as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetSaveDays(PVRHandle id, OOIFNumber *retval);

/**
   Sets OIPF property saveDays as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param saveDays As defined in DAE 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRSetSaveDays(PVRHandle id, OOIFNumber saveDays);

/**
   Gets OIPF property saveEpisodes as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetSaveEpisodes(PVRHandle id, OOIFNumber *retval);

/**
   Sets OIPF property saveEpisodes as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param saveEpisodes as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRSetSaveEpisodes(PVRHandle id, OOIFNumber saveEpisodes);

/**
   Gets OIPF property blocked as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetBlocked(PVRHandle id, OOIFBoolean *retval);

/**
   Gets OIPF property showType as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetShowType(PVRHandle id, OOIFNumber *retval);

/**
   Gets OIPF property subtitles as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetSubtitles(PVRHandle id, OOIFBoolean *retval);

/**
   Gets OIPF property subtitleLanguages as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param count Number of items in the returned array
   \param subtitleLanguages Use this to return the array.
   The OOIF framework takes ownership of the returned array
   and the strings it contains, and will free the memory when appropriate.
   HLIB_STD_MemAlloc(), HLIB_STD_StrDup(), or similar should be used in the native code to allocate
   memory for them.
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetSubtitleLanguages(PVRHandle id, int *count, char ***subtitleLanguages);

/**
   Gets OIPF property isHD as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetIsHD(PVRHandle id, OOIFBoolean *retval);

/**
   Gets OIPF property isWidescreen as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetIsWidescreen(PVRHandle id, OOIFBoolean *retval);

/**
   Gets OIPF property audioType as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetAudioType(PVRHandle id, OOIFNumber *retval);

/**
   Gets OIPF property isMultilingual as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetIsMultilingual(PVRHandle id, OOIFBoolean *retval);

/**
   Gets OIPF property audioLanguages as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param count Number of items in the returned array
   \param audioLanguages Use this to return the array.
   The OOIF framework takes ownership of the returned array
   and the strings it contains, and will free the memory when appropriate.
   HLIB_STD_MemAlloc(), HLIB_STD_StrDup(), or similar should be used in the native code to allocate
   memory for them.
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetAudioLanguages(PVRHandle id, int *count, char ***audioLanguages);

/**
   Gets OIPF property genres as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param count Number of items in the returned array
   \param genres Use this to return the array.
   The OOIF framework takes ownership of the returned array
   and the strings it contains, and will free the memory when appropriate.
   HLIB_STD_MemAlloc(), HLIB_STD_StrDup(), or similar should be used in the native code to allocate
   memory for them.
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetGenres(PVRHandle id, int *count, char ***genres);
#endif /* OIPF */

/**
   Gets OIPF property recordingStartTime as defined in DAE 7.10.5.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in DAE 7.10.5.
   \return OOIF return code
 */
OOIFReturnCode native_PVRGetRecordingStartTime(PVRHandle id, OOIFNumber *retval);

/**
   Gets OIPF property recordingDuration as defined in DAE 7.10.5.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in DAE 7.10.5.
   \return OOIF return code
 */
OOIFReturnCode native_PVRGetRecordingDuration(PVRHandle id, OOIFNumber *retval);

#ifdef OIPF
/**
   Gets OIPF property bookmarks as defined in DAE 7.10.5.2.
   \param id PVR instance identifier
   \param count Number of handles in the returned array
   \param handles Use this to return the bookmarks handles
   array. The OOIF plugin will take over ownership of the array and
   will later call free() to de-allocate the memory
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetBookmarks(PVRHandle id, int *count, BookmarkHandle **handles);

/**
   Gets OIPF property locked as defined in DAE 7.10.5.2.
   \param id PVR (Recording) instance identifier
   \param retval Retval as defined in DAE 7.10.5.2
   \return OOIF return code
*/
OOIFReturnCode native_PVRGetLocked(PVRHandle id, OOIFBoolean *retval);
#endif /* OIPF */
/*@}*/

#ifdef OIPF
/*------------------------------------------
  BookmarkCollection
  ------------------------------------------*/
/** @name BookmarkCollection Bookmark Collection
	This is the native interface for the BookmarkCollection APIs as
	defined in DAE 7.10.9
	@{*/

/**
   This function corresponds to the OIPF function addBookmark() as
   defined in DAE 7.10.9.1.
   \param id PVR (Recording) instance identifier
   \param time as defined in DAE 7.10.9.1
   \param name as defined in DAE 7.10.9.1
   \param retval Retval as defined in DAE 7.10.9.1
   \return OOIF return code
*/
OOIFReturnCode native_PVRAddBookmark(PVRHandle id, OOIFNumber time, const char *name, BookmarkHandle *retval);

/**
   This function corresponds to the OIPF function removeBookmark() as
   defined in DAE 7.10.9.1.
   \param bookmark as defined in DAE 7.10.9.1
   \return OOIF return code
*/
OOIFReturnCode native_PVRRemoveBookmark(PVRHandle id, BookmarkHandle bookmark);

/*@}*/
#endif /* OIPF */

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_RECORDING */
#endif /* HBBTV_PVR || OIPF */
