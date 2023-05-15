/*************************************************************************************************************
	File 		: ncapi_client.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/24
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

#include <sys/types.h>
#include <unistd.h>
#include "ncapi_client.h"
#include "nico_communicator.h"


/*******************************************************************/
/*******************      template         *************************/
/*******************************************************************/
typedef struct
{
	HBOOL is_return;
	command_msg *msg;
} ncapi_client_syncjob_t;

typedef struct
{
	char 		  obj_name[OBJ_NAME_LEN];
	HUINT32		  semaphore;
	HBOOL		  is_connected;
	int			  client_handle;
	unsigned int  client_pid;
	unsigned long client_listen_taskid;
	unsigned long request_id;

	HxList_t	  *synccall_list;
	HxList_t 	  *callback_list;
	HxHASH_t	*cmd_waiting_list;

} st_ncapi_client_object;
typedef st_ncapi_client_object* pst_ncapi_client_object;

st_ncapi_client_object g_client_object =
{
	.obj_name = "ncapi_client",
	.semaphore = 0,
	.client_handle = 0,
	.client_listen_taskid = 0,
	.client_pid = 0,
	.request_id = 0,
	.is_connected = FALSE,
	.cmd_waiting_list = NULL,
};
pst_ncapi_client_object client_object = &g_client_object;

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL		HxSEMT_Get(client_object->semaphore)
#define	LEAVE_CRITICAL		HxSEMT_Release(client_object->semaphore)
#define RECV_TIMEOUT 		5000

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
STATIC HUINT32	__ncapi_client_get_requestid(void)
{
	client_object->request_id = (client_object->request_id + 1) % 0x10000;

	return (client_object->client_pid << 16) | client_object->request_id;
}


HERROR __ncapi_client_operation_listener (event_msg *arg)
{
	HERROR hRet = ERR_OK;
	HINT32 i = 0;
	pevent_msg event = NULL;
	HINT32 callback_count  = 0;

	ENTRY;

	event = (pevent_msg)arg;
	HxLOG_Debug("Received Event Data from NICO +++++++++++++++ \n");
	HxLOG_Debug("event->sync_call   : %s \n", event->sync_call==TRUE?"true":"false");
	HxLOG_Debug("event->request_id  : %08x \n", event->request_id);
	HxLOG_Debug("event->deviceid    : %d \n", event->device_id);
	HxLOG_Debug("event->event type  : %08x \n", event->type.evt_type);
	HxLOG_Debug("+++++++++++++++++++++++++++++++++++++++++++++ \n");

	callback_count = HLIB_LIST_Length(client_object->callback_list);
	HxLOG_Debug("Callback Count registered on Client : %d \n", callback_count);

	for(i = 0; i < callback_count ; i++)
	{
		HxList_t *list = HLIB_LIST_GetListItem(client_object->callback_list, i);
		if(list)
		{
			NCAPI_ClientCallback callback = (NCAPI_ClientCallback)HLIB_LIST_Data(list);
			callback(event);
		}
	}
	EXIT;
	return hRet;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR NCAPI_Client_Init( void )
{
	HERROR hRet = ERR_OK;
	ENTRY;

	nico_communicator_register_callback(__ncapi_client_operation_listener);
	if (TRUE != client_object->is_connected)
	{
		HxSEMT_Create(&client_object->semaphore, "ncapi_client_sem", 0);
		client_object->is_connected 	= TRUE;
	}
	EXIT;
	return hRet;
}

HERROR NCAPI_Client_Deinit( void )
{
	HERROR hRet = ERR_OK;
	ENTRY;

	hRet = HxSEMT_Destroy(client_object->semaphore);
	HxSTD_MemSet(client_object, 0x00, sizeof(st_ncapi_client_object));
	return hRet;
}

HERROR NCAPI_Client_RegisterCallback(NCAPI_ClientCallback callback)
{
	HERROR hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;
	list = HLIB_LIST_Find(client_object->callback_list, callback);
	if (list)
	{
		HxLOG_Error("[%s] pfnObjectHandler(%X) is already registered!!!\n", __FUNCTION__, callback);
		hRet = ERR_FAIL;
	}
	else
	{
		client_object->callback_list = HLIB_LIST_Append(client_object->callback_list, (void*)callback);


	}
	LEAVE_CRITICAL;
	return hRet;

}
HERROR NCAPI_Client_UnregisterCallback(NCAPI_ClientCallback callback)
{
	HERROR	hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;
	list = HLIB_LIST_Find(client_object->callback_list, callback);
	if (list == NULL)
	{
		HxLOG_Error("[%s] pfnObjectHandler(%X) does not registered yet..\n", __FUNCTION__, callback);
		hRet = ERR_FAIL;
	}
	else
	{
		client_object->callback_list = HLIB_LIST_Remove(client_object->callback_list, callback);
	}
	LEAVE_CRITICAL;
	return hRet;

}

HERROR NCAPI_Client_Send(pcommand_msg command)
{
	HERROR hRet = ERR_OK;
	command_msg *retmsg;

	ENTER_CRITICAL;
	ENTRY;
	command->request_id = __ncapi_client_get_requestid();

	HxLOG_Debug("[%s:%d] Send Command+++++++++++++++++++++ ++++++++ \n",
				__FUNCTION__, __LINE__);
	HxLOG_Debug("[%s:%d] command->sync_call   : %s \n",
				__FUNCTION__, __LINE__, command->sync_call==TRUE?"true":"false");
	HxLOG_Debug("[%s:%d] command->request_id  : %08x \n",
				__FUNCTION__, __LINE__, command->request_id);
	HxLOG_Debug("[%s:%d] command->deviceid    : %d \n",
				__FUNCTION__, __LINE__, command->device_id);
	HxLOG_Debug("[%s:%d] command->cmd_type    : %08x \n",
				__FUNCTION__, __LINE__, command->type.cmd_type);
	HxLOG_Debug("[%s:%d] ++++++++++++++++++++++++++++++++++++++++++ \n",
				__FUNCTION__, __LINE__);

	if(NULL != (retmsg = nico_communicator_do_command(command))) //nico_communicator_do_command
	{
		HxSTD_MemCopy(command, retmsg, sizeof(command_msg));
		SAFE_FREE(retmsg);
	}

	LEAVE_CRITICAL;
	EXIT;
	return hRet;
}

