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
 * @file	  		nx_port_system.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PORT_SYSTEM_H__
#define __NX_PORT_SYSTEM_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/

#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include <nx_port_type.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
typedef void (*nx_portImportExportEventListener)(HUINT32 ulType, HUINT32 ulResult);


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/


typedef	struct
{
	HUINT32			ulSignalLevel;		/**< Percent 환산 값 */
	HUINT32			ulSignalQuality;	/**< Percent 환산 값 */
	HUINT32			ulAgc;				/**< AGC */
	float			fBer;				/**< BER */
	float			fSnr;				/**< SNR */
	float			fSignalInputPower;	/**< dBuV */
} NX_SignalInfo_t;

typedef enum // from DxWakeUpReason_e
{
	eSYSTEM_WAKEUP_NONE			= 0,
	eSYSTEM_WAKEUP_KEY,
	eSYSTEM_WAKEUP_TIMER,
	eSYSTEM_WAKEUP_ACPOWER,
	eSYSTEM_WAKEUP_HDMICEC,
	eSYSTEM_WAKEUP_REBOOT
} SystemWakeUpReason_e;

typedef enum // refer apkPower_CurBoxMode_e
{
	eSYSTEM_POWER_UNDEFINED,
	eSYSTEM_POWER_STANDBY,
	eSYSTEM_POWER_OPERATION,
	eSYSTEM_POWER_REBOOT,
	eSYSTEM_POWER_MAX
} NX_SYS_SystemPower_e;


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
HERROR	NX_PORT_SYSTEM_GetVolumeMax(HUINT32 *pulVolumeMax);
HERROR	NX_PORT_SYSTEM_GetVolume(HUINT32 *pulVolume);
HERROR	NX_PORT_SYSTEM_SetVolume(HUINT32 ulVolume);
HERROR	NX_PORT_SYSTEM_GetMute(HBOOL *pbMute);
HERROR	NX_PORT_SYSTEM_SetMute(HBOOL bMute);

// System Info
HERROR  NX_PORT_SYSTEM_GetModelName(HCHAR *pszModelName, HINT32 lMax);
HERROR  NX_PORT_SYSTEM_GetSoftwareVersion(HCHAR *pszSoftwareVersion, HINT32 lMax);
HERROR  NX_PORT_SYSTEM_GetLoaderVersion(HCHAR *pszLoaderVersion, HINT32 lMax);
HERROR  NX_PORT_SYSTEM_GetCIPlusVersion(HCHAR *pszVersionName);
HERROR  NX_PORT_SYSTEM_GetSystemId(HCHAR *pszSystemId, HINT32 lMax);
HERROR  NX_PORT_SYSTEM_GetUpdateDate(HCHAR *pszUpdateDate, HINT32 lMax);
HERROR  NX_PORT_SYSTEM_GetMicomVersion(HCHAR *pszMicomVersion, HINT32 lMax);
HERROR  NX_PORT_SYSTEM_GetSerialNumber(HCHAR *pszSerialNumber, HINT32 lMax);
HERROR	NX_PORT_SYSTEM_GetFusingInfo(HBOOL *bIsSCS, HBOOL *bIsJTAG, HBOOL *bIsOTP, HBOOL *bIsCWE, HUINT32 *pulMarketID);
HERROR	NX_PORT_SYSTEM_GetFusingInfoForCasConax(HBOOL *bIsSCS, HBOOL *bIsJTAG, HBOOL *bIsOTP, HBOOL *bIsCWE, HUINT32* pulOTPField1, HUINT32* pulOTPField2);

// scenario
HERROR	NX_PORT_SYSTEM_GetMaturityRating(DxRatingAge_e *peRating);
HERROR	NX_PORT_SYSTEM_SetMaturityRating(DxRatingAge_e peRating);

HERROR	NX_PORT_SYSTEM_GetBlockCHSetting(HBOOL *pbEnabled);
HERROR	NX_PORT_SYSTEM_SetBlockCHSetting(HBOOL pbEnabled);

HERROR	NX_PORT_SYSTEM_GetMainLanguage(HCHAR *pszLanguage);
HERROR	NX_PORT_SYSTEM_SetMainLanguage(const HCHAR *pszLanguage);

HERROR	NX_PORT_SYSTEM_GetAudioLanguage(HINT32 lIndex, HCHAR *pszLanguage);
HERROR	NX_PORT_SYSTEM_SetAudioLanguage(HINT32 lIndex, const HCHAR *pszLanguage);

HERROR	NX_PORT_SYSTEM_GetSubtitleLanguage(HINT32 lIndex, HCHAR *pszLanguage);
HERROR	NX_PORT_SYSTEM_SetSubtitleLanguage(HINT32 lIndex, const HCHAR *pszLanguage);

HERROR	NX_PORT_SYSTEM_GetSubtitleSetting(HBOOL *pbSubtitle);
HERROR	NX_PORT_SYSTEM_SetSubtitleSetting(HBOOL bSubtitle);

HERROR	NX_PORT_SYSTEM_GetOperatorType(DxOperator_e *peOpType);
HERROR	NX_PORT_SYSTEM_SetOperatorType(DxOperator_e eOpType);

HERROR	NX_PORT_SYSTEM_GetCountryCode(HxCountry_e *peCountryCode);
HERROR	NX_PORT_SYSTEM_SetCountryCode(HxCountry_e eCountryCode);

HERROR	NX_PORT_SYSTEM_GetTvAspectRatio(DxAspectRatio_e *peTVAspectRatio);
HERROR	NX_PORT_SYSTEM_SetTvAspectRatio(DxAspectRatio_e eTVAspectRatio);

HERROR	NX_PORT_SYSTEM_GetDisplayFormat(DxDfcSelect_e *peDfcSelect);
HERROR	NX_PORT_SYSTEM_SetDisplayFormat(DxDfcSelect_e eDfcSelect);

HERROR	NX_PORT_SYSTEM_GetResolutionSelect(DxResolutionSelect_e *peResolutionSelect);
HERROR	NX_PORT_SYSTEM_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect);

HERROR	NX_PORT_SYSTEM_GetSpdifAudioOutputMode(DxDigitalAudioFormat_e *peSpdifOutput);
HERROR	NX_PORT_SYSTEM_SetSpdifAudioOutputMode(DxDigitalAudioFormat_e eSpdifOutput);

HERROR	NX_PORT_SYSTEM_GetHdmiAudioOutputMode(DxDigitalAudioFormat_e *peHdmiOutput);
HERROR	NX_PORT_SYSTEM_SetHdmiAudioOutputMode(DxDigitalAudioFormat_e eHdmiOutput);

HERROR	NX_PORT_SYSTEM_GetSoundMode(DxStereoSelect_e *peAudioConfig);
HERROR	NX_PORT_SYSTEM_SetSoundMode(DxStereoSelect_e eAudioConfig);

HERROR	NX_PORT_SYSTEM_GetLipSyncDelay(HUINT32 *pulLipSyncDelay);
HERROR	NX_PORT_SYSTEM_SetLipSyncDelay(HUINT32 ulLipSyncDelay);

HBOOL	NX_PORT_SYSTEM_CheckPinCode(HCHAR *pszPinStr);
HBOOL	NX_PORT_SYSTEM_PINCODE_Verify(const HCHAR *pszPinStr);
HERROR	NX_PORT_SYSTEM_ResetPinCode(HCHAR *pszPinStr);

HERROR	NX_PORT_SYSTEM_SetAvEnable(HBOOL bFlag);

HERROR	NX_PORT_SYSTEM_GetLastServiceGroup(HINT32 *pnLastSvcGroup);
HERROR	NX_PORT_SYSTEM_SetLastServiceGroup(HINT32 nLastSvcGroup);

HERROR	NX_PORT_SYSTEM_GetAutomaticPowerdownOnOff(HBOOL *pbOnOff);
HERROR	NX_PORT_SYSTEM_SetAutomaticPowerdownOnOff(HBOOL bOnOff);

HERROR	NX_PORT_SYSTEM_GetStandByPowerMode(HINT32 *plStandByPowerMode);
HERROR	NX_PORT_SYSTEM_SetStandByPowerMode(HINT32 lStandByPowerMode);

HERROR 	NX_PORT_SYSTEM_GetSWUpdateVersion(HCHAR *pszSWUpdateVersion, HINT32 nMax);

// HDMI
HERROR	NX_PORT_SYSTEM_GetHdmiSupportCEC(HBOOL *pbSupport);
HERROR	NX_PORT_SYSTEM_GetHdmiHdcpVersion(DxHdcpVersion_e *peVersion);
HERROR	NX_PORT_SYSTEM_GetHdmiHdcpStatus(DxHdcpStatus_e *peHdcpStatus);
HERROR	NX_PORT_SYSTEM_GetHdmiEdidResolutionCapability(DxResolutionStatus_b *pulResolutionCapability);
HERROR	NX_PORT_SYSTEM_GetHdmiCecMode(HBOOL *pbHdmiCecMode);
HERROR	NX_PORT_SYSTEM_SetHdmiCecMode(HBOOL bHdmiCecMode);
HERROR	NX_PORT_SYSTEM_GetHdmiCecVolumeMode(HBOOL *pbHdmiCecVolumeMode);
HERROR	NX_PORT_SYSTEM_SetHdmiCecVolumeMode(HBOOL bHdmiCecVolumeMode);

HERROR	NX_PORT_SYSTEM_GetHdmiIsPlugIn(HBOOL *pbPlugInOut);

// Signal Info - Tunner
HERROR	NX_PORT_SYSTEM_GetTunerDeviceNum(HUINT32 *ulTunerNum);
HERROR	NX_PORT_SYSTEM_GetTunerStatus(HINT32 lIdx, HBOOL *bTunerLock);
HERROR	NX_PORT_SYSTEM_GetSignalInfo(HINT32 lIdx, NX_SignalInfo_t *pStSignalInfo);
HERROR	NX_PORT_SYSTEM_GetTuneParam(HINT32 lIdx, DxTuneParam_t *pstTuneParam);

// Anteann Info
HERROR	NX_PORT_SYSTEM_SetAntennaConnection(DxAntennaConnection_e eConnectionType);
HERROR	NX_PORT_SYSTEM_GetAntennaConnection(DxAntennaConnection_e *peConnectionType);

// life cycle
HERROR	NX_PORT_SYSTEM_GetFirstTimeBoot( HBOOL *pbFirstTimeBoot);
HERROR NX_PORT_SYSTEM_SetFirstTimeBoot( HBOOL bFirstTimeBoot);

HERROR	NX_PORT_SYSTEM_GetWakeupReason(SystemWakeUpReason_e *peWakeupReason);
HBOOL	NX_PORT_SYSTEM_IsStandbyMode(void);
HERROR	NX_PORT_SYSTEM_RequestShutDown(void);
HERROR	NX_PORT_SYSTEM_RequestReboot(void);
HERROR	NX_PORT_SYSTEM_RequestOperation(void);
HBOOL	NX_PORT_SYSTEM_IsPowerStateStandbyMode(void);
HERROR	NX_PORT_SYSTEM_GetCustomCode(HUINT32 *pulCustomCode);
HERROR	NX_PORT_SYSTEM_GetColdBootDone(HBOOL *pbCheckColdBootDone);
void 	NX_PORT_SYSTEM_DoFactoryDefault(void);

#if defined(CONFIG_SUPPORT_IPEPG_APP)
HERROR	NX_PORT_SYSTEM_SetEnableIPEpg(HBOOL bEnableIPEpg);
HERROR	NX_PORT_SYSTEM_GetEnableIPEpg(HBOOL *pbEnableIPEpg);
#endif//CONFIG_SUPPORT_IPEPG_APP

#if	defined(CONFIG_MW_MM_PVR)
HERROR	NX_PORT_SYSTEM_GetEnableSeriesPlay(HBOOL *pbSeriesPlayEnable);
HERROR	NX_PORT_SYSTEM_SetEnableSeriesPlay(HBOOL bSeriesPlayEnable);
HERROR	NX_PORT_SYSTEM_GetInstantReplayTime(HUINT32 *pulInstantReplayTime);
HERROR	NX_PORT_SYSTEM_SetInstantReplayTime(HUINT32 ulInstantReplayTime);
HERROR	NX_PORT_SYSTEM_GetSkipForwardTime(HUINT32 *pulSkipForwardTime);
HERROR	NX_PORT_SYSTEM_SetSkipForwardTime(HUINT32 ulSkipForwardTime);
HERROR	NX_PORT_SYSTEM_GetEndPaddingTime(HUINT32 *pulEndPaddingTime);
HERROR	NX_PORT_SYSTEM_SetEndPaddingTime(HUINT32 ulEndPaddingTime);
HERROR	NX_PORT_SYSTEM_GetStartPaddingTime(HUINT32 *pulStartPaddingTime);
HERROR	NX_PORT_SYSTEM_SetStartPaddingTime(HUINT32 ulStartPaddingTime);
HERROR	NX_PORT_SYSTEM_GetAutoDelete(HBOOL *pbAutoDelete);
HERROR	NX_PORT_SYSTEM_SetAutoDelete(HBOOL bAutoDelete);
#endif//CONFIG_MW_MM_PVR

HERROR	NX_PORT_SYSTEM_ExportDBToUsb(nx_portImportExportEventListener listener, HBOOL bSync);
HERROR	NX_PORT_SYSTEM_ImportDBFromUsb(nx_portImportExportEventListener listener, HBOOL bSync);

HERROR	NX_PORT_SYSTEM_GetDayLightSavingTime(HBOOL *pbEnabled);
HERROR	NX_PORT_SYSTEM_SetDayLightSavingTime(HBOOL bEnabled);

HERROR	NX_PORT_SYSTEM_GetGMTOffset(HINT32 *pnGmtOffset);
HERROR	NX_PORT_SYSTEM_SetGMTOffset(HINT32 nGmtOffset);


void	NX_PORT_SYSTEM_POWER_RegisterNotifier(void);
void	NX_PORT_SYSTEM_AUDIO_RegisterNotifier(void);
void	NX_PORT_SYSTEM_HDMI_RegisterNotifier(void);
void	NX_PORT_SYSTEM_THERMAL_RegisterNotifier(void);
void	NX_PORT_SYSTEM_CHANNEL_IMPORT_EXPORT_RegisterNotifier(void);

#endif // __NX_PORT_SYSTEM_H__

