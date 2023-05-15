// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "NativeProgramme.h"

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NativeRecording.h"

#include "oplprogramme.h"
#include "oplscheduler.h"
#include "oplmetadatasearch.h"
#include "oplavcomponent.h"

OOIFReturnCode native_ProgrammeCreate(ProgrammeHandle *id, long int window_identifier, const char *host)
{
	OPL_ProgrammeHandle	self = OPL_Programme_New(NULL);
	if (self == NULL)
		return OOIF_RETURN_VALUE_NULL;
	*id = (ProgrammeHandle)self;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeReleaseHandle(ProgrammeHandle id)
{
	//OPL_METADATASEARCH_RemoveDependency(NULL, (OPL_ProgrammeHandle)id);

	OPL_Programme_Delete((OPL_ProgrammeHandle)id);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetName(ProgrammeHandle id, const char **retval)
{
	*retval = OPL_Programme_GetName((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetDescription(ProgrammeHandle id,
											  const char **retval)
{
	*retval = OPL_Programme_GetDescription((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetStartTime(ProgrammeHandle id, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetStartTime((OPL_ProgrammeHandle)id);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetDuration(ProgrammeHandle id, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetDuration((OPL_ProgrammeHandle)id);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetChannelID(ProgrammeHandle id, const char **retval)
{
	*retval = OPL_Programme_GetChannelID((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetParentalRating(ProgrammeHandle id,
												 int *count,
												 ParentalRatingHandle **handles)
{
	int	value;

	OPL_ParentalRating_t	*parentalRating;
	ParentalRatingHandle	*myHandles = (ParentalRatingHandle*)HLIB_STD_MemAlloc(sizeof(ParentalRatingHandle));

	value = OPL_Programme_GetParentalRating((OPL_ProgrammeHandle)id);

	parentalRating = OPL_PCMAN_PRC_AddParentalRating(NULL, NULL, NULL, value, 0, NULL);
	myHandles[0] = (ParentalRatingHandle)parentalRating;

	*handles = myHandles;
	*count = 1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetName(ProgrammeHandle id, const char* name)
{
	OPL_Programme_SetName((OPL_ProgrammeHandle)id, name);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetLongName(ProgrammeHandle id, const char* name)
{
	//OPL_Programme_SetLongName((OPL_ProgrammeHandle)id, name);
	// NOT AVAILABLE
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_ProgrammeSetDescription(ProgrammeHandle id,
											  const char* description)
{
	OPL_Programme_SetDescription((OPL_ProgrammeHandle)id, description);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetStartTime(ProgrammeHandle id,
											OOIFNumber startTime)
{
	OPL_Programme_SetStartTime((OPL_ProgrammeHandle)id, (unsigned long)startTime);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetDuration(ProgrammeHandle id,
										   OOIFNumber duration)
{
	OPL_Programme_SetDuration((OPL_ProgrammeHandle)id, (unsigned long)duration);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetChannelID(ProgrammeHandle id,
											const char* channelId)
{
	OPL_Programme_SetChannelID((OPL_ProgrammeHandle)id, channelId);
	return OOIF_RETURN_OK;
}

#if defined(OIPF) || defined(HBBTV_1_2_1)
OOIFReturnCode native_ProgrammeGetProgrammeID(ProgrammeHandle identifier, const char **retval)
{
	*retval = OPL_Programme_GetProgrammeID((OPL_ProgrammeHandle)identifier);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetProgrammeIDType(ProgrammeHandle identifier, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetProgrammeIDType((OPL_ProgrammeHandle)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetLongDescription(ProgrammeHandle identifier,
												  const char **retval)
{
	*retval = OPL_Programme_GetLongDescription((OPL_ProgrammeHandle)identifier);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetProgrammeID(ProgrammeHandle id, const char* programmeID)
{
	OPL_Programme_SetProgrammeID((OPL_ProgrammeHandle)id, programmeID);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetProgrammeIDType(ProgrammeHandle id, OOIFNumber programmeIDType)
{
	OPL_Programme_SetProgrammeIDType((OPL_ProgrammeHandle)id, (int)programmeIDType);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetLongDescription(ProgrammeHandle id,
											  const char* description)
{
	OPL_Programme_SetLongDescription((OPL_ProgrammeHandle)id, description);
	return OOIF_RETURN_OK;
}
#endif // OIPF || HBBTV_1_2_1

#ifdef OIPF
OOIFReturnCode native_ProgrammeSetEpisode(ProgrammeHandle id, OOIFNumber episode)
{
	OPL_Programme_SetEpisode((OPL_ProgrammeHandle)id, (int)episode);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetLongName(ProgrammeHandle identifier, const char **retval)
{
	// NOT AVAILABLE
	(void)identifier;
	*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetEpisode(ProgrammeHandle identifier, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetEpisode((OPL_ProgrammeHandle)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetTotalEpisodes(ProgrammeHandle identifier, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetTotalEpisodes((OPL_ProgrammeHandle)identifier);
	return OOIF_RETURN_OK;
}

#ifdef HMX_WEBUI
OOIFReturnCode native_ProgrammeGetSeason(RecordingHandle identifier, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetSeason((OPL_ProgrammeHandle)identifier);
    return OOIF_RETURN_OK;
}
#endif

OOIFReturnCode native_ProgrammeGetShowType(ProgrammeHandle identifier, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetShowType((OPL_ProgrammeHandle)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetAudioType(ProgrammeHandle identifier, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetAudioType((OPL_ProgrammeHandle)identifier);

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_ProgrammeGetBlocked(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_IsBlocked((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetSubtitles(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_HasSubtitle((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

#ifdef HMX_WEBUI
OOIFReturnCode native_ProgrammeGetSubtitleType(ProgrammeHandle identifier, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetSubtitleType((OPL_ProgrammeHandle)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetSubtitleTypes(ProgrammeHandle id, int *count, char ***types)
{
 	OPL_Collection_t *coll;

	coll = OPL_Programme_GetSubtitleTypes((OPL_ProgrammeHandle)id);
	if (coll)
	{
		*types = OPL_Collection_DumpString(coll, count);
		// note : don't delete below line 
		//because use CONFIG_OP_UK_DTT config on OPL_Programme_GetSubtitleTypes
		OPL_Collection_Delete(coll);
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
#endif

OOIFReturnCode native_ProgrammeGetIsHD(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_IsHD((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetIsMultilingual(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_IsMultilingual((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetLocked(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_IsLocked((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetHasRecording(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_HasRecording((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetGenre(ProgrammeHandle id, int *count, char ***genres)
{
	OPL_Collection_t *coll;

	coll = OPL_Programme_GetGenre((OPL_ProgrammeHandle)id);
	if (coll)
	{
		*genres = OPL_Collection_DumpString(coll, count);
	}
	else
	{
		*genres = NULL;
	}
	if (*genres == NULL)
	{
		*count  = 0;
		*genres = (char **)HLIB_STD_StrDup("will be freed");
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetAudioLanguages(ProgrammeHandle id, int *count,
												 char ***languages)
{
	OPL_Collection_t *coll;

	coll = OPL_Programme_GetAudioLanguages((OPL_ProgrammeHandle)id);
	if (coll)
	{
		*languages = OPL_Collection_DumpString(coll, count);
	}
	else
	{
		*languages = NULL;
	}
	if (*languages == NULL)
	{
		*count  = 0;
		*languages = (char **)HLIB_STD_StrDup("will be freed");
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetSubtitleLanguages(ProgrammeHandle id, int *count,
													char ***languages)
{
	OPL_Collection_t *coll;

	coll = OPL_Programme_GetSubtitleLanguages((OPL_ProgrammeHandle)id);
	if (coll)
	{
		*languages = OPL_Collection_DumpString(coll, count);
	}
	else
	{
		*languages = NULL;
	}
	if (*languages == NULL)
	{
		*count  = 0;
		*languages = (char **)HLIB_STD_StrDup("will be freed");
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetChannel(ProgrammeHandle identifier, ChannelHandle *retval)
{
	OPL_Channel_t	channel;

	channel = OPL_Programme_GetChannel((OPL_ProgrammeHandle)identifier);
	if (!channel)
		return OOIF_RETURN_VALUE_UNDEFINED;
	*retval = (ChannelHandle)OPL_Channel_Clone(channel);

	return OOIF_RETURN_OK;
}

#ifdef HMX_WEBUI

OOIFReturnCode native_ProgrammeGetProgrammeIDSubtype(ProgrammeHandle identifier, const char **retval)
{
	*retval = OPL_Programme_GetProgrammeIDSubtype((OPL_ProgrammeHandle)identifier);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetRecording(ProgrammeHandle identifier,
													 ScheduledRecordingHandle *retval)
{
	OPL_ScheduledItem_t	*schedule;


	schedule = (OPL_ScheduledItem_t*)OPL_Programme_GetRecording((OPL_ProgrammeHandle)identifier);
	if (!schedule)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if (OPL_ScheduledItem_InstanceOf(schedule, OPL_ScheduledItem_t))
	{
		*retval = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(schedule);
	}
	else
	{
		*retval = (ScheduledRecordingHandle)schedule;
	}

	if (!(*retval))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetRecordings(ProgrammeHandle id, int *count,
											 RecordingHandle **handles)
{
	OPL_Collection_t	*shceduleColl;
	int					nScheduleCollCnt;

	shceduleColl = OPL_Programme_GetRecordings((OPL_ProgrammeHandle)id);
	if (!shceduleColl)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	nScheduleCollCnt = OPL_Collection_Length(shceduleColl);
	ScheduledRecordingHandle *array = (ScheduledRecordingHandle *) HLIB_STD_MemAlloc (sizeof(ScheduledRecordingHandle) * nScheduleCollCnt);
	if (array == NULL)
	{
		if (shceduleColl)
			OPL_Collection_Delete(shceduleColl);
		return OOIF_RETURN_OOM_ERROR;
	}

	for (int i = 0; i < nScheduleCollCnt; ++i)
	{
		OPL_ScheduledItem_t	*schedule = (OPL_ScheduledItem_t*)OPL_Collection_ItemAt(shceduleColl, i);
		if (OPL_ScheduledItem_InstanceOf(schedule, OPL_ScheduledItem_t))
		{
			array[i] = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(schedule);
		}
		else
		{
			array[i] = (ScheduledRecordingHandle)OPL_Recording_Clone((OPL_Recording_t *)schedule);
		}
	}

	*handles = array;
	*count	 = nScheduleCollCnt;

	if (shceduleColl)
	{	// don't use cahce
		OPL_Collection_Delete(shceduleColl);
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetScheduledRecording(ProgrammeHandle identifier,
													 ScheduledRecordingHandle *retval)
{
	OPL_ScheduledItem_t	*schedule;

	schedule = (OPL_ScheduledItem_t*)OPL_Programme_GetScheduledRecording((OPL_ProgrammeHandle)identifier);
	if (!schedule)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if (OPL_ScheduledItem_InstanceOf(schedule, OPL_ScheduledItem_t))
	{
		*retval = (ScheduledRecordingHandle)OPL_ScheduledItem_Clone(schedule);
	}

	if (!(*retval))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	return OOIF_RETURN_OK;
}

#else
OOIFReturnCode native_ProgrammeGetRecording(ProgrammeHandle id, PVRHandle *recording)
{
	*retval = (void*)4;
	return OOIF_RETURN_OK;
}
#endif

OOIFReturnCode native_ProgrammeGetField(ProgrammeHandle id, const char *fieldId,
										const char **retval)
{
	*retval = OPL_Programme_GetField((OPL_ProgrammeHandle)id, fieldId);
	if (*retval == NULL)
		return OOIF_RETURN_VALUE_UNDEFINED;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetIMI(ProgrammeHandle id, const char **retval)
{
	*retval = OPL_Programme_GetIMI((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetGroupCRIDs(ProgrammeHandle id, int *count,
											 char ***crids)
{
 	OPL_Collection_t *coll;

	coll = OPL_Programme_GetGroupCRIDs((OPL_ProgrammeHandle)id);
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

OOIFReturnCode native_ProgrammeGetCRIDSearchFlag(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_GetCRIDSearchFlag((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetProgrammeCRID(ProgrammeHandle id,const char **retval)
{
	*retval = OPL_Programme_GetProgrammeCRID((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetRecommendationCRID(ProgrammeHandle id,const char **retval)
{
	*retval = OPL_Programme_GetRecommendationCRID((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetTotalEpisodes(ProgrammeHandle id, OOIFNumber totalEpisodes)
{
	OPL_Programme_SetTotalEpisodes((OPL_ProgrammeHandle)id, (int)totalEpisodes);
	return OOIF_RETURN_OK;
}
#endif // OIPF
#if defined(OIPF) || defined(HBBTV_1_2_1)
OOIFReturnCode native_ProgrammeGetSIDescriptors(ProgrammeHandle id, OOIFNumber tag,
                                                OOIFBoolean hasTagExtension,
                                                OOIFNumber tagExtension,
                                                int *count, char ***sidescriptors, int **sidescriptor_lenghts)
{
	OPL_Collection_t *coll;
	char			 *rawDesc;

    // have to pass hasTagExtension to OPL
	coll = OPL_Programme_GetSiDescription((OPL_ProgrammeHandle)id , (int)tag, (int)tagExtension);
	if (coll)
	{
		*sidescriptors = (char **)OPL_Collection_Dump(coll, count, NULL);
		if (*sidescriptors != NULL && *count > 0 )
		{
			int *mySidescriptor_lenghts = (int *)HLIB_STD_MemAlloc(sizeof(int)*(*count));
			for ( int i=0; i<*count; ++i )
			{
#ifdef JLABS
				mySidescriptor_lenghts[i] = strlen(const_cast<char *>(*sidescriptors[i]));
#else
				rawDesc = *sidescriptors[i];
				mySidescriptor_lenghts[i] = rawDesc[1] + 2;	// sidescriptors[1] : desc length, 2: desc tag + desc length
#endif
			}
			*sidescriptor_lenghts = mySidescriptor_lenghts;
		}
	}
	else
	{
		*sidescriptors = NULL;
	}

	if (*sidescriptors == NULL)
	{
		*count  = 0;
		//*sidescriptors = (char **)HLIB_STD_StrDup("will be freed");
		return OOIF_RETURN_VALUE_NULL;
	}

	return OOIF_RETURN_OK;
}
#endif // OIPF || HBBTV_1_2_1

#ifdef JLABS
OOIFReturnCode native_ProgrammeGetDigitalCopyControl(ProgrammeHandle id, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetCopyControl((OPL_ProgrammeHandle)id, DIGITAL_COPY_TYPE);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetAnalogCopyControl(ProgrammeHandle id, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetCopyControl((OPL_ProgrammeHandle)id, ANALOG_COPY_TYPE);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetIs3D(ProgrammeHandle id, OOIFBoolean *retval)
{
	*retval = (OOIFBoolean)OPL_Programme_GetIs3D((OPL_ProgrammeHandle)id);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetIsCharged(ProgrammeHandle id, OOIFBoolean *retval)
{
	*retval = (OOIFBoolean)OPL_Programme_GetIsCharged((OPL_ProgrammeHandle)id);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetContractVerificationInfo(ProgrammeHandle id, const char **retval)
{
	*retval = OPL_Programme_GetContractVerificationInfo((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetRecordingStatus(ProgrammeHandle id, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetRecordingStatus((OPL_ProgrammeHandle)id);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetComponents(ProgrammeHandle id, OOIFNumber componentType, int *count, AVComponentHandle **retval)
{
	OPL_Collection_t *coll;

	coll = OPL_Programme_GetComponents((OPL_ProgrammeHandle)id , (eOplAVComponentType)componentType);
	if (coll)
	{
		*retval = (AVComponentHandle *)OPL_Collection_Dump(coll, count, NULL);
	}
	else
	{
		*retval = NULL;
	}

	if (*retval == NULL)
	{
		*count = 0;
		*retval = NULL;
		return OOIF_RETURN_VALUE_NULL;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetDigitalCopyControl(ProgrammeHandle id, OOIFNumber digitalCopyContorl)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetAnalogCopyControl(ProgrammeHandle id, OOIFNumber analogCopyControl)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetIs3D(ProgrammeHandle id, OOIFBoolean is3D)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetIsCharged(ProgrammeHandle id, OOIFBoolean isCharged)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeSetContractVerificationInfo(ProgrammeHandle id, const char *contractVerificationInfo)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetSharedProgrammeIds(ProgrammeHandle id, int *count,	char ***sharedProgrammeIds)
{
	OPL_Collection_t *coll;

	coll = OPL_Programme_GetSharedProgrammeIds((OPL_ProgrammeHandle)id);
	if (coll)
	{
		*sharedProgrammeIds = OPL_Collection_DumpString(coll, count);
	}
	else
	{
		*sharedProgrammeIds = NULL;
	}
	if (*sharedProgrammeIds == NULL)
	{
		*count  = 0;
		*sharedProgrammeIds = (char **)HLIB_STD_StrDup("will be freed");
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetSharedProgrammeInfo(ProgrammeHandle id, const char *sharedProgrammeId, ProgrammeHandle *pSharedProgramme)
{
	*pSharedProgramme = OPL_Programme_GetSharedProgrammeInfo( sharedProgrammeId );

	return OOIF_RETURN_OK;
}

#endif // JLABS


OOIFReturnCode native_ProgrammeGetMediaDeeplinkID(ProgrammeHandle id, const char **retval)
{
	*retval = OPL_Programme_GetMediaDeeplinkID((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_ProgrammeGetMediaDeeplinkUrl(ProgrammeHandle id, const char **retval)
{
	*retval = OPL_Programme_GetMediaDeeplinkUrl((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		*retval = "";
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_ProgrammeGetMediaDeeplinkExpireDate(ProgrammeHandle id, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetMediaDeeplinkExpireDate((OPL_ProgrammeHandle)id);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetOnDemandStartOfAvailability(ProgrammeHandle id, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetOnDemandStartOfAvailability((OPL_ProgrammeHandle)id);
	if (*retval == 0)
		return OOIF_RETURN_VALUE_NULL;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetOnDemandEndOfAvailability(ProgrammeHandle id, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetOnDemandEndOfAvailability((OPL_ProgrammeHandle)id);
	if (*retval == 0)
		return OOIF_RETURN_VALUE_NULL;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetOnDemandPublishedDuration(ProgrammeHandle id, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OPL_Programme_GetOnDemandPublishedDuration((OPL_ProgrammeHandle)id);
	if (*retval == 0)
		return OOIF_RETURN_VALUE_NULL;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetOnDemandIsFree(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_GetOnDemandIsFree((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetOnDemandIsHD(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_GetOnDemandIsHD((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetOnDemandIsMediaAvailable(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_GetOnDemandIsMediaAvailable((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetOnDemandIsFEPGAvailable(ProgrammeHandle identifier, OOIFBoolean *retval)
{
	*retval = OPL_Programme_GetOnDemandIsFEPGAvailable((OPL_ProgrammeHandle)identifier) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetOnDemandProgramUrl(ProgrammeHandle id, const char **retval)
{
	*retval = OPL_Programme_GetOnDemandProgramUrl((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		return OOIF_RETURN_VALUE_NULL;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetOnDemandAuxiliaryUrl(ProgrammeHandle id, const char **retval)
{
	*retval = OPL_Programme_GetOnDemandAuxiliaryUrl((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		return OOIF_RETURN_VALUE_NULL;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ProgrammeGetOnDemandDeliveryMode(ProgrammeHandle id, const char **retval)
{
	*retval = OPL_Programme_GetOnDemandDeliveryMode((OPL_ProgrammeHandle)id);
	if (*retval == NULL)
		return OOIF_RETURN_VALUE_NULL;
	return OOIF_RETURN_OK;
}





