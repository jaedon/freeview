/**
	@file     xsvc_sys_base.c
	@brief    xsvc_sys_base.c

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2013 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <pal_sys.h>
#include <pal_param.h>
#include <pal_fuse.h>

#include <isosvc.h>

#include <svc_sys.h>

#include "../local_include/_xsvc_sys.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define BASE_RGBLED_BLINK_INTERVAL		(1000)	//ms

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR	xsvc_sys_GetDeviceId_Base(HUINT8 *pszDeviceId, HUINT32 ulBufSize)
{
	if (pszDeviceId)
		pszDeviceId[0] = '\0';

	return ERR_FAIL;
}

HERROR	xsvc_sys_GetSystemId_Base(HUINT32 *pulSystemId)
{
	if (PalParam_Activated())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_ID, (void *)pulSystemId, sizeof(HUINT32));
	}
	else
	{
		return PAL_SYS_GetSystemId(pulSystemId);
	}
}

HERROR	xsvc_sys_GetModelName_Base(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn)
{
	if (pszModelName == NULL)
	{
		HxLOG_Error("Target is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_snprintf(pszModelName, ulLength, "%s%s", SYSTEM_MODEL_NAME_PREFIX, SYSTEM_MODEL_NAME);

	return ERR_OK;
}

HERROR	xsvc_sys_GetModelName_DerivBase(HUINT8 *pszModelName, HUINT32 ulLength)
{
	HUINT8		szBuffer[16] = {0, };

	if(pszModelName == NULL)
	{
		HxLOG_Error("Target is NULL\n");
		return ERR_FAIL;
	}

	/* prefix */
	HxSTD_StrNCpy((char *)pszModelName, SYSTEM_MODEL_NAME_PREFIX, ulLength);

	/* project CONFIG NAME */
	HxSTD_MemSet(szBuffer, 0, (sizeof(HUINT8) * 16));

	HxSTD_StrNCpy((char *)szBuffer, SYSTEM_MODEL_NAME, 16-1);
	HxSTD_StrNCat((char *)pszModelName, (const char *)szBuffer, ulLength);

	return ERR_OK;
}


STATIC HBOOL							s_bPanelStatus = TRUE;
STATIC HBOOL							s_bPanelDisplay = TRUE;
STATIC PanelDimmingLevel_t				s_ePanelDimLevel = ePANEL_DIM_LevelMax;
STATIC SWUP_Flag_e						s_eSwupFlag = eSWUP_FLAG_NO_INFO;
STATIC HUINT8							s_aucLang[LANG_639_CODE_MAX] = { 'e', 'n', 'g', };
STATIC DxScartFormat_b					s_eScartRgb = eDxSCART_FORMAT_CVBS;
STATIC DxResolutionStatus_b				s_eHdResolution = eDxRESOLUTION_STATUS_576I;
STATIC HUINT8							s_ucTunerId = 0;
STATIC HUINT8							s_ucAntennaPwr = 0;
#if 0 // #if 0 - temporally disblaed (for the warning removing)
STATIC HUINT8							s_ucNetDev = 0;	// 추후에 로더에 무선랜 들어가면 살릴 것.
#endif // #if 0
STATIC HUINT32							s_ulNetConf = 0;
STATIC INET_IP_SETTING_t				s_stNetaddr = {
										 	{ 192, 0, 2, 200, },
											{ 255, 255, 255, 255, },
											{ 192, 0, 2, 1, },
											{ 192, 0, 2, 1, },
										};

STATIC HUINT8							s_aucNetPortalIp[INET_ADDR_LENGTH] = {192, 168, 0, 250, };
#if 0 // #if 0 - for the warning removing
STATIC HUINT8							s_szNetDownPath[INET_DOWN_PATH_MAX] = "http://unkown";
#endif // #if 0

HERROR	xsvc_sys_GetEeprom4LoaderFactoryReset_Base(SvcSys_FactoryDefaultItem_t **pastDefaultItem, HUINT32 *pulItemNum)
{
	STATIC SvcSys_FactoryDefaultItem_t s_astSysFactoryItem[] =
	{
		/* panel status : HUINT32 x 1byte, default: HBOOL */
		{ ePAL_PARAM_OSD_DISPLAY,		(void *)&s_bPanelStatus,			sizeof(HBOOL)}

		/* panel display : HUINT32 x 1byte, default: HBOOL */
		,{ ePAL_PARAM_PANEL_DISPLAY,	(void *)&s_bPanelDisplay,			sizeof(HBOOL)}

		/* panel display dim level : HUINT8 x 1byte, default: PanelDimmingLevel_t */
		,{ ePAL_PARAM_PANEL_DIMMING,	(void *)&s_ePanelDimLevel,			sizeof(PanelDimmingLevel_t)}

		/* ota control : HUINT32 x 1byte, default: SWUP_Flag_e */
		,{ ePAL_PARAM_SWUP_FLAG1,		(void *)&s_eSwupFlag,				sizeof(SWUP_Flag_e)}

		/* 639-2 language code : HUINT8 x 3byte, default: ENG */
		,{ ePAL_PARAM_LANGUAGE,			(void *)s_aucLang,					sizeof(HUINT8) * LANG_639_CODE_MAX}

		/* scart output : HUINT32 x 1byte, default: DxScartFormat_b */
		,{ ePAL_PARAM_SCART_RGB,		(void *)&s_eScartRgb,				sizeof(DxScartFormat_b)}

		/* HD output resolution : HUINT32 x 1byte, default: DxResolutionStatus_b */
		,{ ePAL_PARAM_HD_RESOULTION,	(void *)&s_eHdResolution,			sizeof(DxResolutionStatus_b)}

		/* tuner id : HUINT8 x 1byte, default: Tuner Id */
		,{ ePAL_PARAM_TUNER_ID1,		(void *)&s_ucTunerId,				sizeof(HUINT8)}

		/* external terrestrial antena power : HUINT8 x 1byte, default: External Antena Power for Ter, 0:Off/1:On */
		,{ ePAL_PARAM_ANTENA_POWER1,	(void *)&s_ucAntennaPwr,			sizeof(HUINT8)}

#if 0	// 추후에 로더에 무선랜 들어가면 살릴 것.
		/* network device name : HUINT8 x 1byte, default: ethernet device id 0 = eth0 */
		,{ ePAL_PARAM_NET_DEVICE_ID,	(void *)&s_ucNetDev,				sizeof(HUINT8)}
#endif

		/* network address mode : HUINT8 x 1byte, default: DHCP/Manual */
		,{ ePAL_PARAM_NET_IP_CONFIG,	(void *)&s_ulNetConf,				sizeof(HUINT32)}

		/* network address : HUINT8 x 16byte, default: INET_IP_SETTING_t */
		,{ ePAL_PARAM_NET_IP_SETTING,	(void *)&s_stNetaddr,				sizeof(INET_IP_SETTING_t)}

		/* net portal address : HUINT8 x 4byte, default: IP address */
		,{ ePAL_PARAM_NET_PORTAL_IP,	(void *)s_aucNetPortalIp,			sizeof(HUINT8) * INET_ADDR_LENGTH}

#if 0	// nagra spec 때문에 안된다고 함. 우 차장님..
		/* net portal address : HUINT8 x 44byte, default: IP address */
		,{ ePAL_PARAM_NET_DOWN_PATH,	(void *)s_szNetDownPath,			sizeof(HUINT8) * INET_DOWN_PATH_MAX}
#endif

		,{ SvcSys_PARAM_KEY_INVALID,	(void *)NULL,						0}
	};

	*pulItemNum = sizeof(s_astSysFactoryItem);
	*pastDefaultItem = s_astSysFactoryItem;

	return ERR_OK;
}

HERROR	xsvc_sys_GetFusingInfo_Base(Sys_FusingInfo_t *pstFusingInfo)
{
	HERROR	hErr;

	HxSTD_MemSet(pstFusingInfo, 0, sizeof(Sys_FusingInfo_t));

	hErr = PAL_FUSE_GetInfo(pstFusingInfo);

	return hErr;
}

HERROR	xsvc_sys_SetFusingInfoJTAG_Base(HUINT8 ucJTAG)
{
	HERROR				hErr = ERR_FAIL;
	Sys_FusingInfo_t	stFusingInfo;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));
	stFusingInfo.ucSCS	= FALSE;
	stFusingInfo.ucJTAG = ucJTAG;
	stFusingInfo.ucOTP	= FALSE;
	stFusingInfo.ucCWE	= FALSE;

	hErr = PAL_PARAM_SetField(ePAL_PARAM_FUSING_INFO, (void *)&stFusingInfo, sizeof(Sys_FusingInfo_t));

	return hErr;
}

HERROR	xsvc_sys_SetFusingInfoCW_Base(HUINT8 ucCWE)
{
	HERROR				hErr = ERR_FAIL;
	Sys_FusingInfo_t	stFusingInfo;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));
	stFusingInfo.ucSCS	= FALSE;
	stFusingInfo.ucJTAG = FALSE;
	stFusingInfo.ucOTP	= FALSE;
	stFusingInfo.ucCWE	= ucCWE;

	hErr = PAL_PARAM_SetField(ePAL_PARAM_FUSING_INFO, (void *)&stFusingInfo, sizeof(Sys_FusingInfo_t));

	return hErr;
}

HERROR	xsvc_sys_SetFusingInfoSecureBoot_Base(HUINT8 ucSCS)
{
	HERROR				hErr = ERR_FAIL;
	Sys_FusingInfo_t	stFusingInfo;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));
	stFusingInfo.ucSCS	= ucSCS;
	stFusingInfo.ucJTAG = FALSE;
	stFusingInfo.ucOTP	= FALSE;
	stFusingInfo.ucCWE	= FALSE;

	hErr = PAL_PARAM_SetField(ePAL_PARAM_FUSING_INFO, (void *)&stFusingInfo, sizeof(Sys_FusingInfo_t));

	return hErr;
}

HUINT32	xsvc_sys_UpgradeGetMaxUpgradeSize_Base(void)
{
	return 1024*1024*128;
}

HERROR xsvc_sys_GetRgbLedInfo_Base(SvcSys_RgbLedInfo_t **pRgbLedList, HUINT32 *pulNum)
{
// USAGE : If Use to RGB LED then define to SvcSys_RgbLedInfo_t list below
// return ERR_OK and fill the variable *pRgbLedList = s_stRgbLedInfo, *pulNum = N
/*
	STATIC SvcSys_RgbLedInfo_t		s_stRgbLedInfo[2] = 
		{
				{eDxLED_ID_RGB_REC, {eDxRGBLED_COLOR_NONE, eDxRGBLED_DIMMLEVEL_SOLID}, FALSE, NULL},
				{eDxLED_ID_RGB_NET, {eDxRGBLED_COLOR_NONE, eDxRGBLED_DIMMLEVEL_SOLID}, FALSE, NULL}
		};
*/


	// No RGB Led
	*pRgbLedList = NULL;
	*pulNum = 0;
	
	return ERR_FAIL;
}

HULONG xsvc_sys_GetRgbBlinkInterval_Base(void)
{
	return BASE_RGBLED_BLINK_INTERVAL;
}

