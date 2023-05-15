/**************************************************************************************/
/**
 * @file NativeConfiguration.cpp
 *
 * Configuration module
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com), Inseon Son(isson@humaxdigital.com)
 * @date    2011/04/07
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>
#include <hapi.h>

#include "NativeHConfiguration.h"
#include "macros.h"

#include "opllocalsystem.h"
#include "oplnetwork.h"
#include "oplsignalinfo.h"
#include "opldms.h"
#include "oplscheduler.h"


#define	MAX_BUF_LEN 32
#define	MAX_BUF_LEN_FOR_UPDATEDLIST 1024
#define MAX_URL_LEN 2048
#define	MAX_LONG_BUF_LEN 512

#ifdef OIPF
static hlocalSystemListener global_hlocalSystemListener = NULL;

#if defined(HMX_WEBUI)
static void native_hlocalsystem_sendCamStateChangedEvent(const unsigned char ucSlot, const unsigned int ulState)
{
	if (global_hlocalSystemListener)
	{
		static HLocalSystemEventInfo info;
		info.cam_state_change_data.slotNumber = (OOIFNumber)ucSlot;
		info.cam_state_change_data.camState = (OOIFNumber)ulState;
		global_hlocalSystemListener(HLOCAL_SYSTEM_EVENT_CAM_STATE_CHANGE, &info);
	}
}
static void native_hlocalsystem_sendSmartcardStateChangedEvent(const unsigned char ucSlot, const unsigned int ulState)
{
	if (global_hlocalSystemListener)
	{
		static HLocalSystemEventInfo info;
		info.sc_state_change_data.slotNumber = (OOIFNumber)ucSlot;
		info.sc_state_change_data.scState = (OOIFNumber)ulState;
		global_hlocalSystemListener(HLOCAL_SYSTEM_EVENT_SC_STATE_CHANGED, &info);
	}
}
static void native_hlocalsystem_sendThermalProtectionOccuredEvent()
{
	if (global_hlocalSystemListener)
	{
		static HLocalSystemEventInfo info;
		global_hlocalSystemListener(HLOCAL_SYSTEM_EVENT_THERMAL_PROTECTION_OCCURED, &info);
	}
}
static void native_hlocalsystem_sendExtDevInfoListener(int nDevType, int nNotiType, BPL_BOOL bConnected, unsigned int ulDeviceId, int nPortNum)
{
	if (global_hlocalSystemListener)
	{
		if (nDevType == HOTPLUG_DEVTYPE_MIC)
		{
			static HLocalSystemEventInfo info;
			info.ext_dev_info_data.devId = (OOIFNumber)ulDeviceId;
			info.ext_dev_info_data.devType = (OOIFNumber)nDevType;
			info.ext_dev_info_data.devState = (OOIFNumber)bConnected;
			if (bConnected == TRUE)
			{
				global_hlocalSystemListener(HLOCAL_SYSTEM_EVENT_MIC_ATTACHED, &info);
			}
			else
			{
				global_hlocalSystemListener(HLOCAL_SYSTEM_EVENT_MIC_DETACHED, &info);
			}
		} else if (nDevType == HOTPLUG_DEVTYPE_USB_WIRELESS) {
			static HLocalSystemEventInfo infoWireless;
			infoWireless.ext_dev_info_data.devId = (OOIFNumber)ulDeviceId;
			infoWireless.ext_dev_info_data.devType = (OOIFNumber)nDevType;
			infoWireless.ext_dev_info_data.devState = (OOIFNumber)bConnected;
			global_hlocalSystemListener(HLOCAL_SYSTEM_EVENT_USB_WIRELESS_STATE_CHANGE, &infoWireless);
		}
	}
}
#endif

static void sendPowerStateChangedEvent(OOIFNumber state)
{
	//Dummy Implementation TODO
	OOIF_LOG_DEBUG("sendPowerStateChangedEvent");

	if (global_hlocalSystemListener) {
		HLocalSystemEventInfo info;
		info.power_state_change_data.powerState = state;
		global_hlocalSystemListener(HLOCAL_SYSTEM_EVENT_POWER_STATE_CHANGE, &info);
	}
}

static void userActionIdleEvent(void)
{
	if (global_hlocalSystemListener) {
		HLocalSystemEventInfo info;
		global_hlocalSystemListener(HLOCAL_SYSTEM_EVENT_USER_ACTION_IDLE, &info);
	}
}

static void keyboardResultEvent(char* resultStr)
{
	if (global_hlocalSystemListener) {
		HLocalSystemEventInfo info;
		info.keyboard_result_data.resultStr = resultStr;
		global_hlocalSystemListener(HLOCAL_SYSTEM_EVENT_KEYBOARD_RESULT, &info);
	}
}


static void native_hlocalsystem_sendPowerStateChangedEvent(int curState)
{
#if 0
	fflush(NULL);
	OOIF_LOG_PRINT("\n\n\n\n\n[%s:%d] curState:%d\n\n\n\n\n",__FUNCTION__,__LINE__,curState);
	fflush(NULL);
#endif
	sendPowerStateChangedEvent((OOIFNumber)curState);
}

OOIFReturnCode native_HLocalSystemSetListener(hlocalSystemListener listener)
{
#if 0
	fflush(NULL);
	OOIF_LOG_PRINT("\n\n\n\n\n[%s:%d] listener:[0x%X] native_hlocalsystem_sendPowerStateChangedEvent:[0x%X]n\n\n\n\n",__FUNCTION__,__LINE__,listener, native_hlocalsystem_sendPowerStateChangedEvent);
	fflush(NULL);
#endif
	global_hlocalSystemListener = listener;

	OplLocalSystem::setPowerStateChangeListener(native_hlocalsystem_sendPowerStateChangedEvent);
#if defined(HMX_WEBUI)
	OplLocalSystem::setCamStateChangeListener(native_hlocalsystem_sendCamStateChangedEvent);
	OplLocalSystem::setSmartcardStateChangedListener(native_hlocalsystem_sendSmartcardStateChangedEvent);
	OplLocalSystem::setThermalProtectionOccuredListener(native_hlocalsystem_sendThermalProtectionOccuredEvent);
	OplLocalSystem::setExtDevInfoListener(native_hlocalsystem_sendExtDevInfoListener);
#endif
	OplLocalSystem::setUserActionIdleListener(userActionIdleEvent);
	OplLocalSystem::setKeyboardResultListener(keyboardResultEvent);


	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemUnsetListener(void)
{
	global_hlocalSystemListener = NULL;

#if defined(HMX_WEBUI)
	OplLocalSystem::unsetCamStateChangeListener();
#endif
#if 0
	fflush(NULL);
	OOIF_LOG_PRINT("\n\n\n\n\n[%s:%d] global_hlocalSystemListener:[0x%X]n\n\n\n\n",__FUNCTION__,__LINE__,*global_hlocalSystemListener);
	fflush(NULL);
#endif
	return OOIF_RETURN_OK;
}

// ro String deviceID
OOIFReturnCode native_HLocalSystemGetDeviceID(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szDeviceId[MAX_BUF_LEN];
	HxSTD_MemSet(szDeviceId,0,MAX_BUF_LEN);

	OplLocalSystem::getDeviceID(szDeviceId);

	*retval = szDeviceId;

	return OOIF_RETURN_OK;
}

// Boolean systemReady
OOIFReturnCode native_HLocalSystemGetSystemReady(OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (OOIFBoolean)OplLocalSystem::getSystemReady();

	return OOIF_RETURN_OK;
}

// ro String vendorName
OOIFReturnCode native_HLocalSystemGetVendorName(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szVendorName[MAX_BUF_LEN];

	HxSTD_MemSet(szVendorName,0,MAX_BUF_LEN);

	OplLocalSystem::getVendorName(szVendorName);

	*retval = szVendorName;

	return OOIF_RETURN_OK;
}

// ro String modelName
OOIFReturnCode native_HLocalSystemGetModelName(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szModelName[MAX_BUF_LEN];

	HxSTD_MemSet(szModelName,0,MAX_BUF_LEN);

	OplLocalSystem::getModelName(szModelName);

	*retval = szModelName;

	return OOIF_RETURN_OK;
}

// ro String updated List info
OOIFReturnCode native_HLocalSystemGetUpdatedList(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szUpdatedList[MAX_BUF_LEN_FOR_UPDATEDLIST];

	HxSTD_MemSet(szUpdatedList,0,MAX_BUF_LEN_FOR_UPDATEDLIST);

	OplLocalSystem::getUpdatedList(szUpdatedList);

	*retval = szUpdatedList;

	return OOIF_RETURN_OK;
}

// ro String chipId
OOIFReturnCode native_HLocalSystemGetChipId(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szChipId[MAX_BUF_LEN];

	HxSTD_MemSet(szChipId,0,MAX_BUF_LEN);

	OplLocalSystem::getChipId(szChipId);

	*retval = szChipId;
	//OOIF_LOG_PRINT("\n native_HLocalSystemGetChipId - szChipId : %s\n", szChipId);
	return OOIF_RETURN_OK;
}

// ro String updateDate
OOIFReturnCode native_HLocalSystemGetUpdateDate(const char **retval)
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
OOIFReturnCode native_HLocalSystemGetSoftwareVersion(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szSWVersion[MAX_BUF_LEN];

	HxSTD_MemSet(szSWVersion,0,MAX_BUF_LEN);

	OplLocalSystem::getSoftwareVersion(szSWVersion);

	*retval = szSWVersion;

	return OOIF_RETURN_OK;
}

// ro String packageVersion
OOIFReturnCode native_HLocalSystemGetPackageVersion(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szPackageVersion[MAX_BUF_LEN];

	HxSTD_MemSet(szPackageVersion,0,MAX_BUF_LEN);

	OplLocalSystem::getPackageVersion(szPackageVersion);

	*retval = szPackageVersion;

	return OOIF_RETURN_OK;
}

// ro String hardwareVersion
OOIFReturnCode native_HLocalSystemGetHardwareVersion(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szHWVersion[MAX_BUF_LEN];

	HxSTD_MemSet(szHWVersion,0,MAX_BUF_LEN);

	OplLocalSystem::getHardwareVersion(szHWVersion);

	*retval = szHWVersion;

	return OOIF_RETURN_OK;
}
// ro String serialNumber
OOIFReturnCode native_HLocalSystemGetSerialNumber(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szSerialNum[MAX_BUF_LEN];

	HxSTD_MemSet(szSerialNum,0,MAX_BUF_LEN);

	OplLocalSystem::getSerialNumber(szSerialNum);

	*retval = szSerialNum;

	return OOIF_RETURN_OK;
}

// Boolean pvrEnabled
OOIFReturnCode native_HLocalSystemGetPvrEnabled(OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (OOIFBoolean)OplLocalSystem::getPvrEnabled();

	return OOIF_RETURN_OK;
}

// standbyState in HLocalSystem (getter)
OOIFReturnCode native_HLocalSystemGetStandbyState(OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (OOIFBoolean)OplLocalSystem::getStandby();

	return OOIF_RETURN_OK;
}

// standbyState in HLocalSystem (setter)
OOIFReturnCode native_HLocalSystemSetStandbyState(OOIFBoolean v)
{
	OOIFNumber state;

	OplLocalSystem::setStandby(v);
	if ( TRUE == v) {
		state = 2;
	} else {
		state = 1;
	}
	sendPowerStateChangedEvent(state);

	return OOIF_RETURN_OK;
}

//	Power State
OOIFReturnCode	native_HLocalSystemGetPowerState(OOIFNumber *retval)
{
	int nOipfPowerState;

	OplLocalSystem::getOipfPowerState(&nOipfPowerState);

	*retval = (OOIFNumber)nOipfPowerState;

	return OOIF_RETURN_OK;
}

OOIFReturnCode	native_HLocalSystemGetPreviousPowerState(OOIFNumber *retval)
{
	int nOipfPreviousPowerState;

	OplLocalSystem::getOipfPreviousPowerState(&nOipfPreviousPowerState);

	*retval = (OOIFNumber)nOipfPreviousPowerState;

	return OOIF_RETURN_OK;
}

OOIFReturnCode	native_HLocalSystemGetTimeCurrentPowerState(OOIFNumber *retval)
{
	int nOipfPowerStateTime;

	OplLocalSystem::getOipfTimeCurrentPowerState(&nOipfPowerStateTime);

	*retval = (OOIFNumber)nOipfPowerStateTime;

	return OOIF_RETURN_OK;
}

// ro Integer releaseVersion
OOIFReturnCode native_HLocalSystemGetReleaseVersion(OOIFNumber *retval)
{
	//Dummy Implementation TODO
	OOIF_LOG_DEBUG("LocalSystemGetReleaseVersion");
	*retval = 2;
	return OOIF_RETURN_OK;
}

// ro Integer majorVersion
OOIFReturnCode native_HLocalSystemGetMajorVersion(OOIFNumber *retval)
{
	//Dummy Implementation TODO
	OOIF_LOG_DEBUG("LocalSystemGetMajorVersion");
	*retval = 2;
	return OOIF_RETURN_OK;
}

// ro Integer minorVersion
OOIFReturnCode native_HLocalSystemGetMinorVersion(OOIFNumber *retval)
{
	//Dummy Implementation TODO
	OOIF_LOG_DEBUG("LocalSystemGetMinorVersion");
	*retval = 0;
	return OOIF_RETURN_OK;
}

//ro String oipfProfile
OOIFReturnCode native_HLocalSystemGetOipfProfile(const char **retval)
{
	//Dummy Implementation TODO
	OOIF_LOG_DEBUG("LocalSystemGetOipfProfile");
	*retval = "OIPF Profile";
	return OOIF_RETURN_OK;
}

//ro Boolean ciplusEnabled
OOIFReturnCode native_HLocalSystemGetCiplusEnabled(OOIFBoolean *retval)
{
	//Dummy Implementation TODO
	OOIF_LOG_DEBUG("LocalSystemGetCiplusEnabled");
	*retval = 1;
	return OOIF_RETURN_OK;
}

// Integer volume
OOIFReturnCode native_HLocalSystemGetVolume(OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	unsigned int volume = 0;

	OplLocalSystem::getVolume(&volume);

	*retval = volume;

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_HLocalSystemSetVolume(OOIFNumber v)
{
	OplLocalSystem::setVolume((unsigned int)v);

	return OOIF_RETURN_OK;
}

// Boolean mute
OOIFReturnCode native_HLocalSystemGetMute(OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (OOIFBoolean)OplLocalSystem::getMute();

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_HLocalSystemSetMute(OOIFBoolean v)
{
	OplLocalSystem::setMute((BPL_BOOL)v);

	return OOIF_RETURN_OK;
}

// ro AVOutputCollection outputs
// FIXME: Figure out native interface for AVOutputCollection
// ro HNetworkInterfaceCollection networkInterfaces
// FIXME: Figure out native interface for HNetworkInterfaceCollection
// ro Integer tvStandard
OOIFReturnCode native_HLocalSystemGetTvStandard(OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	eOplTVStandard standard;

	OplLocalSystem::getTVStandard(&standard);

	*retval = (OOIFNumber)standard;

	return OOIF_RETURN_OK;
}

// ro Integer pvrSupport
OOIFReturnCode native_HLocalSystemGetPvrSupport(OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	*retval = 1;

	return OOIF_RETURN_OK;
}

// methods
// ---------------

// Boolean setScreenSize(Integer width, Integer height)
OOIFReturnCode native_HLocalSystemSetScreenSize(OOIFNumber width, OOIFNumber height, OOIFBoolean *retval)
{
#if 0
	browser_error err = browser_config_system_set_screen_size(width, height);
	if ( err != ERR_BROWSER_OK )
	{
		*retval = 0;
		return OOIF_RETURN_ERROR;
	}

	*retval = 1;
#endif

	return OOIF_RETURN_OK;
}

// Integer setPvrSupport(Integer state)
OOIFReturnCode native_HLocalSystemSetPvrSupport(OOIFNumber state, OOIFNumber *retval)
{
#if 0
	browser_error err = browser_config_system_set_pvr_support(state);
	if ( err != ERR_BROWSER_OK )
	{
		return OOIF_RETURN_ERROR;
	}

	*retval = state;
#endif

	return OOIF_RETURN_OK;
}

// Integer setPowerState(Integer type)
OOIFReturnCode native_HLocalSystemSetPowerState(OOIFNumber type, OOIFBoolean *retval)
{
	int		standbyMode;

	standbyMode = (int)type;
#if 0
	fflush(NULL);
	OOIF_LOG_PRINT("\n\n\n\n\n[%s:%d] standbyMode:[%d]n\n\n\n\n",__FUNCTION__,__LINE__,standbyMode);
	fflush(NULL);
#endif
	if ( 4 > standbyMode ) {
		OplLocalSystem::controlPower(standbyMode);
		*retval = 1;

//		sendPowerStateChangedEvent(type);
	} else {
		OOIF_LOG_ERROR("Invalid Standby Mode --> [%d]\n", standbyMode);

		return OOIF_RETURN_NOT_SUPPORTED;
	}

	return OOIF_RETURN_OK;
}

// String getText(String key)
OOIFReturnCode native_HLocalSystemGetField(const char *key, const char **retval)
{
	if ((NULL == retval) || (NULL == key))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	static char szLocalFieldbuf[2048] = {0,};
	*retval = NULL;

	memset(szLocalFieldbuf,0,(MAX_BUF_LEN*2));

	if (!strcmp(key,(const char *)"system_id"))
	{
		OplLocalSystem::getSystemID(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"loader_version"))
	{
		OplLocalSystem::getLoaderVersion(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"update_date"))
	{
		OplLocalSystem::getUpdateDate(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"ciplus_enable"))
	{
		OplLocalSystem::getCIPlusEnabled(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"ciplus_version"))
	{
		OplLocalSystem::getCIPlusVersion(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"cislot_count"))
	{
		OplLocalSystem::getCISlotCount(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"micom_version"))
	{
		OplLocalSystem::getMicomVersion(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"rcu_customcode"))
	{
		int	customCode;
		OplLocalSystem::getRcuCustomCode(&customCode);
		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),"%d", customCode);
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"wakeupReason"))
	{
		OplLocalSystem::getWakeupReason(szLocalFieldbuf);
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"url_rp"))
	{
		OplLocalSystem::getRpURL(szLocalFieldbuf, MAX_URL_LEN);
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"url_portal"))
	{
		OplLocalSystem::getPortalURL(szLocalFieldbuf, MAX_URL_LEN);
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"url_hbbtv"))
	{
		if(OplLocalSystem::getHbbtvURL(szLocalFieldbuf, MAX_URL_LEN) == false)
		{
			*retval = NULL;
		}
		else
		{
			*retval = szLocalFieldbuf;
		}
	}
	else if (!strcmp(key,(const char *)"fvc_networkuriprefix"))
	{
		if(OplLocalSystem::getFvcNetworkUriPrefix(szLocalFieldbuf, MAX_URL_LEN) == false)
		{
			*retval = NULL;
		}
		else
		{
			*retval = szLocalFieldbuf;
		}
	}
	else if (!strcmp(key,(const char *)"fvc_baseurl"))
	{
		if(OplLocalSystem::getFvcBaseURL(szLocalFieldbuf, MAX_URL_LEN) == false)
		{
			*retval = NULL;
		}
		else
		{
			*retval = szLocalFieldbuf;
		}
	}
	else if (!strcmp(key,(const char *)"fvc_authhash"))
	{
		if(OplLocalSystem::getFvcAuthHash(szLocalFieldbuf, MAX_URL_LEN) == false)
		{
			*retval = NULL;
		}
		else
		{
			*retval = szLocalFieldbuf;
		}
	}
	else if (!strcmp(key,(const char *)"fvc_authtimestamp"))
	{
		if(OplLocalSystem::getFvcAuthTimeStamp(szLocalFieldbuf, MAX_URL_LEN) == false)
		{
			*retval = NULL;
		}
		else
		{
			*retval = szLocalFieldbuf;
		}
	}
	else if (!strcmp(key,(const char *)"url_ipota"))
	{
		OplLocalSystem::getIpOtaURL(szLocalFieldbuf, MAX_URL_LEN);
			*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdetoclient_version"))
	{
		OplLocalSystem::getIrdetoClientVersion(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_cwe"))
	{
		unsigned int bTemp = 0;

		OplLocalSystem::getIrdetoCWE(&bTemp);

		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),(bTemp==1)?"O":"X");
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_jtag"))
	{
		unsigned int bTemp = 0;

		OplLocalSystem::getIrdetoJTAG(&bTemp);

		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),(bTemp==1)?"O":"X");
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_sboot"))
	{
		unsigned int bTemp = 0;

		OplLocalSystem::getIrdetoSBOOT(&bTemp);

		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),(bTemp==1)?"O":"X");
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_otp"))
	{
		unsigned int bTemp = 0;

		OplLocalSystem::getIrdetoOTP(&bTemp);

		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),(bTemp==1)?"O":"X");
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_mancode"))
	{
		OplLocalSystem::getIrdetoManCode(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_hwcode"))
	{
		OplLocalSystem::getIrdetoHwCode(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_variant"))
	{
		OplLocalSystem::getIrdetoVariant(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_sysid"))
	{
		OplLocalSystem::getIrdetoSysId(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_sigver"))
	{
		OplLocalSystem::getIrdetoSigVer(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_keyver"))
	{
		OplLocalSystem::getIrdetoKeyVer(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_virtualscw"))
	{
		unsigned int bTemp = 0;

		OplLocalSystem::getIrdetoVirtualSecureCW(&bTemp);

		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),(bTemp==1)?"true":"false");
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_control_byte"))
	{
		int bTemp = 0;

		OplLocalSystem::getIrdetoControlByte(&bTemp);

		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),"%d", bTemp);
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"irdeto_private_data"))
	{
		OplLocalSystem::getIrdetoPrivateData(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"nagra_cwe"))
	{
		unsigned int bTemp = 0;

		OplLocalSystem::getNagraCWE(&bTemp);

		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),(bTemp==1)?"O":"X");
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"nagra_jtag"))
	{
		unsigned int bTemp = 0;

		OplLocalSystem::getNagraJTAG(&bTemp);

		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),(bTemp==1)?"O":"X");
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"nagra_sboot"))
	{
		unsigned int bTemp = 0;

		OplLocalSystem::getNagraSBOOT(&bTemp);

		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),(bTemp==1)?"O":"X");
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"nagra_otp"))
	{
		unsigned int bTemp = 0;

		OplLocalSystem::getNagraOTP(&bTemp);

		HxSTD_snprintf(szLocalFieldbuf, (MAX_BUF_LEN*2),(bTemp==1)?"O":"X");
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"nagra_nuid"))
	{
		OplLocalSystem::getNagraNUID(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"nagra_casn"))
	{
		OplLocalSystem::getNagraCASN(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if(!strcmp(key,(const char *)"nagra_fusing_info"))
	{
#if 0
		char string[50];
#endif
		BPL_BOOL IsSCS = FALSE, IsJTAG = FALSE, IsOTP = FALSE, IsCWE = FALSE;

		OplLocalSystem::getNagraBoxFusingInfo(&IsSCS, &IsJTAG, &IsOTP, &IsCWE);

#if 0
		memset(szLocalFieldbuf, 0, MAX_BUF_LEN*7);

		HxSTD_snprintf(string, 50, "{\"FUSINGINFO\":[");
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"SCS\", \"VALUE\":%d},", IsSCS);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"JTAG\", \"VALUE\":%d},", IsJTAG);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"OTP\", \"VALUE\":%d},", IsOTP);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"CWE\", \"VALUE\":%d}]}", IsCWE);
		HxSTD_StrCat(szLocalFieldbuf, string);
#else
		HxSTD_snprintf(szLocalFieldbuf, sizeof(szLocalFieldbuf),
				"{\"FUSINGINFO\":["
				"{\"NAME\":\"SCS\", \"VALUE\":%d},"
				"{\"NAME\":\"JTAG\", \"VALUE\":%d},"
				"{\"NAME\":\"OTP\", \"VALUE\":%d},"
				"{\"NAME\":\"CWE\", \"VALUE\":%d}]}",
				IsSCS, IsJTAG, IsOTP, IsCWE);
#endif

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"fusing_info"))
	{
		char string[50];
		BPL_BOOL IsSCS = FALSE, IsJTAG = FALSE, IsOTP = FALSE, IsCWE = FALSE;
        unsigned int ulMarketId = 0;

		OplLocalSystem::getBoxFusingInfo(&IsSCS, &IsJTAG, &IsOTP, &IsCWE, &ulMarketId);

		memset(szLocalFieldbuf, 0, MAX_BUF_LEN*7);
		HxSTD_snprintf(string, 50, "{\"FUSINGINFO\":[");
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"SCS\", \"VALUE\":%d},", IsSCS);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"JTAG\", \"VALUE\":%d},", IsJTAG);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"OTP\", \"VALUE\":%d},", IsOTP);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"CWE\", \"VALUE\":%d},", IsCWE);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"M-ID\", \"VALUE\":%d}]}", ulMarketId);
		HxSTD_StrCat(szLocalFieldbuf, string);
		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"fusing_info_ext"))
	{
		char string[50];
		BPL_BOOL IsSCS = FALSE, IsJTAG = FALSE, IsOTP = FALSE, IsCWE = FALSE;
		unsigned int	ulOTPField1 = 0;
		unsigned int	ulOTPField2 = 0;

		OplLocalSystem::getBoxFusingInfoExt(&IsSCS, &IsJTAG, &IsOTP, &IsCWE, &ulOTPField1, &ulOTPField2);

		memset(szLocalFieldbuf, 0, MAX_BUF_LEN*7);

		HxSTD_snprintf(string, 50, "{\"FUSINGINFO\":[");
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"SCS\", \"VALUE\":%d},", IsSCS);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"JTAG\", \"VALUE\":%d},", IsJTAG);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"OTP\", \"VALUE\":%d},", IsOTP);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"CWE\", \"VALUE\":%d},", IsCWE);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"CWE\", \"VALUE\":%d},", IsCWE);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"OTPField1\", \"VALUE\":%d},", ulOTPField1);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"OTPField2\", \"VALUE\":%d}]}", ulOTPField2);
		HxSTD_StrCat(szLocalFieldbuf, string);


		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"nand_badblocks"))
	{
		char string[50];

		unsigned int ulCntLdr = 0;
		unsigned int ulCntUBI = 0;
		unsigned int ulCntUBIFS = 0;

		OplLocalSystem::getNandBadBlocks(&ulCntLdr, &ulCntUBI, &ulCntUBIFS );

		memset(szLocalFieldbuf, 0, MAX_BUF_LEN*7);

		HxSTD_snprintf(string, 50, "{\"NANDBADBLOCKS\":[");
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"LOADER\", \"VALUE\":%d},", ulCntLdr);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"UBI\", \"VALUE\":%d},", ulCntUBI);
		HxSTD_StrCat(szLocalFieldbuf, string);
		HxSTD_snprintf(string, 50, "{\"NAME\":\"UBIFS\", \"VALUE\":%d}]}", ulCntUBIFS);
		HxSTD_StrCat(szLocalFieldbuf, string);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"hdmi_edidinfo"))
	{
		OplLocalSystem::getHdmiEdidInfo(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
	}
	else if (!strcmp(key,(const char *)"network_ids"))
	{
		if(OplLocalSystem::getNetworkId(szLocalFieldbuf, MAX_URL_LEN) == false)
		{
			*retval = NULL;
		}
		else
		{
			*retval = szLocalFieldbuf;
		}
	}
    //upgrader version of boot loader
    else if(!strcmp(key,(const char *)"upgrader_version"))
    {
        OplLocalSystem::getUpgraderVersion(szLocalFieldbuf);

		*retval = szLocalFieldbuf;
    }
    //for verimaxtrix
    else if(!strcmp(key,(const char *)"verimatrix_iptvhybridlib_version"))
    {
        OplLocalSystem::getVerimatrixLibVersion(OPLVERIMATRIX_LIB_IPTV, szLocalFieldbuf, 2048);

        *retval = szLocalFieldbuf;
    }
    else if(!strcmp(key,(const char *)"verimatrix_weblib_version"))
    {
        OplLocalSystem::getVerimatrixLibVersion(OPLVERIMATRIX_LIB_WEB, szLocalFieldbuf, 2048);

        *retval = szLocalFieldbuf;
    }
    else if(!strcmp(key,(const char *)"verimatrix_gatewaylib_version"))
    {
        OplLocalSystem::getVerimatrixLibVersion(OPLVERIMATRIX_LIB_GATEWAY, szLocalFieldbuf, 2048);

        *retval = szLocalFieldbuf;
    }
    else if (!strcmp(key,(const char *)"nflix_esn"))
    {
        OplLocalSystem::getNflixESN(szLocalFieldbuf);

        *retval = szLocalFieldbuf;
    }
	else
	{
		*retval = "Dummy Native Text";
	}

	return OOIF_RETURN_OK;
}


// void setText(String key, String value);
OOIFReturnCode native_HLocalSystemSetField(const char *key, const char *value)
{
	if (!strcmp(key,(const char *)"irdeto_virtualscw"))
	{
		HBOOL	bvirtualscw = FALSE;

		if (strcmp(value, "true") == 0)
			bvirtualscw = 1;
		else
			bvirtualscw = 0;

		OplLocalSystem::setIrdetoVirtualSecureCW(bvirtualscw);

		return OOIF_RETURN_OK;
	}
	else if (!strcmp(key,(const char *)"fvc_baseurl"))
	{
		OplLocalSystem::setFvcBaseURL(value);

		return OOIF_RETURN_OK;
	}
	else if (!strcmp(key,(const char *)"fvc_authhash"))
	{
		OplLocalSystem::setFvcAuthHash(value);

		return OOIF_RETURN_OK;
	}
	else if (!strcmp(key,(const char *)"fvc_authtimestamp"))
	{
		OplLocalSystem::setFvcAuthTimeStamp(value);

		return OOIF_RETURN_OK;
	}
	else if (!strcmp(key,(const char *)"ntp_server_url"))
	{
		OplLocalSystem::setNtpServerURL(value);

		return OOIF_RETURN_OK;
	}
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_HLocalSystemSetBoxFirstBootStatus(OOIFBoolean nStatus)
{
	OplLocalSystem::setBoxFirstBootStatus(nStatus);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemGetBoxFirstBootStatus(OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	BPL_BOOL status;

	OplLocalSystem::getBoxFirstBootStatus(&status);

	*retval = (OOIFNumber)status;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemGetLaunchAppMode(OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	unsigned int status;

	OplLocalSystem::getBoxLaunchAppMode(&status);

	*retval = (OOIFNumber)status;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemSetAntennaPowerStatus(OOIFBoolean nStatus)
{
	OplLocalSystem::setAntennaPowerStatus(nStatus);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemGetAntennaPowerStatus(OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	BPL_BOOL status;

	OplLocalSystem::getAntennaPowerStatus(&status);

	*retval = (OOIFNumber)status;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemSetServiceUpdateFlag(OOIFBoolean nStatus)
{
	OplLocalSystem::setServiceUpdateFlag(nStatus);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemGetServiceUpdateFlag(OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	BPL_BOOL status;

	OplLocalSystem::getServiceUpdateFlag(&status);

	*retval = (OOIFNumber)status;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemCheckFileExistence( const char *uri, OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplLocalSystem::checkFileExist((char*)uri, (unsigned int*)retval);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemShowMsgbox(OOIFBoolean *retval)
{

	OplLocalSystem::showMsgbox();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemShowKeyboard(OOIFBoolean *retval, OOIFNumber type, OOIFBoolean pwmode, const char *str)
{
	OplLocalSystem::showKeyboard((int)type, (int)pwmode, (char*)str);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemUserActionIdle(OOIFBoolean *retval)
{

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemGetHCRData(const char **retval)
{
	HBOOL result;
	char	*pVal = NULL;

	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	result = OplLocalSystem::getHCRData(&pVal);

	*retval = pVal;

	if (result != TRUE)
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemCheckHCRData(OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	*retval = OplLocalSystem::checkHCRData();

	return OOIF_RETURN_OK;
}

#endif // OIPF


// -------------------------------
// Configuration
// -------------------------------

// parameters
// ---------------

// String preferredAudioLanguage
OOIFReturnCode native_HConfigGetPreferredAudioLanguage(const char **retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	static char szPreferredAudioLangbuf[MAX_BUF_LEN];

	HxSTD_MemSet(szPreferredAudioLangbuf,0,MAX_BUF_LEN);

	OplLocalSystem::getPreferredAudioLanguage(szPreferredAudioLangbuf, 0);

	*retval = szPreferredAudioLangbuf;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetPreferredSecondAudioLanguage(const char **retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	static char szPreferredAudioLangbuf[MAX_BUF_LEN];

	HxSTD_MemSet(szPreferredAudioLangbuf,0,MAX_BUF_LEN);

	OplLocalSystem::getPreferredAudioLanguage(szPreferredAudioLangbuf, 1);

	*retval = szPreferredAudioLangbuf;

	return OOIF_RETURN_OK;
}

#if defined OIPF || defined OPT_HBBTV_EXT_CONFIG
OOIFReturnCode native_HConfigSetPrefferedAudioLangue(const char *value)
{
	if (NULL == value)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplLocalSystem::setPreferredAudioLanguage(value, 0);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetPrefferedSecondAudioLangue(const char *value)
{
	if (NULL == value)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplLocalSystem::setPreferredAudioLanguage(value, 1);

	return OOIF_RETURN_OK;
}
#endif // OIPF || OPT_HBBTV_EXT_CONFIG

// String preferredSubtitleLanguage
OOIFReturnCode native_HConfigGetPreferredSubtitleLanguage(const char **retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	static char szPreferredSubtitleLangbuf[MAX_BUF_LEN];

	HxSTD_MemSet(szPreferredSubtitleLangbuf,0,MAX_BUF_LEN);

	OplLocalSystem::getPreferredSubtitleLanguage(szPreferredSubtitleLangbuf, 0);

	*retval = szPreferredSubtitleLangbuf;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetPreferredSecondSubtitleLanguage(const char **retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	static char szPreferredSubtitleLangbuf[MAX_BUF_LEN];

	HxSTD_MemSet(szPreferredSubtitleLangbuf,0,MAX_BUF_LEN);

	OplLocalSystem::getPreferredSubtitleLanguage(szPreferredSubtitleLangbuf, 1);

	*retval = szPreferredSubtitleLangbuf;

	return OOIF_RETURN_OK;
}


#if defined OIPF || defined OPT_HBBTV_EXT_CONFIG
OOIFReturnCode native_HConfigSetPreferredSubtitleLanguage(const char *value)
{

	OplLocalSystem::setPreferredSubtitleLanguage(value, 0);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetPreferredSecondSubtitleLanguage(const char *value)
{

	OplLocalSystem::setPreferredSubtitleLanguage(value, 1);

	return OOIF_RETURN_OK;
}

#endif // OIPF || OPT_HBBTV_EXT_CONFIG

// String countryId
OOIFReturnCode native_HConfigGetCountryId(const char **retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	static char szCountryIdbuf[MAX_BUF_LEN];

	HxSTD_MemSet(szCountryIdbuf,0,MAX_BUF_LEN);

	OplLocalSystem::getCountryID(szCountryIdbuf,MAX_BUF_LEN);

	*retval = szCountryIdbuf;


	return OOIF_RETURN_OK;
}

#if defined OIPF || defined OPT_HBBTV_EXT_CONFIG
OOIFReturnCode native_HConfigSetCountryId(const char *value)
{
	OplLocalSystem::setCountryID(value);

	return OOIF_RETURN_OK;
}


// Integer operator id
OOIFReturnCode native_HConfigGetOperatorId(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int operatorID = 0;

	OplLocalSystem::getOperatorID(&operatorID);

	*retval = (OOIFNumber)operatorID;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HConfigSetOperatorId(OOIFNumber value)
{
	OplLocalSystem::setOperatorID((int)value);

	return OOIF_RETURN_OK;
}


// Integer regionId
OOIFReturnCode native_HConfigGetRegionId(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int regionID = 0;

	OplLocalSystem::getRegionID(&regionID);

	*retval = (OOIFNumber)regionID;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetRegionId(OOIFNumber value)
{
	OplLocalSystem::setRegionID((int)value);

	return OOIF_RETURN_OK;
}

// ro Boolean isPINEntryLocked
OOIFReturnCode native_HConfigGetIsPINEntryLocked(OOIFBoolean *retval)
{
	*retval = 0;
	return OOIF_RETURN_NOT_SUPPORTED;
}

// Integer pvrPolicy
OOIFReturnCode native_HConfigGetPvrPolicy(OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetPvrPolicy(OOIFNumber value)
{
	return OOIF_RETURN_OK;
}

// Integer pvrSaveEpisodes
OOIFReturnCode native_HConfigGetPvrSaveEpisodes(OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetPvrSaveEpisodes(OOIFNumber value)
{
	return OOIF_RETURN_OK;
}

// Integer pvrSaveDays
OOIFReturnCode native_HConfigGetPvrSaveDays(OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetPvrSaveDays(OOIFNumber value)
{
	return OOIF_RETURN_OK;
}

// Integer pvrStartPadding
OOIFReturnCode native_HConfigGetPvrStartPadding(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int nStartPaddingTime = 0;

	OplLocalSystem::getStartPaddingTime(&nStartPaddingTime);

	*retval = (OOIFNumber)nStartPaddingTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetPvrStartPadding(OOIFNumber value)
{
	OplLocalSystem::setStartPaddingTime((int)value);

	return OOIF_RETURN_OK;

}

// Integer pvrEndPadding
OOIFReturnCode native_HConfigGetPvrEndPadding(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int nEndPaddingTime = 0;

	OplLocalSystem::getEndPaddingTime(&nEndPaddingTime);

	*retval = (OOIFNumber)nEndPaddingTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetPvrEndPadding(OOIFNumber value)
{
	OplLocalSystem::setEndPaddingTime((int)value);

	return OOIF_RETURN_OK;
}

// Integer preferredTimeShiftMode
OOIFReturnCode native_HConfigGetPreferredTimeShiftMode(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetPreferredTimeShiftMode(OOIFNumber value)
{
	OOIF_LOG_DEBUG("Setting Configuration.preferredTimeShiftMode to : %f",value);
	return OOIF_RETURN_OK;
}

// methods
// ---------------

// Integer setParentalControlPIN(String oldPcPin, String newPcPin)
OOIFReturnCode native_HConfigSetParentalControlPIN(const char *oldPcPin,
		const char *newPcPin,
		OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 0;
	return OOIF_RETURN_NOT_SUPPORTED;
}

// Integer setParentalControlPINEnable(String pcPIN, Boolean enable)
OOIFReturnCode native_HConfigSetParentalControlPINEnable(const char *pcPIN, OOIFBoolean enable,
		OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 0;
	return OOIF_RETURN_NOT_SUPPORTED;
}

// Boolean getParentalControlPINEnable()
OOIFReturnCode native_HConfigGetParentalControlPINEnable(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 0;
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_HConfigUnlockWithParentalControlPIN(const char *pcPIN,
		void *target, OOIFNumber duration,
		OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 0;
	return OOIF_RETURN_NOT_SUPPORTED;
}

// Integer verifyParentalControlPIN(String pcPIN)
OOIFReturnCode native_HConfigVerifyParentalControlPIN(const char *pcPIN, OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 0;
	return OOIF_RETURN_NOT_SUPPORTED;
}

// Integer setBlockUnrated(String pcPIN, Boolean block)
OOIFReturnCode native_HConfigSetBlockUnrated(const char *pcPIN, OOIFBoolean block, OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 0;
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_HConfigGetBlockUnrated(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 1;
	return OOIF_RETURN_NOT_SUPPORTED;
}

// String getText(String key)
OOIFReturnCode native_HConfigGetText(const char *key, const char **retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = "Not valid Key";
	return OOIF_RETURN_OK;
}

// void setText(String key, String value);
OOIFReturnCode native_HConfigSetText(const char *key, const char *value)
{
	return OOIF_RETURN_NOT_SUPPORTED;
}

#if defined(HMX_WEBUI)
// String preferredMenuLanguage
OOIFReturnCode native_HConfigGetPreferredMenuLanguage(const char **retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	static char szPreferredMenuLang[5];

	HxSTD_MemSet(szPreferredMenuLang,0,5);

	OplLocalSystem::getPreferredMenuLanguage(szPreferredMenuLang);

	*retval = szPreferredMenuLang;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetPreferredMenuLanguage(const char *value)
{
	if (NULL == value)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplLocalSystem::setPreferredMenuLanguage(value);
	HAPI_BroadcastSignal("signal:onSystemLanguageChanged", 0, "i", HLIB_LANG_639ToId(value));

	return OOIF_RETURN_OK;
}

// standbyPowerMode in HConfiguration (getter)
OOIFReturnCode native_HConfigGetStandByPowerMode(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int nStandByPowerMode = 0;

	OplLocalSystem::getStandByPowerMode(&nStandByPowerMode);

	*retval = (OOIFNumber)nStandByPowerMode;

	return OOIF_RETURN_OK;
}

// standbyPowerMode in HConfiguration (setter)
OOIFReturnCode native_HConfigSetStandByPowerMode(OOIFNumber v)
{
	OplLocalSystem::setStandByPowerMode((int)v);

	return OOIF_RETURN_OK;
}

// standbySleepMode in HConfiguration (getter)
OOIFReturnCode native_HConfigGetStandBySleepMode(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int nStandBySleepMode = 0;

	OplLocalSystem::getStandBySleepMode(&nStandBySleepMode);

	*retval = (OOIFNumber)nStandBySleepMode;

	return OOIF_RETURN_OK;
}

// standbySleepMode in HConfiguration (setter)
OOIFReturnCode native_HConfigSetStandBySleepMode(OOIFNumber v)
{
	OplLocalSystem::setStandBySleepMode((int)v);

	return OOIF_RETURN_OK;
}

// standbyBootMode in HConfiguration (getter)
OOIFReturnCode native_HConfigGetStandByBootMode(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int nStandByBootMode = 0;

	OplLocalSystem::getStandByBootMode(&nStandByBootMode);

	*retval = (OOIFNumber)nStandByBootMode;

	return OOIF_RETURN_OK;
}

// standbyBootMode in HConfiguration (setter)
OOIFReturnCode native_HConfigSetStandByBootMode(OOIFNumber v)
{
	OplLocalSystem::setStandByBootMode((int)v);

	return OOIF_RETURN_OK;
}

// standbyWifiPower in HConfiguration (getter)
OOIFReturnCode native_HConfigGetStandByWifiPower(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int nStandByWifiPower = 0;

	OplLocalSystem::getStandByWifiPower(&nStandByWifiPower);

	*retval = (OOIFNumber)nStandByWifiPower;

	return OOIF_RETURN_OK;
}

// standbyWifiPower in HConfiguration (setter)
OOIFReturnCode native_HConfigSetStandByWifiPower(OOIFNumber v)
{
	OplLocalSystem::setStandByWifiPower((int)v);

	return OOIF_RETURN_OK;
}

// Boolean automaticStandby
OOIFReturnCode native_HConfigGetAutomaticStandby(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (OOIFBoolean)OplLocalSystem::getAutomaticStandby();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetAutomaticStandby(OOIFBoolean v)
{
	OplLocalSystem::setAutomaticStandby((BPL_BOOL)v);

	return OOIF_RETURN_OK;
}


// Boolean autoDelete
OOIFReturnCode native_HConfigGetAutoDelete(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (OOIFBoolean)OplLocalSystem::getAutoDelete();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetAutoDelete(OOIFBoolean v)
{
	OplLocalSystem::setAutoDelete((BPL_BOOL)v);

	return OOIF_RETURN_OK;
}


// Boolean audioBleepEnabled
OOIFReturnCode native_HConfigGetAudioBleep(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (OOIFBoolean)OplLocalSystem::getAudioBleep();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetAudioBleep(OOIFBoolean v)
{
	OplLocalSystem::setAudioBleep((BPL_BOOL)v);

	return OOIF_RETURN_OK;
}


// Boolean subtitleFontType
OOIFReturnCode native_HConfigGetSubtitleFontType(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int subtitleFontType = 0;

	OplLocalSystem::getSubtitleFontType(&subtitleFontType);

	*retval = (OOIFNumber)subtitleFontType;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetSubtitleFontType(OOIFNumber v)
{
	OplLocalSystem::setSubtitleFontType(v);

	return OOIF_RETURN_OK;
}

// Boolean infoDisplayTimeout
OOIFReturnCode native_HConfigGetInfoDisplayTimeout(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int infoDispTimeout = 0;

	OplLocalSystem::getInfoDisplayTimeout(&infoDispTimeout);

	*retval = infoDispTimeout;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetInfoDisplayTimeout(OOIFNumber v)
{
	OplLocalSystem::setInfoDisplayTimeout(v);

	return OOIF_RETURN_OK;
}

// String getText(String key)
OOIFReturnCode native_HConfigGetField(const char *key, const char **retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	static char szConfFieldbuf[MAX_BUF_LEN*2];

	HxSTD_MemSet(szConfFieldbuf,0,(MAX_BUF_LEN*2));

	OplLocalSystem::getField(key, szConfFieldbuf, MAX_BUF_LEN*2);

	*retval = szConfFieldbuf;

	OOIF_LOG_INFO("[%s:%d] [key:%s][*retval:%s]\n",__FUNCTION__,__LINE__,key,*retval);

	return OOIF_RETURN_OK;
}


// void setText(String key, String value);
OOIFReturnCode native_HConfigSetField(const char *key, const char *value)
{
	if ((NULL == key) || (NULL == value))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OOIF_LOG_INFO("[%s:%d] key:%s value:%s\n",__FUNCTION__,__LINE__,key,value);

	OplLocalSystem::setField(key, value);

	return OOIF_RETURN_OK;
}

// void setText(String key, String value);
OOIFReturnCode native_HConfigDoFactoryDefault(const char *Default_Values)
{
	OplLocalSystem::doFactoryDefault(Default_Values);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetTimeshiftEnabled(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (OOIFBoolean)OplLocalSystem::getTimeshiftEnabled();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetTimeshiftEnabled(OOIFBoolean v)
{
	OplLocalSystem::setTimeshiftEnabled(v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetFavChListNumberingMode(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (OOIFBoolean)OplLocalSystem::getFavChListNumberingMode();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetFavChListNumberingMode(OOIFBoolean v)
{
	OplLocalSystem::setFavChListNumberingMode(v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetDescrambleOnOff(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (OOIFBoolean)OplLocalSystem::getDescrambleOnOff();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetDescrambleOnOff(OOIFBoolean v)
{
	OplLocalSystem::setDescrambleOnOff(v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetSkipForwardTime(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int SkipForwardTime = 0;

	OplLocalSystem::getSkipForwardTime(&SkipForwardTime);

	*retval = SkipForwardTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetSkipForwardTime(OOIFNumber v)
{
	OplLocalSystem::setSkipForwardTime(v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetInstantReplayTime(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int InstantReplayTime = 0;

	OplLocalSystem::getInstantReplayTime(&InstantReplayTime);

	*retval = InstantReplayTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetInstantReplayTime(OOIFNumber v)
{
	OplLocalSystem::setInstantReplayTime(v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetFirstTimeBoot(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int InstantReplayTime = 0;

	OplLocalSystem::getFirstTimeBoot(&InstantReplayTime);

	*retval = InstantReplayTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetFirstTimeBoot(OOIFBoolean v)
{
	OplLocalSystem::setFirstTimeBoot(v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigDoDbRestore(const char *ModelName, OOIFNumber type_of_restore, OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetCamNameBySlot(OOIFNumber v, const char **retval)
{
	unsigned char slotNumber = (unsigned char)v;

	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	// MAX_CI_STR_LEN´Â 40, APKS_CAM_NAME_LEN´Â 50À¸·Î µÇ¾î¼­, ÀÏ´Ü buf Å©±â ´Ã·ÁÁÜ.
	static char szCamNamebuf[MAX_BUF_LEN*2];

	HxSTD_MemSet(szCamNamebuf,0,MAX_BUF_LEN*2);

	OplLocalSystem::getCICamNameBySlot(slotNumber, szCamNamebuf);

	*retval = szCamNamebuf;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetTargetStorage(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int TargetStorage = 0;

	OplLocalSystem::getTargetStorage(&TargetStorage);

	*retval = TargetStorage;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetTargetStorage(OOIFNumber v)
{
	OplLocalSystem::setTargetStorage((OOIFNumber)v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetServerActivationTime(OOIFNumber startHour, OOIFNumber startMin, OOIFNumber durationMin, OOIFBoolean bEnable)
{
	int startTime, duration;

	if (bEnable == FALSE)
	{
		startTime = 0;
		duration = 0;
	}
	else
	{
		startTime = (startHour * 60 * 60) + (startMin * 60);
		duration = durationMin * 60;
	}

	OPL_Scheduler_ActivationTimeSet(OPL_Scheduler_GetInstance(), (unsigned long)startTime, (unsigned long)duration, bEnable);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetHbbTvEnabled(OOIFBoolean v)
{
	OplLocalSystem::setHbbTvEnabled((OOIFBoolean)v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetHbbTvEnabled(OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	BPL_BOOL status;

	OplLocalSystem::getHbbTvEnabled(&status);

	*retval = (OOIFBoolean)status;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetLastServiceGroup(OOIFNumber value)
{
	OplLocalSystem::setLastServiceGroup((int)value);

	return OOIF_RETURN_OK;

}

// Integer pvrEndPadding
OOIFReturnCode native_HConfigGetLastServiceGroup(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int nLastSvcGroup = 0;

	OplLocalSystem::getLastServiceGroup(&nLastSvcGroup);

	*retval = (OOIFNumber)nLastSvcGroup;

	return OOIF_RETURN_OK;
}

// batupdatemode in HConfiguration (getter)
OOIFReturnCode native_HConfigGetBatUpdateMode(OOIFNumber *retval)
{
	if (NULL == retval)
{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int nBatUpdateMode = 0;

	OplLocalSystem::getBatUpdateMode(&nBatUpdateMode);

	*retval = (OOIFNumber)nBatUpdateMode;

	return OOIF_RETURN_OK;
}

// batupdatemode in HConfiguration (setter)
OOIFReturnCode native_HConfigSetBatUpdateMode(OOIFNumber v)
{
	OplLocalSystem::setBatUpdateMode((int)v);

	return OOIF_RETURN_OK;
}

// nitlcnupdatemode in HConfiguration (getter)
OOIFReturnCode native_HConfigGetNitLcnUpdateMode(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int nNitLcnUpdateMode = 0;

	OplLocalSystem::getNitLcnUpdateMode(&nNitLcnUpdateMode);

	*retval = (OOIFNumber)nNitLcnUpdateMode;

	return OOIF_RETURN_OK;
}

// nitlcnupdatemode in HConfiguration (setter)
OOIFReturnCode native_HConfigSetNitLcnUpdateMode(OOIFNumber v)
{
	OplLocalSystem::setNitLcnUpdateMode((int)v);

	return OOIF_RETURN_OK;
}

#define ____HMX_Channel_setter_functions____
// Boolean channelchangepin
OOIFReturnCode native_HConfigGetChannelSetupPIN(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (OOIFBoolean)OplLocalSystem::getChannelSetupPIN();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetChannelSetupPIN(OOIFBoolean v)
{
	OplLocalSystem::setChannelSetupPIN((BPL_BOOL)v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetGuidancePolicy(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	unsigned int GuidancePolicy = 0;

	OplLocalSystem::getGuidancePolicy(&GuidancePolicy);

	*retval = GuidancePolicy;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetGuidancePolicy(OOIFNumber v)
{
	OplLocalSystem::setGuidancePolicy((OOIFNumber)v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetHdmiCecMode(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OOIFBoolean fMode = 0;

	OplLocalSystem::getHdmiCecMode((BPL_BOOL*)&fMode);

	*retval = fMode;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetHdmiCecMode(OOIFBoolean v)
{
	OplLocalSystem::setHdmiCecMode((BPL_BOOL)v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigGetHdmiCecVolumeCtrlMode(OOIFBoolean *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OOIFBoolean fMode = 0;

	OplLocalSystem::getHdmiCecVolumeCtrlMode((BPL_BOOL*)&fMode);

	*retval = fMode;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HConfigSetHdmiCecVolumeCtrlMode(OOIFBoolean v)
{
	OplLocalSystem::setHdmiCecVolumeCtrlMode((BPL_BOOL)v);

	return OOIF_RETURN_OK;
}


#define SESSATRECORD_CONFIRMCODE_LEN_MAX (8)
OOIFReturnCode native_HConfigGetSatRecordConfirmCode(const char *registrationCode, const char **retval)
{
	static char szConfirmcode[SESSATRECORD_CONFIRMCODE_LEN_MAX+1];
	HxSTD_MemSet(szConfirmcode,0,SESSATRECORD_CONFIRMCODE_LEN_MAX+1);

	OplLocalSystem::getSatRecordConfirmCode(registrationCode, szConfirmcode);

	*retval = szConfirmcode;
	 return OOIF_RETURN_OK;
}

// Integer microphoneVolume
OOIFReturnCode native_HLocalSystemGetMicrophoneVolume(OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	unsigned int volume = 0;

	OplLocalSystem::getMicrophoneVolume(&volume);

	*retval = volume;

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_HLocalSystemSetMicrophoneVolume(OOIFNumber v)
{
	OplLocalSystem::setMicrophoneVolume((unsigned int)v);

	return OOIF_RETURN_OK;
}

// Boolean microphoneEnabled
OOIFReturnCode native_HLocalSystemGetMicrophoneEnabled(OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	*retval = (OOIFBoolean)OplLocalSystem::getMicrophoneEnabled();

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_HLocalSystemSetMicrophoneEnabled(OOIFBoolean v)
{
	OplLocalSystem::setMicrophoneEnabled((BPL_BOOL)v);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemGetConnectedMicrophone(OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	unsigned int count = 0;

	OplLocalSystem::getConnectedMicrophone(&count);

	*retval = count;

	return OOIF_RETURN_OK;
}

// Integer time (msec)
OOIFReturnCode native_HLocalSystemGetAutomaticPowerdown(OOIFNumber *retval)
{
	unsigned int nTimeout = 0;

	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	OplLocalSystem::getAutomaticPowerdown(&nTimeout);
	*retval = (OOIFNumber)nTimeout;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemSetAutomaticPowerdown(OOIFNumber value)
{
	OplLocalSystem::setAutomaticPowerdown((int)value);

	return OOIF_RETURN_OK;

}

// priority outputs
// 0: HDMI
// 1 : scart
// 2 : all

OOIFReturnCode native_HLocalSystemGetPriorityOutputs(OOIFNumber *retval)
{
    unsigned int priority = 0;

    if (NULL == retval)
    {
        return OOIF_RETURN_VALUE_UNDEFINED;
    }
    OplLocalSystem::getPriorityHDMIOutputs(&priority);
    *retval = (OOIFNumber)priority;

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemSetPriorityOutputs(OOIFNumber value)
{
	OplLocalSystem::setPriorityHDMIOutputs((unsigned int)value);

	return OOIF_RETURN_OK;

}

#endif	// HUMAX EXTENSION
#endif // OIPF || OPT_HBBTV_EXT_CONFIG

#ifdef OIPF
OOIFReturnCode native_HNetworkInterfaceGetIndex(HNetworkInterfaceHandle identifier, OOIFNumber *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = (OOIFNumber)networkinterfaceObj->getIndex();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetIPAddress(HNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = networkinterfaceObj->getIpAddress();

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetMacAddress(HNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = networkinterfaceObj->getMacAddress();

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetConnected(HNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = (OOIFBoolean)networkinterfaceObj->getConnected();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetEnabled(HNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = (OOIFBoolean)networkinterfaceObj->getEnabled();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetEnabled(HNetworkInterfaceHandle identifier, OOIFBoolean enabled)
{
#if 0
	browser_network_t *netdef = (browser_network_t *)identifier;

	browser_error err = browser_config_network_set_enabled(netdef, (unsigned int)enabled);
	if ( err != ERR_BROWSER_OK )
	{
		return OOIF_RETURN_ERROR;
	}
#endif

	return OOIF_RETURN_NOT_SUPPORTED;
}

#if defined(HMX_WEBUI)
OOIFReturnCode native_HNetworkInterfaceGetNetmask(HNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = networkinterfaceObj->getNetmask();

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetGateway(HNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = networkinterfaceObj->getGateway();

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetDns1(HNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = networkinterfaceObj->getDNS1();

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetDns2(HNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = networkinterfaceObj->getDNS2();

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetDHCPStatus(HNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = (OOIFBoolean)networkinterfaceObj->getDHCPStatus();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetDNSAutoStatus(HNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = (OOIFBoolean)networkinterfaceObj->getDNSAutoStatus();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetNetworkType(HNetworkInterfaceHandle identifier, OOIFNumber *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	*retval = networkinterfaceObj->getNetworkType();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetIPAddres(HNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpAddress( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetNetmask(HNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setNetmask( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetGateway(HNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setGateway( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetDNS1(HNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setDNS1( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetDNS2(HNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setDNS2( value );

	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetDHCPStatus(HNetworkInterfaceHandle identifier, OOIFBoolean bOn )
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setDHCPStatus( bOn );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetDNSAutoStatus(HNetworkInterfaceHandle identifier, OOIFBoolean bOn )
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setDNSAutoStatus( bOn );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}


#define ____Support_IPv6_start____

OOIFReturnCode native_HNetworkInterfaceGetIpv6Address(HNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpv6Address();

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetIpv6Prefix(HNetworkInterfaceHandle identifier, OOIFNumber *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	int ret;
	ret  = networkinterfaceObj->getIpv6Prefix();
	*retval = (OOIFNumber) ret;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetIpv6Gateway(HNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpv6Gateway();

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetIpv6Dns1(HNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpv6DNS1();

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetIpv6Dns2(HNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpv6DNS2();

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetIpv6DHCPStatus(HNetworkInterfaceHandle identifier, OOIFBoolean *value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*value = (OOIFBoolean)networkinterfaceObj->getIpv6DhcpOn();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceGetIpv6DNSAutoStatus(HNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = (OOIFBoolean)networkinterfaceObj->getIpv6DnsAuto();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetIpv6Addres(HNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6Address( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetIpv6Prefix(HNetworkInterfaceHandle identifier, OOIFNumber value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6Prefix( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetIpv6Gateway(HNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6Gateway( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetIpv6DNS1(HNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6DNS1( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetIpv6DNS2(HNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6DNS2( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetIpv6DHCPStatus(HNetworkInterfaceHandle identifier, OOIFBoolean bOn )
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6DhcpOn( bOn );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceSetIpv6DNSAutoStatus(HNetworkInterfaceHandle identifier, OOIFBoolean bOn )
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6DnsSAuto( bOn );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}


#define ___Support_IPv6_end___



OOIFReturnCode native_HNetworkInterfaceApplyConfigure(HNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{

	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->applyConfigure();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceStopConnecting(HNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->stopConnecting();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceCheckGatewayAccess(HNetworkInterfaceHandle identifier, OOIFNumber *retval)
{

	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->checkGatewayAccess();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceCheckDnsAccess(HNetworkInterfaceHandle identifier, OOIFNumber *retval)
{

	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->checkDnsAccess();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceCheckInternetConnection(HNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{

	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->checkInternetConnection();
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HNetworkInterfaceRegisterEventHandler(HNetworkInterfaceHandle identifier, NetworkEventCallback cbFunc)
{

	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OplNetworkInterface::registerEventListener( networkinterfaceObj, cbFunc );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceUnRegisterEventHandler(HNetworkInterfaceHandle identifier)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OplNetworkInterface::unRegisterEventListener( networkinterfaceObj );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceRefreshAddressInfo(HNetworkInterfaceHandle identifier)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	networkinterfaceObj->refreshData();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkManagerDefaultDevIndex(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int index;
	OPL_Network_GetDefaultDevIndex( &index );
	*retval = index;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkManagerLinkStatus(OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	int status = 0;
	OPL_Network_GetLinkStatus( &status );
	*retval = status;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkManagerRegisterEventHandler( NetworkEventCallback cbFunc )
{
	OPL_NetworkManager_RegisterListener( cbFunc );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkManagerUnRegisterEventHandler( void )
{
	OPL_NetworkManager_UnRegisterListener();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkWiFiConfigApScanning( int nDevIndex, OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int status = TRUE;
	OPL_NetworkWiFi_ApScanning( nDevIndex );
	*retval = status;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkWiFiConfigApplyConnectToAP( int nDevIndex, void * identifier, const char *szKey, OOIFBoolean *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	OPL_NetworkWiFi_ConnectToAP( nDevIndex, apObj, szKey );
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkWiFiConfigApplyConnectToAPbyWPS( int nDevIndex, int nType, const char *szPincode, OOIFBoolean *retval)
{
	OPL_NetworkWiFi_ConnectToAPbyWPS( nDevIndex, nType, szPincode );
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkWiFiGetConnectedApInfo( int nDevIndex, NetworkApHandle * identifier)
{
	BPLStatus result = BPL_STATUS_ERROR;
	if ( NULL == identifier )
	{
		return OOIF_RETURN_ERROR;
	}

	OplNetworkAccessPoint * apInfo = (OplNetworkAccessPoint *)HLIB_STD_MemAlloc(sizeof(OplNetworkAccessPoint));
	HxSTD_MemSet(apInfo, 0x00, sizeof(OplNetworkAccessPoint));
	result = OPL_NetworkWiFi_GetConnectedInfo( nDevIndex, apInfo );
	if ( BPL_STATUS_OK != result )
	{
		HLIB_STD_MemFree( apInfo );
		*identifier = NULL;
		return OOIF_RETURN_OK;
	}
	/*¸Þ¸ð¸® ÇØÁ¦´Â OIPF °´Ã¼ ¼Ò¸ê½Ã*/
	*identifier = apInfo;
	return OOIF_RETURN_OK;
}

static OplNetworkAccessPoint * s_userAp = NULL;
OOIFReturnCode native_HNetworkWiFiGetUserAp( int nDevIndex, NetworkApHandle * identifier)
{

	if ( NULL == identifier )
	{
		return OOIF_RETURN_ERROR;
	}

	if ( NULL == s_userAp )
	{
		s_userAp = (OplNetworkAccessPoint *)HLIB_STD_MemAlloc(sizeof(OplNetworkAccessPoint));
		HxSTD_MemSet(s_userAp, 0x00, sizeof(OplNetworkAccessPoint));
	}

	*identifier = s_userAp;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkWiFiGetFWVersion( int nDevIndex, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szFWVersion[MAX_BUF_LEN];

	HxSTD_MemSet(szFWVersion,0,MAX_BUF_LEN);

    OPL_NetworkWiFi_GetFWVersion(nDevIndex,szFWVersion, MAX_BUF_LEN);


	*retval = szFWVersion;

	return OOIF_RETURN_OK;

}

OOIFReturnCode native_HNetworkAPScanningResultListener(NetworkEventCallback cbFunc)
{
	OPL_NetworkWiFi_RegisterListener( cbFunc );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPScanningResultListenerRelease(NetworkEventCallback cbFunc)
{
	OPL_NetworkWiFi_UnRegisterListener();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPGetEssid(void * identifier, const char **retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = apObj->szESSID;

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPSetEssid(void * identifier, const char *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	strncpy( apObj->szESSID, retval, sizeof(apObj->szESSID)-1 );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPGetSecureType(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = apObj->nSecureType;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPSetSecureType(void * identifier, OOIFNumber retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	apObj->nSecureType = OPL_NetworkWiFi_ConvertToSecurityInt(retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPGetAuthenType(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = apObj->nAuthenType;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPSetAuthenType(void * identifier, OOIFNumber retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	apObj->nAuthenType = OPL_NetworkWiFi_ConvertToAuthenInt(retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPGetEncryptType(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = apObj->nEncryptType;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPSetEncryptType(void * identifier, OOIFNumber retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	apObj->nEncryptType = OPL_NetworkWiFi_ConvertToEncryptInt(retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPGetWaveStrength(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = apObj->nWaveStrength;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPGetConnectSpeed(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = apObj->nConnectSpeed;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPGetIsConnected(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = apObj->bIsConnected;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAPGetMacAddr(void * identifier, const char **retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = apObj->szMacAddr;

	return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkAccessPointReleaseHandle(void * identifier)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	if ( apObj && apObj != s_userAp)
	{
		HLIB_STD_MemFree(apObj);
		apObj = NULL;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkManagerGetFTPServer(OOIFNumber *retval)
{
	int ret;
	OPL_Network_GetFTPStatus( &ret);
	OOIF_LOG_INFO("[%s:%d] FTP Server Status : %s\n", __PRETTY_FUNCTION__, __LINE__, ret?"Running":"NOT Running");
	*retval = ret;
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HNetworkManagerSetFTPServer(OOIFNumber bOn)
{
	OOIF_LOG_INFO("[%s:%d] FTP Server Setting : %s\n", __PRETTY_FUNCTION__, __LINE__, bOn?"On":"Off");
	OPL_Network_SetFTPStatus( bOn );

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_HNetworkDMSGetStatus(OOIFNumber *retval)
{
	HUINT32 status = 0;
	OPL_DMS_GetActionStatus(&status);
	*retval = (OOIFNumber)status;
	return OOIF_RETURN_OK;
}

OOIFReturnCode	native_HNetworkDMSSetAcionMode(OOIFNumber mode)
{
	if ( !!mode )
		OPL_DMS_Start();
	else
		OPL_DMS_Stop();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkPPPoEConfigRegisterEventHandler( NetworkEventCallback cbFunc )
{
	OPL_NetworkPPPoE_RegisterListener( cbFunc );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkPPPoEConfigUnRegisterEventHandler( void )
{
	OPL_NetworkPPPoE_UnRegisterListener();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkPPPoEConfigGetInfo(int nDevIndex, char **szId, char **szPassword)
{
	BPLStatus result = BPL_STATUS_ERROR;

	result = OPL_NetworkPPPoE_GetInfo(nDevIndex, szId, szPassword);
	if ( BPL_STATUS_OK != result )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkPPPoEConfigSetInfo(int nDevIndex, const char *szId, const char *szPassword)
{
	BPLStatus result = BPL_STATUS_ERROR;

	result = OPL_NetworkPPPoE_SetInfo(nDevIndex, szId, szPassword);
	if ( BPL_STATUS_OK != result )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkPPPoEConfigGetOnOff(int nDevIndex, OOIFBoolean *bOn)
{
	BPLStatus result = BPL_STATUS_ERROR;

	result = OPL_NetworkPPPoE_GetOnOff(nDevIndex, (BPL_BOOL*)bOn);
	if ( BPL_STATUS_OK != result )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkPPPoEConfigSetOnOff(int nDevIndex, OOIFBoolean bOn)
{
	BPLStatus result = BPL_STATUS_ERROR;

	result = OPL_NetworkPPPoE_SetOnOff(nDevIndex, (BPL_BOOL)bOn);
	if ( BPL_STATUS_OK != result )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkPPPoEConfigConnect(int nDevIndex, const char *szId, const char *szPassword, OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OPL_NetworkPPPoE_Connect(nDevIndex, szId, szPassword);
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkPPPoEConfigDisconnect(int nDevIndex, OOIFNumber *retval)
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OPL_NetworkPPPoE_Disconnect(nDevIndex);
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

#endif


OOIFReturnCode native_HLocalSystemGetNetworkInterfaces(int *count, HNetworkInterfaceHandle **handles)
{
	if (NULL == count)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*count = OplLocalSystem::getNetworkInterfacesCount();
	if ( *count <= 0 )
	{
		*count = 0;
		*handles = NULL;
		return OOIF_RETURN_OK;
	}

	HNetworkInterfaceHandle *myHandles = new HNetworkInterfaceHandle[*count];
	if ( NULL == myHandles )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int index;
	for ( index = 0; index < *count; index++ )
	{
		myHandles[index] = OplLocalSystem::getNetworkInterfaceByIndex(index);
	}

	*handles = myHandles;


	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HNetworkInterfaceReleaseHandle(HNetworkInterfaceHandle id)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(id);
	if ( networkinterfaceObj )
	{
		delete networkinterfaceObj;
		networkinterfaceObj = NULL;
	}

	return OOIF_RETURN_OK;
}
#endif // OIPF

#ifdef OIPF
// -------------------------------------
// AVOutput getters
OOIFReturnCode native_HAVOutputGetName(AVOutputHandle identifier, const char **retval)
{
	*retval = "AV 1";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputGetType(AVOutputHandle identifier, const char **retval)
{
	*retval = "video";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputGetEnabled(AVOutputHandle identifier, OOIFBoolean *retval)
{
	*retval = 1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputGetSubtitleEnabled(AVOutputHandle identifier, OOIFBoolean *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputGetVideoMode(AVOutputHandle identifier, const char **retval)
{
	*retval = "normal";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputGetDigitalAudioMode(AVOutputHandle identifier, const char **retval)
{
	*retval = "ac3";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputGetAudioRange(AVOutputHandle identifier, const char **retval)
{
	*retval = "wide";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputGetHDVideoFormat(AVOutputHandle identifier, const char **retval)
{
	*retval = "1080p";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputGetTVAspectRatio(AVOutputHandle identifier, const char **retval)
{
	*retval = "16:9";
	return OOIF_RETURN_OK;
}

// AVOutput setters
OOIFReturnCode native_HAVOutputSetEnabled(AVOutputHandle identifier, OOIFBoolean enabled)
{
	OOIF_LOG_DEBUG("Setting AVOutput enabled %d", enabled);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputSetSubtitleEnabled(AVOutputHandle identifier, OOIFBoolean enabled)
{
	OOIF_LOG_DEBUG("Setting AVOoutput subtitleEnabled %d", enabled);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputSetVideoMode(AVOutputHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting AVOutput videoMode %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputSetDigitalAudioMode(AVOutputHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting AVOutput digitalAudioMode %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputSetAudioRange(AVOutputHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting AVOutput audioRange %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputSetHDVideoFormat(AVOutputHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting AVOutput hdVideoFormat %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAVOutputSetTVAspectRatio(AVOutputHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting AVOutput tvAspectRatio %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemGetOutputs(int *count, AVOutputHandle **handles)
{
	AVOutputHandle *myHandles = (AVOutputHandle*)HLIB_STD_MemAlloc(sizeof(AVOutputHandle)*2);
	myHandles[0] = (void*)101;
	myHandles[1] = (void*)102;
	*handles = myHandles;
	*count = 2;
	return OOIF_RETURN_OK;
}

OOIFReturnCode	native_HLocalSystemGetTuners(int *count, TunerHandle **handles)
{
	unsigned int	nNumOfDev, idx;
	TunerHandle		*myHandles;

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

OOIFReturnCode native_HAVOutputReleaseHandle(AVOutputHandle id)
{
	return OOIF_RETURN_OK;
}

#if defined(HMX_WEBUI)

OOIFReturnCode native_HOutputReleaseHandle(HMX_OutputHandle id)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioReleaseHandle(HMX_AudioHandle id)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemGetHmxOutputs(int *count, HMX_OutputHandle **handles)
{
	unsigned int nNumOfDev,idx,startValue;
	HMX_OutputHandle *myHandles;

	OplHmxOutput::getNumOfDev(&nNumOfDev);

	OOIF_LOG_INFO("[%s:%d] nNumOfDev:%d\n",__PRETTY_FUNCTION__,__LINE__,nNumOfDev);

	myHandles = (HMX_OutputHandle*)HLIB_STD_MemAlloc(sizeof(HMX_OutputHandle)*nNumOfDev);
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

OOIFReturnCode native_HLocalSystemGetHmxAudios(int *count, HMX_AudioHandle **handles)
{
	unsigned int nNumOfDev,idx,startValue;
	HMX_OutputHandle *myHandles;

	OplHmxAudio::getNumOfDev(&nNumOfDev);

	OOIF_LOG_INFO("[%s:%d] nNumOfDev:%d\n",__PRETTY_FUNCTION__,__LINE__,nNumOfDev);

	myHandles = (HMX_AudioHandle*)HLIB_STD_MemAlloc(sizeof(HMX_AudioHandle)*nNumOfDev);
	startValue = 0;
	for (idx = 0; idx < nNumOfDev; idx++)
	{
		myHandles[idx] = (void*)(startValue+idx);
		OOIF_LOG_INFO("[%s:%d] myHandles[%d]:0x%X\n",__PRETTY_FUNCTION__,__LINE__,idx,(unsigned int)myHandles[idx]);
	}

	*handles = myHandles;
	*count = nNumOfDev;
	return OOIF_RETURN_OK;


	return OOIF_RETURN_OK;
}


#define ____HMX_Output_getter_functions____

OOIFReturnCode native_HOutputGetName(HMX_OutputHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szOutputName[(MAX_BUF_LEN+1)];

	HxSTD_MemSet(szOutputName,0,(MAX_BUF_LEN+1));

	snprintf(szOutputName,MAX_BUF_LEN,"Output %d",(unsigned int)identifier);

	*retval = szOutputName;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetType(HMX_OutputHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = "output";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = 1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetScartEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplHmxOutput::IsSupportScart((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetTvScartEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplHmxOutput::IsSupportTvScart((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetVcrScartEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplHmxOutput::IsSupportVcrScart((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetCompositeEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplHmxOutput::IsSupportComposite((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetComponentEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplHmxOutput::IsSupportComponent((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetRfEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplHmxOutput::IsSupportRF((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetHdmiEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplHmxOutput::IsSupportHdmi((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetSpdifEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplHmxOutput::IsSupportSpdif((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetSupportedTvScartFormats(HMX_OutputHandle identifier, int *count, char ***strings)
{
	if (NULL == strings)
		return OOIF_RETURN_VALUE_UNDEFINED;

	// static buffer used in getSupportedTvScartFormat() for strings.
	if ( !OplHmxOutput::getSupportedTvScartFormat((unsigned int)identifier, count, strings) )
	{
		OOIF_LOG_ERROR("[%s:%d] getSupportedVcrScartFormat ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetSupportedVcrScartFormats(HMX_OutputHandle identifier, int *count, char ***strings)
{
	if (NULL == strings)
		return OOIF_RETURN_VALUE_UNDEFINED;

	// static buffer used in getSupportedVcrScartFormat() for strings.
	if ( !OplHmxOutput::getSupportedVcrScartFormat((unsigned int)identifier, count, strings) )
	{
		OOIF_LOG_ERROR("[%s:%d] getSupportedVcrScartFormat ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetCurrentResolution(HMX_OutputHandle identifier, const char **retval)
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

OOIFReturnCode native_HOutputGetSupportedResolutions(HMX_OutputHandle identifier, int *count, char ***strings)
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

OOIFReturnCode native_HOutputGetTvScartFormat(HMX_OutputHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szTvScartFormat[MAX_BUF_LEN];

	HxSTD_MemSet(szTvScartFormat,0,MAX_BUF_LEN);

	if ( !OplHmxOutput::getCurTvScartFormat((unsigned int)identifier, szTvScartFormat) )
	{
		OOIF_LOG_ERROR("[%s:%d] getCurTvScartFormat ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] pszTvScartFormat:%s\n",__PRETTY_FUNCTION__,__LINE__,szTvScartFormat);

	*retval = szTvScartFormat;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetVcrScartFormat(HMX_OutputHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szVcrScartFormat[MAX_BUF_LEN];

	HxSTD_MemSet(szVcrScartFormat,0,MAX_BUF_LEN);

	if ( !OplHmxOutput::getCurVcrScartFormat((unsigned int)identifier, szVcrScartFormat) )
	{
		OOIF_LOG_ERROR("[%s:%d] getCurVcrScartFormat ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] VCR Scart Format:%s\n",__PRETTY_FUNCTION__,__LINE__,szVcrScartFormat);

	*retval = szVcrScartFormat;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetTVAspectRatio(HMX_OutputHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szTVAspectRatio[MAX_BUF_LEN];

	HxSTD_MemSet(szTVAspectRatio,0,MAX_BUF_LEN);

	if ( !OplHmxOutput::getTvAspectRatio((unsigned int)identifier, szTVAspectRatio) )
	{
		OOIF_LOG_ERROR("[%s:%d] getCurTvAspectRatio ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] TVAspectRatio:%s\n",__PRETTY_FUNCTION__,__LINE__,szTVAspectRatio);

	*retval = szTVAspectRatio;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetVideoDisplayFormat(HMX_OutputHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szVideoDisplayFormat[MAX_BUF_LEN];

	HxSTD_MemSet(szVideoDisplayFormat,0,MAX_BUF_LEN);

	if ( !OplHmxOutput::getDisplayFormat((unsigned int)identifier, szVideoDisplayFormat) )
	{
		OOIF_LOG_ERROR("[%s:%d] getDisplayFormat ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] Display Format:%s\n",__PRETTY_FUNCTION__,__LINE__,szVideoDisplayFormat);

	*retval = szVideoDisplayFormat;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetVideoStandard(HMX_OutputHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szVideoStandard[MAX_BUF_LEN];

	HxSTD_MemSet(szVideoStandard,0,MAX_BUF_LEN);

	if ( !OplHmxOutput::getVideoStandard((unsigned int)identifier, szVideoStandard) )
	{
		OOIF_LOG_ERROR("[%s:%d] getVideoStandard ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] VideoStandard:%s\n",__PRETTY_FUNCTION__,__LINE__,szVideoStandard);

	*retval = szVideoStandard;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetAvAddtionalSignal(HMX_OutputHandle identifier, const char **retval)
{

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szAvAddtionalSignal[MAX_BUF_LEN];

	HxSTD_MemSet(szAvAddtionalSignal,0,MAX_BUF_LEN);

	if ( !OplHmxOutput::getAvAddtionalSignal((unsigned int)identifier, szAvAddtionalSignal) )
	{
		OOIF_LOG_ERROR("[%s:%d] getAvAddtionalSignal ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] AvAddtionalSignal:%s\n",__PRETTY_FUNCTION__,__LINE__,szAvAddtionalSignal);

	*retval = szAvAddtionalSignal;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetWss2HdmiMode(HMX_OutputHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szWss2HdmiMode[MAX_BUF_LEN];

	HxSTD_MemSet(szWss2HdmiMode,0,MAX_BUF_LEN);

	if ( !OplHmxOutput::getWss2HdmiMode((unsigned int)identifier, szWss2HdmiMode) )
	{
		OOIF_LOG_ERROR("[%s:%d] getWss2HdmiMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] Wss2HdmiMode:%s\n",__PRETTY_FUNCTION__,__LINE__,szWss2HdmiMode);

	*retval = szWss2HdmiMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetAudioDescriptionEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	*retval = OplHmxOutput::IsAudioDescriptionEnabled((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetSubtitleEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	*retval = OplHmxOutput::IsSubtitleEnabled((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputGetHardOfHearingEnabled(HMX_OutputHandle identifier, OOIFBoolean *retval)
{
	*retval = OplHmxOutput::IsHardOfHearingEnabled((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

#define ____HMX_Output_setter_functions____

OOIFReturnCode native_HOutputSetEnabled(HMX_OutputHandle identifier, OOIFBoolean enabled)
{
	OOIF_LOG_DEBUG("Setting AVOutput enabled %d", enabled);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputSetTvScartFormat(HMX_OutputHandle identifier, const char *value)
{
	if ( !OplHmxOutput::setCurTvScartFormat((unsigned int)identifier, value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setCurTvScartFormat ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputSetVcrScartFormat(HMX_OutputHandle identifier, const char *value)
{
	if ( !OplHmxOutput::setCurVcrScartFormat((unsigned int)identifier, value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setCurVcrScartFormat ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputSetTVAspectRatio(HMX_OutputHandle identifier, const char *value)
{
	if ( !OplHmxOutput::setTvAspectRatio((unsigned int)identifier, value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setCurTvAspectRatio ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputSetVideoDisplayFormat(HMX_OutputHandle identifier, const char *value)
{
	if ( !OplHmxOutput::setDisplayFormat((unsigned int)identifier, value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setDisplayFormat ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputSetCurrentResolution(HMX_OutputHandle identifier, const char *value)
{
	if ( !OplHmxOutput::setCurResolution((unsigned int)identifier, value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setCurResolution ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputSetAudioDescriptionEnabled(HMX_OutputHandle identifier, OOIFBoolean enabled)
{
	if ( !OplHmxOutput::setAudioDescriptionEnabled((unsigned int)identifier, enabled) )
	{
		OOIF_LOG_ERROR("[%s:%d] setAudioDescriptionEnabled ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] enabled:%d\n",__PRETTY_FUNCTION__,__LINE__,enabled);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputSetSubtitleEnabled(HMX_OutputHandle identifier, OOIFBoolean enabled)
{
	if ( !OplHmxOutput::setSubtitleEnabled((unsigned int)identifier, enabled) )
	{
		OOIF_LOG_ERROR("[%s:%d] setSubtitleEnabled ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] enabled:%d\n",__PRETTY_FUNCTION__,__LINE__,enabled);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HOutputSetHardOfHearingEnabled(HMX_OutputHandle identifier, OOIFBoolean enabled)
{
	if ( !OplHmxOutput::setHardOfHearingEnabled((unsigned int)identifier, enabled) )
	{
		OOIF_LOG_ERROR("[%s:%d] setHardOfHearingEnabled ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] enabled:%d\n",__PRETTY_FUNCTION__,__LINE__,enabled);

	return OOIF_RETURN_OK;
}


#define ____HMX_Audio_setter_functions____


OOIFReturnCode native_HAudioGetName(HMX_AudioHandle identifier, const char **retval)
{
	unsigned int idx = (unsigned int)identifier;

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szAudioName[MAX_BUF_LEN];

	if (!OplHmxAudio::getName((unsigned int)idx, szAudioName))
	{
		OOIF_LOG_ERROR("[%s:%d] getName ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = szAudioName;
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HAudioGetType(HMX_AudioHandle identifier, const char **retval)
{
	*retval = "audio";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioGetEnabled(HMX_AudioHandle identifier, OOIFBoolean *retval)
{
	// TODO: we have to detect HDMI Audio connection.
	*retval = 1; // temporary....
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioGetMaxVolume(HMX_AudioHandle identifier, OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	int nMaxVolume;

	*retval = 0;

	if (!OplHmxAudio::getVolumeMax((unsigned int)identifier, &nMaxVolume))
	{
		OOIF_LOG_ERROR("[%s:%d] getVolumeMax ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = (OOIFNumber)nMaxVolume;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioGetMinVolume(HMX_AudioHandle identifier, OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	int nMinVolume;

	*retval = 0;

	if (!OplHmxAudio::getVolumeMin((unsigned int)identifier, &nMinVolume))
	{
		OOIF_LOG_ERROR("[%s:%d] getVolumeMin ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = (OOIFNumber)nMinVolume;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioGetLipSyncDelay(HMX_AudioHandle identifier, OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	int nLipSyncDelay;

	*retval = 0;

	if (!OplHmxAudio::getLipSyncDelay((unsigned int)identifier, &nLipSyncDelay))
	{
		OOIF_LOG_ERROR("[%s:%d] getLipSyncDelay ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = (OOIFNumber)nLipSyncDelay;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioGetSoundMode(HMX_AudioHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szSoundMode[MAX_BUF_LEN];

	HxSTD_MemSet(szSoundMode,0,MAX_BUF_LEN);

	if ( !OplHmxAudio::getSoundMode((unsigned int)identifier, szSoundMode) )
	{
		OOIF_LOG_ERROR("[%s:%d] getSoundMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] Sound Mode:%s\n",__PRETTY_FUNCTION__,__LINE__,szSoundMode);

	*retval = szSoundMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioGetTranscodingEnabled(HMX_AudioHandle identifier, OOIFBoolean *retval)
{
	*retval = OplHmxAudio::IsTranscodingEnabled((unsigned int)identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioGetDigitalOutputMode(HMX_AudioHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szDigitalOutputMode[MAX_BUF_LEN];

	HxSTD_MemSet(szDigitalOutputMode,0,MAX_BUF_LEN);

	if ( !OplHmxAudio::getDigitalOutputMode((unsigned int)identifier, szDigitalOutputMode) )
	{
		OOIF_LOG_ERROR("[%s:%d] getDigitalOutputMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] Digital Output Mode:%s\n",__PRETTY_FUNCTION__,__LINE__,szDigitalOutputMode);

	*retval = szDigitalOutputMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioGetDigitalOutputCodec(HMX_AudioHandle identifier, const char **retval)
{
	if ( (NULL == retval))
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szDigitalOutputCodec[MAX_BUF_LEN];

	HxSTD_MemSet(szDigitalOutputCodec,0,MAX_BUF_LEN);

	if ( !OplHmxAudio::getDigitalOutputCodec((unsigned int)identifier, szDigitalOutputCodec) )
	{
		OOIF_LOG_ERROR("[%s:%d] getDigitalOutputCodec ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] Digital Output Codec:%s\n",__PRETTY_FUNCTION__,__LINE__,szDigitalOutputCodec);

	*retval = szDigitalOutputCodec;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioGetTranscodingMode(HMX_AudioHandle identifier, const char **retval)
{
	if ( (NULL == retval))
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szTransCodingMode[MAX_BUF_LEN];

	HxSTD_MemSet(szTransCodingMode,0,MAX_BUF_LEN);

	if ( !OplHmxAudio::getTranscodingMode((unsigned int)identifier, szTransCodingMode) )
	{
		OOIF_LOG_ERROR("[%s:%d] getTranscodingMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] Transcoding Mode:%s\n",__PRETTY_FUNCTION__,__LINE__,szTransCodingMode);

	*retval = szTransCodingMode;

	return OOIF_RETURN_OK;
}

// TODO: memory leak°¡ ÀÇ½É µÊ.
// TODO: Still not implemented !!!
OOIFReturnCode native_HAudioGetSupportedSoundOutputModes(HMX_AudioHandle identifier, int *count, char ***strings)
{
	if ( (NULL == count) || (NULL == strings))
		return OOIF_RETURN_VALUE_UNDEFINED;

	if ( !OplHmxAudio::getSupportedSoundOutputModes((unsigned int)identifier, count, strings) )
	{
		OOIF_LOG_ERROR("[%s:%d] getSupportedSoundOutputModes ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

// TODO: memory leak°¡ ÀÇ½É µÊ.
// TODO: Still not implemented !!!
OOIFReturnCode native_HAudioGetSupportedDigitalOutputCodecs(HMX_AudioHandle identifier, int *count, char ***strings)
{
	if ( (NULL == count) || (NULL == strings))
		return OOIF_RETURN_VALUE_UNDEFINED;

	if ( !OplHmxAudio::getSupportedDigitalOutputCodecs((unsigned int)identifier, count, strings) )
	{
		OOIF_LOG_ERROR("[%s:%d] getSupportedDigitalOutputCodecs ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

// TODO: memory leak°¡ ÀÇ½É µÊ.
// TODO: Still not implemented !!!
OOIFReturnCode native_HAudioGetSupportedDigitalOutputModes(HMX_AudioHandle identifier, int *count, char ***strings)
{
	if ( (NULL == count) || (NULL == strings))
		return OOIF_RETURN_VALUE_UNDEFINED;

	if ( !OplHmxAudio::getSupportedDigitalOutputModes((unsigned int)identifier, count, strings) )
	{
		OOIF_LOG_ERROR("[%s:%d] getSupportedDigitalOutputModes ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

// TODO: memory leak°¡ ÀÇ½É µÊ.
// TODO: Still not implemented !!!
OOIFReturnCode native_HAudioGetSupportedTranscodingModes(HMX_AudioHandle identifier, int *count, char ***strings)
{
	if ( (NULL == count) || (NULL == strings))
		return OOIF_RETURN_VALUE_UNDEFINED;

	if ( !OplHmxAudio::getSupportedTranscodingModes((unsigned int)identifier, count, strings) )
	{
		OOIF_LOG_ERROR("[%s:%d] getSupportedTranscodingModes ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}
#define ____HMX_Audio_setter_functions____

OOIFReturnCode native_HAudioSetEnabled(HMX_AudioHandle identifier, OOIFBoolean enabled)
{
	OOIF_LOG_DEBUG("Setting HMX AUdio enabled %d", enabled);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioSetLipSyncDelay(HMX_AudioHandle identifier, OOIFNumber value)
{
	if (!OplHmxAudio::setLipSyncDelay((unsigned int)identifier, (int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setLipSyncDelay ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioSetSoundMode(HMX_AudioHandle identifier, const char *value)
{
	if (NULL == value)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplHmxAudio::setSoundMode((unsigned int)identifier, value))
	{
		OOIF_LOG_ERROR("[%s:%d] setSoundMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioSetDigitalOutputMode(HMX_AudioHandle identifier, const char *value)
{
	if (NULL == value)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplHmxAudio::setDigitalOutputMode((unsigned int)identifier, value))
	{
		OOIF_LOG_ERROR("[%s:%d] setDigitalOutputMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioSetDigitalOutputCodec(HMX_AudioHandle identifier, const char *value)
{
	if (NULL == value)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplHmxAudio::setDigitalOutputCodec((unsigned int)identifier, value))
	{
		OOIF_LOG_ERROR("[%s:%d] setDigitalOutputCodec ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HAudioSetTranscodingMode(HMX_AudioHandle identifier, const char *value)
{
	if (NULL == value)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplHmxAudio::setTranscodingMode((unsigned int)identifier, value))
	{
		OOIF_LOG_ERROR("[%s:%d] setTranscodingMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}


#define ____HMX_FrontPanel_functions____

OOIFReturnCode native_HFrontPanel_SetString(const char *panelString, int align, int dimlevel, OOIFBoolean bAuto)
{
	OPL_HFrontPanel_SetString(panelString, align, dimlevel, bAuto);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HFrontPanel_SetLedOnOff(int ledId, OOIFBoolean bOnOff, OOIFBoolean bAuto, OOIFBoolean bAllClear)
{
	OPL_HFrontPanel_SetLedOnOff(ledId, bOnOff, bAuto, bAllClear);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HFrontPanel_SetAnimation(int fpItemType, int param1, int param2, int param3)
{
	OPL_HFrontPanel_SetAnimation(fpItemType, param1, param2, param3);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HFrontPanel_SetCategoryInform(int nCategory)
{
	OPL_HFrontPanel_SetCategoryInform(nCategory);
	return OOIF_RETURN_OK;
}


#define ____HMX_OperaTVBrowser__functions___

OOIFReturnCode native_HLocalSystemStartTVBrowser(const char *url, OOIFBoolean *retval)
{
	if ( OplLocalSystem::startTVBrowser(url))
	{
		*retval = 1;
	}
	else
	{
		*retval = 0;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HLocalSystemGetStatusOfTVBrowser(OOIFBoolean *retval)
{
	if( OplLocalSystem::getStatusOfTVBrowser() )
	{
		*retval = 1;
	}
	else
	{
		*retval = 0;
	}

	return OOIF_RETURN_OK;
}


#endif // #if defined(HMX_WEBUI)

#endif // #ifdef OIPF
