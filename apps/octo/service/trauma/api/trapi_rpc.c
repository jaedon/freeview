
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
#include <trapi_int.h>
#include <trapi.h>
#include <vkernel.h>

/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct{
	HINT32		nRpcHandle;
	HINT32		nClientSocketInfo;
	HUINT32		ulSemId;
}_trapi_rpc_context;

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
static void 	trapi_rpc_connect(_trapi_rpc_context *context);
static void 	trapi_rpc_socketListener (void *arg);

static _trapi_rpc_context *trapi_rpc_getIntance(void)
{
	static _trapi_rpc_context *s_pcontext= NULL;

	if(s_pcontext == NULL)
	{
		HULONG ulTaskId=0;

		s_pcontext = (_trapi_rpc_context*)HLIB_MEM_Calloc(sizeof(_trapi_rpc_context));
		(void)HxSEMT_Create(&(s_pcontext->ulSemId), "trapi_rpc_sem", 0);
		trapi_rpc_connect(s_pcontext);
		(void)VK_TASK_Create(trapi_rpc_socketListener, 64, SIZE_512K, "trapi_rpc_listener", (void *)s_pcontext, &ulTaskId, 0);
		(void)VK_TASK_Start(ulTaskId);
	}
	HxSEMT_Get(s_pcontext->ulSemId);
	return s_pcontext;
}

static void trapi_rpc_releaseInstance(_trapi_rpc_context *pcontext)
{
	HxSEMT_Release(pcontext->ulSemId);
}

static void trapi_rpc_disconnect(void)
{
	_trapi_rpc_context *context = trapi_rpc_getIntance();

	context->nClientSocketInfo = 0;
	HLIB_RPC_Close(context->nRpcHandle);
	trapi_rpc_releaseInstance(context);

}

static void trapi_rpc_connect(_trapi_rpc_context *context)
{
	context->nClientSocketInfo		= HLIB_EXTRA_OpenClient("trauma");
	context->nRpcHandle		        = HLIB_RPC_Open(context->nClientSocketInfo, HLIB_STD_GetPID());
}

static void trapi_rpc_onPacketReceived (void *pvUserData, HINT32 nSocket, HUINT32 ulRequestId, HINT32 nSize, void *pvData)
{
	_trapi_rpc_context		*pstInst = (_trapi_rpc_context*)pvUserData;
	HxSEMT_Get(pstInst->ulSemId);
	HLIB_RPC_ProcessRPC(pstInst->nRpcHandle, (HUINT32)nSize, pvData, ulRequestId, nSocket);
	HxSEMT_Release(pstInst->ulSemId);
}

static void trapi_rpc_onStatusChanged (void *pvUserData, HxSocket_Status_e eStatus, HINT32 nSocket)
{
	_trapi_rpc_context		*pstInst = (_trapi_rpc_context*)pvUserData;

	HxLOG_Debug("(+) eStatus(%d)\n", eStatus);

	switch (eStatus)
	{
	case HxSOCKET_DISCONNECT_CLIENT:
		HxLOG_Critical("HxSOCKET_DISCONNECT_CLIENT!!!\n");
		HxSEMT_Get(pstInst->ulSemId);
		trapi_rpc_disconnect();
		HxSEMT_Release(pstInst->ulSemId);
		break;

	default:
		break;
	}
	HxLOG_Debug("(-)\n");
}

static void trapi_rpc_socketListener (void *arg)
{
	_trapi_rpc_context 	*pcontext = (_trapi_rpc_context *)arg;

	while(1)
	{
		if(pcontext->nClientSocketInfo)
		{
			HLIB_SOCKET_ListenTimeout(pcontext->nClientSocketInfo,10, trapi_rpc_onPacketReceived, trapi_rpc_onStatusChanged, (void *)pcontext);
		}
		else
		{
			HLIB_STD_TaskSleep(100);
			HxSEMT_Get(pcontext->ulSemId);
			trapi_rpc_connect(pcontext);
			HxSEMT_Release(pcontext->ulSemId);
		}
	}
}

/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
void	trapi_rpc_erin_send(TRAPI_ERIN_t *pData)
{
	_trapi_rpc_context *context = trapi_rpc_getIntance();

	HLIB_RPC_Call(context->nRpcHandle, TRAPI_RPC_ERIN, "b", pData,sizeof(TRAPI_ERIN_t));
	trapi_rpc_releaseInstance(context);
}

void trapi_rpc_acceptprivacy_send(HBOOL bAccept)
{
	_trapi_rpc_context *context = trapi_rpc_getIntance();

	HLIB_RPC_Call(context->nRpcHandle, TRAPI_RPC_SEND_ACCEPT_PRIVACY,"i",bAccept);
	trapi_rpc_releaseInstance(context);
}

void trapi_rpc_statisticsLog_send(HCHAR *pszLog)
{
	_trapi_rpc_context *context = trapi_rpc_getIntance();

	HLIB_RPC_Call(context->nRpcHandle, TRAPI_RPC_SEND_STATISTICS_LOG,"s",pszLog);
	trapi_rpc_releaseInstance(context);
}


