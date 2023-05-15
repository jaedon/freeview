/** **********************************************************************************************************
	File 		: napi_parser.c
	author 		:
	comment		:
	date    	: 2013/07/02
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
#include <hlib.h>
#include "napi_parser.h"
#include "napi_def.h"
#include "napi_protocol.h"
#include "../napi_debug.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definitions
 *----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
void napi_parser_macNtoA(const HUINT8 *pMacAddress, HUINT8 *pszMacAddrStr)
{
	HINT32		i = 0;
	HUINT8		mac_temp[8] = {0, };

	for(i = 0; i < 6; i++)
	{
		HxSTD_MemSet(mac_temp, 0, 8);
		HxSTD_snprintf(mac_temp, sizeof(mac_temp), "%02x", pMacAddress[i]);
		HxSTD_StrNCat(pszMacAddrStr, mac_temp, 2);
		if(i < 5)
			HxSTD_StrNCat(pszMacAddrStr, ":", 1);
	}
}

void napi_parser_macAtoN(const HUINT8 *pszMacAddrStr, HUINT8 *pMacAddress)
{
	HUINT32	octet[6] = { 0 };

	sscanf ( pszMacAddrStr, "%02x:%02x:%02x:%02x:%02x:%02x", &octet[0], &octet[1], &octet[2], &octet[3], &octet[4], &octet[5] );

	pMacAddress[0] = octet[0];
	pMacAddress[1] = octet[1];
	pMacAddress[2] = octet[2];
	pMacAddress[3] = octet[3];
	pMacAddress[4] = octet[4];
	pMacAddress[5] = octet[5];
}

void napi_parser_addrNtoA(HUINT8 *ulAddr, HCHAR *pAddr, HINT32 bufsize)
{
	HxSTD_snprintf( pAddr, bufsize, "%d.%d.%d.%d",
				ulAddr[0], ulAddr[1], ulAddr[2], ulAddr[3] );
}

void napi_parser_addrAtoN(const HCHAR *pAddr, HUINT8 *pulAddr)
{
	HULONG	octet[4] = { 0 };

	sscanf (pAddr, "%ld.%ld.%ld.%ld", &octet[0], &octet[1], &octet[2], &octet[3]);

	pulAddr[0] = octet[0];
	pulAddr[1] = octet[1];
	pulAddr[2] = octet[2];
	pulAddr[3] = octet[3];
	
	/* Use when pulAddr is HULONG, something else don't use.
	*pulAddr = 0;
	*pulAddr |= octet[0];
	*pulAddr |= ( octet[1] << 8 );
	*pulAddr |= ( octet[2] << 16 );
	*pulAddr |= ( octet[3] << 24 );
	*/
}

HINT32 napi_parser_devtype_NapitoProtocol(HINT32 napi_dev_type)
{
	HINT32 protocol_dev_type = -1;

	switch( napi_dev_type )
	{
		case eNAPI_NETCONF_LAN:
			protocol_dev_type = eNAPI_PROTO_NETCONF_LAN; break;
		case eNAPI_NETCONF_WIFI:
			protocol_dev_type = eNAPI_PROTO_NETCONF_WIFI; break;
		case eNAPI_NETCONF_CM:
			protocol_dev_type = eNAPI_PROTO_NETCONF_CM; break;
		case eNAPI_NETCONF_BLUETOOTH:
			protocol_dev_type = eNAPI_PROTO_NETCONF_BLUETOOTH; break;
		case eNAPI_NETCONF_RF4CE:
			protocol_dev_type = eNAPI_PROTO_NETCONF_RF4CE; break;
		case eNAPI_NETCONF_PPPOE:
			protocol_dev_type = eNAPI_PROTO_NETCONF_PPPOE; break;			
		default: 
			protocol_dev_type = -1; break;
	}

	return protocol_dev_type;
}

HINT32 napi_parser_devtype_ProtocoltoNapi(HINT32 protocol_dev_type)
{
	HINT32 napi_dev_type = -1;

	switch( protocol_dev_type )
	{
		case eNAPI_PROTO_NETCONF_LAN:
			napi_dev_type = eNAPI_NETCONF_LAN; break;
		case eNAPI_PROTO_NETCONF_WIFI:
			napi_dev_type = eNAPI_NETCONF_WIFI; break;
		case eNAPI_PROTO_NETCONF_CM:
			napi_dev_type = eNAPI_NETCONF_CM; break;
		case eNAPI_PROTO_NETCONF_BLUETOOTH:
			napi_dev_type = eNAPI_NETCONF_BLUETOOTH; break;
		case eNAPI_PROTO_NETCONF_RF4CE:
			napi_dev_type = eNAPI_NETCONF_RF4CE; break;
		case eNAPI_PROTO_NETCONF_PPPOE:
			napi_dev_type = eNAPI_NETCONF_PPPOE; break;
		case eNAPI_PROTO_NETCONF_WAN:
		case eNAPI_PROTO_NETCONF_MAX:
		default: 
			napi_dev_type = -1; break;
	}

	return napi_dev_type;
}


void napi_parser_apInfo_PtoN(const void *src, void *dst)
{
	NAPIWifiInfo_t *wifi_info = NULL;
	NAPIProtocol_Wifi_t *info = NULL;
	HINT32 i = 0;

	if( !src || !dst )
		return;
	
	info = (NAPIProtocol_Wifi_t *)src;
	wifi_info = (NAPIWifiInfo_t *)dst;
	
	wifi_info->dev_id = info->dev_id;
	wifi_info->count = info->count;

	for( i=0; i<wifi_info->count; i++ )
	{
		HxSTD_MemCopy(wifi_info->ap[i].essid, info->ap[i].essid, NAPI_PROTO_AP_ESSID);
		HxSTD_MemCopy(wifi_info->ap[i].apkey, info->ap[i].apkey, NAPI_AP_KEY);
		napi_parser_macNtoA(info->ap[i].macaddr, wifi_info->ap[i].macaddr);
		wifi_info->ap[i].wave_strength = info->ap[i].wave_strength;
		wifi_info->ap[i].connect_speed = info->ap[i].connect_speed;
		wifi_info->ap[i].op_mode = info->ap[i].op_mode;
		wifi_info->ap[i].secure_type = info->ap[i].secure_type;
		wifi_info->ap[i].authen_type = info->ap[i].authen_type;
		wifi_info->ap[i].encrypt_type = info->ap[i].encrypt_type;
		wifi_info->ap[i].bIsConnected = info->ap[i].is_connected;
	}
	
	return;
}

void napi_parser_apInfo_NtoP(const void *src, void *dst)
{
	NAPIProtocol_Wifi_t *wifi_info = NULL;
	NAPIWifiInfo_t *info = NULL;
	HINT32 i = 0;

	if( !src || !dst )
		return;
	
	info = (NAPIWifiInfo_t *)src;
	wifi_info = (NAPIProtocol_Wifi_t *)dst;
	
	wifi_info->dev_id = info->dev_id;
	wifi_info->count = info->count;

	for( i=0; i<wifi_info->count; i++ )
	{
		HxSTD_MemCopy(wifi_info->ap[i].essid, info->ap[i].essid, NAPI_PROTO_AP_ESSID);
		HxSTD_MemCopy(wifi_info->ap[i].apkey, info->ap[i].apkey, NAPI_AP_KEY);
		napi_parser_macAtoN(info->ap[i].macaddr, wifi_info->ap[i].macaddr);
		wifi_info->ap[i].wave_strength = info->ap[i].wave_strength;
		wifi_info->ap[i].connect_speed = info->ap[i].connect_speed;
		wifi_info->ap[i].op_mode = info->ap[i].op_mode;
		wifi_info->ap[i].secure_type = info->ap[i].secure_type;
		wifi_info->ap[i].authen_type = info->ap[i].authen_type;
		wifi_info->ap[i].encrypt_type = info->ap[i].encrypt_type;
		wifi_info->ap[i].is_connected = info->ap[i].bIsConnected;
	}
	
	return;
}

void napi_parser_netInfo_PtoN(const void *src, void *dst)
{
	NAPIProtocol_Netconf_t *info = NULL;
	NAPINetConfInfo_t *netconf_info = NULL;

	if( !src || !dst )
		return;

	netconf_info = (NAPINetConfInfo_t *)dst;
	info = (NAPIProtocol_Netconf_t *)src;
	
	netconf_info->id = info->id;
	HxSTD_MemCopy(netconf_info->name, info->name, NAPI_MAX_INTERFACE_NAME);
	netconf_info->type = napi_parser_devtype_ProtocoltoNapi(info->type);
	netconf_info->linked = info->linked;
	netconf_info->active = info->active;
	netconf_info->dhcp = info->dhcp;
	netconf_info->dns_auto = info->dns_auto;
	netconf_info->auto_ip = info->auto_ip;
	netconf_info->ip_ver = info->ip_ver;
	if( netconf_info->ip_ver == eNAPI_NETCONF_IPV4 )
	{
		napi_parser_macNtoA(info->ip_info.ipv4.macaddr, netconf_info->ip_info.ipv4.macaddr);
		napi_parser_addrNtoA(info->ip_info.ipv4.ipAddr, netconf_info->ip_info.ipv4.ipAddr, NAPI_MAX_IP4ADDR);
		napi_parser_addrNtoA(info->ip_info.ipv4.netmask, netconf_info->ip_info.ipv4.netmask, NAPI_MAX_IP4ADDR);
		napi_parser_addrNtoA(info->ip_info.ipv4.gateway, netconf_info->ip_info.ipv4.gateway, NAPI_MAX_IP4ADDR);
		napi_parser_addrNtoA(info->ip_info.ipv4.dns1, netconf_info->ip_info.ipv4.dns1, NAPI_MAX_IP4ADDR);
		napi_parser_addrNtoA(info->ip_info.ipv4.dns2, netconf_info->ip_info.ipv4.dns2, NAPI_MAX_IP4ADDR);
	}
	else if(netconf_info->ip_ver == eNAPI_NETCONF_IPV6 )
	{
		napi_parser_macNtoA(info->ip_info.ipv6.macaddr, netconf_info->ip_info.ipv6.macaddr);
		napi_parser_addrNtoA(info->ip_info.ipv6.ipAddr, netconf_info->ip_info.ipv6.ipAddr, NAPI_MAX_IP6ADDR);
		napi_parser_addrNtoA(info->ip_info.ipv6.gateway, netconf_info->ip_info.ipv6.gateway, NAPI_MAX_IP6ADDR);
		napi_parser_addrNtoA(info->ip_info.ipv6.dns1, netconf_info->ip_info.ipv6.dns1, NAPI_MAX_IP6ADDR);
		napi_parser_addrNtoA(info->ip_info.ipv6.dns2, netconf_info->ip_info.ipv6.dns2, NAPI_MAX_IP6ADDR);
	}

	return;
}

void napi_parser_netInfo_NtoP(const void *src, void *dst)
{
	NAPIProtocol_Netconf_t *netconf = NULL;
	NAPINetConfInfo_t *info = NULL;

	if( !src || !dst )
		return;

	netconf = (NAPIProtocol_Netconf_t *)dst;
	info = (NAPINetConfInfo_t *)src;

	netconf->id = info->id;
	HxSTD_MemCopy(netconf->name, info->name, NAPI_PROTO_MAX_INTERFACE_NAME);
	netconf->type = napi_parser_devtype_NapitoProtocol(info->type);
	netconf->linked = info->linked;
	netconf->active = info->active;
	netconf->dhcp = info->dhcp;
	netconf->dns_auto = info->dns_auto;
	netconf->auto_ip = info->auto_ip;
	netconf->ip_ver = info->ip_ver;
	if( info->ip_ver == eNAPI_NETCONF_IPV4 )
	{
		napi_parser_macAtoN(info->ip_info.ipv4.macaddr, netconf->ip_info.ipv4.macaddr);
		napi_parser_addrAtoN(info->ip_info.ipv4.ipAddr, netconf->ip_info.ipv4.ipAddr);
		napi_parser_addrAtoN(info->ip_info.ipv4.netmask, netconf->ip_info.ipv4.netmask);
		napi_parser_addrAtoN(info->ip_info.ipv4.gateway, netconf->ip_info.ipv4.gateway);
		napi_parser_addrAtoN(info->ip_info.ipv4.dns1, netconf->ip_info.ipv4.dns1);
		napi_parser_addrAtoN(info->ip_info.ipv4.dns2, netconf->ip_info.ipv4.dns2);
	}
	else if( info->ip_ver == eNAPI_NETCONF_IPV6 )
	{
		napi_parser_macAtoN(info->ip_info.ipv6.macaddr, netconf->ip_info.ipv6.macaddr);
		napi_parser_addrAtoN(info->ip_info.ipv6.ipAddr, netconf->ip_info.ipv6.ipAddr);
		napi_parser_addrAtoN(info->ip_info.ipv6.gateway, netconf->ip_info.ipv6.gateway);
		napi_parser_addrAtoN(info->ip_info.ipv6.dns1, netconf->ip_info.ipv6.dns1);
		napi_parser_addrAtoN(info->ip_info.ipv6.dns2, netconf->ip_info.ipv6.dns2);
	}

	return;
}

