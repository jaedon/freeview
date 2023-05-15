/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_scenario.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

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

#include <hapi.h>
#include <oapi.h>
#include <tapi.h>
#include <apk.h>

#define ___INTERNAL_STRUCTURES___

#define ___INTERNAL_VARIABLES___
static	HCHAR	*s_szFvcBaseUrl = NULL;
static	HCHAR	*s_szFvcAuthHash = NULL;
static	HCHAR	*s_szFvcAuthTimeStamp = NULL;

#define ___PRIVATE_FUNCTION___
STATIC HCHAR*	apk_scenario_concatStrInt(const HCHAR *pszString, HINT32 nNum)
{
	STATIC HxCBUF_t	s_nCBuf = NULL;

	HCHAR	*pszBuf;

	if (s_nCBuf == NULL)
		s_nCBuf = HLIB_CBUF_New(4096);

	pszBuf = HLIB_CBUF_GetBuffer(s_nCBuf, 256);
	HxSTD_snprintf(pszBuf, 255, "%s%d", pszString, nNum);

	return pszBuf;
}

STATIC HERROR	apk_scenario_GetStrValue(const HCHAR *pszKey, HCHAR *pszValue, HINT32 nMax)
{
	return HAPI_GetRegistryStr(pszKey, pszValue, nMax);
}

STATIC HERROR	apk_scenario_GetBoolValue(const HCHAR *pszKey, HBOOL *pbEnabled)
{
	HINT32	nValue;

	if (HAPI_GetRegistryInt(pszKey, &nValue) == ERR_OK)
	{
		*pbEnabled = (nValue ? TRUE : FALSE);
		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC HERROR	apk_scenario_GetIntValue(const HCHAR *pszKey, HINT32 *pnValue)
{
	return HAPI_GetRegistryInt(pszKey, pnValue);
}

STATIC HERROR	apk_scenario_GetEnumValue(const HCHAR *pszKey, const HCHAR *enumType, HINT32 *pnValue)
{
	return HAPI_GetRegistryEnum(pszKey, enumType, pnValue);
}

static HERROR	apk_scenario_onFvcBaseUrlChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*szCurProcessName = NULL;
	HCHAR	*szSrcProcessName = NULL;
	HBOOL	bIsCameFromMe = FALSE;

	if (nArgc != 2)
	{
		return ERR_FAIL;
	}
	szCurProcessName = HLIB_STD_GetCurProcessName();
	szSrcProcessName = (char*)HxOBJECT_STR(apArgv[1]);

	bIsCameFromMe = (HLIB_STD_StrCaseCmp(szCurProcessName,szSrcProcessName) == 0) ? TRUE: FALSE;
	if (bIsCameFromMe == TRUE)
	{
		return ERR_FAIL;
	}

	if (s_szFvcBaseUrl)
	{
		HLIB_MEM_Free(s_szFvcBaseUrl);
	}

	s_szFvcBaseUrl = HLIB_MEM_StrDup((char*)HxOBJECT_STR(apArgv[0]));

	return ERR_OK;
}

static HERROR	apk_scenario_onFvcAuthHashChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*szCurProcessName = NULL;
	HCHAR	*szSrcProcessName = NULL;
	HBOOL	bIsCameFromMe = FALSE;

	if (nArgc != 2)
	{
		return ERR_FAIL;
	}
	szCurProcessName = HLIB_STD_GetCurProcessName();
	szSrcProcessName = (char*)HxOBJECT_STR(apArgv[1]);
	bIsCameFromMe = (HLIB_STD_StrCaseCmp(szCurProcessName,szSrcProcessName) == 0) ? TRUE: FALSE;
	if (bIsCameFromMe == TRUE)
	{
		return ERR_FAIL;
	}

	if (s_szFvcAuthHash)
	{
		HLIB_MEM_Free(s_szFvcAuthHash);
	}

	s_szFvcAuthHash = HLIB_MEM_StrDup((char*)HxOBJECT_STR(apArgv[0]));

	return ERR_OK;

}

static HERROR	apk_scenario_onFvcAuthTimeStampChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*szCurProcessName = NULL;
	HCHAR	*szSrcProcessName = NULL;
	HBOOL	bIsCameFromMe = FALSE;

	if (nArgc != 2)
	{
		return ERR_FAIL;
	}
	szCurProcessName = HLIB_STD_GetCurProcessName();
	szSrcProcessName = (char*)HxOBJECT_STR(apArgv[1]);
	bIsCameFromMe = (HLIB_STD_StrCaseCmp(szCurProcessName,szSrcProcessName) == 0) ? TRUE: FALSE;
	if (bIsCameFromMe == TRUE)
	{
		return ERR_FAIL;
	}

	if (s_szFvcAuthTimeStamp)
	{
		HLIB_MEM_Free(s_szFvcAuthTimeStamp);
	}

	s_szFvcAuthTimeStamp = HLIB_MEM_StrDup((char*)HxOBJECT_STR(apArgv[0]));

	return ERR_OK;

}


#define _____NOTIFIER_FUNCTION_____

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR	APK_SCENARIO_Init(void)
{
	HAPI_ConnectSignal("signal:onFvcBaseUrlChanged", 		NULL, apk_scenario_onFvcBaseUrlChanged);
	HAPI_ConnectSignal("signal:onFvcAuthHashChanged",   	NULL, apk_scenario_onFvcAuthHashChanged);
	HAPI_ConnectSignal("signal:onFvcAuthTimeStampChanged",	NULL, apk_scenario_onFvcAuthTimeStampChanged);

	return OAPI_SCENARIO_Init();
}

HERROR	APK_SCENARIO_DeInit(void)
{
	HAPI_DisconnectSignal("signal:onFvcBaseUrlChanged",		apk_scenario_onFvcBaseUrlChanged);
	HAPI_DisconnectSignal("signal:onFvcAuthHashChanged",	apk_scenario_onFvcAuthHashChanged);
	HAPI_DisconnectSignal("signal:onFvcAuthTimeStampChanged", apk_scenario_onFvcAuthTimeStampChanged);

	return OAPI_SCENARIO_DeInit();
}

HERROR	APK_SCENARIO_GetMainLanguage(HCHAR *pszISO639)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_MAINLANGUAGE, pszISO639, 4);
}

HERROR	APK_SCENARIO_SetMainLanguage(const HCHAR *pszISO639)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_MAINLANGUAGE, "s", pszISO639);
}

HERROR	APK_SCENARIO_GetAudioLanguage(HINT32 nIndex, HCHAR *pszISO639)
{
	HCHAR	*pszTempKey;

	pszTempKey = apk_scenario_concatStrInt(OAPI_SCENARIO_AUDIOLANGUAGE, nIndex);

	return apk_scenario_GetStrValue(pszTempKey, pszISO639, 4);
}

HERROR	APK_SCENARIO_SetAudioLanguage(HINT32 nIndex, const HCHAR *pszISO639)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_AUDIOLANGUAGE, "is", nIndex, pszISO639);
}

HERROR	APK_SCENARIO_GetAudioLanguageUndefined(HCHAR *pszISO639)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_AUDIOLANGUAGE_UNDEFFINED, pszISO639, 4);
}

HERROR	APK_SCENARIO_SetAudioLanguageUndefined(const HCHAR *pszISO639)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_AUDIOLANGUAGE_UNDEFFINED, "s", pszISO639);
}

HERROR	APK_SCENARIO_GetAudioLanguageOriginal(HCHAR *pszISO639)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_AUDIOLANGUAGE_ORIGINAL, pszISO639, 4);
}

HERROR	APK_SCENARIO_SetAudioLanguageOriginal(const HCHAR *pszISO639)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_AUDIOLANGUAGE_ORIGINAL, "s", pszISO639);
}

HERROR	APK_SCENARIO_GetAudioLanguageDefault(HCHAR *pszISO639)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_AUDIOLANGUAGE_DEFAULT, pszISO639, 4);
}

HERROR	APK_SCENARIO_SetAudioLanguageDefault(const HCHAR *pszISO639)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_AUDIOLANGUAGE_DEFAULT, "s", pszISO639);
}

HERROR	APK_SCENARIO_GetSubtitleLanguage(HINT32 nIndex, HCHAR *pszISO639)
{
	HCHAR	*pszTempKey;

	pszTempKey = apk_scenario_concatStrInt(OAPI_SCENARIO_SUBTITLELANGUAGE, nIndex);

	return apk_scenario_GetStrValue(pszTempKey, pszISO639, 4);
}

HERROR	APK_SCENARIO_SetSubtitleLanguage(HINT32 nIndex, const HCHAR *pszISO639)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_SUBTITLELANGUAGE, "is", nIndex, pszISO639);
}

HERROR	APK_SCENARIO_GetSubtitleLanguageUndefined(HCHAR *pszISO639)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_SUBTITLELANGUAGE_UNDEFFINED, pszISO639, 4);
}

HERROR	APK_SCENARIO_SetSubtitleLanguageUndefined(const HCHAR *pszISO639)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_SUBTITLELANGUAGE_UNDEFFINED, "s", pszISO639);
}

HERROR	APK_SCENARIO_GetSubtitleLanguageOriginal(HCHAR *pszISO639)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_SUBTITLELANGUAGE_ORIGINAL, pszISO639, 4);
}

HERROR	APK_SCENARIO_SetSubtitleLanguageOriginal(const HCHAR *pszISO639)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_SUBTITLELANGUAGE_ORIGINAL, "s", pszISO639);
}

HERROR	APK_SCENARIO_GetSubtitleLanguageDefault(HCHAR *pszISO639)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_SUBTITLELANGUAGE_DEFAULT, pszISO639, 4);
}

HERROR	APK_SCENARIO_SetSubtitleLanguageDefault(const HCHAR *pszISO639)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_SUBTITLELANGUAGE_DEFAULT, "s", pszISO639);
}

HERROR	APK_SCENARIO_GetHardOfHearing(HBOOL *pbEnabled)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_HARDOFHEARING, pbEnabled);
}

HERROR	APK_SCENARIO_SetHardOfHearing(HBOOL bEnabled)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_HARDOFHEARING, "i", bEnabled);
}

HERROR	APK_SCENARIO_GetMaturityRating(DxRatingAge_e *peRating)
{
	return apk_scenario_GetIntValue(OAPI_SCENARIO_MATURITYRATING, (HINT32*)peRating);
}

HERROR	APK_SCENARIO_SetMaturityRating(DxRatingAge_e eRating)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_MATURITYRATING, "i", eRating);
}

HERROR	APK_SCENARIO_GetCountryCode(HxCountry_e *peCountryCode)
{
	return apk_scenario_GetIntValue(OAPI_SCENARIO_COUNTRYCODE, (HINT32*)peCountryCode);
}

HERROR	APK_SCENARIO_SetCountryCode(HxCountry_e eCountryCode)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_COUNTRYCODE, "i", eCountryCode);
}

HERROR	APK_SCENARIO_GetOperatorType(DxOperator_e *peOpType)
{
	return apk_scenario_GetEnumValue(OAPI_SCENARIO_OPERATORTYPE, "DxOperator_e", (HINT32*)peOpType);
}

HERROR	APK_SCENARIO_SetOperatorType(DxOperator_e eOpType)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_OPERATORTYPE, "i", eOpType);
}

HERROR	APK_SCENARIO_GetAntennaConnection(DxAntennaConnection_e *peConnectionType)
{
	return apk_scenario_GetEnumValue(OAPI_SCENARIO_ANTENNACONNECTION, "DxAntennaConnection_e", (HINT32*)peConnectionType);
}

HERROR	APK_SCENARIO_SetAntennaConnection(DxAntennaConnection_e eConnectionType)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_ANTENNACONNECTION, "i", eConnectionType);
}

HERROR	APK_SCENARIO_GetStartPadding(HINT32 *pnPadding)
{
	return apk_scenario_GetIntValue(OAPI_SCENARIO_STARTPADDING, pnPadding);
}

HERROR	APK_SCENARIO_SetStartPadding(HINT32 nPadding)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_STARTPADDING, "i", nPadding);
}

HERROR	APK_SCENARIO_GetEndPadding(HINT32 *pnPadding)
{
	return apk_scenario_GetIntValue(OAPI_SCENARIO_ENDPADDING, pnPadding);
}

HERROR	APK_SCENARIO_SetEndPadding(HINT32 nPadding)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_ENDPADDING, "i", nPadding);
}

HERROR	APK_SCENARIO_GetAutoDelete(HBOOL *pbAutoDelete)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_AUTODELETE, pbAutoDelete);
}

HERROR	APK_SCENARIO_SetAutoDelete(HBOOL bAutoDelete)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_AUTODELETE, "i", (bAutoDelete ? 1 : 0));
}

HERROR	APK_SCENARIO_GetPostCode(HCHAR *pszPostcode, HINT32 nMax)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_POSTCODE, pszPostcode, nMax);
}

HERROR	APK_SCENARIO_SetPostCode(const HCHAR *pszPostcode)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_POSTCODE, "s", pszPostcode);
}

HERROR	APK_SCENARIO_GetGMTOffset(HINT32 *pnGmtOffset)
{
	return apk_scenario_GetIntValue(OAPI_SCENARIO_GMTOFFSET, pnGmtOffset);
}

HERROR	APK_SCENARIO_SetGMTOffset(HINT32 nGmtOffset)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_GMTOFFSET, "i", nGmtOffset);
}

HERROR	APK_SCENARIO_GetChannelSetupPIN(HBOOL *pbEnabled)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_CHANNELSETUPPIN, pbEnabled);
}

HERROR	APK_SCENARIO_SetChannelSetupPIN(HBOOL bEnabled)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_CHANNELSETUPPIN, "i", (bEnabled ? 1 : 0));
}

HERROR	APK_SCENARIO_GetGuidancePolicy(DxGuidancePolicy_e *peGuidancePolicy)
{
	return apk_scenario_GetEnumValue(OAPI_SCENARIO_GUIDANCEPOLICY, "DxGuidancePolicy_e", (HINT32*)peGuidancePolicy);
}

HERROR	APK_SCENARIO_SetGuidancePolicy(DxGuidancePolicy_e eGuidancePolicy)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_GUIDANCEPOLICY, "i", eGuidancePolicy);
}

HERROR	APK_SCENARIO_GetSubtitleSetting(HBOOL *pbSubtitle)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_SUBTITLESETTING, pbSubtitle);
}

HERROR	APK_SCENARIO_SetSubtitleSetting(HBOOL bSubtitle)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_SUBTITLESETTING, "i", bSubtitle);
}

HERROR	APK_SCENARIO_GetDayLightSaving(HBOOL *pbEnabled)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_DAYLIGHTSAVING, pbEnabled);
}

HERROR	APK_SCENARIO_SetDayLightSaving(HBOOL bEnabled)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_DAYLIGHTSAVING, "i", bEnabled);
}

HERROR	APK_SCENARIO_GetTimeUpdate(HBOOL *pnEnabled)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_TIMEUPDATE, pnEnabled);
}

HERROR	APK_SCENARIO_SetTimeUpdate(HBOOL bEnabled)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_TIMEUPDATE, "i", bEnabled);
}

HERROR	APK_SCENARIO_GetBouquetName(HCHAR *pszBouquet, HINT32 nMax)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_BOUQUETNAME, pszBouquet, nMax);
}

HERROR	APK_SCENARIO_SetBouquetName(const HCHAR *pszBouquet)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_BOUQUETNAME, "s", pszBouquet);
}

HERROR	APK_SCENARIO_GetBouquetID(HINT32 *pnBouquetId)
{
	return apk_scenario_GetIntValue(OAPI_SCENARIO_BOUQUETID, pnBouquetId);
}

HERROR	APK_SCENARIO_SetBouquetID(HINT32 nBouquetId)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_BOUQUETID, "i", nBouquetId);
}

HERROR	APK_SCENARIO_GetSIMode(DxSIMode_e *peSIMode)
{
	return apk_scenario_GetEnumValue(OAPI_SCENARIO_SIMODE, "DxSIMode_e", (HINT32*)peSIMode);
}

HERROR	APK_SCENARIO_SetSIMode(DxSIMode_e eSIMode)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_SIMODE, "i", eSIMode);
}

HERROR	APK_SCENARIO_GetRegionCode(HINT32 *pnRegionCode)
{
	return apk_scenario_GetIntValue(OAPI_SCENARIO_REGIONCODE, pnRegionCode);
}

HERROR	APK_SCENARIO_SetRegionCode(HINT32 nRegionCode)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_REGIONCODE, "i", nRegionCode);
}

HERROR	APK_SCENARIO_GetAdultChannels(DxAdultChannelType_e *peAdultChannel)
{
	return apk_scenario_GetEnumValue(OAPI_SCENARIO_ADULTCHANNEL, "DxAdultChannelType_e", (HINT32*)peAdultChannel);
}

HERROR	APK_SCENARIO_SetAdultChannels(DxAdultChannelType_e eAdultChannel)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_ADULTCHANNEL, "i", eAdultChannel);
}

HERROR	APK_SCENARIO_GetSWUpdateVersion(HCHAR *pszSWUpdateVersion, HINT32 nMax)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_SWUPDATEVERSION, pszSWUpdateVersion, nMax);
}

HERROR	APK_SCENARIO_RemoveSWUpdateVersion(void)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_SWUPDATEVERSION, "s", "");
}

HERROR	APK_SCENARIO_GetIPSWUpdateVersion(HCHAR *pszSWUpdateVersion, HINT32 nMax)
{
	return apk_scenario_GetStrValue(OAPI_SCENARIO_IPSWUPDATEVERSION, pszSWUpdateVersion, nMax);
}

HERROR	APK_SCENARIO_SetSatRecReceiverId(HINT32 nReceiverId)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_SATREC_RECEIVERID, "i", nReceiverId);
}

HERROR	APK_SCENARIO_GetSatRecUpdateInterval(HUINT32 *pUpdateInterval)
{
	return apk_scenario_GetIntValue(OAPI_SCENARIO_SATREC_INTERVAL, pUpdateInterval);
}

HERROR	APK_SCENARIO_SetSatRecUpdateInterval(DxSatRecIntervalType_e eIntervalType, HUINT16 nTime)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_SATREC_INTERVAL, "ii", eIntervalType, nTime);

}
HERROR	APK_SCENARIO_GetTVTVEnable(HBOOL *pbEnabled)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_TVTV_ENABLE, pbEnabled);
}

HERROR	APK_SCENARIO_SetTVTVEnable(HBOOL bEnabled)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_TVTV_ENABLE, "i", bEnabled);
}

HERROR	APK_SCENARIO_GetTVTVEPGDownload_StandbyEnable(HBOOL *pbEnabled)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_TVTV_DOWNTYPE, pbEnabled);
}

HERROR	APK_SCENARIO_SetTVTVEPGDownload_StandbyEnable(HBOOL bEnabled)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_TVTV_DOWNTYPE, "i", bEnabled);
}

HERROR	APK_SCENARIO_GetFavChListNumberingMode(HBOOL *pbFavChListNumberingMode)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_FAVCHLISTNUMBERING, pbFavChListNumberingMode);
}

HERROR	APK_SCENARIO_SetFavChListNumberingMode(HBOOL bFavChListNumberingMode)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_FAVCHLISTNUMBERING, "i", bFavChListNumberingMode);
}

HERROR	APK_SCENARIO_GetDescrambleOnOff(HBOOL *pbOnOff)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_DESCRAMBLEONOFF, pbOnOff);
}

HERROR	APK_SCENARIO_SetDescrambleOnOff(HBOOL bOnOff)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_DESCRAMBLEONOFF, "i", bOnOff);
}


HERROR	APK_SCENARIO_SetNtpServerURL(HCHAR * psNtpUrl)
{
	return TAPI_SetNTP(psNtpUrl);
}

HERROR	APK_SCENARIO_GetFvcBaseUrl(HCHAR *pszUrl, HINT32 ulLen)
{
	if(s_szFvcBaseUrl == NULL)
	{
		return ERR_FAIL;
	}
	HxSTD_MemSet(pszUrl,0,ulLen);
	HxSTD_StrNCpy(pszUrl, s_szFvcBaseUrl, ulLen);
	pszUrl[ulLen-1] = '\0';

	return ERR_OK;
}

HERROR	APK_SCENARIO_SetFvcBaseUrl(const HCHAR *str)
{
	if (s_szFvcBaseUrl)
	{
		HLIB_MEM_Free(s_szFvcBaseUrl);
	}

	s_szFvcBaseUrl = HLIB_MEM_StrDup(str);

	return ERR_OK;
}

HERROR	APK_SCENARIO_GetFvcAuthHash(HCHAR *str, HINT32 ulLen)
{
	if(s_szFvcAuthHash == NULL)
	{
		return ERR_FAIL;
	}
	HxSTD_MemSet(str,0,ulLen);
	HxSTD_StrNCpy(str, s_szFvcAuthHash, ulLen);
	str[ulLen-1] = '\0';

	return ERR_OK;
}

HERROR	APK_SCENARIO_SetFvcAuthHash(const HCHAR *str)
{
	if (s_szFvcAuthHash)
	{
		HLIB_MEM_Free(s_szFvcAuthHash);
	}

	s_szFvcAuthHash = HLIB_MEM_StrDup(str);

	return ERR_OK;
}

HERROR	APK_SCENARIO_GetFvcAuthTimeStamp(HCHAR *str, HINT32 ulLen)
{
	if(s_szFvcAuthTimeStamp == NULL)
	{
		return ERR_FAIL;
	}
	HxSTD_MemSet(str,0,ulLen);
	HxSTD_StrNCpy(str, s_szFvcAuthTimeStamp, ulLen);
	str[ulLen-1] = '\0';

	return ERR_OK;
}

HERROR	APK_SCENARIO_SetFvcAuthTimeStamp(const HCHAR *str)
{
	if (s_szFvcAuthTimeStamp)
	{
		HLIB_MEM_Free(s_szFvcAuthTimeStamp);
	}

	s_szFvcAuthTimeStamp = HLIB_MEM_StrDup(str);

	return ERR_OK;
}

HERROR	APK_SCENARIO_GetLiveStreamPriority (DxLiveStreamPriority_e *pePriority)
{
	return apk_scenario_GetEnumValue(OAPI_SCENARIO_LIVESTREAM_PRIORITY, "DxLiveStreamPriority_e", (HINT32*)pePriority);
}

HERROR	APK_SCENARIO_SetLiveStreamPriority (DxLiveStreamPriority_e ePriority)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_LIVESTREAM_PRIORITY, "i", ePriority);
}

HERROR	APK_SCENARIO_GetAutomaticPowerdown(HUINT32 *pTimeout)
{
	return apk_scenario_GetIntValue(OAPI_SCENARIO_AUTOMATIC_POWERDOWN,pTimeout);
}


HERROR	APK_SCENARIO_SetAutomaticPowerdown (HUINT32 ulTimeout)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_AUTOMATIC_POWERDOWN, "i", ulTimeout);
}

HERROR	APK_SCENARIO_GetPriorityHDMIOutput(HBOOL *pEnable)
{
	return apk_scenario_GetIntValue(OAPI_SCENARIO_PRIORITY_HDMIOUTPUT,pEnable);
}

HERROR	APK_SCENARIO_SetPriorityHDMIOutput (HBOOL bEnable)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_PRIORITY_HDMIOUTPUT, "i", bEnable);
}

HERROR	APK_SCENARIO_GetBatUpdateOnOff(HBOOL *pbOnOff)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_BATUPDATEONOFF, pbOnOff);
}

HERROR	APK_SCENARIO_SetBatUpdateOnOff(HBOOL bOnOff)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_BATUPDATEONOFF, "i", bOnOff);
}

HERROR	APK_SCENARIO_GetNitLcnUpdateOnOff(HBOOL *pbOnOff)
{
	return apk_scenario_GetBoolValue(OAPI_SCENARIO_NITLCNUPDATEONOFF, pbOnOff);
}

HERROR	APK_SCENARIO_SetNitLcnUpdateOnOff(HBOOL bOnOff)
{
	return OAPI_SCENARIO_Set(OAPI_SCENARIO_NITLCNUPDATEONOFF, "i", bOnOff);
}



HERROR	APK_SCENARIO_GetLastServiceGroup(HINT32 *pnLastSvcGroup)
{
	return HAPI_GetRegistryInt(OAPI_SCENARIO_LAST_SVC_GROUP, pnLastSvcGroup);
}

HERROR	APK_SCENARIO_SetLastServiceGroup(HINT32 nLastSvcGroup)
{
	return HAPI_SetRegistryInt(OAPI_SCENARIO_LAST_SVC_GROUP, nLastSvcGroup);
}

HERROR	APK_SCENARIO_SendKeyFromApp(HUINT32 ulKeySymbol)
{
	return HAPI_SendRemoteRCUKey(ulKeySymbol);
}

/* end of file */
