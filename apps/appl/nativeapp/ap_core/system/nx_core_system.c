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

/******************************************************************************/
/**
 * @file	  		nx_core_system.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_core_system.h>
#include <nx_core_message.h>
#include <nx_core_av.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

HERROR	NX_SYSTEM_GetVolumeMax(HUINT32 *pulVolumeMax)
{
	return NX_PORT_SYSTEM_GetVolumeMax(pulVolumeMax);
}


HERROR	NX_SYSTEM_GetVolume(HUINT32 *pulVolume)
{
	return NX_PORT_SYSTEM_GetVolume(pulVolume);
}


HERROR	NX_SYSTEM_SetVolume(HUINT32 ulVolume)
{
	return NX_PORT_SYSTEM_SetVolume(ulVolume);
}


HERROR	NX_SYSTEM_GetMute(HBOOL *pbMute)
{
	return NX_PORT_SYSTEM_GetMute(pbMute);
}


HERROR	NX_SYSTEM_SetMute(HBOOL bMute)
{
	return NX_PORT_SYSTEM_SetMute(bMute);
}


HERROR	NX_SYSTEM_GetTime (HUINT32 *pulUnixTime)
{
	if (!pulUnixTime)
	{
		return ERR_FAIL;
	}

#if defined(CONFIG_PROD_OS_EMULATOR)
	VK_CLOCK_GetTime (pulUnixTime);
#else
	*pulUnixTime = (HUINT32)time(NULL);
#endif

	if (*pulUnixTime < NX_VALID_UNIXTIME)
	{
		*pulUnixTime = 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR		NX_SYSTEM_GetMaturityRating(DxRatingAge_e *peRating)
{
	return NX_PORT_SYSTEM_GetMaturityRating(peRating);
}


HERROR		NX_SYSTEM_SetMaturityRating(DxRatingAge_e peRating)
{
	return NX_PORT_SYSTEM_SetMaturityRating(peRating);
}


HERROR		NX_SYSTEM_GetChannelSetupPIN(HBOOL *pbEnabled)
{
	return APK_SCENARIO_GetChannelSetupPIN(pbEnabled);
}


HERROR		NX_SYSTEM_SetChannelSetupPIN(HBOOL pbEnabled)
{
	return APK_SCENARIO_SetChannelSetupPIN(pbEnabled);
}


HERROR		NX_SYSTEM_GetMenuLanuage(HINT32 *lValue)
{
	HCHAR			szLang639[NX_TEXT_SIZE_32] = {0,};
	HERROR			hError = ERR_FAIL;
	HxLANG_Id_e		eLangId = eLangID_English;


	hError = NX_PORT_SYSTEM_GetMainLanguage(szLang639);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_SYSTEM_GetMainLanguage() Fail!!\n");
		return	ERR_FAIL;
	}

	eLangId = HLIB_LANG_639ToId(szLang639);
	*lValue = (HINT32)eLangId;

	return	ERR_OK;
}


HERROR		NX_SYSTEM_SetMenuLanuage(HINT32 lValue)
{
	HCHAR			*pszLanguage = NULL;
	HERROR			hError = ERR_FAIL;
	HxLANG_Id_e		eLangId = eLangID_English;


	eLangId = (HxLANG_Id_e)lValue;
	pszLanguage = (HCHAR *)HLIB_LANG_IdTo639(eLangId);
	if (NULL == pszLanguage)
	{
		NX_APP_Error("Error!!! HLIB_LANG_IdTo639() - eLangId (0x%x)Fail!!\n", eLangId);
		return ERR_FAIL;
	}

	hError = NX_PORT_SYSTEM_SetMainLanguage(pszLanguage);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_SYSTEM_SetMainLanguage() Fail!!\n");
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR		NX_SYSTEM_GetAudioLanuage(HINT32 *lValue)
{
	HCHAR			szLang639[NX_TEXT_SIZE_32] = {0,};
	HERROR			hError = ERR_FAIL;
	HxLANG_Id_e		eLangId = eLangID_English;


	hError = NX_PORT_SYSTEM_GetAudioLanguage(NX_PORT_INDEX_0, szLang639);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_SYSTEM_GetAudioLanguage() Fail!!\n");
		return	ERR_FAIL;
	}

	eLangId = HLIB_LANG_639ToId(szLang639);
	*lValue = (HINT32)eLangId;

	return	ERR_OK;
}


HERROR		NX_SYSTEM_SetAudioLanuage(HINT32 lValue)
{
	HCHAR			*pszLanguage = NULL;
	HERROR			hError = ERR_FAIL;
	HxLANG_Id_e		eLangId = eLangID_English;


	eLangId = (HxLANG_Id_e)lValue;
	pszLanguage = (HCHAR *)HLIB_LANG_IdTo639(eLangId);
	if (NULL == pszLanguage)
	{
		NX_APP_Error("Error!!! HLIB_LANG_IdTo639() Fail!!\n");
		return ERR_FAIL;
	}

	hError = NX_PORT_SYSTEM_SetAudioLanguage(NX_PORT_INDEX_0, pszLanguage);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_SYSTEM_SetAudioLanguage() Fail!!\n");
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR		NX_SYSTEM_GetSecondAudioLanuage(HINT32 *lValue)
{
	HCHAR			szLang639[NX_TEXT_SIZE_32] = {0,};
	HERROR			hError = ERR_FAIL;
	HxLANG_Id_e		eLangId = eLangID_English;


	hError = NX_PORT_SYSTEM_GetAudioLanguage(NX_PORT_INDEX_1, szLang639);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_SYSTEM_GetAudioLanguage() Fail!!\n");
		return	ERR_FAIL;
	}

	eLangId = HLIB_LANG_639ToId(szLang639);
	*lValue = (HINT32)eLangId;

	return	ERR_OK;
}


HERROR		NX_SYSTEM_SetSecondAudioLanuage(HINT32 lValue)
{
	HCHAR			*pszLanguage = NULL;
	HERROR			hError = ERR_FAIL;
	HxLANG_Id_e		eLangId = eLangID_English;


	eLangId = (HxLANG_Id_e)lValue;
	pszLanguage = (HCHAR *)HLIB_LANG_IdTo639(eLangId);
	if (NULL == pszLanguage)
	{
		NX_APP_Error("Error!!! HLIB_LANG_IdTo639() Fail!!\n");
		return ERR_FAIL;
	}

	hError = NX_PORT_SYSTEM_SetAudioLanguage(NX_PORT_INDEX_1, pszLanguage);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_SYSTEM_SetAudioLanguage() Fail!!\n");
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR		NX_SYSTEM_GetSubtitleLanuage(HINT32 *lValue)
{
	HCHAR			szLang639[NX_TEXT_SIZE_4] = {0,};
	HERROR			hError = ERR_FAIL;
	HxLANG_Id_e		eLangId = eLangID_NONE;


	hError = NX_PORT_SYSTEM_GetSubtitleLanguage(NX_PORT_INDEX_0, szLang639);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_SYSTEM_GetSubtitleLanguage() Fail!!\n");
		return	ERR_FAIL;
	}

	eLangId = HLIB_LANG_639ToId(szLang639);
	*lValue = (HINT32)eLangId;

	return	ERR_OK;
}


HERROR		NX_SYSTEM_SetSubtitleLanuage(HINT32 lValue)
{
	HCHAR			*pszLanguage = NULL;
	HCHAR			ucLang639[NX_TEXT_SIZE_4] = {0,};
	HERROR			hError = ERR_FAIL;
	HxLANG_Id_e		eLangId = eLangID_NONE;


	eLangId = (HxLANG_Id_e)lValue;
	pszLanguage = (HCHAR *)HLIB_LANG_IdTo639(eLangId);
	if (NULL == pszLanguage)
	{
		if (eLangId == eLangID_NONE)
		{
			HxSTD_snprintf(ucLang639, NX_TEXT_SIZE_4, "%s", NX_RES_TEXT_OFF_ID);
			hError = NX_PORT_SYSTEM_SetSubtitleLanguage(NX_PORT_INDEX_0, ucLang639);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_PORT_SYSTEM_SetSubtitleLanguage() pszLanguage:(%s) Fail!!\n", pszLanguage);
				return	ERR_FAIL;
			}
		}
		else
		{
			NX_APP_Error("Error!!! NX_SYSTEM_SetSubtitleLanuage() eLangId:(0x%x) Fail!!\n", eLangId);
			return ERR_FAIL;
		}
	}
	else
	{
		hError = NX_PORT_SYSTEM_SetSubtitleLanguage(NX_PORT_INDEX_0, pszLanguage);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_PORT_SYSTEM_SetSubtitleLanguage() pszLanguage:(%s) Fail!!\n", pszLanguage);
			return	ERR_FAIL;
		}
	}

	return	ERR_OK;
}


HERROR		NX_SYSTEM_GetSecondSubtitleLanuage(HINT32 *lValue)
{
	HCHAR			szLang639[NX_TEXT_SIZE_4] = {0,};
	HERROR			hError = ERR_FAIL;
	HxLANG_Id_e		eLangId = eLangID_NONE;


	hError = NX_PORT_SYSTEM_GetSubtitleLanguage(NX_PORT_INDEX_1, szLang639);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_SYSTEM_GetSubtitleLanguage() Fail!!\n");
		return	ERR_FAIL;
	}

	eLangId = HLIB_LANG_639ToId(szLang639);
	*lValue = (HINT32)eLangId;

	return	ERR_OK;
}


HERROR		NX_SYSTEM_SetSecondSubtitleLanuage(HINT32 lValue)
{
	HCHAR			*pszLanguage = NULL;
	HCHAR			ucLang639[NX_TEXT_SIZE_4] = {0,};
	HERROR			hError = ERR_FAIL;
	HxLANG_Id_e		eLangId = eLangID_NONE;


	eLangId = (HxLANG_Id_e)lValue;
	pszLanguage = (HCHAR *)HLIB_LANG_IdTo639(eLangId);
	if (NULL == pszLanguage)
	{
		if (eLangId == eLangID_NONE)
		{
			HxSTD_snprintf(ucLang639, NX_TEXT_SIZE_4, "%s", NX_RES_TEXT_OFF_ID);
			hError = NX_PORT_SYSTEM_SetSubtitleLanguage(NX_PORT_INDEX_1, ucLang639);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_PORT_SYSTEM_SetSubtitleLanguage() pszLanguage:(%s) Fail!!\n", pszLanguage);
				return	ERR_FAIL;
			}
		}
		else
		{
			NX_APP_Error("Error!!! NX_SYSTEM_SetSubtitleLanuage() eLangId:(0x%x) Fail!!\n", eLangId);
			return ERR_FAIL;
		}
	}
	else
	{
		hError = NX_PORT_SYSTEM_SetSubtitleLanguage(NX_PORT_INDEX_1, pszLanguage);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_PORT_SYSTEM_SetSubtitleLanguage() pszLanguage:(%s) Fail!!\n", pszLanguage);
			return	ERR_FAIL;
		}
	}

	return	ERR_OK;
}


HERROR  NX_SYSTEM_GetInfoDisplayTime(HINT32 *plValue)
{
    return  NX_PORT_DB_GetInt(USERDB_SCREENDISP_INFO_DISP_TIME, plValue);
}


HERROR  NX_SYSTEM_SetInfoDisplayTime(HINT32 lValue)
{
    return  NX_PORT_DB_SetInt(USERDB_SCREENDISP_INFO_DISP_TIME, lValue);
}

HERROR  NX_SYSTEM_GetSubtitleFont(HINT32 *plValue)
{
    return  NX_PORT_DB_GetInt(USERDB_SUBTITLE_FONT, plValue);
}

HERROR  NX_SYSTEM_SetSubtitleFont(HINT32 lValue)
{
    return  NX_PORT_DB_SetInt(USERDB_SUBTITLE_FONT, lValue);
}

void	NX_SYSTEM_ShowSubtitle(HBOOL bShow)
{
	ONDK_GWM_SendMessage(NULL, MSG_SIGNAL_SHOWSUBTITLE, HANDLE_NULL, 0, 0, bShow);
}

HERROR	NX_SYSTEM_GetTvAspectRatio(DxAspectRatio_e *peTVAspectRatio)
{
	return	NX_PORT_SYSTEM_GetTvAspectRatio(peTVAspectRatio);
}

HERROR	NX_SYSTEM_SetTvAspectRatio(DxAspectRatio_e eTVAspectRatio)
{
	return	NX_PORT_SYSTEM_SetTvAspectRatio(eTVAspectRatio);
}

HERROR	NX_SYSTEM_GetDisplayFormat(DxDfcSelect_e *peDfcSelect)
{
	return	NX_PORT_SYSTEM_GetDisplayFormat(peDfcSelect);
}

HERROR	NX_SYSTEM_SetDisplayFormat(DxDfcSelect_e eDfcSelect)
{
	return	NX_PORT_SYSTEM_SetDisplayFormat(eDfcSelect);
}

HERROR	NX_SYSTEM_GetResolutionSelect(DxResolutionSelect_e *peResolutionSelect)
{
	return	NX_PORT_SYSTEM_GetResolutionSelect(peResolutionSelect);
}

HERROR	NX_SYSTEM_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect)
{
	return	NX_PORT_SYSTEM_SetResolutionSelect(eResolutionSelect);
}

HERROR	NX_SYSTEM_GetSpdifAudioOutputMode(DxDigitalAudioFormat_e *peSpdifOutput)
{
	return	NX_PORT_SYSTEM_GetSpdifAudioOutputMode(peSpdifOutput);
}

HERROR	NX_SYSTEM_SetSpdifAudioOutputMode(DxDigitalAudioFormat_e eSpdifOutput)
{
	return	NX_PORT_SYSTEM_SetSpdifAudioOutputMode(eSpdifOutput);
}

HERROR	NX_SYSTEM_GetHdmiAudioOutputMode(DxDigitalAudioFormat_e *peHdmiOutput)
{
	return	NX_PORT_SYSTEM_GetHdmiAudioOutputMode(peHdmiOutput);
}

HERROR	NX_SYSTEM_SetHdmiAudioOutputMode(DxDigitalAudioFormat_e eHdmiOutput)
{
	return	NX_PORT_SYSTEM_SetHdmiAudioOutputMode(eHdmiOutput);
}

HERROR	NX_SYSTEM_GetSoundMode(DxStereoSelect_e *peAudioConfig)
{
	return	NX_PORT_SYSTEM_GetSoundMode(peAudioConfig);
}

HERROR	NX_SYSTEM_SetSoundMode(DxStereoSelect_e eAudioConfig)
{
	return	NX_PORT_SYSTEM_SetSoundMode(eAudioConfig);
}

HERROR	NX_SYSTEM_GetLipSyncDelay(HUINT32 *pulLipSyncDelay)
{
	return	NX_PORT_SYSTEM_GetLipSyncDelay(pulLipSyncDelay);
}

HERROR	NX_SYSTEM_SetLipSyncDelay(HUINT32 ulLipSyncDelay)
{
	return	NX_PORT_SYSTEM_SetLipSyncDelay(ulLipSyncDelay);
}

HERROR	NX_SYSTEM_GetOperatorType(DxOperator_e *peOpType)
{
	NX_CHECK_VAR(peOpType);

	return NX_PORT_SYSTEM_GetOperatorType(peOpType);
}

HERROR	NX_SYSTEM_SetOperatorType(DxOperator_e eOpType)
{
	return NX_PORT_SYSTEM_SetOperatorType(eOpType);
}

HERROR	NX_SYSTEM_GetCountryCode(HxCountry_e *peCountryCode)
{
	return NX_PORT_SYSTEM_GetCountryCode(peCountryCode);
}

HERROR	NX_SYSTEM_SetCountryCode(HxCountry_e eCountryCode)
{
	return NX_PORT_SYSTEM_SetCountryCode(eCountryCode);
}

HBOOL	NX_SYSTEM_CheckPinCode(HCHAR *pszPinStr)
{
	return	NX_PORT_SYSTEM_CheckPinCode(pszPinStr);
}

HERROR	NX_SYSTEM_ResetPinCode(HCHAR *pszPinStr)
{
	return	NX_PORT_SYSTEM_ResetPinCode(pszPinStr);
}

HBOOL	NX_SYSTEM_VerifyPinCode(HCHAR *pszPinStr)
{
	return NX_PORT_SYSTEM_PINCODE_Verify(pszPinStr);
}

HERROR	NX_SYSTEM_CheckInstallPinCode(HCHAR *pszPinStr)
{
	HERROR		hError = ERR_FAIL;
	HCHAR		szInstallPinCode[NX_PORT_PIN_DIGIT_MAX] = { 0, };


	hError = NX_PORT_DB_GetStr(USERDB_INSTALLTION_PINCOCE, szInstallPinCode, NX_PORT_PIN_DIGIT_MAX);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_DB_GetStr() - USERDB_INSTALLTION_PINCOCE Fail!! \n");
		return	ERR_FAIL;
	}
	else
	{
		szInstallPinCode[NX_PORT_PIN_DIGIT_MAX-1] = '\0';
		if (HxSTD_StrCmp(pszPinStr, szInstallPinCode) == 0)
		{
			return	ERR_OK;
		}
		else
		{
			NX_APP_Error("Error!!! Invalid Install pin-code !!\n");
			return	ERR_FAIL;
		}
	}
}

HERROR	NX_SYSTEM_SetAvEnable(HBOOL bFlag)
{
	return NX_PORT_SYSTEM_SetAvEnable(bFlag);
}

HERROR  NX_SYSTEM_GetModelName(HCHAR *pszModelName, HINT32 lMax)
{
    return NX_PORT_SYSTEM_GetModelName(pszModelName, lMax);
}

HERROR  NX_SYSTEM_GetSoftwareVersion(HCHAR *pszSoftwareVersion, HINT32 lMax)
{
    return NX_PORT_SYSTEM_GetSoftwareVersion(pszSoftwareVersion, lMax);
}

HERROR  NX_SYSTEM_GetLoaderVersion(HCHAR *pszLoaderVersion, HINT32 lMax)
{
	HERROR	hError = ERR_FAIL;
	HUINT32 	i = 0, j = 0, ulstrlen = 0;

	hError = NX_PORT_SYSTEM_GetLoaderVersion(pszLoaderVersion, lMax);
	ulstrlen = 	HxSTD_StrLen(pszLoaderVersion);

	for (i = 0; i < ulstrlen; i++)
	{
		if (*(pszLoaderVersion + i) >= ' ' && *(pszLoaderVersion + i) <= '~')
		{
			*(pszLoaderVersion + j) = *(pszLoaderVersion + i);
			j++;
		}
	}
	*(pszLoaderVersion + j) = '\0';

	return hError;
}

HERROR  NX_SYSTEM_GetCIPlusVersion(HCHAR *pszVersionName)
{
    return NX_PORT_SYSTEM_GetCIPlusVersion(pszVersionName);
}

HERROR  NX_SYSTEM_GetSystemId(HCHAR *pszSystemId, HINT32 lMax)
{
    return NX_PORT_SYSTEM_GetSystemId(pszSystemId, lMax);
}

HERROR  NX_SYSTEM_GetUpdateDate(HCHAR *pszUpdateDate, HINT32 lMax)
{
    return NX_PORT_SYSTEM_GetUpdateDate(pszUpdateDate, lMax);
}

HERROR  NX_SYSTEM_GetMicomVersion(HCHAR *pszMicomVersion, HINT32 lMax)
{
    return NX_PORT_SYSTEM_GetMicomVersion(pszMicomVersion, lMax);
}

HERROR  NX_SYSTEM_GetSerialNumber(HCHAR *pszSerialNumber, HINT32 lMax)
{
    return NX_PORT_SYSTEM_GetSerialNumber(pszSerialNumber, lMax);
}

HERROR	NX_SYSTEM_GetFusingInfo(HBOOL *bIsSCS, HBOOL *bIsJTAG, HBOOL *bIsOTP, HBOOL *bIsCWE, HUINT32 *pulMarketID)
{
    return NX_PORT_SYSTEM_GetFusingInfo(bIsSCS, bIsJTAG, bIsOTP, bIsCWE, pulMarketID);
}

HERROR	NX_SYSTEM_GetFusingInfoForCasConax(HBOOL *bIsSCS, HBOOL *bIsJTAG, HBOOL *bIsOTP, HBOOL *bIsCWE, HUINT32* pulOTPField1, HUINT32* pulOTPField2)
{
    return NX_PORT_SYSTEM_GetFusingInfoForCasConax(bIsSCS, bIsJTAG, bIsOTP, bIsCWE, pulOTPField1, pulOTPField2);
}


/******************************************************************************/
#define ________________RELATED_HDMI_____________________________________
/******************************************************************************/

HERROR	NX_SYSTEM_GetHdmiSupportCEC(HBOOL *pbSupport)
{
	return	NX_PORT_SYSTEM_GetHdmiSupportCEC(pbSupport);
}

HERROR	NX_SYSTEM_GetHdmiHdcpVersion(DxHdcpVersion_e *peVersion)
{
	return	NX_PORT_SYSTEM_GetHdmiHdcpVersion(peVersion);
}

HERROR	NX_SYSTEM_GetHdmiHdcpStatus(DxHdcpStatus_e *peHdcpStatus)
{
	return	NX_PORT_SYSTEM_GetHdmiHdcpStatus(peHdcpStatus);
}

HERROR	NX_SYSTEM_GetHdmiEdidResolutionCapability(DxResolutionStatus_b *pulResolutionCapability)
{
	return	NX_PORT_SYSTEM_GetHdmiEdidResolutionCapability(pulResolutionCapability);
}

HERROR	NX_SYSTEM_GetHdmiCecMode(HBOOL *pbHdmiCecMode)
{
	return	NX_PORT_SYSTEM_GetHdmiCecMode(pbHdmiCecMode);
}

HERROR	NX_SYSTEM_SetHdmiCecMode(HBOOL bHdmiCecMode)
{
	return	NX_PORT_SYSTEM_SetHdmiCecMode(bHdmiCecMode);
}

HERROR	NX_SYSTEM_GetHdmiCecVolumeMode(HBOOL *pbHdmiCecMode)
{
	return	NX_PORT_SYSTEM_GetHdmiCecVolumeMode(pbHdmiCecMode);
}

HERROR	NX_SYSTEM_SetHdmiCecVolumeMode(HBOOL bHdmiCecMode)
{
	return	NX_PORT_SYSTEM_SetHdmiCecVolumeMode(bHdmiCecMode);
}

HERROR	NX_SYSTEM_GetHdmiIsPlugIn(HBOOL *pbPlugInOut)
{
	return	NX_PORT_SYSTEM_GetHdmiIsPlugIn(pbPlugInOut);
}


/******************************************************************************/
#define ________________RELATED_TUNNER_____________________________________
/******************************************************************************/

HERROR	NX_SYSTEM_GetTunerDeviceNum(HUINT32 *ulTunerNum)
{
	if (ulTunerNum == NULL)
	{
		NX_APP_Error("Error!!! ulTunerNum == NULL !! \n");
		return	ERR_FAIL;
	}

	return	NX_PORT_SYSTEM_GetTunerDeviceNum(ulTunerNum);
}

HERROR	NX_SYSTEM_GetTunerStatus(HINT32 lIdx, HBOOL *bTunerLock)
{
	return	NX_PORT_SYSTEM_GetTunerStatus(lIdx, bTunerLock);
}

HERROR	NX_SYSTEM_GetSignalInfo(HINT32 lIdx, NX_SignalInfo_t *pStSignalInfo)
{
	if (pStSignalInfo == NULL)
	{
		NX_APP_Error("Error!!! pStSignalInfo == NULL !! \n");
		return	ERR_FAIL;
	}

	return	NX_PORT_SYSTEM_GetSignalInfo(lIdx, pStSignalInfo);
}

HERROR	NX_SYSTEM_GetTuneParam(HINT32 lIdx, DxTuneParam_t *pstTuneParam)
{
	if (pstTuneParam == NULL)
	{
		NX_APP_Error("Error!!! pstTuneParam == NULL !! \n");
		return	ERR_FAIL;
	}

	return	NX_PORT_SYSTEM_GetTuneParam(lIdx, pstTuneParam);
}


/******************************************************************************/
#define ________________RELATED_LIFE_CYCLE_____________________________________
/******************************************************************************/

HERROR	NX_SYSTEM_SetAntennaConnection(DxAntennaConnection_e eConnectionType)
{
	return	NX_PORT_SYSTEM_SetAntennaConnection(eConnectionType);
}

HERROR	NX_SYSTEM_GetAntennaConnection(DxAntennaConnection_e *peConnectionType)
{
	return	NX_PORT_SYSTEM_GetAntennaConnection(peConnectionType);
}


/******************************************************************************/
#define ________________RELATED_LIFE_CYCLE_____________________________________
/******************************************************************************/
#if 0
static void nx_SYSTEM_Power_EventListner(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HxLOG_Error("nx_SYSTEM_Power_EventListner event received: %d %d %d\n", ulParam1, ulParam2, ulParam3);

	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_POWER, ulParam3, 0, 0);
}
#endif

HBOOL	NX_SYSTEM_IsStandbyMode(void)
{
	return NX_PORT_SYSTEM_IsStandbyMode();
}

HERROR	NX_SYSTEM_RequestShutDown(void)
{
	return NX_PORT_SYSTEM_RequestShutDown();
}

HERROR	NX_SYSTEM_RequestOperation(void)
{
	return NX_PORT_SYSTEM_RequestOperation();
}

HBOOL	NX_SYSTEM_IsPowerStateStandbyMode(void)
{
	return NX_PORT_SYSTEM_IsPowerStateStandbyMode();
}

HERROR	NX_SYSTEM_GetFirstTimeBoot( HBOOL *pbFirstTimeBoot)
{
	return NX_PORT_SYSTEM_GetFirstTimeBoot(pbFirstTimeBoot);
}

HERROR	NX_SYSTEM_SetFirstTimeBoot( HBOOL bFirstTimeBoot)
{
	return NX_PORT_SYSTEM_SetFirstTimeBoot(bFirstTimeBoot);
}

HERROR	NX_SYSTEM_SetAutomaticPowerdownOnOff(HBOOL bOnOff)
{
	return NX_PORT_SYSTEM_SetAutomaticPowerdownOnOff(bOnOff);
}

HERROR	NX_SYSTEM_GetAutomaticPowerdownOnOff(HBOOL *pbOnOff)
{
	return NX_PORT_SYSTEM_GetAutomaticPowerdownOnOff(pbOnOff);
}

HERROR	NX_SYSTEM_SetStandByPowerMode(HINT32 lStandByPowerMode)
{
	return NX_PORT_SYSTEM_SetStandByPowerMode(lStandByPowerMode);
}

HERROR	NX_SYSTEM_GetStandByPowerMode(HINT32 *plStandByPowerMode)
{
	return NX_PORT_SYSTEM_GetStandByPowerMode(plStandByPowerMode);
}

HERROR	NX_SYSTEM_GetCustomCode(HUINT32 *pulCustomCode)
{
	return NX_PORT_SYSTEM_GetCustomCode(pulCustomCode);
}

HERROR	NX_SYSTEM_GetColdBootDone(HBOOL *pbCheckColdBootDone)
{
	return NX_PORT_SYSTEM_GetColdBootDone(pbCheckColdBootDone);
}

HERROR 	NX_SYSTEM_DoFactoryDefault(void)
{
	NX_AV_StopChannel();
	NX_PORT_SYSTEM_DoFactoryDefault();

	return ERR_OK;
}

#if defined(CONFIG_SUPPORT_IPEPG_APP)
HERROR	NX_SYSTEM_GetEnableIPEpg(HBOOL *pbIPEpgEnable)
{
	return NX_PORT_SYSTEM_GetEnableIPEpg(pbIPEpgEnable);
}

HERROR	NX_SYSTEM_SetEnableIPEpg(HBOOL	bIPEpgEnable)
{
	return NX_PORT_SYSTEM_SetEnableIPEpg(bIPEpgEnable);
}
#endif//CONFIG_SUPPORT_IPEPG_APP

#if defined(CONFIG_MW_MM_PVR)
HERROR	NX_SYSTEM_GetEnableSeriesPlay(HBOOL	*pbSeriesPlayEnable)
{
	return NX_PORT_SYSTEM_GetEnableSeriesPlay(pbSeriesPlayEnable);
}

HERROR	NX_SYSTEM_SetEnableSeriesPlay(HBOOL	bSeriesPlayEnable)
{
	return NX_PORT_SYSTEM_SetEnableSeriesPlay(bSeriesPlayEnable);
}

HERROR	NX_SYSTEM_GetInstantReplayTime(HUINT32 *pulInstantReplayTime)
{
	return NX_PORT_SYSTEM_GetInstantReplayTime(pulInstantReplayTime);
}

HERROR	NX_SYSTEM_SetInstantReplayTime(HUINT32 ulInstantReplayTime)
{
	return NX_PORT_SYSTEM_SetInstantReplayTime(ulInstantReplayTime);
}

HERROR	NX_SYSTEM_GetSkipForwardTime(HUINT32 *pulSkipForwardTime)
{
	return NX_PORT_SYSTEM_GetSkipForwardTime(pulSkipForwardTime);
}

HERROR	NX_SYSTEM_SetSkipForwardTime(HUINT32 ulSkipForwardTime)
{
	return NX_PORT_SYSTEM_SetSkipForwardTime(ulSkipForwardTime);
}

HERROR	NX_SYSTEM_GetEndPaddingTime(HUINT32	*pulEndPaddingTime)
{
	return NX_PORT_SYSTEM_GetEndPaddingTime(pulEndPaddingTime);
}

HERROR	NX_SYSTEM_SetEndPaddingTime(HUINT32	ulEndPaddingTime)
{
	return NX_PORT_SYSTEM_SetEndPaddingTime(ulEndPaddingTime);
}

HERROR	NX_SYSTEM_GetStartPaddingTime(HUINT32 *pulStartPaddingTime)
{
	return NX_PORT_SYSTEM_GetStartPaddingTime(pulStartPaddingTime);
}

HERROR	NX_SYSTEM_SetStartPaddingTime(HUINT32 ulStartPaddingTime)
{
	return NX_PORT_SYSTEM_SetStartPaddingTime(ulStartPaddingTime);
}

HERROR	NX_SYSTEM_GetAutoDelete(HBOOL *pbAutoDelete)
{
	return NX_PORT_SYSTEM_GetAutoDelete(pbAutoDelete);
}

HERROR	NX_SYSTEM_SetAutoDelete(HBOOL bAutoDelete)
{
	return NX_PORT_SYSTEM_SetAutoDelete(bAutoDelete);
}
#endif//CONFIG_MW_MM_PVR

/******************************************************************************/
#define ___________________RELATED_CHANNEL_IMPORT_EXPORT_________________________________
/******************************************************************************/

static void	nx_SYSTEM_ImportExportEventListener(HUINT32 ulType, HUINT32 ulResult)
{
	HxLOG_Warning("nx_SYSTEM_ImportExport_EventListner event received: %u %u\n", ulType, ulResult);
	ONDK_GWM_PostMessage(NULL, MSG_CORE_IMPORT_EXPORT_NOTIFY, 0, (HINT32)ulType, (HINT32)ulResult, 0);
}

HERROR	NX_SYSTEM_StartChannelExport()
{
	NX_PORT_SYSTEM_ExportDBToUsb(nx_SYSTEM_ImportExportEventListener, TRUE);

	return ERR_OK;
}

HERROR	NX_SYSTEM_StartChannelImport()
{
	NX_PORT_SYSTEM_ImportDBFromUsb(nx_SYSTEM_ImportExportEventListener, TRUE);

	return ERR_OK;
}

/******************************************************************************/
#define ___________________RELATED_TIME_________________________________
/******************************************************************************/

HERROR	NX_SYSTEM_GetDayLightSavingTime(HBOOL *pbEnabled)
{
	return NX_PORT_SYSTEM_GetDayLightSavingTime(pbEnabled);
}
HERROR	NX_SYSTEM_SetDayLightSavingTime(HBOOL bEnabled)
{
	return NX_PORT_SYSTEM_SetDayLightSavingTime(bEnabled);
}

HERROR	NX_SYSTEM_GetGMTOffset(HINT32 *pnGmtOffset)
{
	return NX_PORT_SYSTEM_GetGMTOffset(pnGmtOffset);
}

HERROR	NX_SYSTEM_SetGMTOffset(HINT32 nGmtOffset)
{
	return NX_PORT_SYSTEM_SetGMTOffset(nGmtOffset);
}



/******************************************************************************/
#define ___________________Public_functions_________________________________
/******************************************************************************/
HERROR NX_SYSTEM_Init(void)
{
	HERROR hErr = ERR_FAIL;

	NX_PORT_SYSTEM_POWER_RegisterNotifier();
	NX_PORT_SYSTEM_AUDIO_RegisterNotifier();
	NX_PORT_SYSTEM_HDMI_RegisterNotifier();
	NX_PORT_SYSTEM_THERMAL_RegisterNotifier();

	return hErr;
}



/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

