/** **********************************************************************************************************
	File 		: nima_server.c
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
#include "nima_scenario.h"


static eNimaNetworkStatus s_etNetworkStatus = NIMA_NETWORK_DISCONNECTED;

// priority 가 level 에 의해 netconf 정보 output
static NIMAProtocol_Netconf_t *__nima_scenario_get_netinfo_by_priority(NIMAProtocol_devinfo_t *devinfo, HINT32 priority_level)
{
	HINT32 i = 0, j = 0, level = 0;

	if( devinfo )
	{
		for( i=0; i<devinfo->dev_count; i++ )
		{
			level = 0;
			for( j=0; j<devinfo->dev_count; j++ )
			{
				if( devinfo->netconf[j].priority < devinfo->netconf[i].priority )
				{
					level++;
				}
			}

			if( priority_level == level )
			{
				return &devinfo->netconf[i];
			}
		}
	}

	return NULL;
}

static void __nima_scenario_set_netinfo(NIMAProtocol_Netconf_t *netinfo)
{
	NCAPI_NetConfInfo_t *info = NULL;

	if( NULL != netinfo )
	{
		info = HLIB_STD_MemAlloc(sizeof(*info));
		if( info )
		{
			HxSTD_MemSet(info, 0x00, sizeof(*info));

			info->id = netinfo->id;
			info->priority = netinfo->priority;
			HxSTD_MemCopy(info->name, netinfo->name, NIMA_PROTO_MAX_INTERFACE_NAME);
			info->type = netinfo->type;
			info->linked = netinfo->linked;
			info->active = netinfo->active;
			info->dhcp = netinfo->dhcp;
			info->dns_auto = netinfo->dns_auto;
			info->auto_ip = netinfo->auto_ip;
			info->ip_ver = netinfo->ip_ver;
			switch(info->ip_ver)
			{
				case eNCAPI_NETCONF_IPV4:
					HxSTD_MemCopy(info->ip_info.ipv4.macaddr, netinfo->ip_info.ipv4.macaddr, NIMA_PROTO_MAX_MACADDR);
					HxSTD_MemCopy(info->ip_info.ipv4.ipAddr, netinfo->ip_info.ipv4.ipAddr, NIMA_PROTO_MAX_IP4ADDR);
					HxSTD_MemCopy(info->ip_info.ipv4.netmask, netinfo->ip_info.ipv4.netmask, NIMA_PROTO_MAX_IP4ADDR);
					HxSTD_MemCopy(info->ip_info.ipv4.gateway, netinfo->ip_info.ipv4.gateway, NIMA_PROTO_MAX_IP4ADDR);
					HxSTD_MemCopy(info->ip_info.ipv4.dns[0], netinfo->ip_info.ipv4.dns1, NIMA_PROTO_MAX_IP4ADDR);
					HxSTD_MemCopy(info->ip_info.ipv4.dns[1], netinfo->ip_info.ipv4.dns2, NIMA_PROTO_MAX_IP4ADDR);
					break;
				case eNCAPI_NETCONF_IPV6:
					HxSTD_MemCopy(info->ip_info.ipv6.macaddr, netinfo->ip_info.ipv6.macaddr, NIMA_PROTO_MAX_MACADDR);
					HxSTD_MemCopy(info->ip_info.ipv6.ipAddr, netinfo->ip_info.ipv6.ipAddr, NIMA_PROTO_MAX_IP6ADDR);
					HxSTD_MemCopy(info->ip_info.ipv6.gateway, netinfo->ip_info.ipv6.gateway, NIMA_PROTO_MAX_IP6ADDR);
					HxSTD_MemCopy(info->ip_info.ipv6.dns[0], netinfo->ip_info.ipv6.dns1, NIMA_PROTO_MAX_IP6ADDR);
					HxSTD_MemCopy(info->ip_info.ipv6.dns[1], netinfo->ip_info.ipv6.dns2, NIMA_PROTO_MAX_IP6ADDR);
					break;
			}

			nima_scenario_set_net_status(NIMA_NETWORK_CONNECTING);


			NCAPI_NetConf_SetInfo(info->id, info);

			if( NULL != info )
			{
				HLIB_STD_MemFree(info);
			}
		}
	}
}

static void __nima_scenario_set_apinfo(HINT32 dev_id, NIMAProtocol_APInfo_t *apinfo)
{
	NCAPI_WifiAPInfo_t *info = NULL;

	if( NULL != apinfo )
	{
		info = HLIB_STD_MemAlloc(sizeof(*info));
		if (info == NULL)
		{
			HxLOG_Critical("HLIB_STD_MemAlloc() failed\n");
			return;
		}
		
		nima_conv_ncapi_apinfo(apinfo, info);
		if(apinfo->essid && '\0' != apinfo->essid[0])
		{
			nima_scenario_set_net_status(NIMA_NETWORK_CONNECTING);
			NCAPI_Wifi_Connect(dev_id, info);
		}

		if( NULL != info )
		{
			HLIB_STD_MemFree(info);
		}
	}

	return;
}

static void __nima_scenario_set_bt_enable(HBOOL enable)
{
	FuncEnter;
	NCAPI_BT_SetEnable(enable);
	FuncLeave;
}

static void __nima_scenario_set_netconf_disconnect(HINT32 dev_id)
{
	FuncEnter;
	NCAPI_NetConf_StopConnect(dev_id);
	FuncLeave;
}

static void __nima_scenario_set_wifi_disconnect(HINT32 dev_id)
{
	FuncEnter;
	NCAPI_Wifi_Disconnect(dev_id);
	NCAPI_NetConf_DevDown(dev_id);
	FuncLeave;
}

HERROR nima_scenario_apply(void *info)
{
	HINT32 i = 0;
	HERROR eRet = ERR_OK;
	NIMAProtocol_Netconf_t *netinfo = NULL;
	NIMAProtocol_devinfo_t *devinfo = (NIMAProtocol_devinfo_t *)info;

	if( NULL != devinfo )
	{
		for( i=0; i<devinfo->dev_count; i++ )
		{
			netinfo = __nima_scenario_get_netinfo_by_priority(devinfo, i);
			if( NULL != netinfo )
			{
				if( (TRUE == netinfo->linked) && (TRUE == netinfo->active) )
				{
					// top priority network device is connected !!
					nima_scenario_set_net_status(NIMA_NETWORK_CONNECTED);
					return ERR_OK;
				}

				if( eNIMA_NETCONF_PPPOE == netinfo->type )
				{
					eRet = nima_scenario_apply_pppoe(devinfo);
					if( ERR_OK == eRet )
					{
						return ERR_OK;
					}
					else
					{
						 continue;
					}
				}

				if( (TRUE == netinfo->linked) && (FALSE == netinfo->active) )
				{
					if( eNIMA_NETCONF_WIFI == netinfo->type )
					{
						if(NIMA_NETWORK_CONNECTING != nima_scenario_get_net_status())
						{
							__nima_scenario_set_apinfo(netinfo->id, &devinfo->apinfo);
						}
						return ERR_OK;
					}
					if( eNIMA_NETCONF_LAN == netinfo->type )
					{
						__nima_scenario_set_netinfo(netinfo);
						return ERR_OK;
					}
				}
			}
		}
	}

	return ERR_OK;
}

HERROR nima_scenario_apply_by_devid(HINT32 dev_id, void *info)
{
	HINT32 i = 0;
	NIMAProtocol_Netconf_t *netinfo = NULL;
	NIMAProtocol_devinfo_t *devinfo = (NIMAProtocol_devinfo_t *)info;

	if( NULL != devinfo )
	{
		for( i=0; i<devinfo->dev_count; i++ )
		{
			if( devinfo->netconf[i].id == dev_id )
			{
				netinfo = &devinfo->netconf[i];
				if( (TRUE == netinfo->linked) && (FALSE == netinfo->active) )
				{
					__nima_scenario_set_netinfo(netinfo);
					return ERR_OK;
				}
			}
		}
	}

	return ERR_FAIL;
}

HERROR nima_scenario_apply_bluetooth(void *info)
{
	NIMAProtocol_devinfo_t *devinfo = (NIMAProtocol_devinfo_t *)info;

	if( NULL != devinfo )
	{
		__nima_scenario_set_bt_enable(devinfo->btinfo.enable);

	}

	return ERR_OK;
}

HERROR nima_scenario_apply_pppoe(void *info)
{
	HINT32 i = 0;
	NCAPI_PPPoEInfo_t pppoe_info;
	NIMAProtocol_devinfo_t *devinfo = (NIMAProtocol_devinfo_t *)info;

	FuncEnter;

	for(i = 0; i < devinfo->dev_count; i++)
	{
		if( devinfo->netconf[i].type == eNIMA_NETCONF_PPPOE )
		{
			if( devinfo->netconf[i].linked == FALSE || devinfo->netconf[i].active == FALSE )
			{
				nima_conv_ncapi_pppoe(&devinfo->pppoeinfo, &pppoe_info);

				if( pppoe_info.connect_id[0] != '\0' && pppoe_info.connect_pw[0] != '\0' )
				{
					if( pppoe_info.enable == TRUE )
					{
						NCAPI_PPPoE_Connect(i, &pppoe_info);
						return ERR_OK;
					}
				}
			}
		}
	}

	FuncLeave;

	return ERR_FAIL;
}

HERROR nima_scenario_shutdown(void *info)
{
	HINT32 i = 0;
	NIMAProtocol_devinfo_t *devinfo = (NIMAProtocol_devinfo_t *)info;

	if( NULL != devinfo )
	{
		if( devinfo->btinfo.enable )
		{
			__nima_scenario_set_bt_enable(FALSE);
		}

		for( i=0; i<devinfo->dev_count; i++ )
		{
			if( devinfo->netconf[i].active && devinfo->netconf[i].linked )
			{
				__nima_scenario_set_netconf_disconnect(devinfo->netconf[i].id);

				if( eNIMA_NETCONF_WIFI == devinfo->netconf[i].type )
				{
					__nima_scenario_set_wifi_disconnect(devinfo->netconf[i].id);
				}
			}
		}
	}

	return ERR_OK;
}

HERROR nima_scenario_operation(void *info)
{
	HINT32 i = 0;
	NIMAProtocol_devinfo_t *devinfo = (NIMAProtocol_devinfo_t *)info;

	if( NULL != devinfo )
	{
		if( devinfo->btinfo.enable )
		{
			__nima_scenario_set_bt_enable(TRUE);
		}

		for( i=0; i<devinfo->dev_count; i++ )
		{
			if( devinfo->netconf[i].active && devinfo->netconf[i].linked )
			{
				if( eNIMA_NETCONF_LAN == devinfo->netconf[i].type )
				{
					__nima_scenario_set_netinfo(&devinfo->netconf[i]);
				}
				else if( eNIMA_NETCONF_WIFI == devinfo->netconf[i].type )
				{
					__nima_scenario_set_apinfo(devinfo->netconf[i].id, &devinfo->apinfo);
				}
			}
		}
	}

	return ERR_OK;
}

/**
* This function is used to get the network status
*/

eNimaNetworkStatus nima_scenario_get_net_status()
{
	return s_etNetworkStatus;
}


/**
* This function is used to set the network status
*/

void nima_scenario_set_net_status(eNimaNetworkStatus status)
{
	s_etNetworkStatus = status;
}
