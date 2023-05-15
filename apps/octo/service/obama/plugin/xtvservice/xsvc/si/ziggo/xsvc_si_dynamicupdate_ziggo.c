/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_dynamicupdate_ziggo.c
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
#include <_svc_si_dynamicupdate.h>
#include <_xsvc_si_lib_search_base.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define NEED_PAT_TBL 	(0)

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

STATIC HERROR xsvc_si_DuEsComposeServiceInfoBySdt_Ziggo (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, SIxTable_2ndLoop_t *pstNit2ndLoop,
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

STATIC HERROR xsvc_si_DuEsComposeServiceInfoWithoutSdt_Ziggo (HUINT32 ulActionId, SIxTable_2ndLoop_t *pstNit2ndLoop, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
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
	if (NULL != pulPrvNum)					{ *pulPrvNum = 0; }
	if (NULL != ppstPrvArray)					{ *ppstPrvArray = NULL; }

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSvcArray)					{ OxSI_Free (pstSvcArray); }

	return hResult;
}

STATIC HERROR xsvc_si_DuCollectServiceDataFromSi_Ziggo (svcSi_SrchData_t *pstSrch)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (pstSrch->hAction);
	HUINT32 				 ulCnt, ulTsNum = 0, ulSvcNum = 0, ulPrvNum = 0;
	HUINT32					 ulNetIdx;
	DbSvc_TsInfo_t			 stCurTsInfo, stTsInfo;
	DbSvc_NetInfo_t			 stNetInfo;
	HUINT8					*pucRawDes;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	DbSvc_Info_t				*pstSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
#if (NEED_PAT_TBL == 1)
	SIxTable_PsiTable_t		*pstPatTable = NULL;
#endif
	SIxTable_PsiTable_t		*pstNitTable = NULL, *pstSdtTable = NULL, *pstSdtActTable = NULL, *pstSdtOthTable = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	HxLOG_Debug("pstSrch->hAction: 0x%x, ulActionId: 0x%x \n", pstSrch->hAction, ulActionId);

#if (NEED_PAT_TBL == 1)
	pstPatTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
#endif
	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	pstSdtActTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	pstSdtOthTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

	// other sdt를 못받은 경우에는 actul sdt만 처리 하도록 하자.
	if ( (pstNitTable == NULL) || (pstSdtActTable == NULL) )
	{
		goto END_FUNC;
	}

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

	(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Network, (void *)&stNetInfo, &ulNetIdx);


	// TS Info
	HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
	if (NULL != pstSdtActTable)
	{
		stCurTsInfo.usOnId	= pstSdtActTable->unInfo.stSdt.usOrgNetId;
		stCurTsInfo.usTsId		= pstSdtActTable->unInfo.stSdt.usTsId;
	}
#if (NEED_PAT_TBL == 1)
	else if (NULL != pstPatTable)
	{
		stCurTsInfo.usTsId = pstPatTable->unInfo.stPat.usTsId;
	}
#endif
	else
	{
		HxLOG_Error ("No way to make the TS Info:\n");
		goto END_FUNC;
	}

	// Delivery TS Info :
	if (NULL != pstNitTable)
	{
		hErr = svc_si_LibGetAllDeliveryTsInfoFromNit (ulActionId, TRUE, TRUE, &ulTsNum, &pstTsArray);
		if ((ERR_OK == hErr) && (0 != ulTsNum) && (NULL != pstTsArray))
		{
			HxList_t *pstItem;
			SIxTable_2ndLoop_t *pst2ndLoop;

			for (ulCnt = 0; ulCnt < ulTsNum; ulCnt++)
			{
				HxLOG_Debug("\t LoopCnt = %d, (OnId = 0x%x, tsId = 0x%x, freq = %d, BW = %d) \n", ulCnt,  pstTsArray[ulCnt].usOnId, pstTsArray[ulCnt].usTsId, pstTsArray[ulCnt].stTuningParam.ter.ulFreq, pstTsArray[ulCnt].stTuningParam.ter.ucBandwidth);

				(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Delivery, (void *)&(pstTsArray[ulCnt]), NULL);

				HxSTD_MemSet(&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
				stTsInfo.usNetIdx = (HUINT16)ulNetIdx;
				pstSdtTable = NULL;

				// if this TP is current one...
				if ( (stCurTsInfo.usOnId == pstTsArray[ulCnt].usOnId)
					&& (stCurTsInfo.usTsId == pstTsArray[ulCnt].usTsId) )
				{
					DxTuneParam_t stTuneInfo;
					DbSvc_TsInfo_t stLockTsInfo;

					SVC_CH_GetLockedInfo(ulActionId, &stTuneInfo);

					stLockTsInfo.stTuningParam.cab.ulFreq = stTuneInfo.cab.ulFrequency;
					stLockTsInfo.stTuningParam.cab.usSR = (HUINT16)stTuneInfo.cab.ulSymbolRate;
					stLockTsInfo.stTuningParam.cab.ucCon = stTuneInfo.cab.constellation;
					stLockTsInfo.stTuningParam.cab.ucSpec = stTuneInfo.cab.spectrum;

					if (DB_SVC_UtilCompareDbTuningParam(eDxDELIVERY_TYPE_CAB, &(stLockTsInfo.stTuningParam), &(pstTsArray[ulCnt].stTuningParam)) == 0)
					{
						HxSTD_MemCopy(&stTsInfo.stTuningParam, &pstTsArray[ulCnt].stTuningParam, sizeof(DbSvc_TuningInfo_t));
						HxLOG_Debug("\t Found the Locked TS (OnId = 0x%x, tsId = 0x%x, freq = %d, BW = %d) \n", pstTsArray[ulCnt].usOnId, pstTsArray[ulCnt].usTsId, pstTsArray[ulCnt].stTuningParam.ter.ulFreq, pstTsArray[ulCnt].stTuningParam.ter.ucBandwidth);
					}
					else
					{
						HxSTD_MemCopy(&stTsInfo.stTuningParam, &stLockTsInfo.stTuningParam, sizeof(DbSvc_TuningInfo_t));
					}

					pstSdtTable = pstSdtActTable;
				}
				else
				{
					HxSTD_MemCopy(&stTsInfo.stTuningParam, &pstTsArray[ulCnt].stTuningParam, sizeof(DbSvc_TuningInfo_t));

					if (pstSdtOthTable != NULL)
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
				}

				(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Transponder, (void *)&stTsInfo, NULL);
				HxLOG_Debug("Added TS Info ::: OnID = 0x%x, TsID = 0x%x\n", stTsInfo.usOnId, stTsInfo.usTsId);

				pst2ndLoop = NULL;
				for (pstItem = pstNitTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
				{
					pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem);

					if ( (pst2ndLoop->unInfo.stNit.usOrgNetId ==  pstTsArray[ulCnt].usOnId)
						&& (pst2ndLoop->unInfo.stNit.usTsId == pstTsArray[ulCnt].usTsId) )
					{
						break;
					}
				}

				// Service Info:
				ulSvcNum = ulPrvNum = 0;
				pstSvcArray = pstPrvArray = NULL;

				if (pstSdtTable == NULL)
					continue;

				hErr = xsvc_si_DuEsComposeServiceInfoBySdt_Ziggo (ulActionId, pstSdtTable, pst2ndLoop, &ulSvcNum, &pstSvcArray, &ulPrvNum, &pstPrvArray);
				if (ERR_OK == hErr)
				{
					if (NULL != pstSvcArray)
					{
						DbSvc_Info_t	*pstSvc = NULL;

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

							(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Service, (void *)pstSvc, NULL);
						}
					}

					if (NULL != pstPrvArray)
					{
						for (ulCnt = 0; ulCnt < ulPrvNum; ulCnt++)
						{
							(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Provider, (void *)&(pstPrvArray[ulCnt]), NULL);
						}
					}
				}

				if (NULL != pstSvcArray)	{ OxSI_Free (pstSvcArray); pstSvcArray = NULL; }
				if (NULL != pstPrvArray)	{ OxSI_Free (pstPrvArray); pstPrvArray = NULL; }
			}
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstTsArray)				{ OxSI_Free (pstTsArray); }
	if (NULL != pstSvcArray)				{ OxSI_Free (pstSvcArray); }
	if (NULL != pstPrvArray)				{ OxSI_Free (pstPrvArray); }
	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	if (NULL != pstSdtActTable)				{ PAL_SIRAWPOOL_FreeTable (pstSdtActTable); }
	if (NULL != pstSdtOthTable)			{ PAL_SIRAWPOOL_FreeTable (pstSdtOthTable); }
#if (NEED_PAT_TBL == 1)
	if (NULL != pstPatTable)				{ PAL_SIRAWPOOL_FreeTable (pstPatTable); }
#endif
	return hResult;
}

STATIC HERROR xsvc_si_DuCheckAndComposeSearchData_Ziggo (svcSi_SrchData_t *pstSrch)
{
	HINT32			 nMsgType;
	HERROR			 hErr;

	if (svc_si_DuCheckAllSiTableReceived (pstSrch) == TRUE)
	{
		hErr = xsvc_si_DuCollectServiceDataFromSi_Ziggo (pstSrch);
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
STATIC void xsvc_si_DuMsgCmdStartDynamicUpdate_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	SvcSi_SearchOption_t 	stSrchOption;

	HxSTD_MemSet(&stSrchOption, 0x00, sizeof(SvcSi_SearchOption_t));

	stSrchOption.ulNetId = SvcSi_NETID_SEARCH_NITACT;

	(void)svc_si_DuInitSearchData (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stStartDynamicUpdate.unSpec), &stSrchOption);
	(void)svc_si_DuInitSearchResultTable (pstSrch);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxSPEC_ZIGGO);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	pstSrch->eFiltering = (bSI_FILTERING_NIT_ACT
#if (NEED_PAT_TBL == 1)
							| bSI_FILTERING_PAT			/* compose할 때 ts id 참고 용도 */
#endif
							| bSI_FILTERING_SDT_ACT
							| bSI_FILTERING_SDT_OTH);
	(void)svc_si_DuStartSection (pstSrch);
}

STATIC void xsvc_si_DuMsgCmdStopDynamicUpdate_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_DuStopSection (pstSrch);
	(void)svc_si_DuFreeSearchData (pstSrch);
}

STATIC void xsvc_si_DuMsgEvtPatTable_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_DuCheckAndComposeSearchData_Ziggo (pstSrch);
}

STATIC void xsvc_si_DuMsgEvtNitTable_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_DuCheckAndComposeSearchData_Ziggo (pstSrch);
}

STATIC void xsvc_si_DuMsgEvtSdtTable_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_DuCheckAndComposeSearchData_Ziggo (pstSrch);
}

STATIC void xsvc_si_DuMsgEvtSdtOthTable_Ziggo (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_DuCheckAndComposeSearchData_Ziggo (pstSrch);
}

STATIC void xsvc_si_DuProcessMsg_Ziggo (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL == pstSrch)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_DYNAMICUPDATE:
		HxLOG_Debug("eSIMSG_CMD_START_DYNAMICUPDATE \n");
		xsvc_si_DuMsgCmdStartDynamicUpdate_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_CMD_STOP_DYNAMICUPDATE:
		HxLOG_Debug("eSIMSG_CMD_STOP_DYNAMICUPDATE \n");
		xsvc_si_DuMsgCmdStopDynamicUpdate_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Debug("eSIMSG_EVT_PAT_TABLE \n");
		xsvc_si_DuMsgEvtPatTable_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		HxLOG_Debug("eSIMSG_EVT_NIT_TABLE \n");
		xsvc_si_DuMsgEvtNitTable_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		HxLOG_Debug("eSIMSG_EVT_SDT_TABLE \n");
		xsvc_si_DuMsgEvtSdtTable_Ziggo (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_OTH_TABLE:
		HxLOG_Debug("eSIMSG_EVT_SDT_OTH_TABLE \n");
		xsvc_si_DuMsgEvtSdtOthTable_Ziggo (pstSrch, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_DuGetDynamicUpdateActionFunc_Ziggo (void)
{
	return xsvc_si_DuProcessMsg_Ziggo;
}

void *xsvc_si_DuAllocDynamicUpdateActionData_Ziggo (void)
{
	svcSi_SrchData_t	*pstSrch;

	pstSrch = (svcSi_SrchData_t *)OxSI_Calloc (sizeof(svcSi_SrchData_t));
	if (NULL != pstSrch)
	{
		pstSrch->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstSrch;
}

void xsvc_si_DuFreeDynamicUpdateActionData_Ziggo (void *pvData)
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

