/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_resource.c
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

#include <hlib.h>
#include <rlib.h>
#include <apk.h>
#include <_apk_int.h>
#include <oapi_resource.h>

#define ___INTERNAL_STRUCTURES___
typedef struct
{
	OxRsc_Notifier_t		 pfListener;
	void					*pvUserData;
} _apk_rsc_Notifier_t;

#define ___INTERNAL_VARIABLES___
STATIC HxList_t				*s_pstApkRsc_NotiList = NULL;		// _apk_rsc_Notifier_t

#define ___PRIVATE_FUNCTION___

#define _____NOTIFIER_FUNCTION_____
STATIC void	apk_rsc_FreeCbNotifier (HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 4);

	pvData = apArgV[2];
	if (pvData)
		APK_Free(pvData);
}

STATIC void apk_rsc_CbNotifier (OxRsc_Event_e eEvent, OxRsc_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HxList_t		*pstListItem;
	APK_EVENT_EMIT_ARGS	stApkEvtArgs;

	for (pstListItem = s_pstApkRsc_NotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		_apk_rsc_Notifier_t		*pstNotiItem = (_apk_rsc_Notifier_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotiItem) && (NULL != pstNotiItem->pfListener))
		{
			HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));
			stApkEvtArgs.pfnFreeFunc = apk_rsc_FreeCbNotifier;
			stApkEvtArgs.ulArgCount = 4;
			stApkEvtArgs.apArgV[0] = (void *)pstNotiItem->pfListener;
			stApkEvtArgs.apArgV[1] = (void *)eEvent;
			stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pstNotifyData, sizeof(OxRsc_NotifierData_t));
			stApkEvtArgs.apArgV[3] = (void *)pstNotiItem->pvUserData;
			APK_EVENT_Emit(&stApkEvtArgs);
		}
	}
}

STATIC _apk_rsc_Notifier_t *apk_rsc_FindNotifier (OxRsc_Notifier_t pfNotifier, void *pvUserData)
{
	HxList_t		*pstListItem;

	for (pstListItem = s_pstApkRsc_NotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		_apk_rsc_Notifier_t		*pstNotiItem = (_apk_rsc_Notifier_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstNotiItem)
		{
			if ((pfNotifier == pstNotiItem->pfListener) &&
				(pvUserData == pstNotiItem->pvUserData))
			{
				return pstNotiItem;
			}
		}
	}

	return NULL;
}


#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR APK_RSC_Init (void)
{
	HERROR			 hErr;

	hErr = OAPI_RSC_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_RSC_Init error:\n");
		return ERR_FAIL;
	}

	hErr = OAPI_RSC_SetListener(apk_rsc_CbNotifier, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_RSC_SetListener error:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_RSC_SetListener (OxRsc_Notifier_t pfNotifier, void *pvUserData)
{
	_apk_rsc_Notifier_t			*pstNotiItem;

	if (NULL == pfNotifier)					{ return ERR_FAIL; }

	pstNotiItem = apk_rsc_FindNotifier(pfNotifier, pvUserData);
	if (NULL != pstNotiItem)
	{
		HxLOG_Error("already pfNotifier and pvUserData registered:\n");
		return ERR_OK;
	}

	pstNotiItem = (_apk_rsc_Notifier_t *)APK_Calloc(sizeof(_apk_rsc_Notifier_t));
	if (NULL == pstNotiItem)
	{
		HxLOG_Error("Memory allocation failed:");
		return ERR_FAIL;
	}

	pstNotiItem->pfListener = pfNotifier;
	pstNotiItem->pvUserData = pvUserData;

	s_pstApkRsc_NotiList = HLIB_LIST_Append(s_pstApkRsc_NotiList, (void *)pstNotiItem);
	return ERR_OK;
}

HERROR APK_RSC_ResetListener (OxRsc_Notifier_t pfNotifier, void *pvUserData)
{
	_apk_rsc_Notifier_t			*pstNotiItem;

	if (NULL == pfNotifier)					{ return ERR_FAIL; }

	pstNotiItem = apk_rsc_FindNotifier(pfNotifier, pvUserData);
	if (NULL == pstNotiItem)
	{
		HxLOG_Error("pfNotifier and pvUserData not registered:\n");
		return ERR_OK;
	}

	s_pstApkRsc_NotiList = HLIB_LIST_Remove(s_pstApkRsc_NotiList, (const void *)pstNotiItem);
	APK_Free(pstNotiItem);

	return ERR_OK;
}

HERROR APK_RSC_GetInitialInfo (OxRsc_InitialInfo_t *pstInitInfo)
{
	return OAPI_RSC_GetInitialInfo(pstInitInfo);
}

HERROR APK_RSC_GetResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId)
{
	return OAPI_RSC_GetResourceId(ulActionId, eRscType, pulRscId);
}

HERROR APK_RSC_GetResourceNumber( RxRscType_e eRscType, HUINT32 *pulRscNum)
{
	return OAPI_RSC_GetResourceNumber(eRscType,pulRscNum);
}

Handle_t APK_RSC_MakeResourceRequestHandle (HUINT32 ulActionId, HUINT32 ulActionPriority, HBOOL bUseVirtualHandle)
{
	return OAPI_RSC_MakeResourceRequestHandle(ulActionId, ulActionPriority, bUseVirtualHandle);
}

Handle_t APK_RSC_DuplicateRequestHandle (RxRscReqHandle_t hRscReq)
{
	return OAPI_RSC_DuplicateRequestHandle(hRscReq);
}

HERROR APK_RSC_AddRemovableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId, HUINT32 ulActionPriority)
{
	return OAPI_RSC_AddRemovableActionIdToRequestHandle(hRscReq, ulActionId, ulActionPriority);
}

HERROR APK_RSC_AddIgnorableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId)
{
	return OAPI_RSC_AddIgnorableActionIdToRequestHandle(hRscReq, ulActionId);
}

HERROR APK_RSC_AddRequiredResourceToRequestHandle (Handle_t hRscReq, RxRscType_e eRscType, HUINT32 ulResourceId, HBOOL bSharable, RxUsageType_e eUsageType)
{
	return OAPI_RSC_AddRequiredResourceToRequestHandle(hRscReq, eRscType, ulResourceId, bSharable, eUsageType);
}

HERROR APK_RSC_AddArgumentToRequestHandle (Handle_t hRscReq, RxTrialArg_t *punArg)
{
	return OAPI_RSC_AddArgumentToRequestHandle(hRscReq, punArg);
}

HERROR APK_RSC_TryResourceRequest (Handle_t hRscReq)
{
	return OAPI_RSC_TryResourceRequest(hRscReq);
}

RxRscTrialResult_t *APK_RSC_GetRequestTrialResult (Handle_t hRscReq)
{
	return OAPI_RSC_GetRequestTrialResult(hRscReq);
}

HERROR APK_RSC_DestroyResourceRequestHandle (RxRscReqHandle_t hRscReq)
{
	return OAPI_RSC_DestroyResourceRequestHandle(hRscReq);
}

HERROR APK_RSC_SetResourcesByTrialResults (RxRscReqHandle_t hRscReq)
{
	return OAPI_RSC_SetResourcesByTrialResults(hRscReq);
}

HERROR APK_RSC_GetEmptyRecordAndBgActionIdArray (HUINT32 *pulActionIdArray, HUINT32 ulNum)
{
	return OAPI_RSC_GetEmptyRecordAndBgActionIdArray(pulActionIdArray, ulNum);
}

HERROR APK_RSC_GetEmptyRecordActionIdArray (HUINT32 *pulActionIdArray, HUINT32 ulNum)
{
	return OAPI_RSC_GetEmptyRecordActionIdArray(pulActionIdArray, ulNum);
}

HERROR APK_RSC_GetEmptyRecordActionId (HUINT32 *pulActionId)
{
	return OAPI_RSC_GetEmptyRecordActionId(pulActionId);
}

HERROR APK_RSC_GetEmptyBgActionId (HUINT32 *pulActionId)
{
	return OAPI_RSC_GetEmptyBgActionId(pulActionId);
}

HERROR APK_RSC_GetActionPriority (HUINT32 ulActionId, HUINT32 *pulPriority)
{
	return OAPI_RSC_GetActionPriority(ulActionId, pulPriority);
}

HERROR APK_RSC_ResetVirtualHandleInfo (void)
{
	return OAPI_RSC_ResetVirtualHandleInfo();
}


/* end of file */
