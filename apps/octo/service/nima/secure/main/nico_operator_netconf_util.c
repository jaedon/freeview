/*************************************************************************************************************
	File 		: nico_operator_netconf_util.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/06/04
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

#include "nico_operator_netconf_util.h"
#include "hlib.h"

#include "di_err.h"
#include "di_network.h"

#define NICO_DHCPS_DEFAULT_LEASE_TIME 864000
#define NICO_DHCPS_DEFAULT_DOMAIN	"local"
#define NICO_DHCPS_DEFAULT_ROUTER	"192.168.11.2"
#define NICO_DHCPS_DEFAULT_NETMSK	"255.255.255.0"
#define NICO_DHCPS_DEFAULT_DNS		"129.219.13.81"

HUINT8* nico_netconfutil_ConverMacAddressStringIntoByte(const HCHAR *srcMacString, HUINT8* dstMacBytes)
{
	const HCHAR cSep = '-';
	HINT32 iCounter = 0;
	for (iCounter = 0; iCounter < 6; ++iCounter)
	{
		HUINT32 iNumber = 0;
		HCHAR ch;

		//Convert letter into lower case.
		ch = tolower (*srcMacString++);

		if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
		{
			return NULL;
		}

		//Convert into number.
		//       a. If character is digit then ch - '0'
		//	b. else (ch - 'a' + 10) it is done
		//	because addition of 10 takes correct value.
		iNumber = isdigit (ch) ? (ch - '0') : (ch - 'a' + 10);
		ch = tolower (*srcMacString);

		if ((iCounter < 5 && ch != cSep) ||
			(iCounter == 5 && ch != '\0' && !isspace (ch)))
		{
			++srcMacString;

			if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
			{
				return NULL;
			}

			iNumber <<= 4;
			iNumber += isdigit (ch) ? (ch - '0') : (ch - 'a' + 10);
			ch = *srcMacString;

			if (iCounter < 5 && ch != cSep)
			{
				return NULL;
			}
		}
		/* Store result.  */
		dstMacBytes[iCounter] = (HUINT8) iNumber;
		/* Skip cSep.  */
		++srcMacString;
	}
	return dstMacBytes;
}


HERROR	nico_netconfutil_dhcps_init_info( NICO_DHCPSConf_Info_t *dhcps_config )
{
	HUINT32 ulSubnet = 0, ulBrcast = 0;
	ENTRY;

	if ( 0 == dhcps_config->leasetime )
	{
		dhcps_config->leasetime = NICO_DHCPS_DEFAULT_LEASE_TIME;
	}
	if ( 0 == dhcps_config->router )
	{
		struct in_addr route = {0,};
		HxSTD_MemCopy(&route, &dhcps_config->router, sizeof(struct in_addr));
		inet_aton( NICO_DHCPS_DEFAULT_ROUTER, &route);	
	}
	if ( 0 == dhcps_config->netmask )
	{
		struct in_addr netmask = {0,};
		HxSTD_MemCopy(&netmask, &dhcps_config->netmask, sizeof(struct in_addr));		
		inet_aton( NICO_DHCPS_DEFAULT_NETMSK, &netmask);
	}
	if ( 0 == HxSTD_StrLen(dhcps_config->domain) )
	{
		HxSTD_StrNCpy( dhcps_config->domain, NICO_DHCPS_DEFAULT_DOMAIN, sizeof(dhcps_config->domain) );
	}

	{
		HUINT8 ipOctet[4] = { 0 }, brOctet[4] = { 0 };

		ulSubnet = dhcps_config->router & dhcps_config->netmask;
		ulBrcast = (ulSubnet | ~(dhcps_config->netmask) );

		if ( 0x00 == dhcps_config->startip[0] &&
				0x00 == dhcps_config->startip[1] &&
				0x00 == dhcps_config->startip[2] &&
				0x00 == dhcps_config->startip[3] )
		{
			HxSTD_MemCopy( ipOctet, &dhcps_config->router, sizeof(ipOctet) );
			HxSTD_MemCopy( brOctet, &ulBrcast, sizeof(brOctet) );

			if ( ipOctet[3] < 20 )
				ipOctet[3] = 20;
			else if ( ipOctet[3] < brOctet[3] - 2 )
				ipOctet[3] =+ 1;
			else
				return ERR_FAIL;

			HxSTD_MemCopy( &dhcps_config->startip, ipOctet, sizeof(dhcps_config->startip) );
		}

		if ( 0x00 == dhcps_config->endip[0] &&
				0x00 == dhcps_config->endip[1] &&
				0x00 == dhcps_config->endip[2] &&
				0x00 == dhcps_config->endip[3] )
		{
			HxSTD_MemCopy( brOctet, &ulBrcast, sizeof(brOctet) );
			brOctet[3] -= 1;
			HxSTD_MemCopy( &dhcps_config->endip, brOctet, sizeof(dhcps_config->endip) );
		}
	}

	{
		HINT32 i = 0;
		HINT32 nDnsCount = 0, nDns6Count = 0;
		HUINT32 ulDefDNS = 0;

		struct in_addr *dns = HLIB_STD_MemAlloc(sizeof(struct in_addr));
		HxSTD_MemCopy(dns, &ulDefDNS, sizeof(struct in_addr));		
		
		inet_aton( NICO_DHCPS_DEFAULT_DNS, dns);
		NICO_SAFE_DELETE(dns);
		
		DI_NETWORK_GetDnsCount(&nDnsCount, &nDns6Count);
		for(i=0; i<eNICO_Dns_Max; i++)
		{
			if ( 0x00 == dhcps_config->dns[i][0] &&
					0x00 == dhcps_config->dns[i][1] &&
					0x00 == dhcps_config->dns[i][2] &&
					0x00 == dhcps_config->dns[i][3] )
			{
				DI_NETWORK_GetDnsInfo( DI_NETWORK_IP_VER_4, i, (HUINT8 *)&dhcps_config->dns[i] );
			}
		}
	}
	{
		struct in_addr startip, endip, router, netmask, broadcast, dns1, dns2;
		HxSTD_MemCopy( &startip, 	&dhcps_config->startip, sizeof(struct in_addr) );
		HxSTD_MemCopy( &endip, 		&dhcps_config->endip, sizeof(struct in_addr) );
		HxSTD_MemCopy( &router, 	&dhcps_config->router, sizeof(struct in_addr) );
		HxSTD_MemCopy( &netmask, 	&dhcps_config->netmask, sizeof(struct in_addr) );
		HxSTD_MemCopy( &broadcast, 	&ulBrcast, sizeof(struct in_addr) );
		HxSTD_MemCopy( &dns1, 		&dhcps_config->dns[0], sizeof(struct in_addr) );
		HxSTD_MemCopy( &dns2, 		&dhcps_config->dns[1], sizeof(struct in_addr) );

		HxLOG_Print("[%s:%d] ============= DHCP Server Stting ==============\n", __FUNCTION__, __LINE__ );
		HxLOG_Print("[%s:%d] \t start ip : %s\n", __FUNCTION__, __LINE__, inet_ntoa(startip) );
		HxLOG_Print("[%s:%d] \t end ip   : %s\n", __FUNCTION__, __LINE__, inet_ntoa(endip) );
		HxLOG_Print("[%s:%d] \t router   : %s\n", __FUNCTION__, __LINE__, inet_ntoa(router) );
		HxLOG_Print("[%s:%d] \t subnet   : %s\n", __FUNCTION__, __LINE__, inet_ntoa(netmask) );
		HxLOG_Print("[%s:%d] \t brdcast  : %s\n", __FUNCTION__, __LINE__, inet_ntoa(broadcast) );
		HxLOG_Print("[%s:%d] \t dns 01   : %s\n", __FUNCTION__, __LINE__, inet_ntoa(dns1) );
		HxLOG_Print("[%s:%d] \t dns 02   : %s\n", __FUNCTION__, __LINE__, inet_ntoa(dns2) );
		HxLOG_Print("[%s:%d] \t lease    : %d\n", __FUNCTION__, __LINE__, dhcps_config->leasetime);
		HxLOG_Print("[%s:%d] ============= DHCP Server Stting ================\n", __FUNCTION__, __LINE__);
	}
	EXIT;
	return ERR_OK;
}


