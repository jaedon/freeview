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

#include "taskdef.h"
#include "drv_err.h"
#include "vkernel.h"

#include "di_common_server_client.h"
#include "di_power_server_client.h"

#define ___DEFINE________________________________________________________________________________________

#define PWRCLIENT_DEBUG
#ifdef PWRCLIENT_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)
#endif /* PWRCLIENT_DEBUG */

#define DiCLIENT_POWER_REQUEST_TIMEOUT    10000  /*ms*/

#define ___TYPEDEF_______________________________________________________________________________________

typedef struct {
	HBOOL initialized;
	HUINT32 sock_hdl;
	HUINT32 rpc_hdl;
	HUINT32 thread_id;
} POWER_CLIENT_T;

typedef struct {
	HINT32 ret;
	void *data;
	HUINT32 size;
} POWER_CLIENT_RET_T;

#define ___STATIC_VARIABLES______________________________________________________________________________

static POWER_CLIENT_T stClientPower;

#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________

#define	___PRIVATE_API___________________________________________________________________________________

static void P_POWER_CLIENT_ReceivePkt(void *pUsrdata, HUINT32 hSock, HUINT32 nReqId, void *pvData, HUINT32 nSize)
{
	(void)VK_RPC_ProcessRpc((HUINT32)pUsrdata, nReqId, pvData, nSize, hSock);
}

static void P_POWER_CLIENT_Thread(void *pParam)
{
	HAPPY(pParam);
	
	while(1)
	{
		(void)VK_SOCKET_Listen(stClientPower.sock_hdl, 0xFFFFFFFE, P_POWER_CLIENT_ReceivePkt, (void*)stClientPower.rpc_hdl);		
	}
}

static HINT32 p_power_SetStandbySettingCb(HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nRequestId, void *pvUserData)
{
	HINT32 ret = -1;
	POWER_CLIENT_RET_T *rpc_ret = pvUserData;
	VKRPC_OBJECT_T *obj;

	HAPPY(nRequestId);

	DONE_ERR((nArgc!=1));
	obj = apArgv[0];
	DONE_ERR((obj->type!=VkOBJECT_TYPE_INT));	

	rpc_ret->ret = obj->u.i.num;
	ret = 0;
	
done:
	PrintDebug("[%s,%d] rpc_ret->ret(%d), ret(%d)\n", __FUNCTION__, __LINE__, rpc_ret->ret, ret);
	return ret;
}

static HINT32 p_power_GetStandbyStatusCb(HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nRequestId, void *pvUserData)
{
	HINT32 ret = -1;
	POWER_CLIENT_RET_T *rpc_ret = pvUserData;
	VKRPC_OBJECT_T *obj;

	HAPPY(nRequestId);

	DONE_ERR((nArgc!=1));
	obj = apArgv[0];
	DONE_ERR((obj->type!=VkOBJECT_TYPE_BIN));

	rpc_ret->data = VK_malloc(obj->u.b.size);
	DONE_ERR((!rpc_ret->data));
	VK_memcpy(rpc_ret->data, obj->u.b.data, obj->u.b.size);
	rpc_ret->size = obj->u.b.size;
	rpc_ret->ret = ret = 0;
done:
	return ret;

}

static HINT32 p_power_SetSysFsCb(HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nRequestId, void *pvUserData)
{
	HINT32 ret = -1;
	POWER_CLIENT_RET_T *rpc_ret = pvUserData;
	VKRPC_OBJECT_T *obj;

	HAPPY(nRequestId);

	DONE_ERR((nArgc!=1));
	obj = apArgv[0];
	DONE_ERR((obj->type!=VkOBJECT_TYPE_INT));

	rpc_ret->ret = obj->u.i.num;
	ret = 0;
done:
	return ret;
}


#define	___PUBLIC_API____________________________________________________________________________________

DI_ERR_CODE DI_POWER_CLIENT_SetStandbySetting(void *pNexusStandbySettings, unsigned long size)
{
	DI_ERR_CODE ret = DI_ERR_ERROR;
	HINT32 rc;
	POWER_CLIENT_RET_T *rpc_ret = NULL;

	DONE_ERR((!pNexusStandbySettings));
	DONE_ERR((!size));
	
	rpc_ret = VK_malloc(sizeof(POWER_CLIENT_RET_T));
	DONE_ERR((!rpc_ret));

	rpc_ret->ret = -1;
	rpc_ret->data = NULL;
	rpc_ret->size = 0;
	
	rc = VK_RPC_Request(stClientPower.rpc_hdl,
		                p_power_SetStandbySettingCb,
		                DiCLIENT_POWER_REQUEST_TIMEOUT,
		                (void*)rpc_ret,
		                RPC_METHOD_NAME_POWER_SETSETTING,
		                "b",
		                pNexusStandbySettings,
		                size);
	DONE_ERR((rc!=VK_OK));
	DONE_ERR((rpc_ret->ret < 0));
	
	ret = DI_ERR_OK;
done:
	if (rpc_ret)
	{
		if (rpc_ret->data) VK_free(rpc_ret->data);
		VK_free(rpc_ret);
	}
	return ret;	
}

DI_ERR_CODE DI_POWER_CLIENT_GetStandbyStatus(void *pNexusStandbyStatus)
{
	HINT32 rc;
	DI_ERR_CODE ret = DI_ERR_ERROR;	
	POWER_CLIENT_RET_T *rpc_ret = NULL;

	DONE_ERR((!pNexusStandbyStatus));
	
	rpc_ret = VK_malloc(sizeof(POWER_CLIENT_RET_T));
	DONE_ERR((!rpc_ret));

	rpc_ret->ret = -1;
	rpc_ret->data = NULL;
	rpc_ret->size = 0;
	
	rc = VK_RPC_Request(stClientPower.rpc_hdl,
		                p_power_GetStandbyStatusCb,
		                DiCLIENT_POWER_REQUEST_TIMEOUT,
		                (void*)rpc_ret,
		                RPC_METHOD_NAME_POWER_GETSTANDBYSTATUS,
		                "i",
		                0);
	DONE_ERR((rc!=VK_OK));
	DONE_ERR((rpc_ret->ret < 0));

	VK_memcpy(pNexusStandbyStatus, rpc_ret->data, rpc_ret->size);
	
	ret = DI_ERR_OK;
done:
	if (rpc_ret)
	{
		if (rpc_ret->data) VK_free(rpc_ret->data);
		VK_free(rpc_ret);
	}
	return ret;	
}


DI_ERR_CODE DI_POWER_CLIENT_SetSysFs(HCHAR *pCmd)
{
	DI_ERR_CODE ret = DI_ERR_ERROR;
	HINT32 rc;
	POWER_CLIENT_RET_T *rpc_ret = NULL;

	DONE_ERR((!pCmd));
	
	rpc_ret = VK_malloc(sizeof(POWER_CLIENT_RET_T));
	DONE_ERR((!rpc_ret));

	rpc_ret->ret = -1;
	rpc_ret->data = NULL;
	rpc_ret->size = 0;

	rc = VK_RPC_Request(stClientPower.rpc_hdl,
		                p_power_SetSysFsCb,
		                DiCLIENT_POWER_REQUEST_TIMEOUT,
		                (void*)rpc_ret,
		                RPC_METHOD_NAME_POWER_SETSYSFS,
		                "s",
		                pCmd);
	DONE_ERR((rc!=VK_OK));
	DONE_ERR((rpc_ret->ret < 0));
	
	ret = DI_ERR_OK;
done:
	if (rpc_ret)
	{
		if (rpc_ret->data) VK_free(rpc_ret->data);
		VK_free(rpc_ret);
	}
	return ret;	
}

DI_ERR_CODE DI_POWER_CLIENT_Init(void)
{
	DI_ERR_CODE ret = DI_ERR_ERROR;
	HINT32 rc;
	HUINT32 port = MODULE_POWER_PORT_NUM;
	HUINT32 progId = syscall(SYS_gettid);

	if (stClientPower.initialized)
		return DI_ERR_OK;
	
	rc = VK_SOCKET_CLIENT_Open(VK_SOCKET_TYPE_INET, NULL, &port, 10, &stClientPower.sock_hdl);
	DONE_ERR((rc!=VK_OK));

	rc = VK_RPC_Open(stClientPower.sock_hdl, progId, &stClientPower.rpc_hdl);
	DONE_ERR((rc!=VK_OK));

	VK_TASK_Create(P_POWER_CLIENT_Thread,
		           USER_PRIORITY0,
		           TIMER_TASK_STACK_SIZE,
		           "pwr_client",
		           NULL,
		           (unsigned long*)&stClientPower.thread_id,
		           VK_SUSPENDTYPE_FIFO);
	VK_TASK_Start(stClientPower.thread_id);
	
	stClientPower.initialized = TRUE;
	ret = DI_ERR_OK;
done:
	return ret;	
}

