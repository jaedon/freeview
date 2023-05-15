/*
 * (c) 2011-2015 Humax Co., Ltd.
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
 * @file	  		nx_core_recorder.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

#include <nx_core_recorder.h>
#include <nx_prism_app.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static	NxRecorderInst_t	*s_pstRecorderInst = NULL;
static	NxRecMgrContext_t	*s_pstRecMgrContext = NULL;

/**************************************************************************************************/
#define ______________Private_prototype__________________________________________
/**************************************************************************************************/
static NxRecorderInst_t	*nx_recorder_GetRecInst(void);
static HERROR	nx_recorder_GetBufferingTypeOfTime(NX_Channel_t *pstChannel, NX_BufferingType_e *peType, HUINT32 *pulBufTime);
static HERROR	nx_recorder_GetMaxOffset(NX_Channel_t *pstChannel, HUINT32 *pulPbOffset);
static void nx_recorder_notifyRecFunc ( HUINT32 ulSvcUid, HUINT32 ulSessionId, OxMediaRec_Event_e eEvent, OxMediaRec_NotifierData_t *pstNotifyData, void *pvUserData );
static void 	nx_recorder_RecInstanceListDelete(void *item);
static HBOOL 	nx_recorder_RecInstanceListFindSession(void *data, void *userdata);
static HBOOL 	nx_recorder_RecInstanceListFindSvcUid(void *data, void *userdata);

#define	___________RELATED_RECORDING_ITEM_____________
static NxRecMgrContext_t *nx_recorder_mgr_GetContext(void);
static HERROR	nx_recorder_mgr_LoadRecordItem(NxRecMgrContext_t *pstMgrContext);
static void 	nx_recorder_mgr_Recorditem_EventListener (NX_MetaRec_Handle_t self, HINT32 event, const HINT32 *args, void *userdata);
static HERROR	nx_recorder_mgr_RecordingCreate(NxRecMgrContext_t *pstRecMgrContext);
static HBOOL 	nx_recorder_mgr_FindRecCid(void *data, void *userdata);
static NxRecItem_t	*nx_recorder_mgr_GetRecItem(HxVector_t *recItemList, HUINT32 ulContentId);
static HBOOL	nx_recorder_mgr_IsRecItem(HxVector_t *recItemList, HUINT32 ulContentId);
static void 	nx_recorder_mgr_RecItemListDelete(void *item);
static HCHAR	*nx_recorder_mgr_GetThumbnail(HCHAR *szFileUrl, HUINT32 ulExtractPosition, const HCHAR *pszMountPath, HBOOL bRetExpectURL);

#if defined(CONFIG_DEBUG)
static void nx_recorder_Command_Init(void);
#endif

/**************************************************************************************************/
#define ______________Private_functions__________________________________________
/**************************************************************************************************/
static NxRecorderInst_t	*nx_recorder_GetRecInst(void)
{
	if (s_pstRecorderInst == NULL)
	{
		s_pstRecorderInst = (NxRecorderInst_t*)NX_APP_Calloc(sizeof(NxRecorderInst_t));
		s_pstRecorderInst->recInstanceList = HLIB_VECTOR_NewEx(NULL, 0,
												(void *(*)(void *))NULL,
												(void *(*)(void *))nx_recorder_RecInstanceListDelete,
												NULL);
	}
	return s_pstRecorderInst;
}

static	HERROR	nx_recorder_GetBufferingTypeOfTime(NX_Channel_t *pstChannel, NX_BufferingType_e *peType, HUINT32 *pulBufTime)
{
	HUINT32		ulTime = 0;
	HUINT32 	ulRsvSlot = 0;
	HERROR		hErr = ERR_FAIL;
	HBOOL		bRecording;

	*peType  = eNxBufferingType_NONE;

	hErr = APK_MEDIA_RECORD_IsRecording(pstChannel->svcUid, &bRecording);
	if (ERR_OK != hErr)
	{
		goto EXIT_FUNC;
	}

	if (bRecording == TRUE)
	{
		ulTime = 0;
		// svc uid를 통해 Sama의 slot id를 가져와 APK_MEDIA_RECORD_GetRecTime에 넣어준다
		hErr = APK_MEDIA_RECORD_GetRsvSlotByUID(pstChannel->svcUid, &ulRsvSlot);
		if (ERR_OK != hErr)
		{
			goto EXIT_FUNC;
		}

		hErr = APK_MEDIA_RECORD_GetRecTime(ulRsvSlot, &ulTime);
		if (ERR_OK != hErr)
		{
			goto EXIT_FUNC;
		}
		*peType = eNxBufferingType_USR;
	}
	else
	{
		hErr = NX_PORT_MEDIA_PLAY_GetTsrRecTime(0, &ulTime);
		if (ERR_OK != hErr)
		{
			goto EXIT_FUNC;
		}
		*peType = eNxBufferingType_TSR;
	}

	*pulBufTime = ulTime;

EXIT_FUNC:
	return ERR_OK;
}

static HERROR	nx_recorder_GetMaxOffset(NX_Channel_t *pstChannel, HUINT32 *pulPbOffset)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulTime = 0;
	NX_BufferingType_e	eBufType = eNxBufferingType_NONE;

	hErr = nx_recorder_GetBufferingTypeOfTime(pstChannel, &eBufType, &ulTime);
	if ((ERR_OK != hErr) || (eNxBufferingType_NONE == eBufType))
	{
		*pulPbOffset = 0;
		return ERR_OK;
	}

	*pulPbOffset = ulTime;
	return ERR_OK;
}

static void nx_recorder_notifyRecFunc ( HUINT32 ulSvcUid, HUINT32 ulSessionId, OxMediaRec_Event_e eEvent, OxMediaRec_NotifierData_t *pstNotifyData, void *pvUserData )
{
	NxRecInstItem_t *pstRecInstance = NULL;
	NxRecorderInst_t *pstRecInstanceContext = nx_recorder_GetRecInst();

	switch (eEvent)
	{
	case eOxMR_EVENT_REC_STARTED:
		HxLOG_Warning("eOxMR_EVENT_REC_STARTED session %d\n", ulSessionId);
		//adding instance
		if(pstNotifyData)
		{
			if(pstRecInstanceContext)
			{
				pstRecInstance = (NxRecInstItem_t*)NX_APP_Calloc(sizeof(NxRecInstItem_t));
				if(pstRecInstance)
				{
					pstRecInstance->ulSvcUid = ulSvcUid;
					pstRecInstance->ulSessionId = ulSessionId;
					HxSTD_MemCopy(&(pstRecInstance->stStartInfo), pstNotifyData, sizeof(OxMediaRec_StartInfo_t));
					HLIB_VECTOR_Add(pstRecInstanceContext->recInstanceList, pstRecInstance);
				}
			}
		}
		ONDK_GWM_PostMessage(NULL, MSG_APP_RECORD_START, 0, ulSvcUid, ulSessionId, 0);
		break;

	case eOxMR_EVENT_REC_STOPPED:
		HxLOG_Warning("eOxMR_EVENT_REC_STOPPED session %d\n", ulSessionId);
		if(pstRecInstanceContext)
		{ 
			HINT32 nLen = HLIB_VECTOR_Length(pstRecInstanceContext->recInstanceList);
			HxLOG_Warning("rec instant removed : len [%d] \n", nLen);
			if(nLen > 0)
			{
				HINT32 nResCnt = 0;
				nResCnt = HLIB_VECTOR_RemoveIf(pstRecInstanceContext->recInstanceList, nx_recorder_RecInstanceListFindSession, (void*)ulSessionId);
				HxLOG_Warning("rec instant removed : session [%d] cnt [%d] \n", ulSessionId, nResCnt);
			}
		}
		ONDK_GWM_PostMessage(NULL, MSG_APP_RECORD_STOP, 0, ulSvcUid, ulSessionId, 0);
		break;

	case eOxMR_EVENT_REC_TSRBUFFER_COPYEND:
		HxLOG_Warning("eOxMR_EVENT_REC_TSRBUFFER_COPYEND session %d\n", ulSessionId);
		ONDK_GWM_PostMessage(NULL, MSG_APP_RECORD_TSRBUFFER_COPYEND, 0, ulSvcUid, ulSessionId, 0);
		break;
	default:
		return;
	}

}

static void 	nx_recorder_RecInstanceListDelete(void *item)
{
	NX_APP_Free(item);
}

static HBOOL 	nx_recorder_RecInstanceListFindSession(void *data, void *userdata)
{
	NxRecInstItem_t *pstRecInstantItem = (NxRecInstItem_t *)data;
	HUINT32 ulSessionId = (HUINT32)userdata;

	if(pstRecInstantItem->ulSessionId == ulSessionId)
	{
		return TRUE;
	}

	return FALSE;
}

static HBOOL 	nx_recorder_RecInstanceListFindSvcUid(void *data, void *userdata)
{
	NxRecInstItem_t *pstRecInstantItem = (NxRecInstItem_t *)data;
	HUINT32 ulSvcUid = (HUINT32)userdata;

	if(pstRecInstantItem->ulSvcUid == ulSvcUid)
	{
		return TRUE;
	}

	return FALSE;
}


#define _____________nx_recorder_mgr_____________________________________________

static NxRecMgrContext_t *nx_recorder_mgr_GetContext(void)
{
	if (s_pstRecMgrContext == NULL)
	{
		s_pstRecMgrContext = (NxRecMgrContext_t*)NX_APP_Calloc(sizeof(NxRecMgrContext_t));
		s_pstRecMgrContext->recItemList = HLIB_VECTOR_NewEx(NULL, 0,
												(void *(*)(void *))NULL,
												(void *(*)(void *))nx_recorder_mgr_RecItemListDelete,
												NULL);
	}
	return s_pstRecMgrContext;
}

static HERROR	nx_recorder_mgr_LoadRecordItem(NxRecMgrContext_t *pstMgrContext)
{
	HUINT32 		ulCount = 0;
	HxVector_t		*pRecItemList = NULL;
	HINT32			nRecItemLen = 0, i = 0;
	DxRecListData_t	*pstRecItemData = NULL;

	ulCount 		= NX_PORT_META_RECORD_Count();

	pRecItemList	= NX_PORT_META_RECORD_GetItems();

	if (pRecItemList == NULL)
	{
		HxLOG_Error("No Record Item.\n");
		return ERR_OK;
	}

	nRecItemLen = HLIB_VECTOR_Length(pRecItemList);

	for (i = 0; i < nRecItemLen; i++)
	{
		pstRecItemData = (DxRecListData_t*)HLIB_VECTOR_ItemAt(pRecItemList, i);
		if(pstRecItemData && (nx_recorder_mgr_IsRecItem(pstMgrContext->recItemList, pstRecItemData->ulContentId) == FALSE))
		{
			NxRecItem_t *pstNxRecItem = (NxRecItem_t*)NX_APP_Calloc(sizeof(NxRecItem_t));
			pstNxRecItem->ulContentId = pstRecItemData->ulContentId;

			HxLOG_Debug("Add RecItem. (%d). recItem: ulContentId(%u), name(%s), url(%s)\n",
							i, pstRecItemData->ulContentId, pstRecItemData->szName, pstRecItemData->szUrl);

			HLIB_VECTOR_Add(pstMgrContext->recItemList, pstNxRecItem);
		}
		else
		{
			HxLOG_Debug("Can't Get Record Item(CID:%u), or Already is Record Item in List.\n");
		}
	}
	return ERR_OK;
}

static void 	nx_recorder_mgr_Recorditem_EventListener (NX_MetaRec_Handle_t self, HINT32 event, const HINT32 *args, void *userdata)
{
	HCHAR			*szUrl = NULL;
	DxRecListData_t	*pDxRecListData = NULL;
	NxRecItem_t		*pstFoundNxRecItem = NULL;
	NxRecMgrContext_t *pstMgrContext = nx_recorder_mgr_GetContext();

	if (NULL == args)
	{
		HxLOG_Error("args is NULL, event = (%d) \n", event);
		HxLOG_Error("\n\n\n Record Event args is NULL!!!!\n\n\n");

		// do try load
		nx_recorder_mgr_LoadRecordItem(pstMgrContext);
		return ;
	}

	szUrl = (HCHAR*)args;
	HxLOG_Debug("szUrl (%s)\n", szUrl);

	pDxRecListData = NX_PORT_META_RECORD_GetByUrl(szUrl);

	if(pDxRecListData == NULL)
	{
		HxLOG_Error("Can't find Record... url(%s)!\n",szUrl);
		return;
	}

	HxLOG_Debug("\n==================== RecordItem Updated =======================\n");
	HxLOG_Debug("DxRec. ulContentId (%u),  Url(%s)\n", pDxRecListData->ulContentId, pDxRecListData->szUrl);

	pstFoundNxRecItem = nx_recorder_mgr_GetRecItem(pstMgrContext->recItemList, pDxRecListData->ulContentId);

	if (pstFoundNxRecItem == NULL)	// 기존 리스트에 없는 거라면, 추가한다.
	{
		NxRecItem_t *pstNxRecItem = NX_APP_Calloc(sizeof(NxRecItem_t));
		pstNxRecItem->ulContentId = pDxRecListData->ulContentId;
		HLIB_VECTOR_Add(pstMgrContext->recItemList, pstNxRecItem);
	}

	if (event == eAPK_META_REC_EVENT_UPDATED)
	{
		//ONDK_GWM_PostMessage(NULL, MSG_CORE_UPDATE_REC_ITEM, 0, 0, 0, 0);
	}

	NX_PORT_META_RECORD_Release(pDxRecListData);
}

static HERROR	nx_recorder_mgr_RecordingCreate(NxRecMgrContext_t *pstRecMgrContext)
{
	NX_PORT_META_RECORD_EnableLoadEvent();
	NX_PORT_META_RECORD_AddNotifier(nx_recorder_mgr_Recorditem_EventListener, NULL);

	return ERR_OK;
}

static HBOOL 	nx_recorder_mgr_FindRecCid(void *data, void *userdata)
{
	NxRecItem_t *pstRecItem = (NxRecItem_t *)data;
	HUINT32 ulContentId = (HUINT32)userdata;

	if(pstRecItem->ulContentId == ulContentId)
		return TRUE;
	else
		return FALSE;
}

static NxRecItem_t	*nx_recorder_mgr_GetRecItem(HxVector_t *recItemList, HUINT32 ulContentId)
{
	NxRecItem_t *pFoundData = NULL;
	pFoundData = HLIB_VECTOR_Find(recItemList, nx_recorder_mgr_FindRecCid, (void*)ulContentId);

	return pFoundData;
}

static HBOOL	nx_recorder_mgr_IsRecItem(HxVector_t *recItemList, HUINT32 ulContentId)
{
	NxRecItem_t *pFoundData = NULL;
	pFoundData = HLIB_VECTOR_Find(recItemList, nx_recorder_mgr_FindRecCid, (void*)ulContentId);

	if (pFoundData)
		return TRUE;

	return FALSE;
}

static void 	nx_recorder_mgr_RecItemListDelete(void *item)
{
	NX_APP_Free(item);
}

static HCHAR	*nx_recorder_mgr_GetThumbnail(HCHAR *szFileUrl, HUINT32 ulExtractPosition, const HCHAR *pszMountPath, HBOOL bRetExpectURL)
{
	HCHAR tempUrl[1024] = {0,};
	HCHAR *pstRet = NULL;
	HCHAR *pThumbnail = NULL;

#if defined(CONFIG_SUPPORT_THMA)
	pThumbnail = (HCHAR*)THAPI_GetThumbnailURL((HCHAR*)szFileUrl, ulExtractPosition, (HCHAR*)pszMountPath, bRetExpectURL);
#endif

	// pThumbnail 는 cbuf 이므로 release mem 필요없음.
	if (pThumbnail)
	{
		HxLOG_Debug("Get Thumbnail (%s)\n", pThumbnail);
		HxSTD_PrintToStrN(tempUrl, 1024, "%s", pThumbnail); // without file:///
		pstRet = HLIB_STD_StrDup(tempUrl);
		return pstRet;
	}

	return NULL;
}

/**************************************************************************************************/
#define _____________Public_functions____________________________________________
/**************************************************************************************************/
HERROR NX_RECORDER_Init(void)
{
	NxRecMgrContext_t * pstRecMgrContext = NULL;

	HxLOG_Error("\n\n=============== NX_RECORDER_Init() ==================\n\n");

	HxLOG_Debug("Create NxRecMgrContext_t.\n");
	pstRecMgrContext = nx_recorder_mgr_GetContext();

	nx_recorder_mgr_RecordingCreate(pstRecMgrContext);

	APK_MEDIA_RECORD_RegisterNotifier(-1, (OxMediaRec_Notifier_t)nx_recorder_notifyRecFunc, NULL);

#if defined(CONFIG_DEBUG)
	nx_recorder_Command_Init();
#endif

	return ERR_OK;
}

NxRecorderInst_t	*NX_RECORDER_GetRecordingInst(void)
{
	return nx_recorder_GetRecInst();
}

HINT32	NX_RECORDER_GetRecordingInstCount(void)
{
	NxRecorderInst_t *pstRecInstanceContext = nx_recorder_GetRecInst();
	HINT32 nLen = 0;

	if(pstRecInstanceContext)
	{
		nLen = HLIB_VECTOR_Length(pstRecInstanceContext->recInstanceList);
	}

	return nLen;
}

NxRecInstItem_t	*NX_RECORDER_GetRecordingInstItem(HINT32 nIndex)
{
	NxRecorderInst_t *pstRecInstanceContext = nx_recorder_GetRecInst();
	NxRecInstItem_t *pstRecInstance = NULL;

	if(pstRecInstanceContext)
	{
		pstRecInstance = HLIB_VECTOR_ItemAt(pstRecInstanceContext->recInstanceList, nIndex);
	}

	return pstRecInstance;
}

HBOOL NX_RECORDER_FindRecordingInstBySvcUid(HUINT32 ulSvcUid)
{
	NxRecorderInst_t *pstRecInstanceContext = nx_recorder_GetRecInst();
	NxRecInstItem_t *pstRecInstance = NULL;
	HINT32 nResCnt = 0;

	pstRecInstance = HLIB_VECTOR_Find(pstRecInstanceContext->recInstanceList, nx_recorder_RecInstanceListFindSvcUid, (void*)ulSvcUid);
	if(pstRecInstance)
	{
		return  TRUE;
	}

	return FALSE;
}

HERROR NX_RECORDER_StartRec(NX_Channel_t *pstChannel, HINT32 nStartOffset, HUINT32 ulDuration, OxMediaRec_TargetStorage_e eStorageType, HUINT8 *szStorageId)
{
	HERROR					hErr = ERR_FAIL;
	DxService_t		 		*pstFoundService = NULL;
	HUINT32 ulCurrTime = 0, ulStartTime = 0, ulRecTime = 0, uluid;
	OxMediaRec_StartInfo_t 	*pstStartInfo = NULL;
	HCHAR					szName[256];
	HINT32					eventId = 0;
	HUINT32					ulRecSessionId = 0;
	HCHAR					szRecordID[256] = {'\0',};

	HxLOG_Trace();

	pstStartInfo = (OxMediaRec_StartInfo_t*)NX_APP_Calloc(sizeof(OxMediaRec_StartInfo_t));
	if (pstStartInfo == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() Fail!! \n");
		return ERR_FAIL;
	}

	uluid = pstChannel->svcUid;

	pstStartInfo->ulMasterSvcUid		= uluid;
	pstStartInfo->ulSuppleSvcUid		= uluid;
	pstStartInfo->eSuppSvcType			= eOxMP_SUPPLESVCTYPE_Normal;
	pstStartInfo->nStartOffset			= nStartOffset;
	pstStartInfo->eStorageType			= eStorageType;
	ulCurrTime 			= HLIB_STD_GetSystemTime();

	if (nStartOffset <= 0)
	{
		// Instant Rec ...
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrTime, &(pstStartInfo->stStartTime));
		ulStartTime = ulCurrTime;
	}
	else
	{
		// Delayed Rec ...
		nx_recorder_GetMaxOffset(pstChannel, &ulRecTime);
		ulStartTime = ulCurrTime - ulRecTime + nStartOffset;
		HxLOG_Debug("ulRecTime(%d), nStartOffset(%d), ulStartTime(%d)\n", ulRecTime, nStartOffset, ulStartTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &(pstStartInfo->stStartTime));
	}

	HxSTD_MemSet(pstStartInfo->szStorageId, 0, sizeof(pstStartInfo->szStorageId));
	if (szStorageId != NULL)
		HLIB_STD_StrNCpySafe(pstStartInfo->szStorageId, (HCHAR*)szStorageId, sizeof(pstStartInfo->szStorageId));

	pstStartInfo->ulDuration	= ulDuration;

	HxSTD_MemSet(szName, 0, 256);

	pstFoundService = NX_CHANNEL_FindServiceBySvcUid(pstChannel->svcUid);

	hErr = APK_META_QUERY_FindEventName(uluid, ulStartTime, ulStartTime + ulDuration, szName, eventId);
	if (ERR_OK == hErr)
	{
		if(HxSTD_StrLen(szName) > 0)
		{
			HxLOG_Error ("APK_META_QUERY_GetEventNameByStartTime (Event Name: %s) \n", szName);
			HLIB_STD_StrNCpySafe(pstStartInfo->szName, szName, sizeof(pstStartInfo->szName));
			pstStartInfo->nEvtId = eventId;
		}
		else
		{
			HxLOG_Error ("APK_META_QUERY_GetEventNameByStartTime Error (Svcname: %s) \n", pstFoundService->name);
			HLIB_STD_StrNCpySafe(pstStartInfo->szName, pstFoundService->name, sizeof(pstStartInfo->szName));
		}
	}
	else
	{
		HxLOG_Error ("APK_META_QUERY_GetEventNameByStartTime Error (Svcname: %s) \n", pstFoundService->name);
		HLIB_STD_StrNCpySafe(pstStartInfo->szName, pstFoundService->name, sizeof(pstStartInfo->szName));
	}

	hErr = APK_MEDIA_RECORD_Start ( pstStartInfo, &ulRecSessionId, szRecordID);

	if (ERR_OK != hErr)
	{
		if (pstStartInfo)
		{
			NX_APP_Free(pstStartInfo);
			pstStartInfo = NULL;
		}
		HxLOG_Error ("APK_MEDIA_RECORD_Start err: \n");
		return ERR_FAIL;
	}

    if (pstStartInfo)
    {
        NX_APP_Free(pstStartInfo);
        pstStartInfo = NULL;
    }

	HxLOG_Error("\nSTARTING RECORDING..!! \n");

	return ERR_OK;
}

HERROR NX_RECORDER_RecordNow(NX_Channel_t *pstChannel, HUINT32 ulDurationSec)
{
	NX_VideoResource_t 	*pstVideoRes = NX_AV_GetVideoResInform(0);

	if ((pstVideoRes->eTunningState == eNxTunningState_STARTED)
		&& ((pstVideoRes->ePlayState == eNxPlayState_CONNECTING) || (pstVideoRes->ePlayState == eNxPlayState_PRESENTING)))
	{
		return NX_RECORDER_StartRec(pstChannel, 0, ulDurationSec, eOxMR_TargetStorage_IntHdd, NULL);
	}
	else
	{
		HxLOG_Warning("not available status to record = %d\n", pstVideoRes->eTunningState);
	}

	return ERR_OK;
}

HERROR NX_RECORDER_RecordNowAt(NX_Channel_t *pstChannel, HUINT32 ulDurationSec, OxMediaRec_TargetStorage_e eTarget)
{
	HERROR	hErr = ERR_FAIL;

	hErr = NX_RECORDER_StartRec(pstChannel, 0, ulDurationSec, eTarget, NULL);

	return hErr;
}

HERROR NX_RECORDER_StopRecording(HUINT32 ulSessionId)
{
	HERROR		hErr = ERR_FAIL;

	HxLOG_Warning("Stop Record session[%d]\n", ulSessionId);

	hErr = APK_MEDIA_RECORD_Stop(ulSessionId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Failed Stop Recording!!! ulRecSessionId(0x%x)\n", ulSessionId);
		return ERR_FAIL;
	}

	HxLOG_Error("Stopped Recording!!\n");

	return ERR_OK;
}

#define	___________RELATED_RECORDING_ITEM_____________
HERROR	NX_RECORDER_ReloadRecItem(void)
{
	NxRecMgrContext_t *pstMgrContext = nx_recorder_mgr_GetContext();

	return nx_recorder_mgr_LoadRecordItem(pstMgrContext);
}

HxVector_t *NX_RECORDER_GetRecItemList(void)
{
	NxRecMgrContext_t *pstMgrContext = nx_recorder_mgr_GetContext();

	if (pstMgrContext != NULL)
		return pstMgrContext->recItemList;
	else
		return NULL;
}

HERROR	NX_RECORDER_GetRecItems(HxVector_t **ddstList, HINT32 *nListNum)
{
	NxRecMgrContext_t *pstMgrContext = nx_recorder_mgr_GetContext();

	if (ddstList == NULL)
	{
		HxLOG_Error("ddstList is NULL!\n");
		return ERR_FAIL;
	}

	if (pstMgrContext != NULL && pstMgrContext->recItemList)
	{
		*ddstList = pstMgrContext->recItemList;
		*nListNum = HLIB_VECTOR_Length(pstMgrContext->recItemList);
		return ERR_OK;
	}
	else
		return ERR_FAIL;
}

DxRecListData_t* NX_RECORDER_GetByCID(const HUINT32 cid)
{
	return NX_PORT_META_RECORD_GetByCID(cid);
}

HCHAR 	*NX_RECORDER_GetThumbnail(HCHAR *szUrl, HUINT32 ulExtractPosition, const HCHAR *pszMountPath, HBOOL bRetExpectURL)
{
	return nx_recorder_mgr_GetThumbnail(szUrl, ulExtractPosition, pszMountPath, bRetExpectURL);
}

#if 0
HERROR NX_RECORDER_PlayRec(void)
{
	OxMediaPlay_StartInfo_t	*pstStartInfo = NULL;
	pstStartInfo = (OxMediaPlay_StartInfo_t*)NX_APP_Calloc(sizeof(OxMediaPlay_StartInfo_t));

	pstStartInfo->stPvrPb.eType = eOxMP_MEDIATYPE_PVRPLAYBACK;
	pstStartInfo->stPvrPb.ulContentId = APK_META_RECORD_GetCID( const_cast<char *>(m_url.c_str()) );
	pstStartInfo->stPvrPb.ulStartTime = 0;

	NX_PORT_MEDIA_PLAY_Start (ulViewId, &unStart, &ulSessionId);

}
#endif



#define ______________RELATED_RECORDER_COMMAND____________________
/***************************************************************************
Related Search Command
***************************************************************************/
#if defined(CONFIG_DEBUG)

static HINT32 cmdGetRecItems(void *szArgs)
{
	HINT32	i = 0, nRecItemLen = 0;
	NxRecMgrContext_t *pstMgrContext = nx_recorder_mgr_GetContext();

	if (pstMgrContext->recItemList == NULL)
	{
		HxLOG_Error("\n recording item is 0!!\n\n");
		return 0;
	}

	nRecItemLen = HLIB_VECTOR_Length(pstMgrContext->recItemList);

	for (i = 0; i < nRecItemLen; i++)
	{
		NxRecItem_t *pstNxRecItem = (NxRecItem_t*)HLIB_VECTOR_ItemAt(pstMgrContext->recItemList, i);

		if (pstNxRecItem)
		{
			DxRecListData_t *pstRecData = (DxRecListData_t*)APK_META_RECORD_GetByCID(pstNxRecItem->ulContentId);
			if(pstRecData)
			{
				HxLOG_Error("\ncid(%u), storageId(%s), name(%s), svcName(%s), szUrl(%s), thumbnailPath(%s)\n",
					pstRecData->ulContentId, pstRecData->szStorageId, pstRecData->szName,
					pstRecData->szSvcName, pstRecData->szUrl, pstRecData->szThumbnailPath);
			}
			else
			{
				HxLOG_Error("\nis cid (%u), but DxRecListData_t is NULL!\n", pstNxRecItem->ulContentId);
			}
		}
	}

	return HxCMD_OK;
}

static HINT32 cmdPlayRecItem(void *szArgs)
{
	HUINT32	playCID = 0;
	DxRecListData_t *pstRecData = NULL;

	char			 *aucArg=NULL;

	NxRecMgrContext_t *pstMgrContext = nx_recorder_mgr_GetContext();

	if (pstMgrContext->recItemList == NULL)
	{
		HxLOG_Error("\n recording item is 0!!\n\n");
		return 0;
	}

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &playCID) == 0 )
			return HxCMD_ERR;
	}

	pstRecData = (DxRecListData_t*)NX_PORT_META_RECORD_GetByCID(playCID);

	if (pstRecData)
	{
		HUINT32 ulSessionId = 0;
		OxMediaPlay_StartInfo_t	*pstStartInfo = NULL;

		HxLOG_Print("\n\n[%s.%d] Play CID(%u) (%s)\n\n",__FUNCTION__,__LINE__,
			pstRecData->ulContentId, pstRecData->szUrl);


		pstStartInfo = (OxMediaPlay_StartInfo_t*)NX_APP_Calloc(sizeof(OxMediaPlay_StartInfo_t));

		pstStartInfo->stPvrPb.eType = eOxMP_MEDIATYPE_PVRPLAYBACK;
		pstStartInfo->stPvrPb.ulContentId = playCID;
		pstStartInfo->stPvrPb.ulStartTime = 0;

		NX_PORT_MEDIA_PLAY_Start (0, pstStartInfo, &ulSessionId);
	}
	else
	{
		HxLOG_Print("\n\n[%s.%d] NotFound Cid RecITEM (%u) \n\n",__FUNCTION__,__LINE__, playCID);
	}

	return HxCMD_OK;
}


static void nx_recorder_Command_Init(void)
{
#define cmdHandle		"nxrecordercmd"

	HLIB_CMD_RegisterWord(cmdHandle, cmdGetRecItems,
						 (HINT8 *)"getRecItems",
						 (HINT8 *)"print record item list.",
						 (HINT8 *)"getRecItems");
	HLIB_CMD_RegisterWord(cmdHandle, cmdPlayRecItem,
						 (HINT8 *)"playrecitem",
						 (HINT8 *)"try play record item",
						 (HINT8 *)"playrecitem");
}

#endif		// #if defined(CONFIG_DEBUG)

