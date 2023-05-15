/**************************************************************
 *	@file		oapi_rpcprotocol.c
 *	This module for OCTO 2.0's Open API Interface
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013/01/XX
 *	@author			humax
 *	@brief			OCTO Open API Interface
 **************************************************************/
#include 	<hlib.h>
#include	<obama_int.h>
#include	<oapi.h>
#include	<vkernel.h>

#include	"oapi_mem.h"



typedef struct
{
	HINT32			nSock;
	HINT32			nRpcHandle;

	HUINT32			ulSemaphore;

	HINT32			nPid;
} OAPI_RpcProtocol_t;

static OAPI_RpcProtocol_t *	oapi_RpcProtocol_GetInstance (void);




static void 	oapi_RpcProtocol_OnDataReceived (void *pvUserData, HINT32 nSock, HUINT32 ulRequestId, HUINT32 ulSize, void *pvData)
{
	OAPI_RpcProtocol_t			*pOAPI;

	if (ulSize == 0 || pvData == NULL)
	{
		HxLOG_Error("Invalid response\n");
		return;
	}

	pOAPI = oapi_RpcProtocol_GetInstance();

	if(pOAPI)
	{
		HLIB_RPC_ProcessRPC(pOAPI->nRpcHandle, (HUINT32)ulSize, pvData, ulRequestId, nSock);
	}
}

static void 	oapi_RpcProtocol_OnStatusChanged (void *pvUserdata, HxSocket_Status_e eStatus, HINT32 nSock)
{
	HxLOG_Debug("(+) eStatus(%d)\n", eStatus);

	switch (eStatus)
	{
	case HxSOCKET_DISCONNECT_CLIENT:
		HxLOG_Critical("HxSOCKET_DISCONNECT_CLIENT!!!\n");
		HLIB_SOCKET_Close(nSock);
		break;

	default:
		break;
	}

	HxLOG_Debug("(-)\n");

}


static void 	oapi_SocketListener (void *arg)
{
	OAPI_RpcProtocol_t	*pOAPI = (OAPI_RpcProtocol_t *)arg;

	HLIB_SOCKET_Listen(pOAPI->nSock, TRUE, (HxSocket_DataListener_t)oapi_RpcProtocol_OnDataReceived, oapi_RpcProtocol_OnStatusChanged, (void *)pOAPI);
}

static OAPI_RpcProtocol_t *	oapi_RpcProtocol_GetInstance (void)
{
	static OAPI_RpcProtocol_t	s_stOAPI = {0, };

	if (s_stOAPI.nSock == 0)
	{
		unsigned long	task_id;

		HxLOG_Trace();
		HxLOG_Debug("%s() oapi_rpcprotocol initialize.\n", __FUNCTION__);

		// initialize
		HxSEMT_Create(&s_stOAPI.ulSemaphore, "oapi_rpcprotocol_sem", 0);

		s_stOAPI.nSock  = HLIB_EXTRA_OpenClient("obama_rpcprotocol");
		s_stOAPI.nRpcHandle		= HLIB_RPC_Open(s_stOAPI.nSock, HLIB_STD_GetPID());

		s_stOAPI.nPid				= HLIB_STD_GetPID();



		VK_TASK_Create(oapi_SocketListener, 70, SIZE_64K, "oapi_rpcprotocol_listener", (void*)&s_stOAPI, &task_id, 0);
		VK_TASK_Start(task_id);
	}
	else
	{
		;
	}

	return &s_stOAPI;
}








HERROR	OAPI_RpcProtocol_GetRpcHandle (HINT32 *pnRpcHandle)
{
	OAPI_RpcProtocol_t			*pOAPI;

	HxLOG_Trace();
	pOAPI = oapi_RpcProtocol_GetInstance();

	if(pOAPI)
	{
		*pnRpcHandle = pOAPI->nRpcHandle;
		HxLOG_Debug("*pnRpcHandle: %d \n", *pnRpcHandle);
		return ERR_OK;

	}

	*pnRpcHandle = 0;
	HxLOG_Error("pOAPI is NULL, *pnRpcHandle = 0 \n");
	return ERR_FAIL;
}






