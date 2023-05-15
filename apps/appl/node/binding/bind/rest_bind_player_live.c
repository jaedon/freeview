/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_player_live.c
	@brief

	Description:  									\n
	Module: node/binding/bind		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <apk.h>
#include <rest_maintask.h>
#include <rest_datatype.h>
#include <rest_bind.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___
STATIC HCHAR *rest_bind_play_live_MakeNotifierMsg (HUINT32 ulPlayerId, HCHAR *szEvent, HCHAR *szStatusMember)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	RestPlayerStatus_t		*pstStatus;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrString = REST_ERR_STRING_INTERNAL;

	pstStatus = rest_bind_play_GetPlayerStatus(ulPlayerId);
	if (NULL == pstStatus)
	{
		HxLOG_Error("Memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;		szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		nErrCode = REST_ERR_CODE_NOMEM;		szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	// Jsonize the state
	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Event
		(void)HxJSONWRITER_String(nJWriter, "event", szEvent);
		
		// Data
		(void)HxJSONWRITER_Object(nJWriter, "data");
		{
			// Play Status
			(void)HxJSONWRITER_StringValue(nJWriter, "playStatus");
			(void)rest_datatype_JsonizePlayerStatus(nJWriter, pstStatus, szStatusMember);
		}
		(void)HxJSONWRITER_ObjectEnd(nJWriter);
		
		// Error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);
	nJWriter = 0;

END_FUNC:
	if (0 != nJWriter)
	{
		rest_bind_CloseJsonWriter(nJWriter);
	}
	
	if (NULL != pstStatus)
	{
		rest_bind_play_FreePlayerStatus(pstStatus);
	}
	
	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}


#define ___MEMBER_FUNCTIONS___
HCHAR *rest_bind_play_NotifyApkMediaPlay_Live (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData)
{
	HCHAR						*szPayload = NULL;

	switch (eEvent)
	{
	case eOxMP_EVENT_LIVE_SvcChanged:
		szPayload = rest_bind_play_live_MakeNotifierMsg(ulViewId, "sourceChanged", "playerId,sourceType,state,svcUid");
		break;

	case eOxMP_EVENT_LIVE_SvcStopped:
		szPayload = rest_bind_play_live_MakeNotifierMsg(ulViewId, "stopped", "playerId,state");
		break;

	case eOxMP_EVENT_COMPONENT_VIDEO_CHANGED:
		szPayload = rest_bind_play_live_MakeNotifierMsg(ulViewId, "videoComponentChanged", "playerId,state,currentVideoIndex,videoComponent");
		break;
		
	case eOxMP_EVENT_COMPONENT_AUDIO_CHANGED:
		szPayload = rest_bind_play_live_MakeNotifierMsg(ulViewId, "audioComponentChanged", "playerId,state,currentAudioIndex,audioComponent");
		break;
		
	case eOxMP_EVENT_COMPONENT_SUBTITLE_CHANGED:
		szPayload = rest_bind_play_live_MakeNotifierMsg(ulViewId, "subtitleComponentChanged", "playerId,state,currentSubtitleIndex,subtitleComponent");
		break;
			
	case eOxMP_EVENT_COMPONENT_DATA_CHANGED:
		szPayload = rest_bind_play_live_MakeNotifierMsg(ulViewId, "subtitleComponentChanged", "playerId,state,currentDataIndex,dataComponent");
		break;

	case eOxMP_EVENT_COMPONENT_SELECTED:
		switch (pstNotifyData->stEventNotify.ulParam1)
		{
		case eOxMP_COMPONENT_VIDEO:
			szPayload = rest_bind_play_live_MakeNotifierMsg(ulViewId, "videoSelected", "playerId,currentVideoIndex");
			break;
			
		case eOxMP_COMPONENT_AUDIO:
			szPayload = rest_bind_play_live_MakeNotifierMsg(ulViewId, "audioSelected", "playerId,currentAudioIndex");
			break;

		case eOxMP_COMPONENT_SUBTITLE:
			szPayload = rest_bind_play_live_MakeNotifierMsg(ulViewId, "subtitleSelected", "playerId,currentSubtitleIndex");
			break;

		case eOxMP_COMPONENT_DATA:
			szPayload = rest_bind_play_live_MakeNotifierMsg(ulViewId, "dataSelected", "playerId,currentDataIndex");
			break;

		default:
			break;
		}

		break;

	default:
		break;
	}

	return szPayload;
}

#define _____NOTIFIERS_____

