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
 * @file	  		nx_port_system.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_port.h>



/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

#define ________________RELATED_SYSTEM_VOLUME_________________
HERROR NX_PORT_SYSTEM_GetVolumeMax(HUINT32 *pulVolumeMax)
{
	NX_CHECK_VAR(pulVolumeMax);

	if (APK_OUTPUT_AUDIO_GetVolumeMax((unsigned int *)pulVolumeMax) != ERR_OK)
	{
		HxLOG_Error("Getting the voulme info is failed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR NX_PORT_SYSTEM_GetVolume(HUINT32 *pulVolume)
{
	NX_CHECK_VAR(pulVolume);

	APK_OUTPUT_AUDIO_GetVolume(pulVolume);

	return ERR_OK;
}

HERROR NX_PORT_SYSTEM_SetVolume(HUINT32 ulVolume)
{
	APK_OUTPUT_AUDIO_SetVolume(ulVolume);

	return ERR_OK;
}

HERROR NX_PORT_SYSTEM_GetMute(HBOOL *pbMute)
{
	NX_CHECK_VAR(pbMute);

	APK_OUTPUT_AUDIO_GetMasterMute(pbMute);

	return ERR_OK;
}

HERROR NX_PORT_SYSTEM_SetMute(HBOOL bMute)
{
	APK_OUTPUT_AUDIO_SetMasterMute(bMute);

	return ERR_OK;
}


#define ________________RELATED_SYSTEM_INFO________________________

HERROR  NX_PORT_SYSTEM_GetModelName(HCHAR *pszModelName, HINT32 lMax)
{
    return APK_SYSINFO_STB_GetModelName(pszModelName, lMax);
}

HERROR  NX_PORT_SYSTEM_GetSoftwareVersion(HCHAR *pszSoftwareVersion, HINT32 lMax)
{
    return APK_SYSINFO_STB_GetSoftwareVersion(pszSoftwareVersion, lMax);
}

HERROR  NX_PORT_SYSTEM_GetLoaderVersion(HCHAR *pszLoaderVersion, HINT32 lMax)
{
    return APK_SYSINFO_STB_GetLoaderVersion(pszLoaderVersion, lMax);
}

HERROR  NX_PORT_SYSTEM_GetCIPlusVersion(HCHAR *pszVersionName)
{
    return APK_CAS_CI_GetCIPlusVersionName(pszVersionName);
}

HERROR  NX_PORT_SYSTEM_GetSystemId(HCHAR *pszSystemId, HINT32 lMax)
{
    return APK_SYSINFO_STB_GetSystemId(pszSystemId, lMax);
}

HERROR  NX_PORT_SYSTEM_GetUpdateDate(HCHAR *pszUpdateDate, HINT32 lMax)
{
    return APK_SYSINFO_STB_GetUpdateDate(pszUpdateDate, lMax);
}

HERROR  NX_PORT_SYSTEM_GetMicomVersion(HCHAR *pszMicomVersion, HINT32 lMax)
{
    return APK_SYSINFO_STB_GetMicomVersion(pszMicomVersion, lMax);
}

HERROR  NX_PORT_SYSTEM_GetSerialNumber(HCHAR *pszSerialNumber, HINT32 lMax)
{
    return APK_SYSINFO_STB_GetSerialNumber(pszSerialNumber, lMax);
}

HERROR	NX_PORT_SYSTEM_GetFusingInfo(HBOOL *bIsSCS, HBOOL *bIsJTAG, HBOOL *bIsOTP, HBOOL *bIsCWE, HUINT32 *pulMarketID)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		ucSCS = 0, ucJTAG = 0, ucOTP = 0, ucCWE = 0;
	HUINT32		ulMarketID = 0;


	hError = APK_SYSINFO_STB_GetFusingInfo(&ucSCS, &ucJTAG, &ucOTP, &ucCWE, &ulMarketID);

	*bIsSCS = (HBOOL)ucSCS ? TRUE : FALSE;
	*bIsJTAG = (HBOOL)ucJTAG? TRUE : FALSE;
	*bIsOTP = (HBOOL)ucOTP? TRUE : FALSE;
	*bIsCWE = (HBOOL)ucCWE? TRUE : FALSE;
	*pulMarketID = ulMarketID;

	return hError;
}

HERROR	NX_PORT_SYSTEM_GetFusingInfoForCasConax(HBOOL *bIsSCS, HBOOL *bIsJTAG, HBOOL *bIsOTP, HBOOL *bIsCWE, HUINT32* pulOTPField1, HUINT32* pulOTPField2)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		ucSCS = 0, ucJTAG = 0, ucOTP = 0, ucCWE = 0;

	hError = APK_SYSINFO_STB_GetFusingInfoExt(&ucSCS, &ucJTAG, &ucOTP, &ucCWE, pulOTPField1, pulOTPField2);

	*bIsSCS = (HBOOL)ucSCS ? TRUE : FALSE;
	*bIsJTAG = (HBOOL)ucJTAG? TRUE : FALSE;
	*bIsOTP = (HBOOL)ucOTP? TRUE : FALSE;
	*bIsCWE = (HBOOL)ucCWE? TRUE : FALSE;

	return hError;
}


#define ________________RELATED_SCENARIO________________

HERROR	NX_PORT_SYSTEM_GetMaturityRating(DxRatingAge_e *peRating)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_SCENARIO_GetMaturityRating(peRating);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_SCENARIO_GetMaturityRating ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR	NX_PORT_SYSTEM_SetMaturityRating(DxRatingAge_e peRating)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_SCENARIO_SetMaturityRating(peRating);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_SCENARIO_SetMaturityRating ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR	NX_PORT_SYSTEM_GetBlockCHSetting(HBOOL *pbEnabled)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_SCENARIO_GetChannelSetupPIN(pbEnabled);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_SCENARIO_GetChannelSetupPIN ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetBlockCHSetting(HBOOL pbEnabled)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_SCENARIO_SetChannelSetupPIN(pbEnabled);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_SCENARIO_SetChannelSetupPIN ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

HERROR	NX_PORT_SYSTEM_GetMainLanguage(HCHAR *pszLanguage)
{
	HERROR		hError = ERR_FAIL;

	NX_CHECK_VAR(pszLanguage);

	hError = APK_SCENARIO_GetMainLanguage(pszLanguage);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! APK_SCENARIO_GetMainLanguage() \n");
	}
	pszLanguage[NX_PORT_LANGUAGE_CODE - 1] = '\0';

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetMainLanguage(const HCHAR *pszLanguage)
{
	HERROR		hError = ERR_FAIL;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	NX_CHECK_VAR(pszLanguage);

	if (pszLanguage == NULL)
	{
		HxLOG_Error("Error!!! (pszLanguage == NULL) \n");
		return ERR_FAIL;
	}

	eResult = ONDK_SetLanguage((HINT8*)pszLanguage);
	if (eResult != ONDK_RET_OK)
	{
		HxLOG_Error("Error!!! ONDK_SetLanguage() - pszLanguage:(%s) \n", pszLanguage);
	}

	hError = APK_SCENARIO_SetMainLanguage(pszLanguage);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! APK_SCENARIO_SetMainLanguage() - pszLanguage:(%s)\n");
	}

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_GetAudioLanguage(HINT32 lIndex, HCHAR *pszLanguage)
{
	HERROR		hError = ERR_FAIL;

	NX_CHECK_VAR(pszLanguage);

	hError = APK_SCENARIO_GetAudioLanguage(lIndex, pszLanguage);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! APK_SCENARIO_GetAudioLanguage() - lIndex:(%d)\n", lIndex);
	}
	pszLanguage[NX_PORT_LANGUAGE_CODE - 1] = '\0';

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetAudioLanguage(HINT32 lIndex, const HCHAR *pszLanguage)
{
	HERROR		hError = ERR_FAIL;

	NX_CHECK_VAR(pszLanguage);

	if (pszLanguage == NULL)
	{
		HxLOG_Error("Error!!! (pszLanguage == NULL) \n");
		return ERR_FAIL;
	}

	hError = APK_SCENARIO_SetAudioLanguage(lIndex, pszLanguage);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! APK_SCENARIO_SetAudioLanguage() - lIndex:(%d), pszLanguage:(%s)\n", lIndex, pszLanguage);
	}

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_GetSubtitleLanguage(HINT32 lIndex, HCHAR *pszLanguage)
{
	HERROR		hError = ERR_FAIL;

	NX_CHECK_VAR(pszLanguage);

	hError = APK_SCENARIO_GetSubtitleLanguage(lIndex, pszLanguage);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! APK_SCENARIO_GetSubtitleLanguage() - lIndex:(%d) \n", lIndex);
	}
	pszLanguage[NX_PORT_LANGUAGE_CODE-1] = '\0';

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetSubtitleLanguage(HINT32 lIndex, const HCHAR *pszLanguage)
{
	HERROR		hError = ERR_FAIL;

	NX_CHECK_VAR(pszLanguage);

	if (pszLanguage == NULL)
	{
		HxLOG_Error("Error!!! (pszLanguage == NULL) \n");
		return ERR_FAIL;
	}

	hError = APK_SCENARIO_SetSubtitleLanguage(lIndex, pszLanguage);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! APK_SCENARIO_SetSubtitleLanguage() - lIndex:(%d), pszLanguage:(%s)\n", lIndex, pszLanguage);
	}

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_GetSubtitleSetting(HBOOL *pbSubtitle)
{
	return APK_SCENARIO_GetSubtitleSetting(pbSubtitle);
}

HERROR	NX_PORT_SYSTEM_SetSubtitleSetting(HBOOL bSubtitle)
{
	return APK_SCENARIO_SetSubtitleSetting(bSubtitle);
}

HERROR	NX_PORT_SYSTEM_GetOperatorType(DxOperator_e *peOpType)
{
	NX_CHECK_VAR(peOpType);

	return APK_SCENARIO_GetOperatorType(peOpType);
}

HERROR 	NX_PORT_SYSTEM_SetOperatorType(DxOperator_e eOpType)
{
	return APK_SCENARIO_SetOperatorType(eOpType);
}

HERROR	NX_PORT_SYSTEM_GetCountryCode(HxCountry_e *peCountryCode)
{
	return APK_SCENARIO_GetCountryCode(peCountryCode);
}

HERROR	NX_PORT_SYSTEM_SetCountryCode(HxCountry_e eCountryCode)
{
	return APK_SCENARIO_SetCountryCode(eCountryCode);
}

HERROR	NX_PORT_SYSTEM_GetTvAspectRatio(DxAspectRatio_e *peTVAspectRatio)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_VIDEO_GetTvAspectRatio(peTVAspectRatio);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_GetResolutionSelect ret Error:0x%X.\n", hError);
		*peTVAspectRatio = eDxASPECTRATIO_16X9;
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetTvAspectRatio(DxAspectRatio_e eTVAspectRatio)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_VIDEO_SetTvAspectRatio(eTVAspectRatio);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_SetTvAspectRatio ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}
	return	ERR_OK;
}


HERROR	NX_PORT_SYSTEM_GetDisplayFormat(DxDfcSelect_e *peDfcSelect)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_VIDEO_GetDfcSelect(peDfcSelect);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_GetResolutionSelect ret Error:0x%X.\n", hError);
		*peDfcSelect = eDxDFC_SELECT_DEFAULT;
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR	NX_PORT_SYSTEM_SetDisplayFormat(DxDfcSelect_e eDfcSelect)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_VIDEO_SetDfcSelect(eDfcSelect);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_SetDfcSelect ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR	NX_PORT_SYSTEM_GetResolutionSelect(DxResolutionSelect_e *peResolutionSelect)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_VIDEO_GetResolutionSelect(peResolutionSelect);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_GetResolutionSelect ret Error:0x%X.\n", hError);
		*peResolutionSelect = eDxRESOLUTION_STATUS_1080I;
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_VIDEO_SetResolutionSelect(eResolutionSelect);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_SetResolutionSelect ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR	NX_PORT_SYSTEM_GetSpdifAudioOutputMode(DxDigitalAudioFormat_e *peSpdifOutput)
{
	HERROR		hError = ERR_FAIL;

	*peSpdifOutput = eDxDIGITALAUDIO_FORMAT_UNKNOWN;

	hError = APK_OUTPUT_AUDIO_GetSpdifFormat(peSpdifOutput);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_AUDIO_GetSpdifFormat ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetSpdifAudioOutputMode(DxDigitalAudioFormat_e eSpdifOutput)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_AUDIO_SetSpdifFormat(eSpdifOutput);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_AUDIO_SetSpdifFormat ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

HERROR	NX_PORT_SYSTEM_GetHdmiAudioOutputMode(DxDigitalAudioFormat_e *peHdmiOutput)
{
	HERROR		hError = ERR_FAIL;

	*peHdmiOutput = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	hError = APK_OUTPUT_AUDIO_GetHdmiFormat(peHdmiOutput);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_AUDIO_GetHdmiFormat ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetHdmiAudioOutputMode(DxDigitalAudioFormat_e eHdmiOutput)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_AUDIO_SetHdmiFormat(eHdmiOutput);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_AUDIO_SetHdmiFormat ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR	NX_PORT_SYSTEM_GetSoundMode(DxStereoSelect_e *peAudioConfig)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_AUDIO_GetStereoSelect(peAudioConfig);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_AUDIO_GetStereoSelect ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR	NX_PORT_SYSTEM_SetSoundMode(DxStereoSelect_e eAudioConfig)
{
	HERROR		hError = ERR_FAIL;
#if defined(CONFIG_APUI_SET_DUALMONO_IN_SOUNDMODE)
	DxDualMonoSelect_e		eDualmonoConfig;
#endif

	hError = APK_OUTPUT_AUDIO_SetStereoSelect(eAudioConfig);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_AUDIO_SetStereoSelect ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

#if defined(CONFIG_APUI_SET_DUALMONO_IN_SOUNDMODE)
	switch (eAudioConfig)
	{
		case eDxSTEREO_SELECT_MONOLEFT:		eDualmonoConfig = eDxDUALMONO_SELECT_LEFT;		break;
		case eDxSTEREO_SELECT_MONORIGHT:	eDualmonoConfig = eDxDUALMONO_SELECT_RIGHT;		break;
		case eDxSTEREO_SELECT_UNKNOWN:		eDualmonoConfig = eDxDUALMONO_SELECT_UNKNOWN;	break;
		default:							eDualmonoConfig = eDxDUALMONO_SELECT_BOTH;		break;
	}

	hError = APK_OUTPUT_AUDIO_SetDualMonoSelect(eAudioConfig);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_AUDIO_SetDualMonoSelect ret Error:0x%X.\n", hError);
	}
#endif

	return	ERR_OK;
}


HERROR	NX_PORT_SYSTEM_GetLipSyncDelay(HUINT32 *pulLipSyncDelay)
{
	HUINT32		ulLipSyncDelay = 0;
	HERROR		hError = ERR_FAIL;

	*pulLipSyncDelay = 0;
	hError = APK_OUTPUT_AUDIO_GetLipSyncDelay(&ulLipSyncDelay);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_AUDIO_GetLipSyncDelay ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	*pulLipSyncDelay = ulLipSyncDelay;

	return	ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetLipSyncDelay(HUINT32 ulLipSyncDelay)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_AUDIO_SetLipSyncDelay(ulLipSyncDelay);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_OUTPUT_AUDIO_SetLipSyncDelay ret Error:0x%X.\n", hError);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HBOOL	NX_PORT_SYSTEM_CheckPinCode(HCHAR *pszPinStr)
{
	HBOOL	bChecked;

	bChecked = APK_SCENARIO_PINCODE_Check(pszPinStr);
	if (TRUE == bChecked)
	{
		HxLOG_Error("APK_SCENARIO_PINCODE_Check ret  bChecked:0x%X.\n", bChecked);
		return	TRUE;
	}

	return	FALSE;
}

HBOOL	NX_PORT_SYSTEM_PINCODE_Verify(const HCHAR *pszPinStr)
{
	return APK_SCENARIO_PINCODE_Verify(pszPinStr);
}

HERROR	NX_PORT_SYSTEM_ResetPinCode(HCHAR *pszPinStr)
{
	HBOOL	bChangePinCode;

	bChangePinCode = APK_SCENARIO_PINCODE_Reset(pszPinStr);
	if (TRUE == bChangePinCode)
	{
		HxLOG_Error("NX_PORT_SYSTEM_SetPinCode ret  bChangePinCode:0x%X.\n", bChangePinCode);
		return	TRUE;
	}

	return	FALSE;
}

HERROR	NX_PORT_SYSTEM_SetAvEnable(HBOOL bFlag)
{
	return	APK_OUTPUT_VIDEO_SetAvEnable(bFlag);
}

HERROR	NX_PORT_SYSTEM_GetLastServiceGroup(HINT32 *pnLastSvcGroup)
{
	return	APK_SCENARIO_GetLastServiceGroup(pnLastSvcGroup);
}

HERROR	NX_PORT_SYSTEM_SetLastServiceGroup(HINT32 nLastSvcGroup)
{
	return APK_SCENARIO_SetLastServiceGroup(nLastSvcGroup);
}

HERROR	NX_PORT_SYSTEM_GetAutomaticPowerdownOnOff(HBOOL *pbOnOff)
{
	NX_CHECK_VAR(pbOnOff);
	return  NX_PORT_DB_GetInt(USERDB_SYSTEM_AUTOMATIC_POWER_DOWN, pbOnOff);
}

HERROR	NX_PORT_SYSTEM_SetAutomaticPowerdownOnOff(HBOOL bOnOff)
{
	return	NX_PORT_DB_SetInt(USERDB_SYSTEM_AUTOMATIC_POWER_DOWN, bOnOff);
}

HERROR	NX_PORT_SYSTEM_GetStandByPowerMode(HINT32 *plStandByPowerMode)
{
	HERROR	hError = ERR_FAIL;
	APK_POWER_BoxStandbyType_e	val = eAPK_POWER_STANDBYTYPE_NORMAL;

	hError = APK_POWER_GetStandbyPowerMode(&val);
	*plStandByPowerMode = (HINT32)val;

	return hError;
}

HERROR	NX_PORT_SYSTEM_SetStandByPowerMode(HINT32 lStandByPowerMode)
{
	HERROR	hError = ERR_FAIL;

	hError = APK_POWER_SetStandbyPowerMode(lStandByPowerMode);

	return hError;
}

HERROR 	NX_PORT_SYSTEM_GetSWUpdateVersion(HCHAR *pszSWUpdateVersion, HINT32 nMax)
{
	return APK_SCENARIO_GetSWUpdateVersion(pszSWUpdateVersion, nMax);
}


/******************************************************************************/
#define ________________RELATED_HDMI___________________________________________
/******************************************************************************/

HERROR	NX_PORT_SYSTEM_GetHdmiSupportCEC(HBOOL *pbSupport)
{
	return	APK_OUTPUT_HDMI_SupportCec(pbSupport);
}

HERROR	NX_PORT_SYSTEM_GetHdmiHdcpVersion(DxHdcpVersion_e *peVersion)
{
	return	APK_OUTPUT_HDMI_GetHdcpVersion(peVersion);
}

HERROR	NX_PORT_SYSTEM_GetHdmiHdcpStatus(DxHdcpStatus_e *peHdcpStatus)
{
	return	APK_OUTPUT_HDMI_GetHdcpStatus(peHdcpStatus);
}

HERROR	NX_PORT_SYSTEM_GetHdmiEdidResolutionCapability(DxResolutionStatus_b *pulResolutionCapability)
{
	return	APK_OUTPUT_HDMI_GetEdidResolutionCapability(pulResolutionCapability);
}

HERROR	NX_PORT_SYSTEM_GetHdmiCecMode(HBOOL *pbHdmiCecMode)
{
	HERROR		hError = ERR_FAIL;

	if (pbHdmiCecMode == NULL)
	{
		HxLOG_Error("Error!!! NX_PORT_SYSTEM_GetHdmiCecMode() - (pbHdmiCecMode == NULL) \n");
		return ERR_FAIL;
	}

	hError = APK_OUTPUT_HDMI_GetCecEnable((HBOOL *)pbHdmiCecMode);
	if (hError != ERR_OK)
	{
		*pbHdmiCecMode = FALSE;
		HxLOG_Error("Error!!! APK_OUTPUT_HDMI_GetCecEnable() Fail!! \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetHdmiCecMode(HBOOL bHdmiCecMode)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_HDMI_SetCecEnable(bHdmiCecMode);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! APK_OUTPUT_HDMI_SetCecEnable() Fail!! \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_GetHdmiCecVolumeMode(HBOOL *pbHdmiCecVolumeMode)
{
	HERROR		hError = ERR_FAIL;

	if (pbHdmiCecVolumeMode == NULL)
	{
		HxLOG_Error("Error!!! NX_PORT_SYSTEM_GetHdmiCecVolumeMode() - (pbHdmiCecMode == NULL) \n");
		return ERR_FAIL;
	}

	hError = APK_OUTPUT_HDMI_GetCecVolumeCtrlEnable((HBOOL *)pbHdmiCecVolumeMode);
	if (hError != ERR_OK)
	{
		*pbHdmiCecVolumeMode = FALSE;
		HxLOG_Error("Error!!! APK_OUTPUT_HDMI_GetCecVolumeCtrlEnable() Fail!! \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_SetHdmiCecVolumeMode(HBOOL bHdmiCecVolumeMode)
{
	HERROR		hError = ERR_FAIL;

	hError = APK_OUTPUT_HDMI_SetCecVolumeCtrlEnable(bHdmiCecVolumeMode);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! APK_OUTPUT_HDMI_SetCecVolumeCtrlEnable() Fail!! \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_GetHdmiIsPlugIn(HBOOL *pbPlugInOut)
{
	return	APK_OUTPUT_HDMI_IsPlugIn(pbPlugInOut);
}


/******************************************************************************/
#define ________________RELATED_TUNNER_____________________________________
/******************************************************************************/
HERROR	NX_PORT_SYSTEM_GetTunerDeviceNum(HUINT32 *ulTunerNum)
{
	return	APK_SYSINFO_TUNER_GetDeviceNum(ulTunerNum);
}


HERROR	NX_PORT_SYSTEM_GetTunerStatus(HINT32 lIdx, HBOOL *bTunerLock)
{
	return	APK_SYSINFO_TUNER_IsLocked(lIdx, bTunerLock);
}


HERROR	NX_PORT_SYSTEM_GetSignalInfo(HINT32 lIdx, NX_SignalInfo_t *pStSignalInfo)
{
	APK_SYSINFO_TUNER_GetSignalLevel(lIdx, &(pStSignalInfo->ulSignalLevel));
	APK_SYSINFO_TUNER_GetSignalQuality(lIdx, &(pStSignalInfo->ulSignalQuality));
	APK_SYSINFO_TUNER_GetAgc(lIdx, &(pStSignalInfo->ulAgc));
	APK_SYSINFO_TUNER_GetBer(lIdx, &(pStSignalInfo->fBer));
	APK_SYSINFO_TUNER_GetSnr(lIdx, &(pStSignalInfo->fSnr));
	APK_SYSINFO_TUNER_GetInputPower(lIdx, &(pStSignalInfo->fSignalInputPower));

	HxLOG_Info("===== [ Signal Info ] ===== \n");
	HxLOG_Info("===== Level : (%u) =====\n", pStSignalInfo->ulSignalLevel);
	HxLOG_Info("===== Quality : (%u) =====\n", pStSignalInfo->ulSignalQuality);
	HxLOG_Info("===== ulAgc : (%u) =====\n", pStSignalInfo->ulAgc);
	HxLOG_Info("===== fBer : (%f) =====\n", pStSignalInfo->fBer);
	HxLOG_Info("===== fSnr : (%f) =====\n", pStSignalInfo->fSnr);
	HxLOG_Info("===== InputPower : (%f) =====\n", pStSignalInfo->fSignalInputPower);
	HxLOG_Info("============================ \n\n");

	return	ERR_OK;
}


HERROR	NX_PORT_SYSTEM_GetTuneParam(HINT32 lIdx, DxTuneParam_t *pstTuneParam)
{
	return	APK_SYSINFO_TUNER_GetTuneParam(lIdx, pstTuneParam);
}


/******************************************************************************/
#define ________________RELATED_ANTENNA_________________________________
/******************************************************************************/

HERROR	NX_PORT_SYSTEM_SetAntennaConnection(DxAntennaConnection_e eConnectionType)
{
	return	APK_SCENARIO_SetAntennaConnection(eConnectionType);
}

HERROR	NX_PORT_SYSTEM_GetAntennaConnection(DxAntennaConnection_e *peConnectionType)
{
	return	APK_SCENARIO_GetAntennaConnection(peConnectionType);
}


/******************************************************************************/
#define ________________RELATED_LIFE_CYCLE_________________________________
/******************************************************************************/
HERROR	NX_PORT_SYSTEM_GetFirstTimeBoot( HBOOL *pbFirstTimeBoot)
{
	HINT32		value;

	*pbFirstTimeBoot = FALSE;
	if (APK_DB_GetInt(USERDB_SYSTEM_B_FIRSTTIME_BOOT, &value) != ERR_OK)
	{
		HxLOG_Warning("read udb failure : value is [%d]\n", value);
		*pbFirstTimeBoot = TRUE;
	}
	else
	{
		HxLOG_Warning("read udb ok : value is [%d]\n", value);
		if (value)
		{
			*pbFirstTimeBoot = TRUE;
		}
	}

	return ERR_OK;
}

HERROR NX_PORT_SYSTEM_SetFirstTimeBoot( HBOOL bFirstTimeBoot)
{
	APK_DB_SetInt(USERDB_SYSTEM_B_FIRSTTIME_BOOT, bFirstTimeBoot);

	return ERR_OK;
}


HERROR	NX_PORT_SYSTEM_GetWakeupReason(SystemWakeUpReason_e *peWakeupReason)
{
	DxWakeUpReason_e	eWakeupReason = eDxWAKEUP_NONE;

	if (APK_POWER_GetWakeupReason(&eWakeupReason) != ERR_OK)
	{
		HxLOG_Error("Error APK_POWER_GetWakeupReason\n");
		return ERR_FAIL;
	}
	*peWakeupReason = (SystemWakeUpReason_e)eWakeupReason;

	HxLOG_Warning("Wakeup Reason : %d \n", eWakeupReason);

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_RequestReboot(void)
{
	return APK_POWER_RequestReboot();
}

HBOOL	NX_PORT_SYSTEM_IsStandbyMode(void)
{
	// current system power mode while operating.
	return APK_POWER_IsStandbyMode();
}

HERROR	NX_PORT_SYSTEM_RequestShutDown(void)
{
	// requst shut down to system.
	HERROR hErr = APK_DB_SetInt(USERDB_SYSTEM_POWER_STATE, (HINT32)eSYSTEM_POWER_STANDBY);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\n\n DB Setting Error! USERDB_SYSTEM_POWER_STATE !!!! \n\n");
	}
	return APK_POWER_RequestShutDown();
}

HERROR	NX_PORT_SYSTEM_RequestOperation(void)
{
	// request operation to system.
	HERROR hErr = APK_DB_SetInt(USERDB_SYSTEM_POWER_STATE, (HINT32)eSYSTEM_POWER_OPERATION);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\n\n DB Setting Error! USERDB_SYSTEM_POWER_STATE !!!! \n\n");
	}
	return APK_POWER_RequestOperation();
}

HBOOL	NX_PORT_SYSTEM_IsPowerStateStandbyMode(void)
{
	// last power stat on DB.
	HINT32 fPowerState = eSYSTEM_POWER_UNDEFINED;
	HERROR hErr = APK_DB_GetInt(USERDB_SYSTEM_POWER_STATE, &fPowerState);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\n\n Not found USERDB_SYSTEM_POWER_STATE. It may be first initialization or DB error !!!! \n\n");
		fPowerState = eSYSTEM_POWER_OPERATION;
	}

	if(fPowerState == eSYSTEM_POWER_STANDBY)
	{
		return TRUE;
	}

	return FALSE;
}

HERROR	NX_PORT_SYSTEM_GetCustomCode(HUINT32 *pulCustomCode)
{
	return APK_SYSINFO_STB_GetCustomCode(pulCustomCode);
}


HERROR	NX_PORT_SYSTEM_GetColdBootDone(HBOOL *pbCheckColdBootDone)
{
	return APK_SYSINFO_STB_GetColdBootDone(pbCheckColdBootDone, NULL);
}

void 	NX_PORT_SYSTEM_DoFactoryDefault(void)
{
	fflush(NULL);
	HxLOG_Print("\n\n\n\n====\n[%s:%d] We have to go Factory Default Process!!!!!!!!!!!!!!!\n====\n\n\n\n",__FUNCTION__,__LINE__);
	fflush(NULL);

	NX_PORT_DB_Default();
	APK_EXE_StartFactoryDefault(eDxFAC_DEF_TYPE_ALL);
}


#if defined(CONFIG_SUPPORT_IPEPG_APP)
HERROR	NX_PORT_SYSTEM_GetEnableIPEpg(HBOOL *pbEnableIPEpg)
{
	return APK_DB_GetInt(USERDB_IPEPG_ONOFF, (HINT32*)pbEnableIPEpg);
}

HERROR	NX_PORT_SYSTEM_SetEnableIPEpg(HBOOL bEnableIPEpg)
{
	return APK_DB_SetInt(USERDB_IPEPG_ONOFF, (HINT32)bEnableIPEpg);
}
#endif//CONFIG_SUPPORT_IPEPG_APP

#if defined(CONFIG_MW_MM_PVR)
HERROR	NX_PORT_SYSTEM_GetEnableSeriesPlay(HBOOL *pbSeriesPlayEnable)
{
	return APK_DB_GetInt(USERDB_SERIES_PLAYBACK, (HINT32*)pbSeriesPlayEnable);
}

HERROR	NX_PORT_SYSTEM_SetEnableSeriesPlay(HBOOL bSeriesPlayEnable)
{
	return APK_DB_SetInt(USERDB_SERIES_PLAYBACK, (HINT32)bSeriesPlayEnable);
}

HERROR	NX_PORT_SYSTEM_GetInstantReplayTime(HUINT32 *pulInstantReplayTime)
{
	return APK_DB_GetInt(USERDB_RECORDING_INSTANT_REPLAY_TIME, (HINT32*)pulInstantReplayTime);
}

HERROR	NX_PORT_SYSTEM_SetInstantReplayTime(HUINT32 ulInstantReplayTime)
{
	return APK_DB_SetInt(USERDB_RECORDING_INSTANT_REPLAY_TIME, (HINT32)ulInstantReplayTime);
}

HERROR	NX_PORT_SYSTEM_GetSkipForwardTime(HUINT32 *pulSkipForwardTime)
{
	return APK_DB_GetInt(USERDB_RECORDING_SKIP_FORWARD_TIME, (HINT32*)pulSkipForwardTime);
}

HERROR	NX_PORT_SYSTEM_SetSkipForwardTime(HUINT32 ulSkipForwardTime)
{
	return APK_DB_SetInt(USERDB_RECORDING_SKIP_FORWARD_TIME, (HINT32)ulSkipForwardTime);
}

HERROR	NX_PORT_SYSTEM_GetEndPaddingTime(HUINT32 *pulEndPaddingTime)
{
	return APK_SCENARIO_GetEndPadding((HINT32*)pulEndPaddingTime);
}

HERROR	NX_PORT_SYSTEM_SetEndPaddingTime(HUINT32 ulEndPaddingTime)
{
	return APK_SCENARIO_SetEndPadding((HINT32)ulEndPaddingTime);
}

HERROR	NX_PORT_SYSTEM_GetStartPaddingTime(HUINT32 *pulStartPaddingTime)
{
	return APK_SCENARIO_GetStartPadding((HINT32*)pulStartPaddingTime);
}

HERROR	NX_PORT_SYSTEM_SetStartPaddingTime(HUINT32 ulStartPaddingTime)
{
	return APK_SCENARIO_SetStartPadding((HINT32)ulStartPaddingTime);
}

HERROR	NX_PORT_SYSTEM_GetAutoDelete(HBOOL *pbAutoDelete)
{
	return APK_SCENARIO_GetAutoDelete(pbAutoDelete);
}

HERROR	NX_PORT_SYSTEM_SetAutoDelete(HBOOL bAutoDelete)
{
	return APK_SCENARIO_SetAutoDelete(bAutoDelete);
}
#endif

static nx_portImportExportEventListener s_nxPortImportExportListener = NULL;

static void nx_PORT_ImportExportEventListener(HUINT32 ulType, HUINT32 ulResult)
{
	if (s_nxPortImportExportListener)
	{
		s_nxPortImportExportListener(ulType, ulResult);
	}
}

HERROR	NX_PORT_SYSTEM_ExportDBToUsb(nx_portImportExportEventListener listener, HBOOL bSync)
{
	s_nxPortImportExportListener = listener;
	APK_EXE_ExportDB(nx_PORT_ImportExportEventListener, bSync);

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_ImportDBFromUsb(nx_portImportExportEventListener listener, HBOOL bSync)
{
	s_nxPortImportExportListener = listener;
	APK_EXE_ImportDB(nx_PORT_ImportExportEventListener, bSync);

	return ERR_OK;
}

HERROR	NX_PORT_SYSTEM_GetDayLightSavingTime(HBOOL *pbEnabled)
{
	return APK_SCENARIO_GetDayLightSaving(pbEnabled);
}

HERROR	NX_PORT_SYSTEM_SetDayLightSavingTime(HBOOL bEnabled)
{
	return APK_SCENARIO_SetDayLightSaving(bEnabled);
}

HERROR	NX_PORT_SYSTEM_GetGMTOffset(HINT32 *pnGmtOffset)
{
	return APK_SCENARIO_GetGMTOffset(pnGmtOffset);
}

HERROR	NX_PORT_SYSTEM_SetGMTOffset(HINT32 nGmtOffset)
{
	return APK_SCENARIO_SetGMTOffset(nGmtOffset);
}


/******************************************************************************/
#define _________Event Listner____________________________________________________
/******************************************************************************/

static void nx_PORT_SYSTEM_POWER_EventListner(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HxLOG_Warning("nx_SYSTEM_Power_EventListner event received: %u %u %u\n", ulParam1, ulParam2, ulParam3);

	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_POWER, ulParam3, 0, 0);
}

void	NX_PORT_SYSTEM_POWER_RegisterNotifier(void)
{
	APK_POWER_RegisterNotifier(nx_PORT_SYSTEM_POWER_EventListner);
}

static void nx_PORT_SYSTEM_AUDIO_EventListner(void)
{
	HxLOG_Warning("nx_PORT_SYSTEM_AUDIO_EventListner event received \n");
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE,GWM_APK_SQC_AUDIO, 0, 0, 0);
}

void	NX_PORT_SYSTEM_AUDIO_RegisterNotifier(void)
{
	APK_OUTPUT_AUDIO_RegisterNotifier(nx_PORT_SYSTEM_AUDIO_EventListner);
}

static void nx_PORT_SYSTEM_HDMI_EventListner(void)
{
	HxLOG_Warning("nx_PORT_SYSTEM_HDMI_EventListner event received \n");
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE,GWM_APK_HDMI, 0, 0, 0);
}

void	NX_PORT_SYSTEM_HDMI_RegisterNotifier(void)
{
	APK_OUTPUT_HDMI_RegisterNotifier(nx_PORT_SYSTEM_HDMI_EventListner);
}

static void nx_PORT_SYSTEM_THERMAL_EventListner(void)
{
	HxLOG_Warning("nx_PORT_SYSTEM_HTERMAL_EventListner event received \n");

	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_THERMAL, 0, 0, 0);
}

void	NX_PORT_SYSTEM_THERMAL_RegisterNotifier(void)
{
	APK_OUTPUT_THERMAL_RegisterNotifier(nx_PORT_SYSTEM_THERMAL_EventListner);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

