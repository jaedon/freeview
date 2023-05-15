/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_search_ziggo.c
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
#define ZIGGO_FAST_CHANNEL_SCAN (1)


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
STATIC void xsvc_si_SrchMsgCmdStartSearch_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
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

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxSPEC_ZIGGO);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxTEXT_ENC_300468_8859_9_BASE);
	(void)svc_si_SrchGetFilteringTypes (pstSrch);

#if (ZIGGO_FAST_CHANNEL_SCAN == 1)
	pstSrch->eFiltering = bSI_FILTERING_NIT_ACT | bSI_FILTERING_SDT_ACT | bSI_FILTERING_SDT_OTH;
#endif

	(void)svc_si_SrchStartSection (pstSrch);
}

STATIC void xsvc_si_SrchMsgCmdStopSearch_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_SrchStopSection (pstSrch);
	(void)svc_si_SrchFreeSearchData (pstSrch);
	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstSrch->hAction));
}

STATIC void xsvc_si_SrchMsgEvtPatTable_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnPatReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtPmtTable_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtSdtTable_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtSdtOthTable_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitTable_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitOthTable_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchProcessMsg_Ziggo (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL == pstSrch)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_SEARCH:
		HxLOG_Print("===> Start SI retriveing....\n");
		xsvc_si_SrchMsgCmdStartSearch_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_CMD_STOP_SEARCH:
		HxLOG_Print("===> Start SI retriveing....\n");
		xsvc_si_SrchMsgCmdStopSearch_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Print("===> Get PAT table....\n");
		xsvc_si_SrchMsgEvtPatTable_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Print("===> Get PMT table....\n");
		xsvc_si_SrchMsgEvtPmtTable_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		HxLOG_Print("===> Get act_SDT table....\n");
		xsvc_si_SrchMsgEvtSdtTable_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_OTH_TABLE:
		HxLOG_Print("===> Get oth_SDT table....\n");
		xsvc_si_SrchMsgEvtSdtOthTable_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		HxLOG_Print("===> Get act_NIT table....\n");
		xsvc_si_SrchMsgEvtNitTable_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_OTH_TABLE:
		HxLOG_Print("===> Get oth_NIT table....\n");
		xsvc_si_SrchMsgEvtNitOthTable_Ziggo (pstSrch, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#if (ZIGGO_FAST_CHANNEL_SCAN == 1)
STATIC HERROR xsvc_si_SrchEsComposeServiceInfoBySdt_Ziggo (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, SIxTable_2ndLoop_t *pstNit2ndLoop,
																				HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HUINT16					 usPmtPid = PID_NULL;
	HUINT32					 ulSvcNum = 0, ulPrvNum = 0;
	HxList_t					*pstSdtSvcLoop, *pstSdtSvcDesLoop;
	SIxTable_PsiTable_t		*pstCurrSdt = NULL;
	DbSvc_Info_t				*pstSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;
	DxOperator_e			 	eSvcOpType = eDxOPERATOR_None;
	HxList_t					*pstNitSvcListDesLoop;
	HBOOL					bFoundSvcAtSvcListDesInNit = TRUE;

	if (pstSdtTable == NULL || pstNit2ndLoop == NULL) 		{ return ERR_FAIL; }


	for (pstCurrSdt = pstSdtTable; NULL != pstCurrSdt; pstCurrSdt = NULL)
	{
		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t *pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

#if (0)
			bFoundSvcAtSvcListDesInNit = FALSE;

			for (pstNitSvcListDesLoop = pstNit2ndLoop->pstDesLoop; NULL != pstNitSvcListDesLoop; pstNitSvcListDesLoop = pstNitSvcListDesLoop->next)
			{
				HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstNitSvcListDesLoop);
				switch (*pucDesRaw)
				{
					case eSIxDESCTAG_SERVICE_LIST:
					{
						SIxSvcListDes_t *pstSvcListDes = (SIxSvcListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

						if (NULL != pstSvcListDes)
						{
							SIxSvcListInfo_t *pSvcListItem = pstSvcListDes->pstInfo;
							while(pSvcListItem)
							{
								if (pstSdtSvcItem->unInfo.stSdt.usSvcId == pSvcListItem->usSvcId)
								{
									bFoundSvcAtSvcListDesInNit = TRUE;
									break;
								}
								pSvcListItem = pSvcListItem->pstNext;
							}

							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcListDes);
						}
					}
				}
			}
#endif

			if (bFoundSvcAtSvcListDesInNit == TRUE)
			{
				for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
				{
					HUINT8	*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);
					if (eSIxDESCTAG_SERVICE == *pucRawDes)
					{
						ulSvcNum ++;
					}
				}
			}
		}
	}

	if (0 == ulSvcNum)
	{
		HxLOG_Error ("No Service found in SDT\n");
		goto END_FUNC;
	}

	pstSvcArray = (DbSvc_Info_t *)OxSI_Calloc (sizeof(DbSvc_Info_t) * ulSvcNum);
	pstPrvArray = (DbSvc_PrvInfo_t *)OxSI_Calloc (sizeof(DbSvc_PrvInfo_t) * ulSvcNum);
	if ((NULL == pstSvcArray) || (NULL == pstPrvArray))
	{
		HxLOG_Error ("Memory allocation failed\n");
		goto END_FUNC;
	}

	ulSvcNum = 0;
	for (pstCurrSdt = pstSdtTable; NULL != pstCurrSdt; pstCurrSdt = NULL)
	{
		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t *pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

#if (0)
			bFoundSvcAtSvcListDesInNit = FALSE;

			for (pstNitSvcListDesLoop = pstNit2ndLoop->pstDesLoop; NULL != pstNitSvcListDesLoop; pstNitSvcListDesLoop = pstNitSvcListDesLoop->next)
			{
				HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstNitSvcListDesLoop);
				switch (*pucDesRaw)
				{
					case eSIxDESCTAG_SERVICE_LIST:
					{
						SIxSvcListDes_t *pstSvcListDes = (SIxSvcListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

						if (NULL != pstSvcListDes)
						{
							SIxSvcListInfo_t *pSvcListItem = pstSvcListDes->pstInfo;
							while(pSvcListItem)
							{
								if (pstSdtSvcItem->unInfo.stSdt.usSvcId == pSvcListItem->usSvcId)
								{
									bFoundSvcAtSvcListDesInNit = TRUE;
									break;
								}
								pSvcListItem = pSvcListItem->pstNext;

							}
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcListDes);
						}
					}
				}
			}
#endif

			if (bFoundSvcAtSvcListDesInNit == TRUE)
			{
				for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
				{
					HUINT8 *pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);
					DbSvc_Info_t	*pstSvcInfo = &(pstSvcArray[ulSvcNum]);

					if (eSIxDESCTAG_SERVICE == *pucRawDes)
					{
						SIxSvcDes_t *pstSvcDes = (SIxSvcDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

						if (NULL != pstSvcDes)
						{
							DxSvcType_e			eSvcType;
							DxVideoType_e		eVideoType;

							DB_SVC_InitServiceInfo (pstSvcInfo);

							DbSvc_SetOnId(pstSvcInfo, pstCurrSdt->unInfo.stSdt.usOrgNetId);
							DbSvc_SetTsId(pstSvcInfo, pstCurrSdt->unInfo.stSdt.usTsId);
							DbSvc_SetSvcId(pstSvcInfo, pstSdtSvcItem->unInfo.stSdt.usSvcId);

							svc_si_LibCheckServiceNameBySvcDes (pstSvcDes->ucSvcName, pstSvcDes->pszServName, DbSvc_GetName(pstSvcInfo), NULL);

							HxLOG_Print("Service Info(0x%x, 0x%x, 0x%x:%s)\n", pstSvcInfo->usOnId, pstSvcInfo->usTsId, pstSvcInfo->usSvcId, pstSvcInfo->szSvcName);

							// Provider
							if ((pstSvcDes->ucPrvName > 0) && (NULL != pstSvcDes->pszProviderName))
							{
								HUINT32			 ulPrvCnt, ulPrvLen = (pstSvcDes->ucPrvName < DxMAX_PRV_NAME_LEN) ? (HUINT32)pstSvcDes->ucPrvName : DxMAX_PRV_NAME_LEN;
								HCHAR			 szPrvName[DxMAX_PRV_NAME_LEN + 1];

								HxSTD_StrNCpy (szPrvName, pstSvcDes->pszProviderName, ulPrvLen);
								szPrvName[ulPrvLen] = '\0';

								for (ulPrvCnt = 0; ulPrvCnt < ulPrvNum; ulPrvCnt++)
								{
									if (HxSTD_StrNCmp ((HCHAR*)pstPrvArray[ulPrvCnt].szPrvName, szPrvName, DxMAX_PRV_NAME_LEN) == 0)
									{
										break;
									}
								}

								if (ulPrvCnt < ulPrvNum)
								{
									DbSvc_SetPrvIdx(pstSvcInfo, ulPrvCnt);
								}
								else
								{
									DbSvc_SetPrvIdx(pstSvcInfo, ulPrvNum);
									pstPrvArray[ulPrvNum].usPrvIdx = (HUINT16)ulPrvNum;
									HxSTD_StrNCpy ((HCHAR*)pstPrvArray[ulPrvNum].szPrvName, szPrvName, DxMAX_PRV_NAME_LEN);
									pstPrvArray[ulPrvNum].szPrvName[DxMAX_PRV_NAME_LEN - 1] = '\0';

									ulPrvNum ++;
								}
							}

#if (0)
							// PIDs
							hErr = svc_si_LibGetPmtPidFromPat (ulActionId, DbSvc_GetSvcId(pstSvcInfo), &usPmtPid);
							DbSvc_SetPmtPid(pstSvcInfo, (ERR_OK == hErr) ? usPmtPid : PID_NULL);

							{
								SvcSi_UserSettings_t stUserSettings = { { 0, }, };

								(void)svc_si_SvcInfo2UserSettings((const DbSvc_Info_t *)pstSvcInfo, &stUserSettings);

								hErr = svc_si_LibGetServiceInfoPid (ulActionId, DbSvc_GetSvcId(pstSvcInfo), pstSvcInfo, pstSvcInfo, svc_si_GetSysConfig(&stUserSettings), NULL);
								if (ERR_OK != hErr)
								{
									HxLOG_Debug ("svc_si_LibGetServiceInfoPid err: SvcId(0x%04x)\n", DbSvc_GetSvcId(pstSvcInfo));
								}
							}
#endif

							// Service type, Video Type
							eSvcType = eDxSVC_TYPE_All;
							eVideoType = eDxVIDEO_TYPE_ALL;
							hErr = svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (ulActionId, pstSvcDes->ucServType, &eSvcType, &eVideoType);
							if (ERR_OK != hErr)
							{
								HxLOG_Error ("svc_si_LibGetSvcAndVideoTypeFromDvbSvcType err: SvcType(%d)\n", pstSvcDes->ucServType);
							}

							DbSvc_SetSvcType(pstSvcInfo, eSvcType);
							DbSvc_SetVideoType(pstSvcInfo, eVideoType);

							// orgSvcType 은 Dvb Svc Type을 그대로 저장한다..by chmin
							//DbSvc_SetOrgSvcTypePtr(pstSvcInfo, DbSvc_GetSvcTypePtr(pstSvcInfo));
							DbSvc_SetOrgSvcType(pstSvcInfo, pstSvcDes->ucServType);

							// op type
							hErr = xsvc_si_LibSrchGetServiceOptype(ulActionId, &eSvcOpType);
							if (hErr != ERR_OK)
							{
								HxLOG_Error("xsvc_si_LibSrchGetServiceOptype() return failed..!! ulActionId(%d) \n", ulActionId);
								eSvcOpType = eDxOPERATOR_None;
							}
							DbSvc_SetOpType(pstSvcInfo, eSvcOpType);

							ulSvcNum++;
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcDes);
						}
					}
				}
			}
		}
	}

	if (NULL != pulSvcNum)						{ *pulSvcNum = ulSvcNum; }
	if (NULL != ppstSvcArray)
	{
		if (ulSvcNum > 0)
		{
			*ppstSvcArray = pstSvcArray;
			pstSvcArray = NULL;
		}
		else
		{
			*ppstSvcArray = NULL;
		}
	}

	if (NULL != pulPrvNum)						{ *pulPrvNum = ulPrvNum; }
	if (NULL != ppstPrvArray)
	{
		if (ulPrvNum > 0)
		{
			*ppstPrvArray = pstPrvArray;
			pstPrvArray = NULL;
		}
		else
		{
			*ppstPrvArray = NULL;
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSvcArray)					{ OxSI_Free (pstSvcArray); }
	if (NULL != pstPrvArray)					{ OxSI_Free (pstPrvArray); }

	return hResult;
}

STATIC HERROR xsvc_si_SrchEsComposeServiceInfoWithoutSdt_Ziggo (HUINT32 ulActionId, SIxTable_2ndLoop_t *pstNit2ndLoop, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HERROR					 hResult = ERR_FAIL;
	HERROR					 hErr = ERR_OK;
	HUINT32					 ulSvcNum = 0;
	DbSvc_Info_t				*pstSvcArray = NULL;
	HUINT16					 usPmtPid = PID_NULL;
	HUINT32					 ulDvbSvcType;
	DxOperator_e			 	 eSvcOpType = eDxOPERATOR_None;
	HxList_t					*pstNitSvcListDesLoop;

	if (pstNit2ndLoop == NULL) 		{ return ERR_FAIL; }

	for (pstNitSvcListDesLoop = pstNit2ndLoop->pstDesLoop; NULL != pstNitSvcListDesLoop; pstNitSvcListDesLoop = pstNitSvcListDesLoop->next)
	{
		HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstNitSvcListDesLoop);
		switch (*pucDesRaw)
		{
			case eSIxDESCTAG_SERVICE_LIST:
			{
				SIxSvcListDes_t *pstSvcListDes = (SIxSvcListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

				if (NULL != pstSvcListDes)
				{
					SIxSvcListInfo_t *pSvcListItem = pstSvcListDes->pstInfo;
					while(pSvcListItem)
					{
						ulSvcNum++;
						pSvcListItem = pSvcListItem->pstNext;
					}
					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcListDes);
				}
			}
		}
	}

	if (0 == ulSvcNum)
	{
		HxLOG_Error ("no pmt found...!! \n");
		goto END_FUNC;
	}

	pstSvcArray = (DbSvc_Info_t *)OxSI_Calloc (sizeof(DbSvc_Info_t) * ulSvcNum);
	if (NULL == pstSvcArray)
	{
		HxLOG_Error ("Memory allocation failed\n");
		goto END_FUNC;
	}

	ulSvcNum = 0;
	for (pstNitSvcListDesLoop = pstNit2ndLoop->pstDesLoop; NULL != pstNitSvcListDesLoop; pstNitSvcListDesLoop = pstNitSvcListDesLoop->next)
	{
		HUINT8			*pszUndefinedSvcName = NULL;
		DxSvcType_e 		eSvcType;
		DxVideoType_e	eVideoType;

		HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstNitSvcListDesLoop);
		switch (*pucDesRaw)
		{
			case eSIxDESCTAG_SERVICE_LIST:
			{
				SIxSvcListDes_t *pstSvcListDes = (SIxSvcListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

				if (NULL != pstSvcListDes)
				{
					SIxSvcListInfo_t *pSvcListItem = pstSvcListDes->pstInfo;
					while(pSvcListItem)
					{
						DbSvc_Info_t	*pstSvcInfo = &(pstSvcArray[ulSvcNum]);
						DB_SVC_InitServiceInfo (pstSvcInfo);

						DbSvc_SetOnId(pstSvcInfo, pstNit2ndLoop->unInfo.stNit.usOrgNetId);
						DbSvc_SetTsId(pstSvcInfo, pstNit2ndLoop->unInfo.stNit.usTsId);
						DbSvc_SetSvcId(pstSvcInfo, pSvcListItem->usSvcId);

						// Service name
						hErr = svc_si_LibGetUndefinedServiceName(ulActionId, &pszUndefinedSvcName);
						if ((hErr == ERR_OK) && (pszUndefinedSvcName != NULL))
						{
							//HLIB_STD_StrUtf8NCpy(pstSvcInfo->szSvcName, pszUndefinedSvcName, CONFIG_DB_SVC_MAX_NAME_BYTES-1);
							DbSvc_SetName(pstSvcInfo, pszUndefinedSvcName);
						}

#if (0)
						// PIDs
						hErr = svc_si_LibGetPmtPidFromPat (ulActionId, DbSvc_GetSvcId(pstSvcInfo), &usPmtPid);
						DbSvc_SetPmtPid(pstSvcInfo, (ERR_OK == hErr) ? usPmtPid : PID_NULL);
						{
							SvcSi_UserSettings_t stUserSettings = { { 0, }, };

							(void)svc_si_SvcInfo2UserSettings((const DbSvc_Info_t *)pstSvcInfo, &stUserSettings);

							hErr = svc_si_LibGetServiceInfoPid (ulActionId, DbSvc_GetSvcId(pstSvcInfo), pstSvcInfo, pstSvcInfo, svc_si_GetSysConfig(&stUserSettings), NULL);
							if (ERR_OK != hErr)
							{
								HxLOG_Debug ("svc_si_LibGetServiceInfoPid err: SvcId(0x%04x)\n", DbSvc_GetSvcId(pstSvcInfo));
							}
						}
#endif

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

						HxLOG_Print("Service Info(0x%x, 0x%x, 0x%x:%s), SvcType(%d, %d)\n", pstSvcInfo->usOnId, pstSvcInfo->usTsId, pstSvcInfo->usSvcId, pstSvcInfo->szSvcName, ulDvbSvcType, eSvcType);

						DbSvc_SetSvcType(pstSvcInfo, eSvcType);
						DbSvc_SetVideoType(pstSvcInfo, eVideoType);

						// orgSvcType 은 Dvb Svc Type을 그대로 저장한다..by chmin
						//DbSvc_SetOrgSvcTypePtr(pstSvcInfo, DbSvc_GetSvcTypePtr(pstSvcInfo));
						DbSvc_SetOrgSvcType(pstSvcInfo, ulDvbSvcType);

						// op type
						hErr = xsvc_si_LibSrchGetServiceOptype(ulActionId, &eSvcOpType);
						if (hErr != ERR_OK)
						{
							HxLOG_Error("xsvc_si_LibSrchGetServiceOptype() return failed..!! ulActionId(%d) \n", ulActionId);
							eSvcOpType = eDxOPERATOR_None;
						}
						DbSvc_SetOpType(pstSvcInfo, eSvcOpType);

						ulSvcNum++;
						pSvcListItem = pSvcListItem->pstNext;
					}
					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcListDes);
				}
			}
		}
	}

	if (NULL != pulSvcNum)						{ *pulSvcNum = ulSvcNum; }
	if (NULL != ppstSvcArray)
	{
		if (ulSvcNum > 0)
		{
			*ppstSvcArray = pstSvcArray;
			pstSvcArray = NULL;
		}
		else
		{
			*ppstSvcArray = NULL;
		}
	}

	// pmt로는 provider info를 채울 수가 없다..
	if (NULL != pulPrvNum)						{ *pulPrvNum = 0; }
	if (NULL != ppstPrvArray)					{ *ppstPrvArray = NULL; }

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSvcArray)					{ OxSI_Free (pstSvcArray); }

	return hResult;
}
#endif

#define ___MEMBER_FUNCTIONS___
#if (	ZIGGO_FAST_CHANNEL_SCAN == 1)
HERROR xsvc_si_SrchCollectServiceDataFromSi_Ziggo (void *pvData)
{
	svcSi_SrchData_t 			*pstSrch = NULL;
	HUINT32					 ulActionId = 0;
	HUINT32 				 ulCnt, ulTsNum = 0, ulSvcNum = 0, ulPrvNum = 0;
	HUINT32					 ulNetIdx = 0;
	DbSvc_TsInfo_t			 stTsInfo, stCurTsInfo;
	DbSvc_NetInfo_t			 stNetInfo;
	HUINT8					*pucRawDes;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	DbSvc_Info_t				*pstSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SIxTable_PsiTable_t 		*pstNitTable = NULL, *pstSdtTable = NULL, *pstSdtActTable = NULL, *pstSdtOthTable = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;
	HxList_t					*pstItem1, *pstItem2;

	if (pvData == NULL)
	{
		HxLOG_Error("null parameter error ! \n");
		return ERR_FAIL;
	}
	pstSrch = (svcSi_SrchData_t *)pvData;
	ulActionId = PAL_PIPE_GetActionId (pstSrch->hAction);

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	pstSdtActTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	pstSdtOthTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

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
	}

	(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Network, (void *)&stNetInfo, &ulNetIdx);


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


	// TS Info & Service Info
	// Get Current TS info from act_SDT
	if (NULL != pstSdtActTable)
	{
		stCurTsInfo.usOnId = pstSdtActTable->unInfo.stSdt.usOrgNetId;
		stCurTsInfo.usTsId	= pstSdtActTable->unInfo.stSdt.usTsId;

		HxLOG_Print("\n Current TP info from actSDT (0x%x, 0x%x)\n", stCurTsInfo.usOnId, stCurTsInfo.usTsId);
	}

	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		memset(&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
		stTsInfo.usNetIdx = (HUINT16)ulNetIdx;
		stTsInfo.usOnId = pst2ndLoop->unInfo.stNit.usOrgNetId;
		stTsInfo.usTsId = pst2ndLoop->unInfo.stNit.usTsId;

		HxLOG_Print("\n\n\n");
		HxLOG_Print("==> TP info from actNIT (0x%x, 0x%x)\n", stTsInfo.usOnId, stTsInfo.usTsId);

		pstSdtTable = NULL;
		if ( (stTsInfo.usOnId == stCurTsInfo.usOnId)
			&& (stTsInfo.usTsId == stCurTsInfo.usTsId) )
		{
			HxLOG_Print("==> This TP is current one (0x%x, 0x%x), assigned to actSDT\n", stTsInfo.usOnId, stTsInfo.usTsId);
			pstSdtTable = pstSdtActTable;
			stTsInfo.usTsIdx = 1;   // current TS 일때만 1로 setting
		}
		else
		{
			SIxTable_PsiTable_t *pstSiTable = pstSdtOthTable;
			while (pstSiTable != NULL)
			{
				if ( (stTsInfo.usOnId == pstSiTable->unInfo.stSdt.usOrgNetId)
					&& (stTsInfo.usTsId == pstSiTable->unInfo.stSdt.usTsId) )
				{
					HxLOG_Print("==> This TP is not current one (0x%x, 0x%x), assigned to othSDT\n", stTsInfo.usOnId, stTsInfo.usTsId);
					pstSdtTable = pstSiTable;
					break;
				}
				pstSiTable = pstSiTable->pstNextTable;
			}
		}

		(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Transponder, (void *)&stTsInfo, NULL);

		// Service Info
		ulSvcNum = ulPrvNum = 0;
		pstSvcArray = pstPrvArray = NULL;

		if (NULL != pstSdtTable)
		{
			HxLOG_Print("==> Found SDT table (0x%x, 0x%x)\n", stTsInfo.usOnId, stTsInfo.usTsId);
			hErr = xsvc_si_SrchEsComposeServiceInfoBySdt_Ziggo(ulActionId, pstSdtTable, pst2ndLoop, &ulSvcNum, &pstSvcArray, &ulPrvNum, &pstPrvArray);
		}
		else
		{
			HxLOG_Print("==> Not found SDT table (0x%x, 0x%x)\n", stTsInfo.usOnId, stTsInfo.usTsId);
			hErr = xsvc_si_SrchEsComposeServiceInfoWithoutSdt_Ziggo(ulActionId, pst2ndLoop, &ulSvcNum, &pstSvcArray, &ulPrvNum, &pstPrvArray);
		}

		HxLOG_Print("==> Search channel info the report is err = 0x%x, numSvc = %d, numPrv = %d\n", hErr, ulSvcNum, ulPrvNum);

		if (ERR_OK == hErr)
		{
			if (NULL != pstSvcArray)
			{
				DbSvc_Info_t	*pstSvc = NULL;

				for (ulCnt = 0; ulCnt < ulSvcNum; ulCnt++)
				{
					pstSvc = &pstSvcArray[ulCnt];
				#if (1)
					pstSrch->stOption.ulSvcType = eDxSVC_TYPE_All;
					pstSrch->stOption.ulCasType = eDxCAS_TYPE_All;
				#endif
					if (xsvc_si_LibSrchCompareSvcType(pstSrch->stOption.ulSvcType, DbSvc_GetSvcType(pstSvc)) != ERR_OK)
					{
						// search option에 설정된 svc type과 다르다
						HxLOG_Print("==> Not added service : SvcType = %d\n", DbSvc_GetSvcType(pstSvc));
						continue;
					}

					if (xsvc_si_LibSrchCompareCasType(pstSrch->stOption.ulCasType, DbSvc_GetCasType(pstSvc)) != ERR_OK)
					{
						// search option에 설정된 cas type과 다르다
						HxLOG_Print("==> Not added service : CasType = %d\n", DbSvc_GetCasType(pstSvc));
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

		if (NULL != pstSvcArray)	{ OxSI_Free (pstSvcArray); pstSvcArray = NULL;}
		if (NULL != pstPrvArray)	{ OxSI_Free (pstPrvArray); pstPrvArray = NULL;}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstTsArray)				{ OxSI_Free (pstTsArray); }
	if (NULL != pstSvcArray)				{ OxSI_Free (pstSvcArray); }
	if (NULL != pstPrvArray)				{ OxSI_Free (pstPrvArray); }
	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	if (NULL != pstSdtActTable)				{ PAL_SIRAWPOOL_FreeTable (pstSdtActTable); }
	if (NULL != pstSdtOthTable)			{ PAL_SIRAWPOOL_FreeTable (pstSdtOthTable); }

	return hResult;
}
#else
HERROR xsvc_si_SrchCollectServiceDataFromSi_Ziggo (void *pvData)
{
	svcSi_SrchData_t 			*pstSrch = NULL;
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
	if (NULL != pstTsArray)				{ OxSI_Free (pstTsArray); }
	if (NULL != pstSvcArray)				{ OxSI_Free (pstSvcArray); }
	if (NULL != pstPrvArray)				{ OxSI_Free (pstPrvArray); }
	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	if (NULL != pstSdtTable)				{ PAL_SIRAWPOOL_FreeTable (pstSdtTable); }
	if (NULL != pstPatTable)				{ PAL_SIRAWPOOL_FreeTable (pstPatTable); }

	return hResult;
}
#endif


#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SrchGetSearchActionFunc_Ziggo (void)
{
	return xsvc_si_SrchProcessMsg_Ziggo;
}

void *xsvc_si_SrchAllocSearchActionData_Ziggo (void)
{
	svcSi_SrchData_t	*pstSrch;

	pstSrch = (svcSi_SrchData_t *)OxSI_Calloc (sizeof(svcSi_SrchData_t));
	if (NULL != pstSrch)
	{
		pstSrch->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstSrch;
}

void xsvc_si_SrchFreeSearchActionData_Ziggo (void *pvData)
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

