/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_resource.c
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
#include <rlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <oapi_resource.h>
#include <octo_common.h>
#include <svc_pipe.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_search.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	NOTIFY_ACTION_STATUS_HIGHER_THAN_VIEW	1
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

#define _____OAPI_CALLBACK_FUNCTIONS_____
// resource.onActivated (OxRsc_InitialInfo_t *pstInitInfo)
STATIC void om_rsc_CbOnActivated (void)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		RxRscType_e				 eRscType;
		OxRsc_InitialInfo_t 	 stInitInfo;

		if (OxRSC_RSCTYPE_MAX < eRxRSCTYPE_MAX)
		{
			HxLOG_Critical ("[%s:%d] OxRSC_RSCTYPE_MAX is smaller than eRxRSCTYPE_MAX: You must increase eRxRSCTYPE_MAX!!\n");
			abort();
		}

		HxSTD_MemSet (&stInitInfo, 0, sizeof(OxRsc_InitialInfo_t));

		stInitInfo.ulTotalActionNum		= eActionId_MAX;
		stInitInfo.ulRscTypeNum			= eRxRSCTYPE_MAX;
		stInitInfo.ulMainViewActId		= MGR_ACTION_GetMainActionId();
		stInitInfo.ulViewActIdFirst		= eActionId_VIEW_FIRST;
		stInitInfo.ulViewActIdLast		= eActionId_VIEW_LAST;
		stInitInfo.ulRecActIdFirst		= eActionId_REC_FIRST;
		stInitInfo.ulRecActIdLast		= eActionId_REC_LAST;
		stInitInfo.ulBgActIdFirst		= eActionId_BG_FIRST;
		stInitInfo.ulBgActIdLast		= eActionId_BG_LAST;
		stInitInfo.ulViewActPriority	= eAmPriority_VIEW_ACTION;
		stInitInfo.ulRecActPriority		= eAmPriority_RECORD;
		stInitInfo.ulLsActPriority		= eAmPriority_VIEW_ACTION;

		for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
		{
			(void)SVC_PIPE_GetResourceNumber(eRscType, &(stInitInfo.aulRscNum[eRscType]));
		}
		HLIB_RPC_Notify (nRpcHandle, "resource.onActivated", "b", &stInitInfo, sizeof(OxRsc_InitialInfo_t));
	}
}

// resource.onRscUsageTypeSet (RxRscType_e eRscType, int nRscNum, RxUsageType_e *astRscUsageArray)
STATIC void om_rsc_CbOnRscCapabilitySet (void)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		RxRscType_e				 eRscType;
		RxUsageType_e			*pstRscUsageArray = (RxUsageType_e *)HLIB_STD_MemAlloc (sizeof(RxUsageType_e) * 64);

		if (NULL != pstRscUsageArray)
		{
			for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
			{
				HUINT32 				 ulRscCnt, ulRscNum = 0;
				HERROR					 hErr;

				hErr = SVC_PIPE_GetResourceNumber(eRscType, &ulRscNum);
				if ((ERR_OK == hErr) && (ulRscNum > 0))
				{
					for (ulRscCnt = 0; ulRscCnt < ulRscNum; ulRscCnt++)
					{
						hErr = SVC_PIPE_GetResourceUsageType (eRscType, ulRscCnt, &(pstRscUsageArray[ulRscCnt]));
						if (ERR_OK != hErr)
						{
							pstRscUsageArray[ulRscCnt] = eRxUSAGETYPE_NONE;
						}
					}
					HLIB_RPC_Notify (nRpcHandle, "resource.onRscUsageTypeSet", "iib", eRscType, ulRscNum, pstRscUsageArray, sizeof(RxUsageType_e) * ulRscNum);
				}
			}

			HLIB_STD_MemFree (pstRscUsageArray);
		}
	}
}

// resource.onRfInputSet (int nRfInputId, RxRfInfoCapacity_t *pstCapacity)
STATIC void om_rsc_CbOnRfInputCapabilitySet (void)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		HUINT32 				 ulRfInputId, ulRscNum = 0;
		RxRfInfoCapacity_t		 stRfInput;
		HERROR					 hErr;

		hErr = SVC_PIPE_GetResourceNumber(eRxRSCTYPE_RFINPUT, &ulRscNum);
		if ((ERR_OK == hErr) && (ulRscNum > 0))
		{
			for (ulRfInputId = 0; ulRfInputId < ulRscNum; ulRfInputId++)
			{
				hErr = SVC_PIPE_GetRfInputCapacity(ulRfInputId, &stRfInput);
				if (ERR_OK == hErr)
				{
					HLIB_RPC_Notify (nRpcHandle, "resource.onRfInputSet", "ib", ulRfInputId, &stRfInput, sizeof(RxRfInfoCapacity_t));

				}
			}
		}
	}
}

// resource.onTunerSet (int nTunerId, DxDeliveryType_e eSupportedDeliType)
STATIC void om_rsc_CbOnTunerCapabilitySet (void)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);


	if (hErr == ERR_OK)
	{
		HUINT32 				 ulRscId, ulRscNum = 0;
		DxDeliveryType_e		 eSupportedDeliType;
		HERROR					 hErr;

		hErr = SVC_PIPE_GetResourceNumber(eRxRSCTYPE_TUNER_GROUP, &ulRscNum);
		if ((ERR_OK == hErr) && (ulRscNum > 0))
		{
			for (ulRscId = 0; ulRscId < ulRscNum; ulRscId++)
			{
				hErr = SVC_PIPE_GetTunerCapacity(ulRscId, &eSupportedDeliType);
				if (ERR_OK == hErr)
				{
					HLIB_RPC_Notify (nRpcHandle, "resource.onTunerSet", "ii", ulRscId, eSupportedDeliType);
				}
			}
		}
	}
}

// resource.onRfInputAbilityChanged (int nRfInputId, HBOOL bAbility)
STATIC void om_rsc_CbOnRfInputAbilityChanged (HINT32 ulRfInputId, HBOOL bAbility)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify (nRpcHandle, "resource.onRfInputAbilityChanged", "ii", ulRfInputId, bAbility);
	}
}

// resource.onActionTaken (int nActionId, int nPriority, int nNumRscAllocated, RxRscType_e *peRscType, RxRscObject_t *astRscObjArray)
STATIC void om_rsc_CbOnActionTaken (HUINT32 ulActionId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	RxRscType_e				*peRscTypeArray = NULL;
	RxRscObject_t			*pstRscObjArray = NULL;


	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		HUINT32				 ulSendRscNum = 0;
		RxRscType_e			 eRscType;
		HERROR				 hErr;

		peRscTypeArray = (RxRscType_e *)HLIB_STD_MemAlloc (sizeof(RxRscType_e) * eRxRSCTYPE_MAX);
		pstRscObjArray = (RxRscObject_t *)HLIB_STD_MemAlloc (sizeof(RxRscObject_t) * eRxRSCTYPE_MAX);

		if ((NULL == peRscTypeArray) || (NULL == pstRscObjArray))
		{
			HxLOG_Error ("Memory Allocation Failed:\n");
			goto END_FUNC;
		}

		for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
		{
			hErr = SVC_PIPE_GetResourceObject (ulActionId, eRscType, &(pstRscObjArray[ulSendRscNum]));
			if (ERR_OK == hErr)
			{
				peRscTypeArray[ulSendRscNum] = eRscType;
				ulSendRscNum ++;
			}
		}

		if (ulSendRscNum > 0)
		{
			HLIB_RPC_Notify (nRpcHandle, "resource.onActionTaken", "iiibb", ulActionId, MGR_ACTION_GetPrePriority (ulActionId),
							ulSendRscNum,
							peRscTypeArray, sizeof(RxRscType_e) * ulSendRscNum,
							pstRscObjArray, sizeof(RxRscObject_t) * ulSendRscNum);


		}
	}

END_FUNC:
	if (NULL != peRscTypeArray)					{ HLIB_STD_MemFree (peRscTypeArray); }
	if (NULL != pstRscObjArray)					{ HLIB_STD_MemFree (pstRscObjArray); }

	return;
}

// resource.onActionReleased (int nActionId)
STATIC void om_rsc_CbOnActionReleased (HUINT32 ulActionId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);


	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify (nRpcHandle, "resource.onActionReleased", "i", ulActionId);
	}
}

// resource.onActionPriorityChanged (int nActionId, int nPriority)
STATIC void om_rsc_CbOnActionPriorityChanged (HUINT32 ulActionId, HUINT32 ulPriority)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);


	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify (nRpcHandle, "resource.onActionPriorityChanged", "ii", ulActionId, ulPriority);
	}
}

// resource.onMainActionIdChanged (int nMainActionId)
STATIC void om_rsc_CbOnMainActionIdChanged (HUINT32 ulActionId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);


	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify (nRpcHandle, "resource.onMainActionIdChanged", "i", ulActionId);
	}
}

#define _____OAPI_COMMAND_FUNCTIONS_____
// void resource.activate (void)
STATIC HERROR om_rsc_CbActivate (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	DxAntennaConnection_e		 eAntConnectType = eDxANTCONN_ONECABLE;

	om_rsc_CbOnActivated();
	om_rsc_CbOnRscCapabilitySet();
	om_rsc_CbOnRfInputCapabilitySet();
	om_rsc_CbOnTunerCapabilitySet();

	(void)DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32 *)&eAntConnectType, 0);

	switch (eAntConnectType)
	{
		case eDxANTCONN_TWOSAME:
			om_rsc_CbOnRfInputAbilityChanged(0, TRUE);
			om_rsc_CbOnRfInputAbilityChanged(1, TRUE);
			break;

		case eDxANTCONN_ONECABLE:
		default:
			om_rsc_CbOnRfInputAbilityChanged(0, TRUE);
			om_rsc_CbOnRfInputAbilityChanged(1, FALSE);
			break;
	}

	return ERR_OK;
}

#define _____RPC_INITIALIZATION_____
STATIC HERROR om_rsc_InitRpcFunctions (void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
		//	 ----------------------------- //
		//	-------   RPC Methods	------- //
		//	 ----------------------------- //

		// void resource.activate (void)
		HLIB_RPC_RegisterCall (nRpcHandle, "resource.activate", NULL, (HxRPC_Func_t)om_rsc_CbActivate, NULL,
								"Activate the resource module: (void)");

		//   ----------------------------- //
		//	-------   Notifier  Callbakcs------- //
		//   ----------------------------- //

		// resource.onActivated (OxRsc_InitialInfo_t *pstInitInfo)
		HLIB_RPC_CreateNotification (nRpcHandle, "resource.onActivated", "b",
			"Resource Activated.\n"
			"\t   - (OxRsc_InitialInfo_t *pstInitInfo)\n"
			);

		// resource.onRscUsageTypeSet (RxRscType_e eRscType, int nRscNum, RxUsageType_e *astRscUsageArray)
		HLIB_RPC_CreateNotification (nRpcHandle, "resource.onRscUsageTypeSet", "iib",
			"Resource Usage Type Informed.\n"
			"\t   - (RxRscType_e eRscType, int nRscNum, RxUsageType_e *astRscUsageArray)\n"
			);

		// resource.onRfInputSet (int nRfInputId, RxRfInfoCapacity_t *pstCapacity)
		HLIB_RPC_CreateNotification (nRpcHandle, "resource.onRfInputSet", "ib",
			"RF Input Capacity Informed.\n"
			"\t   - (int nRfInputId, RxRfInfoCapacity_t *pstCapacity)\n"
			);

		// resource.onTunerSet (int nTunerId, DxDeliveryType_e eSupportedDeliType)
		HLIB_RPC_CreateNotification (nRpcHandle, "resource.onTunerSet", "ii",
			"Tuner Capacity Informed.\n"
			"\t   - (int nTunerId, DxDeliveryType_e eSupportedDeliType)\n"
			);

		// resource.onActionTaken (int nActionId, int nPriority, int nNumRscAllocated, RxRscType_e *peRscType, RxRscObject_t *astRscObjArray)
		HLIB_RPC_CreateNotification (nRpcHandle, "resource.onActionTaken", "iiibb",
			"Action Taken: Resource allocated.\n"
			"\t   - (int nActionId, int nPriority, int nNumRscAllocated, RxRscType_e *peRscType, RxRscObject_t *astRscObjArray)\n"
			);

		// resource.onActionReleased (int nActionId)
		HLIB_RPC_CreateNotification (nRpcHandle, "resource.onActionReleased", "i",
			"Action Released: Resource de-allocated.\n"
			"\t   - (int nActionId)\n"
			);

		// resource.onActionPriorityChanged (int nActionId, int nPriority)
		HLIB_RPC_CreateNotification (nRpcHandle, "resource.onActionPriorityChanged", "ii",
			"Action Priority Changed:\n"
			"\t   - (int nActionId, int nPriority)\n"
			);

		// resource.resource.onMainActionIdChanged (int nMainActionId)
		HLIB_RPC_CreateNotification (nRpcHandle, "resource.resource.onMainActionIdChanged", "i",
			"Main Action ID Changed:\n"
			"\t   - (int nMainActionId)\n"
			);

		// resource.onRfInputAbilityChanged (int nRfInputId, int nAbility)
		HLIB_RPC_CreateNotification (nRpcHandle, "resource.onRfInputAbilityChanged", "ii",
			"RF Input Ability Changed.\n"
			"\t   - (int nRfInputId, int nAbility)\n"
			);
	}
	return ERR_OK;
}

#define _____MSG_PROCESS_FUNCTIONS_____
STATIC BUS_Result_t om_rsc_MsgBusCreate (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_rsc_MsgBusDestroy (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_rsc_MsgActionSetupSucceeded (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hAction);

	om_rsc_CbOnActionTaken (ulActionId);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_rsc_MsgActionReleased (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hAction);

	om_rsc_CbOnActionReleased (ulActionId);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_rsc_MsgActionPrioriryChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hAction);

	om_rsc_CbOnActionPriorityChanged (ulActionId, p1);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_rsc_MsgActionMainChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulMainActionId = MGR_ACTION_GetMainActionId();

	om_rsc_CbOnMainActionIdChanged (ulMainActionId);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_rsc_EvtDbParamUpdated (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxAntennaConnection_e		 eAntConnectType = eDxANTCONN_ONECABLE;

	switch (p1)
	{
	case eDB_PARAM_ITEM_ANT_CONN_TYPE:
		(void)DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32 *)&eAntConnectType, 0);

		switch (eAntConnectType)
		{
			case eDxANTCONN_TWOSAME:
				om_rsc_CbOnRfInputAbilityChanged(0, TRUE);
				om_rsc_CbOnRfInputAbilityChanged(1, TRUE);
				break;
			case eDxANTCONN_ONECABLE:
			default:
				om_rsc_CbOnRfInputAbilityChanged(0, TRUE);
				om_rsc_CbOnRfInputAbilityChanged(1, FALSE);
				break;
		}

		break;

	default:
		break;
	}

	return MESSAGE_PASS;
}


#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_rsc_Proto(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult=MESSAGE_PASS;
#if defined(NOTIFY_ACTION_STATUS_HIGHER_THAN_VIEW)
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hHandle);
	MgrAction_Priority_e		eActionPriority = MGR_ACTION_GetPrePriority (ulActionId);
#endif

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
		eResult = om_rsc_MsgBusCreate (hHandle, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		eResult = om_rsc_MsgBusDestroy (hHandle, p1, p2, p3);
		break;

	case eMEVT_ACTION_SETUP_SUCCEEDED:
#if defined(NOTIFY_ACTION_STATUS_HIGHER_THAN_VIEW)
		if(eActionPriority < eAmPriority_VIEW_ACTION)
		{
			HxLOG_Debug("[eMEVT_ACTION_SETUP_SUCCEEDED] do not deal with ulActionId: %d because eActionPriority is %x (lower than view)\n", ulActionId, eActionPriority);
			break;
		}
		else
#endif
		{
			eResult = om_rsc_MsgActionSetupSucceeded (hHandle, p1, p2, p3);
		}
		break;

	case eMEVT_ACTION_NOTIFY_RELEASED:
#if defined(NOTIFY_ACTION_STATUS_HIGHER_THAN_VIEW)
		if(p2 < eAmPriority_VIEW_ACTION)
		{
			HxLOG_Debug("[eMEVT_ACTION_NOTIFY_RELEASED] do not deal with ulActionId: %d because eActionPriority is %x (lower than view)\n", ulActionId, eActionPriority);
			break;
		}
		else
#endif
		{
			eResult = om_rsc_MsgActionReleased (hHandle, p1, p2, p3);
		}
		break;

	case eMEVT_ACTION_NOTIFY_PRIORITY_CHANGED: // p1: new, p2: old, p3: action type
		HxLOG_Debug("[eMEVT_ACTION_NOTIFY_RELEASED] p1: %d, p2: %d, p3: %d \n", p1, p2, p3);
#if defined(NOTIFY_ACTION_STATUS_HIGHER_THAN_VIEW)
		if(p2 < eAmPriority_VIEW_ACTION && p1 >= eAmPriority_VIEW_ACTION)
		{
			HxLOG_Debug("update resource status like setup succeeded \n");
			//om_rsc_CbOnActionTaken에서 hSvc는 oapi로 올리고 있지는 않아서.
			eResult = om_rsc_MsgActionSetupSucceeded (hHandle, p3, 0, 0);
		}
		else if(p2 >= eAmPriority_VIEW_ACTION && p1 < eAmPriority_VIEW_ACTION)
		{
			HxLOG_Debug("update resource status like action released \n");
			// 마찬가지로 om_rsc_CbOnActionReleased에서 action priority는 안 올리고 있어서.
			eResult = om_rsc_MsgActionReleased (hHandle, p3, 0, 0);
		}
		else if(p2 < eAmPriority_VIEW_ACTION && p1 < eAmPriority_VIEW_ACTION)
		{
			HxLOG_Debug("nothing to do \n");
			// nothing to do.
			;
		}
		else
#endif
		{
			HxLOG_Debug("call om_rsc_MsgActionPrioriryChanged \n");
			eResult = om_rsc_MsgActionPrioriryChanged (hHandle, p1, p2, p3);
		}
		break;

	case eMEVT_ACTION_NOTIFY_MAIN_CHANGED:
		eResult = om_rsc_MsgActionMainChanged(hHandle, p1, p2, p3);
		break;

	case eSEVT_DB_PARAM_UPDATED:
		eResult = om_rsc_EvtDbParamUpdated (hHandle, p1, p2, p3);
		break;

	case eMEVT_SEARCH_ANT_CONNECT_TYPE_DETECTED:
		eResult = om_rsc_EvtDbParamUpdated (hHandle, eDB_PARAM_ITEM_ANT_CONN_TYPE, p2, p3);
		break;

	default:
		eResult = MESSAGE_PASS;
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}


#define ___MEMBER_FUNCTIONS___

#define ___GLOBAL_FUNCTIONS___
HERROR OM_RSC_Init (void)
{
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_rsc_Proto, HANDLE_NULL, 0, 0, 0);

	om_rsc_InitRpcFunctions();

	return ERR_OK;
}



