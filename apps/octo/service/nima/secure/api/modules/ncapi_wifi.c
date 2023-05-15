/*************************************************************************************************************
	File 		: ncapi_wifi.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/27
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


#include "ncapi_wifi.h"
#include "ncapi_client.h"
#include "ncapi_codec.h"

#include <sys/types.h>
#include <unistd.h>

/*******************************************************************/
/********************      typedef		        ********************/
/*******************************************************************/
typedef struct
{
	NCAPI_WifiAPInfo_t	apInfo;
	HUINT32	semaphore;
	HxList_t *callback_list;
} st_ncapi_wifi_object;
typedef st_ncapi_wifi_object* pst_ncapi_wifi_object;

st_ncapi_wifi_object g_ncapi_wifi_object =
{
	//.apInfo,
	.callback_list = NULL,
};


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL		HxSEMT_Get(g_ncapi_wifi_object.semaphore)
#define	LEAVE_CRITICAL		HxSEMT_Release(g_ncapi_wifi_object.semaphore)

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
STATIC void __ncapi_wifi_print_apinfo(NCAPI_WifiAPInfo_t *info)
{
	if(info)
	{
		HxLOG_Info("+++ [ncapi] Updated AP Information ++++++++++ \n");
		HxLOG_Info("channel       : %d \n", info->channel);
		HxLOG_Info("freq          : %d \n", info->freq);
		HxLOG_Info("is_wps        : %d \n", info->is_wps);
		HxLOG_Info("op_mode       : %d \n", info->op_mode);
		HxLOG_Info("wave_strength : %d \n", info->wave_strength);
		HxLOG_Info("connect_speed : %d \n", info->connect_speed);
		HxLOG_Info("secure_type   : %d \n", info->secure_type);
		HxLOG_Info("authen_type   : %d \n", info->authen_type);
		HxLOG_Info("ecrypt_type   : %d \n", info->encrypt_type);
		HxLOG_Info("keyindex      : %d \n", info->keyindex);
		HxLOG_Info("mac           : %02x-%02x-%02x-%02x-%02x-%02x \n",
										info->macaddr[0],info->macaddr[1],
										info->macaddr[2],info->macaddr[3],
										info->macaddr[4],info->macaddr[5]);
		HxLOG_Info("essid         : %s \n", info->essid);
		HxLOG_Info("id            : %s \n", info->id);
		HxLOG_Info("apkey         : %s \n", info->apkey);
		HxLOG_Info("++++++++++++++++++++++++++++++++++++++++++ \n");
	}
}
STATIC void __ncapi_wifi_update_apinfo(HINT32 apcount, NICO_Wifi_APInfo_t *src, NCAPI_WifiAPInfo_t *dst)
{
	HINT32 index=0;
	HxLOG_Debug("src : %p \n", src);
	HxLOG_Debug("dst : %p \n", dst);
	if(apcount == 0 && NULL != dst)
	{
		HxSTD_MemSet(dst, 0x00, sizeof(*dst));
	}
	else
	{
		for(index = 0; index < apcount; index++)
		{
			if(src && dst)
			{
				dst[index].channel = src[index].channel;
				dst[index].freq = src[index].freq;
				dst[index].is_wps = src[index].is_wps;
				dst[index].op_mode = src[index].op_mode;
				dst[index].wave_strength = src[index].wave_strength;
				dst[index].connect_speed = src[index].bitrate;
				dst[index].secure_type = src[index].secure_type;
				dst[index].authen_type = src[index].authen_type;
				dst[index].encrypt_type = src[index].encrypt_type;
				dst[index].keyindex = src[index].keyindex;
				dst[index].is_connected = src[index].is_connected;
				HxSTD_MemCopy(dst[index].macaddr, src[index].macaddr, sizeof(dst[index].macaddr));
				HxSTD_MemCopy(dst[index].essid, src[index].essid, sizeof(dst[index].essid));
				HxSTD_MemCopy(dst[index].id, src[index].id, sizeof(dst[index].id));
				HxSTD_MemCopy(dst[index].apkey, src[index].apkey, sizeof(dst[index].apkey));

				__ncapi_wifi_print_apinfo(&dst[index]);
			}
		}
	}
}

STATIC void __ncapi_wifi_update_ap(HINT32 apcount, NICO_Wifi_APInfo_t *src, NCAPI_WifiAPInfo_t *dst)
{
	HxLOG_Debug("src : %p \n", src);
	HxLOG_Debug("dst : %p \n", dst);

	if(NULL != dst)
	{
		if(apcount == 0)
		{
			HxSTD_MemSet(dst, 0x00, sizeof(*dst));
		}
		else if(NULL != src)
		{
			dst->channel = src->channel;
			dst->freq = src->freq;
			dst->is_wps = src->is_wps;
			dst->op_mode = src->op_mode;
			dst->wave_strength = src->wave_strength;
			dst->connect_speed = src->bitrate;
			dst->secure_type = src->secure_type;
			dst->authen_type = src->authen_type;
			dst->encrypt_type = src->encrypt_type;
			dst->keyindex = src->keyindex;
			dst->is_connected = src->is_connected;
			HxSTD_MemCopy(dst->macaddr, src->macaddr, sizeof(dst->macaddr));
			HxSTD_MemCopy(dst->essid, src->essid, sizeof(dst->essid));
			HxSTD_MemCopy(dst->id, src->id, sizeof(dst->id));
			HxSTD_MemCopy(dst->apkey, src->apkey, sizeof(dst->apkey));

			__ncapi_wifi_print_apinfo(dst);
		}
	}
}


STATIC void __ncapi_wifi_print_eventtype(eNico_event_type nico_evttype)
{
	switch(nico_evttype)
	{
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_USB_INSERTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_USB_EXTRACTED);

		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_SCAN_FINISHED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_SCAN_OUTPUT);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_SCAN_FAIL);

		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_AP_CONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_AP_CONNECT_FAIL);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_WPS_CONNECT_SUCCESS);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_WPS_CONNECT_FAIL);

		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_EXT_SCAN_STARTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_EXT_DISCONNECTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_EXT_RECONNECTED);
		CASE_EVT_ENUMTOSTR(EVENT_NET_WIFI_EXT_INVALID_KEY);
		default:
			break;
	}
}

//1 #### Wifi Event Listener
STATIC void __ncapi_wifi_client_listener (pevent_msg event)
{
	HINT32 ap_count = 0;
	eNcapiWifiEvent		ncapi_evttype = eNCAPI_WIFI_EVENT_UNKNOWN;
	eNico_event_type	nico_evttype  = event->type.evt_type;
	HINT32				device_id = event->device_id;
	NCAPI_WifiAPInfo_t	 *ap_info = NULL;
	ap_info = HLIB_STD_MemAlloc(sizeof(NCAPI_WifiAPInfo_t) * NCAPI_MAX_AP);

	HxSTD_MemSet(ap_info, 0x00, sizeof(NCAPI_WifiAPInfo_t) * NCAPI_MAX_AP);

	ENTRY;

	if(EVT_WIFI != (nico_evttype & 0xffff0000))
	{
	}
	else
	{
		__ncapi_wifi_print_eventtype(nico_evttype);
		switch(nico_evttype)
		{
			case EVENT_NET_WIFI_USB_INSERTED:
				ncapi_evttype = eNCAPI_WIFI_EVENT_INSERTED;
				break;
			case EVENT_NET_WIFI_USB_EXTRACTED:
				ncapi_evttype = eNCAPI_WIFI_EVENT_EXTRACTED;
				break;
			case EVENT_NET_WIFI_SCAN_FINISHED:
				{
					NICO_Wifi_APInfo_t *recvInfo = (NICO_Wifi_APInfo_t*)&event->evt_protocol.evt_wifi.evt.evt_scan.apinfo;
					ap_count = event->evt_protocol.evt_wifi.evt.evt_scan.apcount;
					__ncapi_wifi_update_apinfo(ap_count, recvInfo, ap_info);
					ncapi_evttype = eNCAPI_WIFI_EVENT_SCANNING_END;
				}
				break;
			case EVENT_NET_WIFI_SCAN_OUTPUT: // ????
				ncapi_evttype = eNCAPI_WIFI_EVENT_SCANNING_OUTPUT;
				break;
			case EVENT_NET_WIFI_SCAN_FAIL:
				ncapi_evttype = eNCAPI_WIFI_EVENT_SCANNING_FAIL;
				break;
			case EVENT_NET_WIFI_AP_CONNECT_SUCCESS:
				ap_count = 1;
				HxSTD_MemCopy(&ap_info[0], &event->evt_protocol.evt_wifi.evt.evt_connect.apinfo, sizeof(NICO_Wifi_APInfo_t));
				__ncapi_wifi_print_apinfo(&ap_info[0]);
				ncapi_evttype = eNCAPI_WIFI_EVENT_AP_CONNECT_SUCCESS;
				break;
			case EVENT_NET_WIFI_AP_CONNECT_FAIL:
				ncapi_evttype = eNCAPI_WIFI_EVENT_AP_CONNECT_FAIL;
				break;
			case EVENT_NET_WIFI_WPS_CONNECT_SUCCESS:
				ap_count = 1;
				HxSTD_MemCopy(&ap_info[0], &event->evt_protocol.evt_wifi.evt.evt_connect.apinfo, sizeof(NICO_Wifi_APInfo_t));
				__ncapi_wifi_print_apinfo(&ap_info[0]);
				ncapi_evttype = eNCAPI_WIFI_EVENT_WPS_SUCCESS;
				break;
			case EVENT_NET_WIFI_WPS_CONNECT_FAIL:
				ncapi_evttype = eNCAPI_WIFI_EVENT_WPS_FAIL;
				break;
			case EVENT_NET_WIFI_EXT_INVALID_KEY:
				ncapi_evttype = eNCAPI_WIFI_EVENT_KEY_INVALID;
				break;
			case EVENT_NET_WIFI_EXT_SCAN_STARTED:
				ncapi_evttype = eNCAPI_WIFI_EVENT_SCANNING_START;
				break;
			case EVENT_NET_WIFI_EXT_DISCONNECTED:
				ncapi_evttype = eNCAPI_WIFI_EVENT_AP_DISCONNECTED;
				break;
			case EVENT_NET_WIFI_EXT_RECONNECTED:
				ncapi_evttype = eNCAPI_WIFI_EVENT_AP_RECONNECTED;
				break;
			default:
				EXIT;
				if( NULL != ap_info ) HLIB_STD_MemFree(ap_info);
				return;
		}

		{
			HINT32 i = 0, cb_count = HLIB_LIST_Length(g_ncapi_wifi_object.callback_list);
			for(i = 0; i < cb_count ; i++)
			{
				HxList_t *pFound = HLIB_LIST_GetListItem(g_ncapi_wifi_object.callback_list, i);
				if(pFound)
				{
					NCAPI_WifiCallback callback = (NCAPI_WifiCallback)HLIB_LIST_Data(pFound);
					callback(ncapi_evttype, device_id, ap_count, ap_info);
				}
			}
		}
	}
	if( NULL != ap_info ) HLIB_STD_MemFree(ap_info);
	EXIT;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR NCAPI_Wifi_Init(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = HxSEMT_Create(&g_ncapi_wifi_object.semaphore, "ncapi_wifi_sem", 0);
	NCAPI_Client_RegisterCallback(__ncapi_wifi_client_listener);
	EXIT;
	return hRet;

}
HERROR NCAPI_Wifi_Deinit(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = HxSEMT_Destroy(g_ncapi_wifi_object.semaphore);
	NCAPI_Client_UnregisterCallback(__ncapi_wifi_client_listener);

	HxSTD_MemSet(&g_ncapi_wifi_object, 0x00, sizeof(st_ncapi_wifi_object));
	EXIT;
	return hRet;
}

HERROR NCAPI_Wifi_RegisterCallback(NCAPI_WifiCallback callback)
{
	HERROR hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;
	list = HLIB_LIST_Find(g_ncapi_wifi_object.callback_list, callback);
	if (list)
	{
		HxLOG_Error("[%s] pfnObjectHandler(%X) is already registered!!!\n", __FUNCTION__, callback);
		hRet = ERR_FAIL;
	}
	else
	{
		g_ncapi_wifi_object.callback_list = HLIB_LIST_Append(g_ncapi_wifi_object.callback_list, (void*)callback);
	}
	LEAVE_CRITICAL;
	EXIT;
	return hRet;
}

HERROR NCAPI_Wifi_UnregisterCallback(NCAPI_WifiCallback callback)
{
	HERROR	hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;

	list = HLIB_LIST_Find(g_ncapi_wifi_object.callback_list, callback);
	if (list == NULL)
	{
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", callback);
		hRet = ERR_FAIL;
	}
	else
	{
		g_ncapi_wifi_object.callback_list = HLIB_LIST_Remove(g_ncapi_wifi_object.callback_list, callback);
	}
	LEAVE_CRITICAL;
	EXIT;
	return hRet;

}

//sync
HINT32 NCAPI_Wifi_Get_AplistInfo(HINT32 id, HINT32 *ap_count, NCAPI_WifiAPInfo_t *info )
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_GET_WIFI_AP_INFO_LIST;
		hRet = NCAPI_Client_Send(msg);
		if(ERR_OK == hRet)
		{
			NICO_Wifi_APInfo_t *aplist = (NICO_Wifi_APInfo_t*)&msg->cmd_protocol.cmd_wifi.cmd.cmd_get_aplistinfo.aplist;
			*ap_count = msg->cmd_protocol.cmd_wifi.cmd.cmd_get_aplistinfo.ap_count;
			__ncapi_wifi_update_apinfo(*ap_count, aplist, info);
		}
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_Wifi_ScanStart(HINT32 id)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_WIFI_START_SCAN_AP;
		msg->cmd_protocol.cmd_wifi.cmd.cmd_set_startscan.id = id;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;

}

HERROR NCAPI_Wifi_ScanStop(HINT32 id)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_WIFI_STOP_SCAN_AP;
		msg->cmd_protocol.cmd_wifi.cmd.cmd_set_stopscan.id = id;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_Wifi_Connect(HINT32 id, const NCAPI_WifiAPInfo_t *info)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_WIFI_CONNECT_AP;
		msg->cmd_protocol.cmd_wifi.cmd.cmd_set_connectap.id = id;
		if(info)
		{
			HxSTD_MemCopy(&msg->cmd_protocol.cmd_wifi.cmd.cmd_set_connectap.apInfo,
							info, sizeof(NCAPI_WifiAPInfo_t));
		}
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_Wifi_Connect_byWPS(HINT32 id, const NCAPI_WifiWPSInfo_t *info)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_WIFI_CONNECT_WPS;
		msg->cmd_protocol.cmd_wifi.cmd.cmd_set_connectwps.id = id;
		if(info)
		{
			HxSTD_MemCopy(&msg->cmd_protocol.cmd_wifi.cmd.cmd_set_connectwps.wpsInfo,
							info, sizeof(NCAPI_WifiWPSInfo_t));
		}
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_Wifi_Disconnect(HINT32 id)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_WIFI_DISCONNECT_AP;
		msg->cmd_protocol.cmd_wifi.cmd.cmd_set_disconnectap.id = id;
		hRet = NCAPI_Client_Send(msg);
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}
HERROR NCAPI_Wifi_GetConnectedAPInfo(HINT32 id, HINT32 *apcount, NCAPI_WifiAPInfo_t *info)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_GET_WIFI_AP_INFO_CONNECTED;
		msg->cmd_protocol.cmd_wifi.cmd.cmd_get_connectedapinfo.id = id;
		hRet = NCAPI_Client_Send(msg);
		if(ERR_OK == hRet)
		{
			NICO_Wifi_APInfo_t *apinfo = &msg->cmd_protocol.cmd_wifi.cmd.cmd_get_connectedapinfo.apinfo;
			*apcount = msg->cmd_protocol.cmd_wifi.cmd.cmd_get_connectedapinfo.apcount;
			__ncapi_wifi_update_apinfo(*apcount, apinfo, info);
		}
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}


/**
* This function is used to configure WoWLAN feature
* @param[in] id: index of wifi network interface
* @param[in] mode: chosen mode
* @return ERR_OK if this WoWLAN feature is configured succesfully, ERR_FAIL if vice versa
*/

HERROR NCAPI_Wifi_SetWakeupOnWlan(HINT32 id, eNcapiWifiWoWlan mode)
{
	HERROR hRet = ERR_FAIL;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = TRUE;
		msg->device_id = id;
		msg->type.cmd_type = CMD_SET_WIFI_WOWLAN;
		msg->cmd_protocol.cmd_wifi.cmd.cmd_set_wowlan.ret = FALSE;
		msg->cmd_protocol.cmd_wifi.cmd.cmd_set_wowlan.id = id;
		msg->cmd_protocol.cmd_wifi.cmd.cmd_set_wowlan.mode = mode;

		hRet = NCAPI_Client_Send(msg);
		if(ERR_OK == hRet)
		{
			hRet = msg->cmd_protocol.cmd_wifi.cmd.cmd_set_wowlan.ret;
		}
		HLIB_STD_MemFree(msg);
	}
	EXIT;

	return hRet;
}

HERROR NCAPI_Wifi_GetPinCodebyWPS(HINT32 id, HCHAR *szPincode)
{
     HERROR hRet = ERR_OK;
     command_msg *msg = NULL;

     ENTRY;
     msg = HLIB_STD_MemAlloc(sizeof(*msg));
     if(NULL != msg)
     {
          HxSTD_MemSet(msg, 0x00, sizeof(*msg));
          msg->sync_call = TRUE;
          msg->device_id = id;
          msg->type.cmd_type = CMD_SET_WIFI_GET_WPS_PINCODE;
          msg->cmd_protocol.cmd_wifi.cmd.cmd_get_pincodewps.id = id;

          hRet = NCAPI_Client_Send(msg);
          if(ERR_OK == hRet)
          {
               HxSTD_MemCopy(szPincode, &msg->cmd_protocol.cmd_wifi.cmd.cmd_get_pincodewps.szPincode,
                                   NICO_MAX_WPS_CODE_LEN);
          }
         
          HLIB_STD_MemFree(msg);
     }
     EXIT;
    
     return hRet;
}



