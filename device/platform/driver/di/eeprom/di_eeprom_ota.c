#include "htype.h"
#include "vkernel.h"
#include "di_err.h"
#include "di_eeprom.h"
#include "di_eeprom_ota.h"


#include "di_channel.h"
#include "di_uart.h"

#include "di_channel_s.h"
#include "di_channel_t.h"
#include "di_channel_c.h"

#if defined(CONFIG_OTALOADER)
//#define HDEBUG
//#define HDEBUG_DEBUG
#include "hdebug.h"
#endif

static HBOOL bIsInit = FALSE;

#define DEBUG_ERR DI_UART_Print
#define DEBUG_INFO DI_UART_Print

#define XOR_KEY	0xA5

#define EEPROM_START_ADDR 		0x00
#define EEPROM_CRC_START_ADDR 	0x04
#define EEPROM_CRC_ADDR			0xAE

#define EEPROM_BACKUP_DATA 		0xB6
#define EEPROM_BACKUP_ADDR 		0xFA
#define EEPROM_BACKUP_SIZE 		0xFC
#define EEPROM_BACKUP_CRC_ADDR	0xFE
#define SIZE_EEPROMBACKUP  		0x4A	// largest block of eeprom data

#define SIZE_EEPROMDATA  		0xB0
#define SIZE_BACKUP_ADDR		0x02
#define SIZE_BACKUP_SIZE		0x02
#define SIZE_CRC  				0x02

#define EEPROM_CRC_OFFSET		0x5A5A

#define SIZE_CHANNEL_INFO_S				sizeof(CH_SAT_TuneParam_t)
#define SIZE_CHANNEL_INFO_T				sizeof(CH_TER_TuneParam_t)
#define SIZE_CHANNEL_INFO_C				sizeof(CH_CAB_TuneParam_t)
#define SIZE_CHANNEL_INFO_NET			SIZE_CHANNEL_INFO_S
#define SIZE_CHANNEL_INFO_FTP			sizeof(DI_EEPROM_FTP_OTA_SETTING)

#define SIZE_SYS_CHANNEL_INFO			SIZE_CHANNEL_INFO_S
#define 	SIZE_SYS_STORE_CHANNEL_INFO		SIZE_CHANNEL_INFO_S

#define SIZE_SYS_SYSTEMID				4
#define 	SIZE_SYS_STORE_SYSTEMID			4
#define SIZE_SYS_OTA_TYPE				4
#define 	SIZE_SYS_STORE_OTA_TYPE			1
#define SIZE_SYS_CHANNEL_TYPE			4
#define 	SIZE_SYS_STORE_CHANNEL_TYPE		1
#define SIZE_SYS_NET_PORTAL_IP			4
#define 	SIZE_SYS_STORE_NET_PORTAL_IP	4
#define SIZE_SYS_NET_IP_CONFIG			4
#define 	SIZE_SYS_STORE_NET_IP_CONFIG	1
#define SIZE_SYS_NET_IP_SETTING			0x10
#define 	SIZE_SYS_STORE_NET_IP_SETTING	0x10
#define SIZE_SYS_ES_PID					2
#define 	SIZE_SYS_STORE_ES_PID			2
#define SIZE_SYS_LOADER_VER				2
#define 	SIZE_SYS_STORE_LOADER_VER		2
#define SIZE_SYS_APP_VER				4
#define 	SIZE_SYS_STORE_APP_VER			4
#define SIZE_SYS_OTA_FLAG				4
#define 	SIZE_SYS_STORE_OTA_FLAG			1
#define SIZE_SYS_OTA_FILE				1
#define 	SIZE_SYS_STORE_OTA_FILE			1
#define SIZE_SYS_PANEL_STATUS			4
#define 	SIZE_SYS_STORE_PANEL_STATUS		1
#define SIZE_SYS_PANELID				2
#define 	SIZE_SYS_STORE_PANELID			2
#define SIZE_SYS_LANGUAGE				3
#define 	SIZE_SYS_STORE_LANGUAGE			3
#define SIZE_SYS_TV_STANDARD			4
#define 	SIZE_SYS_STORE_TV_STANDARD		1
#define SIZE_SYS_SCART_RGB				4
#define 	SIZE_SYS_STORE_SCART_RGB		1
#define SIZE_SYS_HD_RESOLUTION			4
#define 	SIZE_SYS_STORE_HD_RESOLUTION	1
#define SIZE_SYS_TUNER_ID				1
#define 	SIZE_SYS_STORE_TUNER_ID			1
#define SIZE_SYS_ANTENA_POWER			1
#define 	SIZE_SYS_STORE_ANTENA_POWER		1
#define SIZE_SYS_TRANSACTION_ID			2
#define 	SIZE_SYS_STORE_TRANSACTION_ID	2
#define SIZE_SYS_OAD_VER				8
#define 	SIZE_SYS_STORE_OAD_VER			8
#define SIZE_SYS_RCU_CUSTOM_CODE		2
#define 	SIZE_SYS_STORE_RCU_CUSTOM_CODE	2
#define SIZE_SYS_PANEL_DISPLAY			4
#define 	SIZE_SYS_STORE_PANEL_DISPLAY	1
#define SIZE_SYS_PANEL_DIMMING			1
#define 	SIZE_SYS_STORE_PANEL_DIMMING	1
#define SIZE_SYS_OSD_DISPLAY			4
#define 	SIZE_SYS_STORE_OSD_DISPLAY		1
#define SIZE_SYS_ASPECT_RATIO			4
#define 	SIZE_SYS_STORE_ASPECT_RATIO		1
#define SIZE_SYS_COUNTRY_CODE			4
#define 	SIZE_SYS_STORE_COUNTRY_CODE		3
#define SIZE_SYS_USAGE_ID				1
#define 	SIZE_SYS_STORE_USAGE_ID			1
#define SIZE_SYS_OTA_MODE				1
#define 	SIZE_SYS_STORE_OTA_MODE			1


static HUINT8 s_ucEEPROMData[SIZE_EEPROMDATA];
static HUINT8 s_ucEEPROMBackupData[SIZE_EEPROMBACKUP];
typedef enum StoreTypeE
{
	STORE_TYPE_NORMAL,
	STORE_TYPE_BIG_ENDIAN,
	STORE_TYPE_LITTLE_ENDIAN,
	STORE_TYPE_XOR_ENCRYPT
} STORE_TYPE_E;

typedef struct eeprom_map_info
{
	DI_EEPROM_FIELD_t	type;
	DI_EEPROM_FIELD_t	aliasType;
	HUINT16				address;
	HUINT16				size;
	HUINT16				storeSize;
	STORE_TYPE_E		storeType;
} DRV_EEPROM_MAP;

DRV_EEPROM_MAP s_DrvEepromMap[] =
{
	{DI_EEPROM_FIELD_SYSTEM_ID,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_SYSTEMID		,	SIZE_SYS_STORE_SYSTEMID			, STORE_TYPE_BIG_ENDIAN },
	{DI_EEPROM_FIELD_LANGUAGE,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_LANGUAGE		,	SIZE_SYS_STORE_LANGUAGE			, STORE_TYPE_XOR_ENCRYPT },
	{DI_EEPROM_FIELD_PANEL_STATUS,			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_PANEL_STATUS	,	SIZE_SYS_STORE_PANEL_STATUS		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_PANEL_ID,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_PANELID		,	SIZE_SYS_STORE_PANELID			, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_TV_STANDARD,			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_TV_STANDARD	,	SIZE_SYS_STORE_TV_STANDARD		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_SCART_RGB,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_SCART_RGB		,	SIZE_SYS_STORE_SCART_RGB		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_HD_RESOULTION,			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_HD_RESOLUTION	,	SIZE_SYS_STORE_HD_RESOLUTION	, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_SW_UPGRADE,			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_OTA_FLAG		,	SIZE_SYS_STORE_OTA_FLAG			, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_OTA_TYPE,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_OTA_TYPE		,	SIZE_SYS_STORE_OTA_TYPE			, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_CH_TYPE,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_CHANNEL_TYPE	,	SIZE_SYS_STORE_CHANNEL_TYPE		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_TUNER_ID,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_TUNER_ID		,	SIZE_SYS_STORE_TUNER_ID			, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_ANTENA_POWER,			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_ANTENA_POWER	,	SIZE_SYS_STORE_ANTENA_POWER		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_CHANNEL_INFO,			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_CHANNEL_INFO	,	SIZE_SYS_STORE_CHANNEL_INFO		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_ES_PID,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_ES_PID			,	SIZE_SYS_STORE_ES_PID			, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_TRANSACTION_ID,		DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_TRANSACTION_ID	,	SIZE_SYS_STORE_TRANSACTION_ID	, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_APP_VERSION,			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_APP_VER		,	SIZE_SYS_STORE_APP_VER			, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_OTALDR_VERSION,		DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_LOADER_VER		,	SIZE_SYS_STORE_LOADER_VER		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_CUR_OTAIMAGE_VERSION,	DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_LOADER_VER		,	SIZE_SYS_STORE_LOADER_VER		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_OAD_VERSION,			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_OAD_VER		,	SIZE_SYS_STORE_OAD_VER			, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_OTA_FILE,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_OTA_FILE		,	SIZE_SYS_STORE_OTA_FILE			, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_NET_IP_CONFIG, 		DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_NET_IP_CONFIG	,	SIZE_SYS_STORE_NET_IP_CONFIG 	, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_NET_IP_SETTING,		DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_NET_IP_SETTING	,	SIZE_SYS_STORE_NET_IP_SETTING 	, STORE_TYPE_XOR_ENCRYPT },
	{DI_EEPROM_FIELD_RCU_CUSTOM_CODE,		DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_RCU_CUSTOM_CODE ,	SIZE_SYS_STORE_RCU_CUSTOM_CODE	, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_RCU_CUSTOM_CODE2,		DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_RCU_CUSTOM_CODE ,	SIZE_SYS_STORE_RCU_CUSTOM_CODE	, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_PANEL_DISPLAY,			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_PANEL_DISPLAY ,	SIZE_SYS_STORE_PANEL_DISPLAY	, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_PANEL_DIMMING, 		DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_PANEL_DIMMING ,	SIZE_SYS_STORE_PANEL_DIMMING	, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_ASPECT_RATIO, 			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_ASPECT_RATIO	,	SIZE_SYS_STORE_ASPECT_RATIO		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_COUNTRY_CODE,			DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_COUNTRY_CODE	,	SIZE_SYS_STORE_COUNTRY_CODE 	, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_USAGE_ID,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_USAGE_ID		,	SIZE_SYS_STORE_USAGE_ID 		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_OTA_MODE,				DI_EEPROM_FIELD_DUMMY,			0,	SIZE_SYS_OTA_MODE		,	SIZE_SYS_STORE_OTA_MODE 		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_DUMMY,					DI_EEPROM_FIELD_DUMMY,			0,	0						,	0						, 0		}
};
DRV_EEPROM_MAP s_DrvEepromAliasMap[] =
{
	{DI_EEPROM_FIELD_CH_INFO_S,				DI_EEPROM_FIELD_CHANNEL_INFO,	0,	SIZE_CHANNEL_INFO_S		,	SIZE_CHANNEL_INFO_S				, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_CH_INFO_T,				DI_EEPROM_FIELD_CHANNEL_INFO,	0,	SIZE_CHANNEL_INFO_T		,	SIZE_CHANNEL_INFO_T				, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_CH_INFO_C,				DI_EEPROM_FIELD_CHANNEL_INFO,	0,	SIZE_CHANNEL_INFO_C		,	SIZE_CHANNEL_INFO_C				, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_NET_PORTAL_IP,			DI_EEPROM_FIELD_ES_PID,			0,	SIZE_SYS_NET_PORTAL_IP 	,	SIZE_SYS_STORE_NET_PORTAL_IP	, STORE_TYPE_XOR_ENCRYPT },
	{DI_EEPROM_FIELD_NET_DOWN_PATH, 		DI_EEPROM_FIELD_CHANNEL_INFO,	0,	SIZE_CHANNEL_INFO_NET	,	SIZE_CHANNEL_INFO_NET 			, STORE_TYPE_XOR_ENCRYPT },
	{DI_EEPROM_FIELD_LOADER_VERSION,		DI_EEPROM_FIELD_OTALDR_VERSION,	0,	SIZE_SYS_LOADER_VER		,	SIZE_SYS_STORE_LOADER_VER		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_OSD_DISPLAY,			DI_EEPROM_FIELD_PANEL_STATUS, 0,	SIZE_SYS_OSD_DISPLAY 	,	SIZE_SYS_STORE_OSD_DISPLAY		, STORE_TYPE_NORMAL },
	{DI_EEPROM_FIELD_FTP_OTA_INFO,			DI_EEPROM_FIELD_CHANNEL_INFO, 0,	SIZE_CHANNEL_INFO_FTP 	,	SIZE_CHANNEL_INFO_FTP		, STORE_TYPE_XOR_ENCRYPT },
	{DI_EEPROM_FIELD_DUMMY, 				DI_EEPROM_FIELD_DUMMY, 			0,	0						,	0						, 0		}
};
HINT32		s_DrvEepromMapSize;
HINT32		s_DrvEepromAliasMapSize;

static const unsigned short s_ausCRC_16_TABLE[256] =
{
	0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
	0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
	0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
	0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
	0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
	0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
	0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
	0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
	0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
	0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
	0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
	0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
	0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
	0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
	0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
	0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
	0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
	0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
	0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
	0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
	0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
	0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
	0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
	0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
	0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
	0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
	0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
	0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
	0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
	0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
	0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
	0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

extern DI_ERR_CODE DRV_EEPROM_Write(HUINT32 usAddress, HUINT8* pucBuffer, HUINT32 ulSize);
extern DI_ERR_CODE DRV_EEPROM_Read(HUINT32 usAddress, HUINT8* pucBuffer, HUINT32 ulSize);
DI_ERR_CODE DRV_EEPROM_GetFieldInfo(DI_EEPROM_FIELD_t type, HUINT32 *pAddress, HUINT32 *pSize, HUINT32 *pStoreSize, STORE_TYPE_E *pEndian);

unsigned short DRV_UTIL_GetCrc16(unsigned char *BufferPtr, unsigned short NumberOfByte)
{
	unsigned short		CRC ;

	CRC = 0;
	while (NumberOfByte--)
		CRC = (CRC >> 8) ^ s_ausCRC_16_TABLE[(CRC ^ *BufferPtr++) & 0xff] ;

	return (CRC+EEPROM_CRC_OFFSET) ;
} /* GetCrc16 */

#if defined(CONFIG_OTALOADER)
#else
static void DRV_EEPROM_SetDefault(void)
{
	HUINT16 usGetCRC = 0;
	HUINT16	*p;

	VK_MEM_Memset(s_ucEEPROMData, 0, SIZE_EEPROMDATA);
	VK_MEM_Memset(s_ucEEPROMBackupData, 0, SIZE_EEPROMBACKUP);

	usGetCRC = DRV_UTIL_GetCrc16(s_ucEEPROMData+EEPROM_CRC_START_ADDR, SIZE_EEPROMDATA - EEPROM_CRC_START_ADDR - SIZE_CRC);
	p = (HUINT16*)(s_ucEEPROMData + SIZE_EEPROMDATA - SIZE_CRC);
	*p = usGetCRC;
	DEBUG_ERR("EEPROM CRC:0x%x\n", usGetCRC);
	usGetCRC = DRV_UTIL_GetCrc16(s_ucEEPROMBackupData, SIZE_EEPROMBACKUP - SIZE_CRC);
	p = (HUINT16*)(s_ucEEPROMBackupData + SIZE_EEPROMBACKUP - SIZE_CRC);
	*p = usGetCRC;
	DRV_EEPROM_Write(EEPROM_CRC_START_ADDR,s_ucEEPROMData+EEPROM_CRC_START_ADDR, SIZE_EEPROMDATA-EEPROM_CRC_START_ADDR);
	DRV_EEPROM_Write(EEPROM_BACKUP_DATA,s_ucEEPROMBackupData,SIZE_EEPROMBACKUP);
}
#endif

void DRV_EPRMMGR_Init(void)
{
	HUINT16	usGetCRC, usReadCrc;
	HUINT16	usBackupAddr, usBackupSize;
	DI_ERR_CODE  etRtnCode = DI_ERR_OK;

	DRV_EEPROM_Read(EEPROM_START_ADDR, s_ucEEPROMData, SIZE_EEPROMDATA);
	DRV_EEPROM_Read(EEPROM_BACKUP_DATA, s_ucEEPROMBackupData, SIZE_EEPROMBACKUP);

	usGetCRC = DRV_UTIL_GetCrc16(s_ucEEPROMData+EEPROM_CRC_START_ADDR, SIZE_EEPROMDATA - EEPROM_CRC_START_ADDR - SIZE_CRC);
	DRV_EEPROM_Read(EEPROM_CRC_ADDR, (HUINT8 *)&usReadCrc, SIZE_CRC);
	if (usGetCRC != usReadCrc)
	{
		DEBUG_ERR("error EEPROM is corrupt\n");
		DRV_EEPROM_Read(EEPROM_BACKUP_ADDR, 	(HUINT8 *)&usBackupAddr, SIZE_BACKUP_ADDR);
		DRV_EEPROM_Read(EEPROM_BACKUP_SIZE, 	(HUINT8 *)&usBackupSize, SIZE_BACKUP_SIZE);
		usGetCRC = DRV_UTIL_GetCrc16(s_ucEEPROMBackupData, usBackupSize);
		DRV_EEPROM_Read(EEPROM_BACKUP_CRC_ADDR, (HUINT8 *)&usReadCrc, SIZE_CRC);
		if (usGetCRC == usReadCrc && ((usBackupAddr+usBackupSize)<SIZE_EEPROMDATA))
		{
			DEBUG_ERR("restore from backup\n");
			VK_MEM_Memcpy(s_ucEEPROMData+usBackupAddr, s_ucEEPROMBackupData, usBackupSize);
			usGetCRC = DRV_UTIL_GetCrc16((void*)(s_ucEEPROMData + EEPROM_CRC_START_ADDR), SIZE_EEPROMDATA - EEPROM_CRC_START_ADDR - SIZE_CRC);
			VK_MEM_Memcpy((HUINT8*)s_ucEEPROMData + EEPROM_CRC_ADDR, &usGetCRC, SIZE_CRC);

			etRtnCode = DRV_EEPROM_Write(EEPROM_START_ADDR + usBackupAddr, s_ucEEPROMData+usBackupAddr, usBackupSize);
			etRtnCode |= DRV_EEPROM_Write(EEPROM_CRC_ADDR, (HUINT8 *)&usGetCRC, SIZE_CRC );
			if(etRtnCode  != DI_ERR_OK)
			{
				DEBUG_ERR("error at write n backup %d line\n", __LINE__);
				return;
			}
		}
		else
		{
#if defined(CONFIG_OTALOADER)
#else
			DEBUG_ERR("All EEPROM DATA corrupt !!! \n");
			DRV_EEPROM_SetDefault();
#endif
		}
	}

	bIsInit = TRUE;

	return;
}

static DI_ERR_CODE P_EEPROM_GetFieldInfo(DI_EEPROM_FIELD_t type, HUINT32 *pAddress, HUINT32 *pSize, HUINT32 *pStoreSize, STORE_TYPE_E *pEndian)
{
	HINT32	i;

	for (i=0; i<s_DrvEepromMapSize; i++)
	{
		if (type == s_DrvEepromMap[i].type)
		{
			*pAddress = s_DrvEepromMap[i].address;
			*pSize = s_DrvEepromMap[i].size;
			*pStoreSize = s_DrvEepromMap[i].storeSize;
			*pEndian = s_DrvEepromMap[i].storeType;
			return DI_ERR_OK;
		}
	}
	return 1;
}

DI_ERR_CODE DRV_EEPROM_MapInit(void)
{
	HINT32	i;
	HUINT32	addrss, size, storeSize;
	STORE_TYPE_E	storeType;
	DI_ERR_CODE		result;

	s_DrvEepromMapSize = sizeof(s_DrvEepromMap)/sizeof(s_DrvEepromMap[0]);
	s_DrvEepromMap[0].address = 0;
	for (i=1; i<s_DrvEepromMapSize; i++)
	{
		s_DrvEepromMap[i].address = s_DrvEepromMap[i-1].address + s_DrvEepromMap[i-1].storeSize;
//		DI_UART_Print("EEPROM MAP : %d - %04X(%04X)\n", i, s_DrvEepromMap[i].address, s_DrvEepromMap[i].storeSize);
	}
	if (s_DrvEepromMap[s_DrvEepromMapSize-1].address + s_DrvEepromMap[s_DrvEepromMapSize-1].storeSize > SIZE_EEPROMDATA)
	{
		DI_UART_Print("[DRV_EEPROM_MapInit] error : map address exceed addr(%04X) size (%04X)\n", s_DrvEepromMap[s_DrvEepromMapSize-1].address, s_DrvEepromMap[s_DrvEepromMapSize-1].storeSize);
	}
	s_DrvEepromAliasMapSize = sizeof(s_DrvEepromAliasMap)/sizeof(s_DrvEepromAliasMap[0]);
	for (i=0; i<s_DrvEepromAliasMapSize; i++)
	{
		result = P_EEPROM_GetFieldInfo(s_DrvEepromAliasMap[i].aliasType, &addrss, &size, &storeSize, &storeType);
		if (result != DI_ERR_OK)
			continue;
		s_DrvEepromAliasMap[i].address = (HUINT16)addrss;
//		DI_UART_Print("EEPROM ALI : %d - %04X(%04X)\n", i, s_DrvEepromAliasMap[i].address, s_DrvEepromAliasMap[i].storeSize);
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DRV_EEPROM_GetFieldInfo(DI_EEPROM_FIELD_t type, HUINT32 *pAddress, HUINT32 *pSize, HUINT32 *pStoreSize, STORE_TYPE_E *pEndian)
{
	HINT32	i;

	for (i=0; i<s_DrvEepromMapSize; i++)
	{
		if (type == s_DrvEepromMap[i].type)
		{
			*pAddress = s_DrvEepromMap[i].address;
			*pSize = s_DrvEepromMap[i].size;
			*pStoreSize = s_DrvEepromMap[i].storeSize;
			*pEndian = s_DrvEepromMap[i].storeType;
			return DI_ERR_OK;
		}
	}
	for (i=0; i<s_DrvEepromAliasMapSize; i++)
	{
		if (type == s_DrvEepromAliasMap[i].type)
		{
			*pAddress = s_DrvEepromAliasMap[i].address;
			*pSize = s_DrvEepromAliasMap[i].size;
			*pStoreSize = s_DrvEepromAliasMap[i].storeSize;
			*pEndian = s_DrvEepromAliasMap[i].storeType;
			return DI_ERR_OK;
		}
	}
	return 1;
}

static DI_ERR_CODE DRV_EPRMMGR_Write(HUINT32 ulOffset, HUINT8 * pData,  HUINT32 ulSize)
{
	HUINT16 usGetCRC;
	DI_ERR_CODE  etRtnCode = DI_ERR_OK;

	if (bIsInit != TRUE)
	{
		DRV_EPRMMGR_Init();
		if(bIsInit != TRUE)
		{
			DEBUG_ERR("[DRV_EPRMMGR_Write] (bIsInit != TRUE)\n");
			return DI_ERR_ERROR;
		}
	}

	VK_MEM_Memcpy((HUINT8*)s_ucEEPROMBackupData, pData, ulSize);
	usGetCRC = DRV_UTIL_GetCrc16((void*)s_ucEEPROMBackupData, (HUINT16)ulSize);
	VK_MEM_Memcpy((HUINT8*)s_ucEEPROMBackupData + SIZE_EEPROMBACKUP - SIZE_CRC, &usGetCRC, SIZE_CRC);

	VK_MEM_Memcpy((HUINT8*)s_ucEEPROMData + ulOffset, pData, ulSize);
	usGetCRC = DRV_UTIL_GetCrc16((void*)(s_ucEEPROMData+EEPROM_CRC_START_ADDR), SIZE_EEPROMDATA - EEPROM_CRC_START_ADDR - SIZE_CRC);
	VK_MEM_Memcpy((HUINT8*)s_ucEEPROMData + EEPROM_CRC_ADDR, &usGetCRC, SIZE_CRC);

	etRtnCode = DRV_EEPROM_Write(EEPROM_BACKUP_DATA, pData, ulSize);
	etRtnCode |= DRV_EEPROM_Write(EEPROM_BACKUP_ADDR, (HUINT8 *)s_ucEEPROMBackupData+(SIZE_EEPROMBACKUP-(SIZE_BACKUP_ADDR+SIZE_BACKUP_SIZE+SIZE_CRC)), SIZE_BACKUP_ADDR+SIZE_BACKUP_SIZE+SIZE_CRC);

	etRtnCode |= DRV_EEPROM_Write(EEPROM_START_ADDR + ulOffset, pData, ulSize);
	etRtnCode |= DRV_EEPROM_Write(EEPROM_CRC_ADDR, (HUINT8 *)&usGetCRC, SIZE_CRC );

	if(etRtnCode  != DI_ERR_OK)
	{
		DEBUG_ERR("error at write n backup %d line\n", __LINE__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE  DRV_EEPROM_SetXorKey(HUINT8 *pszString, HINT32 nSize)
{
	HINT32 i;

	for (i=0; i<nSize; i++)
	{
		pszString[i] ^= XOR_KEY;
	}
	return DI_ERR_OK;
}

static HUINT32 EEPROM_SWAP32(HUINT32 A)
{
	return ((A << 24) | ((A << 8) &  0xFF0000) | ((A >> 8) &  0xFF00) |(A >> 24));
}

DI_ERR_CODE DRV_EEPROM_GetField(DI_EEPROM_FIELD_t type, void *data, HINT32 nDataSize)
{
	DI_ERR_CODE errCode = DI_ERR_ERROR;
	HUINT32		address, size, storeSize;
	STORE_TYPE_E	storeType;

	if( data == NULL)
	{
		return DI_ERR_INVALID_PARAM;
	}

	errCode = DRV_EEPROM_GetFieldInfo(type, &address, &size, &storeSize, &storeType);
	if (errCode != DI_ERR_OK)
	{
		DEBUG_ERR("[DRV_EEPROM_GetField] type(%d) error(%08X) : DRV_EEPROM_GetFieldInfo\n", type, errCode);
		return errCode;
	}
	if (size != (HUINT32)nDataSize)
	{
		DEBUG_ERR("[DRV_EEPROM_GetField] ERROR : wrong size of the field type(%d) wanted size(%d) input size (%d)\n", type, size, nDataSize);
		size = nDataSize;
	}

	switch (storeType)
	{
	case STORE_TYPE_BIG_ENDIAN :
		if ((size & 0x03) == 0)
		{
			HUINT32	i;
			HUINT32	*pInData;

			pInData = data;
			errCode = DRV_EEPROM_Read(address, (HUINT8*)pInData, size);
			if (errCode != DI_ERR_OK)
			{
				return errCode;
			}
			for (i=0; i<size; i+=4)
			{
				*pInData = EEPROM_SWAP32(*pInData);
				pInData++;
			}
		}
		break;
	case STORE_TYPE_XOR_ENCRYPT :
		{
			HUINT8 *pucData;

			pucData = (HUINT8*)DD_MEM_Alloc(size);
			if (pucData == NULL)
			{
				DEBUG_ERR("[DRV_EEPROM_GetField] error(%08X) : DD_MEM_Alloc = NULL\n");
				return 1;
			}
			errCode = DRV_EEPROM_Read(address, pucData, size);
			if (errCode != DI_ERR_OK)
			{
				DEBUG_ERR("[DRV_EEPROM_GetField] error(%08X) : DRV_EEPROM_Read\n", errCode);
				return errCode;
			}
			DRV_EEPROM_SetXorKey(pucData, size);
			VK_MEM_Memcpy(data, pucData, size);
			DD_MEM_Free(pucData);
		}
		break;
	default :
		if (size != storeSize)
		{
			if (size == 4U && storeSize == 1U)
			{
				HUINT8	ucData;
				HUINT32	*pulData;
				pulData = data;

				errCode = DRV_EEPROM_Read(address, &ucData, storeSize);
				if (errCode != DI_ERR_OK)
				{
					DEBUG_ERR("[DRV_EEPROM_GetField] error(%08X) : DRV_EPRMMGR_Write\n", errCode);
					return errCode;
				}
				*pulData = ucData;
				break;
			}
			else if (size == 4U && storeSize == 3U)
			{
				HUINT8	*pucData;
				pucData = data;

				errCode = DRV_EEPROM_Read(address, pucData, storeSize);
				if (errCode != DI_ERR_OK)
				{
					DEBUG_ERR("[DRV_EEPROM_GetField] error(%08X) : DRV_EPRMMGR_Write\n", errCode);
					return errCode;
				}
				pucData[3] = 0;
				break;
			}
		}
		errCode = DRV_EEPROM_Read(address, data, size);
		if (errCode != DI_ERR_OK)
		{
			DEBUG_ERR("[DRV_EEPROM_GetField] type(%d) error(%08X) : DRV_EEPROM_Read\n", type, errCode);
			return errCode;
		}
		break;
	}

	return errCode;
}


/*********************************************************************
 * Function 	DI_EEPROM_ReadPanelInfo
 * Description
 * Inputs
 * Outputs
 * Returns
 *********************************************************************/
DI_ERR_CODE DI_EEPROM_SetField(DI_EEPROM_FIELD_t type, void *data, HINT32 nDataSize)
{
	DI_ERR_CODE errCode = DI_ERR_ERROR;
	HUINT32		address, size, storeSize;
	STORE_TYPE_E	storeType;

	if( data == NULL)
	{
		return DI_ERR_INVALID_PARAM;
	}

	errCode = DRV_EEPROM_GetFieldInfo(type, &address, &size, &storeSize, &storeType);
	if (errCode != DI_ERR_OK)
	{
		DEBUG_ERR("[DI_EEPROM_SetField] error(%08X) : DRV_EEPROM_GetFieldInfo, type (%d)\n", errCode, type);
		return errCode;
	}
	if (size != (HUINT32)nDataSize)
	{
		DEBUG_ERR("[DI_EEPROM_SetField] ERROR : wrong size of the field type(%d) wanted size(%d) input size (%d)\n", type, size, nDataSize);
		size = nDataSize;
	}

	switch (storeType)
	{
	case STORE_TYPE_BIG_ENDIAN :
		if ((size & 0x03) == 0)
		{
			HUINT32	i;
			HUINT32	*pInData;
			HUINT32 *p, *pOrg;

			pOrg = p = (HUINT32*)DD_MEM_Alloc(size);
			if (p == NULL)
			{
				DEBUG_ERR("[DI_EEPROM_SetField] error(%08X) : DD_MEM_Alloc = NULL\n");
				return 1;
			}
			pInData = data;
			for (i=0; i<size; i+=4)
			{
				*p++ = EEPROM_SWAP32(*pInData++);
			}
			errCode = DRV_EPRMMGR_Write(address, (HUINT8*)pOrg, size);
			if (errCode != DI_ERR_OK)
			{
				DEBUG_ERR("[DI_EEPROM_SetField] error(%08X) : DRV_EPRMMGR_Write\n", errCode);
				return errCode;
			}
			DD_MEM_Free(pOrg);
		}
		break;
	case STORE_TYPE_XOR_ENCRYPT :
		{
			HUINT8 *pucData;

			pucData = (HUINT8*)DD_MEM_Alloc(size);
			if (pucData == NULL)
			{
				DEBUG_ERR("[DI_EEPROM_SetField] error(%08X) : DD_MEM_Alloc = NULL\n");
				return 1;
			}
			VK_MEM_Memcpy(pucData, data, size);
			DRV_EEPROM_SetXorKey(pucData, size);
			errCode = DRV_EPRMMGR_Write(address, pucData, size);
			DD_MEM_Free(pucData);
			if (errCode != DI_ERR_OK)
			{
				DEBUG_ERR("[DI_EEPROM_SetField] error(%08X) : DRV_EPRMMGR_Write\n", errCode);
				return errCode;
			}
		}
		break;
	default :
		if (size != storeSize)
		{
			if (size == 4U && storeSize == 1U)
			{
				HUINT8	aucData[1];
				HUINT32	*pulData;
				pulData = data;
				aucData[0] = (HUINT8)((HUINT32)*pulData);

				errCode = DRV_EPRMMGR_Write(address, aucData, storeSize);
				if (errCode != DI_ERR_OK)
				{
					DEBUG_ERR("[DI_EEPROM_SetField] error(%08X) : DRV_EPRMMGR_Write\n", errCode);
					return errCode;
				}
				break;
			}
			else if (size == 4U && storeSize == 3U)
			{
				HUINT8	*pucData;
				pucData = data;

				errCode = DRV_EPRMMGR_Write(address, pucData, storeSize);
				if (errCode != DI_ERR_OK)
				{
					DEBUG_ERR("[DI_EEPROM_SetField] error(%08X) : DRV_EPRMMGR_Write\n", errCode);
					return errCode;
				}
				break;
			}
		}
		errCode = DRV_EPRMMGR_Write(address, data, size);
		if (errCode != DI_ERR_OK)
		{
			DEBUG_ERR("[DI_EEPROM_SetField] error(%08X) : DRV_EPRMMGR_Write\n", errCode);
			return errCode;
		}
		break;
	}

	return errCode;
}


/*********************************************************************
 * Function 	DI_EEPROM_ReadPanelInfo
 * Description
 * Inputs
 * Outputs
 * Returns
 *********************************************************************/
DI_ERR_CODE DI_EEPROM_GetField(DI_EEPROM_FIELD_t type, void *data, HINT32 nDataSize)
{
	return DRV_EEPROM_GetField(type, data, nDataSize);
}

/*********************************************************************
 * Function 	DI_EEPROM_ReadPanelInfo
 * Description
 * Inputs
 * Outputs
 * Returns
 *********************************************************************/
unsigned long DI_EEPROM_GetFieldSize(DI_EEPROM_FIELD_t type)
{
	DI_ERR_CODE errCode = DI_ERR_ERROR;
	HUINT32 address, size, storeSize;
	STORE_TYPE_E	storeType;

	errCode = DRV_EEPROM_GetFieldInfo(type, &address, &size, &storeSize, &storeType);
	if (errCode != DI_ERR_OK)
	{
		DEBUG_ERR("[DI_EEPROM_GetFieldSize] error(%08X) : DRV_EEPROM_GetFieldInfo\n", errCode);
		return 0;
	}
	return size;
}


