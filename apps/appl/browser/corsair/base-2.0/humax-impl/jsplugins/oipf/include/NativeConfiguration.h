/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_CONFIGURATION
#define __NATIVE_CONFIGURATION

#include "NativeOOIFTypes.h"
#ifdef OIPF
#include "NativeTuner.h"
#endif /* OIPF */

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
typedef void *NetworkInterfaceHandle;
typedef void *AVOutputHandle;
#endif /* OIPF */

/*------------------------------------------
  Configuration
  ------------------------------------------*/
/** @name Configuration
	The Configuration class as defined in DAE 7.3.2
	@{*/

/**
   Gets the OIPF property preferredAudioLanguage as defined in DAE 7.3.2.1
   \param retval Retval as defined in  DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetPreferredAudioLanguage(const char **retval);

/**
   Gets the OIPF property preferredSubtitleLanguage as defined in DAE 7.3.2.1
   \param retval Retval as defined in  DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetPreferredSubtitleLanguage(const char **retval);

/**
   Gets the OIPF property countryId as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetCountryId(const char **retval);

#ifdef HBBTV_1_2_1
/**
   Gets the OIPF property prefferedUILanguage as defined in DAE1.2 7.3.2.1
   \param retval Retval as defined in DAE1.2 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetPreferredUILanguage(const char **retval);
#endif /* HBBTV_1_2_1 */
#ifdef HBBTV
/**
   Gets the HbbTV property timeshiftSynchronized as defined in A.2.4.3
   \param retval Retval as defined in A.2.4.3
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetTimeShiftSynchronized(OOIFBoolean *retval);
#endif /* HBBTV */
#ifdef OIPF
/**
   Gets the OIPF property preferredTimeShiftMode as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetPreferredTimeShiftMode(OOIFNumber *retval);
#endif /* OIPF */

/**
   Gets the OIPF property regionId as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetRegionId(OOIFNumber *retval);

/**
   Gets the OIPF property pvrPolicy as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetPvrPolicy(OOIFNumber *retval);

/**
   Gets the OIPF property pvrSaveEpisodes as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetPvrSaveEpisodes(OOIFNumber *retval);

/**
   Gets the OIPF property pvrSaveDays as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/

OOIFReturnCode native_ConfigGetPvrSaveDays(OOIFNumber *retval);
/**
   Gets the OIPF property pvrStartPadding as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetPvrStartPadding(OOIFNumber *retval);

/**
   Gets the OIPF property pvrEndPadding as defined in DAE 7.3.2.1
   \param retval Retval as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetPvrEndPadding(OOIFNumber *retval);

/**
   This function corresponds to the OIPF function getText() as
   defined in DAE 7.3.2.2.
   \param key As defined in DAE 7.3.2.2
   \param retval Retval as defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetText(const char *key, const char **retval);

/**
   This function corresponds to the OIPF function setText() as
   defined in DAE 7.3.2.2.
   \param key As defined in DAE 7.3.2.2
   \param value As defined in DAE 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetText(const char *key, const char *value);

#if defined(HBBTV) && !defined(HBBTV_1_2_1)
/**
   Gets the OIPF property isPINEntryLocked as defined in DAE1.1 7.3.2.1
   \param retval Retval as defined in DAE1.1 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetIsPINEntryLocked(OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function setParentalControlPIN() as
   defined in DAE1.1 7.3.2.2.
   \param oldPcPin As defined in DAE1.1 7.3.2.2
   \param newPcPin As defined in DAE1.1 7.3.2.2
   \param retval Retval as defined in DAE1.1 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetParentalControlPIN(const char *oldPcPin, const char *newPcPin, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function setParentalControlPINEnable() as
   defined in DAE1.1 7.3.2.2.
   \param pcPIN As defined in DAE1.1 7.3.2.2
   \param enable As defined in DAE1.1 7.3.2.2
   \param retval Retval as defined in DAE1.1 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetParentalControlPINEnable(const char *pcPIN, OOIFBoolean enable, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function getParentalControlPINEnable() as
   defined in DAE1.1 7.3.2.2.
   \param retval Retval as defined in DAE1.1 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetParentalControlPINEnable(OOIFBoolean *retval);

/**
   This function corresponds to the OIPF
   function unlockWithParentalControlPIN() as defined in DAE1.1 7.3.2.2.
   \param pcPIN As defined in DAE 7.3.2.2
   \param target Either a ChannelHandle or a ProgrammeHandle
   indicating the object to be unlocked
   \param duration As defined in DAE1.1 7.3.2.2
   \param retval Retval as defined in DAE1.1 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ConfigUnlockWithParentalControlPIN(const char *pcPIN, void *target, OOIFNumber duration, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function verifyParentalControlPIN() as
   defined in DAE1.1 7.3.2.2.
   \param pcPIN As defined in DAE1.1 7.3.2.2
   \param retval Retval as defined in DAE1.1 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ConfigVerifyParentalControlPIN(const char *pcPIN, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function setBlockUnrated() as
   defined in DAE1.1 7.3.2.2.
   \param pcPIN As defined in DAE1.1 7.3.2.2
   \param block As defined in DAE1.1 7.3.2.2
   \param retval Retval as defined in DAE1.1 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetBlockUnrated(const char *pcPIN, OOIFBoolean block, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function getBlockUnrated() as
   defined in DAE1.1 7.3.2.2.
   \param retval Retval as defined in DAE1.1 7.3.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ConfigGetBlockUnrated(OOIFBoolean *retval);
#endif //HBBTV && !HBBTV_1_2_1

#ifdef OIPF
/**
   Sets the OIPF property preferredTimeShiftMode as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetPreferredTimeShiftMode(OOIFNumber value);

/**
   Sets the OIPF property preferredAudioLanguage as defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetPreferedAudioLanguage(const char *value);

/**
   Sets the OIPF property preferredSubtitleLanguage as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetPreferredSubtitleLanguage(const char *value);

/**
   Sets the OIPF property countryId as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetCountryId(const char *value);
#endif // OIPF

/**
   Sets the OIPF property regionId as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetRegionId(OOIFNumber value);

/**
   Sets the OIPF property pvrPolicy as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetPvrPolicy(OOIFNumber value);

/**
   Sets the OIPF property pvrSaveEpisodes as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetPvrSaveEpisodes(OOIFNumber value);

/**
   Sets the OIPF property pvrSaveDays as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetPvrSaveDays(OOIFNumber value);

/**
   Sets the OIPF property pvrStartPadding as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetPvrStartPadding(OOIFNumber value);

/**
   Sets the OIPF property pvrEndPadding as defined in DAE 7.3.2.1
   \param value As defined in DAE 7.3.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ConfigSetPvrEndPadding(OOIFNumber value);

/*@}*/

#ifdef OIPF

/*------------------------------------------
  LocalSystem
  ------------------------------------------*/
/** @name LocalSystem
	The LocalSystem class as defined in DAE 7.3.3
	@{*/

/**
  Constants defined in DAE 7.3.3.1
*/
typedef enum {
	STATE_OFF = 0,
	STATE_ON = 1,
	STATE_PASSIVE_STANDBY = 2,
	STATE_ACTIVE_STANDBY = 3,
	STATE_PASSIVE_STANDBY_HIBERNATE = 4

} SystemState;

/**
   LocalSystem event types.
*/
typedef enum {
	/** This event corresponds to the onPowerStateChange event defined in DAE 7.3.3.2 */
	LOCAL_SYSTEM_EVENT_POWER_STATE_CHANGE = 0,
#ifdef JLABS
	LOCAL_SYSTEM_EVENT_POWER_STATE_CHANGE_ERROR
#endif
} LocalSystemEvent;

#ifdef JLABS
typedef enum {
	/** This event corresponds to the onPowerStateChangeError event defined in C.2.5.1.1. LocalSystem jcom expended */
	POWER_SATATE_CHANGE_ERROR_RECORDING = 1,
	POWER_SATATE_CHANGE_ERROR_SEND_DMS =2,
	POWER_SATATE_CHANGE_ERROR_IP_DUBBING =4,
	POWER_SATATE_CHANGE_ERROR_OTHER =8,
} PowerStateChangeErrorReason;
#endif 
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
#ifdef JLABS
	struct {
		OOIFNumber reason;
	} power_state_change_error_data;
#endif
} LocalSystemEventInfo;

/**
   The callback function that the native platform should call to
   dispatch a event. The OOIF plugin will propagate the event
   notification to the registered listeners.
   \param event The event type
   \param info Event context info
   \see native_LocalSystemSetListener
*/
typedef void (localSystemListener)(LocalSystemEvent event, LocalSystemEventInfo* info);

/**
   Registers new listener on the PowerStateChange event.
   \param listener The listener
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_LocalSystemSetListener(localSystemListener listener);

/**
 * Called when an event listener is added, removed, or accessed from Javascript
 * on the DownloadManager Object.  OOIF will handle it automatically, so no
 * action is needed from the integration layer. Nonetheless, this callback
 * gives the opportunity to prevent the listener from being added, removed, or
 * accessed, by returning an error code. This may be used to implement capability checks as defined in
 * DAE 7 and DAE 9.3
   \param window The identifier of the window the call originates from
   \param host The host URL (domain) of the current context
 * \param eventName the name of the event
 * \return OOIF_RETURN_OK, or OOIF_RETURN_SECURITY_ERROR, or OOIF_RETURN_TYPE_ERROR
 */
OOIFReturnCode native_LocalSystemOnListenerAccessed(long int window, const char *host, const char *eventName);

/**
   Gets the OIPF property deviceID as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetDeviceID(const char **retval);

/**
   Gets the OIPF property systemReady as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetSystemReady(OOIFBoolean *retval);

/**
   Gets the OIPF property vendorName as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetVendorName(const char **retval);

/**
   Gets the OIPF property modelName as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetModelName(const char **retval);

/**
   Gets the OIPF property softwareVersion as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetSoftwareVersion(const char **retval);

/**
   Gets the OIPF property hardwareVersion as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetHardwareVersion(const char **retval);

/**
   Gets the OIPF property serialNumber as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetSerialNumber(const char **retval);

/**
   Gets the OIPF property releaseVersion as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetReleaseVersion(OOIFNumber *retval);

/**
   Gets the OIPF property majorVersion as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetMajorVersion(OOIFNumber *retval);

/**
   Gets the OIPF property minorVersion as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetMinorVersion(OOIFNumber *retval);

/**
   Gets the OIPF property oipfProfile as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetOipfProfile(const char **retval);

/**
   Gets the OIPF property pvrEnabled as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetPvrEnabled(OOIFBoolean *retval);

/**
   Gets the OIPF property ciplusEnabled as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetCiplusEnabled(OOIFBoolean *retval);

/**
   Gets the OIPF property standbyState as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetStandbyState(OOIFBoolean *retval);

/**
   Gets the OIPF property powerState as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetPowerState(OOIFNumber *retval);

/**
   Gets the OIPF property previousPowerState as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetPreviousPowerState(OOIFNumber *retval);

/**
   Gets the OIPF property timeCurrentPowerState as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetTimeCurrentPowerState(OOIFNumber *retval);

/**
   Gets the OIPF property volume as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetVolume(OOIFNumber *retval);

/**
   Gets the OIPF property mute as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetMute(OOIFBoolean *retval);

/**
   Gets the OIPF property outputs as defined in DAE 7.3.3.2.
   \param count Number of components in the returned array
   \param handles Property value as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetOutputs(int *count, AVOutputHandle **handles);

/**
   Gets the OIPF property networkInterfaces as defined in DAE 7.3.3.2.
   \param count Number of components in the returned array
   \param handles Property value as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetNetworkInterfaces(int *count, NetworkInterfaceHandle **handles);

/**
   Gets the OIPF property tuners as defined in DAE 7.3.3.2.
   \param window The identifier of the window the call originates from
   \param host The host URI of the application the create call originates from
   \param count Number of components in the returned array
   \param handles Property value as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetTuners(long int window, const char* host, int *count, TunerHandle **handles);

/**
   Gets the OIPF property tvStandard as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetTvStandard(OOIFNumber *retval);

/**
   Gets the OIPF property pvrSupport as defined in DAE 7.3.3.2.
   \param retval Retval as defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemGetPvrSupport(OOIFNumber *retval);

/**
   Sets the OIPF property standbyState as defined in DAE 7.3.3.2.
   \param value As defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemSetStandbyState(OOIFBoolean value);

/**
   Sets the OIPF property volume as defined in DAE 7.3.3.2.
   \param value As defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemSetVolume(OOIFNumber value);

/**
   Sets the OIPF property mute as defined in DAE 7.3.3.2.
   \param value As defined in 7.3.3.2
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemSetMute(OOIFBoolean value);

/**
   Set the resolution of the graphics plane as defined in DAE 7.3.3.3.
   \param width The width of the display, in pixels
   \param height The height of the display, in pixels
   \param retval As defined in 7.3.3.3
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemSetScreenSize(OOIFNumber width, OOIFNumber height, OOIFBoolean *retval);

/**
   Set the type of PVR support used by the application as defined in DAE 7.3.3.3.
   \param state The type of PVR support desired by the application
   \param retval As defined in 7.3.3.3
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemSetPvrSupport(OOIFNumber state, OOIFNumber *retval);

/**
   Modifies the OITF state as defined in DAE 7.3.3.3.
   \param type As defined in DAE 7.3.3.1
   \param retval As defined in 7.3.3.3
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemSetPowerState(OOIFNumber type, OOIFBoolean *retval);

/**
   Sets Digest Credentials as defined in DAE 7.3.3.3.
   \param protocol The protocol to apply the credentials
   \param domain The domain to which the credentials apply
   \param username The username to be used in the digest authentication
   \param password The password to be used in the digest authentication
   \param retval Retval as defined in 7.3.3.3
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemSetDigestCredentials(const char *protocol, const char *domain, const char *username, const char *password, OOIFBoolean *retval);

/**
   Clears Digest Credentials as defined in DAE 7.3.3.3.
   \param protocol The protocol to apply the credentials
   \param domain The domain to which the credentials apply
   \param retval Retval as defined in 7.3.3.3
   \return OOIF return code
*/
OOIFReturnCode native_LocalSystemClearDigestCredentials(const char *protocol, const char *domain, OOIFBoolean *retval);

#ifdef HMX_WEBUI
/**
  To notify WebApplication Ready
*/
OOIFReturnCode native_LocalSystemSetApplicationReady(OOIFBoolean *retval);
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
OOIFReturnCode native_NetworkInterfaceReleaseHandle(NetworkInterfaceHandle id);

/** @name NetworkInterface
	The NetworkInterface class as defined in DAE 7.3.4
	@{*/

/**
   Gets the OIPF property ipAddress as defined in DAE 7.3.4.1
   \param identifier Unique ID of the given network interface
   \param value Property value as defined in 7.3.4.1
   \return OOIF return code
*/
OOIFReturnCode native_NetworkInterfaceGetIPAddress(NetworkInterfaceHandle identifier, const char **value);

/**
   Gets the OIPF property macAddress as defined in DAE 7.3.4.1
   \param identifier Unique ID of the given network interface
   \param value Property value as defined in 7.3.4.1
   \return OOIF return code
*/
OOIFReturnCode native_NetworkInterfaceGetMacAddress(NetworkInterfaceHandle identifier, const char **value);

/**
   Gets the OIPF property connected as defined in DAE 7.3.4.1
   \param identifier Unique ID of the given network interface
   \param value Property value as defined in 7.3.4.1
   \return OOIF return code
*/
OOIFReturnCode native_NetworkInterfaceGetConnected(NetworkInterfaceHandle identifier, OOIFBoolean *value);

/**
   Gets the OIPF property enabled as defined in DAE 7.3.4.1
   \param identifier Unique ID of the given network interface
   \param value Property value as defined in 7.3.4.1
   \return OOIF return code
*/
OOIFReturnCode native_NetworkInterfaceGetEnabled(NetworkInterfaceHandle identifier, OOIFBoolean *value);

/**
   Sets the OIPF property enabled as defined in DAE 7.3.4.1
   \param identifier Unique ID of the given network interface
   \param enabled As defined in 7.3.4.1
   \return OOIF return code
*/
OOIFReturnCode native_NetworkInterfaceSetEnabled(NetworkInterfaceHandle identifier, OOIFBoolean enabled);

/*@}*/
/*------------------------------------------
  AVOutput
  ------------------------------------------*/

/**
   Called when the AVOutput instance is destroyed in the OOIF plugin.
   \param id AVOutput instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_AVOutputReleaseHandle(AVOutputHandle id);

/** @name AVOutput
	The AVOutput class as defined in DAE 7.3.5
	@{*/

/** DAE 7.3.5.1 */

/**
   Gets the OIPF property name as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param retval Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetName(AVOutputHandle identifier, const char **retval);

/**
   Gets the OIPF property type as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param retval Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetType(AVOutputHandle identifier, const char **retval);

/**
   Gets the OIPF property enabled as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param retval Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetEnabled(AVOutputHandle identifier, OOIFBoolean *retval);

/**
   Gets the OIPF property subtitleEnabled as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param retval Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetSubtitleEnabled(AVOutputHandle identifier, OOIFBoolean *retval);

/**
   Gets the OIPF property videoMode as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param retval Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetVideoMode(AVOutputHandle identifier, const char **retval);

/**
   Gets the OIPF property digitalAudioMode as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param retval Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetDigitalAudioMode(AVOutputHandle identifier, const char **retval);

/**
   Gets the OIPF property audioRange as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param retval Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetAudioRange(AVOutputHandle identifier, const char **retval);

/**
   Gets the OIPF property hdVideoFormat as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param retval Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetHDVideoFormat(AVOutputHandle identifier, const char **retval);

/**
   Gets the OIPF property tvAspectRatio as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param retval Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetTVAspectRatio(AVOutputHandle identifier, const char **retval);

/**
   Gets the OIPF property supportedVideoModes as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param count Size of the array returned in strings parameter
   \param strings Attribute values array as defined in 7.3.5.1. The OOIF plugin will take
   over ownership of the array and will later call free() to de-allocate the memory.
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetSupportedVideoModes(AVOutputHandle identifier, int *count, char ***strings);

/**
   Gets the OIPF property supportedDigitalAudioModes as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param count Size of the array returned in strings parameter
   \param strings Attribute values array as defined in 7.3.5.1. The OOIF plugin will take
   over ownership of the array and will later call free() to de-allocate the memory.
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetSupportedDigitalAudioModes(AVOutputHandle identifier, int *count, char ***strings);

/**
   Gets the OIPF property supportedAudioRanges as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param count Size of the array returned in strings parameter
   \param strings Attribute values array as defined in 7.3.5.1. The OOIF plugin will take
   over ownership of the array and will later call free() to de-allocate the memory.
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetSupportedAudioRanges(AVOutputHandle identifier, int *count, char ***strings);

/**
   Gets the OIPF property supportedHdVideoFormats as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param count Size of the array returned in strings parameter
   \param strings Attribute values array as defined in 7.3.5.1. The OOIF plugin will take over
   ownership of the array and will later call free() to de-allocate the memory.
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetSupportedHdVideoFormats(AVOutputHandle identifier, int *count, char ***strings);

/**
   Gets the OIPF property supportedAspectRatios as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param count Size of the array returned in strings parameter
   \param strings Attribute values array as defined in 7.3.5.1. The OOIF plugin will
   take over ownership of the array and will later call free() to de-allocate the memory.
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputGetSupportedAspectRatios(AVOutputHandle identifier, int *count, char ***strings);

/**
   Sets the OIPF property enabled as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param enabled Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputSetEnabled(AVOutputHandle identifier, OOIFBoolean enabled);

/**
   Sets the OIPF property subtitleEnabled as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param enabled Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputSetSubtitleEnabled(AVOutputHandle identifier, OOIFBoolean enabled);

/**
   Sets the OIPF property videoMode as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param value Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputSetVideoMode(AVOutputHandle identifier, const char *value);

/**
   Sets the OIPF property videoMode as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param value Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputSetDigitalAudioMode(AVOutputHandle identifier, const char *value);

/**
   Sets the OIPF property audioRange as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param value Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputSetAudioRange(AVOutputHandle identifier, const char *value);

/**
   Sets the OIPF property hdVideoFormat as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param value Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputSetHDVideoFormat(AVOutputHandle identifier, const char *value);

/**
   Sets the OIPF property tvAspectRatio as defined in DAE 7.3.5.1
   \param identifier AV output identifier
   \param value Attribute value as defined in 7.3.5.1
   \return OOIF return code
*/
OOIFReturnCode native_AVOutputSetTVAspectRatio(AVOutputHandle identifier, const char *value);

/*@}*/
/*@}*/
#endif /* OIPF */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_CONFIGURATION */
