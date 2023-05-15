/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_sys.c
	@brief	  Octo pal system information

	Description: Product 정보를 제공하는 PAL		\n
	Module: PAL/SYS			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <stdio.h>

#include <htype.h>
#include <stb_params.h>
#include <isosvc.h>
#include <hapi.h>

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
#include <nvram_map.h>

#include <pal_sys.h>
#include <db_param.h>
#include <pal_channel.h>

#if defined( CONFIG_MW_INET_SNMP )
#include <di_snmp.h>
#endif
#if defined(CONFIG_DEBUG_AGING_TEST) && defined(CONFIG_PROD_DEVICE_STORAGE_USB) && !defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_SUPPORT_UMMA)
#include "di_usb.h"
#endif


/*******************************************************************/
/************************     Definition   * ***********************/
/*******************************************************************/
#define palSys_ENCRYPTION_KEY_SIZE 			16
#define palSys_FILE_STRING_LENGTH			80
#define	palSys_MAKE_STR_LEN					32

#define palSys_VERSION_FILE_NAME			"/etc/.version"
#define palSys_RESOURCE_VERSION_FILE_NAME	"/opt/share/.version"
#define palSys_DB_VERSION_FILE_NAME			OBAMA_DIR_DATA "/.version"
#define palSys_KERNEL_VERSION_FILE_NAME		"/proc/version"

#define palSys_TOO_MUCH_FOR_FILEUPDATE		(256 * 1024 * 1024)			// 256 MByte

#define palSys_MICOM_VER_STR_LENGTH			9

#define	palSys_MAX_VER_HEAD_LEN				6
#define	palSys_MAX_APP_VER_LEN				(palSys_MAX_VER_HEAD_LEN + 3)
#define	palSys_MAX_LOADER_VER_LEN			(palSys_MAX_VER_HEAD_LEN + 2)
#define	palSys_MAX_MICOM_VER_LEN			(palSys_MAX_VER_HEAD_LEN + 4)
#define	palSys_MAX_UPGRADER_VER_LEN			(palSys_MAX_VER_HEAD_LEN + 3)
#define	palSys_MAX_VER_LEN					10

#define	palSys_GET_VER_VAL(a, b, c)		{																\
											HUINT8	i=0;												\
											*a = 0;													\
											for(i=palSys_MAX_VER_HEAD_LEN; i<c; i++)							\
											{														\
												*a |= (HUINT32)b[i] << ((c - i -1)*8);					\
											}														\
										}

#if 0//defined(CONFIG_DROP_ROOT_PRIVILEGES)
#define ALLOW_DIR_PERMISSION			(0776)	//USER(rWx)Group(rwx)Other(rw-)
#define ALLOW_FILE_PERMISSION			(0666)	//USER(rw-)Group(rw-)Other(rw-)
#endif
#if defined(CONFIG_MW_CAS_IRDETO)
#define	palSys_LOADER_IRDTO_VER_HEAD				(HCHAR*)"IRDETO"
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eVersionByteType_2Bytes,
	eVersionByteType_Irdeto,
#if defined(CONFIG_OTA_USE_PKG_VERSION)
	eVersionByteType_4Bytes,
#endif
	eVersionByteType_Default
} PalSys_VersionByteType_e;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32	s_ulEncryptType		= 0;

#if defined(CONFIG_DEBUG_AGING_TEST) && defined(CONFIG_PROD_DEVICE_STORAGE_USB) && !defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_SUPPORT_UMMA)
STATIC HCHAR *pszMountName = NULL;
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

typedef struct
{
	PAL_FLASH_FIELD_t		ePalField;
	DI_NVRAM_FIELD_T		eDiNvramField;
} palSys_PanelConverterInfo_t;

palSys_PanelConverterInfo_t s_stPalSysConvInfo[PAL_FLASH_FIELD_MAX] =
{
	 {PAL_FLASH_FIELD_SERIAL_NO,					DI_NVRAM_FIELD_SERIAL_NO}
	,{PAL_FLASH_FIELD_DTCP_KEY,						DI_NVRAM_FIELD_DTCP_KEY}
	,{PAL_FLASH_FIELD_CI_PLUS_KEY,					DI_NVRAM_FIELD_CI_PLUS_KEY}
	,{PAL_FLASH_FIELD_MAC_ADDR,						DI_NVRAM_FIELD_MAC_ADDR}
	,{PAL_FLASH_FIELD_HDCP_KEY,						DI_NVRAM_FIELD_HDCP_KEY}
	,{PAL_FLASH_FIELD_PARING_DATA,					DI_NVRAM_FIELD_PARING_DATA}
	,{PAL_FLASH_FIELD_SYSTEM_ID,					DI_NVRAM_FIELD_SYSTEM_ID}
	,{PAL_FLASH_FIELD_LOADER_VER,					DI_NVRAM_FIELD_LOADER_VERSION}
	,{PAL_FLASH_FIELD_APP_VERSION_NEXT,				DI_NVRAM_FIELD_APP_VERSION_NEXT}
	,{PAL_FLASH_FIELD_APP_VER,						DI_NVRAM_FIELD_APP_VERSION}
	,{PAL_FLASH_FIELD_PRIVATE,						DI_NVRAM_FIELD_PRIVATE}
	,{PAL_FLASH_FIELD_CSC_KEY,						DI_NVRAM_FIELD_CSC_KEY}
	,{PAL_FLASH_FIELD_DSTB_ID,						DI_NVRAM_FIELD_DSTB_ID}

	,{PAL_FLASH_FIELD_IRDETO_BBCB,					DI_NVRAM_FIELD_IRDETO_BBCB}
	,{PAL_FLASH_FIELD_IRDETO_UC_SECURE_ID,			DI_NVRAM_FIELD_IRDETO_UC_SECURE_ID}
	,{PAL_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA,		DI_NVRAM_FIELD_IRDETO_UC_PRIVATE_DATA}
	,{PAL_FLASH_FIELD_IRDETO_CPCB,					DI_NVRAM_FIELD_IRDETO_CPCB}
	,{PAL_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED,		DI_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED}
	,{PAL_FLASH_FIELD_IRDETO_CA_PARTITION,			DI_NVRAM_FIELD_IRDETO_CA_PARTITION}
	,{PAL_FLASH_FIELD_IRDETO_LOADER_PARTITION,		DI_NVRAM_FIELD_IRDETO_LOADER_PARTITION}
	,{PAL_FLASH_FIELD_IRDETO_IR_PARTITION,			DI_NVRAM_FIELD_IRDETO_IR_PARTITION}
	,{PAL_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION,	DI_NVRAM_FIELD_IRDETO_IR_BACKUP_PARTITION}

};

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
static PAL_OverTemperatureCallback_t	s_pfncOverTempCallback;
static PAL_FanLockCallback_t			s_pfncFanLockCallback;
#endif

STATIC HERROR pal_sys_MakeVerText (HUINT8 *szHeadStr, HUINT32 ulVerNum, HUINT8 *szText, PalSys_VersionByteType_e eVerType)
{
	HUINT32		ulVerNum1, ulVerNum2, ulVerNum3, ulTemp = 0;			// VerNum1.VerNum2.VerNum3

	if (szText == NULL)
	{
		return ERR_FAIL;
	}

	ulVerNum1 = (ulVerNum >> 16) & 0xFF;
	ulVerNum2 = (ulVerNum >> 8) & 0xFF;
	ulVerNum3 = (ulVerNum) & 0xFF;

	switch (eVerType)
	{
	case eVersionByteType_2Bytes:
		if (szHeadStr != NULL && szHeadStr[0] != '\0')
		{
			HxSTD_PrintToStrN (szText, 32, "%s %d.%02d", szHeadStr, ulVerNum2, ulVerNum3);
		}
		else
		{
			HxSTD_PrintToStrN (szText, 32, "%d.%02d", ulVerNum2, ulVerNum3);
		}

		break;

	case eVersionByteType_Irdeto:	// For Irdeto Loader
		ulVerNum1 = (ulVerNum >> 8) & 0xFF;
		ulTemp = (ulVerNum) & 0xFF;
		ulVerNum2 =(ulTemp>>4) & 0x0F;
		ulVerNum3 = (ulTemp) & 0x0F;

		if (szHeadStr != NULL && szHeadStr[0] != '\0')
		{
			HxSTD_PrintToStrN (szText, palSys_MAKE_STR_LEN, "%s %d.%d.%d", szHeadStr, ulVerNum1, ulVerNum2, ulVerNum3);
		}
		else
		{
			HxSTD_PrintToStrN (szText, palSys_MAKE_STR_LEN, "%d.%d.%d", ulVerNum1, ulVerNum2, ulVerNum3);
		}
		break;

#if defined(CONFIG_OTA_USE_PKG_VERSION)
	case eVersionByteType_4Bytes:
		ulVerNum1 = (ulVerNum >> 16) & 0xFFFF;
		ulVerNum2 = (ulVerNum >> 8) & 0xFF;
		ulVerNum3 = (ulVerNum) & 0xFF;
		if (szHeadStr != NULL && szHeadStr[0] != '\0')
		{
			HxSTD_PrintToStrN (szText, 32, "%s %d.%02d.%02d", szHeadStr, ulVerNum1, ulVerNum2, ulVerNum3);
		}
		else
		{
			HxSTD_PrintToStrN (szText, 32, "%d.%02d.%02d", ulVerNum1, ulVerNum2, ulVerNum3);
		}
		break;
#endif

	default:
		if (szHeadStr != NULL && szHeadStr[0] != '\0')
		{
			HxSTD_PrintToStrN (szText, 32, "%s %d.%02d.%02d", szHeadStr, ulVerNum1, ulVerNum2, ulVerNum3);
		}
		else
		{
			HxSTD_PrintToStrN (szText, 32, "%d.%02d.%02d", ulVerNum1, ulVerNum2, ulVerNum3);
		}

		break;
	}


	return ERR_OK;
}

STATIC HERROR pal_sys_GetRootFsVer (float *pfVer)
{
	FILE		*pFile;
	char		 szString[palSys_FILE_STRING_LENGTH];

	if (pfVer == NULL)					{ return ERR_FAIL; }

	*pfVer = 0.0;

	pFile = fopen (palSys_VERSION_FILE_NAME, "rt");
	if (pFile != NULL)
	{
		fgets (szString, palSys_FILE_STRING_LENGTH, pFile);
		szString[palSys_FILE_STRING_LENGTH - 1] = '\0';
		sscanf (szString, "%f", pfVer);
		fclose (pFile);
	}

	return ERR_OK;
}

STATIC HERROR pal_sys_GetResourceVer (float *pfVer)
{
	FILE		*pFile;
	char		 szString[palSys_FILE_STRING_LENGTH];

	if (pfVer == NULL)					{ return ERR_FAIL; }

	*pfVer = 0.0;

	pFile = fopen (palSys_RESOURCE_VERSION_FILE_NAME, "rt");
	if (pFile != NULL)
	{
		fgets (szString, palSys_FILE_STRING_LENGTH, pFile);
		szString[palSys_FILE_STRING_LENGTH - 1] = '\0';
		sscanf (szString, "%f", pfVer);
		fclose (pFile);
	}

	return ERR_OK;
}

STATIC HERROR pal_sys_GetDbVer (float *pfVer)
{
	FILE		*pFile;
	char		 szString[palSys_FILE_STRING_LENGTH];

	if (pfVer == NULL)					{ return ERR_FAIL; }

	*pfVer = 0.0;

	pFile = fopen (palSys_DB_VERSION_FILE_NAME, "rt");
	if (pFile != NULL)
	{
		fgets (szString, palSys_FILE_STRING_LENGTH, pFile);
		szString[palSys_FILE_STRING_LENGTH - 1] = '\0';
		sscanf (szString, "%f", pfVer);
		fclose (pFile);
	}

	return ERR_OK;
}

STATIC HERROR pal_sys_GetVersionHeader (VersionType_t eSystemType, HUINT8 *pucVersionHeader, HUINT32 ulBufMax)
{
	DI_ERR_CODE		 hErr;

	HUINT8			aucVer[palSys_MAX_VER_LEN];

	if (pucVersionHeader == NULL || ulBufMax < palSys_MAX_VER_HEAD_LEN + 1)
	{
		return PAL_SYS_ERR_FAIL;
	}

	HxSTD_MemSet(pucVersionHeader, 0, ulBufMax);
	switch (eSystemType)
	{
	case eVERSION_TYPE_LOADER:
		hErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_LOADER_VERSION, 0, (void *)aucVer, palSys_MAX_LOADER_VER_LEN);
		if (hErr == DI_ERR_OK)
		{
			HLIB_STD_StrUtf8NCpy(pucVersionHeader, aucVer, palSys_MAX_LOADER_VER_LEN);
			return ERR_OK;
		}
		else if (hErr == DI_ERR_CRC)
		{
			HLIB_STD_StrUtf8NCpy(pucVersionHeader, SYSTEM_APP_VER_HEAD, palSys_MAX_LOADER_VER_LEN);
			return ERR_FAIL;
		}
		break;

	case eVERSION_TYPE_APPL:
		HLIB_STD_StrUtf8NCpy(pucVersionHeader, SYSTEM_APP_VER_HEAD, palSys_MAX_LOADER_VER_LEN);
		return ERR_OK;

	case eVERSION_TYPE_UPGRADER:
		hErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RW, (HUINT8*)OTALDR_VERSION, 0, (void *)&aucVer, palSys_MAX_UPGRADER_VER_LEN);
		if(DI_ERR_OK == hErr)
		{
			HLIB_STD_StrUtf8NCpy(pucVersionHeader, aucVer, palSys_MAX_UPGRADER_VER_LEN);
			return ERR_OK;
		}
		break;

	default:
		break;
	}

	return ERR_FAIL;
}

STATIC HBOOL pal_sys_IsDigit (HUINT8 ucChar)
{
	if ((ucChar >= '0' && ucChar <= '9') || (ucChar == '.'))
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HERROR pal_sys_GetKernelVerStr (HUINT8 *szVerStr)
{
	HUINT32		 ulStrIdx, ulBufIdx;
	HUINT8		 szBuffer[32];
	FILE		*pFile;
	HINT32		 nRead = 0;

	if (szVerStr == NULL)					{ return ERR_FAIL; }

	pFile = fopen (palSys_KERNEL_VERSION_FILE_NAME, "rt");
	if (pFile != NULL)
	{
		HxSTD_MemSet (szBuffer, 0, 32);
		nRead = fread (szBuffer, 32, 1, pFile);
		/* fix for prevent #158029 */
		if(nRead == 0)
		{
			HxLOG_Warning("nRead is 0\n");
		}
		fclose (pFile);

		ulStrIdx = 0;
		ulBufIdx = 0;

		while (ulBufIdx < 32)
		{
			if (pal_sys_IsDigit (szBuffer[ulBufIdx]) == TRUE)
			{

				while (ulBufIdx < 32)
				{
					if (pal_sys_IsDigit (szBuffer[ulBufIdx]) == FALSE)
					{
						szVerStr[ulStrIdx] = '\0';
						return ERR_OK;
					}

					szVerStr[ulStrIdx] = szBuffer[ulBufIdx];
					ulStrIdx ++;
					ulBufIdx ++;
				}
			}

			ulBufIdx++;
		}

		if (ulStrIdx != 0)
		{
			szVerStr[ulStrIdx] = '\0';
			return ERR_OK;
		}
	}

	HxSTD_PrintToStrN (szVerStr, 8, "0.0.00");

	return ERR_OK;
}

STATIC DI_CRYPT_CryptMode pal_sys_GetEncrypteModePal2DI(PvrEncryptMode_t EncType)
{
	switch(EncType)
	{
	case ePvrEncrypt3Des : 	return DI_CRYPT_DES;
	case ePvrEncryptAes : 	return DI_CRYPT_AES;
	case ePvrEncryptNone :
	case ePvrEncryptUnknown :
	default:
		HxLOG_Error("unknown Encryption Type[%d]\n",EncType);
		return DI_CRYPT_MAX;
	}
}

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
void	pal_sys_cbOverTemp(void* pData)
{
	DI_SYS_EVENT *peSysEvt = NULL;

	peSysEvt = (DI_SYS_EVENT*)pData;

	HxLOG_Error("[MJAHN] [pal_sys_cbOverTemp] +++ \n");

	if(s_pfncOverTempCallback == NULL)
	{
		HxLOG_Error("\n\n\t\t[%s : %d] OverTemp Callback is NULL!! \n\n",__FUNCTION__,__LINE__);
		return;
	}
	if(pData == NULL)
	{
		HxLOG_Error("\n\n\t\t[%s : %d] param is NULL!! \n\n",__FUNCTION__,__LINE__);
		return;
	}

	HxLOG_Print("[%s : %d] peSysEvt is %d\n",__FUNCTION__,__LINE__,*peSysEvt);
	if(*peSysEvt == DI_SYS_EVENT_OVER_TEMPERATURE)
	{
		s_pfncOverTempCallback();
	}
	return;
}

void	pal_sys_cbFanLock(void* pData)
{
	DI_SYS_EVENT *peSysEvt = NULL;

	peSysEvt = (DI_SYS_EVENT*)pData;

	HxLOG_Error("[MJAHN] [pal_sys_cbFanLock] +++ \n");

	if(s_pfncFanLockCallback == NULL)
	{
		HxLOG_Error("\n\n\t\t[%s : %d] Fanlock Callback is NULL!! \n\n",__FUNCTION__,__LINE__);
		return;
	}
	if(pData == NULL)
	{
		HxLOG_Error("\n\n\t\t[%s : %d] param is NULL!! \n\n",__FUNCTION__,__LINE__);
		return;
	}

	HxLOG_Print("[%s : %d] peSysEvt is %d\n",__FUNCTION__,__LINE__,*peSysEvt);
	if(*peSysEvt == DI_SYS_EVENT_FAN_LOCKED)
	{
		s_pfncFanLockCallback();
	}

	return;
}
#endif

/*******************************************************************/
/*********************** EXTERN API Functions *************************/
/*******************************************************************/

#include "pal_sys_eeprominit.h"

/* factory reset할 때 loader에서 보고 있는 app와의 공유값을 초기화 해 준다. */
HERROR	PAL_SYS_SetEeprom4LoaderFactoryReset(void)
{
	DI_ERR_CODE		 errError = DI_ERR_OK;

	/* panel status : HUINT32 x 1byte, default: DI_NVRAM_OSD_DISPLAY_E */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_OSD_DISPLAY, 0, &s_panelStatus, sizeof(DI_NVRAM_OSD_DISPLAY_E));

	/* panel display : HUINT32 x 1byte, default: DI_OTA_PANELDISPLAY_t */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_PANEL_DISPLAY, 0, (void *)&s_panelDisplay, sizeof(DI_NVRAM_FRONTPANEL_DISPLAY_E));

	/* panel display dim level : HUINT8 x 1byte, default: HUINT8 */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_PANEL_DIMMING, 0, (void *)&s_panelDimLevel, sizeof(HUINT8));

	/* ota control : HUINT32 x 1byte, default: DI_NVRAM_OTA_FLAG_E */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_OTA_FLAG1, 0, (void *)&s_OTA, sizeof(HUINT32));

	/* 639-2 language code : HUINT8 x 3byte, default: ENG */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_LANGUAGE, 0, (void *)s_language, sizeof(HUINT8) * LANG_639_CODE_MAX);

	/* scart output : HUINT32 x 1byte, default: DI_SCART_SETTING */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_SCART_RGB, 0, (void *)&s_scartRGB, sizeof(DI_NVRAM_SCART_SETTING_VALUE));

	/* HD output resolution : HUINT32 x 1byte, default: DI_VIDEO_HD_RESOLUTION */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_HD_RESOULTION, 0, (void *)&s_HDresolution, sizeof(DI_NVRAM_VIDEO_RESOLUTION_E));

	/* tuner id : HUINT8 x 1byte, default: Tuner Id */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_TUNER_ID1, 0, (void *)&s_tunerId, sizeof(HUINT8));

	/* external terrestrial antena power : HUINT8 x 1byte, default: External Antena Power for Ter, 0:Off/1:On */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_ANTENA_POWER1, 0, (void *)&s_antennaPwr, sizeof(HUINT8));

#if 0	// 추후에 로더에 무선랜 들어가면 살릴 것.
	/* network device name : HUINT8 x 1byte, default: ethernet device id 0 = eth0 */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_NET_DEVICE_ID, 0, (void *)&s_netDev, sizeof(HUINT8));
#endif

	/* network address mode : HUINT8 x 1byte, default: DHCP/Manual */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_NET_IP_CONFIG, 0, (void *)&s_netConf, sizeof(HUINT32));

	/* network address : HUINT8 x 16byte, default: Manual IP setting */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_NET_IP_SETTING, 0, (void *)s_netaddr, sizeof(HUINT8) * INET_ADDR_LENGTH * INET_ADDR_LENGTH);

	/* net portal address : HUINT8 x 4byte, default: IP address */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_NET_PORTAL_IP, 0, (void *)s_netPortalIp, sizeof(HUINT8) * INET_ADDR_LENGTH);

#if 0	// nagra spec 때문에 안된다고 함. 우 차장님..
	/* net portal address : HUINT8 x 44byte, default: IP address */
	errError |= DI_NVRAM_SetField (DI_NVRAM_FIELD_NET_DOWN_PATH, 0, (void *)s_netDownPath, sizeof(HUINT8) * INET_DOWN_PATH_MAX);
#endif
	if(errError != DI_ERR_OK)
	{
		fprintf(stderr, "assert!!, writing to shared field with loader is failed~~\n");

		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR PAL_SYS_GetSystemId(HUINT32 *pulSystemId)
{
	DI_ERR_CODE		 hErr;

	if (pulSystemId == NULL)
	{
		return ERR_FAIL;
	}

	*pulSystemId = 0x37680000;//0x809C7D00;//0x37680000;//0x35f10000;//0x38ac0000;//0x7a010000;//0x809C7D00;

	hErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_SYSTEM_ID, 0, (void *)pulSystemId, sizeof(HUINT32));

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

PAL_SYS_ERR_CODE_t PAL_SYS_GetSystemVersion(VersionType_t eSystemType, HUINT32 *pulSystemVer)
{
	float			 fReadVer;
	HUINT16			 usReadVar = 0;
	//HUINT32			 ulReadVar;
	DI_ERR_CODE		 hErr;

	HUINT8			aucVer[palSys_MAX_VER_LEN];

	if (pulSystemVer == NULL)
	{
		return PAL_SYS_ERR_FAIL;
	}

	switch (eSystemType)
	{
	case eVERSION_TYPE_NEXTOTA:
		hErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_APP_VERSION_NEXT, 0, (void *)aucVer, palSys_MAX_APP_VER_LEN);
		if (hErr == DI_ERR_OK)
		{
			palSys_GET_VER_VAL(pulSystemVer, aucVer, palSys_MAX_APP_VER_LEN);
			return PAL_SYS_ERR_OK;
		}
		else if (hErr == DI_ERR_CRC)
		{
			palSys_GET_VER_VAL(pulSystemVer, aucVer, palSys_MAX_APP_VER_LEN);
			return PAL_SYS_ERR_CRC;
		}
		break;

	case eVERSION_TYPE_LOADER:
		hErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_LOADER_VERSION, 0, (void *)aucVer, palSys_MAX_LOADER_VER_LEN);
		if (hErr == DI_ERR_OK)
		{
			palSys_GET_VER_VAL(pulSystemVer, aucVer, palSys_MAX_LOADER_VER_LEN);
			return PAL_SYS_ERR_OK;
		}
		else if (hErr == DI_ERR_CRC)
		{
			palSys_GET_VER_VAL(pulSystemVer, aucVer, palSys_MAX_LOADER_VER_LEN);
			return PAL_SYS_ERR_CRC;
		}

		break;

	case eVERSION_TYPE_BOOTLOADER:
		hErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_LOADER_VERSION, 0, (void *)&usReadVar, sizeof(HUINT16));
		if (hErr == DI_ERR_OK)
		{
			*pulSystemVer = (HUINT32)usReadVar;
			return PAL_SYS_ERR_OK;
		}

		break;

	case eVERSION_TYPE_ROOTFS:
		hErr = pal_sys_GetRootFsVer (&fReadVer);
		if (hErr == DI_ERR_OK)
		{
			*pulSystemVer = (HUINT32)(fReadVer * 100.0);
			return PAL_SYS_ERR_OK;
		}

		break;


	case eVERSION_TYPE_RESOURCE:
		hErr = pal_sys_GetResourceVer (&fReadVer);
		if (hErr == DI_ERR_OK)
		{
			*pulSystemVer = (HUINT32)(fReadVer * 100.0);
			return PAL_SYS_ERR_OK;
		}

		break;

	case eVERSION_TYPE_DB:
		hErr = pal_sys_GetDbVer (&fReadVer);
		if (hErr == DI_ERR_OK)
		{
			*pulSystemVer = (HUINT32)(fReadVer * 100.0);
			return PAL_SYS_ERR_OK;
		}

		break;

	case eVERSION_TYPE_UPGRADER:
		hErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RW, (HUINT8*)OTALDR_VERSION, 0, (void *)&aucVer, palSys_MAX_UPGRADER_VER_LEN);
		if(DI_ERR_OK == hErr)
		{
			HxLOG_Print("[OTALDR_VERSION] %c%c%c%c%c%c %d.%02d.%02d\n", aucVer[0], aucVer[1], aucVer[2], aucVer[3], aucVer[4], aucVer[5], aucVer[6], aucVer[7], aucVer[8]);
			palSys_GET_VER_VAL(pulSystemVer, aucVer, palSys_MAX_UPGRADER_VER_LEN);
			return PAL_SYS_ERR_OK;
		}
		else if (DI_ERR_CRC == hErr)
		{
			palSys_GET_VER_VAL(pulSystemVer, aucVer, palSys_MAX_UPGRADER_VER_LEN);
			return PAL_SYS_ERR_CRC;
		}
		else
		{
			HxLOG_Error("can't DI_NVRAM_Read ( OTALDR_VERSION )\n");
		}
		break;

#if defined(CONFIG_OTA_USE_PKG_VERSION)
	case eVERSION_TYPE_PACKAGE:
	case eVERSION_TYPE_APPL:
		{
		/* 	PKG_VERSION is modified by making OTA image
			덴마크 TDC라는 OP에 obama를 제외한 모든 source code를 공개하도록 되어있는데, 그들이 직접 OTA image를 만들고자 할때
			Obama를 수정할 수 없기 때문에 OTA image 만들때 버전 정보를 PKG_VERSION 이라는 이름으로 NVRAM에 저장하고,
			Obama에서는 이 값을 version 정보로 이용하기 위해 추가되었다.  */
			HUINT8 aucVer[4] = {0,};
			HUINT32 ulTmpVersion = 0;

			hErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RW, (HUINT8*)PKG_VERSION, 0, (void *)&aucVer, sizeof(HUINT32));
			if(DI_ERR_OK != hErr)
			{
				HxLOG_Error("Error, DI_NVRAM_Read\n");
				*pulSystemVer = (HUINT32)0;
			}
			else
			{
				ulTmpVersion = ((HUINT32)aucVer[0] << 24) & 0xff000000;
				ulTmpVersion = ulTmpVersion | (((HUINT32)aucVer[1] << 16) & 0x00ff0000);
				ulTmpVersion = ulTmpVersion | (((HUINT32)aucVer[2] << 8) & 0x0000ff00);
				ulTmpVersion = ulTmpVersion | ((HUINT32)aucVer[3] & 0x000000ff);

				*pulSystemVer = ulTmpVersion;
			}
			return PAL_SYS_ERR_OK;
		}
	case eVERSION_TYPE_APPL_UI:
#else
	case eVERSION_TYPE_APPL:
#endif
	{
		STATIC HUINT8 ucCheckAppVersion = 0;

		*pulSystemVer = (HUINT32)SYSTEM_APP_VERSION;

		if(ucCheckAppVersion == 0)
		{
			HUINT32 ulTmpVersion = 0;
			HUINT8 aucTmpVer[palSys_MAX_APP_VER_LEN];

			hErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_APP_VERSION, 0, (void *)&aucTmpVer, palSys_MAX_APP_VER_LEN);

			palSys_GET_VER_VAL(&ulTmpVersion, aucTmpVer, palSys_MAX_APP_VER_LEN);
			if ((hErr != DI_ERR_OK) || (ulTmpVersion !=  (HUINT32)SYSTEM_APP_VERSION))
			{
				ulTmpVersion = SYSTEM_APP_VERSION;

				HxSTD_MemCopy(aucTmpVer, SYSTEM_APP_VER_HEAD, palSys_MAX_VER_HEAD_LEN);
				aucTmpVer[palSys_MAX_VER_HEAD_LEN] = (HUINT8)(0x000000FF & (ulTmpVersion >> 16));
				aucTmpVer[palSys_MAX_VER_HEAD_LEN+1] = (HUINT8)(0x000000FF & (ulTmpVersion >> 8));
				aucTmpVer[palSys_MAX_VER_HEAD_LEN+2] = (HUINT8)(0x000000FF & ulTmpVersion);
				hErr = DI_NVRAM_SetField (DI_NVRAM_FIELD_APP_VERSION, 0, (void *)aucTmpVer, palSys_MAX_APP_VER_LEN);
				if(hErr != DI_ERR_OK)
				{
					HxLOG_Print("[%s:%d] Error, DI_NVRAM_SetField\n", __FUNCTION__, __LINE__);
				}
			}

			ucCheckAppVersion = 0xFF;
		}

		return PAL_SYS_ERR_OK;
	}

	default:
		break;
	}

	return PAL_SYS_ERR_FAIL;
}

HERROR PAL_SYS_SetSystemVersion(VersionType_t eSystemType, HUINT32 ulSystemVer)
{
	HUINT32			 ulWriteVar;
	DI_ERR_CODE		 hErr;
	HUINT8			 aucTmpVer[palSys_MAX_VER_LEN];

	switch (eSystemType)
	{
	case eVERSION_TYPE_NEXTOTA:
		HxSTD_MemCopy(aucTmpVer, SYSTEM_APP_VER_HEAD, palSys_MAX_VER_HEAD_LEN);
		ulWriteVar = ulSystemVer;
		aucTmpVer[palSys_MAX_VER_HEAD_LEN] = (HUINT8)(0x000000FF & (ulWriteVar >> 16));
		aucTmpVer[palSys_MAX_VER_HEAD_LEN+1] = (HUINT8)(0x000000FF & (ulWriteVar >> 8));
		aucTmpVer[palSys_MAX_VER_HEAD_LEN+2] = (HUINT8)(0x000000FF & ulWriteVar);
		hErr = DI_NVRAM_SetField (DI_NVRAM_FIELD_APP_VERSION_NEXT, 0, (void *)aucTmpVer, palSys_MAX_APP_VER_LEN);
		if(hErr != DI_ERR_OK)
		{
			HxLOG_Print("[%s:%d] Error, DI_NVRAM_SetField\n", __FUNCTION__, __LINE__);
		}
		else
		{
			return ERR_OK;
		}

	// Loader나 다른 Version은 APP에서 덮어 쓰면 안된다.

	default:
		break;
	}

	return ERR_FAIL;
}

#if defined (CONFIG_MW_CI_PLUS)
HERROR PAL_SYS_GetCiPlusCoreVersion(HUINT8 *pucString)
{
	if( pucString == NULL )
		return ERR_FAIL;

	MWC_UTIL_DvbStrcpy(pucString, (HUINT8*)SYSTEM_CIPLUS_VERSION);

	return ERR_OK;
}
#endif

HERROR PAL_SYS_MakeSystemVerString (VersionType_t eSystemType, HUINT32 ulSystemVer, HUINT8 *szSysVerStr)
{
	if (szSysVerStr == NULL)
	{
		return ERR_FAIL;
	}

	switch (eSystemType)
	{
	case eVERSION_TYPE_NEXTOTA:
		pal_sys_MakeVerText (NULL, ulSystemVer, szSysVerStr, eVersionByteType_Default);
		return ERR_OK;

	case eVERSION_TYPE_LOADER:
#if defined(CONFIG_MW_CAS_IRDETO)
		pal_sys_MakeVerText (palSys_LOADER_IRDTO_VER_HEAD, ulSystemVer, szSysVerStr, eVersionByteType_Irdeto);
#else
	{
		HUINT8 ucVersionHeader[palSys_MAX_VER_HEAD_LEN + 2]={0,};

#if defined(CONFIG_PROD_FVP4000T)
		HUINT8	ucTempVersionHeader[palSys_MAX_VER_HEAD_LEN + 2];
		HUINT32	i, j;

		pal_sys_GetVersionHeader(eSystemType, ucTempVersionHeader, palSys_MAX_VER_HEAD_LEN+2);
		// ctrl char는 display하지 않는다.
		for(i=0,j=0; i<HxSTD_StrLen(ucTempVersionHeader); i++)
		{
			if(ucTempVersionHeader[i] < 0x20)
			{
				continue;
			}
			else if(ucTempVersionHeader[i] > 0x7f)
			{
				continue;
			}
			else
			{
				ucVersionHeader[j] = ucTempVersionHeader[i];
				j++;
			}
		}
		pal_sys_MakeVerText (ucVersionHeader, ulSystemVer, szSysVerStr, eVersionByteType_2Bytes); 	// MODEL~ X.XX
#else
		pal_sys_GetVersionHeader(eSystemType, ucVersionHeader, palSys_MAX_VER_HEAD_LEN+2);
		pal_sys_MakeVerText (ucVersionHeader, ulSystemVer, szSysVerStr, eVersionByteType_2Bytes);	// MODEL~ X.XX
#endif
	}
#endif
		return ERR_OK;

	case eVERSION_TYPE_BOOTLOADER:
		pal_sys_MakeVerText (NULL, ulSystemVer, szSysVerStr, eVersionByteType_Default);
		return ERR_OK;

	case eVERSION_TYPE_APPL:
		pal_sys_MakeVerText (SYSTEM_APP_VER_HEAD, ulSystemVer, szSysVerStr, eVersionByteType_Default);
		return ERR_OK;

	case eVERSION_TYPE_UPGRADER:
	{
		HUINT8 ucVersionHeader[palSys_MAX_UPGRADER_VER_LEN]={0,};
		pal_sys_GetVersionHeader(eSystemType, ucVersionHeader, palSys_MAX_UPGRADER_VER_LEN);
		pal_sys_MakeVerText (ucVersionHeader, ulSystemVer, szSysVerStr, eVersionByteType_Default);
		return ERR_OK;
	}
#if defined(CONFIG_OTA_USE_PKG_VERSION)
	case eVERSION_TYPE_PACKAGE:
		pal_sys_MakeVerText (NULL, ulSystemVer, szSysVerStr, eVersionByteType_4Bytes);
		return ERR_OK;
#endif
	default:
		break;
	}

	return ERR_FAIL;
}

HERROR PAL_SYS_GetTotalSystemVersionString (HUINT8 *szVerStr)
{
	float			 fRootFsVer, fResourceVer, fDbVer;

	if (szVerStr == NULL)			{ return ERR_FAIL; }

	pal_sys_GetRootFsVer (&fRootFsVer);
	pal_sys_GetResourceVer (&fResourceVer);
	pal_sys_GetDbVer (&fDbVer);

	HxSTD_PrintToStrN (szVerStr, 48,  "%05.2f-%05.2f-%05.2f", fRootFsVer, fResourceVer, fDbVer);
	return ERR_OK;
}

HERROR PAL_SYS_GetKernelVersionString (HUINT8 *szVerStr)
{
	return pal_sys_GetKernelVerStr (szVerStr);
}

HERROR PAL_SYS_GetMicomVerString(HUINT8 *szVerStr)
{
	DI_ERR_CODE		errDiError = DI_ERR_ERROR;
	HUINT8 			aucMiComVersion[palSys_MAX_VER_LEN + 1];
	HUINT32			ulSystemVer = 0;
	HUINT8			ucVersionHeader[palSys_MAX_VER_HEAD_LEN + 2];
	HUINT8 			i;

	if(szVerStr == NULL)
	{
		return ERR_FAIL;
	}
	errDiError = DI_SYSTEM_GetSystemInfo(DI_SYS_MICOM_STRVER, aucMiComVersion, palSys_MAX_MICOM_VER_LEN);
	if(errDiError != DI_ERR_OK)
	{
		return ERR_FAIL;
	}
	HxLOG_Print("\nPAL_SYS_GetMicomVerString : %s  \n",aucMiComVersion);
	for(i = 0; i < palSys_MAX_MICOM_VER_LEN; i++)
		HxLOG_Print("0x%x ", aucMiComVersion[i]);
	HxLOG_Print("\n");

	HxSTD_MemSet(ucVersionHeader, 0, palSys_MAX_VER_HEAD_LEN + 2);

	HLIB_STD_StrUtf8NCpy(ucVersionHeader, aucMiComVersion, palSys_MAX_VER_HEAD_LEN + 1);

	palSys_GET_VER_VAL(&ulSystemVer, aucMiComVersion, palSys_MAX_MICOM_VER_LEN);
	pal_sys_MakeVerText (ucVersionHeader, ulSystemVer, szVerStr, eVersionByteType_Default);

	return ERR_OK;
}

HERROR PAL_SYS_GetChipId(HUINT32 *pulChipId )
{
	DI_ERR_CODE		errDiError = DI_ERR_OK;

	if(pulChipId == NULL)
	{
		return ERR_FAIL;
	}

	errDiError = DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_ID, (void *)pulChipId, sizeof(HUINT32));
	if(errDiError != DI_ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SYS_GetChipId_x64(HUINT32 *pulFrontId, HUINT32 *pulRearId)
{
	DI_ERR_CODE		errDiError = DI_ERR_OK;
	HUINT8			aucChipIdX64[8];

	if ((pulFrontId == NULL) || (pulRearId == NULL))
	{
		return ERR_FAIL;
	}

	errDiError = DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_ID_X64, (void *)aucChipIdX64, sizeof(aucChipIdX64));
	if(errDiError != DI_ERR_OK)
	{
		return ERR_FAIL;
	}

	*pulFrontId	= (aucChipIdX64[0] << 24) + (aucChipIdX64[1] << 16) + (aucChipIdX64[2] << 8) + aucChipIdX64[3];
	*pulRearId 	= (aucChipIdX64[4] << 24) + (aucChipIdX64[5] << 16) + (aucChipIdX64[6] << 8) + aucChipIdX64[7];

	return ERR_OK;
}


HERROR PAL_SYS_GetChipRevision(HUINT8 *pucChipRevision )
{
	DI_ERR_CODE		errDiError;
	//HUINT8			ucChiprevision;
	errDiError = DI_ERR_OK;

	if(pucChipRevision == NULL)
	{
		return ERR_FAIL;
	}

	errDiError = DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_REVISION, (void *)pucChipRevision, sizeof(HUINT8));
	if(errDiError != DI_ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SYS_GetChipCheckNumber(HUINT32 *pulChipCheckNumber )
{
	DI_ERR_CODE		errDiError = DI_ERR_OK;

	if(pulChipCheckNumber == NULL)
	{
		return ERR_FAIL;
	}

	errDiError = DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_CHECK_NO, (void *)pulChipCheckNumber, sizeof(HUINT32));
	if(errDiError != DI_ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SYS_GetSysHWRevision(HUINT32 *pulHWRevision )
{
	DI_ERR_CODE		errDiError = DI_ERR_OK;

	if(pulHWRevision == NULL)
	{
		return ERR_FAIL;
	}

	errDiError = DI_SYSTEM_GetSystemInfo(DI_SYS_HW_REVISION, (void *)pulHWRevision, sizeof(HUINT32));
	if(errDiError != DI_ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SYS_GetNandBadBlocks(HUINT32 *pulCntLdr, HUINT32 *pulCntUBI, HUINT32 *pulCntUBIFS)
{
	DI_ERR_CODE errDiError = DI_ERR_ERROR;

	*pulCntLdr = 0;
	*pulCntUBI = 0;
	*pulCntUBIFS = 0;

	errDiError = DI_NANDFLASH_GetBadblkCount(DI_NANDFLASH_PARTITION_BOOTLOADER, pulCntLdr);
	if(errDiError != DI_ERR_OK)
	{
		HxLOG_Error("PAL_SYS_GetNandBadBlocks : DI_NANDFLASH_PARTITION_BOOTLOADER \n");
		return ERR_FAIL;
	}

	errDiError = DI_NANDFLASH_GetBadblkCount(DI_NANDFLASH_PARTITION_UBI, pulCntUBI);
	if(errDiError != DI_ERR_OK)
	{
		HxLOG_Error("PAL_SYS_GetNandBadBlocks : DI_NANDFLASH_PARTITION_UBI \n");
		return ERR_FAIL;
	}

	errDiError = DI_NANDFLASH_GetBadblkCount(DI_NANDFLASH_PARTITION_UBIFS, pulCntUBIFS);
	if(errDiError != DI_ERR_OK)
	{
		HxLOG_Error("PAL_SYS_GetNandBadBlocks : DI_NANDFLASH_PARTITION_UBIFS \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR PAL_SYS_GetFusingInfoForConax(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT8 *pucRemux, HUINT8 *pucMarketId)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				errDiError;

	*pucSCS 	= 0;
	*pucJTAG 	= 0;
	*pucOTP		= 0;
	*pucCWE		= 0;
	*pucRemux	= 0;
	*pucMarketId	= 0;

	errDiError = DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));

	if ( errDiError != DI_ERR_OK )
		return ERR_FAIL;

	*pucSCS 	= stFusingInfo.ucSCS;
	*pucJTAG 	= stFusingInfo.ucJTAG;
	*pucOTP		= stFusingInfo.ucOTP;
	*pucCWE		= stFusingInfo.ucCWE;
	*pucRemux	= stFusingInfo.ucRemuxFusing;
	*pucMarketId = stFusingInfo.ucMarketID;

	return ERR_OK;
}
#endif

#ifdef CONFIG_MW_CAS_VIACCESS
#if 0
HERROR PAL_SYS_SetFusingInfo(HUINT8 ucCWE)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				errDiError;

	stFusingInfo.ucSCS 	= FALSE;
	stFusingInfo.ucJTAG 	= FALSE;
	stFusingInfo.ucOTP 	= FALSE;
	stFusingInfo.ucCWE 	= ucCWE;

	errDiError = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));

	if ( errDiError != DI_ERR_OK )
		return ERR_FAIL;

	return ERR_OK;
}
#endif

// pucPlain, pucEncrypted는 각각 16byte 이상의 array를 point하고 있다고 가정
// ST에서 buffer size를 128 byte 이상으로 하라고 해서 이렇게 한거니, 다른 chip에서는 거기 맞게 조정하시면 됨.
#if defined (CONFIG_MW_CAS_VIACCESS_PVR)
HERROR PAL_SYS_EncryptRecKey(HUINT8 *pucPlain, HUINT8 *pucEncrypted)
{
	DI_ERR_CODE	nErr;
	stCrypt_t 		stCryptInfo;
	HUINT8			aucSrc[128], aucDest[128];

	HxSTD_memset(aucSrc, 0, 128);
	HxSTD_memset(aucDest, 0, 128);

	HxSTD_memcpy(aucSrc, pucPlain, 16);

	stCryptInfo.etCryptMode = DI_CRYPT_AES;
	stCryptInfo.etCryptKey = DI_CRYPT_DEFAULT_SYSTEM_KEY;
	stCryptInfo.pKey = NULL;
	stCryptInfo.ulKeySize = 16;
	stCryptInfo.pSrcBuffer = aucSrc;
	stCryptInfo.pDstBuffer = aucDest;
	stCryptInfo.ulBuffSize = 128;

	nErr = DI_CRYPT_Encrypt(&stCryptInfo);

#if 0
{
	int i;

	VA_OS_Printf("PLAIN:");
	for (i = 0; i < 128; i++)
	{
		VA_OS_Printf("%02X ", aucSrc[i]);
	}

	VA_OS_Printf("\nENCRYPTED:");
	for (i = 0; i < 128; i++)
	{
		VA_OS_Printf("%02X ", aucDest[i]);
	}
	VA_OS_Printf("\n");
}
#endif

	if (nErr != DI_ERR_OK)
		return ERR_FAIL;

	HxSTD_memcpy(pucEncrypted, aucDest, 16);

	return ERR_OK;
}
#endif
#endif

HERROR PAL_SYS_Encrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucEncrypted, HUINT32 ulSize)
{
	HUINT8		*pucSrc, *pucDest;
	stCrypt_t 	stCryptInfo;
	DI_ERR_CODE	nErr;

	HxSTD_memset(&stCryptInfo, 0, sizeof(stCryptInfo));

	pucSrc = DI_CRYPT_MemAlloc(ulSize);
	if(pucSrc == NULL)
	{
		HxLOG_Print("[%s:%d] pucSrc NULL!!\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pucDest = DI_CRYPT_MemAlloc(ulSize);
	if(pucDest == NULL)
	{
		DI_CRYPT_MemFree(pucSrc);
		HxLOG_Print("[%s:%d] pucSrc NULL!!\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pucSrc, pucPlain, ulSize);

	stCryptInfo.etCryptMode = DI_CRYPT_TDES;
	stCryptInfo.etCryptData = (bBlock) ? DI_CRYPT_BLOCK : DI_CRYPT_STREAM;
	stCryptInfo.ulKeySize = 16;
	stCryptInfo.pSrcBuffer = pucSrc;
	stCryptInfo.pDstBuffer = pucDest;
	stCryptInfo.ulBuffSize = ulSize;

	if(pucKey == NULL)
	{
		stCryptInfo.etCryptKey = DI_CRYPT_DEFAULT_SYSTEM_KEY;
	}
	else
	{
		stCryptInfo.etCryptKey = DI_CRYPT_USER_KEY;
		stCryptInfo.pKey = pucKey;
	}

	nErr = DI_CRYPT_Encrypt(&stCryptInfo);

	if (nErr != DI_ERR_OK)
	{
		HxLOG_Print("[%s:%d] error (%d)\n", __FUNCTION__, __LINE__, nErr);
		DI_CRYPT_MemFree(pucSrc);
		DI_CRYPT_MemFree(pucDest);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pucEncrypted, pucDest, ulSize);

	DI_CRYPT_MemFree(pucSrc);
	DI_CRYPT_MemFree(pucDest);

	return ERR_OK;
}

HERROR PAL_SYS_Decrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucDecrypted, HUINT32 ulSize)
{
	HUINT8		*pucSrc, *pucDest;
	stCrypt_t 	stCryptInfo;
	DI_ERR_CODE	nErr;

	HxSTD_memset(&stCryptInfo, 0, sizeof(stCryptInfo));

	pucSrc = DI_CRYPT_MemAlloc(ulSize);
	if(pucSrc == NULL)
	{
		HxLOG_Print("[%s:%d] pucSrc NULL!!\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pucDest = DI_CRYPT_MemAlloc(ulSize);
	if(pucDest == NULL)
	{
		DI_CRYPT_MemFree(pucSrc);
		HxLOG_Print("[%s:%d] pucDest NULL!!\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pucSrc, pucPlain, ulSize);

	stCryptInfo.etCryptMode = pal_sys_GetEncrypteModePal2DI(PAL_SYS_GetPvrEncryptMode());
	stCryptInfo.etCryptData = (bBlock) ? DI_CRYPT_BLOCK : DI_CRYPT_STREAM;
	stCryptInfo.ulKeySize 	= PAL_SYS_GetEncryptionKeySize ();
	stCryptInfo.pSrcBuffer = pucSrc;
	stCryptInfo.pDstBuffer = pucDest;
	stCryptInfo.ulBuffSize = ulSize;

	if(pucKey == NULL)
	{
		stCryptInfo.etCryptKey = DI_CRYPT_DEFAULT_SYSTEM_KEY;
	}
	else
	{
		stCryptInfo.etCryptKey = DI_CRYPT_USER_KEY;
		stCryptInfo.pKey = pucKey;
	}

	nErr = DI_CRYPT_Decrypt(&stCryptInfo);

	if (nErr != DI_ERR_OK)
	{
		DI_CRYPT_MemFree(pucSrc);
		DI_CRYPT_MemFree(pucDest);
		HxLOG_Print("[%s:%d] error (%d)\n", __FUNCTION__, __LINE__, nErr);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pucDecrypted, pucDest, ulSize);

	DI_CRYPT_MemFree(pucSrc);
	DI_CRYPT_MemFree(pucDest);

	return ERR_OK;
}

HERROR PAL_SYS_EncryptData(PAL_Crypt_t *pstPalCrypt)
{
	DI_CRYPT_CryptMode      	eDiCryptMode;
	DI_CRYPT_CryptKeyType   	eDiCryptKeyType;
	HUINT8		            		*pucSrc = NULL, *pucDest = NULL;
	stCrypt_t 	            		stCrypt;
	DI_ERR_CODE	            	errDiError;

	if (pstPalCrypt == NULL)
	{
    	VK_Print("[%s:%d] Param. is NULL \n", __FUNCTION__, __LINE__);
   	 return ERR_FAIL;
	}

	switch(pstPalCrypt->etCryptMode)
	{
    	case PAL_CRYPT_AES      	: eDiCryptMode = DI_CRYPT_AES; break;
    	case PAL_CRYPT_DES      	: eDiCryptMode = DI_CRYPT_DES; break;
    	case PAL_CRYPT_TDES     	: eDiCryptMode = DI_CRYPT_TDES; break;
    	case PAL_CRYPT_AES_CTR		: eDiCryptMode = DI_CRYPT_AES_CTR; break;
    	case PAL_CRYPT_AES_CBC_MAC	: eDiCryptMode = DI_CRYPT_AES_CBC_MAC; break;
    	case PAL_CRYPT_TDES_DTCPIP	: eDiCryptMode = DI_CRYPT_TDES_DTCPIP; break;
    	default :
        		return ERR_FAIL;
	}

	switch(pstPalCrypt->etCryptKey)
	{
    	case PAL_CRYPT_DEFAULT_SYSTEM_KEY   	: eDiCryptKeyType = DI_CRYPT_DEFAULT_SYSTEM_KEY; break;
   		case PAL_CRYPT_INTERNAL_CAS_KEY     	: eDiCryptKeyType = DI_CRYPT_INTERNAL_CAS_KEY; break;
    	case PAL_CRYPT_USER_KEY             	: eDiCryptKeyType = DI_CRYPT_USER_KEY; break;
    	default :
       	return ERR_FAIL;
	}

	pucSrc = DI_CRYPT_MemAlloc(pstPalCrypt->ulBuffSize);
	if (pucSrc == NULL)
	{
		VK_Print("DI_CRYPT_MemAlloc fail  (%d)\n", pstPalCrypt->ulBuffSize);
		return ERR_FAIL;
	}

	pucDest = DI_CRYPT_MemAlloc(pstPalCrypt->ulBuffSize);
	if (pucDest == NULL)
	{
		VK_Print("DI_CRYPT_MemAlloc fail  (%d)\n", pstPalCrypt->ulBuffSize);
		return ERR_FAIL;
	}

	HxSTD_memset(&stCrypt, 0, sizeof(stCrypt_t));
	HxSTD_memcpy(pucSrc, pstPalCrypt->pSrcBuffer, pstPalCrypt->ulBuffSize);
	stCrypt.etCryptMode = eDiCryptMode;
	stCrypt.etCryptKey  = eDiCryptKeyType;
	stCrypt.ulKeySize   = pstPalCrypt->ulKeySize;
	stCrypt.pSrcBuffer  = pucSrc;
	stCrypt.pDstBuffer  = pucDest;
	stCrypt.ulBuffSize  = pstPalCrypt->ulBuffSize;
	stCrypt.pKey        = pstPalCrypt->pKey;

	errDiError = DI_CRYPT_Encrypt(&stCrypt);
	if (errDiError != DI_ERR_OK)
	{
		DI_CRYPT_MemFree((void *)pucSrc);
		DI_CRYPT_MemFree((void *)pucDest);
		VK_Print("DI_CRYPT_Encrypt fail  (0x%X)\n", errDiError);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstPalCrypt->pDstBuffer, pucDest, pstPalCrypt->ulBuffSize);

	DI_CRYPT_MemFree((void *)pucSrc);
	DI_CRYPT_MemFree((void *)pucDest);

	 return ERR_OK;
}

HERROR PAL_SYS_DecryptData(PAL_Crypt_t *pstPalCrypt)
{
	DI_CRYPT_CryptMode      	eDiCryptMode;
	DI_CRYPT_CryptKeyType   	eDiCryptKeyType;
	HUINT8		            		*pucSrc = NULL, *pucDest = NULL;
	stCrypt_t 	            		stCrypt;
	DI_ERR_CODE	            	errDiError;

	if (pstPalCrypt == NULL)
	{
	    	VK_Print("[%s:%d] Param. is NULL \n", __FUNCTION__, __LINE__);
	    	return ERR_FAIL;
	}

	switch(pstPalCrypt->etCryptMode)
	{
    	case PAL_CRYPT_AES      	: eDiCryptMode = DI_CRYPT_AES; break;
    	case PAL_CRYPT_DES      	: eDiCryptMode = DI_CRYPT_DES; break;
    	case PAL_CRYPT_TDES     	: eDiCryptMode = DI_CRYPT_TDES; break;
    	case PAL_CRYPT_AES_CTR		: eDiCryptMode = DI_CRYPT_AES_CTR; break;
    	case PAL_CRYPT_AES_CBC_MAC	: eDiCryptMode = DI_CRYPT_AES_CBC_MAC; break;
    	case PAL_CRYPT_TDES_DTCPIP	: eDiCryptMode = DI_CRYPT_TDES_DTCPIP; break;
    	default :
        return ERR_FAIL;
	}

	switch(pstPalCrypt->etCryptKey)
	{
	   	case PAL_CRYPT_DEFAULT_SYSTEM_KEY   	: eDiCryptKeyType = DI_CRYPT_DEFAULT_SYSTEM_KEY; break;
    	case PAL_CRYPT_INTERNAL_CAS_KEY     	: eDiCryptKeyType = DI_CRYPT_INTERNAL_CAS_KEY; break;
    	case PAL_CRYPT_USER_KEY             	: eDiCryptKeyType = DI_CRYPT_USER_KEY; break;
    	default :
	 	return ERR_FAIL;
	}

	pucSrc = DI_CRYPT_MemAlloc(pstPalCrypt->ulBuffSize);
	if (pucSrc == NULL)
	{
		VK_Print("DI_CRYPT_MemAlloc fail  (%d)\n", pstPalCrypt->ulBuffSize);
		return ERR_FAIL;
	}

	pucDest = DI_CRYPT_MemAlloc(pstPalCrypt->ulBuffSize);
	if (pucDest == NULL)
	{
		VK_Print("DI_CRYPT_MemAlloc fail  (%d)\n", pstPalCrypt->ulBuffSize);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pucSrc, pstPalCrypt->pSrcBuffer, pstPalCrypt->ulBuffSize);

	HxSTD_memset(&stCrypt, 0, sizeof(stCrypt_t));
	stCrypt.etCryptMode = eDiCryptMode;
	stCrypt.etCryptKey  = eDiCryptKeyType;
	stCrypt.ulKeySize   = pstPalCrypt->ulKeySize;
	stCrypt.pSrcBuffer  = pucSrc;
	stCrypt.pDstBuffer  = pucDest;
	stCrypt.ulBuffSize  = pstPalCrypt->ulBuffSize;
	stCrypt.pKey        = pstPalCrypt->pKey;

	errDiError = DI_CRYPT_Decrypt(&stCrypt);
	if (errDiError != DI_ERR_OK)
	{
		DI_CRYPT_MemFree((void *)pucSrc);
		DI_CRYPT_MemFree((void *)pucDest);
		VK_Print("DI_CRYPT_Decrypt fail  (0x%X)\n", errDiError);
		return ERR_FAIL;
	}

    	HxSTD_memcpy(pstPalCrypt->pDstBuffer, pucDest, pstPalCrypt->ulBuffSize);

	DI_CRYPT_MemFree((void *)pucSrc);
	DI_CRYPT_MemFree((void *)pucDest);

   	return ERR_OK;
}

#if 0
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
HERROR PAL_SYS_SetSecureBootFusingInfo(HUINT8 ucSB)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				errDiError;

	VK_memset32(&stFusingInfo, 0, sizeof(DI_SYS_FUSING_INFO_t));

	stFusingInfo.ucSCS 	= ucSB;
	stFusingInfo.ucJTAG	= FALSE;
	stFusingInfo.ucOTP 	= FALSE;
	stFusingInfo.ucCWE 	= FALSE;

	stFusingInfo.ulParms[0]	= 0;
	stFusingInfo.ulParms[1]	= 0;

	errDiError = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));

	if ( errDiError != DI_ERR_OK )
		return ERR_FAIL;

	return ERR_OK;
}

HERROR PAL_SYS_SetSecureCWFusingInfo(HUINT8 ucSC)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				errDiError;

	VK_memset32(&stFusingInfo, 0, sizeof(DI_SYS_FUSING_INFO_t));

	stFusingInfo.ucSCS 	= FALSE;
	stFusingInfo.ucJTAG	= FALSE;
	stFusingInfo.ucOTP 	= FALSE;
	stFusingInfo.ucCWE 	= ucSC;

	errDiError = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));

	if ( errDiError != DI_ERR_OK )
		return ERR_FAIL;

	return ERR_OK;
}

HERROR PAL_SYS_SetJtagFusingInfo(HUINT8 ucJtag)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				errDiError;

	VK_memset32(&stFusingInfo, 0, sizeof(DI_SYS_FUSING_INFO_t));

	stFusingInfo.ucSCS 	= FALSE;
	stFusingInfo.ucJTAG	= ucJtag;
	stFusingInfo.ucOTP 	= FALSE;
	stFusingInfo.ucCWE 	= FALSE;

	errDiError = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));

	if ( errDiError != DI_ERR_OK )
		return ERR_FAIL;

	return ERR_OK;
}
#endif
#endif

HERROR PAL_SYS_GetOtaFlag(SWUP_InfoSlot_e eSlot, SWUP_Flag_e *peOtaFlag)
{
	DI_NVRAM_OTA_FLAG_E		eOtaStart;
	DI_ERR_CODE				hErr;
	DI_NVRAM_FIELD_T		eNvramField;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	if (peOtaFlag == NULL)				{ return ERR_FAIL; }

	eNvramField = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_OTA_FLAG1 + eSlot);
	hErr = DI_NVRAM_GetField (eNvramField, 0, (void *)&eOtaStart, sizeof(DI_NVRAM_OTA_FLAG_E) );
	if (hErr != ERR_OK)					{ return ERR_FAIL; }

	switch (eOtaStart)
	{
	case DI_NVRAM_OTA_FLAG_NO_OTA:
		*peOtaFlag = eSWUP_FLAG_NO_INFO;
		break;

	case DI_NVRAM_OTA_FLAG_DETECTED:
		*peOtaFlag = eSWUP_FLAG_INFO_DETECTED;
		break;

	default:
		*peOtaFlag = eSWUP_FLAG_NO_INFO;
		break;
	}

	return ERR_OK;
}

HERROR PAL_SYS_SetOtaFlag(SWUP_InfoSlot_e eSlot, SWUP_Flag_e eOtaFlag)
{
	DI_NVRAM_OTA_FLAG_E		eOtaStart;
	DI_ERR_CODE				hErr;
	DI_NVRAM_FIELD_T		eNvramField;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	switch (eOtaFlag)
	{
	case eSWUP_FLAG_NO_INFO:
		eOtaStart = DI_NVRAM_OTA_FLAG_NO_OTA;
		break;

	case eSWUP_FLAG_INFO_DETECTED:
		eOtaStart = DI_NVRAM_OTA_FLAG_DETECTED;
		break;

	default:
		return ERR_FAIL;
	}

	eNvramField = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_OTA_FLAG1 + eSlot);
	hErr = DI_NVRAM_SetField (eNvramField, 0, (void *)&eOtaStart, sizeof(DI_NVRAM_OTA_FLAG_E));
	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_GetOtaType(SWUP_InfoSlot_e eSlot, SWUP_DownloadType_e *peOtaType)
{
	DI_NVRAM_OTA_TYPE_E	 eDiOtaType;
	HERROR				 hErr;
	DI_NVRAM_FIELD_T	 eNvramField;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	if (peOtaType == NULL)				{ return ERR_FAIL; }

	eNvramField = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_OTA_TYPE1 + eSlot);
	hErr = DI_NVRAM_GetField (eNvramField, 0, (void *)&eDiOtaType, sizeof(DI_NVRAM_OTA_TYPE_E));
	if (hErr != ERR_OK)					{ return ERR_FAIL; }

	switch (eDiOtaType)
	{
	case DI_NVRAM_OTA_SSU:
		*peOtaType = eSWUP_DOWNLOAD_DVBSSU;
		break;

	case DI_NVRAM_OTA_DTG:
		*peOtaType = eSWUP_DOWNLOAD_DTG;
		break;

	case DI_NVRAM_OTA_HUMAX :
		*peOtaType = eSWUP_DOWNLOAD_HUMAXTYPE;
		break;

	case DI_NVRAM_OTA_FILE:
		*peOtaType = eSWUP_DOWNLOAD_FILE;
		break;

	case DI_NVRAM_OTA_SKYD :
		*peOtaType = eSWUP_DOWNLOAD_SKYD;
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SYS_SetOtaType(SWUP_InfoSlot_e eSlot, SWUP_DownloadType_e eOtaType)
{
	DI_NVRAM_OTA_TYPE_E	 eDiOtaType;
	HERROR				 hErr;
	DI_NVRAM_FIELD_T	 eNvramField;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	switch (eOtaType)
	{
	case eSWUP_DOWNLOAD_DVBSSU:
		eDiOtaType = DI_NVRAM_OTA_SSU;
		break;

	case eSWUP_DOWNLOAD_DTG:
		eDiOtaType = DI_NVRAM_OTA_DTG;
		break;

	case eSWUP_DOWNLOAD_HUMAXTYPE:
		eDiOtaType = DI_NVRAM_OTA_HUMAX;
		break;

	case eSWUP_DOWNLOAD_FILE:
		eDiOtaType = DI_NVRAM_OTA_FILE;
		break;

	case eSWUP_DOWNLOAD_SKYD:
		eDiOtaType = DI_NVRAM_OTA_SKYD;
		break;

	default:
		return ERR_FAIL;
	}

	eNvramField = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_OTA_TYPE1 + eSlot);
	hErr = DI_NVRAM_SetField (eNvramField, 0, (void *)&eDiOtaType, sizeof(DI_NVRAM_OTA_TYPE_E));
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	PAL_SYS_SetChannelType(SWUP_InfoSlot_e eSlot, SWUP_ChannelType_e eChType)
{
	DI_NVRAM_OTA_CHANNEL_E	 eDiChType;
	HERROR				 	hErr;
	DI_NVRAM_FIELD_T	 	eNvramField;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	switch (eChType)
	{
	case eSWUP_CHANNEL_SAT:		eDiChType = DI_NVRAM_OTA_CHANNEL_SAT;		break;
	case eSWUP_CHANNEL_CAB:		eDiChType = DI_NVRAM_OTA_CHANNEL_CAB;		break;
	case eSWUP_CHANNEL_TER:		eDiChType = DI_NVRAM_OTA_CHANNEL_TER;		break;
	case eSWUP_CHANNEL_FLASH:	eDiChType = DI_NVRAM_OTA_CHANNEL_FLASH;		break;
	case eSWUP_CHANNEL_FTP:		eDiChType = DI_NVRAM_OTA_CHANNEL_FTP;		break;

	default:
		return ERR_FAIL;
	}

	eNvramField = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_CH_TYPE1 + eSlot);
	hErr = DI_NVRAM_SetField (eNvramField, 0, (void *)&eDiChType, sizeof(DI_NVRAM_OTA_CHANNEL_E));
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SYS_SetOtaPid(SWUP_InfoSlot_e eSlot, HUINT16 usPid)
{
	DI_ERR_CODE			 hErr;
	DI_NVRAM_FIELD_T	 eNvramField;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	eNvramField = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_ES_PID1 + eSlot);
	hErr = DI_NVRAM_SetField(eNvramField, 0, (void *)&usPid, sizeof(HUINT16));
	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetOtaTuningInfo(SWUP_InfoSlot_e eSlot, DxDeliveryType_e eDeliType, void *pvDdTuningInfo)
{
	HUINT8					 ucAntPower;
	HUINT32					 ulValue;
	DI_NVRAM_OTA_CHANNEL_E	 eDiChType;
	HERROR					 hErr;
	DI_NVRAM_FIELD_T		 eNvramFieldChType;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	eNvramFieldChType = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_CH_TYPE1 + eSlot);

	switch (eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		eDiChType = DI_NVRAM_OTA_CHANNEL_SAT;
		hErr  = DI_NVRAM_SetField (eNvramFieldChType, 0, (void *)&eDiChType, sizeof(DI_NVRAM_OTA_CHANNEL_E));
		hErr |= DI_NVRAM_SetField (DI_NVRAM_FIELD_CH_INFO_S, 0, pvDdTuningInfo, sizeof(CH_SAT_TuneParam_t) );
		break;

	case eDxDELIVERY_TYPE_TER:
		{
			DI_NVRAM_FIELD_T		 eNvramFieldAntPower;

			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_TER_SRCH_POWER, &ulValue, 0);
			if(hErr != ERR_OK)
			{
				HxLOG_Print("\n\t[eDB_PARAM_ITEM_TER_SRCH_POWER] Read Error from Setup DB (0x%x)\n\n",hErr);
				ulValue = 0;
			}

			ucAntPower = (HUINT8)ulValue;
			eNvramFieldAntPower = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_ANTENA_POWER1 + eSlot);
			hErr  = DI_NVRAM_SetField (eNvramFieldAntPower, 0, (void *)&ucAntPower, sizeof(HUINT8));

			eDiChType = DI_NVRAM_OTA_CHANNEL_TER;
			hErr |= DI_NVRAM_SetField (eNvramFieldChType, 0, (void *)&eDiChType, sizeof(DI_NVRAM_OTA_CHANNEL_E));
#if 0 // For debugging.
			{
				CH_TER_TuneParam_t *pstTerTunInfo = pvDdTuningInfo;

				HxLOG_Print("======================================\n\t OTA Param to EEPROM in PAL_SYS_SetOtaTuningInfo()\n");
				HxLOG_Print("\tFreq:0x%X\n",pstTerTunInfo->ulFrequency);
				HxLOG_Print("\tBandWidth:0x%X\n",pstTerTunInfo->etBandWidth);
				HxLOG_Print("\tModulation:0x%X\n",pstTerTunInfo->etModulation);
				HxLOG_Print("\tHierarchy:0x%X\n",pstTerTunInfo->etHierarchy);
				HxLOG_Print("\tCodeRate:0x%X\n",pstTerTunInfo->etCodeRate);
				HxLOG_Print("\tTransMode:0x%X\n",pstTerTunInfo->etTransMode);
				HxLOG_Print("\tGuardInterval:0x%X\n",pstTerTunInfo->etGuardInterval);
				HxLOG_Print("\tStream:0x%X\n",pstTerTunInfo->etStream);
				HxLOG_Print("\tOffset:0x%X\n",pstTerTunInfo->etOffset);
				HxLOG_Print("\tSystem:0x%X\n",pstTerTunInfo->etTerSystem);
				HxLOG_Print("\t\tPlpId:0x%X\n",pstTerTunInfo->stT2Param.ulPlpId);
				HxLOG_Print("\t\tPreambleFormat:0x%X\n",pstTerTunInfo->stT2Param.etPreambleFormat);
				HxLOG_Print("\t\tPreambleMixed:0x%X\n",pstTerTunInfo->stT2Param.etPreambleMixed);
				HxLOG_Print("\t\tPilotPattern:0x%X\n",pstTerTunInfo->stT2Param.etPilotPattern);
				HxLOG_Print("\t\tExtendedCarrier:0x%X\n",pstTerTunInfo->stT2Param.etExtendedCarrier);
				HxLOG_Print("\t\tPaprMode:0x%X\n",pstTerTunInfo->stT2Param.etPaprMode);
				HxLOG_Print("\t\tPlpNumBlocks:0x%X\n",pstTerTunInfo->stT2Param.ulPlpNumBlocks);
				HxLOG_Print("======================================\n");
			}
#endif
			hErr |= DI_NVRAM_SetField (DI_NVRAM_FIELD_CH_INFO_T, 0, pvDdTuningInfo, sizeof(CH_TER_TuneParam_t));
		}
		break;

	case eDxDELIVERY_TYPE_CAB:
		eDiChType = DI_NVRAM_OTA_CHANNEL_CAB;
		hErr  = DI_NVRAM_SetField (eNvramFieldChType, 0, (void *)&eDiChType, sizeof(DI_NVRAM_OTA_CHANNEL_E));
		hErr |= DI_NVRAM_SetField (DI_NVRAM_FIELD_CH_INFO_C, 0, pvDdTuningInfo, sizeof(CH_CAB_TuneParam_t));
		break;

	default:
		return ERR_FAIL;
	}

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetSwUpdateTuningInfoSat (SWUP_InfoSlot_e			eSlot,
										HUINT32 				ulFrequency,
										HUINT32 				ulSymbolRate,
										DxSat_CodeRate_e			fecCodeRate,
										DxSat_Polarization_e		polarization,
										DxSat_TransportSpec_e 	transSpec,
										DxSat_PskModulation_e 	pskMod,
										DxSat_Pilot_e 			pilot,
										DxAntennaType_e		antennaType,
										/* LNB && Common */
										HUINT32 				ulLnbFreq,
										DxSAT_LnbVoltage_e		lnbVoltage,
										HBOOL					b22kTone,				/**< 22KHz tone ON/OFF */
										/* DISQEC */
										DxSAT_DiseqcVer_e 		diseqcVersion,			/**< Diseqc Ver. 1.0, 1.1, 1.2, 1.3, 2.0 */
										DxSAT_DiseqcInput_e		diseqcInput,			/**< Diseqc A, B, C, D */
										DxSAT_ToneBurst_e 		toneBurst,				/**< NONE, A, B */
										HUINT32 				ucMotorPosition,		/**< for DiSEqC 1.2 */
										HINT32					sSatLonggitude, 		/**< for DiSEqC 1.3 */
										/* SCD */
										DxSAT_ScdInput_e			scdInput,				/**< SCD diseqc input A, B */
										HUINT32 				ulScdUserBandNum,		/**< Tuner 0용 SCD user band number. 설정에서 읽어서 넣어라. */
										HUINT32 				ulScdUserBandFreq,		/**< Tuner 0용 SCD user band frequency. 설정에서 읽어서 넣어라. */
										HUINT32 				ulScdUserBandNum1,		/**< Tuner 1용 SCD user band number. 설정에서 읽어서 넣어라. */
										HUINT32 				ulScdUserBandFreq1  	/**< Tuner 1용 SCD user band frequency. 설정에서 읽어서 넣어라. */
										)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	//HUINT8					 ucAntPower;
	//HINT32					 ulValue;
	DI_NVRAM_OTA_CHANNEL_E	 eDiChType;
	HERROR					 hErr;
	DI_NVRAM_FIELD_T		 eNvramFieldChType;
	HUINT32					 ulDiDataSize;
	void					*pvDiParam;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	ulDiDataSize = 0;
	pvDiParam = NULL;

	switch(antennaType)
	{
		case eDxANT_TYPE_LNB_ONLY:
			hErr = PAL_CH_LNB_MakeSatTuneParam(ulFrequency, ulSymbolRate, fecCodeRate, polarization,
												transSpec, pskMod, pilot,
												ulLnbFreq, lnbVoltage,
												&ulDiDataSize, &pvDiParam);
			if (hErr)
				HxLOG_Error("PAL_CH_LNB_MakeSatTuneParam ret:0x%X\n",hErr);
			break;

		case eDxANT_TYPE_DISEQC:
			hErr = PAL_CH_DISEQC_MakeSatTuneParam(ulFrequency, ulSymbolRate, fecCodeRate, polarization,
												transSpec, pskMod, pilot,
												ulLnbFreq, lnbVoltage, b22kTone, diseqcVersion, diseqcInput,
												toneBurst, ucMotorPosition, sSatLonggitude,
												&ulDiDataSize, &pvDiParam);
			if (hErr)
				HxLOG_Error("PAL_CH_DISEQC_MakeSatTuneParam ret:0x%X\n",hErr);
			break;

		case eDxANT_TYPE_SCD:
			hErr = PAL_CH_SCD_MakeSatTuneParam(ulFrequency, ulSymbolRate, fecCodeRate, polarization,
												transSpec, pskMod, pilot,
												ulLnbFreq, lnbVoltage, b22kTone, scdInput,
												ulScdUserBandNum, ulScdUserBandFreq,
												&ulDiDataSize, &pvDiParam);
			if (hErr)
				HxLOG_Error("PAL_CH_SCD_MakeSatTuneParam ret:0x%X\n",hErr);
			break;

		case eDxANT_TYPE_SMATV:
			hErr = PAL_CH_SMATV_MakeSatTuneParam(ulFrequency, ulSymbolRate, fecCodeRate, polarization,
												transSpec, pskMod, pilot,
												&ulDiDataSize, &pvDiParam);
			if (hErr)
				HxLOG_Error("PAL_CH_SMATV_MakeSatTuneParam ret:0x%X\n",hErr);
			break;

		default:
			HxLOG_Error("pstChTuneParam->sat.antennaType is unknown:0x%X\n", antennaType);
			hErr = ERR_FAIL;
			break;
	}

	if ((ERR_OK == hErr) && (pvDiParam != NULL))
	{
		eNvramFieldChType = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_CH_TYPE1 + eSlot);
		eDiChType = DI_NVRAM_OTA_CHANNEL_SAT;
		hErr  = DI_NVRAM_SetField (eNvramFieldChType, 0, (void *)&eDiChType, sizeof(DI_NVRAM_OTA_CHANNEL_E));
		hErr |= DI_NVRAM_SetField (DI_NVRAM_FIELD_CH_INFO_S, 0, pvDiParam, sizeof(CH_SAT_TuneParam_t) );
	}
	else
		hErr = ERR_FAIL;

	if (pvDiParam)
		OxMW_Free(pvDiParam);

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

HERROR PAL_SYS_SetSwUpdateTuningInfoTer (SWUP_InfoSlot_e			eSlot,
										HUINT32 				ulFrequency,
										DxTer_BandWidth_e 		bandWidth,
										DxTer_Constellation_e 	constellation,
										DxTer_Hierachy_e			hierachy,
										DxTer_CodeRate_e			codeRate,
										DxTer_TransMode_e 		transMode,
										DxTer_GuardInterval_e 	guardInterval,
										DxTer_Stream_e			stream,
										DxTer_Offset_e			offset,
										DxTer_System_e			system,
										HUINT32 				plpId,
										DxTer_PreambleFormat_e	preambleFormat,
										DxTer_Mixed_e 			mixed,
										DxTer_Pilot_Pattern_e 	pilotPattern,
										DxTer_ExtendedCarrier_e	extenedCarrier,
										DxTer_PAPR_Reduction_e	pAPRreduction,
										HUINT32 				numPlpBlocks)
{
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	HUINT8					 ucAntPower;
	HINT32					 ulValue;
	DI_NVRAM_OTA_CHANNEL_E	 eDiChType;
	HERROR					 hErr;
	DI_NVRAM_FIELD_T		 eNvramFieldChType;
	HUINT32					 ulDiDataSize;
	void					*pvDiParam;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	ulDiDataSize = 0;
	pvDiParam = NULL;

	hErr = PAL_CH_TER_MakeTerTuneParam(ulFrequency, bandWidth, constellation, hierachy,
										codeRate, transMode, guardInterval,
										stream,
										offset, system, plpId, preambleFormat,
										mixed, pilotPattern, extenedCarrier, pAPRreduction, numPlpBlocks,
										&ulDiDataSize, &pvDiParam);
	if (ERR_OK == hErr)
	{
		DI_NVRAM_FIELD_T		 eNvramFieldAntPower;

		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_TER_SRCH_POWER, &ulValue, 0);
		if(hErr != ERR_OK)
		{
			HxLOG_Print("\n\t[eDB_PARAM_ITEM_TER_SRCH_POWER] Read Error from Setup DB (0x%x)\n\n",hErr);
			ulValue = 0;
		}

		ucAntPower = (HUINT8)ulValue;
		eNvramFieldAntPower = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_ANTENA_POWER1 + eSlot);
		hErr  = DI_NVRAM_SetField (eNvramFieldAntPower, 0, (void *)&ucAntPower, sizeof(HUINT8));

		eDiChType = DI_NVRAM_OTA_CHANNEL_TER;
		eNvramFieldChType = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_CH_TYPE1 + eSlot);
		hErr |= DI_NVRAM_SetField (eNvramFieldChType, 0, (void *)&eDiChType, sizeof(DI_NVRAM_OTA_CHANNEL_E));
#if 0 // For debugging.
		{
			CH_TER_TuneParam_t *pstTerTunInfo = pvDiParam;

			HxLOG_Print("======================================\n\t OTA Param to EEPROM in PAL_SYS_SetOtaTuningInfo()\n");
			HxLOG_Print("\tFreq:0x%X\n",pstTerTunInfo->ulFrequency);
			HxLOG_Print("\tBandWidth:0x%X\n",pstTerTunInfo->etBandWidth);
			HxLOG_Print("\tModulation:0x%X\n",pstTerTunInfo->etModulation);
			HxLOG_Print("\tHierarchy:0x%X\n",pstTerTunInfo->etHierarchy);
			HxLOG_Print("\tCodeRate:0x%X\n",pstTerTunInfo->etCodeRate);
			HxLOG_Print("\tTransMode:0x%X\n",pstTerTunInfo->etTransMode);
			HxLOG_Print("\tGuardInterval:0x%X\n",pstTerTunInfo->etGuardInterval);
			HxLOG_Print("\tStream:0x%X\n",pstTerTunInfo->etStream);
			HxLOG_Print("\tOffset:0x%X\n",pstTerTunInfo->etOffset);
			HxLOG_Print("\tSystem:0x%X\n",pstTerTunInfo->etTerSystem);
			HxLOG_Print("\t\tPlpId:0x%X\n",pstTerTunInfo->stT2Param.ulPlpId);
			HxLOG_Print("\t\tPreambleFormat:0x%X\n",pstTerTunInfo->stT2Param.etPreambleFormat);
			HxLOG_Print("\t\tPreambleMixed:0x%X\n",pstTerTunInfo->stT2Param.etPreambleMixed);
			HxLOG_Print("\t\tPilotPattern:0x%X\n",pstTerTunInfo->stT2Param.etPilotPattern);
			HxLOG_Print("\t\tExtendedCarrier:0x%X\n",pstTerTunInfo->stT2Param.etExtendedCarrier);
			HxLOG_Print("\t\tPaprMode:0x%X\n",pstTerTunInfo->stT2Param.etPaprMode);
			HxLOG_Print("\t\tPlpNumBlocks:0x%X\n",pstTerTunInfo->stT2Param.ulPlpNumBlocks);
			HxLOG_Print("======================================\n");
		}
#endif
		hErr |= DI_NVRAM_SetField (DI_NVRAM_FIELD_CH_INFO_T, 0, pvDiParam, sizeof(CH_TER_TuneParam_t));
	}

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

HERROR PAL_SYS_SetSwUpdateTuningInfoCab (SWUP_InfoSlot_e			eSlot,
										HUINT32 				ulFrequency,
										HUINT32 				ulSymbolRate,
										DxCab_Constellation_e 	constellation,
										DxCab_Spectrum_e			spectrum)
{
#if defined(CONFIG_MW_CH_CABLE)
	DI_NVRAM_OTA_CHANNEL_E	 eDiChType;
	HERROR					 hErr;
	DI_NVRAM_FIELD_T		 eNvramFieldChType;
	HUINT32					 ulDiDataSize;
	void					*pvDiParam;
	CH_CAB_TuneParam_t	*pstTuneParam;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	ulDiDataSize = 0;
	pvDiParam = NULL;

	hErr = PAL_CH_CAB_MakeCabTuneParam (ulFrequency, ulSymbolRate, constellation, spectrum,
										&ulDiDataSize, &pvDiParam);
	if (ERR_OK == hErr)
	{
#if defined(CONFIG_OP_TDC)
		pstTuneParam = (CH_CAB_TuneParam_t*)pvDiParam;
		HAPI_GetRegistryInt(DxDB_NETWORK_ID_FOR_NETWORK_SEARCH, &pstTuneParam->nNetId);
#endif
		eNvramFieldChType = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_CH_TYPE1 + eSlot);
		eDiChType = DI_NVRAM_OTA_CHANNEL_CAB;
		hErr  = DI_NVRAM_SetField (eNvramFieldChType, 0, (void *)&eDiChType, sizeof(DI_NVRAM_OTA_CHANNEL_E));
		hErr |= DI_NVRAM_SetField (DI_NVRAM_FIELD_CH_INFO_C, 0, pvDiParam, sizeof(CH_CAB_TuneParam_t));
	}

	if (pvDiParam)
	{
		OxMW_Free(pvDiParam);
		pvDiParam = NULL;
	}

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

HERROR PAL_SYS_GetSystemUpdateDate( HUINT8 *pucString )
{
	if( pucString == NULL )
		return ERR_FAIL;

	MWC_UTIL_DvbStrcpy(pucString, (HUINT8*)SYSTEM_UPDATE_DATE);

	return ERR_OK;
}

HERROR PAL_SYS_GetSystemUpdatedList( HUINT8 *pucString )
{
	if( pucString == NULL )
		return ERR_FAIL;

	MWC_UTIL_DvbStrcpy(pucString, (HUINT8*)SYSTEM_UPDATE_DATE);

	return ERR_OK;
}

HERROR PAL_SYS_GetHwRevision( HUINT8 *pucString )
{
	if( pucString == NULL )
		return ERR_FAIL;

#if defined(SYSTEM_HW_VERSION)
	MWC_UTIL_DvbStrcpy(pucString, (HUINT8*)SYSTEM_HW_VERSION);
#else
	MWC_UTIL_DvbStrcpy(pucString, (HUINT8*)"N/A");
#endif
	return ERR_OK;
}

HERROR PAL_SYS_SetLanguage( HUINT8 *pucLangCode )
{
	DI_ERR_CODE		 hErr=0;

	hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_LANGUAGE, 0, (void *)pucLangCode, 3);

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetScartRGB(DxScartFormat_b eScartRGB)
{
	DI_ERR_CODE		 hErr=0;
	DI_NVRAM_SCART_SETTING_VALUE	tvoutScartSetting;

	switch(eScartRGB)
	{
	case eDxSCART_FORMAT_CVBS:
		tvoutScartSetting = DI_NVRAM_SCART_OUTPUT_CVBS;
		break;

	case eDxSCART_FORMAT_RGB:
		tvoutScartSetting = DI_NVRAM_SCART_OUTPUT_RGB;
		break;

	case eDxSCART_FORMAT_SVIDEO:
		tvoutScartSetting = DI_NVRAM_SCART_OUTPUT_SVIDEO;
		break;

	default:
		return ERR_FAIL;
	}

	hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_SCART_RGB, 0, (void *)&tvoutScartSetting, sizeof(DI_NVRAM_SCART_SETTING_VALUE));

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetResolution(DxResolutionStatus_b eResolution)
{
	DI_ERR_CODE		 hErr = DI_ERR_OK;
	DI_NVRAM_VIDEO_RESOLUTION_E	 eDiNvramResolution, eCurrentResolution;

	eDiNvramResolution = DI_NVRAM_RESOLUTION_Unknown;

	switch (eResolution)
	{
		case eDxRESOLUTION_STATUS_480I:		eDiNvramResolution = DI_NVRAM_RESOLUTION_720x480I;		hErr = ERR_OK;	break;
		case eDxRESOLUTION_STATUS_480P:		eDiNvramResolution = DI_NVRAM_RESOLUTION_720x480P;		hErr = ERR_OK;	break;
		case eDxRESOLUTION_STATUS_576I:		eDiNvramResolution = DI_NVRAM_RESOLUTION_720x576I;		hErr = ERR_OK;	break;
		case eDxRESOLUTION_STATUS_576P:		eDiNvramResolution = DI_NVRAM_RESOLUTION_720x576P;		hErr = ERR_OK;	break;
		case eDxRESOLUTION_STATUS_720P: 	eDiNvramResolution = DI_NVRAM_RESOLUTION_1280x720P;		hErr = ERR_OK;	break;
		case eDxRESOLUTION_STATUS_1080I:	eDiNvramResolution = DI_NVRAM_RESOLUTION_1920x1080I;	hErr = ERR_OK;	break;
		case eDxRESOLUTION_STATUS_1080P:	eDiNvramResolution = DI_NVRAM_RESOLUTION_1920x1080P;	hErr = ERR_OK;	break;
		case eDxRESOLUTION_STATUS_2160P:	eDiNvramResolution = DI_NVRAM_RESOLUTION_3840x2160P;	hErr = ERR_OK;	break;
		default:							return ERR_FAIL;
	}

	hErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_HD_RESOULTION, 0, (void *)&eCurrentResolution, sizeof(DI_NVRAM_VIDEO_RESOLUTION_E));
	if (hErr == DI_ERR_OK && eDiNvramResolution != eCurrentResolution)
	{
		hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_HD_RESOULTION, 0, (void *)&eDiNvramResolution, sizeof(DI_NVRAM_VIDEO_RESOLUTION_E));
	}

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetFrameRate( HUINT32 ulFramerate )
{
	DI_ERR_CODE		 hErr=ERR_OK;
	DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E	 eDiNvramFieldRate, eCurrentFrameRate;

	eDiNvramFieldRate = DI_NVRAM_HDMI_FIELD_RATE_Unknown;

	switch (ulFramerate)
	{
		case eDxPICTURERATE_24P: eDiNvramFieldRate = DI_NVRAM_HDMI_FIELD_RATE_24HZ;	hErr = ERR_OK;	break;
		case eDxPICTURERATE_25P: eDiNvramFieldRate = DI_NVRAM_HDMI_FIELD_RATE_25HZ;	hErr = ERR_OK;	break;
		case eDxPICTURERATE_30P: eDiNvramFieldRate = DI_NVRAM_HDMI_FIELD_RATE_30HZ;	hErr = ERR_OK;	break;
		case eDxPICTURERATE_50I: eDiNvramFieldRate = DI_NVRAM_HDMI_FIELD_RATE_50HZ;	hErr = ERR_OK;	break;
		case eDxPICTURERATE_60I: eDiNvramFieldRate = DI_NVRAM_HDMI_FIELD_RATE_60HZ;	hErr = ERR_OK;	break;
		case eDxPICTURERATE_50P: eDiNvramFieldRate = DI_NVRAM_HDMI_FIELD_RATE_50HZ;	hErr = ERR_OK;	break;
		case eDxPICTURERATE_60P: eDiNvramFieldRate = DI_NVRAM_HDMI_FIELD_RATE_60HZ;	hErr = ERR_OK;	break;
		default:							 return ERR_FAIL;
	}

	hErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_FIELD_RATE, 0, (void *)&eCurrentFrameRate, sizeof(DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E));

	if (hErr == DI_ERR_OK && eCurrentFrameRate != eDiNvramFieldRate)
	{
		hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_FIELD_RATE, 0, (void *)&eDiNvramFieldRate, sizeof(DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E));
	}

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetAspectRatio(DxAspectRatio_e eAspectRatio)
{
	DI_ERR_CODE					 eRet;
	DI_NVRAM_ASPECT_RATIO_E 	 eDiAR, eCurrDiAR;

	switch (eAspectRatio)
	{
	case eDxASPECTRATIO_16X9:
		eDiAR = DI_NVRAM_ASPECT_RATIO_16_9;
		break;

	case eDxASPECTRATIO_4X3:
	default:
		eDiAR = DI_NVRAM_ASPECT_RATIO_4_3;
		break;
	}

	// EEPROM 수명을 위해 같은 값이면 계속 write 하는 행위를 하지 않도록 한다.
	eCurrDiAR = -1;
	eRet = DI_NVRAM_GetField (DI_NVRAM_FIELD_ASPECT_RATIO, 0, (void *)&eCurrDiAR, sizeof(DI_NVRAM_ASPECT_RATIO_E));
	if (eRet == DI_ERR_OK && eCurrDiAR == eDiAR)
	{
		return ERR_OK;
	}

	eRet = DI_NVRAM_SetField (DI_NVRAM_FIELD_ASPECT_RATIO, 0, (void *)&eDiAR, sizeof(DI_NVRAM_ASPECT_RATIO_E));

	return (eRet == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetTunerId(SWUP_InfoSlot_e eSlot, HUINT8 ucTunerId)
{
	DI_ERR_CODE			 hErr=0;
	DI_NVRAM_FIELD_T	 eNvramField;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	eNvramField = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_TUNER_ID1 + eSlot);
	hErr  = DI_NVRAM_SetField (eNvramField, 0, (void *)&ucTunerId, sizeof(HUINT8));

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetAntennaPower(SWUP_InfoSlot_e eSlot, HUINT8 ucAntennaPower)
{
	DI_ERR_CODE		 	 hErr=0;
	DI_NVRAM_FIELD_T	 eNvramField;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)			{ return ERR_FAIL; }

	eNvramField = (DI_NVRAM_FIELD_T)(DI_NVRAM_FIELD_ANTENA_POWER1 + eSlot);
	hErr  = DI_NVRAM_SetField (eNvramField, 0, (void *)&ucAntennaPower, sizeof(HUINT8) );

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetPortalIP( HUINT8 *pucIp )
{
	DI_ERR_CODE		 hErr=0;

	hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_NET_PORTAL_IP, 0, (void *)&pucIp, sizeof(HUINT8) * INET_ADDR_LENGTH);

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetDownloadPath( HUINT8 *pucPath )
{
	DI_ERR_CODE		 hErr=0;

	hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_NET_DOWN_PATH, 0, (void *)&pucPath, sizeof(pucPath));

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetFTPInfo( HUINT8 *pFTPInfo )
{
	DI_ERR_CODE		 hErr=0;

	hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_OTA_FTP_INFO, 0, (void *)pFTPInfo, sizeof(DI_FTP_OTA_SETTING_T));

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetNetworkDeviceId( HUINT8 ucEthernetId )
{
	DI_ERR_CODE		 hErr=0;

	hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_NET_DEVICE_ID, 0, (void *)&ucEthernetId, sizeof(HUINT8));

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetNetworkIpConfig(INET_IP_CONFIG_e eIpConfig )
{
	DI_ERR_CODE		 hErr=0;
	DI_NVRAM_NET_IP_CONFIG_E eConfig;

	switch( eIpConfig )
	{
		case eINET_IP_CONFIG_DHCP:
			eConfig = DI_NVRAM_IP_DHCP;
			break;

		case eINET_IP_CONFIG_MANUAL:
			eConfig = DI_NVRAM_IP_MANUAL;
			break;

		default:
			return ERR_FAIL;
	}

	hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_NET_IP_CONFIG, 0, (void *)&eConfig, sizeof(DI_NVRAM_NET_IP_CONFIG_E));

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetNetworkIp(HULONG ipv4Addr, HULONG netmask, HULONG gatewayAddr, HULONG DNSaddr)
{
	DI_ERR_CODE		 hErr=0;
	DI_NVRAM_NET_IP_SETTING_T	ipSetting;

	memset(&ipSetting, 0, sizeof(DI_NVRAM_NET_IP_SETTING_T));

	memcpy(&(ipSetting.ip), &ipv4Addr, sizeof(HUINT8)*NET_ADDR_LENGTH);
	memcpy(&(ipSetting.mask), &netmask, sizeof(HUINT8)*NET_ADDR_LENGTH);
	memcpy(&(ipSetting.gateway), &gatewayAddr, sizeof(HUINT8)*NET_ADDR_LENGTH);
	memcpy(&(ipSetting.dns), &DNSaddr, sizeof(HUINT8)*NET_ADDR_LENGTH);

	hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_NET_IP_SETTING, 0, (void *)&ipSetting, sizeof(DI_NVRAM_NET_IP_SETTING_T));

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetFirstBoot(HINT32 nFirstBoot)
{
	DI_ERR_CODE		 hErr=0;

	HxLOG_Debug("nFirstBoot: %d \n", nFirstBoot);

	hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_DISPLAY_FACTORY_INFO, 0, (void *)&nFirstBoot, sizeof(HINT32));

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_GetFirstBoot(HINT32 *pnFirstBoot)
{
	DI_ERR_CODE		 hErr=0;

	hErr  = DI_NVRAM_GetField (DI_NVRAM_FIELD_DISPLAY_FACTORY_INFO, 0, (void *)pnFirstBoot, sizeof(HINT32));

	HxLOG_Debug("*pnFirstBoot: %d \n", *pnFirstBoot);

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_GetLaunchAppMode(HUINT8 *pnLaunchAppMode)
{
	if (DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RW, DIAG_APP, 0, (void *)pnLaunchAppMode, sizeof(HUINT8)) == DI_ERR_OK )
		return ERR_OK;
	else
		return ERR_FAIL;
}

HERROR PAL_SYS_SetLaunchAppMode(HUINT8 nLaunchAppMode)
{
	if (DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, DIAG_APP, 0, (void *)&nLaunchAppMode, sizeof(HUINT8)) == DI_ERR_OK )
		return ERR_OK;
	else
		return ERR_FAIL;
}

HERROR PAL_SYS_SetOadVersion( HUINT8 *pucOadVersion )
{
	DI_ERR_CODE		 hErr=0;

	hErr  = DI_NVRAM_SetField (DI_NVRAM_FIELD_OAD_VERSION, 0, (void *)&pucOadVersion, sizeof(HUINT8));

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR	PAL_SYS_SetField(PAL_FLASH_FIELD_t eType, void *pData, HUINT32 ulSize)
{
	HBOOL				bFound=FALSE;
	DI_ERR_CODE			hErr=0;
	HUINT32				i;
	DI_NVRAM_FIELD_T	eDiNvramField = DI_NVRAM_FIELD_DUMMY;

	if( pData == NULL ) return ERR_FAIL;

	for (i=0; i < PAL_FLASH_FIELD_MAX; i++)
	{
		if (eType == s_stPalSysConvInfo[i].ePalField)
		{
			eDiNvramField = s_stPalSysConvInfo[i].eDiNvramField;
			bFound = TRUE;
			break;
		}
	}

	if (FALSE == bFound)
		return ERR_FAIL;

	hErr  = DI_NVRAM_SetField (eDiNvramField, 0, (void *)pData, ulSize);

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR	PAL_SYS_GetField(PAL_FLASH_FIELD_t eType, void *pData, HUINT32 ulSize)
{
	HBOOL				bFound=FALSE;
	DI_ERR_CODE			hErr=0;
	HUINT32				i;
	DI_NVRAM_FIELD_T	eDiNvramField = DI_NVRAM_FIELD_DUMMY;

	if( pData == NULL ) return ERR_FAIL;


	for (i=0; i < PAL_FLASH_FIELD_MAX; i++)
	{
		if (eType == s_stPalSysConvInfo[i].ePalField)
		{
			eDiNvramField = s_stPalSysConvInfo[i].eDiNvramField;
			bFound = TRUE;
			break;
		}
	}

	if (FALSE == bFound)
		return ERR_FAIL;

	hErr  = DI_NVRAM_GetField (eDiNvramField, 0, (void *)pData, ulSize);

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR	PAL_SYS_GetFieldOffset(PAL_FLASH_FIELD_t eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize)
{
	HBOOL				bFound=FALSE;
	DI_ERR_CODE			hErr=0;
	HUINT32				i;
	DI_NVRAM_FIELD_T	eDiNvramField = DI_NVRAM_FIELD_DUMMY;

	if( pData == NULL ) return ERR_FAIL;


	for (i=0; i < PAL_FLASH_FIELD_MAX; i++)
	{
		if (eType == s_stPalSysConvInfo[i].ePalField)
		{
			eDiNvramField = s_stPalSysConvInfo[i].eDiNvramField;
			bFound = TRUE;
			break;
		}
	}

	if (FALSE == bFound)
		return ERR_FAIL;

	hErr  = DI_NVRAM_GetField (eDiNvramField, ulOffset, (void *)pData, ulSize);

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR	PAL_SYS_SetFieldOffset(PAL_FLASH_FIELD_t eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize)
{
	HBOOL				bFound=FALSE;
	DI_ERR_CODE			hErr=0;
	HUINT32				i;
	DI_NVRAM_FIELD_T	eDiNvramField = DI_NVRAM_FIELD_DUMMY;

	if( pData == NULL ) return ERR_FAIL;

	for (i=0; i < PAL_FLASH_FIELD_MAX; i++)
	{
		if (eType == s_stPalSysConvInfo[i].ePalField)
		{
			eDiNvramField = s_stPalSysConvInfo[i].eDiNvramField;
			bFound = TRUE;
			break;
		}
	}

	if (FALSE == bFound)
		return ERR_FAIL;

	hErr  = DI_NVRAM_SetField (eDiNvramField, ulOffset, (void *)pData, ulSize);

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

#if 0
HERROR PAL_SYS_GetModelName (HUINT8 *szModelName, HBOOL bPanelSizeOn)
{
	HUINT8		tmpBuffer[16] = {0, };

	/* parameter check */
	if (szModelName == NULL) return ERR_FAIL;

	/* prefix */
	MWC_UTIL_DvbStrcpy(szModelName, SYSTEM_MODEL_NAME_PREFIX);

	/* project CONFIG NAME */
	memset(tmpBuffer, 0, (sizeof(HUINT8) * 16));

#if defined(CONFIG_PROD_HDR1000S)
	MWC_UTIL_DvbStrcpy(tmpBuffer, SYSTEM_MODEL_NAME);
#else
	MWC_UTIL_DvbStrcpy(tmpBuffer, CONFIG_PRODUCT_NAME);
	MWC_UTIL_ConvertStringUpper(tmpBuffer);
#endif

	MWC_UTIL_DvbStrcat(szModelName, tmpBuffer);

	return ERR_OK;
}
#endif

HERROR PAL_SYS_SetCountryCode(HUINT8  *pucCountryCode)
{
	DI_ERR_CODE	hErr=0;

	hErr = DI_NVRAM_SetField (DI_NVRAM_FIELD_COUNTRY_CODE, 0, (void *)pucCountryCode, 3);

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_SetUsageId(HUINT8 ucUsageId)
{
	DI_ERR_CODE	hErr=0;

	hErr = DI_NVRAM_SetField (DI_NVRAM_FIELD_USAGE_ID, 0, (void *)&ucUsageId, 1);

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_SYS_GetUsageId(HUINT8 *pucUsageId)
{
	DI_ERR_CODE		 hErr=0;
	HUINT8			 ucUsageId = 0;

	hErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_USAGE_ID, 0, (void *)&ucUsageId, 1);
	if(hErr == DI_ERR_OK)
	{
		*pucUsageId = (HUINT8)ucUsageId;
	}

	return (hErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

STATIC PAL_EMERGENCY_FS_PARTITION_t pal_sys_GetPalPartitionType(DI_NVRAM_FS_PARTITION_E eDiPartitionType)
{
	switch (eDiPartitionType)
	{
	case DI_NVRAM_FS_PARTITION_RO:			return PAL_EMERGENCY_FS_PARTITION_RO;
	case DI_NVRAM_FS_PARTITION_RW:			return PAL_EMERGENCY_FS_PARTITION_RW;
	case DI_NVRAM_FS_PARTITION_LOADER:		return PAL_EMERGENCY_FS_PARTITION_LOADER;
	case DI_NVRAM_PARTITION_W_RAWFS:		return PAL_EMERGENCY_PARTITION_W_RAWFS;
	default:
		break;
	}

	return PAL_EMERGENCY_PARTITION_MAX;
}

STATIC DI_NVRAM_FS_PARTITION_E pal_sys_GetDiPartitionType(PAL_EMERGENCY_FS_PARTITION_t ePalPartitionType)
{
	switch (ePalPartitionType)
	{
	case PAL_EMERGENCY_FS_PARTITION_RO:			return DI_NVRAM_FS_PARTITION_RO;
	case PAL_EMERGENCY_FS_PARTITION_RW:			return DI_NVRAM_FS_PARTITION_RW;
	case PAL_EMERGENCY_FS_PARTITION_LOADER:		return DI_NVRAM_FS_PARTITION_LOADER;
	case PAL_EMERGENCY_PARTITION_W_RAWFS:		return DI_NVRAM_PARTITION_W_RAWFS;
	default:
		break;
	}

	return DI_NVRAM_PARTITION_MAX;
}

HERROR PAL_SYS_WriteEmergencySignalling(PAL_EMERGENCY_FS_PARTITION_t partition, HUINT8 *pFileName, HUINT32 offset, void *data, HINT32 nDataSize)
{
	if (DI_NVRAM_Write(pal_sys_GetDiPartitionType(partition), pFileName, offset, data, nDataSize) == DI_ERR_OK )
		return ERR_OK;
	else
		return ERR_FAIL;
}

HERROR PAL_SYS_ReadEmergencySignalling(PAL_EMERGENCY_FS_PARTITION_t partition, HUINT8 *pFileName, HUINT32 offset, void *data, HINT32 nDataSize)
{
	if (DI_NVRAM_Read(pal_sys_GetDiPartitionType(partition), pFileName, offset, data, nDataSize) == DI_ERR_OK )
		return ERR_OK;
	else
		return ERR_FAIL;
}

HERROR PAL_SYS_GetCPUTemperature(HUINT32 *pulCpuTemp)
{
	DI_ERR_CODE		errDiError = DI_ERR_OK;
	DI_SYS_CPU_INFO_t stCpuInfo;

	HxSTD_memset(&stCpuInfo, 0x00, sizeof(DI_SYS_CPU_INFO_t));
	if(pulCpuTemp == NULL)
	{
		return ERR_FAIL;
	}

	errDiError = DI_SYSTEM_GetSystemInfo(DI_SYS_CPU_INFO, (void *)&stCpuInfo, sizeof(HUINT32));
	if(errDiError != DI_ERR_OK)
	{
		return ERR_FAIL;
	}
	*pulCpuTemp = stCpuInfo.ulTemp;

	return ERR_OK;
}


STATIC HUINT8 s_aucPalSys_EncryptKey[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
STATIC HUINT8 s_aucPalSys_EncryptIV[] = {0xad, 0xd6, 0x9e, 0xa3,0x89, 0xc8, 0x17, 0x72, 0x1e, 0xd4, 0x0e, 0xab,0x3d, 0xbc, 0x7a, 0xf2};

//임시로 막아둠
//#define APPLY_64BIT_CHIPID_FOR_KEY	// #if defined(CONFIG_MW_CAS_VERIMATRIX) && defined(CONFIG_MW_SYS_SUPPORT_CHIPID_X64)

HERROR PAL_SYS_InitEncryptionKey (void)
{
	STATIC HBOOL		 s_bInitialized = FALSE;
	HUINT32				 ulChipId = 0;
	HERROR				 hErr;

#if defined(APPLY_64BIT_CHIPID_FOR_KEY)
	HUINT32				 ulChipId2 = 0;
	HUINT32				 ulKey, ulKey2, ulKey3, ulKey4;
#endif

	if (TRUE == s_bInitialized)
	{
		return ERR_OK;
	}

	s_bInitialized = TRUE;

#if defined(APPLY_64BIT_CHIPID_FOR_KEY)
	// *******************************
	// verimatrix + 64bit chip id
	// *******************************
	hErr = PAL_SYS_GetChipId_x64(&ulChipId, &ulChipId2);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_SYS_GetChipId_x64 error: 0x%08x\n", hErr);
		return ERR_FAIL;
	}

	// chip id기반으로 key 생성
	ulKey 	= ulChipId;
	ulKey2 	= ulChipId2;
	ulKey3 	= (ulChipId & ulChipId2) ^ 0x2015;
	ulKey4 	= (ulChipId | ulChipId2) ^ 0x1114;

	HxSTD_memset (s_aucPalSys_EncryptKey, 0, palSys_ENCRYPTION_KEY_SIZE);
	HxSTD_memcpy (&s_aucPalSys_EncryptKey[0], (char *)&ulKey, 4);
	HxSTD_memcpy (&s_aucPalSys_EncryptKey[4], (char *)&ulKey2, 4);
	HxSTD_memcpy (&s_aucPalSys_EncryptKey[8], (char *)&ulKey3, 4);
	HxSTD_memcpy (&s_aucPalSys_EncryptKey[12], (char *)&ulKey4, 4);
#else
	// *******************************
	// 32bit chip id
	// *******************************
	hErr = PAL_SYS_GetChipId (&ulChipId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_SYS_GetChipId error: 0x%08x\n", hErr);
		return ERR_FAIL;
	}

#if defined(CONFIG_IRDETO_HMX_PVR)
	// IRDETO Secure 모델의 경우 ChipId 가 Menu 상에 보여지므로 변형을 한다. ( endian 변경, 3 bit rotate )
	{
		HUINT32	ulChipId2 = 0;

		HxLOG_Info("ORG = %x\n", ulChipId);
		ulChipId2 = (ulChipId >> 24 & 0x000000FF) | (ulChipId >> 8 & 0x0000FF00) | (ulChipId << 8 & 0x00FF0000) | (ulChipId << 24 & 0xFF000000);
		HxLOG_Info("Endian => %x\n", ulChipId2);
		ulChipId = (ulChipId2 >> 3) | ((ulChipId2 & 0x7) << 29);
		HxLOG_Info("Rotate => %x\n", ulChipId);
	}
#endif

	HxSTD_memset (s_aucPalSys_EncryptKey, 0, palSys_ENCRYPTION_KEY_SIZE);
	HxSTD_memcpy (s_aucPalSys_EncryptKey, (char *)&ulChipId, 4);
#endif

#if defined(CONFIG_DEBUG)
	{
		int i;
		HxLOG_Info(" Key value: ");
		for( i=0; i<16; i++)
		{
			HxLOG_Info(" %02X", s_aucPalSys_EncryptKey[i]);
		}
		HxLOG_Info("\n");
	}
#endif

	return ERR_OK;
}

HUINT8 *PAL_SYS_GetEncryptionKey (void)
{
	return s_aucPalSys_EncryptKey;
}

HUINT32 PAL_SYS_GetEncryptionKeySize (void)
{
	return palSys_ENCRYPTION_KEY_SIZE;
}

void *PAL_SYS_CRYPT_MemAlloc (HUINT32 ulSize)
{
	return DI_CRYPT_MemAlloc (ulSize);
}

HUINT8 *PAL_SYS_GetEncryptionIV (void)
{
	return s_aucPalSys_EncryptIV;
}

void PAL_SYS_SetPvrEncryptMode( HUINT32 nEncType )
{
	HxLOG_Debug("[%s] Encrypt Type: %d\n", __FUNCTION__, nEncType);
	s_ulEncryptType = nEncType;

	return;
}

HUINT32 PAL_SYS_GetPvrEncryptMode( void )
{
	/**/

	HxLOG_Debug("[%s] Encrypt Type: %d\n", __FUNCTION__, s_ulEncryptType);
	return s_ulEncryptType;
}

#define __________ENHANCED_SECURITY___________
#if 1// !defined(CONFIG_DROP_ROOT_PRIVILEGES)
HINT32 PAL_SYS_Ioctl(HINT32 nId, HINT32 nRequest, HUINT8 ucCmdNum, ...)
{
#if defined(WIN32)
	return 0;
#else
	typedef void* PVOID;

	va_list 	args;
	HUINT8 	i;
	void 	**ppvData = NULL;
	HINT32 	hErr = 0;

	VK_Print("PAL_SYS_Ioctl : ucNum = %d\n", ucCmdNum);

	if (ucCmdNum <= 0)
		return ERR_FAIL;

	ppvData = (PVOID)OxMW_Malloc(ucCmdNum * sizeof(PVOID));
	if (ppvData == NULL)
		return ERR_FAIL;

	va_start(args, ucCmdNum);
	for (i=0; i<ucCmdNum; i++)
	{
		ppvData[i] = va_arg(args, PVOID);
		VK_Print("\t\t (%d) Arg (x%0x)\n", i, (unsigned int)ppvData[i]);
	}
	va_end(args);

	if (ucCmdNum == 1)
		hErr = ioctl(nId, nRequest, ppvData[0]);
	else if (ucCmdNum == 2)
		hErr = ioctl(nId, nRequest, ppvData[0], ppvData[1]);
	else if (ucCmdNum == 3)
		hErr = ioctl(nId, nRequest, ppvData[0], ppvData[1], ppvData[2]);

	OxMW_Free(ppvData);

	return  hErr;
#endif
}


#else


/* enhanced security define 에 상관없이 호출되는 함수들 - 함수 내부에서 define 처리함 */
HINT32 PAL_SYS_Access(const HINT8 *pucPathname, HINT32 nMode)
{
	HINT32 nRet;

	if (nRet = access(pucPathname, nMode), nRet < 0)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Chmod(pucPathname, ALLOW_FILE_PERMISSION);
			if (nRet == ERR_OK)
				nRet = access(pucPathname, nMode);
		}
	}

	return nRet;
}

HINT32 PAL_SYS_Remove(const HINT8 *pucPathname)
{
	HINT32 nRet;

	if (nRet = remove(pucPathname), nRet < 0)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Remove(pucPathname);
		}
	}

	return nRet;
}

HINT32 PAL_SYS_Mknod(const HINT8 *pucPathname, HUINT32 ulMode, HUINT32 ulDev)
{
#if defined(WIN32)
	return 0;
#else
	HINT32 nRet;

	if (nRet = mknod(pucPathname, ulMode, ulDev), nRet < 0)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Mknod(pucPathname, ulMode, ulDev);
		}
	}

	return nRet;
#endif
}

HINT32 PAL_SYS_Open(const HINT8 *pucPathname, HINT32 nFlags)
{
	HINT32 nRet;
	HINT32 nFd = -1;

	if (nFd = open(pucPathname, nFlags), nFd < 0)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Chmod(pucPathname, ALLOW_FILE_PERMISSION);
			if (nRet == ERR_OK)
				nFd = open(pucPathname, nFlags);

		}
	}

	return nFd;
}

HINT32 PAL_SYS_Open2(const HINT8 *pucPathname, HINT32 nFlags, HUINT32 ulMode)
{
	HINT32 nRet;
	HINT32 nFd = -1;

	if (nFd = open(pucPathname, nFlags, ulMode), nFd < 0)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Chmod(pucPathname, ALLOW_FILE_PERMISSION);
			if (nRet == ERR_OK)
				nFd = open(pucPathname, nFlags, ulMode);
		}
	}

	return nFd;
}

DIR *PAL_SYS_OpenDir(const HINT8 *ucName)
{
	DIR *pDp;
	HINT32 nRet;

	if (pDp = opendir(ucName), NULL == pDp)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Chmod(ucName, ALLOW_DIR_PERMISSION);
			if (nRet == ERR_OK)
				pDp = opendir(ucName);
		}
	}

	return pDp;
}


HINT32 PAL_SYS_ChDir(const HINT8 *pucPathname)
{
	HINT32 nRet;

	if (nRet = chdir(pucPathname), nRet < 0)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Chmod(pucPathname, ALLOW_DIR_PERMISSION);
			if (nRet == ERR_OK)
				nRet = chdir(pucPathname);
		}
	}

	return nRet;
}


HINT32 PAL_SYS_MkDir(const HINT8 *pucPathname, HUINT32 ulMode)
{
	HINT32 nRet;

	if (nRet = mkdir(pucPathname, ulMode), nRet < 0)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Mkdir(pucPathname, ulMode);
		}
	}

	return nRet;
}


HINT32 PAL_SYS_Stat(const HINT8 *pucPath, struct stat *pstBuf)
{
	HINT32 nRet;

	if (nRet = stat(pucPath, pstBuf), nRet < 0)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Stat(pucPath, pstBuf);
		}
	}

	return nRet;
}


HINT32 PAL_SYS_Lstat(const HINT8 *pucPath, struct stat *pstBuf)
{
#if defined(WIN32)
	return 0;
#else
	HINT32 nRet;

	if (nRet = lstat(pucPath, pstBuf), nRet < 0)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Lstat(pucPath, pstBuf);
		}
	}

	return nRet;
#endif
}


HINT32 PAL_SYS_Rename(const HINT8 *pucPath, const HINT8 *pucNewPath)
{
#if defined(WIN32)
	return 0;
#else
	HINT32 nRet;

	if (nRet = rename(pucPath, pucNewPath), nRet < 0)
	{
		if (EACCES == errno)
		{
			nRet = SECAPI_Rename(pucPath, pucNewPath);
		}
	}

	return nRet;
#endif
}

HINT32 PAL_SYS_Socket(HINT32 nDomain, HINT32 nType, HINT32 nProtocol)
{
	return SECAPI_Socket(nDomain, nType, nProtocol);
}


HINT32 PAL_SYS_Close(HINT32 nFd)
{
	return SECAPI_Close(nFd);
}


HINT32 PAL_SYS_Select(HINT32 nFd, fd_set *pstReadFds, fd_set *pstWriteFds, fd_set *pstExceptFds, struct timeval *pstTimeout)
{
	return SECAPI_Select(nFd, pstReadFds, pstWriteFds, pstExceptFds, pstTimeout);
}


HLONG PAL_SYS_Write(HINT32 nFd, const void *pvBuf, HLONG lCount)
{
	return SECAPI_Write(nFd, pvBuf, lCount);
}


HINT32 PAL_SYS_Chown(const HINT8 *pucPathname)
{
	HINT32 i, nIdx, nRet;
	HINT8 ucCurrPath[255];

	VK_Print("pal_sys_Chown == %s\n", pucPathname);
	nIdx = -1;
	for (i=0; i<strlen(pucPathname); i++)
	{
		if (pucPathname[i] == '/')
		{
			nIdx = i;
		}
	}

	VK_Print("pal_sys_Chown == nIdx %d\n", nIdx);

	HxSTD_memset(ucCurrPath, 0, 255);

	if (nIdx > 0)
    	{
		strncpy(ucCurrPath, pucPathname, nIdx+1);
		VK_Print("pal_sys_Chown : ucCurrPath %s\n", ucCurrPath);

		nRet = SECAPI_Chown(ucCurrPath);
	}

	if (strncmp("/mnt/hd2/", ucCurrPath, 9) == 0)
	{
		HINT8 *pucWildPath;

		/* 해당 폴더에 있는 파일의 파일명.*의 setting */
	/*	pucWildPath = strtok(pucPathname, ".");
		if (pucWildPath)
			pucWildPath = strcat(pucWildPath, ".*"); */
		pucWildPath = strncat(ucCurrPath, "*.*", 3);

		VK_Print("pal_sys_Chown : pucWildPath %s\n", pucWildPath);
		nRet = SECAPI_Chown(pucWildPath);
	}
	else
	{
		nRet = SECAPI_Chown(pucPathname);
	}
	return nRet;
}


HINT32 PAL_SYS_Open_BySecHandler(const HINT8 *pucPathname, HINT32 nFlags)
{
	HINT32 nRet = -1;

	nRet = SECAPI_Open(pucPathname, nFlags);

	return nRet;
}


HINT32 PAL_SYS_Lseek(HINT32 fd, off_t pos, HINT32 origin)
{
	HINT32 nRet = -1;

	nRet = SECAPI_Lseek(fd, pos, origin);

	return nRet;
}


HINT32 PAL_SYS_Read(HINT32 fd, void *buf, HUINT32  ulSize)
{
	HINT32 nRet = -1;

	nRet = SECAPI_Read(fd, buf, ulSize);

	return nRet;
}


FILE *PAL_SYS_Fopen(const HINT8 *pucPath, const HINT8 *pucMode)
{
	HINT32 nRet = -1;
	FILE *pFile = NULL;

	pFile = fopen(pucPath,pucMode);
	if (pFile == NULL)
	{
		nRet = SECAPI_Chmod(pucPath, ALLOW_FILE_PERMISSION);
		if (nRet == ERR_OK)
			pFile = fopen(pucPath,pucMode);
	}

	return pFile;
}
#endif


#if defined( CONFIG_MW_INET_SNMP )
void PAL_SYS_SetMWPrivateMIBInfo(PAL_SNMP_MWInformation_t *pstInfo)
{
	DI_SNMP_MWInformation_t stSnmpMWInfo;

	memset ( &stSnmpMWInfo , 0x00 , sizeof(DI_SNMP_MWInformation_t));

	/* 필요한 것만 넣어준다. */
	memcpy( stSnmpMWInfo.szSystemId , pstInfo->szSystemId, 10 );
	memcpy( stSnmpMWInfo.szApplicationVer, pstInfo->szApplicationVer, 10 );
	memcpy( stSnmpMWInfo.szMiddleWareVer, pstInfo->szMiddleWareVer, 10);
	memcpy( stSnmpMWInfo.szSoftBrowserVer, pstInfo->szSoftBrowserVer, 10 );
	memcpy( stSnmpMWInfo.szStbID , pstInfo->szStbID, 6 );

	memcpy( stSnmpMWInfo.szBcasCardId , pstInfo->szBcasCardId, 32 );
	memcpy( stSnmpMWInfo.szCcasCardId , pstInfo->szCcasCardId, 32 );

	stSnmpMWInfo.ulDownFrequency = pstInfo->ulDownFrequency;
	stSnmpMWInfo.ulWatchingChannelId = pstInfo->ulWatchingChannelId;
	stSnmpMWInfo.ulWatchingNetworkId = pstInfo->ulWatchingNetworkId;

	stSnmpMWInfo.etWatchingChannelType = (DI_CH_Type_e)pstInfo->etWatchingChannelType;
	stSnmpMWInfo.ulWatchingTunerGroupId = pstInfo->ulWatchingTunerGroupId;

	stSnmpMWInfo.etPowerStatus = (DI_SNMP_StbPowerStatus_e)pstInfo->etPowerStatus;
	stSnmpMWInfo.etPlayStatus = (DI_SNMP_StbPlayStatus_e)pstInfo->etPlayStatus;

	stSnmpMWInfo.ulTuner1ChannelId = pstInfo->ulTuner1ChannelId;
	stSnmpMWInfo.ulTuner1NetworkId = pstInfo->ulTuner1NetworkId;
	stSnmpMWInfo.ulTuner1Frequency = pstInfo->ulTuner1Frequency;

	stSnmpMWInfo.ulTuner2ChannelId = pstInfo->ulTuner2ChannelId;
	stSnmpMWInfo.ulTuner2NetworkId = pstInfo->ulTuner2NetworkId;
	stSnmpMWInfo.ulTuner2Frequency = pstInfo->ulTuner2Frequency;

	DI_SNMP_SetMWPrivateMIBInfo( &stSnmpMWInfo );
}
#endif

HERROR PAL_SYS_NANDFLASH_Read(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE eDiCode;

	if(pucBuffer == NULL || ulSize == 0)
	{
		HxLOG_Error("pucBuffer is NULL or ulSize is 0\n");
		return ERR_FAIL;
	}

	eDiCode = DI_NANDFLASH_Read(ulAddress, pucBuffer, ulSize);
	if(eDiCode != DI_ERR_OK)
	{
		HxLOG_Error("DI_NANDFLASH_Read() error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SYS_FLASH_Read(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE eDiCode;

	if(pucBuffer == NULL || ulSize == 0)
	{
		HxLOG_Error("pucBuffer is NULL or ulSize is 0\n");
		return ERR_FAIL;
	}

	eDiCode = DI_FLASH_Read(ulAddress, pucBuffer, ulSize);
	if(eDiCode != DI_ERR_OK)
	{
		HxLOG_Error("DI_FLASH_Read() error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
void	PAL_SYS_RegistOverTemperatureCallback(PAL_OverTemperatureCallback_t pfncOverTempCallback)
{
	DI_ERR_CODE err = DI_ERR_ERROR;

	HxLOG_Error("[PAL_SYS_RegistOverTemperatureCallback] +++ \n");

	if(pfncOverTempCallback == NULL)
	{
		HxLOG_Error("\n\n\t\t[%s:%d] pfncOverTempCallback is NULL!\n", __FUNCTION__, __LINE__);
		return;
	}

	s_pfncOverTempCallback = pfncOverTempCallback;

	err = DI_SYSTEM_RegisterEventNotify( DI_SYS_EVENT_OVER_TEMPERATURE, pal_sys_cbOverTemp);
	if(err != DI_ERR_OK)
	{
		HxLOG_Error("\n\n\t\t[%s:%d] DI_SYSTEM_RegisterEventNotify returned  Eror!!!\n", __FUNCTION__, __LINE__);
	}
	return;
}

void	PAL_SYS_RegistFanLockCallback(PAL_FanLockCallback_t pfncFanLockCallback)
{
	DI_ERR_CODE err = DI_ERR_ERROR;

	HxLOG_Error("[PAL_SYS_RegistFanLockCallback] +++ \n");

	if(pfncFanLockCallback == NULL)
	{
		HxLOG_Error("\n\n\t\t[%s:%d] pfncFanLockCallback is NULL!\n", __FUNCTION__, __LINE__);
		return;
	}

	s_pfncFanLockCallback = pfncFanLockCallback;

	err = DI_SYSTEM_RegisterEventNotify( DI_SYS_EVENT_FAN_LOCKED, pal_sys_cbFanLock);
	if(err != DI_ERR_OK)
	{
		HxLOG_Error("\n\n\t\t[%s:%d] DI_SYSTEM_RegisterEventNotify returned	Eror!!!\n", __FUNCTION__, __LINE__);
	}
	return;
}
#endif

#if defined(CONFIG_DEBUG_AGING_TEST) && defined(CONFIG_PROD_DEVICE_STORAGE_USB) && !defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_SUPPORT_UMMA)
STATIC HERROR pal_sys_AgingLog_UsbEventCallback (DI_USB_Event_t etUsbEventType, void *pvUsbInfo)
{
	usb_info_t			*pstDiUsbInfo = (usb_info_t *)pvUsbInfo;
	HERROR				 hErr = ERR_OK;
	HCHAR				 szCmd[128];

	if (pstDiUsbInfo == NULL)
	{
		return ERR_FAIL;
	}

	// start propagation to notice USB storage connection changes
	switch (etUsbEventType)
	{
	case DI_USB_Event_Insert:
		HxLOG_Debug("DI_USB_Event_Insert");

		if(pszMountName != NULL)
		{
			HLIB_STD_MemFree(pszMountName);
			pszMountName = NULL;
		}

		HxSTD_snprintf(szCmd, sizeof(szCmd), "mkdir /media/Aging_%s1", pstDiUsbInfo->devName[0]);
		VK_SYSTEM_Command(szCmd);
		HxSTD_snprintf(szCmd, sizeof(szCmd), "mount /dev/%s1 /media/Aging_%s1", pstDiUsbInfo->devName[0], pstDiUsbInfo->devName[0]);
		VK_SYSTEM_Command(szCmd);

		pszMountName = (HCHAR*)HLIB_STD_MemAlloc(sizeof(HCHAR)*pstDiUsbInfo->devNameSize);
		HxSTD_memcpy(pszMountName, pstDiUsbInfo->devName[0], pstDiUsbInfo->devNameSize);
		break;

	case DI_USB_Event_Extract:
		HxLOG_Debug("DI_USB_Event_Extract");

		HxSTD_snprintf(szCmd, sizeof(szCmd), "umount /media/Aging_%s1", pstDiUsbInfo->devName[0]);
		VK_SYSTEM_Command(szCmd);

		if(pszMountName != NULL)
		{
			HLIB_STD_MemFree(pszMountName);
			pszMountName = NULL;
		}
		break;

	case DI_USB_Event_OverCurrent:
		HxLOG_Debug("DI_USB_Event_OverCurrent");
		break;

	default:
		HxLOG_Critical("\n\n\n");
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

void PAL_SYS_AgingLog_UsbInit(void)
{
	DI_USB_Init();
	DI_USB_RegisterCallback(pal_sys_AgingLog_UsbEventCallback);
	DI_USB_PWR_Ctrl(DI_USB_PWR_ON);
}

HERROR PAL_SYS_AgingLog_GetPath(HCHAR *pszPath)
{
	HERROR bFoundPath = ERR_FAIL;

	if(pszMountName != NULL)
	{
		HxSTD_snprintf(pszPath, HxPATH_MAX, "/media/Aging_%s1", pszMountName);
		bFoundPath = ERR_OK;
	}

	return bFoundPath;
}
#endif


/******** EOF *********/
