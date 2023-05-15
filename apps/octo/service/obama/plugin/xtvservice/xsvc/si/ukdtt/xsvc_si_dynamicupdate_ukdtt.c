/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_dynamicupdate_ukdtt.c
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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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
#include <svc_ch.h>
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

STATIC HERROR xsvc_si_DuSetDefaultAuthoriyInNetwork_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, DbSvc_NetInfo_t *pstNetInfo)
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


STATIC HERROR xsvc_si_DuSetDefaultAuthoriyInTS_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, SIxTable_PsiTable_t *pstSdtTable, DbSvc_TsInfo_t *pstTsInfo)
{
	HUINT8 *pucRawDes;

	if ((pstNitTable != NULL) &&  (pstSdtTable != NULL ) && (pstTsInfo != NULL))
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

STATIC HERROR xsvc_si_DuSetDefaultAuthoriyInSvc_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, DbSvc_Info_t *pstSvc)
{
	HUINT8 				*pucRawDes;
	HxList_t				*pstSdtSvcLoop;
	SIxTable_PsiTable_t	*pstCurrSdt = NULL;

	if ((pstSdtTable != NULL ) && (pstSvc != NULL))
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

STATIC HERROR xsvc_si_DuSetGuidanceInfoInSvc_UkDtt(HUINT32 ulActionId,SIxTable_PsiTable_t *pstSdtTable, DbSvc_Info_t *pstSvc,HUINT32 ulActionSpec)
{
	HERROR				hErr;
	HUINT16 			usRealTsId, usRealOnId;
	SIxTable_PsiTable_t		*pstCurrSdt;
	HBOOL				bFoundPrivate = FALSE;
	HUINT32 			ulPrivateSpecifyValue;
	HxLANG_Id_e			eMenuLangId, eCurLangId, eUndLangId;
	HxList_t				*pstSdtSvcLoop = NULL, *pstSdtSvcDesLoop = NULL;
	SIxUkdttGuidanceDes_t 	*pstUkdttSelGuidanceDesc = NULL;

	if ((pstSdtTable == NULL) || (pstSvc == NULL))
	{
		return ERR_FAIL;
	}

	hErr = svc_si_GetPrivateSpecifyValue(ulActionSpec,&ulPrivateSpecifyValue);
	if(hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	eMenuLangId = eLangID_MAX;
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HINT32 *)&eMenuLangId,0);
	if ((hErr != ERR_OK) || (eMenuLangId == eLangID_MAX))
	{
		return ERR_FAIL;
	}
	eUndLangId = eLangID_Undefined;

	// SDT section Loop :
	for (pstCurrSdt = pstSdtTable; pstCurrSdt != NULL; pstCurrSdt = pstCurrSdt->pstNextTable)
	{

		SIxTable_SdtSec_t		*pstSdtSec = &pstCurrSdt->unInfo.stSdt;

		if(pstSdtSec == NULL)
		{
			HxLOG_Print("[%s:%d] pstSdtSec is NULL \n", __FUNCTION__, __LINE__);
			continue;
		}
		if (pstCurrSdt->unInfo.stSdt.usTsId == DbSvc_GetTsId(pstSvc) && pstCurrSdt->unInfo.stSdt.usOrgNetId == DbSvc_GetOnId(pstSvc))
		{
			/* Get Service Information from db */
			usRealTsId = pstSdtSec->usTsId;
			usRealOnId = pstSdtSec->usOrgNetId;

			for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
			{
				SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

				if (pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(pstSvc))
				{
					bFoundPrivate = FALSE;
					pstUkdttSelGuidanceDesc = NULL;

					for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
					{
						HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);

						if ((*pucRawDes == eSIxDESCTAG_UKDTT_GUIDANCE) && (bFoundPrivate == TRUE))
						{
							SIxUkdttGuidanceDes_t *pstUkdttGuidanceDesc = (SIxUkdttGuidanceDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);



							if (NULL != pstUkdttGuidanceDesc)
							{
								if((pstUkdttGuidanceDesc->ucGuidanceType == 0) || (pstUkdttGuidanceDesc->ucGuidanceType == 1))
								{
									if (pstUkdttSelGuidanceDesc == NULL)
									{
										pstUkdttSelGuidanceDesc = pstUkdttGuidanceDesc;

									}

									eCurLangId = eLangID_MAX;
									eCurLangId = HLIB_LANG_639ToId(pstUkdttGuidanceDesc->unInfo.pUkDttGuidanceInfo->szIso639LangCode);



									if (eCurLangId == eMenuLangId)
									{
										/* Desc Free�� �Ʒ����� ��� �Ŀ� */
										pstUkdttSelGuidanceDesc = pstUkdttGuidanceDesc;
										break;
									}
									else if ((eUndLangId != eLangID_MAX) && (eCurLangId == eUndLangId))
									{
										pstUkdttSelGuidanceDesc = pstUkdttGuidanceDesc;
										break;
									}

									if(pstUkdttSelGuidanceDesc != pstUkdttGuidanceDesc)
									{
										PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstUkdttGuidanceDesc);
										pstUkdttGuidanceDesc = NULL;
									}
								}
								else
								{
									if(NULL != pstUkdttGuidanceDesc)
									{
										PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstUkdttGuidanceDesc);
										pstUkdttGuidanceDesc = NULL;
									}
								}

							}

						}
						else if (*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_SPECIFIER)
						{
							SIxPrivateDataSpecDes_t *pPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

							if(pPrivDataSpecDes != NULL)
							{
								if (pPrivDataSpecDes->ulPrivateDataSpec == ulPrivateSpecifyValue)
								{
									bFoundPrivate = TRUE;
								}

								PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pPrivDataSpecDes);
							}
						}
						else if (*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_INDICATOR)
						{
							bFoundPrivate = FALSE;
						}
					}

					if(pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(pstSvc))
					{
						if(pstUkdttSelGuidanceDesc)
						{



								DB_SVC_SetServiceGuidanceType(pstSvc, pstUkdttSelGuidanceDesc->ucGuidanceType);

							if(pstUkdttSelGuidanceDesc->ucGuidanceType == 0x01)
								DB_SVC_SetServiceGuidanceMode(pstSvc, pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->ucGuidanceMode);
							else
								DB_SVC_SetServiceGuidanceMode(pstSvc, 0);

							DB_SVC_SetServiceGuidanceStr(pstSvc, pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->pszGuidance);
						}
						else
						{
							DB_SVC_SetServiceGuidanceType(pstSvc, 0xff);
						}
						break;
					}


					if (pstUkdttSelGuidanceDesc)
					{
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstUkdttSelGuidanceDesc);
						pstUkdttSelGuidanceDesc = NULL;
					}
				}
			}
		}
	}

	return ERR_OK;
}


STATIC HERROR xsvc_si_DuCollectServiceDataFromSi_UkDtt (svcSi_SrchData_t *pstSrch)
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

	HxLOG_Debug("[%s] pstSrch->hAction: 0x%x, ulActionId: 0x%x \n", __FUNCTION__, pstSrch->hAction, ulActionId);

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

		xsvc_si_DuSetDefaultAuthoriyInNetwork_UkDtt(ulActionId, pstNitTable, &stNetInfo);

	}

	(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Network, (void *)&stNetInfo, &ulNetIdx);

	// TS Info
	HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
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
		HxList_t *pstItem = NULL;

		for (pstItem = pstNitTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem);

			if ( (pst2ndLoop->unInfo.stNit.usTsId == stTsInfo.usTsId)
				&& (pst2ndLoop->unInfo.stNit.usOrgNetId == stTsInfo.usOnId) )
			{
				pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndLoop, eSIxDESCTAG_FREQUENCY_LIST, 0);
				if (pucRawDes != NULL)
				{
					stTsInfo.utExt.stTvAnyTime.bFreqListed = TRUE;
					break;
				}
			}
		}

		xsvc_si_DuSetDefaultAuthoriyInTS_UkDtt(ulActionId, pstNitTable, pstSdtTable, &stTsInfo);

	}

	(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Transponder, (void *)&stTsInfo, &ulTsIdx);

	// Delivery TS Info :
	if (eSI_SRCH_NITOPT_NitAct == pstSrch->eNitOption)
	{
		hErr = svc_si_LibGetAllDeliveryTsInfoFromNit (ulActionId, TRUE, TRUE, &ulTsNum, &pstTsArray);
		if ((ERR_OK == hErr) && (0 != ulTsNum) && (NULL != pstTsArray))
		{
#if 1
			for (ulCnt = 0; ulCnt < ulTsNum; ulCnt++)
			{
				HxLOG_Debug("\t LoopCnt = %d, (OnId = 0x%x, tsId = 0x%x, freq = %d, BW = %d) \n", ulCnt,  pstTsArray[ulCnt].usOnId, pstTsArray[ulCnt].usTsId, pstTsArray[ulCnt].stTuningParam.ter.ulFreq, pstTsArray[ulCnt].stTuningParam.ter.ucBandwidth);
				(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Delivery, (void *)&(pstTsArray[ulCnt]), NULL);
			}
#else
			HBOOL bFound = FALSE;
			DxTuneParam_t stTuneInfo;
			DbSvc_TsInfo_t stLockTsInfo;

			SVC_CH_GetLockedInfo(ulActionId, &stTuneInfo);
			HxSTD_MemCopy(&stLockTsInfo, &stTsInfo, sizeof(DbSvc_TsInfo_t));

			stLockTsInfo.stTuningParam.ter.ulFreq = stTuneInfo.ter.ulFrequency;
			stLockTsInfo.stTuningParam.ter.ucTransMode = stTuneInfo.ter.eTransMode;
			stLockTsInfo.stTuningParam.ter.ucGuardInterval = stTuneInfo.ter.eGuardInterval;
			stLockTsInfo.stTuningParam.ter.ucBandwidth = stTuneInfo.ter.eBandWidth;
#if defined(CONFIG_MW_CH_DVB_T2)
			stLockTsInfo.stTuningParam.ter.ucSystem = stTuneInfo.ter.eSystem;
			stLockTsInfo.stTuningParam.ter.stT2param.ulPlpId = stTuneInfo.ter.stT2Param.ulPlpId;
#endif

			for (ulCnt = 0; ulCnt < ulTsNum; ulCnt++)
			{
				HxLOG_Debug("\t LoopCnt = %d, (OnId = 0x%x, tsId = 0x%x, freq = %d, BW = %d) \n", ulCnt,  pstTsArray[ulCnt].usOnId, pstTsArray[ulCnt].usTsId, pstTsArray[ulCnt].stTuningParam.ter.ulFreq, pstTsArray[ulCnt].stTuningParam.ter.ucBandwidth);
				(void)svc_si_DuAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Delivery, (void *)&(pstTsArray[ulCnt]), NULL);
				if ((stTsInfo.usOnId == pstTsArray[ulCnt].usOnId) && (stTsInfo.usTsId == pstTsArray[ulCnt].usTsId))
				{
					ulTsIdx = ulCnt;
					if (bFound == FALSE)
					{
						if (DB_SVC_UtilCompareDbTuningParam(eDxDELIVERY_TYPE_TER, &(stLockTsInfo.stTuningParam), &(pstTsArray[ulCnt].stTuningParam)) == 0)
						{
							bFound = TRUE;
							HxSTD_MemCopy(&stTsInfo.stTuningParam, &pstTsArray[ulCnt].stTuningParam, sizeof(DbSvc_TuningInfo_t));
							HxLOG_Debug("\t Found the Locked TS (OnId = 0x%x, tsId = 0x%x, freq = %d, BW = %d) \n", pstTsArray[ulCnt].usOnId, pstTsArray[ulCnt].usTsId, pstTsArray[ulCnt].stTuningParam.ter.ulFreq, pstTsArray[ulCnt].stTuningParam.ter.ucBandwidth);
						}
					}
				}
			}

			if (bFound == FALSE)
			{
				HxSTD_MemCopy(&stTsInfo.stTuningParam, &pstTsArray[ulTsIdx].stTuningParam, sizeof(DbSvc_TuningInfo_t));
			}
#endif
		}
	}


	HxLOG_Debug("OnID = 0x%x, TsID = 0x%x,  pstSrch->eNitOption = %d \n", stTsInfo.usOnId, stTsInfo.usTsId, pstSrch->eNitOption);

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
					// search option�� ������ svc type�� �ٸ���
					continue;
				}

				if (xsvc_si_LibSrchCompareCasType(pstSrch->stOption.ulCasType, DbSvc_GetCasType(pstSvc)) != ERR_OK)
				{
					// search option�� ������ cas type�� �ٸ���
					continue;
				}

				// UK Terrestrial ������ MHEG ������ Data Channel�� TV Channel List�� ����� �Ѵ�.
				// Data Type -> TV Type�� ��ȯ
				if(DbSvc_GetSvcType(pstSvc) == eDxSVC_TYPE_DATA)
				{
					DbSvc_SetSvcType(pstSvc, eDxSVC_TYPE_TV);
				}

//Add to TRD Info
#if defined(CONFIG_DB_SVC_SAVE_ORG_LCN)
				DbSvc_SetOrgLcn(pstSvc, DbSvc_GetLcn(pstSvc));
#endif
				xsvc_si_DuSetDefaultAuthoriyInSvc_UkDtt(ulActionId, pstSdtTable, pstSvc);
				xsvc_si_DuSetGuidanceInfoInSvc_UkDtt(ulActionId,pstSdtTable, pstSvc,pstSrch->unSiSpec.stLiveSpec.ulActionSpec);

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

STATIC HERROR xsvc_si_DuCheckAndComposeSearchData_UkDtt (svcSi_SrchData_t *pstSrch)
{
	HINT32			 nMsgType;
	HERROR			 hErr;

	if (svc_si_DuCheckAllSiTableReceived (pstSrch) == TRUE)
	{
		svc_si_DuFreeSearchResult(pstSrch);

		hErr = xsvc_si_DuCollectServiceDataFromSi_UkDtt (pstSrch);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("xsvc_si_LibSrchCollectServiceDataFromSi_Base err:\n");
		}

		nMsgType = (ERR_OK == hErr) ? eSEVT_SI_DYNAMIC_UPDATE_DONE : eSEVT_SI_DYNAMIC_UPDATE_FAIL;

		HxLOG_Debug("Send Message =====> %s\n" , nMsgType == eSEVT_SI_DYNAMIC_UPDATE_DONE ? "eSEVT_SI_DYNAMIC_UPDATE_DONE": "eSEVT_SI_DYNAMIC_UPDATE_FAIL" );
		svc_si_PostMsgToAp (nMsgType, pstSrch->hAction, 0, 0, 0);

		/* filter stop�� search stop�ÿ� �Ѵ� */
	}

	return ERR_OK;
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_DuMsgCmdStartDynamicUpdate_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	SvcSi_SearchOption_t 	stSrchOption;

	HxSTD_MemSet(&stSrchOption, 0x00, sizeof(SvcSi_SearchOption_t));

	stSrchOption.ulNetId = SvcSi_NETID_SEARCH_NITACT;

	(void)svc_si_DuInitSearchData (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stStartDynamicUpdate.unSpec), &stSrchOption);
	(void)svc_si_DuInitSearchResultTable (pstSrch);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxSPEC_UKDTT);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxTEXT_ENC_300468_UK);

	pstSrch->eFiltering = (bSI_FILTERING_PAT			/* compose�� �� ts id ���� �뵵 */
							| bSI_FILTERING_SDT_ACT
							| bSI_FILTERING_NIT_ACT);
	(void)svc_si_DuStartSection (pstSrch);
}

STATIC void xsvc_si_DuMsgCmdStopDynamicUpdate_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_DuStopSection (pstSrch);
	(void)svc_si_DuFreeSearchData (pstSrch);
}

STATIC void xsvc_si_DuMsgEvtPatTable_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_DuCheckAndComposeSearchData_UkDtt (pstSrch);
}

STATIC void xsvc_si_DuMsgEvtSdtTable_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_DuCheckAndComposeSearchData_UkDtt (pstSrch);
}

STATIC void xsvc_si_DuMsgEvtNitTable_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_DuCheckAndComposeSearchData_UkDtt (pstSrch);
}

STATIC void xsvc_si_DuProcessMsg_UkDtt (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL == pstSrch)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_DYNAMICUPDATE:
		HxLOG_Debug("[%s]eSIMSG_CMD_START_DYNAMICUPDATE \n", __FUNCTION__);
		xsvc_si_DuMsgCmdStartDynamicUpdate_UkDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_CMD_STOP_DYNAMICUPDATE:
		HxLOG_Debug("[%s]eSIMSG_CMD_STOP_DYNAMICUPDATE \n", __FUNCTION__);
		xsvc_si_DuMsgCmdStopDynamicUpdate_UkDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Debug("[%s]eSIMSG_EVT_PAT_TABLE \n", __FUNCTION__);
		xsvc_si_DuMsgEvtPatTable_UkDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		HxLOG_Debug("[%s]eSIMSG_EVT_SDT_TABLE \n", __FUNCTION__);
		xsvc_si_DuMsgEvtSdtTable_UkDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		HxLOG_Debug("[%s]eSIMSG_EVT_NIT_TABLE \n", __FUNCTION__);
		xsvc_si_DuMsgEvtNitTable_UkDtt (pstSrch, pstMsg);
		break;

	default:
		HxLOG_Debug ("[%s] Non-processing MSG (0x%08x) -> skipped:\n", __FUNCTION__, pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_DuGetDynamicUpdateActionFunc_UkDtt (void)
{
	return xsvc_si_DuProcessMsg_UkDtt;
}

void *xsvc_si_DuAllocDynamicUpdateActionData_UkDtt (void)
{
	svcSi_SrchData_t	*pstSrch;

	pstSrch = (svcSi_SrchData_t *)OxSI_Calloc (sizeof(svcSi_SrchData_t));
	if (NULL != pstSrch)
	{
		pstSrch->eTextEncType	= eSITEXT_ENCODED_300468_UK;
	}

	return pstSrch;
}

void xsvc_si_DuFreeDynamicUpdateActionData_UkDtt (void *pvData)
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
