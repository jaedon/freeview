/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include common utils first! */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <pthread.h>
#include <sys/socket.h>
#include <vkernel.h>
#include <dirent.h>
#include <signal.h>

#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/sockios.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <errno.h>

/* QV940 Upgrade */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "htype.h"
#include "drv_err.h"
#include "taskdef.h"
#include "di_uart.h"
#include "di_system.h"
//#include "drv_network.h"
//#include "drv_wireless_hotplug.h"
#include "crc.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
//#define DRV_WIRELESS_DEBUG
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
#ifdef DRV_WIRELESS_DEBUG
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

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#define QCSAPI_WIFI_BINARY_FILE "/var/tmp/wifi_binary.img"
#define QCSAPI_UPGRADE_RESULT_FILE "/var/tmp/upgrade_result"

#define IPADDR_QV940	"169.254.119.1"
#define IPADDR_QCS_HOST	"169.254.119.5"
#define QCSAPI_TMP_FILE "/var/tmp/qv_tmp"
#define QCSAPI_PING_LOG_FILE "/var/tmp/qv_ping_log"
#define QCSAPI_WIFI_MODE_FILE "/var/tmp/qv_wifi_mode"

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

extern DRV_Error DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT eSysEvent, HUINT32 ulProgress);

typedef struct
{
	pfnWIFI_UPGRADE_CALLBACK pfnUpgradeCallback;
} WIFI_UPGRADE_CALLBACK_INFO_t;

DI_ERR_CODE DRV_WIFI_Update(char *pImage, unsigned int ulImgSize)
{
	FILE *fp;
	int bin_fp = -1;
	HCHAR qcsapiStatus[129], cmdBuf[255], buf[128], qcsapiMode[129], *pTemp = 0;
#if 0
	DI_WIFI_UPGRADE_CALLBACK_DATA_t wifiUpgradeCallbackData;
#endif

	PrintEnter();

#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 0);
#else
	//wifiUpgradeCallbackData.ulDevId = 0;
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_WRITE_START;
	wifiUpgradeCallbackData.ulProgress = 0;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif

	if ( (bin_fp = open(QCSAPI_WIFI_BINARY_FILE, 770 /* (0x100 | 0x200) | 2 */, 438) ) == -1) {
		PrintError("[%s] Err open %s\n", __FUNCTION__, QCSAPI_WIFI_BINARY_FILE);
#if 1
		DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 5);
#else
		wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_WRITE_START;
		wifiUpgradeCallbackData.ulProgress = 5;
		wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_WRITE_FAIL;
		PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
		(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif
		return DI_ERR_ERROR;
	}
	if ( write(bin_fp, pImage, ulImgSize) == -1) {
		PrintError("[%s] Err write %s\n", __FUNCTION__, QCSAPI_WIFI_BINARY_FILE);
#if 1
		DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 5);
#else
		wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_WRITE_START;
		wifiUpgradeCallbackData.ulProgress = 5;
		wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_WRITE_FAIL;
		PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
			((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
		(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
		close(bin_fp);
#endif
		close(bin_fp);
		return DI_ERR_ERROR;
	}
	close(bin_fp);

#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 5);
#else
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_WRITE_COMPLETE;
	wifiUpgradeCallbackData.ulProgress = 5;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif

#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 10);
#else
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_TRANSFER_START;
	wifiUpgradeCallbackData.ulProgress = 10;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif

retry_transmit_file:
    VK_snprintf(cmdBuf, sizeof(cmdBuf), "ping -c 1 %s > %s", IPADDR_QV940, QCSAPI_PING_LOG_FILE);
	system(cmdBuf);

	VK_TASK_Sleep(2000);

	fp = fopen(QCSAPI_PING_LOG_FILE, "r");
	if(fp == NULL)
	{
		goto retry_transmit_file;
	}

	while(fgets(buf, 128, fp)!= NULL)
	{
		pTemp = NULL;
		pTemp = VK_strstr(buf,"round-trip");

		if (pTemp)
		{
			PrintError("%s(%d) Network connected!!!\n", __FUNCTION__, __LINE__);
			break;
		}
	}
	fclose(fp);

	if (!pTemp)
	{
		goto retry_transmit_file;
	}
	else
	{
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_mode wifi0 > %s",
			IPADDR_QV940, QCSAPI_WIFI_MODE_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WIFI_MODE_FILE, "r");
		if (fp != NULL)
		{
			fgets(buf, 128, fp);
			VK_sscanf(buf, "%s", &qcsapiMode[0]);
			fclose(fp);

			if (VK_strcmp("Station", qcsapiMode) != 0)
			{
				PrintError("[%s] Need to check the RPC: Program not registered!\n", __FUNCTION__);
				goto retry_transmit_file;
			}
		}
	}

	// call_qcsapi_sockrpc send_file (TFTP server)
	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s run_script transmit_file wifi_binary.img %s > %s",
			IPADDR_QV940, IPADDR_QCS_HOST, QCSAPI_TMP_FILE);
	VK_SYSTEM_Command(cmdBuf);

#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 20);
#else
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_TRANSFER_START;
	wifiUpgradeCallbackData.ulProgress = 20;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif

	fp = fopen(QCSAPI_TMP_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%s", &qcsapiStatus[0]);
		fclose(fp);

		if (VK_strcmp("QCS", qcsapiStatus) == 0)
		{
			PrintError("[%s] QCSAPI sockrpc error!\n", __FUNCTION__);
#if 1
			DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 20);
#else
			wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_TRANSFER_START;
			wifiUpgradeCallbackData.ulProgress = 20;
			wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_TRANSFER_FAIL;
			PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
				((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
			(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif
			return DI_ERR_ERROR;
		}
	}
	else
	{
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
#if 1
		DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 20);
#else
		wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_TRANSFER_START;
		wifiUpgradeCallbackData.ulProgress = 20;
		wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_TRANSFER_FAIL;
		PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
			((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
		(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif
		return DI_ERR_ERROR;
	}

	VK_TASK_Sleep(1500);
#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 30);
#else
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_TRANSFER_START;
	wifiUpgradeCallbackData.ulProgress = 30;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif

	VK_TASK_Sleep(1500);
#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 40);
#else
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_TRANSFER_START;
	wifiUpgradeCallbackData.ulProgress = 40;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif

	VK_TASK_Sleep(1500);
#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 50);
#else
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_TRANSFER_START;
	wifiUpgradeCallbackData.ulProgress = 50;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif

	VK_TASK_Sleep(1500);
#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 60);
#else
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_TRANSFER_COMPLETE;
	wifiUpgradeCallbackData.ulProgress = 60;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif

    // call_qcsapi_sockrpc flash_image_update
	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s flash_image_update /tmp/wifi_binary.img live > %s",
			IPADDR_QV940, QCSAPI_UPGRADE_RESULT_FILE);
	VK_SYSTEM_Command(cmdBuf);

#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 80);
#else
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_DOWNLOAD_START;
	wifiUpgradeCallbackData.ulProgress = 80;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif

	VK_TASK_Sleep(1500);
#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 90);
#else
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_DOWNLOAD_START;
	wifiUpgradeCallbackData.ulProgress = 90;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif
	VK_TASK_Sleep(1500);

	fp = fopen(QCSAPI_UPGRADE_RESULT_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%s", &qcsapiStatus[0]);
		fclose(fp);

		if (VK_strcmp("complete", qcsapiStatus) != 0)
		{
			PrintError("[%s] QCSAPI image update error!\n", __FUNCTION__);
#if 1
			DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 95);
#else
			wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_DOWNLOAD_START;
			wifiUpgradeCallbackData.ulProgress = 95;
			wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_DOWNLOAD_FAIL;
			PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
				((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
			(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif
			return DI_ERR_ERROR;
		}
	}
	else
	{
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
#if 1
		DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 95);
#else
		wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_WRITE_START;
		wifiUpgradeCallbackData.ulProgress = 95;
		wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_DOWNLOAD_FAIL;
		PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
			((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
		(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif
		return DI_ERR_ERROR;
	}

#if 1
	DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT_WIFI_UPDATE_PROGRESS, 100);
#else
	wifiUpgradeCallbackData.eEvent = DI_WIFI_NOTIFY_UPGRADE_EVT_DOWNLOAD_COMPLETE;
	wifiUpgradeCallbackData.ulProgress = 100;
	wifiUpgradeCallbackData.eErrorCode = DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE;
	PrintError("[%s] %d%%, %s\n", __FUNCTION__, wifiUpgradeCallbackData.ulProgress,
		((wifiUpgradeCallbackData.eErrorCode == DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE)?"Success!!!":"Error!!!"));
	(*pfnUpgradeCallback)(&wifiUpgradeCallbackData);
#endif

	PrintExit();

	return DI_ERR_OK;
}
