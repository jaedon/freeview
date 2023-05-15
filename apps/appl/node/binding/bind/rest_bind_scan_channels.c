/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_scan_channels.c
	@brief

	Description:  									\n
	Module: node/binding/datatype		\n

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
#include <dlib.h>
#include <apk.h>
#include <rest_maintask.h>
#include <rest_datatype.h>
#include <rest_bind.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct _rest_bind_svcFreeLink_
{
	struct _rest_bind_svcFreeLink_	*pstNext;
	HUINT32			 ulReserved;
} RestBindScan_SvcFreeLink_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___
STATIC HERROR rest_bind_scan_chan_PresetState (RestInstance_t *pstInst, RestScanChanParams_t *pstParam)
{
	switch (pstParam->eState)
	{
	case eREST_SCAN_STATE_None:
	case eREST_SCAN_STATE_Idle:
	case eREST_SCAN_STATE_Ready:
		if ((NULL != pstParam->pstTpList) && (0 != pstParam->eDeliType))
		{
			pstParam->eState = eREST_SCAN_STATE_Ready;
		}
		else
		{
			pstParam->eState = eREST_SCAN_STATE_Idle;
		}

		break;

	default:
		break;
	}

	return ERR_OK;
}

STATIC HERROR rest_bind_scan_chan_StartScan (RestInstance_t *pstInst, RestScanChanParams_t *pstParam)
{
	HUINT32				 hBuilderHandle = 0;
	HxList_t			*pstListItem;
	DxBuilder_Option_t	 stOption;
	HERROR				 hErr;

	hErr = APK_BUILDER_OpenSession(pstParam->eDeliType, "normal", NULL, &hBuilderHandle);
	if ((ERR_OK != hErr) || (0 == hBuilderHandle))
	{
		HxLOG_Error("APK_BUILDER_OpenSession err: eDeliType(0x%08x)\n", pstParam->eDeliType);
		return ERR_FAIL;
	}

	(void)APK_BUILDER_ClearBuildInfo(hBuilderHandle);
	for (pstListItem = pstParam->pstTpList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxTuneParam_t			*pstTuneParam = (DxTuneParam_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstTuneParam)
		{
			(void)APK_BUILDER_AddBuildInfo(hBuilderHandle, pstTuneParam);
		}
	}

	stOption.eSvcType = pstParam->eSvcType;
	stOption.eCasType = pstParam->eCasType;
	stOption.bNetworkSearch = (pstParam->nFlagNetworkScan) ? TRUE : FALSE;
	stOption.bReplaceChannel = (pstParam->nReplaceChannel) ? TRUE : FALSE;
	if (pstParam->nNetworkId > 0)
	{
		stOption.bWithNetworkId = FALSE;
		stOption.ulNetworkId = 0;
	}
	else
	{
		stOption.bWithNetworkId = TRUE;
		stOption.ulNetworkId = (HUINT32)pstParam->nNetworkId;
	}

	(void)APK_BUILDER_SetOption(hBuilderHandle, &stOption);

	hErr = APK_BUILDER_StartScan(hBuilderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_BUILDER_StartScan err: hBuilderHandle(0x%08x)\n", hBuilderHandle);
		return ERR_FAIL;
	}

	pstParam->hBuilderHandle = hBuilderHandle;
	pstParam->eState = eREST_SCAN_STATE_Running;

	return ERR_OK;
}

STATIC HERROR rest_bind_scan_chan_StopScan (RestInstance_t *pstInst, RestScanChanParams_t *pstParam)
{
	HERROR				 hErr;

	hErr = APK_BUILDER_StopScan(pstParam->hBuilderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_BUILDER_StopScan err: hBuilderHandle(0x%08x)\n", pstParam->hBuilderHandle);
	}

	hErr = APK_BUILDER_ClearBuildInfo(pstParam->hBuilderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_BUILDER_StopScan err: hBuilderHandle(0x%08x)\n", pstParam->hBuilderHandle);
	}

	hErr = APK_BUILDER_CloseSession(pstParam->hBuilderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_BUILDER_StopScan err: hBuilderHandle(0x%08x)\n", pstParam->hBuilderHandle);
	}

	// Clear the parameters:
	if (NULL != pstParam->pstTpList)
	{
		pstParam->pstTpList = HLIB_LIST_RemoveAllFunc(pstParam->pstTpList, HLIB_STD_MemFree_CB);
	}

	HxSTD_MemSet(pstParam, 0, sizeof(RestScanChanParams_t));
	return ERR_OK;
}

STATIC HCHAR *rest_bind_scan_chan_CmdStart (RestInstance_t *pstInst, RestScanChanParams_t *pstParam)
{
	HINT32			 nJWriter;
	HERROR			 hErr;
	
	if (eREST_SCAN_STATE_Ready != pstParam->eState)
	{
		HxLOG_Error(">>> CMD Start: State(%d) not Ready!\n", pstParam->eState);
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOTREADY, REST_ERR_STRING_NOTREADY, TRUE);
	}
	
	hErr = rest_bind_scan_chan_StartScan(pstInst, pstParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_bind_scan_chan_StartScan err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_CMD_FAILED, REST_ERR_STRING_CMD_FAILED, TRUE);
	}

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		HxJSONWRITER_StringValue(nJWriter, "scannerStatus");
		(void)rest_datatype_JsonizeScanParams_ChannelScan(nJWriter, pstParam, "scannerUid,scanType,scanState");
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	return rest_bind_CloseJsonWriter(nJWriter);
}

STATIC HCHAR *rest_bind_scan_chan_CmdStop (RestInstance_t *pstInst, RestScanChanParams_t *pstParam)
{
	HINT32			 nJWriter;
	HERROR			 hErr;
	
	if ((eREST_SCAN_STATE_Idle == pstParam->eState) || (eREST_SCAN_STATE_Ready == pstParam->eState))
	{
		HxLOG_Error(">>> CMD Stop: State(%d) is running state\n", pstParam->eState);
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOTREADY, REST_ERR_STRING_NOTREADY, TRUE);
	}
	
	hErr = rest_bind_scan_chan_StopScan(pstInst, pstParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_bind_scan_chan_StopScan err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_CMD_FAILED, REST_ERR_STRING_CMD_FAILED, TRUE);
	}

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		HxJSONWRITER_StringValue(nJWriter, "scannerStatus");
		(void)rest_datatype_JsonizeScanParams_ChannelScan(nJWriter, pstParam, "scannerUid,scanType,scanState");
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	return rest_bind_CloseJsonWriter(nJWriter);
}

STATIC DxService_t *rest_bind_scan_chan_AllocService (RestScanChanParams_t *pstParam)
{
	if (NULL != pstParam)
	{
		if (NULL == pstParam->pvFreeSvcObject)
		{
			HUINT32				 ulCnt;
			DxService_t			*pstSvcObjBlock = (DxService_t *)HLIB_STD_MemCalloc(sizeof(DxService_t) * 512);

			if (NULL != pstSvcObjBlock)
			{
				for (ulCnt = 0; ulCnt < 512; ulCnt++)
				{
					DxService_t					*pstSvc = &(pstSvcObjBlock[ulCnt]);
					RestBindScan_SvcFreeLink_t	*pstSvcLink = (RestBindScan_SvcFreeLink_t *)pstSvc;

					pstSvcLink->pstNext = (RestBindScan_SvcFreeLink_t *)pstParam->pvFreeSvcObject;
					pstParam->pvFreeSvcObject = (void *)pstSvcLink;
				}

				pstParam->pstAllocedBufferList = HLIB_LIST_Prepend(pstParam->pstAllocedBufferList, (void *)pstSvcObjBlock);
			}
		}

		if (NULL != pstParam->pvFreeSvcObject)
		{
			RestBindScan_SvcFreeLink_t 	*pstSvcLink = (RestBindScan_SvcFreeLink_t *)pstParam->pvFreeSvcObject;
			DxService_t 				*pstSvc = (DxService_t *)pstParam->pvFreeSvcObject;

			pstParam->pvFreeSvcObject = (void *)pstSvcLink->pstNext;
			HxSTD_MemSet(pstSvcLink, 0, sizeof(RestBindScan_SvcFreeLink_t));
			pstSvc->typeOf = DxNAMEOF(DxService_t);

			return pstSvc;
		}
	}

	return NULL;	
}

STATIC INLINE HERROR rest_bind_scan_chan_AppendList (HxList_t **ppstListHead, HxList_t **ppstListTail, void *pvData)
{
	HxList_t		*pstListHead = *ppstListHead;
	HxList_t		*pstListTail = *ppstListTail;

	if ((NULL == pstListHead) || (NULL == pstListTail))
	{
		pstListHead = HLIB_LIST_Append(pstListHead, pvData);
		pstListTail = pstListHead;
	}
	else
	{
		pstListHead = HLIB_LIST_Append(pstListTail, pvData);
		pstListTail = pstListTail->next;
	}

	*ppstListHead = pstListHead;
	*ppstListTail = pstListTail;

	return ERR_OK;	
}

STATIC HERROR rest_bind_scan_chan_LoadUnreadScannedChannels (RestScanChanParams_t *pstParam)
{
	HINT32						 nCnt, nResultNo = 0;
	DxBuilder_SearchedInfo_t	*pstBuildSrchList = NULL;
	HERROR						 hErr;
	
	
	hErr = APK_BUILDER_GetBuilderSvcInfo(pstParam->hBuilderHandle, (HINT32)pstParam->ulChannelNum, (HINT32)100000,
										&nResultNo, &pstBuildSrchList);
	if (ERR_OK == hErr)
	{
		for (nCnt = 0; nCnt < nResultNo; nCnt++)
		{
			DxService_t 		*pstNewSvc = rest_bind_scan_chan_AllocService(pstParam);
	
			if (NULL != pstNewSvc)
			{
				pstNewSvc->svcType	= (DxSvcType_e)pstBuildSrchList[nCnt].ulSvcType;
				pstNewSvc->casType	= (DxCasType_e)pstBuildSrchList[nCnt].ulCasType;
				pstNewSvc->lcn		= (HINT32)pstBuildSrchList[nCnt].ulLcn;
				HxSTD_StrNCpy(pstNewSvc->name, (HCHAR *)pstBuildSrchList[nCnt].szSvcName, DxBUILDER_SVC_NAME_LEN);
				pstNewSvc->name[DxBUILDER_SVC_NAME_LEN] = '\0';

				(void)rest_bind_scan_chan_AppendList(&(pstParam->pstAllSvcList), &(pstParam->pstLastSvcAll), (void *)pstNewSvc);
				pstParam->ulChannelNum ++;

				switch (pstNewSvc->svcType)
				{
				case eDxSVC_TYPE_TV:
					(void)rest_bind_scan_chan_AppendList(&(pstParam->pstTvSvcList), &(pstParam->pstLastSvcTv), (void *)pstNewSvc);
					pstParam->ulTvNum ++;
					break;
	
				case eDxSVC_TYPE_RADIO:
					(void)rest_bind_scan_chan_AppendList(&(pstParam->pstRadioSvcList), &(pstParam->pstLastSvcRadio), (void *)pstNewSvc);
					pstParam->ulRadioNum ++;
					break;
	
				default:
					break;
				}
			}						
		}
	}
	
	if (NULL != pstBuildSrchList)			{ (void)APK_BUILDER_FreeBuilderSvcInfo(pstBuildSrchList); }
	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HCHAR *rest_bind_scan_chan_CmdGet (RestInstance_t *pstInst, RestScanChanParams_t *pstParam, HxJSON_t jsArgs)
{
	HINT32					 nJWriter;
	HINT32					 nSvcStartIdx = -1, nSvcCnt = -1;
	DxSvcType_e				 eSvcType = eDxSVC_TYPE_All;
	RestScanChanSend_e		 eChanSend = eREST_SCAN_CHANSEND_None;
	HERROR					 hErr;
	
	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	if (NULL != jsArgs)
	{
		(void)rest_datatype_GetJsonEnum(jsArgs, "channelList", (HCHAR *)DxNAMEOF(RestScanChanSend_e), (HINT32 *)&eChanSend);
		(void)rest_datatype_GetJsonEnum(jsArgs, "channelType", (HCHAR *)DxNAMEOF(DxSvcType_e), (HINT32 *)&eSvcType);
		(void)rest_datatype_GetJsonInteger(jsArgs, "channelStartIdx", &nSvcStartIdx);
		(void)rest_datatype_GetJsonInteger(jsArgs, "channelCount", &nSvcCnt);
	}

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// scannerStatus
		HxJSONWRITER_StringValue(nJWriter, "scannerStatus");
		(void)rest_datatype_JsonizeScanParams_ChannelScan(nJWriter, pstParam, "~scanTps");

		// scanResult
		if ((eREST_SCAN_STATE_Running == pstParam->eState) ||
			(eREST_SCAN_STATE_Paused == pstParam->eState) ||
			(eREST_SCAN_STATE_Finished == pstParam->eState))
		{
			HUINT32 					 ulSearchState, ulFrequency;
			DxDeliveryType_e			 eDeliType;
			RestScanChanResult_t		 stResult;
			DxTuneParam_t				 stTuneParam;

			HxSTD_MemSet(&stResult, 0, sizeof(RestScanChanResult_t));

			stResult.ulScannerUid = pstParam->ulScannerUid;
			stResult.eScanType = pstParam->eScanType;
			stResult.eChannelSendType = eChanSend;
			stResult.eListSvcType = eSvcType;

			hErr = APK_BUILDER_GetBuilderStatus(pstParam->hBuilderHandle, &(stResult.ulTotalTuningNum), &(stResult.ulCompleteTuningNum),
												&(stResult.ulSearchedChannelNum), &ulSearchState, &ulFrequency,
												&eDeliType, &stTuneParam);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("APK_BUILDER_GetBuilderStatus err: hBuilder(0x%08x)\n", pstParam->hBuilderHandle);
			}

			stResult.pstCurrTuneParam = &stTuneParam;

			hErr = APK_BUILDER_GetBuilderSignalStatus(pstParam->hBuilderHandle, (HINT32 *)&(stResult.ulCurrSignalStrength), (HINT32 *)&(stResult.ulCurrSignalQuality));
			if (ERR_OK != hErr)
			{
				HxLOG_Error("APK_BUILDER_GetBuilderSignalStatus err: hBuilder(0x%08x)\n", pstParam->hBuilderHandle);
			}

			// Update the temporary service lists if the searched services are increased:
			if (stResult.ulSearchedChannelNum > pstParam->ulChannelNum)
			{
				(void)rest_bind_scan_chan_LoadUnreadScannedChannels(pstParam);
			}
			
			stResult.ulSearchedTvNum = pstParam->ulTvNum;
			stResult.ulSearchedRadioNum = pstParam->ulRadioNum;
			
			if ((eREST_SCAN_CHANSEND_Simple == eChanSend) || (eREST_SCAN_CHANSEND_Detailed == eChanSend))
			{
				HINT32			 nCurrSvcNum;
				HxList_t		*pstSvcList;

				switch (eSvcType)
				{
				case eDxSVC_TYPE_TV:
					nCurrSvcNum = (HINT32)pstParam->ulTvNum;
					pstSvcList = pstParam->pstTvSvcList;
					break;

				case eDxSVC_TYPE_RADIO:
					nCurrSvcNum = (HINT32)pstParam->ulRadioNum;
					pstSvcList = pstParam->pstRadioSvcList;
					break;

				case eDxSVC_TYPE_All:
				default:
					nCurrSvcNum = (HINT32)pstParam->ulChannelNum;
					pstSvcList = pstParam->pstAllSvcList;
					break;
				}
				
				if ((nSvcCnt <= 0) || (nSvcCnt > 200))
				{
					nSvcCnt = 20;
				}

				if ((nSvcStartIdx < 0) || ((nSvcStartIdx + nSvcCnt) >= nCurrSvcNum))
				{
					nSvcStartIdx = nCurrSvcNum - nSvcCnt;
				}

				stResult.ulListSvcNum = (HUINT32)nSvcCnt;
				stResult.pstSvcListHead = HLIB_LIST_GetListItem(pstSvcList, nSvcStartIdx);
			}

			HxJSONWRITER_StringValue(nJWriter, "scanResult");
			(void)rest_datatype_JsonizeScanResult_ChannelScan(nJWriter, &stResult, NULL);
		}

		// error:
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	return rest_bind_CloseJsonWriter(nJWriter);
}

HCHAR *rest_bind_scan_chan_CmdPut (RestInstance_t *pstInst, RestScanChanParams_t *pstParam, RestScanCmd_e eScanCmd)
{
	HCHAR					*szResult = NULL;

	(void)rest_bind_scan_chan_PresetState(pstInst, pstParam);

	switch (eScanCmd)
	{
	case eREST_SCAN_CMD_Start:
		return rest_bind_scan_chan_CmdStart(pstInst, pstParam);

	case eREST_SCAN_CMD_Stop:
		return rest_bind_scan_chan_CmdStop(pstInst, pstParam);

	default:
		HxLOG_Error("eScanCmd(%d) not supported:\n", eScanCmd);
		break;
	}

	return szResult;
}



