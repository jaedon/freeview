/**
	@file     fs_device.c
	@brief    repository service	\n
			  This implements rec/pb & media(photo/music) services.	\n

	Description: 		\n
	Module: mw/pvr			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <umma_fs_pal_fs.h>
#include <umma_fs_pal_hdd.h>
#include <umma_fs_pal_usb.h>
#include <umma_fs_pal_mmc.h>

#include <dlib.h>
#include <vkernel.h>
#include <umma_int.h>

#include "umma_fs_int.h"
#include "drv_hotplug.h"
#include "se_uapi.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
unsigned long		 g_ulFsDevice_SemId;

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#undef	COMMAND_NAME
#define	COMMAND_NAME			OCTO_SHELL_NAME

#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
#define DEVICE_MAPPER_NAME		"encrypted-hd"
#define VOLUME_NAME 			"encrypted-volume"
#define PASSWORD_NAME			"a1234567890"
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC UMMA_FS_DevInfoMap_t		 s_stFsDevice_Context = { 0, };

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
STATIC void umma_fs_svc_CmdInit(void);
#endif

HERROR umma_fs_svc_GetDeviceInfo (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_DevInfo_t *pstMwDevInfo);

#define _____LOCAL_FUNCTIONS_____

STATIC HERROR umma_fs_dev_GetDeviceVolumeName (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx, UMMA_FS_FsType_e eFsType, HUINT8 *szVolumeName)
{
	PAL_FS_StorDevIdx_t	 ePalDevIdx;
	HERROR				 hErr = ERR_FAIL;

	if (szVolumeName == NULL)				{ return ERR_FAIL; }

	VK_memset (szVolumeName, 0, UMMA_FS_VOLUME_LABEL_SIZE);

	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);

	switch (eFsType)
	{
	case eFS_FileSystem_EXT2:
	case eFS_FileSystem_EXT3:
	case eFS_FileSystem_EXT4:
	case eFS_FileSystem_FAT16:
	case eFS_FileSystem_FAT32:
	case eFS_FileSystem_NTFS:
		hErr = PAL_FS_GetVolumeName(eDevIdx, ulPartIdx, szVolumeName, UMMA_FS_VOLUME_LABEL_SIZE);
		szVolumeName[UMMA_FS_VOLUME_LABEL_SIZE-1] = '\0';
		break;

	case eFS_FileSystem_XFS:
		// DI 함수가 완성될 때까지 가짜 함수를 사용함.
		HxSTD_PrintToStrN (szVolumeName, UMMA_FS_VOLUME_LABEL_SIZE, "Dev%02d_%02d", eDevIdx, ulPartIdx);
		hErr = ERR_OK;
		break;
	default:
#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
		hErr = PAL_FS_USB_GetModelName (ePalDevIdx, szVolumeName);
#endif
		break;
	}

	if (szVolumeName[0] == '\0')
	{
		HxSTD_PrintToStrN (szVolumeName, UMMA_FS_VOLUME_LABEL_SIZE, "Dev%02d_%02d", eDevIdx, ulPartIdx);
	}

	return (hErr == ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR umma_fs_dev_EraseAllDeviceInfo(void)
{
	HERROR					 hErr;
	UMMA_FS_DevContext_t		*pstDevice;
	HUINT32 				ulIdx=0;

	HxLOG_Warning("eFS_SIGNAL_UNMOUNT_ALL !! \n");

	for (ulIdx = eFS_Dev_sda; ulIdx < eFS_Dev_MAX; ulIdx++)
	{
		hErr = umma_fs_dev_GetDeviceInfo(ulIdx, &pstDevice);
		if (hErr != ERR_OK) 						{ continue; }

		umma_fs_dev_InitFsDeviceInfo(pstDevice);
	}

	return ERR_OK;
}

STATIC HERROR umma_fs_dev_ErasePartInfo(UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx)
{
	HBOOL					 bDevMounted = FALSE;
	HUINT32					 ulCount;
	UMMA_FS_DevContext_t				*pstDevice;
	UMMA_FS_PartitionInfo_t		*pstPartInfo;
	HERROR					 hErr;
	HBOOL		bPvr = FALSE, bDefaultPvr = FALSE;
	HUINT32		ulRemovePvrIdx = 0xFFFF;
	UMMA_FS_DevType_e 		 eDevType;
	HUINT8				szMountPath[UMMA_FS_MOUNT_PATH_LEN];

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK)							{ return ERR_FAIL; }
	if (ulPartIdx >= pstDevice->ulNumPartition)		{ return ERR_FAIL; }

	pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);
	eDevType	 = pstDevice->eDevType;

	if(pstPartInfo->bPvrFlag == TRUE)
	{
		bPvr = TRUE;
		ulRemovePvrIdx = pstPartInfo->ulPvrIdx;
		bDefaultPvr = pstPartInfo->bDefaultPvr;
	}

	// Partition 내 mount 정보는 다 지운다.
	pstPartInfo->bMounted		= FALSE;
	pstPartInfo->bPvrFlag		= FALSE;
	pstPartInfo->bSystemFlag	= FALSE;
	pstPartInfo->bWritable		= FALSE;
	pstPartInfo->ulDriveNo		= 0;
	pstPartInfo->bDefaultPvr 	= FALSE;

	pstPartInfo = NULL;
	// 해당 Device 내의 모든 Partition이 unmount 되었는지 체크
	for (ulCount = 0; ulCount < pstDevice->ulNumPartition; ulCount ++)
	{
		pstPartInfo = &(pstDevice->pstPartArray[ulCount]);
		if (pstPartInfo->bAvailable == TRUE && pstPartInfo->bMounted == TRUE)
		{
			bDevMounted = TRUE;
			break;
		}
	}

	pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);
	HLIB_STD_StrNCpySafe(szMountPath, pstPartInfo->szMountPath, UMMA_FS_MOUNT_PATH_LEN);

	if (pstPartInfo && pstPartInfo->ulPlaybackIdx != DxSTORAGE_INVALID_PVR_INDEX)
	{
		hErr = umma_fs_svc_PvrReleasePlaybackIdx(pstPartInfo->ulPlaybackIdx);
		if (hErr == ERR_OK)
		{
			(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_PLAYBACK_DISABLED, (HINT32)eDevIdx, 0, pstPartInfo->ulPlaybackIdx);
		}
	}

	if (bDevMounted == FALSE)
	{
		pstDevice->bDevMounted = FALSE;
		if (pstDevice->bDevConnected == FALSE)
		{
			umma_fs_dev_InitFsDeviceInfo (pstDevice);
			pstPartInfo = NULL;
		}
		else
		{
			 //안전 제거 표시
			pstDevice->eDevStatus = eFS_DevStatus_SafeRemoved;
		}
	}

	if (bPvr == TRUE)
	{
		HUINT32 				 ulPvrIdx, ulCount = 0;
		if(eDevType == eFS_DevType_UsbHdd)
		{
			(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_PVR_DISABLED, (HINT32)eDevIdx, 0, ulRemovePvrIdx);
		}
		else 	if (pstDevice->eDevType == eFS_DevType_Detachable && pstDevice->eDevStatus == eFS_DevStatus_SafeRemoved)
		{
			// safe remove인경우의 처리
			(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_DISCONNECTED, (HINT32)eDevIdx, 0, ulRemovePvrIdx);
		}

		umma_fs_svc_PvrReleasePvrIdx(ulRemovePvrIdx);

		//제거된 HDD가 default PVR HDD였다면 남은 연결된 HDD 중에서 default 를 선택한다.
		if(bDefaultPvr == TRUE)
		{
			umma_fs_svc_PvrGetConnectedPvrStorageCount(&ulCount);
			if(ulCount != 0)	// 남은게 없으면 진행X
			{
				hErr = umma_fs_svc_PvrSelectNewDefaultPvrIdx(&ulPvrIdx);
				if(hErr == ERR_OK)
				{
					umma_fs_svc_PvrSetPvrDefaultHdd(ulPvrIdx,FALSE);
				}
			}
		}
	}

	return ERR_OK;
}

#define _____GLOBAL_FUNCTIONS_____
static UxSendEvtCallback_t	s_pfFsEvtCb;
static UxSendDataCallback_t	s_pfFsDataCb;

HERROR umma_fs_svc_SendEventToMainloop(HINT32 nEvt, HINT32 nP1, HINT32 nP2, HINT32 nP3)
{
	if (s_pfFsEvtCb)
	{
		s_pfFsEvtCb (nEvt, nP1, nP2, nP3);
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR umma_fs_svc_SendDataToMainloop (HINT32 nEvt, void *nP1, HINT32 nP1Size, void *nP2, HINT32 nP2Size, HINT32 nP3)
{
	if (s_pfFsDataCb)
	{
		s_pfFsDataCb (nEvt, nP1, nP1Size, nP2, nP2Size, nP3);
		return ERR_OK;
	}

	return ERR_FAIL;
}

void	umma_fs_svc_registerNotifier(UxSendEvtCallback_t pfCb)
{
	if (pfCb)
	{
		s_pfFsEvtCb = pfCb;
	}
}
void	umma_fs_svc_registerDataNotifier(UxSendDataCallback_t pfCb)
{
	if (pfCb)
	{
		s_pfFsDataCb = pfCb;
	}
}

HERROR umma_fs_svc_GetPartitionInfoArray (UMMA_FS_DevIdx_e eDevIdx, HUINT32 *pulPartNum, UMMA_FS_PartitionInfo_t **ppstPartArray)
{
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	SVC_FS_SEM_GET;

	if (pulPartNum != NULL) 						{ *pulPartNum = 0; }
	if (ppstPartArray != NULL)						{ *ppstPartArray = NULL; }

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)		{ goto END_FNC; }

	if (pstDevice->ulNumPartition == 0 || pstDevice->pstPartArray == NULL)
	{
		goto END_FNC;
	}

	if (ppstPartArray != NULL)
	{
		*ppstPartArray = (UMMA_FS_PartitionInfo_t *)UMMA_Malloc (sizeof(UMMA_FS_PartitionInfo_t) * pstDevice->ulNumPartition);
		if (*ppstPartArray == NULL)					{ goto END_FNC; }

		VK_memcpy (*ppstPartArray, pstDevice->pstPartArray, sizeof(UMMA_FS_PartitionInfo_t) * pstDevice->ulNumPartition);
	}

	if (pulPartNum != NULL)
	{
		*pulPartNum = pstDevice->ulNumPartition;
	}

	hResult = ERR_OK;

END_FNC:
	SVC_FS_SEM_RELEASE;
	return hResult;
}

HERROR umma_fs_svc_GetDeviceInfo (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_DevInfo_t *pstMwDevInfo)
{
	UMMA_FS_DevContext_t	*pstDevInfo;
	HERROR			 hErr, hResult = ERR_FAIL;

	if (pstMwDevInfo == NULL)	{ goto END_FNC; }

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevInfo);
	if (hErr != ERR_OK) 		{ goto END_FNC; }

	VK_memset (pstMwDevInfo, 0, sizeof(UMMA_FS_DevInfo_t));

	pstMwDevInfo->eDevIdx			= pstDevInfo->eDevIdx;
	pstMwDevInfo->eDevType			= pstDevInfo->eDevType;
	pstMwDevInfo->eDevStatus		= pstDevInfo->eDevStatus;
	pstMwDevInfo->eDevSubStatus		= pstDevInfo->eDevSubStatus;
	pstMwDevInfo->eDevUsbSpeed		= pstDevInfo->eDevUsbSpeed;
	pstMwDevInfo->bConnected		= pstDevInfo->bDevConnected;
	pstMwDevInfo->bMounted			= pstDevInfo->bDevMounted;
	pstMwDevInfo->eFsType			= pstDevInfo->eFsType;
	pstMwDevInfo->bWriteProtected	= pstDevInfo->bWriteProtected;
	pstMwDevInfo->ulPortNum		= pstDevInfo->ulPortNum;

	umma_fs_dev_GetDrivePath (eDevIdx, pstMwDevInfo->szMountPath);
	VK_memcpy (pstMwDevInfo->szLabel, pstDevInfo->szLabel, UMMA_FS_VOLUME_LABEL_SIZE);
	pstMwDevInfo->szLabel[UMMA_FS_VOLUME_LABEL_SIZE-1] = '\0';
	pstMwDevInfo->ulNumPartition	= pstDevInfo->ulNumPartition;

	HxLOG_Print("[%s (%04d)] pstDevInfo->ulNumPartition [%ld] >>>\n", __FUNCTION__, __LINE__, pstDevInfo->ulNumPartition);

	pstMwDevInfo->ullTotalSizeKb	= pstDevInfo->ullDevSizeKb;

	hResult = ERR_OK;

END_FNC:

	return hResult;
}



HERROR umma_fs_dev_Init (void)
{
	HUINT32				 ulIndex;
	UMMA_FS_DevInfoMap_t		*pstContext;
	HERROR				 hErr;

	pstContext = umma_fs_dev_GetContext();
	VK_memset (pstContext, 0, sizeof(UMMA_FS_DevInfoMap_t));

	for (ulIndex = 0; ulIndex < eFS_Dev_MAX; ulIndex++)
	{
		umma_fs_dev_InitFsDeviceInfo (&(pstContext->astDevice[ulIndex]));
	}

	hErr = VK_SEM_Create (&g_ulFsDevice_SemId, "fsdevsem", VK_SUSPENDTYPE_PRIORITY);
	if(hErr != VK_OK)	HxLOG_Critical("Critical Eror!!!\n");

#if defined(CONFIG_DEBUG)
	umma_fs_svc_CmdInit();
#endif

	return ERR_OK;
}

UMMA_FS_DevInfoMap_t *umma_fs_dev_GetContext (void)
{
	return &s_stFsDevice_Context;
}

UMMA_FS_DevIdx_e umma_fs_dev_ConvertPalDevIdxToMwDevIdx (PAL_FS_StorDevIdx_t ePalIdx)
{
	if (ePalIdx >= ePAL_FS_DEV_sda && ePalIdx <= PAL_FS_DEV_IDX_END)
	{
		return (UMMA_FS_DevIdx_e)(ePalIdx - ePAL_FS_DEV_sda + eFS_Dev_sda);
	}

	return eFS_Dev_NULL;
}

PAL_FS_StorDevIdx_t umma_fs_dev_ConvertMwDevIdxToPalDevIdx (UMMA_FS_DevIdx_e eMwIdx)
{
	if (eMwIdx >= eFS_Dev_sda && eMwIdx < eFS_Dev_MAX)
	{
		return (PAL_FS_StorDevIdx_t)(eMwIdx - eFS_Dev_sda + ePAL_FS_DEV_sda);
	}

	return ePAL_FS_DEV_MAX;
}

UMMA_FS_FsType_e umma_fs_dev_ConvertPalFsTypeToMwFsType (PAL_FS_FsType_t ePalFsType)
{
	switch (ePalFsType)
	{
	case PAL_FS_EXT2_FILESYSTEM:
		return eFS_FileSystem_EXT2;

	case PAL_FS_EXT3_FILESYSTEM:
		return eFS_FileSystem_EXT3;

	case PAL_FS_EXT4_FILESYSTEM:
		return eFS_FileSystem_EXT4;

	case PAL_FS_XFS_FILESYSTEM:
		return eFS_FileSystem_XFS;

	case PAL_FS_FAT16_FILESYSTEM:
		return eFS_FileSystem_FAT16;

	case PAL_FS_FAT32_FILESYSTEM:
		return eFS_FileSystem_FAT32;

	case PAL_FS_NTFS_FILESYSTEM:
		return eFS_FileSystem_NTFS;

	case PAL_FS_MSDOS_FILESYSTEM:
		return eFS_FileSystem_DOS;

	case PAL_FS_EXFAT_FILESYSTEM:
		return eFS_FileSystem_EXFAT;

	default:
		return eFS_FileSystem_OTHER;
	}
}

PAL_FS_FsType_t umma_fs_dev_ConvertMwFsTypeToPalFsType (UMMA_FS_FsType_e eMwFsType)
{
	switch (eMwFsType)
	{
	case eFS_FileSystem_EXT2:
		return PAL_FS_EXT2_FILESYSTEM;

	case eFS_FileSystem_EXT3:
		return PAL_FS_EXT3_FILESYSTEM;

	case eFS_FileSystem_EXT4:
		return PAL_FS_EXT4_FILESYSTEM;

	case eFS_FileSystem_XFS:
		return PAL_FS_XFS_FILESYSTEM;

	case eFS_FileSystem_FAT16:
		return PAL_FS_FAT16_FILESYSTEM;

	case eFS_FileSystem_FAT32:
		return PAL_FS_FAT32_FILESYSTEM;

	case eFS_FileSystem_NTFS:
		return PAL_FS_NTFS_FILESYSTEM;

	case eFS_FileSystem_DOS:
		return PAL_FS_MSDOS_FILESYSTEM;

	case eFS_FileSystem_EXFAT:
		return PAL_FS_EXFAT_FILESYSTEM;

	case eFS_FileSystem_NONE:
	case eFS_FileSystem_OTHER:
	default:
		return PAL_FS_OTHER_FILESYSTEM;
	}
}

HBOOL umma_fs_dev_IsFsTypeWritable (UMMA_FS_FsType_e eFsType)
{
	// File System에 따라 writable 한지에 대한 구분.
	// SVC_PVR_IsStorageWritable 함수에서 구현한 것에 따라 NTFS와 OTHER_FS 에서는 File Write 가 불가능한 걸로 간주시킨다.
	switch (eFsType)
	{
	case eFS_FileSystem_NONE:
	case eFS_FileSystem_NTFS:
	case eFS_FileSystem_OTHER:
		return FALSE;

	default:
		break;
	}

	return TRUE;
}

// path 명 뒤에 separator (/) 가 없으면 붙여주는 함수.
// ( /mnt/hd2 -> /mnt/hd2/ )
HERROR umma_fs_dev_AppendSeparatorToPath (HUINT8 *szPath)
{
	HUINT32			 ulPathLen;

	if (szPath == NULL)					{ return ERR_FAIL; }

	ulPathLen = VK_strlen (szPath);
	if (ulPathLen > 0 && szPath[ulPathLen - 1] != FS_PATH_SEPARATOR)
	{
		szPath[ulPathLen] = FS_PATH_SEPARATOR;
		szPath[ulPathLen + 1] = '\0';
	}
	else if (ulPathLen == 0)
	{
		szPath[ulPathLen] = FS_PATH_SEPARATOR;
		szPath[ulPathLen + 1] = '\0';
	}

	return ERR_OK;
}

// path 명 뒤에 separator (/) 가 있으면 떼어주는 함수.
// ( /mnt/hd2/ -> /mnt/hd2 )
HERROR umma_fs_dev_RemoveSeparatorFromPath (HUINT8 *szPath)
{
	HUINT32			 ulPathLen;

	if (szPath == NULL)					{ return ERR_FAIL; }

	ulPathLen = VK_strlen (szPath);
	if (ulPathLen > 0)
	{
		if (szPath[ulPathLen - 1] == FS_PATH_SEPARATOR)
		{
			szPath[ulPathLen - 1] = '\0';
		}
	}

	return ERR_OK;
}

HERROR umma_fs_dev_MakeMountPathByDriveNo (HUINT32 ulDriveNo, HUINT8 *pszPath)
{
	if (pszPath == NULL)						{ return ERR_FAIL; }

	// Drive Number 가 0 이면 /media/drive1 로 들어간다.
	HxSTD_PrintToStrN (pszPath, 32, "%s%s%d", PAL_FS_EXT_STORAGE_MOUNT_PATH, PAL_FS_EXT_STOR_INTER_PATH, ulDriveNo + 1);
	return ERR_OK;
}

HERROR umma_fs_dev_GetDriveNoFromMountPath (HUINT8 *szPath, HUINT32 *pulDriveNo)
{
	HINT32		 nDriveNo;
	HUINT32		 ulNumStrLen, ulExtStorLen;
	HUINT8		*szNumString;

	if (szPath == NULL || pulDriveNo == NULL)				{ return ERR_FAIL; }

	szNumString = strstr (szPath, PAL_FS_EXT_STOR_INTER_PATH);
	if (szNumString == NULL)
	{
		return ERR_FAIL;
	}

	ulNumStrLen = VK_strlen (szNumString);
	ulExtStorLen = VK_strlen (PAL_FS_EXT_STOR_INTER_PATH);
	if (ulNumStrLen <= ulExtStorLen)
	{
		return ERR_FAIL;
	}

	szNumString += ulExtStorLen;

	nDriveNo = (HUINT32)atoi (szNumString);
	if (nDriveNo <= 0)
	{
		return ERR_FAIL;
	}

	*pulDriveNo = (HUINT32)(nDriveNo - 1);
	return ERR_OK;
}


HERROR umma_fs_dev_InitFsDeviceInfo (UMMA_FS_DevContext_t *pstDevice)
{
	if (pstDevice != NULL)
	{
		if (pstDevice->pstPartArray != NULL)
		{
			UMMA_Free (pstDevice->pstPartArray);
		}

		VK_memset (pstDevice, 0, sizeof(UMMA_FS_DevContext_t));

		pstDevice->eDevIdx			= eFS_Dev_NULL;
		pstDevice->bDevAvailable	= FALSE;
		pstDevice->eDevStatus		= eFS_DevStatus_NoDevice;
		pstDevice->eDevSubStatus	= eFS_DevSubStatus_None;

		pstDevice->ullDevSizeKb		= 0;

		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR umma_fs_dev_ClearMountInfoInFsDeviceInfo (UMMA_FS_DevContext_t *pstDevice)
{
	if (pstDevice != NULL)
	{
		pstDevice->bDevMounted		= FALSE;

		pstDevice->ulNumPartition	= 0;
		pstDevice->ulMaxPartition	= 0;
		if (pstDevice->pstPartArray != NULL)
		{
			UMMA_Free (pstDevice->pstPartArray);
			pstDevice->pstPartArray = NULL;
		}

		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR umma_fs_dev_FindEmptyDeviceInfoSlot (UMMA_FS_DevContext_t **ppstDevice)
{
	HUINT32				 ulDevCnt;
	UMMA_FS_DevInfoMap_t		*pstContext;
	UMMA_FS_DevContext_t		*pstDevInfo;


	if (ppstDevice == NULL)			{ return ERR_FAIL; }

	pstContext = umma_fs_dev_GetContext();

	// Find the empty slot
	for (ulDevCnt = 0; ulDevCnt < eFS_Dev_MAX; ulDevCnt++)
	{
		pstDevInfo = &(pstContext->astDevice[ulDevCnt]);

		if (pstDevInfo->bDevAvailable != TRUE && pstDevInfo->eDevStatus != eFS_DevStatus_SafeRemoved)
		{
			*ppstDevice = pstDevInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR umma_fs_dev_GetNewDriveNo (HUINT32 *pulNewDriveNo)
{
	HUINT32				 ulDevCnt, ulPartCnt;
	HUINT32				 ulNewDriveNo;
	UMMA_FS_DevInfoMap_t		*pstContext;
	UMMA_FS_DevContext_t		*pstDevInfo;

	if (pulNewDriveNo == NULL)					{ return ERR_FAIL; }

	pstContext = umma_fs_dev_GetContext();

	// Find the new drive number : the existing driver number shall be skipped.
	ulNewDriveNo = 0;
	for (ulDevCnt = 0; ulDevCnt < eFS_Dev_MAX; ulDevCnt++)
	{
		pstDevInfo = &(pstContext->astDevice[ulDevCnt]);

		if (pstDevInfo->bDevAvailable != TRUE || pstDevInfo->eDevStatus == eFS_DevStatus_SafeRemoved)		{ continue; }

		// Drive Number가 할당되는 외장형일 경우에만 Count시키도록 해야 한다.
		if (pstDevInfo->eDevType == eFS_DevType_UsbHdd || pstDevInfo->eDevType == eFS_DevType_UsbMem || pstDevInfo->eDevType == eFS_DevType_eSataHdd)
		{
			for (ulPartCnt = 0; ulPartCnt < pstDevInfo->ulNumPartition; ulPartCnt++)
			{
				if (pstDevInfo->pstPartArray[ulPartCnt].bMounted == TRUE)
				{
					if (pstDevInfo->pstPartArray[ulPartCnt].ulDriveNo >= ulNewDriveNo)
					{
						ulNewDriveNo = pstDevInfo->pstPartArray[ulPartCnt].ulDriveNo + 1;
					}
				}
			}
		}
	}

	*pulNewDriveNo = ulNewDriveNo;
	return ERR_OK;
}

HBOOL umma_fs_dev_IsDeviceExist (UMMA_FS_DevIdx_e eDevIdx)
{
	HINT32				 nIndex;
	UMMA_FS_DevInfoMap_t		*pstContext;
	UMMA_FS_DevContext_t		*pstSrcDevInfo;


	pstContext = umma_fs_dev_GetContext();

	// Check if the device is already mounted
	for (nIndex = 0; nIndex < eFS_Dev_MAX; nIndex++)
	{
		pstSrcDevInfo = &(pstContext->astDevice[nIndex]);

		HxLOG_Info("pstSrcDevInfo->bDevAvailable [%d], pstSrcDevInfo->eDevIdx [%d], eDevIdx [%d] >>>\n", pstSrcDevInfo->bDevAvailable, pstSrcDevInfo->eDevIdx, eDevIdx);
		if (pstSrcDevInfo->bDevAvailable == TRUE && pstSrcDevInfo->eDevIdx == eDevIdx)
		{
			return TRUE;
		}
	}

	HxLOG_Info("[umma_fs_dev_GetDeviceInfo] No Device Info (%d)\n", eDevIdx);
	return FALSE;
}


HERROR umma_fs_dev_GetDeviceInfo (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_DevContext_t **ppstDevice)
{
	HINT32				 nIndex;
	UMMA_FS_DevInfoMap_t		*pstContext;
	UMMA_FS_DevContext_t		*pstSrcDevInfo;

	if (ppstDevice == NULL)			{ return ERR_FAIL; }

	pstContext = umma_fs_dev_GetContext();

	// Check if the device is already mounted
	for (nIndex = 0; nIndex < eFS_Dev_MAX; nIndex++)
	{
		pstSrcDevInfo = &(pstContext->astDevice[nIndex]);

		HxLOG_Info("pstSrcDevInfo->bDevAvailable [%d], pstSrcDevInfo->eDevIdx [%d], eDevIdx [%d] >>>\n", pstSrcDevInfo->bDevAvailable, pstSrcDevInfo->eDevIdx, eDevIdx);
		if (pstSrcDevInfo->bDevAvailable == TRUE && pstSrcDevInfo->eDevIdx == eDevIdx)
		{
			*ppstDevice = pstSrcDevInfo;
			return ERR_OK;
		}
	}

	HxLOG_Warning("[umma_fs_dev_GetDeviceInfo] No Device Info (%d)\n", eDevIdx);
	return ERR_FAIL;
}

HERROR umma_fs_dev_GetDeviceIndexArray (HUINT32 *pulDeviceNum, UMMA_FS_DevIdx_e **ppeIdxArray)
{
	HUINT32				 ulIndex, ulDeviceNum = 0;
	UMMA_FS_DevIdx_e		 aeIdxArray[eFS_Dev_MAX];
	UMMA_FS_DevInfoMap_t		*pstContext;

	if (pulDeviceNum == NULL || ppeIdxArray == NULL)			{ return ERR_FAIL; }

	pstContext = umma_fs_dev_GetContext();

	// Check if the device is already mounted
	for (ulDeviceNum = 0, ulIndex = eFS_Dev_sda; ulIndex < eFS_Dev_MAX; ulIndex++)
	{
		if (pstContext->astDevice[ulIndex].bDevAvailable == TRUE && pstContext->astDevice[ulIndex].eDevStatus != eFS_DevStatus_SafeRemoved)
		{
			aeIdxArray[ulDeviceNum] = pstContext->astDevice[ulIndex].eDevIdx;
			ulDeviceNum ++;
		}
	}

	if (ulDeviceNum == 0)
	{
		HxLOG_Debug("no storage device\n");
		return ERR_NO_DEVICE;
	}

	*ppeIdxArray = (UMMA_FS_DevIdx_e *)UMMA_Malloc (sizeof(UMMA_FS_DevIdx_e) * ulDeviceNum);
	if (*ppeIdxArray == NULL)			{ return ERR_FAIL; }

	*pulDeviceNum = ulDeviceNum;
	VK_memcpy (*ppeIdxArray, aeIdxArray, sizeof(UMMA_FS_DevIdx_e) * ulDeviceNum);

	return ERR_OK;
}

HERROR umma_fs_dev_FreeDeviceIndexArray (UMMA_FS_DevIdx_e *peIdxArray)
{
	if (peIdxArray == NULL)			{ return ERR_FAIL; }

	UMMA_Free (peIdxArray);
	return ERR_OK;
}

HUINT32 umma_fs_dev_CountMountedDevice (UMMA_FS_DevType_e eTypeFlag)
{
	HINT32				 nIndex;
	HUINT32				 ulCount;
	UMMA_FS_DevInfoMap_t		*pstContext;

	ulCount = 0;
	pstContext = umma_fs_dev_GetContext();

	if (eTypeFlag == 0x00)				{ eTypeFlag = eFS_DevType_ALL; }

	// Check if the device is already mounted
	for (nIndex = 0; nIndex < eFS_Dev_MAX; nIndex++)
	{
		if ((pstContext->astDevice[nIndex].bDevAvailable == TRUE) &&
			(pstContext->astDevice[nIndex].eDevStatus != eFS_DevStatus_SafeRemoved) &&
			((pstContext->astDevice[nIndex].eDevType & eTypeFlag) != 0))
		{
			ulCount ++;
		}
	}

	return ulCount;
}

HERROR umma_fs_dev_GetDrivePath (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szDrivePath)
{
	HINT32			 nCount;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR			 hErr;
	HUINT32		ulMountPartition = 0xFF;

	if (szDrivePath == NULL)			{ return ERR_FAIL; }

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("[umma_fs_dev_GetDrivePath] umma_fs_dev_GetDeviceInfo eDevIdx(%d) err(0x%08x)\n", eDevIdx, hErr);
		return ERR_FAIL;
	}

	if (pstDevice->ulNumPartition == 0)
	{
		HxLOG_Print("[umma_fs_dev_GetDrivePath:%d] No partition mounted in eDevIdx(%d)\n", __LINE__, eDevIdx);
		return ERR_FAIL;
	}

	// PVR Partition을 찾는다.
	// 없으면 첫번째 mount Partition을 반환한다.
	for (nCount = 0; nCount < pstDevice->ulNumPartition; nCount++)
	{
		if (pstDevice->pstPartArray[nCount].bMounted == TRUE)
		{
			ulMountPartition = nCount;
			if (pstDevice->pstPartArray[nCount].bPvrFlag == TRUE)
			{
				VK_strncpy (szDrivePath, pstDevice->pstPartArray[nCount].szMountPath, UMMA_FS_MOUNT_PATH_LEN);
				return ERR_OK;
			}
		}
	}

	if (ulMountPartition != 0xFF)
	{
		VK_strncpy (szDrivePath, pstDevice->pstPartArray[ulMountPartition].szMountPath, UMMA_FS_MOUNT_PATH_LEN);
		return ERR_OK;
	}

	HxLOG_Error("[umma_fs_dev_GetDrivePath:%d] No partition mounted in eDevIdx(%d)\n", __LINE__, eDevIdx);
	return ERR_FAIL;
}

HBOOL umma_fs_dev_CheckPathIsValid (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szPath)
{
	HUINT32			 ulIndex;
	HUINT32			 ulPathLen, ulMountLen;
	HUINT8			*szSrcPath = NULL, *szMountPath = NULL;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR			 hErr;
	HBOOL			 bResult = FALSE;

	if (szPath == NULL)			{ return FALSE; }

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL || pstDevice->pstPartArray == NULL)
	{
		HxLOG_Error("[umma_fs_dev_CheckPathIsValid] umma_fs_dev_GetDeviceInfo eDevIdx(%d) err(0x%08x)\n", eDevIdx, hErr);
		goto ERROR;
	}

	// separator를 붙여서 비교해야 하므로 따로 여유있게 잡아 copy한 후 비교한다.
	szSrcPath	= UMMA_Malloc (HxSTD_StrLen(szPath) + 4);
	szMountPath	= UMMA_Malloc (UMMA_FS_MOUNT_PATH_LEN + 4);
	if (szSrcPath == NULL || szMountPath == NULL)
	{
		HxLOG_Error("[umma_fs_dev_CheckPathIsValid] mem alloc failed\n");
		goto ERROR;
	}

	// 비교를 위해 seperator (/)를 붙인다. szPath의 내용을 고치면 안되기 때문에 카피해서 쓴다.
	HxSTD_StrNCpy(szSrcPath, szPath, HxSTD_StrLen(szPath));
	umma_fs_dev_AppendSeparatorToPath(szSrcPath);
	ulPathLen = HxSTD_StrLen(szSrcPath);

	for (ulIndex = 0; ulIndex < pstDevice->ulNumPartition; ulIndex++)
	{
		HxSTD_StrNCpy(szMountPath, pstDevice->pstPartArray[ulIndex].szMountPath, UMMA_FS_MOUNT_PATH_LEN + 3);
		umma_fs_dev_AppendSeparatorToPath (szMountPath);
		ulMountLen = VK_strlen (szMountPath);

		// szPath쪽이 더 길어야 당연히 Mount된 것이 소속된다.
		// ex: mount : /mnt/hd2/ , szPath = /mnt/hd2/video/
		if (ulMountLen <= ulPathLen)
		{
			if (VK_strncmp (szSrcPath, szMountPath, ulMountLen) == 0)
			{
				bResult = TRUE;
				goto ERROR;
			}
		}
	}

	HxLOG_Print("[umma_fs_dev_CheckPathIsValid] Path (%s) not valid in Dev(%d) \n", szPath, eDevIdx);

ERROR:
	if (szSrcPath != NULL)				{ UMMA_Free (szSrcPath); }
	if (szMountPath != NULL)			{ UMMA_Free (szMountPath); }

	return bResult;
}

HERROR umma_fs_dev_GetDeviceFileFullName (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szFileName)
{
//	PAL_FS_StorDevIdx_t	 ePalDevIdx;
//	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);
	return PAL_FS_GetDeviceFileFullName (eDevIdx, szFileName);
}


HERROR umma_fs_dev_GetDeviceUuid (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szUuid)
{
	HUINT32				 ulCnt, ulUuidsize=0;
//	PAL_FS_StorDevIdx_t	 ePalDevIdx;
	UMMA_FS_DevContext_t		*pstDevice;
	HUINT8				 szPalUuid[UMMA_FS_DEV_UUID_BUFSIZE];
	HERROR				 hErr;
	HUINT32 			ulIdx=0;
	HUINT8 			*pCur=NULL;

	if (szUuid == NULL)			{ return ERR_FAIL; }

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_dev_GetDeviceUuid] No Device (%d) \n", eDevIdx);
		return ERR_FAIL;
	}

	// 구조체에 값이 있다면 단순히 복사하여 리턴
	if(pstDevice->szUUID[0] != 0x0)
	{
		VK_memcpy(szUuid, pstDevice->szUUID, UMMA_FS_DEV_UUID_BUFSIZE);
		return ERR_OK;
	}

//	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (pstDevice->eDevIdx);

	VK_memset (szPalUuid, 0, UMMA_FS_DEV_UUID_BUFSIZE);

	hErr = ERR_FAIL;

#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
	if (pstDevice->eDevType == eFS_DevType_MMC)
	{
		PalMmc_Info_e			stInfo;
		VK_memset (&stInfo, 0, sizeof(PalMmc_Info_e));

		hErr = PAL_MMC_GetMMCInfo(pstDevice->ulPortNum, &stInfo);
		HxSTD_StrNCpy(szUuid, stInfo.szUuid, HxSTD_StrLen(stInfo.szUuid));

		//Save to cache
		HxSTD_StrNCpy(pstDevice->szUUID, stInfo.szUuid, HxSTD_StrLen(stInfo.szUuid));
	}
	else
#endif
	{
		// NOTE : 현재는 HDD 등록시 포멧을 무조건 하도록 되어있기에 ulCnt가 0인 파티션만 유효할 것이다.
		// partition이 추가 된다면 code도 수정되어야한다.
		for (ulCnt = 0; ulCnt < pstDevice->ulNumPartition; ulCnt++)
		{
			HxLOG_Print("[umma_fs_dev_GetDeviceUuid] pstDevice->pstPartArray[ulCnt].ePartFsType = %d\n", pstDevice->pstPartArray[ulCnt].ePartFsType);


			hErr = PAL_FS_GetVolumeUuid(eDevIdx, ulCnt, szPalUuid, VOLUUIDSZ, &ulUuidsize, (PAL_FS_FsType_t)pstDevice->pstPartArray[ulCnt].ePartFsType);
			if(hErr == ERR_OK)
			{
				pCur = szUuid;

				// DI에서 받은 Hex로된 UUID를 String으로 변환하여 올려준다.
				for(ulIdx=0; ulIdx<ulUuidsize ; ulIdx++)
				{
					HxSTD_PrintToStrN(pCur, 3, "%02x",szPalUuid[ulIdx]);
					pCur = pCur + 2;
				}
				break;
			}
		}
	}
	if(hErr == ERR_OK)
	{
		HxLOG_Print("[umma_fs_dev_GetDeviceUuid] eDevIdx : %d,  UUID : %s\n",eDevIdx, szUuid);
		return ERR_OK;
	}

	HxLOG_Error("[umma_fs_dev_GetDeviceUuid] Fail to get uuid\n");

	return ERR_FAIL;
}

// PvrIdx를 입력으로 하여 UUID 얻는 API
HERROR umma_fs_dev_GetDeviceUuidByPvrIdx (HUINT32 ulPvrIdx, HUINT8 *szUuid)
{
	UMMA_FS_DevIdx_e eDevIdx;
	HERROR				 hErr;

	hErr = umma_fs_svc_PvrFindPvrDevIdPartIdx(ulPvrIdx, &eDevIdx, NULL);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_dev_GetDeviceUuidByPvrIdx] Can't find DevIdx. (%d) \n", eDevIdx);
		return ERR_FAIL;
	}

	return umma_fs_dev_GetDeviceUuid(eDevIdx, szUuid);
}


HERROR umma_fs_dev_GetDeviceIdByDriveNo (HUINT32 ulDriveNo, UMMA_FS_DevIdx_e *peDevIdx)
{
	HINT32					 nIndex, nPartIdx;
	UMMA_FS_DevInfoMap_t			*pstContext;
	UMMA_FS_PartitionInfo_t	*pstPartInfo;

	if (peDevIdx == NULL)				{ return ERR_FAIL; }

	pstContext = umma_fs_dev_GetContext();

	// Check if the device is already mounted
	for (nIndex = 0; nIndex < eFS_Dev_MAX; nIndex++)
	{
		if (pstContext->astDevice[nIndex].bDevAvailable != TRUE || pstContext->astDevice[nIndex].eDevStatus == eFS_DevStatus_SafeRemoved)			{ continue; }

		for (nPartIdx = 0; nPartIdx < pstContext->astDevice[nIndex].ulNumPartition; nPartIdx++)
		{
			pstPartInfo = &(pstContext->astDevice[nIndex].pstPartArray[nPartIdx]);

			if (pstPartInfo->bMounted == TRUE)
			{
				if (pstPartInfo->ulDriveNo == ulDriveNo)
				{
					*peDevIdx = pstContext->astDevice[nIndex].eDevIdx;
					return ERR_OK;
				}
			}
		}
	}

	*peDevIdx = eFS_Dev_NULL;
	return ERR_FAIL;
}

HERROR umma_fs_dev_ConnectNewDevice (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_DevContext_t **ppstDevice)
{
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	UMMA_FS_FormatPartList_t 	stFormatList;
	HERROR				 hErr;

	if (ppstDevice == NULL)					{ return ERR_FAIL; }

	// Check if the device is already mounted
	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr == ERR_OK && pstDevice != NULL)
	{
		// Device가 존재한 경우는 세가지이다.
		// 1. 이미 Connection 처리되어 있고 Mount 처리도 된 상태 : Skip 이다.
		if (pstDevice->bDevConnected == TRUE && pstDevice->bDevMounted == TRUE)
		{
			HxLOG_Print("[FS_DEV_ConnectDevice] Already Dev(%d) mounted\n", eDevIdx);
			return ERR_FAIL;
		}
		// 2. Device가 Connect 되어 있다고 인식되어 있는데 mount가 안 된 상태 : Device를 초기화하고 여기서 재설정으로 간다.
		else if (pstDevice->bDevConnected == TRUE)
		{
			umma_fs_dev_InitFsDeviceInfo (pstDevice);
			//pstDevice = NULL;
		}
		// 3. Device가 Connect 되어 있지 않은 것으로 인식하는데 mount는 되어 있는 상태 : unmount를 진행한 후, device를 초기화하고 재설정해야 한다.
		else if (pstDevice->bDevMounted == TRUE)
		{
			(void)umma_fs_dev_UnmountDevice (eDevIdx);
			//pstDevice = NULL;
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
			HxLOG_Error("[FS_DEV_ConnectDevice] No empty device slot.\n");
			return ERR_FAIL;
		}
	}

	// Device connection setting
	umma_fs_dev_InitFsDeviceInfo (pstDevice);

	pstDevice->bDevAvailable	= TRUE;
	pstDevice->bDevConnected	= TRUE;
	pstDevice->eDevIdx			= eDevIdx;
	pstDevice->eDevStatus		= eFS_DevStatus_NeedFormat;

	// 2. PAL에서 Partition Info를 읽어 온다.
	// mount 되지 않은 상태이라도 현재 partition 정보는 필요하다.

	VK_memset(&stFormatList, 0, sizeof(UMMA_FS_FormatPartList_t)); // MOON_encr
	umma_fs_dev_ReinitPartitionInfo (eDevIdx, &stFormatList, FALSE);

	*ppstDevice = pstDevice;
	return ERR_OK;
}


HERROR umma_fs_dev_GetDevIdxByDevUuid(UMMA_FS_DevIdx_e *pulDevIdx, HUINT8 *szUuid)
{
	HERROR			 hErr;
	HUINT32			ulIdx=0;
	HUINT8			szPalUuid[UMMA_FS_DEV_UUID_BUFSIZE];


	// devIdx를 루프로 HDD의 UUID를 받아서 szUuid와 비교한다.
	for(ulIdx = 0 ; ulIdx < eFS_Dev_MAX ; ulIdx++)
	{
		VK_memset (szPalUuid, 0, UMMA_FS_DEV_UUID_BUFSIZE);

		if (umma_fs_dev_IsDeviceExist(ulIdx) == TRUE)
		{
			hErr = umma_fs_dev_GetDeviceUuid(ulIdx, szPalUuid);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("Get UUID Error!!\n");
				continue;
			}

			if(VK_strncmp(szPalUuid, szUuid, UMMA_FS_DEV_UUID_BUFSIZE) == 0)
			{
				if(pulDevIdx != NULL)		*pulDevIdx = ulIdx;
				return ERR_OK;
			}
		}

	}
	return ERR_FAIL;
}


HERROR	umma_fs_dev_SetDeviceVolumeName(UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx, HUINT8 *szName)
{
	HERROR	hErr = ERR_FAIL;
	UMMA_FS_DevContext_t		*pstSrcDevInfo;
	UMMA_FS_PartitionInfo_t	*pstPartArray;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstSrcDevInfo);
	if (hErr != ERR_OK || pstSrcDevInfo == NULL)
	{
		HxLOG_Error("\numma_fs_dev_GetDeviceInfo Error!!!\n\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
	if (pstSrcDevInfo->eDevType == eFS_DevType_MMC)
	{
		hErr = PAL_MMC_SetVolumeName(pstSrcDevInfo->ulPortNum, szName, UMMA_FS_VOLUME_LABEL_SIZE-1);
	}
	else
#endif
	{
		hErr = PAL_FS_SetVolumeName(eDevIdx, ulPartIdx, szName);
	}

	if(ERR_OK != hErr)
	{
		HxLOG_Error("\numma_fs_dev_SetDeviceVolumeName FAIL !! \n\n");
		return hErr;
	}

	// 변경된 Volume name으로 MW context 수정
	HLIB_STD_StrNCpySafe(pstSrcDevInfo->szLabel, szName, UMMA_FS_VOLUME_LABEL_SIZE);

	pstPartArray = &(pstSrcDevInfo->pstPartArray[ulPartIdx]);
	HLIB_STD_StrNCpySafe(pstPartArray->szVolumeName, szName, UMMA_FS_VOLUME_LABEL_SIZE);

	return ERR_OK;
}

HERROR	umma_fs_dev_GetUuidByPath(HUINT8 *pszPath, HUINT8 *pszUuid)
{
	HUINT32					 ulDevCnt, ulPartIdx;
	UMMA_FS_DevInfoMap_t			*pstContext;
	UMMA_FS_DevContext_t			*pstDevice;
	UMMA_FS_PartitionInfo_t	*pstPartInfo;
	HBOOL				bFind = FALSE;

	pstContext = umma_fs_dev_GetContext();

	for (ulDevCnt = 0; ulDevCnt < eFS_Dev_MAX; ulDevCnt++)
	{
		pstDevice = &(pstContext->astDevice[ulDevCnt]);
		if (pstDevice->bDevAvailable != TRUE)				{ continue; }

		for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
		{
			pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);

			if(TRUE != pstPartInfo->bMounted)		{ continue; }

			 //UUID를 얻기위한 것임으로 mount path 까지만 비교해도 된다.
			if(HxSTD_StrNCmp(pstPartInfo->szMountPath, pszPath, HxSTD_StrLen(pstPartInfo->szMountPath)) == 0)
			{
				HxSTD_StrNCpy(pszUuid, pstDevice->szUUID, UMMA_FS_DEV_UUID_BUFSIZE);
				bFind = TRUE;
				break;
			}
		}
		if(bFind)			{ break; }
	}

	if(bFind)		return ERR_OK;

	return ERR_FAIL;
}

HERROR	umma_fs_dev_GetDevSize(HCHAR *pszDevName, HUINT64 *pullTotalKb)
{
	return SEUAPI_GetDevSize(pszDevName, pullTotalKb);
}

HERROR	umma_fs_dev_GetPathSize(HCHAR *pszPathName, HUINT64 *pullSizeKb)
{
	return SEUAPI_GetPathSize(pszPathName, pullSizeKb);
}

#define _____PARTITION_____

// Path 로부터 Device Index와 Partition Index를 구하는 함수.
HERROR umma_fs_dev_GetDevPartIndexByPath (HUINT8 *szDrivePath, UMMA_FS_DevIdx_e *peDevIdx, HUINT32 *pulPartIdx)
{
	HUINT32				 ulDevIdx, ulPartIdx;
	HUINT8				*szPartPath;
	UMMA_FS_DevInfoMap_t		*pstContext;
	UMMA_FS_DevContext_t		*pstSrcDevInfo;

	if (szDrivePath == NULL || peDevIdx == NULL || pulPartIdx == NULL)
	{
		return ERR_FAIL;
	}

	pstContext = umma_fs_dev_GetContext();

	// Check if the device is already mounted
	for (ulDevIdx = 0; ulDevIdx < eFS_Dev_MAX; ulDevIdx++)
	{
		pstSrcDevInfo = &(pstContext->astDevice[ulDevIdx]);

		if (pstSrcDevInfo->bDevAvailable != TRUE || pstSrcDevInfo->eDevStatus == eFS_DevStatus_SafeRemoved)				{ continue; }

		for (ulPartIdx = 0; ulPartIdx < pstSrcDevInfo->ulNumPartition; ulPartIdx++)
		{
			szPartPath = pstSrcDevInfo->pstPartArray[ulPartIdx].szMountPath;

			if (strstr (szDrivePath, szPartPath) != NULL)
			{
				*peDevIdx	= pstSrcDevInfo->eDevIdx;
				*pulPartIdx	= ulPartIdx;
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

HERROR umma_fs_dev_GetPartitionListFromPal (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_PartitionList_t *pstMwPartList)
{
	PAL_FS_StorDevIdx_t		 ePalDevIdx;
	HUINT32					 ulIndex, ulPartNum;
	UMMA_FS_PartitionInfo_t	*pstMwPartInfo;
	PAL_FS_PartitionList_t	*pstPalPartList;
	PAL_FS_PartitionInfo_t	*pstPalPartInfo;
	HERROR					 hErr;
	UMMA_FS_DevContext_t 			*pstDevice = NULL;
	if (pstMwPartList == NULL)			{ return ERR_FAIL; }

	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);

	pstPalPartList = (PAL_FS_PartitionList_t *)UMMA_Calloc(sizeof(PAL_FS_PartitionList_t));
	if (pstPalPartList == NULL)			{ return ERR_FAIL; }

	hErr =umma_fs_dev_GetDeviceInfo(eDevIdx,&pstDevice);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_dev_GetDeviceUuid] No Device (%d) \n", eDevIdx);
		if (pstPalPartList)
			UMMA_Free(pstPalPartList);
		return ERR_FAIL;
	}

	if(pstDevice->eDevType == eFS_DevType_MMC)
	{
		if (pstPalPartList)
			UMMA_Free(pstPalPartList);
		return ERR_FAIL; // MMC 의 MBR 은 fs_mmc 에서 가져온다.
	}

	hErr = PAL_FS_GetPartitionFromDeviceMbr (ePalDevIdx, pstPalPartList);
	if (hErr != ERR_OK && hErr!= 11 /* DI_ERR_NO_PARTITIONS*/)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		UMMA_Free (pstPalPartList);
		return ERR_FAIL;
	}

	if(hErr == 11 /* DI_ERR_NO_PARTITIONS*/)
	{
		pstPalPartList->ulNumPartition = 1;
		PAL_FS_GetFsType(ePalDevIdx,0,&pstPalPartList->astPartition[0].eFsType);
	}

	VK_memset (pstMwPartList, 0, sizeof(UMMA_FS_PartitionList_t));

	ulPartNum = (pstPalPartList->ulNumPartition > PAL_FS_MAX_PARTITIONS) ? PAL_FS_MAX_PARTITIONS: pstPalPartList->ulNumPartition;

	pstMwPartList->eDevIdx				= eDevIdx;
	pstMwPartList->ulUnitSize			= (HUINT32)pstPalPartList->ullUnitSize;
	pstMwPartList->ulTotalUnitNum		= pstPalPartList->ulNumUnit;
	pstMwPartList->ulPartitionNum		= ulPartNum;
	VK_strncpy (pstMwPartList->szDeviceName, pstPalPartList->szDevFileName, UMMA_FS_DEVICE_PATH_LEN);

	for (ulIndex = 0; ulIndex < ulPartNum; ulIndex++)
	{
		pstMwPartInfo	= &(pstMwPartList->astPartArray[ulIndex]);
		pstPalPartInfo	= &(pstPalPartList->astPartition[ulIndex]);

		pstMwPartInfo->bAvailable	= pstPalPartInfo->bAvailable;
		pstMwPartInfo->ulStartUnit	= pstPalPartInfo->ulStartUnit;
		pstMwPartInfo->ulEndUnit	= pstPalPartInfo->ulEndUnit;
		pstMwPartInfo->ePartFsType	= umma_fs_dev_ConvertPalFsTypeToMwFsType (pstPalPartInfo->eFsType);
	}

	UMMA_Free (pstPalPartList);
	return ERR_OK;
}

HERROR umma_fs_dev_ReinitPartitionInfo (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_FormatPartList_t *pstFormatList, HBOOL bTryFormat)
{
	UMMA_FS_PartitionList_t	*pstPartList = NULL;
	UMMA_FS_PartitionInfo_t	*pstPartArray = NULL;
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	HERROR				hErr, hResult = ERR_FAIL;

	if (pstFormatList == NULL)
	{
		HxLOG_Error("[umma_fs_dev_ReinitPartitionInfo] pstFormatList is NULL\n");
		goto ERROR;
	}

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("[umma_fs_dev_ReinitPartitionInfo] umma_fs_dev_GetDeviceInfo err(%08x)", hErr);
		goto ERROR;
	}

	pstPartList = (UMMA_FS_PartitionList_t *)UMMA_Malloc(sizeof(UMMA_FS_PartitionList_t));
	if (pstPartList == NULL)
	{
		HxLOG_Error("[umma_fs_dev_ReinitPartitionInfo] UMMA_Malloc (pstPartList) fail\n");
		goto ERROR;
	}

	pstDevice->ulNumPartition = 0;
	if (pstDevice->pstPartArray != NULL)
	{
		UMMA_Free (pstDevice->pstPartArray);
		pstDevice->pstPartArray = NULL;
	}

#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM) && defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
// MOON_encr
{
	HUINT32 	ulPartIdx;
	HINT8	uPassword[33];
	HINT8 	uMapName[50];
	HBOOL	bGetPword = FALSE;

	for (ulPartIdx=0; ulPartIdx < pstFormatList->ulPartitionNum && ulPartIdx < UMMA_FS_MAX_PARTITION; ulPartIdx++)
	{
		if (pstFormatList->astPartArray[ulPartIdx].bSecured == TRUE)
		{
			if (bGetPword == FALSE)
			{
				VK_memset(uPassword, 0, 33);
				hErr = UMMA_FS_PAL_HDD_GetEncryptionKey(PAL_HDD_PORT_NUM_1, uPassword);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("[umma_fs_dev_ReinitPartitionInfo] error in UMMA_FS_PAL_HDD_GetEncryptionKey\n");
					goto ERROR;
				}
				bGetPword = TRUE;
			}

			HxLOG_Info("umma_fs_dev_ReinitPartitionInfo : eDevIdx = %d, PASSWORD_NAME = %s\n", eDevIdx, uPassword);

			if (bTryFormat == TRUE)
			{
				hErr = PAL_FS_ENCR_Close(eDevIdx, ulPartIdx);
				hErr |= PAL_FS_ENCR_Format(eDevIdx, ulPartIdx, (char*)uPassword);
			}

			VK_memset(uMapName, 0, 50);
			snprintf(uMapName, 50, "%s%02d", DEVICE_MAPPER_NAME, ulPartIdx);
			hErr |= PAL_FS_ENCR_Open(eDevIdx, ulPartIdx, (char*)uMapName, (char*)uPassword);
		}
	}
}
#endif

	pstPartList->ulPartitionNum = 0;
	hErr = umma_fs_dev_GetPartitionListFromPal (eDevIdx, pstPartList);
	if (hErr != ERR_OK || pstPartList->ulPartitionNum == 0)
	{
		HxLOG_Error("[umma_fs_dev_ReinitPartitionInfo] umma_fs_dev_GetPartitionListFromPal err(%08x), partnum(%d)", hErr, pstPartList->ulPartitionNum);
		goto ERROR;
	}

	// detachable은 시나리오상 NTFS만 허용한다... EXT3 등 다른 포멧인 경우 mount시켜서는 안되고 needFormat 상태가 되어야 한다.
	if (pstDevice->eDevType == eFS_DevType_Detachable)
	{
		if (pstPartList->ulPartitionNum != pstFormatList->ulPartitionNum)
		{
			HxLOG_Error("[umma_fs_dev_ReinitPartitionInfo] Detachable HDD's partitionNum is (%d) but it must be (%d)\n", pstPartList->ulPartitionNum, pstFormatList->ulPartitionNum);
			goto ERROR;
		}

		if (pstPartList->astPartArray[0].ePartFsType != pstFormatList->astPartArray[0].ePartFsType)
		{
			HxLOG_Error("[umma_fs_dev_ReinitPartitionInfo] Detachable HDD's FsType is (%d) but it must be (%d)\n", pstPartList->astPartArray[0].ePartFsType, pstFormatList->astPartArray[0].ePartFsType);
			goto ERROR;
		}
	}

	pstPartArray = (UMMA_FS_PartitionInfo_t *)UMMA_Malloc (sizeof(UMMA_FS_PartitionInfo_t) * pstPartList->ulPartitionNum);
	if (pstPartArray != NULL)
	{
		HUINT32 ulPartIdx;
		VK_memcpy (pstPartArray, pstPartList->astPartArray, sizeof(UMMA_FS_PartitionInfo_t) * pstPartList->ulPartitionNum);

		pstDevice->ulNumPartition	= pstPartList->ulPartitionNum;
		pstDevice->ulMaxPartition	= pstPartList->ulPartitionNum;
		pstDevice->pstPartArray 	= pstPartArray;
		for (ulPartIdx=0; ulPartIdx <pstPartList->ulPartitionNum; ulPartIdx++)
		{
			pstDevice->pstPartArray[ulPartIdx].ulPvrIdx = DxSTORAGE_INVALID_PVR_INDEX;
			pstDevice->pstPartArray[ulPartIdx].ulPlaybackIdx = DxSTORAGE_INVALID_PVR_INDEX;
		}
	}


#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
// MOON_encr
{
	HUINT32 ulPartIdx;
	for (ulPartIdx=0; ulPartIdx < pstPartList->ulPartitionNum && ulPartIdx < pstFormatList->ulPartitionNum && ulPartIdx < UMMA_FS_MAX_PARTITION; ulPartIdx++)
	{
		pstDevice->pstPartArray[ulPartIdx].bSecured = pstFormatList->astPartArray[ulPartIdx].bSecured;
	}
}
#endif


	hResult = ERR_OK;

ERROR:
	if (pstPartList != NULL)			{ UMMA_Free (pstPartList); }
	return hResult;
}

/*
	Partition을 File System에 Mount 하는 함수.
	ppstDstPartInfo 에는 Device Context 내에서 Mount 된 Partition Info의 pointer가 return된다.
*/
HERROR umma_fs_dev_MountPartition (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx, HBOOL bSecured, HUINT8 *szMountPath, UMMA_FS_PartitionInfo_t **ppstDstPartInfo)
{
	HBOOL					 bWritable;
	HUINT64					 ullTotalSizeKb;
	PAL_FS_StorDevIdx_t		 ePalDevIdx;
	PAL_FS_FsType_t			 ePalFsType;
	UMMA_FS_DevContext_t				*pstDevice = NULL;
	UMMA_FS_PartitionInfo_t		*pstPartInfo;
	HERROR					 hErr, hResult = ERR_FAIL;
	HxLOG_Print("-----------[%d] [%s] [%s]\n", __LINE__, __FUNCTION__, __FILE__);

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)					{ return ERR_FAIL; }

	if (ulPartIdx >= pstDevice->ulNumPartition)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}
	HxLOG_Print("-----------[%d] [%s] [%s]\n", __LINE__, __FUNCTION__, __FILE__);

	pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);

	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);
	HxLOG_Print("-----------[%d] [%s] [%s] before ePalFsType = %d\n", __LINE__, __FUNCTION__, __FILE__, pstPartInfo->ePartFsType);
	ePalFsType = umma_fs_dev_ConvertMwFsTypeToPalFsType (pstPartInfo->ePartFsType);
	HxLOG_Print("-----------[%d] [%s] [%s] ePalFsType = %d\n", __LINE__, __FUNCTION__, __FILE__, ePalFsType);

	hErr = PAL_FS_MountPartition (ePalDevIdx, ulPartIdx, szMountPath, ePalFsType, bSecured);

	if (hErr == ERR_OK)
	{
		pstPartInfo->bMounted		= TRUE;
		pstPartInfo->bPvrFlag		= FALSE;
		pstPartInfo->bSystemFlag	= FALSE;
		pstPartInfo->bDefaultPvr	= FALSE;
		pstPartInfo->bWritable		= TRUE;
		pstPartInfo->bSecured		= bSecured;
		VK_strncpy (pstPartInfo->szMountPath, szMountPath, UMMA_FS_MOUNT_PATH_LEN);
		umma_fs_dev_GetDeviceVolumeName (eDevIdx, ulPartIdx, pstPartInfo->ePartFsType, pstPartInfo->szVolumeName);

		hErr = umma_fs_dev_GetMountedPartitionStatus (eDevIdx, ulPartIdx, &ullTotalSizeKb, NULL, &bWritable);
		if (hErr == ERR_OK)
		{
			pstPartInfo->ullTotalSizeKb	= ullTotalSizeKb;
			pstPartInfo->bWritable		= bWritable;
		}

		// 첫 mount 성공한 partition 정보를 대표격으로 Device Info에 기록하도록 한다.
		if (pstDevice->bDevMounted != TRUE)
		{
			pstDevice->eFsType = pstPartInfo->ePartFsType;
			pstDevice->bWriteProtected = (pstPartInfo->bWritable == TRUE) ? FALSE : TRUE;
			VK_strncpy (pstDevice->szLabel, pstPartInfo->szVolumeName, UMMA_FS_VOLUME_LABEL_SIZE);

			pstDevice->bDevMounted = TRUE;
			pstDevice->bDevConnected = TRUE;
			pstDevice->ullDevSizeKb = 0;
		}

		// UUID를 채운다.
		VK_memset(pstDevice->szUUID, 0, UMMA_FS_DEV_UUID_BUFSIZE);
		hErr = umma_fs_dev_GetDeviceUuid (eDevIdx, pstDevice->szUUID);

		if (hErr != ERR_OK)
		{
			HxSTD_PrintToStrN(pstDevice->szUUID, UMMA_FS_DEV_UUID_BUFSIZE, "unknown_%d",eDevIdx);
		}

		HxLOG_Info("hErr [0x%x], ullTotalSizeKb [%lld] KB >>>\n", hErr, ullTotalSizeKb);
		HxLOG_Info("hErr [0x%x], szLabel [%s] >>>\n", hErr, pstDevice->szLabel);

		hResult = ERR_OK;
	}
	else
	{
		pstPartInfo->bMounted		= FALSE;
		pstPartInfo->bPvrFlag		= FALSE;
		pstPartInfo->bSystemFlag	= FALSE;
		pstPartInfo->bWritable		= FALSE;
		pstPartInfo->ulDriveNo		= 0;
		pstPartInfo->ullTotalSizeKb	= 0;
		pstPartInfo->ePartFsType	= eFS_FileSystem_NONE;
		VK_memset (pstPartInfo->szMountPath, 0, UMMA_FS_MOUNT_PATH_LEN);
		VK_memset (pstPartInfo->szVolumeName, 0, UMMA_FS_VOLUME_LABEL_SIZE);

		HxLOG_Error("PAL_FS_MountPartition() Fail!!! >>>\n");
	}

	if (ppstDstPartInfo != NULL)
	{
		*ppstDstPartInfo = pstPartInfo;
	}

	return hResult;
}



HERROR umma_fs_dev_UnmountPartition (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx)
{
	PAL_FS_StorDevIdx_t		 ePalDevIdx;
	PAL_FS_FsType_t			 ePalFsType;
	UMMA_FS_DevContext_t				*pstDevice;
	UMMA_FS_PartitionInfo_t		*pstPartInfo;
	HERROR					 hErr, hRet;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK)								{ return ERR_FAIL; }
	if (ulPartIdx >= pstDevice->ulNumPartition)		{ return ERR_FAIL; }

	pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);
	if (pstPartInfo->bMounted == FALSE)
	{
		HxLOG_Error("[umma_fs_dev_UnmountPartition] Partition (%d:%d :%s) isn't mounted\n", eDevIdx, ulPartIdx, pstPartInfo->szMountPath);
		return ERR_FAIL;
	}

	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);
	ePalFsType = umma_fs_dev_ConvertMwFsTypeToPalFsType (pstPartInfo->ePartFsType);

	HxLOG_Debug("[umma_fs_dev_UnmountPartition] Unmount Partition : [%d, %d, %d, %d]\n", ePalDevIdx, ulPartIdx, ePalFsType, pstPartInfo->bSecured);
	hRet = PAL_FS_UnmountPartition (ePalDevIdx, ulPartIdx, ePalFsType, pstPartInfo->bSecured); // MOON_encr
	if (hRet == ERR_OK)
	{
		HLIB_DIR_Delete(pstPartInfo->szMountPath);
		PAL_FS_Flush();									// Sync the file system
	}
	else
	{
		// PAL_FS_UnmountPartition에서 무엇인가의 이유로 실패했다고 해도 (이미 없는 것을 재 unmount?)
		// 일단 MW FS 쪽에서 해당 등록 정보는 삭제시키도록 한다. (return ERR_FAIL을 안 하는 이유임)
		HxLOG_Error("[umma_fs_dev_UnmountPartition] PAL_FS_UnmountPartition ,path[%s], err(0x%08x)\n", pstPartInfo->szMountPath,hRet);

		return ERR_FAIL;

	}
#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
	if (pstPartInfo->bSecured == TRUE)
	{
		hRet = PAL_FS_ENCR_Close(ePalDevIdx, ulPartIdx);
		if (hRet != ERR_OK)
		{
			HxLOG_Error("Error : PAL_FS_ENCR_Close(ePalDevIdx : %d, ulPartIdx : %d) \n",ePalDevIdx, ulPartIdx);
		}
		pstPartInfo->bSecured = FALSE;
	}
#endif

	pstPartInfo->bMounted = FALSE;

	return ERR_OK;
}

HERROR umma_fs_dev_UnmountDevice (UMMA_FS_DevIdx_e eDevIdx)
{
	HUINT32				 ulCount; //,ulTry;
	UMMA_FS_DevContext_t		*pstDevice;
	HERROR				 hErr;
	HCHAR				szFileName[50] = {0,};

	HxLOG_Debug("eDevIdx : [%d]\n", eDevIdx);
	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK)		{ return ERR_FAIL; }

	hErr = ERR_OK;
	for (ulCount = 0/*,ulTry=0*/; ulCount < pstDevice->ulNumPartition; ulCount++)
	{
		if(pstDevice->pstPartArray[ulCount].bMounted == TRUE)
		{
			// 실제 존재하는 mount 정보만 umount
			if(HLIB_DIR_IsMountedPath(pstDevice->pstPartArray[ulCount].szMountPath) == TRUE)
			{
				if(pstDevice->bDevConnected == TRUE)
				{
					/* Delete file for Safely remove */
					HxSTD_PrintToStrN(szFileName, 50, "%s%s", pstDevice->pstPartArray[ulCount].szMountPath, UMMA_FS_SAFELY_REMOVE_CHK_FILE);
					if(HLIB_FILE_Exist(szFileName) == TRUE)
					{
						/* 즉시 삭제를 위해 remove 함수 사용 */
						(void)remove(szFileName);
					}
				}
				HxLOG_Warning("found mount path in /proc/mountinfo : [%s]\n", pstDevice->pstPartArray[ulCount].szMountPath);
				if(umma_fs_dev_UnmountPartition(eDevIdx, ulCount) == ERR_OK)
				{
					HxLOG_Warning("unmount ok ID[%d],part[%d]\n",eDevIdx, ulCount);
					umma_fs_dev_ErasePartInfo(eDevIdx,ulCount); // 등록 정보를 지우자.
				}
				else
				{
					hErr = ERR_FAIL;
					HxLOG_Warning("unmount fail ID[%d],part[%d]\n",eDevIdx, ulCount);
				}
			}
			else // obama 가 알지 못하는 이유로.. 파티션이 없는 상태
			{
				HxLOG_Warning("Couldn't find Mount Path.. [%s]\n",pstDevice->pstPartArray[ulCount].szMountPath);
				umma_fs_dev_ErasePartInfo(eDevIdx,ulCount); // 등록 정보를 지우자.
			}
		}
	}
	HxLOG_Print("(-)[%s:%d] hErr=[%d]\n",__FUNCTION__,__LINE__,hErr);
	return (hErr == ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR umma_fs_dev_GetMountedPartitionStatus (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx, HUINT64 *pullTotalKbSize, HUINT64 *pullUsedKbSize, HBOOL *pbWritable)
{
//	PAL_FS_StorDevIdx_t		 ePalDevIdx;
//	PAL_FS_FsType_t			 ePalFsType;
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	UMMA_FS_PartitionInfo_t	*pstPartInfo = NULL;
	HERROR					 hErr;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK)								{ return ERR_FAIL; }
	if (ulPartIdx >= pstDevice->ulNumPartition)		{ return ERR_FAIL; }

	pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);

//	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);
//	ePalFsType = umma_fs_dev_ConvertMwFsTypeToPalFsType (pstPartInfo->ePartFsType);

	hErr = HLIB_STORAGE_GetPartitionSize(pstPartInfo->szMountPath, pullTotalKbSize, pullUsedKbSize, pbWritable);
	if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");

	return hErr;
}

HERROR umma_fs_dev_GetDirectoryStatus (HUINT8 *szDirPath, HUINT64 *pullTotalKbSize, HUINT64 *pullUsedKbSize)
{
	HUINT32					 ulIndex, ulPartIdx;
	HUINT32					 ulDirLen, ulPartLen, ulCompLen;
	HUINT64					 ullTotalKbSize = 0,   ullUsedKbSize = 0;
	HUINT64					 ullTotalPartSize = 0, ullUsedPartSize = 0;
	HUINT8					*szDirPath2 = NULL, *szMountPath2 = NULL;
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	UMMA_FS_PartitionInfo_t	*pstPartInfo = NULL;
	PAL_FS_StorDevIdx_t		 ePalDevIdx;
	PAL_FS_FsType_t			 ePalFsType;
	UMMA_FS_DevInfoMap_t			*pstContext;
	HERROR					 hErr, hResult = ERR_FAIL;

	if (szDirPath == NULL || szDirPath[0] == '\0')
	{
		goto ERROR;
	}

	// 둘을 정확히 비교하기 위해 / (separator)를 붙여서 비교해야 한다.
	// argument나 device info의 값을 바꾸지 않고 고치기 위해 text buffer를 따로 준비한다.
	szDirPath2 = (HUINT8 *)UMMA_Malloc (256);
	szMountPath2 = (HUINT8 *)UMMA_Malloc (256);
	if (szDirPath2 == NULL || szMountPath2 == NULL)
	{
		goto ERROR;
	}

	// DirPath를 Copy한 후 Separator를 붙인다.
	VK_strncpy (szDirPath2, szDirPath, 255);
	szDirPath2[254] = '\0';
	umma_fs_dev_AppendSeparatorToPath (szDirPath2);
	ulDirLen = VK_strlen (szDirPath2);

	pstContext = umma_fs_dev_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("[umma_fs_dev_GetDirectoryStatus] umma_fs_dev_GetContext NULL\n");
		goto ERROR;
	}

	for (ulIndex = 0; ulIndex < eFS_Dev_MAX; ulIndex++)
	{
		pstDevice = &(pstContext->astDevice[ulIndex]);
		if ((pstDevice->bDevAvailable == FALSE) ||
			(pstDevice->ulNumPartition == 0) || (pstDevice->pstPartArray == NULL))		{ continue; }

		ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (pstDevice->eDevIdx);

		for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
		{
			pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);
			if (pstPartInfo->bAvailable == TRUE && pstPartInfo->bMounted == TRUE)
			{
				// szMountPath를 Copy한 후 Separator를 붙인다.
				VK_strncpy (szMountPath2, pstPartInfo->szMountPath, 255);
				szMountPath2[254] = '\0';
				umma_fs_dev_AppendSeparatorToPath (szMountPath2);
				ulPartLen = VK_strlen (szMountPath2);

				// EX1) dir_path 가 "/mnt/" 이고 mounted_path가 "/mnt/hd1/" 일 경우 :
				// EX2) dir_path 가 "/mnt/hd1/video/" 이고 mounted_path가 "/mnt/hd1/" 일 경우 :
				// EX1, EX2 둘 다 현재 partition의 Size들을 얻어가야 한다.
				ulCompLen = (ulDirLen > ulPartLen) ? ulPartLen : ulDirLen;
				if (VK_strncmp (szDirPath2, szMountPath2, ulCompLen) == 0)
				{
					ullTotalPartSize = ullUsedPartSize = 0;
					ePalFsType = umma_fs_dev_ConvertMwFsTypeToPalFsType (pstPartInfo->ePartFsType);

					hErr = PAL_FS_GetMountedPartitionStatus (ePalDevIdx, ulPartIdx, ePalFsType, &ullTotalPartSize, &ullUsedPartSize, NULL);
					if (hErr == ERR_OK)
					{
						ullTotalKbSize	+= ullTotalPartSize;
						ullUsedKbSize	+= ullUsedPartSize;
					}
					else
					{
						HxLOG_Critical("\nCritical Error!!!\n\n");
					}
				}
			}
		}
	}

	hResult = (ullTotalKbSize > 0) ? ERR_OK : ERR_FAIL;

ERROR:
	if (szDirPath2 != NULL)					{ UMMA_Free (szDirPath2); }
	if (szMountPath2 != NULL)				{ UMMA_Free (szMountPath2); }
	if (pullTotalKbSize != NULL)			{ *pullTotalKbSize = ullTotalKbSize; }
	if (pullUsedKbSize != NULL)				{ *pullUsedKbSize = ullUsedKbSize; }

	return hResult;
}

HERROR umma_fs_dev_GetPhysicalDeviceSize(UMMA_FS_DevIdx_e eDevIdx, HUINT64 *pullDevSizeKb)
{
	PAL_FS_StorDevIdx_t 	 ePalDevIdx;
	PAL_FS_PartitionList_t	*pstPalPartList = NULL;
	HERROR					 hErr = ERR_FAIL;
	UMMA_FS_DevContext_t			*pstDevice = NULL;

	if (pullDevSizeKb == NULL)
		return ERR_FAIL;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK)								{ return ERR_FAIL; }

	if (TRUE != pstDevice->bDevConnected)
	{
		// 연결이 안되어 있다면 access 하지 않는다. (연결 하자마자 제거하면 발생)
		HxLOG_Error("Disconnected device (%d)\n", eDevIdx);
		return ERR_FAIL;
	}

	if (pstDevice->ullDevSizeKb == 0)
	{
		*pullDevSizeKb = 0;
		ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);

		pstPalPartList = (PAL_FS_PartitionList_t *)UMMA_Malloc (sizeof(PAL_FS_PartitionList_t));
		if (pstPalPartList == NULL)
			return ERR_FAIL;

		hErr = PAL_FS_GetPartitionFromDeviceMbr (ePalDevIdx, pstPalPartList);
		if (hErr != ERR_OK)
		{
			UMMA_Free (pstPalPartList);
			return ERR_FAIL;
		}

		pstDevice->ullDevSizeKb = pstPalPartList->ullTotalSize / SIZE_1K;
		UMMA_Free (pstPalPartList);
	}

	*pullDevSizeKb = pstDevice->ullDevSizeKb;

	return ERR_OK;
}

HERROR umma_fs_dev_GetPhysicalMMCDeviceSize(UMMA_FS_DevIdx_e eDevIdx, HUINT64 *pullDevSizeKb, HUINT64 *pullUsedSizeKb,HUINT64 *pullAvailSizeKb)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
	HERROR					 hErr = ERR_FAIL;
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	PalMmc_Info_e			stInfo;
	if (pullDevSizeKb == NULL)
		return ERR_FAIL;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK)								{ return ERR_FAIL; }

	if (TRUE != pstDevice->bDevConnected)
	{
		// 연결이 안되어 있다면 access 하지 않는다. (연결 하자마자 제거하면 발생)
		HxLOG_Error("Disconnected device (%d)\n", eDevIdx);
		return ERR_FAIL;
	}
	if(	PAL_MMC_GetMMCInfo(pstDevice->ulPortNum,&stInfo) != ERR_OK)
	{
		HxLOG_Error("PAL_MMC_GetMMCInfo .\n");
		return ERR_FAIL;
	}

	if (pullDevSizeKb)	 { *pullDevSizeKb 	= stInfo.uiDiskSize;}
	if (pullUsedSizeKb)	{*pullUsedSizeKb = stInfo.uiUsedSize;}
	if (pullAvailSizeKb) { *pullAvailSizeKb = stInfo.uiAvailableSize;}
	return ERR_OK;
#else
	if (pullDevSizeKb)	{*pullDevSizeKb 	= 0;}
	if (pullUsedSizeKb)	{*pullUsedSizeKb = 0;}
	if (pullAvailSizeKb) {*pullAvailSizeKb = 0;}

	return ERR_FAIL;
#endif
}




#define _____DEBUG_FUNCTIONS_____

#if defined(CONFIG_DEBUG)
STATIC int umma_fs_svc_CmdDevice (void *szArgList)
{
	HUINT32					 ulDevCnt, ulPartIdx;
	UMMA_FS_DevInfoMap_t			*pstContext;
	UMMA_FS_DevContext_t			*pstDevice;
	UMMA_FS_PartitionInfo_t	*pstPartInfo;

	pstContext = umma_fs_dev_GetContext();

	HLIB_CMD_Printf ("\n\n");
	HLIB_CMD_Printf ("-----------------------------------------------------------------\n");
	HLIB_CMD_Printf ("---                   Storage Device Status                   ---\n");
	HLIB_CMD_Printf ("-----------------------------------------------------------------\n");

	for (ulDevCnt = 0; ulDevCnt < eFS_Dev_MAX; ulDevCnt++)
	{
		pstDevice = &(pstContext->astDevice[ulDevCnt]);
		if (pstDevice->bDevAvailable != TRUE)				{ continue; }

		HLIB_CMD_Printf ("  Device (%d) :", pstDevice->eDevIdx);

		if (pstDevice->bDevConnected == TRUE)				{ HLIB_CMD_Printf (" Connected"); }
		if (pstDevice->bDevMounted == TRUE)					{ HLIB_CMD_Printf (" mounted"); }

		HLIB_CMD_Printf ("\n");
		HLIB_CMD_Printf ("    DeviceType(0x%02x), DeviceStatus(%d), DeviceSubStatus(0x%08x), Partition Num(%d) \n", pstDevice->eDevType, pstDevice->eDevStatus, pstDevice->eDevSubStatus, pstDevice->ulNumPartition);
		HLIB_CMD_Printf ("    PortNum(%d)\n", pstDevice->ulPortNum);
		HLIB_CMD_Printf ("    FsType(%d), WriteProtected(%d)\n", pstDevice->eFsType, pstDevice->bWriteProtected);
		HLIB_CMD_Printf ("    Label : [%s]\n", pstDevice->szLabel);
		HLIB_CMD_Printf ("    UUID : [%s]\n", pstDevice->szUUID);

		for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
		{
			pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);

			HLIB_CMD_Printf ("        Partition (%d) : %s\n", ulPartIdx, (pstPartInfo->bAvailable == TRUE) ? "AVAILABLE" : "UNAVAILABLE");
			HLIB_CMD_Printf ("                  PartitionType(%d), StartUnit(%d), EndUnit(%d)\n", pstPartInfo->ePartFsType, pstPartInfo->ulStartUnit, pstPartInfo->ulEndUnit);
			HLIB_CMD_Printf ("                  Mounted(%d), PVR(%d), System(%d), Writable(%d)\n", pstPartInfo->bMounted, pstPartInfo->bPvrFlag, pstPartInfo->bSystemFlag, pstPartInfo->bWritable);
			HLIB_CMD_Printf ("                  PvrIdx(%d), DefaultPvr(%d)\n", pstPartInfo->ulPvrIdx, pstPartInfo->bDefaultPvr);
			HLIB_CMD_Printf ("                  PlaybackIdx(%d) \n", pstPartInfo->ulPlaybackIdx);
			HLIB_CMD_Printf ("                  DriveNo(%d), TotalSize(%dM)\n", pstPartInfo->ulDriveNo, ((HUINT32)pstPartInfo->ullTotalSizeKb)/1024);
			HLIB_CMD_Printf ("                  Mounted Path [%s]\n", pstPartInfo->szMountPath);
			HLIB_CMD_Printf ("                  Volume Name  [%s]\n", pstPartInfo->szVolumeName);
		}

		HLIB_CMD_Printf ("\n");
	}

	HLIB_CMD_Printf ("-----------------------------------------------------------------\n");

	return ERR_OK;
}

STATIC void umma_fs_svc_CmdInit(void)
{
#define	hCmdHandle		"FS"
	HLIB_CMD_RegisterWord(hCmdHandle,
							umma_fs_svc_CmdDevice,
							"device", 						/* keyword */
							"test MW FS module",			/*	 help  */
							"device"
							);
}

#endif

/*********************** End of File ******************************/
