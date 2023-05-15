/** **********************************************************************************************************
	File 		: nima_conv.h
	author 		:
	comment		:
	date    	: 2013/12/19
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

#include "htype.h"
#include "nima_conv.h"

eNimaEvent nima_conv_event_netconf(eNcapiNetConfEvent event)
{
	switch( event )
	{
		case eNCAPI_NETCONF_EVENT_CONN_SUCCESS:
			return NIMA_EVENT_NETCONF_CONN_SUCCESS;
		case eNCAPI_NETCONF_EVENT_CONN_FAIL:
			return NIMA_EVENT_NETCONF_CONN_FAIL;
		case eNCAPI_NETCONF_EVENT_AUTOIP_SUCCESS:
			return NIMA_EVENT_NETCONF_AUTOIP_SUCCESS;
		case eNCAPI_NETCONF_EVENT_AUTOIP_FAIL:
			return NIMA_EVENT_NETCONF_AUTOIP_FAIL;
		case eNCAPI_NETCONF_EVENT_LINK_CONNECTED:
			return NIMA_EVENT_NETCONF_LINK_CONNECTED;
		case eNCAPI_NETCONF_EVENT_LINK_DISCONNECTED:
			return NIMA_EVENT_NETCONF_LINK_DISCONNECTED;
		case eNCAPI_NETCONF_EVENT_DHCPSERVER_START:
			return NIMA_EVENT_NETCONF_DHCPSERVER_START;
		case eNCAPI_NETCONF_EVENT_DHCPSERVER_STOP:
			return NIMA_EVENT_NETCONF_DHCPSERVER_STOP;
		case eNCAPI_NETCONF_EVENT_STATUS_UPDATED:
			return NIMA_EVENT_NETCONF_STATUS_UPDATED;
		default:
			return -1;
	}
}

eNimaEvent nima_conv_event_wifi(eNcapiWifiEvent event)
{
	switch( event )
	{
		case eNCAPI_WIFI_EVENT_INSERTED:
			return NIMA_EVENT_WIFI_INSERTED;
		case eNCAPI_WIFI_EVENT_EXTRACTED:
			return NIMA_EVENT_WIFI_EXTRACTED;
		case eNCAPI_WIFI_EVENT_SCANNING_START:
			return NIMA_EVENT_WIFI_SCANNING_START;
		case eNCAPI_WIFI_EVENT_SCANNING_END:
			return NIMA_EVENT_WIFI_SCANNING_END;
		case eNCAPI_WIFI_EVENT_SCANNING_OUTPUT:
			return NIMA_EVENT_WIFI_SCANNING_OUTPUT;
		case eNCAPI_WIFI_EVENT_SCANNING_FAIL:
			return NIMA_EVENT_WIFI_SCANNING_FAIL;
		case eNCAPI_WIFI_EVENT_KEY_INVALID:
			return NIMA_EVENT_WIFI_KEY_INVALID;
		case eNCAPI_WIFI_EVENT_AP_CONNECT_SUCCESS:
			return NIMA_EVENT_WIFI_AP_CONNECT_SUCCESS;
		case eNCAPI_WIFI_EVENT_AP_CONNECT_FAIL:
			return NIMA_EVENT_WIFI_AP_CONNECT_FAIL;
		case eNCAPI_WIFI_EVENT_WPS_SUCCESS:
			return NIMA_EVENT_WIFI_WPS_SUCCESS;
		case eNCAPI_WIFI_EVENT_WPS_FAIL:
			return NIMA_EVENT_WIFI_WPS_FAIL;
		case eNCAPI_WIFI_EVENT_AP_DISCONNECTED:
			return NIMA_EVENT_WIFI_AP_DISCONNECTED;
		case eNCAPI_WIFI_EVENT_AP_RECONNECTED:
			return NIMA_EVENT_WIFI_AP_RECONNECTED;
		case eNCAPI_WIFI_EVENT_AP_CONNECT_STOP:
			return NIMA_EVENT_WIFI_AP_CONNECT_STOP;
		case eNCAPI_WIFI_EVENT_LINK_UPDATED:
			return NIMA_EVENT_WIFI_LINK_UPDATED;
		default:
			return -1;
	}

}

eNimaEvent nima_conv_event_pppoe(eNcapiPPPoEEvent event)
{
	switch( event )
	{
		case eNCAPI_PPPOE_EVENT_CONNECT_SUCCESS:
			return NIMA_EVENT_PPPOE_CONNECTED;
		case eNCAPI_PPPOE_EVENT_CONNECT_FAIL:
		case eNCAPI_PPPOE_EVENT_DISCONNECT_SUCCESS:
		case eNCAPI_PPPOE_EVENT_DISCONNECT_FAIL:
			return NIMA_EVENT_PPPOE_DISCONNECTED;
		case eNCAPI_PPPOE_EVENT_SET_INFO:
			return NIMA_EVENT_PPPOE_SET_INFO;
		case eNCAPI_PPPOE_EVENT_SET_ONOFF:
			return NIMA_EVENT_PPPOE_SET_ONOFF;
		default:
			return -1;
	}
}

eNimaEvent nima_conv_event_bt(eNcapiBTEvent event)
{
	switch( event )
	{
		case eNCAPI_BT_EVENT_INSERTED:
			return NIMA_EVENT_BT_INSERTED;
		case eNCAPI_BT_EVENT_EXTRACTED:
			return NIMA_EVENT_BT_EXTRACTED;
		case eNCAPI_BT_EVENT_ENABLE:
			return NIMA_EVENT_BT_ENABLE;
		case eNCAPI_BT_EVENT_DISABLE:
			return NIMA_EVENT_BT_DISABLE;
		case eNCAPI_BT_EVENT_SCAN_SUCCESS:
			return NIMA_EVENT_BT_SCAN_SUCCESS;
		case eNCAPI_BT_EVENT_SCAN_FAIL:
			return NIMA_EVENT_BT_SCAN_FAIL;
		case eNCAPI_BT_EVENT_AUTHEN_SUCCESS:
			return NIMA_EVENT_BT_AUTHEN_SUCCESS;
		case eNCAPI_BT_EVENT_AUTHEN_FAIL:
			return NIMA_EVENT_BT_AUTHEN_FAIL;
		case eNCAPI_BT_EVENT_PASSKEY:
			return NIMA_EVENT_BT_PASSKEY;
		case eNCAPI_BT_EVENT_CONNECT_SUCCESS:
			return NIMA_EVENT_BT_CONNECT_SUCCESS;
		case eNCAPI_BT_EVENT_CONNECT_FAIL:
			return NIMA_EVENT_BT_CONNECT_FAIL;
		case eNCAPI_BT_EVENT_REMOVED:
			return NIMA_EVENT_BT_REMOVED;
		case eNCAPI_BT_EVENT_REMOTE_CMD:
			return NIMA_EVENT_BT_REMOTE_CMD;
		case eNCAPI_BT_EVENT_MAX:
			return NIMA_EVENT_BT_MAX;
		default:
			return -1;
	}

}

eNimaBTStatus nima_conv_bt_status(eNcapiBTStatus status)
{
	switch( status )
	{
		case eNCAPI_BT_DEV_NOT_PAIRED:
			return eNIMA_BT_STATUS_NotPaired;
		case eNCAPI_BT_DEV_CONNECTED:
			return eNIMA_BT_STATUS_Connected;
		case eNCAPI_BT_DEV_DISCONNECTED:
			return eNIMA_BT_STATUS_Disconnected;
		default:
			return -1;
	}
}

HERROR nima_conv_netconf(NCAPI_NetConfInfo_t *src, NIMAProtocol_Netconf_t *dst)
{
	if(!src  || !dst) return  ERR_FAIL;

	dst->id = src->id;
	dst->priority = src->priority;
	HxSTD_MemCopy(dst->name, src->name, NCAPI_MAX_NET_INTERFACE_NAME);
	dst->type = src->type;
	dst->linked = src->linked;
	dst->active = src->active;
	dst->dhcp = src->dhcp;
	dst->dns_auto = src->dns_auto;
	dst->auto_ip = src->auto_ip;
	dst->ip_ver = src->ip_ver;

	switch(src->ip_ver)
	{
		case eNCAPI_NETCONF_IPV4:
			{
				HxSTD_MemCopy(dst->ip_info.ipv4.macaddr, src->ip_info.ipv4.macaddr, NCAPI_MAX_MACADDR);
				HxSTD_MemCopy(dst->ip_info.ipv4.ipAddr, src->ip_info.ipv4.ipAddr, NCAPI_MAX_IP4ADDR);
				HxSTD_MemCopy(dst->ip_info.ipv4.netmask, src->ip_info.ipv4.netmask, NCAPI_MAX_IP4ADDR);
				HxSTD_MemCopy(dst->ip_info.ipv4.gateway, src->ip_info.ipv4.gateway, NCAPI_MAX_IP4ADDR);
				HxSTD_MemCopy(dst->ip_info.ipv4.dns1, src->ip_info.ipv4.dns[0], NCAPI_MAX_IP4ADDR);
				HxSTD_MemCopy(dst->ip_info.ipv4.dns2, src->ip_info.ipv4.dns[1], NCAPI_MAX_IP4ADDR);
			}
			break;
		case eNCAPI_NETCONF_IPV6:
			{
				HxSTD_MemCopy(dst->ip_info.ipv6.macaddr, src->ip_info.ipv6.macaddr, NCAPI_MAX_MACADDR);
				HxSTD_MemCopy(dst->ip_info.ipv6.ipAddr, src->ip_info.ipv6.ipAddr, NCAPI_MAX_IP6ADDR);
				HxSTD_MemCopy(dst->ip_info.ipv6.gateway, src->ip_info.ipv6.gateway, NCAPI_MAX_IP6ADDR);
				HxSTD_MemCopy(dst->ip_info.ipv6.dns1, src->ip_info.ipv6.dns[0], NCAPI_MAX_IP6ADDR);
				HxSTD_MemCopy(dst->ip_info.ipv6.dns2, src->ip_info.ipv6.dns[1], NCAPI_MAX_IP6ADDR);
			}
			break;
	}

	HLIB_STD_StrNCpySafe(dst->fw_ver, src->fw_ver, NIMA_PROTO_FW_VERSION_LEN_MAX);

	return ERR_OK;
}

HERROR nima_conv_apinfo(NCAPI_WifiAPInfo_t *src, NIMAProtocol_APInfo_t *dst)
{
	if(!src || !dst) return ERR_FAIL;

	HxSTD_MemCopy(dst->apkey, src->apkey, NIMA_PROTO_AP_KEY);
	HxSTD_MemCopy(dst->essid, src->essid, NIMA_PROTO_AP_ESSID);
	HxSTD_MemCopy(dst->macaddr, src->macaddr, NIMA_PROTO_MAX_MACADDR);
	dst->wave_strength = src->wave_strength;
	dst->connect_speed = src->connect_speed;

	switch(src->op_mode)
	{
		case eNCAPI_WIFI_OP_AUTO:
			dst->op_mode = eNIMA_WIFI_OP_AUTO;
			break;
		case eNCAPI_WIFI_OP_ADHOC:
			dst->op_mode = eNIMA_WIFI_OP_ADHOC;
			break;
		case eNCAPI_WIFI_OP_INFRA:
			dst->op_mode = eNIMA_WIFI_OP_INFRA;
			break;
		case eNCAPI_WIFI_OP_MASTER:
			dst->op_mode = eNIMA_WIFI_OP_MASTER;
			break;
		case eNCAPI_WIFI_OP_REPEATER:
			dst->op_mode = eNIMA_WIFI_OP_REPEATER;
			break;
		case eNCAPI_WIFI_OP_SECONDARY:
			dst->op_mode = eNIMA_WIFI_OP_SECONDARY;
			break;
		case eNCAPI_WIFI_OP_MONITOR:
			dst->op_mode = eNIMA_WIFI_OP_MONITOR;
			break;
		default :
			dst->op_mode = eNIMA_WIFI_OP_UNKNOWN;
			break;
	}

	switch(src->secure_type)
	{
		case eNCAPI_WIFI_SECURITY_OPEN:
			dst->secure_type = eNIMA_WIFI_SECURITY_OPEN;
			break;
		case eNCAPI_WIFI_SECURITY_WEP:
			dst->secure_type = eNIMA_WIFI_SECURITY_WEP;
			break;
		case eNCAPI_WIFI_SECURITY_WPA:
			dst->secure_type = eNIMA_WIFI_SECURITY_WPA;
			break;
		case eNCAPI_WIFI_SECURITY_WPA2:
			dst->secure_type = eNIMA_WIFI_SECURITY_WPA2;
			break;
		case eNCAPI_WIFI_SECURITY_WPA_WPA2:
			dst->secure_type = eNIMA_WIFI_SECURITY_WPA_WPA2;
			break;
		default :
			dst->secure_type = eNIMA_WIFI_SECURITY_UNKNOWN;
			break;
	}

	switch(src->authen_type)
	{
		case eNCAPI_WIFI_AUTH_OPEN :
			dst->authen_type = eNIMA_WIFI_AUTH_OPEN;
			break;
		case eNCAPI_WIFI_AUTHEN_PSK :
			dst->authen_type = eNIMA_WIFI_AUTH_PSK;
			break;
		case eNCAPI_WIFI_AUTHEN_802_1X :
			dst->authen_type = eNIMA_WIFI_AUTH_802_1X;
			break;
		default :
			dst->authen_type = eNIMA_WIFI_AUTH_UNKNOWN;
			break;
	}
	switch(src->encrypt_type)
	{
		case eNCAPI_WIFI_ENCRYPT_NONE :
			dst->encrypt_type = eNIMA_WIFI_ENCRYPT_NONE;
			break;
		case eNCAPI_WIFI_ENCRYPT_WEP_64 :
			dst->encrypt_type = eNIMA_WIFI_ENCRYPT_WEP_64;
			break;
		case eNCAPI_WIFI_ENCRYPT_WEP_64HEX :
			dst->encrypt_type = eNIMA_WIFI_ENCRYPT_WEP_64HEX;
			break;
		case eNCAPI_WIFI_ENCRYPT_WEP_128 :
			dst->encrypt_type = eNIMA_WIFI_ENCRYPT_WEP_128;
			break;
		case eNCAPI_WIFI_ENCRYPT_WEP_128HEX :
			dst->encrypt_type = eNIMA_WIFI_ENCRYPT_WEP_128HEX;
			break;
		case eNCAPI_WIFI_ENCRYPT_TKIP :
			dst->encrypt_type = eNIMA_WIFI_ENCRYPT_TKIP;
			break;
		case eNCAPI_WIFI_ENCRYPT_AES :
			dst->encrypt_type = eNIMA_WIFI_ENCRYPT_AES;
			break;
		case eNCAPI_WIFI_ENCRYPT_TKIP_AES :
			dst->encrypt_type = eNIMA_WIFI_ENCRYPT_TKIP_AES;
			break;
		default :
			dst->encrypt_type = eNIMA_WIFI_ENCRYPT_UNKNOWN;
			break;
	}

	dst->is_connected = src->is_connected;
	return ERR_OK;
}

HERROR nima_conv_ncapi_apinfo(NIMAProtocol_APInfo_t *src, NCAPI_WifiAPInfo_t *dst)
{
	if(!src || !dst) return ERR_FAIL;

	HxSTD_MemSet(dst, 0x00, sizeof(NCAPI_WifiAPInfo_t));
	HxSTD_MemCopy(dst->essid, src->essid, NIMA_PROTO_AP_ESSID);
	HxSTD_MemCopy(dst->apkey, src->apkey, NIMA_PROTO_AP_KEY);
	HxSTD_MemCopy(dst->macaddr, src->macaddr, NIMA_PROTO_MAX_MACADDR);
	dst->wave_strength = src->wave_strength;
	dst->connect_speed = src->connect_speed;

	switch(src->op_mode)
	{
		case eNIMA_WIFI_OP_AUTO:
			dst->op_mode = eNCAPI_WIFI_OP_AUTO;
			break;
		case eNIMA_WIFI_OP_ADHOC:
			dst->op_mode = eNCAPI_WIFI_OP_ADHOC;
			break;
		case eNIMA_WIFI_OP_INFRA:
			dst->op_mode = eNCAPI_WIFI_OP_INFRA;
			break;
		case eNIMA_WIFI_OP_MASTER:
			dst->op_mode = eNCAPI_WIFI_OP_MASTER;
			break;
		case eNIMA_WIFI_OP_REPEATER:
			dst->op_mode = eNCAPI_WIFI_OP_REPEATER;
			break;
		case eNIMA_WIFI_OP_SECONDARY:
			dst->op_mode = eNCAPI_WIFI_OP_SECONDARY;
			break;
		case eNIMA_WIFI_OP_MONITOR:
			dst->op_mode = eNCAPI_WIFI_OP_MONITOR;
			break;
		default :
			dst->op_mode = eNCAPI_WIFI_OP_UNKNOWN;
			break;
	}

	switch(src->secure_type)
	{
		case eNIMA_WIFI_SECURITY_OPEN:
			dst->secure_type = eNCAPI_WIFI_SECURITY_OPEN;
			break;
		case eNIMA_WIFI_SECURITY_WEP:
			dst->secure_type = eNCAPI_WIFI_SECURITY_WEP;
			break;
		case eNIMA_WIFI_SECURITY_WPA:
			dst->secure_type = eNCAPI_WIFI_SECURITY_WPA;
			break;
		case eNIMA_WIFI_SECURITY_WPA2:
			dst->secure_type = eNCAPI_WIFI_SECURITY_WPA2;
			break;
		case eNIMA_WIFI_SECURITY_WPA_WPA2:
			dst->secure_type = eNCAPI_WIFI_SECURITY_WPA_WPA2;
			break;
		default :
			dst->secure_type = eNCAPI_WIFI_SECURITY_UNKNOWN;
			break;
	}

    switch(src->authen_type)
    {
        case eNIMA_WIFI_AUTH_OPEN :
                dst->authen_type = eNCAPI_WIFI_AUTH_OPEN;
                break;
        case eNIMA_WIFI_AUTH_802_1X :
                dst->authen_type = eNCAPI_WIFI_AUTHEN_802_1X;
                break;
		case eNIMA_WIFI_AUTH_PSK:
                dst->authen_type = eNCAPI_WIFI_AUTHEN_PSK;
                break;
        default :
                dst->authen_type = eNCAPI_WIFI_AUTHEN_UNKNOWN;
                break;
    }

    switch(src->encrypt_type)
    {
        case eNIMA_WIFI_ENCRYPT_NONE :
                dst->encrypt_type = eNCAPI_WIFI_ENCRYPT_NONE;
                break;
        case eNIMA_WIFI_ENCRYPT_WEP_64 :
                dst->encrypt_type = eNCAPI_WIFI_ENCRYPT_WEP_64;
                break;
        case eNIMA_WIFI_ENCRYPT_WEP_64HEX :
                dst->encrypt_type = eNCAPI_WIFI_ENCRYPT_WEP_64HEX;
                break;
        case eNIMA_WIFI_ENCRYPT_WEP_128 :
                dst->encrypt_type = eNCAPI_WIFI_ENCRYPT_WEP_128;
                break;
        case eNIMA_WIFI_ENCRYPT_WEP_128HEX :
                dst->encrypt_type = eNCAPI_WIFI_ENCRYPT_WEP_128HEX;
                break;
        case eNIMA_WIFI_ENCRYPT_TKIP :
                dst->encrypt_type = eNCAPI_WIFI_ENCRYPT_TKIP;
                break;
        case eNIMA_WIFI_ENCRYPT_AES :
                dst->encrypt_type = eNCAPI_WIFI_ENCRYPT_AES;
                break;
        case eNIMA_WIFI_ENCRYPT_TKIP_AES :
                dst->encrypt_type = eNCAPI_WIFI_ENCRYPT_TKIP_AES;
                break;
        default :
                dst->encrypt_type = eNCAPI_WIFI_ENCRYPT_UNKNOWN;
                break;
    }

	return ERR_OK;
}
HERROR nima_conv_pppoe(NCAPI_PPPoEInfo_t *src, NIMAProtocol_PPPOE_info_t *dst)
{
	if(!src || !dst) return ERR_FAIL;

	dst->dev_id = src->id;
	dst->bOn    = src->enable;
	HxSTD_MemCopy(dst->szUserID, src->connect_id, NCAPI_PPPOE_ID_LEN);
	HxSTD_MemCopy(dst->szUserPW, src->connect_pw, NCAPI_PPPOE_PW_LEN);
	return ERR_OK;
}

HERROR nima_conv_ncapi_pppoe(NIMAProtocol_PPPOE_info_t *src, NCAPI_PPPoEInfo_t *dst)
{
	if(!src || !dst) return ERR_FAIL;

	dst->id = src->dev_id;
	dst->enable = src->bOn;
	HxSTD_MemCopy(dst->connect_id, src->szUserID, NIMA_PROTO_PPPOE_ID_LEN_MAX - 1);
	HxSTD_MemCopy(dst->connect_pw, src->szUserPW, NIMA_PROTO_PPPOE_PW_LEN_MAX - 1);

	return ERR_OK;
}

HERROR nima_conv_btinfo(NCAPI_BTInfo_t *src, NIMAProtocol_bt_info_t *dst)
{
	HINT32 i = 0;
	if(!src || !dst) return ERR_FAIL;

	dst->enable = src->enable;
	dst->dev_count = src->count;
	dst->passkey = src->passkey;
	//host device setting
	dst->host_info.status = nima_conv_bt_status(src->host_info.status);
	dst->host_info.support_profile = src->host_info.available_profile;
	HxSTD_MemCopy(dst->host_info.name, src->host_info.name, NIMA_PROTO_BT_NAME_MAX);
	HxSTD_MemCopy(dst->host_info.bdaddr, src->host_info.bdaddr, NIMA_PROTO_BT_BDADDR_LEN);
	HxSTD_MemCopy(dst->host_info.dev_class, src->host_info.dev_class, NIMA_PROTO_BT_CLASS_LEN);
	HxSTD_MemCopy(dst->connected_bdaddr, src->connected_bdaddr, NIMA_PROTO_BT_BDADDR_LEN);
	// device list setiing
	for( i=0; i<dst->dev_count; i++ )
	{
		dst->dev_info[i].status = nima_conv_bt_status(src->dev_list[i].status);
		dst->dev_info[i].support_profile = src->dev_list[i].available_profile;
		HxSTD_MemCopy(dst->dev_info[i].name, src->dev_list[i].name, NIMA_PROTO_BT_NAME_MAX);
		HxSTD_MemCopy(dst->dev_info[i].bdaddr, src->dev_list[i].bdaddr, NIMA_PROTO_BT_BDADDR_LEN);
		HxSTD_MemCopy(dst->dev_info[i].dev_class, src->dev_list[i].dev_class, NIMA_PROTO_BT_CLASS_LEN);
	}
	return ERR_OK;
}

HERROR nima_conv_bt_devinfo(NCAPI_BTDevInfo_t *src, NIMAProtocol_bt_dev_t *dst)
{
	if(!src || !dst) return ERR_FAIL;

	dst->status = nima_conv_bt_status(src->status);
	dst->support_profile = src->available_profile;
	HxSTD_MemCopy(dst->name, src->name, NIMA_PROTO_BT_NAME_MAX);
	HxSTD_MemCopy(dst->bdaddr, src->bdaddr, NIMA_PROTO_BT_BDADDR_LEN);
	HxSTD_MemCopy(dst->dev_class, src->dev_class, NIMA_PROTO_BT_CLASS_LEN);

	return ERR_OK;
}

