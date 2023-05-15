/** **********************************************************************************************************
	File 		: napi_agent.c
	author 		:
	comment		:
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

#include "napi_agent_common.h"
#include "napi_cache.h"
#include "napi_parser.h"
#include "napi_def.h"
#include "napi_conv.h"

#include "../napi_debug.h"

#include <vkernel.h>
#include <papi.h>

/*-----------------------------------------------------------------------------------------------
 *	pre-definitions
 *----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 *	internal
 *----------------------------------------------------------------------------------------------*/

#define CASE_EVT_ENUMTOSTR(a) \
						case a : HxLOG_Debug("received evt from napi : %s (%08x) \n", #a, a);break; \

/*-----------------------------------------------------------------------------------------------
 *	internal
 *----------------------------------------------------------------------------------------------*/

static napi_sync_call_info_t s_sync_call_obj =
{
	TRUE,	//.bDone = TRUE,
	0,		//.lCode = 0,
	NULL, 	//.pvResult = NULL,
};

napiagent_t *s_agent_obj = NULL;
napiagent_info_t *__agent_info(napiagent_t *agent)
{
	return (napiagent_info_t *)agent;
}

static void __agent_constructor(void) /* Dont use __attribute__((constructor)), __attribute__((destructor)) by napi_deinit() crash */
{
	if (s_agent_obj == NULL)
	{
	s_agent_obj = HLIB_STD_MemAlloc(sizeof(napiagent_info_t));
	}
	if(s_agent_obj != NULL)
		HxSTD_MemSet(s_agent_obj, 0, sizeof(napiagent_info_t));
}

static void __agent_destructor(void)
{
	if ( s_agent_obj )
	{
		HLIB_STD_MemFree(s_agent_obj);
		s_agent_obj = NULL;
	}
}


/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
static void __napi_agent_print_code(HINT32 code)
{
	switch(code)
	{
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_NETCONF );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_CM );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BLUETOOTH );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_NETCONF_CONN_SUCCESS );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_NETCONF_CONN_FAIL );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_NETCONF_AUTOIP_SUCCESS );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_NETCONF_AUTOIP_FAIL );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_NETCONF_LINK_CONNECTED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_NETCONF_LINK_DISCONNECTED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_NETCONF_STATUS_UPDATED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_NETCONF_DHCPSERVER_START );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_NETCONF_DHCPSERVER_STOP );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_INSERTED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_EXTRACTED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_SCANNING_START );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_SCANNING_END );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_SCANNING_OUTPUT );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_SCANNING_FAIL );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_KEY_INVALID );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_AP_CONNECT_SUCCESS );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_AP_CONNECT_FAIL );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_WPS_SUCCESS );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_WPS_FAIL );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_AP_DISCONNECTED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_AP_RECONNECTED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_AP_CONNECT_STOP );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_WIFI_LINK_UPDATED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_INSERTED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_EXTRACTED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_ENABLE );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_DISABLE );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_SCAN_SUCCESS );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_SCAN_FAIL );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_AUTHEN_SUCCESS );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_AUTHEN_FAIL );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_PASSKEY );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_CONNECT_SUCCESS );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_CONNECT_FAIL );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_REMOVED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_REMOTE_CMD );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_BT_MAX );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_PPPOE_CONNECTED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_PPPOE_DISCONNECTED );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_PPPOE_SET_INFO );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_PPPOE_GET_INFO );
		CASE_EVT_ENUMTOSTR( NAPI_PROTO_EVENT_PPPOE_SET_ONOFF );
		default: HxLOG_Debug("  (#)RECEIVED EVENT : ERROR !!!  \n",__FUNCTION__,__LINE__); break;
	}
}

static HERROR __napi_agent_connect_netconf(NAPIProtocol_Netconf_t *netconf_info)
{
	napipacket_t *packet = NULL;
	HINT32 packet_size = 0;


	if( !netconf_info ) return ERR_FAIL;

	HxLOG_Debug("id : %d \n", netconf_info->id);

	packet_size = sizeof(napipacket_t) + sizeof(NAPIProtocol_Netconf_t);
	packet = HLIB_STD_MemAlloc(packet_size);

	if ( NULL != packet )
	{
		HxSTD_MemSet(packet, 0x0, packet_size);
		packet->type = NAPI_PROTOTYPE_REQUEST;
		packet->code = NAPI_PROTO_SET_NET_INFO;
		packet->dlen = sizeof(NAPIProtocol_Netconf_t);

		HxSTD_MemCopy(packet->data, netconf_info, packet->dlen);
		napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, packet_size);
	}

	if( NULL != packet ) HLIB_STD_MemFree(packet);

	return ERR_OK;
}

static void __napi_agent_connect_ap(NAPIWifiInfo_t *info)
{
	NAPIProtocol_Wifi_t *wifi_info = NULL;
	HINT32 data_size = 0;
	napipacket_t *packet = NULL;
	FuncEnter;

	if( !info ) return;
	data_size = sizeof(NAPIProtocol_Wifi_t) + sizeof(NAPIProtocol_APInfo_t);
	wifi_info = HLIB_STD_MemAlloc(data_size);

	if( NULL != wifi_info )
	{
		HxSTD_MemSet(wifi_info, 0x00, data_size);

		wifi_info->dev_id = info->dev_id;
		wifi_info->count = 1;
		napi_conv_cmd_apinfo(&info->ap[0], &wifi_info->ap[0]);
		packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + data_size);

		if( NULL != packet )
		{
			HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + data_size);
			packet->type = NAPI_PROTOTYPE_REQUEST;
			packet->code = NAPI_PROTO_WIFI_CONNECT_AP;
			packet->dlen = data_size;

			HxSTD_MemCopy(packet->data, wifi_info, packet->dlen);

			napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t) + packet->dlen);
		}
	}

	if( packet ) HLIB_STD_MemFree(packet);
	if( wifi_info ) HLIB_STD_MemFree(wifi_info);

	FuncLeave;
}

static void __napi_agent_connect_pppoe(NAPIPPPOEInfo_t *info)
{
	NAPIProtocol_PPPOE_info_t *pppoe_info = NULL;
	HINT32 data_size = 0;
	napipacket_t *packet = NULL;
	FuncEnter;

	if( !info ) return;
	data_size = sizeof(NAPIProtocol_PPPOE_info_t) + sizeof(NAPIProtocol_PPPOE_info_t);
	pppoe_info = HLIB_STD_MemAlloc(data_size);
	if( !pppoe_info ) return;
	HxSTD_MemSet(pppoe_info, 0x00, data_size);

	napi_conv_cmd_pppoe(info, pppoe_info);

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + data_size);
	if ( !packet )
	{
		HLIB_STD_MemFree(pppoe_info);
		return;
	}

	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + data_size);
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_PPPOE_CONNECT;
	packet->dlen = data_size;

	HxSTD_MemCopy(packet->data, pppoe_info, packet->dlen);

	napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t) + packet->dlen);

	if( packet ) HLIB_STD_MemFree(packet);
	if( pppoe_info ) HLIB_STD_MemFree(pppoe_info);

	FuncLeave;
}


static HERROR __napi_agent_wifi_do_dhcp(HINT32 dev_id)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	NAPIProtocol_Netconf_t *netconf = NULL;
	NAPIDevInfo_t *dev_info = NULL;
	NAPINetConfInfo_t *info = NULL;

	FuncEnter;

	dev_info = napi_cache_devinfo_get(napi_cache_get_instance());
	if( !dev_info ) return ERR_FAIL;

	info = napi_cache_netconf_get(napi_cache_get_instance(),dev_id);
	if( !info ) return ERR_FAIL;

	netconf = HLIB_STD_MemAlloc(sizeof(NAPIProtocol_Netconf_t));
	if( NULL != netconf )
	{
		HxSTD_MemSet(netconf, 0x0, sizeof(*netconf));
		napi_conv_cmd_netconf(info, netconf);
		packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(NAPIProtocol_Netconf_t));
		if( NULL != packet )
		{

			HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + sizeof(NAPIProtocol_Netconf_t));
			packet->type = NAPI_PROTOTYPE_REQUEST;
			packet->code = NAPI_PROTO_SET_NET_INFO;
			packet->dlen = sizeof(NAPIProtocol_Netconf_t);

			HxSTD_MemCopy(packet->data, netconf, packet->dlen);
			eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);
		}
	}

	if( packet ) HLIB_STD_MemFree(packet);
	if( netconf ) HLIB_STD_MemFree(netconf);

	FuncLeave;
	return eRet;
}

static void __napi_agent_shutdown(void)
{
	napiagent_t *agent = NULL;
	napiagent_info_t *obj = NULL;
	NAPINetConfInfo_t *netconf_info = NULL;
	NAPIProtocol_cmd_bt_t *bt_cmd = NULL;
	NAPIBTInfo_t *bt_info = NULL;
	napipacket_t *packet = NULL;
	HINT32 count, i, packet_size = 0;

	FuncEnter;

	agent = napi_agent_get_instance();
	obj = __agent_info(agent);

	if( !obj ) return;

	count = napi_cache_netconf_get_count(napi_cache_get_instance());
	for( i=0; i<count; i++ )
	{
		netconf_info = napi_cache_netconf_get(napi_cache_get_instance(), i);
		if( !netconf_info ) break;

		if( netconf_info->active && netconf_info->linked )
		{
			packet_size = sizeof(napipacket_t) + sizeof(HINT32);
			packet = HLIB_STD_MemAlloc(packet_size);
			if( NULL != packet )
			{
				HxSTD_MemSet(packet, 0x0, packet_size);
				packet->type = NAPI_PROTOTYPE_REQUEST;
				packet->code = NAPI_PROTO_DISCONNECT;
				packet->dlen = sizeof(HINT32);
				HxSTD_MemCopy(packet->data, &netconf_info->id, packet->dlen);
				if( obj->system_status == NAPI_SYSTEM_STATUS_Operation )
				{
					napi_agent_write_data(agent, (HCHAR *)packet, packet_size);
				}
			}

			if( NULL != packet ) HLIB_STD_MemFree(packet);
		}

		if( netconf_info->type == eNAPI_NETCONF_WIFI )
		{
			packet_size = sizeof(napipacket_t) + sizeof(HINT32);
			packet = HLIB_STD_MemAlloc(packet_size);
			if( NULL != packet )
			{
				HxSTD_MemSet(packet, 0x0, packet_size);
				packet->type = NAPI_PROTOTYPE_REQUEST;
				packet->code = NAPI_PROTO_WIFI_DISCONNECT_AP;
				packet->dlen = sizeof(HINT32);
				HxSTD_MemCopy(packet->data, &netconf_info->id, packet->dlen);
				if( obj->system_status == NAPI_SYSTEM_STATUS_Operation )
				{
					napi_agent_write_data(agent, (HCHAR *)packet, packet_size);
				}
			}
			if( NULL != packet ) HLIB_STD_MemFree(packet);
		}

		if( netconf_info->type == eNAPI_NETCONF_PPPOE )
		{
			packet_size = sizeof(napipacket_t) + sizeof(HINT32);
			packet = HLIB_STD_MemAlloc(packet_size);
			if( NULL != packet )
			{
				HxSTD_MemSet(packet, 0x0, packet_size);
				packet->type = NAPI_PROTOTYPE_REQUEST;
				packet->code = NAPI_PROTO_PPPOE_DISCONNECT;
				packet->dlen = sizeof(HINT32);
				HxSTD_MemCopy(packet->data, &netconf_info->id, packet->dlen);
				if( obj->system_status == NAPI_SYSTEM_STATUS_Operation )
				{
					napi_agent_write_data(agent, (HCHAR *)packet, packet_size);
				}
			}
			if( NULL != packet ) HLIB_STD_MemFree(packet);
		}
	}

	bt_info = napi_cache_bt_get(napi_cache_get_instance());
	if( bt_info )
	{
		if( bt_info->enable )
		{
			bt_cmd = HLIB_STD_MemAlloc(sizeof(NAPIProtocol_cmd_bt_t));
			if( NULL != bt_cmd )
			{
				HxSTD_MemSet(bt_cmd, 0x00, sizeof(NAPIProtocol_cmd_bt_t));

				bt_cmd->enable = FALSE;

				packet_size = sizeof(napipacket_t) + sizeof(NAPIProtocol_cmd_bt_t);
				packet = HLIB_STD_MemAlloc(packet_size);

				if( NULL != packet )
				{
					HxSTD_MemSet(packet, 0x0, packet_size);
					packet->type = NAPI_PROTOTYPE_REQUEST;
					packet->code = NAPI_PROTO_BT_ENABLE;
					packet->dlen = sizeof(NAPIProtocol_cmd_bt_t);
					HxSTD_MemCopy(packet->data, bt_cmd, sizeof(NAPIProtocol_cmd_bt_t));

					napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, packet_size);
				}
			}

			if( NULL != bt_cmd ) HLIB_STD_MemFree(bt_cmd);
			if( NULL != packet ) HLIB_STD_MemFree(packet);
		}
	}

	obj->system_status = NAPI_SYSTEM_STATUS_Shutdown;
	FuncLeave;
	return;
}

static void __napi_agent_parser_update(napiagent_info_t *agent, HCHAR *data, HINT32 dlen, HINT32 code)
{
	HINT32 i = 0, dev_type = 0, dev_id = 0;
	NAPIProtocol_devinfo_t *info = NULL;
	NAPIDevInfo_t *dev_info = NULL;
	NAPINetConfInfo_t *net_info = NULL;
	NAPIWifiInfo_t *wifi_info = NULL;
	NAPIPPPOEInfo_t *pppoe_info = NULL;
	//NAPIBTInfo_t *bt_info = NULL;

	FuncEnter;

	info = (NAPIProtocol_devinfo_t *)data;
	if( !agent || !info) return;

	// device info update
	dev_info = (NAPIDevInfo_t *)HLIB_STD_MemAlloc(sizeof(NAPIDevInfo_t));
	if( NULL == dev_info )
	{
		return;
	}

	HxSTD_MemSet(dev_info, 0x00, sizeof(NAPIDevInfo_t));
	dev_info->default_dev = info->default_dev;
	dev_info->dev_count = info->dev_count;

	if( dev_info->default_dev < 0 )
	{
		dev_info->default_dev = 0;
	}

	if( dev_info->dev_count > 0 )
	{
		napi_cache_devinfo_update(napi_cache_get_instance(), dev_info);
	}

	if( NULL != dev_info)
	{
		HLIB_STD_MemFree(dev_info);
		dev_info = NULL;
	}

	if( !info->dev_count )
	{
		agent->bInit = TRUE;
	}
	else
	{

		// netconf update
		for( i=0; i<info->dev_count; i++ )
		{
			net_info = (NAPINetConfInfo_t *)HLIB_STD_MemAlloc(sizeof(*net_info));
			if( net_info )
			{
				HxSTD_MemSet(net_info, 0x00, sizeof(*net_info));
				napi_conv_event_netconf(&info->netconf[i], net_info);

				napi_cache_netconf_update(napi_cache_get_instance(), net_info);

				dev_type = net_info->type;
				dev_id = net_info->id;

				if( net_info )
				{
					HLIB_STD_MemFree(net_info);
				}

				// ap info update
				if( eNAPI_NETCONF_WIFI == dev_type )
				{
					wifi_info = (NAPIWifiInfo_t *)HLIB_STD_MemAlloc(sizeof(NAPIWifiInfo_t) + sizeof(NAPIWifiAPInfo_t));
					if( wifi_info )
					{
						HxSTD_MemSet(wifi_info, 0x00, sizeof(NAPIWifiInfo_t) + sizeof(NAPIWifiAPInfo_t));
						wifi_info->dev_id = dev_id;
						wifi_info->count = 1;

						napi_conv_event_apinfo(&info->apinfo,  &wifi_info->ap[0]);
						napi_cache_wifi_update(napi_cache_get_instance(), wifi_info);

						if( NULL != wifi_info )
						{
							HLIB_STD_MemFree(wifi_info);
						}
					}
					else
					{
						HxLOG_Debug("out of memory for net_info !! \n");
					}
				}
				else if( dev_type == eNAPI_NETCONF_PPPOE )
				{
					pppoe_info = (NAPIPPPOEInfo_t *)HLIB_STD_MemAlloc(sizeof(NAPIPPPOEInfo_t));
					if( pppoe_info )
					{
						HxSTD_MemSet(pppoe_info, 0x00, sizeof(NAPIPPPOEInfo_t));
						napi_conv_event_pppoe(&info->pppoeinfo, pppoe_info);
						napi_cache_pppoe_update_info(napi_cache_get_instance(), pppoe_info);
						napi_cache_pppoe_update_onoff(napi_cache_get_instance(), pppoe_info);

						if( pppoe_info )
						{
							HLIB_STD_MemFree(pppoe_info);
						}
					}
					else
					{
						HxLOG_Debug("out of memory for net_info !! \n");
					}
				}
			}
			else
			{
				HxLOG_Debug("out of memory for net_info !! \n");
			}
		}
	}

	agent->bInit = TRUE;

	FuncLeave;
}

static void __napi_agent_parser_dhcpserver(napiagent_info_t *agent, HCHAR *data, HINT32 dlen, HINT32 code)
{
	NAPIDHCPServerInfo_t *dhcpserver_info = NULL;
	FuncEnter;
	//2 #### dhcpserver 의 device id 를 0 으로.....  추후 수정 필요
	dhcpserver_info = (NAPIDHCPServerInfo_t *)HLIB_STD_MemAlloc(sizeof(NAPIDHCPServerInfo_t));
	HxSTD_MemSet(dhcpserver_info, 0x00, sizeof(NAPIDHCPServerInfo_t));
	if( code == NAPI_EVENT_NETCONF_DHCPSERVER_STOP ) dhcpserver_info->bActive = FALSE;
	else if( code == NAPI_PROTO_EVENT_NETCONF_DHCPSERVER_START ) dhcpserver_info->bActive = TRUE;

	napi_cache_dhcpserver_update(napi_cache_get_instance(), dhcpserver_info);
	napi_agent_notify_callback((napiagent_t *)agent, eAGENT_EVENT_TYPE_NETCONF, code, NULL);

	HLIB_STD_MemFree(dhcpserver_info);
	FuncLeave;
}

static void __napi_agent_parser_netconf(napiagent_info_t *agent, HCHAR *data, HINT32 dlen, HINT32 code)
{
	NAPIProtocol_Netconf_t *info = NULL;
	NAPINetConfInfo_t *netconf_info = NULL;

	FuncEnter;

	netconf_info = (NAPINetConfInfo_t *)HLIB_STD_MemAlloc(sizeof(NAPINetConfInfo_t));
	if( netconf_info && agent )
	{
		HxSTD_MemSet(netconf_info, 0x0, sizeof(*netconf_info));

		info = (NAPIProtocol_Netconf_t *)data;
		if( info )
		{
			napi_parser_netInfo_PtoN(info, netconf_info);
		}

		//napi_cache_netconf_update(napi_cache_get_instance(), netconf_info);
		napi_agent_notify_callback((napiagent_t *)agent, eAGENT_EVENT_TYPE_NETCONF, code, (void *)netconf_info);
	}
	else
	{
		HxLOG_Error("  (#) Error MemAlloc !!!\n");
	}

	if( netconf_info )
	{
		HLIB_STD_MemFree(netconf_info);
	}

	FuncLeave;
	return;
}

static void __napi_agent_parser_wifi(napiagent_info_t *agent, HCHAR *data, HINT32 dlen, HINT32 code)
{
	HINT32 i = 0;
	NAPIProtocol_Wifi_t *info = NULL;
	NAPIWifiInfo_t *wifi_info = NULL;
	NAPIWifiInfo_t *wifi_db = NULL;
	NAPINetConfInfo_t *net_info = NULL;

	FuncEnter;

	info = (NAPIProtocol_Wifi_t *)data;
	if( info && agent )
	{
		wifi_info = (NAPIWifiInfo_t *)HLIB_STD_MemAlloc(sizeof(NAPIWifiInfo_t) + sizeof(NAPIWifiAPInfo_t) * info->count);
		if( wifi_info )
		{
			HxSTD_MemSet(wifi_info, 0x00, sizeof(NAPIWifiInfo_t) + sizeof(NAPIWifiAPInfo_t) * info->count);
			wifi_info->dev_id = info->dev_id;
			wifi_info->count = info->count;
			for( i=0; i<wifi_info->count; i++ )
			{
				napi_conv_event_apinfo(&info->ap[i], &wifi_info->ap[i]);
			}

			switch( code )
			{
				case NAPI_EVENT_WIFI_SCANNING_END:
					{
						wifi_db = napi_cache_wifi_get(napi_cache_get_instance(), wifi_info->dev_id);
						if( NULL != wifi_db )
						{
							for( i=0; i<wifi_info->count; i++ )
							{
 								if( !HxSTD_StrCmp(wifi_info->ap[i].essid, wifi_db->ap[0].essid) &&
									!HxSTD_StrCmp(wifi_info->ap[i].macaddr, wifi_db->ap[0].macaddr))
								{
									wifi_info->ap[i].bIsConnected = TRUE;
 								}
							}
						}
					}
					break;
				case NAPI_EVENT_WIFI_AP_CONNECT_FAIL:
				case NAPI_EVENT_WIFI_KEY_INVALID:
					{
						/* work around */
						/* ap connect fail 시 web 의 connecting... 메시지 박스가 없어지지 않는다. */
						/* web 에서 수정이 필요하지만, 수정 되기 전 가지 NIMA 에서 Network fail 을 올려 줌 */
						net_info = napi_cache_netconf_get(napi_cache_get_instance(), wifi_info->dev_id);
						if( net_info )
						{
							napi_agent_notify_callback((napiagent_t *)agent,
								eAGENT_EVENT_TYPE_NETCONF, NAPI_EVENT_NETCONF_CONN_FAIL, (void *)net_info);
						}
					}
					break;
				default:
					break;
			}

			napi_agent_notify_callback((napiagent_t *)agent, eAGENT_EVENT_TYPE_WIFI, code, (void *)wifi_info);

			if( wifi_info )
			{
				HLIB_STD_MemFree(wifi_info);
			}
		}
		else
		{
			HxLOG_Error("  (#) Error MemAlloc !!!\n");
		}
	}
	else
	{
		HxLOG_Error("  (#) NO WIFI DATA !!!\n");
	}

	FuncLeave;
	return;
}

static void __napi_agent_bt_openprofile(void)
{
	/*empty*/
	return;
}

static void __napi_agent_parser_bt(napiagent_info_t *agent, napipacket_t *packet, HINT32 event)
{
	HINT32 info_size = 0, i = 0, index = 0;
	NAPIProtocol_bt_info_t *info = NULL;
	NAPIBTInfo_t *bt_info = NULL;
	NAPIBTInfo_t *bt_sort = NULL;
	NAPIBTInfo_t *bt_db = NULL;

	FuncEnter;

	if( !agent || !packet ) return;

	info_size = sizeof(NAPIBTInfo_t);
	bt_info = (NAPIBTInfo_t *)HLIB_STD_MemAlloc(info_size);
	if( !bt_info ) return;
	HxSTD_MemSet(bt_info, 0x00, info_size);

	if( packet->data ) info = (NAPIProtocol_bt_info_t *)packet->data;
	if( info )
	{
		napi_conv_event_btinfo(info, bt_info);

		// bluetooth device sort
		if( bt_info->dev_count > 0 )
		{
			bt_sort = HLIB_STD_MemAlloc(sizeof(*bt_sort));
			if( NULL != bt_sort )
			{
				HxSTD_MemSet(bt_sort, 0x00, sizeof(*bt_sort));
				HxSTD_MemCopy(bt_sort, bt_info, sizeof(*bt_sort));

				for( i=0; i<bt_info->dev_count; i++ )
				{
					if( eNAPI_BT_STATUS_Connected == bt_sort->dev_info[i].status )
					{
						HxSTD_MemCopy(&bt_info->dev_info[index++], &bt_sort->dev_info[i], sizeof(NAPIBTDevInfo_t));
					}
				}

				for( i=0; i<bt_info->dev_count; i++ )
				{
					if( eNAPI_BT_STATUS_Disconnected == bt_sort->dev_info[i].status )
					{
						HxSTD_MemCopy(&bt_info->dev_info[index++], &bt_sort->dev_info[i], sizeof(NAPIBTDevInfo_t));
					}
				}

				for( i=0; i<bt_info->dev_count; i++ )
				{
					if( eNAPI_BT_STATUS_NotPaired == bt_sort->dev_info[i].status )
					{
						HxSTD_MemCopy(&bt_info->dev_info[index++], &bt_sort->dev_info[i], sizeof(NAPIBTDevInfo_t));
					}
				}

				HLIB_STD_MemFree(bt_sort);
			}

		}

		bt_db = napi_cache_bt_get(napi_cache_get_instance());
		if( NULL != bt_db )
		{
			HxSTD_MemCopy(&bt_info->host_info, &bt_db->host_info, sizeof(bt_db->host_info));
		}

		napi_cache_bt_update(napi_cache_get_instance(), bt_info);
	}

	napi_agent_notify_callback((napiagent_t *)agent, eAGENT_EVENT_TYPE_BT, event, bt_info);

	if( bt_info ) HLIB_STD_MemFree(bt_info);

	// work around for web message box
	switch( event )
	{
		case NAPI_EVENT_DISCONNECTED:
		case NAPI_EVENT_CONNECTED:
		case NAPI_EVENT_CONNECT_FAILED:
		case NAPI_EVENT_REMOVED:
			{
				bt_info = NULL;
				bt_info = napi_cache_bt_get(napi_cache_get_instance());
				napi_agent_notify_callback((napiagent_t *)agent, eAGENT_EVENT_TYPE_BT, NAPI_EVENT_LIST_UPDATED, bt_info);
			}
			break;
		default:
			break;
	}

	FuncLeave;
	return;
}

static void __napi_agent_parser_pppoe(napiagent_info_t *agent, HCHAR *data, HINT32 dlen, HINT32 code)
{
	NAPIProtocol_PPPOE_info_t *info = NULL;
	NAPIPPPOEInfo_t *pppoe_info = NULL;

	FuncEnter;

	info = (NAPIProtocol_PPPOE_info_t *)data;
	if( info == NULL )
	{
		napi_agent_notify_callback((napiagent_t *)agent, eAGENT_EVENT_TYPE_PPPOE, code, NULL);
		return;
	}

	pppoe_info = (NAPIPPPOEInfo_t *)HLIB_STD_MemAlloc(sizeof(NAPIPPPOEInfo_t));
	if ( !pppoe_info ) return;

	HxSTD_MemSet(pppoe_info, 0x0, sizeof(NAPIPPPOEInfo_t));

	if( code == NAPI_EVENT_PPPOE_SET_ONOFF )
	{
		pppoe_info->dev_id = info->dev_id;
		pppoe_info->bOn = info->bOn;

		if ( napi_cache_pppoe_update_onoff(napi_cache_get_instance(), pppoe_info) != ERR_OK )
		{
			HLIB_STD_MemFree(pppoe_info);
			return;
		}
	}
	else
	{
		pppoe_info->dev_id = info->dev_id;
		HxSTD_MemCopy(pppoe_info->szUserID, info->szUserID, NAPI_PROTO_PPPOE_USER_ID_LEN_MAX - 1);
		HxSTD_MemCopy(pppoe_info->szUserPW, info->szUserPW, NAPI_PROTO_PPPOE_USER_PW_LEN_MAX - 1);

		if ( napi_cache_pppoe_update_info(napi_cache_get_instance(), pppoe_info) != ERR_OK )
		{
			HLIB_STD_MemFree(pppoe_info);
			return;
		}
	}

	napi_agent_notify_callback((napiagent_t *)agent, eAGENT_EVENT_TYPE_PPPOE, code, (void *)pppoe_info);
	HLIB_STD_MemFree(pppoe_info);

	FuncLeave;
}

void napi_agent_parser(napiagent_info_t *agent, napipacket_t *packet)
{
	HINT16 event_code = 0;
	FuncEnter;

	if( !packet ) return;

	__napi_agent_print_code(packet->code);

	switch(packet->code & 0xF000)
	{
		case NAPI_PROTO_EVENT_PPPOE:
			{
				event_code = (HINT16)(packet->code - NAPI_PROTO_EVENT_PPPOE);
				switch(packet->code)
				{
					case NAPI_PROTO_EVENT_PPPOE_CONNECTED:
					case NAPI_PROTO_EVENT_PPPOE_SET_ONOFF:
					case NAPI_PROTO_EVENT_PPPOE_SET_INFO:
						__napi_agent_parser_pppoe(agent, packet->data, packet->dlen, event_code);
						break;
					default:
						break;
				}
			}
			break;
		case NAPI_PROTO_EVENT_BLUETOOTH:
			{
				event_code = (HINT16)napi_conv_event_bt(packet->code);
				__napi_agent_parser_bt(agent, packet, event_code);
			}
			break;
		case NAPI_PROTO_EVENT_CM:
			{
			}
			break;
		case NAPI_PROTO_EVENT_WIFI:
			{
				event_code = packet->code - NAPI_PROTO_EVENT_WIFI;
				__napi_agent_parser_wifi(agent, packet->data, packet->dlen, event_code);
			}
			break;
		case NAPI_PROTO_EVENT_NETCONF:
			{
				event_code = packet->code - NAPI_PROTO_EVENT_NETCONF;
				switch(packet->code)
				{
					case NAPI_PROTO_EVENT_NETCONF_STATUS_UPDATED:
						__napi_agent_parser_update(agent, packet->data, packet->dlen, event_code);
						break;
					case NAPI_PROTO_EVENT_NETCONF_DHCPSERVER_STOP:
					case NAPI_PROTO_EVENT_NETCONF_DHCPSERVER_START:
						__napi_agent_parser_dhcpserver(agent, packet->data, packet->dlen, event_code);
						break;
					case NAPI_PROTO_EVENT_NETCONF_LINK_CONNECTED:
					case NAPI_PROTO_EVENT_NETCONF_LINK_DISCONNECTED:
					case NAPI_PROTO_EVENT_NETCONF_CONN_SUCCESS:
					case NAPI_PROTO_EVENT_NETCONF_CONN_FAIL:
					case NAPI_PROTO_EVENT_NETCONF_AUTOIP_SUCCESS:
					case NAPI_PROTO_EVENT_NETCONF_AUTOIP_FAIL:
						__napi_agent_parser_netconf(agent, packet->data, packet->dlen, event_code);
						break;
					default: break;
				}
			}
			break;
		default:
			break;
	}


	FuncLeave;
}

static void __napi_receivePacket(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	napiagent_info_t *agent = NULL;
	napipacket_t *packet = NULL;
	(void)nSocketHandle;
	(void)requestId;

	FuncEnter;


	agent = (napiagent_info_t *)userData;
	packet = HLIB_STD_MemAlloc(size);
	if ( !packet ) return;
	HxSTD_MemSet(packet, 0x0, size);

	HxSTD_MemCopy(packet, pvData, size);

	HxLOG_Debug("+++++++++++++ REPLY +++++++++++++++ \n");
	HxLOG_Debug("packet->sync: %s \n", packet->sync == TRUE?"true":"false");
	HxLOG_Debug("packet->code: %d \n", packet->code);
	HxLOG_Debug("packet->dlen: %d \n", packet->dlen);

	if(packet->sync)
	{
		if(packet->code == s_sync_call_obj.lCode && FALSE == s_sync_call_obj.bDone)
		{
			if(s_sync_call_obj.pvResult != NULL)
			{
				HLIB_STD_MemFree(s_sync_call_obj.pvResult);
				s_sync_call_obj.pvResult = NULL;
			}

			s_sync_call_obj.pvResult = HLIB_STD_MemAlloc(packet->dlen);
			HxSTD_MemCopy(s_sync_call_obj.pvResult, packet->data, packet->dlen);
			s_sync_call_obj.bDone = TRUE;
		}
	}
	else
	{
		napi_agent_parser(agent, packet);
	}

	HxLOG_Debug("  (#)event : %d (%x), data size : %d \n", packet->code, packet->code, size);

	if( packet ) HLIB_STD_MemFree(packet);
	FuncLeave;
}

static void __napi_changeStatus(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{

	FuncEnter;
	(void)userData;
	(void)eStatus;
	(void)nClientHandle;
	FuncLeave;
}

void napi_client(void *arg)
{
	napiagent_info_t *obj = NULL;
	FuncEnter;

	obj = (napiagent_info_t *)arg;
	if( obj == NULL )
		return;

	HLIB_SOCKET_Listen(obj->socket, TRUE, __napi_receivePacket, __napi_changeStatus, (void *)obj);

	FuncLeave;
	return;
}

napiagent_t *napi_agent_get_instance(void)
{
#ifndef USE_NAPI_CONSTRUCTOR
	if (s_agent_obj == NULL)
	{
		__agent_constructor();
	}
#endif
	return (napiagent_t *)s_agent_obj;
}
void napi_agent_create_instance(void)
{
#ifndef USE_NAPI_CONSTRUCTOR
	__agent_constructor();
#endif
}

void napi_agent_destroy_instance(void)
{
#ifndef USE_NAPI_CONSTRUCTOR
	__agent_destructor();
#endif
}

HERROR napi_agent_notify_callback(napiagent_t *agent, eAgentDeviceType type,
										HINT32 event, void* data)
{
	HERROR eRet = ERR_OK;
	napiagent_info_t *obj = NULL;
	FuncEnter;

	obj = __agent_info(agent);
	if(obj == NULL)
		return ERR_FAIL;

	HxLOG_Debug("  (#)[%s:%d] notify callback : event = %d, pData = %p\n",
		__FUNCTION__,__LINE__, event, data);
	if(obj->callbacks[type])
		obj->callbacks[type](event, data);

	FuncLeave;
	return eRet;
}

HERROR napi_agent_register_callback(napiagent_t *agent,
		napi_agent_callback callback, eAgentDeviceType type)
{
	HERROR eRet = ERR_OK;
	napiagent_info_t *obj = NULL;
	FuncEnter;

	obj = __agent_info(agent);
	if(obj == NULL)
		return ERR_FAIL;

	obj->callbacks[type] = callback;

	FuncLeave;
	return eRet;
}


/**
* This function is used to to get result of a sync call
* Maximum time to wait for the reply from NIMA is 5 seconds
* pvResult memory area MUST be freed after calling this function
* @param[in] packet: packet which is sent from NAPI to NIMA
* @param[out] pvResult: the pointer which point to the memory area which allocated for storing result
* @return ERR_OK if napi can receive result from NIMA, ERR_FAIL if vice versa
*/

HERROR napi_agent_get_sync_call_result(napipacket_t *packet, void ** pvResult)
{
	return napi_agent_get_sync_call_result_timeout(packet, NAPI_SYNC_CALL_TIMEOUT, pvResult);
}

/**
* This function is used to to get result of a sync call
* pvResult memory area MUST be freed after calling this function
* @param[in] packet: packet which is sent from NAPI to NIMA
* @param[in] lTimeout: time to wait for the reply from NIMA
* @param[in] packet: packet which is sent from NAPI to NIMA
* @param[out] pvResult: the pointer which point to the memory area which allocated for storing result
* @return ERR_OK if napi can receive result from NIMA, ERR_FAIL if vice versa
*/

HERROR napi_agent_get_sync_call_result_timeout(napipacket_t *packet, HINT32 lTimeout, void ** pvResult)
{
	HERROR 		eRet = ERR_OK;
	HUINT32 	starttick = 0;
	HUINT32 	duration = 0;

	/*	 __napi_receivePacket will check bDone varible and receive result from NIMA
		After receiving result, it will set this variable to TRUE */

	s_sync_call_obj.bDone = FALSE;
	s_sync_call_obj.lCode = packet->code;

	//Wait for reply message
	{
		starttick = HLIB_STD_GetSystemTick();
		while(1)
		{
			if(duration > lTimeout)
			{
				HxLOG_Debug("NAPI sync call code: [%d] timed out!\n", packet->code);
				s_sync_call_obj.bDone = TRUE;
				eRet = ERR_FAIL;
				break;
			}
			if(s_sync_call_obj.bDone)
			{
				HxLOG_Debug("NAPI sync call code: [%d] successful in %d ms!\n", packet->code, duration);
				*pvResult = (void *) s_sync_call_obj.pvResult;
				eRet = ERR_OK;
				break;
			}
			duration = HLIB_STD_GetSystemTick() - starttick;
			HLIB_STD_TaskSleep(10);
		}
	}
	return eRet;
}



