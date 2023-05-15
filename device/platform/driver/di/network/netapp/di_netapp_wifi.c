#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/if_ether.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <errno.h>

#include <vkernel.h>

#include "htype.h"
#include "drv_err.h"
#include "taskdef.h"
#include "di_wifi.h"
#include "di_uart.h"
#include "drv_network.h"
#include "drv_wireless_hotplug.h"

#include "netapp.h"
#include "di_netapp.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DI_DEBUG
#if defined(CONFIG_SHARED_COMPILE)
#ifdef DI_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()			VK_DBG_Print ("(+)%s\n", __FUNCTION__)
#define PrintExit()				VK_DBG_Print ("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()			VK_DBG_Print ("(+)%s\n", __FUNCTION__)
#define PrintExit()				VK_DBG_Print ("(-)%s\n", __FUNCTION__)
#endif
#else
#ifdef DI_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()				DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()				DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()				DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()				DI_UART_Print("(-)%s\n", __FUNCTION__)

#endif
#endif

#define UNUSED(x) (void)(x)

#define WIFI_MAC_SIZE	(6)
#define WIFI_AP_MAX		(128)
#define WIFI_MAX_NUM	(3)

#define WIFI_CONNECT_MONITOR	5000
#define WIFI_CONNECT_CHECK		3000
#define WIFI_SCAN_DONE_CHECK	1000
#define SCAN_TIMEOUT			10000000 //usec

#define	PCI_INFO_SIZE 256

/* BCM WIFI Info. */
#if defined(CONFIG_BCM43567)
#define	BCM_WIFI_CONTROLLER			"BCMaa31"
#define BCM_WIFI_VENDOR_DEVICE_ID	"14e4aa31"
#define BCM_WIFI_DRIVER_NAME		"dhd"
#endif

#define NETAPP_SCAN_REPORTING_FREQUENCY (0) /* indicates one-pass scan */
#define NETAPP_SCAN_CHANNEL_HOVER_PERIOD (200)

#define WIFI_CHANNEL_LUT_SIZE 42

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum
{
	WIFI_EVT_INSERT,
	WIFI_EVT_EXTRACT,
	WIFI_EVT_SCAN_START,
	WIFI_EVT_SCAN_STOP,
	WIFI_EVT_CONNECT_START,
	WIFI_EVT_CONNECT_OK,
	WIFI_EVT_CONNECT_FAIL,
	WIFI_EVT_CONNECT_CHECK,
	WIFI_EVT_DISCONNECT,
	WIFI_EVT_WPS_PBC_START,
	WIFI_EVT_WPS_PIN_START,
	WIFI_EVT_WPS_STOP,
	WIFI_EVT_MAX
} WIFI_EVENT_e;

typedef enum
{
	WIFI_STATUS_OFF=0,
	WIFI_STATUS_IDLE,
	WIFI_STATUS_CONNECTED,
	WIFI_STATUS_CONNECTING,  /* Only next to CONNECT or DISCONNECT */
	WIFI_STATUS_RECONNECTING,
	WIFI_STATUS_DISCONNECTED,
	WIFI_STATUS_MAX
} WIFI_STATUS_e;

typedef enum
{
	WIFI_DONGLE_INSERTED,
	WIFI_DONGLE_EXTRACTED,
	WIFI_DONGLE_MAX
} WIFI_DONGLE_e;

typedef enum
{
	WIFI_CTL_NONE,
	WIFI_CTL_WL,
	WIFI_CTL_WPA_SUPPLICANT,
	WIFI_CTL_MAX
} WIFI_CONTROL_TYPE_e;

typedef struct
{
	pfnWIFI_EVT_CALLBACK pfnCallback;
} WIFI_CALLBACK_INFO_t;

typedef struct
{
	HUINT32 		ulDevId;
	HINT8			acIfName[IFNAMSIZ];
	WIFI_STATUS_e	eUserSetting;
	DI_WIFI_WPS_e	eWPS;
	HINT32 			nWPSPid;
	WIFI_STATUS_e	eStatus;
	HBOOL			bScanning;
	WIFI_DONGLE_e	eDongleStatus;
	HUINT32			ulNumOfAPs;
	HUINT8 			aucMAC[6];		/*HEX*/
	DI_WIFI_INFO_t 	stUserSetAP;
	DI_WIFI_INFO_t 	astAPList[WIFI_AP_MAX];
	WIFI_CALLBACK_INFO_t 	stCallbackInfo[DI_WIFI_NOTIFY_EVT_MAX];
}WIFI_INFO_t;

typedef struct
{
	HUINT32			ulMsg;
	HUINT32			ulTask;
	HUINT32			ulSema;
	HUINT32			ulDisConnectTimerId;
	WIFI_INFO_t		stWiFiInfo[WIFI_MAX_NUM];
} WIFI_INSTANCE_t;

typedef struct
{
	HUINT32			ulIndex;
	WIFI_EVENT_e 	eMsg;
	HUINT32			ulTime;
	HINT8 			acDevName[IW_ESSID_MAX_SIZE+1]; /*IW_ESSID_MAX_SIZE 32*/
	HUINT8			aucPin[8];
} WIFI_MSG_t;


typedef struct
{
	HUINT32	ulChannelNo;
	HUINT32	ulFrequency;
} WIFI_CHANNEL_LUT_t;

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/*********************** extern function prototypes ****************/
/*******************************************************************/
extern DRV_NETWORK_DEV_INFO_t s_stDevList[];
extern HUINT32 DRV_CFG_GetNetMaxDevice(void);

/*******************************************************************/
/*********************** Static function prototypes ****************/
/*******************************************************************/
static NETAPP_HANDLE hWifiNetapp = NULL;

static WIFI_INSTANCE_t s_stWiFiInstance;
static WIFI_CONTROL_TYPE_e s_eWifiCtlInterface = WIFI_CTL_NONE;

static HBOOL s_bIsScanComplete = FALSE;
static HBOOL s_bIsConnected = FALSE;
static HBOOL s_bIsReconnecting = FALSE;

static const WIFI_CHANNEL_LUT_t g_stWifiChLut[WIFI_CHANNEL_LUT_SIZE] = {
	/* 2.4 GHz - 14 channels */
	{1, 2412}, {2, 2417}, {3, 2422}, {4, 2427}, {5, 2432},
	{6, 2437}, {7, 2442}, {8, 2447}, {9, 2452}, {10, 2457},
	{11, 2462}, {12, 2467}, {13, 2472}, {14, 2484},
	/* 5 GHz - 28 channels */
	{34, 5170}, {36, 5180}, {38, 5190}, {40, 5200}, {42, 5210},
	{44, 5220}, {46, 5230}, {48, 5240}, {52, 5260}, {56, 5280},
	{60, 5300}, {64, 5320}, {100, 5500}, {104, 5520}, {108, 5540},
	{112, 5560}, {116, 5580}, {120, 5600}, {124, 5620}, {128, 5640},
	{132, 5660}, {136, 5680}, {140, 5700}, {149, 5745}, {153, 5765},
	{157, 5785}, {161, 5805}, {165, 5825}
};

/*------------------------------------------------------------------*/
/*
	Function List.
 */
/*------------------------------------------------------------------*/
static void P_WIFI_NotifyDImsg(DRV_Wireless_Event_t etWirelessEventType,void *pucWirelessInfo)
{
	wireless_info_t		*pInfoFromDI = NULL;
	HINT32		nVkErr = VK_OK;
	WIFI_MSG_t	stWiFiMsg;

	pInfoFromDI = (wireless_info_t *)pucWirelessInfo;

	VK_MEM_Memset(stWiFiMsg.acDevName,0x00,sizeof(stWiFiMsg.acDevName));

	switch(etWirelessEventType)
	{
		case DRV_WIRELESS_Insert:

			stWiFiMsg.eMsg = WIFI_EVT_INSERT;
			VK_strncpy((char *)stWiFiMsg.acDevName, (char *)pInfoFromDI->devName, sizeof(stWiFiMsg.acDevName) -1);
			nVkErr = VK_MSG_Send(s_stWiFiInstance.ulMsg, &stWiFiMsg, sizeof(WIFI_MSG_t));
			if(nVkErr != VK_OK)
			{
				PrintError("[%s][%d] Fail to send message (%d)!!\n",__FUNCTION__, __LINE__, nVkErr);
			}

			break;

		case DRV_WIRELESS_Extract:

			stWiFiMsg.eMsg = WIFI_EVT_EXTRACT;
			VK_strncpy((char *)stWiFiMsg.acDevName, (char *)pInfoFromDI->devName, sizeof(stWiFiMsg.acDevName) -1);

			nVkErr = VK_MSG_Send(s_stWiFiInstance.ulMsg, &stWiFiMsg, sizeof(WIFI_MSG_t));
			if(nVkErr != VK_OK)
			{
				PrintError("[%s][%d] Fail to send message (%d)!!\n",__FUNCTION__, __LINE__, nVkErr);
			}

			break;

		default:

			break;
	}
}


#if defined(CONFIG_BCM43567)
static DRV_Error P_CheckPcieWifiDevice(HBOOL *bConnected)
{
	DRV_Error dResult = DRV_OK;
	int ret = 0;
	FILE *fp;
	HCHAR *pIdBuf = NULL;
	long int ulFileSize=0;
	HBOOL bPciDetected = FALSE;

	/* Open PCI device info */
	VK_SYSTEM_Command("cat /proc/bus/pci/devices > /tmp/pci_devices.txt");
	fp = fopen("/tmp/pci_devices.txt","r");
	if(fp == NULL)
	{
		PrintError("[%s][%d] Err fopen %s\n", __FUNCTION__, __LINE__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	/* Get file size */
	ret = fseek(fp, 0, SEEK_END);
	if(ret != 0)
	{
		PrintError("[%s][%d] Err fseek %s\n", __FUNCTION__, __LINE__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	ulFileSize = ftell(fp);
	if(ulFileSize <= 0)
	{
		dResult = DRV_ERR;
		goto go_out;
	}

	ret = fseek(fp, 0, SEEK_SET);
	if(ret != 0)
	{
		PrintError("[%s][%d] Err fseek %s\n", __FUNCTION__, __LINE__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	/* Get vendor & device ID */
	pIdBuf = DD_MEM_Alloc(PCI_INFO_SIZE);
	while(fgets(pIdBuf, PCI_INFO_SIZE, fp))
	{
		if(strstr(pIdBuf, BCM_WIFI_VENDOR_DEVICE_ID))
		{
			bPciDetected = TRUE;
			break;
		}

		VK_MEM_Memset(pIdBuf, 0x0, PCI_INFO_SIZE);
	}

	if(bPciDetected == FALSE)
	{
		PrintError("[%s][%d] PCI Wireless Not found\n", __FUNCTION__, __LINE__);
	}


go_out :
	if(pIdBuf != NULL)
	{
		DD_MEM_Free(pIdBuf);
	}

	if(fp != NULL)
	{
		fclose(fp);
	}

	VK_SYSTEM_Command("rm -f /tmp/pci_devices.txt");

	*bConnected = bPciDetected;

	return dResult;

}


static void P_CheckPcieWifiConnection(void)
{
	DRV_Error dResult = DRV_ERR;
	HBOOL bPcieWifi;
	wireless_info_t wireless;

	dResult = P_CheckPcieWifiDevice(&bPcieWifi);
	if(dResult != DRV_OK)
	{
		PrintError("[%s][%d] Fail to check PCIe WIFI device (%d) \n", __FUNCTION__, __LINE__, dResult);
		return;
	}
	else
	{
		if(bPcieWifi == TRUE)
		{
			VK_MEM_Memset(wireless.devName,0x00,sizeof(wireless.devName));
			VK_snprintf(wireless.devName, sizeof(wireless.devName),BCM_WIFI_CONTROLLER);

			P_WIFI_NotifyDImsg(DRV_WIRELESS_Insert, &wireless);
		}
	}
}
#endif


static void P_WIFI_SetBrcmUp(void)
{
	/* Currently, do nothing */
	return ;
}


static void P_WIFI_SetBrcmDown(void)
{
	/* Currently, do nothing */
	return ;
}


/* P_WIFI_DisConnectCallback 호출 시 s_stWiFiInstance.ulDisConnectTimerId 값 체크 하지 않는다. */
/* Semaphore 로 보호 되지 않음 */
/* WIFI_EVT_DISCONNECT 받는 부분에서 DISCONNECT 상태 체크 함 */
static void P_WIFI_DisConnectCallback(unsigned long ulTid, void *arg)
{
	HINT32 		nVkErr = 0;
	WIFI_MSG_t 	stWiFiMsg;
	HUINT32 ulIndex = *((HUINT32 *)arg);

	if(s_stWiFiInstance.ulDisConnectTimerId == ulTid)
	{
		stWiFiMsg.ulIndex = ulIndex;
		stWiFiMsg.eMsg = WIFI_EVT_CONNECT_FAIL;

		nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &stWiFiMsg, sizeof(WIFI_MSG_t),0);
		if( nVkErr != 0 )
		{
			PrintError("[%s][%d] Fail to send message (%d)!!\n",__FUNCTION__, __LINE__, nVkErr);
		}
	}
	s_stWiFiInstance.ulDisConnectTimerId = (HUINT32)NULL;
	return ;
}


static HBOOL P_WIFI_FindDevice(HINT8 *pDevName)
{
	HINT8		acBuff[1024] = {0, };
	FILE		*pFile = NULL;
	HINT8		acTmpBuff[1024] = {0, };
	HINT32 nVkErr = 0;

	pFile = fopen("/proc/net/wireless", "r");
	if(pFile == NULL)
	{
		return FALSE;
	}

	/* Success : use data from /proc/net/wireless */
	/* Eat 2 lines of header */
	fgets((char*)acBuff, sizeof(acBuff), pFile);
	fgets((char*)acBuff, sizeof(acBuff), pFile);

	while(fgets((char*)acBuff, sizeof(acBuff), pFile))
	{
		HINT8	*pcStart = acBuff;
		HINT8	*pcEnd;

		/* Skip empty or almost empty lines. It seems that in some
		 * cases fgets return a line with only a newline. */
		if((pcStart[0] == '\0') || (pcStart[1] == '\0'))
		{
			continue;
		}

		/* Skip leading spaces */
		while(isspace(*pcStart))
		{
			pcStart++;
		}

		/* Get name up to ": "
		 * Note : we compare to ": " to make sure to process aliased interfaces
		 * properly. Doesn't work on /proc/net/dev, because it doesn't guarantee
		 * a ' ' after the ':'*/
		pcEnd = (HINT8*)strstr((char*)pcStart, (char*)": ");

		/* Not found ??? To big ??? */
		if((pcEnd == NULL) || (((pcEnd - pcStart) + 1) > IFNAMSIZ + 1))
		{
			continue;
		}
		VK_MEM_Memset(acTmpBuff,0,1024);
		VK_memcpy(acTmpBuff, pcStart, (pcEnd - pcStart));
		/* add & delete */
	}
	fclose(pFile);

	nVkErr = VK_memcmp(pDevName,acTmpBuff,IFNAMSIZ);
	if(nVkErr != 0)
	{
		return FALSE;
	}

	return TRUE;

}


static HUINT32 P_WIFI_GetIndexbyDevId(HUINT32 ulDevId)
{
	HUINT32 ulLoop =0;

	for(ulLoop = 0; ulLoop < WIFI_MAX_NUM; ulLoop++)
	{
		if(s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId == ulDevId)
		{
			break;
		}
	}
	return ulLoop;
}


static DRV_Error P_WIFI_IsKeyValid(DI_WIFI_INFO_t *pstConfig)
{
	DRV_Error dResult = DRV_ERR;
	HINT32 nKeyLength = 0;

	nKeyLength = strlen(pstConfig->aucKey);

	switch (pstConfig->eSecureType)
	{
		case DI_WIFI_SECURE_OPEN :
		{
			PrintError("[%s][%d] : Security OPEN, Key existed!, Ignored \n", __FUNCTION__, __LINE__);
			dResult = DRV_OK;
			break;
		}
		case DI_WIFI_SECURE_WEP :
		{
			if ((nKeyLength == 5) || (nKeyLength == 13) || (nKeyLength == 10) || (nKeyLength = 26))
			{
				dResult = DRV_OK;
			}
			else
			{
				PrintError("[%s][%d] : Security WEP, Key is invalid %d \n", __FUNCTION__, __LINE__, nKeyLength);
				dResult = DRV_ERR;
			}
			break;
		}

		case DI_WIFI_SECURE_WPA :
		case DI_WIFI_SECURE_WPA2 :
		case DI_WIFI_SECURE_WPA_WPA2:
		{
			if (nKeyLength >= 8)
			{
				dResult = DRV_OK;
			}
			else
			{
				PrintError("[%s][%d] : Security WPA or WPA2, Key is invalid %d \n", __FUNCTION__, __LINE__, nKeyLength);
				dResult = DRV_ERR;
			}
			break;
		}

		default :
		{
			PrintError("[%s][%d] : Security is invalid %d \n", __FUNCTION__, __LINE__, pstConfig->eSecureType);
			dResult = DRV_ERR;
			break;
		}
	}

	return dResult;
}


static void P_WIFI_ConvertApInfo(DI_WIFI_INFO_t *pstDiApInfo, NETAPP_WIFI_AP_INFO pstNetappApInfo)
{
	HUINT32 ulMac[6] = {0,};
	HUINT32 ulLutIdx=0;
	HINT32 nTempRssi = 0;
	
	/* SSID */
	VK_snprintf(pstDiApInfo->aucESSID,sizeof(pstNetappApInfo.cSSID),pstNetappApInfo.cSSID);

	/* BSSID */
	VK_sscanf(pstNetappApInfo.cBSSID,"%x:%x:%x:%x:%x:%x",&ulMac[0],&ulMac[1],&ulMac[2],&ulMac[3],&ulMac[4],&ulMac[5]);
	pstDiApInfo->aucMAC[0] = ulMac[0];
	pstDiApInfo->aucMAC[1] = ulMac[1];
	pstDiApInfo->aucMAC[2] = ulMac[2];
	pstDiApInfo->aucMAC[3] = ulMac[3];
	pstDiApInfo->aucMAC[4] = ulMac[4];
	pstDiApInfo->aucMAC[5] = ulMac[5];

	/* Strength (from RSSI) */
	nTempRssi = (90 + pstNetappApInfo.lRSSI) * 2;
	if(nTempRssi > 100)
	{
		pstDiApInfo->nWaveStrength = 100;
	}
	else if(nTempRssi < 0)
	{
		pstDiApInfo->nWaveStrength = 0;
	}
	else
	{
		pstDiApInfo->nWaveStrength = nTempRssi;
	}

	/* Security & Authentication & Encryption */
	switch (pstNetappApInfo.tSecurity)
	{
		case NETAPP_WIFI_SECURITY_NONE :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_OPEN;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_OPEN;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_NONE;
			break;

		case NETAPP_WIFI_SECURITY_WEP :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_WEP;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_OPEN;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_WEP_64;
			break;

		case NETAPP_WIFI_SECURITY_WPA_PSK_TKIP :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_WPA;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP;
			break;
			
		case NETAPP_WIFI_SECURITY_WPA_PSK_AES :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_WPA;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_AES;
			break;

		case (NETAPP_WIFI_SECURITY_WPA_PSK_TKIP | NETAPP_WIFI_SECURITY_WPA_PSK_AES) :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_WPA;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
			break;
			
		case NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_WPA2;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP;
			break;
			
		case NETAPP_WIFI_SECURITY_WPA2_PSK_AES :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_WPA2;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_AES;
			break;

		case NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP | NETAPP_WIFI_SECURITY_WPA2_PSK_AES :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_WPA2;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
			break;

		case NETAPP_WIFI_SECURITY_WPA_PSK_TKIP | NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_WPA_WPA2;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP;
			break;

		case NETAPP_WIFI_SECURITY_WPA_PSK_AES | NETAPP_WIFI_SECURITY_WPA2_PSK_AES :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_WPA_WPA2;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_AES;
			break;

		case NETAPP_WIFI_SECURITY_WPA_PSK_TKIP | NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP | NETAPP_WIFI_SECURITY_WPA_PSK_AES | NETAPP_WIFI_SECURITY_WPA2_PSK_AES :
			pstDiApInfo->eSecureType = DI_WIFI_SECURE_WPA_WPA2;
			pstDiApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
			pstDiApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
			break;
			
		default:
			PrintError("[%s][%d] Invalid security type (0x%x) \n", __FUNCTION__, __LINE__, pstNetappApInfo.tSecurity);
			break;
	}

	/* WPS supporting */
	if(pstNetappApInfo.bWPS)
	{
		pstDiApInfo->bWPS = TRUE;
	}

	/* Channel */
	pstDiApInfo->ulChannel = pstNetappApInfo.ulChannel;

	/* Frequency */
	for (ulLutIdx = 0 ; ulLutIdx < WIFI_CHANNEL_LUT_SIZE ; ulLutIdx++)
	{
		if (g_stWifiChLut[ulLutIdx].ulChannelNo == pstDiApInfo->ulChannel)
		{
			pstDiApInfo->ulFreq = g_stWifiChLut[ulLutIdx].ulFrequency;
		}
	}

	/* Bit-Rate (bps) */
	pstDiApInfo->nBitRate = (pstNetappApInfo.lRate/2)*1000000;

/* Just for Debugging */
#if 0
	printf("********WIFI APs INFO********\n");
	printf("SSID:	   %s\n", pstNetappApInfo.cSSID);
	printf("BSSID:	   %s\n", pstNetappApInfo.cBSSID);
	printf("Passwd:    %s\n", pstNetappApInfo.cPassword);
	switch (pstNetappApInfo.tRSSI)
	{
		case NETAPP_WIFI_RSSI_POOR:
			printf("RSSI: I    (%d dB)\n", pstNetappApInfo.lRSSI);
			break;
		case NETAPP_WIFI_RSSI_FAIR:
			printf("RSSI: II   (%d dB)\n", pstNetappApInfo.lRSSI);
			break;
		case NETAPP_WIFI_RSSI_GOOD:
			printf("RSSI: III  (%d dB)\n", pstNetappApInfo.lRSSI);
			break;
		case NETAPP_WIFI_RSSI_EXCELLENT:
			printf("RSSI: IIII (%d dB)\n", pstNetappApInfo.lRSSI);
			break;
		default:
		case NETAPP_WIFI_RSSI_NONE:
			printf("RSSI: NONE\n");
			break;
	}

	printf("802.11:    ");
	if (pstNetappApInfo.tMode & NETAPP_WIFI_802_11_MODE_A) { printf("a "); }
	if (pstNetappApInfo.tMode & NETAPP_WIFI_802_11_MODE_B) { printf("b "); }
	if (pstNetappApInfo.tMode & NETAPP_WIFI_802_11_MODE_G) { printf("g "); }
	if (pstNetappApInfo.tMode & NETAPP_WIFI_802_11_MODE_N) { printf("n "); }
	if (pstNetappApInfo.tMode & NETAPP_WIFI_802_11_MODE_AC) { printf("ac "); }
	printf("\n");

	printf("Security(0x%x):  ", pstNetappApInfo.tSecurity);
	switch (pstNetappApInfo.tSecurity)
	{
		case NETAPP_WIFI_SECURITY_NONE :
			printf("No Security\n");
			break;

		case NETAPP_WIFI_SECURITY_WEP :
			printf("Shared or Open, WEP\n");
			break;

		case NETAPP_WIFI_SECURITY_WPA_PSK_TKIP :
			printf("WPA-Personal, TKIP encryp\n");
			break;
			
		case NETAPP_WIFI_SECURITY_WPA_PSK_AES :
			printf("WPA-Personal, AES encryp\n");
			break;

		case (NETAPP_WIFI_SECURITY_WPA_PSK_TKIP | NETAPP_WIFI_SECURITY_WPA_PSK_AES) :
			printf("WPA-Personal, TKIP/AES encryp\n");
			break;
			
		case NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP :
			printf("WPA2-Personal, TKIP encryp.\n");
			break;
			
		case NETAPP_WIFI_SECURITY_WPA2_PSK_AES :
			printf("WPA2-Personal, AES encryp.\n");
			break;

		case NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP | NETAPP_WIFI_SECURITY_WPA2_PSK_AES :
			printf("WPA2-Personal, TKIP/AES encryp\n");
			break;

		case NETAPP_WIFI_SECURITY_WPA_PSK_TKIP | NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP :
			printf("WPA/WPA2-Personal, TKIP encryp\n");
			break;

		case NETAPP_WIFI_SECURITY_WPA_PSK_AES | NETAPP_WIFI_SECURITY_WPA2_PSK_AES :
			printf("WPA/WPA2-Personal, AES encryp\n");
			break;

		case NETAPP_WIFI_SECURITY_WPA_PSK_TKIP | NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP | NETAPP_WIFI_SECURITY_WPA_PSK_AES | NETAPP_WIFI_SECURITY_WPA2_PSK_AES :
			printf("WPA/WPA2-Personal, TKIP/AES encryp\n");
			break;
			
		default:
			PrintError("[%s][%d] Invalid security type (0x%x) \n", __FUNCTION__, __LINE__, pstNetappApInfo.tSecurity);
			break;
	}

    printf("adHoc:     %s\n", pstNetappApInfo.bAdHoc ? "YES" : "NO");
	printf("WPS:	   %s\n", (pstNetappApInfo.bWPS  == 1) ? "YES" : "NO");
	printf("Channel:   %d\n", pstNetappApInfo.ulChannel);
	printf("Rate:	   %d Mbps\n", pstNetappApInfo.lRate/2);
	printf("Phy Noise: %d dB\n", pstNetappApInfo.lPhyNoise);
	printf("Bandwidth: ");
	switch(pstNetappApInfo.tChanBandwidth)
	{
		case NETAPP_WIFI_BANDWIDTH_10MHz:
			printf("10 MHz\n");
			break;
		case NETAPP_WIFI_BANDWIDTH_20MHz:
			printf("20 MHz\n");
			break;
		case NETAPP_WIFI_BANDWIDTH_40MHz:
			printf("40 MHz\n");
			break;
		case NETAPP_WIFI_BANDWIDTH_80MHz:
			printf("80 MHz\n");
			break;
		case NETAPP_WIFI_BANDWIDTH_160MHz:
			printf("160 MHz\n");
			break;
		case NETAPP_WIFI_BANDWIDTH_INVALID:
		default:
			printf("Unknown!\n");
			break;
	}
	printf("**************************\n");
#endif
}


static DRV_Error P_WIFI_NETAPP_GetRate(HUINT32 ulIndex, HINT32 *pValue)
{
	NETAPP_RETCODE nResult = NETAPP_FAILURE;
	NETAPP_WIFI_AP_INFO stApInfo;

	UNUSED(ulIndex);

	VK_MEM_Memset(&stApInfo, 0x0, sizeof(NETAPP_WIFI_AP_INFO));

	nResult = NetAppWiFiGetConnectedApInfo(hWifiNetapp, &stApInfo);
	if (nResult != NETAPP_SUCCESS)
	{
		PrintError("[%s][%d] Fail to get connected AP info (%d) \n", __FUNCTION__, __LINE__, nResult);
		return DRV_ERR;
	}

	*pValue = (stApInfo.lRate/2) * 1000000;

	return DRV_OK;
}


static DRV_Error P_WIFI_NETAPP_GetStrength(HUINT32 ulIndex, HINT32 *pValue)
{
	NETAPP_RETCODE nResult = NETAPP_FAILURE;
	NETAPP_WIFI_AP_INFO stApInfo;

	UNUSED(ulIndex);
	
	VK_MEM_Memset(&stApInfo, 0x0, sizeof(NETAPP_WIFI_AP_INFO));

	nResult = NetAppWiFiGetConnectedApInfo(hWifiNetapp, &stApInfo);
	if (nResult != NETAPP_SUCCESS)
	{
		PrintError("[%s][%d] Fail to get connected AP info (%d) \n", __FUNCTION__, __LINE__, nResult);
		return DRV_ERR;
	}

	/* could not call convert API */
	*pValue = (90 + stApInfo.lRSSI) * 2;
	if(*pValue > 100)
	{
		*pValue = 100;
	}
	else if(*pValue < 0)
	{
		*pValue = 0;
	}
	else
	{
		/* do nothing */
	}
	
	return DRV_OK;
}


static DRV_Error P_WIFI_NETAPP_GetRssi(HUINT32 ulIndex, DI_WIFI_RSSI_INFO_t *pRssiInfo)
{
	HUINT8 ucAntIdx = 0;

	NETAPP_RETCODE nResult = NETAPP_FAILURE;
	NETAPP_WIFI_RSSI_PER_ANTENNA stRssiPerAnt;

	UNUSED(ulIndex);
	
	VK_MEM_Memset(&stRssiPerAnt, 0x0, sizeof(NETAPP_WIFI_RSSI_PER_ANTENNA));
	
	nResult = NetAppWiFiTestGetRssiPerAntenna(hWifiNetapp, &stRssiPerAnt);
	if (nResult != NETAPP_SUCCESS)
	{
		PrintError("[%s][%d] Fail to get RSSIs of each Antenna (%d)\n", __FUNCTION__, __LINE__, nResult);
		
		return DRV_ERR;
	}

#if defined(CONFIG_BCM43567)
	pRssiInfo->iAntCount = stRssiPerAnt.ulNumAntennas - 1;
#else
	pRssiInfo->iAntCount = stRssiPerAnt.ulNumAntennas;
#endif

	for (ucAntIdx = 0; ucAntIdx < pRssiInfo->iAntCount ; ucAntIdx++)
	{
		pRssiInfo->uiAntRssi[ucAntIdx] = stRssiPerAnt.lRSSI_ant[ucAntIdx];
	}

	pRssiInfo->uiRssi = stRssiPerAnt.lRSSI;
	
	return DRV_OK;
}

static DRV_Error P_WIFI_NETAPP_StartScanAp(HUINT32 ulIndex)
{
    HUINT32 ulNumScannedAp=0, ulApIdx=0;
	HUINT8 ucScanCheckCounter = 0;

    NETAPP_RETCODE nResult = NETAPP_FAILURE;

    nResult = NetAppWiFiStartScan(hWifiNetapp, NETAPP_SCAN_REPORTING_FREQUENCY, NETAPP_SCAN_CHANNEL_HOVER_PERIOD);
    if (nResult != NETAPP_SUCCESS)
    {
        PrintError("[%s][%d] Fail to start scan (%d) \n", __FUNCTION__, __LINE__, nResult);
		return DRV_ERR;
    }

	VK_TASK_Sleep(WIFI_SCAN_DONE_CHECK*2);

	while (s_bIsScanComplete != TRUE)
	{
		VK_TASK_Sleep(WIFI_SCAN_DONE_CHECK);
		if (ucScanCheckCounter*WIFI_SCAN_DONE_CHECK > SCAN_TIMEOUT)
		{
			PrintError("[%s][%d] Fail to scan because of time-out (%d) \n", __FUNCTION__, __LINE__, ucScanCheckCounter);
			return DRV_ERR;
		}
		ucScanCheckCounter++;
	}

	if(s_bIsScanComplete)
	{
		NETAPP_WIFI_AP_INFO *pApInfoList = NULL;

		pApInfoList = DD_MEM_Alloc(sizeof(NETAPP_WIFI_AP_INFO)*WIFI_AP_MAX);
		
		nResult = NetAppWiFiGetScanResults(hWifiNetapp, &pApInfoList, &ulNumScannedAp);
		if (nResult != NETAPP_SUCCESS)
		{
			PrintError("[%s][%d] Fail to get scan results (%d) \n", __FUNCTION__, __LINE__, nResult);
			return DRV_ERR;
		}

		if (pApInfoList)
		{
			s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs = ulNumScannedAp;
			PrintDebug("[%s][%d] Total Number of Scanned AP = %d \n", __FUNCTION__, __LINE__, s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs);
			
			for (ulApIdx=0; ulApIdx<ulNumScannedAp; ulApIdx++)
			{
				P_WIFI_ConvertApInfo(&s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[ulApIdx], pApInfoList[ulApIdx]);
			}
		}

		if (pApInfoList)
	    {
	        DD_MEM_Free(pApInfoList);
	    }
	}

	return DRV_OK;
}


static DRV_Error P_WIFI_NETAPP_ConnectToAp(HUINT32 ulIndex, HINT8 *pszIFname, DI_WIFI_INFO_t *pstConfig)
{
    NETAPP_RETCODE nResult = NETAPP_FAILURE;
    NETAPP_WIFI_AP_INFO stApInfo;

	if ((pszIFname == NULL) || (pstConfig == NULL))
	{
		PrintError("[%s][%d] pszIFname or pstConfig is NULL ! \n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTING;
	s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_NONE;

    VK_MEM_Memset(&stApInfo, 0, sizeof(NETAPP_WIFI_AP_INFO));
	
    stApInfo.tSecurity = NETAPP_WIFI_SECURITY_AUTO_DETECT;

	VK_strncpy(stApInfo.cSSID, pstConfig->aucESSID, NETAPP_MAX_SSID_LEN);
    VK_strncpy(stApInfo.cPassword, pstConfig->aucKey, NETAPP_MAX_PASSWORD_LEN);
	
	nResult = NetAppWiFiConnect(hWifiNetapp, &stApInfo);
	if (nResult != NETAPP_SUCCESS)
	{
		   PrintError("[%s][%d] Fail to start connection (%d) \n", __FUNCTION__, __LINE__, nResult);
		   return DRV_ERR;
    }

	return DRV_OK;
}


static void P_WIFI_NETAPP_ConnectWpsToAp(HUINT32 ulIndex, WIFI_EVENT_e eConnectType)
{
	NETAPP_RETCODE nResult = NETAPP_FAILURE;

	s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTING;

	if(eConnectType == WIFI_EVT_WPS_PBC_START)
	{
		PrintDebug("[%s][%d] WPS connection type is PBC \n", __FUNCTION__, __LINE__);
		
		s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_PBC;
		
		nResult = NetAppWiFiConnectByPb(hWifiNetapp);
		if (nResult != NETAPP_SUCCESS)
		{
			PrintError("[%s][%d] Fail to start connection via WPS PBC (%d) \n", __FUNCTION__, __LINE__, nResult);
		}
	}
	else
	{
		HUINT32 ulWpsPinCode = 0;

		PrintDebug("[%s][%d] WPS connection type is PIN \n", __FUNCTION__, __LINE__);
		
		s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_PIN;

		ulWpsPinCode = \
					+ s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[0] * 10000000 \
					+ s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[1] * 1000000 \
					+ s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[2] * 100000 \
					+ s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[3] * 10000 \
					+ s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[4] * 1000 \
					+ s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[5] * 100 \
					+ s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[6] * 10 \
					+ s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[7];

		PrintDebug("[%s][%d] WPS PIN code = %d \n", __FUNCTION__, __LINE__, ulWpsPinCode);
		
		nResult = NetAppWiFiConnectByPin(hWifiNetapp, NULL, ulWpsPinCode, TRUE);
		if (nResult != NETAPP_SUCCESS)
		{
			PrintError("[%s][%d] Fail to start connection via WPS PIN (%d) \n", __FUNCTION__, __LINE__, nResult);
		}
	}

	return ;
}


static DRV_Error P_WIFI_NETAPP_Disconnect(HUINT32 ulIndex)
{
    NETAPP_RETCODE nResult = NETAPP_FAILURE;

	UNUSED(ulIndex);

	nResult = NetAppWiFiDisconnect(hWifiNetapp);
	if (nResult != NETAPP_SUCCESS)
	{
		   PrintError("[%s][%d] Fail to disconnect (%d) \n", __FUNCTION__, __LINE__, nResult);
		   return DRV_ERR;
    }

	return DRV_OK;
}


void P_WIFI_Netapp_Callback(void* pParam, NETAPP_CB_TYPE tCbType, const void *pvBuffer, HUINT32 ulData0, NETAPP_RETCODE tResult, NETAPP_IFACE tIFace)
{
	UNUSED(pParam);
	UNUSED(pvBuffer);

	if (!hWifiNetapp || (tIFace != NETAPP_IFACE_WIRELESS))
	{
		PrintError("[%s][%d] hWifiNetapp is NULL or tIFace is not for WIFI(%d)! \n", __FUNCTION__, __LINE__, tIFace);
		return ;
	}
	
	switch(tCbType)
	{
		case NETAPP_CB_SCAN_DONE:
			{
				switch(tResult)
				{
					case NETAPP_SUCCESS:
					case NETAPP_SCAN_EMPTY:
						s_bIsScanComplete = TRUE;
						break;

					case NETAPP_TIMEOUT:
					case NETAPP_CANCELED:
						s_bIsScanComplete = FALSE;
						break;

					default:
						PrintDebug("[%s][%d] Unhandled result code (%d) for SCAN \n", __FUNCTION__, __LINE__, tResult);
						break;
				}
			}
			break;

		case NETAPP_CB_LINK:
			{
				if (ulData0 == NETAPP_LINK_DOWN)
				{
					if (s_bIsConnected == TRUE)
					{
						PrintDebug("[%s][%d] Connection lost detected (%d)(%d) \n", __FUNCTION__, __LINE__, tCbType, ulData0);
						s_bIsReconnecting = TRUE;
						s_bIsConnected = FALSE;
					}
				}
				else
				{
					if (s_bIsReconnecting == TRUE)
					{
						if (s_bIsConnected == FALSE)
						{
							PrintDebug("[%s][%d] Connection re-established (%d)(%d) \n", __FUNCTION__, __LINE__, tCbType, ulData0);
							s_bIsReconnecting = FALSE;
							s_bIsConnected = TRUE;
						}
					}
				}
#if 0
				nret = NetAppWiFiGetConnectedApInfo(hWifiNetapp, &tApInfo);
				if (NETAPP_SUCCESS != nret)
				{
					if (TRUE == dw_handle->disconnectionInProgress)
					{
						/* Note that dwlan_disconnect() will still issue the connection-cancelled event */
						PrintDebug("Ignoring connection lost status since explicit disconnection request was made");
					}
					else
					{
						/* This appears to be a spontaneous (unexpected) drop in connection */
						update_stats(FALSE, dw_handle, STATS_INC_DISCONNECTION_COUNT);

						PrintDebug("Invoking dw_callback for connection being lost (id = %d)...", dw_handle->connect_request_id);
						dw_handle->interface_state = DWLAN_ST_CONNECTING;
						dw_handle->dw_callback(dw_handle, dw_handle->caller_handle, DWLAN_CB_CONNECT, DWLAN_CONNECT_ST_LOST, dw_handle->connect_request_id);
					}

					PrintDebug("Connection loss was detected");
					SIGNAL_EVENT(CMD_EVENT_WIFI_CONNECTION_LOSS_DETECTED);
				}
#endif
			}
			break;

		case NETAPP_CB_RSSI_EVENT:
			{
				PrintDebug("[%s][%d] NetApp reported rssi event with value %d \n", __FUNCTION__, __LINE__, ulData0);
#if 0
				switch(ulData0)
				{
					case NETAPP_WIFI_RSSI_NONE:
						rssi = DWLAN_RSSI_NONE;
						break;
					case NETAPP_WIFI_RSSI_POOR:
						rssi = DWLAN_RSSI_POOR;
						break;
					case NETAPP_WIFI_RSSI_FAIR:
						rssi = DWLAN_RSSI_FAIR;
						break;
					case NETAPP_WIFI_RSSI_GOOD:
						rssi = DWLAN_RSSI_GOOD;
						break;
					case NETAPP_WIFI_RSSI_EXCELLENT:
						rssi = DWLAN_RSSI_EXCELLENT;
						break;
					default:
						rssi = DWLAN_RSSI_NONE;
						break;
				}
#endif
			}
			break;

		case NETAPP_CB_CONNECT:
			{
#if 1
				if (tResult ==  NETAPP_SUCCESS)
				{
					PrintDebug("[%s][%d] Connection Established (%d)(%d)\n", __FUNCTION__, __LINE__, tCbType, tResult);
					s_bIsConnected = TRUE;
				}
				else
				{
					s_bIsConnected = FALSE;
				}
#else
				switch(tResult)
				{
					case NETAPP_SUCCESS:
						result = DWLAN_CONNECT_ST_SUCCESS;

						/* save the current time - will be used in subsequent calculations to provide uptime statistics */
						time(&(dw_handle->connectedAt));
						break;

					case NETAPP_FAILURE:
						autoRetry = TRUE;
						result = DWLAN_CONNECT_ST_FAILURE;
						break;

					case NETAPP_INVALID_PARAMETER:
						dw_handle->fault_tolerance_count++;
						if (dw_handle->fault_tolerance_count <= DWLAN_CONNECTION_FAILURE_TOLERANCE_LIMIT)
						{
							DBG("Cannot preclude a bad router, so DWLAN will retry again (tolerance counter = %d)", dw_handle->fault_tolerance_count);
							autoRetry = TRUE;
						}
						else
						{
							DBG("Router was given enough time to stabilize, all attempts failed, we will now give up.");
							dw_handle->fault_tolerance_count = 0;
						}
						result = DWLAN_CONNECT_ST_INVALID_PASSWORD;
						break;

					case NETAPP_INCORRECT_PASSWORD:
						result = DWLAN_CONNECT_ST_INCORRECT_PASSWORD;
						break;

					case NETAPP_INVALID_PIN:
						result = DWLAN_CONNECT_ST_INVALID_PASSWORD;
						break;

					case NETAPP_TIMEOUT:
						autoRetry = TRUE;
						result = DWLAN_CONNECT_ST_TIMEOUT;
						break;

					case NETAPP_CANCELED:
						result = DWLAN_CONNECT_ST_CANCELLED;
						break;

					case NETAPP_WPS_MULTIPLE_AP_FOUND:
						result = DWLAN_CONNECT_ST_PB_MULTIPLE_AP_FOUND;
						break;

					default:
						autoRetry = TRUE;
						result = DWLAN_CONNECT_ST_FAILURE;
						break;
				}

				if (NETAPP_SUCCESS == tResult)
				{
					PrintDebug("NetApp indicates a successful connection");
					dw_handle->interface_state = DWLAN_ST_CONNECTED;
					memset(dw_handle->pCurrentAp, 0, sizeof(dwlan_ap_info));
					dw_handle->fault_tolerance_count = 0;
				}
				else
				{
					PrintDebug("NetApp indicates a failed connection attempt");
					if ((DWLAN_ST_CONNECTED != dw_handle->interface_state) && (DWLAN_ST_CONNECTING != dw_handle->interface_state))
					{
						dw_handle->interface_state = (TRUE == dw_handle->netapp_scanning) ? DWLAN_ST_SCANNING : DWLAN_ST_IDLE;
					}
					else if (FALSE == dw_handle->retryInProgress)
					{
						dw_handle->interface_state = DWLAN_ST_IDLE;
					}

					if (autoRetry)
					{
						/* we don't know for sure that the failure was caused by bad user inputs */
						PrintDebug("Failure was probably not due to bad user inputs");
						if (dw_handle->pCurrentAp->ssid[0] != '\0')
						{
							/* DWLAN may need to take responsibility for automatic reconnections using the cached information */
							if (FALSE == dw_handle->netapp_scanning)
							{
								if (FALSE == dw_handle->retryInProgress)
								{
									/* DWLAN has not already started its auto-reconnection routine */
									msg.cmd = CMD_INITIATE_AUTO_RECONNECTION;
									OS_MsgQSend(dw_handle->msgQid, (char*)&msg, sizeof(message), 0, 0);
								 }
								 else
								 {
									DBG("DWLAN auto-reconnection process is already active");
								 }
							}
							else
							{
								DBG("Deferring auto-connection until after scan is finished or aborted");
							}
						}
						else
						{
							DBG("DWLAN does not have cached credentials, therefore NetApp must re-attempt connection");
							dw_handle->interface_state = DWLAN_ST_CONNECTING;
						}
					}
					else
					{
						DBG("Failure was due to bad user inputs");
						memset(dw_handle->pCurrentAp, 0, sizeof(dwlan_ap_info));
						dw_handle->fault_tolerance_count = 0;
					}
				}

				if (dw_handle->fault_tolerance_count == 0)
				{
					DBG("Invoking dw_callback for connection results (netapp result code = %d, id = %d)...", tResult, dw_handle->connect_request_id);
					dw_handle->dw_callback(dw_handle, dw_handle->caller_handle, DWLAN_CB_CONNECT, result, dw_handle->connect_request_id);
				}
#endif
			}
			break;

		case NETAPP_CB_FETCHED_APINFO:
			{
#if 0
				switch(tResult)
				{
					case NETAPP_SUCCESS:
						result = DWLAN_SCAN_ST_COMPLETE;
						break;

					case NETAPP_FAILURE:
					case NETAPP_TIMEOUT:
						result = DWLAN_SCAN_ST_TIMEOUT;
						break;

					default:
						result = DWLAN_SCAN_ST_COMPLETE;
						DBG("Unhandled result code (%d) encountered", tResult);
						break;
				}

				if (DWLAN_ST_SCANNING == dw_handle->interface_state)
				{
					dw_handle->interface_state = DWLAN_ST_IDLE;
				}

				PrintDebug("Invoking dw_callback for fetched AP results (netapp result code = %d, id = %d)...", tResult, dw_handle->scan_request_id);
				dw_handle->dw_callback(dw_handle, dw_handle->caller_handle, DWLAN_CB_SCAN, result, dw_handle->scan_request_id);

				if ((FALSE == dw_handle->retryInProgress) && (dw_handle->pCurrentAp->ssid[0] != '\0'))
				{
					PrintDebug("Now that the scan is over, need to restart the auto-connection sequence");
					msg.cmd = CMD_INITIATE_AUTO_RECONNECTION;
					OS_MsgQSend(dw_handle->msgQid, (char*)&msg, sizeof(message), 0, 0);
				}
#endif
			}
			break;

		case NETAPP_CB_WPS_PROGRESS:
	        {
				PrintDebug("[%s][%d] WPS connection progress (%d)(%d) \n", __FUNCTION__, __LINE__, tCbType, ulData0);
	        }
	        break;

		default:
			PrintDebug("[%s][%d] Unhandled NetApp callback type (%d) \n", __FUNCTION__, __LINE__, tCbType);
			break;
	}

}


static void P_WIFI_NETAPP_CheckConnectStatus(HUINT32 ulIndex, HUINT32 ulMsgTime)
{
	WIFI_MSG_t	WiFiMsg;
	HINT32	nVkErr = 0;

	UNUSED(ulIndex);
	UNUSED(ulMsgTime);

	if (s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus >= WIFI_STATUS_DISCONNECTED)
	{
		/* PrintDebug("\n(*) WIFI_STATUS_MONITOR : Not Connected (Status = %d)\n",s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus); */
		goto ret;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTING
		|| s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_RECONNECTING)
	{
		PrintDebug("[%s][%d] WIFI_STATUS_CONNECTING\n", __FUNCTION__, __LINE__);

		if (s_bIsConnected == TRUE)
		{
			PrintDebug("[%s][%d] WIFI_STATUS_CONNECTED\n", __FUNCTION__, __LINE__);
			
			if(s_stWiFiInstance.ulDisConnectTimerId)
			{
				VK_TIMER_Cancel(s_stWiFiInstance.ulDisConnectTimerId);
				s_stWiFiInstance.ulDisConnectTimerId = (HUINT32)NULL;
			}

			WiFiMsg.ulIndex = ulIndex;
			WiFiMsg.eMsg = WIFI_EVT_CONNECT_OK;

			nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
			if(nVkErr)
			{
				PrintError("[%s][%d] Fail to send message (%d)!!\n",__FUNCTION__, __LINE__, nVkErr);
			}
		}

		goto ret;
	}
	else if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTED)
	{
		if (s_bIsConnected != TRUE)
		{
			PrintDebug("[%s][%d] WIFI_STATUS_RECONNECTING \n", __FUNCTION__, __LINE__);
			s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_RECONNECTING;
			if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_FAIL].pfnCallback)
			{
				DI_WIFI_CALLBACK_DATA_t stCallbackData;
				stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
				stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_CONNECT_FAIL;
				(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_FAIL].pfnCallback)(&stCallbackData);
			}
		}

		goto ret;
	}

ret :
	return ;
}


static DRV_Error P_WIFI_GetRate(HUINT32 ulIndex, HINT32 *pValue)
{
	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		P_WIFI_NETAPP_GetRate(ulIndex, pValue);
	}
	else
	{
		PrintError("[%s][%d] Error : s_eWifiCtlInterface (%d) \n", __FUNCTION__, __LINE__, s_eWifiCtlInterface);
		*pValue = 0;
	}

	return DRV_OK;
}


static DRV_Error P_WIFI_GetStrength(HUINT32 ulIndex, HINT32 *pValue)
{
	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		P_WIFI_NETAPP_GetStrength(ulIndex, pValue);
	}
	else
	{
		PrintError("[%s][%d] Error : s_eWifiCtlInterface %d \n", __FUNCTION__, __LINE__, s_eWifiCtlInterface);
		*pValue = 0;
	}

	return DRV_OK;
}


static DRV_Error P_WIFI_StartScanAp(HUINT32 ulIndex)
{
	DRV_Error dResult = DRV_OK;

	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		dResult = P_WIFI_NETAPP_StartScanAp(ulIndex);
		if(dResult != DRV_OK)
		{
			goto ret;
		}
	}
	else
	{
		PrintError("[%s][%d] Error : s_eWifiCtlInterface (%d) \n", __FUNCTION__, __LINE__, s_eWifiCtlInterface);
		dResult = DRV_ERR;
		goto ret;
	}

ret:
	return dResult;

}


static void P_WIFI_ConnectToAp(HUINT32 ulIndex, HINT8 *pszIFname, DI_WIFI_INFO_t *pstConfig)
{
	DRV_Error dResult = DRV_OK;

	if(s_eWifiCtlInterface ==WIFI_CTL_WL)
	{
		dResult = P_WIFI_NETAPP_ConnectToAp(ulIndex, pszIFname, pstConfig);
		if(dResult != DRV_OK)
		{
			goto ret;
		}
	}
	else
	{
		PrintError("[%s] Error : s_eWifiCtlInterface %d \n", __FUNCTION__, s_eWifiCtlInterface);
		goto ret;
	}

ret:
	return ;

}


static void P_WIFI_ConnectWpsToAp(HUINT32 ulIndex, WIFI_EVENT_e eConnectType)
{
	if((eConnectType != WIFI_EVT_WPS_PBC_START) && (eConnectType != WIFI_EVT_WPS_PIN_START))
	{
		PrintError("[%s] Error : eConnectType %d \n", __FUNCTION__, eConnectType);
		goto ret;
	}

	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		P_WIFI_NETAPP_ConnectWpsToAp(ulIndex, eConnectType);
	}
	else
	{
		PrintError("[%s] Error : s_eWifiCtlInterface %d \n", __FUNCTION__, s_eWifiCtlInterface);
		goto ret;
	}

ret:
	return ;
}


static void P_WIFI_Disconnect(HUINT32 ulIndex)
{
	DRV_Error dResult = DRV_OK;

	if(s_eWifiCtlInterface ==WIFI_CTL_WL)
	{
		dResult = P_WIFI_NETAPP_Disconnect(ulIndex);
		if(dResult != DRV_OK)
		{
			goto ret;
		}
	}
	else
	{
		PrintError("[%s] Error : s_eWifiCtlInterface %d \n", __FUNCTION__, s_eWifiCtlInterface);
		goto ret;
	}

ret:
	return ;

}


static void P_WIFI_CheckConnectStatus(HUINT32 ulIndex, HUINT32 ulMsgTime)
{
	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		P_WIFI_NETAPP_CheckConnectStatus(ulIndex, ulMsgTime);
	}
	else
	{
		PrintError("[%s][%d] WIFI is not controlled : s_eWifiCtlInterface (%d) \n", __FUNCTION__, __LINE__, s_eWifiCtlInterface);
	}

	return ;
}


static void P_WIFI_Task(void *pvParam)
{
	DRV_Error dResult = DRV_OK;
	int			vkMsg;
	WIFI_MSG_t	WiFiMsg;
	char cmdBuf[256] = { 0 };
	char devName[256] = { 0 };
	HBOOL bIsDev = FALSE;
	HUINT32 ulLoop = 0;
	HUINT32 ulIndex = 0;


	pvParam = pvParam;

	while (1)
	{
		vkMsg = VK_MSG_ReceiveTimeout( s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t), WIFI_CONNECT_CHECK );
		if( vkMsg != VK_OK )
		{
			if ( vkMsg == VK_TIMEOUT )
			{
				HUINT32	ulTmpLoop = 0;
				HUINT32	ulTmpIndex = 0;
				for (ulTmpLoop = 0 ; ulTmpLoop < WIFI_MAX_NUM; ulTmpLoop++)
				{
					/* check device id */
					if (s_stWiFiInstance.stWiFiInfo[ulTmpLoop].ulDevId != 0xFF)
					{
						ulTmpIndex = P_WIFI_GetIndexbyDevId(s_stWiFiInstance.stWiFiInfo[ulTmpLoop].ulDevId);
						if (s_stWiFiInstance.stWiFiInfo[ulTmpIndex].eDongleStatus == WIFI_DONGLE_INSERTED)
						{
							P_WIFI_CheckConnectStatus(ulTmpIndex, 0);
						}
					}
				}
			}
			else
			{
				/* VK_ERROR */
				PrintError("[%s][%d] VK_MSG_ReceiveTimeout vkMsg = %d \n",__FUNCTION__, __LINE__, WiFiMsg.eMsg);
			}
			continue;
		}

		switch (WiFiMsg.eMsg)
		{
			case WIFI_EVT_INSERT:
				PrintDebug("[%s] WIFI_EVT_INSERT - Index : %d ,Device ID : %d ,Driver Name : %s IfName : %s \n",
					__FUNCTION__, ulLoop,s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId, WiFiMsg.acDevName,s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName);

				if ( !VK_strncmp(BCM_WIFI_CONTROLLER, (char*)WiFiMsg.acDevName,(size_t)VK_strlen(BCM_WIFI_CONTROLLER)))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "dhd" );
					s_eWifiCtlInterface = WIFI_CTL_WL;
				}

				if(s_eWifiCtlInterface == WIFI_CTL_WL)
				{
					P_WIFI_SetBrcmUp();
				}

				dResult = DRV_NETAPP_GetHandle(&hWifiNetapp);
				if (dResult != DRV_OK)
				{
					PrintError("[%s][%d] Fail to get Netapp handle (%d) !! \n", __FUNCTION__, __LINE__, dResult);
					return ;
				}

				s_stWiFiInstance.stWiFiInfo[ulLoop].eDongleStatus = WIFI_DONGLE_INSERTED;

				if(s_stWiFiInstance.stWiFiInfo[ulLoop].stCallbackInfo[DI_WIFI_NOTIFY_EVT_INSERTED].pfnCallback)
				{
					DI_WIFI_CALLBACK_DATA_t stCallbackData;
					stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId;
					stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_INSERTED;
					stCallbackData.ulNumOfData = 0;
					stCallbackData.pvEventData = NULL;
					(*s_stWiFiInstance.stWiFiInfo[ulLoop].stCallbackInfo[DI_WIFI_NOTIFY_EVT_INSERTED].pfnCallback)(&stCallbackData);
				}
				break;

			case WIFI_EVT_EXTRACT:
				for( ulLoop = 0 ; ulLoop < WIFI_MAX_NUM; ulLoop++)
				{
					if(s_stWiFiInstance.stWiFiInfo[ulLoop].eDongleStatus == WIFI_DONGLE_INSERTED)
					{
						bIsDev = P_WIFI_FindDevice((HINT8 *)s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName);
						if(bIsDev != TRUE)
						{
							break;
						}
					}
				}

				if(ulLoop == WIFI_MAX_NUM)
				{
					PrintError("[%s][%d] Do not find to Removed WIFI Interface(%s)\n", __FUNCTION__, __LINE__, WiFiMsg.acDevName);
					break;
				}

				PrintDebug("[%s] WIFI_EVT_EXTRACT - Index : %d ,Device ID : %d ,Driver Name : %s IfName : %s \n",
					__FUNCTION__, ulLoop,s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId, WiFiMsg.acDevName, s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName);

				if(s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_PBC || s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_PIN)
				{
					s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_NONE;
				}
				
				if(s_eWifiCtlInterface == WIFI_CTL_WL)
				{
					P_WIFI_SetBrcmDown();
				}

				if ( !strncmp(BCM_WIFI_CONTROLLER, (char*)WiFiMsg.acDevName,(size_t)strlen(BCM_WIFI_CONTROLLER)))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", BCM_WIFI_DRIVER_NAME);
				}
				else
				{
					break;
				}

				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod %s", devName );
				VK_SYSTEM_Command( cmdBuf );
				VK_TASK_Sleep(500);

				s_eWifiCtlInterface = WIFI_CTL_NONE;
				s_stWiFiInstance.stWiFiInfo[ulLoop].eDongleStatus = WIFI_DONGLE_EXTRACTED;

				if(s_stWiFiInstance.stWiFiInfo[ulLoop].stCallbackInfo[DI_WIFI_NOTIFY_EVT_EXTRACTED].pfnCallback)
				{
					DI_WIFI_CALLBACK_DATA_t stCallbackData;
					stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId;
					stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_EXTRACTED;
					stCallbackData.ulNumOfData = 0;
					stCallbackData.pvEventData = NULL;
					(*s_stWiFiInstance.stWiFiInfo[ulLoop].stCallbackInfo[DI_WIFI_NOTIFY_EVT_EXTRACTED].pfnCallback)(&stCallbackData);
				}

				break;
			case WIFI_EVT_SCAN_START:
				ulIndex = WiFiMsg.ulIndex;
				s_stWiFiInstance.stWiFiInfo[ulIndex].bScanning = TRUE;

				dResult = P_WIFI_StartScanAp(ulIndex);
				if(dResult == DRV_OK)
				{
#if 0 /* Just for debugging */
					HUINT32 i=0;

					for ( i = 0; i < s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs ; i++ )
					{
						printf("  --------------- AP %2d Info -------------\n", i);
						printf("  * ESSID  : %s\n", s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucESSID);
						printf("  * MAC    : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[0]+s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[1]+s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[2]+s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[3]+s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[4]+s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[5]);
						printf("  * SEC    : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eSecureType);
						printf("  * AUTH   : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eAuthenType);
						printf("  * ENC    : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eEncryptType);
						printf("  \n");
					}
#endif
					if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS].pfnCallback)
					{
						DI_WIFI_CALLBACK_DATA_t stCallbackData;
						stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
						stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS;
						stCallbackData.ulNumOfData = s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs;
						stCallbackData.pvEventData = s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList;
						(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS].pfnCallback)(&stCallbackData);
					}
				}
				else
				{
					if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_SCAN_FAIL].pfnCallback)
					{
						DI_WIFI_CALLBACK_DATA_t stCallbackData;
						stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
						stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_SCAN_FAIL;
						(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_SCAN_FAIL].pfnCallback)(&stCallbackData);
					}
				}

				s_stWiFiInstance.stWiFiInfo[ulIndex].bScanning = FALSE;

				s_bIsScanComplete = FALSE;

				break;

			case WIFI_EVT_SCAN_STOP:
				break;

			case WIFI_EVT_CONNECT_START:
				ulIndex = WiFiMsg.ulIndex;
				P_WIFI_ConnectToAp(ulIndex, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, &s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP);
				break;

			case WIFI_EVT_WPS_PBC_START:
			case WIFI_EVT_WPS_PIN_START:
				P_WIFI_ConnectWpsToAp(WiFiMsg.ulIndex, WiFiMsg.eMsg);
				break;

			case WIFI_EVT_CONNECT_OK:
				ulIndex = WiFiMsg.ulIndex;
				s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTED;
				if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS].pfnCallback)
				{
					DI_WIFI_CALLBACK_DATA_t stCallbackData;
					stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
					stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS;
					(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS].pfnCallback)(&stCallbackData);
				}

				break;

			case WIFI_EVT_DISCONNECT:
			case WIFI_EVT_CONNECT_FAIL:
				ulIndex = WiFiMsg.ulIndex;

				s_bIsConnected = FALSE;

				P_WIFI_Disconnect(ulIndex);
				
				if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_DISCONNECTED)
				{
					VK_MEM_Memset(s_stWiFiInstance.stWiFiInfo[ulIndex].aucMAC, 0x00, sizeof(s_stWiFiInstance.stWiFiInfo[ulIndex].aucMAC));

					if(s_eWifiCtlInterface == WIFI_CTL_WL)
					{
						PrintDebug("[%s][%d] Current Status = %d \n", __FUNCTION__, __LINE__, s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus);
						P_WIFI_SetBrcmDown();
						P_WIFI_SetBrcmUp();
					}

					if(s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_PBC || s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_PIN)
					{
						if(s_eWifiCtlInterface == WIFI_CTL_WL)
						{
							s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_NONE;
						}
					}

					s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_DISCONNECTED;
				}

				if(WiFiMsg.eMsg == WIFI_EVT_CONNECT_FAIL)
				{
					if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_FAIL].pfnCallback)
					{
						DI_WIFI_CALLBACK_DATA_t stCallbackData;
						stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
						stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_CONNECT_FAIL;
						(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_FAIL].pfnCallback)(&stCallbackData);
					}
				}
				break;

			default:
				break;
		}
	}
}


DRV_Error DRV_WIFI_GetDongleStatus(HUINT32 ulDevId, HBOOL *pbConnect)
{
	HUINT32 ulIndex = 0;

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM || pbConnect == NULL)
	{
		PrintError("[%s][%d] Invalid ulDevId(%d) or NULL pbConnect !!\n", __FUNCTION__, __LINE__, ulDevId);
		return DRV_ERR;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus == WIFI_DONGLE_INSERTED)
	{
		*pbConnect = TRUE;
	}
	else if(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus == WIFI_DONGLE_EXTRACTED)
	{
		*pbConnect = FALSE;
	}
	else
	{
		PrintError("[%s][%d] Invalid Dongle Status (%d) !!\n", __FUNCTION__, __LINE__, s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus);
		return DRV_ERR;
	}

	return DRV_OK;
}


DRV_Error DRV_WIFI_Init(void)
{
	DRV_Error dResult = DRV_OK;
	HINT32	nVkErr;
	HUINT32 ulLoop,ulIndex = 0;

	VK_MEM_Memset(&s_stWiFiInstance,0x00,sizeof(s_stWiFiInstance));
	nVkErr = VK_SEM_Create((unsigned long*)&s_stWiFiInstance.ulSema, "Wi-Fi", VK_SUSPENDTYPE_FIFO);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Creating Wi-Fi VK_SEM_Create failed! (%d)\n", __FUNCTION__, __LINE__, nVkErr);
		return DRV_ERR;
	}

	nVkErr = VK_MSG_Create(MSGQ_SIZE_NORMAL, sizeof(WIFI_MSG_t),"WiFiMsgQue",(unsigned long*)&s_stWiFiInstance.ulMsg,VK_SUSPENDTYPE_FIFO);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Creating Wi-Fi VK_MSG_Create failed! (%d)\n", __FUNCTION__, __LINE__, nVkErr);
		return DRV_ERR;
	}

	nVkErr = VK_TASK_Create(P_WIFI_Task, WIFI_TASK_PRIORITY, WIFI_TASK_STACK_SIZE*100, "P_WIFI_Task", (void *)0, (unsigned long*)&s_stWiFiInstance.ulTask, FALSE);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Creating Wi-Fi VK_TASK_Create failed! (%d)\n", __FUNCTION__, __LINE__, nVkErr);
		return DRV_ERR;
	}

	for(ulLoop = 0; ulLoop < WIFI_MAX_NUM ; ulLoop++)
	{
		s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId			= 0xFF;
		s_stWiFiInstance.stWiFiInfo[ulLoop].nWPSPid 		= -1;
		s_stWiFiInstance.stWiFiInfo[ulLoop].eWPS 			= DI_WIFI_WPS_NONE;
		s_stWiFiInstance.stWiFiInfo[ulLoop].eUserSetting	= WIFI_STATUS_DISCONNECTED;
		s_stWiFiInstance.stWiFiInfo[ulLoop].eStatus 		= WIFI_STATUS_DISCONNECTED;
		s_stWiFiInstance.stWiFiInfo[ulLoop].eDongleStatus	= WIFI_DONGLE_EXTRACTED;
	}

	for(ulLoop = 0; ulLoop < DRV_CFG_GetNetMaxDevice(); ulLoop++)
	{
		if(s_stDevList[ulLoop].eDevType == DI_NETWORK_DEV_WIFI)
		{
			s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId = s_stDevList[ulLoop].ulDevId;
			VK_snprintf( (char *)s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, sizeof(s_stDevList[ulLoop].szName), (char *)s_stDevList[ulLoop].szName);
			PrintDebug("[%s] DevId : %d IfName : %s \n",__FUNCTION__,s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId,s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName);
			ulIndex++;
		}
	}

	dResult = DRV_WIRELESS_Hotplug_Init();
	if(dResult != DRV_OK)
	{
		PrintError("[%s][%d] Fail to init wireless hot-plug (%d) \n", __FUNCTION__, __LINE__, dResult);
		return DRV_ERR;
	}

	dResult = DRV_WIRELESS_RegisterCallback(P_WIFI_NotifyDImsg);
	if(dResult != DRV_OK)
	{
		PrintError("[%s][%d] Fail to register wireless hot-plug callback (%d) \n", __FUNCTION__, __LINE__, dResult);
		return DRV_ERR;
	}

	VK_TASK_Start(s_stWiFiInstance.ulTask);

#if defined(CONFIG_BCM43567)
	P_CheckPcieWifiConnection();
#endif

	return DRV_OK;
}


DI_ERR_CODE 	DI_WIFI_RegisterEventCallback(HUINT32 ulDevId, DI_WIFI_NOTIFY_EVT_e eEvent, pfnWIFI_EVT_CALLBACK pfnCallback)
{
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Fail to get SEM (%d) \n",__FUNCTION__, __LINE__, nVkErr);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s][%d] Invalid ulDevId(%d) !!\n", __FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[eEvent].pfnCallback = pfnCallback;
	if(eEvent == DI_WIFI_NOTIFY_EVT_INSERTED)
	{
		if(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus == WIFI_DONGLE_INSERTED)
		{
			DI_WIFI_CALLBACK_DATA_t stCallbackData;
			stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
			stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_INSERTED;
			stCallbackData.ulNumOfData = 0;
			stCallbackData.pvEventData = NULL;
			(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_INSERTED].pfnCallback)(&stCallbackData);
		}
	}
	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}


/* SCAN 도중에 SCAN 함수가 호출 되면....? */
/* 기준 SCAN 유지하면 됨.*/
DI_ERR_CODE		DI_WIFI_Scan(HUINT32 ulDevId)
{
	HINT32	nVkErr;
	WIFI_MSG_t tMsg;
	HUINT32 ulIndex = 0;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Fail to get SEM (%d) \n",__FUNCTION__, __LINE__, nVkErr);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s][%d] Invalid ulDevId(%d) !!\n", __FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus != WIFI_DONGLE_INSERTED)
	{
		PrintError("[%s][%d] device(%d) was removed !!\n", __FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTING)
	{
		tMsg.ulIndex = ulIndex;
		tMsg.eMsg = WIFI_EVT_DISCONNECT;
		PrintError("[%s][%d] device(%d) is connecting!! try to disconnect and re-scan !!\n",__FUNCTION__, __LINE__,ulDevId);
		nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &tMsg, sizeof(WIFI_MSG_t),0);
		if(nVkErr)
		{
			PrintError("[%s][%d] Fail to send message (%d)!!\n",__FUNCTION__, __LINE__, nVkErr);
			VK_SEM_Release(s_stWiFiInstance.ulSema);
			return DI_ERR_ERROR;
		}
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].bScanning == TRUE)
	{
		PrintError("[%s][%d] device(%d) is scanning !!\n",__FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	tMsg.ulIndex = ulIndex;
	tMsg.eMsg = WIFI_EVT_SCAN_START;
	nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &tMsg, sizeof(WIFI_MSG_t),0);
	if(nVkErr)
	{
		PrintError("[%s][%d] Fail to send message (%d)!!\n",__FUNCTION__, __LINE__, nVkErr);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}


DI_ERR_CODE		DI_WIFI_Connect(HUINT32 ulDevId, DI_WIFI_WPS_e eWPS, DI_WIFI_INFO_t *pstApInfo,HUINT32 ulWaitSeconds)
{
	int			vkMsg;
	WIFI_MSG_t WiFiMsg;
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	if(eWPS >= DI_WIFI_WPS_MAX)
	{
		PrintError("[%s][%d] Invaild WPS type (%d) \n",__FUNCTION__, __LINE__, eWPS);
		return DI_ERR_ERROR;
	}

	if( ((eWPS == DI_WIFI_WPS_NONE)||(eWPS == DI_WIFI_WPS_PIN)) && (pstApInfo == NULL) )
	{
		PrintError("[%s][%d] eWPS [%d] Invalid AP info \n",__FUNCTION__, __LINE__, eWPS);
		return DI_ERR_ERROR;
	}

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Fail to get SEM (%d) \n",__FUNCTION__, __LINE__, nVkErr);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s][%d] Invalid ulDevId(%d) !!\n", __FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus != WIFI_DONGLE_INSERTED)
	{
		PrintError("[%s][%d] device(%d) was removed !!\n", __FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eUserSetting == WIFI_STATUS_CONNECTED
		&& s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_DISCONNECTED )
	{
		PrintError("[%s][%d] device(%d) was already set to CONNECT \n",__FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].bScanning == TRUE)
	{
		PrintError("[%s][%d] device(%d) is scanning !!\n",__FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	s_stWiFiInstance.stWiFiInfo[ulIndex].eUserSetting = WIFI_STATUS_CONNECTED;

	if(eWPS == DI_WIFI_WPS_NONE)
	{
		dResult = P_WIFI_IsKeyValid(pstApInfo);
		if (dResult != DRV_OK)
		{
			PrintError("[%s][%d] Invalid key type (%d) \n", __FUNCTION__, __LINE__, dResult);
			return DI_ERR_INVALID_PARAM;
		}
	
		VK_memcpy(&s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP,pstApInfo,sizeof(DI_WIFI_INFO_t));

		WiFiMsg.ulIndex = ulIndex;
		WiFiMsg.eMsg = WIFI_EVT_CONNECT_START;
		vkMsg = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
		if(vkMsg)
		{
			PrintError("[%s][%d] Fail to send message (%d)!!\n",__FUNCTION__, __LINE__, vkMsg);
			VK_SEM_Release(s_stWiFiInstance.ulSema);
			return DI_ERR_ERROR;
		}
	}
	else if(eWPS == DI_WIFI_WPS_PBC)
	{
		if (s_eWifiCtlInterface == WIFI_CTL_WL)
		{
			P_WIFI_SetBrcmDown();
			P_WIFI_SetBrcmUp();
		}

		WiFiMsg.ulIndex = ulIndex;
		WiFiMsg.eMsg = WIFI_EVT_WPS_PBC_START;
		vkMsg = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
		if(vkMsg)
		{
			PrintError("[%s][%d] Fail to send message (%d)!!\n",__FUNCTION__, __LINE__, vkMsg);
			VK_SEM_Release(s_stWiFiInstance.ulSema);
			return DI_ERR_ERROR;
		}
	}
	else if(eWPS == DI_WIFI_WPS_PIN)
	{
		if (s_eWifiCtlInterface == WIFI_CTL_WL)
		{
			P_WIFI_SetBrcmDown();
			P_WIFI_SetBrcmUp();
		}

		VK_memcpy(&s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP,pstApInfo,sizeof(DI_WIFI_INFO_t));

		WiFiMsg.ulIndex = ulIndex;
		WiFiMsg.eMsg = WIFI_EVT_WPS_PIN_START;

		vkMsg = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
		if(vkMsg)
		{
			PrintError("[%s][%d] Fail to send message (%d)!!\n",__FUNCTION__, __LINE__, vkMsg);
			VK_SEM_Release(s_stWiFiInstance.ulSema);
			return DI_ERR_ERROR;
		}
	}

	ulWaitSeconds = ulWaitSeconds * 1000;

	nVkErr = VK_TIMER_EventAfter(ulWaitSeconds,P_WIFI_DisConnectCallback, (void *)&ulIndex,sizeof(HUINT32), (unsigned long*)&s_stWiFiInstance.ulDisConnectTimerId);
	if(nVkErr)
	{
		PrintError("[%s][%d] Fail to make event (%d)!!\n",__FUNCTION__, __LINE__, nVkErr);
	}

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}


DI_ERR_CODE		DI_WIFI_Disconnect(HUINT32 ulDevId)
{
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;
	WIFI_MSG_t 	stWiFiMsg;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Fail to get SEM (%d) \n",__FUNCTION__, __LINE__, nVkErr);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s][%d] Invalid ulDevId(%d) !!\n", __FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eUserSetting == WIFI_STATUS_DISCONNECTED)
	{
		PrintError("[%s][%d] device(%d) was already set to DISCONNECT \n",__FUNCTION__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}

	stWiFiMsg.ulIndex = ulIndex;
	stWiFiMsg.eMsg = WIFI_EVT_DISCONNECT;
	
	nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &stWiFiMsg, sizeof(WIFI_MSG_t),0);
	if( nVkErr != 0 )
	{
		PrintError("[%s][%d] Fail to send message (%d)!!\n",__FUNCTION__, __LINE__, nVkErr);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}
	if(s_stWiFiInstance.ulDisConnectTimerId)
	{
		VK_TIMER_Cancel(s_stWiFiInstance.ulDisConnectTimerId);
		s_stWiFiInstance.ulDisConnectTimerId = 0;
	}
	s_stWiFiInstance.stWiFiInfo[ulIndex].eUserSetting = WIFI_STATUS_DISCONNECTED;

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}


/*
	ESSID,OpMode,MAC,WaveStrength,BitRate,SecureType,AuthenType,EncryptType,KeyIndex,Id,Key
*/
DI_ERR_CODE		DI_WIFI_GetAPInfo(HUINT32 ulDevId, DI_WIFI_INFO_t *pstApInfo)
{
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;

	NETAPP_RETCODE nResult = NETAPP_FAILURE;
	NETAPP_WIFI_AP_INFO stApInfo;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Fail to get SEM (%d) \n",__FUNCTION__, __LINE__, nVkErr);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s][%d] Invalid ulDevId(%d) !!\n", __FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}
	/* Connect or Disconnect ? */
	if(!((s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTED)||(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_RECONNECTING)))
	{
		PrintError("[%s][%d] Disconnect !!\n",__FUNCTION__, __LINE__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	/* Copy User Setting */
	VK_memcpy(pstApInfo,&s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP,sizeof(DI_WIFI_INFO_t));

	VK_MEM_Memset(&stApInfo, 0x0, sizeof(NETAPP_WIFI_AP_INFO));

	/* Get rest of AP info */
	nResult = NetAppWiFiGetConnectedApInfo(hWifiNetapp, &stApInfo);
	if (nResult != NETAPP_SUCCESS)
	{
		PrintError("[%s][%d] Fail to get connected AP info (%d) \n", __FUNCTION__, __LINE__, nResult);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_OK;
	}

	P_WIFI_ConvertApInfo(pstApInfo, stApInfo);

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;

}


DI_ERR_CODE		DI_WIFI_GetStrength(HUINT32 ulDevId, HINT32 *pnStrength)
{
	HUINT32 ulIndex = 0;
	HINT32	nVkErr;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Fail to get SEM (%d) \n",__FUNCTION__, __LINE__, nVkErr);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s][%d] Invalid ulDevId(%d) !!\n", __FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	/* Connect or Disconnect ? */
	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_CONNECTED)
	{
		PrintError("[%s][%d] Disconnected !!\n",__FUNCTION__, __LINE__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	/* Get Strength */
	P_WIFI_GetStrength(ulIndex, pnStrength);

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}


DI_ERR_CODE		DI_WIFI_GetBitRate(HUINT32 ulDevId, HINT32 *pnBitRate)
{
	HUINT32 ulIndex = 0;
	HINT32	nVkErr;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Fail to get SEM (%d) \n",__FUNCTION__, __LINE__, nVkErr);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s][%d] Invalid ulDevId(%d) !!\n", __FUNCTION__, __LINE__, ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	/* Connect or Disconnect ? */
	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_CONNECTED)
	{
		PrintError("[%s][%d] Disconnected !!\n",__FUNCTION__, __LINE__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	/* bit rate */
	P_WIFI_GetRate(ulIndex, pnBitRate);

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}


DI_ERR_CODE		DI_WIFI_GetRSSI(HUINT32 ulDevId, DI_WIFI_RSSI_INFO_t *pRssiInfo)
{
	/* Used for only Factory test SW */
	DI_ERR_CODE diResult = DI_ERR_ERROR;
	DRV_Error drvError = DRV_ERR;
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Fail to get SEM (%d) \n",__FUNCTION__, __LINE__, nVkErr);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s][%d] Invalid ulDevId(%d) !!\n", __FUNCTION__, __LINE__, ulDevId);
		diResult = DI_ERR_NO_RESOURCE;
		goto ret;
	}

	/* Connect or Disconnect ? */
	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_CONNECTED)
	{
		PrintError("[%s][%d] Disconnected !!\n",__FUNCTION__, __LINE__);
		diResult = DI_ERR_NO_RESOURCE;
		goto ret;
	}

	VK_MEM_Memset(pRssiInfo, 0x0, sizeof(DI_WIFI_RSSI_INFO_t));

	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		drvError = P_WIFI_NETAPP_GetRssi(ulIndex, pRssiInfo);
		if(drvError == DRV_ERR)
		{
			diResult = DI_ERR_ERROR;
		}
		else
		{
			diResult = DI_ERR_OK;
		}
		goto ret;
	}
	else
	{
		PrintError("[%s][%d] Error : s_eWifiCtlInterface (%d) \n", __FUNCTION__, __LINE__, s_eWifiCtlInterface);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

ret:
	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return diResult;
}


DI_ERR_CODE		DI_WIFI_GetDrvInfo(HUINT32 ulDevId, DI_WIFI_DRV_INFO_t * pstWifiDrvInfo)
{
	UNUSED(ulDevId);
	UNUSED(pstWifiDrvInfo);

	return DI_ERR_OK;
}


DI_ERR_CODE		DI_WIFI_ResetModule(HUINT32 ulDevId)
{
	UNUSED(ulDevId);

	return DI_ERR_OK;
}


DI_ERR_CODE		DI_WIFI_SetWakeOnWLAN(HUINT32 ulDevId, DI_WIFI_WOWL_SETTINGS_t *pstWowlSettings)
{
	UNUSED(ulDevId);
	UNUSED(pstWowlSettings);

	PrintError("[%s][%d] Currently not supported !! \n", __FUNCTION__, __LINE__);
	
	return DI_ERR_NOT_SUPPORTED;
}


/* end of file */


