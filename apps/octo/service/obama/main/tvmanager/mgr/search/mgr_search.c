/**
	@file     	mgr_search.c
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
#include <svc_pipe.h>
#include <svc_si.h>
#include <mgr_common.h>
#include <mgr_search.h>
#include <pmgr_search.h>
#include <pmgr_model.h>

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

BUS_Callback_t MGR_SEARCH_GetSearchAction (HUINT32 ulActionId, DxDeliveryType_e eDeliveryType)
{
	return pmgr_search_GetSearchAction (ulActionId, eDeliveryType);
}

BUS_Callback_t	MGR_SEARCH_GetFinadSatAction (HUINT32 ulActionId)
{
	return pmgr_search_GetFinadSatAction(ulActionId);
}

HERROR MGR_SEARCH_SetSearchMode (MgrSearch_NetType_e eSrcNetType, MgrSearch_TpType_e eSrcTpType, SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pstTuningInfo)
{
	return pmgr_search_SetSearchMode (eSrcNetType, eSrcTpType, eSatType, pstTuningInfo);
}

HERROR MGR_SEARCH_GetSearchType (HUINT8 *pucSearchType)
{
	return pmgr_search_GetSearchType (pucSearchType);
}

HERROR MGR_SEARCH_GetSiSpec (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec)
{
	return MGR_MODEL_SearchGetSiSpec (pstOption, pstSiSpec);
}

HERROR MGR_SEARCH_GetExtSearchSiSpec (SvcSi_ExtSearchOption_t *pstOption, SvcSi_ExtSearchSpec_t *pstSiSpec)
{
	return MGR_MODEL_ExtsearchGetSiSpec (pstOption, pstSiSpec);
}

HERROR MGR_SEARCH_GetNetworkIdAndSearchParam (HUINT16 *pusNetId, HUINT32 *pulStartFreq, HUINT32 *pulStopFreq)
{
	return pmgr_search_GetNetworkIdAndSearchParam (pusNetId, pulStartFreq, pulStopFreq);
}

/* Non-LCN Service 관련 Numbering Spec을 알아 낸다 */
HERROR MGR_SEARCH_GetLcnPolicy (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy)
{
	return MGR_MODEL_SearchGetLcnPolicy (peLcnPolicy, pePreProgChNumPolicy);
}

HBOOL MGR_SEARCH_IsTsInfoAppendableForSameOnTsIdAndDiffTuneInfo(HUINT32 ulActionId, DxDeliveryType_e eDeliType)
{
	return pmgr_search_IsTsInfoAppendableForSameOnTsIdAndDiffTuneInfo(ulActionId, eDeliType);
}

BUS_Callback_t		MGR_SEARCH_GetSatConnectionAction(void)
{
	return pmgr_search_GetSatConnectionAction();
}

BUS_Callback_t		MGR_SEARCH_GetLcnUpdateSearchAction(void)
{
	return pmgr_search_GetLcnUpdateSearchAction();
}

BUS_Callback_t		MGR_SEARCH_GetLcnUpdateSearchMainProc(void)
{
	return pmgr_search_GetLcnUpdateSearchMainProc();
}

MgrSearch_ResultContext_t *mgr_search_ResultGetContext(HUINT32 ulActionId)
{
	return pmgr_search_ResultGetContext(ulActionId);
}

HERROR	MGR_SEARCH_StartLcnUpdate(HUINT32 ulSearchMode)
{
	if (BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, MGR_SEARCH_GetLcnUpdateSearchAction(), HANDLE_NULL, ulSearchMode, 0, 0) == ERR_OK)
	{
		BUS_SendMessage(MGR_SEARCH_GetLcnUpdateSearchAction(), eMEVT_SEARCH_ACTION_START, HANDLE_NULL, 0, 0, 0);
		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR	MGR_SEARCH_StopLcnUpdate(BUS_DestroyReason_t eDestroyReason)
{
	if (BUS_MGR_Get(MGR_SEARCH_GetLcnUpdateSearchAction()) != NULL)
	{
		BUS_MGR_DestroyWithReason(MGR_SEARCH_GetLcnUpdateSearchAction(), eDestroyReason, 0);
	}
	return ERR_OK;
}

// For Batch.
BUS_Callback_t		MGR_SEARCH_InitLcnUpdateBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType)
{
	BUS_Callback_t		pfnSrchProc = NULL;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	// TODO: MGR_SEARCH_StartLcnUpdate 안에서 MGR_SEARCH_GetLcnUpdateSearchAction 하고 있으므로 중복.
	pfnSrchProc = MGR_SEARCH_GetLcnUpdateSearchAction();
	if (pfnSrchProc == NULL)
	{
		HxLOG_Error("no LCN proc \n");
		return NULL;
	}

	hErr = MGR_SEARCH_StartLcnUpdate(0);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("cannot start lcn update \n");
	}

	return pfnSrchProc;

}

HERROR			MGR_SEARCH_DeinitLcnUpdateBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR			hErr = ERR_FAIL;
	HxLOG_Trace();

	hErr = MGR_SEARCH_StopLcnUpdate(eDESTROY_BUS_MYSELF);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("stop lcn update fail because it's already destroyed.\n");
	}

	return hErr;
}

HERROR			MGR_SEARCH_RescheduleLcnUpdateBatch(DxBatchType_e eBatchType)
{
	HxLOG_Trace();
	return ERR_OK;
}

HERROR 			MGR_SEARCH_GetBatchLcnUpdateCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	HxLOG_Trace();

	fnBatchProcCBList->fnAppInit = MGR_SEARCH_InitLcnUpdateBatch;
	fnBatchProcCBList->fnAppDeinit = MGR_SEARCH_DeinitLcnUpdateBatch;
	fnBatchProcCBList->fnAppReSchedule = MGR_SEARCH_RescheduleLcnUpdateBatch;

	return ERR_OK;
}

HERROR 			MGR_SEARCH_GetBatchLcnUpdateTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	HxLOG_Trace();
	return ERR_OK;
}


/*********************** End of File ******************************/
