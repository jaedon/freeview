/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_builder_svcsrch.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>

#include "_om_builder.h"


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

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
HERROR om_builder_svcsrch_StartSearch (HUINT32 ulBuilderId)
{
	omBldContext_t		*pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	HUINT32				ulActionId;
	Handle_t			hAction;

	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context found: ulBuildId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("call DB_SVC_SetNotifier(FALSE)\n"));
	DB_SVC_SetNotifier(FALSE);

	ulActionId = pstContext->ulActionId;
	hAction = SVC_PIPE_GetActionHandle(ulActionId);

	pstContext->pfnSearchProc = MGR_SEARCH_GetSearchAction(ulActionId, pstContext->stOption.eDeliveryType);

	if (BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, pstContext->pfnSearchProc, hAction, (HINT32)&pstContext->stSearchOption, 0, 0) == ERR_OK)
	{
		pstContext->eState = eDxBUILDER_STATE_RUNNING;
		BUS_SendMessage(pstContext->pfnSearchProc, eMEVT_SEARCH_ACTION_START, hAction, 0, 0, 0);
	}

	return ERR_OK;
}

HERROR om_builder_svcsrch_StopSearch (HUINT32 ulBuilderId)
{
	omBldContext_t		*pstContext = om_builder_GetContextByBuilderId (ulBuilderId);

	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context found: ulBuildId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	BUS_MGR_Destroy(pstContext->pfnSearchProc);

	HxLOG_Print(HxANSI_COLOR_YELLOW("call DB_SVC_SetNotifier(TRUE)\n"));
	DB_SVC_SetNotifier(TRUE);

	pstContext->eState = eDxBUILDER_STATE_STOPPED;
	return ERR_OK;
}

BUS_Result_t om_builder_svcsrch_MsgSearchStatusChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 	 eResult = MESSAGE_PASS;
	HUINT32					 	 ulActionId = SVC_PIPE_GetActionId(hAction);
	omBldContext_t				*pstContext;

	OxBuilder_BuilderState_t	*pstBuilderState;
	CH_SignalInfo_t				 stSignalInfo;
	HUINT32						 ulNum;
	HUINT32						 ulSvcNameLen = 0;

	DxTuneParam_t 				 stChInfo;
	DxDeliveryType_e			 eDeliType;

	HUINT32 					 ulResourceId;
	DxTuneParam_t				 stTuningInfo;
	HERROR						 hErr = ERR_FAIL;

	pstContext = om_builder_GetContextByActionId (ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulActionId(%d)\n", ulActionId);
		return eResult;
	}

	pstBuilderState = &pstContext->stBuilderState;
	if (MGR_SEARCHRESULT_GetNumTpTotal(ulActionId, &ulNum) == ERR_OK)
	{
		pstBuilderState->ulTotalTuningNum= ulNum;
	}

	if (MGR_SEARCHRESULT_GetNumTpCompleted(ulActionId, &ulNum) == ERR_OK)
	{
		pstBuilderState->ulCompleteTuningNum= ulNum;
	}

	//	add searched list to status svcList Chain.
	hErr = MGR_SEARCHRESULT_CountService(ulActionId, eDxSVC_TYPE_All, &ulNum);
	if ((hErr == ERR_OK) && (ulNum > 0))
	{
		HINT32 i;

		HxLOG_Debug(">>> searched svc num : (%d)\n", pstBuilderState->ulSearchedSvcNum);

	 	// apk으로 보낼 svcInfo 만들자
		if (pstContext->pstTmpSvcInfo != NULL)
		{
			// 여기서 free되면 안되는데.. 디버깅 필요..!!
			HxLOG_Critical ("ERROR ! pstTmpSvcInfo(0x%X) already alloced..!! \n", pstContext->pstTmpSvcInfo);
			OxMGR_Free(pstContext->pstTmpSvcInfo);
			pstContext->pstTmpSvcInfo = NULL;
		}

		pstContext->ulSvcCount = ulNum - pstBuilderState->ulSearchedSvcNum;
		if (pstContext->ulSvcCount > 0)
		{
			pstContext->pstTmpSvcInfo = (DxBuilder_SearchedInfo_t *)OxMGR_Calloc(sizeof(DxBuilder_SearchedInfo_t) * pstContext->ulSvcCount);
			if (pstContext->pstTmpSvcInfo == NULL)
			{
				HxLOG_Error ("pstTmpSvcInfo alloc failed.. : ulActionId(%d)\n", ulActionId);
				return eResult;
			}

			for (i = pstBuilderState->ulSearchedSvcNum; i < ulNum; i++)
			{
				DbSvc_Info_t				 stSvcInfo;
				DxBuilder_SearchedInfo_t	*pstBuilderSvcInfo;
				HUINT32					ulIdx = i-pstBuilderState->ulSearchedSvcNum;

				HxSTD_MemSet(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
				hErr = MGR_SEARCHRESULT_GetService(ulActionId, eDxSVC_TYPE_All, i, &stSvcInfo);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("MGR_SEARCHRESULT_GetService error\n");
				}

				pstBuilderSvcInfo = &pstContext->pstTmpSvcInfo[ulIdx];
				pstBuilderSvcInfo->onId			= (HINT32)DbSvc_GetOnId(&stSvcInfo);
				pstBuilderSvcInfo->tsId			= (HINT32)DbSvc_GetTsId(&stSvcInfo);
				pstBuilderSvcInfo->svcId		= (HINT32)DbSvc_GetSvcId(&stSvcInfo);
				pstBuilderSvcInfo->ulSvcType 	= (HUINT32)DbSvc_GetSvcType(&stSvcInfo);
				pstBuilderSvcInfo->ulCasType 	= (HUINT32)DbSvc_GetCasType(&stSvcInfo);
				pstBuilderSvcInfo->ulLcn		= (HUINT32)DbSvc_GetLcn(&stSvcInfo);

				ulSvcNameLen = HxSTD_StrLen(DbSvc_GetName(&stSvcInfo)) + 1;
				if (ulSvcNameLen > sizeof(pstBuilderSvcInfo->szSvcName))
				{
					ulSvcNameLen = sizeof(pstBuilderSvcInfo->szSvcName);
				}
 				HLIB_STD_StrNCpySafe(pstBuilderSvcInfo->szSvcName, DbSvc_GetName(&stSvcInfo), ulSvcNameLen);

				HxLOG_Debug("idx(%d=%d,%d) nameLen(%d) lcn(%d) svc(%s) type(0x%x)\n"
					, ulIdx, pstBuilderState->ulSearchedSvcNum, i, ulSvcNameLen
					, pstBuilderSvcInfo->ulLcn
					, pstBuilderSvcInfo->szSvcName
					, pstBuilderSvcInfo->ulSvcType
					);
			}
		}

		pstBuilderState->ulSearchedSvcNum = ulNum;
	}

	pstBuilderState->ulSignalStrength = 0;
	pstBuilderState->ulSignalQuality = 0;

	HxSTD_MemSet(&stSignalInfo, 0x00, sizeof(CH_SignalInfo_t));
	hErr = SVC_CH_GetSignalInfo(ulActionId, &eDeliType, &stSignalInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Debug("SVC_CH_GetSignalInfo() error !! actionId(%d) deliType(%d) \n",	ulActionId, eDeliType);
#if defined (CONFIG_SUPPORT_WIND3_APP)
		// NO siginal 인 경우 UI 를 갱신할 시간을 준다.
		sleep(1);
#endif
	}

	hErr = SVC_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_TUNER_GROUP, &ulResourceId);
	if(hErr == ERR_OK)
	{
		hErr = SVC_CH_GetRequestedTuneInfo(ulResourceId, &stTuningInfo);
		if(hErr == ERR_OK)
		{
			if(stTuningInfo.eDeliType == eDxDELIVERY_TYPE_SAT)
			{
				/* web에서는 strength값으로(strength >= 10) locked상태를 파악하는데 unlock이 경우라도 strengh값이 조건에 부합할 가능성이 있어 lock될경우만 10을 setting, otherwise 0 setting by Moon */
				hErr = SVC_CH_GetLockedInfo(ulActionId, &stChInfo);
				if (hErr == ERR_OK)
				{
					pstBuilderState->ulSignalStrength = 10;
					stTuningInfo.sat.tuningInfo.ePolarization = stChInfo.sat.tuningInfo.ePolarization;
				}

				/* web에서 tracsSpec으로 DVBS와 DVBS2를 구분하지 않음, PskMod로 구분 pskMod = 0(DVBS), pskMod = 1(DVBS2-QPSK), pskMod = 2(DVBS2-8PSK) */
				if (stTuningInfo.sat.tuningInfo.eTransSpec == eDxSAT_TRANS_DVBS)
				{
					stTuningInfo.sat.tuningInfo.ePskMod = eDxSAT_PSK_AUTO;
				}

				pstBuilderState->stTuneParam.eDeliType = stTuningInfo.eDeliType;
				HxSTD_MemCopy(&pstBuilderState->stTuneParam.sat, &stTuningInfo.sat, sizeof(DxSatTuningInfo_t));

				/* web에서 관리하는 antenna id와 mw에서 관리하는 id 서로 달라서 web에서 관리하는 id로 변경처리 by Moon */
				MGR_SEARCHRESULT_GetAntennaId(ulActionId, &pstBuilderState->stTuneParam.sat.tuningInfo.antuid);

				pstBuilderState->ulFrequency = stTuningInfo.sat.tuningInfo.ulFrequency;
				pstBuilderState->eDeliveryType = stTuningInfo.eDeliType;
			}
			else if(stTuningInfo.eDeliType == eDxDELIVERY_TYPE_CAB)
			{
				HxSTD_MemCopy(&pstBuilderState->stTuneParam.cab, &stTuningInfo.cab, sizeof(DxCabTuningInfo_t));
				pstBuilderState->ulFrequency = stTuningInfo.cab.ulFrequency;
				pstBuilderState->eDeliveryType = stTuningInfo.eDeliType;

				hErr = SVC_CH_GetLockedInfo(ulActionId, &stChInfo);
				if (hErr == ERR_OK)
				{
					pstBuilderState->ulSignalStrength = stSignalInfo.signalLevel;
					pstBuilderState->ulSignalQuality = stSignalInfo.signalQuality;
				}
			}
			else if(stTuningInfo.eDeliType == eDxDELIVERY_TYPE_TER)
			{
				HxSTD_MemCopy(&pstBuilderState->stTuneParam.ter, &stTuningInfo.ter, sizeof(DxTerTuningInfo_t));
				pstBuilderState->ulFrequency = stTuningInfo.ter.ulFrequency;
				pstBuilderState->eDeliveryType = stTuningInfo.eDeliType;

				hErr = SVC_CH_GetLockedInfo(ulActionId, &stChInfo);
				if (hErr == ERR_OK)
				{
					pstBuilderState->ulSignalStrength = stSignalInfo.signalLevel;
					pstBuilderState->ulSignalQuality = stSignalInfo.signalQuality;

					pstBuilderState->stTuneParam.ter.ulFrequency = stChInfo.ter.ulFrequency;
					pstBuilderState->stTuneParam.ter.eBandWidth = stChInfo.ter.eBandWidth;
					pstBuilderState->stTuneParam.ter.eConstellation = stChInfo.ter.eConstellation;
					pstBuilderState->stTuneParam.ter.eHierachy = stChInfo.ter.eHierachy;
					pstBuilderState->stTuneParam.ter.eStream	 = stChInfo.ter.eStream;
					pstBuilderState->stTuneParam.ter.eCodeRate = stChInfo.ter.eCodeRate;
					pstBuilderState->stTuneParam.ter.eGuardInterval = stChInfo.ter.eGuardInterval;
					pstBuilderState->stTuneParam.ter.eTransMode = stChInfo.ter.eTransMode;
					pstBuilderState->stTuneParam.ter.eOffset = stChInfo.ter.eOffset;
#if defined(CONFIG_MW_CH_DVB_T2)
					pstBuilderState->stTuneParam.ter.eSystem = stChInfo.ter.eSystem;
#endif
				}
			}
		}
	}

	HxLOG_Debug("searched svc num : (%d) complete/total tp (%d/%d) : builderId(%d)\n",
					pstBuilderState->ulSearchedSvcNum, pstBuilderState->ulCompleteTuningNum, pstBuilderState->ulTotalTuningNum, pstContext->ulBuilderId);

	om_builder_CheckAndNotifyStateChanged(pstContext->ulBuilderId);

	return eResult;
}


BUS_Result_t om_builder_svcsrch_MsgSearchFinished (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	omBldContext_t			*pstContext;
	BUS_Result_t			eResult = MESSAGE_PASS;

	pstContext = om_builder_GetContextByActionId (ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulActionId(%d)\n", ulActionId);
		return eResult;
	}

	BUS_MGR_Destroy(pstContext->pfnSearchProc);

	HxLOG_Print(HxANSI_COLOR_YELLOW("call DB_SVC_SetNotifier(TRUE)\n"));
	DB_SVC_SetNotifier(TRUE);

	return eResult;
}

/* end of file */
