/** **********************************************************************************************************
	File 		: nima_session.c
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

#include "nima_db.h"
#include "nima_util.h"
#include "nima_session.h"
#include "nima_protocol.h"
#include "nima_conv.h"
#include "nima_command_procedure.h"
#include "ncapi.h"
#include "nima_scenario.h"


#include <vkernel.h>
#include <hlib.h>
#include <stdio.h>
#include <unistd.h>

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
/* TODO::Should move "nima_macro.h" */
#define NIMA_SAFE_DELETE(x) \
			do{ \
				if(NULL != x){	\
					HLIB_STD_MemFree(x); \
					x = NULL; \
				} \
			}while(0)


/*-----------------------------------------------------------------------------------------------
 *	internal functions
 *----------------------------------------------------------------------------------------------*/
static void __nima_do_command_connect(const HCHAR *data)
{
	FuncEnter;
	/*empty*/
	FuncLeave;
}

static void __nima_do_command_disconnect(const HCHAR *data)
{
	HINT32 dev_id = 0;
	FuncEnter;
	HxSTD_MemCopy(&dev_id, data, sizeof(HINT32));
	NCAPI_NetConf_StopConnect(dev_id);
	FuncLeave;
}

static void __nima_do_command_get_netinfo(const HCHAR *data)
{
	FuncEnter;
	/*empty*/
	FuncLeave;
}

static void __nima_do_command_set_netinfo(HCHAR *data)
{
	NIMAProtocol_Netconf_t *netInfo = NULL;
	NCAPI_NetConfInfo_t *info = NULL;

	FuncEnter;

	netInfo = (NIMAProtocol_Netconf_t *)data;
	info = HLIB_STD_MemAlloc(sizeof(NCAPI_NetConfInfo_t));
	HxSTD_MemSet(info, 0x00, sizeof(NCAPI_NetConfInfo_t));
	info->id = netInfo->id;
	info->priority = netInfo->priority;
	HxSTD_MemCopy(info->name, netInfo->name, NIMA_PROTO_MAX_INTERFACE_NAME);
	info->type = netInfo->type;
	info->linked = netInfo->linked;
	info->active = netInfo->active;
	info->dhcp = netInfo->dhcp;
	info->dns_auto = netInfo->dns_auto;
	info->auto_ip = netInfo->auto_ip;
	info->ip_ver = netInfo->ip_ver;

	HxLOG_Debug("id : %d (netinfo->id: %d)\n", info->id, netInfo->id);

	switch(info->ip_ver)
	{
		case eNCAPI_NETCONF_IPV4:
			HxSTD_MemCopy(info->ip_info.ipv4.macaddr, netInfo->ip_info.ipv4.macaddr, NIMA_PROTO_MAX_MACADDR);
			HxSTD_MemCopy(info->ip_info.ipv4.ipAddr, netInfo->ip_info.ipv4.ipAddr, NIMA_PROTO_MAX_IP4ADDR);
			HxSTD_MemCopy(info->ip_info.ipv4.netmask, netInfo->ip_info.ipv4.netmask, NIMA_PROTO_MAX_IP4ADDR);
			HxSTD_MemCopy(info->ip_info.ipv4.gateway, netInfo->ip_info.ipv4.gateway, NIMA_PROTO_MAX_IP4ADDR);
			HxSTD_MemCopy(info->ip_info.ipv4.dns[0], netInfo->ip_info.ipv4.dns1, NIMA_PROTO_MAX_IP4ADDR);
			HxSTD_MemCopy(info->ip_info.ipv4.dns[1], netInfo->ip_info.ipv4.dns2, NIMA_PROTO_MAX_IP4ADDR);
			break;
		case eNCAPI_NETCONF_IPV6:
			HxSTD_MemCopy(info->ip_info.ipv6.macaddr, netInfo->ip_info.ipv6.macaddr, NIMA_PROTO_MAX_MACADDR);
			HxSTD_MemCopy(info->ip_info.ipv6.ipAddr, netInfo->ip_info.ipv6.ipAddr, NIMA_PROTO_MAX_IP6ADDR);
			HxSTD_MemCopy(info->ip_info.ipv6.gateway, netInfo->ip_info.ipv6.gateway, NIMA_PROTO_MAX_IP6ADDR);
			HxSTD_MemCopy(info->ip_info.ipv6.dns[0], netInfo->ip_info.ipv6.dns1, NIMA_PROTO_MAX_IP6ADDR);
			HxSTD_MemCopy(info->ip_info.ipv6.dns[1], netInfo->ip_info.ipv6.dns2, NIMA_PROTO_MAX_IP6ADDR);
			break;
	}

	NCAPI_NetConf_SetInfo(info->id, info);
	NIMA_SAFE_DELETE(info);
	FuncLeave;
}



static void __nima_do_command_set_autoip(const HCHAR *data)
{

}

static void __nima_do_command_set_dynamic_ip(const HCHAR *data)
{

}

static void __nima_do_command_set_static_ip(const HCHAR *data)
{

}

static void __nima_do_command_shutdown_eth(const HCHAR *data)
{
	/*empty*/
}

static void __nima_do_command_dev_up(HCHAR *data)
{
	HINT32 dev_id = 0;

	FuncEnter;
	if(!data) return;
	HxSTD_MemCopy(&dev_id, data, sizeof(HINT32));
	NCAPI_NetConf_DevUp(dev_id);
	FuncLeave;
}

static void __nima_do_command_dev_down(HCHAR *data)
{
	HINT32 dev_id = 0;
	FuncEnter;
	if( !data) return;
	HxSTD_MemCopy(&dev_id, data, sizeof(HINT32));
	NCAPI_NetConf_DevDown(dev_id);
	FuncLeave;
}

static void __nima_do_command_dev_priority(HCHAR *data)
{
	NIMAProtocol_priority_t *info = NULL;
	FuncEnter;
	if( !data) return;
	info = (NIMAProtocol_priority_t *)data;
	NCAPI_NetConf_SetDevPriority(info->id, info->priority);
	FuncLeave;
}

static void __nima_do_command_wifi_scan_start(const HCHAR *data)
{
	HINT32 dev_id = 0;
	FuncEnter;
	if( !data ) return;
	HxSTD_MemCopy(&dev_id, data, sizeof(HINT32));
	NCAPI_Wifi_ScanStart(dev_id);
	FuncLeave;
}

static void __nima_do_command_wifi_scan_stop(const HCHAR *data)
{
	HINT32 dev_id = 0;

	FuncEnter;
	if( !data ) return;
	HxSTD_MemCopy(&dev_id, data, sizeof(HINT32));
	NCAPI_Wifi_ScanStop(dev_id);
	FuncLeave;
}

static void __nima_do_command_wifi_ap_connect_t1(const HINT32 dev_id, const HCHAR *data)
{
	NCAPI_WifiAPInfo_t *info = NULL;
	NIMAProtocol_APInfo_t *tmp = (NIMAProtocol_APInfo_t *)data;

	if( !tmp ) return;

	info = HLIB_STD_MemAlloc(sizeof(NCAPI_WifiAPInfo_t));
	HxSTD_MemSet(info, 0x00, sizeof(NCAPI_WifiAPInfo_t));

	nima_conv_ncapi_apinfo(tmp, info);

	NCAPI_Wifi_Connect(dev_id, info);
	NIMA_SAFE_DELETE(info);

}

static void __nima_do_command_wifi_ap_connect_t2(const HCHAR *data)
{
	NIMAProtocol_Wifi_t *wifi_info = (NIMAProtocol_Wifi_t*)data;
	NIMAProtocol_APInfo_t *tmp = NULL;
	NCAPI_WifiAPInfo_t *info = NULL;

	tmp = (NIMAProtocol_APInfo_t *)wifi_info->ap;
	if( !tmp ) return;

	info = HLIB_STD_MemAlloc(sizeof(NCAPI_WifiAPInfo_t));
	HxSTD_MemSet(info, 0x00, sizeof(NCAPI_WifiAPInfo_t));

	nima_conv_ncapi_apinfo(tmp, info);

	if(NIMA_NETWORK_CONNECTING == nima_scenario_get_net_status())
	{
		NCAPI_Wifi_Disconnect(wifi_info->dev_id);
	}

	nima_scenario_set_net_status(NIMA_NETWORK_CONNECTING);
	NCAPI_Wifi_Connect(wifi_info->dev_id, info);
	NIMA_SAFE_DELETE(info);
}

static void __nima_do_command_wifi_wps_connect(const HCHAR *data)
{
	NIMAProtocol_WPS_t *wps_info = (NIMAProtocol_WPS_t*)data;
	NCAPI_WifiWPSInfo_t *info = NULL;
	info = HLIB_STD_MemAlloc(sizeof(NCAPI_WifiWPSInfo_t));

	HxSTD_MemSet(info, 0x00, sizeof(NCAPI_WifiWPSInfo_t));

	switch(wps_info->wps_type)
	{
		case eNIMA_WIFI_WPS_PIN:
			info->eType = eNCAPI_WIFI_WPS_PIN;
			break;
		case eNIMA_WIFI_WPS_PBC:
			info->eType = eNCAPI_WIFI_WPS_PBC;
			break;
		default :
			info->eType = eNCAPI_WIFI_WPS_UNKNOWN;
			break;
	}

	HxSTD_MemCopy(info->szCode, wps_info->pin_code, NIMA_PROTO_WPS_PINCODE);

	if(NIMA_NETWORK_CONNECTING == nima_scenario_get_net_status())
	{
		NCAPI_Wifi_Disconnect(wps_info->dev_id);
	}

	nima_scenario_set_net_status(NIMA_NETWORK_CONNECTING);

	NCAPI_Wifi_Connect_byWPS(wps_info->dev_id, info);
	NIMA_SAFE_DELETE(info);
}

static void __nima_do_command_wifi_ap_disconnect(const HCHAR *data)
{
	HINT32 dev_id = 0;

	FuncEnter;
	if( !data ) return;
	HxSTD_MemCopy(&dev_id, data, sizeof(HINT32));
	NCAPI_Wifi_Disconnect(dev_id);
	FuncLeave;
}

static void __nima_do_command_dhcpserver_start(void)
{
	FuncEnter;
	NCAPI_NetConf_StartDHCPServer(0,0);
	FuncLeave;
}

static void __nima_do_command_dhcpserver_stop(void)
{
	FuncEnter;
	NCAPI_NetConf_StopDHCPServer(0,0);
	FuncLeave;
}

static void __nima_do_command_cm_set_downstream(const HCHAR *data)
{
	NCAPI_CMDownStream_t dsInfo;

	FuncEnter;
	HxSTD_MemCopy(&dsInfo, data, sizeof(dsInfo));
	NCAPI_CM_SetDownStreamInfo(&dsInfo);
	FuncLeave;
}

static void __nima_do_command_set_ntp_clent(const HCHAR *data)
{
	NIMAProtocol_cmd_ntp_t *cmd_info = NULL;
	HCHAR	ipAddr[NIMA_PROTO_MAX_ADDR];

	FuncEnter;
	if( !data) return;
	cmd_info = (NIMAProtocol_cmd_ntp_t *)data;
	HxSTD_MemCopy(ipAddr, cmd_info->ipAddr, NIMA_PROTO_MAX_ADDR);
	NCAPI_Netconf_SetNTPClient(ipAddr);
	FuncLeave;
}

static void __nima_do_command_bt_enable(const HCHAR *data)
{
	NIMAProtocol_cmd_bt_t *bt_cmd_info = NULL;

	FuncEnter;
	if( !data) return;
	bt_cmd_info = (NIMAProtocol_cmd_bt_t *)data;
	NCAPI_BT_SetEnable(bt_cmd_info->enable);
	FuncLeave;
}
static void __nima_do_command_bt_scan(const HCHAR *data)
{
	NIMAProtocol_cmd_bt_t *bt_cmd_info = NULL;

	FuncEnter;
	if( !data ) return;
	bt_cmd_info = (NIMAProtocol_cmd_bt_t *)data;
	NCAPI_BT_StartScan(bt_cmd_info->profile);
	FuncLeave;
}
static void __nima_do_command_bt_send_pincode(const HCHAR *data)
{
	NIMAProtocol_cmd_bt_t *bt_cmd_info = NULL;

	FuncEnter;
	if( !data ) return;
	bt_cmd_info = (NIMAProtocol_cmd_bt_t *)data;
	NCAPI_BT_SendPinCode(bt_cmd_info->bdaddr, bt_cmd_info->pincode);
	FuncLeave;
}
static void __nima_do_command_bt_connect(const HCHAR *data)
{
	NIMAProtocol_cmd_bt_t *bt_cmd_info = NULL;

	FuncEnter;
	if( !data ) return;
	bt_cmd_info = (NIMAProtocol_cmd_bt_t *)data;
	NCAPI_BT_Connect(bt_cmd_info->bdaddr);
	FuncLeave;
}
static void __nima_do_command_bt_disconnect(const HCHAR *data)
{
	NIMAProtocol_cmd_bt_t *bt_cmd_info = NULL;

	FuncEnter;
	if( !data ) return;
	bt_cmd_info = (NIMAProtocol_cmd_bt_t *)data;
	NCAPI_BT_Disconnect(bt_cmd_info->bdaddr);
	FuncLeave;
}
static void __nima_do_command_bt_remove(const HCHAR *data)
{
	NIMAProtocol_cmd_bt_t *bt_cmd_info = NULL;

	FuncEnter;
	if( !data ) return;
	bt_cmd_info = (NIMAProtocol_cmd_bt_t *)data;
	NCAPI_BT_Remove(bt_cmd_info->bdaddr);
	FuncLeave;
}
static void __nima_do_command_bt_openprofile(const HCHAR *data)
{
	NIMAProtocol_cmd_bt_t *bt_cmd_info = NULL;

	FuncEnter;
	if( !data ) return;
	bt_cmd_info = (NIMAProtocol_cmd_bt_t *)data;
	NCAPI_BT_OpenProfile(bt_cmd_info->bdaddr, bt_cmd_info->profile);
	FuncLeave;
}

static void __nima_do_command_bt_closeprofile(const HCHAR *data)
{
	NIMAProtocol_cmd_bt_t *bt_cmd_info = NULL;

	FuncEnter;
	if( !data ) return;
	bt_cmd_info = (NIMAProtocol_cmd_bt_t *)data;
	NCAPI_BT_CloseProfile(bt_cmd_info->bdaddr, bt_cmd_info->profile);
	FuncLeave;
}

static void __nima_do_command_bt_devlist_update(const HCHAR *data)
{
	FuncEnter;
	/*empty*/
	FuncLeave;

}

static void __nima_do_command_pppoe_connect(const HCHAR *data)
{
	NIMAProtocol_PPPOE_info_t *pppoe_info = NULL;
	NCAPI_PPPoEInfo_t *info = NULL;

	FuncEnter;
	if( !data ) return;
	pppoe_info = (NIMAProtocol_PPPOE_info_t *)data;
	info = HLIB_STD_MemAlloc(sizeof(NCAPI_PPPoEInfo_t));
	if( !info) return;

	HxSTD_MemSet(info, 0x00, sizeof(NCAPI_PPPoEInfo_t));

	nima_conv_ncapi_pppoe(pppoe_info, info);

	NCAPI_PPPoE_Connect(info->id, info);
	HLIB_STD_MemFree(info);
	FuncLeave;
}
static void __nima_do_command_pppoe_disconnect(const HCHAR *data)
{
	HINT32 dev_id = 0;
	FuncEnter;
	if( !data ) return;
	HxSTD_MemCopy(&dev_id, data, sizeof(HINT32));
	NCAPI_PPPoE_Disconnect(dev_id);
	FuncLeave;
}

static void __nima_do_command_pppoe_setinfo(HCHAR *data)
{
	NIMAProtocol_PPPOE_info_t *pppoe_info = NULL;
	NCAPI_PPPoEInfo_t *info = NULL;

	FuncEnter;
	if( !data ) return;
	pppoe_info = (NIMAProtocol_PPPOE_info_t *)data;
	info = HLIB_STD_MemAlloc(sizeof(NCAPI_PPPoEInfo_t));
	if( !info ) return;

	HxSTD_MemSet(info, 0x00, sizeof(NCAPI_PPPoEInfo_t));

	nima_conv_ncapi_pppoe(pppoe_info, info);

	HxLOG_Debug("\033[01;36m[%s:%s:%d] id : %s, pw : %s  \033[00m\n",
					__FILE__, __FUNCTION__, __LINE__,
					info->connect_id, info->connect_pw);
	NCAPI_PPPoE_SetInfo(info->id, info);

	if(info)
		HLIB_STD_MemFree(info);

	FuncLeave;
}

static void __nima_do_command_pppoe_setonoff(HCHAR *data)
{
	HINT32 devId = 0;
	HBOOL bOn = TRUE;

	FuncEnter;

	if( data )
	{
		devId = (HINT32) *data;
		bOn = (HBOOL) *(data+sizeof(HINT32));
		NCAPI_PPPoE_SetOnOff(devId, bOn);
	}

	FuncLeave;
}




/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
// return means if it needs event, or not.
void nima_do_command_shutdown(NIMAProtocol_devinfo_t *devinfo)
{
	HINT32 i = 0, device_id = 0;

	if( devinfo )
	{
		NIMAProtocol_cmd_bt_t cmd = { .enable = FALSE,};
		__nima_do_command_bt_enable((HCHAR *)&cmd);

		for(i=0; i<devinfo->dev_count; i++)
		{
			if( devinfo->netconf[i].active && devinfo->netconf[i].linked )
			{
				__nima_do_command_disconnect((HCHAR *)&devinfo->netconf[i].id);

				if( eNIMA_NETCONF_WIFI == devinfo->netconf[i].type )
					__nima_do_command_wifi_ap_disconnect((HCHAR *)&device_id);
			}
		}
	}
	return;
}

nimapacket_t *nima_do_encode_packet(eNimaProtoType type, eNimaEvent code, HINT32 dlen, void *data)
{
	nimapacket_t *packet = HLIB_STD_MemAlloc(sizeof(nimapacket_t)+dlen);
	if(packet)
	{
		packet->type = type;
		packet->code = code;
		packet->dlen = dlen;
		packet->sync = FALSE;
		if(NULL != data) HxSTD_MemCopy(packet->data, data, packet->dlen);
	}
	return packet;
}

/**
* This function processes all asynchonous function calls
* @param[in] packet: pointer to data packet
* @return void
*/

void nima_do_command_async(nimapacket_t *packet)
{
	FuncEnter;
	switch ( packet->code )
	{
		case NIMA_PROTO_CONNECT: 			__nima_do_command_connect(packet->data); 			break;
		case NIMA_PROTO_DISCONNECT:			__nima_do_command_disconnect(packet->data);			break;
		case NIMA_PROTO_GET_NET_INFO:		__nima_do_command_get_netinfo(packet->data);		break;
		case NIMA_PROTO_SET_NET_INFO:		__nima_do_command_set_netinfo(packet->data);		break;
		case NIMA_PROTO_SET_DYNAMIC_IP:		__nima_do_command_set_dynamic_ip(packet->data);		break;
		case NIMA_PROTO_SET_STATIC_IP:		__nima_do_command_set_static_ip(packet->data);		break;
		case NIMA_PROTO_SET_AUTO_IP:		__nima_do_command_set_autoip(packet->data);			break;
		case NIMA_PROTO_SHUTDOWN_ETH:		__nima_do_command_shutdown_eth(packet->data);		break;
		case NIMA_PROTO_DEV_UP:				__nima_do_command_dev_up(packet->data);				break;
		case NIMA_PROTO_DEV_DOWN:			__nima_do_command_dev_down(packet->data);			break;
		case NIMA_PROTO_SET_DEV_PRIORITY:	__nima_do_command_dev_priority(packet->data);		break;
		case NIMA_PROTO_SET_NTP_CLIENT:		__nima_do_command_set_ntp_clent(packet->data);		break;
		case NIMA_PROTO_WIFI_SCAN_START:	__nima_do_command_wifi_scan_start(packet->data);	break;
		case NIMA_PROTO_WIFI_SCAN_STOP:		__nima_do_command_wifi_scan_stop(packet->data);		break;
		case NIMA_PROTO_WIFI_CONNECT_AP:	__nima_do_command_wifi_ap_connect_t2(packet->data);	break;
		case NIMA_PROTO_WIFI_CONNECT_WPS:	__nima_do_command_wifi_wps_connect(packet->data);	break;
		case NIMA_PROTO_WIFI_DISCONNECT_AP:	__nima_do_command_wifi_ap_disconnect(packet->data);	break;
		case NIMA_PROTO_DHCPSERVER_START:	__nima_do_command_dhcpserver_start();				break;
		case NIMA_PROTO_DHCPSERVER_STOP:	__nima_do_command_dhcpserver_stop();				break;
		case NIMA_PROTO_SET_CM_DOWNSTREAM:	__nima_do_command_cm_set_downstream(packet->data);	break;
		case NIMA_PROTO_BT_ENABLE:			__nima_do_command_bt_enable(packet->data);			break;
		case NIMA_PROTO_BT_SCAN:			__nima_do_command_bt_scan(packet->data);			break;
		case NIMA_PROTO_BT_SEND_PINCODE:	__nima_do_command_bt_send_pincode(packet->data);	break;
		case NIMA_PROTO_BT_CONNECT:			__nima_do_command_bt_connect(packet->data);			break;
		case NIMA_PROTO_BT_DISCONNECT:		__nima_do_command_bt_disconnect(packet->data);		break;
		case NIMA_PROTO_BT_REMOVE:			__nima_do_command_bt_remove(packet->data); 			break;
		case NIMA_PROTO_BT_OPEN_PROFILE:	__nima_do_command_bt_openprofile(packet->data); 	break;
		case NIMA_PROTO_BT_CLOSE_PROFILE:	__nima_do_command_bt_closeprofile(packet->data);	break;
		case NIMA_PROTO_BT_DEVLIST_UPDATE:	__nima_do_command_bt_devlist_update(packet->data);	break;
		case NIMA_PROTO_PPPOE_CONNECT:		__nima_do_command_pppoe_connect(packet->data);		break;
		case NIMA_PROTO_PPPOE_DISCONNECT:	__nima_do_command_pppoe_disconnect(packet->data);	break;
		case NIMA_PROTO_PPPOE_SET_INFO:		__nima_do_command_pppoe_setinfo(packet->data);		break;
		case NIMA_PROTO_PPPOE_SET_ONOFF:	__nima_do_command_pppoe_setonoff(packet->data);		break;
		default:
			break;
	}
	FuncLeave;
}

nimapacket_t * __nima_do_command_set_wakeup_on_wlan(const HCHAR *szData)
{

	HERROR eRet =  ERR_FAIL;
	nimapacket_t * pstRetPacket = NULL;
	NIMAProtocol_WoWLAN_t * pstWowlanInfo = NULL;

	FuncEnter;
	if(!szData)
	{
		return NULL;
	}

	pstRetPacket = HLIB_STD_MemAlloc(sizeof(nimapacket_t)+sizeof(HERROR));
	pstWowlanInfo = (NIMAProtocol_WoWLAN_t * ) szData;

	eRet = NCAPI_Wifi_SetWakeupOnWlan(pstWowlanInfo->dev_id, pstWowlanInfo->mode);
	pstRetPacket->dlen = sizeof(HERROR);
	HxSTD_MemCopy(pstRetPacket->data, &eRet, pstRetPacket->dlen);

	FuncLeave;

	return pstRetPacket;
}

nimapacket_t * __nima_do_command_get_pincode_by_wps(const HCHAR *szData)
{
	HERROR eRet =  ERR_FAIL;
	nimapacket_t * pstRetPacket = NULL;
	NIMAProtocol_WPS_t * pstWpsInfo = NULL;

	FuncEnter;
	if(!szData)
	{
		return NULL;
	}

	pstWpsInfo = (NIMAProtocol_WPS_t * ) szData;

	pstRetPacket = HLIB_STD_MemAlloc(sizeof(nimapacket_t)+sizeof(pstWpsInfo->pin_code));

	eRet = NCAPI_Wifi_GetPinCodebyWPS(pstWpsInfo->dev_id, pstWpsInfo->pin_code);
	if(eRet == ERR_FAIL)
		HxLOG_Debug("NCAPI_Wifi_GetPinCodebyWPS is Fail\n");

	pstRetPacket->dlen = sizeof(pstWpsInfo->pin_code);
	HxSTD_MemCopy(pstRetPacket->data, pstWpsInfo->pin_code, pstRetPacket->dlen);

	FuncLeave;

	return pstRetPacket;
}


/**
* This function processes all synchonous function calls
* @param[in] pstPacket: pointer to data packet
* @return a pointer to the result packet, NULL if fail
*/

nimapacket_t * nima_do_command_sync(nimapacket_t *pstPacket)
{
	nimapacket_t * pstRetPacket = NULL;
	FuncEnter;
	switch ( pstPacket->code )
	{
		case NIMA_PROTO_WIFI_SET_WAKEUP_ON_WLAN:
			pstRetPacket = __nima_do_command_set_wakeup_on_wlan(pstPacket->data);
			break;
		case NIMA_PROTO_WIFI_GET_WPS_PINCODE:
			pstRetPacket = __nima_do_command_get_pincode_by_wps(pstPacket->data);
			break;
		default:
			break;
	}

	if(NULL != pstRetPacket)
	{
		pstRetPacket->code = pstPacket->code;
		pstRetPacket->sync = TRUE;
		pstRetPacket->type = NIMA_PROTOTYPE_REPLY;
	}
	FuncLeave;
	return pstRetPacket;
}


