/*************************************************************************************************************
	File 		: nico_netconf_lan.c
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
#include "nico_operator_netconf.h"
#include "nico_operator_netconf_util.h"

#include "di_err.h"
#include "di_network.h"


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
	LAN_RUNNING,
	LAN_STOPPED,
} e_netconf_state;

typedef struct _nico_netconf_context
{
	e_netconf_state state;

	HINT32 device_count;
	NICO_NetDevice_Info_t **deviceinfo;
} st_netconf_context;

typedef st_netconf_context* pst_netconf_context;

typedef struct _nico_netconf_object
{
	st_netconf_context obj_ctx;

	void  (*listener)(void *msg);
} st_netconf_object;
typedef st_netconf_object* pst_netconf_object;
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC st_netconf_object g_netconf_object;

/*******************************************************************/
/********************      Public 			  ******************/
/*******************************************************************/
pst_netconf_object netconf_object = &g_netconf_object;

/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
STATIC pst_netconf_context __nico_netconf_getcontext(void)
{
	return &netconf_object->obj_ctx;
}

STATIC void __nico_netconf_di_listener(void *param)
{
	ENTRY;

	if(netconf_object->listener)
	{
		netconf_object->listener(param);
	}
	EXIT;
}

STATIC void __nico_netconf_send_extendevent(HINT32 id, HUINT32 event_type)
{
	DI_NETWORK_CALLBACK_DATA_t evt = { .ulDevId = id, .eEvent = event_type};
	ENTRY;
	__nico_netconf_di_listener(&evt);
	EXIT;
}

STATIC HINT32 __nico_netconf_get_wol_deviceid(void)
{
	pst_netconf_context ctx = __nico_netconf_getcontext();
	HINT32 i = 0;
	ENTRY;
	for(i=0; i<ctx->device_count; i++)
	{
		if ( eNICO_DEVTYPE_LAN == ctx->deviceinfo[i]->netinfo.dev_type )
			break;
	}
	EXIT;
	return i;
}

STATIC HERROR __nico_netconf_init_dhcpserver(HINT32 dev_id, eNicoNetConfIpVersion eIpVer)
{
	HERROR hRet = ERR_OK;
	pst_netconf_context ctx = __nico_netconf_getcontext();
	NICO_DHCPSContext_t *dhcps = &ctx->deviceinfo[dev_id]->dhcpserverinfo;
	ENTRY;
	HxSTD_MemSet(dhcps, 0x00, sizeof(NICO_DHCPSContext_t));
	dhcps->is_run = FALSE;
	dhcps->eIPvType = eIpVer;

	nico_netconfutil_dhcps_init_info(&dhcps->config);
	EXIT;
	return hRet;
}

STATIC HERROR __nico_netconf_start_dhcpserver(HINT32 dev_id, eNicoNetConfIpVersion eIpVer)
{
	HERROR hRet = ERR_OK;
	DI_NETWORK_SET_INFO_t set_ipinfo;
	DI_NETWORK_DHCPS_PARAMS_t set_dhcps_param;
	pst_netconf_context ctx = __nico_netconf_getcontext();
	NICO_DHCPSContext_t *dhcps = &ctx->deviceinfo[dev_id]->dhcpserverinfo;
	NICO_DHCPSConf_Info_t *dhcpserverinfo = &dhcps->config;
	HULONG broadcast = 0;

	ENTRY;
	HxSTD_MemSet( &set_ipinfo, 0x00, sizeof(DI_NETWORK_SET_INFO_t) );
	HxSTD_MemSet( &set_dhcps_param, 0x00, sizeof(DI_NETWORK_DHCPS_PARAMS_t) );

	broadcast = (dhcpserverinfo->router & dhcpserverinfo->netmask) | ~dhcpserverinfo->netmask;
	HxSTD_MemCopy(set_ipinfo.stIpv4Addr.aucIpAddr, &dhcpserverinfo->router, sizeof(set_ipinfo.stIpv4Addr.aucIpAddr) );
	HxSTD_MemCopy(set_ipinfo.stIpv4Addr.aucMaskAddr, &dhcpserverinfo->netmask, sizeof(set_ipinfo.stIpv4Addr.aucMaskAddr) );
	HxSTD_MemCopy(set_ipinfo.stIpv4Addr.aucBcastAddr, &broadcast, sizeof(set_ipinfo.stIpv4Addr.aucBcastAddr) );
	HxSTD_MemCopy(set_dhcps_param.aucStart, dhcpserverinfo->startip, sizeof(set_dhcps_param.aucStart));
	HxSTD_MemCopy(set_dhcps_param.aucEnd, dhcpserverinfo->endip, sizeof(set_dhcps_param.aucEnd));
	{
		HUINT8 i=0;
		for(i=0;i<eNICO_Dns_Max;i++)
			HxSTD_MemCopy(set_dhcps_param.aucDns[i], dhcpserverinfo->dns[i], sizeof(set_dhcps_param.aucDns[i]));

		set_dhcps_param.aucSubnet[0] = 0xff & (dhcpserverinfo->netmask >> 24);
		set_dhcps_param.aucSubnet[1] = 0xff & (dhcpserverinfo->netmask >> 16);
		set_dhcps_param.aucSubnet[2] = 0xff & (dhcpserverinfo->netmask >> 8);
		set_dhcps_param.aucSubnet[3] = 0xff & (dhcpserverinfo->netmask);
	}
	HxSTD_MemCopy(set_dhcps_param.aucDomain, dhcpserverinfo->domain, sizeof(set_dhcps_param.aucDomain));
	set_dhcps_param.ulLease= dhcpserverinfo->leasetime;

	hRet = DI_NETWORK_SetDevInfo(dev_id, &set_ipinfo);

	switch ( eIpVer )
	{
		case eNICO_VER_IPV4 :
			{
				hRet = DI_NETWORK_StartDhcpServer( dev_id, DI_NETWORK_IP_VER_4, &set_dhcps_param );
			}
			break;
		case eNICO_VER_IPV6 :
			{
				//1 #### TODO: Convert ip4 to ip6 address
				hRet = DI_NETWORK_StartDhcpServer( dev_id, DI_NETWORK_IP_VER_6, &set_dhcps_param);
			}
			break;
		default:
			break;
	}

	if ( DI_ERR_OK == hRet ) dhcps->is_run = TRUE;
	else 					 dhcps->is_run = FALSE;
	EXIT;
	return hRet;
}
STATIC HERROR __nico_netconf_stop_dhcpserver(HINT32 dev_id, eNicoNetConfIpVersion eIpVer)
{
	HERROR hRet = ERR_OK;
	pst_netconf_context ctx = __nico_netconf_getcontext();
	NICO_DHCPSContext_t *dhcps = &ctx->deviceinfo[dev_id]->dhcpserverinfo;

	ENTRY;
	switch ( eIpVer )
	{
		case eNICO_VER_IPV4 :
			HxLOG_Debug("Try to stop dhcp-server(dev_id:%d / ipv4) !! \n", dev_id);
			hRet = DI_NETWORK_StopDhcpServer( dev_id, DI_NETWORK_IP_VER_4 );
			break;
		case eNICO_VER_IPV6 :
			HxLOG_Debug("Try to stop dhcp-server(dev_id:%d / ipv6) !! \n", dev_id);
			hRet = DI_NETWORK_StopDhcpServer( dev_id, DI_NETWORK_IP_VER_6 );
			break;
		default:
			break;
	}
	if ( DI_ERR_OK == hRet ) dhcps->is_run = FALSE;
	else 					 dhcps->is_run = TRUE;

	//2 #### TODO: make event and call-up


	EXIT;
	return hRet;
}
STATIC HERROR __nico_netconf_set_wol(HINT32 dev_id, HBOOL is_enable)
{
	HERROR hRet = ERR_OK;
#if 0
	hRet = DI_NETWORK_SetWakeOnLAN(dev_id, is_enable);
#endif
	return hRet;
}

STATIC HERROR __nico_netconf_set_autoip(HINT32 dev_id)
{
	HERROR eRet = ERR_OK;
	ENTRY;
	eRet = DI_NETWORK_StartAutoIp( dev_id );
	if(ERR_OK != eRet)
	{
		HxLOG_Error("Fail to set auto-ip (dev_id:%d) n", dev_id);
	}
	EXIT;
	return ERR_OK;

}

STATIC HINT32 __nico_netconf_get_activedeviceid(HINT32 device_count, NICO_NetDevice_Info_t **device)
{
	HINT32 device_id = -1;
	HINT32 index = 0;
	ENTRY;
	for(index = 0; index < device_count; index ++)
	{
		NICO_NetConf_Info_t *netinfo = &device[index]->netinfo;
		//1 #### First Top Priority
#if defined(CONFIG_MW_INET_PPPOE)
		if(eNICO_DEVTYPE_PPPOE == netinfo->dev_type &&
				TRUE == netinfo->linked &&
				TRUE == netinfo->active)
		{
			device_id = index;
			break;
		}
		else
#endif
		if(eNICO_DEVTYPE_LAN == netinfo->dev_type &&
						TRUE == netinfo->linked &&
						TRUE == netinfo->active &&
						(FALSE == HxSTD_StrEmpty(netinfo->ip_info.ipv4.ipAddr) ||
							(FALSE == HxSTD_StrEmpty(netinfo->ip_info.ipv6.ipAddr))))
		{
			device_id = index;
			break;
		}
		//2 #### Second Top Priority
		else if(eNICO_DEVTYPE_WIFI == netinfo->dev_type &&
				TRUE == netinfo->linked &&
				TRUE == netinfo->active &&
				(FALSE == HxSTD_StrEmpty(netinfo->ip_info.ipv4.ipAddr)
					|| (FALSE == HxSTD_StrEmpty(netinfo->ip_info.ipv6.ipAddr))))
		{
			device_id = index;
			break;
		}
		//3 #### Third Priority
		else if(eNICO_DEVTYPE_CM == netinfo->dev_type &&
				TRUE == netinfo->linked &&
				TRUE == netinfo->active &&
				(FALSE == HxSTD_StrEmpty(netinfo->ip_info.ipv4.ipAddr)
					|| (FALSE == HxSTD_StrEmpty(netinfo->ip_info.ipv6.ipAddr))))
		{
			device_id = index;
			break;
		}
		else if(eNICO_DEVTYPE_BLUETOOTH == netinfo->dev_type)
		{
		}
		else if(eNICO_DEVTYPE_RF4CE == netinfo->dev_type)
		{
		}
	}
	EXIT;
	return (device_id < 0 ? 0 : device_id);
}

STATIC HERROR __nico_netconf_renew(HCHAR *sh_path)
{
	HUINT32 dev_count = 0, i = 0;
	HCHAR szCmd[512] = {0,};
	DI_NETWORK_GET_INFO_t netinfo;

	if( NULL == sh_path )
		return ERR_FAIL;

	DI_NETWORK_GetDevCount(&dev_count);

	for( i=0; i<dev_count; i++ )
	{
		HxSTD_MemSet(&netinfo, 0x00, sizeof(netinfo));
		DI_NETWORK_GetDevInfo(i, &netinfo);
		if( netinfo.bIsUp && netinfo.bIsConnected )
		{
			HxSTD_snprintf(szCmd, sizeof(szCmd), "sh %s %s", sh_path, netinfo.szName);
			if( VK_OK != VK_SYSTEM_Command(szCmd) )
			{
				return ERR_FAIL;
			}
			break;
		}
	}

	return ERR_OK;
}


psyncreturn_msg __nico_netconf_operate_sync(command_msg *params)
{
	pst_netconf_context ctx = __nico_netconf_getcontext();
	Cmd_NetConf_t *pcommand =  (Cmd_NetConf_t*)&params->cmd_protocol.cmd_netconf;
	psyncreturn_msg retmsg = HLIB_STD_MemCalloc(sizeof(syncreturn_msg));

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
			case CMD_GET_NETCONF_ACTIVE_DEVICEID:
				{
					Cmd_NetConf_GetActiveDefaultDevIndex_t *cmdinfo = &pcommand->cmd.cmd_get_activedevice;
					(void*)cmdinfo;
					retmsg->cmd_protocol.cmd_netconf.cmd.cmd_get_activedevice.id = __nico_netconf_get_activedeviceid(ctx->device_count, ctx->deviceinfo);
				}
				break;
			case CMD_GET_NETCONF_DEVICE_COUNT:
				{
					Cmd_NetConf_GetDevCount_t *cmdinfo = &pcommand->cmd.cmd_get_devicecount;
					(void*)cmdinfo;
					retmsg->cmd_protocol.cmd_netconf.cmd.cmd_get_devicecount.device_count = ctx->device_count;
				}
				break;
			case CMD_GET_NETCONF_NETINFO:
				{
					Cmd_NetConf_GetNetInfo_t *cmdinfo = &pcommand->cmd.cmd_get_netinfo;
					HINT32 dev_id = cmdinfo->id;
					if(eNICO_DEVTYPE_WIFI == ctx->deviceinfo[dev_id]->netinfo.dev_type)
					{
						DI_WIFI_GetVersion(dev_id, ctx->deviceinfo[dev_id]->netinfo.fw_ver);
						HxLOG_Debug("Set WIFI firmware version value: %s.\n",ctx->deviceinfo[dev_id]->netinfo.fw_ver);
					}

					HxSTD_MemCopy(&retmsg->cmd_protocol.cmd_netconf.cmd.cmd_get_netinfo.info,
									&ctx->deviceinfo[dev_id]->netinfo, sizeof(NICO_NetConf_Info_t));


					retmsg->cmd_protocol.cmd_netconf.cmd.cmd_get_netinfo.info.id = dev_id;
				}
				break;
			case CMD_SET_NETCONF_DEVICE_ENABLE:
				{
					Cmd_NetConf_IfUp_t *cmdinfo = &pcommand->cmd.cmd_set_devup;
					HINT32 dev_id = cmdinfo->id;
#if defined(CONFIG_USES_NFSROOT)
					__nico_netconf_send_extendevent(cmdinfo->id, EVENT_NET_EXT_PLUGGED_IN);
					retmsg->cmd_protocol.cmd_netconf.cmd.cmd_set_devup.is_up = TRUE;
#else
					if(ERR_OK == nico_op_common_interface_updown(cmdinfo->id, TRUE))
					{
						__nico_netconf_send_extendevent(cmdinfo->id, EVENT_NET_EXT_PLUGGED_IN);
						retmsg->cmd_protocol.cmd_netconf.cmd.cmd_set_devup.is_up = TRUE;
					}
					else
					{
						__nico_netconf_send_extendevent(cmdinfo->id, EVENT_NET_EXT_PLUGGED_OUT);
						retmsg->cmd_protocol.cmd_netconf.cmd.cmd_set_devup.is_up = FALSE;
					}
#endif
					retmsg->cmd_protocol.cmd_netconf.cmd.cmd_set_devup.id = dev_id;
				}
				break;
			case CMD_SET_NETCONF_DEVICE_DISABLE:
				{
					Cmd_NetConf_IfDown_t *cmdinfo = &pcommand->cmd.cmd_set_devdown;
					HINT32 dev_id = cmdinfo->id;
#if defined(CONFIG_USES_NFSROOT)
					__nico_netconf_send_extendevent(cmdinfo->id, EVENT_NET_EXT_PLUGGED_OUT);
					retmsg->cmd_protocol.cmd_netconf.cmd.cmd_set_devdown.is_down = TRUE;
#else
					if(ERR_OK == nico_op_common_interface_updown(cmdinfo->id, FALSE))
					{
						__nico_netconf_send_extendevent(cmdinfo->id, EVENT_NET_EXT_PLUGGED_OUT);
						retmsg->cmd_protocol.cmd_netconf.cmd.cmd_set_devdown.is_down = TRUE;
					}
					else
					{
						__nico_netconf_send_extendevent(cmdinfo->id, EVENT_NET_EXT_PLUGGED_IN);
						retmsg->cmd_protocol.cmd_netconf.cmd.cmd_set_devdown.is_down = FALSE;
					}
#endif
					retmsg->cmd_protocol.cmd_netconf.cmd.cmd_set_devdown.id = dev_id;
				}
				break;
				case CMD_SET_NETCONF_RENEW:
				{
#if defined (CONFIG_ENHANCED_SECURITY)
					Cmd_NetConf_SetRenew_t *cmdinfo = &pcommand->cmd.cmd_set_renew;
					if(ERR_OK == __nico_netconf_renew(cmdinfo->addr))
					{
						retmsg->cmd_protocol.cmd_netconf.cmd.cmd_set_renew.is_set_renew = TRUE;
					}
					else
					{
						retmsg->cmd_protocol.cmd_netconf.cmd.cmd_set_renew.is_set_renew = FALSE;
					}
#endif
				}
				break;
			default:
				break;
		}
	}
	return retmsg;
}

void	__nico_netconf_operate_async(command_msg *params)
{
	pst_netconf_context ctx = __nico_netconf_getcontext();
	Cmd_NetConf_t *pcommand =  (Cmd_NetConf_t*)&params->cmd_protocol.cmd_netconf;
	switch(params->type.cmd_type)
	{
		case CMD_SET_NETCONF_DEVICE_CONFIG:
			{
				Cmd_NetConf_SetNetInfo_t *cmdinfo = &pcommand->cmd.cmd_set_netinfo;
				NICO_NetConf_Info_t *cmd_netinfo = &cmdinfo->info;
				HINT32 dev_id = cmdinfo->id;
#if defined(CONFIG_USES_NFSROOT)
				if(cmd_netinfo->dev_type == eNICO_DEVTYPE_LAN) //Keep the Ethernet connection for NFS
				{

					ctx->deviceinfo[dev_id]->netinfo.dhcp = cmd_netinfo->dhcp;
					__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_CONNECT_SUCCESS);
				}
				else // Same as the release mode
				{
					if(ERR_FAIL == nico_op_common_set_config(dev_id, ctx->deviceinfo[dev_id], cmd_netinfo))
					{
						__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_CONNECT_FAIL);
					}
					else
					{
						if(cmd_netinfo->dhcp == FALSE)
							__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_CONNECT_SUCCESS);
					}
				}
#else
				if(ERR_FAIL == nico_op_common_set_config(dev_id, ctx->deviceinfo[dev_id], cmd_netinfo))
				{
					__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_CONNECT_FAIL);
				}
				else
				{
					if(cmd_netinfo->dhcp == FALSE)
						__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_CONNECT_SUCCESS);
				}
#endif
			}
			break;
		case CMD_SET_NETCONF_RESTART_DHCP_CLIENT:
			{
				Cmd_NetConf_StopConnect_t *cmdinfo = &pcommand->cmd.cmd_set_stopconnecting;
				HINT32 dev_id = cmdinfo->id;
				nico_op_common_restart_dhcp_connecting(dev_id, ctx->deviceinfo[dev_id]->netinfo.ip_ver);
				HxLOG_Warning("Restart DHCP client\n");
			}
			break;
		case CMD_SET_NETCONF_STOP_CONNECTING:
			{
				Cmd_NetConf_StopConnect_t *cmdinfo = &pcommand->cmd.cmd_set_stopconnecting;
				HINT32 dev_id = cmdinfo->id;

#if defined(CONFIG_USES_NFSROOT)
				__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_STOP_CONNECTING);
#else
				if(ctx->deviceinfo[dev_id]->netinfo.dhcp == TRUE)
				{
					if(ERR_OK == nico_op_common_stop_dhcp_connecting(dev_id, ctx->deviceinfo[dev_id]->netinfo.ip_ver))
						__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_STOP_CONNECTING);
				}
				else
				{
					if(ERR_OK == nico_op_common_stop_menual_connecting(dev_id, ctx->deviceinfo[dev_id]->netinfo.ip_ver))
						__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_STOP_CONNECTING);
				}
#endif
				//1 	#### TODO? after stop wired, will be start wireless???

			}
			break;
		case CMD_SET_NETCONF_START_DHCPSERVER:
			{
				Cmd_NetConf_DHCPServer_t *cmdinfo = &pcommand->cmd.cmd_set_dhcpserver;
				HINT32 dev_id = cmdinfo->id;
#if defined(CONFIG_USES_NFSROOT)
				__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_STOPPED_DHCPSERVER);
				(void)__nico_netconf_init_dhcpserver;
				(void)__nico_netconf_start_dhcpserver;
				(void)__nico_netconf_stop_dhcpserver;
#else
				eNicoNetConfIpVersion eIpVer = cmdinfo->eIpVer;
				__nico_netconf_init_dhcpserver(dev_id, eIpVer);

				if(ERR_OK == __nico_netconf_start_dhcpserver(dev_id, eIpVer))
					__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_STARTED_DHCPSERVER);
				else
					__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_STOPPED_DHCPSERVER);
#endif
			}
			break;
		case CMD_SET_NETCONF_STOP_DHCPSERVER:
			{
				Cmd_NetConf_DHCPServer_t *cmdinfo = &pcommand->cmd.cmd_set_dhcpserver;
#if defined(CONFIG_USES_NFSROOT)
				(void*)cmdinfo;
#else
				HINT32 dev_id = cmdinfo->id;
				eNicoNetConfIpVersion eIpVer = cmdinfo->eIpVer;
				if(ERR_OK == __nico_netconf_stop_dhcpserver(dev_id, eIpVer))
					__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_STOPPED_DHCPSERVER);
#endif
			}
			break;
		case CMD_SET_NETCONF_WOL:
			{
				Cmd_NetConf_WOL_t *cmdinfo = &pcommand->cmd.cmd_set_wol;
				HINT32 dev_id = __nico_netconf_get_wol_deviceid();
				HBOOL is_enable = cmdinfo->is_enable;
				if(ERR_OK == __nico_netconf_set_wol(dev_id, is_enable))
					__nico_netconf_send_extendevent(dev_id, EVENT_NET_WOL_ENABLED);
				else
					__nico_netconf_send_extendevent(dev_id, EVENT_NET_WOL_DISABLED);
			}
			break;
		case CMD_SET_NETCONF_AUTOIP:
			{
				Cmd_NetConf_SetAutoip_t *cmdinfo = &pcommand->cmd.cmd_set_autoip;
				HINT32 dev_id = cmdinfo->id;
#if defined(CONFIG_USES_NFSROOT)
				__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_CONNECT_AUTOIP_FAIL);
				(void)__nico_netconf_set_autoip;
#else
				if(ERR_OK != __nico_netconf_set_autoip(dev_id))
					__nico_netconf_send_extendevent(dev_id, EVENT_NET_EXT_CONNECT_AUTOIP_FAIL);
#endif
			}
			break;
		case CMD_SET_NETCONF_DEVICE_PRIORITY:
			{
				Cmd_NetConf_SetDevPriority_t *cmdinfo = &pcommand->cmd.cmd_set_devpriority;
				HINT32 dev_id = cmdinfo->id;
				ctx->deviceinfo[dev_id]->netinfo.priority = cmdinfo->priority;
			}
			break;
		case CMD_SET_NETCONF_NTP_CLIENT:
			{
				HCHAR szCmd[512] = {0,};
				Cmd_NetConf_SetNTPClent_t *cmdinfo = &pcommand->cmd.cmd_set_ntpclient;

				HxSTD_snprintf(szCmd, 512, "ntpclient -s -h %s", cmdinfo->addr);
				HxLOG_Debug("+++++ NTP Clent ADDR : %s +++++ \n", szCmd);
				VK_SYSTEM_Command(szCmd);
			}
			break;
		default:
			break;
	}
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR nico_netconf_init( HINT32 dev_count, NICO_NetDevice_Info_t *devices[] )
{
	HERROR hRet = ERR_OK;
	HUINT32	i = 0, j = 0;
	pst_netconf_context ctx = __nico_netconf_getcontext();

	ENTRY;
	for(i=0; i < dev_count; i++)
	{
		for(j=0; j<DI_NETWORK_EVENT_MAX; j++)
		{
			hRet = DI_NETWORK_RegisterEventCallback(i, (DI_NETWORK_EVENT_e)j, __nico_netconf_di_listener);
			if(hRet != ERR_OK)
				HxLOG_Error("fail regist wifi callback function. \n");
		}
	}

	ctx->device_count = dev_count;
	ctx->deviceinfo = devices;

	for(i = 0; i < dev_count; i++)
	{
		ctx->deviceinfo[i]->netinfo.dhcp = TRUE;
	}

	EXIT;
	return hRet;
}


psyncreturn_msg nico_netconf_operate( command_msg *params )
{
	psyncreturn_msg retmsg = NULL;
	ENTRY;
	if(TRUE == params->sync_call)
	{
		retmsg = __nico_netconf_operate_sync(params);
	}
	else
	{
		__nico_netconf_operate_async(params);
	}

	EXIT;
	return retmsg;
}

HERROR nico_netconf_deinit( void )
{
	HERROR hRet = ERR_OK;
	ENTRY;

	if(netconf_object) netconf_object->listener = NULL;

	EXIT;
	return hRet;
}

HERROR nico_netconf_register_callback( void *callback )
{
	ENTRY;

	if( netconf_object )
		netconf_object->listener = callback;

	EXIT;
	return ERR_OK;
}

