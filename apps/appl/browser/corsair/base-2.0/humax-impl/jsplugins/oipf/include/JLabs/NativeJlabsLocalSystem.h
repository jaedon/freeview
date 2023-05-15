/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABSLOCALSYSTEM__
#define __NATIVE_JLABSLOCALSYSTEM__

#include "NativeOOIFTypes.h"

#include "jlabsNetworkInterface.h"
#include "jlabsNetworkService.h"
#include "jlabsPPVHistory.h"
#include "jlabsScanning.h"
#include "NativeDisc.h"
#include "opljlabslocalsystem.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define	SCAN_NETWORK_TYPE_LEN_MAX		20
#define	SCAN_SYMBOL_RATE_LEN_MAX		10

typedef enum
{
	SCAN_NETWORK_DT,
	SCAN_NETWORK_BS,
	SCAN_NETWORK_CATV,
	SCAN_NETWORK_JCHITS,
	SCAN_NETWORK_CS1,
	SCAN_NETWORK_CS2,
#if 1
	SCAN_NETWORK_ADVANCED_CATV,
	SCAN_NETWORK_ADVANCED_JCHITS,
#endif
	SCAN_NETWORK_MAX
}eNativeJlabsLocalSystem_NetworkType;

typedef struct
{
	// networkType
	unsigned char aucNetworkType[SCAN_NETWORK_TYPE_LEN_MAX];
	// channelScanOptions
	int			nChannelType;
	bool			bReplaceExisting;
	// channelScanParameter	/* DVBC¸¸ °í·Á */
	int			nStartFrequency;
	int			nEndFrequency;
	int 			nRaster;
	bool			bstartNoetworkScanOnNIT;
	int			nModulationModes;
	unsigned char aucSymbolRate[SCAN_SYMBOL_RATE_LEN_MAX];
	int			nNetworkId;
}stNativeJlabsLocalSystem_ScanSetting_t;


typedef enum{

	JLABS_LOCALSYSTEM_EVENT_FINISH_DISC_FORMAT = 0,
	JLABS_LOCALSYSTEM_EVENT_DISC_ATTACHED =1,
	JLABS_LOCALSYSTEM_EVENT_DISC_DETACHED = 2,
	JLABS_LOCALSYSTEM_EVENT_RESULT_PAIRING = 3,
	JLABS_LOCALSYSTEM_EVENT_RESIDENT_APP_FINISH = 4,
	JLABS_LOCALSYSTEM_EVENT_DISC_LOAD_FINISHED = 5,
	JLABS_LOCALSYSTEM_EVENT_INSERT_SETUP_FINISHED = 6,

}JlabsLocalsystemEvent;

typedef union {
	struct{
		HDiscInfoHandle		handle;
		int				state;
	}format_data;	// result of format

	struct{
		HDiscInfoHandle		handle;
	}attach_data;	// disc attach

	struct{
		char		*discId;
		char		*discName;
	}detach_data;	//disc detach

	struct{
		int		result;
	}result_pairing_data;	//result pairing

	struct{
		char		*appName;
	}resident_app_finish_data;	// ResidentAppFinish

	struct{
		HDiscInfoHandle		handle;
	}scan_data;	// disc scan finish
	struct{
		OOIFNumber	errorNumber;
	}insertResult_data;
}JlabsLocalsystemEventInfo;


typedef void* JlabsLocalSystemHandle;
typedef void (jlabsLocalSystemListener)(JlabsLocalsystemEvent event, JlabsLocalsystemEventInfo* info);

OOIFReturnCode native_jlabsLocalSystemSetListener(jlabsLocalSystemListener listener);
OOIFReturnCode native_jlabsLocalSystemGetStorages(int *count, HDiscInfoHandle **handles);
OOIFReturnCode native_jlabsLocalSystemFormatDisc(HDiscInfoHandle handle, OOIFBoolean *retval);
OOIFReturnCode native_jlabsLocalSystemDetachDisc(HDiscInfoHandle handle, OOIFBoolean force, OOIFBoolean *retval);
OOIFReturnCode native_jlabsLocalSystemGetDefaultRecDisc(const char **retval);
OOIFReturnCode native_jlabsLocalSystemGetDefaultFactoryForDap(OOIFNumber *retval);
OOIFReturnCode native_jlabsLocalSystemGetDefaultDiscForDap(const char **retval);
OOIFReturnCode native_jlabsLocalSystemSetDefaultRecDisc(const char *value);
OOIFReturnCode native_jlabsLocalSystemSetDefaultFactoryForDap(OOIFNumber value);
OOIFReturnCode native_jlabsLocalSystemSetDefaultDiscForDap(const char *value);

OOIFReturnCode native_jlabsLocalSystemGetPPVHistory(int *count, jlabsPPVHistoryHandle **handles);
OOIFReturnCode native_jlabsLocalSystemGetScanComponent(int *count, jlabsScanningHandle **handles);
OOIFReturnCode native_jlabsLocalSystemGetNetworkInterfaces(int *count, jlabsNetworkInterfaceHandle **handles);
OOIFReturnCode native_jlabsLocalSystemGetNetworkService(int *count, jlabsNetworkServiceHandle **handles);
OOIFReturnCode native_jlabsLocalSystemGetWiFiAccessPoint(int *count, jlabsNetworkServiceHandle **handles);
OOIFReturnCode native_jlabsLocalSystemSetFactoryReset(OOIFNumber *retval);
OOIFReturnCode native_jlabsLocalSystemResetUserInfo(OOIFNumber *retval);
OOIFReturnCode native_jlabsLocalSystemStartPairing(OOIFBoolean *retval);
OOIFReturnCode native_jlabsLocalSystemAbortPairing(OOIFBoolean *retval);
OOIFReturnCode native_jlabsLocalSystemStartResidentApp(const char *appName, const char *exInfo, OOIFBoolean *retval);
OOIFReturnCode native_jlabsLocalSystemResidentAppFinish(const char *appName, OOIFBoolean *retval);
OOIFReturnCode native_jlabsLocalSystemGetSoftwareDownloadState(OOIFNumber *retval);
OOIFReturnCode native_jlabsLocalSystemInsertSetupInfo(OOIFNumber *retval);
OOIFReturnCode native_jlabsLocalSystemLoginApp(const char *userId, OOIFNumber permissionLevel, OOIFBoolean *retval);
OOIFReturnCode native_jlabsLocalSystemLogoffApp(OOIFBoolean *retval);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABSLOCALSYSTEM__ */
