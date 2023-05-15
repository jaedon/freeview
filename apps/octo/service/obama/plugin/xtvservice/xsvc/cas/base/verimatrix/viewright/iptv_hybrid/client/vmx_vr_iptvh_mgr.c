/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  vmx_vr_iptvh_mgr.c
	@brief

	Description:  									\n
	Module: 	OBAMA / TvService				\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
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
#include "svc_si.h"

#include "vmx_vr_iptvh_mgr.h"
#include "vmx_vr_iptvh_dscr.h"
#include "vmx_vr_iptvh_connection.h"
#include "vmx_vr_iptvh_copycontrol.h"
#include "vmx_vr_iptvh_api.h"
#include "vmx_vr_iptvh_ui.h"

#include "_xsvc_cas_vmx_iptv_main.h"
#include "_xsvc_cas_vmx_os.h"
#include "_xsvc_cas_vmx_psi.h"
#include "_xsvc_cas_vmx_iptvh_ui.h"
#include "_xsvc_cas_vmx_action.h"

#if defined(CONFIG_PROD_GCC_GLIBC)
#include <hlib.h>
#include <db_param.h>
#include <hapi.h>
#endif

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
#if defined(VMX_USE_MAIN_MGR_TASK)
STATIC HBOOL		s_bVmxIptvCoreTaskRun = FALSE;
STATIC HULONG 		s_ulVmxIptvTaskMsgQId = 0;
STATIC HULONG 		s_ulVmxIptvCoreTid = 0;
#endif

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#if defined(VMX_USE_MAIN_MGR_TASK)

STATIC void vmx_vr_iptvh_mgr_Task(void)
{
	Vmx_Message_t		stMsg;
	VmxIptv_MsgType_t 	eMsgType = eVmxMsg_Msg_Max;
	HUINT32 			ulResult = 0;

	VMX_memset(&stMsg, 0, sizeof(Vmx_Message_t));

	while (1)
	{
		ulResult = VMX_MSG_Receive(s_ulVmxIptvTaskMsgQId, &stMsg, sizeof(Vmx_Message_t));

		eMsgType = stMsg.eMsg;

		if(ulResult != ERR_OK || eMsgType >= eVmxMsg_Msg_Max)
		{
			HxLOG_Print("GetMessage Error !!! : 0x%X\n", ulResult);
			continue;
		}

		switch (eMsgType)
		{
			case eVmxMsg_ReqBootServerConnection:
				vmx_vr_iptvh_connection_ManageConnection();
				break;

			case eVmxMsg_StartService:
				xsvc_vmx_action_StartSession(&stMsg.stStartSession);
				break;

			case eVmxMsg_StopService:
				xsvc_vmx_action_StopSession(&stMsg.stStopSession);
				break;

			case eVmxMsg_SetPid:
				xsvc_vmx_action_SetPid(&stMsg.stSetPid);
				break;

			default :
				HxLOG_Error("Undefined Message (%d)\n", eMsgType);
				break;
		}
	}

	HxLOG_Print("END OF WHILE\n");

	VMX_MSG_Destroy(s_ulVmxIptvTaskMsgQId);
	return;
}

STATIC void vmx_vr_iptvh_mgr_StartTask(void)
{
	HINT32	lResult = 0;

	if (s_bVmxIptvCoreTaskRun != FALSE)
	{
		HxLOG_Error("Task is already started\n");
		return;
	}

	lResult = (HUINT32)VMX_MSG_Create(VMX_IPTVH_MGR_TASK_MSGQ_SIZE, sizeof(Vmx_Message_t), VMX_IPTVH_MGR_TASK_MSGQ_NAME, &s_ulVmxIptvTaskMsgQId);
	if (lResult != VK_OK)
	{
		HxLOG_Error("fail to create msg queue\n");
		return;
	}

	s_bVmxIptvCoreTaskRun = TRUE;

	// VMX_VR_IPTVH_MGR_Init()에서 이 함수를 호출하면 task가 돌기도 전에 msg를 보내는 경우가 발생해 여기서 호출하도록 함
	vmx_vr_iptvh_connection_Initialize();

	vmx_vr_iptvh_mgr_Task();
	return;
}

STATIC HERROR vmx_vr_iptvh_mgr_CreateTask(void)
{
	HINT32 nErr;

	nErr = VMX_TASK_Create((void*)vmx_vr_iptvh_mgr_StartTask, VMX_IPTVH_MGR_TASK_PRIORITY, VMX_IPTVH_MGR_STACK_SIZE, VMX_IPTVH_MGR_TASK_NAME, NULL, &s_ulVmxIptvCoreTid, 0);
	if (nErr != VK_OK)
	{
		HxLOG_Error("VMX_TASK_Create() failed. err = 0x%x\n", nErr);
		return ERR_FAIL;
	}

	nErr = VMX_TASK_Start(s_ulVmxIptvCoreTid);
	if (nErr != VK_OK)
	{
		HxLOG_Error("VMX_TASK_Start() failed. err = 0x%x\n", nErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

STATIC void vmx_vr_iptvh_mgr_InitContext(Vmx_ActionTable_t *pAct)
{
	HxLOG_Print("[Enter]\n");

	if(pAct == NULL)
	{
		HxLOG_Error("\n");
		return;
	}

	pAct->ucVmxServiceIdx				= 0;
	pAct->pContext						= NULL;
	pAct->bServiceStarted				= FALSE;
	pAct->eEncrAlgo						= eEncAlg_null;
	pAct->stDecryptType 				= eVmxIPtv_Decrypt_HW;
	pAct->stCbList.pfnCbDecodeFail 		= NULL;
	pAct->stCbList.pfnCbUserData 		= NULL;
	pAct->stCbList.pfnCbCW 				= NULL;
	pAct->stCbList.pfnCbFingerPrinting 	= NULL;
	pAct->stCbList.pfnCbCopyControl 	= NULL;
	pAct->stCbList.pfCbOsdMsg 			= NULL;
	pAct->stCbList.pfCbOpaqueData 		= NULL;

	HxLOG_Print("[Exit]\n");
}

STATIC HERROR vmx_vr_iptvh_mgr_RegisterCallbackList(Vmx_ActionTable_t *pAct)
{
	int    all_algorithms[1] = { -1 };

	HxLOG_Print("[Enter]\n");

	if(pAct == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	pAct->stCbList.pfnCbDecodeFail 		= vmx_vr_iptvh_ui_DecodeCallback;
	pAct->stCbList.pfnCbUserData 		= vmx_vr_iptvh_psi_mgr_UserDataCallback;
	pAct->stCbList.pfnCbCW 				= vmx_vr_iptvh_dscr_ControlWordCallback;
	pAct->stCbList.pfnCbFingerPrinting 	= vmx_vr_iptvh_ui_FingerPrintingCallback;
	pAct->stCbList.pfnCbCopyControl 	= vmx_vr_iptvh_copycontrol_CopycontrolCallback;
	pAct->stCbList.pfCbOsdMsg 			= vmx_vr_iptvh_ui_OSDMessgaeCallback;
	pAct->stCbList.pfCbOpaqueData 		= vmx_vr_iptvh_psi_mgr_OpaqueDataCallback;

	vmx_vr_iptvh_api_RegisterControlWordCallback(pAct->pContext, pAct->stCbList.pfnCbCW, (void *)pAct, 1, all_algorithms);
	vmx_vr_iptvh_api_RegisterDecodeFailureCb(pAct->pContext, pAct->stCbList.pfnCbDecodeFail, (void *)pAct);
	vmx_vr_iptvh_api_RegisterFingerPrintingCb(pAct->pContext, pAct->stCbList.pfnCbFingerPrinting, (void *)pAct);
	vmx_vr_iptvh_api_RegisterOsdMessageCb(pAct->stCbList.pfCbOsdMsg, (void *)pAct);							// 여기에 할지 다시 확인필요
	vmx_vr_iptvh_api_RegisterCopyControlCb(pAct->pContext, pAct->stCbList.pfnCbCopyControl, (void *)pAct);
	vmx_vr_iptvh_api_RegisterIrdOpaqueDataCb(pAct->stCbList.pfCbOpaqueData, (void *)pAct);					// 여기에 할지 다시 확인필요
	vmx_vr_iptvh_api_RegisterUserDataCb(pAct->pContext, pAct->stCbList.pfnCbUserData, (void *)pAct);

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

STATIC HERROR vmx_vr_iptvh_mgr_UnRegisterCallbackList(Vmx_ActionTable_t *pAct)
{
	int    all_algorithms[1] = { -1 };

	HxLOG_Print("[Enter]\n");
	if(pAct == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	pAct->stCbList.pfnCbDecodeFail 		= NULL;
	pAct->stCbList.pfnCbUserData 		= NULL;
	pAct->stCbList.pfnCbCW 				= NULL;
	pAct->stCbList.pfnCbFingerPrinting 	= NULL;
	pAct->stCbList.pfnCbCopyControl 	= NULL;
	pAct->stCbList.pfCbOsdMsg 			= NULL;
	pAct->stCbList.pfCbOpaqueData 		= NULL;

	vmx_vr_iptvh_api_RegisterControlWordCallback(pAct->pContext, NULL, (void *)pAct, 1, all_algorithms);
	vmx_vr_iptvh_api_RegisterDecodeFailureCb(pAct->pContext, NULL, (void *)pAct);
	vmx_vr_iptvh_api_RegisterFingerPrintingCb(pAct->pContext, NULL, (void *)pAct);
	vmx_vr_iptvh_api_RegisterOsdMessageCb(NULL, (void *)pAct);
	vmx_vr_iptvh_api_RegisterCopyControlCb(pAct->pContext, NULL, (void *)pAct);
	vmx_vr_iptvh_api_RegisterIrdOpaqueDataCb(NULL, (void *)pAct);
	vmx_vr_iptvh_api_RegisterUserDataCb(pAct->pContext, NULL, (void *)pAct);

	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}

void vmx_vr_iptvh_mgr_SetNetworkTime(void)
{
	// glibc를 사용하면 localtime을 사용해야 한다
#if defined(CONFIG_PROD_GCC_GLIBC) && defined(CONFIG_MW_SI_LOCALTIME)
	{
		HERROR			hErr = ERR_FAIL;
		HxCountry_e 	eCountryId = eCountryID_NONE;
		HUINT32 		ulRegionId = 0;

		// 예외처리 - /tmp/localtime 이 없으면 만든다
		if (HLIB_FILE_Exist(LOCALTIME_NAME_PATH_TMP) != TRUE)
		{
			VK_SYSTEM_Command(LOCALTIME_COPY_CMD);
		}

		// data dir에 countryId와 region을 보고 localtime link를 만든다
		// si를 받아도 만들어지지만 cas쪽에서는 ntp를 사용하기 위해 먼저 설정이 되어 있어야 한다
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
		}

		hErr = HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID, &ulRegionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID) Error!!\n");
			ulRegionId = 0;
		}

		(void)SVC_SI_UpdateLocaltime_linkTzDB(eCountryId, ulRegionId);
	}
#endif

	HxLOG_Error("============================================================================\n");
	HxLOG_Error("Verimatrix IPTV Hybrid Library version(%s) \n", vmx_vr_iptvh_api_GetVersionString());
	HxLOG_Error("============================================================================\n");

	// connection시에도 시간 업데이트 체크는 하지만 시간이 걸릴 수 있으니 여기에서 미리 해본다
	vmx_vr_iptvh_connection_UpdateTime();

	return;
}

#define ________PUBLIC_FUNCTION______________________________________________________________________
#if defined(VMX_USE_MAIN_MGR_TASK)
HERROR VMX_VR_IPTVH_MGR_SendMessage(Vmx_Message_t *pstMsg)
{
	if (pstMsg == NULL)
	{
		HxLOG_Error("NULL Message! \n");
	}

	if (s_bVmxIptvCoreTaskRun)
	{
		VMX_MSG_Send(s_ulVmxIptvTaskMsgQId, pstMsg, sizeof(Vmx_Message_t));
	}
	else
	{
		HxLOG_Error("Vmx core task not running yet!! \n");
	}

	return ERR_OK;
}
#endif

HERROR VMX_VR_IPTVH_MGR_RegisterCallbackList(Vmx_ActionTable_t *pAct)
{
	return vmx_vr_iptvh_mgr_RegisterCallbackList(pAct);
}

HERROR VMX_VR_IPTVH_MGR_UnRegisterCallbackList(Vmx_ActionTable_t *pAct)
{
	return vmx_vr_iptvh_mgr_UnRegisterCallbackList(pAct);
}

void VMX_VR_IPTVH_MGR_InitContext(Vmx_ActionTable_t *pAct)
{
	vmx_vr_iptvh_mgr_InitContext(pAct);
}

HERROR VMX_VR_IPTVH_MGR_Init(void)
{
#if defined(VMX_USE_MAIN_MGR_TASK)
	if(vmx_vr_iptvh_mgr_CreateTask() != ERR_OK)
	{
		HxLOG_Error("vmx_vr_iptvh_mgr_CreateTask() : error\n");
	}
#endif

	vmx_vr_iptvh_mgr_SetNetworkTime();

	if(vmx_vr_iptvh_ui_Init() != ERR_OK)
	{
		HxLOG_Error("vmx_vr_iptvh_ui_Init() : error\n");
	}

	if(vmx_vr_iptvh_ecm_Init() != ERR_OK)
	{
		HxLOG_Error("vmx_vr_iptvh_ecm_Init() : error\n");
	}

#if 1//sem err발생하여 다시 enable, 정리할것임 !  // !defined(CONFIG_OP_TDC)	// TDC는 emm이 OOB로 들어오는데 cas lib이 알아서 업데이트 한다고 한다. RF에서 처리할 필요 없음
	if(vmx_vr_iptvh_emm_Init() != ERR_OK)
	{
		HxLOG_Error("vmx_vr_iptvh_emm_Init() : error\n");
	}
#endif

	if(vmx_vr_iptvh_psi_mgr_Init() != ERR_OK)
	{
		HxLOG_Error("vmx_vr_iptvh_psi_mgr_Init() : error\n");
	}

	// task 사용할 때엔 async 문제를 방지하기 위해 이 함수를 startTask에서 호출한다
#if !defined(VMX_USE_MAIN_MGR_TASK)
	vmx_vr_iptvh_connection_Initialize();
#endif

	return ERR_OK;
}

/* end of file */
