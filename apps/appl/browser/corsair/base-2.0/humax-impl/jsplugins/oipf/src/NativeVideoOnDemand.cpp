// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "NativeVideoOnDemand.h"
#include "VideoOnDemand.h"

#include "macros.h"


#ifdef JLABS
#include "oplvodjlabshandle.h"
#else
#include "oplvodavchandle.h"
#endif

#include "oplprogramme.h"
#include "opllocalsystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Ooif;

static const char* supportedMimeTypes[] =
{
	"audio/mpeg",
	"audio/mp3",
	"audio/mp4",
	"audio/x-wav",
	"audio/wma",
	"video/H264",
	"video/mp4",
	"video/mpeg",
	"video/mpeg4",
	"video/x-ms-wmv",
	"video/x-ms-asf",
	"application/dash+xml",
	"application/x-mpegURL",
	"application/vnd.ms-sstr+xml"
};

#ifdef JLABS
#define MAKE_VOD_OBJECT(in, out) OplVodJlabsHandle* out = static_cast<OplVodJlabsHandle*>(in)
#else
#define MAKE_VOD_OBJECT(in, out) OplVodAvcHandle* out = static_cast<OplVodAvcHandle*>(in)
#endif

static void native_videoondemand_event_listener(void *hPlayer, void *handle, eOplVodEventType type)
{
	MAKE_VOD_OBJECT(handle, vodObj);
	if ( vodObj == NULL ) return;

	VodEventInfo info;
	switch ( type )
	{
	case OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED:
		OOIF_LOG_DEBUG("PlayStateChange\n");
		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_PLAY_STATE_CHANGED,
					NULL, static_cast<VodHandle>(vodObj));
		break;
	case OPLVOD_EVENT_TYPE_PLAY_SPEED_CHANGED:
	{
		OOIF_LOG_DEBUG("PlaySpeedChanged\n");
		int speed = vodObj->getPlaySpeed();
		info.play_speed_changed_data.speed = (double)(speed / (double)100.0);	// OIPF (-32/-16/-8/-4/-2/0/1/2/4/8/16/32):OPL(OIPF value*100)
		OOIF_LOG_DEBUG("	OPLVOD_EVENT_TYPE_PLAY_SPEED_CHANGED (%d) - oipfSpeed (%f)\n", speed, info.play_speed_changed_data.speed);
		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_PLAY_SPEED_CHANGED,
					&info, static_cast<VodHandle>(vodObj));
		break;
	}
	case OPLVOD_EVENT_TYPE_SUPPORTED_PLAY_SPEED_CHANGED:
	{
		OOIF_LOG_DEBUG("PlaySpeedsArrayChanged\n");
		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_PLAY_SPEEDS_ARRAY_CHANGED,
					NULL, static_cast<VodHandle>(vodObj));
		break;
	}
	case OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED:
	{
		OOIF_LOG_DEBUG("PlayPositionChange\n");
		unsigned int playPosition = vodObj->getPlayPosition();
		info.play_position_changed_data.has_position = 1;
		info.play_position_changed_data.position = playPosition;

		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_PLAY_POSITION_CHANGED,
					&info, static_cast<VodHandle>(vodObj));
		break;
	}
	case OPLVOD_EVENT_TYPE_SELECTED_COMPONENT_CHANGED:
	{
		OOIF_LOG_DEBUG("SelectedComponentChanged\n");
		eOplAVComponentType eComponentType = vodObj->getSelectedComponentType();
		info.selected_component_changed_data.componentType = eComponentType;
		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_SELECTED_COMPONENT_CHANGED,
					&info, static_cast<VodHandle>(vodObj));
		break;
	}
	case OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED:
	{
		eOplVodBufferState state = vodObj->getBufferState();
		switch ( state )
		{
		case OPLVOD_BUFFER_STATE_NOT_ENOUGH:
		case OPLVOD_BUFFER_STATE_ENOUGH:
		case OPLVOD_BUFFER_STATE_FINISHED:
		case OPLVOD_BUFFER_STATE_CANPLAY:
			VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_READY_STATE_CHANGED,
						NULL, static_cast<VodHandle>(vodObj));
			break;
		case OPLVOD_BUFFER_STATE_NONE:
		default:
			break;
		}
	}
		break;

	case OPLVOD_EVENT_TYPE_PARENTAL_RATING_CHANGE:
	{
		bool blocked = false;
		unsigned int rating = 0;
		char *cas_id = NULL;

		if ( vodObj->getParentalRating(&blocked, &rating, &cas_id) == false ) break;

		OPL_ParentalRating_t *opr = OPL_PCMAN_PRC_AddParentalRating(NULL,
												NULL, NULL, rating, 0, NULL);
		if ( !opr ) break;

		info.parental_rating_change_data.contentID = NULL;
		info.parental_rating_change_data.blocked = blocked;
		info.parental_rating_change_data.DRMSystemID = cas_id;
		info.parental_rating_change_data.ratings = reinterpret_cast<ParentalRatingHandle *>(opr);

		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_PARENTAL_RATING_CHANGE,
					&info, static_cast<VodHandle>(vodObj));
	}
		break;

	case OPLVOD_EVENT_TYPE_DRM_RIGHT_ERROR:
	{
		eOplVodDrmRightsError error;
		char *cas_id = NULL;
		char *content_id = NULL;
		char *rights_issuer_id = NULL;

		if ( vodObj->getDrmRightsError(&error, &cas_id, &content_id, &rights_issuer_id) == false ) break;

		info.drm_rights_error_data.error_state = static_cast<OOIFNumber>(error);
		info.drm_rights_error_data.content_id = content_id;
		info.drm_rights_error_data.drm_system_id = cas_id;
		info.drm_rights_error_data.rights_issuer_url = rights_issuer_id;

		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_DRM_RIGHTS_ERROR,
					&info, static_cast<VodHandle>(vodObj));
	}
		break;

	case OPLVOD_EVENT_TYPE_TRICK_RESTRICT_MODE_CHANGED:
	{
		OOIF_LOG_DEBUG("TrickRestrictModeChanged\n");
		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_TRICK_RESTRICT_MODE_CHANGED,
					NULL, static_cast<VodHandle>(vodObj));
		break;
	}

	case OPLVOD_EVENT_TYPE_AUDIO_OUTPUT_RESTRICT:
	{
		OOIF_LOG_DEBUG("SampleRataeRestrictError\n");
		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_AUDIO_OUTPUT_RESTRICT,
					NULL, static_cast<VodHandle>(vodObj));
		break;
	}
#ifdef JLABS
	case OPLVOD_EVENT_TYPE_PROGRAMME_CHANGED:
		OOIF_LOG_DEBUG("ProgrammeChange\n");
		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_PROGRAMME_CHANGED,
					NULL, static_cast<VodHandle>(vodObj));
		break;

	case OPLVOD_EVENT_TYPE_BML_START:
		OOIF_LOG_DEBUG("bml start\n");
		info.bml_start.autoFlag = vodObj->getBmlStartFlag();
		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_BML_START,
					&info, static_cast<VodHandle>(vodObj));
		break;

	case OPLVOD_EVENT_TYPE_BML_END:
	{
		int		cause;

		OOIF_LOG_DEBUG("bml end\n");
		if ( false == vodObj->getBmlEndParam(&cause, info.bml_end.param) ) return;

		info.bml_end.cause = cause;
		VideoOnDemand::onVideoOnDemandEvent(VOD_EVENT_BML_END,
					&info, static_cast<VodHandle>(vodObj));
	}
		break;
#endif

	default:
		break;
	}
}

OOIFReturnCode native_VodGetSupportedMimetypes(char ***mime_types, int* count)
{
	*count = sizeof(supportedMimeTypes) / sizeof(supportedMimeTypes[0]);
	*mime_types = (char**) HLIB_STD_MemAlloc(*count * sizeof(char*));
	for (int i = 0; i < *count; i++)
	{
		(*mime_types)[i] = HLIB_STD_StrDup(supportedMimeTypes[i]);
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodCreate(VodHandle *id, long int window_identifier, const char *host_uri, const char *mime_type, const char *data)
{
#ifdef JLABS
	OplVodJlabsHandle *vodObj = OplVodJlabsHandle::create(OplVideoHandle::EHandleOwnerTypeUserOipf);
#else
	OplVodAvcHandle *vodObj = OplVodAvcHandle::create(OplVideoHandle::EHandleOwnerTypeUserOipf);
#endif
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	if ( data )
		vodObj->setUrl(data);

	if ( mime_type )
		vodObj->setMimeType(mime_type);

	*id = static_cast<void *>(vodObj);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetListener(vodListener listener)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodReleaseHandle(VodHandle id)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	OplVideoHandle::unRegist(vodObj);
	delete vodObj;
	vodObj = NULL;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodOnListenerAccessed(VodHandle id, const char *eventName)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetData(VodHandle id, const char **retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	const char *url = vodObj->getUrl();
	if ( url == NULL )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*retval = url;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetType(VodHandle id, const char **retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	const char *mime_type = vodObj->getMimeType();
	if ( mime_type == NULL )
	{
		return OOIF_RETURN_VALUE_NULL;
	}
	*retval = mime_type;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetData(VodHandle id, const char *data)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	vodObj->setUrl(data);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetType(VodHandle id, const char* type)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	vodObj->setMimeType(type);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetPlayPosition(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int playPosition = vodObj->getPlayPosition();

	*retval = (OOIFNumber)playPosition;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetPlayTime(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int playTime = vodObj->getDuration();

	*retval = (OOIFNumber)playTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetPlayState(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	eOplVodPlayState playState = vodObj->getPlayState();

	*retval = static_cast<OOIFNumber>(playState);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetError(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	eOplVodPlayError playError = vodObj->getPlayError();

	*retval = static_cast<OOIFNumber>(playError);

	return OOIF_RETURN_OK;
}

#if defined(MAXDOME)
OOIFReturnCode native_VodGetErrorDetails(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	eOplVodPlayErrorDetails playErrorDetails = vodObj->getPlayErrorDetails();

	*retval = static_cast<OOIFNumber>(playErrorDetails);

	return OOIF_RETURN_OK;
}
#endif

OOIFReturnCode native_VodGetSpeed(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int speed = vodObj->getPlaySpeed();

	double fSpeed = (double)((double)speed / (double)100.0);
	*retval = (OOIFNumber)fSpeed;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetNrTracks(VodHandle id, OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("VOD native_VodGetNrTracks called");
	*retval =1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetCurrentTrackIndex(VodHandle id, OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("VOD native_VodSetCurrentTrackIndex called");
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetCurrentTrackIndex(VodHandle id, OOIFNumber index)
{
	OOIF_LOG_DEBUG("VOD native_VodSetCurrentTrackIndex called");
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetPersist(VodHandle id, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("VOD native_VodGetPersist called");
	*retval = true;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetPersist(VodHandle id, OOIFBoolean _persist)
{
	OOIF_LOG_DEBUG("VOD native_VodSetPersist called");
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_VodGetSinkProtocolInfo(VodHandle id, const char **retval)
{
	OOIF_LOG_DEBUG("VOD getSinkProtocolInfo called");
	*retval = "http-get:*:audio/mpeg:*";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetTransportStateVariables(VodHandle id, const char *stateVariableList, const char **retval)
{
	OOIF_LOG_DEBUG("VOD getTransportStateVariables called: stateVariableList = %s", stateVariableList);
	*retval = "dummy transport state variable";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetTransportStateVariables(VodHandle id, const char *stateVariableValuePairs, const char **retval)
{
	OOIF_LOG_DEBUG("VOD setTransportStateVariables called: stateVariableValuePairs = %s", stateVariableValuePairs);
	*retval = "CurrentPlayMode,CurrentTrack";
	return OOIF_RETURN_OK;
}
#endif // OIPF


// functions
OOIFReturnCode native_VodPlay(VodHandle id, OOIFNumber speed, OOIFBoolean *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		*retval = FALSE;
		return OOIF_RETURN_ERROR;
	}

	vodObj->setListener((void *)id, native_videoondemand_event_listener);

	if ( !vodObj->play() )
	{
		*retval = FALSE;
		return OOIF_RETURN_OK;
	}

	double fSpeed = 0;
	fSpeed = (double)((double)speed * (double)100.0);
	vodObj->setPlaySpeed(static_cast<int>(fSpeed));

	*retval = TRUE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodStop(VodHandle id, OOIFBoolean *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		*retval = FALSE;
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->stop() )
	{
		*retval = FALSE;
		return OOIF_RETURN_ERROR;
	}

	*retval = TRUE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSeek(VodHandle id, OOIFNumber pos, OOIFBoolean *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		*retval = FALSE;
		return OOIF_RETURN_ERROR;
	}

	if ( pos < 0 )
	{
		*retval = FALSE;
		return OOIF_RETURN_ERROR;
	}
    
#if defined(CONFIG_TIMESTAMP_64)
    if ( !vodObj->seek(static_cast<unsigned long long>(pos)) )
    {
        *retval = FALSE;
        return OOIF_RETURN_ERROR;
    }
#else
    if ( !vodObj->seek(static_cast<unsigned int>(pos)) )
    {
        *retval = FALSE;
        return OOIF_RETURN_ERROR;
    }
#endif

	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetVolume(VodHandle id, OOIFNumber volume, OOIFBoolean *retval)
{
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_VodGetVolume(VodHandle id, OOIFNumber *volume)
{
	OOIF_LOG_DEBUG("VOD getVolume called");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_VodNext(VodHandle id, OOIFBoolean *retval)
{
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_VodPrevious(VodHandle id, OOIFBoolean *retval)
{
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_VodSetFullScreen(VodHandle id, OOIFBoolean fullscreen)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->setFullScreen(fullscreen) )
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetComponents(VodHandle id, OOIFNumber componentType, int *count, AVComponentHandle **components)
{
#if defined(OIPF) || defined(HBBTV_1_2_1)
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	HUINT32						 ulIndex, ulCompCnt = 0, ulCompNum = 0;
	AVComponentData				*pstCompData;
	AVComponentHandle			*handles = NULL;
	OplAVComponent_t			 stOplComponent;

	OOIF_LOG_DEBUG("[%s:%d] VOD getComponents called with type (%d)", __FUNCTION__, __LINE__, componentType);

	*components = NULL;
	*count = 0;

	ulCompNum = vodObj->getComponentsCount((eOplAVComponentType)componentType);
	if (0 == ulCompNum)						{ goto END_FUNC; }

	handles = (AVComponentHandle*)HLIB_STD_MemAlloc (sizeof(AVComponentHandle) * ulCompNum);
	if (NULL == handles)					{ goto END_FUNC; }
	memset (handles, 0, sizeof(AVComponentHandle) * ulCompNum);

	OOIF_LOG_DEBUG ("\n\n\n=== Component Type (%d), Component Num (%d) ===\n", componentType, ulCompNum);

	for (ulIndex = 0; ulIndex < ulCompNum; ulIndex++)
	{
		if (!vodObj->getComponent((eOplAVComponentType)componentType, ulIndex, &stOplComponent))
		{
			continue;
		}

		pstCompData = (AVComponentData *)HLIB_STD_MemAlloc (sizeof(AVComponentData));
		if (NULL == pstCompData)				{ continue; }
		memset (pstCompData, 0, sizeof(AVComponentData));

		pstCompData->componentTag	= stOplComponent.ulComponentTag;
		pstCompData->pid			= stOplComponent.ulPid;
		pstCompData->type			= static_cast<AVComponentType>(stOplComponent.eType);
#if defined(HMX_WEBUI)
		pstCompData->subType		= stOplComponent.eSubType;
#endif
		if (0 < strlen( stOplComponent.encoding))
			pstCompData->encoding 	= HLIB_STD_StrDup (stOplComponent.encoding);

		switch(pstCompData->type){
			case COMPONENT_TYPE_VIDEO:
				pstCompData->has_aspectRatio = TRUE;
				pstCompData->aspectRatio	= stOplComponent.fAspectRatio;
				break;
			case COMPONENT_TYPE_AUDIO:
				if (0 < strlen( stOplComponent.szLanguage))
					pstCompData->audio_language		= HLIB_STD_StrDup (stOplComponent.szLanguage);
#if defined(HMX_WEBUI)
				if (0 < strlen( stOplComponent.szLanguage2))
					pstCompData->language2		= HLIB_STD_StrDup (stOplComponent.szLanguage2);
#endif
				pstCompData->audioDescription  = stOplComponent.bAudioDescription;
				pstCompData->audioChannels		= stOplComponent.ulAudioChannels;
				break;
			case COMPONENT_TYPE_SUBTITLE:
				if (0 < strlen( stOplComponent.szLanguage))
					pstCompData->subtitle_language		= HLIB_STD_StrDup (stOplComponent.szLanguage);
				pstCompData->hearingImpaired= stOplComponent.bHardOfHearing_Impaired;
				break;
			default:
				break;
		}
#ifdef JLABS
		pstCompData->componentType = stOplComponent.ulComponentType;
		pstCompData->streamType = stOplComponent.ulStreamType;
		pstCompData->associatedContentFlag = stOplComponent.bAssociatedContentFlag;
		pstCompData->dataEntryComponentFlag = stOplComponent.bDataEntryComponentFlag;
		if (0 < strlen( stOplComponent.szLanguage2)) pstCompData->language2 	= HLIB_STD_StrDup (stOplComponent.szLanguage2);
		if (0 < strlen( stOplComponent.szText)) pstCompData->text		= HLIB_STD_StrDup (stOplComponent.szText);
#endif // JLABS
#if defined(HMX_WEBUI)
		OOIF_LOG_DEBUG("\n\n\n===[native_VideoOnDemandGetComponents]  PID (%#x), LANG (%s) type (%d) subType(%d)===\n", (int)pstCompData->pid, (int)pstCompData->audio_language, (int)pstCompData->type,pstCompData->subType);
#endif
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
				if (NULL != pstCompData->audio_language)				{ HLIB_STD_MemFree(pstCompData->audio_language); }
#if defined(HMX_WEBUI)
				if (NULL != pstCompData->language2)				{ HLIB_STD_MemFree(pstCompData->language2); }
#endif
				HLIB_STD_MemFree(pstCompData);
			}
		}

		HLIB_STD_MemFree(handles);
	}

	return OOIF_RETURN_OK;
#else
	return OOIF_RETURN_NOT_SUPPORTED;
#endif
}



OOIFReturnCode native_VodGetCurrentActiveComponents(VodHandle id, OOIFNumber componentType, int *count, AVComponentHandle **components)
{
#if defined(OIPF) || defined(HBBTV_1_2_1)
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	AVComponentData 			*pstCompData;
	AVComponentHandle			*handles = NULL;
	OplAVComponent_t 			 stOplComponent;

	OOIF_LOG_DEBUG("VOD getCurActive Components called with type (%d)\n", componentType);

	*components = NULL;
	*count = 0;

	handles = (AVComponentHandle*)HLIB_STD_MemAlloc (sizeof(AVComponentHandle));
	if (NULL == handles)					{ goto END_FUNC; }
	memset (handles, 0, sizeof(AVComponentHandle));

	if (!vodObj->getActiveComponent((eOplAVComponentType)componentType, &stOplComponent))
	{
		OOIF_LOG_DEBUG ("\n\n\n=== native_VodGetCurrentActiveComponents() FAIL===\n");
		goto END_FUNC;
	}

	pstCompData = (AVComponentData *)HLIB_STD_MemAlloc (sizeof(AVComponentData));
	if (NULL == pstCompData)
	{
		goto END_FUNC;
	}

	memset (pstCompData, 0, sizeof(AVComponentData));

	pstCompData->componentTag	= stOplComponent.ulComponentTag;
	pstCompData->pid			= stOplComponent.ulPid;
	pstCompData->type			= static_cast<AVComponentType>(stOplComponent.eType);
#if defined(HMX_WEBUI)
	pstCompData->subType		= stOplComponent.eSubType;
#endif
	if (0 < strlen( stOplComponent.szLanguage))   pstCompData->audio_language 	= HLIB_STD_StrDup (stOplComponent.szLanguage);
#if defined(HMX_WEBUI)
	if (0 < strlen( stOplComponent.szLanguage2))   pstCompData->language2 	= HLIB_STD_StrDup (stOplComponent.szLanguage2);
#endif

#ifdef JLABS
	pstCompData->componentType = stOplComponent.ulComponentType;
	pstCompData->streamType = stOplComponent.ulStreamType;
	pstCompData->associatedContentFlag = stOplComponent.bAssociatedContentFlag;
	pstCompData->dataEntryComponentFlag = stOplComponent.bDataEntryComponentFlag;
	if (0 < strlen( stOplComponent.szLanguage2)) pstCompData->language2 	= HLIB_STD_StrDup (stOplComponent.szLanguage2);
	if (0 < strlen( stOplComponent.szText)) pstCompData->text		= HLIB_STD_StrDup (stOplComponent.szText);
#endif // JLABS

	handles[0] = (AVComponentHandle)pstCompData;
#if defined(HMX_WEBUI)
	OOIF_LOG_DEBUG("\n\n\n===[native_VodGetCurrentActiveComponents] PID (%d), LANG (%s) type (%d) subType(%d)===\n"
			, pstCompData->pid, pstCompData->audio_language,pstCompData->type,pstCompData->subType);
#endif
	*components = handles;
	*count = 1;

	handles = NULL;

END_FUNC:
	if (NULL != handles)
	{
		pstCompData = (AVComponentData *)handles[0];
		if (NULL != pstCompData)
		{
			if (NULL != pstCompData->audio_language)				{ HLIB_STD_MemFree(pstCompData->audio_language); }
#if defined(HMX_WEBUI)
			if (NULL != pstCompData->language2)				{ HLIB_STD_MemFree(pstCompData->language2); }
#endif

			HLIB_STD_MemFree(pstCompData);
		}

		HLIB_STD_MemFree(handles);
	}

	return OOIF_RETURN_OK;
#else
	return OOIF_RETURN_NOT_SUPPORTED;
#endif
}

OOIFReturnCode native_VodSelectComponent(VodHandle id, AVComponentHandle component)
{
#if defined(OIPF) || defined(HBBTV_1_2_1)
	MAKE_VOD_OBJECT(id, vodObj);
	AVComponentData 	*pstCompData= static_cast<AVComponentData *>(component);

	OOIF_LOG_DEBUG("[%s:%d] called with component: (%d)", __FUNCTION__, __LINE__, component);

	if (( !vodObj ) || ( !pstCompData))
	{
		return OOIF_RETURN_ERROR;
	}

	if (!vodObj->setActiveComponent((eOplAVComponentType)pstCompData->type, (unsigned int)pstCompData->pid, (char*)pstCompData->audio_language))
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
#else
	return OOIF_RETURN_NOT_SUPPORTED;
#endif
}

OOIFReturnCode native_VodUnselectComponent(VodHandle id, AVComponentHandle component)
{
	return OOIF_RETURN_NOT_SUPPORTED;
}

#if defined(OIPF) || defined(HBBTV_1_2_1)
OOIFReturnCode native_VodSelectComponent2(VodHandle id, OOIFNumber component)
{
	OOIF_LOG_DEBUG("VOD selectComponent called with component: %d", static_cast<int>(component));
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodUnselectComponent2(VodHandle id, OOIFNumber component)
{
	OOIF_LOG_DEBUG("VOD unselectComponent called with component: %d", static_cast<int>(component));
	return OOIF_RETURN_OK;
}
#endif // OIPF || HBBTV_1_2_1

OOIFReturnCode native_VodQueue(VodHandle id, const char* url, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("VOD queue called with url %s", url ? url : "NULL");
	*retval = 1;
	return OOIF_RETURN_OK;

}

#if defined(HBBTV_PVR) || defined(HBBTV_DOWNLOAD) || defined(OIPF)
OOIFReturnCode native_VodSetSource(VodHandle identifier, const char* id, OOIFBoolean *retval)
{
	MAKE_VOD_OBJECT(identifier, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	// TODO : check valid ids - Download.id, Recording.id, CODAsset.uid
	vodObj->setUrl(id);

	// TODO : check below condition.
	// OIPF 2.1 - 7.14.7 If the content item represented by id can be accepted by the A/V
	// control object for playback, the method returns true. The method returns false if
	// the item cannot be accepted for playback.
	*retval = TRUE;

	OOIF_LOG_DEBUG("native_VodSetSource");
	return OOIF_RETURN_OK;
}
#endif /* HBBTV_PVR || HBBTV_DOWNLOAD || OIPF */

#ifdef OIPF
OOIFReturnCode native_VodGetPlaySpeeds(VodHandle id, int *count, OOIFNumber **playSpeeds)
{
	OOIF_LOG_DEBUG("Vod get property playSpeeds called, returning [ 1.0, 0.25 ]");
	*playSpeeds = NULL;
	*count = 0;

	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	int speed_count = vodObj->getPlaySpeedCount();
	if ( speed_count <= 0 )
	{
		return OOIF_RETURN_OK;
	}

	int *speeds_array = NULL;
	if ( !vodObj->getPlaySpeeds(&speeds_array) )
	{
		return OOIF_RETURN_OK;
	}

	OOIFNumber *speeds = (OOIFNumber*)HLIB_STD_MemAlloc(sizeof(OOIFNumber)*speed_count);
	if ( speeds == NULL )
	{
		delete []speeds_array;
		return OOIF_RETURN_OK;
	}

	for ( int i=0; i<speed_count; ++i )
	{
		speeds[i] = speeds_array[i];
	}

	delete []speeds_array;

	*playSpeeds = speeds;
	*count = speed_count;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetOitfSourceIPAddress(VodHandle id, const char **retval)
{
	OOIF_LOG_DEBUG("Vod get property oitfSourceIPAddress, returning string: 127.0.0.1");
	*retval = "127.0.0.1";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetOitfSourcePortAddress(VodHandle id, const char **retval)
{
	OOIF_LOG_DEBUG("Vod get property oitfSourcePortAddress, returning string: 1234");
	*retval = "1234";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetOitfNoRTSPSessionControl(VodHandle id, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("Vod get property oitfNoRTSPSessionControl, returning boolean: false");
	*retval = false;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetOitfNoRTSPSessionControl(VodHandle id, OOIFBoolean value)
{
	OOIF_LOG_DEBUG("Vod set property oitfNoRTSPSessionControl, value %d", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetOitfRTSPSessionId(VodHandle id, const char **retval)
{
	OOIF_LOG_DEBUG("Vod get property oitfRTSPSessionId, returning string: abcdefg");
	*retval = "abcdefg";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetOitfRTSPSessionId(VodHandle id, const char *value)
{
	OOIF_LOG_DEBUG("Vod set property oitfRTSPSessionId, value %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetReadyToPlay(VodHandle identifier, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("Vod getReadyToPlay called");
	*retval = FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetAvailableRepresentationsBandwidth(VodHandle id, int *count, OOIFNumber **bandwidths)
{
	OOIF_LOG_DEBUG("Vod get property availableRepresentationsBandwidth called, returning [ 128, 768, 1024 ]");

	OOIFNumber *bd = (OOIFNumber*)HLIB_STD_MemAlloc(sizeof(OOIFNumber)*3);

	bd[1] = 128.0;
	bd[1] = 768.0;
	bd[2] = 1024.0;

	*bandwidths = bd;
	*count = 2;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetCurrentRepresentation(VodHandle id, OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("Vod get property currentRepresentation called, returning 768");
	*retval = 768;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetMaxRepresentation(VodHandle id, OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("Vod get property maxRepresentation called, returning 1024");
	*retval = 1024;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetMinRepresentation(VodHandle id, OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("Vod get property minRepresentation called, returning 128");
	*retval = 128;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetSupportedStrategies(VodHandle id, int *count, char ***strategies)
{
	OOIF_LOG_DEBUG("Vod get property supportedStrategies called, returning [sustained_playback, low_latency]");
	char **st = (char**)HLIB_STD_MemAlloc(sizeof(char*)*2);
	st[0] = HLIB_STD_StrDup("sustained_playback");
	st[1] = HLIB_STD_StrDup("low_latency");
	*strategies = st;
	*count = 2;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetAvailablePlayTime(VodHandle indentifier, OOIFBoolean fromPlayPosition, OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("Vod getAvailablePlayTime called with fromPlayPosition: %s", fromPlayPosition ? "TRUE" : "FALSE");
	if (fromPlayPosition)
	{
		*retval = 0;
	}
	else
	{
		*retval = 1000;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetBufferingStrategy(VodHandle indentifier, const char *name, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("Vod setBufferingStrategy called with name: %s", name);
	if (!strcmp(name, "sustained_playback"))
	{
		*retval = TRUE;
	}
	else
	{
		*retval = FALSE;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetRepresentationStrategy(VodHandle id, OOIFNumber maxBandwidth, OOIFNumber minBandwidth, OOIFNumber position, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("Vod setRepresentationStrategy called with maxBandwidth: %f, minBandwidth: %f, position: %f", maxBandwidth, minBandwidth, position);
	*retval = true;
	return OOIF_RETURN_OK;
}
#endif /* OIPF */

OOIFReturnCode native_VodSetVisibility(VodHandle id, OOIFBoolean visibility, OOIFBoolean unloading)
{
	OOIF_LOG_DEBUG("VOD setVisibility called with visability: %s and unloading: %s",
			visibility ? "true" : "false", unloading ? "true" : "false");

	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->setVisibility(visibility) )
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetVideoWindow(VodHandle id, int x, int y, int width, int height)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->setVideoPosition(x, y, width, height) )
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetLoop(VodHandle id, const char* value)
{
	OOIF_LOG_DEBUG("VOD set loop was called, value: %s", value ? value : "NULL");
	return OOIF_RETURN_OK;
}

#if defined(HMX_VOD_PDL)
OOIFReturnCode native_VodGetBufferingProgress(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	unsigned int buffering_percent = vodObj->getBufferingPercent();

	double db_percent = buffering_percent/100.0;
	*retval = db_percent;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetBufferingTime(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	unsigned int buffering_progress = vodObj->getBufferingRemainedTime();

	*retval = static_cast<OOIFNumber>(buffering_progress);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetPdlChunks(VodHandle id, const char **retval)
{
	static string s_chunk_str;
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !OplLocalSystem::getInternalHddEnabled() )
	{
		*retval = "{pdlActive:false}";
		return OOIF_RETURN_OK;
	}

	unsigned int *start_times = NULL, *end_times = NULL;
	int count = vodObj->getBufferingInfo(&start_times, &end_times);
	if ( count <= 0 )
	{
		*retval = "{pdlActive:false}";
		return OOIF_RETURN_OK;
	}

	s_chunk_str = "{pdlActive:true, chunks:[";
	for ( int i=0; i<count; ++i )
	{
		char buf[64] = {0,};
		snprintf(buf, sizeof(buf), "{start:%d, end:%d},", (start_times[i]/1000), (end_times[i]/1000));
		s_chunk_str += buf;
	}

	s_chunk_str[s_chunk_str.size()] = '\0';
	s_chunk_str += "]}";

	*retval = s_chunk_str.c_str();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetReadyState(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	double ready_state = 0;

	eOplVodBufferState state = vodObj->getBufferState();
	switch ( state )
	{
	case OPLVOD_BUFFER_STATE_NOT_ENOUGH:
		ready_state = 31;
		break;
	case OPLVOD_BUFFER_STATE_CANPLAY:
	case OPLVOD_BUFFER_STATE_ENOUGH:
		ready_state = 33;
		break;
	case OPLVOD_BUFFER_STATE_FINISHED:
		ready_state = 34;
		break;
	case OPLVOD_BUFFER_STATE_NONE:
	default:
		break;
	}

	*retval = ready_state;

	return OOIF_RETURN_OK;
}
#endif

#if defined(HMX_VOD_MEDIA_SBTL)
OOIFReturnCode native_VodSetSubtitleCurrLanguage(VodHandle id, const char *lang)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	char strlang[32], i;
	memset(strlang, 0, 32);

	for(i=0; i<4; i++)
	{
		if ( !vodObj->getSubtitleLanguage(i, strlang) )
		{
			return OOIF_RETURN_VALUE_UNDEFINED;
		}

		if(strcmp(lang, strlang) == 0)
		{
			if ( !vodObj->setSubtitleLanguage(i) )
			{
				return OOIF_RETURN_VALUE_UNDEFINED;
			}
			return OOIF_RETURN_OK;
		}
	}

	return OOIF_RETURN_ERROR;
}

OOIFReturnCode native_VodSetSubtitleSyncTime(VodHandle id, OOIFNumber time)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->setSubtitleSyncTime(static_cast<int>(time)) )
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_VodSetSubtitleFontSize(VodHandle id, OOIFNumber size)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->setSubtitleFontSize(static_cast<int>(size)) )
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_VodSetSubtitleTextPosition(VodHandle id, OOIFNumber pos)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->setSubtitleTextPosition(static_cast<int>(pos)) )
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetSubtitleDisplay(VodHandle id, OOIFBoolean show)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	int bDisplay = 0;

	if(show == true)
		bDisplay = 1;
	else
		bDisplay = 0;

	if ( !vodObj->setSubtitleDisplay(static_cast<int>(bDisplay)) )
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetSubtitleDisplay(VodHandle id, OOIFBoolean *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	bool bDisplay = vodObj->getSubtitleDisplay();

	*retval = bDisplay;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_VodGetSubtitleTotalLanguageCount(VodHandle id, OOIFBoolean *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int count = vodObj->getSubtitleLanguageCount();

	*retval = ( count == 0 ) ? false : true;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetSubtitleCurrLanguage(VodHandle id, const char **retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int nCurridx = vodObj->getSubtitleActiveLanguageIndex();

	static char strlang[32];
	memset(strlang, 0, 32);

	if ( !vodObj->getSubtitleLanguage(nCurridx, strlang))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = strlang;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetSubtitleSyncTime(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int time = vodObj->getSubtitleSyncTime();

	*retval = static_cast<OOIFNumber>(time);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetSubtitleFontSize(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int size = vodObj->getSubtitleFontSize();

	*retval = static_cast<OOIFNumber>(size);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetSubtitleTextPosition(VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int pos = vodObj->getSubtitleTextPosition();

	*retval = static_cast<OOIFNumber>(pos);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetSubtitleSupportedLanguage(VodHandle id, int *count, char ***retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int lang_count = vodObj->getSubtitleLanguageCount();
	if ( lang_count <= 0 )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (char**)HLIB_STD_MemAlloc(sizeof(char*)*lang_count);
	if ( !(*retval) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	char lang[32] = {0, };

	for ( int i = 0 ; i<lang_count; ++i )
	{
		memset(lang, 0, 32);
		vodObj->getSubtitleLanguage(i, lang);
		(*retval)[i] = HLIB_STD_StrDup(lang);
	}

	*count = lang_count;

	/* all allocated memory will be freed by StringCollection */

	return OOIF_RETURN_OK;
}
#endif

#ifdef JLABS
OOIFReturnCode native_VodGetJlabsPbProgramme(VodHandle id, jlabsPbProgrammeHandle *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if (!vodObj)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	jlabsPbProgrammeData	*pstData = NULL;
	OplVodProgramme_t	 	stOplVodProgramme;

	*retval = NULL;
	memset(&stOplVodProgramme, 0, sizeof(OplVodProgramme_t));
	if (vodObj->getProgramme(&stOplVodProgramme))
	{
		pstData = (jlabsPbProgrammeData *)HLIB_STD_MemAlloc(sizeof(jlabsPbProgrammeData));
		if (NULL == pstData)
		{
			goto END_FUNC;
		}

		memset(pstData, 0, sizeof(jlabsPbProgrammeData));

		/* 여기서 strdup에 의해 malloc된 memory는 jlabsPbProgramme class의 releaseNative()에서 해제 된다. */
		pstData->svcuid = stOplVodProgramme.svcuid;			/* To get channel class */
		pstData->eventId = stOplVodProgramme.eventId;		/* programmeID */
		pstData->parentalRating = stOplVodProgramme.parentalRating;
		if (NULL != stOplVodProgramme.name)
		{
			pstData->name = HLIB_STD_StrDup(stOplVodProgramme.name);
			HLIB_STD_MemFree(stOplVodProgramme.name);
		}
		if (NULL != stOplVodProgramme.description)
		{
			pstData->description = HLIB_STD_StrDup(stOplVodProgramme.description);
			HLIB_STD_MemFree(stOplVodProgramme.description);
		}
		if (NULL != stOplVodProgramme.longDescription)
		{
			pstData->longDescription = HLIB_STD_StrDup(stOplVodProgramme.longDescription);
			HLIB_STD_MemFree(stOplVodProgramme.longDescription);
		}
		pstData->startTime = stOplVodProgramme.startTime;
		pstData->duration = stOplVodProgramme.duration;
		pstData->channelID = NULL;
		pstData->programmeID = NULL;
		pstData->programmeIDType = eOPL_ProgrammeID_DVB_EVENT;
		pstData->hasRecording = stOplVodProgramme.hasRecording;
		pstData->subtitles = stOplVodProgramme.subtitles;
		pstData->isHD = stOplVodProgramme.isHD;
		pstData->content = stOplVodProgramme.content;
		pstData->digitalCopyControl = stOplVodProgramme.digitalCopyControl;
		pstData->analogCopyControl = stOplVodProgramme.analogCopyControl;
		pstData->is3D = stOplVodProgramme.is3D;
		pstData->isCharged = stOplVodProgramme.isCharged;
		if (NULL != stOplVodProgramme.contractVertificationInfo)
		{
			pstData->contractVertificationInfo = HLIB_STD_StrDup(stOplVodProgramme.contractVertificationInfo);
			HLIB_STD_MemFree(stOplVodProgramme.contractVertificationInfo);
		}
		pstData->recordingStatus = stOplVodProgramme.recordingStatus;

		*retval = (jlabsPbProgrammeHandle)pstData;
	}
	else
	{
		return OOIF_RETURN_VALUE_NULL;
	}

END_FUNC:

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodShowVideoMessage (VodHandle id, OOIFNumber type, OOIFBoolean *pbRetval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->showVideoMessage (type))
	{
		*pbRetval = FALSE;
	}
	else
	{
		*pbRetval = TRUE;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodHideVideoMessage (VodHandle id, OOIFNumber type, OOIFBoolean *pbRetval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->hideVideoMessage (type))
	{
		*pbRetval = FALSE;
	}
	else
	{
		*pbRetval = TRUE;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodStartBML (VodHandle id)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->startBML ())
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodEndBML (VodHandle id)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vodObj->endBML ())
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodSetBMLKeyControlMode (VodHandle id, OOIFBoolean permit, OOIFNumber *pbRetval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	int	retVal = -1;
	vodObj->setBMLKeyControlMode (permit, &retVal);
	*pbRetval = retVal;

	return OOIF_RETURN_OK;
}

#endif /* End of #ifdef JLABS */

#if defined(HMX_WEBUI) && !defined(JLABS)
OOIFReturnCode native_VodGetPlayingThumbnail(VodHandle id, OOIFNumber pos, const char **retval)
{
	MAKE_VOD_OBJECT(id, vodObj);
	if ( !vodObj )
	{
		*retval = NULL;
		return OOIF_RETURN_ERROR;
	}

	*retval = vodObj->getPlayingThumbnail(pos);
	if ( *retval == NULL )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	return OOIF_RETURN_OK;
}
#endif

#if defined(HMX_WEBUI)
OOIFReturnCode native_VodOffComponent (VodHandle id, OOIFNumber componentType, OOIFNumber p1)
{
	MAKE_VOD_OBJECT(id, vodObj);

	OOIF_LOG_DEBUG("VOD selectComponent called with componentType: %d", (int)componentType);

	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if (!vodObj->offComponent((eOplAVComponentType)componentType, (unsigned int)p1))
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;

}

OOIFReturnCode native_VodGetTrickRestrictMode (VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);

	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	OplVodTrickRestrictMode_e eMode;
	if (!vodObj->getTrickRestrictMode(&eMode))
	{
		return OOIF_RETURN_ERROR;
	}

	*retval = static_cast<OOIFNumber>(eMode);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_VodGetStorageRight (VodHandle id, OOIFNumber *retval)
{
	MAKE_VOD_OBJECT(id, vodObj);

	if ( !vodObj )
	{
		return OOIF_RETURN_ERROR;
	}

	OplVodStorageRight_e eRight;
	if (!vodObj->getStorageRight(&eRight))
	{
		return OOIF_RETURN_ERROR;
	}

	*retval = static_cast<OOIFNumber>(eRight);

	return OOIF_RETURN_OK;
}

#endif



