/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>
#include <napi.h>

#include <rema_int.h>
#include "rema_cwmp.h"
#include "RDM_Network.h"


HERROR		RDM_NETWORK_CONF_AddrN2A( HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize )
{
	snprintf( pszAddr, nbufSize, "%ld.%ld.%ld.%ld",
			(ulAddr & 0x000000FF), (ulAddr & 0x0000FF00)>>8, (ulAddr &0x00FF0000)>>16, (ulAddr & 0xFF000000)>>24 );

	return ERR_OK;
}

HERROR		RDM_NETWORK_CONF_AddrA2N( HCHAR *pszAddr, HULONG *pulAddr )
{
	HULONG	octet[4] = { 0 };

	sscanf ( pszAddr, "%ld.%ld.%ld.%ld", &octet[0], &octet[1], &octet[2], &octet[3] );

	*pulAddr = 0;
	*pulAddr |=	octet[0];
	*pulAddr |= ( octet[1] << 8 );
	*pulAddr |= ( octet[2] << 16 );
	*pulAddr |= ( octet[3] << 24 );

	return ERR_OK;
}

HERROR		RDM_NETWORK_CONF_IsSupportNetwork(HBOOL *bIsSupport )
{
	HERROR hRet = ERR_OK;
	
	hRet = NAPI_NetConf_IsNetworkAvailable(bIsSupport);
	
	return hRet;
}


HERROR		RDM_NETWORK_CONF_GetDefaultDev(HINT32 *pnIndex )
{
	*pnIndex = NAPI_NetConf_GetDefaultIndex();
	HxLOG_Debug("(#)[%s:%d] netconf : default device : %d \n",__FUNCTION__,__LINE__, *pnIndex);
	
	return ERR_OK;
}

HERROR		RDM_NETWORK_CONF_SetAddrInfo( HINT32 nIndex, RDM_NETIFADDR_t* pstNetAddr  )
{
	NAPINetConfInfo_t network_info;
	
	if( !pstNetAddr )
		return ERR_FAIL;

	HxSTD_MemSet(&network_info, 0x00, sizeof(NAPINetConfInfo_t));
	network_info.id = nIndex;
	network_info.dhcp = pstNetAddr->bDhcpOn;
	network_info.dns_auto = pstNetAddr->bDnsAuto;
	network_info.auto_ip = TRUE;
	network_info.ip_ver = pstNetAddr->bIpv6;
	if( network_info.ip_ver == eNAPI_NETCONF_IPV4 )
	{
		RDM_NETWORK_CONF_AddrN2A(pstNetAddr->ulIpAddr, network_info.ip_info.ipv4.ipAddr, RDM_NETWORK_MAX_IP4ADDR);
		RDM_NETWORK_CONF_AddrN2A(pstNetAddr->ulNetmask, network_info.ip_info.ipv4.netmask, RDM_NETWORK_MAX_IP4ADDR);
		RDM_NETWORK_CONF_AddrN2A(pstNetAddr->ulGateway, network_info.ip_info.ipv4.gateway, RDM_NETWORK_MAX_IP4ADDR);
		RDM_NETWORK_CONF_AddrN2A(pstNetAddr->ulDns1, network_info.ip_info.ipv4.dns1, RDM_NETWORK_MAX_IP4ADDR);
		RDM_NETWORK_CONF_AddrN2A(pstNetAddr->ulDns2, network_info.ip_info.ipv4.dns2, RDM_NETWORK_MAX_IP4ADDR);
	}
	else if( network_info.ip_ver == eNAPI_NETCONF_IPV6 )
	{
		HxSTD_MemCopy(network_info.ip_info.ipv6.ipAddr, pstNetAddr->szIp6_addr, RDM_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(network_info.ip_info.ipv6.gateway, pstNetAddr->szIp6_gateway, RDM_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(network_info.ip_info.ipv6.dns1, pstNetAddr->szIp6_dns_addr_1, RDM_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(network_info.ip_info.ipv6.dns2, pstNetAddr->szIp6_dns_addr_2, RDM_NETWORK_MAX_IP6ADDR);
	}

	if( NAPI_NetConf_SetInfo(nIndex, &network_info) != ERR_OK ) return ERR_FAIL;

	return ERR_OK;
}
