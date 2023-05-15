/** **********************************************************************************************************
	File 		: nima_debug.c
	author 		: STB Component tstbcomp@humaxdigital.com
	comment		:
	date    	: 2014/01/23
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
#include "nima_debug.h"

void nima_debug_log_netconf(NCAPI_NetConfInfo_t *info)
{
	HxLOG_Debug(" (#)-----------------------------------------\n");
	HxLOG_Debug(" (#)netconf info \n");
	HxLOG_Debug(" (#)id : %d \n", info->id);
	HxLOG_Debug(" (#)priority : %d \n", info->priority);
	HxLOG_Debug(" (#)name : %s \n", info->name);
	HxLOG_Debug(" (#)type : %d \n", info->type);
	HxLOG_Debug(" (#)linked : %d \n", info->linked);
	HxLOG_Debug(" (#)active : %d \n", info->active);
	HxLOG_Debug(" (#)dhcp : %d \n", info->dhcp);
	HxLOG_Debug(" (#)dns_auto : %d \n", info->dns_auto);
	HxLOG_Debug(" (#)auto_ip : %d \n", info->auto_ip);
	HxLOG_Debug(" (#)ip_ver : %d \n", info->ip_ver);
	if( info->ip_ver == eNCAPI_NETCONF_IPV4 )
	{
		HxLOG_Debug(" (#)ip_ver : eNCAPI_NETCONF_IPV4 \n");
		HxLOG_Debug(" (#)macaddr : %02x:%02x:%02x:%02x:%02x:%02x \n",
							info->ip_info.ipv4.macaddr[0], info->ip_info.ipv4.macaddr[1], info->ip_info.ipv4.macaddr[2],
							info->ip_info.ipv4.macaddr[3], info->ip_info.ipv4.macaddr[4], info->ip_info.ipv4.macaddr[5]);
		HxLOG_Debug(" (#)ipaddr : %d.%d.%d.%d \n",
							info->ip_info.ipv4.ipAddr[0], info->ip_info.ipv4.ipAddr[1],
							info->ip_info.ipv4.ipAddr[2], info->ip_info.ipv4.ipAddr[3]);
		HxLOG_Debug(" (#)netmask : %d.%d.%d.%d\n",
							info->ip_info.ipv4.netmask[0], info->ip_info.ipv4.netmask[1],
							info->ip_info.ipv4.netmask[2], info->ip_info.ipv4.netmask[3]);
		HxLOG_Debug(" (#)gateway : %d.%d.%d.%d \n",
							info->ip_info.ipv4.gateway[0], info->ip_info.ipv4.gateway[1],
							info->ip_info.ipv4.gateway[2], info->ip_info.ipv4.gateway[3]);
		HxLOG_Debug(" (#)dns1 : %d.%d.%d.%d \n",
							info->ip_info.ipv4.dns[0][0], info->ip_info.ipv4.dns[0][1],
							info->ip_info.ipv4.dns[0][2], info->ip_info.ipv4.dns[0][3]);
		HxLOG_Debug(" (#)dns2 : %d.%d.%d.%d \n",
							info->ip_info.ipv4.dns[1][0], info->ip_info.ipv4.dns[1][1],
							info->ip_info.ipv4.dns[1][2], info->ip_info.ipv4.dns[1][3]);
	}
	else if( info->ip_ver == eNCAPI_NETCONF_IPV6 )
	{
		HxLOG_Debug(" (#)ip_ver : eNCAPI_NETCONF_IPV6 \n");
		HxLOG_Debug(" (#)macaddr : %02x:%02x:%02x:%02x:%02x:%02x \n",
							info->ip_info.ipv6.macaddr[0], info->ip_info.ipv6.macaddr[1], info->ip_info.ipv6.macaddr[2],
							info->ip_info.ipv6.macaddr[3], info->ip_info.ipv6.macaddr[4], info->ip_info.ipv6.macaddr[5]);
		HxLOG_Debug(" (#)ipaddr : %d.%d.%d.%d \n",
							info->ip_info.ipv6.ipAddr[0], info->ip_info.ipv6.ipAddr[1],
							info->ip_info.ipv6.ipAddr[2], info->ip_info.ipv6.ipAddr[3]);
		HxLOG_Debug(" (#)gateway : %d.%d.%d.%d \n",
							info->ip_info.ipv6.gateway[0], info->ip_info.ipv6.gateway[1],
							info->ip_info.ipv6.gateway[2], info->ip_info.ipv6.gateway[3]);
		HxLOG_Debug(" (#)dns1 : %d.%d.%d.%d \n",
							info->ip_info.ipv6.dns[0][0], info->ip_info.ipv6.dns[0][1],
							info->ip_info.ipv6.dns[0][2], info->ip_info.ipv6.dns[0][3]);
		HxLOG_Debug(" (#)dns2 : %d.%d.%d.%d \n",
							info->ip_info.ipv6.dns[1][0], info->ip_info.ipv6.dns[1][1],
							info->ip_info.ipv6.dns[1][2], info->ip_info.ipv6.dns[1][3]);
	}

	return;
}

void nima_debug_log_wifi(NCAPI_WifiAPInfo_t *info, HINT32 count)
{
	HINT32 i = 0;

	for( i=0; i<count; i++ )
	{
		HxLOG_Debug(" (#)-----------------------------------------\n");
		HxLOG_Debug(" (#)AP INDEX : [%d] \n", i);
		HxLOG_Debug(" (#)essid : %s \n", info[i].essid);
		HxLOG_Debug(" (#)apkey : %s \n", info[i].apkey);
		HxLOG_Debug(" (#)macaddr : %02x:%02x:%02x:%02x:%02x:%02x \n",
			info[i].macaddr[0], info[i].macaddr[1], info[i].macaddr[2],
			info[i].macaddr[3], info[i].macaddr[4], info[i].macaddr[5]);
		HxLOG_Debug(" (#)connect_speed : %d \n", info[i].connect_speed);
		HxLOG_Debug(" (#)wave_strength : %d \n", info[i].wave_strength);
		HxLOG_Debug(" (#)op_mode : %d \n", info[i].op_mode);
		HxLOG_Debug(" (#)secure_type : %d \n", info[i].secure_type);
		HxLOG_Debug(" (#)authen_type : %d \n", info[i].authen_type);
		HxLOG_Debug(" (#)encrypt_type : %d \n", info[i].encrypt_type);
		HxLOG_Debug(" (#)-----------------------------------------\n");
	}

	return;
}

void nima_debug_log_bt(NCAPI_BTDevInfo_t *info, HINT32 count)
{
	HINT32 i = 0;

	for( i=0; i<count; i++ )
	{
		if(info)
		{
			HxLOG_Debug(" (#)++ Debug BT Info +++++++++++++++++++++++++++++++ \n");
			HxLOG_Debug(" (#)name          				 : %s \n", info[i].name);
			HxLOG_Debug(" (#)[%s:%d] mac_addr      				 : %02x-%02x-%02x-%02x-%02x-%02x \n",
											__FUNCTION__, __LINE__,
											info[i].bdaddr[0], info[i].bdaddr[1], info[i].bdaddr[2],
											info[i].bdaddr[3], info[i].bdaddr[4], info[i].bdaddr[5]);
			HxLOG_Debug(" (#)[%s:%d] class (Service/Major/Minor) : %02x / %02x / %02x \n",
											__FUNCTION__, __LINE__,
											info[i].dev_class[0], info[i].dev_class[1], info[i].dev_class[2]);
			HxLOG_Debug(" (#)[%s:%d] status        				 : %s%s%s \n",
											__FUNCTION__, __LINE__,
											(info[i].status == eNCAPI_BT_DEV_NOT_PAIRED) ? "Not_Paired":"",
											(info[i].status == eNCAPI_BT_DEV_CONNECTED) ? "Connected":"",
											(info[i].status == eNCAPI_BT_DEV_DISCONNECTED) ? "Not Connected":"" );
			HxLOG_Debug(" (#)[%s:%d] avaliable profiles			 : %s%s%s \n",
											__FUNCTION__, __LINE__,
											(info[i].available_profile & eNCAPI_BT_Profile_A2DP) ? "A2DP" : "",
											(info[i].available_profile & eNCAPI_BT_Profile_AVRCP) ? "AVRCP" : "",
											(info[i].available_profile & eNCAPI_BT_Profile_HID) ? "HID" : "" );
			HxLOG_Debug(" (#)----------------------------------------------- \n");
		}
	}
}

void nima_debug_log_pppoe(NCAPI_PPPoEInfo_t *info)
{
	if(info)
	{
		HxLOG_Debug(" (#)DevID : %d \n", info->id);
		HxLOG_Debug(" (#)bOn : %d \n", info->enable);
		HxLOG_Debug(" (#)ID : %s \n", info->connect_id);
		HxLOG_Debug(" (#)Password : %s \n", info->connect_pw);
	}
}

