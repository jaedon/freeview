/*************************************************************************************************************
	File 		: ncapi_bluetooth.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/27
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

	THE SOFTWARE IS PROVIDED ¢®¡ÆAS IS¢®¡¾ AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
#include "ncapi_bluetooth.h"
#include "ncapi_client.h"
#include "ncapi_codec.h"

#include <sys/types.h>
#include <unistd.h>

/*******************************************************************/
/********************      typedef		        ********************/
/*******************************************************************/

typedef struct
{
	NCAPI_BTInfo_t btInfo;
	HUINT32	semaphore;
	HxList_t *callback_list;
} st_ncapi_bt_object;
typedef st_ncapi_bt_object* pst_ncapi_bt_object;

st_ncapi_bt_object g_ncapi_bt_object =
{
	//.apInfo,
	.callback_list = NULL,
};
pst_ncapi_bt_object bt_object = &g_ncapi_bt_object;

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL		HxSEMT_Get(bt_object->semaphore)
#define	LEAVE_CRITICAL		HxSEMT_Release(bt_object->semaphore)

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/

STATIC void __ncapi_bt_print_devinfo(NCAPI_BTDevInfo_t *info, HINT32 count)
{
	HINT32 i = 0;

	if(!info) return;

	for( i=0; i<count; i++ )
	{
		HxLOG_Info("+++ Updated BT Information ++++++++++ \n");
		HxLOG_Info("name       	  	  : %s \n", info[i].name);
		HxLOG_Info("bdaddr        	  : %02X-%02X-%02X-%02X-%02X-%02X \n",
								info[i].bdaddr[0],info[i].bdaddr[1],
								info[i].bdaddr[2],info[i].bdaddr[3],
								info[i].bdaddr[4],info[i].bdaddr[5]);
		HxLOG_Info("class         	  : %02X/%02X/%02X\n",
								info[i].dev_class[0],
								info[i].dev_class[1],
								info[i].dev_class[2]);
		HxLOG_Info("status        	  : %d \n", info[i].status);
		HxLOG_Info("abailable profile : %d \n", info[i].available_profile);
		HxLOG_Info("++++++++++++++++++++++++++++++++++++++++++ \n");
	}

}

STATIC void __ncapi_bt_update_devinfo(NICO_BT_Info_t *src, NCAPI_BTInfo_t *dst)
{
	HINT32 index=0;
	HxLOG_Debug("src : %p \n", src);
	HxLOG_Debug("dst : %p \n", dst);

	if( !src || !dst ) return;

	HxSTD_MemSet(dst, 0x00, sizeof(*dst));

	dst->enable = src->enable;
	dst->count = src->count;
	dst->passkey = src->passkey;

// host device info
	dst->host_info.status = src->host_info.status;
	dst->host_info.available_profile = src->host_info.available_profile;
	HxSTD_MemCopy(dst->host_info.name, src->host_info.name, NICO_BT_NAME_MAX);
	HxSTD_MemCopy(dst->host_info.bdaddr, src->host_info.bdaddr, NICO_BT_BDADDR_LEN);
	HxSTD_MemCopy(dst->host_info.dev_class, src->host_info.dev_class, NICO_BT_CLASS_LEN);
	HxSTD_MemCopy(dst->connected_bdaddr, src->connected_bdaddr, NICO_BT_BDADDR_LEN);

	for( index = 0; index < dst->count; index++ )
	{
		dst->dev_list[index].status = src->dev_list[index].status;
		dst->dev_list[index].available_profile = src->dev_list[index].available_profile;
		HxSTD_MemCopy(dst->dev_list[index].name, src->dev_list[index].name, NICO_BT_NAME_MAX);
		HxSTD_MemCopy(dst->dev_list[index].bdaddr, src->dev_list[index].bdaddr, NICO_BT_BDADDR_LEN);
		HxSTD_MemCopy(dst->dev_list[index].dev_class, src->dev_list[index].dev_class, NICO_BT_CLASS_LEN);
	}

	__ncapi_bt_print_devinfo(dst->dev_list, dst->count);
}

STATIC void __ncapi_bt_print_eventtype(eNico_event_type nico_evttype)
{
	switch(nico_evttype)
	{
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_INSERTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_EXTRACTED);

		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_ENABLE);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_DISABLE);

		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_SCAN_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_SCAN_FAIL);

		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_AUTHEN_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_AUTHEN_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_PASSKEY);

		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_CONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_CONNECT_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_REMOVED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_REMOTE_CMD);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_MAX);
		default:
			break;
	}
}

//1 #### BT Event Listener
STATIC void __ncapi_bt_client_listener (pevent_msg event)
{
	eNcapiBTEvent		ncapi_evttype = eNCAPI_BT_EVENT_MAX;
	eNico_event_type	nico_evttype  = event->type.evt_type;
	NCAPI_BTInfo_t		bt_info;
	HINT32 i = 0, cb_count = 0;
	NICO_BT_Info_t *recvInfo = NULL;

	HxSTD_MemSet(&bt_info, 0x00, sizeof(NCAPI_BTInfo_t));
	ENTRY;

	if(!(EVT_BT != (nico_evttype & 0xffff0000)))
	{
		HxLOG_Debug("Got BT Event (%08x) \n", nico_evttype);
		__ncapi_bt_print_eventtype(nico_evttype);
		switch(nico_evttype)
		{
			case EVENT_NET_BT_INSERTED:
				ncapi_evttype = eNCAPI_BT_EVENT_INSERTED;
				break;
			case EVENT_NET_BT_EXTRACTED:
				ncapi_evttype = eNCAPI_BT_EVENT_EXTRACTED;
				break;
			case EVENT_NET_BT_ENABLE:
				ncapi_evttype = eNCAPI_BT_EVENT_ENABLE;
				break;
			case EVENT_NET_BT_DISABLE:
				ncapi_evttype = eNCAPI_BT_EVENT_DISABLE;
				break;
			case EVENT_NET_BT_SCAN_SUCCESS:
				ncapi_evttype = eNCAPI_BT_EVENT_SCAN_SUCCESS;
				break;
			case EVENT_NET_BT_SCAN_FAIL:
				ncapi_evttype = eNCAPI_BT_EVENT_SCAN_FAIL;
				break;
			case EVENT_NET_BT_AUTHEN_SUCCESS:
				ncapi_evttype = eNCAPI_BT_EVENT_AUTHEN_SUCCESS;
				break;
			case EVENT_NET_BT_AUTHEN_FAIL:
				ncapi_evttype = eNCAPI_BT_EVENT_AUTHEN_FAIL;
				break;
			case EVENT_NET_BT_PASSKEY:
				ncapi_evttype = eNCAPI_BT_EVENT_PASSKEY;
				break;		
			case EVENT_NET_BT_CONNECT_SUCCESS:
				ncapi_evttype = eNCAPI_BT_EVENT_CONNECT_SUCCESS;
				break;
			case EVENT_NET_BT_CONNECT_FAIL:
				ncapi_evttype = eNCAPI_BT_EVENT_CONNECT_FAIL;
				break;
			case EVENT_NET_BT_REMOVED:
				ncapi_evttype = eNCAPI_BT_EVENT_REMOVED;
				break;
			case EVENT_NET_BT_REMOTE_CMD:
				ncapi_evttype = eNCAPI_BT_EVENT_REMOTE_CMD;
				break;
			case EVENT_NET_BT_MAX:
				ncapi_evttype = eNCAPI_BT_EVENT_MAX;
				break;
			default:
				EXIT;
				return;
		}

		cb_count = HLIB_LIST_Length(bt_object->callback_list);
		recvInfo = (NICO_BT_Info_t *)&event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info;
		if( recvInfo )
			__ncapi_bt_update_devinfo(recvInfo, &bt_info);

		for(i = 0; i < cb_count ; i++)
		{
			HxList_t *pFound = HLIB_LIST_GetListItem(bt_object->callback_list, i);
			if(pFound)
			{
				NCAPI_BTCallback callback = (NCAPI_BTCallback)HLIB_LIST_Data(pFound);
				callback(ncapi_evttype, &bt_info);
			}
		}
	}

	EXIT;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR NCAPI_BT_Init(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = HxSEMT_Create(&bt_object->semaphore, "ncapi_bluetooth_sem", 0);
	NCAPI_Client_RegisterCallback(__ncapi_bt_client_listener);
	EXIT;
	return hRet;
}
HERROR NCAPI_BT_Deinit(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = HxSEMT_Destroy(bt_object->semaphore);
	NCAPI_Client_UnregisterCallback(__ncapi_bt_client_listener);

	HxSTD_MemSet(bt_object, 0x00, sizeof(st_ncapi_bt_object));
	EXIT;
	return hRet;
}
HERROR NCAPI_BT_RegisterCallback(NCAPI_BTCallback callback)
{
	HERROR hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;
	list = HLIB_LIST_Find(bt_object->callback_list, callback);
	if (list)
	{
		HxLOG_Error("[%s] pfnObjectHandler(%X) is already registered!!!\n", __FUNCTION__, callback);
		hRet = ERR_FAIL;
	}
	else
	{
		bt_object->callback_list = HLIB_LIST_Append(bt_object->callback_list, (void*)callback);
	}
	LEAVE_CRITICAL;
	EXIT;
	return hRet;
}


HERROR NCAPI_BT_UnregisterCallback(NCAPI_BTCallback callback)
{
	HERROR	hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;

	list = HLIB_LIST_Find(bt_object->callback_list, callback);
	if (list == NULL)
	{
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", callback);
		hRet = ERR_FAIL;
	}
	else
	{
		bt_object->callback_list = HLIB_LIST_Remove(bt_object->callback_list, callback);
	}
	LEAVE_CRITICAL;
	EXIT;
	return hRet;

}


HERROR NCAPI_BT_SetEnable(HBOOL enable)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_BT_ENABLE;
		msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_enable.enable = enable;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_BT_StartScan(eNcapiBTProfile profile)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_BT_START_SCAN;
		msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_startscan.profile = profile;
				hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_BT_SendPinCode(HUINT8 *bdaddr, HUINT8 *pincode)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_BT_PINCODE;
		if(bdaddr)
		{
			HxSTD_MemCopy(&msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_pincode.bdaddr, 
				bdaddr, NCAPI_BT_BDADDR_LEN);
		}
		if(pincode)
		{
			HxSTD_MemCopy(&msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_pincode.pincode,
				pincode, NCAPI_BT_PINCODE_LEN);
		}
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}

	EXIT;

	return hRet;
}

HERROR NCAPI_BT_Connect(HUINT8 *bdaddr)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_BT_CONNECT;
		if(bdaddr)
		{
			HxSTD_MemCopy(&msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_connect.bdaddr,
							bdaddr, NCAPI_BT_BDADDR_LEN);
		}
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_BT_Disconnect(HUINT8 *bdaddr)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_BT_DISCONNECT;
		if(bdaddr)
		{
			HxSTD_MemCopy(&msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_disconnect.bdaddr,
							bdaddr, NCAPI_BT_BDADDR_LEN);
		}
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_BT_Remove(HUINT8 *bdaddr)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_BT_REMOVE;
		if(bdaddr)
		{
			HxSTD_MemCopy(&msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_remove.bdaddr,
							bdaddr, NCAPI_BT_BDADDR_LEN);
		}
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;
	
	return hRet;
}

HERROR NCAPI_BT_OpenProfile(HUINT8 *bdaddr, eNcapiBTProfile profile)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_BT_OPEN_PROFILE;
		msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_openprofile.profile = profile;
		if(bdaddr)
		{
			HxSTD_MemCopy(&msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_openprofile.bdaddr,
							bdaddr, NCAPI_BT_BDADDR_LEN);
		}
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;
	
	return hRet;
}

HERROR NCAPI_BT_CloseProfile(HUINT8 *bdaddr, eNcapiBTProfile profile)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_BT_CLOSE_PROFILE;
		msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_closeprofile.profile = profile;
		if(bdaddr)
		{
			HxSTD_MemCopy(&msg->cmd_protocol.cmd_bluetooth.cmd.cmd_set_closeprofile.bdaddr,
							bdaddr, NCAPI_BT_BDADDR_LEN);
		}
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;
	
	return hRet;
}

HERROR NCAPI_BT_GetHostInfo(NCAPI_BTDevInfo_t *info)
{
	HERROR hRet = ERR_FAIL;
	NICO_BT_Info_t bt_info;
	NCAPI_BTInfo_t dst_info;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->type.cmd_type = CMD_GET_BT_HOST_INFO;
		hRet = NCAPI_Client_Send(msg);
		if(ERR_OK == hRet)
		{
			NICO_BT_DevInfo_t *dev_info = (NICO_BT_DevInfo_t*)&msg->cmd_protocol.cmd_bluetooth.cmd.cmd_get_hostinfo.dev_info;
			HxSTD_MemSet(&bt_info, 0x00, sizeof(NICO_BT_Info_t));
			HxSTD_MemCopy(&bt_info.host_info, dev_info, sizeof(NICO_BT_DevInfo_t));
			__ncapi_bt_update_devinfo(&bt_info, &dst_info);
			if( info )
				HxSTD_MemCopy(info, &dst_info.host_info, sizeof(NCAPI_BTDevInfo_t));
		}
		HLIB_STD_MemFree(msg);
	}
	EXIT;
	
	return hRet;
}

HERROR NCAPI_BT_GetDevList(NCAPI_BTInfo_t *info)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE,
		msg->type.cmd_type = CMD_GET_BT_DEVICE_LIST,
		hRet = NCAPI_Client_Send(msg);
		if(ERR_OK == hRet)
		{
			NICO_BT_Info_t *bt_info = (NICO_BT_Info_t*)&msg->cmd_protocol.cmd_bluetooth.cmd.cmd_get_devlist.info;
			if(bt_info)
			{
				info->count = bt_info->count;
				__ncapi_bt_update_devinfo(bt_info, info);
			}
		}
		HLIB_STD_MemFree(msg);
	}
	EXIT;
	
	return hRet;
}

HERROR NCAPI_BT_Remove_DB(void)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_BT_REMOVE_DB;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}


