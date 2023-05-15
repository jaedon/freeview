/*******************************************************************
    File Description
********************************************************************/
/**
    @file     apk_dlan_airplay.c
    @brief

    Description:                                    \n
    Module: APPKITv2            \n

    Copyright (c) 2013 HUMAX Co., Ltd.              \n
    All rights reserved.                            \n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
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

#define	________________Header_Files_______________________________
#include	<apk.h>

#include	"_apk_int.h"
#include	"apk_airplay.h"
#include 	<homapi.h>
#include 	<homapi_airplay.h>

#define ________________Static_Variables___________________________
static HxList_t	*s_pst_airplay_cblist;

#define ________________Internal_Functions_________________________
STATIC void	apk_airplay_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 4);

	pvData = apArgV[2];
	if (pvData)
		APK_Free(pvData);
}

static HERROR ___event_cb(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	APKE_AIRPLAY_Property_e eType = eAPK_AIRPLAY_Unknown;
	HERROR		hErr	= ERR_FAIL;
	void	*pResultData = NULL;
	HUINT32	uiResultDataSize = 0;
	HOMxAIRPLAY_Event_t cmd = p1;
	HxLOG_Debug("[APK_AIRPLAY] Receive airplay msg [%d] \n", cmd);
	switch(cmd)
	{
		case eHOMAPI_AIRPLAY_OP_PLAY:
			{
				HOMxAIRPLAY_PARAMETER_WITH_URL_t *rcvPraram = (HOMxAIRPLAY_PARAMETER_WITH_URL_t *)p2;
				APKS_AIRPLAY_ReqPlayParam_t *sndParam = APK_Calloc(sizeof( APKS_AIRPLAY_ReqPlayParam_t));
				if(sndParam)
				{
					sndParam->nPlayPosition = rcvPraram->decimalParam;
					memcpy(sndParam->contentsURL, rcvPraram->contentURL, strlen(rcvPraram->contentURL));
					pResultData = (void *)sndParam;
					uiResultDataSize = sizeof( APKS_AIRPLAY_ReqPlayParam_t);
				}
				eType = eAPK_AIRPLAY_RequestPlay;
				if(rcvPraram && sndParam)
				{
					HxLOG_Debug("[APK_AIRPLAY] PLAY with url [%s] from [%lld]\n", rcvPraram->contentURL, sndParam->nPlayPosition);
				}
			}
			break;

		case eHOMAPI_AIRPLAY_OP_SETPOSITION:
			{
				HOMxAIRPLAY_PARAMETER_t *rcvParam = (HOMxAIRPLAY_PARAMETER_t *)p2;
				APKS_AIRPLAY_ReqSetPositionParam_t *sndParam = APK_Calloc(sizeof( APKS_AIRPLAY_ReqSetPositionParam_t));
				if(sndParam)
				{
					sndParam->nPlayPosition = rcvParam->decimalParam;
					pResultData = (void *)sndParam;
					uiResultDataSize = sizeof( APKS_AIRPLAY_ReqSetPositionParam_t);
				}
				eType = eAPK_AIRPLAY_RequestSetPosition;
				if(sndParam)
				{
					HxLOG_Debug("[APK_AIRPLAY] SetPosition with val [%lld]\n", sndParam->nPlayPosition);
				}
			}
			break;

		case eHOMAPI_AIRPLAY_OP_SETRATE:
			{
				HOMxAIRPLAY_PARAMETER_t *rcvParam = (HOMxAIRPLAY_PARAMETER_t *)p2;
				APKS_AIRPLAY_ReqSetRateParam_t *sndParam = APK_Calloc(sizeof( APKS_AIRPLAY_ReqSetRateParam_t));
				if(sndParam)
				{
					sndParam->nPlaySpeed = rcvParam->decimalParam;
					pResultData = (void *)sndParam;
					uiResultDataSize = sizeof( APKS_AIRPLAY_ReqSetRateParam_t);

					HxLOG_Debug("[APK_AIRPLAY] SetRate with val [%lld]\n", sndParam->nPlaySpeed);
				}
				eType = eAPK_AIRPLAY_RequestSetRate;
			}
			break;

		case eHOMAPI_AIRPLAY_OP_STOP:
			{
				pResultData = NULL;
				eType = eAPK_AIRPLAY_RequestStop;
				HxLOG_Debug("[APK_AIRPLAY] Stop\n");
			}
			break;

		case eHOMAPI_AIRPLAY_OP_SHOWPHOTO:
			{
				HOMxAIRTUNES_ALBUMART_t * rcvPraram = (HOMxAIRTUNES_ALBUMART_t *)p2;
				APKS_AIRTUNES_ReqSetAlbumart_t *sndParam = APK_Calloc(sizeof( APKS_AIRTUNES_ReqSetAlbumart_t));
				if(sndParam)
				{
					memcpy(sndParam, rcvPraram, sizeof(APKS_AIRTUNES_ReqSetAlbumart_t));
					HxLOG_Debug("[APK_AIRPLAY] Show Photo this %s\n", sndParam->szAlbumArtPath);
					pResultData = (void *)sndParam;
					uiResultDataSize = sizeof( APKS_AIRTUNES_ReqSetAlbumart_t);
				}
				eType = eAPK_AIRPLAY_RequestShowPhoto;
				HxLOG_Debug("[APK_AIRPLAY] Show Photo\n");
			}
			break;

		case eHOMAPI_AIRPLAY_OP_GET_PLAYBACK_INFO:
			{
				pResultData = NULL;
				eType = eAPK_AIRPLAY_RequestGetPlaybackInfo;
				HxLOG_Debug("[APK_AIRPLAY] GetPlaybackInfo\n");
			}
			break;

		case eHOMAPI_AIRTUNES_OP_PLAY:
			{
				HOMxAIRTUNES_URL_t *rcvPraram = (HOMxAIRTUNES_URL_t *)p2;
				APKS_AIRTUNES_ReqPlay_t *sndParam = APK_Calloc(sizeof( APKS_AIRTUNES_ReqPlay_t));
				if(sndParam)
				{
					memcpy(sndParam->contentURL, rcvPraram->contentURL, strlen(rcvPraram->contentURL));
					pResultData = (void *)sndParam;
					uiResultDataSize = sizeof( APKS_AIRTUNES_ReqPlay_t);
				}
				eType = eAPK_AIRTUNES_RequestPlay;
				if(rcvPraram && sndParam)
				{
					HxLOG_Debug("[APK_AIRPLAY] Airtunes PLAY with url [%s] \n", rcvPraram->contentURL);
				}
			}
			break;
		case eHOMAPI_AIRTUNES_OP_FLUSH:
			{
				pResultData = NULL;
				eType = eAPK_AIRTUNES_RequestFlush;
				HxLOG_Debug("[APK_AIRPLAY] Airtunes Flush\n");
			}
			break;
		case eHOMAPI_AIRTUNES_OP_STOP:
			{
				pResultData = NULL;
				eType = eAPK_AIRTUNES_RequestStop;
				HxLOG_Debug("[APK_AIRPLAY] Airtunes Stop\n");
			}
			break;
		case eHOMAPI_AIRTUNES_OP_SET_METADATA:
			{
				HOMxAIRTUNES_METADATA_t *rcvPraram = (HOMxAIRTUNES_METADATA_t *)p2;
				APKS_AIRTUNES_ReqSetMetadata_t *sndParam = APK_Calloc(sizeof( APKS_AIRTUNES_ReqSetMetadata_t));
				if(sndParam)
				{
					memcpy(sndParam, rcvPraram, sizeof(APKS_AIRTUNES_ReqSetMetadata_t));
					pResultData = (void *)sndParam;
					uiResultDataSize = sizeof( APKS_AIRTUNES_ReqSetMetadata_t);
				}
				eType = eAPK_AIRTUNES_RequestSetMetadata;
				if(rcvPraram && sndParam)
				{
					HxLOG_Debug("[APK_AIRPLAY] Airtunes Set Metadata [%s,%s,%s] \n", sndParam->szAlbum, sndParam->szArtist, sndParam->szTitle);
				}
			}
			break;
		case eHOMAPI_AIRTUNES_OP_SET_ALBUMART:
			{
				HOMxAIRTUNES_ALBUMART_t *rcvPraram = (HOMxAIRTUNES_ALBUMART_t *)p2;
				APKS_AIRTUNES_ReqSetAlbumart_t *sndParam = APK_Calloc(sizeof( APKS_AIRTUNES_ReqSetAlbumart_t));
				if(sndParam)
				{
					memcpy(sndParam, rcvPraram, sizeof(APKS_AIRTUNES_ReqSetAlbumart_t));
					pResultData = (void *)sndParam;
					uiResultDataSize = sizeof( APKS_AIRTUNES_ReqSetAlbumart_t);
				}
				eType = eAPK_AIRTUNES_RequestSetAlbumart;
				if(rcvPraram && sndParam)
				{
					HxLOG_Debug("[APK_AIRPLAY] Airtunes Set Albumart %s \n", sndParam->szAlbumArtPath);
				}
			}
			break;

		default:
			/* unknown event */
			break;
	}

	if ( s_pst_airplay_cblist )
	{
		HxList_t 		*item =  s_pst_airplay_cblist;
		APKS_AIRPLAY_CbHandler pfnObjectHandler = NULL;

		while ( item )
		{
			pfnObjectHandler = (APKS_AIRPLAY_CbHandler)HLIB_LIST_Data(item);
			if ( pfnObjectHandler )
			{
				APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

				stApkEvtArgs.pfnFreeFunc = apk_airplay_freeEventCb;
				stApkEvtArgs.ulArgCount = 4;
				stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
				stApkEvtArgs.apArgV[1] = (void *)eType;
				stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, uiResultDataSize);
				stApkEvtArgs.apArgV[3] = (void *)1;

				APK_EVENT_Emit(&stApkEvtArgs);
			}

			item = item->next;
		}
	}

	if(pResultData)
		APK_Free(pResultData);

	return hErr;
}

#define ________________Public_Interfaces__________________________
void	APK_AIRPLAY_Init( void )
{
	HOMAPI_AIRPLAY_RegisterNotifier(___event_cb);
}

void	APK_AIRPLAY_DeInit( void )
{
	HOMAPI_AIRPLAY_UnRegisterNotifier();
	HOMAPI_AIRPLAY_DeInit();
}

void	APK_AIRPLAY_RegisterNotifier( APKS_AIRPLAY_CbHandler pfnObjectHandler )
{
	HxList_t 		*list = HLIB_LIST_Find( s_pst_airplay_cblist, pfnObjectHandler );

	if ( list )
		HxLOG_Error("pfnObjectHandler(%X) is already registered!!!\n", pfnObjectHandler);
	else
		s_pst_airplay_cblist = HLIB_LIST_Append( s_pst_airplay_cblist, (void *)pfnObjectHandler );
}

void	APK_AIRPLAY_UnRegisterNotifier( APKS_AIRPLAY_CbHandler pfnObjectHandler )
{
	HxList_t		*list = HLIB_LIST_Find( s_pst_airplay_cblist, pfnObjectHandler );

	if ( NULL == list )
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", pfnObjectHandler);
	else
		s_pst_airplay_cblist = HLIB_LIST_Remove( s_pst_airplay_cblist, pfnObjectHandler );
}

HERROR	APK_AIRPLAY_Start( void )
{
	return HOMAPI_AIRPLAY_Start();
}

HERROR	APK_AIRPLAY_Stop( void )
{
	return HOMAPI_AIRPLAY_Stop();
}

HERROR APK_AIRPLAY_SetPlaybackInfo(APKS_AIRPLAY_PlaybackInfo_t tPlaybackInfo)
{
	HOMxAIRPLAY_PLAYBACK_INFO_t tParam;
	memset(&tParam, 0x0, sizeof(HOMxAIRPLAY_PLAYBACK_INFO_t));

	switch(tPlaybackInfo.ePlaybackStatus)
	{
		case eAPK_AIRPLAY_PBSTATUS_Pause:
			tParam.ePlaybackStatus = eHOMAPI_AIRPLAY_PLAYBACK_PAUSE;
			break;
		case eAPK_AIRPLAY_PBSTATUS_Playing:
			tParam.ePlaybackStatus = eHOMAPI_AIRPLAY_PLAYBACK_PLAYING;
			break;
		case eAPK_AIRPLAY_PBSTATUS_ReachedEOF:
			tParam.ePlaybackStatus = eHOMAPI_AIRPLAY_PLAYBACK_REACHED_EOF;
			break;
		case eAPK_AIRPLAY_PBSTATUS_Stopped:
			tParam.ePlaybackStatus = eHOMAPI_AIRPLAY_PLAYBACK_STOPPED;
			break;
		case eAPK_AIRPLAY_PBSTATUS_Loading:
			tParam.ePlaybackStatus = eHOMAPI_AIRPLAY_PLAYBACK_LOADING;
			break;
		case eAPK_AIRPLAY_PBSTATUS_Error:
			tParam.ePlaybackStatus = eHOMAPI_AIRPLAY_PLAYBACK_ERROR;
			break;
		case eAPK_AIRPLAY_PBSTATUS_Unknown:
		default:
			tParam.ePlaybackStatus = eHOMAPI_AIRPLAY_PLAYBACK_NONE;
			break;
	}

	tParam.ullBufferedRange = tPlaybackInfo.ullBufferedRange;
	tParam.ullDuration = tPlaybackInfo.ullDuration;
	tParam.ullPosition = tPlaybackInfo.ullPosition;
	tParam.ullSeekableRange = tPlaybackInfo.ullSeekableRange;
	tParam.ullStartPosition = tPlaybackInfo.ullStartPosition;
	tParam.unRate = tPlaybackInfo.unRate;

	HxLOG_Debug("[ APK_AIRPLAY ] send playback info\n");
	return HOMAPI_AIRPLAY_SetPlaybackInfo(tParam);
}

