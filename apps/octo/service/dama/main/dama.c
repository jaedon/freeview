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

#include <hapi.h>
#include <papi.h>
#include <uapi.h>
#include <dlib.h>
#include "dama_int.h"
#include "dama_db.h"

#ifdef	DAMA_CONFIG_MULTITHREAD
#include "dama_server.h"
#endif
#if defined(CONFIG_DAMA_MULTITHREAD)
#include "dama_thread.h"
#endif

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif
#ifdef	CONFIG_DEBUG
#include "dama_debug.c"
#endif
#define	DAMA_BUF_THRESHOLD			(1024 * 1024 * 2)
#define	DAMA_TEMP_BUF				3
#define	DAMA_TEMP_STREAM			3
#define	DAMA_CLIENT_NUM			128
#define	DAMA_SYNC_CYCLE			2000	// ms
#define	DAMA_NOTIFICATION_CYCLE	1000	// ms
#define	DAMA_NOTIFY_TERM			1000	// ms
#define	DAMA_REST_IN_PEACE		10000	// ms

//#define	DAMA_SHARE_MONITOR

#if defined(CONFIG_DEBUG) && defined(DAMA_SHARE_MONITOR)
#include "dama_share.c"

#define		DAMA_SHARE(dama,name,type,expr)	\
 	do {\
 		if (dama->share) {\
	 		HUINT32	__tick__ = HLIB_STD_GetSystemTick();\
			expr;\
			DAMA_SHARE_AddInfo(dama->share, name, type, HLIB_STD_GetSystemTick() - __tick__);\
	 	} else {\
	 		expr;\
	 	}\
	 } while (0)
#else
#define		DAMA_SHARE(dama,name,type,expr)	expr;
#endif

typedef struct
{
	DAMA_DB_t		*db;
	HINT32			server;
	HxHASH_t	*observer;

	struct {
		HINT32	rwstream[DAMA_TEMP_STREAM];
		HINT32	index;
	} tempstream;

	struct {
		void *	buf[DAMA_TEMP_BUF];
		HUINT32	size[DAMA_TEMP_BUF];
		HINT32	index;
	} tempbuf;

#ifdef	DAMA_SHARE_MONITOR
	dama_share_t	*share;
#endif
} dama_t;

typedef enum
{
	DAMA_SHUTDOWN_NONE			= 0,
	DAMA_SHUTDOWN_REQUESTED		= 1,
	DAMA_REBOOT_REQUESTED		= 2,
	DAMA_SHUTDOWNDONE_REQUESTED	= 4,
	DAMA_WAIT_SHUTDOWN			= 8
} dama_shutdown_state_e;

static void	dama_push_notification (dama_t *dama);
static void	dama_process_packet (dama_t *dama, HINT32 client_handle, HUINT32 request_id, const void *data, HUINT32 size);


#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
STATIC unsigned long		 s_ulDamaSyncMsgQId = 0;

STATIC unsigned long dama_getSyncMsgQueueId (void)
{
	return s_ulDamaSyncMsgQId;
}

STATIC void dama_setSyncMsgQueueId (unsigned long ulQueueId)
{
	s_ulDamaSyncMsgQId = ulQueueId;
}

HERROR DAMA_SendSyncCmdToSyncThread (DAMA_AsyncMsgType_e eType, void *pvDamaDb, HCHAR *szObjName, HCHAR *szOrgFileName, HCHAR *szTmpFileName)
{
	DAMA_SyncMsg_t				 stMsg;

	HxSTD_MemSet(&stMsg, 0, sizeof(DAMA_SyncMsg_t));

	stMsg.pvDamaDb	= pvDamaDb;
	stMsg.eType		= eType;

	if ((NULL != szObjName) && ('\0' != *szObjName))
	{
		HxSTD_StrNCpy(stMsg.szObjName, szObjName, 63);
		stMsg.szObjName[63] = '\0';
	}

	if ((NULL != szOrgFileName) && ('\0' != *szOrgFileName))
	{
		HxSTD_StrNCpy(stMsg.szOrgFileName, szOrgFileName, 63);
		stMsg.szOrgFileName[63] = '\0';
	}

	if ((NULL != szTmpFileName) && ('\0' != *szTmpFileName))
	{
		HxSTD_StrNCpy(stMsg.szTmpFileName, szTmpFileName, 63);
		stMsg.szTmpFileName[63] = '\0';
	}

	if (VK_MSG_SendTimeout(dama_getSyncMsgQueueId(), (const void *)&stMsg, sizeof(DAMA_SyncMsg_t), 0) != VK_OK)
	{
		HxLOG_Error("VK_MSG_SendTimeout Failed:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif


void		dama_remove_compile_warning (void)
{
#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 1
	(void)dama_Logging;
#endif
}

static dama_shutdown_state_e	dama_shutdown_state (HINT32 opcode, dama_shutdown_state_e state)
{
	static dama_shutdown_state_e	s_state;
	static HUINT32					s_sem;

	if (!s_sem)
	{
		HxSEMT_Create(&s_sem, "dama_shutdown", HxSEMT_FIFO);
	}

	HxSEMT_Get(s_sem);
	if (opcode == 0)
	{
		// do nothing..
	}
	else if (opcode < 0)
	{
		s_state &= ~state;
	}
	else //if (opcode > 0)
	{
		s_state |= state;
	}
	state = s_state;
	HxSEMT_Release(s_sem);

#if defined(CONFIG_DAMA_MULTITHREAD)
	if (DAMA_MULTITHREAD_CheckMultithreadsRunning() == TRUE)
	{
		state |= DAMA_WAIT_SHUTDOWN;
	}
#endif

	return state;
}

static void			dama_shutdown_delay (dama_t *dama)
{
	dama_shutdown_state_e	state = dama_shutdown_state(0, DAMA_SHUTDOWN_NONE);

	if(state & DAMA_SHUTDOWNDONE_REQUESTED)
	{
		DAMA_DB_SetChangedDataFlag(FALSE);
		do
		{
			state = dama_shutdown_state(0, DAMA_SHUTDOWNDONE_REQUESTED);
			if(state & DAMA_SHUTDOWNDONE_REQUESTED)
			{
				HLIB_STD_TaskSleep(10);
			}
		}while(state & DAMA_SHUTDOWNDONE_REQUESTED);
	}

	if (state & DAMA_SHUTDOWN_REQUESTED
		|| state & DAMA_REBOOT_REQUESTED)
	{
		HBOOL bIsChangedData = DAMA_DB_HasChange(dama->db);
		dama_shutdown_state(-1, DAMA_SHUTDOWN_REQUESTED);
		dama_shutdown_state(-1, DAMA_REBOOT_REQUESTED);

		if (bIsChangedData)
		{
			HxLOG_Warning("GET PAPI REQUEST Do ForcedSync...\n", __FUNCTION__);
			DAMA_DB_ForcedSync(dama->db, NULL);
			HxLOG_Info(HxANSI_COLOR_YELLOW("DB FORCED SYNC:")" standby requested!!\n");
		}
		else
		{
			DAMA_DB_SetChangedDataFlag(FALSE);
		}
	}
}

static void *		dama_get_tempbuf (dama_t *dama, HUINT32 size)
{
	HINT32	index;

	index = dama->tempbuf.index;
	if (dama->tempbuf.size[index] < size || dama->tempbuf.size[index] > DAMA_BUF_THRESHOLD)
	{
		void *buf = HLIB_STD_MemAlloc(size);
		if (buf == NULL)
			return NULL;
		if (dama->tempbuf.buf[index])
			HLIB_STD_MemFree(dama->tempbuf.buf[index]);
		dama->tempbuf.buf[index]  = buf;
		dama->tempbuf.size[index] = size;
	}

	dama->tempbuf.index = (index + 1) % DAMA_TEMP_BUF;
	return dama->tempbuf.buf[index];
}

static HINT32		dama_get_tempstream (dama_t *dama)
{
	HINT32	stream;

	if (dama->tempstream.rwstream[dama->tempstream.index] == 0)
	{
		dama->tempstream.rwstream[dama->tempstream.index] = HLIB_RWSTREAM_Open(NULL, 1024);
	}
	stream = dama->tempstream.rwstream[dama->tempstream.index];
	if (stream)
	{
		if (HLIB_RWSTREAM_GetBufSize(stream) > DAMA_BUF_THRESHOLD)
		{
			HLIB_RWSTREAM_Close(stream);
			dama->tempstream.rwstream[dama->tempstream.index] = HLIB_RWSTREAM_Open(NULL, 1024);
			stream = dama->tempstream.rwstream[dama->tempstream.index];
		}
		else
		{
			HLIB_RWSTREAM_Rewind(stream);
		}
	}
	dama->tempstream.index = (dama->tempstream.index + 1) % DAMA_TEMP_STREAM;
	return stream;
}


/*
static const HCHAR *dama_make_tempstr (dama_t *dama, const HCHAR *fmt, ...)
{
	HCHAR	*buf;

	buf = (HCHAR *)dama_get_tempbuf(dama, 1024);
	if (buf)
	{
		va_list	ap;
		va_start(ap, fmt);
		HxSTD_PrintToStrV(buf, 1023, fmt, ap);
		va_end(ap);
	}
	return buf;
}
*/

static void		dama_add_to_notification_q(dama_t *dama, const HCHAR *name, HUID uid, HINT32 client_handle)
{
	DAMA_Observer_t	*observer;
	HUINT32			tick;

	HxLOG_Trace();
	HxLOG_Debug("object[%s] uid["HUID_FMT"]\n", name, uid);

	tick  = HLIB_STD_GetSystemTick();
	observer = (void *)HLIB_HASH_Lookup(dama->observer, (void *)name);
	for (; observer ; observer = observer->next)
	{
		if (observer->clientHandle == client_handle)
		{
			if(observer->header.allowNotifyMyself == FALSE)
			{
				continue;
			}
		}

		if (observer->header.uid != uid
			&& observer->header.uid != DAMA_TABLE_NOTIFIER)
			continue;

		if (observer->tickStamp.first == 0)
			observer->tickStamp.first = tick;
		observer->tickStamp.last = tick;
		HLIB_RWSTREAM_Write(observer->updated, (void *)&uid, sizeof(HUID));
	}
}

static HBOOL	dama_compare_observer_request_id (const DAMA_Observer_t *observer, void *userdata)
{
	return (observer->requestId == (HUINT32)userdata) ? TRUE : FALSE;
}

static HBOOL	dama_compare_observer_socket (const DAMA_Observer_t *observer, void *userdata)
{
	return (observer->clientHandle == (HINT32)userdata) ? TRUE : FALSE;
}

static HINT32	dama_remove_observer_if (dama_t *dama, HBOOL (* _if)(const DAMA_Observer_t *observer, void *userdata), void *userdata)
{
	HxHASH_TableIter_t	iter;
	DAMA_Observer_t 	*observer;
	DAMA_Observer_t 	*garbage;
	DAMA_Observer_t 	*sibling;
	HxVector_t			*list;
	HINT32				removed = 0;
	HINT32				i, n;

	list = HLIB_VECTOR_NewEasy(HLIB_HASH_Size(dama->observer), NULL, NULL);
	if (list == NULL)
		return 0;

	HLIB_HASH_IterInit(&iter, dama->observer);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&observer))
		HLIB_VECTOR_Add(list, observer);

	n = HLIB_VECTOR_Length(list);
	for (i = 0 ; i < n ; i++)
	{
		observer = (DAMA_Observer_t *)HLIB_VECTOR_ItemAt(list, i);
		sibling  = NULL;
		while (observer)
		{
			if (_if(observer, userdata))
			{
				garbage = observer;

				if (sibling)
				{
					observer = observer->next;
					sibling->next = observer;
				}
				else if (observer->next)
				{
					HLIB_HASH_Replace(dama->observer, (void *)HLIB_STD_StrDup(observer->header.name), observer->next);
					observer = observer->next;
				}
				else
				{
					HLIB_HASH_Remove(dama->observer, observer->header.name);
					observer = NULL;
				}

				if (garbage->updated)
					HLIB_RWSTREAM_Close(garbage->updated);
				HLIB_STD_MemFree(garbage);
				removed++;
			}
			else
			{
				sibling  = observer;
				observer = observer->next;
			}
		}
	}
	HLIB_VECTOR_Delete(list);
	return removed;
}

static HBOOL	dama_response (dama_t *dama, HINT32 sock, const void *data, HUINT32 size, HUINT32 requestId)
{
#ifdef	DAMA_CONFIG_MULTITHREAD
	if (!DAMA_SERVER_Response(sock, data, size, requestId))
	{
		HINT32 removed = dama_remove_observer_if(dama, dama_compare_observer_socket, (void *)sock);

		HxLOG_Warning("%s(observer removed:%d)\n", __FUNCTION__, removed);
		return FALSE;
	}
	return TRUE;
#else
	return HLIB_SOCKET_Write(sock, data, size, requestId) == ERR_OK ? TRUE : FALSE;
#endif
}

static HUINT32		dama_broadcast (dama_t *dama, const void *data, HUINT32 size)
{
#ifdef	DAMA_CONFIG_MULTITHREAD
	return DAMA_SERVER_Broadcast (data, size, 0);
#else
	HINT32	clients[20];
	HUINT32	i, n;

	n = HLIB_SOCKET_GetClients(dama->server, clients, 20);
	for (i = 0 ; i < n ; i++)
	{
		dama_response(dama, clients[i], data, size, 0);
	}
	return n;
#endif
}

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)

static void	dama_command_datachanged (dama_t *dama, HINT32 client_handle, const DAMA_Header_t *header, const void *data)
{
	DERROR			err;
	HUID				uid;
	HUINT32			nUidCnt;
	HUINT32			i;

	HxLOG_Trace();

	if ( (dama == NULL) || (header == NULL) || (data == NULL) )
		return;

	err = DAMA_DB_Datachanged(dama->db, header->name);
	switch (err)
	{
		case DERR_OK:
			break;
		case DERR_FAIL:
			break;
		case DERR_DATACHANGED:
			if(header->size > 0)
			{
				nUidCnt = ((header->size) / sizeof(HUID));
			}
			for(i = 0; i < nUidCnt; i++)
			{
				DAMA_MemCpy(&uid, data+(i*sizeof(HUID)), sizeof(HUID));
				dama_add_to_notification_q(dama, header->name, uid, client_handle);
			}
			break;
		default:
			break;
	}
}
#endif

static void	dama_command_set (dama_t *dama, HINT32 client_handle, const DAMA_Header_t *header, const void *data)
{
	HxLOG_Trace();

	if ( (dama == NULL) || (header == NULL) || (data == NULL) )
	 	return;

	if(DERR_DATACHANGED == DAMA_DB_Set(dama->db, header->name, header->uid, header->size, data, TRUE))
	{
		dama_add_to_notification_q(dama, header->name, header->uid, client_handle);
	}
}

static void	dama_command_reset (dama_t *dama, HINT32 client_handle, const DAMA_Header_t *header)
{
	HxLOG_Trace();

	if ( (dama == NULL) || (header == NULL) )
			return;

	if (header->name[0] == '\0')
	{
		HxLOG_Debug("%s(%s) reset all\n", __FUNCTION__, header->name);
		DAMA_DB_Remove(dama->db, NULL, 0, TRUE);
		return;
	}
	DAMA_DB_Remove(dama->db, header->name, header->uid, TRUE);
	dama_add_to_notification_q(dama, header->name, header->uid, client_handle);
}

static void	dama_command_request (dama_t *dama, HINT32 client_handle, const DAMA_Header_t *header, const void *data)
{
	HCHAR		*buf;
	HINT32		temp_sd;
	HINT32		res_sd;
	DAMA_Header_t	resheader;
	DAMA_Result_t	*result;
	const DAMA_Item_t	*item;

	HxLOG_Trace();

	if ( (dama == NULL) || (header == NULL) || (data == NULL) )
		return;

//	HLIB_DBG_BeginTrace("DaMaQuery");
	memcpy(&resheader, header, sizeof(DAMA_Header_t));
	resheader.cmd  = eDAMA_CMD_RESPONSE;
	resheader.size = 0;

	buf = dama_get_tempbuf(dama, header->size + 1);
	memcpy(buf, data, header->size);
	buf[header->size] = '\0';

	result = DAMA_DB_Query(dama->db, header->name, buf);
	if (result == NULL)
	{
		HxLOG_Warning("%s(%s) Empty repository!\n", __FUNCTION__, header->name);
//		HLIB_DBG_Trace("DaMaQuery", 0, buf);
//		HLIB_DBG_EndTrace("DaMaQuery");
		HLIB_DBG_Trace("DaMaQuery", {}, 0, buf);
		goto ERROR;
	}

	temp_sd = dama_get_tempstream(dama);
	HLIB_DBG_Trace("DaMaQuery", {
		res_sd  = dama_get_tempstream(dama);
		while ((item = DAMA_DB_NextItem(result)) != NULL)
		{
			HLIB_RWSTREAM_Write(res_sd, (void *)&(item->uid) , sizeof(HUID));
			HLIB_RWSTREAM_Write(res_sd, (void *)&(item->size), sizeof(HUINT32));
			// +1 의미는 DAMA_Item_t 만큼 주소 점프 해 확장된 공간을 가르침
			HLIB_RWSTREAM_Write(res_sd, (void *)(item + 1), item->size);
		}

		resheader.size = HLIB_RWSTREAM_GetOffset(res_sd);
		HLIB_RWSTREAM_Write(temp_sd, &resheader, sizeof(DAMA_Header_t));
		if (resheader.size > 0)
			HLIB_RWSTREAM_Write(temp_sd, HLIB_RWSTREAM_GetBuf(res_sd), resheader.size);

		dama_response(dama, client_handle, HLIB_RWSTREAM_GetBuf(temp_sd), HLIB_RWSTREAM_GetOffset(temp_sd), resheader.requestId);

//		HLIB_DBG_Trace("DaMaQuery", HLIB_RWSTREAM_GetOffset(temp_sd), buf);
//		HLIB_DBG_EndTrace("DaMaQuery");
	}, HLIB_RWSTREAM_GetOffset(temp_sd), buf);

	return;

ERROR:
	temp_sd = dama_get_tempstream(dama);
	resheader.size = 0;
	HLIB_RWSTREAM_Write(temp_sd, &resheader, sizeof(DAMA_Header_t));
	dama_response(dama, client_handle, HLIB_RWSTREAM_GetBuf(temp_sd), HLIB_RWSTREAM_GetOffset(temp_sd), resheader.requestId);
}

static void	dama_command_add_notifier (dama_t *dama, HINT32 client_handle, const DAMA_Header_t *header, const void *data)
{
	DAMA_Observer_t	*observer;
	DAMA_Observer_t	*sibling;

	if ( (dama == NULL) || (header == NULL) )
		return;

	HxLOG_Trace();

	observer = (DAMA_Observer_t *)HLIB_STD_MemCalloc(sizeof(DAMA_Observer_t));
	if (observer == NULL)
	{
		HxLOG_Error(":: Out of memory!\n");
		return;
	}
	observer->updated = HLIB_RWSTREAM_Open(NULL, 0);
	if (!observer->updated)
	{
		HLIB_STD_MemFree(observer);
		HxLOG_Error(":: Out of memory!\n");
		return;
	}
	memcpy(&observer->header, header, sizeof(DAMA_Header_t));
	observer->clientHandle = client_handle;
	observer->pushCycle    = DLIB_GetPushCycle(header->name);

	sibling = (DAMA_Observer_t *)HLIB_HASH_Lookup(dama->observer, (void *)header->name);
	if (sibling)
	{
		while (sibling->next)
			sibling = sibling->next;
	}
	if (sibling)
		sibling->next = observer;
	else
		HLIB_HASH_Insert(dama->observer, (void *)HLIB_STD_StrDup(header->name), observer);
	HxLOG_Debug(":: The notifier(%s:"HUID_FMT") added!\n", header->name, header->uid);
}

static void	dama_command_allow_notifyMyself (dama_t *dama, HINT32 client_handle, const DAMA_Header_t *header)
{
	DAMA_Observer_t *observer;

	HxLOG_Trace();

	observer = (void *)HLIB_HASH_Lookup(dama->observer, (void *)header->name);
	for (; observer ; observer = observer->next)
	{
		if(observer->header.uid != DAMA_TABLE_NOTIFIER)
		{
			continue;
		}

		if(observer->clientHandle != client_handle)
		{
			continue;
		}

		if(observer->header.requestId != header->requestId)
		{
			continue;
		}

		observer->header.allowNotifyMyself = header->allowNotifyMyself;
	}
}

static void	dama_command_remove_notifier (dama_t *dama, HINT32 client_handle, const DAMA_Header_t *header)
{
	if ( (dama == NULL) || (header == NULL) )
		return;

	HxLOG_Trace();

	dama_remove_observer_if(dama, dama_compare_observer_request_id, (void *)header->requestId);
}

static void	dama_command_forced_sync (dama_t *dama, const DAMA_Header_t *header)
{
	if ( (dama == NULL) || (header == NULL) )
		return;

	if (header->name[0] == '\0')
		DAMA_DB_ForcedSync(dama->db, NULL);
	else
		DAMA_DB_ForcedSync(dama->db, header->name);
}


static void	dama_command_restore (dama_t *dama, const DAMA_Header_t *header)
{
	if ( (dama == NULL) || (header == NULL) )
		return;

	DAMA_DB_Restore(dama->db, header->name);
}


static void	dama_command_batch (dama_t *dama, HINT32 client_handle, const DAMA_Header_t *header, const void *data)
{
	DAMA_Header_t	eachhdr;
	const HUINT8	*p;
	HUINT32			size;

	if ( (dama == NULL) || (data == NULL) )
		return;

	p = (const HUINT8 *)data;

	HLIB_DBG_Trace("DaMaBatch", {
//		HLIB_DBG_BeginTrace("DaMaBatch");

//		HLIB_DBG_Trace("DaMaBatch", header->name, header->size);
		DAMA_DB_AutoCommit(dama->db, FALSE);

		for (size = 0 ; size < header->size ; size += eachhdr.size + sizeof(DAMA_Header_t))
		{
			memcpy(&eachhdr, p, sizeof(DAMA_Header_t));
			dama_process_packet(dama, client_handle, header->requestId, (const void *)p, eachhdr.size + sizeof(DAMA_Header_t));
			p += eachhdr.size + sizeof(DAMA_Header_t);
		}

		DAMA_DB_AutoCommit(dama->db, TRUE);
	}, header->name, header->size);

//	HLIB_DBG_EndTrace("DaMaBatch");
}


static void	dama_command_factory_reset (dama_t *dama, HINT32 client_handle, const DAMA_Header_t *header)
{
	DAMA_Header_t	resheader;

	HxLOG_Trace();

	if (dama == NULL || header == NULL)
		return;

	HxLOG_Warning("%s(%s) reset all\n", __FUNCTION__, header->name);

	DAMA_DB_Remove(dama->db, NULL, 0, TRUE);

	memcpy(&resheader, header, sizeof(DAMA_Header_t));
	resheader.cmd  = eDAMA_CMD_RESPONSE;
	resheader.size = 0;
	dama_response(dama, client_handle, &resheader, sizeof(DAMA_Header_t), resheader.requestId);

//	HLIB_STD_TaskSleep(DAxFACOTRY_RESET_SLEEP_TIME);

	DAMA_TRACE("dama shutting down\n");
//	exit(0);
}

static void	dama_command_server_sync (dama_t *dama, HINT32 client_handle, const DAMA_Header_t *header)
{
	DAMA_Header_t	resheader;

	HxLOG_Trace();

	if (dama == NULL || header == NULL)
		return;

	HxLOG_Debug("%s(%s) memory db is updated\n", __FUNCTION__, header->name);

	memcpy(&resheader, header, sizeof(DAMA_Header_t));
	resheader.cmd  = eDAMA_CMD_RESPONSE;
	resheader.size = 0;
	dama_response(dama, client_handle, &resheader, sizeof(DAMA_Header_t), resheader.requestId);
}

#if defined(CONFIG_DAMA_MULTITHREAD)
static void dama_waitAllThreadJobDone (dama_t *dama)
{
	while (DAMA_MULTITHREAD_CheckMultithreadsRunning() == TRUE)
	{
		VK_TASK_Sleep(100);
	}
}

static HERROR dama_sendProperThreads (dama_t *dama, DAMA_Request_t *req)
{
//	HINT32			 client_handle = req->sock;
	HUINT32			 request_id = req->reqId;
	void			*data = req->data;
	HUINT32			 size = req->size;
	DAMA_Header_t	*header = NULL;
	HERROR			 err = ERR_FAIL;

	if (size < sizeof(DAMA_Header_t) || data == NULL)
	{
		HxLOG_Error(":: Invalid size\n");
		return ERR_FAIL;
	}

	header = (DAMA_Header_t *)data;
	if (header->requestId != request_id)
	{
		HxLOG_Error(":: Invalid request id (%u != header.%u), name[%s] cmd[%d] \n", request_id, header->requestId, header->name, header->cmd);
		return ERR_FAIL;
	}

	size = size - sizeof(DAMA_Header_t);
	if (size == 0)
	{
		data = NULL;
	}
	else
	{
		HUINT8			*pucData;

		pucData = (HUINT8 *)data;
		pucData += sizeof(DAMA_Header_t);
		data = (void *)pucData;
	}

	if (header->size != size)
	{
		HxLOG_Error(":: Size does NOT match (%u != header.%u)\n", size, header->size);
		return ERR_FAIL;
	}

	switch (header->cmd)
	{
	// Items which can be processed in the multi-threads
	case eDAMA_CMD_SET:
		if (header->size > 0)
		{
			// Set Table
			err = DAMA_MULTITHREAD_PushRequest(header->name, req);
		}
		else
		{
			// Reset All
			if (header->name[0] == '\0')
			{
				err = DAMA_MULTITHREAD_PushRequestToAllTable(req);
			}
			// Reset Table
			else
			{
				err = DAMA_MULTITHREAD_PushRequest(header->name, req);
			}
		}

		break;

	case eDAMA_CMD_REQUEST:
		if (header->size > 0)
		{
			if (!HxSTD_StrEmpty(header->name))
			{
				err = DAMA_MULTITHREAD_PushRequest(header->name, req);
			}

			if (ERR_OK != err)
			{
				HCHAR		*pszBuffStr = (HCHAR *)DAMA_Malloc(header->size + 1);

				if (NULL != pszBuffStr)
				{
					HxSTD_MemCopy(pszBuffStr, data, header->size);
					pszBuffStr[header->size] = '\0';

					err = DAMA_MULTITHREAD_PushRequestByQuery(pszBuffStr, req);

					DAMA_Free(pszBuffStr);
				}
			}
		}

		break;

	// In the case of the notifier,
	// It shall be executed in the table threads and main thread.

	case eDAMA_CMD_ADD_NOTIFIER:
		(void)DAMA_MULTITHREAD_PushRequest(header->name, req);
		err = ERR_FAIL;
		break;

	case eDAMA_CMD_ALLOW_NOTIFY_MYSELF:
		(void)DAMA_MULTITHREAD_PushRequest(header->name, req);
		err = ERR_FAIL;
		break;

	case eDAMA_CMD_REMOVE_NOTIFIER:
		(void)DAMA_MULTITHREAD_PushRequest(header->name, req);
		err = ERR_FAIL;
		break;

	case eDAMA_CMD_SYNC:
		if (header->name[0] == '\0')
		{
			err = DAMA_MULTITHREAD_PushRequestToAllTable(req);
		}
		else
		{
			err = DAMA_MULTITHREAD_PushRequest(header->name, req);
		}

		break;

	case eDAMA_CMD_RESTORE:
		err = DAMA_MULTITHREAD_PushRequest(header->name, req);
		break;

	case eDAMA_CMD_BATCH:
		err = DAMA_MULTITHREAD_PushRequest(header->name, req);
		break;

	// Items which can not be processed in the multi-threads:
	// eDAMA_CMD_FACTORY_RESET and eDAMA_CMD_SERVER_SYNC shall be waited to finish all thread job
	case eDAMA_CMD_FACTORY_RESET:
		dama_waitAllThreadJobDone(dama);
		err = ERR_FAIL;
		break;

	case eDAMA_CMD_SERVER_SYNC:
		dama_waitAllThreadJobDone(dama);
		err = ERR_FAIL;
		break;

#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
	case eDAMA_CMD_SYNCFINISHED:
		err = ERR_FAIL;
		break;
#endif

#if defined(DAMA_CHECK_STORAGE_STATUS)
	case eDAMA_CMD_STORAGE_INSERTED:
	case eDAMA_CMD_STORAGE_REMOVED:
	case eDAMA_CMD_STORAGE_UNMOUNTED:
		err = ERR_FAIL;
		break;
#endif

	default:
		HxLOG_Error(":: Invalid cmd(%d)\n", header->cmd);
		break;
	}

	return err;
}

static HERROR dama_processRequestInMainThread (dama_t *dama, DAMA_Request_t *req)
{
	dama_process_packet(dama, req->sock, req->reqId, req->data, req->size);
	return ERR_OK;
}
#endif

static void	dama_process_packet (dama_t *dama, HINT32 client_handle, HUINT32 request_id, const void *data, HUINT32 size)
{
#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
	const void		*orgdata = data;
	HUINT32			 orgsize = size;
#endif
	DAMA_Header_t	*header = NULL;

	if (size < sizeof(DAMA_Header_t) || data == NULL)
	{
		HxLOG_Error(":: Invalid size\n");
		return;
	}

	header = (DAMA_Header_t	*)data;

	if (header->requestId != request_id)
	{
		HxLOG_Error(":: Invalid request id (%u != header.%u), name[%s] cmd[%d] \n", request_id, header->requestId, header->name, header->cmd);
		return;
	}

	size = size - sizeof(DAMA_Header_t);
	if (size == 0)
		data = NULL;
	else
		data = (const HUINT8 *)data + sizeof(DAMA_Header_t);

	if (header->size != size)
	{
		HxLOG_Error(":: Size does NOT match (%u != header.%u)\n", size, header->size);
		return;
	}

	switch (header->cmd)
	{
	case eDAMA_CMD_SET:
		DAMA_SHARE(dama, header->name, DAMA_SHARE_TYPE_SET, {
			if (header->size > 0)
				dama_command_set(dama, client_handle, header, data);
			else
				dama_command_reset(dama, client_handle, header);
		});

#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
		(void)DAMA_DB_QueueDataDuringSync(dama->db, header->name, orgdata, orgsize);
#endif
		break;

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
	case eDAMA_CMD_DATACHANGED:
		DAMA_SHARE(dama, header->name, DAMA_SHARE_TYPE_SET, {
			dama_command_datachanged(dama, client_handle, header, data);
		});
		break;
#endif

	case eDAMA_CMD_REQUEST:
		DAMA_SHARE(dama, header->name, DAMA_SHARE_TYPE_QUERY,
			dama_command_request(dama, client_handle, header, data)
		);
		break;

	case eDAMA_CMD_ADD_NOTIFIER:
		dama_command_add_notifier(dama, client_handle, header, data);
		break;

	case eDAMA_CMD_ALLOW_NOTIFY_MYSELF:
		dama_command_allow_notifyMyself(dama, client_handle, header);
		break;

	case eDAMA_CMD_REMOVE_NOTIFIER:
		dama_command_remove_notifier(dama, client_handle, header);
		break;

	case eDAMA_CMD_SYNC:
		DAMA_SHARE(dama, header->name, DAMA_SHARE_TYPE_SYNC,
			dama_command_forced_sync(dama, header)
		);
		break;

	case eDAMA_CMD_RESTORE:
		dama_command_restore(dama, header);
		break;

	case eDAMA_CMD_BATCH:
		dama_command_batch(dama, client_handle, header, data);
		break;

	case eDAMA_CMD_FACTORY_RESET:
		dama_command_factory_reset(dama, client_handle, header);
		break;

	case eDAMA_CMD_SERVER_SYNC:
		dama_command_server_sync(dama, client_handle, header);
		break;

#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
	case eDAMA_CMD_SYNCFINISHED:
		DAMA_DB_FinalizeSync(dama->db, header->name);
		break;
#endif

#if defined(DAMA_CHECK_STORAGE_STATUS)
	case eDAMA_CMD_STORAGE_INSERTED:
	case eDAMA_CMD_STORAGE_REMOVED:
	case eDAMA_CMD_STORAGE_UNMOUNTED:
		DAMA_DB_ExecuteForStorageChange(dama->db, header->cmd, data);
		break;
#endif

	default:
		HxLOG_Error(":: Invalid cmd(%d)\n", header->cmd);
		break;
	}
}

static void	dama_on_packet_received (void *userData, HINT32 socketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	dama_process_packet((dama_t *)userData, socketHandle, requestId, pvData, size);
}

static void dama_on_status_changed (void *userData, HxSocket_Status_e eStatus, HINT32 clientHandle)
{
	dama_t	*dama;

	dama = (dama_t *)userData;

	HxLOG_Debug("%s(status:%d, handle:0x%X)\n", __FUNCTION__, eStatus, clientHandle);

	if (eStatus == HxSOCKET_REMOVE_CLIENT)
	{
		HINT32 removed = dama_remove_observer_if(dama, dama_compare_observer_socket, (void *)clientHandle);

		HxLOG_Warning("%s(observer removed:%d)\n", __FUNCTION__, removed);
	}
}

static void	dama_cast_limit_status (const HCHAR *name, HUINT32 count, void *userdata)
{
	dama_t			*dama;
	DAMA_Header_t	header;
	DAMA_Status_e	status;
	HINT32			sd;

	dama = (dama_t *)userdata;

	HxLOG_Debug(HxANSI_COLOR_YELLOW("%s(%s:count %d)\n"), __FUNCTION__, name, count);

	HxSTD_memset(&header, 0, sizeof(DAMA_Header_t));
	header.cmd  = eDAMA_CMD_MONITOR;
	HLIB_STD_StrNCpySafe(header.name, name, DAMA_OBJECTNAME_LEN);
	header.size = sizeof(DAMA_Status_e) + sizeof(HUINT32);
	sd = dama_get_tempstream(dama);

	status = eDAMA_LIMIT;

	HLIB_RWSTREAM_Write(sd, &header, sizeof(DAMA_Header_t));
	HLIB_RWSTREAM_Write(sd, &status, sizeof(DAMA_Status_e));
	HLIB_RWSTREAM_Write(sd, &count , sizeof(HUINT32));

	dama_broadcast(dama, HLIB_RWSTREAM_GetBuf(sd), HLIB_RWSTREAM_GetOffset(sd));
}

static HBOOL	dama_sync (dama_t *dama)
{
	HCHAR		 szName[DxLIB_MAX_NAME];

	while (DAMA_DB_ReloadTable(dama->db, szName) == ERR_OK)
	{
		dama_add_to_notification_q(dama, szName, DAMA_TABLE_NOTIFIER, 0);
	}

	if (DAMA_DB_HasChange(dama->db))
	{
		DAMA_DB_Sync(dama->db);
		return TRUE;
	}
	else
	{
		DAMA_DB_SetChangedDataFlag(FALSE);
	}
	return FALSE;
}

static void	dama_push_notification (dama_t *dama)
{
	HUINT32				tick;
	HxHASH_TableIter_t	iter;
	DAMA_Observer_t 	*observer;
	DAMA_Header_t		header;
	HINT32				pushsd;

	HxLOG_Trace();

	pushsd = dama_get_tempstream(dama);
	tick   = HLIB_STD_GetSystemTick();

	HLIB_HASH_IterInit(&iter, dama->observer);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&observer))
	{
		for (; observer ; observer = observer->next)
		{
			if (observer->tickStamp.last == 0)
				continue;

			if (observer->pushCycle)
			{
				if (tick - observer->tickStamp.first < observer->pushCycle)
					continue;
			}
			else
			{
				if (tick - observer->tickStamp.last < DAMA_NOTIFY_TERM)
					continue;
			}

			memcpy(&header, &observer->header, sizeof(DAMA_Header_t));
			header.cmd  = eDAMA_CMD_NOTIFICATION;
			header.size = HLIB_RWSTREAM_GetOffset(observer->updated);
			header.requestId = observer->header.requestId;

			HLIB_RWSTREAM_Write(pushsd, &header, sizeof(DAMA_Header_t));
			if (header.size > 0)
				HLIB_RWSTREAM_Write(pushsd, HLIB_RWSTREAM_GetBuf(observer->updated), HLIB_RWSTREAM_GetOffset(observer->updated));

			DAMA_SERVER_Response(observer->clientHandle, HLIB_RWSTREAM_GetBuf(pushsd), HLIB_RWSTREAM_GetOffset(pushsd), header.requestId);
			//dama_response(dama, observer->clientHandle, HLIB_RWSTREAM_GetBuf(pushsd), HLIB_RWSTREAM_GetOffset(pushsd), header.requestId);
			// dama_response 함수 내부적으로 hash에 add or delete 하게 되고 hash가 틀어진 상태에서dama_push_notification 의 hash iterator를 돌게 되어 잘못된 주소를 call 하게 되는 문제가 존재.
			HxLOG_Debug("%s:%d :: push notification! (%s("HUID_FMT") size:%u)\n", __FUNCTION__, __LINE__, header.name, header.uid, header.size);

			HLIB_RWSTREAM_Rewind(pushsd);
			HLIB_RWSTREAM_Rewind(observer->updated);
			observer->tickStamp.first = 0;
			observer->tickStamp.last  = 0;
		}
	}
}



static void	dama_thread (void *arg)
{
	dama_t *dama = (dama_t*)arg;
	HUINT32	curr;
	HUINT32	prevsync;
	HUINT32	prevpush;
	HUINT32	prevcheck;
	HUINT32	rip;
	HINT32	recv;
	HBOOL	synced;
#ifdef	DAMA_CONFIG_MULTITHREAD
	DAMA_Request_t	*req;
#endif
	DAMA_Header_t	*header = NULL;

	HxLOG_Trace();

	rip = prevcheck = prevpush = prevsync = HLIB_STD_GetSystemTick();

	for (;;)
	{
#ifdef	DAMA_CONFIG_MULTITHREAD
		dama_shutdown_state(+1, DAMA_WAIT_SHUTDOWN);
		for (recv = 0 ; (req = DAMA_SERVER_Listen()) != NULL ; recv++)
		{
			header = (DAMA_Header_t	*)req->data;

#if defined(CONFIG_DAMA_MULTITHREAD)
			if ((!HxSTD_StrEmpty(header->name)) && (!DAMA_MULTITHREAD_IsMultithreaded(header->name)))
			{
				DAMA_DB_CreateTable(dama->db, header->name);
			}

			if (dama_sendProperThreads(dama, req) != ERR_OK)
			{
				dama_processRequestInMainThread(dama, req);
			}

			HLIB_STD_MemFree(req);
#else
			if (!HxSTD_StrEmpty(header->name))	DAMA_DB_CreateTable(dama->db, header->name);
			dama_process_packet(dama, req->sock, req->reqId, req->data, req->size);



			HLIB_STD_MemFree(req);  // Don't free req->data. Already Aloc with DAMA_Request_t( + data).
#endif

			curr = HLIB_STD_GetSystemTick();
			if(curr-rip > DAMA_REST_IN_PEACE)
			{
				DAMA_TRACE(HxANSI_COLOR_YELLOW("REST In Peace(%d tick) recv(%d)\n"), curr-rip, recv);
				break;
			}
		}
		dama_shutdown_state(-1, DAMA_WAIT_SHUTDOWN);
		(void)dama_on_packet_received;
		(void)dama_on_status_changed;
#else
		recv = HLIB_SOCKET_Listen(dama->server, FALSE, dama_on_packet_received, dama_on_status_changed, dama);
#endif

#if defined(CONFIG_DAMA_MULTITHREAD)
		curr = HLIB_STD_GetSystemTick();
		if (recv == 0 || curr - rip > DAMA_REST_IN_PEACE/*10초 넘도록 바쁘면 그냥 잠깐 쉬게 해주자...*/)
		{
			synced = FALSE;
			rip = curr;
			if (curr - prevsync > DAMA_SYNC_CYCLE)
			{
				synced   = dama_sync(dama);
				prevsync = curr;
			}

			if (curr - prevpush > DAMA_NOTIFICATION_CYCLE)
			{
				dama_push_notification(dama);
				prevpush = curr;
			}

			if (synced || curr - prevcheck > DAMA_REST_IN_PEACE)
			{
				DAMA_DB_Vacuum(dama->db);
				DAMA_DB_CheckThreshold(dama->db, dama_cast_limit_status, (void *)dama);
				prevcheck = curr;
			}

			// shutdown 요쳥 받으면, 일단 한번은 바로 force sync하자
			dama_shutdown_delay(dama);
#ifdef	DAMA_SHARE_MONITOR
			DAMA_DEBUG(DAMA_SHARE_Dump(dama->share, stdout));
#endif
			if (recv == 0)
			{
				HLIB_STD_TaskSleep(10);
			}
		}
#else
		curr = HLIB_STD_GetSystemTick();
		if (recv == 0 || curr - rip > DAMA_REST_IN_PEACE/*10초 넘도록 바쁘면 그냥 잠깐 쉬게 해주자...*/)
		{
			synced = FALSE;
			rip = curr;
			if (curr - prevsync > DAMA_SYNC_CYCLE)
			{
				synced   = dama_sync(dama);
				prevsync = curr;
			}

			if (curr - prevpush > DAMA_NOTIFICATION_CYCLE)
			{
				dama_push_notification(dama);
				prevpush = curr;
			}

			if (synced || curr - prevcheck > DAMA_REST_IN_PEACE)
			{
				DAMA_DB_Vacuum(dama->db);
				DAMA_DB_CheckThreshold(dama->db, dama_cast_limit_status, (void *)dama);
				prevcheck = curr;
			}

			// shutdown 요쳥 받으면, 일단 한번은 바로 force sync하자
			dama_shutdown_delay(dama);
#ifdef	DAMA_SHARE_MONITOR
			DAMA_DEBUG(DAMA_SHARE_Dump(dama->share, stdout));
#endif
			if (recv == 0)
			{
				HLIB_STD_TaskSleep(10);
			}
		}
#endif
	}
}

#if defined(CONFIG_DAMA_BACKGROUND_SYNC) || defined(CONFIG_DAMA_BACKGROUND_LOAD)
static unsigned long			 s_ulDamaSyncSemId = 0;
static int						 s_nDamaSyncJobCnt = 0;

static HBOOL dama_IsSyncThreadBusy (void)
{
	HBOOL			 bSync;

	VK_SEM_Get(s_ulDamaSyncSemId);
	bSync = (s_nDamaSyncJobCnt > 0) ? TRUE : FALSE;
	VK_SEM_Release (s_ulDamaSyncSemId);

	return bSync;
}

void DAMA_TakeSyncJob (void)
{
	VK_SEM_Get(s_ulDamaSyncSemId);
	s_nDamaSyncJobCnt++;
	VK_SEM_Release (s_ulDamaSyncSemId);
}

void DAMA_ReleaseSyncJob (void)
{
	VK_SEM_Get(s_ulDamaSyncSemId);
	s_nDamaSyncJobCnt--;
	VK_SEM_Release (s_ulDamaSyncSemId);
}

static void dama_syncInit (void)
{
	int				 nRet;

	nRet = VK_SEM_Create(&s_ulDamaSyncSemId, "damaSyncSem", VK_SUSPENDTYPE_FIFO);
	if (VK_OK != nRet)
	{
		HxLOG_Critical("VK_SEM_Create [damaSyncSem] failed!!!!\n");
	}
}

static void	dama_syncThread (void *arg)
{
	unsigned long	 msgQid = dama_getSyncMsgQueueId();
	dama_t			*dama = (dama_t*)arg;
	DAMA_SyncMsg_t	 stMsg;
	int				 nRet;

#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
	(void)DAMA_DB_LoadDbTableAsync(dama->db);

	HAPI_SendReadySignal(HLIB_STD_GetPID());
#endif

	while (1)
	{
		stMsg.eType = eDAMA_ASYNCTHREAD_MSGTYPE_None;
		nRet = VK_MSG_Receive(msgQid, &stMsg, sizeof(DAMA_SyncMsg_t));
		if (VK_OK == nRet)
		{
			switch (stMsg.eType)
			{
#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
			case eDAMA_ASYNCTHREAD_MSGTYPE_SyncTable:
				nRet = DAMA_DB_SyncTempFile((DAMA_DB_t *)stMsg.pvDamaDb, stMsg.szObjName, stMsg.szTmpFileName);
				if (ERR_OK == nRet)
				{
					DAMA_Header_t		 stHeader;
					DAMA_Request_t		*pstRequest = NULL;

					HxSTD_MemSet(&stHeader, 0, sizeof(DAMA_Header_t));

					stHeader.cmd = eDAMA_CMD_SYNCFINISHED;
					stHeader.requestId = (HUINT32)-1;
					HxSTD_StrNCpy(stHeader.name, stMsg.szObjName, DAMA_OBJECTNAME_LEN);

					pstRequest = (DAMA_Request_t *)HLIB_STD_MemAlloc(sizeof(DAMA_Request_t) + sizeof(DAMA_Header_t));
					if (NULL != pstRequest)
					{
						pstRequest->sock = -1;
						pstRequest->reqId = (HUINT32)-1;
						pstRequest->size = sizeof(DAMA_Header_t);
						pstRequest->data = (HCHAR *)(pstRequest) + sizeof(DAMA_Request_t);
						pstRequest->prior = DAMA_CONFIG_SYNC_DONE_PRIORITY;
						HxSTD_MemCopy(pstRequest->data, &stHeader, sizeof(DAMA_Header_t));

						DAMA_SERVER_SendToMainThread(pstRequest);
					}
				}

				DAMA_ReleaseSyncJob();
				break;
#endif
#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
			case eDAMA_ASYNCTHREAD_MSGTYPE_LoadTable:
				(void)DAMA_DB_LoadDbTableAsync(dama->db);
				break;
#endif

			default:
				break;
			}
		}
		else
		{
			VK_TASK_Sleep(100);
		}
	}
}
#endif

#if defined(DAMA_CHECK_STORAGE_STATUS)
STATIC void dama_cbUapiOnStatusChanged (DxStorage_NotiType_e eType, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HCHAR				*pucBuffer;
	DAMA_Request_t		*pstRequest = NULL;
	DAMA_Header_t		 stHeader;

	HxLOG_Trace();

	if (eDxSTORAGE_NOTI_DevChange == eType)
	{
		HBOOL				 bConnect = (HBOOL)nParam1;
		DxStorage_Info_t	*pstStorage = (DxStorage_Info_t *)nParam2;

		HxLOG_Debug("eDxSTORAGE_NOTI_DevChange: DevId(%d), Connect(%s)\n", pstStorage->ulDeviceId, (TRUE == bConnect) ? "ON" : "OFF");

		HxSTD_MemSet(&stHeader, 0, sizeof(DAMA_Header_t));

		stHeader.cmd = (TRUE == bConnect) ? eDAMA_CMD_STORAGE_INSERTED : eDAMA_CMD_STORAGE_REMOVED;
		stHeader.size = sizeof(DxStorage_Info_t);
		stHeader.requestId = (HUINT32)-1;

		pstRequest = (DAMA_Request_t *)HLIB_STD_MemCalloc(sizeof(DAMA_Request_t) + sizeof(DAMA_Header_t) + sizeof(DxStorage_Info_t));
		if (NULL != pstRequest)
		{
			pstRequest->sock = -1;
			pstRequest->reqId = (HUINT32)-1;
			pstRequest->size = sizeof(DAMA_Header_t) + sizeof(DxStorage_Info_t);
			pucBuffer = (HCHAR *)(pstRequest) + sizeof(DAMA_Request_t);
			pstRequest->data = (void *)pucBuffer;
			pstRequest->prior = DAMA_CONFIG_STORAGE_PRIORITY;
			HxSTD_MemCopy(pucBuffer, &stHeader, sizeof(DAMA_Header_t));
			HxSTD_MemCopy(pucBuffer + sizeof(DAMA_Header_t), pstStorage, sizeof(DxStorage_Info_t));
		}
	}
	else
	{
		HxLOG_Info("NotifyType = %d\n", eType);
	}

	if (NULL != pstRequest)
	{
		DAMA_SERVER_SendToMainThread(pstRequest);
	}
}

STATIC HERROR dama_cbHapiOnStorageUnmounted (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR			*szUnmountPath = NULL;

	HxLOG_Trace();

	if (1 != nArgc)
	{
		HxLOG_Error("nArgc not 1, (%d)\n", nArgc);
		return ERR_FAIL;
	}

	szUnmountPath = HxOBJECT_STR(apArgv[0]);
	if ((NULL != szUnmountPath) && ('\0' != *szUnmountPath))
	{
		HUINT32				 ulPathLen = HxSTD_StrLen(szUnmountPath);
		HCHAR				*pucBuffer;
		DAMA_Request_t		*pstRequest = NULL;
		DAMA_Header_t		 stHeader;

		HxSTD_MemSet(&stHeader, 0, sizeof(DAMA_Header_t));

		stHeader.cmd = eDAMA_CMD_STORAGE_UNMOUNTED;
		stHeader.size = sizeof(HUINT32) + ulPathLen + 1;
		stHeader.requestId = (HUINT32)-1;

		pstRequest = (DAMA_Request_t *)HLIB_STD_MemCalloc(sizeof(DAMA_Request_t) + sizeof(DAMA_Header_t) + sizeof(HUINT32) + ulPathLen + 1);
		if (NULL != pstRequest)
		{
			pstRequest->sock = -1;
			pstRequest->reqId = (HUINT32)-1;
			pstRequest->size = sizeof(DAMA_Header_t) + sizeof(HUINT32) + ulPathLen + 1;
			pucBuffer = (HCHAR *)(pstRequest) + sizeof(DAMA_Request_t);
			pstRequest->data = (void *)pucBuffer;
			pstRequest->prior = DAMA_CONFIG_STORAGE_PRIORITY;
			HxSTD_MemCopy(pucBuffer, &stHeader, sizeof(DAMA_Header_t));
			HxSTD_MemCopy(pucBuffer + sizeof(DAMA_Header_t), &ulPathLen, sizeof(HUINT32));
			HxSTD_MemCopy(pucBuffer + sizeof(DAMA_Header_t) + sizeof(HUINT32), szUnmountPath, ulPathLen);

			DAMA_SERVER_SendToMainThread(pstRequest);
		}
	}

	return ERR_OK;
}
#endif

static void *	dama_run (int argc, char *argv[])
{
	dama_t			*dama;
	unsigned long	tid;

	HLIB_JSON_Init (eHLIB_JSON_Conf_Minimum, (void *(*)(unsigned long))HLIB_STD_MemAlloc_CB, (int (*)(void *))HLIB_STD_MemFree_CB);

	dama = (dama_t *)HLIB_STD_MemCalloc(sizeof(dama_t));
	dama->observer = HLIB_HASH_NewFull((_HxHashFunc)HLIB_HASH_GetStringKey, (_HxEqualFunc)HLIB_HASH_GetStringKey
							, (_HxDestroyNotify)HLIB_STD_MemFree_CB
							, NULL
						);

	// Create the Sync/Load Thread Message Queue first, before opening the DB.
#if defined(CONFIG_DAMA_BACKGROUND_SYNC) || defined(CONFIG_DAMA_BACKGROUND_LOAD)
{
	unsigned long	syncQid = 0;
	int				nRet;

	nRet = VK_MSG_Create(32, sizeof(DAMA_SyncMsg_t),"damaSyncQ", &syncQid, VK_SUSPENDTYPE_FIFO);
	if (VK_OK == nRet)
	{
		dama_setSyncMsgQueueId(syncQid);
	}
	else
	{
		HxLOG_Error("VK_MSG_Create Err:\n");
	}
}
#endif

#if defined(CONFIG_DAMA_MULTITHREAD)
	dama->db = DAMA_DB_Open(DAMA_DIR_DB, DAMA_DIR_BACKUPDB, DAMA_DIR_TEMP_PATH, TRUE, FALSE);
	DAMA_MULTITHREAD_Init(dama->db);
#else
	dama->db = DAMA_DB_Open(DAMA_DIR_DB, DAMA_DIR_BACKUPDB, DAMA_DIR_TEMP_PATH, TRUE, TRUE);
#endif

#ifdef	DAMA_SHARE_MONITOR
	DAMA_DEBUG(dama->share = DAMA_SHARE_New(5000));
#endif

#ifdef	DAMA_CONFIG_MULTITHREAD
	DAMA_SERVER_Run();
#else
	dama->server   = HLIB_EXTRA_OpenServer("dama");// HLIB_SOCKET_OpenServer(HxSOCKET_UDS, DAMA_SIGNAL_UDS);
#endif

#if defined(CONFIG_DAMA_BACKGROUND_SYNC) || defined(CONFIG_DAMA_BACKGROUND_LOAD)
	dama_syncInit();			// dama_thread보다 먼저 해 줘야 한다.
#endif

	VK_TASK_Create(dama_thread, VK_TASK_PRIORITY_MW_REF, SIZE_256K, "dama", (void*)dama, &tid, 0);
	VK_TASK_Start(tid);

#if defined(CONFIG_DAMA_BACKGROUND_SYNC) || defined(CONFIG_DAMA_BACKGROUND_LOAD)
{
	unsigned long	syncTid = 0;
	int				nRet;

	nRet = VK_TASK_Create(dama_syncThread, (VK_TASK_PRIORITY_MW_REF - 10), SIZE_256K, "damaSync", (void*)dama, &syncTid, 0);
	if (VK_OK != nRet)
	{
		HxLOG_Error("VK_TASK_Create Err:\n");
	}

	nRet = VK_TASK_Start(syncTid);
	if (VK_OK != nRet)
	{
		HxLOG_Error("VK_TASK_Start Err:\n");
	}
}
#endif

	if (argc == 1)
	{
		/**
		 * freesat의 경우, json db를 사용했었기 때문에, 아래와 같이 복구 과정을 거쳐야 한다.
		 */
		if (HLIB_FILE_Exist(DAMA_FILE_JSON_DB) || HLIB_FILE_Exist(DAMA_FILE_JSON_BACKUPDB))
		{
			HCHAR	*cmd;

			HxLOG_Info(HxANSI_COLOR_GREEN("RECOVER LEGACY JSON DB\n"));

			cmd = HLIB_STD_FmtString(NULL, 0, "%s %s %s"
					, argv[0]
					, DAMA_FILE_JSON_DB
					, DAMA_FILE_JSON_BACKUPDB
				);
			if (cmd)
			{
				VK_SYSTEM_Command(cmd);
				HLIB_STD_FreeFmtString(cmd);
			}
			HxLOG_Warning("delete db %s and %s \n", DAMA_FILE_JSON_DB, DAMA_FILE_JSON_BACKUPDB);
			HLIB_FILE_Delete(DAMA_FILE_JSON_DB);
			HLIB_FILE_Delete(DAMA_FILE_JSON_BACKUPDB);
		}
	}

	return (void *)dama->db; // for debugging purpose
}


static HERROR	dama_on_SystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	PxSTATUS_e	ePapiStatus;

	HxLOG_Assert(nArgc == 1);
	ePapiStatus = HxOBJECT_INT(apArgv[0]);
	if(ePapiStatus == ePAMA_STATUS_Reboot || ePapiStatus == ePAMA_STATUS_Shutdown)
	{
		dama_shutdown_state_e	state;

		if(ePapiStatus == ePAMA_STATUS_Shutdown)
		{
			state = dama_shutdown_state(+1, DAMA_SHUTDOWN_REQUESTED);
			dama_shutdown_state(-1, DAMA_SHUTDOWNDONE_REQUESTED);	// For S3 mode
		}
		else
		{
			state = dama_shutdown_state(+1, DAMA_REBOOT_REQUESTED);
		}

#if defined(CONFIG_DAMA_MULTITHREAD)
		DAMA_MULTITHREAD_SetShutdown(TRUE);
		DAMA_MULTITHREAD_SetShutdownDone(FALSE);
#endif

		HxLOG_Warning("dama_on_SystemStateChanged state : %d changedFlag : %d remaindCount : %d\n", state, DAMA_DB_GetChangedDataDataFlag(), DAMA_SERVER_RemaindRequest());
		if(DAMA_DB_GetChangedDataDataFlag()
			|| DAMA_SERVER_RemaindRequest()
#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
			|| dama_IsSyncThreadBusy()
#endif
			|| state & DAMA_WAIT_SHUTDOWN
			)
		{
			HxLOG_Warning("%s(eHSIG_GoShutDown or eHSIG_GoReboot) wait few seconds...\n", __FUNCTION__);
			if(ePapiStatus == ePAMA_STATUS_Shutdown)
				PAPI_WaitForShutDown(PxDEFAULT_WAIT);
			else
				PAPI_WaitForReboot(PxDEFAULT_WAIT);
		}
		else
		{
			HxLOG_Warning("dama_on_SystemStateChanged No Need To Wait\n");
		}
	}
	else if(ePapiStatus == ePAMA_STATUS_ShutdownDone)
	{
		dama_shutdown_state(+1, DAMA_SHUTDOWNDONE_REQUESTED);
#if defined(CONFIG_DAMA_MULTITHREAD)
		DAMA_MULTITHREAD_SetShutdown(FALSE);
		DAMA_MULTITHREAD_SetShutdownDone(TRUE);
#endif
	}
	else if(ePapiStatus == ePAMA_STATUS_Operation)
	{
		dama_shutdown_state(-1, DAMA_SHUTDOWNDONE_REQUESTED);
#if defined(CONFIG_DAMA_MULTITHREAD)
		DAMA_MULTITHREAD_SetShutdown(FALSE);
		DAMA_MULTITHREAD_SetShutdownDone(FALSE);
#endif
	}
	return ERR_OK;
}

/**************************************************************
	public functions
 **************************************************************/

int		DAMA_Main(int argc, char *argv[])
{
	void *db;

#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();

	HLIB_DBG_SetupTrace("DaMaBatch", 2, "s:module", "i:size");
	HLIB_DBG_SetupTrace("DaMaQuery", 2, "i:size", "s:query");
#ifdef	DAMA_LOGGING
	(void)dama_Logging;
#endif
#endif

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("dama");

	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif

#if defined(DAMA_LOGGING)
	if (argc > 2)
	{
		if (strcmp(argv[1], "-x") == 0)
		{
			return dama_debug_extract_log(argc - 2, argv + 2);
		}
		return 0;
	}
#endif
	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, dama_on_SystemStateChanged);

	db = dama_run(--argc, ++argv);

#if defined(DAMA_CHECK_STORAGE_STATUS)
	(void)UAPI_FS_Init();
	(void)UAPI_FS_SetNotifier(dama_cbUapiOnStatusChanged);
	(void)HAPI_ConnectSignal("signal:onStorageUnmounted", NULL, dama_cbHapiOnStorageUnmounted);
#endif

#if !defined(CONFIG_DAMA_BACKGROUND_LOAD)
	HAPI_SendReadySignal(HLIB_STD_GetPID());
#endif

#ifdef	CONFIG_DEBUG
	dama_cmdtool_init(db);
#else
	(void)db;
#endif

	return 0;
}

