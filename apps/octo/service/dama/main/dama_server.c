/**************************************************************
 *	@file		dama.c
 *	Humax Data Manager for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/01
 *	@author			humax
 *	@brief			HumaxDataManager for OCTO 2.0 Project
 **************************************************************/
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <vkernel.h>
#include <hlib.h>
#include <dlib.h>

#include "dama_int.h"

// #define TEMP_UDB_PRIORITY_UP

#ifdef	DAMA_CONFIG_MULTITHREAD
#include "dama_server.h"

typedef struct
{
	HINT32			sock;
#ifdef	DAMA_CONFIG_PRIORITY
	HUINT32			threshold;
	HxPriorityQueue_t	*queue;
#else
	HxQueue_t		*queue;
#endif
	HxHASH_t	*disconn;
	HUINT32			semaphore;
#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 1
	FILE			*logfp;
#endif
} dama_server_t;

static dama_server_t	s_damaServer;
#define	DAMA_SERVER_ENTER	HxSEMT_Get(s_damaServer.semaphore)
#define	DAMA_SERVER_LEAVE	HxSEMT_Release(s_damaServer.semaphore)


static DAMA_Request_t *	dama_server_PopRequest (dama_server_t *server)
{
	DAMA_Request_t	*req;
#ifdef CONFIG_DEBUG
	STATIC HUINT32 tick     =  0 ; 
	STATIC HUINT32	threshold = 0;
	STATIC HUINT32	popCount = 0;
	if(threshold < server->threshold)
	{
		threshold = server->threshold;
	}
	if(tick + 10000 < HLIB_STD_GetSystemTick())
	{
		tick	 =	HLIB_STD_GetSystemTick();	
		
		HxLOG_Info("===========> OverFlow[%d] cur[%d] queue(%d) pop(%d)\n",threshold , server->threshold, HLIB_PRIORITYQUEUE_Length(server->queue), popCount);
		popCount = 0;
	}
#endif
#ifdef	DAMA_CONFIG_PRIORITY
	req = (DAMA_Request_t *)HLIB_PRIORITYQUEUE_Pop(server->queue);
	if (req)
	{
#ifdef CONFIG_DEBUG
		popCount++;
#endif
		server->threshold -= req->size;
		while (server->threshold > DAMA_CONFIG_Q_THRESHOLD && req->prior > DAMA_CONFIG_PRIORITY)
		{
			HLIB_STD_MemFree(req);
			req = (DAMA_Request_t *)HLIB_PRIORITYQUEUE_Pop(server->queue);
			if (req == NULL)
				break;
			server->threshold -= req->size;
		}
	}
#else
	req = (DAMA_Request_t *)HLIB_QUEUE_Pop(s_damaServer.queue);
#endif
	return req;
}


static void	dama_server_PushRequest (dama_server_t *server, DAMA_Request_t *req)
{
#ifdef	DAMA_CONFIG_PRIORITY
	server->threshold += req->size;
	HLIB_PRIORITYQUEUE_Push(server->queue, req);
#else
	HLIB_QUEUE_Push(server->queue, req);
#endif
}


static void	dama_server_OnPacketReceived (void *userData, HINT32 socketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	DAMA_Request_t	*req;

	req = (DAMA_Request_t *)HLIB_STD_MemAlloc(sizeof(DAMA_Request_t) + size);
	if (req)
	{
		req->sock  = socketHandle;
		req->reqId = requestId;
#if defined(CONFIG_DAMA_MULTITHREAD)
		req->forDummy = FALSE;
#endif
		req->size  = size;
		req->data  = (HCHAR *)(req) + sizeof(DAMA_Request_t);
		if (size > 0)
		{
			req->prior = DAMA_CONFIG_PRIORITY;
#ifdef	DAMA_CONFIG_PRIORITY
			if (size >= sizeof(DAMA_Header_t))
			{
				DAMA_Header_t	header;
				memcpy(&header, pvData, sizeof(DAMA_Header_t));

				if (header.cmd == eDAMA_CMD_BATCH && DLIB_HasTag(header.name, "EPG") && DLIB_HasTag(header.name, "RF"))
				{
					req->prior = 1000;
				}
#if defined(TEMP_UDB_PRIORITY_UP)
				else if (HxSTD_StrCmp(header.name, "UDBMODULE") == 0)
				{	
					req->prior = 90;
				}
#endif
			}
#endif
			HxSTD_memcpy(req->data, pvData, size);
		}
		dama_server_PushRequest(&s_damaServer, req);

#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 1
		dama_Logging(s_damaServer.logfp, '>', pvData, size, requestId);
#endif
	}
}

static void dama_server_OnStatusChanged (void *userData, HxSocket_Status_e eStatus, HINT32 clientHandle)
{
	HxLOG_Debug("%s(status:%d, handle:0x%X)\n", __FUNCTION__, eStatus, clientHandle);

	if (eStatus == HxSOCKET_REMOVE_CLIENT)
	{
		HLIB_HASH_Insert(s_damaServer.disconn, (void *)clientHandle, (void *)clientHandle);
	}
}

#ifdef	DAMA_CONFIG_PRIORITY
static HINT32	dama_server_ComparePriority (const DAMA_Request_t *lhs, const DAMA_Request_t *rhs)
{
	return lhs->prior - rhs->prior;
}
#endif

static void	dama_server_Listener (void *arg)
{
	HINT32	ret;

	HxLOG_Trace();

	for (;;)
	{
		DAMA_SERVER_ENTER;
		ret = HLIB_SOCKET_ListenTimeout(s_damaServer.sock, 0, dama_server_OnPacketReceived, dama_server_OnStatusChanged, NULL);
		DAMA_SERVER_LEAVE;
		if (ret < 0)
			break;

		if (ret == 0)
		{
			HLIB_STD_TaskSleep(10);
		}
	}
	DAMA_ERROR("%s() done!! check\n", __FUNCTION__);
	(void)arg;
}

void	DAMA_SERVER_Run (void)
{
	unsigned long	tid;

	HxLOG_Trace();

	s_damaServer.sock = HLIB_EXTRA_OpenServer("dama"); //HLIB_SOCKET_OpenServer(HxSOCKET_UDS, DAMA_SIGNAL_UDS);
	HxSEMT_Create(&s_damaServer.semaphore, "dama_server", HxSEMT_FIFO);
#ifdef	DAMA_CONFIG_PRIORITY
	s_damaServer.queue   = HLIB_PRIORITYQUEUE_New((HxCompareFunc_t)dama_server_ComparePriority, HLIB_STD_MemFree_CB);
#else
	s_damaServer.queue   = HLIB_QUEUE_New((HxQueue_RemoveCallback_t)HLIB_STD_MemFree_CB);
#endif
	s_damaServer.disconn = HLIB_HASH_New(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual);

#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 1
	s_damaServer.logfp   = fopen("dama.comm", "wb");
#endif
	VK_TASK_Create(dama_server_Listener, VK_TASK_PRIORITY_MW_REF, SIZE_256K, "dama_server", NULL, &tid, 0);
	VK_TASK_Start(tid);
}

HUINT32		DAMA_SERVER_RemaindRequest(void)
{
	HUINT32	uiThreshold = 0;
	DAMA_SERVER_ENTER;
	uiThreshold = s_damaServer.threshold;
	DAMA_SERVER_LEAVE;
	return uiThreshold;
}


DAMA_Request_t *	DAMA_SERVER_Listen (void)
{
	DAMA_Request_t	*req;

	DAMA_SERVER_ENTER;
	req = dama_server_PopRequest(&s_damaServer);
	DAMA_SERVER_LEAVE;

	return req;
}

void		DAMA_SERVER_SendToMainThread (DAMA_Request_t *req)
{
	DAMA_SERVER_ENTER;
	dama_server_PushRequest(&s_damaServer, req);
	DAMA_SERVER_LEAVE;
}

HBOOL		DAMA_SERVER_Response (HINT32 sock, const void *data, HUINT32 size, HUINT32 reqId)
{
	DAMA_SERVER_ENTER;
	if (HLIB_HASH_Detach(s_damaServer.disconn, (void *)sock))
	{
		DAMA_TRACE("%s(sock:0x%X) is disconnected!\n", __FUNCTION__, sock);
		DAMA_SERVER_LEAVE;
		return FALSE;
	}
	HLIB_SOCKET_Write(sock, data, size, reqId);
#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 1
	dama_Logging(s_damaServer.logfp, '<', data, size, reqId);
#endif
	DAMA_SERVER_LEAVE;
	return TRUE;
}

HUINT32		DAMA_SERVER_Broadcast (const void *data, HUINT32 size, HUINT32 reqId)
{
	HINT32	clients[24];
	HUINT32	i, n;

	DAMA_SERVER_ENTER;
	n = HLIB_SOCKET_GetClients(s_damaServer.sock, clients, 24);
	for (i = 0 ; i < n ; i++)
	{
		HLIB_SOCKET_Write(clients[i], data, size, reqId);
#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 1
		dama_Logging(s_damaServer.logfp, '<', data, size, reqId);
#endif
	}
	DAMA_SERVER_LEAVE;
	return n;
}
#endif


