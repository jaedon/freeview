/*************************************************************************************************************
	File 		: nico_operator_wifi.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/16
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
#include "nico_debug.h"
#include "nico_operator_wifi.h"
#include "nico_operator_wifi_util.h"

#include "di_wifi.h"
#include "di_network.h"
#include "drv_err.h"
#include "drv_hotplug.h"

/*******************************************************************/
/*******************      template         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	WIFI_RUNNING,
	WIFI_STOPPED,
} e_wifi_state;

typedef struct
{
	e_wifi_state state;
	NICO_Wifi_APInfo_t apinfo[NICO_MAX_AP];

	HULONG msgqid;
	HULONG	tid;

	HINT32 device_count;
	NICO_NetDevice_Info_t **deviceinfo;
} st_wifi_context;

typedef st_wifi_context* pst_wifi_context;

typedef struct
{
	st_wifi_context obj_ctx;
	void  (*listener)(void *dimsg);
} st_wifi_object;
typedef st_wifi_object* pst_wifi_object;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC st_wifi_object g_wifi_object;

/*******************************************************************/
/********************      Public 			  ******************/
/*******************************************************************/

pst_wifi_object wifi_object = &g_wifi_object;

/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
STATIC pst_wifi_context __nico_wifi_getcontext(void)
{
	return &wifi_object->obj_ctx;
}

#if defined(CONFIG_MW_INET_WLAN)
STATIC HERROR	__nico_wifi_scan_start(HINT32 id)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	if ( DI_ERR_OK != DI_WIFI_Scan( id ) )
	{
		hRet = ERR_FAIL;
	}
	EXIT;
	return hRet;
}
STATIC HERROR	__nico_wifi_scan_stop(HINT32 id)
{
	HERROR hRet = ERR_OK;

	return hRet;
}

STATIC HERROR __nico_wifi_check_key(NICO_Wifi_APInfo_t *apinfo)
{
	HERROR hRet = ERR_OK;
	switch(apinfo->secure_type)
	{
		case eNICO_WIFI_SECURITY_OPEN:
			break;
		case eNICO_WIFI_SECURITY_WEP:
		case eNICO_WIFI_SECURITY_WPA:
		case eNICO_WIFI_SECURITY_WPA2:
			if(FALSE == nico_wifiutil_is_validkey(apinfo->encrypt_type, apinfo->apkey))
				hRet = ERR_FAIL;
			break;
		case eNICO_WIFI_SECURITY_UNKNOWN:
		default:
			break;
	}
	return hRet;
}

STATIC HERROR __nico_wifi_connect_ap(HINT32 id, NICO_Wifi_APInfo_t *apinfo)
{
	HERROR hRet = ERR_OK;
	const HUINT32 wait_time = 45;
	DI_WIFI_INFO_t	stWiFiInfo;
	HxSTD_MemSet( &stWiFiInfo, 0x00, sizeof(DI_WIFI_INFO_t) );
	ENTRY;
	if(TRUE == nico_op_common_wifi_is_connected(id)) DI_WIFI_Disconnect(id);

	nico_debug_print_apinfo(apinfo);

	stWiFiInfo.ulChannel = apinfo->channel;
	stWiFiInfo.ulFreq = apinfo->freq;
	stWiFiInfo.eSecureType = (DI_WIFI_SECURE_e)apinfo->secure_type;
	stWiFiInfo.eAuthenType = (DI_WIFI_AUTHEN_e)apinfo->authen_type;
	stWiFiInfo.eEncryptType = (DI_WIFI_ENCRYPT_e)apinfo->encrypt_type;
	HxSTD_MemCopy(stWiFiInfo.aucESSID, apinfo->essid, sizeof(stWiFiInfo.aucESSID));
	HxSTD_MemCopy(stWiFiInfo.aucKey, apinfo->apkey, sizeof(stWiFiInfo.aucKey));
	HxLOG_Warning("DI_WIFI_Connect: id=%d SSID:%s, Pass:%s SAE: %d%d%d\n", id, stWiFiInfo.aucESSID, stWiFiInfo.aucKey, stWiFiInfo.eSecureType, stWiFiInfo.eAuthenType, stWiFiInfo.eEncryptType);
	hRet = DI_WIFI_Connect( id, DI_WIFI_WPS_NONE, &stWiFiInfo, wait_time );

	EXIT;
	return hRet;
}

STATIC HERROR __nico_wifi_connect_ap_bywps(HINT32 id, NICO_Wifi_WPSInfo_t *wpsinfo)
{
	HERROR hRet = ERR_OK;
	HUINT32 i = 0, wait_time = 120;
	DI_WIFI_INFO_t	stWiFiInfo;
	HxSTD_MemSet( &stWiFiInfo, 0x00, sizeof(DI_WIFI_INFO_t) );
	ENTRY;
	if(TRUE == nico_op_common_wifi_is_connected(id)) DI_WIFI_Disconnect(id);
	
	if ( eNICO_WIFI_WPS_PIN == wpsinfo->eType )
	{
		for ( i = 0; i < NICO_MAX_WPS_CODE_LEN; i++)
		{
			stWiFiInfo.aucKey[i]  = wpsinfo->szCode[i] - '0';
		}
	}

	hRet = DI_WIFI_Connect( id, nico_op_common_get_wpstype(wpsinfo->eType), &stWiFiInfo, wait_time );

	EXIT;
	return hRet;
}

STATIC HERROR __nico_wifi_get_pincode_bywps(HINT32 id, HCHAR *szPincode)
{
	HERROR hRet = ERR_OK;
	HCHAR  szTempPincode[NICO_MAX_WPS_CODE_LEN] = {0, };
	ENTRY;

	if (DI_ERR_OK != DI_WIFI_GetWpsPinCode(id, szTempPincode))
	{
		hRet = ERR_FAIL;
	}

	//FIX ME : for test
#if 0
	HxSTD_MemCopy(szPincode, "12345678", NICO_MAX_WPS_CODE_LEN);
#else
	HxSTD_MemCopy(szPincode, szTempPincode, NICO_MAX_WPS_CODE_LEN);
#endif

	EXIT;
	return hRet;
}

STATIC HERROR __nico_wifi_disconnect_ap(HINT32 id)
{
	HERROR hRet = ERR_FAIL;
	if(DI_ERR_OK == DI_WIFI_Disconnect( id ))
	{
		hRet = ERR_OK;
	}
	return hRet;
}

STATIC HINT32 __nico_wifi_get_apinfolist(HINT32 dev_id, NICO_Wifi_APInfo_t *aplist)
{
	pst_wifi_context ctx = __nico_wifi_getcontext();
	HINT32 index=0;
	HINT32 ap_count = 0;
	for(index = 0 ; index < NICO_MAX_AP; index++)
	{
		if(HxSTD_StrEmpty(ctx->deviceinfo[dev_id]->apinfo_list[index].essid))
			continue;
		HxSTD_MemCopy(&aplist[ap_count++], &ctx->deviceinfo[dev_id]->apinfo_list[index], sizeof(NICO_Wifi_APInfo_t));
	}
	return ap_count;
}

STATIC NICO_Wifi_APInfo_t *__nico_wifi_get_connected_apinfo(HINT32 dev_id)
{
	pst_wifi_context ctx = __nico_wifi_getcontext();

	if(TRUE == ctx->deviceinfo[dev_id]->netinfo.linked)
	{
		nico_op_common_get_connected_apinfo(dev_id, &ctx->deviceinfo[dev_id]->apinfo_connected);
	}
	else
	{
		HxSTD_MemSet(&ctx->deviceinfo[dev_id]->apinfo_connected, 0x00,
						sizeof(ctx->deviceinfo[dev_id]->apinfo_connected));
	}
	nico_debug_print_apinfo(&ctx->deviceinfo[dev_id]->apinfo_connected);
	return &ctx->deviceinfo[dev_id]->apinfo_connected;
}

STATIC void	__nico_wifi_di_listener(void *param)
{
	if(wifi_object->listener)
	{
		wifi_object->listener(param);
	}
}

STATIC void __nico_wifi_send_extendevent(HINT32 id, HUINT32 event_type)
{
	DI_WIFI_CALLBACK_DATA_t evt = {.ulDevId = id, .eEvent = event_type};
	__nico_wifi_di_listener(&evt);
}

/**
* This function is used to configure WoWLAN feature
* @param[in] id: index of wifi network interface
* @param[in] mode: chosen mode
* @return ERR_OK if this WoWLAN feature is configured succesfully, ERR_FAIL if vice versa
*/

STATIC HERROR __nico_wifi_set_wakeup_on_wlan(HINT32 id, eNicoWifiWoWlan mode)
{
	DI_WIFI_WOWL_SETTINGS_t stWowlSettings;
	HERROR hRet = ERR_OK;
	ENTRY;
	HxSTD_MemSet( &stWowlSettings, 0x00, sizeof(DI_WIFI_WOWL_SETTINGS_t) );
	stWowlSettings.eWowlMask = mode;

	if (DI_ERR_OK != DI_WIFI_SetWakeOnWLAN(id, &stWowlSettings))
	{
		hRet = ERR_FAIL;
	}
	EXIT;
	return hRet;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR nico_wifi_init( HINT32 dev_count, NICO_NetDevice_Info_t **devices )
{
	HERROR hRet = ERR_OK;
	HUINT32	i = 0, j = 0;
	pst_wifi_context ctx = __nico_wifi_getcontext();
	ENTRY;
	for(i=0; i < dev_count; i++)
	{
		if(eNICO_DEVTYPE_WIFI == devices[i]->netinfo.dev_type)
		{
			for(j=0; j<DI_WIFI_NOTIFY_EVT_MAX; j++)
			{
				hRet = DI_WIFI_RegisterEventCallback(i, (DI_WIFI_NOTIFY_EVT_e)j, __nico_wifi_di_listener);
				if(hRet != ERR_OK)
					HxLOG_Error("fail regist wifi callback function. \n");
			}

		}
	}
	ctx->device_count = dev_count;
	ctx->deviceinfo = devices;
	EXIT;
	return hRet;
}

psyncreturn_msg __nico_wifi_operate_sync(command_msg *params)
{
	psyncreturn_msg retmsg = HLIB_STD_MemCalloc(sizeof(syncreturn_msg));
	Cmd_Wifi_t *pcommand =  (Cmd_Wifi_t*)&params->cmd_protocol.cmd_wifi;

	if(NULL == retmsg)
	{
		HxLOG_Error("Out of Memory +++ \n");
	}
	else
	{
		HxSTD_MemCopy(retmsg, params, sizeof(syncreturn_msg));
		retmsg->type.evt_type = params->type.cmd_type;
		switch(params->type.cmd_type)
		{
			case CMD_SET_WIFI_GET_WPS_PINCODE:
				{
					HCHAR  szPincode[NICO_MAX_WPS_CODE_LEN] = {0, };
					Cmd_Wifi_GetPincodeWPS_t *cmdinfo = &pcommand->cmd.cmd_get_pincodewps;
					HINT32 dev_id = cmdinfo->id;

					__nico_wifi_get_pincode_bywps(dev_id, szPincode);
					retmsg->cmd_protocol.cmd_wifi.cmd.cmd_get_pincodewps.id = cmdinfo->id;
					if(szPincode != NULL)
					{
						HxSTD_MemCopy(&retmsg->cmd_protocol.cmd_wifi.cmd.cmd_get_pincodewps.szPincode,
										szPincode, NICO_MAX_WPS_CODE_LEN);
					}
				}
				break;
			case CMD_SET_WIFI_WOWLAN:
				{
					Cmd_Wifi_Set_WoWlan_t *cmdinfo = &pcommand->cmd.cmd_set_wowlan;
					retmsg->cmd_protocol.cmd_wifi.cmd.cmd_set_wowlan.ret = __nico_wifi_set_wakeup_on_wlan(cmdinfo->id, cmdinfo->mode);
					retmsg->cmd_protocol.cmd_wifi.cmd.cmd_set_wowlan.id = cmdinfo->id;
					retmsg->cmd_protocol.cmd_wifi.cmd.cmd_set_wowlan.mode = cmdinfo->mode;
				}
				break;
			case CMD_GET_WIFI_AP_INFO_CONNECTED:
				{
					Cmd_Wifi_ConnectedApInfo_t *cmdinfo = &pcommand->cmd.cmd_get_connectedapinfo;
					HINT32 dev_id = cmdinfo->id;
					NICO_Wifi_APInfo_t *apinfo = __nico_wifi_get_connected_apinfo(dev_id);
					retmsg->cmd_protocol.cmd_wifi.cmd.cmd_get_connectedapinfo.id = dev_id;
					if(apinfo)
					{
						retmsg->cmd_protocol.cmd_wifi.cmd.cmd_get_connectedapinfo.apcount = 1;
						HxSTD_MemCopy(&retmsg->cmd_protocol.cmd_wifi.cmd.cmd_get_connectedapinfo.apinfo,
										apinfo, sizeof(NICO_Wifi_APInfo_t));
					}
				}
				break;
			case CMD_GET_WIFI_AP_INFO_LIST:
				{
					Cmd_Wifi_GetApInfoList_t *cmdinfo = &pcommand->cmd.cmd_get_aplistinfo;
					HINT32 dev_id = cmdinfo->id;
					HINT32 ap_count = __nico_wifi_get_apinfolist(dev_id, cmdinfo->aplist);
					retmsg->cmd_protocol.cmd_wifi.cmd.cmd_get_aplistinfo.id = dev_id;
					retmsg->cmd_protocol.cmd_wifi.cmd.cmd_get_aplistinfo.ap_count = ap_count;
					HxSTD_MemCopy(&retmsg->cmd_protocol.cmd_wifi.cmd.cmd_get_aplistinfo,
									cmdinfo->aplist, sizeof(NICO_Wifi_APInfo_t)*NICO_MAX_AP);
				}
				break;
			case CMD_UNKNOWN:
			default:
				break;
		}
	}
	return retmsg;
}

void __nico_wifi_operate_async(command_msg *params)
{
	pst_wifi_context ctx = __nico_wifi_getcontext();
	Cmd_Wifi_t *pcommand =  (Cmd_Wifi_t*)&params->cmd_protocol.cmd_wifi;

	switch(params->type.cmd_type)
	{
		case CMD_SET_WIFI_START_SCAN_AP:
			{
				Cmd_Wifi_StartScan_t *cmdinfo = &pcommand->cmd.cmd_set_startscan;

				if(ERR_OK == __nico_wifi_scan_start(cmdinfo->id))
					__nico_wifi_send_extendevent(cmdinfo->id, EVENT_NET_WIFI_EXT_SCAN_STARTED);
				else
					__nico_wifi_send_extendevent(cmdinfo->id, EVENT_NET_WIFI_EXT_SCAN_FAIL);
			}
			break;
		case CMD_SET_WIFI_STOP_SCAN_AP:
			{
				Cmd_Wifi_StopScan_t *cmdinfo = &pcommand->cmd.cmd_set_stopscan;
				if(ERR_OK == __nico_wifi_scan_stop(cmdinfo->id))
					__nico_wifi_send_extendevent(cmdinfo->id, EVENT_NET_WIFI_EXT_SCAN_FINISHED);
				else
					__nico_wifi_send_extendevent(cmdinfo->id, EVENT_NET_WIFI_EXT_SCAN_FAIL);
			}
			break;
		case CMD_SET_WIFI_CONNECT_AP:
			{
				Cmd_Wifi_ConnectAp_t *cmdinfo = &pcommand->cmd.cmd_set_connectap;
				HINT32 dev_id = cmdinfo->id;
				NICO_Wifi_APInfo_t *apinfo = &cmdinfo->apInfo;

				ctx->deviceinfo[dev_id]->is_trywps = FALSE;
				if(ERR_OK != __nico_wifi_check_key(apinfo))
					__nico_wifi_send_extendevent(dev_id, EVENT_NET_WIFI_EXT_INVALID_KEY);
				else
				{
					if(ERR_OK != __nico_wifi_connect_ap(dev_id, apinfo))
						__nico_wifi_send_extendevent(dev_id, EVENT_NET_WIFI_EXT_AP_CONNECT_FAIL);
				}
			}
			break;
		case CMD_SET_WIFI_DISCONNECT_AP:
			{
				Cmd_Wifi_DisconnectAp_t *cmdinfo = &pcommand->cmd.cmd_set_disconnectap;
				HINT32 dev_id = cmdinfo->id;
				if(ERR_OK == __nico_wifi_disconnect_ap(dev_id))
					__nico_wifi_send_extendevent(dev_id, EVENT_NET_WIFI_EXT_DISCONNECTED);
			}
			break;
		case CMD_SET_WIFI_CONNECT_WPS:
			{
				Cmd_Wifi_ConnectWps_t *cmdinfo = &pcommand->cmd.cmd_set_connectwps;
				HINT32 dev_id = cmdinfo->id;
				NICO_Wifi_WPSInfo_t *wpsinfo = &cmdinfo->wpsInfo;
				ctx->deviceinfo[dev_id]->is_trywps = TRUE;
				if(ERR_OK != __nico_wifi_connect_ap_bywps(dev_id, wpsinfo))
					__nico_wifi_send_extendevent(dev_id, EVENT_NET_WIFI_EXT_WPS_CONNECT_FAIL);
			}
		case CMD_UNKNOWN:
		default:

			break;
	}
}
psyncreturn_msg nico_wifi_operate( command_msg *params )
{
	psyncreturn_msg retmsg = NULL;
	if(TRUE == params->sync_call)
	{
		retmsg = __nico_wifi_operate_sync(params);
	}
	else
	{
		__nico_wifi_operate_async(params);
	}
	return retmsg;
}

HERROR nico_wifi_deinit( void )
{
	HERROR hRet = ERR_OK;
	pst_wifi_context ctx = __nico_wifi_getcontext();

	ctx->state = WIFI_STOPPED;
	if(wifi_object) wifi_object->listener = NULL;

	return hRet;
}

HERROR nico_wifi_register_callback( void *callback )
{
	ENTRY;

	if( wifi_object )
		wifi_object->listener = callback;

	EXIT;
	return ERR_OK;
}

#else

HERROR nico_wifi_init( HINT32 dev_count, NICO_NetDevice_Info_t **devices ){return ERR_FAIL;}
psyncreturn_msg nico_wifi_operate( command_msg *params ){return NULL;}
HERROR nico_wifi_deinit( void ){return ERR_FAIL;}
HERROR nico_wifi_register_callback( void *callback ){return ERR_FAIL;}


#endif

