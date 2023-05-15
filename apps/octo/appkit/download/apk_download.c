/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_download.c
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <apk.h>
#include <_apk_int.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_DNLD_CRITICAL		HxSEMT_Get(s_ulDNLDSemId)
#define	LEAVE_DNLD_CRITICAL		HxSEMT_Release(s_ulDNLDSemId)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	APK_DOWNLOAD_Notifier_t	 pfListener;
	void					*pvUserData;
} _apk_dnld_Notifier_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HxList_t				*s_pstApkDnld_NotiList = NULL;
STATIC HUINT32				s_ulDNLDSemId = 0;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___PRIVATE_FUNCTION___

#define _____NOTIFIER_FUNCTION_____
STATIC void	apk_download_freeCbNotifier(HINT32 nArgc, void *apArgv[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 6);

	pvData = apArgv[4];
	if (pvData)
	{
		APK_Free(pvData);
	}
}

STATIC void apk_download_CbNotifier (HUINT32 ulDownloadId, DxDownload_Type_e eType, DxDownload_Event_e eEvent, DxDownload_NotifierData_t *punNotifyData, void *pvUserData)
{
	HxList_t		*pstListItem;

	ENTER_DNLD_CRITICAL;
	for (pstListItem = s_pstApkDnld_NotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		_apk_dnld_Notifier_t		*pstNotiItem = (_apk_dnld_Notifier_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotiItem) && (NULL != pstNotiItem->pfListener))
		{
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			stApkEvtArgs.pfnFreeFunc = apk_download_freeCbNotifier;
			stApkEvtArgs.ulArgCount = 6;
			stApkEvtArgs.apArgV[0] = (void *)pstNotiItem->pfListener;
			stApkEvtArgs.apArgV[1] = (void *)ulDownloadId;
			stApkEvtArgs.apArgV[2] = (void *)eType;
			stApkEvtArgs.apArgV[3] = (void *)eEvent;
			stApkEvtArgs.apArgV[4] = (void *)APK_MemDup(punNotifyData, sizeof(DxDownload_NotifierData_t));
			stApkEvtArgs.apArgV[5] = (void *)pstNotiItem->pvUserData;

			APK_EVENT_Emit(&stApkEvtArgs);
		}
	}
	LEAVE_DNLD_CRITICAL;
}

STATIC void apk_download_CbInitNotified (void)
{
	HxLOG_Info("[%s:%d] Download Activated: Now it can be used:\n", __FUNCTION__, __LINE__ );
	OAPI_DOWNLOAD_SetListener(apk_download_CbNotifier, NULL);
}

STATIC _apk_dnld_Notifier_t *apk_download_FindNotifier (APK_DOWNLOAD_Notifier_t pfNotifier, void *pvUserData)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstApkDnld_NotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		_apk_dnld_Notifier_t		*pstNoti = (_apk_dnld_Notifier_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstNoti) &&
			(pstNoti->pfListener == pfNotifier) &&
			(pstNoti->pvUserData == pvUserData))
		{
			return pstNoti;
		}
	}

	return NULL;
}

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR APK_DOWNLOAD_Init (void)
{
	HxSEMT_Create(&s_ulDNLDSemId, "apkdnld", HxSEMT_FIFO);
	OAPI_DOWNLOAD_Init(apk_download_CbInitNotified);
	return ERR_OK;
}

HERROR APK_DOWNLOAD_RegisterNotifier (APK_DOWNLOAD_Notifier_t pfNotifier, void *pvUserData)
{
	_apk_dnld_Notifier_t			*pstNotiItem;

	if (NULL == pfNotifier)
	{
		HxLOG_Error ("[%s:%d] Notifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	ENTER_DNLD_CRITICAL;
	pstNotiItem = apk_download_FindNotifier(pfNotifier, pvUserData);
	if (NULL != pstNotiItem)
	{
		HxLOG_Error("[%s:%d] Already Registered:\n", __FUNCTION__, __LINE__);
		LEAVE_DNLD_CRITICAL;
		return ERR_OK;
	}

	pstNotiItem = (_apk_dnld_Notifier_t *)HLIB_STD_MemCalloc(sizeof(_apk_dnld_Notifier_t));
	if (NULL == pstNotiItem)
	{
		HxLOG_Error("[%s:%d] Memory allocation failed:\n", __FUNCTION__, __LINE__);
		LEAVE_DNLD_CRITICAL;
		return ERR_FAIL;
	}

	pstNotiItem->pfListener	= pfNotifier;
	pstNotiItem->pvUserData	= pvUserData;

	s_pstApkDnld_NotiList = HLIB_LIST_Append(s_pstApkDnld_NotiList, (void *)pstNotiItem);
	LEAVE_DNLD_CRITICAL;
	return ERR_OK;
}

HERROR APK_DOWNLOAD_UnregisterNotifier (APK_DOWNLOAD_Notifier_t pfNotifier, void *pvUserData)
{
	_apk_dnld_Notifier_t	*pstNoti;

	if (NULL == pfNotifier)
	{
		HxLOG_Error("[%s:%d] Notifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	ENTER_DNLD_CRITICAL;
	pstNoti = apk_download_FindNotifier(pfNotifier, pvUserData);
	if (NULL == pstNoti)
	{
		HxLOG_Error("[%s:%d] Not Registered:\n", __FUNCTION__, __LINE__);
		LEAVE_DNLD_CRITICAL;
		return ERR_OK;
	}

	s_pstApkDnld_NotiList = HLIB_LIST_Remove (s_pstApkDnld_NotiList, (const void *)pstNoti);
	LEAVE_DNLD_CRITICAL;
	HLIB_STD_MemFree (pstNoti);

	return ERR_OK;
}

Handle_t APK_DOWNLOAD_Probe (DxDownload_Start_t *punStart)
{
	return OAPI_DOWNLOAD_Probe(punStart);
}


Handle_t APK_DOWNLOAD_Start (Handle_t hDownload, DxDownload_Start_t *punStart)
{
	return OAPI_DOWNLOAD_Start(hDownload, punStart);
}

HERROR APK_DOWNLOAD_Stop (Handle_t hDownload)
{
	return OAPI_DOWNLOAD_Stop(hDownload);
}

HERROR APK_DOWNLOAD_Pause (Handle_t hDownload)
{
	return OAPI_DOWNLOAD_Pause(hDownload);
}

HERROR APK_DOWNLOAD_Resume (Handle_t hDownload)
{
	return OAPI_DOWNLOAD_Resume(hDownload);
}

HERROR APK_DOWNLOAD_GetStatus (Handle_t hDownload, DxDownload_Status_t *pstStatus)
{
	return OAPI_DOWNLOAD_GetStatus(hDownload, pstStatus);
}

HERROR APK_DOWNLOAD_GetBufferingChunkInfo (Handle_t hDownload, HUINT32 *pnChunks, HUINT32 **ppulStartOffsets, HUINT32 **ppulEndOffsets)
{
	return OAPI_DOWNLOAD_GetBufferingChunkInfo(hDownload, pnChunks, ppulStartOffsets, ppulEndOffsets);
}

HERROR APK_DOWNLOAD_GetSeekableRangeInfo (Handle_t hDownload, HUINT32 *nChunks, HUINT64 **ppullSeekableStart, HUINT64 **ppullSeekableEnd)
{
	return OAPI_DOWNLOAD_GetSeekableRangeInfo(hDownload, nChunks, ppullSeekableStart, ppullSeekableEnd);
}

HERROR APK_DOWNLOAD_GetRemainBufferingTime (Handle_t hDownload, HUINT32 *punRemainBufferingTime)
{
	return OAPI_DOWNLOAD_GetRemainBufferingTime (hDownload, punRemainBufferingTime);
}

HERROR APK_DOWNLOAD_GetBufferingPercent (Handle_t hDownload, HUINT32 *pulBufferPercent)
{
	return OAPI_DOWNLOAD_GetBufferingPercent(hDownload, pulBufferPercent);
}

HERROR APK_DOWNLOAD_GetBufferedBytes (Handle_t hDownload, HINT64 *pllBufferedBytes)
{
	return OAPI_DOWNLOAD_GetBufferedBytes (hDownload, pllBufferedBytes);

}

HERROR APK_DOWNLOAD_GetRemainingBytes (Handle_t hDownload, HINT64 *pllRemainingBytes)
{
	return OAPI_DOWNLOAD_GetRemainingBytes (hDownload, pllRemainingBytes);
}

HERROR APK_DOWNLOAD_GetComponentNum (Handle_t hDownload, OxDownload_Component_e eCompType, HUINT32 *pulCompNum)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pulCompNum )
		return ERR_FAIL;

	hErr = OAPI_DOWNLOAD_GetComponentNum ( hDownload, eCompType, pulCompNum);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_DOWNLOAD_GetComponentNum err: hDownload=%d\n", hDownload);
		return ERR_FAIL;
	}

	return ERR_OK;
}
HERROR APK_DOWNLOAD_GetComponent (Handle_t hDownload, OxDownload_Component_e eCompType, HINT32 nCompIndex, OxDownload_Component_t *pstComponet)
{
	HERROR hErr;

	if( NULL == pstComponet )
		return ERR_FAIL;

	hErr = OAPI_DOWNLOAD_GetComponent (hDownload, eCompType, nCompIndex, pstComponet);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_DOWNLOAD_GetComponent err: hDownload=%d\n", hDownload);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_DOWNLOAD_GetComponentIndex (Handle_t hDownload, OxDownload_Component_e eCompType, HINT32 *pnCompIndex, HINT32 *pnCompSubIndex)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pnCompIndex )
		return ERR_FAIL;

	hErr = OAPI_DOWNLOAD_GetComponentIndex ( hDownload, eCompType, pnCompIndex, pnCompSubIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_DOWNLOAD_GetComponentIndex err: hDownload=%d\n", hDownload);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_DOWNLOAD_GetAvailabilityStartTime (Handle_t hDownload, HUINT32 pnSzBuffer, HUINT8 *pszTimeString)
{
	HERROR hErr;

	HxLOG_Trace();

	if(NULL == pszTimeString)
		return ERR_FAIL;

	hErr = OAPI_DOWNLOAD_GetAvailabilityStartTime(hDownload, pnSzBuffer, pszTimeString);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_DOWNLOAD_GetAvailabilityStartTime err: hDownload=%d\n", hDownload);
		return ERR_FAIL;
	}

	return ERR_OK;
}


/* end of file */
