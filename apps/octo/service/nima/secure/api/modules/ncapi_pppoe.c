/*************************************************************************************************************
	File 		: ncapi_pppoe.c
	author 		: IP/OTT
	comment		: PPPoE
	date    	: 2013/11/12
	attention 	:

	(c) 2011-2013 Humax Co., Ltd.
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


#include "ncapi_pppoe.h"
#include "ncapi_client.h"



/*******************************************************************/
/********************      typedef		        ********************/
/*******************************************************************/
typedef struct
{
	NCAPI_PPPoEInfo_t	pppoe_info;
	HUINT32	semaphore;
	HxList_t *callback_list;
} st_ncapi_pppoe_object;
typedef st_ncapi_pppoe_object* pst_ncapi_pppoe_object;

st_ncapi_pppoe_object g_ncapi_pppoe_object =
{
	.callback_list = NULL,
};

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL		HxSEMT_Get(g_ncapi_pppoe_object.semaphore)
#define	LEAVE_CRITICAL		HxSEMT_Release(g_ncapi_pppoe_object.semaphore)


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/

STATIC void __ncapi_pppoe_print_info(NCAPI_PPPoEInfo_t *info)
{
	if(info)
	{
		HxLOG_Info("+++ Updated PPPoE Information ++++++++++ \n");
		HxLOG_Info("id          : %d \n", info->id);
		HxLOG_Info("enable      : %d \n", info->enable);
		HxLOG_Info("connect_id  : %s \n", info->connect_id);
		HxLOG_Info("connect_pw  : %s \n", info->connect_pw);
		HxLOG_Info("++++++++++++++++++++++++++++++++++++++++++ \n");
	}
}

STATIC void __ncapi_pppoe_update_info(NICO_PPPoE_Info_t *src, NCAPI_PPPoEInfo_t *dst)
{
	HxLOG_Debug("src : %p \n", src);
	HxLOG_Debug("dst : %p \n", dst);
	if(NULL == dst)
	{
		//HxSTD_MemSet(dst, 0x00, sizeof(*dst));
	}
	else
	{
		if(src && dst)
		{
			dst->id = src->id;
			dst->enable = src->enable;
			HxSTD_MemCopy(dst->connect_id, src->connect_id, sizeof(dst->connect_id));
			HxSTD_MemCopy(dst->connect_pw, src->connect_pw, sizeof(dst->connect_pw));

			__ncapi_pppoe_print_info(dst);
		}
	}
}

STATIC void __ncapi_pppoe_print_eventtype(eNico_event_type nico_evttype)
{
	switch(nico_evttype)
	{
		CASE_EVT_ENUMTOSTR(EVENT_NET_PPPOE_CONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_PPPOE_CONNECT_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_PPPOE_DISCONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_PPPOE_DISCONNECT_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_PPPOE_SET_INFO);
		CASE_EVT_ENUMTOSTR(EVENT_NET_PPPOE_SET_ONOFF);
		default:
			break;
	}
}

STATIC void __ncapi_pppoe_client_listener (pevent_msg event)
{
	eNico_event_type	nico_evt = event->type.evt_type;
	eNcapiPPPoEEvent	ncapi_evt = eNCAPI_PPPOE_EVENT_UNKNOWN;

	HINT32 i = 0, cb_count = 0;
	NCAPI_PPPoEInfo_t pppoe_info = {0,};
	NICO_PPPoE_Info_t *evt = NULL;

	ENTRY;
	if(EVT_PPPOE != (nico_evt & 0xffff0000)) return;

	HxLOG_Debug("Got PPPoE Event (%08x) \n", nico_evt);
	__ncapi_pppoe_print_eventtype(nico_evt);

	switch(nico_evt)
	{
		case EVENT_NET_PPPOE_CONNECT_SUCCESS:
			ncapi_evt = eNCAPI_PPPOE_EVENT_CONNECT_SUCCESS;
			evt = &event->evt_protocol.evt_pppoe.evt.evt_connect.info;
			__ncapi_pppoe_update_info(evt, &pppoe_info);
			__ncapi_pppoe_print_info(&pppoe_info);
			break;
		case EVENT_NET_PPPOE_CONNECT_FAIL:
			ncapi_evt = eNCAPI_PPPOE_EVENT_CONNECT_FAIL;
			evt = &event->evt_protocol.evt_pppoe.evt.evt_connect.info;
			__ncapi_pppoe_update_info(evt, &pppoe_info);
			__ncapi_pppoe_print_info(&pppoe_info);
			break;
		case EVENT_NET_PPPOE_DISCONNECT_SUCCESS:
			ncapi_evt = eNCAPI_PPPOE_EVENT_DISCONNECT_SUCCESS;
			evt = &event->evt_protocol.evt_pppoe.evt.evt_disconnect.info;
			__ncapi_pppoe_update_info(evt, &pppoe_info);
			__ncapi_pppoe_print_info(&pppoe_info);
			break;
		case EVENT_NET_PPPOE_DISCONNECT_FAIL:
			ncapi_evt = eNCAPI_PPPOE_EVENT_DISCONNECT_FAIL;
			evt = &event->evt_protocol.evt_pppoe.evt.evt_disconnect.info;
			__ncapi_pppoe_update_info(evt, &pppoe_info);
			__ncapi_pppoe_print_info(&pppoe_info);
			break;
		case EVENT_NET_PPPOE_SET_INFO:
			ncapi_evt = eNCAPI_PPPOE_EVENT_SET_INFO;
			evt = &event->evt_protocol.evt_pppoe.evt.evt_info.info;
			__ncapi_pppoe_update_info(evt, &pppoe_info);
			__ncapi_pppoe_print_info(&pppoe_info);
			break;
		case EVENT_NET_PPPOE_SET_ONOFF:
			ncapi_evt = eNCAPI_PPPOE_EVENT_SET_ONOFF;
			evt = &event->evt_protocol.evt_pppoe.evt.evt_info.info;
			__ncapi_pppoe_update_info(evt, &pppoe_info);
			__ncapi_pppoe_print_info(&pppoe_info);
			break;
		default:
			EXIT;
			return;
	}
	{
		cb_count = HLIB_LIST_Length(g_ncapi_pppoe_object.callback_list);
		for(i = 0; i < cb_count ; i++)
		{
			HxList_t *pFound = HLIB_LIST_GetListItem(g_ncapi_pppoe_object.callback_list, i);
			if(pFound)
			{
				NCAPI_PPPoECallback callback = (NCAPI_PPPoECallback)HLIB_LIST_Data(pFound);
				callback(ncapi_evt, &pppoe_info);
			}
		}
	}
	EXIT;
}

HERROR NCAPI_PPPoE_Init(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;

	hRet = HxSEMT_Create(&g_ncapi_pppoe_object.semaphore, "ncapi_pppoe_sem", 0);
	NCAPI_Client_RegisterCallback(__ncapi_pppoe_client_listener);
	EXIT;
	return hRet;
}

HERROR NCAPI_PPPoE_Deinit(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = HxSEMT_Destroy(g_ncapi_pppoe_object.semaphore);
	NCAPI_Client_UnregisterCallback(__ncapi_pppoe_client_listener);

	HxSTD_MemSet(&g_ncapi_pppoe_object, 0x00, sizeof(st_ncapi_pppoe_object));
	EXIT;
	return hRet;
}

HERROR NCAPI_PPPoE_RegisterCallback(NCAPI_PPPoECallback callback)
{
	HERROR hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;
	list = HLIB_LIST_Find(g_ncapi_pppoe_object.callback_list, callback);
	if (list)
	{
		HxLOG_Error("[%s] pfnObjectHandler(%X) is already registered!!!\n", __FUNCTION__, callback);
		hRet = ERR_FAIL;
	}
	else
	{
		g_ncapi_pppoe_object.callback_list = HLIB_LIST_Append(g_ncapi_pppoe_object.callback_list, (void*)callback);
	}
	LEAVE_CRITICAL;
	EXIT;
	return hRet;
}

HERROR NCAPI_PPPoE_UnregisterCallback(NCAPI_PPPoECallback callback)
{
	HERROR	hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;

	list = HLIB_LIST_Find(g_ncapi_pppoe_object.callback_list, callback);
	if (list == NULL)
	{
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", callback);
		hRet = ERR_FAIL;
	}
	else
	{
		g_ncapi_pppoe_object.callback_list = HLIB_LIST_Remove(g_ncapi_pppoe_object.callback_list, callback);
	}
	LEAVE_CRITICAL;
	EXIT;
	return hRet;
}

HERROR NCAPI_PPPoE_Connect(HINT32 id, const NCAPI_PPPoEInfo_t *info)
{
	HERROR hRet = ERR_FAIL;
	command_msg *msg = NULL;
	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_PPPOE_CONNECT;
		msg->cmd_protocol.cmd_pppoe.cmd.cmd_set_connect.id = id;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}

	EXIT;
	return hRet;
}

HERROR NCAPI_PPPoE_Disconnect(HINT32 id)
{
	HERROR hRet = ERR_FAIL;
	command_msg *msg = NULL;
	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_PPPOE_DISCONNECT;
		msg->cmd_protocol.cmd_pppoe.cmd.cmd_set_disconnect.id = id;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;
	return hRet;
}

HERROR NCAPI_PPPoE_GetInfo(HINT32 id, NCAPI_PPPoEInfo_t *info)
{
	HERROR hRet = ERR_FAIL;
	command_msg *msg = NULL;
	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_GET_PPPOE_INFO;
		msg->cmd_protocol.cmd_pppoe.cmd.cmd_get_infodb.id = id;
		hRet = NCAPI_Client_Send(msg);

		if(ERR_OK == hRet)
		{
			NICO_PPPoE_Info_t *pppoe_info = &msg->cmd_protocol.cmd_pppoe.cmd.cmd_get_infodb.info;
			__ncapi_pppoe_update_info(pppoe_info, info);
		}
		HLIB_STD_MemFree(msg);
	}

	EXIT;
	return hRet;
}

HERROR NCAPI_PPPoE_SetInfo(HINT32 id, const NCAPI_PPPoEInfo_t *info)
{
	HERROR hRet = ERR_FAIL;
	command_msg *msg = NULL;
	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_PPPOE_INFO;
		msg->cmd_protocol.cmd_pppoe.cmd.cmd_set_infodb.id = id;

		if(info)
		{
			HxSTD_MemCopy(&msg->cmd_protocol.cmd_pppoe.cmd.cmd_set_infodb.info,
							info, sizeof(NCAPI_PPPoEInfo_t));
		}
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}

	EXIT;
	return hRet;
}

HERROR NCAPI_PPPoE_GetOnOff(HINT32 id, NCAPI_PPPoEInfo_t *info)
{
	HERROR hRet = ERR_FAIL;

	/* TODO */

	return hRet;
}

HERROR NCAPI_PPPoE_SetOnOff(HINT32 id, HBOOL enable)
{
	HERROR hRet = ERR_FAIL;
	command_msg *msg = NULL;
	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_PPPOE_ONOFF;
		msg->cmd_protocol.cmd_pppoe.cmd.cmd_set_enable.id = id;
		msg->cmd_protocol.cmd_pppoe.cmd.cmd_set_enable.enable = enable;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}

	EXIT;
	return hRet;
}

