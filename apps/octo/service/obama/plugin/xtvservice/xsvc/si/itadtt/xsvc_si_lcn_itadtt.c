/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lcn_itadtt.c
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

/*
	=== Digitvi LCN Scenario ===

	Step 1> Assign the LCN to each service according to Logical_channel_descriptor.
			In case of the LCN conflict, apply conditions as below
				1. In the case of the presence of the same service(triplet id) on two different frequencies, Qos take priority.
				2. In presence of a conflict between different services that request the same logical channel number, Network Id(0x3001 - 0x3100) take priority.
				3. If some competing services is in the Network Id, the resolution is left up to the customer. Qos shall never be perforned. The rest of competing services moved to Main Overflow.
				4. If all competing services is not in the Network Id, the resolution is left up to the customer. Qos shall never be perforned. The rest of competing services moved to Main Overflow.

		Step 2> After Step 1, Assign the LCN to each service accoding to HD simulcast descriptor.
					In case of the LCN conflict, apply condition as below
						1. Find unprecessed service
							a. same NID and one of the follow condition
								b. Service type is HD ? (OR)
								c. HD LCN < LCN ? (OR)
								d. HD LCN not appearin LCN ? (OR)
						2. Signal strength greater than BER threshold value?
						3. Assign HD simulcast LCN
							a. HD simulcast LCN allocated to existing service?
								b. If No, assign HD simulcast LCN to new service
								c. If Yes, find displaced service LCN assignment in the HD simulcast LCN with a valid NID, Alternative LCN allocation found for displaced service?
									d. if No, mark displaced service with unassigned LCN
									e. if Yes, find existing service to be displaced in advance
										f. if No, assign displaced(3.f) service with HD simulcast LCN.
										g. if Yes. assign displaced(3.f) service with HD simulcast LCN.
											h. if found in DB, assign displaced service with LCN from HD simulcast LCN
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
#define ITA_LCN_PREFERENCE_START							1
#define ITA_LCN_PREFERENCE_END							99
#define ITA_LCN_ASSIGNMENT_START							100
#define ITA_LCN_ASSIGNMENT_WITHOUT_MAIN_OVERFLOW_END	849
#define ITA_LCN_MAIN_OVERFLOW_START						850
#define ITA_LCN_MAIN_OVERFLOW_END						999

#define ITA_LCN_IS_PREFERENCES_ZONE(usLcn) 			((usLcn >= ITA_LCN_PREFERENCE_START && usLcn <= ITA_LCN_PREFERENCE_END) ? TRUE : FALSE)
#define ITA_LCN_IS_ASSIGNMENT_WO_MO_ZONE(usLcn) 	((usLcn >= ITA_LCN_ASSIGNMENT_START && usLcn <= ITA_LCN_ASSIGNMENT_WITHOUT_MAIN_OVERFLOW_END) ? TRUE : FALSE)
#define ITA_LCN_IS_MAIN_OVERFLOW_ZONE(usLcn) 		((usLcn >= ITA_LCN_MAIN_OVERFLOW_START && usLcn <= ITA_LCN_MAIN_OVERFLOW_END) ? TRUE : FALSE)

#define ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM				9000 // add this magic number to lcn for webapp to distinguish conflicting lcn service
#define ITA_LCN_ADD_CONFLICT_MAGIC_NUM(pSvcInfo)		{ 															\
															HUINT16 usLcn = DbSvc_GetLcn(pSvcInfo); 				\
															if (usLcn < ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM) 		\
															{ 														\
																usLcn = usLcn + ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM; \
																DbSvc_SetLcn(pSvcInfo, usLcn); 						\
															}														\
														}

#define ITA_LCN_SUB_CONFLICT_MAGIC_NUM(pSvcInfo)		{ 															\
															HUINT16 usLcn = DbSvc_GetLcn(pSvcInfo); 				\
															if (usLcn > ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM) 		\
															{ 														\
																usLcn = usLcn - ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM; \
																DbSvc_SetLcn(pSvcInfo, usLcn); 						\
															}														\
														}

#define ITA_TER_NETWORK_ID_MIN		0x3001
#define ITA_TER_NETWORK_ID_MAX		0x3100

#define LOCAL_MACRO_IS_ITA_TER_NETWORK_ID(usNetId) 			(((usNetId >= ITA_TER_NETWORK_ID_MIN) && (usNetId <= ITA_TER_NETWORK_ID_MAX)) ? TRUE : FALSE)

#define ITA_LCN_MAIN_OVERFLOW_NUM						(ITA_LCN_MAIN_OVERFLOW_END - ITA_LCN_MAIN_OVERFLOW_START + 1)
#define TER_SIGNAL_QUALITY_THRESHOLD	5
#define TER_SIGNAL_LEVEL_THRESHOLD		5

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef  struct
{
	HUINT16		usNetId;
	HUINT16		usTsId;
	HUINT16		usOnId;
	HUINT16		usServiceId;
	HUINT8		ucVisibleServiceFlag;
	HUINT16		usLogicalChannelNumber;
	HBOOL		bUsed;
} xsvcSi_LcnInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HUINT16	s_usNonLcnStartNum[eDxSVC_TYPE_RADIO];

#define ___LOCAL_FUNCTIONS___

STATIC HERROR xsvc_si_LcnGetLogicalChannelFromNit_ItaDtt (HUINT32 ulActionId, HxList_t **ppLcnList)
{
	HxList_t					*pstItem1, *pstItem2;
	SIxTable_PsiTable_t			*pstNitTable = NULL;
	HxList_t 					*pLcnList = NULL;

	if (NULL == ppLcnList)
	{
		HxLOG_Error ("Invalid paremeters \n");
		return ERR_FAIL;
	}

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	if (pstNitTable == NULL)
	{
		HxLOG_Error ("Invalid pstNitTable \n");
		return ERR_FAIL;
	}

	/************************************************************
		Logical Channel Descriptor
	*************************************************************/
	HxLOG_Print("===== Process Logical Channel Descriptor =========\n");

	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		if (pst2ndLoop == NULL)
		{
			HxLOG_Critical("===== Error in 2nd Loop (LCN) =========\n");
			continue;
		}

		for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
		{
			HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			switch (*pucDesRaw)
			{
				case eSIxDESCTAG_ITADTT_LOGICAL_CHANNEL:
					{
						SIxItadttLogicalChannelDes_t	*pstLogicChDes = (SIxItadttLogicalChannelDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
						if (NULL != pstLogicChDes)
						{
							SIxItadttLogicalChannelInfo_t *pChListItem = pstLogicChDes->pstInfo;

							while(pChListItem)
							{
								xsvcSi_LcnInfo_t *pstLcnInfo = NULL;

								pstLcnInfo = (xsvcSi_LcnInfo_t*)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_t));
								if (pstLcnInfo != NULL)
								{
									pstLcnInfo->usNetId = pstNitTable->unInfo.stNit.usNetId;
									pstLcnInfo->usTsId = pst2ndLoop->unInfo.stNit.usTsId;
									pstLcnInfo->usOnId = pst2ndLoop->unInfo.stNit.usOrgNetId;

									pstLcnInfo->usServiceId = pChListItem->usServiceId;
									pstLcnInfo->ucVisibleServiceFlag = pChListItem->ucVisibleServiceFlag;
									pstLcnInfo->usLogicalChannelNumber = pChListItem->usLogicalChannelNumber;
									pstLcnInfo->bUsed = FALSE;

									HxLOG_Debug ("\t LCN Info (0x%x:0x%x:0x%x:0x%x) VisibleServiceFlag = %d, usLogicalChannelNumber = %d\n", pstLcnInfo->usNetId, pstLcnInfo->usOnId, pstLcnInfo->usTsId, pstLcnInfo->usServiceId, pstLcnInfo->ucVisibleServiceFlag, pstLcnInfo->usLogicalChannelNumber);

									pLcnList = HLIB_LIST_Append(pLcnList, (void*)pstLcnInfo);
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

	*ppLcnList = pLcnList;

	if (NULL != pstNitTable)	{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }

	return (pLcnList == NULL) ? ERR_FAIL : ERR_OK;
}

STATIC HERROR xsvc_si_LcnGetHdSimulcastLCFromNit_ItaDtt (HUINT32 ulActionId, HxList_t **ppLcnList)
{
	HxList_t					*pstItem1, *pstItem2;
	SIxTable_PsiTable_t			*pstNitTable = NULL;
	HxList_t 					*pLcnList = NULL;

	if (NULL == ppLcnList)
	{
		HxLOG_Error ("Invalid paremeters \n");
		return ERR_FAIL;
	}

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	if (pstNitTable == NULL)
	{
		HxLOG_Error ("Invalid pstNitTable \n");
		return ERR_FAIL;
	}

	/************************************************************
		HD SimulCast Logical Channel Descriptor
	*************************************************************/
	HxLOG_Print("===== Process HD SimulCast Logical Channel Descriptor =========\n");

	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		if (pst2ndLoop == NULL)
		{
			HxLOG_Critical("===== Error in 2nd Loop (HD LCN) =========\n");
			continue;
		}

		for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
		{
			HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			switch (*pucDesRaw)
			{
				case eSIxDESCTAG_ITADTT_HD_SIMULCAST_LOGICAL_CHANNEL:
					{
						SIxItadttHDSimulcastLcnDes_t *pstHdItaSimulcastLcnDes = (SIxItadttHDSimulcastLcnDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
						if (NULL != pstHdItaSimulcastLcnDes)
						{
							SIxItadttHDSimulLcnInfo_t	*pChListItem = pstHdItaSimulcastLcnDes->pstInfo;

							while(pChListItem)
							{
								xsvcSi_LcnInfo_t *pstLcnInfo = NULL;

								pstLcnInfo = (xsvcSi_LcnInfo_t*)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_t));
								if (pstLcnInfo != NULL)
								{
									pstLcnInfo->usNetId = pstNitTable->unInfo.stNit.usNetId;
									pstLcnInfo->usTsId = pst2ndLoop->unInfo.stNit.usTsId;
									pstLcnInfo->usOnId = pst2ndLoop->unInfo.stNit.usOrgNetId;

									pstLcnInfo->usServiceId = pChListItem->usServiceId;
									pstLcnInfo->ucVisibleServiceFlag = pChListItem->ucVisibleServiceFlag;
									pstLcnInfo->usLogicalChannelNumber = pChListItem->usLogicalChannelNumber;
									pstLcnInfo->bUsed = FALSE;

									HxLOG_Debug ("\t HD Simulcast LCN Info (%d:0x%x:0x%x:0x%x:0x%x) VisibleServiceFlag = %d, usLogicalChannelNumber = %d\n",
													pstLcnInfo->bUsed, pstLcnInfo->usNetId, pstLcnInfo->usOnId,
													pstLcnInfo->usTsId, pstLcnInfo->usServiceId, pstLcnInfo->ucVisibleServiceFlag,
													pstLcnInfo->usLogicalChannelNumber);

									pLcnList = HLIB_LIST_Append(pLcnList, (void*)pstLcnInfo);
								}
								pChListItem = pChListItem->pstNext;
							}

							PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstHdItaSimulcastLcnDes);
						}
					}
					break;

				default:
					break;
			}
		}
	}

	*ppLcnList = pLcnList;

	if (NULL != pstNitTable)	{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }

	return (pLcnList == NULL) ? ERR_FAIL : ERR_OK;
}

STATIC HERROR xsvc_si_LcnGetUnassginedNonLcn_ItaDtt(DefaultLcnPolicy_t eLcnPolicy , DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HUINT16 *pusLcn)
{
	HUINT16 	usUnassignedNonLcnNum = 0;
	Handle_t 	hSvc;
	HERROR		hErr;

	if (pusLcn == NULL)
		return ERR_FAIL;

	DB_SVC_GetLcnOrderNumber (eOrgDeliType, eSvcType, &usUnassignedNonLcnNum);

	HxLOG_Print ("get a number of the (%d) Main Overflow space (%d)\n", eSvcType, usUnassignedNonLcnNum);

	if (eLcnPolicy == eDEFAULTLCN_ITADTT)
	{
		if (usUnassignedNonLcnNum >= ITA_LCN_MAIN_OVERFLOW_NUM)
		{
			HUINT16 		usBackwardNum = ITA_LCN_MAIN_OVERFLOW_START - (usUnassignedNonLcnNum - ITA_LCN_MAIN_OVERFLOW_START + 1);
			HxLOG_Print ("The Main Overflow space is full (%d), so set number from 849 backwards (%d) \n", usUnassignedNonLcnNum, usBackwardNum);

			usUnassignedNonLcnNum = usBackwardNum;
		}
		else
		{
			usUnassignedNonLcnNum += xsvc_si_LibLcnGetSvcNumBase (eLcnPolicy, eOrgDeliType, eSvcType);
		}

		hErr = DB_SVC_GetServiceHandleByNumber(eOrgDeliType, eSvcType, usUnassignedNonLcnNum, &hSvc);
		if(hErr == ERR_OK || hSvc != HANDLE_NULL)
		{
			// if service with the same non-lcn is existed, Unassigned-lcn number is wrong. So rearrange the non-Lcn number on all non-lcn services.
			HINT32				i, ulNonLcnStartNum = ITA_LCN_MAIN_OVERFLOW_START;
			DbSvc_Condition_t	stSvcCond;
			HINT32			 	ulSvcCnt;
			Handle_t			*phSvcList = NULL;
			DbSvc_Info_t	stSvcInfo;

			DB_SVC_InitSvcFindCondition (&stSvcCond);
			stSvcCond.nLcnFlag	= FALSE;
			stSvcCond.nOrgDeliType	= eOrgDeliType;
			stSvcCond.nSvcType = eSvcType;

			hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &ulSvcCnt, &phSvcList);
			if(hErr == ERR_OK && ulSvcCnt > 0 && phSvcList != NULL)
			{
				for (i = 0; i < ulSvcCnt; i++)
				{
					hErr = DB_SVC_GetServiceInfo(phSvcList[i], &stSvcInfo);
					if (hErr != ERR_OK)
						continue;

					if (DbSvc_GetLcn(&stSvcInfo) != ulNonLcnStartNum)
					{
						HxLOG_Debug("change lcn number %d to %d\n", DbSvc_GetLcn(&stSvcInfo), ulNonLcnStartNum);
						DbSvc_SetLcn(&stSvcInfo, ulNonLcnStartNum);
						DB_SVC_UpdateServiceInfo(phSvcList[i], &stSvcInfo);
					}
					ulNonLcnStartNum++;
				}
			}

			DB_SVC_GetLcnOrderNumber (eOrgDeliType, eSvcType, &usUnassignedNonLcnNum);
			usUnassignedNonLcnNum += xsvc_si_LibLcnGetSvcNumBase (eLcnPolicy, eOrgDeliType, eSvcType);
		}
	}
	else
	{
		// do something, if needed.
	}

	*pusLcn = usUnassignedNonLcnNum;
	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnIsSignalStrongerThanDbTsInfo_ItaDtt (DbSvc_TsInfo_t *pstNewTsInfo, DbSvc_TsInfo_t *pstDbTsInfo, HBOOL *pbStronger)
{
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

STATIC HBOOL xsvc_si_LcnIsSameLcnInfo_ItaDtt (void *pvDstLcnInfo, void *pvSrcLcnInfo)
{
	xsvcSi_LcnInfo_t	*pstDestLcnInfo = (xsvcSi_LcnInfo_t*) pvDstLcnInfo;
	xsvcSi_LcnInfo_t	*pstSrcLcnInfo = (xsvcSi_LcnInfo_t*) pvSrcLcnInfo;

	if ( (pstDestLcnInfo->usOnId == pstSrcLcnInfo->usOnId)
		&& (pstDestLcnInfo->usTsId == pstSrcLcnInfo->usTsId)
		&& (pstDestLcnInfo->usServiceId == pstSrcLcnInfo->usServiceId)
		&& (pstDestLcnInfo->usLogicalChannelNumber == pstSrcLcnInfo->usLogicalChannelNumber) )
	{

		HxLOG_Print ("\t\t ===> found same LCN info : (%d:0x%x:0x%x:0x%x:0x%x:%d)\n",
			pstSrcLcnInfo->bUsed, pstSrcLcnInfo->usNetId, pstSrcLcnInfo->usOnId,
			pstSrcLcnInfo->usTsId, pstSrcLcnInfo->usServiceId, pstSrcLcnInfo->usLogicalChannelNumber);

		return TRUE;
	}

	return FALSE;
}

STATIC HERROR xsvc_si_LcnFindUnprocessedSvcLcnInfoFromHdScLcnList_ItaDtt (HxList_t* pHdSimulCastLcnList, HxList_t* pLcnList, DbSvc_Info_t* pstSvcInfo, xsvcSi_LcnInfo_t *pstLcnInfo)
{
	xsvcSi_LcnInfo_t	*pstItemLcnInfo = NULL;
	HxList_t			*pstItem = NULL;
	DbSvc_NetInfo_t 	stNetworkInfo;
	DbSvc_TsInfo_t 		stTsInfo;
	HUINT16				usLcn;
	HBOOL	bHdSvcType = FALSE;
	HBOOL	bLessHdLcn = FALSE;
	HBOOL	bNotExistedInLcn = FALSE;

	HLIB_LIST_First(pHdSimulCastLcnList);
	for (pstItem = pHdSimulCastLcnList; NULL != pstItem; pstItem = pstItem->next)
	{
		pstItemLcnInfo = (xsvcSi_LcnInfo_t *)HLIB_LIST_Data(pstItem);
		if (pstItemLcnInfo == NULL)
		{
			HxLOG_Critical("===== Error in xsvc_si_LcnFindUnprocessedSvcLcnInfoFromHdScLcnList_ItaDtt (LCN) =========\n");
			continue;
		}

		if (pstItemLcnInfo->bUsed == FALSE
			&& pstItemLcnInfo->usTsId == DbSvc_GetTsId(pstSvcInfo)
			&& pstItemLcnInfo->usOnId == DbSvc_GetOnId(pstSvcInfo)
			&& pstItemLcnInfo->usServiceId == DbSvc_GetSvcId(pstSvcInfo))
		{
			pstItemLcnInfo->bUsed = TRUE;

			HxLOG_Print ("\t\t ===> found unprocessed LCN info : name = %s, LCN(%d:%d), Type=0x%x, casType=0x%x, conflict=%d\n",
									DbSvc_GetName(pstSvcInfo), DbSvc_GetLcnFlag(pstSvcInfo), DbSvc_GetLcn(pstSvcInfo),
									DbSvc_GetSvcType(pstSvcInfo), DbSvc_GetCasType(pstSvcInfo), DbSvc_GetSvcConflictFlag(pstSvcInfo));

			// Step 2> 1.a
			// check network id (0x3001 - 3100)
			DB_SVC_GetTsInfo(DbSvc_GetTsIdx(pstSvcInfo), &stTsInfo);
			DB_SVC_GetNetworkInfo(stTsInfo.usNetIdx, &stNetworkInfo);

			HxLOG_Print ("check geographic region (0x%x) \n", stNetworkInfo.usNetId);

			if (LOCAL_MACRO_IS_ITA_TER_NETWORK_ID(stNetworkInfo.usNetId))
			{
				// Step 2> 1.b
				bHdSvcType = (DbSvc_GetVideoType(pstSvcInfo) == eDxVIDEO_TYPE_HD) ? TRUE : FALSE;
				// Step 2> 1.c
				usLcn = DbSvc_GetLcn(pstSvcInfo);
				if (usLcn > ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM)
				{
					usLcn -= ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM;
				}

				bLessHdLcn = (pstItemLcnInfo->usLogicalChannelNumber < usLcn) ? TRUE : FALSE;
				// Step 2> 1.d
				bNotExistedInLcn = (HLIB_LIST_FindEx(pLcnList, (const void*)pstItemLcnInfo, xsvc_si_LcnIsSameLcnInfo_ItaDtt) == NULL) ? TRUE : FALSE;

				HxLOG_Print ("\t\t ===> check unprocessed service condition : hd(%d), hd lcn < lcn(%d), not existed in lcn(%d) \n", bHdSvcType, bLessHdLcn, bNotExistedInLcn);

				if (bHdSvcType || bLessHdLcn || bNotExistedInLcn)
				{
					*pstLcnInfo = *pstItemLcnInfo;
					return ERR_OK;
				}
				else
				{
					return ERR_FAIL;
				}
			}
		}
	}

	return ERR_FAIL;
}

STATIC HERROR xsvc_si_LcnFindDisplacedLcnInfoFromHdScLcnList_ItaDtt (HxList_t* pHdSimulCastLcnList, DbSvc_Info_t* pstSvcInfo, xsvcSi_LcnInfo_t *pstLcnInfo)
{
	xsvcSi_LcnInfo_t	*pstItemLcnInfo = NULL;
	HxList_t			*pstItem = NULL;
	DbSvc_NetInfo_t 	stDbNetworkInfo;
	DbSvc_TsInfo_t 		stTsInfo;

	// check network id (0x3001 - 3100)
	DB_SVC_GetTsInfo(DbSvc_GetTsIdx(pstSvcInfo), &stTsInfo);
	DB_SVC_GetNetworkInfo(stTsInfo.usNetIdx, &stDbNetworkInfo);

	HxLOG_Print ("check geographic region (0x%x) \n", stDbNetworkInfo.usNetId);

	if (LOCAL_MACRO_IS_ITA_TER_NETWORK_ID(stDbNetworkInfo.usNetId))
	{

		HLIB_LIST_First(pHdSimulCastLcnList);
		for (pstItem = pHdSimulCastLcnList; NULL != pstItem; pstItem = pstItem->next)
		{
			pstItemLcnInfo = (xsvcSi_LcnInfo_t *)HLIB_LIST_Data(pstItem);
			if (pstItemLcnInfo == NULL)
			{
				HxLOG_Critical("===== Error in xsvc_si_LcnFindDisplacedLcnInfoFromHdScLcnList_ItaDtt (LCN) =========\n");
				continue;
			}

			if (pstItemLcnInfo->bUsed == FALSE
				&& pstItemLcnInfo->usTsId == DbSvc_GetTsId(pstSvcInfo)
				&& pstItemLcnInfo->usOnId == DbSvc_GetOnId(pstSvcInfo)
				&& pstItemLcnInfo->usServiceId == DbSvc_GetSvcId(pstSvcInfo))
			{
				pstItemLcnInfo->bUsed = TRUE;

				HxLOG_Print ("\t\t ===> found displaced LCN info : (0x%x:0x%x:0x%x:0x%x:%d) name = %s, LCN(%d:%d), Type=0x%x, casType=0x%x, conflict=%d\n",
										pstItemLcnInfo->usNetId, pstItemLcnInfo->usOnId, pstItemLcnInfo->usTsId,
										pstItemLcnInfo->usServiceId, pstItemLcnInfo->usLogicalChannelNumber,
										DbSvc_GetName(pstSvcInfo), DbSvc_GetLcnFlag(pstSvcInfo), DbSvc_GetLcn(pstSvcInfo),
										DbSvc_GetSvcType(pstSvcInfo), DbSvc_GetCasType(pstSvcInfo), DbSvc_GetSvcConflictFlag(pstSvcInfo));

				*pstLcnInfo = *pstItemLcnInfo;
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

STATIC HERROR xsvc_si_LcnFindDbSvcInfoByLcn_ItaDtt (HUINT16 usLcn, DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, Handle_t *phSvc, DbSvc_Info_t *pstSvcInfo)
{
	HERROR 			hErr;
	HUINT32 		i, ulSvcCnt;
	Handle_t 		hSvc = 0, *phSvcList = NULL;
	HUINT16 		usTempLcn;
	DbSvc_Attr_t 	stAttr;
	DbSvc_Info_t	stSvcInfo;

	DB_SVC_InitSvcAttribute (&stAttr);

	stAttr.eOrgDeliType = eOrgDeliType;
	stAttr.eSvcType = eSvcType;

	hErr = DB_SVC_FindServiceList(eSvcGroup_All, &stAttr, 0, HANDLE_NULL, eSvcFindDirection_FowardFromStart, &ulSvcCnt, &phSvcList);

	HxLOG_Debug ("\n DB_SVC_FindServiceList (cnt = %d)\n", ulSvcCnt);
	if (hErr == ERR_OK)
	{
		for (i=0; i<ulSvcCnt; i++)
		{
			hErr = DB_SVC_GetServiceInfo(phSvcList[i], &stSvcInfo);
			if (hErr != ERR_OK)
				continue;

			usTempLcn = DbSvc_GetLcn(&stSvcInfo);
			if (usTempLcn > ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM)
			{
				usTempLcn -= ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM;
			}

			if (usLcn == usTempLcn)
			{
				HxLOG_Print ("\t\t ===> found conflicting svc info : (0x%x:0x%x:0x%x) name = %s, LCN(%d:%d), Type=0x%x, casType=0x%x, conflict=%d\n",
										DbSvc_GetOnId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo),
										DbSvc_GetName(&stSvcInfo), DbSvc_GetLcnFlag(&stSvcInfo), DbSvc_GetLcn(&stSvcInfo),
										DbSvc_GetSvcType(&stSvcInfo), DbSvc_GetCasType(&stSvcInfo), DbSvc_GetSvcConflictFlag(&stSvcInfo));
				hSvc = phSvcList[i];
				break;
			}
		}

		DB_SVC_FreeServiceList(phSvcList);
	}

	*phSvc = hSvc;
	*pstSvcInfo = stSvcInfo;

	return (hSvc == 0) ? ERR_FAIL : ERR_OK;
}

STATIC HERROR xsvc_si_LcnFindLcnInfoFromLcnList_ItaDtt (HxList_t *pLcnList, DbSvc_Info_t *pstSvcInfo, xsvcSi_LcnInfo_t *pstLcnInfo)
{
	xsvcSi_LcnInfo_t *pstItemLcnInfo = NULL;
	HxList_t	*pstItem = NULL;

	HLIB_LIST_First(pLcnList);

	for (pstItem = pLcnList; NULL != pstItem; pstItem = pstItem->next)
	{
		pstItemLcnInfo = (xsvcSi_LcnInfo_t *)HLIB_LIST_Data(pstItem);
		if (pstItemLcnInfo == NULL)
		{
			HxLOG_Critical("===== Error in xsvc_si_LcnFindLcnInfoFromLcnList_ItaDtt (LCN) =========\n");
			continue;
		}

		if (pstItemLcnInfo->bUsed == FALSE
			&& pstItemLcnInfo->usTsId == DbSvc_GetTsId(pstSvcInfo)
			&& pstItemLcnInfo->usOnId == DbSvc_GetOnId(pstSvcInfo)
			&& pstItemLcnInfo->usServiceId == DbSvc_GetSvcId(pstSvcInfo))
		{
			pstItemLcnInfo->bUsed = TRUE;

			HxLOG_Print ("\t\t ===> found LCN info : (0x%x:0x%x:0x%x:0x%x) name = %s, LCN(%d:%d), Type=0x%x, casType=0x%x, conflict=%d\n",
									pstItemLcnInfo->usNetId, pstItemLcnInfo->usOnId, pstItemLcnInfo->usTsId, pstItemLcnInfo->usServiceId,
									DbSvc_GetName(pstSvcInfo), DbSvc_GetLcnFlag(pstSvcInfo), DbSvc_GetLcn(pstSvcInfo),
									DbSvc_GetSvcType(pstSvcInfo), DbSvc_GetCasType(pstSvcInfo), DbSvc_GetSvcConflictFlag(pstSvcInfo));

			*pstLcnInfo = *pstItemLcnInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR xsvc_si_LcnAssignPrefereneOrAssignmentWoMainOverFlowZone_ItaDtt(DbSvc_Info_t *pstSvcInfo, HUINT16 usLcn, HUINT8 ucVisibleServiceFlag)
{
	DbSvc_SetLcnFlag(pstSvcInfo, TRUE);
	DbSvc_SetLcn(pstSvcInfo, usLcn);
	DbSvc_SetVisibleFlag(pstSvcInfo, ucVisibleServiceFlag);

	HxLOG_Print ("\t\t ===> assign svc info : (0x%x:0x%x:0x%x) name = %s, LCN(%d:%d), Type=0x%x, casType=0x%x, conflict=%d\n",
								DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo),
								DbSvc_GetName(pstSvcInfo), DbSvc_GetLcnFlag(pstSvcInfo), DbSvc_GetLcn(pstSvcInfo),
								DbSvc_GetSvcType(pstSvcInfo), DbSvc_GetCasType(pstSvcInfo), DbSvc_GetSvcConflictFlag(pstSvcInfo));

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnAssignMainOverflow_ItaDtt(DbSvc_Info_t *pstSvcInfo, HUINT16 usLcn)
{
	DxSvcType_e 		eSvcType;

	if (pstSvcInfo == NULL)
		return ERR_FAIL;

	eSvcType = DbSvc_GetSvcType(pstSvcInfo);

	HxLOG_Print ("unassigned Non-Lcn number(%d)\n", s_usNonLcnStartNum[eSvcType]);

	DbSvc_SetLcnFlag(pstSvcInfo, FALSE);
	DbSvc_SetLcn(pstSvcInfo, s_usNonLcnStartNum[eSvcType]);
	//DbSvc_SetOrgLcn(pstSvcInfo, usLcn);

	HxLOG_Print ("\t\t ===> searched svc info : (0x%x:0x%x:0x%x) name = %s, LCN(%d:%d), Type=0x%x, casType=0x%x, conflict=%d\n",
							DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo),
							DbSvc_GetName(pstSvcInfo), DbSvc_GetLcnFlag(pstSvcInfo), DbSvc_GetLcn(pstSvcInfo),
							DbSvc_GetSvcType(pstSvcInfo), DbSvc_GetCasType(pstSvcInfo), DbSvc_GetSvcConflictFlag(pstSvcInfo));

	if (ITA_LCN_IS_MAIN_OVERFLOW_ZONE(s_usNonLcnStartNum[eSvcType]))
	{
		if (s_usNonLcnStartNum[eSvcType] == ITA_LCN_MAIN_OVERFLOW_END)
		{
			s_usNonLcnStartNum[eSvcType] = ITA_LCN_MAIN_OVERFLOW_START - 1;
		}
		else
		{
			s_usNonLcnStartNum[eSvcType]++;
		}
	}
	else
	{
		s_usNonLcnStartNum[eSvcType]--;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnAssignHdSimulcastLcnInfo_ItaDtt(xsvcSi_LcnInfo_t stLcnInfo, DbSvc_Info_t *pstSvcInfo, HxList_t* pHdSimulCastLcnList)
{
	Handle_t 			hSvc = (Handle_t)NULL;
	DbSvc_Info_t		stDbSvcInfo, stDbDisplacedSvcInfo;
	DxDeliveryType_e	eOrgDeliType;
	DxSvcType_e 		eSvcType;
	xsvcSi_LcnInfo_t 	stDisplacedLcnInfo;

	if (pstSvcInfo == NULL)
		return ERR_FAIL;

	eOrgDeliType = DbSvc_GetDeliTypePtr(pstSvcInfo);
	eSvcType = DbSvc_GetSvcType(pstSvcInfo);

	// Step 2> 3.a
	if (xsvc_si_LcnFindDbSvcInfoByLcn_ItaDtt(stLcnInfo.usLogicalChannelNumber, eOrgDeliType, eSvcType, &hSvc, &stDbSvcInfo) == ERR_OK)
	{
FIND_HDLCN_RECURSIVE:
		// Step 2> 3.c : find displaced service
		if (xsvc_si_LcnFindDisplacedLcnInfoFromHdScLcnList_ItaDtt(pHdSimulCastLcnList, &stDbSvcInfo, &stDisplacedLcnInfo) == ERR_OK)
		{
			eOrgDeliType = DbSvc_GetDeliType(stDbSvcInfo);
			eSvcType = DbSvc_GetSvcType(&stDbSvcInfo);

			// Step 2> 3.e
			if (xsvc_si_LcnFindDbSvcInfoByLcn_ItaDtt(stDisplacedLcnInfo.usLogicalChannelNumber, eOrgDeliType, eSvcType, &hSvc, &stDbDisplacedSvcInfo) == ERR_OK)
			{
				// Step 2> 3.g
				if (ITA_LCN_IS_PREFERENCES_ZONE(stDisplacedLcnInfo.usLogicalChannelNumber) || ITA_LCN_IS_ASSIGNMENT_WO_MO_ZONE(stDisplacedLcnInfo.usLogicalChannelNumber))
				{
					xsvc_si_LcnAssignPrefereneOrAssignmentWoMainOverFlowZone_ItaDtt(&stDbSvcInfo, stDisplacedLcnInfo.usLogicalChannelNumber, stDisplacedLcnInfo.ucVisibleServiceFlag);
				}
				else
				{
					xsvc_si_LcnAssignMainOverflow_ItaDtt(&stDbSvcInfo, stDisplacedLcnInfo.usLogicalChannelNumber);
				}

				// if already confliced, change to unconflicted.
				if (DbSvc_GetSvcConflictFlag(&stDbSvcInfo) == TRUE)
				{
					ITA_LCN_SUB_CONFLICT_MAGIC_NUM(&stDbSvcInfo);
					DbSvc_SetSvcConflictFlag(&stDbSvcInfo, FALSE);
				}
				DB_SVC_UpdateServiceInfo(hSvc, &stDbSvcInfo);
				stDbSvcInfo = stDbDisplacedSvcInfo;
				// Step 2> 3.h
				goto FIND_HDLCN_RECURSIVE; // change this goto statement to recursive function
			}
			else
			{
				// Step 2> 3.f
				if (ITA_LCN_IS_PREFERENCES_ZONE(stDisplacedLcnInfo.usLogicalChannelNumber) || ITA_LCN_IS_ASSIGNMENT_WO_MO_ZONE(stDisplacedLcnInfo.usLogicalChannelNumber))
				{
					xsvc_si_LcnAssignPrefereneOrAssignmentWoMainOverFlowZone_ItaDtt(&stDbSvcInfo, stDisplacedLcnInfo.usLogicalChannelNumber, stDisplacedLcnInfo.ucVisibleServiceFlag);
				}
				else
				{
					xsvc_si_LcnAssignMainOverflow_ItaDtt(&stDbSvcInfo, stDisplacedLcnInfo.usLogicalChannelNumber);
				}

				// if already confliced, change to unconflicted.
				if (DbSvc_GetSvcConflictFlag(&stDbSvcInfo) == TRUE)
				{
					ITA_LCN_SUB_CONFLICT_MAGIC_NUM(&stDbSvcInfo);
					DbSvc_SetSvcConflictFlag(&stDbSvcInfo, FALSE);
				}
				DB_SVC_UpdateServiceInfo(hSvc, &stDbSvcInfo);
			}
		}
		else
		{
			// Step 2> 3.d
			// if already confliced, change to unconflicted.
			if (DbSvc_GetSvcConflictFlag(&stDbSvcInfo) == TRUE)
			{
				ITA_LCN_SUB_CONFLICT_MAGIC_NUM(&stDbSvcInfo);
				DbSvc_SetSvcConflictFlag(&stDbSvcInfo, FALSE);
			}

			xsvc_si_LcnAssignMainOverflow_ItaDtt(&stDbSvcInfo, DbSvc_GetLcn(&stDbSvcInfo));
			DB_SVC_UpdateServiceInfo(hSvc, &stDbSvcInfo);
		}
	}

	// Step 2> 3.b
	if (ITA_LCN_IS_PREFERENCES_ZONE(stLcnInfo.usLogicalChannelNumber) || ITA_LCN_IS_ASSIGNMENT_WO_MO_ZONE(stLcnInfo.usLogicalChannelNumber))
	{
		xsvc_si_LcnAssignPrefereneOrAssignmentWoMainOverFlowZone_ItaDtt(pstSvcInfo, stLcnInfo.usLogicalChannelNumber, stLcnInfo.ucVisibleServiceFlag);
	}
	else
	{
		xsvc_si_LcnAssignMainOverflow_ItaDtt(pstSvcInfo, stLcnInfo.usLogicalChannelNumber);
	}

	// if already confliced, change to unconflicted.
	if (DbSvc_GetSvcConflictFlag(pstSvcInfo) == TRUE)
	{
		ITA_LCN_SUB_CONFLICT_MAGIC_NUM(pstSvcInfo);
		DbSvc_SetSvcConflictFlag(pstSvcInfo, FALSE);
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnAssignLcnInfo_ItaDtt(xsvcSi_LcnInfo_t stLcnInfo, DbSvc_Info_t *pstNewSvcInfo, SRCHSVC_AppendFlag_t *peSvcFlag)
{
	HERROR 				hErr;
	Handle_t 			hSvc = (Handle_t)NULL;
	DbSvc_Info_t		stDbSvcInfo;
	DxDeliveryType_e	eOrgDeliType;
	DxSvcType_e 		eSvcType;

	if (pstNewSvcInfo == NULL)
		return ERR_FAIL;

	eOrgDeliType = DbSvc_GetDeliTypePtr(pstNewSvcInfo);
	eSvcType = DbSvc_GetSvcType(pstNewSvcInfo);

	// find conflicting LCN service info
	if (xsvc_si_LcnFindDbSvcInfoByLcn_ItaDtt(stLcnInfo.usLogicalChannelNumber, eOrgDeliType, eSvcType, &hSvc, &stDbSvcInfo) == ERR_OK)
	{
		if ((DbSvc_GetOnId(pstNewSvcInfo) == DbSvc_GetOnId(&stDbSvcInfo))
			&& (DbSvc_GetTsId(pstNewSvcInfo) == DbSvc_GetTsId(&stDbSvcInfo))
			&& (DbSvc_GetSvcId(pstNewSvcInfo) == DbSvc_GetSvcId(&stDbSvcInfo)))
		{	// Step 1> 1
			DbSvc_TsInfo_t stNewTsInfo, stDbTsInfo;

			HxLOG_Print ("same triplet service \n");

			// check ts index whether same or not
			hErr = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(pstNewSvcInfo), &stNewTsInfo);
			hErr |= DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stDbSvcInfo), &stDbTsInfo);
			if (hErr == ERR_OK)
			{
				if (stNewTsInfo.usTsIdx == stDbTsInfo.usTsIdx)
				{
					HxLOG_Print ("found same ts info (0x%x, 0x%x), so skipped\n", stNewTsInfo.usTsIdx, stDbTsInfo.usTsIdx);
					*peSvcFlag = eSRCHSVC_SKIP;
				}
				else
				{ //compare QoS

					HBOOL	bSignalStrong;

					HxLOG_Debug ("found diff ts info (0x%x, 0x%x), so compare QoS\n", stNewTsInfo.usTsIdx, stDbTsInfo.usTsIdx);
					xsvc_si_LcnIsSignalStrongerThanDbTsInfo_ItaDtt(&stNewTsInfo, &stDbTsInfo, &bSignalStrong);
					if (bSignalStrong == TRUE)
					{
						xsvc_si_LcnAssignPrefereneOrAssignmentWoMainOverFlowZone_ItaDtt(pstNewSvcInfo, stLcnInfo.usLogicalChannelNumber, stLcnInfo.ucVisibleServiceFlag);

						xsvc_si_LcnAssignMainOverflow_ItaDtt(&stDbSvcInfo, stLcnInfo.usLogicalChannelNumber);
						DB_SVC_UpdateServiceInfo(hSvc, &stDbSvcInfo);

					}
					else
					{
						xsvc_si_LcnAssignMainOverflow_ItaDtt(pstNewSvcInfo, stLcnInfo.usLogicalChannelNumber);
					}
				}
			}
		}
		else
		{ // Step 1> 2, 3, 4
			DbSvc_NetInfo_t stNewNetworkInfo, stDbNetworkInfo;
			DbSvc_TsInfo_t stTsInfo;

			HxLOG_Print ("different triplet service \n");

			// check network id (0x3001 - 3100)
			DB_SVC_GetTsInfo(DbSvc_GetTsIdx(pstNewSvcInfo), &stTsInfo);
			DB_SVC_GetNetworkInfo(stTsInfo.usNetIdx, &stNewNetworkInfo);

			DB_SVC_GetNetworkInfoByServiceHandle(hSvc, &stDbNetworkInfo);

			HxLOG_Print ("check geographic region (0x%x / 0x%x) \n", stNewNetworkInfo.usNetId, stDbNetworkInfo.usNetId);

			if (LOCAL_MACRO_IS_ITA_TER_NETWORK_ID(stNewNetworkInfo.usNetId))
			{
				if (LOCAL_MACRO_IS_ITA_TER_NETWORK_ID(stDbNetworkInfo.usNetId))
				{
					// Step 1> 3
					HxLOG_Print ("customer resolution \n");

					// firstly assign LCN to new service and add conflict list.
					xsvc_si_LcnAssignPrefereneOrAssignmentWoMainOverFlowZone_ItaDtt(pstNewSvcInfo, stLcnInfo.usLogicalChannelNumber + ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM, stLcnInfo.ucVisibleServiceFlag);

					DbSvc_SetSvcConflictFlag(pstNewSvcInfo, TRUE);

					ITA_LCN_ADD_CONFLICT_MAGIC_NUM(&stDbSvcInfo);
					DbSvc_SetSvcConflictFlag(&stDbSvcInfo, TRUE);
					DB_SVC_UpdateServiceInfo(hSvc, &stDbSvcInfo);
				}
				else
				{
					HxLOG_Print ("new service take priority\n");

					// Step 1> 2
					xsvc_si_LcnAssignPrefereneOrAssignmentWoMainOverFlowZone_ItaDtt(pstNewSvcInfo, stLcnInfo.usLogicalChannelNumber, stLcnInfo.ucVisibleServiceFlag);

					xsvc_si_LcnAssignMainOverflow_ItaDtt(&stDbSvcInfo, stLcnInfo.usLogicalChannelNumber);
					DB_SVC_UpdateServiceInfo(hSvc, &stDbSvcInfo);
				}
			}
			else
			{
				if (LOCAL_MACRO_IS_ITA_TER_NETWORK_ID(stDbNetworkInfo.usNetId))
				{
					HxLOG_Print ("db service take priority \n");

					// Step 1> 2
					xsvc_si_LcnAssignMainOverflow_ItaDtt(pstNewSvcInfo, stLcnInfo.usLogicalChannelNumber);
				}
				else
				{
					// Step 1> 4
					HxLOG_Print ("customer resolution \n");

					// firstly assign LCN to new service and add conflict list.
					xsvc_si_LcnAssignPrefereneOrAssignmentWoMainOverFlowZone_ItaDtt(pstNewSvcInfo, stLcnInfo.usLogicalChannelNumber + ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM, stLcnInfo.ucVisibleServiceFlag);

					DbSvc_SetSvcConflictFlag(pstNewSvcInfo, TRUE);

					ITA_LCN_ADD_CONFLICT_MAGIC_NUM(&stDbSvcInfo);
					DbSvc_SetSvcConflictFlag(&stDbSvcInfo, TRUE);
					DB_SVC_UpdateServiceInfo(hSvc, &stDbSvcInfo);
				}
			}

		}
	}
	else
	{	// no conflict
		xsvc_si_LcnAssignPrefereneOrAssignmentWoMainOverFlowZone_ItaDtt(pstNewSvcInfo, stLcnInfo.usLogicalChannelNumber, stLcnInfo.ucVisibleServiceFlag);
	}

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___

// ============================================================================================
// Function : xsvc_si_LcnSetServiceNumber_ItaDtt
HERROR xsvc_si_LcnSetServiceNumber_ItaDtt (Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	HERROR				hErr;
	HUINT32				ulValue;
	HUINT32			 	ulIndex;
	xsvcSi_LcnInfo_t 	stLcnInfo;
	DbSvc_Info_t		*pstSvcInfo;
	HxList_t			*pLcnList = NULL;
	HxList_t			*pHdSimulCastLcnList = NULL;
	HxList_t			*pstItem = NULL;

	// TODO: check automatic numbering
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_TER_SRCH_AUTO_CH_NUMBERING, &ulValue, 0);
	ulValue = (hErr == ERR_OK) ? ulValue : (HUINT8)0;

	if (0) // if (ulValue == 0) // not automatic numbering by manufacture
	{
		xsvc_si_LcnGetUnassginedNonLcn_ItaDtt(eDEFAULTLCN_NORMAL, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV, &s_usNonLcnStartNum[eDxSVC_TYPE_TV]);
		xsvc_si_LcnGetUnassginedNonLcn_ItaDtt(eDEFAULTLCN_NORMAL, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO, &s_usNonLcnStartNum[eDxSVC_TYPE_RADIO]);

		HxLOG_Print ("unassigned Non-Lcn tv number(%d)\n", s_usNonLcnStartNum[eDxSVC_TYPE_TV]);
		HxLOG_Print ("unassigned Non-Lcn radio number(%d)\n", s_usNonLcnStartNum[eDxSVC_TYPE_RADIO]);

		for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
		{
			if (peFlagArray[ulIndex] != eSRCHSVC_ADD &&
				peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
			{
				continue;
			}

			pstSvcInfo = &(pstSvcArray[ulIndex]);
			switch (DbSvc_GetSvcType(pstSvcInfo))
			{
			case eDxSVC_TYPE_TV:
				DbSvc_SetLcn(pstSvcInfo, s_usNonLcnStartNum[eDxSVC_TYPE_TV]);
				s_usNonLcnStartNum[eDxSVC_TYPE_TV]++;
				break;

			case eDxSVC_TYPE_RADIO:
				DbSvc_SetLcn(pstSvcInfo, s_usNonLcnStartNum[eDxSVC_TYPE_RADIO]);
				s_usNonLcnStartNum[eDxSVC_TYPE_RADIO]++;
				break;

			default:
				HxLOG_Critical("\n\n");
				HxLOG_Error("===> Strange Type %d\n", DbSvc_GetSvcType(pstSvcInfo));
				break;
			}
		}
	}
	else // automatic numbering by lcn
	{
		xsvc_si_LcnGetUnassginedNonLcn_ItaDtt(eDEFAULTLCN_ITADTT, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV, &s_usNonLcnStartNum[eDxSVC_TYPE_TV]);
		xsvc_si_LcnGetUnassginedNonLcn_ItaDtt(eDEFAULTLCN_ITADTT, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO, &s_usNonLcnStartNum[eDxSVC_TYPE_RADIO]);

		/* start Step 1> as above */
		if (xsvc_si_LcnGetLogicalChannelFromNit_ItaDtt(PAL_PIPE_GetActionId(hAction), &pLcnList) == ERR_OK)
		{
			for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
			{
				if (peFlagArray[ulIndex] != eSRCHSVC_ADD
					&& peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
				{
					continue;
				}

				pstSvcInfo = &(pstSvcArray[ulIndex]);

				switch (DbSvc_GetSvcType(pstSvcInfo))
				{
				case eDxSVC_TYPE_TV:
				case eDxSVC_TYPE_RADIO:
					/* find LCN in LCN list */
					if (xsvc_si_LcnFindLcnInfoFromLcnList_ItaDtt(pLcnList, pstSvcInfo, &stLcnInfo) == ERR_OK)
					{
						if (ITA_LCN_IS_PREFERENCES_ZONE(stLcnInfo.usLogicalChannelNumber) || ITA_LCN_IS_ASSIGNMENT_WO_MO_ZONE(stLcnInfo.usLogicalChannelNumber))
						{
							// assign found LCN
							xsvc_si_LcnAssignLcnInfo_ItaDtt(stLcnInfo, pstSvcInfo, &(peFlagArray[ulIndex]));
						}
						else
						{
							// assign LCN to MainOver flow
							xsvc_si_LcnAssignMainOverflow_ItaDtt(pstSvcInfo, stLcnInfo.usLogicalChannelNumber);
						}
					}
					else
					{
						// assign LCN to MainOver flow
						xsvc_si_LcnAssignMainOverflow_ItaDtt(pstSvcInfo, 0);
					}
					break;
				default:
					HxLOG_Critical("\n\n");
					HxLOG_Error("===> Strange Type %d\n", DbSvc_GetSvcType(pstSvcInfo));
					DbSvc_SetLcn(pstSvcInfo, ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM);
				}
			}
		}
		else
		{
			HxLOG_Error ("\n\t xsvc_si_LcnSetServiceNumber_ItaDtt ::: error in xsvc_si_LcnGetLogicalChannelFromNit_ItaDtt\n");

			for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
			{
				if (peFlagArray[ulIndex] != eSRCHSVC_ADD
					&& peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
				{
					continue;
				}

				pstSvcInfo = &(pstSvcArray[ulIndex]);
				switch (DbSvc_GetSvcType(pstSvcInfo))
				{
				case eDxSVC_TYPE_TV:
				case eDxSVC_TYPE_RADIO:
					// assign LCN to MainOver flow
					xsvc_si_LcnAssignMainOverflow_ItaDtt(pstSvcInfo, 0);
					break;
				default:
					HxLOG_Critical("\n\n");
					HxLOG_Error("===> Strange Type %d\n", DbSvc_GetSvcType(pstSvcInfo));
					DbSvc_SetLcn(pstSvcInfo, ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM);
				}
			}
		}

		//Step 2>  assign the LCN to each servcie according to HD Simulcast Locigal Channel Descriptor
		if (xsvc_si_LcnGetHdSimulcastLCFromNit_ItaDtt(PAL_PIPE_GetActionId(hAction), &pHdSimulCastLcnList) == ERR_OK)
		{
			for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
			{
				if (peFlagArray[ulIndex] != eSRCHSVC_ADD
					&& peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
				{
					continue;
				}

				pstSvcInfo = &(pstSvcArray[ulIndex]);

				// Step 2> 1 find unprocessed service lcn in LCN list
				if (xsvc_si_LcnFindUnprocessedSvcLcnInfoFromHdScLcnList_ItaDtt(pHdSimulCastLcnList, pLcnList, pstSvcInfo, &stLcnInfo) == ERR_OK)
				{
					// Step 2> 2  // TODO: What is BER threshold value?

					// Step 2> 3
					xsvc_si_LcnAssignHdSimulcastLcnInfo_ItaDtt(stLcnInfo, pstSvcInfo, pHdSimulCastLcnList);
 				}


			}
		}
		else
		{
			HxLOG_Error ("\n\t xsvc_si_LcnSetServiceNumber_ItaDtt ::: No HD Simulcast LCN in xsvc_si_LcnGetHdSimulcastLCFromNit_ItaDtt\n");
		}

		if (pHdSimulCastLcnList)
		{
			HLIB_LIST_First(pHdSimulCastLcnList);
			for (pstItem = pHdSimulCastLcnList; NULL != pstItem; pstItem = pstItem->next)
			{
				void *pvData = (void *)HLIB_LIST_Data(pstItem);
				if (pvData) OxSI_Free(pvData);

			}

			HLIB_LIST_RemoveAll(pHdSimulCastLcnList);
		}

		if (pLcnList)
		{
			HLIB_LIST_First(pLcnList);
			for (pstItem = pLcnList; NULL != pstItem; pstItem = pstItem->next)
			{
				void *pvData = (void *)HLIB_LIST_Data(pstItem);
				if (pvData) OxSI_Free(pvData);

			}
			HLIB_LIST_RemoveAll(pLcnList);
		}
	}
	return ERR_OK;
}

HERROR xsvc_si_LcnSetSelectedConflictLcn_ItaDtt (Handle_t hAction, HUINT32 ulNum, DxBuilder_SearchedInfo_t *pstInfoArray)
{

	HERROR			err;
	DbSvc_Attr_t	stAttr;
	HUINT32 		i, j, ulSvcCnt;
	Handle_t		*phSvcList = NULL;
	DbSvc_Info_t	stSvcInfo;

	if ((0 == ulNum) || (NULL == pstInfoArray))
	{
		HxLOG_Error ("No Selected Conflict Lcn Info:\n");
		return ERR_FAIL;
	}

	DB_SVC_InitSvcAttribute (&stAttr);

	stAttr.eOrgDeliType = eDxDELIVERY_TYPE_TER;
	stAttr.eSvcType = eDxSVC_TYPE_All;
	DbSvc_SetUser1Flag(stAttr.eUserFlag1, eSvcUserFlag_31_SvcConflictLcn, TRUE);
	DbSvc_SetUser1Flag(stAttr.eUserMask1, eSvcUserFlag_31_SvcConflictLcn, TRUE);

	err = DB_SVC_FindServiceList(eSvcGroup_All, &stAttr, 0, HANDLE_NULL, eSvcFindDirection_FowardFromStart, &ulSvcCnt, &phSvcList);

	if (err == ERR_OK)
	{
		for (i = 0; i < ulSvcCnt; i++)
		{
			err = DB_SVC_GetServiceInfo(phSvcList[i], &stSvcInfo);
			if (err != ERR_OK)
				continue;

			for (j = 0; j < ulNum; j++)
			{
				if (pstInfoArray[j].ulLcn == 0) // invalid or already used
					continue;

				if (pstInfoArray[j].ulLcn < ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM)
				{
					pstInfoArray[j].ulLcn += ITA_LCN_CONFLICT_DISTICT_MAGIC_NUM;
				}

				if ((DbSvc_GetOnId(&stSvcInfo) == pstInfoArray[j].onId)
					&& (DbSvc_GetTsId(&stSvcInfo) == pstInfoArray[j].tsId)
					&& (DbSvc_GetSvcId(&stSvcInfo) == pstInfoArray[j].svcId))
				{

					ITA_LCN_SUB_CONFLICT_MAGIC_NUM(&stSvcInfo);
					DbSvc_SetSvcConflictFlag(&stSvcInfo, FALSE);
					DB_SVC_UpdateServiceInfo(phSvcList[i], &stSvcInfo);

					HxLOG_Print ("\t\t ===> set lcn to selected svc info : (0x%x:0x%x:0x%x) name = %s, LCN(%d:%d), Type=0x%x, casType=0x%x, conflict=%d\n",
															DbSvc_GetOnId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo),
															DbSvc_GetName(&stSvcInfo), DbSvc_GetLcnFlag(&stSvcInfo), DbSvc_GetLcn(&stSvcInfo),
															DbSvc_GetSvcType(&stSvcInfo), DbSvc_GetCasType(&stSvcInfo), DbSvc_GetSvcConflictFlag(&stSvcInfo));

					pstInfoArray[j].ulLcn = 0; // set as used
					break;
				}
			}

			if (j >= ulNum)
			{
				ITA_LCN_SUB_CONFLICT_MAGIC_NUM(&stSvcInfo);
				xsvc_si_LcnAssignMainOverflow_ItaDtt(&stSvcInfo, DbSvc_GetLcn(&stSvcInfo));
				DbSvc_SetSvcConflictFlag(&stSvcInfo, FALSE);
				DB_SVC_UpdateServiceInfo(phSvcList[i], &stSvcInfo);

				HxLOG_Print ("\t\t ===> set main overflow lcn to unselected svc info : (0x%x:0x%x:0x%x) name = %s, LCN(%d:%d), Type=0x%x, casType=0x%x, conflict=%d\n",
														DbSvc_GetOnId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo),
														DbSvc_GetName(&stSvcInfo), DbSvc_GetLcnFlag(&stSvcInfo), DbSvc_GetLcn(&stSvcInfo),
														DbSvc_GetSvcType(&stSvcInfo), DbSvc_GetCasType(&stSvcInfo), DbSvc_GetSvcConflictFlag(&stSvcInfo));

			}

		}

		DB_SVC_FreeServiceList(phSvcList);
	}

	return ERR_OK;
}

/* end of file */

