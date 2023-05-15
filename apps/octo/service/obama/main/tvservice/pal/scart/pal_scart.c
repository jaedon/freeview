/**
	@file	pal_scart.c
	@brief	pal_scart.c

	Description: SCART 관련 사양 정리 및 시나리오를 처리한다. \n
	Module: PAL /SCART												\n

	Copyright (c) 2008 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include <di_video.h>
#include <di_scart.h>
#include <di_nvram.h>

#include <get_enum_str.h>

#include <pal_scart.h>
#include <pal_sys.h>
#include <pal_scaler.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef            *******************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables      *****************************/
/*******************************************************************/
STATIC HBOOL				s_bOutputEnable;

STATIC DxAspectRatio_e	s_eScartAspectRatio;

STATIC DxScartFormat_e		s_eTvScartFormat;
STATIC DxScartFormat_e		s_eVcrScartFormat;

STATIC DxScartPath_e			s_eScartPath;

/*******************************************************************/
/********************      Functions       ********************************/
/*******************************************************************/
#if 0
STATIC HUINT32 pal_scart_ConvertDiScartSettingValue (DI_SCART_SETTING_VALUE eValue)
{
	switch (eValue)
	{
	// PATH :
	case DI_SCART_MODE_NORMAL:		// Path : STB->TV & STB->VCR
		return (HUINT32)eDxSCART_PATH_STB2TV;

	case DI_SCART_MODE_BYPASS:		// Path : VCR->TV
		return (HUINT32)eDxSCART_PATH_VCR2TV;

	case DI_SCART_MODE_HW_BYPASS:	// Path : VCR<->TV (bi-directional)
	case DI_SCART_MODE_HW_BYPASS_NO_DET:
		return (HUINT32)eDxSCART_PATH_HWBYPASS;

	// Aspect Ration
	case DI_SCART_AR_4_3:
		return (HUINT32)eDxASPECTRATIO_4X3;

	case DI_SCART_AR_16_9:
		return (HUINT32)eDxASPECTRATIO_16X9;

	// Output Format
	case DI_SCART_OUTPUT_CVBS:
		return (HUINT32)eDxSCART_FORMAT_CVBS;

	case DI_SCART_OUTPUT_RGB:
		return (HUINT32)eDxSCART_FORMAT_RGB;

	case DI_SCART_OUTPUT_SVIDEO:
		return (HUINT32)eDxSCART_FORMAT_SVIDEO;

	default:
		HxLOG_Critical("\n\n\n");
		return 0;
	}

}
#endif

STATIC void pal_scart_DiVcrPowerCallback(void *pvParam)
{
	HBOOL bIsConnect;

	if (pvParam != NULL)
	{
		bIsConnect = *((HBOOL *)pvParam);

		HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, bIsConnect);

		if(PalScart_Callback_t != NULL)
		{
			if(bIsConnect != FALSE)
			{
				PalScart_Callback_t(eSCART_EVENT_VCR_POWER, 0, TRUE);
			}
			else
			{
				PalScart_Callback_t(eSCART_EVENT_VCR_POWER, 0, FALSE);
			}
		}
	}

	return;
}

STATIC void pal_scart_DiVcrAspectRatioCallback(void *pvParam)
{
	if (pvParam != NULL)
	{
		if (PalScart_Callback_t != NULL)
		{
			PalScart_Callback_t(eSCART_EVENT_VCR_ASPECTRATIO, 0, 1);
		}
	}

	return;
}

#if defined(CONFIG_DEBUG)
HERROR pal_scart_PrintStatus(void)
{
	DI_SCART_SETTINGS	 stDiSetting;

	HLIB_CMD_Printf("===================== pal scart status ============================ \n");
	HLIB_CMD_Printf("Output Enable : %d\n", s_bOutputEnable);

	HLIB_CMD_Printf("SCART AspectRatio : 0x%X[%s]\n", s_eScartAspectRatio, OTL_ENUM2STR_AspectRatio(s_eScartAspectRatio));
	HLIB_CMD_Printf("TV SCART Format : 0x%X[%s]\n", s_eTvScartFormat, OTL_ENUM2STR_ScartFormat(s_eTvScartFormat));
	HLIB_CMD_Printf("VCR SCART Format : 0x%X[%s]\n", s_eVcrScartFormat, OTL_ENUM2STR_ScartFormat(s_eVcrScartFormat));

	HLIB_CMD_Printf("SCART Path : 0x%X[%s]\n", s_eScartPath, OTL_ENUM2STR_ScartPath(s_eScartPath));

	HLIB_CMD_Printf("===================== di scart status ============================ \n");
	DI_SCART_GetSettings (&stDiSetting);

	HLIB_CMD_Printf("stDiSetting.eMode : 0x%X\n", stDiSetting.eMode);
	HLIB_CMD_Printf("stNormalMode.eTvAspectRatioOutput : 0x%X\n", stDiSetting.stNormalMode.eTvAspectRatioOutput);
	HLIB_CMD_Printf("stNormalMode.eVcrAspectRatioOutput : 0x%X\n", stDiSetting.stNormalMode.eVcrAspectRatioOutput);
	HLIB_CMD_Printf("stNormalMode.eTvAspectRatioInput : 0x%X\n", stDiSetting.stNormalMode.eTvAspectRatioInput);
	HLIB_CMD_Printf("stNormalMode.eTvVideoOutput : 0x%X\n", stDiSetting.stNormalMode.eTvVideoOutput);
	HLIB_CMD_Printf("stNormalMode.eVcrVideoOutput : 0x%X\n", stDiSetting.stNormalMode.eVcrVideoOutput);
	HLIB_CMD_Printf("stNormalMode.eTvVideoInput : 0x%X\n", stDiSetting.stNormalMode.eTvVideoInput);

	HLIB_CMD_Printf("==================== end of output status ============================ \n");

	return ERR_OK;
}

STATIC int pal_scart_ProcessCommand_status(void *szArgList)
{
	pal_scart_PrintStatus();

	return HxCMD_OK;
}

#define	hCmdHandle		"pal_scart"

STATIC void pal_scart_RegisterCommand(void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_scart_ProcessCommand_status,
						"status",
						"Print SCART status",
						"status");
}
#endif

STATIC HERROR pal_scart_SetScartRgbToNvram(DxScartFormat_b eScartRGB)
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



void PAL_SCART_Init(void)
{

//	HLIB_LOG_SetLevel (HxLOG_DOMAIN, HxLOG_ALL);

	DI_SCART_RegisterEventNotify(DS_EVENT_VCR_PWR_CHANGE, pal_scart_DiVcrPowerCallback);
	DI_SCART_RegisterEventNotify(DS_EVENT_VCR_AR_CHANGE, pal_scart_DiVcrAspectRatioCallback);

#if defined(CONFIG_DEBUG)
	pal_scart_RegisterCommand();
#endif
}

HERROR PAL_SCART_RegisterCallback(void (*ScartCallbackFunc)(ScartEvent_t eEvent, HUINT32 ulScartId, HUINT32 ulParam1))
{
	PalScart_Callback_t = ScartCallbackFunc;

	return ERR_OK;
}

HERROR PAL_SCART_SetOutput(HBOOL bEnable)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	if(bEnable == TRUE)
	{
		s_bOutputEnable = TRUE;

		PAL_SCART_SetAspecRatio(s_eScartAspectRatio);
		PAL_SCART_SetTvFormat(s_eTvScartFormat);
		PAL_SCART_SetVcrFormat(s_eVcrScartFormat);
		PAL_SCART_SetPath(s_eScartPath);
	}
	else
	{
		PAL_SCART_SetAspecRatio(eDxASPECTRATIO_UNKNOWN);
		s_bOutputEnable = FALSE;
	}

	return ERR_OK;
}

HERROR PAL_SCART_GetPath (DxScartPath_e *pePath)
{
	if(pePath != NULL)
	{
		*pePath = s_eScartPath;
	}

	if (s_bOutputEnable == TRUE)
	{
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL;
	}
}

HERROR PAL_SCART_SetPath (DxScartPath_e ePath)
{
	DI_ERR_CODE				diErr;
	DI_SCART_SETTINGS		di_setting;
	HERROR					hRes = ERR_FAIL;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_ScartPath(ePath));

	if (s_bOutputEnable == FALSE)
	{
		s_eScartPath = ePath;
		hRes = ERR_OK;
		goto FUNC_OUT;
	}

	DI_SCART_GetSettings(&di_setting);

	if(ePath == eDxSCART_PATH_STB2TV)
	{
		di_setting.eMode = DI_SCART_MODE_NORMAL;
	}
	else if(ePath == eDxSCART_PATH_VCR2TV)
	{
		di_setting.eMode = DI_SCART_MODE_BYPASS;
	}
	else if(ePath == eDxSCART_PATH_HWBYPASS)
	{
		di_setting.eMode = DI_SCART_MODE_HW_BYPASS;
	}
	else
	{
		HxLOG_Error("Error ePath (%d) \n", ePath);
		hRes = ERR_FAIL;
		goto FUNC_OUT;
	}

	diErr = DI_SCART_SetSettings(&di_setting);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("DI_SCART_SetSettings ret:0x%X\n", diErr);
		hRes = ERR_FAIL;
		goto FUNC_OUT;
	}
	else
		hRes = ERR_OK;

	s_eScartPath = ePath;

FUNC_OUT:

	return hRes;
}

HERROR PAL_SCART_SetAspecRatio(DxAspectRatio_e eAR)
{
	DI_ERR_CODE				diErr;
	DI_SCART_SETTINGS		di_setting;
	DI_SCART_SETTING_VALUE	eDiScartSettingValue;
	HERROR					hRes = ERR_FAIL;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_AspectRatio(eAR) );

	if (s_bOutputEnable == FALSE)
	{
		s_eScartAspectRatio = eAR;
		hRes = ERR_OK;
		goto FUNC_OUT;
	}

	DI_SCART_GetSettings(&di_setting);

	switch(eAR)
	{
		case eDxASPECTRATIO_UNKNOWN:
			eDiScartSettingValue = DI_SCART_NO_SIGNAL;
			break;

		case eDxASPECTRATIO_4X3:
			eDiScartSettingValue = DI_SCART_AR_4_3;
			break;

		case eDxASPECTRATIO_16X9:
			eDiScartSettingValue = DI_SCART_AR_16_9;
			break;

		default:
			HxLOG_Error("not supported value eAR(%d)\n", eAR);
			hRes = ERR_FAIL;
			goto FUNC_OUT;
			break;
	}

	if( (eDiScartSettingValue != di_setting.stNormalMode.eTvAspectRatioOutput)
		|| (eDiScartSettingValue != di_setting.stNormalMode.eVcrAspectRatioOutput)
	)
	{
		di_setting.stNormalMode.eTvAspectRatioOutput = eDiScartSettingValue;
		di_setting.stNormalMode.eVcrAspectRatioOutput = eDiScartSettingValue;

		diErr = DI_SCART_SetSettings(&di_setting);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("DI_SCART_SetSettings ret:0x%X\n", diErr);
			hRes = ERR_FAIL;
			goto FUNC_OUT;
		}
		else
		{
			HxLOG_Debug("DI_SCART_SetSettings(%d)\n", eDiScartSettingValue);
			hRes = ERR_OK;
		}
	}

	s_eScartAspectRatio = eAR;

FUNC_OUT:

	return hRes;
}

HERROR PAL_SCART_GetTvFormat (DxScartFormat_e *peScartFormat)
{
	if(peScartFormat != NULL)
	{
		*peScartFormat = s_eTvScartFormat;
	}

	if (s_bOutputEnable == TRUE)
	{
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL;
	}
}

HERROR PAL_SCART_SetTvFormat (DxScartFormat_e eScartFormat)
{
	DI_ERR_CODE				diErr;
	DI_SCART_SETTINGS		di_setting;
	HERROR					hRes = ERR_FAIL;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_ScartFormat(eScartFormat) );

	if (s_bOutputEnable == FALSE)
	{
		s_eTvScartFormat = eScartFormat;
		hRes = ERR_OK;
		goto FUNC_OUT;
	}

	DI_SCART_GetSettings(&di_setting);

	switch(eScartFormat)
	{
		case eDxSCART_FORMAT_CVBS:
			di_setting.stNormalMode.eTvVideoOutput = DI_SCART_OUTPUT_CVBS;
			break;
		case eDxSCART_FORMAT_SVIDEO:
			di_setting.stNormalMode.eTvVideoOutput = DI_SCART_OUTPUT_SVIDEO;
			break;
		case eDxSCART_FORMAT_RGB:
			di_setting.stNormalMode.eTvVideoOutput = DI_SCART_OUTPUT_RGB;
			break;
		default:
			HxLOG_Error("not supported value eOutput(0x%X)\n",eScartFormat);
			hRes = ERR_FAIL;
			goto FUNC_OUT;
			break;
	}

	pal_scart_SetScartRgbToNvram(eScartFormat);

	diErr = DI_SCART_SetSettings(&di_setting);
	if(diErr != ERR_OK)
	{
		HxLOG_Error("DI_SCART_SetSettings ret:0x%X\n",diErr);
		hRes = ERR_FAIL;
		goto FUNC_OUT;
	}
	else
		hRes = ERR_OK;

	s_eTvScartFormat = eScartFormat;

FUNC_OUT:

	return hRes;
}

HERROR PAL_SCART_GetVcrFormat (DxScartFormat_e *peScartFormat)
{
	if(peScartFormat != NULL)
	{
		*peScartFormat = s_eVcrScartFormat;
	}

	if (s_bOutputEnable == TRUE)
	{
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL;
	}
}

HERROR PAL_SCART_SetVcrFormat (DxScartFormat_e eScartFormat)
{
	DI_ERR_CODE				diErr;
	DI_SCART_SETTINGS		di_setting;
	HERROR					hRes = ERR_FAIL;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_ScartFormat(eScartFormat) );

	if (s_bOutputEnable == FALSE)
	{
		s_eVcrScartFormat = eScartFormat;
		hRes = ERR_OK;
		goto FUNC_OUT;
	}

	DI_SCART_GetSettings(&di_setting);

	switch(eScartFormat)
	{
		case eDxSCART_FORMAT_CVBS:
			di_setting.stNormalMode.eVcrVideoOutput = DI_SCART_OUTPUT_CVBS;
			break;
		case eDxSCART_FORMAT_SVIDEO:
			di_setting.stNormalMode.eVcrVideoOutput = DI_SCART_OUTPUT_SVIDEO;
			break;
		case eDxSCART_FORMAT_RGB:
			di_setting.stNormalMode.eVcrVideoOutput = DI_SCART_OUTPUT_RGB;
			break;
		default:
			HxLOG_Error("not supported value eOutput(0x%X)\n",eScartFormat);
			hRes = ERR_FAIL;
			goto FUNC_OUT;
			break;
	}

	pal_scart_SetScartRgbToNvram(eScartFormat);

	diErr = DI_SCART_SetSettings(&di_setting);
	if(diErr != ERR_OK)
	{
		HxLOG_Error("DI_SCART_SetSettings ret:0x%X\n",diErr);
		hRes = ERR_FAIL;
		goto FUNC_OUT;
	}
	else
		hRes = ERR_OK;

	s_eVcrScartFormat = eScartFormat;

FUNC_OUT:

	return hRes;
}


/*********************** End of File ******************************/
