/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_download.c
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
#include <oapi.h>
#include <obama_int.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#define OAPI_DOWNLOAD_SEM_GET			HxSEMT_Get		(s_stOapiDnld_Context.ulSemId)
#define OAPI_DOWNLOAD_SEM_RELEASE		HxSEMT_Release	(s_stOapiDnld_Context.ulSemId)
#define OAPI_DOWNLOAD_CACHE_NUM			10

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	OxDownload_Notifier_t	 pfListener;
	void					*pvUserData;
} oapiDnldNotifier_t;

typedef struct
{
	HUINT32					 ulDownloadId;
	DxDownload_Type_e		 eType;
	OxDownload_State_e		 eState;
	DxDownload_Start_t		 unStartInfo;

	HUINT32					 ulStartedSec;
	HUINT32					 ulStoppedSec;
	DxDownload_Status_t		 stStatusCache;

	OxDownload_BufferInfo_t	 stBufferInfo;
	OxDownload_SeekableInfo_t stSeekableInfo;

} oapiDnldInstance_t;

typedef struct
{
	HBOOL						 bActivated;

	HINT32						 nRpcHandle;
	HUINT32						 ulSemId;
	HxList_t					*pstInstList;			// oapiDnldInstance_t
	HxList_t					*pstFinishedInstList;	// oapiDnldInstance_t
	HxList_t					*pstNotiList;			// oapiDnldNotifier_t
	OxDownload_ActivatedCb_t	 pfCbActivated;

	/*CACHE*/
	OxDownload_Cache_t		stCache[OAPI_DOWNLOAD_CACHE_NUM]; // download 갯수. define 해야 함. _om_download.h : omDownload_Context_t 와 동기화 시켜야 함.
} oapiDnldContext_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL					 s_bOapiDnld_Inited = FALSE;
STATIC oapiDnldContext_t		 s_stOapiDnld_Context;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
STATIC HCHAR *oapi_dnld_GetEventString (OxMediaRec_Event_e eEvent)
{
	switch (eEvent)
	{
	ENUM_TO_STR (eDxDNLD_EVENT_Started);
	ENUM_TO_STR (eDxDNLD_EVENT_Completed);
	ENUM_TO_STR (eDxDNLD_EVENT_Failed);
	ENUM_TO_STR (eDxDNLD_EVENT_DataReceiving);
	ENUM_TO_STR (eDxDNLD_EVENT_ProbeSuccess);
	ENUM_TO_STR (eDxDNLD_EVENT_ProbeFailed);
	ENUM_TO_STR (eDxDNLD_EVENT_DownloadInfoChanged);	
	default:
		break;
	}

	return "Unknown";
}

STATIC HCHAR *oapi_dnld_GetStateString (OxMediaRec_RecState_e eState)
{
	switch (eState)
	{
	ENUM_TO_STR (eOxMR_RECSTATE_NONE);
	ENUM_TO_STR (eOxMR_RECSTATE_STOPPING);
	ENUM_TO_STR (eOxMR_RECSTATE_STOPPED);
	ENUM_TO_STR (eOxMR_RECSTATE_REQUESTED);
	ENUM_TO_STR (eOxMR_RECSTATE_CONNECTING);
	ENUM_TO_STR (eOxMR_RECSTATE_RECORDING);
	default:
		break;
	}

	return "Unknown";
}

STATIC oapiDnldContext_t *oapi_dnld_GetContext (void)
{
	return &s_stOapiDnld_Context;
}

STATIC oapiDnldInstance_t *oapi_dnld_GetDownloadInstance (HUINT32 ulDownloadId, HBOOL bIncludeFinished)
{
	HxList_t			*pstListItem;
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();

	for (pstListItem = pstContext->pstInstList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiDnldInstance_t			*pstInst = (oapiDnldInstance_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstInst) && (pstInst->ulDownloadId == ulDownloadId))
		{
			return pstInst;
		}
	}	

	if (TRUE == bIncludeFinished)
	{
		for (pstListItem = pstContext->pstFinishedInstList; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			oapiDnldInstance_t			*pstInst = (oapiDnldInstance_t *)HLIB_LIST_Data(pstListItem);

			if ((NULL != pstInst) && (pstInst->ulDownloadId == ulDownloadId))
			{
				return pstInst;
			}
		}
	}

	return NULL;
}

STATIC void oapi_dnld_NotifyToListener (HUINT32 ulDownloadId, DxDownload_Event_e eEvent, DxDownload_NotifierData_t *punNotifyData)
{
	HxList_t			*pstListItem;
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();
	oapiDnldInstance_t	*pstInst = NULL;
	DxDownload_Type_e	eType;
	OAPI_DOWNLOAD_SEM_GET;	
	pstInst = oapi_dnld_GetDownloadInstance(ulDownloadId, TRUE);
	if (NULL == pstInst)
	{
		OAPI_DOWNLOAD_SEM_RELEASE;
		return;
	}
	eType = pstInst->eType;
	OAPI_DOWNLOAD_SEM_RELEASE;

	for (pstListItem = pstContext->pstNotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiDnldNotifier_t			*pstNotifier = (oapiDnldNotifier_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotifier) && (NULL != pstNotifier->pfListener))
		{
			HxLOG_Debug("EVENT : (%s)\n", oapi_dnld_GetEventString(eEvent));
			(pstNotifier->pfListener)(ulDownloadId, eType, eEvent, punNotifyData, pstNotifier->pvUserData);
		}
	}
}

STATIC oapiDnldNotifier_t *oapi_dnld_FindNotifier (OxDownload_Notifier_t pfListener, void *pvUserData)
{
	HxList_t			*pstListItem;
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();

	for (pstListItem = pstContext->pstNotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiDnldNotifier_t			*pstNotifier = (oapiDnldNotifier_t *)HLIB_LIST_Data (pstListItem);

		if ((pfListener == pstNotifier->pfListener) &&
			(pvUserData == pstNotifier->pvUserData))
		{
			return pstNotifier;
		}
	}

	return NULL;
}

STATIC HUINT32 oapi_dnld_GetNextDownloadId (void)
{
	STATIC HUINT32			 s_ulLastCnt = 0x0001;
	HUINT32					 ulNextCnt = s_ulLastCnt;
	HUINT32					 ulDownloadId;
	HUINT32					 ulMask = (HLIB_STD_GetPID() & 0xFFFF) << 16;

	do
	{
		ulNextCnt++;
		if (ulNextCnt >= 0xFFFF)				{ ulNextCnt = 0x0001; }

		ulDownloadId = (ulMask | ulNextCnt);
		if (oapi_dnld_GetDownloadInstance(ulDownloadId, TRUE) == NULL)
		{
			s_ulLastCnt = ulNextCnt;
			return ulDownloadId;
		}
	} while (ulNextCnt != s_ulLastCnt);

	return 0;
}

STATIC HERROR oapi_dnld_AppendInstance (oapiDnldInstance_t *pstInst)
{
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();

	if (NULL == pstInst)						{ return ERR_FAIL; }

	pstInst->ulStartedSec = HLIB_STD_GetSystemTick() / 1000;
	pstContext->pstInstList = HLIB_LIST_Append(pstContext->pstInstList, (void *)pstInst);
	return ERR_OK;
}

STATIC HERROR oapi_dnld_RemoveInstance (oapiDnldInstance_t *pstInst)
{
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();

	if (NULL == pstInst)						{ return ERR_FAIL; }

	pstInst->ulStoppedSec = HLIB_STD_GetSystemTick() / 1000;
	pstContext->pstInstList = HLIB_LIST_Remove(pstContext->pstInstList, (const void *)pstInst);
	pstContext->pstFinishedInstList = HLIB_LIST_Append(pstContext->pstFinishedInstList, (void *)pstInst);
	return ERR_OK;
}

#define _____NOTIFICATION_CALLBACKS_____
STATIC HERROR oapi_dnld_CbStarted (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulDownloadId;
// 	HUINT32				 ulSessionId;
	DxDownload_Start_t	*punStart;
	oapiDnldInstance_t	*pstInst;

	DxDownload_SessionInfo_t stDnldSessionInfo;

	if (3 == nArgc)
	{
		ulDownloadId	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		stDnldSessionInfo.ulSessionId		= (HUINT32)HxOBJECT_INT(apArgv[1]);
		punStart		= (DxDownload_Start_t *)HxOBJECT_BIN_DATA(apArgv[2]);

		OAPI_DOWNLOAD_SEM_GET;
		pstInst = oapi_dnld_GetDownloadInstance(ulDownloadId, FALSE);
		if (NULL == pstInst)
		{
			pstInst = (oapiDnldInstance_t *)HLIB_STD_MemCalloc(sizeof(oapiDnldInstance_t));
			if (NULL == pstInst)
			{
				HxLOG_Error("memory allocation failed:\n");
				OAPI_DOWNLOAD_SEM_RELEASE;
				return ERR_FAIL;
			}

			pstInst->ulDownloadId = ulDownloadId;
			pstInst->eType = punStart->eType;
			HxSTD_MemCopy(&(pstInst->unStartInfo), punStart, sizeof(DxDownload_Start_t));

			(void)oapi_dnld_AppendInstance(pstInst);
		}

		pstInst->eState = eOxDNLD_STATE_Started;
		OAPI_DOWNLOAD_SEM_RELEASE;

		oapi_dnld_NotifyToListener(ulDownloadId, eDxDNLD_EVENT_Started, (DxDownload_NotifierData_t *)&stDnldSessionInfo);
	}

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbCompleted (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulDownloadId;
	oapiDnldInstance_t	*pstInst;

	if (nArgc == 1)
	{
		ulDownloadId	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		OAPI_DOWNLOAD_SEM_GET;
		pstInst = oapi_dnld_GetDownloadInstance(ulDownloadId, FALSE);
		if (NULL != pstInst)
		{
			pstInst->eState = eOxDNLD_STATE_Finished;
			//(void)oapi_dnld_RemoveInstance(pstInst);
		}
		OAPI_DOWNLOAD_SEM_RELEASE;
		oapi_dnld_NotifyToListener(ulDownloadId, eDxDNLD_EVENT_Completed, NULL);
	}

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbFailed (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulDownloadId;
	oapiDnldInstance_t	*pstInst;

	if (nArgc == 1)
	{
		ulDownloadId	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		OAPI_DOWNLOAD_SEM_GET;
		pstInst = oapi_dnld_GetDownloadInstance(ulDownloadId, FALSE);
		if (NULL != pstInst)
		{
			pstInst->eState = eOxDNLD_STATE_Finished;
			//(void)oapi_dnld_RemoveInstance(pstInst);
		}
		OAPI_DOWNLOAD_SEM_RELEASE;
		oapi_dnld_NotifyToListener(ulDownloadId, eDxDNLD_EVENT_Failed, NULL);
	}

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbDataReceiving (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulDownloadId;
	oapiDnldInstance_t	*pstInst;
	DxDownload_Status_t	*pstStatus;

	HxLOG_Debug("TimeCode Changed...\n");
	if (nArgc == 2)
	{
		ulDownloadId	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		pstStatus		= (DxDownload_Status_t *)HxOBJECT_BIN_DATA(apArgv[1]);
		OAPI_DOWNLOAD_SEM_GET;
		pstInst = oapi_dnld_GetDownloadInstance(ulDownloadId, FALSE);
		if (NULL != pstInst)
		{
			HxSTD_MemCopy(&(pstInst->stStatusCache), pstStatus, sizeof(DxDownload_Status_t));
		}
		OAPI_DOWNLOAD_SEM_RELEASE;
		oapi_dnld_NotifyToListener(ulDownloadId, eDxDNLD_EVENT_DataReceiving, (DxDownload_NotifierData_t *)pstStatus);
	}

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbProbeSuccess (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulDownloadId;
	oapiDnldInstance_t	*pstInst;
	DxDownload_SessionInfo_t stDnldSessionInfo;

	if (nArgc == 7)
	{
		OxDownload_SeekableInfo_t *pstSeekableInfo = NULL;
		ulDownloadId						= (HUINT32)HxOBJECT_INT(apArgv[0]);
		stDnldSessionInfo.ulSessionId		= (HUINT32)HxOBJECT_INT(apArgv[1]);
#if defined(CONFIG_TIMESTAMP_64)
		stDnldSessionInfo.ullDuration		= (HUINT64)HxOBJECT_INT64(apArgv[2]);
		stDnldSessionInfo.ullStartPos		= (HUINT64)HxOBJECT_INT64(apArgv[3]);
#else
		stDnldSessionInfo.ulDuration		= (HUINT32)HxOBJECT_INT(apArgv[2]);
		stDnldSessionInfo.ulStartPos		= (HUINT32)HxOBJECT_INT(apArgv[3]);
#endif
		stDnldSessionInfo.ulMaxBitrate		= (HUINT32)HxOBJECT_INT(apArgv[4]);
		stDnldSessionInfo.bInfinity			= (HBOOL)HxOBJECT_INT(apArgv[5]);
		pstSeekableInfo						= (OxDownload_SeekableInfo_t*)HxOBJECT_BIN_DATA(apArgv[6]);

		OAPI_DOWNLOAD_SEM_GET;
		pstInst = oapi_dnld_GetDownloadInstance(ulDownloadId, FALSE);
		if (NULL != pstInst)
		{
			pstInst->eState = eOxDNLD_STATE_Started;
			pstInst->ulDownloadId = ulDownloadId;
			(void)oapi_dnld_AppendInstance(pstInst);
			
			/* CACHE UPDATE */
			HxSTD_MemCopy(&(pstInst->stBufferInfo.stSeekableInfo), pstSeekableInfo, sizeof(OxDownload_SeekableInfo_t));
			HxLOG_Debug("[%s](L:%d) ---------------------------------- \n", __FUNCTION__, __LINE__);
			HxLOG_Debug("[%s](L:%d) ulSeekableChunks(%u)\n", __FUNCTION__, __LINE__, pstInst->stBufferInfo.stSeekableInfo.ulSeekableChunks);
			HxLOG_Debug("[%s](L:%d) ---------------------------------- \n", __FUNCTION__, __LINE__);
			
		}
		OAPI_DOWNLOAD_SEM_RELEASE;
		oapi_dnld_NotifyToListener(ulDownloadId, eDxDNLD_EVENT_ProbeSuccess, (DxDownload_NotifierData_t *)&stDnldSessionInfo);
	}

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbProbeFailed (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulDownloadId;
	oapiDnldInstance_t	*pstInst;
	OxMediaPlay_ErrorReason_e		eFailReason;
	DxDownload_SessionInfo_t stDnldSessionInfo;

	if (nArgc == 2)
	{
		ulDownloadId	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		eFailReason		= (OxMediaPlay_ErrorReason_e)HxOBJECT_INT(apArgv[1]);
		OAPI_DOWNLOAD_SEM_GET;
		pstInst = oapi_dnld_GetDownloadInstance(ulDownloadId, FALSE);
		if (NULL != pstInst)
		{
			pstInst->eState = eOxDNLD_STATE_Finished;
			//(void)oapi_dnld_RemoveInstance(pstInst);
		}
		OAPI_DOWNLOAD_SEM_RELEASE;
		HxSTD_MemSet( &stDnldSessionInfo, 0x00, sizeof(DxDownload_SessionInfo_t));
		stDnldSessionInfo.ulErrorType = (HUINT32)eDxDNLD_EVENT_ProbeFailed;
		stDnldSessionInfo.ulErrorReason = (HUINT32)eFailReason;

		oapi_dnld_NotifyToListener(ulDownloadId, eDxDNLD_EVENT_ProbeFailed, (DxDownload_NotifierData_t *)&stDnldSessionInfo);
	}

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbOnDownloadInfoChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulDownloadId, ulSessionId;
	HBOOL						bMhegIcs = FALSE;
	oapiDnldInstance_t			*pstInst = NULL;

	DxDownload_SessionInfo_t 	stDnldSessionInfo;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		OxDownload_BufferInfo_t *pstBufferInfo	=	NULL;
		
		HxSTD_MemSet( &stDnldSessionInfo, 0x00, sizeof(DxDownload_SessionInfo_t));
		
		ulDownloadId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		// apArgv[1] not used ;
		pstBufferInfo	=	(OxDownload_BufferInfo_t*)HxOBJECT_BIN_DATA(apArgv[3]);

		OAPI_DOWNLOAD_SEM_GET;

		pstInst = oapi_dnld_GetDownloadInstance(ulDownloadId, FALSE);
		if (NULL != pstInst)
		{
			bMhegIcs = pstInst->unStartInfo.stIpMedia.bMhegIcsStream;
		
			/* CACHE UPDATE */
			HxSTD_MemCopy(&(pstInst->stBufferInfo), pstBufferInfo, sizeof(OxDownload_BufferInfo_t));
			HxLOG_Debug("[%s](L:%d) ---------------------------------- \n", __FUNCTION__, __LINE__);
			HxLOG_Debug("[%s](L:%d) DownloadInfo(%u:Handle_%u) \n", __FUNCTION__, __LINE__, ulSessionId,ulDownloadId);
			HxLOG_Debug("[%s](L:%d) ---------------------------------- \n", __FUNCTION__, __LINE__);
			HxLOG_Debug("[%s](L:%d) nArrivalBytedPerSec(%d) \n", __FUNCTION__, __LINE__, pstInst->stBufferInfo.nArrivalBytedPerSec);
			HxLOG_Debug("[%s](L:%d) ulBufferPercent(%u)\n", __FUNCTION__, __LINE__, pstInst->stBufferInfo.ulBufferPercent);
			HxLOG_Debug("[%s](L:%d) unRemainBufferingTime(%u)\n", __FUNCTION__, __LINE__, pstInst->stBufferInfo.unRemainBufferingTime);
			HxLOG_Debug("[%s](L:%d) llBufferedBytes(%lld) \n", __FUNCTION__, __LINE__, pstInst->stBufferInfo.llBufferedBytes);
			HxLOG_Debug("[%s](L:%d) llTotalBytesRemaining(%lld) \n", __FUNCTION__, __LINE__, pstInst->stBufferInfo.llTotalBytesRemaining);
			HxLOG_Debug("[%s](L:%d) llTotalStreamBytes(%lld) \n", __FUNCTION__, __LINE__, pstInst->stBufferInfo.llTotalStreamBytes);
			HxLOG_Debug("[%s](L:%d) nStreamBytesPerSec(%d) \n", __FUNCTION__, __LINE__, pstInst->stBufferInfo.nStreamBytesPerSec);
			HxLOG_Debug("[%s](L:%d) ulSeekableChunks(%u)\n", __FUNCTION__, __LINE__, pstInst->stBufferInfo.stSeekableInfo.ulSeekableChunks);
			HxLOG_Debug("[%s](L:%d) ---------------------------------- \n\n", __FUNCTION__, __LINE__);

			/* EMIT EVENT */
			HxLOG_Trace();
		}
		
		OAPI_DOWNLOAD_SEM_RELEASE;

		if(bMhegIcs)
		{
			oapi_dnld_NotifyToListener (ulDownloadId, eDxDNLD_EVENT_DownloadInfoChanged, (DxDownload_NotifierData_t *)&stDnldSessionInfo);
		}
	}

	return ERR_OK;
}

STATIC void oapi_dnld_CleanCache(HUINT32 ulSessionId)
{
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();
	if( NULL == pstContext )
		return ;

	HxSTD_MemSet(&(pstContext->stCache[ulSessionId].stVideoCompTable),		0, sizeof(OxDownload_VideoCompTable_t) );
	HxSTD_MemSet(&(pstContext->stCache[ulSessionId].stAudioCompTable),		0, sizeof(OxDownload_AudioCompTable_t) );
	HxSTD_MemSet(&(pstContext->stCache[ulSessionId].stSubtitleCompTable),	0, sizeof(OxDownload_SubtitleCompTable_t) );
	HxSTD_MemSet(&(pstContext->stCache[ulSessionId].stDataCompTable),		0, sizeof(OxDownload_DataCompTable_t) );

	pstContext->stCache[ulSessionId].ulDownloadId = 0;

	return ;
}

STATIC HERROR oapi_dnld_CbStopped (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 			 ulDownloadId;
	HUINT32 			 ulSessionId;

	if (2 == nArgc)
	{
		ulDownloadId	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		ulSessionId		= (HUINT32)HxOBJECT_INT(apArgv[1]);

		OAPI_DOWNLOAD_SEM_GET;

		/* CACHE UPDATE */
		oapi_dnld_CleanCache(ulSessionId);

		OAPI_DOWNLOAD_SEM_RELEASE;

	}

	return ERR_OK;
}

STATIC void	oapi_dnld_PrintComponent(OxDownload_Component_e eType, OxDownload_Component_t *pstComponent)
{
	if (pstComponent == NULL)
		return;

	HxLOG_Debug("---- oapi_dnld_PrintComponent, eType :%d, \n", eType);

	switch (eType)
	{
		case eOxDNLD_COMPONENT_SUBTITLE:
			{
				switch (pstComponent->stSubtitle.eSubtitleType)
				{
					case eOxDNLD_SUBTITLETYPE_Dvb:
						HxLOG_Debug("\tDVB : PID[0x%x] LangCode[%s]\n", pstComponent->stSubtitle.nPid, pstComponent->stSubtitle.utArg.stDvbInfo.szLangCode);
						break;
					case eOxDNLD_SUBTITLETYPE_Ebu:
						HxLOG_Debug("\tEBU : PID[0x%x] LangCode[%s] Magazine[%d] Page[%x]\n", pstComponent->stSubtitle.nPid, pstComponent->stSubtitle.utArg.stEbuInfo.szLangCode, pstComponent->stSubtitle.utArg.stEbuInfo.nMagazineNum, pstComponent->stSubtitle.utArg.stEbuInfo.nPageNum);
						break;
					case eOxDNLD_SUBTITLETYPE_Media:
						HxLOG_Debug("\tMEDIA : PID[0x%x] \n", pstComponent->stSubtitle.nPid);
						break;
					default:
						break;
				}
			}
			break;
		case eOxDNLD_COMPONENT_VIDEO:
			HxLOG_Debug("\tDVB : PID[0x%x] LangCode[%s]\n", pstComponent->stVideo.nPid, pstComponent->stVideo.aucStreamName);
			break;
		case eOxDNLD_COMPONENT_AUDIO:
			HxLOG_Debug("\tDVB : PID[0x%x] LangCode[%s]\n", pstComponent->stAudio.nPid, pstComponent->stAudio.aucStreamName);
			break;
		case eOxDNLD_COMPONENT_TELETEXT:
		case eOxDNLD_COMPONENT_CLOSEDCAPTION:
		case eOxDNLD_COMPONENT_DATA:
			break;
		default:
			break;
	}

}

STATIC HERROR oapi_dnld_CbOnComponentVideoChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 				ulDownloadId, ulSessionId;
	oapiDnldContext_t		*pstContext = oapi_dnld_GetContext();

	if (nArgc == 3)
	{
		OxDownload_VideoCompTable_t *pstVideoCompTable	=	NULL;

		ulDownloadId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId		= (HUINT32)HxOBJECT_INT (apArgv[1]);
		pstVideoCompTable	= (OxDownload_VideoCompTable_t*)HxOBJECT_BIN_DATA (apArgv[2]);

		OAPI_DOWNLOAD_SEM_GET;
		/* CACHE UPDATE */
		pstContext->stCache[ulSessionId].ulDownloadId = ulDownloadId;
		HxSTD_MemCopy(&(pstContext->stCache[ulSessionId].stVideoCompTable), pstVideoCompTable, sizeof(OxDownload_VideoCompTable_t) );
		OAPI_DOWNLOAD_SEM_RELEASE;

#if defined(CONFIG_DEBUG)
{
		HINT32 i=0;

		HxLOG_Debug("--VIDEO TRACK LIST--\n");
		for( i=0 ; i<pstContext->stCache[ulSessionId].stVideoCompTable.nArraySize ; i++ )
			oapi_dnld_PrintComponent (eOxDNLD_COMPONENT_VIDEO, (OxDownload_Component_t*)&pstContext->stCache[ulSessionId].stVideoCompTable.astCompArray[i]);
}
#endif
		oapi_dnld_NotifyToListener (ulDownloadId, eDxDNLD_EVENT_COMPONENT_VIDEO_CHANGED, NULL);
	}

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbOnComponentAudioChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 				ulDownloadId, ulSessionId;
	oapiDnldContext_t		*pstContext = oapi_dnld_GetContext();

	if (nArgc == 3)
	{
		OxDownload_AudioCompTable_t *pstAudioCompTable	=	NULL;

		ulDownloadId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId		= (HUINT32)HxOBJECT_INT (apArgv[1]);
		pstAudioCompTable	= (OxDownload_AudioCompTable_t*)HxOBJECT_BIN_DATA (apArgv[2]);

		OAPI_DOWNLOAD_SEM_GET;
		/* CACHE UPDATE */
		pstContext->stCache[ulSessionId].ulDownloadId = ulDownloadId;
		HxSTD_MemCopy(&(pstContext->stCache[ulSessionId].stAudioCompTable), pstAudioCompTable, sizeof(OxDownload_AudioCompTable_t) );
		OAPI_DOWNLOAD_SEM_RELEASE;

#if defined(CONFIG_DEBUG)
{
		HINT32 i=0;

		HxLOG_Debug("--AUDIO TRACK LIST--\n");
		for( i=0 ; i<pstContext->stCache[ulSessionId].stAudioCompTable.nArraySize ; i++ )
			oapi_dnld_PrintComponent (eOxDNLD_COMPONENT_AUDIO, (OxDownload_Component_t*)&pstContext->stCache[ulSessionId].stAudioCompTable.astCompArray[i]);
}
#endif
		oapi_dnld_NotifyToListener (ulDownloadId, eDxDNLD_EVENT_COMPONENT_AUDIO_CHANGED, NULL);
	}

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbOnComponentSubtitleChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 				ulDownloadId, ulSessionId;
	oapiDnldContext_t		*pstContext = oapi_dnld_GetContext();

	if (nArgc == 3)
	{
		OxDownload_SubtitleCompTable_t *pstSubtitleCompTable	=	NULL;

		ulDownloadId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId		= (HUINT32)HxOBJECT_INT (apArgv[1]);
		pstSubtitleCompTable	= (OxDownload_SubtitleCompTable_t*)HxOBJECT_BIN_DATA (apArgv[2]);

		OAPI_DOWNLOAD_SEM_GET;
		/* CACHE UPDATE */
		pstContext->stCache[ulSessionId].ulDownloadId = ulDownloadId;
		HxSTD_MemCopy(&(pstContext->stCache[ulSessionId].stSubtitleCompTable), pstSubtitleCompTable, sizeof(OxDownload_SubtitleCompTable_t) );
		OAPI_DOWNLOAD_SEM_RELEASE;

#if defined(CONFIG_DEBUG)
{
		HINT32 i=0;

		HxLOG_Debug("--SUBTITLE TRACK LIST--\n");
		for( i=0 ; i<pstContext->stCache[ulSessionId].stSubtitleCompTable.nArraySize ; i++ )
			oapi_dnld_PrintComponent (eOxDNLD_COMPONENT_SUBTITLE, (OxDownload_Component_t*)&pstContext->stCache[ulSessionId].stSubtitleCompTable.astCompArray[i]);
}
#endif
		oapi_dnld_NotifyToListener (ulDownloadId, eDxDNLD_EVENT_COMPONENT_SUBTITLE_CHANGED, NULL);
	}

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbOnComponentDataChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulDownloadId, ulSessionId;
	oapiDnldContext_t			*pstContext = oapi_dnld_GetContext();

	if (nArgc == 3)
	{
		OxDownload_DataCompTable_t *pstDataCompTable	=	NULL;

		ulDownloadId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId		= (HUINT32)HxOBJECT_INT (apArgv[1]);
		pstDataCompTable	= (OxDownload_DataCompTable_t*)HxOBJECT_BIN_DATA (apArgv[2]);

		OAPI_DOWNLOAD_SEM_GET;
		/* CACHE UPDATE */
		pstContext->stCache[ulSessionId].ulDownloadId = ulDownloadId;
		HxSTD_MemCopy(&(pstContext->stCache[ulSessionId].stDataCompTable), pstDataCompTable, sizeof(OxDownload_DataCompTable_t) );
		OAPI_DOWNLOAD_SEM_RELEASE;

#if defined(CONFIG_DEBUG)
{
		HINT32 i=0;

		HxLOG_Debug("--DATA TRACK LIST--\n");
		for( i=0 ; i<pstContext->stCache[ulSessionId].stDataCompTable.nArraySize ; i++ )
			oapi_dnld_PrintComponent (eOxDNLD_COMPONENT_DATA, (OxDownload_Component_t*)&pstContext->stCache[ulSessionId].stDataCompTable.astCompArray[i]);
}
#endif
		oapi_dnld_NotifyToListener (ulDownloadId, eDxDNLD_EVENT_COMPONENT_DATA_CHANGED, NULL);
	}

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbOnAvailabilityStartTimeChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulDownloadId, ulSessionId;
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();

	HCHAR* pszStartTime =	NULL;
	
	ulDownloadId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
	pszStartTime	= (HCHAR*)HxOBJECT_STR (apArgv[2]);

	if(pszStartTime != NULL)
	{
		OAPI_DOWNLOAD_SEM_GET;
		/* CACHE UPDATE */
		pstContext->stCache[ulSessionId].ulDownloadId = ulDownloadId;
		HxSTD_snprintf(pstContext->stCache[ulSessionId].szAvailabilityStartTime, sizeof(pstContext->stCache[ulSessionId].szAvailabilityStartTime), "%s", pszStartTime);
		OAPI_DOWNLOAD_SEM_RELEASE;
	}
	return ERR_OK;
}


/* download 모듈에서 select 는 의미 없다. */
STATIC HERROR oapi_dnld_CbOnComponentSelected (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
#if 0
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 6)
	{
		OxMediaPlay_Component_e 	 eCompType	=	0;
		HINT32	nSelectIndex	=	0, nSelectSubIndex = 0;


		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		eCompType		= (OxMediaPlay_Component_e)HxOBJECT_INT(apArgv[3]);
		nSelectIndex	= (HINT32)HxOBJECT_INT(apArgv[4]);
		nSelectSubIndex	= (HINT32)HxOBJECT_INT(apArgv[5]);	// SelectSubIndex 는 Audio 일때만 사용됨.subtitle, video은 사용 안함..(-1)

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_EventNotify_t	 stEventNotify;

			stEventNotify.ulParam1	=	eCompType;
			stEventNotify.ulParam2	=	nSelectIndex;
			stEventNotify.ulParam3	=	0;

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			switch( eCompType )
			{
				case eOxMP_COMPONENT_VIDEO:
					pstContext->stCache[ulSessionId].stVideoCompTable.nCurrIndex	=	nSelectIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d \n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache[ulSessionId].stVideoCompTable.nCurrIndex);
					break;
				case eOxMP_COMPONENT_AUDIO:
					pstContext->stCache[ulSessionId].stAudioCompTable.nCurrIndex	=	nSelectIndex;
					pstContext->stCache[ulSessionId].stAudioCompTable.nCurrSubIndex	=	nSelectSubIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d , nCurrSubIndex = %d\n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache[ulSessionId].stAudioCompTable.nCurrIndex, pstContext->stCache[ulSessionId].stAudioCompTable.nCurrSubIndex);
					break;
				case eOxMP_COMPONENT_SUBTITLE:
					pstContext->stCache[ulSessionId].stSubtitleCompTable.nCurrIndex	=	nSelectIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d \n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache[ulSessionId].stSubtitleCompTable.nCurrIndex);
					break;
				case eOxMP_COMPONENT_DATA:
					pstContext->stCache[ulSessionId].stDataCompTable.nCurrIndex		=	nSelectIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d \n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache[ulSessionId].stDataCompTable.nCurrIndex);
					break;
				default:
					break;
			}


			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_COMPONENT_SELECTED, (OxMediaPlay_NotifierData_t *)&stEventNotify, TRUE);
		}
	}

#endif

	return ERR_OK;
}

STATIC HERROR oapi_dnld_CbOnActivated (HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	oapiDnldContext_t				*pstContext = oapi_dnld_GetContext();

	HxLOG_Debug("onActivated...\n");

	pstContext->bActivated = TRUE;

	// Activate the notifiers:
	HLIB_RPC_RegisterNotifyCallback(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnStarted	, "iib", NULL, oapi_dnld_CbStarted);
	HLIB_RPC_RegisterNotifyCallback(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnCompleted	, "i",  NULL, oapi_dnld_CbCompleted);
	HLIB_RPC_RegisterNotifyCallback(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnFailed		, "i",  NULL, oapi_dnld_CbFailed);
	HLIB_RPC_RegisterNotifyCallback(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnReceiving	, "ib", NULL, oapi_dnld_CbDataReceiving);
#if defined(CONFIG_TIMESTAMP_64)
	HLIB_RPC_RegisterNotifyCallback(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnProbeComplete	, "iilliib", NULL, oapi_dnld_CbProbeSuccess);
#else
	HLIB_RPC_RegisterNotifyCallback(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnProbeComplete	, "iiiiiib", NULL, oapi_dnld_CbProbeSuccess);
#endif
	HLIB_RPC_RegisterNotifyCallback(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnProbeFail	, "ii", NULL, oapi_dnld_CbProbeFailed);
	HLIB_RPC_RegisterNotifyCallback(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnDownloadInfoChanged	, "iiib", NULL, oapi_dnld_CbOnDownloadInfoChanged);	
	HLIB_RPC_RegisterNotifyCallback(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnStopped	, "ii", NULL, oapi_dnld_CbStopped);
	HLIB_RPC_RegisterNotifyCallback (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentVideoChanged 	, "iib", NULL, oapi_dnld_CbOnComponentVideoChanged);
	HLIB_RPC_RegisterNotifyCallback (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentAudioChanged 	, "iib", NULL, oapi_dnld_CbOnComponentAudioChanged);
	HLIB_RPC_RegisterNotifyCallback (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentSubtitleChanged, "iib", NULL, oapi_dnld_CbOnComponentSubtitleChanged);
	HLIB_RPC_RegisterNotifyCallback (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentDataChanged	, "iib", NULL, oapi_dnld_CbOnComponentDataChanged);
	HLIB_RPC_RegisterNotifyCallback (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentSelected 		, "iiiii", NULL, oapi_dnld_CbOnComponentSelected);
	HLIB_RPC_RegisterNotifyCallback (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnAvailabilityStartTimeChanged, "iis", NULL, oapi_dnld_CbOnAvailabilityStartTimeChanged);
	/* init callback */
	if (NULL != pstContext->pfCbActivated)
	{
		(void)pstContext->pfCbActivated();
	}

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___
HERROR OAPI_DOWNLOAD_Init (OxDownload_ActivatedCb_t pfActivated)
{
	oapiDnldContext_t				*pstContext = oapi_dnld_GetContext();
	HERROR							 hErr;

	if (TRUE != s_bOapiDnld_Inited)
	{
		HxSTD_MemSet(pstContext, 0, sizeof(oapiDnldContext_t));

		hErr = OAPI_RpcProtocol_GetRpcHandle(&(pstContext->nRpcHandle));
		if (ERR_OK != hErr )
		{
			HxLOG_Error("Error. \n");
			return ERR_FAIL;
		}

		pstContext->pfCbActivated = pfActivated;

		HxSEMT_Create(&(pstContext->ulSemId), "OxDownloadSvcMgr_t", 0);

		HLIB_RPC_Request (pstContext->nRpcHandle, (HxRPC_Response_t)oapi_dnld_CbOnActivated, NULL, RPC_OAPI_DOWNLOAD_Activate, NULL);

		s_bOapiDnld_Inited = TRUE;
	}

	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_SetListener (OxDownload_Notifier_t pfListener, void *pvUserData)
{
	oapiDnldContext_t				*pstContext = oapi_dnld_GetContext();
	oapiDnldNotifier_t				*pstNotifier;

	if ((NULL == pstContext) || (NULL == pfListener))		{ return ERR_FAIL; }

	pstNotifier = oapi_dnld_FindNotifier(pfListener, pvUserData);
	if (NULL != pstNotifier)
	{
		HxLOG_Error ("Already registered...\n");
		return ERR_OK;
	}

	pstNotifier = (oapiDnldNotifier_t *)HLIB_STD_MemAlloc (sizeof(oapiDnldNotifier_t));
	if (NULL == pstNotifier)
	{
		HxLOG_Error ("Memory allocation failed\n");
		return ERR_FAIL;
	}

	pstNotifier->pfListener	= pfListener;
	pstNotifier->pvUserData	= pvUserData;

	pstContext->pstNotiList = HLIB_LIST_Append (pstContext->pstNotiList, (void *)pstNotifier);
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_ResetListener (OxDownload_Notifier_t pfListener, void *pvUserData)
{
	oapiDnldContext_t				*pstContext = oapi_dnld_GetContext();
	oapiDnldNotifier_t				*pstNotifier;

	if ((NULL == pstContext) || (NULL == pfListener))		{ return ERR_FAIL; }

	pstNotifier = oapi_dnld_FindNotifier(pfListener, pvUserData);
	if (NULL == pstNotifier)
	{
		HxLOG_Error ("No notifier...\n");
		return ERR_OK;
	}

	pstContext->pstNotiList = HLIB_LIST_Remove (pstContext->pstNotiList, (const void *)pstNotifier);
	HLIB_STD_MemFree (pstNotifier);

	return ERR_OK;
}

Handle_t OAPI_DOWNLOAD_Probe (DxDownload_Start_t *punStart)
{
	HUINT32							 ulDownloadId = 0;
	oapiDnldContext_t				*pstContext = oapi_dnld_GetContext();
	oapiDnldInstance_t				*pstInst = NULL;

	if (NULL == punStart) { return 0; }

	OAPI_DOWNLOAD_SEM_GET;
	ulDownloadId = oapi_dnld_GetNextDownloadId();
	OAPI_DOWNLOAD_SEM_RELEASE;
	
	if (0 == ulDownloadId) { return 0; }

	pstInst = (oapiDnldInstance_t *)HLIB_STD_MemCalloc(sizeof(oapiDnldInstance_t));
	if (NULL == pstInst)
	{
		HxLOG_Error("memory allocation failed:\n");
		return 0;
	}

	pstInst->ulDownloadId = ulDownloadId;
	pstInst->eType = punStart->eType;
	pstInst->eState = eOxDNLD_STATE_Requested;
	HxSTD_MemCopy(&(pstInst->unStartInfo), punStart, sizeof(DxDownload_Start_t));

	HLIB_RPC_Call(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Probe, "ib", ulDownloadId, punStart, sizeof(DxDownload_Start_t));
	OAPI_DOWNLOAD_SEM_GET;
	(void)oapi_dnld_AppendInstance(pstInst);
	OAPI_DOWNLOAD_SEM_RELEASE;
	return (Handle_t)ulDownloadId;
}


Handle_t OAPI_DOWNLOAD_Start (Handle_t hDownload, DxDownload_Start_t *punStart)
{
	HUINT32							 ulDownloadId = (HUINT32)hDownload;
	oapiDnldContext_t				*pstContext = oapi_dnld_GetContext();
	oapiDnldInstance_t				*pstInst = NULL;

	if ((NULL == punStart))				{ return 0; }

	if(hDownload == 0)
	{	
		OAPI_DOWNLOAD_SEM_GET;
		ulDownloadId = oapi_dnld_GetNextDownloadId();
		OAPI_DOWNLOAD_SEM_RELEASE;
	}
	pstInst = (oapiDnldInstance_t *)HLIB_STD_MemCalloc(sizeof(oapiDnldInstance_t));
	if (NULL == pstInst)
	{
		HxLOG_Error("memory allocation failed:\n");
		return 0;
	}

	pstInst->ulDownloadId = ulDownloadId;
	pstInst->eType = punStart->eType;
	pstInst->eState = eOxDNLD_STATE_Requested;
	HxSTD_MemCopy(&(pstInst->unStartInfo), punStart, sizeof(DxDownload_Start_t));
	HLIB_RPC_Call(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Start, "ib", ulDownloadId, punStart, sizeof(DxDownload_Start_t));
	OAPI_DOWNLOAD_SEM_GET;
	(void)oapi_dnld_AppendInstance(pstInst);
	OAPI_DOWNLOAD_SEM_RELEASE;
	return (Handle_t)ulDownloadId;
}

HERROR OAPI_DOWNLOAD_Stop (Handle_t hDownload)
{
	oapiDnldContext_t				*pstContext = oapi_dnld_GetContext();
	oapiDnldInstance_t				*pstInst = NULL;

	OAPI_DOWNLOAD_SEM_GET;
	pstInst = oapi_dnld_GetDownloadInstance((HUINT32)hDownload, FALSE);

	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with hDownload(0x%08x)\n", hDownload);
		OAPI_DOWNLOAD_SEM_RELEASE;
		return ERR_FAIL;
	}
	(void)oapi_dnld_RemoveInstance(pstInst);
	OAPI_DOWNLOAD_SEM_RELEASE;

	HLIB_RPC_Call(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Stop, "i", hDownload);
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_Pause (Handle_t hDownload)
{
	oapiDnldContext_t				*pstContext = oapi_dnld_GetContext();
	oapiDnldInstance_t				*pstInst = NULL;
	OAPI_DOWNLOAD_SEM_GET;
	pstInst = oapi_dnld_GetDownloadInstance((HUINT32)hDownload, FALSE);
	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with hDownload(0x%08x)\n", hDownload);
		OAPI_DOWNLOAD_SEM_RELEASE;
		return ERR_FAIL;
	}
	OAPI_DOWNLOAD_SEM_RELEASE;
	HLIB_RPC_Call(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Pause, "i", hDownload);
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_Resume (Handle_t hDownload)
{
	oapiDnldContext_t				*pstContext = oapi_dnld_GetContext();
	oapiDnldInstance_t				*pstInst = NULL;
	OAPI_DOWNLOAD_SEM_GET;
	pstInst = oapi_dnld_GetDownloadInstance((HUINT32)hDownload, FALSE);
	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with hDownload(0x%08x)\n", hDownload);
		OAPI_DOWNLOAD_SEM_RELEASE;
		return ERR_FAIL;
	}
	OAPI_DOWNLOAD_SEM_RELEASE;
	HLIB_RPC_Call(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Resume, "i", hDownload);
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_GetStatus (Handle_t hDownload, DxDownload_Status_t *pstStatus)
{
	oapiDnldInstance_t				*pstInst = NULL;
	if (NULL == pstStatus)					
	{ 
		return ERR_FAIL; 
	}
	
	OAPI_DOWNLOAD_SEM_GET;
	pstInst = oapi_dnld_GetDownloadInstance((HUINT32)hDownload, FALSE);

	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with hDownload(0x%08x)\n", hDownload);
		OAPI_DOWNLOAD_SEM_RELEASE;
		return ERR_FAIL;
	}
	HxSTD_MemCopy(pstStatus, &(pstInst->stStatusCache), sizeof(DxDownload_Status_t));
	OAPI_DOWNLOAD_SEM_RELEASE;
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_GetBufferingChunkInfo (Handle_t hDownload, HUINT32 *pnChunks, HUINT32 **ppulStartOffsets, HUINT32 **ppulEndOffsets)
{
	oapiDnldInstance_t				*pstInst = NULL;

	if (NULL == pnChunks) 						{ return ERR_FAIL; }
	if (NULL == ppulStartOffsets) 				{ return ERR_FAIL; }
	if (NULL == ppulEndOffsets) 				{ return ERR_FAIL; }
	
	OAPI_DOWNLOAD_SEM_GET;
	pstInst = oapi_dnld_GetDownloadInstance((HUINT32)hDownload, FALSE);
	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with hDownload(0x%08x)\n", hDownload);
		OAPI_DOWNLOAD_SEM_RELEASE;
		return ERR_FAIL;
	}
	*pnChunks 			= pstInst->stBufferInfo.ulChunks;
	*ppulStartOffsets 	= pstInst->stBufferInfo.ullStartOffsets;
	*ppulEndOffsets   	= pstInst->stBufferInfo.ullEndOffsets;
	OAPI_DOWNLOAD_SEM_RELEASE;
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_GetSeekableRangeInfo (Handle_t hDownload, HUINT32 *pnChunks, HUINT64 **ppullSeekableStart, HUINT64 **ppullSeekableEnd)
{
	oapiDnldInstance_t				*pstInst = NULL;

	if (NULL == pnChunks) 						{ return ERR_FAIL; }
	if (NULL == ppullSeekableStart) 				{ return ERR_FAIL; }
	if (NULL == ppullSeekableEnd) 				{ return ERR_FAIL; }

	OAPI_DOWNLOAD_SEM_GET;
	pstInst = oapi_dnld_GetDownloadInstance((HUINT32)hDownload, FALSE);
	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with hDownload(0x%08x)\n", hDownload);
		OAPI_DOWNLOAD_SEM_RELEASE;
		return ERR_FAIL;
	}
	*pnChunks 				= pstInst->stBufferInfo.stSeekableInfo.ulSeekableChunks;
	*ppullSeekableStart 	= pstInst->stBufferInfo.stSeekableInfo.ullSeekableStart;
	*ppullSeekableEnd   	= pstInst->stBufferInfo.stSeekableInfo.ullSeekableEnd;
	OAPI_DOWNLOAD_SEM_RELEASE;
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_GetRemainBufferingTime (Handle_t hDownload, HUINT32 *punRemainBufferingTime)
{
	oapiDnldInstance_t				*pstInst = NULL;
	if(punRemainBufferingTime == NULL)
	{
		return ERR_FAIL;
	}
	OAPI_DOWNLOAD_SEM_GET;
	pstInst = oapi_dnld_GetDownloadInstance((HUINT32)hDownload, FALSE);
	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with hDownload(0x%08x)\n", hDownload);
		OAPI_DOWNLOAD_SEM_RELEASE;
		return ERR_FAIL;
	}

	*punRemainBufferingTime = pstInst->stBufferInfo.unRemainBufferingTime;
	OAPI_DOWNLOAD_SEM_RELEASE;
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_GetBufferingPercent (Handle_t hDownload, HUINT32 *pulBufferPercent)
{
	oapiDnldInstance_t				*pstInst = NULL;
	if(pulBufferPercent == NULL)
	{
		return ERR_FAIL;
	}
	OAPI_DOWNLOAD_SEM_GET;
	pstInst = oapi_dnld_GetDownloadInstance((HUINT32)hDownload, FALSE);
	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with hDownload(0x%08x)\n", hDownload);
		OAPI_DOWNLOAD_SEM_RELEASE;
		return ERR_FAIL;
	}
	*pulBufferPercent = pstInst->stBufferInfo.ulBufferPercent;
	OAPI_DOWNLOAD_SEM_RELEASE;
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_GetBufferedBytes (Handle_t hDownload, HINT64 *pllBufferedBytes)
{
	oapiDnldInstance_t				*pstInst = NULL;
	if(pllBufferedBytes == NULL)
	{
		return ERR_FAIL;
	}
	OAPI_DOWNLOAD_SEM_GET;
	pstInst = oapi_dnld_GetDownloadInstance((HUINT32)hDownload, FALSE);
	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with hDownload(0x%08x)\n", hDownload);
		OAPI_DOWNLOAD_SEM_RELEASE;
		return ERR_FAIL;
	}
	*pllBufferedBytes = pstInst->stBufferInfo.llBufferedBytes;
	OAPI_DOWNLOAD_SEM_RELEASE;
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_GetRemainingBytes (Handle_t hDownload, HINT64 *pllRemainingBytes)
{
	oapiDnldInstance_t				*pstInst = NULL;
	if(pllRemainingBytes == NULL)
	{
		return ERR_FAIL;
	}
	OAPI_DOWNLOAD_SEM_GET;
	pstInst = oapi_dnld_GetDownloadInstance((HUINT32)hDownload, FALSE);
	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with hDownload(0x%08x)\n", hDownload);
		OAPI_DOWNLOAD_SEM_RELEASE;
		return ERR_FAIL;
	}	
	*pllRemainingBytes = pstInst->stBufferInfo.llTotalBytesRemaining;
	OAPI_DOWNLOAD_SEM_RELEASE;
	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_GetComponentNum (HUINT32 hDownload, OxDownload_Component_e eCompType, HUINT32* pulCompNum)
{
	HUINT32 ulSessionId;
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();
	if (NULL == pstContext)		{ return ERR_FAIL; }

	for(ulSessionId = 0; ulSessionId < OAPI_DOWNLOAD_CACHE_NUM; ulSessionId++)
	{
		if(pstContext->stCache[ulSessionId].ulDownloadId == hDownload)
		{
			HxLOG_Error("OAPI_DOWNLOAD_GetComponentNum - found session id for download id\n");
			break;
		}
	}

	if (ulSessionId >= OAPI_DOWNLOAD_CACHE_NUM)		{ return ERR_FAIL; }

	switch( eCompType )
	{
		case eOxDNLD_COMPONENT_VIDEO:
			*pulCompNum = pstContext->stCache[ulSessionId].stVideoCompTable.nArraySize;
			break;
		case eOxDNLD_COMPONENT_AUDIO:
			*pulCompNum = pstContext->stCache[ulSessionId].stAudioCompTable.nArraySize;
			break;
		case eOxDNLD_COMPONENT_SUBTITLE:
			*pulCompNum = pstContext->stCache[ulSessionId].stSubtitleCompTable.nArraySize;
			break;
		case eOxDNLD_COMPONENT_DATA:
			*pulCompNum = pstContext->stCache[ulSessionId].stDataCompTable.nArraySize;
			break;
		default:
			{
				HxLOG_Error("no support track type (%d) \n", eCompType );
				return ERR_FAIL;
			}

	}
	return ERR_OK;
}
HERROR OAPI_DOWNLOAD_GetComponent (Handle_t hDownload, OxDownload_Component_e eCompType, HUINT32 ulCompIndex, OxDownload_Component_t *pstComponet)
{
	HUINT32 ulSessionId;
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();
	if (NULL == pstContext)		{ return ERR_FAIL; }

	for(ulSessionId = 0; ulSessionId < OAPI_DOWNLOAD_CACHE_NUM; ulSessionId++)
	{
		if(pstContext->stCache[ulSessionId].ulDownloadId == hDownload)
		{
			HxLOG_Error("OAPI_DOWNLOAD_GetComponent - found session id for download id\n");
			break;
		}
	}

	if (ulSessionId >= OAPI_DOWNLOAD_CACHE_NUM)		{ return ERR_FAIL; }

	switch( eCompType )
	{
		case eOxDNLD_COMPONENT_VIDEO:
			if( pstContext->stCache[ulSessionId].stVideoCompTable.nArraySize < ulCompIndex )
			{
				HxLOG_Error("out of video track - index(%d) \n", ulCompIndex );
				return ERR_FAIL;
			}

			HxSTD_MemCopy(pstComponet, &(pstContext->stCache[ulSessionId].stVideoCompTable.astCompArray[ulCompIndex]), sizeof(OxDownload_VideoComp_t));
			break;
		case eOxDNLD_COMPONENT_AUDIO:
			if( pstContext->stCache[ulSessionId].stAudioCompTable.nArraySize < ulCompIndex )
			{
				HxLOG_Error("out of audio track - index(%d) \n", ulCompIndex );
				return ERR_FAIL;
			}

			HxSTD_MemCopy(pstComponet, &(pstContext->stCache[ulSessionId].stAudioCompTable.astCompArray[ulCompIndex]), sizeof(OxDownload_AudioComp_t));
			break;

		case eOxDNLD_COMPONENT_SUBTITLE:
			if( pstContext->stCache[ulSessionId].stSubtitleCompTable.nArraySize < ulCompIndex )
			{
				HxLOG_Error("out of subtitle track - index(%d) \n", ulCompIndex );
				return ERR_FAIL;
			}

			HxSTD_MemCopy(pstComponet, &(pstContext->stCache[ulSessionId].stSubtitleCompTable.astCompArray[ulCompIndex]), sizeof(OxDownload_SubtitleComp_t));
			break;

		case eOxDNLD_COMPONENT_DATA:
			if( pstContext->stCache[ulSessionId].stDataCompTable.nArraySize < ulCompIndex )
			{
				HxLOG_Error("out of data track - index(%d) \n", ulCompIndex );
				return ERR_FAIL;
			}

			HxSTD_MemCopy(pstComponet, &(pstContext->stCache[ulSessionId].stDataCompTable.astCompArray[ulCompIndex]), sizeof(OxDownload_DataComp_t));
			break;
		default:
			{
				HxLOG_Error("no support track type (%d) \n", eCompType );
				return ERR_FAIL;
			}

	}

	return ERR_OK;
}

HERROR OAPI_DOWNLOAD_GetComponentIndex (Handle_t hDownload, OxDownload_Component_e eCompType, HINT32* pnCompIndex, HINT32* pnCompSubIndex)
{
	HUINT32 ulSessionId;
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();

	if (NULL == pstContext)						{ return ERR_FAIL; }

	for(ulSessionId = 0; ulSessionId < OAPI_DOWNLOAD_CACHE_NUM; ulSessionId++)
	{
		if(pstContext->stCache[ulSessionId].ulDownloadId == hDownload)
		{
			HxLOG_Error("OAPI_DOWNLOAD_GetComponent - found session id for download id\n");
			break;
		}
	}

	if (ulSessionId >= OAPI_DOWNLOAD_CACHE_NUM)		{ return ERR_FAIL; }

	switch( eCompType )
	{
		case eOxMP_COMPONENT_VIDEO:
			*pnCompIndex = pstContext->stCache[ulSessionId].stVideoCompTable.nCurrIndex;
			break;
		case eOxMP_COMPONENT_AUDIO:
			*pnCompIndex = pstContext->stCache[ulSessionId].stAudioCompTable.nCurrIndex;
			if (pnCompSubIndex != NULL)
			{
				*pnCompSubIndex = pstContext->stCache[ulSessionId].stAudioCompTable.nCurrSubIndex;
			}
			break;

		case eOxMP_COMPONENT_SUBTITLE:
			*pnCompIndex = pstContext->stCache[ulSessionId].stSubtitleCompTable.nCurrIndex;
			break;

		case eOxMP_COMPONENT_DATA:
			*pnCompIndex = pstContext->stCache[ulSessionId].stDataCompTable.nCurrIndex;
			break;

		default:
			{
				HxLOG_Error("no support track type (%d) \n", eCompType );
				return ERR_FAIL;
			}

	}

	HxLOG_Debug("hDownload[%d] eCompType[%d] *pnCompIndex[%d]\n", hDownload, eCompType, *pnCompIndex);
	return ERR_OK;
}


HERROR OAPI_DOWNLOAD_GetAvailabilityStartTime (Handle_t hDownload, HUINT32 szTimeStringBuffer, HUINT8 *pucTimeString)
{
	HUINT32 ulSessionId, szString;
	oapiDnldContext_t	*pstContext = oapi_dnld_GetContext();
	
	if (NULL == pstContext)		{ return ERR_FAIL; }
	if (NULL == pucTimeString)	{ return ERR_FAIL; }
	
	for(ulSessionId = 0; ulSessionId < OAPI_DOWNLOAD_CACHE_NUM; ulSessionId++)
	{
		if(pstContext->stCache[ulSessionId].ulDownloadId == hDownload)
		{
			HxLOG_Error("OAPI_DOWNLOAD_GetComponentNum - found session id for download id\n");
			break;
		}
	}
	if (ulSessionId >= OAPI_DOWNLOAD_CACHE_NUM)		{ return ERR_FAIL; }
	
	szString = HxSTD_StrLen(pstContext->stCache[ulSessionId].szAvailabilityStartTime);
	if(szString)
	{
		if(szString > szTimeStringBuffer)
		{
			szString = szTimeStringBuffer - 1;
		}
		HxSTD_MemCopy( pucTimeString, pstContext->stCache[ulSessionId].szAvailabilityStartTime, szString);
		pucTimeString[szString] = '\0';
	}
	HxLOG_Info("*pucTimeString=(%s) \n", pucTimeString);

	return ERR_OK;
}

