// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_PVR) || defined(OIPF)

#include "NativeRecording.h"

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opltype.h"
#include "oplscheduler.h"
#include "oplrecorditem.h"
/*------------------------------------------
  defines
  ------------------------------------------*/

/*------------------------------------------
  RecordingScheduler
  ------------------------------------------*/
OOIFReturnCode native_RecordingSchedulerOnCreate(long int window, const char *host)
{
	ENTRY;
    OOIF_LOG_DEBUG("recording Scheduler On Create : %d (host: %s)", window, host);

//	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();	// for initialization~!!

#ifdef OIPF
	OPL_Scheduler_AddEventListener(scheduler, scheduler_EventListener);
#endif

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingSchedulerRecord(ProgrammeHandle programme, long int window_identifier, const char* host, PVRHandle *retval)
{
	ENTRY;
	OOIF_LOG_DEBUG("Creating scheduled recording (from Programme): %d (host: %s)", window_identifier, host);

	OPL_ScheduledItem_t *sched = OPL_Scheduler_Record(
										OPL_Scheduler_GetInstance(),
										(OPL_ProgrammeHandle)programme
									);

	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (PVRHandle)OPL_ScheduledItem_Clone(sched);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingSchedulerRecordAt(OOIFNumber startTime, OOIFNumber duration, OOIFNumber repeatDays, const char *channelID, const char *host, PVRHandle *retval)
{
	ENTRY;
	OPL_ScheduledItem_t *sched;

	sched = OPL_Scheduler_RecordAt(OPL_Scheduler_GetInstance(),
					(unsigned long)startTime,
					(unsigned long)duration,
					(int)repeatDays,
					channelID,
					(int)0
				);

	if (sched == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (PVRHandle)OPL_ScheduledItem_Clone(sched);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_RecordingSchedulerGetScheduledRecordings(const char *host, int *count, PVRHandle **handles)
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
		return OOIF_RETURN_OK;
	}

	PVRHandle *array = (PVRHandle *) HLIB_STD_MemAlloc (sizeof(PVRHandle) * c);
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
				array[cnt] = (PVRHandle)OPL_Recording_ScheduledToRecording_Clone((OPL_ScheduledItem_t*)item);
			}
			else
			{
				array[cnt] = (PVRHandle)OPL_ScheduledItem_Clone(item);
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


OOIFReturnCode native_RecordingSchedulerRemove(PVRHandle id)
{
	ENTRY;

	OPL_Scheduler_Remove(OPL_Scheduler_GetInstance(), (OPL_ScheduledItem_t *)id);

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_RecordingSchedulerGetRecordings(const char *host, int *count, PVRHandle **handles)
{
	ENTRY;
	OPL_HANDLE scheduler = OPL_Scheduler_GetInstance();

	int	rc = OPL_RecordItem_CountRecordings();
	OOIF_LOG_DEBUG("==========CountRecordings %d\n", rc);
	int sc = OPL_Scheduler_CountScheduledProgrammes(scheduler);
	OOIF_LOG_DEBUG("==========CountScheduledProgrammes %d\n", sc);
	int tc = rc + sc;
	if (tc == 0)
	{
		*count  = 0;
		handles = NULL;
		return OOIF_RETURN_OK;
	}

	PVRHandle *array = (PVRHandle *) HLIB_STD_MemAlloc (sizeof(PVRHandle) * tc);
	if (array == NULL)
	{
		return OOIF_RETURN_OOM_ERROR;
	}


	OPL_Recording_t 	*rItem;
	OPL_Collection_t  	*collection;
	collection = OPL_RecordItem_GetRecordings();

	for (int i = 0 ; i < rc; i++)
	{
		rItem = (OPL_Recording_t*)OPL_Collection_ItemAt(collection,i);
		array[i] = (PVRHandle)rItem;
	}

	OPL_ScheduledItem_t *sItem;
	for (int i = 0 ; i < sc; i++)
	{
		sItem = OPL_Scheduler_GetScheduledProgrammeAt(scheduler, i);
		array[rc + i] = (PVRHandle)OPL_ScheduledItem_Clone(sItem);
	}

	*handles = array;
	*count	 = tc;

	
	if (collection != NULL)
	{
		OPL_Collection_Delete(collection);
	}

    return OOIF_RETURN_OK;
}

#ifdef OIPF

OOIFReturnCode native_RecordingSchedulerSetListener(recordingSchedulerListener listener)
{
	ENTRY;

	global_scheduleListener = listener;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingSchedulerGetRecording(const char* id, PVRHandle* retval)
{
	ENTRY;

	OPL_Recording_t *recording = OPL_Scheduler_GetRecordingByID(OPL_Scheduler_GetInstance(), id);

	if (recording == NULL)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*retval = (RecordingHandle)OPL_Recording_Clone(recording);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RecordingSchedulerStop(PVRHandle id)
{
	ENTRY;

	OPL_Scheduler_Stop(OPL_Scheduler_GetInstance(), (OPL_Recording_t *)identifier);

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_RecordingSchedulerRefresh()
{
	ENTRY;

	OPL_Scheduler_Refresh(OPL_Scheduler_GetInstance());
	return OOIF_RETURN_OK;
}
#endif // OIPF

/*------------------------------------------
  ScheduledRecording
  ------------------------------------------*/

#ifndef HBBTV_1_2_1
OOIFReturnCode native_PVRGetRepeatDays(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->repeatDays;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetLongName(PVRHandle identifier, const char **retval)
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

OOIFReturnCode native_PVRGetLongDescription(PVRHandle identifier, const char **retval)
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
#endif // HBBTV_1_2_1

OOIFReturnCode native_PVRGetStartPadding(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->startPadding;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRSetStartPadding(PVRHandle identifier, OOIFNumber startPadding)
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

OOIFReturnCode native_PVRGetEndPadding(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->endPadding;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRSetEndPadding(PVRHandle identifier, OOIFNumber endPadding)
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

OOIFReturnCode native_PVRGetName(PVRHandle identifier, const char **retval)
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

OOIFReturnCode native_PVRGetDescription(PVRHandle identifier, const char **retval)
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

OOIFReturnCode native_PVRGetStartTime(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->startTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetDuration(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->duration;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetChannel(PVRHandle identifier, ChannelHandle *retval)
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
	return OOIF_RETURN_OK;}

#if defined(HBBTV) && !defined(HBBTV_1_2_1)
OOIFReturnCode native_PVRGetIsSeries(PVRHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (item->isScheduledAsSeries) ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}
#endif // HBBTV && !HBBTV_1_2_1

#ifndef HBBTV_1_2_1
OOIFReturnCode native_PVRGetProgrammeID(PVRHandle identifier, const char **retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->programmeID == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = item->programmeID;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetProgrammeIDType(PVRHandle identifier, ScheduledRecordingProgrammeIDType *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	if (item->programmeID == NULL)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = (ScheduledRecordingProgrammeIDType)OPL_Programme_GetIDTypeFromProgrammeID(item->programmeID);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetEpisode(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->episode;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetTotalEpisodes(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t *)identifier;

	*retval = (OOIFNumber)item->totalEpisodes;

	return OOIF_RETURN_OK;
}
#endif // !HBBTV_1_2_1

/*------------------------------------------
  Recording
  ------------------------------------------*/

OOIFReturnCode native_PVRGetState(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->state;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetID(PVRHandle identifier, const char **retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	if (item->id == NULL)
		return OOIF_RETURN_VALUE_NULL;

	*retval = item->id;
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_PVRGetError(PVRHandle id, OOIFNumber *retval)
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

OOIFReturnCode native_PVRGetIsManual(PVRHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isManual ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetDoNotDelete(PVRHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->doNotDelete ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_PVRSetDoNotDelete(PVRHandle identifier, OOIFBoolean doNotDelete)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;
	OPL_BOOL		value = doNotDelete ? OPL_TRUE : OPL_FALSE;
	OPL_BOOL		backup;

	if (item->doNotDelete != value)
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

OOIFReturnCode native_PVRGetSaveDays(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->saveDays;
	return OOIF_RETURN_OK;

}

OOIFReturnCode native_PVRSetSaveDays(PVRHandle identifier, OOIFNumber saveDays)
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

OOIFReturnCode native_PVRGetSaveEpisodes(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->saveEpisodes;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRSetSaveEpisodes(PVRHandle identifier, OOIFNumber saveEpisodes)
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

OOIFReturnCode native_PVRGetBlocked(PVRHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->blocked ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetShowType(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->showType;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetSubtitles(PVRHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->subtitles ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetSubtitleLanguages(PVRHandle identifier, int *count, char ***subtitleLanguages)
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

OOIFReturnCode native_PVRGetIsHD(PVRHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isHD ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetIsWidescreen(PVRHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isWideScreen ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetAudioType(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->audioType;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetIsMultilingual(PVRHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = item->isMultilingual ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetAudioLanguages(PVRHandle identifier, int *count, char ***audioLanguages)
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

OOIFReturnCode native_PVRGetGenres(PVRHandle identifier, int *count, char ***genres)
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

OOIFReturnCode native_PVRGetRecordingStartTime(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->recordingStartTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRGetRecordingDuration(PVRHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->recordingDuration;

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_PVRGetBookmarks(PVRHandle id, int *count, BookmarkHandle **handles)
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

OOIFReturnCode native_PVRGetLocked(PVRHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OPL_Recording_t *item = (OPL_Recording_t *)identifier;

	*retval = (OOIFNumber)item->locked;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRAddBookmark(PVRHandle recording, OOIFNumber time, const char *name, BookmarkHandle *retval)
{
	ENTRY;
	OOIF_LOG_DEBUG("Adding bookmark: %f %s", time, name);

	*retval = OPL_Recording_Bookmark_Add(recording, time, name);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRRemoveBookmark(PVRHandle recording, BookmarkHandle bookmark)
{
    ENTRY;
	OOIF_LOG_DEBUG("Removing bookmark: %p", bookmark);

	OPL_Recording_Bookmark_Remove(recording, bookmark);
	return OOIF_RETURN_OK;
}
#endif // OIPF

/*------------------------------------------
  Shared Recording & Scheduled Recording
  ------------------------------------------*/

OOIFReturnCode native_PVRGetParentalRatings(PVRHandle identifier, int *count, ParentalRatingHandle **handles)
{
	int	value;

	OPL_ScheduledItem_t	*scheditem;
	OPL_ParentalRating_t	*parentalRating;
	ParentalRatingHandle	*myHandles = (ParentalRatingHandle*)HLIB_STD_MemAlloc(sizeof(ParentalRatingHandle));

	scheditem = (OPL_ScheduledItem_t*)identifier;
	value = OPL_ScheduledItem_GetParentalRating(scheditem);

	parentalRating = OPL_PCMAN_PRC_AddParentalRating(NULL, NULL, NULL, value, 0, NULL);
	myHandles[0] = (ParentalRatingHandle)parentalRating;

	*handles = myHandles;
	*count = 1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PVRReleaseHandle(PVRHandle id)
{
	OOIF_LOG_DEBUG("Releasing PVRHandle: %p", id);
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_PVRInitialize(PVRData* data)
{
	data->RECORDING_UNREALIZED = 0;
	data->RECORDING_SCHEDULED = 1;
	data->RECORDING_DEL_SCHEDULED = 2;
	data->RECORDING_REC_PRESTART = 3;
	data->RECORDING_REC_ACQUIRING_RESOURCES = 4;
	data->RECORDING_REC_STARTED = 5;
	data->RECORDING_REC_UPDATED = 6;
	data->RECORDING_REC_COMPLETED = 7;
	data->RECORDING_REC_PARTIALLY_COMPLETED = 8;
	data->RECORDING_TS_ACQUIRING_RESOURCES = 9;
	data->RECORDING_TS_STARTED = 10;
	data->RECORDING_ERROR = 11;

	data->ERROR_REC_RESOURCE_LIMITATION = 12;
	data->ERROR_INSUFFICIENT_STORAGE = 13;
	data->ERROR_TUNER_CONFLICT = 14;
	data->ERROR_REC_DRM_RESTRICTION = 15;
	data->ERROR_REC_UNKNOWN = 16;
	data->ERROR_TS_RESOURCE_LIMITATION = 17;
	data->ERROR_TS_DRM_RESTRICTION = 18;
	data->ERROR_TS_UNKNOWN = 19;
	return OOIF_RETURN_OK;
}
#endif // OIPF

#endif // HBBTV_PVR || OIPF
