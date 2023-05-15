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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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
#define VMXIPTV_BOOTSERVER_RETRY_TIMEOUT_OFFLINE		(60 * 1000)	// 60 sec, offline�� ��쿣 ���� �� ���� ��ٸ���


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
	HUINT16						usTmpRetryCount;	// �ӽ÷� ����. eMEVT_COLDBOOT_NOTIFY_DONE �̺�Ʈ ���� �� ���� retry �ϴ� ������ �ٲٸ� �ش� ���� ����.

/* should be removed
	// ����� ���� ������� ���� �Ÿ� ������ ����.. �ϴ� ���Ƶΰ� ���߿� ����
	void* 						pVmxContext;	// Connection ����� �� �� ó�� �����Ǵ� Verimatrix Context. ������ �ϰ� �������� ����. (�� Service�� ���ؼ��� ���� Context�� �������)
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
	// STB�� ���� IP address�� ���������� ��� �Դ��� check �ϴ� function.
	// ip address�� ���������� set �Ǿ� �ִ� ����� bNetworkConnected = TRUE, �׷��� ���� ����� bNetworkConnected = FALSE...


	// TODO: IP address ���������� �Ҵ� �Ǿ����� check �� bNetworkConnected �� ����.
#if defined(CONFIG_OP_TDC)
	UNIXTIME		ulUnixTime = HLIB_STD_GetSystemTime();

	// ntp�� �̿��� ����
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
			// offline ���� connection ���뿡 ���� status �� timer ���� ��.
			pstConnContext->bConnected = FALSE;
			pstConnContext->eVmxContextStatus = eVMXIPTV_ContextStatus_Offline;
			break;

		case eVMXIPTV_ContextStatus_Online:
			// online ���� connection ���뿡 ���� status �� timer ���� ��.
			pstConnContext->bConnected = TRUE;
			pstConnContext->eVmxContextStatus = eVMXIPTV_ContextStatus_Online;
			// �̹� online ���¸� ���̻� �õ��� ���� �ʾƵ� ��.
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
		// boot server connection failed..  ���߿� �ٽ� �õ��غ���
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
		// boot server ���� ����
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
			// �̹� Verimatrix Context�� Online���� �����Ǿ� �ִ� ����
			// Do Nothing...
		}
		else
		{
			// TODO: eMEVT_COLDBOOT_NOTIFY_DONE ���� ���� ������ online connection �õ��ϵ��� ���� �ʿ�
			// STB�� ���������� BOOT �Ϸ� �� ������ online connection�� �õ��ϰ�, �� ���Ŀ��� online connection�� �̷����� ������ offline���� context�� ������.
			// SetConfig�� STB�� ����ð��� ȹ���Ͽ��� ���������� ���� ��
			// �ϴ� retry count�� online connection ��õ��� �õ� ��. ���Ŀ� eMEVT_COLDBOOT_NOTIFY_DONE �̺�Ʈ ���� ������ �õ��ϴ� ������ ���� �ʿ�.
			if (vmx_vr_iptvh_api_SetConfig(pstConnContext->aucStorePath, pstConnContext->aucBootIpAddress, pstConnContext->usBootPort) == ERR_OK)
			{
				// SetConfig�� ���������� �̷����ٴ� ���� Verimatrix VCAS Server�� ���������� ������ �ǰ�, context�� create �� �� �ִ� ����.
				// online ���·� context create ��.
				hErr = iptvh_connection_SetConnectionStatus(pstConnContext, eVMXIPTV_ContextStatus_Online);
			}
			else
			{
				// TODO: check  STB Initialized?
				// eMEVT_COLDBOOT_NOTIFY_DONE ó���� �� �κп���...
				if (pstConnContext->usTmpRetryCount > VMXIPTV_BOOTSERVER_RETRY_COUNTER)
				{
					HxLOG_Info("RetryCount: [%d]...\n", pstConnContext->usTmpRetryCount);

					// ���Ѿ��� stb�ε�.. �ϴ��� drop 3������ ���� �õ����� �ʵ��� �ӽ� ó���صд�
					// ��� cas lib�� ȣ���ϸ鼭 stb key�� �� �Դ� ������ ����� �� �ϴ�
					// TODO:  ���� �ʿ���..!
#if 0
					// STB�� Init �Ǿ��� ���
					// Online���� ���� �� �� �ִ� ���´� �ƴ����� STB ������ �Ϸ� �Ǿ��� ������ �ϴ� offline mode�� context�� create ��
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
			// �̹� Verimatrix Context�� Online���� �����Ǿ� �ִ� ����
			// Do Nothing...
		}
		else
		{
			// TODO: check  STB Initialized?
			if (0)
			{
				// STB�� Init �Ǿ��� ���
				// Online���� ���� �� �� �ִ� ���´� �ƴ����� STB ������ �Ϸ� �Ǿ��� ������ �ϴ� offline mode�� context�� create ��
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
		// ������ ������ ���� �� ����. �ٵ� ������ ������ �� �Ǵµ� ??
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

	// VMConfig�� �����ϱ����� ����Ǿ������Ѵ�.
	if(vmx_vr_iptvh_api_SetVcasCommProtocol(pstConnContext->eVcasVersion) != ERR_OK)
	{
		HxLOG_Error("ERROR... at vmx_vr_iptvh_api_SetVcasCommProtocol()\n");
	}

	// boot server�� ������ �غ���
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
