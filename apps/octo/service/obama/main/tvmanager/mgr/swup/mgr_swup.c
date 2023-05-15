/**
	@file     ap_ota.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/01/15		initiated					JK Baek\n
	 2008/03/14		renamed					wjmoh\n
	 2008/09/05		Satellite part is divided		jinh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <db_param.h>
#include <db_svc.h>

#include <svc_si.h>
#include <svc_swup.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_swup.h>

#include <pmgr_swup.h>
#include <pmgr_model.h>

#include "local_include/_mgr_swup.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	struct
{
	HINT32		nMessage;
	Handle_t	hAction;
	HINT32		nParam1;
	HINT32		nParam2;
	HINT32		nParam3;
} MgrSwup_IntParam_t;

typedef	struct
{
	MgrSwup_BaseContents_t		stBaseContents;

	HCHAR				szModuleName[SWUPDATE_MODULE_NAME_LEN];

	MgrSwup_State_e		eState;
	MgrSwup_State_e		ePrevState;

	void				*pvUserData;

	//	target RF download info.
	HBOOL				bOTAFlag;
	DbSvc_TsInfo_t		stSvcTsInfo;
	HINT32				nOtaInfoLength;
	HUINT8				*pucOtaInfo;

	//	target IP download info.
	SvcSwUpdate_DetectResult_t	stDetectResult;

	pfnEnterAction_t	pfnEnterAction;
	pfnGetNextState_t	pfnGetNextState;
	pfnLeaveAction_t	pfnLeaveAction;
} MgrSwup_StateInfo_t;

typedef	struct
{
	HxList_t			*pstStateInfoList;
} MgrSwup_Contents_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC MgrSwup_Contents_t		s_stSwupContentsInfo;
STATIC MgrSwup_ModuleInfo_t	s_stSwupModuleInfo;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___Static_Function_ProtoType____
STATIC BUS_Result_t	proc_swup_Main (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC Handle_t		mgr_swup_Create (HCHAR *pszModuleName, void *pvUserData, pfnEnterAction_t pfnEnterAction, pfnLeaveAction_t pfnLeaveAction, pfnGetNextState_t pfnGetNextState, DxBatchType_e	eBatchProcess);
STATIC HERROR		mgr_swup_Destroy (Handle_t hSwUpdate);

#define ___Static_Function____

#if defined(CONFIG_DEBUG)
STATIC const HINT8 * mgr_swup_MakeStrMessage(HINT32 nMessage)
{
	switch (nMessage)
	{
		ENUM_TO_STR(eMEVT_SWUP);
		ENUM_TO_STR(eMEVT_SWUP_SET_STATE);
		ENUM_TO_STR(eMEVT_SWUP_STATE_POST_MSG);
		ENUM_TO_STR(eMEVT_SWUP_INIT);
		ENUM_TO_STR(eMEVT_SWUP_CONNECT_START);
		ENUM_TO_STR(eMEVT_SWUP_CONNECT_SUCCESS);
		ENUM_TO_STR(eMEVT_SWUP_CONNECT_FAIL);
		ENUM_TO_STR(eMEVT_SWUP_DETECT_START);
		ENUM_TO_STR(eMEVT_SWUP_DETECT_STOP);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_DETECT_SUCCESS);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_DETECT_FAIL);
		ENUM_TO_STR(eMEVT_SWUP_DOWNLOAD_START);
		ENUM_TO_STR(eMEVT_SWUP_DOWNLOAD_STOP);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_DOWNLOAD_PROGRESS);
		ENUM_TO_STR(eMEVT_SWUP_INSTALL_START);
		ENUM_TO_STR(eMEVT_SWUP_INSTALL_STOP);
		ENUM_TO_STR(eMEVT_SWUP_INSTALL_SUCCESS);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_INSTALL_FAIL);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_INSTALL_PROGRESS);
		default:
			break;
	}
	return ("Unknown");

}

STATIC const HINT8 * mgr_swup_GetEnumStrOfState(MgrSwup_State_e State)
{
	switch (State)
	{
		ENUM_TO_STR(eSWUP_STATE_Unknown);
		ENUM_TO_STR(eSWUP_STATE_Init);
		ENUM_TO_STR(eSWUP_STATE_Connect);
		ENUM_TO_STR(eSWUP_STATE_Detect);
		ENUM_TO_STR(eSWUP_STATE_Download);
		ENUM_TO_STR(eSWUP_STATE_Install);
		ENUM_TO_STR(eSWUP_STATE_Error);
		ENUM_TO_STR(eSWUP_STATE_Finish);
		default:
			return ("Unknown");
	}
}
#endif

STATIC MgrSwup_StateInfo_t	*mgr_swup_getStateInfo(Handle_t hSwUpdate)
{
	return (MgrSwup_StateInfo_t *)hSwUpdate;
}

STATIC HERROR	mgr_swup_IsValidSwUpdate(Handle_t hSwUpdate)
{
	HxList_t			*list = NULL;

	if (NULL == (void*)hSwUpdate)
		return ERR_FAIL;

	list = HLIB_LIST_Find(s_stSwupContentsInfo.pstStateInfoList, (void*)hSwUpdate);
	if (list != NULL)
		return ERR_OK;

	return ERR_FAIL;
}

STATIC MgrSwup_ModuleInfo_t *mgr_swup_GetModuleInfo(void)
{
	return &s_stSwupModuleInfo;
}

STATIC HERROR	mgr_swup_RegisterCB(MgrSwup_ModuleCb_t *pstCallback)
{
	MgrSwup_ModuleCb_t *pstCb = &s_stSwupModuleInfo.stCallback;

	pstCb->pfnCreate		= pstCallback->pfnCreate;
	pstCb->pfnEnterAction	= pstCallback->pfnEnterAction;
	pstCb->pfnLeaveAction	= pstCallback->pfnLeaveAction;
	pstCb->pfnGetNextState	= pstCallback->pfnGetNextState;
	pstCb->pfnDestroy		= pstCallback->pfnDestroy;
	pstCb->pfnSetManualInfo	= pstCallback->pfnSetManualInfo;
	pstCb->pfnGetHomeTpInfo	= pstCallback->pfnGetHomeTpInfo;

	return ERR_OK;
}

STATIC MgrSwup_ModuleCb_t *mgr_swup_GetCB(void)
{
	return &s_stSwupModuleInfo.stCallback;
}

HERROR	MGR_SWUP_RegisterModuleInfo(MgrSwup_ModuleInfo_t *pstModuleInfo)
{
	HxSTD_MemSet(s_stSwupModuleInfo.szModuleName, '\0', SWUPDATE_MODULE_NAME_LEN);
	HLIB_STD_StrUtf8NCpy(s_stSwupModuleInfo.szModuleName, pstModuleInfo->szModuleName, SWUPDATE_MODULE_NAME_LEN);
	if ( mgr_swup_RegisterCB(&pstModuleInfo->stCallback) != ERR_OK)
		return ERR_FAIL;

	return ERR_OK;
}

HERROR	MGR_SWUP_SetDetectInfo(Handle_t hSwUpdate, SvcSwUpdate_DetectResult_t *pstDetectResultInfo)
{
	MgrSwup_StateInfo_t	*pstStateInfo = mgr_swup_getStateInfo(hSwUpdate);

	if (pstStateInfo == NULL || (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK) )
	{
		HxLOG_Critical("pstStateInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy(&pstStateInfo->stDetectResult, pstDetectResultInfo, sizeof(SvcSwUpdate_DetectResult_t));

	HxLOG_Debug("SetDetectInfo : [%d]\n", pstDetectResultInfo->eSource);
	return ERR_OK;
}

HERROR	MGR_SWUP_GetDetectInfo(Handle_t hSwUpdate, SvcSwUpdate_DetectResult_t *pstDetectResultInfo)
{
	MgrSwup_StateInfo_t	*pstStateInfo = mgr_swup_getStateInfo(hSwUpdate);

	if (pstStateInfo == NULL || (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK) )
	{
		HxLOG_Critical("pstStateInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstDetectResultInfo, &pstStateInfo->stDetectResult, sizeof(SvcSwUpdate_DetectResult_t));
	HxLOG_Debug("GetDetectInfo : [%d]\n", pstDetectResultInfo->eSource);

	return ERR_OK;
}

STATIC BUS_Result_t mgr_swup_state_SetState(MgrSwup_StateInfo_t *pstStateInfo, MgrSwup_State_e eSetState, HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		 hRes;

	if (NULL == pstStateInfo)
		return ERR_BUS_SOURCE_NULL;

	hRes = ERR_BUS_MESSAGE_PASS;

	if ( mgr_swup_IsValidSwUpdate((Handle_t)pstStateInfo) != ERR_OK)
	{
		HxLOG_Critical("Can't find hSwUpdateHandle...\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("Msg [%s] Before State : Prev[%s <- %s -> %s] \n",
					mgr_swup_MakeStrMessage(nMessage),
					mgr_swup_GetEnumStrOfState(pstStateInfo->ePrevState),
					mgr_swup_GetEnumStrOfState(pstStateInfo->eState),
					mgr_swup_GetEnumStrOfState(eSetState));
#endif

	if (pstStateInfo->eState != eSetState)
	{
		// Execute Leave Action
		if (pstStateInfo->pfnLeaveAction)
		{
			hRes = (pstStateInfo->pfnLeaveAction)((Handle_t)pstStateInfo, pstStateInfo->eState, eSetState, pstStateInfo->pvUserData,
													nMessage, hAction, p1, p2, p3);
		}

		// State Transition
		pstStateInfo->ePrevState = pstStateInfo->eState;
		pstStateInfo->eState = eSetState;

		// Execute Enter Action
		if (pstStateInfo->pfnEnterAction)
		{
			hRes = pstStateInfo->pfnEnterAction((Handle_t)pstStateInfo, pstStateInfo->eState, pstStateInfo->ePrevState, pstStateInfo->pvUserData,
													nMessage, hAction, p1, p2, p3);
		}
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("Msg [%s] After State : [%s -> %s] \n",
					mgr_swup_MakeStrMessage(nMessage),
					mgr_swup_GetEnumStrOfState(pstStateInfo->ePrevState),
					mgr_swup_GetEnumStrOfState(pstStateInfo->eState));
#endif

	return hRes;
}

STATIC HERROR mgr_swup_state_dispatchMessage(Handle_t hSwUpdate, HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxList_t			*list;
	MgrSwup_StateInfo_t	*pstStateInfo;

	MgrSwup_State_e		eState;
	BUS_Result_t		hRes = MESSAGE_PASS;


	if ( hSwUpdate == HANDLE_NULL || (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK))
	{
		list = s_stSwupContentsInfo.pstStateInfoList;
		while (list)
		{
			pstStateInfo = (MgrSwup_StateInfo_t*)HLIB_LIST_Data(list);

			eState = eSWUP_STATE_Unknown;
			hRes   = MESSAGE_PASS;

			if (pstStateInfo->pfnGetNextState)
			{
				eState = pstStateInfo->pfnGetNextState((Handle_t)pstStateInfo, pstStateInfo->eState, pstStateInfo->pvUserData,
												nMessage, hAction, p1, p2, p3);

				if (eState != eSWUP_STATE_Unknown)
				{
					hRes = mgr_swup_state_SetState(pstStateInfo, eState, nMessage, hAction, p1, p2, p3);
				}
			}

			if (hRes != MESSAGE_PASS)
				break;

			list = list->next;
		}
	}
	else
	{
		if (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK)
		{
			HxLOG_Critical("Can't find hSwUpdate...\n");
			return ERR_FAIL;
		}

		pstStateInfo = mgr_swup_getStateInfo(hSwUpdate);
		eState = eSWUP_STATE_Unknown;
		hRes   = MESSAGE_PASS;

		if (pstStateInfo->pfnGetNextState)
		{
			eState = pstStateInfo->pfnGetNextState((Handle_t)pstStateInfo, pstStateInfo->eState, pstStateInfo->pvUserData,
											nMessage, hAction, p1, p2, p3);

			if (eState != eSWUP_STATE_Unknown)
			{
				hRes = mgr_swup_state_SetState(pstStateInfo, eState, nMessage, hAction, p1, p2, p3);
			}
		}

		return hRes;
	}

	return hRes;
}

STATIC Handle_t mgr_swup_Create(HCHAR *pszModuleName, void *pvUserData, pfnEnterAction_t pfnEnterAction, pfnLeaveAction_t pfnLeaveAction, pfnGetNextState_t pfnGetNextState, DxBatchType_e	eBatchProcess)
{
	MgrSwup_StateInfo_t		*pstInfo;

	pstInfo = (MgrSwup_StateInfo_t*)HLIB_STD_MemCalloc(sizeof(MgrSwup_StateInfo_t));

	if (pszModuleName)
		HLIB_STD_StrUtf8NCpy(pstInfo->szModuleName, pszModuleName, SWUPDATE_MODULE_NAME_LEN);
	else
		HLIB_STD_StrUtf8NCpy(pstInfo->szModuleName, "undefined", SWUPDATE_MODULE_NAME_LEN);

	pstInfo->stBaseContents.eBatchProcess = eBatchProcess;

	pstInfo->eState				= eSWUP_STATE_Unknown;
	pstInfo->ePrevState			= eSWUP_STATE_Unknown;
	pstInfo->pfnEnterAction		= pfnEnterAction;
	pstInfo->pfnLeaveAction		= pfnLeaveAction;
	pstInfo->pfnGetNextState	= pfnGetNextState;
	pstInfo->pvUserData			= pvUserData;

	s_stSwupContentsInfo.pstStateInfoList = HLIB_LIST_Append(s_stSwupContentsInfo.pstStateInfoList, (void*)pstInfo);

	return (Handle_t)pstInfo;
}

STATIC HERROR mgr_swup_Destroy(Handle_t hSwUpdate)
{
	MgrSwup_StateInfo_t		*pstInfo = mgr_swup_getStateInfo(hSwUpdate);

	s_stSwupContentsInfo.pstStateInfoList = HLIB_LIST_Remove(s_stSwupContentsInfo.pstStateInfoList, (void*)pstInfo);
	HLIB_STD_MemFree(pstInfo);

	return ERR_OK;
}

Handle_t MGR_SWUP_Create(void *pvUserSetInfo, DxBatchType_e	eBatchProcess)
{
	MgrSwup_ModuleInfo_t	*pstModuleInfo		= NULL;
	MgrSwup_ModuleCb_t	*pstModuleCb	 	= NULL;
	void				*pstUserContents	= NULL;
	Handle_t			hSwUpdate;

	if (MGR_MODEL_SwupInitProduct() == ERR_FAIL)
		return HANDLE_NULL;

	pstModuleInfo	= mgr_swup_GetModuleInfo();

	pstModuleCb		= mgr_swup_GetCB();

	if(pstModuleCb->pfnCreate)
		pstUserContents	= (void*)pstModuleCb->pfnCreate(pvUserSetInfo, eBatchProcess);

	hSwUpdate		= mgr_swup_Create(pstModuleInfo->szModuleName, pstUserContents, pstModuleCb->pfnEnterAction,
																				  pstModuleCb->pfnLeaveAction,
																				  pstModuleCb->pfnGetNextState,
																				  eBatchProcess);
	return hSwUpdate;
}

HERROR MGR_SWUP_GetUserData(Handle_t hSwUpdate, void **ppvUserData)
{
	MgrSwup_StateInfo_t		*pstStateInfo = mgr_swup_getStateInfo(hSwUpdate);

	if (pstStateInfo == NULL || (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK))
	{
		HxLOG_Critical("hSwUpdate Handle is NULL\n");
		return ERR_FAIL;
	}

	*ppvUserData = pstStateInfo->pvUserData;

	return ERR_OK;
}

HERROR MGR_SWUP_PostMessage(Handle_t hSwUpdate, HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrSwup_IntParam_t	*pstParam = NULL;
	MgrSwup_StateInfo_t	*pstInfo = mgr_swup_getStateInfo(hSwUpdate);

	if (pstInfo == NULL || (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK))
	{
		HxLOG_Critical("Can't find hSwUpdate...\n");
		return ERR_FAIL;
	}

	pstParam = HLIB_STD_MemCalloc(sizeof(MgrSwup_IntParam_t));
	if (NULL == pstParam)
		return ERR_FAIL;

	pstParam->nMessage = nMessage;
	pstParam->hAction  = hAction;
	pstParam->nParam1  = p1;
	pstParam->nParam2  = p2;
	pstParam->nParam3  = p3;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("nMessage (%s) \n", mgr_swup_MakeStrMessage(nMessage));
#endif

	BUS_PostData(proc_swup_Main, eMEVT_SWUP_STATE_POST_MSG, hSwUpdate, 0, 0, (void *)pstParam, sizeof(MgrSwup_IntParam_t));

	if(pstParam != NULL)
	{
		HLIB_STD_MemFree(pstParam);
		pstParam = NULL;
	}

	return ERR_OK;
}

HERROR MGR_SWUP_InitState(Handle_t hSwUpdate, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrSwup_StateInfo_t		*pstStateInfo = mgr_swup_getStateInfo(hSwUpdate);

	if ((pstStateInfo == NULL) || (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK))
	{
		HxLOG_Error("[%s][%4d] hSwUpdate Handle is NULL\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	if(pstStateInfo->eState != eSWUP_STATE_Unknown)
	{
		HxLOG_Error("[%s][%4d] pstStateInfo->eState is UnKown(0x%x) \n", __FUNCTION__, __LINE__, pstStateInfo->eState);
	//	return ERR_FAIL;
	}

	mgr_swup_state_SetState(pstStateInfo, eSWUP_STATE_Init, eMEVT_SWUP_INIT, (Handle_t)0, p1, p2, 0);

	return ERR_OK;
}

HUINT32	MGR_SWUP_GetStartUpdateSource(DxBatchType_e eBatchType)
{
	return pmgr_swup_GetStartUpdateSource(eBatchType);
}

MgrSwup_State_e	MGR_SWUP_GetCurrentState(Handle_t hSwUpdate)
{
	MgrSwup_StateInfo_t	*pstStateInfo = mgr_swup_getStateInfo(hSwUpdate);

	if (pstStateInfo == NULL || (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK))
	{
		HxLOG_Critical("hSwUpdate handle is NULL\n");
		return eSWUP_STATE_Unknown;
	}

	return pstStateInfo->eState;
}

// 현재 S/W Update 가 먼가 작업을 하는 가 ?
// FALSE --> s/w update 동작 X ,  TRUE --> s/w update 동작 O
HBOOL	MGR_SWUP_IsExec(Handle_t hSwUpdate)
{
	MgrSwup_StateInfo_t	*pstStateInfo 	= mgr_swup_getStateInfo(hSwUpdate);

	if (pstStateInfo == NULL || (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK))
	{
		HxLOG_Critical("hSwUpdate handle is NULL\n");
		return FALSE;
	}

	return TRUE;
}

HUINT32 MGR_SWUP_GetSource(Handle_t hSwUpdate)
{
	MgrSwup_StateInfo_t	*pstStateInfo = mgr_swup_getStateInfo(hSwUpdate);

	if (pstStateInfo == NULL || (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK))
	{
		HxLOG_Critical("hSwUpdate handle is NULL\n");
		return eDxSWUPDATE_SOURCE_None;
	}

	return pstStateInfo->stDetectResult.eSource;
}

/*
	스케쥴 레코딩 등에서 IsSwupExec 함수로 현재 S/W Update 가 실행되는지 여부를
	S/W Update 의 Object 를 바탕으로 비교하므로 Destroy 함수는 call 되도록 한다.
	s_stSwupContentsInfo 에 관리되며 , op 별 data 도 삭제시켜줌.
*/
HERROR	MGR_SWUP_Destroy(Handle_t hSwUpdate)
{
	void	*pstModuleData	= NULL;

	HxLOG_Trace();

	if (mgr_swup_IsValidSwUpdate(hSwUpdate) != ERR_OK)
	{
		HxLOG_Critical("Can't find hSwUpdate...\n");
		return ERR_FAIL;
	}

	if (MGR_SWUP_GetUserData(hSwUpdate, &pstModuleData) == ERR_OK)
	{
		if (pstModuleData)
		{
			MgrSwup_ModuleInfo_t *pstModuleInfo = mgr_swup_GetModuleInfo();
			if (pstModuleInfo && pstModuleInfo->stCallback.pfnDestroy)
			{
				pstModuleInfo->stCallback.pfnDestroy(pstModuleData);
			}
		}
	}

	return mgr_swup_Destroy(hSwUpdate);
}

HERROR	MGR_SWUP_SetManual(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo)
{
	MgrSwup_ModuleCb_t	*pstModuleCb	 	= NULL;

	pstModuleCb		= mgr_swup_GetCB();

	if(pstModuleCb)
		return pstModuleCb->pfnSetManualInfo(pstDstHomeTsInfo, pstUserHomeTsInfo);
	else
		return ERR_FAIL;
}

HERROR	MGR_SWUP_GetHomeTpInfo(DbSvc_TsInfo_t *pstHomeTsInfo)
{
	MgrSwup_ModuleCb_t	*pstModuleCb	 	= NULL;

	pstModuleCb		= mgr_swup_GetCB();

	if(pstModuleCb)
		return pstModuleCb->pfnGetHomeTpInfo(pstHomeTsInfo);
	else
		return ERR_FAIL;
}

STATIC HERROR	mgr_swup_MsgGwmCreate(MgrSwup_Contents_t *pstContents)
{
	//BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "proc_swup_Main");

	HxSTD_MemSet(pstContents, 0, sizeof(MgrSwup_Contents_t));

	return ERR_OK;
}

STATIC HERROR	mgr_swup_MsgGwmDestroyed(MgrSwup_Contents_t *pstContents)
{
	pstContents->pstStateInfoList = HLIB_LIST_RemoveAll(pstContents->pstStateInfoList);

	return ERR_OK;
}

STATIC BUS_Result_t proc_swup_Main(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	hErr;

	MgrSwup_Contents_t		*pstContents = &s_stSwupContentsInfo;

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Debug("---------- eMEVT_BUS_CREATE\n");
			hErr = mgr_swup_MsgGwmCreate (pstContents);
			if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }

			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Debug("---------- eMEVT_BUS_DESTROY\n");
			hErr = mgr_swup_MsgGwmDestroyed (pstContents);
			if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
			break;

		case eMEVT_DN_SWUP_ERROR:
			HxLOG_Print("\neMEVT_DN_SWUP_ERROR \n\n");
			{
				Handle_t			hMgrSwupdate = (Handle_t)p1;
				BUS_DestroyReason_t	eDestroyReason = eDESTROY_BUS_MYSELF;
				MgrSwup_FailReason_e	eFailReason = (MgrSwup_FailReason_e)p3;
				//eSWUP_FAILREASON_Unknown

				MGR_SWUP_Destroy(hMgrSwupdate);

				HxLOG_Debug("destory sw update main proc \n");

				if(eFailReason == eSWUP_FAILREASON_Action)
				{
					eDestroyReason = eDESTROY_BUS_ACTION;
				}
				BUS_MGR_DestroyWithReason(NULL, eDestroyReason, 0);
			}
			break;

		case eMEVT_DN_SWUP_FINISH:
			HxLOG_Print("\neMEVT_DN_SWUP_FINISH !! \n\n");
			{
				Handle_t			hMgrSwupdate = (Handle_t)p1;

				MGR_SWUP_Destroy(hMgrSwupdate);

				HxLOG_Debug("destory sw update main proc \n");
				BUS_MGR_Destroy(NULL);
			}
			break;

		case eMEVT_SWUP_SET_STATE:
			HxLOG_Debug("---------- eMEVT_SWUP_SET_STATE \n");
			break;

		case eMEVT_SWUP_STATE_POST_MSG:
			HxLOG_Debug("---------- eMEVT_SWUP_STATE_POST_MSG \n");
			{
				BUS_Result_t	hRes = MESSAGE_PASS;
				MgrSwup_IntParam_t	*pstParam = (MgrSwup_IntParam_t*)p3;

				if (pstParam == NULL)
				{
					HxLOG_Critical("pstParam is NULL\n");
					break;
				}

				hRes = mgr_swup_state_dispatchMessage(hAction, pstParam->nMessage, pstParam->hAction,
												pstParam->nParam1, pstParam->nParam2, pstParam->nParam3);

				return hRes;
			}
			break;

		default:
			return MESSAGE_PASS;
	}

	HxLOG_Print("MGR_SWUP_Main() out\n");

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

HERROR			MGR_SWUP_Start(void)
{
	BUS_Result_t	eRes;
	BUS_Mgr_t			mgr = NULL;

	mgr = BUS_MGR_Get(proc_swup_Main);
	if (mgr != NULL)
	{
		HxLOG_Debug("proc_swup_Main already exists \n");
		return ERR_OK;
	}

	eRes = BUS_MGR_Create("proc_swup_Main", APP_MONITORING_MGR_PRIORITY, proc_swup_Main, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR			MGR_SWUP_Stop(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Destroy(proc_swup_Main);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR MGR_SWUP_SetDefaultTp (void)
{
	return pmgr_swup_SetDefaultTp();
}

HERROR MGR_SWUP_GetSpec (DxSwUpdate_Spec_e *peSwUpdateSpec)
{
	if (peSwUpdateSpec == NULL)					{ return ERR_BUS_TARGET_NULL; }

	return pmgr_swup_GetSpec (peSwUpdateSpec);
}

DxSwUpdate_Spec_e MGR_SWUP_GetMultipleSpec(DxBatchType_e eBatchType)
{
	return pmgr_swup_GetMultipleSpec (eBatchType);
}

HERROR MGR_SWUP_SetSpec (DxSwUpdate_Spec_e eSwUpdateSpec)
{
	return pmgr_swup_SetSpec (eSwUpdateSpec);
}

HERROR MGR_SWUP_GetSiSpec (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec)
{
	if (NULL == pstSiSpec)					{ return ERR_FAIL; }

	return MGR_MODEL_SwupGetSiSpec (pstInArg, pstSiSpec);
}

BUS_Callback_t mgr_swup_DetectGetAction(DxSwUpdate_Source_e eSource)
{
	if (eSource == eDxSWUPDATE_SOURCE_Rf)
		return pmgr_swup_DetectGetAction();

	return (BUS_Callback_t)mgr_swup_IpGetProc();
}

BUS_Callback_t mgr_swup_DownloadGetAction(DxSwUpdate_Source_e eSource)
{
	DxSwUpdate_Spec_e	eSwUpdateSpec;

	pmgr_swup_GetSpec (&eSwUpdateSpec);
	if (eSource == eDxSWUPDATE_SOURCE_Rf)
	{
		return pmgr_swup_DownloadGetAction(eSwUpdateSpec);
	}

	return (BUS_Callback_t)mgr_swup_IpGetProc();
}

HBOOL	MGR_SWUP_CheckProc(MgrSwup_Type_e eType, HINT32 nParam, BUS_Callback_t pfnProc)
{
	HBOOL		bRet = FALSE;

	if (NULL == pfnProc)
		return FALSE;

	switch (eType)
	{
	case eSWUP_TYPE_Detect:
		bRet = (pfnProc == mgr_swup_DetectGetAction((DxSwUpdate_Source_e)nParam)) ? TRUE : FALSE;
		break;

	case eSWUP_TYPE_Download:
		bRet = (pfnProc == mgr_swup_DownloadGetAction((DxSwUpdate_Source_e)nParam)) ? TRUE : FALSE;
		break;

	case eSWUP_TYPE_Install:
		bRet = (pfnProc == mgr_swup_InstallGetProc()) ? TRUE : FALSE;
		break;

	default:
		break;
	}

	return bRet;
}

HBOOL MGR_SWUP_CheckMainProcExist(void)
{
	BUS_Mgr_t			mgr = NULL;

	mgr = BUS_MGR_Get(proc_swup_Main);
	if (mgr != NULL)
	{
		HxLOG_Debug("proc_swup_Main already exists \n");
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR	MGR_SWUP_FindUsbSwImage(Handle_t hSWUpdate, HCHAR *szPath, SvcSwUpdate_UsbDetectInfo_t *pstContents)
{
	return pmgr_swup_FindUsbSwImage(hSWUpdate, szPath, pstContents);
}

// TODO: 위치 이동. 일단 build 용.
typedef	struct
{
	Handle_t	hSwUpdate;
} MgrActionBatch_SwUp_Info_t;

// For Batch.
BUS_Callback_t			MGR_SWUP_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType) // Batch 에서만 사용할 init 함수
{
	HERROR							 hErr = ERR_FAIL;
	MgrActionBatch_SwUp_Info_t		*pstSwUpInfo;
	Handle_t						 hSwUpdate;
#if defined(CONFIG_OTA_MULTIPLE)
	DxSwUpdate_Spec_e				 eSwUpdateSpec = eDxSwUpdate_SPEC_NONE;
#endif

	HxLOG_Trace();

	HxLOG_Print(HxANSI_COLOR_YELLOW("eBatchType = %d\n"), eBatchType);

	// TODO: 이 함수에서 return NULL 이 될 리는 없지만,,,만약 return NULL이 되면 batch mgr에서 계속 restart 하게 된다.
	// TODO: 원래는 MGR_SWUP_GetBatchOtaCBList() 에서 아예 cb 을 꼽지 말아야 한다.

	hErr = MGR_SWUP_Start();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("MGR_SWUP_Start error \n");
		return NULL;
	}


	// TODO: 각종 init 작업, plug in에서 담당.

#if defined(CONFIG_OTA_MULTIPLE)
	eSwUpdateSpec = MGR_SWUP_GetMultipleSpec(eBatchType);
	hErr = MGR_SWUP_SetSpec(eSwUpdateSpec);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("MGR_SWUP_SetSpec Error \n");
		return NULL;
	}
#endif

	hSwUpdate = MGR_SWUP_Create(NULL, eBatchType);

	if(hSwUpdate == HANDLE_NULL)
	{
		HxLOG_Error("hSwUpdate is NULL \n");
		return NULL;
	}

	pstSwUpInfo = HLIB_STD_MemAlloc(sizeof(MgrActionBatch_SwUp_Info_t));
	if(pstSwUpInfo == NULL)
	{
		HxLOG_Error("pstSwUpInfo MemAlloc Error \n");
		return NULL;
	}
	else
	{
		HxLOG_Debug("hSwUpdate(0x%08x)  \n", hSwUpdate);
		pstSwUpInfo->hSwUpdate = hSwUpdate;

		MGR_SWUP_InitState(pstSwUpInfo->hSwUpdate, MGR_SWUP_GetStartUpdateSource(eBatchType),0,0);
		MGR_SWUP_PostMessage(pstSwUpInfo->hSwUpdate, eMEVT_SWUP_CONNECT_START, (Handle_t) 0 , 0, 0, 0);

		*ppvUserData = (void*)pstSwUpInfo;

		*pulUserDataLength = sizeof(MgrActionBatch_SwUp_Info_t);

		// main proc의 handle 을 return 하여 batch 에서 사용한다.
		return proc_swup_Main;
	}
}

HERROR			MGR_SWUP_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType) // Batch 에서만 사용할 deinit 함수
{
	HERROR	hErr = ERR_FAIL;
	MgrActionBatch_SwUp_Info_t			*pstSwUpInfo = (MgrActionBatch_SwUp_Info_t*)pvUserData;
	SvcSwUpdate_DetectResult_t			*pstDetectResult = NULL;

	HxLOG_Trace();

	HxLOG_Print(HxANSI_COLOR_YELLOW("eBatchType = %d\n"), eBatchType);

	if (pstSwUpInfo == NULL)
	{
		HxLOG_Error("pstSwUpInfo is NULL \n");
		return ERR_FAIL;
	}

	pstDetectResult = (SvcSwUpdate_DetectResult_t *)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
	if (pstDetectResult == NULL)
	{
		HxLOG_Error("Memory alloc fail!!\n");
		return ERR_FAIL;
	}

	hErr = MGR_SWUP_GetDetectInfo(pstSwUpInfo->hSwUpdate, pstDetectResult);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("Can not Get OTA Detected Info. \n");
	}

	// deinit은 이미 mgr_swup 이 kill 된 이후에 불릴 것이기 때문에, err를 탈 것이지만...일단 넣어둔다.
	hErr = MGR_SWUP_Destroy(pstSwUpInfo->hSwUpdate);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("MGR_SWUP_Destroy. because it's already destroyed. \n");
	}

	hErr = MGR_SWUP_Stop();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("MGR_SWUP_Stop. because it's already destroyed. \n");
	}

	if(pstDetectResult->eSource != eDxSWUPDATE_SOURCE_None)
	{
		hErr = MGR_SWUP_DownloadDestroy(pstDetectResult->eSource);
		if(hErr == ERR_FAIL)
		{
			HxLOG_Error("MGR_SWUP_DownloadDestroy. because it's already destroyed. \n");
		}
	}

	if(pstSwUpInfo != NULL)
	{
		HLIB_STD_MemFree(pstSwUpInfo);
		pstSwUpInfo = NULL;
	}

	if(pstDetectResult != NULL)
	{
		HLIB_STD_MemFree(pstDetectResult);
	}
	// TODO: 각종 init 작업, plug in에서 담당.

	return hErr;
}

HERROR			MGR_SWUP_RescheduleBatch(DxBatchType_e eBatchType) // Batch 에서만 사용할 reschedule 함수
{
	HxLOG_Trace();

	return ERR_OK;
}
HERROR MGR_SWUP_GetBatchOtaCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	HxLOG_Trace();

	fnBatchProcCBList->fnAppInit = MGR_SWUP_InitBatch;
	fnBatchProcCBList->fnAppDeinit = MGR_SWUP_DeinitBatch;
	fnBatchProcCBList->fnAppReSchedule = MGR_SWUP_RescheduleBatch;

	return ERR_OK;
}

HERROR MGR_SWUP_GetBatchOtaTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	HxLOG_Trace();
	return ERR_OK;
}

MgrSwup_FailReason_e MGR_SWUP_ConvertFailReasonSvctoMgr(SvcSi_OtaFailReason_e eSvcFailreason)
{
	switch (eSvcFailreason)
	{
		case	eSvcSi_OTA_FAILREASON_Unknown:
			return eSWUP_FAILREASON_Unknown;
		break;

		case	eSvcSi_OTA_FAILREASON_Latest_Version:
			return eSWUP_FAILREASON_Latest_Version;
		break;

		case	eSvcSi_OTA_FAILREASON_Action:
			return eSWUP_FAILREASON_Action;
		break;

		default:
			return eSWUP_FAILREASON_Unknown;
		break;
	}

	return eSWUP_FAILREASON_Unknown;
}
/* End of File. ---------------------------------------------------------- */

