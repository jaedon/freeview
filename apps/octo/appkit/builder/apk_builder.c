/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_builder.c
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
#include <apk_builder.h>

#define ___INTERNAL_STRUCTURES___

typedef struct
{
	OxBuilder_Listener_t	 pfListener;
	void					*pvUserData;
} apkBuilder_Notifier_t;

#define apkBuilder_ScanType_NormalScan			"normal"
#define apkBuilder_ScanType_AntConnection		"antennaconnection"
#define apkBuilder_ScanType_DiseqcDetect		"diseqcdetection"
#define apkBuilder_ScanType_TuneOnly			"tuneonly"
#define apkBuilder_ScanType_CiplusScan			"ciplussearch"
/*********************************************************************/
#define apkBuilder_ScanType_ERTSearch			"ertsearch"
#define apkBuilder_ScanType_OpTpListSearch		"optplistsearch"
/*********************************************************************/


#define ___INTERNAL_VARIABLES___

STATIC HxList_t				*s_pstApkBuilder_NotiList = NULL;

#define ___PRIVATE_FUNCTION___

STATIC HERROR apk_builder_GetScanOptionType(HCHAR *scanType, OxBuilder_OptionType_e *peOptType)
{
	OxBuilder_OptionType_e	eOptType;

	if ((scanType == NULL) || (peOptType == NULL))
	{
		return ERR_FAIL;
	}

	if (HxSTD_StrCmp(scanType, apkBuilder_ScanType_AntConnection) == 0)
	{
		eOptType = eOxBUILDER_OPTIONTYPE_ANTCONNECT;
	}
	else if (HxSTD_StrCmp(scanType, apkBuilder_ScanType_DiseqcDetect) == 0)
	{
		eOptType = eOxBUILDER_OPTIONTYPE_DISEQCDETECT;
	}
	else if (HxSTD_StrCmp(scanType, apkBuilder_ScanType_TuneOnly) == 0)
	{
		eOptType = eOxBUILDER_OPTIONTYPE_TUNEONLY;
	}
	else if (HxSTD_StrCmp(scanType, apkBuilder_ScanType_CiplusScan) == 0)
	{
		eOptType = eOxBUILDER_OPTIONTYPE_CIPLUS_SEARCH;
	}
/*********************************************************************/
	else if (HxSTD_StrCmp(scanType, apkBuilder_ScanType_ERTSearch) == 0)
	{
		eOptType = eOxBUILDER_OPTIONTYPE_ERTELECOM_SEARCH;
	}
	else if (HxSTD_StrCmp(scanType, apkBuilder_ScanType_OpTpListSearch) == 0)
	{
		eOptType = eOxBUILDER_OPTIONTYPE_OP_TP_LIST_SEARCH;
	}
/*********************************************************************/
	else
	{
		eOptType = eOxBUILDER_OPTIONTYPE_SVCSEARCH;
	}

	*peOptType = eOptType;
	return ERR_OK;
}

STATIC	HCHAR* apk_builder_GetScanTypeStr(OxBuilder_OptionType_e eOptionType)
{
	HCHAR				*szScanType = apkBuilder_ScanType_NormalScan;

	switch(eOptionType)
	{
		case eOxBUILDER_OPTIONTYPE_SVCSEARCH:				szScanType = apkBuilder_ScanType_NormalScan;			break;
		case eOxBUILDER_OPTIONTYPE_ANTCONNECT:				szScanType = apkBuilder_ScanType_AntConnection;			break;
		case eOxBUILDER_OPTIONTYPE_DISEQCDETECT:			szScanType = apkBuilder_ScanType_DiseqcDetect;			break;
		case eOxBUILDER_OPTIONTYPE_TUNEONLY:				szScanType = apkBuilder_ScanType_TuneOnly;				break;
		case eOxBUILDER_OPTIONTYPE_CIPLUS_SEARCH:			szScanType = apkBuilder_ScanType_CiplusScan;			break;
/*********************************************************************/
		case eOxBUILDER_OPTIONTYPE_ERTELECOM_SEARCH:		szScanType = apkBuilder_ScanType_ERTSearch;				break;
		case eOxBUILDER_OPTIONTYPE_OP_TP_LIST_SEARCH:		szScanType = apkBuilder_ScanType_OpTpListSearch;		break;
/*********************************************************************/
		default:
			HxLOG_Error("Invalid eOptionType(%d)... return normal scan \n", eOptionType);
			szScanType = apkBuilder_ScanType_NormalScan;
			break;
	}

	return szScanType;
}

#define ___MEMBER_FUNCTION___

#define ___NOTIFIER_FUNCTION___
STATIC void	apk_builder_freeCbNotifier(HINT32 nArgc, void *apArgV[])
{
	DxBuilder_NotifierData_t	*pstNotifyData;

	HxLOG_Assert(nArgc == 5);

	pstNotifyData = (DxBuilder_NotifierData_t*)apArgV[3];
	if (NULL != pstNotifyData)
	{
		APK_Free(pstNotifyData);
	}
}

STATIC void apk_builder_CbNotifier (HUINT32 ulBuilderId, DxBuilder_Event_e eEvent, DxBuilder_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HxList_t		*pstListItem;

	for (pstListItem = s_pstApkBuilder_NotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		apkBuilder_Notifier_t		*pstNotiItem = (apkBuilder_Notifier_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotiItem)
			 && (NULL != pstNotiItem->pfListener))
		{
			DxBuilder_NotifierData_t	*pDupData = NULL;
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0, };
			if(NULL != pstNotifyData)
			{
				pDupData = (DxBuilder_NotifierData_t *)APK_MemDup(pstNotifyData, sizeof(DxBuilder_NotifierData_t));
			}

			stApkEvtArgs.pfnFreeFunc = apk_builder_freeCbNotifier;
			stApkEvtArgs.ulArgCount = 5;
			stApkEvtArgs.apArgV[0] = (void *)pstNotiItem->pfListener;
			stApkEvtArgs.apArgV[1] = (void *)ulBuilderId;
			stApkEvtArgs.apArgV[2] = (void *)eEvent;
			stApkEvtArgs.apArgV[3] = (void *)pDupData;
			stApkEvtArgs.apArgV[4] = (void *)pstNotiItem->pvUserData;

			APK_EVENT_Emit(&stApkEvtArgs);
		}
	}

	(void)pstNotifyData;
}

STATIC void apk_builder_CbInitNotified (void)
{
	HERROR	 hErr;

	hErr = OAPI_BUILDER_SetListener (apk_builder_CbNotifier, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_SetListener err:\n");
	}
	return;
}

STATIC apkBuilder_Notifier_t *apk_builder_FindNotifier (OxBuilder_Listener_t pfNotifier, void *pvUserData)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstApkBuilder_NotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		apkBuilder_Notifier_t		*pstNoti = (apkBuilder_Notifier_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstNoti)
 			&& (pstNoti->pfListener == pfNotifier)
			&& (pstNoti->pvUserData == pvUserData))
		{
			return pstNoti;
		}
	}

	return NULL;
}


#define ___API_FUNCTION___
HERROR APK_BUILDER_Init (void)
{
	OAPI_BUILDER_Init (apk_builder_CbInitNotified);
	return ERR_OK;
}

HERROR APK_BUILDER_RegisterNotifier (HUINT32 hBuilderHandle, APK_BUILDER_Notifier_t pfNotifier, void *pvUserData)
{
	apkBuilder_Notifier_t		*pstNotiItem;

	if (NULL == pfNotifier)
	{
		HxLOG_Error ("Notifier NULL \n");
		return ERR_FAIL;
	}

	pstNotiItem = apk_builder_FindNotifier ((OxBuilder_Listener_t)pfNotifier, pvUserData);
	if (NULL != pstNotiItem)
	{
		HxLOG_Error("Already Registered:\n");
		return ERR_OK;
	}

	pstNotiItem = (apkBuilder_Notifier_t *)HLIB_STD_MemCalloc(sizeof(apkBuilder_Notifier_t));
	if (NULL == pstNotiItem)
	{
		HxLOG_Error("Memory allocation failed:\n");
		return ERR_FAIL;
	}

	pstNotiItem->pfListener	= (OxBuilder_Listener_t)pfNotifier;
	pstNotiItem->pvUserData	= pvUserData;

	s_pstApkBuilder_NotiList = HLIB_LIST_Append (s_pstApkBuilder_NotiList, (void *)pstNotiItem);

	return ERR_OK;
}

HERROR APK_BUILDER_UnregisterNotifier (HUINT32 hBuilderHandle, APK_BUILDER_Notifier_t pfNotifier, void *pvUserData)
{
	apkBuilder_Notifier_t		*pstNotiItem;

	if (NULL == pfNotifier)
	{
		HxLOG_Error ("Notifier NULL \n");
		return ERR_FAIL;
	}

	pstNotiItem = apk_builder_FindNotifier((OxBuilder_Listener_t)pfNotifier, pvUserData);
	if (NULL == pstNotiItem)
	{
		HxLOG_Error("Not Registered:\n");
		return ERR_OK;
	}

	s_pstApkBuilder_NotiList = HLIB_LIST_Remove (s_pstApkBuilder_NotiList, (const void *)pstNotiItem);
	HLIB_STD_MemFree (pstNotiItem);

	return ERR_OK;
}

HERROR APK_BUILDER_OpenSession (DxDeliveryType_e eDeliveryType, HCHAR *scanType, ApkBuilder_Action_t *pstInfo, HUINT32 *phBuilderHandle)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = 0;
	OxBuilder_Option_t	stOption;

	HxLOG_Info("+++ scanType(%s) \n", scanType);

	hErr = OAPI_BUILDER_Open(&ulBuilderId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_Open Error.\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stOption, 0x00, sizeof(OxBuilder_Option_t));

	// set search type
	hErr = apk_builder_GetScanOptionType(scanType, &stOption.eOptionType);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("apk_builder_GetScanOptionType Error.\n");
		return ERR_FAIL;
	}

	// set deli. type
	stOption.eDeliveryType = eDeliveryType;

	// set deli. info
	if (pstInfo != NULL)
	{
		if (stOption.eDeliveryType == eDxDELIVERY_TYPE_SAT)
		{
			HxSTD_MemCopy(&stOption.unArgs.stSatInfo, &pstInfo->utInfo.satInfo, sizeof(DxBuilder_SatelliteInfo_t));
		}
		else
		{
			// nothing to do now
		}
	}

	// SET search option
	hErr = OAPI_BUILDER_SetOption(ulBuilderId, &stOption);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_SetOption Error.\n");
		return ERR_FAIL;
	}

	*phBuilderHandle = (HUINT32)ulBuilderId;

	if (pstInfo != NULL)
	{
		HxLOG_Debug("scanType(%s) deliType(%d) builderId(%d)\n", scanType, pstInfo->eDeliveryType, ulBuilderId);
	}
	else
	{
		HxLOG_Debug("pstInfo is NULL \n");
	}

	return ERR_OK;
}

HERROR APK_BUILDER_CloseSession (HUINT32 hBuilderHandle)
{
	HERROR				hErr;
	HUINT32				ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("ulBuilderId(%d) --- \n", ulBuilderId);

	hErr = OAPI_BUILDER_ClearBuildInfo (ulBuilderId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_ClearBuildInfo Err:\n");
	}

	hErr = OAPI_BUILDER_Close (ulBuilderId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_Close Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_ClearBuildInfo (HUINT32 hBuilderHandle)
{
	HERROR				hErr;
	HUINT32				ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	hErr = OAPI_BUILDER_ClearBuildInfo (ulBuilderId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_ClearBuildInfo Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_AddBuildInfo (HUINT32 hBuilderHandle, DxTuneParam_t *pstInfo)
{
	HERROR					hErr = ERR_FAIL;
	HUINT32					ulBuilderId = (HUINT32)hBuilderHandle;
	OxBuilder_Option_t		stOption;
	DxBuilder_BuildInfo_t 	stParam;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	if (pstInfo == NULL)
	{
		HxLOG_Error("Invalid parameter \n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stOption, 0x00, sizeof(OxBuilder_Option_t));
	HxSTD_MemSet(&stParam, 0x00, sizeof(DxBuilder_BuildInfo_t));

	// get delivery type
	hErr = OAPI_BUILDER_GetOption(ulBuilderId, &stOption);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_GetOption failed.. \n");
		stParam.unArgs.stTuneParam.eDeliType = eDxDELIVERY_TYPE_SAT;
	}
	else
	{
		stParam.unArgs.stTuneParam.eDeliType = stOption.eDeliveryType;
	}

	stParam.eType = eDxBUILDER_BUILDINFO_TP;
	HxSTD_MemCopy(&stParam.unArgs.stTuneParam, pstInfo, sizeof(DxTuneParam_t));

	hErr = OAPI_BUILDER_AddBuildInfo(ulBuilderId, &stParam);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_AddBuildInfo failed.. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_RemoveBuildInfo (HUINT32 hBuilderHandle, DxBuilder_BuildInfo_t *pstParam)
{
	HERROR					hErr = ERR_FAIL;
	HUINT32					ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	hErr = OAPI_BUILDER_RemoveBuildInfo (ulBuilderId, pstParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_RemoveBuildInfo Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_GetOption (HUINT32 hBuilderHandle, DxBuilder_Option_t *pstInfo)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32				ulBuilderId = (HUINT32)hBuilderHandle;
	OxBuilder_Option_t 	stOption;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	if (pstInfo == NULL)
	{
		HxLOG_Error("Invalid parameter \n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stOption, 0x00, sizeof(OxBuilder_Option_t));
	hErr = OAPI_BUILDER_GetOption(ulBuilderId, &stOption);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_GetOption failed.. \n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstInfo, &stOption.stSvcSrchOpt, sizeof(DxBuilder_Option_t));

	return ERR_OK;
}

HERROR	APK_BUILDER_SetOption(HUINT32 hBuilderHandle, DxBuilder_Option_t *pstInfo)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32				ulBuilderId = (HUINT32)hBuilderHandle;
	OxBuilder_Option_t 	stOption;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	if (pstInfo == NULL)
	{
		HxLOG_Error("Invalid parameter \n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stOption, 0x00, sizeof(OxBuilder_Option_t));
	hErr = OAPI_BUILDER_GetOption(ulBuilderId, &stOption);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_GetOption failed.. \n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy(&stOption.stSvcSrchOpt, pstInfo, sizeof(DxBuilder_Option_t));

	HxLOG_Print(HxANSI_COLOR_YELLOW(
		"\n\t stOption.eDeliveryType                = %d"
		"\n\t stOption.eOptionType                  = %d"
		"\n\t stOption.stSvcSrchOpt.bNetworkSearch  = %d"
		"\n\t stOption.stSvcSrchOpt.bWithNetworkId  = %d"
		"\n\t stOption.stSvcSrchOpt.eCasType        = %d"
		"\n\t stOption.stSvcSrchOpt.eSvcType        = %d"
		"\n\t stOption.stSvcSrchOpt.ulNetworkId     = %d\n"
		)
		, stOption.eDeliveryType
		, stOption.eOptionType
		, stOption.stSvcSrchOpt.bNetworkSearch
		, stOption.stSvcSrchOpt.bWithNetworkId
		, stOption.stSvcSrchOpt.eCasType
		, stOption.stSvcSrchOpt.eSvcType
		, stOption.stSvcSrchOpt.ulNetworkId
		);


	// SET search option
	hErr = OAPI_BUILDER_SetOption(ulBuilderId, &stOption);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_SetOption Error.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_GetBuilderStatus(HUINT32 hBuilderHandle,
															HUINT32 *pulTotalTuningNum,
															HUINT32 *pulCompleteTuningNum,
															HUINT32 *pulSearchedTuningNum,
															HUINT32	*pulSearchState,
															HUINT32 *pulFrequency,
															DxDeliveryType_e *peDeliveryType,
															DxTuneParam_t *pstTuningInfo)
{
	HERROR						hErr = ERR_FAIL;
	HUINT32						ulBuilderId = (HUINT32)hBuilderHandle;
	OxBuilder_BuilderState_t 	stBuilderStatus;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	HxSTD_MemSet(&stBuilderStatus, 0x00, sizeof(OxBuilder_BuilderState_t));
	hErr = OAPI_BUILDER_GetBuilderStatus(ulBuilderId, &stBuilderStatus);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_GetBuilderStatus failed.. \n");
		return ERR_FAIL;
	}

	if (pulTotalTuningNum != NULL)
	{
		*pulTotalTuningNum = stBuilderStatus.ulTotalTuningNum;
	}

	if (pulCompleteTuningNum != NULL)
	{
		*pulCompleteTuningNum = stBuilderStatus.ulCompleteTuningNum;
	}

	if (pulSearchedTuningNum != NULL)
	{
		*pulSearchedTuningNum = stBuilderStatus.ulSearchedSvcNum;
	}

	if (pulSearchState != NULL)
	{
		// 현재 일본향에서만 사용중이라 일단 제외
		*pulSearchState = 0;
	}

	if (pulFrequency != NULL)
	{
		*pulFrequency = stBuilderStatus.ulFrequency;
	}

	if (peDeliveryType != NULL)
	{
		*peDeliveryType = stBuilderStatus.eDeliveryType;
	}

	if (pstTuningInfo != NULL)
	{
		HxSTD_MemCopy(pstTuningInfo, &stBuilderStatus.stTuneParam, sizeof(DxTuneParam_t));
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_GetBuilderSignalStatus(HUINT32 hBuilderHandle, HINT32 *pnSignalStrength, HINT32 *pnSignalQuality)
{
	HERROR						hErr = ERR_FAIL;
	HUINT32						ulBuilderId = (HUINT32)hBuilderHandle;
	OxBuilder_BuilderState_t 	stBuilderStatus;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	HxSTD_MemSet(&stBuilderStatus, 0x00, sizeof(OxBuilder_BuilderState_t));
	hErr = OAPI_BUILDER_GetBuilderStatus(ulBuilderId, &stBuilderStatus);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("APK_BUILDER_GetBuilderStatus() failed.. \n");
		return ERR_FAIL;
	}

	if (pnSignalStrength != NULL)
	{
		*pnSignalStrength = (HINT32)stBuilderStatus.ulSignalStrength;
	}

	if (pnSignalQuality != NULL)
	{
		*pnSignalQuality = (HINT32)stBuilderStatus.ulSignalQuality;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_GetBuilderSvcInfo (HUINT32 hBuilderHandle, HINT32 lStartIndex, HINT32 lCount, HINT32 *plResultNo, DxBuilder_SearchedInfo_t **ppstSvcInfoList)
{
	HERROR				 			 hErr;
	HUINT32 						 ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	if (NULL == plResultNo)				{ return ERR_FAIL; }
	if (NULL == ppstSvcInfoList)		{ return ERR_FAIL; }

	*plResultNo = 0;
	*ppstSvcInfoList = NULL;

	hErr = OAPI_BUILDER_GetBuilderSvcInfo(ulBuilderId, lStartIndex, lCount, plResultNo, ppstSvcInfoList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_GetBuilderSvcInfo Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_FreeBuilderSvcInfo (DxBuilder_SearchedInfo_t *pstSvcInfoList)
{
	HxLOG_Info("\n");

	if (pstSvcInfoList == NULL)
	{
		HxLOG_Error ("Invalid parameter - NULL pointer \n");
		return ERR_FAIL;
	}

	return OAPI_BUILDER_FreeBuilderSvcInfo(pstSvcInfoList);
}

const HCHAR* APK_BUILDER_GetScanType(HUINT32 hBuilderHandle)
{
	HERROR				 hErr = ERR_FAIL;
	HUINT32 			 ulBuilderId = (HUINT32)hBuilderHandle;
	OxBuilder_Option_t 	 stOption;
	HCHAR				*szScanType = apkBuilder_ScanType_NormalScan;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	HxSTD_MemSet(&stOption, 0x00, sizeof(OxBuilder_Option_t));

	hErr = OAPI_BUILDER_GetOption(ulBuilderId, &stOption);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_GetOption failed.. \n");
		return szScanType;
	}

	szScanType = apk_builder_GetScanTypeStr(stOption.eOptionType);
	HxLOG_Debug("szScanType(%s) \n", szScanType);

	return szScanType;
}

HERROR	APK_BUILDER_StartScan (HUINT32 hBuilderHandle)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	hErr = OAPI_BUILDER_Start(ulBuilderId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_Start failed.. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_StopScan (HUINT32 hBuilderHandle)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	hErr = OAPI_BUILDER_Stop(ulBuilderId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_Stop failed.. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_PauseScan (HUINT32 hBuilderHandle)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	hErr = OAPI_BUILDER_Pause(ulBuilderId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_Pause failed.. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_ResumeScan (HUINT32 hBuilderHandle)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	hErr = OAPI_BUILDER_Resume(ulBuilderId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_Resume failed.. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_FinishScan (HUINT32 hBuilderHandle)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("ulBuilderId(%d)\n", ulBuilderId);

	hErr = OAPI_BUILDER_Finish(ulBuilderId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_Finish failed.. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR	APK_BUILDER_SetAdditionalNumbers(HUINT32 hBuilderHandle)
{
	// ARIB ONLY !
	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);
	HxLOG_Error("Not support yet ! \n");
	return ERR_OK;
}

HERROR APK_BUILDER_MotorGotoPosition (HUINT32 hBuilderHandle, HINT32 lPosition)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorGotoPosition (ulBuilderId, lPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorGotoPosition Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_MotorStorePosition (HUINT32 hBuilderHandle, HINT32 lPosition)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorStorePosition (ulBuilderId, lPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorStorePosition Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_MotorResetPosition (HUINT32 hBuilderHandle)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorResetPosition (ulBuilderId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorResetPosition Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_MotorRecalculate (HUINT32 hBuilderHandle, HINT32 lPosition)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorRecalculate (ulBuilderId, lPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorRecalculate Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_MotorMove (HUINT32 hBuilderHandle, DxSat_MOTOR_DIRECTION_e eDirection, DxSat_MOTOR_DRIVEMODE_e eMode, HINT32 step)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorMove(ulBuilderId, eDirection, eMode, step);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_MotorMove failed.. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_MotorStop (HUINT32 hBuilderHandle)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorStop(ulBuilderId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_MotorStop failed.. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_MotorSetLimit (HUINT32 hBuilderHandle, DxSat_MOTOR_DIRECTION_e eDirection)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorSetLimit(ulBuilderId, eDirection);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_BUILDER_MotorSetLimit failed.. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_MotorEnableLimit (HUINT32 hBuilderHandle)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorEnableLimit (ulBuilderId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorEnableLimit Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_MotorDisableLimit (HUINT32 hBuilderHandle)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorDisableLimit (ulBuilderId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorDisableLimit Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_MotorGotoDegree (HUINT32 hBuilderHandle, HINT32 lDegree)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorGotoDegree (ulBuilderId, lDegree);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorGotoDegree Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_BUILDER_MotorGotoSatellite (HUINT32 hBuilderHandle, HINT32 lSatellite)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_MotorGotoSatellite (ulBuilderId, lSatellite);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorGotoSatellite Err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	APK_BUILDER_MotorSetMyLongitude (HINT32 lNewLongitude)
{
	HERROR						 hErr;

	HxLOG_Info("lNewLongitude(%d)\n", lNewLongitude);

	hErr = OAPI_BUILDER_MotorSetMyLongitude (lNewLongitude);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorSetMyLongitude Err:\n");
	}

	return hErr;
}

HERROR	APK_BUILDER_MotorSetMyLatitude (HINT32 lNewLatitude)
{
	HERROR						 hErr;

	HxLOG_Info("lNewLatitude(%d)\n", lNewLatitude);

	hErr = OAPI_BUILDER_MotorSetMyLatitude (lNewLatitude);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorSetMyLatitude Err:\n");
	}

	return hErr;
}

HERROR	APK_BUILDER_MotorGetMyLongitude (HINT32 *plMyLongitude)
{
	HERROR						 hErr;

	HxLOG_Info("\n");

	hErr = OAPI_BUILDER_MotorGetMyLongitude (plMyLongitude);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorGetMyLongitude Err:\n");
	}

	return hErr;
}

HERROR	APK_BUILDER_MotorGetMyLatitude (HINT32 *plMyLatitude)
{
	HERROR						 hErr;

	HxLOG_Info("\n");

	hErr = OAPI_BUILDER_MotorGetMyLatitude (plMyLatitude);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_MotorGetMyLatitude Err:\n");
	}

	return hErr;
}

HERROR	APK_BUILDER_SetSelectedConflictLcn(HUINT32 hBuilderHandle, DxBuilder_SearchedInfo_t *selectedInfos, int count)
{
	HERROR		hErr;
	HUINT32 	ulBuilderId = (HUINT32)hBuilderHandle;

	HxLOG_Info("hBuilderHandle(0x%x)\n", hBuilderHandle);

	hErr = OAPI_BUILDER_SetSelectedConflictLcnSvcInfo(ulBuilderId, selectedInfos, count);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OAPI_BUILDER_SetSelectedConflictLcnSvcInfo  Err:\n");
	}

	return hErr;
}

/* end of file */
