/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_settings.c
	@brief

	Description:  									\n
	Module: node/binding/bind		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <apk.h>
#include <rest_maintask.h>
#include <rest_datatype.h>
#include <rest_bind.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL						 s_bRestBindSetting_Inited = FALSE;
STATIC HxList_t						*s_pstRestBindSetting_Notifier = NULL;

#define ___LOCAL_FUNCTIONS___
STATIC HERROR rest_bind_setting_ReadSettings (RestSetting_t *pstSetting)
{
	HERROR			 hErr;
	
	if (NULL == pstSetting)				{ return ERR_FAIL; }

	(void)rest_datatype_CleanSettings(pstSetting);

	{
		(void)APK_SCENARIO_GetCountryCode((HxCountry_e *)&(pstSetting->nCountry));
		(void)APK_SCENARIO_GetOperatorType((DxOperator_e *)&(pstSetting->nOperator));
		(void)APK_SCENARIO_GetGMTOffset(&(pstSetting->nGmtOffset));
	}

	// Language
	{
		(void)APK_SCENARIO_GetMainLanguage(pstSetting->szMainLangCode);
		(void)APK_SCENARIO_GetAudioLanguage(0, pstSetting->szAudioLangCode);
		(void)APK_SCENARIO_GetSubtitleLanguage(0, pstSetting->szSubtitleLangCode);
	}
	
	// Parental Control
	{
		(void)APK_SCENARIO_GetMaturityRating((DxRatingAge_e *)&(pstSetting->nMaturityRating));
	}

	// Output Video
	{
		HUINT8			 ucRed, ucGreen, ucBlue;
		
		(void)APK_OUTPUT_VIDEO_GetAvEnable((HBOOL *)&(pstSetting->nVideoOnOff));
		(void)APK_OUTPUT_VIDEO_GetResolutionSelect((DxResolutionSelect_e *)&(pstSetting->nResolutionSetting));
		(void)APK_OUTPUT_VIDEO_GetResolutionStatus((DxResolutionStatus_e *)&(pstSetting->nAppliedResolution));
		(void)APK_OUTPUT_VIDEO_GetTvAspectRatio((DxAspectRatio_e *)&(pstSetting->nTvAspectRatio));
		(void)APK_OUTPUT_VIDEO_GetPictureRate((DxPictureRate_e *)&(pstSetting->nPictureRate));
		(void)APK_OUTPUT_VIDEO_GetDfcSelect((DxDfcSelect_e *)&(pstSetting->nDisplayFormatSetting));
		(void)APK_OUTPUT_VIDEO_GetDfcStatus((DxDfcStatus_b *)&(pstSetting->nAppliedDisplayFormat));

		hErr = APK_OUTPUT_VIDEO_GetBackGroundColor(&ucRed, &ucGreen, &ucBlue);
		if (ERR_OK == hErr)
		{
			pstSetting->nBgColorRed = (HINT32)ucRed;
			pstSetting->nBgColorGreen = (HINT32)ucGreen;
			pstSetting->nBgColorBlue = (HINT32)ucBlue;
		}
	}

	// Output Audio
	{
		(void)APK_OUTPUT_AUDIO_GetMasterMute((HBOOL *)&(pstSetting->nAudioMute));
		(void)APK_OUTPUT_AUDIO_GetVolumeMax((HUINT32 *)&(pstSetting->nMaxVolume));
		(void)APK_OUTPUT_AUDIO_GetVolume((HUINT32 *)&(pstSetting->nVolume));
		(void)APK_OUTPUT_AUDIO_GetLipSyncDelay((HUINT32 *)&(pstSetting->nLipSyncDelay));
		(void)APK_OUTPUT_AUDIO_GetAudioDescription((HBOOL *)&(pstSetting->nAudioDescription));
		(void)APK_OUTPUT_AUDIO_GetStereoSelect((DxStereoSelect_e *)&(pstSetting->nStereoSelect));
		(void)APK_OUTPUT_AUDIO_GetDualMonoSelect((DxDualMonoSelect_e *)&(pstSetting->nDualMonoSelect));
	}

	// Composite Port
	{
		(void)APK_OUTPUT_VIDEO_SupportComposite((HBOOL *)&(pstSetting->nCompositeSupport));
		(void)APK_OUTPUT_VIDEO_GetCompositeStandard((DxCompositeStandard_e *)&(pstSetting->nCompositeStandard));
	}

	// Scart Port
	{
		HBOOL		 bSupport;

		bSupport = FALSE;
		(void)APK_OUTPUT_VIDEO_SupportTvScart(&bSupport);
		if (TRUE == bSupport)
		{
			(void)APK_OUTPUT_VIDEO_GetSupportTvScartFormat((DxScartFormat_b *)&(pstSetting->nTvScartSupportedFormat));
			(void)APK_OUTPUT_VIDEO_GetTvScartFormat((DxScartFormat_e *)&(pstSetting->nTvScartFormat));
		}

		bSupport = FALSE;
		(void)APK_OUTPUT_VIDEO_SupportVcrScart(&bSupport);
		if (TRUE == bSupport)
		{
			(void)APK_OUTPUT_VIDEO_GetSupportVcrScartFormat((DxScartFormat_b *)&(pstSetting->nVcrScartSupportedFormat));
			(void)APK_OUTPUT_VIDEO_GetVcrScartFormat((DxScartFormat_e *)&(pstSetting->nVcrScartFormat));
		}

		(void)APK_OUTPUT_VIDEO_GetScartPath((DxScartPath_e *)&(pstSetting->nScartPath));
	}
	
	// HDMI port
	{
		(void)APK_OUTPUT_AUDIO_GetHdmiFormat((DxDigitalAudioFormat_e *)&(pstSetting->nHdmiDigitalAudioFormat));
		(void)APK_OUTPUT_HDMI_SupportCec((HBOOL *)&(pstSetting->nCecSupport));
		(void)APK_OUTPUT_HDMI_GetCecEnable((HBOOL *)&(pstSetting->nCecOnOff));
		(void)APK_OUTPUT_HDMI_GetHdcpStatus((DxHdcpStatus_e *)&(pstSetting->nHdcpState));
		(void)APK_OUTPUT_HDMI_Get3DSelect((DxHdmi3DSelect_e *)&(pstSetting->nHdmi3dSelect));
		(void)APK_OUTPUT_HDMI_Get3DStatus((DxVideo3DFormat_e *)&(pstSetting->nHdmi3dState));
		(void)APK_OUTPUT_HDMI_GetEdid3DFormatCapability((DxVideo3DFormat_e *)&(pstSetting->nEdid3dFormatCap));
		(void)APK_OUTPUT_HDMI_GetEdidResolutionCapability((DxResolutionStatus_b *)&(pstSetting->nEdidResolCap));
	}
	
	// SPDIF
	{
		(void)APK_OUTPUT_AUDIO_SupportSpdif((HBOOL *)&(pstSetting->nSpdifSupport));
		(void)APK_OUTPUT_AUDIO_GetSpdifFormat((DxDigitalAudioFormat_e *)&(pstSetting->nSpdifDigitalAudioFormat));
	}

	// Tuner-related
	{
		(void)APK_SCENARIO_GetAntennaConnection((DxAntennaConnection_e *)&(pstSetting->nAntConnectType));
	}

	return ERR_OK;
}

STATIC HERROR rest_bind_setting_GetAdminSettingFlag (RestSettingAdminFlag_t *pstAdminFlag)
{
	if (NULL == pstAdminFlag)					{ return ERR_FAIL; }

	(void)rest_datatype_CleanAdminSettingFlag(pstAdminFlag);

	pstAdminFlag->nOperator		 	= 1;
	pstAdminFlag->nMaturityRating	= 1;

	return ERR_OK;
}

STATIC HERROR rest_bind_setting_CheckAndChangeSettings (HxJSON_t jsPayLoad, RestSetting_t *pstResult)
{
	RestSetting_t			*pstOrgSettings = NULL, *pstNewSettings = NULL;

	if (NULL == pstResult)					{ return ERR_FAIL; }

	(void)rest_datatype_CleanSettings(pstResult);
	
	pstOrgSettings = (RestSetting_t *)HLIB_STD_MemAlloc(sizeof(RestSetting_t));
	pstNewSettings = (RestSetting_t *)HLIB_STD_MemAlloc(sizeof(RestSetting_t));
	if ((NULL == pstOrgSettings) || (NULL == pstNewSettings))
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}
	
	(void)rest_bind_setting_ReadSettings(pstOrgSettings);
	HxSTD_MemCopy(pstNewSettings, pstOrgSettings, sizeof(RestSetting_t));
	(void)rest_datatype_DejsonizeSettings(jsPayLoad, pstNewSettings, FALSE);

	/*
		Compare and change the new settings
	*/
	{
		if (pstOrgSettings->nCountry != pstNewSettings->nCountry)
		{
			(void)APK_SCENARIO_SetCountryCode((HxCountry_e)pstNewSettings->nCountry);
			pstResult->nCountry = pstNewSettings->nCountry;
		}

		if (pstOrgSettings->nOperator != pstNewSettings->nOperator)
		{
			(void)APK_SCENARIO_SetOperatorType((DxOperator_e)pstNewSettings->nOperator);
			pstResult->nOperator = pstNewSettings->nOperator;
		}
		
		if (pstOrgSettings->nGmtOffset != pstNewSettings->nGmtOffset)
		{
			(void)APK_SCENARIO_SetGMTOffset(pstNewSettings->nGmtOffset);
			pstResult->nGmtOffset = pstNewSettings->nGmtOffset;
		}
	}

	// Language
	{
		if (HxSTD_StrNCmp(pstOrgSettings->szMainLangCode, pstNewSettings->szMainLangCode, 4) != 0)
		{
			(void)APK_SCENARIO_SetMainLanguage((const HCHAR *)pstNewSettings->szMainLangCode);
			HxSTD_StrNCpy(pstResult->szMainLangCode, pstNewSettings->szMainLangCode, 4);
		}
		
		if (HxSTD_StrNCmp(pstOrgSettings->szAudioLangCode, pstNewSettings->szAudioLangCode, 4) != 0)
		{
			(void)APK_SCENARIO_SetAudioLanguage(0, (const HCHAR *)pstNewSettings->szAudioLangCode);
			HxSTD_StrNCpy(pstResult->szAudioLangCode, pstNewSettings->szAudioLangCode, 4);
		}
		
		if (HxSTD_StrNCmp(pstOrgSettings->szSubtitleLangCode, pstNewSettings->szSubtitleLangCode, 4) != 0)
		{
			(void)APK_SCENARIO_SetSubtitleLanguage(0, (const HCHAR *)pstNewSettings->szSubtitleLangCode);
			HxSTD_StrNCpy(pstResult->szSubtitleLangCode, pstNewSettings->szSubtitleLangCode, 4);
		}
	}

	// Parental Control
	{
		if (pstOrgSettings->nMaturityRating != pstNewSettings->nMaturityRating)
		{
			(void)APK_SCENARIO_SetMaturityRating((DxRatingAge_e)pstNewSettings->nMaturityRating);
			pstResult->nMaturityRating = pstNewSettings->nMaturityRating;
		}
	}

	// Output Video
	{
		if (pstOrgSettings->nVideoOnOff != pstNewSettings->nVideoOnOff)
		{
			(void)APK_OUTPUT_VIDEO_SetAvEnable((HBOOL)pstNewSettings->nVideoOnOff);
			pstResult->nVideoOnOff = pstNewSettings->nVideoOnOff;
		}

		if (pstOrgSettings->nResolutionSetting != pstNewSettings->nResolutionSetting)
		{
			(void)APK_OUTPUT_VIDEO_SetResolutionSelect((DxResolutionSelect_e)pstNewSettings->nResolutionSetting);
			pstResult->nResolutionSetting = pstNewSettings->nResolutionSetting;
		}
		
		if (pstOrgSettings->nTvAspectRatio != pstNewSettings->nTvAspectRatio)
		{
			(void)APK_OUTPUT_VIDEO_SetTvAspectRatio((DxAspectRatio_e)pstNewSettings->nTvAspectRatio);
			pstResult->nTvAspectRatio = pstNewSettings->nTvAspectRatio;
		}
		
		if (pstOrgSettings->nDisplayFormatSetting != pstNewSettings->nDisplayFormatSetting)
		{
			(void)APK_OUTPUT_VIDEO_SetDfcSelect((DxDfcSelect_e)pstNewSettings->nDisplayFormatSetting);
			pstResult->nDisplayFormatSetting = pstNewSettings->nDisplayFormatSetting;
		}

		if ((pstOrgSettings->nBgColorRed != pstNewSettings->nBgColorRed) ||
			(pstOrgSettings->nBgColorGreen != pstNewSettings->nBgColorGreen) ||
			(pstOrgSettings->nBgColorBlue != pstNewSettings->nBgColorBlue))
		{
			(void)APK_OUTPUT_VIDEO_SetBackGroundColor((HUINT8)pstNewSettings->nBgColorRed, (HUINT8)pstNewSettings->nBgColorGreen, (HUINT8)pstNewSettings->nBgColorBlue);
			pstResult->nBgColorRed = pstNewSettings->nBgColorRed;
			pstResult->nBgColorGreen = pstNewSettings->nBgColorGreen;
			pstResult->nBgColorBlue = pstNewSettings->nBgColorBlue;
		}
	}
	
	// Output Audio
	{
		if (pstOrgSettings->nAudioMute != pstNewSettings->nAudioMute)
		{
			(void)APK_OUTPUT_AUDIO_SetMasterMute((HBOOL)pstNewSettings->nAudioMute);
			pstResult->nAudioMute = pstNewSettings->nAudioMute;
		}

		if (pstOrgSettings->nVolume != pstNewSettings->nVolume)
		{
			(void)APK_OUTPUT_AUDIO_SetVolume((HUINT32)pstNewSettings->nVolume);
			pstResult->nVolume = pstNewSettings->nVolume;
		}

		if (pstOrgSettings->nLipSyncDelay != pstNewSettings->nLipSyncDelay)
		{
			(void)APK_OUTPUT_AUDIO_SetLipSyncDelay((HUINT32)pstNewSettings->nLipSyncDelay);
			pstResult->nLipSyncDelay = pstNewSettings->nLipSyncDelay;
		}

		if (pstOrgSettings->nAudioDescription != pstNewSettings->nAudioDescription)
		{
			(void)APK_OUTPUT_AUDIO_SetAudioDescription((DxStereoSelect_e)pstNewSettings->nLipSyncDelay);
			pstResult->nAudioDescription = pstNewSettings->nAudioDescription;
		}

		if (pstOrgSettings->nStereoSelect != pstNewSettings->nStereoSelect)
		{
			(void)APK_OUTPUT_AUDIO_SetStereoSelect((DxStereoSelect_e)pstNewSettings->nStereoSelect);
			pstResult->nStereoSelect = pstNewSettings->nStereoSelect;
		}
		
		if (pstOrgSettings->nDualMonoSelect != pstNewSettings->nDualMonoSelect)
		{
			(void)APK_OUTPUT_AUDIO_SetDualMonoSelect((DxDualMonoSelect_e)pstNewSettings->nDualMonoSelect);
			pstResult->nDualMonoSelect = pstNewSettings->nDualMonoSelect;
		}
	}

	// Composite Port
	{
	}

	// Scart Port
	{
		if (pstOrgSettings->nTvScartFormat != pstNewSettings->nTvScartFormat)
		{
			(void)APK_OUTPUT_VIDEO_SetTvScartFormat((DxScartFormat_e)pstNewSettings->nTvScartFormat);
			pstResult->nTvScartFormat = pstNewSettings->nTvScartFormat;
		}

		if (pstOrgSettings->nVcrScartFormat != pstNewSettings->nVcrScartFormat)
		{
			(void)APK_OUTPUT_VIDEO_SetVcrScartFormat((DxScartFormat_e)pstNewSettings->nVcrScartFormat);
			pstResult->nVcrScartFormat = pstNewSettings->nVcrScartFormat;
		}

		if (pstOrgSettings->nScartPath != pstNewSettings->nScartPath)
		{
			(void)APK_OUTPUT_VIDEO_SetScartPath((DxScartPath_e)pstNewSettings->nScartPath);
			pstResult->nScartPath = pstNewSettings->nScartPath;
		}
	}
	
	// HDMI port
	{
		if (pstOrgSettings->nHdmiDigitalAudioFormat != pstNewSettings->nHdmiDigitalAudioFormat)
		{
			(void)APK_OUTPUT_AUDIO_SetHdmiFormat((DxDigitalAudioFormat_e)pstNewSettings->nHdmiDigitalAudioFormat);
			pstResult->nHdmiDigitalAudioFormat = pstNewSettings->nHdmiDigitalAudioFormat;
		}

		if (pstOrgSettings->nCecOnOff != pstNewSettings->nCecOnOff)
		{
			(void)APK_OUTPUT_HDMI_SetCecEnable((HBOOL)pstNewSettings->nCecOnOff);
			pstResult->nCecOnOff = pstNewSettings->nCecOnOff;
		}

		if (pstOrgSettings->nHdmi3dSelect != pstNewSettings->nHdmi3dSelect)
		{
			(void)APK_OUTPUT_HDMI_Set3DSelect((DxHdmi3DSelect_e)pstNewSettings->nHdmi3dSelect);
			pstResult->nHdmi3dSelect = pstNewSettings->nHdmi3dSelect;
		}
	}
	
	// SPDIF
	{
		if (pstOrgSettings->nSpdifDigitalAudioFormat != pstNewSettings->nSpdifDigitalAudioFormat)
		{
			(void)APK_OUTPUT_AUDIO_SetHdmiFormat((DxDigitalAudioFormat_e)pstNewSettings->nSpdifDigitalAudioFormat);
			pstResult->nSpdifDigitalAudioFormat = pstNewSettings->nSpdifDigitalAudioFormat;
		}
	}

	// Tuner-related
	{
		if (pstOrgSettings->nAntConnectType != pstNewSettings->nAntConnectType)
		{
			(void)APK_SCENARIO_SetAntennaConnection((DxAntennaConnection_e)pstNewSettings->nAntConnectType);
			pstResult->nAntConnectType = pstNewSettings->nAntConnectType;
		}
	}


END_FUNC:
	if (NULL != pstOrgSettings)				{ HLIB_STD_MemFree(pstOrgSettings); }
	if (NULL != pstNewSettings)				{ HLIB_STD_MemFree(pstNewSettings); }

	return ERR_OK;
}


#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_setting_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindSetting_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindSetting_Notifier = HLIB_LIST_Append(s_pstRestBindSetting_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_setting_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_setting_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_setting_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindSetting_Notifier = HLIB_LIST_Remove(s_pstRestBindSetting_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_setting_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_setting_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HCHAR *rest_bind_setting_CmdGet (HCHAR *szSessionId, HxJSON_t jsArgs)
{
	HINT32					 nJsonWriter = rest_bind_OpenJsonWriter();
	HCHAR					*szMember = NULL;
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);
	RestSetting_t			*pstSetting = NULL;
	RestSettingAdminFlag_t	*pstAdminFlag = NULL;
	HINT32					 nErrCode = REST_ERR_CODE_OK;
	HCHAR					*szErrString = REST_ERR_STRING_OK;
	HCHAR					*szResult = NULL;

	if (0 == nJsonWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;				szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		nErrCode = REST_ERR_CODE_INVALID_SESSION;		szErrString = REST_ERR_STRING_INVALID_SESSION;
		goto END_FUNC;
	}

	pstSetting = (RestSetting_t *)HLIB_STD_MemAlloc(sizeof(RestSetting_t));
	if (NULL == pstSetting)
	{
		HxLOG_Error("memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;				szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	pstAdminFlag = (RestSettingAdminFlag_t *)HLIB_STD_MemAlloc(sizeof(RestSettingAdminFlag_t));
	if (NULL == pstSetting)
	{
		HxLOG_Error("memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;				szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	if (NULL != jsArgs)
	{
		szMember = (HCHAR *)HLIB_JSON_Object_GetString(jsArgs, "member");
	}
	
	(void)rest_bind_setting_ReadSettings(pstSetting);
	(void)rest_bind_setting_GetAdminSettingFlag(pstAdminFlag);

	HxJSONWRITER_ObjectBegin(nJsonWriter);
	{
		// Settings
		rest_datatype_JsonizeSettings(nJsonWriter, pstSetting, "settings", szMember);
		
		// Administrator Setting Flags
		rest_datatype_JsonizeAdminSettingItems(nJsonWriter, pstAdminFlag, "adminItems", szMember);

		// Error
		(void)rest_bind_MakeErrorJsWriter(nJsonWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJsonWriter);
	
	szResult = rest_bind_CloseJsonWriter(nJsonWriter);

END_FUNC:
	if (NULL != pstSetting)					{ HLIB_STD_MemFree(pstSetting); }
	if (NULL != pstAdminFlag)				{ HLIB_STD_MemFree(pstAdminFlag); }
	if (NULL == szResult)
	{
		szResult = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResult;
}

STATIC HCHAR *rest_bind_setting_CmdPut (HCHAR *szSessionId, HxJSON_t jsPayLoad)
{
	HINT32					 nJsonWriter = rest_bind_OpenJsonWriter();
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);
	RestSetting_t			*pstSetting = NULL;
	HINT32					 nErrCode = REST_ERR_CODE_OK;
	HCHAR					*szErrString = REST_ERR_STRING_OK;
	HCHAR					*szResult = NULL;

	if (0 == nJsonWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;				szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		nErrCode = REST_ERR_CODE_INVALID_SESSION;		szErrString = REST_ERR_STRING_INVALID_SESSION;
		goto END_FUNC;
	}

	pstSetting = (RestSetting_t *)HLIB_STD_MemAlloc(sizeof(RestSetting_t));
	if (NULL == pstSetting)
	{
		HxLOG_Error("memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;				szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	(void)rest_bind_setting_CheckAndChangeSettings(jsPayLoad, pstSetting);

	HxJSONWRITER_ObjectBegin(nJsonWriter);
	{
		// Settings
		rest_datatype_JsonizeSettings(nJsonWriter, pstSetting, "changedSettings", NULL);
		
		// Error
		(void)rest_bind_MakeErrorJsWriter(nJsonWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJsonWriter);
	
	szResult = rest_bind_CloseJsonWriter(nJsonWriter);

END_FUNC:
	if (NULL != pstSetting)					{ HLIB_STD_MemFree(pstSetting); }
	if (NULL == szResult)
	{
		szResult = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResult;
}

STATIC HERROR rest_bind_setting_CbGet (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsArgs;
	HCHAR				*szResult;

	jsArgs = HLIB_JSON_Decode((const HCHAR *)pstMsg->aulArgs[0]);
	szResult = rest_bind_setting_CmdGet(pstMsg->szSessionId, jsArgs);
	pstMsg->pvResult = (void *)szResult;

	if (NULL != jsArgs)				{ HLIB_JSON_Delete(jsArgs); }
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_setting_CbPut (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsPayload;
	HCHAR				*szResult;

	jsPayload = HLIB_JSON_Decode((const HCHAR *)pstMsg->aulArgs[0]);
	szResult = rest_bind_setting_CmdPut(pstMsg->szSessionId, jsPayload);
	pstMsg->pvResult = (void *)szResult;

	if (NULL != jsPayload)			{ HLIB_JSON_Delete(jsPayload); }
	rest_task_SyncOut();

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_setting_Init (void)
{
	if (TRUE != s_bRestBindSetting_Inited)
	{
		// Initialize the APPKIT
		(void)APK_SCENARIO_Init();
		(void)APK_OUTPUT_VIDEO_Init();
		(void)APK_OUTPUT_AUDIO_Init();
		(void)APK_OUTPUT_HDMI_Init();

		s_bRestBindSetting_Inited = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___
int REST_BIND_SETTINGS_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_setting_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_SETTINGS_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;
	
	hErr = rest_task_RequestToExecute(rest_bind_setting_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

// GET http://127.0.0.1:37564/stb-api/v1/settings
char *REST_BIND_SETTINGS_Get (char *szSessionId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szArgJson;
	
	hErr = rest_task_RequestToExecute(rest_bind_setting_CbGet, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		goto ERROR;
	}

	rest_task_SyncIn();

	szResult = (char *)stMsg.pvResult;
	if (NULL == szResult)
	{
		HxLOG_Error("cmd not executed:\n");
		goto ERROR;
	}

	rest_datatype_AppendStringBufferToTemporary(szResult);
	return szResult;

ERROR:
	return "";
}

// PUT http://127.0.0.1:37564/stb-api/v1/settings
char *REST_BIND_SETTINGS_Put (char *szSessionId, char *szPayload)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szPayload;
	
	hErr = rest_task_RequestToExecute(rest_bind_setting_CbPut, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		goto ERROR;
	}

	rest_task_SyncIn();

	szResult = (char *)stMsg.pvResult;
	if (NULL == szResult)
	{
		HxLOG_Error("cmd not executed:\n");
		goto ERROR;
	}

	rest_datatype_AppendStringBufferToTemporary(szResult);
	return szResult;

ERROR:
	return "";
}



