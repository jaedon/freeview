/**************************************************************************************/
/**
 * @file bplinit.c
 *
 * Initialization
 *
 * @author  ipm(@humaxdigital.com)
 * @date    2012/09/24
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "bplinit.h"
#include "bpldb.h"

#include "vkernel.h"
#include "hlib.h"
#include "hapi.h"
#include "oapi.h"
#include "hapi.h"

#include "apk.h"

//#define WIDGET_MOUNT_RETRY_MAX		20
//#define WIDGET_MOUNT_PATH			"/mnt/hd4/"

/*********************************************************************************
 * Static functions
 *********************************************************************************/

static BPLStatus __init_subtitle()
{
	HINT32 ulValue;

	APK_DB_GetInt(USERDB_SUBTITLE_DISPLAY, &ulValue);
	HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", ulValue ? 1 : 0);

	APK_DB_GetInt(USERDB_SUBTITLE_FONT, &ulValue);
	if (ulValue)
		HAPI_BroadcastSignal("signal:onFontTypeChanged", 1, "i", 1);
	else
		HAPI_BroadcastSignal("signal:onFontTypeChanged", 1, "i", 0);

	return BPL_STATUS_OK;
}

static void __status_callback(HBOOL bConnected)
{
	HERROR	hErr;
	HINT32	nValue;
	HCHAR	szFriendlyName[128];
	HCHAR	szDefaultFriendlyName[128];
	HBOOL bAddDlnaServiceName = FALSE; // 2016/3/8 mhkang: UPnP Device Name은 Homma가 UPnP Device Type에 맞게 설정한다(http://svn.humaxdigital.com:3000/issues/111203)

	if(bConnected == TRUE)
	{
		/* HOMMA RPC Server is ready */
		HxLOG_Warning("Homma server is ready --> Start DLNA modules!\n");

		/* Set Friendly name & DLNA module start */
		if (ERR_OK == APK_SYSINFO_STB_GetModelName(szDefaultFriendlyName, 128))
		{
			// DMS
			HxSTD_MemSet(szFriendlyName, 0, 128);
			hErr = APK_DB_GetStr( USERDB_DLNA_DMS_FRIENDLYNAME, szFriendlyName, 128 );
			if ((hErr != ERR_OK) || HxSTD_StrEmpty(szFriendlyName))
			{
				/* HMS PH#1/PH#2, HMS-1000T --> Use modelName itself for friendlyName without upnp device service name */
				HxSTD_snprintf(szFriendlyName, sizeof(szFriendlyName)-1, "%s", szDefaultFriendlyName);
				if (bAddDlnaServiceName == TRUE)
					HxSTD_StrNCat(szFriendlyName, " Media Server", sizeof(szFriendlyName)-1);

				if (APK_DB_SetStr(USERDB_DLNA_DMS_FRIENDLYNAME, szFriendlyName)  != ERR_OK)
					HxLOG_Error("BPL_INIT: Error> DB_SetStr(DMS) failed\n");
			}

			if (ERR_OK == APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION, &nValue))
			{
				if (nValue)
				{
					APK_DLNADMS_Start(szFriendlyName, TRUE);
					APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HINT32)TRUE );
				}
				else
				{
					APK_DLNADMS_Stop();
					APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HINT32)FALSE );
				}
			}
			else
			{
				APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HUINT32)FALSE );
			}

#if !defined(CONFIG_OP_JAPAN)
			// WOON
			HxSTD_MemSet(szFriendlyName, 0, 128);
			hErr = APK_DB_GetStr( USERDB_WOONSERVER_FRIENDLYNAME, szFriendlyName, 128 );
			if ((hErr != ERR_OK) || HxSTD_StrEmpty(szFriendlyName))
			{
				HxSTD_snprintf( szFriendlyName, sizeof(szFriendlyName), "%s", szDefaultFriendlyName);
				if (bAddDlnaServiceName == TRUE)
					HxSTD_StrNCat(szFriendlyName, " Woon Server", sizeof(szFriendlyName)-1);

				if (APK_DB_SetStr(USERDB_WOONSERVER_FRIENDLYNAME, szFriendlyName)  != ERR_OK)
					HxLOG_Error("BPL_INIT: Error> DB_SetStr(WOONSERVER) failed\n");
			}

			if (ERR_OK == APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION, &nValue))
			{
				if (nValue)
				{
					if(ERR_OK == APK_DB_GetInt(USERDB_WOONSERVER_ACTIVATION, &nValue))
					{
						if (nValue)
						{
							APK_WOONSERVER_Start(szFriendlyName);
							APK_DB_SetInt(USERDB_WOONSERVER_ACTIVATION, (HINT32)TRUE );
						}
						else
						{
							APK_WOONSERVER_Stop();
							APK_DB_SetInt(USERDB_WOONSERVER_ACTIVATION, (HUINT32)FALSE );
						}
					}
					else
					{
						APK_DB_SetInt(USERDB_WOONSERVER_ACTIVATION, (HUINT32)FALSE );
					}
				}
			}
#endif

			// DMR
			HxSTD_MemSet(szFriendlyName, 0, 128);
			hErr = APK_DB_GetStr( USERDB_DLNA_DMR_FRIENDLYNAME, szFriendlyName, 128);
			if ((hErr != ERR_OK) || HxSTD_StrEmpty(szFriendlyName))
			{
				HxSTD_snprintf( szFriendlyName, sizeof(szFriendlyName)-1, "%s", szDefaultFriendlyName);
				if (bAddDlnaServiceName == TRUE)
					HxSTD_StrNCat(szFriendlyName, " Media Renderer", sizeof(szFriendlyName)-1);

				if (APK_DB_SetStr(USERDB_DLNA_DMR_FRIENDLYNAME, szFriendlyName)  != ERR_OK)
					HxLOG_Error("BPL_INIT: Error> DB_SetStr(DMR) failed\n");
			}

			if (ERR_OK == APK_DB_GetInt(USERDB_DLNA_DMR_ACTIVATION, &nValue))
			{
				if (nValue)
				{
					APK_DLNADMR_Start(szFriendlyName);
					APK_DB_SetInt(USERDB_DLNA_DMR_ACTIVATION, (HINT32)TRUE );
				}
				else
				{
					APK_DLNADMR_Stop();
					APK_DB_SetInt(USERDB_DLNA_DMR_ACTIVATION, (HINT32)FALSE );
				}
			}
			else
			{
				APK_DB_SetInt(USERDB_DLNA_DMR_ACTIVATION, (HUINT32)FALSE );
			}


			// DIAL
			HxSTD_MemSet(szFriendlyName, 0, 128);
			hErr = APK_DB_GetStr( USERDB_DLNA_DIAL_FRIENDLYNAME, szFriendlyName, 128);
			if ((hErr != ERR_OK) || HxSTD_StrEmpty(szFriendlyName))
			{
				HxSTD_snprintf( szFriendlyName, sizeof(szFriendlyName)-1, "%s", szDefaultFriendlyName);
				if (bAddDlnaServiceName == TRUE)
					HxSTD_StrNCat(szFriendlyName, " DIAL Server", sizeof(szFriendlyName)-1);

				if (APK_DB_SetStr(USERDB_DLNA_DIAL_FRIENDLYNAME, szFriendlyName)  != ERR_OK)
					HxLOG_Error("BPL_INIT: Error> DB_SetStr(DIAL) failed\n");
			}

			if (ERR_OK == APK_DB_GetInt(USERDB_DLNA_DIAL_ACTIVATION, &nValue))
			{
				if (1 /*nValue*/)	// DIAL module always have to execute in FVP-4000T
				{
					APK_DIAL_Start(szFriendlyName);
					APK_DB_SetInt(USERDB_DLNA_DIAL_ACTIVATION, (HINT32)TRUE );
				}
				else
				{
					APK_DIAL_Stop();
					APK_DB_SetInt(USERDB_DLNA_DIAL_ACTIVATION, (HINT32)FALSE );
				}
	 		}
			else
			{
				APK_DB_SetInt(USERDB_DLNA_DIAL_ACTIVATION, (HUINT32)FALSE );
			}

			// SATIP
			HxSTD_MemSet(szFriendlyName, 0, 128);
			hErr = APK_DB_GetStr( USERDB_DLNA_SATIP_FRIENDLYNAME, szFriendlyName, 128);
			if ((hErr != ERR_OK) || HxSTD_StrEmpty(szFriendlyName))
			{
				HxSTD_snprintf( szFriendlyName, sizeof(szFriendlyName)-1, "%s", szDefaultFriendlyName);
				if (bAddDlnaServiceName == TRUE)
					HxSTD_StrNCat(szFriendlyName, " SAT>IP Server", sizeof(szFriendlyName)-1);

				if (APK_DB_SetStr(USERDB_DLNA_SATIP_FRIENDLYNAME, szFriendlyName)  != ERR_OK)
					HxLOG_Error("BPL_INIT: Error> DB_SetStr(SATIP) failed\n");
			}

			if (ERR_OK == APK_DB_GetInt(USERDB_DLNA_SATIP_ACTIVATION, &nValue))
			{
				if (nValue)
				{
					APK_SATIP_Start(szFriendlyName);
					APK_DB_SetInt(USERDB_DLNA_SATIP_ACTIVATION, (HINT32)TRUE );
				}
				else
				{
					APK_SATIP_Stop();
					APK_DB_SetInt(USERDB_DLNA_SATIP_ACTIVATION, (HINT32)FALSE );
				}
			}
			else
			{
				APK_DB_SetInt(USERDB_DLNA_SATIP_ACTIVATION, (HUINT32)FALSE );
			}
		}
	}
	else
	{
		/* HOMMA RPC Server is ready */
		HxLOG_Warning("Homma server is disconnected --> Do nothing!\n");
	}
}

static void __init_dlna_module(void)
{
	// 1. DLNA Module init
	APK_DLNADMS_Init();
	APK_DLNADMP_Init();
	APK_DLNADMR_Init();

	APK_AIRPLAY_Init();

	APK_DIAL_Init();
	APK_SATIP_Init();

#if !(defined(CONFIG_OP_JAPAN) && !defined(CONFIG_OP_MIDDLE_EAST))
	APKI_WOONSERVER_Init();
#endif

	// 2. Register notifier
	APK_DLNA_COMMON_RegisterNotifier(__status_callback);
}

/*********************************************************************************
 * Interfaces
 *********************************************************************************/
#if defined(CONFIG_PROD_HMS1000S)
void BPL_VIDEO_ConvertResolution(void)
{
#define eRESSELECT_UNKNOWN			0x00000000	// You can see this definition from hms1000s_ph1
#define eRESSELECT_640x480P 		0x00000001
#define eRESSELECT_720x480I 		0x00000002
#define eRESSELECT_720x576I 		0x00000004
#define eRESSELECT_720x480P 		0x00000008
#define eRESSELECT_720x576P 		0x00000010
#define eRESSELECT_1080x720P		0x00000020
#define eRESSELECT_1920x1080I		0x00000040
#define eRESSELECT_1920x1080P		0x00000080
#define eRESSELECT_FOLLOW_SOURCE	0x00000100
#define eRESSELECT_EDID_MAX 		0x00000200
#define eRESSELECT_D1				0x00000400
#define eRESSELECT_D2				0x00000800
#define eRESSELECT_D3				0x00001000
#define eRESSELECT_D4				0x00002000
#define eRESSELECT_MAX				0x00003FFF
#define	USERDB_VIDEO_RESOLUTION		"UDB/VIDEO_Resolution"

	HUINT32 eResolution = eRESSELECT_UNKNOWN;

	if(APK_DB_GetInt(USERDB_VIDEO_RESOLUTION, (HINT32*)&eResolution) == ERR_OK)
	{
		DxResolutionSelect_e eNewResolution;
		switch(eResolution)
		{
			case eRESSELECT_UNKNOWN :
				eNewResolution = eDxRESOLUTION_SELECT_UNKNOWN;
				break;
			case eRESSELECT_720x480I : case eRESSELECT_720x480P :
				eNewResolution = eDxRESOLUTION_SELECT_480I;
				break;
			case eRESSELECT_720x576I :
				eNewResolution = eDxRESOLUTION_SELECT_576I;
				break;
			case eRESSELECT_640x480P :
				eNewResolution = eDxRESOLUTION_SELECT_480P;
				break;
			case eRESSELECT_720x576P :
				eNewResolution = eDxRESOLUTION_SELECT_576P;
				break;
			case eRESSELECT_1080x720P :
				eNewResolution = eDxRESOLUTION_SELECT_720P;
				break;
			case eRESSELECT_1920x1080I : case eRESSELECT_D1: case eRESSELECT_D2: case eRESSELECT_D3: case eRESSELECT_D4:
				eNewResolution = eDxRESOLUTION_SELECT_1080I;
				break;
			case eRESSELECT_1920x1080P :
				eNewResolution = eDxRESOLUTION_SELECT_1080P;
				break;
			case eRESSELECT_FOLLOW_SOURCE :
				eNewResolution = eDxRESOLUTION_SELECT_ORIGINAL;
				break;
			case eRESSELECT_EDID_MAX :
				eNewResolution = eDxRESOLUTION_SELECT_EDID_MAX;
				break;
			default :
				eNewResolution = eDxRESOLUTION_SELECT_1080I;
				break;
		}
		APK_DB_Remove(USERDB_VIDEO_RESOLUTION);
		APK_OUTPUT_VIDEO_SetResolutionSelect(eNewResolution);
	}

#undef USERDB_VIDEO_RESOLUTION
#undef eRESSELECT_UNKNOWN
#undef eRESSELECT_640x480P
#undef eRESSELECT_720x480I
#undef eRESSELECT_720x576I
#undef eRESSELECT_720x480P
#undef eRESSELECT_720x576P
#undef eRESSELECT_1080x720P
#undef eRESSELECT_1920x1080I
#undef eRESSELECT_1920x1080P
#undef eRESSELECT_FOLLOW_SOURCE
#undef eRESSELECT_EDID_MAX
#undef eRESSELECT_D1
#undef eRESSELECT_D2
#undef eRESSELECT_D3
#undef eRESSELECT_D4
#undef eRESSELECT_MAX
}


void BPL_VIDEO_ConvertRatio(void)
{
#define	USERDB_VIDEO_SCREEN_RATIO		"UDB/VIDEO_ScreenRatio"

	HUINT32 eAspectRatio = eDxASPECTRATIO_1X1;

	if(APK_DB_GetInt(USERDB_VIDEO_SCREEN_RATIO, (HINT32*)&eAspectRatio) == ERR_OK)
	{
		APK_DB_Remove(USERDB_VIDEO_SCREEN_RATIO);
		APK_OUTPUT_VIDEO_SetTvAspectRatio(eAspectRatio);
	}
#undef USERDB_VIDEO_SCREEN_RATIO
}


void BPL_VIDEO_ConvertScartInfo(void)
{
#define	USERDB_VIDEO_TV_SCART		"UDB/VIDEO_TvScart"

	DxScartFormat_e 		eScartFormat;

	if(APK_DB_GetInt(USERDB_VIDEO_TV_SCART, (HINT32*)&eScartFormat) == ERR_OK)
	{
		APK_DB_Remove(USERDB_VIDEO_TV_SCART);
		APK_OUTPUT_VIDEO_SetTvScartFormat(eScartFormat);
	}

#undef USERDB_VIDEO_TV_SCART
}

void BPL_VIDEO_ConvertDFC(void)
{
#define eDFC_BestPixel 7				// You can see this definition from hms1000s_ph1
#define	USERDB_VIDEO_DISPLAY_FORMAT		"UDB/VIDEO_DisplayFormat"

	DxDfcSelect_e			eDfcSelect;

	if(APK_DB_GetInt(USERDB_VIDEO_DISPLAY_FORMAT, (HINT32*)&eDfcSelect) == ERR_OK)
	{
		if(eDfcSelect == eDFC_BestPixel)
			eDfcSelect = eDxDFC_SELECT_DEFAULT;
		else if(eDfcSelect > eDFC_BestPixel)
			eDfcSelect--;

		APK_DB_Remove(USERDB_VIDEO_DISPLAY_FORMAT);
		APK_OUTPUT_VIDEO_SetDfcSelect(eDfcSelect);
	}

#undef USERDB_VIDEO_DISPLAY_FORMAT
#undef eDFC_BestPixel
}
#endif


void	BPL_VIDEO_Init(void)
{
#ifdef CONFIG_PROD_HMS1000S
	BPL_VIDEO_ConvertScartInfo();
	BPL_VIDEO_ConvertResolution();
	BPL_VIDEO_ConvertRatio();
	BPL_VIDEO_ConvertDFC();
#endif
}

BPLStatus BPL_SetInitialValue(void)
{
	HERROR	hErr;
	HINT32	nValue;

	BPL_VIDEO_Init();

//	if (APK_DB_GetInt(USERDB_FTP_LAUNCH, &nValue) == ERR_OK)
//		APK_SQCFTP_SetFTPOn();

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HGS1000S)
	APK_MEDIA_PLAY_SetTSREnable(TRUE);
#else
	if (APK_DB_GetInt(USERDB_RECORDING_TSR_ONOFF, &nValue) == ERR_OK)
	{
		if (nValue)
			APK_MEDIA_PLAY_SetTSREnable(TRUE);
	}
#endif

	nValue = FALSE;
	hErr = APK_DB_GetInt(USERDB_SAMBASERVER_ACTIVATION, &nValue);
	if (hErr == ERR_OK)
	{
		if (nValue)
			APK_FILEMGR_SAMBASERVER_Start();
		else
			APK_FILEMGR_SAMBASERVER_Stop();
	}
	else
	{
		APK_DB_SetInt(USERDB_SAMBASERVER_ACTIVATION, (HUINT32)FALSE );
	}

	nValue = FALSE;
	hErr = APK_DB_GetInt(USERDB_FTPSERVER_ACTIVATION, &nValue);
	if (hErr == ERR_OK)
	{
		if (nValue)
			APK_FILEMGR_FTPSERVER_Start();
		else
			APK_FILEMGR_FTPSERVER_Stop();
	}
	else
	{
		APK_DB_SetInt(USERDB_FTPSERVER_ACTIVATION, (HUINT32)FALSE );
	}

	return BPL_STATUS_OK;
}

BPLStatus BPL_Init(void)
{
//	int retry = WIDGET_MOUNT_RETRY_MAX;

#ifdef	CONFIG_DEBUG
	APK_CMD_Init();
#endif

	if (APK_DB_Init() != ERR_OK)
		BPL_DB_Default();

	APK_NETWORK_Init();

	APK_POWER_Init();

	APK_DOWNLOAD_Init();

	APK_MEDIA_PLAY_Init ();
	APK_MEDIA_RECORD_Init ();
	APK_RSC_Init();

	APK_SYSINFO_TUNER_Init();
	APK_SYSINFO_STB_Init();

	APK_BUILDER_Init();
	APK_OUTPUT_VIDEO_Init();
	APK_OUTPUT_AUDIO_Init();
	APK_OUTPUT_HDMI_Init();
	APK_OUTPUT_PANEL_Init();
	APK_OUTPUT_THERMAL_Init();

	APK_META_SVC_Init(TRUE);

	APK_META_ANTENNA_Init(TRUE);

	APK_SCENARIO_Init();
	APK_SCENARIO_PINCODE_Init();

	APK_META_QUERY_Init(TRUE);
	APK_META_RECORD_Init();

	APK_SWUPDATE_Init();
	APK_DSMCC_Init();

	APK_FILEMGR_Init();
	APK_FILEMGR_STORAGE_Init();
	APK_FILEMGR_SAMBACLIENT_Init();
	APK_FILEMGR_SAMBASERVER_Init();
	APK_FILEMGR_FTPSERVER_Init();
	APK_EXTDEV_Init();

#if defined(CONFIG_OP_JAPAN)
	APKI_CAS_BC_Init();
#else	// CAS module init ºÎºÐ Á¤¸® ÇÊ¿ä.
	APK_CAS_NAGRA_Init();
	APK_CAS_IRDETO_Init();
	APK_CAS_MMI_Init();
	APK_CAS_CI_Init();
	APK_CAS_ACTION_Init();
	APK_CAS_VERIMATRIX_Init();
#endif
	// corsair ?????? napi senario ?? enable ???ش?.

	//APK_NETWORK_BT_Init();
	APK_NETWORK_CM_Init();
	APK_NETWORK_DHCPSVR_Init();

	__init_dlna_module();

	APK_EXE_Init();

	BPL_SetInitialValue();

	if ( __init_subtitle() != BPL_STATUS_OK ) return BPL_STATUS_ERROR;

// improve booting time :  Don't wait for mount of WIDGET_MOUNT_PATH
#if 0 //defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMS1000T)
	/* Temp: browser have to wait untill finishing /mnt/hd4 mount. */
	while ( retry-- )
	{
		if ( HLIB_DIR_IsMountedPath(WIDGET_MOUNT_PATH) ) break;
		usleep(1000*1000);
	}

	HxLOG_Critical("BPL_INIT: Widget partition has been mounted.\n");
#endif

	return BPL_STATUS_OK;
}

BPLStatus BPL_GetHumaxUIString(char *buf, int bufsize)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	const char *__ui_name = "PRISM";
	const char *__operator = "NONE";
#elif defined(CONFIG_PROD_JCNGS)
	const char *__ui_name = "NAVY";
	const char *__operator = "JCOM";
#elif defined(CONFIG_PROD_HDR1000S)
	const char *__ui_name = "FREESATG2";
	const char *__operator = "FREESAT";
#elif defined(CONFIG_PROD_ICORDPRO)
	const char *__ui_name = "PRISM";
	const char *__operator = "SES";
	const char *__extension = " hdplusSmartTV/1.0 (NETRANGEMMH; )";
#elif defined(CONFIG_PROD_HGS1000S)
	const char *__ui_name = "PRISM";
	const char *__operator = "SES";
	const char *__extension = " hdplusSmartTV/1.0 (NETRANGEMMH; )";
#elif defined(CONFIG_PROD_HMS1000T)
	const char *__ui_name = "PRISM";
	const char *__operator = "NONE";
#elif defined(CONFIG_PROD_DIGIMAXT2)
	const char *__ui_name = "PRISM Zero";
	const char *__operator = "NONE";
//#elif defined(CONFIG_OP_UK_DTT)
#else
	const char *__ui_name = "PRISM";
	const char *__operator = "NONE";
#endif

	if ( !buf ) return BPL_STATUS_ERROR;

#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HGS1000S)
	HxSTD_snprintf(buf, bufsize, "%s-%s-%s-%s; %s", "UX", __ui_name, "OP", __operator, __extension);
#else
	HxSTD_snprintf(buf, bufsize, "%s-%s-%s-%s", "UX", __ui_name, "OP", __operator);
#endif

	return BPL_STATUS_OK;
}
