/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  uapi_rpcprotocol.c
	@brief

	Description:  									\n
	Module: UAPI RPC Protocol Module			\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#include 	<hlib.h>
#include	<vkernel.h>
#include	<uapi.h>
#include	<umma_int.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
// #define UAPI_SOCKET_TIMEOUT

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

typedef struct
{
	HINT32			nSock;
	HINT32			nRpcHandle;

//	HUINT32			ulSemaphore;
//
//	HINT32			nPid;
} UAPI_RpcProtocol_t;

static UAPI_RpcProtocol_t *	uapi_RpcProtocol_GetInstance (void);




static void 	uapi_RpcProtocol_OnDataReceived (void *pvUserData, HINT32 nSock, HUINT32 ulRequestId, HUINT32 ulSize, void *pvData)
{
	UAPI_RpcProtocol_t			*pUAPI;

	if (ulSize == 0 || pvData == NULL)
	{
		HxLOG_Error("Invalid response\n");
		return;
	}

	pUAPI = uapi_RpcProtocol_GetInstance();

	if(pUAPI)
	{
		HLIB_RPC_ProcessRPC(pUAPI->nRpcHandle, (HUINT32)ulSize, pvData, ulRequestId, nSock);
	}
}

static void 	uapi_RpcProtocol_OnStatusChanged (void *pvUserdata, HxSocket_Status_e eStatus, HINT32 nSock)
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


static void 	uapi_SocketListener (void *arg)
{
	UAPI_RpcProtocol_t	*pUAPI = (UAPI_RpcProtocol_t *)arg;

	HLIB_SOCKET_Listen(pUAPI->nSock, TRUE, (HxSocket_DataListener_t)uapi_RpcProtocol_OnDataReceived, uapi_RpcProtocol_OnStatusChanged, (void *)pUAPI);
}

static UAPI_RpcProtocol_t *	uapi_RpcProtocol_GetInstance (void)
{
	static UAPI_RpcProtocol_t	s_stUAPI;

	if (s_stUAPI.nSock == 0)
	{
		unsigned long	task_id;

		HxLOG_Trace();
		HxLOG_Debug("%s() uapi_rpcprotocol initialize.\n", __FUNCTION__);

		// initialize
//		HxSEMT_Create(&s_stUAPI.ulSemaphore, "uapi_rpcprotocol_sem", 0);

		s_stUAPI.nSock  = HLIB_EXTRA_OpenClient("umma");
		s_stUAPI.nRpcHandle		= HLIB_RPC_Open(s_stUAPI.nSock, HLIB_STD_GetPID());

//		s_stUAPI.nPid				= HLIB_STD_GetPID();

#if defined(UAPI_SOCKET_TIMEOUT)
		if (s_stUAPI.nSock)
		{
			HLIB_SOCKET_SetSendTimeout(s_stUAPI.nSock, (4 * 1000 * 1000));
		}
#endif

		VK_TASK_Create(uapi_SocketListener, 70, SIZE_64K, "uapi_rpcprotocol_listener", (void*)&s_stUAPI, &task_id, 0);
		VK_TASK_Start(task_id);
	}
	else
	{
		;
	}

	return &s_stUAPI;
}



HERROR	UAPI_RpcProtocol_GetRpcHandle (HINT32 *pnRpcHandle)
{
	UAPI_RpcProtocol_t			*pUAPI;

	HxLOG_Trace();
	pUAPI = uapi_RpcProtocol_GetInstance();

	if(pUAPI)
	{
		*pnRpcHandle = pUAPI->nRpcHandle;
		HxLOG_Debug("*pnRpcHandle: %d \n", *pnRpcHandle);
		return ERR_OK;

	}

	*pnRpcHandle = 0;
	HxLOG_Error("pUAPI is NULL, *pnRpcHandle = 0 \n");
	return ERR_FAIL;
}








