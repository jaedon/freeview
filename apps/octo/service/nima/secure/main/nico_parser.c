/*************************************************************************************************************
	File 		: nico_parser.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/15
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "nico_parser.h"
#include "nico_debug.h"
#include "di_err.h"
#include "di_network.h"
#include "di_cm.h"
#include "di_wifi.h"
#include "di_pppoe.h"

/*******************************************************************/
/*******************      template         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef void (*__nico_parser_encode)(nico_dievtmsg_t *di_event, event_msg *event);

typedef struct _nico_parser_context
{
	HINT32 device_count;
	NICO_NetDevice_Info_t **deviceinfo;
} st_parser_context;

typedef st_parser_context* pst_parser_context;

typedef struct _nico_parser_object
{
	st_parser_context obj_ctx;
} st_parser_object;
typedef st_parser_object* pst_parser_object;

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
st_parser_object g_parser_object;
pst_parser_object parser_object = &g_parser_object;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/

STATIC pst_parser_context __nico_parser_getcontext(void)
{
	return &parser_object->obj_ctx;
}
STATIC HINT32 __nico_parser_get_cm_devid(void)
{
	HINT32 i = 0;
	pst_parser_context ctx = __nico_parser_getcontext();

	for(i=0;i<ctx->device_count;i++)
	{
		if(eNICO_DEVTYPE_CM == ctx->deviceinfo[i]->netinfo.dev_type)
		{
			break;
		}
	}
	return (i == ctx->device_count ? -1 : i);
}
STATIC HINT32 __nico_parser_get_pppoe_devid(void)
{
	HINT32 i = 0;
	pst_parser_context ctx = __nico_parser_getcontext();

	for(i=0;i<ctx->device_count;i++)
	{
		if(eNICO_DEVTYPE_PPPOE == ctx->deviceinfo[i]->netinfo.dev_type)
		{
			break;
		}
	}
	return (i == ctx->device_count ? -1 : i);
}
STATIC HINT32 __nico_parser_get_deviceid(nico_dievtmsg_t *di_event)
{
	HINT32 ret_deviceid = -1;
	HINT32 dev_count = 0;

	if(EVT_WIFI == di_event->type)
	{
		DI_WIFI_CALLBACK_DATA_t *data = di_event->data;
		ret_deviceid = data->ulDevId;
		switch(data->eEvent)
		{
			CASE_EVT_ENUMTOSTR(DI_WIFI_NOTIFY_EVT_INSERTED);
			CASE_EVT_ENUMTOSTR(DI_WIFI_NOTIFY_EVT_EXTRACTED);
			CASE_EVT_ENUMTOSTR(DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_WIFI_NOTIFY_EVT_SCAN_FAIL);
			CASE_EVT_ENUMTOSTR(DI_WIFI_NOTIFY_EVT_AUTHEN_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_WIFI_NOTIFY_EVT_AUTHEN_FAIL);
			CASE_EVT_ENUMTOSTR(DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_WIFI_NOTIFY_EVT_CONNECT_FAIL);
			CASE_EVT_ENUMTOSTR(DI_WIFI_NOTIFY_EVT_MAX);
			default: HxLOG_Debug("received ext-evt from di : %08x \n", data->eEvent);	break;
		}
	}
	else if(EVT_NETCONF == di_event->type)
	{
		DI_NETWORK_CALLBACK_DATA_t *data = di_event->data;
		ret_deviceid = data->ulDevId;
		switch(data->eEvent)
		{
			CASE_EVT_ENUMTOSTR(DI_NETWORK_EVENT_CABLE_CONNECT);
			CASE_EVT_ENUMTOSTR(DI_NETWORK_EVENT_CABLE_DISCONNECT);
			CASE_EVT_ENUMTOSTR(DI_NETWORK_EVENT_DHCP_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_NETWORK_EVENT_DHCP_FAIL);
			CASE_EVT_ENUMTOSTR(DI_NETWORK_EVENT_DHCP6_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_NETWORK_EVENT_DHCP6_FAIL);
			CASE_EVT_ENUMTOSTR(DI_NETWORK_EVENT_AUTOIP_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_NETWORK_EVENT_AUTOIP_FAIL);
			CASE_EVT_ENUMTOSTR(DI_NETWORK_EVENT_MAX);
			default: HxLOG_Debug("received ext-evt from di : %08x \n", data->eEvent);	break;
		}
	}
	else if(EVT_CM == di_event->type)
	{
		DI_CM_LinkInfo_e *data = di_event->data;
		switch(*data)
		{
			CASE_EVT_ENUMTOSTR(DI_CM_EVENT_ONLINE);
			CASE_EVT_ENUMTOSTR(DI_CM_EVENT_OFFLINE);
			default: 	break;
		}
		ret_deviceid = __nico_parser_get_cm_devid();
	}
	else if(EVT_BT == di_event->type)
	{
		DI_BT_CALLBACK_DATA_t *data = di_event->data;
		DI_NETWORK_GetDevCount(&dev_count);
		ret_deviceid = dev_count;
		switch(data->eEvent)
		{
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_INSERTED);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_EXTRACTED);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_ENABLE);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_DISABLE);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_SCAN_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_SCAN_FAIL);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_AUTHEN_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_AUTHEN_FAIL);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_PASSKEY);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_CONNECT_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_CONNECT_FAIL);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_REMOTE_CMD);
			CASE_EVT_ENUMTOSTR(DI_BT_NOTIFY_EVT_MAX);
			default: HxLOG_Debug("received ext-evt from di : %08x \n", data->eEvent);	break;
		}
	}
	else if(EVT_PPPOE == di_event->type)
	{
		DI_PPPOE_CALLBACK_DATA_t *data = di_event->data;
		switch(data->eEvent)
		{
			CASE_EVT_ENUMTOSTR(DI_PPPOE_EVENT_CONNECT_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_PPPOE_EVENT_CONNECT_FAIL);
			CASE_EVT_ENUMTOSTR(DI_PPPOE_EVENT_DISCONNECT_SUCCESS);
			CASE_EVT_ENUMTOSTR(DI_PPPOE_EVENT_DISCONNECT_FAIL);
			CASE_EVT_ENUMTOSTR(DI_PPPOE_EVENT_SET_INFO);
			CASE_EVT_ENUMTOSTR(DI_PPPOE_EVENT_SET_ONOFF);
			default: HxLOG_Debug("received ext-evt from di : %08x \n", data->eEvent);	break;
		}
		ret_deviceid = __nico_parser_get_pppoe_devid();
	}

	return ret_deviceid;
}
STATIC void __nico_parser_print_command(eNcapi_command_type cmd_type)
{
	switch(cmd_type)
	{
		CASE_CMD_ENUMTOSTR(CMD_GET_NETCONF_NETINFO);
		CASE_CMD_ENUMTOSTR(CMD_GET_WIFI_AP_INFO_CONNECTED);
		CASE_CMD_ENUMTOSTR(CMD_GET_WIFI_AP_INFO_WAVE);
		CASE_CMD_ENUMTOSTR(CMD_GET_NETCONF_DHCPSERVER_INFO);

		CASE_CMD_ENUMTOSTR(CMD_SET_NETCONF_DEVICE_CONFIG);
		CASE_CMD_ENUMTOSTR(CMD_SET_NETCONF_RESTART_DHCP_CLIENT);
		CASE_CMD_ENUMTOSTR(CMD_SET_NETCONF_STOP_CONNECTING);
		CASE_CMD_ENUMTOSTR(CMD_SET_NETCONF_DEVICE_ENABLE);
		CASE_CMD_ENUMTOSTR(CMD_SET_NETCONF_DEVICE_DISABLE);
		CASE_CMD_ENUMTOSTR(CMD_SET_NETCONF_START_DHCPSERVER);
		CASE_CMD_ENUMTOSTR(CMD_SET_NETCONF_STOP_DHCPSERVER);
		CASE_CMD_ENUMTOSTR(CMD_SET_NETCONF_WOL);

		CASE_CMD_ENUMTOSTR(CMD_SET_WIFI_START_SCAN_AP);
		CASE_CMD_ENUMTOSTR(CMD_SET_WIFI_STOP_SCAN_AP);
		CASE_CMD_ENUMTOSTR(CMD_SET_WIFI_CONNECT_AP);
		CASE_CMD_ENUMTOSTR(CMD_SET_WIFI_CONNECT_WPS);
		CASE_CMD_ENUMTOSTR(CMD_SET_WIFI_DISCONNECT_AP);
		CASE_CMD_ENUMTOSTR(CMD_SET_WIFI_GET_WPS_PINCODE);
		CASE_CMD_ENUMTOSTR(CMD_SET_WIFI_WOWLAN);

		CASE_CMD_ENUMTOSTR(CMD_SET_CM_DOWNSTREAM_INFO);

		CASE_CMD_ENUMTOSTR(CMD_SET_BT_ENABLE);
		CASE_CMD_ENUMTOSTR(CMD_SET_BT_START_SCAN);
		CASE_CMD_ENUMTOSTR(CMD_SET_BT_PINCODE);
		CASE_CMD_ENUMTOSTR(CMD_SET_BT_CONNECT);
		CASE_CMD_ENUMTOSTR(CMD_SET_BT_DISCONNECT);
		CASE_CMD_ENUMTOSTR(CMD_SET_BT_REMOVE);
		CASE_CMD_ENUMTOSTR(CMD_SET_BT_OPEN_PROFILE);
		CASE_CMD_ENUMTOSTR(CMD_SET_BT_CLOSE_PROFILE);
		CASE_CMD_ENUMTOSTR(CMD_GET_BT_HOST_INFO);
		CASE_CMD_ENUMTOSTR(CMD_GET_BT_DEVICE_LIST);
		CASE_CMD_ENUMTOSTR(CMD_SET_BT_INFO_DB);
		CASE_CMD_ENUMTOSTR(CMD_SET_BT_REMOVE_DB);

		CASE_CMD_ENUMTOSTR(CMD_SET_PPPOE_CONNECT);
		CASE_CMD_ENUMTOSTR(CMD_SET_PPPOE_DISCONNECT);
		CASE_CMD_ENUMTOSTR(CMD_SET_PPPOE_INFO);
		CASE_CMD_ENUMTOSTR(CMD_GET_PPPOE_INFO);
		CASE_CMD_ENUMTOSTR(CMD_SET_PPPOE_ONOFF);

		CASE_CMD_ENUMTOSTR(CMD_UNKNOWN);
		default:
			break;
	}
}

/*
* Begin of __nico_parser_write_xxxx_context
* __nico_parser_write_xxxx_context() has hard code
*
*/
STATIC void __nico_parser_write_netconf_context(HUINT32 di_event, HINT32 dev_id)
{
	pst_parser_context ctx = __nico_parser_getcontext();
	(void)ctx;

	switch(di_event)
	{
		default:
			break;
	}
}

STATIC void __nico_parser_write_wifi_context(HUINT32 di_event, HINT32 dev_id)
{
	pst_parser_context ctx = __nico_parser_getcontext();
	(void)ctx;

	switch(di_event)
	{
		case DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS:
			ctx->deviceinfo[dev_id]->netinfo.dhcp = TRUE;
			ctx->deviceinfo[dev_id]->netinfo.dns_auto = TRUE;
			break;
		default:
			break;
	}
}
STATIC void __nico_parser_write_bluetooth_context(HUINT32 di_event, HINT32 dev_id)
{

}

STATIC void __nico_parser_write_cm_context(HUINT32 di_event, HINT32 dev_id)
{

}

STATIC void __nico_parser_write_pppoe_context(HUINT32 di_event, HINT32 dev_id)
{

}

/* End of __nico_parser_write_xxxx_context */

STATIC HINT32 __nico_parser_wifi_apcount_with_essid(DI_WIFI_CALLBACK_DATA_t *data)
{
	DI_WIFI_INFO_t *di_aplist = (DI_WIFI_INFO_t *)(data->pvEventData);
	HINT32 real_apcount = 0, i = 0;

	for(i = 0; i < data->ulNumOfData; i++)
	{
		if(!HxSTD_StrEmpty(di_aplist[i].aucESSID))
		{
			real_apcount++;
		}

		if(real_apcount == NICO_MAX_AP)
			break;
	}
	return real_apcount;
}

STATIC void __nico_parser_wifi_copy_apinfo(DI_WIFI_INFO_t *src, NICO_Wifi_APInfo_t *dst)
{
	dst->channel = src->ulChannel;
	dst->freq = src->ulFreq;
	dst->is_wps = src->bWPS;
	dst->op_mode = src->eOpMode;
	dst->wave_strength = src->nWaveStrength;
	dst->bitrate = src->nBitRate;
	dst->secure_type = nico_op_common_ap_securetype(src->eSecureType);
	dst->authen_type = nico_op_common_ap_authentype(src->eAuthenType);
	dst->encrypt_type = nico_op_common_ap_encrypttype(src->eEncryptType);
	dst->keyindex = src->aucKeyIndex;
	HxSTD_MemCopy(dst->macaddr, src->aucMAC, sizeof(char)*NICO_MAX_MACADDR);
	HxSTD_MemCopy(dst->essid, src->aucESSID, sizeof(char)*NICO_AP_ESSID);
	HxSTD_MemCopy(dst->id, src->aucId, sizeof(char)*NICO_MAX_ID);
	HxSTD_MemCopy(dst->apkey, src->aucKey, sizeof(char)*NICO_AP_KEY);
}

STATIC void __nico_parser_bt_devlist(DI_BT_DEV_LIST_t *src, NICO_BT_Info_t *dst)
{
	HINT32 i = 0;
	ENTRY;
		dst->count = src->ulNumOfData;
		for( i=0; i<dst->count; i++ )
		{
			HxSTD_MemCopy(dst->dev_list[i].name, src->stBtDevList[i].ucName, NICO_BT_NAME_MAX);
			HxSTD_MemCopy(dst->dev_list[i].bdaddr, src->stBtDevList[i].ucBdAddr, NICO_BT_BDADDR_LEN);
			HxSTD_MemCopy(dst->dev_list[i].dev_class, src->stBtDevList[i].ucClass, NICO_BT_CLASS_LEN);
			dst->dev_list[i].status = src->stBtDevList[i].etDevStatus;
			dst->dev_list[i].available_profile = src->stBtDevList[i].etAvailableProfile;
		}
	EXIT;
}

STATIC void __nico_parser_encode_wifi(nico_dievtmsg_t *di_event, event_msg *event)
{
	DI_WIFI_CALLBACK_DATA_t *data = di_event->data;
	HUINT32 id = data->ulDevId, i = 0, j = 0;
	DI_WIFI_INFO_t *di_aplist = (DI_WIFI_INFO_t *)(data->pvEventData);
	pst_parser_context ctx = __nico_parser_getcontext();

	ENTRY;
	{	//1 #### Common ####
		event->device_id = id;
		HxLOG_Debug("+ encoded wifi device information +++++ \n");
		HxLOG_Debug("wifi device id : %d \n", id);
		HxLOG_Debug("+++++++++++++++++++++++++++++++++++++++ \n");
	}

	__nico_parser_write_wifi_context(data->eEvent, id);

	switch(data->eEvent)
	{
		case DI_WIFI_NOTIFY_EVT_INSERTED:
			{
				event->type.evt_type = EVENT_NET_WIFI_USB_INSERTED;
				event->evt_protocol.evt_wifi.evt.evt_dongle.id = id;
			}
			break;
		case DI_WIFI_NOTIFY_EVT_EXTRACTED:
			{
				event->type.evt_type = EVENT_NET_WIFI_USB_EXTRACTED;
				event->evt_protocol.evt_wifi.evt.evt_dongle.id = id;
			}
			break;
		case DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS:
			{
				HINT32 apcount = 0;
				NICO_Wifi_APInfo_t templist;

				apcount = __nico_parser_wifi_apcount_with_essid(data);
				HxSTD_MemSet(&ctx->deviceinfo[id]->apinfo_list, 0x00, sizeof(ctx->deviceinfo[id]->apinfo_list));
				for(i = 0; i < data->ulNumOfData; i++)
				{
					HxSTD_MemSet(&templist, 0x00, sizeof(NICO_Wifi_APInfo_t));
					if(!HxSTD_StrEmpty(di_aplist[i].aucESSID))
					{
						__nico_parser_wifi_copy_apinfo(&di_aplist[i], &templist);
						nico_debug_print_apinfo(&templist);

						HxSTD_MemCopy(&ctx->deviceinfo[id]->apinfo_list[j], &templist, sizeof(NICO_Wifi_APInfo_t));
						HxSTD_MemCopy(&event->evt_protocol.evt_wifi.evt.evt_scan.apinfo[j++], &templist, sizeof(NICO_Wifi_APInfo_t));
					}

                                        if(j == NICO_MAX_AP)
						break;
				}

#if defined(CONFIG_MW_INET_WLAN)
				if( (apcount == 0) && (data->ulNumOfData > 0) )
				{
					nico_op_common_wifi_scan(id);
				}
#endif

				ctx->deviceinfo[id]->ap_count = apcount;
				event->type.evt_type = EVENT_NET_WIFI_SCAN_FINISHED;
				event->evt_protocol.evt_wifi.evt.evt_scan.id = id;
				event->evt_protocol.evt_wifi.evt.evt_scan.apcount = apcount;
			}
			break;
		case DI_WIFI_NOTIFY_EVT_SCAN_FAIL:
			{
				event->type.evt_type = EVENT_NET_WIFI_SCAN_FAIL;
				event->evt_protocol.evt_wifi.evt.evt_scan.id = id;
				event->evt_protocol.evt_wifi.evt.evt_scan.apcount = 0;
			}
			break;
		case DI_WIFI_NOTIFY_EVT_AUTHEN_SUCCESS:
			{
			}
			break;
		case DI_WIFI_NOTIFY_EVT_AUTHEN_FAIL:
			{
			}
			break;
		case DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS:
			{
				if(TRUE == ctx->deviceinfo[id]->is_trywps)
					event->type.evt_type = EVENT_NET_WIFI_WPS_CONNECT_SUCCESS;
				else
					event->type.evt_type = EVENT_NET_WIFI_AP_CONNECT_SUCCESS;

				event->evt_protocol.evt_wifi.evt.evt_connect.id = id;


				HxSTD_MemCopy(&event->evt_protocol.evt_wifi.evt.evt_connect.apinfo,
								&ctx->deviceinfo[id]->apinfo_connected, sizeof(NICO_Wifi_APInfo_t));
				HxLOG_Debug("Received DI event: DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS\n");
				nico_debug_print_apinfo(&ctx->deviceinfo[id]->apinfo_connected);
			}
			break;
		case DI_WIFI_NOTIFY_EVT_CONNECT_FAIL:
			{
				if(TRUE == ctx->deviceinfo[id]->is_trywps)
					event->type.evt_type = EVENT_NET_WIFI_WPS_CONNECT_FAIL;
				else
					event->type.evt_type = EVENT_NET_WIFI_AP_CONNECT_FAIL;
				event->evt_protocol.evt_wifi.evt.evt_connect.id = id;
			}
			break;
		case DI_WIFI_NOTIFY_EVT_MAX:
			{
			}
			break;
		default:
			event->type.evt_type = data->eEvent;
			break;
	}
	EXIT;
}

STATIC void __nico_parser_encode_netconf(nico_dievtmsg_t *di_event, event_msg *event)
{
	DI_NETWORK_CALLBACK_DATA_t *data = di_event->data;
	pst_parser_context ctx = __nico_parser_getcontext();
	HUINT32 id = data->ulDevId;

	ENTRY;
	event->device_id = id;

	__nico_parser_write_netconf_context(data->eEvent, id);

	switch(data->eEvent)
	{
		case DI_NETWORK_EVENT_CABLE_CONNECT:
			{
				event->type.evt_type = EVENT_NET_PLUGGED_IN;
				event->evt_protocol.evt_netconf.evt.evt_plugstate.id = id;
			}
			break;
		case DI_NETWORK_EVENT_CABLE_DISCONNECT:
			{
				event->type.evt_type = EVENT_NET_PLUGGED_OUT;
				event->evt_protocol.evt_netconf.evt.evt_plugstate.id = id;
			}
			break;
		case DI_NETWORK_EVENT_DHCP_SUCCESS:
		case DI_NETWORK_EVENT_DHCP6_SUCCESS:
			{
				event->type.evt_type = EVENT_NET_CONNECT_SUCCESS;
				event->evt_protocol.evt_netconf.evt.evt_dhcpc.id = id;
				break;
			}
		case DI_NETWORK_EVENT_DHCP_FAIL:
		case DI_NETWORK_EVENT_DHCP6_FAIL:
			{
				event->type.evt_type = EVENT_NET_CONNECT_FAIL;
				event->evt_protocol.evt_netconf.evt.evt_dhcpc.id = id;
			}
			break;
		case DI_NETWORK_EVENT_AUTOIP_SUCCESS:
			{
				event->type.evt_type = EVENT_NET_CONNECT_SUCCESS;
				event->evt_protocol.evt_netconf.evt.evt_autoip.id = id;
			}
			break;
		case DI_NETWORK_EVENT_AUTOIP_FAIL:
			{
				event->type.evt_type = EVENT_NET_CONNECT_FAIL;
				event->evt_protocol.evt_netconf.evt.evt_autoip.id = id;
			}
			break;
		case DI_NETWORK_EVENT_MAX:
			break;
		default:
			event->type.evt_type = data->eEvent;
			break;
	}

	{
		HxSTD_MemCopy(&event->evt_protocol.evt_netconf.evt_netinfo,
						&ctx->deviceinfo[id]->netinfo, sizeof(NICO_NetConf_Info_t));
		nico_debug_print_netinfo(&ctx->deviceinfo[id]->netinfo);
	}
	EXIT;
}

STATIC void __nico_parser_encode_cm(nico_dievtmsg_t *di_event, event_msg *event)
{
	pst_parser_context ctx = __nico_parser_getcontext();
	DI_CM_LinkInfo_e *data = di_event->data;
	HINT32 cm_devid = __nico_parser_get_cm_devid();

	ENTRY;

	switch(*data)
	{
		case DI_CM_EVENT_ONLINE:
			{
				event->type.evt_type = EVENT_NET_CM_ONLINE;
				event->evt_protocol.evt_cm.evt.evt_state.is_online = TRUE;
			}
			break;
		case DI_CM_EVENT_OFFLINE:
			{
				event->type.evt_type = EVENT_NET_CM_OFFLINE;
				event->evt_protocol.evt_cm.evt.evt_state.is_online = FALSE;
			}
			break;
		default:
			return;
	}
	if(cm_devid < 0) return;

	HxSTD_MemCopy(&event->evt_protocol.evt_cm.cminfo,
					&ctx->deviceinfo[cm_devid]->cminfo, sizeof(NICO_CM_Info_t));
	EXIT;
}

STATIC void __nico_parser_encode_bluetooth(nico_dievtmsg_t *di_event, event_msg *event)
{
//	HINT32 i = 0;
	DI_BT_CALLBACK_DATA_t *data = di_event->data;
	DI_BT_DEV_LIST_t *di_devlist = (DI_BT_DEV_LIST_t *)(data->pvEventData);
	pst_parser_context ctx = __nico_parser_getcontext();

	ENTRY;
	{	//1 #### Common ####
		HxLOG_Debug("+ encoded wifi device information +++++ \n");
		HxLOG_Debug("bluetooth device \n");
		HxLOG_Debug("+++++++++++++++++++++++++++++++++++++++ \n");
	}

	event->evt_protocol.evt_bluetooth.evt.evt_result.dev_id = ctx->device_count;

	switch(data->eEvent)
	{
		case DI_BT_NOTIFY_EVT_INSERTED:
			{
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = *((HBOOL *)(data->pvEventData));
				event->type.evt_type = EVENT_NET_BT_INSERTED;
			}
			break;
		case DI_BT_NOTIFY_EVT_EXTRACTED:
			{
				event->type.evt_type = EVENT_NET_BT_EXTRACTED;
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = FALSE;
			}
			break;
		case DI_BT_NOTIFY_EVT_ENABLE:
			{
				event->type.evt_type = EVENT_NET_BT_ENABLE;
				__nico_parser_bt_devlist(di_devlist, &event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info);
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = TRUE;
			}
			break;
		case DI_BT_NOTIFY_EVT_DISABLE:
			{
				event->type.evt_type = EVENT_NET_BT_DISABLE;
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = FALSE;
			}
			break;
		case DI_BT_NOTIFY_EVT_SCAN_SUCCESS:
			{
				__nico_parser_bt_devlist(di_devlist, &event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info);
				event->type.evt_type = EVENT_NET_BT_SCAN_SUCCESS;
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = TRUE;
			}
			break;
		case DI_BT_NOTIFY_EVT_SCAN_FAIL:
			{
				event->type.evt_type = EVENT_NET_BT_SCAN_FAIL;
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = TRUE;
			}
			break;
		case DI_BT_NOTIFY_EVT_AUTHEN_SUCCESS:
			{
				event->type.evt_type = EVENT_NET_BT_AUTHEN_SUCCESS;
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = TRUE;
			}
			break;
		case DI_BT_NOTIFY_EVT_AUTHEN_FAIL:
			{
				__nico_parser_bt_devlist(di_devlist, &event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info);
				event->type.evt_type = EVENT_NET_BT_AUTHEN_FAIL;
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = TRUE;
			}
			break;
		case DI_BT_NOTIFY_EVT_PASSKEY:
			{
				HINT32 *key = (HINT32 *)data->pvEventData;
				if( NULL != key )
				{
					HxLOG_Debug("++[NICO] BT : passkey : %d \n", *key);
					event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.passkey = *key;
				}
				event->type.evt_type = EVENT_NET_BT_PASSKEY;
			}
			break;
		case DI_BT_NOTIFY_EVT_CONNECT_SUCCESS:
			{
				__nico_parser_bt_devlist(di_devlist, &event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info);
				event->type.evt_type = EVENT_NET_BT_CONNECT_SUCCESS;
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = TRUE;
			}
			break;
		case DI_BT_NOTIFY_EVT_CONNECT_FAIL:
			{
				__nico_parser_bt_devlist(di_devlist, &event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info);
				event->type.evt_type = EVENT_NET_BT_CONNECT_FAIL;
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = TRUE;
			}
			break;
		case DI_BT_NOTIFY_EVT_REMOTE_CMD:
			{
				event->type.evt_type = EVENT_NET_BT_REMOTE_CMD;
				event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = TRUE;
			}
			break;
		default:
			event->type.evt_type = data->eEvent;
			break;
	}

	if( data->eEvent == (HUINT32)EVENT_NET_BT_REMOVED )
	{
		if( ERR_OK == nico_op_common_get_bluetooth_devicelist(&event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info) )
		{
			event->type.evt_type = EVENT_NET_BT_REMOVED;
			event->evt_protocol.evt_bluetooth.evt.evt_result.bt_info.enable = TRUE;
		}
	}

	EXIT;
}

STATIC void __nico_parser_encode_pppoe(nico_dievtmsg_t *di_event, event_msg *event)
{
	DI_PPPOE_CALLBACK_DATA_t *data = di_event->data;
	HINT32 pppoe_devid = __nico_parser_get_pppoe_devid();
	pst_parser_context ctx = __nico_parser_getcontext();

	ENTRY;
	{
		event->device_id = pppoe_devid;
		HxLOG_Debug("+ encoded pppoe device information +++++ \n");
		HxLOG_Debug("pppoe device id : %d \n", event->device_id);
		HxLOG_Debug("data->eEvent : %d \n", data->eEvent);
		HxLOG_Debug("+++++++++++++++++++++++++++++++++++++++ \n");
	}

	switch(data->eEvent)
	{
		case DI_PPPOE_EVENT_CONNECT_SUCCESS:
			event->type.evt_type = EVENT_NET_PPPOE_CONNECT_SUCCESS;
			HxSTD_MemCopy(&event->evt_protocol.evt_pppoe.evt.evt_connect.info,
					&ctx->deviceinfo[pppoe_devid]->pppoeinfo, sizeof(NICO_PPPoE_Info_t));
			break;
		case DI_PPPOE_EVENT_CONNECT_FAIL:
			event->type.evt_type = EVENT_NET_PPPOE_CONNECT_FAIL;
			break;
		case DI_PPPOE_EVENT_DISCONNECT_SUCCESS:
			event->type.evt_type = EVENT_NET_PPPOE_DISCONNECT_SUCCESS;
			break;
		case DI_PPPOE_EVENT_DISCONNECT_FAIL:
			event->type.evt_type = EVENT_NET_PPPOE_DISCONNECT_FAIL;
			break;
		case DI_PPPOE_EVENT_SET_INFO:
			event->type.evt_type = EVENT_NET_PPPOE_SET_INFO;
			HxSTD_MemCopy(&event->evt_protocol.evt_pppoe.evt.evt_info.info,
					&ctx->deviceinfo[pppoe_devid]->pppoeinfo, sizeof(NICO_PPPoE_Info_t));
			break;
		case DI_PPPOE_EVENT_SET_ONOFF:
			event->type.evt_type = EVENT_NET_PPPOE_SET_ONOFF;
			HxSTD_MemCopy(&event->evt_protocol.evt_pppoe.evt.evt_info.info,
					&ctx->deviceinfo[pppoe_devid]->pppoeinfo, sizeof(NICO_PPPoE_Info_t));
			break;
		default:
			break;
	}

	EXIT;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR nico_parser_init( HINT32 dev_count, NICO_NetDevice_Info_t *devices[]  )
{
	HERROR hRet = ERR_OK;
	pst_parser_context ctx = __nico_parser_getcontext();

	ENTRY;
	ctx->device_count = dev_count;
	ctx->deviceinfo = devices;
	EXIT;
	return hRet;
}

HERROR nico_parser_decode( command_msg *params )
{
	HERROR hRet = ERR_OK;
	ENTRY;
	__nico_parser_print_command(params->type.cmd_type);
	EXIT;
	return hRet;
}
HERROR nico_parser_encode( nico_dievtmsg_t *di_event, event_msg *event )
{
	HERROR hRet = ERR_OK;
	__nico_parser_encode protocol_encoder = NULL;
	pst_parser_context ctx = __nico_parser_getcontext();
	HINT32 dev_id = __nico_parser_get_deviceid(di_event);

	ENTRY;

	nico_op_common_update_devinfo(dev_id, ctx->deviceinfo[dev_id]);
	if( ERR_OK == nico_op_common_set_autoip(dev_id, di_event, ctx->deviceinfo[dev_id]) ) return ERR_OK;
	nico_op_common_get_bluetooth_host(dev_id, di_event, event);

	switch(di_event->type)
	{
		case EVT_WIFI:
			protocol_encoder = __nico_parser_encode_wifi;
			break;
		case EVT_NETCONF:
			protocol_encoder = __nico_parser_encode_netconf;
			break;
		case EVT_CM:
			protocol_encoder = __nico_parser_encode_cm;
			break;
		case EVT_BT:
			protocol_encoder = __nico_parser_encode_bluetooth;
			break;
		case EVT_PPPOE:
			protocol_encoder = __nico_parser_encode_pppoe;
			break;
		default:
			protocol_encoder = NULL;
			break;
	}

	if(protocol_encoder)
	{
		protocol_encoder(di_event, event);
	}
	EXIT;
	return hRet;
}

HERROR nico_parser_deinit( void )
{
	HERROR hRet = ERR_OK;
	ENTRY;

	EXIT;
	return hRet;
}

