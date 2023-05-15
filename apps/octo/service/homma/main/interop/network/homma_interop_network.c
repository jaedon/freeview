/**************************************************************
*	@file	homma_interop.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			HOMMA / interoperation supporting instead
*					of using APK
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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

/* ------------------------------------------------------------- //
 * Author : tjjang
 * Description : This is for replacing apk methods
 *  - HOMMA_INTEROP prefix is used for external methods
 *  - s_HOMMA_INTEROP prefix is used to represent static methods
 */

#include <homma_interop.h>

#ifdef HOMMA_USE_APPKIT_DIRECTLY
#else
#include <napi.h>

hintopNetwork_Notifier_t	s_fnNetworkNotifier;

#endif

/*
 * methods
 *  - static & external
 */


/* ############################################################################
              _                          _
             | |                        | |
 _ __    ___ | |_ __      __ ___   _ __ | | __
| '_ \  / _ \| __|\ \ /\ / // _ \ | '__|| |/ /
| | | ||  __/| |_  \ V  V /| (_) || |   |   <
|_| |_| \___| \__|  \_/\_/  \___/ |_|   |_|\_\
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  network  xxxxxxxx
############################################################################ */


// unwilling to use apps/octo/appkit/
#ifdef HOMMA_USE_APPKIT_DIRECTLY

HBOOL HOMMA_INTEROP_NETWORK_Init (void)
{
	return APK_NETWORK_Init();
}

HERROR HOMMA_INTEROP_NETWORK_CONF_GetMacAddr (HINT32 nIndex, HCHAR *pszMacAddr, HINT32 nBufSize)
{
	return APK_NETWORK_CONF_GetMacAddr(nIndex, pszMacAddr, nBufSize);
}

HERROR HOMMA_INTEROP_NETWORK_CONF_AddrN2A (HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize)
{
	return APK_NETWORK_CONF_AddrN2A(ulAddr, pszAddr, nbufSize);
}

HERROR HOMMA_INTEROP_NETWORK_CONF_GetNumOfNetDev(HINT32 *pnCount)
{
	return APK_NETWORK_CONF_GetNumOfNetDev(pnCount);
}

HERROR HOMMA_INTEROP_NETWORK_CONF_GetDefaultDev(HINT32 *pnIndex)
{
	return APK_NETWORK_CONF_GetDefaultDev(pnIndex);
}

HERROR HOMMA_INTEROP_NETWORK_CONF_IsNetworkAvailable(HBOOL *isEnable)
{
	return APK_NETWORK_CONF_IsNetworkAvailable(isEnable);
}

HERROR HOMMA_INTEROP_NETWORK_CONF_AddrA2N (HCHAR *pszAddr, HULONG *pulAddr)
{
	return APK_NETWORK_CONF_AddrA2N(pszAddr, pulAddr);
}

HERROR HOMMA_INTEROP_NETWORK_CONF_GetAddrInfo (HINT32 nIndex, HOMMA_INTEROP_NETIFADDR_t *pstNetAddr)
{
	return APK_NETWORK_CONF_GetAddrInfo(nIndex, pstNetAddr);
}

HERROR HOMMA_INTEROP_NETWORK_CONF_GetDevInfo (HINT32 nIndex, HOMMA_INTEROP_NETDEVINFO_t *pstDevInfo)
{
	return APK_NETWORK_CONF_GetDevInfo(nIndex, pstDevInfo);
}

void HOMMA_INTEROP_NETWORK_RegisterNotifier(HOMMA_INTEROP_NETWORK_CbHandler pfnObjectHandler)
{
	APK_NETWORK_RegisterNotifier(pfnObjectHandler);
}

#else // #ifdef HOMMA_USE_APPKIT_DIRECTLY else

void homma_interop_network_netconf_callback(HINT32 event, void* data)
{
	HINT32 eEvent = -1;
	HINT32 idx = 0;

	if(data)
	{
		idx = ((NAPINetConfInfo_t *)data)->id;
	}

	switch( event )
	{
		case NAPI_EVENT_NETCONF_CONN_SUCCESS:
			eEvent = eHOMMA_INTEROP_NET_STATUS_CON_SUCCESS; break;
		case NAPI_EVENT_NETCONF_CONN_FAIL:
			eEvent = eHOMMA_INTEROP_NET_STATUS_CON_FAIL; break;
		case NAPI_EVENT_NETCONF_AUTOIP_SUCCESS:
			eEvent = eHOMMA_INTEROP_NET_STATUS_CON_AUTOIP_SUCCESS; break;
		case NAPI_EVENT_NETCONF_AUTOIP_FAIL:
			eEvent = eHOMMA_INTEROP_NET_STATUS_CON_AUTOIP_FAIL; break;
		case NAPI_EVENT_NETCONF_LINK_CONNECTED:
			eEvent = eHOMMA_INTEROP_NET_STATUS_CABLE_LINK_CONNECTED; break;
		case NAPI_EVENT_NETCONF_LINK_DISCONNECTED:
			eEvent = eHOMMA_INTEROP_NET_STATUS_CABLE_LINK_DISCONNECTED; break;
		case NAPI_EVENT_NETCONF_STATUS_UPDATED:
			eEvent = eHOMMA_INTEROP_NET_STATUS_LINK_UPDATED; break;
		case NAPI_EVENT_NETCONF_DHCPSERVER_START:
			eEvent = eHOMMA_INTEROP_NET_STATUS_DHCPD_START; break;
		case NAPI_EVENT_NETCONF_DHCPSERVER_STOP:
			eEvent = eHOMMA_INTEROP_NET_STATUS_DHCPD_STOP; break;
		case NAPI_EVENT_NETCONF_INTERNET_CONNECTED:
			eEvent = eHOMMA_INTEROP_NET_STATUS_INTERNET_CONNECTED; break;
		case NAPI_EVENT_NETCONF_INTERNET_DISCONNECTED:
			eEvent = eHOMMA_INTEROP_NET_STATUS_INTERNET_DISCONNECTED; break;
	}

	HxLOG_Debug("	(+)[%s:%d] eEvent : %d, idx : %d\n", __FUNCTION__,__LINE__, eEvent, idx);

	if(s_fnNetworkNotifier)
	{
		s_fnNetworkNotifier(eEvent, idx);
	}
}

void homma_interop_network_wifi_callback(HINT32 event, void* data)
{
	HINT32 eEvent = -1;
	HINT32 idx = 0;

	if(data)
	{
		idx = ((NAPIWifiInfo_t*)data)->dev_id;
	}

	switch( event )
	{
		case NAPI_EVENT_WIFI_INSERTED:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_USB_INSERTED; break;
		case NAPI_EVENT_WIFI_EXTRACTED:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_USB_EXTRACTED; break;
		case NAPI_EVENT_WIFI_SCANNING_START:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_SCANNING_START; break;
		case NAPI_EVENT_WIFI_SCANNING_END:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_SCANNING_END; break;
		case NAPI_EVENT_WIFI_SCANNING_OUTPUT:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_SCANNING_OUTPUT; break;
		case NAPI_EVENT_WIFI_SCANNING_FAIL:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_SCANNING_FAIL; break;
		case NAPI_EVENT_WIFI_KEY_INVALID:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_KEY_INVALID; break;
		case NAPI_EVENT_WIFI_AP_CONNECT_SUCCESS:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_AP_CONNECT_SUCCESS; break;
		case NAPI_EVENT_WIFI_AP_CONNECT_FAIL:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_AP_CONNECT_FAIL; break;
		case NAPI_EVENT_WIFI_WPS_SUCCESS:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_WPS_SUCCESS; break;
		case NAPI_EVENT_WIFI_WPS_FAIL:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_WPS_FAIL; break;
		case NAPI_EVENT_WIFI_AP_DISCONNECTED:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_AP_DISCONNECTED; break;
		case NAPI_EVENT_WIFI_AP_RECONNECTED:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_AP_RECONNECTED; break;
		case NAPI_EVENT_WIFI_AP_CONNECT_STOP:
			eEvent = eHOMMA_INTEROP_NET_STATUS_WIFI_AP_CONNECT_STOP; break;
		case NAPI_EVENT_WIFI_LINK_UPDATED:
			eEvent = eHOMMA_INTEROP_NET_STATUS_LINK_UPDATED; break;
	}

	HxLOG_Debug("	(+)[%s:%d] eEvent : %d, idx : %d\n", __FUNCTION__,__LINE__, eEvent, idx);

	if(s_fnNetworkNotifier)
	{
		s_fnNetworkNotifier(eEvent, idx);
	}
}

// APK_NETWORK_Init()
HBOOL HOMMA_INTEROP_NETWORK_Init (void)
{
	NAPI_Init();
	NAPI_NetConf_RegisterCallback(homma_interop_network_netconf_callback);
	NAPI_Wifi_RegisterCallback(homma_interop_network_wifi_callback);

	return TRUE;
}

//APK_NETWORK_CONF_GetMacAddr(i, macBuf, sizeof(macBuf));
HERROR HOMMA_INTEROP_NETWORK_CONF_GetMacAddr (HINT32 nIndex, HCHAR *pszMacAddr, HINT32 nBufSize)
{
	NAPINetConfInfo_t network_info;

	if (!pszMacAddr)
		return ERR_FAIL;

	HxSTD_MemSet(&network_info, 0x00, sizeof(NAPINetConfInfo_t));

	NAPI_NetConf_GetInfo(nIndex, &network_info);
	HxSTD_MemCopy(pszMacAddr, network_info.ip_info.ipv4.macaddr, nBufSize);

	return ERR_OK;
}

//APK_NETWORK_CONF_AddrN2A(devAddr.ulDns2, pIface->szDns2, sizeof(pIface->szDns2));
HERROR HOMMA_INTEROP_NETWORK_CONF_AddrN2A (HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize)
{
	snprintf(pszAddr, nbufSize, "%ld.%ld.%ld.%ld",
				(ulAddr & 0x000000FF), (ulAddr & 0x0000FF00)>>8, (ulAddr &0x00FF0000)>>16, (ulAddr & 0xFF000000)>>24);

	return ERR_OK;
}

//APK_NETWORK_CONF_GetNumOfNetDev(&devCnt);
HERROR HOMMA_INTEROP_NETWORK_CONF_GetNumOfNetDev(HINT32 *pnCount)
{
	*pnCount = NAPI_NetConf_GetCount();
	return ERR_OK;
}

//APK_NETWORK_CONF_GetDefaultDev(&dev_idx);
HERROR HOMMA_INTEROP_NETWORK_CONF_GetDefaultDev(HINT32 *pnIndex)
{
	*pnIndex = NAPI_NetConf_GetDefaultIndex();
	return ERR_OK;
}

//APK_NETWORK_CONF_IsNetworkAvailable(&bIsAvailable)
HERROR HOMMA_INTEROP_NETWORK_CONF_IsNetworkAvailable(HBOOL *isEnable)
{
	HERROR hRet = ERR_OK;
	hRet = NAPI_NetConf_IsNetworkAvailable(isEnable);
	return hRet;
}

// APK_NETWORK_CONF_AddrA2N()
HERROR HOMMA_INTEROP_NETWORK_CONF_AddrA2N (HCHAR *pszAddr, HULONG *pulAddr)
{
	HULONG  octet[4] = { 0 };

	sscanf(pszAddr, "%ld.%ld.%ld.%ld", &octet[0], &octet[1], &octet[2], &octet[3]);

	*pulAddr = 0;
	*pulAddr |= octet[0];
	*pulAddr |= (octet[1] << 8);
	*pulAddr |= (octet[2] << 16);
	*pulAddr |= (octet[3] << 24);

	return ERR_OK;
}

//APK_NETWORK_CONF_GetAddrInfo(didx, &tNetAddr);
HERROR HOMMA_INTEROP_NETWORK_CONF_GetAddrInfo (HINT32 nIndex, HOMMA_INTEROP_NETIFADDR_t *pstNetAddr)
{
	NAPINetConfInfo_t network_info;

	if(!pstNetAddr)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(&network_info, 0x00, sizeof(NAPINetConfInfo_t));

	NAPI_NetConf_GetInfo(nIndex, &network_info);

	pstNetAddr->bDhcpOn = network_info.dhcp;
	pstNetAddr->bDnsAuto = network_info.dns_auto;
	pstNetAddr->bIpv6 = network_info.ip_ver;
	pstNetAddr->bEnable = network_info.linked;
	pstNetAddr->bIpv6Mode = network_info.ip_ver;
	pstNetAddr->bDns6 = network_info.dns_auto;

	if(pstNetAddr->bIpv6 == eNAPI_NETCONF_IPV4)
	{
		HOMMA_INTEROP_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.ipAddr, &(pstNetAddr->ulIpAddr));
		HOMMA_INTEROP_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.netmask, &(pstNetAddr->ulNetmask));
		HOMMA_INTEROP_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.gateway, &(pstNetAddr->ulGateway));
		HOMMA_INTEROP_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.dns1, &(pstNetAddr->ulDns1));
		HOMMA_INTEROP_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.dns2, &(pstNetAddr->ulDns2));
	}
	else if(pstNetAddr->bIpv6 == eNAPI_NETCONF_IPV6)
	{
		HxSTD_MemCopy(pstNetAddr->szIp6_addr, network_info.ip_info.ipv6.ipAddr, HOMMA_INTEROP_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(pstNetAddr->szIp6_gateway, network_info.ip_info.ipv6.gateway, HOMMA_INTEROP_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(pstNetAddr->szIp6_dns_addr_1, network_info.ip_info.ipv6.dns1, HOMMA_INTEROP_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(pstNetAddr->szIp6_dns_addr_2, network_info.ip_info.ipv6.dns2, HOMMA_INTEROP_NETWORK_MAX_IP6ADDR);
	}

	return ERR_OK;
}

static HINT32 s_HOMMA_INTEROP_network_devtype_NAPItoINTEROP(HINT32 napi_dev_type)
{
	HINT32 apk_dev_type = -1;

	switch (napi_dev_type)
	{
		case eNAPI_NETCONF_LAN:
			apk_dev_type = eHOMMA_INTEROP_INTERFACE_TYPE_LAN; break;
		case eNAPI_NETCONF_WIFI:
			apk_dev_type = eHOMMA_INTEROP_INTERFACE_TYPE_WLAN; break;
		case eNAPI_NETCONF_PPPOE:
			apk_dev_type = eHOMMA_INTEROP_INTERFACE_TYPE_PPPOE; break;
		case eNAPI_NETCONF_CM:
		case eNAPI_NETCONF_BLUETOOTH:
		case eNAPI_NETCONF_RF4CE:
		default:
			apk_dev_type = -1; break;
	}

	return apk_dev_type;
}

//APK_NETWORK_CONF_GetDevInfo(dev_idx, &dev_info);
HERROR HOMMA_INTEROP_NETWORK_CONF_GetDevInfo (HINT32 nIndex, HOMMA_INTEROP_NETDEVINFO_t *pstDevInfo)
{
	NAPINetConfInfo_t network_info;

	if (!pstDevInfo)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(&network_info, 0x00, sizeof(NAPINetConfInfo_t));

	NAPI_NetConf_GetInfo(nIndex, &network_info);
	/* workaroud: need DI Network update */
	if (network_info.id == 0)
	{
		network_info.type = 0;
	}
	else if (network_info.id == 1)
	{
		network_info.type = 1;
	}

	// must be change apk interface type
	pstDevInfo->eType = s_HOMMA_INTEROP_network_devtype_NAPItoINTEROP(network_info.type);
	pstDevInfo->bConnected = network_info.linked;
	pstDevInfo->bNetActive = network_info.active;
	pstDevInfo->bRunning = 0; //tmp network_info.active;
	HxSTD_MemCopy(pstDevInfo->szMacAddr, network_info.ip_info.ipv4.macaddr, HOMMA_INTEROP_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(pstDevInfo->szDev_name, 0x00, sizeof(pstDevInfo->szDev_name));
	HxSTD_MemCopy(pstDevInfo->szDev_name, network_info.name, sizeof(pstDevInfo->szDev_name));

	return ERR_OK;
}

void HOMMA_INTEROP_NETWORK_RegisterNotifier(hintopNetwork_Notifier_t pfnObjectHandler)
{
	if(pfnObjectHandler != NULL)
	{
		 s_fnNetworkNotifier = pfnObjectHandler;
	}
}

#endif // #ifdef HOMMA_USE_APPKIT_DIRECTLY endif

