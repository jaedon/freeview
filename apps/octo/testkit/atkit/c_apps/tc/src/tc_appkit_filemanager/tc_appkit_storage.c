#define __HEAD___________________________________________________________________________

#include <uapi.h>
#include "atkit.h"

#define __DEFINE_________________________________________________________________________


#define __STATIC_VARIABLES_______________________________________________________________

static HxJSON_t s_manifest = NULL;

#define __STATIC_FUNCTIONS_______________________________________________________________


#define __TESTCASE_______________________________________________________________________

HINT32 TC_FILEMANAGER_GetStorageInfo(HxJSON_t j_args)
{
	DxStorage_Info_t *pastInfoList = (DxStorage_Info_t *)malloc(sizeof(DxStorage_Info_t));
	HUINT32 ulDevNum = 0;

	HINT64 ullTotalKbSize = 0;
	HINT64 ullAvailableKbSize = 0;
	HINT64 ullUsedKbSize = 0;
	HINT64 ullReservedKbSize = 0;
	HINT64 ullAvailablePvrKbSize = 0;
	HINT64 ullPvrReservedKbSize = 0;

	HERROR hErr = ERR_FAIL;
	HUINT32 ulIndex = 0, i = 0;

	printf("TC NAME ( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));

	printf("[%s] TESTING.. : Get Information of Storage Device\n", __FUNCTION__);
	hErr = APK_FILEMGR_STORAGE_GetList(0xFFFFFFFF /* eDxSTORAGE_FILTER_ALL */, &pastInfoList, &ulDevNum);
	if (ERR_OK != hErr)
	{
		printf("[%s] TEST FAIL : Fail to APK_FILEMGR_STORAGE_GetList() - Err ( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	else
	{
		printf("################# [%s] ################\n", "APK_FILEMGR_STORAGE_GetList()");
		printf("%s - %d\n", "Number of Storage", ulDevNum);
		fflush(stdout);
	}
	
	if (ulDevNum > 0)	
	{	
		for (ulIndex = 0; ulIndex < ulDevNum; ulIndex++)
		{
			printf("%s - %d\n", "Storage device number", ulIndex);
			hErr = APK_FILEMGR_STORAGE_GetSize(pastInfoList[ulIndex].ulDeviceId, &ullTotalKbSize, &ullAvailableKbSize, &ullUsedKbSize, &ullReservedKbSize, &ullAvailablePvrKbSize);
			if (ERR_OK != hErr)
			{
				printf("[%s] TEST FAIL : Fail to APK_FILEMGR_STORAGE_GetSize() - Err ( %d )\n", __FUNCTION__, hErr);
				fflush(stdout);
				return FALSE;
			}
			else
			{
				printf("################# [%s] ################\n", "APK_FILEMGR_STORAGE_GetSize()");
				printf("%s - %lld\n", "ullTotalKbSize", ullTotalKbSize);
				printf("%s - %lld\n", "ullAvailableKbSize", ullAvailableKbSize);
				printf("%s - %lld\n", "ullUsedKbSize", ullUsedKbSize);
				printf("%s - %lld\n", "ullReservedKbSize", ullReservedKbSize);
				printf("%s - %lld\n", "ullAvailablePvrKbSize", ullAvailablePvrKbSize);
				fflush(stdout);
			}

			hErr = APK_FILEMGR_STORAGE_GetPvrReservedSize(pastInfoList[ulIndex].ulDeviceId, &ullPvrReservedKbSize);
			if (ERR_OK != hErr)
			{
				printf("[%s] TEST FAIL : Fail to APK_FILEMGR_STORAGE_GetPvrReservedSize() - Err ( %d )\n", __FUNCTION__, hErr);
				fflush(stdout);
				return FALSE;
			}
			else
			{
				printf("################# [%s] ################\n", "APK_FILEMGR_STORAGE_GetPvrReservedSize()");
				printf("%s - %lld\n", "ullPvrReservedKbSize", ullPvrReservedKbSize);
			}

			if (pastInfoList[ulIndex].eDevType < eDxSTORAGE_DEVTYPE_NONE || pastInfoList[ulIndex].eDevType > eDxSTORAGE_DEVTYPE_ALL)
			{
				printf("[%s] TEST FAIL : Fail to APK_FILEMGR_STORAGE_GetDevTypeString() - Err ( %d )\n", __FUNCTION__, pastInfoList[ulIndex].eDevType);
				fflush(stdout);
				return FALSE;
			}
			else
			{
				printf("################ [%s] ################\n", "APK_FILEMGR_STORAGE_GetDevTypeString()");
				printf("%s - %s\n", "Device Type", APK_FILEMGR_STORAGE_GetDevTypeString(pastInfoList[ulIndex].eDevType));
				fflush(stdout);
			}

			if (pastInfoList[ulIndex].nPartitionNum <= 0)
			{
				printf("[%s] TEST FAIL : Fail to APK_FILEMGR_STORAGE_GetFsString() - Err ( %d )\n", __FUNCTION__, pastInfoList[ulIndex].nPartitionNum);
				fflush(stdout);
				return FALSE;
			}
			else 
			{
				printf("################ [%s] ################\n", "APK_FILEMGR_STORAGE_GetFsString()");
				for (i = 0; i < pastInfoList[ulIndex].nPartitionNum; i++)
				{
					printf("%s ( %d ) - %s\n", "Filesystem type", i, APK_FILEMGR_STORAGE_GetFsString(pastInfoList[ulIndex].astPartitionInfos[i].eFsType));
					fflush(stdout);
				}
			}	
		}
	}

	printf("[%s] TEST SUCCESS : TC_FILEMANAGER_GetStorageInfo()\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;

}

HINT32 TC_FILEMANAGER_PairHdd(HxJSON_t j_args)
{
	DxStorage_Info_t *pastInfoList = (DxStorage_Info_t *)malloc(sizeof(DxStorage_Info_t));
	HUINT32 ulDevNum = 0;
	HUINT32 ulIndex = 0;
	HERROR hErr = ERR_FAIL;

	printf("TC NAME ( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));

	printf("[%s] TESTING.. : Pairing/Unparing Storage Device\n", __FUNCTION__);
	hErr = APK_FILEMGR_STORAGE_GetList(0xFFFFFFFF /* eDxSTORAGE_FILTER_ALL */, &pastInfoList, &ulDevNum);
	if (ERR_OK != hErr)
	{
		printf("[%s] TEST FAIL : Fail to APK_FILEMGR_STORAGE_GetList() - Err ( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	else
	{
		printf("################# [%s] ################\n", "APK_FILEMGR_STORAGE_GetList()");
		printf("%s - %d\n", "Number of Storage", ulDevNum);
		fflush(stdout);
	}
	/* ********* Pairing Concept in Real Case ************** 
		* Disable the current PVR HDD
		* Pairing the choosen HDD
		* Unparing the choosen HDD
	********************************************************/
	for (ulIndex = 0; ulIndex < ulDevNum; ulIndex++)
	{
		hErr = APK_FILEMGR_STORAGE_DisablePVRHdd(pastInfoList[ulIndex].szUUID);
		if (ERR_OK != hErr)
		{
			printf("[%s] TEST FAIL : Fail to APK_FILEMGR_STORAGE_DisablePVRHdd() - ( %s ) - Err ( %d )\n", __FUNCTION__, pastInfoList[ulIndex].szUUID, hErr);
			fflush(stdout);
			return FALSE;
		}
		else
		{
			printf("%s - %s\n", "APK_FILEMGR_STORAGE_DisablePVRHdd() SUCCESS", pastInfoList[ulIndex].szUUID);
			fflush(stdout);
		}
		hErr = APK_FILEMGR_STORAGE_PairingHdd(pastInfoList[ulIndex].szUUID);
		if (ERR_OK != hErr)
		{
			printf("[%s] TEST FAIL : Fail to APK_FILEMGR_STORAGE_PairingHdd() - ( %s ) - Err( %d )\n", __FUNCTION__, pastInfoList[ulIndex].szUUID, hErr);
			fflush(stdout);
			return FALSE;
		}
		else
		{
			printf("%s - %s\n", "APK_FILEMGR_STORAGE_PairingHdd() SUCCESS", pastInfoList[ulIndex].szUUID);
			fflush(stdout);
		}

		hErr = APK_FILEMGR_STORAGE_UnPairingHdd(pastInfoList[ulIndex].szUUID);
		if (ERR_OK != hErr)
		{
			printf("[%s] TEST FAIL : Fail to APK_FILEMGR_STORAGE_UnPairingHdd() - ( %s ) - Err( %d )\n", __FUNCTION__, pastInfoList[ulIndex].szUUID, hErr);
			fflush(stdout);
			return FALSE;
		}
		else
		{
			printf("%s - %s\n", "APK_FILEMGR_STORAGE_UnPairingHdd SUCCESS", pastInfoList[ulIndex].szUUID);
			fflush(stdout);
		}
	}

	printf("[%s] TEST SUCCESS : TC_FILEMANAGER_PairHdd()\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;
}

void TC_FILEMANAGER_StorageInit(HxJSON_t jManifest)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);

	s_manifest = jManifest;

	printf("%s\n", "Initialize UAPI_FS_Init()");
	UAPI_FS_Init();

	/* Wait for 2s to update the information */
	HLIB_STD_TaskSleep(2000);

	APK_FILEMGR_STORAGE_Init();
	printf("%s - %s\n", "APK_FILEMGR_STORAGE_Init()", "DONE!");

	ADD_TC(TC_FILEMANAGER_GetStorageInfo);
	ADD_TC(TC_FILEMANAGER_PairHdd);
}

void TC_FILEMANAGER_StorageDeInit(void)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}
