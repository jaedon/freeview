// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hlib.h"
#include "NativeConfiguration.h"
#include "NativeHConfiguration.h"
#include "macros.h"
#include "opllocalsystem.h"

#if JLABS
#include "opljlabslocalsystem.h"
#endif

//
// CONFIGURATION INTERFACE
//

OOIFReturnCode native_ConfigGetPreferredAudioLanguage(const char **retval)
{
	*retval = "DEBUG Preferred Audio Language";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigGetPreferredSubtitleLanguage(const char **retval)
{
	*retval = "DEBUG Preferred Subtitle Language";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigGetCountryId(const char **retval)
{
#if defined JLABS
	*retval = "jpn";
#else
	*retval = "DEBUG Country ID";
#endif

	return OOIF_RETURN_OK;
}

#ifdef HBBTV_1_2_1
OOIFReturnCode native_ConfigGetPreferredUILanguage(const char **retval)
{
	*retval = "Dummy Preferred UI Language";
	return OOIF_RETURN_OK;
}
#endif // HBBTV_1_2_1

#ifdef HBBTV
OOIFReturnCode native_ConfigGetTimeShiftSynchronized(OOIFBoolean *retval)
{
	*retval = false;
	return OOIF_RETURN_OK;
}
#endif // HBBTV

#ifdef OIPF
OOIFReturnCode native_ConfigGetPreferredTimeShiftMode(OOIFNumber *retval)
{
	OOIFBoolean value;

	OOIF_LOG_INFO("[%s:%d] \n",__FUNCTION__,__LINE__);

	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	value = (OOIFBoolean)OplLocalSystem::getTimeshiftEnabled();
	if(value == true)
		*retval = 1; // use "local resource"
	else
		*retval = 0; // turned off

	return OOIF_RETURN_OK;
}
#endif // OIPF

OOIFReturnCode native_ConfigGetRegionId(OOIFNumber *retval)
{
#if defined JLABS
	*retval = 0; /* 일본의 time zone은 1개 뿐이다. */
#else
	*retval = 123;
#endif
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigGetPvrPolicy(OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigGetPvrSaveEpisodes(OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigGetPvrSaveDays(OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigGetPvrStartPadding(OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigGetPvrEndPadding(OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigGetText(const char *key, const char **retval)
{
	OOIF_LOG_DEBUG("Called Configuration.getText with %s", key);
	*retval = "Dummy Native Text";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetText(const char *key, const char *value)
{
	OOIF_LOG_DEBUG("Called Configuration.setText with %s, %s", key, value);
	return OOIF_RETURN_OK;
}

//optional part of interface for HbbTV
#if defined(HBBTV) && !defined(HBBTV_1_2_1)
OOIFReturnCode native_ConfigGetIsPINEntryLocked(OOIFBoolean *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetParentalControlPIN(const char *oldPcPin, const char *newPcPin, OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetParentalControlPINEnable(const char *pcPIN, OOIFBoolean enable, OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigGetParentalControlPINEnable(OOIFBoolean *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigUnlockWithParentalControlPIN(const char *pcPIN, void *target, OOIFNumber duration, OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigVerifyParentalControlPIN(const char *pcPIN, OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetBlockUnrated(const char *pcPIN, OOIFBoolean block, OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}



OOIFReturnCode native_ConfigGetBlockUnrated(OOIFBoolean *retval)
{
	*retval = 1;
	return OOIF_RETURN_OK;
}
#endif //HBBTV && !HBBTV_1_2_1

#ifdef OIPF
OOIFReturnCode native_ConfigSetPreferredTimeShiftMode(OOIFNumber value)
{
	OOIF_LOG_INFO("Setting Configuration.preferredTimeShiftMode to : %f", value);
	if(value == 0)
		OplLocalSystem::setTimeshiftEnabled(false);
	else
		OplLocalSystem::setTimeshiftEnabled(true);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetPreferedAudioLanguage(const char* value)
{
	OOIF_LOG_DEBUG("Setting Configuration.preferredAudioLanguage to : %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetPreferredSubtitleLanguage(const char *value)
{
	OOIF_LOG_DEBUG("Setting Configuration.preferredSubtitleLanguage to : %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetCountryId(const char *value)
{
	OOIF_LOG_DEBUG("Setting Configuration.countryId to : %s", value);
	return OOIF_RETURN_OK;
}
#endif // OIPF

//optional part for HbbTV / HbbTV 1.2.1 and mandatory for OIPF
OOIFReturnCode native_ConfigSetRegionId(OOIFNumber value)
{
	OOIF_LOG_DEBUG("Setting Configuration.regionId to : %f", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetPvrPolicy(OOIFNumber value)
{
	OOIF_LOG_DEBUG("Setting Configuration.pvrPolicy to : %f", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetPvrSaveEpisodes(OOIFNumber value)
{
	OOIF_LOG_DEBUG("Setting Configuration.pvrSaveEpisodes to : %f", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetPvrSaveDays(OOIFNumber value)
{
	OOIF_LOG_DEBUG("Setting Configuration.pvrSaveDays to : %f", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetPvrStartPadding(OOIFNumber value)
{
	OOIF_LOG_DEBUG("Setting Configuration.pvrStartPadding to : %f", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ConfigSetPvrEndPadding(OOIFNumber value)
{
	OOIF_LOG_DEBUG("Setting Configuration.pvrEndPadding to : %f", value);
	return OOIF_RETURN_OK;
}

//
// LOCAL SYSYTEM INTERFACE
//

#ifdef OIPF

#define	MAX_BUF_LEN 32
localSystemListener *global_localSystemListener = 0;
static int	s_nCurrentPowerState = 1;					//	current ON mode
static int	s_nPreviousPowerState = 2;					//	previous PASSIVE_STANDBY

void sendPowerStateChangedEvent(OOIFNumber state)
{
	OOIF_LOG_DEBUG("sendPowerStateChangedEvent");
	if (global_localSystemListener)
	{
		LocalSystemEventInfo info;
		info.power_state_change_data.powerState = state;
		global_localSystemListener(LOCAL_SYSTEM_EVENT_POWER_STATE_CHANGE, &info);
	}
}

static void native_localsystem_sendPowerStateChangedEvent(int curState)
{
	if (s_nCurrentPowerState != curState)
	{
		s_nPreviousPowerState = s_nCurrentPowerState;
		s_nCurrentPowerState = curState;
	}
	sendPowerStateChangedEvent((OOIFNumber)curState);
}

#ifdef JLABS
void sendPowerStateChangeErrorEvent(OOIFNumber reason)
{
	OOIF_LOG_DEBUG("sendPowerStateChangeEvent");
	if (global_localSystemListener)
	{
		LocalSystemEventInfo info;
		info.power_state_change_error_data.reason = reason;
		global_localSystemListener(LOCAL_SYSTEM_EVENT_POWER_STATE_CHANGE_ERROR, &info);
	}
}

static void native_localsystem_sendPowerStateChangedErrorEvent(int reason)
{
	sendPowerStateChangeErrorEvent((OOIFNumber)reason);
}
#endif

OOIFReturnCode native_LocalSystemSetListener(localSystemListener listener)
{
	OOIF_LOG_DEBUG("LocalSystemSetListener");
	global_localSystemListener = listener;

	OplLocalSystem::setPowerStateChangeListener(native_localsystem_sendPowerStateChangedEvent);
#ifdef JLABS
	OplLocalSystem::setPowerStateChangeErrorListener(native_localsystem_sendPowerStateChangedErrorEvent);
#endif
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_LocalSystemOnListenerAccessed(long int window, const char *host, const char *eventName)
{
	//Dummy Implementation TODO
	OOIF_LOG_DEBUG("LocalSystemOnListenerAccessed");
	return OOIF_RETURN_OK;
}

// ro String deviceID
OOIFReturnCode native_LocalSystemGetDeviceID(const char **retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetDeviceID");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szDeviceId[MAX_BUF_LEN];

	HxSTD_MemSet(szDeviceId,0,MAX_BUF_LEN);

	OplLocalSystem::getDeviceID(szDeviceId);

	*retval = szDeviceId;

	return OOIF_RETURN_OK;
}

//ro Boolean systemReady
OOIFReturnCode native_LocalSystemGetSystemReady(OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetSystemReady");
	*retval = 1;
	return OOIF_RETURN_OK;
}

//ro String vendorName
OOIFReturnCode native_LocalSystemGetVendorName(const char **retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetVendorName");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szVendorName[MAX_BUF_LEN];

	HxSTD_MemSet(szVendorName,0,MAX_BUF_LEN);

	OplLocalSystem::getVendorName(szVendorName);

	*retval = szVendorName;

	return OOIF_RETURN_OK;
}

//ro String modelName
OOIFReturnCode native_LocalSystemGetModelName(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szModelName[MAX_BUF_LEN];

	HxSTD_MemSet(szModelName,0,MAX_BUF_LEN);

	OplLocalSystem::getModelName(szModelName);

	*retval = szModelName;

	return OOIF_RETURN_OK;
}

// ro String softwareVersion
OOIFReturnCode native_LocalSystemGetSoftwareVersion(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szSWVersion[MAX_BUF_LEN];

	HxSTD_MemSet(szSWVersion,0,MAX_BUF_LEN);

	OplLocalSystem::getSoftwareVersion(szSWVersion);

	*retval = szSWVersion;

	return OOIF_RETURN_OK;
}

// ro String hardwareVersion
OOIFReturnCode native_LocalSystemGetHardwareVersion(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szHWVersion[MAX_BUF_LEN];

	HxSTD_MemSet(szHWVersion,0,MAX_BUF_LEN);

	OplLocalSystem::getSystemID(szHWVersion);

	*retval = szHWVersion;

	return OOIF_RETURN_OK;
}

// ro String serialNumber
OOIFReturnCode native_LocalSystemGetSerialNumber(const char **retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetSerialNumber");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szSerialNum[MAX_BUF_LEN];

	HxSTD_MemSet(szSerialNum,0,MAX_BUF_LEN);

	OplLocalSystem::getSerialNumber(szSerialNum);

	*retval = szSerialNum;

	return OOIF_RETURN_OK;
}

// ro Integer releaseVersion
OOIFReturnCode native_LocalSystemGetReleaseVersion(OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetReleaseVersion");
	*retval = 2; /* now oipf version is 2 */
	return OOIF_RETURN_OK;
}

// ro Integer majorVersion
OOIFReturnCode native_LocalSystemGetMajorVersion(OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetMajorVersion");
	*retval = 2;
	return OOIF_RETURN_OK;
}

// ro Integer minorVersion
OOIFReturnCode native_LocalSystemGetMinorVersion(OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetMinorVersion");
	*retval = 0;
	return OOIF_RETURN_OK;
}

//ro String oipfProfile
OOIFReturnCode native_LocalSystemGetOipfProfile(const char **retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetOipfProfile");
	*retval = "OIPF Profile";
	return OOIF_RETURN_OK;
}

// ro Boolean pvrEnabled
OOIFReturnCode native_LocalSystemGetPvrEnabled(OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetPvrEnabled");
	*retval = 1;
	return OOIF_RETURN_OK;
}

//ro Boolean ciplusEnabled
OOIFReturnCode native_LocalSystemGetCiplusEnabled(OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetCiplusEnabled");
	*retval = 1;
	return OOIF_RETURN_OK;
}

// Boolean standbyState
OOIFReturnCode native_LocalSystemGetStandbyState(OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetStandbyState");
	*retval = 1;
	return OOIF_RETURN_OK;
}

//ro Integer powerState
OOIFReturnCode native_LocalSystemGetPowerState(OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetPowerState");
	*retval = (OOIFNumber)s_nCurrentPowerState;

	return OOIF_RETURN_OK;
}

//ro Integer previousPowerState
OOIFReturnCode native_LocalSystemGetPreviousPowerState(OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetPreviousPowerState");
	*retval = (OOIFNumber)s_nPreviousPowerState;

	return OOIF_RETURN_OK;
}

//ro Integer timeCurrentPowerState
OOIFReturnCode native_LocalSystemGetTimeCurrentPowerState(OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetTimeCurrentPowerState");
	*retval = 1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_LocalSystemSetStandbyState(OOIFBoolean v)
{
	OOIF_LOG_DEBUG("LocalSystem set standbyState: %d", v);
	return OOIF_RETURN_OK;
}

// Integer volume
OOIFReturnCode native_LocalSystemGetVolume(OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetVolume");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	unsigned int volume = 0;

	OplLocalSystem::getVolume(&volume);

	*retval = volume;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_LocalSystemSetVolume(OOIFNumber v)
{
	OOIF_LOG_DEBUG("LocalSystem set volume: %f", v);
	OplLocalSystem::setVolume((unsigned int)v);
	return OOIF_RETURN_OK;
}

// Boolean mute
OOIFReturnCode native_LocalSystemGetMute(OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetMute");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (OOIFBoolean)OplLocalSystem::getMute();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_LocalSystemSetMute(OOIFBoolean v)
{
	OOIF_LOG_DEBUG("LocalSystem set mute %d", v);
	OplLocalSystem::setMute((BPL_BOOL)v);

	return OOIF_RETURN_OK;
}

//ro NetworkInterfaceCollection networkInterfaces
OOIFReturnCode native_LocalSystemGetNetworkInterfaces(int *count, NetworkInterfaceHandle **handles)
{
	OOIF_LOG_DEBUG("LocalSystemGetNetworkInterfaces");
	NetworkInterfaceHandle *myHandles = (NetworkInterfaceHandle*)HLIB_STD_MemAlloc(sizeof(NetworkInterfaceHandle)*2);
	myHandles[0] = (void*)922;
	myHandles[1] = (void*)933;
	*handles = myHandles;
	*count = 2;
	return OOIF_RETURN_OK;
}

//ro AVOutputCollection outputs
OOIFReturnCode native_LocalSystemGetOutputs(int *count, AVOutputHandle **handles)
{
	OOIF_LOG_DEBUG("LocalSystemGetOutputs");
	unsigned int nNumOfDev,idx,startValue;
	AVOutputHandle *myHandles;

	OplHmxOutput::getNumOfDev(&nNumOfDev);

	OOIF_LOG_INFO("[%s:%d] nNumOfDev:%d\n",__PRETTY_FUNCTION__,__LINE__,nNumOfDev);

	myHandles = (AVOutputHandle*)HLIB_STD_MemAlloc(sizeof(AVOutputHandle)*nNumOfDev);
	startValue = 0;
	for (idx = 0; idx < nNumOfDev; idx++)
	{
		myHandles[idx] = (void*)(startValue+idx);
		OOIF_LOG_INFO("[%s:%d] myHandles[%d]:0x%X\n",__PRETTY_FUNCTION__,__LINE__,idx,(unsigned int)myHandles[idx]);
	}

	*handles = myHandles;
	*count = nNumOfDev;

	return OOIF_RETURN_OK;
}

//ro TunerCollection tuners
OOIFReturnCode native_LocalSystemGetTuners(long int window, const char* host, int *count, TunerHandle **handles)
{
	OOIF_LOG_DEBUG("LocalSystemGetTuners");
	unsigned int	nNumOfDev, idx;
	TunerHandle 	*myHandles;

	OplTuner::getNumOfDev(&nNumOfDev);

	OOIF_LOG_INFO("[%s:%d] nNumOfDev:%d\n", __PRETTY_FUNCTION__, __LINE__, nNumOfDev);

	myHandles = (TunerHandle*)HLIB_STD_MemAlloc(sizeof(TunerHandle) * nNumOfDev);
	for (idx = 0; idx < nNumOfDev; idx++)
	{
		myHandles[idx] = (void*)(idx);
		OOIF_LOG_INFO("[%s:%d] myHandles[%d]:0x%X\n", __PRETTY_FUNCTION__, __LINE__, idx, myHandles[idx]);
	}

	*handles = myHandles;
	*count = nNumOfDev;

	return OOIF_RETURN_OK;
}

// ro Integer tvStandard
OOIFReturnCode native_LocalSystemGetTvStandard(OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetTvStandard");
	*retval = 0;
	return OOIF_RETURN_OK;
}

// ro Integer pvrSupport
OOIFReturnCode native_LocalSystemGetPvrSupport(OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("LocalSystemGetPvrSupport");
	*retval = 0;
	return OOIF_RETURN_OK;
}

// methods
// ---------------

// Boolean setScreenSize(Integer width, Integer height)
OOIFReturnCode native_LocalSystemSetScreenSize(OOIFNumber width, OOIFNumber height, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("LocalSystem setScreenSize: width=%f, height=%f", width, height);
	*retval = 1;
	return OOIF_RETURN_OK;
}

// Integer setPvrSupport(Integer state)
OOIFReturnCode native_LocalSystemSetPvrSupport(OOIFNumber state, OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("LocalSystem setPvrSupport: type=%f", state);
	*retval = state;
	return OOIF_RETURN_OK;
}

// Integer setPowerState(Integer type)
OOIFReturnCode native_LocalSystemSetPowerState(OOIFNumber type, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("LocalSystem setPowerState: type=%f", type);
	int		standbyMode;

	standbyMode = (int)type;
	if ( 4 > standbyMode ) {
		OplLocalSystem::controlPower(standbyMode);
		*retval = 1;

		sendPowerStateChangedEvent(type);
	} else {
		OOIF_LOG_ERROR("Invalid Standby Mode --> [%d]\n", standbyMode);

		return OOIF_RETURN_NOT_SUPPORTED;
	}

	return OOIF_RETURN_OK;
}

// Boolean setDigestCredentials( String protocol, String domain, String username, String password )
OOIFReturnCode native_LocalSystemSetDigestCredentials(const char *protocol, const char *domain, const char *username, const char *password, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("LocalSystem setDigestCredentials: protocol=%s, domain=%s, username=%s, password=%s", protocol, domain, username, password);
	*retval = 1;
	return OOIF_RETURN_OK;
}

// Boolean clearDigestCredentials( String protocol, String domain )
OOIFReturnCode native_LocalSystemClearDigestCredentials(const char *protocol, const char *domain, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("LocalSystem clearDigestCredentials: protocol=%s, domain=%s", protocol, domain);
	*retval = 1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_NetworkInterfaceGetIPAddress(NetworkInterfaceHandle identifier, const char **retval)
{
	if ((void*)922 == identifier)
		*retval = "301.300.300.300";
	else // if ((void*)933 == identifier)
		*retval = "DEAD:DEAD:DEAD:DEAD:DEAD:DEAD:DEAD:DEAD";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_NetworkInterfaceGetMacAddress(NetworkInterfaceHandle identifier, const char **retval)
{
	*retval = "AA:BB:CC:DD:EE:FF";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_NetworkInterfaceGetConnected(NetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	if ((void*)922 == identifier)
		*retval = 1;
	else // if ((void*)933 == identifier)
		*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_NetworkInterfaceGetEnabled(NetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	if ((void*)922 == identifier)
		*retval = 1;
	else // if ((void*)933 == identifier)
		*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_NetworkInterfaceSetEnabled(NetworkInterfaceHandle identifier, OOIFBoolean enabled)
{
	OOIF_LOG_DEBUG("Setting network enabled to %d", enabled);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_NetworkInterfaceReleaseHandle(NetworkInterfaceHandle id)
{
	return OOIF_RETURN_OK;
}

// -------------------------------------
// AVOutput getters
OOIFReturnCode native_AVOutputGetName(AVOutputHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szOutputName[(MAX_BUF_LEN+1)];

	HxSTD_MemSet(szOutputName,0,(MAX_BUF_LEN+1));

	snprintf(szOutputName,MAX_BUF_LEN,"Output %d",(unsigned int)identifier);

	*retval = szOutputName;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetType(AVOutputHandle identifier, const char **retval)
{
	*retval = "both";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetEnabled(AVOutputHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplHmxOutput::IsSupportHdmi((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetSubtitleEnabled(AVOutputHandle identifier, OOIFBoolean *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetVideoMode(AVOutputHandle identifier, const char **retval)
{
	*retval = "normal";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetDigitalAudioMode(AVOutputHandle identifier, const char **retval)
{
	*retval = "ac3";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetAudioRange(AVOutputHandle identifier, const char **retval)
{
	*retval = "wide";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetHDVideoFormat(AVOutputHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szResolution[(MAX_BUF_LEN+1)];

	HxSTD_MemSet(szResolution,0,(MAX_BUF_LEN+1));

	if ( !OplHmxOutput::getCurResolution((unsigned int)identifier, szResolution, MAX_BUF_LEN) )
	{
		OOIF_LOG_ERROR("[%s:%d] getCurResolution ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] pszResolution:%s\n",__PRETTY_FUNCTION__,__LINE__,szResolution);

	*retval = szResolution;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetTVAspectRatio(AVOutputHandle identifier, const char **retval)
{
	*retval = "16:9";
	return OOIF_RETURN_OK;
}

// AVOutput setters
OOIFReturnCode native_AVOutputSetEnabled(AVOutputHandle identifier, OOIFBoolean enabled)
{
	OOIF_LOG_DEBUG("Setting AVOutput enabled %d", enabled);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputSetSubtitleEnabled(AVOutputHandle identifier, OOIFBoolean enabled)
{
	OOIF_LOG_DEBUG("Setting AVOoutput subtitleEnabled %d", enabled);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputSetVideoMode(AVOutputHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting AVOutput videoMode %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputSetDigitalAudioMode(AVOutputHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting AVOutput digitalAudioMode %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputSetAudioRange(AVOutputHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting AVOutput audioRange %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputSetHDVideoFormat(AVOutputHandle identifier, const char *value)
{
	if ( !OplHmxOutput::setCurResolution((unsigned int)identifier, value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setCurResolution ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputSetTVAspectRatio(AVOutputHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting AVOutput tvAspectRatio %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetSupportedVideoModes(AVOutputHandle identifier, int *count, char ***strings)
{
	char **myStrings = (char**)HLIB_STD_MemAlloc(sizeof(char*)*2);
	myStrings[0] = HLIB_STD_StrDup("normal");
	myStrings[1] = HLIB_STD_StrDup("stretch");
	*strings = myStrings;
	*count = 2;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetSupportedDigitalAudioModes(AVOutputHandle identifier, int *count, char ***strings)
{
	char **myStrings = (char**)HLIB_STD_MemAlloc(sizeof(char*)*2);
	myStrings[0] = HLIB_STD_StrDup("ac3");
	myStrings[1] = HLIB_STD_StrDup("uncompressed");
	*strings = myStrings;
	*count = 2;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetSupportedAudioRanges(AVOutputHandle identifier, int *count, char ***strings)
{
	char **myStrings = (char**)HLIB_STD_MemAlloc(sizeof(char*)*2);
	myStrings[0] = HLIB_STD_StrDup("wide");
	myStrings[1] = HLIB_STD_StrDup("narrow");
	*strings = myStrings;
	*count = 2;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetSupportedHdVideoFormats(AVOutputHandle identifier, int *count, char ***strings)
{
	if (NULL == strings)
		return OOIF_RETURN_VALUE_UNDEFINED;

	// static buffer used in getSupportedResolutions() for strings.
	if ( !OplHmxOutput::getSupportedResolutions((unsigned int)identifier, count, strings) )
	{
		OOIF_LOG_ERROR("[%s:%d] getSupportedResolutions ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputGetSupportedAspectRatios(AVOutputHandle identifier, int *count, char ***strings)
{
	char **myStrings = (char**)HLIB_STD_MemAlloc(sizeof(char*)*2);
	myStrings[0] = HLIB_STD_StrDup("16:9");
	myStrings[1] = HLIB_STD_StrDup("14:9");
	*strings = myStrings;
	*count = 2;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVOutputReleaseHandle(AVOutputHandle id)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_LocalSystemSetApplicationReady(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplLocalSystem::setApplicationReady();

	*retval = 1;
	return OOIF_RETURN_OK;
}

#endif // OIPF
