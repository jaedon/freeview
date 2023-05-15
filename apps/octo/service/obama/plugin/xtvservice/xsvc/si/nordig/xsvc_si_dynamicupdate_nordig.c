/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_dynamicupdate_nordig.c
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
#include <hapi.h>

#include <vkernel.h>
#include <octo_common.h>

#include <pal_pipe.h>

#include <isosvc.h>
#include <pal_sirawpool.h>

#include <db_svc.h>
#include <svc_ch.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_svc_si_dynamicupdate.h>
#include <_xsvc_si_lib_search_base.h>
#include <_xsvc_si_lib_service_base.h>
#include <_xsvc_si_lib.h>
#include <_xsvc_si_search.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagSiDUContext_Nordig
{
	HBOOL					bAllSiTblReceived;
	HUINT32					ulLastNitCrc32;
} SiDUContext_Nordig_t;
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC SiDUContext_Nordig_t	s_stSiDUContext_Nordig;

STATIC void xsvc_si_DuMsgCmdStartDynamicUpdate_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg);
STATIC void xsvc_si_DuMsgCmdStopDynamicUpdate_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg);

#define ___LOCAL_FUNCTIONS___

INLINE STATIC SiDUContext_Nordig_t *xsvc_si_DuGetContent_Nordig(void)
{
	return &s_stSiDUContext_Nordig;
}

STATIC HERROR xsvc_si_DuInitContext_Nordig(SiDUContext_Nordig_t *pstContext)
{
	HUINT32		ulLastNitCrc32;

	if (pstContext == NULL)
	{
		return ERR_FAIL;
	}

	ulLastNitCrc32 = pstContext->ulLastNitCrc32;

	HxSTD_memset(pstContext, 0x00, sizeof(SiDUContext_Nordig_t));
	pstContext->bAllSiTblReceived = FALSE;
	pstContext->ulLastNitCrc32 = ulLastNitCrc32;

	return ERR_OK;
}

#if 0
STATIC HERROR xsvc_si_DuSvcEsComposeServiceInfoInTp_Nordig (HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HUINT32 				 ulSvcNum = 0, ulPrvNum = 0, ulFltSvcNum = 0;
	HUINT32 				 ulActSvcNum = 0, ulActPrvNum = 0;
	HUINT32 				 ulOthSvcNum = 0, ulOthPrvNum = 0;
	HUINT32 				 ulNitSvcNum = 0;
	DbSvc_Info_t			*pstSvcArray = NULL;
	DbSvc_Info_t			*pstActSvcArray = NULL;
	DbSvc_Info_t			*pstOthSvcArray = NULL;
	DbSvc_Info_t			*pstNitSvcArray = NULL;
	DbSvc_Info_t			*pstFltSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	DbSvc_PrvInfo_t			*pstActPrvArray = NULL;
	DbSvc_PrvInfo_t			*pstOthPrvArray = NULL;
	DbSvc_PrvInfo_t			*pstNitPrvArray = NULL;

	SIxTable_PsiTable_t		*pstNitActTbl = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	SIxTable_PsiTable_t		*pstSdtActTbl = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	SIxTable_PsiTable_t		*pstSdtOthTbl = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

	HUINT32					 ulIndex = 0, ulSubIndex = 0;
	HERROR					 hErr1 = ERR_OK, hErr2 = ERR_OK;
	HBOOL					 bMemAlloc = TRUE;

	HxList_t				*pRcvSDTLst = NULL;
	HxList_t				*pstSvcIdxLst = NULL;
	HUINT32					 ulTpUid = 0;

	if (NULL != pstSdtActTbl)
	{
		HxLOG_Info ("SDT Found: Composite the Services by SDT ACT\n");
		hErr1 = xsvc_si_LibSvcEsComposeServiceInfoBySdt_Base (ulActionId, pstSdtActTbl, &ulActSvcNum, &pstActSvcArray, &ulActPrvNum, &pstActPrvArray);
		ulSvcNum = ulActSvcNum;
		ulPrvNum = ulActPrvNum;

		ulTpUid = ((HUINT32)pstSdtActTbl->unInfo.stSdt.usOrgNetId << 16) | ((HUINT32)pstSdtActTbl->unInfo.stSdt.usTsId);
		pRcvSDTLst = HLIB_LIST_Append(pRcvSDTLst , (void *)ulTpUid);
	}

	if (NULL != pstSdtOthTbl)
	{
		SIxTable_PsiTable_t		*pstTable = NULL;
		HxLOG_Info ("SDT Found: Composite the Services by SDT OTH\n");
		hErr2 = xsvc_si_LibSvcEsComposeServiceInfoBySdt_Base (ulActionId, pstSdtOthTbl, &ulOthSvcNum, &pstOthSvcArray, &ulOthPrvNum, &pstOthPrvArray);
		ulSvcNum += ulOthSvcNum;
		ulPrvNum += ulOthPrvNum;

		for (pstTable = pstSdtOthTbl; pstTable != NULL; pstTable = pstTable->pstNextTable)
		{
			ulTpUid = ((HUINT32)pstTable->unInfo.stSdt.usOrgNetId << 16) | ((HUINT32)pstTable->unInfo.stSdt.usTsId);
			pRcvSDTLst = HLIB_LIST_Append(pRcvSDTLst , (void *)ulTpUid);
		}
	}

	// TODO: compose svc from nit.
#if 0
	if (NULL != pstNitActTbl)
	{
		HxLOG_Info ("NIT Found: Composite the Services by NIT\n");
		hErr2 = xsvc_si_SrchCheckAndComposeServiceInfoByNit_Nordig(ulActionId, pstNitActTbl, &ulNitSvcNum, &pstNitSvcArray, &ulNitPrvNum, &pstNitPrvArray);
	}
#endif
	if ((ulSvcNum > 0) && (ulNitSvcNum > 0))
	{
		pstSvcArray = (DbSvc_Info_t *)OxSI_Calloc (sizeof(DbSvc_Info_t) * ulSvcNum);
		if (NULL == pstSvcArray)
		{
			bMemAlloc = FALSE;
			HxLOG_Error ("Memory allocation failed\n");
			goto END_FUNC;
		}

		HxSTD_memcpy(pstSvcArray, pstActSvcArray, sizeof(DbSvc_Info_t) * ulActSvcNum);
		HxSTD_memcpy(&pstSvcArray[ulActSvcNum], pstOthSvcArray, sizeof(DbSvc_Info_t) * ulOthSvcNum);

		for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
		{
			for (ulSubIndex = 0; ulSubIndex < ulNitSvcNum; ulSubIndex++)
			{
		                ulTpUid = ((HUINT32)(DbSvc_GetOnId(&pstNitSvcArray[ulSubIndex]) << 16) | ((HUINT32)DbSvc_GetTsId(&pstNitSvcArray[ulSubIndex])));
		                if(HLIB_LIST_Find(pRcvSDTLst, (const void*)ulTpUid) != NULL)
				{
					if ((DbSvc_GetOnId(&pstNitSvcArray[ulSubIndex]) == DbSvc_GetOnId(&pstSvcArray[ulIndex]))
					&& (DbSvc_GetTsId(&pstNitSvcArray[ulSubIndex]) == DbSvc_GetTsId(&pstSvcArray[ulIndex]))
					&& (DbSvc_GetSvcId(&pstNitSvcArray[ulSubIndex]) == DbSvc_GetSvcId(&pstSvcArray[ulIndex])))
					{
						HxSTD_memcpy(&pstNitSvcArray[ulSubIndex], &pstSvcArray[ulIndex], sizeof(DbSvc_Info_t));
					}

					if (HLIB_LIST_Find(pstSvcIdxLst, (const void*)ulSubIndex) == NULL)
					{
						pstSvcIdxLst = HLIB_LIST_Append(pstSvcIdxLst , (void *)ulSubIndex);
					}
                }
			}
		}

		if (pstSvcIdxLst != NULL)
		{
			ulFltSvcNum = HLIB_LIST_Length(pstSvcIdxLst);
			pstFltSvcArray = (DbSvc_Info_t *)OxSI_Calloc (sizeof(DbSvc_Info_t) * ulFltSvcNum);
			if (NULL == pstFltSvcArray)
			{
				bMemAlloc = FALSE;
				HxLOG_Error ("Memory allocation failed\n");
				goto END_FUNC;
			}

			ulIndex = 0;
			while(pstSvcIdxLst)
			{
				ulSubIndex = (HUINT32)HLIB_LIST_Data(pstSvcIdxLst);
				HxSTD_memcpy(&pstFltSvcArray[ulIndex], &pstNitSvcArray[ulSubIndex], sizeof(DbSvc_Info_t));
				pstSvcIdxLst = HLIB_LIST_Next(pstSvcIdxLst);
				ulIndex++;
			}
		}

#if defined(CONFIG_DEBUG)
		for (ulIndex = 0; ulIndex < ulFltSvcNum; ulIndex++)
		{
			HxLOG_Debug("[%03d]0x%04x, 0x%04x, 0x%04x, %s\n", ulIndex,
				DbSvc_GetOnId(&pstFltSvcArray[ulIndex]),
				DbSvc_GetTsId(&pstFltSvcArray[ulIndex]),
				DbSvc_GetSvcId(&pstFltSvcArray[ulIndex]),
				DbSvc_GetName(&pstFltSvcArray[ulIndex]));
		}
#endif

		*ppstSvcArray = pstFltSvcArray;
		pstFltSvcArray = NULL;
		*pulSvcNum = ulFltSvcNum;
	}

	if (ulPrvNum > 0)
	{
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
	if (NULL != pstFltSvcArray)					{ OxSI_Free (pstFltSvcArray); }
	if (NULL != pstActSvcArray)					{ OxSI_Free (pstActSvcArray); }
	if (NULL != pstOthSvcArray)					{ OxSI_Free (pstOthSvcArray); }
	if (NULL != pstNitSvcArray)					{ OxSI_Free (pstNitSvcArray); }
	if (NULL != pstPrvArray)					{ OxSI_Free (pstPrvArray); }
	if (NULL != pstActPrvArray)					{ OxSI_Free (pstActPrvArray); }
	if (NULL != pstOthPrvArray)					{ OxSI_Free (pstOthPrvArray); }
	if (NULL != pstNitPrvArray)					{ OxSI_Free (pstNitPrvArray); }
	if (NULL != pstNitActTbl)					{ PAL_SIRAWPOOL_FreeTable (pstNitActTbl); }
	if (NULL != pstSdtActTbl)					{ PAL_SIRAWPOOL_FreeTable (pstSdtActTbl); }
	if (NULL != pstSdtOthTbl)					{ PAL_SIRAWPOOL_FreeTable (pstSdtOthTbl); }
	if (NULL != pRcvSDTLst)						{ HLIB_LIST_RemoveAll(pRcvSDTLst);}
	if (NULL != pstSvcIdxLst)					{ HLIB_LIST_RemoveAll(pstSvcIdxLst);}
	return (((hErr1 == ERR_OK || hErr2 == ERR_OK) && bMemAlloc == TRUE) ? ERR_OK : ERR_FAIL);
}
#endif

STATIC HERROR xsvc_si_DuCollectServiceDataFromSi_Nordig(svcSi_SrchData_t *pstSrch)
{
	HERROR					hResult = ERR_FAIL;
#if 0
	HUINT32					 ulActionId = 0;
	HUINT32 				 ulCnt, ulSubCnt, ulTsNum = 0, ulNitSvcNum = 0, ulSdtSvcNum = 0, ulPrvNum = 0;
	HUINT32					 ulNetIdx = 0, ulTsIdx = 0;
	DbSvc_NetInfo_t			 stNetInfo;
	HUINT8					*pucRawDes;
	DbSvc_Info_t			*pstSdtSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SIxTable_PsiTable_t		*pstNitTable = NULL, *pstCurNitTable = NULL;;
	HERROR					 hErr = ERR_FAIL;
	HxList_t				*pstItem;
	HxList_t				*pstItem1, *pstItem2;
	DbSvc_TsInfo_t			 stTsInfo;
	HxList_t 				*pChList = NULL;
	HBOOL					 bDsdFound;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	HBOOL					 bNitAct = FALSE;

	ulActionId = PAL_PIPE_GetActionId (pstSrch->hAction);
#endif

	/* check same nit section 0 */
	if(eSVCSI_SEARCHTYPE_NORDIG_NitOthBased == pstSrch->stOption.ulSearchType)
	{
		HUINT32 				ulActionId = 0;
		HINT32					nNetId = -1;
		SIxTable_PsiTable_t 	*pstNitTable = NULL;
		SiDUContext_Nordig_t	*pstContext = xsvc_si_DuGetContent_Nordig();

		(void)HAPI_GetRegistryInt(DxDB_NETWORK_ID_FOR_NETWORK_SEARCH, &nNetId);
		if(-1 == nNetId)
		{
			HxLOG_Error("can't get Network Id\n");
			return ERR_FAIL;
		}

		ulActionId = PAL_PIPE_GetActionId (pstSrch->hAction);
		pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitOth, (HUINT32)nNetId);

		/* There is no section number in pstNitTable, Let's supposse that the first table is section 0. */
		if(NULL != pstNitTable)
		{
			if(pstContext->ulLastNitCrc32 == pstNitTable->unInfo.stNit.ulCrc32)
			{
				HxLOG_Print("same nit crc32(0x%x) - do not parse results\n", pstNitTable->unInfo.stNit.ulCrc32);
				if (NULL != pstNitTable)			{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
				return ERR_FAIL;
			}
			else
			{
				HxLOG_Print("nit crc32 changed(0x%x -> 0x%x) - do the next step\n", pstContext->ulLastNitCrc32, pstNitTable->unInfo.stNit.ulCrc32);
				pstContext->ulLastNitCrc32 = pstNitTable->unInfo.stNit.ulCrc32;
				if (NULL != pstNitTable)			{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
			}
		}
	}


#if 1
	hResult = xsvc_si_SrchGetSearchedResultFromSi_Nordig((void *)pstSrch);

#if 0
	hResult = _xsvc_sisearch_GetResults_NORDIG_NitActSearch(pstSrch, ulActionId);
	if(ERR_OK != hErr)
	{
		hResult = _xsvc_sisearch_GetResults_NORDIG_NitOthSearch(pstSrch, ulActionId);
	}
#endif

	return hResult;

#else

	// 어차피 정해진 net id만 걸었으니. 다 가져와본다.
	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitOth, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstNitTable)
	{
		HxLOG_Print("there is no nit other \n");
		// 어라 없다? act 도 걸었으니 함 가져와 본다.
		pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
		if (NULL == pstNitTable)
		{
			HxLOG_Error("there is no nit actual \n");
			goto END_FUNC;
		}
		else
		{
			bNitAct = TRUE;
		}
	}


	pstCurNitTable = pstNitTable;
	while(NULL != pstCurNitTable)
	{

		// nit other, actual 모두 특정 net id 만 걸었으니, net id 가 맞는지 check 안함..

		// Network Info
		HxSTD_MemSet(&stNetInfo, 0x00, sizeof(DbSvc_NetInfo_t));
		stNetInfo.usNetId = pstCurNitTable->unInfo.stNit.usNetId;
		pucRawDes = SILIB_TABLE_Get1stLoopDescriptor (pstCurNitTable, eSIxDESCTAG_NETWORK_NAME, 0);
		if (NULL != pucRawDes)
		{
			SIxNetNameDes_t	*pstNameDes = (SIxNetNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstNameDes)
			{
				HxSTD_StrNCpy (stNetInfo.szNetName, pstNameDes->pszNetName, DxMAX_NET_NAME_LEN - 1);
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstNameDes);
			}
		}
		(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Network, (void *)&stNetInfo, &ulNetIdx);
		HxLOG_Print("[NET] id = %d, name = %s\n", stNetInfo.usNetId, stNetInfo.szNetName);


		// TS Info - NIT SLD가 SDT service_desc 보다 우선
		// And make service list
		for (pstItem1 = pstCurNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
		{
			SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

			HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
			if (NULL != pst2ndLoop)
			{
				stTsInfo.usOnId	= pst2ndLoop->unInfo.stNit.usOrgNetId;
				stTsInfo.usTsId	= pst2ndLoop->unInfo.stNit.usTsId;
			}
			else
			{
				HxLOG_Error ("No way to make the TS Info:\n");
				goto END_FUNC;
			}
			stTsInfo.usNetIdx = (HUINT16)ulNetIdx;

			// (1) Update delivery system info.
			bDsdFound = FALSE;
			for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
			{
				HUINT8		*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

				hErr = svc_si_LibGetDeliveryTsInfoFromDsd(ulActionId, pucDesRaw, &stTsInfo);
				if(ERR_OK == hErr)
				{
					stTsInfo.usTsIdx = ulTsNum;
					(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Transponder, (void *)&stTsInfo, &ulTsIdx);
					ulTsNum++;
					HxLOG_Print("[TS](%d) onid/tsid/freq/sr/cons = %d/%d/%d/%d/%d\n"
									, ulTsIdx, stTsInfo.usOnId, stTsInfo.usTsId
									, stTsInfo.stTuningParam.cab.ulFreq, stTsInfo.stTuningParam.cab.usSR, stTsInfo.stTuningParam.cab.ucCon);
					bDsdFound = TRUE;
					break;
				}
			}

			if(FALSE == bDsdFound)
				continue;


			// TODO: SLD가 없는 경우 고려 - Telenor Stream

			// (2) Make Service List from SLD
			// (3) Update LCN
			hErr = xsvc_sisearch_ComposeSldBasedServiceInfo_Nordig(ulActionId, &stTsInfo, pst2ndLoop->pstDesLoop, &pChList);
			if(ERR_OK != hErr)
			{
				HxLOG_Warning("no proper service in this ts loop\n");
			}
		}

		pstCurNitTable = pstCurNitTable->pstNextTable;
	}


#if 1 // TODO: check.
	// Delivery Info
	hErr = svc_si_LibGetAllDeliveryTsInfoFromNit (ulActionId, bNitAct, TRUE, &ulTsNum, &pstTsArray);
	if ((ERR_OK == hErr) && (0 != ulTsNum) && (NULL != pstTsArray))
	{
		for (ulCnt = 0; ulCnt < ulTsNum; ulCnt++)
		{
			(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Delivery, (void *)&(pstTsArray[ulCnt]), NULL);
		}
	}
#endif


	HxLOG_Debug("Compose service info based on SDT \n");
	hErr = xsvc_sisearch_ComposeSdtBasedServiceInfo_Nordig(ulActionId, &ulSdtSvcNum, &pstSdtSvcArray, &ulPrvNum, &pstPrvArray);
	if(ERR_OK != hErr)
	{
		HxLOG_Warning("can't make sdt base service info\n");
	}


	HxLOG_Debug("add provider info \n");
	// Provider Info
	if (NULL != pstPrvArray)
	{
		for (ulCnt = 0; ulCnt < ulPrvNum; ulCnt++)
		{
			(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Provider, (void *)&(pstPrvArray[ulCnt]), NULL);
		}
	}

	ulNitSvcNum = HLIB_LIST_Length(pChList);
	HxLOG_Print("ulNitSvcNum = %d, ulSdtSvcNum = %d\n", ulNitSvcNum, ulSdtSvcNum);



	HxLOG_Debug("add service info \n");
	// Service Info
	ulCnt = 0;
	HLIB_LIST_First(pChList);
	for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next, ulCnt++)
	{
		DbSvc_Info_t 	*pstNitSvcInfo = (DbSvc_Info_t *)HLIB_LIST_Data(pstItem);
		HBOOL			bSameSvcFound = FALSE;

		// Update service name
		for (ulSubCnt = 0; ulSubCnt < ulSdtSvcNum; ulSubCnt++)
		{
			DbSvc_Info_t	*pstSdtSvcInfo = &(pstSdtSvcArray[ulSubCnt]);

			if (DbSvc_GetTsId(pstNitSvcInfo) == DbSvc_GetTsId(pstSdtSvcInfo)
				&& DbSvc_GetOnId(pstNitSvcInfo) == DbSvc_GetOnId(pstSdtSvcInfo)
				&& DbSvc_GetSvcId(pstNitSvcInfo) == DbSvc_GetSvcId(pstSdtSvcInfo))
			{
				DbSvc_SetName(pstNitSvcInfo, DbSvc_GetName(pstSdtSvcInfo));
				bSameSvcFound = TRUE;
				HxLOG_Print("[SVC](%03d) [%3d. %s] tsid:0x%x, svcid:0x%x, tsidx:%d, svcidx:%d, prvidx:%d\n", ulCnt
					, DbSvc_GetLcn(pstNitSvcInfo)
					, DbSvc_GetName(pstNitSvcInfo)
					, DbSvc_GetTsId(pstNitSvcInfo)
					, DbSvc_GetSvcId(pstNitSvcInfo)
					, DbSvc_GetTsIdx(pstNitSvcInfo)
					, DbSvc_GetSvcIdx(pstNitSvcInfo)
					, DbSvc_GetPrvIdx(pstNitSvcInfo)
					);
				break;
			}
		}
		if(TRUE != bSameSvcFound)
		{
			HxLOG_Print("[SVC](%03d) [%3d. %s] no same svc in sdt\n", ulSubCnt, DbSvc_GetLcn(pstNitSvcInfo), DbSvc_GetName(pstNitSvcInfo));
		}

		(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Service, (void *)pstNitSvcInfo, NULL);
	}

	xsvc_sisearch_DeleteChannels_Nordig(pChList);

	hResult = ERR_OK;


END_FUNC:
	if (NULL != pstTsArray)				{ OxSI_Free (pstTsArray); }
	if (NULL != pstSdtSvcArray)			{ OxSI_Free (pstSdtSvcArray); }
	if (NULL != pstPrvArray)			{ OxSI_Free (pstPrvArray); }
	if (NULL != pstNitTable)			{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }

	return hResult;
#endif

}


STATIC HERROR xsvc_si_DuCheckAndComposeSearchData_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	HINT32						nMsgType;
	HERROR						hErr;
	SiDUContext_Nordig_t		*pstContext = xsvc_si_DuGetContent_Nordig();

	if (svc_si_DuCheckAllSiTableReceived (pstSrch) == TRUE)
	{
		if (pstContext->bAllSiTblReceived == TRUE)
		{
#if 0
			svc_si_DuFreeSearchResult(pstSrch);
#else
			xsvc_si_DuMsgCmdStopDynamicUpdate_Nordig(pstSrch, pstMsg);
			xsvc_si_DuMsgCmdStartDynamicUpdate_Nordig(pstSrch, pstMsg);
			return ERR_OK;
#endif
		}

		pstContext->bAllSiTblReceived = TRUE;
		HxLOG_Print(HxANSI_COLOR_YELLOW("All Table Received!!!!\n"));

		hErr = xsvc_si_DuCollectServiceDataFromSi_Nordig(pstSrch);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("xsvc_si_LibSrchCollectServiceDataFromSi_Base err:\n");
		}

		nMsgType = (ERR_OK == hErr) ? eSEVT_SI_DYNAMIC_UPDATE_DONE : eSEVT_SI_DYNAMIC_UPDATE_FAIL;
		svc_si_PostMsgToAp (nMsgType, pstSrch->hAction, 0, 0, 0);

		/* filter stop은 search stop시에 한다 */
	}

	return ERR_OK;
}

#define _____SI_MSG_PROCESS_FUNCTION_____

STATIC void xsvc_si_DuMsgCmdStartDynamicUpdate_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	SvcSi_SearchOption_t 	stSrchOption;
	SiDUContext_Nordig_t	*pstContext = xsvc_si_DuGetContent_Nordig();
	HERROR					hErr = ERR_FAIL;
	HINT32					nNetworkId = 0;


	xsvc_si_DuInitContext_Nordig(pstContext);


	HxSTD_MemSet(&stSrchOption, 0x00, sizeof(SvcSi_SearchOption_t));

	(void)svc_si_DuInitSearchData (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stStartDynamicUpdate.unSpec), &stSrchOption);
	(void)svc_si_DuInitSearchResultTable (pstSrch);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxSPEC_NORDIG);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	hErr = HAPI_GetRegistryInt(DxDB_NETWORK_ID_FOR_NETWORK_SEARCH, &nNetworkId);
	HxLOG_Print("hErr = %d, nNetworkId = %d\n", hErr, nNetworkId);
	if (ERR_OK != hErr)
	{
		nNetworkId = 0;
	}

	if(0 == nNetworkId)
	{
		pstSrch->eFiltering = bSI_FILTERING_NIT_ACT;
		//pstSrch->stOption.ulSearchType = eSVCSI_SEARCHTYPE_NORDIG_NitActBased;
		pstSrch->stOption.ulSearchType = eSVCSI_SEARCHTYPE_NORDIG_NitActBased_DynamicUpdate;
	}
	else
	{
		pstSrch->eFiltering = bSI_FILTERING_NIT_OTH;
		pstSrch->usNitNetId = nNetworkId;
		pstSrch->stOption.ulSearchType = eSVCSI_SEARCHTYPE_NORDIG_NitOthBased;
	}
	pstSrch->eFiltering |= (bSI_FILTERING_SDT_ACT | bSI_FILTERING_SDT_OTH);


	(void)svc_si_DuStartSection (pstSrch);

}

STATIC void xsvc_si_DuMsgCmdStopDynamicUpdate_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	SiDUContext_Nordig_t		*pstContext = xsvc_si_DuGetContent_Nordig();

	(void)svc_si_DuStopSection (pstSrch);
	(void)svc_si_DuFreeSearchData (pstSrch);
	xsvc_si_DuInitContext_Nordig(pstContext);
}

STATIC void xsvc_si_DuMsgEvtNitTable_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_DuCheckAndComposeSearchData_Nordig (pstSrch, pstMsg);
}

STATIC void xsvc_si_DuMsgEvtSdtTable_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_DuCheckAndComposeSearchData_Nordig (pstSrch, pstMsg);
}

STATIC void xsvc_si_DuMsgEvtSdtOthTable_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_DuCheckAndComposeSearchData_Nordig (pstSrch, pstMsg);
}

STATIC void xsvc_si_DuProcessMsg_Nordig (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL == pstSrch)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_DYNAMICUPDATE:
		HxLOG_Print("eSIMSG_CMD_START_DYNAMICUPDATE \n");
		xsvc_si_DuMsgCmdStartDynamicUpdate_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_CMD_STOP_DYNAMICUPDATE:
		HxLOG_Print("eSIMSG_CMD_STOP_DYNAMICUPDATE \n");
		xsvc_si_DuMsgCmdStopDynamicUpdate_Nordig (pstSrch, pstMsg);
		break;

	// TODO: home channel 에서만 해야 한다면,
	// XMGR_LiveView_BASE_EvtSiNit 로 처리된 부분 참고하여, (linkage type 0x04에 대하여 저장하도록 되어 있음)
	// dynamic update에서 get 후에 비교. 안 맞으면 skip~~~
	// live 에서 먼저 set 이 잘되는지 확인 필요~~ (timing 상.)

	// 혹은, user setting에 의하여, 혹은 default 로 저장된 값을 읽어와서 비교. 안 맞으면 skip~~
	case eSIMSG_EVT_NIT_TABLE:
		HxLOG_Print("eSIMSG_EVT_NIT_TABLE\n");
		xsvc_si_DuMsgEvtNitTable_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_OTH_TABLE:
		HxLOG_Print("eSIMSG_EVT_NIT_OTH_TABLE\n");
		xsvc_si_DuMsgEvtNitTable_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		HxLOG_Print("eSIMSG_EVT_SDT_TABLE \n");
		xsvc_si_DuMsgEvtSdtTable_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_OTH_TABLE:
		HxLOG_Print("eSIMSG_EVT_SDT_OTH_TABLE \n");
		xsvc_si_DuMsgEvtSdtOthTable_Nordig (pstSrch, pstMsg);
		break;

	default:
		HxLOG_Print ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_DuGetDynamicUpdateActionFunc_Nordig (void)
{
	return xsvc_si_DuProcessMsg_Nordig;
}

void *xsvc_si_DuAllocDynamicUpdateActionData_Nordig (void)
{
	svcSi_SrchData_t	*pstSrch;

	pstSrch = (svcSi_SrchData_t *)OxSI_Calloc (sizeof(svcSi_SrchData_t));
	if (NULL != pstSrch)
	{
		pstSrch->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstSrch;
}

void xsvc_si_DuFreeDynamicUpdateActionData_Nordig (void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL != pstSrch)
	{
		svc_si_DuFreeSearchData (pstSrch);
		OxSI_Free (pstSrch);
		pstSrch = NULL;
	}

	return;
}
/* end of file */


