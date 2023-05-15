/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_dynamicupdate.c
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
#include <_svc_si_dynamicupdate.h>

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
STATIC HERROR svc_si_DuConvertTableType2FilterTypeAndExtId (svcSi_SrchData_t *pstSrchData, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
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

STATIC SvcSi_TableFilterMode_e svc_si_DuGetTableFilterMode(SIxTable_Type_e eTableType, HUINT32 ulExtId)
{
	SvcSi_TableFilterMode_e		 eFilterMode = eSI_TABLE_FILTER_MODE_Monitor;

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

#define ___MEMBER_FUNCTIONS___
HERROR svc_si_DuInitSearchData (svcSi_SrchData_t *pstSrchData, Handle_t hAction, SvcSi_SiSpec_t *punSiSpec, SvcSi_SearchOption_t *pstSrchOption)
{
	return svc_si_SrchInitSearchData(pstSrchData, hAction, punSiSpec, pstSrchOption);
}

void svc_si_DuFreeSearchData (svcSi_SrchData_t *pstSrchData)
{
	svc_si_SrchFreeSearchData (pstSrchData);
}

HERROR svc_si_DuStartFilter (svcSi_SrchData_t *pstSrchData, SIxTable_Type_e eTableType, HUINT32 ulExtId, HUINT32 ulPid)
{
	HUINT32						 ulFilterId;
	svcSi_FilteringInfo_t		*pstFilterInfo = (svcSi_FilteringInfo_t *)OxSI_Calloc (sizeof(svcSi_FilteringInfo_t));
	HERROR						 hErr;
	SvcSi_TableFilterMode_e		 eFilterMode = eSI_TABLE_FILTER_MODE_Monitor;

	if (NULL == pstFilterInfo)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		return ERR_FAIL;
	}

	pstFilterInfo->eTableType		= eTableType;
	pstFilterInfo->ulExtId			= ulExtId;
	pstFilterInfo->ulPid			= ulPid;
	eFilterMode						= svc_si_DuGetTableFilterMode(eTableType, ulExtId);

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

HERROR svc_si_DuStartMultiTable (svcSi_SrchData_t *pstSrchData, SIxTable_Type_e eTableType)
{
	HINT32				 i;
	HERROR				 hErr;

	if (NULL == pstSrchData)
	{
		return ERR_FAIL;
	}

	if(eTableType < eSIxTABLETYPE_Pat || eTableType > eSIxTABLETYPE_EndOfCase)
	{
		return ERR_FAIL;
	}

	if((0 < pstSrchData->nNumOfNetId) && (NULL != pstSrchData->pusNitOthNetId))
	{
		for(i = 0; i < pstSrchData->nNumOfNetId; i++)
		{
			hErr = svc_si_DuStartFilter (pstSrchData, eTableType, pstSrchData->pusNitOthNetId[i], NIT_PID);
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


HERROR svc_si_DuStartSection (svcSi_SrchData_t *pstSrchData)
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
		hErr = svc_si_DuConvertTableType2FilterTypeAndExtId (pstSrchData, eTableType, &eFilterType, &ulExtId, &ulPid);
		if ((ERR_OK == hErr) && (pstSrchData->eFiltering & eFilterType))
		{
			HxLOG_Print(HxANSI_COLOR_YELLOW("eTableType = %d, ulExtId = 0x%x, ulPid = %d\n"), eTableType, ulExtId, ulPid);
			hErr = svc_si_DuStartFilter (pstSrchData, eTableType, ulExtId, ulPid);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("svc_si_DuStartFilter err: eTableType(%d)\n", eTableType);
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

HERROR svc_si_DuStopSection (svcSi_SrchData_t *pstSrchData)
{
	return svc_si_SrchStopSection (pstSrchData);
}

HBOOL svc_si_DuCheckAllSiTableReceived (svcSi_SrchData_t *pstSrchData)
{
	return svc_si_SrchCheckAllSiTableReceived(pstSrchData);
}

#define _____SEARCH_RESULT_DATA_____
HERROR svc_si_DuInitSearchResultTable (svcSi_SrchData_t *pstSrchData)
{
	return svc_si_SrchInitSearchResultTable (pstSrchData);
}

HERROR svc_si_DuAddSearchResultItem (svcSi_SrchData_t *pstSrchData, SvcSi_SrchInfoType_e eSrchInfoType, void *pvItem, HUINT32 *pulItemIndex)
{
	return svc_si_SrchAddSearchResultItem(pstSrchData, eSrchInfoType, pvItem, pulItemIndex);
}

HERROR svc_si_DuRemoveSearchResultItem (svcSi_SrchData_t *pstSrchData, SvcSi_SrchInfoType_e eSrchInfoType, void *pvItem)
{
	return svc_si_SrchRemoveSearchResultItem(pstSrchData, eSrchInfoType, pvItem);
}

HERROR svc_si_DuClearSearchResult (svcSi_SrchData_t *pstSrchData, SvcSi_SrchInfoType_e eSrchInfoType)
{
	return svc_si_SrchClearSearchResult(pstSrchData, eSrchInfoType);
}

HERROR svc_si_DuCountSearchedResult (HUINT32 ulActionId, SvcSi_SrchInfoType_e eInfoType, HUINT32 *pulInfoNum)
{
	return svc_si_SrchCountSearchedResult(ulActionId, eInfoType, pulInfoNum);
}

HERROR svc_si_DuGetSearchedResults (HUINT32 ulActionId, SvcSi_SrchInfoType_e eInfoType, HUINT32 ulMaxArrayNum, HUINT32 *pulInfoNum, void *pvInfoArray)
{
	return svc_si_SrchGetSearchedResults(ulActionId, eInfoType, ulMaxArrayNum, pulInfoNum, pvInfoArray);
}

HERROR svc_si_DuFreeSearchResult (svcSi_SrchData_t *pstSrchData)
{
	return svc_si_SrchFreeSearchResult(pstSrchData);
}


/* end of file */

