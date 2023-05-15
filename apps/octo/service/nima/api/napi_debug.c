/** **********************************************************************************************************
	File 		: napi_debug.c
	author 		:
	comment		: Network Configuration
	date    	: 2014/01/18
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

#include "napi_debug.h"
#include "napi_def.h"

#include <hlib.h>

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------
 *	internal functions
 *----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
void napi_debug_log_netconf(HINT32 id, void *info)
{
	NAPINetConfInfo_t *netconf = NULL;
	
	if( !info ) return;
	netconf = (NAPINetConfInfo_t *)info;
	
	HxLOG_Debug(" (#)-----------------------------------------\n");
	HxLOG_Debug(" (#)netconf info : devid : %d\n", id);
	HxLOG_Debug(" (#)id : %d \n", netconf->id);
	HxLOG_Debug(" (#)name : %s \n", netconf->name);
	HxLOG_Debug(" (#)type : %d \n", netconf->type);
	HxLOG_Debug(" (#)linked : %d \n", netconf->linked);
	HxLOG_Debug(" (#)active : %d \n", netconf->active);
	HxLOG_Debug(" (#)dhcp : %d \n", netconf->dhcp);
	HxLOG_Debug(" (#)dns_auto : %d \n", netconf->dns_auto);
	HxLOG_Debug(" (#)auto_ip : %d \n", netconf->auto_ip);
	HxLOG_Debug(" (#)ip_ver : %d \n", netconf->ip_ver);
	if( netconf->ip_ver == eNAPI_NETCONF_IPV4 )
	{
		HxLOG_Debug(" (#)ip_ver : eNAPI_NETCONF_IPV4 \n");
		HxLOG_Debug(" (#)macaddr : %s \n", netconf->ip_info.ipv4.macaddr);
		HxLOG_Debug(" (#)ipaddr : %s \n", netconf->ip_info.ipv4.ipAddr);
		HxLOG_Debug(" (#)netmask : %s \n", netconf->ip_info.ipv4.netmask);
		HxLOG_Debug(" (#)gateway : %s \n", netconf->ip_info.ipv4.gateway);
		HxLOG_Debug(" (#)dns1 : %s \n", netconf->ip_info.ipv4.dns1);
		HxLOG_Debug(" (#)dns2 : %s \n", netconf->ip_info.ipv4.dns2);
	}
	else if( netconf->ip_ver == eNAPI_NETCONF_IPV6 )
	{
		HxLOG_Debug(" (#)ip_ver : eNAPI_NETCONF_IPV6 \n");
		HxLOG_Debug(" (#)macaddr : %s \n", netconf->ip_info.ipv6.macaddr);
		HxLOG_Debug(" (#)ipaddr : %s \n", netconf->ip_info.ipv6.ipAddr);
		HxLOG_Debug(" (#)gateway : %s \n", netconf->ip_info.ipv6.gateway);
		HxLOG_Debug(" (#)dns1 : %s \n", netconf->ip_info.ipv6.dns1);
		HxLOG_Debug(" (#)dns2 : %s \n", netconf->ip_info.ipv6.dns2);
	}
	HxLOG_Debug(" (#)-----------------------------------------\n");

	return;
}

void napi_debug_log_ap(HINT32 id, HINT32 count, void *info)
{
	NAPIWifiAPInfo_t *ap = NULL;
	HINT32 i = 0;

	if( !info ) return;
	ap = (NAPIWifiAPInfo_t *)info;

	for( i=0; i<count; i++ )
	{
		HxLOG_Debug(" (#)-----------------------------------------\n");
		HxLOG_Debug(" (#)AP INDEX : [%d], Device ID : %d \n", i, id);
		HxLOG_Debug(" (#)essid : %s \n", ap[i].essid);
		HxLOG_Debug(" (#)apkey : %s \n", ap[i].apkey);
		HxLOG_Debug(" (#)macaddr : %s\n", ap[i].macaddr);
		HxLOG_Debug(" (#)wave_strength : %d \n", ap[i].wave_strength);
		HxLOG_Debug(" (#)connect_speed : %d \n", ap[i].connect_speed);
		HxLOG_Debug(" (#)op_mode : %d \n", ap[i].op_mode);
		HxLOG_Debug(" (#)secure_type : %d \n", ap[i].secure_type);
		HxLOG_Debug(" (#)authen_type : %d \n", ap[i].authen_type);
		HxLOG_Debug(" (#)encrypt_type : %d \n", ap[i].encrypt_type);
		HxLOG_Debug(" (#)bIsConnected : %d \n", ap[i].bIsConnected);
		HxLOG_Debug(" (#)-----------------------------------------\n");
	}
	
	return;
}


