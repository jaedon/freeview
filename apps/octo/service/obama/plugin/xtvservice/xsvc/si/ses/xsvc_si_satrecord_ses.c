
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_satrecord_ses.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIVE			\n

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
#include <pal_sirawpool.h>
#include <pal_pipe.h>

#include <db_svc.h>
#include <svc_si.h>
#include <_xsvc_si_satrecord_ses.h>
#include <_xsvc_si_lcnupdate_ses.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_svc_si_lib.h>
#include <_xsvc_si_lib_search_base.h>
#include <_svc_si_satrecord.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define MAX_SVCLIST_NAME_LEN				64
//#define SGT_ALL_FILTERING					0xffff0000	// temporary define for compile

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	svcSi_SrchData_t			stBase;

	SvcSi_SatRecordType_e		eSatRecordMode;				// SvcSi_SearchType_e

	HUINT16						usSvcId;
} svcSi_SatRecordData_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
//STATIC HERROR xsvc_si_SatRecordSrchParseSgt_Ses(svcSi_SatRecordData_t *pstSatRecord, Handle_t hAction);
//STATIC HERROR xsvc_si_SatRecordSrchGetServiceListIds_Ses(Handle_t hAction, HUINT32 *pulSvcListNum, HUINT16 **ppusSvcListIdArray);
//STATIC HERROR xsvc_si_SatRecordSrchFreeServiceListIdArray_Ses(HUINT16 *pusSvcListIdArray);
//STATIC HERROR xsvc_si_SatRecordSrchGetServiceListBySvcListId_Ses(Handle_t hAction, HUINT16 usSvcListId, SI_SesSatRecordSvcList_t *pstSvcList);

#define ___LOCAL_FUNCTIONS___

STATIC HERROR xsvc_si_SatRecordSrchInitData_Ses (svcSi_SatRecordData_t *pstSatRecord, Handle_t hAction, SvcSi_SiSpec_t *punSiSpec, SvcSi_SatRecordOption_t *pstSatRecOption)
{
	HERROR				 hErr;
	HUINT32 			 ulDemuxId;
	HUINT32 			 ulActionId = PAL_PIPE_GetActionId (hAction);

	if ((NULL == pstSatRecord) || (NULL == punSiSpec) || (NULL == pstSatRecOption))
	{
		return ERR_FAIL;
	}

	hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_PIPE_GetResourceId: ulActionId(%d), eRxRSCTYPE_DEMUX\n", ulActionId);
		return ERR_FAIL;
	}

	HxSTD_MemSet (pstSatRecord, 0, sizeof(svcSi_SrchData_t));

	// set search base data - 사용할 것만 init
	pstSatRecord->stBase.hAction = hAction;
	HxSTD_MemCopy (&(pstSatRecord->stBase.unSiSpec), punSiSpec, sizeof(SvcSi_SiSpec_t));
	pstSatRecord->stBase.ulDemuxId = ulDemuxId;
	pstSatRecord->stBase.usBatBouquetId = SES_SATREC_BOUQUET_ID;

	pstSatRecord->eSatRecordMode = (SvcSi_SatRecordType_e)pstSatRecOption->ulSatRecordMode;
	pstSatRecord->usSvcId = pstSatRecOption->usExtId3;

	return ERR_OK;
}


STATIC HERROR xsvc_si_SatRecordSrchClearSearchTable (svcSi_SatRecordData_t *pstSatRecord)
{
//	HUINT32			 ulSvcListCnt;

	if (pstSatRecord != NULL)
	{
		HxSTD_memset (pstSatRecord, 0, sizeof(svcSi_SatRecordData_t));
	}

    return ERR_OK;
}

STATIC HERROR xsvc_si_SatRecordSrchCheckAndComposePatData_Ses (svcSi_SatRecordData_t *pstSatRecord, SvcSi_Msg_t *pstMsg)
{
	HERROR			hErr = ERR_FAIL;
	HUINT32 		ulActionId = PAL_PIPE_GetActionId (pstMsg->hAction);
	HUINT16			usPmtPid = PID_NULL;

	switch(pstSatRecord->eSatRecordMode)
	{
		case eSVCSI_SATRECORDTYPE_SES_SATREC_TP:
			if(pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			hErr = svc_si_LibGetPmtPidFromPat (ulActionId, pstSatRecord->usSvcId, &usPmtPid);
			if(hErr != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			hErr = svc_si_SrchStartFilter (&pstSatRecord->stBase, eSIxTABLETYPE_Pmt, (HUINT32)pstSatRecord->usSvcId, (HUINT32)usPmtPid);
			if(hErr != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			return ERR_OK;


		default:
			return ERR_FAIL;
	}
	return ERR_OK;
}

STATIC HERROR xsvc_si_SatRecordSrchCheckAndComposePmtData_Ses (svcSi_SatRecordData_t *pstSatRecord, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Print("[%s:%d] eSatRecordMode \n", __FUNCTION__, __LINE__ , pstSatRecord->eSatRecordMode);

	switch(pstSatRecord->eSatRecordMode)
	{
		case eSVCSI_SATRECORDTYPE_SES_SATREC_TP:
			// Timeout Check
			if(pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstSatRecord->stBase.hAction, 0, 0, 0);
				return ERR_OK;
			}

			svc_si_PostMsgToAp (eSEVT_SI_SEARCH_DONE, pstSatRecord->stBase.hAction, 0, 0, 0);

			break;
		default:
			HxLOG_Error("Non-processing eSatRecordMode (0x%08x) -> skipped:\n", pstSatRecord->eSatRecordMode);
			return ERR_FAIL;

	}
	return ERR_OK;
}

HERROR xsvc_si_SatRecordSrchGetFilteringTypes (svcSi_SatRecordData_t *pstSatRecord)
{
	if (NULL == pstSatRecord)
	{
		return ERR_FAIL;
	}

	switch (pstSatRecord->eSatRecordMode)
	{
		case eSVCSI_SATRECORDTYPE_SES_HOME_TP:
			pstSatRecord->stBase.eFiltering = bSI_FILTERING_NIT_ACT;
			break;

		case eSVCSI_SATRECORDTYPE_SES_REFNIT_TP:
			pstSatRecord->stBase.eFiltering = bSI_FILTERING_BAT;
			break;

		case eSVCSI_SATRECORDTYPE_SES_SATREC_TP:
			pstSatRecord->stBase.eFiltering = bSI_FILTERING_PAT;
			break;

		default:
	//		SesLcnAssert (0);
			return ERR_FAIL;
	}

	return ERR_OK;
}
// eSVCSI_EXTSEARCH_DATA_SES_DELIVERY_TSINFO
// Delivery System : (TS Info)
STATIC HERROR xsvc_si_SatRecordSrchGetDeliveryTsInfo(Handle_t hAction, HUINT32 *pulNumTs, void **ppvInfoArray)
{
	HUINT32			ulActionId;
	HUINT32			ulTotalTsNum = 0;
	DbSvc_TsInfo_t	*pstTsInfoArray = NULL, *pstInfoArray = NULL;
	HERROR			hErr = ERR_FAIL;

	if ((pulNumTs == NULL) || (ppvInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter \n");
		return ERR_FAIL;
	}

	*pulNumTs = 0;
	*ppvInfoArray = NULL;

	ulActionId = PAL_PIPE_GetActionId (hAction);

	hErr = svc_si_LibGetAllDeliveryTsInfoFromNit (ulActionId, TRUE, FALSE, &ulTotalTsNum, &pstTsInfoArray);
	if (hErr != ERR_OK || ulTotalTsNum == 0)
	{
		return ERR_FAIL;
	}

	pstInfoArray = (DbSvc_TsInfo_t *)OxSI_Malloc (sizeof(DbSvc_TsInfo_t) * ulTotalTsNum);
	if (pstInfoArray == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstInfoArray, pstTsInfoArray, sizeof(DbSvc_TsInfo_t) * ulTotalTsNum);

	*pulNumTs = ulTotalTsNum;
	*ppvInfoArray = pstInfoArray;

	if (pstTsInfoArray != NULL)			{ OxSI_Free (pstTsInfoArray); }

	return hErr;
}
STATIC HERROR xsvc_si_SatRecordSrchFindLinkageFromNit(Handle_t hAction, HUINT8 ucLinkType, HUINT32 *pulNumLink, DbSvc_TripleId_t **ppstTripleIdArray)
{
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	SIxTable_PsiTable_t		*pstNitTable = NULL, *pstTable = NULL;
	HxList_t				*pstList = NULL;
	SIxLinkDes_t 			*pstLinkDes = NULL;
	HUINT32					 ulIdx = 0, ulCntRefNit = 0, ulNumRefNit = 0;
	POINTER_LIST_T			*pstDesList = NULL;
	DbSvc_TripleId_t		*pstTripleIdArray = NULL;
	HERROR					hErr = ERR_FAIL;

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	if (pstNitTable == NULL)
	{
		return ERR_FAIL;
	}

	pstTable = pstNitTable;

	while(pstTable != NULL)
	{
		// parse 1st des loop
		for (pstList = pstTable->pstDesLoop; pstList != NULL; pstList = pstList->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);

			HxLOG_Print("start 1st loop desc parsing \n");

			if (pucRawDes == NULL)
			{
				HxLOG_Print("pucRawDes == NULL, continue \n");
				continue;
			}

			if (*pucRawDes == eSIxDESCTAG_LINKAGE)
			{
				pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
				if (pstLinkDes != NULL)
				{
					if (pstLinkDes->ucLinkType == ucLinkType)
					{
						HxLOG_Print("[SES_LCN] NIT : Link (%02x) Found (%04x:%04x:%04x)\n", ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
						ulNumRefNit++;
						pstDesList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstDesList, (void *)pstLinkDes);
					}
				}
				else
				{
					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
				}
			}
		}

		// parse 2nd des loop
		for (pstList = pstTable->pst2ndLoop; pstList != NULL; pstList = pstList->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstList);
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
					pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if (pstLinkDes != NULL)
					{
						if (pstLinkDes->ucLinkType == ucLinkType)
						{
							HxLOG_Print("[SES_LCN] NIT : Link (%02x) Found (%04x:%04x:%04x)\n", ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
							ulNumRefNit++;
							pstDesList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstDesList, (void *)pstLinkDes);
						}
					}
					else
					{
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
					}
				}
			}
		}

		pstTable = pstTable->pstNextTable;
	}

	if (ulNumRefNit == 0)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (pulNumLink == NULL && ppstTripleIdArray == NULL)
	{
		hErr = ERR_OK;
		goto END_FUNC;
	}

	pstTripleIdArray = (DbSvc_TripleId_t *)OxSI_Malloc (sizeof(DbSvc_TripleId_t) * ulNumRefNit);
	if (pstTripleIdArray == NULL)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	while (pstDesList != NULL && ulCntRefNit < ulNumRefNit)
	{
		pstLinkDes = (SIxLinkDes_t *)pstDesList->pvContents;
		if (pstLinkDes != NULL)
		{
			// pstTripleIdArray 내에 이미 들어 있는 Triple ID 인가?
			for (ulIdx = 0; ulIdx < ulCntRefNit; ulIdx++)
			{
				if (pstTripleIdArray[ulIdx].usTsId == pstLinkDes->usTsId &&
					pstTripleIdArray[ulIdx].usOnId == pstLinkDes->usOrgNetId)
				{
					break;
				}
			}

			// pstTripleIdArray 안에 미리 들어 있는 동일한 Triple ID가 없다 : New One
			if (ulIdx == ulCntRefNit)
			{
				pstTripleIdArray[ulCntRefNit].usOnId  = pstLinkDes->usOrgNetId;
				pstTripleIdArray[ulCntRefNit].usTsId  = pstLinkDes->usTsId;
				pstTripleIdArray[ulCntRefNit].usSvcId = pstLinkDes->usSvcId;

				ulCntRefNit++;
			}

			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
		}

		pstDesList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstDesList, pstDesList, NULL);
	}

	if (ulCntRefNit == 0)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (pulNumLink != NULL)
	{
		*pulNumLink = ulCntRefNit;
	}

	if (ppstTripleIdArray != NULL)
	{
		*ppstTripleIdArray = pstTripleIdArray;
		pstTripleIdArray = NULL;
	}

	hErr = ERR_OK;

END_FUNC:
	if (pstTripleIdArray != NULL) 				{ OxSI_Free (pstTripleIdArray); }

	pstDesList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstDesList, (FREE_FUNC)PAL_SIRAWPOOL_FreeDes);
	if(pstDesList != NULL)
	{
		HxLOG_Error("???????????????\n");
	}

	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }

	return hErr;
}

STATIC HERROR xsvc_si_SatRecordSrchFindLinkageFromBat(Handle_t hAction, HUINT8 ucLinkType, HUINT32 *pulNumLink, DbSvc_TripleId_t **ppstTripleIdArray)
{
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	SIxTable_PsiTable_t		*pstBatTable = NULL, *pstTable = NULL;
	HxList_t				*pstList = NULL;
	SIxLinkDes_t 			*pstLinkDes = NULL;
	HUINT32					 ulIdx = 0, ulCntRefNit = 0, ulNumRefNit = 0;
	POINTER_LIST_T			*pstDesList = NULL;
	DbSvc_TripleId_t		*pstTripleIdArray = NULL;
	HERROR					hErr = ERR_FAIL;

	pstBatTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Bat, SILIB_TABLE_EXT_ID_ALL);
	if (pstBatTable == NULL)
	{
		return ERR_FAIL;
	}

	pstTable = pstBatTable;

	while(pstTable != NULL)
	{
		// parse 1st des loop
		for (pstList = pstTable->pstDesLoop; pstList != NULL; pstList = pstList->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);

			HxLOG_Print("start 1st loop desc parsing \n");

			if (pucRawDes == NULL)
			{
				HxLOG_Print("pucRawDes == NULL, continue \n");
				continue;
			}

			if (*pucRawDes == eSIxDESCTAG_LINKAGE)
			{
				pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
				if (pstLinkDes != NULL)
				{
					if (pstLinkDes->ucLinkType == ucLinkType)
					{
						HxLOG_Print("[SES_LCN] NIT : Link (%02x) Found (%04x:%04x:%04x)\n", ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
						ulNumRefNit++;
						pstDesList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstDesList, (void *)pstLinkDes);
					}
				}
				else
				{
					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
				}
			}
		}

		// parse 2nd des loop
		for (pstList = pstTable->pst2ndLoop; pstList != NULL; pstList = pstList->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstList);
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
					pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if (pstLinkDes != NULL)
					{
						if (pstLinkDes->ucLinkType == ucLinkType)
						{
							HxLOG_Print("[SES_LCN] NIT : Link (%02x) Found (%04x:%04x:%04x)\n", ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
							ulNumRefNit++;
							pstDesList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstDesList, (void *)pstLinkDes);
						}
					}
					else
					{
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
					}
				}
			}
		}

		pstTable = pstTable->pstNextTable;
	}

	if (ulNumRefNit == 0)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (pulNumLink == NULL && ppstTripleIdArray == NULL)
	{
		hErr = ERR_OK;
		goto END_FUNC;
	}

	pstTripleIdArray = (DbSvc_TripleId_t *)OxSI_Malloc (sizeof(DbSvc_TripleId_t) * ulNumRefNit);
	if (pstTripleIdArray == NULL)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	while (pstDesList != NULL && ulCntRefNit < ulNumRefNit)
	{
		pstLinkDes = (SIxLinkDes_t *)pstDesList->pvContents;
		if (pstLinkDes != NULL)
		{
			// pstTripleIdArray 내에 이미 들어 있는 Triple ID 인가?
			for (ulIdx = 0; ulIdx < ulCntRefNit; ulIdx++)
			{
				if (pstTripleIdArray[ulIdx].usTsId == pstLinkDes->usTsId &&
					pstTripleIdArray[ulIdx].usOnId == pstLinkDes->usOrgNetId)
				{
					break;
				}
			}

			// pstTripleIdArray 안에 미리 들어 있는 동일한 Triple ID가 없다 : New One
			if (ulIdx == ulCntRefNit)
			{
				pstTripleIdArray[ulCntRefNit].usOnId  = pstLinkDes->usOrgNetId;
				pstTripleIdArray[ulCntRefNit].usTsId  = pstLinkDes->usTsId;
				pstTripleIdArray[ulCntRefNit].usSvcId = pstLinkDes->usSvcId;


				ulCntRefNit++;
			}

			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
		}

		pstDesList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstDesList, pstDesList, NULL);
	}

	if (ulCntRefNit == 0)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (pulNumLink != NULL)
	{
		*pulNumLink = ulCntRefNit;
	}

	if (ppstTripleIdArray != NULL)
	{
		*ppstTripleIdArray = pstTripleIdArray;
		pstTripleIdArray = NULL;
	}

	hErr = ERR_OK;

END_FUNC:
	if (pstTripleIdArray != NULL) 				{ OxSI_Free (pstTripleIdArray); }

	pstDesList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstDesList, (FREE_FUNC)PAL_SIRAWPOOL_FreeDes);
	if(pstDesList != NULL)
	{
		HxLOG_Error("???????????????\n");
	}

	if (NULL != pstBatTable)				{ PAL_SIRAWPOOL_FreeTable (pstBatTable); }

	return hErr;
}

STATIC HERROR xsvc_si_SatRecordSrchGetSatRecInfoFromDataBroadcastIdDes (SIxDataBroadcastIdDes_t *pstDbcIdDes, SatRecordInfo_t *pstSatRecInfo)
{
	HUINT32						 ulSelectorNum;
	svcSi_SatRecord_RRInfo_t	*pstSelectors;
	HERROR			 			hErr;

	hErr = svc_si_SatRecordGetRRInfoFromDataBroadcastIdDes (pstDbcIdDes, &ulSelectorNum, &pstSelectors);

	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_SI_OTA_GetSsuInfoFromDataBroadcastIdDes Error \n");
		return ERR_FAIL;
	}


//	svc_si_SatRecordFreeSatRecInfo (pstSelectors);
	return hErr;
}

STATIC void	 xsvc_si_SatRecordSrchFreeListItem (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxSI_Free (pvItem);
	}
}

STATIC HERROR xsvc_si_SatRecordSrchFindSatRecInfoFromPmt(Handle_t hAction, HUINT32 *pulNumData, SatRecordInfo_t **ppvSatRecInfos)
{
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	HERROR					hErr = ERR_FAIL;
	HUINT32					ulNumSatRec = 0, ulCntSatRec, i;
	SIxTable_PsiTable_t		*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t				*pstPmt2ndLoop = NULL, *pstPmtDesLoop = NULL;
	HxList_t				*pstSatRecInfoList = NULL, *pstList = NULL;

	SatRecordInfo_t			*pstSatRecInfo= NULL, *pstSatRecInfoArray= NULL;
	SatRecordInfo_t			stSatRecInfo;


	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (pstPmtTable == NULL)
	{
		return ERR_FAIL;
	}

	for (pstCurrPmt = pstPmtTable; pstCurrPmt != NULL; pstCurrPmt = pstCurrPmt->pstNextTable)
	{
//		SIxTable_PmtSec_t		*pstPmtSec = &pstCurrPmt->unInfo.stPmt;


		for (pstPmt2ndLoop = pstCurrPmt->pst2ndLoop; NULL != pstPmt2ndLoop; pstPmt2ndLoop = pstPmt2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstPmt2ndLoop);
			SIxTable_PmtElement_t		*pstPmtElem;

			if (NULL == pst2ndItem) 				{ continue; }

			pstPmtElem = (SIxTable_PmtElement_t*)&(pst2ndItem->unInfo.stPmt);
			if(pstPmtElem  == NULL)	{continue;}

			if(pstPmtElem->ucStreamType == DSMCC_STREAM_TYPE)
			{
				for (pstPmtDesLoop = pst2ndItem->pstDesLoop; NULL != pstPmtDesLoop; pstPmtDesLoop = pstPmtDesLoop->next)
				{
					HUINT8					*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstPmtDesLoop);
					SIxDataBroadcastIdDes_t	*pstDbcIdDes = NULL;

					if (NULL == pucRawDes) 				{ continue; }


					if (eSIxDESCTAG_DATA_BROADCAST_ID == *pucRawDes)
					{
						pstDbcIdDes = (SIxDataBroadcastIdDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);

						if (pstDbcIdDes != NULL)
						{
							hErr = xsvc_si_SatRecordSrchGetSatRecInfoFromDataBroadcastIdDes(pstDbcIdDes, &stSatRecInfo);
							if (hErr == ERR_OK)
							{
								pstSatRecInfo = (SatRecordInfo_t *)OxSI_Malloc (sizeof(SatRecordInfo_t));
								if (pstSatRecInfo != NULL)
								{
									stSatRecInfo.dsmccPid = pstPmtElem->usElemPid;

									HxSTD_memcpy(pstSatRecInfo, &stSatRecInfo, sizeof(SatRecordInfo_t));
									pstSatRecInfoList = HLIB_LIST_Append(pstSatRecInfoList, (void *)pstSatRecInfo);
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
				HxLOG_Error("	Stream Type is not Sat Recprd Type: ucStreamType(%04x)\n", pstPmtElem->ucStreamType);
			}

		}
	}

	ulNumSatRec = HLIB_LIST_Length(pstSatRecInfoList);
	if (ulNumSatRec == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pulNumData == NULL && ppvSatRecInfos == NULL)
	{
		hErr = ERR_OK;
		goto Error;
	}

	pstSatRecInfoArray = (SatRecordInfo_t *)OxSI_Malloc (sizeof(SatRecordInfo_t) * ulNumSatRec);
	if (pstSatRecInfoArray == NULL)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	ulCntSatRec = 0;

	for (i = 0; i < ulNumSatRec; i++)
	{
		pstList = HLIB_LIST_GetListItem(pstSatRecInfoList, i);
		if (pstList == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", ulNumSatRec, i);
			continue;
		}

		pstSatRecInfo = (SatRecordInfo_t *)HLIB_LIST_Data(pstList);
		if (pstSatRecInfo == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", i);
			continue;
		}

		HxSTD_MemCopy(&pstSatRecInfoArray[i], pstSatRecInfo, sizeof(SatRecordInfo_t));
		ulCntSatRec++;
	}

	if (ulCntSatRec == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pulNumData != NULL)
	{
		*pulNumData = ulCntSatRec;
	}

	if (ppvSatRecInfos != NULL)
	{
		*ppvSatRecInfos = pstSatRecInfoArray;
		pstSatRecInfoArray = NULL;
	}

	hErr = ERR_OK;

Error:
	if (pstSatRecInfoArray != NULL)				{ OxSI_Free (pstSatRecInfoArray); }
	if (pstPmtTable != NULL)					{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }
	if (pstSatRecInfoList != NULL) 			{ HLIB_LIST_RemoveAllFunc(pstSatRecInfoList, xsvc_si_SatRecordSrchFreeListItem); }

	return hErr;
}

// eSVCSI_EXTSEARCH_DATA_SES_REFNITLINK_TRIPLEID:
// ASTRA Reference NIT 를 가리키는 Triple ID
STATIC HERROR xsvc_si_SatRecordSrchGetRefNitTripleId(Handle_t hAction, HUINT32 *pulNumLink, void **ppvTripleIdArray)
{
	// TODO: NIT의 Linkage Descriptor가 ASTRA_REF_NIT_LINKAGE (0x04)인 놈의 triple id를 얻어온다.

	// sema get (?)
	xsvc_si_SatRecordSrchFindLinkageFromNit(hAction, ASTRA_REF_NIT_LINKAGE, pulNumLink, (DbSvc_TripleId_t **)ppvTripleIdArray);
	// sema release (?)

	return ERR_OK;
}

STATIC HERROR xsvc_si_SatRecordSrchGetAstraRRTripleId(Handle_t hAction, HUINT32 *pulNumLink, void **ppvTripleIdArray)
{

	// sema get (?)
	xsvc_si_SatRecordSrchFindLinkageFromBat(hAction, ASTRA_RECORDING_LINKAGE, pulNumLink, (DbSvc_TripleId_t **)ppvTripleIdArray);
	// sema release (?)

	return ERR_OK;
}

STATIC HERROR xsvc_si_SatRecordSrchGetHDPlusRRTripleId(Handle_t hAction, HUINT32 *pulNumLink, void **ppvTripleIdArray)
{

	// sema get (?)
	xsvc_si_SatRecordSrchFindLinkageFromBat(hAction, HDPLUS_RECORDING_LINKAGE, pulNumLink, (DbSvc_TripleId_t **)ppvTripleIdArray);
	// sema release (?)

	return ERR_OK;
}

STATIC HERROR xsvc_si_SatRecordSrchGetSatRecInfo(Handle_t hAction, HUINT32 *pulNumData, void **ppvSatRecInfoArray)
{
	xsvc_si_SatRecordSrchFindSatRecInfoFromPmt(hAction, pulNumData, (SatRecordInfo_t **)ppvSatRecInfoArray);
	return ERR_OK;
}


#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_SatRecordSrchMsgCmdStartSearch_Ses (svcSi_SatRecordData_t *pstSatRecord, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_SatRecordSrchInitData_Ses (pstSatRecord, pstMsg->hAction, &(pstMsg->unArg.stStartSatRecord.unSpec), &(pstMsg->unArg.stStartSatRecord.stOption));

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstMsg->hAction), eSIxSPEC_SES);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstMsg->hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)xsvc_si_SatRecordSrchGetFilteringTypes (pstSatRecord);
	(void)svc_si_SrchStartSection (&pstSatRecord->stBase);
}

STATIC void xsvc_si_SatRecordSrchMsgCmdStopSearch_Ses (svcSi_SatRecordData_t *pstSatRecord, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_SrchStopSection (&pstSatRecord->stBase);
	(void)svc_si_SrchFreeSearchData (&pstSatRecord->stBase);
	(void)xsvc_si_SatRecordSrchClearSearchTable (pstSatRecord);
}

STATIC void xsvc_si_SatRecordSrchMsgEvtPatTable_Ses (svcSi_SatRecordData_t *pstSatRecord, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstSatRecord->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_SatRecordSrchCheckAndComposePatData_Ses (pstSatRecord, pstMsg);
}

STATIC void xsvc_si_SatRecordSrchMsgEvtPmtTable_Ses (svcSi_SatRecordData_t *pstSatRecord, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstSatRecord->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_SatRecordSrchCheckAndComposePmtData_Ses (pstSatRecord, pstMsg);
}

STATIC void xsvc_si_SatRecordSrchMsgEvtNitTable_Ses (svcSi_SatRecordData_t *pstSatRecord, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstSatRecord->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	if(pstSatRecord->eSatRecordMode == eSVCSI_SATRECORDTYPE_SES_HOME_TP)
	{
		if(pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
		{
			svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
		}
		else
		{
			svc_si_PostMsgToAp (eSEVT_SI_SEARCH_DONE, pstMsg->hAction, 0, 0, 0);
		}

		return;
	}

	return;
}

STATIC void xsvc_si_SatRecordSrchMsgEvtBatTable_Ses (svcSi_SatRecordData_t *pstSatRecord, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstSatRecord->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	if(pstSatRecord->eSatRecordMode == eSVCSI_SATRECORDTYPE_SES_REFNIT_TP)
	{
		if(pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
		{
			svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
		}
		else
		{
			svc_si_PostMsgToAp (eSEVT_SI_SEARCH_DONE, pstMsg->hAction, 0, 0, 0);
		}

		return;
	}

	return;
}

STATIC void xsvc_si_SatRecordSrchProcessMsg_Ses (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_SatRecordData_t	*pstSatRecord = (svcSi_SatRecordData_t *)pvData;

	if (NULL == pstSatRecord)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_SATRECORD:
		xsvc_si_SatRecordSrchMsgCmdStartSearch_Ses (pstSatRecord, pstMsg);
		break;

	case eSIMSG_CMD_STOP_SATRECORD:
		xsvc_si_SatRecordSrchMsgCmdStopSearch_Ses (pstSatRecord, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		xsvc_si_SatRecordSrchMsgEvtNitTable_Ses (pstSatRecord, pstMsg);
		break;

	case eSIMSG_EVT_BAT_TABLE:
		xsvc_si_SatRecordSrchMsgEvtBatTable_Ses (pstSatRecord, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_SatRecordSrchMsgEvtPatTable_Ses (pstSatRecord, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_SatRecordSrchMsgEvtPmtTable_Ses (pstSatRecord, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}
#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SatRecordGetSearchActionFunc_Ses (void)
{
	return xsvc_si_SatRecordSrchProcessMsg_Ses;
}

void *xsvc_si_SatRecordAllocSearchActionData_Ses (void)
{
	svcSi_SatRecordData_t	*pstSatRecord;

	pstSatRecord = (svcSi_SatRecordData_t *)OxSI_Calloc (sizeof(svcSi_SatRecordData_t));
	if (NULL != pstSatRecord)
	{
		pstSatRecord->stBase.eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstSatRecord;
}

void xsvc_si_SatRecordFreeSearchActionData_Ses (void *pvData)
{
	// TODO:
#if 0
	svcSi_SatRecordData_t	*pstSatRecord = (svcSi_SatRecordData_t *)pvData;

	if (NULL != pstSatRecord)
	{
		svc_si_SrchFreeSearchData (&pstSatRecord->stBase);

		// 추가로 free가 필요한 context var.들은 여기에서 정리..!
		//  TODO: ...

		OxSI_Free (pstSatRecord);
		pstSatRecord = NULL;
	}
#endif
	return;
}

HERROR xsvc_si_SatRecordGetSrchData_Ses (Handle_t hAction, SvcSi_SatRecordDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR						hErr = ERR_FAIL;
	HUINT32						ulActionId = PAL_PIPE_GetActionId(hAction);
	svcSi_SatRecordData_t		*pstSatRecord = (svcSi_SatRecordData_t *)svc_si_GetSiData(ulActionId);

	if (pulDataNum == NULL)				{ return ERR_FAIL; }
	if (ppvDataArray == NULL)			{ return ERR_FAIL; }
	if (pstSatRecord == NULL)			{ return ERR_FAIL; }

	HxLOG_Print("[%s] eDataType: 0x%x \n", __FUNCTION__, eDataType);

	*pulDataNum = 0;
	*ppvDataArray = NULL;

	switch (eDataType)
	{
		case eSVCSI_SATRECORD_DATA_SES_DELIVERY_TSINFO:
			hErr = xsvc_si_SatRecordSrchGetDeliveryTsInfo (hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_SATRECORD_DATA_SES_REFNITLINK_TRIPLEID:
			hErr = xsvc_si_SatRecordSrchGetRefNitTripleId (hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_SATRECORD_DATA_SES_ASTRA_RR_TRIPLEID:
			hErr = xsvc_si_SatRecordSrchGetAstraRRTripleId(hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_SATRECORD_DATA_SES_HDPLUS_RR_TRIPLEID:
			hErr = xsvc_si_SatRecordSrchGetHDPlusRRTripleId(hAction, pulDataNum, ppvDataArray);
			break;
		case eSVCSI_SATRECORD_DATA_SES_SATREC_DSMCCINFO:
			hErr = xsvc_si_SatRecordSrchGetSatRecInfo(hAction, pulDataNum, ppvDataArray);
			break;

		default:
			hErr = ERR_FAIL;
			break;

	}

	return hErr;
}

HERROR xsvc_si_SatRecordFreeSrchData_Ses (Handle_t hAction, SvcSi_SatRecordDataType_t eDataType, void *pvDataArray)
{
	(void)hAction;
	(void)eDataType;

	if (pvDataArray != NULL)
	{
		OxSI_Free (pvDataArray);
	}

	return ERR_OK;
}

/* end of file */

