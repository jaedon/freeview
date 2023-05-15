/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  vmx_vr_iptvh_connection.c
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
#include "VMClient.h"
#include "_xsvc_cas_vmx_iptv_main.h"
#include "vmx_vr_iptvh_mgr.h"
#include "vmx_vr_iptvh_api.h"
#include "vmx_vr_iptvh_connection.h"

#include "tapi.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
/* input parameter for VCAS */
#if defined(CONFIG_OP_TDC)
/* TDC */
#define VMXIPTV_DEAULT_BOOT_IPADDRESS			"vkey-dvb.cdn.yousee.tv" //"80.62.121.91"
#define VMXIPTV_DEFAULT_BOOT_PORT				12686
#define VMXIPTV_DEFAULT_VCAS_VERSION			eVcasVersion_340
#define VMXIPTV_DEFAULT_STORE_FILE				"/var/lib/humaxtv/Verimatrix.store"
#else
/* default - veramatrix public vcas server */
#define VMXIPTV_DEAULT_BOOT_IPADDRESS			"public2.verimatrix.com"
#define VMXIPTV_DEFAULT_BOOT_PORT				12686
#define VMXIPTV_DEFAULT_VCAS_VERSION			eVcasVersion_340
#define VMXIPTV_DEFAULT_STORE_FILE				"/var/lib/humaxtv/Verimatrix.store" // test only
#endif

#define VMXIPTV_BOOT_IPADDRESS_MAX_LEN					(50)
#define VMXIPTV_STORE_PATH_MAX_LEN						(50)
#define VMXIPTV_BOOTSERVER_RETRY_COUNTER 				(5)
#define VMXIPTV_BOOTSERVER_RETRY_TIMEOUT				(30 * 1000)	// 1000 = 1sec
#define VMXIPTV_BOOTSERVER_RETRY_TIMEOUT_OFFLINE		(60 * 1000)	// 60 sec, offline인 경우엔 조금 더 오래 기다린다


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eVmxIptv_ContextStatus_NotInitialized = 0,
	eVMXIPTV_ContextStatus_Offline,
	eVMXIPTV_ContextStatus_Online
} VmxIptv_ConnectionStatus_e;

typedef struct
{
	HBOOL						bConnected;
	HUINT8 						aucBootIpAddress[VMXIPTV_BOOT_IPADDRESS_MAX_LEN];
	HUINT16						usBootPort;
	HUINT8 						aucStorePath[VMXIPTV_STORE_PATH_MAX_LEN];
	eVcasVersion_t 				eVcasVersion;
	VmxIptv_ConnectionStatus_e	eVmxContextStatus;
	HUINT16						usTmpRetryCount;	// 임시로 만듬. eMEVT_COLDBOOT_NOTIFY_DONE 이벤트 받을 때 까지 retry 하는 것으로 바꾸면 해당 변수 삭제.

/* should be removed
	// 만들어 놓고 사용하지 않을 거면 만들지 말자.. 일단 막아두고 나중에 삭제
	void* 						pVmxContext;	// Connection 만들어 진 후 처음 생성되는 Verimatrix Context. 생성만 하고 사용되지는 않음. (각 Service를 위해서는 새로 Context가 만들어짐)
*/
} VmxIptv_ConnectContext_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC VmxIptv_ConnectContext_t		s_stVmxConnectContext;
STATIC HERROR iptvh_connection_ManageConnection(void);

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

// Local function
#define __________LOCAL_FUNCTION__________

STATIC void iptvh_connection_UpdateTime(void)
{
	UNIXTIME		ulUnixTime = HLIB_STD_GetSystemTime();

	if (HLIB_DATETIME_IsValidUnixTime(ulUnixTime) != ERR_OK)
	{
#if defined(CONFIG_OP_TDC)
#if defined(USE_NTP_COMMAND_TOOL)
		// use ntpclient
		HCHAR pszCommandStr[512];

		HxSTD_MemSet(pszCommandStr, 0x00, sizeof(pszCommandStr));
		snprintf(pszCommandStr, sizeof(pszCommandStr), "ntpclient -s -h dk.pool.ntp.org");
		VK_SYSTEM_Command(pszCommandStr);
#else
		if (TAPI_SetNTP(TDC_NTP_SERVER_ADDR) != ERR_OK)
		{
			HxLOG_Error("TAPI_SetNTP() failed..!");
		}
#endif
#endif
	}

	return;
}

STATIC HERROR iptvh_connection_CheckNetworkAvailable(HBOOL *bNetworkConnected)
{
	// STB가 현재 IP address를 정상적으로 얻어 왔는지 check 하는 function.
	// ip address가 정상적으로 set 되어 있는 경우라면 bNetworkConnected = TRUE, 그렇지 않은 경우라면 bNetworkConnected = FALSE...


	// TODO: IP address 정상적으로 할당 되었는지 check 후 bNetworkConnected 값 셋팅.
#if defined(CONFIG_OP_TDC)
	UNIXTIME		ulUnixTime = HLIB_STD_GetSystemTime();

	// ntp를 이용해 보자
	if (HLIB_DATETIME_IsValidUnixTime(ulUnixTime) != ERR_OK)
	{
#if defined(USE_NTP_COMMAND_TOOL)
		// use ntpclient
		HCHAR pszCommandStr[512];

		HxSTD_MemSet(pszCommandStr, 0x00, sizeof(pszCommandStr));
		snprintf(pszCommandStr, sizeof(pszCommandStr), "ntpclient -s -h dk.pool.ntp.org");
		VK_SYSTEM_Command(pszCommandStr);
#else
		if (TAPI_SetNTP(TDC_NTP_SERVER_ADDR) != ERR_OK)
		{
			HxLOG_Error("TAPI_SetNTP() failed..!");
		}
#endif
		*bNetworkConnected = FALSE;
	}
	else
	{
		*bNetworkConnected = TRUE;
	}
#else
	*bNetworkConnected = TRUE;
#endif

	return ERR_OK;
}

STATIC VmxIptv_ConnectContext_t *iptvh_connection_GetInstance(void)
{
	return &s_stVmxConnectContext;
}

/* should be removed
STATIC void * iptvh_connection_GetContext(void)
{
	return s_stVmxConnectContext.pVmxContext;
}
*/

STATIC HERROR iptvh_connection_SetConnectionStatus(VmxIptv_ConnectContext_t *pstConnContext, VmxIptv_ConnectionStatus_e eConnectType)
{
	HERROR 		hErr = ERR_FAIL;

	if(pstConnContext->eVmxContextStatus == eVmxIptv_ContextStatus_NotInitialized)
	{
		if (eConnectType == eVMXIPTV_ContextStatus_Offline)
		{
			// Set Config as offline mode
			hErr = vmx_vr_iptvh_api_SetConfigOffline(pstConnContext->aucStorePath);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("vmx_vr_iptvh_api_SetConfigOffline() failed..\n");
			}
		}

#if defined(VMX_CREATE_ALL_CONTEXT_ON_BOOT)
		// create context
		vmx_vr_iptvh_psi_mgr_CreateAllChannelCtx();
#endif
	}

	switch(eConnectType)
	{
		case eVMXIPTV_ContextStatus_Offline:
			// offline 으로 connection 만듬에 따라 status 및 timer 조절 함.
			pstConnContext->bConnected = FALSE;
			pstConnContext->eVmxContextStatus = eVMXIPTV_ContextStatus_Offline;
			break;

		case eVMXIPTV_ContextStatus_Online:
			// online 으로 connection 만듬에 따라 status 및 timer 조절 함.
			pstConnContext->bConnected = TRUE;
			pstConnContext->eVmxContextStatus = eVMXIPTV_ContextStatus_Online;
			// 이미 online 상태면 더이상 시도해 보지 않아도 됨.
			break;

		default:
			HxLOG_Error("ERROR... Invalid Connection Type : [%d]...\n", eConnectType);
			break;
	}

	return ERR_OK;
}

STATIC void iptvh_connection_SendMsg_ReqBootServerConnect(void)
{
	Vmx_Message_t stMsg;

	stMsg.eMsg = eVmxMsg_ReqBootServerConnection;
	VMX_VR_IPTVH_MGR_SendMessage(&stMsg);
	return;
}

STATIC HERROR iptvh_connection_ManageConnection(void)
{
	VmxIptv_ConnectContext_t	*pstConnContext;
	HULONG						ulTimerID = 0;
	HERROR 						hErr = ERR_FAIL;
	HINT32						lVkRet = VK_ERROR;
//	HBOOL 						bNetworkConnected = FALSE; 	// check network available...
//	UNIXTIME					ulUnixTime = 0;

	HxLOG_Trace();

	// Get Connection Context
	pstConnContext = iptvh_connection_GetInstance();

#if defined(VMX_USE_MAIN_MGR_TASK)
	if(pstConnContext->eVmxContextStatus == eVMXIPTV_ContextStatus_Online)
	{
		// do nothing
		return ERR_OK;
	}

	// check time
	iptvh_connection_UpdateTime();

	//  connect to boot server
	hErr = vmx_vr_iptvh_api_SetConfig(pstConnContext->aucStorePath, pstConnContext->aucBootIpAddress, pstConnContext->usBootPort);
	if (hErr != ERR_OK)
	{
		// boot server connection failed..  나중에 다시 시도해본다
		hErr = iptvh_connection_SetConnectionStatus(pstConnContext, eVMXIPTV_ContextStatus_Offline);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("iptvh_connection_SetConnectionStatus() failed! \n");
		}

		// Set retry Timer
		HxLOG_Error("Verimatrix : connection failed.. set retry timer now ! %d ms\n", VMXIPTV_BOOTSERVER_RETRY_TIMEOUT);
		lVkRet = VK_TIMER_EventAfter(VMXIPTV_BOOTSERVER_RETRY_TIMEOUT, (void*)iptvh_connection_SendMsg_ReqBootServerConnect, NULL, 0, &ulTimerID);
		if (lVkRet != VK_OK)
		{
			HxLOG_Error("VK_TIMER_EventAfter() failed..!! \n");
			return ERR_FAIL;
		}
	}
	else
	{
		// boot server 접속 성공
		hErr = iptvh_connection_SetConnectionStatus(pstConnContext, eVMXIPTV_ContextStatus_Online);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("iptvh_connection_SetConnectionStatus() failed! \n");
		}
	}
#else
	if (iptvh_connection_CheckNetworkAvailable(&bNetworkConnected) == ERR_FAIL)
	{
		HxLOG_Error("ERROR... at iptvh_connection_CheckNetworkAvailable()\n");
		return ERR_FAIL;
	}

	// if Network is available... (IP address is assigned)
	if(bNetworkConnected == TRUE)
	{
		if(pstConnContext->eVmxContextStatus == eVMXIPTV_ContextStatus_Online)
		{
			// 이미 Verimatrix Context가 Online으로 생성되어 있는 상태
			// Do Nothing...
		}
		else
		{
			// TODO: eMEVT_COLDBOOT_NOTIFY_DONE 전달 받을 때까지 online connection 시도하도록 변경 필요
			// STB가 정상적으로 BOOT 완료 될 때까지 online connection을 시도하고, 그 이후에도 online connection이 이뤄지지 않으면 offline으로 context를 생성함.
			// SetConfig는 STB가 현재시간을 획득하여야 정상적으로 실행 됨
			// 일단 retry count로 online connection 재시도를 시도 함. 추후에 eMEVT_COLDBOOT_NOTIFY_DONE 이벤트 발을 때까지 시도하는 것으로 변경 필요.
			if (vmx_vr_iptvh_api_SetConfig(pstConnContext->aucStorePath, pstConnContext->aucBootIpAddress, pstConnContext->usBootPort) == ERR_OK)
			{
				// SetConfig가 정상적으로 이뤄졌다는 얘기는 Verimatrix VCAS Server에 정상적으로 접근이 되고, context를 create 할 수 있는 상태.
				// online 상태로 context create 함.
				hErr = iptvh_connection_SetConnectionStatus(pstConnContext, eVMXIPTV_ContextStatus_Online);
			}
			else
			{
				// TODO: check  STB Initialized?
				// eMEVT_COLDBOOT_NOTIFY_DONE 처리는 이 부분에서...
				if (pstConnContext->usTmpRetryCount > VMXIPTV_BOOTSERVER_RETRY_COUNTER)
				{
					HxLOG_Info("RetryCount: [%d]...\n", pstConnContext->usTmpRetryCount);

					// 권한없는 stb인듯.. 일단은 drop 3에서는 더는 시도하지 않도록 임시 처리해둔다
					// 계속 cas lib를 호출하면서 stb key가 안 먹는 증상이 생기는 듯 하다
					// TODO:  정리 필요함..!
#if 0
					// STB가 Init 되었을 경우
					// Online으로 접근 할 수 있는 상태는 아니지만 STB 부팅이 완료 되었기 때문에 일단 offline mode로 context를 create 함
					hErr = iptvh_connection_SetConnectionStatus(pstConnContext, eVMXIPTV_ContextStatus_Offline);
#endif
				}
				else
				{
					HxLOG_Info("RetryCount: [%d]...\n", pstConnContext->usTmpRetryCount);
					pstConnContext->usTmpRetryCount++;

					// Set Timer
					lVkRet = VK_TIMER_EventAfter(VMXIPTV_BOOTSERVER_RETRY_TIMEOUT, (void*)iptvh_connection_ManageConnection, NULL, 0, &ulTimerID);
					if (lVkRet != VK_OK)
					{
						HxLOG_Error("VK_TIMER_EventAfter() failed..!! \n");
						return ERR_FAIL;
					}
				}
			}
		}
	}
	else
	{
		if(pstConnContext->eVmxContextStatus == eVMXIPTV_ContextStatus_Online)
		{
			// 이미 Verimatrix Context가 Online으로 생성되어 있는 상태
			// Do Nothing...
		}
		else
		{
			// TODO: check  STB Initialized?
			if (0)
			{
				// STB가 Init 되었을 경우
				// Online으로 접근 할 수 있는 상태는 아니지만 STB 부팅이 완료 되었기 때문에 일단 offline mode로 context를 create 함
				hErr = iptvh_connection_SetConnectionStatus(pstConnContext, eVMXIPTV_ContextStatus_Offline);
			}
			else
			{
				// Set Timer to 10 sec
				lVkRet = VK_TIMER_EventAfter(VMXIPTV_BOOTSERVER_RETRY_TIMEOUT_OFFLINE, (void*)iptvh_connection_ManageConnection, NULL, 0, &ulTimerID);
				if (lVkRet != VK_OK)
				{
					HxLOG_Error("VK_TIMER_EventAfter() failed..!! \n");
					return ERR_FAIL;
				}
			}
		}
	}
#endif

	return ERR_OK;
}

STATIC HBOOL iptvh_connection_GetInitFlag(void)
{
	VmxIptv_ConnectContext_t 	*pstConnContext = iptvh_connection_GetInstance();

	if ((pstConnContext != NULL)
		&& (pstConnContext->eVmxContextStatus != eVmxIptv_ContextStatus_NotInitialized))
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HERROR iptvh_connection_RemoveStoreFile(void)
{
	HERROR hErr = ERR_FAIL;

	if (HLIB_FILE_Exist(VMXIPTV_DEFAULT_STORE_FILE) == TRUE)
	{
		hErr =  HLIB_FILE_Delete(VMXIPTV_DEFAULT_STORE_FILE);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error! fail to delete file (%s)\n", VMXIPTV_DEFAULT_STORE_FILE);
		}
	}
	else
	{
		// 파일이 없으니 지울 게 없다. 근데 파일이 없으면 안 되는데 ??
		HxLOG_Error("Error! Store file not exist (%s)\n", VMXIPTV_DEFAULT_STORE_FILE);
	}

	return hErr;
}

STATIC void iptvh_connection_Initialize(void)
{
	// instance init
	VmxIptv_ConnectContext_t	*pstConnContext;

	// Get Connection Context
	pstConnContext = iptvh_connection_GetInstance();

	pstConnContext->bConnected = FALSE;
	pstConnContext->eVmxContextStatus = eVmxIptv_ContextStatus_NotInitialized;

	HxSTD_memset(pstConnContext->aucBootIpAddress, 0x00, sizeof(pstConnContext->aucBootIpAddress));
	HxSTD_snprintf(pstConnContext->aucBootIpAddress, sizeof(pstConnContext->aucBootIpAddress), "%s", VMXIPTV_DEAULT_BOOT_IPADDRESS);

	HxSTD_memset(pstConnContext->aucStorePath, 0x00, sizeof(pstConnContext->aucStorePath));
	HxSTD_snprintf(pstConnContext->aucStorePath, sizeof(pstConnContext->aucStorePath), "%s", VMXIPTV_DEFAULT_STORE_FILE);

	pstConnContext->usBootPort = VMXIPTV_DEFAULT_BOOT_PORT;
	pstConnContext->eVcasVersion = VMXIPTV_DEFAULT_VCAS_VERSION;

	// Temp
	pstConnContext->usTmpRetryCount = 0;

	// VMConfig를 실행하기전에 수행되어져야한다.
	if(vmx_vr_iptvh_api_SetVcasCommProtocol(pstConnContext->eVcasVersion) != ERR_OK)
	{
		HxLOG_Error("ERROR... at vmx_vr_iptvh_api_SetVcasCommProtocol()\n");
	}

	// boot server에 접속을 해보자
#if defined(VMX_USE_MAIN_MGR_TASK)
	iptvh_connection_SendMsg_ReqBootServerConnect();
#else
	iptvh_connection_ManageConnection();
#endif

	return;
}

#define __________PUBLIC_FUNCTION__________

void vmx_vr_iptvh_connection_Initialize(void)
{
	iptvh_connection_Initialize();
}

HBOOL vmx_vr_iptvh_connection_GetInitFlag(void)
{
	return iptvh_connection_GetInitFlag();
}

HERROR vmx_vr_iptvh_connection_ManageConnection(void)
{
	return iptvh_connection_ManageConnection();
}

void vmx_vr_iptvh_connection_UpdateTime(void)
{
	return iptvh_connection_UpdateTime();
}

HERROR vmx_vr_iptvh_connection_RemoveStoreFile(void)
{
	return iptvh_connection_RemoveStoreFile();
}

/* end of file */
