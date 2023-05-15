/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_param.h
	@brief	  Octo pal system information header

	Description: Product 정보를 제공하는 system information PAL의 헤더 파일.\n
	Module: PAL/SYS			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_PARAM_H__
#define	__PAL_PARAM_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

//#define	SUPPORT_PAL_PARAM

#if defined(SUPPORT_PAL_PARAM)
#define	PalParam_Activated()			TRUE
#else
#define	PalParam_Activated()			FALSE
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/* ========================================================================================================================================================================================================================================================
 *	Key										// Source Type,		DI Key,										Data Type,				Pal Size,								Di Size,								Converter
 * ========================================================================================================================================================================================================================================================
 */
typedef enum
{
	ePAL_PARAM_SERIAL_NO = 0,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_SERIAL_NO,					ePAL_PARAM_TYPE_BIN,	HUMAX_SERIAL_NUMBER_SIZE(14),			<=, 									No Converter
	ePAL_PARAM_DTCP_KEY,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_DTCP_KEY,					ePAL_PARAM_TYPE_BIN,	?,										<=,										No Converter
	ePAL_PARAM_CI_PLUS_KEY,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_CI_PLUS_KEY,					ePAL_PARAM_TYPE_BIN,	GetAlignedLen(sizeof(CI_HOST_LICENSE_CONST)),<=,								No Converter
	ePAL_PARAM_MAC_ADDR,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_MAC_ADDR,					ePAL_PARAM_TYPE_BIN,	?,										<=,										No Converter
	ePAL_PARAM_HDCP_KEY,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_HDCP_KEY,					ePAL_PARAM_TYPE_BIN,	?,										<=,										No Converter
	ePAL_PARAM_PARING_DATA,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_PARING_DATA,					ePAL_PARAM_TYPE_BIN,	?,										<=,										No Converter
	ePAL_PARAM_SYSTEM_ID,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_SYSTEM_ID,					ePAL_PARAM_TYPE_INT,	sizeof(HUINT32),						<=,										No Converter
	ePAL_PARAM_LOADER_VERSION,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_LOADER_VERSION,				ePAL_PARAM_TYPE_INT,	sizeof(HUINT32),						<=,										No Converter
	ePAL_PARAM_APP_VERSION,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_APP_VERSION_NEXT,			ePAL_PARAM_TYPE_INT,	sizeof(HUINT32),						<=,										No Converter
	ePAL_PARAM_APP_VERSION_NEXT,			// eSOURCE_NVRAM,	DI_NVRAM_FIELD_APP_VERSION,					ePAL_PARAM_TYPE_INT,	sizeof(HUINT32),						<=,										No Converter
	ePAL_PARAM_PRIVATE,						// eSOURCE_NVRAM,	DI_NVRAM_FIELD_PRIVATE,						ePAL_PARAM_TYPE_BIN,	?,										<=,										No Converter

	ePAL_PARAM_CSC_KEY,						// eSOURCE_NVRAM,	DI_NVRAM_FIELD_CSC_KEY,						ePAL_PARAM_TYPE_BIN,	CSC_FLASH_SIZE,							<=,										No Converter
	ePAL_PARAM_DSTB_ID,						// eSOURCE_NVRAM,	DI_NVRAM_FIELD_DSTB_ID,						ePAL_PARAM_TYPE_BIN,	?,										<=,										No Converter

	ePAL_PARAM_IRDETO_BBCB,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_BBCB,					ePAL_PARAM_TYPE_BIN,	IR_BBCB_SIZE,							<=,										No Converter
	ePAL_PARAM_IRDETO_UC_SECURE_ID,			// eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_UC_SECURE_ID,			ePAL_PARAM_TYPE_BIN,	IRUC_FLASH_SECURE_ID_SIZE,				<=,										No Converter
	ePAL_PARAM_IRDETO_UC_PRIVATE_DATA,		// eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_UC_PRIVATE_DATA,		ePAL_PARAM_TYPE_BIN,	IRUC_FLASH_PRIVATEDATA_SIZE,			<=,										No Converter
	ePAL_PARAM_IRDETO_CPCB,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_CPCB,					ePAL_PARAM_TYPE_BIN,	IR_CPCB_SIZE,							<=,										No Converter
	ePAL_PARAM_IRDETO_CPCB_ENCRYPTED,		// eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED,		ePAL_PARAM_TYPE_BIN,	IR_CPCB_SIZE,							<=,										No Converter
	ePAL_PARAM_IRDETO_CA_PARTITION,			// eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_CA_PARTITION,			ePAL_PARAM_TYPE_BIN,	IR_PARTITION_CA_SIZE,					<=,										No Converter
	ePAL_PARAM_IRDETO_LOADER_PARTITION,		// eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_LOADER_PARTITION,		ePAL_PARAM_TYPE_BIN,	IR_PARTITION_LOADER_SIZE,				<=,										No Converter
	ePAL_PARAM_IRDETO_IR_PARTITION,			// eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_IR_PARTITION,			ePAL_PARAM_TYPE_BIN,	IR_PARTITION_IRDETO_SIZE,				<=,										No Converter
	ePAL_PARAM_IRDETO_IR_BACKUP_PARTITION,	// eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_IR_BACKUP_PARTITION,	ePAL_PARAM_TYPE_BIN,	IR_PARTITION_IRDETO_BACKUP_SIZE,		<=,										No Converter

	ePAL_PARAM_TUNER_ID1,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_TUNER_ID1,					ePAL_PARAM_TYPE_INT,	sizeof(HUINT8),							<=,										No Converter
	ePAL_PARAM_TUNER_ID2,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_TUNER_ID2,					ePAL_PARAM_TYPE_INT,	sizeof(HUINT8),							<=,										No Converter
	ePAL_PARAM_TUNER_ID3,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_TUNER_ID3,					ePAL_PARAM_TYPE_INT,	sizeof(HUINT8),							<=,										No Converter
	ePAL_PARAM_ANTENA_POWER1,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_ANTENA_POWER1,				ePAL_PARAM_TYPE_INT,	sizeof(HUINT8),							<=,										No Converter
	ePAL_PARAM_ANTENA_POWER2,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_ANTENA_POWER2,				ePAL_PARAM_TYPE_INT,	sizeof(HUINT8),							<=,										No Converter
	ePAL_PARAM_ANTENA_POWER3,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_ANTENA_POWER3,				ePAL_PARAM_TYPE_INT,	sizeof(HUINT8),							<=,										No Converter
	ePAL_PARAM_SWUP_FLAG1,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_FLAG1,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_OTA_FLAG_E),			sizeof(SWUP_Flag_e),					pal_param_ConverterSwupFlag
	ePAL_PARAM_SWUP_FLAG2,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_FLAG2,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_OTA_FLAG_E),			sizeof(SWUP_Flag_e),					pal_param_ConverterSwupFlag
	ePAL_PARAM_SWUP_FLAG3,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_FLAG3,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_OTA_FLAG_E),			sizeof(SWUP_Flag_e),					pal_param_ConverterSwupFlag
	ePAL_PARAM_SWUP_TYPE1,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_TYPE1,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_OTA_TYPE_E),			sizeof(SWUP_DownloadType_e),			pal_param_ConverterSwupType
	ePAL_PARAM_SWUP_TYPE2,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_TYPE2,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_OTA_TYPE_E),			sizeof(SWUP_DownloadType_e),			pal_param_ConverterSwupType
	ePAL_PARAM_SWUP_TYPE3,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_TYPE3,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_OTA_TYPE_E),			sizeof(SWUP_DownloadType_e),			pal_param_ConverterSwupType
	ePAL_PARAM_SWUP_CH_TYPE1, 				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_CH_TYPE1,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_OTA_CHANNEL_E),			sizeof(DxDeliveryType_b),				pal_param_ConverterSwupChType
	ePAL_PARAM_SWUP_CH_TYPE2,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_CH_TYPE2,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_OTA_CHANNEL_E),			sizeof(DxDeliveryType_b),				pal_param_ConverterSwupChType
	ePAL_PARAM_SWUP_CH_TYPE3,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_CH_TYPE3,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_OTA_CHANNEL_E),			sizeof(DxDeliveryType_b),				pal_param_ConverterSwupChType

	ePAL_PARAM_ES_PID1,						// eSOURCE_NVRAM,	DI_NVRAM_FIELD_ES_PID1,						ePAL_PARAM_TYPE_INT,	sizeof(HUINT16),						<=,										N/A
	ePAL_PARAM_ES_PID2,						// eSOURCE_NVRAM,	DI_NVRAM_FIELD_ES_PID2,						ePAL_PARAM_TYPE_INT,	sizeof(HUINT16),						<=,										N/A
	ePAL_PARAM_ES_PID3,						// eSOURCE_NVRAM,	DI_NVRAM_FIELD_ES_PID3,						ePAL_PARAM_TYPE_INT,	sizeof(HUINT16),						<=,										N/A
	ePAL_PARAM_NET_PORTAL_IP,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_NET_PORTAL_IP,				ePAL_PARAM_TYPE_BIN,	sizeof(HUINT8)*INET_ADDR_LENGTH,		<=,										N/A
	ePAL_PARAM_NET_DOWN_PATH,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_NET_DOWN_PATH,				ePAL_PARAM_TYPE_STR,	sizeof(HUINT8)*INET_DOWN_PATH_MAX,		<=,										N/A
	ePAL_PARAM_OTA_FTP_INFO,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_FTP_INFO,				ePAL_PARAM_TYPE_BIN,	sizeof(DI_FTP_OTA_SETTING_T),			sizeof(SwUpdate_IpEmergencyInfo_t),		pal_param_ConverterOtaFtpInfo
#if 0
	ePAL_PARAM_NET_DEVICE_ID,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_NET_DEVICE_ID,				ePAL_PARAM_TYPE_INT,	sizeof(HUINT8),							<=,										N/A
#endif
	ePAL_PARAM_NET_IP_CONFIG,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_NET_IP_CONFIG,				ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_NET_IP_CONFIG_E),		sizeof(INET_IP_CONFIG_e),				pal_param_ConverterNetIpConfig
	ePAL_PARAM_NET_IP_SETTING,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_NET_IP_SETTING,				ePAL_PARAM_TYPE_BIN,	sizeof(DI_NVRAM_NET_IP_SETTING_T),		sizeof(INET_IP_SETTING_t),				pal_param_ConverterNetIpAddr
	ePAL_PARAM_DISPLAY_FACTORY_INFO,		// eSOURCE_NVRAM,	DI_NVRAM_FIELD_DISPLAY_FACTORY_INFO,		ePAL_PARAM_TYPE_INT,	sizeof(HINT32),							<=,										N/A
	ePAL_PARAM_OAD_VERSION,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_OAD_VERSION,					ePAL_PARAM_TYPE_INT,	sizeof(HUINT8),							<=,										N/A
	ePAL_PARAM_COUNTRY_CODE,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_COUNTRY_CODE,				ePAL_PARAM_TYPE_BIN,	sizeof(HUINT8)*4,						<=,										N/A
	ePAL_PARAM_USAGE_ID,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_USAGE_ID,					ePAL_PARAM_TYPE_INT,	sizeof(HUINT8),							<=,										N/A
	ePAL_PARAM_LANGUAGE,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_LANGUAGE,					ePAL_PARAM_TYPE_BIN,	sizeof(HUINT8)*4,						<=,										N/A
	ePAL_PARAM_SCART_RGB,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_SCART_RGB,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_SCART_SETTING_VALUE),	sizeof(DxScartFormat_b),				pal_param_ConverterScartOutput
	ePAL_PARAM_ASPECT_RATIO, 				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_ASPECT_RATIO,				ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_ASPECT_RATIO_E),		sizeof(DxAspectRatio_e),				pal_param_ConverterAspectRatio
	ePAL_PARAM_HD_RESOULTION,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_HD_RESOULTION,				ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_VIDEO_RESOLUTION_E),	sizeof(DxResolutionSelect_e),			pal_param_ConverterHdResolution
	ePAL_PARAM_PANEL_DIMMING,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_PANEL_DIMMING,				ePAL_PARAM_TYPE_INT,	sizeof(PanelDimmingLevel_t),			<=,										N/A
	ePAL_PARAM_PANEL_DISPLAY,				// eSOURCE_NVRAM,	DI_NVRAM_FIELD_PANEL_DISPLAY,				ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_FRONTPANEL_DISPLAY_E),	sizeof(HBOOL),							pal_param_ConverterPanelDisplay
	ePAL_PARAM_OSD_DISPLAY,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_OSD_DISPLAY,					ePAL_PARAM_TYPE_INT,	sizeof(DI_NVRAM_OSD_DISPLAY_E),			sizeof(HBOOL),							pal_param_ConverterOsdDisplay
	ePAL_PARAM_TV_STANDARD,					// eSOURCE_NVRAM,	DI_NVRAM_FIELD_TV_STANDARD,					ePAL_PARAM_TYPE_INT,	sizeof(DxCompositeStandard_e),			<=,										N/A

	ePAL_PARAM_MICOM_STRVER,				// eSOURCE_SYS,		DI_SYS_MICOM_STRVER,						ePAL_PARAM_TYPE_STR,	sizeof(HUINT8)*palParam_MAX_MICOM_VER_LEN,<=,									N/A
	ePAL_PARAM_CHIP_ID,						// eSOURCE_SYS,		DI_SYS_CHIP_ID,								ePAL_PARAM_TYPE_INT,	sizeof(HUINT32),						<=,										N/A
	ePAL_PARAM_CHIP_REVISION,				// eSOURCE_SYS,		DI_SYS_CHIP_REVISION,						ePAL_PARAM_TYPE_INT,	sizeof(HUINT8),							<=,										N/A
	ePAL_PARAM_CHIP_CHECK_NO,				// eSOURCE_SYS,		DI_SYS_CHIP_CHECK_NO,						ePAL_PARAM_TYPE_INT,	sizeof(HUINT32),						<=,										N/A
	ePAL_PARAM_FUSING_INFO,					// eSOURCE_SYS,		DI_SYS_FUSING_INFO,							ePAL_PARAM_TYPE_BIN,	sizeof(DI_SYS_FUSING_INFO_t),			sizeof(Sys_FusingInfo_t),				pal_param_ConverterFusingInfo

	ePAL_PARAM_BOOTLOADER_VERSION,			// eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTALDR_VERSION,				ePAL_PARAM_TYPE_INT,	sizeof(HUINT16),						<=,										N/A
	ePAL_PARAM_ROOTFS_VERSION,				// eSOURCE_FILE, 	N/A,										ePAL_PARAM_TYPE_INT,	sizeof(HUINT32),						<=,										N/A
	ePAL_PARAM_RESOURCE_VERSION,			// eSOURCE_FILE,	N/A,										ePAL_PARAM_TYPE_INT,	sizeof(HUINT32),						<=,										N/A
	ePAL_PARAM_DB_VERSION,					// eSOURCE_FILE,	N/A,										ePAL_PARAM_TYPE_INT,	sizeof(HUINT32),						<=,										N/A
	ePAL_PARAM_KERNEL_VERSION,				// eSOURCE_FILE,	N/A,										ePAL_PARAM_TYPE_STR,	sizeof(HUINT8)*palParam_MAX_MICOM_VER_LEN,<=,									N/A

	ePAL_PARAM_SYSTEM_UPDATE_DATE,			// eSOURCE_DEFINE,	(HUINT32)SYSTEM_UPDATE_DATE,				ePAL_PARAM_TYPE_STR,	strlen(SYSTEM_UPDATE_DATE),				<=,										N/A
	ePAL_PARAM_SYSTEM_UPDATED_LIST, 		// eSOURCE_DEFINE,	(HUINT32)SYSTEM_UPDATE_DATE,				ePAL_PARAM_TYPE_STR,	strlen(SYSTEM_UPDATE_DATE),				<=,										N/A
	ePAL_PARAM_SYSTEM_HW_VERSION,			// eSOURCE_DEFINE,	(HUINT32)SYSTEM_HW_VERSION,					ePAL_PARAM_TYPE_STR,	strlen(SYSTEM_HW_VERSION),				<=,										N/A
	ePAL_PARAM_MAX
} PAL_PARAM_Item_e;

typedef enum
{
	ePAL_PARAM_ERR_OK = 0,
	ePAL_PARAM_ERR_FAIL = 1,
	ePAL_PARAM_ERR_CRC = 2,
	ePAL_PARAM_ERR_MAX
} PAL_PARAM_ERR_CODE_e;

typedef enum
{
	ePAL_PARAM_ACTION_All = 0,
	ePAL_PARAM_ACTION_Set,
	ePAL_PARAM_ACTION_Get
} PAL_PARAM_ACTION_e;

typedef HBOOL (* PalParam_IsActionAvailable_t)(PAL_PARAM_Item_e eItem, PAL_PARAM_ACTION_e eAction);

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


extern HERROR PAL_PARAM_Init(PalParam_IsActionAvailable_t fnAvailableCb);
extern HERROR PAL_PARAM_SetField(PAL_PARAM_Item_e eItem, void *pvData, HUINT32 ulSize);
extern HERROR PAL_PARAM_SetFieldOffset(PAL_PARAM_Item_e eItem, HUINT32 ulOffset, void *pvData, HUINT32 ulSize);
extern HERROR PAL_PARAM_GetField(PAL_PARAM_Item_e eItem, void *pvData, HUINT32 ulSize);
extern HERROR PAL_PARAM_GetFieldOffset(PAL_PARAM_Item_e eItem, HUINT32 ulOffset, void *pvData, HUINT32 ulSize);

#endif	//!__PAL_PARAM_H__

