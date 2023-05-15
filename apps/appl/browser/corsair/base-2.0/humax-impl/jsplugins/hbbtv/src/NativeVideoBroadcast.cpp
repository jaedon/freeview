// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "NativeVideoBroadcast.h"
#include "VideoBroadcast.h"

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_VIDEOHANDLE
#include "oplvideobroadcasthandle.h"
#else
#include "oplbroadcast.h"
#endif
#include "oplprogramme.h"
#include "oplmetadatasearch.h"
#include "oplavcomponent.h"
#include "oplmail.h"

#include "hapi.h"

#ifdef USE_VIDEOHANDLE
	#define MAKE_VB_OBJECT(in, out) OplVideoBroadcastHandle* out = reinterpret_cast<OplVideoBroadcastHandle*>(in);
#else
	#define MAKE_VB_OBJECT(in, out) OplVideoBroadcast* out = static_cast<OplVideoBroadcast *>(in);
#endif

using namespace Ooif;

static videoBroadcastListener *g_VideoBroadcastListener = NULL;
bool g_OnNullService = false;

OOIFReturnCode native_BroadcastGetCurrentChannel(VideoBroadcastHandle id, ChannelHandle *retval);

#if 0
static void native_BroadcastChannelChangedListener(const void *private_data)
{
#if 0
	VideoBroadcastHandle id = (VideoBroadcastHandle)private_data;
	BroadcastEventInfo info;
	info.channel_change_succeed_data.channel = NULL;
	VideoBroadcast::onVideoBroadcastEvent(BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED,
			&info, id);
#endif
	OOIF_LOG_DEBUG("native_BroadcastChannelChangedListener is called.");
}
#endif

static void _broadcast_EmitEvent (BroadcastEvent event, BroadcastEventInfo *info, VideoBroadcastHandle id)
{
	if (g_VideoBroadcastListener)
	{
		g_VideoBroadcastListener(event, info, id);
	}
}


static void _broadcast_onLiveChange(void *hBroadcast, eOplBroadcastEventType type)
{
	VideoBroadcastHandle	 id = (VideoBroadcastHandle)hBroadcast;
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( vbcastObj == NULL ) return;

	BroadcastEventInfo info;

	switch ( type )
	{
	case OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED:
		info.channel_change_succeed_data.channel = NULL;
		native_BroadcastGetCurrentChannel (id, &(info.channel_change_succeed_data.channel));
		_broadcast_EmitEvent(BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED, &info, id);
		break;

	case OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR:
		info.channel_change_error_data.channel = NULL;
		native_BroadcastGetCurrentChannel (id, &(info.channel_change_succeed_data.channel));
		info.channel_change_error_data.error_state = vbcastObj->getErrorType();

		_broadcast_EmitEvent(BROADCAST_EVENT_CHANNEL_CHANGE_ERROR,
				&info, id);
		break;
	case OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE:
		{
			OplBroadcastPlayState_e eState = OPLBROADCAST_PLAY_STATE_UNREALIZED;

			if(vbcastObj->getPlayState(&eState) == TRUE)
			{
				info.playstate_change_data.has_error = FALSE;

				/*
				if(eState == OPLBROADCAST_PLAY_STATE_PRESENTING)
				{
					info.playstate_change_data.has_error = FALSE;
				}
				else
				{
					info.playstate_change_data.has_error = TRUE;
					info.playstate_change_data.error = static_cast<OOIFNumber>(vbcastObj->getErrorType());
					if(info.playstate_change_data.error == OPLBROADCAST_CHANNEL_ERROR_NO_ERROR)
					{
						info.playstate_change_data.has_error = FALSE;
					}
				}
				*/

				switch(eState)
				{
					case OPLBROADCAST_PLAY_STATE_CONNECTING:
						info.playstate_change_data.state = BROADCAST_PLAYSTATE_CONNECTING;
						break;
					case OPLBROADCAST_PLAY_STATE_PRESENTING:
						info.playstate_change_data.state = BROADCAST_PLAYSTATE_PRESENTING;
						break;
					case OPLBROADCAST_PLAY_STATE_STOPPED:
						info.playstate_change_data.state = BROADCAST_PLAYSTATE_STOPPED;
						break;
					default:
						info.playstate_change_data.state = BROADCAST_PLAYSTATE_UNREALIZED;
						break;
				}
				_broadcast_EmitEvent(BROADCAST_EVENT_PLAYSTATE_CHANGE, &info, id);
			}
		}
		break;
	case OPLBROADCAST_EVENT_TYPE_PROGRAMMES_CHANGED:
		_broadcast_EmitEvent(BROADCAST_EVENT_PROGRAMMES_CHANGED, &info, id);
		break;

	case OPLBROADCAST_EVENT_TYPE_EVENT_RELAY_NOTIFY:
#ifdef JLABS
		 if( vbcastObj->getEventRelayChannel( (OPL_Channel_t*)&info.event_relay.channel ) == TRUE )
		 {
			_broadcast_EmitEvent(BROADCAST_EVENT_EVENT_RELAY_NOTIFY, &info, id);
		 }
		 else
		 {
			OOIF_LOG_ERROR("OPLBROADCAST_EVENT_TYPE_EVENT_RELAY_NOTIFY : Not Found Channel \n");
		 }
#endif
		break;

	case OPLBROADCAST_EVENT_TYPE_EWS_NOTIFY:
		break;

#if defined(OIPF) || defined(HBBTV_1_2_1)
	case OPLBROADCAST_EVENT_TYPE_PARENTAL_RATING_CHANGE:
		{
			unsigned int rating = 0;
			memset(&info, 0x00, sizeof(BroadcastEventInfo));
			if( vbcastObj->getParentalRatingChangeStatus((char **)&(info.parental_rating_change_data.contentID),
												(HBOOL*)&(info.parental_rating_change_data.blocked),
												&rating,
												(char **)&(info.parental_rating_change_data.DRMSystemID)) == TRUE )
			{
				OPL_ParentalRating_t			*parentalRating = NULL;
				parentalRating = OPL_PCMAN_PRC_AddParentalRating(NULL, NULL, NULL, rating, 0, NULL);
				info.parental_rating_change_data.ratings = (ParentalRatingHandle *)parentalRating;
			 	_broadcast_EmitEvent(BROADCAST_EVENT_PARENTAL_RATING_CHANGE, &info, id);
			}
			else
			{
			   OOIF_LOG_ERROR("OPLBROADCAST_EVENT_TYPE_PARENTAL_RATING_CHANGE : Not Found Channel \n");
			}
			vbcastObj->releaseParentalRatingChangeStatus((char *)(info.parental_rating_change_data.contentID),
												(char*)(info.parental_rating_change_data.DRMSystemID));
		}
		break;
#endif
	// HBBTV_2_0
	case OPLBROADCAST_EVENT_TYPE_COMPONENT_CHANGED:
		{
			if ( vbcastObj->getChangedComponent((eOplAVComponentType *)&info.component_changed_data.componentType) == TRUE)
			{
				_broadcast_EmitEvent(BROADCAST_EVENT_COMPONENT_CHANGED, &info, id);
			}
		}
		break;

	default:
		break;
	}
}



static void _broadcast_onTsrChange(void *hBroadcast, eOplBroadcastEventType type)
{
	VideoBroadcastHandle	 id = (VideoBroadcastHandle)hBroadcast;
	MAKE_VB_OBJECT(id, vbcastObj);

	if ( vbcastObj == NULL ) return;

	BroadcastEventInfo info;

	switch ( type )
	{
#if defined(HBBTV_PVR) || defined(OIPF)
	case OPLBROADCAST_EVENT_TYPE_PLAY_SPEED_CHANGED:
	{
		OOIF_LOG_DEBUG("PlaySpeedChanged\n");
		HINT32 speed = 0;
		if ( vbcastObj->getPlaySpeed(&speed) == false ) return;

		info.play_speed_changed_data.speed = (double)(speed / (double)100.0);	// OIPF (-32/-16/-8/-4/-2/0/1/2/4/8/16/32):OPL(OIPF value*100)
		OOIF_LOG_DEBUG("	OPLBROADCAST_EVENT_TYPE_PLAY_SPEED_CHANGED (%d) - oipfSpeed (%f)\n", speed, info.play_speed_changed_data.speed);
		_broadcast_EmitEvent(BROADCAST_EVENT_PLAY_SPEED_CHANGED,
			&info, id);
		break;
	}
	case OPLBROADCAST_EVENT_TYPE_PLAY_POSITION_CHANGED:
	{
		OOIF_LOG_DEBUG("PlayPositionChange\n");
		unsigned int playbackOffset = 0;
		if ( vbcastObj->getPlaybackOffset(&playbackOffset) == false ) return;

		info.play_position_changed_data.has_position = 1;
		info.play_position_changed_data.position = playbackOffset;
		_broadcast_EmitEvent(BROADCAST_EVENT_PLAY_POSITION_CHANGED,
			&info, id);
		break;
	}
#endif // #if defined(HBBTV_PVR) || defined(OIPF)
	default:
		break;
	}
}

static void _broadcast_onRecEvent(void *hBroadcast, eOplBroadcastEventType type)
{
	VideoBroadcastHandle	 id = (VideoBroadcastHandle)hBroadcast;
	MAKE_VB_OBJECT(id, vbcastObj);

	if ( vbcastObj == NULL ) return;

	BroadcastEventInfo info;

	switch ( type )
	{
	case OPLBROADCAST_EVENT_TYPE_RECORDING_EVENT:
	{
		char					 *pszRecordId;
		eOplRecorderState		 eRecState;

		OOIF_LOG_DEBUG("RecordingEvent\n");
		if ( vbcastObj->getRecordingState(&eRecState) == false ) return;

		pszRecordId = (char *)HLIB_STD_MemAlloc(OPLRECORDER_MAX_RECORDID_LEN);
		if(!pszRecordId)
			return;

		memset(pszRecordId, 0, OPLRECORDER_MAX_RECORDID_LEN);
		if (vbcastObj->getRecordId(pszRecordId) != true)
		{
			pszRecordId[0] = '\0';
		}

		info.recording_event_data.state = (OOIFNumber)eRecState;
		info.recording_event_data.has_error = (OOIFBoolean)false;
		info.recording_event_data.recordingId = (const char *)pszRecordId;

		OOIF_LOG_DEBUG("	BROADCAST_EVENT_RECORDING_EVENT (%d), recordID[%s]\n", eRecState, pszRecordId);
		_broadcast_EmitEvent(BROADCAST_EVENT_RECORDING_EVENT, &info, id);
		HLIB_STD_MemFree(pszRecordId);
		break;
	}
#if defined(HBBTV_PVR) || defined(OIPF)
	case OPLBROADCAST_EVENT_TYPE_PLAY_POSITION_CHANGED:
	{
		OOIF_LOG_DEBUG("PlayPositionChange\n");
		unsigned int playbackOffset = 0;
		if ( vbcastObj->getPlaybackOffset(&playbackOffset) == false ) return;

		info.play_position_changed_data.has_position = 1;
		info.play_position_changed_data.position = playbackOffset;
		_broadcast_EmitEvent(BROADCAST_EVENT_PLAY_POSITION_CHANGED,
			&info, id);
		break;
	}
#endif
	default:
		break;
	}
}

OOIFReturnCode native_BroadcastCreate(VideoBroadcastHandle *id, long int window_identifier, const char *host_uri)
{
#ifdef USE_VIDEOHANDLE
	OplVideoBroadcastHandle *vbcastObj = OplVideoBroadcastHandle::create(OplVideoHandle::EHandleOwnerTypeUserHbbtv);
#else
	OplVideoBroadcast *vbcastObj = new OplVideoBroadcast();
#endif

	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*id = vbcastObj;

	vbcastObj->setLiveEventListener(_broadcast_onLiveChange);
	vbcastObj->setTsrEventListener(_broadcast_onTsrChange);
	vbcastObj->setRecEventListener(_broadcast_onRecEvent);
    g_OnNullService = false;

	return OOIF_RETURN_OK;
}

#ifdef USE_VIDEOHANDLE
void native_BroadcastRegisterListener(OplVideoBroadcastHandle *vbcastObj)
{
	vbcastObj->setLiveEventListener(_broadcast_onLiveChange);
	vbcastObj->setTsrEventListener(_broadcast_onTsrChange);
	vbcastObj->setRecEventListener(_broadcast_onRecEvent);
}

#endif

OOIFReturnCode native_BroadcastSetListener(videoBroadcastListener listener)
{
	if(g_VideoBroadcastListener != NULL)
	{
		OOIF_LOG_WARNING("Listener has already been registered\n");
	}

	g_VideoBroadcastListener = listener;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastOnListenerAccessed(VideoBroadcastHandle id, const char *eventName)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastAddStreamEventListener(const char* targetURL,
        const char *eventName,
        void *listenerId,
        videoBroadcastStreamEventListener listener,
        VideoBroadcastHandle id,
        long int windowIdentifier)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_BroadcastAddXMLStreamEventListener(const char *eventName, OOIFNumber eventId, OOIFNumber componentTag, void *listenerId, videoBroadcastStreamEventListener listener, VideoBroadcastHandle id, long int windowIdentifier)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_BroadcastRemoveStreamEventListener(const char* targetURL,
        const char *eventName,
        void *listenerId)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_BroadcastRemoveAllStreamEventListeners(VideoBroadcastHandle id)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_BroadcastReleaseHandle(VideoBroadcastHandle id)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( vbcastObj )
	{
#ifdef USE_VIDEOHANDLE
		OplVideoHandle::unRegist(vbcastObj);
#endif

		//vbcastObj->setChannelHandle(NULL);
		//vbcastObj->setSupportScalerByVideoFrame(false);

#if defined(CONFIG_PROD_FVP4000T)
		vbcastObj->setFullScreen(TRUE);
#endif

		delete vbcastObj;
		vbcastObj = NULL;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastSetVisibility( VideoBroadcastHandle id, OOIFBoolean visibility, OOIFBoolean unloading)
{
	OOIF_LOG_DEBUG("Broadcast setVisibility called with visability: %s and unloading: %s",
			visibility ? "true" : "false", unloading ? "true" : "false");

	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vbcastObj->setVisibility(visibility) )
	{
		return OOIF_RETURN_ERROR;
	}
/*
	if ( unloading )
	{
		vbcastObj->setFullScreen(TRUE);
	}
	else
	{
		vbcastObj->setFullScreen(!visibility);
	}
*/

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastSetVideoWindow(VideoBroadcastHandle id, int x, int y, int width, int height)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	//vbcastObj->setSupportScalerByVideoFrame(true);

	if ( !vbcastObj->setVideoPosition(x, y, width, height) )
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

#if defined(OIPF) || defined(HBBTV_1_2_1)
// Properties
OOIFReturnCode native_BroadcastGetData(VideoBroadcastHandle id, const char **retval)
{
	OOIF_LOG_DEBUG("Video Broadcast data accessed, returning string: data");
	*retval = "";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastSetData(VideoBroadcastHandle id, const char *data)
{
	OOIF_LOG_DEBUG("Broadcast data set to %s", data);
	return OOIF_RETURN_OK;
}
#endif // OIPF || HBBTV_1_2_1

OOIFReturnCode native_BroadcastGetPlayState(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplBroadcastPlayState_e eState;
	if ( !vbcastObj->getPlayState(&eState) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if(eState==OPLBROADCAST_PLAY_STATE_CONNECTING)
	{
		OOIF_LOG_DEBUG("native_BroadcastGetPlayState set to OPLBROADCAST_PLAY_STATE_UNREALIZED");
		eState= OPLBROADCAST_PLAY_STATE_UNREALIZED;
	}

	*retval = static_cast<OOIFNumber>(eState);

	return OOIF_RETURN_OK;
}

//#ifdef OIPF
OOIFReturnCode native_BroadcastSetPlayState(VideoBroadcastHandle id, OOIFNumber playState)
{
	OOIF_LOG_DEBUG("Broadcast playState set to %d", (int)playState);
	MAKE_VB_OBJECT(id, vbcastObj);

	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if( !vbcastObj->setPlayState((OplBroadcastPlayState_e)playState) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if( playState == 0 )
	{
		vbcastObj->setErrorType(OPLBROADCAST_CHANNEL_ERROR_NO_ERROR);
	}

	return OOIF_RETURN_OK;
}
//#endif // OIPF

OOIFReturnCode native_BroadcastGetCurrentChannel(VideoBroadcastHandle id, ChannelHandle *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	OPL_Channel_t	channel = vbcastObj->getCurrentChannel();
	if ( !channel )
	{
	    if(g_OnNullService)
    		return OOIF_RETURN_VALUE_NULL;
        else
            channel = vbcastObj->getLatestChannel();
        if(!channel)
            return OOIF_RETURN_VALUE_NULL;
	}

	*retval = (ChannelHandle)OPL_Channel_Clone(channel);

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_BroadcastBindToCurrentChannel(VideoBroadcastHandle id, ChannelHandle* retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	vbcastObj->bindToCurrentChannel();
	return OOIF_RETURN_OK;
}
#elif defined(HBBTV)
OOIFReturnCode native_BroadcastBindToCurrentChannel(VideoBroadcastHandle id)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	vbcastObj->bindToCurrentChannel();
	return OOIF_RETURN_OK;
}
#endif //HBBTV

// Functions
OOIFReturnCode native_BroadcastCreateChannelObject3(VideoBroadcastHandle id, OOIFNumber idType, const unsigned char *dsd, int dsd_len, OOIFNumber sid, ChannelHandle *retval)
{
	OOIF_LOG_DEBUG(" native_BroadcastCreateChannelObject3 called with dsd:");
	return OOIF_RETURN_VALUE_NULL;
}

OOIFReturnCode native_BroadcastCreateChannelObject6(VideoBroadcastHandle id, OOIFNumber idType, OOIFBoolean has_onid, OOIFNumber onid, OOIFBoolean has_tsid, OOIFNumber tsid, OOIFBoolean has_sid, OOIFNumber sid, OOIFBoolean has_sourceID, OOIFNumber sourceID, const char *ipBroadcastID, ChannelHandle *retval)
{
	OOIF_LOG_DEBUG(" native_BroadcastCreateChannelObject6 called with: %f %d %f %d %f %d %f %d %f %s",
	               idType, has_onid, onid, has_tsid, tsid, has_sid, sid, has_sourceID, sourceID, ipBroadcastID ? ipBroadcastID : "NULL");

	if (idType >= 0)
	{
			MAKE_VB_OBJECT(id, vbcastObj);
		if ( !vbcastObj )
		{
			return OOIF_RETURN_VALUE_NULL;
		}

		OPL_Channel_t	channel = vbcastObj->createChannelObject(idType, (int)onid, (int)tsid, (int)sid);
		if ( !channel )
		{
			return OOIF_RETURN_VALUE_NULL;
		}

		*retval = (ChannelHandle)OPL_Channel_Clone(channel);

		return OOIF_RETURN_OK;
	}
	return OOIF_RETURN_VALUE_NULL;
}

OOIFReturnCode native_BroadcastSetChannel(VideoBroadcastHandle id, ChannelHandle channel, OOIFBoolean has_trickplay, OOIFBoolean trickplay, const char *contentAccessDescriptorURL, OOIFBoolean has_offset, OOIFNumber offset)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

    g_OnNullService = false;

	if ( channel == NULL )
	{
		vbcastObj->release_object();
        g_OnNullService = true;

		/* DTV Labs HbbTV 1.1.1 v0.10 org.hbbtv_00000320 */
		BroadcastEventInfo info;
		info.channel_change_succeed_data.channel = NULL;
		VideoBroadcast::onVideoBroadcastEvent(BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED, &info, id);
	}
	else if ( !vbcastObj->setChannel((OPL_Channel_t)channel) )
	{
		return OOIF_RETURN_ERROR;
	}

	vbcastObj->setChannelHandle((OPL_Channel_t)channel);

	// This is temporary, we have to consider, this is right?
/*
	BroadcastEventInfo info;
	info.channel_change_succeed_data.channel = NULL;
	VideoBroadcast::onVideoBroadcastEvent(BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED,
			&info, id);
*/
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastPrevChannel(VideoBroadcastHandle id)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vbcastObj->prevChannel() )
	{
		return OOIF_RETURN_ERROR;
	}

	/*
	BroadcastEventInfo info;
	info.channel_change_succeed_data.channel = NULL;
	VideoBroadcast::onVideoBroadcastEvent(BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED,
			&info, id);
	*/
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastNextChannel(VideoBroadcastHandle id)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vbcastObj->nextChannel() )
	{
		return OOIF_RETURN_ERROR;
	}

	/*
	BroadcastEventInfo info;
	info.channel_change_succeed_data.channel = NULL;
	VideoBroadcast::onVideoBroadcastEvent(BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED,
			&info, id);
	*/

	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_BroadcastSetFullScreen(VideoBroadcastHandle id, OOIFBoolean fullscreen)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vbcastObj->setFullScreen(fullscreen) )
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_BroadcastSetVolume(VideoBroadcastHandle id, OOIFNumber volume, OOIFBoolean *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vbcastObj->setVolume(volume) )
	{
		return OOIF_RETURN_ERROR;
	}

	*retval = volume+1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetVolume(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	unsigned int volume = 0;
	if ( !vbcastObj->getVolume(&volume) )
	{
		return OOIF_RETURN_ERROR;
	}

	*retval = static_cast<OOIFNumber>(volume);

	return OOIF_RETURN_OK;
}
#endif // OIPF

OOIFReturnCode native_BroadcastRelease(VideoBroadcastHandle id)
{
	MAKE_VB_OBJECT(id, vbcastObj);

	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	//vbcastObj->setSupportScalerByVideoFrame(false);
	vbcastObj->setFullScreen(TRUE);

	vbcastObj->release_object();
	//delete vbcastObj;
	//vbcastObj = NULL;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastStop(VideoBroadcastHandle id)
{
	OOIF_LOG_DEBUG("Broadcast stop");
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	vbcastObj->stop();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetProgrammes(VideoBroadcastHandle id, int *count, ProgrammeHandle **handles)
{
	OOIF_LOG_DEBUG("Video Broadcast getProgrammes called");
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	if (!vbcastObj->getProgrammes(count, (OPL_ProgrammeHandle **)handles))
	{
		*handles = (ProgrammeHandle *)HLIB_STD_StrDup("will be freed");
		*count   = 0;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetComponents(VideoBroadcastHandle id, BroadcastComponentType componentType, int *count, AVComponentHandle **components)
{
#if defined(OIPF) || defined(HBBTV_1_2_1)
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	HUINT32						 ulIndex, ulCompCnt = 0, ulCompNum = 0;
	AVComponentData				*pstCompData;
	AVComponentHandle			*handles = NULL;
	OplAVComponent_t			 stOplComponent;

	OOIF_LOG_DEBUG("Video Broadcast getComponents called with type %d", componentType);

	*components = NULL;
	*count = 0;

	if (!vbcastObj->countComponent((eOplAVComponentType)componentType, &ulCompNum))
	{
		goto END_FUNC;
	}

	if (0 == ulCompNum)						{ goto END_FUNC; }

	handles = (AVComponentHandle*)HLIB_STD_MemAlloc (sizeof(AVComponentHandle) * ulCompNum);
	if (NULL == handles)					{ goto END_FUNC; }
	memset (handles, 0, sizeof(AVComponentHandle) * ulCompNum);

	OOIF_LOG_DEBUG ("\n\n\n=== Component Type (%d), Component Num (%d) ===\n", componentType, ulCompNum);

	for (ulIndex = 0; ulIndex < ulCompNum; ulIndex++)
	{
		if (!vbcastObj->getComponent((eOplAVComponentType)componentType, ulIndex, &stOplComponent))
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
			pstCompData->encoding = HLIB_STD_StrDup (stOplComponent.encoding);

		switch(pstCompData->type){
			case COMPONENT_TYPE_VIDEO:
				pstCompData->has_aspectRatio = TRUE;
				if( (1.77f < stOplComponent.fAspectRatio) && (1.79f > stOplComponent.fAspectRatio) )
					pstCompData->aspectRatio = 1.78;
				else if( (1.32f < stOplComponent.fAspectRatio) && (1.34f > stOplComponent.fAspectRatio) )
					pstCompData->aspectRatio = 1.33;
				else
					pstCompData->aspectRatio = (OOIFNumber)stOplComponent.fAspectRatio;
				break;
			case COMPONENT_TYPE_AUDIO:
				if (0 < strlen( stOplComponent.szLanguage))
					pstCompData->audio_language = HLIB_STD_StrDup (stOplComponent.szLanguage);
				pstCompData->audioDescription = stOplComponent.bAudioDescription;
				pstCompData->audioChannels	= stOplComponent.ulAudioChannels;
#if defined(HMX_WEBUI)
				if (0 < strlen( stOplComponent.szLanguage2)) {
					pstCompData->language2		= HLIB_STD_StrDup (stOplComponent.szLanguage2);
					OOIF_LOG_ERROR("%s:%d Dual Mono audio string(%s)", __FUNCTION__, __LINE__, pstCompData->language2);
				}
#endif
				break;
			case COMPONENT_TYPE_SUBTITLE:
				if (0 < strlen( stOplComponent.szLanguage))
					pstCompData->subtitle_language = HLIB_STD_StrDup (stOplComponent.szLanguage);
				pstCompData->hearingImpaired= stOplComponent.bHardOfHearing_Impaired;
				break;
			default:
				OOIF_LOG_ERROR("%s:%d Unkown type(%d)", __FUNCTION__, __LINE__, pstCompData->type);
				break;
		}

#ifdef JLABS
		pstCompData->componentType = stOplComponent.ulComponentType;
		pstCompData->streamType = stOplComponent.ulStreamType;
		pstCompData->associatedContentFlag = stOplComponent.bAssociatedContentFlag;
		pstCompData->dataEntryComponentFlag = stOplComponent.bDataEntryComponentFlag;
		if (0 < strlen( stOplComponent.szLanguage2)) pstCompData->language2		= HLIB_STD_StrDup (stOplComponent.szLanguage2);
		if (0 < strlen( stOplComponent.szText)) pstCompData->text		= HLIB_STD_StrDup (stOplComponent.szText);
		if( stOplComponent.nMultiViewNum > 0 )
		{
			HINT32 i =0;

			pstCompData->multiview = ( char ** )HLIB_STD_MemAlloc( sizeof(char *) * stOplComponent.nMultiViewNum );
			for( i =0; i< stOplComponent.nMultiViewNum; i++ )
			{
				if (0 < strlen(stOplComponent.szMultiView[i])) 	{ pstCompData->multiview[i]= HLIB_STD_StrDup (stOplComponent.szMultiView[i]); }
			}
		}
		pstCompData->multiviewNum = stOplComponent.nMultiViewNum;
		pstCompData->digitalCopyControl = stOplComponent.nDigitalCopyControl;
#endif // JLABS

		//OOIF_LOG_DEBUG("\n\n\n===[native_BroadcastGetComponents]  PID (%d), LANG (%s) type (%d) subType(%d)===\n", pstCompData->pid, pstCompData->language,pstCompData->type,pstCompData->subType);
		// OOIF_LOG_DEBUG (">>>>> Type(%d) AVComponentHandle(%d)\n", (HUINT32)pstCompData->type, (HUINT32)pstCompData);
		handles[ulCompCnt++] = (AVComponentHandle)pstCompData;
	}
	// OOIF_LOG_DEBUG ("=== End of Component ===\n\n\n\n\n\n\n");

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
				if (NULL != pstCompData->audio_language)
				{
					HLIB_STD_MemFree(pstCompData->audio_language);
				}
#if defined(HMX_WEBUI)
				if (NULL != pstCompData->language2)
				{
					HLIB_STD_MemFree(pstCompData->language2);
				}
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

OOIFReturnCode native_BroadcastGetCurrentActiveComponents(VideoBroadcastHandle id, BroadcastComponentType componentType, int *count, AVComponentHandle **components)
{
#if defined(OIPF) || defined(HBBTV_1_2_1)
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	AVComponentData 			*pstCompData;
	AVComponentHandle			*handles = NULL;
	OplAVComponent_t 	 stOplComponent;

	OOIF_LOG_DEBUG("Video Broadcast getComponents called with type %d", componentType);

	*components = NULL;
	*count = 0;

	handles = (AVComponentHandle*)HLIB_STD_MemAlloc (sizeof(AVComponentHandle));
	if (NULL == handles)					{ goto END_FUNC; }
	memset (handles, 0, sizeof(AVComponentHandle));

	if (!vbcastObj->getCurrentComponent((eOplAVComponentType)componentType, &stOplComponent))
	{
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

	if (0 < strlen( stOplComponent.encoding))
		pstCompData->encoding = HLIB_STD_StrDup (stOplComponent.encoding);

	switch(pstCompData->type){
		case COMPONENT_TYPE_VIDEO:
			pstCompData->has_aspectRatio = TRUE;
			if( (1.77f < stOplComponent.fAspectRatio) && (1.79f > stOplComponent.fAspectRatio) )
				pstCompData->aspectRatio = 1.78;
			else if( (1.32f < stOplComponent.fAspectRatio) && (1.34f > stOplComponent.fAspectRatio) )
				pstCompData->aspectRatio = 1.33;
			else
				pstCompData->aspectRatio = (OOIFNumber)stOplComponent.fAspectRatio;
			break;
		case COMPONENT_TYPE_AUDIO:
			pstCompData->audioDescription = stOplComponent.bAudioDescription;
			pstCompData->audioChannels = stOplComponent.ulAudioChannels;
			break;
		default:
			break;
	}

	if (0 < strlen( stOplComponent.szLanguage))   pstCompData->audio_language 	= HLIB_STD_StrDup (stOplComponent.szLanguage);

	handles[0] = (AVComponentHandle)pstCompData;
	//OOIF_LOG_DEBUG("\n\n\n===[native_BroadcastGetCurrentActiveComponents] PID (%d), LANG (%s) type (%d) subType(%d)===\n", pstCompData->pid, pstCompData->language,pstCompData->type,pstCompData->subType);

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
			HLIB_STD_MemFree(pstCompData);
		}

		HLIB_STD_MemFree(handles);
	}

	return OOIF_RETURN_OK;
#else
	return OOIF_RETURN_NOT_SUPPORTED;
#endif
}

OOIFReturnCode native_BroadcastSelectComponent(VideoBroadcastHandle id, AVComponentHandle component)
{
#if defined(OIPF) || defined(HBBTV_1_2_1)
	MAKE_VB_OBJECT(id, vbcastObj);
	AVComponentData 			*pstCompData = static_cast<AVComponentData *>(component);

	OOIF_LOG_DEBUG("Video Broadcast selectComponent called with component: %d", (int)component);

	if (( !vbcastObj ) || ( !pstCompData))
	{
		return OOIF_RETURN_ERROR;
	}

	//OOIF_LOG_DEBUG ("\n\n\n\n\nSelect Component (%d) PID=%d, componentTag=%d\n\n\n\n\n",(unsigned int)component, (unsigned int)pstCompData->pid, (unsigned int)pstCompData->componentTag);

  	if (!vbcastObj->setCurrentComponent((eOplAVComponentType)pstCompData->type, (unsigned int)pstCompData->pid, (unsigned char*)pstCompData->audio_language))
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
#else
	return OOIF_RETURN_NOT_SUPPORTED;
#endif
}

OOIFReturnCode native_BroadcastUnselectComponent(VideoBroadcastHandle id, AVComponentHandle component)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	AVComponentData 			*pstCompData = static_cast<AVComponentData *>(component);
	OOIF_LOG_DEBUG("Video Broadcast unselectComponent called with component: %d", (int)component);

	if (( !vbcastObj ) || ( !pstCompData))
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

#if defined(OIPF) || defined(HBBTV_1_2_1)
OOIFReturnCode native_BroadcastSelectComponent2(VideoBroadcastHandle id, OOIFNumber component)
{
	OOIF_LOG_DEBUG("Video Broadcast selectComponent called with component: %d", static_cast<int>(component));
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastUnselectComponent2(VideoBroadcastHandle id, OOIFNumber component)
{
	OOIF_LOG_DEBUG("Video Broadcast unselectComponent called with component: %d", static_cast<int>(component));
	return OOIF_RETURN_OK;
}
#endif // OIPF || HBBTV_1_2_1

#if defined(HBBTV_PVR) || defined(OIPF)
OOIFReturnCode native_BroadcastGetPlaybackOffset(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int playbackOffset = 0;
	if ( !vbcastObj->getPlaybackOffset(&playbackOffset) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = static_cast<OOIFNumber>(playbackOffset);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetMaxOffset(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int maxOffset = 0;
	if ( !vbcastObj->getMaxOffset(&maxOffset) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = static_cast<OOIFNumber>(maxOffset);
	return OOIF_RETURN_OK;
}
#endif // HBBTV_PVR || OIPF

OOIFReturnCode native_BroadcastGetRecordingState(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	eOplRecorderState eState;
	if ( !vbcastObj->getRecordingState(&eState) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = static_cast<OOIFNumber>(eState);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetPlayPosition(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplRecorder *recorder = vbcastObj->getRecorder();
	if ( !recorder )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int position = 0;
	if ( !recorder->getPlayPosition(&position) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = static_cast<OOIFNumber>(position);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetPlaySpeed(VideoBroadcastHandle id, OOIFNumber *retval)
{
	double				 fSpeed;
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	signed int speed = 0;
	if ( !vbcastObj->getPlaySpeed(&speed) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	fSpeed = (double)((double)speed / (double)100.0);

	*retval = static_cast<OOIFNumber>(fSpeed);

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_BroadcastGetTimeShiftMode(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int timeShiftMode = 0;
	if ( !vbcastObj->getTimeShiftMode(&timeShiftMode) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = static_cast<OOIFNumber>(timeShiftMode);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastSetTimeShiftMode(VideoBroadcastHandle id, OOIFNumber timeShiftMode)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int nTimeShiftMode = static_cast<unsigned int>(timeShiftMode);
	if ( !vbcastObj->setTimeShiftMode(nTimeShiftMode) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetCurrentTimeShiftMode(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int timeShiftMode = 0;
	if ( !vbcastObj->getCurrentTimeShiftMode(&timeShiftMode) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = static_cast<OOIFNumber>(timeShiftMode);

	return OOIF_RETURN_OK;
}
#endif // OIPF

#if defined(HBBTV_PVR) || defined(OIPF)
OOIFReturnCode native_BroadcastGetPlaySpeeds(VideoBroadcastHandle id, int *count, OOIFNumber **playSpeeds)
{
	MAKE_VB_OBJECT(id, vbcastObj);

	*playSpeeds = NULL;
	*count = 0;

	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int speed_count = vbcastObj->getPlaySpeedCount();
	if ( speed_count <= 0 )
	{
		return OOIF_RETURN_OK;
	}

	int *speeds_array = NULL;
	if ( !vbcastObj->getPlaySpeeds(&speeds_array) )
	{
		return OOIF_RETURN_OK;
	}

	OOIFNumber *speeds = (OOIFNumber*)HLIB_STD_MemCalloc(sizeof(OOIFNumber)*speed_count);
	if ( speeds == NULL )
	{
		delete speeds_array;
		return OOIF_RETURN_OK;
	}

	for ( int i=0; i<speed_count; ++i ) {
		speeds[i] = (OOIFNumber)speeds_array[i];
	}

	delete speeds_array;

	*playSpeeds = speeds;
	*count = speed_count;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastRecordNow(VideoBroadcastHandle id, OOIFNumber duration, const char **retval)
{
	char				*szRet = NULL, *szRecordId = NULL;
	MAKE_VB_OBJECT(id, vbcastObj);
	OOIFReturnCode		 eRet = OOIF_RETURN_VALUE_UNDEFINED;

	if ( !vbcastObj )
	{
		goto END_FUNC;
	}

	szRecordId = (char *)HLIB_STD_MemAlloc (sizeof(char) * 2048);
	if ( !szRecordId )
	{
		goto END_FUNC;
	}

	if ( !vbcastObj->recordNow(vbcastObj->getCurrentChannel(), static_cast<unsigned int>(duration), szRecordId))
	{
		eRet = OOIF_RETURN_VALUE_NULL;
		goto END_FUNC;
	}

	// should check if HLIB_STD_StrDup can be used.
	szRet = HLIB_STD_StrDup(szRecordId);
	if ( !szRet )
	{
		goto END_FUNC;
	}

	*retval = szRet;
	eRet = OOIF_RETURN_OK;

END_FUNC:
	if (szRecordId)									{ HLIB_STD_MemFree(szRecordId); }
	return eRet;
}

OOIFReturnCode native_BroadcastStopRecording(VideoBroadcastHandle id)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if ( !vbcastObj->stopRecording(vbcastObj->getCurrentChannel()) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastPause(VideoBroadcastHandle id, OOIFBoolean *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		*retval = 0;
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplRecorder *recorder = vbcastObj->getRecorder();
	if ( !recorder )
	{
		*retval = 0;
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if ( !recorder->pause() )
	{
		*retval = 0;
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastResume(VideoBroadcastHandle id, OOIFBoolean *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		*retval = 0;
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplRecorder *recorder = vbcastObj->getRecorder();
	if ( !recorder )
	{
		*retval = 0;
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if ( !recorder->resume() )
	{
		*retval = 0;
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastSetSpeed(VideoBroadcastHandle id, OOIFNumber speed, OOIFBoolean *retval)
{
	signed int			 nSpeed;
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		*retval = 0;
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	nSpeed = (signed int)((double)speed * (double)100.0);
	if ( !vbcastObj->setPlaySpeed (nSpeed) )
	{
		*retval = 0;
		return OOIF_RETURN_OK;
	}

	*retval = 1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastSeek(VideoBroadcastHandle id, OOIFNumber offset, BroadcastPosition reference, OOIFBoolean *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		*retval = 0;
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if ( !vbcastObj->seek (static_cast<int>(offset), static_cast<unsigned int>(reference)) )
	{
		*retval = 0;
		return OOIF_RETURN_OK;
	}

	*retval = 1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastStopTimeshift(VideoBroadcastHandle id, OOIFBoolean *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		*retval = 0;
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if ( !vbcastObj->stopTimeshift() )
	{
		*retval = 0;
		return OOIF_RETURN_OK;
	}

	*retval = 1;
	return OOIF_RETURN_OK;
}


#define __RETURN_NULL_IF(cond)      do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
OOIFReturnCode native_BroadcastFindChannelByTriplet(OOIFNumber idType, OOIFNumber onid, OOIFNumber tsid, OOIFNumber sid,  ChannelHandle *retval)
{
    //__RETURN_NULL_IF(!OPL_Channel_IsSupportedChannelID(__toOplIdType(idType)));

    OPL_Channel_t channel = OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), (int)onid, (int)tsid, (int)sid);
    __RETURN_NULL_IF(!channel);

    //*retval = (ChannelHandle)OPL_Channel_Clone(channel);
    *retval = (ChannelHandle)channel;
    __RETURN_NULL_IF((*retval == (ChannelHandle)0));

    return OOIF_RETURN_OK;
}

#endif // HBBTV_PVR || OIPF
