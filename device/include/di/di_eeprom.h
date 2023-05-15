/* $Header: $ */
/****************************************************************************
* $Workfile:   di_eeprom.h  $
* $Modtime:   Jul 30 2007 00:17:52  $
*
* Author:
* Description:
*
*                                 Copyright (c) 2008 HUMAX Co., Ltd.
*                                               All rights reserved.
*****************************************************************************/

#ifndef _DI_EEPROM_H_
#define _DI_EEPROM_H_

#include "htype.h"
#include "di_err.h"

#include "di_channel_s.h"
#include "di_channel_t.h"
#include "di_channel_c.h"

/****************************************************************************
* Definitions
*****************************************************************************/

#define  DI_OTA_FILEUPDATED  0xAA

/****************************************************************************
* Typedef & const data
*****************************************************************************/
typedef enum
{
	DI_OTA_PANEL_ON = 0,
	DI_OTA_PANEL_OFF,				/* Loader side : panel off   Main appl side : panel off */
	DI_OTA_PANEL_MAIN_ON			/* Main appl side : panel on */
}DI_OTA_PANELSTATUS_t;

typedef enum
{
	DI_EEPROM_OSD_ON = 0,
	DI_EEPROM_OSD_OFF
}DI_EEPROM_OSD_DISPLAY_t;

typedef enum
{
	DI_EEPROM_PANEL_DISPLAY_ON = 0,
	DI_EEPROM_PANEL_DISPLAY_OFF
}DI_EEPROM_PANELDISPLAY_t;

typedef enum
{
	DI_EEPROM_HDD_POWER_ON,
	DI_EEPROM_HDD_POWER_OFF
}DI_EEPROM_HDD_POWER_t;

typedef enum
{
	DI_EEPROM_ASPECT_RATIO_4_3 = 0,
	DI_EEPROM_ASPECT_RATIO_16_9
}DI_EEPROM_ASPECT_RATIO_t;

typedef enum
{
	DI_OTA_DVBSSU = 1,
	DI_OTA_DTG = 2,
	DI_OTA_HUMAXDOWNLOAD = 3,
	DI_OTA_NORDIG = 4,
	DI_OTA_FILE = 5,
	DI_OTA_SKYD = 6,
	DI_OTA_DUMMY
}DI_OTA_TYPE_t;


typedef enum
{
	DI_OTA_SAT		= 1,
	DI_OTA_CAB		= 2,
	DI_OTA_TER		= 3,
	DI_OTA_TS   	= 4,/* <== for LF7700 */
	DI_OTA_NET		= 5,
	DI_OTA_FTP		= 6
}DI_OTA_CHANNEL_t;

typedef enum {
	DI_EEPROM_HD_RESOLUTION_480I = 0,
	DI_EEPROM_HD_RESOLUTION_480P,
	DI_EEPROM_HD_RESOLUTION_576I,
	DI_EEPROM_HD_RESOLUTION_576P,
	DI_EEPROM_HD_RESOLUTION_720P,
	DI_EEPROM_HD_RESOLUTION_1080I,
	DI_EEPROM_HD_RESOLUTION_720P_50HZ,
	DI_EEPROM_HD_RESOLUTION_1080I_50HZ,
	DI_EEPROM_HD_RESOLUTION_1080P_50HZ,
	DI_EEPROM_HD_RESOLUTION_480I_J,
	DI_EEPROM_HD_RESOLUTION_1080P_24HZ,
	DI_EEPROM_HD_RESOLUTION_1080P_30HZ,	
	DI_EEPROM_HD_RESOLUTION_1080P_60HZ,
	DI_EEPROM_HD_RESOLUTION_2160P_25HZ,
	DI_EEPROM_HD_RESOLUTION_2160P_30HZ,
	DI_EEPROM_HD_RESOLUTION_2160P_50HZ,
	DI_EEPROM_HD_RESOLUTION_2160P_60HZ,
	DI_EEPROM_HD_RESOLUTION_MAX
}DI_EEPROM_HD_RESOLUTION;

typedef enum
{
	DI_EEPROM_SCART_OUTPUT_CVBS=3,
	DI_EEPROM_SCART_OUTPUT_RGB,
	DI_EEPROM_SCART_OUTPUT_SVIDEO,

	DI_EEPROM_SCART_MAX
} DI_EEPROM_SCART_SETTING_VALUE;

typedef enum tagDI_EEPROM_FIELD
{
	DI_EEPROM_FIELD_SYSTEM_ID = 0,			// HUINT32				// 4				// System Id
	DI_EEPROM_FIELD_OTA_TYPE,				// HUINT32				// 1				// DI_OTA_TYPE_t
	DI_EEPROM_FIELD_CH_TYPE,				// HUINT32				// 1				// DI_OTA_CHANNEL_t
	DI_EEPROM_FIELD_CH_INFO_S,				// DI_CHANNEL_PARAM		// 0x44			// DI_CHANNEL_PARAM
	DI_EEPROM_FIELD_CH_INFO_T,				// DI_CHANNEL_PARAM		// 0x44			// DI_CHANNEL_PARAM
	DI_EEPROM_FIELD_CH_INFO_C,				// DI_CHANNEL_PARAM		// 0x44			// DI_CHANNEL_PARAM
	DI_EEPROM_FIELD_ES_PID,					// HUINT16				// 2				// Element Stream PID for OTA
	DI_EEPROM_FIELD_LOADER_VERSION,			// HUINT16				// 2				// Version of boot Loader
	DI_EEPROM_FIELD_HDF_VERSION,			// HUINT8					// 0				// Not Used
	DI_EEPROM_FIELD_SW_UPGRADE,				// HUINT32				// 1				// DI_OTAFLAG_t
	DI_EEPROM_FIELD_APP_VERSION,			// HUINT32				// 4				// Version number of Application
	DI_EEPROM_FIELD_OTALDR_VERSION,			// HUINT16				// 2				// Version of OTA Loader
	DI_EEPROM_FIELD_PANEL_ID,				// HUINT16				// 0				// Not Used
	DI_EEPROM_FIELD_STRING_VERSION,			// HUINT32				// 0				// Not Used
	DI_EEPROM_FIELD_CUR_OTAIMAGE_VERSION,	// HUINT32				// 4				// Version number of OTA image
	DI_EEPROM_FIELD_OTA_STATUS,		        // HUINT8 Flash Write status	// 0				// Not Used
	DI_EEPROM_FIELD_OTA_FILE,		        // HUINT8 File updated		// 1				// ???
	DI_EEPROM_FIELD_PANEL_STATUS,			// HUINT32 Panel Status		// 1				// DI_OTA_PANELSTATUS_t
	DI_EEPROM_FIELD_LANGUAGE,				// 3	eng/ger				// 3	eng/ger				// OSD language code in ISO 639 code
	DI_EEPROM_FIELD_TV_STANDARD,			// HUINT32				// 1						// DI_VIDEO_SD_STANDARD
	DI_EEPROM_FIELD_SCART_RGB,				// DI_EEPROM_SCART_SETTING_VALUE				// 1						// DI_EEPROM_SCART_SETTING_VALUE
	DI_EEPROM_FIELD_HD_RESOULTION,			// HUINT32				// 1						// DI_EEPROM_HD_RESOLUTION
	DI_EEPROM_FIELD_TUNER_ID,				// HUINT8					// 1						// Tuner Id
	DI_EEPROM_FIELD_ANTENA_POWER,			// HUINT8					// 1						// External Antena Power for Ter, 0:Off/1:On
	DI_EEPROM_FIELD_CHANNEL_INFO,			// DI_CHANNEL_PARAM		// 0x44					// Channel Info of Sat, Ter, Cab, Network
	DI_EEPROM_FIELD_NET_PORTAL_IP,			// 4						// 4	 					// portal ip address
	DI_EEPROM_FIELD_NET_DOWN_PATH,			// DI_CHANNEL_PARAM 		// 0x44 					// Network Download Path
	DI_EEPROM_FIELD_NET_DEVICE_ID,			// 1						// 1						// ethernet device id 0 = eth0
	DI_EEPROM_FIELD_NET_IP_CONFIG,			// DI_EEPROM_NET_IP_CONFIG	// 1 						// DHCP/Manual
	DI_EEPROM_FIELD_NET_IP_SETTING,			// DI_EEPROM_NET_IP_SETTING	// 0x10 					// Manual IP setting
	DI_EEPROM_FIELD_TRANSACTION_ID,			// HUINT16				// 2						// Transaction Id for Nordic OTA
	DI_EEPROM_FIELD_OAD_VERSION,			// 8						// 8						// for MHEG ???
	DI_EEPROM_FIELD_RCU_CUSTOM_CODE,		// HUINT16				// 2						// for RCU custom code
	DI_EEPROM_FIELD_RCU_CUSTOM_CODE2,		// HUINT16				// 2						// for alternative RCU custom code
	DI_EEPROM_FIELD_HDD_POWER_CONTROL,		// DI_EEPROM_HDD_POWER_t	// 1						// for HDD power control
	DI_EEPROM_FIELD_PANEL_DISPLAY,			// DI_OTA_PANELDISPLAY_t	// 1						// DI_OTA_PANELDISPLAY_t
	DI_EEPROM_FIELD_PANEL_DIMMING,			// HUINT8					// 1						// HUINT8
	DI_EEPROM_FIELD_OSD_DISPLAY,			// DI_EEPROM_OSD_DISPLAY_t					// 1		// DI_EEPROM_OSD_DISPLAY_t
	DI_EEPROM_FIELD_START_BANK_FLAG,		// 두 개의 bank중 어디로 부팅 할 지 결정
	DI_EEPROM_FIELD_CHECK_USB,				// 부팅 시 USB 마운트해서 hdf를 검사할지 말지 결정
	DI_EEPROM_FIELD_CHECK_USB_COUNT,		// 위 동작 실패 시 몇 번 재 시도 할지 count를 저장
	DI_EEPROM_FIELD_ASPECT_RATIO,			// DI_EEPROM_ASPECT_RATIO_t					// 1		// DI_EEPROM_ASPECT_RATIO_t
	DI_EEPROM_FIELD_COUNTRY_CODE,			// HUINT8 a[3] 			// 3						// HUINT8 a[3]
	DI_EEPROM_FIELD_USAGE_ID,				// HUINT8					// 1						// for UPC OTA
	DI_EEPROM_FIELD_OTA_MODE,				// HUINT8					// 1						// for backgrond ota
	DI_EEPROM_FIELD_MAC_ADDRESS,
	DI_EEPROM_FIELD_FTP_OTA_INFO,
	DI_EEPROM_FIELD_DISPLAY_FACTORY_INFO,
	DI_EEPROM_FIELD_MAX,
	DI_EEPROM_FIELD_DUMMY = 0xFF
} DI_EEPROM_FIELD_t;

typedef enum
{
	DI_OTAFLAG_NO_OTA				= 0x00,
	DI_OTAFLAG_OTA_DETECTED		= 0x01,
	DI_OTAFLAG_OTA_COMPLETED	= 0x02,
	DI_OTAFLAG_FORCE_UPGRADE	= 0x04,
	DI_OTAFLAG_NAGRA_UPGRADE	= 0x08,
	DI_OTAFLAG_OTA_DOWNLOADING	= 0x10,
	DI_OTAFLAG_OTA_WRITING		= 0x20,
	DI_OTAFLAG_OTA_ERROR		= 0xFF
} DI_OTAFLAG_t;

typedef enum
{
	DI_BG_OTA_NONE = 0,
	DI_BG_OTA_FOR_NAND, 
	DI_BG_OTA_FOR_HDD
}DI_BG_OTA_MODE;

#define NET_ADDR_LENGTH 4
typedef struct tagDiEepromNetIpSetting
{
	HUINT8		ip[NET_ADDR_LENGTH];
	HUINT8		mask[NET_ADDR_LENGTH];
	HUINT8 		gateway[NET_ADDR_LENGTH];
	HUINT8		dns[NET_ADDR_LENGTH];
} DI_EEPROM_NET_IP_SETTING;

#define FTP_USER_LENGTH 		10
#define FTP_PASSWD_LENGTH 		10
#define FTP_DIR_PATH_LENGTH 	24
#define FTP_FILE_NAME_LENGTH 	20
typedef struct tagDiEepromFtpOtaSetting
{
	HUINT8		aServerIp[NET_ADDR_LENGTH];
	HUINT8 		aUserName[FTP_USER_LENGTH];
	HUINT8 		aPasswd[FTP_PASSWD_LENGTH];
	HUINT8 		aDirPath[FTP_DIR_PATH_LENGTH];
	HUINT8 		aFileName[FTP_FILE_NAME_LENGTH];

} DI_EEPROM_FTP_OTA_SETTING;

/*
* FTP OTA 관련 size 가 overwrite 된 상태로 이미 나간 모델(iCord HD+/ME, IR2020HD 등) 의 
* OTA 를 위해 overwirte 된 define & struct 를 추가 함.
* 그외 모델들은 size 가 수정된 동일한 구조의 위 define & struct 를 사용해야 함.
*/
#define FTP_USER_LENGTH_OVERWRITE			20
#define FTP_PASSWD_LENGTH_OVERWRITE			20
#define FTP_DIR_PATH_LENGTH_OVERWRITE		200
#define FTP_FILE_NAME_LENGTH_OVERWRITE		50
typedef struct tagDiEepromFtpOtaSettingOverWrite
{
	HUINT8		aServerIp[NET_ADDR_LENGTH];
	HUINT8		aUserName[FTP_USER_LENGTH_OVERWRITE];
	HUINT8		aPasswd[FTP_PASSWD_LENGTH_OVERWRITE];
	HUINT8		aDirPath[FTP_DIR_PATH_LENGTH_OVERWRITE];
	HUINT8		aFileName[FTP_FILE_NAME_LENGTH_OVERWRITE];
			 
} DI_EEPROM_FTP_OTA_SETTING_OVERWRITE;

typedef enum
{
	DI_EEPROM_IP_DHCP = 0,
	DI_EEPROM_IP_MANUAL
} DI_EEPROM_NET_IP_CONFIG;

typedef HUINT32 				DI_EEPROM_FIELD_SYSTEM_ID_t;
typedef DI_OTA_TYPE_t 			DI_EEPROM_FIELD_OTA_TYPE_t;
typedef DI_OTA_CHANNEL_t 		DI_EEPROM_FIELD_CH_TYPE_t;
typedef CH_SAT_TuneParam_t		DI_EEPROM_FIELD_CH_INFO_S_t;
typedef CH_TER_TuneParam_t		DI_EEPROM_FIELD_CH_INFO_T_t;
typedef CH_CAB_TuneParam_t		DI_EEPROM_FIELD_CH_INFO_C_t;
typedef HUINT16 				DI_EEPROM_FIELD_ES_PID_t;
typedef HUINT16 				DI_EEPROM_FIELD_LOADER_VERSION_t;
typedef HUINT8 					DI_EEPROM_FIELD_HDF_VERSION_t;
typedef HUINT8 					DI_EEPROM_FIELD_SW_UPGRADE_t;
typedef HUINT32 				DI_EEPROM_FIELD_APP_VERSION_t;
typedef HUINT16 				DI_EEPROM_FIELD_OTALDR_VERSION_t;
typedef HUINT16 				DI_EEPROM_FIELD_PANEL_ID_t;
typedef HUINT32 				DI_EEPROM_FIELD_STRING_VERSION_t;


#define DI_EEPROM_NUM_CHIP		(1)
#define DI_EEPROM_ONE_CHIP_SIZE	(0x2000)
#define DI_EEPROM_SIZE			(DI_EEPROM_ONE_CHIP_SIZE)


/****************************************************************************
* Global function prototypes
*****************************************************************************/

DI_ERR_CODE DI_EEPROM_GetCapabilities(unsigned long *pulEepromSize);
DI_ERR_CODE DI_EEPROM_Read(unsigned long address, unsigned char *data, unsigned long numToRead);
DI_ERR_CODE DI_EEPROM_Write(unsigned long address, unsigned char *data, unsigned long numToWrite);
DI_ERR_CODE DI_EEPROM_SetField(DI_EEPROM_FIELD_t type, void *data, HINT32 size);
DI_ERR_CODE DI_EEPROM_GetField(DI_EEPROM_FIELD_t type, void *data, HINT32 size);
#endif	/* _DI_EEPROM_H_ */

