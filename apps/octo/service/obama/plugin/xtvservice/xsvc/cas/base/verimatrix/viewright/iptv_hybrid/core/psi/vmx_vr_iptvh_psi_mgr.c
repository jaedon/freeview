/* (c) 2011-2015 Humax Co., Ltd.
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
	@file     vmx_vr_iptvh_psi_mgr.c
	@brief    vmx_vr_iptvh_psi_mgr.c (Verimatrix send Data to vmx)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include "bc_consts.h"

#include "_xsvc_cas_vmx_psi.h"
#include "_xsvc_cas_vmx_os.h"
#include "_xsvc_cas_vmx_dscr.h"
#include "_xsvc_cas_vmx_iptv_main.h"

#include "vmx_vr_iptvh_mgr.h"
#include "vmx_vr_iptvh_api.h"
#include "vmx_vr_iptvh_connection.h"

#include "rlib.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

// server에서 주는 default lv은 5이지만 로그가 많아서 3으로 낮춘다
#define VMX_DEFAULT_LOG_LEVEL		3


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
STATIC HULONG 		s_ulVmxIptvPsiMgrTid;
/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/
// EMM, ECM이 존재하면 무조건 Vmx lib로 전달한다.
STATIC void iptvh_psi_mgr_StartTask(void)
{
	while(1)
	{
		//## ECM 처리가 최우선이므로 ecm 큐에 뭐가 있나 보고 있으면 처리
		// 메시지가 ecm 메시지 였으면 다음 메시지를 얻어온다
		if (vmx_vr_iptvh_ecm_IsQueueEmpty() == FALSE)
		{
			vmx_vr_iptvh_ecm_SendEcmMsgToVm();
		}

		vmx_vr_iptvh_emm_SendEmmMsgToVm(0);

		VMX_TASK_Sleep(10);
	}
}

STATIC HERROR iptvh_psi_mgr_init(void)
{
	int nErr;
	nErr = VMX_TASK_Create((void*)iptvh_psi_mgr_StartTask, VMX_PSI_TASK_PRIORITY, VMX_PSI_STACK_SIZE, VMX_PSI_TASK_NAME, NULL, &s_ulVmxIptvPsiMgrTid, 0);

	if (!nErr)
	{
		nErr = VMX_TASK_Start(s_ulVmxIptvPsiMgrTid);
	}

	return nErr;
}

STATIC HERROR iptvh_psi_mgr_DestoryChannelCtx(Handle_t hSTBSession)
{
	Vmx_Instance_t 		*pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t 	*pAct = NULL;
	HUINT32 			 ulActionId = GET_ACTION_ID_FROM_STBHANDLE(hSTBSession);

	HxLOG_Print("[Enter]\n");

	if (pstInst == NULL)
	{
		HxLOG_Error("Error! xsvc_vmx_main_GetInstance() failed! \n");
		return ERR_FAIL;
	}

	if (ulActionId >= VMX_CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("\r ulActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	pAct = &pstInst->stActionList[ulActionId];
	if (pAct == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if (vmx_vr_iptvh_connection_GetInitFlag() == FALSE)
	{
		HxLOG_Error("\r\n");
//		return;
	}

	if (pAct->bServiceStarted == FALSE)
	{
		HxLOG_Error("Already, bServiceStarted was stoped \n");
		return ERR_FAIL;
	}

	VMX_VR_IPTVH_MGR_UnRegisterCallbackList(pAct);

	HxLOG_Print("actionId[%d], svcidx(0x%x)\n", ulActionId, pAct->ucVmxServiceIdx);

	pAct->ucVmxServiceIdx	= 0;
	pAct->pContext				= NULL;
	pAct->bServiceStarted		= FALSE;
	pAct->eEncrAlgo			= eEncAlg_null;

#ifdef CONFIG_DEBUG
	xsvc_vmx_main_PrintContext();
#endif
	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}

HERROR vmx_vr_iptvh_psi_mgr_CreateChannelCtx(Handle_t hSTBSession)
{
	Vmx_Instance_t 		*pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t 	*pAct = NULL;
	HUINT32 			 ulActionId = GET_ACTION_ID_FROM_STBHANDLE(hSTBSession);

	HxLOG_Print("[Enter]\n");

	if (pstInst == NULL)
	{
		HxLOG_Error("Error! xsvc_vmx_main_GetInstance() failed! \n");
		return ERR_FAIL;
	}

	if (ulActionId >= VMX_CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("Error! out of range. ulActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	pAct = &pstInst->stActionList[ulActionId];
	if (pAct == NULL)
	{
		HxLOG_Error("Error! Null action list! ulActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	if (vmx_vr_iptvh_connection_GetInitFlag() == FALSE)
	{
		HxLOG_Error("\r\n");
//		return;
	}

	// 아래 내용은 적용할지 말지 추후 결정하자.
	if (pAct->bServiceStarted == TRUE)
	{
		HxLOG_Error("Already, bServiceStarted was started \n");
		return ERR_FAIL;
	}

	if (vmx_vr_iptvh_api_CreateContext(NULL, 1, &pAct->pContext) != ERR_OK)
	{
		HxLOG_Error("\r\n");
	}

	// ucVmxServiceIdx는 xsvc_vmx_main_GetServiceIdx()에서 할당해주니 여기서는 아무것도 하지 말자
	HxLOG_Print("ulActionId[%d], svcidx(0x%x)\n", ulActionId, pAct->ucVmxServiceIdx);

	VMX_VR_IPTVH_MGR_RegisterCallbackList(pAct);

	if(vmx_vr_iptvh_api_ResetStream(pAct->pContext) != ERR_OK)
	{
		HxLOG_Error("Error! vmx_vr_iptvh_api_ResetStream() is not ok\n");
	}

	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}

void vmx_vr_iptvh_psi_mgr_UserDataCallback(void *pUserParam, int userdataLength, void *pUserDataValues)
{
	Vmx_Instance_t *pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t *pstAction = NULL;
	Vmx_ActionTable_t *pAct = NULL;
	HUINT16 i;

	HxLOG_Print("[Enter]\n");

	if(pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	if(pUserDataValues == NULL || userdataLength == 0)
	{
		HxLOG_Error("\r\n");
		return;
	}

	if(pUserParam)
	{
		HxLOG_Print("UserParam is exist\n");
		pstAction = (Vmx_ActionTable_t *)pUserParam;
		for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
		{
			pAct = &pstInst->stActionList[i];
			if(pAct == NULL)
			{
				continue;
			}
			if(pstAction == pAct)
			{
				break;
			}
		}
	}

	// TODO
	HxLOG_Print("[Exit]\n");
}

void vmx_vr_iptvh_psi_mgr_OpaqueDataCallback(void*pUserParam, unsigned int nDataSize, void *pIrdOpaqueData)
{
	Vmx_Instance_t *pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t *pstAction = NULL;
	Vmx_ActionTable_t *pAct = NULL;
	HUINT16 i;

	HxLOG_Print("[Enter]\n");

	if(pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	if(pIrdOpaqueData == NULL || nDataSize == 0)
	{
		HxLOG_Error("\r\n");
		return;
	}

	if(pUserParam)
	{
		HxLOG_Print("UserParam is exist\n");
		pstAction = (Vmx_ActionTable_t *)pUserParam;
		for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
		{
			pAct = &pstInst->stActionList[i];
			if(pAct == NULL)
			{
				continue;
			}
			if(pstAction == pAct)
			{
				break;
			}
		}
	}

	// TODO
	HxLOG_Print("[Exit]\n");
}


void vmx_vr_iptvh_psi_mgr_SetOverrideRatingLevelToVm(Handle_t hSTBSession)
{
	Vmx_Instance_t *pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t *pAct = NULL;
	HINT32 nActionId = GET_ACTION_ID_FROM_STBHANDLE(hSTBSession);
	HUINT8 ucCurrentStbRatingLevel = 255;

	HxLOG_Print("[Enter]\n");

	if(pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	if(nActionId >= VMX_CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("\r nActionId(%d)\n", nActionId);
		return;
	}

	pAct = &pstInst->stActionList[nActionId];
	if(pAct == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	if(pAct->pContext != NULL)
	{
		if(vmx_vr_iptvh_api_OverrideRatingLevel(pAct->pContext, &ucCurrentStbRatingLevel) != ERR_OK)
		{
			HxLOG_Error("vmx_vr_iptvh_api_StopService() is not ok\n");
		}
	}

	HxLOG_Print("nActionId(%d), ucCurrentStbRatingLevel(%d)\n", nActionId, ucCurrentStbRatingLevel);
	HxLOG_Print("[Exit]\n");
}


void vmx_vr_iptvh_psi_mgr_StartService(Handle_t hStbHandle)
{
	Vmx_Instance_t 		*pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t 	*pAct = NULL;
	HUINT32 			 ulActionId = GET_ACTION_ID_FROM_STBHANDLE(hStbHandle);
	HUINT32 			 ulPidNum = xsvc_vmx_psi_session_GetPidNum(hStbHandle);
	HUINT16 			*pPidList = xsvc_vmx_psi_session_GetPidList(hStbHandle);

	HUINT32				 i;
	HUINT8				 ucNewPidNum;
	HUINT16 			 ausNewPidList[VMX_MAX_NUM_OF_ESPID];
	HUINT32 			 ulDemuxId;
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Print("[Enter] aid(%d)\n", ulActionId);

	if (pstInst == NULL)
	{
		HxLOG_Error("Error! xsvc_vmx_main_GetInstance() fail\n");
		return;
	}

	if (ulActionId >= VMX_CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("Error! invalid ulActionId(%d)\n", ulActionId);
		return;
	}

	pAct = &pstInst->stActionList[ulActionId];
	if (pAct == NULL)
	{
		HxLOG_Error("Error! null action list! actionId(%d)\n", ulActionId);
		return;
	}

	if (vmx_vr_iptvh_connection_GetInitFlag()==FALSE)
	{
		HxLOG_Error("Error! Not connection initialized yet! \n");
		//return;
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("=============== StartServcie -> ulActionId(%d)\n", ulActionId);
	HxLOG_Print("=============== StartServcie -> bServiceStarted(%d)\n",  pAct->bServiceStarted);
	HxLOG_Print("=============== StartServcie -> pContext(0x%08x)\n", pAct->pContext);
	HxLOG_Print("=============== StartServcie -> ucVmxServiceIdx(0x%x)\n", pAct->ucVmxServiceIdx);
	HxLOG_Print("=============== StartServcie -> ulPidNum(%d)\n", ulPidNum);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_IPTVH_MGR, "PidList", pPidList, ulPidNum);
#endif

	// setPid에는 설정되었으나 실제로 해당 pid가 ecm pid를 가지지 않는 경우엔 start에서 제외한다 (이미 ecm pid가 없어서 desc open도 안 됨)
	// 그렇지 않으면 descrambler open도 안 된 pid를 start하게 되어 오동작이 발생할 수 있다
	ucNewPidNum = 0;
	HxSTD_MemSet(ausNewPidList, 0x00, sizeof(ausNewPidList));
	for (i = 0; i < ulPidNum; i++)
	{
		if ((pPidList[i] != PID_NULL)
			&& xsvc_vmx_psi_stream_GetEcmHandleByEsPid(pPidList[i]) != HANDLE_NULL)
		{
			ausNewPidList[ucNewPidNum] = pPidList[i];
			ucNewPidNum++;
		}
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("================= StartServcie -> ucNewPidNum(%d)\n", ucNewPidNum);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_IPTVH_MGR, "ausNewPidList", ausNewPidList, ucNewPidNum);
#endif

	// start service가 내려왔을 때 설정된 pid가 없다면 vmx_vr_iptvh_api_StartService()를 부르지 않는다
	// set pid가 되면 pid가 생길 것이고 그때 시작해야 한다
	if (ucNewPidNum == 0)
	{
		HxLOG_Error("no pid list. skip vmx start service. actionId(%d) hSvc(0x%X)\n", ulActionId, pAct->hSvc);
		return;
	}

	// 아래 내용은 적용할지 말지 추후 결정하자.
	if (pAct->bServiceStarted == TRUE)
	{
		HxLOG_Error("Already, bServiceStarted was started \n");
		return;
	}

	if (pAct->pContext == NULL)
	{
		// vmx context 생성전이면 만들어준다
		hErr = vmx_vr_iptvh_psi_mgr_CreateChannelCtx(hStbHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error! vmx_vr_iptvh_psi_mgr_CreateChannelCtx() failed. \n");
			return;
		}

		if (pAct->pContext == NULL)
		{
			HxLOG_Error("Error! null context. actionId(%d) \n", ulActionId);
			return;
		}
	}

	// vmx lib에서 맞는 stream handle을 가져갈 수 있도록 start service 하기 전에 demux id를 셋팅해준다
	if (svc_cas_SubSvcRmGetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId) == ERR_OK)
	{
		xsvc_vmx_dscr_SetDemuxId(ulDemuxId);
	}
	else
	{
		HxLOG_Error("svc_cas_SubSvcRmGetResourceId() failed! actionId(%d) \r\n", ulActionId);
	}

	if (vmx_vr_iptvh_api_StartService(pAct->pContext, pAct->ucVmxServiceIdx, ucNewPidNum, ausNewPidList) != ERR_OK)
	{
		HxLOG_Error("vmx_vr_iptvh_api_StartService() Error! \n");
	}
	else
	{
		pAct->bServiceStarted = TRUE;
	}

	HxLOG_Print("[Exit]\n");
	return;
}

void vmx_vr_iptvh_psi_mgr_StopService(Handle_t hSTBSession)
{
	Vmx_Instance_t 		*pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t 	*pAct = NULL;
	HUINT32 			 ulActionId = GET_ACTION_ID_FROM_STBHANDLE(hSTBSession);

	HxLOG_Print("[Enter]\n");

	if(pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	if (ulActionId >= VMX_CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("\r ulActionId(%d)\n", ulActionId);
		return;
	}

	pAct = &pstInst->stActionList[ulActionId];
	if (pAct == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("=============== StartStop -> ulActionId(%d)\n", ulActionId);
	HxLOG_Print("=============== StartStop -> ucVmxServiceIdx(0x%x)\n", pAct->ucVmxServiceIdx);
	HxLOG_Print("=============== StartStop -> bServiceStarted(%d)\n", pAct->bServiceStarted);
#endif

	if (pAct->bServiceStarted == TRUE)
	{
		if (pAct->pContext != NULL)
		{
#if !defined(VMX_CREATE_ALL_CONTEXT_ON_BOOT)
			iptvh_psi_mgr_DestoryChannelCtx(hSTBSession);
#endif
			if (vmx_vr_iptvh_api_StopService(pAct->pContext, pAct->ucVmxServiceIdx) != ERR_OK)
			{
				HxLOG_Error("vmx_vr_iptvh_api_StopService() is not ok\n");
			}

			if (vmx_vr_iptvh_api_ResetStream(pAct->pContext) != ERR_OK)
			{
				HxLOG_Error("vmx_vr_iptvh_api_ResetStream() is not ok\n");
			}
		}
		pAct->bServiceStarted = FALSE;
	}

	HxLOG_Print("[Exit]\n");
	return;
}

void vmx_vr_iptvh_psi_mgr_CreateAllChannelCtx(void)
{
	Vmx_Instance_t *pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t *pAct = NULL;
	HUINT16 i;

	HxLOG_Print("[Enter]\n");

	if(pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	if(vmx_vr_iptvh_connection_GetInitFlag()==FALSE)
	{
		HxLOG_Error("\r\n");
//		return;
	}

	// VMCreateContext ->  VMRegisterControlwordCallback -> VMResetStream 순서로 초기화해야 한다
	// ENG-212_Addendum_ViewrightSTBforIPTV-3.5_advanced_security_integration_for_Broadcom.pdf 참고
	for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
	{
		pAct = &pstInst->stActionList[i];

		if(pAct == NULL)
		{
			continue;
		}

		if(vmx_vr_iptvh_api_CreateContext(NULL, 1, &pAct->pContext) != ERR_OK)
		{
			HxLOG_Error("\r\n");
			continue;
		}

		pAct->ucVmxServiceIdx				= 0x00;					// default로 0으로 초기화하고 나중에 start할 때 xsvc_vmx_main_GetServiceIdx()로 할당받아 사용한다
		pAct->stDecryptType 				= eVmxIPtv_Decrypt_HW; 	// default로 hardware decryption으로 설정한다.

		VMX_VR_IPTVH_MGR_RegisterCallbackList(pAct);

		if(vmx_vr_iptvh_api_ResetStream(pAct->pContext) != ERR_OK)
		{
			HxLOG_Error("vmx_vr_iptvh_api_ResetStream() is not ok\n");
		}
	}

#ifdef CONFIG_DEBUG
	xsvc_vmx_main_PrintContext();
#endif

	return;
}

HERROR vmx_vr_iptvh_psi_mgr_Init(void)
{
	return iptvh_psi_mgr_init();
}

/* end of file */
