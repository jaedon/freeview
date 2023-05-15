/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_chanscan.c
	@brief

	Description:  									\n
	Module: node/binding/datatype_convert		\n

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
#include <rest_bind_api.h>
#include <rest_datatype.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
// Converting Types:
STATIC RestEnumStringTable_t		s_stRestDataType_ScanType =
{
	(HCHAR*)DxNAMEOF(RestScanType_e),
		
	{
		{	"none", 			eREST_SCAN_TYPE_None			},
		{	"channel-scan", 	eREST_SCAN_TYPE_ChannelScan 	},
		{	"auto-antenna", 	eREST_SCAN_TYPE_AutoAntenna 	},
		{	"auto-connection",	eREST_SCAN_TYPE_AutoConnection	},
		{	NULL,				0								}
	}
};

STATIC RestEnumStringTable_t		s_stRestDataType_ScanCmd =
{
	(HCHAR*)DxNAMEOF(RestScanCmd_e),
		
	{
		{	"start", 		eREST_SCAN_CMD_Start 	},
		{	"stop", 		eREST_SCAN_CMD_Stop		},
		{	"pause",		eREST_SCAN_CMD_Pause	},
		{	"resume",		eREST_SCAN_CMD_Resume	},
		{	"save",			eREST_SCAN_CMD_Save		},
		{	NULL,				0					}
	}
};

STATIC RestEnumStringTable_t		s_stRestDataType_ScanState =
{
	(HCHAR*)DxNAMEOF(RestScanState_e),
		
	{
		{	"idle", 		eREST_SCAN_STATE_Idle 		},
		{	"ready", 		eREST_SCAN_STATE_Ready		},
		{	"starting",		eREST_SCAN_STATE_Starting	},
		{	"running",		eREST_SCAN_STATE_Running	},
		{	"paused", 		eREST_SCAN_STATE_Paused		},
		{	"finished",		eREST_SCAN_STATE_Finished	},
		{	NULL,				0						}
	}
};

STATIC RestEnumStringTable_t		s_stRestDataType_ScanChanSend =
{
	(HCHAR*)DxNAMEOF(RestScanChanSend_e),
		
	{
		{	"none", 		eREST_SCAN_CHANSEND_None 		},
		{	"simple", 		eREST_SCAN_CHANSEND_Simple		},
//		{	"detailed",		eREST_SCAN_CHANSEND_Detailed	},
		{	NULL,				0							}
	}
};

#define ___LOCAL_FUNCTIONS___
STATIC HBOOL rest_datatype_CbCmdTuningParam (void *pvUserData, void *pvListData)
{
	DxTuneParam_t		*pstUserData = (DxTuneParam_t *)pvUserData;
	DxTuneParam_t		*pstListData = (DxTuneParam_t *)pvListData;

	return rest_datatype_CheckTuningParamSame(pstUserData, pstListData);
	
}

#define ___MEMBER_FUNCTIONS___
HINT32 rest_datatype_JsonizeScanParams (HINT32 nJsonWriter, RestScanParams_t *punScanStatus, HCHAR *szSelect)
{
	if (NULL == punScanStatus)
	{
		HxLOG_Error("punScanStatus NULL\n");
		return nJsonWriter;
	}

	switch (punScanStatus->stBase.eScanType)
	{
	case eREST_SCAN_TYPE_ChannelScan:
		return rest_datatype_JsonizeScanParams_ChannelScan(nJsonWriter, &(punScanStatus->stChanScan), szSelect);

	case eREST_SCAN_TYPE_None:
		return rest_datatype_JsonizeScanParams_None(nJsonWriter, punScanStatus, szSelect);
		
	default:
		break;
	}

	return nJsonWriter;
}

HERROR rest_datatype_DejsonizeScanParams (HxJSON_t jsObject, RestScanParams_t *punScanStatus, HBOOL bClearObj)
{
	RestScanType_e			 eScanType = eREST_SCAN_TYPE_None;
	HERROR					 hErr = ERR_FAIL;

	if (NULL == punScanStatus)
	{
		HxLOG_Error("punScanStatus NULL\n");
		return ERR_FAIL;
	}

	if (NULL == jsObject)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	(void)rest_datatype_GetJsonEnum(jsObject, "scanType", DxNAMEOF(RestScanType_e), (HINT32 *)&eScanType);

	switch (eScanType)
	{
	case eREST_SCAN_TYPE_ChannelScan:
		hErr = rest_datatype_DejsonizeScanParams_ChannelScan(jsObject, &(punScanStatus->stChanScan), bClearObj);
		break;
		
	default:
		HxLOG_Error("ScanType(%d) not supported:\n", eScanType);
		return ERR_FAIL;
	}

	return hErr;
}

HINT32 rest_datatype_JsonizeScanParams_None (HINT32 nJsonWriter, RestScanParams_t *punScanStatus, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == punScanStatus)
	{
		HxLOG_Error("pstParams NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		RestScanBaseParam_t		*pstBaseParam = &(punScanStatus->stBase);
		
		(void)rest_datatype_SetJsonInteger(nWriter, "scannerUid", (HINT32)pstBaseParam->ulScannerUid, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "scanType", DxNAMEOF(RestScanType_e), (HINT32)pstBaseParam->eScanType, szSelect);
		//(void)rest_datatype_SetJsonEnum(nWriter, "scanCmd", DxNAMEOF(RestScanCmd_e), (HINT32)pstParams->eCmd, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "scanState", DxNAMEOF(RestScanState_e), (HINT32)pstBaseParam->eState, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}

HINT32 rest_datatype_JsonizeScanParams_ChannelScan (HINT32 nJsonWriter, RestScanChanParams_t *pstParams, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstParams)
	{
		HxLOG_Error("pstParams NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "scannerUid", (HINT32)pstParams->ulScannerUid, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "scanType", DxNAMEOF(RestScanType_e), (HINT32)pstParams->eScanType, szSelect);
		//(void)rest_datatype_SetJsonEnum(nWriter, "scanCmd", DxNAMEOF(RestScanCmd_e), (HINT32)pstParams->eCmd, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "scanState", DxNAMEOF(RestScanState_e), (HINT32)pstParams->eState, szSelect);

		(void)rest_datatype_SetJsonInteger(nWriter, "networkScan", pstParams->nFlagNetworkScan, szSelect);
		if (pstParams->nNetworkId > 0)
		{
			(void)rest_datatype_SetJsonInteger(nWriter, "networkId", pstParams->nNetworkId, szSelect);
		}
		(void)rest_datatype_SetJsonEnum(nWriter, "channelType", DxNAMEOF(DxSvcType_e), (HINT32)pstParams->eSvcType, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "casType", DxNAMEOF(DxCasType_e), (HINT32)pstParams->eCasType, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "replaceChannel", (HINT32)pstParams->nReplaceChannel, szSelect);

		if ((NULL != pstParams->pstTpList) && (rest_datatype_CheckWriteSelected("scanTps", szSelect)))
		{
			HxList_t			*pstListItem;

			HxJSONWRITER_Array(nWriter, "scanTps");
			for (pstListItem = pstParams->pstTpList; NULL != pstListItem; pstListItem = pstListItem->next)
			{
				DxTuneParam_t			*pstTuneParam = (DxTuneParam_t *)HLIB_LIST_Data(pstListItem);

				if (NULL != pstTuneParam)
				{
					(void)rest_datatype_JsonizeTuningParam(nWriter, pstTuneParam, "~antennaParam");					
				}
			}
			HxJSONWRITER_ArrayEnd(nWriter);
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}

HERROR rest_datatype_DejsonizeScanParams_ChannelScan (HxJSON_t jsObject, RestScanChanParams_t *pstParams, HBOOL bClearObj)
{
	HINT32					 nCnt, nArrayNum;
//	RestScanType_e			 eScanType = eREST_SCAN_TYPE_None;
	HxJSON_t				 jsAddTp, jsDeleteTp, jsTp;
	HxList_t				*pstItemFound;
	DxTuneParam_t			*pstTuneParam = NULL;
//	HERROR					 hErr;

	if (NULL == pstParams)
	{
		HxLOG_Error("pstParams NULL\n");
		return ERR_FAIL;
	}

	if (NULL == jsObject)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		pstParams->pstTpList = HLIB_LIST_RemoveAllFunc(pstParams->pstTpList, HLIB_STD_MemFree_CB);
		HxSTD_MemSet(pstParams, 0, sizeof(RestScanChanParams_t));
		
		pstParams->eCmd = eREST_SCAN_CMD_None;
		pstParams->nNetworkId = -1;
	}

	(void)rest_datatype_GetJsonEnum(jsObject, "scanType", DxNAMEOF(RestScanType_e), (HINT32 *)&(pstParams->eScanType));
	(void)rest_datatype_GetJsonEnum(jsObject, "scanCmd", DxNAMEOF(RestScanCmd_e), (HINT32 *)&(pstParams->eCmd));

	(void)rest_datatype_GetJsonInteger(jsObject, "networkScan", (HINT32 *)&(pstParams->nFlagNetworkScan));
	(void)rest_datatype_GetJsonInteger(jsObject, "networkId", (HINT32 *)&(pstParams->nNetworkId));

	(void)rest_datatype_GetJsonEnum(jsObject, "channelType", DxNAMEOF(DxSvcType_e), (HINT32 *)&(pstParams->eSvcType));
	(void)rest_datatype_GetJsonEnum(jsObject, "casType", DxNAMEOF(DxCasType_e), (HINT32 *)&(pstParams->eCasType));
	(void)rest_datatype_GetJsonInteger(jsObject, "replaceChannel", (HINT32 *)&(pstParams->nReplaceChannel));

	jsAddTp = HLIB_JSON_Object_GetArray(jsObject, "scanTps");
	if (NULL != jsAddTp)
	{
		nArrayNum = HLIB_JSON_GetSize(jsAddTp);
		if (nArrayNum > 0)
		{
			for (nCnt = 0; nCnt < nArrayNum; nCnt++)
			{
				if (NULL == pstTuneParam)
				{
					pstTuneParam = (DxTuneParam_t *)HLIB_STD_MemAlloc(sizeof(DxTuneParam_t));
				}

				jsTp = HxJSON_Array_Get(jsAddTp, nCnt);
				if ((NULL != jsTp) && (NULL != pstTuneParam))
				{
					(void)rest_datatype_DejsonizeTuningParam(jsTp, pstTuneParam, TRUE);

					if (0 == pstParams->eDeliType)
					{
						pstParams->eDeliType = pstTuneParam->eDeliType;
					}
					else if (pstTuneParam->eDeliType != pstParams->eDeliType)
					{
						continue;
					}

					if (eDxDELIVERY_TYPE_SAT == pstTuneParam->eDeliType)
					{
						DxAntInfo_t 			*pstAntInfo = APK_META_ANTENNA_GetInfo(pstTuneParam->sat.antInfo.uid);
						
						if (NULL != pstAntInfo)
						{
							HxSTD_MemCopy(&(pstTuneParam->sat.antInfo), pstAntInfo, sizeof(DxAntInfo_t));
							HLIB_STD_MemFree(pstAntInfo);
						}
						else
						{
							continue;
						}
					}

					// If the same tuning parameter does not exist in the list, add it.
					pstItemFound = HLIB_LIST_FindEx(pstParams->pstTpList, (const void *)pstTuneParam, rest_datatype_CbCmdTuningParam);
					if (NULL == pstItemFound)
					{
						pstParams->pstTpList = HLIB_LIST_Append(pstParams->pstTpList, (void *)pstTuneParam);
						pstTuneParam = NULL;
					}
				}
			}
		}
	}

	jsDeleteTp = HLIB_JSON_Object_GetArray(jsObject, "removeScanTps");
	if (NULL != jsAddTp)
	{
		nArrayNum = HLIB_JSON_GetSize(jsAddTp);
		if (nArrayNum > 0)
		{
			for (nCnt = 0; nCnt < nArrayNum; nCnt++)
			{
				if (NULL == pstTuneParam)
				{
					pstTuneParam = (DxTuneParam_t *)HLIB_STD_MemAlloc(sizeof(DxTuneParam_t));
				}

				jsTp = HxJSON_Array_Get(jsAddTp, nCnt);
				if ((NULL != jsTp) && (NULL != pstTuneParam))
				{
					(void)rest_datatype_DejsonizeTuningParam(jsTp, pstTuneParam, TRUE);

					if (eDxDELIVERY_TYPE_SAT == pstTuneParam->eDeliType)
					{
						DxAntInfo_t 			*pstAntInfo = APK_META_ANTENNA_GetInfo(pstTuneParam->sat.antInfo.uid);
						
						if (NULL != pstAntInfo)
						{
							HxSTD_MemCopy(&(pstTuneParam->sat.antInfo), pstAntInfo, sizeof(DxAntInfo_t));
							HLIB_STD_MemFree(pstAntInfo);
						}
						else
						{
							continue;
						}
					}

					// If the same tuning parameter does not exist in the list, add it.
					pstItemFound = HLIB_LIST_FindEx(pstParams->pstTpList, (const void *)pstTuneParam, rest_datatype_CbCmdTuningParam);
					if (NULL != pstItemFound)
					{
						DxTuneParam_t		*pstToRemove = (DxTuneParam_t *)HLIB_LIST_Data(pstItemFound);

						if (NULL != pstToRemove)
						{
							pstParams->pstTpList = HLIB_LIST_Remove(pstParams->pstTpList, (const void *)pstToRemove);
							HLIB_STD_MemFree(pstToRemove);
						}
					}
				}
			}
		}
	}

	if (NULL != pstTuneParam)
	{
		HLIB_STD_MemFree(pstTuneParam);
	}

	return ERR_OK;
}

HINT32 rest_datatype_JsonizeScanResult (HINT32 nJsonWriter, RestScanResult_t *punScanResult, HCHAR *szSelect)
{
	if (NULL == punScanResult)
	{
		HxLOG_Error("punScanResult NULL\n");
		return nJsonWriter;
	}

	switch (punScanResult->stBase.eScanType)
	{
	case eREST_SCAN_TYPE_ChannelScan:
		return rest_datatype_JsonizeScanResult_ChannelScan(nJsonWriter, &(punScanResult->stChanScan), szSelect);
		
	default:
		break;
	}

	return nJsonWriter;
}

HINT32 rest_datatype_JsonizeScanResult_ChannelScan (HINT32 nJsonWriter, RestScanChanResult_t *pstResult, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstResult)
	{
		HxLOG_Error("pstResult NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "totalTpNumbers", pstResult->ulTotalTuningNum, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "finishedTpNumbers", pstResult->ulCompleteTuningNum, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "scannedChannerNumbers", pstResult->ulSearchedChannelNum, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "scannedTvNumbers", pstResult->ulSearchedTvNum, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "scannedRadioNumbers", pstResult->ulSearchedRadioNum, szSelect);

		(void)rest_datatype_SetJsonInteger(nWriter, "signalStrength", pstResult->ulCurrSignalStrength, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "signalQuality", pstResult->ulCurrSignalQuality, szSelect);

		if ((rest_datatype_CheckWriteSelected("currentTuningParam", szSelect)) && (NULL != pstResult->pstCurrTuneParam))
		{
			HxJSONWRITER_StringValue(nWriter, "currentTuningParam");
			{
				(void)rest_datatype_JsonizeTuningParam(nWriter, pstResult->pstCurrTuneParam, "~antennaParam");
			}
		}

		if (rest_datatype_CheckWriteSelected("scannedChannels", szSelect))
		{
			HBOOL		 bWriteChannel = FALSE;
			HCHAR		*szChanSelect = NULL;
			
			switch (pstResult->eChannelSendType)
			{
			case eREST_SCAN_CHANSEND_Simple:
				bWriteChannel = TRUE;
				szChanSelect = "channelNumber,name,channelNumber,casType";
				break;

			default:
				break;
			}

			if ((TRUE == bWriteChannel) && (0 != pstResult->ulListSvcNum))
			{
				(void)rest_datatype_SetJsonEnum(nWriter, "channelType", DxNAMEOF(DxSvcType_e), (HINT32)pstResult->eListSvcType, NULL);
				HxJSONWRITER_Array(nWriter, "scannedChannels");
				{
					HUINT32			 ulCnt = 0;
					HxList_t		*pstListItem = pstResult->pstSvcListHead;
					DxService_t		*pstSvc;

					for (; (NULL != pstListItem) && (ulCnt < pstResult->ulListSvcNum); ulCnt++, pstListItem = pstListItem->next)
					{
						pstSvc = (DxService_t *)HLIB_LIST_Data(pstListItem);

						if (NULL != pstSvc)
						{
							(void)rest_datatype_JsonizeService(nJsonWriter, pstSvc, NULL, szChanSelect);
						}
					}
				}
				HxJSONWRITER_ArrayEnd(nWriter);
			}
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}

#define _____INITIALIZATION_____
HERROR rest_datatype_InitScan (void)
{
	// Initialize the enumeration
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_ScanType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_ScanCmd);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_ScanState);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_ScanChanSend);
	
	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___

