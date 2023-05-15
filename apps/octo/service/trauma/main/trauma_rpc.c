
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
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <trauma_int.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct
{
	HINT32			server;
	HINT32			nRpcHandle;
} trauma_rpc_t;

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
static void		trauma_rpc_onstatuschange(void *userData, HxSocket_Status_e eStatus, HINT32 clientHandle)
{
	trauma_rpc_t		*context = (trauma_rpc_t *)userData;

	switch (eStatus)
	{
	case HxSOCKET_REMOVE_CLIENT:
		HLIB_RPC_RemoveClientHandle (context->nRpcHandle, clientHandle);
		HxLOG_Error("Client removed... !! \n");
		break;
	default:
		break;
	}
}

static	void	trauma_rpc_PacketReceived(void *userData, HINT32 socketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	trauma_rpc_t	*context = (trauma_rpc_t*)userData;
	HLIB_RPC_ProcessRPC(context->nRpcHandle, size, pvData, requestId, socketHandle);
}

static void	trauma_rpc_listener (void *arg)
{
	trauma_rpc_t	*context = (trauma_rpc_t*)arg;
	HLIB_SOCKET_Listen(context->server, TRUE, trauma_rpc_PacketReceived, trauma_rpc_onstatuschange, arg);
}

static trauma_rpc_t*	trauma_rpc_getInstance(void)
{
	static trauma_rpc_t	*s_pstTrauma_rpc= NULL;

	if (s_pstTrauma_rpc == NULL)
	{
		HULONG	tid;

		s_pstTrauma_rpc = (trauma_rpc_t*)HLIB_MEM_Calloc(sizeof(trauma_rpc_t));
		s_pstTrauma_rpc->server = HLIB_EXTRA_OpenServer("trauma");
		s_pstTrauma_rpc->nRpcHandle = HLIB_RPC_Open(s_pstTrauma_rpc->server, HLIB_STD_GetPID());

		VK_TASK_Create(trauma_rpc_listener, VK_TASK_PRIORITY_MW_REF, SIZE_32K, "trauma_rpc", (void*)s_pstTrauma_rpc, &tid, 0);
		VK_TASK_Start(tid);

		return s_pstTrauma_rpc;
	}
	return s_pstTrauma_rpc;
}

HERROR	trauma_rpc_recv_erin(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	TRAPI_ERIN_t		*pstErin;
	HERROR				 hResult = ERR_FAIL;
	TRAUMA_MSG_t		 stMsg;

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	pstErin = (TRAPI_ERIN_t *)HxOBJECT_BIN_DATA (apArgv[0]);
	if (NULL == pstErin)
	{
		HxLOG_Error ("pstErin NULL\n");
		goto END_FUNC;
	}

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	stMsg.msg 		= eTRAUMA_MSG_ERIN_RECEIVE;
	stMsg.param1 	= (HINT32)HLIB_MEM_MemDup(pstErin,sizeof(TRAPI_ERIN_t));

	TRAUMA_MSG_Send(&stMsg);

	hResult = ERR_OK;
END_FUNC:
	return hResult;
}

HERROR	trauma_rpc_recv_acceptprivacy(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR				 hResult = ERR_FAIL;
	TRAUMA_MSG_t		 stMsg;

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	stMsg.msg 		= eTRAUMA_MSG_STATISTICS_SET_ACCEPT_PRIVACY;
	stMsg.param1	= (HINT32)HxOBJECT_INT(apArgv[0]);

	TRAUMA_MSG_Send(&stMsg);

	hResult = ERR_OK;
END_FUNC:

	return hResult;
}

HERROR	trauma_rpc_recv_statisticslog(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR				 hResult = ERR_FAIL;
	TRAUMA_MSG_t		 stMsg;
	HCHAR				 *pszLog = NULL;
	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	pszLog = HxOBJECT_STR (apArgv[0]);
	if (HxSTD_StrEmpty(pszLog)== TRUE)
	{
		HxLOG_Error ("statistics log message is empty\n");
		goto END_FUNC;
	}

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	stMsg.msg 		= eTRAUMA_MSG_STATISTICS_LOG;
	stMsg.param1	= (HINT32)HLIB_STD_StrDup(pszLog);

	TRAUMA_MSG_Send(&stMsg);

	hResult = ERR_OK;
END_FUNC:

	return hResult;

}

static void trauma_rpc_registerRpcMethods(trauma_rpc_t *ptrauma)
{
	HLIB_RPC_RegisterCall(ptrauma->nRpcHandle, TRAPI_RPC_ERIN, "b", trauma_rpc_recv_erin,	ptrauma, "TRAUMA revice ERIN info (TRAPI_ERIN_t*)");
	HLIB_RPC_RegisterCall(ptrauma->nRpcHandle, TRAPI_RPC_SEND_ACCEPT_PRIVACY, "i", trauma_rpc_recv_acceptprivacy,	ptrauma, "TRAUMA receive (Set) accept privacy status");
	HLIB_RPC_RegisterCall(ptrauma->nRpcHandle, TRAPI_RPC_SEND_STATISTICS_LOG, "s", trauma_rpc_recv_statisticslog,	ptrauma, "TRAUMA receive Statistics log ");
}

/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

void TRAUMA_RPC_Init(void)
{
	trauma_rpc_t *ptrauma = trauma_rpc_getInstance();
	trauma_rpc_registerRpcMethods(ptrauma);
}

HINT32 TRAUMA_RPC_GetHandle(void)
{
	trauma_rpc_t *ptrauma = trauma_rpc_getInstance();

	return ptrauma->nRpcHandle;
}

void TRAUMA_RPC_Print(void)
{
	trauma_rpc_t *ptrauma = trauma_rpc_getInstance();

	HLIB_RPC_PrintHelp(ptrauma->nRpcHandle, NULL);
}



