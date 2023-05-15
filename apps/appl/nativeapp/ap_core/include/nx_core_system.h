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
 * @file	  		nx_core_system.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_SYSTEM_H__
#define __NX_CORE_SYSTEM_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <dapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define NX_VALID_UNIXTIME					(60 * 60 * 24 * 366)			// until 1971/01/02


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eNxScreenSaverMode_LIVE,
	eNxScreenSaverMode_RADIO,
	eNxScreenSaverMode_MUSIC,
}NX_ScreenSaverMode_e;

typedef void (*nx_coreImportExportEventListener)(HUINT32 ulType, HUINT32 ulResult);

/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


HERROR	NX_SYSTEM_GetVolumeMax(HUINT32 *pulVolumeMax);
HERROR	NX_SYSTEM_GetVolume(HUINT32 *pulVolume);
HERROR	NX_SYSTEM_SetVolume(HUINT32 ulVolume);

HERROR	NX_SYSTEM_GetMute(HBOOL *pbMute);
HERROR	NX_SYSTEM_SetMute(HBOOL bMute);


HERROR 	NX_SYSTEM_GetTime (HUINT32 *pulUnixTime);

HERROR	NX_SYSTEM_GetMaturityRating(DxRatingAge_e *peRating);
HERROR	NX_SYSTEM_SetMaturityRating(DxRatingAge_e peRating);

HERROR	NX_SYSTEM_GetChannelSetupPIN(HBOOL *pbEnabled);
HERROR	NX_SYSTEM_SetChannelSetupPIN(HBOOL pbEnabled);

HERROR	NX_SYSTEM_GetMenuLanuage(HINT32 *lValue);
HERROR	NX_SYSTEM_SetMenuLanuage(HINT32 lValue);

HERROR	NX_SYSTEM_GetAudioLanuage(HINT32 *lValue);
HERROR	NX_SYSTEM_SetAudioLanuage(HINT32 lValue);

HERROR	NX_SYSTEM_GetSecondAudioLanuage(HINT32 *lValue);
HERROR	NX_SYSTEM_SetSecondAudioLanuage(HINT32 lValue);

HERROR	NX_SYSTEM_GetSubtitleLanuage(HINT32 *lValue);
HERROR	NX_SYSTEM_SetSubtitleLanuage(HINT32 lValue);

HERROR	NX_SYSTEM_GetSecondSubtitleLanuage(HINT32 *lValue);
HERROR	NX_SYSTEM_SetSecondSubtitleLanuage(HINT32 lValue);

HERROR	NX_SYSTEM_GetTvAspectRatio(DxAspectRatio_e *peTVAspectRatio);
HERROR	NX_SYSTEM_SetTvAspectRatio(DxAspectRatio_e eTVAspectRatio);

HERROR	NX_SYSTEM_GetDisplayFormat(DxDfcSelect_e *peDfcSelect);
HERROR	NX_SYSTEM_SetDisplayFormat(DxDfcSelect_e eDfcSelect);

HERROR	NX_SYSTEM_GetResolutionSelect(DxResolutionSelect_e *peResolutionSelect);
HERROR	NX_SYSTEM_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect);

HERROR	NX_SYSTEM_GetSpdifAudioOutputMode(DxDigitalAudioFormat_e *peSpdifOutput);
HERROR	NX_SYSTEM_SetSpdifAudioOutputMode(DxDigitalAudioFormat_e eSpdifOutput);

HERROR	NX_SYSTEM_GetHdmiAudioOutputMode(DxDigitalAudioFormat_e *peHdmiOutput);
HERROR	NX_SYSTEM_SetHdmiAudioOutputMode(DxDigitalAudioFormat_e eHdmiOutput);

HERROR	NX_SYSTEM_GetSoundMode(DxStereoSelect_e *peAudioConfig);
HERROR	NX_SYSTEM_SetSoundMode(DxStereoSelect_e eAudioConfig);

HERROR	NX_SYSTEM_GetOperatorType(DxOperator_e *peOpType);
HERROR	NX_SYSTEM_SetOperatorType(DxOperator_e eOpType);

HERROR	NX_SYSTEM_GetCountryCode(HxCountry_e *peCountryCode);
HERROR	NX_SYSTEM_SetCountryCode(HxCountry_e eCountryCode);

HERROR	NX_SYSTEM_GetLipSyncDelay(HUINT32 *pulLipSyncDelay);
HERROR	NX_SYSTEM_SetLipSyncDelay(HUINT32 ulLipSyncDelay);

HERROR	NX_SYSTEM_GetTunerDeviceNum(HUINT32 *ulTunerNum);
HERROR	NX_SYSTEM_GetTunerStatus(HINT32 lIdx, HBOOL *bTunerLock);
HERROR	NX_SYSTEM_GetSignalInfo(HINT32 lIdx, NX_SignalInfo_t *pStSignalInfo);
HERROR	NX_SYSTEM_GetTuneParam(HINT32 lIdx, DxTuneParam_t *pstTuneParam);

HERROR	NX_SYSTEM_SetAntennaConnection(DxAntennaConnection_e eConnectionType);
HERROR	NX_SYSTEM_GetAntennaConnection(DxAntennaConnection_e *peConnectionType);

HBOOL	NX_SYSTEM_CheckPinCode(HCHAR *pszPinStr);
HBOOL	NX_SYSTEM_VerifyPinCode(HCHAR *pszPinStr);
HERROR	NX_SYSTEM_ResetPinCode(HCHAR *pszPinStr);
HERROR	NX_SYSTEM_CheckInstallPinCode(HCHAR *pszPinStr);

HERROR	NX_SYSTEM_SetAvEnable(HBOOL bFlag);

HERROR  NX_SYSTEM_GetModelName(HCHAR *pszModelName, HINT32 lMax);
HERROR  NX_SYSTEM_GetSoftwareVersion(HCHAR *pszSoftwareVersion, HINT32 lMax);
HERROR  NX_SYSTEM_GetLoaderVersion(HCHAR *pszLoaderVersion, HINT32 lMax);
HERROR  NX_SYSTEM_GetCIPlusVersion(HCHAR *pszVersionName);
HERROR  NX_SYSTEM_GetSystemId(HCHAR *pszSystemId, HINT32 lMax);
HERROR  NX_SYSTEM_GetUpdateDate(HCHAR *pszUpdateDate, HINT32 lMax);
HERROR  NX_SYSTEM_GetMicomVersion(HCHAR *pszMicomVersion, HINT32 lMax);
HERROR  NX_SYSTEM_GetSerialNumber(HCHAR *pszSerialNumber, HINT32 lMax);
HERROR	NX_SYSTEM_GetFusingInfo(HBOOL *bIsSCS, HBOOL *bIsJTAG, HBOOL *bIsOTP, HBOOL *bIsCWE, HUINT32 *pulMarketID);
HERROR	NX_SYSTEM_GetFusingInfoForCasConax(HBOOL *bIsSCS, HBOOL *bIsJTAG, HBOOL *bIsOTP, HBOOL *bIsCWE, HUINT32* pulOTPField1, HUINT32* pulOTPField2);

HERROR  NX_SYSTEM_GetInfoDisplayTime(HINT32 *plValue);
HERROR  NX_SYSTEM_SetInfoDisplayTime(HINT32 lValue);
HERROR  NX_SYSTEM_GetSubtitleFont(HINT32 *plValue);
HERROR  NX_SYSTEM_SetSubtitleFont(HINT32 lValue);
void	NX_SYSTEM_ShowSubtitle(HBOOL bShow);

HBOOL	NX_SYSTEM_IsStandbyMode(void);
HERROR	NX_SYSTEM_RequestOperation(void);
HERROR	NX_SYSTEM_RequestShutDown(void);
HBOOL	NX_SYSTEM_IsPowerStateStandbyMode(void);

HERROR	NX_SYSTEM_GetFirstTimeBoot( HBOOL *pbFirstTimeBoot);
HERROR	NX_SYSTEM_SetFirstTimeBoot( HBOOL bFirstTimeBoot);

HERROR	NX_SYSTEM_GetStandByPowerMode(HINT32 *plStandByPowerMode);
HERROR	NX_SYSTEM_SetStandByPowerMode(HINT32 lStandByPowerMode);
HERROR	NX_SYSTEM_GetCustomCode(HUINT32 *pulCustomCode);

HERROR	NX_SYSTEM_GetColdBootDone(HBOOL *pbCheckColdBootDone);

HERROR 	NX_SYSTEM_DoFactoryDefault(void);

#if defined(CONFIG_SUPPORT_IPEPG_APP)
HERROR	NX_SYSTEM_GetEnableIPEpg(HBOOL *pbIPEpgEnable);
HERROR	NX_SYSTEM_SetEnableIPEpg(HBOOL	bIPEpgEnable);
#endif//CONFIG_SUPPORT_IPEPG_APP

#if defined(CONFIG_MW_MM_PVR)
HERROR	NX_SYSTEM_GetEnableSeriesPlay(HBOOL	*pbSeriesPlayEnable);
HERROR	NX_SYSTEM_SetEnableSeriesPlay(HBOOL	bSeriesPlayEnable);
HERROR	NX_SYSTEM_GetInstantReplayTime(HUINT32 *pulInstantReplayTime);
HERROR	NX_SYSTEM_SetInstantReplayTime(HUINT32 ulInstantReplayTime);
HERROR	NX_SYSTEM_GetSkipForwardTime(HUINT32 *pulSkipForwardTime);
HERROR	NX_SYSTEM_SetSkipForwardTime(HUINT32 ulSkipForwardTime);
HERROR	NX_SYSTEM_GetEndPaddingTime(HUINT32	*pulEndPaddingTime);
HERROR	NX_SYSTEM_SetEndPaddingTime(HUINT32	ulEndPaddingTime);
HERROR	NX_SYSTEM_GetStartPaddingTime(HUINT32 *pulStartPaddingTime);
HERROR	NX_SYSTEM_SetStartPaddingTime(HUINT32 ulStartPaddingTime);
HERROR	NX_SYSTEM_GetAutoDelete(HBOOL *pbAutoDelete);
HERROR	NX_SYSTEM_SetAutoDelete(HBOOL bAutoDelete);
#endif//CONFIG_MW_MM_PVR

HERROR	NX_SYSTEM_GetAutomaticPowerdownOnOff(HBOOL *pbOnOff);
HERROR	NX_SYSTEM_SetAutomaticPowerdownOnOff(HBOOL bOnOff);

HERROR	NX_SYSTEM_GetHdmiSupportCEC(HBOOL *pbSupport);
HERROR	NX_SYSTEM_GetHdmiHdcpVersion(DxHdcpVersion_e *peVersion);
HERROR	NX_SYSTEM_GetHdmiHdcpStatus(DxHdcpStatus_e *peHdcpStatus);
HERROR	NX_SYSTEM_GetHdmiEdidResolutionCapability(DxResolutionStatus_b *pulResolutionCapability);
HERROR	NX_SYSTEM_GetHdmiCecMode(HBOOL *pbHdmiCecMode);
HERROR	NX_SYSTEM_SetHdmiCecMode(HBOOL bHdmiCecMode);
HERROR	NX_SYSTEM_GetHdmiCecVolumeMode(HBOOL *pbHdmiCecMode);
HERROR	NX_SYSTEM_SetHdmiCecVolumeMode(HBOOL bHdmiCecMode);
HERROR	NX_SYSTEM_GetHdmiIsPlugIn(HBOOL *pbPlugInOut);

HERROR	NX_SYSTEM_StartChannelExport(void);
HERROR	NX_SYSTEM_StartChannelImport(void);

HERROR	NX_SYSTEM_GetDayLightSavingTime(HBOOL *pbEnabled);
HERROR	NX_SYSTEM_SetDayLightSavingTime(HBOOL bEnabled);

HERROR	NX_SYSTEM_GetGMTOffset(HINT32 *pnGmtOffset);
HERROR	NX_SYSTEM_SetGMTOffset(HINT32 nGmtOffset);

HERROR	NX_PANEL_SetAppPriority(HINT32 lPriority);
HERROR	NX_PANEL_SetLedAuto(void);

HERROR	NX_PANEL_SetLedOnOff(DxLedId_e eLedId, HBOOL bOnOff, HBOOL bAuto, HBOOL bAllClear);
HERROR	NX_PANEL_SetString(HUINT8 *pszPanelString, DxPanelAlign_e eAlign, HBOOL bAuto);
HERROR	NX_PANEL_ClearString(void);

HERROR	NX_SYSTEM_Init(void);


#endif // __NX_CORE_SYSTEM_H__

