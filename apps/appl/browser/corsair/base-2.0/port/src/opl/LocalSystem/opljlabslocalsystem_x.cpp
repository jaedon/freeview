/**************************************************************************************/
/**
 * @file opljlabslocalsystem_x.cpp
 *
 * System information
 *
 * @author  Jung Youn Kim(jykim2@humaxdigital.com)
 * @date    2012/07/28
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "opljlabslocalsystem.h"
#include "oplchannel.h"

#include "oapi.h"
#include "hapi.h"
#include <apk.h>
#include <stdlib.h>
#include <string.h>
#include <hlib.h>
#include <errno.h>
#include <unistd.h>

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Typedef
******************************************************************/

/******************************************************************
	Static function prototypes
******************************************************************/

#if  1 // DEBUG_STORAGE_UTIL
struct TTagList
{
	const HCHAR		*tag;
	HUINT32		value;
};
const HCHAR* NullStr_t = "";

const TTagList		_sqc_ms_interface_table[] =
{
	{	"interface_sata",				eMS_DEV_INTERFACE_SATA		},
	{	"interface_usb", 				eMS_DEV_INTERFACE_USB			},
	{	"interface_sd", 				eMS_DEV_INTERFACE_SD			},
	{	NullStr_t,							eMS_DEV_INTERFACE_Unknown		}
};

const TTagList		_sqc_ms_type_table[] =
{
	{	"dev_type_int",					eMS_DEV_TYPE_Internal			},
	{	"dev_type_ext",					eMS_DEV_TYPE_External			},
	{	NullStr_t,							eMS_DEV_TYPE_Unknown			}
};

const TTagList		_sqc_ms_fstype_table[] =
{
	{	"fs_ext2",						eMS_FS_EXT2				},
	{	"fs_ext3",						eMS_FS_EXT3				},
	{	"fs_xfs",						eMS_FS_XFS					},
	{	"fs_fat16",						eMS_FS_FAT16				},
	{	"fs_fat32",						eMS_FS_FAT32				},
	{	"fs_ntfs",						eMS_FS_NTFS				},
	{	"fs_dos",						eMS_FS_DOS					},
	{	"fs_other",						eMS_FS_OTHER				},
	{	NullStr_t,							eMS_FS_NONE				}
};

const TTagList		_sqc_ms_kind_table[] =
{
	{	"dev_kind_hdd",					eMS_DEV_KIND_HDD				},
	{	"dev_kind_usbmem",				eMS_DEV_KIND_USBMEM				},
	{	"dev_kind_sdmem",				eMS_DEV_KIND_SDMEM				},
	{	NullStr_t,							eMS_DEV_KIND_Unknown			}
};

const TTagList		_sqc_ms_usage_table[] =
{
	{	"dev_usage_pvr",				eMS_DEV_USAGE_PVR				},
	{	"dev_usage_subpvr",			eMS_DEV_USAGE_SUB_PVR		},
	{	"dev_usage_storage",			eMS_DEV_USAGE_STORAGE			},
	{	NullStr_t,							eMS_DEV_USAGE_Unknown			}
};

const TTagList		_sqc_ms_status_physical_table[] =
{
	{	"dev_status_malfunction",		eMS_DEV_STATUS_Malfunction		},
	{	"dev_status_plugin",			eMS_DEV_STATUS_PlugIn			},
	{	"dev_status_plugout",			eMS_DEV_STATUS_PlugOut			},
	{	"dev_status_overcurrent",		eMS_DEV_STATUS_Overcurrent		},
	{	NullStr_t,							eMS_DEV_STATUS_PhysicalUnknown	}
};

const TTagList		_sqc_ms_status_mount_table[] =
{
	{	"dev_status_mounted",			eMS_DEV_STATUS_Mounted			},
	{	"dev_status_unmounted",			eMS_DEV_STATUS_Unmounted		},
	{	NullStr_t,							eMS_DEV_STATUS_MountUnknown		}
};

const TTagList		_sqc_ms_status_format_table[] =
{
	{	"dev_status_needformat",		eMS_DEV_STATUS_NeedFormat		},
	{	"dev_status_formatting",		eMS_DEV_STATUS_Formatting		},
	{	"dev_status_formatdone",		eMS_DEV_STATUS_FormatDone		},
	{	"dev_status_formatfail",		eMS_DEV_STATUS_FormatFail		},
	{	NullStr_t,							eMS_DEV_STATUS_FormatUnknown	}
};

const TTagList		_sqc_ms_status_pairing_table[] =
{
	{	"dev_status_pairing_ok",		eMS_DEV_STATUS_PairingOk			},
	{	"dev_status_pairing_fail",		eMS_DEV_STATUS_PairingFail		},
	{	"dev_status_pairing_noinfo",	eMS_DEV_STATUS_PairingNoInfo		},
	{	NullStr_t,							eMS_DEV_STATUS_PairingUnknown	}
};


/******************************************************************
	Static variables
******************************************************************/
HUINT32 OplJlabsLocalSystem::mCountDeviceInfoList = 0;
APKS_MS_DEVINFO_t* OplJlabsLocalSystem::mDeviceInfoList = NULL;
HCHAR  *OplJlabsLocalSystem::mDetachDiscId = NULL;

static OplJlabsLocalListener_t	s_pfnOplJLSEventListener = NULL;
static OPL_BOOL		s_bIsRegisterJLSEventListener = OPL_FALSE;
static OPL_BOOL		s_bIsFullBrowserStarted = OPL_FALSE;

static const HCHAR* getEnumString(const TTagList* aTagList, HUINT32 aValue)
{
	const TTagList* tagList = aTagList;
	do {
		if (tagList->value == aValue) {
			return tagList->tag;
		}
		tagList++;
	} while (tagList->tag != NullStr_t);
	return NullStr_t;
}
#endif // DEBUG_STORAGE_UTIL

static bool compareDeviceInfo(const APKS_MS_DEVINFO_t& aDevInfo1, const APKS_MS_DEVINFO_t& aDevInfo2)
{
	if (aDevInfo1.ulDeviceId != aDevInfo2.ulDeviceId) return false;
	//if (aDevInfo1.szUUID != aDevInfo2.szUUID) return false;
	return true;
}

static bool hasDeviceInfo(const APKS_MS_DEVINFO_t* aList, const HUINT32& aListCount, const APKS_MS_DEVINFO_t& aDevInfo)
{
	for (HUINT32 i=0; i<aListCount; i++) {
		const APKS_MS_DEVINFO_t& devInfo(aList[i]);
		if (compareDeviceInfo(devInfo, aDevInfo)) {
			return true;
		}
	}
	return false;
}

void OplJlabsLocalSystem::printDeviceInfo()
{
	if (mCountDeviceInfoList) {
		for (HUINT32 i=0; i<mCountDeviceInfoList; i++) {
			const APKS_MS_DEVINFO_t& devInfo(mDeviceInfoList[i]);
			HxLOG_Print("  ulDeviceId: 0x%08x\n", devInfo.ulDeviceId);
			HxLOG_Print("    eDevInterface: %s\n", getEnumString(_sqc_ms_interface_table ,devInfo.eDevInterface));
			HxLOG_Print("    eDevType: %s\n", getEnumString(_sqc_ms_type_table, devInfo.eDevType));
			HxLOG_Print("    eDevKind: %s\n", getEnumString(_sqc_ms_kind_table, devInfo.eDevKind));
			HxLOG_Print("    eDevUsage: %s\n", getEnumString(_sqc_ms_usage_table, devInfo.eDevUsage));
			HxLOG_Print("    eFsType: %s\n", getEnumString(_sqc_ms_fstype_table, devInfo.eFsType));
			HxLOG_Print("    stStatus\n");
			HxLOG_Print("      ePhysical: %s\n", getEnumString(_sqc_ms_status_physical_table, devInfo.stStatus.ePhysical));
			HxLOG_Print("      eMount: %s\n", getEnumString(_sqc_ms_status_mount_table, devInfo.stStatus.eMount));
			HxLOG_Print("      eFormat: %s\n", getEnumString(_sqc_ms_status_format_table, devInfo.stStatus.eFormat));
			HxLOG_Print("      stProgress\n");
			HxLOG_Print("        ulParam1: 0x%08x\n", devInfo.stStatus.stProgress.ulParam1);
			HxLOG_Print("        ulParam2: 0x%08x\n", devInfo.stStatus.stProgress.ulParam2);
			HxLOG_Print("        ulParam3: 0x%08x\n", devInfo.stStatus.stProgress.ulParam3);
			HxLOG_Print("      ePairing: %s\n", getEnumString(_sqc_ms_status_pairing_table, devInfo.stStatus.ePairing));
			HxLOG_Print("    ulTotalSizeKb: %d\n", devInfo.ulTotalSizeKb);
			HxLOG_Print("    ulTotalSizeGb: %d\n", devInfo.ulTotalSizeGb);
			HxLOG_Print("    szLabel: %s\n", devInfo.szLabel);
			HxLOG_Print("    szUUID: %s\n", devInfo.szUUID);
			HxLOG_Print("    szDevName: %s\n", devInfo.szDevName);
			HxLOG_Print("    ulPartitionNum: %d\n", devInfo.ulPartitionNum);
			for (HUINT32 j=0; j<devInfo.ulPartitionNum; j++) {
				const APKS_MS_PARTITION_t& partition(devInfo.astPartitionList[j]);
				HxLOG_Print("    ulPartitionId: %d\n", partition.ulPartitionId);
				HxLOG_Print("      bAvailable: %d\n", partition.bAvailable);
				HxLOG_Print("      eFsType: %s\n", getEnumString(_sqc_ms_fstype_table, partition.eFsType));
				HxLOG_Print("      ulStartUnit: %d\n", partition.ulStartUnit);
				HxLOG_Print("      ulEndUnit: %d\n", partition.ulEndUnit);
				HxLOG_Print("      bMounted: %d\n", partition.bMounted);
				HxLOG_Print("      ulPvrIdx: %d\n", partition.ulPvrIdx);
				HxLOG_Print("      bPvrFlag: %d\n", partition.bPvrFlag);
				HxLOG_Print("      bDefaultPvr: %d\n", partition.bDefaultPvr);
				HxLOG_Print("      bWritable: %d\n", partition.bWritable);
				HxLOG_Print("      ulDriveNo: %d\n", partition.ulDriveNo);
				HxLOG_Print("      ulTotalSizeKb: %d\n", partition.ulTotalSizeKb);
				HxLOG_Print("      ulTotalSizeGb: %d\n", partition.ulTotalSizeGb);
				HxLOG_Print("      szMountPath: %s\n", partition.szMountPath);
				HxLOG_Print("      szVolumeName: %s\n", partition.szVolumeName);
			}
		}
	}
}


void OplJlabsLocalSystem::DiscScanNotifier(APKE_MS_NOTIFY_Type_e eType, void *pvData)
{
	OplDiscInfo *discInfo;

	// Notify device removal
	for (HUINT32 i=0; i<mCountDeviceInfoList; i++)
	{
		const APKS_MS_DEVINFO_t& devInfo(mDeviceInfoList[i]);

		if(strcmp((HCHAR *)devInfo.szUUID, (HCHAR *)pvData) == 0)
		{
			discInfo = makeDiscInfo(devInfo);

			HxLOG_Print("[%s] [%d]DISC_LOAD ID : %s, Name : %s, PVR : %d\n",__FUNCTION__,__LINE__,discInfo->DiscId, discInfo->Name, discInfo->IsFormatted);
			s_pfnOplJLSEventListener(OPL_JLABS_LOCALSYSTEM_EVENT_DISC_LOAD_FINISHED, (void *)discInfo, (void *)NULL, (void *)NULL);

		}
	}

	HLIB_STD_MemFree(pvData);
}

void OplJlabsLocalSystem::insertSetupInfoNotifier(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	APKE_JLABS_SysCtrlJapanEvent_t eEvent = (APKE_JLABS_SysCtrlJapanEvent_t)p1;
	eOplJlabsLocalsystemEventType eOplEvent = OPL_JLABS_LOCALSYSTEM_EVENT_UNKOWN;
	int					errorNumber = 1; //abort
	HxLOG_Print("[%s] eEvent %d\n",__FUNCTION__,eEvent);
	switch(eEvent)
	{
		case eSysCtrl_Event_InsertSetupInfoOK:
			eOplEvent = OPL_JLABS_LOCALSYSTEM_EVENT_INSERT_SETUPINFO_FINISHED;
			errorNumber = 0;	//complete
			break;
		case eSysCtrl_Event_InsertSetupInfoFAIL:
			eOplEvent = OPL_JLABS_LOCALSYSTEM_EVENT_INSERT_SETUPINFO_FINISHED;
			errorNumber = 1;	//abort
			break;
		default:
			return;
	}
	HxLOG_Print("[%s] eOplEvent %d\n",__FUNCTION__,eOplEvent);
	if(eOplEvent != OPL_JLABS_LOCALSYSTEM_EVENT_UNKOWN)
	{
		HxLOG_Print("[%s]call s_pfnOplJLSEventListener eOplEvent %d\n",__FUNCTION__,eOplEvent);
		s_pfnOplJLSEventListener(eOplEvent, (void*)errorNumber, NULL, NULL);
	}
}

void OplJlabsLocalSystem::setEventListener(OplJlabsLocalListener_t listener)
{
	if(listener == NULL)
	{
		return;
	}

	if(s_pfnOplJLSEventListener == NULL)
	{
		s_pfnOplJLSEventListener = listener;
		if(s_bIsRegisterJLSEventListener == OPL_FALSE)
		{
			APK_SQCMS_RegisterDeviceListNotifier(OplJlabsLocalSystem::DeviceListNotifier);
			APK_SQCSCENARIOJAPAN_RegisterNotifier(OplJlabsLocalSystem::insertSetupInfoNotifier);

			s_bIsRegisterJLSEventListener = OPL_TRUE;
		}
	}

	updateDeviceInfoList();

	APK_EXE_RFVOD_RegisterNotifier(OplJlabsLocalSystem::ResidentAppFinishNotifier);
	APK_FACTORY_RegisterNotifier(OplJlabsLocalSystem::ResidentAppFinishNotifier);
}

OplDiscInfo* OplJlabsLocalSystem::makeDiscInfo(const APKS_MS_DEVINFO_t &devInfo)
{
	OplDiscInfo *data = new OplDiscInfo();
	APKS_MS_Size_t	*pstSize;
	HUINT32			i;

	pstSize = (APKS_MS_Size_t *)&devInfo.stSize;

	data->IsFormatted = FALSE;

	for(i=0 ; i< devInfo.ulPartitionNum ; i++)
	{
		if(devInfo.astPartitionList[i].bPvrFlag == TRUE)
		{
			JLS_DBG(HxLOG_Print("[%d] PvrIdx : %d\n", i, devInfo.astPartitionList[i].ulPvrIdx);)
			data->IsFormatted = TRUE;
		}
	}

	data->TotalSize = (pstSize->ulTotalSizeGb* 1024) + (pstSize->ulTotalSizeKb / 1024);
	data->FreeSize = (pstSize->ulAvailableSizeGb * 1024) + (pstSize->ulAvailableSizeKb / 1024);
	data->ReservedSize = (pstSize->ulReservedSizeGb* 1024) + (pstSize->ulReservedSizeKb / 1024) ;

	data->DiscId = HLIB_STD_StrDup((HCHAR *)devInfo.szUUID);
	data->Name = HLIB_STD_StrDup((HCHAR *)devInfo.szLabel);

	JLS_DBG(
	HxLOG_Print("------------MakeDiscInfo------------\n");
	HxLOG_Print("Total  : [%ld Gb]+[%ld Kb]\n", pstSize->ulTotalSizeGb, pstSize->ulTotalSizeKb);
	HxLOG_Print("Free : [%ld Gb]+[%ld Kb]\n", pstSize->ulAvailableSizeGb, pstSize->ulAvailableSizeKb);
	HxLOG_Print("Reserved	: [%ld Gb]+[%ld Kb]\n", pstSize->ulReservedSizeGb, pstSize->ulReservedSizeKb);
	HxLOG_Print("PVR : %s\n",(data->IsFormatted) ? "OK" : "Not Support");
	HxLOG_Print("DiscId : %s\n", data->DiscId);
	HxLOG_Print("Name : %s\n", data->Name);
	HxLOG_Print("---------------------------------\n");
	)

	return data;
}

void OplJlabsLocalSystem::updateDeviceInfoList()
{
	HUINT32 deviceInfoListCount(0);
	APKS_MS_DEVINFO_t* deviceInfoList(NULL);
	OplDiscInfo *discInfo;
	HBOOL  bSafeRemoved = FALSE;

	/* USB Device의 연결을 전반 적으로 관리 한다. 탈착 이벤트는 HDD의 경우에만 올려준다. */
	HxLOG_Warning("[%s] [%d]==========Update Disc===============\n",__FUNCTION__,__LINE__);

	if (APK_SQCMS_INFO_GetList(eDEV_LIST_FILTER_All, &deviceInfoList, &deviceInfoListCount) != ERR_OK) {
		HxLOG_Error("OplJlabsLocalSystem::init FAILED to get APK_SQCMS_INFO_GetList\n");
	}

	JLS_DBG(
	HxLOG_Print("OplJlabsLocalSystem::updateDeviceInfoList");
	HxLOG_Print(" from[");
	for (HUINT32 i=0; i<mCountDeviceInfoList; i++) {
		const APKS_MS_DEVINFO_t& devInfo(mDeviceInfoList[i]);
		HxLOG_Print("%d,", devInfo.ulDeviceId);
	}
	HxLOG_Print("] to[");
	for (HUINT32 i=0; i<deviceInfoListCount; i++) {
		const APKS_MS_DEVINFO_t& devInfo(deviceInfoList[i]);
		HxLOG_Print("%d,", devInfo.ulDeviceId);
	}
	HxLOG_Print("]\n");
	)

	bool changed(false);

	// Notify device removal
	for (HUINT32 i=0; i<mCountDeviceInfoList; i++)
	{
		const APKS_MS_DEVINFO_t& devInfo(mDeviceInfoList[i]);
		if (!hasDeviceInfo(deviceInfoList, deviceInfoListCount, devInfo))
		{
			// 연결 해제된 device
			APK_SQCMS_UnRegisterDeviceInfoNotifier(devInfo.ulDeviceId, OplJlabsLocalSystem::DeviceInfoNotifier);
			discInfo = makeDiscInfo(devInfo);
			if(mDetachDiscId)
			{
				if(strcmp(mDetachDiscId, (HCHAR *)discInfo->DiscId) == 0)
				{
					HLIB_STD_MemFree(mDetachDiscId);
					mDetachDiscId = NULL;
					bSafeRemoved = TRUE;
				}
			}

			// 제거알림 : 안전제거 된것은 Sync로 처리되므로 알리지 않는다. HDD외에 다른 device도 알리지 않는다.
			if(bSafeRemoved == FALSE && devInfo.eDevKind == eMS_DEV_KIND_HDD)
			{
				HxLOG_Warning("[%s] [%d] [DISC DETACHED] ID : %s, Name : %s, bPVR : %d\n",__FUNCTION__,__LINE__,discInfo->DiscId, discInfo->Name, discInfo->IsFormatted);
				s_pfnOplJLSEventListener(OPL_JLABS_LOCALSYSTEM_EVENT_DISC_DETACHED, (void *)discInfo->DiscId, (void *)discInfo->Name, (void *)NULL);
			}

			changed = true;
		}
	}
	// Notify new device
	for (HUINT32 i=0; i<deviceInfoListCount; i++) {
		const APKS_MS_DEVINFO_t& devInfo(deviceInfoList[i]);
		if (!hasDeviceInfo(mDeviceInfoList, mCountDeviceInfoList, devInfo)) {
			APK_SQCMS_RegisterDeviceInfoNotifier(devInfo.ulDeviceId, OplJlabsLocalSystem::DeviceInfoNotifier);
			discInfo = makeDiscInfo(devInfo);

			// 연결알린다.
			if(devInfo.eDevKind == eMS_DEV_KIND_HDD)
			{
				HxLOG_Warning("[%s] [%d] [DISC ATTACHED] ID : %s, Name : %s, bPVR : %d\n",__FUNCTION__,__LINE__,discInfo->DiscId, discInfo->Name, discInfo->IsFormatted);
				s_pfnOplJLSEventListener(OPL_JLABS_LOCALSYSTEM_EVENT_DISC_ATTACHED, (void *)discInfo, (void *)NULL, (void *)NULL);
			}
			changed = true;
		}
	}

	// Cache new info
	if (mDeviceInfoList) {
		APK_SQCMS_INFO_FreeList(mDeviceInfoList, mCountDeviceInfoList);
		mDeviceInfoList = NULL;
	}
	mCountDeviceInfoList = deviceInfoListCount;
	mDeviceInfoList = deviceInfoList;

	// Process all device info
	for (HUINT32 i=0; i<mCountDeviceInfoList; i++) {
		const APKS_MS_DEVINFO_t& devInfo(mDeviceInfoList[i]);

		//printf("[%d:%d]", devInfo.ulDeviceId, devInfo.stStatus.ePhysical);
		// Unmount plugged out device
		if (devInfo.stStatus.ePhysical != eMS_DEV_STATUS_PlugIn) {
			if (devInfo.stStatus.eMount == eMS_DEV_STATUS_Mounted)
			{
				// Unmount
				HxLOG_Warning("[%s] [%d] Unmount HDD devInfo.ulDeviceId : %d\n",__FUNCTION__,__LINE__, devInfo.ulDeviceId);
				APK_SQCMS_Unmount(devInfo.ulDeviceId);
			}
		}
	}

	if(changed)
	{
		JLS_DBG(printDeviceInfo();)
	}
}

void OplJlabsLocalSystem::DeviceInfoNotifier(APKE_MS_NOTIFY_Type_e eType, void *pvData)
{
	JLS_DBG(HxLOG_Print("OplJlabsLocalSystem::local_DeviceInfoNotifier 0x%08x 0x%08x\n", eType, pvData));

	if (eMS_NOTI_DevInfo == eType || eMS_NOTI_Progress == eType)
	{
		if (eMS_NOTI_Progress == eType)
		{
//			APKS_MS_Progress_t	*pstProgress = (APKS_MS_Progress_t *)pvData;

		}
		else if (eMS_NOTI_DevInfo == eType)
		{
			OplDiscInfo *discInfo;
			APKS_MS_DEVINFO_t	*pstDevInfo = (APKS_MS_DEVINFO_t *)pvData;
			if(pstDevInfo != NULL)
			{
				const APKS_MS_DEVINFO_t& devInfo(*pstDevInfo);
				const APKS_MS_Status_t& status(devInfo.stStatus);
#if 0
				if(status.eFormat == eMS_DEV_STATUS_FormatDone){
					discInfo = makeDiscInfo(devInfo);
					s_pfnOplJLSEventListener(OPL_JLABS_LOCALSYSTEM_EVENT_STORAGE_PROGRESS, (void *)discInfo, (void *)OPL_JLABS_LOCALSYSTEM_PAIRING_DONE, (void *)NULL);
				}
				else
#endif
				if(status.eFormat == eMS_DEV_STATUS_FormatFail){
					discInfo = makeDiscInfo(devInfo);
					HxLOG_Warning("[%s] [%d] [FORMAT FAIL] ID : %s, Name : %s, bPVR : %d\n",__FUNCTION__,__LINE__,discInfo->DiscId, discInfo->Name, discInfo->IsFormatted);
					s_pfnOplJLSEventListener(OPL_JLABS_LOCALSYSTEM_EVENT_STORAGE_PROGRESS, (void *)discInfo, (void *)OPL_JLABS_LOCALSYSTEM_PAIRING_ABNORMAL_HDD, (void *)NULL);

				}
				else	if(status.ePairing == eMS_DEV_STATUS_PairingOk){
					discInfo = makeDiscInfo(devInfo);
					HxLOG_Warning("[%s] [%d] [PAIRING OK] ID : %s, Name : %s, bPVR : %d\n",__FUNCTION__,__LINE__,discInfo->DiscId, discInfo->Name, discInfo->IsFormatted);
					s_pfnOplJLSEventListener(OPL_JLABS_LOCALSYSTEM_EVENT_STORAGE_PROGRESS, (void *)discInfo, (void *)OPL_JLABS_LOCALSYSTEM_PAIRING_DONE, (void *)NULL);
				}
				else if(status.ePairing == eMS_DEV_STATUS_PairingFail){
					discInfo = makeDiscInfo(devInfo);
					HxLOG_Warning("[%s] [%d] [PAIRING FAIL] ID : %s, Name : %s, bPVR : %d\n",__FUNCTION__,__LINE__,discInfo->DiscId, discInfo->Name, discInfo->IsFormatted);
					s_pfnOplJLSEventListener(OPL_JLABS_LOCALSYSTEM_EVENT_STORAGE_PROGRESS, (void *)discInfo, (void *)OPL_JLABS_LOCALSYSTEM_PAIRING_OVER_MAX_HDD, (void *)NULL);
				}
			}
			updateDeviceInfoList();
		}
	}
}

void OplJlabsLocalSystem::DeviceListNotifier(APKE_MS_NOTIFY_Type_e eType, void *pvData)
{
	updateDeviceInfoList();
}

bool OplJlabsLocalSystem::setDefaultStorage(const char *storageId)
{
	if(APK_SQCMS_SetDefaultPvrHdd((HCHAR *)storageId) == ERR_OK)
		return TRUE;

	return FALSE;
}

bool OplJlabsLocalSystem::setStorageName(const char *storageId, const char *storageName)
{
	if(APK_SQCMS_SetStorageName((HCHAR *)storageId, (HCHAR *)storageName) == ERR_OK)
		return TRUE;

	return FALSE;	;
}

bool OplJlabsLocalSystem::setStoragePairing(const char *storageId, const char *storageName)
{
	HUINT32 ulDeviceId;
	OplDiscInfo *discInfo;
	HBOOL	bFind(FALSE);

	for (HUINT32 i=0; i<mCountDeviceInfoList; i++)
	{
		const APKS_MS_DEVINFO_t& devInfo(mDeviceInfoList[i]);

		if(strcmp(storageId, (HCHAR *)devInfo.szUUID) == 0)
		{
			JLS_DBG(HxLOG_Print("Format deviceId : %d,", devInfo.ulDeviceId););
			ulDeviceId = devInfo.ulDeviceId;
			bFind = TRUE;
			discInfo = makeDiscInfo(devInfo);
			break;
		}
	}

	if(bFind != TRUE)
	{
		s_pfnOplJLSEventListener(OPL_JLABS_LOCALSYSTEM_EVENT_STORAGE_PROGRESS, (void *)discInfo, (void *)OPL_JLABS_LOCALSYSTEM_PAIRING_ABNORMAL_HDD, (void *)NULL);
		return FALSE;
	}

	APK_SQCMS_Format(ulDeviceId, (const HCHAR *)storageName);
	delete discInfo;
	return TRUE;

}

bool OplJlabsLocalSystem::DetachDisc(const char *storageId, bool force)
{
	HUINT32 ulDeviceId;
	OplDiscInfo *discInfo;
	HBOOL	bFind(FALSE);
	HERROR		hErr = ERR_FAIL;

	for (HUINT32 i=0; i<mCountDeviceInfoList; i++)
	{
		const APKS_MS_DEVINFO_t& devInfo(mDeviceInfoList[i]);

		if(strcmp(storageId, (HCHAR *)devInfo.szUUID) == 0)
		{
			JLS_DBG(HxLOG_Print("Format deviceId : %d,", devInfo.ulDeviceId););
			ulDeviceId = devInfo.ulDeviceId;
			bFind = TRUE;
			discInfo = makeDiscInfo(devInfo);
			break;
		}
	}

	mDetachDiscId= HLIB_STD_StrDup((HCHAR *)storageId);

	if(bFind != TRUE)
	{
		s_pfnOplJLSEventListener(OPL_JLABS_LOCALSYSTEM_EVENT_STORAGE_PROGRESS, (void *)discInfo, (void *)OPL_JLABS_LOCALSYSTEM_PAIRING_ABNORMAL_HDD, (void *)NULL);
		return FALSE;
	}
	delete discInfo;

	hErr = APK_SQCMS_SafeRemove(ulDeviceId, force);

	if(hErr == ERR_OK)
	{
		return TRUE;
	}
	return FALSE;
}

bool OplJlabsLocalSystem::startPairing()
{

	return FALSE;
}

bool OplJlabsLocalSystem::abortPairing()
{

	return FALSE;
}

bool OplJlabsLocalSystem::startResidentApp(const char *appName, const char *exInfo)
{
	if(strcmp(appName, (HCHAR *)"VOD") == 0)
	{
//		APK_EXE_RFVOD_Start(TRUE, (HUINT8*)exInfo);
		return TRUE;
	}
	else if(strcmp(appName, (HCHAR *)"ITV") == 0)
	{
//		APK_EXE_RFVOD_Start(FALSE, (HUINT8*)exInfo);
		return TRUE;
	}
	else if(strcmp(appName, (HCHAR *)"FULLBROWSER") == 0)
	{
		if( s_bIsFullBrowserStarted == OPL_TRUE )
		{
			HxLOG_Error("%s %s %d Surfer is already started... return.\n", __FILE__, __FUNCTION__, __LINE__);
			return TRUE;
		}

		pid_t pid;
		pid = fork();
		if(pid == (-1))
		{
			HxLOG_Error("%s %s %d fork error.\n", __FILE__, __FUNCTION__, __LINE__);
			return FALSE;
		}
		else if(pid == 0)
		{
			char *argv[3];
			argv[0] = (char *)"surfer";
			argv[1] = (char *)exInfo;
			argv[2] = NULL;

			//WA-7000 config
			setenv("LANG", "ja",1);

			execv("/usr/bin/surfer", (char **)argv);
			s_bIsFullBrowserStarted = OPL_TRUE;
		}
		else
		{
			HxLOG_Print("%s %s %d parent, then stay.\n", __FILE__, __FUNCTION__, __LINE__);
		}

		return TRUE;
	}
	else if(strcmp(appName, (HCHAR *)"TEST") == 0)
	{
		HxLOG_Warning("[%s:%d] Not supported function....", __FUNCTION__, __LINE__);
//		APK_FACTORY_Start();
		return TRUE;
	}

	return FALSE;
}

bool OplJlabsLocalSystem::ResidentAppFinish(const char *appName)
{
	if(strcmp(appName, (HCHAR *)"VOD") == 0)
	{
//		APK_EXE_RFVOD_Stop();
		return TRUE;
	}
	else if(strcmp(appName, (HCHAR *)"ITV") == 0)
	{
//		APK_EXE_RFVOD_Stop();
		return TRUE;
	}
	else if(strcmp(appName, (HCHAR *)"FULLBROWSER") == 0)
	{
		// TODO:
		HAPI_BroadcastSignal("signal:onStopFullBrowser", 0, "i", 0);
		return TRUE;
	}
	else if(strcmp(appName, (HCHAR *)"TEST") == 0)
	{
		APK_FACTORY_Finish();
		return TRUE;
	}

	return FALSE;
}

void OplJlabsLocalSystem::ResidentAppFinishNotifier(HCHAR *appName)
{
	HxLOG_Print("[%s] [%d] Destroyed App Name : %s\n",__FUNCTION__,__LINE__, appName);
	if(strcmp(appName, (HCHAR *)"FULLBROWSER") == 0)
		s_bIsFullBrowserStarted = OPL_FALSE;
	s_pfnOplJLSEventListener(OPL_JLABS_LOCALSYSTEM_EVENT_RESIDENT_APP_FINISH, (void *)appName, (void *)NULL, (void *)NULL);
}

/* end of file */

