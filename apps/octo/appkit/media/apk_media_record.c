/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_mediarec.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

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

#include <apk.h>
#include <_apk_int.h>
#include <sapi.h>

#define USE_SAMA_TO_RECORD

#define ___INTERNAL_STRUCTURES___
typedef struct
{
	HUINT32					 ulSvcUid;
	OxMediaRec_Notifier_t	 pfListener;
	void					*pvUserData;
} _apk_mr_Notifier_t;


#define ___INTERNAL_VARIABLES___

STATIC HxList_t				*s_pstApkMrRec_NotiList = NULL;


#define ___PRIVATE_FUNCTION___

#define _____NOTIFIER_FUNCTION_____
STATIC void	apk_media_record_freeCbNotifier(HINT32 nArgc, void *apArgv[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 6);

	pvData = apArgv[4];
	if (pvData)
		APK_Free(pvData);
}

STATIC void apk_media_record_CbNotifier (HUINT32 ulSvcUid, HUINT32 ulSessionId, OxMediaRec_Event_e eEvent, OxMediaRec_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HxList_t		*pstListItem;

	for (pstListItem = s_pstApkMrRec_NotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		_apk_mr_Notifier_t		*pstNotiItem = (_apk_mr_Notifier_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotiItem) &&
			(((HUINT32)-1 == pstNotiItem->ulSvcUid) || (ulSvcUid == pstNotiItem->ulSvcUid)) &&
			(NULL != pstNotiItem->pfListener))
		{
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			stApkEvtArgs.pfnFreeFunc = apk_media_record_freeCbNotifier;
			stApkEvtArgs.ulArgCount = 6;
			stApkEvtArgs.apArgV[0] = (void *)pstNotiItem->pfListener;
			stApkEvtArgs.apArgV[1] = (void *)ulSvcUid;
			stApkEvtArgs.apArgV[2] = (void *)ulSessionId;
			stApkEvtArgs.apArgV[3] = (void *)eEvent;
			stApkEvtArgs.apArgV[4] = (void *)APK_MemDup(pstNotifyData, sizeof(OxMediaRec_NotifierData_t));
			stApkEvtArgs.apArgV[5] = (void *)pstNotiItem->pvUserData;

			APK_EVENT_Emit(&stApkEvtArgs);
		}
	}
}

STATIC void apk_media_record_CbInitNotified (void)
{
	HxLOG_Info("[%s:%d] MediaRec Activated: Now it can be used:\n", __FUNCTION__, __LINE__ );
	OAPI_MEDIA_REC_SetListener(apk_media_record_CbNotifier, NULL);
}

STATIC _apk_mr_Notifier_t *apk_media_record_FindNotifier (HUINT32 ulSvcUid, OxMediaRec_Notifier_t pfNotifier, void *pvUserData)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstApkMrRec_NotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		_apk_mr_Notifier_t		*pstNoti = (_apk_mr_Notifier_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstNoti) &&
			(pstNoti->ulSvcUid == ulSvcUid) &&
			(pstNoti->pfListener == pfNotifier) &&
			(pstNoti->pvUserData == pvUserData))
		{
			return pstNoti;
		}
	}

	return NULL;
}

STATIC HERROR apk_media_record_GetURL(HUINT32 ulSessionId, HCHAR * pszRecordID)
{
	HUINT32 ulStartTick;
	HUINT32 nContentId;

	ulStartTick = HLIB_STD_GetSystemTick();

	if (NULL == pszRecordID)
		return ERR_FAIL;

	do
	{
		OAPI_MEDIA_REC_GetContentId (ulSessionId, &nContentId);

		if( -1 != nContentId )
		{
			DxRecListData_t* pstRec;
			pstRec = APK_META_RECORD_GetByCID(nContentId );
			if( NULL != pstRec )
			{
				if ('\0' != pstRec->szUrl[0])
				{
					{
						HxSTD_PrintToStrN (pszRecordID, OxMP_MEDIAPLAY_URL, "HPVR:/%s", pstRec->szUrl);
					}
				}

				APK_META_RECORD_Release(pstRec);
				return ERR_OK;
			}
		}

		HLIB_STD_TaskSleep(50);

	} while ((HLIB_STD_GetSystemTick() - ulStartTick) <= 1000);

	return ERR_FAIL;
}


#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR APK_MEDIA_RECORD_Init (void)
{
	OAPI_MEDIA_REC_Init (apk_media_record_CbInitNotified);
	return ERR_OK;
}

HERROR APK_MEDIA_RECORD_RegisterNotifier (HUINT32 ulSvcUid, OxMediaRec_Notifier_t pfNotifier, void *pvUserData)
{
	_apk_mr_Notifier_t			*pstNotiItem;

	if (NULL == pfNotifier)
	{
		HxLOG_Error ("[%s:%d] Notifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pstNotiItem = apk_media_record_FindNotifier (ulSvcUid, pfNotifier, pvUserData);
	if (NULL != pstNotiItem)
	{
		HxLOG_Error("[%s:%d] Already Registered:\n", __FUNCTION__, __LINE__);
		return ERR_OK;
	}

	pstNotiItem = (_apk_mr_Notifier_t *)HLIB_STD_MemCalloc(sizeof(_apk_mr_Notifier_t));
	if (NULL == pstNotiItem)
	{
		HxLOG_Error("[%s:%d] Memory allocation failed:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pstNotiItem->ulSvcUid	= ulSvcUid;
	pstNotiItem->pfListener	= pfNotifier;
	pstNotiItem->pvUserData	= pvUserData;

	s_pstApkMrRec_NotiList = HLIB_LIST_Append (s_pstApkMrRec_NotiList, (void *)pstNotiItem);
	return ERR_OK;
}

HERROR APK_MEDIA_RECORD_UnregisterNotifier (HUINT32 ulSvcUid, OxMediaRec_Notifier_t pfNotifier, void *pvUserData)
{
	_apk_mr_Notifier_t	*pstNoti;

	if (NULL == pfNotifier)
	{
		HxLOG_Error("[%s:%d] Notifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pstNoti = apk_media_record_FindNotifier(ulSvcUid, pfNotifier, pvUserData);
	if (NULL == pstNoti)
	{
		HxLOG_Error("[%s:%d] Not Registered:\n", __FUNCTION__, __LINE__);
		return ERR_OK;
	}

	s_pstApkMrRec_NotiList = HLIB_LIST_Remove (s_pstApkMrRec_NotiList, (const void *)pstNoti);
	HLIB_STD_MemFree (pstNoti);

	return ERR_OK;
}

HERROR APK_MEDIA_RECORD_GetCapability (OxMediaRec_Capability_t *pstCap)
{
	HERROR hErr;

	hErr = OAPI_MEDIA_REC_GetCapability (pstCap);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_REC_GetCapability err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_RECORD_IsRecording (HUINT32 ulMasterSvcUid, HBOOL *pIsRecording)
{
	HERROR hErr;

	hErr = OAPI_MEDIA_REC_IsRecording (ulMasterSvcUid, pIsRecording);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_REC_IsRecording err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_RECORD_GetCID (HUINT32 ulMasterSvcUid, HUINT32 *pCID)
{
	HERROR hErr;

	hErr = OAPI_MEDIA_REC_GetCID (ulMasterSvcUid, pCID);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_REC_GetCID err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_RECORD_Start (OxMediaRec_StartInfo_t *pstStart, HUINT32 *pulSessionId, HCHAR * pszRecordID)
{
	HUINT32						 ulSlot;
	DxSchedule_t				 stSchedule;
	HERROR						 hErr;
	HINT32	nTSid, nSVCid, nOnid;

	if ( (NULL == pstStart) || (NULL == pulSessionId) || (NULL == pszRecordID) )
	{
		return ERR_FAIL;
	}

	hErr = SAPI_GetAvailableSlot(&ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_GetAvailableSlot err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stSchedule, 0, sizeof(DxSchedule_t));

	stSchedule.ulSlot			= ulSlot;
	stSchedule.eRsvType			= (pstStart->nStartOffset <= 0) ? DxRSVTYPE_RECORD_INSTANT : DxRSVTYPE_RECORD_DELAYED;
	stSchedule.ulSvcUid			= pstStart->ulSuppleSvcUid;
	if(APK_META_SVC_GetServiceTriplet(stSchedule.ulSvcUid, &nTSid, &nOnid, &nSVCid)==TRUE)
	{
		stSchedule.stTripleId.usTsId = nTSid;
		stSchedule.stTripleId.usSvcId = nSVCid;
		stSchedule.stTripleId.usOnId = nOnid;
	}
	else
	{
		HxLOG_Error("[%s:%d] Can not get Triple ID\n", __FUNCTION__, __LINE__);
		// Conflict 처리시에 Triple ID를 본다... samalib_cflt_compareScheduleUsingFilter
		return ERR_FAIL;
	}

	stSchedule.ulDuration		= pstStart->ulDuration;
	stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
	HxSTD_MemCopy (&(stSchedule.stStartTime), &(pstStart->stStartTime), sizeof(HxDATETIME_t));

	/* save event */
	stSchedule.uExtInfo.stRec.nEvtId	=	pstStart->nEvtId;
	HxSTD_MemCopy (&(stSchedule.uExtInfo.stRec.szEventName), pstStart->szName, sizeof(pstStart->szName));

	HLIB_STD_StrUtf8NCpy(stSchedule.uExtInfo.stRec.aucStorageUUID, "0", DxRSV_MAX_UUID_BUFSIZE);

#if 0
	/* OAPI_MEDIA_REC_Start 안의 oapi_mr_GetEmptyContext 에서 최대레코딩을 넘어갈 경우에 대한 에러처리를 해버려
	Sama에서 Conflict를 못올려주는 문제가 발생하여 막습니다. OAPI_MEDIA_REC_Start는 추후 사용가능성 있어 코드는 지우지 않음...*/
	hErr = OAPI_MEDIA_REC_Start(pstStart, ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] OAPI_MEDIA_REC_Start err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	hErr = SAPI_MakeSchedule (ulSlot, &stSchedule, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] SAPI_MakeSchedule err:\n", __FUNCTION__, __LINE__);
		OAPI_MEDIA_REC_Stop(ulSlot);
		return ERR_FAIL;
	}
#else
	hErr = SAPI_MakeSchedule (ulSlot, &stSchedule, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] SAPI_MakeSchedule err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
#endif
	if (NULL != pulSessionId)
	{
		*pulSessionId = ulSlot;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_RECORD_Stop (HUINT32 ulSessionId)
{
	HBOOL						 bStopped = FALSE;
	HUINT32						 ulCnt, ulNumSch = 0;
	DxSchedule_t				*pstSchArray = NULL;
	HERROR						 hErr, hResult = ERR_FAIL;
	HUINT32 					 ulSvcUid;

	(void)SAPI_CountSchedule(&ulNumSch);
	if (0 == ulNumSch)
	{
		HxLOG_Error("[%s:%d] No Schedule: Nothing to stop\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("[%s:%d] memory allocation failed\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_GetAllSchedules err:\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	hErr = OAPI_MEDIA_REC_GetSVCUid( ulSessionId, &ulSvcUid );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_REC_GetSVCUid err:\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
	{
		DxSchedule_t		*pstSchedule = &(pstSchArray[ulCnt]);

		switch (pstSchedule->eRsvType)
		{
		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_INSTANT:
		case DxRSVTYPE_RECORD_DELAYED:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
			if ((pstSchedule->ulSvcUid == ulSvcUid) && (DxRSVSTATUS_RUNNING == pstSchedule->eStatus))
			{
				hErr = SAPI_StopSchedule(pstSchedule->ulSlot);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("[%s:%d] SAPI_CancelSchedule failed: ulSlot(%d)\n", __FUNCTION__, __LINE__, pstSchedule->ulSlot);
					return ERR_FAIL;
				}

				bStopped = TRUE;
			}

			break;

		default:
			break;
		}
	}

	hResult = (bStopped) ? ERR_OK : ERR_FAIL;

END_FUNC:
	if (pstSchArray)
		HLIB_STD_MemFree(pstSchArray);

	return hResult;
}

HERROR APK_MEDIA_RECORD_StopByUID (HUINT32 ulSvcUid)
{
	HBOOL						 bStopped = FALSE;
	HUINT32						 ulCnt, ulNumSch = 0;
	DxSchedule_t				*pstSchArray = NULL;
	HERROR						 hErr, hResult = ERR_FAIL;

	(void)SAPI_CountSchedule(&ulNumSch);
	if (0 == ulNumSch)
	{
		HxLOG_Error("[%s:%d] No Schedule: Nothing to stop\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("[%s:%d] memory allocation failed\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_GetAllSchedules err:\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
	{
		DxSchedule_t		*pstSchedule = &(pstSchArray[ulCnt]);

		switch (pstSchedule->eRsvType)
		{
		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_INSTANT:
		case DxRSVTYPE_RECORD_DELAYED:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
			if ((pstSchedule->ulSvcUid == ulSvcUid) && (DxRSVSTATUS_RUNNING == pstSchedule->eStatus))
			{
				hErr = SAPI_StopSchedule(pstSchedule->ulSlot);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("[%s:%d] SAPI_CancelSchedule failed: ulSlot(%d)\n", __FUNCTION__, __LINE__, pstSchedule->ulSlot);
					return ERR_FAIL;
				}

				bStopped = TRUE;
			}

			break;

		default:
			break;
		}
	}

	hResult = (bStopped) ? ERR_OK : ERR_FAIL;

END_FUNC:
	if (pstSchArray)
		HLIB_STD_MemFree(pstSchArray);

	return hResult;
}

/********************************************************************************
 * APK_MEDIA_REC_GetStartTime
 ********************************************************************************/
HERROR APK_MEDIA_RECORD_GetStartTime(HUINT32 ulSessionId, HUINT32 *pulStartTime)
{
	HERROR hErr;

	if( NULL == pulStartTime )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_REC_GetStartTime (ulSessionId, pulStartTime );
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] OAPI_MEDIA_REC_GetStartTime err: ulSessionId=%d\n", __FUNCTION__, __LINE__, ulSessionId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_REC_GetRecTime
 ********************************************************************************/
HERROR APK_MEDIA_RECORD_GetRecTime (HUINT32 ulSessionId, HUINT32 *pulRecTime)
{
	HERROR hErr;

	if( NULL == pulRecTime )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_REC_GetRecTime (ulSessionId, pulRecTime );
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] OAPI_MEDIA_REC_GetRecTime err: ulSessionId=%d\n", __FUNCTION__, __LINE__, ulSessionId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_REC_GetURL
 ********************************************************************************/
HERROR APK_MEDIA_RECORD_GetURL(HUINT32 ulSessionId, HCHAR * pszRecordID)
{
	return apk_media_record_GetURL( ulSessionId, pszRecordID );
}

/********************************************************************************
 * APK_MEDIA_RECORD_GetSessionIdBySvcUid
 ********************************************************************************/
HERROR APK_MEDIA_RECORD_GetRsvSlotByUID(HUINT32 ulSvcUid, HUINT32 *pulRsvSlot)
{
	HBOOL						 bFound = FALSE;
	HUINT32 					 ulCnt, ulNumSch = 0;
	DxSchedule_t				*pstSchArray = NULL;
	HERROR						 hErr, hResult = ERR_FAIL;

	if (NULL == pulRsvSlot)
	{
		HxLOG_Error("[%s:%d] incorrect param!!!\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	(void)SAPI_CountSchedule(&ulNumSch);
	if (0 == ulNumSch)
	{
		HxLOG_Error("[%s:%d] No Schedule: Nothing to stop\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("[%s:%d] memory allocation failed\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_GetAllSchedules err:\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	for (ulCnt = 0; (ulCnt < ulNumSch) && !bFound ; ulCnt++)
	{
		DxSchedule_t		*pstSchedule = &(pstSchArray[ulCnt]);

		switch (pstSchedule->eRsvType)
		{
		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_INSTANT:
		case DxRSVTYPE_RECORD_DELAYED:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
			if ((pstSchedule->ulSvcUid == ulSvcUid) && (DxRSVSTATUS_RUNNING == pstSchedule->eStatus))
			{
				*pulRsvSlot = pstSchedule->ulSlot;
				bFound = TRUE;
			}

			break;

		default:
			break;
		}
	}

	hResult = (bFound) ? ERR_OK : ERR_FAIL;

END_FUNC:
	if (pstSchArray)
		HLIB_STD_MemFree(pstSchArray);

	return hResult;


}

HERROR APK_MEDIA_RECORD_GetContentId (HUINT32 ulSlot, HUINT32 *pulContentId)
{
	return OAPI_MEDIA_REC_GetContentId(ulSlot, pulContentId);
}


/* end of file */
