/**************************************************************
 *	@file		apk_network_netconfig.c
 *	appkit for network
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013-07-15
 *	@author			humax
 *	@brief			appkit for network
 **************************************************************/
#include	<hlib.h>

#include 	<apk.h>
#include 	<oapi.h>
#include 	<napi.h>

#if defined(CONFIG_SUPPORT_NIMA)


#define FuncEnter HxLOG_Debug("(+)[%s:%d] Enter!!\n",__FUNCTION__,__LINE__);
#define FuncLeave HxLOG_Debug("(-)[%s:%d] Leave -- \n",__FUNCTION__,__LINE__);

static void __apk_network_log_dev(HINT32 index, APKS_NETDEVINFO_t *netdev)
{
	HxLOG_Debug("#############################################\n");
	HxLOG_Debug("#### APK : DevInfo : ID : %d\n", index);
	HxLOG_Debug("#### APK : DevInfo : Type : %d\n", netdev->eType);
	HxLOG_Debug("#### APK : DevInfo : bConnected : %d\n", netdev->bConnected);
	HxLOG_Debug("#### APK : DevInfo : bNetActive : %d\n", netdev->bNetActive);
	HxLOG_Debug("#### APK : DevInfo : bRunning : %d\n", netdev->bRunning);
	HxLOG_Debug("#### APK : DevInfo : szMacAddr : %s\n", netdev->szMacAddr);
	HxLOG_Debug("#### APK : DevInfo : szDevName : %s\n", netdev->szDev_name);
	HxLOG_Debug("#############################################\n");
}

static void __apk_network_log_net(HINT32 index, APKS_NETIFADDR_t *netinfo)
{
	HUINT8 *p;
	HxLOG_Debug("#############################################\n");
	HxLOG_Debug("#### APK : DevInfo : ID : %d\n", index);
	HxLOG_Debug("#### APK : DevInfo : bDhcpOn : %d\n", netinfo->bDhcpOn);
	HxLOG_Debug("#### APK : DevInfo : bDnsAuto : %d\n", netinfo->bDnsAuto); p = (HUINT8*)&(netinfo->ulIpAddr);
	HxLOG_Debug("#### APK : DevInfo : ipAddr : %ld.%ld.%ld.%ld \n", p[0], p[1], p[2], p[3]); p = (HUINT8*)&(netinfo->ulNetmask);
	HxLOG_Debug("#### APK : DevInfo : szNetmask : %ld.%ld.%ld.%ld \n", p[0], p[1], p[2], p[3]); p = (HUINT8*)&(netinfo->ulGateway);
	HxLOG_Debug("#### APK : DevInfo : szGateway : %ld.%ld.%ld.%ld \n", p[0], p[1], p[2], p[3]); p = (HUINT8*)&(netinfo->ulDns1);
	HxLOG_Debug("#### APK : DevInfo : szDns1 : %ld.%ld.%ld.%ld \n", p[0], p[1], p[2], p[3]); p = (HUINT8*)&(netinfo->ulDns2);
	HxLOG_Debug("#### APK : DevInfo : szDns2 : %ld.%ld.%ld.%ld \n", p[0], p[1], p[2], p[3]);
	HxLOG_Debug("#############################################\n");
}

static HINT32 __apk_network_devtype_NAPItoAPK(HINT32 napi_dev_type)
{
	HINT32 apk_dev_type = -1;

	switch( napi_dev_type )
	{
		case eNAPI_NETCONF_LAN:
			apk_dev_type = eINTERFACE_TYPE_LAN; break;
		case eNAPI_NETCONF_WIFI:
			apk_dev_type = eINTERFACE_TYPE_WLAN; break;
		case eNAPI_NETCONF_PPPOE:
			apk_dev_type = eINTERFACE_TYPE_PPPOE; break;
		case eNAPI_NETCONF_CM:
		case eNAPI_NETCONF_BLUETOOTH:
		case eNAPI_NETCONF_RF4CE:
		default:
			apk_dev_type = -1; break;
	}

	HxLOG_Debug("(#)[%s:%d] napi_dev_type : %d, apk_dev_type : %d \n",__FUNCTION__,__LINE__, napi_dev_type, apk_dev_type);

	return apk_dev_type;
}


HERROR		APK_NETWORK_CONF_GetNumOfNetDev(HINT32 *pnCount )
{
	FuncEnter
	*pnCount = NAPI_NetConf_GetCount();
	HxLOG_Debug("(#)[%s:%d] netconf : device count : %d \n",__FUNCTION__,__LINE__, *pnCount);
	FuncLeave
	return ERR_OK;
}

HERROR		APK_NETWORK_CONF_GetDefaultDev(HINT32 *pnIndex )
{
	FuncEnter

	*pnIndex = NAPI_NetConf_GetDefaultIndex();
	HxLOG_Debug("(#)[%s:%d] netconf : default device : %d \n",__FUNCTION__,__LINE__, *pnIndex);
	FuncLeave

	return ERR_OK;
}

HERROR 		APK_NETWORK_CONF_IsNetworkAvailable(HBOOL *isEnable)
{
	HERROR hRet = ERR_OK;
	FuncEnter
	hRet = NAPI_NetConf_IsNetworkAvailable(isEnable);
	HxLOG_Debug("(#)[%s:%d] netconf : network Avaliable : %d \n",__FUNCTION__,__LINE__, *isEnable);
	FuncLeave
	return hRet;
}

HERROR		APK_NETWORK_CONF_GetDevInfo( HINT32 nIndex, APKS_NETDEVINFO_t *pstDevInfo )
{
	NAPINetConfInfo_t network_info;

	FuncEnter
	if( !pstDevInfo )
		return ERR_FAIL;

	HxSTD_MemSet(&network_info, 0x00, sizeof(NAPINetConfInfo_t));

	NAPI_NetConf_GetInfo(nIndex, &network_info);
	/* workaroud: need DI Network update */
	if ( network_info.id == 0 ) network_info.type = 0;
	else if ( network_info.id == 1 ) network_info.type = 1;
	// must be change apk interface type
	pstDevInfo->eType = __apk_network_devtype_NAPItoAPK(network_info.type);
	pstDevInfo->bConnected = network_info.linked;
	pstDevInfo->bNetActive = network_info.active;
	pstDevInfo->bRunning = 0;//tmp network_info.active;
	HxSTD_MemCopy(pstDevInfo->szMacAddr, network_info.ip_info.ipv4.macaddr, APKD_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(pstDevInfo->szDev_name, 0x00, sizeof(pstDevInfo->szDev_name));
	HxSTD_MemCopy(pstDevInfo->szDev_name, network_info.name, sizeof(pstDevInfo->szDev_name));

	HLIB_STD_StrNCpySafe(pstDevInfo->szFwVer, network_info.fw_ver, APKD_FW_VERSION_LEN_MAX);

	__apk_network_log_dev(nIndex, pstDevInfo);
	FuncLeave
	return ERR_OK;
}

HERROR		APK_NETWORK_CONF_GetMacAddr( HINT32 nIndex, HCHAR *pszMacAddr, HINT32 nBufSize )
{
	NAPINetConfInfo_t network_info;
	FuncEnter

	if( !pszMacAddr )
		return ERR_FAIL;

	HxSTD_MemSet(&network_info, 0x00, sizeof(NAPINetConfInfo_t));

	NAPI_NetConf_GetInfo(nIndex, &network_info);
	HxSTD_MemCopy(pszMacAddr, network_info.ip_info.ipv4.macaddr, nBufSize);
	FuncLeave

	return ERR_OK;
}

HERROR		APK_NETWORK_CONF_GetAddrInfo( HINT32 nIndex, APKS_NETIFADDR_t *pstNetAddr )
{
	NAPINetConfInfo_t network_info;
	FuncEnter
	if( !pstNetAddr )
		return ERR_FAIL;

	HxSTD_MemSet(&network_info, 0x00, sizeof(NAPINetConfInfo_t));

	NAPI_NetConf_GetInfo(nIndex, &network_info);

	pstNetAddr->bDhcpOn		= network_info.dhcp;
	pstNetAddr->bDnsAuto	= network_info.dns_auto;
	pstNetAddr->bIpv6		= network_info.ip_ver;
	pstNetAddr->bEnable		= network_info.linked;
	pstNetAddr->bIpv6Mode	= network_info.ip_ver;
	pstNetAddr->bDns6		= network_info.dns_auto;
	pstNetAddr->bActive		= network_info.active;
	//pstNetAddr->nPrefix;

	if( pstNetAddr->bIpv6 == eNAPI_NETCONF_IPV4 )
	{
		APK_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.ipAddr, &(pstNetAddr->ulIpAddr));
		APK_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.netmask, &(pstNetAddr->ulNetmask));
		APK_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.gateway, &(pstNetAddr->ulGateway));
		APK_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.dns1, &(pstNetAddr->ulDns1));
		APK_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.dns2, &(pstNetAddr->ulDns2));
	}
	else if( pstNetAddr->bIpv6 == eNAPI_NETCONF_IPV6 )
	{
		HxSTD_MemCopy(pstNetAddr->szIp6_addr, network_info.ip_info.ipv6.ipAddr, APKD_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(pstNetAddr->szIp6_gateway, network_info.ip_info.ipv6.gateway, APKD_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(pstNetAddr->szIp6_dns_addr_1, network_info.ip_info.ipv6.dns1, APKD_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(pstNetAddr->szIp6_dns_addr_2, network_info.ip_info.ipv6.dns2, APKD_NETWORK_MAX_IP6ADDR);

	}
	__apk_network_log_net(nIndex, pstNetAddr);
	FuncLeave

	return ERR_OK;
}

HERROR		APK_NETWORK_CONF_SetAddrInfo( HINT32 nIndex, APKS_NETIFADDR_t *pstNetAddr  )
{
	NAPINetConfInfo_t network_info;
	FuncEnter

	if( !pstNetAddr )
		return ERR_FAIL;

#if defined(CONFIG_OP_TDC_NETWORK_LED)	
	APK_NETWORK_LED_Init();

	APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_GREEN, TRUE);
#endif

	HxSTD_MemSet(&network_info, 0x00, sizeof(NAPINetConfInfo_t));
	network_info.id = nIndex;
	network_info.dhcp = pstNetAddr->bDhcpOn;
	network_info.dns_auto = pstNetAddr->bDnsAuto;
	network_info.auto_ip = TRUE;
	network_info.ip_ver = pstNetAddr->bIpv6;
	if( network_info.ip_ver == eNAPI_NETCONF_IPV4 )
	{
		APK_NETWORK_CONF_AddrN2A(pstNetAddr->ulIpAddr, network_info.ip_info.ipv4.ipAddr, APKD_NETWORK_MAX_IP4ADDR);
		APK_NETWORK_CONF_AddrN2A(pstNetAddr->ulNetmask, network_info.ip_info.ipv4.netmask, APKD_NETWORK_MAX_IP4ADDR);
		APK_NETWORK_CONF_AddrN2A(pstNetAddr->ulGateway, network_info.ip_info.ipv4.gateway, APKD_NETWORK_MAX_IP4ADDR);
		APK_NETWORK_CONF_AddrN2A(pstNetAddr->ulDns1, network_info.ip_info.ipv4.dns1, APKD_NETWORK_MAX_IP4ADDR);
		APK_NETWORK_CONF_AddrN2A(pstNetAddr->ulDns2, network_info.ip_info.ipv4.dns2, APKD_NETWORK_MAX_IP4ADDR);
	}
	else if( network_info.ip_ver == eNAPI_NETCONF_IPV6 )
	{
		HxSTD_MemCopy(network_info.ip_info.ipv6.ipAddr, pstNetAddr->szIp6_addr, APKD_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(network_info.ip_info.ipv6.gateway, pstNetAddr->szIp6_gateway, APKD_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(network_info.ip_info.ipv6.dns1, pstNetAddr->szIp6_dns_addr_1, APKD_NETWORK_MAX_IP6ADDR);
		HxSTD_MemCopy(network_info.ip_info.ipv6.dns2, pstNetAddr->szIp6_dns_addr_2, APKD_NETWORK_MAX_IP6ADDR);
	}

	if( NAPI_NetConf_SetInfo(nIndex, &network_info) != ERR_OK ) return ERR_FAIL;

	FuncLeave

	__apk_network_log_net(nIndex, pstNetAddr);
	return ERR_OK;
}

HERROR		APK_NETWORK_CONF_StopConnecting( HINT32 nIndex )
{
	/* empty */
	return ERR_OK;
}

HERROR		APK_NETWORK_CONF_SaveEnableStatus( HINT32 nIndex, HBOOL bOn )
{
	/* empty */
	return ERR_OK;
}

HERROR		APK_NETWORK_CONF_AddrN2A( HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize )
{
	snprintf( pszAddr, nbufSize, "%ld.%ld.%ld.%ld",
			(ulAddr & 0x000000FF), (ulAddr & 0x0000FF00)>>8, (ulAddr &0x00FF0000)>>16, (ulAddr & 0xFF000000)>>24 );

	return ERR_OK;
}

HERROR		APK_NETWORK_CONF_AddrA2N( HCHAR *pszAddr, HULONG *pulAddr )
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

HERROR		APK_NETWORK_CONF_IsSupportNetwork(HBOOL *bIsSupport )
{
	HERROR hRet = ERR_OK;
	FuncEnter
	hRet = NAPI_NetConf_IsNetworkAvailable(bIsSupport);
	FuncLeave
	return hRet;
}


/**
* This function is used to check the connection from STB to the Internet
* The Internet endpoint which is used is HUMAX Portal(Port 80)
* emit event eNETWORK_STATUS_INTERNET_CONNECTED if the STB can connect to the Internet in Ltimeout miliseconds
* emit event eNETWORK_STATUS_INTERNET_DISCONNECTED if the STB cannot connect to the Internet in Ltimeout miliseconds
* @param[in] lTimeout: time in Miliseconds
* @return ERR_OK if this function is called succesfully, ERR_FAIL if vice versa
*/

HERROR		APK_NETWORK_CONF_CheckInternetConnection(HINT32 lTimeout)
{
	HERROR hRet = ERR_FAIL;
	FuncEnter
	hRet = NAPI_NetConf_CheckInternetConnection(lTimeout);
	FuncLeave
	return hRet;
}


#else



HERROR		APK_NETWORK_CONF_GetNumOfNetDev(HINT32 *pnCount )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_GetDefaultDev(HINT32 *pnIndex )
{
	return ERR_FAIL;
}

HERROR 		APK_NETWORK_CONF_IsNetworkAvailable(HBOOL *isEnable)
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_GetDevInfo( HINT32 nIndex, APKS_NETDEVINFO_t *pstDevInfo )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_GetMacAddr( HINT32 nIndex, HCHAR *pszMacAddr, HINT32 nBufSize )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_GetAddrInfo( HINT32 nIndex, APKS_NETIFADDR_t *pstNetAddr )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_SetAddrInfo( HINT32 nIndex, APKS_NETIFADDR_t *pstNetAddr  )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_StopConnecting( HINT32 nIndex )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_SaveEnableStatus( HINT32 nIndex, HBOOL bOn )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_AddrN2A( HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_AddrA2N( HCHAR *pszAddr, HULONG *pulAddr )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_IsSupportNetwork(HBOOL *bIsSupport )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CONF_CheckInternetConnection(HINT32 lTimeout)
{
	return ERR_FAIL;
}

#endif
