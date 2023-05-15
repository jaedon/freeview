/*************************************************************************************************************
	File 		: ncapi_netconf.c
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

#include "ncapi_netconf.h"

/*******************************************************************/
/********************      typedef		        ********************/
/*******************************************************************/
typedef struct
{
	NCAPI_NetConfInfo_t	netInfo;
	HUINT32	semaphore;
	HxList_t *callback_list;
} st_ncapi_netconf_object;
typedef st_ncapi_netconf_object* pst_ncapi_netconf_object;

st_ncapi_netconf_object g_ncapi_netconf_object =
{
	//.netInfo,
	.callback_list = NULL,
};

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL		HxSEMT_Get(g_ncapi_netconf_object.semaphore)
#define	LEAVE_CRITICAL		HxSEMT_Release(g_ncapi_netconf_object.semaphore)


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
STATIC void __ncapi_netconf_printf_netinfo(NCAPI_NetConfInfo_t *info)
{
	if(info)
	{
		HxLOG_Info("+++ Updated Net Information ++++++++++ \n");
		HxLOG_Info("device id  : %d \n", info->id);
		HxLOG_Info("priority   : %d \n", info->priority);
		HxLOG_Info("type type  : %d \n", info->type);
		HxLOG_Info("linked     : %d \n", info->linked);
		HxLOG_Info("active     : %d \n", info->active);
		HxLOG_Info("auto_ip    : %d \n", info->auto_ip);
		HxLOG_Info("dhcp       : %d \n", info->dhcp);
		HxLOG_Info("dns_auto   : %d \n", info->dns_auto);
		HxLOG_Info("ip ver     : %s \n", info->ip_ver == eNCAPI_NETCONF_IPV4 ? "ipv4":"ipv6");

		HxLOG_Info("[%s:%d] mac        : %02x-%02x-%02x-%02x-%02x-%02x \n",
									__FUNCTION__, __LINE__,
									info->ip_info.ipv4.macaddr[0], info->ip_info.ipv4.macaddr[1],
									info->ip_info.ipv4.macaddr[2], info->ip_info.ipv4.macaddr[3],
									info->ip_info.ipv4.macaddr[4], info->ip_info.ipv4.macaddr[5]);
		HxLOG_Info("[%s:%d] ip address : %3d.%3d.%3d.%3d \n",
									__FUNCTION__, __LINE__,
									info->ip_info.ipv4.ipAddr[0], info->ip_info.ipv4.ipAddr[1],
									info->ip_info.ipv4.ipAddr[2], info->ip_info.ipv4.ipAddr[3]);
		HxLOG_Info("[%s:%d] netmask    : %3d.%3d.%3d.%3d \n",
									__FUNCTION__, __LINE__,
									info->ip_info.ipv4.netmask[0], info->ip_info.ipv4.netmask[1],
									info->ip_info.ipv4.netmask[2], info->ip_info.ipv4.netmask[3]);
		HxLOG_Info("[%s:%d] gateway    : %3d.%3d.%3d.%3d \n",
									__FUNCTION__, __LINE__,
									info->ip_info.ipv4.gateway[0], info->ip_info.ipv4.gateway[1],
									info->ip_info.ipv4.gateway[2], info->ip_info.ipv4.gateway[3]);
		{
			HUINT32 i = 0;
			for(i=0; i<eNCAPI_Dns_Max;i++)
			{
				HxLOG_Info("[%s:%d] dns       : %3d.%3d.%3d.%3d \n",
									__FUNCTION__, __LINE__,
									info->ip_info.ipv4.dns[i][0], info->ip_info.ipv4.dns[i][1],
									info->ip_info.ipv4.dns[i][2], info->ip_info.ipv4.dns[i][3]);
			}
		}
	}
}

STATIC eNcapiNetConfDevType __ncapi_netconf_get_ncapi_devtype(eNicoNetConfDevType dev_type)
{
	eNcapiNetConfDevType eRet = eNCAPI_NETCONF_MAX;
	switch(dev_type)
	{
		case eNICO_DEVTYPE_LAN:
			eRet = eNCAPI_NETCONF_LAN;
			break;
		case eNICO_DEVTYPE_WAN:
			eRet = eNCAPI_NETCONF_WAN;
			break;
		case eNICO_DEVTYPE_CM:
			eRet = eNCAPI_NETCONF_CM;
			break;
		case eNICO_DEVTYPE_WIFI:
			eRet = eNCAPI_NETCONF_WIFI;
			break;
		case eNICO_DEVTYPE_BLUETOOTH:
			eRet = eNCAPI_NETCONF_BLUETOOTH;
			break;
		case eNICO_DEVTYPE_RF4CE:
			eRet = eNCAPI_NETCONF_RF4CE;
			break;
		case eNICO_DEVTYPE_PPPOE:
			eRet = eNCAPI_NETCONF_PPPOE;
			break;
		case eNICO_DEVTYPE_MAX:
			eRet = eNCAPI_NETCONF_MAX;
			break;
	}
	return eRet;
}
STATIC  eNicoNetConfDevType __ncapi_netconf_get_nico_devtype( eNcapiNetConfDevType dev_type)
{
	eNicoNetConfDevType eRet = eNICO_DEVTYPE_MAX;
	switch(dev_type)
	{
		case eNCAPI_NETCONF_LAN :
			eRet = eNICO_DEVTYPE_LAN;
			break;
		case eNCAPI_NETCONF_WAN :
			eRet = eNICO_DEVTYPE_WAN ;
			break;
		case eNCAPI_NETCONF_CM :
			eRet = eNICO_DEVTYPE_CM;
			break;
		case eNCAPI_NETCONF_WIFI :
			eRet = eNICO_DEVTYPE_WIFI;
			break;
		case eNCAPI_NETCONF_BLUETOOTH :
			eRet = eNICO_DEVTYPE_BLUETOOTH;
			break;
		case eNCAPI_NETCONF_RF4CE :
			eRet = eNICO_DEVTYPE_RF4CE;
			break;
		case eNCAPI_NETCONF_PPPOE:
			eRet = eNICO_DEVTYPE_PPPOE;
			break;
		case eNCAPI_NETCONF_MAX :
			eRet = eNICO_DEVTYPE_MAX;
			break;
	}
	return eRet;
}

STATIC void __ncapi_netconf_convert_netinfo_ncapi_to_nico(const NCAPI_NetConfInfo_t *src, NICO_NetConf_Info_t *dst)
{
	if(src && dst)
	{
		dst->id = src->id;
		dst->priority = src->priority;
		dst->dev_type = __ncapi_netconf_get_nico_devtype(src->type);
		dst->linked = src->linked;
		dst->active = src->active;
		dst->auto_ip = src->auto_ip;
		dst->dhcp = src->dhcp;
		dst->dns_auto = src->dns_auto;
		dst->ip_ver = src->ip_ver;
		HxSTD_MemCopy(dst->name, src->name, sizeof(dst->name));

		switch(dst->ip_ver)
		{
			case eNCAPI_NETCONF_IPV4:
				HxSTD_MemCopy(&dst->ip_info.ipv4.macaddr, &src->ip_info.ipv4.macaddr, sizeof(dst->ip_info.ipv4.macaddr));
				HxSTD_MemCopy(&dst->ip_info.ipv4.ipAddr, &src->ip_info.ipv4.ipAddr, sizeof(dst->ip_info.ipv4.ipAddr));
				HxSTD_MemCopy(&dst->ip_info.ipv4.netmask, &src->ip_info.ipv4.netmask, sizeof(dst->ip_info.ipv4.netmask));
				HxSTD_MemCopy(&dst->ip_info.ipv4.gateway, &src->ip_info.ipv4.gateway, sizeof(dst->ip_info.ipv4.gateway));
				HxSTD_MemCopy(&dst->ip_info.ipv4.broadcast, &src->ip_info.ipv4.broadcast, sizeof(dst->ip_info.ipv4.broadcast));
				HxSTD_MemCopy(&dst->ip_info.ipv4.dns[eNCAPI_Dns_Primary], &src->ip_info.ipv4.dns[eNCAPI_Dns_Primary], sizeof(dst->ip_info.ipv4.dns[eNCAPI_Dns_Primary]));
				HxSTD_MemCopy(&dst->ip_info.ipv4.dns[eNCAPI_Dns_Secondary], &src->ip_info.ipv4.dns[eNCAPI_Dns_Secondary], sizeof(dst->ip_info.ipv4.dns[eNCAPI_Dns_Secondary]));
				break;
			case eNCAPI_NETCONF_IPV6:
				/* TODO */
				break;
		}
	}
}

STATIC void __ncapi_netconf_convert_netinfo_nico_to_ncapi(NICO_NetConf_Info_t *src, NCAPI_NetConfInfo_t *dst)
{
	if(src && dst)
	{
		dst->id = src->id;
		dst->priority = src->priority;
		dst->type = __ncapi_netconf_get_ncapi_devtype(src->dev_type);
		dst->linked = src->linked;
		dst->active = src->active;
		dst->auto_ip = src->auto_ip;
		dst->dhcp = src->dhcp;
		dst->dns_auto = src->dns_auto;
		dst->ip_ver = src->ip_ver;
		HxSTD_MemCopy(dst->name, src->name, sizeof(dst->name));

		switch(dst->ip_ver)
		{
			case eNCAPI_NETCONF_IPV4:
				HxSTD_MemCopy(&dst->ip_info.ipv4.macaddr, &src->ip_info.ipv4.macaddr, sizeof(dst->ip_info.ipv4.macaddr));
				HxSTD_MemCopy(&dst->ip_info.ipv4.ipAddr, &src->ip_info.ipv4.ipAddr, sizeof(dst->ip_info.ipv4.ipAddr));
				HxSTD_MemCopy(&dst->ip_info.ipv4.netmask, &src->ip_info.ipv4.netmask, sizeof(dst->ip_info.ipv4.netmask));
				HxSTD_MemCopy(&dst->ip_info.ipv4.gateway, &src->ip_info.ipv4.gateway, sizeof(dst->ip_info.ipv4.gateway));
				HxSTD_MemCopy(&dst->ip_info.ipv4.broadcast, &src->ip_info.ipv4.broadcast, sizeof(dst->ip_info.ipv4.broadcast));
				HxSTD_MemCopy(&dst->ip_info.ipv4.dns[eNCAPI_Dns_Primary], &src->ip_info.ipv4.dns[eNCAPI_Dns_Primary], sizeof(dst->ip_info.ipv4.dns[eNCAPI_Dns_Primary]));
				HxSTD_MemCopy(&dst->ip_info.ipv4.dns[eNCAPI_Dns_Secondary], &src->ip_info.ipv4.dns[eNCAPI_Dns_Secondary], sizeof(dst->ip_info.ipv4.dns[eNCAPI_Dns_Secondary]));
				break;
			case eNCAPI_NETCONF_IPV6:
				break;
		}
		HLIB_STD_StrNCpySafe(dst->fw_ver, src->fw_ver, NCAPI_FW_VERSION_MAX_LEN);
	}
}

STATIC void __ncapi_netconf_print_eventtype(eNico_event_type nico_evttype)
{
	switch(nico_evttype)
	{
		CASE_EVT_ENUMTOSTR(EVENT_NET_CONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_CONNECT_FAIL);

		CASE_EVT_ENUMTOSTR(EVENT_NET_CONNECT_AUTOIP_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_CONNECT_AUTOIP_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_PLUGGED_IN);

		CASE_EVT_ENUMTOSTR(EVENT_NET_PLUGGED_OUT);
		CASE_EVT_ENUMTOSTR(EVENT_NET_STATUS_UPDATED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_STOP_CONNECTING);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WOL_ENABLED);

		CASE_EVT_ENUMTOSTR(EVENT_NET_WOL_DISABLED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_EXT_STARTED_DHCPSERVER);
		CASE_EVT_ENUMTOSTR(EVENT_NET_EXT_STOPPED_DHCPSERVER);
		default:
			break;
	}
}

STATIC void __ncapi_netconf_client_listener (pevent_msg event)
{
	eNico_event_type	nico_evt = event->type.evt_type;
	eNcapiNetConfEvent	ncapi_evt = eNCAPI_NETCONF_EVENT_UNKNOWN;

	ENTRY;
	if(EVT_NETCONF != (nico_evt & 0xffff0000)) return;

	HxLOG_Debug("Got Netconf Event (%08x) \n", nico_evt);
	__ncapi_netconf_print_eventtype(nico_evt);

	switch(nico_evt)
	{
		case EVENT_NET_CONNECT_SUCCESS:
			ncapi_evt = eNCAPI_NETCONF_EVENT_CONN_SUCCESS;
			break;
		case EVENT_NET_CONNECT_FAIL:
			ncapi_evt = eNCAPI_NETCONF_EVENT_CONN_FAIL;
			break;
		case EVENT_NET_CONNECT_AUTOIP_SUCCESS:
			ncapi_evt = eNCAPI_NETCONF_EVENT_AUTOIP_SUCCESS;
			break;
		case EVENT_NET_CONNECT_AUTOIP_FAIL:
			ncapi_evt = eNCAPI_NETCONF_EVENT_AUTOIP_FAIL;
			break;
		case EVENT_NET_PLUGGED_IN:
			ncapi_evt = eNCAPI_NETCONF_EVENT_LINK_CONNECTED;
			break;
		case EVENT_NET_PLUGGED_OUT:
			ncapi_evt = eNCAPI_NETCONF_EVENT_LINK_DISCONNECTED;
			break;
		case EVENT_NET_STATUS_UPDATED:
			ncapi_evt = eNCAPI_NETCONF_EVENT_STATUS_UPDATED;
			break;
		case EVENT_NET_WOL_ENABLED:
			ncapi_evt = eNCAPI_NETCONF_EVENT_WOL_ENABLED;
			break;
		case EVENT_NET_WOL_DISABLED:
			ncapi_evt = eNCAPI_NETCONF_EVENT_WOL_DISABLED;
			break;
		case EVENT_NET_EXT_STARTED_DHCPSERVER:
			ncapi_evt = eNCAPI_NETCONF_EVENT_DHCPSERVER_START;
			break;
		case EVENT_NET_EXT_STOPPED_DHCPSERVER:
			ncapi_evt = eNCAPI_NETCONF_EVENT_DHCPSERVER_STOP;
			break;
		default:
			EXIT;
			return;
	}
	{
		HINT32 i = 0, cb_count = 0;
		NCAPI_NetConfInfo_t info = {0,};
		NICO_NetConf_Info_t *evt = &event->evt_protocol.evt_netconf.evt_netinfo;

		__ncapi_netconf_convert_netinfo_nico_to_ncapi(evt, &info);
		__ncapi_netconf_printf_netinfo(&info);

		cb_count = HLIB_LIST_Length(g_ncapi_netconf_object.callback_list);
		for(i = 0; i < cb_count ; i++)
		{
			HxList_t *pFound = HLIB_LIST_GetListItem(g_ncapi_netconf_object.callback_list, i);
			if(pFound)
			{
				NCAPI_NetConfCallback callback = (NCAPI_NetConfCallback)HLIB_LIST_Data(pFound);
				callback(ncapi_evt, event->device_id, &info);
			}
		}
	}
	EXIT;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR NCAPI_NetConf_Init(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;

	hRet = HxSEMT_Create(&g_ncapi_netconf_object.semaphore, "ncapi_netconf_sem", 0);
	NCAPI_Client_RegisterCallback(__ncapi_netconf_client_listener);
	EXIT;
	return hRet;
}

HERROR NCAPI_NetConf_Deinit(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = HxSEMT_Destroy(g_ncapi_netconf_object.semaphore);
	NCAPI_Client_UnregisterCallback(__ncapi_netconf_client_listener);

	HxSTD_MemSet(&g_ncapi_netconf_object, 0x00, sizeof(st_ncapi_netconf_object));
	EXIT;
	return hRet;
}

HERROR NCAPI_NetConf_RegisterCallback(NCAPI_NetConfCallback callback)
{
	HERROR hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;
	list = HLIB_LIST_Find(g_ncapi_netconf_object.callback_list, callback);
	if (list)
	{
		HxLOG_Error("[%s] pfnObjectHandler(%X) is already registered!!!\n", __FUNCTION__, callback);
		hRet = ERR_FAIL;
	}
	else
	{
		g_ncapi_netconf_object.callback_list = HLIB_LIST_Append(g_ncapi_netconf_object.callback_list, (void*)callback);

	}
	LEAVE_CRITICAL;
	EXIT;
	return hRet;
}


HERROR NCAPI_NetConf_UnregisterCallback(NCAPI_NetConfCallback callback)
{
	HERROR	hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;

	list = HLIB_LIST_Find(g_ncapi_netconf_object.callback_list, callback);
	if (list == NULL)
	{
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", callback);
		hRet = ERR_FAIL;
	}
	else
	{
		g_ncapi_netconf_object.callback_list = HLIB_LIST_Remove(g_ncapi_netconf_object.callback_list, callback);
	}
	LEAVE_CRITICAL;
	EXIT;
	return hRet;
}

HERROR NCAPI_NetConf_GetDeviceCount(HINT32 *device_count)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->type.cmd_type = CMD_GET_NETCONF_DEVICE_COUNT;
		hRet = NCAPI_Client_Send(msg);
		*device_count = msg->cmd_protocol.cmd_netconf.cmd.cmd_get_devicecount.device_count;
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_NetConf_GetDefaultDevIndex(HINT32 *device_id)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->type.cmd_type = CMD_GET_NETCONF_ACTIVE_DEVICEID;
		hRet = NCAPI_Client_Send(msg);
		*device_id = msg->cmd_protocol.cmd_netconf.cmd.cmd_get_activedevice.id;
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_NetConf_GetDeviceInfo(HINT32 id, NCAPI_NetConfInfo_t *info)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_GET_NETCONF_NETINFO;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_get_netinfo.id = id;
		hRet = NCAPI_Client_Send(msg);
		if(ERR_OK == hRet)
		{
			NICO_NetConf_Info_t *recvInfo = &msg->cmd_protocol.cmd_netconf.cmd.cmd_get_netinfo.info;
			__ncapi_netconf_convert_netinfo_nico_to_ncapi(recvInfo, info);
		}
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_NetConf_RestartDHCPClient(HINT32 id)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_NETCONF_RESTART_DHCP_CLIENT;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_stopconnecting.id = id;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_NetConf_StopConnect(HINT32 id)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_NETCONF_STOP_CONNECTING;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_stopconnecting.id = id;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_NetConf_SetAutoip(HINT32 id)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_NETCONF_AUTOIP;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_autoip.id = id;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}


HERROR NCAPI_NetConf_SetInfo(HINT32 id, const NCAPI_NetConfInfo_t *info)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;
	NICO_NetConf_Info_t *setInfo = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_NETCONF_DEVICE_CONFIG;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_netinfo.id = id;
		setInfo = &msg->cmd_protocol.cmd_netconf.cmd.cmd_set_netinfo.info;
		__ncapi_netconf_convert_netinfo_ncapi_to_nico(info, setInfo);
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_NetConf_StartDHCPServer(HINT32 id, eNcapiNetConfIpVersion eIpVer)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_NETCONF_START_DHCPSERVER;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_dhcpserver.id = id;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_dhcpserver.eIpVer = eIpVer;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_NetConf_StopDHCPServer(HINT32 id, eNcapiNetConfIpVersion eIpVer)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_NETCONF_STOP_DHCPSERVER;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_dhcpserver.id = id;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_dhcpserver.eIpVer = eIpVer;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_NetConf_DevUp(HINT32 id)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_NETCONF_DEVICE_ENABLE;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_devup.id = id;
		hRet = NCAPI_Client_Send(msg);
		if(TRUE != msg->cmd_protocol.cmd_netconf.cmd.cmd_set_devup.is_up)
		{
			hRet = ERR_FAIL;
		}
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_NetConf_DevDown(HINT32 id)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_NETCONF_DEVICE_DISABLE;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_devdown.id = id;
		hRet = NCAPI_Client_Send(msg);
		if(TRUE != msg->cmd_protocol.cmd_netconf.cmd.cmd_set_devdown.is_down)
		{
			hRet = ERR_FAIL;
		}
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}


HERROR NCAPI_NetConf_SetWOL(HBOOL is_enable)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_NETCONF_WOL;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_wol.is_enable = is_enable;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_NetConf_SetDevPriority(HINT32 id, HINT32 priority)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_NETCONF_DEVICE_PRIORITY;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_devpriority.id = id;
		msg->cmd_protocol.cmd_netconf.cmd.cmd_set_devpriority.priority = priority;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_Netconf_SetNTPClient(HCHAR *addr)
{
	HERROR hRet = ERR_OK;
	HINT32 dev_id = 0;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = dev_id;
		msg->type.cmd_type = CMD_SET_NETCONF_NTP_CLIENT;
		HxSTD_MemCopy(msg->cmd_protocol.cmd_netconf.cmd.cmd_set_ntpclient.addr, addr, NICO_MAX_ADDR);
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_Netconf_Renew(HCHAR *addr)
{
	HERROR hRet = ERR_OK;
	HINT32 dev_id = 0;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->device_id = dev_id;
		msg->type.cmd_type = CMD_SET_NETCONF_RENEW;
		if( !addr )
		{
			HLIB_STD_MemFree(msg);
			return ERR_FAIL;
		}
		HLIB_STD_StrNCpySafe(msg->cmd_protocol.cmd_netconf.cmd.cmd_set_renew.addr, addr, NICO_MAX_ADDR);
		hRet = NCAPI_Client_Send(msg);
		if( FALSE == msg->cmd_protocol.cmd_netconf.cmd.cmd_set_renew.is_set_renew )
		{
			hRet = ERR_FAIL;
		}
		HLIB_STD_MemFree(msg);
	};
	EXIT;

	return hRet;
}

