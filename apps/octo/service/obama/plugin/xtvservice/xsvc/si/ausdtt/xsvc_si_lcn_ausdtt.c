/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lcn_AusDtt.c
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
#include <isosvc.h>
#include <pal_pipe.h>
#include <pal_sirawpool.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_si.h>
#include <_svc_si.h>
#include <_xsvc_si_lib_lcn_base.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define CHECK_OVERLAY_AREA	(1)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef  struct
{
	HUINT16		usTsId;
	HUINT16		usOnId;
	HUINT16		usServiceId;
	HUINT8		ucVisibleServiceFlag;
	HUINT16		usLogicalChannelNumber;
} xsvcSi_LcnInfo_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___

STATIC HERROR xsvc_si_LcnGetLogicalChannelFromNit_AusDtt (HUINT32 ulActionId, HxList_t **ppChList)
{
	HxList_t					*pstItem1, *pstItem2;
	SIxTable_PsiTable_t		*pstNitTable = NULL;
	HERROR					 hResult = ERR_FAIL;
	HxList_t 					*pChList = NULL;

	if (NULL == ppChList)
	{
		HxLOG_Error ("Invalid paremeters \n");
		goto END_FUNC;
	}

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	if (pstNitTable == NULL)
	{
		HxLOG_Error ("Invalid pstNitTable \n");
		goto END_FUNC;
	}

	// Memory Allocation 을 위해 대략적인 개수를 잰다.
	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
		{
			HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			switch (*pucDesRaw)
			{
				case eSIxDESCTAG_LOGICAL_CHANNEL:
				{
					SIxAusdttLogicalChannelDes_t	*pstLogicChDes = (SIxAusdttLogicalChannelDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
					if (NULL != pstLogicChDes)
					{
						SIxAusdttLogicalChannelInfo_t *pChListItem = pstLogicChDes->pstInfo;

						while(pChListItem)
						{
							xsvcSi_LcnInfo_t *pstLcnInfo = NULL;

							pstLcnInfo = (xsvcSi_LcnInfo_t*)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_t));
							if (pstLcnInfo != NULL)
							{
								pstLcnInfo->usTsId = pst2ndLoop->unInfo.stNit.usTsId;
								pstLcnInfo->usOnId = pst2ndLoop->unInfo.stNit.usOrgNetId;

								pstLcnInfo->usServiceId = pChListItem->usServiceId;
								pstLcnInfo->ucVisibleServiceFlag = pChListItem->ucVisibleServiceFlag;
								pstLcnInfo->usLogicalChannelNumber = pChListItem->usLogicalChannelNumber;

								HxLOG_Debug ("\t LCN Info (0x%x:0x%x:0x%x) VisibleServiceFlag = %d, usLogicalChannelNumber = %d\n", pstLcnInfo->usOnId, pstLcnInfo->usTsId, pstLcnInfo->usServiceId, pstLcnInfo->ucVisibleServiceFlag, pstLcnInfo->usLogicalChannelNumber);

								pChList = HLIB_LIST_Append(pChList, (void*)pstLcnInfo);
							}
							pChListItem = pChListItem->pstNext;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLogicChDes);
					}
				}
				break;

			default:
				break;
			}
		}
	}

	*ppChList = pChList;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstNitTable)	{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }

	return hResult;
}

STATIC HERROR xsvc_si_LcnIsSignalStrongerThanDbTsInfo_AusDtt (DbSvc_TsInfo_t *pstNewTsInfo, DbSvc_TsInfo_t *pstDbTsInfo, HBOOL *pbStronger)
{
	#define TER_SIGNAL_QUALITY_THRESHOLD		5
	#define TER_SIGNAL_LEVEL_THRESHOLD		5

	DbSvc_TerTuningInfo_t	*pstNewTuneInfo, *pstDbTuneInfo;

	pstNewTuneInfo	= &(pstNewTsInfo->stTuningParam.ter);
	pstDbTuneInfo	= &(pstDbTsInfo->stTuningParam.ter);

	// Quality 가 높으면 TRUE

	HxLOG_Debug ("\t Qulity Check (%d, %d)\n", pstNewTuneInfo->ucQuality, (pstDbTuneInfo->ucQuality + TER_SIGNAL_QUALITY_THRESHOLD));

	if ((HUINT32)pstNewTuneInfo->ucQuality > ((HUINT32)pstDbTuneInfo->ucQuality + TER_SIGNAL_QUALITY_THRESHOLD))
	{
		*pbStronger = TRUE;
		return ERR_OK;
	}

	// Quality 가 낮으면 FALSE
	if (((HUINT32)pstNewTuneInfo->ucQuality + TER_SIGNAL_QUALITY_THRESHOLD) < (HUINT32)pstDbTuneInfo->ucQuality)
	{
		*pbStronger = FALSE;
		return ERR_OK;
	}

	// Quality가 거의 동일
	// Level 이 높으면 TRUE
	HxLOG_Debug ("\t Level Check (%d, %d)\n",pstNewTuneInfo->ucLevel, (pstDbTuneInfo->ucLevel + TER_SIGNAL_LEVEL_THRESHOLD));
	if ((HUINT32)pstNewTuneInfo->ucLevel > ((HUINT32)pstDbTuneInfo->ucLevel + TER_SIGNAL_LEVEL_THRESHOLD))
	{
		*pbStronger = TRUE;
		return ERR_OK;
	}

	// 그 외에는 FALSE
	*pbStronger = FALSE;

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnReassignLcn_AusDtt(DxDeliveryType_e	eOrgDeliType, DxSvcType_e eSvcType, HUINT16 *pusLcn)
{
	HUINT16 	usStartNum;

	if (pusLcn == NULL)
		return ERR_FAIL;

	usStartNum = 0;
	DB_SVC_GetLcnOrderNumber (eOrgDeliType, eSvcType, &usStartNum);

	usStartNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_AUSDTT, eOrgDeliType, eSvcType);

	*pusLcn = usStartNum;
	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnCheckOverlayLcn_AusDtt (DbSvc_Info_t	*pstNewSvcInfo, SRCHSVC_AppendFlag_t *peSvcFlag)
{
	HERROR err;
	HUINT32 i, ulSvcCnt;
	Handle_t *phSvcList = NULL;
	DbSvc_Attr_t stAttr;

	if (pstNewSvcInfo == NULL || peSvcFlag == NULL)
		return ERR_FAIL;

	DB_SVC_InitSvcAttribute (&stAttr);

	stAttr.eOrgDeliType = DbSvc_GetDeliTypePtr(pstNewSvcInfo);
	stAttr.eSvcType = DbSvc_GetSvcType(pstNewSvcInfo);

	err = DB_SVC_FindServiceList(eSvcGroup_All, &stAttr, 0, HANDLE_NULL, eSvcFindDirection_FowardFromStart, &ulSvcCnt, &phSvcList);

	HxLOG_Debug ("\n DB_SVC_FindServiceList (cnt = %d)\n", ulSvcCnt);
	if (err == ERR_OK)
	{
		HUINT16 usLcn;
		DbSvc_Info_t	stSvcInfo;

		usLcn = DbSvc_GetLcn(pstNewSvcInfo);
		for (i=0; i<ulSvcCnt; i++)
		{
			err = DB_SVC_GetServiceInfo(phSvcList[i], &stSvcInfo);
			if (err != ERR_OK)
				continue;

			if (usLcn == DbSvc_GetLcn(&stSvcInfo))
			{
				HBOOL bStronger;
				DbSvc_TsInfo_t stNewTsInfo, stDbTsInfo;

				HxLOG_Debug ("\n found Same LCN (%d)\n", usLcn);

				err = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(pstNewSvcInfo), &stNewTsInfo);
				err |= DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stSvcInfo), &stDbTsInfo);
				if (err != ERR_OK)
					continue;

				if (stNewTsInfo.usTsIdx == stDbTsInfo.usTsIdx)
				{
					HxLOG_Print ("\n found same ts info (0x%x, 0x%x)... so skipped.....\n", stNewTsInfo.usTsIdx, stDbTsInfo.usTsIdx);
					continue;
				}

				HxLOG_Debug ("\n found diff ts info (0x%x, 0x%x)\n", stNewTsInfo.usTsIdx, stDbTsInfo.usTsIdx);
				err = xsvc_si_LcnIsSignalStrongerThanDbTsInfo_AusDtt(&stNewTsInfo, &stDbTsInfo, &bStronger);
				if (err != ERR_OK)
				{
					HxLOG_Error ("\n error in xsvc_si_LcnIsSignalStrongerThanDbTsInfo_AusDtt...\n");
					continue;
				}

				if ( (DbSvc_GetOnId(pstNewSvcInfo) == DbSvc_GetOnId(&stSvcInfo))
					&& (DbSvc_GetTsId(pstNewSvcInfo) == DbSvc_GetTsId(&stSvcInfo)) )
				{
					HxLOG_Debug  ("\n Is same TP (new=0x%x:0x%x : old=0x%x:0x%x), bStronger = %d\n", DbSvc_GetOnId(pstNewSvcInfo), DbSvc_GetTsId(pstNewSvcInfo), DbSvc_GetOnId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo), bStronger);
					if ( (stNewTsInfo.utExt.stAusDtt.bFreqListed == TRUE)
						&& (stDbTsInfo.utExt.stAusDtt.bFreqListed == TRUE) )
					{
						if (bStronger)
						{
							/* 기존 service 삭제처리 */
							DB_SVC_DeleteServiceInfo(phSvcList[i]);
						}
						else
						{
							*peSvcFlag = eSRCHSVC_DONTADD;
						}
					}
					else
					{
						if (bStronger)
						{
							HUINT16 usNewLcn;
							if (xsvc_si_LcnReassignLcn_AusDtt(DbSvc_GetDeliType(stSvcInfo), DbSvc_GetSvcType(&stSvcInfo), &usNewLcn) == ERR_OK)
							{
								DbSvc_SetLcnFlag(&stSvcInfo, FALSE);
								DbSvc_SetLcn(&stSvcInfo, usNewLcn);

								DB_SVC_UpdateServiceInfo(phSvcList[i], &stSvcInfo);
							}
						}
						else
						{
							DbSvc_SetLcnFlag(pstNewSvcInfo, FALSE);
						}
					}
				}
				else
				{
					HxLOG_Debug  ("\n Is diff TP (new=0x%x:0x%x : old=0x%x:0x%x), bStronger = %d\n", DbSvc_GetOnId(pstNewSvcInfo), DbSvc_GetTsId(pstNewSvcInfo), DbSvc_GetOnId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo), bStronger);
					if (bStronger)
					{
						HUINT16 usNewLcn;
						if (xsvc_si_LcnReassignLcn_AusDtt(DbSvc_GetDeliType(stSvcInfo), DbSvc_GetSvcType(&stSvcInfo), &usNewLcn) == ERR_OK)
						{
							DbSvc_SetLcnFlag(&stSvcInfo, FALSE);
							DbSvc_SetLcn(&stSvcInfo, usNewLcn);

							DB_SVC_UpdateServiceInfo(phSvcList[i], &stSvcInfo);
						}
					}
					else
					{
						DbSvc_SetLcnFlag(pstNewSvcInfo, FALSE);
					}
				}
			}
		}
	}

	if (phSvcList != NULL)
	{
		DB_SVC_FreeServiceList(phSvcList);
	}

	return ERR_OK;
}


// ============================================================================================
// Function : xsvc_si_LcnSetServiceNumber_AusDtt
HERROR xsvc_si_LcnSetServiceNumber_AusDtt (Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	HUINT16			 usStartTvNum, usStartRadioNum;
	HUINT32			 ulIndex;
	DbSvc_Info_t		*pstSvcInfo;
	HxList_t			*pChList = NULL;
	HxList_t			*pstItem = NULL;

	usStartTvNum = usStartRadioNum = 0;
	DB_SVC_GetLcnOrderNumber (DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV, &usStartTvNum);
	DB_SVC_GetLcnOrderNumber (DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO, &usStartRadioNum);

	/* base lcn 만큼 보정 */
	usStartTvNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_AUSDTT, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV);
	usStartRadioNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_AUSDTT, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO);

	if (xsvc_si_LcnGetLogicalChannelFromNit_AusDtt(PAL_PIPE_GetActionId(hAction), &pChList) != ERR_OK)
	{
		HxLOG_Error ("\n\t xsvc_si_LcnSetServiceNumber_AusDtt ::: error in xsvc_si_LcnGetLogicalChannelFromNit_AusDtt\n");
	}

	for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
	{
		if (peFlagArray[ulIndex] != eSRCHSVC_ADD &&
			peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
		{
			continue;
		}

		pstSvcInfo = &(pstSvcArray[ulIndex]);

		/* set LCN */
		HLIB_LIST_First(pChList);
		for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next)
		{
			xsvcSi_LcnInfo_t *pstLcnInfo = (xsvcSi_LcnInfo_t *)HLIB_LIST_Data(pstItem);
			if (pstLcnInfo->usTsId == DbSvc_GetTsId(pstSvcInfo)
				&& pstLcnInfo->usOnId == DbSvc_GetOnId(pstSvcInfo)
				&& pstLcnInfo->usServiceId == DbSvc_GetSvcId(pstSvcInfo))
			{
				DbSvc_SetLcnFlag(pstSvcInfo, TRUE);
				DbSvc_SetLcn(pstSvcInfo, pstLcnInfo->usLogicalChannelNumber);
				DbSvc_SetVisibleFlag(pstSvcInfo, pstLcnInfo->ucVisibleServiceFlag);
			}
		}

		HxLOG_Print ("\t\t ===> searched svc info : (0x%x:0x%x:0x%x) name = %s, LCN(%d:0x%x), Type=0x%x, casType=0x%x\n",
								DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo), DbSvc_GetName(pstSvcInfo), DbSvc_GetLcnFlag(pstSvcInfo), DbSvc_GetLcn(pstSvcInfo), DbSvc_GetSvcType(pstSvcInfo), DbSvc_GetCasType(pstSvcInfo));
#if (CHECK_OVERLAY_AREA == 1)
		if (DbSvc_GetLcnFlag(pstSvcInfo) == TRUE)
		{
			if (xsvc_si_LcnCheckOverlayLcn_AusDtt(pstSvcInfo, &(peFlagArray[ulIndex])) != ERR_OK)
			{
				continue;
			}

			if (peFlagArray[ulIndex] != eSRCHSVC_ADD &&
				peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
			{
				continue;
			}
		}
#endif

		if (DbSvc_GetLcnFlag(pstSvcInfo) != TRUE)
		{
			switch (DbSvc_GetSvcType(pstSvcInfo))
			{
			case eDxSVC_TYPE_TV:
				DbSvc_SetLcn(pstSvcInfo, usStartTvNum);
				usStartTvNum++;
				break;

			case eDxSVC_TYPE_RADIO:
				DbSvc_SetLcn(pstSvcInfo, usStartRadioNum);
				usStartRadioNum++;
				break;

			default:
				HxLOG_Critical("\n\n");
				HxLOG_Error("===> Strange Type %d\n", DbSvc_GetSvcType(pstSvcInfo));
				break;
			}
		}
	}

	if (pChList)
	{
		HLIB_LIST_First(pChList);
		for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next)
		{
			void *pvData = (void *)HLIB_LIST_Data(pstItem);
			if (pvData) OxSI_Free(pvData);

		}
		HLIB_LIST_RemoveAll(pChList);
	}

	return ERR_OK;
}

/* end of file */

