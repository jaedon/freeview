#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/input.h>
#include "htype.h"
#include "drv_err.h"
#include "taskdef.h"
#include "vkernel.h"
#include "di_bt.h"

#define BUILDCFG
#include "bt_target.h"
#include "bsa_api.h"
#include "app_xml_utils.h"
#include "app_utils.h"
#include "app_services.h"
#include "app_hh.h"
#include "app_hh_db.h"

#if defined(CONFIG_SHARED_COMPILE)
#include <ctype.h>	//for "isxdigit"
#include <stdlib.h>	//for "strtol"
#include "nexus_platform_client.h"
#include "nexus_audio_capture.h"
#else
#include "di_uart.h"
#include "drv_network.h"
#include "drv_bt.h"
#include "drv_nvram.h"
#include "nvram_map.h"
#include "drv_wireless_hotplug.h"
#include "di_audio.h"
#include "drv_audio.h"
#endif


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
//#define DI_DEBUG
#if defined(CONFIG_SHARED_COMPILE)
#ifdef DI_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#endif
#else
#ifdef DI_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#endif
#endif

#define UNUSED(x) (void)(x)

#define SYSTEM_CMD_MAX							512
#define BT_MSG_SIZE 							MSGQ_SIZE_NORMAL
#define BT_OPERATION_DELAY						(500*3)		/* 1.5 sec */

#define BT_CONNECTION_TIMEOUT					8000 /* 8 secs */
#define BT_RETRY_DELAY							500  /* 500 msecs */

#define BT_XML_CONFIG_FILE_PATH					"/var/lib/humaxtv_user/bt_config.xml"	/* Local BT Configuration */

/* Default path to UIPC */
#define BT_DEFAULT_UIPC_PATH					"/tmp/"

/* Default local BT DevAddress */
#define BT_DEFAULT_BT_ADDR						{0xBE, 0xEF, 0xBE, 0xEF, 0x00, 0x01}

/* Default local BT DevName */
//#define BT_DEFAULT_BT_NAME						"HMS-1000S"/* HUMAX_STB_Bluetooth */
#define BT_DEFAULT_BT_NAME		CONFIG_PRODUCT_NAME

/* Default COD SetTopBox (Major Service = MISCC) ( Major DevClass = Audio/Vidoe) (Minor = STB) */
#define BT_DEFAULT_BT_COD						{0x00, 0x04, 0x24}

/* Default Pin Code */
#define BT_DEFAULT_PINCODE						"0000"
#define BT_DEFAULT_PINLENGTH					4

/* Number of simultaneous A2DP connections supported */
#define BT_AV_DEVICE_MAX 1		/* Available upto 2  */

/* Default : Target(Receivce command) and Controller(Send Command) enabled */
#define BT_AV_FEAT_MASK (BSA_AV_FEAT_RCTG | BSA_AV_FEAT_RCCT)

#define BT_AV_USE_RC TRUE
#ifndef BSA_AV_UIPC_BUFFER_SIZE
#define BSA_AV_UIPC_BUFFER_SIZE 20001
#endif

typedef enum
{
	BT_STATUS_NOT_INIT=0,
	BT_STATUS_INIT,
	BT_STATUS_ENABLE,	/* BT_STATUS_READY,*/
	BT_STATUS_SCANNING,
	BT_STATUS_SCANNED,		/* Not necessary */
	BT_STATUS_CONNECTING,
	BT_STATUS_CONNECTED,	/* Not necessary */
	BT_STATUS_DISCONNECT	/* Not necessary */
} DI_BT_STATUS_e;

typedef enum
{
	BT_MSG_INSERT,
	BT_MSG_ENABLE,
	BT_MSG_DISABLE,
	BT_MSG_EXTRACT,
	BT_MSG_SCAN_START,
	BT_MSG_SCAN_END,
	BT_MSG_CONNECT_START,
	BT_MSG_CONNECT_OK,
	BT_MSG_CONNECT_FAIL,
	BT_MSG_CONNECT_CHECK,
	BT_MSG_DISCONNECT,
	BT_MSG_REMOVE,
	BT_MSG_MAX
} BT_EVENT_e;

typedef struct BT_MSG
{
	BT_EVENT_e	eMsg;
	HUINT32		ulParam1;
} BT_MSG_t;

typedef struct
{
	pfnBT_EVT_CALLBACK pfnCallback;
} BT_CALLBACK_INFO_t;

typedef struct
{
	tAPP_XML_REM_DEVICE devs[BT_NB_PAIR_DEVICES];
	HUINT32	xmlCnt;
}BT_CON_DEV_INFO_t;

typedef struct
{
	HBOOL	bInUse;
	HUINT8	ucBdAddr[BT_NB_BDADDR_LEN];
}BT_LINK_DEVICE_t;

typedef struct
{
	BT_LINK_DEVICE_t devs[BT_NB_PAIR_DEVICES];	/* Maximum number of ACL links currently established */
	HUINT32	CurLinkCnt;							/* Blutooth Spec = 7 , Broadcom = 10 */
}BT_LINK_DEV_INFO_t;

typedef struct
{
	tBSA_DISC_DEV devs[BT_NB_DISC_DEVICES];
	HUINT32	DiscoveryCnt;
}BT_DISC_DEV_INFO_t;

typedef struct
{
	HBOOL	bEnable;	
	HBOOL	bDiscoverable;
	HBOOL	bConnectable;
	HUINT8 ucBdAddr[BT_NB_BDADDR_LEN];
	HCHAR ucName[BT_NB_NAME_LEN];
	HUINT8 ucCOD[BT_NB_CLASS_LEN];	/* Class Of Device*/
	HCHAR cRootPath[BT_NB_ROOT_PATH_LEN];
	HUINT8	ucPinCode[BT_NB_PINCODE_LEN];
	HCHAR	cPinLength;
	tBSA_SEC_IO_CAP io_cap;	/* Simple Paring Accept/Reject*/
}BT_DEV_SETTING_t;

typedef struct
{
	HUINT32				ulMsg;
	HUINT32				ulTask;
	HUINT32				ulSema;
	BT_CON_DEV_INFO_t	stPairDevInfo;
	BT_LINK_DEV_INFO_t	stLinkDevInfo;
	BT_DISC_DEV_INFO_t	discDevInfo;
	BT_DEV_SETTING_t	stHostSettings;
	BT_CALLBACK_INFO_t 	stCallbackInfo[DI_BT_NOTIFY_EVT_MAX];
} DI_BT_INSTANCE_t;


typedef struct
{
	HBOOL bInUse;
	HBOOL bIsStreamActive;
	tBSA_AV_HNDL handle;
	DI_BT_DEV_INFO_t stAvDevInfo;
}BT_AV_DEVICE_t;

typedef struct
{
	BT_AV_DEVICE_t stAvTable[BT_AV_DEVICE_MAX];
	tUIPC_CH_ID stUipcChannel;
	HUINT32 ulAutoPairTimer;
}BT_AV_t;

typedef struct
{
	HUINT8		ucBsaKey;
	HUINT8		ucLinuxKey;
}BT_AV_INPUT_KEY_TABLE_t;

extern tAPP_XML_REM_DEVICE app_xml_remote_devices_db[APP_MAX_NB_REMOTE_STORED_DEVICES];

static unsigned long ulBtUSBHotplugTaskId;
static DI_BT_INSTANCE_t s_stBtInstance;
static DI_BT_STATUS_e s_eBtDiStatus = 	BT_STATUS_NOT_INIT;

static HBOOL s_bBsaServer = FALSE;
static HBOOL s_bMgtOpen = FALSE;

static DI_BT_DEV_LIST_t s_sCbEvtDevInfo;	/* DI DEVICE LIST for Management of ulDevId */
static BT_AV_t s_stBtAvDevInfo;
#if defined(BCM20702)
static BT_AV_INPUT_KEY_TABLE_t KeyLookupTable[] = {
    { BSA_AV_ID_SELECT,     KEY_ENTER},
    { BSA_AV_ID_UP,         KEY_UP},
    { BSA_AV_ID_DOWN,       KEY_DOWN},
    { BSA_AV_ID_LEFT,       KEY_LEFT},
    { BSA_AV_ID_RIGHT,      KEY_RIGHT},
    { BSA_AV_ID_RIGHT_UP,   KEY_RIGHT},
    { BSA_AV_ID_RIGHT_DOWN, KEY_RIGHT},
    { BSA_AV_ID_LEFT_UP,    KEY_LEFT},
    { BSA_AV_ID_LEFT_DOWN,  KEY_LEFT},
    { BSA_AV_ID_ROOT_MENU,  KEY_MENU},
    { BSA_AV_ID_SETUP_MENU, KEY_SETUP},
    //{ BSA_AV_ID_CONT_MENU,  KEY_},
    //{ BSA_AV_ID_FAV_MENU, KEY_},
    { BSA_AV_ID_EXIT,       KEY_ESC},
    { BSA_AV_ID_0,          KEY_0},
    { BSA_AV_ID_1,          KEY_1},
    { BSA_AV_ID_2,          KEY_2},
    { BSA_AV_ID_3,          KEY_3},
    { BSA_AV_ID_4,          KEY_4},
    { BSA_AV_ID_5,          KEY_5},
    { BSA_AV_ID_6,          KEY_6},
    { BSA_AV_ID_7,          KEY_7},
    { BSA_AV_ID_8,          KEY_8},
    { BSA_AV_ID_9,          KEY_9},
    { BSA_AV_ID_DOT,        KEY_DOT},
    { BSA_AV_ID_ENTER,      KEY_ENTER},
    { BSA_AV_ID_CLEAR,      KEY_BACK},
    { BSA_AV_ID_CHAN_UP,    KEY_UP},
    { BSA_AV_ID_CHAN_DOWN,  KEY_DOWN},
    { BSA_AV_ID_PREV_CHAN,  KEY_BACK},
    //{ BSA_AV_ID_SOUND_SEL,  KEY_},
    //{ BSA_AV_ID_INPUT_SEL,  KEY_},
    //{ BSA_AV_ID_DISP_INFO,  KEY_},
    //{ BSA_AV_ID_HELP,       KEY_},
    //{ BSA_AV_ID_PAGE_UP, KEY_},
    //{ BSA_AV_ID_PAGE_DOWN, KEY_},
    { BSA_AV_ID_POWER,      KEY_POWER},
    { BSA_AV_ID_VOL_UP,     KEY_VOLUMEUP},
    { BSA_AV_ID_VOL_DOWN,   KEY_VOLUMEDOWN},
    { BSA_AV_ID_MUTE,       KEY_MUTE},
    { BSA_AV_ID_PLAY,       KEY_PLAY},
    { BSA_AV_ID_STOP,       KEY_STOP},
    { BSA_AV_ID_PAUSE,      KEY_PAUSE},
    { BSA_AV_ID_RECORD,     KEY_RECORD},
    { BSA_AV_ID_REWIND,     KEY_REWIND},
    { BSA_AV_ID_FAST_FOR,   KEY_FASTFORWARD},
    { BSA_AV_ID_EJECT,      KEY_EJECTCD},
    { BSA_AV_ID_FORWARD,    KEY_FORWARD},
    { BSA_AV_ID_BACKWARD,   KEY_BACK},
    //{ BSA_AV_ID_ANGLE,      KEY_},
    //{ BSA_AV_ID_SUBPICT,    KEY_},
    { BSA_AV_ID_F1,         KEY_F1},
    { BSA_AV_ID_F2,         KEY_F2},
    { BSA_AV_ID_F3,         KEY_F3},
    { BSA_AV_ID_F4,         KEY_F4},
    { BSA_AV_ID_F5,         KEY_F5}
};
#else
static BT_AV_INPUT_KEY_TABLE_t KeyLookupTable[] = {
    { BSA_AV_RC_SELECT,     KEY_ENTER},
    { BSA_AV_RC_UP,         KEY_UP},
    { BSA_AV_RC_DOWN,       KEY_DOWN},
    { BSA_AV_RC_LEFT,       KEY_LEFT},
    { BSA_AV_RC_RIGHT,      KEY_RIGHT},
    { BSA_AV_RC_RIGHT_UP,   KEY_RIGHT},
    { BSA_AV_RC_RIGHT_DOWN, KEY_RIGHT},
    { BSA_AV_RC_LEFT_UP,    KEY_LEFT},
    { BSA_AV_RC_LEFT_DOWN,  KEY_LEFT},
    { BSA_AV_RC_ROOT_MENU,  KEY_MENU},
    { BSA_AV_RC_SETUP_MENU, KEY_SETUP},
    { BSA_AV_RC_EXIT,       KEY_ESC},
    { BSA_AV_RC_0,          KEY_0},
    { BSA_AV_RC_1,          KEY_1},
    { BSA_AV_RC_2,          KEY_2},
    { BSA_AV_RC_3,          KEY_3},
    { BSA_AV_RC_4,          KEY_4},
    { BSA_AV_RC_5,          KEY_5},
    { BSA_AV_RC_6,          KEY_6},
    { BSA_AV_RC_7,          KEY_7},
    { BSA_AV_RC_8,          KEY_8},
    { BSA_AV_RC_9,          KEY_9},
    { BSA_AV_RC_DOT,        KEY_DOT},
    { BSA_AV_RC_ENTER,      KEY_ENTER},
    { BSA_AV_RC_CLEAR,      KEY_BACK},
    { BSA_AV_RC_CHAN_UP,    KEY_UP},
    { BSA_AV_RC_CHAN_DOWN,  KEY_DOWN},
//    { BSA_AV_RC_PREV_CHAN,  KEY_BACK},
    { BSA_AV_RC_POWER,      KEY_POWER},
    { BSA_AV_RC_VOL_UP,     KEY_VOLUMEUP},
    { BSA_AV_RC_VOL_DOWN,   KEY_VOLUMEDOWN},
    { BSA_AV_RC_MUTE,       KEY_MUTE},
    { BSA_AV_RC_PLAY,       KEY_PLAY},
    { BSA_AV_RC_STOP,       KEY_STOP},
    { BSA_AV_RC_PAUSE,      KEY_PAUSE},
    { BSA_AV_RC_RECORD,     KEY_RECORD},
    { BSA_AV_RC_REWIND,     KEY_REWIND},
    { BSA_AV_RC_FAST_FOR,   KEY_FASTFORWARD},
    { BSA_AV_RC_EJECT,      KEY_EJECTCD},
    { BSA_AV_RC_FORWARD,    KEY_FORWARD},
    { BSA_AV_RC_BACKWARD,   KEY_BACK},
    { BSA_AV_RC_F1,         KEY_F1},
    { BSA_AV_RC_F2,         KEY_F2},
    { BSA_AV_RC_F3,         KEY_F3},
    { BSA_AV_RC_F4,         KEY_F4},
    { BSA_AV_RC_F5,         KEY_F5}
};
#endif

static NEXUS_AudioCaptureHandle    hCapture = NULL;

char services_string[BSA_MAX_SERVICE_ID * 7];

/*******************************************************************/
/******************** Functions Prototype **************************/
/*******************************************************************/
static DRV_Error P_BT_Open(void);
static DRV_Error P_BT_Close(void);
static void P_BT_AV_Capture_Callback(void *pParam, int param);
static void P_BT_AV_SampleRate_Callback(void *context, int param);
static DRV_Error P_BT_AV_Init(void);
static DRV_Error P_BT_AV_Deinit(void);
static DRV_Error P_BT_AV_Connect(DI_BT_DEV_INFO_t *pstAvDevInfo);
static DRV_Error P_BT_AV_Disconnect(DI_BT_DEV_INFO_t *pstAvDevInfo);
static DRV_Error P_BT_AV_Start(void);
static DRV_Error P_BT_AV_Stop(void);
static DRV_Error P_BT_AV_SendAudioBuffer(tUIPC_CH_ID stUipcChannel, void *pvBuffer, HUINT32 ulLength);

static DRV_Error P_BT_HID_Init(void);
static DRV_Error P_BT_HID_Deinit(void);
static DRV_Error P_BT_HID_Connect(DI_BT_DEV_INFO_t *pstHidDevInfo);
static DRV_Error P_BT_HID_Disconnect(DI_BT_DEV_INFO_t *pstHidDevInfo);
static HBOOL P_BT_HID_Callback(tBSA_HH_EVT event, tBSA_HH_MSG *p_data);
static HBOOL P_BT_HID_UIPC_Callback(BT_HDR *p_msg);
static HBOOL P_BT_Dev_IsEnabled(void);
static void P_BT_USBDev_HotplugParse(char *pBuffer);
static void P_BT_USBDev_HotplugTask(void *pvParam);

static DRV_Error P_BT_NEXUS_AudioCapture_GetHandle(HUINT32 ulDevId);
static void P_BT_NEXUS_AudioCapture_Start(void);

/*******************************************************************/
/************************ For Debuging *****************************/
/*******************************************************************/
static int P_BT_ConvertHex2Str(HUINT8 *pucHexBdAddr, HINT8 *pszStrBdAddr)
{
	return snprintf(pszStrBdAddr, 17+1,"%02X:%02X:%02X:%02X:%02X:%02X",
			pucHexBdAddr[0], pucHexBdAddr[1],pucHexBdAddr[2],pucHexBdAddr[3],pucHexBdAddr[4],pucHexBdAddr[5]);
}
#if defined(CONFIG_SHARED_COMPILE)
static int P_BT_IsValidBdAddr(const HINT8 *pszStrBdAddr)
{
	if (!pszStrBdAddr)
		return -1;

	if (strlen(pszStrBdAddr) != 17)
		return -1;

	while (*pszStrBdAddr) {
		if (!isxdigit(*pszStrBdAddr++))
			return -1;
		if (!isxdigit(*pszStrBdAddr++))
			return -1;
		if (*pszStrBdAddr == 0)
			break;
		if (*pszStrBdAddr++ != ':')
			return -1;
	}
	return 0;
}

static int P_BT_ConvertStr2Hex(HINT8 *pszStrBdAddr, HUINT8 *pucHexBdAddr)
{
	int i;

	if (P_BT_IsValidBdAddr(pszStrBdAddr) < 0) {
		VK_MEM_Memset(pucHexBdAddr, 0x0,BT_NB_BDADDR_LEN);
		return -1;
	}

	for (i = 0; i < BT_NB_BDADDR_LEN; i++, pszStrBdAddr += 3)
	{
		pucHexBdAddr[i] = strtol(pszStrBdAddr, NULL, 16);
	}
	return 0;
}
#endif
#if defined(DI_DEBUG)
#define ENUM_TO_STR(enumVal) 			case enumVal: return #enumVal;
static HUINT8 *P_BT_GetEvtText(DI_BT_NOTIFY_EVT_e etEvent)
{
	switch (etEvent)
	{
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_INSERTED);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_EXTRACTED);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_ENABLE);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_DISABLE);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_SCAN_SUCCESS);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_SCAN_FAIL);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_AUTHEN_SUCCESS);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_AUTHEN_FAIL);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_CONNECT_SUCCESS);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_CONNECT_FAIL);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_REMOTE_CMD);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_PASSKEY);
		ENUM_TO_STR(DI_BT_NOTIFY_EVT_MAX);
		default :
			break;
	}

	return NULL;
}

static void P_BT_PrintDeviceInfo(HUINT32 ulDevId, DI_BT_DEV_INFO_t *pstDevInfo)		/* DI_DEBUG */	
{
	HINT8 szBdAddr[20];
	HUINT8 szCOD[10];
	HUINT8 szStatus[15];

	P_BT_ConvertHex2Str(pstDevInfo->ucBdAddr, szBdAddr);
	sprintf(szCOD, "%02X/%02X/%02X",
			pstDevInfo->ucClass[0], pstDevInfo->ucClass[1], pstDevInfo->ucClass[2]);
	sprintf(szStatus,"%s%s%s",
			(pstDevInfo->etDevStatus == DI_BT_DEV_NOT_PAIRED) ? "Not Paired":"",
			(pstDevInfo->etDevStatus == DI_BT_DEV_CONNECTED) ? "Connected" :"",
			(pstDevInfo->etDevStatus == DI_BT_DEV_DISCONNECTED) ? "Not Connected" : "");
	PrintDebug("  %2d | %-20s | %17s | %8s(%10s) | %13s | %s %s",
			ulDevId,
			pstDevInfo->ucName,
			szBdAddr,
			szCOD,
			app_get_cod_string(pstDevInfo->ucClass),
			szStatus,
			app_service_mask_to_string(pstDevInfo->etAvailableProfile),
			(pstDevInfo->etDevStatus == DI_BT_DEV_NOT_PAIRED) ? "\n":"<- used before\n");
}


static void P_BT_PrintDeviceList(DI_BT_DEV_LIST_t *pstDevList)		/* DI_DEBUG */
{
	HUINT32 i;

	PrintDebug("[DI_BT] Device List[%d]\n", pstDevList->ulNumOfData);

	PrintDebug("  %2s | %-20s | %17s | %21s | %13s | %s\n",
			"ID",
			"Name",
			"BD Address",
			"COD(Type)",
			"Status",
			"Profiles");

	for (i = 0 ; i < pstDevList->ulNumOfData; i++)
	{
		P_BT_PrintDeviceInfo(i, &pstDevList->stBtDevList[i]);
	}

}
#endif
/*******************************************************************/
/********************** Local Functions ****************************/
/*******************************************************************/

static void P_BT_PostMsgToMw(DI_BT_CALLBACK_DATA_t *pstCbData)
{
	if(s_stBtInstance.stCallbackInfo[pstCbData->eEvent].pfnCallback != NULL)
	{
		(*s_stBtInstance.stCallbackInfo[pstCbData->eEvent].pfnCallback)(pstCbData);
	}
	else
	{
		PrintDebug("[%s] BT Callback is NULL [%s] ! \n",__FUNCTION__, P_BT_GetEvtText(pstCbData->eEvent));
	}

}

#if defined(CONFIG_SHARED_COMPILE)
static DRV_Error P_BT_Get_BdAddrFromCmdline(unsigned char *pucMacAddr)
{
	DRV_Error drvResult = DRV_OK;

	FILE	*fp;
	char	szBuf[256] = {0,};
	char	szBtMacString[30] = {0,};
	fp = fopen("/proc/cmdline", "r");
	if (!fp)
	{
		PrintError("[%s] CAN'T OPEN DEVICE FILES : %s\n", __FUNCTION__, strerror(errno));
		drvResult = DRV_ERR;
		goto ret;
	}

	while(fgets(szBuf, sizeof(szBuf), fp))
	{
		char	*p;
		szBuf[sizeof(szBuf)-1] = 0;
		p = strstr(szBuf, "btmacaddr=");	/* btmacaddr */
		if (p)
		{
			p = p + strlen("btmacaddr=");
			sscanf(p, "%s", szBtMacString);
			szBtMacString[17] ='\0';
			PrintError("[%s] Bd Address from /proc/cmdline: %s\n", __FUNCTION__, szBtMacString);
			P_BT_ConvertStr2Hex(szBtMacString, pucMacAddr);
			break;
		}
	}
	fclose(fp);

	
ret :
	return drvResult;
}

static DRV_Error P_BT_Get_BdAddrFromTempNvramDir(unsigned char *pucMacAddr)
{
	DRV_Error drvResult = DRV_OK;

	FILE	*fp;
	unsigned char	ucBtMacAddr[BT_NB_BDADDR_LEN] = {0,};
	int i;

	fp = fopen("/tmp/nvram/btmac", "r");
	if (!fp)
	{
		PrintError("[%s] fopen Error : %s!\n", __FUNCTION__, strerror(errno));
		return DRV_ERR;
	}
	else
	{
		for ( i= 0 ; i < BT_NB_BDADDR_LEN ; i++ )
		{
			ucBtMacAddr[i] = (unsigned char) fgetc(fp);
			PrintDebug("[%s] %d %02x\n",__FUNCTION__, i, ucBtMacAddr[i]);
		}
		fclose(fp);
	}
	PrintDebug("[%s] BD Addr %02x:%02x:%02x:%02x:%02x:%02x\n",__FUNCTION__,
			ucBtMacAddr[0], ucBtMacAddr[1], ucBtMacAddr[2],
			ucBtMacAddr[3], ucBtMacAddr[4], ucBtMacAddr[5]);
	VK_MEM_Memcpy(pucMacAddr, ucBtMacAddr, sizeof(ucBtMacAddr));

	return drvResult;
}
#else
static DRV_Error P_BT_Get_BdAddrFromNvram(unsigned char *pucMacAddr)
{
	DRV_Error drvResult = DRV_OK;
	
	drvResult = DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RO, (HUINT8 *)BTMAC_ADDR, 0, pucMacAddr, BT_NB_BDADDR_LEN);
	if( drvResult != DRV_OK )
	{
		PrintError("[%s] Error: DRV_NVRAM_Read() \n", __FUNCTION__);
		drvResult = DRV_ERR;
		goto ret;
	}
	
ret :
	return drvResult;
}

static DRV_Error P_BT_Set_BdAddrToNvram(unsigned char *pucMacAddr)
{
	DRV_Error drvResult = DRV_OK;
	
	drvResult = DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, (HUINT8 *)BTMAC_ADDR, 0, pucMacAddr, BT_NB_BDADDR_LEN);
	if( drvResult != DRV_OK )
	{
		PrintError("[%s] Error: DRV_NVRAM_Write() \n", __FUNCTION__);
		drvResult = DRV_ERR;
		goto ret;
	}
	
ret :
	return drvResult;
}
#endif

static DRV_Error P_BT_MGT_Read_Config(BT_DEV_SETTING_t *pstHostSettings)
{
	int status;
	tAPP_XML_CONFIG app_xml_config;

	VK_MEM_Memset(&app_xml_config, 0x0, sizeof(tAPP_XML_CONFIG));

	status = app_xml_read_cfg(BT_XML_CONFIG_FILE_PATH, &app_xml_config);
	if (status < 0)
	{
		PrintError("app_xml_read_cfg failed:%d", status);
		return DRV_ERR;
	}
	else
	{
		pstHostSettings->bEnable = app_xml_config.enable;
		pstHostSettings->bDiscoverable = app_xml_config.discoverable;
		pstHostSettings->bConnectable = app_xml_config.connectable;
		bdcpy(pstHostSettings->ucBdAddr, app_xml_config.bd_addr);  

		strncpy((char *)pstHostSettings->ucName, (char *)app_xml_config.name, sizeof(app_xml_config.name));
		pstHostSettings->ucName[sizeof(pstHostSettings->ucName) - 1] = '\0';

		VK_MEM_Memcpy(pstHostSettings->ucCOD, app_xml_config.class_of_device, sizeof(app_xml_config.class_of_device));
		
		strncpy(pstHostSettings->cRootPath, app_xml_config.root_path, sizeof(app_xml_config.root_path));
		pstHostSettings->cRootPath[sizeof(pstHostSettings->cRootPath) - 1] = '\0';

		VK_MEM_Memcpy(pstHostSettings->ucPinCode, app_xml_config.pin_code, app_xml_config.pin_len);
		
		pstHostSettings->cPinLength = app_xml_config.pin_len;
		pstHostSettings->io_cap = app_xml_config.io_cap;
		
		PrintDebug("Enable:%d\n", app_xml_config.enable);
		PrintDebug("Discoverable:%d\n", app_xml_config.discoverable);
		PrintDebug("Connectable:%d\n", app_xml_config.connectable);
		PrintDebug("Name:%s\n", app_xml_config.name);
		PrintDebug("Bdaddr %02x:%02x:%02x:%02x:%02x:%02x\n",
				app_xml_config.bd_addr[0], app_xml_config.bd_addr[1],
				app_xml_config.bd_addr[2], app_xml_config.bd_addr[3],
				app_xml_config.bd_addr[4], app_xml_config.bd_addr[5]);
		PrintDebug("ClassOfDevice:%02x:%02x:%02x => %s\n", app_xml_config.class_of_device[0],
				app_xml_config.class_of_device[1], app_xml_config.class_of_device[2],
				app_get_cod_string(app_xml_config.class_of_device));
		PrintDebug("RootPath:%s\n", app_xml_config.root_path);
		PrintDebug("Default PIN (%d):%s\n", app_xml_config.pin_len, app_xml_config.pin_code);
		PrintDebug("I/O Capability:%d\n", app_xml_config.io_cap);
	}
	return DRV_OK;
}

static DRV_Error P_BT_MGT_Write_Config(BT_DEV_SETTING_t *pstHostSettings)
{
    int status;

	tAPP_XML_CONFIG xml_config;
	
	VK_MEM_Memset(&xml_config,0x0,sizeof(tAPP_XML_CONFIG));

	xml_config.enable= pstHostSettings->bEnable;
	xml_config.discoverable= pstHostSettings->bDiscoverable;
	xml_config.connectable = pstHostSettings->bConnectable;
	bdcpy(xml_config.bd_addr, pstHostSettings->ucBdAddr); 

	strncpy((char *)xml_config.name, (char *)pstHostSettings->ucName, sizeof(pstHostSettings->ucName));
	xml_config.name[sizeof(xml_config.name) - 1] = '\0';

	VK_MEM_Memcpy(xml_config.class_of_device, pstHostSettings->ucCOD, sizeof(pstHostSettings->ucCOD));

	strncpy(xml_config.root_path, pstHostSettings->cRootPath, sizeof(pstHostSettings->cRootPath));		
	xml_config.root_path[sizeof(xml_config.root_path) - 1] = '\0';	
	
	VK_MEM_Memcpy(xml_config.pin_code, pstHostSettings->ucPinCode, pstHostSettings->cPinLength);
	
	xml_config.pin_len=pstHostSettings->cPinLength;
	xml_config.io_cap=pstHostSettings->io_cap;

	status = app_xml_write_cfg(BT_XML_CONFIG_FILE_PATH, &xml_config);
	if (status < 0)
	{
		PrintError("app_xml_write_cfg failed: %d", status);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("\tEnable:%d\n", xml_config.enable);
		PrintDebug("\tDiscoverable:%d\n", xml_config.discoverable);
		PrintDebug("\tConnectable:%d\n", xml_config.connectable);
		PrintDebug("\tName:%s\n", xml_config.name);
		PrintDebug ("\tBdaddr %02x:%02x:%02x:%02x:%02x:%02x\n",
				xml_config.bd_addr[0], xml_config.bd_addr[1],
				xml_config.bd_addr[2], xml_config.bd_addr[3],
				xml_config.bd_addr[4], xml_config.bd_addr[5]);
		PrintDebug("\tClassOfDevice:%02x:%02x:%02x\n", xml_config.class_of_device[0],
				xml_config.class_of_device[1], xml_config.class_of_device[2]);
		PrintDebug("\tRootPath:%s\n", xml_config.root_path);
		PrintDebug("\tDefault PIN (%d):%s\n", xml_config.pin_len, xml_config.pin_code);
		PrintDebug("I/O Capability:%d\n", xml_config.io_cap);
	}
	return DRV_OK;
}


static DRV_Error P_BT_MGT_SetDeviceConfig(BT_DEV_SETTING_t *pstLocalBt)
{
	tBSA_STATUS status;
	tBSA_DM_SET_CONFIG bt_config;

	/* Init config parameter */
	status = BSA_DmSetConfigInit(&bt_config);

	/* Set bt configuration */
//	bt_config.config_mask = 0;
	
	/* Set BT Enable/Disable */
	bt_config.enable = pstLocalBt->bEnable;
	
	/* Set Visibility */
//	bt_config.config_mask |= BSA_DM_CONFIG_VISIBILITY_MASK;
	bt_config.discoverable = pstLocalBt->bDiscoverable;
	bt_config.connectable = pstLocalBt->bConnectable;
	
	/* Set Local BD Address*/
//	bt_config.config_mask |= BSA_DM_CONFIG_BDADDR_MASK;
	bdcpy(bt_config.bd_addr, pstLocalBt->ucBdAddr);

	/* Set Local BD Name */
//	bt_config.config_mask |= BSA_DM_CONFIG_NAME_MASK;
	VK_MEM_Memcpy((char *)bt_config.name, pstLocalBt->ucName, sizeof(bt_config.name));
	bt_config.name[sizeof(bt_config.name) - 1] = '\0';
	
	/* Set Loca BT Class Of Device */
//	bt_config.config_mask |= BSA_DM_CONFIG_DEV_CLASS_MASK;
	VK_MEM_Memcpy(bt_config.class_of_device, pstLocalBt->ucCOD, sizeof(bt_config.class_of_device));

	PrintDebug("[%s] \no Setting Host Bluetooth Device Info to BSA_Server \n", __FUNCTION__);
	PrintDebug("  - Name:%s\n", bt_config.name);
	PrintDebug("  - Bdaddr %02x:%02x:%02x:%02x:%02x:%02x\n",
			bt_config.bd_addr[0], bt_config.bd_addr[1],
			bt_config.bd_addr[2], bt_config.bd_addr[3],
			bt_config.bd_addr[4], bt_config.bd_addr[5]);
	PrintDebug("  - Enable:%d\n", bt_config.enable);
	PrintDebug("  - Discoverable:%d\n", bt_config.discoverable);
	PrintDebug("  - Connectable:%d\n", bt_config.connectable);
	PrintDebug("  - ClassOfDevice:%02x:%02x:%02x ==> %s\n", bt_config.class_of_device[0],
			bt_config.class_of_device[1], bt_config.class_of_device[2],app_get_cod_string(bt_config.class_of_device));

	/* Apply BT config */
	status = BSA_DmSetConfig(&bt_config);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_DmSetConfig failed:%d \n", __FUNCTION__, status);
		return DRV_ERR;
	}
	status = BSA_DmGetConfigInit(&bt_config);
	status = BSA_DmGetConfig(&bt_config);
	PrintDebug("[%s] \no Get Host Bluetooth Device Info From BSA_Server \n", __FUNCTION__);
	PrintDebug("  - Name:%s\n", bt_config.name);
	PrintDebug("  - Bdaddr %02x:%02x:%02x:%02x:%02x:%02x\n",
			bt_config.bd_addr[0], bt_config.bd_addr[1],
			bt_config.bd_addr[2], bt_config.bd_addr[3],
			bt_config.bd_addr[4], bt_config.bd_addr[5]);
	PrintDebug("  - Enable:%d\n", bt_config.enable);
	PrintDebug("  - Discoverable:%d\n", bt_config.discoverable);
	PrintDebug("  - Connectable:%d\n", bt_config.connectable);
	PrintDebug("  - ClassOfDevice:%02x:%02x:%02x ==> %s\n", bt_config.class_of_device[0],
			bt_config.class_of_device[1], bt_config.class_of_device[2],app_get_cod_string(bt_config.class_of_device));
	PrintDebug("  - First Disabled Channel:%d\n", bt_config.first_disabled_channel);
	PrintDebug("  - Last Disabled Channel:%d\n", bt_config.last_disabled_channel);
	PrintDebug("  - Page Scan Interval:%d\n", bt_config.page_scan_interval);
	PrintDebug("  - Page Scan Window:%d\n", bt_config.page_scan_window);
	PrintDebug("  - Inquiry Scan Interval:%d\n", bt_config.inquiry_scan_interval);
	PrintDebug("  - Inquiry Scan Window:%d\n", bt_config.inquiry_scan_window);
	PrintDebug("  - Tx Power(Point to Point):%d\n", bt_config.tx_power);
	return DRV_OK;
}

static void P_BT_MGT_Callback(tBSA_MGT_EVT event, tBSA_MGT_MSG *p_data)
{
	switch(event)
	{
		case BSA_MGT_STATUS_EVT:
			PrintDebug("[%s] BSA_MGT_STATUS_EVT \n",__FUNCTION__);
			break;
			
		case BSA_MGT_DISCONNECT_EVT:
			PrintError("[%s] BSA_MGT_DISCONNECT_EVT reason: %d \n",__FUNCTION__, p_data->disconnect.reason);
#if defined (BT_STATUS_CHECK)
			/* P_BT_Diable() called */

			/*BTUSB poweroff */
			if ( s_eBtDiStatus == BT_STATUS_ENABLE )
			{
				BT_MSG_t tBtMsg;
				tBtMsg.eMsg = BT_MSG_DISABLE;
				tBtMsg.ulParam1 = FALSE;
				VK_MSG_SendTimeout(s_stBtInstance.ulMsg, &tBtMsg, sizeof(BT_MSG_t),0);
			}
#endif
			break;
			
		default:
			break;
	}
}

static DRV_Error P_BT_MGT_Open(BT_DEV_SETTING_t *pstLocalBt)
{
	DRV_Error dResult = DRV_OK;
	tBSA_STATUS bsaResult;
	tBSA_MGT_OPEN bsa_open_param;
	int i;

	/* Connect to bt daemon */
	bsaResult = BSA_MgtOpenInit(&bsa_open_param);

	bsa_open_param.callback = (tBSA_MGT_CBACK *)P_BT_MGT_Callback;

	if (strlen(pstLocalBt->cRootPath) >= sizeof(bsa_open_param.uipc_path))
	{
		PrintError("[%s] uipc_path too long to fit API (%s) \n", __FUNCTION__, pstLocalBt->cRootPath);
		dResult = DRV_ERR;
		goto ret;
	}

	VK_MEM_Memcpy((char *)bsa_open_param.uipc_path, pstLocalBt->cRootPath, sizeof(bsa_open_param.uipc_path));
//	bsa_open_param.uipc_path[sizeof(bsa_open_param.uipc_path) -1] = '\0';

	for (i = 0;(i*BT_RETRY_DELAY) < BT_CONNECTION_TIMEOUT; i++)
	{
		PrintDebug("[%s] BSA_MgtOpen try %d ...\n",__FUNCTION__, i);
		bsaResult = BSA_MgtOpen(&bsa_open_param);
		if (bsaResult == BSA_SUCCESS)
		{
			PrintDebug("[%s] SUCCESS - [%d] Connect to BSA Server\n", __FUNCTION__, i);
			break;
		}
		else
		{
			PrintDebug("[%s] BSA_MgtOpen failed:%d, try %d ...\n",__FUNCTION__, bsaResult, i);
			VK_TASK_Sleep(BT_RETRY_DELAY);
		}
	}

	if (bsaResult != BSA_SUCCESS)
	{
		PrintError("[%s] Unable to connect to server \n", __FUNCTION__);
		dResult = DRV_ERR;
	}

ret:
	return dResult;
}


static DRV_Error P_BT_MGT_Close(void)
{
	tBSA_MGT_CLOSE  bsa_close_param;
	tBSA_STATUS bsa_status;

	BSA_MgtCloseInit(&bsa_close_param);
	bsa_status = BSA_MgtClose(&bsa_close_param);

	if(bsa_status != BSA_SUCCESS)
	{
		if(bsa_status == BSA_ERROR_CLI_NOT_CONNECTED)
		{
			PrintError("Not connected to server:%d\n", bsa_status);
		}
		else
		{
			PrintError("BSA_MgtClose failed status:%d\n", bsa_status);
			return DRV_ERR;
		}
	}
	return DRV_OK;
}

static void P_BT_MGT_KillServer(void)
{
	tBSA_MGT_KILL_SERVER  param;

	BSA_MgtKillServerInit(&param);
	BSA_MgtKillServer(&param);
}

static DRV_Error P_BT_XML_GetDevList(void)	/*  Not Connected (Disconnected) Device List */
{
	HUINT32 i,j;
	tAPP_XML_REM_DEVICE stTemp;

	
	/* Initialize Paired Device List */
	VK_MEM_Memset(&s_stBtInstance.stPairDevInfo, 0x0, sizeof(s_stBtInstance.stPairDevInfo));


	/* Sorting TRUE Up / FALSE Down */
	for (i = 0; i < APP_MAX_NB_REMOTE_STORED_DEVICES ; i++)
	{
		if (app_xml_remote_devices_db[i].in_use != TRUE)
		{
			for (j = i + 1; j < APP_MAX_NB_REMOTE_STORED_DEVICES; j++)
			{
				if (app_xml_remote_devices_db[j].in_use == TRUE )
				{
					VK_MEM_Memcpy(&stTemp, &app_xml_remote_devices_db[i], sizeof(tAPP_XML_REM_DEVICE));
					VK_MEM_Memcpy(&app_xml_remote_devices_db[i], &app_xml_remote_devices_db[j], sizeof(tAPP_XML_REM_DEVICE));
					VK_MEM_Memcpy(&app_xml_remote_devices_db[j], &stTemp, sizeof(tAPP_XML_REM_DEVICE));
					break;
				}
			}
		}
	}

	/* Saving stPairDevInfo */
	for (i = 0 ; i < APP_MAX_NB_REMOTE_STORED_DEVICES; i++)
	{
		if(app_xml_remote_devices_db[i].in_use == TRUE)
		{
			VK_MEM_Memcpy(&s_stBtInstance.stPairDevInfo.devs[i], &app_xml_remote_devices_db[i], sizeof(tAPP_XML_REM_DEVICE));
			s_stBtInstance.stPairDevInfo.xmlCnt++;
		}
	}

	return DRV_OK;

}




static DRV_Error P_BT_GetPairedDevList(DI_BT_DEV_LIST_t *pstDevList)
{
/*	DRV_Error drvResult = DRV_OK;*/
	HUINT32 ulNum = 0;
	HUINT32 index;

	/* Saving stPairDevInfo  --> pstDevList */
	for (index = 0; index < s_stBtInstance.stPairDevInfo.xmlCnt; index++)	
	{
		if(s_stBtInstance.stPairDevInfo.devs[index].in_use == TRUE)
		{
			VK_MEM_Memcpy(pstDevList->stBtDevList[index].ucName, s_stBtInstance.stPairDevInfo.devs[index].name, BT_NB_NAME_LEN);
			VK_MEM_Memcpy(pstDevList->stBtDevList[index].ucBdAddr, s_stBtInstance.stPairDevInfo.devs[index].bd_addr, BT_NB_BDADDR_LEN);
			VK_MEM_Memcpy(pstDevList->stBtDevList[index].ucClass, s_stBtInstance.stPairDevInfo.devs[index].class_of_device, BT_NB_CLASS_LEN);
			pstDevList->stBtDevList[index].etDevStatus = DI_BT_DEV_DISCONNECTED;
			pstDevList->stBtDevList[index].etAvailableProfile = s_stBtInstance.stPairDevInfo.devs[index].trusted_services;
			ulNum++;
		}
	}
	pstDevList->ulNumOfData = ulNum;
	PrintDebug("[%s] Number of Devices saved in .xml :%d \n", __FUNCTION__, pstDevList->ulNumOfData);



	return DRV_OK;
}

static DRV_Error P_BT_GetDiscDevList(DI_BT_DEV_LIST_t *pstDevList)
{
	HUINT32 ulNumDisc = 0;
	HUINT32 i,j,index;
	tBSA_DISC_DEV stTemp;

	/* Finding a Disc Device in PairedList
	 * If Yes, Disc Device in_use =  FALSE <== Not new Discovere Device */
	for (i = 0 ; i < BT_NB_DISC_DEVICES; i++)
	{
		if(s_stBtInstance.discDevInfo.devs[i].in_use == TRUE)
		{
			for ( j = 0 ; j < s_stBtInstance.stPairDevInfo.xmlCnt; j++)
			{
				if( s_stBtInstance.stPairDevInfo.devs[j].in_use == TRUE)
				{
					if( bdcmp ( s_stBtInstance.discDevInfo.devs[i].device.bd_addr, s_stBtInstance.stPairDevInfo.devs[j].bd_addr) == 0)
					{
						s_stBtInstance.discDevInfo.devs[i].in_use = FALSE;
					}
				}
			}
		}
	}

	PrintDebug("[%s] s_stBtInstance.discDevInfo.DiscoveryCnt :%d \n", __FUNCTION__, s_stBtInstance.discDevInfo.DiscoveryCnt);

	
	/* Sorting TRUE Up / FALSE Down */
	for (i = 0; i < s_stBtInstance.discDevInfo.DiscoveryCnt; i++)
	{
		if (s_stBtInstance.discDevInfo.devs[i].in_use != TRUE)
		{
			for (j = i + 1; j < s_stBtInstance.discDevInfo.DiscoveryCnt; j++)
			{
				if (s_stBtInstance.discDevInfo.devs[j].in_use == TRUE )
				{
					VK_MEM_Memcpy(&stTemp, &s_stBtInstance.discDevInfo.devs[i], sizeof(tBSA_DISC_DEV));
					VK_MEM_Memcpy(&s_stBtInstance.discDevInfo.devs[i], &s_stBtInstance.discDevInfo.devs[j], sizeof(tBSA_DISC_DEV));
					VK_MEM_Memcpy(&s_stBtInstance.discDevInfo.devs[j], &stTemp, sizeof(tBSA_DISC_DEV));
					break;
				}
			}

			if (j >= s_stBtInstance.discDevInfo.DiscoveryCnt)
			{
				break;
			}
		}
	}

	/* Saving stPairDevInfo  --> pstDevList */
	for (index = 0 ; index < BT_NB_DISC_DEVICES; index++)
	{
		if(s_stBtInstance.discDevInfo.devs[index].in_use == TRUE)
		{
			VK_MEM_Memcpy(pstDevList->stBtDevList[index].ucName, s_stBtInstance.discDevInfo.devs[index].device.name, BT_NB_NAME_LEN);
			VK_MEM_Memcpy(pstDevList->stBtDevList[index].ucBdAddr, s_stBtInstance.discDevInfo.devs[index].device.bd_addr, BT_NB_BDADDR_LEN);
			VK_MEM_Memcpy(pstDevList->stBtDevList[index].ucClass, s_stBtInstance.discDevInfo.devs[index].device.class_of_device, BT_NB_CLASS_LEN);
			pstDevList->stBtDevList[index].etDevStatus = DI_BT_DEV_NOT_PAIRED;
			pstDevList->stBtDevList[index].etAvailableProfile = s_stBtInstance.discDevInfo.devs[index].device.services; 
			ulNumDisc++;
		}
	}
	PrintDebug("[%s] Number of Discovered Devices :%d \n", __FUNCTION__, ulNumDisc);
	pstDevList->ulNumOfData  = ulNumDisc;

	return DRV_OK;
}


/* TODO */
static DRV_Error P_BT_SaveDIDevList(void)
{
	DRV_Error drvResult = DRV_OK;

	DI_BT_DEV_LIST_t stPairedDevList;
	DI_BT_DEV_LIST_t stDiscDevList;

	HUINT32 i;

	VK_MEM_Memset(&stPairedDevList, 0x0, sizeof(DI_BT_DEV_LIST_t));
	VK_MEM_Memset(&stDiscDevList, 0x0, sizeof(DI_BT_DEV_LIST_t));

	/* Initial DIDevice List(s_sCbEvtDevInfo) */ 
	VK_MEM_Memset(&s_sCbEvtDevInfo, 0x0, sizeof(DI_BT_DEV_LIST_t));

	/* Loading app_xml_rem_devices_db to stPairDevInfo */
	drvResult = P_BT_XML_GetDevList();		/* Init stPairDevInfo --> Sorting TRUE/FALSE -->Save stPairDevInfo*/
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_XML_GetDevList failed:%d \n", __FUNCTION__, drvResult);
	}
	/* Get Not_Connected Device List from s_stBtInstance.stPairDevInfo  */ 
	drvResult = P_BT_GetPairedDevList(&stPairedDevList);
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_GetPairedDevList failed!\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Get Not_Paired Device List from s_stBtInstance.discDevInfo  */
	drvResult = P_BT_GetDiscDevList(&stDiscDevList);
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_GetDiscDevList failed!\n", __FUNCTION__);
		return DRV_ERR;
	}


	/* Save Not_Connected Device List in s_sCbEvtDevInfo */ 
	VK_MEM_Memcpy(&s_sCbEvtDevInfo, &stPairedDevList, sizeof(DI_BT_DEV_LIST_t));
	
	/* Save Not_Paired Device List in s_sCbEvtDevInfo */
	if( stDiscDevList.ulNumOfData >0)
	{
		s_sCbEvtDevInfo.ulNumOfData = stPairedDevList.ulNumOfData +  stDiscDevList.ulNumOfData;
		for(i = stPairedDevList.ulNumOfData; i < s_sCbEvtDevInfo.ulNumOfData ; i ++ )
		{
			VK_MEM_Memcpy(&s_sCbEvtDevInfo.stBtDevList[i], &stDiscDevList.stBtDevList[i-stPairedDevList.ulNumOfData], sizeof(DI_BT_DEV_INFO_t));
		}
	
	}
//#if defined(DI_DEBUG)
//	P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
//#endif
	return DRV_OK;
}

static HUINT32 P_BT_GetDIDevId(HUINT8 *pucBdAddr)
{
	HUINT32 i;
	for ( i = 0 ; i < BT_NB_DISC_DEVICES ; i++)
	{
		if( bdcmp ( s_sCbEvtDevInfo.stBtDevList[i].ucBdAddr, pucBdAddr)== 0 )
		{
			return i;
		}
	}
	return i;
}

static DRV_Error P_BT_DISC_FindDevice(BD_ADDR bd_addr, tBSA_DISC_DEV *pstDev)	/* related to app_disc_find_device */
{
	int index;

	/* Unfortunately, the BSA_SEC_AUTH_CMPL_EVT does not contain COD */
	/* let's look in the Discovery database */
	for (index = 0; index < BT_NB_DISC_DEVICES; index++)
	{
		if ((s_stBtInstance.discDevInfo.devs[index].in_use != FALSE) &&
				(bdcmp(s_stBtInstance.discDevInfo.devs[index].device.bd_addr, bd_addr) == 0))
		{
			VK_MEM_Memcpy (pstDev, &s_stBtInstance.discDevInfo.devs[index], sizeof(tBSA_DISC_DEV));
			return DRV_OK;
		}
	}
	pstDev = NULL;
	return DRV_ERR;
}

static void P_BT_DISC_Callback(tBSA_DISC_EVT event, tBSA_DISC_MSG *p_data)
{
	int index;

	HINT32	drvResult = 0;
	HBOOL bNewDevice = FALSE;
	DI_BT_CALLBACK_DATA_t stCbData;
	HUINT32 i,j;
	HINT8 szBdAddr[20];

	switch (event)
	{
	    /* a New Device has been discovered */
	    case BSA_DISC_NEW_EVT:
			P_BT_ConvertHex2Str(p_data->disc_new.bd_addr, szBdAddr);
			PrintDebug("[%s] BSA_DISC_NEW_EVT bd_addr: %s\n", __FUNCTION__,szBdAddr);
			bNewDevice = FALSE;
			/* check if this device has already been received (update) */
			for (index = 0; index < BT_NB_DISC_DEVICES; index++)
			{
				if ((s_stBtInstance.discDevInfo.devs[index].in_use == TRUE) &&
					(!bdcmp(s_stBtInstance.discDevInfo.devs[index].device.bd_addr, p_data->disc_new.bd_addr)))
				{
					/* Update device */
					PrintDebug("[%s] Update device : %d\n", __FUNCTION__, index);
					s_stBtInstance.discDevInfo.devs[index].device = p_data->disc_new;
					bNewDevice = FALSE;
					break;
				}
			}

			/* If this is a new device */
			if (index >= BT_NB_DISC_DEVICES)
			{
				/* Look for a free place to store dev info */
				for (index = 0; index < BT_NB_DISC_DEVICES; index++)
				{
					if (s_stBtInstance.discDevInfo.devs[index].in_use == FALSE)
					{
						PrintDebug("[%s] New Discovered device : %d\n", __FUNCTION__, index);
						s_stBtInstance.discDevInfo.devs[index].in_use = TRUE;
						VK_MEM_Memcpy(&s_stBtInstance.discDevInfo.devs[index].device, &p_data->disc_new, sizeof(tBSA_DISC_REMOTE_DEV));
						bNewDevice = TRUE;
						s_stBtInstance.discDevInfo.DiscoveryCnt++;
						break;
					}
				}
			}
			
			if (index >= BT_NB_DISC_DEVICES)
			{
				PrintError("[%s]  No room to save new discovered %d \n", __FUNCTION__, index);
				break;
			}
			
			/* If this is a new device */
			PrintDebug("[%s] \n", __FUNCTION__);
			if(bNewDevice == TRUE)
			{
				PrintDebug("\tBdaddr:%s\n", szBdAddr);
				PrintDebug("\tName:%s\n", p_data->disc_new.name);
				PrintDebug("\tClassOfDevice:%02x:%02x:%02x => %s\n",
						p_data->disc_new.class_of_device[0], p_data->disc_new.class_of_device[1],
						p_data->disc_new.class_of_device[2],app_get_cod_string(p_data->disc_new.class_of_device));
				PrintDebug("\tServices:0x%08x (%s)\n",(int) p_data->disc_new.services,
						app_service_mask_to_string(p_data->disc_new.services));
				PrintDebug("\tRssi:%d\n", p_data->disc_new.rssi);
				if (p_data->disc_new.eir_vid_pid[0].valid)
				{
					PrintDebug("\tVidSrc:%d Vid:0x%04X Pid:0x%04X Version:0x%04X\n",
							p_data->disc_new.eir_vid_pid[0].vendor_id_source,
							p_data->disc_new.eir_vid_pid[0].vendor,
							p_data->disc_new.eir_vid_pid[0].product,
							p_data->disc_new.eir_vid_pid[0].version);
				}
			}
			break;

		case BSA_DISC_CMPL_EVT: /* Discovery complete. */
			PrintDebug("[%s] BSA_DISC_CMPL_EVT\n Discovery complete \n", __FUNCTION__);
			
			s_eBtDiStatus = BT_STATUS_SCANNED;

			/* Read the Remote device xml file to have a fresh view */
			app_read_xml_remote_devices();

			/* Reload s_sCbEvtDevInfo */
			drvResult = P_BT_SaveDIDevList();
			if(drvResult != DRV_OK)
			{
				PrintError("[%s] P_BT_SaveDIDevList failed!\n", __FUNCTION__);
			}

			/* Finding Connecting Device on s_sCbEvtDevInfo */
			/* Updating the status of Device on s_sCbEvtDevInfo */
			for( i = 0 ; i < s_sCbEvtDevInfo.ulNumOfData ; i++)
			{
				for ( j = 0 ; j < BT_NB_PAIR_DEVICES ; j++)
				{
					if( (s_stBtInstance.stLinkDevInfo.devs[j].bInUse == TRUE ) &&
							(!bdcmp (s_sCbEvtDevInfo.stBtDevList[i].ucBdAddr,s_stBtInstance.stLinkDevInfo.devs[j].ucBdAddr)))
					{
						s_sCbEvtDevInfo.stBtDevList[i].etDevStatus = DI_BT_DEV_CONNECTED;
					
					}
				}
			}
#if defined(DI_DEBUG)
//			P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif
			if( s_stBtInstance.stPairDevInfo.xmlCnt < s_sCbEvtDevInfo.ulNumOfData )
			{
				stCbData.eEvent = DI_BT_NOTIFY_EVT_SCAN_SUCCESS;	/*DI_BT_NOTIFY_EVT_SCAN_CMPL*/
				stCbData.ulDevId = 0;
				stCbData.ulNumOfData =  s_sCbEvtDevInfo.ulNumOfData;
				stCbData.pucBdAddr = NULL;
				stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
			}
			else
			{
				stCbData.eEvent = DI_BT_NOTIFY_EVT_SCAN_SUCCESS;
				stCbData.ulDevId = 0;
				stCbData.ulNumOfData =  s_sCbEvtDevInfo.ulNumOfData;
				stCbData.pucBdAddr = NULL;
				stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
			}
			P_BT_PostMsgToMw(&stCbData);
			break;

		case BSA_DISC_DEV_INFO_EVT: /* Discovery Device Info */
			PrintDebug("[%s] BSA_DEV_INFO_EVT ", __FUNCTION__);
			PrintDebug("Discover Device Info status:%d\n", p_data->dev_info.status);
			P_BT_ConvertHex2Str(p_data->dev_info.bd_addr, szBdAddr);
			PrintDebug("\tBdaddr:%s\n", szBdAddr);
			if(p_data->dev_info.status == BSA_SUCCESS)
			{
				PrintDebug("\tDevice VendorIdSource:0x%x\n", p_data->dev_info.vendor_id_source);
				PrintDebug("\tDevice Vendor:0x%x\n", p_data->dev_info.vendor);
				PrintDebug("\tDevice Product:0x%x\n", p_data->dev_info.product);
				PrintDebug("\tDevice Version:0x%x\n", p_data->dev_info.version);
				PrintDebug("\tDevice SpecId:0x%x\n", p_data->dev_info.spec_id);
			}
			break;

		default:
			PrintError("[%s] unknown event:%d\n",__FUNCTION__, event);
			break;
	}
}

static DRV_Error P_BT_DISC_Start(HUINT32 ulServiceMask)
{
	tBSA_SERVICE_MASK   stBsaServices = 0;
	tBSA_DISC_START     disc_start_param;
	DRV_Error	drvResult = DRV_OK;
	tBSA_STATUS status;
	

	PrintDebug("[%s] Start Discovery !!! \n", __FUNCTION__);
	/* Init device list & discovery status */
	VK_MEM_Memset(&s_stBtInstance.discDevInfo, 0x0, sizeof(s_stBtInstance.discDevInfo));

	/* Discovery init */
	status = BSA_DiscStartInit(&disc_start_param);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_DiscStartInit failed status:%d \n", __FUNCTION__, status);
		return DRV_ERR;
	}

	/* Setting Discovery Parameter */
	if (ulServiceMask == DI_BT_PROFILE_ALL)
	{
	/* All profiles */
		/*		stBsaServices = BSA_ALL_SERVICE_MASK;*/
		/* If this mask is used, discovery time is too long because of searching all profiles.
		 * Every profile for 4*1.28s
		 */

		/* HMS Supporting Profiles(expected) */

		// stBsaServices |= BSA_SPP_SERVICE_MASK;		/* Serial port profile. */
		// stBsaServices |= BSA_HSP_SERVICE_MASK;		/* Headset profile. */
		// stBsaServices |= BSA_HFP_SERVICE_MASK;		/* Hands-free profile. */
		// stBsaServices |= BSA_OPP_SERVICE_MASK;		/* Object push  */
		// stBsaServices |= BSA_FTP_SERVICE_MASK;		/* File transfer */
		// stBsaServices |= BSA_PANU_SERVICE_MASK;		/* PAN Network user */
		// stBsaServices |= BSA_NAP_SERVICE_MASK;		/* PAN Network access point */
		// stBsaServices |= BSA_GN_SERVICE_MASK;		/* PAN Group Ad-hoc networks */
		// stBsaServices |= BSA_SAP_SERVICE_MASK;		/* SIM Access profile.  */
		//stBsaServices |= BSA_A2DP_SERVICE_MASK;		/* Advanced audio distribution */	//Goal : PH1
		//stBsaServices |= BSA_AVRCP_SERVICE_MASK;		/* A/V remote control */
		//stBsaServices |= BSA_HID_SERVICE_MASK;		/* HID */
		// stBsaServices |= BSA_VDP_SERVICE_MASK;		/* Video distribution */
		// stBsaServices |= BSA_HL_SERVICE_MASK;		/* Health Profile Profile */
		
		/* HMS Supporting Profiles(expected) */

		//stBsaServices = BSA_ALL_SERVICE_MASK;
		stBsaServices = 0;
	}
	else
	{
		stBsaServices |= (ulServiceMask & DI_BT_PROFILE_A2DP) ? BSA_A2DP_SERVICE_MASK : 0;
		stBsaServices |= (ulServiceMask & DI_BT_PROFILE_AVRCP)? BSA_AVRCP_SERVICE_MASK: 0;
		stBsaServices |= (ulServiceMask & DI_BT_PROFILE_HID)  ? BSA_HID_SERVICE_MASK  : 0;
	}
	disc_start_param.services = stBsaServices;
	disc_start_param.cback = P_BT_DISC_Callback;
	disc_start_param.nb_devices = 0;
	disc_start_param.duration = 4;

#if 0 //For Debug, Scan Headsets Only
	disc_start_param.filter_type = BSA_DM_INQ_DEV_CLASS;
	FIELDS_TO_COD(disc_start_param.filter_cond.dev_class_cond.dev_class,0x04,0x04, 0x2000);
	FIELDS_TO_COD(disc_start_param.filter_cond.dev_class_cond.dev_class_mask,BTM_COD_MINOR_CLASS_MASK, BTM_COD_MAJOR_CLASS_MASK,0x2000);
#endif

	status = BSA_DiscStart(&disc_start_param);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_DiscStart failed status:%d \n", __FUNCTION__, status);
		return DRV_ERR;
	
	}
	s_eBtDiStatus = BT_STATUS_SCANNING;
	

	return drvResult;

}

/*
static DRV_Error P_BT_SEC_Bond(BD_ADDR bd_addr)
{
	tBSA_STATUS         status;
	tBSA_SEC_BOND       sec_bond;

	BSA_SecBondInit(&sec_bond);
    bdcpy(sec_bond.bd_addr, bd_addr);

    status = BSA_SecBond(&sec_bond);

	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_SecBond failed: Unable to Bond BdAddr(%02x:%02x:%02x:%02x:%02x:%02x)\n", __FUNCTION__,  status,
				sec_bond.bd_addr[0], sec_bond.bd_addr[1],sec_bond.bd_addr[2],
				sec_bond.bd_addr[3],sec_bond.bd_addr[4], sec_bond.bd_addr[5]);
		return DRV_ERR;
	}
	return DRV_OK;
}
*/
static DRV_Error P_BT_SEC_PinCodeReply(BD_ADDR bd_addr, PIN_CODE_PTR pPin_Code, HUINT32 ulPinCodeLength )
{
	tBSA_STATUS         status;
	tBSA_SEC_PIN_CODE_REPLY pin_code_reply;

	BSA_SecPinCodeReplyInit(&pin_code_reply);

	bdcpy(pin_code_reply.bd_addr, bd_addr);

	VK_MEM_Memcpy(pin_code_reply.pin_code, pPin_Code,BT_NB_PINCODE_LEN);
	pin_code_reply.pin_len = ulPinCodeLength;

	status = BSA_SecPinCodeReply(&pin_code_reply);

	PrintDebug("[%s] Reply Pincode : %s (%d)\n",__FUNCTION__, pin_code_reply.pin_code, pin_code_reply.pin_len); 
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_SecPinCodeReply failed: Unable to Reply BdAddr(%02x:%02x:%02x:%02x:%02x:%02x)\n", __FUNCTION__,
				pin_code_reply.bd_addr[0], pin_code_reply.bd_addr[1],
				pin_code_reply.bd_addr[2], pin_code_reply.bd_addr[3],
				pin_code_reply.bd_addr[4], pin_code_reply.bd_addr[5]);
		return DRV_ERR;
	}
	return DRV_OK;
}



static DRV_Error P_BT_SEC_AddDevice(tAPP_XML_REM_DEVICE *pstXmlDev)
{
	tBSA_STATUS         status;
	tBSA_SEC_ADD_DEV sec_add_dev;

	BSA_SecAddDeviceInit(&sec_add_dev);

	VK_MEM_Memcpy(sec_add_dev.bd_name, pstXmlDev->name, sizeof(sec_add_dev.bd_name));
	bdcpy(sec_add_dev.bd_addr, pstXmlDev->bd_addr);
	sec_add_dev.link_key_present	= pstXmlDev->link_key_present;
	VK_MEM_Memcpy(sec_add_dev.link_key, pstXmlDev->link_key, sizeof(sec_add_dev.link_key));
	VK_MEM_Memcpy(sec_add_dev.class_of_device, pstXmlDev->class_of_device, sizeof(sec_add_dev.class_of_device));
	sec_add_dev.trusted_services	= pstXmlDev->trusted_services;
	sec_add_dev.is_trusted			= TRUE;
	sec_add_dev.key_type			= pstXmlDev->key_type;
	sec_add_dev.io_cap				= pstXmlDev->io_cap;
    status = BSA_SecAddDevice(&sec_add_dev);

	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_SecAddDevice failed: Unable to Add BdAddr(%02x:%02x:%02x:%02x:%02x:%02x)\n", __FUNCTION__,  status,
				sec_add_dev.bd_addr[0], sec_add_dev.bd_addr[1],sec_add_dev.bd_addr[2],
				sec_add_dev.bd_addr[3],sec_add_dev.bd_addr[4], sec_add_dev.bd_addr[5]);
		return DRV_ERR;
	}
	return DRV_OK;
}


static DRV_Error P_BT_SEC_RemoveDevice(BD_ADDR bd_addr)
{
	int status;
	tBSA_SEC_REMOVE_DEV sec_dev;
	
	BSA_SecRemoveDeviceInit(&sec_dev);
	bdcpy(sec_dev.bd_addr, bd_addr);
	status = BSA_SecRemoveDevice(&sec_dev);

	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_SecRemoveDevice failed: Unable to Remove BdAddr(%02x:%02x:%02x:%02x:%02x:%02x)\n", __FUNCTION__,  status,
				sec_dev.bd_addr[0], sec_dev.bd_addr[1],sec_dev.bd_addr[2],
				sec_dev.bd_addr[3],sec_dev.bd_addr[4], sec_dev.bd_addr[5]);
		return DRV_ERR;
	}
	return DRV_OK;
	
}



static void P_BT_SEC_Callback(tBSA_SEC_EVT event, tBSA_SEC_MSG *p_data)
{
	int status;
	HINT32 drvResult = 0;
	tBSA_SEC_AUTH_REPLY autorize_reply;
	tBSA_SEC_SP_CFM_REPLY sp_cfm_reply;

	tBSA_DISC_DEV disc_dev;
	HUINT32 ulDevId;
	DI_BT_CALLBACK_DATA_t stCbData;
	HINT8 szBdAddr[20];

	switch(event)
	{
	case BSA_SEC_LINK_UP_EVT:	  /* A device is physically connected (for info) */
		P_BT_ConvertHex2Str(p_data->link_up.bd_addr, szBdAddr);
		PrintDebug("[%s] BSA_SEC_LINK_UP_EVT bd_addr: %s\n", __FUNCTION__, szBdAddr);
		PrintDebug("[%s] Class of Device: %02x:%02x:%02x => %s\n",__FUNCTION__,
						p_data->link_up.class_of_device[0], p_data->link_up.class_of_device[1],
						p_data->link_up.class_of_device[2],app_get_cod_string(p_data->link_up.class_of_device));
		break;
	case BSA_SEC_LINK_DOWN_EVT:
		P_BT_ConvertHex2Str(p_data->link_down.bd_addr,szBdAddr);
		PrintDebug("[%s] BSA_SEC_LINK_DOWN_EVT bd_addr: %s\n",__FUNCTION__, szBdAddr);
		PrintDebug("[%s] Reason: %d (0X%x)\n", __FUNCTION__, p_data->link_down.status, p_data->link_down.status);
		break;
	case BSA_SEC_PIN_REQ_EVT:
		P_BT_ConvertHex2Str(p_data->pin_req.bd_addr,szBdAddr);
		PrintDebug("[%s] BSA_SEC_PIN_REQ_EVT (Pin Code Request) received from \nbd_addr: %s\n", __FUNCTION__, szBdAddr);
#if defined(DI_DEBUG)
//		P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif
		ulDevId = P_BT_GetDIDevId(p_data->pin_req.bd_addr);
		/* Send EVT to MW */
		stCbData.eEvent = DI_BT_NOTIFY_EVT_AUTHEN_FAIL;
		stCbData.ulDevId = ulDevId;
		stCbData.ulNumOfData = s_sCbEvtDevInfo.ulNumOfData;
		stCbData.pucBdAddr = (HUINT8 *)&p_data->pin_req.bd_addr;
		stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
		P_BT_PostMsgToMw(&stCbData);
		break;

	case BSA_SEC_AUTH_CMPL_EVT:
		PrintDebug("[%s] BSA_SEC_AUTH_CMPL_EVT (name = %s, success= %d)\n", __FUNCTION__,
			p_data->auth_cmpl.bd_name, p_data->auth_cmpl.success);
		if(p_data->auth_cmpl.success ==0)
		{
			PrintDebug("[%s]	fail_reason=%d\n",__FUNCTION__, p_data->auth_cmpl.success);

			/* Send EVT, Callbackdata to MW */
#if defined(DI_DEBUG)
//			P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif
			ulDevId= P_BT_GetDIDevId(p_data->auth_cmpl.bd_addr);
			stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_FAIL;
			stCbData.ulDevId = ulDevId;
			stCbData.ulNumOfData = s_sCbEvtDevInfo.ulNumOfData;
			stCbData.pucBdAddr = (HUINT8 *)&p_data->auth_cmpl.bd_addr;
			stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
			P_BT_PostMsgToMw(&stCbData);

		}
		P_BT_ConvertHex2Str(p_data->auth_cmpl.bd_addr, szBdAddr);
		PrintDebug("[%s] bd_addr: %s\n",__FUNCTION__, szBdAddr);

		if(p_data->auth_cmpl.key_present != FALSE)
		{
			PrintDebug("[%s]LinkKey:  %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",__FUNCTION__,
					p_data->auth_cmpl.key[0], p_data->auth_cmpl.key[1], p_data->auth_cmpl.key[2],p_data->auth_cmpl.key[4],
					p_data->auth_cmpl.key[4], p_data->auth_cmpl.key[5], p_data->auth_cmpl.key[6],p_data->auth_cmpl.key[7],
					p_data->auth_cmpl.key[8], p_data->auth_cmpl.key[9], p_data->auth_cmpl.key[10],p_data->auth_cmpl.key[11],
					p_data->auth_cmpl.key[12], p_data->auth_cmpl.key[13], p_data->auth_cmpl.key[14],p_data->auth_cmpl.key[15]);
		}

		if (p_data->auth_cmpl.success != 0)
		{
			/* Read the Remote device xml file to have a fresh view */
			app_read_xml_remote_devices();

			app_xml_update_name_db(app_xml_remote_devices_db,
					APP_NUM_ELEMENTS(app_xml_remote_devices_db),
					p_data->auth_cmpl.bd_addr,
					p_data->auth_cmpl.bd_name);

			if (p_data->auth_cmpl.key_present != FALSE)
			{
				app_xml_update_key_db(app_xml_remote_devices_db,
						APP_NUM_ELEMENTS(app_xml_remote_devices_db),
						p_data->auth_cmpl.bd_addr,
						p_data->auth_cmpl.key,
						p_data->auth_cmpl.key_type);
			}

			/* Unfortunately, the BSA_SEC_AUTH_CMPL_EVT does not contain COD */
			/* so update the XML with info found during inquiry */
			drvResult = P_BT_DISC_FindDevice(p_data->auth_cmpl.bd_addr, &disc_dev);
			if ( drvResult != DRV_ERR)
			{
				app_xml_update_cod_db(app_xml_remote_devices_db,
						APP_NUM_ELEMENTS(app_xml_remote_devices_db),
						p_data->auth_cmpl.bd_addr,
						disc_dev.device.class_of_device);
			}
		}
		break;

	case BSA_SEC_BOND_CANCEL_CMPL_EVT:/* Bond cancel complete indication */
		PrintDebug("[%s] BSA_SEC_BOND_CANCEL_CMPL_EVT status = %d\n",__FUNCTION__, p_data->bond_cancel.status);
		break;

	case BSA_SEC_AUTHORIZE_EVT:	/* Authorization request *//* When A Remote Device tries to connect HMS */
		PrintDebug("[%s] BSA_SEC_AYTHORIZE_EVT\n Remote Device: %s\n",__FUNCTION__, p_data->authorize.bd_name);
		P_BT_ConvertHex2Str(p_data->authorize.bd_addr,szBdAddr);
		PrintDebug("[%s] bd_addr : %s\n",__FUNCTION__, szBdAddr);
		PrintDebug("[%s] Request access to service:%x (%s)\n		Access Granted (permanently)",__FUNCTION__,
				(int)p_data->authorize.service,
				app_service_mask_to_string(p_data->authorize.service));
		status = BSA_SecAuthorizeReplyInit(&autorize_reply);
		bdcpy(autorize_reply.bd_addr, p_data->authorize.bd_addr);
		autorize_reply.trusted_service = p_data->authorize.service;
		autorize_reply.auth = BSA_SEC_AUTH_PERM;
		status = BSA_SecAuthorizeReply(&autorize_reply);

		ulDevId = P_BT_GetDIDevId(p_data->authorize.bd_addr);
		/*
		 * Update XML database
		 * */
		/* Read the Remote device xml file to have a fresh view */
		app_read_xml_remote_devices();

		/* Add AV service for this devices in XML database */
		app_xml_add_trusted_services_db(app_xml_remote_devices_db,
				APP_NUM_ELEMENTS(app_xml_remote_devices_db), p_data->authorize.bd_addr,
				1 << p_data->authorize.service);

		if (strlen((char *)p_data->authorize.bd_name) > 0)
		{
			app_xml_update_name_db(app_xml_remote_devices_db,
					APP_NUM_ELEMENTS(app_xml_remote_devices_db),
					p_data->authorize.bd_addr, p_data->authorize.bd_name);
		}

		/* Update database => write on disk */
		drvResult = app_write_xml_remote_devices();
		if ( drvResult != DRV_OK)
		{
			PrintError("[%s] fail to store remote devices database!!![%d]\n",__FUNCTION__, drvResult);	
		}
		else
		{
			PrintDebug("[%s] updated remote devices database[%d]\n",__FUNCTION__, drvResult);
		}
		break;

	case BSA_SEC_SP_CFM_REQ_EVT:	/* Simple Paring confirm reqeust */
		PrintDebug("[%s] BSA_SEC_SP_CFM_REQ_EVT\n", __FUNCTION__);
		PrintDebug("[%s]    Remote device:%s\n",__FUNCTION__, p_data->cfm_req.bd_name);
		P_BT_ConvertHex2Str(p_data->cfm_req.bd_addr, szBdAddr);
		PrintDebug("[%s]    bd_addr: %s\n",__FUNCTION__, szBdAddr);
		PrintDebug("[%s]    ClassOfDevice:%02x:%02x:%02x => %s\n",__FUNCTION__,
				p_data->cfm_req.class_of_device[0], p_data->cfm_req.class_of_device[1], p_data->cfm_req.class_of_device[2],
				app_get_cod_string(p_data->cfm_req.class_of_device));
		PrintDebug("[%s]    Just Work:%s\n",__FUNCTION__, p_data->cfm_req.just_works == TRUE ? "TRUE" : "FALSE");
		PrintDebug("[%s]    Numeric Value:%d\n",__FUNCTION__, p_data->cfm_req.num_val);

		/* When A Remote Device tries to connect HMS thorugh Secure Simple Paring
		 * Auto Accept TRUE
		 */
		status = BSA_SecSpCfmReplyInit(&sp_cfm_reply);
		sp_cfm_reply.accept = TRUE;		//TODO:Modify
		bdcpy(sp_cfm_reply.bd_addr, p_data->cfm_req.bd_addr);
		status = BSA_SecSpCfmReply(&sp_cfm_reply);
		/* TODO */
		/* When A Remote Device tries to connect HMS thorugh Secure Simple Paring
		 * Auto Accept FALSE
		 */

		break;

	case BSA_SEC_SP_KEY_NOTIF_EVT:  /* Simple Paring Passkey Notification */
		/* When HMS tries to connect a remote device thorugh Secure Simple Paring */ 
		P_BT_ConvertHex2Str(p_data->key_notif.bd_addr,szBdAddr);
		PrintDebug("[%s] BSA_SEC_SP_KEY_NOTIF_EVT bd_addr: %s\n",__FUNCTION__, szBdAddr);
		PrintDebug("[%s]   ClassOfDevice:%02x:%02x:%02x => %s\n",__FUNCTION__,
				p_data->key_notif.class_of_device[0], p_data->key_notif.class_of_device[1], p_data->key_notif.class_of_device[2],
				app_get_cod_string(p_data->key_notif.class_of_device));
		PrintDebug("[%s]   Numeric Value:%d\n",__FUNCTION__, p_data->key_notif.passkey);
		PrintDebug("[%s]\t => You must enter this value on peer device's keyboard",__FUNCTION__);

		/* Send EVT to MW */
		ulDevId= P_BT_GetDIDevId(p_data->key_notif.bd_addr);
		stCbData.eEvent = DI_BT_NOTIFY_EVT_PASSKEY;
		stCbData.ulDevId = ulDevId;
		stCbData.ulNumOfData = s_sCbEvtDevInfo.ulNumOfData;
		stCbData.pucBdAddr = (HUINT8 *)&p_data->key_notif.bd_addr;
		stCbData.pvEventData = (void *)&p_data->key_notif.passkey;
		P_BT_PostMsgToMw(&stCbData);

		break;

	case BSA_SEC_SP_KEYPRESS_EVT: /* Simple Pairing Key press notification event */
		PrintDebug("[%s] BSA_SEC_SP_KEYPRESS_EVT (type:%d)\n",__FUNCTION__,  p_data->key_press.notif_type);
		P_BT_ConvertHex2Str(p_data->key_press.bd_addr, szBdAddr);
		PrintDebug("[%s]   bd_addr: %s\n",__FUNCTION__, szBdAddr);
		break;

	case BSA_SEC_SP_RMT_OOB_EVT: /* Simple Pairing Remote OOB Data request */
		PrintDebug("[%s] BSA_SEC_SP_RMT_OOB_EVT[%d] received\n",__FUNCTION__, event);
		PrintDebug("[%s] Not Yet Implemented", __FUNCTION__);
        break;

	case BSA_SEC_SUSPENDED_EVT:/* An ACL Link is in Suspended mode */
		P_BT_ConvertHex2Str(p_data->suspended.bd_addr, szBdAddr);
		PrintDebug("[%s] BSA_SEC_SUSPENDED_EVT bd_addr: %s\n",__FUNCTION__,szBdAddr);
        break;

	case BSA_SEC_RESUMED_EVT: /* An ACL Link is in Resumed mode */
		P_BT_ConvertHex2Str(p_data->resumed.bd_addr, szBdAddr);
		PrintDebug("[%s] BSA_SEC_RESUMED_EVT bd_addr: %s\n",__FUNCTION__,szBdAddr);
        break;

	default:
		PrintError("[%s] unknown event:%d\n",__FUNCTION__, event);
		break;
	}
}


static DRV_Error P_BT_SEC_SetSecurity(BT_DEV_SETTING_t *pstLocalBt)
{
	tBSA_STATUS				status;
	tBSA_SEC_SET_SECURITY	set_security;

	BSA_SecSetSecurityInit(&set_security);
	set_security.simple_pairing_io_cap = pstLocalBt->io_cap;
	set_security.sec_cback = P_BT_SEC_Callback;

	status = BSA_SecSetSecurity(&set_security);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_SecSetSecurity failed: Unable to Set Security\n", __FUNCTION__,  status);
		return DRV_ERR;
	}
	return DRV_OK;
}

static DRV_Error P_BT_TM_ReadVersion(void)
{
	tBSA_TM_READ_VERSION bsa_read_version;
	tBSA_STATUS bsa_status;
	bsa_status = BSA_TmReadVersionInit(&bsa_read_version);
	bsa_status = BSA_TmReadVersion(&bsa_read_version);
	if (bsa_status != BSA_SUCCESS)
	{
		PrintError("[%s]BSA_TmReadVersion failed status:%d\n", __FUNCTION__, bsa_status);
		return DRV_ERR;
	}
	
	PrintError("[%s]Server status:%d\n", __FUNCTION__, bsa_read_version.status);
	PrintError("[%s]FW Version:%d.%d.%d.%d\n",__FUNCTION__,
			bsa_read_version.fw_version.major, bsa_read_version.fw_version.minor,
			bsa_read_version.fw_version.build, bsa_read_version.fw_version.config);
	PrintError("[%s]BSA Server Version:%s\n",__FUNCTION__,  bsa_read_version.bsa_server_version);
	return DRV_OK;
}

static DRV_Error P_BT_TM_SetTrace(int level)
{
	tBSA_TM_SET_TRACE_LEVEL trace_level_req;
	tBSA_STATUS status;

	BSA_TmSetTraceLevelInit(&trace_level_req);

	if ((level < BSA_TM_TRACE_LEVEL_NONE) ||
			(level > BSA_TM_TRACE_LEVEL_DEBUG))
	{
		PrintError("[%s] bad trace level:%d\n",__FUNCTION__, level);
		return DRV_ERR;
	}
	trace_level_req.all_trace_level = level;

	status = BSA_TmSetTraceLevel(&trace_level_req);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s]BSA_TmSetTraceLevel failed status:%d\n",__FUNCTION__,status);
		return DRV_ERR;
	}
	return DRV_OK;
}

static DRV_Error P_BT_Enable(void)
{
	DRV_Error drvResult = DRV_OK;
	DI_BT_CALLBACK_DATA_t stCbData;
	HUINT32 i;
#if defined (BT_STATUS_CHECK)
	if (s_eBtDiStatus >= BT_STATUS_ENABLE)
	{
		PrintError("[%s] BT driver already enabled\n", __FUNCTION__);

		return drvResult;
	}
#endif
	/* Setting */
	s_stBtInstance.stHostSettings.bEnable = TRUE;
	s_stBtInstance.stHostSettings.bDiscoverable = FALSE;	//주변에 다른 HMS-1000S가 스캔되지 않도록
	s_stBtInstance.stHostSettings.bConnectable = TRUE;

	/* Run BSA Server */
	if ( s_bBsaServer != TRUE )
	{
		drvResult = P_BT_Open();
		if(drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_Open failed!\n", __FUNCTION__);
			return DRV_ERR;
		}
		s_bBsaServer = TRUE;
	}

	/* Open Communiation to BSA_SERVER */
	if ( s_bMgtOpen != TRUE )
	{
		drvResult = P_BT_MGT_Open(&s_stBtInstance.stHostSettings);
		if(drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_MGT_Open failed!\n", __FUNCTION__);
			return DRV_ERR;
		}
		s_bMgtOpen = TRUE;
	}

	/* Initial AV Profile */	
	drvResult = P_BT_AV_Init();
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_AV_INIT failed!\n", __FUNCTION__);
	}

	/* Initialize HID Profile */
	P_BT_HID_Init();

	/* Send CMD to BSA_Server */
	drvResult = P_BT_MGT_SetDeviceConfig(&s_stBtInstance.stHostSettings);		// Execution
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_MGT_SetDeviceConfig failed:%d \n", __FUNCTION__, drvResult);
		return DRV_ERR;
	}
	/* Set Local Bt Device Security related to simple pairing */
	drvResult = P_BT_SEC_SetSecurity(&s_stBtInstance.stHostSettings);
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_SEC_SetSecurity failed!\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Initial DIDevice List(s_sCbEvtDevInfo) */ 
	VK_MEM_Memset(&s_sCbEvtDevInfo, 0x0, sizeof(DI_BT_DEV_LIST_t));

	/* Loading XML File to app_xml_rem_devices_db */
	app_read_xml_remote_devices();

	/* Loading app_xml_rem_devices_db to stPairDevInfo */
	drvResult = P_BT_XML_GetDevList();		/* Init stPairDevInfo --> Sorting TRUE/FALSE -->Save stPairDevInfo*/
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_XML_GetDevList failed:%d \n", __FUNCTION__, drvResult);
	}
	/* Load s_sCbEvtDevInfo */
	drvResult = P_BT_GetPairedDevList(&s_sCbEvtDevInfo);
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_GetPairedDevList failed!\n", __FUNCTION__);
	}
	/* Add devices in BSA_Server Database */
	for ( i = 0 ; i < s_sCbEvtDevInfo.ulNumOfData;i++)
	{
		drvResult = P_BT_SEC_AddDevice(&app_xml_remote_devices_db[i]);
		if ( drvResult != DRV_OK)
		{
			PrintError("[%s] Error: P_BT_SEC_AddDevice \n", __FUNCTION__);
		}
	}

	/* Store Bt_config.xml */
	drvResult = P_BT_MGT_Write_Config(&s_stBtInstance.stHostSettings);
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] Writing Local Bt Device Setting to bt_config.xml Failed!\n",__FUNCTION__);
		return DRV_ERR;
	}
#if defined (BT_STATUS_CHECK)
	s_eBtDiStatus = BT_STATUS_ENABLE;
#endif
	stCbData.eEvent = DI_BT_NOTIFY_EVT_ENABLE;
	stCbData.ulDevId = 0;
	stCbData.ulNumOfData = s_sCbEvtDevInfo.ulNumOfData;
	stCbData.pucBdAddr = NULL;
	stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;

	P_BT_PostMsgToMw(&stCbData);
	
	return drvResult;
}

static DRV_Error P_BT_Disable(void)
{
	DRV_Error drvResult = DRV_OK;
	DI_BT_CALLBACK_DATA_t stCbData;
#if defined (BT_STATUS_CHECK)
	if (s_eBtDiStatus < BT_STATUS_ENABLE)
	{
		PrintError("[%s] BT driver doesn't enabled\n", __FUNCTION__);

		return drvResult;
	}
#endif
	/* Setting */
	s_stBtInstance.stHostSettings.bEnable = FALSE;
	s_stBtInstance.stHostSettings.bDiscoverable = FALSE;
	s_stBtInstance.stHostSettings.bConnectable = FALSE;

	/* Clear s_sCbEvtDevInfo */
	VK_MEM_Memset(&s_sCbEvtDevInfo, 0x0, sizeof(DI_BT_DEV_LIST_t));

	/* Clear Device Info */ 
	VK_MEM_Memset(&s_stBtInstance.stPairDevInfo, 0x0, sizeof(BT_CON_DEV_INFO_t));
	VK_MEM_Memset(&s_stBtInstance.stLinkDevInfo, 0x0, sizeof(BT_LINK_DEV_INFO_t));
	VK_MEM_Memset(&s_stBtInstance.discDevInfo, 0x0, sizeof(BT_DISC_DEV_INFO_t));

	/* Deinit AV */
	drvResult = P_BT_AV_Deinit();
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_AV_Deinit failed!\n", __FUNCTION__);
	}

	/* Deinit HID */
	P_BT_HID_Deinit();

	/* Close BSA_SerVer */
	drvResult = P_BT_Close();

	/* Close Communiation to BSA_SERVER */
	drvResult = P_BT_MGT_Close();
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_MGT_Close failed\n", __FUNCTION__);
	}

	/* Saving app_xml_rem_devices_db to bt_device.xml */
	drvResult = app_write_xml_remote_devices();
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] app_write_xml_remote_devices failed\n", __FUNCTION__);
	}

	/* Store Bt_config.xml */
	drvResult = P_BT_MGT_Write_Config(&s_stBtInstance.stHostSettings);
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] Writing Local Bt Device Setting to bt_config.xml Failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

#if defined (BT_STATUS_CHECK)
	s_eBtDiStatus = BT_STATUS_INIT;
#endif

	s_bBsaServer = FALSE;
	s_bMgtOpen = FALSE;


	stCbData.eEvent = DI_BT_NOTIFY_EVT_DISABLE;
	stCbData.ulDevId = 0;
	stCbData.ulNumOfData = 0;
	stCbData.pucBdAddr = NULL;
	stCbData.pvEventData = (void *)NULL;

	P_BT_PostMsgToMw(&stCbData);
	
	return drvResult;
}
/*
static DRV_Error P_BT_Pair(HUINT32 ulDevId)
{
	DRV_Error drvResult = DRV_OK;


	drvResult = P_BT_SEC_Bond(s_sCbEvtDevInfo.stBtDevList[ulDevId].ucBdAddr);
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_SEC_Bond failed:%d \n", __FUNCTION__, drvResult);
		return DRV_ERR;
	}

	return drvResult;
}
*/
static DRV_Error P_BT_Connect(HUINT32 ulDevId)
{
	DRV_Error drvResult = DRV_OK;
	DI_BT_DEV_INFO_t *pstConnectDev = NULL;
	DI_BT_CALLBACK_DATA_t stCbData;

	pstConnectDev = &s_sCbEvtDevInfo.stBtDevList[ulDevId];

	/* Audio Device(0x04) */ 	/* A2DP */
	if((pstConnectDev->etAvailableProfile & DI_BT_PROFILE_A2DP)||((pstConnectDev->ucClass[1]&0x1F) == BTM_COD_MAJOR_AUDIO))
	{
		drvResult = P_BT_AV_Connect(pstConnectDev);
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_Connect failed:%d \n", __FUNCTION__, drvResult);
			stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_FAIL;
			stCbData.ulDevId = ulDevId;
			stCbData.ulNumOfData = s_sCbEvtDevInfo.ulNumOfData;
			stCbData.pucBdAddr = (HUINT8 *)&pstConnectDev->ucBdAddr;
			stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
			P_BT_PostMsgToMw(&stCbData);
		}
	}
	/* PERIPHERAL Device(0x05)*//* HID */
	if((pstConnectDev->etAvailableProfile & DI_BT_PROFILE_HID) || ((pstConnectDev->ucClass[1]&0x1F) == BTM_COD_MAJOR_PERIPHERAL)) 
	{
		/* TODO: Phase 2? */
		drvResult = P_BT_HID_Connect(pstConnectDev);
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_HID_Connect failed:%d \n", __FUNCTION__, drvResult);
			stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_FAIL;
			stCbData.ulDevId = ulDevId;
			stCbData.ulNumOfData = s_sCbEvtDevInfo.ulNumOfData;
			stCbData.pucBdAddr = (HUINT8 *)&pstConnectDev->ucBdAddr;
			stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
			P_BT_PostMsgToMw(&stCbData);
		}
	}
#if 0
	else
	{
		PrintError("[%s] Not Supported Profile \n", __FUNCTION__, drvResult);
		stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_FAIL;
		stCbData.ulDevId = ulDevId;
		stCbData.ulNumOfData = s_sCbEvtDevInfo.ulNumOfData;
		stCbData.pucBdAddr = (HUINT8 *)&pstConnectDev->ucBdAddr;
		stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
		P_BT_PostMsgToMw(&stCbData);
/*		drvResult =  P_BT_Pair(ulDevId);
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_Pair failed:%d \n", __FUNCTION__, drvResult);
		}*/
	}
#endif
	return drvResult;
}

static DRV_Error P_BT_SendPinCode(HUINT32 ulDevId, HUINT8 *pucPinCode)
{
	DRV_Error drvResult = DRV_OK;
	DI_BT_DEV_INFO_t *pDevInfo;
	HUINT32 ulPinLength;
	
	pDevInfo = &s_sCbEvtDevInfo.stBtDevList[ulDevId];

	ulPinLength = VK_strlen(pucPinCode);
	if( pucPinCode[ulPinLength-1] == '\n')
	{
		pucPinCode[ulPinLength-1] == '\0';
		ulPinLength--;
	}

	drvResult = P_BT_SEC_PinCodeReply(pDevInfo->ucBdAddr, pucPinCode, ulPinLength);
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_SEC_PinCodeReply failed:%d \n", __FUNCTION__, drvResult);
	}

	return drvResult;
}

static DRV_Error P_BT_Disconnect(HUINT32 ulDevId)
{

	HINT32 drvResult = DRV_OK;
	DI_BT_DEV_INFO_t *pstDisconnectDev = NULL;
	
	pstDisconnectDev = &s_sCbEvtDevInfo.stBtDevList[ulDevId];

	pstDisconnectDev->etDevStatus = DI_BT_DEV_DISCONNECTED;

	if((pstDisconnectDev->etAvailableProfile & DI_BT_PROFILE_A2DP)||((pstDisconnectDev->ucClass[1]&0x1F) == BTM_COD_MAJOR_AUDIO))	/* Audio Device(0x04) */ 	/* A2DP */
	{
		drvResult = P_BT_AV_Disconnect(pstDisconnectDev);
		if(drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_Disconnect failed\n", __FUNCTION__);
		}
	}
	if((pstDisconnectDev->etAvailableProfile & DI_BT_PROFILE_HID) || ((pstDisconnectDev->ucClass[1]&0x1F) == BTM_COD_MAJOR_PERIPHERAL)) /* PERIPHERAL Device(0x05)*//* HID */
	{
		/* TODO */
		drvResult = P_BT_HID_Disconnect(pstDisconnectDev);
		if(drvResult != DRV_OK)
		{
			PrintError("[%s]  P_BT_HID_Disconnect failed\n", __FUNCTION__);
		}
	}


	return drvResult;
}


static DRV_Error P_BT_Remove(HUINT32 ulDevId)
{

	HINT32 drvResult = 0;
	HUINT32 i = 0;
	DI_BT_DEV_INFO_t *pstRemoveDev = NULL;
	tAPP_XML_REM_DEVICE *pstXmlDev = NULL;

	pstRemoveDev = &s_sCbEvtDevInfo.stBtDevList[ulDevId];


#if defined(DI_DEBUG)
	P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif
	if (s_sCbEvtDevInfo.ulNumOfData == 0)
	{
		PrintError("[%s] There is no device in DI DevList!\n", __FUNCTION__);
	}
	else
	{
		/* Send CMD(Remove Device) to BSA_SERVER */
		drvResult = P_BT_SEC_RemoveDevice(pstRemoveDev->ucBdAddr);
		if ( drvResult != DRV_OK)
		{
			PrintError("[%s] Error:  P_BT_SEC_RemoveDevice \n", __FUNCTION__);
		}
		else
		{
			PrintError("[%s] Complete: P_BT_SEC_RemoveDevice \n", __FUNCTION__);
			if(pstRemoveDev->etDevStatus == DI_BT_DEV_CONNECTED)
			{
				PrintError("[%s] Remove a Connecting Device \n", __FUNCTION__);
				VK_TASK_Sleep(100);	//Time to deal with disconnecting a connected device 
			}
		}

		/* Erase the device from the XML file */
		pstXmlDev = app_xml_find_dev_db(app_xml_remote_devices_db,
					APP_NUM_ELEMENTS(app_xml_remote_devices_db),
					pstRemoveDev->ucBdAddr);
		if (pstXmlDev != NULL)
		{
			pstXmlDev->in_use = FALSE;
		}
		/* Saving app_xml_remote_devices_db to .XMLfiles */
		drvResult = app_write_xml_remote_devices();
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] app_write_xml_remote_devices failed\n", __FUNCTION__);
		}
		/* Loading app_xml_rem_devices_db to stPairDevInfo */
		drvResult = P_BT_XML_GetDevList();		/* Init stPairDevInfo --> Sorting TRUE/FALSE -->Save stPairDevInfo*/
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_XML_GetDevList failed:%d \n", __FUNCTION__, drvResult);
		}

		/* Update DI Device List(s_sCbEvtDevInfo) */
		for ( i = ulDevId ; i < s_sCbEvtDevInfo.ulNumOfData-1 ; i++)
		{
			if( i < BT_NB_DISC_DEVICES-1)		// for prevent defect
			{
				VK_MEM_Memcpy(&s_sCbEvtDevInfo.stBtDevList[i], &s_sCbEvtDevInfo.stBtDevList[i+1],sizeof(DI_BT_DEV_INFO_t));
			}
		}
		VK_MEM_Memset(&s_sCbEvtDevInfo.stBtDevList[s_sCbEvtDevInfo.ulNumOfData-1], 0x0,sizeof(DI_BT_DEV_INFO_t));
#if defined(DI_DEBUG)
	P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif
		s_sCbEvtDevInfo.ulNumOfData--;
	}
#if defined(DI_DEBUG)
	P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif
	return drvResult;
}

static DRV_Error P_BT_Init(void)
{
	DRV_Error	drvResult = DRV_OK;
	HUINT8 local_bdAddr[BT_NB_BDADDR_LEN] = BT_DEFAULT_BT_ADDR;
	HUINT8 local_COD[BT_NB_CLASS_LEN] = BT_DEFAULT_BT_COD;

	drvResult = access (BT_XML_CONFIG_FILE_PATH, F_OK);
	if ( drvResult == 0 )
	{
		PrintError("[%s] %s exists!!\n",__FUNCTION__, BT_XML_CONFIG_FILE_PATH);
		drvResult = P_BT_MGT_Read_Config(&s_stBtInstance.stHostSettings);
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] Reading Local Bt Device Setting from bt_config.xml failed!\n",__FUNCTION__);
		}
	}
	else
	{
		PrintError("ERROR: %s doesn't exist!\n",BT_XML_CONFIG_FILE_PATH);
		/* Local BT Device Setting */
		s_stBtInstance.stHostSettings.bEnable = FALSE;			/* Default : Disable */
		s_stBtInstance.stHostSettings.bDiscoverable = FALSE;	/* Default : Disable by Remote Devices */
		s_stBtInstance.stHostSettings.bConnectable = FALSE;		/* Default : Disable by Remote Devices */

		bdcpy(s_stBtInstance.stHostSettings.ucBdAddr, local_bdAddr);  	/* Default : BE:EF:BE:EF:00:01*/

		snprintf(s_stBtInstance.stHostSettings.ucName, sizeof(s_stBtInstance.stHostSettings.ucName), BT_DEFAULT_BT_NAME);			/* Default : HMS-1000S */
		VK_MEM_Memcpy(s_stBtInstance.stHostSettings.ucCOD, local_COD, sizeof(s_stBtInstance.stHostSettings.ucCOD));					/* Default : {0x00, 0x04, 0x24} */

		snprintf(s_stBtInstance.stHostSettings.cRootPath, sizeof(s_stBtInstance.stHostSettings.cRootPath), BT_DEFAULT_UIPC_PATH);	/* Default : BT_DEFAULT_UIPC_PATH*/

		snprintf(s_stBtInstance.stHostSettings.ucPinCode,sizeof(s_stBtInstance.stHostSettings.ucPinCode), BT_DEFAULT_PINCODE);		/* Default : 0000 */
		s_stBtInstance.stHostSettings.cPinLength = BT_DEFAULT_PINLENGTH;		/* Default : 4 */
		/* s_stBtInstance.stHostSettings.io_cap = BSA_SEC_IO_CAP_OUT;*/			/* Default : BSA_SEC_IO_CAP_OUT *//*TODO*/
		s_stBtInstance.stHostSettings.io_cap = BSA_SEC_IO_CAP_IO;				/* Default : BSA_SEC_IO_CAP_IO *//*TODO*/
	}

#if defined(CONFIG_SHARED_COMPILE)
	drvResult = P_BT_Get_BdAddrFromCmdline(local_bdAddr);		/* Get BD Address from /proc/cmdline */
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] Error: P_BT_Get_BdAddrFromCmdline \n", __FUNCTION__);
	}
#else
	drvResult = P_BT_Get_BdAddrFromNvram(local_bdAddr);		/* Get BD Address from Nvram */
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] Error: P_BT_Get_BdAddrFromNvram \n", __FUNCTION__);
		return DRV_ERR;
	}
#endif
	if(bdcmp(local_bdAddr, bd_addr_null) != 0)
	{
		bdcpy(s_stBtInstance.stHostSettings.ucBdAddr, local_bdAddr);
	}
	else
	{
		PrintError("[%s] BdAddrFromNvram is NULL \n", __FUNCTION__);
	}
	PrintError("Setting Local BdAddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
			s_stBtInstance.stHostSettings.ucBdAddr[0], s_stBtInstance.stHostSettings.ucBdAddr[1],
			s_stBtInstance.stHostSettings.ucBdAddr[2], s_stBtInstance.stHostSettings.ucBdAddr[3],
			s_stBtInstance.stHostSettings.ucBdAddr[4], s_stBtInstance.stHostSettings.ucBdAddr[5]);

	return DRV_OK;
}

// BT_FIRMWARE_PATH "/lib/firmware/brcm"
// BT_CHIP			"BCM20702" "BCM43242" "BCM20705"
// BT_DEVNAME		"/dev/btusb0" "/dev/ttySx"

static DRV_Error P_BT_Open(void)
{
	char cSysCmd[SYSTEM_CMD_MAX];
	HINT32	vResult = 0;

	if (TRUE == P_BT_Dev_IsEnabled())
	{
		/* Run BSA Server */
		VK_MEM_Memset(cSysCmd, 0x0, SYSTEM_CMD_MAX);
		snprintf(cSysCmd, sizeof(cSysCmd), "bsa_server -u %s -p %s/%sA1.hcd -d %s -all=0 &", BT_DEFAULT_UIPC_PATH, BT_FIRMWARE_PATH, BT_CHIP, BT_DEVNAME);

		vResult = VK_SYSTEM_Command((char *)cSysCmd);
		if(vResult != VK_OK)
		{
			PrintError("[%s] VK_SYSTEM_Command(%s) failed!\n",__FUNCTION__,cSysCmd);
			return DRV_ERR;
		}
		/* BSA Server need some init time */
		VK_TASK_Sleep(BT_OPERATION_DELAY);
	}
	else
	{
		PrintError("[%s] P_BT_Dev_IsEnabled(): FALSE, \n",__FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_BT_Close(void)
{
	/* BT is built in device */
	P_BT_MGT_KillServer();

	return DRV_OK;
}


static void P_BT_Event_Task(void *pvParam)
{
	BT_MSG_t tBtMsg;
	HINT32	vResult = 0;
	DRV_Error drvResult = DRV_OK;
	UNUSED(pvParam);
	
	while(1)
	{
		vResult = VK_MSG_Receive(s_stBtInstance.ulMsg, &tBtMsg, sizeof(BT_MSG_t));
		if(vResult != VK_OK)
		{
			PrintDebug("[%s] [%d]\n", __FUNCTION__,__LINE__);
			continue;
		}
		
		switch (tBtMsg.eMsg)
		{
			case BT_MSG_INSERT:
				drvResult = P_BT_Init();
				if (drvResult != DRV_OK)
				{
					PrintError("[%s] P_BT_Init failed:%d \n", __FUNCTION__, drvResult);
				}
				break;
			case BT_MSG_EXTRACT:	/* TODO */
				drvResult = P_BT_Close();
				if (drvResult != DRV_OK)
				{
					PrintError("[%s] P_BT_Close failed:%d \n", __FUNCTION__, drvResult);
				}
				break;
	
			case BT_MSG_ENABLE:
				drvResult = P_BT_Enable();
				if (drvResult != DRV_OK)
				{
					PrintError("[%s] P_BT_Enable() failed:%d \n", __FUNCTION__, drvResult);
				}
				break;
			case BT_MSG_DISABLE:
				drvResult = P_BT_Disable();
				if (drvResult != DRV_OK)
				{
					PrintError("[%s] P_BT_Disable() failed:%d \n", __FUNCTION__, drvResult);
				}
				break;			
			case BT_MSG_SCAN_START:
				drvResult = P_BT_DISC_Start(tBtMsg.ulParam1);
				if (drvResult != DRV_OK)
				{
					PrintError("[%s] P_BT_DISC_Start failed:%d \n", __FUNCTION__, drvResult);
				}
				break;
			
			case BT_MSG_SCAN_END:
				break;
				
			case BT_MSG_CONNECT_START:
				drvResult = P_BT_Connect(tBtMsg.ulParam1);
				if (drvResult != DRV_OK)
				{
					PrintError("[%s] P_BT_Connect failed:%d \n", __FUNCTION__, drvResult);
				}
				break;
				
			case BT_MSG_CONNECT_CHECK:
				break;
				
			case BT_MSG_CONNECT_OK:
				break;

			case BT_MSG_CONNECT_FAIL:
				break;

			case BT_MSG_DISCONNECT:
				drvResult = P_BT_Disconnect(tBtMsg.ulParam1);
				if (drvResult != DRV_OK)
				{
					PrintError("[%s] P_BT_Disconnect failed:%d \n", __FUNCTION__, drvResult);
				}
				break;
			case BT_MSG_REMOVE:
				break;
			default:
				break;
		}		
 	}
}
/*******************************************************************/
/************************* Audio Functions *************************/
/*******************************************************************/
static void P_BT_AV_Capture_Callback(void *pParam, int param)
{
        NEXUS_AudioCaptureHandle capture = pParam;
	UNUSED(param);
	NEXUS_Error errCode;
	HUINT32 i;

	for(;;)
	{
		void *pBuffer;
		size_t bufferSize;
		HBOOL bUIPCInUse = FALSE;
		/* Check available buffer space */
		errCode = NEXUS_AudioCapture_GetBuffer(capture, (void **)&pBuffer, &bufferSize);
		if ( errCode )
		{
			PrintError("[%s]Error getting capture buffer\n",__FUNCTION__);
			NEXUS_AudioCapture_Stop(capture);
			return;
		}

		if ( bufferSize > 0 )
		{
			for ( i = 0 ; i < BT_AV_DEVICE_MAX;i++)
			{
				if(s_stBtAvDevInfo.stAvTable[i].bInUse && s_stBtAvDevInfo.stAvTable[i].bIsStreamActive)
				{
					bUIPCInUse = TRUE;
				}
			}
			if ( TRUE == bUIPCInUse)
			{
				P_BT_AV_SendAudioBuffer(s_stBtAvDevInfo.stUipcChannel, pBuffer, bufferSize);
			}

			/*PrintError("[%s]Data callback - Wrote %d bytes\n",__FUNCTION__, (int)bufferSize); */
			errCode = NEXUS_AudioCapture_ReadComplete(capture, bufferSize);
			if ( errCode )
			{
				PrintError("[%s] Error committing capture buffer\n", __FUNCTION__);
				NEXUS_AudioCapture_Stop(capture);
				return;
			}
		}
		else
		{
			break;
		}
	}
}

static void P_BT_AV_SampleRate_Callback(void *context, int param)
{
	NEXUS_AudioCaptureHandle capture = context;
	UNUSED(param);
	DRV_Error drvResult=DRV_OK;
	PrintDebug("[%s] entered\n", __FUNCTION__);
	
	NEXUS_AudioCaptureStatus captureStatus;
	NEXUS_AudioCapture_GetStatus(capture,&captureStatus);

	PrintDebug("[%s] sampling frequency%d\n", __FUNCTION__,captureStatus.sampleRate);

	drvResult = P_BT_AV_Stop();
	if (drvResult == DRV_OK)
	{
		PrintDebug("[%s] P_BT_AV_STOP Completed\n", __FUNCTION__);
	}

	VK_TASK_Sleep(100);

	drvResult = P_BT_AV_Start();
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_AV_Start failed:%d \n", __FUNCTION__, drvResult);
	}

}


static void P_BT_AV_Callback(tBSA_AV_EVT event, tBSA_AV_MSG *p_data)
{
	HUINT32 i,index;
	BT_AV_DEVICE_t *pstAvDevInfo = NULL;	//Current AV Devince Info
	DRV_Error drvResult = DRV_OK;
	HUINT32 ulDevId=0;
	DI_BT_CALLBACK_DATA_t stCbData;
	tAPP_XML_REM_DEVICE *pstXmlDev = NULL;
	DI_BT_REMOTE_CMD_INFO_t keydata;
	
	switch(event)
	{
	case BSA_AV_OPEN_EVT: /* Connection open */
		PrintDebug("[%s] : BSA_AV_OPEN_EVT: status%d, handle:%d\n", __FUNCTION__,p_data->open.status, p_data->open.handle);
		/* Look If this is an Open form a Connect() request */
		for(i=0; i< BT_AV_DEVICE_MAX; i++)
		{
			if(s_stBtAvDevInfo.stAvTable[i].bInUse == TRUE)
			{
				if(bdcmp(s_stBtAvDevInfo.stAvTable[i].stAvDevInfo.ucBdAddr, p_data->open.bd_addr)==0)
				{
					pstAvDevInfo = &s_stBtAvDevInfo.stAvTable[i];
					//ulDevId = P_BT_GetDIDevId(p_data->open.bd_addr);
					//pstAvDevInfo->bInUse = TRUE;
					//s_stBtAvDevInfo.stAvTable[i].handle = p_data->open.handle;
					break;
				}
			}
		}

		/* Open Request from Remtoe Device */
		if (pstAvDevInfo == NULL)
		{
			for(i=0; i< BT_AV_DEVICE_MAX; i++)
			{
				if(s_stBtAvDevInfo.stAvTable[i].bInUse == FALSE)
				{
					pstAvDevInfo = &s_stBtAvDevInfo.stAvTable[i];
					//pstAvDevInfo->bInUse = TRUE;
					break;
				}
			}
		}
		ulDevId = P_BT_GetDIDevId(p_data->open.bd_addr);
		if ( p_data ->open.status == BSA_SUCCESS)
		{
			/* check if this device is conneted */
			for (index = 0; index < BT_NB_PAIR_DEVICES; index++)
			{
				if ((s_stBtInstance.stLinkDevInfo.devs[index].bInUse == TRUE) &&
						(!bdcmp(s_stBtInstance.stLinkDevInfo.devs[index].ucBdAddr, p_data->open.bd_addr)))
				{
					break;
				}
			}
			/* If this is a new connected device */
			if (index >= BT_NB_PAIR_DEVICES)
			{
				/* Look for a free place to store dev info */
				for (index = 0; index < BT_NB_PAIR_DEVICES; index++)
				{
					if (s_stBtInstance.stLinkDevInfo.devs[index].bInUse == FALSE)
					{
						PrintDebug("[%s] New Connected device : %d\n", __FUNCTION__, index);
						s_stBtInstance.stLinkDevInfo.devs[index].bInUse = TRUE;
						bdcpy(s_stBtInstance.stLinkDevInfo.devs[index].ucBdAddr, p_data->open.bd_addr);
						s_stBtInstance.stLinkDevInfo.CurLinkCnt++;
						break;
					}
				}
			}
			if (index >= BT_NB_PAIR_DEVICES)
			{
				PrintError("[%s] : No room to save new connected device %d \n", __FUNCTION__, index);
				break;
			}

			if (pstAvDevInfo != NULL)
			{
				pstAvDevInfo->bInUse = TRUE;
				pstAvDevInfo->handle = p_data->open.handle;
				VK_MEM_Memcpy(&pstAvDevInfo->stAvDevInfo, &s_sCbEvtDevInfo.stBtDevList[ulDevId], sizeof(DI_BT_DEV_INFO_t));
			}

			s_sCbEvtDevInfo.stBtDevList[ulDevId].etDevStatus = DI_BT_DEV_CONNECTED;
			s_sCbEvtDevInfo.stBtDevList[ulDevId].etAvailableProfile |= DI_BT_PROFILE_A2DP;
			

			/* Add a trustes profile */
			app_xml_add_trusted_services_db(app_xml_remote_devices_db,
					APP_NUM_ELEMENTS(app_xml_remote_devices_db),
					p_data->open.bd_addr, DI_BT_PROFILE_A2DP);
			/* Saving app_xml_remote_devices_db to .XMLfiles */
			drvResult = app_write_xml_remote_devices();
			if(drvResult!= DRV_OK)
			{
				PrintError("[%s] fail to store remote devices database!!!\n",__FUNCTION__);
			}

#if defined(DI_DEBUG)
			P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif	
			/* Av Start(PH2) */
			drvResult = P_BT_AV_Start();
			if (drvResult != DRV_OK)
			{
				PrintError("[%s] P_BT_AV_Start failed:%d \n", __FUNCTION__, drvResult);
			}

			/* Send Evt to MW */
			stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_SUCCESS;
			stCbData.ulDevId = ulDevId;
			stCbData.ulNumOfData =  s_sCbEvtDevInfo.ulNumOfData;
			stCbData.pucBdAddr = (HUINT8 *)&p_data->open.bd_addr;
			stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
			P_BT_PostMsgToMw(&stCbData);
		}
		else
		{
			PrintDebug("[%s]Connecting to AV deivces Failed\n", __FUNCTION__);

			/* Send Evt to MW */
			stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_FAIL;
			stCbData.ulDevId = ulDevId;
			stCbData.ulNumOfData =  s_sCbEvtDevInfo.ulNumOfData;
			stCbData.pucBdAddr = (HUINT8 *)&p_data->open.bd_addr;
			stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
			P_BT_PostMsgToMw(&stCbData);
			if (pstAvDevInfo != NULL)
			{
				pstAvDevInfo->bInUse = FALSE;
			}

		}

		break;
	case BSA_AV_CLOSE_EVT:
		PrintDebug("[%s] BSA_AV_CLOSE_EVT: status%d, handle:%d\n", __FUNCTION__,p_data->close.status, p_data->close.handle);
		/* Update s_stBtAvDevInfo */
		if (p_data->close.status == BSA_SUCCESS)
		{
			for(i=0;i<BT_AV_DEVICE_MAX;i++)
			{
				if(s_stBtAvDevInfo.stAvTable[i].handle == p_data->close.handle)
				{
					s_stBtAvDevInfo.stAvTable[i].bInUse = FALSE;
					
					/* Update the DI_DEV_LIST */
					ulDevId = P_BT_GetDIDevId(s_stBtAvDevInfo.stAvTable[i].stAvDevInfo.ucBdAddr);
					s_sCbEvtDevInfo.stBtDevList[ulDevId].etDevStatus = DI_BT_DEV_DISCONNECTED;
		
					/* Av Stop(PH2) */
					drvResult = P_BT_AV_Stop();
					if (drvResult == DRV_OK)
					{
						PrintDebug("[%s] P_BT_AV_STOP Completed\n", __FUNCTION__);
					}
					
					stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_FAIL;	//DISCONNECT by Remote상황에서 DI_BT_NOTIFY_EVT_CONNECT_FAIL로 올리는 걸로 수정
					stCbData.ulDevId = ulDevId;
					stCbData.ulNumOfData =  s_sCbEvtDevInfo.ulNumOfData;
					stCbData.pucBdAddr = (HUINT8 *)&s_stBtAvDevInfo.stAvTable[i].stAvDevInfo.ucBdAddr;
					stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
					P_BT_PostMsgToMw(&stCbData);

					/* Finding the Device in stLinkDevInfo */
					/* Then, Change Status */
					for (index = 0; index < BT_NB_PAIR_DEVICES; index++)
					{
						if ((s_stBtInstance.stLinkDevInfo.devs[index].bInUse == TRUE) &&
								(bdcmp(s_stBtInstance.stLinkDevInfo.devs[index].ucBdAddr, s_stBtAvDevInfo.stAvTable[i].stAvDevInfo.ucBdAddr) == 0))
						{
							s_stBtInstance.stLinkDevInfo.devs[index].bInUse = FALSE;
							s_stBtInstance.stLinkDevInfo.CurLinkCnt--;
							break;
						}
					}
					break;
					
				}
			}
		}
		else
		{
			PrintError("BSA_AvClose error Status %d\n", p_data->close.status);
		}
		break;
	case BSA_AV_START_EVT:
		PrintDebug("[%s] BSA_AV_START_EVT: status%d\n", __FUNCTION__,p_data->start.status);

		/* Audio Capture Start */
		P_BT_NEXUS_AudioCapture_Start();

		/* Update s_stBtAvDevInfo */
		if (p_data->start.status == BSA_SUCCESS)
		{
			for(i=0;i<BT_AV_DEVICE_MAX;i++)
			{
				if(s_stBtAvDevInfo.stAvTable[i].bInUse == TRUE)
				{
					s_stBtAvDevInfo.stAvTable[i].bIsStreamActive = TRUE;
				}
			}
		}
		break;
	case BSA_AV_STOP_EVT:
		PrintDebug("[%s] BSA_AV_STOP_EVT: status%d\n", __FUNCTION__,p_data->stop.pause);

		/* Audio Capture Stop */
		NEXUS_AudioCapture_Stop(hCapture);

		/* Update s_stBtAvDevInfo */
		if (p_data->stop.pause == TRUE)
		{
			for( i = 0 ; i < BT_AV_DEVICE_MAX;i++)
			{
				if(s_stBtAvDevInfo.stAvTable[i].bInUse == TRUE)
				{
					s_stBtAvDevInfo.stAvTable[i].bIsStreamActive = FALSE;
				}
			}
		}
		break;
	case BSA_AV_RC_OPEN_EVT:
		PrintDebug("[%s] BSA_AV_RC_OPEN_EVT: status%d\n", __FUNCTION__, p_data->rc_open.status);
		/* Look If this is an Open form a Connect() request */
		for(i=0; i< BT_AV_DEVICE_MAX; i++)
		{
			if(s_stBtAvDevInfo.stAvTable[i].bInUse == TRUE)
			{
				if(bdcmp(s_stBtAvDevInfo.stAvTable[i].stAvDevInfo.ucBdAddr, p_data->rc_open.peer_addr)==0)
				{
					pstAvDevInfo = &s_stBtAvDevInfo.stAvTable[i];
					//ulDevId = P_BT_GetDIDevId(p_data->rc_open.peer_addr);
					break;
				}
			}
		}
		/* Open Request from Remtoe Device */
		if (pstAvDevInfo == NULL)
		{
			for(i=0; i< BT_AV_DEVICE_MAX; i++)
			{
				if(s_stBtAvDevInfo.stAvTable[i].bInUse == FALSE)
				{
					pstAvDevInfo = &s_stBtAvDevInfo.stAvTable[i];

					pstXmlDev = app_xml_find_dev_db(app_xml_remote_devices_db,
							APP_NUM_ELEMENTS(app_xml_remote_devices_db),
							p_data->rc_open.peer_addr);
					if ( pstXmlDev != NULL)
					{
						VK_MEM_Memcpy(pstAvDevInfo->stAvDevInfo.ucBdAddr, pstXmlDev->bd_addr,BD_ADDR_LEN);
						VK_MEM_Memcpy(pstAvDevInfo->stAvDevInfo.ucName, pstXmlDev->name, BD_NAME_LEN);
						drvResult = P_BT_AV_Connect(&pstAvDevInfo->stAvDevInfo);
						if (drvResult != DRV_OK)
						{
							PrintError("[%s] P_BT_AV_Connect failed:%d \n", __FUNCTION__, drvResult);
						}
					}
					break;
				}
			}
		}
		ulDevId = P_BT_GetDIDevId(p_data->rc_open.peer_addr);
		if(p_data->rc_open.status == BSA_SUCCESS)
		{
			/* Update the DI_DEV_LIST */
			s_sCbEvtDevInfo.stBtDevList[ulDevId].etAvailableProfile |= DI_BT_PROFILE_AVRCP;

			/* Add a trustes profile */
			app_xml_add_trusted_services_db(app_xml_remote_devices_db,
					APP_NUM_ELEMENTS(app_xml_remote_devices_db),
					p_data->rc_open.peer_addr, DI_BT_PROFILE_AVRCP);
			/* Saving app_xml_remote_devices_db to .XMLfiles */
			drvResult = app_write_xml_remote_devices();
			if(drvResult!= DRV_OK)
			{
				PrintError("[%s] fail to store remote devices database!!!\n",__FUNCTION__);
			}
		}
	
		break;
	case BSA_AV_RC_CLOSE_EVT:
		PrintDebug("[%s] BSA_AV_RC_CLOSE_EVT: handle%d\n", __FUNCTION__, p_data->rc_close.rc_handle);
		break;
	case BSA_AV_REMOTE_CMD_EVT:
		PrintDebug("[%s] BSA_AV_REMOTE_CMD_EVT: key:%d State:%d\n", __FUNCTION__, p_data->remote_cmd.rc_id, p_data->remote_cmd.key_state);
		/* For AVRCP, TODO */
		for ( i = 0; i < (sizeof(KeyLookupTable)/sizeof(KeyLookupTable[0])); i++)
		{
			if ( KeyLookupTable[i].ucBsaKey == p_data->remote_cmd.rc_id)
			{
				keydata.ulKey = KeyLookupTable[i].ucLinuxKey;
				keydata.bPressed = (p_data->remote_cmd.key_state == 0);

				stCbData.eEvent = DI_BT_NOTIFY_EVT_REMOTE_CMD;
				stCbData.ulDevId = 0;
				stCbData.ulNumOfData =  0;
				stCbData.pucBdAddr = NULL;
				stCbData.pvEventData = (void *)&keydata;
				P_BT_PostMsgToMw(&stCbData);
				break;
			}
		}
		if ( i == (sizeof(KeyLookupTable) / sizeof(KeyLookupTable[0])))
		{
			PrintError("[%s] Received unhandled input %d", __FUNCTION__, p_data->remote_cmd.rc_id);
		}
		break;
	case BSA_AV_REMOTE_RSP_EVT:
		PrintDebug("[%s] BSA_AV_REMOTE_RSP_EVT: handle%d\n", __FUNCTION__, p_data->remote_rsp.rc_handle);
		break;
	case BSA_AV_META_MSG_EVT:
		PrintDebug("[%s] BSA_AV_META_MSG_EVT: status%d\n", __FUNCTION__, p_data->meta_msg.rc_handle);
		break;
	default:
		PrintError("[%s] Unkown Event %d\n", __FUNCTION__, event);
		break;
	}
}

static DRV_Error P_BT_AV_Enable(void)
{
	tBSA_STATUS status;
	tBSA_AV_ENABLE stParams_enable;
	BSA_AvEnableInit(&stParams_enable);

	stParams_enable.features = BT_AV_FEAT_MASK;
	stParams_enable.p_cback = P_BT_AV_Callback;

	status = BSA_AvEnable(&stParams_enable);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_AvEnable failed status:%d \n", __FUNCTION__, status);
		return DRV_ERR;
	}
	return DRV_OK;
}
static DRV_Error P_BT_AV_Disable(void)
{
	tBSA_STATUS status;
	tBSA_AV_DISABLE stParams_disable;
	BSA_AvDisableInit(&stParams_disable);
	status = BSA_AvDisable(&stParams_disable);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_AvDisable failed status:%d \n", __FUNCTION__, status);
		return DRV_ERR;
	}
	return DRV_OK;
}

static DRV_Error P_BT_AV_Register(void)
{
	tBSA_AV_REGISTER stParams_register;
	HUINT32 index;
	
	VK_MEM_Memset(&stParams_register, 0x0, sizeof(tBSA_AV_REGISTER));

	/* Registser */
	for (index = 0 ; index < BT_AV_DEVICE_MAX;index++)
	{
		BSA_AvRegisterInit(&stParams_register);

		BSA_AvRegister(&stParams_register);

		s_stBtAvDevInfo.stAvTable[index].handle = stParams_register.handle;

		if(s_stBtAvDevInfo.stUipcChannel != stParams_register.uipc_channel)
		{
			s_stBtAvDevInfo.stUipcChannel = stParams_register.uipc_channel;
			PrintDebug("[%s] stUIPCChanel = %d\n", __FUNCTION__, s_stBtAvDevInfo.stUipcChannel);
			if (UIPC_Open(s_stBtAvDevInfo.stUipcChannel, NULL) == FALSE)
			{
				PrintError("[%s]Unable to open UIPC channel\n", __FUNCTION__);
				return DRV_ERR;
			}
		}
	}
	return DRV_OK;
}

static DRV_Error P_BT_AV_Deregister(HUINT8 ucHandle)
{
	tBSA_STATUS status;
	tBSA_AV_DEREGISTER stParams_deregister;

	BSA_AvDeregisterInit(&stParams_deregister);
	stParams_deregister.handle = ucHandle;
	status = BSA_AvDeregister(&stParams_deregister);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_AvDeregister failed status:%d \n", __FUNCTION__, status);
		return DRV_ERR;
	}
	return DRV_OK;
}

static DRV_Error P_BT_AV_Close(HUINT8 ucHandle)
{
	tBSA_STATUS status;
	tBSA_AV_CLOSE stParams_close;
	BSA_AvCloseInit(&stParams_close);
	stParams_close.handle = ucHandle;
	status = BSA_AvClose(&stParams_close);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_AvClose failed status:%d \n", __FUNCTION__, status);
		return DRV_ERR;
	}
	return DRV_OK;
}

static DRV_Error P_BT_AV_Init(void)
{
	DRV_Error drvResult = DRV_OK;

	PrintDebug("%s(): Entered!\n", __FUNCTION__);

	VK_MEM_Memset(&s_stBtAvDevInfo, 0x0, sizeof(BT_AV_t));
	
	drvResult = P_BT_AV_Enable();
	if(drvResult!= DRV_OK)
	{
		PrintError("[%s] P_BT_AV_Enable failed\n",__FUNCTION__);
		return DRV_ERR;
	}

	drvResult = P_BT_AV_Register();/* Register &UIPC Open */
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_AV_Register failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Getting the opened audioCaptureHandle to hCapture */
	drvResult = P_BT_NEXUS_AudioCapture_GetHandle(0);
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_NEXUS_AudioCapture_GetHandle \n", __FUNCTION__);
		return DRV_ERR;
	}

	return drvResult;
}

static DRV_Error P_BT_AV_Deinit(void)
{
	HUINT32 index;
	DRV_Error drvResult = DRV_OK;

	for (index = 0 ; index < BT_AV_DEVICE_MAX;index++)
	{
		if(s_stBtAvDevInfo.stAvTable[index].bInUse == TRUE)
		{
			PrintDebug("[%s] Disconnecting Av Device : %d", __FUNCTION__, s_stBtAvDevInfo.stAvTable[index].handle);

			drvResult = P_BT_AV_Close(s_stBtAvDevInfo.stAvTable[index].handle);
			if (drvResult != DRV_OK)
			{
				PrintError("[%s] P_BT_AV_Close failed:%d\n", __FUNCTION__, drvResult);
			}
		}
		drvResult = P_BT_AV_Deregister(s_stBtAvDevInfo.stAvTable[index].handle);
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_Deregister failed:%d\n", __FUNCTION__, drvResult);
		}

	}
	UIPC_Close(s_stBtAvDevInfo.stUipcChannel);

	drvResult = P_BT_AV_Disable();
	if(drvResult!= DRV_OK)
	{
		PrintError("[%s] P_BT_AV_Disable failed\n",__FUNCTION__);
	}

	return drvResult;
}

static DRV_Error P_BT_AV_Connect(DI_BT_DEV_INFO_t *pstAvDevInfo)
{
	HUINT32 i;
	tBSA_STATUS status;
	tBSA_AV_OPEN stParams_open;

	if ( pstAvDevInfo == NULL)
	{
		PrintError("[%s] wrong parameter\n",__FUNCTION__);
		return  DRV_ERR;
	}

	status = BSA_AvOpenInit(&stParams_open);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_AvOpenInit failed status:%d \n", __FUNCTION__, status);
		return DRV_ERR;
	}

	for( i = 0 ; i < BT_AV_DEVICE_MAX; i++)
	{
		if(s_stBtAvDevInfo.stAvTable[i].bInUse == FALSE)
		{
			bdcpy(stParams_open.bd_addr, pstAvDevInfo->ucBdAddr);
			stParams_open.handle = s_stBtAvDevInfo.stAvTable[i].handle;
			stParams_open.use_rc = BT_AV_USE_RC;

			VK_MEM_Memcpy(&s_stBtAvDevInfo.stAvTable[i].stAvDevInfo, pstAvDevInfo, sizeof(DI_BT_DEV_INFO_t));
			PrintDebug("[%s]Call BSA_AvOpen on handle [%d]\n",__FUNCTION__, stParams_open.handle);
			status = BSA_AvOpen(&stParams_open);
			if (status != BSA_SUCCESS)
			{
				PrintError("[%s]BSA_AvOpen failed status:%d \n", __FUNCTION__, status);
				return DRV_ERR;
			}
			PrintDebug("[%s]Connecting to AV deivces:%s\n", __FUNCTION__, pstAvDevInfo->ucName);

			break;
		}
	}
	if (i == BT_AV_DEVICE_MAX)
	{
		PrintError("[%s]Maximum AV device number in used\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;

}


static DRV_Error P_BT_AV_Disconnect(DI_BT_DEV_INFO_t *pstAvDevInfo)
{
	HUINT32 i;
	DRV_Error drvResult = DRV_OK;
	HUINT8 ucCurHandle=0;

	for( i = 0 ; i < BT_AV_DEVICE_MAX; i++)
	{
		if(bdcmp(s_stBtAvDevInfo.stAvTable[i].stAvDevInfo.ucBdAddr, pstAvDevInfo->ucBdAddr) ==0)
		{
			ucCurHandle = s_stBtAvDevInfo.stAvTable[i].handle;
			PrintDebug("[%s] Found Av Device to Disconnect [handle %d]\n",__FUNCTION__, ucCurHandle);
			break;
		}
	}
	if ( i < BT_AV_DEVICE_MAX)
	{
		drvResult = P_BT_AV_Close(ucCurHandle);
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_Close failed:%d\n", __FUNCTION__, drvResult);
		}
	}

	return drvResult;

}


static DRV_Error P_BT_AV_Start(void)
{
	tBSA_STATUS status;
	tBSA_AV_START stParams_start;
	NEXUS_AudioCaptureStatus captureStatus;

	BSA_AvStartInit(&stParams_start);
	stParams_start.media_feeding.format = BSA_AV_CODEC_PCM;
	stParams_start.media_feeding.cfg.pcm.num_channel = 2;
	stParams_start.media_feeding.cfg.pcm.bit_per_sample = 16;

	NEXUS_AudioCapture_GetStatus(hCapture,&captureStatus);
	stParams_start.media_feeding.cfg.pcm.sampling_freq = captureStatus.sampleRate;
	PrintError("[%s] sampling frequency%d\n", __FUNCTION__, stParams_start.media_feeding.cfg.pcm.sampling_freq);

	UIPC_Ioctl(s_stBtAvDevInfo.stUipcChannel, UIPC_WRITE_NONBLOCK, NULL);
	
	stParams_start.feeding_mode = BSA_AV_FEEDING_SYNCHRONOUS;
	stParams_start.latency = 40;

	status = BSA_AvStart(&stParams_start);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_AvStart failed status:%d\n", __FUNCTION__, status);
	}

	return DRV_OK;
}


static DRV_Error P_BT_AV_Stop(void)
{
	tBSA_STATUS status;
	tBSA_AV_STOP stParams_stop;
	

	status = BSA_AvStopInit(&stParams_stop);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_AvStopInit failed status:%d \n", __FUNCTION__, status);
		return DRV_ERR;
	}

	stParams_stop.pause = FALSE;
	status = BSA_AvStop(&stParams_stop);
	if (status != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_AvStop failed status:%d\n", __FUNCTION__, status);
		return DRV_ERR;
	}

	return DRV_OK;


}

static DRV_Error P_BT_AV_SendAudioBuffer(tUIPC_CH_ID stUipcChannel, void *pvBuffer, HUINT32 ulLength)
{
	HUINT8 *pucTempBuf = (HUINT8 *)pvBuffer;
	HUINT32 ulSent = 0;
	HUINT32 ulWriteLen = (ulLength <= BSA_AV_UIPC_BUFFER_SIZE) ? ulLength : BSA_AV_UIPC_BUFFER_SIZE;
	DRV_Error drvResult=DRV_OK;

	while(ulSent < ulLength)
	{
		if( UIPC_Send(stUipcChannel,0, (HUINT8 *) pucTempBuf, ulWriteLen) != TRUE)
		{
			PrintDebug("[%s] UIPC_Send could not send data length %d\n", __FUNCTION__, ulWriteLen);
			drvResult = DRV_ERR;
			break;
		}
		pucTempBuf +=BSA_AV_UIPC_BUFFER_SIZE;
		ulSent += ulWriteLen;
		ulWriteLen = ((ulLength - ulSent) <= BSA_AV_UIPC_BUFFER_SIZE)? (ulLength - ulSent) : BSA_AV_UIPC_BUFFER_SIZE;
	}
	return drvResult;

}

static DRV_Error P_BT_HID_Init(void)
{
	DRV_Error drvResult = DRV_OK;

	PrintDebug("%s(): Entered!\n", __FUNCTION__);

	app_hh_init();

	app_hh_event_cback_register((tAPP_HH_EVENT_CBACK *) P_BT_HID_Callback);
	app_hh_uipc_cback_register((tAPP_HH_CUSTOM_UIPC_CBACK *) P_BT_HID_UIPC_Callback);

	app_hh_start();

	return drvResult;
}

static DRV_Error P_BT_HID_Deinit(void)
{
	DRV_Error drvResult = DRV_OK;

	PrintDebug("%s(): Entered!\n", __FUNCTION__);

	app_hh_exit();

	return drvResult;
}

static DRV_Error P_BT_HID_Connect(DI_BT_DEV_INFO_t *pstHidDevInfo)
{
	DRV_Error drvResult = DRV_OK;
	tBSA_STATUS bsastatus;
	tBSA_HH_OPEN hh_open_param;
	PrintDebug("%s(): Entered!\n", __FUNCTION__);

	if ( pstHidDevInfo == NULL)
	{
		PrintError("[%s] wrong parameter\n",__FUNCTION__);
		return  DRV_ERR;
	}

	BSA_HhOpenInit(&hh_open_param);
	bdcpy(hh_open_param.bd_addr, pstHidDevInfo->ucBdAddr);
	hh_open_param.mode = BSA_HH_PROTO_RPT_MODE; //TODO
	hh_open_param.sec_mask = BSA_SEC_NONE; //TODO

	bsastatus = BSA_HhOpen(&hh_open_param);
	if (bsastatus != BSA_SUCCESS)
	{
		PrintError("[%s] BSA_HhOpen failed: %d", __FUNCTION__, bsastatus);
	}
	else
	{
		PrintError("[%s]Connecting to HID device:%s", __FUNCTION__, pstHidDevInfo->ucName);
		/* XML Database update (Service) will be done upon reception of HH OPEN event */
        /* Read the Remote device XML file to have a fresh view */

		if (app_read_xml_remote_devices() < 0)
		{
			/*
			 * * Must reset the hh device database if bt_devices.xml does not exist or
			 * * the descriptor will not be submitted to the kernel (issues seen if user
			 * * deletes bt_devices.xml but keeps bt_hh_devices.xml)
			 * */
			app_hh_db_remove_db();
		}

		app_xml_update_name_db(app_xml_remote_devices_db,
				APP_NUM_ELEMENTS(app_xml_remote_devices_db), pstHidDevInfo->ucBdAddr, pstHidDevInfo->ucName);

		app_xml_update_cod_db(app_xml_remote_devices_db,
				APP_NUM_ELEMENTS(app_xml_remote_devices_db), pstHidDevInfo->ucBdAddr, pstHidDevInfo->ucClass);

		/* Update database => write on disk */
		app_write_xml_remote_devices();
	}
	return drvResult;
}

static DRV_Error P_BT_HID_Disconnect(DI_BT_DEV_INFO_t *pstHidDevInfo)
{
	DRV_Error drvResult = DRV_OK;
	tAPP_HH_DEVICE *pstAppHHDev;
	tBSA_HH_CLOSE hh_close_param;
	tBSA_STATUS bsastatus;

	PrintDebug("%s(): Entered!\n", __FUNCTION__);

	if ( pstHidDevInfo == NULL)
	{
		PrintError("[%s] wrong parameter\n",__FUNCTION__);
		return  DRV_ERR;
	}

	pstAppHHDev = app_hh_pdev_find_by_bdaddr(pstHidDevInfo->ucBdAddr);
	if ( pstAppHHDev == NULL)
	{
		//TODO
		return DRV_ERR;
	}
	else
	{
		BSA_HhCloseInit(&hh_close_param);
		hh_close_param.handle = pstAppHHDev->handle;
		bsastatus = BSA_HhClose(&hh_close_param);
		if (bsastatus != BSA_SUCCESS)
		{
			PrintError("[%s]Unable to disconnect HID device status:%d", __FUNCTION__,bsastatus);
			return DRV_ERR;
		}
		else
		{
			PrintDebug("[%s]Disconnecting from HID device...",__FUNCTION__);
			drvResult = DRV_OK;
		}
	}

	return drvResult;
}

//BOOLEAN (tAPP_HH_EVENT_CBACK(tBSA_HH_EVT event, tBSA_HH_MSG *p_data)
static HBOOL P_BT_HID_Callback(tBSA_HH_EVT event, tBSA_HH_MSG *p_data)
{
	int index;
	tAPP_HH_DEVICE *pstAppHHDev;
	HUINT32 ulDevId;
	DI_BT_CALLBACK_DATA_t stCbData;

	switch (event)
	{
		case BSA_HH_OPEN_EVT:
			PrintError("[%s]BSA_HH_OPEN_EVT: status:%d, handle:%d mode:%s (%d)\n", __FUNCTION__,
					p_data->open.status, p_data->open.handle,
					p_data->open.mode==BTA_HH_PROTO_RPT_MODE?"Report":"Boot", 
					p_data->open.mode);
			PrintError("[%s]BdAddr :%02X-%02X-%02X-%02X-%02X-%02X\n", __FUNCTION__,
				p_data->open.bd_addr[0], p_data->open.bd_addr[1], p_data->open.bd_addr[2],
				p_data->open.bd_addr[3], p_data->open.bd_addr[4], p_data->open.bd_addr[5]);
			if (p_data->open.status == BSA_SUCCESS)
			{
				ulDevId = P_BT_GetDIDevId(p_data->open.bd_addr);
				/* check if this device is conneted */
				for (index = 0; index < BT_NB_PAIR_DEVICES; index++)
				{
					if ((s_stBtInstance.stLinkDevInfo.devs[index].bInUse == TRUE) &&
							(!bdcmp(s_stBtInstance.stLinkDevInfo.devs[index].ucBdAddr, p_data->open.bd_addr)))
					{
						break;
					}
				}
				/* If this is a new connected device */
				if (index >= BT_NB_PAIR_DEVICES)
				{
					/* Look for a free place to store dev info */
					for (index = 0; index < BT_NB_PAIR_DEVICES; index++)
					{
						if (s_stBtInstance.stLinkDevInfo.devs[index].bInUse == FALSE)
						{
							PrintDebug("[%s] New Connected device : %d\n", __FUNCTION__, index);
							s_stBtInstance.stLinkDevInfo.devs[index].bInUse = TRUE;
							bdcpy(s_stBtInstance.stLinkDevInfo.devs[index].ucBdAddr, p_data->open.bd_addr);
							s_stBtInstance.stLinkDevInfo.CurLinkCnt++;
							break;
						}
					}
				}
				if (index >= BT_NB_PAIR_DEVICES)
				{
					PrintError("[%s] : No room to save new connected device %d \n", __FUNCTION__, index);
					break;
				}
				s_sCbEvtDevInfo.stBtDevList[ulDevId].etDevStatus = DI_BT_DEV_CONNECTED;
				s_sCbEvtDevInfo.stBtDevList[ulDevId].etAvailableProfile |= DI_BT_PROFILE_HID;
	
#if defined(DI_DEBUG)
				P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif	
		
				/* Send Evt to MW */
				stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_SUCCESS;
				stCbData.ulDevId = ulDevId;
				stCbData.ulNumOfData =  s_sCbEvtDevInfo.ulNumOfData;
				stCbData.pucBdAddr = (HUINT8 *)&p_data->open.bd_addr;
				stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
				P_BT_PostMsgToMw(&stCbData);
			}
			break;
			/* coverity[MISSING BREAK] False-positive: Unplug event is handled as a close event */
		case BSA_HH_VC_UNPLUG_EVT:
			PrintError("[%s]BSA_HH_VC_UNPLUG_EVT is handled like a close event, redirecting\n", __FUNCTION__);
		case BSA_HH_CLOSE_EVT:
			PrintError("[%s]BSA_HH_CLOSE_EVT: status:%d  handle:%d\n", __FUNCTION__,
					p_data->close.status, p_data->close.handle);

			if (p_data->close.status == BSA_SUCCESS)
			{
				pstAppHHDev = app_hh_pdev_find_by_handle(p_data->close.handle);
				if ( pstAppHHDev !=NULL )
				{
					ulDevId = P_BT_GetDIDevId(pstAppHHDev->bd_addr);
					s_sCbEvtDevInfo.stBtDevList[ulDevId].etDevStatus = DI_BT_DEV_DISCONNECTED;

					stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_FAIL;
					stCbData.ulDevId = ulDevId;
					stCbData.ulNumOfData =  s_sCbEvtDevInfo.ulNumOfData;
					stCbData.pucBdAddr = (HUINT8 *)&pstAppHHDev->bd_addr;
					stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
					P_BT_PostMsgToMw(&stCbData);

					/* Finding the Device in stLinkDevInfo */
					/* Then, Change Status */
					for (index = 0; index < BT_NB_PAIR_DEVICES; index++)
					{
						if ((s_stBtInstance.stLinkDevInfo.devs[index].bInUse == TRUE) &&
								(bdcmp(s_stBtInstance.stLinkDevInfo.devs[index].ucBdAddr, pstAppHHDev->bd_addr ) == 0))
						{
							s_stBtInstance.stLinkDevInfo.devs[index].bInUse = FALSE;
							s_stBtInstance.stLinkDevInfo.CurLinkCnt--;
							break;
						}
					}
				}
			}
#if defined(DI_DEBUG)
				P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif	
			break;

		case BSA_HH_GET_DSCPINFO_EVT:
			PrintError("[%s]BSA_HH_GET_DSCPINFO_EVT: status:%d handle:%d\n", __FUNCTION__,
					p_data->dscpinfo.status, p_data->dscpinfo.handle);
			
			if (p_data->dscpinfo.status == BSA_SUCCESS)
			{
				PrintError("[%s]DscpInfo: VID/PID (hex) = %04X/%04X\n", __FUNCTION__,
						p_data->dscpinfo.peerinfo.vendor_id,
						p_data->dscpinfo.peerinfo.product_id);
				PrintError("[%s]DscpInfo: version = %d\n", __FUNCTION__, p_data->dscpinfo.peerinfo.version);
				PrintError("[%s]DscpInfo: ssr_max_latency = %d ssr_min_tout = %d\n", __FUNCTION__,
						p_data->dscpinfo.peerinfo.ssr_max_latency,
						p_data->dscpinfo.peerinfo.ssr_min_tout);
				PrintError("[%s]DscpInfo: supervision_tout = %d\n", __FUNCTION__, p_data->dscpinfo.peerinfo.supervision_tout);
				PrintError("[%s]DscpInfo (descriptor len:%d):\n", __FUNCTION__, p_data->dscpinfo.dscpdata.length);
			}
			break;
		case BSA_HH_GET_REPORT_EVT:
			PrintError("[%s]BSA_HH_GET_REPORT_EVT: status:%d handle:%d len:%d\n", __FUNCTION__,
					p_data->get_report.status, p_data->get_report.handle,
					p_data->get_report.report.length);
			if (p_data->get_report.report.length > 0)
			{
				PrintError("[%s]Report ID: [%02x]\n", __FUNCTION__, p_data->get_report.report.data[0]);
			}
			break;

		case BSA_HH_SET_REPORT_EVT:
			PrintError("[%s]BSA_HH_SET_REPORT_EVT: status:%d handle:%d\n", __FUNCTION__,
					p_data->set_report.status, p_data->set_report.handle);
			break;

		case BSA_HH_GET_PROTO_EVT:
			PrintError("[%s]BSA_HH_GET_PROTO_EVT: status:%d handle:%d mode:%d\n", __FUNCTION__,
					p_data->get_proto.status, p_data->get_proto.handle,
					p_data->get_proto.mode);
			if (p_data->get_proto.status == BSA_SUCCESS)
			{
				/* Boot mode setting here is required to pass HH PTS test HOS_HID_BV_09_C*/
				app_hh_change_proto_mode(p_data->get_proto.handle,
						BSA_HH_PROTO_BOOT_MODE);
			}
			break;

		case BSA_HH_SET_PROTO_EVT:
			PrintError("[%s]BSA_HH_SET_PROTO_EVT: status:%d, handle:%d\n", __FUNCTION__,
					p_data->set_proto.status, p_data->set_proto.handle);
			break;

		case BSA_HH_GET_IDLE_EVT:
			PrintError("[%s]BSA_HH_GET_IDLE_EVT: status:%d, handle:%d, idle:%d\n", __FUNCTION__,
					p_data->get_idle.status, p_data->get_idle.handle, p_data->get_idle.idle);
			break;

		case BSA_HH_SET_IDLE_EVT:
			PrintError("[%s]BSA_HH_SET_IDLE_EVT: status:%d, handle:%d\n", __FUNCTION__,
					p_data->set_idle.status, p_data->set_idle.handle);
			break;

		case BSA_HH_MIP_START_EVT:
			PrintError("[%s]BSA_HH_MIP_START_EVT\n", __FUNCTION__);
			app_hh_cb.mip_handle = p_data->mip_start.handle;
			break;

		case BSA_HH_MIP_STOP_EVT:
			PrintError("[%s]BSA_HH_MIP_STOP_EVT\n", __FUNCTION__);
			app_hh_cb.mip_handle = p_data->mip_stop.handle;
			break;

		default:
			PrintError("[%s]bad event:%d\n", __FUNCTION__, event);
			break;
	}

	return FALSE;	//FALSE, for using app_hh_cback in app_hh.c
					//TURE, not using it
}

//BOOLEAN (tAPP_HH_CUSTOM_UIPC_CBACK(BT_HDR *p_msg)
static HBOOL P_BT_HID_UIPC_Callback(BT_HDR *p_msg)
{
	tBSA_HH_UIPC_REPORT *p_uipc_report = (tBSA_HH_UIPC_REPORT *)p_msg;
	HINT8 szBdAddr[20];

	P_BT_ConvertHex2Str(p_uipc_report->report.bd_addr, szBdAddr);

	PrintDebug("[%s] Handle: %d, BdAddr:%s\n", __FUNCTION__, p_uipc_report->report.handle, szBdAddr);
	PrintDebug("[%s] Mode : %d, SubClass:0x%x, CtryCode:%d, len:%d, event: %d\n", __FUNCTION__,
			p_uipc_report->report.mode,
			p_uipc_report->report.sub_class,
			p_uipc_report->report.ctry_code,
			p_uipc_report->report.report_data.length,
			p_uipc_report->hdr.event);

	return FALSE;	//FALSE, for using app_hh_uipc_cback in app_hh.c
					//TURE, not using it
}

static HBOOL P_BT_Dev_IsEnabled(void)
{
	int fd=0;
	HBOOL bEnabled = FALSE;

	fd = open(BT_DEVNAME, O_RDWR);
	if ( fd >= 0)
	{
		bEnabled = TRUE;
	}
	else
	{
		return FALSE;
	}
	close(fd);
	return bEnabled;
}

static void P_BT_USBDev_HotplugParse(char *pBuffer)
{
	DRV_Error drvResult = DRV_OK;
	DI_BT_CALLBACK_DATA_t stCbData;
	HUINT8 local_bdAddr[BT_NB_BDADDR_LEN] = {0,};

	VK_MEM_Memset( &stCbData, 0x0, sizeof(DI_BT_CALLBACK_DATA_t));

	if (strncmp(pBuffer,"add@",4) == 0)
	{
		stCbData.eEvent = DI_BT_NOTIFY_EVT_INSERTED;
		stCbData.pvEventData = (void *) &s_stBtInstance.stHostSettings.bEnable;
	}

	if (strncmp(pBuffer,"remove@",7) == 0) 
	{
		stCbData.eEvent = DI_BT_NOTIFY_EVT_EXTRACTED;
	}

	if (strstr(pBuffer, "/usb/btusb0"))		/*  btusb0 */
	{
		PrintDebug("[%s] '/usb/btusb0' Found in %s\n", __FUNCTION__, pBuffer);

#if defined(CONFIG_SHARED_COMPILE)
		drvResult = P_BT_Get_BdAddrFromTempNvramDir(local_bdAddr);
		if(drvResult != DRV_OK)
		{
			PrintError("[%s] Getting from TempNvramDir(/tmp/nvram/btmac) failed\n", __FUNCTION__);
		}
		else
		{
			if(bdcmp(local_bdAddr, bd_addr_null) != 0)
			{
				bdcpy(s_stBtInstance.stHostSettings.ucBdAddr, local_bdAddr);
			}
			else
			{
				PrintError("[%s] BdAddrFromTempNvramDir is NULL \n", __FUNCTION__);
			}
		}
		PrintError("[%s] %s Local BdAddr: %02x:%02x:%02x:%02x:%02x:%02x\n", __FUNCTION__, s_stBtInstance.stHostSettings.ucName,
				s_stBtInstance.stHostSettings.ucBdAddr[0], s_stBtInstance.stHostSettings.ucBdAddr[1],
				s_stBtInstance.stHostSettings.ucBdAddr[2], s_stBtInstance.stHostSettings.ucBdAddr[3],
				s_stBtInstance.stHostSettings.ucBdAddr[4], s_stBtInstance.stHostSettings.ucBdAddr[5]);

#endif
		while(1)
		{
			if(s_stBtInstance.stCallbackInfo[stCbData.eEvent].pfnCallback != NULL)
			{
				(*s_stBtInstance.stCallbackInfo[stCbData.eEvent].pfnCallback)(&stCbData);
				break;
			}
			else
			{
				PrintDebug("[%s] BT Callback is NULL [%s] ! \n",__FUNCTION__, P_BT_GetEvtText(stCbData.eEvent));
			}
			VK_TASK_Sleep(1000);
		}
	}

	return;
}

static void P_BT_USBDev_HotplugTask(void *pvParam)	/* Getting from drv_hotplug.c */
{
	int ns = -1;
	struct sockaddr_nl sa;
	char buf[1024];
	struct iovec iov = { buf, sizeof(buf) };
	struct msghdr msg = { &sa, sizeof(sa), &iov, 1, NULL, 0, 0};
	int len = 1024;
	UNUSED(pvParam);

	VK_MEM_Memset(&sa, 0, sizeof(sa));
	sa.nl_family = AF_NETLINK;
	sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

	ns = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
	if (ns < 0) 
	{
		PrintError("[%s] socket()",__FUNCTION__);
		return;
	}

	if (bind(ns, (struct sockaddr *)&sa, sizeof(sa))) 
	{
		PrintError("[%s] bind()",__FUNCTION__);
		close(ns);
		return;
	}

	while (1)
	{
		len = recvmsg(ns, &msg, 0);
		if (len < 0) 
		{
			PrintError("[%s] recvmsg()",__FUNCTION__);
		}
		else
		{
			P_BT_USBDev_HotplugParse(buf);
		}
		fflush(stdout);
	}
	close(ns);
	return;
}

static DRV_Error P_BT_NEXUS_AudioCapture_GetHandle(HUINT32 ulDevId)
{
	if( NULL == hCapture)
	{
#if defined(CONFIG_SHARED_COMPILE)
		hCapture = NEXUS_AudioCapture_GetHandle(ulDevId);
#else
		DRV_AUDIO_GetCaptureHandle(ulDevId, &hCapture);
#endif
		if ( NULL == hCapture)
		{
			PrintError("[%s] Unable to Get Capture Handle\n", __FUNCTION__);
			return DRV_ERR;
		}
	}
	else
	{
		PrintDebug("[%s] AudioCapture %p is already open\n", __FUNCTION__, hCapture);
	}

	return DRV_OK;
}

static void P_BT_NEXUS_AudioCapture_Start(void)
{
	NEXUS_AudioCaptureStartSettings captureSettings;
	NEXUS_Error rc;

	/************* AUDIO CAPTURE CODE *********************/
	NEXUS_AudioCapture_GetDefaultStartSettings(&captureSettings);
	captureSettings.dataCallback.callback = P_BT_AV_Capture_Callback;
	captureSettings.dataCallback.context = hCapture;
	captureSettings.sampleRateCallback.callback = P_BT_AV_SampleRate_Callback;
	captureSettings.sampleRateCallback.context = hCapture;

	rc = NEXUS_AudioCapture_Start(hCapture, &captureSettings);
	if ( rc != NEXUS_SUCCESS)
	{
		PrintError("[%s]AudioCapture cannot start\n", __FUNCTION__);
	}
	else
	{
		PrintDebug("[%s]AudioCapture start\n", __FUNCTION__);
	}


	return;
}


/*******************************************************************/
/********************** External Functions *************************/
/*******************************************************************/

DRV_Error DRV_BT_Init(void)
{
	HINT32	vResult = 0;
	char cSysCmd[SYSTEM_CMD_MAX];

	PrintDebug("[%s] Start \n",__FUNCTION__);
#if defined (BT_STATUS_CHECK)
	if(s_eBtDiStatus != BT_STATUS_NOT_INIT)
	{	
		PrintError("[%s] BT was already initialized ! \n",__FUNCTION__);
		return DRV_OK;
	}	
#endif
	VK_MEM_Memset(&s_stBtInstance, 0x00, sizeof(s_stBtInstance));
	
	vResult = VK_SEM_Create((unsigned long*)&s_stBtInstance.ulSema, "BT", VK_SUSPENDTYPE_FIFO);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Create failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

	vResult = VK_MSG_Create(BT_MSG_SIZE, sizeof(BT_MSG_t), (char*)"BtMsg", (unsigned long*)&s_stBtInstance.ulMsg, VK_SUSPENDTYPE_FIFO);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_MSG_Create failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Create(P_BT_USBDev_HotplugTask, HOTPLUG_TASK_PRIORITY, HOTPLUG_TASK_STACK_SIZE, "BtUSBHotplugTask", (void *)0, &ulBtUSBHotplugTaskId, FALSE);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_TASK_Create failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Start(ulBtUSBHotplugTaskId);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_TASK_Start failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Create(P_BT_Event_Task, BT_TASK_PRIORITY, BT_TASK_STACK_SIZE, "BtEventTask", (void *)0, (unsigned long*)&s_stBtInstance.ulTask, FALSE);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_TASK_Create failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Start(s_stBtInstance.ulTask);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_TASK_Start failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

	/* Load KO drivers */
	VK_MEM_Memset(cSysCmd, 0x0, SYSTEM_CMD_MAX);
	snprintf(cSysCmd, sizeof(cSysCmd), "%s", "modprobe btusb.ko");
	(void)VK_SYSTEM_Command((char *)cSysCmd);
	
	VK_MEM_Memset(cSysCmd, 0x0, SYSTEM_CMD_MAX);
	snprintf(cSysCmd, sizeof(cSysCmd), "%s", "modprobe bthid.ko");
	(void)VK_SYSTEM_Command((char *)cSysCmd);

	/* Get Local BD Address from NVRAM */
	vResult = P_BT_Init();
	if (vResult != DRV_OK)
	{
		PrintError("[%s] P_BT_Init failed:%d \n", __FUNCTION__, vResult);
		return DRV_ERR;
	}

#if defined(CONFIG_SHARED_COMPILE) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	/* 
	* Only necessary for Multi Process to join Nexus Server
	*/
	/* TODO */
	NEXUS_Error eNError = NEXUS_SUCCESS;
	eNError =NEXUS_Platform_Join();
	if(eNError != NEXUS_SUCCESS)
	{
		PrintError("[%s]NEXUS_Platform_Join(): Error Code(%d)\n", __FUNCTION__, eNError);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("[%s]NEXUS_Platform_Join(): OK!\n", __FUNCTION__);
	}
#if defined(DI_DEBUG)	
	const char *modestr[] = {"untrusted", "protected", "unprotected", "invalid"};
	NEXUS_ClientConfiguration clientConfig;
	NEXUS_Platform_GetClientConfiguration(&clientConfig);
	PrintDebug("[%s]%s unauthenticated client joined\n",__FUNCTION__, modestr[clientConfig.mode]);
	/* print client heaps */
	for (int i=0;i<NEXUS_MAX_HEAPS;i++) 
	{
		NEXUS_MemoryStatus status;
		NEXUS_HeapHandle heap;
		heap = clientConfig.heap[i];
		if (!heap) break;
		eNError = NEXUS_Heap_GetStatus(heap, &status);
		PrintDebug("client heap[%d]: MEMC %d, offset %#x, addr %p, size %d (%#x), alignment %d\n",
			i, status.memcIndex, status.offset, status.addr, status.size, status.size, status.alignment);
	}
#endif
#endif
	s_eBtDiStatus = BT_STATUS_INIT;

	PrintDebug("[%s] End \n",__FUNCTION__);

	return DRV_OK;
}

DI_ERR_CODE	DI_BT_Init(void)
{
	DRV_Error	drvResult = DRV_OK;

	drvResult = DRV_BT_Init();
	if(drvResult != DRV_OK)
	{
		PrintError("%s->DRV_BT_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, drvResult, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_BT_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_BT_Enable(HBOOL bEnable)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;

	HINT32		vResult = 0;
	BT_MSG_t	tBtMsg;

/*	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	if(s_stBtInstance.stHostSettings.bEnable == bEnable )
	{
		PrintError("[%s] Error: Bluetooth is already %s \n", __FUNCTION__,
				(bEnable ==TRUE)? "enabled":"disabled");
		diResult = DI_ERR_ERROR;
		goto ret;
	}
*/
	/* Set Bluetooth Enable/Disable */
	if (bEnable == TRUE)
	{
		tBtMsg.eMsg = BT_MSG_ENABLE;

	}
	else
	{
		tBtMsg.eMsg = BT_MSG_DISABLE;
	}
	
	tBtMsg.ulParam1 = (HUINT32)bEnable;

	vResult = VK_MSG_SendTimeout(s_stBtInstance.ulMsg, &tBtMsg, sizeof(BT_MSG_t),0);
	if(vResult)
	{
		PrintError("[%s] VK_MSG_SendTimeout Err(0x%x)!!\n",__func__, vResult);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

ret:
//	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
}



DI_ERR_CODE	DI_BT_Scan(HUINT32 ulServiceMask)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;

	HINT32		vResult = 0;
	BT_MSG_t	tBtMsg;

/*	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
*/
#if defined (BT_STATUS_CHECK)
	if(s_eBtDiStatus < BT_STATUS_ENABLE)
	{
		PrintError("[%s] : Error: BT is Disabled \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}	
#endif
	if(s_eBtDiStatus == BT_STATUS_SCANNING)
	{
		PrintError("[%s] : BT is Already Scanning \n", __FUNCTION__);
		diResult = DI_ERR_OK;
		goto ret;
	}	

	/* Start scan BT devices */
	tBtMsg.eMsg = BT_MSG_SCAN_START;
	tBtMsg.ulParam1 = ulServiceMask;
	vResult = VK_MSG_SendTimeout(s_stBtInstance.ulMsg, &tBtMsg, sizeof(BT_MSG_t),0);
	if(vResult)
	{
		PrintError("[%s] VK_MSG_SendTimeout Err(0x%x)!!\n",__func__, vResult);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

ret:
//	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
}

DI_ERR_CODE DI_BT_Connect(HUINT32 ulDevId)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	HINT32		vResult = 0;
	BT_MSG_t	tBtMsg;

/*	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
*/
#if defined (BT_STATUS_CHECK)
	if(s_eBtDiStatus < BT_STATUS_ENABLE)
	{
		PrintError("[%s] : Error: BT is Disabled \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#endif
/*	if(ulDevId >= s_sCbEvtDevInfo.ulNumOfData)
	{
		PrintError("[%s] : Error: ulDevId[%d] is invaild \n", __FUNCTION__, ulDevId);
		diResult = DI_ERR_ERROR;
		goto ret;
	}*/
	/* TODO */
/*	if(s_sCbEvtDevInfo.stBtDevList[ulDevId].etDevStatus == DI_BT_DEV_CONNECTED)
	{
		PrintError("[%s] : Error: Device [%s] is now connecting\n", __FUNCTION__,s_sCbEvtDevInfo.stBtDevList[ulDevId].ucName);
		diResult = DI_ERR_ERROR;
		goto ret;
	}*/

	/* Connect to BT devices */
	tBtMsg.eMsg = BT_MSG_CONNECT_START;
	tBtMsg.ulParam1 = ulDevId;
	vResult = VK_MSG_SendTimeout(s_stBtInstance.ulMsg, &tBtMsg, sizeof(BT_MSG_t),0);
	if(vResult)
	{
		PrintError("[%s] VK_MSG_SendTimeout Err(0x%x)!!\n",__func__, vResult);
		diResult = DI_ERR_ERROR;
	}
#if defined (BT_STATUS_CHECK)
ret:
#endif
//	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
}

DI_ERR_CODE DI_BT_SendPinCode(HUINT32 ulDevId, HUINT8 *pucPinCode)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	DRV_Error	drvResult = DRV_OK;
	HINT32		vResult = 0;
	HUINT8 ucPinCode[BT_NB_PINCODE_LEN];

	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
	if ( pucPinCode == NULL)
	{
		PrintError("[%s] wrong parameter\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	VK_MEM_Memcpy(ucPinCode,pucPinCode, BT_NB_PINCODE_LEN);

	/* Reply Pin Code to BSA_Server */
	drvResult = P_BT_SendPinCode(ulDevId, ucPinCode);
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_SendPinCode failed:%d \n", __FUNCTION__, drvResult);
	}

ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;

}

/* TODO */
DI_ERR_CODE DI_BT_Disconnect(HUINT32 ulDevId)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	HINT32		vResult = 0;
	BT_MSG_t	tBtMsg;


/*	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
*/
#if defined (BT_STATUS_CHECK)
	if(s_eBtDiStatus < BT_STATUS_ENABLE)
	{
		PrintError("[%s] : Error: BT is Disabled \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#endif
	if(ulDevId >= s_sCbEvtDevInfo.ulNumOfData)
	{
		PrintError("[%s] : Error: ulDevId[%d] is invaild \n", __FUNCTION__, ulDevId);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

/*	if(s_sCbEvtDevInfo.stBtDevList[ulDevId].etDevStatus != DI_BT_DEV_CONNECTED)
	{
		PrintError("[%s] : Error: Device [%s] is not connected now\n", __FUNCTION__,s_sCbEvtDevInfo.stBtDevList[ulDevId].ucName);
		diResult = DI_ERR_ERROR;
		goto ret;
	}*/

	/* Disconnect a BT device */
	tBtMsg.eMsg = BT_MSG_DISCONNECT;
	tBtMsg.ulParam1 = ulDevId;
	vResult = VK_MSG_SendTimeout(s_stBtInstance.ulMsg, &tBtMsg, sizeof(BT_MSG_t),0);
	if(vResult)
	{
		PrintError("[%s] VK_MSG_SendTimeout Err(0x%x)!!\n",__func__, vResult);
		diResult = DI_ERR_ERROR;
	}

ret:
//	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
}


DI_ERR_CODE DI_BT_Remove(HUINT32 ulDevId)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	HINT32		vResult = 0;
	DRV_Error	drvResult = DRV_OK;

	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	drvResult = P_BT_Remove(ulDevId);
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_Remove failed:%d \n", __FUNCTION__, drvResult);
		diResult = DI_ERR_ERROR;
	}

ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
}


DI_ERR_CODE DI_BT_GetHostInfo(DI_BT_DEV_INFO_t *stDevInfo)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	HINT32		vResult = 0;

	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
	if ( stDevInfo == NULL)
	{
		PrintError("[%s] wrong parameter\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
	/* Get device info from Local Global Varaible(s_stBtInstance.stHostSettings) */
	
	VK_MEM_Memcpy(stDevInfo->ucName, s_stBtInstance.stHostSettings.ucName, BT_NB_NAME_LEN);
	VK_MEM_Memcpy(stDevInfo->ucBdAddr, s_stBtInstance.stHostSettings.ucBdAddr, BT_NB_BDADDR_LEN);
	VK_MEM_Memcpy(stDevInfo->ucClass, s_stBtInstance.stHostSettings.ucCOD, BT_NB_CLASS_LEN);

	PrintDebug("[%s] Local Bluetooth Device Information : %s\n", __FUNCTION__, s_stBtInstance.stHostSettings.ucName);
	PrintDebug("[%s] Name %s \n", __FUNCTION__, s_stBtInstance.stHostSettings.ucName);
	PrintDebug("[%s] Enable %s \n", __FUNCTION__, s_stBtInstance.stHostSettings.bEnable == TRUE ? "ENABLE" : "DISABLE");
	PrintDebug("[%s] Discoverable %s \n", __FUNCTION__, s_stBtInstance.stHostSettings.bDiscoverable == TRUE ? "ENABLE" : "DISABLE");
	PrintDebug("[%s] Connectable %s \n", __FUNCTION__, s_stBtInstance.stHostSettings.bConnectable == TRUE ? "ENABLE" : "DISABLE");
	PrintDebug("[%s] Bd Addr %x:%x:%x:%x:%x:%x \n", __FUNCTION__, 
				s_stBtInstance.stHostSettings.ucBdAddr[0], s_stBtInstance.stHostSettings.ucBdAddr[1],
				s_stBtInstance.stHostSettings.ucBdAddr[2], s_stBtInstance.stHostSettings.ucBdAddr[3],
				s_stBtInstance.stHostSettings.ucBdAddr[4], s_stBtInstance.stHostSettings.ucBdAddr[5]);
	PrintDebug("[%s] Class Of Device(Service:Major Device:Minor Device) %x:%x:%x \n", __FUNCTION__, 
			s_stBtInstance.stHostSettings.ucCOD[0], s_stBtInstance.stHostSettings.ucCOD[1], s_stBtInstance.stHostSettings.ucCOD[2]);
	PrintDebug("[%s] UIPC PATH %s  \n", __FUNCTION__, s_stBtInstance.stHostSettings.cRootPath);
	PrintDebug("[%s] PIN_CODE(Length) %s (%d)  \n", __FUNCTION__, s_stBtInstance.stHostSettings.ucPinCode, s_stBtInstance.stHostSettings.cPinLength);
	PrintDebug("[%s] Input/Output Capabilities use for Simpe Paring %d \n", __FUNCTION__, s_stBtInstance.stHostSettings.io_cap);

ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
}


DI_ERR_CODE DI_BT_SetHostInfo(HUINT32 ulDevId)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	DRV_Error	drvResult = DRV_OK;
	HINT32		vResult = 0;

	UNUSED(ulDevId);	

	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#if defined (BT_STATUS_CHECK)	
	if(s_eBtDiStatus < BT_STATUS_ENABLE)
	{
		PrintError("[%s] : Error: BT is Disabled \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#endif
	/* Set device info */	
	drvResult = P_BT_MGT_SetDeviceConfig(&s_stBtInstance.stHostSettings);
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_MGT_SetDeviceConfig failed!\n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
	}

ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
}

DI_ERR_CODE DI_BT_SetPinCode(HUINT8 *pucPinCode)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	HINT32		vResult = 0;

	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
	
	PrintDebug("[%s] Set PinCode from %s(%d)", __func__, s_stBtInstance.stHostSettings.ucPinCode,s_stBtInstance.stHostSettings.cPinLength);
	VK_MEM_Memcpy(s_stBtInstance.stHostSettings.ucPinCode, pucPinCode, sizeof(s_stBtInstance.stHostSettings.ucPinCode));
	s_stBtInstance.stHostSettings.ucPinCode[BT_NB_PINCODE_LEN -1] = 0; /* Pin Code' MSB is 0 */
	s_stBtInstance.stHostSettings.cPinLength = VK_strlen(pucPinCode);
	PrintDebug("to %s(%d)", s_stBtInstance.stHostSettings.ucPinCode, s_stBtInstance.stHostSettings.cPinLength);

ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;

}


DI_ERR_CODE DI_BT_GetDeviceList(DI_BT_DEV_LIST_t *pstDevList)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	HINT32		vResult = 0;

	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
/*
	if(s_eBtDiStatus < BT_STATUS_ENABLE)
	{
		PrintError("[%s] : Error: Bluetooth is Disabled \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
*/
	if ( pstDevList == NULL)
	{
		PrintError("[%s] wrong parameter\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	VK_MEM_Memcpy(pstDevList, &s_sCbEvtDevInfo, sizeof(DI_BT_DEV_LIST_t));


ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
}

/* TODO */
DI_ERR_CODE DI_BT_OpenProfile(HUINT32 ulDevId, DI_BT_PROFILE_e eProfile)
{
	DI_ERR_CODE diResult = DI_ERR_OK;
	HINT32		vResult = 0;
	DRV_Error	drvResult = DRV_OK;

	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#if defined (BT_STATUS_CHECK)	
	if(s_eBtDiStatus < BT_STATUS_ENABLE)
	{
		PrintError("[%s] : Error: BT is Disabled \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#endif
	if(ulDevId >= s_sCbEvtDevInfo.ulNumOfData)
	{
		PrintError("[%s] : Error: ulDevId[%d] is invaild \n", __FUNCTION__, ulDevId);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	/* Open profile */
	if(eProfile & DI_BT_PROFILE_A2DP)
	{
		drvResult = P_BT_AV_Start();
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_Start failed:%d \n", __FUNCTION__, drvResult);
			diResult = DI_ERR_ERROR;
			goto ret;
		}
	}
	if(eProfile & DI_BT_PROFILE_HID)
	{
		/* TODO */
		PrintError("[%s] HID Profile Open already:%d \n", __FUNCTION__, drvResult);
	}

ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;

}
/* TODO */
DI_ERR_CODE DI_BT_CloseProfile(HUINT32 ulDevId, DI_BT_PROFILE_e eProfile)
{
	DI_ERR_CODE diResult = DI_ERR_OK;
	HINT32		vResult = 0;
	DRV_Error	drvResult = DRV_OK;


	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#if defined (BT_STATUS_CHECK)
	if(s_eBtDiStatus < BT_STATUS_ENABLE)
	{
		PrintError("[%s] : Error: BT is Disabled \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#endif
	if(ulDevId >= s_sCbEvtDevInfo.ulNumOfData)
	{
		PrintError("[%s] : Error: ulDevId[%d] is invaild \n", __FUNCTION__, ulDevId);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	/* Close profile */
	if(eProfile & DI_BT_PROFILE_A2DP)
	{
		drvResult = P_BT_AV_Stop();
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_Stop failed:%d \n", __FUNCTION__, drvResult);
			diResult = DI_ERR_ERROR;
			goto ret;
		}
	}
	if(eProfile & DI_BT_PROFILE_HID)
	{
		/* TODO */
		PrintError("[%s] HID Profile Close :%d \n", __FUNCTION__, drvResult);
	}

ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
	
}

DI_ERR_CODE DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_e eEvt, pfnBT_EVT_CALLBACK pfnCallback)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	HINT32		vResult = 0;
	
	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
	
	s_stBtInstance.stCallbackInfo[eEvt].pfnCallback = pfnCallback;
	PrintDebug("[%s] eEvt %d 0x%x 0x%x \n",__func__, eEvt, s_stBtInstance.stCallbackInfo[eEvt].pfnCallback, pfnCallback);
	
ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
}



DI_ERR_CODE DI_BT_Connect2BdAddr(HUINT8 *pucBdAddr)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	DRV_Error drvResult = DRV_OK;

	HUINT32 ulDevId = 0 ;
	DI_BT_DEV_INFO_t *pstConnectDev = NULL;
	DI_BT_CALLBACK_DATA_t stCbData;


	ulDevId = P_BT_GetDIDevId(pucBdAddr);
	if (ulDevId >= BT_NB_DISC_DEVICES)
	{
		PrintError("[%s] Device(%02X:%02X:%02X:%02X:%02X:%02X) is not in DI Device List\n",__FUNCTION__,
			pucBdAddr[0],pucBdAddr[1],pucBdAddr[2],pucBdAddr[3],pucBdAddr[4],pucBdAddr[5]);
		return DI_ERR_ERROR;	//List에 없는 것은 Connect 할 수 없음
	}
	else
	{
		PrintError("[%s] Device BD Address : %02X:%02X:%02X:%02X:%02X:%02X -----> Index : %d\n",__FUNCTION__,
			pucBdAddr[0],pucBdAddr[1],pucBdAddr[2],pucBdAddr[3],pucBdAddr[4],pucBdAddr[5],
			ulDevId);
	}

	pstConnectDev = &s_sCbEvtDevInfo.stBtDevList[ulDevId];

	/* Connect to BT devices */
	if((pstConnectDev->etAvailableProfile & DI_BT_PROFILE_A2DP) || ((pstConnectDev->ucClass[1]&0x1F) == BTM_COD_MAJOR_AUDIO))
	{
		drvResult = P_BT_AV_Connect(pstConnectDev);
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_Connect failed:%d \n", __FUNCTION__, drvResult);
			stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_FAIL;
			stCbData.ulDevId = ulDevId;
			stCbData.ulNumOfData = s_sCbEvtDevInfo.ulNumOfData;
			stCbData.pucBdAddr = pucBdAddr;
			stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
			P_BT_PostMsgToMw(&stCbData);
		}
	}
	/* PERIPHERAL Device(0x05)*//* HID */
	if((pstConnectDev->etAvailableProfile & DI_BT_PROFILE_HID) || ((pstConnectDev->ucClass[1]&0x1F) == BTM_COD_MAJOR_PERIPHERAL)) 
	{
		/* TODO: Phase 2? */
		drvResult = P_BT_HID_Connect(pstConnectDev);
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_HID_Connect failed:%d \n", __FUNCTION__, drvResult);
			stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_FAIL;
			stCbData.ulDevId = ulDevId;
			stCbData.ulNumOfData = s_sCbEvtDevInfo.ulNumOfData;
			stCbData.pucBdAddr = pucBdAddr;
			stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
			P_BT_PostMsgToMw(&stCbData);
		}
	}
#if 0
	else
	{
		PrintError("[%s] Not Supported Profile \n", __FUNCTION__, drvResult);
		stCbData.eEvent = DI_BT_NOTIFY_EVT_CONNECT_FAIL;
		stCbData.ulDevId = ulDevId;
		stCbData.ulNumOfData = s_sCbEvtDevInfo.ulNumOfData;
		stCbData.pucBdAddr = pucBdAddr;
		stCbData.pvEventData = (void *)&s_sCbEvtDevInfo;
		P_BT_PostMsgToMw(&stCbData);
/*		drvResult =  P_BT_Pair(ulDevId);
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_Pair failed:%d \n", __FUNCTION__, drvResult);
		}*/
	}
#endif

	return diResult;
}

DI_ERR_CODE DI_BT_SendPinCode2BdAddr(HUINT8 *pucBdAddr, HUINT8 *pucPinCode)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	DRV_Error	drvResult = DRV_OK;
	HUINT32		ulDevId = 0;

	ulDevId = P_BT_GetDIDevId(pucBdAddr);
	if (ulDevId >= BT_NB_DISC_DEVICES)
	{
		PrintError("[%s] Device(%02X:%02X:%02X:%02X:%02X:%02X) is not in DI Device List\n",__FUNCTION__,
			pucBdAddr[0],pucBdAddr[1],pucBdAddr[2],pucBdAddr[3],pucBdAddr[4],pucBdAddr[5]);
		return DI_ERR_ERROR;	//List에 없는 것은 SendPinCode 할 수 없음
	}
	else
	{
		PrintError("[%s] Device BD Address : %02X:%02X:%02X:%02X:%02X:%02X -----> Index : %d\n",__FUNCTION__,
			pucBdAddr[0],pucBdAddr[1],pucBdAddr[2],pucBdAddr[3],pucBdAddr[4],pucBdAddr[5],
			ulDevId);
	}

	/* Reply Pin Code to BSA_Server */
	drvResult = P_BT_SendPinCode(ulDevId, pucPinCode);
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_SendPinCode failed:%d \n", __FUNCTION__, drvResult);
	}

	return diResult;
}

/* TODO */
DI_ERR_CODE DI_BT_Disconnect2BdAddr(HUINT8 *pucBdAddr)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	HUINT32		ulDevId = 0;
	DRV_Error	drvResult = DRV_OK;
	DI_BT_DEV_INFO_t *pstDisconnectDev = NULL;

	ulDevId = P_BT_GetDIDevId(pucBdAddr);
	if (ulDevId >= BT_NB_DISC_DEVICES)
	{
		PrintError("[%s] Device(%02X:%02X:%02X:%02X:%02X:%02X) is not in DI Device List\n",__FUNCTION__,
			pucBdAddr[0],pucBdAddr[1],pucBdAddr[2],pucBdAddr[3],pucBdAddr[4],pucBdAddr[5]);
		return DI_ERR_ERROR;	//List에 없는 것은 Disconnect 할 수 없음
	}
	else
	{
		PrintError("[%s] Device BD Address : %02X:%02X:%02X:%02X:%02X:%02X -----> Index : %d\n",__FUNCTION__,
			pucBdAddr[0],pucBdAddr[1],pucBdAddr[2],pucBdAddr[3],pucBdAddr[4],pucBdAddr[5],
			ulDevId);
	}

	pstDisconnectDev = &s_sCbEvtDevInfo.stBtDevList[ulDevId];

	/* Disconnect a BT device */
	pstDisconnectDev->etDevStatus = DI_BT_DEV_DISCONNECTED;

	if((pstDisconnectDev->etAvailableProfile & DI_BT_PROFILE_A2DP)||((pstDisconnectDev->ucClass[1]&0x1F) == BTM_COD_MAJOR_AUDIO))	/* Audio Device(0x04) */ 	/* A2DP */
	{
		drvResult = P_BT_AV_Disconnect(pstDisconnectDev);
		if(drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_Disconnect failed\n", __FUNCTION__);
		}
	}
	if((pstDisconnectDev->etAvailableProfile & DI_BT_PROFILE_HID)||((pstDisconnectDev->ucClass[1]&0x1F) == BTM_COD_MAJOR_PERIPHERAL)) /* PERIPHERAL Device(0x05)*//* HID */
	{
		/* TODO */
		drvResult = P_BT_HID_Disconnect(pstDisconnectDev);
		if(drvResult != DRV_OK)
		{
			PrintError("[%s]  P_BT_HID_Disconnect failed\n", __FUNCTION__);
		}
	}


	return diResult;
}


DI_ERR_CODE DI_BT_Remove2BdAddr(HUINT8 *pucBdAddr)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	HINT32		vResult = 0;
	DRV_Error	drvResult = DRV_OK;
	HUINT32		ulDevId = 0;
	HUINT32		i=0;
	DI_BT_DEV_INFO_t *pstRemoveDev = NULL;
	tAPP_XML_REM_DEVICE *pstXmlDev = NULL;

	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	ulDevId = P_BT_GetDIDevId(pucBdAddr);
	if (ulDevId >= BT_NB_DISC_DEVICES)
	{
		PrintError("[%s] Device(%02X:%02X:%02X:%02X:%02X:%02X) is not in DI Device List\n",__FUNCTION__,
			pucBdAddr[0],pucBdAddr[1],pucBdAddr[2],pucBdAddr[3],pucBdAddr[4],pucBdAddr[5]);
		diResult = DI_ERR_ERROR;		//List에 없는 것은 Remove 할 수 없음
		goto ret;
	}
	else
	{
		PrintError("[%s] Device BD Address : %02X:%02X:%02X:%02X:%02X:%02X -----> Index : %d\n",__FUNCTION__,
			pucBdAddr[0],pucBdAddr[1],pucBdAddr[2],pucBdAddr[3],pucBdAddr[4],pucBdAddr[5],
			ulDevId);
	}
	pstRemoveDev = &s_sCbEvtDevInfo.stBtDevList[ulDevId];

#if defined(DI_DEBUG)
	P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif
	if (s_sCbEvtDevInfo.ulNumOfData == 0)
	{
		PrintError("[%s] There is no device in DI DevList!\n", __FUNCTION__);
		diResult = DI_ERR_ERROR;	
		goto ret;
	}
	else
	{
		/* Send CMD(Remove Device) to BSA_SERVER */
		drvResult = P_BT_SEC_RemoveDevice(pstRemoveDev->ucBdAddr);
		if ( drvResult != DRV_OK)
		{
			PrintError("[%s] Error:  P_BT_SEC_RemoveDevice \n", __FUNCTION__);
		}
		else
		{
			PrintError("[%s] Complete: P_BT_SEC_RemoveDevice \n", __FUNCTION__);
			if(pstRemoveDev->etDevStatus == DI_BT_DEV_CONNECTED)
			{
				PrintError("[%s] Remove a Connecting Device \n", __FUNCTION__);
				VK_TASK_Sleep(100);	//Time to deal with disconnecting a connected device 
			}
		}

		/* Erase the device from the XML file */
		pstXmlDev = app_xml_find_dev_db(app_xml_remote_devices_db,
					APP_NUM_ELEMENTS(app_xml_remote_devices_db),
					pstRemoveDev->ucBdAddr);
		if (pstXmlDev != NULL)
		{
			pstXmlDev->in_use = FALSE;
		}
		/* Saving app_xml_remote_devices_db to .XMLfiles */
		drvResult = app_write_xml_remote_devices();
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] app_write_xml_remote_devices failed\n", __FUNCTION__);
		}
		/* Loading app_xml_rem_devices_db to stPairDevInfo */
		drvResult = P_BT_XML_GetDevList();		/* Init stPairDevInfo --> Sorting TRUE/FALSE -->Save stPairDevInfo*/
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_XML_GetDevList failed:%d \n", __FUNCTION__, drvResult);
		}

		/* Update DI Device List(s_sCbEvtDevInfo) */
		for ( i = ulDevId ; i < s_sCbEvtDevInfo.ulNumOfData-1 ; i++)
		{
			if( i < BT_NB_DISC_DEVICES-1)		// for prevent defect
			{
				VK_MEM_Memcpy(&s_sCbEvtDevInfo.stBtDevList[i], &s_sCbEvtDevInfo.stBtDevList[i+1],sizeof(DI_BT_DEV_INFO_t));
			}
		}
		VK_MEM_Memset(&s_sCbEvtDevInfo.stBtDevList[s_sCbEvtDevInfo.ulNumOfData-1], 0x0,sizeof(DI_BT_DEV_INFO_t));
#if defined(DI_DEBUG)
	P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif
		s_sCbEvtDevInfo.ulNumOfData--;
	}
#if defined(DI_DEBUG)
	P_BT_PrintDeviceList(&s_sCbEvtDevInfo);		/* DI_DEBUG */
#endif
	

ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
}


/* TODO */
DI_ERR_CODE DI_BT_OpenProfile2BdAddr(HUINT8 *pucBdAddr, DI_BT_PROFILE_e eProfile)
{
	DI_ERR_CODE diResult = DI_ERR_OK;
	HINT32		vResult = 0;
	DRV_Error	drvResult = DRV_OK;
	HUINT32		ulDevId = 0;

	UNUSED(pucBdAddr);

	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#if defined (BT_STATUS_CHECK)	
	if(s_eBtDiStatus < BT_STATUS_ENABLE)
	{
		PrintError("[%s] : Error: BT is Disabled \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#endif
	if(ulDevId >= s_sCbEvtDevInfo.ulNumOfData)
	{
		PrintError("[%s] : Error: ulDevId[%d] is invaild \n", __FUNCTION__, ulDevId);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	/* Open profile */
	if(eProfile & DI_BT_PROFILE_A2DP)
	{
		drvResult = P_BT_AV_Start();
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_Start failed:%d \n", __FUNCTION__, drvResult);
			diResult = DI_ERR_ERROR;
			goto ret;
		}
	}
	if(eProfile & DI_BT_PROFILE_HID)
	{
		/* TODO */
		PrintError("[%s] HID Profile Open already:%d \n", __FUNCTION__, drvResult);
	}

ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;

}
/* TODO */
DI_ERR_CODE DI_BT_CloseProfile2BdAddr(HUINT8 *pucBdAddr, DI_BT_PROFILE_e eProfile)
{
	DI_ERR_CODE diResult = DI_ERR_OK;
	HINT32		vResult = 0;
	DRV_Error	drvResult = DRV_OK;
	HUINT32		ulDevId = 0;

	UNUSED(pucBdAddr);

	vResult = VK_SEM_Get(s_stBtInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#if defined (BT_STATUS_CHECK)
	if(s_eBtDiStatus < BT_STATUS_ENABLE)
	{
		PrintError("[%s] : Error: BT is Disabled \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#endif
	if(ulDevId >= s_sCbEvtDevInfo.ulNumOfData)
	{
		PrintError("[%s] : Error: ulDevId[%d] is invaild \n", __FUNCTION__, ulDevId);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	/* Close profile */
	if(eProfile & DI_BT_PROFILE_A2DP)
	{
		drvResult = P_BT_AV_Stop();
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_Stop failed:%d \n", __FUNCTION__, drvResult);
			diResult = DI_ERR_ERROR;
			goto ret;
		}
	}
	if(eProfile & DI_BT_PROFILE_HID)
	{
		/* TODO */
		PrintError("[%s] HID Profile Close :%d \n", __FUNCTION__, drvResult);
	}


ret:
	VK_SEM_Release(s_stBtInstance.ulSema);
	return diResult;
	
}

DI_ERR_CODE	DI_BT_SetBdAddrToNvram(HUINT8 *pucBdAddr)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	DRV_Error	drvResult = DRV_OK;

	if ( pucBdAddr == NULL)
	{
		PrintError("[%s] wrong parameter\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#if defined(CONFIG_SHARED_COMPILE)
	PrintError("[%s] Not Supported in Shared Library Mode:%d \n", __FUNCTION__,drvResult);
#else
	drvResult = P_BT_Set_BdAddrToNvram(pucBdAddr);		/* Set BD Address to Nvram */
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] Error: P_BT_Set_BdAddrToNvram:%d \n", __FUNCTION__,drvResult);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#endif

ret:
	return diResult;
}


DI_ERR_CODE	DI_BT_GetBdAddrFromNvram(HUINT8 *pucBdAddr)
{
	DI_ERR_CODE	diResult = DI_ERR_OK;
	DRV_Error	drvResult = DRV_OK;

	if ( pucBdAddr == NULL)
	{
		PrintError("[%s] wrong parameter\n",__func__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#if defined(CONFIG_SHARED_COMPILE)
	drvResult = P_BT_Get_BdAddrFromCmdline(pucBdAddr);		/* Get BD Address from /proc/cmdline */
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] Error: P_BT_Get_BdAddrFromCmdline \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#else
	drvResult = P_BT_Get_BdAddrFromNvram(pucBdAddr);		/* Get BD Address from Nvram */
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] Error: P_BT_Get_BdAddrFromNvram \n", __FUNCTION__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#endif

ret:
	return diResult;
}


DI_ERR_CODE DI_BT_ReadVersion(void)
{
	DRV_Error	drvResult = DRV_OK;

	drvResult = P_BT_TM_ReadVersion();
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_TM_ReadVersion failed:%d \n", __FUNCTION__, drvResult);
		return DI_ERR_ERROR;
	}
	return DI_ERR_OK;
}

DI_ERR_CODE DI_BT_SetTraceLevel(int nLevel)
{
	DRV_Error	drvResult = DRV_OK;

	drvResult = P_BT_TM_SetTrace(nLevel);
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_TM_SetTrace failed:%d \n", __FUNCTION__, drvResult);
		return DI_ERR_ERROR;
	}
	return DI_ERR_OK;
}

/* For Factory Default */

DI_ERR_CODE DI_BT_DelXmlFiles(void)
{
	HINT32	vResult = 0;

	char cSysCmd[SYSTEM_CMD_MAX];

	/* Clear app_xml_remote_devices_db */
	VK_MEM_Memset(app_xml_remote_devices_db, 0, sizeof(app_xml_remote_devices_db));

	/* Delete .xml files */
	VK_MEM_Memset(cSysCmd, 0x0, SYSTEM_CMD_MAX);
	snprintf(cSysCmd, sizeof(cSysCmd), "%s", "rm -f /var/lib/humaxtv_user/bt_*.xml");

	vResult = VK_SYSTEM_Command((char *)cSysCmd);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SYSTEM_Command(%s) failed!\n",__FUNCTION__,cSysCmd);
		return DI_ERR_ERROR;
	}
	
	return DI_ERR_OK;
}



