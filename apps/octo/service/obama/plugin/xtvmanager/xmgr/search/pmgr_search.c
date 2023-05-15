/**
	@file     	pmgr_search.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_appmgr.h>
#include <mgr_search.h>
#include <xmgr_search.h>
#include "./local_include/_xmgr_search.h"
#include <mgr_action.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

BUS_Callback_t	pmgr_search_GetSearchAction (HUINT32 ulActionId, DxDeliveryType_e eDeliveryType)
{
	BUS_Callback_t	pfnSearchProc = NULL;
	DxOperator_e	eOperatorType = 0;
	HUINT32			ulMainActionId = MGR_ACTION_GetMainActionId();
	HERROR			hErr;

//	eOperatorType = MWC_PARAM_GetOperatorType();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperatorType, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}
	HxLOG_Print("eOperatorType = %d\n", eOperatorType);


	switch (eDeliveryType)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case	eDxDELIVERY_TYPE_SAT:
			switch(eOperatorType)
			{
			#if defined(CONFIG_OP_FREESAT)
			case	eDxOPERATOR_Freesat:
				pfnSearchProc = xmgr_search_GetAction_Freesat(ulActionId);
				break;
			#endif

			#if defined(CONFIG_OP_MBC)
			case	eDxOPERATOR_MBC:
				pfnSearchProc = (ulMainActionId == ulActionId) ? xproc_search_mbc_Action_0 : xproc_search_mbc_Action_1;
				break;
			#endif
			
			default:
				pfnSearchProc = (ulMainActionId == ulActionId) ? xproc_search_SatAction0_Base : xproc_search_SatAction1_Base;
				break;
			}
			break;
#endif

#if defined(CONFIG_MW_CH_CABLE)
		case	eDxDELIVERY_TYPE_CAB:
			switch(eOperatorType)
			{
			#if defined(CONFIG_OP_ERTELECOM)
			case	eDxOPERATOR_ERtelecom:
				pfnSearchProc = (ulMainActionId == ulActionId) ? xproc_search_ertelecom_Action_0 : xproc_search_ertelecom_Action_1;
				break;
			#endif
			#if defined(CONFIG_OP_ZIGGO)
			case	eDxOPERATOR_Ziggo:
				pfnSearchProc = (ulMainActionId == ulActionId) ? xproc_search_CabAction0_Ziggo : xproc_search_CabAction1_Ziggo;
				break;
			#endif
			#if defined(CONFIG_OP_NORDIG)
			case	eDxOPERATOR_Tdc:
			case	eDxOPERATOR_Telenor:
				pfnSearchProc = (ulMainActionId == ulActionId) ? xproc_search_nordig_Action_0 : xproc_search_nordig_Action_1;
				break;
			#endif
			default:
			#if defined(CONFIG_OP_NORDIG) // search action proc 강제 지정 - eOperatorType 제대로 저장 되기 전까지 임시 사용
				pfnSearchProc = (ulMainActionId == ulActionId) ? xproc_search_nordig_Action_0 : xproc_search_nordig_Action_1;
			#else
				pfnSearchProc = (ulMainActionId == ulActionId) ? xproc_search_CabAction0_Base : xproc_search_CabAction1_Base;
			#endif
				break;
			}
			break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case	eDxDELIVERY_TYPE_TER:
			pfnSearchProc = (ulMainActionId == ulActionId) ? xproc_search_TerAction0_Base : xproc_search_TerAction1_Base;
			break;
#endif
		default:
			HxLOG_Error("Not Supported Tuner Type!!!\n");
			break;
	}

	return pfnSearchProc;
}

BUS_Callback_t	pmgr_search_GetFinadSatAction (HUINT32 ulActionId)
{
	BUS_Callback_t	pfnFindSatProc = NULL;
	DxOperator_e	eOperatorType = 0;
//	HUINT32			ulMainActionId = MGR_ACTION_GetMainActionId();
	HERROR			hErr;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperatorType, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}

#if defined(CONFIG_MW_CH_SATELLITE)
	switch(eOperatorType)
	{
	default:
		pfnFindSatProc = xproc_search_SatFind;
		break;
	}
#endif

	return pfnFindSatProc;
}

HERROR	pmgr_search_SetSearchMode (MgrSearch_NetType_e eSrcNetType, MgrSearch_TpType_e eSrcTpType, SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pstTuningInfo)
{
	return xmgr_search_SetSearchMode_Base (eSrcNetType, eSrcTpType, eSatType, pstTuningInfo);
}

HERROR pmgr_search_GetSearchType (HUINT8 *pucSearchType)
{
#if defined(CONFIG_OP_FREESAT)
	return ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

HERROR	pmgr_search_GetNetworkIdAndSearchParam (HUINT16 *pusNetId, HUINT32 *pulStartFreq, HUINT32 *pulStopFreq)
{
	return xmgr_search_GetNetworkIdAndSearchParam_Base (pusNetId, pulStartFreq, pulStopFreq);
}

HBOOL pmgr_search_IsTsInfoAppendableForSameOnTsIdAndDiffTuneInfo (HUINT32 ulActionId, DxDeliveryType_e eDeliType)
{
	return xmgr_search_IsTsInfoAppendableForSameOnTsIdAndDiffTuneInfo_Base (ulActionId, eDeliType);
}

MgrSearch_ResultContext_t *pmgr_search_ResultGetContext (HUINT32 ulActionId)
{
	return xmgr_search_ResultGetContext (ulActionId);
}

HERROR pmgr_search_ResultCountService (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 *pulSvcNum)
{
#if defined(CONFIG_OP_FREESAT)
	return xmgr_search_ResultCountService_Freesat (ulActionId, eSvcType, pulSvcNum);
#else
	return xmgr_search_ResultCountService_Base (ulActionId, eSvcType, pulSvcNum);
#endif
}

HERROR pmgr_search_ResultGetService (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 ulIndex, DbSvc_Info_t *pstSvcInfo)
{
#if defined(CONFIG_OP_FREESAT)
	return xmgr_search_ResultGetService_Freesat (ulActionId, eSvcType, ulIndex, pstSvcInfo);
#else
	return xmgr_search_ResultGetService_Base (ulActionId, eSvcType, ulIndex, pstSvcInfo);
#endif
}

BUS_Callback_t	pmgr_search_GetSatConnectionAction (void)
{
#if defined(CONFIG_MW_CH_SATELLITE)
#if defined(CONFIG_MW_CH_SAT_RFINPUT_SELECTABLE)
	// RF-Input에 따라 방식이 달라진다.
	return xproc_search_SatRfDetect;
#else
	return xproc_search_SatConnectionTypeDetectAction;
#endif
#else
	return NULL;
#endif
}

BUS_Callback_t	pmgr_search_GetLcnUpdateSearchAction (void)
{
#if defined(CONFIG_OP_SES)
	return xproc_search_lcnupdate_Action_Ses;
#else
	return NULL;
#endif
}

BUS_Callback_t	pmgr_search_GetLcnUpdateSearchMainProc (void)
{
#if defined(CONFIG_OP_SES)
	return xproc_search_lcnupdate_Ses;
#else
	return NULL;
#endif
}


/*********************** End of File ******************************/
