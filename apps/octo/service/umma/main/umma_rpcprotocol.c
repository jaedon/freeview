/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_rpcprotocol.c
	@brief

	Description:  									\n
	Module: Umma RPC Protocol Module			\n

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
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<dirent.h>

#include	<vkernel.h>
#include	<hlib.h>
#include	"umma_int.h"
#include 	<umma_fm.h>
#include 	<umma_fs.h>
#include 	<umma_hotplug.h>
#include 	<umma_smbc.h>
#include 	<umma_smbs.h>
#include 	<umma_ftps.h>

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HINT32		nServerHandle;
	HINT32		nRpcHandle;
}Umma_RpcProtocol_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static Umma_RpcProtocol_t	*s_pstUmmaRpc = NULL;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/

static void		umma_RpcProtocol_onPacketReceived(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	Umma_RpcProtocol_t	*pstUmma = (Umma_RpcProtocol_t*)userData;

	HLIB_RPC_ProcessRPC(pstUmma->nRpcHandle, size, pvData, requestId, nSocketHandle);
}

static void		umma_RpcProtocol_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	Umma_RpcProtocol_t	*pstUmma = (Umma_RpcProtocol_t*)userData;

	if (eStatus == HxSOCKET_REMOVE_CLIENT)
		HLIB_RPC_RemoveClientHandle(pstUmma->nRpcHandle, nClientHandle);
}

static void		umma_RpcProtocol_eventListener(void *arg)
{
	Umma_RpcProtocol_t	*pstUmma = (Umma_RpcProtocol_t*)arg;

	HLIB_SOCKET_Listen(pstUmma->nServerHandle, TRUE, umma_RpcProtocol_onPacketReceived, umma_RpcProtocol_onStatusChanged, arg);
}

static Umma_RpcProtocol_t*	umma_RpcProtocol_GetInstance(void)
{
	if (s_pstUmmaRpc == NULL)
	{
		unsigned long	tid;

		s_pstUmmaRpc = (Umma_RpcProtocol_t*)UMMA_Calloc(sizeof(Umma_RpcProtocol_t));

		s_pstUmmaRpc->nServerHandle = HLIB_EXTRA_OpenServer("umma");
		s_pstUmmaRpc->nRpcHandle    = HLIB_RPC_Open(s_pstUmmaRpc->nServerHandle, HLIB_STD_GetPID());

		//	UMMA MainTask의 Wakeup을 위한 Event.
		VK_TASK_Create(umma_RpcProtocol_eventListener, 70, SIZE_64K, "ummaEventListener", (void*)s_pstUmmaRpc, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);
	}

	return s_pstUmmaRpc;
}

HERROR		UMMA_RpcProtocol_GetRpcHandle (HINT32 *pnRpcHandle)
{
	Umma_RpcProtocol_t *pUmma;

	HxLOG_Trace();
	pUmma = umma_RpcProtocol_GetInstance();
	if (pUmma)
	{
		*pnRpcHandle = pUmma->nRpcHandle;
		HxLOG_Debug("*pnRpcHandle: %d \n", *pnRpcHandle);
		return ERR_OK;
	}

	*pnRpcHandle = 0;
	HxLOG_Error("pUmma is NULL, *pnRpcHandle = 0 \n");
	return ERR_FAIL;

}


