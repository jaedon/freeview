/**************************************************************************************/
/**
 * @file NativeJlabsLocalSystem.cpp
 *
 * jlabsDTTChanne module
 *
 * @author
 * @date
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "NativeJlabsLocalSystem.h"
#include "macros.h"

#include "opllocalsystem.h"
#include "oplppvhistory.h"
#include "oplnetwork.h"
#include "oplfavourite.h"
#include "ChannelScanOptions.h"
#include "humax/ChannelScanParameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)

// TODO: 나중에 setup 값에 저장된 것을 read해서 채워질 수도 있음.
static stNativeJlabsLocalSystem_ScanSetting_t s_aucScanSetting[SCAN_NETWORK_MAX] =
{
	{"DTT", 				0, 	true, 	90000, 	77000, 	6000, 	false, 	16, 	"5274000", 	0x7000}
	,{"BS", 				0, 	true, 	90000, 	77000, 	6000, 	false, 	16, 	"5274000", 	0x0004}
	,{"CATV", 			0, 	true, 	90000, 	77000, 	6000, 	false, 	16, 	"5274000", 	0xFFFE}
	,{"JC-HITS", 			0, 	true, 	90000, 	77000, 	6000, 	false, 	16, 	"5274000", 	0xFFFD}
	,{"CS1", 				0, 	true, 	90000, 	77000, 	6000, 	false, 	16, 	"5274000", 	0x0006}
	,{"CS2", 				0, 	true, 	90000, 	77000, 	6000, 	false, 	16, 	"5274000", 	0x0007}
#if 1
	,{"Advanced_CATV", 	0, 	true, 	90000, 	77000, 	6000, 	false, 	16, 	"5274000", 	0xFFFA}
	,{"Advanced_JC-HITS", 	0, 	true, 	90000, 	77000, 	6000, 	false, 	16, 	"5274000", 	0xFFF9}
#endif
};

jlabsLocalSystemListener *global_JlabsLocalSystemListener = 0;

static void _jlabsLocalSystem_EmitEvent (JlabsLocalsystemEvent event, JlabsLocalsystemEventInfo *info);
static void _jlabsLocalSystem_onEvent(eOplJlabsLocalsystemEventType type, void *param1, void *param2, void *param3);


OOIFReturnCode native_jlabsLocalSystemGetPPVHistory(int *count, jlabsPPVHistoryHandle **handles)
{
	OPL_PPVHistoryHandle *myHandles = NULL;
	HINT32 numOfHandle = 0;
	OOIF_LOG_DUMMY("native_LocalSystemGetPPVHistory");
	numOfHandle = OPL_PPVHistory_GetPPVHistoryList(&myHandles);
	if(numOfHandle != 0 && myHandles != NULL)
	{
		*handles = myHandles;
		*count = numOfHandle;
	}
	else
	{
		*handles = NULL;
		*count = 0;
		if(myHandles!= NULL)
		{
			HLIB_STD_MemFree(myHandles);
		}
	}
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsLocalSystemGetScanComponent(int *count, jlabsScanningHandle **handles)
{
	jlabsScanningHandle *pHandles;
	stNativeJlabsLocalSystem_ScanSetting_t 	*pstSetting;
	int	i;

	OOIF_LOG_DUMMY("native_jlabsLocalSystemGetNetworkService");

	if ( NULL == count)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	pHandles = (jlabsScanningHandle*)HLIB_STD_MemAlloc(sizeof(jlabsScanningHandle)*SCAN_NETWORK_MAX);
	if ( NULL == pHandles )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	pstSetting = (stNativeJlabsLocalSystem_ScanSetting_t*)HLIB_STD_MemAlloc(sizeof(stNativeJlabsLocalSystem_ScanSetting_t)*SCAN_NETWORK_MAX);
	if ( NULL == pstSetting )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	memcpy(pstSetting, &s_aucScanSetting, (sizeof(stNativeJlabsLocalSystem_ScanSetting_t) *SCAN_NETWORK_MAX));
	for(i=0;i<SCAN_NETWORK_MAX;i++)
	{
		pHandles[i] = (jlabsScanningHandle*)&pstSetting[i];
	}

	*count = SCAN_NETWORK_MAX;
	*handles = pHandles;
	return OOIF_RETURN_OK;
}

#define JCOM_NETDEV_MAX 5
#define H_HOME_IDX	0  // jcom index : 0
#define H_WAN_IDX	1  // jcom index : 1
#define H_BCM_IDX	2  // jcom index : 4
#define H_WIFI_IDX 	3  // jcom index : 2
#define H_CM_IDX	4  // jcom index : 3
static int JCOM_DEFINE_INDEX[JCOM_NETDEV_MAX] = { H_HOME_IDX, H_WAN_IDX, H_WIFI_IDX, H_CM_IDX, H_BCM_IDX };
OOIFReturnCode native_jlabsLocalSystemGetNetworkInterfaces(int *count, jlabsNetworkInterfaceHandle **handles)
{
	if ( NULL == count)
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

	jlabsNetworkInterfaceHandle *myHandles = new jlabsNetworkInterfaceHandle[*count];
	if ( NULL == myHandles )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	int index;
	for ( index = 0; (index < *count && index < JCOM_NETDEV_MAX); index++ )
	{
#if 1 // jlabsNetworkInterface Index, 0 : home, 1 : wan, 2: wifi, 3, cableModem, 4, cableMOdem(hidden)
		myHandles[index] = OplLocalSystem::getNetworkInterfaceByIndex( JCOM_DEFINE_INDEX[index] );
#else
		myHandles[index] = OplLocalSystem::getNetworkInterfaceByIndex(index);
#endif
	}

	*handles = myHandles;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemGetNetworkService(int *count, jlabsNetworkServiceHandle **handles)
{
	int i = 0;
	if ( NULL == count)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*count = OplLocalSystem::getNetworkServiceCount();
	if ( *count <= 0 )
	{
		*count = 0;
		*handles = NULL;
		return OOIF_RETURN_OK;
	}

	jlabsNetworkServiceHandle *myHandles = new jlabsNetworkServiceHandle[*count];
	if ( NULL == myHandles )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	for ( i = 0; i < *count; i++ )
	{
		myHandles[i] = OplLocalSystem::getNetworkService(i);
	}

	*handles = myHandles;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemGetWiFiAccessPoint(int *count, jlabsWiFiAccessPointHandle **handles)
{
	int i = 0;
	if ( NULL == count)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*count = OplLocalSystem::getWiFiAccessPointCount();
	if ( *count <= 0 )
	{
		*count = 0;
		*handles = NULL;
		return OOIF_RETURN_OK;
	}

	jlabsWiFiAccessPointHandle *myHandles = new jlabsWiFiAccessPointHandle[*count];
	if ( NULL == myHandles )
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	for ( i = 0; i < *count; i++ )
	{
		myHandles[i] = (void *)OplLocalSystem::getWiFiAccessPoint(i);
	}

	*handles = myHandles;
	return OOIF_RETURN_OK;
}
static void _jlabsLocalSystem_EmitEvent (JlabsLocalsystemEvent event, JlabsLocalsystemEventInfo *info)
{
	if (global_JlabsLocalSystemListener)
	{
		global_JlabsLocalSystemListener(event, info);
	}
}

static void _jlabsLocalSystem_onEvent(eOplJlabsLocalsystemEventType type, void *param1, void *param2, void *param3)
{
	JlabsLocalsystemEventInfo info;

	switch ( type )
	{
		case OPL_JLABS_LOCALSYSTEM_EVENT_STORAGE_PROGRESS:
			// Format 상태를 가져와서 올려준다.
			OOIF_LOG_DEBUG("[OPL_JLABS_LOCALSYSTEM_EVENT_STORAGE_PROGRESS] state : %d\n", info.format_data.state);
			info.format_data.handle = (HDiscInfoHandle)param1;
			info.format_data.state  = (int)param2;
			_jlabsLocalSystem_EmitEvent(JLABS_LOCALSYSTEM_EVENT_FINISH_DISC_FORMAT, &info);
			break;

		case OPL_JLABS_LOCALSYSTEM_EVENT_DISC_ATTACHED:
			// 연결된 DiscInfo를 가져와서 올려준다.
			OOIF_LOG_DEBUG("[OPL_JLABS_LOCALSYSTEM_EVENT_DISC_ATTACHED] \n");
			info.attach_data.handle = (HDiscInfoHandle)param1;
			_jlabsLocalSystem_EmitEvent(JLABS_LOCALSYSTEM_EVENT_DISC_ATTACHED, &info);
			break;

		case OPL_JLABS_LOCALSYSTEM_EVENT_DISC_DETACHED:
			// 해제된 DiscInfo를 가져와서 올려준다.
			OOIF_LOG_DEBUG("[OPL_JLABS_LOCALSYSTEM_EVENT_DISC_DETACHED] \n");
			info.detach_data.discId = HLIB_STD_StrDup((char *)param1);
			info.detach_data.discName = HLIB_STD_StrDup((char *)param2);
			_jlabsLocalSystem_EmitEvent(JLABS_LOCALSYSTEM_EVENT_DISC_DETACHED, &info);
			HLIB_STD_MemFree(param1);
			HLIB_STD_MemFree(param2);
			break;

		case OPL_JLABS_LOCALSYSTEM_EVENT_RESULT_PAIRING:
			// 해제된 DiscInfo를 가져와서 올려준다.
			OOIF_LOG_DEBUG("[OPL_JLABS_LOCALSYSTEM_EVENT_RESULT_PAIRING] \n");
			info.result_pairing_data.result = (int) param1;
			_jlabsLocalSystem_EmitEvent(JLABS_LOCALSYSTEM_EVENT_RESULT_PAIRING, &info);
			break;

		case OPL_JLABS_LOCALSYSTEM_EVENT_RESIDENT_APP_FINISH:
			// 해제된 DiscInfo를 가져와서 올려준다.
			OOIF_LOG_DEBUG("[OPL_JLABS_LOCALSYSTEM_EVENT_RESIDENT_APP_FINISH] \n");
			info.resident_app_finish_data.appName = (char *)param1;
			_jlabsLocalSystem_EmitEvent(JLABS_LOCALSYSTEM_EVENT_RESIDENT_APP_FINISH, &info);
			HLIB_STD_MemFree(param1);
			break;

		case OPL_JLABS_LOCALSYSTEM_EVENT_DISC_LOAD_FINISHED:
			// 연결된 DiscInfo를 가져와서 올려준다.
			OOIF_LOG_DEBUG("[OPL_JLABS_LOCALSYSTEM_EVENT_DISC_SCAN_FINISH] \n");
			info.scan_data.handle = (HDiscInfoHandle)param1;
			_jlabsLocalSystem_EmitEvent(JLABS_LOCALSYSTEM_EVENT_DISC_LOAD_FINISHED, &info);
			break;
		case OPL_JLABS_LOCALSYSTEM_EVENT_INSERT_SETUPINFO_FINISHED:
			// insertSetupInfo결과를 올려준다.
			OOIF_LOG_DEBUG("[OPL_JLABS_LOCALSYSTEM_EVENT_INSERT_SETUPINFO_FINISHED] \n");
			info.insertResult_data.errorNumber = static_cast<OOIFNumber>((int)param1);
			_jlabsLocalSystem_EmitEvent(JLABS_LOCALSYSTEM_EVENT_INSERT_SETUP_FINISHED, &info);
			break;
		default:
			break;
	}
}

OOIFReturnCode native_jlabsLocalSystemSetListener(jlabsLocalSystemListener listener)
{
	global_JlabsLocalSystemListener = listener;

	OplJlabsLocalSystem::setEventListener(_jlabsLocalSystem_onEvent);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemGetStorages(int *count, HDiscInfoHandle **handles)
{
	HDiscInfoHandle *array = NULL;
	HUINT32 	cnt=0, i;
	OplDiscInfo *Info = NULL;

	OPL_Discinfo_GetDiscInfoList(&Info, &cnt, eDiscInfo_All);

	array = new HDiscInfoHandle[cnt];

	for(i = 0 ; i < cnt ; i++)
	{
		array[i] = (HDiscInfoHandle)OPL_DiscInfo_Clone(&Info[i]);
	}

	*count = cnt;
	*handles = array;
	array = NULL;

	delete []Info;
	return OOIF_RETURN_OK;

}

OOIFReturnCode native_jlabsLocalSystemGetDefaultRecDisc(const char **retval)
{
	OplDiscInfo	Info;
	static HCHAR szDiscId[eMS_MAX_UUID_LEN];

	if (OPL_Discinfo_GetDefaultDiscInfo(&Info) != BPL_STATUS_OK)
	{
		return OOIF_RETURN_ERROR;
	}

	HxSTD_MemSet(szDiscId, 0, eMS_MAX_UUID_LEN);
	if (Info.DiscId != NULL)
	{
		HxSTD_MemCopy(szDiscId, Info.DiscId, eMS_MAX_UUID_LEN);
	}
	*retval = szDiscId;

	HLIB_STD_MemFree(Info.Name);
	HLIB_STD_MemFree(Info.DiscId);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemGetDefaultFactoryForDap(OOIFNumber *retval)
{
	HUINT32 nMode = 0;

	if (OPL_JlabsLocalSystem_GetDefaultFactoryForDap(&nMode) != BPL_STATUS_OK)
	{
		return OOIF_RETURN_ERROR;
	}

	*retval = nMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemGetDefaultDiscForDap(const char **retval)
{
	static HCHAR szDiscId[eMS_MAX_UUID_LEN];

	if (OPL_JlabsLocalSystem_GetDefaultDiscForDap(szDiscId, eMS_MAX_UUID_LEN) != BPL_STATUS_OK)
	{
		return OOIF_RETURN_ERROR;
	}
	*retval = szDiscId;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemSetDefaultRecDisc(const char *value)
{
	OplJlabsLocalSystem::setDefaultStorage(value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemSetDefaultFactoryForDap(OOIFNumber value)
{
	OPL_JlabsLocalSystem_SetDefaultFactoryForDap(value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemSetDefaultDiscForDap(const char *value)
{
	if (value != NULL)
	{
		OPL_JlabsLocalSystem_SetDefaultDiscForDap((HCHAR *)value);
	}
	return OOIF_RETURN_OK;
}


// Format + PVR Hdd 등록
OOIFReturnCode native_jlabsLocalSystemFormatDisc(HDiscInfoHandle handle, OOIFBoolean *retval)
{
	OplDiscInfo	*Info = (OplDiscInfo *)handle;

	if(OplJlabsLocalSystem::setStoragePairing(Info->DiscId, Info->Name))
	{
		*retval = 1;
	}
	else
	{
		*retval = 0;
	}
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsLocalSystemDetachDisc(HDiscInfoHandle handle, OOIFBoolean force, OOIFBoolean *retval)
{
	OplDiscInfo	*Info = (OplDiscInfo *)handle;

	if(OplJlabsLocalSystem::DetachDisc(Info->DiscId, force))
	{
		*retval = 1;
	}
	else
	{
		*retval = 0;
	}
	return OOIF_RETURN_OK;
}



OOIFReturnCode native_jlabsLocalSystemSetFactoryReset(OOIFNumber *retval)
{
	OplLocalSystem::doFactoryDefault("all");
	*retval = 1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemResetUserInfo(OOIFNumber *retval)
{
	int 				count = 0 , i =0;
	OPL_Favourite_t 	*handles = NULL;

	count = (int)OPL_Favourite_GetCollection(&handles);
	if (count > 0)
	{
		for( i =0; i<count; i++)
		{
			OPL_Favourite_Remove( handles[i] );
		}
	}
	if( handles )
	{
		HLIB_STD_MemFree( handles );
	}

	OplJlabsLocalSystem::resetUserInfo();
	*retval = 0; // 무조건 성공 , 실패 케이스가 없다.

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemStartPairing(OOIFBoolean *retval)
{
	if(OplJlabsLocalSystem::startPairing())
	{
		*retval = 1;
	}
	else
	{
		*retval = 0;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemAbortPairing(OOIFBoolean *retval)
{
	if(OplJlabsLocalSystem::abortPairing())
	{
		*retval = 1;
	}
	else
	{
		*retval = 0;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemStartResidentApp(const char *appName, const char *exInfo, OOIFBoolean *retval)
{
	if(OplJlabsLocalSystem::startResidentApp(appName, exInfo))
	{
		*retval = 1;
	}
	else
	{
		*retval = 0;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemResidentAppFinish(const char *appName, OOIFBoolean *retval)
{
	if(OplJlabsLocalSystem::ResidentAppFinish(appName))
	{
		*retval = 1;
	}
	else
	{
		*retval = 0;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemGetSoftwareDownloadState(OOIFNumber *retval)
{
	HUINT32 state;

	OOIF_LOG_DUMMY("native_jlabsLocalSystemGetSoftwareDownloadState");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getSoftwareDownloadState(&state))
	{
		OOIF_LOG_ERROR("[%s:%d] getSoftwareDownloadState ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	if((state != 0) && (state != 1) && (state != 2))
		state = 0;

	OOIF_LOG_INFO("[%s:%d] value:%d\n", __PRETTY_FUNCTION__, __LINE__, state);

	*retval = state;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemInsertSetupInfo(OOIFNumber *retval)
{
	HINT32 nInsertResult = -1;
	if(!OplJlabsLocalSystem::insertSetupInfo(&nInsertResult))
	{
		*retval = -1;
	}
	else
	{
		*retval = (OOIFNumber)nInsertResult;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemLoginApp(const char *userId, OOIFNumber permissionLevel, OOIFBoolean *retval)
{
	/*dummy */
	OOIF_LOG_DUMMY("native_jlabsLocalSystemLoginApp not implemented yet\n");

	if((userId == NULL) || (retval == NULL))
		return OOIF_RETURN_VALUE_UNDEFINED;
	*retval = FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsLocalSystemLogoffApp(OOIFBoolean *retval)
{
	/*dummy */
	OOIF_LOG_DUMMY("native_jlabsLocalSystemLogoffApp not implemented yet\n");
	if(retval == NULL)
		return OOIF_RETURN_VALUE_UNDEFINED;
	*retval = FALSE;
	return OOIF_RETURN_OK;
}

