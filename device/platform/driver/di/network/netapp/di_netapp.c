/*******************************************************************/
/****************************** include  *****************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>
#include <vkernel.h>

#include "typedefs.h"
#include "taskdef.h"

#include "drv_err.h"
#include "di_uart.h"

#include "netapp.h"

#include "di_netapp.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define UNUSED(x) (void)(x)

#define DI_DEBUG
#if defined(CONFIG_SHARED_COMPILE)
#ifdef DI_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()			VK_DBG_Print ("(+)%s\n", __FUNCTION__)
#define PrintExit()				VK_DBG_Print ("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()
#define PrintExit()
#endif
#else
#ifdef DI_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()				DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()				DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()
#define PrintExit()
#endif
#endif

typedef struct
{
	HUINT32			ulSema;
	HUINT32			ulTask;
} NETAPP_INSTANCE_t;

static NETAPP_INSTANCE_t s_stNetappInstance;

static NETAPP_HANDLE hNetApp;

void netapp_callback(void* pParam, NETAPP_CB_TYPE tCbType, const void *pvBuffer, HUINT32 ulData0, NETAPP_RETCODE tResult, NETAPP_IFACE tIFace)
{
    NETAPP_HOTPLUG_DEVICE_INFO *pHpdInfo;

    switch(tCbType)
    {
        case NETAPP_CB_DYING:
            PrintDebug("[%s][%d] NetApp has detected a fatal error and is unstable - need hardware reset.\n", __FUNCTION__, __LINE__);
            PrintDebug("[%s][%d] Invoking dw_callback for device error...\n", __FUNCTION__, __LINE__);
	        break;

        case NETAPP_CB_HOTPLUG:
            pHpdInfo = (NETAPP_HOTPLUG_DEVICE_INFO *)pvBuffer;

            if ((pHpdInfo) && (NETAPP_HOTPLUG_DEVICE_WIFI == pHpdInfo->tType) && (NETAPP_HOTPLUG_REMOVE == pHpdInfo->tAction))
            {
                PrintDebug("[%s][%d] Abnormal removal of wireless hardware!!! \n", __FUNCTION__, __LINE__);
                PrintDebug("[%s][%d] Invoking dw_callback for device error... \n", __FUNCTION__, __LINE__);
            }
	        break;

        case NETAPP_CB_CONNECT:
			PrintDebug("[%s][%d] Received NETAPP_CB_CONNECT - %s \n", __FUNCTION__, __LINE__, (tIFace == NETAPP_IFACE_WIRELESS) ? "Wireless" : "Bluetooth");
	        break;

        default:
            PrintDebug("[%s][%d] Unhandled NetApp callback type (%d) \n", __FUNCTION__, __LINE__, tCbType);
            break;
    }

	if (tIFace == NETAPP_IFACE_WIRELESS)
	{
		P_WIFI_Netapp_Callback(pParam, tCbType, pvBuffer, ulData0, tResult, tIFace);
	}
#if defined(CONFIG_NETAPP_BLUETOOTH)
//	P_BT_NetApp_Callback(pParam, tCbType, pvBuffer, ulData0, tResult, tIFace);
#endif

}

static NETAPP_RETCODE P_NETAPP_Open(NETAPP_HANDLE *hHandle)
{
	
	NETAPP_RETCODE nret = NETAPP_FAILURE;
    NETAPP_SETTINGS netAppSettings;
    NETAPP_INIT_SETTINGS netAppInitSettings;
    NETAPP_OPEN_SETTINGS netAppOpenSettings;

	if(hNetApp != NULL)
	{
		PrintError("[%s][%d] Already initialized \n", __FUNCTION__, __LINE__);
		return nret;
	}

	NetAppGetDefaultSettings(&netAppSettings);
    NetAppGetDefaultInitSettings(&netAppInitSettings);

	netAppInitSettings.bBurstScanResults = false;
#if defined(CONFIG_SUPPORT_2P4G_40M_BW)
	netAppInitSettings.tMimoBwCap = NETAPP_MIMO_BW_CAP_40MHZ;
#else
	netAppInitSettings.tMimoBwCap = NETAPP_MIMO_BW_CAP_20MHZ_40MHz;
#endif
	netAppInitSettings.WiFiIfacePrefix = "wlan";
	netAppInitSettings.tDriverMode = NETAPP_WIFI_DRIVER_MODE_PRODUCTION;

	/* we REQUIRE a valid country code; either from a limited list at compile time, or from a slightly expanded list at run-time */
#if 0
#if defined(DWLAN_COUNTRY_CODE_US)
	netAppInitSettings.pCountryCode = "US/997";
#elif defined(DWLAN_COUNTRY_CODE_MX)
	netAppInitSettings.pCountryCode = "MX/33";
#elif defined(DWLAN_COUNTRY_CODE_BR)
	netAppInitSettings.pCountryCode = "BR/4";
#else
	/* the legacy approach of setting the country at build time was NOT employed */
	switch(pMasterParams->config.country_code)
	{
		case DWLAN_COUNTRY_UNITEDSTATES:
			netAppInitSettings.pCountryCode = "US/997";
			break;
		case DWLAN_COUNTRY_MEXICO:
			netAppInitSettings.pCountryCode = "MX/33";
			break;
		case DWLAN_COUNTRY_BRAZIL:
			netAppInitSettings.pCountryCode = "BR/4";
			break;
		case DWLAN_COUNTRY_ARGENTINA:
			netAppInitSettings.pCountryCode = "AR/21";
			break;
		case DWLAN_COUNTRY_ECUADOR:
			netAppInitSettings.pCountryCode = "EC/18";
			break;
		case DWLAN_COUNTRY_COLOMBIA:
			netAppInitSettings.pCountryCode = "CO/17";
			break;
		case DWLAN_COUNTRY_URUGUAY:
			netAppInitSettings.pCountryCode = "UY/6";
			break;
		case DWLAN_COUNTRY_PERU:
			netAppInitSettings.pCountryCode = "PE/20";
			break;
		case DWLAN_COUNTRY_CHILE:
			netAppInitSettings.pCountryCode = "CL/7";
			break;
		case DWLAN_COUNTRY_VENEZUELA:
			netAppInitSettings.pCountryCode = "VE/0";
			break;
		case DWLAN_COUNTRY_TRINIDAD:
			netAppInitSettings.pCountryCode = "TT/3";
			break;
		case DWLAN_COUNTRY_CURACAO:
			netAppInitSettings.pCountryCode = "NL/4";
			break;
		case DWLAN_COUNTRY_BARBADOS:
			netAppInitSettings.pCountryCode = "BB/0";
			break;
		case DWLAN_COUNTRY_PUERTO_RICO:
			netAppInitSettings.pCountryCode = "PR/20";
			break;

		default:
			DBG_ERR_EXIT(DWLAN_RC_INVALID_PARAMETER, "unsupported country code requested")
			break;
	}
#endif
#endif

	netAppSettings.bAutoReConnect = false;
	netAppSettings.bForceWiFi = true;

#if defined(CONFIG_NETAPP_WIFI_WPS)	&& defined (CONFIG_SUPPORT_WPS)
	netAppSettings.bWPS2_0 = true;
#endif
#if defined(CONFIG_NETAPP_BLUETOOTH) && defined(CONFIG_BLUETOOTH) /* ToDo. */
	netAppSettings.tBtSettings.bAutoPair=true;
#endif
#if defined(CONFIG_NETAPP_SOFTAP)
	netAppSettings.bIsSoftAp = true;
#endif
#if defined(CONFIG_NETAPP_P2P)
	netAppSettings.bIsP2PGO= true;
	netAppSettings.bP2PAutoGO = true;
	netAppSettings.bAutoP2PDiscover = false;	
#endif
	netAppSettings.bBridgingMode = false;
	netAppSettings.bHideDuplicateAPs = false;

	netAppOpenSettings.tCallback = netapp_callback;
	//netAppOpenSettings.pParam = hNetApp;

	nret = NetAppOpen(&hNetApp, &netAppOpenSettings, &netAppInitSettings, &netAppSettings);
	if (nret != NETAPP_SUCCESS)
	{
		PrintError("[%s][%d] unable to open NetApp \n", __FUNCTION__, __LINE__);
	}

	*hHandle = hNetApp;

	return nret;
}

/* Currently, we do not consider in the case of closing NetApp */
#if 0
static NETAPP_RETCODE P_NetApp_Close()
{
    NETAPP_RETCODE nret;

    if (hNetApp)
    {
        nret = NetAppClose(hNetApp);
        if (NETAPP_SUCCESS != nret)
        {
            PrintError("[%s][%d] unable to close NetApp \n", __FUNCTION__, __LINE__);
        }
        hNetApp = NULL;
    }

	return nret;
}
#endif

static void P_NETAPP_Task(void *pvParam)
{
	NETAPP_HANDLE hNetAppTmp = NULL;
	HINT32	nVkErr;

	UNUSED(pvParam);

	VK_SEM_Get(s_stNetappInstance.ulSema);
	
	P_NETAPP_Open(&hNetAppTmp);

	VK_SEM_Release(s_stNetappInstance.ulSema);
}

DRV_Error DRV_NETAPP_GetHandle(NETAPP_HANDLE *pstNetappHandle)
{
	HINT32	nVkErr;

	nVkErr = VK_SEM_Get(s_stNetappInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] VK_SEM_Get failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
	
	*pstNetappHandle = hNetApp;

	VK_SEM_Release(s_stNetappInstance.ulSema);
	
	return DRV_OK;
}

DRV_Error DRV_NETAPP_Init(void)
{
	HINT32	nVkErr;

	VK_MEM_Memset(&s_stNetappInstance,0x00,sizeof(s_stNetappInstance));

	nVkErr = VK_SEM_Create((unsigned long*)&s_stNetappInstance.ulSema, "NETAPP", VK_SUSPENDTYPE_FIFO);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] Creating NETAPP VK_SEM_Create failed!\n",__func__);
		return DRV_ERR;
	}

	nVkErr = VK_TASK_Create(P_NETAPP_Task, NETAPP_TASK_PRIORITY, NETAPP_TASK_STACK_SIZE, "P_NETAPP_Task", NULL, (unsigned long*)&s_stNetappInstance.ulTask, FALSE);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Creating NETAPP VK_TASK_Create failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	if ((s_stNetappInstance.ulTask == 0) || (nVkErr != VK_OK))
	{
		PrintError("[%s][%d] nRet(0x%x) \n", __FUNCTION__, __LINE__, nVkErr);
		return DRV_ERR;
	}
	else
	{
		VK_TASK_Start(s_stNetappInstance.ulTask);
	}

	return DRV_OK;
}




