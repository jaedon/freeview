/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lcnupdate_ses.c
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
#include <xdb_svc.h>
#include <svc_si.h>
#include <_xsvc_si_lcnupdate_ses.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_svc_si_lib.h>
#include <_xsvc_si_lib_search_base.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MAX_SVCLIST_NAME_LEN				64
#define SGT_ALL_FILTERING					0xffff0000	// temporary define for compile

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT16					 usSvcListId;
	HUINT8					 ucSecVer;
	HUINT8					 szSvcListName[MAX_SVCLIST_NAME_LEN];

#if 1		// octo 2.0에서 변경..?
	SvcSi_BouquetInfoTable_t		 stBqInfoTable;
	SvcSi_NetworkInfoTable_t		 stNetInfoTable;
	SvcSi_TsInfoTable_t				 stTsInfoTable;
	SvcSi_ServiceInfoTable_t		 stSvcInfoTable;
	SvcSi_ProviderInfoTable_t		 stPrvInfoTable;
#endif
} SI_SesLcnSvcList_t;

typedef struct
{
	svcSi_SrchData_t			stBase;

	// search init 에서 초기화 된다.
	SvcSi_SearchType_e			eSrchType;				// SvcSi_SearchType_e

	HUINT16						usSvcListId;			// Service List ID
	HUINT8						bAllSearch;				// 모든 SvcList를 다 찾을 것인가
	HUINT16						usSgtSvcId;

	// Search Result
	HUINT32					 	ulSvcListNum;
	SI_SesLcnSvcList_t			*pstSvcListArray;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	HUINT16 					usPortalSvcId;
	SvcSi_ServiceInfoTable_t			stPortalSvcInfoTable;
#endif
} svcSi_LcnUpdateData_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HERROR xsvc_si_LcnUpdateSrchParseSgt_Ses(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction);
STATIC HERROR xsvc_si_LcnUpdateSrchGetServiceListIds_Ses(Handle_t hAction, HUINT32 *pulSvcListNum, HUINT16 **ppusSvcListIdArray);
STATIC HERROR xsvc_si_LcnUpdateSrchFreeServiceListIdArray_Ses(HUINT16 *pusSvcListIdArray);
STATIC HERROR xsvc_si_LcnUpdateSrchGetServiceListBySvcListId_Ses(Handle_t hAction, HUINT16 usSvcListId, SI_SesLcnSvcList_t *pstSvcList);
STATIC HERROR xsvc_si_LcnUpdateSrchParseAstraPortal_Ses(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction);

#define ___LOCAL_FUNCTIONS___
STATIC HERROR xsvc_si_LcnUpdateSrchInitSearchData (svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, SvcSi_SiSpec_t *punSiSpec, SvcSi_ExtSearchOption_t *pstExtSrchOption)
{
	HUINT32 			 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32 			 ulDemuxId;
	HERROR				 hErr;

	if ((NULL == pstLcnUpdate) || (NULL == punSiSpec) || (NULL == pstExtSrchOption))
	{
		return ERR_FAIL;
	}

	hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_PIPE_GetResourceId: ulActionId(%d), eRxRSCTYPE_DEMUX\n", ulActionId);
		return ERR_FAIL;
	}

	HxSTD_MemSet (pstLcnUpdate, 0, sizeof(svcSi_SrchData_t));

	// set search base data - 사용할 것만 init
	pstLcnUpdate->stBase.hAction = hAction;
	HxSTD_MemCopy (&(pstLcnUpdate->stBase.unSiSpec), punSiSpec, sizeof(SvcSi_SiSpec_t));
	pstLcnUpdate->stBase.ulDemuxId = ulDemuxId;

	// set ext search data
//	HxSTD_MemCopy (&(pstLcnUpdate->stExtSrchOption), pstExtSrchOption, sizeof(SvcSi_ExtSearchOption_t));

	pstLcnUpdate->eSrchType = (SvcSi_SearchType_e)pstExtSrchOption->ulExtSearchType;

	pstLcnUpdate->bAllSearch = (HBOOL)pstExtSrchOption->usExtId1;
	pstLcnUpdate->usSvcListId	= pstExtSrchOption->usExtId2;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	if(pstLcnUpdate->eSrchType == eSVCSI_SEARCHTYPE_SES_ASTRA_PORTAL_TP
		|| pstLcnUpdate->eSrchType == eSVCSI_SEARCHTYPE_SES_ASTRA_REGIONAL_PORTAL_TP)
	{
		pstLcnUpdate->usPortalSvcId	= pstExtSrchOption->usExtId3;
	}
	else
	{
		pstLcnUpdate->usSgtSvcId	= pstExtSrchOption->usExtId3;
	}
#else
	pstLcnUpdate->usSgtSvcId	= pstExtSrchOption->usExtId3;
#endif

	pstLcnUpdate->stBase.ulSearchResult = eSVCSI_SEARCH_RESULT_None;

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchCheckSiTableReceived (svcSi_SrchData_t *pstSrchData, SIxTable_Type_e eTableType, HUINT32 ulExtId, HUINT32 *pulFilterStatus)
{
	svcSi_FilteringInfo_t	*pstFilterInfo = NULL;

	pstFilterInfo = svc_si_SrchFindFilterInfoByTableTypeAndExtId(pstSrchData, eTableType, ulExtId);
	if(NULL == pstFilterInfo)
	{
		HxLOG_Error ("No Such Filter...\n");
		return ERR_FAIL;
	}

	*pulFilterStatus = pstFilterInfo->eFilterStatus;

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchFreeSvcListArray (SI_SesLcnSvcList_t *pstSvcList)
{
	if (pstSvcList == NULL)					{ return ERR_FAIL; }

	if (pstSvcList->stBqInfoTable.pstBqArray != NULL)
	{
		OxSI_Free (pstSvcList->stBqInfoTable.pstBqArray);
	}

	if (pstSvcList->stNetInfoTable.pstNetworkArray != NULL)
	{
		OxSI_Free (pstSvcList->stNetInfoTable.pstNetworkArray);
	}

	if (pstSvcList->stTsInfoTable.pstTsArray != NULL)
	{
		OxSI_Free (pstSvcList->stTsInfoTable.pstTsArray);
	}

	if (pstSvcList->stSvcInfoTable.pstSvcArray != NULL)
	{
		OxSI_Free (pstSvcList->stSvcInfoTable.pstSvcArray);
	}

	if (pstSvcList->stPrvInfoTable.pstProviderArray != NULL)
	{
		OxSI_Free (pstSvcList->stPrvInfoTable.pstProviderArray);
	}

	HxSTD_memset (pstSvcList, 0, sizeof(SI_SesLcnSvcList_t));

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchClearSearchTable (svcSi_LcnUpdateData_t *pstLcnUpdate)
{
	HUINT32			 ulSvcListCnt;

	if (pstLcnUpdate != NULL)
	{
		if (pstLcnUpdate->pstSvcListArray != NULL)
		{
			for (ulSvcListCnt = 0; ulSvcListCnt < pstLcnUpdate->ulSvcListNum; ulSvcListCnt ++)
			{
				xsvc_si_LcnUpdateSrchFreeSvcListArray (&(pstLcnUpdate->pstSvcListArray[ulSvcListCnt]));
			}

			OxSI_Free (pstLcnUpdate->pstSvcListArray);
		}

		HxSTD_memset (pstLcnUpdate, 0, sizeof(svcSi_LcnUpdateData_t));
	}

    return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchCheckAndComposePatData_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	HERROR			hErr = ERR_FAIL;
	HUINT32 		ulActionId = PAL_PIPE_GetActionId (pstMsg->hAction);
	HUINT16			usPmtPid = PID_NULL;

	switch(pstLcnUpdate->eSrchType)
	{
		case eSVCSI_SEARCHTYPE_SES_LCN_SGT_TP:
			if(pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			hErr = svc_si_LibGetPmtPidFromPat (ulActionId, pstLcnUpdate->usSgtSvcId, &usPmtPid);
			if(hErr != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			hErr = svc_si_SrchStartFilter (&pstLcnUpdate->stBase, eSIxTABLETYPE_Pmt, (HUINT32)pstLcnUpdate->usSgtSvcId, (HUINT32)usPmtPid);
			if(hErr != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			return ERR_OK;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
		case eSVCSI_SEARCHTYPE_SES_ASTRA_PORTAL_TP:
			if (svc_si_SrchCheckAllSiTableReceived (&pstLcnUpdate->stBase) != TRUE)
			{
				break;
			}

			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			hErr = xsvc_si_LcnUpdateSrchParseAstraPortal_Ses(pstLcnUpdate, pstMsg->hAction);
			if (hErr == ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_DONE, pstMsg->hAction, 0, 0, 0);
			}
			else
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
			}

			return ERR_OK;
#endif

		default:
			return ERR_FAIL;
	}
	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchCheckAndComposePmtData_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	HERROR					hErr = ERR_FAIL;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	HUINT32					ulSvcListId;
	HxList_t				*pstList = NULL;
	SIxTable_PsiTable_t		*pstPmtTable = NULL;

	switch(pstLcnUpdate->eSrchType)
	{
		case eSVCSI_SEARCHTYPE_SES_LCN_SGT_TP:
			pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, pstLcnUpdate->usSgtSvcId);
			if(pstPmtTable == NULL)
			{
				return ERR_FAIL;
			}

			// parse 2nd des loop
			for (pstList = pstPmtTable->pst2ndLoop; pstList != NULL; pstList = pstList->next)
			{
				SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstList);
				SIxTable_PmtElement_t	*pstPmtElem;

				if (pst2ndItem == NULL)
				{
					HxLOG_Print("pst2ndItem == NULL, continue \n");
					continue;
				}

				pstPmtElem = &(pst2ndItem->unInfo.stPmt);

				if(pstPmtElem->ucStreamType == ISO_PRIVDATA_1)
				{
					// SGT PID Found : Set the filter

					ulSvcListId = (pstLcnUpdate->bAllSearch == TRUE) ? SGT_ALL_FILTERING : (HUINT32)pstLcnUpdate->usSvcListId;

					hErr = svc_si_SrchStartFilter (&pstLcnUpdate->stBase, eSIxTABLETYPE_Sgt, ulSvcListId, (HUINT32)pstPmtElem->usElemPid);
					if(hErr == ERR_OK)
					{
						PAL_SIRAWPOOL_FreeTable(pstPmtTable);
						return ERR_OK;
					}
				}
			}

			// PMT에 해당 정보가 없다 :
			svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);

			PAL_SIRAWPOOL_FreeTable(pstPmtTable);
			return ERR_OK;

		default:
			return ERR_FAIL;

	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchCheckAndComposeNitData_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	switch(pstLcnUpdate->eSrchType)
	{
		case eSVCSI_SEARCHTYPE_SES_LCN_HOME_TP:
			if (svc_si_SrchCheckAllSiTableReceived (&pstLcnUpdate->stBase) != TRUE)
			{
				break;
			}

			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			svc_si_PostMsgToAp (eSEVT_SI_SEARCH_DONE, pstMsg->hAction, 0, 0, 0);
			return ERR_OK;

		case eSVCSI_SEARCHTYPE_SES_LCN_REFNIT_TP:
			// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			svc_si_PostMsgToAp (eSEVT_SI_SEARCH_DONE, pstMsg->hAction, 0, 0, 0);
			return ERR_OK;

		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchCheckAndComposeTdtData_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	HERROR					hErr;
	HUINT32					ulNitFilterStatus;
	SIxTable_PsiTable_t		*pstTdtTable;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);

	switch (pstLcnUpdate->eSrchType)
	{
	case eSVCSI_SEARCHTYPE_SES_LCN_HOME_TP:
#if 1	// TODO: TDT 셋팅
		pstTdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Tdt, SILIB_TABLE_EXT_ID_NOT);
		if (NULL != pstTdtTable)
		{
			svc_si_LibUpdateTimeByTdt (ulActionId, HANDLE_NULL, pstTdtTable);
			PAL_SIRAWPOOL_FreeTable (pstTdtTable);
		}
#endif

		if (svc_si_SrchCheckAllSiTableReceived (&pstLcnUpdate->stBase) != TRUE)
		{
			break;
		}

		// NIT 성사 여부에 Success / Fail로 나뉜다
		hErr = xsvc_si_LcnUpdateSrchCheckSiTableReceived(&pstLcnUpdate->stBase, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_NOT, &ulNitFilterStatus);
		if ((hErr == ERR_FAIL) || (hErr == ERR_OK && ulNitFilterStatus == eSI_TABLE_TIMEOUT))
		{
			svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
			return ERR_OK;
		}

		svc_si_PostMsgToAp (eSEVT_SI_SEARCH_DONE, pstMsg->hAction, 0, 0, 0);
		return ERR_OK;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchCheckAndComposeSgtData_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	HERROR			hErr = ERR_FAIL;

	switch (pstLcnUpdate->eSrchType)
	{
		case eSVCSI_SEARCHTYPE_SES_LCN_SGT_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			hErr = xsvc_si_LcnUpdateSrchParseSgt_Ses(pstLcnUpdate, pstMsg->hAction);
			if (hErr == ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_DONE, pstMsg->hAction, 0, 0, 0);
			}
			else
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
			}

		default:
			return ERR_FAIL;
	}
	return ERR_OK;
}

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
STATIC HERROR xsvc_si_LcnUpdateSrchParseAstraPortal_Ses(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction)
{
	HERROR						hErr = ERR_FAIL;
	HUINT32						ulActionId = PAL_PIPE_GetActionId(hAction);
	SIxTable_PsiTable_t			*pstSdtTable = NULL, *pstTable = NULL;
	HxList_t					*pstList = NULL;
	SIxSvcDes_t 				*pstSvcDes = NULL;
	SIxTable_SdtService_t		*pstSvcInfo = NULL;
	DbSvc_Info_t				stSvcInfo;
	HBOOL						bNameDiff;
	HUINT16						usPmtPid;
	HUINT32						ulSvcIndex;

	pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	if (pstSdtTable == NULL)
	{
		return ERR_FAIL;
	}

	pstTable = pstSdtTable;

	while(pstTable != NULL)
	{
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

				pstSvcInfo = &(pst2ndItem->unInfo.stSdt);

				if(*pucRawDes == eSIxDESCTAG_SERVICE)
				{
					pstSvcDes = (SIxSvcDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if (pstSvcDes != NULL)
					{
						if(pstSvcInfo->usSvcId == pstLcnUpdate->usPortalSvcId)
						{
							DxSvcType_e		eSvcType;
							DxVideoType_e	eVideoType;


							VK_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));

							// Service ID:
							DbSvc_SetSvcId(&stSvcInfo, pstSvcInfo->usSvcId);

							// Service Name:
							svc_si_LibCheckServiceNameBySvcDes(pstSvcDes->ucSvcName, pstSvcDes->pszServName, DbSvc_GetName(&stSvcInfo), &bNameDiff);

							// PMT PID
							hErr = svc_si_LibGetPmtPidFromPat (ulActionId, DbSvc_GetSvcId(&stSvcInfo), &usPmtPid);
							DbSvc_SetPmtPid(&stSvcInfo, (hErr == ERR_OK) ? usPmtPid : PID_NULL);

							// Humax Service Type(TV/RADIO/DATA) & Video Type(SD/HD)
							eSvcType = eDxSVC_TYPE_All;
							eVideoType = eDxVIDEO_TYPE_ALL;
							hErr = svc_si_LibGetSvcAndVideoTypeFromDvbSvcType(ulActionId, (HUINT32)pstSvcDes->ucServType, &eSvcType, &eVideoType);
							DbSvc_SetSvcType(&stSvcInfo, eSvcType);
							DbSvc_SetVideoType(&stSvcInfo, eVideoType);
							DbSvc_SetOrgSvcType(&stSvcInfo, DbSvc_GetSvcType(&stSvcInfo));

							if (hErr == ERR_OK)
							{
								svc_si_LibAddSvcInfoToSvcInfoTable (&stSvcInfo, TRUE, &(pstLcnUpdate->stPortalSvcInfoTable), &ulSvcIndex);
#if 0//defined(CONFIG_DEBUG)
								HxLOG_Print("\n\t[ASTRA PORTAL Svc_info_t]\n");
								HxLOG_Print("\tSvcID (0x%x) SvcName(%s) \n", DbSvc_GetSvcId(&stSvcInfo), (char *)DbSvc_GetName(&stSvcInfo));
								HxLOG_Print("\tPmt PID (0x%x) Humax Service Type(%d) \n\n", DbSvc_GetPmtPid(stSvcInfo), DbSvc_GetSvcType(&stSvcInfo));
#endif
							}
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcDes);
					}
				}
			}
		}

		pstTable = pstTable->pstNextTable;
	}

	PAL_SIRAWPOOL_FreeTable(pstSdtTable);

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchCheckAndComposeBatData_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	switch (pstLcnUpdate->eSrchType)
	{
		case eSVCSI_SEARCHTYPE_SES_ASTRA_PORTAL_REFNIT_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			svc_si_PostMsgToAp (eSEVT_SI_SEARCH_DONE, pstMsg->hAction, 0, 0, 0);

		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchCheckAndComposeSdtData_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	HERROR			hErr = ERR_FAIL;

	switch (pstLcnUpdate->eSrchType)
	{
		case eSVCSI_SEARCHTYPE_SES_ASTRA_PORTAL_TP:
			if (svc_si_SrchCheckAllSiTableReceived (&pstLcnUpdate->stBase) != TRUE)
			{
				break;
			}

			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
				return ERR_OK;
			}

			hErr = xsvc_si_LcnUpdateSrchParseAstraPortal_Ses(pstLcnUpdate, pstMsg->hAction);
			if (hErr == ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_DONE, pstMsg->hAction, 0, 0, 0);
			}
			else
			{
				svc_si_PostMsgToAp (eSEVT_SI_SEARCH_FAIL, pstMsg->hAction, 0, 0, 0);
			}

			return ERR_OK;

		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

STATIC HERROR xsvc_si_LcnUpdateSrchParseSgt_Ses(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction)
{
	HERROR			hErr = ERR_FAIL;
	HUINT32			ulSvcListNum = 0, ulSvcListCnt = 0;
	HUINT16			*pusSvcListIdArray = NULL;

	// 1. Count the service lists
	hErr = xsvc_si_LcnUpdateSrchGetServiceListIds_Ses(hAction, &ulSvcListNum, &pusSvcListIdArray);
	if(hErr != ERR_OK || ulSvcListNum == 0 || pusSvcListIdArray == NULL)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstLcnUpdate->pstSvcListArray = (SI_SesLcnSvcList_t *)OxSI_Malloc (sizeof(SI_SesLcnSvcList_t) * ulSvcListNum);
	if(pstLcnUpdate->pstSvcListArray == NULL)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	HxSTD_MemSet(pstLcnUpdate->pstSvcListArray, 0, sizeof(SI_SesLcnSvcList_t) * ulSvcListNum);

	pstLcnUpdate->ulSvcListNum = ulSvcListNum;

	for (ulSvcListCnt = 0; ulSvcListCnt < ulSvcListNum; ulSvcListCnt ++)
	{
		xsvc_si_LcnUpdateSrchGetServiceListBySvcListId_Ses (hAction, pusSvcListIdArray[ulSvcListCnt], &(pstLcnUpdate->pstSvcListArray[ulSvcListCnt]));
	}

	hErr = ERR_OK;

END_FUNC:
	if (pusSvcListIdArray != NULL)
	{
		xsvc_si_LcnUpdateSrchFreeServiceListIdArray_Ses (pusSvcListIdArray);
	}

	return hErr;
}

STATIC HERROR xsvc_si_LcnUpdateSrchGetServiceListIds_Ses(Handle_t hAction, HUINT32 *pulSvcListNum, HUINT16 **ppusSvcListIdArray)
{
	HERROR					hErr = ERR_FAIL;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);

	HUINT32					ulSvcListNum = 0, ulSvcListCnt = 0;
	HUINT32					ulIndex = 0, ulSubIndex = 0;
	HUINT32					ulLastSvcListId = 0xffffffff;
	POINTER_LIST_T			*pstIdLists = NULL;
	HUINT32					*pulSvcListId = NULL;
	HUINT16					*pusSvcListIdArray = NULL;
	HUINT16				 	usTempSvcListId;

	SIxTable_PsiTable_t		*pstSgtTable = NULL, *pstTable = NULL;
	SIxTable_SgtSec_t		*pstSgtSec = NULL;

	if(pulSvcListNum == NULL || ppusSvcListIdArray == NULL)		{ return ERR_FAIL; }

	pstSgtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Sgt, SILIB_TABLE_EXT_ID_ALL);
	if(pstSgtTable == NULL)
	{
		return ERR_FAIL;
	}

	pstTable = pstSgtTable;

	while(pstTable != NULL)
	{
		pstSgtSec = &(pstTable->unInfo.stSgt);

		if(ulLastSvcListId != pstSgtSec->usServiceListId)
		{
			pulSvcListId = (HUINT32 *)OxSI_Malloc (sizeof(HUINT32));
			if(pulSvcListId != NULL)
			{
				*pulSvcListId = (HUINT32)pstSgtSec->usServiceListId;
				ulLastSvcListId = *pulSvcListId;

				ulSvcListNum++;
				pstIdLists = UTIL_LINKEDLIST_AppendListItemToPointerList (pstIdLists, (void *)pulSvcListId);
			}
		}

		pstTable = pstTable->pstNextTable;
	}

	if(ulSvcListNum == 0)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pusSvcListIdArray = (HUINT16 *)OxSI_Malloc (sizeof(HUINT16) * ulSvcListNum);
	if(pusSvcListIdArray == NULL)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	while(pstIdLists != NULL)
	{
		pulSvcListId = (HUINT32 *)UTIL_LINKEDLIST_GetContents(pstIdLists);
		if(pulSvcListId != NULL)
		{
			pusSvcListIdArray[ulSvcListCnt] = (HUINT16)(*pulSvcListId);
			ulSvcListCnt++;

			OxSI_Free(pulSvcListId);
		}

		pstIdLists = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstIdLists, pstIdLists, NULL);
	}

	// Sort the Service List IDs
	for (ulIndex = 1; ulIndex < ulSvcListCnt; ulIndex ++)
	{
		for (ulSubIndex = 0; ulSubIndex < ulIndex; ulSubIndex ++)
		{
			if (pusSvcListIdArray[ulSubIndex] > pusSvcListIdArray[ulIndex])
			{
				usTempSvcListId = pusSvcListIdArray[ulSubIndex];
				pusSvcListIdArray[ulSubIndex] = pusSvcListIdArray[ulIndex];
				pusSvcListIdArray[ulIndex] = usTempSvcListId;
			}
		}
	}

	*pulSvcListNum = ulSvcListCnt;
	*ppusSvcListIdArray = pusSvcListIdArray;

	hErr = ERR_OK;

END_FUNC:
	while (pstIdLists != NULL)
	{
		pulSvcListId = (HUINT32 *)UTIL_LINKEDLIST_GetContents(pstIdLists);
		if (pulSvcListId != NULL)
		{
			OxSI_Free (pulSvcListId);
		}

		pstIdLists = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstIdLists, pstIdLists, NULL);
	}

	return hErr;
}

STATIC HERROR xsvc_si_LcnUpdateSrchFreeServiceListIdArray_Ses(HUINT16 *pusSvcListIdArray)
{
	if (pusSvcListIdArray != NULL)
	{
		OxSI_Free (pusSvcListIdArray);
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchGetServiceListBySvcListId_Ses(Handle_t hAction, HUINT16 usSvcListId, SI_SesLcnSvcList_t *pstSvcList)
{
	HERROR					hErr = ERR_FAIL;
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	HBOOL					bSvcListName = FALSE, bNameUpdated;
	HxLANG_Id_e				eMenuLang = eLangID_NONE, eSvcListLang = eLangID_NONE;
	SIxTable_PsiTable_t		*pstSgtTable = NULL;
	SIxTable_SgtSec_t		*pstSgtSvcListInfo = NULL;
	SIxTable_SgtService_t	*pstSvcInfo = NULL;
	DbSvc_Info_t			stSvcInfo;
	DbSvc_BouqInfo_t		stBqInfo;
	HxList_t				*pstList = NULL;
	HUINT32					ulValue;
	HUINT16					usBqIdx;
	HUINT32					ulSvcIndex;
	DxCasType_e				eCasType, eCasTypeInSec, eTempCasType;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&ulValue, 0);
	eMenuLang = (hErr == ERR_OK) ? (HxLANG_Id_e)ulValue : eLangID_English;

	pstSgtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Sgt, usSvcListId);
	if (pstSgtTable == NULL)
	{
		return ERR_FAIL;
	}

	pstSgtSvcListInfo = &(pstSgtTable->unInfo.stSgt);

	pstSvcList->usSvcListId = pstSgtSvcListInfo->usServiceListId;
	pstSvcList->ucSecVer = pstSgtSvcListInfo->ucVer;
	eCasTypeInSec = 0;

	HxLOG_Print("[xsvc_si_LcnUpdateSrchGetServiceListBySvcListId_Ses] service list id : 0x%x, version : 0x%x, pstSgtTable->pstDesLoop : 0x%x\n", pstSvcList->usSvcListId, pstSvcList->ucSecVer, pstSgtTable->pstDesLoop);

	// parse 1st des loop
	for (pstList = pstSgtTable->pstDesLoop; pstList != NULL; pstList = pstList->next)
	{
		HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);

		if (pucRawDes == NULL)
		{
			HxLOG_Print("pucRawDes == NULL, continue \n");
			continue;
		}

		HxLOG_Print("start 1st loop desc parsing, des : 0x%x \n", *pucRawDes);

		switch(*pucRawDes)
		{
			case eSIxDESCTAG_SES_SERVICE_LIST_NAME:
				{
					SIxServiceListNameDes_t		*pstDesc = NULL;
					pstDesc = (SIxServiceListNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if (pstDesc != NULL)
					{
						if(bSvcListName != TRUE)
						{
							HxLOG_Print("\t\t\t[eSIxDESCTAG_SES_SERVICE_LIST_NAME] pszSvcListName : %s\n", pstDesc->pszSvcListName);
							MWC_UTIL_DvbStrncpy (pstSvcList->szSvcListName, pstDesc->pszSvcListName, MAX_SVCLIST_NAME_LEN);
							pstSvcList->szSvcListName[MAX_SVCLIST_NAME_LEN - 1] = '\0';
							bSvcListName = TRUE;
						}
						else
						{
							hErr = MWC_UTIL_GetLangIDBy639Code (pstDesc->szIso639LangCode, &eSvcListLang);
							if (hErr != ERR_OK)				{ eSvcListLang = eLangID_NONE; }

							if (eMenuLang == eSvcListLang)
							{
								HxLOG_Print("\t\t\t[eSIxDESCTAG_SES_SERVICE_LIST_NAME] pszSvcListName : %s\n", pstDesc->pszSvcListName);
								MWC_UTIL_DvbStrncpy (pstSvcList->szSvcListName, pstDesc->pszSvcListName, MAX_SVCLIST_NAME_LEN);
								pstSvcList->szSvcListName[MAX_SVCLIST_NAME_LEN - 1] = '\0';
							}
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
						pstDesc = NULL;
					}
					else
					{
						HxLOG_Print("\t\t [eSIxDESCTAG_SES_SERVICE_LIST_NAME] parse FAIL !! ");
					}
				}
				break;

			case eSIxDESCTAG_CA_IDENTIFIER:
				{
					SIxCaIdentDes_t		*pstDesc = NULL;
					pstDesc = (SIxCaIdentDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if (pstDesc != NULL)
					{
						SIxCaIdentInfo_t			*pstInfo = pstDesc->pstInfo;

						while (pstInfo != NULL)
						{
							eCasType = 0;
							 svc_si_LibConvertSiCasIdToCasType((HUINT32)pstInfo->usCaStmId, &eCasType);
							eCasTypeInSec |= eCasType;
							HxLOG_Print("\t\t\t[eSIxDESCTAG_CA_IDENTIFIER] usCaStmId : 0x%x, eCasType : 0x%x, eCasTypeInSec : 0x%x\n", pstInfo->usCaStmId, eCasType, eCasTypeInSec);

							pstInfo = pstInfo->pstNext;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
					}
					else
					{
						HxLOG_Print("\t\t [eSIxDESCTAG_CA_IDENTIFIER] parse FAIL !! ");
					}
				}
				break;

			case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:
			default:
				HxLOG_Print("Unknown 1st loop descriptor (0x%X)\n", *pucRawDes);
				break;
		}
	}

	// parse 2nd des loop
	for (pstList = pstSgtTable->pst2ndLoop; pstList != NULL; pstList = pstList->next)
	{
		SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstList);
		HxList_t				*pst2ndDesLoop = NULL;

		if (pst2ndItem == NULL)
		{
			HxLOG_Print("pst2ndItem == NULL, continue \n");
			continue;
		}

		HxLOG_Print("start 2nd loop desc parsing \n");

		pstSvcInfo = &(pst2ndItem->unInfo.stSgt);

		DB_SVC_InitServiceInfo(&stSvcInfo);

		DbSvc_SetLcnFlag(&stSvcInfo, TRUE);
		DbSvc_SetOnId(&stSvcInfo, pstSvcInfo->usOnId);
		DbSvc_SetTsId(&stSvcInfo, pstSvcInfo->usTsId);
		DbSvc_SetSvcId(&stSvcInfo, pstSvcInfo->usServiceId);
		DbSvc_SetLcn(&stSvcInfo, pstSvcInfo->usLcn);
		DbSvc_SetSvcSection(&stSvcInfo, eDxSvcSection_Lcn);
#if defined(CONFIG_OP_SES)
		XdbSvc_SesSetNewSvc(stSvcInfo, (HBOOL)pstSvcInfo->ucNewSvcFlag);
#endif
		DbSvc_SetCasType(&stSvcInfo, eCasTypeInSec);
		DbSvc_SetVisibleFlag(&stSvcInfo, (HBOOL)pstSvcInfo->ucVisibleSvcFlag);
		
		eTempCasType = eDxCAS_TYPE_All;
		for (pst2ndDesLoop = pst2ndItem->pstDesLoop; pst2ndDesLoop != NULL; pst2ndDesLoop = pst2ndDesLoop->next)
		{
			HUINT8				*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pst2ndDesLoop);

			if (pucRawDes == NULL)
			{
				HxLOG_Print("pucRawDes == NULL, continue \n");
				continue;
			}

			switch (*pucRawDes)
			{
				case eSIxDESCTAG_SERVICE:
					{
						SIxSvcDes_t						*pstDesc = NULL;
						pstDesc = (SIxSvcDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
						if (pstDesc != NULL)
						{
							DxSvcType_e		eSvcType;
							DxVideoType_e	eVideoType;

							// Service Type
							HxLOG_Print("\t\t\t[eSIxDESCTAG_SERVICE] ucServType : 0x%x\n", pstDesc->ucServType);
							eSvcType = eDxSVC_TYPE_All;
							eVideoType = eDxVIDEO_TYPE_ALL;
							svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (ulActionId, (HUINT32)pstDesc->ucServType, &eSvcType, &eVideoType);

							DbSvc_SetSvcType(&stSvcInfo, eSvcType);
							DbSvc_SetVideoType(&stSvcInfo, eVideoType);
							DbSvc_SetOrgSvcType(&stSvcInfo, DbSvc_GetSvcType(&stSvcInfo));

							switch (pstDesc->ucServType)
							{
								case SERVICE_TYPE_HEVC_DIGITAL_TV:
									DbSvc_SetVideoCodec(&stSvcInfo, eDxVIDEO_CODEC_H265);
									break;
								
								case SERVICE_TYPE_AVC_SD_DIGITAL_TV:
								case SERVICE_TYPE_AVC_HD_DIGITAL_TV:
								case SERVICE_TYPE_AVC_HD_DIGITAL_TV2:
									DbSvc_SetVideoCodec(&stSvcInfo, eDxVIDEO_CODEC_H264);
									break;

								case SERVICE_TYPE_DIGITAL_TV:
								case SERVICE_TYPE_MPEG2_HD:
									DbSvc_SetVideoCodec(&stSvcInfo, eDxVIDEO_CODEC_MPEG2);
									break;

								default:
									break;
							}

							HxLOG_Print("\t\t\t[eSIxDESCTAG_SERVICE] pszServName : %s\n", pstDesc->pszServName);

							// Service Name
							svc_si_LibCheckServiceNameBySvcDes (pstDesc->ucSvcName, pstDesc->pszServName, DbSvc_GetName(&stSvcInfo), &bNameUpdated);

							// Provider Name (차후 구현)

							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
						}
					}
					break;

				case eSIxDESCTAG_CA_IDENTIFIER:
					{
						SIxCaIdentDes_t					*pstDesc = NULL;
						pstDesc = (SIxCaIdentDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
						if (pstDesc != NULL)
						{
							// TODO:
							SIxCaIdentInfo_t			*pstInfo = pstDesc->pstInfo;

							while (pstInfo != NULL)
							{
								eCasType = 0;
								svc_si_LibConvertSiCasIdToCasType((HUINT32)pstInfo->usCaStmId, &eCasType);
								eTempCasType |= eCasType;
								HxLOG_Print("\t\t\t[eSIxDESCTAG_CA_IDENTIFIER] usCaStmId : 0x%x, eCasType : 0x%x, eCasTypeInSec : 0x%x\n", pstInfo->usCaStmId, eCasType, eCasTypeInSec);

								pstInfo = pstInfo->pstNext;
							}

							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
						}
					}
					break;

				case eSIxDESCTAG_SES_BOUQUET_LIST:
					{
						SIxBouquetListDes_t				*pstDesc = NULL;
						pstDesc = (SIxBouquetListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
						if (pstDesc != NULL)
						{
							// TODO:
							SIxBouquetListInfo_t		*pstInfo = pstDesc->pstInfo;

							while (pstInfo != NULL)
							{
								HxSTD_MemSet(&stBqInfo, 0, sizeof(DbSvc_BouqInfo_t));

								HxLOG_Print("\t\t\t[eSIxDESCTAG_SES_BOUQUET_LIST] szBouqName : %s\n", pstInfo->pszBouquetName);
								MWC_UTIL_DvbStrncpy (stBqInfo.szBouqName, pstInfo->pszBouquetName, DxMAX_BOUQ_NAME_LEN);
								hErr = svc_si_LibAddBqInfoToBqInfoTable (&stBqInfo, FALSE, &(pstSvcList->stBqInfoTable), &usBqIdx);
								if (hErr == ERR_OK)
								{
#if defined(CONFIG_OP_SES)
									DB_SVC_AddBouquetIndexToSvcInfo (&stSvcInfo, usBqIdx);
#endif
								}

								pstInfo = pstInfo->pstNext;
							}

							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
						}
					}
					break;

				case eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID:
					{
						SIxVirtualServiceIdDes_t		*pstDesc = NULL;
						pstDesc = (SIxVirtualServiceIdDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
						if (pstDesc != NULL)
						{
#if defined(CONFIG_OP_SES)
							HxLOG_Print("\t\t\t[eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID] usVirtualServiceId : 0x%x\n", pstDesc->usVirtualServiceId);
							XdbSvc_SesSetVirtualSvcId(stSvcInfo, pstDesc->usVirtualServiceId);
#endif
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
						}
					}
					break;

				default:
					HxLOG_Print("\t\tUnknown 2nd loop descriptor (0x%X)\n", *pucRawDes);
					break;

			}
		}

		eTempCasType = (eTempCasType == eDxCAS_TYPE_All) ? eDxCAS_TYPE_FTA : eTempCasType;
		DbSvc_SetCasType(&stSvcInfo, eTempCasType);
		
		// 추가된 서비스를 넣는다.
		// HD가 제공되지 않는 STB에서는 HD 서비스가 추가되어선 안된다.
		if (DbSvc_GetSvcType(&stSvcInfo) != eDxSVC_TYPE_All)
		{
			svc_si_LibAddSvcInfoToSvcInfoTable (&stSvcInfo, FALSE, &(pstSvcList->stSvcInfoTable), &ulSvcIndex);
		}
	}

	PAL_SIRAWPOOL_FreeTable(pstSgtTable);

	return ERR_OK;
}

HERROR xsvc_si_LcnUpdateSrchGetFilteringTypes (svcSi_LcnUpdateData_t *pstLcnUpdate)
{
	if (NULL == pstLcnUpdate)
	{
		return ERR_FAIL;
	}

	switch (pstLcnUpdate->eSrchType)
	{
	case eSVCSI_SEARCHTYPE_SES_LCN_HOME_TP:
		pstLcnUpdate->stBase.eFiltering = bSI_FILTERING_NIT_ACT |  bSI_FILTERING_TDT;
		break;

	case eSVCSI_SEARCHTYPE_SES_LCN_REFNIT_TP:
		pstLcnUpdate->stBase.eFiltering = bSI_FILTERING_NIT_ACT;
		break;

	case eSVCSI_SEARCHTYPE_SES_LCN_SGT_TP:
		pstLcnUpdate->stBase.eFiltering = bSI_FILTERING_PAT;
		break;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	case eSVCSI_SEARCHTYPE_SES_ASTRA_PORTAL_REFNIT_TP: 	// Astra Portal TP
		pstLcnUpdate->stBase.eFiltering = bSI_FILTERING_BAT;
		pstLcnUpdate->stBase.usBatBouquetId = ASTRA_PORTAL_BOUQUET_ID;
		break;

	case eSVCSI_SEARCHTYPE_SES_ASTRA_PORTAL_TP: 			// Astra Portal TP
		pstLcnUpdate->stBase.eFiltering = bSI_FILTERING_PAT | bSI_FILTERING_SDT_ACT;
		break;

	case eSVCSI_SEARCHTYPE_SES_ASTRA_REGIONAL_PORTAL_TP:	// Astra Regional TP
		pstLcnUpdate->stBase.eFiltering = bSI_FILTERING_PAT | bSI_FILTERING_SDT_ACT;
		break;
#endif

	default:
//		SesLcnAssert (0);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC SI_SesLcnSvcList_t *xsvc_si_LcnUpdateSrchGetSelectedServiceList (svcSi_LcnUpdateData_t *pstLcnUpdate)
{
	HUINT32				 ulIndex;
	SI_SesLcnSvcList_t	*pstSvcList;

	for (ulIndex = 0; ulIndex < pstLcnUpdate->ulSvcListNum; ulIndex++)
	{
		pstSvcList = &(pstLcnUpdate->pstSvcListArray[ulIndex]);

		if (pstSvcList->usSvcListId == pstLcnUpdate->usSvcListId)
		{
			return pstSvcList;
		}
	}

	return NULL;
}

// eSVCSI_EXTSEARCH_DATA_SES_DELIVERY_TSINFO
// Delivery System : (TS Info)
STATIC HERROR xsvc_si_LcnUpdateSrchGetDeliveryTsInfo(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, HUINT32 *pulNumTs, void **ppvInfoArray)
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

// eSVCSI_EXTSEARCH_DATA_SES_NETWORK_NETINFO
// Network 정보를 가져 온다.
STATIC HERROR xsvc_si_LcnUpdateSrchGetNetworkInfo(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, HUINT32 *pulNumNet, void **ppvNetInfoArray)
{
	HUINT32				ulActionId;
#if 0		// not used
	DbSvc_TsInfo_t		stTsInfo;
#endif
	DbSvc_NetInfo_t		*pstNetInfo;
	HERROR			 	hErr = ERR_FAIL;

#if 1
	HUINT8				*pszName = NULL;
	HUINT32				ulLen = 0;
#endif

	pstNetInfo = (DbSvc_NetInfo_t *)OxSI_Malloc (sizeof(DbSvc_NetInfo_t));
	if (pstNetInfo == NULL)
	{
		return ERR_FAIL;
	}

	ulActionId = PAL_PIPE_GetActionId (hAction);

#if 0		// have to check (yjlim) -> sdt / pat도 걸어줘야 하나??
	// 새로 추가된 두 함수 모두 stTsInfo.usUniqueId == 0 으로 만드는데 괜찮은 지...??
	// 필요하다고 하면 start search할 때 ts unique id를 param으로 가져와서 pstLcnUpate context에 저장해두고 이를 넣어야 함 (OCTO 1.0엔 이렇게 구현됨)
	hErr = xsvc_si_LibSrchGetNetTsInfoFromNIT_Base (ulActionId, pstNetInfo, &stTsInfo);
	if (hErr != ERR_OK)
	{
		// NIT 에서 얻을 수 없으면 다른 Table을 통해 TsInfo 라도 채워 넣는다.
		hErr = xsvc_si_LibSrchComposeTsInfoFromSi_Base (ulActionId, pstNetInfo, &stTsInfo);
		if (hErr != ERR_OK)
		{
			OxSI_Free(pstNetInfo);
			return ERR_FAIL;
		}
	}
#else
	/* NIT가 없기 때문에 Dummy로 만들어 준다. */
	pstNetInfo->usNetId = 0x0;
	hErr = svc_si_LibGetUndefinedNetworkName(ulActionId, &pszName);
	if (hErr == ERR_OK && NULL != pszName)
	{
		ulLen = HxSTD_StrLen(pszName);
		if (ulLen > DxMAX_NET_NAME_LEN-1)
		{
			ulLen = DxMAX_NET_NAME_LEN-1;
		}
#if 0
		if (ulLen > (sizeof(pstNetInfo->szNetName) - 1))
		{
			ulLen = sizeof(pstNetInfo->szNetName) - 1;
		}
#endif
		HxSTD_memcpy(&pstNetInfo->szNetName[0], pszName, ulLen);
	}

	pstNetInfo->szNetName[ulLen] = '\0';
#endif

	*pulNumNet = 1;
	*ppvNetInfoArray = (void *)pstNetInfo;

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnUpdateSrchFindLinkageFromNit(Handle_t hAction, HUINT8 ucLinkType, HUINT32 *pulNumLink, DbSvc_TripleId_t **ppstTripleIdArray)
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
	if (pstNitTable != NULL)					{ PAL_SIRAWPOOL_FreeTable(pstNitTable); }

	pstDesList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstDesList, (FREE_FUNC)PAL_SIRAWPOOL_FreeDes);
	if(pstDesList != NULL)
	{
		HxLOG_Error("???????????????\n");
	}

	return hErr;
}

// eSVCSI_EXTSEARCH_DATA_SES_REFNITLINK_TRIPLEID:
// ASTRA Reference NIT 를 가리키는 Triple ID
STATIC HERROR xsvc_si_LcnUpdateSrchGetRefNitTripleId(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, HUINT32 *pulNumLink, void **ppvTripleIdArray)
{
	// TODO: NIT의 Linkage Descriptor가 ASTRA_REF_NIT_LINKAGE (0x04)인 놈의 triple id를 얻어온다.

	// sema get (?)
	xsvc_si_LcnUpdateSrchFindLinkageFromNit(hAction, ASTRA_REF_NIT_LINKAGE, pulNumLink, (DbSvc_TripleId_t **)ppvTripleIdArray);
	// sema release (?)

	return ERR_OK;
}

// eSVCSI_EXTSEARCH_DATA_SES_SGTLINK_TRIPLEID:
// SGT Service 를 가리키는 Triple ID
STATIC HERROR xsvc_si_LcnUpdateSrchGetSgtSvcTripleId(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, HUINT32 *pulNumLink, void **ppvTripleIdArray)
{
	// TODO: NIT의 Linkage Descriptor가 ASTRA_SGT_LINKAGE (0x90)인 놈의 triple id를 얻어온다.

	// sema get (?)
	xsvc_si_LcnUpdateSrchFindLinkageFromNit(hAction, ASTRA_SGT_LINKAGE, pulNumLink, (DbSvc_TripleId_t **)ppvTripleIdArray);
	// sema release (?)

	return ERR_OK;
}

// eSVCSI_EXTSEARCH_DATA_SES_SVCLISTINFO:
// Service List Info 을 가져온다
STATIC HERROR xsvc_si_LcnUpdateSrchGetSvcListNames(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, HUINT32 *pulNumSvcList, void **ppvInfoArray)
{
	HUINT32				 ulIndex;
	SvcSi_LcnSvcListInfo_t	*pstArray;

	if (pstLcnUpdate->ulSvcListNum == 0)				{ return ERR_FAIL; }

	pstArray = (SvcSi_LcnSvcListInfo_t *)OxSI_Malloc (sizeof(SvcSi_LcnSvcListInfo_t) * pstLcnUpdate->ulSvcListNum);
	if (pstArray == NULL)							{ return ERR_FAIL; }

	for (ulIndex = 0; ulIndex < pstLcnUpdate->ulSvcListNum; ulIndex++)
	{
		pstArray[ulIndex].usSvcListId	= pstLcnUpdate->pstSvcListArray[ulIndex].usSvcListId;
		pstArray[ulIndex].ucSecVer		= pstLcnUpdate->pstSvcListArray[ulIndex].ucSecVer;
		MWC_UTIL_DvbStrcpy (pstArray[ulIndex].szSvcListName, pstLcnUpdate->pstSvcListArray[ulIndex].szSvcListName);
	}

	*pulNumSvcList = pstLcnUpdate->ulSvcListNum;
	*ppvInfoArray  = pstArray;

	return ERR_OK;
}

// eSVCSI_EXTSEARCH_DATA_SES_SERVICE_SVCINFO
// 선택된 Service Info들을 가져 온다.
STATIC HERROR xsvc_si_LcnUpdateSrchGetServiceInfos(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, HUINT32 *pulNumSvcList, void **ppvInfoArray)
{
	HUINT32				 ulNumSvcList;
	DbSvc_Info_t			*pstSvcInfoArray;
	SI_SesLcnSvcList_t	*pstSvcList;

	pstSvcList = xsvc_si_LcnUpdateSrchGetSelectedServiceList (pstLcnUpdate);
	if (pstSvcList == NULL)							{ return ERR_FAIL; }

	ulNumSvcList = pstSvcList->stSvcInfoTable.ulSvcNum;
	if (ulNumSvcList == 0)							{ return ERR_FAIL; }

	pstSvcInfoArray = (DbSvc_Info_t *)OxSI_Malloc (sizeof(DbSvc_Info_t) * ulNumSvcList);
	if (pstSvcInfoArray == NULL)					{ return ERR_FAIL; }

	HxSTD_memcpy (pstSvcInfoArray, pstSvcList->stSvcInfoTable.pstSvcArray, sizeof(DbSvc_Info_t) * ulNumSvcList);

	*pulNumSvcList = ulNumSvcList;
	*ppvInfoArray = (void **)pstSvcInfoArray;

	return ERR_OK;
}

// eSVCSI_EXTSEARCH_DATA_SES_BOUQUET_BQINFO
// 선택된 Bouquet List 들을 가져온다
STATIC HERROR xsvc_si_LcnUpdateSrchGetBouquetInfos(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, HUINT32 *pulNumList, void **ppvInfoArray)
{
	HUINT32				 	ulNumBqList;
	DbSvc_BouqInfo_t		*pstBqInfoArray;

	SI_SesLcnSvcList_t		*pstSvcList;

	pstSvcList = xsvc_si_LcnUpdateSrchGetSelectedServiceList (pstLcnUpdate);
	if (pstSvcList == NULL)									{ return ERR_FAIL; }
	if (pstSvcList->stBqInfoTable.pstBqArray == NULL)		{ return ERR_FAIL; }

	ulNumBqList = pstSvcList->stBqInfoTable.ulBqNum;
	if (ulNumBqList == 0)							{ return ERR_FAIL; }

	pstBqInfoArray = (DbSvc_BouqInfo_t *)OxSI_Malloc (sizeof(DbSvc_BouqInfo_t) * ulNumBqList);
	if (pstBqInfoArray == NULL)						{ return ERR_FAIL; }

	HxSTD_memcpy (pstBqInfoArray, pstSvcList->stBqInfoTable.pstBqArray, sizeof(DbSvc_BouqInfo_t) * ulNumBqList);

	*pulNumList		= ulNumBqList;
	*ppvInfoArray	= (void **)pstBqInfoArray;

	return ERR_OK;
}

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
STATIC HERROR xsvc_si_LcnUpdateSrchFindAstraPortalFromBat(Handle_t hAction, HUINT8 ucLinkType, HUINT32 *pulNumLink, DbSvc_TripleId_t **ppstTripleIdArray)
{
	HUINT32					ulActionId = PAL_PIPE_GetActionId(hAction);
	SIxTable_PsiTable_t		*pstBatTable = NULL, *pstTable = NULL;
	HxList_t				*pstList = NULL;
	SIxLinkDes_t 			*pstLinkDes = NULL;
	HUINT32					 ulIdx = 0, ulCntSsuSvc = 0, ulNumSsuSvc = 0;
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
		if(pstTable->unInfo.stBat.usBqId != ASTRA_PORTAL_BOUQUET_ID)
		{
			continue;
		}

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
						HxLOG_Print("[SES_LCN] 1.BAT : Portal LINK(0x%02x) Found (%04x:%04x:0x%04x)\n", pstLinkDes->ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
						ulNumSsuSvc++;
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
							HxLOG_Print("[SES_LCN] 2.BAT : Portal LINK(0x%02x) Found (%04x:%04x:0x%04x)\n", pstLinkDes->ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
							ulNumSsuSvc++;
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

	if (ulNumSsuSvc == 0)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (pulNumLink == NULL && ppstTripleIdArray == NULL)
	{
		hErr = ERR_OK;
		goto END_FUNC;
	}

	pstTripleIdArray = (DbSvc_TripleId_t *)OxSI_Malloc (sizeof(DbSvc_TripleId_t) * ulNumSsuSvc);
	if (pstTripleIdArray == NULL)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	while (pstDesList != NULL && ulCntSsuSvc < ulNumSsuSvc)
	{
		pstLinkDes = (SIxLinkDes_t *)pstDesList->pvContents;
		if (pstLinkDes != NULL)
		{
			// pstTripleIdArray 내에 이미 들어 있는 Triple ID 인가?
			for (ulIdx = 0; ulIdx < ulCntSsuSvc; ulIdx++)
			{
				if (pstTripleIdArray[ulIdx].usTsId == pstLinkDes->usTsId &&
					pstTripleIdArray[ulIdx].usOnId == pstLinkDes->usOrgNetId &&
					pstTripleIdArray[ulIdx].usSvcId == pstLinkDes->usSvcId)
				{
					break;
				}
			}

			// pstTripleIdArray 안에 미리 들어 있는 동일한 Triple ID가 없다 : New One
			if (ulIdx == ulCntSsuSvc)
			{
				pstTripleIdArray[ulCntSsuSvc].usOnId  = pstLinkDes->usOrgNetId;
				pstTripleIdArray[ulCntSsuSvc].usTsId  = pstLinkDes->usTsId;
				pstTripleIdArray[ulCntSsuSvc].usSvcId = pstLinkDes->usSvcId;

				ulCntSsuSvc++;
			}

			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
		}

		pstDesList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstDesList, pstDesList, NULL);
	}

	if (ulCntSsuSvc == 0)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (pulNumLink != NULL)
	{
		*pulNumLink = ulCntSsuSvc;
	}

	if (ppstTripleIdArray != NULL)
	{
		*ppstTripleIdArray = pstTripleIdArray;
		pstTripleIdArray = NULL;
	}

	hErr = ERR_OK;

END_FUNC:
	if (pstTripleIdArray != NULL) 				{ OxSI_Free (pstTripleIdArray); }
	if (pstBatTable != NULL)					{ PAL_SIRAWPOOL_FreeTable(pstBatTable); }

	pstDesList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstDesList, (FREE_FUNC)PAL_SIRAWPOOL_FreeDes);
	if(pstDesList != NULL)
	{
		HxLOG_Error("???????????????\n");
	}

	return hErr;
}

// eSVCSI_EXTSEARCH_DATA_SES_ASTRA_PORTAL_TS_INFO
STATIC HERROR xsvc_si_LcnUpdateSrchGetAstraPortalTripleId(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, HUINT32 *pulNumLink, void **ppvTripleIdArray)
{
	xsvc_si_LcnUpdateSrchFindAstraPortalFromBat (hAction, ASTRA_PORTAL_LINKAGE, pulNumLink, (DbSvc_TripleId_t **)ppvTripleIdArray);

	return ERR_OK;
}

// eSVCSI_EXTSEARCH_DATA_SES_ASTRA_REGIONAL_PORTAL_TS_INFO
STATIC HERROR xsvc_si_LcnUpdateSrchGetAstraRegionalPortalTripleId(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, HUINT32 *pulNumLink, void **ppvTripleIdArray)
{
	xsvc_si_LcnUpdateSrchFindAstraPortalFromBat (hAction, ASTRA_REGIONAL_PORTAL_LINKAGE, pulNumLink, (DbSvc_TripleId_t **)ppvTripleIdArray);

	return ERR_OK;
}

// eSVCSI_EXTSEARCH_DATA_SES_PORTAL_INFO
STATIC HERROR xsvc_si_LcnUpdateSrchGetPortalServiceInfos(svcSi_LcnUpdateData_t *pstLcnUpdate, Handle_t hAction, HUINT32 *pulNumSvcList, void **ppvInfoArray)
{
	HUINT32						ulNumSvcList;
	DbSvc_Info_t				*pstSvcInfoArray;
	SvcSi_ServiceInfoTable_t	*pstSvcList;

	pstSvcList = &(pstLcnUpdate->stPortalSvcInfoTable);
	if (pstSvcList == NULL)							{ return ERR_FAIL; }

	ulNumSvcList = pstSvcList->ulSvcNum;
	if (ulNumSvcList == 0)							{ return ERR_FAIL; }

	pstSvcInfoArray = (DbSvc_Info_t *)OxSI_Malloc (sizeof(DbSvc_Info_t) * ulNumSvcList);
	if (pstSvcInfoArray == NULL)					{ return ERR_FAIL; }

	HxSTD_memcpy (pstSvcInfoArray, pstSvcList->pstSvcArray, sizeof(DbSvc_Info_t) * ulNumSvcList);

	*pulNumSvcList = ulNumSvcList;
	*ppvInfoArray = (void **)pstSvcInfoArray;

	return ERR_OK;
}
#endif


#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_LcnUpdateSrchMsgCmdStartSearch_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LcnUpdateSrchInitSearchData (pstLcnUpdate, pstMsg->hAction, &(pstMsg->unArg.stStartExtSrch.unSpec), &(pstMsg->unArg.stStartExtSrch.stOption));
	(void)svc_si_SrchInitSearchResultTable (&pstLcnUpdate->stBase);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstMsg->hAction), eSIxSPEC_SES);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstMsg->hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)xsvc_si_LcnUpdateSrchGetFilteringTypes (pstLcnUpdate);
	(void)svc_si_SrchStartSection (&pstLcnUpdate->stBase);
}

STATIC void xsvc_si_LcnUpdateSrchMsgCmdStopSearch_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_SrchStopSection (&pstLcnUpdate->stBase);
	(void)svc_si_SrchFreeSearchData (&pstLcnUpdate->stBase);
	(void)xsvc_si_LcnUpdateSrchClearSearchTable (pstLcnUpdate);
}

STATIC void xsvc_si_LcnUpdateSrchMsgEvtPatTable_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
//	(void)xsvc_si_LibSrchOnPatReceived_Base (&pstLcnUpdate->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstLcnUpdate->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LcnUpdateSrchCheckAndComposePatData_Ses (pstLcnUpdate, pstMsg);
}

STATIC void xsvc_si_LcnUpdateSrchMsgEvtPmtTable_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstLcnUpdate->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LcnUpdateSrchCheckAndComposePmtData_Ses (pstLcnUpdate, pstMsg);
}

STATIC void xsvc_si_LcnUpdateSrchMsgEvtNitTable_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstLcnUpdate->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LcnUpdateSrchCheckAndComposeNitData_Ses (pstLcnUpdate, pstMsg);
}

STATIC void xsvc_si_LcnUpdateSrchMsgEvtTdtTable_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstLcnUpdate->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LcnUpdateSrchCheckAndComposeTdtData_Ses (pstLcnUpdate, pstMsg);
}

STATIC void xsvc_si_LcnUpdateSrchMsgEvtSgtTable_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstLcnUpdate->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LcnUpdateSrchCheckAndComposeSgtData_Ses (pstLcnUpdate, pstMsg);
}

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
STATIC void xsvc_si_LcnUpdateSrchMsgEvtBatTable_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstLcnUpdate->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LcnUpdateSrchCheckAndComposeBatData_Ses (pstLcnUpdate, pstMsg);
}

STATIC void xsvc_si_LcnUpdateSrchMsgEvtSdtTable_Ses (svcSi_LcnUpdateData_t *pstLcnUpdate, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (&pstLcnUpdate->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LcnUpdateSrchCheckAndComposeSdtData_Ses (pstLcnUpdate, pstMsg);
}
#endif

STATIC void xsvc_si_LcnUpdateSrchProcessMsg_Ses (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_LcnUpdateData_t	*pstLcnUpdate = (svcSi_LcnUpdateData_t *)pvData;

	if (NULL == pstLcnUpdate)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_EXTSEARCH:
		HxLOG_Print("[%s][eSIMSG_CMD_START_EXTSEARCH] called!!\n", __FUNCTION__);
		xsvc_si_LcnUpdateSrchMsgCmdStartSearch_Ses (pstLcnUpdate, pstMsg);
		break;

	case eSIMSG_CMD_STOP_EXTSEARCH:
		HxLOG_Print("[%s][eSIMSG_CMD_STOP_EXTSEARCH] called!!\n", __FUNCTION__);
		xsvc_si_LcnUpdateSrchMsgCmdStopSearch_Ses (pstLcnUpdate, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Print("[%s][eSIMSG_EVT_PAT_TABLE] called!!\n", __FUNCTION__);
		xsvc_si_LcnUpdateSrchMsgEvtPatTable_Ses (pstLcnUpdate, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Print("[%s][eSIMSG_EVT_PMT_TABLE] called!!\n", __FUNCTION__);
		xsvc_si_LcnUpdateSrchMsgEvtPmtTable_Ses (pstLcnUpdate, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		HxLOG_Print("[%s][eSIMSG_EVT_NIT_TABLE] called!!\n", __FUNCTION__);
		xsvc_si_LcnUpdateSrchMsgEvtNitTable_Ses (pstLcnUpdate, pstMsg);
		break;

	case eSIMSG_EVT_TDT_TABLE:
		HxLOG_Print("[%s][eSIMSG_EVT_TDT_TABLE] called!!\n", __FUNCTION__);
		xsvc_si_LcnUpdateSrchMsgEvtTdtTable_Ses (pstLcnUpdate, pstMsg);
		break;

	case eSIMSG_EVT_SGT_TABLE:
		HxLOG_Print("[%s][eSIMSG_EVT_SGT_TABLE] called!!\n", __FUNCTION__);
		xsvc_si_LcnUpdateSrchMsgEvtSgtTable_Ses (pstLcnUpdate, pstMsg);
		break;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	case eSIMSG_EVT_BAT_TABLE:
		HxLOG_Print("[%s][eSIMSG_EVT_BAT_TABLE] called!!\n", __FUNCTION__);
		xsvc_si_LcnUpdateSrchMsgEvtBatTable_Ses (pstLcnUpdate, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		HxLOG_Print("[%s][eSIMSG_EVT_SDT_TABLE] called!!\n", __FUNCTION__);
		xsvc_si_LcnUpdateSrchMsgEvtSdtTable_Ses (pstLcnUpdate, pstMsg);
		break;
#endif

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_LcnUpdateSrchGetSearchActionFunc_Ses (void)
{
	return xsvc_si_LcnUpdateSrchProcessMsg_Ses;
}

void *xsvc_si_LcnUpdateSrchAllocSearchActionData_Ses (void)
{
	svcSi_LcnUpdateData_t	*pstLcnUpdate;

	pstLcnUpdate = (svcSi_LcnUpdateData_t *)OxSI_Calloc (sizeof(svcSi_LcnUpdateData_t));
	if (NULL != pstLcnUpdate)
	{
		pstLcnUpdate->stBase.eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstLcnUpdate;
}

void xsvc_si_LcnUpdateSrchFreeSearchActionData_Ses (void *pvData)
{
	svcSi_LcnUpdateData_t	*pstLcnUpdate = (svcSi_LcnUpdateData_t *)pvData;

	if (NULL != pstLcnUpdate)
	{
		svc_si_SrchFreeSearchData (&pstLcnUpdate->stBase);

		// 추가로 free가 필요한 context var.들은 여기에서 정리..!
		//  TODO: ...

		OxSI_Free (pstLcnUpdate);
		pstLcnUpdate = NULL;
	}

	return;
}

HERROR xsvc_si_LcnUpdateSrchSetExtSrchData_Ses (Handle_t hAction, SvcSi_ExtSrchOptionType_t eOptionType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HUINT32 					ulActionId = PAL_PIPE_GetActionId(hAction);
	svcSi_LcnUpdateData_t		*pstLcnUpdate = (svcSi_LcnUpdateData_t *)svc_si_GetSiData(ulActionId);

	if (pstLcnUpdate == NULL)			{ return ERR_FAIL; }

	HxLOG_Print("eOptionType: 0x%x \n", eOptionType);

	switch (eOptionType)
	{
		case eSVCSI_EXTSEARCH_OPTION_SES_SVCLISTID:
			pstLcnUpdate->usSvcListId = (HUINT16)ulParam1;
			break;

		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xsvc_si_LcnUpdateSrchGetExtSrchData_Ses (Handle_t hAction, SvcSi_ExtSrchDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR						hErr = ERR_FAIL;
	HUINT32						ulActionId = PAL_PIPE_GetActionId(hAction);
	svcSi_LcnUpdateData_t		*pstLcnUpdate = (svcSi_LcnUpdateData_t *)svc_si_GetSiData(ulActionId);

	if (pulDataNum == NULL)				{ return ERR_FAIL; }
	if (ppvDataArray == NULL)			{ return ERR_FAIL; }
	if (pstLcnUpdate == NULL)			{ return ERR_FAIL; }

	HxLOG_Print("[%s] eDataType: 0x%x \n", __FUNCTION__, eDataType);

	*pulDataNum = 0;
	*ppvDataArray = NULL;

	switch (eDataType)
	{
		case eSVCSI_EXTSEARCH_DATA_SES_DELIVERY_TSINFO:
			hErr = xsvc_si_LcnUpdateSrchGetDeliveryTsInfo (pstLcnUpdate, hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_EXTSEARCH_DATA_SES_NETWORK_NETINFO:
			hErr = xsvc_si_LcnUpdateSrchGetNetworkInfo (pstLcnUpdate, hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_EXTSEARCH_DATA_SES_REFNITLINK_TRIPLEID:
			hErr = xsvc_si_LcnUpdateSrchGetRefNitTripleId (pstLcnUpdate, hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_EXTSEARCH_DATA_SES_SGTLINK_TRIPLEID:
			hErr = xsvc_si_LcnUpdateSrchGetSgtSvcTripleId (pstLcnUpdate, hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_EXTSEARCH_DATA_SES_SVCLISTINFO:
			hErr = xsvc_si_LcnUpdateSrchGetSvcListNames (pstLcnUpdate, hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_EXTSEARCH_DATA_SES_SERVICE_SVCINFO:
			hErr = xsvc_si_LcnUpdateSrchGetServiceInfos (pstLcnUpdate, hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_EXTSEARCH_DATA_SES_BOUQUET_BQINFO:
			hErr = xsvc_si_LcnUpdateSrchGetBouquetInfos (pstLcnUpdate, hAction, pulDataNum, ppvDataArray);
			break;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
		case eSVCSI_EXTSEARCH_DATA_SES_ASTRA_PORTAL_TS_INFO:
			hErr = xsvc_si_LcnUpdateSrchGetAstraPortalTripleId (pstLcnUpdate, hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_EXTSEARCH_DATA_SES_ASTRA_REGIONAL_PORTAL_TS_INFO:
			hErr = xsvc_si_LcnUpdateSrchGetAstraRegionalPortalTripleId (pstLcnUpdate, hAction, pulDataNum, ppvDataArray);
			break;

		case eSVCSI_EXTSEARCH_DATA_SES_PORTAL_INFO:
			hErr = xsvc_si_LcnUpdateSrchGetPortalServiceInfos (pstLcnUpdate, hAction, pulDataNum, ppvDataArray);
			break;
#endif

		case eSVCSI_EXTSEARCH_DATA_SES_TS_TSINFO:
		default:
			hErr = ERR_FAIL;
			break;

	}

	return hErr;
}

HERROR xsvc_si_LcnUpdateSrchFreeExtSrchData_Ses (Handle_t hAction, SvcSi_ExtSrchDataType_t eDataType, void *pvDataArray)
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

