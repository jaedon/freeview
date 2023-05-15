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

/******************************************************************************/
/**
 * @file	  		nx_port_media.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PORT_MEDIA_H__
#define __NX_PORT_MEDIA_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/

#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include <nx_port_type.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
HERROR	NX_PORT_MEDIA_PLAY_GetLockState(HUINT32 ulViewId, OxMediaPlay_LiveLockState_e *peLockState);
HERROR	NX_PORT_MEDIA_PLAY_CheckPlayChangable(HUINT32 ulViewId, HUINT32 svcuid, HBOOL *pbChangable);
HERROR	NX_PORT_MEDIA_PLAY_GetPlayType(HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType);
HERROR	NX_PORT_MEDIA_PLAY_GetNeedRadioBg (HUINT32 ulViewId, HBOOL *pbNeed);
HERROR	NX_PORT_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId);
HERROR	NX_PORT_MEDIA_PLAY_GetMainViewId(HUINT32 *pulViewId);
HERROR	NX_PORT_MEDIA_PLAY_GetComponentNum(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HUINT32 *pulCompNum);
HERROR	NX_PORT_MEDIA_PLAY_GetComponentIndex(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 *pnCompIndex, HINT32 *pnCompSubIndex);
HERROR	NX_PORT_MEDIA_PLAY_SetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 lCompIndex);
HERROR	NX_PORT_MEDIA_PLAY_GetComponent(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, OxMediaPlay_Component_t *pstComponet);
HERROR	NX_PORT_MEDIA_PLAY_RegisterNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, OxMediaPlay_EventFilter_t pfEventFilter, void *pvUserData);

#define ______PLAYER_CONTROL________
HERROR	NX_PORT_MEDIA_PLAY_Stop(HUINT32 ulViewId);
HERROR	NX_PORT_MEDIA_PLAY_Start(HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart, HUINT32 *pulSessionId);
HERROR	NX_PORT_MEDIA_PLAY_SetPlaySpeed (HUINT32 ulViewId, HINT32 nSpeed);
HERROR 	NX_PORT_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT32 ulPosition);
HERROR	NX_PORT_MEDIA_PLAY_SetSubtitleEnable(HUINT32 ulViewId, HBOOL bEnable);
HERROR	NX_PORT_MEDIA_PLAY_SetSubtitleFontSize(HUINT32 ulViewId, HUINT32 ulFontSize);
HERROR	NX_PORT_MEDIA_PLAY_SetSubtitleTextPosition(HUINT32 ulViewId, HUINT32 ulTextPosition);
HERROR	NX_PORT_MEDIA_PLAY_SetSubtitleSyncTime(HUINT32 ulViewId, HUINT32 ulSyncTime);

#define ______PLAYER_GETTER_________
HERROR	NX_PORT_MEDIA_PLAY_GetPlayPosition (HUINT32 ulViewId, HUINT32 *pulPosition);
HERROR	NX_PORT_MEDIA_PLAY_GetPlayState(HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState);
HERROR	NX_PORT_MEDIA_PLAY_GetTsrRecTime(HUINT32 ulViewId, HUINT32 *pulRecTime);
HERROR 	NX_PORT_MEDIA_PLAY_GetDurationTime (HUINT32 ulViewId, HUINT32 *pulDurationTime);
HERROR	NX_PORT_MEDIA_PLAY_GetPlayError (HUINT32 ulViewId, OxMediaPlay_PlayError_e *peError, OxMediaPlay_ErrorReason_e *peReason);
HERROR	NX_PORT_MEDIA_PLAY_GetBufferState (HUINT32 ulViewId, OxMediaPlay_BufferState_e *peBufferState);
HERROR	NX_PORT_MEDIA_PLAY_GetPlaySpeed (HUINT32 ulViewId, HINT32 *pnSpeed);
HERROR	NX_PORT_MEDIA_PLAY_GetSubtitleEnable (HUINT32 ulViewId, HBOOL *pbEnable);
HERROR	NX_PORT_MEDIA_PLAY_GetSubtitleFontSize (HUINT32 ulViewId, HUINT32 *pulFontSize);
HERROR	NX_PORT_MEDIA_PLAY_GetSubtitleTextPosition (HUINT32 ulViewId, HUINT32 *pulTextPosition);
HERROR	NX_PORT_MEDIA_PLAY_GetSubtitleSyncTime (HUINT32 ulViewId, HUINT32 *pulSyncTime);

#define ______FUNCTION_PROPERTY______
HERROR 	NX_PORT_MEDIA_PLAY_SetVideoHide (HUINT32 ulViewId, HBOOL bHide);
HERROR 	NX_PORT_MEDIA_PLAY_SetTSREnable (HBOOL bEnable);
HERROR	NX_PORT_MEDIA_PLAY_Probe (HUINT32 ulViewId, HCHAR *pszURI);
HERROR	NX_PORT_MEDIA_PLAY_SeBufferingScenario(HUINT32 ulViewId, HBOOL bAuto, HINT32 nThreshold);
HERROR	NX_PORT_MEDIA_PLAY_SetDrmLicenseData (HUINT32 ulViewId, const HCHAR *pszData);
HERROR	NX_PORT_MEDIA_PLAY_SetBrowserUserAgentData (HUINT32 ulViewId, HCHAR *pszUAData);

// data streamer
HERROR	NX_PORT_MEDIA_DATASTREAMER_InitDataStreamer(APK_DATASTREAMER_Notifier_t fnNotifier);
HERROR	NX_PORT_MEDIA_DATASTREAMER_ConnectDataStreamer(HCHAR *data_type);
HERROR	NX_PORT_MEDIA_DATASTREAMER_DisconnectDataStreamer(HCHAR *data_type);

#endif // __NX_PORT_MEDIA_H__

