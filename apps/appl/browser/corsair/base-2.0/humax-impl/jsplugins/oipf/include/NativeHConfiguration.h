/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_HUMAX_CONFIGURATION
#define __NATIVE_HUMAX_CONFIGURATION

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   \defgroup Configuration Configuration and Settings
   @{

   This is the native interface for the Configuration and setting APIs
   as defined in DAE 7.3.
 */

#ifdef OIPF
typedef void *AVOutputHandle;

typedef void *HNetworkInterfaceHandle;
typedef void *HNetworkHandle;
typedef	void *NetworkApHandle;
typedef void (NetworkEventCallback)( int event, HNetworkHandle id, int nDevIndex );

typedef void *HMX_OutputHandle;
typedef void *HMX_AudioHandle;
typedef	void *TunerHandle;
typedef	void *SignalInfoHandle;
typedef	void *AntennaInfoHandle;

#endif /* OIPF */

/*------------------------------------------
  Configuration
  ------------------------------------------*/
/** @name Configuration
	The Configuration class as defined in DAE 7.3.2
	@{*/

#if defined(HMX_WEBUI)
/**
  defined by HUMAX
 */
typedef enum {
	SUBTITLE_FONT_TYPE_FIXED,
	SUBTITLE_FONT_TYPE_VARIABLE
} SubtitleFontType;

typedef enum {
	DB_RESTORE_LAST = 0,
	DB_RESTORE_FACTORY
} DBRestoreType;

typedef enum {
	STANDBY_NORMAL = 0,
	STANDBY_POWER_SAVING,
	STANDBY_WAKEUP_ON_LAN
} StandByType;

/* Please matched with dlib_hotplugs.h of DxHotplug_DevType_e */
typedef enum {
	HOTPLUG_DEVTYPE_NONE	= 0,
	HOTPLUG_DEVTYPE_HDD,
	HOTPLUG_DEVTYPE_USB,
	HOTPLUG_DEVTYPE_USB_STOR,
	HOTPLUG_DEVTYPE_USB_INPUT,
	HOTPLUG_DEVTYPE_USB_WIRELESS,
	HOTPLUG_DEVTYPE_SD,
	HOTPLUG_DEVTYPE_MIC,
	HOTPLUG_DEVTYPE_HID,

	HOTPLUG_DEVTYPE_ALL		= 0xFF
} HotplugDevType;
#endif

/**
   Gets the OIPF property preferredAudioLanguage as defined in DAE 7.3.2.1
   \param retval Retval as defined in  DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPreferredAudioLanguage(const char **retval);

/**
   Gets the OIPF property preferredSecondAudioLanguage as defined in DAE 7.3.2.1
   \param retval Retval as defined in  DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPreferredSecondAudioLanguage(const char **retval);

#if defined OIPF || defined OPT_HBBTV_EXT_CONFIG
/**
   Sets the OIPF property preferredAudioLanguage as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPrefferedAudioLangue(const char *value);

/**
   Sets the OIPF property preferredSecondAudioLanguage as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPrefferedSecondAudioLangue(const char *value);

#endif  /* OIPF || OPT_HBBTV_EXT_CONFIG */

/**
   Gets the OIPF property preferredSubtitleLanguage as defined in DAE 7.3.2.1
   \param retval Retval as defined in  DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPreferredSubtitleLanguage(const char **retval);

/**
   Gets the OIPF property preferredSecondSubtitleLanguage as defined in DAE 7.3.2.1
   \param retval Retval as defined in  DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPreferredSecondSubtitleLanguage(const char **retval);


#if defined OIPF || defined OPT_HBBTV_EXT_CONFIG
/**
   Sets the OIPF property preferredSubtitleLanguage as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPreferredSubtitleLanguage(const char *value);

/**
   Sets the OIPF property preferredSecondSubtitleLanguage as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPreferredSecondSubtitleLanguage(const char *value);

#endif /* OIPF || OPT_HBBTV_EXT_CONFIG */

/**
   Gets the OIPF property countryId as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetCountryId(const char **retval);

#if defined OIPF || defined OPT_HBBTV_EXT_CONFIG
/**
   Sets the OIPF property countryId as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetCountryId(const char *value);

/**
   Gets the OIPF property operatorid defined by humax
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetOperatorId(OOIFNumber *retval);

/**
   Sets the OIPF property operatorid defined by humax
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetOperatorId(OOIFNumber value);

/**
   Gets the OIPF property regionId as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetRegionId(OOIFNumber *retval);

/**
   Sets the OIPF property regionId as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetRegionId(OOIFNumber value);

/**
   Gets the OIPF property isPINEntryLocked as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetIsPINEntryLocked(OOIFBoolean *retval);

/**
   Gets the OIPF property pvrPolicy as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPvrPolicy(OOIFNumber *retval);

/**
   Sets the OIPF property pvrPolicy as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPvrPolicy(OOIFNumber value);

/**
   Gets the OIPF property pvrSaveEpisodes as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPvrSaveEpisodes(OOIFNumber *retval);

/**
   Sets the OIPF property pvrSaveEpisodes as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPvrSaveEpisodes(OOIFNumber value);

/**
   Gets the OIPF property pvrSaveDays as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPvrSaveDays(OOIFNumber *retval);

/**
   Sets the OIPF property pvrSaveDays as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPvrSaveDays(OOIFNumber value);

/**
   Gets the OIPF property pvrStartPadding as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPvrStartPadding(OOIFNumber *retval);

/**
   Sets the OIPF property pvrStartPadding as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPvrStartPadding(OOIFNumber value);

/**
   Gets the OIPF property pvrEndPadding as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPvrEndPadding(OOIFNumber *retval);

/**
   Sets the OIPF property pvrEndPadding as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPvrEndPadding(OOIFNumber value);

/**
   Gets the OIPF property preferredTimeShiftMode as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPreferredTimeShiftMode(OOIFNumber *retval);

/**
   Sets the OIPF property preferredTimeShiftMode as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPreferredTimeShiftMode(OOIFNumber value);

/**
   This function corresponds to the OIPF function setParentalControlPIN() as
   defined in DAE  7.3.2.2.
   \param oldPcPin As defined in DAE 7.3.2.2
   \param newPcPin As defined in DAE 7.3.2.2
   \param retval Retval as defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetParentalControlPIN(const char *oldPcPin, const char *newPcPin, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function setParentalControlPINEnable() as
   defined in DAE  7.3.2.2.
   \param pcPIN As defined in DAE 7.3.2.2
   \param enable As defined in DAE 7.3.2.2
   \param retval Retval as defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetParentalControlPINEnable(const char *pcPIN, OOIFBoolean enable, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function getParentalControlPINEnable() as
   defined in DAE  7.3.2.2.
   \param retval Retval as defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetParentalControlPINEnable(OOIFBoolean *retval);

/**
   This function corresponds to the OIPF
   function unlockWithParentalControlPIN() as defined in DAE 7.3.2.2.
   \param pcPIN As defined in DAE 7.3.2.2
   \param target Either a ChannelHandle or a ProgrammeHandle
   indicating the object to be unlocked
   \param duration As defined in DAE 7.3.2.2
   \param retval Retval as defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_HConfigUnlockWithParentalControlPIN(const char *pcPIN, void *target, OOIFNumber duration, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function verifyParentalControlPIN() as
   defined in DAE  7.3.2.2.
   \param pcPIN As defined in DAE 7.3.2.2
   \param retval Retval as defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_HConfigVerifyParentalControlPIN(const char *pcPIN, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function setBlockUnrated() as
   defined in DAE  7.3.2.2.
   \param pcPIN As defined in DAE 7.3.2.2
   \param block As defined in DAE 7.3.2.2
   \param retval Retval as defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetBlockUnrated(const char *pcPIN, OOIFBoolean block, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function getBlockUnrated() as
   defined in DAE  7.3.2.2.
   \param retval Retval as defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetBlockUnrated(OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function getText() as
   defined in DAE  7.3.2.2.
   \param key As defined in DAE 7.3.2.2
   \param retval Retval as defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetText(const char *key, const char **retval);

/**
   This function corresponds to the OIPF function setText() as
   defined in DAE  7.3.2.2.
   \param key As defined in DAE 7.3.2.2
   \param value As defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetText(const char *key, const char *value);

#endif  /* OIPF || OPT_HBBTV_EXT_CONFIG */

#if defined(HMX_WEBUI)
/**
   Gets the OIPF -extension property preferredMenuLanguage as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetPreferredMenuLanguage(const char **retval);

/**
   Sets the OIPF - extension property preferredMenuLanguage as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetPreferredMenuLanguage(const char *value);

/**
   Gets the OIPF -extension property standbyPowerMode as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetStandByPowerMode(OOIFNumber *retval);

/**
   Sets the OIPF - extension property standbyPowerMode as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetStandByPowerMode(OOIFNumber v);

/**
   Gets the OIPF -extension property standbySleepMode as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetStandBySleepMode(OOIFNumber *retval);

/**
   Sets the OIPF - extension property standbySleepMode as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetStandBySleepMode(OOIFNumber v);

/**
   Gets the OIPF -extension property standbyBootMode as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetStandByBootMode(OOIFNumber *retval);

/**
   Sets the OIPF - extension property standbyBootMode as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetStandByBootMode(OOIFNumber v);

/**
   Gets the OIPF -extension property standbyWifiPower as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetStandByWifiPower(OOIFNumber *retval);

/**
   Sets the OIPF - extension property standbyWifiPower as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetStandByWifiPower(OOIFNumber v);

/**
   Gets the OIPF -extension property automaticStandby as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetAutomaticStandby(OOIFBoolean *retval);

/**
   Sets the OIPF - extension property automaticStandby as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetAutomaticStandby(OOIFBoolean v);

/**
   Gets the OIPF -extension property autoDelete as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetAutoDelete(OOIFBoolean *retval);

/**
   Sets the OIPF - extension property autoDelete as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetAutoDelete(OOIFBoolean v);

/**
   Gets the OIPF -extension property audioBleepEnabled as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetAudioBleep(OOIFBoolean *retval);

/**
   Sets the OIPF - extension property audioBleepEnabled as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetAudioBleep(OOIFBoolean v);

/**
   Gets the OIPF -extension property subtitleFontType as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetSubtitleFontType(OOIFNumber *retval);

/**
   Sets the OIPF - extension property subtitleFontType as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetSubtitleFontType(OOIFNumber v);

/**
   Gets the OIPF -extension property native_HConfigSetInfoDisplayTimeout as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetInfoDisplayTimeout(OOIFNumber *retval);

/**
   Sets the OIPF - extension property native_HConfigSetInfoDisplayTimeout as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetInfoDisplayTimeout(OOIFNumber v);

/**
   This function corresponds to the HUMAX OIPF function getText() as
   defined by HUMAX
   \param key As defined by HUMAX
   \param retval Retval as defined by HUMAX
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetField(const char *key, const char **retval);

/**
   This function corresponds to the HUMAX OIPF function setText() as
   defined by HUMAX.
   \param key As defined by HUMAX
   \param value As defined by HUMAX
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetField(const char *key, const char *value);

/**
   This function corresponds to the HUMAX OIPF function doFactoryDefault() as
   defined by HUMAX.
   \param key As defined by HUMAX
   \param value As defined by HUMAX
   \return OOIF return code
*/
OOIFReturnCode native_HConfigDoFactoryDefault(const char *Default_Values);

/**
   Sets the OIPF - extension property tsr as defined by HUMAX
   \param value is boolean
   \return OOIF return code
*/

OOIFReturnCode native_HConfigGetTimeshiftEnabled(OOIFBoolean *retval);

/**
   Sets the OIPF - extension property tsr as defined by HUMAX
   \param value is boolean
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetTimeshiftEnabled(OOIFBoolean v);

/**
   Gets the OIPF - extension property favChListNumberingMode as defined by HUMAX
   \param value is boolean
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetFavChListNumberingMode(OOIFBoolean *retval);

/**
   Sets the OIPF - extension property favChListNumberingMode as defined by HUMAX
   \param value is boolean
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetFavChListNumberingMode(OOIFBoolean v);

/**
   Sets the OIPF - extension property skipForwardTime as defined by HUMAX
   \param value is integer
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetDescrambleOnOff(OOIFBoolean *retval);
/**
   Sets the OIPF - extension property DescrambleOnOff as defined by HUMAX
   \param value is integer
   \return OOIF return code
*/

OOIFReturnCode native_HConfigSetDescrambleOnOff(OOIFBoolean v);
/**
   Sets the OIPF - extension property DescrambleOnOff as defined by HUMAX
   \param value is integer
   \return OOIF return code
*/

OOIFReturnCode native_HConfigGetSkipForwardTime(OOIFNumber *retval);

/**
   Sets the OIPF - extension property skipForwardTime as defined by HUMAX
   \param value is integer
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetSkipForwardTime(OOIFNumber v);

/**
   Sets the OIPF - extension property instantReplayTime as defined by HUMAX
   \param value is integer
   \return OOIF return code
*/

OOIFReturnCode native_HConfigGetInstantReplayTime(OOIFNumber *retval);

/**
   Sets the OIPF - extension property instantReplayTime as defined by HUMAX
   \param value is integer
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetInstantReplayTime(OOIFNumber v);

////////// First Time Boot Flag..
OOIFReturnCode native_HConfigGetFirstTimeBoot(OOIFBoolean *retval);
OOIFReturnCode native_HConfigSetFirstTimeBoot(OOIFBoolean v);

OOIFReturnCode native_HConfigDoDbRestore(const char *ModelName, OOIFNumber type_of_restore, OOIFNumber *retval);

/**
   Sets the OIPF - extension property tsr as defined by HUMAX
   \param value is boolean
   \return OOIF return code
*/

OOIFReturnCode native_HConfigGetCamNameBySlot(OOIFNumber v, const char **retval);
OOIFReturnCode native_HConfigGetNcdNetworkMsg(const char **retval);

OOIFReturnCode native_HConfigGetChannelSetupPIN(OOIFBoolean *retval);
OOIFReturnCode native_HConfigSetChannelSetupPIN(OOIFBoolean v);
OOIFReturnCode native_HConfigGetGuidancePolicy(OOIFNumber *retval);
OOIFReturnCode native_HConfigSetGuidancePolicy(OOIFNumber v);

OOIFReturnCode native_HConfigGetHdmiCecMode(OOIFBoolean *retval);
OOIFReturnCode native_HConfigSetHdmiCecMode(OOIFBoolean v);
OOIFReturnCode native_HConfigGetHdmiCecVolumeCtrlMode(OOIFBoolean *retval);
OOIFReturnCode native_HConfigSetHdmiCecVolumeCtrlMode(OOIFBoolean v);


OOIFReturnCode native_HConfigGetTargetStorage(OOIFNumber *retval);
OOIFReturnCode native_HConfigSetTargetStorage(OOIFNumber v);
OOIFReturnCode native_HConfigSetServerActivationTime(OOIFNumber startHour, OOIFNumber startMin, OOIFNumber durationMin, OOIFBoolean bEnable);

/* HBBTV */
OOIFReturnCode native_HConfigSetHbbTvEnabled(OOIFBoolean v);
OOIFReturnCode native_HConfigGetHbbTvEnabled(OOIFBoolean *retval);

OOIFReturnCode native_HConfigSetLastServiceGroup(OOIFNumber value);
OOIFReturnCode native_HConfigGetLastServiceGroup(OOIFNumber *retval);

/**
   Gets the OIPF -extension property BatUpdate mode as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetBatUpdateMode(OOIFNumber *retval);

/**
   Sets the OIPF - extension property BatUpdate as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetBatUpdateMode(OOIFNumber v);

/**
   Gets the OIPF -extension property NitLcnUpdate mode as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigGetNitLcnUpdateMode(OOIFNumber *retval);

/**
   Sets the OIPF - extension property NitLcnUpdate mode as defined by HUMAX
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_HConfigSetNitLcnUpdateMode(OOIFNumber v);

/* SES extension */
OOIFReturnCode native_HConfigGetSatRecordConfirmCode(const char *registrationCode, const char **retval);



#endif
/*@}*/

#ifdef OIPF

/*------------------------------------------
  LocalSystem
  ------------------------------------------*/
/** @name LocalSystem
	The LocalSystem class as defined in DAE 7.3.3
	@{*/

/** 7.3.3.1 */

/**
   LocalSystem event types.
*/
typedef enum {
	/** This event corresponds to the onPowerStateChange event defined in DAE 7.3.3.2 */
	HLOCAL_SYSTEM_EVENT_POWER_STATE_CHANGE = 0
#if defined(HMX_WEBUI)
	/** This event corresponds to the onCamStateChaneg event defined by HUMAX */
	,HLOCAL_SYSTEM_EVENT_CAM_STATE_CHANGE
	,HLOCAL_SYSTEM_EVENT_USER_ACTION_IDLE
	,HLOCAL_SYSTEM_EVENT_KEYBOARD_RESULT
	/** This event corresponds to the onSmartcardStateChanged event defined by HUMAX */
	,HLOCAL_SYSTEM_EVENT_SC_STATE_CHANGED
	,HLOCAL_SYSTEM_EVENT_MIC_ATTACHED
	,HLOCAL_SYSTEM_EVENT_MIC_DETACHED
	,HLOCAL_SYSTEM_EVENT_USB_WIRELESS_STATE_CHANGE
	/** This event corresponds to the onThermalProtectionOccured event defined by HUMAX */
	,HLOCAL_SYSTEM_EVENT_THERMAL_PROTECTION_OCCURED
#endif
} HLocalSystemEvent;

/**
	Union used to pass event context info in an event callback. What
	attribute to use is decided by the event type defined by the
	#LocalSystemEvent.
*/
typedef union {
	/** used for LOCAL_SYSTEM_EVENT_POWER_STATE_CHANGE as described in DAE 7.3.3.2 */
	struct {
		OOIFNumber powerState;
	} power_state_change_data;

#if defined(HMX_WEBUI)
	/** used for LOCAL_SYSTEM_EVENT_CAM_STATE_CHANGE as described by HUMAX */
	struct {
		OOIFNumber slotNumber;
		OOIFNumber camState;
	} cam_state_change_data;
	/** used for HLOCAL_SYSTEM_EVENT_SC_STATE_CHANGED as described by HUMAX */
	struct {
		OOIFNumber slotNumber;
		OOIFNumber scState;
	} sc_state_change_data;

	struct {
		OOIFNumber devId;
		OOIFNumber devType;
		OOIFNumber devState;
	} ext_dev_info_data;

	struct {
		char* resultStr;
	} keyboard_result_data;
#endif
} HLocalSystemEventInfo;

/**
   The callback function that the native platform should call to
   dispatch a event. The OOIF plugin will propagate the event
   notification to the registered listeners.
   \param event The event type
   \param info Event context info
   \see native_LocalSystemSetListener
*/
typedef void (*hlocalSystemListener)(HLocalSystemEvent event, HLocalSystemEventInfo* info);

/**
   Registers new listener on the PowerStateChange event.
   \param listener The listener
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_HLocalSystemSetListener(hlocalSystemListener listener);
OOIFReturnCode native_HLocalSystemUnsetListener(void);

OOIFReturnCode native_HLocalSystemGetDeviceID(const char **retval);
OOIFReturnCode native_HLocalSystemGetSystemReady(OOIFBoolean *retval);
OOIFReturnCode native_HLocalSystemGetVendorName(const char **retval);
OOIFReturnCode native_HLocalSystemGetModelName(const char **retval);
OOIFReturnCode native_HLocalSystemGetUpdatedList(const char **retval);
OOIFReturnCode native_HLocalSystemGetChipId(const char **retval);
OOIFReturnCode native_HLocalSystemGetSoftwareVersion(const char **retval);
OOIFReturnCode native_HLocalSystemGetPackageVersion(const char **retval);
OOIFReturnCode native_HLocalSystemGetHardwareVersion(const char **retval);
OOIFReturnCode native_HLocalSystemGetSerialNumber(const char **retval);
OOIFReturnCode native_HLocalSystemGetPvrEnabled(OOIFBoolean *retval);
OOIFReturnCode native_HLocalSystemGetStandbyState(OOIFBoolean *retval);
OOIFReturnCode native_HLocalSystemGetVolume(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemGetMute(OOIFBoolean *retval);
OOIFReturnCode native_HLocalSystemGetOutputs(int *count, AVOutputHandle **handles);
OOIFReturnCode native_HLocalSystemGetTuners(int *count, TunerHandle **handles);
OOIFReturnCode native_HLocalSystemGetNetworkInterfaces(int *count, HNetworkInterfaceHandle **handles);
OOIFReturnCode native_HLocalSystemGetTvStandard(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemGetPvrSupport(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemSetStandbyState(OOIFBoolean value);

OOIFReturnCode native_HGetSESSatrecordConfirmcode(const char **retval);
OOIFReturnCode native_HSetSESSatrecordConfirmcode(const char *retval);

OOIFReturnCode	native_HLocalSystemGetPowerState(OOIFNumber *retval);
OOIFReturnCode	native_HLocalSystemGetPreviousPowerState(OOIFNumber *retval);
OOIFReturnCode	native_HLocalSystemGetTimeCurrentPowerState(OOIFNumber *retval);

OOIFReturnCode native_HLocalSystemSetVolume(OOIFNumber value);
OOIFReturnCode native_HLocalSystemSetMute(OOIFBoolean value);
/** 7.3.3.1 */
OOIFReturnCode native_HLocalSystemSetScreenSize(OOIFNumber width, OOIFNumber height, OOIFBoolean *retval);
OOIFReturnCode native_HLocalSystemSetPvrSupport(OOIFNumber state, OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemSetPowerState(OOIFNumber type, OOIFBoolean *retval);

/* OIPF 2.1 */
OOIFReturnCode native_HLocalSystemGetReleaseVersion(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemGetMajorVersion(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemGetMinorVersion(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemGetOipfProfile(const char **retval);
OOIFReturnCode native_HLocalSystemGetCiplusEnabled(OOIFBoolean *retval);
OOIFReturnCode native_HLocalSystemGetPowerState(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemGetPreviousPowerState(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemGetTimeCurrentPowerState(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemGetHCRData(const char **retval);
OOIFReturnCode native_HLocalSystemCheckHCRData(OOIFBoolean *retval);




#if defined(HMX_WEBUI)
OOIFReturnCode native_HLocalSystemGetHmxOutputs(int *count, HMX_OutputHandle **handles);
OOIFReturnCode native_HLocalSystemGetHmxAudios(int *count, HMX_AudioHandle **handles);
OOIFReturnCode native_HLocalSystemGetMicrophoneVolume(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemGetMicrophoneEnabled(OOIFBoolean *retval);
OOIFReturnCode native_HLocalSystemGetConnectedMicrophone(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemSetMicrophoneVolume(OOIFNumber value);
OOIFReturnCode native_HLocalSystemSetMicrophoneEnabled(OOIFBoolean value);
OOIFReturnCode native_HLocalSystemGetAutomaticPowerdown(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemSetAutomaticPowerdown(OOIFNumber value);
OOIFReturnCode native_HLocalSystemGetPriorityOutputs(OOIFNumber *retval);
OOIFReturnCode native_HLocalSystemSetPriorityOutputs(OOIFNumber value);
#endif

/*@}*/
/*------------------------------------------
  NetworkInterface
  ------------------------------------------*/

/**
   Called when the NetworkInterface instance is destroyed in the OOIF plugin.
   \param id NetworkInterface instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_HNetworkInterfaceReleaseHandle(HNetworkInterfaceHandle id);

/** @name NetworkInterface
	The NetworkInterface class as defined in DAE 7.3.4
	@{*/

/** 7.3.4.1 */
OOIFReturnCode native_HNetworkInterfaceGetIndex(HNetworkInterfaceHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HNetworkInterfaceGetIPAddress(HNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkInterfaceGetMacAddress(HNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkInterfaceGetConnected(HNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HNetworkInterfaceGetEnabled(HNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HNetworkInterfaceSetEnabled(HNetworkInterfaceHandle identifier, OOIFBoolean enabled);
#if defined(HMX_WEBUI)
OOIFReturnCode native_HNetworkInterfaceGetNetmask(HNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkInterfaceGetGateway(HNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkInterfaceGetDns1(HNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkInterfaceGetDns2(HNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkInterfaceGetDHCPStatus(HNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HNetworkInterfaceGetDNSAutoStatus(HNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HNetworkInterfaceGetNetworkType(HNetworkInterfaceHandle identifier, OOIFNumber *retval);

OOIFReturnCode native_HNetworkInterfaceGetIpv6Address(HNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkInterfaceGetIpv6Prefix(HNetworkInterfaceHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HNetworkInterfaceGetIpv6Gateway(HNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkInterfaceGetIpv6Dns1(HNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkInterfaceGetIpv6Dns2(HNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkInterfaceGetIpv6DHCPStatus(HNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HNetworkInterfaceGetIpv6DNSAutoStatus(HNetworkInterfaceHandle identifier, OOIFBoolean *retval);

OOIFReturnCode native_HNetworkInterfaceSetIPAddres(HNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_HNetworkInterfaceSetNetmask(HNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_HNetworkInterfaceSetGateway(HNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_HNetworkInterfaceSetDNS1(HNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_HNetworkInterfaceSetDNS2(HNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_HNetworkInterfaceSetDHCPStatus(HNetworkInterfaceHandle identifier, OOIFBoolean bOn );
OOIFReturnCode native_HNetworkInterfaceSetDNSAutoStatus(HNetworkInterfaceHandle identifier, OOIFBoolean bOn );

OOIFReturnCode native_HNetworkInterfaceSetIpv6Addres(HNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_HNetworkInterfaceSetIpv6Prefix(HNetworkInterfaceHandle identifier, OOIFNumber value);
OOIFReturnCode native_HNetworkInterfaceSetIpv6Gateway(HNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_HNetworkInterfaceSetIpv6DNS1(HNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_HNetworkInterfaceSetIpv6DNS2(HNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_HNetworkInterfaceSetIpv6DHCPStatus(HNetworkInterfaceHandle identifier, OOIFBoolean bOn );
OOIFReturnCode native_HNetworkInterfaceSetIpv6DNSAutoStatus(HNetworkInterfaceHandle identifier, OOIFBoolean bOn );

OOIFReturnCode native_HNetworkInterfaceApplyConfigure(HNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HNetworkInterfaceStopConnecting(HNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HNetworkInterfaceCheckGatewayAccess(HNetworkInterfaceHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HNetworkInterfaceCheckDnsAccess(HNetworkInterfaceHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HNetworkInterfaceCheckInternetConnection(HNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HNetworkInterfaceRegisterEventHandler(HNetworkInterfaceHandle identifier, NetworkEventCallback cbFunc);
OOIFReturnCode native_HNetworkInterfaceUnRegisterEventHandler(HNetworkInterfaceHandle identifier);
OOIFReturnCode native_HNetworkInterfaceRefreshAddressInfo(HNetworkInterfaceHandle identifier);

OOIFReturnCode native_HNetworkManagerDefaultDevIndex(OOIFNumber *retval);
OOIFReturnCode native_HNetworkManagerLinkStatus(OOIFNumber *retval);
OOIFReturnCode native_HNetworkManagerRegisterEventHandler(NetworkEventCallback cbFuc);
OOIFReturnCode native_HNetworkManagerUnRegisterEventHandler(void);

OOIFReturnCode native_HNetworkManagerGetFTPServer(OOIFNumber *retval);
OOIFReturnCode native_HNetworkManagerSetFTPServer(OOIFNumber bOn);

OOIFReturnCode native_HNetworkDMSGetStatus(OOIFNumber *retval);
OOIFReturnCode native_HNetworkDMSSetAcionMode(OOIFNumber mode);

OOIFReturnCode native_HNetworkPPPoEConfigRegisterEventHandler(NetworkEventCallback cbFuc);
OOIFReturnCode native_HNetworkPPPoEConfigUnRegisterEventHandler(void);

OOIFReturnCode native_HNetworkPPPoEConfigGetInfo(int nDevIndex, char **szId, char **szPassword);
OOIFReturnCode native_HNetworkPPPoEConfigSetInfo(int nDevIndex, const char *szId, const char *szPassword);
OOIFReturnCode native_HNetworkPPPoEConfigGetOnOff(int nDevIndex, OOIFBoolean *bOn);
OOIFReturnCode native_HNetworkPPPoEConfigSetOnOff(int nDevIndex, OOIFBoolean bOn);
OOIFReturnCode native_HNetworkPPPoEConfigConnect(int nDevIndex, const char *szId, const char *szPassword, OOIFNumber *retval);
OOIFReturnCode native_HNetworkPPPoEConfigDisconnect(int nDevIndex, OOIFNumber *retval);

OOIFReturnCode native_HNetworkWiFiConfigApScanning( int nDevIndex, OOIFNumber *retval);
OOIFReturnCode native_HNetworkWiFiConfigApplyConnectToAP( int nDevIndex, void * identifier, const char *szkey, OOIFBoolean *retval);
OOIFReturnCode native_HNetworkWiFiConfigApplyConnectToAPbyWPS( int nDevIndex, int nType, const char *szPincode, OOIFBoolean *retval);
OOIFReturnCode native_HNetworkWiFiGetConnectedApInfo( int nDevIndex, NetworkApHandle * identifier);
OOIFReturnCode native_HNetworkWiFiGetUserAp( int nDevIndex, NetworkApHandle * identifier);
OOIFReturnCode native_HNetworkWiFiGetFWVersion( int nDevIndex, const char **retval);

OOIFReturnCode native_HNetworkAPScanningResultListener(NetworkEventCallback cbFunc);
OOIFReturnCode native_HNetworkAPScanningResultListenerRelease(NetworkEventCallback cbFunc);

OOIFReturnCode native_HNetworkAPGetEssid(NetworkApHandle identifier, const char **retval);
OOIFReturnCode native_HNetworkAPSetEssid(NetworkApHandle identifier, const char *retval);
OOIFReturnCode native_HNetworkAPGetSecureType(NetworkApHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HNetworkAPSetSecureType(NetworkApHandle identifier, OOIFNumber retval);
OOIFReturnCode native_HNetworkAPGetAuthenType(NetworkApHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HNetworkAPSetAuthenType(NetworkApHandle identifier, OOIFNumber retval);
OOIFReturnCode native_HNetworkAPGetEncryptType(NetworkApHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HNetworkAPSetEncryptType(NetworkApHandle identifier, OOIFNumber retval);
OOIFReturnCode native_HNetworkAPGetWaveStrength(NetworkApHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HNetworkAPGetConnectSpeed(NetworkApHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HNetworkAPGetIsConnected(NetworkApHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HNetworkAPGetMacAddr(void * identifier, const char **retval);

OOIFReturnCode native_HNetworkAccessPointReleaseHandle(void * identifier);

#endif

/*@}*/
/*------------------------------------------
  AVOutput
  ------------------------------------------*/

/**
   Called when the AVOutput instance is destroyed in the OOIF plugin.
   \param id AVOutput instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_HAVOutputReleaseHandle(AVOutputHandle id);

/** @name AVOutput
	The AVOutput class as defined in DAE 7.3.5
	@{*/

/** 7.3.5.1 */
OOIFReturnCode native_HAVOutputGetName(AVOutputHandle identifier, const char **retval);
OOIFReturnCode native_HAVOutputGetType(AVOutputHandle identifier, const char **retval);
OOIFReturnCode native_HAVOutputGetEnabled(AVOutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HAVOutputGetSubtitleEnabled(AVOutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HAVOutputGetVideoMode(AVOutputHandle identifier, const char **retval);
OOIFReturnCode native_HAVOutputGetDigitalAudioMode(AVOutputHandle identifier, const char **retval);
OOIFReturnCode native_HAVOutputGetAudioRange(AVOutputHandle identifier, const char **retval);
OOIFReturnCode native_HAVOutputGetHDVideoFormat(AVOutputHandle identifier, const char **retval);
OOIFReturnCode native_HAVOutputGetTVAspectRatio(AVOutputHandle identifier, const char **retval);

OOIFReturnCode native_HAVOutputSetEnabled(AVOutputHandle identifier, OOIFBoolean enabled);
OOIFReturnCode native_HAVOutputSetSubtitleEnabled(AVOutputHandle identifier, OOIFBoolean enabled);
OOIFReturnCode native_HAVOutputSetVideoMode(AVOutputHandle identifier, const char *value);
OOIFReturnCode native_HAVOutputSetDigitalAudioMode(AVOutputHandle identifier, const char *value);
OOIFReturnCode native_HAVOutputSetAudioRange(AVOutputHandle identifier, const char *value);
OOIFReturnCode native_HAVOutputSetHDVideoFormat(AVOutputHandle identifier, const char *value);
OOIFReturnCode native_HAVOutputSetTVAspectRatio(AVOutputHandle identifier, const char *value);

/*@}*/
/*@}*/
#endif /* OIPF */

#if defined(HMX_WEBUI)
/**
   This function corresponds to the OIPF function getText() as
   defined by HUMAX
   \param key As defined by HUMAX
   \param retval Retval as defined by HUMAX
   \return OOIF return code
*/
OOIFReturnCode native_HLocalSystemGetField(const char *key, const char **retval);

/**
   This function corresponds to the OIPF function setText() as
   defined by HUMAX.
   \param key As defined by HUMAX
   \param value As defined by HUMAX
   \return OOIF return code
*/
OOIFReturnCode native_HLocalSystemSetField(const char *key, const char *value);

/**
  Box First Boot Status Management

*/
OOIFReturnCode native_HLocalSystemSetBoxFirstBootStatus(OOIFBoolean nStatus);
OOIFReturnCode native_HLocalSystemGetBoxFirstBootStatus(OOIFBoolean *retval);

/**
  Box launch app mode
  0 : Main Application
  1 : Diagnostics Application
*/
OOIFReturnCode native_HLocalSystemGetLaunchAppMode(OOIFNumber *retval);

/**
  Áö»óÆÄ Antenna Power Status Management

*/
OOIFReturnCode native_HLocalSystemSetAntennaPowerStatus(OOIFBoolean nStatus);
OOIFReturnCode native_HLocalSystemGetAntennaPowerStatus(OOIFBoolean *retval);

/**
   Dynamic Channel Update Management

*/
OOIFReturnCode native_HLocalSystemSetServiceUpdateFlag(OOIFBoolean nStatus);
OOIFReturnCode native_HLocalSystemGetServiceUpdateFlag(OOIFBoolean *retval);

/**
  check file existence

*/
OOIFReturnCode native_HLocalSystemCheckFileExistence( const char *uri, OOIFBoolean *retval);

/**
  Call Full Browser Native Notification

*/
OOIFReturnCode native_HLocalSystemShowMsgbox(OOIFBoolean *retval);

OOIFReturnCode native_HLocalSystemShowKeyboard(OOIFBoolean *retval, OOIFNumber type, OOIFBoolean pwmode, const char *str);




/*------------------------------------------
  HMX_Output
  ------------------------------------------*/

/**
   Called when the HMX_Output instance is destroyed in the OOIF plugin.
   \param id HMX_Output instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_HOutputReleaseHandle(HMX_OutputHandle id);

/** @name HMX_Output
	The HMX_Output class as nit defined in OIPF
	@{*/

/** no chapter */
OOIFReturnCode native_HOutputGetName(HMX_OutputHandle identifier, const char **retval);
OOIFReturnCode native_HOutputGetType(HMX_OutputHandle identifier, const char **retval);
OOIFReturnCode native_HOutputGetEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetScartEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetTvScartEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetVcrScartEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetCompositeEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetComponentEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetRfEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetHdmiEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetSpdifEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetSupportedTvScartFormats(HMX_OutputHandle identifier, int *count, char ***strings);
OOIFReturnCode native_HOutputGetSupportedVcrScartFormats(HMX_OutputHandle identifier, int *count, char ***strings);
OOIFReturnCode native_HOutputGetTvScartFormat(HMX_OutputHandle identifier, const char **retval);
OOIFReturnCode native_HOutputGetVcrScartFormat(HMX_OutputHandle identifier, const char **retval);
OOIFReturnCode native_HOutputGetVideoStandard(HMX_OutputHandle identifier, const char **retval);
OOIFReturnCode native_HOutputGetAvAddtionalSignal(HMX_OutputHandle identifier, const char **retval);
OOIFReturnCode native_HOutputGetTVAspectRatio(HMX_OutputHandle identifier, const char **retval);
OOIFReturnCode native_HOutputGetWss2HdmiMode(HMX_OutputHandle identifier, const char **retval);
OOIFReturnCode native_HOutputGetVideoDisplayFormat(HMX_OutputHandle identifier, const char **retval);
OOIFReturnCode native_HOutputGetCurrentResolution(HMX_OutputHandle identifier, const char **retval);
OOIFReturnCode native_HOutputGetSupportedResolutions(HMX_OutputHandle identifier, int *count, char ***strings);
OOIFReturnCode native_HOutputGetAudioDescriptionEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetSubtitleEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HOutputGetHardOfHearingEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval);


OOIFReturnCode native_HOutputSetEnabled(HMX_OutputHandle identifier, OOIFBoolean enabled);
OOIFReturnCode native_HOutputSetTvScartFormat(HMX_OutputHandle identifier, const char *value);
OOIFReturnCode native_HOutputSetVcrScartFormat(HMX_OutputHandle identifier, const char *value);
OOIFReturnCode native_HOutputSetTVAspectRatio(HMX_OutputHandle identifier, const char *value);
OOIFReturnCode native_HOutputSetVideoDisplayFormat(HMX_OutputHandle identifier, const char *value);
OOIFReturnCode native_HOutputSetCurrentResolution(HMX_OutputHandle identifier, const char *value);
OOIFReturnCode native_HOutputSetAudioDescriptionEnabled(HMX_OutputHandle identifier, OOIFBoolean enabled);
OOIFReturnCode native_HOutputSetSubtitleEnabled(HMX_OutputHandle identifier, OOIFBoolean enabled);
OOIFReturnCode native_HOutputSetHardOfHearingEnabled(HMX_OutputHandle identifier, OOIFBoolean enabled);

/*@}*/
/*@}*/

/*------------------------------------------
  HMX_Audio
  ------------------------------------------*/

/**
   Called when the HMX_Output instance is destroyed in the OOIF plugin.
   \param id HMX_Output instance identifier
   \return OOIF_RETURN_OK
 */

OOIFReturnCode native_HAudioReleaseHandle(HMX_AudioHandle id);


/** @name HMX_Audio
	The HMX_AVOutput class as nit defined in OIPF
	@{*/

/** no chapter */
OOIFReturnCode native_HAudioGetName(HMX_AudioHandle identifier, const char **retval);
OOIFReturnCode native_HAudioGetType(HMX_AudioHandle identifier, const char **retval);
OOIFReturnCode native_HAudioGetEnabled(HMX_AudioHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HAudioGetMaxVolume(HMX_AudioHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HAudioGetMinVolume(HMX_AudioHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HAudioGetLipSyncDelay(HMX_AudioHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HAudioGetSoundMode(HMX_AudioHandle identifier, const char **retval);
OOIFReturnCode native_HAudioGetTranscodingEnabled(HMX_AudioHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_HAudioGetDigitalOutputMode(HMX_AudioHandle identifier, const char **retval);
OOIFReturnCode native_HAudioGetDigitalOutputCodec(HMX_AudioHandle identifier, const char **retval);
OOIFReturnCode native_HAudioGetTranscodingMode(HMX_AudioHandle identifier, const char **retval);
OOIFReturnCode native_HAudioGetSupportedSoundOutputModes(HMX_AudioHandle identifier, int *count, char ***strings);
OOIFReturnCode native_HAudioGetSupportedDigitalOutputCodecs(HMX_AudioHandle identifier, int *count, char ***strings);
OOIFReturnCode native_HAudioGetSupportedDigitalOutputModes(HMX_AudioHandle identifier, int *count, char ***strings);
OOIFReturnCode native_HAudioGetSupportedTranscodingModes(HMX_AudioHandle identifier, int *count, char ***strings);

#define ____HMX_Audio_setter_functions____

OOIFReturnCode native_HAudioSetEnabled(HMX_AudioHandle identifier, OOIFBoolean enabled);
OOIFReturnCode native_HAudioSetLipSyncDelay(HMX_AudioHandle identifier, OOIFNumber value);
OOIFReturnCode native_HAudioSetSoundMode(HMX_AudioHandle identifier, const char *value);
OOIFReturnCode native_HAudioSetDigitalOutputMode(HMX_AudioHandle identifier, const char *value);
OOIFReturnCode native_HAudioSetDigitalOutputCodec(HMX_AudioHandle identifier, const char *value);
OOIFReturnCode native_HAudioSetTranscodingMode(HMX_AudioHandle identifier, const char *value);


#define ____HMX_FrontPanel_functions____
OOIFReturnCode native_HFrontPanel_SetString(const char *panelString, int align, int dimlevel, OOIFBoolean bAuto);
OOIFReturnCode native_HFrontPanel_SetLedOnOff(int ledId, OOIFBoolean bOnOff, OOIFBoolean bAuto, OOIFBoolean bAllClear);
OOIFReturnCode native_HFrontPanel_SetAnimation(int fpItemType, int param1, int param2, int param3);
OOIFReturnCode native_HFrontPanel_SetCategoryInform(int nCategory);

#endif



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HUMAX_CONFIGURATION */
