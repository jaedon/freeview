/**************************************************************
 *	@file		apk_network_netconfig.c
 *	appkit for network
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013-07-15
 *	@author			humax
 *	@brief			appkit for network
 **************************************************************/
#include 	<apk.h>
#include 	<oapi.h>
#include 	<napi.h>

#include	"_apk_int.h"

#if defined(CONFIG_SUPPORT_NIMA)

static	HxList_t *_NetWork_object_handler = NULL;
static HBOOL s_audio_mute = FALSE;
#if defined(CONFIG_OP_TDC_NETWORK_LED)
static HULONG 	s_ulLedTimer 			= 0;
static HINT32	s_nCurLEDColor 				= eDxRGBLED_COLOR_GREEN;
static HINT32	s_nDimCnt				 	= 0;
#endif

#define	APKD_NETWORK_AP_ESSID		48
#define	APKD_NETWORK_AP_KEY			64

#if defined(CONFIG_OP_TDC_NETWORK_LED)
#define LED_GREEN_RED_INTERVAL		700 	/* 0.7 sec */
#define LED_ASSOCIATED_DIM_INTERVAL	2000	/* 2 sec -> 2 sec after association the LED should be dimed */
#define LED_WPS_FAIL_DIM_INTERVAL	30000	/* 30 sec -> LED dimmed after 30 sec */
#define LED_FAIL_OFF_INTEVAL		180000	/* 180 sec -> LED turned off after 180 sec. */
#define LED_FAIL_DIM_INTERVAL		10000	/* 10 sec -> after 10 sec the LED must be dimmed */
#define LED_SWITCH_DIM_COUNT		20000	/* 20 sec -> After 20 sec LED must be dimmed */



typedef enum __APK_NET_EVT__
{
	APK_NET_EVT_NONE,
	APK_NET_EVT_LED_RED,
	APK_NET_EVT_LED_GREEN,
	APK_NET_EVT_LED_DIM,
	APK_NET_EVT_MAX,
} APK_NET_EVT;

typedef enum __LED_NETWORK_TYPE__
{
	LED_NETWORK_TYPE_NONE,
	LED_NETWORK_TYPE_WIFI_WPS,
	LED_NETWORK_TYPE_WIFI,
	LED_NETWORK_TYPE_ETH,
	LED_NETWORK_TYPE_MAX,
} LED_NETWORK_TYPE;

enum {
	APK_MSGQ_SIZE	= 32,
	APK_MSGQ_TIMEOUT	= 500,
};

typedef struct
{
	HUINT32			ulMsg;
	HUINT32			ulTask;
	HUINT32			ulSema;
} APK_INSTANCE_t;

typedef struct
{
	APK_NET_EVT 	eLedEvt;
} APK_MSG_t;

static APK_INSTANCE_t s_stAPKInstance;

#endif

#define FuncEnter HxLOG_Debug("   (+)[%s:%d] Enter!!\n",__FUNCTION__,__LINE__)
#define FuncLeave HxLOG_Debug("   (-)[%s:%d] Leave -- \n",__FUNCTION__,__LINE__)

/***//***********************************************************
	private functions
 **************************************************************/
#if	0
static void __apk_network_log_netconf(NAPINetConfInfo_t *info)
{
	HxLOG_Debug("   (#)[%s:%d]-----------------------------------------\n",__FUNCTION__,__LINE__);
	HxLOG_Debug("   (#)[%s:%d] netconf info \n",__FUNCTION__,__LINE__);
	HxLOG_Debug("   (#)[%s:%d] id : %d \n",__FUNCTION__,__LINE__, info->id);
	HxLOG_Debug("   (#)[%s:%d] name : %s \n",__FUNCTION__,__LINE__, info->name);
	HxLOG_Debug("   (#)[%s:%d] type : %d \n",__FUNCTION__,__LINE__, info->type);
	HxLOG_Debug("   (#)[%s:%d] linked : %d \n",__FUNCTION__,__LINE__, info->linked);
	HxLOG_Debug("   (#)[%s:%d] active : %d \n",__FUNCTION__,__LINE__, info->active);
	HxLOG_Debug("   (#)[%s:%d] dhcp : %d \n",__FUNCTION__,__LINE__, info->dhcp);
	HxLOG_Debug("   (#)[%s:%d] dns_auto : %d \n",__FUNCTION__,__LINE__, info->dns_auto);
	HxLOG_Debug("   (#)[%s:%d] auto_ip : %d \n",__FUNCTION__,__LINE__, info->auto_ip);
	HxLOG_Debug("   (#)[%s:%d] ip_ver : %d \n",__FUNCTION__,__LINE__, info->ip_ver);
	if( info->ip_ver == eNAPI_NETCONF_IPV4 )
	{
		HxLOG_Debug("   (#)[%s:%d] ip_ver : eNCAPI_NETCONF_IPV4 \n", __FUNCTION__,__LINE__);
		HxLOG_Debug("   (#)[%s:%d] macaddr : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv4.macaddr);
		HxLOG_Debug("   (#)[%s:%d] ipaddr : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv4.ipAddr);
		HxLOG_Debug("   (#)[%s:%d] netmask : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv4.netmask);
		HxLOG_Debug("   (#)[%s:%d] gateway : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv4.gateway);
		HxLOG_Debug("   (#)[%s:%d] dns1 : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv4.dns1);
		HxLOG_Debug("	(#)[%s:%d] dns2 : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv4.dns2);
	}
	else if( info->ip_ver == eNAPI_NETCONF_IPV6 )
	{
		HxLOG_Debug("   (#)[%s:%d] ip_ver : eNCAPI_NETCONF_IPV6 \n", __FUNCTION__,__LINE__);
		HxLOG_Debug("   (#)[%s:%d] macaddr : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv6.macaddr);
		HxLOG_Debug("   (#)[%s:%d] ipaddr : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv6.ipAddr);
		HxLOG_Debug("   (#)[%s:%d] gateway : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv6.gateway);
		HxLOG_Debug("   (#)[%s:%d] dns1 : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv6.dns1);
		HxLOG_Debug("	(#)[%s:%d] dns2 : %s \n",__FUNCTION__,__LINE__, info->ip_info.ipv6.dns2);
	}
	HxLOG_Debug("   (#)[%s:%d]-----------------------------------------\n",__FUNCTION__,__LINE__);
}
#endif

#if defined(CONFIG_OP_TDC_NETWORK_LED)
static void __apk_network_initLedSwitch(void)
{
	if(	s_stAPKInstance.ulMsg == 0 )
	{
		return;
	}

	s_nCurLEDColor  = eDxRGBLED_COLOR_GREEN;
	s_nDimCnt = 0;

	if(s_ulLedTimer)
	{
		VK_TIMER_Cancel(s_ulLedTimer);
		s_ulLedTimer = 0;
	}
}

static void __apk_network_ledSwitchTask(void *params)
{
	UNUSED(params);

	if(	s_stAPKInstance.ulMsg == 0 )
	{
		return;
	}

	while(1)
	{
		APK_MSG_t msg;
		HxSTD_MemSet(&msg, 0, sizeof(msg));

		if (VK_MSG_ReceiveTimeout(s_stAPKInstance.ulMsg, &msg, sizeof(APK_MSG_t), APK_MSGQ_TIMEOUT ) == VK_OK )
		{
			switch(msg.eLedEvt)
			{
				case APK_NET_EVT_LED_GREEN:
					APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_GREEN, FALSE);
					break;

				case APK_NET_EVT_LED_RED:
					APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_RED, FALSE);
					break;

				case APK_NET_EVT_LED_DIM :
				{
					if(s_nCurLEDColor == eDxRGBLED_COLOR_GREEN)
					{
						APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_GREEN, FALSE);
					}
					else
					{
						APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_RED, FALSE);
					}

					APK_OUTPUT_PANEL_SetRgbLedDimm(eDxLED_ID_RGB_NET, eDxRGBLED_DIMMLEVEL_DIMMED);
				}
					break;

				default :
					HxLOG_Debug("Invalid apk network msg event !!! \n");
					break;
			}
		}
	}
}

static void __apk_network_ledOffTimerCallback(unsigned long TimerId, void *params)
{
	HERROR hErr = ERR_FAIL;

	UNUSED(TimerId);
	UNUSED(params);

	if(	s_stAPKInstance.ulMsg == 0 )
	{
		return;
	}


	if(s_ulLedTimer)
	{
		VK_TIMER_Cancel(s_ulLedTimer);
		s_ulLedTimer = 0;
	}

	hErr = APK_OUTPUT_PANEL_SetRgbLedDimm(eDxLED_ID_RGB_NET, eDxRGBLED_DIMMLEVEL_SOLID);
	if(hErr != ERR_OK)
	{
		HxLOG_Debug("APK_OUTPUT_PANEL_SetRgbLedDimm is Failed !!!\n");
	}

	hErr = APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_NONE, FALSE);
	if(hErr != ERR_OK)
	{
		HxLOG_Debug("APK_OUTPUT_PANEL_SetRgbLed is Failed !!!\n");
	}

	hErr = APK_OUTPUT_PANEL_SetRgbLedOff(eDxLED_ID_RGB_NET);
	if(hErr != ERR_OK)
	{
		HxLOG_Debug("APK_OUTPUT_PANEL_SetRgbLedOff is Failed !!! \n");
	}
}

static void __apk_network_ledSwitchTimerCallback(unsigned long TimerId, void *params)
{
	APK_MSG_t msg;
	int			vkMsg;

	UNUSED(TimerId);
	UNUSED(params);

	if(	s_stAPKInstance.ulMsg == 0 )
	{
		return;
	}

	HxSTD_MemSet(&msg, 0, sizeof(msg));

	if(s_nCurLEDColor == eDxRGBLED_COLOR_GREEN)
	{
		s_nCurLEDColor = eDxRGBLED_COLOR_RED;
		msg.eLedEvt = APK_NET_EVT_LED_RED;
	}
	else
	{
		s_nCurLEDColor = eDxRGBLED_COLOR_GREEN;
		msg.eLedEvt = APK_NET_EVT_LED_GREEN;
	}

	s_nDimCnt += LED_GREEN_RED_INTERVAL;

	if(s_nDimCnt >= LED_SWITCH_DIM_COUNT)
	{
		s_nDimCnt = LED_SWITCH_DIM_COUNT;
		msg.eLedEvt = APK_NET_EVT_LED_DIM;
	}

	vkMsg = VK_MSG_SendTimeout(s_stAPKInstance.ulMsg, &msg, sizeof(APK_MSG_t),0);
	if(vkMsg)
	{
		HxLOG_Error("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,vkMsg);
		return;
	}

}

static void __apk_network_ledAssociatedTimerCallback(unsigned long TimerId, void *params)
{
	HERROR hErr = ERR_FAIL;

	UNUSED(TimerId);
	UNUSED(params);

	if(	s_stAPKInstance.ulMsg == 0 )
	{
		return;
	}

	__apk_network_initLedSwitch();

	hErr = APK_OUTPUT_PANEL_SetRgbLedDimm(eDxLED_ID_RGB_NET, eDxRGBLED_DIMMLEVEL_DIMMED);

	if(hErr != ERR_OK)
	{
		HxLOG_Debug("APK_OUTPUT_PANEL_SetRgbLedDimm is Failed OK\n");
	}
}

static void __apk_network_ledFailTimerCallback(unsigned long TimerId, void *params)
{
	HERROR hErr = ERR_FAIL;

	UNUSED(TimerId);
	UNUSED(params);

	if(	s_stAPKInstance.ulMsg == 0 )
	{
		return;
	}

	__apk_network_initLedSwitch();

	hErr = APK_OUTPUT_PANEL_SetRgbLedDimm(eDxLED_ID_RGB_NET, eDxRGBLED_DIMMLEVEL_DIMMED);

	if(hErr != ERR_OK)
	{
		HxLOG_Debug("APK_OUTPUT_PANEL_SetRgbLedDimm is Failed OK\n");
	}

	VK_TIMER_EventAfter((unsigned long)LED_FAIL_OFF_INTEVAL, __apk_network_ledOffTimerCallback, (void *)NULL, (int)0, (unsigned long *)&s_ulLedTimer);
}

static void __apk_network_led_connected_lan(void)
{
	HERROR hErr = ERR_FAIL;

	if(	s_stAPKInstance.ulMsg == 0 )
	{
		return;
	}

	__apk_network_ledOffTimerCallback(0, NULL);

	hErr = APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_GREEN, FALSE);

	if(hErr != ERR_OK)
	{
		HxLOG_Debug("APK_OUTPUT_PANEL_SetRgbLed is Failed OK\n");
	}

	__apk_network_initLedSwitch();

	VK_TIMER_EventEvery((unsigned long)LED_GREEN_RED_INTERVAL, __apk_network_ledSwitchTimerCallback, (void *)NULL, (int)0, (unsigned long *)&s_ulLedTimer);


}

static void __apk_network_led_associated(void)
{
	HERROR hErr = ERR_FAIL;

	if(	s_stAPKInstance.ulMsg == 0 )
	{
		return;
	}

	__apk_network_ledOffTimerCallback(0, NULL);

	hErr = APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_GREEN, FALSE);

	if(hErr != ERR_OK)
	{
		HxLOG_Debug("APK_OUTPUT_PANEL_SetRgbLed is Failed OK\n");
	}

	__apk_network_initLedSwitch();

	VK_TIMER_EventAfter((unsigned long)LED_ASSOCIATED_DIM_INTERVAL, __apk_network_ledAssociatedTimerCallback, (void *)NULL, (int)0, (unsigned long *)&s_ulLedTimer);
}

static void __apk_network_led_associationFailYellowDim(void)
{
	HERROR hErr = ERR_FAIL;

	if(	s_stAPKInstance.ulMsg == 0 )
	{
		return;
	}

	__apk_network_ledOffTimerCallback(0, NULL);

	hErr = APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_YELLOW, FALSE);

	if(hErr != ERR_OK)
	{
		HxLOG_Debug("APK_OUTPUT_PANEL_SetRgbLed is Failed OK\n");
	}

	__apk_network_initLedSwitch();

	VK_TIMER_EventAfter((unsigned long)LED_WPS_FAIL_DIM_INTERVAL, __apk_network_ledFailTimerCallback, (void *)NULL, (int)0, (unsigned long *)&s_ulLedTimer);
}

static void __apk_network_led_associationFail(HBOOL bBlink)
{
	HERROR hErr = ERR_FAIL;

	if(	s_stAPKInstance.ulMsg == 0 )
	{
		return;
	}

	__apk_network_ledOffTimerCallback(0, NULL);

	hErr = APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_RED, bBlink);

	if(hErr != ERR_OK)
	{
		HxLOG_Debug("APK_OUTPUT_PANEL_SetRgbLed is Failed OK\n");
	}

	__apk_network_initLedSwitch();

	VK_TIMER_EventAfter((unsigned long)LED_FAIL_DIM_INTERVAL, __apk_network_ledFailTimerCallback, (void *)NULL, (int)0, (unsigned long *)&s_ulLedTimer);
}

void APK_NETWORK_InitTask(void)
{
	HERROR hErr = ERR_FAIL;

	if(	s_stAPKInstance.ulMsg == 0 )
	{
		VK_MEM_Memset(&s_stAPKInstance,0x00,sizeof(s_stAPKInstance));

		hErr = VK_MSG_Create(APK_MSGQ_SIZE, sizeof(APK_MSG_t),"ApkMsgQue",(unsigned long*)&s_stAPKInstance.ulMsg,VK_SUSPENDTYPE_FIFO);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("[%s] Creating APK VK_MSG_Create failed!\n",__func__);
			return;
		}

		hErr = VK_TASK_Create(__apk_network_ledSwitchTask, 40, SIZE_16K, "ledSwitchTask", (void*)NULL, (unsigned long*)&s_stAPKInstance.ulTask, 0);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("[%s] Creating APK network VK_TASK_Create failed!\n",__func__);
			return;
		}
		VK_TASK_Start(s_stAPKInstance.ulTask);
	}
}

#endif

static void __apk_network_log_wifi(NAPIWifiInfo_t *info)
{
	HINT32 i = 0;

	HxLOG_Debug("   (#)[%s:%d]-----------------------------------------\n",__FUNCTION__,__LINE__);
	HxLOG_Debug("   (#)[%s:%d] device id : %d \n",__FUNCTION__,__LINE__, info->dev_id);
	HxLOG_Debug("   (#)[%s:%d] ap count : %d \n",__FUNCTION__,__LINE__, info->count);
	HxLOG_Debug("   (#)[%s:%d]-----------------------------------------\n",__FUNCTION__,__LINE__);
	for( i=0; i<info->count; i++ )
	{
		HxLOG_Debug("   (#)[%s:%d] AP INDEX : [%d] \n",__FUNCTION__,__LINE__, i);
		HxLOG_Debug("   (#)[%s:%d] essid : %s \n",__FUNCTION__,__LINE__, info->ap[i].essid);
		HxLOG_Debug("   (#)[%s:%d] apkey : %s \n",__FUNCTION__,__LINE__, info->ap[i].apkey);
		HxLOG_Debug("   (#)[%s:%d] macaddr : %s \n",__FUNCTION__,__LINE__, info->ap[i].macaddr);
		HxLOG_Debug("   (#)[%s:%d] connect_speed : %d \n",__FUNCTION__,__LINE__, info->ap[i].connect_speed);
		HxLOG_Debug("   (#)[%s:%d] wave_strength : %d \n",__FUNCTION__,__LINE__, info->ap[i].wave_strength);
		HxLOG_Debug("   (#)[%s:%d] op_mode : %d \n",__FUNCTION__,__LINE__, info->ap[i].op_mode);
		HxLOG_Debug("   (#)[%s:%d] secure_type : %d \n",__FUNCTION__,__LINE__, info->ap[i].secure_type);
		HxLOG_Debug("   (#)[%s:%d] authen_type : %d \n",__FUNCTION__,__LINE__, info->ap[i].authen_type);
		HxLOG_Debug("   (#)[%s:%d] encrypt_type : %d \n",__FUNCTION__,__LINE__, info->ap[i].encrypt_type);
	}
	HxLOG_Debug("   (#)[%s:%d]-----------------------------------------\n",__FUNCTION__,__LINE__);
}

STATIC void	__apk_network_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 5);

	pvData = apArgV[3];
	if (pvData)
	{
		APK_Free(pvData);
	}
}

static void __apk_network_broadcast_notifier(HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	APKS_NETWORK_CbHandler callback;
	HxList_t *pos = NULL;
	APK_EVENT_EMIT_ARGS	stApkEvtArgs;

	FuncEnter;

	pos = HLIB_LIST_First(_NetWork_object_handler);
	while ( pos )
	{
		callback = HLIB_LIST_Data(pos);
		if ( callback == NULL )
		{
			HxLOG_Debug("   (+)[%s:%d] have no callback : %d \n",__FUNCTION__,__LINE__);
			FuncLeave;
			return;
		}

		HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 5;
		stApkEvtArgs.apArgV[0] = (void *)callback;
		stApkEvtArgs.apArgV[1] = (void *)eEvent;
		stApkEvtArgs.apArgV[2] = (void *)ulParam1;
		stApkEvtArgs.apArgV[3] = (void *)ulParam2;
		stApkEvtArgs.apArgV[4] = (void *)ulParam3;
		APK_EVENT_Emit(&stApkEvtArgs);

		pos = HLIB_LIST_Next(pos);
	}

	FuncLeave;
	return;
}

static void __apk_network_broadcast_notifier_senddata(HINT32 eEvent, HUINT32 ulParam1,
	HUINT32 ulParam2, HINT32 param2_size, HUINT32 ulParam3)
{
	APKS_NETWORK_CbHandler callback;
	HxList_t *pos = NULL;
	APK_EVENT_EMIT_ARGS	stApkEvtArgs;

	FuncEnter;

	pos = HLIB_LIST_First(_NetWork_object_handler);
	while ( pos )
	{
		callback = HLIB_LIST_Data(pos);
		if ( callback == NULL )
		{
			HxLOG_Debug("   (+)[%s:%d] have no callback : %d \n",__FUNCTION__,__LINE__);
			FuncLeave;
			return;
		}

		HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));

		stApkEvtArgs.pfnFreeFunc = __apk_network_freeEventCb;
		stApkEvtArgs.ulArgCount = 5;
		stApkEvtArgs.apArgV[0] = (void *)callback;
		stApkEvtArgs.apArgV[1] = (void *)eEvent;
		stApkEvtArgs.apArgV[2] = (void *)ulParam1;
		stApkEvtArgs.apArgV[3] = (void *)APK_MemDup((const void*)ulParam2, param2_size);
		stApkEvtArgs.apArgV[4] = (void *)ulParam3;
		APK_EVENT_Emit(&stApkEvtArgs);

		pos = HLIB_LIST_Next(pos);
	}

	FuncLeave;
	return;
}

static void __sqc_netconf_callback(HINT32 event, void* data)
{
	HINT32 eEvent = -1;
	HUINT32 param1 = 0, param2 = 0, param3 = 0;
#if defined(CONFIG_OP_TDC_NETWORK_LED)
	HCHAR	szIPAddr[NAPI_MAX_IP4ADDR] = {0, };
#endif

	FuncEnter;

	if(data)
	{
		param1 = ((NAPINetConfInfo_t *)data)->id;
#if defined(CONFIG_OP_TDC_NETWORK_LED)
		HxSTD_MemCopy(szIPAddr, ((NAPINetConfInfo_t *)data)->ip_info.ipv4.ipAddr, NAPI_MAX_IP4ADDR);
#endif
	}

	switch( event )
	{
		case NAPI_EVENT_NETCONF_CONN_SUCCESS:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			// For "DHCP request failed"
			if (!strncmp(szIPAddr, "169.254", 7))
			{
				__apk_network_led_associationFail(TRUE);
			}
			else
			{
				APK_NETWORK_CONF_CheckInternetConnection( CHECK_INTERNET_MAX_TIME );
			}
#else
			APK_OUTPUT_PANEL_SetLed(eDxLED_ID_LINK, eDxLEDSTATUS_ON);
#endif
			eEvent = eNETWORK_STATUS_CON_SUCCESS; break;

		}
		case NAPI_EVENT_NETCONF_CONN_FAIL:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			__apk_network_led_associationFail(TRUE);
#endif
			eEvent = eNETWORK_STATUS_CON_FAIL;
			break;
		}

		case NAPI_EVENT_NETCONF_AUTOIP_SUCCESS:
			eEvent = eNETWORK_STATUS_CON_AUTOIP_SUCCESS; break;
		case NAPI_EVENT_NETCONF_AUTOIP_FAIL:
			eEvent = eNETWORK_STATUS_CON_AUTOIP_FAIL; break;
		case NAPI_EVENT_NETCONF_LINK_CONNECTED:
			eEvent = eNETWORK_STATUS_CABLE_LINK_CONNECTED; break;
		case NAPI_EVENT_NETCONF_LINK_DISCONNECTED:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			// https://yousee.jira.com/browse/NHYS-828
			// LED - When the ethernet cable is disconnected, net led does not turn red
			__apk_network_led_associationFail(FALSE);
#else
			APK_OUTPUT_PANEL_SetLed(eDxLED_ID_LINK, eDxLEDSTATUS_OFF);
#endif
			eEvent = eNETWORK_STATUS_CABLE_LINK_DISCONNECTED; break;
		}
		case NAPI_EVENT_NETCONF_STATUS_UPDATED:
			eEvent = eNETWORK_STATUS_LINK_UPDATED; break;
		case NAPI_EVENT_NETCONF_DHCPSERVER_START:
			eEvent = eNETWORK_STATUS_DHCPD_START; break;
		case NAPI_EVENT_NETCONF_DHCPSERVER_STOP:
			eEvent = eNETWORK_STATUS_DHCPD_STOP; break;
		case NAPI_EVENT_NETCONF_INTERNET_CONNECTED:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			__apk_network_led_associated();
#endif
			eEvent = eNETWORK_STATUS_INTERNET_CONNECTED;
		}
			break;

		case NAPI_EVENT_NETCONF_INTERNET_DISCONNECTED:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
				__apk_network_led_connected_lan();
#endif
			eEvent = eNETWORK_STATUS_INTERNET_DISCONNECTED;
			break;
		}
	}

	__apk_network_broadcast_notifier(eEvent, param1, param2, param3);

	FuncLeave;
	return;
}

static void __sqc_wifi_callback(HINT32 event, void *data)
{
	HINT32 i = 0;
	HINT32 eEvent = -1;
	HUINT32 param1 = 0, param2 = 0, param3 = 0;
	APKS_NETAP_Info_t *ap_list = NULL;
	NAPIWifiInfo_t *napi_ap_list = NULL;
	HINT32 data_size = 0;
	HBOOL	wasConnected = FALSE;

	FuncEnter;

	if( data )
	{
		napi_ap_list = data;
		ap_list = HLIB_STD_MemAlloc(sizeof(APKS_NETAP_Info_t)*(napi_ap_list->count));
		for( i=0; i<napi_ap_list->count; i++ )
		{
			HxSTD_MemCopy(ap_list[i].szESSID, napi_ap_list->ap[i].essid, APKD_NETWORK_AP_ESSID);
			HxSTD_MemCopy(ap_list[i].szMacAddr, napi_ap_list->ap[i].macaddr, APKD_NETWORK_MAX_MACADDR);
			ap_list[i].nOpMode = napi_ap_list->ap[i].op_mode;
			ap_list[i].nSecureType = napi_ap_list->ap[i].secure_type;
			ap_list[i].nAuthenType = napi_ap_list->ap[i].authen_type;
			ap_list[i].nEncryptType = napi_ap_list->ap[i].encrypt_type;
			ap_list[i].nWaveStrength = napi_ap_list->ap[i].wave_strength;
			ap_list[i].nConnectSpeed = napi_ap_list->ap[i].connect_speed;
			ap_list[i].bIsConnected = napi_ap_list->ap[i].bIsConnected;

			// After boot, scan is performed. At that time are any associated with it?
			if(ap_list[i].bIsConnected == TRUE)
			{
				wasConnected = TRUE;
			}
		}

		param1 = napi_ap_list->dev_id;
		param3 = napi_ap_list->count;
		data_size = sizeof(APKS_NETAP_Info_t)*(napi_ap_list->count);
		param2 = (HUINT32)APK_MemDup(ap_list, data_size);

		__apk_network_log_wifi(napi_ap_list);
	}

	switch( event )
	{
		case NAPI_EVENT_WIFI_INSERTED:
		{
			eEvent = eNETWORK_STATUS_WIFI_USB_INSERTED;
			break;
		}
		case NAPI_EVENT_WIFI_EXTRACTED:
			eEvent = eNETWORK_STATUS_WIFI_USB_EXTRACTED; break;

		case NAPI_EVENT_WIFI_SCANNING_START:
		{
			eEvent = eNETWORK_STATUS_WIFI_SCANNING_START;
			break;
		}

		case NAPI_EVENT_WIFI_SCANNING_END:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			// After scanning, if network already is connected, Net LED should not blink.
			if(wasConnected == TRUE)
			{
				APK_NETWORK_LED_Init();
				APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_YELLOW, TRUE);

				APK_NETWORK_CONF_CheckInternetConnection( CHECK_INTERNET_MAX_TIME );
			}
#endif
			eEvent = eNETWORK_STATUS_WIFI_SCANNING_END;
			break;
		}

		case NAPI_EVENT_WIFI_SCANNING_OUTPUT:
			eEvent = eNETWORK_STATUS_WIFI_SCANNING_OUTPUT; break;
		case NAPI_EVENT_WIFI_SCANNING_FAIL:
			eEvent = eNETWORK_STATUS_WIFI_SCANNING_FAIL; break;
		case NAPI_EVENT_WIFI_KEY_INVALID:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			__apk_network_led_associationFailYellowDim();
#endif
			eEvent = eNETWORK_STATUS_WIFI_KEY_INVALID;
			break;
		}

		case NAPI_EVENT_WIFI_AP_CONNECT_SUCCESS:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			__apk_network_led_associated();
#else
			APK_OUTPUT_PANEL_SetLed(eDxLED_ID_WIFI, eDxLEDSTATUS_ON);
#endif
			eEvent = eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS; break;
		}
		case NAPI_EVENT_WIFI_AP_CONNECT_FAIL:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			__apk_network_led_associationFailYellowDim();
#else
			APK_OUTPUT_PANEL_SetLed(eDxLED_ID_WIFI, eDxLEDSTATUS_OFF);
#endif
			eEvent = eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL; break;
		}

		case NAPI_EVENT_WIFI_WPS_SUCCESS:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			__apk_network_led_associated();
#endif
			eEvent = eNETWORK_STATUS_WIFI_WPS_SUCCESS;
			break;
		}

		case NAPI_EVENT_WIFI_WPS_FAIL:
		{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			__apk_network_led_associationFailYellowDim();
#endif
			eEvent = eNETWORK_STATUS_WIFI_WPS_FAIL;
			break;
		}

		case NAPI_EVENT_WIFI_AP_DISCONNECTED:
			eEvent = eNETWORK_STATUS_WIFI_AP_DISCONNECTED; break;
		case NAPI_EVENT_WIFI_AP_RECONNECTED:
			eEvent = eNETWORK_STATUS_WIFI_AP_RECONNECTED; break;
		case NAPI_EVENT_WIFI_AP_CONNECT_STOP:
			eEvent = eNETWORK_STATUS_WIFI_AP_CONNECT_STOP; break;
		case NAPI_EVENT_WIFI_LINK_UPDATED:
			eEvent = eNETWORK_STATUS_LINK_UPDATED; break;
	}

	HxLOG_Debug("	(+)[%s:%d] eEvent : %d, param1 : %ld, param2 : %ld : param3 : %ld \n",
		__FUNCTION__,__LINE__, eEvent, param1, param2, param3);

	if( NULL == ap_list )
	{
		__apk_network_broadcast_notifier(eEvent, param1, param2, param3);
	}
	else
	{
		if( NULL != napi_ap_list )
		{
			__apk_network_broadcast_notifier_senddata(eEvent, param1, param2, data_size, param3);
		}
	}

	if( param2 ) APK_Free((APKS_NETAP_Info_t *)param2);
	if( ap_list ) APK_Free(ap_list);

	FuncLeave;
	return;
}

static void __sqc_bt_callback(HINT32 event, void* data)
{
	HINT32 eEvent = -1;//, i = 0;
	HINT32 i = 0;
	HUINT32 param1 = 0, param2 = 0, param3 = 0;
	NAPIBTInfo_t *bt_data = NULL;
	//APKS_BT_DEVICEINFO_t *apk_bt = NULL;
	FuncEnter;

	switch( event )
	{
		case NAPI_EVENT_LIST_UPDATED:
			{
				if( data )
				{
					bt_data = (NAPIBTInfo_t *)data;
					if( bt_data )
						param1 = (HUINT32)bt_data->enable;
				}
				eEvent = eNETWORK_STATUS_BT_LIST_UPDATED; break;
			}
		case NAPI_EVENT_CONNECTED:
			{
				if( data )
				{
					bt_data = (NAPIBTInfo_t *)data;
					for(i = 0; i < bt_data->dev_count; i++)
					{
						if( bt_data->dev_info[i].status == eNAPI_BT_STATUS_Connected )
						{
							if( bt_data->dev_info[i].support_profile & eNAPI_BT_Profile_A2DP )
							{
								APK_OUTPUT_AUDIO_GetMasterMute(&s_audio_mute);
								if( !s_audio_mute )
								{
									s_audio_mute = TRUE;
									APK_OUTPUT_AUDIO_SetMasterMute(s_audio_mute);
									param1 = (HUINT32)s_audio_mute;
									eEvent = eNETWORK_STATUS_BT_AUDIO_MUTE;
									__apk_network_broadcast_notifier(eEvent, param1, param2, param3);
									break;
								}
							}
						}
					}
				}
				param1 = 0;
				eEvent = eNETWORK_STATUS_BT_CONNECTED; break;
			}
		case NAPI_EVENT_CONNECT_FAILED:
			{
				if( data )
				{
					bt_data = (NAPIBTInfo_t *)data;
					for(i = 0; i < bt_data->dev_count; i++)
					{
						if( bt_data->dev_info[i].status == eNAPI_BT_STATUS_Disconnected)
						{
							if( bt_data->dev_info[i].support_profile & eNAPI_BT_Profile_A2DP )
							{
								APK_OUTPUT_AUDIO_GetMasterMute(&s_audio_mute);
								if( s_audio_mute )
								{
									s_audio_mute = FALSE;
									APK_OUTPUT_AUDIO_SetMasterMute(s_audio_mute);
									param1 = (HUINT32)s_audio_mute;
									eEvent = eNETWORK_STATUS_BT_AUDIO_MUTE;
									__apk_network_broadcast_notifier(eEvent, param1, param2, param3);
									break;
								}
							}
						}
					}
				}
				param1 = 0;
				eEvent = eNETWORK_STATUS_BT_CONNECT_FAILED; break;
			}
		case NAPI_EVENT_PIN_REQUESTED:
			eEvent = eNETWORK_STATUS_BT_PIN_REQUESTED; break;
		case NAPI_EVENT_PASSKEY:
			bt_data = (NAPIBTInfo_t *)data;
			if(bt_data)
			{
				param1 = (HUINT32)bt_data->passkey;
				//printf("#### [KSKIM2][appkit] bluetooth passkey : %d\n", param1);
			}
			eEvent = eNETWORK_STATUS_BT_PASSKEY; break;
		case NAPI_EVENT_DISCONNECTED:
			eEvent = eNETWORK_STATUS_BT_DISCONNECTED; break;
		case NAPI_EVENT_PROFILE_STARTED:
			eEvent = eNETWORK_STATUS_BT_PROFILE_STARTED; break;
		case NAPI_EVENT_PROFILE_STOPPED:
			eEvent = eNETWORK_STATUS_BT_PROFILE_STOPPED; break;
		case NAPI_EVENT_AUDIO_MUTE:
			if(bt_data)
			{
				param1 = (HUINT32)bt_data->audio_mute;
			}
			eEvent = eNETWORK_STATUS_BT_AUDIO_MUTE; break;
		default: return;
	}

	HxLOG_Debug("   (#)[%s:%d] eEvent : %d, param1 : %d, param2 : %d, param3 : %d \n",__FUNCTION__,__LINE__, eEvent, param1, param2, param3);
	__apk_network_broadcast_notifier(eEvent, param1, param2, param3);

	FuncLeave;
	return;
}

static void __sqc_pppoe_callback(HINT32 event, void* data)
{
	HINT32 eEvent = -1;
	HUINT32 param1 = 0, param2 = 0, param3 = 0;

	FuncEnter;

	switch( event )
	{
		case NAPI_EVENT_PPPOE_CONNECTED:
			eEvent = eNETWORK_STATUS_PPPOE_CONNECTED; break;
		case NAPI_EVENT_PPPOE_DISCONNECTED:
			eEvent = eNETWORK_STATUS_PPPOE_DISCONNECTED; break;
		default :
			break;
	}

	__apk_network_broadcast_notifier(eEvent, param1, param2, param3);

	FuncLeave;
	return;
}


/**************************************************************
	public-internal functions
 **************************************************************/
HBOOL	APK_NETWORK_Init(void)
{
	FuncEnter;
	APK_OUTPUT_PANEL_Init();

	NAPI_Init();
	NAPI_NetConf_RegisterCallback(__sqc_netconf_callback);
	NAPI_Wifi_RegisterCallback(__sqc_wifi_callback);
	NAPI_BT_RegisterCallback(__sqc_bt_callback);
	NAPI_PPPOE_RegisterCallback(__sqc_pppoe_callback);


	FuncLeave;
	return TRUE;
}

HBOOL	APK_NETWORK_DeInit(void)
{
	FuncEnter;

	NAPI_DeInit();

	FuncLeave;
	return TRUE;
}

/**************************************************************
	public functions
 **************************************************************/
void	APK_NETWORK_RegisterNotifier(APKS_NETWORK_CbHandler  pfnObjectHandler)
{
	HxList_t	*list;

	FuncEnter;

	list = HLIB_LIST_Find(_NetWork_object_handler, pfnObjectHandler);
	if (list)
	{
		return;
	}

	_NetWork_object_handler = HLIB_LIST_Append(_NetWork_object_handler, (void*)pfnObjectHandler);

	FuncLeave;
	return;
}

void	APK_NETWORK_UnRegisterNotifier(APKS_NETWORK_CbHandler  pfnObjectHandler)
{
	HxList_t	*list;

	FuncEnter;

	list = HLIB_LIST_Find(_NetWork_object_handler, pfnObjectHandler);
	if (list == NULL)
	{
		return;
	}

	_NetWork_object_handler = HLIB_LIST_Remove(_NetWork_object_handler, pfnObjectHandler);

	FuncLeave;
	return;
}

void	APK_NETWORK_LED_Init(void)
{
#if defined(CONFIG_OP_TDC_NETWORK_LED)
	__apk_network_ledOffTimerCallback(0, NULL);
#endif
	return;
}


#else



HBOOL	APK_NETWORK_Init(void)
{
	return FALSE;
}

HBOOL	APK_NETWORK_DeInit(void)
{
	return FALSE;
}

void	APK_NETWORK_RegisterNotifier(APKS_NETWORK_CbHandler  pfnObjectHandler)
{
	return;
}

void	APK_NETWORK_UnRegisterNotifier(APKS_NETWORK_CbHandler  pfnObjectHandler)
{
	return;
}

void	APK_NETWORK_LED_Init(void)
{
	return;
}


#endif
