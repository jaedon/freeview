/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_ota_astra.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /OTACHECK	\n

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
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>
#include <pal_pipe.h>
#include <pal_sirawpool.h>
#include <pal_sys.h>
#include <pal_param.h>

#include <db_svc.h>
#include <svc_si.h>
#include <xsvc_si.h>
#include <svc_swup.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_swup.h>

#include <_xsvc_si_lib_swup_base.h>
#include <_xsvc_si_swup_base.h>
#include <_xsvc_si_swup.h>
#include <db_param.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if 0
typedef struct
{
	svcSi_SwupCheck_Base_t		 stBase;

	HUINT16					usPmtSvcId, usPmtPid;
	HUINT16					usBatBouquetId;

	HBOOL					bFound;
	SvcSi_SwupSignal_t		stOtaInfo;

}XsvcSi_OtaData_Astra_t;
#endif

#if defined(CONFIG_DEBUG)
STATIC const HINT8 * xsvc_si_SwupCheckMakeStrMode_Astra(XsvcSi_SwupMode_e eSwupMode)
{
	switch (eSwupMode)
	{
		ENUM_TO_STR(eSI_SWUP_MODE_ASTRA_HOME_TP);	// Home TP 일 경우
		ENUM_TO_STR(eSI_SWUP_MODE_ASTRA_REFNIT_TP);	// Reference NIT TP
		ENUM_TO_STR(eSI_SWUP_MODE_ASTRA_SSUBAT_TP);	// SSUBAT TP 일 경우
		ENUM_TO_STR(eSI_SWUP_MODE_ASTRA_SSUSVC_TP);	// SSUSVC TP 일 경우
		default:
			break;
	}
	return ("Unknown");

}
#endif

#define _____STATIC_FUNCTIONS_____

STATIC HERROR xsvc_si_SwupCheckInitSwupData_Astra (xsvcSi_SwupData_t *pstSwupCheck)
{
	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

	pstSwupCheck->usBatBouquetId 	= SSU_BOUQUET_ID;

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%d] ulSwupMode(%s) usBatBouquetId(0x%x) \n"
		,__FUNCTION__,__LINE__, xsvc_si_SwupCheckMakeStrMode_Astra(pstSwupCheck->stBase.stSwupOption.ulSwupMode), pstSwupCheck->usBatBouquetId );
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_ASTRA_SSUSVC_TP:
		pstSwupCheck->usPmtSvcId = pstSwupCheck->stBase.stSwupOption.usExtId1;
		break;

	default:
		break;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckGetFilteringTypes_Astra(xsvcSi_SwupData_t *pstSwupCheck)
{
	if (NULL == pstSwupCheck)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%d] ulSwupMode(%s) \n" ,__FUNCTION__,__LINE__, xsvc_si_SwupCheckMakeStrMode_Astra(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_ASTRA_HOME_TP:
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_NIT_ACT;
		break;

	case eSI_SWUP_MODE_ASTRA_REFNIT_TP:
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_NIT_ACT;
		break;

	case eSI_SWUP_MODE_ASTRA_SSUBAT_TP:
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_BAT;
		break;

	case eSI_SWUP_MODE_ASTRA_SSUSVC_TP:
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_PAT;
		break;

	default:
		pstSwupCheck->stBase.eFiltering = 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_Astra(xsvcSi_SwupData_t *pstSwupCheck, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
{
	if (NULL == pstSwupCheck)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

	switch (eTableType)
	{
	case eSIxTABLETYPE_Pat:
		*peFilterType		= bSI_FILTERING_PAT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= PAT_PID;
		break;

	case eSIxTABLETYPE_NitAct:
		*peFilterType		= bSI_FILTERING_NIT_ACT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= NIT_PID;
		break;

	case eSIxTABLETYPE_Bat:
		*peFilterType	= bSI_FILTERING_BAT;
		*pulExtId		= (HUINT32)pstSwupCheck->usBatBouquetId;
		*pulPid			= BAT_PID;
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckStartSection_Astra(xsvcSi_SwupData_t *pstSwupCheck)
{
	HUINT32					 ulExtId, ulPid;
	SIxTable_Type_e			 eTableType;
	svcSi_FilteringType_b	 eFilterType;
	HERROR					 hErr;

	if (NULL == pstSwupCheck)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

	for (eTableType = 0; eTableType < eSIxTABLETYPE_MAX; eTableType++)
	{
		hErr = xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_Astra(pstSwupCheck, eTableType, &eFilterType, &ulExtId, &ulPid);
		if ((ERR_OK == hErr) && (pstSwupCheck->stBase.eFiltering & eFilterType))
		{
			hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eTableType, ulExtId, ulPid);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("svc_si_SrchStartFilter err: eTableType(%d)\n", eTableType);
			}
		}
	}

	if (NULL == pstSwupCheck->stBase.pstFilterings)
	{
		HxLOG_Error ("No filter has started: All Failed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckStopSection_Astra(xsvcSi_SwupData_t *pstSwupCheck, SIxTable_Type_e eSiTableType, HUINT32 ulExtId)
{
	HERROR			hErr = ERR_FAIL;

	if (NULL == pstSwupCheck)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

	hErr = svc_si_SwupCheckStopSection(&pstSwupCheck->stBase, eSiTableType, ulExtId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_si_SwupCheckStopSection() failed..! eSiTableType(%d) extId(0x%X) \n", eSiTableType, ulExtId);
	}

	return hErr;
}

STATIC void	xsvc_si_SwupCheckFreeListItem_Astra (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxSI_Free (pvItem);
	}
}

#define _____OTA_PARSING_____

STATIC HERROR xsvc_si_SwupCheckParseSelectorBytes_Astra(HUINT8 *pucRaw, SvcSi_SwupSignal_t *pstSelector)
{
	if (pucRaw == NULL || pstSelector == NULL)
	{
		HxLOG_Error("Invalid Parameters\n");
		return ERR_FAIL;
	}

	pstSelector->ulStartSystemId = HxMACRO_Get32Bit (pucRaw);			pucRaw += 4;		// 0~3 : Start Product ID
	pstSelector->usHdfVer = HxMACRO_Get16Bit (pucRaw);					pucRaw += 2;		// 4~5 : HDF Version
	pstSelector->usDataType = HxMACRO_Get16Bit (pucRaw);				pucRaw += 2;		// 6~7 : Data Type
	svc_si_SwupCheckConvertSwupSiDataTypeToVersionType((SvcSi_SwupImageType_e)pstSelector->usDataType, &(pstSelector->eDataType));
	pstSelector->ulDataVersion = HxMACRO_Get32Bit (pucRaw);				pucRaw += 4;		// 8~11 : Data Version
	pstSelector->ulEndSystemId = HxMACRO_Get32Bit (pucRaw);				pucRaw += 4;		// 12~15 : End Product ID
	if (*pucRaw > 0)
	{
		pstSelector->ucSwupType = pucRaw[1];
	}
	else
	{
		pstSelector->ucSwupType = SvcSi_SWUP_TYPE_USER_SELECT;
	}
	pstSelector->ucUsage = 0;

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckFindProperOuiData_Astra(HUINT32 ulSelectorNum, svcSi_SwupCheck_SsuInfo_t *pstSelectors, SvcSi_SwupSignal_t *pstSwupInfo)
{
	HBOOL			 bFound;
	HUINT32			 ulIndex;
	HUINT32			 ulSystemId, ulSystemVer;
	HUINT8			*pucRaw;
	SvcSi_SwupSignal_t	 	stSwupInfo;
	HERROR			 hErr;

	if (ulSelectorNum == 0 || pstSelectors == NULL || pstSelectors == NULL)
	{
		HxLOG_Error("invalid Parameters \n");
		return ERR_FAIL;
	}

	bFound = FALSE;
	if (PalParam_Activated())
	{
		hErr = PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_ID, (void *)&ulSystemId, sizeof(HUINT32));
	}
	else
	{
		hErr = PAL_SYS_GetSystemId (&ulSystemId);
	}
	if (hErr != ERR_OK)
	{
		HxLOG_Error("PAL_SYS_GetSystemId  Error\n");
		return ERR_FAIL;
	}

	for (ulIndex = 0; ulIndex < ulSelectorNum; ulIndex++)
	{
		if (pstSelectors[ulIndex].ulOui == eSWUP_OUI_HUMAX)
		{
			if (pstSelectors[ulIndex].ulSelectorLen < 16)
			{
				continue;
			}

			pucRaw = pstSelectors[ulIndex].pucSelectSelectorPnt;
			HxSTD_memset(&stSwupInfo, 0, sizeof(SvcSi_SwupSignal_t));
			hErr = xsvc_si_SwupCheckParseSelectorBytes_Astra (pucRaw, &stSwupInfo);
			if (hErr == ERR_OK)
			{
				if (stSwupInfo.ulEndSystemId != 0xFFFFFFFF)
				{
					if (PalParam_Activated())
					{
						PAL_PARAM_Item_e	eItem = xsvc_si_SwupCheckCvtVersionTypeToPalParam(stSwupInfo.eDataType);

						if (eItem == ePAL_PARAM_MAX)	{ continue; }
						hErr = PAL_PARAM_GetField(eItem, (void *)&ulSystemId, sizeof(HUINT32));
					}
					else
					{
						hErr = PAL_SYS_GetSystemVersion(stSwupInfo.eDataType, &ulSystemVer);
					}
					if (hErr != ERR_OK) 			{ continue; }

					if (stSwupInfo.ulStartSystemId <= ulSystemId &&
						stSwupInfo.ulEndSystemId >= ulSystemId &&
						stSwupInfo.ulDataVersion > ulSystemVer)
					{
						bFound = TRUE;

						if (pstSwupInfo != NULL)
						{
							*pstSwupInfo = stSwupInfo;
						}

						break;
					}
				}
			}
		}
	}

	return (bFound == TRUE) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR xsvc_si_SwupCheckGetSwupSignalFromDataBroadcastIdDes_Astra (SIxDataBroadcastIdDes_t *pstDbcIdDes, SvcSi_SwupSignal_t *pstSwupSignal)
{
	HERROR				 		hErr;
	HUINT32					 	ulSelectorNum;
	svcSi_SwupCheck_SsuInfo_t	*pstOuiSelectors;

	hErr = svc_si_SwupCheckGetSsuInfoFromDataBroadcastIdDes (pstDbcIdDes, &ulSelectorNum, &pstOuiSelectors);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_si_SwupCheckGetSsuInfoFromDataBroadcastIdDes Error \n");
		return ERR_FAIL;
	}

	hErr = xsvc_si_SwupCheckFindProperOuiData_Astra(ulSelectorNum, pstOuiSelectors, pstSwupSignal);

	svc_si_SwupCheckFreeSsuInfo (pstOuiSelectors);

	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckFindSsuInfoFromPmt_Astra (xsvcSi_SwupData_t *pstSwupCheck, HUINT32 *pulNumSsu, SvcSi_SwupSignal_t **pastSwupSignals)
{
	HERROR					 hErr = ERR_FAIL;
	HUINT32 				 ulActionId, ulNumSsuSvc = 0, ulCntSsuSvc, i;
	SIxTable_PsiTable_t 	*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t				*pstPmt2ndLoop = NULL, *pstPmtDesLoop = NULL;
	HxList_t				*pstSwupInfoList = NULL, *pstList = NULL;
	SvcSi_SwupSignal_t		*pstSwupInfo= NULL, *astSwupInfoArray = NULL;
	SvcSi_SwupSignal_t		 stSwupInfo;

	// PMT parsing해서
	ulActionId = PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction);
	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", ulActionId);
		return ERR_FAIL;
	}

	for (pstCurrPmt = pstPmtTable; pstCurrPmt != NULL; pstCurrPmt = pstCurrPmt->pstNextTable)
	{
		SIxTable_PmtSec_t		*pstPmtSec = &pstCurrPmt->unInfo.stPmt;

		for (pstPmt2ndLoop = pstCurrPmt->pst2ndLoop; NULL != pstPmt2ndLoop; pstPmt2ndLoop = pstPmt2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstPmt2ndLoop);
			SIxTable_PmtElement_t		*pstPmtElem;

			if (NULL == pst2ndItem) 				{ continue; }

			pstPmtElem = (SIxTable_PmtElement_t*)&(pst2ndItem->unInfo.stPmt);
			if(pstPmtElem  == NULL) {continue;}

			hErr = svc_si_SwupCheckIsSsuStreamType(pstPmtElem->ucStreamType);
			if(hErr == ERR_OK)
			{
				for (pstPmtDesLoop = pst2ndItem->pstDesLoop; NULL != pstPmtDesLoop; pstPmtDesLoop = pstPmtDesLoop->next)
				{
					HUINT8					*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstPmtDesLoop);
					SIxDataBroadcastIdDes_t 	*pstDbcIdDes = NULL;

					if (eSIxDESCTAG_DATA_BROADCAST_ID == *pucRawDes)
					{
						pstDbcIdDes = (SIxDataBroadcastIdDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
						if (pstDbcIdDes != NULL)
						{
							hErr = xsvc_si_SwupCheckGetSwupSignalFromDataBroadcastIdDes_Astra(pstDbcIdDes, &stSwupInfo);
							if (hErr == ERR_OK)
							{
								//HxLOG_Print("[%s:%d]	PMT : Data Broadcast ID found : SvcId (%04x), PID(%04x)\n", stPmtProgInfo.usProgNum, stPmtElemInfo.usElemPid);

								pstSwupInfo = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t));
								if (pstSwupInfo != NULL)
								{
									stSwupInfo.usSvcId	= pstPmtSec->usProgNum;
									stSwupInfo.usPid	= pstPmtElem->usElemPid;
									HxSTD_memcpy(pstSwupInfo, &stSwupInfo, sizeof(SvcSi_SwupSignal_t));

									pstSwupInfoList = HLIB_LIST_Append (pstSwupInfoList, (void *)pstSwupInfo);
								}
							}
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDbcIdDes);
						}
					}
					else
					{
						HxLOG_Error("	Descriptor is not eSIxDESCTAG_DATA_BROADCAST_ID: DescTag(%04x)\n", *pucRawDes);
					}
				}
			}
			else
			{
				HxLOG_Error("	Stream Type is not SSU Type: ucStreamType(%04x)\n", pstPmtElem->ucStreamType);
			}
		}
	}

	ulNumSsuSvc = HLIB_LIST_Length(pstSwupInfoList);
	if (ulNumSsuSvc == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	astSwupInfoArray = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t) * ulNumSsuSvc);
	if (astSwupInfoArray == NULL)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	ulCntSsuSvc = 0;

	for (i = 0; i < ulNumSsuSvc; i++)
	{
		pstList = HLIB_LIST_GetListItem(pstSwupInfoList, i);
		if (pstList == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", ulNumSsuSvc, i);
			continue;
		}

		pstSwupInfo = (SvcSi_SwupSignal_t *)HLIB_LIST_Data(pstList);
		if (pstSwupInfo == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", i);
			continue;
		}

		HxSTD_MemCopy(&astSwupInfoArray[i], pstSwupInfo, sizeof(SvcSi_SwupSignal_t));
		ulCntSsuSvc++;
	}

	if (ulCntSsuSvc == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pulNumSsu != NULL)
	{
		*pulNumSsu = ulCntSsuSvc;
	}

	if (pastSwupSignals != NULL)
	{
		*pastSwupSignals = astSwupInfoArray;
		astSwupInfoArray = NULL;
	}

	hErr = ERR_OK;

Error:
	if (astSwupInfoArray != NULL)			{ OxSI_Free (astSwupInfoArray); }
	if (pstPmtTable != NULL)				{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }
	if (pstSwupInfoList != NULL)			{ HLIB_LIST_RemoveAllFunc(pstSwupInfoList, xsvc_si_SwupCheckFreeListItem_Astra); }

	return hErr;

}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_SwupCheckMsgCmdStartSwup_Astra(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	(void)svc_si_SwupCheckInitData(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stStartSwupCheck.unSpec), (void*)&(pstMsg->unArg.stStartSwupCheck.stOption), FALSE);
	(void)xsvc_si_SwupCheckInitSwupData_Astra(pstSwupCheck);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)xsvc_si_SwupCheckGetFilteringTypes_Astra(pstSwupCheck);
	(void)xsvc_si_SwupCheckStartSection_Astra (pstSwupCheck);
}

STATIC void xsvc_si_SwupCheckMsgCmdStopSwup_Astra(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

	(void)xsvc_si_SwupCheckStopSection_Astra(pstSwupCheck, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPatTable_Astra(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR		hErr;
	HUINT32		ulActionId;
	HUINT16		usPmtPid;

	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_Astra(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_ASTRA_SSUSVC_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			ulActionId = PAL_PIPE_GetActionId (pstSwupCheck->stBase.hAction);

			hErr = svc_si_LibGetPmtPidFromPat(ulActionId, pstSwupCheck->usPmtSvcId,  &usPmtPid);
			if(hErr != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			pstSwupCheck->usPmtPid = usPmtPid;

			hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Pmt, (HUINT32)pstSwupCheck->usPmtSvcId, (HUINT32)pstSwupCheck->usPmtPid);
			if(hErr != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_Astra(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPmtTable_Astra(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_Astra(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_ASTRA_SSUSVC_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);

			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_Astra(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtNitTable_Astra(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR	hErr;

	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_Astra(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_ASTRA_HOME_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_REF_NIT, NULL, NULL);
			if (hErr != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);

			break;

		case eSI_SWUP_MODE_ASTRA_REFNIT_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			// SSU-SCAN Linkage정보를 찾아야 한다.
			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_SSU_SCAN, NULL, NULL);
			if (hErr != ERR_OK)
			{
				// SSU-SCAN Linkage 정보는 없지만 SSU-SVC 정보가 직접 있는가?
				hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_ALL, NULL, NULL);
				if (hErr != ERR_OK)
				{
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return;
				}
			}

			svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);

			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_Astra(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			break;
	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtBatTable_Astra(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR	hErr;

	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_Astra(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_ASTRA_SSUBAT_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			// SSU-SVC Linkage 정보를 찾는다.
			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_ALL, NULL, NULL);
			if (hErr != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			// BAT 에 필요한 정보가 있다.
			svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
			break;
		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_Astra(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			break;
	}
	return;
}

STATIC void xsvc_si_SwupCheckProcessMsg_Astra (SvcSi_Msg_t *pstMsg, void *pvData)
{
	xsvcSi_SwupData_t	*pstSwupCheck = (xsvcSi_SwupData_t *)pvData;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_OTACHECK:
		HxLOG_Print("[%s:%d]eSIMSG_CMD_START_OTACHECK!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgCmdStartSwup_Astra(pstSwupCheck, pstMsg);
		break;

	case eSIMSG_CMD_STOP_OTACHECK:
		HxLOG_Print("[%s:%d]eSIMSG_CMD_STOP_OTACHECK!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgCmdStopSwup_Astra(pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_PAT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtPatTable_Astra (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_PMT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtPmtTable_Astra (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
	case eSIMSG_EVT_NIT_OTH_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_NIT_TABLE! (%s)\n",__FUNCTION__,__LINE__, pstMsg->eMsgClass == eSIMSG_EVT_NIT_TABLE? "NIT_ACT" : "NIT_OTH");
		xsvc_si_SwupCheckMsgEvtNitTable_Astra (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_BAT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_BAT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtBatTable_Astra (pstSwupCheck, pstMsg);
		break;

	default:
		HxLOG_Debug("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define _____OTA_INFO_TO_API_____

STATIC HERROR xsvc_si_SwupCheckGetDataDeliveryTsInfo_Astra(Handle_t hAction, HUINT32 *pulNumTs, void **ppvTsInfoArray)
{
	HUINT32 			ulActionId;
	HERROR			hErr = ERR_FAIL;
	HERROR			hResult = ERR_FAIL;
	HUINT32 			ulNitActTsNum = 0, ulNitOthTsNum = 0;
	DbSvc_TsInfo_t	*pstNitActTsInfo = NULL;
	DbSvc_TsInfo_t	*pstNitOthTsInfo = NULL;
	DbSvc_TsInfo_t	*pstNitTsInfo = NULL;


	if ((pulNumTs == NULL) || (ppvTsInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumTs : %p, ppvTsInfoArray : %p)\n",pulNumTs, ppvTsInfoArray );
		return ERR_FAIL;
	}

	*pulNumTs = 0;
	*ppvTsInfoArray = NULL;

	ulActionId = PAL_PIPE_GetActionId (hAction);
	hErr = svc_si_LibGetAllDeliveryTsInfoFromNit(ulActionId, TRUE, TRUE, &ulNitActTsNum, &pstNitActTsInfo);
	if ((hErr != ERR_OK) || (ulNitActTsNum == 0))
	{
		HxLOG_Print("[%s:%d] svc_si_LibGetAllDeliveryTsInfoFromNit() fail or ulNitActTsNum == 0\n", __FUNCTION__, __LINE__);
	}

	hErr = svc_si_LibGetAllDeliveryTsInfoFromNit(ulActionId, FALSE, TRUE, &ulNitOthTsNum, &pstNitOthTsInfo);
	if ((hErr != ERR_OK) || (ulNitOthTsNum == 0))
	{
		HxLOG_Print("[%s:%d] svc_si_LibGetAllDeliveryTsInfoFromNit() fail or ulNitOthTsNum == 0\n", __FUNCTION__, __LINE__);
	}

	if (ulNitActTsNum + ulNitOthTsNum == 0)
	{
		HxLOG_Print("[%s:%d] no ts delivery infomation \n", __FUNCTION__, __LINE__);
		goto Exit_Func;
	}

	pstNitTsInfo = (DbSvc_TsInfo_t *)OxSI_Malloc (sizeof(DbSvc_TsInfo_t) * (ulNitActTsNum + ulNitOthTsNum));
	if (NULL == pstNitTsInfo)
	{
		HxLOG_Error ("Memory allocation failed\n");
		goto Exit_Func;
	}

	if (pstNitActTsInfo > 0)
	{
		HxSTD_MemCopy(pstNitTsInfo, pstNitActTsInfo, sizeof(DbSvc_TsInfo_t) * ulNitActTsNum);
	}

	if (ulNitOthTsNum > 0)
	{
		HxSTD_MemCopy(&pstNitTsInfo[ulNitActTsNum], pstNitOthTsInfo, sizeof(DbSvc_TsInfo_t) * ulNitOthTsNum);
	}

	*pulNumTs = ulNitActTsNum + ulNitOthTsNum;
	*ppvTsInfoArray = pstNitTsInfo;
	hResult = ERR_OK;

Exit_Func:
	if (pstNitActTsInfo != NULL)			{ OxSI_Free (pstNitActTsInfo); }
	if (pstNitOthTsInfo != NULL)			{ OxSI_Free (pstNitOthTsInfo); }

	return hResult;
}

// NIT를 통해 얻은 Reference NIT Linkage 정보 (REF-NIT를 가리키는 Triple ID List)
static HERROR xsvc_si_SwupCheckGetRefNitTripleId_Astra (Handle_t hAction, HUINT32 *pulNumSsuScan, void **ppvTripleIdArray)
{
	HERROR		 			hErr;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_t	*pstSwupCheck = (xsvcSi_SwupData_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumSsuScan == NULL) || (ppvTripleIdArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumSsuScan : %p, ppvTripleIdInfoArray : %p)\n",pulNumSsuScan, ppvTripleIdArray );
		return ERR_FAIL;
	}

	*pulNumSsuScan = 0;
	*ppvTripleIdArray = NULL;

	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_REF_NIT, pulNumSsuScan, (DbSvc_TripleId_t **)ppvTripleIdArray);

	return hErr;
}

// NIT를 통해 얻은 SSU SCAN Linkage 정보 (SSU-BAT를 가리키는 Triple ID List)
STATIC HERROR xsvc_si_SwupCheckGetSsuScanTripleId_Astra (Handle_t hAction, HUINT32 *pulNumSsuScan, void **ppvTripleIdArray)
{
	HERROR		 			hErr;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_t		*pstSwupCheck = (xsvcSi_SwupData_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumSsuScan == NULL) || (ppvTripleIdArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumSsuLink : %p, ppvTripleIdInfoArray : %p)\n",pulNumSsuScan, ppvTripleIdArray );
		return ERR_FAIL;
	}

	*pulNumSsuScan = 0;
	*ppvTripleIdArray = NULL;

	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_SSU_SCAN, pulNumSsuScan, (DbSvc_TripleId_t **)ppvTripleIdArray);

	return hErr;
}

// BAT를 통해 얻은 SSU Linkage 정보 (SSU-SVC를 가리키는 Triple ID List)
STATIC HERROR xsvc_si_SwupCheckGetSsuLinkTripleId_Astra(Handle_t hAction,  HUINT32 *pulNumSsu, void **ppvTripleIdArray)
{
	HERROR		 			hErr;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_t		*pstSwupCheck = (xsvcSi_SwupData_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumSsu == NULL) || (ppvTripleIdArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumSsu : %p, ppvTripleIdInfoArray : %p)\n",pulNumSsu, ppvTripleIdArray );
		return ERR_FAIL;
	}

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_ASTRA_HOME_TP:
		hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_ALL, pulNumSsu, (DbSvc_TripleId_t **)ppvTripleIdArray);
		break;

	case eSI_SWUP_MODE_ASTRA_SSUBAT_TP:
		hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_ALL, pulNumSsu, (DbSvc_TripleId_t **)ppvTripleIdArray);
		break;
	default:
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

// PMT를 통해 얻은 OTA Signalling Information
STATIC HERROR xsvc_si_SwupCheckGetSwupSignalInfo_Astra(Handle_t hAction, HUINT32 *pulNumInfo, void **pavSwupInfoArray)
{
	HERROR						 hErr;
	HUINT32						 ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_t	*pstSwupCheck = (xsvcSi_SwupData_t *)svc_si_GetSiData(ulActionId);

	hErr = xsvc_si_SwupCheckFindSsuInfoFromPmt_Astra(pstSwupCheck, pulNumInfo, (SvcSi_SwupSignal_t **)pavSwupInfoArray);

	return hErr;
}

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SwupCheckGetActionFunc_Astra (void)
{
	return xsvc_si_SwupCheckProcessMsg_Astra;
}

void *xsvc_si_SwupCheckAllocData_Astra (void)
{
	xsvcSi_SwupData_t		*pstSwupCheck;

	pstSwupCheck = (xsvcSi_SwupData_t *)OxSI_Calloc(sizeof(xsvcSi_SwupData_t));
	if (NULL != pstSwupCheck)
	{
		pstSwupCheck->stBase.eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return (void *)pstSwupCheck;
}

void xsvc_si_SwupCheckFreeData_Astra (void *pvData)
{
	xsvcSi_SwupData_t		*pstSwupCheck = (xsvcSi_SwupData_t *)pvData;

	if (NULL != pstSwupCheck)
	{
		OxSI_Free(pstSwupCheck);
		pstSwupCheck = NULL;
	}

	return;
}

HERROR xsvc_si_SwupCheckGetSwupCheckData_Astra(Handle_t hAction, HUINT32 ulDataType, HUINT32 *pulDataNum, void **pavDataArray)
{
	HERROR						hErr = ERR_FAIL;
	XsvcSi_SwupDataType_e 		eDataType = (XsvcSi_SwupDataType_e)ulDataType;

	if (pulDataNum == NULL || pavDataArray == NULL)
	{
		HxLOG_Error ("invalide parameter\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%d] ulSwupMode(%s) \n",__FUNCTION__,__LINE__, xsvc_si_SwupCheckMakeStrMode_Astra(eDataType) );
#endif

	switch (eDataType)
	{
	case eSI_SWUP_DATA_DELIVERY_TSINFO:
		hErr = xsvc_si_SwupCheckGetDataDeliveryTsInfo_Astra(hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_REFNIT_TRIPLEID:
		hErr = xsvc_si_SwupCheckGetRefNitTripleId_Astra(hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_SSU_SCAN_TRIPLEID:
		hErr = xsvc_si_SwupCheckGetSsuScanTripleId_Astra(hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_SSU_LINK_TRIPLEID:
		hErr = xsvc_si_SwupCheckGetSsuLinkTripleId_Astra(hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_SSU_SIGNAL:
		hErr = xsvc_si_SwupCheckGetSwupSignalInfo_Astra(hAction, pulDataNum, pavDataArray);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	return hErr;

}

HERROR xsvc_si_SwupCheckFreeSwupCheckData_Astra (Handle_t hAction, HUINT32 ulDataType, void *pvDataArray)
{
	(void)hAction;
	(void)ulDataType;

	if (pvDataArray != NULL)
	{
		OxSI_Free (pvDataArray);
		pvDataArray = NULL;
	}

	return ERR_OK;
}

HERROR xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Astra (SIxLinkDes_t *pstLinkDes)
{
	return svc_si_SwupCheckFindProperOuiDataFromLinkDes(pstLinkDes);
}



/* end of file */

