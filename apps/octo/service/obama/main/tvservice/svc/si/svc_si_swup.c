/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_swup.c
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
#include <linkedlist.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>

#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_svc_si_swup.h>
#include <psvc_si.h>

/*******************************************************************/
/********************      Defines         *************************/
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

#define ___LOCAL_FUNCTIONS___
STATIC HBOOL svc_si_SwupCheckCompareFilterInfo_FilterId(void *pvUser, void *pvListItem)
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

STATIC HBOOL svc_si_SwupCheckCompareFilterInfo_TableType(void *pvUser, void *pvListItem)
{
	svcSi_FilteringInfo_t		*pstUser = (svcSi_FilteringInfo_t *)pvUser;
	svcSi_FilteringInfo_t		*pstListItem = (svcSi_FilteringInfo_t *)pvListItem;

	if ((NULL != pstUser) && (NULL != pstListItem))
	{
		if (pstUser->eTableType == pstListItem->eTableType)
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC void svc_si_SwupCheckFreeFilteringInfo (void *pvData)
{
	svcSi_FilteringInfo_t	*pstFilterInfo = (svcSi_FilteringInfo_t *)pvData;

	if (NULL != pstFilterInfo)
	{
		OxSI_Free(pstFilterInfo);
	}

	return;
}

#define ___MEMBER_FUNCTIONS___
HERROR svc_si_SwupCheckInitData(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData, Handle_t hAction, SvcSi_SiSpec_t *punSiSpec, void *pvSwupCheckOption, HBOOL bLiveSwupCheck)
{
	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32				 ulDemuxId;
	HERROR				 hErr;

	if ((NULL == pstSwupCheckBaseData) || (NULL == punSiSpec) || (NULL == pvSwupCheckOption))
	{
		HxLOG_Error ("Parameter is NULL (pstSwupCheckBaseData : %p , punSiSpec : %p , pvSwupCheckOption : %p \n", ulActionId, pstSwupCheckBaseData, punSiSpec, pvSwupCheckOption);
		return ERR_FAIL;
	}

	hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_PIPE_GetResourceId: ulActionId(%d), eRxRSCTYPE_DEMUX\n", ulActionId);
		return ERR_FAIL;
	}

	HxSTD_MemSet (pstSwupCheckBaseData, 0, sizeof(svcSi_SwupCheck_Base_t));

	pstSwupCheckBaseData->hAction = hAction;
	HxSTD_MemCopy (&(pstSwupCheckBaseData->unSiSpec), punSiSpec, sizeof(SvcSi_SiSpec_t));
	if(bLiveSwupCheck == FALSE)
	{
		HxSTD_MemCopy (&(pstSwupCheckBaseData->stSwupOption), pvSwupCheckOption, sizeof(SvcSi_SwupCheckOption_t));
	}
	else
	{
		HxSTD_MemCopy (&(pstSwupCheckBaseData->stLiveSwupOption), pvSwupCheckOption, sizeof(SvcSi_LiveSwupOption_t));
	}

	pstSwupCheckBaseData->ulDemuxId = ulDemuxId;
	pstSwupCheckBaseData->bLiveSwupCheck = bLiveSwupCheck;

	return ERR_OK;
}

HERROR svc_si_SwupCheckStartFilter(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData, SIxTable_Type_e eTableType, HUINT32 ulExtId, HUINT32 ulPid)
{
	HUINT32						 ulFilterId;
	svcSi_FilteringInfo_t		*pstFilterInfo = (svcSi_FilteringInfo_t *)OxSI_Calloc (sizeof(svcSi_FilteringInfo_t));
	HERROR						 hErr;

	if (NULL == pstFilterInfo)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		return ERR_FAIL;
	}

	pstFilterInfo->eTableType		= eTableType;
	pstFilterInfo->ulExtId			= ulExtId;
	pstFilterInfo->ulPid			= ulPid;

	hErr = svc_si_FilterStartTableSection (pstSwupCheckBaseData->hAction, pstSwupCheckBaseData->ulDemuxId, eTableType, ulExtId, ulPid, eSI_TABLE_FILTER_MODE_Once, &ulFilterId);
	if (ERR_OK == hErr)
	{
		pstFilterInfo->ulFilterId		= ulFilterId;
		pstFilterInfo->eFilterStatus	= eSI_TABLE_WAITING;

		pstSwupCheckBaseData->pstFilterings = HLIB_LIST_Append (pstSwupCheckBaseData->pstFilterings, (void *)pstFilterInfo);
	}
	else
	{
		HxLOG_Error ("svc_si_FilterStartTableSection hAction(0x%08x) TableType(%d) err:\n", pstSwupCheckBaseData->hAction, eTableType);
		OxSI_Free (pstFilterInfo);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_SwupCheckStopSection(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData, SIxTable_Type_e eSiTableType, HUINT32 ulExtId)
{
	HERROR						 hErr;

	if (NULL == pstSwupCheckBaseData)
	{
		return ERR_FAIL;
	}

	hErr = svc_si_FilterStopTableSection (pstSwupCheckBaseData->hAction, eSiTableType, ulExtId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_FilterStopTableSection err: hAction(0x%08x) eSiTableType(%d) extId(0x%X) \n", pstSwupCheckBaseData->hAction, eSiTableType, ulExtId);
		return ERR_FAIL;
	}

	// ALL인 경우는 ota stop이므로 filter status 처리하지 않아도 됨
	if (eSiTableType != eSIxTABLETYPE_ALL)
	{
		HxList_t					*pstFilterList = pstSwupCheckBaseData->pstFilterings;
		svcSi_FilteringInfo_t		*pstFilterInfo;
		HBOOL						 bFoundFilterInfo;

		for (pstFilterList = pstSwupCheckBaseData->pstFilterings; pstFilterList != NULL; pstFilterList = pstFilterList->next)
		{
			pstFilterInfo = (svcSi_FilteringInfo_t *)HLIB_LIST_Data (pstFilterList);
			if (pstFilterInfo == NULL)		{ continue; }

			bFoundFilterInfo = FALSE;
			if (ulExtId > 0xffff)
			{
				if (pstFilterInfo->eTableType == eSiTableType)
				{
					bFoundFilterInfo = TRUE;
				}
			}
			else
			{
				if ((pstFilterInfo->eTableType == eSiTableType) && (pstFilterInfo->ulExtId == ulExtId))
				{
					bFoundFilterInfo = TRUE;
				}
			}

			if (bFoundFilterInfo == TRUE)
			{
				pstFilterInfo->eFilterStatus = eSI_TABLE_TIMEOUT;
			}
		}
	}
	else
	{
		// Stop - OTA
		if(pstSwupCheckBaseData->pstFilterings != NULL)
		{
			pstSwupCheckBaseData->pstFilterings = HLIB_LIST_RemoveAllFunc(pstSwupCheckBaseData->pstFilterings, svc_si_SwupCheckFreeFilteringInfo);
			pstSwupCheckBaseData->pstFilterings = NULL;
		}
	}

	return ERR_OK;
}

svcSi_FilteringInfo_t *svc_si_SwupCheckFindFilterInfoByFilterId(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData, HUINT32 ulFilterId)
{
	svcSi_FilteringInfo_t		 stFilterVal;
	HxList_t					*pstListItem;

	stFilterVal.ulFilterId = ulFilterId;

	pstListItem = HLIB_LIST_FindEx (pstSwupCheckBaseData->pstFilterings, (const void *)&stFilterVal, svc_si_SwupCheckCompareFilterInfo_FilterId);
	if (NULL != pstListItem)
	{
		return (svcSi_FilteringInfo_t *)HLIB_LIST_Data (pstListItem);
	}

	return NULL;
}

HBOOL svc_si_SwupCheckCheckAllSiTableReceived(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData)
{
	HxList_t			*pstListItem;

	if (NULL == pstSwupCheckBaseData)
	{
		return FALSE;
	}

	for (pstListItem = pstSwupCheckBaseData->pstFilterings; NULL != pstListItem; pstListItem = pstListItem->next)
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

HERROR svc_si_SwupCheckSetCheckParam(SvcSi_SwupCheckSpec_t *pstSwupSpec, SvcSi_SwupFlag_t *pstSwupCheckParam)
{
	return psvc_si_SetSwupCheckParam (pstSwupSpec, pstSwupCheckParam);
}


HERROR svc_si_SwupCheckGetData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupSpec, HUINT32 ulDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	if ((pstSwupSpec == NULL) || (pulDataNum == NULL) || (ppvDataArray == NULL))
	{
		HxLOG_Error("Invalid parameter~~ \n");
		return ERR_FAIL;
	}

	return psvc_si_GetSwupCheckData(hAction, pstSwupSpec, ulDataType, pulDataNum, ppvDataArray);
}

HERROR svc_si_SwupCheckFreeData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupCheckSpec, HUINT32 ulDataType, void *pvDataArray)
{
	if ((pstSwupCheckSpec== NULL) || (pvDataArray == NULL))
	{
		HxLOG_Error("Invalid parameter~~ \n");
		return ERR_FAIL;
	}

	return psvc_si_FreeSwupCheckData(hAction, pstSwupCheckSpec, ulDataType, pvDataArray);
}


HERROR svc_si_SwupCheckGetSsuInfoFromSsuLinkDes (SIxLinkDes_t *pstLinkDes, HUINT32 *pulSelectorNum, svcSi_SwupCheck_SsuInfo_t **ppstSelector)
{
	HINT32						 nOuiLen, nSelectorLen;
	HUINT32						 ulSelectorNum;
	HUINT32						 ulSiOui;
	HUINT8						*pucRaw;
	svcSi_SwupCheck_SsuInfo_t	*pstSelector;

	if (pstLinkDes == NULL)
	{
		HxLOG_Error ("pstLinkDes  NULL \n");
		return ERR_FAIL;
	}

	if (pstLinkDes->ucLinkType != eLINKAGE_SSU)
	{
		HxLOG_Error ("ucLinkType is Not eLINKAGE_SSU(0x%x) \n", pstLinkDes->ucLinkType);
		return ERR_FAIL;
	}

	if (pstLinkDes->pstPrivateDataInfo == NULL)
	{
		HxLOG_Error ("pstPrivateDataInfo is NULL \n");
		return ERR_FAIL;
	}

	if (pstLinkDes->pstPrivateDataInfo->pucPrivateData == NULL)
	{
		HxLOG_Error ("pucPrivateData is NULL \n");
		return ERR_FAIL;
	}

	pucRaw = pstLinkDes->pstPrivateDataInfo->pucPrivateData;

	ulSelectorNum = 0;
	nOuiLen = (HINT32)*pucRaw;			pucRaw++;

	while (nOuiLen >= 4)			// 최소한 OUI + selector length는 있어야 한다.
	{
		ulSiOui = HxMACRO_Get24Bit (pucRaw);
		pucRaw += 3;
		nOuiLen -= 3;

		nSelectorLen = (HINT32)*pucRaw;
		pucRaw ++;
		nOuiLen --;

		pucRaw += nSelectorLen;
		nOuiLen -= nSelectorLen;

		if (nOuiLen >= 0)
		{
			ulSelectorNum ++;
		}
	}

	if (ulSelectorNum == 0)
	{
		HxLOG_Error ("ulSelectorNum is Zero \n");
		return ERR_FAIL;
	}

	if (pulSelectorNum != NULL)
	{
		*pulSelectorNum = ulSelectorNum;
	}

	if (ppstSelector != NULL)
	{
		pstSelector = (svcSi_SwupCheck_SsuInfo_t *)OxSI_Malloc (sizeof(svcSi_SwupCheck_SsuInfo_t) * ulSelectorNum);
		if (pstSelector == NULL)
		{
			HxLOG_Error ("pstSelector MemAlloc Error \n");
			return ERR_FAIL;
		}
		HxSTD_MemSet(pstSelector, 0, sizeof(svcSi_SwupCheck_SsuInfo_t) * ulSelectorNum);

		// 다시 Parsing 하면서 내용물을 채운다.
		pucRaw = pstLinkDes->pstPrivateDataInfo->pucPrivateData;

		ulSelectorNum = 0;
		nOuiLen = (HINT32)*pucRaw;			pucRaw++;

		while (nOuiLen >= 4)			// 최소한 OUI + selector length는 있어야 한다.
		{
			ulSiOui = HxMACRO_Get24Bit (pucRaw);
			pucRaw += 3;
			nOuiLen -= 3;

			nSelectorLen = (HINT32)*pucRaw;
			pucRaw ++;
			nOuiLen --;

			if (nOuiLen < nSelectorLen)			{ break; }

			pstSelector[ulSelectorNum].ulOui = ulSiOui;
			pstSelector[ulSelectorNum].ulSelectorLen = (HUINT32)nSelectorLen;
			pstSelector[ulSelectorNum].pucSelectSelectorPnt = pucRaw;

			pucRaw += nSelectorLen;
			nOuiLen -= nSelectorLen;
			ulSelectorNum ++;
		}

		*ppstSelector = pstSelector;
	}

	return ERR_OK;
}

HERROR svc_si_SwupCheckGetSsuInfoFromDataBroadcastIdDes (SIxDataBroadcastIdDes_t *pstDbcIdDes, HUINT32 *pulSelectorNum, svcSi_SwupCheck_SsuInfo_t **ppstSelector)
{
	HBOOL			 bUpdateVerFlag;
	HUINT8			 ucUpdateType, ucUpdateVer;
	HINT32			 nOuiLen, nSelectorLen;
	HUINT32			 ulSelectorNum;
	HUINT32			 ulSiOui;
	HUINT8			*pucRaw;
	svcSi_SwupCheck_SsuInfo_t	*pstSelector;

	if (pstDbcIdDes == NULL)
	{
		HxLOG_Error ("pstDbcIdDes  NULL \n");
		return ERR_FAIL;
	}

	if (pstDbcIdDes->usDataBroadcastId != DATA_BROADCAST_ID_SSU)
	{
		HxLOG_Error ("usDataBroadcastId is Not   DATA_BROADCAST_ID_SSU (0x%x)  \n", pstDbcIdDes->usDataBroadcastId);
		return ERR_FAIL;
	}

	if (pstDbcIdDes->pucIdSelector == NULL)
	{
		HxLOG_Error ("pucIdSelector is NULL \n");
		return ERR_FAIL;
	}

	pucRaw = pstDbcIdDes->pucIdSelector;

	ulSelectorNum = 0;
	nOuiLen = (HINT32)*pucRaw;			pucRaw++;

	while (nOuiLen >= 6)			// 최소한 OUI + infos + selector length는 있어야 한다.
	{
		ulSiOui = HxMACRO_Get24Bit (pucRaw);
		pucRaw += 5;					// (OUI=3bytes + etc info=2bytes)
		nOuiLen -= 5;					// (OUI=3bytes + etc info=2bytes)

		nSelectorLen = (HINT32)*pucRaw;
		pucRaw ++;
		nOuiLen --;

		pucRaw += nSelectorLen;
		nOuiLen -= nSelectorLen;

		if (nOuiLen >= 0)
		{
			ulSelectorNum ++;
		}
	}

	if (ulSelectorNum == 0)
	{
		return ERR_FAIL;
	}

	if (pulSelectorNum != NULL)
	{
		*pulSelectorNum = ulSelectorNum;
	}

	if (ppstSelector != NULL)
	{
		pstSelector = (svcSi_SwupCheck_SsuInfo_t *)OxSI_Malloc (sizeof(svcSi_SwupCheck_SsuInfo_t) * ulSelectorNum);
		if (pstSelector == NULL)
		{
			HxLOG_Error ("pstSelector MemAlloc Error \n");
			return ERR_FAIL;
		}
		HxSTD_MemSet(pstSelector, 0, sizeof(svcSi_SwupCheck_SsuInfo_t) * ulSelectorNum);

		// 다시 Parsing 하면서 내용물을 채운다.
		pucRaw = pstDbcIdDes->pucIdSelector;

		ulSelectorNum = 0;
		nOuiLen = (HINT32)*pucRaw;			pucRaw++;

		while (nOuiLen >= 6)			// 최소한 OUI + selector length는 있어야 한다.
		{
			ulSiOui = HxMACRO_Get24Bit (pucRaw);
			pucRaw += 3;
			nOuiLen -= 3;

			ucUpdateType = (*pucRaw) & 0x0f;
			pucRaw ++;
			nOuiLen --;

			bUpdateVerFlag = ((*pucRaw & 0x20) != 0) ? TRUE : FALSE;
			ucUpdateVer = (HUINT8)(*pucRaw & 0x1f);
			pucRaw ++;
			nOuiLen --;

			nSelectorLen = (HINT32)(*pucRaw);
			pucRaw ++;
			nOuiLen --;

			if (nOuiLen < nSelectorLen)			{ break; }

			pstSelector[ulSelectorNum].ulOui = ulSiOui;
			pstSelector[ulSelectorNum].ucUpdateType = ucUpdateType;
			pstSelector[ulSelectorNum].bUpdateVerFlag = bUpdateVerFlag;
			pstSelector[ulSelectorNum].ucUpdateVer = ucUpdateVer;
			pstSelector[ulSelectorNum].ulSelectorLen = (HUINT32)nSelectorLen;
			pstSelector[ulSelectorNum].pucSelectSelectorPnt = pucRaw;

			pucRaw += nSelectorLen;
			nOuiLen -= nSelectorLen;
			ulSelectorNum ++;
		}

		*ppstSelector = pstSelector;
	}

	return ERR_OK;
}

HERROR svc_si_SwupCheckFreeSsuInfo (svcSi_SwupCheck_SsuInfo_t *pstSelector)
{
	if (pstSelector == NULL)			{ return ERR_FAIL; }

	OxSI_Free (pstSelector);
	return ERR_OK;
}

HERROR svc_si_SwupCheckConvertSwupSiDataTypeToVersionType (SvcSi_SwupImageType_e eSwupImageType, VersionType_t *peVerType)
{
	if (peVerType == NULL)
	{
		return ERR_FAIL;
	}

	switch (eSwupImageType)
	{
	case eSWUP_IMAGE_TYPE_LOADER:
		*peVerType = eVERSION_TYPE_LOADER;
		break;

	case eSWUP_IMAGE_TYPE_APP:
		*peVerType = eVERSION_TYPE_APPL;
		break;

	case eSWUP_IMAGE_TYPE_EEPROM:
		*peVerType = eVERSION_TYPE_EEPROM;
		break;

	case eSWUP_IMAGE_TYPE_FLASH:
		*peVerType = eVERSION_TYPE_DB;
		break;

	// 위에서 지정한 값 외에는 ERR_FAIL, 단 APPLICATION이라고 세팅은 해 준다.
	default:
		*peVerType = eVERSION_TYPE_APPL;
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR svc_si_SwupCheckIsSsuStreamType(HUINT8 ucStreamType)
{
	if ((ucStreamType == 0x0B) || (ucStreamType == 0x0D) ||(ucStreamType >= 0x80))
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR svc_si_SwupCheckFindProperOuiDataFromLinkDes(SIxLinkDes_t *pstLinkDes)
{
	HUINT32			 ulSelectorIdx, ulSelectorNum;
	svcSi_SwupCheck_SsuInfo_t	*pstOuiSelectors;
	HERROR			 hErr;

	hErr = svc_si_SwupCheckGetSsuInfoFromSsuLinkDes (pstLinkDes, &ulSelectorNum, &pstOuiSelectors);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_si_SwupCheckGetSsuInfoFromSsuLinkDes Error \n");
		return ERR_FAIL;
	}

	for (ulSelectorIdx = 0; ulSelectorIdx < ulSelectorNum; ulSelectorIdx++)
	{
		if (pstOuiSelectors[ulSelectorIdx].ulOui == eSWUP_OUI_HUMAX
			|| pstOuiSelectors[ulSelectorIdx].ulOui == eSWUP_OUI_DVB)
		{
			break;
		}
	}

	svc_si_SwupCheckFreeSsuInfo (pstOuiSelectors);

	return (ulSelectorIdx < ulSelectorNum) ? ERR_OK : ERR_FAIL;
}

HERROR svc_si_SwupCheckFindLinkage(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData, SIxTable_Type_e eSiType, HUINT8 ucLinkType, HUINT32 *pulNumLink, DbSvc_TripleId_t **pastTripleIdArray)
{
	HUINT32 			ulActionId;
	HUINT32 			ulNumLinkage, ulCntLinkage, i, j;
	HERROR				hErr;
	SIxTable_PsiTable_t 	*pstPsiTable = NULL, *pstCurrPsi=NULL;
	HxList_t				*pst2ndLoop=NULL, *pstDesLoop=NULL;
	SIxLinkDes_t			*pstLinkDes=NULL;
	HxList_t				*pstLinkDesList=NULL, *pstList=NULL;
	DbSvc_TripleId_t		*astTripleIdArray=NULL;
	HBOOL				bSsuLinkage = FALSE;
	ulActionId = PAL_PIPE_GetActionId (pstSwupCheckBaseData->hAction);
	pstPsiTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSiType, SILIB_TABLE_EXT_ID_ALL);

	if (NULL == pstPsiTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, eSiType(%x) \n", ulActionId, eSiType);
		return ERR_FAIL;
	}

	for (pstCurrPsi = pstPsiTable; pstCurrPsi != NULL; pstCurrPsi = pstCurrPsi->pstNextTable)
	{
		// parse 1st des loop
		for (pstDesLoop = pstCurrPsi->pstDesLoop; pstDesLoop != NULL; pstDesLoop = pstDesLoop->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data (pstDesLoop);

			//HxLOG_Print("start 1st loop desc parsing \n");

			if (pucRawDes == NULL)
			{
				HxLOG_Print("pucRawDes == NULL, continue \n");
				continue;
			}
			
			if (*pucRawDes == eSIxDESCTAG_LINKAGE)
			{
				bSsuLinkage = FALSE;

				pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
				if (pstLinkDes != NULL)
				{
					if (ucLinkType == pstLinkDes->ucLinkType)
					{
						if(ucLinkType == eLINKAGE_SSU)
						{
							bSsuLinkage = TRUE;
						}
					}
					else if(ucLinkType == eLINKAGE_ALL)
					{
						if(pstLinkDes->ucLinkType == eLINKAGE_SSU)
						{
							bSsuLinkage = TRUE;
						}
					}
					else
					{
						HxLOG_Print("1. eSiType(%d) : Not Support ucLinkType (0x%x) \n", eSiType, ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
						pstLinkDes = NULL;
						continue;
					}

					/* 
						SSU-SCAN Linkage (0x0A) 인 경우에는 OUI 를 비교할 필요 없고, 
						SSU-SVC Linage(0x09) 인  경우에만 OUI를 비교해야 된다.
					*/
					if(bSsuLinkage == TRUE)
					{
						hErr = psvc_si_FindProperOuiDataFromLinkDes(&pstSwupCheckBaseData->unSiSpec.stSwupCheckSpec, pstLinkDes);
						if(hErr == ERR_OK)
						{
							HxLOG_Print("1. eSiType(%d) : ucLinkType(0x%08x) Found (%04x:%04x:%04x)\n", eSiType, ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
							pstLinkDesList = HLIB_LIST_Append (pstLinkDesList, (void *)pstLinkDes);
						}
						else
						{
							HxLOG_Print("1. eSiType(%d) : ucLinkType(0x%08x) Not Match OUI (%04x:%04x:%04x)\n", eSiType, ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
							pstLinkDes = NULL;
							continue;
						}
					}
					else
					{
						pstLinkDesList = HLIB_LIST_Append (pstLinkDesList, (void *)pstLinkDes);
					}
				}
			}
		}

		// parse 2nd des loop
		for (pst2ndLoop = pstCurrPsi->pst2ndLoop; pst2ndLoop != NULL; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t	*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pst2ndLoop);
			HxList_t				*pst2ndDesLoop = NULL;

			//HxLOG_Print("start 2nd loop desc parsing \n");

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
					bSsuLinkage = FALSE;

					pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if (pstLinkDes != NULL)
					{
						if (ucLinkType == pstLinkDes->ucLinkType)
						{
							if(ucLinkType == eLINKAGE_SSU)
							{
								bSsuLinkage = TRUE;
							}
						}
						else if(ucLinkType == eLINKAGE_ALL)
						{
							if(pstLinkDes->ucLinkType == eLINKAGE_SSU)
							{
								bSsuLinkage = TRUE;
							}
						}
						else
						{
							HxLOG_Print("2. eSiType(%d) : Not Support ucLinkType (0x%x) \n", eSiType, ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
							pstLinkDes = NULL;
							continue;
						}

						/* 
							SSU-SCAN Linkage (0x0A) 인 경우에는 OUI 를 비교할 필요 없고, 
							SSU-SVC Linage(0x09) 인  경우에만 OUI를 비교해야 된다.
						*/
						if(bSsuLinkage == TRUE)
						{
							hErr = psvc_si_FindProperOuiDataFromLinkDes(&pstSwupCheckBaseData->unSiSpec.stSwupCheckSpec, pstLinkDes);
							if(hErr == ERR_OK)
							{
								HxLOG_Print("2. eSiType(%d) : ucLinkType(0x%08x) Found (%04x:%04x:%04x)\n", eSiType, ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
								pstLinkDesList = HLIB_LIST_Append (pstLinkDesList, (void *)pstLinkDes);
							}
							else
							{
								HxLOG_Print("2. eSiType(%d) : ucLinkType(0x%08x) Not Match OUI (%04x:%04x:%04x)\n", eSiType, ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
								PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
								pstLinkDes = NULL;
								continue;
							}
						}
						else
						{
							pstLinkDesList = HLIB_LIST_Append (pstLinkDesList, (void *)pstLinkDes);
						}
					}
				}
			}
		}
	}

	// list에 저장된 정보가 하나도 없다면 error~~
	ulNumLinkage = HLIB_LIST_Length(pstLinkDesList);
	if (ulNumLinkage == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	HxLOG_Print("ulNumLinkage: %d\n", ulNumLinkage);

	if (pulNumLink == NULL && pastTripleIdArray == NULL)
	{
		hErr = ERR_OK;
		goto Error;
	}

	astTripleIdArray = (DbSvc_TripleId_t *)OxSI_Malloc (sizeof(DbSvc_TripleId_t) * ulNumLinkage);
	if (astTripleIdArray == NULL)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	ulCntLinkage = 0;

	for (i = 0; i < ulNumLinkage; i++)
	{
		pstList = HLIB_LIST_GetListItem(pstLinkDesList, i);
		if (pstList == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", ulNumLinkage, i);
			continue;
		}

		pstLinkDes = (SIxLinkDes_t *)HLIB_LIST_Data(pstList);
		if (pstLinkDes == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", i);
			continue;
		}

		for (j = 0; j < ulCntLinkage; j++)
		{
			// eLINKAGE_ALL 인 경우는 SSU-SVC 를 찾는 경우이다..그래서 Service ID까지 비교해야 된다..
			if(ucLinkType ==eLINKAGE_ALL)
			{
				if (astTripleIdArray[j].usTsId == pstLinkDes->usTsId &&
					astTripleIdArray[j].usOnId == pstLinkDes->usOrgNetId &&
					astTripleIdArray[j].usSvcId == pstLinkDes->usSvcId)
				{
					break;
				}
			}
			else
			{
				if (astTripleIdArray[j].usTsId == pstLinkDes->usTsId &&
					astTripleIdArray[j].usOnId == pstLinkDes->usOrgNetId )
				{
					break;
				}
			}
		}

		// astTripleIdArray 안에 미리 들어 있는 동일한 Triple ID가 없다 : New One
		if (j == ulCntLinkage)
		{
			astTripleIdArray[ulCntLinkage].usOnId  = pstLinkDes->usOrgNetId;
			astTripleIdArray[ulCntLinkage].usTsId  = pstLinkDes->usTsId;
			astTripleIdArray[ulCntLinkage].usSvcId = pstLinkDes->usSvcId;

			ulCntLinkage++;
		}
	}

	if (ulCntLinkage == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pulNumLink != NULL)
	{
		*pulNumLink = ulCntLinkage;
	}

	if (pastTripleIdArray != NULL)
	{
		*pastTripleIdArray = astTripleIdArray;
		astTripleIdArray = NULL;
	}

	hErr = ERR_OK;

Error:
	if (astTripleIdArray != NULL)		{ OxSI_Free (astTripleIdArray); }
	if (pstPsiTable != NULL)			{ PAL_SIRAWPOOL_FreeTable(pstPsiTable); }
	if(pstLinkDesList != NULL)			{HLIB_LIST_RemoveAllFunc (pstLinkDesList, (FREE_FUNC)PAL_SIRAWPOOL_FreeDes);}

	return hErr;

}
/* end of file */

