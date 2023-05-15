

#include <hlib.h>
#include <dlib.h>
#include <vkernel.h>
#include "obama_rpcprotocol.h"
#include "obama_int.h"
#include "obama_memconfig.h"

#include <bus.h>


typedef struct
{
	HINT32				nSock;			///< socket (server)
	HINT32			 	nRpcHandle;
	unsigned long		ulSempahore;	///< semaphore
	HxQueue_t			*pJobQueue;		///< job queue
} Obama_RpcProtocol_t;



typedef struct
{
	HINT32			nClientSock;
	HUINT32			ulRequestId;
	void				*pvData;
	HUINT32			ulSize;
} Obama_RpcProtocolJob_t;


static Obama_RpcProtocol_t *	obama_RpcProtocol_GetInstance (void);

static void 	obama_RpcProtocol_OnDataReceived (void *pvUserdata, HINT32 nClientSock, HUINT32 ulRequestId, HUINT32 ulSize, const void *pvData)
{

	Obama_RpcProtocol_t 	*pGaka = (Obama_RpcProtocol_t*)pvUserdata;
	Obama_RpcProtocolJob_t	*pJob;

	HxSEMT_Get(pGaka->ulSempahore);

	pJob = (Obama_RpcProtocolJob_t *)HLIB_STD_MemCalloc(sizeof(Obama_RpcProtocolJob_t));
	if (pJob == NULL)
	{
		HxLOG_Error("cannot alloc the job! \n");

		HxSEMT_Release(pGaka->ulSempahore);
		return;
	}

	pJob->ulSize     = ulSize;
	if(ulSize > 0)
	{
		pJob->pvData = HLIB_STD_MemCalloc(ulSize);
		if(pvData != NULL)
		{
			HxSTD_MemCopy(pJob->pvData, pvData, ulSize);
		}
	}
	pJob->ulRequestId     = ulRequestId;
	pJob->nClientSock  = nClientSock;

	HLIB_QUEUE_Push(pGaka->pJobQueue, pJob);

	HxSEMT_Release(pGaka->ulSempahore);

	//	Notifying Obama BUS.
	BUS_PostRPCReady();
}

static void		obama_RpcProtocol_ClientDisconnected (Obama_RpcProtocol_t *pGaka, HINT32 nSock)
{
	HxQueue_t					*pTempQ;
	Obama_RpcProtocolJob_t 		*pJob;

	HxLOG_Debug("%s(0x%X)\n", __FUNCTION__, nSock);

	HxSEMT_Get(pGaka->ulSempahore);

	pTempQ = HLIB_QUEUE_New(NULL);
	if (pTempQ)
	{
		while ((pJob = (Obama_RpcProtocolJob_t *)HLIB_QUEUE_Pop(pGaka->pJobQueue)) != NULL)
		{
			if (pJob->nClientSock != nSock)
			{
				HLIB_QUEUE_Push(pTempQ, pJob);
				continue;
			}

			if(pJob->pvData)
				HLIB_STD_MemFree(pJob->pvData);
			HLIB_STD_MemFree(pJob);
		}
		HLIB_QUEUE_Delete(pGaka->pJobQueue);
		pGaka->pJobQueue = pTempQ;
	}

	HxSEMT_Release(pGaka->ulSempahore);

}

static void 	obama_RpcProtocol_OnStatusChanged (void *pvUserdata, HxSocket_Status_e eStatus, HINT32 nClientSock)
{

	Obama_RpcProtocol_t 	*pGaka = (Obama_RpcProtocol_t*)pvUserdata;

	switch (eStatus)
	{
	case HxSOCKET_ADD_CLIENT:
		HLIB_SOCKET_SetSendTimeout(nClientSock, 0);	// 0: use default
		break;
	case HxSOCKET_REMOVE_CLIENT:
		obama_RpcProtocol_ClientDisconnected(pGaka, nClientSock);
		HLIB_RPC_RemoveClientHandle (pGaka->nRpcHandle, nClientSock);
		break;

	default:
		break;
	}
}

static HERROR		obama_RpcProtocol_OnError (HINT32 nClient, HUINT32 errorno)
{
	switch (errorno)
	{
		case EAGAIN:
			HxLOG_Error("client not available. block client(0x%x) \n",nClient);
			HLIB_RPC_SetClientFlag(nClient, eHLIB_RPC_FLAG_BLOCKED);
			break;
		default:
			break;
	}

	return ERR_OK;
}

static void		obama_RpcProtocol_ServerListener (void *arg)
{
	Obama_RpcProtocol_t		*pstGaka = NULL;

	HxLOG_Trace();
	pstGaka = (Obama_RpcProtocol_t*)arg;

	(void) HLIB_SOCKET_Listen((HINT32)pstGaka->nSock, TRUE, (HxSocket_DataListener_t)obama_RpcProtocol_OnDataReceived, obama_RpcProtocol_OnStatusChanged, (void *)pstGaka);
}


static Obama_RpcProtocol_t *	obama_RpcProtocol_GetInstance (void)
{
	static Obama_RpcProtocol_t *s_pGaka = NULL;

	if (s_pGaka == NULL)
	{
		unsigned long	tid;

		s_pGaka = (Obama_RpcProtocol_t *)OxRPC_Calloc(sizeof(Obama_RpcProtocol_t));
		HxLOG_Assert(s_pGaka);

		s_pGaka->nSock = HLIB_EXTRA_OpenServer("obama_rpcprotocol");
		s_pGaka->nRpcHandle = HLIB_RPC_Open (s_pGaka->nSock, HLIB_STD_GetPID());
		HLIB_RPC_SetOnError(s_pGaka->nRpcHandle, obama_RpcProtocol_OnError);
		HxLOG_Assert(s_pGaka->nSock);

		s_pGaka->pJobQueue = HLIB_QUEUE_New(NULL);
		HxLOG_Assert(s_pGaka->pJobQueue);

		if(VK_SEM_Create(&(s_pGaka->ulSempahore), "obama_rpcprotocol", VK_SUSPENDTYPE_FIFO) != VK_OK)
		{
			return  NULL;
		}

		if(VK_TASK_Create(obama_RpcProtocol_ServerListener, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "obama_rpcprotocol", (void *)s_pGaka, &tid, 0) != VK_OK)
		{
			VK_SEM_Destroy(s_pGaka->ulSempahore);
			return  NULL;
		}
		VK_TASK_Start(tid);
	}

	return s_pGaka;
}




HINT32		OBAMA_RpcProtocol_ReadAndDispatch (HINT32 nCount)
{
	Obama_RpcProtocol_t 		*pGaka = obama_RpcProtocol_GetInstance();
	Obama_RpcProtocolJob_t		*pJob;
	HINT32					nDispatched = 0;

	if(pGaka == NULL)
	{
		goto END_FUNC;
	}

	if(pGaka->pJobQueue == NULL)
	{
		goto END_FUNC;
	}

	while (nCount--)
	{
		HxSEMT_Get(pGaka->ulSempahore);

		pJob = (Obama_RpcProtocolJob_t *)HLIB_QUEUE_Pop(pGaka->pJobQueue);

		if (pJob == NULL)
		{
			HxSEMT_Release(pGaka->ulSempahore);
			continue;
		}


		// callback은 critical section 밖에서 호출해야 함
		(void)HLIB_RPC_ProcessRPC (pGaka->nRpcHandle, pJob->ulSize, pJob->pvData, pJob->ulRequestId, pJob->nClientSock);

		if(pJob->pvData)
			HLIB_STD_MemFree(pJob->pvData);
		HLIB_STD_MemFree(pJob);

		nDispatched++;
		HxSEMT_Release(pGaka->ulSempahore);
	}

END_FUNC:
	return nDispatched;


}

HERROR		OBAMA_RpcProtocol_GetRpcHandle (HINT32 *pnRpcHandle)
{
	Obama_RpcProtocol_t *pGaka;

	HxLOG_Trace();
	pGaka = obama_RpcProtocol_GetInstance();
	if (pGaka)
	{
		*pnRpcHandle = pGaka->nRpcHandle;
		HxLOG_Debug("*pnRpcHandle: %d \n", *pnRpcHandle);
		return ERR_OK;
	}

	*pnRpcHandle = 0;
	HxLOG_Error("pGaka is NULL, *pnRpcHandle = 0 \n");
	return ERR_FAIL;

}


