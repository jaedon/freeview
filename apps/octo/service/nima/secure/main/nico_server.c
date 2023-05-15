/*************************************************************************************************************
	File 		: nico_server.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/15
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "nico_server.h"

/*******************************************************************/
/*******************      template         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL		HxSEMT_Get(server_object->obj_ctx.semaphore)
#define	LEAVE_CRITICAL		HxSEMT_Release(server_object->obj_ctx.semaphore)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct 
{
	HINT32		sock;
	HBOOL		is_active;
} nicosession_t;

typedef struct 
{
	HINT32	semaphore;
	HINT32  server_handle;
	HULONG server_taskid;
	HxList_t	*sessions;
} st_server_context;
typedef st_server_context* pst_server_context;

typedef struct 
{
	st_server_context obj_ctx;
	psyncreturn_msg  (*listener)(command_msg *msg);
} st_server_object;
typedef st_server_object* pst_server_object;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC st_server_object g_server_object;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
pst_server_object server_object = &g_server_object;

/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
pst_server_context __nico_server_getcontext(void)
{
	return &server_object->obj_ctx;
}

static void		__nico_server_received_op(void *pvUser, HINT32 nSocketHandle, HUINT32 ulRequestId, HINT32 nSize, void *pvData)
{
	ENTRY;
	ENTER_CRITICAL;
	{
		pcommand_msg recv_msg = (pcommand_msg)pvData;
		command_msg *msg = NULL;

		msg = HLIB_STD_MemAlloc(sizeof(*msg));
		if(NULL != msg)
		{
			HxSTD_MemSet(msg, 0x00, sizeof(*msg));
			msg->sync_call = recv_msg->sync_call;
			msg->type.cmd_type = recv_msg->type.cmd_type;
			msg->device_id = recv_msg->device_id;
			msg->sock_fd = nSocketHandle;
			msg->request_id = ulRequestId;
		
			HxSTD_MemCopy(&msg->cmd_protocol, &recv_msg->cmd_protocol, sizeof(msg->cmd_protocol));

			HxLOG_Debug("Received Command Data from Client ++++++++ \n");
			HxLOG_Debug("command->sync_call    : %s \n", msg->sync_call==TRUE?"true":"false");
			HxLOG_Debug("command->request_id   : %08x \n", msg->request_id);
			HxLOG_Debug("command->deviceid     : %d \n", msg->device_id);
			HxLOG_Debug("command->command_type : %08x \n", msg->type.cmd_type);
			HxLOG_Debug("sizeof command        : %d (%d Kbytes)\n", sizeof(*msg), sizeof(*msg)/1024);
			HxLOG_Debug("socket handle         : 0x%x \n", nSocketHandle);
			HxLOG_Debug("++++++++++++++++++++++++++++++++++++++++++ \n");

			if(server_object->listener)
			{
				psyncreturn_msg ret_msg = NULL;
				if(NULL != (ret_msg = server_object->listener(msg)))
				{
					HLIB_SOCKET_Write(nSocketHandle, ret_msg, sizeof(event_msg), ulRequestId);
					HxLOG_Debug("Send SyncCall-Event ( reqid: %08x ) !!! \n", ulRequestId);
					HLIB_STD_MemFree(ret_msg);
					ret_msg = NULL;
				}
				else if(TRUE == recv_msg->sync_call)
				{
					ret_msg = HLIB_STD_MemAlloc(sizeof(event_msg));
					ret_msg->sync_call = recv_msg->sync_call;
					ret_msg->is_not_support = TRUE;
					HLIB_SOCKET_Write(nSocketHandle, ret_msg, sizeof(event_msg), ulRequestId);
					HLIB_STD_MemFree(ret_msg);
					ret_msg = NULL;
				}
			}
			HLIB_STD_MemFree(msg);
		}
	}
	LEAVE_CRITICAL;
	EXIT;
}

static void 	__nico_server_add_session(pst_server_context ctx, HINT32 sock_fd)
{
	nicosession_t *session = NULL;
	ENTRY;
	session = (nicosession_t*)HLIB_STD_MemCalloc(sizeof(nicosession_t));
	if(session)
	{
		session->sock = sock_fd;
		session->is_active = TRUE;
		ctx->sessions = HLIB_LIST_Append(ctx->sessions, session);
	}
	EXIT;
}
nicosession_t *__nico_server_find_session(pst_server_context ctx, HINT32 sock)
{
	HxList_t *pos = NULL;
	pos = HLIB_LIST_First(ctx->sessions);
	while ( pos )
	{
		nicosession_t *session = HLIB_LIST_Data(pos);
		if ( !session ) return NULL;
		if ( session->sock == sock ) return session;
		pos = HLIB_LIST_Next(pos);
	}
	return NULL;
}

static void 	__nico_server_remove_session(pst_server_context ctx, HINT32 sock_fd)
{
	nicosession_t *session = __nico_server_find_session(ctx, sock_fd);
	ENTRY;
	if(session)
	{
		ctx->sessions = HLIB_LIST_Remove(ctx->sessions, session);
		HLIB_STD_MemFree(session);
		session = NULL;
	}
	EXIT;
}

static void		__nico_server_notification(void *pvUser, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	ENTRY;
	switch (eStatus)
	{
		case HxSOCKET_ADD_CLIENT:
			HxLOG_Debug("[%s](%d) client is connected : [%x]\n", \
							__FUNCTION__, __LINE__, nClientHandle);
			__nico_server_add_session((pst_server_context)pvUser, nClientHandle);
			break;
		case HxSOCKET_REMOVE_CLIENT:
		case HxSOCKET_DISCONNECT_CLIENT:
			HxLOG_Debug("[%s](%d) client is disconnected : [%x]\n", \
							__FUNCTION__, __LINE__, nClientHandle);
			__nico_server_remove_session((pst_server_context)pvUser, nClientHandle);
		default:
			break;
	}
	EXIT;
}

void __nico_server_listener(void *param)
{
	ENTRY;
	HLIB_SOCKET_Listen((HINT32)param, TRUE, __nico_server_received_op, __nico_server_notification, &server_object->obj_ctx);
	EXIT;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR nico_server_init( HCHAR* addr)
{
	HERROR hRet = ERR_OK;
	st_server_context* ctx = __nico_server_getcontext();

	ENTRY;
	HxSEMT_Create(&ctx->semaphore, "ncapi_client_sem", 0);
	ctx->server_handle = HLIB_EXTRA_OpenServer("nico");// HLIB_SOCKET_OpenServer(HxSOCKET_UDS, (void*)addr);
	if(0 == ctx->server_handle)
	{
		hRet = ERR_FAIL;
	}
	EXIT;
	return hRet;
}

HERROR nico_server_start( void )
{
	HERROR hRet = ERR_OK;
	st_server_context* ctx = __nico_server_getcontext();
	ENTRY;
	if(VK_OK != VK_TASK_Create(__nico_server_listener,
					VK_TASK_PRIORITY_MW_REF, SIZE_64K, "nico_server_listener",
					(void*)ctx->server_handle, &(ctx->server_taskid), 0))
	{
		hRet = ERR_FAIL;
	}
	if(VK_OK != VK_TASK_Start(ctx->server_taskid))
	{
		hRet = ERR_FAIL;
	}
	EXIT;
	return hRet;
}

HERROR nico_server_stop( void )
{
	HERROR hRet = ERR_OK;
	st_server_context* ctx = __nico_server_getcontext();
	ENTRY;
	HLIB_SOCKET_Close(ctx->server_handle);

#if 0
	if(VK_OK != VK_TASK_Stop(ctx->server_taskid) ||
		VK_OK != VK_TASK_Destroy(ctx->server_taskid))
	{
		hRet = ERR_FAIL;
	}
#endif
	EXIT;
	return hRet;
}

HERROR nico_server_sendevent(event_msg *params)
{
	HERROR hRet = ERR_OK;
	HINT32 session_counter = 0;
	HxList_t *pos = NULL;
	pst_server_context ctx = __nico_server_getcontext();
	ENTRY;
	pos = HLIB_LIST_First(ctx->sessions);
	if(!pos)
	{
		HxLOG_Debug("There is no session(%p) \n", pos);
		hRet = ERR_FAIL;
	}
	while ( pos )
	{
		nicosession_t *session = HLIB_LIST_Data(pos);
		if ( !session )
		{
			hRet = ERR_FAIL;
			break;
		}
		HxLOG_Debug("Send Event ++++++++++++++++++++++++++ \n");
		HxLOG_Debug("%d.session(%p) \n", session_counter++, session);
		HxLOG_Debug("+++++++++++++++++++++++++++++++++++++ \n");
		ENTER_CRITICAL;
		HLIB_SOCKET_Write(session->sock, params, sizeof(event_msg), params->request_id);
		LEAVE_CRITICAL;
		HxLOG_Debug("Send Event (reqid: %08x)\n", params->request_id);
		pos = HLIB_LIST_Next(pos);
	}
	EXIT;
	return hRet;
}

HERROR nico_server_deinit( void )
{
	HERROR hRet = ERR_OK;
	pst_server_context ctx = __nico_server_getcontext();
	ENTRY;
	hRet = HxSEMT_Destroy(ctx->semaphore);

	if(server_object) server_object->listener = NULL;
	
	(void*)ctx;
	EXIT;
	return hRet;
}

HERROR nico_server_register_callback( void *callback )
{	
	ENTRY;

	if( server_object )
		server_object->listener = callback;
	
	EXIT;
	return ERR_OK;
}

