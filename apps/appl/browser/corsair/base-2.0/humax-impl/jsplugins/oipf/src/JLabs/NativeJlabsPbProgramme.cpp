// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "NativeJlabsPbProgramme.h"

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NativeRecording.h"

#include "oplprogramme.h"
#include "oplscheduler.h"
#include "oplmetadatasearch.h"
#include "oplavcomponent.h"

#if defined(CONFIG_OP_JAPAN)
#include "oplprogramme_japan.h"
#endif


OOIFReturnCode native_jlabsPbProgrammeReleaseHandle(jlabsPbProgrammeHandle id)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		if (NULL != pstSrc->name)
		{
			HLIB_STD_MemFree(pstSrc->name);
		}

		if (NULL != pstSrc->description)
		{
			HLIB_STD_MemFree(pstSrc->description);
		}

		if (NULL != pstSrc->longDescription)
		{
			HLIB_STD_MemFree(pstSrc->longDescription);
		}

		if (NULL != pstSrc->channelID)
		{
			HLIB_STD_MemFree(pstSrc->channelID);
		}

		if (NULL != pstSrc->programmeID)
		{
			HLIB_STD_MemFree(pstSrc->programmeID);
		}

		if (NULL != pstSrc->contractVertificationInfo)
		{
			HLIB_STD_MemFree(pstSrc->contractVertificationInfo);
		}

		HLIB_STD_MemFree(pstSrc);
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetParentalRating(jlabsPbProgrammeHandle id, int *count, ParentalRatingHandle **handles)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;
	int						value = 0;
	OPL_ParentalRating_t	*parentalRating;
	ParentalRatingHandle	*myHandles = (ParentalRatingHandle*)HLIB_STD_MemAlloc(sizeof(ParentalRatingHandle));

	if (NULL != pstSrc)
	{
		value = pstSrc->parentalRating;
	}

	parentalRating = OPL_PCMAN_PRC_AddParentalRating(NULL, NULL, NULL, value, 0, NULL);
	myHandles[0] = (ParentalRatingHandle)parentalRating;

	*handles = myHandles;
	*count = 1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetName(jlabsPbProgrammeHandle id, const char **retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		if (NULL != pstSrc->name)
		{
			(*retval) = pstSrc->name;
		}
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetDescription(jlabsPbProgrammeHandle id, const char **retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		if (NULL != pstSrc->description)
		{
			(*retval) = pstSrc->description;
		}
		else
		{
			(*retval) = "";
		}
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetLongDescription(jlabsPbProgrammeHandle id, const char **retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		if (NULL != pstSrc->longDescription)
		{
			(*retval) = pstSrc->longDescription;
		}
		else
		{
			(*retval) = "";
		}
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetStartTime(jlabsPbProgrammeHandle id, OOIFNumber *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		(*retval) = pstSrc->startTime;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetDuration(jlabsPbProgrammeHandle id, OOIFNumber *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		(*retval) = pstSrc->duration;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetChannelID(jlabsPbProgrammeHandle id, const char **retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		OPL_Channel_t channel = OPL_Channel_FindChannelByUID(OPL_Channel_GetManager(), (int)pstSrc->svcuid);
		if (channel)
		{
			char buf[128];

			if (OPL_Channel_GetCCID(channel, buf, 128))
			{
				pstSrc->channelID = HLIB_STD_StrDup(buf);
			}
		}

		if (NULL != pstSrc->channelID)
		{
			(*retval) = pstSrc->channelID;
		}
		else
		{
			(*retval) = "";
		}
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetProgrammeID(jlabsPbProgrammeHandle id, const char **retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		char buf[128];

		OPL_Programme_MakeProgrammeID(buf, 128, (int)pstSrc->svcuid, (int)pstSrc->eventId);
		pstSrc->programmeID = HLIB_STD_StrDup(buf);
		if (NULL != pstSrc->programmeID)
		{
			(*retval) = pstSrc->programmeID;
		}
		else
		{
			(*retval) = "";
		}
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetProgrammeIDType(jlabsPbProgrammeHandle id, OOIFNumber *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		char buf[128];

		OPL_Programme_MakeProgrammeID(buf, 128, (int)pstSrc->svcuid, (int)pstSrc->eventId);
		pstSrc->programmeIDType = OPL_Programme_GetIDTypeFromProgrammeID(buf);
		(*retval) = pstSrc->programmeIDType;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetChannel(jlabsPbProgrammeHandle id, ChannelHandle *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;
	OPL_Channel_t			channel;

	if (NULL != pstSrc)
	{
		channel = OPL_Channel_FindChannelByUID(OPL_Channel_GetManager(), (int)pstSrc->svcuid);
		if (!channel)
		{
			return OOIF_RETURN_VALUE_UNDEFINED;
		}

		*retval = (ChannelHandle)OPL_Channel_Clone(channel);
	}
	else
	{
		*retval = NULL;
	}


	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetHasRecording(jlabsPbProgrammeHandle id, OOIFBoolean *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		*retval = pstSrc->hasRecording;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetSubtitles(jlabsPbProgrammeHandle id, OOIFBoolean *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		*retval = pstSrc->subtitles;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetIsHD(jlabsPbProgrammeHandle id, OOIFBoolean *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		*retval = pstSrc->isHD;
	}

	return OOIF_RETURN_OK;
}

static OPL_Collection_t	*getGenre(jlabsPbProgrammeHandle id)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;
	OPL_Collection_t		*coll = NULL;
	char					*text = NULL;
	unsigned short			word;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	if (coll == NULL)
	{
		return NULL;
	}

	word = pstSrc->content;
	if (word == 0)
	{
		return NULL;
	}

	text = OPL_PROGRAMME_JAPAN_GetGenre(word);
	if (text == NULL)
	{
		return NULL;
	}

	(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(text));
	HLIB_STD_MemFree(text);

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}

	return coll;
}

OOIFReturnCode native_jlabsPbProgrammeGetGenre(jlabsPbProgrammeHandle id, int *count, char ***genres)
{
	OPL_Collection_t 		*coll = NULL;

	coll = getGenre((jlabsPbProgrammeHandle)id);
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
		*count	= 0;
		*genres = (char **)HLIB_STD_StrDup("will be freed");
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetField(jlabsPbProgrammeHandle id, const char *fieldId,	const char **retval)
{
/*
	*retval = OPL_Programme_GetField((OPL_ProgrammeHandle)id, fieldId);
	if (*retval == NULL)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
*/
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetDigitalCopyControl(jlabsPbProgrammeHandle id, OOIFNumber *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		(*retval) = pstSrc->digitalCopyControl;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetAnalogCopyControl(jlabsPbProgrammeHandle id, OOIFNumber *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		(*retval) = pstSrc->analogCopyControl;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetIs3D(jlabsPbProgrammeHandle id, OOIFBoolean *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		*retval = pstSrc->is3D;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetIsCharged(jlabsPbProgrammeHandle id, OOIFBoolean *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		*retval = pstSrc->isCharged;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetContractVertificationInfo(jlabsPbProgrammeHandle id, const char **retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		if (NULL != pstSrc->contractVertificationInfo)
		{
			(*retval) = pstSrc->contractVertificationInfo;
		}
		else
		{
			(*retval) = "";
		}
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetRecordingStatus(jlabsPbProgrammeHandle id, OOIFNumber *retval)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;

	if (NULL != pstSrc)
	{
		(*retval) = pstSrc->recordingStatus;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPbProgrammeGetComponents(jlabsPbProgrammeHandle id, int componentType, int *count, AVComponentHandle **components)
{
	jlabsPbProgrammeData 	*pstSrc = (jlabsPbProgrammeData *)id;
	HUINT32					ulIndex, ulCompCnt = 0, ulCompNum = 0;
	AVComponentData			*pstCompData;
	AVComponentHandle		*handles = NULL;
	OplAVComponent_t		stOplComponent;

	if (NULL == pstSrc)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	OOIF_LOG_DUMMY("[%s:%d] VOD getComponents called with type (%d)", __FUNCTION__, __LINE__, componentType);

	*components = NULL;
	*count = 0;

	/* Get component number */
	/*
	if (!vodObj->countComponent((eOplAVComponentType)componentType, &ulCompNum))
	{
		goto END_FUNC;
	}
	*/

	if (0 == ulCompNum)
	{
		goto END_FUNC;
	}

	handles = (AVComponentHandle*)HLIB_STD_MemAlloc(sizeof(AVComponentHandle) * ulCompNum);
	if (NULL == handles)
	{
		goto END_FUNC;
	}

	memset(handles, 0, sizeof(AVComponentHandle) * ulCompNum);
	for (ulIndex = 0; ulIndex < ulCompNum; ulIndex++)
	{
		/* Get component */
		/*
		if (!vodObj->getComponent((eOplAVComponentType)componentType, ulIndex, &stOplComponent))
		{
			continue;
		}
		*/

		pstCompData = (AVComponentData *)HLIB_STD_MemAlloc(sizeof(AVComponentData));
		if (NULL == pstCompData)
		{
			continue;
		}

		memset (pstCompData, 0, sizeof(AVComponentData));
		pstCompData->componentTag	= stOplComponent.ulComponentTag;
		pstCompData->pid			= stOplComponent.ulPid;
		pstCompData->type			= stOplComponent.eType;
#if defined(HMX_WEBUI)
		pstCompData->subType		= stOplComponent.eSubType;
#endif
		pstCompData->audioDescription	=  stOplComponent.bAudioDescription;
		pstCompData->audioChannels 		=  stOplComponent.ulAudioChannels;

		if (0 < strlen(stOplComponent.szLanguage))
		{
			pstCompData->language	= HLIB_STD_StrDup(stOplComponent.szLanguage);
		}

#ifdef JLABS
		pstCompData->componentType = stOplComponent.ulComponentType;
		pstCompData->streamType = stOplComponent.ulStreamType;
		pstCompData->associatedContentFlag = stOplComponent.bAssociatedContentFlag;
		pstCompData->dataEntryComponentFlag = stOplComponent.bDataEntryComponentFlag;
		if (0 < strlen(stOplComponent.szLanguage2))
		{
			pstCompData->language2 	= HLIB_STD_StrDup(stOplComponent.szLanguage2);
		}
		if (0 < strlen(stOplComponent.szText))
		{
			pstCompData->text		= HLIB_STD_StrDup(stOplComponent.szText);
		}
#endif // JLABS

		handles[ulCompCnt++] = (AVComponentHandle)pstCompData;
	}

	if (ulCompCnt > 0)
	{
		*components = handles;
		*count = ulCompCnt;
		handles = NULL;
	}

END_FUNC:
	if (NULL != handles)
	{
		for (ulIndex = 0; ulIndex < ulCompNum; ulIndex++)
		{
			pstCompData = (AVComponentData *)handles[ulIndex];
			if (NULL != pstCompData)
			{
				if (NULL != pstCompData->language)
				{
					HLIB_STD_MemFree(pstCompData->language);
				}
				HLIB_STD_MemFree(pstCompData);
			}
		}
		HLIB_STD_MemFree(handles);
	}

	return OOIF_RETURN_OK;
}

