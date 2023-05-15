/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_search_mbc.c
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
#include <octo_common.h>

#include <pal_pipe.h>

#include <isosvc.h>
#include <pal_sirawpool.h>

#include <db_svc.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_xsvc_si_lib_search_base.h>
#include <_xsvc_si_lib_lcn_base.h>
#include <_xsvc_si_lib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MBC_TEST_EUSAT_SEARCH

#define BOUQUET_ID_MBC_MIDDLE_EAST		0xE000
#define BOUQUET_ID_MBC_NORTH_AFRICA		0xF000
#define BOUQUET_ID_MBC_REGION_MASK		0xF000


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


#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_SrchMsgCmdStartSearch_Mbc (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstMsg->hAction));

	(void)svc_si_SrchInitSearchData (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stStartSrch.unSpec), &(pstMsg->unArg.stStartSrch.stOption));
	(void)svc_si_SrchInitSearchResultTable (pstSrch);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxSPEC_MBC);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)svc_si_SrchGetFilteringTypes (pstSrch);

#if !defined(MBC_TEST_EUSAT_SEARCH)
	pstSrch->eFiltering |= (bSI_FILTERING_BAT);
	pstSrch->eFiltering |= (bSI_FILTERING_SDT_OTH);
	pstSrch->usBatBouquetId = (HUINT16)SILIB_TABLE_EXT_ID_ALL; // not use EXT id
#endif

	(void)svc_si_SrchStartSection (pstSrch);
}

STATIC void xsvc_si_SrchMsgCmdStopSearch_Mbc (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_SrchStopSection (pstSrch);
	(void)svc_si_SrchFreeSearchData (pstSrch);
	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstSrch->hAction));
}

STATIC void xsvc_si_SrchMsgEvtPatTable_Mbc (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnPatReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtPmtTable_Mbc (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtSdtTable_Mbc (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));	
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtSdtOthTable_Mbc (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));	
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitTable_Mbc (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitOthTable_Mbc (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtBatTable_Mbc (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));		
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchProcessMsg_Mbc (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL == pstSrch)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_SEARCH:
		xsvc_si_SrchMsgCmdStartSearch_Mbc (pstSrch, pstMsg);
		break;

	case eSIMSG_CMD_STOP_SEARCH:
		xsvc_si_SrchMsgCmdStopSearch_Mbc (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_SrchMsgEvtPatTable_Mbc (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_SrchMsgEvtPmtTable_Mbc (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		xsvc_si_SrchMsgEvtSdtTable_Mbc (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_OTH_TABLE:
		xsvc_si_SrchMsgEvtSdtOthTable_Mbc (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		xsvc_si_SrchMsgEvtNitTable_Mbc (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_OTH_TABLE:
		xsvc_si_SrchMsgEvtNitOthTable_Mbc (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_BAT_TABLE:
		xsvc_si_SrchMsgEvtBatTable_Mbc (pstSrch, pstMsg);
		break;
		
	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SrchGetSearchActionFunc_Mbc (void)
{
	return xsvc_si_SrchProcessMsg_Mbc;
}

void *xsvc_si_SrchAllocSearchActionData_Mbc (void)
{
	svcSi_SrchData_t	*pstSrch;

	pstSrch = (svcSi_SrchData_t *)OxSI_Calloc (sizeof(svcSi_SrchData_t));
	if (NULL != pstSrch)
	{
		pstSrch->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstSrch;
}

void xsvc_si_SrchFreeSearchActionData_Mbc (void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL != pstSrch)
	{
		svc_si_SrchFreeSearchData (pstSrch);
		OxSI_Free (pstSrch);
		pstSrch = NULL;
	}

	return;
}

STATIC void	_xsvc_sisearch_DeleteChannels(HxList_t *pChList)
{
	if (pChList)
	{
		HxList_t	*pstItem;

		HLIB_LIST_First(pChList);
		for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next)
		{
			void *pvData = (void *)HLIB_LIST_Data(pstItem);
			if (pvData) OxSI_Free(pvData);

		}
		HLIB_LIST_RemoveAll(pChList);
	}
}

STATIC void	_xsvc_sisearch_DeleteLcnInfoList(HxList_t *pLcnInfoList)
{
	if (pLcnInfoList)
	{
		HxList_t	*pstItem;

		HLIB_LIST_First(pLcnInfoList);
		for (pstItem = pLcnInfoList; NULL != pstItem; pstItem = pstItem->next)
		{
			void *pvData = (void *)HLIB_LIST_Data(pstItem);
			if (pvData) OxSI_Free(pvData);

		}
		HLIB_LIST_RemoveAll(pLcnInfoList);
	}
}

STATIC HERROR _xsvc_sisearch_GetLcnInfoList(HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndLoop, HxList_t **ppLcnInfoList)
{
	HxList_t						*pstItem = NULL;
	HxList_t						*pLcnInfoList = NULL;
	HxList_t 						*pst2ndDesLoop = NULL;			
	HUINT8							*pucDesRaw = NULL;
	xsvcSi_LcnInfo_Mbc_t	 		*pstLcnInfo;
	SIxMbcLogicalChannelDes_t		*pstLcd = NULL;
	DbSvc_TsInfo_t 					 stTsInfo;
	HBOOL							 bLcdFound = FALSE;
		
	HxLOG_Debug(HxANSI_COLOR_YELLOW("Enter > _xsvc_sisearch_GetLcnInfoList\n"));

	if ((NULL == pst2ndLoop) || (NULL == ppLcnInfoList))
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	pst2ndDesLoop = pst2ndLoop->pstDesLoop;
	pLcnInfoList = *ppLcnInfoList;

	HxSTD_MemSet(&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
	stTsInfo.usOnId = pst2ndLoop->unInfo.stBat.usOrgNetId;
	stTsInfo.usTsId = pst2ndLoop->unInfo.stBat.usTsId;

	// (1) Finding LCD
	for (pstItem = pst2ndDesLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem);
		
		if (eSIxDESCTAG_MBC_LOGICAL_CHANNEL != *pucDesRaw)
		{
			continue;
		}

		pstLcd = (SIxMbcLogicalChannelDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
		if (NULL != pstLcd)
		{
			SIxMbcLogicalChannelInfo_t		*pChListItem = pstLcd->pstInfo;

			while(pChListItem)
			{			
				pstLcnInfo = (DbSvc_Info_t *)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_Mbc_t));

				if(NULL != pstLcnInfo)
				{		
					pstLcnInfo->usOnId = stTsInfo.usOnId;
					pstLcnInfo->usTsId = stTsInfo.usTsId;
					pstLcnInfo->usServiceId = pChListItem->usServiceId;
					pstLcnInfo->ucVisibleServiceFlag = pChListItem->ucVisibleServiceFlag;
					pstLcnInfo->usLogicalChannelNumber = pChListItem->usLogicalChannelNumber;

					pLcnInfoList = HLIB_LIST_Append(pLcnInfoList, (void *)pstLcnInfo);
					bLcdFound = TRUE;
				}

				pChListItem = pChListItem->pstNext;
			}

			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLcd);
		}
	}

END_FUNC:

	if(bLcdFound == TRUE)
	{
		*ppLcnInfoList = pLcnInfoList;
		HxLOG_Debug(HxANSI_COLOR_YELLOW("Exit - LCD found \n"), bLcdFound);
		return ERR_OK;
	}
	else
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("Exit - LCD not found\n"));
		return ERR_FAIL;
	}
}

STATIC HERROR _xsvc_sisearch_SetLcnInfo(DbSvc_Info_t *pstSvcInfo, HxList_t *pLcnInfoList)
{
	HBOOL		 hErr = ERR_FAIL;
	HxList_t	*pstItem = NULL;

	HUINT16		 usSvcOnid = DbSvc_GetOnId(pstSvcInfo);
	HUINT16		 usSvcTsid = DbSvc_GetTsId(pstSvcInfo);
	HUINT16		 usSvcSid  = DbSvc_GetSvcId(pstSvcInfo);

	HxLOG_Debug(" [SET LCN INFO] LCN Info Num = [%d] \n", HLIB_LIST_Length(pLcnInfoList));
	
	DbSvc_SetLcnFlag(pstSvcInfo, FALSE);
	DbSvc_SetLcn(pstSvcInfo, 0);
	DbSvc_SetVisibleFlag(pstSvcInfo, TRUE);

	HLIB_LIST_First(pLcnInfoList);

	for (pstItem = pLcnInfoList; NULL != pstItem; pstItem = pstItem->next)
	{
		xsvcSi_LcnInfo_Mbc_t 		*pstLcnInfo = (xsvcSi_LcnInfo_Mbc_t *)HLIB_LIST_Data(pstItem);

		if((usSvcOnid == pstLcnInfo->usOnId) && (usSvcTsid == pstLcnInfo->usTsId) && (usSvcSid == pstLcnInfo->usServiceId))
		{
			DbSvc_SetLcnFlag(pstSvcInfo, TRUE);
			DbSvc_SetLcn(pstSvcInfo, pstLcnInfo->usLogicalChannelNumber);
			DbSvc_SetVisibleFlag(pstSvcInfo, pstLcnInfo->ucVisibleServiceFlag);
			HxLOG_Debug(" [SET LCN INFO]  SVC Name = [%s], LCN = [%d] \n", DbSvc_GetName(pstSvcInfo), pstLcnInfo->usLogicalChannelNumber);
			
			hErr = ERR_OK;
			break;
		}
	}

	if(hErr == ERR_FAIL)
	{
		HxLOG_Print(" [SET LCN INFO]   SVC Name = [%s] : Can not found a LCN \n", DbSvc_GetName(pstSvcInfo));
	}

	return hErr;
}

STATIC HERROR _xsvc_sisearch_ComposeSldBasedServiceInfo(HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndLoop, HUINT16 usBouquetId, HxList_t **ppChList)
{
	HxList_t			*pstItem = NULL;
	HxList_t 			*pst2ndDesLoop = NULL;	
	HxList_t 			*pChList = NULL;
	HUINT8 				*pucDesRaw = NULL;
	DbSvc_TsInfo_t 		 stTsInfo;
	HERROR				 hErr = ERR_OK;
	HUINT32				 ulSvcNum = 0;

	HxLOG_Info(HxANSI_COLOR_YELLOW("Enter > _xsvc_sisearch_ComposeSldBasedServiceInfo \n"));

	if ((NULL == pst2ndLoop) || (NULL == ppChList))
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	pst2ndDesLoop = pst2ndLoop->pstDesLoop;
	pChList = *ppChList;

	HxSTD_MemSet(&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
	stTsInfo.usOnId = pst2ndLoop->unInfo.stBat.usOrgNetId;
	stTsInfo.usTsId = pst2ndLoop->unInfo.stBat.usTsId;

	for (pstItem = pst2ndDesLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem);
		if(eSIxDESCTAG_SERVICE_LIST == *pucDesRaw)
		{
			SIxSvcListDes_t *pstSvcListDes = (SIxSvcListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
			HUINT8			*pszUndefinedSvcName = NULL;
			HUINT32			 ulDvbSvcType;
			DxSvcType_e 	 eSvcType;
			DxVideoType_e	 eVideoType;
			DxOperator_e	 eSvcOpType = eDxOPERATOR_None;

			if (NULL != pstSvcListDes)
			{
				SIxSvcListInfo_t 	*pSvcListItem = pstSvcListDes->pstInfo;
				
				while(pSvcListItem)
				{
					HxList_t			*pstSvcItem = NULL;
					DbSvc_Info_t		*pstSvcInfo = NULL;
					DbSvc_Info_t		*pstBatSvcInfo;
					HBOOL				 bSameSvcFound = FALSE;					

					for (pstSvcItem = pChList; NULL != pstSvcItem; pstSvcItem = pstSvcItem->next)
					{
						pstBatSvcInfo = (DbSvc_Info_t *)HLIB_LIST_Data(pstSvcItem);
						
						if (DbSvc_GetTsId(pstBatSvcInfo) == stTsInfo.usTsId
							&& DbSvc_GetOnId(pstBatSvcInfo) == stTsInfo.usOnId
							&& DbSvc_GetSvcId(pstBatSvcInfo) == pSvcListItem->usSvcId)
						{
							// Service Added Already
							bSameSvcFound = TRUE;
							pstSvcInfo = pstBatSvcInfo;
							break;
						}
					}

					if(bSameSvcFound == FALSE)
					{
						pstSvcInfo = (DbSvc_Info_t *)OxSI_Malloc(sizeof(DbSvc_Info_t));
						DB_SVC_InitServiceInfo(pstSvcInfo);
					}
					
					if(NULL != pstSvcInfo)
					{
						DbSvc_SetTsIdx(pstSvcInfo, stTsInfo.usTsIdx);
						DbSvc_SetOnId(pstSvcInfo, stTsInfo.usOnId);
						DbSvc_SetTsId(pstSvcInfo, stTsInfo.usTsId);
						DbSvc_SetSvcId(pstSvcInfo, pSvcListItem->usSvcId);

						// Bouquet ID
						DbSvc_SetBouquetId(pstSvcInfo, 0, usBouquetId);
						
						// Service name
						hErr = svc_si_LibGetUndefinedServiceName(ulActionId, &pszUndefinedSvcName);
						if ((ERR_OK == hErr) && (NULL != pszUndefinedSvcName))
						{
							DbSvc_SetName(pstSvcInfo, pszUndefinedSvcName);
						}

						// guess service type - tv or radio
						ulDvbSvcType = pSvcListItem->ucServType;

						// Service type, Video Type
						eSvcType = eDxSVC_TYPE_All;
						eVideoType = eDxVIDEO_TYPE_ALL;
						hErr = svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (ulActionId, ulDvbSvcType, &eSvcType, &eVideoType);
						if (ERR_OK != hErr)
						{
							HxLOG_Error ("svc_si_LibGetSvcAndVideoTypeFromDvbSvcType err: fixed SvcType(%d)\n", ulDvbSvcType);
						}
						else
						{
							HxLOG_Info("\t[SVC] (0x%x:%s), SvcType(%d, %d)\n", DbSvc_GetSvcId(pstSvcInfo), DbSvc_GetName(pstSvcInfo), ulDvbSvcType, eSvcType);

							DbSvc_SetSvcType(pstSvcInfo, eSvcType);
							DbSvc_SetVideoType(pstSvcInfo, eVideoType);

							// orgSvcType 은 Dvb Svc Type을 그대로 저장한다..by chmin
							DbSvc_SetOrgSvcType(pstSvcInfo, ulDvbSvcType);

							// op type
							hErr = xsvc_si_LibSrchGetServiceOptype(ulActionId, &eSvcOpType);
							if (hErr != ERR_OK)
							{
								HxLOG_Error("xsvc_si_LibSrchGetServiceOptype() return failed..!! ulActionId(%d) \n", ulActionId);
								eSvcOpType = eDxOPERATOR_None;
							}
							DbSvc_SetOpType(pstSvcInfo, eSvcOpType);

							if(bSameSvcFound == FALSE)
							{
								pChList = HLIB_LIST_Append(pChList, (void *)pstSvcInfo);
							}
							
							ulSvcNum++;
						}
					}

					pSvcListItem = pSvcListItem->pstNext;
				}
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcListDes);
			}
		}
	}

	if(0 == ulSvcNum)
	{
		return ERR_FAIL;
	}
	else
	{
		*ppChList = pChList;
		return ERR_OK;
	}
}

STATIC HERROR _xsvc_sisearch_ComposeCategoryServiceInfo(HUINT32 ulActionId, SIxTable_PsiTable_t *pstBatTable, SIxTable_2ndLoop_t *pst2ndLoop, HUINT16 usBouquetId, HxList_t **ppChList)
{
	HxList_t			*pstDescItem = NULL;
	HxList_t			*pstSvcItem = NULL;
	HUINT8				*pucDesRaw = NULL;
	HxList_t			*pstDesLoop = NULL;
	HxList_t 			*pst2ndDesLoop = NULL;	
	HxList_t 			*pChList = NULL;
	DbSvc_Info_t		*pstBatSvcInfo;
	DbSvc_TsInfo_t 		 stTsInfo;
	HBOOL				 isFoundBouqName = FALSE;

		
	if ((NULL == pstBatTable) || (NULL == pst2ndLoop) || (NULL == ppChList))
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	pstDesLoop = pstBatTable->pstDesLoop;
	pst2ndDesLoop = pst2ndLoop->pstDesLoop;
	pChList = *ppChList;

	HxSTD_MemSet(&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
	stTsInfo.usOnId = pst2ndLoop->unInfo.stBat.usOrgNetId;
	stTsInfo.usTsId = pst2ndLoop->unInfo.stBat.usTsId;

	for (pstDescItem = pst2ndDesLoop; NULL != pstDescItem; pstDescItem = pstDescItem->next)
	{
		pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstDescItem);
		if(eSIxDESCTAG_SERVICE_LIST == *pucDesRaw)
		{
			SIxSvcListDes_t *pstSvcListDes = (SIxSvcListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
			HUINT8			*pszUndefinedSvcName = NULL;
			HUINT32			 ulDvbSvcType;
			DxSvcType_e 	 eSvcType;
			DxVideoType_e	 eVideoType;
			DxOperator_e	 eSvcOpType = eDxOPERATOR_None;

			if (NULL != pstSvcListDes)
			{
				SIxSvcListInfo_t *pSvcListItem = pstSvcListDes->pstInfo;
				while(pSvcListItem)
				{
					DbSvc_Info_t	*pstSvcInfo;
					HUINT32			 ulGrpIndex;
					HBOOL			 bSameSvcFound = FALSE;
						
					HLIB_LIST_First(pChList);
					
					for (pstSvcItem = pChList; NULL != pstSvcItem; pstSvcItem = pstSvcItem->next)
					{
						pstBatSvcInfo = (DbSvc_Info_t *)HLIB_LIST_Data(pstSvcItem);
						
						if (DbSvc_GetTsId(pstBatSvcInfo) == stTsInfo.usTsId
							&& DbSvc_GetOnId(pstBatSvcInfo) == stTsInfo.usOnId
							&& DbSvc_GetSvcId(pstBatSvcInfo) == pSvcListItem->usSvcId)
						{
							// Service Added Already
							bSameSvcFound = TRUE;
							pstSvcInfo = pstBatSvcInfo;
							break;
						}
					}

					if(bSameSvcFound == FALSE)
					{
						// Service Not Added
						pstSvcInfo = (DbSvc_Info_t *)OxSI_Malloc(sizeof(DbSvc_Info_t));
 
						if(NULL != pstSvcInfo)
						{
							DB_SVC_InitServiceInfo(pstSvcInfo);

							DbSvc_SetTsIdx(pstSvcInfo, stTsInfo.usTsIdx);
							DbSvc_SetOnId(pstSvcInfo, stTsInfo.usOnId);
							DbSvc_SetTsId(pstSvcInfo, stTsInfo.usTsId);
							DbSvc_SetSvcId(pstSvcInfo, pSvcListItem->usSvcId);
						}
					}					

					if(NULL != pstSvcInfo)
					{
						// Get Group Num
						for (ulGrpIndex=0; ulGrpIndex<DxGROUP_LEN; ulGrpIndex++)
						{
							if (DbSvc_GetGroupId(pstSvcInfo, ulGrpIndex) <= 0)
							{
								break;
							}
						}

						// Add Group ID
						if(ulGrpIndex < DxGROUP_LEN)
						{
							DbSvc_SetGroupId(pstSvcInfo, ulGrpIndex, usBouquetId);	// usBouquetId == Group ID
						}

						if(bSameSvcFound == FALSE)
						{
							pChList = HLIB_LIST_Append(pChList, (void *)pstSvcInfo);
						}
					}

					pSvcListItem = pSvcListItem->pstNext;
				}
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcListDes);
			}
		}
	}

	*ppChList = pChList;
	return ERR_OK;
}

STATIC HERROR _xsvc_sisearch_ComposeCategoryInfo(HUINT32 ulActionId, SIxTable_PsiTable_t *pstBatTable, HxList_t **ppGroupList)
{
	HxList_t		*pstItem = NULL;
	HUINT8 			*pucDesRaw = NULL;
	HxList_t 		*pstDesLoop = pstBatTable->pstDesLoop;
	HxList_t 		*pGroupList = NULL;
	HBOOL			 isFoundBouqName = FALSE;

		
	if ((NULL == pstDesLoop) || (NULL == ppGroupList))
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	pGroupList = *ppGroupList;

	for (pstItem = pstDesLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem);
		if(eSIxDESCTAG_BOUQUET_NAME == *pucDesRaw)
		{
			DbSvc_GroupInfo_t	*pstGroupInfo = NULL;
			SIxBouqNameDes_t 	*pstBouqNameDes = (SIxBouqNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

			if(pstBouqNameDes != NULL)
			{
				pstGroupInfo = (DbSvc_GroupInfo_t *)OxSI_Malloc(sizeof(DbSvc_GroupInfo_t));

				if(pstGroupInfo != NULL)
				{
					pstGroupInfo->ulGroupId = pstBatTable->unInfo.stBat.usBqId;
					HLIB_STD_StrNCpySafe((HCHAR*)pstGroupInfo->szGroupName, pstBouqNameDes->pszBouqName, DxMAX_GROUP_NAME_LEN);

					pGroupList = HLIB_LIST_Append(pGroupList, (void *)pstGroupInfo);

					isFoundBouqName = TRUE;
					break;
				}
			}
		}
	}

	if(isFoundBouqName == FALSE)
	{
		return ERR_FAIL;
	}
	else
	{
		*ppGroupList = pGroupList;
		return ERR_OK;
	}
}

STATIC HBOOL _xsvc_sisearch_isMiddleEastCountry_Mbc(HxCountry_e eCountryId)
{
	HBOOL		isMiddleEastCountry = FALSE;

	switch(eCountryId)
	{
		// Middle-East
		case eCountryID_BHR:	// Bahrain
		case eCountryID_IRN:	// Iran
		case eCountryID_IRQ:	// Iraq
		case eCountryID_KWT:	// Kuwait
		case eCountryID_OMN:	// Oman
		case eCountryID_QAT:	// Qatar
		case eCountryID_SAU:	// Saudi Arabia
		case eCountryID_ARE:	// United Arab Emirates
			isMiddleEastCountry = TRUE;
			break;
		
		// North Africa
		case eCountryID_EGY:	// Egypt
		case eCountryID_TUN:	// Tunisia
		case eCountryID_DZA:	// Algeria
		case eCountryID_MAR:	// Morocco
		// ?????
		case eCountryID_GRC:	// Greece
		case eCountryID_TUR:	// Turkey
		default:
			isMiddleEastCountry = FALSE;
			break;
	}

	return isMiddleEastCountry;
}

STATIC HERROR _xsvc_sisearch_ComposeBatBasedServiceInfo_Mbc(HUINT32 ulActionId, HxList_t **ppChList, HxList_t **ppLcnInfoList, HxList_t **ppBouqList, HxList_t **ppGroupList)
{
	HERROR					 hErr;
	HERROR					 hRet = ERR_FAIL;
	HUINT16 				 usBouquetId;
	HBOOL					 bComposeServiceInfo, bComposeCategoryInfo = FALSE;
	HxList_t				*pstItem1, *pstItem2;
	SIxTable_2ndLoop_t		*pst2ndLoop;
	SIxTable_PsiTable_t 	*pstBatTable = NULL, *pstCurBatTable = NULL;
	HxCountry_e				 eCountryId;
	HBOOL					 isMiddleEastCountry = FALSE;
	

	pstBatTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Bat, SILIB_TABLE_EXT_ID_ALL);
	
	if (NULL != pstBatTable)
	{		
		pstCurBatTable = pstBatTable;

		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
		if(ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
			eCountryId = eCountryID_ARE;
		}

		isMiddleEastCountry = _xsvc_sisearch_isMiddleEastCountry_Mbc(eCountryId);

		while(NULL != pstCurBatTable)
		{
			usBouquetId = pstCurBatTable->unInfo.stBat.usBqId;
			bComposeServiceInfo = FALSE;
			bComposeCategoryInfo = FALSE;
			
			if(isMiddleEastCountry)
			{
				// Middle-East Country
				if(usBouquetId == BOUQUET_ID_MBC_MIDDLE_EAST)  		// Middle-East Service
				{
					bComposeServiceInfo = TRUE;
				}
				else if((usBouquetId & BOUQUET_ID_MBC_REGION_MASK) == BOUQUET_ID_MBC_MIDDLE_EAST)		// Middle-East Service Category
				{
					bComposeCategoryInfo = TRUE;
				}
			}
			else
			{
				// North Africa Country
				if(usBouquetId == BOUQUET_ID_MBC_NORTH_AFRICA)  // North-Africa Service
				{
					bComposeServiceInfo = TRUE;
				}
				else if((usBouquetId & BOUQUET_ID_MBC_REGION_MASK) == BOUQUET_ID_MBC_NORTH_AFRICA) 	// North-Africa Service Category
				{
					bComposeCategoryInfo = TRUE;
				}
			}
					
			if(bComposeServiceInfo == TRUE)
			{				
				HUINT8 			*pucDesRaw = NULL;
				HxList_t		*pstItem = NULL;
				HxList_t 		*pBouqList = *ppBouqList;
				
				for (pstItem = pstCurBatTable->pstDesLoop; NULL != pstItem; pstItem = pstItem->next)
				{
					pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem);
					if(eSIxDESCTAG_BOUQUET_NAME == *pucDesRaw)
					{
						DbSvc_BouqInfo_t	*pstBouqInfo = NULL;
						SIxBouqNameDes_t 	*pstBouqNameDes = (SIxBouqNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

						if(pstBouqNameDes != NULL)
						{
							pstBouqInfo = (DbSvc_BouqInfo_t *)OxSI_Malloc(sizeof(DbSvc_BouqInfo_t));

							if(pstBouqInfo != NULL)
							{
								pstBouqInfo->usBouqId = pstCurBatTable->unInfo.stBat.usBqId;
								HLIB_STD_StrNCpySafe((HCHAR*)pstBouqInfo->szBouqName, pstBouqNameDes->pszBouqName, DxMAX_BOUQ_NAME_LEN);

								pBouqList = HLIB_LIST_Append(pBouqList, (void *)pstBouqInfo);
								break;
							}
						}
					}
				}
				
				for (pstItem1 = pstCurBatTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
				{
					pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

					// (1) Get LCN Info List from BAT
					(void)_xsvc_sisearch_GetLcnInfoList(ulActionId, pst2ndLoop, ppLcnInfoList);
					
					// (2) Make Service List from SLD
					(void)_xsvc_sisearch_ComposeSldBasedServiceInfo(ulActionId, pst2ndLoop, usBouquetId, ppChList);
					
					hRet = ERR_OK;
				}

				*ppBouqList = pBouqList;
			}
			else if(bComposeCategoryInfo == TRUE)
			{
				// TODO: 

				// (1) Category ID & Name
				(void)_xsvc_sisearch_ComposeCategoryInfo(ulActionId, pstCurBatTable, ppGroupList);

				// (2) Service in Category
				for (pstItem1 = pstCurBatTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
				{
					pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

					(void)_xsvc_sisearch_ComposeCategoryServiceInfo(ulActionId, pstCurBatTable, pst2ndLoop, usBouquetId, ppChList);
				}
			}

			pstCurBatTable = pstCurBatTable->pstNextTable;
		}
	}

END_FUNC:
	if (NULL != pstBatTable)				{ PAL_SIRAWPOOL_FreeTable (pstBatTable); }

	return hRet;
}

STATIC HERROR _xsvc_sisearch_ComposeSdtBasedServiceInfo_Mbc(HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HUINT32 				 ulSvcNum = 0, ulPrvNum = 0;
	HUINT32 				 ulActSvcNum = 0, ulActPrvNum = 0;
	HUINT32 				 ulOthSvcNum = 0, ulOthPrvNum = 0;
	DbSvc_Info_t			*pstSvcArray = NULL;
	DbSvc_Info_t			*pstActSvcArray = NULL;
	DbSvc_Info_t			*pstOthSvcArray = NULL;
	DbSvc_PrvInfo_t 		*pstPrvArray = NULL;
	DbSvc_PrvInfo_t 		*pstActPrvArray = NULL;
	DbSvc_PrvInfo_t 		*pstOthPrvArray = NULL;

	SIxTable_PsiTable_t 	*pstSdtActTbl = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	SIxTable_PsiTable_t 	*pstSdtOthTbl = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

	HERROR					 hErr1 = ERR_OK, hErr2 = ERR_OK;
	HBOOL					 bMemAlloc = TRUE;

	if (NULL != pstSdtActTbl)
	{
		HxLOG_Info ("SDT Found: Composite the Services by SDT ACT\n");
		hErr1 = svc_si_LibSvcEsComposeServiceInfoBySdt (ulActionId, pstSdtActTbl, &ulActSvcNum, &pstActSvcArray, &ulActPrvNum, &pstActPrvArray);
		ulSvcNum = ulActSvcNum;
		ulPrvNum = ulActPrvNum;
	}

	if (NULL != pstSdtOthTbl)
	{
		HxLOG_Info ("SDT Found: Composite the Services by SDT OTH\n");
		hErr2 = svc_si_LibSvcEsComposeServiceInfoBySdt (ulActionId, pstSdtOthTbl, &ulOthSvcNum, &pstOthSvcArray, &ulOthPrvNum, &pstOthPrvArray);
		ulSvcNum += ulOthSvcNum;
		ulPrvNum += ulOthPrvNum;
	}

	if (ulSvcNum > 0)
	{
		HxLOG_Info("ulSvcNum: %d \n", ulSvcNum);

		pstSvcArray = (DbSvc_Info_t *)OxSI_Calloc (sizeof(DbSvc_Info_t) * ulSvcNum);
		if (NULL == pstSvcArray)
		{
			bMemAlloc = FALSE;
			HxLOG_Error ("Memory allocation failed\n");
			goto END_FUNC;
		}

		HxSTD_memcpy(pstSvcArray, pstActSvcArray, sizeof(DbSvc_Info_t) * ulActSvcNum);
		HxSTD_memcpy(&pstSvcArray[ulActSvcNum], pstOthSvcArray, sizeof(DbSvc_Info_t) * ulOthSvcNum);

		*ppstSvcArray = pstSvcArray;
		pstSvcArray = NULL;
		*pulSvcNum = ulSvcNum;
	}

	if (ulPrvNum > 0)
	{
		HxLOG_Info("ulPrvNum: %d \n", ulPrvNum);

		pstPrvArray = (DbSvc_PrvInfo_t *)OxSI_Calloc (sizeof(DbSvc_PrvInfo_t) * ulPrvNum);
		if (NULL == pstPrvArray)
		{
			bMemAlloc = FALSE;
			HxLOG_Error ("Memory allocation failed\n");
			goto END_FUNC;
		}

		HxSTD_memcpy(pstPrvArray, pstActPrvArray, sizeof(DbSvc_PrvInfo_t) * ulActPrvNum);
		HxSTD_memcpy(&pstPrvArray[ulActPrvNum], pstOthPrvArray, sizeof(DbSvc_PrvInfo_t) * ulOthPrvNum);

		*ppstPrvArray = pstPrvArray;
		pstPrvArray = NULL;
		*pulPrvNum = ulPrvNum;
	}

END_FUNC:
	if (NULL != pstSvcArray)					{ OxSI_Free (pstSvcArray); }
	if (NULL != pstActSvcArray) 				{ OxSI_Free (pstActSvcArray); }
	if (NULL != pstOthSvcArray) 				{ OxSI_Free (pstOthSvcArray); }
	if (NULL != pstPrvArray)					{ OxSI_Free (pstPrvArray); }
	if (NULL != pstActPrvArray) 				{ OxSI_Free (pstActPrvArray); }
	if (NULL != pstOthPrvArray) 				{ OxSI_Free (pstOthPrvArray); }
	if (NULL != pstSdtActTbl)					{ PAL_SIRAWPOOL_FreeTable (pstSdtActTbl); }
	if (NULL != pstSdtOthTbl)					{ PAL_SIRAWPOOL_FreeTable (pstSdtOthTbl); }

	return (((hErr1 == ERR_OK || hErr2 == ERR_OK) && bMemAlloc == TRUE) ? ERR_OK : ERR_FAIL);
}

STATIC HERROR _xsvc_sisearch_MergeServiceInfo_Mbc(svcSi_SrchData_t *pstSrch, HxList_t *pChList, DbSvc_Info_t *pstSdtSvcArray, HUINT32 ulSdtSvcNum, HxList_t *pLcnInfoList)
{
	HUINT32 				 ulCnt, ulSubCnt;
	HxList_t				*pstItem;
	DbSvc_Info_t			*pstBatSvcInfo;
	DbSvc_Info_t			*pstSdtSvcInfo;
	HBOOL					 bSameSvcFound;
	
	ulCnt = 0;
	HLIB_LIST_First(pChList);

	for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next)
	{
		pstBatSvcInfo = (DbSvc_Info_t *)HLIB_LIST_Data(pstItem);
		bSameSvcFound = FALSE;

		// Update service name
		for (ulSubCnt = 0; ulSubCnt < ulSdtSvcNum; ulSubCnt++)
		{
			pstSdtSvcInfo = &(pstSdtSvcArray[ulSubCnt]);

			if (DbSvc_GetTsId(pstBatSvcInfo) == DbSvc_GetTsId(pstSdtSvcInfo)
				&& DbSvc_GetOnId(pstBatSvcInfo) == DbSvc_GetOnId(pstSdtSvcInfo)
				&& DbSvc_GetSvcId(pstBatSvcInfo) == DbSvc_GetSvcId(pstSdtSvcInfo))
			{
				DbSvc_SetName(pstBatSvcInfo, DbSvc_GetName(pstSdtSvcInfo));
				DbSvc_SetSvcType(pstBatSvcInfo, DbSvc_GetSvcType(pstSdtSvcInfo));

				HxLOG_Print("[SVC:%d] [%3d. %s] type:%d, id=(0x%x,0x%x), idx=(%d,%d,%d) \n"
					, ulCnt+1
					, DbSvc_GetLcn(pstBatSvcInfo)
					, DbSvc_GetName(pstBatSvcInfo)
					, DbSvc_GetSvcType(pstBatSvcInfo)
					, DbSvc_GetTsId(pstBatSvcInfo)
					, DbSvc_GetSvcId(pstBatSvcInfo)
					, DbSvc_GetTsIdx(pstBatSvcInfo)
					, DbSvc_GetSvcIdx(pstBatSvcInfo)
					, DbSvc_GetPrvIdx(pstBatSvcInfo)
					);

				(void)_xsvc_sisearch_SetLcnInfo(pstBatSvcInfo, pLcnInfoList);
				(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Service, (void *)pstBatSvcInfo, NULL);

				bSameSvcFound = TRUE;
				ulCnt++;
				break;
			}

		}

		if(TRUE != bSameSvcFound)
		{
			HxLOG_Print("[SVC] [%d. %s] id=(0x%x,0x%x,0x%x) no same svc in sdt\n"
				, DbSvc_GetLcn(pstBatSvcInfo)
				, DbSvc_GetName(pstBatSvcInfo)
				, DbSvc_GetOnId(pstBatSvcInfo)
				, DbSvc_GetTsId(pstBatSvcInfo)
				, DbSvc_GetSvcId(pstBatSvcInfo)
				);
		}
	}

	return ERR_OK;
}

STATIC HERROR _xsvc_sisearch_GetResults_MBC_BatSearch (svcSi_SrchData_t *pstSrch, HUINT32 ulActionId)
{
	HUINT32 				 ulCnt;
	HUINT32					 ulTsNum = 0, ulPrvNum = 0;
	HUINT32					 ulSvcNum = 0, ulSdtSvcNum = 0;
	HUINT32 				 ulNetIdx = 0, ulTsIdx = 0;
	DbSvc_TsInfo_t			 stTsInfo;
	DbSvc_NetInfo_t 		 stNetInfo;
	HxList_t 				*pLcnInfoList = NULL;
	HxList_t 				*pChList = NULL;
	HxList_t				*pBouqList = NULL;
	HxList_t				*pGroupList = NULL;
	HUINT8					*pucRawDes;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	DbSvc_Info_t			*pstSvcArray = NULL;
	DbSvc_Info_t			*pstSdtSvcArray = NULL;
	DbSvc_PrvInfo_t 		*pstPrvArray = NULL;
	SIxTable_PsiTable_t 	*pstNitTable = NULL, *pstSdtTable = NULL, *pstPatTable = NULL;
	
	HERROR					 hErr, hResult = ERR_FAIL;

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	pstPatTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);

	// Network
	HxSTD_MemSet(&stNetInfo, 0x00, sizeof(DbSvc_NetInfo_t));
	if (NULL != pstNitTable)
	{
		stNetInfo.usNetId = pstNitTable->unInfo.stNit.usNetId;
		pucRawDes = SILIB_TABLE_Get1stLoopDescriptor (pstNitTable, eSIxDESCTAG_NETWORK_NAME, 0);
		if (NULL != pucRawDes)
		{
			SIxNetNameDes_t *pstNameDes = (SIxNetNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstNameDes)
			{
				HxSTD_StrNCpy ((char *)stNetInfo.szNetName, (const char *)pstNameDes->pszNetName, DxMAX_NET_NAME_LEN - 1);
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstNameDes);
			}
		}
	}
	else
	{
		// make dummy netInfo
		HUINT8		*pszNetName = NULL;
		HUINT32 	 ulNameLen = 0;

		hErr = svc_si_LibGetUndefinedNetworkName(ulActionId, &pszNetName);
		if (hErr == ERR_OK)
		{
			ulNameLen = HxSTD_StrLen((const char *)pszNetName);
			if (ulNameLen > (sizeof(stNetInfo.szNetName) - 1))
			{
				ulNameLen = (sizeof(stNetInfo.szNetName) - 1);
			}
			HxSTD_StrNCpy((char *)stNetInfo.szNetName, (const char *)pszNetName, ulNameLen);
		}

		stNetInfo.szNetName[ulNameLen] = '\0';
	}

	(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Network, (void *)&stNetInfo, &ulNetIdx);

	// TS Info
	if (NULL != pstSdtTable)
	{
		stTsInfo.usOnId = pstSdtTable->unInfo.stSdt.usOrgNetId;
		stTsInfo.usTsId = pstSdtTable->unInfo.stSdt.usTsId;
	}
	else if (NULL != pstPatTable)
	{
		stTsInfo.usTsId = pstPatTable->unInfo.stPat.usTsId;
	}
	else
	{
		HxLOG_Error ("No way to make the TS Info:\n");
		goto END_FUNC;
	}

	stTsInfo.usNetIdx = (HUINT16)ulNetIdx;

	(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Transponder, (void *)&stTsInfo, &ulTsIdx);

	// Delivery TS Info :
	if (eSI_SRCH_NITOPT_NitAct == pstSrch->eNitOption)
	{
		hErr = svc_si_LibGetAllDeliveryTsInfoFromNit (ulActionId, TRUE, TRUE, &ulTsNum, &pstTsArray);
		if ((ERR_OK == hErr) && (0 != ulTsNum) && (NULL != pstTsArray))
		{
			for (ulCnt = 0; ulCnt < ulTsNum; ulCnt++)
			{
				(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Delivery, (void *)&(pstTsArray[ulCnt]), NULL);
			}
		}
	}
	
	//--------------------------------------------------------------------
	// 1. BAT에서 데이터 추출
	//--------------------------------------------------------------------
	hErr = _xsvc_sisearch_ComposeBatBasedServiceInfo_Mbc(ulActionId, &pChList, &pLcnInfoList, &pBouqList, &pGroupList);
	if(ERR_OK != hErr)
	{
		HxLOG_Warning("can't make bat base service info\n");
	}
	
	//--------------------------------------------------------------------
	// 2. SDT에서 데이터 추출
	//--------------------------------------------------------------------
	hErr = _xsvc_sisearch_ComposeSdtBasedServiceInfo_Mbc(ulActionId, &ulSdtSvcNum, &pstSdtSvcArray, &ulPrvNum, &pstPrvArray);
	if(ERR_OK != hErr)
	{
		HxLOG_Warning("can't make sdt base service info\n");
	}
	HxLOG_Print(HxANSI_COLOR_YELLOW("ulBatSvcNum = %d, ulSdtSvcNum = %d\n"), HLIB_LIST_Length(pChList), ulSdtSvcNum);
	
	//--------------------------------------------------------------------
	// 3. Add - Service Info 구성 (BAT + SDT)
	//--------------------------------------------------------------------
	hErr = _xsvc_sisearch_MergeServiceInfo_Mbc(pstSrch, pChList, pstSdtSvcArray, ulSdtSvcNum, pLcnInfoList);
	if(ERR_OK != hErr)
	{
		HxLOG_Warning("can't make service info\n");
	}

	//--------------------------------------------------------------------
	// 4. Add - Bouquet Info
	//--------------------------------------------------------------------
	if (NULL != pBouqList)
	{
		HxList_t			*pstItem;
		DbSvc_BouqInfo_t	*pstBouqInfo;
		
		HLIB_LIST_First(pBouqList);

		for (pstItem = pBouqList; NULL != pstItem; pstItem = pstItem->next)
		{
			pstBouqInfo = (DbSvc_BouqInfo_t *)HLIB_LIST_Data(pstItem);
			(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Bouquet, (void *)pstBouqInfo, NULL);
		}
	}

	//--------------------------------------------------------------------
	// 5. Add - Provider Info
	//--------------------------------------------------------------------
	if (NULL != pstPrvArray)
	{
		for (ulCnt = 0; ulCnt < ulPrvNum; ulCnt++)
		{
			(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Provider, (void *)&(pstPrvArray[ulCnt]), NULL);
		}
	}

	//--------------------------------------------------------------------
	// 6. Add - Group(Category) Info
	//--------------------------------------------------------------------
	if (NULL != pGroupList)
	{
		HxList_t			*pstItem;
		DbSvc_GroupInfo_t	*pstGroupInfo;
		
		HLIB_LIST_First(pGroupList);

		for (pstItem = pGroupList; NULL != pstItem; pstItem = pstItem->next)
		{
			pstGroupInfo = (DbSvc_GroupInfo_t *)HLIB_LIST_Data(pstItem);
			(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Group, (void *)pstGroupInfo, NULL);
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pChList)					{ _xsvc_sisearch_DeleteChannels(pChList); }
	if (NULL != pLcnInfoList)				{ _xsvc_sisearch_DeleteLcnInfoList(pLcnInfoList); }
	if (NULL != pstSdtSvcArray)				{ OxSI_Free (pstSdtSvcArray); }
	if (NULL != pstTsArray) 				{ OxSI_Free (pstTsArray); }
	if (NULL != pstSvcArray)				{ OxSI_Free (pstSvcArray); }
	if (NULL != pstPrvArray)				{ OxSI_Free (pstPrvArray); }
	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	if (NULL != pstSdtTable)				{ PAL_SIRAWPOOL_FreeTable (pstSdtTable); }
	if (NULL != pstPatTable)				{ PAL_SIRAWPOOL_FreeTable (pstPatTable); }

	return hResult;
}


HERROR xsvc_si_SrchGetSearchedResultFromSi_Mbc (void *pvData)
{
	svcSi_SrchData_t 		*pstSrch = NULL;
	HUINT32					 ulActionId = 0;
	HERROR					 hResult = ERR_FAIL;

	if (pvData == NULL)
	{
		HxLOG_Error("null parameter error ! \n");
		return ERR_FAIL;
	}
	pstSrch = (svcSi_SrchData_t *)pvData;
	ulActionId = PAL_PIPE_GetActionId (pstSrch->hAction);

#if defined(MBC_TEST_EUSAT_SEARCH)
	// EU_SAT Search
	hResult = xsvc_si_LibSrchCollectServiceDataFromSi_Base(pstSrch);
#else
	// MBC_BAT Search
	hResult = _xsvc_sisearch_GetResults_MBC_BatSearch(pstSrch, ulActionId);
#endif

	return hResult;
}

/* end of file */

