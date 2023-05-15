/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_live_base.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIBRARY		\n

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
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_xsvc_si_lib_search_base.h>

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


#define _____LOCAL_COMPOSING_FUNCTIONS_____
STATIC HERROR xsvc_si_LibSrchGetSearchedResultFromSi_Base (svcSi_SrchData_t *pstSrch)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (pstSrch->hAction);
	HUINT32 				 ulCnt, ulTsNum = 0, ulSvcNum = 0, ulPrvNum = 0;
	HUINT32					 ulNetIdx = 0, ulTsIdx = 0;
	DbSvc_TsInfo_t			 stTsInfo;
	DbSvc_NetInfo_t			 stNetInfo;
	HUINT8					*pucRawDes;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	DbSvc_Info_t				*pstSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SIxTable_PsiTable_t		*pstNitTable = NULL, *pstSdtTable = NULL, *pstPatTable = NULL;
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
			SIxNetNameDes_t	*pstNameDes = (SIxNetNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
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
		HUINT32		 ulNameLen = 0;

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
		stTsInfo.usOnId	= pstSdtTable->unInfo.stSdt.usOrgNetId;
		stTsInfo.usTsId	= pstSdtTable->unInfo.stSdt.usTsId;
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

	// Service Info:
	hErr = svc_si_LibComposeServiceInfoInTp (ulActionId, &ulSvcNum, &pstSvcArray, &ulPrvNum, &pstPrvArray);
	if (ERR_OK == hErr)
	{
		if (NULL != pstSvcArray)
		{
			DbSvc_Info_t		*pstSvc = NULL;

			for (ulCnt = 0; ulCnt < ulSvcNum; ulCnt++)
			{
				pstSvc = &pstSvcArray[ulCnt];
				if (xsvc_si_LibSrchCompareSvcType(pstSrch->stOption.ulSvcType, DbSvc_GetSvcType(pstSvc)) != ERR_OK)
				{
					// search option에 설정된 svc type과 다르다
					continue;
				}

				if (xsvc_si_LibSrchCompareCasType(pstSrch->stOption.ulCasType, DbSvc_GetCasType(pstSvc)) != ERR_OK)
				{
					// search option에 설정된 cas type과 다르다
					continue;
				}

				(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Service, (void *)pstSvc, NULL);
			}
		}

		if (NULL != pstPrvArray)
		{
			for (ulCnt = 0; ulCnt < ulPrvNum; ulCnt++)
			{
				(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Provider, (void *)&(pstPrvArray[ulCnt]), NULL);
			}
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstTsArray)					{ OxSI_Free (pstTsArray); }
	if (NULL != pstSvcArray)				{ OxSI_Free (pstSvcArray); }
	if (NULL != pstPrvArray)				{ OxSI_Free (pstPrvArray); }
	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	if (NULL != pstSdtTable)				{ PAL_SIRAWPOOL_FreeTable (pstSdtTable); }
	if (NULL != pstPatTable)				{ PAL_SIRAWPOOL_FreeTable (pstPatTable); }

	return hResult;
}

#define ___MEMBER_FUNCTIONS___
HERROR xsvc_si_LibSrchCompareSvcType(HUINT32 ulSvcTypeInOption, DxSvcType_e eSvcType)
{
	DxSvcType_e eSvcTypeInOption = (DxSvcType_e)ulSvcTypeInOption;

	if (eSvcTypeInOption == eDxSVC_TYPE_All)
	{
		if (eSvcType == eDxSVC_TYPE_TV || eSvcType == eDxSVC_TYPE_RADIO || eSvcType == eDxSVC_TYPE_DATA || eSvcType == eDxSVC_TYPE_HIDDEN)
		{
			return ERR_OK;
		}
	}
	else if (eSvcTypeInOption == eDxSVC_TYPE_TV || eSvcTypeInOption == eDxSVC_TYPE_RADIO || eSvcTypeInOption == eDxSVC_TYPE_DATA|| eSvcType == eDxSVC_TYPE_HIDDEN )
	{
		if (eSvcType == eSvcTypeInOption)
		{
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR xsvc_si_LibSrchCompareCasType(HUINT32 ulCasTypeInOption, DxCasType_e eCasType)
{
	DxCasType_e eCasTypeInOption = (DxCasType_e)ulCasTypeInOption;

	switch(eCasTypeInOption)
	{
		case eDxCAS_TYPE_All:
			return ERR_OK;

		case eDxCAS_TYPE_FTA:
			if (eCasType == eDxCAS_TYPE_FTA)
			{
				return ERR_OK;
			}
			break;

		default: // cas
			if (eCasType > eDxCAS_TYPE_FTA)
			{
				return ERR_OK;
			}
			break;
	}

	return ERR_FAIL;
}

HERROR xsvc_si_LibSrchOnPatReceived_Base (svcSi_SrchData_t *pstSrch, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	HBOOL					 bPmtFiltered = FALSE;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32					 ulCnt, ulNumPmt = 0;
	SIxTable_PatProgram_t	*pstProgramArray = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	hErr = xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, hAction, pstSecCb);
	if ((ERR_OK != hErr) || (eSI_TABLE_TIMEOUT == pstSecCb->ulTableStatus))
	{
		HxLOG_Error ("xsvc_si_LibSrchOnSiTableReceived_Base err or Timeout:\n");
		goto END_FUNC;
	}

	hErr = svc_si_LibGetAllPmtProgramsFromPat (ulActionId, &ulNumPmt, &pstProgramArray);
	if ((ERR_OK != hErr) || (0 == ulNumPmt) || (NULL == pstProgramArray))
	{
		HxLOG_Error ("svc_si_LibGetAllPmtProgramsFromPat err: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	for (ulCnt = 0; ulCnt < ulNumPmt; ulCnt++)
	{
		SIxTable_PatProgram_t	*pstPatProg = &(pstProgramArray[ulCnt]);

		hErr = svc_si_SrchStartFilter (pstSrch, eSIxTABLETYPE_Pmt, (HUINT32)pstPatProg->usProgNum, (HUINT32)pstPatProg->usProgMapPid);
		if (ERR_OK == hErr)
		{
			bPmtFiltered = TRUE;
		}
		else
		{
			HxLOG_Error ("svc_si_SrchStartFilter err: ProgNum(%d), PID(%d)\n", (HUINT32)pstPatProg->usProgNum, (HUINT32)pstPatProg->usProgMapPid);
		}
	}

	hResult = (TRUE == bPmtFiltered) ? ERR_OK : ERR_FAIL;

END_FUNC:
	if (NULL != pstProgramArray)					{ OxSI_Free (pstProgramArray); }
	return hResult;
}

HERROR xsvc_si_LibSrchOnSiTableReceived_Base (svcSi_SrchData_t *pstSrch, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	svcSi_FilteringInfo_t	*pstFilterInfo = NULL;

	if (hAction != pstSrch->hAction)
	{
		HxLOG_Error ("Action Handle Diff: DataActHandle(0x%08x), MsgActHandle(0x%08x)\n", pstSrch->hAction, hAction);
		return ERR_FAIL;
	}

	pstFilterInfo = svc_si_SrchFindFilterInfoByFilterId (pstSrch, pstSecCb->ulFilterId);
	if (NULL == pstFilterInfo)
	{
		HxLOG_Error ("No Such Filter...\n");
		return ERR_FAIL;
	}

	if (eSI_TABLE_TIMEOUT == pstSecCb->ulTableStatus)
	{
		pstFilterInfo->eFilterStatus = eSI_TABLE_TIMEOUT;
		return ERR_FAIL;
	}

	if (eSI_TABLE_RUNING == pstSecCb->ulTableStatus)
	{
		// filter mode가 continue인 경우 running으로 올라온다
		pstFilterInfo->eFilterStatus = eSI_TABLE_WAITING;
		return ERR_FAIL;
	}

	pstFilterInfo->eFilterStatus = eSI_TABLE_RECEIVED;
	return ERR_OK;
}

HERROR xsvc_si_LibSrchGetNetTsInfoFromNIT_Base (HUINT32 ulActionId, DbSvc_NetInfo_t *pstNetInfo, DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR					 hResult = ERR_FAIL;
	SIxTable_PsiTable_t		*pstNitTable = NULL, *pstTable = NULL;
	SIxTable_PsiTable_t		*pstSdtTable = NULL;
	HxList_t				*pst2ndLoop = NULL;
	HUINT8					*pucRawDes = NULL;

	if ((pstNetInfo == NULL) || (pstTsInfo == NULL))
	{
		HxLOG_Error ("invalid parameters \n");
		goto END_FUNC;
	}

	HxSTD_memset (pstNetInfo, 0x00, sizeof(DbSvc_NetInfo_t));
	HxSTD_memset (pstTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	if (pstSdtTable == NULL)
	{
		HxLOG_Error ("SDTact parse error! \n");
	}

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	if (pstNitTable == NULL)
	{
		HxLOG_Error ("NITact parse error! \n");
		goto END_FUNC;
	}

	for (pstTable = pstNitTable; pstTable != NULL; pstTable = pstTable->pstNextTable)
	{
		// make network info
		pstNetInfo->usNetId = pstTable->unInfo.stNit.usNetId;
		pucRawDes = SILIB_TABLE_Get1stLoopDescriptor (pstTable, eSIxDESCTAG_NETWORK_NAME, 0);
		if (NULL != pucRawDes)
		{
			SIxNetNameDes_t	*pstNameDes = (SIxNetNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstNameDes)
			{
				HxSTD_StrNCpy ((char *)pstNetInfo->szNetName, (const char *)pstNameDes->pszNetName, DxMAX_NET_NAME_LEN - 1);
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstNameDes);
			}
		}

		if (pstSdtTable == NULL)
		{
			// sdt에서 onId, TsId를 가져와서 nit와 동일한지 비교해 pstTsInfo에 넣어야 하는데
			// sdt table이 없으면 비교할 수 없으니 skip~
			break;
		}

		for (pst2ndLoop = pstTable->pst2ndLoop; pst2ndLoop != NULL; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = NULL;

			pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pst2ndLoop);
			if (pst2ndItem == NULL)
			{
				continue;
			}

			if ((pst2ndItem->unInfo.stNit.usOrgNetId != pstSdtTable->unInfo.stSdt.usOrgNetId)
				|| (pst2ndItem->unInfo.stNit.usTsId != pstSdtTable->unInfo.stSdt.usTsId))
			{
				continue;
			}

			pstTsInfo->usOnId = pst2ndItem->unInfo.stNit.usOrgNetId;
			pstTsInfo->usTsId = pst2ndItem->unInfo.stNit.usTsId;

			hResult = ERR_OK;
			goto END_FUNC;
		}
	}

END_FUNC:
	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	if (NULL != pstSdtTable)				{ PAL_SIRAWPOOL_FreeTable (pstSdtTable); }

	return hResult;

}

HERROR xsvc_si_LibSrchComposeTsInfoFromSi_Base (HUINT32 ulActionId, DbSvc_NetInfo_t *pstNetInfo, DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR					 hResult = ERR_FAIL;
	SIxTable_PsiTable_t		*pstSiTable = NULL;
	HUINT8					*pszName = NULL;
	HUINT32					 ulLen = 0;

	if ((pstNetInfo == NULL) || (pstTsInfo == NULL))
	{
		HxLOG_Error ("invalid parameters \n");
		goto END_FUNC;
	}

	HxSTD_memset (pstNetInfo, 0x00, sizeof(DbSvc_NetInfo_t));
	HxSTD_memset (pstTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	pstSiTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	if (pstSiTable != NULL)
	{
		// SDT에서 정보를 가져오자
		pstTsInfo->usTsId = pstSiTable->unInfo.stSdt.usTsId;
		pstTsInfo->usOnId = pstSiTable->unInfo.stSdt.usOrgNetId;
	}
	else
	{
		// SDT가 없으니 PAT에서 정보를 가져오자
		pstSiTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
		if (pstSiTable == NULL)
		{
			HxLOG_Error ("PAT parsing failed.. \n");
			goto END_FUNC;
		}

		pstTsInfo->usTsId = pstSiTable->unInfo.stPat.usTsId;
		pstTsInfo->usOnId = 0;
	}

	/* NIT가 없기 때문에 Dummy로 만들어 준다. */
	pstNetInfo->usNetId = 0x0;
	hResult = svc_si_LibGetUndefinedNetworkName(ulActionId, &pszName);
	if (hResult == ERR_OK)
	{
		ulLen = HxSTD_StrLen((const char *)pszName);
		if (ulLen > (sizeof(pstNetInfo->szNetName) - 1))
		{
			ulLen = (sizeof(pstNetInfo->szNetName) - 1);
		}
		HxSTD_memcpy(&pstNetInfo->szNetName[0], pszName, ulLen);
	}

	pstNetInfo->szNetName[ulLen] = '\0';
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSiTable)				{ PAL_SIRAWPOOL_FreeTable (pstSiTable); }

	return hResult;
}

HERROR xsvc_si_LibSrchGetServiceOpType_Base (DxOperator_e *peSvcOpType)
{
	HERROR				hErr = ERR_FAIL;
	DxOperator_e		eOperator = eDxOPERATOR_NOT_DEFINED;

	if (peSvcOpType == NULL)
	{
		return ERR_FAIL;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperator, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
		return ERR_FAIL;
	}

	*peSvcOpType = eOperator;
	return ERR_OK;
}

#define _____COMPOSING_FUNCTIONS_____
HERROR xsvc_si_LibSrchCollectServiceDataFromSi_Base (svcSi_SrchData_t *pstSrch)
{
	HERROR					 hErr;

	hErr = xsvc_si_LibSrchGetSearchedResultFromSi_Base (pstSrch);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("xsvc_si_LibSrchGetSearchedResultFromSi_Base failed:\n");
	}

	return hErr;
}
/* end of file */

