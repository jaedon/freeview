/**************************************************************************************/
/**
 * @file opldiscinfo.cpp
 *
 * Details of storage usage
 *
 **************************************************************************************
 **/

#include "opldiscinfo.h"
#include <hlib.h>
#include "apk.h"

/******************************************************************
	class OplDiscInfo
******************************************************************/

OplDiscInfo	g_defaultDiscInfo;


#if defined(CONFIG_OP_JAPAN)
OplDiscInfo::OplDiscInfo(): TotalSize(0), FreeSize(0), ReservedSize(0), IsFormatted(FALSE), DiscId(NULL), Name(NULL)
{
}

OplDiscInfo::~OplDiscInfo()
{
}

void local_OplDiscInfoFillDiscInfo(OplDiscInfo *info, APKS_MS_DEVINFO_t* devInfo, HBOOL bFlag)
{
	HUINT64 ullUsedSize = 0;
	HUINT64 ullTotalSize = 0;
	HUINT64 ullReservedSize = 0;
	HUINT64 ullAvailSize = 0;

	info->IsFormatted = bFlag;

	APK_SQCMS_UTIL_GetSize(devInfo->ulDeviceId, &ullTotalSize, &ullAvailSize, &ullUsedSize, &ullReservedSize, NULL);

	info->TotalSize = ullTotalSize / 1024;
	info->FreeSize = ullAvailSize / 1024;
	info->ReservedSize = ullReservedSize / 1024 ;
	info->DiscId = HLIB_STD_StrDup((HCHAR *)devInfo->szUUID);
	info->Name = HLIB_STD_StrDup((HCHAR *)devInfo->szLabel);
}

BPLStatus	OPL_Discinfo_GetDiscInfoList(OplDiscInfo **info, HUINT32 *deviceCount,  DiscInfo_StorageType_e eType)
{
	APKS_MS_DEVINFO_t* deviceInfoList =NULL;
	APKS_MS_DEVINFO_t* devInfo = NULL;
	HUINT32		ulListType;
	OplDiscInfo 		*discInfoArray = NULL;
	HUINT32 			deviceInfoListCount = 0;
	HUINT32 			i, j;
	HBOOL			bPVR = FALSE;

	if (info != NULL)					{ *info = NULL; }

	if(eType == eDiscInfo_All)
	{
		ulListType = eDEV_LIST_FILTER_Hdd;
	}
	else if(eType == eDiscInfo_PVR_Only)
	{
		ulListType = eDEV_LIST_FILTER_PVR | eDEV_LIST_FILTER_Hdd;
	}

	if (APK_SQCMS_INFO_GetList(ulListType, &deviceInfoList, &deviceInfoListCount) != ERR_OK) {
		HxLOG_Print("OplJlabsLocalSystem::init FAILED to get APK_SQCMS_INFO_GetList\n");
	}

	if(deviceInfoListCount != 0)
	{	discInfoArray = new OplDiscInfo[deviceInfoListCount];
		if (discInfoArray == NULL)
		{
			return BPL_STATUS_ERROR;
		}
	}

	// Fill storages
	for (i=0; i<deviceInfoListCount; i++) {
		devInfo = &deviceInfoList[i];
		bPVR = FALSE;

		for(j=0 ; j< devInfo->ulPartitionNum ; j++)
		{
			//현재는 Ext HDD의 경우 0번 partition사용한다.
			//Int. HDD는 1번을 사용한다.
			// 어디에 있는 발견되면 PVR HDD이므로 TRUE 설정하고 나간다.
			if(devInfo->astPartitionList[j].bPvrFlag == TRUE)
			{
				bPVR = TRUE;
				break;
			}
		}
		local_OplDiscInfoFillDiscInfo(&discInfoArray[i], devInfo, bPVR);
	}

	if (deviceInfoList) {
		APK_SQCMS_INFO_FreeList(deviceInfoList, deviceInfoListCount);
		deviceInfoList = NULL;
	}

	*deviceCount = deviceInfoListCount;
	*info = discInfoArray;
	discInfoArray = NULL;

	return BPL_STATUS_OK;

}

BPLStatus	OPL_Discinfo_GetDefaultDiscInfo(OplDiscInfo *info)
{
	HUINT32 deviceInfoListCount = 0;
	APKS_MS_DEVINFO_t* deviceInfoList =NULL;
	APKS_MS_DEVINFO_t* devInfo = NULL;
	HUINT32 i, j;

	if (APK_SQCMS_INFO_GetList(eDEV_LIST_FILTER_PVR | eDEV_LIST_FILTER_Hdd, &deviceInfoList, &deviceInfoListCount) != ERR_OK) {
		HxLOG_Print("OplJlabsLocalSystem::init FAILED to get APK_SQCMS_INFO_GetList\n");
	}

	info->IsFormatted = FALSE;
	for (i=0; i<deviceInfoListCount; i++) {
		devInfo = &deviceInfoList[i];
		for(j=0 ; j< devInfo->ulPartitionNum ; j++)
		{
			if(devInfo->astPartitionList[j].bPvrFlag == TRUE && devInfo->astPartitionList[j].bDefaultPvr == TRUE)
			{
				local_OplDiscInfoFillDiscInfo(info, devInfo, TRUE);
				break;
			}
		}
		if(info->IsFormatted == TRUE)
		{
			break;
		}
	}
	if (deviceInfoList) {
		APK_SQCMS_INFO_FreeList(deviceInfoList, deviceInfoListCount);
		deviceInfoList = NULL;
	}

	return BPL_STATUS_OK;

}

OplDiscInfo* OPL_DiscInfo_Clone(OplDiscInfo *info)
{
	OplDiscInfo *clone;

	clone = (OplDiscInfo *)HLIB_STD_MemCalloc(sizeof(OplDiscInfo));
	if (clone == NULL)
	{
		return NULL;
	}

	clone->FreeSize = info->FreeSize;
	clone->TotalSize = info->TotalSize;
	clone->ReservedSize = info->ReservedSize;
	clone->IsFormatted= info->IsFormatted;
	clone->DiscId           = HLIB_STD_StrDup(info->DiscId);
	clone->Name           = HLIB_STD_StrDup(info->Name);

	return clone;
}

BPLStatus OPL_DiscInfo_SetStorageName(const char *storageId, const char *storageName)
{
	APK_SQCMS_SetStorageName((HCHAR *)storageId, (HCHAR *)storageName);
	return BPL_STATUS_OK;
}

#else
OplDiscInfo::OplDiscInfo()
{
}

OplDiscInfo::~OplDiscInfo()
{
}

unsigned int OplDiscInfo::getFreeSize()
{
	unsigned int ulFree = 0;

	if ( OPL_Discinfo_GetFree(&ulFree) != BPL_STATUS_OK )
	{
		return 0;
	}

	return ulFree;
}

unsigned int OplDiscInfo::getTotalSize()
{
	unsigned int ulTotal = 0;

	if ( OPL_Discinfo_GetTotal(&ulTotal) != BPL_STATUS_OK )
	{
		return 0;
	}

	return ulTotal;
}

unsigned int OplDiscInfo::getReservedSize()
{
	unsigned int ulReserved = 0;

	if ( OPL_Discinfo_GetReserved(&ulReserved) != BPL_STATUS_OK )
	{
		return 0;
	}

	return ulReserved;
}
#endif
