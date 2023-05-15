/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_ota_ses.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /SWUP CHECK	\n

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
#include <linkedlist.h>
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

#include <_xsvc_si_lcnupdate_ses.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	svcSi_SwupCheck_Base_t		stBase;
	HBOOL						bUpdateTime;

	HUINT16						usPmtSvcId;
	HUINT16						usPmtPid;

	HUINT16						usBatBouquetId;
	HUINT16						usNetIdForNitOth;

	HUINT16						usCurrOnId;
	HUINT16						usCurrTsId;

	HBOOL						 bFound;
	SvcSi_SwupSignal_t	 		stSwupInfo;
	SIxDeferAssocTagDes_t		stAssociationInfo;
} xsvcSi_SwupData_Ses_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#if defined(CONFIG_DEBUG)
STATIC const HINT8 * xsvc_si_SwupCheckMakeStrMode_Ses(XsvcSi_SwupMode_e eSwupMode)
{
	switch (eSwupMode)
	{
		ENUM_TO_STR(eSI_SWUP_MODE_SES_HOME_TP);
		ENUM_TO_STR(eSI_SWUP_MODE_SES_REFNIT_TP);
		ENUM_TO_STR(eSI_SWUP_MODE_SES_SSUBAT_TP);
		ENUM_TO_STR(eSI_SWUP_MODE_SES_SSUSVC_TP);
		default:
			break;
	}
	return ("Unknown");

}
#endif

STATIC HERROR xsvc_si_SwupCheckInitData_Ses (xsvcSi_SwupData_Ses_t *pstSwupCheck)
{
	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

	pstSwupCheck->usBatBouquetId 	= SSU_BOUQUET_ID;
	pstSwupCheck->bUpdateTime = FALSE;

	pstSwupCheck->bFound = FALSE;
	HxSTD_MemSet(&pstSwupCheck->stSwupInfo, 0x00, sizeof(SvcSi_SwupSignal_t));
	HxSTD_MemSet(&pstSwupCheck->stAssociationInfo, 0x00, sizeof(SIxDeferAssocTagDes_t));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_SES_REFNIT_TP:
		pstSwupCheck->usCurrOnId = pstSwupCheck->stBase.stSwupOption.usExtId1;
		pstSwupCheck->usCurrTsId = pstSwupCheck->stBase.stSwupOption.usExtId2;
		break;
	case eSI_SWUP_MODE_SES_SSUSVC_TP:
	case eSI_SWUP_MODE_SES_BURST_TP:
		pstSwupCheck->usPmtSvcId = pstSwupCheck->stBase.stSwupOption.usExtId1;
		break;
	default:
		break;
	}

#if defined(CONFIG_DEBUG)
		HxLOG_Print("[%s:%d] ulSwupMode(%d) usBatBouquetId(0x%x) \n"
			,__FUNCTION__,__LINE__, xsvc_si_SwupCheckMakeStrMode_Ses(pstSwupCheck->stBase.stSwupOption.ulSwupMode), pstSwupCheck->usBatBouquetId );
#endif

	return ERR_OK;
}

HERROR xsvc_si_SwupCheckGetFilteringTypes_Ses(xsvcSi_SwupData_Ses_t *pstSwupCheck)
{
	if (NULL == pstSwupCheck)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%d] ulSwupMode(%s) \n" ,__FUNCTION__,__LINE__, xsvc_si_SwupCheckMakeStrMode_Ses(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_SES_HOME_TP:
			pstSwupCheck->stBase.eFiltering = bSI_FILTERING_PAT|bSI_FILTERING_NIT_ACT | bSI_FILTERING_BAT | bSI_FILTERING_TDT;

			if (pstSwupCheck->usNetIdForNitOth)
			{
				pstSwupCheck->stBase.eFiltering |= bSI_FILTERING_NIT_OTH;
			}
			break;

		case eSI_SWUP_MODE_SES_REFNIT_TP:
			pstSwupCheck->stBase.eFiltering = bSI_FILTERING_NIT_ACT | bSI_FILTERING_BAT | bSI_FILTERING_TDT;

			if (pstSwupCheck->usNetIdForNitOth)
			{
				pstSwupCheck->stBase.eFiltering |= bSI_FILTERING_NIT_OTH;
			}
			break;

		case eSI_SWUP_MODE_SES_SSUBAT_TP:
			pstSwupCheck->stBase.eFiltering = bSI_FILTERING_BAT;
			break;

		case eSI_SWUP_MODE_SES_SSUSVC_TP:
		case eSI_SWUP_MODE_SES_BURST_TP:
			pstSwupCheck->stBase.eFiltering = bSI_FILTERING_PAT;
			break;

		default:
			pstSwupCheck->stBase.eFiltering = 0;
			return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_Ses (xsvcSi_SwupData_Ses_t *pstSwupData, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
{
	switch (eTableType)
	{
	case eSIxTABLETYPE_Pat:
		*peFilterType	= bSI_FILTERING_PAT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= PAT_PID;
		break;

	case eSIxTABLETYPE_NitAct:
		*peFilterType	= bSI_FILTERING_NIT_ACT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= NIT_PID;
		break;

	case eSIxTABLETYPE_NitOth:
		*peFilterType	= bSI_FILTERING_NIT_OTH;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= NIT_PID;
		break;

	case eSIxTABLETYPE_Bat:
		*peFilterType	= bSI_FILTERING_BAT;
		*pulExtId		= (HUINT32)pstSwupData->usBatBouquetId;
		*pulPid			= BAT_PID;
		break;

	case eSIxTABLETYPE_Tdt:
		*peFilterType	= bSI_FILTERING_TDT;
		*pulExtId		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= TDT_PID;
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xsvc_si_SwupCheckStartSection_Ses(xsvcSi_SwupData_Ses_t *pstSwupCheck)
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
		hErr = xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_Ses (pstSwupCheck, eTableType, &eFilterType, &ulExtId, &ulPid);
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

HERROR xsvc_si_SwupCheckStopSection_Ses (xsvcSi_SwupData_Ses_t *pstSwupCheck, SIxTable_Type_e eSiTableType, HUINT32 ulExtId)
{
	HERROR			hErr = ERR_FAIL;

	hErr = svc_si_SwupCheckStopSection(&pstSwupCheck->stBase, eSiTableType, ulExtId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_si_SwupCheckStopSection() failed..! eSiTableType(%d) extId(0x%X) \n", eSiTableType, ulExtId);
	}

	return hErr;
}

STATIC void	 xsvc_si_SwupCheckFreeListItem_Ses (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxSI_Free (pvItem);
	}
}

#define ___LOCAL_SWUP_INFO_FUNCTIONS___

STATIC HERROR xsvc_si_SwupCheckParseSelectorBytes_Ses (HUINT8 *pucRaw, SvcSi_SwupSignal_t *pstSelector)
{
	if (pucRaw == NULL || pstSelector == NULL)
	{
		HxLOG_Error("Invalid Parameters\n");
		return ERR_FAIL;
	}

	pstSelector->ulStartSystemId = HxMACRO_Get32Bit (pucRaw);			pucRaw += 4;		// 0~3 : Start Product ID

		pstSelector->usHdfVer = HxMACRO_Get16Bit (pucRaw);					pucRaw += 2;		// 4~5 : HDF Version
		pstSelector->usDataType = HxMACRO_Get16Bit (pucRaw);				pucRaw += 2;		// 6~7 : Data Type
		svc_si_SwupCheckConvertSwupSiDataTypeToVersionType ((SvcSi_SwupImageType_e)pstSelector->usDataType, &(pstSelector->eDataType));
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
#if 0
	if (*pucRaw >= 4 && pstSelector->ucSwupType == SvcSi_SWUP_TYPE_CONDITIONAL)
	{
		pstSelector->usConditionId = HxMACRO_Get16Bit (pucRaw + 3);
	}
	else
	{
		pstSelector->usConditionId = 0;
	}
#endif

#ifndef CONFIG_BURST_OTA
	if(pstSelector->ucSwupType >= SvcSi_SWUP_TYPE_BURST_USER_SELECT)
	{
		return ERR_FAIL;
	}
#endif
	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckFindProperOuiData_Ses (HUINT32 ulSelectorNum, svcSi_SwupCheck_SsuInfo_t *pstSelectors, SvcSi_SwupSignal_t *pstSwupInfo)
{
	HBOOL			 bFound = FALSE;
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

	HxSTD_MemSet(&stSwupInfo, 0, sizeof(SvcSi_SwupSignal_t));

	for (ulIndex = 0; ulIndex < ulSelectorNum; ulIndex++)
	{
		if( pstSelectors[ulIndex].ulOui == eSWUP_OUI_HUMAX)
		{

			if(pstSelectors[ulIndex].ulSelectorLen < 16)
			{
				continue;
			}

#ifndef CONFIG_BURST_OTA
			if(pstSelectors[ulIndex].ucUpdateType == 0x02 || pstSelectors[ulIndex].ucUpdateType == 0x03)
			{
				continue;
			}
#endif
			HxSTD_MemSet(&stSwupInfo, 0, sizeof(SvcSi_SwupSignal_t));
			pucRaw = pstSelectors[ulIndex].pucSelectSelectorPnt;

			hErr = xsvc_si_SwupCheckParseSelectorBytes_Ses(pucRaw, &stSwupInfo);

			if (hErr == ERR_OK)
			{
				if (PalParam_Activated())
				{
					PAL_PARAM_Item_e	eItem = xsvc_si_SwupCheckCvtVersionTypeToPalParam(stSwupInfo.eDataType);
					if (ePAL_PARAM_MAX == eItem)	{ continue; }
					hErr = PAL_PARAM_GetField(eItem, (void *)&ulSystemVer, sizeof(HUINT32));
				}
				else
				{
					hErr = PAL_SYS_GetSystemVersion (stSwupInfo.eDataType, &ulSystemVer);
				}

				if (hErr != ERR_OK) 				{ continue; }

					if (stSwupInfo.ulEndSystemId != 0xFFFFFFFF)
					{
						if (stSwupInfo.ulStartSystemId <= ulSystemId &&
							stSwupInfo.ulEndSystemId >= ulSystemId &&
							stSwupInfo.ulDataVersion != ulSystemVer)
						{
							bFound = TRUE;
							break;
						}
					}
				}
			}
		}

 	if (bFound)
	{
		if (pstSwupInfo != NULL)
		{
			*pstSwupInfo = stSwupInfo;
		}
	}

	return (bFound == TRUE) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR xsvc_si_SwupCheckGetSwupSignalFromDataBroadcastIdDes_Ses (SIxDataBroadcastIdDes_t *pstDbcIdDes, SvcSi_SwupSignal_t *pstSwupSignal)
{
	HUINT32						 ulSelectorNum;
	svcSi_SwupCheck_SsuInfo_t	*pstOuiSelectors;
	HERROR			 			 hErr;

	hErr = svc_si_SwupCheckGetSsuInfoFromDataBroadcastIdDes (pstDbcIdDes, &ulSelectorNum, &pstOuiSelectors);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_si_SwupCheckGetSsuInfoFromDataBroadcastIdDes Error \n");
		return ERR_FAIL;
	}

	hErr = xsvc_si_SwupCheckFindProperOuiData_Ses(ulSelectorNum, pstOuiSelectors, pstSwupSignal);

	svc_si_SwupCheckFreeSsuInfo (pstOuiSelectors);

	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckFindSsuInfoFromPmt_Ses (xsvcSi_SwupData_Ses_t *pstSwupCheck, HUINT32 *pulNumSsu, SvcSi_SwupSignal_t **pastSwupSignals)
{
	HUINT32					 ulActionId, ulNumSsuSvc= 0, ulCntSsuSvc, i;
	HERROR					 hErr = ERR_FAIL;
	SIxTable_PsiTable_t		*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t				*pstPmt2ndLoop = NULL, *pstPmtDesLoop = NULL;
	HxList_t				*pstSwupInfoList = NULL, *pstList = NULL;
	SvcSi_SwupSignal_t		*pstSwupInfo = NULL, *astSwupInfoArray = NULL;
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

		for (pstPmtDesLoop = pstCurrPmt->pstDesLoop; NULL != pstPmtDesLoop; pstPmtDesLoop = pstPmtDesLoop->next)
		{
			HUINT8					*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstPmtDesLoop);

			if (eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG == *pucRawDes)
			{
				SIxDeferAssocTagDes_t *pstDeferAssocDes;

				pstDeferAssocDes = (SIxDeferAssocTagDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);

				if(pstDeferAssocDes != NULL)
				{
					HxSTD_MemMove(&pstSwupCheck->stAssociationInfo, pstDeferAssocDes, sizeof(SIxDeferAssocTagDes_t));

					pstSwupCheck->stAssociationInfo.ausAssocTags = (HUINT16 *)OxSI_Malloc (sizeof(HUINT16) * pstDeferAssocDes->ucAssocTagSize);
					if(pstSwupCheck->stAssociationInfo.ausAssocTags)
					{
						HxSTD_MemMove(pstSwupCheck->stAssociationInfo.ausAssocTags, pstDeferAssocDes->ausAssocTags, sizeof(HUINT16) * pstDeferAssocDes->ucAssocTagSize);
					}

					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDeferAssocDes);
				}
			}
		}

		for (pstPmt2ndLoop = pstCurrPmt->pst2ndLoop; NULL != pstPmt2ndLoop; pstPmt2ndLoop = pstPmt2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstPmt2ndLoop);
			SIxTable_PmtElement_t	*pstPmtElem;

			if (NULL == pst2ndItem) 				{ continue; }

			pstPmtElem = (SIxTable_PmtElement_t*)&(pst2ndItem->unInfo.stPmt);
			if(pstPmtElem  == NULL)	{continue;}

			hErr = svc_si_SwupCheckIsSsuStreamType(pstPmtElem->ucStreamType);
			if(hErr == ERR_OK)
			{
				for (pstPmtDesLoop = pst2ndItem->pstDesLoop; NULL != pstPmtDesLoop; pstPmtDesLoop = pstPmtDesLoop->next)
				{
					HUINT8					*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstPmtDesLoop);
					SIxDataBroadcastIdDes_t	*pstDbcIdDes = NULL;

					if (eSIxDESCTAG_DATA_BROADCAST_ID == *pucRawDes)
					{
						pstDbcIdDes = (SIxDataBroadcastIdDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
						if (pstDbcIdDes != NULL)
						{
							hErr = xsvc_si_SwupCheckGetSwupSignalFromDataBroadcastIdDes_Ses(pstDbcIdDes, &stSwupInfo);
							if (hErr == ERR_OK)
							{
								//HxLOG_Print("[%s:%d]  PMT : Data Broadcast ID found : SvcId (%04x), PID(%04x)\n", stPmtProgInfo.usProgNum, stPmtElemInfo.usElemPid);

								pstSwupInfo = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t));
								if (pstSwupInfo != NULL)
								{
									stSwupInfo.usSvcId	= pstPmtSec->usProgNum;
									stSwupInfo.usPid		= pstPmtElem->usElemPid;

									HxSTD_memcpy(pstSwupInfo, &stSwupInfo, sizeof(SvcSi_SwupSignal_t));

									pstSwupInfoList = HLIB_LIST_Append(pstSwupInfoList, (void *)pstSwupInfo);
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

	if (pulNumSsu == NULL && pastSwupSignals == NULL)
	{
		hErr = ERR_OK;
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
	if (astSwupInfoArray != NULL)				{ OxSI_Free (astSwupInfoArray); }
	if (pstPmtTable != NULL)					{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }
	if (pstSwupInfoList != NULL)				{ HLIB_LIST_RemoveAllFunc(pstSwupInfoList, xsvc_si_SwupCheckFreeListItem_Ses); }

	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckFindBurstInfoFromPmt_Ses (Handle_t hAction, HUINT32 *pulNumPid, void **pastPids)
{
	HERROR					hErr;
	HUINT16 				*pusPid, *astPidArray = NULL;
	HUINT32 				 ulActionId, ulNumPid = 0, i;
	SIxTable_PsiTable_t 	*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t				*pstPmt2ndLoop = NULL;
	HxList_t				*pstPidList = NULL, *pstList = NULL;

	// PMT parsing해서
	ulActionId = PAL_PIPE_GetActionId (hAction);
	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", ulActionId);
		return ERR_FAIL;
	}

	for (pstCurrPmt = pstPmtTable; pstCurrPmt != NULL; pstCurrPmt = pstCurrPmt->pstNextTable)
	{
		for (pstPmt2ndLoop = pstCurrPmt->pst2ndLoop; NULL != pstPmt2ndLoop; pstPmt2ndLoop = pstPmt2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstPmt2ndLoop);
			SIxTable_PmtElement_t	*pstPmtElem;

			if (NULL == pst2ndItem) 				{ continue; }

			pstPmtElem = (SIxTable_PmtElement_t*)&(pst2ndItem->unInfo.stPmt);
			if(pstPmtElem  == NULL) {continue;}

			hErr = svc_si_SwupCheckIsSsuStreamType(pstPmtElem->ucStreamType);
			if(hErr == ERR_OK)
			{
				pusPid = (HUINT16 *)OxSI_Malloc (sizeof(HUINT16));
				*pusPid = pstPmtElem->usElemPid;
				pstPidList = HLIB_LIST_Append(pstPidList, (void *)pusPid);
			}
		}
	}

	ulNumPid = HLIB_LIST_Length(pstPidList);

	if (ulNumPid == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pulNumPid == NULL && pastPids == NULL)
	{
		hErr = ERR_OK;
		goto Error;
	}

	astPidArray = (HUINT16 *)OxSI_Malloc (sizeof(HUINT16) * ulNumPid);
	if (astPidArray == NULL)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	for (i = 0; i < ulNumPid; i++)
	{
		pstList = HLIB_LIST_GetListItem(pstPidList, i);
		if (pstList == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", ulNumPid, i);
			continue;
		}

		pusPid = (HUINT16 *)HLIB_LIST_Data(pstList);
		if (pusPid == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", i);
			continue;
		}

		HxSTD_MemCopy(&astPidArray[i], pusPid, sizeof(HUINT16));
	}

	if (pulNumPid != NULL)
	{
		*pulNumPid = ulNumPid;
	}

	if (pastPids != NULL)
	{
		*pastPids = astPidArray;
		astPidArray = NULL;
	}

	hErr = ERR_OK;

	Error:
		if (astPidArray != NULL)			{ OxSI_Free (astPidArray); }
		if (pstPmtTable != NULL)			{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }
		if (pstPidList != NULL) 			{ HLIB_LIST_RemoveAllFunc(pstPidList, xsvc_si_SwupCheckFreeListItem_Ses); }

		return hErr;
}

#define _____SWUP_INFO_TO_API_____

// NIT를 통해 얻은 Delivery TP Info들
STATIC HERROR xsvc_si_SwupCheckGetDataTsDeliveryInfo_Ses(Handle_t hAction, HUINT32 *pulNumTs, void **ppvTsInfoArray)
{
	HUINT32			 ulActionId;
	HERROR			 hErr = ERR_FAIL;
	HERROR			 hResult = ERR_FAIL;
	HUINT32			 ulNitActTsNum = 0, ulNitOthTsNum = 0;
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
STATIC HERROR xsvc_si_SwupCheckGetRefNitTripleId_Ses (Handle_t hAction, HUINT32 *pulNumRefNit, void **ppvTripleIdInfoArray)
{
	HERROR		 hErr;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_Ses_t		*pstSwupCheck = (xsvcSi_SwupData_Ses_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumRefNit == NULL) || (ppvTripleIdInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumRefNit : %p, ppvTripleIdInfoArray : %p)\n",pulNumRefNit, ppvTripleIdInfoArray );
		return ERR_FAIL;
	}

	*pulNumRefNit = 0;
	*ppvTripleIdInfoArray = NULL;

	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_REF_NIT, pulNumRefNit, (DbSvc_TripleId_t **)ppvTripleIdInfoArray);

	return hErr;
}

// NIT를 통해 얻은 SSU SCAN Linkage 정보 (SSU-BAT를 가리키는 Triple ID List)
STATIC HERROR xsvc_si_SwupCheckGetSsuScanTripleId_Ses (Handle_t hAction, HUINT32 *pulNumSsuScan, void **ppvTripleIdInfoArray)
{
	HERROR		 hErr;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_Ses_t		*pstSwupCheck = (xsvcSi_SwupData_Ses_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumSsuScan == NULL) || (ppvTripleIdInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumSsuScan : %p, ppvTripleIdInfoArray : %p)\n",pulNumSsuScan, ppvTripleIdInfoArray );
		return ERR_FAIL;
	}

	*pulNumSsuScan = 0;
	*ppvTripleIdInfoArray = NULL;

	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_SSU_SCAN, pulNumSsuScan, (DbSvc_TripleId_t **)ppvTripleIdInfoArray);

	return hErr;
}

// BAT를 통해 얻은 SSU Linkage 정보 (SSU-SVC를 가리키는 Triple ID List)
STATIC HERROR xsvc_si_SwupCheckGetSsuLinkTripleId_Ses(Handle_t hAction, HUINT32 *pulNumSsuLink, void **ppvTripleIdInfoArray)
{
	HERROR		 hErr;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_Ses_t		*pstSwupCheck = (xsvcSi_SwupData_Ses_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumSsuLink == NULL) || (ppvTripleIdInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumSsuLink : %p, ppvTripleIdInfoArray : %p)\n",pulNumSsuLink, ppvTripleIdInfoArray );
		return ERR_FAIL;
	}

	*pulNumSsuLink = 0;
	*ppvTripleIdInfoArray = NULL;

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_SES_HOME_TP:
		hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_ALL, pulNumSsuLink, (DbSvc_TripleId_t **)ppvTripleIdInfoArray);
		if (hErr != ERR_OK)
		{
			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_ALL, pulNumSsuLink, (DbSvc_TripleId_t **)ppvTripleIdInfoArray);
		}
		break;

	case eSI_SWUP_MODE_SES_SSUBAT_TP:
		hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_ALL, pulNumSsuLink, (DbSvc_TripleId_t **)ppvTripleIdInfoArray);
		break;

	default:
		hErr = ERR_FAIL;
	}

	return hErr;
}

// NIT 에 있는 SGT  Linkage 정보 (LCN을 하는 Triple ID)
STATIC HERROR xsvc_si_SwupCheckGetSgtLinkTripleId_Ses (Handle_t hAction, HUINT32 *pulNumSgtLink, void **ppvTripleIdInfoArray)
{
	HERROR		 hErr;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_Ses_t		*pstSwupCheck = (xsvcSi_SwupData_Ses_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumSgtLink == NULL) || (ppvTripleIdInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumSgtLink : %p, ppvTripleIdInfoArray : %p)\n",pulNumSgtLink, ppvTripleIdInfoArray );
		return ERR_FAIL;
	}

	*pulNumSgtLink = 0;
	*ppvTripleIdInfoArray = NULL;

	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, ASTRA_SGT_LINKAGE, pulNumSgtLink, (DbSvc_TripleId_t **)ppvTripleIdInfoArray);

	return hErr;
}

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
// NIT 에 있는 Astra Portal  Linkage 정보 (Astra Portal 의 Triple ID)
STATIC HERROR xsvc_si_SwupCheckGetAstraPortalLinkTripleId_Ses (Handle_t hAction, HUINT32 *pulNumLink, void **ppvTripleIdArray)
{
	HERROR		 hErr;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_Ses_t		*pstSwupCheck = (xsvcSi_SwupData_Ses_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumLink == NULL) || (ppvTripleIdArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumLink : %p, ppvTripleIdArray : %p)\n",pulNumLink, ppvTripleIdArray );
		return ERR_FAIL;
	}

	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, ASTRA_PORTAL_LINKAGE, pulNumLink, (DbSvc_TripleId_t **)ppvTripleIdArray);

	return hErr;
}

// NIT 에 있는 Astra Portal  Linkage 정보 (Astra Regional Portal 의 Triple ID)
STATIC HERROR xsvc_si_SwupCheckGetAstraRegionalPortalLinkTripleId_Ses (Handle_t hAction, HUINT32 *pulNumLink, void **ppvTripleIdArray)
{
	HERROR		 hErr;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_Ses_t		*pstSwupCheck = (xsvcSi_SwupData_Ses_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumLink == NULL) || (ppvTripleIdArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumLink : %p, ppvTripleIdArray : %p)\n",pulNumLink, ppvTripleIdArray );
		return ERR_FAIL;
	}

	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, ASTRA_REGIONAL_PORTAL_LINKAGE, pulNumLink, (DbSvc_TripleId_t **)ppvTripleIdArray);

	return hErr;
}
#endif


// PMT를 통해 얻은 SWUP Signalling Information
STATIC HERROR xsvc_si_SwupCheckGetSwupSignalInfo_Ses  (Handle_t hAction, HUINT32 *pulNumInfo, void **pavSwupInfoArray)
{
	HERROR		 hErr = ERR_FAIL;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	SvcSi_SwupSignal_t		*pstSwupInfo = NULL;
	xsvcSi_SwupData_Ses_t	*pstSwupCheck = (xsvcSi_SwupData_Ses_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumInfo == NULL) || (pavSwupInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumInfo : %p, pavSwupInfoArray : %p)\n",pulNumInfo, pavSwupInfoArray );
		return ERR_FAIL;
	}

	if(pstSwupCheck == NULL)
	{
		HxLOG_Error("pstSwupCheck is NULL \n");
		return ERR_FAIL;
	}

	if (pstSwupCheck->bFound!= TRUE)
	{
		HxLOG_Error("bFound is FALE \n");
		return ERR_FAIL;
	}

	if (pulNumInfo != NULL)
	{
		*pulNumInfo = 1;
	}

	if (pavSwupInfoArray != NULL)
	{
		pstSwupInfo = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t));
		HxSTD_MemSet(pstSwupInfo, 0x00, sizeof(SvcSi_SwupSignal_t));

		if (pstSwupInfo == NULL)
		{
			HxLOG_Error("pstSwupInfo Memalloc Error\n");
			return ERR_FAIL;
		}

		*pstSwupInfo = pstSwupCheck->stSwupInfo;
		*pavSwupInfoArray = (void *) pstSwupInfo;
		hErr = ERR_OK;

	}

	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckParseCompability_Ses (HUINT8 *pucDesRaw, HUINT8 **ppucNextDesRaw, HBOOL *pbHwFound, HBOOL *pbSwFound, HUINT32 *pulSystemVer)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulSignalSystemId, ulSignalSystemVer;
	HUINT32		 ulOui, ulSystemId, ulSystemVer;
	HUINT8		*pucNextDesRaw;
	HERROR		 hErr;

	if (PalParam_Activated())
	{
		hErr = PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_ID, (void *)&ulSystemId, sizeof(HUINT32));
		hErr |= PAL_PARAM_GetField(ePAL_PARAM_APP_VERSION, (void *)&ulSystemVer, sizeof(HUINT32));
	}
	else
	{
		hErr  = PAL_SYS_GetSystemId (&ulSystemId);
		hErr |= PAL_SYS_GetSystemVersion (eVERSION_TYPE_APPL, &ulSystemVer);
	}
	if (hErr != ERR_OK)				{ return ERR_FAIL; }

	ucDesTag = pucDesRaw[0];
	ucDesLen = pucDesRaw[1];
	pucDesRaw += 2;
	pucNextDesRaw = pucDesRaw + ucDesLen;

	switch (ucDesTag)
	{
	case 0x01:				// System Hardware Descriptor
		ulOui = HxMACRO_Get32Bit(pucDesRaw) & 0xffffff;					pucDesRaw += 4;
		ulSignalSystemId = HxMACRO_Get32Bit (pucDesRaw);					pucDesRaw += 4;

		HxLOG_Print("[%s][%d] ________________________ ulOui(0x%x) ulSignalSystemId(0x%x) ulSystemId(0x%x) \n", __FUNCTION__, __LINE__, ulOui, ulSignalSystemId, ulSystemId);
		if ( (ulOui == eSWUP_OUI_HUMAX) && (ulSignalSystemId == ulSystemId) )
		{
			if (ppucNextDesRaw != NULL)				{ *ppucNextDesRaw = pucNextDesRaw; }
			*pbHwFound = TRUE;
			return ERR_OK;
		}
		break;

	case 0x02:				// System Software Descriptor
		ulOui = HxMACRO_Get32Bit (pucDesRaw) & 0xffffff;					pucDesRaw += 4;
		ulSignalSystemVer = HxMACRO_Get32Bit (pucDesRaw);					pucDesRaw += 4;
		HxLOG_Print("[%s][%d] ________________________ ulOui(0x%x) ulSignalSystemVer(0x%x) ulSystemVer0x%x) \n", __FUNCTION__, __LINE__, ulOui, ulSignalSystemVer, ulSystemVer);
		if ((ulOui == eSWUP_OUI_HUMAX) && (ulSignalSystemVer > ulSystemVer))
		{
			if (ppucNextDesRaw != NULL)				{ *ppucNextDesRaw = pucNextDesRaw; }
			*pbSwFound = TRUE;
			*pulSystemVer = ulSignalSystemVer;
			return ERR_OK;
		}

		break;
	}

	if (ppucNextDesRaw != NULL) 			{ *ppucNextDesRaw = pucNextDesRaw; }
	return ERR_OK;

}


STATIC HERROR xsvc_si_SwupCheckGetNetworkInfo_Ses  (Handle_t hAction, HUINT32 *pulNumNetInfo, void **ppvNetInfoArray)
{
	HUINT32			 ulActionId;
	DbSvc_NetInfo_t	*pstNetInfo;
	SIxTable_PsiTable_t		*pstNitTable, *pstCurrNit;
	SIxTable_NitSec_t		*pstNetSec;
	SIxNetNameDes_t		*pstNetNameDes;
	HxList_t				*pstDesLoop, *pst2ndLoop;

	if ((pulNumNetInfo == NULL) || (ppvNetInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumNetInfo : %p, ppvNetInfoArray : %p)\n",pulNumNetInfo, ppvNetInfoArray );
		return ERR_FAIL;
	}

	*pulNumNetInfo = 0;
	*ppvNetInfoArray = NULL;

	pstNetInfo = (DbSvc_NetInfo_t *)OxSI_Malloc (sizeof(DbSvc_NetInfo_t));
	if (pstNetInfo == NULL)
	{
		HxLOG_Error("DbSvc_NetInfo_t mem Alloc error \n" );
		return ERR_FAIL;
	}

	ulActionId = PAL_PIPE_GetActionId (hAction);
	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	if(pstNitTable == NULL)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, NIT\n", ulActionId);

		if (pstNetInfo != NULL)		{ OxSI_Free (pstNetInfo); }
		return ERR_FAIL;
	}

	for (pstCurrNit = pstNitTable; pstCurrNit != NULL; pstCurrNit = pstCurrNit->pstNextTable)
	{
		pstNetSec = (SIxTable_NitSec_t*)&pstCurrNit->unInfo.stNit;
		if(pstNetSec == NULL)	{continue;}

		pstNetInfo->usNetId = pstNetSec->usNetId;

		// parse 1st des loop
		for (pstDesLoop = pstNitTable->pstDesLoop; pstDesLoop != NULL; pstDesLoop = pstDesLoop->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data (pstDesLoop);

			HxLOG_Print("start 1st loop desc parsing \n");

			if (pucRawDes == NULL)
			{
				HxLOG_Print("pucRawDes == NULL, continue \n");
				continue;
			}

			if (*pucRawDes == eSIxDESCTAG_NETWORK_NAME)
			{
				pstNetNameDes = (SIxNetNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
				if (pstNetNameDes != NULL)
				{
					HLIB_STD_StrUtf8NCpy ((HCHAR *)pstNetInfo->szNetName, (const HCHAR *)pstNetNameDes->pszNetName, DxMAX_NET_NAME_LEN );

					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstNetNameDes);
				}
			}
		}

		// parse 2nd des loop
		for (pst2ndLoop = pstNitTable->pst2ndLoop; pst2ndLoop != NULL; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t	*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pst2ndLoop);
			HxList_t				*pst2ndDesLoop = NULL;

			HxLOG_Print("start 2nd loop desc parsing \n");

			if (pst2ndItem == NULL)
			{
				HxLOG_Print("pst2ndItem == NULL, continue \n");
				continue;
			}

			for (pst2ndDesLoop = pst2ndItem->pstDesLoop; pst2ndDesLoop != NULL; pst2ndDesLoop = pst2ndDesLoop->next)
			{
				HUINT8				*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pst2ndDesLoop);

				if (pucRawDes == NULL)
				{
					HxLOG_Print("pucRawDes == NULL, continue \n");
					continue;
				}

				if (*pucRawDes == eSIxDESCTAG_LINKAGE)
				{
					if (*pucRawDes == eSIxDESCTAG_NETWORK_NAME)
					{
						pstNetNameDes = (SIxNetNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
						if (pstNetNameDes != NULL)
						{
							HLIB_STD_StrUtf8NCpy ((HCHAR *)pstNetInfo->szNetName, (const HCHAR *)pstNetNameDes->pszNetName, DxMAX_NET_NAME_LEN );

							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstNetNameDes);
						}
					}
				}
			}
		}
	}

	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }

	*pulNumNetInfo = 1;
	*ppvNetInfoArray = (void *)pstNetInfo;

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckGetScheduleInfo_Ses  (Handle_t hAction, HUINT32 *pulNumSchedInfo, void **ppvSchedInfoArray)
{
	HERROR				hErr;
	HBOOL				bHwFound = FALSE, bSwFound = FALSE;
	HUINT32				ulActionId;
	HUINT32 			ulNumSchedule, ulCntSchedule, i, ulSignalVersion = 0xffffffff;
	UNIXTIME			ulCurTime;
	SIxTable_PsiTable_t	*pstUntTable = NULL;
	SIxTable_UntSec_t	*pstUntSec;
	SIxSchedulingDes_t	*pstSchDes = NULL;
	SIxSsuLocationDes_t	 *pstAssocDes = NULL;
	HUINT16				usAssociationTag = 0;		/** < data_broadcast_id == 0x000A 일때만 사용				*/
	HxList_t			*pstCurrUnt, *pstSchDesList=NULL, *pstList=NULL;
	SvcSi_SwupSchedule_t	*pstSchdInfo = NULL;
	xsvcSi_SwupData_Ses_t	*pstSwupCheck;

	ulActionId = PAL_PIPE_GetActionId(hAction);
	pstSwupCheck = (xsvcSi_SwupData_Ses_t *)svc_si_GetSiData(ulActionId);
	pstUntTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);

	if (NULL == pstUntTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, UNT\n", ulActionId);
		return ERR_FAIL;
	}

	// section info
	pstUntSec = &pstUntTable->unInfo.stUnt;
	if (pstUntSec->ulOui != eSWUP_OUI_HUMAX)
	{
		HxLOG_Warning("OUI Data doesn't match\n");
		hErr = ERR_FAIL;
		goto Error;
	}

	// 1st desc loop - common desc.
	for (pstCurrUnt = pstUntTable->pstDesLoop; NULL != pstCurrUnt; pstCurrUnt = pstCurrUnt->next)
	{
		HUINT8 *pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstCurrUnt);


		if (*pucRawDes == eSIxDESCTAG_UNT_SSU_LOCATION)
		{
			pstAssocDes = (SIxSsuLocationDes_t *)PAL_SIRAWPOOL_ParseSsuUntDes (ulActionId, pucRawDes);
			if(pstAssocDes != NULL)
			{
				usAssociationTag = pstAssocDes->usAssociationTag;
				PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstAssocDes);
			}
		}
		else if (*pucRawDes == eSIxDESCTAG_UNT_SCHEDULING)
		{
			pstSchDes = (SIxSchedulingDes_t *)PAL_SIRAWPOOL_ParseSsuUntDes (ulActionId, pucRawDes);

			if(pstSchDes != NULL)
			{
				pstSchDesList = HLIB_LIST_Append (pstSchDesList, (void *)pstSchDes);
			}
		}
	}

	// 2nd loop
	for (pstCurrUnt = pstUntTable->pst2ndLoop; NULL != pstCurrUnt; pstCurrUnt = pstCurrUnt->next)
	{
		SIxTable_UntInfo_t	*pstUnt = NULL;
		HxList_t			*pstLoop;

		SIxTable_2ndLoop_t	*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstCurrUnt);

		if (NULL == pst2ndItem)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, UNT\n", ulActionId);
			continue;
		}

		pstUnt = &pst2ndItem->unInfo.stUnt;

		for (pstLoop = pstUnt->pstCompDesLoop; NULL != pstLoop; pstLoop = pstLoop->next)
		{
			HUINT8 *pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstLoop);

			xsvc_si_SwupCheckParseCompability_Ses(pucRawDes, NULL, &bHwFound, &bSwFound, &ulSignalVersion);

			if(bHwFound && bSwFound)	break;
		}

		for (pstLoop = pstUnt->pstOperationalDesLoop; NULL != pstLoop; pstLoop = pstLoop->next)
		{
			HUINT8 *pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstLoop);
			if (*pucRawDes == eSIxDESCTAG_UNT_SSU_LOCATION)
			{
				pstAssocDes = (SIxSsuLocationDes_t *)PAL_SIRAWPOOL_ParseSsuUntDes (ulActionId, pucRawDes);
				if(pstAssocDes != NULL)
				{
					usAssociationTag = pstAssocDes->usAssociationTag;
					PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstAssocDes);
				}
			}
			else if (*pucRawDes == eSIxDESCTAG_UNT_SCHEDULING)
			{
				pstSchDes = (SIxSchedulingDes_t *)PAL_SIRAWPOOL_ParseSsuUntDes (ulActionId, pucRawDes);

				if(pstSchDes != NULL)
				{
					pstSchDesList = HLIB_LIST_Append (pstSchDesList, (void *)pstSchDes);
				}
			}
		}

	}

	pstSchdInfo = (SvcSi_SwupSchedule_t *)OxSI_Malloc (sizeof(SvcSi_SwupSchedule_t));

	if (pstAssocDes == NULL)
	{
		HxLOG_Error ("Association descriptor is Null\n");
		hErr = ERR_FAIL;
		goto Error;
	}
	else
	{
		HBOOL bMatched = FALSE;

		for(i = 0; i < pstSwupCheck->stAssociationInfo.ucAssocTagSize; i++)
		{
			if(pstSwupCheck->stAssociationInfo.ausAssocTags[i] == usAssociationTag)
			{
				bMatched = TRUE;
				break;
			}
		}

		if(bMatched)
		{
			pstSchdInfo->stTripleId.usOnId = pstSwupCheck->stAssociationInfo.usOriginalNetworkId;
			pstSchdInfo->stTripleId.usTsId = pstSwupCheck->stAssociationInfo.usTransportStreamId;
			pstSchdInfo->stTripleId.usSvcId = pstSwupCheck->stAssociationInfo.usProgNum;
		}
		else
		{
			HxLOG_Error ("Association descriptor's tag value is not Matched\n");
			hErr = ERR_FAIL;
			goto Error;
		}

	}


	if(bHwFound && bSwFound && (ulSignalVersion != 0xffffffff))
	{
		pstSchdInfo->ulDataVersion = ulSignalVersion;
	}
	else
	{
		HxLOG_Error ("Association descriptor's version info is not Matched\n");
		hErr = ERR_FAIL;
		goto Error;
	}

	// list에 저장된 정보가 하나도 없다면 error~~
	ulNumSchedule = HLIB_LIST_Length(pstSchDesList);
	if (ulNumSchedule == 0)
	{
		HxLOG_Error ("schedule is nothing\n");
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pulNumSchedInfo == NULL && ppvSchedInfoArray == NULL)
	{
		HxLOG_Error ("invalid parameter\n");
		hErr = ERR_OK;
		goto Error;
	}

	ulCurTime = HLIB_STD_GetSystemTime();
	ulCurTime += 60*5;

	if (HLIB_DATETIME_IsValidUnixTime(ulCurTime) != ERR_OK )
	{
		HxLOG_Error ("Fail : Get system time\n");
		hErr = ERR_FAIL;
		goto Error;
	}

	ulCntSchedule = 0;

#if defined(CONFIG_DEBUG)
	HxLOG_Print("+++++++++++++++++++++++++ BURST UPDATE SCHEDULE +++++++++++++++++++++++++");
#endif

	for(i = 0; i < ulNumSchedule; i ++)
	{
		pstList = HLIB_LIST_GetListItem(pstSchDesList, i);
		if (pstList == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", ulNumSchedule, i);
			continue;
		}

		pstSchDes = (SIxSchedulingDes_t *)HLIB_LIST_Data(pstList);

		if(ulCurTime >= pstSchDes->startDateTime)		continue;

		pstSchdInfo->startDateTime[ulCntSchedule] = pstSchDes->startDateTime;
		pstSchdInfo->endDateTime[ulCntSchedule] = pstSchDes->endDateTime;

#if defined(CONFIG_DEBUG)
		{
			HxDATETIME_t start, end;
			HLIB_DATETIME_ConvertUnixTimeToDateTime(pstSchDes->startDateTime, &start);
			HLIB_DATETIME_ConvertUnixTimeToDateTime(pstSchDes->endDateTime, &end);

			HxLOG_Print("[%d] Start Time : %d/%d/%d, %d:%d:%d ~ End Time : %d/%d/%d, %d:%d:%d\n", i,
				start.stDate.usYear, start.stDate.ucMonth, start.stDate.ucDay, start.stTime.ucHour, start.stTime.ucMinute, start.stTime.ucSecond,
				end.stDate.usYear, end.stDate.ucMonth, end.stDate.ucDay, end.stTime.ucHour, end.stTime.ucMinute, end.stTime.ucSecond
				);
		}
#endif

		ulCntSchedule++;

		if(ulCntSchedule >= SvcSi_MAX_SCHUDULE)
			break;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("---------------------------------------------------------------------------");
#endif

	if(ulCntSchedule > 0)
	{
		if (pulNumSchedInfo != NULL)
		{
			*pulNumSchedInfo = ulCntSchedule;
		}

		if (ppvSchedInfoArray != NULL)
		{
			*ppvSchedInfoArray = pstSchdInfo;
			pstSchdInfo = NULL;
		}

		hErr = ERR_OK;
	}
	else
		hErr = ERR_FAIL;

Error:
	if (pstSwupCheck->stAssociationInfo.ausAssocTags != NULL)
	{
		OxSI_Free(pstSwupCheck->stAssociationInfo.ausAssocTags);
		pstSwupCheck->stAssociationInfo.ausAssocTags = NULL;
	}
	if (pstSchdInfo != NULL)	{ OxSI_Free (pstSchdInfo); }
	if (pstUntTable != NULL)	{ PAL_SIRAWPOOL_FreeTable(pstUntTable); }
	if (pstSchDesList != NULL)	{ HLIB_LIST_RemoveAllFunc (pstSchDesList, (FREE_FUNC)PAL_SIRAWPOOL_FreeDes);}

	return hErr;
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC HERROR xsvc_si_SwupCheckProcessAllInHomeTp_Ses (xsvcSi_SwupData_Ses_t *pstSwupCheck)
{
	HBOOL			 	bIsCurrRefNit;
	HUINT32			 	ulCntLink, ulNumLink;
	DbSvc_TripleId_t		*pstTripleIdArray = NULL;
	HERROR			 	hErr;

	// Ref-NIT Linkage 정보를 찾는다.
	bIsCurrRefNit = FALSE;
	pstTripleIdArray = NULL;

	// 현재 Home TP가 Reference NIT TP이기도 한가?
	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_REF_NIT, &ulNumLink, &pstTripleIdArray);
	if (hErr == ERR_OK && ulNumLink > 0 && pstTripleIdArray != NULL)
	{
		for (ulCntLink = 0; ulCntLink < ulNumLink; ulCntLink++)
		{
			if (pstTripleIdArray[ulCntLink].usTsId == pstSwupCheck->usCurrTsId)
			{
				bIsCurrRefNit = TRUE;
			}
		}
	}

	if (pstTripleIdArray != NULL)
	{
		OxSI_Free (pstTripleIdArray);
		pstTripleIdArray = NULL;
	}

	svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);

	NOT_USED_PARAM(bIsCurrRefNit);

	return ERR_OK;
}


STATIC void xsvc_si_SwupCheckMsgCmdStart_Ses(xsvcSi_SwupData_Ses_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Print("[%s:%d] +++ \n", __FUNCTION__, __LINE__);

	(void)svc_si_SwupCheckInitData(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stStartSwupCheck.unSpec), (void*)&(pstMsg->unArg.stStartSwupCheck.stOption), FALSE);
	(void)xsvc_si_SwupCheckInitData_Ses(pstSwupCheck);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)xsvc_si_SwupCheckGetFilteringTypes_Ses(pstSwupCheck);
	(void)xsvc_si_SwupCheckStartSection_Ses (pstSwupCheck);
}

STATIC void xsvc_si_SwupCheckMsgCmdStop_Ses(xsvcSi_SwupData_Ses_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Print("[%s:%d] +++ \n", __FUNCTION__, __LINE__);

	(void)xsvc_si_SwupCheckStopSection_Ses(pstSwupCheck, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPatTable_Ses (xsvcSi_SwupData_Ses_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR				hErr;
	HUINT32				ulActionId;
	HUINT16				usPmtPid;
	SIxTable_PsiTable_t		*pstPatTable = NULL;
	SIxTable_PatSec_t		*pstPatSec = NULL;

	HxLOG_Print("[%s:%d] +++ \n", __FUNCTION__, __LINE__);
	HxLOG_Print("[%s:%d] ulSwupMode \n", __FUNCTION__, __LINE__ , pstSwupCheck->stBase.stSwupOption.ulSwupMode);

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_SES_HOME_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			ulActionId = PAL_PIPE_GetActionId (pstSwupCheck->stBase.hAction);
			pstPatTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
			if (NULL == pstPatTable)
			{
				HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, Pat\n", ulActionId);
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			pstPatSec = &pstPatTable->unInfo.stPat;
			if(pstPatSec != NULL)
			{
				pstSwupCheck->usCurrTsId= pstPatSec->usTsId;
				HxLOG_Print("[%s:%d] pstSwupCheck->usCurrTsId (0x%x) \n", __FUNCTION__, __LINE__, pstSwupCheck->usCurrTsId);
			}

			if (pstPatTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstPatTable); }

			if(TRUE == svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase))
			{
				xsvc_si_SwupCheckProcessAllInHomeTp_Ses(pstSwupCheck);
			}
			break;

		case eSI_SWUP_MODE_SES_SSUSVC_TP:
		case eSI_SWUP_MODE_SES_BURST_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
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
			HxLOG_Error("Non-processing ulSwupMode (0x%08x) -> skipped:\n", pstSwupCheck->stBase.stSwupOption.ulSwupMode);
			return ;

	}

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPmtTable_Ses (xsvcSi_SwupData_Ses_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulIndex, ulNumSsu;
	SvcSi_SwupSignal_t *pstSwupInfoArray = NULL;

	HxLOG_Print("[%s:%d] +++ \n", __FUNCTION__, __LINE__);
#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%d] ulSwupMode(%s) \n" ,__FUNCTION__,__LINE__, xsvc_si_SwupCheckMakeStrMode_Ses(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_SES_SSUSVC_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			hErr = xsvc_si_SwupCheckFindSsuInfoFromPmt_Ses(pstSwupCheck, &ulNumSsu, &pstSwupInfoArray);

			if (hErr != ERR_OK || ulNumSsu == 0 || pstSwupInfoArray == NULL)
			{
				HxLOG_Print("Fail : Get SSU Info.\n");

				if(pstSwupInfoArray != NULL)
				{
					OxSI_Free (pstSwupInfoArray);
					pstSwupInfoArray = NULL;
				}
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			for (ulIndex = 0; ulIndex < ulNumSsu; ulIndex++)
			{
				if (pstSwupInfoArray[ulIndex].ucSwupType == SvcSi_SWUP_TYPE_BURST_USER_SELECT|| pstSwupInfoArray[ulIndex].ucSwupType == SvcSi_SWUP_TYPE_BURST_FORCED)
				{
					HxLOG_Print("[DVBSSU_SWUP] Retrieve UNT (PID 0x%04x)\n", pstSwupInfoArray[ulIndex].usPid);
					hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_NOT, (HUINT32)pstSwupInfoArray[ulIndex].usPid);
					break;
				}
			}

			pstSwupCheck->bFound = TRUE;
			HxSTD_memcpy(&pstSwupCheck->stSwupInfo, &pstSwupInfoArray[0], sizeof(SvcSi_SwupSignal_t));

			if(ulIndex >= ulNumSsu)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
			}

			OxSI_Free (pstSwupInfoArray);
			break;

		case eSI_SWUP_MODE_SES_BURST_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}
			if(TRUE == svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase))
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
			}
			break;

		default:
			HxLOG_Error("Non-processing ulSwupMode (0x%08x) -> skipped:\n", pstSwupCheck->stBase.stSwupOption.ulSwupMode);
			return;

	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtTdtTable_Ses (xsvcSi_SwupData_Ses_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HUINT32					ulActionId;
	SIxTable_PsiTable_t		*pstTdtTable;

	HxLOG_Print("[%s:%d] +++ \n", __FUNCTION__, __LINE__);
	HxLOG_Print("[%s:%d] ulSwupMode = %d\n", __FUNCTION__, __LINE__ , pstSwupCheck->stBase.stSwupOption.ulSwupMode);

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_SES_HOME_TP:
		case eSI_SWUP_MODE_SES_REFNIT_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

#if 1	// TODO: TDT 셋팅

			if(pstSwupCheck->bUpdateTime == FALSE)
			{
				ulActionId = PAL_PIPE_GetActionId (pstSwupCheck->stBase.hAction);

				pstTdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Tdt, SILIB_TABLE_EXT_ID_NOT);
				if (NULL != pstTdtTable)
				{
					pstSwupCheck->bUpdateTime = TRUE;
					svc_si_LibUpdateTimeByTdt (ulActionId, HANDLE_NULL, pstTdtTable);
					PAL_SIRAWPOOL_FreeTable (pstTdtTable);
				}
			}
#endif

			if(TRUE == svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase))
			{
				if(pstSwupCheck->stBase.stSwupOption.ulSwupMode == eSI_SWUP_MODE_SES_HOME_TP)
				{
					xsvc_si_SwupCheckProcessAllInHomeTp_Ses(pstSwupCheck);
				}
				else if (pstSwupCheck->stBase.stSwupOption.ulSwupMode == eSI_SWUP_MODE_SES_REFNIT_TP)
				{
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
				}
			}

			break;

		default:
			HxLOG_Error("Non-processing ulSwupMode (0x%08x) -> skipped:\n", pstSwupCheck->stBase.stSwupOption.ulSwupMode);
			return;

	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtNitTable_Ses (xsvcSi_SwupData_Ses_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HBOOL			bFoundLink = FALSE, bIsCurrRefNit = FALSE, bIsCurrSsuBat = FALSE;
	HUINT32			 ulCntLink, ulNumLink;
	DbSvc_TripleId_t	*pstTripleIdArray;
	HERROR			 	hErr;

	HxLOG_Print("[%s:%d] +++ \n", __FUNCTION__, __LINE__);

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch(pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_SES_HOME_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			// Ref-NIT Linkage 정보를 찾는다.
			bIsCurrRefNit = FALSE;
			pstTripleIdArray = NULL;

			// 현재 Home TP가 Reference NIT TP이기도 한가?
			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_REF_NIT, &ulNumLink, &pstTripleIdArray);
			if (hErr != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			if (ulNumLink > 0 && pstTripleIdArray != NULL)
			{
				for (ulCntLink = 0; ulCntLink < ulNumLink; ulCntLink++)
				{
					if (pstTripleIdArray[ulCntLink].usTsId == pstSwupCheck->usCurrTsId)
					{
						bIsCurrRefNit = TRUE;
					}
				}
			}

			if (pstTripleIdArray != NULL)
			{
				OxSI_Free (pstTripleIdArray);
				pstTripleIdArray = NULL;
			}

			bIsCurrSsuBat = FALSE;
			pstTripleIdArray = NULL;

			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_SSU_SCAN, &ulNumLink, &pstTripleIdArray);
			if (hErr == ERR_OK && ulNumLink > 0 && pstTripleIdArray != NULL)
			{
				for (ulCntLink = 0; ulCntLink < ulNumLink; ulCntLink++)
				{
					if (pstTripleIdArray[ulCntLink].usTsId == pstSwupCheck->usCurrTsId)
					{
						bIsCurrSsuBat = TRUE;
					}
				}
			}

			if (pstTripleIdArray != NULL)
			{
				OxSI_Free (pstTripleIdArray);
				pstTripleIdArray = NULL;
			}

			// HOME TP가 SSU-BAT TP가 아니다 : BAT가 무의미하다.
			if (bIsCurrSsuBat == FALSE)
			{
				HxLOG_Print("[SES_SWUP] This Home TP is not SSU-BAT TP\n");

				xsvc_si_SwupCheckStopSection_Ses(pstSwupCheck, eSIxTABLETYPE_Bat, SILIB_TABLE_EXT_ID_ALL);
			}

			if(TRUE == svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase))
			{
				xsvc_si_SwupCheckProcessAllInHomeTp_Ses (pstSwupCheck);
			}

			break;

		case eSI_SWUP_MODE_SES_REFNIT_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			// SSU-BAT Linkage 정보를 찾는다.
			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_SSU_SCAN, &ulNumLink, &pstTripleIdArray);
			if (hErr == ERR_OK && ulNumLink > 0 && pstTripleIdArray != NULL)
			{
				for (ulCntLink = 0; ulCntLink < ulNumLink; ulCntLink++)
				{
					if (pstTripleIdArray[ulCntLink].usOnId == pstSwupCheck->usCurrOnId &&
						pstTripleIdArray[ulCntLink].usTsId == pstSwupCheck->usCurrTsId)
					{
						bFoundLink = TRUE;
					}
				}
			}

			if (pstTripleIdArray != NULL)
			{
				OxSI_Free (pstTripleIdArray);
				pstTripleIdArray = NULL;
			}

			if (bFoundLink == FALSE)
			{
				xsvc_si_SwupCheckStopSection_Ses(pstSwupCheck, eSIxTABLETYPE_Bat, SILIB_TABLE_EXT_ID_ALL);
			}

			if(TRUE == svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase))
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
			}
			break;


		default:
			HxLOG_Error("Non-processing ulSwupMode (0x%08x) -> skipped:\n", pstSwupCheck->stBase.stSwupOption.ulSwupMode);
			break;
	}

	NOT_USED_PARAM(bIsCurrRefNit);

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtBatTable_Ses (xsvcSi_SwupData_Ses_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR	hErr;

	HxLOG_Print("[%s:%d] +++ \n", __FUNCTION__, __LINE__);
	HxLOG_Print("[%s:%d] ulSwupMode : %d\n", __FUNCTION__, __LINE__ , pstSwupCheck->stBase.stSwupOption.ulSwupMode);

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_SES_HOME_TP:
			if(TRUE == svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase))
			{
				xsvc_si_SwupCheckProcessAllInHomeTp_Ses(pstSwupCheck);
			}
			break;

		case eSI_SWUP_MODE_SES_REFNIT_TP:
			if(TRUE == svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase))
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
			}
			break;

		case eSI_SWUP_MODE_SES_SSUBAT_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_ALL, NULL, NULL);
			if(hErr != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);

			break;

		default:
			HxLOG_Error("Non-processing ulSwupMode (0x%08x) -> skipped:\n", pstSwupCheck->stBase.stSwupOption.ulSwupMode);
			return ;

	}

	return;

}

STATIC void xsvc_si_SwupCheckProcessMsg_Ses (SvcSi_Msg_t *pstMsg, void *pvData)
{
	xsvcSi_SwupData_Ses_t	*pstSwupData = (xsvcSi_SwupData_Ses_t *)pvData;

	if (NULL == pstSwupData)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_OTACHECK:
		HxLOG_Print("[%s:%d]eSIMSG_CMD_START_OTACHECK!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgCmdStart_Ses(pstSwupData, pstMsg);
		break;

	case eSIMSG_CMD_STOP_OTACHECK:
		HxLOG_Print("[%s:%d]eSIMSG_CMD_STOP_OTACHECK!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgCmdStop_Ses(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_PAT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtPatTable_Ses(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_PMT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtPmtTable_Ses(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
	case eSIMSG_EVT_NIT_OTH_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_NIT_TABLE! (%s)\n",__FUNCTION__,__LINE__, pstMsg->eMsgClass == eSIMSG_EVT_NIT_TABLE? "NIT_ACT" : "NIT_OTH");
		xsvc_si_SwupCheckMsgEvtNitTable_Ses(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_TDT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_TDT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtTdtTable_Ses(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_BAT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_BAT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtBatTable_Ses(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_UNT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_UNT_TABLE!\n",__FUNCTION__,__LINE__);
		(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupData->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
		if(TRUE == svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupData->stBase))
		{
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupData->stBase.hAction, 0, 0, 0);
			else
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupData->stBase.hAction, 0, 0, 0);
		}
		//		xsvc_si_SwupCheckMsgEvtUntTable_Dvbssu (pstSwupCheck, pstMsg);
		break;
	default:
		HxLOG_Print("[%s:%d] Non-processing MSG (0x%08x) -> skipped:\n", __FUNCTION__, __LINE__, pstMsg->eMsgClass);
		break;
	}
}

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SwupCheckGetActionFunc_Ses (void)
{
	return xsvc_si_SwupCheckProcessMsg_Ses;
}

void *xsvc_si_SwupCheckAllocData_Ses (void)
{
	xsvcSi_SwupData_Ses_t	*pstSwupCheck;

	pstSwupCheck = (xsvcSi_SwupData_Ses_t *)OxSI_Calloc(sizeof(xsvcSi_SwupData_Ses_t));
	if (NULL != pstSwupCheck)
	{
		pstSwupCheck->stBase.eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return (void *)pstSwupCheck;
}

void xsvc_si_SwupCheckFreeData_Ses (void *pvData)
{
	xsvcSi_SwupData_Ses_t	*pstSwupCheck = (xsvcSi_SwupData_Ses_t *)pvData;

	if (NULL != pstSwupCheck)
	{
		OxSI_Free(pstSwupCheck);
		pstSwupCheck = NULL;
	}

	return;
}

HERROR xsvc_si_SwupCheckGetSwupCheckData_Ses(Handle_t hAction, HUINT32 ulDataType, HUINT32 *pulDataNum, void **pavDataArray)
{
	HERROR						hErr = ERR_FAIL;
	XsvcSi_SwupDataType_e 		eDataType = (XsvcSi_SwupDataType_e)ulDataType;

	if (pulDataNum == NULL)				{ return ERR_FAIL; }
	if (pavDataArray == NULL)			{ return ERR_FAIL; }

	HxLOG_Print("eDataType: %d \n", eDataType);
	*pulDataNum = 0;
	*pavDataArray = NULL;

	switch (eDataType)
	{
	case eSI_SWUP_DATA_DELIVERY_TSINFO:
		hErr = xsvc_si_SwupCheckGetDataTsDeliveryInfo_Ses(hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_REFNIT_TRIPLEID:
		hErr = xsvc_si_SwupCheckGetRefNitTripleId_Ses(hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_SSU_SCAN_TRIPLEID:
		hErr = xsvc_si_SwupCheckGetSsuScanTripleId_Ses(hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_SSU_LINK_TRIPLEID:
		hErr = xsvc_si_SwupCheckGetSsuLinkTripleId_Ses(hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_SESLCN_TRIPLEID:
		hErr = xsvc_si_SwupCheckGetSgtLinkTripleId_Ses (hAction, pulDataNum, pavDataArray);
		break;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	case eSI_SWUP_DATA_ASTAR_PORTAL_TRIPLEID:
		hErr = xsvc_si_SwupCheckGetAstraPortalLinkTripleId_Ses (hAction, pulDataNum, pavDataArray);
		break;
	case eSI_SWUP_DATA_ASTAR_REGIONAL_PORTAL_TRIPLEID:
		hErr = xsvc_si_SwupCheckGetAstraRegionalPortalLinkTripleId_Ses  (hAction, pulDataNum, pavDataArray);
		break;
#endif
	case eSI_SWUP_DATA_SSU_SIGNAL:
		hErr = xsvc_si_SwupCheckGetSwupSignalInfo_Ses (hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_NETWORK_NETINFO:
		hErr =xsvc_si_SwupCheckGetNetworkInfo_Ses (hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_SSU_SCHEDULE_INFO:
		hErr = xsvc_si_SwupCheckGetScheduleInfo_Ses (hAction, pulDataNum, pavDataArray);
		break;

	case eSI_SWUP_DATA_SSU_BURST_INFO:
		hErr = xsvc_si_SwupCheckFindBurstInfoFromPmt_Ses (hAction, pulDataNum, pavDataArray);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	HxLOG_Print("hErr: %d \n", hErr);

	return hErr;

}

HERROR xsvc_si_SwupCheckFreeSwupCheckData_Ses (Handle_t hAction, HUINT32 ulDataType, void *pvDataArray)
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

HERROR xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Ses (SIxLinkDes_t *pstLinkDes)
{
	return svc_si_SwupCheckFindProperOuiDataFromLinkDes(pstLinkDes);
}


/* end of file */

