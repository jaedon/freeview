/**
	@file     pal_param_cmd.c
	@brief    PAL/PARAM command tool

	Description: 		\n
	Module: PAL/Demux			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/********************************************************************
 Header Files
 ********************************************************************/
#include <octo_common.h>
#include <pal_param.h>
#include "_pal_param.h"

#include "cutest.h"
/********************************************************************
 Macro Definition
 ********************************************************************/
#define	palParam_CMD_STR_BUFF_SIZE			128

/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/

/********************************************************************
 External Variable Declaration
 ********************************************************************/

/********************************************************************
 Static Function
 ********************************************************************/
#if defined(CONFIG_DEBUG)
void pal_param_CmdDump(HUINT8 *pucBuffer, HUINT32 unSize, HUINT32 unBaseAddr, HUINT8 bFlagDetail, HCHAR *pszPrintBuf);
#endif

/********************************************************************
  Global/Static Variable Definition
*********************************************************************/
#if defined(CONFIG_DEBUG)
STATIC int pal_param_CmdPrintItem(void *pvArgList)
{
	HUINT32				i;
	HUINT8				szStr[palParam_CMD_STR_BUFF_SIZE];
	HUINT32				ulDiSize, ulPalSize;
	PAL_PARAM_Item_e	eItem;
	HCHAR				*pszDataTypeStr;

	HLIB_CMD_Printf("\n\n");
	HLIB_CMD_Printf("==================================================================================\n");
	HLIB_CMD_Printf("==================== [PAL_PARAM_Item_e List(Total:%04d Items)] ===================\n", ePAL_PARAM_MAX);
	HLIB_CMD_Printf("==================================================================================\n\n");
	HLIB_CMD_Printf("\t[Item][Type] : size(DI  :PAL ), [----------------  Name  ----------------]\n");
	for (i=0; i<ePAL_PARAM_MAX; i++)
	{
		eItem = (PAL_PARAM_Item_e)i;

		ulDiSize = ulPalSize = 0;
		(void)pal_param_GetSizeInfo(eItem, &ulDiSize, &ulPalSize);
		pszDataTypeStr = pal_param_GetDataTypeStr(pal_param_GetDataType(eItem));

		HxSTD_PrintToStrN(szStr, palParam_CMD_STR_BUFF_SIZE, "[%04d][%-4s] : size(%04d:%04d), name[%-36s]", eItem, pszDataTypeStr, ulDiSize, ulPalSize, pal_param_GetItemStr(eItem));
		HLIB_CMD_Printf("\t%s\n", szStr);
	}
	HLIB_CMD_Printf("==================================================================================\n");
	HLIB_CMD_Printf("==================================================================================\n\n\n");

	return HxCMD_OK;
}

STATIC HUINT32 pal_param_CmdGetDefaultSize(PAL_PARAM_Item_e eItem)
{
	HUINT32 ulSize = 0;

	(void)pal_param_GetSizeInfo(eItem, NULL, &ulSize);

	if (0 == ulSize)
	{
		switch (eItem)
		{
#if 0
		case ePAL_PARAM_DTCP_KEY:					ulSize = 100;		break;
		case ePAL_PARAM_CI_PLUS_KEY:				ulSize = 100;		break;
		case ePAL_PARAM_HDCP_KEY:					ulSize = 100;		break;
		case ePAL_PARAM_PARING_DATA:				ulSize = 100;		break;
		case ePAL_PARAM_DSTB_ID:					ulSize = 100;		break;
		case ePAL_PARAM_IRDETO_CPCB:				ulSize = 100;		break;
		case ePAL_PARAM_IRDETO_CPCB_ENCRYPTED:		ulSize = 100;		break;
#endif
		case ePAL_PARAM_SYSTEM_UPDATE_DATE:			ulSize = 12;		break;
		case ePAL_PARAM_SYSTEM_UPDATED_LIST:		ulSize = 12;		break;
#if 0
		case ePAL_PARAM_SYSTEM_HW_VERSION:			ulSize = 12;		break;
#endif
		default:									break;
		}
	}

	return ulSize;
}

STATIC HBOOL pal_param_CmdCheckIgnoreItem(PAL_PARAM_Item_e eItem, HBOOL bSet)
{
	HBOOL	bAvailable;

	bAvailable = pal_param_IsActionAvailable(eItem, bSet);
	if (FALSE == bAvailable)
	{
		return TRUE;
	}

	if (bSet)
	{
		switch (eItem)
		{
		case ePAL_PARAM_DTCP_KEY:
		case ePAL_PARAM_CI_PLUS_KEY:
		case ePAL_PARAM_HDCP_KEY:
		case ePAL_PARAM_PARING_DATA:
		case ePAL_PARAM_PRIVATE:
		case ePAL_PARAM_IRDETO_CPCB:
		case ePAL_PARAM_IRDETO_CPCB_ENCRYPTED:
		case ePAL_PARAM_FUSING_INFO:
			return TRUE;
		default:
			break;
		}
	}
	else	// Get
	{
		switch (eItem)
		{
		case ePAL_PARAM_CHIP_CHECK_NO:
		case ePAL_PARAM_ROOTFS_VERSION:
#if 1
		case ePAL_PARAM_DTCP_KEY:
		case ePAL_PARAM_CI_PLUS_KEY:
		case ePAL_PARAM_HDCP_KEY:
		case ePAL_PARAM_PARING_DATA:
		case ePAL_PARAM_DSTB_ID:
		case ePAL_PARAM_IRDETO_CPCB:
		case ePAL_PARAM_IRDETO_CPCB_ENCRYPTED:
		case ePAL_PARAM_SYSTEM_HW_VERSION:
#endif
			return TRUE;
		default:
			break;
		}
	}

	return FALSE;
}

STATIC HINT32 pal_param_CmdParseValueFromArg(PAL_PARAM_Item_e eItem, HCHAR *pszArg, void *pvValue, HUINT32 *pulSize, HCHAR *pszPrintBuf)
{
	HINT32	 nRes, nResult;
	HUINT32	 ulDataType;

	if (NULL == pvValue)		return HxCMD_ERR;
	if (NULL == pulSize)		return HxCMD_ERR;

	// Get Data Type
	ulDataType = pal_param_GetDataType(eItem);

	// Set Default Value
	nResult = HxCMD_ERR;
	*pulSize = 0;
	switch (ulDataType)
	{
	case ePAL_PARAM_TYPE_INT:
		{
			HINT32		nTemp;
			HUINT32		*pulValue;

			nRes = sscanf(pszArg, "%d", &nTemp);
			if (nRes == 0)
			{
				HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail to Get 2nd Argument! Invalid Format [%s]\n", pszArg);
				nResult = HxCMD_ERR;
			}
			else
			{
				pulValue = (HUINT32 *)pvValue;
				*pulValue = (HUINT32)nTemp;
				*pulSize = sizeof(HUINT32);
				nResult = HxCMD_OK;
			}
		}
		break;
	case ePAL_PARAM_TYPE_STR:
		{
			HCHAR		 szValue[128];
			HCHAR		*pszValue = NULL;
			HUINT32		 ulSize;

			nRes = sscanf(pszArg, "%s", szValue);
			if (nRes == 0)
			{
				HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail to Get 2nd Argument! Invalid Format [%s]\n", pszArg);
				nResult = HxCMD_ERR;
			}
			else
			{
				szValue[127] = '\0';
				ulSize = HxSTD_StrLen(szValue);
				pszValue = (HCHAR *)pvValue;
				HxSTD_StrNCpy(pszValue, szValue, ulSize);
				*pulSize = ulSize;
				nResult = HxCMD_OK;
			}
		}
		break;

	case ePAL_PARAM_TYPE_BIN:
		HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"PARAM Set Fail Binary is Not Supported!\n");
		nResult = HxCMD_ERR;
		break;

	default:
		HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"PARAM Set Fail Not Supported Type [%d]!!!\n", ulDataType);
		nResult = HxCMD_ERR;
		break;
	}

	return nResult;
}

typedef struct
{
	PAL_PARAM_Item_e		 eItem;
	HUINT32					 ulSize;
	void					*pvData;
} palParam_CmdSetData_t;

// ePAL_PARAM_MAC_ADDR
STATIC HUINT8							s_aucMacAddr[6] = { 0x00, 0x01, 0x02, 0x03, 0x00, 0x64 };
// ePAL_PARAM_APP_VERSION_NEXT
STATIC HUINT32							s_ulAppVerNext = 0x01230088;
// ePAL_PARAM_CSC_KEY
// ePAL_PARAM_IRDETO_BBCB
// ePAL_PARAM_IRDETO_UC_SECURE_ID
// ePAL_PARAM_IRDETO_UC_PRIVATE_DATA
// ePAL_PARAM_IRDETO_CA_PARTITION
// ePAL_PARAM_IRDETO_LOADER_PARTITION
// ePAL_PARAM_IRDETO_IR_PARTITION
// ePAL_PARAM_IRDETO_IR_BACKUP_PARTITION
STATIC HUINT8							s_aucTmp[0x200] = { 0x00, 0xBA, 0xB0, 0xDA, 0xBA, 0xB0, 0xDA, };
// ePAL_PARAM_TUNER_ID1, ePAL_PARAM_TUNER_ID2, ePAL_PARAM_TUNER_ID3
STATIC HUINT8							s_ucTunerId1 = 0x01, s_ucTunerId2 = 0x02, s_ucTunerId3 = 0x03;
// ePAL_PARAM_ANTENA_POWER1, ePAL_PARAM_ANTENA_POWER2, ePAL_PARAM_ANTENA_POWER3
STATIC HUINT8							s_ucAntPower1 = 0x01, s_ucAntPower2 = 0x00 , s_ucAntPower3 = 0x01;
// ePAL_PARAM_SWUP_FLAG1, ePAL_PARAM_SWUP_FLAG2, ePAL_PARAM_SWUP_FLAG3
STATIC SWUP_Flag_e						s_eSwupFlag1 = eSWUP_FLAG_INFO_DETECTED, s_eSwupFlag2 = eSWUP_FLAG_NO_INFO, s_eSwupFlag3 = eSWUP_FLAG_INFO_DETECTED;
// ePAL_PARAM_SWUP_TYPE1, ePAL_PARAM_SWUP_TYPE2, ePAL_PARAM_SWUP_TYPE3
STATIC SWUP_DownloadType_e				s_eDownType1 = eSWUP_DOWNLOAD_DVBSSU, s_eDownType2 = eSWUP_DOWNLOAD_FILE, s_eDownType3 = eSWUP_DOWNLOAD_HUMAXTYPE;
// ePAL_PARAM_SWUP_CH_TYPE1, ePAL_PARAM_SWUP_CH_TYPE2, ePAL_PARAM_SWUP_CH_TYPE3
STATIC DxDeliveryType_b					s_eDeliType1 = eDxDELIVERY_TYPE_SAT, s_eDeliType2 = eDxDELIVERY_TYPE_TER, s_eDeliType3 = eDxDELIVERY_TYPE_CAB;
// ePAL_PARAM_ES_PID1, ePAL_PARAM_ES_PID2, ePAL_PARAM_ES_PID3
STATIC HUINT16							s_usEsPid1 = 0x111, s_usEsPid2 = 0x112, s_usEsPid3 = 0x113;
// ePAL_PARAM_NET_PORTAL_IP
STATIC HUINT8							s_aucPortalIp[INET_ADDR_LENGTH] = { 192, 168, 0, 1 };
// ePAL_PARAM_NET_DOWN_PATH
STATIC HUINT8							s_aucIpDownPath[INET_DOWN_PATH_MAX] = { 'h', 't', 't', 'p', ':', '/', '/', '\0' };
// ePAL_PARAM_OTA_FTP_INFO
STATIC SwUpdate_IpEmergencyInfo_t		s_stEmergencyInfo = { { 192, 168, 0, 10 },  };
// ePAL_PARAM_NET_IP_CONFIG
STATIC INET_IP_CONFIG_e					s_eIpConfig = eINET_IP_CONFIG_MANUAL;
// ePAL_PARAM_NET_IP_SETTING
STATIC INET_IP_SETTING_t				s_stIpSetting = { { 192, 168, 0, 11 },  };
// ePAL_PARAM_DISPLAY_FACTORY_INFO
STATIC HBOOL							s_bDisplayFactoryInfo = TRUE;
// ePAL_PARAM_LAUNCH_APP_MODE
STATIC HUINT32							s_nLaunchAppMode= 0;
// ePAL_PARAM_OAD_VERSION
STATIC HUINT8							s_ucOadVer = 31;
// ePAL_PARAM_COUNTRY_CODE
STATIC HUINT8							s_aucCountryCode[4] = { 'k', 'o', 'r', '\0' };
// ePAL_PARAM_USAGE_ID
STATIC HUINT8							s_ucUsageId = 1;
// ePAL_PARAM_LANGUAGE
STATIC HUINT8 							s_aucLanguage[4] = { 'e', 'n', 'g', '\0' };
// ePAL_PARAM_SCART_RGB
STATIC DxScartFormat_b					s_bScartFormat = eDxSCART_FORMAT_RGB;
// ePAL_PARAM_ASPECT_RATIO
STATIC DxAspectRatio_e					s_eAspectRatio = eDxASPECTRATIO_16X9;
// ePAL_PARAM_HD_RESOULTION
STATIC DxResolutionSelect_e				s_eResolutionSelect = eDxRESOLUTION_SELECT_1080P;
// ePAL_PARAM_PANEL_DIMMING
STATIC PanelDimmingLevel_t				s_eDimmingLevel = ePANEL_DIM_Level100;
// ePAL_PARAM_PANEL_DISPLAY
STATIC HBOOL							s_bPanelDisplay = TRUE;
// ePAL_PARAM_OSD_DISPLAY
STATIC HBOOL							s_bOsdDisplay = FALSE;
// ePAL_PARAM_TV_STANDARD
STATIC DxCompositeStandard_e			s_eCompositeStandard = eDxCOMPOSITE_STANDARD_PAL_M;


STATIC palParam_CmdSetData_t	s_astSetData[] =
{
//	{ ePAL_PARAM_DTCP_KEY,						?,										NULL },							// ePAL_PARAM_TYPE_BIN,
//	{ ePAL_PARAM_CI_PLUS_KEY,					?,										NULL },							// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_MAC_ADDR,						sizeof(HUINT8)*6,						(void *)s_aucMacAddr },			// ePAL_PARAM_TYPE_BIN,
//	{ ePAL_PARAM_HDCP_KEY,						?,										NULL }, 						// ePAL_PARAM_TYPE_BIN,
//	{ ePAL_PARAM_PARING_DATA,					?,										NULL }, 						// ePAL_PARAM_TYPE_BIN,
//	{ ePAL_PARAM_SYSTEM_ID,						sizeof(HUINT32),						NULL }, 						// ePAL_PARAM_TYPE_INT,
//	{ ePAL_PARAM_LOADER_VERSION,				sizeof(HUINT32),						NULL }, 						// ePAL_PARAM_TYPE_INT,
//	{ ePAL_PARAM_APP_VERSION,					sizeof(HUINT32),						NULL }, 						// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_APP_VERSION_NEXT,				sizeof(HUINT32),						(void *)&s_ulAppVerNext}, 		// ePAL_PARAM_TYPE_INT,
//	{ ePAL_PARAM_PRIVATE,						?,										NULL }, 						// ePAL_PARAM_TYPE_BIN,

	{ ePAL_PARAM_CSC_KEY,						256,									(void *)s_aucTmp }, 			// ePAL_PARAM_TYPE_BIN,
//	{ ePAL_PARAM_DSTB_ID,						?,										NULL }, 						// ePAL_PARAM_TYPE_BIN,

	{ ePAL_PARAM_IRDETO_BBCB,					24,										(void *)s_aucTmp }, 			// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_IRDETO_UC_SECURE_ID,			4,										(void *)s_aucTmp }, 			// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_IRDETO_UC_PRIVATE_DATA,		16,										(void *)s_aucTmp }, 			// ePAL_PARAM_TYPE_BIN,
//	{ ePAL_PARAM_IRDETO_CPCB,					IR_CPCB_SIZE,							NULL }, 						// ePAL_PARAM_TYPE_BIN,
//	{ ePAL_PARAM_IRDETO_CPCB_ENCRYPTED,			IR_CPCB_SIZE,							NULL }, 						// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_IRDETO_CA_PARTITION,			0x200, 									(void *)s_aucTmp }, 			// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_IRDETO_LOADER_PARTITION,		0x200, 									(void *)s_aucTmp }, 			// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_IRDETO_IR_PARTITION,			0x200, 									(void *)s_aucTmp }, 			// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_IRDETO_IR_BACKUP_PARTITION,	0x200, 									(void *)s_aucTmp }, 			// ePAL_PARAM_TYPE_BIN,

	{ ePAL_PARAM_TUNER_ID1,						sizeof(HUINT8),							(void *)&s_ucTunerId1 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_TUNER_ID2,						sizeof(HUINT8),							(void *)&s_ucTunerId2 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_TUNER_ID3,						sizeof(HUINT8),							(void *)&s_ucTunerId3 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_ANTENA_POWER1,					sizeof(HUINT8),							(void *)&s_ucAntPower1 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_ANTENA_POWER2,					sizeof(HUINT8),							(void *)&s_ucAntPower2 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_ANTENA_POWER3,					sizeof(HUINT8),							(void *)&s_ucAntPower3 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_SWUP_FLAG1,					sizeof(SWUP_Flag_e),					(void *)&s_eSwupFlag1 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_SWUP_FLAG2,					sizeof(SWUP_Flag_e),					(void *)&s_eSwupFlag2 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_SWUP_FLAG3,					sizeof(SWUP_Flag_e),					(void *)&s_eSwupFlag3 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_SWUP_TYPE1,					sizeof(SWUP_DownloadType_e),			(void *)&s_eDownType1 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_SWUP_TYPE2,					sizeof(SWUP_DownloadType_e),			(void *)&s_eDownType2 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_SWUP_TYPE3,					sizeof(SWUP_DownloadType_e),			(void *)&s_eDownType3 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_SWUP_CH_TYPE1, 				sizeof(DxDeliveryType_b),				(void *)&s_eDeliType1 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_SWUP_CH_TYPE2,					sizeof(DxDeliveryType_b),				(void *)&s_eDeliType2 }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_SWUP_CH_TYPE3,					sizeof(DxDeliveryType_b),				(void *)&s_eDeliType3 }, 		// ePAL_PARAM_TYPE_INT,

	{ ePAL_PARAM_ES_PID1,						sizeof(HUINT16),						(void *)&s_usEsPid1 }, 			// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_ES_PID2,						sizeof(HUINT16),						(void *)&s_usEsPid2 }, 			// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_ES_PID3,						sizeof(HUINT16),						(void *)&s_usEsPid3 }, 			// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_NET_PORTAL_IP,					sizeof(HUINT8)*INET_ADDR_LENGTH,		(void *)s_aucPortalIp }, 		// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_NET_DOWN_PATH,					sizeof(HUINT8)*INET_DOWN_PATH_MAX,		(void *)s_aucIpDownPath }, 		// ePAL_PARAM_TYPE_STR,
	{ ePAL_PARAM_OTA_FTP_INFO,					sizeof(SwUpdate_IpEmergencyInfo_t),		(void *)&s_stEmergencyInfo }, 	// ePAL_PARAM_TYPE_BIN,
#if 0
	{ ePAL_PARAM_NET_DEVICE_ID,					sizeof(HUINT8),							NULL }, 						// ePAL_PARAM_TYPE_INT,
#endif
	{ ePAL_PARAM_NET_IP_CONFIG,					sizeof(INET_IP_CONFIG_e),				(void *)&s_eIpConfig }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_NET_IP_SETTING,				sizeof(INET_IP_SETTING_t),				(void *)&s_stIpSetting }, 		// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_DISPLAY_FACTORY_INFO,			sizeof(HINT32),							(void *)&s_bDisplayFactoryInfo }, // ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_LAUNCH_APP_MODE,				sizeof(HINT32),							(void *)&s_nLaunchAppMode}, // ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_OAD_VERSION,					sizeof(HUINT8),							(void *)&s_ucOadVer }, 			// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_COUNTRY_CODE,					sizeof(HUINT8)*4,						(void *)s_aucCountryCode }, 	// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_USAGE_ID,						sizeof(HUINT8),							(void *)&s_ucUsageId }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_LANGUAGE,						sizeof(HUINT8)*4,						(void *)s_aucLanguage }, 		// ePAL_PARAM_TYPE_BIN,
	{ ePAL_PARAM_SCART_RGB,						sizeof(DxScartFormat_b),				(void *)&s_bScartFormat }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_ASPECT_RATIO, 					sizeof(DxAspectRatio_e),				(void *)&s_eAspectRatio }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_HD_RESOULTION,					sizeof(DxResolutionSelect_e),			(void *)&s_eResolutionSelect }, // ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_PANEL_DIMMING,					sizeof(PanelDimmingLevel_t),			(void *)&s_eDimmingLevel }, 	// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_PANEL_DISPLAY,					sizeof(HBOOL),							(void *)&s_bPanelDisplay }, 	// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_OSD_DISPLAY,					sizeof(HBOOL),							(void *)&s_bOsdDisplay }, 		// ePAL_PARAM_TYPE_INT,
	{ ePAL_PARAM_TV_STANDARD,					sizeof(DxCompositeStandard_e),			(void *)&s_eCompositeStandard }, // ePAL_PARAM_TYPE_INT,

//	{ ePAL_PARAM_FUSING_INFO,					sizeof(Sys_FusingInfo_t),				NULL }, 						// ePAL_PARAM_TYPE_BIN,

//	{ ePAL_PARAM_BOOTLOADER_VERSION,			sizeof(HUINT16),						NULL }, 						// ePAL_PARAM_TYPE_INT,
//	{ ePAL_PARAM_ROOTFS_VERSION,				sizeof(HUINT32),						NULL }, 						// ePAL_PARAM_TYPE_INT,
//	{ ePAL_PARAM_RESOURCE_VERSION,				sizeof(HUINT32),						NULL }, 						// ePAL_PARAM_TYPE_INT,
//	{ ePAL_PARAM_DB_VERSION,					sizeof(HUINT32),						NULL }, 						// ePAL_PARAM_TYPE_INT,
//	{ ePAL_PARAM_KERNEL_VERSION,				sizeof(HUINT8)*palParam_MAX_MICOM_VER_LEN,NULL }, 						// ePAL_PARAM_TYPE_STR,

//	{ ePAL_PARAM_SYSTEM_UPDATE_DATE,			strlen(SYSTEM_UPDATE_DATE),				NULL }, 						// ePAL_PARAM_TYPE_STR,
//	{ ePAL_PARAM_SYSTEM_UPDATED_LIST, 			strlen(SYSTEM_UPDATE_DATE),				NULL }, 						// ePAL_PARAM_TYPE_STR,
//	{ ePAL_PARAM_SYSTEM_HW_VERSION,				strlen(SYSTEM_HW_VERSION),				NULL }, 						// ePAL_PARAM_TYPE_STR,
};


STATIC palParam_CmdSetData_t* pal_param_CmdGetHardcodingData(PAL_PARAM_Item_e eItem)
{
	HUINT32 i, ulNum;

	ulNum = sizeof(s_astSetData) / sizeof(palParam_CmdSetData_t);

	for (i=0; i<ulNum; i++)
	{
		if (s_astSetData[i].eItem == eItem)
			return &s_astSetData[i];
	}

	return NULL;
}


STATIC HINT32 pal_param_CmdGetValueFromHardcode(PAL_PARAM_Item_e eItem, void *pvValue, HUINT32 *pulSize)
{
	HINT32					 nResult;
	HUINT32					 ulDataType;
	palParam_CmdSetData_t	*pstData;

	if (NULL == pvValue)		return HxCMD_ERR;
	if (NULL == pulSize)		return HxCMD_ERR;

	pstData = pal_param_CmdGetHardcodingData(eItem);
	if (NULL == pstData)
		return HxCMD_ERR;

	*pulSize = pstData->ulSize;

	// Get Data Type
	ulDataType = pal_param_GetDataType(eItem);

	switch (ulDataType)
	{
	case ePAL_PARAM_TYPE_INT:
		{
			nResult = HxCMD_OK;
			if (1 == pstData->ulSize)
			{
				HUINT8 	*pucValue = (HUINT8 *)pvValue;

				*pucValue = *((HUINT8 *)pstData->pvData);

//				HLIB_CMD_Printf("[%04d:INT] Value [%d], Size [%d]\n", eItem, *pucValue, pstData->ulSize);
			}
			else if (2 == pstData->ulSize)
			{
				HUINT16 *pusValue = (HUINT16 *)pvValue;

				*pusValue = *((HUINT16 *)pstData->pvData);

//				HLIB_CMD_Printf("[%04d:INT] Value [%d], Size [%d]\n", eItem, *pusValue, pstData->ulSize);
			}
			else if (4 == pstData->ulSize)
			{
				HUINT32 *pulValue = (HUINT32 *)pvValue;

				*pulValue = *((HUINT32 *)pstData->pvData);

//				HLIB_CMD_Printf("[%04d:INT] Value [%d], Size [%d]\n", eItem, *pulValue, pstData->ulSize);
			}
			else
			{
				nResult = HxCMD_ERR;
			}
		}
		break;

	case ePAL_PARAM_TYPE_STR:
		{
			HCHAR		*pszValue = (HCHAR *)pvValue;

			HxSTD_StrNCpy(pszValue, (char *)pstData->pvData, pstData->ulSize);

//			HLIB_CMD_Printf("[%04d:STR] Value [%s], Size [%d]\n", eItem, pszValue, pstData->ulSize);
			nResult = HxCMD_OK;
		}
		break;

	case ePAL_PARAM_TYPE_BIN:
		{
			HUINT8		*pucValue = (HUINT8 *)pvValue;

			HxSTD_MemCopy(pucValue, pstData->pvData, pstData->ulSize);

//			HLIB_CMD_Printf("[%04d:STR] Value [%c][%c][%c][%c]..., Size [%d]\n", eItem, pucValue[0], pucValue[1], pucValue[2], pucValue[3], pstData->ulSize);
			nResult = HxCMD_OK;
		}
		break;

	default:
		HLIB_CMD_Printf("PARAM Set Fail Not Supported Type [%d]!!!\n", ulDataType);
		nResult = HxCMD_ERR;
		break;
	}

	return nResult;
}

STATIC int pal_param_CmdSetValue(PAL_PARAM_Item_e eItem, void *pvValue, HUINT32 ulSize, HCHAR *pszPrintBuf)
{
	HINT32	 nResult;
	HERROR	 hErr;
	HUINT32	 ulDataType;

	if (NULL == pvValue)	return HxCMD_ERR;
	if (0 == ulSize)		return HxCMD_ERR;

	// Get Data Type
	ulDataType = pal_param_GetDataType(eItem);

	nResult = HxCMD_ERR;

	switch (ulDataType)
	{
	case ePAL_PARAM_TYPE_INT:
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)pvValue, ulSize);
			if (ERR_OK != hErr)
			{
				HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail! PAL_PARAM_SetField(%d, %p, %d) error [%d]", eItem, pvValue, ulSize, hErr);
				nResult = HxCMD_ERR;
			}
			else
			{
				void		*pvWrittenValue = NULL;
				HUINT8		 ucWrittenValue = 0;
				HUINT16		 usWrittenValue = 0;
				HUINT32		 ulWrittenValue = 0;

				switch (ulSize)
				{
				case 1: pvWrittenValue = &ucWrittenValue; break;
				case 2: pvWrittenValue = &usWrittenValue; break;
				case 4: pvWrittenValue = &ulWrittenValue; break;
				default:									break;
				}

				if (NULL == pvWrittenValue)
				{
					HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Invalid Int Size [%d]!", ulSize);
					nResult = HxCMD_ERR;
				}
				else
				{
					HLIB_STD_TaskSleep(100);
					hErr = PAL_PARAM_GetField(eItem, (void *)pvWrittenValue, ulSize);
					if (ERR_OK == hErr)
					{
						if (HxSTD_MemCmp(pvValue, pvWrittenValue, ulSize) == 0)
						{
							switch (ulSize)
							{
							case 1: HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Set INT Value (size:%04d)[value:(0x%02x),%d]!", ulSize, *((HUINT8 *)pvWrittenValue), *((HUINT8 *)pvWrittenValue));
									break;
							case 2: HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Set INT Value (size:%04d)[value:(0x%04x),%d]!", ulSize, *((HUINT16 *)pvWrittenValue), *((HUINT16 *)pvWrittenValue));
									break;
							case 4: HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Set INT Value (size:%04d)[value:(0x%08x),%d]!", ulSize, *((HUINT32 *)pvWrittenValue), *((HUINT32 *)pvWrittenValue));
									break;
							default:									break;
							}
							nResult = HxCMD_OK;
						}
						else
						{
							switch (ulSize)
							{
							case 1: HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail! [(0x%02x)%d] != [(0x%02x)%d]", *((HUINT8 *)pvValue), *((HUINT8 *)pvValue), *((HUINT8 *)pvWrittenValue), *((HUINT8 *)pvWrittenValue));
									break;
							case 2: HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail! [(0x%04x)%d] != [(0x%04x)%d]", *((HUINT16 *)pvValue), *((HUINT16 *)pvValue), *((HUINT16 *)pvWrittenValue), *((HUINT16 *)pvWrittenValue));
									break;
							case 4: HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail! [(0x%08x)%d] != [(0x%08x)%d]", *((HUINT32 *)pvValue), *((HUINT32 *)pvValue), *((HUINT32 *)pvWrittenValue), *((HUINT32 *)pvWrittenValue));
									break;
							default:									break;
							}

							nResult = HxCMD_ERR;
						}
					}
				}
			}
		}
		break;

	case ePAL_PARAM_TYPE_STR:
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)pvValue, ulSize);
			if (ERR_OK != hErr)
			{
				HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail! PAL_PARAM_SetField(%d, %p, %d) error [%d]", eItem, pvValue, ulSize, hErr);
				nResult = HxCMD_ERR;
			}
			else
			{
				HCHAR		*pszWrittenValue = NULL;

				pszWrittenValue = OxPAL_Malloc(ulSize);
				if (pszWrittenValue)
				{
					HLIB_STD_TaskSleep(100);
					pszWrittenValue[0] = '\0';
					hErr = PAL_PARAM_GetField(eItem, (void *)pszWrittenValue, ulSize);
					if (ERR_OK == hErr)
					{
						HCHAR		*pszValue = (HCHAR *)pvValue;

						if (HxSTD_StrNCmp(pszValue, pszWrittenValue, ulSize) == 0)
						{
							HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Set STR Value (size:%04d)[value:\"%s\"]", ulSize, pszWrittenValue);
							nResult = HxCMD_OK;
						}
						else
						{
							HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail! [%s] != [%s]", pszValue, pszWrittenValue);
							nResult = HxCMD_ERR;
						}
					}
					OxPAL_Free(pszWrittenValue);
				}
				else
				{
					HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail to Allocate String Buffer!");
				}
			}
		}
		break;

	case ePAL_PARAM_TYPE_BIN:
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)pvValue, ulSize);
			if (ERR_OK != hErr)
			{
				HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail! PAL_PARAM_SetField(%d, %p, %d) error [%d]", eItem, pvValue, ulSize, hErr);
				nResult = HxCMD_ERR;
			}
			else
			{
				HUINT8		*pucWrittenValue = NULL;

				pucWrittenValue = OxPAL_Malloc(ulSize);
				if (pucWrittenValue)
				{
					HLIB_STD_TaskSleep(100);
					hErr = PAL_PARAM_GetField(eItem, (void *)pucWrittenValue, ulSize);
					if (ERR_OK == hErr)
					{
						if (HxSTD_MemCmp(pvValue, pucWrittenValue, ulSize) == 0)
						{
							HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Set BIN Value (size:%04d)[Addr:%p]", ulSize, pvValue);
							pal_param_CmdDump(pvValue, ulSize, 0, FALSE, pszPrintBuf);
							nResult = HxCMD_OK;
						}
						else
						{
							HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail!");
							nResult = HxCMD_ERR;
						}
					}
				}
				else
				{
					HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail to Binary Buffer!");
					nResult = HxCMD_ERR;
				}
			}
		}
		break;

	default:
		HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"PARAM Set Fail Not Supported Type [%d]!!!", ulDataType);
		nResult = HxCMD_ERR;
		break;
	}

	return nResult;
}

#if 0
STATIC int pal_param_CmdSetValueFromAgr(PAL_PARAM_Item_e eItem, HCHAR **ppszArgList, HUINT8 *pucData)
{
	HINT32		nResult = HxCMD_ERR;
	HUINT32		ulDataSize = 0;

	if (NULL == pucData)
		return HxCMD_ERR;

	nResult = pal_param_CmdParseValueFromArg(eItem, ppszArgList, (void *)pucData, &ulDataSize);
	if (HxCMD_OK == nResult)
	{
		nResult = pal_param_CmdSetValue(eItem, ppszArgList, (void *)pucData, ulDataSize);
	}

	return nResult;
}
#endif

STATIC int pal_param_CmdSetValueList(HUINT32 ulStartItem, HUINT32 ulEndItem, HUINT8 *pucData, HCHAR *pszPrintBuf)
{
	HINT32				nResult;
	PAL_PARAM_Item_e	eItem;
	HINT32				i;
	HUINT32  			ulDataSize = 0;
	HCHAR				szPreFix[palParam_CMD_STR_BUFF_SIZE];
	HUINT32 			ulDataType, ulIgnoredNum, ulErrNum;
	HxList_t			*pstIgnoredList = NULL, *pstErrList = NULL;

	if (NULL == pucData)
	{
		return HxCMD_ERR;
	}

	for (i=ulStartItem; i<=ulEndItem; i++)
	{
		eItem = (PAL_PARAM_Item_e)i;

		// Get Data Type
		ulDataType = pal_param_GetDataType(eItem);
		// Get Prefix
		HxSTD_snprintf((char *)szPreFix, palParam_CMD_STR_BUFF_SIZE, (const char *)"(%04d)[%-36s][%s]", eItem, pal_param_GetItemStr(eItem), pal_param_GetDataTypeStr(ulDataType));

		if (pal_param_CmdCheckIgnoreItem(eItem, TRUE))
		{
			HLIB_CMD_Printf("%s : Ignored Item\n", szPreFix);
			pstIgnoredList = HLIB_LIST_Append(pstIgnoredList, (void *)eItem);
		}
		else
		{
			pszPrintBuf[0] = '\0';
			nResult = pal_param_CmdGetValueFromHardcode(eItem, pucData, &ulDataSize);
			if (HxCMD_OK == nResult)
			{
				nResult = pal_param_CmdSetValue(eItem, pucData, ulDataSize, pszPrintBuf);
				if (HxCMD_OK != nResult)
				{
					pstErrList = HLIB_LIST_Append(pstErrList, (void *)eItem);
				}
				HLIB_CMD_Printf("%s : %s\n", szPreFix, pszPrintBuf);
				if (ePAL_PARAM_TYPE_BIN == ulDataType)
				{
					HLIB_CMD_Printf("\n");
				}
				HLIB_STD_TaskSleep(100);
			}
			else
			{
				pstErrList = HLIB_LIST_Append(pstErrList, (void *)eItem);
				HLIB_CMD_Printf("%s : Fail To Get Hardcoded Data!!\n", szPreFix);
			}
		}
	}

	// Print Failed or Ignored List

	ulIgnoredNum = HLIB_LIST_Length(pstIgnoredList);
	ulErrNum = HLIB_LIST_Length(pstErrList);
	if (ulIgnoredNum>0 || ulErrNum>0)
	{
		HxList_t	*pstTmpList;

		HLIB_CMD_Printf("\n\t======================================\n");
		HLIB_CMD_Printf("\t== Ignored(%04d) / Failed(%04d) Item List ==\n", ulIgnoredNum, ulErrNum);
		HLIB_CMD_Printf("\t======================================\n");
		pstTmpList = pstIgnoredList;
		while (pstTmpList)
		{
			eItem = (PAL_PARAM_Item_e)HLIB_LIST_Data(pstTmpList);
			HLIB_CMD_Printf("\t== [%-10s] : (%04d)[%-36s]\n", "Ignored", eItem, pal_param_GetItemStr(eItem), pal_param_GetDataTypeStr(pal_param_GetDataType(eItem)));
			pstTmpList = HLIB_LIST_Next(pstTmpList);
		}
		pstIgnoredList = HLIB_LIST_RemoveAll(pstIgnoredList);
		(void)pstIgnoredList;

		pstTmpList = pstErrList;
		while (pstTmpList)
		{
			eItem = (PAL_PARAM_Item_e)HLIB_LIST_Data(pstTmpList);
			HLIB_CMD_Printf("\t== [%-10s] : (%04d)[%-36s]\n", "Failed", eItem, pal_param_GetItemStr(eItem), pal_param_GetDataTypeStr(pal_param_GetDataType(eItem)));
			pstTmpList = HLIB_LIST_Next(pstTmpList);
		}
		pstErrList = HLIB_LIST_RemoveAll(pstErrList);
		(void)pstErrList;

		HLIB_CMD_Printf("\t======================================\n\n");
	}

	return HxCMD_OK;
}

STATIC int pal_param_CmdSetValueFromArg(PAL_PARAM_Item_e eItem, HUINT8 *pucData, HCHAR *pszArg, HCHAR *pszPrintBuf)
{
	HINT32		nResult = HxCMD_OK;
	HUINT32		ulDataSize = 0;

	nResult = pal_param_CmdParseValueFromArg(eItem, pszArg, pucData, &ulDataSize, pszPrintBuf);
	if (HxCMD_OK == nResult)
	{
		nResult = pal_param_CmdSetValue(eItem, (void *)pucData, ulDataSize, pszPrintBuf);
	}

	return nResult;
}

STATIC int pal_param_SetCmd(void *pvArgList)
{
	HCHAR				*pszArgList = (HCHAR *)pvArgList;
	HUINT32 			 ulStart, ulEnd;
	HBOOL				 bList = FALSE;
	HINT32				 nTemp, nRes;
	HUINT8				*pucData = NULL;
	HCHAR				*pszArg = NULL;
	HCHAR				*pszPrintBuf = NULL;

	if (NULL == pvArgList)
	{
		HLIB_CMD_Printf("Fail to Get 1st Argument! NULL String!\n");
		return HxCMD_ERR;
	}

	pszArg = HLIB_CMD_GetNextArg(&pszArgList);
	if (pszArg == NULL)
	{
		ulStart = 0;
		ulEnd = ePAL_PARAM_MAX-1;
		bList = TRUE;
	}
	else
	{
		nRes = sscanf(pszArg, "%d", &nTemp);
		if (nRes == 0)
		{
			HCHAR szArg[32];

			nRes = sscanf(pszArg, "%s", szArg);
			if (nRes == 0)
			{
				HLIB_CMD_Printf("Fail to Get 1st Argument! Invalid Format [%s]\n", pszArg);
				return HxCMD_ERR;
			}
			else
			{
				if (HxSTD_StrCmp(HLIB_STD_StrUpper(szArg), "ALL") == 0)
				{
					ulStart = 0;
					ulEnd = ePAL_PARAM_MAX-1;
					bList = TRUE;
				}
				else
				{
					HLIB_CMD_Printf("Fail to Get 1st Argument! Invalid String Format [%s]\n", szArg);
					return HxCMD_ERR;
				}
			}
		}
		else
		{
			ulStart = ulEnd = (HUINT32)nTemp;
		}
	}

	pucData = OxPAL_Malloc(sizeof(HUINT8)*palParam_CMD_STR_BUFF_SIZE*100);
	if (NULL == pucData)
	{
		HLIB_CMD_Printf("Fail to Alloc Buffer!\n");
		goto err_exit;
	}

	// Alloc Print Buffer
	pszPrintBuf = OxPAL_Malloc(sizeof(HUINT8)*palParam_CMD_STR_BUFF_SIZE*100);
	if (NULL == pszPrintBuf)
	{
		HLIB_CMD_Printf("NULL == pszPrintBuf\n");
		goto err_exit;
	}

	if (bList)
	{
		if (ulStart == ulEnd)
		{
			HLIB_CMD_Printf("\n\t\t [PAL_PARAM] SetItem [%d]\n", ulStart);
		}
		else
		{
			HLIB_CMD_Printf("\n\t\t [PAL_PARAM] SetItem List [From %d To %d]\n", ulStart, ulEnd);
		}
		(void)pal_param_CmdSetValueList(ulStart, ulEnd, pucData, pszPrintBuf);
	}
	else
	{
		HCHAR				szPreFix[palParam_CMD_STR_BUFF_SIZE];
		HUINT32 			ulDataType;
		PAL_PARAM_Item_e	eItem = (PAL_PARAM_Item_e)ulStart;

		// Get Data Type
		ulDataType = pal_param_GetDataType(eItem);
		// Get Prefix
		HxSTD_snprintf((char *)szPreFix, palParam_CMD_STR_BUFF_SIZE, (const char *)"(%04d)[%-36s][%s]", eItem, pal_param_GetItemStr(eItem), pal_param_GetDataTypeStr(ulDataType));

		if (pal_param_CmdCheckIgnoreItem(eItem, TRUE))
		{
			HLIB_CMD_Printf("%s : Ignored Item\n", szPreFix);
		}
		else
		{
			pszArg = HLIB_CMD_GetNextArg(&pszArgList);
			if (pszArg)
			{
				(void)pal_param_CmdSetValueFromArg(eItem, pucData, pszArg, pszPrintBuf);
				HLIB_CMD_Printf("%s : %s\n", szPreFix, pszPrintBuf);
			}
			else
			{
				HLIB_CMD_Printf("%s : Fail to Get 2nd Argument! Invalid String Format [%s]\n", szPreFix, pszArg);
				goto err_exit;
			}
		}
	}

	if (pucData)		OxPAL_Free(pucData);
	if (pszPrintBuf)	OxPAL_Free(pszPrintBuf);

	return HxCMD_OK;

err_exit:
	if (pucData)		OxPAL_Free(pucData);
	if (pszPrintBuf)	OxPAL_Free(pszPrintBuf);
	return HxCMD_ERR;
}

STATIC int pal_param_CmdGetIntValue(PAL_PARAM_Item_e eItem, HCHAR *pszPrintBuf, HUINT32 ulSize)
{
	HERROR		hErr;
	HUINT32 	ulValue = 0;

	hErr = PAL_PARAM_GetField(eItem, (void *)&ulValue, ulSize);
	if (ERR_OK != hErr)
	{
//		HLIB_CMD_Printf("Fail! PAL_PARAM_GetField(%d, %p, %d) error [%d]\n", eItem, &ulValue, ulSize, hErr);
		return HxCMD_ERR;
	}

	HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Get INT Value (size:%04d)[value:(0x%08x),%d]!", ulSize, ulValue, (int)ulValue);

	return HxCMD_OK;
}

STATIC int pal_param_CmdGetStrValue(PAL_PARAM_Item_e eItem, HCHAR *pszPrintBuf, HUINT32 ulSize)
{
	HERROR			 hErr;
	HCHAR			*pszValue;
	STATIC HCHAR	 s_szBuff[palParam_CMD_STR_BUFF_SIZE];
	HINT32			 nRet;
	HBOOL			 bAlloc = FALSE;

	if (0 == ulSize)
	{
		ulSize = palParam_CMD_STR_BUFF_SIZE;
	}

	if (ulSize <= palParam_CMD_STR_BUFF_SIZE)
	{
		pszValue = s_szBuff;
	}
	else
	{
		pszValue = OxPAL_Malloc(ulSize);
		if (NULL != pszValue)
			bAlloc = TRUE;
	}

	if (NULL == pszValue)
	{
		HLIB_CMD_Printf("Fail! : NULL == pszValue\n");
		return HxCMD_ERR;
	}

	hErr = PAL_PARAM_GetField(eItem, (void *)pszValue, ulSize);
	if (ERR_OK == hErr)
	{
		HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Get STR Value (size:%04d)[value:\"%s\"]", ulSize, pszValue);
		nRet = HxCMD_OK;
	}
	else
	{
		HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Fail! : PAL_PARAM_GetField(%d, %p, %d) error [%d]", eItem, pszValue, ulSize, hErr);
		nRet = HxCMD_ERR;
	}

	if (bAlloc)
		OxPAL_Free(pszValue);

	return nRet;
}

#define	palParam_DUMP_NUMBER_IN_LINE		16
#define	palParam_DUMP_NUMBER_IN_PAGE		0x200

void pal_param_CmdDump(HUINT8 *pucBuffer, HUINT32 unSize, HUINT32 unBaseAddr, HUINT8 bFlagDetail, HCHAR *pszPrintBuf)
{
	HUINT32		 unChar, unCount, unNumOfDump;
	HUINT32		 ullAddr = 0;
	HUINT8		 aucChar[palParam_DUMP_NUMBER_IN_LINE + 1];
	int			 i;
	HCHAR		 szBuff[palParam_CMD_STR_BUFF_SIZE];
	HCHAR		*pszLine;

	HxSTD_MemSet(aucChar, 0, palParam_DUMP_NUMBER_IN_LINE + 1);

	i = 0;
	unNumOfDump = 0;

	ullAddr += unBaseAddr;

	pszLine = pszPrintBuf;

	while (unNumOfDump < unSize)
	{
		unChar = 0x000000ff & (*pucBuffer++);

		if ((unChar >= 0x20) && (unChar <= 0x7e))
			aucChar[i++] = (HUINT8)unChar;
		else
			aucChar[i++] = '.';

		if ((ullAddr % palParam_DUMP_NUMBER_IN_PAGE) == 0)
		{
			if (bFlagDetail)
			{
				HxSTD_StrNCat((char *)pszLine, "\n-----------------------------------------------\n", palParam_CMD_STR_BUFF_SIZE);
			}
			else
			{
				HxSTD_StrNCat((char *)pszLine, (const char *)"\n"
							"\t\t\t00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n"
							"\t\t\t-----------------------------------------------\n"
							"\t\t\t |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |",
							200
						);
			}
		}

		if ((ullAddr % palParam_DUMP_NUMBER_IN_LINE) == 0)
		{
			if (bFlagDetail)
			{
				HxSTD_snprintf((char *)szBuff, palParam_CMD_STR_BUFF_SIZE, (const char *)"\n");
			}
			else
			{
				HxSTD_snprintf((char *)szBuff, palParam_CMD_STR_BUFF_SIZE, (const char *)"\n\t%8x : \t", ullAddr);
			}
			HxSTD_StrNCat((char *)pszLine, szBuff, palParam_CMD_STR_BUFF_SIZE);
		}

		HxSTD_snprintf((char *)szBuff, palParam_CMD_STR_BUFF_SIZE, (const char *)"%02x ", unChar);
		HxSTD_StrNCat((char *)pszLine, szBuff, palParam_CMD_STR_BUFF_SIZE);

		if ((ullAddr % palParam_DUMP_NUMBER_IN_LINE) == (palParam_DUMP_NUMBER_IN_LINE - 1))
		{
			HxSTD_snprintf((char *)szBuff, palParam_CMD_STR_BUFF_SIZE, (const char *)"\t%s", aucChar);
			HxSTD_StrNCat((char *)pszLine, szBuff, palParam_CMD_STR_BUFF_SIZE);
			i = 0;
		}


		ullAddr++;
		unNumOfDump++;
	}

	unCount = ullAddr % palParam_DUMP_NUMBER_IN_LINE;

	aucChar[i] = 0;
	if (unCount != 0)
	{
		for (i = unCount; i < palParam_DUMP_NUMBER_IN_LINE; ++i)
		{
			HxSTD_StrNCat((char *)pszLine, "   ", 10);
		}
	}

	HxSTD_snprintf((char *)szBuff, palParam_CMD_STR_BUFF_SIZE, (const char *)"\t%s", aucChar);
	HxSTD_StrNCat((char *)pszLine, szBuff, palParam_CMD_STR_BUFF_SIZE);
}


STATIC int pal_param_CmdGetBinValue(PAL_PARAM_Item_e eItem, HCHAR *pszPrintBuf, HUINT32 ulSize)
{
	HERROR		 hErr;
	HUINT8		*pucBin = NULL;
	HINT32		 nRet;

	if (0 == ulSize)
	{
		HLIB_CMD_Printf("0 == ulSize\n");
		return HxCMD_ERR;
	}

	pucBin = OxPAL_Malloc(sizeof(HUINT8)*ulSize);
	if (NULL == pucBin)
	{
		HLIB_CMD_Printf("NULL == pucBin\n");
		return HxCMD_ERR;
	}

	hErr = PAL_PARAM_GetField(eItem, (void *)pucBin, ulSize);
	if (ERR_OK == hErr)
	{
		HxSTD_snprintf((char *)pszPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Get BIN Value (size:%04d)[Addr:%p]\n", ulSize, pucBin);
		pal_param_CmdDump(pucBin, ulSize, 0, FALSE, pszPrintBuf);
		nRet = HxCMD_OK;
	}
	else
	{
		nRet = HxCMD_ERR;
	}

	OxPAL_Free(pucBin);

	return nRet;
}

STATIC int pal_param_CmdPrintValues(HUINT32 ulStartItem, HUINT32 ulEndItem)
{
	HUINT32 			 i;
	HUINT32 			 ulSize = 0;
	HCHAR				*pszPrintBuf = NULL;
	HCHAR				 szPreFix[palParam_CMD_STR_BUFF_SIZE];
	HUINT32 			 ulDataType;
	HERROR				 hErr;
	HUINT32				 ulErrNum, ulIgnoredNum;
	PAL_PARAM_Item_e	 eItem, aeIgnoredItems[ePAL_PARAM_MAX], aeErrItems[ePAL_PARAM_MAX];
	HBOOL				 bPrintValue;

	if (ulStartItem == ulEndItem)
	{
		HLIB_CMD_Printf("\n\t\t [PAL_PARAM] GetItem [%d]\n", ulStartItem);
	}
	else
	{
		HLIB_CMD_Printf("\n\t\t [PAL_PARAM] GetItem List [From %d To %d]\n", ulStartItem, ulEndItem);
	}

	// Alloc Print Buffer
	pszPrintBuf = OxPAL_Malloc(sizeof(HUINT8)*palParam_CMD_STR_BUFF_SIZE*100);
	if (NULL == pszPrintBuf)
	{
		HLIB_CMD_Printf("NULL == pszPrintBuf\n");
		return HxCMD_ERR;
	}

	ulErrNum = ulIgnoredNum = 0;
	for (i=ulStartItem; i<=ulEndItem; i++)
	{
		// Get Item
		eItem = (PAL_PARAM_Item_e)i;
		// Get Data Type
		ulDataType = pal_param_GetDataType(eItem);
		// Get Prefix
		HxSTD_snprintf((char *)szPreFix, palParam_CMD_STR_BUFF_SIZE, (const char *)"(%04d)[%-36s][%s]", eItem, pal_param_GetItemStr(eItem), pal_param_GetDataTypeStr(ulDataType));

		// Check Ignored Item
		if (pal_param_CmdCheckIgnoreItem(eItem, FALSE))
		{
			HLIB_CMD_Printf("%s : Ignored Item\n", szPreFix);
			aeIgnoredItems[ulIgnoredNum++] = eItem;
		}
		else
		{
			// Get Data Size
			ulSize = pal_param_CmdGetDefaultSize(eItem);
			// Print Data or Not
			bPrintValue = (0 < ulSize && 1024 > ulSize) ? TRUE : FALSE; // 너무 크면 출력하지 않는다..

			if (bPrintValue)
			{
				pszPrintBuf[0] = '\0';
				hErr = ERR_FAIL;
				switch (ulDataType)
				{
				case ePAL_PARAM_TYPE_INT:		hErr = pal_param_CmdGetIntValue(eItem, pszPrintBuf, ulSize);		break;
				case ePAL_PARAM_TYPE_STR:		hErr = pal_param_CmdGetStrValue(eItem, pszPrintBuf, ulSize);		break;
				case ePAL_PARAM_TYPE_BIN:		hErr = pal_param_CmdGetBinValue(eItem, pszPrintBuf, ulSize);		break;
				case ePAL_PARAM_TYPE_MAX:		break;
				default:						break;
				}

				if (ERR_OK == hErr)
				{
					HLIB_CMD_Printf("%s : %s\n", szPreFix, pszPrintBuf);
					if (ePAL_PARAM_TYPE_BIN == ulDataType)
					{
						HLIB_CMD_Printf("\n");
					}
				}
				else
				{
					HLIB_CMD_Printf("%s : Fail To Get Data! (size:%d)\n", szPreFix, ulSize);
					aeErrItems[ulErrNum++] = eItem;
				}
			}
			else
			{
				if (0 == ulSize)
				{
					HLIB_CMD_Printf("%s : Fail To Get Default Data Size\n", szPreFix, ulSize);
				}
				else
				{
					HLIB_CMD_Printf("%s : Too Big To Print (size:%d)\n", szPreFix, ulSize);
				}
				aeErrItems[ulErrNum++] = eItem;
			}
			HLIB_STD_TaskSleep(100);
		}
	}

	// Free Print Buffer
	if (pszPrintBuf)
	{
		OxPAL_Free(pszPrintBuf);
	}

	// Print Failed or Ignored List
	if (ulIgnoredNum>0 || ulErrNum>0)
	{
		HLIB_CMD_Printf("\n\t================================\n");
		HLIB_CMD_Printf("\t== Failed / Ignored Item List ==\n");
		HLIB_CMD_Printf("\t================================\n");
		for (i=0; i<ulIgnoredNum; i++)
		{
			eItem = aeIgnoredItems[i];
			HLIB_CMD_Printf("\t== [%-10s] : (%04d)[%-36s]\n", "Ignored", eItem, pal_param_GetItemStr(eItem), pal_param_GetDataTypeStr(pal_param_GetDataType(eItem)));
		}
		for (i=0; i<ulErrNum; i++)
		{
			eItem = aeErrItems[i];
			HLIB_CMD_Printf("\t== [%-10s] : (%04d)[%-36s]\n", "Failed", eItem, pal_param_GetItemStr(eItem), pal_param_GetDataTypeStr(pal_param_GetDataType(eItem)));
		}
		HLIB_CMD_Printf("\t================================\n\n");
	}

	return HxCMD_OK;
}


STATIC int pal_param_GetCmd(void *pvArgList)
{
	HINT32		 nTemp, nRes;
	HCHAR		*pszArg = NULL;
	HUINT32 	 ulStart, ulEnd;
	HCHAR		*pszArgList = (HCHAR *)pvArgList;

	if (NULL == pvArgList)
	{
		HLIB_CMD_Printf("Fail to Get 1st Argument! NULL String!\n");
		return HxCMD_ERR;
	}

	ulStart = ulEnd = 0;

	pszArg = HLIB_CMD_GetNextArg(&pszArgList);
	if (pszArg == NULL)
	{
		ulStart = 0;
		ulEnd = ePAL_PARAM_MAX-1;
	}
	else
	{
		nRes = sscanf(pszArg, "%d", &nTemp);
		if (nRes == 0)
		{
			HCHAR szArg[32];

			nRes = sscanf(pszArg, "%s", szArg);
			if (nRes == 0)
			{
				HLIB_CMD_Printf("Fail to Get 1st Argument! Invalid Format [%s]\n", pszArg);
				return HxCMD_ERR;
			}
			else
			{
				if (HxSTD_StrCmp(HLIB_STD_StrUpper(szArg), "ALL") == 0)
				{
					ulStart = 0;
					ulEnd = ePAL_PARAM_MAX-1;
				}
				else
				{
					HLIB_CMD_Printf("Fail to Get 1st Argument! Invalid String Format [%s]\n", szArg);
					return HxCMD_ERR;
				}
			}
		}
		else
		{
			ulStart = ulEnd = (HUINT32)nTemp;
		}
	}

	pal_param_CmdPrintValues(ulStart, ulEnd);

	return HxCMD_OK;
}


STATIC void pal_param_UtTestSetValue(Cutest_TestCase_t *tc, PAL_PARAM_Item_e eItem, void *pvValue, HUINT32 ulSize)
{

	HERROR	 hErr;
	HUINT32	 ulDataType;
	HUINT8	szPrintBuf[palParam_CMD_STR_BUFF_SIZE] = {0, };


	if (NULL == pvValue)
	{
		HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Field (%d) fail, NULL == pvValue", eItem);
		CutestFail(tc, (const char *)&szPrintBuf);
		return ;
	}
	if (0 == ulSize)
	{
		HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Field (%d) fail, 0 == ulSize", eItem);
		CutestFail(tc, (const char *)&szPrintBuf);
		return ;
	}

	// Get Data Type
	ulDataType = pal_param_GetDataType(eItem);



	switch (ulDataType)
	{
	case ePAL_PARAM_TYPE_INT:
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)pvValue, ulSize);
			if (ERR_OK != hErr)
			{
				HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Set Field (%d) fail", eItem);
				CutestFail(tc, (const char *)&szPrintBuf);
			}
			else
			{
				void		*pvWrittenValue = NULL;
				HUINT8		 ucWrittenValue = 0;
				HUINT16		 usWrittenValue = 0;
				HUINT32		 ulWrittenValue = 0;

				switch (ulSize)
				{
				case 1: pvWrittenValue = &ucWrittenValue; break;
				case 2: pvWrittenValue = &usWrittenValue; break;
				case 4: pvWrittenValue = &ulWrittenValue; break;
				default:									break;
				}

				HLIB_STD_TaskSleep(100);
				hErr = PAL_PARAM_GetField(eItem, (void *)pvWrittenValue, ulSize);
				if (ERR_OK == hErr)
				{
					HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Field (%d) different value", eItem);
					CutestAssertMemEquals_Msg(tc, (const char *)&szPrintBuf, pvValue, pvWrittenValue, ulSize);
				}
				else
				{
					HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Get Field (%d) fail", eItem);
					CutestFail(tc, (const char *)&szPrintBuf);
				}



			}
		}
		break;

	case ePAL_PARAM_TYPE_STR:
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)pvValue, ulSize);
			if (ERR_OK != hErr)
			{
				HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Set Field (%d) fail", eItem);
				CutestFail(tc, (const char *)&szPrintBuf);
			}
			else
			{
				HCHAR		*pszWrittenValue = NULL;

				pszWrittenValue = OxPAL_Malloc(ulSize);
				if (pszWrittenValue)
				{
					HLIB_STD_TaskSleep(100);
					pszWrittenValue[0] = '\0';
					hErr = PAL_PARAM_GetField(eItem, (void *)pszWrittenValue, ulSize);
					if (ERR_OK == hErr)
					{
						HCHAR		*pszValue = (HCHAR *)pvValue;

						HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Field (%d) different value", eItem);
						CutestAssertStrEquals_Msg(tc, (const char *)&szPrintBuf, pszValue, pszWrittenValue);

					}
					OxPAL_Free(pszWrittenValue);
				}
				else
				{
					HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"alloc fail");
					CutestFail(tc, (const char *)&szPrintBuf);
				}
			}
		}
		break;

	case ePAL_PARAM_TYPE_BIN:
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)pvValue, ulSize);
			if (ERR_OK != hErr)
			{
				HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Set Field (%d) fail", eItem);
				CutestFail(tc, (const char *)&szPrintBuf);
			}
			else
			{
				HUINT8		*pucWrittenValue = NULL;

				pucWrittenValue = OxPAL_Malloc(ulSize);
				if (pucWrittenValue)
				{
					HLIB_STD_TaskSleep(100);
					hErr = PAL_PARAM_GetField(eItem, (void *)pucWrittenValue, ulSize);
					if (ERR_OK == hErr)
					{
						HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"Field (%d) different value", eItem);
						CutestAssertMemEquals_Msg(tc, (const char *)&szPrintBuf, pvValue, pucWrittenValue, ulSize);

					}
				}
				else
				{
					HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"alloc fail");
					CutestFail(tc, (const char *)&szPrintBuf);
				}
			}
		}
		break;

	default:
		HxSTD_snprintf((char *)&szPrintBuf, palParam_CMD_STR_BUFF_SIZE, (const char *)"PARAM Set Fail Not Supported Type [%d]!!!", ulDataType);
		CutestFail(tc, (const char *)&szPrintBuf);

		break;
	}

	return ;
}

STATIC int pal_param_UtTestSetValueList(Cutest_TestCase_t *tc, HUINT32 ulStartItem, HUINT32 ulEndItem, HUINT8 *pucData)
{
	HINT32				nResult;
	PAL_PARAM_Item_e	eItem;
	HINT32				i;
	HUINT32  			ulDataSize = 0;


	if (NULL == pucData)
	{
		return HxCMD_ERR;
	}

	for (i=ulStartItem; i<=ulEndItem; i++)
	{
		eItem = (PAL_PARAM_Item_e)i;


		if (pal_param_CmdCheckIgnoreItem(eItem, TRUE))
		{
			;
		}
		else
		{
			nResult = pal_param_CmdGetValueFromHardcode(eItem, pucData, &ulDataSize);
			if (HxCMD_OK == nResult)
			{
				pal_param_UtTestSetValue(tc, eItem, pucData, ulDataSize);
			}
			else
			{
				;
			}
		}
	}


	return HxCMD_OK;
}

void pal_param_UtTestSet(Cutest_TestCase_t *tc)
{
	HUINT32 			 ulStart, ulEnd;
	HUINT8				*pucData = NULL;


	ulStart = 0;
	ulEnd = ePAL_PARAM_MAX-1;

	pucData = OxPAL_Malloc(sizeof(HUINT8)*palParam_CMD_STR_BUFF_SIZE*100);
	if (NULL == pucData)
	{
		HLIB_CMD_Printf("Fail to Alloc Buffer!\n");
		goto END_FUNC;
	}


	pal_param_UtTestSetValueList(tc, ulStart, ulEnd, pucData);


END_FUNC:

	if (pucData)		OxPAL_Free(pucData);

	return ;

}


STATIC Cutest_Suite_t* pal_param_UtGetSuite(void)
{
	Cutest_Suite_t* suite = CUTEST_SuiteNew();


	CUTEST_SUITE_ADD_TEST(suite, pal_param_UtTestSet, "test");

	return suite;
}

STATIC int pal_param_Ut(void *pvArgList)
{
	Cutest_String_t *output = CUTEST_StringNew();
	Cutest_Suite_t* suite = CUTEST_SuiteNew();

	CUTEST_SuiteMerge(suite, pal_param_UtGetSuite());


	CUTEST_SuiteRun(suite, 0);
	CUTEST_SuiteSummary(suite, output);
	CUTEST_SuiteDetail(suite, output);

	CUTEST_MakeTestListToFile(suite, "pal_param_UnitaryTest");
	CUTEST_MakeTestResultToFile(suite, "pal_param_UnitaryTest");

	HLIB_CMD_Printf("%s\n", output->buffer);

	CUTEST_StringDelete(output);
	CUTEST_SuiteDelete(suite);

	return HxCMD_OK;
}
#endif

/**********************************************
 Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
#define	hCmdHandle		"pal_param"

void CMD_Register_PAL_Param(void)
{
#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord(hCmdHandle,
			pal_param_CmdPrintItem,
			"param_list", 								/* keyword */
			"param_list",								/*	 help  */
			"param_list");								/*	usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			pal_param_SetCmd,
			"param_set", 								/* keyword */
			"set PAL_PARAM Item",						/*	 help  */
			"param_set all/[eItem Value Size]");		/*	usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			pal_param_GetCmd,
			"param_get", 								/* keyword */
			"Get PAL_PARAM Item(s)",					/*	 help  */
			"param_get all/[eItem]");					/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,
			pal_param_Ut,
			"param_ut", 								/* keyword */
			"unitary test",					/*	 help  */
			"param_ut ");					/*	usage  */
#endif
}


