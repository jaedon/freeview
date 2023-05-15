/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_param.c
	@brief	  Octo pal system information

	Description: Product 정보를 제공하는 PAL		\n
	Module: PAL/SYS			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <stdio.h>

#include <htype.h>
#include <irldvt_stb_params.h>
//#include <isosvc.h>

#include <di_scart.h>
#include <di_err.h>
#include <di_nvram.h>
#include <di_channel.h>
#include <di_channel_s.h>
#include <di_channel_t.h>
#include <di_channel_c.h>
#include <di_video.h>
#include <di_system.h>
#include <di_display_panel.h>
#include <di_ftp.h>
#include <di_rf.h>
#include <di_crypt.h>
#include <di_flash.h>
#include <version.h>
#if defined( CONFIG_MW_INET_SNMP )
#include <di_snmp.h>
#endif

#include <pal_param.h>
#include "_pal_param.h"

#include <pal_channel.h>


/*******************************************************************/
/************************     Definition   * ***********************/
/*******************************************************************/
#define palParam_FILE_STRING_LENGTH				80
#define	palParam_MAKE_STR_LEN					32

#define palParam_ROOTFS_VERSION_FILE_NAME		"/etc/.version"
#define palParam_RESOURCE_VERSION_FILE_NAME		"/opt/share/.version"
#define palParam_DB_VERSION_FILE_NAME			OBAMA_DIR_DATA "/.version"
#define palParam_KERNEL_VERSION_FILE_NAME		"/proc/version"

#define	palParam_MAX_VER_HEAD_LEN				6
#define	palParam_MAX_APP_VER_LEN				(palParam_MAX_VER_HEAD_LEN + 3)
#define	palParam_MAX_LOADER_VER_LEN				(palParam_MAX_VER_HEAD_LEN + 2)
#define	palParam_MAX_MICOM_VER_LEN				(palParam_MAX_VER_HEAD_LEN + 4)
#define	palParam_MAX_VER_LEN					10

#define	palParam_GET_VER_VAL(a, b, c)			{													\
													HUINT8	i=0;									\
													*a = 0;											\
													for(i=palParam_MAX_VER_HEAD_LEN; i<c; i++)		\
													{												\
														*a |= (HUINT32)b[i] << ((c - i -1)*8);		\
													}												\
												}

#define	palParam_GetCbuffer()					s_stParamContext.hParamBuf
#define	palParam_GetHash()						s_stParamContext.phHash

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef HERROR (* palParam_ConverterCb_t)(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
typedef enum
{
	eSOURCE_NVRAM,
	eSOURCE_SYS,
	eSOURCE_DEFINE,
	eSOURCE_FILE,
	eSOURCE_MAX,
} palParam_SourceType_e;

typedef struct
{
	PAL_PARAM_Item_e			ePalKey;	// Key
	palParam_SourceType_e		eSource;	// NVRAM, SYSTEM, ..
	HUINT32						eDiKey;
	HUINT32						ulType;
	HUINT32						ulDiTypeSize;	// Size of Source data
	HUINT32						ulPalTypeSize;	// Size of target data
	palParam_ConverterCb_t		pfnConverter;
} palParam_MapInfo_t;

typedef struct
{
	HxCBUF_t 						 hParamBuf;
	HxHASH_t 						*phHash;
	palParam_MapInfo_t				*pstMap;
	PalParam_IsActionAvailable_t	 fnIsActionAvailableCb;
} palParam_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC palParam_Context_t		s_stParamContext;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC HERROR pal_param_ConverterSwupFlag(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterSwupType(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterSwupChType(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterOtaFtpInfo(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterNetIpConfig(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterNetIpAddr(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterScartOutput(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterAspectRatio(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterHdResolution(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterPanelDisplay(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterOsdDisplay(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);
STATIC HERROR pal_param_ConverterFusingInfo(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget);

STATIC const palParam_MapInfo_t s_astPalParamConvTable[] =
{
	 {ePAL_PARAM_SERIAL_NO,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_SERIAL_NO,
	 											ePAL_PARAM_TYPE_BIN,
	 											14,				// HUMAX_SERIAL_NUMBER_SIZE
	 											14,				// HUMAX_SERIAL_NUMBER_SIZE
	 											NULL}
	,{ePAL_PARAM_DTCP_KEY,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_DTCP_KEY,
												ePAL_PARAM_TYPE_BIN,
												0,
												0,
												NULL}
	,{ePAL_PARAM_CI_PLUS_KEY,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_CI_PLUS_KEY,
												ePAL_PARAM_TYPE_BIN,
												0,				// GetAlignedLen(sizeof(CI_HOST_LICENSE_CONST))
												0,				// GetAlignedLen(sizeof(CI_HOST_LICENSE_CONST))
												NULL}
	,{ePAL_PARAM_MAC_ADDR,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_MAC_ADDR,
												ePAL_PARAM_TYPE_BIN,
												6,				// PAL_VALID_MAC_ADDR_LEN
												6,				// PAL_VALID_MAC_ADDR_LEN
												NULL}
	,{ePAL_PARAM_HDCP_KEY,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_HDCP_KEY,
												ePAL_PARAM_TYPE_BIN,
												0,
												0,
												NULL}
	,{ePAL_PARAM_PARING_DATA,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_PARING_DATA,
												ePAL_PARAM_TYPE_BIN,
												0,
												0,
												NULL}
	,{ePAL_PARAM_SYSTEM_ID,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_SYSTEM_ID,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT32),
												sizeof(HUINT32),
												NULL}

	,{ePAL_PARAM_LOADER_VERSION,				eSOURCE_NVRAM,	DI_NVRAM_FIELD_LOADER_VERSION,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT32),
												sizeof(HUINT32),
												NULL}
	,{ePAL_PARAM_APP_VERSION_NEXT,				eSOURCE_NVRAM,	DI_NVRAM_FIELD_APP_VERSION_NEXT,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT32),
												sizeof(HUINT32),
												NULL}				// PAL_SYS_SetSystemVersion
	,{ePAL_PARAM_APP_VERSION,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_APP_VERSION,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT32),
												sizeof(HUINT32),
												NULL}				// PAL_SYS_GetSystemVersion eVERSION_TYPE_APPL

	,{ePAL_PARAM_PRIVATE,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_PRIVATE,
												ePAL_PARAM_TYPE_BIN,
												4,					// Test
												4,					// Test
												NULL}
	,{ePAL_PARAM_CSC_KEY,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_CSC_KEY,
												ePAL_PARAM_TYPE_BIN,
												256,				// CSC_FLASH_SIZE
												256,				// CSC_FLASH_SIZE
												NULL}
	,{ePAL_PARAM_DSTB_ID,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_DSTB_ID,
												ePAL_PARAM_TYPE_BIN,
												0,
												0,
												NULL}

	,{ePAL_PARAM_IRDETO_BBCB,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_BBCB,
												ePAL_PARAM_TYPE_BIN,
												24,				// IR_BBCB_SIZE
												24,				// IR_BBCB_SIZE
												NULL}
	,{ePAL_PARAM_IRDETO_UC_SECURE_ID,			eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_UC_SECURE_ID,
												ePAL_PARAM_TYPE_BIN,
												4,				// IRUC_FLASH_SECURE_ID_SIZE
												4,				// IRUC_FLASH_SECURE_ID_SIZE
												NULL}
	,{ePAL_PARAM_IRDETO_UC_PRIVATE_DATA,		eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_UC_PRIVATE_DATA,
												ePAL_PARAM_TYPE_BIN,
												16,				// IRUC_FLASH_PRIVATEDATA_SIZE
												16,				// IRUC_FLASH_PRIVATEDATA_SIZE
												NULL}
	,{ePAL_PARAM_IRDETO_CPCB,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_CPCB,
												ePAL_PARAM_TYPE_BIN,
												(8*1024),		// IR_CPCB_SIZE
												(8*1024),		// IR_CPCB_SIZE
												NULL}
	,{ePAL_PARAM_IRDETO_CPCB_ENCRYPTED,			eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED,
												ePAL_PARAM_TYPE_BIN,
												(8*1024),		// IR_CPCB_SIZE
												(8*1024),		// IR_CPCB_SIZE
												NULL}
	,{ePAL_PARAM_IRDETO_CA_PARTITION,			eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_CA_PARTITION,
												ePAL_PARAM_TYPE_BIN,
												0x0200,			// IR_PARTITION_CA_SIZE
												0x0200,			// IR_PARTITION_CA_SIZE
												NULL}
	,{ePAL_PARAM_IRDETO_LOADER_PARTITION,		eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_LOADER_PARTITION,
												ePAL_PARAM_TYPE_BIN,
												0x0200,			// IR_PARTITION_LOADER_SIZE
												0x0200,			// IR_PARTITION_LOADER_SIZE
												NULL}
	,{ePAL_PARAM_IRDETO_IR_PARTITION,			eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_IR_PARTITION,
												ePAL_PARAM_TYPE_BIN,
												0x0200,			// IR_PARTITION_IRDETO_SIZE
												0x0200,			// IR_PARTITION_IRDETO_SIZE
												NULL}
	,{ePAL_PARAM_IRDETO_IR_BACKUP_PARTITION,	eSOURCE_NVRAM,	DI_NVRAM_FIELD_IRDETO_IR_BACKUP_PARTITION,
												ePAL_PARAM_TYPE_BIN,
												0x0200,			// IR_PARTITION_IRDETO_BACKUP_SIZE
												0x0200,			// IR_PARTITION_IRDETO_BACKUP_SIZE
												NULL}

	,{ePAL_PARAM_TUNER_ID1,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_TUNER_ID1,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT8),
												sizeof(HUINT8),
												NULL}							// HUINT8
	,{ePAL_PARAM_TUNER_ID2,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_TUNER_ID2,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT8),
												sizeof(HUINT8),
												NULL}							// PAL_SYS_SetTunerId
	,{ePAL_PARAM_TUNER_ID3,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_TUNER_ID3,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT8),
												sizeof(HUINT8),
												NULL}							//
	,{ePAL_PARAM_ANTENA_POWER1,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_ANTENA_POWER1,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT8),
												sizeof(HUINT8),
												NULL}							// HUINT8
	,{ePAL_PARAM_ANTENA_POWER2,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_ANTENA_POWER2,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT8),
												sizeof(HUINT8),
												NULL}							// PAL_SYS_SetOtaTuningInfo, PAL_SYS_SetSwUpdateTuningInfoTer, PAL_SYS_SetAntennaPower
	,{ePAL_PARAM_ANTENA_POWER3,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_ANTENA_POWER3,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT8),
												sizeof(HUINT8),
												NULL}							//
	,{ePAL_PARAM_SWUP_FLAG1,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_FLAG1,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_OTA_FLAG_E),
												sizeof(SWUP_Flag_e),
												pal_param_ConverterSwupFlag}		// Converter DI_NVRAM_OTA_FLAG_E ? SWUP_Flag_e
	,{ePAL_PARAM_SWUP_FLAG2,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_FLAG2,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_OTA_FLAG_E),
												sizeof(SWUP_Flag_e),
												pal_param_ConverterSwupFlag}		// PAL_SYS_GetOtaFlag, PAL_SYS_SetOtaFlag
	,{ePAL_PARAM_SWUP_FLAG3,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_FLAG3,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_OTA_FLAG_E),
												sizeof(SWUP_Flag_e),
												pal_param_ConverterSwupFlag}		//

	,{ePAL_PARAM_SWUP_TYPE1, 					eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_TYPE1,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_OTA_TYPE_E),
												sizeof(SWUP_DownloadType_e),
												pal_param_ConverterSwupType} 	//
	,{ePAL_PARAM_SWUP_TYPE2, 					eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_TYPE1,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_OTA_TYPE_E),
												sizeof(SWUP_DownloadType_e),
												pal_param_ConverterSwupType} 	//
	,{ePAL_PARAM_SWUP_TYPE3, 					eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_TYPE1,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_OTA_TYPE_E),
												sizeof(SWUP_DownloadType_e),
												pal_param_ConverterSwupType} 	//

	,{ePAL_PARAM_SWUP_CH_TYPE1, 				eSOURCE_NVRAM,	DI_NVRAM_FIELD_CH_TYPE1,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_OTA_CHANNEL_E),
												sizeof(DxDeliveryType_b),
												pal_param_ConverterSwupChType}				// Converter DxDeliveryType_b ? DI_NVRAM_OTA_CHANNEL_E
	,{ePAL_PARAM_SWUP_CH_TYPE2,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_CH_TYPE2,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_OTA_CHANNEL_E),
												sizeof(DxDeliveryType_b),
												pal_param_ConverterSwupChType}				// PAL_SYS_SetChannelType, PAL_SYS_SetOtaTuningInfo, PAL_SYS_SetSwUpdateTuningInfoSat,
	,{ePAL_PARAM_SWUP_CH_TYPE3,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_CH_TYPE3,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_OTA_CHANNEL_E),
												sizeof(DxDeliveryType_b),
												pal_param_ConverterSwupChType}				// PAL_SYS_SetSwUpdateTuningInfoTer, PAL_SYS_SetSwUpdateTuningInfoCab

	,{ePAL_PARAM_ES_PID1,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_ES_PID1,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT16),
												sizeof(HUINT16),
												NULL}										// HUINT16 PAL_SYS_SetOtaPid
	,{ePAL_PARAM_ES_PID2,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_ES_PID2,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT16),
												sizeof(HUINT16),
												NULL}
	,{ePAL_PARAM_ES_PID3,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_ES_PID3,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT16),
												sizeof(HUINT16),
												NULL}
	,{ePAL_PARAM_NET_PORTAL_IP,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_NET_PORTAL_IP,
												ePAL_PARAM_TYPE_BIN,
												sizeof(HUINT8)*INET_ADDR_LENGTH,
												sizeof(HUINT8)*INET_ADDR_LENGTH,
												NULL}										// PAL_SYS_SetPortalIP

	// String
	,{ePAL_PARAM_NET_DOWN_PATH,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_NET_DOWN_PATH,
												ePAL_PARAM_TYPE_STR,
												sizeof(HUINT8)*INET_DOWN_PATH_MAX,
												sizeof(HUINT8)*INET_DOWN_PATH_MAX,
												NULL}										// HUINT8 * PAL_SYS_SetDownloadPath

	// Bin	: TODO: Move SvcSwUpdate_IpEmergencyInfo_t to stb_params.h
	,{ePAL_PARAM_OTA_FTP_INFO,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTA_FTP_INFO,
												ePAL_PARAM_TYPE_BIN,
												sizeof(DI_FTP_OTA_SETTING_T),
												sizeof(SwUpdate_IpEmergencyInfo_t),
												pal_param_ConverterOtaFtpInfo}				// Converter SvcSwUpdate_IpEmergencyInfo_t to DI_FTP_OTA_SETTING_T

#if 0
	,{ePAL_PARAM_NET_DEVICE_ID,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_NET_DEVICE_ID,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT8),
												sizeof(HUINT8),
												NULL}
#endif
	,{ePAL_PARAM_NET_IP_CONFIG,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_NET_IP_CONFIG,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_NET_IP_CONFIG_E),
												sizeof(INET_IP_CONFIG_e),
												pal_param_ConverterNetIpConfig}				// Converter PAL_EEPROM_NET_IP_CONFIG to DI_NVRAM_NET_IP_CONFIG_E
	,{ePAL_PARAM_NET_IP_SETTING,				eSOURCE_NVRAM,	DI_NVRAM_FIELD_NET_IP_SETTING,
												ePAL_PARAM_TYPE_BIN,
												sizeof(DI_NVRAM_NET_IP_SETTING_T),
												sizeof(INET_IP_SETTING_t),
												pal_param_ConverterNetIpAddr}
	,{ePAL_PARAM_DISPLAY_FACTORY_INFO,			eSOURCE_NVRAM,	DI_NVRAM_FIELD_DISPLAY_FACTORY_INFO,
												ePAL_PARAM_TYPE_INT,
												sizeof(HINT32),
												sizeof(HINT32),
												NULL}										// HINT32
	,{ePAL_PARAM_OAD_VERSION,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_OAD_VERSION,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT8),
												sizeof(HUINT8),
												NULL}										// HUINT8
	,{ePAL_PARAM_COUNTRY_CODE,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_COUNTRY_CODE,
												ePAL_PARAM_TYPE_BIN,
												sizeof(HUINT8)*4,
												sizeof(HUINT8)*4,
												NULL}										// HUINT8 [4] "kor"
	,{ePAL_PARAM_USAGE_ID,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_USAGE_ID,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT8),
												sizeof(HUINT8),
												NULL}										// HUINT8
	,{ePAL_PARAM_LANGUAGE,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_LANGUAGE,
												ePAL_PARAM_TYPE_BIN,
												sizeof(HUINT8)*4,
												sizeof(HUINT8)*4,
												NULL}										// HUINT8 [4] "eng"
	,{ePAL_PARAM_SCART_RGB,						eSOURCE_NVRAM,	DI_NVRAM_FIELD_SCART_RGB,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_SCART_SETTING_VALUE),
												sizeof(DxScartFormat_b),
												pal_param_ConverterScartOutput}				// Converter DxScartFormat_b to DI_NVRAM_SCART_SETTING_VALUE
	,{ePAL_PARAM_ASPECT_RATIO, 					eSOURCE_NVRAM,	DI_NVRAM_FIELD_ASPECT_RATIO,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_ASPECT_RATIO_E),
												sizeof(DxAspectRatio_e),
												pal_param_ConverterAspectRatio}				// Converter DxAspectRatio_e to DI_NVRAM_ASPECT_RATIO_E
	,{ePAL_PARAM_HD_RESOULTION,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_HD_RESOULTION,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_VIDEO_RESOLUTION_E),
												sizeof(DxResolutionSelect_e),
												pal_param_ConverterHdResolution}			// Converter DxResolutionSelect_e to DI_NVRAM_VIDEO_RESOLUTION_E
	,{ePAL_PARAM_PANEL_DIMMING,					eSOURCE_NVRAM,	DI_NVRAM_FIELD_PANEL_DIMMING,
												ePAL_PARAM_TYPE_INT,
												sizeof(PanelDimmingLevel_t),
												sizeof(PanelDimmingLevel_t),
												NULL}										// PanelDimmingLevel_t
	,{ePAL_PARAM_PANEL_DISPLAY, 				eSOURCE_NVRAM,	DI_NVRAM_FIELD_PANEL_DISPLAY,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_FRONTPANEL_DISPLAY_E),
												sizeof(HBOOL),
												pal_param_ConverterPanelDisplay}			// DI_NVRAM_FRONTPANEL_DISPLAY_E
	,{ePAL_PARAM_OSD_DISPLAY, 					eSOURCE_NVRAM,	DI_NVRAM_FIELD_OSD_DISPLAY,
												ePAL_PARAM_TYPE_INT,
												sizeof(DI_NVRAM_OSD_DISPLAY_E),
												sizeof(HBOOL),
												pal_param_ConverterOsdDisplay}				// Converter HBOOL to DI_NVRAM_OSD_DISPLAY_E
	,{ePAL_PARAM_TV_STANDARD, 					eSOURCE_NVRAM,	DI_NVRAM_FIELD_TV_STANDARD,
												ePAL_PARAM_TYPE_INT,
												sizeof(DxCompositeStandard_e),
												sizeof(DxCompositeStandard_e),
												NULL}										// DxCompositeStandard_e

	,{ePAL_PARAM_MICOM_STRVER,					eSOURCE_SYS,	DI_SYS_MICOM_STRVER,
												ePAL_PARAM_TYPE_STR,
												sizeof(HUINT8)*palParam_MAX_MICOM_VER_LEN,
												sizeof(HUINT8)*palParam_MAX_MICOM_VER_LEN,
												NULL}

	,{ePAL_PARAM_CHIP_ID,						eSOURCE_SYS,	DI_SYS_CHIP_ID,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT32),
												sizeof(HUINT32),
												NULL}
	,{ePAL_PARAM_CHIP_REVISION,					eSOURCE_SYS,	DI_SYS_CHIP_REVISION,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT8),
												sizeof(HUINT8),
												NULL}
	,{ePAL_PARAM_CHIP_CHECK_NO,					eSOURCE_SYS,	DI_SYS_CHIP_CHECK_NO,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT32),
												sizeof(HUINT32),
												NULL}
	,{ePAL_PARAM_FUSING_INFO,					eSOURCE_SYS,	DI_SYS_FUSING_INFO,
												ePAL_PARAM_TYPE_BIN,
												sizeof(DI_SYS_FUSING_INFO_t),
												sizeof(Sys_FusingInfo_t),
												pal_param_ConverterFusingInfo}


	,{ePAL_PARAM_BOOTLOADER_VERSION,			eSOURCE_NVRAM,	DI_NVRAM_FIELD_OTALDR_VERSION,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT16),
												sizeof(HUINT16),
												NULL}

	,{ePAL_PARAM_ROOTFS_VERSION,				eSOURCE_FILE, 0,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT32),
												sizeof(HUINT32),
												NULL}

	,{ePAL_PARAM_RESOURCE_VERSION,				eSOURCE_FILE, 0,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT32),
												sizeof(HUINT32),
												NULL}

	,{ePAL_PARAM_DB_VERSION,					eSOURCE_FILE, 0,
												ePAL_PARAM_TYPE_INT,
												sizeof(HUINT32),
												sizeof(HUINT32),
												NULL}
	,{ePAL_PARAM_KERNEL_VERSION,				eSOURCE_FILE, 0,
												ePAL_PARAM_TYPE_STR,
												sizeof(HUINT8)*palParam_MAX_MICOM_VER_LEN,
												sizeof(HUINT8)*palParam_MAX_MICOM_VER_LEN,
												NULL}

	,{ePAL_PARAM_SYSTEM_UPDATE_DATE,			eSOURCE_DEFINE,	(HUINT32)SYSTEM_UPDATE_DATE,
												ePAL_PARAM_TYPE_STR,
												sizeof(HUINT8)*12, 	// "15 NOV 2013"
												sizeof(HUINT8)*12, 	// "15 NOV 2013"
												NULL}
	,{ePAL_PARAM_SYSTEM_UPDATED_LIST, 			eSOURCE_DEFINE,	(HUINT32)SYSTEM_UPDATE_DATE,
												ePAL_PARAM_TYPE_STR,
												sizeof(HUINT8)*12,	// "15 NOV 2013"
												sizeof(HUINT8)*12,	// "15 NOV 2013"
												NULL}
	,{ePAL_PARAM_SYSTEM_HW_VERSION,				eSOURCE_DEFINE,	(HUINT32)0,	//SYSTEM_HW_VERSION
												ePAL_PARAM_TYPE_STR,
												0, 0, NULL}

	// 위로 추가하세요..
	,{ePAL_PARAM_MAX,							eSOURCE_MAX,	0,
												ePAL_PARAM_TYPE_MAX,
												0, 0, NULL}
};

#define	palNameOf(name)		#name
HCHAR *pal_param_GetItemStr(PAL_PARAM_Item_e eItem)
{
	switch (eItem)
	{
	case ePAL_PARAM_SERIAL_NO:							return palNameOf(ePAL_PARAM_SERIAL_NO);
	case ePAL_PARAM_DTCP_KEY:							return palNameOf(ePAL_PARAM_DTCP_KEY);
	case ePAL_PARAM_CI_PLUS_KEY:						return palNameOf(ePAL_PARAM_CI_PLUS_KEY);
	case ePAL_PARAM_MAC_ADDR:							return palNameOf(ePAL_PARAM_MAC_ADDR);
	case ePAL_PARAM_HDCP_KEY:							return palNameOf(ePAL_PARAM_HDCP_KEY);
	case ePAL_PARAM_PARING_DATA:						return palNameOf(ePAL_PARAM_PARING_DATA);
	case ePAL_PARAM_SYSTEM_ID:							return palNameOf(ePAL_PARAM_SYSTEM_ID);
	case ePAL_PARAM_LOADER_VERSION:						return palNameOf(ePAL_PARAM_LOADER_VERSION);
	case ePAL_PARAM_APP_VERSION:						return palNameOf(ePAL_PARAM_APP_VERSION);
	case ePAL_PARAM_APP_VERSION_NEXT:					return palNameOf(ePAL_PARAM_APP_VERSION_NEXT);
	case ePAL_PARAM_PRIVATE:							return palNameOf(ePAL_PARAM_PRIVATE);

	case ePAL_PARAM_CSC_KEY:							return palNameOf(ePAL_PARAM_CSC_KEY);
	case ePAL_PARAM_DSTB_ID:							return palNameOf(ePAL_PARAM_DSTB_ID);

	case ePAL_PARAM_IRDETO_BBCB:						return palNameOf(ePAL_PARAM_IRDETO_BBCB);
	case ePAL_PARAM_IRDETO_UC_SECURE_ID:				return palNameOf(ePAL_PARAM_IRDETO_UC_SECURE_ID);
	case ePAL_PARAM_IRDETO_UC_PRIVATE_DATA:				return palNameOf(ePAL_PARAM_IRDETO_UC_PRIVATE_DATA);
	case ePAL_PARAM_IRDETO_CPCB:						return palNameOf(ePAL_PARAM_IRDETO_CPCB);
	case ePAL_PARAM_IRDETO_CPCB_ENCRYPTED:				return palNameOf(ePAL_PARAM_IRDETO_CPCB_ENCRYPTED);
	case ePAL_PARAM_IRDETO_CA_PARTITION:				return palNameOf(ePAL_PARAM_IRDETO_CA_PARTITION);
	case ePAL_PARAM_IRDETO_LOADER_PARTITION:			return palNameOf(ePAL_PARAM_IRDETO_LOADER_PARTITION);
	case ePAL_PARAM_IRDETO_IR_PARTITION:				return palNameOf(ePAL_PARAM_IRDETO_IR_PARTITION);
	case ePAL_PARAM_IRDETO_IR_BACKUP_PARTITION:			return palNameOf(ePAL_PARAM_IRDETO_IR_BACKUP_PARTITION);

	case ePAL_PARAM_TUNER_ID1:							return palNameOf(ePAL_PARAM_TUNER_ID1);
	case ePAL_PARAM_TUNER_ID2:							return palNameOf(ePAL_PARAM_TUNER_ID2);
	case ePAL_PARAM_TUNER_ID3:							return palNameOf(ePAL_PARAM_TUNER_ID3);
	case ePAL_PARAM_ANTENA_POWER1:						return palNameOf(ePAL_PARAM_ANTENA_POWER1);
	case ePAL_PARAM_ANTENA_POWER2:						return palNameOf(ePAL_PARAM_ANTENA_POWER2);
	case ePAL_PARAM_ANTENA_POWER3:						return palNameOf(ePAL_PARAM_ANTENA_POWER3);
	case ePAL_PARAM_SWUP_FLAG1:							return palNameOf(ePAL_PARAM_SWUP_FLAG1);
	case ePAL_PARAM_SWUP_FLAG2:							return palNameOf(ePAL_PARAM_SWUP_FLAG2);
	case ePAL_PARAM_SWUP_FLAG3:							return palNameOf(ePAL_PARAM_SWUP_FLAG3);
	case ePAL_PARAM_SWUP_TYPE1:							return palNameOf(ePAL_PARAM_SWUP_TYPE1);
	case ePAL_PARAM_SWUP_TYPE2:							return palNameOf(ePAL_PARAM_SWUP_TYPE2);
	case ePAL_PARAM_SWUP_TYPE3:							return palNameOf(ePAL_PARAM_SWUP_TYPE3);
	case ePAL_PARAM_SWUP_CH_TYPE1: 						return palNameOf(ePAL_PARAM_SWUP_CH_TYPE1);
	case ePAL_PARAM_SWUP_CH_TYPE2:						return palNameOf(ePAL_PARAM_SWUP_CH_TYPE2);
	case ePAL_PARAM_SWUP_CH_TYPE3:						return palNameOf(ePAL_PARAM_SWUP_CH_TYPE3);

	case ePAL_PARAM_ES_PID1:							return palNameOf(ePAL_PARAM_ES_PID1);
	case ePAL_PARAM_ES_PID2:							return palNameOf(ePAL_PARAM_ES_PID2);
	case ePAL_PARAM_ES_PID3:							return palNameOf(ePAL_PARAM_ES_PID3);
	case ePAL_PARAM_NET_PORTAL_IP:						return palNameOf(ePAL_PARAM_NET_PORTAL_IP);
	case ePAL_PARAM_NET_DOWN_PATH:						return palNameOf(ePAL_PARAM_NET_DOWN_PATH);
	case ePAL_PARAM_OTA_FTP_INFO:						return palNameOf(ePAL_PARAM_OTA_FTP_INFO);
#if 0
	case ePAL_PARAM_NET_DEVICE_ID:						return palNameOf(ePAL_PARAM_NET_DEVICE_ID);
#endif
	case ePAL_PARAM_NET_IP_CONFIG:						return palNameOf(ePAL_PARAM_NET_IP_CONFIG);
	case ePAL_PARAM_NET_IP_SETTING:						return palNameOf(ePAL_PARAM_NET_IP_SETTING);
	case ePAL_PARAM_DISPLAY_FACTORY_INFO:				return palNameOf(ePAL_PARAM_DISPLAY_FACTORY_INFO);
	case ePAL_PARAM_OAD_VERSION:						return palNameOf(ePAL_PARAM_OAD_VERSION);
	case ePAL_PARAM_COUNTRY_CODE:						return palNameOf(ePAL_PARAM_COUNTRY_CODE);
	case ePAL_PARAM_USAGE_ID:							return palNameOf(ePAL_PARAM_USAGE_ID);
	case ePAL_PARAM_LANGUAGE:							return palNameOf(ePAL_PARAM_LANGUAGE);
	case ePAL_PARAM_SCART_RGB:							return palNameOf(ePAL_PARAM_SCART_RGB);
	case ePAL_PARAM_ASPECT_RATIO: 						return palNameOf(ePAL_PARAM_ASPECT_RATIO);
	case ePAL_PARAM_HD_RESOULTION:						return palNameOf(ePAL_PARAM_HD_RESOULTION);
	case ePAL_PARAM_PANEL_DIMMING:						return palNameOf(ePAL_PARAM_PANEL_DIMMING);
	case ePAL_PARAM_PANEL_DISPLAY:						return palNameOf(ePAL_PARAM_PANEL_DISPLAY);
	case ePAL_PARAM_OSD_DISPLAY:						return palNameOf(ePAL_PARAM_OSD_DISPLAY);
	case ePAL_PARAM_TV_STANDARD:						return palNameOf(ePAL_PARAM_TV_STANDARD);

	case ePAL_PARAM_MICOM_STRVER:						return palNameOf(ePAL_PARAM_MICOM_STRVER);
	case ePAL_PARAM_CHIP_ID:							return palNameOf(ePAL_PARAM_CHIP_ID);
	case ePAL_PARAM_CHIP_REVISION:						return palNameOf(ePAL_PARAM_CHIP_REVISION);
	case ePAL_PARAM_CHIP_CHECK_NO:						return palNameOf(ePAL_PARAM_CHIP_CHECK_NO);
	case ePAL_PARAM_FUSING_INFO:						return palNameOf(ePAL_PARAM_FUSING_INFO);

	case ePAL_PARAM_BOOTLOADER_VERSION:					return palNameOf(ePAL_PARAM_BOOTLOADER_VERSION);
	case ePAL_PARAM_ROOTFS_VERSION:						return palNameOf(ePAL_PARAM_ROOTFS_VERSION);
	case ePAL_PARAM_RESOURCE_VERSION:					return palNameOf(ePAL_PARAM_RESOURCE_VERSION);
	case ePAL_PARAM_DB_VERSION:							return palNameOf(ePAL_PARAM_DB_VERSION);
	case ePAL_PARAM_KERNEL_VERSION:						return palNameOf(ePAL_PARAM_KERNEL_VERSION);

	case ePAL_PARAM_SYSTEM_UPDATE_DATE:					return palNameOf(ePAL_PARAM_SYSTEM_UPDATE_DATE);
	case ePAL_PARAM_SYSTEM_UPDATED_LIST: 				return palNameOf(ePAL_PARAM_SYSTEM_UPDATED_LIST);
	case ePAL_PARAM_SYSTEM_HW_VERSION:					return palNameOf(ePAL_PARAM_SYSTEM_HW_VERSION);

	case ePAL_PARAM_MAX:								return palNameOf(ePAL_PARAM_MAX);
	default:											break;
	}

	return "Unknown Type";
}

HCHAR *pal_param_GetDataTypeStr(HUINT32 ulDataType)
{
	switch(ulDataType)
	{
	case ePAL_PARAM_TYPE_INT:		return "Int";
	case ePAL_PARAM_TYPE_STR:		return "Str";
	case ePAL_PARAM_TYPE_BIN:		return "Bin";
	case ePAL_PARAM_TYPE_MAX:		return "N/A";
	default:						break;
	}
	return "N/A";
}

STATIC HERROR pal_param_ConverterSwupFlag(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_SWUP_FLAG1:
	case ePAL_PARAM_SWUP_FLAG2:
	case ePAL_PARAM_SWUP_FLAG3:
		if (bGet)	// DI -> PAL
		{
			SWUP_Flag_e				*peSwupFlag = (SWUP_Flag_e *)pvTarget;
			DI_NVRAM_OTA_FLAG_E		*peDiFlag = (DI_NVRAM_OTA_FLAG_E *)pvSource;

			switch (*peDiFlag)
			{
			case DI_NVRAM_OTA_FLAG_NO_OTA:		*peSwupFlag = eSWUP_FLAG_NO_INFO;				break;
			case DI_NVRAM_OTA_FLAG_DETECTED:	*peSwupFlag = eSWUP_FLAG_INFO_DETECTED;			break;
			default:							*peSwupFlag = eSWUP_FLAG_NO_INFO;				break;
			}
			hErr = ERR_OK;
		}
		else		// PAL -> DI
		{
			SWUP_Flag_e 			*peSwupFlag = (SWUP_Flag_e *)pvSource;
			DI_NVRAM_OTA_FLAG_E		*peDiFlag = (DI_NVRAM_OTA_FLAG_E *)pvTarget;

			switch (*peSwupFlag)
			{
			case eSWUP_FLAG_NO_INFO:			*peDiFlag = DI_NVRAM_OTA_FLAG_NO_OTA;			break;
			case eSWUP_FLAG_INFO_DETECTED:		*peDiFlag = DI_NVRAM_OTA_FLAG_DETECTED; 		break;
			default:							*peDiFlag = DI_NVRAM_OTA_FLAG_NO_OTA;			break;
			}
			hErr = ERR_OK;
		}
		break;
	default:
		break;
	}

	return hErr;
}

STATIC HERROR pal_param_ConverterSwupType(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_SWUP_TYPE1:
	case ePAL_PARAM_SWUP_TYPE2:
	case ePAL_PARAM_SWUP_TYPE3:
		if (bGet)	// DI -> PAL
		{
			SWUP_DownloadType_e		*peSwupType = (SWUP_DownloadType_e *)pvTarget;
			DI_NVRAM_OTA_TYPE_E		*peDiType = (DI_NVRAM_OTA_TYPE_E *)pvSource;

			hErr = ERR_FAIL;
			switch (*peDiType)
			{
			case DI_NVRAM_OTA_SSU:			*peSwupType = eSWUP_DOWNLOAD_DVBSSU;		hErr = ERR_OK;	break;
			case DI_NVRAM_OTA_DTG:			*peSwupType = eSWUP_DOWNLOAD_DTG;			hErr = ERR_OK;	break;
			case DI_NVRAM_OTA_HUMAX:		*peSwupType = eSWUP_DOWNLOAD_HUMAXTYPE;		hErr = ERR_OK;	break;
			case DI_NVRAM_OTA_NORDIG:		*peSwupType = eSWUP_DOWNLOAD_NORDIG;		hErr = ERR_OK;	break;
			case DI_NVRAM_OTA_FILE:			*peSwupType = eSWUP_DOWNLOAD_FILE;			hErr = ERR_OK;	break;
			case DI_NVRAM_OTA_SKYD:			*peSwupType = eSWUP_DOWNLOAD_SKYD;			hErr = ERR_OK;	break;
			default:						break;
			}

		}
		else		// PAL -> DI
		{
			SWUP_DownloadType_e 	*peSwupType = (SWUP_DownloadType_e *)pvSource;
			DI_NVRAM_OTA_TYPE_E		*peDiType = (DI_NVRAM_OTA_TYPE_E *)pvTarget;

			hErr = ERR_FAIL;
			switch (*peSwupType)
			{
			case eSWUP_DOWNLOAD_DVBSSU:		*peDiType = DI_NVRAM_OTA_SSU;				hErr = ERR_OK;	break;
			case eSWUP_DOWNLOAD_DTG:		*peDiType = DI_NVRAM_OTA_DTG;				hErr = ERR_OK;	break;
			case eSWUP_DOWNLOAD_HUMAXTYPE:	*peDiType = DI_NVRAM_OTA_HUMAX;				hErr = ERR_OK;	break;
			case eSWUP_DOWNLOAD_NORDIG:		*peDiType = DI_NVRAM_OTA_NORDIG;			hErr = ERR_OK;	break;
			case eSWUP_DOWNLOAD_FILE:		*peDiType = DI_NVRAM_OTA_FILE;				hErr = ERR_OK;	break;
			case eSWUP_DOWNLOAD_SKYD:		*peDiType = DI_NVRAM_OTA_SKYD;				hErr = ERR_OK;	break;
			default:						break;
			}
		}
		break;
	default:
		break;
	}

	return hErr;
}

STATIC HERROR pal_param_ConverterSwupChType(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_SWUP_CH_TYPE1:
	case ePAL_PARAM_SWUP_CH_TYPE2:
	case ePAL_PARAM_SWUP_CH_TYPE3:
		if (bGet)	// DI -> PAL
		{
			SWUP_ChannelType_e		*peDeliType = (SWUP_ChannelType_e *)pvTarget;
			DI_NVRAM_OTA_CHANNEL_E	*peDiChType = (DI_NVRAM_OTA_CHANNEL_E *)pvSource;

			switch (*peDiChType)
			{
			case DI_NVRAM_OTA_CHANNEL_SAT:		*peDeliType = eSWUP_CHANNEL_SAT;			hErr = ERR_OK;		break;
			case DI_NVRAM_OTA_CHANNEL_CAB:		*peDeliType = eSWUP_CHANNEL_CAB;			hErr = ERR_OK;		break;
			case DI_NVRAM_OTA_CHANNEL_TER:		*peDeliType = eSWUP_CHANNEL_TER;			hErr = ERR_OK;		break;
			case DI_NVRAM_OTA_CHANNEL_FLASH:	*peDeliType = eSWUP_CHANNEL_FLASH;			hErr = ERR_OK;		break;
			case DI_NVRAM_OTA_CHANNEL_FTP:		*peDeliType = eSWUP_CHANNEL_FTP;			hErr = ERR_OK;		break;
			default:							peDeliType = NULL;							hErr = ERR_FAIL;	break;
			}
		}
		else		// PAL -> DI
		{
			SWUP_ChannelType_e 		*peSwupFlag = (SWUP_ChannelType_e *)pvSource;
			DI_NVRAM_OTA_CHANNEL_E	*peDiChType = (DI_NVRAM_OTA_CHANNEL_E *)pvTarget;

			switch (*peSwupFlag)
			{
			case eSWUP_CHANNEL_SAT: 	*peDiChType = DI_NVRAM_OTA_CHANNEL_SAT;		hErr = ERR_OK;		break;
			case eSWUP_CHANNEL_CAB: 	*peDiChType = DI_NVRAM_OTA_CHANNEL_CAB;		hErr = ERR_OK;		break;
			case eSWUP_CHANNEL_TER: 	*peDiChType = DI_NVRAM_OTA_CHANNEL_TER;		hErr = ERR_OK;		break;
			case eSWUP_CHANNEL_FLASH:	*peDiChType = DI_NVRAM_OTA_CHANNEL_FLASH; 	hErr = ERR_OK;		break;
			case eSWUP_CHANNEL_FTP: 	*peDiChType = DI_NVRAM_OTA_CHANNEL_FTP;		hErr = ERR_OK;		break;
			default:					 peDiChType = NULL;							hErr = ERR_FAIL;	break;
			}
		}
		break;
	default:
		break;
	}

	return hErr;
}

STATIC HERROR pal_param_ConverterOtaFtpInfo(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_OTA_FTP_INFO:
		if (bGet)	// DI -> PAL
		{
			DI_FTP_OTA_SETTING_T		*pstDiParam = (DI_FTP_OTA_SETTING_T *)pvSource;
			SwUpdate_IpEmergencyInfo_t 	*pstPalParam = (SwUpdate_IpEmergencyInfo_t *)pvTarget;

			HxSTD_MemSet(pstPalParam, 0, sizeof(SwUpdate_IpEmergencyInfo_t));
			HxSTD_MemCopy(pstPalParam->aServerIp,	pstDiParam->aServerIp,	sizeof(HUINT8)*SWUP_ETH_ADDR_LENGTH);
			HxSTD_MemCopy(pstPalParam->aUserName,	pstDiParam->aUserName,	sizeof(HUINT8)*SWUP_USER_LENGTH);
			HxSTD_MemCopy(pstPalParam->aPasswd,		pstDiParam->aPasswd,	sizeof(HUINT8)*SWUP_PASSWD_LENGTH);
			HxSTD_MemCopy(pstPalParam->aDirPath,	pstDiParam->aDirPath,	sizeof(HUINT8)*SWUP_DIR_PATH_LENGTH);
			HxSTD_MemCopy(pstPalParam->aFileName,	pstDiParam->aFileName,	sizeof(HUINT8)*SWUP_FILE_NAME_LENGTH);
		}
		else		// PAL -> DI
		{
			DI_FTP_OTA_SETTING_T		*pstDiParam = (DI_FTP_OTA_SETTING_T *)pvTarget;
			SwUpdate_IpEmergencyInfo_t 	*pstPalParam = (SwUpdate_IpEmergencyInfo_t *)pvSource;

			HxSTD_MemSet(pstDiParam, 0, sizeof(DI_FTP_OTA_SETTING_T));
			HxSTD_MemCopy(pstDiParam->aServerIp,	pstPalParam->aServerIp,	sizeof(HUINT8)*FTP_ETH_ADDR_LENGTH);
			HxSTD_MemCopy(pstDiParam->aUserName,	pstPalParam->aUserName,	sizeof(HUINT8)*FTP_USER_LENGTH);
			HxSTD_MemCopy(pstDiParam->aPasswd,		pstPalParam->aPasswd,	sizeof(HUINT8)*FTP_PASSWD_LENGTH);
			HxSTD_MemCopy(pstDiParam->aDirPath,		pstPalParam->aDirPath,	sizeof(HUINT8)*FTP_DIR_PATH_LENGTH);
			HxSTD_MemCopy(pstDiParam->aFileName,	pstPalParam->aFileName,	sizeof(HUINT8)*FTP_FILE_NAME_LENGTH);
		}
		break;
	default:
		break;
	}

	return hErr;
}


STATIC HERROR pal_param_ConverterNetIpConfig(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_NET_IP_CONFIG:
		if (bGet)	// DI -> PAL
		{
			DI_NVRAM_NET_IP_CONFIG_E		*peDiParam = (DI_NVRAM_NET_IP_CONFIG_E *)pvSource;
			INET_IP_CONFIG_e				*pePalParam = (INET_IP_CONFIG_e *)pvTarget;

			switch (*peDiParam)
			{
			case DI_NVRAM_IP_DHCP:			*pePalParam = eINET_IP_CONFIG_DHCP;		hErr = ERR_OK;	break;
			case DI_NVRAM_IP_MANUAL:		*pePalParam = eINET_IP_CONFIG_MANUAL;	hErr = ERR_OK;	break;
			default:						break;
			}
		}
		else		// PAL -> DI
		{
			DI_NVRAM_NET_IP_CONFIG_E		*peDiParam = (DI_NVRAM_NET_IP_CONFIG_E *)pvTarget;
			INET_IP_CONFIG_e				*pePalParam = (INET_IP_CONFIG_e *)pvSource;

			switch (*pePalParam)
			{
			case eINET_IP_CONFIG_DHCP:		*peDiParam = DI_NVRAM_IP_DHCP;		hErr = ERR_OK;	break;
			case eINET_IP_CONFIG_MANUAL:	*peDiParam = DI_NVRAM_IP_MANUAL; 	hErr = ERR_OK;	break;
			default:						break;
			}
		}
		break;
	default:
		break;
	}

	return hErr;
}

STATIC HERROR pal_param_ConverterNetIpAddr(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_NET_IP_SETTING:
		if (bGet)	// DI -> PAL
		{
			DI_NVRAM_NET_IP_SETTING_T	*pstDiParam = (DI_NVRAM_NET_IP_SETTING_T *)pvSource;
			INET_IP_SETTING_t 			*pstPalParam = (INET_IP_SETTING_t *)pvTarget;

			HxSTD_MemCopy(pstPalParam->aucIp,		pstDiParam->ip,			sizeof(HUINT8)*INET_ADDR_LENGTH);
			HxSTD_MemCopy(pstPalParam->aucMask,		pstDiParam->mask,		sizeof(HUINT8)*INET_ADDR_LENGTH);
			HxSTD_MemCopy(pstPalParam->aucGateway,	pstDiParam->gateway,	sizeof(HUINT8)*INET_ADDR_LENGTH);
			HxSTD_MemCopy(pstPalParam->aucDns,		pstDiParam->dns,		sizeof(HUINT8)*INET_ADDR_LENGTH);
		}
		else		// PAL -> DI
		{
			DI_NVRAM_NET_IP_SETTING_T	*pstDiParam = (DI_NVRAM_NET_IP_SETTING_T *)pvTarget;
			INET_IP_SETTING_t 			*pstPalParam = (INET_IP_SETTING_t *)pvSource;

			HxSTD_MemCopy(pstDiParam->ip, 			pstPalParam->aucIp,			sizeof(HUINT8)*NET_ADDR_LENGTH);
			HxSTD_MemCopy(pstDiParam->mask,			pstPalParam->aucMask, 		sizeof(HUINT8)*NET_ADDR_LENGTH);
			HxSTD_MemCopy(pstDiParam->gateway,		pstPalParam->aucGateway,	sizeof(HUINT8)*NET_ADDR_LENGTH);
			HxSTD_MemCopy(pstDiParam->dns,			pstPalParam->aucDns,		sizeof(HUINT8)*NET_ADDR_LENGTH);
		}
		break;
	default:
		break;
	}

	return hErr;
}

STATIC HERROR pal_param_ConverterScartOutput(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_SCART_RGB:
		if (bGet)	// DI -> PAL
		{
			DI_NVRAM_SCART_SETTING_VALUE	*peDiParam = (DI_NVRAM_SCART_SETTING_VALUE *)pvSource;
			DxScartFormat_b 				*pePalParam = (DxScartFormat_b *)pvTarget;

			switch (*peDiParam)
			{
			case DI_NVRAM_SCART_OUTPUT_CVBS:		*pePalParam = eDxSCART_FORMAT_CVBS;		hErr = ERR_OK;	break;
			case DI_NVRAM_SCART_OUTPUT_RGB:			*pePalParam = eDxSCART_FORMAT_RGB;		hErr = ERR_OK;	break;
			case DI_NVRAM_SCART_OUTPUT_SVIDEO:		*pePalParam = eDxSCART_FORMAT_SVIDEO;	hErr = ERR_OK;	break;
			default:								break;
			}
		}
		else		// PAL -> DI
		{
			DI_NVRAM_SCART_SETTING_VALUE	*peDiParam = (DI_NVRAM_SCART_SETTING_VALUE *)pvTarget;
			DxScartFormat_b 				*pePalParam = (DxScartFormat_b *)pvSource;

			switch (*pePalParam)
			{
			case eDxSCART_FORMAT_CVBS:				*peDiParam = DI_NVRAM_SCART_OUTPUT_CVBS; 	hErr = ERR_OK;	break;
			case eDxSCART_FORMAT_RGB: 				*peDiParam = DI_NVRAM_SCART_OUTPUT_RGB;		hErr = ERR_OK;	break;
			case eDxSCART_FORMAT_SVIDEO:			*peDiParam = DI_NVRAM_SCART_OUTPUT_SVIDEO;	hErr = ERR_OK;	break;
			default:								break;
			}
		}
		break;
	default:
		break;
	}

	return hErr;
}

STATIC HERROR pal_param_ConverterAspectRatio(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_ASPECT_RATIO:
		if (bGet)	// DI -> PAL
		{
			DI_NVRAM_ASPECT_RATIO_E		*peDiParam = (DI_NVRAM_ASPECT_RATIO_E *)pvSource;
			DxAspectRatio_e 			*pePalParam = (DxAspectRatio_e *)pvTarget;

			switch (*peDiParam)
			{
			case DI_NVRAM_ASPECT_RATIO_4_3:		*pePalParam = eDxASPECTRATIO_4X3;		hErr = ERR_OK;	break;
			case DI_NVRAM_ASPECT_RATIO_16_9:	*pePalParam = eDxASPECTRATIO_16X9;		hErr = ERR_OK;	break;
			default:							*pePalParam = eDxASPECTRATIO_4X3;		hErr = ERR_OK;	break;
			}
		}
		else		// PAL -> DI
		{
			DI_NVRAM_ASPECT_RATIO_E		*peDiParam = (DI_NVRAM_ASPECT_RATIO_E *)pvTarget;
			DxAspectRatio_e 			*pePalParam = (DxAspectRatio_e *)pvSource;

			switch (*pePalParam)
			{
			case eDxASPECTRATIO_4X3:			*peDiParam = DI_NVRAM_ASPECT_RATIO_4_3; 	hErr = ERR_OK;	break;
			case eDxASPECTRATIO_16X9: 			*peDiParam = DI_NVRAM_ASPECT_RATIO_16_9;	hErr = ERR_OK;	break;
			default:							*peDiParam = DI_NVRAM_ASPECT_RATIO_4_3; 	hErr = ERR_OK;	break;
			}
		}
		break;
	default:
		break;
	}

	return hErr;
}

STATIC HERROR pal_param_ConverterHdResolution(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_HD_RESOULTION:
		if (bGet)	// DI -> PAL
		{
			DI_NVRAM_VIDEO_RESOLUTION_E		*peDiParam = (DI_NVRAM_VIDEO_RESOLUTION_E *)pvSource;
			DxResolutionStatus_b			*pePalParam = (DxResolutionStatus_b *)pvTarget;

			switch (*peDiParam)
			{
			case DI_NVRAM_RESOLUTION_720x240P:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_2880x240P:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_640x480P:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_720x480I:		*pePalParam = eDxRESOLUTION_STATUS_480I;	hErr = ERR_OK;	break;
			case DI_NVRAM_RESOLUTION_720x480P:		*pePalParam = eDxRESOLUTION_STATUS_480P;	hErr = ERR_OK;	break;
			case DI_NVRAM_RESOLUTION_1440x480P:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_2880x480I:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_2880x480P:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_720x288P:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_2880x288P:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_720x576I:		*pePalParam = eDxRESOLUTION_STATUS_576I;	hErr = ERR_OK;	break;
			case DI_NVRAM_RESOLUTION_720x576P:		*pePalParam = eDxRESOLUTION_STATUS_576P;	hErr = ERR_OK;	break;
			case DI_NVRAM_RESOLUTION_1440x576P:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_2880x576I:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_2880x576P:		 hErr = ERR_FAIL;	break;
			case DI_NVRAM_RESOLUTION_1080x720P:		*pePalParam = eDxRESOLUTION_STATUS_720P;	hErr = ERR_OK;	break;
			case DI_NVRAM_RESOLUTION_1280x720P:		*pePalParam = eDxRESOLUTION_STATUS_720P;	hErr = ERR_OK;	break;
			case DI_NVRAM_RESOLUTION_1920x1080I:	*pePalParam = eDxRESOLUTION_STATUS_1080I;	hErr = ERR_OK;	break;
			case DI_NVRAM_RESOLUTION_1920x1080P:	*pePalParam = eDxRESOLUTION_STATUS_1080P;	hErr = ERR_OK;	break;
			default:								 hErr = ERR_FAIL;	break;
			}
		}
		else		// PAL -> DI
		{
			DI_NVRAM_VIDEO_RESOLUTION_E		*peDiParam = (DI_NVRAM_VIDEO_RESOLUTION_E *)pvTarget;
			DxResolutionStatus_b			*pePalParam = (DxResolutionStatus_b *)pvSource;

			switch (*pePalParam)
			{
			case eDxRESOLUTION_STATUS_480I:		*peDiParam = DI_NVRAM_RESOLUTION_720x480I;		hErr = ERR_OK;	break;
			case eDxRESOLUTION_STATUS_480P:		*peDiParam = DI_NVRAM_RESOLUTION_720x480P;		hErr = ERR_OK;	break;
			case eDxRESOLUTION_STATUS_576I:		*peDiParam = DI_NVRAM_RESOLUTION_720x576I;		hErr = ERR_OK;	break;
			case eDxRESOLUTION_STATUS_576P:		*peDiParam = DI_NVRAM_RESOLUTION_720x576P;		hErr = ERR_OK;	break;
//			case eDxRESOLUTION_STATUS_720P: 	*peDiParam = DI_NVRAM_RESOLUTION_1080x720P;		hErr = ERR_OK;	break;
			case eDxRESOLUTION_STATUS_720P: 	*peDiParam = DI_NVRAM_RESOLUTION_1280x720P;		hErr = ERR_OK;	break;
			case eDxRESOLUTION_STATUS_1080I:	*peDiParam = DI_NVRAM_RESOLUTION_1920x1080I;	hErr = ERR_OK;	break;
			case eDxRESOLUTION_STATUS_1080P:	*peDiParam = DI_NVRAM_RESOLUTION_1920x1080P;	hErr = ERR_OK;	break;
			default:							 hErr = ERR_FAIL;	break;
			}
		}
		break;
	default:
		break;
	}

	return hErr;
}

STATIC HERROR pal_param_ConverterPanelDisplay(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_PANEL_DISPLAY:
		if (bGet)	// DI -> PAL
		{
			DI_NVRAM_FRONTPANEL_DISPLAY_E 	*peDiParam = (DI_NVRAM_FRONTPANEL_DISPLAY_E *)pvSource;
			HBOOL 							*pbPalParam = (HBOOL *)pvTarget;

			switch (*peDiParam)
			{
			case DI_NVRAM_FRONTPANEL_DISPLAY_ON: 	*pbPalParam = TRUE;		hErr = ERR_OK;	break;
			case DI_NVRAM_FRONTPANEL_DISPLAY_OFF:	*pbPalParam = FALSE;	hErr = ERR_OK;	break;
			default:								 hErr = ERR_FAIL;	break;
			}
		}
		else		// PAL -> DI
		{
			DI_NVRAM_FRONTPANEL_DISPLAY_E 	*peDiParam = (DI_NVRAM_FRONTPANEL_DISPLAY_E *)pvTarget;
			HBOOL 							*pbPalParam = (HBOOL *)pvSource;

			*peDiParam = (*pbPalParam == TRUE) ? DI_NVRAM_FRONTPANEL_DISPLAY_ON : DI_NVRAM_FRONTPANEL_DISPLAY_OFF;
			hErr = ERR_OK;
		}
		break;
	default:
		break;
	}

	return hErr;
}


STATIC HERROR pal_param_ConverterOsdDisplay(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_OSD_DISPLAY:
		if (bGet)	// DI -> PAL
		{
			DI_NVRAM_OSD_DISPLAY_E 	*peDiParam = (DI_NVRAM_OSD_DISPLAY_E *)pvSource;
			HBOOL 					*pbPalParam = (HBOOL *)pvTarget;

			switch (*peDiParam)
			{
			case DI_NVRAM_OSD_ON: 	*pbPalParam = TRUE;		hErr = ERR_OK;	break;
			case DI_NVRAM_OSD_OFF:	*pbPalParam = FALSE;	hErr = ERR_OK;	break;
			default:				 hErr = ERR_FAIL;	break;
			}
		}
		else		// PAL -> DI
		{
			DI_NVRAM_OSD_DISPLAY_E 	*peDiParam = (DI_NVRAM_OSD_DISPLAY_E *)pvTarget;
			HBOOL 					*pbPalParam = (HBOOL *)pvSource;

			*peDiParam = (*pbPalParam == TRUE) ? DI_NVRAM_OSD_ON : DI_NVRAM_OSD_OFF;
			hErr = ERR_OK;
		}
		break;
	default:
		break;
	}

	return hErr;
}


STATIC HERROR pal_param_ConverterFusingInfo(HBOOL bGet, PAL_PARAM_Item_e eItem, const void *pvSource, void *pvTarget)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pvSource)
	{
		HxLOG_Error("NULL == pvSource\n");
		return ERR_FAIL;
	}

	if (NULL == pvTarget)
	{
		HxLOG_Error("NULL == pvTarget\n");
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case ePAL_PARAM_FUSING_INFO:
		if (bGet)	// DI -> PAL
		{
			DI_SYS_FUSING_INFO_t 	*pstDiParam = (DI_SYS_FUSING_INFO_t *)pvSource;
			Sys_FusingInfo_t 		*pstPalParam = (Sys_FusingInfo_t *)pvTarget;

			pstPalParam->ucSCS = pstDiParam->ucSCS;
			pstPalParam->ucJTAG = pstDiParam->ucJTAG;
			pstPalParam->ucOTP = pstDiParam->ucOTP;
			pstPalParam->ucCWE = pstDiParam->ucCWE;
			hErr = ERR_OK;
		}
		else		// PAL -> DI
		{
			DI_SYS_FUSING_INFO_t 	*pstDiParam = (DI_SYS_FUSING_INFO_t *)pvTarget;
			Sys_FusingInfo_t 		*pstPalParam = (Sys_FusingInfo_t *)pvSource;

			pstDiParam->ucSCS = pstPalParam->ucSCS;
			pstDiParam->ucJTAG = pstPalParam->ucJTAG;
			pstDiParam->ucOTP = pstPalParam->ucOTP;
			pstDiParam->ucCWE = pstPalParam->ucCWE;
			hErr = ERR_OK;
		}
		break;
	default:
		break;
	}

	return hErr;
}

STATIC HERROR pal_param_MakeVerText (HUINT8 *pszHeadStr, HUINT32 ulVerNum, HUINT8 *pszText, HUINT32 ulVerByteNum)
{
	HUINT32		ulVerNum1, ulVerNum2, ulVerNum3, ulTemp = 0;			// VerNum1.VerNum2.VerNum3

	if (NULL == pszText)
	{
		return ERR_FAIL;
	}

	ulVerNum1 = (ulVerNum >> 16) & 0xFF;
	ulVerNum2 = (ulVerNum >> 8) & 0xFF;
	ulVerNum3 = (ulVerNum) & 0xFF;

	switch (ulVerByteNum)
	{
	case 2:
		if (pszHeadStr != NULL && pszHeadStr[0] != '\0')
		{
			HxSTD_PrintToStrN (pszText, palParam_MAKE_STR_LEN, "%s %d.%02d", pszHeadStr, ulVerNum2, ulVerNum3);
		}
		else
		{
			HxSTD_PrintToStrN (pszText, palParam_MAKE_STR_LEN, "%d.%02d", ulVerNum2, ulVerNum3);
		}

		break;
	case 3:	// For Irdeto Loader
		ulVerNum1 = (ulVerNum >> 8) & 0xFF;
		ulTemp = (ulVerNum) & 0xFF;
		ulVerNum2 =(ulTemp>>4) & 0x0F;
		ulVerNum3 = (ulTemp) & 0x0F;

		if (pszHeadStr != NULL && pszHeadStr[0] != '\0')
		{
			HxSTD_PrintToStrN (pszText, palParam_MAKE_STR_LEN, "%s %d.%d.%d", pszHeadStr, ulVerNum1, ulVerNum2, ulVerNum3);
		}
		else
		{
			HxSTD_PrintToStrN (pszText, palParam_MAKE_STR_LEN, "%d.%d.%d", ulVerNum1, ulVerNum2, ulVerNum3);
		}
		break;
	case 4:
	default:
		if (pszHeadStr != NULL && pszHeadStr[0] != '\0')
		{
			HxSTD_PrintToStrN (pszText, palParam_MAKE_STR_LEN, "%s %d.%02d.%02d", pszHeadStr, ulVerNum1, ulVerNum2, ulVerNum3);
		}
		else
		{
			HxSTD_PrintToStrN (pszText, palParam_MAKE_STR_LEN, "%d.%02d.%02d", ulVerNum1, ulVerNum2, ulVerNum3);
		}

		break;
	}


	return ERR_OK;
}

/*******************************************************************/
/*********************** EXTERN API Functions *************************/
/*******************************************************************/


STATIC HxHASH_t *pal_param_MakeHash(void)
{
	HxHASH_t 	*phHash = NULL;

	phHash = HLIB_HASH_New(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual);
	HxLOG_Assert(phHash);
	if (phHash)
	{
		HUINT32 	i, ulNum;

		ulNum = sizeof(s_astPalParamConvTable)/sizeof(palParam_MapInfo_t);

		for (i=0; i < ulNum; i++)
		{
			if (ePAL_PARAM_MAX == s_astPalParamConvTable[i].ePalKey)
				break;

			HLIB_HASH_Insert(phHash, (void *)s_astPalParamConvTable[i].ePalKey, (void *)&s_astPalParamConvTable[i]);
		}
	}

	return phHash;
}


HERROR PAL_PARAM_Init(PalParam_IsActionAvailable_t fnAvailableCb)
{
	HxSTD_MemSet(&s_stParamContext, 0, sizeof(palParam_Context_t));

	// Create CBuffer
	s_stParamContext.hParamBuf = HLIB_CBUF_New(1024);
	if (NULL == s_stParamContext.hParamBuf)
	{
		return ERR_FAIL;
	}

	// Create Hash
	s_stParamContext.phHash = pal_param_MakeHash();
	if (NULL == s_stParamContext.phHash)
	{
		return ERR_FAIL;
	}

	s_stParamContext.fnIsActionAvailableCb = fnAvailableCb;

	return ERR_OK;
}

STATIC void *pal_param_GetBuffer(HINT32 nSize)
{
	if (0 >= nSize)
	{
		return NULL;
	}

	return HLIB_CBUF_GetBuffer(palParam_GetCbuffer(), nSize);
}

#if 0
STATIC HERROR pal_param_GetSystemId(HUINT32 *pulSystemId)
{
	DI_ERR_CODE 	eErr;

	if (pulSystemId == NULL)
	{
		return ERR_FAIL;
	}

	*pulSystemId = 0x37680000;//0x809C7D00;//0x37680000;//0x35f10000;//0x38ac0000;//0x7a010000;//0x809C7D00;

	eErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_SYSTEM_ID, 0, (void *)pulSystemId, sizeof(HUINT32));

	return (eErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}
#endif

STATIC HERROR pal_param_GetSystemNextSwupVersion(HUINT32 *pulVersion)
{
	HERROR			hErr;
	DI_ERR_CODE 	eErr;
	HUINT8			aucVer[palParam_MAX_VER_LEN];

	hErr = ERR_FAIL;
	eErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_APP_VERSION_NEXT, 0, (void *)aucVer, palParam_MAX_APP_VER_LEN);
	if (eErr == DI_ERR_OK)
	{
		palParam_GET_VER_VAL(pulVersion, aucVer, palParam_MAX_APP_VER_LEN);
		hErr = ERR_OK;
	}
	else if (eErr == DI_ERR_CRC)
	{
		palParam_GET_VER_VAL(pulVersion, aucVer, palParam_MAX_APP_VER_LEN);
		hErr = ePAL_PARAM_ERR_CRC;
	}

	return hErr;
}

STATIC HERROR pal_param_SetSystemNextSwupVersion(HUINT32 ulVersion)
{
	HERROR			 hErr;
	DI_ERR_CODE		 eErr;
	HUINT32			 ulWriteVar;
	HUINT8			 aucTmpVer[palParam_MAX_VER_LEN];

	HxSTD_MemCopy(aucTmpVer, SYSTEM_APP_VER_HEAD, palParam_MAX_VER_HEAD_LEN);
	ulWriteVar = ulVersion;
	aucTmpVer[palParam_MAX_VER_HEAD_LEN] = (HUINT8)(0x000000FF & (ulWriteVar >> 16));
	aucTmpVer[palParam_MAX_VER_HEAD_LEN+1] = (HUINT8)(0x000000FF & (ulWriteVar >> 8));
	aucTmpVer[palParam_MAX_VER_HEAD_LEN+2] = (HUINT8)(0x000000FF & ulWriteVar);
	eErr = DI_NVRAM_SetField (DI_NVRAM_FIELD_APP_VERSION_NEXT, 0, (void *)aucTmpVer, palParam_MAX_APP_VER_LEN);
	if(eErr != DI_ERR_OK)
	{
		HxLOG_Error("Error, DI_NVRAM_SetField\n");
		hErr = ERR_FAIL;
	}
	else
	{
		hErr = ERR_OK;
	}

	return hErr;
}


STATIC HERROR pal_param_GetSystemLoaderVersion(HUINT32 *pulVersion)
{
	HERROR			hErr;
	DI_ERR_CODE 	eErr;
	HUINT8			aucVer[palParam_MAX_VER_LEN];

	hErr = ERR_FAIL;
	eErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_LOADER_VERSION, 0, (void *)aucVer, palParam_MAX_LOADER_VER_LEN);
	if (eErr == DI_ERR_OK)
	{
		palParam_GET_VER_VAL(pulVersion, aucVer, palParam_MAX_LOADER_VER_LEN);
		hErr = ERR_OK;
	}
	else if (eErr == DI_ERR_CRC)
	{
		palParam_GET_VER_VAL(pulVersion, aucVer, palParam_MAX_LOADER_VER_LEN);
		hErr = ePAL_PARAM_ERR_CRC;
	}

	return hErr;
}

STATIC HERROR pal_param_GetSystemBootLoaderVersion(HUINT32 *pulVersion)
{
	HERROR			hErr;
	DI_ERR_CODE 	eErr;
	HUINT32			usReadVar;

	hErr = ERR_FAIL;
	eErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_OTALDR_VERSION, 0, (void *)&usReadVar, sizeof(HUINT16));
	if (eErr == DI_ERR_OK)
	{
		*pulVersion = (HUINT32)usReadVar;
		hErr = ERR_OK;
	}

	return hErr;
}

STATIC HERROR pal_param_GetSystemRootFsVersion(HUINT32 *pulVersion)
{
#if !defined(WIN32)
	HERROR		 hErr = ERR_FAIL;
	float		 fVersion = 0.0;
	FILE		*pFile;
	HCHAR		 szString[palParam_FILE_STRING_LENGTH];

	pFile = fopen (palParam_ROOTFS_VERSION_FILE_NAME, "rt");
	if (pFile != NULL)
	{
		fgets (szString, palParam_FILE_STRING_LENGTH, pFile);
		szString[palParam_FILE_STRING_LENGTH - 1] = '\0';
		sscanf (szString, "%f", &fVersion);
		fclose (pFile);

		*pulVersion = (HUINT32)(fVersion * 100.0);
		hErr = ERR_OK;
	}

	return hErr;
#else
	return ERR_FAIL;
#endif
}

STATIC HERROR pal_param_GetSystemResourceVersion(HUINT32 *pulVersion)
{
#if !defined(WIN32)
	HERROR		 hErr = ERR_FAIL;
	float		 fVersion = 0.0;
	FILE		*pFile;
	char		 szString[palParam_FILE_STRING_LENGTH];

	pFile = fopen (palParam_RESOURCE_VERSION_FILE_NAME, "rt");
	if (pFile != NULL)
	{
		fgets (szString, palParam_FILE_STRING_LENGTH, pFile);
		szString[palParam_FILE_STRING_LENGTH - 1] = '\0';
		sscanf (szString, "%f", &fVersion);
		fclose (pFile);

		*pulVersion = (HUINT32)(fVersion * 100.0);
		hErr = ERR_OK;
	}

	return hErr;
#else
	return ERR_FAIL;
#endif
}

#if 0
STATIC HERROR pal_param_GetSystemDbVersion(HUINT32 *pulVersion)
{
#if !defined(WIN32)
	HERROR		 hErr = ERR_FAIL;
	float		 fVersion = 0.0;
	FILE		*pFile;
	char		 szString[palParam_FILE_STRING_LENGTH];

	pFile = fopen (palParam_DB_VERSION_FILE_NAME, "rt");
	if (pFile != NULL)
	{
		fgets (szString, palParam_FILE_STRING_LENGTH, pFile);
		szString[palParam_FILE_STRING_LENGTH - 1] = '\0';
		sscanf (szString, "%f", &fVersion);
		fclose (pFile);

		*pulVersion = (HUINT32)(fVersion * 100.0);
		hErr = ERR_OK;
	}

	return hErr;
#else
	return ERR_FAIL;
#endif
}
#endif

STATIC HBOOL pal_param_IsDigit (HUINT8 ucChar)
{
	if ((ucChar >= '0' && ucChar <= '9') || (ucChar == '.'))
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HERROR pal_param_GetKernelVerStr (HUINT8 *pszVersion, HUINT32 ulStrLen)
{
#if !defined(WIN32)
	HUINT32		 ulStrIdx, ulBufIdx;
	HUINT8		*pszBuffer = NULL;
	FILE		*pFile;

	if (pszVersion == NULL)					{ return ERR_FAIL; }
	if (ulStrLen < palParam_MAKE_STR_LEN)	{ return ERR_FAIL; }

	pszBuffer = (HUINT8 *)OxPAL_Malloc(ulStrLen);
	if (NULL == pszBuffer)					{ return ERR_FAIL; }

	pFile = fopen (palParam_KERNEL_VERSION_FILE_NAME, "rt");
	if (pFile != NULL)
	{
		HINT32	nReadSize;

		HxSTD_MemSet (pszBuffer, 0, palParam_MAKE_STR_LEN);
		nReadSize = fread (pszBuffer, palParam_MAKE_STR_LEN, 1, pFile);
		if (nReadSize <= 0)
		{
			HxLOG_Warning("fread (pszBuffer, palParam_MAKE_STR_LEN, 1, pFile) Error!\n");
		}
		fclose (pFile);

		ulStrIdx = 0;
		ulBufIdx = 0;

		while (ulBufIdx < palParam_MAKE_STR_LEN)
		{
			if (pal_param_IsDigit (pszBuffer[ulBufIdx]) == TRUE)
			{
				while (ulBufIdx < palParam_MAKE_STR_LEN)
				{
					if (pal_param_IsDigit (pszBuffer[ulBufIdx]) == FALSE)
					{
						pszVersion[ulStrIdx] = '\0';
						OxPAL_Free(pszBuffer);
						return ERR_OK;
					}

					pszVersion[ulStrIdx] = pszBuffer[ulBufIdx];
					ulStrIdx ++;
					ulBufIdx ++;
				}
			}

			ulBufIdx++;
		}

		if (ulStrIdx != 0)
		{
			pszVersion[ulStrIdx] = '\0';
			if (pszBuffer)
			{
				OxPAL_Free(pszBuffer);
			}
			return ERR_OK;
		}
	}
#endif

	HxSTD_PrintToStrN (pszVersion, 8, "0.0.00");
	if (pszBuffer)
	{
		OxPAL_Free(pszBuffer);
	}

	return ERR_OK;
}


STATIC HERROR pal_param_GetSystemApplVersion(HUINT32 *pulVersion)
{
	static HUINT8	 ucCheckAppVersion = 0;

	*pulVersion = (HUINT32)SYSTEM_APP_VERSION;

	if (ucCheckAppVersion == 0)
	{
		DI_ERR_CODE 	hErr;
		HUINT32			ulTmpVersion = 0;
		HUINT8			aucTmpVer[palParam_MAX_APP_VER_LEN];

		hErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_APP_VERSION, 0, (void *)&aucTmpVer, palParam_MAX_APP_VER_LEN);
		palParam_GET_VER_VAL(&ulTmpVersion, aucTmpVer, palParam_MAX_APP_VER_LEN);
		if ((hErr != DI_ERR_OK) || (ulTmpVersion !=  (HUINT32)SYSTEM_APP_VERSION))
		{
			ulTmpVersion = SYSTEM_APP_VERSION;

			HxSTD_MemCopy(aucTmpVer, SYSTEM_APP_VER_HEAD, palParam_MAX_VER_HEAD_LEN);
			aucTmpVer[palParam_MAX_VER_HEAD_LEN] = (HUINT8)(0x000000FF & (ulTmpVersion >> 16));
			aucTmpVer[palParam_MAX_VER_HEAD_LEN+1] = (HUINT8)(0x000000FF & (ulTmpVersion >> 8));
			aucTmpVer[palParam_MAX_VER_HEAD_LEN+2] = (HUINT8)(0x000000FF & ulTmpVersion);
			hErr = DI_NVRAM_SetField (DI_NVRAM_FIELD_APP_VERSION, 0, (void *)aucTmpVer, palParam_MAX_APP_VER_LEN);
			if(hErr != DI_ERR_OK)
			{
				HxLOG_Error("Error, DI_NVRAM_SetField\n");
			}
		}

		ucCheckAppVersion = 0xFF;
	}

	return ERR_OK;
}

STATIC HERROR pal_param_GetMicomVerString(HUINT8 *pszVersion, HUINT32 ulStrLen)
{
	DI_ERR_CODE		eDiErr = DI_ERR_ERROR;
	HUINT8 			aucMiComVersion[palParam_MAX_VER_LEN + 1];
	HUINT32			ulSystemVer = 0;
	HUINT8			aucVersionHeader[palParam_MAX_VER_HEAD_LEN + 2];
	HUINT8 			i;

	if (pszVersion == NULL)
	{
		HxLOG_Error("pszVersion == NULL\n");
		return ERR_FAIL;
	}

	if (palParam_MAX_MICOM_VER_LEN > ulStrLen)
	{
		HxLOG_Error("palParam_MAX_MICOM_VER_LEN > ulStrLen(%d)\n", ulStrLen);
		return ERR_FAIL;
	}

	eDiErr = DI_SYSTEM_GetSystemInfo(DI_SYS_MICOM_STRVER, aucMiComVersion, palParam_MAX_MICOM_VER_LEN);
	if(eDiErr != DI_ERR_OK)
	{
		HxLOG_Error("DI_SYSTEM_GetSystemInfo(DI_SYS_MICOM_STRVER, %d) Error!\n", palParam_MAX_MICOM_VER_LEN);
		return ERR_FAIL;
	}

	HxLOG_Print("[%s] : %s  \n", __FUNCTION__, aucMiComVersion);
	for(i = 0; i < palParam_MAX_MICOM_VER_LEN; i++)
		HxLOG_Print("0x%x ", aucMiComVersion[i]);
	HxLOG_Print("\n");

	HxSTD_MemSet(aucVersionHeader, 0, palParam_MAX_VER_HEAD_LEN + 2);

	HLIB_STD_StrUtf8NCpy(aucVersionHeader, aucMiComVersion, palParam_MAX_VER_HEAD_LEN + 1);

	palParam_GET_VER_VAL(&ulSystemVer, aucMiComVersion, palParam_MAX_MICOM_VER_LEN);
	pal_param_MakeVerText (aucVersionHeader, ulSystemVer, pszVersion, 4);

	return ERR_OK;
}

STATIC HERROR pal_param_GetSystemUpdateDate(HUINT8 *pucString)
{
	if (pucString == NULL)
		return ERR_FAIL;

	//MWC_UTIL_DvbStrcpy(pucString, (HUINT8*)SYSTEM_UPDATE_DATE);

	return ERR_OK;
}

STATIC HERROR pal_param_GetSystemUpdatedList(HUINT8 *pucString)
{
	if (pucString == NULL)
		return ERR_FAIL;

	//MWC_UTIL_DvbStrcpy(pucString, (HUINT8*)SYSTEM_UPDATE_DATE);

	return ERR_OK;
}

STATIC HERROR pal_param_GetHwRevision(HUINT8 *pucString)
{
	if (pucString == NULL)
		return ERR_FAIL;

#if defined(SYSTEM_HW_VERSION)
	MWC_UTIL_DvbStrcpy(pucString, (HUINT8*)SYSTEM_HW_VERSION);
#else
	//MWC_UTIL_DvbStrcpy(pucString, (HUINT8*)"N/A");
#endif

	return ERR_OK;
}

STATIC const palParam_MapInfo_t *pal_param_FindInfo(PAL_PARAM_Item_e eItem)
{
	return (palParam_MapInfo_t *)HLIB_HASH_Lookup(palParam_GetHash(), (const void *)eItem);
}

HERROR pal_param_GetSizeInfo(PAL_PARAM_Item_e eItem, HUINT32 *pulDiSize, HUINT32 *pulPalSize)
{
	const palParam_MapInfo_t *pstInfo = pal_param_FindInfo(eItem);

	if (NULL == pstInfo)
	{
		return ERR_FAIL;
	}

	if (pulDiSize)
		*pulDiSize = pstInfo->ulDiTypeSize;

	if (pulPalSize)
		*pulPalSize = pstInfo->ulPalTypeSize;

	return ERR_OK;
}

HERROR pal_param_GetDataType(PAL_PARAM_Item_e eItem)
{
	const palParam_MapInfo_t *pstInfo = pal_param_FindInfo(eItem);

	if (NULL == pstInfo)
	{
		return ePAL_PARAM_TYPE_MAX;
	}

	return pstInfo->ulType;
}

STATIC HINT32 pal_param_GetOutputDataSize(HBOOL bGet, const palParam_MapInfo_t *pstInfo)
{
	HINT32	nSize;

	nSize = (bGet) ? pstInfo->ulPalTypeSize : pstInfo->ulDiTypeSize;
	if (0 >= nSize)
	{
		return 0;
	}

	return nSize;
}

STATIC void *pal_param_GetConvertedData(HBOOL bGet, const palParam_MapInfo_t *pstInfo, const void *pvSourceData, void *pvTargetData)
{
	HERROR	 hRes = ERR_OK;

	if (NULL == pstInfo->pfnConverter)
	{
		return NULL;
	}

	if (NULL == pvSourceData)
	{
		return NULL;
	}

	if (NULL == pvTargetData)
	{
		return NULL;
	}

	hRes = (* pstInfo->pfnConverter)(bGet, pstInfo->ePalKey, pvSourceData, pvTargetData);
	if (ERR_OK == hRes)
	{
		return pvTargetData;
	}

	return NULL;
}

STATIC HERROR pal_param_GetDiData(palParam_SourceType_e eSource, HUINT32 ulDiKey, HUINT32 ulOffset, void *pvData, HUINT32 ulSize)
{
	DI_ERR_CODE		eErr = DI_ERR_ERROR;

	if (NULL == pvData)
		return ERR_FAIL;

	switch (eSource)
	{
	case eSOURCE_NVRAM:
		eErr  = DI_NVRAM_GetField ((DI_NVRAM_FIELD_T)ulDiKey, ulOffset, (void *)pvData, (HINT32)ulSize);
		break;
	case eSOURCE_SYS:
		eErr  = DI_SYSTEM_GetSystemInfo((DI_SYS_INFO_e)ulDiKey, (void *)pvData, ulSize);
		break;
	case eSOURCE_DEFINE:
		break;
	default:
		break;
	}

	return (eErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR pal_param_SetDiData(palParam_SourceType_e eSource, HUINT32 ulDiKey, HUINT32 ulOffset, void *pvData, HUINT32 ulSize)
{
	DI_ERR_CODE		eErr = DI_ERR_ERROR;

	if (NULL == pvData)
		return ERR_FAIL;

	switch (eSource)
	{
	case eSOURCE_NVRAM:
		eErr  = DI_NVRAM_SetField((DI_NVRAM_FIELD_T)ulDiKey, ulOffset, (void *)pvData, ulSize);
		break;
	case eSOURCE_SYS:
		eErr  = DI_SYSTEM_SetSystemInfo((DI_SYS_INFO_e)ulDiKey, (void *)pvData, ulSize);
		break;
	case eSOURCE_DEFINE:
		break;
	default:
		break;
	}

	return (eErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR	pal_param_SetFieldOffset(const palParam_MapInfo_t *pstInfo, HUINT32 ulOffset, void *pvData, HUINT32 ulSize)
{
	HERROR		 hErr = ERR_FAIL;
	void		*pvDiData, *pvSourceData, *pvTargetData;
	HINT32		 nBufSize, nSourceSize, nTargetSize;

	nSourceSize = (HINT32)ulSize;

	nBufSize = pal_param_GetOutputDataSize(FALSE, pstInfo);
	if (0 == nBufSize)
	{
		nBufSize = nSourceSize;
	}
	else if (nBufSize < nSourceSize)
	{
		HxLOG_Warning("nBufSize(%d) < nSourceSize(%d)\n", nBufSize, nSourceSize);
		nBufSize = nSourceSize;
	}

	// PAL -> DI
	pvDiData = pal_param_GetBuffer(nBufSize);
	if (NULL == pvDiData)
	{
		HxLOG_Warning("NULL == pvDiData\n");
		return ERR_FAIL;
	}
	nTargetSize = (HUINT32)nBufSize;

	pvSourceData = pvData;

	// PAL -> DI
	pvTargetData = pal_param_GetConvertedData(FALSE, (const palParam_MapInfo_t *)pstInfo, (const void *)pvSourceData, pvDiData);
	if (pvTargetData == NULL)
	{
		// No Converter needed : Same Format (Types from DLIB or stb_params.h)
		pvTargetData = pvDiData;
		nTargetSize = nSourceSize;
		if (nTargetSize > nBufSize)
		{
			HxLOG_Warning("Item(%s) : nTargetSize(%d) > nBufSize(%d)\n", pal_param_GetItemStr(pstInfo->ePalKey), nTargetSize, nBufSize);
			hErr = ERR_FAIL;
		}
		else
		{
			HxSTD_MemCopy(pvDiData, pvSourceData, nSourceSize);
		}
	}

	if (pvTargetData)
	{
		hErr = pal_param_SetDiData(pstInfo->eSource, pstInfo->eDiKey, ulOffset, pvTargetData, nTargetSize);
	}

	return hErr;
}

STATIC HERROR	pal_param_GetFieldOffset(const palParam_MapInfo_t *pstInfo, HUINT32 ulOffset, void *pvData, HUINT32 ulSize)
{
	printf("in pal_param_GetFieldOffset\n");
	HERROR		 hErr = ERR_FAIL;
	void		*pvSourceData, *pvTargetData, *pvPalData;
	HINT32		 nBufSize, nSourceSize;

	nBufSize = pal_param_GetOutputDataSize(TRUE, pstInfo);
	if (0 == nBufSize)
	{
		nBufSize = ulSize;
	}
	else if (nBufSize < ulSize)
	{
//		HxLOG_Warning("[Key:%04d] nBufSize(%d) < ulSize(%d)\n", pstInfo->ePalKey, nBufSize, ulSize);
		nBufSize = ulSize;
	}

	if (0 == nBufSize)
	{
		HxLOG_Critical("0 == nBufSize!!\n");
		return ERR_FAIL;
	}

	pvSourceData = pal_param_GetBuffer(nBufSize);
	if (NULL == pvSourceData)
	{
		HxLOG_Critical("pal_param_GetBuffer(%d) Return NULL!!\n", nBufSize);
		return ERR_FAIL;
	}
	nSourceSize = nBufSize;

	pvPalData = pvData;
	pvTargetData = NULL;

	hErr = pal_param_GetDiData(pstInfo->eSource, pstInfo->eDiKey, ulOffset, pvSourceData, nSourceSize);
	if (ERR_OK == hErr)
	{
		pvTargetData = pal_param_GetConvertedData(TRUE, (const palParam_MapInfo_t *)pstInfo, (const void *)pvSourceData, pvPalData);
		if (NULL == pvTargetData)
		{
			HINT32		 nTargetSize = nSourceSize;

			// No Converter needed : Same Format (Types from DLIB or stb_params.h)
			if (nTargetSize > ulSize)
			{
				HxLOG_Warning("nTargetSize(%d) > ulSize(%d)\n", nTargetSize, ulSize);
				hErr = ERR_FAIL;
			}
			else
			{
				HxSTD_MemCopy(pvPalData, pvSourceData, nTargetSize);
			}
		}
	}
	else
	{
		HxLOG_Critical("[Key:%04d][Size:%d] pal_param_GetDiData(source:%d, di_key:%d) Fail!! [Error:%d]\n", pstInfo->ePalKey, nSourceSize, pstInfo->eSource, pstInfo->eDiKey, hErr);
	}

	return hErr;
}

HBOOL pal_param_IsActionAvailable(PAL_PARAM_Item_e eItem, HBOOL bSet)
{
	HBOOL	bRet = TRUE;

	if (s_stParamContext.fnIsActionAvailableCb)
	{
		return (* s_stParamContext.fnIsActionAvailableCb)(eItem, bSet);
	}
	else
	{
		// Not Implemented / Tested or Not Supported Items
		switch (eItem)
		{
		case ePAL_PARAM_DTCP_KEY:
	//	case ePAL_PARAM_CI_PLUS_KEY:
		case ePAL_PARAM_CHIP_CHECK_NO:
		case ePAL_PARAM_HDCP_KEY:
	//	case ePAL_PARAM_PARING_DATA:
	//	case ePAL_PARAM_DSTB_ID:
		case ePAL_PARAM_IRDETO_CPCB:
		case ePAL_PARAM_IRDETO_CPCB_ENCRYPTED:
		case ePAL_PARAM_ROOTFS_VERSION:
		case ePAL_PARAM_RESOURCE_VERSION:
		case ePAL_PARAM_DB_VERSION:
		case ePAL_PARAM_KERNEL_VERSION:
		case ePAL_PARAM_SYSTEM_HW_VERSION:
			return FALSE;
		default:
			break;
		}

		if (bSet)
		{
			switch (eItem)
			{
			case ePAL_PARAM_SERIAL_NO:
			case ePAL_PARAM_SYSTEM_ID:
			case ePAL_PARAM_LOADER_VERSION:
			case ePAL_PARAM_APP_VERSION:
			case ePAL_PARAM_DSTB_ID:
			case ePAL_PARAM_IRDETO_BBCB:
			case ePAL_PARAM_OAD_VERSION:
			case ePAL_PARAM_MICOM_STRVER:
			case ePAL_PARAM_CHIP_ID:
			case ePAL_PARAM_CHIP_REVISION:
			case ePAL_PARAM_CHIP_CHECK_NO:
	//		case ePAL_PARAM_FUSING_INFO:
			case ePAL_PARAM_BOOTLOADER_VERSION:
			case ePAL_PARAM_ROOTFS_VERSION:
			case ePAL_PARAM_RESOURCE_VERSION:
			case ePAL_PARAM_DB_VERSION:
			case ePAL_PARAM_KERNEL_VERSION:
			case ePAL_PARAM_SYSTEM_UPDATE_DATE:
			case ePAL_PARAM_SYSTEM_UPDATED_LIST:
			case ePAL_PARAM_SYSTEM_HW_VERSION:
				bRet = FALSE;
				HxLOG_Warning("Set Action Is Not Supported for item [%s]\n", pal_param_GetItemStr(eItem));
				break;
			default:
				bRet = TRUE;
				break;
			}
		}
		else
		{
			;
		}
	}

	return bRet;
}

HERROR	PAL_PARAM_SetField(PAL_PARAM_Item_e eItem, void *pvData, HUINT32 ulSize)
{
	HERROR		hErr;

	if (pvData == NULL)
		return ERR_FAIL;

	if (pal_param_IsActionAvailable(eItem, TRUE) == FALSE)
		return ERR_FAIL;

	hErr = ERR_FAIL;
	switch (eItem)
	{
	case ePAL_PARAM_APP_VERSION_NEXT:
		{
			HUINT32 *pulVersion = (HUINT32 *)pvData;
			hErr = pal_param_SetSystemNextSwupVersion(*pulVersion);
		}
		break;

	default:
		{
			const palParam_MapInfo_t	*pstInfo = pal_param_FindInfo(eItem);

			hErr = ERR_FAIL;
			if (pstInfo)
			{
				if (pstInfo->eSource == eSOURCE_DEFINE)
				{
					HxLOG_Warning("Set Action Is Not Supported for source type eSOURCE_DEFINE [item:%s]\n", pal_param_GetItemStr(eItem));
				}
				else
				{
					hErr = pal_param_SetFieldOffset(pstInfo, 0, pvData, ulSize);
				}
			}
			else
			{
				HxLOG_Warning("Can not find Item Information! [item:%s]\n", pal_param_GetItemStr(eItem));
			}
		}
		break;
	}

	return hErr;
}

HERROR	PAL_PARAM_SetFieldOffset(PAL_PARAM_Item_e eItem, HUINT32 ulOffset, void *pvData, HUINT32 ulSize)
{
	const palParam_MapInfo_t *pstInfo;

	if (pvData == NULL)
		return ERR_FAIL;

	if (pal_param_IsActionAvailable(eItem, TRUE) == FALSE)
		return ERR_FAIL;

	pstInfo = pal_param_FindInfo(eItem);
	if (NULL == pstInfo)
	{
		HxLOG_Warning("Can not find Item Information! [item:%s]\n", pal_param_GetItemStr(eItem));
		return ERR_FAIL;
	}

	return pal_param_SetFieldOffset(pstInfo, ulOffset, pvData, ulSize);
}

HERROR	PAL_PARAM_GetField(PAL_PARAM_Item_e eItem, void *pvData, HUINT32 ulSize)
{
	HERROR						 hErr;

	if (pvData == NULL)
		return ERR_FAIL;

	if (pal_param_IsActionAvailable(eItem, FALSE) == FALSE)
		return ERR_FAIL;

	hErr = ERR_FAIL;
	switch (eItem)
	{
#if 0
	case ePAL_PARAM_SYSTEM_ID:
		hErr = pal_param_GetSystemId((HUINT32 *)pvData);
		break;
#endif
	case ePAL_PARAM_LOADER_VERSION:
		hErr = pal_param_GetSystemLoaderVersion((HUINT32 *)pvData);
		break;

	case ePAL_PARAM_APP_VERSION:
		hErr = pal_param_GetSystemApplVersion((HUINT32 *)pvData);
		break;

	case ePAL_PARAM_APP_VERSION_NEXT:
		hErr = pal_param_GetSystemNextSwupVersion((HUINT32 *)pvData);
		break;

	case ePAL_PARAM_MICOM_STRVER:
		hErr = pal_param_GetMicomVerString((HUINT8 *)pvData, ulSize);
		break;

#if 0
	case ePAL_PARAM_BOOTLOADER_VERSION:
		hErr = pal_param_GetSystemBootLoaderVersion((HUINT32 *)pvData);
		break;
#endif

	case ePAL_PARAM_ROOTFS_VERSION:
		hErr = pal_param_GetSystemRootFsVersion((HUINT32 *)pvData);
		break;

	case ePAL_PARAM_RESOURCE_VERSION:
		hErr = pal_param_GetSystemResourceVersion((HUINT32 *)pvData);
		break;

	case ePAL_PARAM_DB_VERSION:
		hErr = pal_param_GetSystemDbVersion((HUINT32 *)pvData);
		break;

	case ePAL_PARAM_KERNEL_VERSION:
		hErr = pal_param_GetKernelVerStr((HUINT8 *)pvData, ulSize);
		break;

	case ePAL_PARAM_SYSTEM_UPDATE_DATE:		// SYSTEM_UPDATE_DATE
		hErr = pal_param_GetSystemUpdateDate((HUINT8 *)pvData);
		break;

	case ePAL_PARAM_SYSTEM_UPDATED_LIST: 	// SYSTEM_UPDATE_DATE
		hErr = pal_param_GetSystemUpdatedList((HUINT8 *)pvData);
		break;

	case ePAL_PARAM_SYSTEM_HW_VERSION:		// SYSTEM_HW_VERSION
		hErr = pal_param_GetHwRevision((HUINT8 *)pvData);
		break;

	default:
		{
			const palParam_MapInfo_t	*pstInfo = pal_param_FindInfo(eItem);

			hErr = ERR_FAIL;
			if (pstInfo)
			{
//				HxLOG_Warning("Item [%d], Name [%s], Size[%d]\n", pstInfo->ePalKey, pal_param_GetItemStr(pstInfo->ePalKey), pstInfo->ulPalTypeSize);
				hErr = pal_param_GetFieldOffset(pstInfo, 0, pvData, ulSize);
			}
			else
			{
				HxLOG_Warning("Can not find Item Information! [item:%s]\n", pal_param_GetItemStr(eItem));
			}
		}
		break;
	}

	return hErr;
}

HERROR	PAL_PARAM_GetFieldOffset(PAL_PARAM_Item_e eItem, HUINT32 ulOffset, void *pvData, HUINT32 ulSize)
{
	const palParam_MapInfo_t *pstInfo;

	if (pvData == NULL)
		return ERR_FAIL;

	if (pal_param_IsActionAvailable(eItem, FALSE) == FALSE)
		return ERR_FAIL;

	pstInfo = pal_param_FindInfo(eItem);
	if (NULL == pstInfo)
	{
		printf("Can not find Item Information! [item:%s]\n", pal_param_GetItemStr(eItem));
		return ERR_FAIL;
	}

	return pal_param_GetFieldOffset(pstInfo, ulOffset, pvData, ulSize);
}


/******** EOF *********/
