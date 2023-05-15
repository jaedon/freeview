// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2012 Opera Software ASA. All rights reserved.
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
#include "oplbookmark.h"

#ifdef USE_VIDEOHANDLE
	#define MAKE_VB_OBJECT(in, out) OplVideoBroadcastHandle* out = reinterpret_cast<OplVideoBroadcastHandle*>(in);
#else
	#define MAKE_VB_OBJECT(in, out) OplVideoBroadcast* out = static_cast<OplVideoBroadcast *>(in);
#endif

using namespace Ooif;

static videoBroadcastListener *g_VideoBroadcastListener = NULL;

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

#ifdef JLABS
static void _broadcast_onMailEvent(void *hBroadcast, eOplBroadcastEventType type)
{
	VideoBroadcastHandle		id = (VideoBroadcastHandle)hBroadcast;
	MAKE_VB_OBJECT(id, vbcastObj);
	BroadcastEventInfo		info;
	ENTRY;

	if(vbcastObj == NULL)
	{
		return;
	}
	memset(&info, 0x00, sizeof(BroadcastEventInfo));

	switch ( type )
	{
		case OPLBROADCAST_EVENT_TYPE_MAIL_NEW_MEESAGE_EVENT:
		{
			OPL_MailHandle handle = 0;
			if(vbcastObj->getNewMailMsg(&handle) == false)
			{
				return;
			}
			else
			{
				if( handle != 0 )
				{
					info.emm_notify_new_message_data.type = OPL_Mail_GetMsgType( handle );
					info.emm_notify_new_message_data.msg_id = OPL_Mail_GetMsgId( handle );
					info.emm_notify_new_message_data.new_msg = (jlabsEmmMessageHandle)handle;

					_broadcast_EmitEvent(BROADCAST_EVENT_EMM_NOTIFY_NEW_MESSAGE, &info, id);
				}
			}
			break;
		}
		default:
			break;

	}
}

static void _broadcast_onCasBcEvent(void *hBroadcast, eOplBroadcastEventType type)
{
	VideoBroadcastHandle		id = (VideoBroadcastHandle)hBroadcast;
	MAKE_VB_OBJECT(id, vbcastObj);
	BroadcastEventInfo	info;
	ENTRY;

	if(vbcastObj == NULL)
	{
		return;
	}
	memset(&info, 0x00, sizeof(BroadcastEventInfo));

	switch ( type )
	{
		case OPLBROADCAST_EVENT_TYPE_CAS_BC_DRM_ERROR_NOTIFY:
			{
			unsigned int			ulErrorNumber = 0x2100;//BCAS_RETURN_CODE_NOMAL_END
			OOIF_LOG_INFO("OPLBROADCAST_EVENT_TYPE_CAS_BC_DRM_ERROR_NOTIFY\n");
			/*정상종료*/
			info.drm_error_notify_data.error_number = ulErrorNumber;
			info.drm_error_notify_data.drm_system_id = NULL;
			if(vbcastObj->getDrmError(&ulErrorNumber, (char **)&(info.drm_error_notify_data.drm_system_id)) == false)
				return;
			info.drm_error_notify_data.error_number = static_cast<OOIFNumber>(ulErrorNumber);
			OOIF_LOG_INFO("info.drm_error_notify_data.error_number 0x%x\n",(unsigned int)info.drm_error_notify_data.error_number);
			OOIF_LOG_INFO("info.drm_error_notify_data.drm_system_id %s\n",info.drm_error_notify_data.drm_system_id);
			_broadcast_EmitEvent(BROADCAST_EVENT_DRM_ERROR_NOTIFY, &info, id);
			if(vbcastObj->releaseDrmError((char *)(info.drm_error_notify_data.drm_system_id)) == false)
				return;

			}
			break;
		case OPLBROADCAST_EVENT_TYPE_DRM_RIGHTS_ERROR_EVENT:
			{
			unsigned int			ulErrorState = 3;
			OOIF_LOG_INFO("OPLBROADCAST_EVENT_TYPE_DRM_RIGHTS_ERROR_EVENT\n");
			/*정상종료*/
			info.drm_rights_error_data.error_state = ulErrorState;
			info.drm_rights_error_data.content_id = NULL;
			info.drm_rights_error_data.drm_system_id = NULL;
			info.drm_rights_error_data.rights_issuer_url = NULL;
			if(vbcastObj->getDrmRightError(&ulErrorState,
										(char **)&(info.drm_rights_error_data.content_id),
										(char **)&(info.drm_rights_error_data.drm_system_id),
										(char **)&(info.drm_rights_error_data.rights_issuer_url)) == false)
				return;
			info.drm_rights_error_data.error_state = static_cast<OOIFNumber>(ulErrorState);
			_broadcast_EmitEvent(BROADCAST_EVENT_DRM_RIGHTS_ERROR_EVENT, &info, id);
			if(vbcastObj->releaseDrmRightError((char *)(info.drm_rights_error_data.content_id),
										(char *)(info.drm_rights_error_data.drm_system_id),
										(char *)(info.drm_rights_error_data.rights_issuer_url)) == false)
				return;
			}
			break;
		case OPLBROADCAST_EVENT_TYPE_CAS_BC_EMM_AUTO_DISP_MSG_NOTIFY:
			OOIF_LOG_INFO("OPLBROADCAST_EVENT_TYPE_CAS_BC_EMM_AUTO_DISP_MSG_NOTIFY\n");
			info.emm_auto_display_message_notify_data.adm_info = NULL;
			if(vbcastObj->getAutoDispMsg(&(info.emm_auto_display_message_notify_data.adm_info)) == false)
			{
				OOIF_LOG_ERROR("getAutoDispMsg returned err\n");
				return;
			}
			OOIF_LOG_INFO("getAutoDispMsg returned ok\n");
			_broadcast_EmitEvent(BROADCAST_EVENT_EMM_AUTO_DISPLAY_MESSAGE_NOTIFY, &info, id);
			break;

		case OPLBROADCAST_EVENT_TYPE_CAS_BC_CA_LINK_MSG_NOTIFY:
			{
			unsigned int tsId = 0;
			unsigned int onId = 0;
			unsigned int serviceId = 0;
			unsigned char *pszCaLinkMsg = NULL;
			info.ca_link_message_data.channel = NULL;
			if((vbcastObj->getCaLinkMsg(&onId, &tsId, &serviceId, &pszCaLinkMsg) == false) || (pszCaLinkMsg == NULL))
			{
				OOIF_LOG_ERROR("getCaLinkMsg returned err\n");
				return;
			}
			info.ca_link_message_data.channel = OPL_Channel_Clone(OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), onId, tsId, serviceId));
			if(info.ca_link_message_data.channel == NULL)
			{
				OOIF_LOG_ERROR("OPL_Channel_FindChannelByTriplet returned err\n");
				return;
			}
			info.ca_link_message_data.message = (char*)pszCaLinkMsg;
			_broadcast_EmitEvent(BROADCAST_EVENT_CA_LINK_MSG_NOTIFY, &info, id);
			}
			break;
		case OPLBROADCAST_EVENT_TYPE_CAS_BC_BS_CONTROL_START:
			_broadcast_EmitEvent(BROADCAST_EVENT_BS_CONTROL_STARTED, NULL, id);
			break;
		default:
			OOIF_LOG_ERROR("INVALID EVENT\n");
			break;
	}

	return;
}

static void _broadcast_onBmlEvent(void *hBroadcast, eOplBroadcastEventType type)
{
	VideoBroadcastHandle		id = (VideoBroadcastHandle)hBroadcast;
	MAKE_VB_OBJECT(id, vbcastObj);
	BroadcastEventInfo		info;

	ENTRY;

	if(vbcastObj == NULL)
	{
		return;
	}

	memset(&info, 0x00, sizeof(BroadcastEventInfo));

	switch ( type )
	{
		case OPLBROADCAST_EVENT_TYPE_BML_VOD:
		{
			OPL_HANDLE handle = 0;
			OOIF_LOG_INFO("OPLBROADCAST_EVENT_TYPE_BML_VOD\n");
			if(vbcastObj->getBmlVodHandle(&handle) == false)
			{
				return;
			}
			else
			{
				if( handle != 0 )
				{
					vbcastObj->getBmlVodName(&info.bml_vod.appName);
					vbcastObj->getBmlVodParam(&info.bml_vod.param );
					_broadcast_EmitEvent(BROADCAST_EVENT_BML_VOD, &info, id);
					HLIB_STD_MemFree(info.bml_vod.appName);
					HLIB_STD_MemFree(info.bml_vod.param);
					HLIB_STD_MemFree(handle);
				}
			}
			break;
		}
		default:
			break;

	}
}

#else //JLABS
static void _broadcast_onCasEvent(void *hBroadcast, eOplBroadcastEventType type)
{
	VideoBroadcastHandle		id = (VideoBroadcastHandle)hBroadcast;
	BroadcastEventInfo			info;
	MAKE_VB_OBJECT(id, vbcastObj);
	unsigned int				nEvt = 0, module = 0, slot = 0, session = 0, message_length = 0;
	char						*message = NULL;

	if(vbcastObj == NULL)
	{
		return;
	}

	bool bResult = false;
	memset(&info, 0x00, sizeof(BroadcastEventInfo));

	switch ( type )
	{
		case OPLBROADCAST_EVENT_TYPE_CAS_MESSAGE_EVENT:
			OOIF_LOG_DEBUG("CASMessageTrigger\n");
			bResult = vbcastObj->getCasMessage(
				(unsigned int *)&(module),
				(unsigned int *)&(slot),
				(unsigned int *)&(session),
				(char **)&(message), 
				(unsigned int *)&message_length);
			if((bResult == false) || (message == NULL))
			{
				return;
			}

			info.cas_message_notify_data.module = (OOIFNumber)module;
			info.cas_message_notify_data.slot = (OOIFNumber)slot;
			info.cas_message_notify_data.session = (OOIFNumber)session;
			info.cas_message_notify_data.message = (const char *)message;
			info.cas_message_notify_data.message_length = (OOIFNumber)message_length;
			_broadcast_EmitEvent(BROADCAST_EVENT_CASMESSAGE_NOTIFY, &info, id);

			if(vbcastObj->releaseCasMessage((char *)(info.cas_message_notify_data.message)) == false)
				return;
			break;

		case OPLBROADCAST_EVENT_TYPE_CAS_UI_TRIGGERED:
			OOIF_LOG_DEBUG("CASUiTrigger\n");
			bResult = vbcastObj->getCasUiEvtInfo(
				(unsigned int *)&(nEvt),
				(unsigned int *)&(module),
				(unsigned int *)&(slot),
				(unsigned int *)&(session));
				//(unsigned int *)&(info.cas_message_notify_data.module),
				//(unsigned int *)&(info.cas_message_notify_data.slot));
			if(bResult == false)
				return;

			info.cas_message_notify_data.module = (OOIFNumber)module;
			info.cas_message_notify_data.slot = (OOIFNumber)slot;
			info.cas_message_notify_data.session = (OOIFNumber)session;

			_broadcast_EmitEvent(BROADCAST_EVENT_CASUI_NOTIFY, &info, id);
			break;

		default:
			OOIF_LOG_DEBUG("INVALID EVENT\n");
			break;
	}

	return;
}
#endif //JLABS

static void _broadcast_onLiveChange(void *hBroadcast, eOplBroadcastEventType type)
{
	VideoBroadcastHandle	 id = (VideoBroadcastHandle)hBroadcast;
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( vbcastObj == NULL ) return;

	BroadcastEventInfo info;

	switch ( type )
	{
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

	case OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED:
		info.channel_change_succeed_data.channel = NULL;
		native_BroadcastGetCurrentChannel (id, &(info.channel_change_succeed_data.channel));
		_broadcast_EmitEvent(BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED,
			&info, id);
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
#ifdef JLABS
		 if( vbcastObj->getEwsChannel( (OPL_Channel_t*)&info.ews.channel ) == TRUE )
		 {
		 	if( vbcastObj->getEwsCurTuner( (HINT32*)&info.ews.tuner ) == TRUE )
		 	{
				_broadcast_EmitEvent(BROADCAST_EVENT_EWS_NOTIFY, &info, id);
		 	}
		 }
		 else
		 {
			OOIF_LOG_ERROR("OPLBROADCAST_EVENT_TYPE_EWS_NOTIFY : Not Found Channel \n");
		 }
#endif
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
	case OPLBROADCAST_EVENT_TYPE_TRICK_RESTRICT_MODE_CHANGED:
	{
		OOIF_LOG_DEBUG("TrickRestrictModeChanged\n");
		_broadcast_EmitEvent(BROADCAST_EVENT_TRICK_RESTRICT_MODE_CHANGED, NULL, id);
		break;
	}

	case OPLBROADCAST_EVENT_TYPE_BML_START:
#ifdef JLABS
		 if( vbcastObj->getBmlStartFlag( &info.bml_start.autoFlag ) == TRUE )
		 {
		 	// TODO: sbkim, Tuner정보 가져오기
			_broadcast_EmitEvent(BROADCAST_EVENT_BML_START, &info, id);
		 }
		 else
		 {
			OOIF_LOG_ERROR("OPLBROADCAST_EVENT_TYPE_BML_START :  \n");
		 }
#endif
		break;

	case OPLBROADCAST_EVENT_TYPE_BML_END:
	{
#ifdef JLABS
		 int	 cause;
		 if( vbcastObj->getBmlEndParam( &cause, info.bml_end.param) == TRUE )
		 {
			info.bml_end.cause = cause;
			_broadcast_EmitEvent(BROADCAST_EVENT_BML_END, &info, id);
		 }
		 else
		 {
			OOIF_LOG_ERROR("OPLBROADCAST_EVENT_TYPE_BML_END :  \n");
		 }
#endif
	}
		break;

	case OPLBROADCAST_EVENT_TYPE_SI_RCT_CHANGED:
	{
//		OOIF_LOG_PRINT("\n\n##### sh ##### [%s][%d] OPLBROADCAST_EVENT_TYPE_SI_RCT_CHANGED\n",__FUNCTION__,__LINE__);
		if( vbcastObj->getRctInfo(&info.rct_info.rctInfo) == TRUE)
		{
			if(info.rct_info.rctInfo == NULL)
			{
//				OOIF_LOG_PRINT("##### sh ##### [%s][%d] info.rct_info.rctInfo NULL \n",__FUNCTION__,__LINE__);
			}
			_broadcast_EmitEvent(BROADCAST_EVENT_SI_RCT_CHANGED, &info, id);
		}
	}
		break;

	case OPLBROADCAST_EVENT_TYPE_REQUEST_PINCODE:
		if( vbcastObj->getRequestPincodeMsg(&info.request_pincode_data.msg) == TRUE)
		{
			_broadcast_EmitEvent(BROADCAST_EVENT_REQUEST_PINCODE, &info, id);
		}
		vbcastObj->releaseRequestPincodeMsg(info.request_pincode_data.msg);
		break;

	case OPLBROADCAST_EVENT_TYPE_NCD_CHANGED:
		if( vbcastObj->getNcdChangedMsg(&info.ncd_changed_info.ncdmessage,(int *)&info.ncd_changed_info.startTimeOfChange,(int *)&info.ncd_changed_info.ASODuration) == TRUE)
		{

			_broadcast_EmitEvent(BROADCAST_EVENT_SI_NCD_CHANGED, &info, id);

		}
		vbcastObj->releaseNcdChangedMsg(info.ncd_changed_info.ncdmessage);
		break;
		
	case OPLBROADCAST_EVENT_TYPE_NETWORK_URI_CHANGED:
		if( vbcastObj->getNetworkUri(&info.network_uri_info.networkUri) == TRUE)
		{
			_broadcast_EmitEvent(BROADCAST_EVENT_SI_NETWORK_URI_CHANGED, &info, id);
		}
		vbcastObj->releaseNetworkUri(info.network_uri_info.networkUri);
		break;
	case OPLBROADCAST_EVENT_TYPE_TSR_STATE_CHANGED:
		if( vbcastObj->getTsrPaused(&info.tsr_status.isPaused) == TRUE)
		{
			_broadcast_EmitEvent(BROADCAST_EVENT_TSR_STATE_CHANGED, &info, id);
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
	case OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE:
	{
		OplBroadcastPlayState_e eState = OPLBROADCAST_PLAY_STATE_UNREALIZED;
		if(vbcastObj->getPlayState(&eState) == TRUE)
		{
			info.playstate_change_data.has_error = TRUE;
			info.playstate_change_data.error = static_cast<OOIFNumber>(vbcastObj->getErrorType());
			if(info.playstate_change_data.error == OPLBROADCAST_CHANNEL_ERROR_NO_ERROR)
			{
				info.playstate_change_data.has_error = FALSE;
			}
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
			//OOIF_LOG_PRINT("_broadcast_onTsrChange PLAYSTATE_CHANGE error[%d]\n", info.playstate_change_data.error);
			_broadcast_EmitEvent(BROADCAST_EVENT_PLAYSTATE_CHANGE, &info, id);
		}
		break;
	}
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
		break;
	}
	case OPLBROADCAST_EVENT_TYPE_DRM_RIGHTS_ERROR_EVENT:
	{
		unsigned int			ulErrorState = 3;
		OOIF_LOG_INFO("OPLBROADCAST_EVENT_TYPE_DRM_RIGHTS_ERROR_EVENT\n");
		/*정상종료*/
		info.drm_rights_error_data.error_state = ulErrorState;
		info.drm_rights_error_data.content_id = NULL;
		info.drm_rights_error_data.drm_system_id = NULL;
		info.drm_rights_error_data.rights_issuer_url = NULL;
		if(vbcastObj->getDrmRightError(&ulErrorState,
									(char **)&(info.drm_rights_error_data.content_id),
									(char **)&(info.drm_rights_error_data.drm_system_id),
									(char **)&(info.drm_rights_error_data.rights_issuer_url)) == false)
			return;
		info.drm_rights_error_data.error_state = static_cast<OOIFNumber>(ulErrorState);
		_broadcast_EmitEvent(BROADCAST_EVENT_DRM_RIGHTS_ERROR_EVENT, &info, id);
		if(vbcastObj->releaseDrmRightError((char *)(info.drm_rights_error_data.content_id),
									(char *)(info.drm_rights_error_data.drm_system_id),
									(char *)(info.drm_rights_error_data.rights_issuer_url)) == false)
			return;
		break;
	}
	case OPLBROADCAST_EVENT_TYPE_SUPPORTED_PLAY_SPEED_CHANGED:
	{
		OOIF_LOG_DEBUG("PlaySpeedsArrayChanged\n");
		_broadcast_EmitEvent(BROADCAST_EVENT_PLAY_SPEEDS_ARRAY_CHANGED, NULL, id);
		break;
	}
	case OPLBROADCAST_EVENT_TYPE_TRICK_RESTRICT_MODE_CHANGED:
	{
		OOIF_LOG_DEBUG("TrickRestrictModeChanged\n");
		_broadcast_EmitEvent(BROADCAST_EVENT_TRICK_RESTRICT_MODE_CHANGED, NULL, id);
		break;
	}
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

	default:
		break;
	}
}

static void _broadcast_onMhegEvent(void *hBroadcast, eOplBroadcastEventType type)
{
	VideoBroadcastHandle	 id = (VideoBroadcastHandle)hBroadcast;
	MAKE_VB_OBJECT(id, vbcastObj);

	BroadcastEventInfo info;

	if ( vbcastObj == NULL ) return;

	OOIF_LOG_PRINT("\n##ysmoon ## [%s, %d] ==== \n",__FUNCTION__, __LINE__);

	switch ( type )
	{
	case OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE:
		{
			unsigned int	ulMhegSvcUid;
			unsigned int	ulMhegTuneMode;
			OPL_Channel_t	hMhegChannel;

			if (vbcastObj->getTuneInfoByMheg(&ulMhegSvcUid, &ulMhegTuneMode, &hMhegChannel)== false )
				return;
		
			OOIF_LOG_DEBUG("OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE ==== MhegSvcUid : %d , MhegTuneMode = 0x%x, hMhegChannel = %p\n", ulMhegSvcUid, ulMhegTuneMode, hMhegChannel);
			info.mheg_tune_info.svcUid = ulMhegSvcUid;
			info.mheg_tune_info.tuneMode = ulMhegTuneMode;
			info.mheg_tune_info.channel = hMhegChannel;

			_broadcast_EmitEvent(BROADCAST_EVENT_MHEG_REQUEST_TUNE,&info, id);
		}
		break;
		case OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE:
			{
				unsigned int ulMhegInterruptFlag;
		
				if (vbcastObj->getMhegInterruptFlag(&ulMhegInterruptFlag) == false ) return;
		
				OOIF_LOG_DEBUG("OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE ==== ulMhegInterruptFlag : %d \n", ulMhegInterruptFlag);
		
				info.mheg_interrupt_info.interruptFlag = ulMhegInterruptFlag;
		
				_broadcast_EmitEvent(BROADCAST_EVENT_MHEG_INTERRUPT,&info, id);
			}
			break;
		case OPLBROADCAST_EVENT_TYPE_MHEG_ICS_STATUS:
			{
				unsigned int ulMhegIcsStatus;
				
				if (vbcastObj->getMhegIcsFlag(&ulMhegIcsStatus) == false )
					return;
		
				OOIF_LOG_DEBUG("OPLBROADCAST_EVENT_TYPE_MHEG_ICS_STATUS ==== ulMhegIcsStatus : %d \n\n",ulMhegIcsStatus);
		
				info.mheg_ics_status.mhegIcsFlag = ulMhegIcsStatus;
		
				_broadcast_EmitEvent(BROADCAST_EVENT_MHEG_ICS_STAUS,&info, id);
			}
			break;
	default:
		break;
	}

}
OOIFReturnCode native_BroadcastCreate(VideoBroadcastHandle *id, long int window_identifier, const char *host_uri)
{
#ifdef USE_VIDEOHANDLE
	OplVideoBroadcastHandle *vbcastObj = OplVideoBroadcastHandle::create(OplVideoHandle::EHandleOwnerTypeUserOipf);
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
#if defined(JLABS)
	vbcastObj->setCasEventListener(_broadcast_onCasBcEvent);
	vbcastObj->setMailEventListener( _broadcast_onMailEvent);
	vbcastObj->setBmlEventListener( _broadcast_onBmlEvent);
#else
	vbcastObj->setCasEventListener(_broadcast_onCasEvent);
#endif
	vbcastObj->setRecEventListener(_broadcast_onRecEvent);

	return OOIF_RETURN_OK;
}

#ifdef USE_VIDEOHANDLE
void native_BroadcastRegisterListener(OplVideoBroadcastHandle *vbcastObj)
{
	vbcastObj->setLiveEventListener(_broadcast_onLiveChange);
	vbcastObj->setTsrEventListener(_broadcast_onTsrChange);
#if defined(JLABS)
	vbcastObj->setCasEventListener(_broadcast_onCasBcEvent);
	vbcastObj->setMailEventListener( _broadcast_onMailEvent);
#else
	vbcastObj->setCasEventListener(_broadcast_onCasEvent);
#endif
	vbcastObj->setRecEventListener(_broadcast_onRecEvent);
	vbcastObj->setMhegEventListener(_broadcast_onMhegEvent);
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

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastSetVideoWindow(VideoBroadcastHandle id, int x, int y, int width, int height)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_DEBUG("[VRM] [%d] native_BroadcastSetVideoWindow\n");
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
	*retval = "data";
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

	*retval = static_cast<OOIFNumber>(eState);

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_BroadcastSetPlayState(VideoBroadcastHandle id, OOIFNumber playState)
{
	OOIF_LOG_DEBUG("Broadcast playState set to %d", (int)playState);
	
	return OOIF_RETURN_OK;
}
#endif // OIPF

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

	OPL_Channel_t	channel = vbcastObj->getCurrentChannel();
	if ( !channel )
	{
		return OOIF_RETURN_VALUE_NULL;
	}
	*retval = (ChannelHandle)OPL_Channel_Clone(channel);
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
#if 0
	for (int i = 0; i < dsd_len; i++)
	{
		OOIF_LOG_DEBUG("\t%02x", dsd[i]);
	}

	if (idType >= 0)
	{
		*retval = (ChannelHandle)1;
		return OOIF_RETURN_OK;
	}
#endif
	return OOIF_RETURN_VALUE_NULL;
}

OOIFReturnCode native_BroadcastCreateChannelObject6(VideoBroadcastHandle id, OOIFNumber idType, OOIFBoolean has_onid, OOIFNumber onid, OOIFBoolean has_tsid, OOIFNumber tsid, OOIFBoolean has_sid, OOIFNumber sid, OOIFBoolean has_sourceID, OOIFNumber sourceID, const char *ipBroadcastID, ChannelHandle *retval)
{
	OOIF_LOG_DEBUG(" native_BroadcastCreateChannelObject6 called with: %f %d %f %d %f %d %f %d %f %s",
	               idType, has_onid, onid, has_tsid, tsid, has_sid, sid, has_sourceID, sourceID, ipBroadcastID ? ipBroadcastID : "NULL");

	if (idType >= 0)
	{
		// TODO: 휘발성 채널을 만들어 튜닝할 수 있는 함수...
		// TODO: 아래 루틴만으론 OCTO 내부에 SVC_Info_t가 없기 때문에 튜닝되지 않음
#if 0
		OplChannel *channel = new OplChannel();
		if ( !channel )
		{
			return OOIF_RETURN_OK;
		}

		channel->eChannelID = static_cast<eOplChannelID>(idType);
		if ( has_onid ) channel->usOrgNetID = onid;
		if ( has_tsid ) channel->usTSID = tsid;
		if ( has_sid ) channel->usServiceID = sid;

		*retval = channel;
		return OOIF_RETURN_OK;
#endif
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

	if ( !vbcastObj->setChannel((OPL_Channel_t)channel) )
	{
		return OOIF_RETURN_ERROR;
	}

	// This is temporary, we have to consider, this is right?
/*
	BroadcastEventInfo info;
	info.channel_change_succeed_data.channel = NULL;
	VideoBroadcast::onVideoBroadcastEvent(BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED,
			&info, id);
*/
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastSetChannelWithTuneInfo(VideoBroadcastHandle id, ChannelHandle channel, OOIFNumber tuneInfo)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}
	if ( !vbcastObj->setChannel(channel, (unsigned int)tuneInfo) )
	{
		return OOIF_RETURN_ERROR;
	}

	// This is temporary, we have to consider, this is right?
/*
	BroadcastEventInfo info;
	info.channel_change_succeed_data.channel = NULL;
	VideoBroadcast::onVideoBroadcastEvent(BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED,
			&info, id);
*/
	return OOIF_RETURN_OK;
}

#ifdef HMX_WEBUI
OOIFReturnCode native_BroadcastSetChannelFCC(VideoBroadcastHandle id, ChannelHandle channel, OOIFNumber viewID)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}
	if ( !vbcastObj->setChannelFCC((OPL_Channel_t)channel, (unsigned int)viewID) )
	{
		return OOIF_RETURN_ERROR;
	}

	// This is temporary, we have to consider, this is right?
/*
	BroadcastEventInfo info;
	info.channel_change_succeed_data.channel = NULL;
	VideoBroadcast::onVideoBroadcastEvent(BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED,
			&info, id);
*/
	return OOIF_RETURN_OK;
}
#endif

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

#if defined(HMX_WEBUI)
OOIFReturnCode native_BroadcastStopByViewID(VideoBroadcastHandle id, int viewID)
{
	OOIF_LOG_DEBUG("Broadcast stop");
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	vbcastObj->stop(viewID);

	return OOIF_RETURN_OK;
}
#endif

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
				pstCompData->aspectRatio	= stOplComponent.fAspectRatio;
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
		// OOIF_LOG_PRINT (">>>>> Type(%d) AVComponentHandle(%d)\n", (HUINT32)pstCompData->type, (HUINT32)pstCompData);
		handles[ulCompCnt++] = (AVComponentHandle)pstCompData;
	}
	// OOIF_LOG_PRINT ("=== End of Component ===\n\n\n\n\n\n\n");

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

#if defined(HMX_WEBUI)
	pstCompData->subType		= stOplComponent.eSubType;
#endif
	if (0 < strlen( stOplComponent.szLanguage))   pstCompData->audio_language 	= HLIB_STD_StrDup (stOplComponent.szLanguage);

#if defined(HMX_WEBUI)
	if (0 < strlen( stOplComponent.szLanguage2)) pstCompData->language2 	= HLIB_STD_StrDup (stOplComponent.szLanguage2);
#endif

	pstCompData->audioDescription = stOplComponent.bAudioDescription;
	pstCompData->audioChannels = stOplComponent.ulAudioChannels;

#ifdef JLABS
	pstCompData->componentType = stOplComponent.ulComponentType;
	pstCompData->streamType = stOplComponent.ulStreamType;
	pstCompData->associatedContentFlag = stOplComponent.bAssociatedContentFlag;
	pstCompData->dataEntryComponentFlag = stOplComponent.bDataEntryComponentFlag;
	if (0 < strlen( stOplComponent.szLanguage2)) pstCompData->language2 	= HLIB_STD_StrDup (stOplComponent.szLanguage2);
	if (0 < strlen( stOplComponent.szText)) pstCompData->text		= HLIB_STD_StrDup (stOplComponent.szText);
#endif // JLABS

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
#if defined(HMX_WEBUI)
			if (NULL != pstCompData->language2) 			{ HLIB_STD_MemFree(pstCompData->language2); }
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

	//OOIF_LOG_PRINT ("\n\n\n\n\nSelect Component (%d) PID=%d, componentTag=%d\n\n\n\n\n",(unsigned int)component, (unsigned int)pstCompData->pid, (unsigned int)pstCompData->componentTag);

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
#ifdef JLABS
	AVComponentHandle *handles = NULL;
	int count = 0;
#endif
	OOIF_LOG_DEBUG("Video Broadcast unselectComponent called with component: %d", (int)component);

	if (( !vbcastObj ) || ( !pstCompData))
	{
		return OOIF_RETURN_ERROR;
	}

#ifdef JLABS
	/* 스펙에서 요구하는 prefered 필드는 사용하지 안하서 무조건 첫째로 defalt로 돌린다. */
	native_BroadcastGetComponents( id , (BroadcastComponentType)pstCompData->type, &count, &handles );
	if( count > 0)
	{
		pstCompData = ( AVComponentData *)handles[0];
	}

	//OOIF_LOG_PRINT ("\n\n\n\n\nSelect Component (%d) PID=%d, componentTag=%d\n\n\n\n\n",(unsigned int)component, (unsigned int)pstCompData->pid, (unsigned int)pstCompData->componentTag);

  	if (!vbcastObj->setCurrentComponent((eOplAVComponentType)pstCompData->type, (unsigned int)pstCompData->pid))
	{
		return OOIF_RETURN_ERROR;
	}
#endif

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
#endif // HBBTV_PVR || OIPF

#if defined(HMX_WEBUI)
OOIFReturnCode native_BroadcastRecordNowAt(VideoBroadcastHandle id, OOIFNumber duration, OOIFNumber target, const char **retval)
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

	if ( !vbcastObj->recordNowAt(vbcastObj->getCurrentChannel(), static_cast<unsigned int>(duration), static_cast<unsigned int>(target), szRecordId))
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

OOIFReturnCode native_BroadcastRecordDelayed(VideoBroadcastHandle id, OOIFNumber startPos, OOIFNumber duration, const char **retval)
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

	if ( !vbcastObj->recordDelayed(vbcastObj->getCurrentChannel(), static_cast<unsigned int>(startPos), static_cast<unsigned int>(duration), szRecordId) )
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
	if (szRecordId) 								{ HLIB_STD_MemFree(szRecordId); }
	return eRet;
}

OOIFReturnCode native_BroadcastRecordDelayedAt(VideoBroadcastHandle id, OOIFNumber startPos, OOIFNumber duration, OOIFNumber target, const char **retval)
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

	if ( !vbcastObj->recordDelayedAt(vbcastObj->getCurrentChannel(), static_cast<unsigned int>(startPos), static_cast<unsigned int>(duration), static_cast<unsigned int>(target), szRecordId) )
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
	if (szRecordId) 								{ HLIB_STD_MemFree(szRecordId); }
	return eRet;
}
OOIFReturnCode native_BroadcastCreateChannelObject3lcn(VideoBroadcastHandle id, OOIFNumber idType, OOIFNumber majorChannel, OOIFNumber minorChannel, ChannelHandle *retval)
{
	OOIF_LOG_DEBUG("native_BroadcastCreateChannelObject3lcn called with: %f %f", majorChannel, minorChannel);
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	OPL_Channel_t	channel = vbcastObj->createChannelObject(idType, majorChannel, minorChannel);
	if ( !channel )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*retval = (ChannelHandle)OPL_Channel_Clone(channel);

	return OOIF_RETURN_OK;
}

#endif // #if defined(HMX_WEBUI)

#if defined(HMX_WEBUI)
OOIFReturnCode native_BroadcastCheckChannelChangable (VideoBroadcastHandle id, ChannelHandle channel, OOIFBoolean *pbChangable)
{
	int					 bChangable = FALSE;
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_OK;
	}

	if ( !vbcastObj->checkChannelChangable (channel, &bChangable))
	{
		return OOIF_RETURN_OK;
	}

	*pbChangable = bChangable;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastCheckChannelRecordable (VideoBroadcastHandle id, ChannelHandle channel, OOIFBoolean bIgnoreLive, OOIFBoolean *pbChangable)
{
	int					 bChangable = FALSE;
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_OK;
	}

	if ( !vbcastObj->checkChannelRecordable (channel, bIgnoreLive, &bChangable))
	{
		return OOIF_RETURN_OK;
	}

	*pbChangable = bChangable;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetPlayingThumbnail (VideoBroadcastHandle id, OOIFNumber pos, const char **retval)
{
	if ( NULL == retval )
	{
		return OOIF_RETURN_ERROR;
	}

	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		*retval = NULL;
		return OOIF_RETURN_ERROR;
	}
	*retval = vbcastObj->getPlayingThumbnail(pos);

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastResultPincode (VideoBroadcastHandle id, OOIFBoolean permit, OOIFNumber *pbRetval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	int					retVal = -1;

	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	vbcastObj->resultPincode (permit, &retVal);
	*pbRetval = retVal;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_BroadcastGetStartOffset(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	unsigned int startOffset = 0;

	if ( !vbcastObj->getStartOffset(&startOffset) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	*retval = static_cast<OOIFNumber>(startOffset);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetTrickRestrictMode(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplBroadcastTrickRestrictMode_e eMode;
	if ( !vbcastObj->getTrickRestrictMode(&eMode) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = static_cast<OOIFNumber>(eMode);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetTsrPausedPoints(VideoBroadcastHandle id, int *count, BookmarkHandle **handles)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	HUINT32				ulNum = 0;
	OPL_Bookmark_t		**ppstBookmarks = NULL;

	//OPL_Broadcast_GetTsrPausedPoints
	if ( !vbcastObj->getTsrPausedPoints(&ulNum, &ppstBookmarks) )
	{
		*handles = NULL;
		*count = 0;
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*handles = (BookmarkHandle *)ppstBookmarks;
	*count = ulNum;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetTsrPaused(VideoBroadcastHandle id, OOIFBoolean *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	HBOOL	bIsPaused;
	if ( !vbcastObj->getTsrPaused(&bIsPaused) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = bIsPaused ? TRUE : FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetStorageRight(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplBroadcastStorageRight_e eRight;
	if ( !vbcastObj->getStorageRight(&eRight) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = static_cast<OOIFNumber>(eRight);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastGetMaxDuration(VideoBroadcastHandle id, OOIFNumber *retval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	unsigned int maxDuration = 0;

	if ( !vbcastObj->getMaxDuration(&maxDuration) )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	*retval = static_cast<OOIFNumber>(maxDuration);
	return OOIF_RETURN_OK;
}

#endif

#if defined(HMX_WEBUI)
OOIFReturnCode native_BroadcastOffComponent (VideoBroadcastHandle id, OOIFNumber componentType, OOIFNumber p1)
{
	MAKE_VB_OBJECT(id, vbcastObj);

	OOIF_LOG_DEBUG("Video Broadcast selectComponent called with componentType: %d", (int)componentType);

	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	//OOIF_LOG_PRINT ("\n\n\n\n\n native_BroadcastOffComponent componentType=%d, p1=%d\n\n\n\n\n",(unsigned int)componentType, (unsigned int)p1);

	if (!vbcastObj->offComponent((eOplAVComponentType)componentType, (unsigned int)p1))
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;

}

OOIFReturnCode native_BroadcastRequestThumbnail(VideoBroadcastHandle id, const char *url,OOIFNumber time, const char **retval)
{
	const char *szRet = NULL;
	MAKE_VB_OBJECT(id, vbcastObj);

	OOIF_LOG_DEBUG("Video Broadcast request thumbnail called with url:[%s],time[%d]", url,(int)time);

	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	szRet = vbcastObj->requestThumbnail(url,(int)time);
	if(szRet == NULL)
		*retval = NULL;
	else
		*retval = szRet;

	return OOIF_RETURN_OK;
}

OOIFReturnCode	native_BroadcastCheckAitSection(VideoBroadcastHandle id)
{
	MAKE_VB_OBJECT(id, vbcastObj);

	OOIF_LOG_DEBUG("Video Broadcast CheckAitSection");

	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	vbcastObj->CheckAitSection();

	return OOIF_RETURN_OK;
}
#endif

#ifdef JLABS
OOIFReturnCode native_BroadcastGetMultiViewComponents(VideoBroadcastHandle id, int *count, jlabsMultiViewComponentHandle **components)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	HUINT32						 	ulIndex, ulCompCnt = 0, ulCompNum = 0;
	jlabsMultiViewComponentData		*pstCompData;
	jlabsMultiViewComponentHandle	*handles = NULL;
	OplMultiViewComponent_t			 stOplComponent;

	OOIF_LOG_DEBUG("Video Broadcast getComponents called with type");

	*components = NULL;
	*count = 0;

	if (!vbcastObj->countMultiViewComponent(&ulCompNum))
	{
		goto END_FUNC;
	}

	if (0 == ulCompNum)						{ goto END_FUNC; }

	handles = (jlabsMultiViewComponentHandle*)HLIB_STD_MemAlloc (sizeof(jlabsMultiViewComponentHandle) * ulCompNum);
	if (NULL == handles)					{ goto END_FUNC; }
	memset (handles, 0, sizeof(jlabsMultiViewComponentHandle) * ulCompNum);

	OOIF_LOG_DEBUG ("\n\n\n=== Component Num (%d) ===\n", ulCompNum);

	for (ulIndex = 0; ulIndex < ulCompNum; ulIndex++)
	{
		if (!vbcastObj->getMultiViewComponent(ulIndex, &stOplComponent))
		{
			continue;
		}
		pstCompData = (jlabsMultiViewComponentData *)HLIB_STD_MemAlloc (sizeof(jlabsMultiViewComponentData));
		memset( pstCompData , 0x00, sizeof(jlabsMultiViewComponentData));

		native_jlabsMultiViewComponentConvert( (void *)&stOplComponent, pstCompData );

		OOIF_LOG_DEBUG("\n\n\n===[native_BroadcastGetMultiViewComponents] index (%d) compNum(%d) name(%s)===\n",
						pstCompData->index, pstCompData->compNum, pstCompData->name);

		handles[ulCompCnt++] = (jlabsMultiViewComponentHandle)pstCompData;
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
			native_jlabsMultiViewComponentReleaseHandle( (jlabsMultiViewComponentHandle )handles[ulIndex] );
		}

		HLIB_STD_MemFree(handles);
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastShowVideoMessage (VideoBroadcastHandle id, OOIFNumber type, OOIFBoolean *pbRetval)
{
	MAKE_VB_OBJECT(id, vbcastObj);

	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vbcastObj->showVideoMessage (type))
	{
		*pbRetval = FALSE;
	}
	else
	{
		*pbRetval = TRUE;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastHideVideoMessage (VideoBroadcastHandle id, OOIFNumber type, OOIFBoolean *pbRetval)
{
	MAKE_VB_OBJECT(id, vbcastObj);

	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vbcastObj->hideVideoMessage (type))
	{
		*pbRetval = FALSE;
	}
	else
	{
		*pbRetval = TRUE;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastStartBML (VideoBroadcastHandle id)
{
	MAKE_VB_OBJECT(id, vbcastObj);

	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vbcastObj->startBML ())
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastEndBML (VideoBroadcastHandle id)
{
	MAKE_VB_OBJECT(id, vbcastObj);

	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( !vbcastObj->endBML ())
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BroadcastSetBMLKeyControlMode (VideoBroadcastHandle id, OOIFBoolean permit, OOIFNumber *pbRetval)
{
	MAKE_VB_OBJECT(id, vbcastObj);
	int					retVal = -1;

	if ( !vbcastObj )
	{
		return OOIF_RETURN_ERROR;
	}

	vbcastObj->setBMLKeyControlMode (permit, &retVal);
	*pbRetval = retVal;

	return OOIF_RETURN_OK;
}

#endif


