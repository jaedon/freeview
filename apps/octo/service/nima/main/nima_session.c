/** **********************************************************************************************************
	File 		: nima_session.c
	author 		: STB Component tstbcomp@humaxdigital.com
	comment		:
	date    	: 2013/05/15
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
*********************************************************************************************************** */


#include "nima_command_procedure.h"
#include "nima_util.h"
#include "nima_session.h"
#include "nima_protocol.h"
#include "nima_db.h"
#include "nima_conv.h"


#include "ncapi.h"
#include "napi.h"

#include <vkernel.h>
#include <hlib.h>
#include <stdio.h>
#include <unistd.h>

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
/* TODO::Should move "nima_macro.h" */
#define NIMA_SAFE_DELETE(x) \
		do{ \
			if(NULL != x){	\
				HLIB_STD_MemFree(x); \
				x = NULL; \
			} \
		}while(0)

#define SAVE_PARAM_TO_DB(DATA_TYPE, size, data, flag) \
		do { \
			DATA_TYPE *param = HLIB_STD_MemAlloc(size); \
			if(param){ \
				HxSTD_MemCopy(param, data, size); \
				nima_db_save(param, flag); \
				NIMA_SAFE_DELETE(param); \
			} \
		}while(0);


typedef enum {
	SESSION_MSGQ_SIZE		= 32,
	SESSION_MSGQ_TIMEOUT	= 500
} eGlobalNumaServer;

typedef struct _nimamsg_t {
	HCHAR		*data;
	HINT32		len;
} nimamsg_t;

NIMAProtocol_devinfo_t *g_pdevinfo = NULL;

/*-----------------------------------------------------------------------------------------------
 *	internal functions
 *----------------------------------------------------------------------------------------------*/

static void __nima_send_event(nimasession_t *session, const HCHAR *data, HINT32 len)
{
	FuncEnter;
	if ( NULL != session )
		HLIB_SOCKET_Write(session->sock, (void *)data, len, 0);

	FuncLeave;
}

static void __nima_do_process(HCHAR *data, HINT32 len, nimasession_t *session)
{
	nimapacket_t *packet = NULL;
	nimapacket_t * ret = NULL;

	FuncEnter;
	if(NULL == data) return;
	packet = (nimapacket_t *)data;

	switch(packet->type)
	{
		case NIMA_PROTOTYPE_REQUEST:
			if(packet->sync)
			{
				ret = nima_do_command_sync(packet);
				__nima_send_event(session, (HCHAR *) ret, sizeof(nimapacket_t) + ret->dlen);
				HLIB_MEM_Free(ret);
			}
			else
			{
				nima_do_command_async(packet);
			}
			break;
		case NIMA_PROTOTYPE_REPLY:

			if(packet->sync)
			{
				//Currently, there is no sync reply packet
			}
			else
			{
				__nima_send_event(session, data, len);
			}
			break;
	}
	FuncLeave;
	return;
}

static void * __nima_session(void *args)
{
	nimasession_t *session = NULL;

	FuncEnter;
	session = (nimasession_t *)args;
	while ( session->is_active )
	{
		nimamsg_t msg;
		HxSTD_MemSet(&msg, 0, sizeof(msg));

		if ( VK_MSG_ReceiveTimeout(session->msgq, &msg, sizeof(msg),
					SESSION_MSGQ_TIMEOUT) == VK_OK )
		{
			__nima_do_process(msg.data, msg.len, session);
			NIMA_SAFE_DELETE(msg.data);
		}
		else
		{
			usleep(100000);
		}
	}
	VK_MSG_Destroy(session->msgq);
    HLIB_STD_MemFree(session);
	FuncLeave;

	return NULL;
}

static HBOOL __nima_run_thread_detached(pthread_t * t, void * (*thread_func)(void *), void * param)
{
   pthread_attr_t attr;
   if (pthread_attr_init(&attr) != 0)
   {
       HxLOG_Error("pthread_attr_init() failed\n");
       return FALSE;
   }
   if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
   {
       HxLOG_Error("pthread_attr_setdetachstate() failed\n");
       pthread_attr_destroy(&attr);
       return FALSE;
   }
   if (pthread_create(t, &attr, thread_func, param) != 0)
   {
       HxLOG_Error("pthread_create() failed\n");
       pthread_attr_destroy(&attr);
       return FALSE;
   }
   pthread_attr_destroy(&attr);

   return TRUE;
}

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
HERROR nima_session_start(nimasession_t *session, HINT32 sock, HINT32 flags)
{
	FuncEnter;

	if ( !session ) return ERR_FAIL;

	session->sock = sock;
	session->is_active = TRUE;
	session->is_init = FALSE;

	if ( VK_MSG_Create(SESSION_MSGQ_SIZE, sizeof(nimamsg_t), "NIMASessionMessage",
				&session->msgq, VK_SUSPENDTYPE_PRIORITY) != VK_OK )
	{
		return ERR_FAIL;
	}
	
	if (__nima_run_thread_detached(&session->thread_id, __nima_session, (void*)session) == FALSE)
	{
		HxLOG_Error("__nima_session thread start failed\n");
		return ERR_FAIL;
	}

	FuncLeave;

	return ERR_OK;
}

HERROR nima_session_stop(nimasession_t *session)
{
	FuncEnter;

	if ( !session ) return ERR_FAIL;
	session->is_active = FALSE;

	FuncLeave;
	return ERR_OK;
}

HERROR nima_session_push(nimasession_t *session, const HCHAR *data, HINT32 len)
{
	nimamsg_t *msg = NULL;

	FuncEnter;
	msg = (nimamsg_t *)data;
	FuncLeave;
	return (VK_MSG_Send(session->msgq, (void *)msg, sizeof(nimamsg_t)) == VK_OK) ? ERR_OK : ERR_FAIL;
}

HBOOL nima_session_is_active(nimasession_t *session)
{
	FuncEnter;
	FuncLeave;
	return ( session ) ? session->is_active : FALSE;
}

HINT32 nima_session_get_sock(nimasession_t *session)
{
	FuncEnter;
	FuncLeave;
	return ( session ) ? session->sock : -1;
}
