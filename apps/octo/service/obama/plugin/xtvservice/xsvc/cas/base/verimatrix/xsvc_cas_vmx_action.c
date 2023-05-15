/*
 * (c) 2011-2015 Humax Co., Ltd.
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
/**
	@file     xsvc_cas_vmx_action.c
	@brief    xsvc_cas_vmx_action.c (Verimatrix MAIN)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include "_xsvc_cas_vmx_iptv_main.h"
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_bc_main.h"
#endif
#include "_xsvc_cas_vmx_action.h"
#include "_xsvc_cas_vmx_os.h"
#include "_xsvc_cas_vmx_psi.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
extern HERROR VMX_VR_IPTVH_MGR_SendMessage(Vmx_Message_t *pstMsg);

#define Callback_functions__________________________________________________

STATIC void vmx_act_StartSession(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType)
{
	Vmx_Instance_t		*pstVmxInst;
	Handle_t 			 hStbHandle;
	HUINT32				 ulActionId;

	pstVmxInst = xsvc_vmx_main_GetInstance();

	ulActionId = GET_ACTION_ID(hAction);

	HxLOG_Print("[Enter] ulActionId(%d), eActType(%d)\n", ulActionId, eActType);

	if (ulActionId < VMX_CAS_MAX_NUM_OF_ACTION)
	{
		pstVmxInst->stActionList[ulActionId].hAction 			= hAction;
		pstVmxInst->stActionList[ulActionId].hSvc 				= hSvc;
		pstVmxInst->stActionList[ulActionId].eActType 			= eActType;
		pstVmxInst->stActionList[ulActionId].ucVmxServiceIdx 	= xsvc_vmx_main_GetServiceIdx(ulActionId);

		if (pstVmxInst->stActionList[ulActionId].ucVmxServiceIdx == VMX_INVALID_SERVICE_IDX)
		{
			HxLOG_Error("xsvc_vmx_main_GetServiceIdx() failed..!! ulActionId(%d) hSvc(0x%X) \n", ulActionId, hSvc);
			return;
		}

		hStbHandle = GET_STBHANDLE(hAction, hSvc);
		xsvc_vmx_psi_session_StartSvc(hStbHandle);
	}

	HxLOG_Print("[Exit]\n");
}

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB) && defined(VMX_SVC_TEST)
STATIC void vmx_act_bc_StopSession(Handle_t hAction, Handle_t hNextSvc)
{
	Vmx_Instance_t		*pstVmxInst = NULL;
	Vmx_Session_t 		*pSession = NULL;
	Handle_t 			hStbHandle;
	HUINT32				ulActionId;

	pstVmxInst = xsvc_vmx_main_GetInstance();
	ulActionId = GET_ACTION_ID(hAction);

	VmxActionPrint(DBG_PRINT, ("[Enter] ulActionId(%d)\n", ulActionId));

	if (ulActionId < VMX_CAS_MAX_NUM_OF_ACTION)
	{
		hStbHandle = GET_STBHANDLE(hAction, pstVmxInst->stActionList[ulActionId].hSvc);

		pSession = xsvc_vmx_psi_session_GetSessionFromhStbHandle(hStbHandle);

		if(pSession == NULL)
		{
			VmxActionError(DBG_ASSERT, ("Action(0x%x)\n", hAction));
			return;
		}

		if(pSession->pstPmtInfo == NULL)
		{
			VmxActionError(DBG_ASSERT, ("Action(0x%x)\n", hAction));
			return;
		}

		if(vmx_vr_dscr_StopDescrambling(hStbHandle, pSession->pstPmtInfo) != ERR_OK)
		{
			VmxActionError(DBG_ASSERT, ("Action(0x%x)\n", hAction));
			return;
		}
	}

	VmxActionPrint(DBG_PRINT, ("[Exit]\n"));
	return;
}
#endif

STATIC void vmx_act_StopSession(Handle_t hAction, Handle_t hNextSvc)
{
	Vmx_Instance_t		*pstVmxInst;
	Handle_t 			 hStbHandle;
	HUINT32				 ulActionId;
	HBOOL				 bNextSvcExist;

	pstVmxInst = xsvc_vmx_main_GetInstance();
	ulActionId = GET_ACTION_ID(hAction);

	HxLOG_Print("[Enter] ulActionId(%d)\n", ulActionId);

	if (ulActionId < VMX_CAS_MAX_NUM_OF_ACTION)
	{
		hStbHandle = GET_STBHANDLE(hAction, pstVmxInst->stActionList[ulActionId].hSvc);

		if (hNextSvc == HANDLE_NULL)
		{
			bNextSvcExist = FALSE;
		}
		else
		{
			bNextSvcExist = TRUE;
		}

		xsvc_vmx_psi_session_StopSvc(hStbHandle, bNextSvcExist);

		pstVmxInst->stActionList[ulActionId].bConnect			= FALSE;
		pstVmxInst->stActionList[ulActionId].hAction 			= HANDLE_NULL;
		pstVmxInst->stActionList[ulActionId].hSvc 				= HANDLE_NULL;
		pstVmxInst->stActionList[ulActionId].ucVmxServiceIdx 	= VMX_INVALID_SERVICE_IDX;
	}

	HxLOG_Print("[Exit]\n");
}

STATIC void vmx_act_SetPid(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	Vmx_Instance_t		*pstVmxInst;
	Vmx_EsPidInfo_t		stInfo;
	HUINT32				i;
	Handle_t 			hStbHandle;
	HUINT32				ulActionId;

	pstVmxInst = xsvc_vmx_main_GetInstance();
	ulActionId = GET_ACTION_ID(hAction);

	HxLOG_Print("[Enter] ulActionId(%d)\n", ulActionId);

	if (ulActionId < CAS_MAX_NUM_OF_ACTION)
	{
		hStbHandle = GET_STBHANDLE(hAction, pstVmxInst->stActionList[ulActionId].hSvc);
		HxSTD_MemSet(&stInfo, 0x00, sizeof(stInfo));


		//pstCaDscInfo->ulPidNum값이 VMX_MAX_NUM_OF_ESPID 값을 넘는 경우 hang-up의 원인이 된다... (?? 확인필요)
		for (i = 0; (i < pstCaDscInfo->ulPidNum) && (i < VMX_MAX_NUM_OF_ESPID) ; i++)
		{
			stInfo.eType[i] = pstCaDscInfo->stPidInfo[i].esType;
			stInfo.usPid[i] = pstCaDscInfo->stPidInfo[i].usPid;
		}

		stInfo.ulPidNum  = i;
		xsvc_vmx_psi_session_SetPid(hStbHandle, &stInfo);
	}

	HxLOG_Print("[Exit]\n");
}

STATIC void vmx_act_UpdatePmt(Handle_t hAction, HUINT8 *pucPmt)
{
	Vmx_Instance_t		*pstVmxInst;
	SvcCas_PmtInfo_t 	*pstPmtInfo;
	Handle_t 			hStbHandle;
	HUINT32				ulActionId;
	HUINT32				ulSecStatus;
	HUINT32				ulErr;

	pstVmxInst = xsvc_vmx_main_GetInstance();

	ulActionId = GET_ACTION_ID(hAction);

	HxLOG_Print("[Enter] ulActionId(%d)\n", ulActionId);

	if (ulActionId < VMX_CAS_MAX_NUM_OF_ACTION)
	{
		hStbHandle = GET_STBHANDLE(hAction, pstVmxInst->stActionList[ulActionId].hSvc);
	 	ulErr = svc_cas_MgrSubGetPmtSectionState(hAction, &ulSecStatus);
		if (ulErr != ERR_OK)
		{
			HxLOG_Print(" return error(%x), Action(0x%x)\n", ulErr, hAction);
			return;
		}

		if (ulSecStatus != TRUE)
		{
			HxLOG_Print("PMT Section status (%d)\n", ulSecStatus);
			return;
		}

		if ((pstPmtInfo = svc_cas_MgrSubGetPmtInfo(hAction)) == NULL)
		{
			HxLOG_Print("\rGetPmtInfo Error\n");
			return;
		}

		xsvc_vmx_psi_session_UpdatePmt(hStbHandle, pstPmtInfo);
	}

	HxLOG_Print("[Exit]\n");
}

STATIC void vmx_act_UpdateCat(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucCat)
{
	Vmx_Instance_t		*pstVmxInst;
	Handle_t 			hStbHandle;
	HUINT32				ulActionId;

	pstVmxInst = xsvc_vmx_main_GetInstance();

	ulActionId = GET_ACTION_ID(hAction);

	HxLOG_Print("[Enter] ulActionId(%d), usUniqueId(%d)\n", ulActionId, usUniqueId);

	if (ulActionId < VMX_CAS_MAX_NUM_OF_ACTION)
	{
		hStbHandle = GET_STBHANDLE(hAction, pstVmxInst->stActionList[ulActionId].hSvc);

		xsvc_vmx_psi_session_UpdateCat(hStbHandle, usUniqueId, pucCat);
	}
	HxLOG_Print("[Exit]\n");
}

STATIC void  vmx_act_ManageInstance(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect)
{
	Vmx_Instance_t		*pstVmxInst;
	HUINT32				ulActionId;
	Handle_t 			hStbHandle;

	pstVmxInst = xsvc_vmx_main_GetInstance();

	ulActionId = GET_ACTION_ID(hAction);

	HxLOG_Print("[Enter] ulActionId(%d)\n", ulActionId);

	if (ulActionId < VMX_CAS_MAX_NUM_OF_ACTION)
	{
		if (pstVmxInst->stActionList[ulActionId].bConnect != bConnect)
		{
			pstVmxInst->stActionList[ulActionId].bConnect = bConnect;
		}

		hStbHandle = GET_STBHANDLE(hAction, pstVmxInst->stActionList[ulActionId].hSvc);

		xsvc_vmx_psi_session_Select(hStbHandle, pstVmxInst->stActionList[ulActionId].bConnect);
	}

	HxLOG_Print("[Exit]\n");
}

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB) && defined(VMX_SVC_TEST)
void vmx_act_bc_StopSessionFromBc(Handle_t hAction, Handle_t hNextSvc)
{
	vmx_act_StopSession(hAction, hNextSvc);
}
#endif

#if defined(VMX_USE_MAIN_MGR_TASK)
STATIC void vmx_act_SendStartSession(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType)
{
	Vmx_Message_t	stMsg;

	stMsg.stStartSession.eMsg 		= eVmxMsg_StartService;
	stMsg.stStartSession.hAction 	= hAction;
	stMsg.stStartSession.hSvc 		= hSvc;
	stMsg.stStartSession.eActType 	= eActType;

	VMX_VR_IPTVH_MGR_SendMessage(&stMsg);
	return;
}

STATIC void vmx_act_SendStopSession(Handle_t hAction, Handle_t hNextSvc)
{
	Vmx_Message_t	stMsg;

	stMsg.stStopSession.eMsg 		= eVmxMsg_StopService;
	stMsg.stStopSession.hAction 	= hAction;
	stMsg.stStopSession.hNextSvc 	= hNextSvc;

	VMX_VR_IPTVH_MGR_SendMessage(&stMsg);
	return;
}

STATIC void vmx_act_SendSetPid(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	Vmx_Message_t	stMsg;

	stMsg.stSetPid.eMsg 		= eVmxMsg_SetPid;
	stMsg.stSetPid.hAction 		= hAction;

	stMsg.stSetPid.stCaDscInfo.ulPidNum = pstCaDscInfo->ulPidNum;
	HxSTD_MemCopy(&stMsg.stSetPid.stCaDscInfo.stPidInfo, pstCaDscInfo->stPidInfo, sizeof(SvcCas_PidInfo_t) * CAS_MAX_NUM_OF_ES);

	VMX_VR_IPTVH_MGR_SendMessage(&stMsg);
	return;
}

void xsvc_vmx_action_StartSession(Vmx_StartSessionMsg_t *pstStartSession)
{
	if (pstStartSession == NULL)
	{
		HxLOG_Error("Invalid parameter! \n");
		return;
	}

	vmx_act_StartSession(pstStartSession->hAction, pstStartSession->hSvc, pstStartSession->eActType);
	return;
}

void xsvc_vmx_action_StopSession(Vmx_StopSessionMsg_t *pstStopSession)
{
	if (pstStopSession == NULL)
	{
		HxLOG_Error("Invalid parameter! \n");
		return;
	}

	vmx_act_StopSession(pstStopSession->hAction, pstStopSession->hNextSvc);
	return;
}

void xsvc_vmx_action_SetPid(Vmx_SetPidMsg_t *pstSetPid)
{
	if (pstSetPid == NULL)
	{
		HxLOG_Error("Invalid parameter! \n");
		return;
	}

	vmx_act_SetPid(pstSetPid->hAction, &pstSetPid->stCaDscInfo);
	return;
}
#endif


HERROR xsvc_vmx_action_Init(void)
{
	Vmx_Instance_t		*pstVmxInst;
	HUINT32				ulSlot = 0;

	HxLOG_Print("[Enter]\n");

	pstVmxInst = xsvc_vmx_main_GetInstance();
#if defined(VMX_USE_MAIN_MGR_TASK)
	pstVmxInst->stCbList.pfnCbStartSession 		= vmx_act_SendStartSession;
	pstVmxInst->stCbList.pfnCbStopSession 		= vmx_act_SendStopSession;
	pstVmxInst->stCbList.pfnCbSetDSC 			= vmx_act_SendSetPid;
#else
	pstVmxInst->stCbList.pfnCbStartSession 		= vmx_act_StartSession;
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB) && defined(VMX_SVC_TEST)
	pstVmxInst->stCbList.pfnCbStopSession 		= vmx_act_bc_StopSession;
#else
	pstVmxInst->stCbList.pfnCbStopSession 		= vmx_act_StopSession;
	pstVmxInst->stCbList.pfnCbSetDSC 			= vmx_act_SetPid;
#endif	// #if defined(CONFIG_MW_CAS_VERIMATRIX_DVB) && defined(VMX_SVC_TEST)
#endif	// #if defined(VMX_USE_MAIN_MGR_TASK)


	pstVmxInst->stCbList.pfnCbUpdatePmt 		= vmx_act_UpdatePmt;
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV) && defined(CONFIG_OP_TDC)
	// TDC는 iptv hybrid라서 cat를 사용하지 않는다 (emm은 oob를 통해 들어온다)
	pstVmxInst->stCbList.pfnCbUpdateCat 		= NULL;
#else
	pstVmxInst->stCbList.pfnCbUpdateCat			= vmx_act_UpdateCat;
#endif
	pstVmxInst->stCbList.pfnCbUpdateCat			= NULL;
	pstVmxInst->stCbList.pfnCbUpdateSdt 		= NULL;
	pstVmxInst->stCbList.pfnCbUpdateNit 		= NULL;
	pstVmxInst->stCbList.pfnCbUpdateEit 		= NULL;
	pstVmxInst->stCbList.pfnCbInstSelected	 	= vmx_act_ManageInstance;
	pstVmxInst->stCbList.pfnCbCheckLastMsg 	= NULL;

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	if(svc_cas_MgrSubRegisterCb(eCasInst_Embedded, eDxCAS_GROUPID_VX, ulSlot, &pstVmxInst->stCbList, 1, &pstVmxInst->hCasInst) != ERR_OK)
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	if(svc_cas_MgrSubRegisterCb(eCasInst_Embedded, eDxCAS_GROUPID_VX, ulSlot, &pstVmxInst->stCbList, 1, &pstVmxInst->hCasInst) != ERR_OK)
#endif
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

