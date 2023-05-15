/** **********************************************************************************************************
	File 		: nima_server.c
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
#include "nima_command_procedure.h"
#include "nima_util.h"
#include "nima_protocol.h"
#include "nima_server.h"
#include "nima_db.h"
#include "nima_scenario.h"
#include "nima_conv.h"
#include "nima_debug.h"

#include "ncapi.h"

#include <vkernel.h>
#include <hlib.h>
#include <stdio.h>
#include <unistd.h>
#include <papi.h>

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


typedef enum {
	SERVER_MSGQ_SIZE		= 32,
	SERVER_MSGQ_TIMEOUT	= 500
} eGlobalNimaServer;

typedef struct _nimaserverobj_t {
	HINT32		magic_code;
	HINT32		hsock;
	HULONG		tid;
	HULONG		tid_callback;
	HULONG		tid_connect;
	HULONG		msgq;
	HBOOL		activate;
	HxList_t	*sessions;
	HCHAR		uds_path[MIMA_SERV_MAX_PATH];
	HUINT8 		ucShutdownDelayCount;
} nimaserverobj_t;

typedef enum
{
	eNIMA_CB_INIT,
	eNIMA_CB_NETCONF,
	eNIMA_CB_WIFI,
	eNIMA_CB_BT,
	eNIMA_CB_PPPOE,
} eNimaCallbackEventType;

typedef struct _nimacallbackmsg{
	eNimaCallbackEventType type;
	HINT32 event;
	HINT32 device_id;
	HINT32 count;
	void *data;
} nimacallmsg_t;

typedef struct _nimamsg_t {
	HCHAR		*data;
	HINT32		len;
} nimamsg_t;

#define MAXIMUM_SHUTDOWN_DELAY 20 /* 20 times - about 2 minutes */

/*-----------------------------------------------------------------------------------------------
 *	internal functions
 *----------------------------------------------------------------------------------------------*/
static nimaserverobj_t *s_nimaserver;
static HINT32 s_etPowerStatus = ePAMA_STATUS_Undefined;
static HBOOL s_enableAutoConnect = TRUE;
static HBOOL s_finishAutoConnect = FALSE;

static inline nimaserverobj_t *__obj(nimaserver_t *server)
{
	return (nimaserverobj_t *)server;
}

static void __nima_server_run_packet(nimapacket_t *data, HINT32 dlen)
{
	nimamsg_t msg = {0,};
	HxList_t *pos = HLIB_LIST_First(s_nimaserver->sessions);

	msg.len = dlen;

	while ( pos )
	{
		nimasession_t *session = HLIB_LIST_Data(pos);
		if ( session == NULL ) return;
		msg.data = HLIB_MEM_MemDup(data, dlen);
		nima_session_push(session, (HCHAR*)&msg, sizeof(msg));
		pos = HLIB_LIST_Next(pos);
	}
}

static void __nima_server_get_deviceinfo(NIMAProtocol_devinfo_t *devinfo)
{
	HINT32 i = 0;
	NCAPI_NetConfInfo_t *net_info = NULL;
	NCAPI_BTDevInfo_t bt_info;

	FuncEnter;

	if( NULL == devinfo )
	{
		return;
	}
	
	NCAPI_NetConf_GetDeviceCount(&devinfo->dev_count);
	NCAPI_NetConf_GetDefaultDevIndex(&devinfo->default_dev);

	net_info = (NCAPI_NetConfInfo_t *)HLIB_STD_MemAlloc(sizeof(*net_info));
	if (NULL == net_info)
	{
		HxLOG_Critical("HLIB_STD_MemAlloc() failed\n");
		return;
	}
	
	for (i=0; i < devinfo->dev_count; i++)
	{
		HxSTD_MemSet(net_info, 0x00, sizeof(*net_info));
		NCAPI_NetConf_GetDeviceInfo(i, net_info);
		nima_conv_netconf(net_info, &devinfo->netconf[i]);
		switch (net_info->type)
		{
		case eNIMA_NETCONF_WIFI:
		{
			HINT32 connected_ap_count = -1;
			NCAPI_WifiAPInfo_t connected_ap[1];
			HxSTD_MemSet(&connected_ap[0], 0x00, sizeof(connected_ap[0]));
			NCAPI_Wifi_GetConnectedAPInfo(net_info->id, &connected_ap_count, connected_ap);
			nima_conv_apinfo(&connected_ap[0], &devinfo->apinfo);
			break;
		}
		case eNIMA_NETCONF_PPPOE:
		{
			NCAPI_PPPoEInfo_t pppoe_info;
			NIMAProtocol_devinfo_t tmp_dev_info;

			HxSTD_MemSet(&pppoe_info, 0x00, sizeof(NCAPI_PPPoEInfo_t));
			HxSTD_MemSet(&tmp_dev_info, 0x00, sizeof(NIMAProtocol_devinfo_t));

			NCAPI_PPPoE_GetInfo(pppoe_info.id, &pppoe_info);

			/* pppoe on/off information has only db.
			   nico on/off information is just context. */
			tmp_dev_info.dev_count = devinfo->dev_count;
			tmp_dev_info.netconf[i].id = i;
			tmp_dev_info.netconf[i].type = eNIMA_NETCONF_PPPOE;
			nima_db_load(&tmp_dev_info);
			pppoe_info.enable = tmp_dev_info.pppoeinfo.bOn;
			nima_conv_pppoe(&pppoe_info, &devinfo->pppoeinfo);
			break;
		}
		default:
			break;
		}
	}
	NIMA_SAFE_DELETE(net_info);

	NCAPI_BT_GetHostInfo(&bt_info);
	{
		nima_conv_bt_devinfo(&bt_info, &devinfo->btinfo.host_info);
	}

	FuncLeave;
	return;
}

static void __nima_server_update_clientcache(NIMAProtocol_devinfo_t *devinfo)
{
	nimapacket_t *command_packet = NULL;
	FuncEnter;

	if( NULL != devinfo )
	{
		command_packet = nima_do_encode_packet(NIMA_PROTOTYPE_REPLY,
												NIMA_EVENT_NETCONF_STATUS_UPDATED,
												sizeof(*devinfo),
												devinfo);

		__nima_server_run_packet(command_packet, sizeof(*command_packet)+command_packet->dlen);

		NIMA_SAFE_DELETE(command_packet);
	}

	FuncLeave;
}

/**
* This function is used to update the network status (static variable in nima_scenerio.c)
* @param[in] eNimaEvent event
* @param[in] NIMAProtocol_devinfo_t *devinfo
* @return void
*/

static void __nima_server_update_network_status(eNimaEvent event, NIMAProtocol_devinfo_t *devinfo)
{
	if(NULL != devinfo)
	{
		HINT32 i = 0;
		HBOOL bIsConnected = FALSE;
		switch(event)
		{
			case NIMA_EVENT_NETCONF_CONN_SUCCESS:
				nima_scenario_set_net_status(NIMA_NETWORK_CONNECTED);
				break;

			case NIMA_EVENT_NETCONF_LINK_DISCONNECTED:
			case NIMA_EVENT_NETCONF_CONN_FAIL:
			case NIMA_EVENT_WIFI_AP_CONNECT_FAIL:
			case NIMA_EVENT_WIFI_WPS_FAIL:
				for(i=0; i<devinfo->dev_count; i++ )
				{
					if(devinfo->netconf[i].linked && devinfo->netconf[i].active)
					{
						bIsConnected = TRUE;
						break;
					}
				}
				if(!bIsConnected)
				{
					nima_scenario_set_net_status(NIMA_NETWORK_DISCONNECTED);
				}
				break;
			default:
				break;
		}
	}
}

static const HCHAR * __nima_event_to_string(eNimaEvent evt)
{
	switch (evt) {
	case NIMA_EVENT_NETCONF_CONN_SUCCESS: return "NIMA_EVENT_NETCONF_CONN_SUCCESS";
	case NIMA_EVENT_NETCONF_CONN_FAIL: return "NIMA_EVENT_NETCONF_CONN_FAIL";
	case NIMA_EVENT_NETCONF_AUTOIP_SUCCESS: return "NIMA_EVENT_NETCONF_AUTOIP_SUCCESS";
	case NIMA_EVENT_NETCONF_AUTOIP_FAIL: return "NIMA_EVENT_NETCONF_AUTOIP_FAIL";
	case NIMA_EVENT_NETCONF_LINK_CONNECTED: return "NIMA_EVENT_NETCONF_LINK_CONNECTED";
	case NIMA_EVENT_NETCONF_LINK_DISCONNECTED: return "NIMA_EVENT_NETCONF_LINK_DISCONNECTED";
	case NIMA_EVENT_NETCONF_STATUS_UPDATED: return "NIMA_EVENT_NETCONF_STATUS_UPDATED";
	case NIMA_EVENT_NETCONF_DHCPSERVER_START: return "NIMA_EVENT_NETCONF_DHCPSERVER_START";
	case NIMA_EVENT_NETCONF_DHCPSERVER_STOP: return "NIMA_EVENT_NETCONF_DHCPSERVER_STOP";
	case NIMA_EVENT_WIFI_INSERTED: return "NIMA_EVENT_WIFI_INSERTED";
	case NIMA_EVENT_WIFI_EXTRACTED: return "NIMA_EVENT_WIFI_EXTRACTED";
	case NIMA_EVENT_WIFI_SCANNING_START: return "NIMA_EVENT_WIFI_SCANNING_START";
	case NIMA_EVENT_WIFI_SCANNING_END: return "NIMA_EVENT_WIFI_SCANNING_END";
	case NIMA_EVENT_WIFI_SCANNING_OUTPUT: return "NIMA_EVENT_WIFI_SCANNING_OUTPUT";
	case NIMA_EVENT_WIFI_SCANNING_FAIL: return "NIMA_EVENT_WIFI_SCANNING_FAIL";
	case NIMA_EVENT_WIFI_KEY_INVALID: return "NIMA_EVENT_WIFI_KEY_INVALID";
	case NIMA_EVENT_WIFI_AP_CONNECT_SUCCESS: return "NIMA_EVENT_WIFI_AP_CONNECT_SUCCESS";
	case NIMA_EVENT_WIFI_AP_CONNECT_FAIL: return "NIMA_EVENT_WIFI_AP_CONNECT_FAIL";
	case NIMA_EVENT_WIFI_WPS_SUCCESS: return "NIMA_EVENT_WIFI_WPS_SUCCESS";
	case NIMA_EVENT_WIFI_WPS_FAIL: return "NIMA_EVENT_WIFI_WPS_FAIL";
	case NIMA_EVENT_WIFI_AP_DISCONNECTED: return "NIMA_EVENT_WIFI_AP_DISCONNECTED";
	case NIMA_EVENT_WIFI_AP_RECONNECTED: return "NIMA_EVENT_WIFI_AP_RECONNECTED";
	case NIMA_EVENT_WIFI_AP_CONNECT_STOP: return "NIMA_EVENT_WIFI_AP_CONNECT_STOP";
	case NIMA_EVENT_WIFI_LINK_UPDATED: return "NIMA_EVENT_WIFI_LINK_UPDATED";
	case NIMA_EVENT_BT_INSERTED: return "NIMA_EVENT_BT_INSERTED";
	case NIMA_EVENT_BT_EXTRACTED: return "NIMA_EVENT_BT_EXTRACTED";
	case NIMA_EVENT_BT_ENABLE: return "NIMA_EVENT_BT_ENABLE";
	case NIMA_EVENT_BT_DISABLE: return "NIMA_EVENT_BT_DISABLE";
	case NIMA_EVENT_BT_SCAN_SUCCESS: return "NIMA_EVENT_BT_SCAN_SUCCESS";
	case NIMA_EVENT_BT_SCAN_FAIL: return "NIMA_EVENT_BT_SCAN_FAIL";
	case NIMA_EVENT_BT_AUTHEN_SUCCESS: return "NIMA_EVENT_BT_AUTHEN_SUCCESS";
	case NIMA_EVENT_BT_AUTHEN_FAIL: return "NIMA_EVENT_BT_AUTHEN_FAIL";
	case NIMA_EVENT_BT_PASSKEY: return "NIMA_EVENT_BT_PASSKEY";
	case NIMA_EVENT_BT_CONNECT_SUCCESS: return "NIMA_EVENT_BT_CONNECT_SUCCESS";
	case NIMA_EVENT_BT_CONNECT_FAIL: return "NIMA_EVENT_BT_CONNECT_FAIL";
	case NIMA_EVENT_BT_REMOVED: return "NIMA_EVENT_BT_REMOVED";
	case NIMA_EVENT_BT_REMOTE_CMD: return "NIMA_EVENT_BT_REMOTE_CMD";
	case NIMA_EVENT_BT_MAX: return "NIMA_EVENT_BT_MAX";
	case NIMA_EVENT_PPPOE_CONNECTED: return "NIMA_EVENT_PPPOE_CONNECTED";
	case NIMA_EVENT_PPPOE_DISCONNECTED: return "NIMA_EVENT_PPPOE_DISCONNECTED";
	case NIMA_EVENT_PPPOE_SET_INFO: return "NIMA_EVENT_PPPOE_SET_INFO";
	case NIMA_EVENT_PPPOE_GET_INFO: return "NIMA_EVENT_PPPOE_GET_INFO";
	case NIMA_EVENT_PPPOE_SET_ONOFF: return "NIMA_EVENT_PPPOE_SET_ONOFF";
	default:
		break;
	}

	return "(unknown)";
}

static HBOOL __nima_server_is_default_dev_is_lan(NIMAProtocol_devinfo_t * devinfo)
{
	if (devinfo == NULL)
	{
		return FALSE;
	}

	return (devinfo->netconf[devinfo->default_dev].type == eNIMA_NETCONF_LAN) ? TRUE : FALSE;
}

static HINT32 __nima_server_get_wlan_dev_id(NIMAProtocol_devinfo_t * devinfo)
{
	HINT32 id;
	if (devinfo == NULL)
	{
		return -1;
	}

	for (id = 0; id < devinfo->dev_count; id++)
	{
		if (devinfo->netconf[id].type == eNIMA_NETCONF_WIFI)
		{
			return id;
		}
	}
	return -1;
}

/** DB Load는 이전 접속 정보가 필요할 경우 do scenario 에서만 수행 */
static void __nima_server_do_scenario(nimapacket_t *packet, NIMAProtocol_devinfo_t *devinfo)
{
	if( (NULL != packet) && (NULL != devinfo))
	{
		NIMAProtocol_devinfo_t *db_devinfo = NULL;
		db_devinfo = HLIB_STD_MemAlloc(sizeof(*db_devinfo));
		HxLOG_Warning("NIMA received the event: %s(%x)\n", __nima_event_to_string(packet->code), packet->code);

		if(NULL != db_devinfo)
		{

			HxSTD_MemSet(db_devinfo, 0x00, sizeof(*db_devinfo));
			HxSTD_MemCopy(db_devinfo, devinfo, sizeof(*db_devinfo));
			nima_db_load(db_devinfo);

			switch(packet->code)
			{
				case NIMA_EVENT_NETCONF_LINK_CONNECTED:
					nima_scenario_apply(db_devinfo);
					break;

				case NIMA_EVENT_WIFI_INSERTED:
					nima_scenario_apply(db_devinfo);
					break;

				case NIMA_EVENT_NETCONF_LINK_DISCONNECTED:
					NCAPI_Netconf_Renew(NIMA_NETCONF_RENEW_SHPATH); /* Update iptables PREROUTING rules for LXC models*/
					nima_scenario_apply(db_devinfo);
					break;

				case NIMA_EVENT_BT_INSERTED:
					nima_scenario_apply_bluetooth(db_devinfo);
					break;

				case NIMA_EVENT_PPPOE_SET_ONOFF:
					nima_scenario_apply_pppoe(db_devinfo);
					break;

				case NIMA_EVENT_PPPOE_SET_INFO:
					db_devinfo->pppoeinfo.bOn = devinfo->pppoeinfo.bOn;
					nima_scenario_apply_pppoe(db_devinfo);
					break;

				case NIMA_EVENT_NETCONF_CONN_SUCCESS:
					NCAPI_Netconf_Renew(NIMA_NETCONF_RENEW_SHPATH);
					if (__nima_server_is_default_dev_is_lan(devinfo) == TRUE)
					{
						HINT32 id = __nima_server_get_wlan_dev_id(devinfo);
						if (id >= 0)
						{
							NCAPI_Wifi_Disconnect(id);
							NCAPI_NetConf_DevDown(id);
						}
					}
					break;

				case NIMA_EVENT_NETCONF_CONN_FAIL:
					NCAPI_Netconf_Renew(NIMA_NETCONF_RENEW_SHPATH);
					break;

				case NIMA_EVENT_WIFI_AP_CONNECT_FAIL:
				case NIMA_EVENT_WIFI_WPS_FAIL:
					{
						NIMAProtocol_Wifi_t *wifi_info = (NIMAProtocol_Wifi_t *)packet->data;
						if( NULL != wifi_info )
						{
							NCAPI_NetConf_StopConnect(wifi_info->dev_id);

						}
					}
					break;

				case NIMA_EVENT_WIFI_AP_CONNECT_SUCCESS:
				case NIMA_EVENT_WIFI_WPS_SUCCESS:
					{
						NIMAProtocol_Wifi_t *wifi_info = (NIMAProtocol_Wifi_t *)packet->data;
						if( NULL != wifi_info )
						{
							nima_scenario_apply_by_devid(wifi_info->dev_id, db_devinfo);
						}

					}
					break;

				default:
					break;
			}
			__nima_server_update_network_status(packet->code, db_devinfo);

			HLIB_STD_MemFree(db_devinfo);
		}
	}
}

/** DB Save 는 update db 함수에서만 사용 */
static void __nima_server_updata_db(nimapacket_t *packet, NIMAProtocol_devinfo_t *devinfo)
{
	if( (NULL != packet) && (NULL != devinfo) )
	{
		switch( packet->code )
		{
			case NIMA_EVENT_NETCONF_CONN_SUCCESS:
			case NIMA_EVENT_NETCONF_CONN_FAIL:
				{
					nima_db_save(packet->data, NIMA_DB_NETCONF_CONNECTED_INFO);
				}
				break;
			case NIMA_EVENT_WIFI_AP_CONNECT_SUCCESS:
			case NIMA_EVENT_WIFI_WPS_SUCCESS:
				{
					nima_db_save(packet->data, NIMA_DB_WIFI_CONNECTED_INFO);
				}
				break;
			case NIMA_EVENT_PPPOE_CONNECTED:
			case NIMA_EVENT_PPPOE_SET_INFO:
				{
					nima_db_save(packet->data, NIMA_DB_PPPOE_CONNECTED_INFO);
				}
				break;
			case NIMA_EVENT_PPPOE_SET_ONOFF:
				{
					nima_db_save(packet->data, NIMA_DB_PPPOE_SET_ONOFF);
				}
				break;
			case NIMA_EVENT_BT_INSERTED:
				{
					nima_db_save(packet->data, NIMA_DB_BT_INFO);
				}
			default:
				break;
		}
	}
}

static void __nima_server_shutdown(void)
{
	NIMAProtocol_devinfo_t *devinfo = NULL;

	devinfo = HLIB_STD_MemAlloc(sizeof(*devinfo));
	if( NULL != devinfo )
	{
		HxSTD_MemSet(devinfo, 0x00, sizeof(*devinfo));

		__nima_server_get_deviceinfo(devinfo);
		nima_db_load(devinfo);
		nima_scenario_shutdown((void *)devinfo);

		NIMA_SAFE_DELETE(devinfo);
	}

	return;
}

static void __nima_server_operation(void)
{
	NIMAProtocol_devinfo_t *devinfo = NULL;

	devinfo = HLIB_STD_MemAlloc(sizeof(*devinfo));
	if( NULL != devinfo )
	{
		HxSTD_MemSet(devinfo, 0x00, sizeof(*devinfo));

		__nima_server_get_deviceinfo(devinfo);
		nima_db_load(devinfo);
		nima_scenario_operation((void *)devinfo);

		NIMA_SAFE_DELETE(devinfo);
	}
	return;
}


static void __nima_server_init_session(void)
{
	NIMAProtocol_devinfo_t *devinfo = NULL;
	HBOOL is_session_initall = TRUE;
	HUINT32	i = 0;

	HxList_t *pos = HLIB_LIST_First(s_nimaserver->sessions);
	while ( pos )
	{
		nimasession_t *session = HLIB_LIST_Data(pos);
		if ( session == NULL ) return;
		if ( !session->is_init ) is_session_initall = FALSE;
		pos = HLIB_LIST_Next(pos);
	}

	if( !is_session_initall )
	{
		devinfo = HLIB_STD_MemAlloc(sizeof(*devinfo));
		if( NULL != devinfo )
		{
			HxSTD_MemSet(devinfo, 0x00, sizeof(*devinfo));

			__nima_server_get_deviceinfo(devinfo);

			for(i = 0; i < devinfo->dev_count; i++)
			{
				devinfo->netconf[i].dhcp = TRUE;
			}

			__nima_server_update_clientcache(devinfo);
			//__nima_server_do_scenario(devinfo);

			NIMA_SAFE_DELETE(devinfo);
		}

		pos = HLIB_LIST_First(s_nimaserver->sessions);
		while ( pos )
		{
			nimasession_t *session = HLIB_LIST_Data(pos);
			if ( session == NULL ) return;
			session->is_init = TRUE;
			pos = HLIB_LIST_Next(pos);
		}
	}

	return;
}


// nico 로 부터 받은 EVENT 를 session 으로 보내기 위해 사용
static void __nima_server_broadcast_session(nimapacket_t *data, HINT32 dlen)
{
	NIMAProtocol_devinfo_t *devinfo = NULL;
	FuncEnter;

	if ( NULL != s_nimaserver || NULL != data )
	{
		devinfo = HLIB_STD_MemAlloc(sizeof(*devinfo));
		if( NULL != devinfo )
		{
			HxSTD_MemSet(devinfo, 0x00, sizeof(*devinfo));
			__nima_server_get_deviceinfo(devinfo);
			__nima_server_update_clientcache(devinfo);
			__nima_server_updata_db(data, devinfo);
			__nima_server_do_scenario(data, devinfo);
			__nima_server_run_packet(data, dlen);
			NIMA_SAFE_DELETE(devinfo);
		}
	}

	FuncLeave;
}

static void __nima_server_recv_callback(void *user, HINT32 sock, HUINT32 reqid,
					HINT32 size, void *data)
{
	nimamsg_t msg = {0, };
	nimasession_t *session = NULL;
	nimaserver_t *server = NULL;

	FuncEnter;

	server = (nimaserver_t *)user;
	msg.data = (char *)HLIB_STD_MemAlloc(size);

	if(msg.data != NULL)
	{
		HxSTD_memcpy(msg.data, data, size);

		msg.len = size;
		session = nima_server_find_session(server, sock);
		if ( session )
			nima_session_push(session, (HCHAR *)&msg, sizeof(msg));
	}

	FuncLeave;
}

static void __nima_server_create_session(nimaserver_t *server, HINT32 sock)
{
	nimasession_t *session = NULL;

	FuncEnter;

	session = nima_server_create_session(server);
	if ( session )
		nima_session_start(session, sock, 0);

	FuncLeave;
}

static void __nima_server_destroy_session(nimaserver_t *server, HINT32 sock)
{

	nimasession_t *session = NULL;

	FuncEnter;

	session = nima_server_find_session(server, sock);
	if ( session )
		nima_server_destroy_session(server, session);

	FuncLeave;
}

static void __nima_server_client_connected(nimaserver_t *server, HINT32 sock)
{
	nimacallmsg_t msg = {0,};

	FuncEnter;
	msg.type = eNIMA_CB_INIT;
	VK_MSG_Send(__obj(s_nimaserver)->msgq, (void *)&msg, sizeof(nimacallmsg_t));
	FuncLeave;
}

static void __nima_server_status_callback(void *user, HxSocket_Status_e status, HINT32 clnt_sock)
{
	nimaserver_t *server = NULL;

	FuncEnter;

	server = (nimaserver_t *)user;
	if ( !server ) return;

	switch ( status )
	{
		case HxSOCKET_ADD_CLIENT:
			__nima_server_create_session(server, clnt_sock);
			__nima_server_client_connected(server, clnt_sock);
			return;
		case HxSOCKET_REMOVE_CLIENT:
		case HxSOCKET_DISCONNECT_CLIENT:
			return __nima_server_destroy_session(server, clnt_sock);
		default:
			break;
	}

	FuncLeave;
}

STATIC HERROR __nima_server_onFactoryDefault(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HERROR hRet = ERR_OK;
	hRet = HAPI_RemoveRegistry();
	hRet = NCAPI_BT_Remove_DB();

	return hRet;
}

STATIC HERROR	__nima_server_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	PxSTATUS_e	eStatus;
	eNimaNetworkStatus eNetworkStatus = nima_scenario_get_net_status();
	HxLOG_Assert(nArgc == 1);
	eStatus = HxOBJECT_INT(apArgv[0]);
	FuncEnter;

	HxLOG_Warning("received system event [%x, previous status:%x], eNetworkStatus: %d\n", eStatus, s_etPowerStatus, eNetworkStatus);
	switch(eStatus)
	{
		case ePAMA_STATUS_Shutdown:
			{
				HxLOG_Warning("received system event Shutdown [sig:%x] \n", eStatus);
				if(NIMA_NETWORK_CONNECTING == eNetworkStatus && __obj(s_nimaserver)->ucShutdownDelayCount < MAXIMUM_SHUTDOWN_DELAY)
				{
					PAPI_WaitForShutDown(PxDEFAULT_WAIT);
					__obj(s_nimaserver)->ucShutdownDelayCount++;
				}

				if(ePAMA_STATUS_Shutdown != s_etPowerStatus)
				{
					if((ePAMA_STATUS_ShutdownDone == s_etPowerStatus) || (ePAMA_STATUS_RebootDone == s_etPowerStatus))
					{
						__nima_server_operation();
					}

					s_etPowerStatus = ePAMA_STATUS_Shutdown;
					/* TODO: check BLUETOOTH scenario for DAC off */
				}
			}
			break;
		case ePAMA_STATUS_RebootDone: /* divided with ShutdownDone because of enabling S3MODE_FASTBOOT */
			{
				HxLOG_Warning("received system event RebootDone [sig:%x] \n", eStatus);

				nima_set_enable_auto_connect(FALSE);

#if defined(CONFIG_USES_NFSROOT)
				;
#else
				if(ePAMA_STATUS_RebootDone != s_etPowerStatus)
				{
					 if(ePAMA_STATUS_ShutdownDone != s_etPowerStatus)
					 {
					 	__nima_server_shutdown();
					 }
					 nima_scenario_set_net_status(NIMA_NETWORK_DISCONNECTED);
					 s_etPowerStatus = ePAMA_STATUS_RebootDone;
				}
#endif
			}
			break;
		case ePAMA_STATUS_ShutdownDone:
			{
				HxLOG_Warning("received system event ShutdownDone [sig:%x] \n", eStatus);

				nima_set_enable_auto_connect(FALSE);
				
#if defined(CONFIG_USES_NFSROOT)
				;
#else
				if(ePAMA_STATUS_ShutdownDone != s_etPowerStatus)
				{
					__nima_server_shutdown();
					nima_scenario_set_net_status(NIMA_NETWORK_DISCONNECTED);
					s_etPowerStatus = ePAMA_STATUS_ShutdownDone;
				}
#endif
			}
			break;
		case ePAMA_STATUS_Operation:
			HxLOG_Warning("received system event ePAMA_STATUS_Operation [sig:%x] \n", eStatus);

			nima_set_enable_auto_connect(TRUE);
			
			if (ePAMA_STATUS_Operation != s_etPowerStatus)
			{
				__obj(s_nimaserver)->ucShutdownDelayCount = 0;
				if(ePAMA_STATUS_Shutdown != s_etPowerStatus)
				{
					__nima_server_operation();
				}
				s_etPowerStatus = ePAMA_STATUS_Operation;
			}
			break;
		default:
			break;
	}
	FuncLeave;
	return ERR_OK;
}

static void __nima_ncapi_callback_netconf(eNcapiNetConfEvent event, HINT32 index, NCAPI_NetConfInfo_t *data)
{
	nimacallmsg_t msg = {0,};

	FuncEnter;
	msg.type = eNIMA_CB_NETCONF;
	msg.event = event;
	msg.device_id = index;
	msg.data = HLIB_MEM_MemDup(data, sizeof(NCAPI_NetConfInfo_t));

	VK_MSG_Send(__obj(s_nimaserver)->msgq, (void *)&msg, sizeof(nimacallmsg_t));
	FuncLeave;
}

static void __nima_ncapi_callback_wifi(eNcapiWifiEvent event, HINT32 device_id, HINT32 count, NCAPI_WifiAPInfo_t *data)
{
	nimacallmsg_t msg = {0,};
	FuncEnter;
	msg.type = eNIMA_CB_WIFI;
	msg.event = event;
	msg.device_id = device_id;
	msg.count = count;
	msg.data = HLIB_MEM_MemDup(data, sizeof(NCAPI_WifiAPInfo_t)*count);

	VK_MSG_Send(__obj(s_nimaserver)->msgq, (void *)&msg, sizeof(nimacallmsg_t));
	FuncLeave;
}

static void __nima_ncapi_callback_bt(eNcapiBTEvent event, NCAPI_BTInfo_t *data)
{
	nimacallmsg_t msg = {0,};
	FuncEnter;
	msg.type = eNIMA_CB_BT;
	msg.event = event;
	msg.data = HLIB_MEM_MemDup(data, sizeof(NCAPI_BTInfo_t));

	VK_MSG_Send(__obj(s_nimaserver)->msgq, (void *)&msg, sizeof(nimacallmsg_t));
	FuncLeave;
}

static void __nima_ncapi_callback_pppoe(eNcapiPPPoEEvent event, NCAPI_PPPoEInfo_t *data)
{
	nimacallmsg_t msg = {0,};
	FuncEnter;
	msg.type = eNIMA_CB_PPPOE;
	msg.event = event;
	msg.data = HLIB_MEM_MemDup(data, sizeof(NCAPI_PPPoEInfo_t));

	VK_MSG_Send(__obj(s_nimaserver)->msgq, (void *)&msg, sizeof(nimacallmsg_t));
	FuncLeave;
}

static void __nima_server_callback_netconf(eNcapiNetConfEvent event, HINT32 index, NCAPI_NetConfInfo_t *data)
{
	nimapacket_t *packet = NULL;
	NIMAProtocol_Netconf_t *info = NULL;

	FuncEnter;
	info = (NIMAProtocol_Netconf_t *)HLIB_STD_MemAlloc(sizeof(NIMAProtocol_Netconf_t));
	HxSTD_MemSet(info, 0x00, sizeof(NIMAProtocol_Netconf_t));

	if(ERR_OK == nima_conv_netconf(data, info))
	{
		nima_debug_log_netconf(data);
	}
	packet = nima_do_encode_packet(NIMA_PROTOTYPE_REPLY,
									nima_conv_event_netconf(event),
									sizeof(*info), info);

	__nima_server_broadcast_session(packet,
									sizeof(*packet) + packet->dlen);

	NIMA_SAFE_DELETE(info);
	NIMA_SAFE_DELETE(packet);
	FuncLeave;
}

static void __nima_server_callback_wifi(eNcapiWifiEvent event, HINT32 device_id, HINT32 ap_count, NCAPI_WifiAPInfo_t *data)
{
	HINT32 i, dLen = 0;
	nimapacket_t *packet = NULL;
	NIMAProtocol_Wifi_t *ap_list = NULL;
	FuncEnter;
	if( ap_count > NIMA_PROTO_MAX_AP ) ap_count = NIMA_PROTO_MAX_AP;

	dLen = sizeof(NIMAProtocol_Wifi_t) + sizeof(NIMAProtocol_APInfo_t)*ap_count;
	ap_list = (NIMAProtocol_Wifi_t *)HLIB_STD_MemAlloc(dLen);
	if ( ap_list )
	{
		HxSTD_MemSet(ap_list, 0x00, dLen);
		for( i=0; i<ap_count; i++ )
		{
			nima_conv_apinfo(&data[i], &ap_list->ap[i]);
		}

		ap_list->dev_id = device_id;
		ap_list->count = ap_count;

		packet = nima_do_encode_packet(NIMA_PROTOTYPE_REPLY,
										nima_conv_event_wifi(event),
										dLen, ap_list);

		nima_debug_log_wifi(data, ap_count);

		__nima_server_broadcast_session(packet,
										sizeof(*packet) + packet->dlen);
		NIMA_SAFE_DELETE(ap_list);
		NIMA_SAFE_DELETE(packet);
	}
	FuncLeave;
}

static void __nima_server_callback_bt(eNcapiBTEvent event, NCAPI_BTInfo_t *data)
{
	HINT32 dLen = 0;
	nimapacket_t *packet = NULL;
	NIMAProtocol_bt_info_t *bt_info = NULL;
	FuncEnter;

	dLen = sizeof(NIMAProtocol_bt_info_t);
	bt_info = (NIMAProtocol_bt_info_t *)HLIB_STD_MemAlloc(dLen);
	HxSTD_MemSet(bt_info, 0x00, dLen);

	if( data )
	{
		nima_conv_btinfo(data, bt_info);
		nima_debug_log_bt(&data->host_info, 1);
		nima_debug_log_bt(data->dev_list, data->count);

		packet = nima_do_encode_packet(NIMA_PROTOTYPE_REPLY,
										nima_conv_event_bt(event),
										dLen, bt_info);
		__nima_server_broadcast_session(packet,
										sizeof(*packet) + packet->dlen);
	}
	NIMA_SAFE_DELETE(bt_info);
	NIMA_SAFE_DELETE(packet);

	FuncLeave;
	return;
}


static void __nima_server_callback_pppoe(eNcapiPPPoEEvent event, NCAPI_PPPoEInfo_t *data)
{
	HINT32 dLen = 0;
	nimapacket_t *packet = NULL;
	NIMAProtocol_PPPOE_info_t *pppoe_info = NULL;
	FuncEnter;

	dLen = sizeof(NIMAProtocol_PPPOE_info_t);
	pppoe_info = (NIMAProtocol_PPPOE_info_t *)HLIB_STD_MemAlloc(dLen);
	if(!pppoe_info) return;

	HxSTD_MemSet(pppoe_info, 0x00, dLen);

	nima_conv_pppoe(data, pppoe_info);

	HxLOG_Debug(" (#)++ PPPOE Info ++\n");
	nima_debug_log_pppoe(data);
	HxLOG_Debug(" (#)++++++++++++++++\n");

	packet = nima_do_encode_packet(NIMA_PROTOTYPE_REPLY,
									nima_conv_event_pppoe(event),
									dLen, pppoe_info);
	__nima_server_broadcast_session(packet, sizeof(*packet) + packet->dlen);

	NIMA_SAFE_DELETE(pppoe_info);
	NIMA_SAFE_DELETE(packet);
	FuncLeave;
	return;
}

static void __nima_callback(void *args)
{
	nimaserverobj_t *server = NULL;

	FuncEnter;
	server = (nimaserverobj_t *)args;

	while ( server->activate )
	{
		nimacallmsg_t msg;
		HxSTD_MemSet(&msg, 0, sizeof(msg));

		if ( VK_MSG_ReceiveTimeout(server->msgq, &msg, sizeof(msg),
					SERVER_MSGQ_TIMEOUT) == VK_OK )
		{
			switch(msg.type)
			{
				case eNIMA_CB_INIT:
					__nima_server_init_session();
					break;
				case eNIMA_CB_NETCONF:
					__nima_server_callback_netconf((eNcapiNetConfEvent)msg.event,
														msg.device_id,
														(NCAPI_NetConfInfo_t*)msg.data);
#if defined(CONFIG_PACKAGE_NTPCLIENT) && (defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_HGS1000S))	// -mhoh- Temporary code for Maxdome test until NTP client scenario is decided !
					if(msg.event == eNCAPI_NETCONF_EVENT_CONN_SUCCESS)
					{
						NCAPI_Netconf_SetNTPClient("de.pool.ntp.org");
					}
#endif
					break;
				case eNIMA_CB_WIFI:
					__nima_server_callback_wifi((eNcapiWifiEvent)msg.event,
													msg.device_id,
													msg.count,
													(NCAPI_WifiAPInfo_t *)msg.data);
					break;
				case eNIMA_CB_BT:
					__nima_server_callback_bt((eNcapiBTEvent)msg.event,
												(NCAPI_BTInfo_t *)msg.data);
					break;
				case eNIMA_CB_PPPOE:
					__nima_server_callback_pppoe((eNcapiPPPoEEvent)msg.event,
													(NCAPI_PPPoEInfo_t *)msg.data);
					break;
				default:
					break;
			}
			NIMA_SAFE_DELETE(msg.data);
		}
		else
		{
			usleep(100000);
		}
	}
	FuncLeave;
}

static void __nima_server(void *args)
{
	HINT32 r = 0;
	nimaserver_t *server = NULL;

	FuncEnter;

	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, __nima_server_onSystemStateChanged);
	HAPI_ConnectSignal("signal:onFactoryDefault", NULL, __nima_server_onFactoryDefault);

	server = (nimaserver_t *)args;
	if ( !server ) return;
	r = HLIB_SOCKET_Listen(__obj(server)->hsock, TRUE, __nima_server_recv_callback,
				__nima_server_status_callback, server);

	if ( r < 0 ) HxLOG_Error("NIMA::ServerTask: Listening failed.\n");
	FuncLeave;
}

static HCHAR * __nima_network_status_to_string(eNimaNetworkStatus status)
{
	switch (status)
	{
	case NIMA_NETWORK_DISCONNECTED:
		return "NIMA_NETWORK_DISCONNECTED";
	case NIMA_NETWORK_CONNECTING:
		return "NIMA_NETWORK_CONNECTING";
	case NIMA_NETWORK_RECONNECTING:
		return "NIMA_NETWORK_RECONNECTING";
	case NIMA_NETWORK_CONNECTED:
		return "NIMA_NETWORK_CONNECTED";
	}
	return "(Unknown)";
}

static HBOOL __nima_need_reconnect(void)
{
	eNimaNetworkStatus eNetworkStatus = nima_scenario_get_net_status();
	HxLOG_Debug("[%s] eNetworkStatus = %s(%d)\n", __FUNCTION__, __nima_network_status_to_string(eNetworkStatus), eNetworkStatus);
	return (eNetworkStatus == NIMA_NETWORK_DISCONNECTED);
}

static HBOOL __nima_is_auto_ip(void)
{
	HINT32 i = 0;
	NIMAProtocol_devinfo_t devinfo;
	HxSTD_MemSet(&devinfo, 0x00, sizeof(NIMAProtocol_devinfo_t));
	__nima_server_get_deviceinfo(&devinfo);
	for (i = 0; i < devinfo.dev_count; i++)
	{
		if (devinfo.netconf[i].ip_ver == eNIMA_NETCONF_IPV4)
		{
			HxLOG_Debug("[%d]ipv4 %d.%d.%d.%d\n", i,
						  devinfo.netconf[i].ip_info.ipv4.ipAddr[0], devinfo.netconf[i].ip_info.ipv4.ipAddr[1],
						  devinfo.netconf[i].ip_info.ipv4.ipAddr[2], devinfo.netconf[i].ip_info.ipv4.ipAddr[3]);
			if (devinfo.netconf[i].ip_info.ipv4.ipAddr[0] == 169 && devinfo.netconf[i].ip_info.ipv4.ipAddr[1] == 254)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

/**
* This function is used to connect to the network automatically
* @param[in] void *args: for future use
* @return void
*/
static void __nima_connect_auto(void *args)
{
	FuncEnter;
	while (s_finishAutoConnect == FALSE)
	{
		if (s_enableAutoConnect == FALSE)
		{
			HLIB_STD_TaskSleep(10);
			continue;
		}
		
		if (__nima_need_reconnect() == TRUE)
		{
			NIMAProtocol_devinfo_t devinfo;
			HxSTD_MemSet(&devinfo, 0x00, sizeof(NIMAProtocol_devinfo_t));
			HxLOG_Warning("[%s] Do try to connect\n", __FUNCTION__);
			__nima_server_get_deviceinfo(&devinfo);
			nima_db_load(&devinfo);
			nima_scenario_apply(&devinfo);
		}
		if (__nima_is_auto_ip() == TRUE)
		{
			NIMAProtocol_devinfo_t devinfo;
			HxSTD_MemSet(&devinfo, 0x00, sizeof(NIMAProtocol_devinfo_t));
			__nima_server_get_deviceinfo(&devinfo);
			NCAPI_NetConf_RestartDHCPClient(devinfo.default_dev);
		}
		HLIB_STD_TaskSleep(5000);
	}
	FuncLeave;
}



/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
nimaserver_t *nima_server_create(HUINT32 magic_code)
{
	nimaserverobj_t *obj = NULL;

	FuncEnter;

	if(s_nimaserver == NULL)
	{
		obj = HLIB_STD_MemAlloc(sizeof(*obj));

		if ( !obj ) return NULL;

		obj->magic_code = magic_code;
		obj->tid = 0;
		obj->tid_callback = 0;
		obj->tid_connect = 0;
		obj->hsock = -1;
		obj->activate = TRUE;
		obj->sessions = NULL;
		HxSTD_MemSet(obj->uds_path, 0, sizeof(obj->uds_path));
		obj->ucShutdownDelayCount = 0;

		s_nimaserver = obj;
	}
	FuncLeave;

	return (nimaserver_t *)s_nimaserver;
}

void nima_server_destroy(nimaserver_t *server)
{
	FuncEnter;

	NCAPI_NetConf_UnregisterCallback(__nima_ncapi_callback_netconf);
	NCAPI_Wifi_UnregisterCallback(__nima_ncapi_callback_wifi);
	NCAPI_BT_UnregisterCallback(__nima_ncapi_callback_bt);
	NCAPI_PPPoE_UnregisterCallback(__nima_ncapi_callback_pppoe);

	NCAPI_Deinit();

	NIMA_SAFE_DELETE(server);

	FuncLeave;
}

void nima_server_init_ncapi(void)
{
	if(ERR_OK == NCAPI_Init())
	{
		NCAPI_NetConf_RegisterCallback(__nima_ncapi_callback_netconf);
		NCAPI_Wifi_RegisterCallback(__nima_ncapi_callback_wifi);
		NCAPI_BT_RegisterCallback(__nima_ncapi_callback_bt);
		NCAPI_PPPoE_RegisterCallback(__nima_ncapi_callback_pppoe);
	}
}

static HERROR __nima_start_auto_connect(nimaserver_t * server, nimaserverobj_t * obj)
{
	s_finishAutoConnect = FALSE;
	s_enableAutoConnect = TRUE;
	if ( VK_TASK_Create(__nima_connect_auto, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "NIMAConnectAuto",
						(void *)server, &obj->tid_connect, 0) != VK_OK
		 || VK_TASK_Start(obj->tid_connect) != VK_OK ) return ERR_FAIL;

	return ERR_OK;
}

HERROR nima_server_start(nimaserver_t *server, const HCHAR *addr)
{
	nimaserverobj_t *obj = NULL;

	FuncEnter;

	obj = __obj(server);
	if ( !obj ) return ERR_FAIL;
	if ( !addr ) return ERR_FAIL;

	obj->hsock = HLIB_EXTRA_OpenServer(addr);//HLIB_SOCKET_OpenServer(HxSOCKET_UDS, (void *)addr);
	if ( obj->hsock == 0 ) return ERR_FAIL;

	if ( VK_MSG_Create(SERVER_MSGQ_SIZE, sizeof(nimacallmsg_t), "NIMACallbackMsg",
				&obj->msgq, VK_SUSPENDTYPE_PRIORITY) != VK_OK ) return ERR_FAIL;

	nima_server_init_ncapi();

	if ( VK_TASK_Create(__nima_callback, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "NIMACallback",
				(void *)server, &obj->tid_callback, 0) != VK_OK
		|| VK_TASK_Start(obj->tid_callback) != VK_OK ) return ERR_FAIL;

	if ( VK_TASK_Create(__nima_server, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "NIMAServer",
				(void *)server, &obj->tid, 0) != VK_OK
		|| VK_TASK_Start(obj->tid) != VK_OK ) return ERR_FAIL;

	HxSTD_StrNCpy(obj->uds_path, addr, HxSTD_StrLen(addr)+1);

	if (__nima_start_auto_connect(server, obj) != ERR_OK)
	{
		return ERR_FAIL;
	}

	FuncLeave;
	return ERR_OK;
}

HERROR nima_server_start_tcp(nimaserver_t *server, HUINT32 ipaddr, HUINT16 port)
{
	return ERR_FAIL;
}

HERROR nima_server_stop(nimaserver_t *server)
{
#if 0 // TODO: Check below routine, if it happen crashes

	nimaserverobj_t *obj = NULL;

	FuncEnter;
	if ( !server ) return ERR_FAIL;

	obj = __obj(server);
	VK_MSG_Destroy(obj->msgq);
	VK_TASK_Stop(obj->tid_callback);
	VK_TASK_Stop(obj->tid);
	VK_TASK_Destroy(obj->tid_callback);
	VK_TASK_Destroy(obj->tid);
	FuncLeave;
#endif
	return ERR_OK;
}

void nima_set_enable_auto_connect(HBOOL enable)
{
	HxLOG_Warning("Set enable auto connect = [%s]\n", (enable ? "YES" : "NO"));
	s_enableAutoConnect = enable;
}

nimasession_t *nima_server_create_session(nimaserver_t *server)
{
	nimasession_t *session = NULL;

	FuncEnter;

	if ( !server ) return NULL;

	session = HLIB_STD_MemAlloc(sizeof(*session));
	if ( !session ) return NULL;

	HxSTD_MemSet(session, 0, sizeof(*session));
	__obj(server)->sessions = HLIB_LIST_Append(__obj(server)->sessions, session);

	FuncLeave;

	return (nimasession_t *)session;
}

void nima_server_destroy_session(nimaserver_t *server, nimasession_t *session)
{
	FuncEnter;

	if ( !server || !session ) return;

	nima_session_stop(session);
	__obj(server)->sessions = HLIB_LIST_Remove(__obj(server)->sessions, session);
	session = NULL;

	FuncLeave;
}

nimasession_t *nima_server_find_session(nimaserver_t *server, HINT32 sock)
{
	HxList_t *pos = NULL;

	FuncEnter;

	if ( !server ) return NULL;

	pos = HLIB_LIST_First(__obj(server)->sessions);
	while ( pos )
	{
		nimasession_t *session = HLIB_LIST_Data(pos);
		if ( !session ) return NULL;
		if ( session->sock == sock ) return session;
		pos = HLIB_LIST_Next(pos);
	}

	FuncLeave;

	return NULL;
}

