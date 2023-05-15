/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_builder_tune.c
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
#include <mgr_rscmap.h>
//#include <mwc_util.h>

#include "_om_builder.h"

#include <otl.h>

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
STATIC HERROR om_builder_tune_NotifyTuningStatusChanged (omBldContext_t *pstContext)
{
	DxDeliveryType_e	eDeliType;
	CH_SignalInfo_t	stSignalInfo;
	HERROR hErr=ERR_FAIL;

	if (pstContext == NULL)
	{
		return ERR_FAIL;
	}

	hErr=SVC_CH_GetSignalInfo(pstContext->ulActionId, &eDeliType, &stSignalInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Debug("SVC_CH_GetSignalInfo() error !! actionId(%d) deliType(%d) \n", pstContext->ulActionId, eDeliType);
	}

	HxSTD_MemSet(&pstContext->stBuilderState, 0x00, sizeof(OxBuilder_BuilderState_t));
	pstContext->stBuilderState.ulSignalStrength = stSignalInfo.signalLevel;
	pstContext->stBuilderState.ulSignalQuality = stSignalInfo.signalQuality;

	HxLOG_Debug("signal strength: %d, quality: %d \n", pstContext->stBuilderState.ulSignalStrength, pstContext->stBuilderState.ulSignalQuality);

	om_builder_CheckAndNotifyStateChanged(pstContext->ulBuilderId);

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR om_builder_tune_StartTune (HUINT32 ulBuilderId)
{
	HERROR				 hErr;
	omBldContext_t		*pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	DbSvc_TuningInfo_t	*pstSvcTuneInfo;
	DxTuneParam_t		 stTuneInfo;
#if defined(CONFIG_MW_CH_SATELLITE)
	DbSvc_AntInfo_t	 stAntInfo;
#endif
	SvcRsc_Info_t		 stRscInfo;
	Handle_t 			 hAction;

	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context found: ulBuildId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	if ((pstContext == NULL)
		|| (pstContext->stSearchOption.ulTpNum == 0))
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stTuneInfo, 0x00, sizeof(DxTuneParam_t));
	pstSvcTuneInfo = &pstContext->stSearchOption.pstTpArray[0].stTuningParam;

	switch (pstContext->stOption.eDeliveryType)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case	eDxDELIVERY_TYPE_SAT:
			hErr = DB_SVC_GetAntInfo(pstSvcTuneInfo->sat.ucAntId, &stAntInfo);
			if (ERR_OK == hErr)
			{
				MGR_SEARCHUTIL_ConvertSatSvcTuningInfoToChTuningParam((HUINT32)-1, &stAntInfo, pstSvcTuneInfo, &stTuneInfo);
			}
			break;
#endif

		case	eDxDELIVERY_TYPE_CAB:
		case	eDxDELIVERY_TYPE_TER:
			{
				DbSvc_TsInfo_t	stTsInfo;

				HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
				stTsInfo.eDeliType		= pstContext->stOption.eDeliveryType;
				stTsInfo.stTuningParam	= *pstSvcTuneInfo;

				OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stTuneInfo);
			}
			break;

		default:
			return ERR_FAIL;
	}

	hErr = MGR_ACTION_SetupSearch(pstContext->ulActionId, &stTuneInfo, eAmPriority_SEARCH);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("MGR_ACTION_SetupSearch() failed..  : ulActionId(%d)\n", pstContext->ulActionId);
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_Take(pstContext->ulActionId, eActionType_SEARCH, om_builder_BaseProc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("MGR_ACTION_Take() failed.. : ulActionId(%d)\n", pstContext->ulActionId);
		return ERR_FAIL;
	}

	hErr= MGR_RSC_GetResourceInfo(pstContext->ulActionId, &stRscInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("MGR_RSC_GetResourceInfo() failed..  : ulActionId(%d)\n", pstContext->ulActionId);
		return ERR_FAIL;
	}

	switch (pstContext->stOption.eDeliveryType)
	{
		case	eDxDELIVERY_TYPE_SAT:
			stTuneInfo.sat.ulTunerGroupId 	= stRscInfo.astRscItem[eRxRSCTYPE_TUNER_GROUP].ulResourceId;
			stTuneInfo.sat.ulRfInputId	   	= stRscInfo.astRscItem[eRxRSCTYPE_RFINPUT].ulResourceId;
			break;

		case	eDxDELIVERY_TYPE_CAB:
			stTuneInfo.cab.ulTunerGroupId 	= stRscInfo.astRscItem[eRxRSCTYPE_TUNER_GROUP].ulResourceId;
			break;

		case	eDxDELIVERY_TYPE_TER:
			stTuneInfo.ter.ulTunerGroupId 	= stRscInfo.astRscItem[eRxRSCTYPE_TUNER_GROUP].ulResourceId;
			break;

		default:
			HxLOG_Error("not supported yet!!!\n");
			break;
	}

	SVC_PIPE_IncreaseActionVersion(pstContext->ulActionId);
	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);

	hErr= SVC_CH_StopTune(hAction, (HUINT16)-1, eAsyncMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("SVC_CH_StopTune() failed..  : ulActionId(%d)\n", pstContext->ulActionId);
		return ERR_FAIL;
	}

	hErr= SVC_CH_StartTune(hAction, &stRscInfo, (HUINT16)-1, &stTuneInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("SVC_CH_StartTune() failed..  : ulActionId(%d)\n", pstContext->ulActionId);
		return ERR_FAIL;
	}

	pstContext->eState = eDxBUILDER_STATE_RUNNING;
	return ERR_OK;
}

HERROR om_builder_tune_StopTune (HUINT32 ulBuilderId)
{
	HERROR				hErr = ERR_FAIL; /* fix for prevent #150274 */
	omBldContext_t		*pstContext = om_builder_GetContextByBuilderId (ulBuilderId);

	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context found: ulBuildId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	hErr = SVC_CH_StopTune(SVC_PIPE_GetActionHandle(pstContext->ulActionId), (HUINT16)-1, eSyncMode);
	MGR_ACTION_Release(pstContext->ulActionId, eActionType_SEARCH);

	pstContext->eState = eDxBUILDER_STATE_STOPPED;
	return hErr;
}

BUS_Result_t om_builder_tune_ProcessTuneEvent (HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				 ulActionId;
	omBldContext_t			*pstContext;
	BUS_Result_t			 eResult = MESSAGE_PASS;

	if (lMessage == eMEVT_BUS_TIMER)
	{
		// timer msg에 달려오는 hAction은 항상 0이다. main action id로 넣어주자
		ulActionId = MGR_ACTION_GetMainActionId();
	}
	else
	{
		ulActionId = SVC_PIPE_GetActionId(hAction);
	}

	pstContext = om_builder_GetContextByActionId (ulActionId);
	if (NULL == pstContext)
	{
		// 이 로그가 찍히면 fcc나 기타 action이 actionId를 가지고 도는 경우임..!
		HxLOG_Debug ("No Context Exist: ulActionId(%d)\n", ulActionId);
		return eResult;
	}

	if (pstContext->stOption.eOptionType != eOxBUILDER_OPTIONTYPE_TUNEONLY)
	{
		HxLOG_Debug ("not allowed search type : eOptionType(%d)\n", pstContext->stOption.eOptionType);
		return eResult;
	}

	switch(lMessage)
	{
		case eSEVT_CH_LOCKED:
			om_builder_tune_NotifyTuningStatusChanged(pstContext);
			BUS_SetTimer(BUILDER_TUNE_TIMER_ID, BUILDER_TUNE_TIMER_TIME);
			break;

		case eSEVT_CH_NO_SIGNAL:
			HxSTD_MemSet(&pstContext->stBuilderState, 0x00, sizeof(OxBuilder_BuilderState_t));
			om_builder_CheckAndNotifyStateChanged(pstContext->ulBuilderId);
			BUS_KillTimer(BUILDER_TUNE_TIMER_ID);
			break;

		case eMEVT_BUS_TIMER:
			if (p1 == BUILDER_TUNE_TIMER_ID)
			{
				if (p2 == BUILDER_TUNE_TIMER_ID)
				{
					// kill timer
					BUS_KillTimer(BUILDER_TUNE_TIMER_ID);
				}
				else
				{
					// timer~
					om_builder_tune_NotifyTuningStatusChanged(pstContext);
				}

			}
			break;
	}
	return eResult;
}


/* end of file */
