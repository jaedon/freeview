/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_settings_base.c
	@brief

	Description:  									\n
	Module: node/binding/datatype_convert/plugins/base		\n

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
#include <rest_bind_api.h>
#include <rest_datatype.h>

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

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
#define _____SYSTEM_ENVIRONMENT_____
HERROR rest_datatype_base_JsonizeSettings (HINT32 nJsonWriter, RestSetting_t *pstSettings, HCHAR *szSelect)
{
	if (NULL == pstSettings)
	{
		HxLOG_Error("pstSettings NULL\n");
		return ERR_FAIL;
	}

	// Contents:
	{
		if (-1 != pstSettings->nCountry)
		{
			HCHAR	*szAlpha3 = (HCHAR *)HLIB_COUNTRY_IdTo3166_Alpha3((HxCountry_e)pstSettings->nCountry);

			if (NULL != szAlpha3)
			{
				(void)rest_datatype_SetJsonString(nJsonWriter, "country", szAlpha3, szSelect);
			}
		}

		if (-1 != pstSettings->nOperator)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "operator", DxNAMEOF(DxOperator_e), pstSettings->nOperator, szSelect);
		}
		
		if (-255 != pstSettings->nGmtOffset)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "gmtOffset", pstSettings->nGmtOffset, szSelect);
		}

		// Language

		if ('\0' != pstSettings->szMainLangCode[0])
		{
			(void)rest_datatype_SetJsonString(nJsonWriter, "mainLanguage", pstSettings->szMainLangCode, szSelect);
		}

		if ('\0' != pstSettings->szAudioLangCode[0])
		{
			(void)rest_datatype_SetJsonString(nJsonWriter, "audioLanguage", pstSettings->szAudioLangCode, szSelect);
		}
		
		if ('\0' != pstSettings->szSubtitleLangCode[0])
		{
			(void)rest_datatype_SetJsonString(nJsonWriter, "subtitleLanguage", pstSettings->szSubtitleLangCode, szSelect);
		}

		// Parental Control

		if (-1 != pstSettings->nMaturityRating)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "maturityRating", DxNAMEOF(DxRatingAge_e), pstSettings->nMaturityRating, szSelect);
		}

		// Output Video

		if (-1 != pstSettings->nVideoOnOff)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "videoOn", pstSettings->nVideoOnOff, szSelect);
		}

		if (-1 != pstSettings->nResolutionSetting)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "resolutionSetting", DxNAMEOF(DxResolutionSelect_e), pstSettings->nResolutionSetting, szSelect);
		}

		if (-1 != pstSettings->nAppliedResolution)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "appliedResolution", DxNAMEOF(DxResolutionStatus_e), pstSettings->nAppliedResolution, szSelect);
		}

		if (-1 != pstSettings->nTvAspectRatio)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "tvAspectRatio", DxNAMEOF(DxAspectRatio_e), pstSettings->nTvAspectRatio, szSelect);
		}

		if (-1 != pstSettings->nPictureRate)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "pictureRate", DxNAMEOF(DxPictureRate_e), pstSettings->nPictureRate, szSelect);
		}

		if (-1 != pstSettings->nDisplayFormatSetting)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "displayFormatSetting", DxNAMEOF(DxDfcSelect_e), pstSettings->nDisplayFormatSetting, szSelect);
		}

		if (-1 != pstSettings->nAppliedDisplayFormat)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "appliedDisplayFormat", DxNAMEOF(DxDfcStatus_b), pstSettings->nAppliedDisplayFormat, szSelect);
		}

		if (-1 != pstSettings->nBgColorRed)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "bgColorRed", pstSettings->nBgColorRed, szSelect);
		}

		if (-1 != pstSettings->nBgColorGreen)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "bgColorGreen", pstSettings->nBgColorGreen, szSelect);
		}

		if (-1 != pstSettings->nBgColorBlue)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "bgColorBlue", pstSettings->nBgColorBlue, szSelect);
		}

		// Output Audio

		if (-1 != pstSettings->nAudioMute)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "audioMute", pstSettings->nAudioMute, szSelect);
		}

		if (-1 != pstSettings->nMaxVolume)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "maxVolume", pstSettings->nMaxVolume, szSelect);
		}

		if (-1 != pstSettings->nVolume)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "volume", pstSettings->nVolume, szSelect);
		}

		if (-1000000 != pstSettings->nLipSyncDelay)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "lipSyncDelay", pstSettings->nLipSyncDelay, szSelect);
		}

		if (-1 != pstSettings->nAudioDescription)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "audioDescription", pstSettings->nAudioDescription, szSelect);
		}

		if (-1 != pstSettings->nStereoSelect)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "stereoSelect", DxNAMEOF(DxStereoSelect_e), pstSettings->nStereoSelect, szSelect);
		}

		if (-1 != pstSettings->nDualMonoSelect)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "dualMonoSelect", DxNAMEOF(DxDualMonoSelect_e), pstSettings->nDualMonoSelect, szSelect);
		}

		// Composite Port

		if (-1 != pstSettings->nCompositeSupport)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "compositeSupport", pstSettings->nCompositeSupport, szSelect);
		}

		if (-1 != pstSettings->nCompositeStandard)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "compositeStandard", DxNAMEOF(DxCompositeStandard_e), pstSettings->nCompositeStandard, szSelect);
		}
		
		// Scart Port
		
		if (-1 != pstSettings->nTvScartSupportedFormat)
		{
			(void)rest_datatype_SetJsonBitwise(nJsonWriter, "tvScartSupportedFormat", DxNAMEOF(DxScartFormat_e), pstSettings->nTvScartSupportedFormat, szSelect);
		}

		if (-1 != pstSettings->nVcrScartSupportedFormat)
		{
			(void)rest_datatype_SetJsonBitwise(nJsonWriter, "vcrScartSupportedFormat", DxNAMEOF(DxScartFormat_e), pstSettings->nVcrScartSupportedFormat, szSelect);
		}

		if (-1 != pstSettings->nTvScartFormat)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "tvScartFormat", DxNAMEOF(DxScartFormat_e), pstSettings->nTvScartFormat, szSelect);
		}

		if (-1 != pstSettings->nVcrScartFormat)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "vcrScartFormat", DxNAMEOF(DxScartFormat_e), pstSettings->nVcrScartFormat, szSelect);
		}

		if (-1 != pstSettings->nScartPath)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "scartPath", DxNAMEOF(DxScartPath_e), pstSettings->nScartPath, szSelect);
		}

		// HDMI port

		if (-1 != pstSettings->nHdmiDigitalAudioFormat)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "hdmiDigitalAudioFormat", DxNAMEOF(DxDigitalAudioFormat_e), pstSettings->nHdmiDigitalAudioFormat, szSelect);
		}

		if (-1 != pstSettings->nCecSupport)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "cecSupport", pstSettings->nCecSupport, szSelect);
		}
		
		if (-1 != pstSettings->nCecOnOff)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "cecOn", pstSettings->nCecOnOff, szSelect);
		}

		if (-1 != pstSettings->nHdcpState)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "hdcpState", DxNAMEOF(DxHdcpStatus_e), pstSettings->nHdcpState, szSelect);
		}

		if (-1 != pstSettings->nHdmi3dSelect)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "hdmi3dSelect", DxNAMEOF(DxHdmi3DSelect_e), pstSettings->nHdmi3dSelect, szSelect);
		}

		if (-1 != pstSettings->nEdid3dFormatCap)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "edid3dFormatCapability", DxNAMEOF(DxVideo3DFormat_e), pstSettings->nEdid3dFormatCap, szSelect);
		}

		if (-1 != pstSettings->nEdidResolCap)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "edidResolutionCapability", DxNAMEOF(DxResolutionStatus_e), pstSettings->nEdidResolCap, szSelect);
		}
		
		// SPDIF
		
		if (-1 != pstSettings->nSpdifSupport)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "spdifSupport", pstSettings->nSpdifSupport, szSelect);
		}

		if (-1 != pstSettings->nSpdifDigitalAudioFormat)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "spdifDigitalAudioFormat", DxNAMEOF(DxDigitalAudioFormat_e), pstSettings->nSpdifDigitalAudioFormat, szSelect);
		}

		// Tuner-related

		if (-1 != pstSettings->nAntConnectType)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "antennaConnectionType", DxNAMEOF(DxAntennaConnection_e), pstSettings->nAntConnectType, szSelect);
		}

	}

	return ERR_OK;
}

HERROR rest_datatype_base_DejsonizeSettings (HxJSON_t *pstJson, RestSetting_t *pstSettings)
{
	HCHAR			 szAlpha3[4];

	if (NULL == pstSettings)
	{
		HxLOG_Error("pstSettings NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(szAlpha3, 0, 4);
	if (rest_datatype_GetJsonString(pstJson, "country", szAlpha3, 3) == ERR_OK)
	{
		HINT32		 nCountry = (HINT32)HLIB_COUNTRY_3166_Alpha3ToId((const HCHAR *)szAlpha3);

		if (eCountryID_NONE != nCountry)
		{
			pstSettings->nCountry = nCountry;
		}
	}

	(void)rest_datatype_GetJsonEnum(pstJson, "operator", DxNAMEOF(DxOperator_e), &(pstSettings->nOperator));
	(void)rest_datatype_GetJsonInteger(pstJson, "gmtOffset", &(pstSettings->nGmtOffset));

	// Language
	(void)rest_datatype_GetJsonString(pstJson, "mainLanguage", pstSettings->szMainLangCode, 3);
	(void)rest_datatype_GetJsonString(pstJson, "audioLanguage", pstSettings->szAudioLangCode, 3);
	(void)rest_datatype_GetJsonString(pstJson, "subtitleLanguage", pstSettings->szSubtitleLangCode, 3);

	// Parental Control
	(void)rest_datatype_GetJsonEnum(pstJson, "maturityRating", DxNAMEOF(DxRatingAge_e), &(pstSettings->nMaturityRating));

	// Output Video
	(void)rest_datatype_GetJsonInteger(pstJson, "videoOn", &(pstSettings->nVideoOnOff));
	(void)rest_datatype_GetJsonEnum(pstJson, "resolutionSetting", DxNAMEOF(DxResolutionSelect_e), &(pstSettings->nResolutionSetting));
	(void)rest_datatype_GetJsonEnum(pstJson, "tvAspectRatio", DxNAMEOF(DxAspectRatio_e), &(pstSettings->nTvAspectRatio));
	(void)rest_datatype_GetJsonEnum(pstJson, "pictureRate", DxNAMEOF(DxPictureRate_e), &(pstSettings->nPictureRate));
	(void)rest_datatype_GetJsonEnum(pstJson, "displayFormatSetting", DxNAMEOF(DxDfcSelect_e), &(pstSettings->nDisplayFormatSetting));
	(void)rest_datatype_GetJsonInteger(pstJson, "bgColorRed", &(pstSettings->nBgColorRed));
	(void)rest_datatype_GetJsonInteger(pstJson, "bgColorGreen", &(pstSettings->nBgColorGreen));
	(void)rest_datatype_GetJsonInteger(pstJson, "bgColorBlue", &(pstSettings->nBgColorBlue));

	// Output Audio
	(void)rest_datatype_GetJsonInteger(pstJson, "audioMute", &(pstSettings->nAudioMute));
	(void)rest_datatype_GetJsonInteger(pstJson, "volume", &(pstSettings->nVolume));
	(void)rest_datatype_GetJsonInteger(pstJson, "lipSyncDelay", &(pstSettings->nLipSyncDelay));
	(void)rest_datatype_GetJsonInteger(pstJson, "audioDescription", &(pstSettings->nAudioDescription));
	(void)rest_datatype_GetJsonEnum(pstJson, "stereoSelect", DxNAMEOF(DxStereoSelect_e), &(pstSettings->nStereoSelect));
	(void)rest_datatype_GetJsonEnum(pstJson, "dualMonoSelect", DxNAMEOF(DxDualMonoSelect_e), &(pstSettings->nDualMonoSelect));

	// Composite Port
	(void)rest_datatype_GetJsonInteger(pstJson, "compositeSupport", &(pstSettings->nCompositeSupport));
	(void)rest_datatype_GetJsonEnum(pstJson, "compositeStandard", DxNAMEOF(DxCompositeStandard_e), &(pstSettings->nCompositeStandard));

	// Scart Port
	(void)rest_datatype_GetJsonEnum(pstJson, "tvScartFormat", DxNAMEOF(DxScartFormat_e), &(pstSettings->nTvScartFormat));
	(void)rest_datatype_GetJsonEnum(pstJson, "vcrScartFormat", DxNAMEOF(DxScartFormat_e), &(pstSettings->nVcrScartFormat));
	(void)rest_datatype_GetJsonEnum(pstJson, "scartPath", DxNAMEOF(DxScartPath_e), &(pstSettings->nScartPath));

	// HDMI port
	(void)rest_datatype_GetJsonEnum(pstJson, "hdmiDigitalAudioFormat", DxNAMEOF(DxDigitalAudioFormat_e), &(pstSettings->nHdmiDigitalAudioFormat));
	(void)rest_datatype_GetJsonInteger(pstJson, "cecOn", &(pstSettings->nCecOnOff));
	(void)rest_datatype_GetJsonEnum(pstJson, "hdmi3dSelect", DxNAMEOF(DxHdmi3DSelect_e), &(pstSettings->nHdmi3dSelect));

	// SPDIF
	(void)rest_datatype_GetJsonEnum(pstJson, "spdifDigitalAudioFormat", DxNAMEOF(DxDigitalAudioFormat_e), &(pstSettings->nSpdifDigitalAudioFormat));

	// Tuner-related
	(void)rest_datatype_GetJsonEnum(pstJson, "antennaConnectionType", DxNAMEOF(DxAntennaConnection_e), &(pstSettings->nAntConnectType));

	return ERR_OK;	
}

HINT32 rest_datatype_base_JsonizeAdminSettingItems (HINT32 nJsonWriter, RestSettingAdminFlag_t *pstAdminFlag, HCHAR *szSelect)
{
	if (NULL == pstAdminFlag)
	{
		HxLOG_Error("pstSettings NULL\n");
		return ERR_FAIL;
	}

	// Contents:
	{
		// Require the administrator setting:
		if (pstAdminFlag->nCountry)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "country");
		}

		if (pstAdminFlag->nOperator)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "operator");
		}
		
		if (pstAdminFlag->nGmtOffset)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "gmtOffset");
		}

		// Language
		if (pstAdminFlag->szMainLangCode[0])
		{
			HxJSONWRITER_StringValue(nJsonWriter, "mainLanguage");
		}

		if (pstAdminFlag->szAudioLangCode[0])
		{
			HxJSONWRITER_StringValue(nJsonWriter, "audioLanguage");
		}
		
		if (pstAdminFlag->szSubtitleLangCode[0])
		{
			HxJSONWRITER_StringValue(nJsonWriter, "subtitleLanguage");
		}

		// Parental Control
		if (pstAdminFlag->nMaturityRating)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "maturityRating");
		}

		// Output Video

		if (pstAdminFlag->nVideoOnOff)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "videoOn");
		}

		if (pstAdminFlag->nResolutionSetting)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "resolutionSetting");
		}

		if (pstAdminFlag->nAppliedResolution)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "appliedResolution");
		}

		if (pstAdminFlag->nTvAspectRatio)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "tvAspectRatio");
		}

		if (pstAdminFlag->nPictureRate)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "pictureRate");
		}

		if (pstAdminFlag->nDisplayFormatSetting)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "displayFormatSetting");
		}

		if (pstAdminFlag->nAppliedDisplayFormat)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "appliedDisplayFormat");
		}

		if (pstAdminFlag->nBgColorRed)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "bgColorRed");
		}

		if (pstAdminFlag->nBgColorGreen)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "bgColorGreen");
		}

		if (pstAdminFlag->nBgColorBlue)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "bgColorBlue");
		}

		// Output Audio

		if (pstAdminFlag->nAudioMute)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "audioMute");
		}

		if (pstAdminFlag->nMaxVolume)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "maxVolume");
		}

		if (pstAdminFlag->nVolume)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "volume");
		}

		if (pstAdminFlag->nLipSyncDelay)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "lipSyncDelay");
		}

		if (pstAdminFlag->nAudioDescription)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "audioDescription");
		}

		if (pstAdminFlag->nStereoSelect)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "stereoSelect");
		}

		if (pstAdminFlag->nDualMonoSelect)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "dualMonoSelect");
		}

		// Composite Port

		if (pstAdminFlag->nCompositeSupport)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "compositeSupport");
		}

		if (pstAdminFlag->nCompositeStandard)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "compositeStandard");
		}
		
		// Scart Port
		
		if (pstAdminFlag->nTvScartSupportedFormat)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "tvScartSupportedFormat");
		}

		if (pstAdminFlag->nVcrScartSupportedFormat)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "vcrScartSupportedFormat");
		}

		if (pstAdminFlag->nTvScartFormat)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "tvScartFormat");
		}

		if (pstAdminFlag->nVcrScartFormat)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "vcrScartFormat");
		}

		if (pstAdminFlag->nScartPath)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "scartPath");
		}

		// HDMI port

		if (pstAdminFlag->nHdmiDigitalAudioFormat)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "hdmiDigitalAudioFormat");
		}

		if (pstAdminFlag->nCecSupport)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "cecSupport");
		}
		
		if (pstAdminFlag->nCecOnOff)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "cecOn");
		}

		if (pstAdminFlag->nHdcpState)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "hdcpState");
		}

		if (pstAdminFlag->nHdmi3dSelect)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "hdmi3dSelect");
		}

		if (pstAdminFlag->nEdid3dFormatCap)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "edid3dFormatCapability");
		}

		if (pstAdminFlag->nEdidResolCap)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "edidResolutionCapability");
		}
		
		// SPDIF
		
		if (pstAdminFlag->nSpdifSupport)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "spdifSupport");
		}

		if (pstAdminFlag->nSpdifDigitalAudioFormat)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "spdifDigitalAudioFormat");
		}

		// Tuner-related

		if (pstAdminFlag->nAntConnectType)
		{
			HxJSONWRITER_StringValue(nJsonWriter, "antennaConnectionType");
		}

	}

	return ERR_OK;
}
