/*************************************************************************************************************
	File 		: nico_operator_common.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/06/05
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

	THE SOFTWARE IS PROVIDED 징째AS IS징짹 AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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

#include "nico_operator_common.h"
#include "nico_debug.h"
#include "hlib.h"

STATIC HBOOL	__nico_op_common_address_isValid(HUINT8 *pAddress)
{
	HINT32		i = 0, nNumOfList = 0;
	HBOOL 	bRet = TRUE;

	const IPaddr_t		invalidIPAddress[] = {
				{ MAKE_IP_ADDRESS(0, 0, 0, 0), MAKE_IP_ADDRESS(255, 255, 255, 255), },
				{ MAKE_IP_ADDRESS(255, 255, 255, 255), MAKE_IP_ADDRESS(255, 255, 255, 255), },
				{ MAKE_IP_ADDRESS(127, 0, 0, 0), MAKE_IP_ADDRESS(255, 0, 0, 0), },
				{ MAKE_IP_ADDRESS(192, 0, 2, 0), MAKE_IP_ADDRESS(255, 255, 255, 0), },	/* 192.0.2.X 전부 안됨. */
	};
	ENTRY;
	nNumOfList = (HINT32)(sizeof(invalidIPAddress)/sizeof(IPaddr_t));
	for(i = 0; i < nNumOfList; i++)
	{
		/* class check */
		if((*pAddress & invalidIPAddress[i].ulNetMask)
			== (invalidIPAddress[i].ulIPaddr & invalidIPAddress[i].ulNetMask))
		{
			/* Not valid */
			bRet = FALSE;
			return bRet;
		}
	}
	EXIT;
	return bRet;
}

/**
* This function is used to check whether an IPv4 address is AutoIP address
* @param[in] pAddress: IP address in structure of a array of 4 integers
* @return TRUE if the IPv4 address is an AutoIP address
*/

STATIC HBOOL	__nico_op_common_isAutoIP_Address(HUINT8 *pAddress)
{
	return (169 == pAddress[0] && 254 == pAddress[1]);
}

STATIC eNicoNetConfDevType __nico_op_common_convert_devtype(DI_NETWORK_DEV_TYPE_e di_devtype)
{
	eNicoNetConfDevType type = eNICO_DEVTYPE_MAX;
	ENTRY;
	switch(di_devtype)
	{
		case DI_NETWORK_DEV_LAN: 		type = eNICO_DEVTYPE_LAN; 	break;
		case DI_NETWORK_DEV_WAN: 		type = eNICO_DEVTYPE_WAN; 	break;
		case DI_NETWORK_DEV_CABLEMODEM: type = eNICO_DEVTYPE_CM; 	break;
		case DI_NETWORK_DEV_WIFI: 		type = eNICO_DEVTYPE_WIFI; 	break;
		case DI_NETWORK_DEV_PPPOE: 		type = eNICO_DEVTYPE_PPPOE; break;
		default: 						type = di_devtype; 			break;
		/* bluetooth, rf4ce type is nothing in di */
	}
	EXIT;
	return type;
}

STATIC HERROR __nico_op_common_update_ipv4(NICO_NetConf_Info_t *netinfo, DI_NETWORK_GET_INFO_t *di_netinfo)
{
	HUINT8	gateway[NICO_MAX_IP4ADDR] = {0,};
	HERROR hRet = ERR_OK;
	ENTRY;
	//1 #### for ipv4
	HxSTD_MemCopy(netinfo->ip_info.ipv4.macaddr, di_netinfo->aucMacAddr, NICO_MAX_MACADDR);
	HxSTD_MemCopy(netinfo->ip_info.ipv4.ipAddr, di_netinfo->stIpv4Addr.aucIpAddr, NICO_MAX_IP4ADDR);
	HxSTD_MemCopy(netinfo->ip_info.ipv4.netmask, di_netinfo->stIpv4Addr.aucMaskAddr, NICO_MAX_IP4ADDR);
	HxSTD_MemCopy(netinfo->ip_info.ipv4.broadcast, di_netinfo->stIpv4Addr.aucBcastAddr, NICO_MAX_IP4ADDR);
	hRet = DI_NETWORK_GetDefaultRoute(netinfo->id, DI_NETWORK_IP_VER_4, (HUINT8*)&gateway);
	if( !( (gateway[0] == 0) && (gateway[1] == 0) && (gateway[2] == 0) && (gateway[3] == 0) ) )
	{
		HxSTD_MemCopy(netinfo->ip_info.ipv4.gateway, gateway, NICO_MAX_IP4ADDR);
	}
	else if(__nico_op_common_isAutoIP_Address(netinfo->ip_info.ipv4.ipAddr))
	{
		//Set default getway as own IP address
		HxSTD_MemCopy(netinfo->ip_info.ipv4.gateway, di_netinfo->stIpv4Addr.aucIpAddr, NICO_MAX_IP4ADDR);
	}
	EXIT;
	return hRet;
}

STATIC HERROR __nico_op_common_update_ipv6(NICO_NetConf_Info_t *netinfo, DI_NETWORK_GET_INFO_t *di_netinfo)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	HxSTD_MemCopy(netinfo->ip_info.ipv6.macaddr, di_netinfo->aucMacAddr, NICO_MAX_MACADDR);
	HxSTD_MemCopy(netinfo->ip_info.ipv6.ipAddr, di_netinfo->stIpv6Addr.aucIpAddr, NICO_MAX_IP6ADDR);

	netinfo->ip_info.ipv6.prefix = di_netinfo->stIpv6Addr.ulPrefix;
	netinfo->ip_info.ipv6.addrscope = (eNICO_IPV6_ADDR_SCOPE_e)di_netinfo->stIpv6Addr.eScope;

	hRet = DI_NETWORK_GetDefaultRoute(netinfo->id, DI_NETWORK_IP_VER_6, (HUINT8*)netinfo->ip_info.ipv6.gateway);
	EXIT;
	return hRet;
}

STATIC HERROR __nico_op_common_update_dns(NICO_NetConf_Info_t *nico_netinfo, NICO_NetDevice_Info_t *deviceinfo)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = DI_NETWORK_GetDnsCount(&deviceinfo->ip4_dns_count, &deviceinfo->ip6_dns_count);
	if(ERR_OK == hRet)
	{
		HINT32 i = 0;
		struct in_addr	ip4_temp;
		struct in6_addr ip6_temp;
		for(i=0; i<deviceinfo->ip4_dns_count && i < eNICO_Dns_Max; i++)
		{
			hRet = DI_NETWORK_GetDnsInfo(eNICO_VER_IPV4, i, (HUINT8 *)&ip4_temp);
			if(ERR_OK == hRet)
			{
				HxSTD_MemCopy(&nico_netinfo->ip_info.ipv4.dns[i], &ip4_temp, sizeof(struct in_addr));
			}
		}
		for(i=0; i<deviceinfo->ip6_dns_count && i < eNICO_Dns_Max; i++)
		{
			hRet = DI_NETWORK_GetDnsInfo(eNICO_VER_IPV6, i, (HUINT8 *)&ip6_temp);
			if(ERR_OK == hRet)
			{
				HxSTD_MemCopy(&nico_netinfo->ip_info.ipv6.dns[i], &ip6_temp, sizeof(struct in6_addr));
			}
		}
	}
	EXIT;
	return hRet;
}

STATIC void __nico_op_common_update(HINT32 id, NICO_NetDevice_Info_t *deviceinfo, DI_NETWORK_GET_INFO_t *di_netinfo)
{
	NICO_NetConf_Info_t *netinfo = &deviceinfo->netinfo;
	ENTRY;
	deviceinfo->id = id;

	netinfo->id = id;
	netinfo->dev_type = __nico_op_common_convert_devtype(di_netinfo->eDevType);
	netinfo->linked = di_netinfo->bIsConnected;		// bIsConntected(=IFF_RUNNING), 드라이버 로드 상태라고 보면 됨.
	netinfo->active = di_netinfo->bIsUp &&
						di_netinfo->bIsConnected &&
						__nico_op_common_address_isValid(di_netinfo->stIpv4Addr.aucIpAddr);   // bIsUp(=ifup)

	HLIB_STD_StrNCpySafe(deviceinfo->name, di_netinfo->szName, sizeof(deviceinfo->name));
	HLIB_STD_StrNCpySafe(netinfo->name, di_netinfo->szName, sizeof(netinfo->name));
	HxSTD_MemCopy(netinfo->ip_info.ipv4.macaddr, di_netinfo->aucMacAddr, NICO_MAX_MACADDR);

	if(FALSE == netinfo->linked )
	{
		/* Don't do anything */
	}
	else
	{
		__nico_op_common_update_ipv4(netinfo, di_netinfo);
		__nico_op_common_update_ipv6(netinfo, di_netinfo);
		__nico_op_common_update_dns(netinfo, deviceinfo);
	}
	EXIT;
}

STATIC void __nico_op_common_update_netinfo(NICO_NetDevice_Info_t *deviceinfo)
{
	NICO_NetConf_Info_t *netinfo = &deviceinfo->netinfo;
	ENTRY;
	(void*)netinfo;

	EXIT;
}

#if defined(CONFIG_MW_INET_WLAN)
HBOOL	nico_op_common_wifi_is_connected( HUINT32 id)
{
	HBOOL bRet = FALSE;
	DI_WIFI_INFO_t	di_apinfo;
	HxSTD_MemSet( &di_apinfo, 0x00, sizeof(DI_WIFI_INFO_t) );

	if(DI_ERR_OK == DI_WIFI_GetAPInfo( id, &di_apinfo ))
	{
		bRet = TRUE;
	}
	return bRet;
}

HBOOL	nico_op_common_wifi_scan(HUINT32 id)
{
	HBOOL bRet = TRUE;
	ENTRY;
	DI_WIFI_Scan(id);
	EXIT;
	return bRet;
}

void nico_op_common_get_connected_apinfo(HINT32 id, NICO_Wifi_APInfo_t *nico_apinfo)
{
	DI_WIFI_INFO_t di_apinfo;
	ENTRY;
	HxSTD_MemSet(&di_apinfo, 0x00, sizeof(DI_WIFI_INFO_t));

	if(DI_ERR_OK == DI_WIFI_GetAPInfo(id, &di_apinfo))
	{
		nico_apinfo->channel = di_apinfo.ulChannel;
		nico_apinfo->freq = di_apinfo.ulFreq;
		nico_apinfo->is_wps = di_apinfo.bWPS;
		nico_apinfo->op_mode = di_apinfo.eOpMode;
		nico_apinfo->wave_strength = di_apinfo.nWaveStrength;
		nico_apinfo->bitrate = di_apinfo.nBitRate;

		nico_apinfo->secure_type = nico_op_common_ap_securetype(di_apinfo.eSecureType);
		nico_apinfo->authen_type = nico_op_common_ap_authentype(di_apinfo.eAuthenType);
		nico_apinfo->encrypt_type = nico_op_common_ap_encrypttype(di_apinfo.eEncryptType);
		nico_apinfo->keyindex = di_apinfo.aucKeyIndex;
		nico_apinfo->is_connected = TRUE;
		HxSTD_MemCopy(nico_apinfo->macaddr, di_apinfo.aucMAC, sizeof(nico_apinfo->macaddr));
		HxSTD_MemCopy(nico_apinfo->essid, di_apinfo.aucESSID, sizeof(nico_apinfo->essid));
		HxSTD_MemCopy(nico_apinfo->id, di_apinfo.aucId, sizeof(nico_apinfo->id));
		HxSTD_MemCopy(nico_apinfo->apkey, di_apinfo.aucKey, sizeof(nico_apinfo->apkey));
	}
	else
	{
		HxSTD_MemSet(nico_apinfo, 0x00, sizeof(NICO_Wifi_APInfo_t));
		HxLOG_Debug("There is no ap connected !!! \n");
	}
	EXIT;

}

STATIC void __nico_op_common_update_apinfo(HINT32 id, NICO_NetDevice_Info_t *deviceinfo)
{
	ENTRY;
	nico_op_common_get_connected_apinfo(id, &deviceinfo->apinfo_connected);
	EXIT;
}
#else
STATIC void __nico_op_common_update_apinfo(HINT32 id, NICO_NetDevice_Info_t *deviceinfo)
{
}
#endif
#if defined(CONFIG_MW_INET_CM)
STATIC void __nico_op_common_update_cminfo(NICO_NetDevice_Info_t *deviceinfo)
{
	const HUINT32 channelid = 0;
	HINT32 ber = 0;
	HINT32 mode = 0;
	NICO_CM_Info_t *nico_cminfo = &deviceinfo->cminfo;
	eNICO_CM_Status state  = eNICO_CM_NONE;

	HxSTD_MemSet(nico_cminfo, 0x00, sizeof(NICO_CM_Info_t));
	ENTRY;
	switch ( DI_CM_CmStatus() )
	{
		case 0:		state = eNICO_CM_NONE;	break;
		case 1:		state = eNICO_CM_OTHER; break;
		case 2:		state = eNICO_CM_NOT_READY; break;
		case 3:		state = eNICO_CM_NOT_SYNC;	break;
		case 4:		state = eNICO_CM_PHY_SYNC_DONE; break;
		case 5:		state = eNICO_CM_PARAM_ACQUIRED; break;
		case 6:		state = eNICO_CM_RANGING_COMPLETE; break;
		case 7:		state = eNICO_CM_IP_COMPLETE; break;
		case 8:		state = eNICO_CM_TOD_CERTIFY; break;
		case 9:		state = eNICO_CM_SECURITY_CERTIFY; break;
		case 10:	state = eNICO_CM_PARAM_TRAANSE_COMPLETE; break;
		case 11:	state = eNICO_CM_REGIST_COMPLETE; break;
		case 12:	state = eNICO_CM_OPERATION; break;
		case 13:	state = eNICO_CM_ACCESS_DENIED; break;
		default :	state = eNICO_CM_NONE; break;
	}
	nico_cminfo->state == state;

	if(DI_ERR_OK == DI_CM_GetFDCBer30( (HINT32)channelid, &ber ))
	{
		nico_cminfo->down_stream.ber = ber / 1000000;
	}

	if(eNICO_CM_NOT_SYNC < nico_cminfo->state)
	{
		DI_CM_GetFDCFreq30(channelid, &nico_cminfo->down_stream.freq);
	}

	if(DI_ERR_OK == DI_CM_GetFDCMod30( (HINT32)channelid, &mode ))
	{
		HINT32 temp = -1;
		switch ( mode )
		{
			case DI_CM_FDC_MOD_ERROR :
			case DI_CM_FDC_MOD_UNKNOWN:	temp = eNICO_CM_QAM_UNKNOWN;break;
			case DI_CM_FDC_MOD_OTHER :	temp = eNICO_CM_QAM_OTHER; 	break;
			case DI_CM_FDC_MOD_QAM64 :	temp = eNICO_CM_QAM_64;		break;
			case DI_CM_FDC_MOD_QAM256 : temp = eNICO_CM_QAM_256;	break;
			default : temp = eNICO_CM_QAM_UNKNOWN;break;
		}
		nico_cminfo->down_stream.qam_type = temp;
	}
	DI_CM_GetFDCPower30(channelid, &nico_cminfo->down_stream.signal_level);
	DI_CM_GetFDCSnr30( (HINT32)channelid, &nico_cminfo->down_stream.snr );
	DI_CM_GetRDCChannelId30( (HINT32)channelid, &nico_cminfo->up_stream.channel_id );

	if(eNICO_CM_NOT_SYNC < nico_cminfo->state)
	{
		DI_CM_GetRDCSymbolRate30( (HINT32)channelid, &nico_cminfo->up_stream.bandwidth );
		DI_CM_GetRDCFreq30( (HINT32)channelid, &nico_cminfo->up_stream.freq );
	}
	if ( eNICO_CM_PHY_SYNC_DONE < nico_cminfo->state )
	{
		DI_CM_GetRDCPower30( (HINT32)channelid, &nico_cminfo->up_stream.signal_level );
	}
	EXIT;
}

STATIC void __nico_op_common_update_cm_ip(NICO_NetDevice_Info_t *deviceinfo)
{
	NICO_NetConf_Info_t *netinfo = &deviceinfo->netinfo;
	ENTRY;
	if(DI_ERR_OK != DI_CM_GetCmMac(netinfo->ip_info.ipv4.macaddr))
		HxLOG_Error("DI_CM_GetCmMac(). ERROR(%d)\n");

	if ( DI_ERR_OK != DI_CM_GetCmIp((HUINT32 *)&netinfo->ip_info.ipv4.ipAddr) )
		HxLOG_Error("DI_CM_GetCmIp(). ERROR(%d)\n");

	if ( DI_ERR_OK != DI_CM_GetGateway( (HUINT32 *)&netinfo->ip_info.ipv4.gateway) )
		HxLOG_Error("DI_CM_GetGateway(). ERROR(%d)\n");

	if ( DI_ERR_OK != DI_CM_GetNetmask( (HUINT32 *)&netinfo->ip_info.ipv4.netmask) )
		HxLOG_Error("DI_CM_GetNetmask(). ERROR(%d)\n");
	EXIT;
}
#else
STATIC void __nico_op_common_update_cminfo(NICO_NetDevice_Info_t *deviceinfo)
{
	NICO_CM_Info_t *nico_cminfo = &deviceinfo->cminfo;
	(void*)nico_cminfo;
}

STATIC void __nico_op_common_update_cm_ip(NICO_NetDevice_Info_t *deviceinfo)
{
	NICO_CM_Info_t *nico_cminfo = &deviceinfo->cminfo;
	(void*)nico_cminfo;
}

#endif

#if defined(CONFIG_PROD_DEVICE_BLUETOOTH)
STATIC void __nico_op_common_update_bluetooth(HINT32 id, NICO_NetDevice_Info_t *deviceinfo)
{
	HCHAR dev_name[NICO_NET_INTERFACE_NAME_MAX] = NICO_BT_DEVICE_NAME;
	DI_BT_DEV_LIST_t di_bt_list;
	DI_BT_DEV_INFO_t di_bt_host;
	NICO_BT_Info_t *nico_bt = &deviceinfo->btinfo;
	HINT32 i = 0;

	HxSTD_MemSet(&di_bt_host, 0x00, sizeof(DI_BT_DEV_INFO_t));

	ENTRY;

	deviceinfo->id = id;
	HxSTD_MemCopy(deviceinfo->name, dev_name, NICO_NET_INTERFACE_NAME_MAX);

	if(DI_ERR_OK == DI_BT_GetHostInfo(&di_bt_host))
	{
		HxSTD_MemCopy(nico_bt->host_info.name, di_bt_host.ucName, NICO_BT_NAME_MAX);
		HxSTD_MemCopy(nico_bt->host_info.bdaddr, di_bt_host.ucBdAddr, NICO_BT_BDADDR_LEN);
		HxSTD_MemCopy(nico_bt->host_info.dev_class, di_bt_host.ucClass, NICO_BT_CLASS_LEN);
		nico_bt->host_info.status = di_bt_host.etDevStatus;
		nico_bt->host_info.available_profile = di_bt_host.etAvailableProfile;

		HxLOG_Debug("++ BT HOST INFO +++++++++++++++++++++++++++++++ \n");
		nico_debug_print_bt_info(&nico_bt->host_info, 1);
		HxLOG_Debug("+++++++++++++++++++++++++++++++++++++++++++++++ \n");
	}

	if(DI_ERR_OK == DI_BT_GetDeviceList(&di_bt_list))
	{
		nico_bt->count = di_bt_list.ulNumOfData;
		for( i=0; i<nico_bt->count; i++ )
		{
			HxSTD_MemCopy(nico_bt->dev_list[i].name, di_bt_list.stBtDevList[i].ucName, NICO_BT_NAME_MAX);
			HxSTD_MemCopy(nico_bt->dev_list[i].bdaddr, di_bt_list.stBtDevList[i].ucBdAddr, NICO_BT_BDADDR_LEN);
			HxSTD_MemCopy(nico_bt->dev_list[i].dev_class, di_bt_list.stBtDevList[i].ucClass, NICO_BT_CLASS_LEN);
			nico_bt->dev_list[i].available_profile = di_bt_list.stBtDevList[i].etAvailableProfile;
			nico_bt->dev_list[i].status = di_bt_list.stBtDevList[i].etDevStatus;
		}
		nico_debug_print_bt_info(nico_bt->dev_list, nico_bt->count);
	}
	EXIT;
	return;
}
#else
STATIC void __nico_op_common_update_bluetooth(HINT32 id, NICO_NetDevice_Info_t *deviceinfo)
{
	return;
}
#endif

#if defined(CONFIG_MW_INET_PPPOE)
STATIC void __nico_op_common_update_pppoe(HINT32 id, NICO_NetDevice_Info_t *deviceinfo)
{
	NICO_PPPoE_Info_t *nico_pppoeinfo = &deviceinfo->pppoeinfo;
	DI_PPPOE_INFO_t di_pppoeinfo;

	if(DI_ERR_OK == DI_PPPOE_GetInfo(id, &di_pppoeinfo))
	{
		nico_pppoeinfo->id = id;
		if( di_pppoeinfo.bConnected == TRUE )
		{
			deviceinfo->netinfo.linked = TRUE;
			deviceinfo->netinfo.active = TRUE;
		}
		HxSTD_MemCopy(nico_pppoeinfo->connect_id, di_pppoeinfo.aucId, NICO_PPPOE_ID_LEN);
		HxSTD_MemCopy(nico_pppoeinfo->connect_pw, di_pppoeinfo.aucPassword, NICO_PPPOE_PW_LEN);
		nico_debug_print_pppoe_info(nico_pppoeinfo);
	}
	else
	{
		HxLOG_Warning("pppoe error !!! \n");
	}
}

#if 0 // sgchoi
void __nico_op_common_set_rt_pppoe(pevent_msg event, HINT32 dev_count,
														NICO_NetDevice_Info_t **deviceinfo, nico_dievtmsg_t *di_event)
{
	HINT32 i = 0;
	HCHAR buf[128] 	= { 0 };
	HINT32 default_route_id = -1;
	HINT32 default_route_priority = 0;
	static HINT32 default_pppoe_id = -1;
	static HINT32 default_eth_id   = -1;

	switch(event->type.evt_type)
	{
		case EVENT_NET_PPPOE_CONNECT_SUCCESS:
			{
				default_route_priority = dev_count;
				for( i=0; i<dev_count; i++ )
				{
					HxSTD_PrintToStrN( buf, sizeof(buf), "route del default %s", deviceinfo[i]->name);

					VK_SYSTEM_Command(buf);
					if(deviceinfo[i]->netinfo.linked && deviceinfo[i]->netinfo.active)
					{
						default_route_priority = deviceinfo[i]->netinfo.priority;
						default_route_id = deviceinfo[i]->id;
					}
				}
				if((default_route_id >= 0) && (default_route_id < dev_count))
				{
					HxSTD_PrintToStrN( buf, sizeof(buf), "route add default %s", deviceinfo[default_route_id]->name);
					VK_SYSTEM_Command(buf);
					default_pppoe_id = default_route_id;
				}
			}
			break;
		case EVENT_NET_PLUGGED_OUT:
			{
				// if ppp0 is route default, route default ppp0 must be deleted.
				HBOOL bIsPPPoEConnect = FALSE;
				default_eth_id = -1;
				DRV_PPPOE_GetLinkStatus(default_pppoe_id, &bIsPPPoEConnect); // <-- warning
				if( (bIsPPPoEConnect == TRUE) && (default_pppoe_id > 0) )  // If PPPoE is coneected, delete ppp0
				{
					DI_PPPOE_Disconnect(default_pppoe_id);
					default_pppoe_id = -1;
				}
			}
			break;
		case EVENT_NET_CONNECT_SUCCESS:
			{
				HBOOL bIsPPPoEConnect = TRUE;
				HINT32 count = 3;

				default_route_priority = dev_count;
				for( i=0; i<dev_count; i++ )
				{
					if(strstr(deviceinfo[i]->name, "eth"))
					{
						default_eth_id = deviceinfo[i]->id;
						HxLOG_Debug("\033[01;36m[%s:%s:%d] default_eth_id : %d  \033[00m\n", __FILE__, __FUNCTION__, __LINE__, default_eth_id);
					}

					if(strstr(deviceinfo[i]->name, "ppp"))
					{
						default_pppoe_id = deviceinfo[i]->id;
						HxLOG_Debug("\033[01;36m[%s:%s:%d] default_pppoe_id : %d  \033[00m\n", __FILE__, __FUNCTION__, __LINE__, default_pppoe_id);
					}
				}
				// ethernet linked is true, pppoe must not be connect.
				if(deviceinfo[default_pppoe_id]->pppoeinfo.enable == TRUE)
					return;

				DRV_PPPOE_GetLinkStatus(default_pppoe_id, &bIsPPPoEConnect);
				if( bIsPPPoEConnect == FALSE && default_pppoe_id > 0)
				{
RETRY:
					if( (deviceinfo[default_pppoe_id]->netinfo.linked == FALSE) &&
							(deviceinfo[default_pppoe_id]->netinfo.active == FALSE) )
					{
						DI_PPPOE_INFO_t pppoe_info = {0, };
						DI_PPPOE_GetInfo(default_pppoe_id, &pppoe_info);
						DI_PPPOE_Connect(default_pppoe_id, &pppoe_info);
					}
				}
				DRV_PPPOE_GetLinkStatus(default_pppoe_id, &bIsPPPoEConnect);
				if( (bIsPPPoEConnect == FALSE) && count--)
				{
					VK_TASK_Sleep(500);
					goto RETRY;
				}
			}
			break;
		default:
			break;
	}
}
#endif

#else
STATIC void __nico_op_common_update_pppoe(HINT32 id, NICO_NetDevice_Info_t *deviceinfo)
{
	NICO_PPPoE_Info_t *pppoe_info = &deviceinfo->pppoeinfo;
	(void*)pppoe_info;
}
#endif

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
DI_WIFI_WPS_e	nico_op_common_get_wpstype(eNicoWifiWps wps_type)
{
	DI_WIFI_WPS_e eRet = DI_WIFI_WPS_MAX;
	switch(wps_type)
	{
		case eNICO_WIFI_WPS_NONE:
		case eNICO_WIFI_WPS_UNKNOWN:	eRet = DI_WIFI_WPS_NONE; break;
		case eNICO_WIFI_WPS_MAX:		eRet = DI_WIFI_WPS_MAX;  break;
		case eNICO_WIFI_WPS_PBC:		eRet = DI_WIFI_WPS_PBC;  break;
		case eNICO_WIFI_WPS_PIN:		eRet = DI_WIFI_WPS_PIN;  break;
	}
	return eRet;
}

eNicoWifiSecurity nico_op_common_ap_securetype(DI_WIFI_SECURE_e sec_type)
{
	eNicoWifiSecurity eRet = eNICO_WIFI_SECURITY_UNKNOWN;
	switch(sec_type)
	{
		case DI_WIFI_SECURE_OPEN:		eRet = eNICO_WIFI_SECURITY_OPEN;		break;
		case DI_WIFI_SECURE_WEP:		eRet = eNICO_WIFI_SECURITY_WEP;			break;
		case DI_WIFI_SECURE_WPA:		eRet = eNICO_WIFI_SECURITY_WPA;			break;
		case DI_WIFI_SECURE_WPA2:		eRet = eNICO_WIFI_SECURITY_WPA2;		break;
		case DI_WIFI_SECURE_WPA_WPA2:	eRet = eNICO_WIFI_SECURITY_WPA_WPA2;	break;
		case DI_WIFI_SECURE_UNKNOWN:
		case DI_WIFI_SECURE_MAX:
		default:
			break;
	}
	return eRet;
}
eNicoWifiAuth nico_op_common_ap_authentype(DI_WIFI_AUTHEN_e auth_type)
{
	eNicoWifiAuth eRet = eNICO_WIFI_AUTHEN_UNKNOWN;

	switch(auth_type)
	{
		case DI_WIFI_AUTHEN_OPEN:	 eRet = eNICO_WIFI_AUTH_OPEN;		break;
		case DI_WIFI_AUTHEN_PSK:	 eRet = eNICO_WIFI_AUTHEN_PSK;		break;
		case DI_WIFI_AUTHEN_802_1X:	 eRet = eNICO_WIFI_AUTHEN_802_1X;	break;
		case DI_WIFI_AUTHEN_UNKNOWN: eRet = eNICO_WIFI_AUTHEN_UNKNOWN;	break;
		case DI_WIFI_AUTHEN_MAX:	 eRet = eNICO_WIFI_AUTHEN_MAX;		break;
	}
	return eRet;
}
eNicoWifiEncrypt nico_op_common_ap_encrypttype(DI_WIFI_ENCRYPT_e enc_type)
{
	eNicoWifiEncrypt eRet = eNICO_WIFI_ENCRYPT_UNKNOWN;

	switch(enc_type)
	{
		case DI_WIFI_ENCRYPT_NONE:		 eRet = eNICO_WIFI_ENCRYPT_NONE;			break;
		case DI_WIFI_ENCRYPT_WEP_64: 	 eRet = eNICO_WIFI_ENCRYPT_WEP_64;		break;
		case DI_WIFI_ENCRYPT_WEP_64HEX:	 eRet = eNICO_WIFI_ENCRYPT_WEP_64HEX;	break;
		case DI_WIFI_ENCRYPT_WEP_128:	 eRet = eNICO_WIFI_ENCRYPT_WEP_128;		break;
		case DI_WIFI_ENCRYPT_WEP_128HEX: eRet = eNICO_WIFI_ENCRYPT_WEP_128HEX;	break;
		case DI_WIFI_ENCRYPT_TKIP:	 	 eRet = eNICO_WIFI_ENCRYPT_TKIP;			break;
		case DI_WIFI_ENCRYPT_AES:		 eRet = eNICO_WIFI_ENCRYPT_AES;			break;
		case DI_WIFI_ENCRYPT_TKIP_AES:	 eRet = eNICO_WIFI_ENCRYPT_TKIP_AES;		break;
		case DI_WIFI_ENCRYPT_UNKNOWN:	 eRet = eNICO_WIFI_ENCRYPT_UNKNOWN;		break;
		case DI_WIFI_ENCRYPT_MAX:
		default:						 eRet = eNICO_WIFI_ENCRYPT_MAX;			break;
	}
	return eRet;
}

unsigned long	nico_op_common_make_bcastaddr(HUINT8 *ip, HUINT8 *netmask)
{
	HULONG ullIp, ullMask;

	HxSTD_MemCopy(&ullIp, ip, sizeof(HULONG));
	HxSTD_MemCopy(&ullMask, netmask, sizeof(HULONG));

    return ((ullIp & ullMask) | ~ullMask);
}

HERROR nico_op_common_interface_updown(HINT32 id, HBOOL is_up)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	if(TRUE == is_up)
		hRet = DI_NETWORK_DevUp(id);
	else
		hRet = DI_NETWORK_DevDown(id);
	EXIT;
	return hRet;
}

HERROR nico_op_common_restart_dhcp_connecting(HINT32 id, eNicoNetConfIpVersion ip_ver)
{
	nico_op_common_stop_dhcp_connecting(id, ip_ver);
	nico_op_common_start_dhcp_client(id);
	return ERR_OK;
}

HERROR nico_op_common_start_dhcp_client(HINT32 id)
{
	DI_NETWORK_DHCPC_PARAMS_t	dhcp_setinfo;
	HxSTD_MemSet(&dhcp_setinfo, 0x00, sizeof(DI_NETWORK_DHCPC_PARAMS_t));
	dhcp_setinfo.ulWaitSeconds = NICO_DHCPC_WAIT_TIMEOUT;
	DI_NETWORK_StartDhcpClient(id, DI_NETWORK_IP_VER_4, &dhcp_setinfo);
	HxLOG_Debug("++++ dhcp mode set ip \n");
	return ERR_OK;
}

HERROR nico_op_common_stop_dhcp_connecting(HINT32 id, eNicoNetConfIpVersion ip_ver)
{
	HERROR hRet = ERR_OK;
	ENTRY;
#if !defined(CONFIG_USES_NFSROOT)
	switch(ip_ver)
	{
		case eNICO_VER_IPV4:
			hRet = DI_NETWORK_DeleteDefaultRoute(id, DI_NETWORK_IP_VER_4);
			hRet = DI_NETWORK_StopDhcpClient(id, DI_NETWORK_IP_VER_4);
			break;
		case eNICO_VER_IPV6:
			hRet = DI_NETWORK_DeleteDefaultRoute(id, DI_NETWORK_IP_VER_6);
			hRet = DI_NETWORK_StopDhcpClient(id, DI_NETWORK_IP_VER_6);
			break;
	}

#endif
	EXIT;
	return hRet;
}

HERROR nico_op_common_stop_menual_connecting(HINT32 id, eNicoNetConfIpVersion ip_ver)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	switch(ip_ver)
	{
		case eNICO_VER_IPV4:
			hRet = DI_NETWORK_DeleteDefaultRoute(id, DI_NETWORK_IP_VER_4);
			break;
		case eNICO_VER_IPV6:
			hRet = DI_NETWORK_DeleteDefaultRoute(id, DI_NETWORK_IP_VER_6);
			break;
	}
	EXIT;
	return hRet;
}

HERROR __nico_op_common_set_dns(NICO_NetConf_Info_t* info)
 {
 	HERROR hRet = ERR_OK;
 	HINT8 i = 0;
	struct in_addr ip4dns[eNICO_Dns_Max];
	HxSTD_MemSet(ip4dns, 0x00, sizeof(struct in_addr));

	hRet = DI_NETWORK_SetDnsMode(info->ip_ver, !info->dns_auto);

	if(TRUE == info->dns_auto)
	{

	}
	else
	{
		for(i=0; i<eNICO_Dns_Max; i++)
		{
			HCHAR temp_ip[16] = {0,};
			HxSTD_snprintf(temp_ip, 16, "%d.%d.%d.%d",
									info->ip_info.ipv4.dns[i][0], info->ip_info.ipv4.dns[i][1],
									info->ip_info.ipv4.dns[i][2], info->ip_info.ipv4.dns[i][3]);
			inet_aton(temp_ip, (struct in_addr *)&ip4dns[i]);
		}

		HxLOG_Debug("dns#1:0x%x, dns#2:0x%x, dns#3:0x%x\n",
				ntohl(ip4dns[eNICO_Dns_Primary].s_addr),
				ip4dns[eNICO_Dns_Secondary].s_addr,
				ip4dns[eNICO_Dns_Thirdly].s_addr);
		hRet = DI_NETWORK_SetDnsInfo(eNICO_Dns_Max-1, (HUINT8 *)ip4dns, 0, NULL);

		/*
		 * workaround for invalid resolv.conf (DNS acquisition error)
		 *	 - Performed DNS updating if previous DNS saved in DB has error value
		 *	 - Reference /usr/share/udhcpc/default.script
		 */
		if ((ip4dns[eNICO_Dns_Primary].s_addr == 0x0) &&
			(ip4dns[eNICO_Dns_Secondary].s_addr == 0x0) &&
			(ip4dns[eNICO_Dns_Thirdly].s_addr == 0x0))
		{
			HxLOG_Debug("Error> Changed DNS mode by invalid dns DB\n");
			hRet = DI_NETWORK_SetDnsMode(info->ip_ver, FALSE);
		}
	}
 	return hRet;
 }

HERROR nico_op_common_set_config(HINT32 id, NICO_NetDevice_Info_t *deviceinfo, NICO_NetConf_Info_t* cmdinfo)
{
	HERROR hRet = ERR_OK;
	NICO_NetDevice_Info_t *target_device = deviceinfo;
	NICO_NetConf_IpInfo_t	*ip_info = &target_device->netinfo.ip_info;

	HxSTD_MemCopy(ip_info, &cmdinfo->ip_info, sizeof(NICO_NetConf_IpInfo_t));
	ENTRY;
	HxLOG_Debug("######################################################## \n");
	HxLOG_Debug("######################################################## \n");
	HxLOG_Debug("######################################################## \n");
	HxLOG_Debug("#########        nico_op_common_set_config      ######## \n");
	HxLOG_Debug("######################################################## \n");
	HxLOG_Debug("######################################################## \n");
	HxLOG_Debug("######################################################## \n");

	if(FALSE == target_device->netinfo.linked)
	{
		HxLOG_Debug("this device is not linking \n");
		return ERR_FAIL;

	}
	if(eNICO_VER_IPV4 == cmdinfo->ip_ver)
	{
		__nico_op_common_set_dns(cmdinfo);

		if(TRUE == cmdinfo->dhcp)
		{
			nico_op_common_start_dhcp_client(id);
		}
		else
		{
			HULONG ulBrcast = 0l;
			DI_NETWORK_SET_INFO_t ip_setinfo;
			DI_NETWORK_ROUTE_PARAMS_t stParams;
			NICO_NetConf_IpInfo_t *cmd_ipinfo = &cmdinfo->ip_info;

			HxLOG_Debug("++++ not dhcp mode set ip \n");

			hRet = DI_NETWORK_StopDhcpClient(id, cmdinfo->ip_ver);
			ulBrcast = nico_op_common_make_bcastaddr(cmd_ipinfo->ipv4.ipAddr, cmd_ipinfo->ipv4.netmask);
			HxSTD_MemCopy(cmd_ipinfo->ipv4.broadcast, &ulBrcast, sizeof(struct in_addr));

			HxSTD_MemSet(&ip_setinfo, 0x00, sizeof(DI_NETWORK_SET_INFO_t));
			HxSTD_MemCopy(ip_setinfo.stIpv4Addr.aucIpAddr, cmd_ipinfo->ipv4.ipAddr, sizeof(struct in_addr));
			HxSTD_MemCopy(ip_setinfo.stIpv4Addr.aucMaskAddr, cmd_ipinfo->ipv4.netmask, sizeof(struct in_addr));
			HxSTD_MemCopy(ip_setinfo.stIpv4Addr.aucBcastAddr, cmd_ipinfo->ipv4.broadcast, sizeof(struct in_addr));
			hRet = DI_NETWORK_SetDevInfo(id, &ip_setinfo);

			HxSTD_MemSet( &stParams, 0x00, sizeof(DI_NETWORK_ROUTE_PARAMS_t) );
			HxSTD_MemCopy( stParams.aucRouteAddr, cmd_ipinfo->ipv4.gateway, sizeof(stParams.aucRouteAddr) );
			hRet = DI_NETWORK_SetDefaultRouteExt(id, cmdinfo->ip_ver, &stParams);
		}
	}
	else if(eNICO_VER_IPV6 == cmdinfo->ip_ver)
	{
		if(FALSE == cmdinfo->dns_auto)
		{
			HINT32 i = 0;
			struct in6_addr	ip6_dns[eNICO_Dns_Max];
			NICO_ipv6_t *ipv6 = &cmdinfo->ip_info.ipv6;
			for(i=0;i<eNICO_Dns_Max;i++)
			{
				HCHAR temp_ip[32] = {0,};
				HxSTD_snprintf(temp_ip, 32, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
						ipv6->dns[i][0], ipv6->dns[i][1], ipv6->dns[i][2], ipv6->dns[i][3],
						ipv6->dns[i][4], ipv6->dns[i][5], ipv6->dns[i][6], ipv6->dns[i][7],
						ipv6->dns[i][8], ipv6->dns[i][9], ipv6->dns[i][10], ipv6->dns[i][11],
						ipv6->dns[i][12], ipv6->dns[i][13], ipv6->dns[i][14], ipv6->dns[i][15]);
				inet_pton(AF_INET6, temp_ip, (struct in6_addr *)&ip6_dns[i]);
			}
			hRet = DI_NETWORK_SetDnsInfo(0, NULL, eNICO_Dns_Max-1, (HUINT8*)ip6_dns);
		}
	}

	if(ERR_OK == hRet)
	{
		target_device->netinfo.ip_ver = cmdinfo->ip_ver;
		target_device->netinfo.dns_auto = cmdinfo->dns_auto;
		target_device->netinfo.auto_ip = cmdinfo->auto_ip;
		target_device->netinfo.dhcp = cmdinfo->dhcp;
	}
	EXIT;
	return hRet;
}

void nico_op_common_update_devinfo(HINT32 id, NICO_NetDevice_Info_t *deviceinfo)
{
	DI_NETWORK_GET_INFO_t	di_netinfo;
	HUINT32 dev_count = 0;
	ENTRY;
	HxSTD_MemSet(&di_netinfo, 0x00, sizeof(DI_NETWORK_GET_INFO_t));

	HxLOG_Debug("[%s] Update <%d> device \n", __FUNCTION__, id);

#if defined(CONFIG_PROD_DEVICE_BLUETOOTH)
	DI_NETWORK_GetDevCount(&dev_count);
	if( dev_count == id )
	{
		__nico_op_common_update_bluetooth(id, deviceinfo);
	}
	else
#endif
	{
		//3 #### Updates Network Configuration
		//1 #### di_netinfo 는 무조건 채워져 올라온다. 실패인 경우 default 값으로 채워짐.
		if(DI_ERR_OK == DI_NETWORK_GetDevInfo(id, &di_netinfo))
		{
			nico_debug_print_dinetinfo(id, &di_netinfo);
			__nico_op_common_update(id, deviceinfo, &di_netinfo);

			nico_debug_print_netinfo(&deviceinfo->netinfo);


			switch(di_netinfo.eDevType)
			{
				case DI_NETWORK_DEV_LAN:
					{
						__nico_op_common_update_netinfo(deviceinfo);
					}
					break;
				case DI_NETWORK_DEV_CABLEMODEM:
					{
						__nico_op_common_update_cminfo(deviceinfo);
						__nico_op_common_update_cm_ip(deviceinfo);
					}
					break;
				case DI_NETWORK_DEV_WIFI:
					{
						__nico_op_common_update_apinfo(id, deviceinfo);
					}
					break;
				case DI_NETWORK_DEV_PPPOE:
					{
						__nico_op_common_update_pppoe(id, deviceinfo);
					}
					break;
				default:
					break;
			}
		}
	}

	(void)dev_count;
	EXIT;
}

NICO_NetConf_Info_t *__nico_op_common_find_current_netinfo(HINT32 devid, HINT32 dev_count, NICO_NetDevice_Info_t **deviceinfo)

{
	HINT32 i = 0;
	for( i=0; i<dev_count; i++ )
	{
		if( devid == deviceinfo[i]->id )
		{
			return &deviceinfo[i]->netinfo;
		}
	}
	return NULL;
}

DI_NETWORK_IP_VER_e __nico_op_common_iptype(eNicoNetConfIpVersion type)
{
	DI_NETWORK_IP_VER_e ret = DI_NETWORK_IP_VER_4;

	switch(type)
	{
		case eNICO_VER_IPV4:
			ret = DI_NETWORK_IP_VER_4;
			break;
		case eNICO_VER_IPV6:
			ret = DI_NETWORK_IP_VER_6;
			break;
		default:
			break;
	}

	return ret;
}

STATIC void __nico_op_common_set_route(HINT32 dev_count, NICO_NetDevice_Info_t **deviceinfo)
{
	HINT32 default_route_priority = 0, default_route_devid = 0, i = 0;
	DI_NETWORK_GET_ROUTE_INFO_t route_get_info;
	DI_NETWORK_SET_ROUTE_INFO_t route_set_info;

	if( NULL == deviceinfo ) return;

	// priority 는 1000보다 작게 설정 ( xml 파일에서 )
	default_route_priority = 1000;

	for( i=0; i<dev_count; i++ )
	{
		DI_NETWORK_DeleteDefaultRoute(deviceinfo[i]->id, deviceinfo[i]->netinfo.ip_ver);
		if(deviceinfo[i]->netinfo.linked && deviceinfo[i]->netinfo.active)
		{
			HxSTD_MemSet(&route_get_info, 0x00, sizeof(DI_NETWORK_GET_ROUTE_INFO_t));
			HxSTD_MemSet(&route_set_info, 0x00, sizeof(DI_NETWORK_SET_ROUTE_INFO_t));

			if( DI_ERR_OK == DI_NETWORK_GetRouteInfo(deviceinfo[i]->netinfo.id,deviceinfo[i]->netinfo.ip_ver, &route_get_info) )
			{
				route_set_info.eRouteMode = DI_NETWORK_DELETE_ROUTE;
				HxSTD_MemCopy(route_set_info.cIfName, route_get_info.cIfName, IF_NAME_SIZE);
				HxSTD_MemCopy(route_set_info.ucDestination, route_get_info.ucDestination, IPV4_ADDR_LEN);
				HxSTD_MemCopy(route_set_info.ucGateway, route_get_info.ucGateway, IPV4_ADDR_LEN);
				HxSTD_MemCopy(route_set_info.ucMaskAddr, route_get_info.ucMaskAddr, IPV4_ADDR_LEN);
				route_set_info.ulFlags = route_get_info.ulFlags;
				route_set_info.ulMetric = route_get_info.ulMetric;
				route_set_info.ulMtu = route_get_info.ulMtu;
				route_set_info.ulWindow = route_get_info.ulWindow;
				route_set_info.ulIrtt = route_get_info.ulIrtt;
				DI_NETWORK_SetRouteInfo(deviceinfo[i]->netinfo.id, deviceinfo[i]->netinfo.ip_ver, &route_set_info);

				route_set_info.eRouteMode = DI_NETWORK_ADD_ROUTE;
				route_set_info.ulMetric = deviceinfo[i]->netinfo.priority;
				DI_NETWORK_SetRouteInfo(deviceinfo[i]->netinfo.id, deviceinfo[i]->netinfo.ip_ver, &route_set_info);
			}

			if(default_route_priority > deviceinfo[i]->netinfo.priority)
			{
				default_route_priority = deviceinfo[i]->netinfo.priority;
				default_route_devid = deviceinfo[i]->netinfo.id;
			}
		}
	}

	if((default_route_devid >= 0) && (default_route_devid < 1000))
	{
		if(eNICO_VER_IPV4 == deviceinfo[default_route_devid]->netinfo.ip_ver)
			DI_NETWORK_SetDefaultRoute(default_route_devid,
								deviceinfo[default_route_devid]->netinfo.ip_ver,
								deviceinfo[default_route_devid]->netinfo.ip_info.ipv4.gateway);
		else
			DI_NETWORK_SetDefaultRoute(default_route_devid,
								deviceinfo[default_route_devid]->netinfo.ip_ver,
								deviceinfo[default_route_devid]->netinfo.ip_info.ipv6.gateway);
	}

	return;
}

/* TODO below routine has to move into nima */
HERROR nico_op_common_set_routetable(pevent_msg event, HINT32 dev_count,
													NICO_NetDevice_Info_t **deviceinfo, nico_dievtmsg_t *di_event)
{
	HERROR hRet = ERR_OK;

	if(!event || !di_event) return ERR_FAIL;

	switch(event->type.evt_type)
	{
#if !defined(CONFIG_USES_NFSROOT)
		case EVENT_NET_CONNECT_FAIL:
		case EVENT_NET_PLUGGED_OUT:
		case EVENT_NET_WIFI_USB_EXTRACTED:
		case EVENT_NET_WIFI_AP_CONNECT_FAIL:
			{
				DI_WIFI_CALLBACK_DATA_t *data = di_event->data;
				NICO_NetConf_Info_t *netinfo = NULL;
				HCHAR buf[128]	= { 0 };

				netinfo = __nico_op_common_find_current_netinfo(data->ulDevId, dev_count, deviceinfo);
				if( NULL != netinfo )
				{
					DI_NETWORK_DeleteDefaultRoute(data->ulDevId, __nico_op_common_iptype(netinfo->ip_ver));
					DI_NETWORK_StopDhcpClient(data->ulDevId,  __nico_op_common_iptype(netinfo->ip_ver));
					HxSTD_PrintToStrN( buf, sizeof(buf), "ifconfig %s 0.0.0.0", deviceinfo[netinfo->id]->name);
					VK_SYSTEM_Command(buf);

					deviceinfo[netinfo->id]->netinfo.active = FALSE;

					__nico_op_common_set_route(dev_count, deviceinfo);
				}
			}
			break;
		case EVENT_NET_CONNECT_SUCCESS:
			{
				__nico_op_common_set_route(dev_count, deviceinfo);
			}
			break;
#endif
		default:
			break;
	}

#if 0 //defined(CONFIG_MW_INET_PPPOE) // sgchoi
	__nico_op_common_set_rt_pppoe(event, dev_count, deviceinfo, di_event);
#endif

	return hRet;
}

HERROR nico_op_common_set_autoip(HINT32 dev_id, nico_dievtmsg_t *di_event, NICO_NetDevice_Info_t *deviceinfo)
{
    HERROR hRet = ERR_FAIL;
	if( !di_event ) return ERR_FAIL;

	if(EVT_NETCONF == di_event->type)
	{
		DI_NETWORK_CALLBACK_DATA_t *data = di_event->data;
		switch(data->eEvent)
		{
		case DI_NETWORK_EVENT_DHCP6_FAIL:
		{
			if(deviceinfo->netinfo.ip_info.ipv6.ipAddr[0] != 254 || deviceinfo->netinfo.ip_info.ipv6.ipAddr[1]>>6 != 2)
			{
				DI_NETWORK_StartAutoIp(dev_id);
			}
			hRet = ERR_OK;
		}
		break;
		case DI_NETWORK_EVENT_DHCP_FAIL:
		{
			// check whether current ip is auto-ip. auto-ip must be 169.254.xxx.xxx
			if (deviceinfo->netinfo.ip_info.ipv4.ipAddr[0] != 169 || deviceinfo->netinfo.ip_info.ipv4.ipAddr[1] != 254)
			{
				DI_NETWORK_StartAutoIp(dev_id);
			}
			hRet = ERR_OK;
		}
		break;
		case DI_NETWORK_EVENT_AUTOIP_SUCCESS:
		{
			HCHAR szCmd[512] = {0,};
			HCHAR temp_ip[16] = {0,};
			HxSTD_snprintf(temp_ip, 16, "%d.%d.%d.%d",
						   deviceinfo->netinfo.ip_info.ipv4.ipAddr[0], deviceinfo->netinfo.ip_info.ipv4.ipAddr[1],
						   deviceinfo->netinfo.ip_info.ipv4.ipAddr[2], deviceinfo->netinfo.ip_info.ipv4.ipAddr[3]);
			HxSTD_snprintf(szCmd, 512, "ifconfig eth0 %s", temp_ip);
			VK_SYSTEM_Command("ifconfig eth0 0.0.0.0");
			VK_SYSTEM_Command(szCmd);
		}
		default:
			break;
		}
	}

    return hRet;
}

#if defined(CONFIG_PROD_DEVICE_BLUETOOTH)
HERROR nico_op_common_get_bluetooth_host(HINT32 dev_id, nico_dievtmsg_t *di_event, event_msg *event)
{
	DI_BT_CALLBACK_DATA_t *data = di_event->data;

	DI_BT_DEV_INFO_t host_info;
	HxSTD_MemSet(&host_info, 0x00, sizeof(DI_BT_DEV_INFO_t));

	if( di_event->type != EVT_BT )
		return ERR_OK;

	switch(data->eEvent)
	{
		case DI_BT_NOTIFY_EVT_INSERTED:
		{
			NICO_BT_DevInfo_t *bt_info = NULL;
			DI_BT_GetHostInfo(&host_info);
			bt_info = &event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.host_info;
			HxSTD_MemCopy(bt_info->name, host_info.ucName, NICO_BT_NAME_MAX);
			HxSTD_MemCopy(bt_info->bdaddr, host_info.ucBdAddr, NICO_BT_BDADDR_LEN);
			HxSTD_MemCopy(bt_info->dev_class, host_info.ucClass, NICO_BT_CLASS_LEN);
			bt_info->status = host_info.etDevStatus;
			bt_info->available_profile = host_info.etAvailableProfile;
		}
		break;
		default: break;
	}

	return ERR_OK;
}

HERROR nico_op_common_get_bluetooth_devicelist(NICO_BT_Info_t *bt_info)
{
	HINT32 i = 0;
	DI_BT_DEV_LIST_t di_devlist;

	if( NULL == bt_info ) return ERR_FAIL;

	if( DI_ERR_OK == DI_BT_GetDeviceList(&di_devlist) )
	{
		bt_info->count = di_devlist.ulNumOfData;
		for( i=0; i<bt_info->count; i++ )
		{
			HxSTD_MemCopy(bt_info->dev_list[i].name, di_devlist.stBtDevList[i].ucName, NICO_BT_NAME_MAX);
			HxSTD_MemCopy(bt_info->dev_list[i].bdaddr, di_devlist.stBtDevList[i].ucBdAddr, NICO_BT_BDADDR_LEN);
			HxSTD_MemCopy(bt_info->dev_list[i].dev_class, di_devlist.stBtDevList[i].ucClass, NICO_BT_CLASS_LEN);
			bt_info->dev_list[i].status = di_devlist.stBtDevList[i].etDevStatus;
			bt_info->dev_list[i].available_profile = di_devlist.stBtDevList[i].etAvailableProfile;
		}
	}

	return ERR_OK;
}

#else
HERROR nico_op_common_get_bluetooth_host(HINT32 dev_id, nico_dievtmsg_t *di_event, event_msg *event)
{
	return ERR_OK;
}
HERROR nico_op_common_get_bluetooth_devicelist(NICO_BT_Info_t *bt_info)
{
	return ERR_OK;
}
#endif
