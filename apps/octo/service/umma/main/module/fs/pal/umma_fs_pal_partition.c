/**
	@file     	pal_int_stor.c
	@brief    	internal storage event handler

	Description: 		\n
	Module: 		pal/fs			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <htype.h>
#ifndef WIN32
#include <mntent.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/statvfs.h>
#include <sys/vfs.h>
#include <sys/mount.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <linux/hdreg.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

#include <hlib.h>

#include "umma_fs_pal_fs.h"

#include "vkernel.h"
#include "htype.h"

#include "di_hdd.h"
#include <di_fs.h>
#include <umma_int.h>

#include "se_uapi.h"
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define MAX_MBR_BUFFER					2

#define DOSPART_OFFSET					446
#define DOSPART_SIZE					16
#define DOSPART_NUM					4

#define MAX_PRIMARY_PARTITION_NUM	4
#define MOUNT_PATH_LEN					128

#define RECYCLE_BIN_DIR					".recycleBin"

#ifdef CONFIG_PROD_ENDIAN_BIG
#include <byteswap.h>
#define MACRO_LittleEndian32bit(_x)		bswap_32(_x)
#else
#define MACRO_LittleEndian32bit(_x)		(_x)
#endif

#define MACRO_GetDosPart(_b, _n)			((PAL_FS_DosPartition_t *)((_b) + DOSPART_OFFSET + (_n) * DOSPART_SIZE))
#define MACRO_IsExtendedFsType(_x)		((_x) == eMBR_FS_TYPE_EXTENDED || \
				 						(_x) == eMBR_FS_TYPE_EXTD_LBA || \
				 						(_x) == eMBR_FS_TYPE_LINUX_EXT)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum {
	eMBR_FS_TYPE_EMPTY				= 0,
	eMBR_FS_TYPE_FAT12				= 0x01,
	eMBR_FS_TYPE_FAT16_32M			= 0x04,
	eMBR_FS_TYPE_EXTENDED			= 0x05,
	eMBR_FS_TYPE_FAT16				= 0x06,
	eMBR_FS_TYPE_HPFS_NTFS			= 0x07,
	eMBR_FS_TYPE_FAT32				= 0x0b,
	eMBR_FS_TYPE_FAT32_LBA			= 0x0c,
	eMBR_FS_TYPE_FAT16_LBA			= 0x0e,
	eMBR_FS_TYPE_EXTD_LBA			= 0x0f,
	eMBR_FS_TYPE_HID_FAT16_32M		= 0x14,
	eMBR_FS_TYPE_HID_FAT16			= 0x16,
	eMBR_FS_TYPE_HID_HPFS_NTFS		= 0x17,
	eMBR_FS_TYPE_HID_FAT32			= 0x1b,
	eMBR_FS_TYPE_HID_FAT32_LBA		= 0x1c,
	eMBR_FS_TYPE_HID_FAT16_LBA		= 0x1e,
	eMBR_FS_TYPE_CUSTOMIZED_FAT		= 0x68,		/* valid partition of IOCELL's IOC-SW2-U20 */
	eMBR_FS_TYPE_CUSTOMIZED_NWFS1	= 0x72,	/* Novell NetWare file system (highly customized FAT) */
	eMBR_FS_TYPE_CUSTOMIZED_NWFS2 	= 0x74,	/* Novell NetWare file system */
	eMBR_FS_TYPE_LINUX_SWAP			= 0x82,
	eMBR_FS_TYPE_LINUX				= 0x83,
	eMBR_FS_TYPE_LINUX_EXT			= 0x85,
	eMBR_FS_TYPE_NTFS_86			= 0x86,
	eMBR_FS_TYPE_NTFS_87			= 0x87,
	eMBR_FS_TYPE_UNKNOWN			= 0xFF
} PAL_FS_MbrFsType_t;

typedef struct
{
	HBIT		status:8;
	HBIT		head:8;
	HBIT		sect:8;
	HBIT		cyl:8;
	HBIT		type:8;
	HBIT		end_head:8;
	HBIT		end_sect:8;
	HBIT		end_cyl:8;
	HBIT		start:32;
	HBIT		size:32;
} PAL_FS_DosPartition_t;

// MBR Info : Partition 정의를 위해 필요하다.
typedef struct
{
	HBOOL					 bAvailable;
	PAL_FS_StorDevIdx_t		 eDevIdx;
	int						 nDevFileDsc;
	unsigned char			 aucMbrBuffer[MAX_SEC_SIZE];

	// 기타 필요 정보
	HUINT32					 ulHeads, ulSectors, ulSecSize;
	HUINT32					 ulSecPerDisk, ulSecsPerCyl, ulNumCylinders;
	HUINT64					 ullDiskSize, ullCylinderSize;
} PAL_FS_MbrInfo_t;

typedef struct
{
	PAL_FS_StorDevIdx_t		 eDevIdx;
	HUINT32					 ulPartIdx;
	HCHAR					 szDstMountDir[MOUNT_PATH_LEN];
	PAL_FS_FsType_t			 eFsType;

	HBOOL					 bRecycleDir;
} _PAL_FS_MountInfo_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

// For FDISK executions:
#if 0
STATIC pthread_mutex_t					 s_ulPalFsPtt_MbrLock = PTHREAD_MUTEX_INITIALIZER;
STATIC PAL_FS_MbrInfo_t					 s_astPalFsPtt_MbrArray[MAX_MBR_BUFFER];
#endif // #if 0

STATIC PAL_FS_Event_Callback_Func		 s_pfPalFormatProgress_NotifyCb = NULL;
STATIC HxList_t							*s_pstPalFsPart_MountList = NULL;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
static HUINT32 pal_fs_DiEvent2PalEvent(HUINT32 eDiEvent)
{
	switch(eDiEvent)
	{
	case DI_FS_EVENT_DISKFRESSSPACEFLOW:		return PAL_FS_EVENT_DISKFREESPACEFLOW;
	case DI_FS_EVENT_INT_HDD_ON:				return PAL_FS_EVENT_HDD_ON;
	case DI_FS_EVENT_INT_HDD_OFF:				return PAL_FS_EVENT_HDD_OFF;
	case DI_FS_EVENT_DISKINTITSTART:			return PAL_FS_EVENT_DISKINITSTART;
	case DI_FS_EVENT_DISKINITEND:				return PAL_FS_EVENT_DISKINITEND;
	case DI_FS_EVENT_DISKERROR:					return PAL_FS_EVENT_DISKERROR;
	case DI_FS_EVENT_HDDLOCKED:					return PAL_FS_EVENT_HDDLOCKED;
	case DI_FS_EVENT_HDDUNLOCKED:				return PAL_FS_EVENT_HDDUNLOCKED;
	case DI_FS_EVENT_USB_CONNECTED:				return PAL_FS_EVENT_USB_CONNECTED;
	case DI_FS_EVENT_USB_DISCONNECTED:			return PAL_FS_EVENT_USB_DISCONNECTED;
	case DI_FS_EVENT_USB_SCSI_ADDED:			return PAL_FS_EVENT_USB_SCSI_ADDED;
	case DI_FS_EVENT_USB_SCSI_REMOVED:			return PAL_FS_EVENT_USB_SCSI_REMOVED;
	case DI_FS_EVENT_USB_OVERCURRENT:			return PAL_FS_EVENT_USB_OVERCURRENT;
	case DI_FS_EVENT_FORMAT_PROGRESS:			return PAL_FS_EVENT_FORMAT_PROGRESS;
	case DI_FS_EVENT_FORMAT_DONE:				return PAL_FS_EVENT_FORMAT_DONE;
	case DI_FS_EVENT_FORMAT_FAIL:				return PAL_FS_EVENT_FORMAT_FAIL;
	case DI_FS_EVENT_FS_TASK_ADD_FILEOP:		return PAL_FS_EVENT_FS_TASK_ADD_FILEOP;
	case DI_FS_EVENT_COPY_PROGRESS:				return PAL_FS_EVENT_COPY_PROGRESS;
	case DI_FS_EVENT_COPY_DONE:					return PAL_FS_EVENT_COPY_DONE;
	case DI_FS_EVENT_DELETE_PROGRESS:			return PAL_FS_EVENT_DELETE_PROGRESS;
	case DI_FS_EVENT_MOVE_DONE:					return PAL_FS_EVENT_MOVE_DONE;
	default:									break;
	}

	return	PAL_FS_EVENT_MAX_NUMBER;
}

DI_FileSystemType_t pal_fs_ConvertPalFsTypeToDIFsType (PAL_FS_FsType_t ePalFsType)
{
	switch (ePalFsType)
	{
	case PAL_FS_EXT2_FILESYSTEM:
		return DI_FS_EXT2_FILESYSTEM;

	case PAL_FS_EXT3_FILESYSTEM:
		return DI_FS_EXT3_FILESYSTEM;

	case PAL_FS_EXT4_FILESYSTEM:
		return DI_FS_EXT4_FILESYSTEM;

	case PAL_FS_XFS_FILESYSTEM:
		return DI_FS_XFS_FILESYSTEM;
	case PAL_FS_FAT16_FILESYSTEM:
		return DI_FS_FAT16_FILESYSTEM;

	case PAL_FS_FAT32_FILESYSTEM:
		return DI_FS_FAT32_FILESYSTEM;

	case PAL_FS_NTFS_FILESYSTEM:
		return DI_FS_NTFS_FILESYSTEM;

	case PAL_FS_MSDOS_FILESYSTEM:
		return DI_FS_MSDOS_FILESYSTEM;

	case PAL_FS_OTHER_FILESYSTEM:
	default:
		return DI_FS_OTHER_FILESYSTEM;
	}
}


DI_FS_MNTOPT_E pal_fs_ConvertPalMntOptToDIMntOpt (PAL_FS_MNTOPT_e ePalMntOpt)
{
	switch (ePalMntOpt)
	{
	case ePAL_FS_MNTOPT_RDONLY:
		return DI_FS_MNTOPT_RDONLY;

	case ePAL_FS_MNTOPT_NOSUID:
		return DI_FS_MNTOPT_NOSUID;

	case ePAL_FS_MNTOPT_NODEV:
		return DI_FS_MNTOPT_NODEV;

	case ePAL_FS_MNTOPT_NOEXEC:
		return DI_FS_MNTOPT_NOEXEC;

	case ePAL_FS_MNTOPT_SYNCHRONOUS:
		return DI_FS_MNTOPT_SYNCHRONOUS;

	case ePAL_FS_MNTOPT_REMOUNT:
		return DI_FS_MNTOPT_REMOUNT;

	case ePAL_FS_MNTOPT_MANDLOCK:
		return DI_FS_MNTOPT_MANDLOCK;

	case ePAL_FS_MNTOPT_NOATIME:
		return DI_FS_MNTOPT_NOATIME;

	case ePAL_FS_MNTOPT_NODIRATIME:
		return DI_FS_MNTOPT_NODIRATIME;

	case ePAL_FS_MNTOPT_BIND:
		return DI_FS_MNTOPT_BIND;

	case ePAL_FS_MNTOPT_MOVE:
		return DI_FS_MNTOPT_MOVE;

	default:
		return 0;
	}
}


#define ____________________________USE_DI_FUNCTIONS_________________________________________________
static void	pal_fs_FormatCallback(HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (s_pfPalFormatProgress_NotifyCb)
	{
		(s_pfPalFormatProgress_NotifyCb)(pal_fs_DiEvent2PalEvent(ulEvent), ulParam1, ulParam2, ulParam3);
	}
}


HERROR PAL_FS_RegisterFormatCallback (PAL_FS_Event_Callback_Func pfEvtCallback)
{
	s_pfPalFormatProgress_NotifyCb = pfEvtCallback;
	return SEUAPI_RegisterFormatProgressCallback((DI_FS_Event_Callback_Func)pal_fs_FormatCallback);
}


#define _____MBR_RELATED_FUNCTIONS_____

// PAL_FS_XxxxDevMbrBuf 함수들을 사용하는데 반드시 필요한 초기 함수
// pal_fs 내의 Device MBR Buffer를 열어두어 Device MBR을 수정할 수 있도록 한다.
HERROR PAL_FS_OpenDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx)
{
	return SEUAPI_OpenDevMbrBuf((DI_FS_StorDevIdx_t)eDevIdx);
}

#if 0	// unused
// 열려 있는 Device MBR buffer 에서 특정 Partition 정보를 지우는 함수.
HERROR PAL_FS_DeletePartitionInDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	return DI_FS_DeletePartitionInDevMbrBuf(eDevIdx, ulPartIdx);
}
#endif

// 열려 있는 Device MBR buffer 에서 모든 Partition 정보를 지우는 함수.
HERROR PAL_FS_DeleteAllPartitionInDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx)
{
	return SEUAPI_DeleteAllPartitionInDevMbrBuf((DI_FS_StorDevIdx_t)eDevIdx);
}

// 열려 있는 Device MBR buffer 에서 특정 Partition 정보를 덮어 쓰는 함수.
HERROR PAL_FS_UpdatePartitionInDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, PAL_FS_FsType_t fsType)
{
	return SEUAPI_UpdatePartitionInDevMbrBuf((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, ulStartUnit, ulEndUnit, pal_fs_ConvertPalFsTypeToDIFsType(fsType));
}

HERROR PAL_FS_SetPartitionInDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT64 ullPartSizeKb, PAL_FS_FsType_t fsType)
{
	return SEUAPI_SetPartitionInDevMbrBuf((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, ullPartSizeKb, pal_fs_ConvertPalFsTypeToDIFsType(fsType));
}

// Device MBR buffer 의 내용을 실제 Storage Device의 MBR 영역에 덮어 쓰는 함수.
// PAL_FS_UpdatePartitionInDevMbrBuf 등으로 내용을 수정한 후에 실제로 덮어 쓰는 것이다.
HERROR PAL_FS_SyncDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx)
{
	return SEUAPI_SyncDevMbrBuf((DI_FS_StorDevIdx_t)eDevIdx);
}

// pal_fs 내의 Device MBR Buffer를 닫고 MBR 영역 file open한 것을 닫는다.
HERROR PAL_FS_CloseDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx)
{
	return SEUAPI_CloseDevMbrBuf((DI_FS_StorDevIdx_t)eDevIdx);
}

/*
		Device MBR Buffer 작업 없이 단독적으로 사용 가능한 MBR 함수들
*/
// 특정 Partition 정보를 읽어 오는 함수.
HERROR PAL_FS_GetPartitionFromDeviceMbr (PAL_FS_StorDevIdx_t eDevIdx, PAL_FS_PartitionList_t *pstPartList)
{
	return SEUAPI_GetPartitionFromDeviceMbr((DI_FS_StorDevIdx_t)eDevIdx, (DI_FS_PartitionList_t*)pstPartList);
}

#if 0	//unused
HERROR PAL_FS_EraseAllPartitionInDeviceMbr (PAL_FS_StorDevIdx_t eDevIdx)
{
#if 0//defined(CONFIG_ENHANCED_SECURITY)
	return SECAPI_FsEraseAllPartitionInDeviceMbr((DI_FS_StorDevIdx_t)eDevIdx);
#else
	return DI_FS_EraseAllPartitionInDeviceMbr((DI_FS_StorDevIdx_t)eDevIdx);
#endif
}
#endif

// Partition Format
#define _____FORMAT_FUNCTIONS_____

HERROR PAL_FS_InitFormatList (PAL_FS_FormatList_t *pstList)
{
	return SEUAPI_InitFormatList((DI_FS_FormatList_t *)pstList);
}

HERROR PAL_FS_AddFormatItemToList (PAL_FS_FormatList_t *pstList, PAL_FS_FormatItem_t *pstItem)
{
	return SEUAPI_AddFormatItemToList((DI_FS_FormatList_t *)pstList, (DI_FS_FormatItem_t *)pstItem);
}

HERROR PAL_FS_ClearFormatList (PAL_FS_FormatList_t *pstList)
{
	return SEUAPI_ClearFormatList((DI_FS_FormatList_t *)pstList);
}

HERROR PAL_FS_FormatLists (PAL_FS_FormatList_t *pstFormatList)
{
	return SEUAPI_FormatLists((DI_FS_FormatList_t*)pstFormatList, TRUE);
}


#define _____MOUNT_FUNCTIONS_____
STATIC HBOOL pal_fs_cmpMountInfoByDevIdxAndPartIdx (void *pvData, void *pvItem)
{
	_PAL_FS_MountInfo_t			*pstData = (_PAL_FS_MountInfo_t *)pvData;
	_PAL_FS_MountInfo_t			*pstItem = (_PAL_FS_MountInfo_t *)pvItem;

	if ((NULL != pstData) && (NULL != pstItem))
	{
		if ((pstData->eDevIdx == pstItem->eDevIdx) && (pstData->ulPartIdx == pstItem->ulPartIdx))
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC _PAL_FS_MountInfo_t *pal_fs_findMountInfo (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	_PAL_FS_MountInfo_t		 stMountInfo;
	HxList_t				*pstListItem;

	stMountInfo.eDevIdx		= eDevIdx;
	stMountInfo.ulPartIdx	= ulPartIdx;

	pstListItem = HLIB_LIST_FindEx (s_pstPalFsPart_MountList, (const void *)&stMountInfo, pal_fs_cmpMountInfoByDevIdxAndPartIdx);
	return (_PAL_FS_MountInfo_t *)HLIB_LIST_Data (pstListItem);
}

STATIC _PAL_FS_MountInfo_t *pal_fs_findMountInfoByFileNamePath (HCHAR *szFilePath)
{
	HUINT32					 ulFilePathLen, ulMountPathLen;
	_PAL_FS_MountInfo_t		*pstFoundInfo = NULL;
	HxList_t				*pstListItem = s_pstPalFsPart_MountList;

	if ((NULL == szFilePath) || ('\0' == szFilePath[0]))
	{
		return NULL;
	}

	ulFilePathLen = HxSTD_StrLen (szFilePath);

	for (pstListItem = s_pstPalFsPart_MountList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		_PAL_FS_MountInfo_t		*pstMountInfo = (_PAL_FS_MountInfo_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != pstMountInfo)
		{
			ulMountPathLen = HxSTD_StrLen (pstMountInfo->szDstMountDir);

			if ((ulFilePathLen > ulMountPathLen) &&
				(HxSTD_StrNCmp (szFilePath, pstMountInfo->szDstMountDir, ulMountPathLen) == 0))
			{
				if ((NULL == pstFoundInfo) ||
					(HxSTD_StrLen (pstFoundInfo->szDstMountDir) < ulMountPathLen))
				{
					pstFoundInfo = pstMountInfo;
				}
			}
		}
	}

	return pstFoundInfo;
}

STATIC HERROR pal_fs_makeRecycleDir (HCHAR *szMountDir)
{
	HCHAR			 szRecycleDir[MOUNT_PATH_LEN + 16];

	HxSTD_PrintToStrN (szRecycleDir, MOUNT_PATH_LEN + 16, "%s/%s", szMountDir, RECYCLE_BIN_DIR);

 	if (TRUE == HLIB_DIR_IsExist((const char *)szRecycleDir))
 	{
		return ERR_OK;
	}

 	if (ERR_OK == HLIB_DIR_Make((const char *)szRecycleDir))
 	{
#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if (HLIB_EXTRA_ChownWithParam(szRecycleDir, UID_umma, GID_pvr) != ERR_OK)
		{
			HxLOG_Error("Fail to HLIB_EXTRA_ChownWithParam (%s)\n", szRecycleDir);
		}

		if (HLIB_DIR_Chmod(szRecycleDir, 0770) != ERR_OK)
		{
			HxLOG_Error ("Fail to HLIB_DIR_Chmod (%s)\n", szRecycleDir);
		}
#endif
		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC HERROR pal_fs_getFileNameFromFullPath (HCHAR *szFullFileName, HCHAR *szFileName, HUINT32 ulFileNameMax)
{
	HCHAR	 szPath[HxPATH_MAX];
	HCHAR	*szRet;

	HLIB_STD_StrNCpySafe(szPath, szFullFileName, HxPATH_MAX);
	szRet = HLIB_PATH_Normalize(szPath);
	if (szRet == NULL)
		return ERR_FAIL;

	szRet = HLIB_PATH_FileName(szPath);
	if (szRet == NULL)
		return ERR_FAIL;

	HLIB_STD_StrNCpySafe(szFileName, szRet, ulFileNameMax);
	return ERR_OK;
}

HERROR PAL_FS_MountPartition (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, PAL_FS_FsType_t eFsType, HBOOL bSecured)
{
	_PAL_FS_MountInfo_t		*pstMountInfo;
	DI_ERR_CODE				 eDiErr;

	pstMountInfo = pal_fs_findMountInfo (eDevIdx, ulPartIdx);
	if (NULL != pstMountInfo)
	{
		if ((HxSTD_StrNCmp (szDstMountDir, pstMountInfo->szDstMountDir, MOUNT_PATH_LEN) == 0) &&
			(pstMountInfo->eFsType == eFsType))
		{
			// Already mounted as the requested:
			return ERR_OK;
		}

		PAL_FS_UnmountPartition (eDevIdx, ulPartIdx, pstMountInfo->eFsType, bSecured);
		pstMountInfo = NULL;
	}

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	eDiErr = SEUAPI_MountPartition((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, szDstMountDir, pal_fs_ConvertPalFsTypeToDIFsType(eFsType));
#else

#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM) // MOON_encr
{
	if (bSecured == TRUE)
	{
		eDiErr = SEUAPI_ENCR_Mount((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, szDstMountDir, pal_fs_ConvertPalFsTypeToDIFsType(eFsType));
	}
	else
	{
		eDiErr = SEUAPI_MountPartition((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, szDstMountDir, pal_fs_ConvertPalFsTypeToDIFsType(eFsType));
	}
}
#else
	eDiErr = SEUAPI_MountPartition((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, szDstMountDir, pal_fs_ConvertPalFsTypeToDIFsType(eFsType));
#endif

#endif
	if (DI_ERR_OK != eDiErr)
	{
		HxLOG_Error ("DI_FS_MountPartition err (eDevIdx:%d, ulPartIdx:%d, szDstMountDir:%s, eFsType:%d\n", eDevIdx, ulPartIdx, szDstMountDir, eFsType);
		return ERR_FAIL;
	}

#if 0
	pstMountInfo = (_PAL_FS_MountInfo_t *)HLIB_STD_MemCalloc (sizeof(_PAL_FS_MountInfo_t));
	if (NULL != pstMountInfo)
	{
		HERROR					 hErr;

		pstMountInfo->eDevIdx		= eDevIdx;
		pstMountInfo->ulPartIdx		= ulPartIdx;
		pstMountInfo->eFsType		= eFsType;
		HLIB_STD_StrNCpySafe(pstMountInfo->szDstMountDir, szDstMountDir, MOUNT_PATH_LEN);

		hErr = pal_fs_makeRecycleDir (pstMountInfo->szDstMountDir);
		pstMountInfo->bRecycleDir = (ERR_OK == hErr) ? TRUE : FALSE;

		s_pstPalFsPart_MountList = HLIB_LIST_Append (s_pstPalFsPart_MountList, (void *)pstMountInfo);

		PAL_FS_ClearRecycleBin (eDevIdx, ulPartIdx);
	}
#else
	pal_fs_makeRecycleDir (szDstMountDir);
#endif

	return ERR_OK;
}

#if 0


HERROR PAL_FS_MountPartitionExt (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, PAL_FS_FsType_t eFsType, HBOOL bSecured, PAL_FS_MNTOPT_e eMntOpt)
{
	_PAL_FS_MountInfo_t		*pstMountInfo;
	DI_ERR_CODE				 eDiErr;

	pstMountInfo = pal_fs_findMountInfo (eDevIdx, ulPartIdx);
	if (NULL != pstMountInfo)
	{
		if ((HxSTD_StrNCmp (szDstMountDir, pstMountInfo->szDstMountDir, MOUNT_PATH_LEN) == 0) &&
			(pstMountInfo->eFsType == eFsType))
		{
			// Already mounted as the requested:
			return ERR_OK;
		}

		PAL_FS_UnmountPartition (eDevIdx, ulPartIdx, pstMountInfo->eFsType, bSecured);
		pstMountInfo = NULL;
	}

	eDiErr = DI_FS_MountPartitionExt((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, szDstMountDir, pal_fs_ConvertPalFsTypeToDIFsType(eFsType), pal_fs_ConvertPalMntOptToDIMntOpt(eMntOpt));

	if (DI_ERR_OK != eDiErr)
	{
		HxLOG_Error ("DI_FS_MountPartition err (eDevIdx:%d, ulPartIdx:%d, szDstMountDir:%s, eFsType:%d\n", eDevIdx, ulPartIdx, szDstMountDir, eFsType);
		return ERR_FAIL;
	}

	pstMountInfo = (_PAL_FS_MountInfo_t *)HLIB_STD_MemCalloc (sizeof(_PAL_FS_MountInfo_t));
	if (NULL != pstMountInfo)
	{
		HERROR					 hErr;

		pstMountInfo->eDevIdx		= eDevIdx;
		pstMountInfo->ulPartIdx		= ulPartIdx;
		pstMountInfo->eFsType		= eFsType;
		HLIB_STD_StrNCpySafe(pstMountInfo->szDstMountDir, szDstMountDir, MOUNT_PATH_LEN);

		hErr = pal_fs_makeRecycleDir (pstMountInfo->szDstMountDir);
		pstMountInfo->bRecycleDir = (ERR_OK == hErr) ? TRUE : FALSE;

		s_pstPalFsPart_MountList = HLIB_LIST_Append (s_pstPalFsPart_MountList, (void *)pstMountInfo);

		PAL_FS_ClearRecycleBin (eDevIdx, ulPartIdx);
	}

	return ERR_OK;
}
#endif


HERROR PAL_FS_UnmountPartition (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, PAL_FS_FsType_t eFsType, HBOOL bSecured)
{
//	_PAL_FS_MountInfo_t		*pstMountInfo;
	DI_ERR_CODE				 eDiErr;

// MOON_encr
#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
	if (bSecured == TRUE)
	{
		eDiErr = SEUAPI_ENCR_Umount((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx);
	}
	else
	{
		eDiErr = SEUAPI_UmountPartition((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, pal_fs_ConvertPalFsTypeToDIFsType(eFsType));
	}
#else
	eDiErr = SEUAPI_UmountPartition((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, pal_fs_ConvertPalFsTypeToDIFsType(eFsType));
#endif

	if (DI_ERR_OK != eDiErr)
	{
		HxLOG_Error ("DI_FS_UnmountPartition err (eDevIdx:%d, ulPartIdx:%d, eFsType:%d\n", eDevIdx, ulPartIdx, eFsType);
		return ERR_FAIL;
	}

#if 0
	pstMountInfo = pal_fs_findMountInfo (eDevIdx, ulPartIdx);
	if (NULL != pstMountInfo)
	{
		s_pstPalFsPart_MountList = HLIB_LIST_Remove (s_pstPalFsPart_MountList, (const void *)pstMountInfo);
		HLIB_STD_MemFree (pstMountInfo);
	}
#endif

	return ERR_OK;
}

// File System에 따라 Access 하는 방식이 다르므로 일단 이렇게 argument를 받아서 구분시킨다.
// 차후에 어떻게 분리할 것인가에 대해 다른 방안을 모색해야 한다.
HERROR PAL_FS_GetMountedPartitionStatus (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, PAL_FS_FsType_t eFsType, HUINT64 *pullTotalKbSize, HUINT64 *pullUsedKbSize, HBOOL *pbWritable)
{
	return SEUAPI_GetMountedPartitionStatus((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, pal_fs_ConvertPalFsTypeToDIFsType(eFsType), pullTotalKbSize, pullUsedKbSize, pbWritable);
}

HERROR PAL_FS_GetRecycleDir (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HCHAR *szRecycleDir)
{
	_PAL_FS_MountInfo_t			*pstMountInfo;

	// s_ulRemoveCnt 와 ulTick은 모든 경우에 있어 Unique 한 File Name을 만들려고 하는 것임.

	pstMountInfo = pal_fs_findMountInfo (eDevIdx, ulPartIdx);
	if ((NULL == pstMountInfo) || (TRUE != pstMountInfo->bRecycleDir))
	{
		return ERR_FAIL;
	}

	if (NULL != szRecycleDir)
	{
		HxSTD_PrintToStrN (szRecycleDir, 128, "%s/%s/", pstMountInfo->szDstMountDir, RECYCLE_BIN_DIR);
	}

	return ERR_OK;
}

HERROR PAL_FS_GetRecycleDirByFileName (HCHAR *szFullFileName, HCHAR *szRecycleDir)
{
	_PAL_FS_MountInfo_t			*pstMountInfo;

	// s_ulRemoveCnt 와 ulTick은 모든 경우에 있어 Unique 한 File Name을 만들려고 하는 것임.

	pstMountInfo = pal_fs_findMountInfoByFileNamePath (szFullFileName);
	if ((NULL == pstMountInfo) || (TRUE != pstMountInfo->bRecycleDir))
	{
		return ERR_FAIL;
	}

	if (NULL != szRecycleDir)
	{
		HxSTD_PrintToStrN (szRecycleDir, 256, "%s/%s/", pstMountInfo->szDstMountDir, RECYCLE_BIN_DIR);
	}

	return ERR_OK;
}


HERROR PAL_FS_GetRemovingFileNameToRecycleDir (HCHAR *szFullFileName, HCHAR *szMovedFileName)
{
	STATIC HUINT32				 s_ulRemoveCnt = 1;
	unsigned long				 ulTick = VK_TIMER_GetSystemTick();
	_PAL_FS_MountInfo_t			*pstMountInfo;
	HERROR						 hErr;

	// s_ulRemoveCnt 와 ulTick은 모든 경우에 있어 Unique 한 File Name을 만들려고 하는 것임.

	pstMountInfo = pal_fs_findMountInfoByFileNamePath (szFullFileName);
	if ((NULL == pstMountInfo) || (TRUE != pstMountInfo->bRecycleDir))
	{
		return ERR_FAIL;
	}

	if (NULL != szMovedFileName)
	{
		HCHAR					 szFileName[256];

		hErr = pal_fs_getFileNameFromFullPath (szFullFileName, szFileName, 256);
		if (ERR_OK != hErr)
		{
			return ERR_FAIL;
		}

		HxSTD_PrintToStrN (szMovedFileName, 308, "%s/%s/%s_%d_%ld", pstMountInfo->szDstMountDir, RECYCLE_BIN_DIR, szFileName, s_ulRemoveCnt, ulTick);
		s_ulRemoveCnt ++;
	}

	return ERR_OK;
}

HERROR PAL_FS_GetMountedFileSystemByFileName (HCHAR *szFullFileName, PAL_FS_FsType_t *peFsType)
{
	_PAL_FS_MountInfo_t			*pstMountInfo;

	pstMountInfo = pal_fs_findMountInfoByFileNamePath (szFullFileName);
	if (NULL == pstMountInfo)
	{
		return ERR_FAIL;
	}

	if (NULL != peFsType)
	{
		*peFsType = pstMountInfo->eFsType;
	}

	return ERR_OK;
}

HERROR PAL_FS_PartitionCheckDisk(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, PAL_FS_FsType_t eFsType)
{
	return SEUAPI_ExecuteCheckDisk(eDevIdx, ulPartIdx, pal_fs_ConvertPalFsTypeToDIFsType(eFsType));
}

HERROR PAL_FS_GetMountRootByFileName (const HCHAR *szFullFileName, HCHAR *szMountRoot, HUINT32 n)
{
	_PAL_FS_MountInfo_t			*pstMountInfo;

	pstMountInfo = pal_fs_findMountInfoByFileNamePath ((HCHAR *)szFullFileName);
	if (NULL == pstMountInfo)
	{
		return ERR_FAIL;
	}

	if (NULL != szMountRoot && n > 0)
	{
		HLIB_STD_StrNCpySafe(szMountRoot, pstMountInfo->szDstMountDir, n);
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM) // MOON_encr
HERROR PAL_FS_ENCR_Format (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szPassword)
{
	return SEUAPI_ENCR_Format((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, szPassword);
}


HERROR PAL_FS_ENCR_Open (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMapperDeviceName, char *szPassword)
{
	return SEUAPI_ENCR_Open((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, szMapperDeviceName, szPassword);
}


HERROR PAL_FS_ENCR_Close (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	return SEUAPI_ENCR_Close((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx);
}
#endif


