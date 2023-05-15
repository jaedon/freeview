/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_cas_action.c
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
#include <svc_sys.h>
#include <svc_cas.h>

#include <mgr_cas.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct{
	DxCAS_GroupId_e				eCasGrpId;
	BUS_Callback_t				fnCasProc;
} om_cas_action_InstInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


STATIC HUINT32					s_ulSessionId;
STATIC APKS_CAS_ActionInfo_t				s_stOmCasActionStatus;
STATIC	HxList_t			*_pstOmCasActionSubCasInstList = NULL;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/



#define ___LOCAL_FUNCTIONS___

HERROR	om_cas_action_GetInfo(APKS_CAS_ActionInfo_t *pstInfo)
{
	return ERR_OK;
}

STATIC om_cas_action_InstInfo_t *om_cas_action_GetSubCasInstInfo(DxCAS_GroupId_e eCasGrpId)
{
	HxList_t						*pstInstItem = NULL;
	om_cas_action_InstInfo_t	*pstInstInfo = NULL;

	pstInstItem = HLIB_LIST_First(_pstOmCasActionSubCasInstList);
	while(pstInstItem != NULL)
	{
		pstInstInfo = (om_cas_action_InstInfo_t *)HLIB_LIST_Data(pstInstItem);
		if(pstInstInfo != NULL)
		{
			if((pstInstInfo->eCasGrpId == eCasGrpId))
			{
				break;
			}
		}

		pstInstItem = pstInstItem->next;
		pstInstInfo = NULL;
	}

	return pstInstInfo;
}


STATIC om_cas_action_InstInfo_t *om_cas_action_AppendSubCasInst(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasProc)
{
	om_cas_action_InstInfo_t	*pstInstInfo = NULL;

	pstInstInfo = (om_cas_action_InstInfo_t *)OxMGR_Calloc(sizeof(om_cas_action_InstInfo_t));
	if(pstInstInfo != NULL)
	{
		pstInstInfo->eCasGrpId = eCasGrpId;
		pstInstInfo->fnCasProc = fnCasProc;

		_pstOmCasActionSubCasInstList = HLIB_LIST_Append(_pstOmCasActionSubCasInstList, pstInstInfo);
	}

	return pstInstInfo;
}


BUS_Result_t om_cas_action_ProcEvent(HINT32 message, DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvSendData)
{
	HERROR					hErr = ERR_OK;
	APKS_CAS_ActionType_e			eActionType = eCasActionType_None;
	HINT32			nRpcHandle = 0;

	// cas action 관련 event 가 올라 왔을 때 각 event 별 처리하는 부분
	// 각 case별로 json data로 encoding 해서 notify 해 주도록
	switch(message)
	{
		// TUNE 관련 event
		case eMEVT_CAS_ACTION_TUNE_REQUEST:
			eActionType = eCasActionType_TuneRequest;
			break;
		case eMEVT_CAS_ACTION_FORCED_TUNE:
			eActionType = eCasActionType_ForcedTune;
			break;
		case eMEVT_CAS_ACTION_RESERVED_TUNE:
			eActionType = eCasActionType_ReservedTune;
			break;

		// UPDATE 관련 event
		case eMEVT_CAS_ACTION_UPDATE_START:
			eActionType = eCasActionType_UpdateStart;
			break;
		case eMEVT_CAS_ACTION_UPDATE_STOP:
			eActionType = eCasActionType_UpdateStop;
			break;

		default:
			return MESSAGE_PASS;
	}

	HxLOG_Info("Search Request EVENT 0x%x %d, eActionType %d\n", eCasGrpId, ulCasSlotNum, eActionType);

	if(hErr == ERR_OK)
	{
		s_stOmCasActionStatus.eCasActionType = eActionType;
		s_stOmCasActionStatus.unSlotId = ulCasSlotNum;
		s_stOmCasActionStatus.eCasGrpId = eCasGrpId;
		if(pvSendData != NULL)
		{
			HLIB_STD_MemCpySafe(s_stOmCasActionStatus.szData, APKS_CAS_SUB_DATA_LENG, pvSendData, HxSTD_StrLen(pvSendData));
		}
		else
		{
			HxSTD_memset(s_stOmCasActionStatus.szData, 0x00, APKS_CAS_SUB_DATA_LENG);
		}
		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
		if (hErr == ERR_OK)
		{
			HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_ACTION_onStatusChanged, "ib", s_ulSessionId, &s_stOmCasActionStatus, sizeof(APKS_CAS_ActionInfo_t));
		}
	}


	return MESSAGE_BREAK;
}


#define _____OAPI_CALLBACK_FUNCTIONS_____
#if 0
STATIC void om_cas_action_CheckStatusChange(void)
{
	HINT32					nCallbackId;
	HBOOL					bChanged = FALSE;
#if 0
	getxx(&value);
	if(value != s_stOmCasActionStatus.value)
	{
		bChanged = TRUE;
		s_stOmCasActionStatus.value = value;
	}

	if(bChanged == TRUE)
#endif
	{
		nCallbackId = OBAMA_GetCallbackID (RPC_OBAMA_CAS_ACTION_onStatusChanged);
		if (nCallbackId)
		{
			OBAMA_Callback(nCallbackId, "ib", s_ulSessionId, &s_stOmCasActionStatus, sizeof(APKS_CAS_ActionInfo_t));
		}
	}

}
#endif

#define _____OAPI_COMMAND_FUNCTIONS_____

STATIC HERROR om_cas_action_Init(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error! OBAMA_RpcProtocol_GetRpcHandle() failed. \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);
	HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", &s_stOmCasActionStatus, sizeof(APKS_CAS_ActionInfo_t));

	return ERR_OK;
}


#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_cas_action(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t				result = MESSAGE_PASS;
//	DxCAS_MmiSessionType_e		event = (DxCAS_MmiSessionType_e)hHandle;
	HUINT32						ulCasSlotNum = 0;
	void						*pvData = NULL;
	DxCAS_GroupId_e				eCasGrpId = eDxCAS_GROUPID_NONE;

	eCasGrpId = (DxCAS_GroupId_e)p1;
	ulCasSlotNum = (HUINT32)p2;
	pvData = (void *)p3;			//pvData가 NULL로 전송되는 event 존재 함.

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("%s created\n", (HINT8*) __FUNCTION__);
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

		case eMEVT_CAS_ACTION_TUNE_REQUEST:
		case eMEVT_CAS_ACTION_FORCED_TUNE:
		case eMEVT_CAS_ACTION_RESERVED_TUNE:
		case eMEVT_CAS_ACTION_UPDATE_START:
		case eMEVT_CAS_ACTION_UPDATE_STOP:
			result = om_cas_action_ProcEvent(message, eCasGrpId, ulCasSlotNum, pvData);
			break;

		default:
			result = MESSAGE_PASS;

	}

	return result;

}

#define ___GLOBAL_FUNCTIONS___
HERROR	OM_CAS_ACTION_RegisterCasProc(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasProc)
{
	om_cas_action_InstInfo_t	*pstInstInfo = NULL;

	pstInstInfo = om_cas_action_GetSubCasInstInfo(eCasGrpId);
	if(pstInstInfo != NULL)
	{
		HxLOG_Error("eCasGrpId(0x%x) exist!\n", eCasGrpId);

		return ERR_FAIL;
	}

	pstInstInfo = om_cas_action_AppendSubCasInst(eCasGrpId, fnCasProc);
	HxLOG_Info("eCasGrpId 0x%x, fnCasProc 0x%x => 0x%x\n", pstInstInfo->eCasGrpId, fnCasProc, pstInstInfo);

	return ERR_OK;
}


HERROR OM_CAS_ACTION_Init(void)
{
	HERROR			err;
	HINT32			nRpcHandle = 0;
	HxLOG_Debug("%s()\n", __FUNCTION__);

	// Initialize OAPI cache data
	om_cas_action_GetInfo(&s_stOmCasActionStatus);

	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_cas_action, 0, 0, 0, 0);
	err = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(err == ERR_OK)
	{
		//	-------   RPC Methods   -------	//
		err = HLIB_RPC_RegisterRequest(nRpcHandle, RPC_OAPI_CAS_ACTION_Init, NULL, (HxRPC_Func_t)om_cas_action_Init, NULL,
								"Activate the cas.action module: (void)");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest(%s) returned error(%d)\n", RPC_OAPI_CAS_ACTION_Init, err);
		}


		//	-------   Notifier  Callbakcs------- //
		err = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_ACTION_onStatusChanged, "ib",
								"CAS Action status changed\n"
								"\t\n"
								);
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_ACTION_onStatusChanged, err);
		}
	}

	return ERR_OK;

}



