/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_search_ausdtt.c
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
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_xsvc_si_lib_search_base.h>
#include <_xsvc_si_lib.h>


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

STATIC HERROR xsvc_si_SrchSetDefaultAuthoriyInNetwork_AusDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, DbSvc_NetInfo_t *pstNetInfo)
{
	HUINT8					*pucRawDes;

	if ((pstNitTable != NULL) && (pstNetInfo != NULL))
	{
		(void)DB_SVC_ResetNetworkDefaultAuthority(pstNetInfo);

		pucRawDes = SILIB_TABLE_Get1stLoopDescriptor (pstNitTable, eSIxDESCTAG_DEFAULT_AUTHORITY, 0);
		if (NULL != pucRawDes)
		{
			SIxDefaultAuthorityDes_t *pstDADes = (SIxDefaultAuthorityDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstDADes)
			{
				(void)DB_SVC_SetNetworkDefaultAuthority(pstNetInfo, (const HUINT8 *)pstDADes->pszDefaultAuthority);
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDADes);
			}
		}
	}

	return ERR_OK;
}


STATIC HERROR xsvc_si_SrchSetDefaultAuthoriyInTS_AusDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, SIxTable_PsiTable_t *pstSdtTable, DbSvc_TsInfo_t *pstTsInfo)
{
	HUINT8 *pucRawDes;

	if ((pstNitTable != NULL) && (pstSdtTable != NULL) && (pstTsInfo != NULL))
	{
		HxList_t *pstItem;

		(void)DB_SVC_ResetTsDefaultAuthority(pstTsInfo);

		for (pstItem = pstNitTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem);

			if ( (pst2ndLoop->unInfo.stNit.usTsId == pstTsInfo->usTsId)
				&& (pst2ndLoop->unInfo.stNit.usOrgNetId == pstTsInfo->usOnId) )
			{
				pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndLoop, eSIxDESCTAG_DEFAULT_AUTHORITY, 0);
				if (NULL != pucRawDes)
				{
					SIxDefaultAuthorityDes_t *pstDADes = (SIxDefaultAuthorityDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
					if (NULL != pstDADes)
					{
						(void)DB_SVC_SetTsDefaultAuthority(pstTsInfo, (const HUINT8 *)pstDADes->pszDefaultAuthority);
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDADes);
						break;
					}
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SrchSetDefaultAuthoriyInSvc_AusDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, DbSvc_Info_t *pstSvc)
{
	HUINT8 				*pucRawDes = NULL;
	HxList_t				*pstSdtSvcLoop = NULL;
	SIxTable_PsiTable_t		*pstCurrSdt = NULL;

	if ((pstSdtTable != NULL) && (pstSvc != NULL))
	{
		(void)DB_SVC_ResetServiceDefaultAuthority(pstSvc);

		for (pstCurrSdt = pstSdtTable; NULL != pstCurrSdt; pstCurrSdt = pstCurrSdt->pstNextTable)
		{
			if (pstCurrSdt->unInfo.stSdt.usTsId == DbSvc_GetTsId(pstSvc) && pstCurrSdt->unInfo.stSdt.usOrgNetId == DbSvc_GetOnId(pstSvc))
			{
				for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
				{
					SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

					if (pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(pstSvc))
					{
						pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pstSdtSvcItem, eSIxDESCTAG_DEFAULT_AUTHORITY, 0);
						if (NULL != pucRawDes)
						{
							SIxDefaultAuthorityDes_t *pstDADes = (SIxDefaultAuthorityDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
							if (NULL != pstDADes)
							{
								(void)DB_SVC_SetServiceDefaultAuthority(pstSvc, (const HUINT8 *)pstDADes->pszDefaultAuthority);
								PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDADes);
								break;
							}
						}
					}
				}
			}
		}
	}

	return ERR_OK;
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_SrchMsgCmdStartSearch_AusDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	if (pstMsg == NULL)
	{
		HxLOG_Error("Error !!  null parameter !! \n");
		return;
	}

	/* NETWORK search 여부와 상관없이 호주향 지상파 경우는 LCN할당을 위해 NIT를 걸어야 함 */
	pstMsg->unArg.stStartSrch.stOption.ulNetId = SvcSi_NETID_SEARCH_NITACT;

	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstMsg->hAction));

	(void)svc_si_SrchInitSearchData (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stStartSrch.unSpec), &(pstMsg->unArg.stStartSrch.stOption));
	(void)svc_si_SrchInitSearchResultTable (pstSrch);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxSPEC_AUSDTT);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)svc_si_SrchGetFilteringTypes (pstSrch);
	(void)svc_si_SrchStartSection (pstSrch);
}

STATIC void xsvc_si_SrchMsgCmdStopSearch_AusDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_SrchStopSection (pstSrch);
	(void)svc_si_SrchFreeSearchData (pstSrch);
	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstSrch->hAction));
}

STATIC void xsvc_si_SrchMsgEvtPatTable_AusDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnPatReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtPmtTable_AusDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtSdtTable_AusDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitTable_AusDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitOthTable_AusDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}


STATIC void xsvc_si_SrchProcessMsg_AusDtt (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL == pstSrch)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_SEARCH:
		xsvc_si_SrchMsgCmdStartSearch_AusDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_CMD_STOP_SEARCH:
		xsvc_si_SrchMsgCmdStopSearch_AusDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_SrchMsgEvtPatTable_AusDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_SrchMsgEvtPmtTable_AusDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		xsvc_si_SrchMsgEvtSdtTable_AusDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		xsvc_si_SrchMsgEvtNitTable_AusDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_OTH_TABLE:
		xsvc_si_SrchMsgEvtNitOthTable_AusDtt (pstSrch, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

HERROR xsvc_si_SrchCollectServiceDataFromSi_AusDtt (void *pvData)
{
	svcSi_SrchData_t 		*pstSrch = NULL;
	HUINT32					 ulActionId = 0;
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

	if (pvData == NULL)
	{
		HxLOG_Error("null parameter error ! \n");
		return ERR_FAIL;
	}
	pstSrch = (svcSi_SrchData_t *)pvData;
	ulActionId = PAL_PIPE_GetActionId (pstSrch->hAction);

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	pstPatTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);

	// Network
	if (NULL != pstNitTable)
	{
		stNetInfo.usNetId = pstNitTable->unInfo.stNit.usNetId;
		pucRawDes = SILIB_TABLE_Get1stLoopDescriptor (pstNitTable, eSIxDESCTAG_NETWORK_NAME, 0);
		if (NULL != pucRawDes)
		{
			SIxNetNameDes_t	*pstNameDes = (SIxNetNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstNameDes)
			{
				HxSTD_StrNCpy (stNetInfo.szNetName, pstNameDes->pszNetName, DxMAX_NET_NAME_LEN - 1);
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstNameDes);
			}
		}

		xsvc_si_SrchSetDefaultAuthoriyInNetwork_AusDtt(ulActionId, pstNitTable, &stNetInfo);
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

	if ( (NULL != pstNitTable) && (NULL != pstSdtTable) )
	{
		HxList_t *pstItem;
		for (pstItem = pstNitTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem);

			if ( (pst2ndLoop->unInfo.stNit.usTsId == stTsInfo.usTsId)
				&& (pst2ndLoop->unInfo.stNit.usOrgNetId == stTsInfo.usOnId) )
			{
				pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndLoop, eSIxDESCTAG_FREQUENCY_LIST, 0);
				if (pucRawDes != NULL)
				{
					stTsInfo.utExt.stAusDtt.bFreqListed = TRUE;
					break;
				}
			}
		}

		xsvc_si_SrchSetDefaultAuthoriyInTS_AusDtt(ulActionId, pstNitTable, pstSdtTable, &stTsInfo);
	}

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
				if (xsvc_si_LibSrchCompareSvcType(pstSrch->stOption.ulSvcType, DbSvc_GetSvcTypePtr(pstSvc)) != ERR_OK)
				{
					// search option에 설정된 svc type과 다르다
					continue;
				}

				if (xsvc_si_LibSrchCompareCasType(pstSrch->stOption.ulCasType, DbSvc_GetCasType(pstSvc)) != ERR_OK)
				{
					// search option에 설정된 cas type과 다르다
					continue;
				}

				xsvc_si_SrchSetDefaultAuthoriyInSvc_AusDtt(ulActionId, pstSdtTable, pstSvc);

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

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SrchGetSearchActionFunc_AusDtt (void)
{
	return xsvc_si_SrchProcessMsg_AusDtt;
}

void *xsvc_si_SrchAllocSearchActionData_AusDtt (void)
{
	svcSi_SrchData_t	*pstSrch;

	pstSrch = (svcSi_SrchData_t *)OxSI_Calloc (sizeof(svcSi_SrchData_t));
	if (NULL != pstSrch)
	{
		pstSrch->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstSrch;
}

void xsvc_si_SrchFreeSearchActionData_AusDtt (void *pvData)
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
/* end of file */

