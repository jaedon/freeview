/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABS_MISCSETTING
#define __NATIVE_JLABS_MISCSETTING

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void* jlabsMiscSettingHandle;

/*** B.3.4.1.5. jlabsMiscSetting 클래스 ***/

typedef enum _NETWORKINTERFACE
{
	eJLAPS_DTT_NETWORK =0,
	eJLAPS_BS_NETWORK,
	eJLAPS_CATV_NETWORK,

	eJLAPS_NETWORK_MAX
}JLAPS_NETWORK_TYPE;

/* GETTER */

/* 문자열 길이 10, 우편번호 7자리 */
OOIFReturnCode native_jlabsMiscSettingGetPostCode(const char **retval);

/* ... */
OOIFReturnCode native_jlabsMiscSettingGetLocalArea(const char **retval);

/* 설정치 16:9 , 콤포짓 출력에서 사용 */
OOIFReturnCode native_jlabsMiscSettingGetTvAspectRatio(const char **retval);

/* "LEFT" -주 , "RIGHT" - 부 , "MIX"- 주 + 부 */
OOIFReturnCode native_jlabsMiscSettingGetSoundMode(const char **retval);

/* "AUTO" / "PCM"  */
OOIFReturnCode native_jlabsMiscSettingGetSpdifMode(const char **retval);

/*	0- 없음, 1- 제1언어 , 2 - 제2언어 */
OOIFReturnCode native_jlabsMiscSettingGetSubtitleMode(OOIFNumber *retval);

/*	0- 없음, 1- 제1언어 , 2 - 제2언어 */
OOIFReturnCode native_jlabsMiscSettingGetSuperImposeMode(OOIFNumber *retval);

/* 0- Normal, 1- wizard, 2- installer */
OOIFReturnCode native_jlabsMiscSettingGetSetupConfigurationLaunchMode(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsMiscSettingGetNumberOfTuners(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsMiscSettingGetDisplayMode3D(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsMiscSettingGetThreeDMode(OOIFBoolean *retval);

/* ... */
OOIFReturnCode native_jlabsMiscSettingGetAutoSoftwareDownloadEnable(OOIFBoolean*retval);

/* ... */
OOIFReturnCode native_jlabsMiscSettingGetChannelMaskEnable(OOIFBoolean *retval);

/* ... */
OOIFReturnCode native_jlabsMiscSettingGetSeamlessEnable(OOIFBoolean *retval);

/* ... */
OOIFReturnCode native_jlabsMiscSettingGetAutoPPVPurchaseEnable(OOIFBoolean *retval);

/*** read only ***/

/* 0- eJLABS_DTT_NETWORK , 1- eJLABS_BS_NETWORK, 2- eJLAS_CATV_NETWORK */
OOIFReturnCode native_jlabsMiscSettingGetLastChannelNetworkTypeName(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsMiscSettingGetLastChannelThreeDigitNumber(OOIFNumber *retval);

#if defined(JLABS_JCOM)
OOIFReturnCode native_jlabsMiscSettingGetLimitPerPPV(const char **retval);
OOIFReturnCode native_jlabsMiscSettingGetLimitPerPPM(const char **retval);
OOIFReturnCode native_jlabsMiscSettingGetHdmiAudioMode(const char **retval);
OOIFReturnCode native_jlabsMiscSettingGetFriendlyName(const char **retval);
OOIFReturnCode native_jlabsMiscSettingGetNaviUrl(const char **retval);
OOIFReturnCode native_jlabsMiscSettingGetInteracTVURL(const char **retval);
OOIFReturnCode native_jlabsMiscSettingGetInternetBrowserURL(const char **retval);
OOIFReturnCode native_jlabsMiscSettingGetVodHomeURL(const char **retval);
OOIFReturnCode native_jlabsMiscSettingGetAcsUrl(const char **retval);
OOIFReturnCode native_jlabsMiscSettingGetPinUnlockExpiration(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetTuningRange(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetTimebarDisplayMode(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetHdmiCecMode(OOIFNumber*retval);
OOIFReturnCode native_jlabsMiscSettingGetAMPermissionLevel(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetHdmiResolutionMode(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetBmlInfoDisplayEnable(OOIFBoolean*retval);
OOIFReturnCode native_jlabsMiscSettingGetIPlateDisplayEnable(OOIFBoolean*retval);
OOIFReturnCode native_jlabsMiscSettingGetFrontDisplayEnable(OOIFBoolean*retval);
OOIFReturnCode native_jlabsMiscSettingGetIsDMSPublic(OOIFBoolean*retval);
OOIFReturnCode native_jlabsMiscSettingGetIsDMSDeliver(OOIFBoolean*retval);
OOIFReturnCode native_jlabsMiscSettingGetIsIPDubbing(OOIFBoolean*retval);

/* read only */
OOIFReturnCode native_jlabsMiscSettingGetLastDTTChannelThreeDigitNumber(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetLastDttChannelBranchNumber(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetLastBSChannelThreeDigitNumber(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetLastCATVChannelThreeDigitNumber(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetLastCS1ChannelThreeDigitNumber(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetLastCS2ChannelThreeDigitNumber(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetLastJCHITSChannelThreeDigitNumber(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetLastChannelBranchNumber(OOIFNumber *retval);

/* 0- 채널마스크 비운용 , 1- 운용패턴 1, 2- 운용패턴 2 */
OOIFReturnCode native_jlabsMiscSettingGetChannelMaskPattern(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetStandbyMode(OOIFNumber *retval);
OOIFReturnCode native_jlabsMiscSettingGetIsOutputHD(OOIFBoolean*retval);
#endif

/* SETTER */
OOIFReturnCode native_jlabsMiscSettingSetPostCode(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetLocalArea(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetTvAspectRatio(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetSoundMode(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetSpdifMode(const char *value);

OOIFReturnCode native_jlabsMiscSettingSetSubtitleMode(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetSuperImposeMode(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetSetupConfigurationLaunchMode(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetNumberOfTuners(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetDisplyMode3D(OOIFNumber value);

OOIFReturnCode native_jlabsMiscSettingSetThreeDMode(OOIFBoolean value);
OOIFReturnCode native_jlabsMiscSettingSetAutoSoftwareDownloadEnable(OOIFBoolean value);
OOIFReturnCode native_jlabsMiscSettingSetChannelMaskEnable(OOIFBoolean value);
OOIFReturnCode native_jlabsMiscSettingSetSeamlessEnable(OOIFBoolean value);
OOIFReturnCode native_jlabsMiscSettingSetAutoPPVPurchaseEnable(OOIFBoolean value);

OOIFReturnCode native_jlabsMiscSettingGet(const char *name, const char **retval);
OOIFReturnCode native_jlabsMiscSettingSet(const char *name, const char *value);

#if defined(JLABS_JCOM)
OOIFReturnCode native_jlabsMiscSettingSetLimitPerPPV(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetLimitPerPPM(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetHdmiAudioMode(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetFriendlyName(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetNaviUrl(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetInteracTVUrl(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetInternetBrowserUrl(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetVodHomeUrl(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetAcsUrl(const char *value);
OOIFReturnCode native_jlabsMiscSettingSetPinUnlockExpiration(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetTuningRange(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetTimebarDisplayMode(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetHdmiResolutionMode(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetStandbyMode(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetHdmiCecMode(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetAmPermissionLevel(OOIFNumber value);
OOIFReturnCode native_jlabsMiscSettingSetBmlInfoDisplayEnable(OOIFBoolean value);
OOIFReturnCode native_jlabsMiscSettingSetIPlateDisplayEnable(OOIFBoolean value);
OOIFReturnCode native_jlabsMiscSettingSetFrontDisplayEnable(OOIFBoolean value);
OOIFReturnCode native_jlabsMiscSettingSetIsDMSPublic(OOIFBoolean value);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABS_MISCSETTING */

#endif // #if defined OIPF && defined JLABS
