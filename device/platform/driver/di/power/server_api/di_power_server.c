/*************************************************************
 * @file		
 * @date		
 * @author		
 * @brief		

 * http://www.humaxdigital.com
 *************************************************************/

#define ___HEADER________________________________________________________________________________________

#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/reboot.h>

#include "taskdef.h" 
#include "vkernel.h"
#include "drv_err.h"

#include "di_common_server_client.h"
#include "di_power_server_client.h"

#include "nexus_platform.h"
#include "nexus_platform_standby.h"

#define ___DEFINE________________________________________________________________________________________

#if defined(CONFIG_DEBUG)
	#define PWRSERVER_DEBUG
	#ifdef PWRSERVER_DEBUG
	#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
	#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
	#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
	#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
	#else
	#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
	#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
	#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)
	#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)
	#endif /* PWRSERVER_DEBUG */
#else
	#define PrintDebug(...)    ((void)0)
	#define PrintError(...)    VK_DBG_Print (__VA_ARGS__)
	#define PrintInfo(...)     ((void)0)
	#define PrintEnter(...)    ((void)0)
	#define PrintExit(...)     ((void)0)
#endif


#define ___TYPEDEF_______________________________________________________________________________________

typedef struct {
	HBOOL initialized;
	HUINT32 sock_hdl;
	HUINT32 rpc_hdl;
	HUINT32 thread_id;
} POWER_SERVER_T;

#define ___STATIC_VARIABLES______________________________________________________________________________

static POWER_SERVER_T stServerPower;

#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________
 
#define	___PRIVATE_API___________________________________________________________________________________

static HINT32 P_POWER_SERVER_SetStandbySetting(HUINT32 hSockCli, HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nReqId, void *pUsrdata) 
{
	HINT32 ret = -1;
	VKRPC_OBJECT_T *obj;
	HUINT32 hRpc = (HUINT32)pUsrdata;
	NEXUS_PlatformStandbySettings nexusStandbySettings;
	NEXUS_Error errCode;

	DONE_ERR((nArgc!=1));
	obj = apArgv[0];
	DONE_ERR((!obj));
	DONE_ERR((obj->type!=VkOBJECT_TYPE_BIN));
	DONE_ERR((obj->u.b.size!=sizeof(NEXUS_PlatformStandbySettings)));

	VK_memcpy(&nexusStandbySettings, obj->u.b.data, obj->u.b.size);

	PrintDebug("[%s,%d] Receive data: size(%d)\n", __FUNCTION__, __LINE__, obj->u.b.size);
	PrintDebug("[%s,%d] mode: (%d)\n", __FUNCTION__, __LINE__, nexusStandbySettings.mode);
	PrintDebug("[%s,%d] ir: (%d)\n", __FUNCTION__, __LINE__, nexusStandbySettings.wakeupSettings.ir);
	PrintDebug("[%s,%d] gpio: (%d)\n", __FUNCTION__, __LINE__, nexusStandbySettings.wakeupSettings.gpio);
	PrintDebug("[%s,%d] timeout: (%d)\n", __FUNCTION__, __LINE__, nexusStandbySettings.wakeupSettings.timeout);
	PrintDebug("[%s,%d] openFrontend: (%d)\n", __FUNCTION__, __LINE__, nexusStandbySettings.openFrontend);

	errCode = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
	DONE_ERR((errCode!=NEXUS_SUCCESS));	

	ret = 0;
done:
	(void)VK_RPC_Response(hRpc,
		                  hSockCli,
		                  nReqId,
		                  RPC_METHOD_NAME_POWER_SETSETTING,
		                  "i",
		                  ret);
	return ret;
}

static HINT32 P_POWER_SERVER_SetSysFs(HUINT32 hSockCli, HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nReqId, void *pUsrdata) 
{
	HINT32 ret = -1;
	VKRPC_OBJECT_T *obj;
	HUINT32 hRpc = (HUINT32)pUsrdata;

	DONE_ERR((nArgc!=1));
	obj = apArgv[0];
	DONE_ERR((!obj));
	DONE_ERR((obj->type!=VkOBJECT_TYPE_STR));

	PrintDebug("[%s,%d] Receive command: %s\n", __FUNCTION__, __LINE__, obj->u.s.str);
	if (VK_strncmp(obj->u.s.str, "reboot", VK_strlen("reboot")) == 0)
	{
		sync();
		reboot(RB_AUTOBOOT);
	}
	else
	{
		VK_SYSTEM_Command(obj->u.s.str);
	}

	ret = 0;
done:
	(void)VK_RPC_Response(hRpc,
		                  hSockCli,
		                  nReqId,
		                  RPC_METHOD_NAME_POWER_SETSYSFS,
		                  "i",
		                  ret);
	return ret;
}

static HINT32 P_POWER_SERVER_GetStandbyStatus(HUINT32 hSockCli, HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nReqId, void *pUsrdata) 
{
	HUINT32 hRpc = (HUINT32)pUsrdata;
	NEXUS_PlatformStandbyStatus stStatus;

	HAPPY(nArgc);
	HAPPY(apArgv);
	
	VK_memset(&stStatus, 0, sizeof(NEXUS_PlatformStandbyStatus));
	NEXUS_Platform_GetStandbyStatus(&stStatus);

	PrintDebug("[%s,%d] Send standby-status\n", __FUNCTION__, __LINE__);
	
	(void)VK_RPC_Response(hRpc,
		                  hSockCli,
		                  nReqId,
		                  RPC_METHOD_NAME_POWER_GETSTANDBYSTATUS,
		                  "b",
		                  &stStatus,
		                  sizeof(NEXUS_PlatformStandbyStatus));
	return 0;
}

static void P_POWER_SERVER_ReceivePkt(void *pUsrdata, HUINT32 hSock, HUINT32 nReqId, void *pvData, HUINT32 nSize)
{
	(void)VK_RPC_ProcessRpc((HUINT32)pUsrdata, nReqId, pvData, nSize, hSock);
}

static void P_POWER_SERVER_Thread(void *pParam)
{
	HAPPY(pParam);
	
	while(1)
	{
		(void)VK_SOCKET_Listen(stServerPower.sock_hdl, 0xFFFFFFFE, P_POWER_SERVER_ReceivePkt, (void*)stServerPower.rpc_hdl);		
	}
}

#define	___PUBLIC_API____________________________________________________________________________________

DI_ERR_CODE DI_POWER_SERVER_Init(void)
{
	DI_ERR_CODE ret = DI_ERR_ERROR;
	HUINT32 port = MODULE_POWER_PORT_NUM;
	HUINT32 progId = syscall(SYS_gettid);
	HINT32 rc;

	if (stServerPower.initialized)
		return DI_ERR_OK;
	
	rc = VK_SOCKET_SERVER_Open(VK_SOCKET_TYPE_INET, &port, &stServerPower.sock_hdl);
	DONE_ERR((rc!=VK_OK));
	
	rc = VK_RPC_Open(stServerPower.sock_hdl, progId, &stServerPower.rpc_hdl);
	DONE_ERR((rc!=VK_OK));

	(void)VK_RPC_RegisterCall(stServerPower.rpc_hdl,
		                      RPC_METHOD_NAME_POWER_SETSETTING,
		                      "b",
		                      P_POWER_SERVER_SetStandbySetting,
		                      (void*)stServerPower.rpc_hdl);
	(void)VK_RPC_RegisterCall(stServerPower.rpc_hdl,
		                      RPC_METHOD_NAME_POWER_SETSYSFS,
		                      "s",
		                      P_POWER_SERVER_SetSysFs,
		                      (void*)stServerPower.rpc_hdl);
	(void)VK_RPC_RegisterCall(stServerPower.rpc_hdl,
		                      RPC_METHOD_NAME_POWER_GETSTANDBYSTATUS,
		                      "i",
		                      P_POWER_SERVER_GetStandbyStatus,
		                      (void*)stServerPower.rpc_hdl);

	VK_TASK_Create(P_POWER_SERVER_Thread,
		           USER_PRIORITY0,
		           TIMER_TASK_STACK_SIZE,
		           "pwr_serv",
		           NULL,
		           (unsigned long*)&stServerPower.thread_id,
		           VK_SUSPENDTYPE_FIFO);
	VK_TASK_Start(stServerPower.thread_id);

	stServerPower.initialized = TRUE;
	ret = DI_ERR_OK;
done:
	return ret;
}
