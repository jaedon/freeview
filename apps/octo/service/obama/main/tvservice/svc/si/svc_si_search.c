/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_search.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

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

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>

#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	svcSi_FilteringType_b		 eFilterType;
	SIxTable_Type_e				 eTableType;
} svcSi_Filter2Table_Conv_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___
STATIC HERROR svc_si_SrchConvertTableType2FilterTypeAndExtId (svcSi_SrchData_t *pstSrchData, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
{
	switch (eTableType)
	{
	case eSIxTABLETYPE_Pat:
		*peFilterType	= bSI_FILTERING_PAT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= PAT_PID;
		break;

	case eSIxTABLETYPE_SdtAct:
		*peFilterType	= bSI_FILTERING_SDT_ACT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= SDT_PID;
		break;

	case eSIxTABLETYPE_SdtOth:
		*peFilterType	= bSI_FILTERING_SDT_OTH;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= SDT_PID;
		break;

	case eSIxTABLETYPE_NitAct:
		*peFilterType	= bSI_FILTERING_NIT_ACT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= NIT_PID;
		break;

	case eSIxTABLETYPE_NitOth:
		*peFilterType	= bSI_FILTERING_NIT_OTH;
		*pulExtId 		= (HUINT32)pstSrchData->usNitNetId;
		*pulPid			= NIT_PID;
		break;

	case eSIxTABLETYPE_Bat:
		*peFilterType	= bSI_FILTERING_BAT;
		*pulExtId		= (HUINT32)pstSrchData->usBatBouquetId;
		*pulPid			= BAT_PID;
		break;

	case eSIxTABLETYPE_Tdt:
		*peFilterType	= bSI_FILTERING_TDT;
		*pulExtId		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= TDT_PID;
		break;

	case eSIxTABLETYPE_Tot:
		*peFilterType	= bSI_FILTERING_TOT;
		*pulExtId		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= TOT_PID;
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HBOOL svc_si_SrchCompareFilterInfo_FilterId (void *pvUser, void *pvListItem)
{
	svcSi_FilteringInfo_t		*pstUser = (svcSi_FilteringInfo_t *)pvUser;
	svcSi_FilteringInfo_t		*pstListItem = (svcSi_FilteringInfo_t *)pvListItem;

	if ((NULL != pstUser) && (NULL != pstListItem))
	{
		if (pstUser->ulFilterId == pstListItem->ulFilterId)
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HBOOL svc_si_SrchCompareFilterInfo_TableTypeAndExtId (void *pvUser, void *pvListItem)
{
	svcSi_FilteringInfo_t		*pstUser = (svcSi_FilteringInfo_t *)pvUser;
	svcSi_FilteringInfo_t		*pstListItem = (svcSi_FilteringInfo_t *)pvListItem;

	if ((NULL != pstUser) && (NULL != pstListItem))
	{
		if ((pstUser->eTableType == pstListItem->eTableType) &&
			(pstUser->ulExtId == pstListItem->ulExtId))
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC SvcSi_TableFilterMode_e svc_si_SrchGetTableFilterMode(SIxTable_Type_e eTableType, HUINT32 ulExtId)
{
	SvcSi_TableFilterMode_e		 eFilterMode = eSI_TABLE_FILTER_MODE_Once;

	if (ulExtId >= 0xffff)
	{
		/* ext id를 사용하지 않는 경우 : SILIB_TABLE_EXT_ID_NOT, SILIB_TABLE_EXT_ID_ALL*/
		switch (eTableType)
		{
			case eSIxTABLETYPE_Bat:			//< BAT ALL
			case eSIxTABLETYPE_Sgt:			//< SGT ALL
			case eSIxTABLETYPE_SdtOth:		//< SDT Other
			case eSIxTABLETYPE_NitOth:		//< NIT Other
				eFilterMode = eSI_TABLE_FILTER_MODE_All;
				break;

			default:
				/* nothing to do */
				break;
		}
	}

	return eFilterMode;
}


#define _____SEARCH_RESULT_DATA_____
STATIC HUINT32 svc_si_SrchGetSearchResultItemSize (SvcSi_SrchInfoType_e eSrchInfoType)
{
	switch (eSrchInfoType)
	{
	case eSI_SRCH_INFOTYPE_Bouquet:			return sizeof(DbSvc_BouqInfo_t);
	case eSI_SRCH_INFOTYPE_Network:			return sizeof(DbSvc_NetInfo_t);
	case eSI_SRCH_INFOTYPE_Transponder:		return sizeof(DbSvc_TsInfo_t);
	case eSI_SRCH_INFOTYPE_Service:			return sizeof(DbSvc_Info_t);
	case eSI_SRCH_INFOTYPE_Provider: 		return sizeof(DbSvc_PrvInfo_t);
	case eSI_SRCH_INFOTYPE_Delivery:		return sizeof(DbSvc_TsInfo_t);
	case eSI_SRCH_INFOTYPE_Group:			return sizeof(DbSvc_GroupInfo_t);
	default:
		HxLOG_Error ("eSrchInfoType(%d): Not Defined\n", eSrchInfoType);
		break;
	}

	return 0;
}

STATIC HUINT32 svc_si_SrchGetSearchResultItemExpansionNum (SvcSi_SrchInfoType_e eSrchInfoType)
{
	switch (eSrchInfoType)
	{
	case eSI_SRCH_INFOTYPE_Bouquet:			return 50;
	case eSI_SRCH_INFOTYPE_Network:			return 10;
	case eSI_SRCH_INFOTYPE_Transponder:		return 20;
	case eSI_SRCH_INFOTYPE_Service:			return 200;
	case eSI_SRCH_INFOTYPE_Provider: 		return 10;
	case eSI_SRCH_INFOTYPE_Delivery:		return 50;
	case eSI_SRCH_INFOTYPE_Group:			return 50;
	default:
		HxLOG_Error ("eSrchInfoType(%d): Not Defined\n", eSrchInfoType);
		break;
	}

	return 0;
}

#define ___MEMBER_FUNCTIONS___
HERROR svc_si_SrchInitSearchData (svcSi_SrchData_t *pstSrchData, Handle_t hAction, SvcSi_SiSpec_t *punSiSpec, SvcSi_SearchOption_t *pstSrchOption)
{
	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32				 ulDemuxId;
	HERROR				 hErr;

	if ((NULL == pstSrchData) || (NULL == punSiSpec) || (NULL == pstSrchOption))
	{
		return ERR_FAIL;
	}

	hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_PIPE_GetResourceId: ulActionId(%d), eRxRSCTYPE_DEMUX\n", ulActionId);
		return ERR_FAIL;
	}

	HxSTD_MemSet (pstSrchData, 0, sizeof(svcSi_SrchData_t));

	pstSrchData->hAction = hAction;
	HxSTD_MemCopy (&(pstSrchData->unSiSpec), punSiSpec, sizeof(SvcSi_SiSpec_t));
	HxSTD_MemCopy (&(pstSrchData->stOption), pstSrchOption, sizeof(SvcSi_SearchOption_t));
	pstSrchData->ulDemuxId = ulDemuxId;

	// Network Search 라면 일단 NIT ACT는 반드시 건다 : SvcSi_NETID_SEARCH_NITACT 면 ID 0, 그 외에는 특정 ID로
	if (SvcSi_NETID_SEARCH_NITACT == pstSrchOption->ulNetId)
	{
		pstSrchData->eNitOption = eSI_SRCH_NITOPT_NitAct;
	}
	else if (SvcSi_NETID_SEARCH_NOTNIT == pstSrchOption->ulNetId)
	{
		pstSrchData->eNitOption = eSI_SRCH_NITOPT_NoNit;
	}
	else
	{
		pstSrchData->eNitOption = eSI_SRCH_NITOPT_NitOth;
		pstSrchData->usNitNetId = (HUINT16)(pstSrchOption->ulNetId & 0xFFFF);
	}

	pstSrchData->ulSearchResult = eSVCSI_SEARCH_RESULT_None;

	return ERR_OK;
}

void svc_si_SrchFreeSearchData (svcSi_SrchData_t *pstSrchData)
{
	HxList_t			*pstListItem;

	if (NULL != pstSrchData)
	{
		// clear filter list
		for (pstListItem = pstSrchData->pstFilterings; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			svcSi_FilteringInfo_t	*pstFilter = (svcSi_FilteringInfo_t *)HLIB_LIST_Data (pstListItem);

			if (NULL != pstFilter)
			{
				OxSI_Free (pstFilter);
			}
		}
		pstSrchData->pstFilterings = HLIB_LIST_RemoveAll (pstSrchData->pstFilterings);

		// clear search result
		svc_si_SrchFreeSearchResult(pstSrchData);
	}
}

HERROR svc_si_SrchGetFilteringTypes (svcSi_SrchData_t *pstSrchData)
{
	if (NULL == pstSrchData)
	{
		return ERR_FAIL;
	}

	// PAT / PMT / SDT shall be retrieved whether the search requires NIT or not.
	pstSrchData->eFiltering = (bSI_FILTERING_PAT | bSI_FILTERING_SDT_ACT);

	switch (pstSrchData->eNitOption)
	{
	case eSI_SRCH_NITOPT_NitAct:
		pstSrchData->eFiltering |= (bSI_FILTERING_NIT_ACT);
		break;

	case eSI_SRCH_NITOPT_NitOth:
		pstSrchData->eFiltering |= (bSI_FILTERING_NIT_OTH);
		break;

	case eSI_SRCH_NITOPT_NoNit:
	default:
		break;
	}

	return ERR_OK;
}

svcSi_FilteringInfo_t *svc_si_SrchFindFilterInfoByFilterId (svcSi_SrchData_t *pstSrchData, HUINT32 ulFilterId)
{
	svcSi_FilteringInfo_t		 stFilterVal;
	HxList_t					*pstListItem;

	stFilterVal.ulFilterId = ulFilterId;

	pstListItem = HLIB_LIST_FindEx (pstSrchData->pstFilterings, (const void *)&stFilterVal, svc_si_SrchCompareFilterInfo_FilterId);
	if (NULL != pstListItem)
	{
		return (svcSi_FilteringInfo_t *)HLIB_LIST_Data (pstListItem);
	}

	return NULL;
}

svcSi_FilteringInfo_t *svc_si_SrchFindFilterInfoByTableTypeAndExtId (svcSi_SrchData_t *pstSrchData, SIxTable_Type_e eTableType, HUINT32 ulExtId)
{
	svcSi_FilteringInfo_t		 stFilterVal;
	HxList_t					*pstListItem;

	stFilterVal.eTableType	= eTableType;
	stFilterVal.ulExtId		= ulExtId;

	pstListItem = HLIB_LIST_FindEx (pstSrchData->pstFilterings, (const void *)&stFilterVal, svc_si_SrchCompareFilterInfo_TableTypeAndExtId);
	if (NULL != pstListItem)
	{
		return (svcSi_FilteringInfo_t *)HLIB_LIST_Data (pstListItem);
	}

	return NULL;
}

HERROR svc_si_SrchStartFilter (svcSi_SrchData_t *pstSrchData, SIxTable_Type_e eTableType, HUINT32 ulExtId, HUINT32 ulPid)
{
	HUINT32						 ulFilterId;
	svcSi_FilteringInfo_t		*pstFilterInfo = (svcSi_FilteringInfo_t *)OxSI_Calloc (sizeof(svcSi_FilteringInfo_t));
	HERROR						 hErr;
	SvcSi_TableFilterMode_e		 eFilterMode = eSI_TABLE_FILTER_MODE_Once;

	if (NULL == pstFilterInfo)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		return ERR_FAIL;
	}

	pstFilterInfo->eTableType		= eTableType;
	pstFilterInfo->ulExtId			= ulExtId;
	pstFilterInfo->ulPid			= ulPid;
	eFilterMode						= svc_si_SrchGetTableFilterMode(eTableType, ulExtId);

	hErr = svc_si_FilterStartTableSection (pstSrchData->hAction, pstSrchData->ulDemuxId, eTableType, ulExtId, ulPid, eFilterMode, &ulFilterId);
	if (ERR_OK == hErr)
	{
		pstFilterInfo->ulFilterId		= ulFilterId;
		pstFilterInfo->eFilterStatus	= eSI_TABLE_WAITING;

		pstSrchData->pstFilterings = HLIB_LIST_Append (pstSrchData->pstFilterings, (void *)pstFilterInfo);
	}
	else
	{
		HxLOG_Error ("svc_si_FilterStartTableSection hAction(0x%08x) TableType(%d) err:\n", pstSrchData->hAction, eTableType);
		OxSI_Free (pstFilterInfo);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_SrchStartSection (svcSi_SrchData_t *pstSrchData)
{
	HUINT32					 ulExtId, ulPid;
	SIxTable_Type_e			 eTableType;
	svcSi_FilteringType_b	 eFilterType;
	HERROR					 hErr;

	if (NULL == pstSrchData)
	{
		return ERR_FAIL;
	}

	for (eTableType = 0; eTableType < eSIxTABLETYPE_MAX; eTableType++)
	{
		hErr = svc_si_SrchConvertTableType2FilterTypeAndExtId (pstSrchData, eTableType, &eFilterType, &ulExtId, &ulPid);
		if ((ERR_OK == hErr) && (pstSrchData->eFiltering & eFilterType))
		{
			hErr = svc_si_SrchStartFilter (pstSrchData, eTableType, ulExtId, ulPid);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("svc_si_SrchStartFilter err: eTableType(%d)\n", eTableType);
			}
		}
	}

	if (NULL == pstSrchData->pstFilterings)
	{
		HxLOG_Error ("No filter has started: All Failed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_SrchStartNitOthMultiTable (svcSi_SrchData_t *pstSrchData)
{
	HINT32				 i;
//	SIxTable_Type_e		 eTableType;
	HERROR				 hErr;

	if (NULL == pstSrchData)
	{
		return ERR_FAIL;
	}

	if((0 < pstSrchData->nNumOfNetId) && (NULL != pstSrchData->pusNitOthNetId))
	{
		for(i = 0; i < pstSrchData->nNumOfNetId; i++)
		{
			hErr = svc_si_SrchStartFilter (pstSrchData, eSIxTABLETYPE_NitOth, pstSrchData->pusNitOthNetId[i], NIT_PID);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("svc_si_SrchStartFilter err: eSIxTABLETYPE_NitOth, ExtId(0x%x)\n", pstSrchData->pusNitOthNetId[i]);
			}
		}
	}

	if (NULL == pstSrchData->pstFilterings)
	{
		HxLOG_Error ("No filter has started: All Failed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_SrchStopSection (svcSi_SrchData_t *pstSrchData)
{
	HERROR					 hErr;

	if (NULL == pstSrchData)
	{
		return ERR_FAIL;
	}

	hErr = svc_si_FilterStopTableSection (pstSrchData->hAction, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_FilterStopTableSection err: hAction(0x%08x)\n", pstSrchData->hAction);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HBOOL svc_si_SrchCheckAllSiTableReceived (svcSi_SrchData_t *pstSrchData)
{
	HxList_t			*pstListItem;

	if (NULL == pstSrchData)
	{
		return FALSE;
	}

	for (pstListItem = pstSrchData->pstFilterings; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_FilteringInfo_t	*pstFilter = (svcSi_FilteringInfo_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstFilter) &&
			(eSI_TABLE_WAITING == pstFilter->eFilterStatus))
		{
			return FALSE;
		}
	}

	return TRUE;
}

#define _____SEARCH_RESULT_DATA_____
HERROR svc_si_SrchInitSearchResultTable (svcSi_SrchData_t *pstSrchData)
{
	SvcSi_SrchInfoType_e	 eInfoType;

	if (NULL == pstSrchData)					{ return ERR_FAIL; }

	for (eInfoType = 0; eInfoType < eSI_SRCH_INFOTYPE_MAX; eInfoType++)
	{
		HUINT32 				 ulItemSize = svc_si_SrchGetSearchResultItemSize (eInfoType);
		HUINT32 				 ulExpansionNum = svc_si_SrchGetSearchResultItemExpansionNum (eInfoType);
		svcSi_SrchInfoTable_t	*pstInfoTable = &(pstSrchData->astSrchInfoTables[eInfoType]);

		if ((ulItemSize == 0) || (ulExpansionNum == 0))
		{
			HxLOG_Error ("!! Info Type Wrong!!! eSrchType(%d) !!!\n", eInfoType);
			continue;
		}

		if (NULL != pstInfoTable->pvInfoArray)
		{
			OxSI_Free (pstInfoTable->pvInfoArray);
		}

		HxSTD_MemSet (pstInfoTable, 0, sizeof(svcSi_SrchInfoTable_t));
		pstInfoTable->eInfoType		= eInfoType;
		pstInfoTable->ulItemSize	= ulItemSize;
		pstInfoTable->ulExpandNum	= ulExpansionNum;
	}

	return ERR_OK;
}

HERROR svc_si_SrchAddSearchResultItem (svcSi_SrchData_t *pstSrchData, SvcSi_SrchInfoType_e eSrchInfoType, void *pvItem, HUINT32 *pulItemIndex)
{
	HUINT8					*pucSrc, *pucDst;
	HUINT32					 ulItemSize = svc_si_SrchGetSearchResultItemSize (eSrchInfoType);
	HUINT32					 ulExpansionNum = svc_si_SrchGetSearchResultItemExpansionNum (eSrchInfoType);
	svcSi_SrchInfoTable_t	*pstInfoTable;

	if ((NULL == pstSrchData) || (eSrchInfoType >= eSI_SRCH_INFOTYPE_MAX) || (NULL == pvItem) ||
		(0 == ulItemSize) || (ulExpansionNum == 0))
	{
		HxLOG_Error ("Argument Wrong: eSrchType(%d)\n", eSrchInfoType);
		return ERR_FAIL;
	}

	pstInfoTable = &(pstSrchData->astSrchInfoTables[eSrchInfoType]);

	if (pstInfoTable->ulInfoNum >= pstInfoTable->ulArraySize)
	{
		HUINT32			 ulNewArraySize = pstInfoTable->ulArraySize + ulExpansionNum;
		void 			*pvNewArray;

		pvNewArray = (void *)OxSI_Realloc (pstInfoTable->pvInfoArray, ulItemSize * ulNewArraySize);
		if (NULL == pvNewArray)
		{
			HxLOG_Error ("Memory allocation failed:\n");
			return ERR_FAIL;
		}

		pstInfoTable->ulArraySize = ulNewArraySize;
		pstInfoTable->pvInfoArray = pvNewArray;
	}

	pucSrc = (HUINT8 *)pvItem;
	pucDst = (HUINT8 *)pstInfoTable->pvInfoArray;

	HxSTD_MemCopy (pucDst + (pstInfoTable->ulInfoNum) * ulItemSize, pucSrc, ulItemSize);
	if (NULL != pulItemIndex)					{ *pulItemIndex = pstInfoTable->ulInfoNum; }

	pstInfoTable->ulInfoNum ++;

	return ERR_OK;
}

HERROR svc_si_SrchRemoveSearchResultItem (svcSi_SrchData_t *pstSrchData, SvcSi_SrchInfoType_e eSrchInfoType, void *pvItem)
{
	HUINT8					*pucSrc, *pucDst;
	HINT32					 nCount, nFoundPos = -1;
	HUINT32					 ulItemSize = svc_si_SrchGetSearchResultItemSize (eSrchInfoType);
	svcSi_SrchInfoTable_t	*pstInfoTable;

	if ((NULL == pstSrchData) || (eSrchInfoType >= eSI_SRCH_INFOTYPE_MAX) ||
		(NULL == pvItem) || (0 == ulItemSize))
	{
		HxLOG_Error ("Argument Wrong: eSrchType(%d)\n", eSrchInfoType);
		return ERR_FAIL;
	}

	pstInfoTable = &(pstSrchData->astSrchInfoTables[eSrchInfoType]);
	pucSrc = (HUINT8 *)pvItem;
	pucDst = (HUINT8 *)pstInfoTable->pvInfoArray;

	for (nCount = 0; nCount < pstInfoTable->ulInfoNum; nCount++)
	{
		if (HxSTD_MemCmp (pucDst + nCount * ulItemSize, pucSrc, ulItemSize) == 0)
		{
			nFoundPos = nCount;
			break;
		}
	}

	if (nFoundPos < 0)
	{
		HxLOG_Error ("No Such Item: \n");
		return ERR_FAIL;
	}

	pstInfoTable->ulInfoNum --;
	for (nCount = nFoundPos; nCount < pstInfoTable->ulInfoNum; nCount++)
	{
		HxSTD_MemCopy (pucDst + (nCount * ulItemSize), pucDst + ((nCount + 1) * ulItemSize), ulItemSize);
	}

	return ERR_OK;
}

HERROR svc_si_SrchClearSearchResult (svcSi_SrchData_t *pstSrchData, SvcSi_SrchInfoType_e eSrchInfoType)
{
	HUINT32					 ulItemSize = svc_si_SrchGetSearchResultItemSize (eSrchInfoType);
	HUINT32					 ulExpansionNum = svc_si_SrchGetSearchResultItemExpansionNum (eSrchInfoType);
	svcSi_SrchInfoTable_t	*pstInfoTable;

	if ((NULL == pstSrchData) || (eSrchInfoType >= eSI_SRCH_INFOTYPE_MAX) || (0 == ulItemSize))
	{
		HxLOG_Error ("Argument Wrong: eSrchType(%d)\n", eSrchInfoType);
		return ERR_FAIL;
	}

	pstInfoTable = &(pstSrchData->astSrchInfoTables[eSrchInfoType]);

	if (NULL != pstInfoTable->pvInfoArray)
	{
		OxSI_Free (pstInfoTable->pvInfoArray);
	}

	HxSTD_MemSet (pstInfoTable, 0, sizeof(svcSi_SrchInfoTable_t));

	pstInfoTable->eInfoType			= eSrchInfoType;
	pstInfoTable->ulItemSize		= ulItemSize;
	pstInfoTable->ulExpandNum		= ulExpansionNum;

	return ERR_OK;
}

HERROR svc_si_SrchCountSearchedResult (HUINT32 ulActionId, SvcSi_SrchInfoType_e eInfoType, HUINT32 *pulInfoNum)
{
	svcSi_ContextState_e	 eState = eSvcSi_CtxtState_None;
	SvcSi_SiSpec_t			 unSiSpec;
	svcSi_SrchData_t		*pstSrchData = (svcSi_SrchData_t *)svc_si_GetSiData (ulActionId);
	svcSi_SrchInfoTable_t	*pstInfoTable;
	HERROR					 hErr;

	if ((ulActionId >= eActionId_MAX) || (eInfoType >= eSI_SRCH_INFOTYPE_MAX) || (NULL == pulInfoNum))
	{
		HxLOG_Error("[%s:%d] invalid param. : actionId(%d) infoType(%d) ", ulActionId, eInfoType);
		return ERR_FAIL;
	}

	*pulInfoNum = 0;

	if (NULL == pstSrchData)
	{
		HxLOG_Error ("Search Data Not allocated... Error:\n");
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiState (ulActionId, &eState);
	if ((ERR_OK != hErr) || (eSvcSi_CtxtState_Running != eState))
	{
		HxLOG_Error ("svc_si_GetSiState err(%d): ulActionId(%d), eState(%d)\n", hErr, ulActionId, eState);
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err(%d): ulActionId(%d)\n", hErr, ulActionId, eState);
		return ERR_FAIL;
	}

	HxLOG_Info("eInfoType: %d, unSiSpec.stTemplate.eActionKind: %d \n", eInfoType, unSiSpec.stTemplate.eActionKind);

	switch (unSiSpec.stTemplate.eActionKind)
	{
	case eSVCSI_ACT_KIND_Search:
	case eSVCSI_ACT_KIND_ExtSearch:
	case eSVCSI_ACT_KIND_DynamicUpdate:
		pstInfoTable = &(pstSrchData->astSrchInfoTables[eInfoType]);
		HxLOG_Info("pstInfoTable->ulInfoNum: %d \n", pstInfoTable->ulInfoNum);
		if(NULL == pstInfoTable->pvInfoArray)
		{
			HxLOG_Info("NULL == pstInfoTable->pvInfoArray \n");
			;
		}

		if ((pstInfoTable->ulInfoNum > 0) && (NULL != pstInfoTable->pvInfoArray))
		{
			*pulInfoNum = pstInfoTable->ulInfoNum;
			return ERR_OK;
		}
		break;

	default:
		HxLOG_Error ("Unsupported Action Kind:(%d)\n", unSiSpec.stTemplate.eActionKind);
		break;
	}

	return ERR_FAIL;
}

HERROR svc_si_SrchGetSearchedResults (HUINT32 ulActionId, SvcSi_SrchInfoType_e eInfoType, HUINT32 ulMaxArrayNum, HUINT32 *pulInfoNum, void *pvInfoArray)
{
	svcSi_ContextState_e	 eState = eSvcSi_CtxtState_None;
	SvcSi_SiSpec_t			 unSiSpec;
	svcSi_SrchData_t		*pstSrchData = (svcSi_SrchData_t *)svc_si_GetSiData (ulActionId);
	svcSi_SrchInfoTable_t	*pstInfoTable;
	HERROR					 hErr;

	if ((ulActionId >= eActionId_MAX) || (eInfoType >= eSI_SRCH_INFOTYPE_MAX) || (0 == ulMaxArrayNum) || (NULL == pvInfoArray))
	{
		return ERR_FAIL;
	}

	if (NULL != pulInfoNum)					{ *pulInfoNum = 0; }

	if (NULL == pstSrchData)
	{
		HxLOG_Error ("Search Data Not allocated... Error:\n");
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiState (ulActionId, &eState);
	if ((ERR_OK != hErr) || (eSvcSi_CtxtState_Running != eState))
	{
		HxLOG_Error ("svc_si_GetSiState err(%d): ulActionId(%d), eState(%d)\n", hErr, ulActionId, eState);
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err(%d): ulActionId(%d)\n", hErr, ulActionId, eState);
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.eActionKind)
	{
	case eSVCSI_ACT_KIND_Search:
	case eSVCSI_ACT_KIND_ExtSearch:
	case eSVCSI_ACT_KIND_DynamicUpdate:
		pstInfoTable = &(pstSrchData->astSrchInfoTables[eInfoType]);
		if ((pstInfoTable->ulInfoNum > 0) && (NULL != pstInfoTable->pvInfoArray))
		{
			HUINT32			 ulInfoNum = (ulMaxArrayNum > pstInfoTable->ulInfoNum) ? pstInfoTable->ulInfoNum : ulMaxArrayNum;
			HUINT32			 ulInfoSize = svc_si_SrchGetSearchResultItemSize (eInfoType);

			if (0 == ulInfoSize)
			{
				HxLOG_Error ("ItemType(%d) has size 0: something wrong!!!\n", eInfoType);
				return ERR_FAIL;
			}

			if (NULL != pulInfoNum)					{ *pulInfoNum = ulInfoNum; }

			HxSTD_MemCopy (pvInfoArray, pstInfoTable->pvInfoArray, ulInfoSize * ulInfoNum);
			return ERR_OK;
		}

		break;

	default:
		HxLOG_Error ("Unsupported Action Kind:(%d)\n", unSiSpec.stTemplate.eActionKind);
		break;
	}

	return ERR_FAIL;
}

HERROR svc_si_SrchFreeSearchResult (svcSi_SrchData_t	 *pstSrchData)
{
	SvcSi_SrchInfoType_e 	eSrchInfoType;
	HERROR					hErr = ERR_FAIL;

	if (NULL == pstSrchData)
	{
		HxLOG_Error ("Argument Wrong\n");
		return ERR_FAIL;
	}

	for (eSrchInfoType = 0; eSrchInfoType < eSI_SRCH_INFOTYPE_MAX ; eSrchInfoType++)
	{
		if (pstSrchData->astSrchInfoTables[eSrchInfoType].ulInfoNum > 0)
		{
			hErr = svc_si_SrchClearSearchResult(pstSrchData, eSrchInfoType);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("svc_si_SrchClearSearchResult() error(%d) - SrchInfoType(%d) \n", hErr, eSrchInfoType);
			}
		}
	}

	return ERR_OK;
}


/* end of file */

