/** **********************************************************************************************************
	File 		: napi_wifi.c
	author 		:
	comment		: WIFI
	date    	: 2013/05/27
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

#include "napi_wifi.h"
#include "napi_protocol.h"
#include "napi_cache.h"
#include "napi_agent_common.h"
#include "napi_parser.h"
#include "../napi_debug.h"
#include "napi_conv.h"


#include <hlib.h>
/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/

HERROR NAPI_Wifi_RegisterCallback(NAPI_WifiCallback callback)
{
	HERROR	eRet = ERR_OK;
	FuncEnter;

	eRet = napi_agent_register_callback(napi_agent_get_instance(), callback, eAGENT_EVENT_TYPE_WIFI);

	FuncLeave;
	return eRet;
}

HERROR NAPI_Wifi_ScanStart(HINT32 id)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	HINT32 dev_id = 0;

	FuncEnter;

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(HINT32));
	if ( !packet ) return ERR_FAIL;
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + sizeof(HINT32));
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_WIFI_SCAN_START;
	packet->dlen = sizeof(HINT32);

	dev_id = id;
	HxSTD_MemCopy(packet->data, &dev_id, packet->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);

	HLIB_STD_MemFree(packet);

	return eRet;
}


HERROR NAPI_Wifi_ScanStop(HINT32 id)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	HINT32 dev_id = 0;

	FuncEnter;

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(HINT32));
	if ( !packet ) return ERR_FAIL;
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + sizeof(HINT32));
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_WIFI_SCAN_STOP;
	packet->dlen = sizeof(HINT32);

	dev_id = id;
	HxSTD_MemCopy(packet->data, &dev_id, packet->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);

	HLIB_STD_MemFree(packet);

	return eRet;
}

HERROR NAPI_Wifi_Connect(HINT32 id, const NAPIWifiAPInfo_t *info)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	NAPIProtocol_Wifi_t *wifi_info = NULL;
	HINT32 data_size = 0;

	FuncEnter;

	if( !info ) return ERR_FAIL;

	data_size = sizeof(NAPIProtocol_Wifi_t) + sizeof(NAPIProtocol_APInfo_t);
	wifi_info = HLIB_STD_MemAlloc(data_size);
	if ( !wifi_info ) return ERR_FAIL;
	HxSTD_MemSet(wifi_info, 0x0, data_size);
	wifi_info->dev_id = id;
	wifi_info->count = 1;

	napi_conv_cmd_apinfo(info, &wifi_info->ap[0]);

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + data_size);
	if ( !packet )
	{
		HLIB_STD_MemFree(wifi_info);
		return ERR_FAIL;
	}
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + data_size);
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_WIFI_CONNECT_AP;
	packet->dlen = data_size;

	HxSTD_MemCopy(packet->data, wifi_info, packet->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);

	if( wifi_info ) HLIB_STD_MemFree( wifi_info );
	if( packet ) HLIB_STD_MemFree(packet);

	FuncLeave;
	return eRet;
}

HERROR NAPI_Wifi_ConnectWPS(HINT32 id, eNapiWifiWPS eWPSType, HCHAR *szPinCode)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	NAPIProtocol_WPS_t *wps_info = NULL;
	HINT32 data_size = 0;

	FuncEnter;

	data_size = sizeof(NAPIProtocol_WPS_t);
	wps_info = HLIB_STD_MemAlloc(data_size);
	if ( !wps_info ) return ERR_FAIL;
	HxSTD_MemSet(wps_info, 0x0, data_size);
	wps_info->dev_id = id;
	wps_info->wps_type = eWPSType;
	HxSTD_MemCopy(wps_info->pin_code, szPinCode, NAPI_PROTO_WPS_PINCODE);

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + data_size);
	if ( !packet )
	{
		HLIB_STD_MemFree(wps_info);
		return ERR_FAIL;
	}
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + data_size);
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_WIFI_CONNECT_WPS;
	packet->dlen = data_size;

	HxSTD_MemCopy(packet->data, wps_info, packet->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);

	if( wps_info ) HLIB_STD_MemFree( wps_info );
	if( packet ) HLIB_STD_MemFree(packet);

	FuncLeave;
	return eRet;
}

HERROR NAPI_Wifi_Disconnect(HINT32 id)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	HINT32 dev_id = 0;

	FuncEnter;

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(HINT32));
	if ( !packet ) return ERR_FAIL;
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + sizeof(HINT32));
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_WIFI_DISCONNECT_AP;
	packet->dlen = sizeof(HINT32);

	dev_id = id;
	HxSTD_MemCopy(packet->data, &dev_id, packet->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);

	HLIB_STD_MemFree(packet);

	return eRet;
}

HERROR NAPI_Wifi_GetConnectedAPInfo(HINT32 id, NAPIWifiAPInfo_t *info)
{
	HERROR	eRet = ERR_OK;
	napicache_t *cache = NULL;
	NAPIWifiInfo_t *cache_info = NULL;

	FuncEnter;

	napi_agent_init_wait();

	if( !info ) return ERR_FAIL;
	HxSTD_MemSet(info, 0x00, sizeof(NAPIWifiAPInfo_t));

	cache = napi_cache_get_instance();
	if ( !cache ) return ERR_FAIL;

	cache_info = napi_cache_wifi_get(cache, id);
	if( !cache_info ) return ERR_FAIL;

	HxSTD_MemCopy(info->essid, cache_info->ap[0].essid, NAPI_AP_ESSID);
	HxSTD_MemCopy(info->apkey, cache_info->ap[0].apkey, NAPI_AP_KEY);
	HxSTD_MemCopy(info->macaddr, cache_info->ap[0].macaddr, NAPI_MAX_MACADDR);
	info->wave_strength = cache_info->ap[0].wave_strength;
	info->connect_speed = cache_info->ap[0].connect_speed;
	info->op_mode = cache_info->ap[0].op_mode;
	info->secure_type = cache_info->ap[0].secure_type;
	info->authen_type = cache_info->ap[0].authen_type;
	info->encrypt_type = cache_info->ap[0].encrypt_type;
	info->bIsConnected = cache_info->ap[0].bIsConnected;

	// debug
	napi_debug_log_ap(id, 1, info);

	FuncLeave;
	return eRet;
}

/**
* This synchonous function is used to configure WoWLAN feature
* @param[in] id: index of wifi network interface
* @param[in] mode: chosen mode
* @return ERR_OK if this WoWLAN feature is configured succesfully, ERR_FAIL if vice versa
*/

HERROR NAPI_Wifi_SetWakeupOnWLAN(HINT32 id, eNapiWifiWoWlan mode)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *pstRequestPacket = NULL;
	NAPIProtocol_WoWLAN_t stWowlanInfo;
	HERROR * eResult = NULL;
	FuncEnter;

	pstRequestPacket = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(NAPIProtocol_WoWLAN_t));
	if ( !pstRequestPacket ) return ERR_FAIL;
	HxSTD_MemSet(pstRequestPacket, 0x0, sizeof(napipacket_t) + sizeof(NAPIProtocol_WoWLAN_t));
	stWowlanInfo.dev_id = id;
	stWowlanInfo.mode = mode;
	pstRequestPacket->type = NAPI_PROTOTYPE_REQUEST;
	pstRequestPacket->code = NAPI_PROTO_WIFI_SET_WAKEUP_ON_WLAN;
	pstRequestPacket->sync = TRUE;
	pstRequestPacket->dlen = sizeof(NAPIProtocol_WoWLAN_t);

	HxSTD_MemCopy(pstRequestPacket->data, &stWowlanInfo, pstRequestPacket->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)pstRequestPacket, sizeof(napipacket_t)+pstRequestPacket->dlen);
	if(ERR_OK == eRet)
	{
		eRet = napi_agent_get_sync_call_result(pstRequestPacket, (void **) &eResult);
		if(ERR_OK == eRet)
		{
			eRet = *eResult;
		}
	}
	HLIB_STD_MemFree(pstRequestPacket);
	return eRet;

}

/**
* This synchonous function is used to get WPS pin code from DI layer
* @param[in] id: index of wifi network interface
* @param[out] szPincode: string of pin code
* @return ERR_OK if this function is called succesfully, ERR_FAIL if vice versa
*/


HERROR NAPI_Wifi_GetPincodeByWPS(HINT32 id, HCHAR *szPincode)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *pstRequestPacket = NULL;
	NAPIProtocol_WPS_t stWpsInfo;
	HCHAR * tempPincode = NULL;
	FuncEnter;

	pstRequestPacket = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(NAPIProtocol_WPS_t));
	if ( !pstRequestPacket ) return ERR_FAIL;
	HxSTD_MemSet(pstRequestPacket, 0x0, sizeof(napipacket_t) + sizeof(NAPIProtocol_WPS_t));
	HxSTD_MemSet(&stWpsInfo, 0x00, sizeof(NAPIProtocol_WPS_t));

	stWpsInfo.dev_id = id;
	pstRequestPacket->type = NAPI_PROTOTYPE_REQUEST;
	pstRequestPacket->code = NAPI_PROTO_WIFI_GET_WPS_PINCODE;
	pstRequestPacket->sync = TRUE;
	pstRequestPacket->dlen = sizeof(NAPIProtocol_WPS_t);

	HxSTD_MemCopy(pstRequestPacket->data, &stWpsInfo, pstRequestPacket->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)pstRequestPacket, sizeof(napipacket_t)+pstRequestPacket->dlen);
	if(ERR_OK == eRet)
	{
		eRet = napi_agent_get_sync_call_result(pstRequestPacket, (void **) &tempPincode);

		if(ERR_OK == eRet)
		{
			HxSTD_MemCopy(szPincode, tempPincode, NAPI_PROTO_WPS_PINCODE);
		}
	}
	HLIB_STD_MemFree(pstRequestPacket);
	return eRet;

}


