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
 * @file	  		nx_port_media.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

#include <nx_port.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/



HERROR	NX_PORT_MEDIA_PLAY_GetLockState(HUINT32 ulViewId, OxMediaPlay_LiveLockState_e *peLockState)
{
	return APK_MEDIA_PLAY_GetLockState(ulViewId, peLockState);
}

HERROR	NX_PORT_MEDIA_PLAY_CheckPlayChangable(HUINT32 ulViewId, HUINT32 svcuid, HBOOL *pbChangable)
{
	HERROR 	hErr	= ERR_FAIL;
	OxMediaPlay_StartInfo_t 	unStart;

	*pbChangable = FALSE;

	unStart.eType	= eOxMP_MEDIATYPE_LIVE;
	unStart.stLive.ulMasterSvcUid	= svcuid;
	unStart.stLive.ulSuppleSvcUid	= svcuid;

	hErr = APK_MEDIA_PLAY_CheckPlayChangable (ulViewId, eOxMP_MEDIATYPE_LIVE, &unStart, pbChangable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_CheckPlayChangable err: ulViewId(0x%04x), svcuid(%d).\n", ulViewId, svcuid);
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR	NX_PORT_MEDIA_PLAY_GetPlayType(HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType)
{
	return APK_MEDIA_PLAY_GetPlayType(ulViewId, peType);
}

HERROR	NX_PORT_MEDIA_PLAY_GetNeedRadioBg (HUINT32 ulViewId, HBOOL *pbNeed)
{
	return APK_MEDIA_PLAY_GetNeedRadioBg(ulViewId, pbNeed);
}

HERROR	NX_PORT_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId)
{
	return APK_MEDIA_PLAY_GetRequestId(ulViewId, eType, pulRequestId);
}

HERROR	NX_PORT_MEDIA_PLAY_GetMainViewId (HUINT32 *pulViewId)
{
	return APK_MEDIA_PLAY_GetMainViewId(pulViewId);
}

HERROR	NX_PORT_MEDIA_PLAY_GetComponentNum (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HUINT32 *pulCompNum)
{
	return	APK_MEDIA_PLAY_GetComponentNum (ulViewId, eCompType, pulCompNum);
}

HERROR	NX_PORT_MEDIA_PLAY_GetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 *plCompIndex, HINT32 *plCompSubIndex)
{
	return	APK_MEDIA_PLAY_GetComponentIndex(ulViewId, eCompType, plCompIndex, plCompSubIndex);
}

HERROR	NX_PORT_MEDIA_PLAY_SetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 lCompIndex)
{
	return	APK_MEDIA_PLAY_SetComponentIndex(ulViewId, eCompType, lCompIndex);
}

HERROR 	NX_PORT_MEDIA_PLAY_GetComponent (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 lCompIndex, OxMediaPlay_Component_t *pstComponet)
{
	return APK_MEDIA_PLAY_GetComponent(ulViewId, eCompType, lCompIndex, pstComponet);
}

HERROR	NX_PORT_MEDIA_PLAY_RegisterNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, OxMediaPlay_EventFilter_t pfEventFilter, void *pvUserData)
{
	return APK_MEDIA_PLAY_RegisterNotifier(ulViewId, pfNotifier, pfEventFilter, pvUserData);
}

#define ______PLAYER_CONTROL________
HERROR	NX_PORT_MEDIA_PLAY_Stop (HUINT32 ulViewId)
{
	return APK_MEDIA_PLAY_Stop(ulViewId);
}

HERROR	NX_PORT_MEDIA_PLAY_Start (HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart, HUINT32 *pulSessionId)
{
	return APK_MEDIA_PLAY_Start (ulViewId, punStart, pulSessionId);
}

HERROR	NX_PORT_MEDIA_PLAY_SetPlaySpeed (HUINT32 ulViewId, HINT32 nSpeed)
{
	return APK_MEDIA_PLAY_SetPlaySpeed(ulViewId, nSpeed);
}

HERROR 	NX_PORT_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT32 ulPosition)
{
	return APK_MEDIA_PLAY_SetPlayPosition(ulViewId, ulPosition);
}

HERROR	NX_PORT_MEDIA_PLAY_SetSubtitleEnable(HUINT32 ulViewId, HBOOL bEnable)
{
	return APK_MEDIA_PLAY_SetSubtitleEnable(ulViewId, bEnable);
}

HERROR	NX_PORT_MEDIA_PLAY_SetSubtitleFontSize(HUINT32 ulViewId, HUINT32 ulFontSize)
{
	return APK_MEDIA_PLAY_SetSubtitleFontSize(ulViewId, ulFontSize);
}

HERROR	NX_PORT_MEDIA_PLAY_SetSubtitleTextPosition(HUINT32 ulViewId, HUINT32 ulTextPosition)
{
	return APK_MEDIA_PLAY_SetSubtitleTextPosition(ulViewId, ulTextPosition);
}

HERROR	NX_PORT_MEDIA_PLAY_SetSubtitleSyncTime(HUINT32 ulViewId, HUINT32 ulSyncTime)
{
	return APK_MEDIA_PLAY_SetSubtitleSyncTime(ulViewId, ulSyncTime);
}

#define ______PLAYER_GETTER_________
HERROR	NX_PORT_MEDIA_PLAY_GetPlayPosition (HUINT32 ulViewId, HUINT32 *pulPosition)
{
	return APK_MEDIA_PLAY_GetPlayPosition(ulViewId, pulPosition);
}

HERROR	NX_PORT_MEDIA_PLAY_GetPlayState(HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState)
{
	return APK_MEDIA_PLAY_GetPlayState(ulViewId, peState);
}

HERROR	NX_PORT_MEDIA_PLAY_GetTsrRecTime(HUINT32 ulViewId, HUINT32 *pulRecTime)
{
	return APK_MEDIA_PLAY_GetTsrRecTime(ulViewId, pulRecTime);
}

HERROR	NX_PORT_MEDIA_PLAY_GetDurationTime (HUINT32 ulViewId, HUINT32 *pulDurationTime)
{
	return APK_MEDIA_PLAY_GetDurationTime(ulViewId, pulDurationTime);
}

HERROR	NX_PORT_MEDIA_PLAY_GetPlayError (HUINT32 ulViewId, OxMediaPlay_PlayError_e *peError, OxMediaPlay_ErrorReason_e *peReason)
{
	return APK_MEDIA_PLAY_GetPlayError(ulViewId, peError, peReason);
}

HERROR	NX_PORT_MEDIA_PLAY_GetBufferState (HUINT32 ulViewId, OxMediaPlay_BufferState_e *peBufferState)
{
	return APK_MEDIA_PLAY_GetBufferState(ulViewId, peBufferState);
}

HERROR	NX_PORT_MEDIA_PLAY_GetPlaySpeed (HUINT32 ulViewId, HINT32 *pnSpeed)
{
	return APK_MEDIA_PLAY_GetPlaySpeed(ulViewId, pnSpeed);
}

HERROR	NX_PORT_MEDIA_PLAY_GetSubtitleEnable (HUINT32 ulViewId, HBOOL *pbEnable)
{
	return APK_MEDIA_PLAY_GetSubtitleEnable(ulViewId, pbEnable);
}

HERROR	NX_PORT_MEDIA_PLAY_GetSubtitleFontSize (HUINT32 ulViewId, HUINT32 *pulFontSize)
{
	return APK_MEDIA_PLAY_GetSubtitleFontSize(ulViewId, pulFontSize);
}

HERROR	NX_PORT_MEDIA_PLAY_GetSubtitleTextPosition (HUINT32 ulViewId, HUINT32 *pulTextPosition)
{
	return APK_MEDIA_PLAY_GetSubtitleTextPosition(ulViewId, pulTextPosition);
}

HERROR	NX_PORT_MEDIA_PLAY_GetSubtitleSyncTime (HUINT32 ulViewId, HUINT32 *pulSyncTime)
{
	return APK_MEDIA_PLAY_GetSubtitleSyncTime(ulViewId, pulSyncTime);
}

#define ______FUNCTION_PROPERTY______
HERROR 	NX_PORT_MEDIA_PLAY_SetVideoHide (HUINT32 ulViewId, HBOOL bHide)
{
	return APK_MEDIA_PLAY_SetVideoHide(ulViewId, bHide);
}

HERROR 	NX_PORT_MEDIA_PLAY_SetTSREnable (HBOOL bEnable)
{
	return APK_MEDIA_PLAY_SetTSREnable(bEnable);
}

#define _________MEDIA_PB____________
HERROR	NX_PORT_MEDIA_PLAY_Probe (HUINT32 ulViewId, HCHAR *pszURI)
{
	return APK_MEDIA_PLAY_Probe(ulViewId, pszURI);
}

HERROR	NX_PORT_MEDIA_PLAY_SeBufferingScenario(HUINT32 ulViewId, HBOOL bAuto, HINT32 nThreshold)
{
	return APK_MEDIA_PLAY_SeBufferingScenario(ulViewId, bAuto, nThreshold);
}

HERROR	NX_PORT_MEDIA_PLAY_SetDrmLicenseData (HUINT32 ulViewId, const HCHAR *pszData)
{
	return APK_MEDIA_PLAY_SetDrmLicenseData(ulViewId, pszData);
}

HERROR	NX_PORT_MEDIA_PLAY_SetBrowserUserAgentData (HUINT32 ulViewId, HCHAR *pszUAData)
{
	return APK_MEDIA_PLAY_SetBrowserUserAgentData(ulViewId, pszUAData);
}


// data streamer
HERROR	NX_PORT_MEDIA_DATASTREAMER_InitDataStreamer(APK_DATASTREAMER_Notifier_t fnNotifier)
{
	return	APK_DATASTREAMER_InitDataStreamer(fnNotifier);
}

HERROR	NX_PORT_MEDIA_DATASTREAMER_ConnectDataStreamer(HCHAR *data_type)
{
	return APK_DATASTREAMER_ConnectDataStreamer(data_type);
}

HERROR	NX_PORT_MEDIA_DATASTREAMER_DisconnectDataStreamer(HCHAR *data_type)
{
	return APK_DATASTREAMER_DisconnectDataStreamer(data_type);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

