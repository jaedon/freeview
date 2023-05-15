/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/**************************************************************************************************/
#define _________FS_MMC_Private_Include_________________________________________________
/**************************************************************************************************/
#include <vkernel.h>

#include <uapi.h>
#include <hapi.h>
#include <umma_fs_pal_mmc.h>

#include <umma_int.h>
#include "umma_fs_int.h"

#define DUMMY_DATA
#define DEVICENAME_MAX_LEN	128

/**************************************************************************************************/
#define _________FS_MMC_Private_Macro_________________________________________________
/**************************************************************************************************/
#define MMC_RETRY_UMOUNT
#define MMC_DEVICE_ID ePAL_FS_DEV_mmc
/**************************************************************************************************/
#define _________FS_MMC_Private_Struct_________________________________________________
/**************************************************************************************************/

typedef struct
{
	HINT32 dPort;
	HCHAR  pszDevName[DEVICENAME_MAX_LEN];
	HINT32 DeviceID;
} SvcFs_MmcContents_t;

/**************************************************************************************************/
#define _________FS_MMC_Private_Value_________________________________________________
/**************************************************************************************************/
/**************************************************************************************************/
#define _________FS_MMC_Private_Prototype_________________________________________________
/**************************************************************************************************/
/**************************************************************************************************/
#define _________FS_MMC_Private_Init__________________________________________________________
/**************************************************************************************************/
/**************************************************************************************************/
#define _________FS_MMC_Private_Destroy__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________FS_MMC_Private_Functions__________________________________________________________
/**************************************************************************************************/
STATIC SvcFs_MmcContents_t *umma_fs_mmc_GetContents(void)
{
	STATIC SvcFs_MmcContents_t *pContents = NULL;

	if(pContents== NULL)
	{
		 pContents = (SvcFs_MmcContents_t*)UMMA_Calloc(sizeof(SvcFs_MmcContents_t));
	}
	return pContents;
}


// 마운트를 여기서 시키도, ePAL_MMC_EVT_MOUNT_DONE / ePAL_MMC_EVT_MOUNT_FAIL 에서 정보를 채워 준다.
STATIC HERROR umma_fs_mmc_AddDevice(HINT32 dPort, HCHAR* pszName)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
	UMMA_FS_DevIdx_e	eDevIdx;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR			hErr;
//	UMMA_FS_PartitionInfo_t	*pstPartArray;
	SvcFs_MmcContents_t *pContents = umma_fs_mmc_GetContents();
	pContents->dPort 		   = dPort;
	HUINT32 nStrLen = HxSTD_StrLen(pszName);

	HxSTD_MemSet(pContents->pszDevName,0x0,DEVICENAME_MAX_LEN);
	HxSTD_StrNCpy(pContents->pszDevName,pszName, (nStrLen < DEVICENAME_MAX_LEN ? nStrLen : DEVICENAME_MAX_LEN - 1) );

	hErr =PAL_MMC_Mount(dPort,MMC_MOUNT_PATH);


	return ERR_OK;

	eDevIdx = umma_fs_dev_ConvertPalDevIdxToMwDevIdx (MMC_DEVICE_ID);

	HxLOG_Print(HxANSI_COLOR_YELLOW("____umma_fs_mmc_mount [%d]\n"),eDevIdx);

	if (eDevIdx < UMMA_FS_DEV_IDX_START || eDevIdx > UMMA_FS_DEV_IDX_END)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}



	// Check if the device is already mounted
	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr == ERR_OK && pstDevice != NULL)
	{
		// Device가 존재한 경우는 세가지이다.
		// 1. 이미 Connection 처리되어 있고 Mount 처리도 된 상태 : Skip 이다.
		if (pstDevice->bDevConnected == TRUE && pstDevice->bDevMounted == TRUE)
		{
			HxLOG_Print("[umma_fs_mmc_AddDevice] Already Dev(%d) mounted\n", eDevIdx);
			return ERR_FAIL;
		}
		// 2. Device가 Connect 되어 있다고 인식되어 있는데 mount가 안 된 상태 : Device를 초기화하고 여기서 재설정으로 간다.
		else if (pstDevice->bDevConnected == TRUE)
		{
			HxLOG_Print(HxANSI_COLOR_YELLOW("pstDevice->bDevConnected == TRUE[%d]\n"),eDevIdx);
			umma_fs_dev_InitFsDeviceInfo (pstDevice);
			//pstDevice = NULL;
		}
		// 3. Device가 Connect 되어 있지 않은 것으로 인식하는데 mount는 되어 있는 상태 : unmount를 진행한 후, device를 초기화하고 재설정해야 한다.
		else if (pstDevice->bDevMounted == TRUE)
		{
		//	PAL_MMC_UnMount(dPort);
		}
		// 그 외의 경우는 있을 수 없다. Error 처리하도록 한다.
		else
		{
			HxLOG_Critical("\nCritical Error!!!\n\n");
			return ERR_FAIL;
		}

	}
	if (pstDevice == NULL)
	{
		// Get the empty slot in the device
		pstDevice = NULL;
		hErr = umma_fs_dev_FindEmptyDeviceInfoSlot (&pstDevice);
		if (hErr != ERR_OK || pstDevice == NULL)
		{
			HxLOG_Error("[umma_fs_mmc_AddDevice] No empty device slot.\n");
			return ERR_FAIL;
		}
	}

	hErr =PAL_MMC_Mount(dPort,MMC_MOUNT_PATH);

	return hErr;

#else
	return ERR_FAIL;
#endif
}

STATIC HERROR umma_fs_mmc_RemoveDevice(void)
{
	UMMA_FS_DevIdx_e		 eDevIdx;
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	HERROR				 hErr;
	HBOOL				bSafeRemoved = FALSE;
	HxLOG_Print(HxANSI_COLOR_YELLOW("____umma_fs_mmc_RemoveDevice\n"));


	eDevIdx = umma_fs_dev_ConvertPalDevIdxToMwDevIdx (MMC_DEVICE_ID);
	if (eDevIdx < UMMA_FS_DEV_IDX_START || eDevIdx > UMMA_FS_DEV_IDX_END)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr == ERR_OK && pstDevice != NULL)
	{

		// 장비가 뽑혔으니 이 Flag도 제거되어야 한다.
		pstDevice->bIsExPvrModule = FALSE;

		// Device가 Disconnect 되었다는 것은 기록해두어야 한다.
		pstDevice->bDevConnected = FALSE;

		 //이미 안전제거(umount)된 device인지 여부를 올려준다.
		if(pstDevice->eDevStatus == eFS_DevStatus_SafeRemoved)
		{
			bSafeRemoved = TRUE;
			HxLOG_Print("\n _______________bSafeRemoved \n");
			umma_fs_dev_InitFsDeviceInfo (pstDevice);
		}
		if (!bSafeRemoved)	 //이미 umount 되었으니 올릴필요 없다.
		{
			HxLOG_Print(HxANSI_COLOR_YELLOW("\n EVT_FS_NOTIFY_MMC_DISCONNECTED\n\n"));
#if 0
			MWC_UTIL_PostMsgToAp (EVT_FS_NOTIFY_MMC_DISCONNECTED, HANDLE_NULL, (HINT32)eDevIdx, 0, 0);
#else
			(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_DISCONNECTED, (HINT32)eDevIdx, 0, 0);
#endif
		}
		else
		{
			// 실제 동작상 에러는 아니지만, 동기화 처리 동작을 위해 FAIL 리턴
			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Error("\numma_fs_dev_GetDeviceInfo error\n\n");
		return ERR_FAIL;
	}


	//HxLOG_Print(HxANSI_COLOR_YELLOW("UnMount OK...[%d]\n"),pContents->dPort);

	return ERR_OK;
}

STATIC HERROR umma_fs_mmc_SendConnectMessage(void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
	UMMA_FS_DevIdx_e			eDevIdx;
	HERROR					hErr;
	UMMA_FS_PartitionInfo_t	*pstPartArray;
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	HCHAR 				fstype[128]={0,};
	UMMA_FS_FsType_e	eDeviceType = eFS_FileSystem_NONE;
	SvcFs_MmcContents_t 		*pContents = umma_fs_mmc_GetContents();
	HUINT64		ullTotalKb = 0;

	UMMA_FS_PvrType_e	ePvrType = eFS_PVR_None;
	HUINT32			    ulPvrIdx =0;

	HxLOG_Print(HxANSI_COLOR_YELLOW("\nePAL_MMC_EVT_MOUNT_DONE \n\n"));

	eDevIdx = umma_fs_dev_ConvertPalDevIdxToMwDevIdx (MMC_DEVICE_ID);

	HxLOG_Print(HxANSI_COLOR_YELLOW("____umma_fs_mmc_mount [%d]\n"),eDevIdx);

	if (eDevIdx < UMMA_FS_DEV_IDX_START || eDevIdx > UMMA_FS_DEV_IDX_END)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}


	// Check if the device is already mounted
	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr == ERR_OK && pstDevice != NULL)
	{
		// Device가 존재한 경우는 세가지이다.
		// 1. 이미 Connection 처리되어 있고 Mount 처리도 된 상태 : Skip 이다.
		if (pstDevice->bDevConnected == TRUE && pstDevice->bDevMounted == TRUE)
		{
			HxLOG_Print("[umma_fs_mmc_AddDevice] Already Dev(%d) mounted\n", eDevIdx);
			return ERR_FAIL;
		}
		// 2. Device가 Connect 되어 있다고 인식되어 있는데 mount가 안 된 상태 : Device를 초기화하고 여기서 재설정으로 간다.
		else if (pstDevice->bDevConnected == TRUE)
		{
			HxLOG_Print(HxANSI_COLOR_YELLOW("_pstDevice__bDevConnected [%d]\n"),eDevIdx);
			umma_fs_dev_InitFsDeviceInfo (pstDevice);
			//pstDevice = NULL;
		}
		// 3. Device가 Connect 되어 있지 않은 것으로 인식하는데 mount는 되어 있는 상태 : unmount를 진행한 후, device를 초기화하고 재설정해야 한다.
		else if (pstDevice->bDevMounted == TRUE)
		{
		//	PAL_MMC_UnMount(dPort);
		}
		// 그 외의 경우는 있을 수 없다. Error 처리하도록 한다.
		else
		{
			HxLOG_Critical("\nCritical Error!!!\n\n");
			return ERR_FAIL;
		}

	}
	if (pstDevice == NULL)
	{
		// Get the empty slot in the device
		pstDevice = NULL;
		hErr = umma_fs_dev_FindEmptyDeviceInfoSlot (&pstDevice);
		if (hErr != ERR_OK || pstDevice == NULL)
		{
			HxLOG_Error("[umma_fs_mmc_AddDevice] No empty device slot.\n");
			return ERR_FAIL;
		}
	}
	pContents->DeviceID 		= eDevIdx;

	pstDevice->bDevMounted 		= TRUE;
	pstDevice->bDevAvailable	= TRUE;
	pstDevice->bDevConnected	= TRUE;
	pstDevice->eDevIdx			= eDevIdx;

	pstDevice->eDevSubStatus 	= eFS_DevSubStatus_None;
	pstDevice->eDevType			= eFS_DevType_MMC;
	pstDevice->ulPortNum 		= pContents->dPort;
	pstDevice->ulNumPartition	= 0;
	pstDevice->ulMaxPartition	= 1;
	pstDevice->eFsType			= eFS_FileSystem_NONE;
	pstDevice->pstPartArray 	=NULL;
	pstDevice->eDevStatus 		= eFS_DevStatus_OK;

	PAL_MMC_GetVolumeName(pstDevice->ulPortNum , pstDevice->szLabel, UMMA_FS_VOLUME_LABEL_SIZE);

	if(HLIB_DIR_GetMountFstype(MMC_MOUNT_PATH,fstype,128) == ERR_OK)
	{
		HxLOG_Print("MMC FsType[%s] \n",fstype);
		if(HLIB_STD_StrCaseCmp(fstype,"vfat")==0)
		{
			eDeviceType = eFS_FileSystem_FAT32;
		}
		else if(HLIB_STD_StrCaseCmp(fstype,"ntfs")==0)
		{
			eDeviceType = eFS_FileSystem_NTFS;
		}
		else if(HLIB_STD_StrCaseCmp(fstype,"ext4")==0)
		{
			eDeviceType = eFS_FileSystem_EXT4;
		}
	}
	pstDevice->eFsType = eDeviceType;

	//2 Get Total size
	umma_fs_dev_GetPhysicalMMCDeviceSize(eDevIdx, &ullTotalKb, NULL, NULL);
	pstDevice->ullDevSizeKb = ullTotalKb;

	pstPartArray = (UMMA_FS_PartitionInfo_t *)UMMA_Calloc(sizeof(UMMA_FS_PartitionInfo_t) * 1);
	if (pstPartArray != NULL)
	{
		pstPartArray->bAvailable 	= TRUE;
		pstPartArray->bMounted 		= TRUE;
		pstPartArray->bWritable 	= TRUE;
		pstPartArray->ullTotalSizeKb = ullTotalKb;

		HxSTD_StrNCpy(pstPartArray->szVolumeName,pstDevice->szLabel,HxSTD_StrLen(pstDevice->szLabel));
		HxSTD_StrNCpy(pstPartArray->szMountPath,MMC_MOUNT_PATH,HxSTD_StrLen(MMC_MOUNT_PATH));
		pstPartArray->ePartFsType = eDeviceType;
		pstDevice->pstPartArray 	= pstPartArray;
		pstDevice->ulNumPartition	= 1;
	}

	(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_CONNECTED, (HINT32)eDevIdx, 0, 0);

	ePvrType = umma_fs_config_GetPvrType(eFS_DevType_MMC);
	if (ePvrType & eFS_PVR_Main || ePvrType & eFS_PVR_Sub)
	{
		HBOOL	bOk;
		bOk = umma_fs_svc_PvrIsPvrStorageRegistrable();

		if(bOk == TRUE)
		{
			hErr = umma_fs_svc_PvrCheckRegisteredPvr(eDevIdx);

			if (hErr != ERR_OK)
			{
				umma_fs_hdd_SetNewDetachableHDD(TRUE);
				umma_fs_svc_WritePvrRegisterInfoToHDD(eDevIdx, INT2NDPVR_PARTIDX);
			}

		    hErr = umma_fs_svc_PvrCheckDeviceIsExtPvrPaired (eDevIdx, FALSE, UMMA_FS_EXT_PVR_PART_IDX);

			if(ERR_OK == hErr)
			{
				//mmc가 Format되었다는 전재로 일단 필요한 Directory를 만들도록 한다
				//기본 mmc storage관련 정책(Format 등등 ) 확정되면 아래 코드는  삭제될 수도 있음
				hErr = umma_fs_svc_DirMakeDefaultDirectory(eDevIdx, UMMA_FS_EXT_PVR_PART_IDX);
				//Ext HDD의 partition은 1개다. 따라서 0으로 내려준다. 추후 바뀌면 수정필요함
				(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_PVR_ENABLED, (HINT32)eDevIdx, (HINT32)0, (HINT32)ulPvrIdx);
			}

		}
	}
	else if(ePvrType & eFS_PVR_Tsr)
	{
		HxLOG_Debug("TSR Only Device..\n");
		umma_fs_svc_WritePvrRegisterInfoToHDD(eDevIdx, 0);
		hErr = umma_fs_svc_DirMakeDefaultDirectory(eDevIdx, 0);

		// This is not PVR device. But set true to bPvrFlag for internal purpose
		if (pstDevice->pstPartArray != NULL)
			pstDevice->pstPartArray[0].bPvrFlag = TRUE;
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_TSR_ENABLED, (HINT32)eDevIdx, (HINT32)0, (HINT32)0);
	}
	else
	{
		// eFS_PVR_None
		HxLOG_Debug("No PVR Device..\n");
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


static void umma_fs_mmc_PalEventCb(PalMmc_Event_e dEvent, HINT32 dPort, HCHAR* pszName)
{
	HxLOG_Print(HxANSI_COLOR_YELLOW("FS-MMC)Event[%d], name[%s] port[%d]\n"),dEvent,((pszName)?pszName:"NULL"),dPort);
	SVC_FS_SEM_GET;
	switch (dEvent)
	{
	case ePAL_MMC_EVT_CONNECTED:
		umma_fs_mmc_AddDevice(dPort,pszName);
		break;
	case ePAL_MMC_EVT_DISCONNECTED:
		umma_fs_mmc_RemoveDevice();
		break;
	case ePAL_MMC_EVT_FORMAT_DONE:
		break;
	case ePAL_MMC_EVT_FORMAT_FAIL:
		HxLOG_Error("ePAL_MMC_EVT_FORMAT_FAIL error\n\n");
		break;
	case ePAL_MMC_EVT_MOUNT_DONE:
		umma_fs_mmc_SendConnectMessage();
		break;
	case ePAL_MMC_EVT_MOUNT_FAIL:
		HxLOG_Error("ePAL_MMC_EVT_MOUNT_FAIL error\n\n");
		break;
	case ePAL_MMC_EVT_UMOUNT_DONE:
		HxLOG_Print(HxANSI_COLOR_YELLOW("\nePAL_MMC_EVT_UMOUNT_DONE \n\n"));
		#if defined(MMC_RETRY_UMOUNT)
		{
			HERROR			hErr;
			UMMA_FS_DevContext_t	*pstDevice;
			SvcFs_MmcContents_t *pContents = umma_fs_mmc_GetContents();
			hErr = umma_fs_dev_GetDeviceInfo (pContents->DeviceID, &pstDevice);
			if (hErr == ERR_OK && pstDevice != NULL)
			{
				umma_fs_dev_InitFsDeviceInfo (pstDevice);

				(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_DISCONNECTED, (HINT32)pContents->DeviceID, 0, 0);
			}
		}
		#endif
		break;
	case ePAL_MMC_EVT_UMOUNT_FAIL:
		HxLOG_Error("ePAL_MMC_EVT_UMOUNT_FAIL error\n\n");
		break;
	default:
		HxLOG_Critical("Critical Error!!!\n\n");
		break;
	}
	SVC_FS_SEM_RELEASE;
}



/**************************************************************************************************/
#define _________FS_MMC_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________FS_MMC_Public_Functions__________________________________________________________
/**************************************************************************************************/
HERROR umma_fs_mmc_Init (void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
	HxLOG_Trace();

	PAL_MMC_Initialize();
	PAL_MMC_RegisterNotifier (umma_fs_mmc_PalEventCb);
	PAL_MMC_PowerOn();

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


// 포맷 이후에, umount 된 device 를 다시 붙이는 용도
HERROR umma_fs_mmc_MountDevice (void)
{
	SvcFs_MmcContents_t *pContents = umma_fs_mmc_GetContents();
	HxLOG_Trace();

	return umma_fs_mmc_AddDevice(pContents->dPort,pContents->pszDevName);
}

// safe remove 와 disconect 이벤트이후 connect false 로 appkit 까지 올라갔다가, umount 명령으로 내려온 경우다.
HERROR umma_fs_mmc_UnMountDevice (UMMA_FS_DevIdx_e eDevIdx, HBOOL bSafeRemove)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
	UMMA_FS_DevContext_t			*pstDevice;
	HERROR					 hErr;
	SvcFs_MmcContents_t 		*pContents = umma_fs_mmc_GetContents();
	//HBOOL 				bSafeRemove = FALSE;
	HxLOG_Trace();

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("\numma_fs_dev_GetDeviceInfo Error!!!\n\n");
		return ERR_FAIL;
	}

	hErr = PAL_MMC_UnMount(pContents->dPort);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\nPAL_MMC_UnMount Error!!!\n\n");
		return ERR_FAIL;
	}

	umma_fs_dev_InitFsDeviceInfo (pstDevice);

	if (bSafeRemove)
	{
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_DISCONNECTED, (HINT32)eDevIdx, 0, 0);
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


HERROR umma_fs_mmc_Format(void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
//	UMMA_FS_DevContext_t		*pstDevice = NULL;
//	HERROR 				hErr = ERR_OK;
	SvcFs_MmcContents_t 	*pContents = umma_fs_mmc_GetContents();


	return PAL_MMC_Format(pContents->dPort);
#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_mmc_PowerOn (void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
	return PAL_MMC_PowerOn();
#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_mmc_PowerOff (void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
	return PAL_MMC_PowerOff();
#else
	return ERR_FAIL;
#endif
}


/**************************************************************************************************/
#define _________FS_MMC_CMD_FUNCTION_________________________________________________________
/**************************************************************************************************/
#ifdef	CONFIG_DEBUG

#endif


/**************************************************************************************************/
#define _________FS_MMC_Public_Proc____________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________FS_MMC_End_Of_File____________________________________________________
/**************************************************************************************************/

