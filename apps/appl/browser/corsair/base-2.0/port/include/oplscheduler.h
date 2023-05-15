/**************************************************************************************/
/**
 * @file oplscheduler.h
 *
 *
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLSCHEDULER_H__
#define __OPLSCHEDULER_H__

#include	"opltype.h"

#include	"oplscheduledprogramme.h"
#include	"oplrecording.h"

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

#if defined(JLABS) || defined(CONFIG_OP_JAPAN)
#define	EARLYSTARTTIME		1	 // 일본향은 영상의 앞부분이 짤리면 안되기 때문에 1초 정도 먼저 시작한다.
#else
#define	EARLYSTARTTIME		0
#endif

typedef enum
{
	  eOPL_SCHEDULER_EVENT_UNREALIZED				= 0
	, eOPL_SCHEDULER_EVENT_SCHEDULED				= 1
	, eOPL_SCHEDULER_EVENT_PRESTART					= 2
	, eOPL_SCHEDULER_EVENT_ACQUIRING_RESOURCES		= 3
	, eOPL_SCHEDULER_EVENT_STARTED					= 4
	, eOPL_SCHEDULER_EVENT_UPDATED					= 5
	, eOPL_SCHEDULER_EVENT_COMPLETED				= 6
	, eOPL_SCHEDULER_EVENT_PARTIALY_COMPLETED		= 7
	, eOPL_SCHEDULER_EVENT_DELETED					= 8
	, eOPL_SCHEDULER_EVENT_RECLIST_COMPLETED		= 9
	, eOPL_SCHEDULER_EVENT_START_ERROR				= 10
	, eOPL_SCHEDULER_EVENT_START_CONFLICT			= 11
	, eOPL_SCHEDULER_EVENT_LOAD						= 12
	, eOPL_SCHEDULER_EVENT_ERROR					= 13
} OPL_SchedulerState;

typedef enum
{
	  eOPL_SCHEDULER_ERROR_NONE						= 0
	, eOPL_SCHEDULER_ERROR_RESOURCE_LIMITATION		= 1
	, eOPL_SCHEDULER_ERROR_INSUFFICIENT_STORAGE		= 2
	, eOPL_SCHEDULER_ERROR_TUNER_CONFLICT			= 3
	, eOPL_SCHEDULER_ERROR_DRM_RESTRICTION			= 4
	, eOPL_SCHEDULER_ERROR_HAVE_ALTERNATIVE			= 5
	, eOPL_SCHEDULER_ERROR_RFOTA					= 6
	, eOPL_SCHEDULER_ERROR_NO_AV					= 7
	, eOPL_SCHEDULER_ERROR_POWER_OFF				= 8
	, eOPL_SCHEDULER_ERROR_UNKNOWN					= 9
	, eOPL_SCHEDULER_ERROR_CAS						= 10
	, eOPL_SCHEDULER_ERROR_HIGHER_PRIORITY			= 11
	, eOPL_SCHEDULER_ERROR_TRACKING_FAIL			= 12
	, eOPL_SCHEDULER_ERROR_OFF_AIR					= 13
	, eOPL_SCHEDULER_ERROR_SATIP_FAILED				= 14
    , eOPL_SCHEDULER_ERROR_MAX_RECORD_LIST          = 15
} OPL_SchedulerError;

typedef enum {
	eOPL_SCHEDULER_TRY_TYPE_NORMAL = 0
	,eOPL_SCHEDULER_TRY_TYPE_FORCE = 1
	,eOPL_SCHEDULER_TRY_TYPE_CRID = 2
} OPL_ReservationTryType;

typedef OPL_BOOL	(* OPL_Scheduler_EventListener_t) (
					  int state		///< @see enum OPL_SchedulerState
					, int error		///< @see enum OPL_SchedulerError
					, OPL_ScheduledItem_t *item
					, OPL_Collection_t    *conflict
					, OPL_Collection_t    *alternative
					, OPL_Collection_t    *altevent
				);

typedef OPL_BOOL	(* OPL_Scheduler_HNReservationFinishedEventListener_t) (
					OPL_ScheduledItem_t *item
					, int error
				);

OPL_HANDLE	OPL_Scheduler_GetInstance (void);
void		OPL_Scheduler_AddEventListener (OPL_HANDLE self, OPL_Scheduler_EventListener_t listener);
OPL_ScheduledItem_t *
			OPL_Scheduler_MakeSchedule (OPL_HANDLE self, OPL_ScheduledItem_t *item, OPL_ReservationTryType type);
OPL_ScheduledItem_t *
			OPL_Scheduler_Record (OPL_HANDLE self, OPL_ProgrammeHandle programme);
OPL_ScheduledItem_t *
			OPL_Scheduler_SeriesRecord (OPL_HANDLE self, OPL_ProgrammeHandle programme);
OPL_ScheduledItem_t *
			OPL_Scheduler_RecordAt (
						  OPL_HANDLE self
						, unsigned long startTime
						, unsigned long duration
						, int repeatDays
						, const char *channelID
						, int volume
					);

OPL_ScheduledItem_t *
			OPL_Scheduler_Watch (OPL_HANDLE self, OPL_ProgrammeHandle programme);
OPL_ScheduledItem_t *
			OPL_Scheduler_WatchAt (
						  OPL_HANDLE self
						, unsigned long startTime
						, unsigned long duration
						, int repeatDays
						, const char *channelID
					);
OPL_BOOL	OPL_Scheduler_ActivationTimeSet (OPL_HANDLE self, unsigned long startTime, unsigned long duration, BPL_BOOL bEnable);

OPL_BOOL	OPL_Scheduler_Wakeup (OPL_HANDLE self, unsigned long time, int repeatDays, const char *channelID, int volume);
OPL_BOOL	OPL_Scheduler_CancelWakeup (OPL_HANDLE self);
OPL_BOOL	OPL_Scheduler_GetWakeupParam (OPL_HANDLE self, unsigned long *time, int *repeatDays, const char **ccid, int *volume);
OPL_BOOL	OPL_Scheduler_TVTVEPG_Download (OPL_HANDLE self, unsigned long time, HBOOL isTvtvStandbyEnable);
OPL_BOOL	OPL_Scheduler_SetTVTVEPGDownload_StandbyEnable (HBOOL isTvtvStandbyEnable);
OPL_BOOL	OPL_Scheduler_CancelTVTVEPG_Download (OPL_HANDLE self);
OPL_BOOL	OPL_Scheduler_GetTVTVEPG_DownloadParam (OPL_HANDLE self, unsigned long *time, HBOOL *isTvtvStandbyEnable);


void		OPL_Scheduler_Remove (OPL_HANDLE self, OPL_ScheduledItem_t *item);
void		OPL_Scheduler_RemoveFirstItem (OPL_HANDLE self, OPL_ScheduledItem_t *item);
void		OPL_Scheduler_RemoveWholeSeries (OPL_HANDLE self, OPL_ScheduledItem_t *item);
void		OPL_Scheduler_Confirm (OPL_HANDLE self, OPL_ScheduledItem_t *item, int type, int check);
void		OPL_Scheduler_Stop (OPL_HANDLE self, OPL_Recording_t *recording);

void		OPL_Scheduler_Refresh (OPL_HANDLE self);
OPL_BOOL	OPL_Scheduler_Update (OPL_HANDLE self, OPL_ScheduledItem_t *item);
OPL_BOOL	OPL_Scheduler_UpdateProperty(OPL_HANDLE self, OPL_ScheduledItem_t *item);


int			OPL_Scheduler_CountScheduledProgrammes (OPL_HANDLE self);
OPL_ScheduledItem_t *
			OPL_Scheduler_GetScheduledProgrammeAt (OPL_HANDLE self, int index);


OPL_HANDLE	OPL_Scheduler_ScheduleClone (OPL_HANDLE self);
void		OPL_Scheduler_ScheduleDelete (OPL_HANDLE self);

OPL_ScheduledItem_t *
			OPL_Scheduler_FindScheduleExceptRecording(OPL_HANDLE self, const char *programmeID);
OPL_ScheduledItem_t *
			OPL_Scheduler_FindSchedule (OPL_HANDLE self, const char *programmeID);
OPL_Collection_t *
			OPL_Scheduler_FindSchedules (OPL_HANDLE self, const char *programmeID);

OPL_Collection_t *
			OPL_Scheduler_GetCurrentRecordings(OPL_HANDLE self);
unsigned int	OPL_Scheduler_GetSystemTime();
HBOOL		OPL_Scheduler_SetName (OPL_ScheduledItem_t *self, unsigned char *name);
OPL_BOOL	OPL_Scheduler_SetSeriesManualLock (OPL_HANDLE self, const char *seriesID, OPL_BOOL bLock);

OPL_Collection_t	*OPL_Scheduler_GetGroupCRIDs(OPL_ScheduledItem_t *self);
const char *		OPL_Scheduler_GetProgrammeCRID(OPL_ScheduledItem_t *self);
void 				OPL_Scheduler_StartSatRecord(void);

OPL_BOOL	OPL_Scheduler_SetDefaultDMS(const char *device, const char *destinationId);
OPL_BOOL	OPL_Scheduler_GetDefaultDMSDeviceHandle(unsigned char **dms);
OPL_BOOL	OPL_Scheduler_GetDefaultDMSRecordDestinationId(unsigned char **dms);
void			OPL_Scheduler_AddHNEventListener (OPL_HANDLE self, OPL_Scheduler_HNReservationFinishedEventListener_t hnlistener);


#ifdef __cplusplus
};
#endif	//__cplusplus

#endif	//__OPLSCHEDULER_H__

