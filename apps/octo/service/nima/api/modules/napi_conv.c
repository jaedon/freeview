/** **********************************************************************************************************
	File 		: napi_conv.c
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

#include <hlib.h>
#include "napi_conv.h"
#include "napi_parser.h"


static eNapiBTType __napi_conv_bt_devtype(HUINT8 *dev_class)
{
	eNapiBTType type = eNAPI_BT_DEVTYPE_Unknown;

	switch(dev_class[1])
	{
		case NAPI_PROTO_BT_DEVTYPE_Misc: 		type = eNAPI_BT_DEVTYPE_Misc; 		break;
		case NAPI_PROTO_BT_DEVTYPE_Computer: 	type = eNAPI_BT_DEVTYPE_Computer; 	break;
		case NAPI_PROTO_BT_DEVTYPE_Phone:		type = eNAPI_BT_DEVTYPE_Phone; 		break;
		case NAPI_PROTO_BT_DEVTYPE_Network: 	type = eNAPI_BT_DEVTYPE_Network; 	break;
		case NAPI_PROTO_BT_DEVTYPE_AV: 			type = eNAPI_BT_DEVTYPE_AV; 		break;
		case NAPI_PROTO_BT_DEVTYPE_Peripheral: 	type = eNAPI_BT_DEVTYPE_Peripheral; break;
		case NAPI_PROTO_BT_DEVTYPE_Imaging: 	type = eNAPI_BT_DEVTYPE_Imaging; 	break;
		case NAPI_PROTO_BT_DEVTYPE_Unknown: 	type = eNAPI_BT_DEVTYPE_Unknown; 	break;
		default: type = eNAPI_BT_DEVTYPE_Unknown; break;
	}

	return type;
}

eNapiProtoEvent napi_conv_event_bt(eNapiProtoEvent event)
{
	eNapiProtoEvent return_event = NAPI_PROTO_EVENT_BT_MAX;

	switch(event)
	{
		case NAPI_PROTO_EVENT_BT_ENABLE:
		case NAPI_PROTO_EVENT_BT_SCAN_SUCCESS:
			{
				return_event = NAPI_EVENT_LIST_UPDATED;
			}
			break;

		case NAPI_PROTO_EVENT_BT_AUTHEN_SUCCESS:
		case NAPI_PROTO_EVENT_BT_CONNECT_SUCCESS:
			{
				return_event = NAPI_EVENT_CONNECTED;
			}
			break;
		case NAPI_PROTO_EVENT_BT_AUTHEN_FAIL:
			{
				return_event = NAPI_EVENT_PIN_REQUESTED;
			}
			break;
		case NAPI_PROTO_EVENT_BT_PASSKEY:
			{
				return_event = NAPI_EVENT_PASSKEY;
			}
		 	break;
		case NAPI_PROTO_EVENT_BT_SCAN_FAIL:
		case NAPI_PROTO_EVENT_BT_CONNECT_FAIL:
			{
				return_event = NAPI_EVENT_CONNECT_FAILED;
			}
			break;
		case NAPI_PROTO_EVENT_BT_REMOVED:
			{
				return_event = NAPI_EVENT_REMOVED;
			}
			break;
		case NAPI_PROTO_EVENT_BT_DISABLE:
		case NAPI_PROTO_EVENT_BT_INSERTED:
		case NAPI_PROTO_EVENT_BT_REMOTE_CMD:
		case NAPI_PROTO_EVENT_BT_EXTRACTED:
		case NAPI_PROTO_EVENT_BT_MAX:
		default: break;
	}

	return return_event;
}


HERROR napi_conv_cmd_netconf(NAPINetConfInfo_t *src, NAPIProtocol_Netconf_t *dst)
{
	HERROR hRet = ERR_OK;
	if(!src || !dst) return ERR_FAIL;

	dst->id = src->id;
	HLIB_STD_StrNCpySafe(dst->name, src->name, NAPI_PROTO_MAX_INTERFACE_NAME);
	dst->type = napi_parser_devtype_NapitoProtocol(src->type);
	dst->linked = src->linked;
	dst->active = src->active;
	dst->dhcp = TRUE; //info->dhcp;
	dst->dns_auto = src->dns_auto;
	dst->auto_ip = src->auto_ip;
	dst->ip_ver = src->ip_ver;

	if( src->ip_ver == eNAPI_NETCONF_IPV4 )
	{
		napi_parser_macAtoN(src->ip_info.ipv4.macaddr, dst->ip_info.ipv4.macaddr);
		napi_parser_addrAtoN(src->ip_info.ipv4.ipAddr, dst->ip_info.ipv4.ipAddr);
		napi_parser_addrAtoN(src->ip_info.ipv4.netmask, dst->ip_info.ipv4.netmask);
		napi_parser_addrAtoN(src->ip_info.ipv4.gateway, dst->ip_info.ipv4.gateway);
		napi_parser_addrAtoN(src->ip_info.ipv4.dns1, dst->ip_info.ipv4.dns1);
		napi_parser_addrAtoN(src->ip_info.ipv4.dns2, dst->ip_info.ipv4.dns2);
	}
	else if( src->ip_ver == eNAPI_NETCONF_IPV6 )
	{
		napi_parser_macAtoN(src->ip_info.ipv6.macaddr, dst->ip_info.ipv6.macaddr);
		napi_parser_addrAtoN(src->ip_info.ipv6.ipAddr, dst->ip_info.ipv6.ipAddr);
		napi_parser_addrAtoN(src->ip_info.ipv6.gateway, dst->ip_info.ipv6.gateway);
		napi_parser_addrAtoN(src->ip_info.ipv6.dns1, dst->ip_info.ipv6.dns1);
		napi_parser_addrAtoN(src->ip_info.ipv6.dns2, dst->ip_info.ipv6.dns2);
	}
	return hRet;
}
HERROR napi_conv_cmd_apinfo(const NAPIWifiAPInfo_t *src, NAPIProtocol_APInfo_t *dst)
{
	if(!src || !dst) return ERR_FAIL;

	HLIB_STD_StrNCpySafe(dst->essid, src->essid, NAPI_PROTO_AP_ESSID);
	HLIB_STD_StrNCpySafe(dst->apkey, src->apkey, NAPI_PROTO_AP_KEY);
	napi_parser_macAtoN(src->macaddr,dst->macaddr);

	dst->wave_strength = src->wave_strength;
	dst->connect_speed = src->connect_speed;
	dst->is_connected = src->bIsConnected;

	switch(src->op_mode)
	{
		case eNAPI_WIFI_OP_AUTO:
			dst->op_mode = eNAPI_TYPE_WIFI_OP_AUTO;
			break;
		case eNAPI_WIFI_OP_ADHOC:
			dst->op_mode = eNAPI_TYPE_WIFI_OP_ADHOC;
			break;
		case eNAPI_WIFI_OP_INFRA:
			dst->op_mode = eNAPI_TYPE_WIFI_OP_INFRA;
			break;
		case eNAPI_WIFI_OP_MASTER:
			dst->op_mode = eNAPI_TYPE_WIFI_OP_MASTER;
			break;
		case eNAPI_WIFI_OP_REPEATER:
			dst->op_mode = eNAPI_TYPE_WIFI_OP_REPEATER;
			break;
		case eNAPI_WIFI_OP_SECONDARY:
			dst->op_mode = eNAPI_TYPE_WIFI_OP_SECONDARY;
			break;
		case eNAPI_WIFI_OP_MONITOR:
			dst->op_mode = eNAPI_TYPE_WIFI_OP_MONITOR;
			break;
		default :
			dst->op_mode = eNAPI_TYPE_WIFI_OP_UNKNOWN;
			break;
	}

	switch(src->secure_type)
	{
		case eNAPI_WIFI_SECURITY_OPEN:
			dst->secure_type = eNAPI_TYPE_WIFI_SECURITY_OPEN;
			break;
		case eNAPI_WIFI_SECURITY_WEP:
			dst->secure_type = eNAPI_TYPE_WIFI_SECURITY_WEP;
			break;
		case eNAPI_WIFI_SECURITY_WPA:
			dst->secure_type = eNAPI_TYPE_WIFI_SECURITY_WPA;
			break;
		case eNAPI_WIFI_SECURITY_WPA2:
			dst->secure_type = eNAPI_TYPE_WIFI_SECURITY_WPA2;
			break;
		case eNAPI_WIFI_SECURITY_WPA_WPA2:
			dst->secure_type = eNAPI_TYPE_WIFI_SECURITY_WPA_WPA2;
			break;
		default :
			dst->secure_type = eNAPI_TYPE_WIFI_SECURITY_UNKNOWN;
			break;
	}

	switch(src->authen_type)
	{
		case eNAPI_WIFI_AUTH_OPEN :
			dst->authen_type = eNAPI_TYPE_WIFI_AUTH_OPEN;
			break;
		case eNAPI_WIFI_AUTH_WPAPSK :
		case eNAPI_WIFI_AUTH_WPA2PSK:
			dst->authen_type = eNAPI_TYPE_WIFI_AUTH_PSK;
			break;
		case eNAPI_WIFI_AUTH_802_1x :
			dst->authen_type = eNAPI_TYPE_WIFI_AUTH_802_1X;
			break;
		default :
			dst->authen_type = eNAPI_TYPE_WIFI_AUTH_UNKNOWN;
			break;
	}
	switch(src->encrypt_type)
	{
		case eNAPI_WIFI_ENCRYPT_NONE :
			dst->encrypt_type = eNAPI_TYPE_WIFI_ENCRYPT_NONE;
			break;
		case eNAPI_WIFI_ENCRYPT_WEP_64 :
		case eNAPI_WIFI_ENCRYPT_WEP_64_ASCII:
			dst->encrypt_type = eNAPI_TYPE_WIFI_ENCRYPT_WEP_64;
			break;
		case eNAPI_WIFI_ENCRYPT_WEP_64_HEX :
			dst->encrypt_type = eNAPI_TYPE_WIFI_ENCRYPT_WEP_64HEX;
			break;
		case eNAPI_WIFI_ENCRYPT_WEP_128 :
		case eNAPI_WIFI_ENCRYPT_WEP_128_ASCII:
			dst->encrypt_type = eNAPI_TYPE_WIFI_ENCRYPT_WEP_128;
			break;
		case eNAPI_WIFI_ENCRYPT_WEP_128_HEX :
			dst->encrypt_type = eNAPI_TYPE_WIFI_ENCRYPT_WEP_128HEX;
			break;
		case eNAPI_WIFI_ENCRYPT_WPA_TKIP :
			dst->encrypt_type = eNAPI_TYPE_WIFI_ENCRYPT_TKIP;
			break;
		case eNAPI_WIFI_ENCRYPT_WPA_AES :
			dst->encrypt_type = eNAPI_TYPE_WIFI_ENCRYPT_AES;
			break;
		case eNAPI_WIFI_ENCRYPT_WPA_TKIP_AES :
			dst->encrypt_type = eNAPI_TYPE_WIFI_ENCRYPT_TKIP_AES;
			break;
		default :
			dst->encrypt_type = eNAPI_TYPE_WIFI_ENCRYPT_UNKNOWN;
			break;
	}

	return ERR_OK;
}

HERROR napi_conv_cmd_pppoe(NAPIPPPOEInfo_t *src, NAPIProtocol_PPPOE_info_t *dst)
{
	HERROR hRet = ERR_OK;
	if(!src || !dst) return ERR_FAIL;

 	dst->dev_id = src->dev_id;
	HLIB_STD_StrNCpySafe(dst->szUserID, src->szUserID, NAPI_PROTO_PPPOE_USER_ID_LEN_MAX);
	HLIB_STD_StrNCpySafe(dst->szUserPW, src->szUserPW, NAPI_PROTO_PPPOE_USER_PW_LEN_MAX);

	return hRet;
}
HERROR napi_conv_cmd_btinfo(NAPIBTInfo_t *src, NAPIProtocol_bt_info_t *dst)
{
	HERROR hRet = ERR_OK;
	if(!src || !dst) return ERR_FAIL;

	return hRet;
}

HERROR napi_conv_event_netconf(NAPIProtocol_Netconf_t *src, NAPINetConfInfo_t *dst)
{
	HERROR hRet = ERR_OK;
	if(!src || !dst) return ERR_FAIL;


	dst->id = src->id;
	HLIB_STD_StrNCpySafe(dst->name, src->name, NAPI_MAX_INTERFACE_NAME);
	dst->type = napi_parser_devtype_ProtocoltoNapi(src->type);
	dst->linked = src->linked;
	dst->active = src->active;
	dst->dhcp = src->dhcp;
	dst->dns_auto = src->dns_auto;
	dst->auto_ip = src->auto_ip;
	dst->ip_ver = src->ip_ver;

	if( dst->ip_ver == eNAPI_NETCONF_IPV4 )
	{
		napi_parser_macNtoA(src->ip_info.ipv4.macaddr, dst->ip_info.ipv4.macaddr);
		napi_parser_addrNtoA(src->ip_info.ipv4.ipAddr, dst->ip_info.ipv4.ipAddr, NAPI_MAX_IP4ADDR);
		napi_parser_addrNtoA(src->ip_info.ipv4.netmask, dst->ip_info.ipv4.netmask, NAPI_MAX_IP4ADDR);
		napi_parser_addrNtoA(src->ip_info.ipv4.gateway, dst->ip_info.ipv4.gateway, NAPI_MAX_IP4ADDR);
		napi_parser_addrNtoA(src->ip_info.ipv4.dns1, dst->ip_info.ipv4.dns1, NAPI_MAX_IP4ADDR);
		napi_parser_addrNtoA(src->ip_info.ipv4.dns2, dst->ip_info.ipv4.dns2, NAPI_MAX_IP4ADDR);
	}
	else if( dst->ip_ver == eNAPI_NETCONF_IPV6 )
	{
		napi_parser_macNtoA(src->ip_info.ipv6.macaddr, dst->ip_info.ipv6.macaddr);
		napi_parser_addrNtoA(src->ip_info.ipv6.ipAddr, dst->ip_info.ipv6.ipAddr, NAPI_MAX_IP6ADDR);
		napi_parser_addrNtoA(src->ip_info.ipv6.gateway, dst->ip_info.ipv6.gateway, NAPI_MAX_IP6ADDR);
		napi_parser_addrNtoA(src->ip_info.ipv6.dns1, dst->ip_info.ipv6.dns1, NAPI_MAX_IP6ADDR);
		napi_parser_addrNtoA(src->ip_info.ipv6.dns2, dst->ip_info.ipv6.dns2, NAPI_MAX_IP6ADDR);
	}

	HLIB_STD_StrNCpySafe(dst->fw_ver, src->fw_ver, NAPI_FW_VERSION_LEN_MAX);

	return hRet;
}
HERROR napi_conv_event_apinfo(NAPIProtocol_APInfo_t *src,  NAPIWifiAPInfo_t *dst)
{
	if(!src || !dst) return ERR_FAIL;

	HxSTD_MemSet(dst, 0x00, sizeof(NAPIWifiAPInfo_t));
	HLIB_STD_StrNCpySafe(dst->essid, src->essid, NAPI_AP_ESSID);
	HLIB_STD_StrNCpySafe(dst->apkey, src->apkey, NAPI_AP_KEY);
	napi_parser_macNtoA(src->macaddr, dst->macaddr);

	dst->wave_strength = src->wave_strength;
	dst->connect_speed = src->connect_speed;
	dst->bIsConnected = src->is_connected;

	switch(src->op_mode)
	{
		case eNAPI_TYPE_WIFI_OP_AUTO:
			dst->op_mode = eNAPI_WIFI_OP_AUTO;
			break;
		case eNAPI_TYPE_WIFI_OP_ADHOC:
			dst->op_mode = eNAPI_WIFI_OP_ADHOC;
			break;
		case eNAPI_TYPE_WIFI_OP_INFRA:
			dst->op_mode = eNAPI_WIFI_OP_INFRA;
			break;
		case eNAPI_TYPE_WIFI_OP_MASTER:
			dst->op_mode = eNAPI_WIFI_OP_MASTER;
			break;
		case eNAPI_TYPE_WIFI_OP_REPEATER:
			dst->op_mode = eNAPI_WIFI_OP_REPEATER;
			break;
		case eNAPI_TYPE_WIFI_OP_SECONDARY:
			dst->op_mode = eNAPI_WIFI_OP_SECONDARY;
			break;
		case eNAPI_TYPE_WIFI_OP_MONITOR:
			dst->op_mode = eNAPI_WIFI_OP_MONITOR;
			break;
		default :
			dst->op_mode = eNAPI_WIFI_OP_UNKNOWN;
			break;
	}

	switch(src->secure_type)
	{
		case eNAPI_TYPE_WIFI_SECURITY_OPEN:
			dst->secure_type = eNAPI_WIFI_SECURITY_OPEN;
			break;
		case eNAPI_TYPE_WIFI_SECURITY_WEP:
			dst->secure_type = eNAPI_WIFI_SECURITY_WEP;
			break;
		case eNAPI_TYPE_WIFI_SECURITY_WPA:
			dst->secure_type = eNAPI_WIFI_SECURITY_WPA;
			break;
		case eNAPI_TYPE_WIFI_SECURITY_WPA2:
			dst->secure_type = eNAPI_WIFI_SECURITY_WPA2;
			break;
		case eNAPI_TYPE_WIFI_SECURITY_WPA_WPA2:
			dst->secure_type = eNAPI_WIFI_SECURITY_WPA_WPA2;
			break;
		default :
			dst->secure_type = eNAPI_WIFI_SECURITY_UNKNOWN;
			break;
	}

    switch(src->authen_type)
    {
        case eNAPI_TYPE_WIFI_AUTH_OPEN :
                dst->authen_type = eNAPI_WIFI_AUTH_OPEN;
                break;

        case eNAPI_TYPE_WIFI_AUTH_PSK :
				dst->authen_type = eNAPI_WIFI_AUTH_WPAPSK;
				break;

		case eNAPI_TYPE_WIFI_AUTH_802_1X:
				dst->authen_type = eNAPI_WIFI_AUTH_802_1x;
				break;

        default :
                dst->authen_type = eNAPI_WIFI_AUTH_UNKNOWN;
                break;
    }

    switch(src->encrypt_type)
    {
        case eNAPI_TYPE_WIFI_ENCRYPT_NONE :
                dst->encrypt_type = eNAPI_WIFI_ENCRYPT_NONE;
                break;
        case eNAPI_TYPE_WIFI_ENCRYPT_WEP_64 :
                dst->encrypt_type = eNAPI_WIFI_ENCRYPT_WEP_64;
                break;
        case eNAPI_TYPE_WIFI_ENCRYPT_WEP_64HEX :
                dst->encrypt_type = eNAPI_WIFI_ENCRYPT_WEP_64_HEX;
                break;
        case eNAPI_TYPE_WIFI_ENCRYPT_WEP_128 :
                dst->encrypt_type = eNAPI_WIFI_ENCRYPT_WEP_128;
                break;
        case eNAPI_TYPE_WIFI_ENCRYPT_WEP_128HEX :
                dst->encrypt_type = eNAPI_WIFI_ENCRYPT_WEP_128_HEX;
                break;
        case eNAPI_TYPE_WIFI_ENCRYPT_TKIP :
                dst->encrypt_type = eNAPI_WIFI_ENCRYPT_WPA_TKIP;
                break;
        case eNAPI_TYPE_WIFI_ENCRYPT_AES :
                dst->encrypt_type = eNAPI_WIFI_ENCRYPT_WPA_AES;
                break;
        case eNAPI_TYPE_WIFI_ENCRYPT_TKIP_AES :
                dst->encrypt_type = eNAPI_WIFI_ENCRYPT_WPA_TKIP_AES;
                break;
        default :
                dst->encrypt_type = eNAPI_WIFI_ENCRYPT_UNKNOWN;
                break;
    }

	return ERR_OK;
}
HERROR napi_conv_event_pppoe(NAPIProtocol_PPPOE_info_t *src, NAPIPPPOEInfo_t *dst)
{
	HERROR hRet = ERR_OK;
	if(!src || !dst) return ERR_FAIL;

	dst->dev_id = src->dev_id;
	dst->bOn = src->bOn;
	HLIB_STD_StrNCpySafe(dst->szUserID, src->szUserID, NAPI_PPPOE_USER_ID_LEN_MAX);
	HLIB_STD_StrNCpySafe(dst->szUserPW, src->szUserPW, NAPI_PPPOE_USER_PW_LEN_MAX);

	return hRet;
}

HERROR napi_conv_event_btinfo(NAPIProtocol_bt_info_t *src, NAPIBTInfo_t *dst)
{
	HERROR hRet = ERR_OK;
	HINT32 i = 0;

	if(!src || !dst) return ERR_FAIL;

	dst->enable = src->enable;
	dst->dev_count = src->dev_count;
	dst->passkey = src->passkey;

	for( i=0; i<dst->dev_count; i++ )
	{
		dst->dev_info[i].dev_id = i;
		HxSTD_MemCopy(dst->dev_info[i].dev_class, src->dev_info[i].dev_class, NAPI_PROTO_BT_CLASS_LEN);

		dst->dev_info[i].status = src->dev_info[i].status;
		dst->dev_info[i].support_profile = src->dev_info[i].support_profile;
		HxSTD_MemCopy(dst->dev_info[i].name, src->dev_info[i].name, NAPI_PROTO_BT_NAME_MAX);
		napi_parser_macNtoA(src->dev_info[i].bdaddr, dst->dev_info[i].bdaddr);
	}
	dst->host_info.dev_id = 0;
	HxSTD_MemCopy(dst->host_info.dev_class, src->host_info.dev_class, NAPI_PROTO_BT_CLASS_LEN);
	dst->host_info.status = src->host_info.status;
	dst->host_info.support_profile = src->host_info.support_profile;
	HLIB_STD_StrNCpySafe(dst->host_info.name, src->host_info.name, NAPI_BT_NAME_MAX);
	napi_parser_macNtoA(src->host_info.bdaddr, dst->host_info.bdaddr);

	return hRet;
}


HERROR napi_conv_event_bt_dev_info(NAPIProtocol_bt_dev_t *src, NAPIBTDevInfo_t *dst)
{
	HERROR hRet = ERR_OK;

	if(!src || !dst) return ERR_FAIL;

	dst->dev_id = 0;
	HxSTD_MemCopy(dst->dev_class, src->dev_class, NAPI_PROTO_BT_CLASS_LEN);
	dst->status = src->status;
	dst->support_profile = src->support_profile;
	HLIB_STD_StrNCpySafe(dst->name, src->name, NAPI_BT_NAME_MAX);
	napi_parser_macNtoA(src->bdaddr, dst->bdaddr);

	return hRet;
}


