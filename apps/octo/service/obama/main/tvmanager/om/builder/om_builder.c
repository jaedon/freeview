/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_builder.c
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
#include <svc_pipe.h>

#include <bus.h>
#include <otl.h>

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
STATIC HxList_t				*s_pstMgrBuilder_ContextList = NULL;			// omBldContext_t


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___
STATIC BUS_Result_t proc_om_builder (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);



#define _____CONTEXT_FUNCTION_____
STATIC omBldContext_t *om_builder_MakeContext (HUINT32 ulBuilderId)
{
	omBldContext_t		*pstContext;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = (omBldContext_t *)OxMGR_Calloc (sizeof(omBldContext_t));
	if (NULL != pstContext)
	{
		pstContext->ulBuilderId	= ulBuilderId;
		pstContext->ulActionId 	= MGR_ACTION_GetMainActionId();
		pstContext->eState		= eDxBUILDER_STATE_IDLE;
		pstContext->eLastState	= eDxBUILDER_STATE_NONE;
		pstContext->pstTmpSvcInfo = NULL;

		s_pstMgrBuilder_ContextList = HLIB_LIST_Append (s_pstMgrBuilder_ContextList, (void *)pstContext);
	}

	return pstContext;
}

STATIC HERROR om_builder_RemoveContext (omBldContext_t *pstContext)
{
	s_pstMgrBuilder_ContextList = HLIB_LIST_Remove (s_pstMgrBuilder_ContextList, pstContext);

	HxLOG_Debug("Enter.. \n");

	if (pstContext->stSearchOption.pstTpArray != NULL)
	{
		OxMGR_Free(pstContext->stSearchOption.pstTpArray);
		pstContext->stSearchOption.pstTpArray = NULL;
	}

	if (pstContext->pstTmpSvcInfo != NULL)
	{
		OxMGR_Free(pstContext->pstTmpSvcInfo);
		pstContext->pstTmpSvcInfo = NULL;
	}

	OxMGR_Free (pstContext);
	pstContext = NULL;

	return ERR_OK;
}

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void om_builder_CbOnActivated (void)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;
	HINT32					 lMyLongitude = 0, lMyLatitude = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() failed.. \n");
		return;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_MY_LONGITUDE, (HUINT32 *)&lMyLongitude, 0);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_PARAM_GetItem() failed..!! - MY_LONGITUDE \n");
		lMyLongitude = 0;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_MY_LATITUDE, (HUINT32 *)&lMyLatitude, 0);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_PARAM_GetItem() failed..!! - MY_LATITUDE \n");
		lMyLatitude = 0;
	}

	HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_BUILDER_Inited, "ii", lMyLongitude, lMyLatitude);

	return;
}

STATIC void om_builder_CbOnStateChanged (HUINT32 ulBuilderId, DxBuilder_State_e eState)
{
	HINT32					 	 nRpcHandle = 0;
	HERROR					 	 hErr = ERR_FAIL;
	omBldContext_t				*pstContext;
	DxBuilder_SearchedInfo_t 	 stDummy, *pstSendSvcList;
	HUINT32						 ulSvcInfoSize = 0;
	HUINT32						 ulSendSvcCount = 0;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("om_builder_GetContextByBuilderId() failed..: ulBuilderId(%d)\n", ulBuilderId);
		return;
	}

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() failed.. \n");
		return;
	}

	// svcCount가 0이면 dummy structure를 올린다
	if ((pstContext->ulSvcCount == 0) || (pstContext->pstTmpSvcInfo == NULL))
	{
		HxSTD_MemSet(&stDummy, 0x00, sizeof(DxBuilder_SearchedInfo_t));
		pstSendSvcList = &stDummy;
		ulSvcInfoSize = sizeof(DxBuilder_SearchedInfo_t);
		ulSendSvcCount = 0;
	}
	else
	{
		pstSendSvcList = pstContext->pstTmpSvcInfo;
		ulSvcInfoSize = sizeof(DxBuilder_SearchedInfo_t) * pstContext->ulSvcCount;
		ulSendSvcCount = pstContext->ulSvcCount;
	}

	HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_BUILDER_onStateChanged, "iibib",
						ulBuilderId, eState,
						&pstContext->stBuilderState, sizeof(OxBuilder_BuilderState_t),
						ulSendSvcCount,
						pstSendSvcList, ulSvcInfoSize);

	if (pstContext->pstTmpSvcInfo != NULL)
	{
		pstContext->ulSvcCount = 0;
		OxMGR_Free(pstContext->pstTmpSvcInfo);
		pstContext->pstTmpSvcInfo = NULL;
	}

	return;
}

STATIC BUS_Result_t om_builder_CbOnChannelListUpdated(void)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_BUS_NO_OBJECT;
	}
	
	HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_META_SERVICE_OnUpdate, "s", "on_update");

	return ERR_BUS_NO_ERROR;
}



#define _____OAPI_COMMAND_FUNCTIONS_____
STATIC HERROR om_builder_Init (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	om_builder_CbOnActivated();

	return ERR_OK;
}

STATIC HERROR om_builder_Open (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL != pstContext)
	{
		HxLOG_Error ("Already Builder Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	pstContext = om_builder_MakeContext (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("om_builder_MakeContext err: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	(void)om_builder_CheckAndNotifyStateChanged (ulBuilderId);

	return ERR_OK;
}

STATIC HERROR om_builder_Close (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	// Clear the information and the action if it remains:
	pstContext->eState 		= eDxBUILDER_STATE_NONE;
	pstContext->eLastState	= eDxBUILDER_STATE_ENDOFCASE;

	// 이미 oapi builder가 close됐기 때문에 올려도 소용없다
	//(void)om_builder_CheckAndNotifyStateChanged (ulBuilderId);

	// Remove the context:
	(void)om_builder_RemoveContext (pstContext);
	return ERR_OK;
}

STATIC HERROR om_builder_AddBuildInfo (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulCount;
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	HUINT32					 ulInfoNum = (HUINT32)HxOBJECT_INT (argv[1]);
	DxBuilder_BuildInfo_t	*pstInfoArray = (DxBuilder_BuildInfo_t *)HxOBJECT_BIN_DATA (argv[2]);
	omBldContext_t			*pstContext;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	if ((0 == ulInfoNum) || (NULL == pstInfoArray))
	{
		HxLOG_Error ("No Build Info:\n");
		return ERR_FAIL;
	}

	pstContext->stSearchOption.ulTpNum = ulInfoNum;
	pstContext->stSearchOption.pstTpArray = (MgrSearch_TpInfo_t*)OxMGR_Calloc(sizeof(MgrSearch_TpInfo_t) * ulInfoNum);
	if (pstContext->stSearchOption.pstTpArray == NULL)
	{

		HxLOG_Error ("OxMGR_Calloc failed.. : ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	for (ulCount = 0; ulCount < ulInfoNum; ulCount++)
	{
		MgrSearch_TpInfo_t			*pTpInfo = &(pstContext->stSearchOption.pstTpArray[ulCount]);
		DxBuilder_BuildInfo_t		*pstSrcInfo = &(pstInfoArray[ulCount]);

		if (pstSrcInfo != NULL)
		{
			pTpInfo->eDeliType = pstSrcInfo->unArgs.stTuneParam.eDeliType;
			pTpInfo->eTpType   = 0;

			switch(pTpInfo->eDeliType)
			{
				case eDxDELIVERY_TYPE_SAT:
					pTpInfo->stTuningParam.sat.ulFrequency		= pstSrcInfo->unArgs.stTuneParam.sat.tuningInfo.ulFrequency;
					pTpInfo->stTuningParam.sat.ulSymbolRate		= pstSrcInfo->unArgs.stTuneParam.sat.tuningInfo.ulSymbolRate;
					pTpInfo->stTuningParam.sat.ePolarization	= pstSrcInfo->unArgs.stTuneParam.sat.tuningInfo.ePolarization;
					pTpInfo->stTuningParam.sat.eFecCodeRate		= pstSrcInfo->unArgs.stTuneParam.sat.tuningInfo.eFecCodeRate;
					pTpInfo->stTuningParam.sat.eTransSpec		= pstSrcInfo->unArgs.stTuneParam.sat.tuningInfo.eTransSpec;
					pTpInfo->stTuningParam.sat.ePskMod			= pstSrcInfo->unArgs.stTuneParam.sat.tuningInfo.ePskMod;
					pTpInfo->stTuningParam.sat.ePilot			= pstSrcInfo->unArgs.stTuneParam.sat.tuningInfo.ePilot;
					pTpInfo->stTuningParam.sat.eRollOff			= pstSrcInfo->unArgs.stTuneParam.sat.tuningInfo.eRollOff;
					pTpInfo->stTuningParam.sat.ucAntId			= (HUINT8)pstSrcInfo->unArgs.stTuneParam.sat.tuningInfo.antuid;
					pTpInfo->stTuningParam.sat.eSatType			= OTL_CONV_SatString2SVC_SatType(pstSrcInfo->unArgs.stTuneParam.sat.tuningInfo.satType);
					break;

				case eDxDELIVERY_TYPE_CAB:
					pTpInfo->stTuningParam.cab.ulFreq 			= pstSrcInfo->unArgs.stTuneParam.cab.ulFrequency;
					pTpInfo->stTuningParam.cab.usSR 			= pstSrcInfo->unArgs.stTuneParam.cab.ulSymbolRate;
					pTpInfo->stTuningParam.cab.usNetworkId 		= pstSrcInfo->unArgs.stTuneParam.cab.nNetId;
					pTpInfo->stTuningParam.cab.ucCon 			= pstSrcInfo->unArgs.stTuneParam.cab.constellation;
					pTpInfo->stTuningParam.cab.ucSpec 			= pstSrcInfo->unArgs.stTuneParam.cab.spectrum;
					break;

				case eDxDELIVERY_TYPE_TER:
					pTpInfo->stTuningParam.ter.ulFreq 			= pstSrcInfo->unArgs.stTuneParam.ter.ulFrequency;
					pTpInfo->stTuningParam.ter.ucBandwidth 		= pstSrcInfo->unArgs.stTuneParam.ter.eBandWidth;
					pTpInfo->stTuningParam.ter.ucConstellation 	= pstSrcInfo->unArgs.stTuneParam.ter.eConstellation;
					pTpInfo->stTuningParam.ter.ucHierachy 		= pstSrcInfo->unArgs.stTuneParam.ter.eHierachy;
					pTpInfo->stTuningParam.ter.ucStream 		= pstSrcInfo->unArgs.stTuneParam.ter.eStream;
					pTpInfo->stTuningParam.ter.ucCodeRate 		= pstSrcInfo->unArgs.stTuneParam.ter.eCodeRate;
					pTpInfo->stTuningParam.ter.ucGuardInterval 	= pstSrcInfo->unArgs.stTuneParam.ter.eGuardInterval;
					pTpInfo->stTuningParam.ter.ucTransMode 		= pstSrcInfo->unArgs.stTuneParam.ter.eTransMode;
					pTpInfo->stTuningParam.ter.ucOffset 			= pstSrcInfo->unArgs.stTuneParam.ter.eOffset;
#if defined(CONFIG_MW_CH_DVB_T2)
					pTpInfo->stTuningParam.ter.ucSystem 		= pstSrcInfo->unArgs.stTuneParam.ter.eSystem;
					pTpInfo->stTuningParam.ter.stT2param.ulPlpId			= pstSrcInfo->unArgs.stTuneParam.ter.stT2Param.ulPlpId;
					pTpInfo->stTuningParam.ter.stT2param.ucPreambleFormat	= pstSrcInfo->unArgs.stTuneParam.ter.stT2Param.preambleFormat;
					pTpInfo->stTuningParam.ter.stT2param.ucMixed			= pstSrcInfo->unArgs.stTuneParam.ter.stT2Param.mixed;
					pTpInfo->stTuningParam.ter.stT2param.ucPilotPattern	= pstSrcInfo->unArgs.stTuneParam.ter.stT2Param.pilotPattern;
					pTpInfo->stTuningParam.ter.stT2param.ucExtenedCarrier	= pstSrcInfo->unArgs.stTuneParam.ter.stT2Param.extenedCarrier;
					pTpInfo->stTuningParam.ter.stT2param.ucPAPRreduction	= pstSrcInfo->unArgs.stTuneParam.ter.stT2Param.PAPRreduction;
					pTpInfo->stTuningParam.ter.stT2param.ulNumPlpBlocks	= pstSrcInfo->unArgs.stTuneParam.ter.stT2Param.numPlpBlocks;
					//pTpInfo->stTuningParam.ter.stT2param 		= pstSrcInfo->unArgs.stTuneParam.ter.stT2Param; // ???
#endif
					break;

				default:
					// not support now.
					HxLOG_Error ("NOT supported delivery type (%d)\n", pTpInfo->eDeliType);
					break;
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR om_builder_SetOption (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	OxBuilder_Option_t		*pstOption = (OxBuilder_Option_t *)HxOBJECT_BIN_DATA (argv[1]);
	omBldContext_t			*pstContext;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if ((NULL == pstContext) || (NULL == pstOption))
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	HxSTD_MemCopy (&(pstContext->stOption), pstOption, sizeof(OxBuilder_Option_t));

	pstContext->stSearchOption.eCasType = pstContext->stOption.stSvcSrchOpt.eCasType;
	pstContext->stSearchOption.eSvcType = pstContext->stOption.stSvcSrchOpt.eSvcType;
	pstContext->stSearchOption.bNetworkSearch = pstContext->stOption.stSvcSrchOpt.bNetworkSearch;
	pstContext->stSearchOption.eSearchType = eSVCSI_SEARCHTYPE_Normal;
	pstContext->stSearchOption.bReplaceChannel = pstContext->stOption.stSvcSrchOpt.bReplaceChannel;
	if (pstContext->stOption.stSvcSrchOpt.bWithNetworkId)
	{
		pstContext->stSearchOption.usNetwork_id = pstContext->stOption.stSvcSrchOpt.ulNetworkId;
	}

	return ERR_OK;
}

STATIC HERROR om_builder_Start (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	if ((eDxBUILDER_STATE_RUNNING == pstContext->eState) ||
		(eDxBUILDER_STATE_FINISHED == pstContext->eState))
	{
		HxLOG_Error ("Current State (%d) not allowed to start\n", pstContext->eState);
		return ERR_FAIL;
	}

	HxLOG_Debug("Enter.. ulBuilderId(%d) eOptionType(%d) \n", ulBuilderId, pstContext->stOption.eOptionType);

	switch (pstContext->stOption.eOptionType)
	{
	case eOxBUILDER_OPTIONTYPE_SVCSEARCH:
		(void)om_builder_svcsrch_StartSearch (ulBuilderId);
		break;

	case eOxBUILDER_OPTIONTYPE_OP_TP_LIST_SEARCH:
		(void)om_builder_optplistsrch_StartSearch (ulBuilderId);
		break;

	case eOxBUILDER_OPTIONTYPE_TUNEONLY:
		(void)om_builder_tune_StartTune (ulBuilderId);
		break;

#if defined(CONFIG_MW_CH_SATELLITE)
	case eOxBUILDER_OPTIONTYPE_ANTCONNECT:
		(void)om_builder_antconn_StartSearch (ulBuilderId);
		break;

	case eOxBUILDER_OPTIONTYPE_DISEQCDETECT:
		(void)om_builder_diseqc_StartDetection (ulBuilderId);
		break;
#endif

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	case eOxBUILDER_OPTIONTYPE_CIPLUS_SEARCH:
		(void)om_builder_ciplussrch_StartSearch (ulBuilderId);
		break;
#endif


#if defined(CONFIG_OP_ERTELECOM)
	case eOxBUILDER_OPTIONTYPE_ERTELECOM_SEARCH:
		(void)om_builder_ertsrch_StartSearch (ulBuilderId);
		break;
#endif
	default:
		HxLOG_Error ("OptionType(%d) not implemented:\n", pstContext->stOption.eOptionType);
		return ERR_FAIL;
	}

	(void)om_builder_CheckAndNotifyStateChanged (ulBuilderId);
	return ERR_OK;
}

STATIC HERROR om_builder_Stop (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	if (eDxBUILDER_STATE_IDLE == pstContext->eState)
	{
		HxLOG_Error ("Current State (%d) not allowed to Stop\n", pstContext->eState);
		return ERR_FAIL;
	}

	switch (pstContext->stOption.eOptionType)
	{
	case eOxBUILDER_OPTIONTYPE_SVCSEARCH:
		(void)om_builder_svcsrch_StopSearch (ulBuilderId);
		break;

	case eOxBUILDER_OPTIONTYPE_OP_TP_LIST_SEARCH:
		(void)om_builder_optplistsrch_StopSearch (ulBuilderId);
		break;

	case eOxBUILDER_OPTIONTYPE_TUNEONLY:
		(void)om_builder_tune_StopTune (ulBuilderId);
		break;

#if defined(CONFIG_MW_CH_SATELLITE)
	case eOxBUILDER_OPTIONTYPE_ANTCONNECT:
		(void)om_builder_antconn_StopSearch (ulBuilderId);
		break;

	case eOxBUILDER_OPTIONTYPE_DISEQCDETECT:
		(void)om_builder_diseqc_StopDetection (ulBuilderId);
		break;
#endif

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	case eOxBUILDER_OPTIONTYPE_CIPLUS_SEARCH:
		(void)om_builder_ciplussrch_StopSearch (ulBuilderId);
		break;
#endif


#if defined(CONFIG_OP_ERTELECOM)
	case eOxBUILDER_OPTIONTYPE_ERTELECOM_SEARCH:
		(void)om_builder_ertsrch_StopSearch (ulBuilderId);
		break;
#endif
	default:
		HxLOG_Error ("OptionType(%d) not implemented:\n", pstContext->stOption.eOptionType);
		return ERR_FAIL;
	}

	// kill timer를 bus에서 하기 위해 msg를 보내 처리한다
	// 새로 msg 선언하지 않고 기존 timer msg 재사용을 하자.. timer timeout과 구분을 위해 p2에도 timer id를 넣어 보낸다
	BUS_SendMessage(proc_om_builder, eMEVT_BUS_TIMER, 0, BUILDER_TUNE_TIMER_ID, BUILDER_TUNE_TIMER_ID, 0);

	// stop 이 불린뒤엔 바로 close가 불리므로 여기서 올려봐야 안 올라간다
	//(void)om_builder_CheckAndNotifyStateChanged (ulBuilderId);

	return ERR_OK;
}

STATIC HERROR om_builder_Pause (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	BUS_SendMessage(pstContext->pfnSearchProc, eMEVT_SEARCH_CANCEL, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
	pstContext->eState = eDxBUILDER_STATE_PAUSED;

	return ERR_OK;
}

STATIC HERROR om_builder_Resume (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	BUS_SendMessage(pstContext->pfnSearchProc, eMEVT_SEARCH_ACTION_START, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
	pstContext->eState = eDxBUILDER_STATE_RUNNING;

	return ERR_OK;
}

STATIC HERROR om_builder_Finish (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;

	HxLOG_Debug("Enter.. ulBuilderId(%d) \n", ulBuilderId);

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	BUS_SendMessage(pstContext->pfnSearchProc, eMEVT_SEARCH_SAVE_RESULT, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorGotoPosition (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	HINT32					 lPosition = (HINT32)HxOBJECT_INT (argv[1]);
	omBldContext_t			*pstContext;
	Handle_t				 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_MOTOR_GotoPosition(hAction, lPosition, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorStorePosition (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	HINT32					 lPosition = (HINT32)HxOBJECT_INT (argv[1]);
	omBldContext_t			*pstContext;
	Handle_t				 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_MOTOR_StorePosition(hAction, lPosition, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorResetPosition (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;
	Handle_t				 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_ResetDiseqc(hAction, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorRecalculate (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	HINT32					 lPosition = (HINT32)HxOBJECT_INT (argv[1]);
	omBldContext_t			*pstContext;
	Handle_t				 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_MOTOR_Recalculate(hAction, lPosition, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorMove (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 	 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	DxSat_MOTOR_DIRECTION_e 	 eDirection = (DxSat_MOTOR_DIRECTION_e)HxOBJECT_INT (argv[1]);
	DxSat_MOTOR_DRIVEMODE_e 	 eDriveMode = (DxSat_MOTOR_DRIVEMODE_e)HxOBJECT_INT (argv[2]);
	HINT32					 	lStep = (HINT32)HxOBJECT_INT (argv[3]);
	omBldContext_t				*pstContext;
	Handle_t					 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	// dlib type으로 통일 필요
	// CH_MotorDirection_t 	== DxSat_MOTOR_DIRECTION_e
	// CH_MotorDriveMode_t	== DxSat_MOTOR_DRIVEMODE_e
	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_MOTOR_Drive(hAction, (CH_MotorDirection_t)eDirection, (CH_MotorDriveMode_t)eDriveMode, lStep, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorStop (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;
	Handle_t				 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_MOTOR_StopMotor(hAction, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorSetLimit (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	DxSat_MOTOR_DIRECTION_e  eDirection = (DxSat_MOTOR_DIRECTION_e)HxOBJECT_INT (argv[1]);
	omBldContext_t			*pstContext;
	Handle_t				 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	// dlib type으로 통일 필요
	// CH_MotorDirection_t 	== DxSat_MOTOR_DIRECTION_e
	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_MOTOR_SetLimit(hAction, (CH_MotorDirection_t)eDirection, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorEnableLimit (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;
	Handle_t				 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_MOTOR_EnableLimit(hAction, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorDisableLimit (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	omBldContext_t			*pstContext;
	Handle_t				 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_MOTOR_DisableLimit(hAction, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorGotoDegree (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	HINT32 					 lDegree = (HINT32)HxOBJECT_INT (argv[1]);
	omBldContext_t			*pstContext;
	Handle_t				 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_MOTOR_GotoDegree(hAction, lDegree, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorGotoSatellite (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	HINT32 					 lSatellite = (HINT32)HxOBJECT_INT (argv[1]);
	omBldContext_t			*pstContext;
	Handle_t				 hAction;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	SVC_CH_MOTOR_GotoSatellite(hAction, lSatellite, eAsyncMode);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorSetMyLongitude (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulNewLongitude = (HUINT32)HxOBJECT_INT (argv[0]);

	DB_PARAM_SetItem(eDB_PARAM_ITEM_MY_LONGITUDE, ulNewLongitude, (HUINT32)0);

	return ERR_OK;
}

STATIC HERROR om_builder_MortorSetMyLatitude (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulNewLatitude = (HUINT32)HxOBJECT_INT (argv[0]);

	DB_PARAM_SetItem(eDB_PARAM_ITEM_MY_LATITUDE, ulNewLatitude, (HUINT32)0);

	return ERR_OK;
}

STATIC HERROR om_builder_SetSelectedConflictLcn (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulBuilderId = (HUINT32)HxOBJECT_INT (argv[0]);
	HUINT32					 ulNum = (HUINT32)HxOBJECT_INT (argv[1]);
	DxBuilder_SearchedInfo_t	*pstInfoArray = (DxBuilder_SearchedInfo_t *)HxOBJECT_BIN_DATA (argv[2]);
	omBldContext_t			*pstContext;
	Handle_t hAction;
	HERROR hErr;

	pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulBuilderId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}
	// TODO: improve & correct this code !!!
	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);

	hErr = SVC_SI_SetSelectedConflictLcn(hAction, ulNum, pstInfoArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	return hErr;
}

#define _____MSG_PROCESS_FUNCTIONS_____
STATIC BUS_Result_t om_builder_MsgBusCreate (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_builder_MsgBusDestroy (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_builder_MsgSearchStatusChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	omBldContext_t			*pstContext;
	BUS_Result_t			 eResult = MESSAGE_PASS;

	pstContext = om_builder_GetContextByActionId (ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Debug("No Context Exist: ulActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	HxLOG_Print("pstContext->stOption.eOptionType = %d\n", pstContext->stOption.eOptionType);
	switch (pstContext->stOption.eOptionType)
	{
	case eOxBUILDER_OPTIONTYPE_SVCSEARCH:
		eResult = om_builder_svcsrch_MsgSearchStatusChanged (hAction, p1, p2, p3);
		break;

	case eOxBUILDER_OPTIONTYPE_OP_TP_LIST_SEARCH:
		eResult = om_builder_optplistsrch_MsgSearchStatusChanged (hAction, p1, p2, p3);
		break;

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	case eOxBUILDER_OPTIONTYPE_CIPLUS_SEARCH:
		eResult = om_builder_ciplussrch_MsgSearchStatusChanged (hAction, p1, p2, p3);
		break;
#endif

#if defined(CONFIG_OP_ERTELECOM)
	case eOxBUILDER_OPTIONTYPE_ERTELECOM_SEARCH:
		eResult = om_builder_ertsrch_MsgSearchStatusChanged (hAction, p1, p2, p3);
		break;
#endif
	default:
		HxLOG_Error ("OptionType(%d) not implemented:\n", pstContext->stOption.eOptionType);
		return ERR_FAIL;
	}

	return eResult;
}

STATIC BUS_Result_t om_builder_MsgSearchFinished (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				 ulActionId = SVC_PIPE_GetActionId(hAction);
	omBldContext_t			*pstContext;
	BUS_Result_t			 eResult = MESSAGE_PASS;

	pstContext = om_builder_GetContextByActionId (ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Debug ("No Context Exist: ulActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	switch (pstContext->stOption.eOptionType)
	{
	case eOxBUILDER_OPTIONTYPE_SVCSEARCH:
		eResult = om_builder_svcsrch_MsgSearchFinished(hAction, p1, p2, p3);
		break;

	case eOxBUILDER_OPTIONTYPE_OP_TP_LIST_SEARCH:
		eResult = om_builder_optplistsrch_MsgSearchFinished(hAction, p1, p2, p3);
		break;

#if defined(CONFIG_MW_CH_SATELLITE)
	case eOxBUILDER_OPTIONTYPE_DISEQCDETECT:
		eResult = om_builder_diseqc_MsgSearchFinished(hAction, p1, p2, p3);
		break;
#endif

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	case eOxBUILDER_OPTIONTYPE_CIPLUS_SEARCH:
		eResult = om_builder_ciplussrch_MsgSearchFinished(hAction, p1, p2, p3);
		break;
#endif

#if defined(CONFIG_OP_ERTELECOM)
	case eOxBUILDER_OPTIONTYPE_ERTELECOM_SEARCH:
		eResult = om_builder_ertsrch_MsgSearchFinished(hAction, p1, p2, p3);
		break;
#endif
	default:
		HxLOG_Error ("OptionType(%d) not implemented:\n", pstContext->stOption.eOptionType);
		return ERR_FAIL;
	}

	return eResult;
}

STATIC BUS_Result_t om_builder_MsgSearchActionDestroyed (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				 ulActionId = SVC_PIPE_GetActionId(hAction);
	omBldContext_t			*pstContext;
	BUS_Result_t			 eResult = MESSAGE_PASS;

	pstContext = om_builder_GetContextByActionId (ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Debug ("No Context Exist: ulActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	switch (pstContext->stOption.eOptionType)
	{
	case eOxBUILDER_OPTIONTYPE_SVCSEARCH:
	case eOxBUILDER_OPTIONTYPE_OP_TP_LIST_SEARCH:
		pstContext->eState = eDxBUILDER_STATE_FINISHED;
		om_builder_CheckAndNotifyStateChanged(pstContext->ulBuilderId);
		break;

#if defined(CONFIG_MW_CH_SATELLITE)
	case eOxBUILDER_OPTIONTYPE_DISEQCDETECT:
		pstContext->eState = eDxBUILDER_STATE_FINISHED;
		om_builder_CheckAndNotifyStateChanged(pstContext->ulBuilderId);
		break;
#endif

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	case eOxBUILDER_OPTIONTYPE_CIPLUS_SEARCH:
		pstContext->eState = eDxBUILDER_STATE_FINISHED;
		om_builder_CheckAndNotifyStateChanged(pstContext->ulBuilderId);
		break;
#endif

#if defined(CONFIG_OP_ERTELECOM)
	case eOxBUILDER_OPTIONTYPE_ERTELECOM_SEARCH:
		pstContext->eState = eDxBUILDER_STATE_FINISHED;
		om_builder_CheckAndNotifyStateChanged(pstContext->ulBuilderId);
		break;
#endif
	default:
		HxLOG_Error ("OptionType(%d) not implemented:\n", pstContext->stOption.eOptionType);
		return ERR_FAIL;
	}

	return eResult;
}

#define _____RPC_INITIALIZATION_____
STATIC HERROR om_builder_InitRpcFunctions (void)
{
	HINT32			nRpcHandle = 0;
	HERROR			hErr = ERR_FAIL;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}
	//   ----------------------------- //
	//	-------   RPC Methods   ---------//
	//   ----------------------------- //

	// void builder.activate (void)
	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_Init, NULL, (HxRPC_Func_t)om_builder_Init, NULL,
							"Activate the builder module: (void)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_Open, "i", (HxRPC_Func_t)om_builder_Open, NULL,
							"Open the builder session: (int nBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_Close, "i", (HxRPC_Func_t)om_builder_Close, NULL,
							"Close the builder session: (int nBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_AddBuildInfo, "iib", (HxRPC_Func_t)om_builder_AddBuildInfo, NULL,
							"Add the Build Info(TP,etc): (int nBuilderId, int nBuildInfoNum, DxBuilder_BuildInfo_t *pstBuildInfoArray)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_SetOption, "ib", (HxRPC_Func_t)om_builder_SetOption, NULL,
							"Set the Build Option: (int nBuilderId, OxBuilder_Option_t *pstOption)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_Start, "i", (HxRPC_Func_t)om_builder_Start, NULL,
							"Start the builder action: (int nBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_Stop, "i", (HxRPC_Func_t)om_builder_Stop, NULL,
							"Stop the builder action: (int nBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_Pause, "i", (HxRPC_Func_t)om_builder_Pause, NULL,
							"Pause the builder action: (int nBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_Resume, "i", (HxRPC_Func_t)om_builder_Resume, NULL,
							"Resume the builder action: (int nBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_Finish, "i", (HxRPC_Func_t)om_builder_Finish, NULL,
							"Resume the builder action: (int nBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorGotoPosition, "ii", (HxRPC_Func_t)om_builder_MortorGotoPosition, NULL,
							"Motor goto position : (HUINT32 ulBuilderId, HINT32 lPosition)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorStorePosition, "ii", (HxRPC_Func_t)om_builder_MortorStorePosition, NULL,
							"Motor store position : (HUINT32 ulBuilderId, HINT32 lPosition)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorResetPosition, "i", (HxRPC_Func_t)om_builder_MortorResetPosition, NULL,
							"Motor reset position : (HUINT32 ulBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorRecalculate, "ii", (HxRPC_Func_t)om_builder_MortorRecalculate, NULL,
							"Motor recalculate : (HUINT32 ulBuilderId, HINT32 lPosition)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorMove, "iiii", (HxRPC_Func_t)om_builder_MortorMove, NULL,
							"Motor move : (HUINT32 ulBuilderId, DxSat_MOTOR_DIRECTION_e eDirection, DxSat_MOTOR_DRIVEMODE_e eDriveMode, HINT32 lStep)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorStop, "i", (HxRPC_Func_t)om_builder_MortorStop, NULL,
							"Motor stop : (int nBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorSetLimit, "ii", (HxRPC_Func_t)om_builder_MortorSetLimit, NULL,
							"Motor set limit : (HUINT32 ulBuilderId, DxSat_MOTOR_DIRECTION_e eDirection)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorEnableLimit, "i", (HxRPC_Func_t)om_builder_MortorEnableLimit, NULL,
							"Motor enable limit : (HUINT32 ulBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorDisableLimit, "i", (HxRPC_Func_t)om_builder_MortorDisableLimit, NULL,
							"Motor disable limit : (HUINT32 ulBuilderId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorGotoDegree, "ii", (HxRPC_Func_t)om_builder_MortorGotoDegree, NULL,
							"Motor goto degree : (HUINT32 ulBuilderId, HINT32 lDegree)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorGotoSatellite, "ii", (HxRPC_Func_t)om_builder_MortorGotoSatellite, NULL,
							"Motor goto satellite : (HUINT32 ulBuilderId, HINT32 lSatellite)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorSetMyLongitude, "i", (HxRPC_Func_t)om_builder_MortorSetMyLongitude, NULL,
							"Motor set my longitude : (HINT32 lNewLongitude)");


	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_MotorSetMyLatitude, "i", (HxRPC_Func_t)om_builder_MortorSetMyLatitude, NULL,
							"Motor set my latitude : (HINT32 lNewLatitude)");


	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_BUILDER_SetSelectedConflictLcn, "iib", (HxRPC_Func_t)om_builder_SetSelectedConflictLcn, NULL
		   , "Set selected conflict lcn : HUINT32 ulBuilderId, HUINT32 ulCount, DxBuilder_SearchedInfo_t *pstSvcInfoList)");

	//   ----------------------------- //
	//	-------   Notifier  Callbakcs ------ //
	//   ----------------------------- //

	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_BUILDER_Inited, "ii",
		"builder Activated.\n"
		"\t   - (void)\n"
		);

	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_BUILDER_onStateChanged, "iibib",
		"builder State Changed.\n"
		"\t   - (int nBuilderId, DxBuilder_State_e eState, OxBuilder_BuilderState_t *pstBuilderState)\n"
		);

	return ERR_OK;
}

#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_builder (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Debug("omBuilder : eMEVT_BUS_CREATE...\n");
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
		eResult = om_builder_MsgBusCreate (hHandle, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Debug("omBuilder : eMEVT_BUS_DESTROY...\n");
		eResult = om_builder_MsgBusDestroy (hHandle, p1, p2, p3);
		break;

	case eMEVT_SEARCH_STATUS_CHANGED:
		HxLOG_Debug("omBuilder : eMEVT_SEARCH_STATUS_CHANGED...\n");
		eResult = om_builder_MsgSearchStatusChanged (hHandle, p1, p2, p3);
		break;

	case eMEVT_SEARCH_FINISHED:
		HxLOG_Debug("omBuilder : eMEVT_SEARCH_FINISHED...\n");
		eResult = om_builder_MsgSearchFinished(hHandle, p1, p2, p3);
		break;

	case eMEVT_SEARCH_ACTION_DESTROYED:
		HxLOG_Debug("omBuilder : eMEVT_SEARCH_ACTION_DESTROYED...\n");
		eResult = om_builder_MsgSearchActionDestroyed(hHandle, p1, p2, p3);
		break;

	case eSEVT_CH_MOTOR_CMD_COMPLETED:
		HxLOG_Debug("omBuilder : eSEVT_CH_MOTOR_CMD_COMPLETED...\n");
		break;

#if defined(CONFIG_MW_CH_SATELLITE)
	case eMEVT_SEARCH_ANT_CONNECT_TYPE_DETECTED:
		HxLOG_Debug("omBuilder : eMEVT_SEARCH_ANT_CONNECT_TYPE_DETECTED...\n");
		eResult = om_builder_antconn_MsgSearchAntConnectTypeDetected(hHandle, p1, p2, p3);
		break;
#endif

	/* 현재 lock, no signal, timer를 사용하는 search type은 tune only 뿐이므로 이렇게 구현한다
	  향후 다른 search type에서도 이런 evt를 처리할 필요가 생긴다면 om_builder_MsgSearchFinished() 등을 참고하자 */
	case eSEVT_CH_LOCKED:
		HxLOG_Debug("omBuilder : eSEVT_CH_LOCKED...\n");
		eResult = om_builder_tune_ProcessTuneEvent(message, hHandle, p1, p2, p3);
		break;

	case eSEVT_CH_NO_SIGNAL:
		HxLOG_Debug("omBuilder : eSEVT_CH_NO_SIGNAL...\n");
		eResult = om_builder_tune_ProcessTuneEvent(message, hHandle, p1, p2, p3);
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Debug("omBuilder : eMEVT_BUS_TIMER... timerId(0x%X)\n", p1);
		eResult = om_builder_tune_ProcessTuneEvent(message, hHandle, p1, p2, p3);
		break;

	case eMEVT_SEARCH_CHANNEL_UPDATED:
		eResult = om_builder_CbOnChannelListUpdated();
		break;

	default:
		eResult = MESSAGE_PASS;
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define ___MEMBER_FUNCTIONS___
omBldContext_t *om_builder_GetContextByBuilderId (HUINT32 ulBuilderId)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstMgrBuilder_ContextList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		omBldContext_t		*pstContext = (omBldContext_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstContext) &&
			(pstContext->ulBuilderId == ulBuilderId))
		{
			return pstContext;
		}
	}

	return NULL;
}

omBldContext_t *om_builder_GetContextByActionId (HUINT32 ulActionId)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstMgrBuilder_ContextList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		omBldContext_t		*pstContext = (omBldContext_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstContext) &&
			(pstContext->ulActionId == ulActionId))
		{
			return pstContext;
		}
	}


#if defined(CONFIG_DEBUG)
	// builder running 상태가 아니라면 실패할 수 있음..
	if (HLIB_LIST_Length(s_pstMgrBuilder_ContextList) > 0)
	{
		// action id로 context를 못 가져옴.. log 찍자
		HxLOG_Error("Can't found context by action Id !! - ulActionId(%d) curMainActionId(%d) \n", ulActionId, MGR_ACTION_GetMainActionId());
		for (pstListItem = s_pstMgrBuilder_ContextList; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			omBldContext_t		*pstContext = (omBldContext_t *)HLIB_LIST_Data (pstListItem);

			if (NULL != pstContext)
			{
				HxLOG_Error("builderId(%d) actionId(%d) eState(%d) \n", pstContext->ulBuilderId, pstContext->ulActionId, pstContext->eState);
			}
		}
	}
#endif

	return NULL;
}


HERROR om_builder_CheckAndNotifyStateChanged (HUINT32 ulBuilderId)
{
	omBldContext_t			*pstContext = om_builder_GetContextByBuilderId (ulBuilderId);

	if (NULL == pstContext)
	{
		HxLOG_Debug("pstContext is NULL \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("eState: %d, eLastState: %d \n", pstContext->eState, pstContext->eLastState);
	if ((pstContext->eState == pstContext->eLastState)
		&& (pstContext->eState != eDxBUILDER_STATE_RUNNING))
	{
		return ERR_FAIL;
	}

	om_builder_CbOnStateChanged (ulBuilderId, pstContext->eState);
	pstContext->eLastState = pstContext->eState;

	return ERR_OK;
}

// om_builder member 간에만 사용해야 함
BUS_Result_t om_builder_BaseProc (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return proc_om_builder(message, hHandle, p1, p2, p3);
}

#define ___GLOBAL_FUNCTIONS___
HERROR OM_BUILDER_Init (void)
{
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_builder, HANDLE_NULL, 0, 0, 0);

	om_builder_InitRpcFunctions();

	return ERR_OK;
}



