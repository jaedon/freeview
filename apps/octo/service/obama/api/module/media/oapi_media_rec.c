/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_mediarec.c
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
#include <oapi.h>
#include <obama_int.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define OAPI_MR_SEM_GET				HxSEMT_Get		(s_ulOxMrSema)
#define OAPI_MR_SEM_RELEASE			HxSEMT_Release	(s_ulOxMrSema)

#define OAPI_MR_TSR_HANDLE_HEAD		0x73520000

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	//BASE
	OxMediaRec_StartInfo_t  stStart;

	HUINT32 				ulRecTime;
	OxMediaRec_RecState_e	eState;

	HUINT32					ulLastTimeTick;

	HUINT32 				nContentId;
} oapiMrCache_t;


typedef struct
{
	HUINT32					 ulRsvSlot;

	/*CACHE*/
	oapiMrCache_t			 stCache;



	HUINT32					ulRecId;
	HUINT32					ulSessionVer;

} oapiMrContext_t;

typedef struct
{
	HUINT32					 ulRecId;
	OxMediaRec_Notifier_t	 pfListener;
	void					*pvUserData;
} oapiMrNotifier_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL				 s_bOapiMr_Inited = FALSE, s_bOapiMrActivated = FALSE;
STATIC HUINT32				 s_ulOapiMr_RecIdMax = 0, s_ulOapiMr_TsrNum = 0, s_ulOapiMr_MaxRec = 0;
STATIC oapiMrContext_t		*s_pstOapiMr_RecContext = NULL;
STATIC oapiMrContext_t		*s_pstOapiMr_TsrContext = NULL;
STATIC HxList_t				*s_pstOapiMr_NotifierList = NULL;			// oapiMrNotifier_t
STATIC OxMediaRec_ActivatedCb_t s_pfActivated = NULL;
STATIC HUINT32				 s_ulOxMrSema = 0;
STATIC HINT32				 s_nRpcHandle = 0;



/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
STATIC HCHAR *oapi_mr_GetEventString (OxMediaRec_Event_e eEvent)
{
	switch (eEvent)
	{
	ENUM_TO_STR (eOxMR_EVENT_REC_STARTED);
	ENUM_TO_STR (eOxMR_EVENT_REC_STOPPED);
	ENUM_TO_STR (eOxMR_EVENT_REC_TIME_CHANGED);
	ENUM_TO_STR (eOxMR_EVENT_REC_STATE_CHANGED);
	ENUM_TO_STR (eOxMR_EVENT_REC_CONTENTS_CHANGED);
	ENUM_TO_STR (eOxMR_EVENT_REC_TSRBUFFER_COPYEND);
	default:
		break;
	}

	return "Unknown";
}

STATIC HCHAR *oapi_mr_GetStateString (OxMediaRec_RecState_e eState)
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

STATIC oapiMrContext_t *oapi_mr_GetRecContext (HUINT32 ulRecId)
{
	if ((ulRecId < s_ulOapiMr_RecIdMax) && (NULL != s_pstOapiMr_RecContext))
	{
		return &(s_pstOapiMr_RecContext[ulRecId]);
	}

	return NULL;
}

STATIC oapiMrContext_t *oapi_mr_GetRecContextByRsvSlot (HUINT32 ulSlot)
{
	HUINT32			 ulRecId;

	if (NULL != s_pstOapiMr_RecContext)
	{
		for (ulRecId = 0; ulRecId < s_ulOapiMr_RecIdMax; ulRecId++)
		{
			oapiMrContext_t		*pstContext = &(s_pstOapiMr_RecContext[ulRecId]);

			if ((eOxMR_RECSTATE_NONE != pstContext->stCache.eState) &&
				(eOxMR_RECSTATE_STOPPED != pstContext->stCache.eState) &&
				(ulSlot == pstContext->ulRsvSlot))
			{
				return pstContext;
			}
		}
	}

	return NULL;
}

STATIC oapiMrContext_t *oapi_mr_GetEmptyRecContext (void)
{
	HUINT32			 ulRecId;

	if (NULL != s_pstOapiMr_RecContext)
	{
		for (ulRecId = 0; ulRecId < s_ulOapiMr_RecIdMax; ulRecId++)
		{
			oapiMrContext_t		*pstContext = &(s_pstOapiMr_RecContext[ulRecId]);

			if ((eOxMR_RECSTATE_NONE == pstContext->stCache.eState) ||
				(eOxMR_RECSTATE_STOPPED == pstContext->stCache.eState))
			{
				return pstContext;
			}
		}
	}

	return NULL;
}

STATIC oapiMrContext_t *oapi_mr_GetTsrContext (HUINT32 ulTsrId)
{
	if ((ulTsrId < s_ulOapiMr_TsrNum) && (NULL != s_pstOapiMr_TsrContext))
	{
		return &(s_pstOapiMr_TsrContext[ulTsrId]);
	}

	return NULL;
}

STATIC oapiMrContext_t *oapi_mr_GetEmptyTsrContext (HUINT32 *pulIndex)
{
	HUINT32			 ulTsrId;

	if (NULL != s_pstOapiMr_TsrContext)
	{
		for (ulTsrId = 0; ulTsrId < s_ulOapiMr_TsrNum; ulTsrId++)
		{
			oapiMrContext_t		*pstContext = &(s_pstOapiMr_TsrContext[ulTsrId]);

			if ((eOxMR_RECSTATE_NONE == pstContext->stCache.eState) ||
				(eOxMR_RECSTATE_STOPPED == pstContext->stCache.eState))
			{
				if (NULL != pulIndex)					{ *pulIndex = ulTsrId; }
				return pstContext;
			}
		}
	}

	return NULL;
}

STATIC HUINT32 oapi_mr_GetSessionId (oapiMrContext_t *pstContext)
{
	return (((pstContext->ulRecId & 0xFFFF) << 16) |
			(pstContext->ulSessionVer & 0xFFFF));
}

STATIC void oapi_mr_GetRecIdSessionVer (HUINT32 ulSessionId, HUINT32 *pulRecId, HUINT32 *pulSessionVer)
{
	if (NULL != pulRecId)							{ *pulRecId = (ulSessionId >> 16) & 0xFFFF; }
	if (NULL != pulSessionVer)						{ *pulSessionVer = ulSessionId & 0xFFFF; }
}

STATIC void oapi_mr_IncreaseSessionVer (oapiMrContext_t *pstContext)
{
	pstContext->ulSessionVer = (pstContext->ulSessionVer >= 0xFFEF) ? 0x0001 : (pstContext->ulSessionVer + 1);
}

STATIC void oapi_mr_NotifyToListener (oapiMrContext_t *pstContext, HUINT32 ulSlot, OxMediaRec_Event_e eEvent, OxMediaRec_NotifierData_t *pstData)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstOapiMr_NotifierList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiMrNotifier_t			*pstNotifier = (oapiMrNotifier_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstNotifier) && (NULL != pstNotifier->pfListener))
		{
			HxLOG_Debug("EVENT : (%s)\n", oapi_mr_GetEventString(eEvent));
			(pstNotifier->pfListener) (pstContext->stCache.stStart.ulMasterSvcUid, ulSlot, eEvent, pstData, pstNotifier->pvUserData);
		}
	}
}

STATIC HUINT32 oapi_mr_CountRecording (void)
{
	HUINT32			 ulRecId, ulRecNum = 0;


	for (ulRecId = 0; ulRecId < s_ulOapiMr_RecIdMax; ulRecId++)
	{
		oapiMrContext_t	*pstContext = oapi_mr_GetRecContext (ulRecId);

		if (NULL != pstContext)
		{
			if ((eOxMR_RECSTATE_REQUESTED == pstContext->stCache.eState) ||
				(eOxMR_RECSTATE_RECORDING == pstContext->stCache.eState))
			{
				ulRecNum ++;
			}
		}
	}

	return ulRecNum;
}

STATIC void oapi_mr_CacheUpdate (oapiMrCache_t *pstCache, OxMediaRec_Event_e eEvent, HINT32 nP1, HINT32 nP2, HINT32 nP3)
{
	if( NULL == pstCache )
		return ;

	OAPI_MR_SEM_GET;

	switch( eEvent )
	{
		case eOxMR_EVENT_REC_STARTED:
		{
			HxSTD_MemCopy(&(pstCache->stStart), (OxMediaRec_StartInfo_t*)nP1, sizeof(OxMediaRec_StartInfo_t) );

			pstCache->eState = eOxMR_RECSTATE_RECORDING;
			break;
		}
		case eOxMR_EVENT_REC_STOPPED:
		{
			HxSTD_MemSet(&(pstCache->stStart), 0, sizeof(OxMediaRec_StartInfo_t) );

			pstCache->eState = eOxMR_RECSTATE_NONE;
			pstCache->nContentId = -1;
			break;
		}
		case eOxMR_EVENT_REC_TIME_CHANGED:
		{
			pstCache->ulLastTimeTick	=	HLIB_STD_GetSystemTick();
			pstCache->ulRecTime = nP1;
			break;
		}
		case eOxMR_EVENT_REC_CONTENTS_CHANGED:
		{
			pstCache->nContentId = nP1;
			break;
		}
		case eOxMR_EVENT_REC_STATE_CHANGED:
		{
			pstCache->eState = (OxMediaRec_RecState_e)nP1;
			break;
		}

		default:
			break;
	}

	OAPI_MR_SEM_RELEASE;

	return;
}

STATIC oapiMrNotifier_t *oapi_mr_FindNotifier (OxMediaRec_Notifier_t pfListener, void *pvUserData)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstOapiMr_NotifierList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiMrNotifier_t			*pstNotifier = (oapiMrNotifier_t *)HLIB_LIST_Data (pstListItem);

		if ((pfListener == pstNotifier->pfListener) &&
			(pvUserData == pstNotifier->pvUserData))
		{
			return pstNotifier;
		}
	}

	return NULL;
}

#define _____NOTIFICATION_CALLBACKS_____
STATIC HERROR oapi_mr_CbRecStarted (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
//	HUINT32						 ulRecId, ulSessionId;
	HUINT32						 ulSlot, ulContentsId;
	OxMediaRec_StartInfo_t		*pstStart;
	oapiMrContext_t				*pstContext;

	if (nArgc == 3)
	{
		ulSlot			= (HUINT32)HxOBJECT_INT(apArgv[0]);
		ulContentsId	= (HUINT32)HxOBJECT_INT(apArgv[1]);
		pstStart		= (OxMediaRec_StartInfo_t *)HxOBJECT_BIN_DATA(apArgv[2]);

		pstContext = oapi_mr_GetRecContextByRsvSlot(ulSlot);
		if (NULL == pstContext)
		{
			pstContext = oapi_mr_GetEmptyRecContext();
			if (NULL == pstContext)
			{
				HxLOG_Critical("Slot(%d) !!!! No Empty Record Context !!!! Something Wrong !!!! Please Check!!!!\n", ulSlot);
				return ERR_FAIL;
			}

			pstContext->ulRsvSlot = ulSlot;
			oapi_mr_CacheUpdate(&(pstContext->stCache), eOxMR_EVENT_REC_STARTED, (HINT32)pstStart, 0, 0);
		}

		pstContext->stCache.eState		= eOxMR_RECSTATE_RECORDING;
		pstContext->stCache.nContentId	= ulContentsId;

		oapi_mr_NotifyToListener(pstContext, ulSlot, eOxMR_EVENT_REC_STARTED, (OxMediaRec_NotifierData_t *)pstStart);
	}

	return ERR_OK;
}

STATIC HERROR oapi_mr_CbRecStopped (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						 ulSlot;
	oapiMrContext_t				*pstContext;

	if (nArgc == 1)
	{
		ulSlot		= (HUINT32)HxOBJECT_INT(apArgv[0]);

		pstContext = oapi_mr_GetRecContextByRsvSlot(ulSlot);
		if (NULL != pstContext)
		{
			pstContext->ulRsvSlot = 0xFFFF;
			oapi_mr_CacheUpdate(&(pstContext->stCache), eOxMR_EVENT_REC_STOPPED, 0, 0, 0);
			oapi_mr_NotifyToListener(pstContext, ulSlot, eOxMR_EVENT_REC_STOPPED, NULL);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mr_CbRecTimecodeChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						 ulSlot;
	HUINT32						 ulRecTime;
	oapiMrContext_t				*pstContext;

	HxLOG_Debug("TimeCode Changed...\n");
	if (nArgc == 2)
	{
		ulSlot		= (HUINT32)HxOBJECT_INT(apArgv[0]);
		ulRecTime	= (HUINT32)HxOBJECT_INT(apArgv[1]);

		pstContext = oapi_mr_GetRecContextByRsvSlot(ulSlot);
		if (NULL != pstContext)
		{
			oapi_mr_CacheUpdate(&(pstContext->stCache), eOxMR_EVENT_REC_TIME_CHANGED, ulRecTime, 0, 0);
			oapi_mr_NotifyToListener(pstContext, ulSlot, eOxMR_EVENT_REC_TIME_CHANGED, NULL);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mr_CbRecTsrBufferCopyEnd(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						 ulSlot;
	oapiMrContext_t				*pstContext;

	if (nArgc == 1)
	{
		ulSlot		= (HUINT32)HxOBJECT_INT(apArgv[0]);
		pstContext = oapi_mr_GetRecContextByRsvSlot(ulSlot);
		if (NULL != pstContext)
		{
			oapi_mr_CacheUpdate(&(pstContext->stCache), eOxMR_EVENT_REC_TSRBUFFER_COPYEND, 0, 0, 0);
			oapi_mr_NotifyToListener(pstContext, ulSlot, eOxMR_EVENT_REC_TSRBUFFER_COPYEND, NULL);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mr_CbRecMaxInstanceChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	if (nArgc == 2)
	{
		s_ulOapiMr_MaxRec = (HUINT32)HxOBJECT_INT(apArgv[0]);
		s_ulOapiMr_RecIdMax = (HUINT32)HxOBJECT_INT(apArgv[1]);
		s_ulOapiMr_TsrNum = (HUINT32)HxOBJECT_INT(apArgv[1]);
	}

	return ERR_OK;
}

STATIC HERROR oapi_mr_CbOnActivated (HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32				 nCnt;
	HINT32				 nMaxRecId = HxOBJECT_INT (apArgv[1]);
	HINT32				 nMaxTsrNum = HxOBJECT_INT (apArgv[1]);
	oapiMrContext_t 	*pstContext = NULL;
	oapiMrContext_t 	*pstContextArray = NULL;

	HxLOG_Debug("onActivated...\n");

	s_ulOapiMr_MaxRec		= HxOBJECT_INT (apArgv[0]);

	if (nMaxRecId > 0)
	{
		pstContextArray = (oapiMrContext_t *)HLIB_STD_MemCalloc (sizeof(oapiMrContext_t) * nMaxRecId);
		if (NULL != pstContextArray)
		{
			for (nCnt = 0; nCnt < nMaxRecId; nCnt++)
			{
				pstContext = &(pstContextArray[nCnt]);

				pstContext->ulRsvSlot		= 0xFFFF;

				pstContext->ulRecId			= (HUINT32)nCnt;
				pstContext->ulSessionVer	= 0x0001;
			}

			s_ulOapiMr_RecIdMax		= (HUINT32)nMaxRecId;
			s_pstOapiMr_RecContext		= pstContextArray;
			s_bOapiMrActivated		= TRUE;
		}
	}

	if (nMaxTsrNum > 0)
	{
		pstContextArray = (oapiMrContext_t *)HLIB_STD_MemCalloc (sizeof(oapiMrContext_t) * nMaxTsrNum);
		if (NULL != pstContextArray)
		{
			for (nCnt = 0; nCnt < nMaxTsrNum; nCnt++)
			{
				pstContext = &(pstContextArray[nCnt]);

				pstContext->ulRsvSlot		= 0xFFFF;

				pstContext->ulRecId			= (HUINT32)nCnt;
				pstContext->ulSessionVer	= 0x0001;
			}

			s_ulOapiMr_TsrNum		= (HUINT32)nMaxTsrNum;
			s_pstOapiMr_TsrContext		= pstContextArray;
			s_bOapiMrActivated		= TRUE;
		}
	}

	// Activate the notifiers:
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OAPI_MEDIA_REC_OnStarted				, "iib", NULL, oapi_mr_CbRecStarted);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OAPI_MEDIA_REC_OnStopped				, "i", NULL, oapi_mr_CbRecStopped);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OAPI_MEDIA_REC_OnTimecodeChanged		, "ii", NULL, oapi_mr_CbRecTimecodeChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OAPI_MEDIA_REC_OnTsrBufferCopyEnd	, "i", NULL, oapi_mr_CbRecTsrBufferCopyEnd);

	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OAPI_MEDIA_REC_OnMaxInstanceChanged, "ii", NULL, oapi_mr_CbRecMaxInstanceChanged);
	/* init callback */
	s_pfActivated();

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___
HERROR OAPI_MEDIA_REC_Init (OxMediaRec_ActivatedCb_t pfActivated)
{
	HERROR hErr;
	HINT32	nReqId;

	if (TRUE != s_bOapiMr_Inited)
	{
		hErr = OAPI_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
		if (ERR_OK != hErr )
		{
			HxLOG_Error("Error. \n");
			return ERR_FAIL;
		}

		s_pfActivated = pfActivated;

		HxSEMT_Create(&s_ulOxMrSema, "OxMediaRecSvcMgr_t", 0);

		nReqId= HLIB_RPC_Request (s_nRpcHandle, (HxRPC_Response_t)oapi_mr_CbOnActivated, NULL, RPC_OAPI_MEDIA_REC_Init, NULL);
		if (HLIB_RPC_Join(s_nRpcHandle, nReqId, RPC_DEFAULT_TIMEOUT) != ERR_OK)
		{
			HxLOG_Error("==========> ERROR !!! [%s] has HLIB_RPC_Request timeout!!!\n\n", __FUNCTION__);
		}
		//OAPI_RegisterCallback (RPC_OAPI_MEDIA_REC_OnInit, oapi_mr_CbOnActivated, NULL);
		//(void)HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_MEDIA_REC_Init, NULL);

		s_bOapiMr_Inited = TRUE;
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_IsRecording (HUINT32 ulMasterSvcUid, HBOOL *pIsRecording)
{
	if( NULL == pIsRecording )
		return ERR_FAIL;

	*pIsRecording = FALSE;

	{
		HINT32	i;
		oapiMrContext_t *	pstContext = NULL;

		for( i=0 ; i<s_ulOapiMr_RecIdMax ; i++ )
		{
			pstContext = oapi_mr_GetRecContext (i);
			if( NULL == pstContext )
				continue;

			if( (ulMasterSvcUid == pstContext->stCache.stStart.ulMasterSvcUid)
				&&( (eOxMR_RECSTATE_REQUESTED == pstContext->stCache.eState) ||(eOxMR_RECSTATE_RECORDING == pstContext->stCache.eState)))
			{
				*pIsRecording = TRUE;
				break;
			}
		}
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_GetCID (HUINT32 ulMasterSvcUid, HUINT32 *pCID)
{
	if( NULL == pCID )
		return ERR_FAIL;

	*pCID = FALSE;

	{
		HINT32	i;
		oapiMrContext_t *	pstContext = NULL;

		for( i=0 ; i<s_ulOapiMr_RecIdMax ; i++ )
		{
			pstContext = oapi_mr_GetRecContext (i);
			if( NULL == pstContext )
				continue;

			if( (ulMasterSvcUid == pstContext->stCache.stStart.ulMasterSvcUid)
				&&( (eOxMR_RECSTATE_REQUESTED == pstContext->stCache.eState) ||(eOxMR_RECSTATE_RECORDING == pstContext->stCache.eState)))
			{
				if( 0xFFFFFFFF == pstContext->stCache.nContentId )
					return ERR_FAIL;

				*pCID = pstContext->stCache.nContentId;
				break;
			}
		}
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_GetCapability (OxMediaRec_Capability_t *pstCap)
{
	if (NULL == pstCap)					{ return ERR_FAIL; }
	if (TRUE != s_bOapiMrActivated)
	{
		HxLOG_Error ("Not Activated yet:\n");
		return ERR_FAIL;
	}

	pstCap->ulMaxRec	= s_ulOapiMr_MaxRec;
	pstCap->ulMaxRecId	= s_ulOapiMr_RecIdMax;

	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_SetListener (OxMediaRec_Notifier_t pfListener, void *pvUserData)
{
	oapiMrNotifier_t	*pstNotifier;

	if (NULL == pfListener)		{ return ERR_FAIL; }

	pstNotifier = oapi_mr_FindNotifier (pfListener, pvUserData);
	if (NULL != pstNotifier)
	{
		HxLOG_Error ("Already registered...\n");
		return ERR_OK;
	}

	pstNotifier = (oapiMrNotifier_t *)HLIB_STD_MemAlloc (sizeof(oapiMrNotifier_t));
	if (NULL == pstNotifier)
	{
		HxLOG_Error ("Memory allocation failed\n");
		return ERR_FAIL;
	}

	pstNotifier->pfListener	= pfListener;
	pstNotifier->pvUserData	= pvUserData;

	s_pstOapiMr_NotifierList = HLIB_LIST_Append (s_pstOapiMr_NotifierList, (void *)pstNotifier);
	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_ResetListener (OxMediaRec_Notifier_t pfListener, void *pvUserData)
{
	oapiMrNotifier_t	*pstNotifier;

	if (NULL == pfListener)		{ return ERR_FAIL; }

	pstNotifier = oapi_mr_FindNotifier (pfListener, pvUserData);
	if (NULL == pstNotifier)
	{
		HxLOG_Error ("No notifier...\n");
		return ERR_OK;
	}

	s_pstOapiMr_NotifierList = HLIB_LIST_Remove (s_pstOapiMr_NotifierList, (const void *)pstNotifier);
	HLIB_STD_MemFree (pstNotifier);

	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_Start (OxMediaRec_StartInfo_t *pstStart, HUINT32 ulSlotId)
{
	oapiMrContext_t				*pstContext;

	if ((NULL == pstStart) || (ulSlotId >= 0xFFFF))					{ return ERR_FAIL; }

	pstContext = oapi_mr_GetEmptyRecContext();
	if (NULL == pstContext)
	{
		HxLOG_Error("Slot(%d), No Context now...\n", ulSlotId);
		return ERR_FAIL;
	}

	pstContext->stCache.eState = eOxMR_RECSTATE_REQUESTED;
	HxSTD_MemCopy(&(pstContext->stCache.stStart), pstStart, sizeof(OxMediaRec_StartInfo_t));
	pstContext->ulRsvSlot = ulSlotId;

	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_Stop (HUINT32 ulSlotId)
{
	oapiMrContext_t				*pstContext;

	pstContext = oapi_mr_GetRecContextByRsvSlot(ulSlotId);
	if (NULL == pstContext)
	{
		HxLOG_Error("Slot(%d), No Context now...\n", ulSlotId);
		return ERR_FAIL;
	}

	switch (pstContext->stCache.eState)
	{
	case eOxMR_RECSTATE_REQUESTED:
		pstContext->ulRsvSlot = 0xFFFF;
		oapi_mr_CacheUpdate(&(pstContext->stCache), eOxMR_EVENT_REC_STOPPED, 0, 0, 0);
		break;

	default:
		break;
	}

	return ERR_OK;
}

Handle_t OAPI_MEDIA_REC_StartTsr (OxMediaRec_StartInfo_t *pstStart)
{
	HUINT32						 ulTsrId = 0xFFFF;
	oapiMrContext_t				*pstTsrContext;

	if (NULL == pstStart)				{ return HANDLE_NULL; }

	pstTsrContext = oapi_mr_GetEmptyTsrContext(&ulTsrId);
	if ((NULL == pstTsrContext) || (0xFFFF == ulTsrId))
	{
		HxLOG_Error("No empty TSR context...\n");
		return HANDLE_NULL;
	}

	(void)HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_MEDIA_REC_TsrStart, "b", pstStart, sizeof(OxMediaRec_StartInfo_t));
	pstTsrContext->stCache.eState = eOxMR_RECSTATE_REQUESTED;
	HxSTD_MemCopy(&(pstTsrContext->stCache.stStart), pstStart, sizeof(OxMediaRec_StartInfo_t));

	return (OAPI_MR_TSR_HANDLE_HEAD | (ulTsrId & 0xFFFF));
}

HERROR OAPI_MEDIA_REC_StopTsr (Handle_t hTsr)
{
	HUINT32						 ulTsrId = (hTsr & 0xFFFF);
	oapiMrContext_t				*pstTsrContext = oapi_mr_GetTsrContext(ulTsrId);


	if ((NULL == pstTsrContext) ||
		((ulTsrId & 0xFFFF0000) != OAPI_MR_TSR_HANDLE_HEAD))
	{
		HxLOG_Error("Wrong hTsr: 0x%08x\n", hTsr);
		return ERR_FAIL;
	}

	(void)HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_MEDIA_REC_TsrStop, "b", &(pstTsrContext->stCache.stStart), sizeof(OxMediaRec_StartInfo_t));
	pstTsrContext->stCache.eState = eOxMR_RECSTATE_STOPPED;

	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_GetCount (HINT32 *pRecCount)
{
	if( NULL == pRecCount )
		return ERR_FAIL;

	*pRecCount = oapi_mr_CountRecording();

	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_GetSVCUid (HUINT32 ulSlot, HUINT32 *pulSvcUid)
{
	oapiMrContext_t 	*pstContext = NULL;

	if (NULL == pulSvcUid)						{ return ERR_FAIL; }

	pstContext = oapi_mr_GetRecContextByRsvSlot(ulSlot);
	if (NULL == pstContext)						{ return ERR_FAIL; }

	*pulSvcUid = pstContext->stCache.stStart.ulMasterSvcUid;

	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_GetStartTime (HUINT32 ulSlot, HUINT32 *pulStartTime)
{
	oapiMrContext_t 	*pstContext = NULL;

	if (NULL == pulStartTime)					{ return ERR_FAIL; }

	pstContext = oapi_mr_GetRecContextByRsvSlot(ulSlot);
	if (NULL == pstContext)						{ return ERR_FAIL; }

	*pulStartTime = pstContext->stCache.stStart.nStartOffset;

	return ERR_OK;
}


HERROR OAPI_MEDIA_REC_GetRecTime (HUINT32 ulSlot, HUINT32 *pulRecTime)
{
	oapiMrContext_t 	*pstContext = NULL;

	if (NULL == pulRecTime)						{ return ERR_FAIL; }

	pstContext = oapi_mr_GetRecContextByRsvSlot(ulSlot);
	if (NULL == pstContext)						{ return ERR_FAIL; }

	if (pstContext->stCache.ulLastTimeTick != 0xFFFFFFFF)
	{
		HUINT32 		 ulCurrTick = HLIB_STD_GetSystemTick();

		if ((0 != pstContext->stCache.ulLastTimeTick) && (ulCurrTick >= pstContext->stCache.ulLastTimeTick))
		{
			HUINT32 		 ulDiffSec = (ulCurrTick - pstContext->stCache.ulLastTimeTick) / 1000;

			*pulRecTime = pstContext->stCache.ulRecTime + ulDiffSec;
		}
		else
		{
			*pulRecTime = 0;
		}
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_REC_GetContentId (HUINT32 ulSlot, HUINT32 *pnContentId)
{
	oapiMrContext_t 	*pstContext = NULL;

	if (NULL == pnContentId)					{ return ERR_FAIL; }

	pstContext = oapi_mr_GetRecContextByRsvSlot(ulSlot);
	if (NULL == pstContext)						{ return ERR_FAIL; }

	*pnContentId = pstContext->stCache.nContentId;

	return ERR_OK;
}
