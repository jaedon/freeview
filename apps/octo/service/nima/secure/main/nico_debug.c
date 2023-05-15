/*************************************************************************************************************
	File 		: nico_debug.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2014/01/24
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

#include "nico_debug.h"

void 	nico_debug_print_eventtype(pevent_msg event)
{
	switch(event->type.evt_type)
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

		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_USB_INSERTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_USB_EXTRACTED);

		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_SCAN_FINISHED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_SCAN_OUTPUT);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_SCAN_FAIL);

		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_AP_CONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_AP_CONNECT_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_WPS_CONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_WPS_CONNECT_FAIL);

		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_EXT_SCAN_STARTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_EXT_DISCONNECTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_EXT_RECONNECTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_EXT_INVALID_KEY);
		CASE_EVT_ENUMTOSTR(EVENT_NET_CM_ONLINE);
		CASE_EVT_ENUMTOSTR(EVENT_NET_CM_OFFLINE);

		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_INSERTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_EXTRACTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_ENABLE);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_DISABLE);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_SCAN_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_SCAN_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_AUTHEN_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_AUTHEN_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_CONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_CONNECT_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_REMOVED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_REMOTE_CMD);
		CASE_EVT_ENUMTOSTR(EVENT_NET_BT_MAX);

		CASE_EVT_ENUMTOSTR(EVENT_NET_PPPOE_CONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_PPPOE_CONNECT_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_PPPOE_DISCONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_PPPOE_DISCONNECT_FAIL);

		CASE_EVT_ENUMTOSTR(EVENT_NET_STATUS_UNKNOWN);
		default:
			break;
	}
}

void nico_debug_print_ipv6info(NICO_NetConf_IpInfo_t *ipinfo)
{
	if(ipinfo)
	{
		HxLOG_Debug("[%s:%d] mac       : %02x-%02x-%02x-%02x-%02x-%02x \n",
										__FUNCTION__, __LINE__,
										ipinfo->ipv6.macaddr[0], ipinfo->ipv6.macaddr[1],
										ipinfo->ipv6.macaddr[1], ipinfo->ipv6.macaddr[3],
										ipinfo->ipv6.macaddr[4], ipinfo->ipv6.macaddr[5]);
		HxLOG_Debug("[%s:%d] ip(6)     : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x \n",
										__FUNCTION__, __LINE__,
										ipinfo->ipv6.ipAddr[0], ipinfo->ipv6.ipAddr[1],
										ipinfo->ipv6.ipAddr[2], ipinfo->ipv6.ipAddr[3],
										ipinfo->ipv6.ipAddr[4], ipinfo->ipv6.ipAddr[5],
										ipinfo->ipv6.ipAddr[6], ipinfo->ipv6.ipAddr[7],
										ipinfo->ipv6.ipAddr[8], ipinfo->ipv6.ipAddr[9],
										ipinfo->ipv6.ipAddr[10], ipinfo->ipv6.ipAddr[11],
										ipinfo->ipv6.ipAddr[12], ipinfo->ipv6.ipAddr[13],
										ipinfo->ipv6.ipAddr[14], ipinfo->ipv6.ipAddr[15]);
		HxLOG_Debug("[%s:%d] gateway     : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x \n",
										__FUNCTION__, __LINE__,
										ipinfo->ipv6.gateway[0], ipinfo->ipv6.gateway[1],
										ipinfo->ipv6.gateway[2], ipinfo->ipv6.gateway[3],
										ipinfo->ipv6.gateway[4], ipinfo->ipv6.gateway[5],
										ipinfo->ipv6.gateway[6], ipinfo->ipv6.gateway[7],
										ipinfo->ipv6.gateway[8], ipinfo->ipv6.gateway[9],
										ipinfo->ipv6.gateway[10], ipinfo->ipv6.gateway[11],
										ipinfo->ipv6.gateway[12], ipinfo->ipv6.gateway[13],
										ipinfo->ipv6.gateway[14], ipinfo->ipv6.gateway[15]);
		{
			HINT32 i = 0;
			for(i=0;i<eNICO_Dns_Max;i++)
			{
				HxLOG_Debug("[%s:%d] dns%d	 : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x \n",
										__FUNCTION__, __LINE__, i+1,
										ipinfo->ipv6.dns[i][0], ipinfo->ipv6.dns[i][1],
										ipinfo->ipv6.dns[i][2], ipinfo->ipv6.dns[i][3],
										ipinfo->ipv6.dns[i][4], ipinfo->ipv6.dns[i][5],
										ipinfo->ipv6.dns[i][6], ipinfo->ipv6.dns[i][7],
										ipinfo->ipv6.dns[i][8], ipinfo->ipv6.dns[i][9],
										ipinfo->ipv6.dns[i][10], ipinfo->ipv6.dns[i][11],
										ipinfo->ipv6.dns[i][12], ipinfo->ipv6.dns[i][13],
										ipinfo->ipv6.dns[i][14], ipinfo->ipv6.dns[i][15]);
			}
		}

		HxLOG_Debug("prefix : %x \n", ipinfo->ipv6.prefix);
		HxLOG_Debug("addr_scope : %d \n", ipinfo->ipv6.addrscope);

	}
	HxLOG_Debug("+++++++++++++++++++++++++++++++++++ \n");
}

void nico_debug_print_ipv4info(NICO_NetConf_IpInfo_t *ipinfo)
{
	if(ipinfo)
	{
		HxLOG_Debug("mac       : %02x-%02x-%02x-%02x-%02x-%02x \n",
										ipinfo->ipv4.macaddr[0], ipinfo->ipv4.macaddr[1],
										ipinfo->ipv4.macaddr[2], ipinfo->ipv4.macaddr[3],
										ipinfo->ipv4.macaddr[4], ipinfo->ipv4.macaddr[5]);
		HxLOG_Debug("ip(4)     : %3d.%3d.%3d.%3d \n",
										ipinfo->ipv4.ipAddr[0], ipinfo->ipv4.ipAddr[1],
										ipinfo->ipv4.ipAddr[2], ipinfo->ipv4.ipAddr[3]);
		HxLOG_Debug("netmask   : %3d.%3d.%3d.%3d \n",
										ipinfo->ipv4.netmask[0], ipinfo->ipv4.netmask[1],
										ipinfo->ipv4.netmask[2], ipinfo->ipv4.netmask[3]);
		HxLOG_Debug("gateway   : %3d.%3d.%3d.%3d \n",
										ipinfo->ipv4.gateway[0], ipinfo->ipv4.gateway[1],
										ipinfo->ipv4.gateway[2], ipinfo->ipv4.gateway[3]);
		{
			HINT32 i = 0;
			for(i=0;i<eNICO_Dns_Max;i++)
			{
				HxLOG_Debug("dns%d     : %3d.%3d.%3d.%3d \n",
										i+1,
										ipinfo->ipv4.dns[i][0], ipinfo->ipv4.dns[i][1],
										ipinfo->ipv4.dns[i][2], ipinfo->ipv4.dns[i][3]);
			}
		}
		HxLOG_Debug("broadcast : %3d.%3d.%3d.%3d \n",
										ipinfo->ipv4.broadcast[0], ipinfo->ipv4.broadcast[1],
										ipinfo->ipv4.broadcast[2], ipinfo->ipv4.broadcast[3]);
	}
}

void nico_debug_print_apinfo(NICO_Wifi_APInfo_t *apinfo)
{
	if(apinfo)
	{
		HxLOG_Debug("++[nico] Debug AP Info +++++++++++++++++++++++++++++++ \n");
		HxLOG_Debug("channel       : %d \n", apinfo->channel);
		HxLOG_Debug("freq          : %d \n", apinfo->freq);
		HxLOG_Debug("is_wps        : %d \n", apinfo->is_wps);
		HxLOG_Debug("op_mode       : %d \n", apinfo->op_mode);
		HxLOG_Debug("wave_strength : %d \n", apinfo->wave_strength);
		HxLOG_Debug("bitrate       : %d \n", apinfo->bitrate);
		HxLOG_Debug("secure_type   : %d \n", apinfo->secure_type);
		HxLOG_Debug("anthen_type   : %d \n", apinfo->authen_type);
		HxLOG_Debug("encrypt_type  : %d \n", apinfo->encrypt_type);
		HxLOG_Debug("keyindex      : %d \n", apinfo->keyindex);
		HxLOG_Debug("mac(BSSID)    : %02x-%02x-%02x-%02x-%02x-%02x \n",
						apinfo->macaddr[0], apinfo->macaddr[1], apinfo->macaddr[2],
						apinfo->macaddr[3], apinfo->macaddr[4], apinfo->macaddr[5]);
		HxLOG_Debug("essid         : %s \n", apinfo->essid);
		HxLOG_Debug("id            : %s \n", apinfo->id);
		HxLOG_Debug("apkey         : %s \n", apinfo->apkey);
		HxLOG_Debug("----------------------------------------------- \n");
	}
}

void nico_debug_print_bt_info(NICO_BT_DevInfo_t *info, HINT32 count)
{
	HINT32 i = 0;

	for( i=0; i<count; i++ )
	{
		if(info)
		{
			HxLOG_Debug("++ Debug BT Info +++++++++++++++++++++++++++++++ \n");
			HxLOG_Debug("name          				 : %s \n", info[i].name);
			HxLOG_Debug("mac_addr      				 : %02x-%02x-%02x-%02x-%02x-%02x \n",
											info[i].bdaddr[0], info[i].bdaddr[1], info[i].bdaddr[2],
											info[i].bdaddr[3], info[i].bdaddr[4], info[i].bdaddr[5]);
			HxLOG_Debug("class (Service/Major/Minor) : %02x / %02x / %02x \n",
											info[i].dev_class[0], info[i].dev_class[1], info[i].dev_class[2]);
			HxLOG_Debug("status        				 : %s%s%s \n",
											(info[i].status == eNICO_BT_DEV_NOT_PAIRED) ? "Not_Paired":"",
											(info[i].status == eNICO_BT_DEV_CONNECTED) ? "Connected":"",
											(info[i].status == eNICO_BT_DEV_DISCONNECTED) ? "Not Connected":"" );
			HxLOG_Debug("avaliable profiles			 : %s%s%s \n",
											(info[i].available_profile & eNICO_BT_Profile_A2DP) ? "A2DP" : "",
											(info[i].available_profile & eNICO_BT_Profile_AVRCP) ? "AVRCP" : "",
											(info[i].available_profile & eNICO_BT_Profile_HID) ? "HID" : "" );
			HxLOG_Debug("----------------------------------------------- \n");
		}
	}
}

void nico_debug_print_pppoe_info(NICO_PPPoE_Info_t *info)
{
	if(info)
	{
		HxLOG_Debug("++ Debug PPPoE Info +++++++++++++++++++++++++++++++ \n");
		HxLOG_Debug("id       	   : %d \n", info->id);
		HxLOG_Debug("enable        : %d \n", info->enable);
		HxLOG_Debug("connect_id    : %s \n", info->connect_id);
		HxLOG_Debug("connect_pw    : %s \n", info->connect_pw);
		HxLOG_Debug("----------------------------------------------- \n");
	}
}

void nico_debug_print_dinetinfo(HINT32 id, DI_NETWORK_GET_INFO_t *di_netinfo)
{
	HxLOG_Debug("################### DI EVENT ##################\n", id);
	HxLOG_Debug("#  device id : %d \n", id);
	HxLOG_Debug("        name : %s \n", di_netinfo->szName);
	HxLOG_Debug("        type : %d \n", di_netinfo->eDevType);
	HxLOG_Debug("        mac  : %02x-%02x-%02x-%02x-%02x-%02x \n",
										di_netinfo->aucMacAddr[0], di_netinfo->aucMacAddr[1], di_netinfo->aucMacAddr[2],
										di_netinfo->aucMacAddr[3], di_netinfo->aucMacAddr[4], di_netinfo->aucMacAddr[5]);
	HxLOG_Debug("       is_up : %d \n", di_netinfo->bIsUp);
	HxLOG_Debug("is_connected : %d \n", di_netinfo->bIsConnected);
	HxLOG_Debug("  is_dynamic : %d \n", di_netinfo->bIsDynamic);
	HxLOG_Debug("          ip : %3d.%3d.%3d.%3d \n",
										di_netinfo->stIpv4Addr.aucIpAddr[0], di_netinfo->stIpv4Addr.aucIpAddr[1],
										di_netinfo->stIpv4Addr.aucIpAddr[2], di_netinfo->stIpv4Addr.aucIpAddr[3]);
	HxLOG_Debug("broadcast ip : %3d.%3d.%3d.%3d \n",
										di_netinfo->stIpv4Addr.aucBcastAddr[0], di_netinfo->stIpv4Addr.aucBcastAddr[1],
										di_netinfo->stIpv4Addr.aucBcastAddr[2], di_netinfo->stIpv4Addr.aucBcastAddr[3]);
	HxLOG_Debug("     netmask : %3d.%3d.%3d.%3d \n",
										di_netinfo->stIpv4Addr.aucMaskAddr[0], di_netinfo->stIpv4Addr.aucMaskAddr[1],
										di_netinfo->stIpv4Addr.aucMaskAddr[2], di_netinfo->stIpv4Addr.aucMaskAddr[3]);
	HxLOG_Debug("###############################################\n\n", id);
}

void nico_debug_print_netinfo(NICO_NetConf_Info_t *netinfo)
{
	HxLOG_Debug("++++++++++ Print Nico Netinfo ++++++++++ \n");

	if(netinfo)
	{
		HxLOG_Debug("device id : %d \n", netinfo->id);
		HxLOG_Debug("priority  : %d \n", netinfo->priority);
		HxLOG_Debug("dev_type  : %d \n", netinfo->dev_type);
		HxLOG_Debug("linked	  : %d \n", netinfo->linked);
		HxLOG_Debug("active	  : %d \n", netinfo->active);
		HxLOG_Debug("dhcp	  : %d \n", netinfo->dhcp);
		HxLOG_Debug("dns_auto  : %d \n", netinfo->dns_auto);
		HxLOG_Debug("ip_ver    : %d \n", netinfo->ip_ver);
		HxLOG_Debug("fw_ver    : %s \n", netinfo->fw_ver);

		switch(netinfo->ip_ver)
		{
			case eNICO_VER_IPV4:
				nico_debug_print_ipv4info(&netinfo->ip_info);
				break;
			case eNICO_VER_IPV6:
				nico_debug_print_ipv6info(&netinfo->ip_info);
				break;
		}
	}
	HxLOG_Debug("+++++++++++++++++++++++++++++++++++ \n\n");
}

