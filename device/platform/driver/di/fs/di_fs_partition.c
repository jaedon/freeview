/**
	@file     	di_int_stor.c
	@brief    	internal storage event handler

	Description: 		\n
	Module: 		di/fs			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<malloc.h>
#endif
#include <htype.h>
#include <pwd.h>
#ifndef WIN32
#include <mntent.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
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

#if defined(CONFIG_EXT)
#include "ext2fs/ext2fs.h"
#include "ext2fs/ext2_types.h"
#include "uuid/uuid.h"
#include "e2p/e2p.h"
#endif
#include "di_fs.h"
#include "di_pvr.h"

#include "vkernel.h"
#include "htype.h"
#include "drv_err.h"

#include "di_hdd.h"
#include "di_uart.h"
#include "flash_map.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#ifdef DI_FS_PARTITION_DEBUG
HUINT32		g_ulDiFsPtt_DbgLevel = DBG_NONE;
#endif

#define UNUSED(x)	(x) = (x)
//#define DI_FS_PARTITION_DEBUG
#ifdef DI_FS_PARTITION_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)
#endif /* DI_FS_DEBUG */
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define MAX_MBR_BUFFER				2

#define DOSPART_OFFSET					446
#define DOSPART_SIZE					16
#define DOSPART_NUM						4

#define MAX_PRIMARY_PARTITION_NUM		4


#ifdef CONFIG_PROD_ENDIAN_BIG
#include <byteswap.h>
#define MACRO_LittleEndian32bit(_x)		bswap_32(_x)
#else
#define MACRO_LittleEndian32bit(_x)		(_x)
#endif

#define MACRO_GetDosPart(_b, _n)		((DI_FS_DosPartition_t *)((_b) + DOSPART_OFFSET + (_n) * DOSPART_SIZE))
#define MACRO_IsExtendedFsType(_x)		((_x) == eMBR_FS_TYPE_EXTENDED || \
				 						(_x) == eMBR_FS_TYPE_EXTD_LBA || \
				 						(_x) == eMBR_FS_TYPE_LINUX_EXT)

#define EXT_UUID_IN_SUPERBLOCK_OFFSET			(0x468)		/* the offset of UUID in the each partition's superblock */
#define EXT_UUID_IN_SUPERBLOCK_SIZE				16			/* the size of ext uuid */

#define EXT_VOLUMENAME_IN_SUPERBLOCK_OFFSET		(0x478)		/* the offset of Volume Name in the each partition's superblock */
#define EXT_VOLUMENAME_IN_SUPERBLOCK_SIZE		16			/* the size of ext volume name */

#define EXT_MAXMOUNTCOUNT_IN_SUPERBLOCK_OFFSET		(0x436)		/* the offset of max-mnt-count in the ext4 superblock */
#define EXT_MAXMOUNTCOUNT_IN_SUPERBLOCK_SIZE		2			/* the size of max-mnt-coun in the ext4 superblock */

#define EXT_CHECKINTERVAL_IN_SUPERBLOCK_OFFSET		(0x444)		/* the offset of check-interval in the ext4 superblock */
#define EXT_CHECKINTERVAL_IN_SUPERBLOCK_SIZE			4			/* the size of check-interval in the ext4 superblock */

#define FAT16_VOLUMENAME_IN_SUPERBLOCK_OFFSET	(0x2B)		/* The offset of volume name in the FAT16 superblock */
#define FAT16_VOLUMENAME_IN_SUPERBLOCK_SIZE		(11)		/* The size of volume name in the FAT32 superblock */

#define FAT32_VOLUMENAME_IN_SUPERBLOCK_OFFSET	(0x47)		/* The offset of volume name in the FAT32 superblock */
#define FAT32_VOLUMENAME_IN_SUPERBLOCK_SIZE		(11)		/* The size of volume name in the FAT32 superblock */

#define NTFS_VOLUEM_LABEL_TEMPORAL_FILE			"/tmp/ntfslabel"
#define NTFS_VOLUME_LABEL_SIZE					(32)		/* The size of volume label for NTFS */

#define FAT16_VOLID_IN_SUPERBLOCK_OFFSET	(0x27)
#define FAT16_VOLID_IN_SUPERBLOCK_SIZE		4

#define FAT32_VOLID_IN_SUPERBLOCK_OFFSET	(0x43)
#define FAT32_VOLID_IN_SUPERBLOCK_SIZE		4

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
} DI_FS_MbrFsType_t;

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
} DI_FS_DosPartition_t;

// MBR Info : Partition 정의를 위해 필요하다.
typedef struct
{
	HBOOL					 bAvailable;
	DI_FS_StorDevIdx_t		 eDevIdx;
	int						 nDevFileDsc;
	unsigned char			 aucMbrBuffer[MAX_SEC_SIZE];

	// 기타 필요 정보
	HUINT32					 ulHeads, ulSectors, ulSecSize;
	HUINT32					 ulSecsPerCyl, ulNumCylinders;
	HUINT64					 ullDiskSize, ullCylinderSize,ullSecPerDisk;
} DI_FS_MbrInfo_t;

enum {
	FS_UNKNOWN = 0,
	FS_NTFS = 10,
	FS_EXT2 = 11,
	FS_EXT3 = 12,
	FS_EXT4 = 13,
	FS_XFS = 14,
	FS_FAT16 = 15,
	FS_FAT32 = 16,
	FS_EXFAT = 17
};

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

// For FDISK executions:
STATIC pthread_mutex_t					 s_ulDiFsPtt_MbrLock = PTHREAD_MUTEX_INITIALIZER;
STATIC DI_FS_MbrInfo_t					 s_astDiFsPtt_MbrArray[MAX_MBR_BUFFER];

static DI_FS_Event_Callback_Func		 s_pfDiFormat_NotifyCb;

/*
 * XXX default configuration
 * base features = sparse_super, filetype, resize_inode, dir_index, ext_attr
 * block_size = 4096
 * inode_size = 256
 * inode_ration = 16384
 */
#define DI_FS_EXT2_BLOCKSIZE 			4096
#define DI_FS_EXT2_BLOCKLOGSIZE			2
#define DI_FS_EXT2_INODESIZE			256
#define DI_FS_EXT2_INODERATIO 			16384


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define _____LOCAL_FUNCTIONS_____

STATIC void local_difs_ExecuteCallback (HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (s_pfDiFormat_NotifyCb != NULL)
	{
		s_pfDiFormat_NotifyCb (ulEvent, ulParam1, ulParam2, ulParam3);
	}
}

#define _____MBR_RELATED_FUNCTIONS_____

INLINE STATIC void local_difs_ClearDosPartition (DI_FS_DosPartition_t *pstDosPart)
{
	pstDosPart->status = 0;
	pstDosPart->head = 0;
	pstDosPart->sect = 0;
	pstDosPart->cyl = 0;
	pstDosPart->type = 0;
	pstDosPart->end_head = 0;
	pstDosPart->end_sect = 0;
	pstDosPart->end_cyl = 0;
	pstDosPart->start = 0;
	pstDosPart->size = 0;
}

INLINE STATIC void local_difs_GetHscFromSector (HUINT32 ulSectorNo, HUINT32 ulSecPerHead, HUINT32 ulHeadPerCyl,
															HUINT8 *pucHead, HUINT8 *pucSector, HUINT8 *pucCylinder)
{
	// Reference : MBR_GRUB.pdf, http://www.boot-us.com/gloss.htm
	// MBR 에서 첫 섹터는 Cylinder = 0 / Head = 0 / sector = 1
	// 다음 Header에서는 C/H/S = 0/1/1 이므로 Sector No 자체가 +1 되는거 보다는 HSC의 S만 +1 이면 된다.
	// Sector 는 0-5 bit는 sector의 6bit 값, 6-7 bit는 CHS cylinder number의 상위 2bit를 의미한다.

	// Sector
	*pucSector	 = (HUINT8)((ulSectorNo % ulSecPerHead + 1) & 0x3F);		ulSectorNo /= ulSecPerHead;

	// Header
	*pucHead	 = (HUINT8)(ulSectorNo % ulHeadPerCyl);						ulSectorNo /= ulHeadPerCyl;

	// Cylinder : 상위 2bit가 sector 쪽 영역에 가는 것을 주의.
	*pucCylinder = (HUINT8)(ulSectorNo & 0xFF);
	*pucSector	|= (HUINT8)((ulSectorNo >> 2) & 0xC0);
}

STATIC DI_FS_MbrInfo_t *local_difs_GetDeviceMbr (DI_FS_StorDevIdx_t eDevIdx)
{
	DI_FS_MbrInfo_t		*pstMbrInfo;
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < MAX_MBR_BUFFER; ulIndex ++)
	{
		pstMbrInfo = &(s_astDiFsPtt_MbrArray[ulIndex]);

		if (pstMbrInfo->bAvailable == TRUE && pstMbrInfo->eDevIdx == eDevIdx)
		{
			return pstMbrInfo;
		}
	}

	return NULL;
}

STATIC DI_FS_MbrInfo_t *local_difs_GetEmptyDeviceMbr (void)
{
	DI_FS_MbrInfo_t		*pstMbrInfo;
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < MAX_MBR_BUFFER; ulIndex ++)
	{
		pstMbrInfo = &(s_astDiFsPtt_MbrArray[ulIndex]);

		if (pstMbrInfo->bAvailable == FALSE)
		{
			return pstMbrInfo;
		}
	}

	return NULL;
}

STATIC DI_ERR_CODE local_difs_OpenDeviceMbr (DI_FS_StorDevIdx_t eDevIdx)
{
	int						 nFileDsc;
	HUINT32					 ulHeads, ulSectors;
	HUINT32					 ulSecsPerCyl;
	HUINT8					 szDevFile[32];
	DI_FS_MbrInfo_t			 *pstMbrInfo;
	struct hd_geometry		 stGeometry;

	// ioctl을 통해 얻는 정보들
	unsigned int			 ulSecSize;
	unsigned long long		 ullDiskSize;
	unsigned long			 ulBlockNum;
	int nCnt = 0;

	PrintEnter();

	// 이미 열은 Device MBR이 있는지 확인
	pstMbrInfo = local_difs_GetDeviceMbr (eDevIdx);
	if (pstMbrInfo != NULL)
	{
		PrintError("[local_difs_OpenDeviceMbr] Already MBR Opened. %d\n", 0);
		PrintExit();
		return DI_ERR_OK;
	}

	// 비어 있는 MBR Info를 가져온다.
	pstMbrInfo = local_difs_GetEmptyDeviceMbr();
	if (pstMbrInfo == NULL)
	{
		PrintError("[local_difs_OpenDeviceMbr] MAXIMUM MBRs are opened. %d\n", 0);
		PrintExit();
		return DI_ERR_ERROR;
	}

	DI_FS_GetDeviceFileFullName (eDevIdx, szDevFile);

	for( nCnt = 0 ; nCnt < 3 ; nCnt++ )
	{
		nFileDsc = open ((char *)szDevFile, O_RDWR);
		if (nFileDsc == -1)
		{
			PrintError("[local_difs_OpenDeviceMbr] Device File (%s) open fail \n", szDevFile);
			VK_TASK_Sleep(50);
		}
		else
		{
			break;
		}
	}
	if (nCnt == 3)
	{
		PrintError("[local_difs_OpenDeviceMbr] Device File (%s) open fail \n", szDevFile);
		PrintExit();
		return DI_ERR_ERROR;
	}

	if (lseek (nFileDsc, 0LL, SEEK_SET) == -1)
	{
		PrintError("[local_difs_OpenDeviceMbr] cannot seek MBR. %d\n", 0);
		close (nFileDsc);
		PrintExit();
		return DI_ERR_ERROR;
	}

	if (ioctl (nFileDsc, BLKSSZGET, &ulSecSize) == -1)
	{
		ulSecSize = MIN_SEC_SIZE;
	}

	if (ioctl (nFileDsc, HDIO_GETGEO, &stGeometry) == -1)
	{
		ulHeads		= 255;
		ulSectors	= 63;
	}
	else
	{
		ulHeads		= stGeometry.heads;
		ulSectors	= stGeometry.sectors;
	}

	ulSecsPerCyl = ulHeads * ulSectors;

	// Disk Size 값을 알아오는 방법:
	// http://www.joinc.co.kr/modules/moniwiki/wiki.php/Code/C/GetDiskSize
	if (ioctl (nFileDsc, BLKGETSIZE64, &ullDiskSize) == -1)
	{
		if (ioctl (nFileDsc, BLKGETSIZE, &ulBlockNum) == -1)
		{
			ulBlockNum = 0;
		}

		ullDiskSize = ((unsigned long long)ulBlockNum / 1024) * ulSecSize;
	}

	if (read (nFileDsc, pstMbrInfo->aucMbrBuffer, ulSecSize) != (ssize_t)ulSecSize)
	{
		close (nFileDsc);
		PrintExit();
		return DI_ERR_ERROR;
	}

	pstMbrInfo->bAvailable	= TRUE;
	pstMbrInfo->eDevIdx		= eDevIdx;
	pstMbrInfo->nDevFileDsc	= nFileDsc;

	pstMbrInfo->ulHeads		= ulHeads;
	pstMbrInfo->ulSectors	= ulSectors;
	pstMbrInfo->ulSecSize	= (HUINT32)ulSecSize;
	pstMbrInfo->ullDiskSize		= ullDiskSize;
	pstMbrInfo->ullSecPerDisk	= ullDiskSize / (HUINT64)ulSecSize;
	pstMbrInfo->ulSecsPerCyl	= ulSecsPerCyl;
	pstMbrInfo->ulNumCylinders	= (HUINT32)(pstMbrInfo->ullSecPerDisk / (HUINT64)ulSecsPerCyl);
	pstMbrInfo->ullCylinderSize	= (HUINT64)ulSecsPerCyl * (HUINT64)ulSecSize;

	if ((pstMbrInfo->aucMbrBuffer[510] != 0x55) || (pstMbrInfo->aucMbrBuffer[511] != 0xAA))
	{
		PrintDebug("[local_difs_OpenDeviceMbr] Device File (%s) has not MBR.\n", szDevFile);

		// master boot record 설정
		VK_memset (pstMbrInfo->aucMbrBuffer, 0, sizeof(MAX_SEC_SIZE));
		pstMbrInfo->aucMbrBuffer[510] = 0x55;
		pstMbrInfo->aucMbrBuffer[511] = 0xaa;
	}

	/*close (nFileDsc);*//*handle shall be freed by calling local_difs_CloseDeviceMbr() when all jobs done.*/
	PrintExit();
	return DI_ERR_OK;
}

STATIC DI_ERR_CODE local_difs_DeletePartitionFromDeviceMbr (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	DI_FS_DosPartition_t	*pstDosPart;
	DI_FS_MbrInfo_t			*pstMbrInfo;

	PrintEnter();

	// Primary Partition은 4개까지
	if (ulPartIdx >= MAX_PRIMARY_PARTITION_NUM)
	{
		PrintExit();
		return DI_ERR_ERROR;
	}

	// 이미 열은 Device MBR이 있는지 확인
	pstMbrInfo = local_difs_GetDeviceMbr (eDevIdx);
	if (pstMbrInfo == NULL)
	{
		PrintError("[local_difs_DeletePartitionFromDeviceMbr] MBR not opened. %d\n", 0);
		PrintExit();
		return DI_ERR_ERROR;
	}

	pstDosPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, ulPartIdx);
	local_difs_ClearDosPartition (pstDosPart);

	PrintExit();
	return DI_ERR_OK;
}

STATIC DI_ERR_CODE local_difs_DeleteAllPartitionFromDeviceMbr (DI_FS_StorDevIdx_t eDevIdx)
{
	HUINT32					 ulIndex;
	DI_FS_DosPartition_t	*pstDosPart;
	DI_FS_MbrInfo_t			*pstMbrInfo;

	PrintEnter();

	// 이미 열은 Device MBR이 있는지 확인
	pstMbrInfo = local_difs_GetDeviceMbr (eDevIdx);
	if (pstMbrInfo == NULL)
	{
		PrintError("[local_difs_DeleteAllPartitionFromDeviceMbr] MBR not opened. %d\n", 0);
		PrintExit();
		return DI_ERR_ERROR;
	}

	for (ulIndex = 0; ulIndex < DOSPART_NUM; ulIndex++)
	{
		pstDosPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, ulIndex);
		local_difs_ClearDosPartition (pstDosPart);
	}

	PrintExit();

	return DI_ERR_OK;
}

STATIC DI_FileSystemType_t local_difs_GetFSInfo(HUINT8 *szPartFileName)
{
	FILE *fp;
	char szBuf[64] = {0,};
	DI_FileSystemType_t diFsType = DI_FS_OTHER_FILESYSTEM;

	PrintEnter();

	if (!szPartFileName)
	{
		PrintExit();
		return diFsType;
	}

	VK_snprintf(szBuf, sizeof(szBuf), "/sbin/probefs %s > /tmp/fstype", szPartFileName);

	VK_SYSTEM_Command(szBuf);

	fp = fopen("/tmp/fstype", "r");

	if (!fp)
	{
		PrintExit();
		return diFsType;
	}

	VK_MEM_Memset(szBuf, 0, sizeof(szBuf));

	fgets(szBuf, sizeof(szBuf)-1, fp);

	fclose(fp);

	if(!VK_strncmp(szBuf, "ntfs", 4))
	{
		diFsType = DI_FS_NTFS_FILESYSTEM;
	}
	else if(!VK_strncmp(szBuf, "ext2", 4))
	{
		diFsType = DI_FS_EXT2_FILESYSTEM;
	}
	else if(!VK_strncmp(szBuf, "ext3", 4))
	{
		diFsType = DI_FS_EXT3_FILESYSTEM;
	}
	else if(!VK_strncmp(szBuf, "ext4", 4))
	{
		diFsType = DI_FS_EXT4_FILESYSTEM;
	}
	else if(!VK_strncmp(szBuf, "xfs", 4))
	{
		diFsType = DI_FS_XFS_FILESYSTEM;
	}
	else if(!VK_strncmp(szBuf, "fat16", 5))
	{
		diFsType = DI_FS_FAT16_FILESYSTEM;
	}
	else if(!VK_strncmp(szBuf, "fat32", 5))
	{
		diFsType = DI_FS_FAT32_FILESYSTEM;
	}
	else if(!VK_strncmp(szBuf, "exfat", 5))
	{
		diFsType = DI_FS_EXFAT_FILESYSTEM;
	}

	PrintExit();

	return diFsType;
}


STATIC DI_ERR_CODE local_difs_SetPartitionToDeviceMbr (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, DI_FileSystemType_t eFsType)
{
	HUINT8					 ucHead, ucSector, ucCylinder;
	HUINT32					 ulStartSec, ulEndSec;
	HUINT32					 ulMinSec, ulMaxSec;
	DI_FS_DosPartition_t	*pstDosPart;
	DI_FS_MbrInfo_t			*pstMbrInfo;
	HUINT32 	 			ulBoundLow, ulBoundHigh;

	PrintEnter();

	// Primary Partition은 4개까지
	if (ulPartIdx >= MAX_PRIMARY_PARTITION_NUM)
	{
		PrintExit();
		return DI_ERR_ERROR;
	}

	// 이미 열은 Device MBR이 있는지 확인
	pstMbrInfo = local_difs_GetDeviceMbr (eDevIdx);
	if (pstMbrInfo == NULL)
	{
		PrintError("[local_difs_SetPartitionToDeviceMbr] MBR not opened. %d\n", 0);
		PrintExit();
		return DI_ERR_ERROR;
	}

	pstDosPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, ulPartIdx);

	if (0 != pstDosPart->type)
	{
		PrintError("[local_difs_SetPartitionToDeviceMbr] partition %d already defined. \n", eDevIdx);
		PrintExit();
		return DI_ERR_ERROR;
	}

	// Start Unit, End Unit 는 1이 시작점으로 해서 오는 것이며,
	// 이를 Sector로 계산하면 StartSector 와 EndSector의 계산법이 다를 수 밖에 없다.
	// (EX: Unit Size = 16sector 일 경우, Start Unit = 1, End Unit = 5 일 경우 Sector로 재환산하면
	//  Start Sector = (1 - 1) * 16 = 0 sector
	//  End Sector   = 5 * 16 - 1 = 79 sector
	ulStartSec	= (ulStartUnit > 0) ? (ulStartUnit - 1) * pstMbrInfo->ulSecsPerCyl : 0;
	ulEndSec	= (ulStartUnit > 0) ? (ulEndUnit * pstMbrInfo->ulSecsPerCyl) - 1 : 1;


#if 1
#define ALIGN_4KB_MASK  (0x00000007)

         if(ulStartSec & ALIGN_4KB_MASK)
                       ulStartSec &= ~ALIGN_4KB_MASK;
         if((ulEndSec+1) & ALIGN_4KB_MASK)
                       ulEndSec = (ulEndSec & (~ALIGN_4KB_MASK))-1;
#endif

	ulMinSec = 8;                                                                          // 앞의 MBR 은 피하려는 것.
	if(pstMbrInfo->ulNumCylinders == 0)
		ulMaxSec 	= pstMbrInfo->ulSecsPerCyl * 1024;
	else
		ulMaxSec 	= pstMbrInfo->ulSecsPerCyl * pstMbrInfo->ulNumCylinders -1;

	// MBR 쪽은 손대지 않도록 Start 는 재조정 필요.
	if (ulStartSec < ulMinSec)					{ ulStartSec = ulMinSec; }
	if (ulEndSec < ulMinSec)					{ ulEndSec = ulMinSec; }

	ulBoundLow = 0;
	ulBoundHigh = ulMaxSec; /* XXX heads * sectors * cylinders - 1;*/

	if (ulPartIdx > 0)
	{
		DI_FS_DosPartition_t	*pstPrevPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, ulPartIdx - 1);

		if (0 != pstPrevPart->type)
		{
			ulBoundLow = MACRO_LittleEndian32bit (pstPrevPart->start);
		}
	}

	if (ulPartIdx < (DOSPART_NUM-1))
	{
		DI_FS_DosPartition_t	*pstNextPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, ulPartIdx + 1);

		if (0 != pstNextPart->type)
		{
			ulBoundHigh = MACRO_LittleEndian32bit (pstNextPart->start);
		}
	}

	if (ulStartSec < ulBoundLow || ulEndSec > ulBoundHigh)
	{
		PrintExit();
		return DI_ERR_ERROR;
	}

	local_difs_ClearDosPartition (pstDosPart);

	// 여기서 start와 size는 실제 사이즈를 넣어줘야 한다.
	if (eFsType == DI_FS_NTFS_FILESYSTEM) {
		pstDosPart->type	= eMBR_FS_TYPE_HPFS_NTFS;
	}
	else if (eFsType == DI_FS_FAT32_FILESYSTEM) {
		pstDosPart->type = eMBR_FS_TYPE_FAT32;
	}
	else if (eFsType == DI_FS_FAT16_FILESYSTEM) {
		pstDosPart->type = eMBR_FS_TYPE_FAT16;
	}
	else {
		pstDosPart->type	= eMBR_FS_TYPE_LINUX;
	}
	pstDosPart->start	= (unsigned long)MACRO_LittleEndian32bit (ulStartSec);
	pstDosPart->size	= (unsigned long)MACRO_LittleEndian32bit (ulEndSec - ulStartSec + 1);

	// 여기서 startSec과 endSec이 MaxSec(1024*Secs per Cylinder)보다 크면 그 MaxSec만큼만 DosPartition에 적어줘야 한다.
	// MaxSec 값으로 이 시점에서 재조정하는 이유는 이 값이 위의 start, size에 영향을 주면 안되기 때문이다.
	if (ulStartSec >= ulMaxSec)			{ ulStartSec = ulMaxSec - 1; }
	local_difs_GetHscFromSector (ulStartSec, pstMbrInfo->ulSectors, pstMbrInfo->ulHeads, &ucHead, &ucSector, &ucCylinder);
	pstDosPart->head = ucHead;			pstDosPart->sect = ucSector;			pstDosPart->cyl = ucCylinder;

	if (ulEndSec >= ulMaxSec)				{ ulEndSec = ulMaxSec - 1; }
	local_difs_GetHscFromSector (ulEndSec, pstMbrInfo->ulSectors, pstMbrInfo->ulHeads, &ucHead, &ucSector, &ucCylinder);
	pstDosPart->end_head = ucHead;		pstDosPart->end_sect = ucSector;		pstDosPart->end_cyl = ucCylinder;

	PrintExit();

	return DI_ERR_OK;
}

static DRV_Error P_FS_SetPartitionToDeviceMbr (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT64 ullPartSizeKb, DI_FileSystemType_t eFsType)
{
	HCHAR	 					szFileName[50];
	HCHAR						szBuf[32] = {0,};
	HUINT8					 ucHead, ucSector, ucCylinder;
	HUINT64					 ullStartSec = 0, ullEndSec, ullMaxSec;
	DI_FS_DosPartition_t	*pstDosPart;
	DI_FS_MbrInfo_t			*pstMbrInfo;
	HUINT64 	 			ullBoundLow, ullBoundHigh;
	HUINT32				ulPhysicalSecSize=0;

	FILE	*fp;

	PrintEnter();

	/* Primary Partition은 4개까지 */
	if (ulPartIdx >= MAX_PRIMARY_PARTITION_NUM)
	{
		PrintExit();
		return DRV_ERR;
	}

	/* 이미 열은 Device MBR이 있는지 확인 */
	pstMbrInfo = local_difs_GetDeviceMbr (eDevIdx);
	if (pstMbrInfo == NULL)
	{
		PrintError("[P_FS_SetPartitionToDeviceMbr] MBR not opened. %d\n", 0);
		PrintExit();
		return DRV_ERR;
	}

	pstDosPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, ulPartIdx);
	if (0 != pstDosPart->type)
	{
		PrintError("[P_FS_SetPartitionToDeviceMbr] partition %d already defined. \n", eDevIdx);
		PrintExit();
		return DRV_ERR;
	}

	/* Check Physical sector size of HDD */
	VK_snprintf (szFileName, sizeof(szFileName) - 1, "/sys/block/sd%c/queue/physical_block_size", 'a' + eDevIdx - eDI_FS_DEV_sda);
	fp = fopen(szFileName, "r");
	if (!fp) {
		PrintError("[P_FS_SetPartitionToDeviceMbr] Can't open physical block size(%d).\n", ulPhysicalSecSize);
		PrintExit();
		return -1;
	}
	while(fgets(szBuf, sizeof(szBuf), fp)) {
		ulPhysicalSecSize=strtol(szBuf,NULL,10);
	}
	fclose(fp);

	/* Physical sector size is less than 4096, check 4k alignment */
	if(ulPhysicalSecSize > 4096)
	{
		PrintError("[P_FS_SetPartitionToDeviceMbr] Unknown Physical sector size(%d). \n", ulPhysicalSecSize);
		PrintExit();
		return DRV_ERR;
	}

	/* Set 1st Partition start sector is 2048 */
	if(ulPartIdx == DI_FS_PRIMARY_PARTITION_1)
	{
		ullStartSec	= 2048;
		ullEndSec	= (ullPartSizeKb*2) - ullStartSec + 1;
	}
	else
	{
		DI_FS_DosPartition_t	*pstPrevPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, ulPartIdx - 1);

		if (0 == pstPrevPart->type)
		{
			PrintError("[P_FS_SetPartitionToDeviceMbr] Unknown Partition #%d\n", ulPartIdx);
			PrintExit();
			return DRV_ERR;
		}

		ullStartSec = MACRO_LittleEndian32bit (pstPrevPart->start) + MACRO_LittleEndian32bit (pstPrevPart->size) + 1;
		ullEndSec	= ullStartSec + (ullPartSizeKb*2) - 1;
	}

	ullMaxSec = (pstMbrInfo->ullDiskSize/512) - 1;

#if 1
#define ALIGN_4KB_MASK  (0x00000007)

         if(ullStartSec & ALIGN_4KB_MASK)
                       ullStartSec &= ~ALIGN_4KB_MASK;
         if((ullEndSec+1) & ALIGN_4KB_MASK)
                       ullEndSec = (ullEndSec & (~ALIGN_4KB_MASK))-1;
#endif

	ullBoundLow = 0;
	ullBoundHigh = ullMaxSec;

	if (ulPartIdx > 0)
	{
		DI_FS_DosPartition_t	*pstPrevPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, ulPartIdx - 1);

		if (0 != pstPrevPart->type)
		{
			ullBoundLow = MACRO_LittleEndian32bit (pstPrevPart->start);
		}
	}

	if (ulPartIdx < (DOSPART_NUM-1))
	{
		DI_FS_DosPartition_t	*pstNextPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, ulPartIdx + 1);

		if (0 != pstNextPart->type)
		{
			ullBoundHigh = MACRO_LittleEndian32bit (pstNextPart->start);
		}
	}

	if ((ullStartSec < ullBoundLow) || (ullEndSec > ullBoundHigh))
	{
		PrintError("[P_FS_SetPartitionToDeviceMbr] Check Start(%llu) or End(%llu) Sector. \n", ullStartSec, ullEndSec);
		PrintExit();
		return DRV_ERR;
	}

	local_difs_ClearDosPartition (pstDosPart);

	// 여기서 start와 size는 실제 사이즈를 넣어줘야 한다.
	if (eFsType == DI_FS_NTFS_FILESYSTEM) {
		pstDosPart->type	= eMBR_FS_TYPE_HPFS_NTFS;
	}
	else if (eFsType == DI_FS_FAT32_FILESYSTEM) {
		pstDosPart->type = eMBR_FS_TYPE_FAT32;
	}
	else if (eFsType == DI_FS_FAT16_FILESYSTEM) {
		pstDosPart->type = eMBR_FS_TYPE_FAT16;
	}
	else {
		pstDosPart->type	= eMBR_FS_TYPE_LINUX;
	}
	pstDosPart->start	= (unsigned long)MACRO_LittleEndian32bit (ullStartSec);
	pstDosPart->size	= (unsigned long)MACRO_LittleEndian32bit (ullEndSec - ullStartSec + 1);

	// 여기서 startSec과 endSec이 MaxSec(1024*Secs per Cylinder)보다 크면 그 MaxSec만큼만 DosPartition에 적어줘야 한다.
	// MaxSec 값으로 이 시점에서 재조정하는 이유는 이 값이 위의 start, size에 영향을 주면 안되기 때문이다.
	if (ullStartSec >= ullMaxSec)			{ ullStartSec = ullMaxSec - 1; }
	local_difs_GetHscFromSector (ullStartSec, pstMbrInfo->ulSectors, pstMbrInfo->ulHeads, &ucHead, &ucSector, &ucCylinder);
	pstDosPart->head = ucHead;			pstDosPart->sect = ucSector;			pstDosPart->cyl = ucCylinder;

	if (ullEndSec >= ullMaxSec)				{ ullEndSec = ullMaxSec - 1; }
	local_difs_GetHscFromSector (ullEndSec, pstMbrInfo->ulSectors, pstMbrInfo->ulHeads, &ucHead, &ucSector, &ucCylinder);
	pstDosPart->end_head = ucHead;		pstDosPart->end_sect = ucSector;		pstDosPart->end_cyl = ucCylinder;

	PrintExit();

	return DRV_OK;
}

STATIC DI_ERR_CODE local_difs_GetPartitionInfo (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FS_MbrInfo_t *pstMbrInfo, DI_FS_DosPartition_t *pstDosPart, HUINT32 ulExtOffset, DI_FS_PartitionInfo_t *pstPartInfo)
{
	HUINT32	ulStart, ulSecNum;
	HUINT8 szPartFileName[32];

	PrintEnter();

	if (eMBR_FS_TYPE_EMPTY == pstDosPart->type)
	{
		pstPartInfo->bAvailable = FALSE;
		PrintExit();
		return DI_ERR_ERROR;
	}

	ulStart		= (HUINT32)MACRO_LittleEndian32bit (pstDosPart->start) + ulExtOffset;
	ulSecNum	= (HUINT32)MACRO_LittleEndian32bit (pstDosPart->size);

	DI_FS_GetPartitionFullFileName(eDevIdx, ulPartIdx, (HUINT8*)szPartFileName);
	pstPartInfo->eFsType = local_difs_GetFSInfo(szPartFileName);
	if (pstPartInfo->eFsType == DI_FS_OTHER_FILESYSTEM)
	{
		if (MACRO_IsExtendedFsType(pstDosPart->type))
		{
			pstPartInfo->ulStartUnit = ulStart;
			PrintError("[local_difs_GetPartitionInfo] Extended Partition!!! %d\n", 0);
			return DI_ERR_OK;
		}
		PrintError("[local_difs_GetPartitionInfo] Invalid FsType! [0x%x]\n", pstDosPart->type);
		return DI_ERR_ERROR;
	}
	pstPartInfo->ulStartUnit	= ulStart / pstMbrInfo->ulSecsPerCyl + 1;
	pstPartInfo->ulEndUnit		= (ulStart + ulSecNum) / pstMbrInfo->ulSecsPerCyl;
	pstPartInfo->bAvailable 	= TRUE;

	PrintExit();

	return DI_ERR_OK;
}

STATIC DI_ERR_CODE local_difs_GetPartitionList (DI_FS_StorDevIdx_t eDevIdx, DI_FS_PartitionList_t *pstPartList)
{
	HINT32					 nPartNum = 0;
	off_t					 ulExtBase = 0, ulExtOffset = 0;
	DI_FS_DosPartition_t	*pstDosPart = NULL;
	DI_FS_MbrInfo_t			*pstMbrInfo = NULL;

	PrintEnter();

	VK_memset (pstPartList, 0, sizeof(DI_FS_PartitionList_t));

	pstMbrInfo = local_difs_GetDeviceMbr (eDevIdx);
	if (pstMbrInfo == NULL)
	{
		PrintError("[local_difs_GetPartitionList] MBR not opened. %d\n", 0);
		PrintExit();
		return DI_ERR_ERROR;
	}

	DI_FS_GetDeviceFileFullName (eDevIdx, pstPartList->szDevFileName);
	pstPartList->ulSectorSize	= pstMbrInfo->ulSecSize;
	pstPartList->ulNumUnit		= pstMbrInfo->ulNumCylinders;
	pstPartList->ullUnitSize	= pstMbrInfo->ullCylinderSize;
	pstPartList->ullTotalSize	= pstMbrInfo->ullDiskSize;

	// Basic Partition
	for (nPartNum = 0; nPartNum < DOSPART_NUM; nPartNum++)
	{
		pstDosPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, nPartNum);
		local_difs_GetPartitionInfo (eDevIdx, nPartNum, pstMbrInfo, pstDosPart, 0, &(pstPartList->astPartition[nPartNum]));

		if (MACRO_IsExtendedFsType (pstDosPart->type))
		{
			ulExtBase = ulExtOffset = pstPartList->astPartition[nPartNum].ulStartUnit;
		}
	}

	// Extended Partition
	while (ulExtOffset != 0 && nPartNum < DI_FS_MAX_PARTITIONS)
	{
		HUINT32			 ulStart;
		off_t			 ulOffset = ulExtOffset * pstMbrInfo->ulSecSize;

		if (lseek (pstMbrInfo->nDevFileDsc, ulOffset, SEEK_SET) < 0) {
			break;
		}
		if (read (pstMbrInfo->nDevFileDsc, pstMbrInfo->aucMbrBuffer, pstMbrInfo->ulSecSize) != (ssize_t)pstMbrInfo->ulSecSize)
		{
			break;
		}

		pstDosPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, 0);
		local_difs_GetPartitionInfo (eDevIdx, nPartNum, pstMbrInfo, pstDosPart, ulExtOffset, &(pstPartList->astPartition[nPartNum]));
		nPartNum ++;

		pstDosPart = MACRO_GetDosPart (pstMbrInfo->aucMbrBuffer, 1);
		ulStart = (HUINT32)MACRO_LittleEndian32bit (pstDosPart->start);
		ulExtOffset = (0 != ulStart) ? ulExtBase + ulStart : 0;
	}

	while (nPartNum > 0)
	{
		if (pstPartList->astPartition[nPartNum - 1].bAvailable == TRUE)
		{
			pstPartList->ulNumPartition = (HUINT32)nPartNum;
			PrintExit();
			return DI_ERR_OK;
		}

		nPartNum --;
	}

	PrintExit();

	return DI_ERR_NO_PARTITIONS;
}

STATIC DI_ERR_CODE local_difs_WriteDeviceMbr (DI_FS_StorDevIdx_t eDevIdx)
{
	HUINT32					 ulSleepCount;
	DI_FS_MbrInfo_t			*pstMbrInfo;
	DI_ERR_CODE				 hErr = DI_ERR_ERROR;

	PrintEnter();

	// 이미 열은 Device MBR이 있는지 확인
	pstMbrInfo = local_difs_GetDeviceMbr (eDevIdx);
	if (pstMbrInfo == NULL)
	{
		PrintError("[local_difs_WriteDeviceMbr] MBR not opened. %d\n", 0);
		PrintExit();
		return DI_ERR_ERROR;
	}

	if (lseek (pstMbrInfo->nDevFileDsc, 0LL, SEEK_SET) == -1)
	{
		PrintError("[local_difs_WriteDeviceMbr] cannot seek MBR. %d\n", 0);
		PrintExit();
		return DI_ERR_ERROR;
	}

	if (write (pstMbrInfo->nDevFileDsc, pstMbrInfo->aucMbrBuffer, 512) != 512)
	{
		PrintError("[local_difs_WriteDeviceMbr] cannot write MBR. %d\n", 0);
		PrintExit();
		return DI_ERR_ERROR;
	}

	sync();
	VK_TASK_Sleep (5);

	// ioctl을 통해 Write 완료을 확인.
	for (ulSleepCount = 0; ulSleepCount < 5; ulSleepCount ++)
	{
		hErr = ioctl (pstMbrInfo->nDevFileDsc, BLKRRPART);
		if (hErr == 0)				{ break; }

		sync();
		VK_TASK_Sleep (5);
	}

	//return (ulSleepCount < 5) ? DI_ERR_OK : DI_ERR_ERROR;
	PrintExit();
	return DI_ERR_OK;
}

STATIC DI_ERR_CODE local_difs_CloseDeviceMbr (DI_FS_StorDevIdx_t eDevIdx)
{
	DI_FS_MbrInfo_t		*pstMbrInfo;

	PrintEnter();
	// 이미 열은 Device MBR이 있는지 확인
	pstMbrInfo = local_difs_GetDeviceMbr (eDevIdx);
	if (pstMbrInfo == NULL)
	{
		PrintError("[local_difs_CloseDeviceMbr] MBR not opened. %d\n", 0);
		PrintExit();
		return DI_ERR_ERROR;
	}

	close (pstMbrInfo->nDevFileDsc);
	VK_memset (pstMbrInfo, 0, sizeof(DI_FS_MbrInfo_t));

	PrintExit();
	return DI_ERR_OK;
}

#if defined(CONFIG_EXT)
#if 0 //not used
static DI_ERR_CODE local_difs_DisableE2fsckAutoCheckConditionExt(HUINT8 *dev)
{
	DI_ERR_CODE hErr = DI_ERR_ERROR;
	HUINT32 ulVolOffset = 0, ulVolSize = 0;
	HUINT16 usMaxMntCount = 0;
	HUINT32 ulCheckInterval = 0;

	/*  Adjust Max-Mount-Count.
	     If Max-Mount-Count is -1, the number of times the filesystem is mounted will be disregarded by e2fsck */
	ulVolOffset = EXT_MAXMOUNTCOUNT_IN_SUPERBLOCK_OFFSET;
	ulVolSize = EXT_MAXMOUNTCOUNT_IN_SUPERBLOCK_SIZE;
	usMaxMntCount = 0xffff;

	hErr = local_difs_Write_SuperBlock(dev, ulVolOffset, ulVolSize, (HUINT8*)&usMaxMntCount);
	if (hErr != DI_ERR_OK)
	{
		PrintError("local_difs_Write_SuperBlock fail. (%s)\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	/*  Adjust Check-Interval.
	     A value of zero will disable the time-dependent checking */
	ulVolOffset = EXT_CHECKINTERVAL_IN_SUPERBLOCK_OFFSET;
	ulVolSize = EXT_CHECKINTERVAL_IN_SUPERBLOCK_SIZE;
	ulCheckInterval = 0;

	hErr = local_difs_Write_SuperBlock(dev, ulVolOffset, ulVolSize, (HUINT8*)&ulCheckInterval);
	if (hErr != DI_ERR_OK)
	{
		PrintError("local_difs_Write_SuperBlock fail. (%s)\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#endif
#endif

#define _____FORMAT_FUNCTIONS_____

STATIC DI_FS_FormatList_t *local_difs_DuplicateFormatList (DI_FS_FormatList_t *pstList)
{
	DI_FS_FormatItem_t		*pstPartParam;
	DI_FS_FormatList_t		*pstDstList;

	PrintEnter();

	if (pstList == NULL)			{ return NULL; }

	pstDstList = (DI_FS_FormatList_t *)FS_MEM_Alloc (sizeof(DI_FS_FormatList_t));
	if (pstDstList == NULL)			{ return NULL; }

	VK_memset (pstDstList, 0, sizeof(DI_FS_FormatList_t));

	pstDstList->nNumCurrPartition = pstList->nNumCurrPartition;
	if (pstList->nNumFormatPartition > 0 && pstList->pstPartParam != NULL)
	{
		pstPartParam = (DI_FS_FormatItem_t *)FS_MEM_Alloc (sizeof(DI_FS_FormatItem_t) * pstList->nNumFormatPartition);
		if (pstPartParam == NULL)
		{
			FS_MEM_Free (pstDstList);
			PrintExit();
			return NULL;
		}

		VK_memcpy (pstPartParam, pstList->pstPartParam, sizeof(DI_FS_FormatItem_t) * pstList->nNumFormatPartition);

		pstDstList->nNumFormatPartition = pstList->nNumFormatPartition;
		pstDstList->pstPartParam = pstPartParam;
	}

	PrintExit();

	return pstDstList;
}

STATIC void local_difs_FreeFormatList (DI_FS_FormatList_t *pstList)
{
	if (pstList != NULL)
	{
		if (pstList->pstPartParam != NULL)
		{
			FS_MEM_Free (pstList->pstPartParam);
		}

		FS_MEM_Free (pstList);
	}
}

static int P_FS_FormatDisk(DI_FileSystemType_t eFsType, const char *szDev, const char *szLabel)
{
	int rc = -1;
	int eVkErr = VK_ERROR;
	char szBuf[128] = {0,};

	PrintEnter();

	DONE_ERR((!szDev));

	switch(eFsType) {
	case DI_FS_EXT3_FILESYSTEM:
		if(!szLabel || !szLabel[0])
		{
			VK_snprintf(szBuf, sizeof(szBuf), "mkfs.ext3 %s", szDev);
		}
		else
		{
			VK_snprintf(szBuf, sizeof(szBuf), "mkfs.ext3 %s -L \"%s\"", szDev, szLabel);
		}
		break;
	case DI_FS_EXT4_FILESYSTEM:
		if(!szLabel || !szLabel[0])
		{
			/* workaround : Ext4: Slow performance on first write after mount */
			VK_snprintf(szBuf, sizeof(szBuf), "mkfs.ext4 -O flex_bg -G 256 -F %s", szDev);
		}
		else
		{
			/* workaround : Ext4: Slow performance on first write after mount */
			VK_snprintf(szBuf, sizeof(szBuf), "mkfs.ext4 -O flex_bg -G 256 -F %s -L \"%s\"", szDev, szLabel);
		}
		break;
	case DI_FS_FAT16_FILESYSTEM:
		VK_snprintf(szBuf, sizeof(szBuf), "mkdosfs -F 16 %s", szDev);
		break;
	case DI_FS_FAT32_FILESYSTEM:
		VK_snprintf(szBuf, sizeof(szBuf), "mkdosfs -F 32 %s", szDev);
		break;
	case DI_FS_XFS_FILESYSTEM:
		VK_snprintf(szBuf, sizeof(szBuf), "mkfs.xfs -f %s", szDev);
		break;
	case DI_FS_NTFS_FILESYSTEM:
		VK_snprintf(szBuf, sizeof(szBuf), "mkntfs -f %s", szDev);
		break;
	case DI_FS_EXFAT_FILESYSTEM:
	default:
		DONE_ERR((1));
	}

	eVkErr = VK_SYSTEM_Command(szBuf);
	DONE_ERR((eVkErr != VK_OK));

	rc = 0;
done:
	PrintExit();
	return rc;
}

STATIC void *local_difs_FormatTask(void *data)
{
	int rc;
	int fail = 0;
	HINT32 nCount;
	DI_FS_StorDevIdx_t eDevIdx = eDI_FS_DEV_NULL;
	DI_FS_FormatList_t *pstList = (DI_FS_FormatList_t *)data;
	DI_FS_FormatItem_t *pstItem;
	HUINT8 szPartFileName[32];

	PrintEnter();

	if (!pstList || !(pstList->pstPartParam))
	{
		PrintExit();
		return NULL;
	}

	for (nCount=0; nCount<pstList->nNumFormatPartition; nCount++)
	{
		pstItem = &(pstList->pstPartParam[nCount]);
		eDevIdx = pstItem->eDevIdx;
		if (eDevIdx == eDI_FS_DEV_NULL)
		{
			continue;
		}

		DI_FS_GetPartitionFullFileName(eDevIdx, pstItem->ulPartitionNo, szPartFileName);
		rc = P_FS_FormatDisk(pstItem->ePartFsType, szPartFileName, pstItem->szVolumeName);
		if (rc < 0)
		{
			fail = 1;
		}

		local_difs_ExecuteCallback(DI_FS_EVENT_FORMAT_PROGRESS, (HUINT32)eDevIdx, (HUINT32)pstItem->ulPartitionNo, 0);
	}

	if (fail)
	{
		local_difs_ExecuteCallback(DI_FS_EVENT_FORMAT_FAIL, (HUINT32)eDevIdx, 0, 0);
	}
	else
	{
		local_difs_ExecuteCallback(DI_FS_EVENT_FORMAT_DONE, (HUINT32)eDevIdx, 0, 0);
	}

	local_difs_FreeFormatList(pstList);
	pthread_detach(pthread_self());

	PrintExit();

	return NULL;
}

#define _____MOUNTED_INFO_FUNCTIONS_____

INLINE STATIC HUINT64 local_difs_GetKbyteFromStatfs (unsigned long ulNumBlock, unsigned long ulBlockSize)
{
	// 반올림 계산을 한다.
	return ((HUINT64)ulNumBlock * (HUINT64)ulBlockSize + 1024/2) / 1024;
}

STATIC HBOOL local_difs_IsSuperUser(void)
{
	struct passwd *pw = NULL;
	uid_t uid = getuid();

	if (uid != 0) {
		return FALSE;
	}

	pw = getpwuid(uid);

	if (pw == NULL) {
		return FALSE;
	}

	if (pw->pw_name && VK_strncmp(pw->pw_name, "root", 4) == 0) {
		return TRUE;
	}

	return FALSE;
}


/*
	얻어오는 사이즈 단위는 Kbyte
*/
STATIC DI_ERR_CODE local_difs_GetMountedInfoFromMtab (HUINT8 *szPath, HUINT64 *pullTotalSize, HUINT64 *pullUsedSize, HBOOL *pbWriteEnable)
{
	char			*szMtabFilePath = "/proc/mounts";		/* It could be '/etc/mtab' under the some of RFS */
	char			*szDevicePath, *szMountedPath;
	HBOOL			 bFound = FALSE, bWriteEnabled = TRUE;
	HUINT64			 ullTotalSize = 0, ullUsedSize = 0;
	FILE			*pstMntTblFile = NULL;
	struct mntent 	*pstMntEntry = NULL;
	struct statfs	 stStatFs;
	long 			freeBlkCnt;
	HBOOL			ret;

	PrintEnter();

	pstMntTblFile = setmntent (szMtabFilePath, "r");
	if (!pstMntTblFile)
	{
		PrintError("[local_difs_GetSizeInfoFromMtab] setmntent err %d\n", 0);
		PrintExit();
		return DI_ERR_ERROR;
	}

	while (1)
	{
		if (pstMntTblFile)
		{
			pstMntEntry = getmntent (pstMntTblFile);
			if (!pstMntEntry)
			{
				endmntent (pstMntTblFile);
				break;
			}
		}

		szDevicePath	= pstMntEntry->mnt_fsname;
		szMountedPath	= pstMntEntry->mnt_dir;

		if (NULL != VK_strstr (szDevicePath, (char *)szPath) || NULL != VK_strstr (szMountedPath, (char *)szPath))
		{
			bFound = TRUE;

			// http://teamblog.joinc.co.kr/yundream/110
			// statfs() 설명 참조
			if (statfs (szMountedPath, &stStatFs) == 0)
			{
				if (stStatFs.f_blocks > 0)
				{
					// File System 내 여유공간은 f_ffree이고, 비 수퍼유저를 위한 여유 블럭은 f_bavail 이다.
					// 비 수퍼유저 권한으로 쓸 수 있는 공간 사이즈를 구해야 하므로 기존 코드에서 f_ffree로 된 것이
					// f_bavail로 바뀌어 있어야 한다.
					ullTotalSize	+= local_difs_GetKbyteFromStatfs (stStatFs.f_blocks, stStatFs.f_bsize);

					// 현재 STB의 실행 모드를 검사하여 사용 가능한 사이즈를 구한다.
					ret = local_difs_IsSuperUser();
					if (ret == TRUE) {
						freeBlkCnt = stStatFs.f_bfree;		// file system 내 free blocks
					}
					else {
						freeBlkCnt = stStatFs.f_bavail;		// non-super user의 free blocks
					}

					ullUsedSize		+= local_difs_GetKbyteFromStatfs (stStatFs.f_blocks - freeBlkCnt, stStatFs.f_bsize);
				}
			}

			// Write Enable/Disable
			if (VK_strstr(pstMntEntry->mnt_opts, "rw"))
			{
				bWriteEnabled = TRUE;
			}
			else if (VK_strstr(pstMntEntry->mnt_opts, "ro"))
			{
				bWriteEnabled = FALSE;
			}
		}
	}

	if (bFound == TRUE)
	{
		if(pullTotalSize != NULL)	*pullTotalSize	= ullTotalSize;
		if(pullUsedSize != NULL)	*pullUsedSize	= ullUsedSize;
		if(pbWriteEnable != NULL)	*pbWriteEnable	= bWriteEnabled;

		PrintExit();
		return DI_ERR_OK;
	}

	PrintExit();
	return DI_ERR_ERROR;
}

STATIC DI_ERR_CODE local_difs_FindMountedPathOfPartition (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType, char *szDstMountedDir, HUINT32 strBufSize)
{
	char			*szLineStr = NULL, *szDevName = NULL, *szMountedDir = NULL;
	char			 szFsType[12];
	char			 szPartFileName[32];
	FILE			*fpMntInfo = NULL;
	HBOOL			 bCheckDir = FALSE;
	DI_ERR_CODE			 hResult = DI_ERR_ERROR;
	char			szPartDevName[32];

	PrintEnter();
	// Make the source partition device file name
	switch (eFsType)
	{
	case DI_FS_MSDOS_FILESYSTEM:
	case DI_FS_OTHER_FILESYSTEM:
		DI_FS_GetDeviceFileFullName (eDevIdx, (HUINT8 *)szPartFileName);
		break;

	default:
		DI_FS_GetPartitionFullFileName (eDevIdx, ulPartIdx, (HUINT8 *)szPartFileName);
		break;
	}

	fpMntInfo = fopen ("/proc/mounts", "r");
	if (fpMntInfo == NULL)
	{
		PrintError("error : lack of resource!! %d\n", 0);
		goto END_FUNC;
	}

	szLineStr = (char *)FS_MEM_Alloc (256);
	szDevName = (char *)FS_MEM_Alloc (80);
	szMountedDir = (char *)FS_MEM_Alloc (80);

	if (szLineStr == NULL || szDevName == NULL || szMountedDir == NULL)
	{
		PrintError("error : lack of resource!! %d\n", 0);
		goto END_FUNC;
	}

	if (ulPartIdx == 0) {
		VK_snprintf(szPartDevName, sizeof(szPartDevName)-1, "/dev/sd%c", 'a'+eDevIdx);
		if (!VK_strcmp(szPartDevName, szPartFileName)) {
			VK_snprintf(szPartDevName, sizeof(szPartDevName)-1, "/dev/sd%c%d", 'a'+eDevIdx, ulPartIdx+1);
		}
		else {
			szPartDevName[0] = 0;
		}
	}

	// 실제로 USB 등과 같이 mount 정보는 남아 있으나 실제로 disconnect된 경우도 있을 수 있으므로
	// 참조 코드는 USB용으로 실제 device 상태를 따지지 않는 옛 DI_FS_UmountEx 을 참조하여 구현하였다.
	while (fgets (szLineStr, 255, fpMntInfo))
	{
		VK_sscanf (szLineStr, "%79s%79s%11s", szDevName, szMountedDir, szFsType);		// prevent fix

		/* doesn't device's stat because there might be no valid/active device node */
		switch (eFsType)
		{
		case DI_FS_MSDOS_FILESYSTEM:
		case DI_FS_OTHER_FILESYSTEM:
			bCheckDir = (NULL != VK_strstr (szDevName, szPartFileName)) ? TRUE : FALSE;
			break;

		default:
			bCheckDir = (0 == VK_strcmp (szPartFileName, szDevName)) ? TRUE : FALSE;
			if ((ulPartIdx == 0) && (!bCheckDir) && (szPartDevName[0] != 0)) {
				bCheckDir = (0 == VK_strcmp (szPartDevName, szDevName)) ? TRUE : FALSE;
			}
			break;
		}

		if (TRUE == bCheckDir)
		{
			if (szDstMountedDir != NULL)				{ VK_strncpy (szDstMountedDir, szMountedDir, strBufSize); }		// prevent fix
			hResult = DI_ERR_OK;
			goto END_FUNC;
		}
	}

END_FUNC:
	if (fpMntInfo != NULL)				{ fclose (fpMntInfo); }
	if (szLineStr != NULL)				{ FS_MEM_Free (szLineStr); }
	if (szDevName != NULL)				{ FS_MEM_Free (szDevName); }
	if (szMountedDir != NULL)			{ FS_MEM_Free (szMountedDir); }

	PrintExit();
	return hResult;
}

#define _____GLOBAL_FUNCTIONS_____

DI_ERR_CODE DI_FS_RegisterFormatProgressCallback (DI_FS_Event_Callback_Func pfEvtCallback)
{
  	PrintEnter();
	if (pfEvtCallback != NULL)
	{
		s_pfDiFormat_NotifyCb = pfEvtCallback;
	}

	PrintExit();
	return DI_ERR_OK;
}

#define _____MBR_RELATED_FUNCTIONS_____

STATIC DI_ERR_CODE local_difs_Mklabel(const HCHAR *dev, const HCHAR *labelType)
{
	DI_ERR_CODE 		hErr = DI_ERR_ERROR;
	char cmd[64];

	PrintEnter();
	VK_snprintf(cmd, sizeof(cmd)-1, "parted -s %s mklabel %s", dev, labelType);
	hErr=VK_SYSTEM_Command(cmd);
	if (hErr!=0) {
		PrintExit();
		return DI_ERR_ERROR;
	}
	PrintExit();
	return DI_ERR_OK;
}

STATIC DI_ERR_CODE local_difs_Parted(const HCHAR *dev, HUINT8 format_type, HUINT64 ullStartKb, HUINT64 ullEndKb)
{
	DI_ERR_CODE 		hErr = DI_ERR_ERROR;
	char				*szFsType = NULL;
	char cmd[1024];

	PrintEnter();
	PrintDebug("%s: Parted. ftype(%d)\n", __FUNCTION__, format_type);

	VK_memset(cmd,0,sizeof(cmd));

	switch (format_type)
	{
		case DI_FS_EXT2_FILESYSTEM:
			szFsType = "ext2";
			break;

		case DI_FS_EXT3_FILESYSTEM:
		case DI_FS_EXT4_FILESYSTEM:
/*  HDD 파티션을 생성할때 사용하는 parted 패키지(현재 버전:1.8.7)가 ext3까지 지원합니다.(1.8.7 상위버전은 GPL V 3.0)
하지만,  ext3로 파티션을 생성하고, mkfs.ext4로 HDD를 포맷한 결과 정상 동작하여 ext3 부분에 추가 했습니다.  */
			szFsType = "ext3";
			break;

		case DI_FS_XFS_FILESYSTEM:
			szFsType = "xfs";
			break;

		case DI_FS_FAT16_FILESYSTEM:
			szFsType = "fat16";
			break;
		case DI_FS_FAT32_FILESYSTEM:
			szFsType = "fat32";
			break;

		case DI_FS_NTFS_FILESYSTEM:
			szFsType = "ntfs";
			break;

		default:
			PrintError("[DI_FS_MountPartition] wrong FS Type (%d)\n", format_type);
			PrintExit();
			return DI_ERR_ERROR;
	}

	VK_snprintf(cmd, sizeof(cmd)-1, "parted -s %s unit KB mkpart primary %s %llu %llu", dev, szFsType, ullStartKb, ullEndKb);
	hErr=VK_SYSTEM_Command(cmd);
	if (hErr!=0)
	{
		PrintExit();
		return DI_ERR_ERROR;
	}
	PrintExit();
	return DI_ERR_OK;
}

STATIC DI_ERR_CODE local_difs_Parted_DeletePartition(const HCHAR *dev, HINT32 partition)
{
	DI_ERR_CODE 		hErr = DI_ERR_ERROR;
	char cmd[64];

	PrintEnter();
	VK_snprintf(cmd, sizeof(cmd)-1, "parted -s %s rm %d", dev, partition);
	hErr=VK_SYSTEM_Command(cmd);
	if (hErr!=0) {
		PrintExit();
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

// DI_FS_XxxxDevMbrBuf 함수들을 사용하는데 반드시 필요한 초기 함수
// di_fs 내의 Device MBR Buffer를 열어두어 Device MBR을 수정할 수 있도록 한다.
DI_ERR_CODE DI_FS_OpenDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx)
{
	DI_ERR_CODE			 hErr = DI_ERR_ERROR;

	PrintEnter();
	pthread_mutex_lock (&s_ulDiFsPtt_MbrLock);
	hErr = local_difs_OpenDeviceMbr (eDevIdx);
	pthread_mutex_unlock (&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

// 열려 있는 Device MBR buffer 에서 특정 Partition 정보를 지우는 함수.
DI_ERR_CODE DI_FS_DeletePartitionInDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	DI_ERR_CODE		 	hErr = DI_ERR_ERROR;
	PrintEnter();
	pthread_mutex_lock (&s_ulDiFsPtt_MbrLock);
	hErr = local_difs_DeletePartitionFromDeviceMbr (eDevIdx, ulPartIdx);
	pthread_mutex_unlock (&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

// 열려 있는 Device MBR buffer 에서 모든 Partition 정보를 지우는 함수.
DI_ERR_CODE DI_FS_DeleteAllPartitionInDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx)
{
	DI_ERR_CODE		 	hErr = DI_ERR_ERROR;
 	PrintEnter();
	pthread_mutex_lock (&s_ulDiFsPtt_MbrLock);
	hErr = local_difs_DeleteAllPartitionFromDeviceMbr (eDevIdx);
	pthread_mutex_unlock (&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

// 열려 있는 Device MBR buffer 에서 특정 Partition 정보를 덮어 쓰는 함수.
DI_ERR_CODE DI_FS_UpdatePartitionInDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, DI_FileSystemType_t eFsType)
{
	DI_ERR_CODE		 	hErr = DI_ERR_ERROR;

 	PrintEnter();
	pthread_mutex_lock (&s_ulDiFsPtt_MbrLock);
	hErr = local_difs_SetPartitionToDeviceMbr (eDevIdx, ulPartIdx, ulStartUnit, ulEndUnit, eFsType);
	pthread_mutex_unlock (&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

DI_ERR_CODE DI_FS_SetPartitionInDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT64 ullPartSizeKb, DI_FileSystemType_t eFsType)
{
	DRV_Error		 	eDrvErr;

 	PrintEnter();
	pthread_mutex_lock (&s_ulDiFsPtt_MbrLock);

	if(ullPartSizeKb > (2ull*1024ull*1024ull*1024ull-1ull))/*2TB-1*/
	{
		PrintError("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%d\n",0);
		PrintError("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%d\n",0);
		PrintError("[%s]Warning : DI_FS_SetPartitionInDevMbrBuf won't create partition larger than 2TB \n",__func__);
		PrintError("[%s]Warning : Try to use DI_FS_Parted \n",__func__);
		PrintError("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%d\n",0);
		PrintError("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%d\n",0);
		ullPartSizeKb =(2ull*1024ull*1024ull*1024ull-1ull);/*2TB-1*/
	}

	eDrvErr = P_FS_SetPartitionToDeviceMbr (eDevIdx, ulPartIdx, ullPartSizeKb, eFsType);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s] DI_FS_SetPartitionInDevMbrBuf Error!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
	pthread_mutex_unlock (&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return DI_ERR_OK;
}

// Device MBR buffer 의 내용을 실제 Storage Device의 MBR 영역에 덮어 쓰는 함수.
// DI_FS_UpdatePartitionInDevMbrBuf 등으로 내용을 수정한 후에 실제로 덮어 쓰는 것이다.
DI_ERR_CODE DI_FS_SyncDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx)
{
	DI_ERR_CODE			hErr = DI_ERR_ERROR;

	PrintEnter();
	pthread_mutex_lock (&s_ulDiFsPtt_MbrLock);
	hErr = local_difs_WriteDeviceMbr (eDevIdx);
	pthread_mutex_unlock (&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

// di_fs 내의 Device MBR Buffer를 닫고 MBR 영역 file open한 것을 닫는다.
DI_ERR_CODE DI_FS_CloseDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx)
{
	DI_ERR_CODE		 	hErr = DI_ERR_ERROR;
	PrintEnter();
	pthread_mutex_lock (&s_ulDiFsPtt_MbrLock);
	hErr = local_difs_CloseDeviceMbr (eDevIdx);
	pthread_mutex_unlock (&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

/*
		Device MBR Buffer 작업 없이 단독적으로 사용 가능한 MBR 함수들
*/
// 특정 Partition 정보를 읽어 오는 함수.
DI_ERR_CODE DI_FS_GetPartitionFromDeviceMbr (DI_FS_StorDevIdx_t eDevIdx, DI_FS_PartitionList_t *pstPartList)
{
	DI_ERR_CODE		 	hErr = DI_ERR_ERROR;

	PrintEnter();
	if (pstPartList == NULL)			{ return DI_ERR_ERROR; }

	pthread_mutex_lock (&s_ulDiFsPtt_MbrLock);
	hErr = local_difs_OpenDeviceMbr (eDevIdx);
	if (hErr == DI_ERR_OK)
	{
		hErr = local_difs_GetPartitionList (eDevIdx, pstPartList);
		local_difs_CloseDeviceMbr (eDevIdx);
	}
	pthread_mutex_unlock (&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

DI_ERR_CODE DI_FS_EraseAllPartitionInDeviceMbr (DI_FS_StorDevIdx_t eDevIdx)
{
	DI_ERR_CODE		 	hErr = DI_ERR_ERROR;

	PrintEnter();
	pthread_mutex_lock (&s_ulDiFsPtt_MbrLock);

	hErr = local_difs_OpenDeviceMbr (eDevIdx);
	if (hErr != DI_ERR_OK)
	{
		PrintError("error : lack of resource!! %d\n", 0);
		goto FINISH;
	}

	hErr = local_difs_DeleteAllPartitionFromDeviceMbr (eDevIdx);
	if (hErr != DI_ERR_OK)
	{
		PrintError("error : lack of resource!! %d\n", 0);
		goto FINISH;
	}

	hErr = local_difs_WriteDeviceMbr (eDevIdx);
	if (hErr != DI_ERR_OK)
	{
		PrintError("error : lack of resource!! %d\n", 0);
		goto FINISH;
	}

FINISH:
	local_difs_CloseDeviceMbr (eDevIdx);
	pthread_mutex_unlock (&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

DI_ERR_CODE DI_FS_PartedEraseAll(DI_FS_PartitionList_t *pstPartList)
{
	DI_ERR_CODE			hErr = DI_ERR_ERROR;
	HUINT32 i;

	PrintEnter();
	for (i=0; i<pstPartList->ulNumPartition; i++) {
		hErr=DI_FS_Parted_DeletePartition((char *)pstPartList->szDevFileName, i);
		if (hErr!=0) {
			return DI_ERR_ERROR;
		}
	}
	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE DI_FS_PartedAll(DI_FS_PartitionList_t *pstPartList, HUINT8 format_type)
{
	DI_ERR_CODE			hErr = DI_ERR_ERROR;
	HUINT32 i;

	PrintEnter();
	for (i=0; i<pstPartList->ulNumPartition; i++) {
		hErr=DI_FS_Parted_DeletePartition((char *)pstPartList->szDevFileName, i);
		if (hErr!=0) {
			return DI_ERR_ERROR;
		}
	}

	for (i=0; i<pstPartList->ulNumPartition; i++) {
		hErr=DI_FS_Parted((char *)pstPartList->szDevFileName, i, format_type, pstPartList->ulSectorSize, pstPartList->astPartition[i].ulStartUnit,
																		pstPartList->astPartition[i].ulEndUnit);
		if (hErr!=0) {
			return DI_ERR_ERROR;
		}
	}
	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE DI_FS_Parted_Mklabel(const HCHAR *dev, const HCHAR *labelType)
{
	DI_ERR_CODE			hErr = DI_ERR_ERROR;

	PrintEnter();
	pthread_mutex_lock(&s_ulDiFsPtt_MbrLock);
	hErr=local_difs_Mklabel(dev, labelType);
	pthread_mutex_unlock(&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

DI_ERR_CODE DI_FS_Parted(const HCHAR *dev, HINT32 partition, HUINT8 format_type, HUINT32 secsize, HUINT64 ullStartKb, HUINT64 ullEndKb)
{
	DI_ERR_CODE 		hErr = DI_ERR_ERROR;

	UNUSED(partition);
	UNUSED(secsize);

	PrintEnter();

	if(ullStartKb >= ullEndKb)
	{
		PrintError("Can not have the end before the start!! %llu %llu\n",ullStartKb,ullEndKb);
		return DI_ERR_ERROR;
	}

	/*check 1Mbyte align*/
	if(ullStartKb < 1024)
	{
		ullStartKb = 1024;
		if(ullEndKb > 1024)
		{
			ullEndKb = ullEndKb-1024;
		}
		else
		{
			PrintError("ullEndKb is wrong!! %llu \n",ullEndKb);
			return DI_ERR_ERROR;
		}
	}
	else if(ullStartKb&(0x3ff))
	{
		ullStartKb = (ullStartKb&~0x3ff);
		ullStartKb = ullStartKb+0x400;
		ullEndKb = ullEndKb-0x400;
	}

	pthread_mutex_lock(&s_ulDiFsPtt_MbrLock);
	hErr=local_difs_Parted(dev, format_type, ullStartKb, ullEndKb);
	pthread_mutex_unlock(&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

DI_ERR_CODE DI_FS_Parted_DeletePartition(const HCHAR *dev, HINT32 partition)
{
	DI_ERR_CODE 		hErr = DI_ERR_ERROR;

	PrintEnter();
	pthread_mutex_lock(&s_ulDiFsPtt_MbrLock);
	hErr=local_difs_Parted_DeletePartition(dev, partition);
	pthread_mutex_unlock(&s_ulDiFsPtt_MbrLock);
	PrintExit();
	return hErr;
}

// Partition Format
#define _____FORMAT_FUNCTIONS_____

DI_ERR_CODE DI_FS_InitFormatList (DI_FS_FormatList_t *pstList)
{
	PrintEnter();
	if (pstList == NULL)			{ return DI_ERR_ERROR; }

	VK_memset (pstList, 0, sizeof(DI_FS_FormatList_t));
	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE DI_FS_AddFormatItemToList (DI_FS_FormatList_t *pstList, DI_FS_FormatItem_t *pstItem)
{
	DI_FS_FormatItem_t		*pstNewArray;

	PrintEnter();
	if (pstList == NULL || pstItem == NULL)			{ return DI_ERR_ERROR; }

	if (pstList->nNumFormatPartition >= pstList->nMaxFormatPartition)
	{
		pstNewArray = FS_MEM_Alloc (sizeof(DI_FS_FormatItem_t) * (pstList->nMaxFormatPartition + 5));
		if (pstNewArray == NULL)					{ return DI_ERR_ERROR; }

		if (pstList->nNumFormatPartition > 0 && pstList->pstPartParam != NULL)
		{
			VK_memcpy (pstNewArray, pstList->pstPartParam, sizeof(DI_FS_FormatItem_t) * pstList->nNumFormatPartition);
			FS_MEM_Free (pstList->pstPartParam);
		}

		pstList->nMaxFormatPartition += 5;
		pstList->pstPartParam = pstNewArray;
	}

	VK_memcpy (&(pstList->pstPartParam[pstList->nNumFormatPartition]), pstItem, sizeof(DI_FS_FormatItem_t));
	pstList->nNumFormatPartition ++;

	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE DI_FS_ClearFormatList (DI_FS_FormatList_t *pstList)
{
	PrintEnter();
	if (pstList == NULL)			{ return DI_ERR_ERROR; }

	if (pstList->pstPartParam != NULL)
	{
		FS_MEM_Free (pstList->pstPartParam);
	}

	VK_memset (pstList, 0, sizeof(DI_FS_FormatList_t));
	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE DI_FS_FormatLists(DI_FS_FormatList_t *pstFormatList, HUINT32 nonblock)
{
	DI_FS_FormatList_t		*pstDstList;
	int status;
	pthread_t tid;

	PrintEnter();
	pstDstList = local_difs_DuplicateFormatList(pstFormatList);
	if (pstDstList == NULL)				{ return DI_ERR_ERROR; }

	if (pthread_create(&tid, NULL, local_difs_FormatTask, pstDstList))
	{
		PrintError("[DI_FS_FormatLists] cannot create format thread %d\n", 0);
		return DI_ERR_ERROR;
	}

	if (nonblock==FALSE) {
		pthread_join(tid, (void **)&status);
		PrintError("[DI_FS_FormatLists] cannot create format thread status : %d\n", status);
	}
	PrintExit();
	return DI_ERR_OK;
}

#define _____MOUNT_FUNCTIONS_____

#define XFS_REPAIR_CNT 							3
#define XFS_REPAIR_CHECK_DATA 					"RUN"
#define XFS_REPAIR_CHECK_DATA_SIZE 				3

int local_difs_Hdd_XFS_check(const char *dstnode)
{
	int nRet;
	char filepath[64]={0,};
	char buf[32]={0,};
	int fd;

	PrintEnter();
	VK_snprintf(filepath, 64, "%s/XFSCHECK", dstnode);
	fd = open(filepath, O_RDWR|O_CREAT|O_TRUNC|O_APPEND|O_NONBLOCK|O_LARGEFILE, 0666);
	if (fd < 0) {
		PrintExit();
		return -1;
	}

	nRet = write(fd, XFS_REPAIR_CHECK_DATA, XFS_REPAIR_CHECK_DATA_SIZE);
	if (nRet != XFS_REPAIR_CHECK_DATA_SIZE) {
		goto err;
	}

	sync();
	fsync(fd);						/* flush buffers */
	fdatasync(fd);					/* flush buffers */
	sync();

	nRet = lseek (fd, 0LL, SEEK_SET);
	if (nRet < 0) {
		goto err;
	}

	nRet = read(fd, buf, XFS_REPAIR_CHECK_DATA_SIZE);
	if (nRet != XFS_REPAIR_CHECK_DATA_SIZE) {
		goto err;
	}

#if 0
	nRet = unlink(filepath);
	if (nRet!=0) {
		switch (errno) {
			case EACCES:
				nRet = -101;
				goto err;
			case ENOENT:
				nRet = -102;
				goto err;
			default:
				nRet = -103;
				goto err;
		}
	}
#else
	nRet = remove(filepath);
	if (0 != nRet) {
		goto err;
	}
#endif

	nRet = VK_strncmp(buf, XFS_REPAIR_CHECK_DATA, XFS_REPAIR_CHECK_DATA_SIZE);
	if (0 != nRet) {
		goto err;
	}

	close(fd);
	PrintExit();
	return 0;

err:
	/* when occuring errors */
	close(fd);
	PrintExit();
	return -1;
}

int local_difs_Hdd_XFS_repair(const char *devname, HUINT32 repaircnt)
{
	int nRet=0;
	char cmd[128]={0,};
	int cnt;

	cnt = repaircnt;
	VK_snprintf(cmd, 128, "xfs_repair -P -L %s", devname);
	while (0<cnt) {
		nRet = VK_SYSTEM_Command(cmd);
		if (0==nRet) break;

		usleep(100);
		cnt--;
	}
	return nRet;
}

int local_difs_Hdd_XFS_repaircheck(const char *devname, const char *dstnode, const char *filesystem, const char *FilterOpts,  HUINT32 repaircnt)
{
	int nRet;

	nRet = mount(devname, dstnode, filesystem, 0, FilterOpts);
	if (0!=nRet) {
		nRet = local_difs_Hdd_XFS_repair(devname, repaircnt);
		if (0!=nRet) {
			return -1;
		}

		nRet = mount(devname, dstnode, filesystem, 0, FilterOpts);
		if (0!=nRet) {
			return -1;
		}
	}

	nRet = local_difs_Hdd_XFS_check(dstnode);
	if (0!=nRet) {
		(void)umount(dstnode);
		nRet = local_difs_Hdd_XFS_repair(devname, repaircnt);
		if (0!=nRet) {
			return -1;
		}

		nRet = mount(devname, dstnode, filesystem, 0, FilterOpts);
		if (0!=nRet) {
			return -1;
		}
	}
	return 0;
}

int local_difs_Hdd_XFS_Mount(const char *devname, const char *dstnode, const char *filesystem, const char *FilterOpts, HUINT32 repaircnt)
{
	int nRet;

	nRet = local_difs_Hdd_XFS_repaircheck(devname, dstnode, filesystem, FilterOpts, repaircnt);
	if (0!=nRet) {
		nRet = local_difs_Hdd_XFS_repaircheck(devname, dstnode, filesystem, FilterOpts, repaircnt);
	}

	return nRet;
}

#if defined(CONFIG_EMMC_FLASH)
static int local_difs_EmmcMountAllPartitions(void)
{
	if(VK_SYSTEM_Command("/etc/init.d/S35mountall.sh") != VK_OK)
	{
		return -1;
	}

	return 0;
}

static int local_difs_EmmcUmountAllPartitions(void)
{
	FILE *fp;
	struct mntent *tMntEnt;
	struct emmc_fs_partition_t *map = (struct emmc_fs_partition_t *)emmc_fs_partition_map;

	fp = setmntent("/proc/mounts", "r");
	if (!fp)
	{
		return -1;
	}

	while(1)
	{
		if (!(tMntEnt = getmntent(fp)))
			break;

		if (!VK_strstr(tMntEnt->mnt_fsname, N_PART_FSAREA_NODE))
			continue;

		for (; map->name; map++)
		{
			if (VK_strstr(tMntEnt->mnt_fsname, map->devnode))
			{
				PrintDebug("[%s] umount partition(mnt_dir:%s)...\n", __FUNCTION__, tMntEnt->mnt_dir);
				umount(tMntEnt->mnt_dir);
				break;
			}
		}
	}
	endmntent(fp);

	return 0;
}
#elif defined(CONFIG_NAND_FLASH)
/* Function : P_StoreUmountInfo
                 : Function for storing unmount information when failing to umount
*/
static void local_difs_NandflashStoreUmountInfo(const char *szMountedDir, int length)
{
#define NAMELEN	256
	DIR 			*dir;
	DIR 			*d_fd;
	struct dirent	*entry;
	int 			pid;
	int 			len;
	char			name[NAMELEN] = {0,};
	char			linkname[NAMELEN] = {0,};
	char			exename[NAMELEN] = {0,};

	if (!szMountedDir || !length ) {
		return;
	}

	/* open /proc directory */
	dir = opendir("/proc");
	if (!dir) {
		return;
	}

	/* write directory name which has problem for umount */
	VK_MEM_Memset(name, 0, NAMELEN);
	VK_snprintf(name, NAMELEN-1, "echo %s > %s/.umount-proc-info", szMountedDir, szMountedDir);
	VK_SYSTEM_Command(name);

	/* get directories in /proc dir */
	while((entry = readdir(dir)) != NULL) {
		/* parse pid from directory name */
		pid = strtol(entry->d_name, NULL, 10);
		if (pid == 0 || getpid() == pid) {
			continue;
		}

		/* parse execute-file name */
		VK_MEM_Memset(name, 0, NAMELEN);
		VK_MEM_Memset(exename, 0, NAMELEN);
		VK_snprintf(name, NAMELEN-1, "/proc/%u/exe", pid);
		len = readlink(name, exename, NAMELEN-1);

		/* check fd lists that are open */
		VK_MEM_Memset(name, 0, NAMELEN);
		VK_snprintf(name, NAMELEN-1, "/proc/%u/fd/", pid);
		d_fd = opendir(name);
		if (!d_fd) {
			continue;
		}

		/* parse fd lists */
		while((entry = readdir(d_fd)) != NULL) {
			if (entry->d_type == DT_LNK) {
				VK_MEM_Memset(name, 0, NAMELEN);
				VK_MEM_Memset(linkname, 0, NAMELEN);
				VK_snprintf(name, NAMELEN-1, "/proc/%u/fd/%s", pid, entry->d_name);
				len = readlink(name, linkname, NAMELEN-1);

				/* compare folder name with target dir for umount */
				if (VK_strncmp(linkname, szMountedDir, length) == 0) {
					VK_MEM_Memset(name, 0, NAMELEN);
					VK_snprintf(name, NAMELEN-1, "echo \"%d\t%s\t%s\" >> %s/.umount-proc-info\n", pid, exename, linkname, szMountedDir);
					VK_SYSTEM_Command(name);
				}
			}
		}
		closedir(d_fd);
	}
	closedir(dir);
	VK_MEM_Memset(name, 0, NAMELEN);
	VK_snprintf(name, NAMELEN-1, "ps >> %s/.umount-proc-info\n", szMountedDir);
	VK_SYSTEM_Command(name);

	return;
}

DI_ERR_CODE	local_difs_NandflashMountPartitions(void)
{
	if(VK_SYSTEM_Command("/etc/init.d/S35mountall.sh") != 0)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE	local_difs_NandflashUmountPartitions(void)
{
	struct mntent	*tMntEnt;
	FILE			*fp;
	int				err;
#if defined(CONFIG_PRODUCT_IMAGE_DEBUG)
	char			cmd[256];
#endif

	fp = setmntent("/proc/mounts","r");
	if (!fp) {
		return DI_ERR_ERROR;
	}

	while(1) {
		tMntEnt = getmntent(fp);
		if (!tMntEnt)
			break;
		if (VK_strncmp(tMntEnt->mnt_type, "ubifs", VK_strlen("ubifs"))==0) {
			err = umount(tMntEnt->mnt_dir);
			if (err < 0) {
				PrintError("[%s] Error!! Can't umount %s directory\n", __FUNCTION__, tMntEnt->mnt_dir);

#if defined(CONFIG_PRODUCT_IMAGE_DEBUG)
				/* if debug mode, print open file lists */
				VK_MEM_Memset(cmd, 0, sizeof(cmd));
				VK_snprintf(cmd, sizeof(cmd)-1, "lsof | grep %s", tMntEnt->mnt_dir);
				VK_SYSTEM_Command(cmd);
#endif

				/* Store open file lists on target root dir */
				local_difs_NandflashStoreUmountInfo(tMntEnt->mnt_dir, VK_strlen(tMntEnt->mnt_dir));

				endmntent(fp);
				return DI_ERR_ERROR;
			}
		}
	}
	endmntent(fp);

	PrintError("[%s] Success! Umount Done\n", __FUNCTION__);
	return DI_ERR_OK;
}

#endif

static void P_FS_StoreUmountInfo(const char *szMountedDir, int length)
{
#define NAMELEN	256
	DIR				*dir;
	DIR				*d_fd;
	struct dirent	*entry;
	int				pid;
	int				len;
	char 			name[NAMELEN] = {0,};
	char 			linkname[NAMELEN] = {0,};
	char			exename[NAMELEN] = {0,};

	if (!szMountedDir || !length ) {
		return;
	}

	/* open /proc directory */
	dir = opendir("/proc");
	if (!dir) {
		return;
	}

	/* write directory name which has problem for umount */
	VK_MEM_Memset(name, 0, NAMELEN);
	VK_snprintf(name, NAMELEN-1, "echo %s > %s/.umount-proc-info", szMountedDir, szMountedDir);
	VK_SYSTEM_Command(name);

	/* get directories in /proc dir */
	while((entry = readdir(dir)) != NULL) {
		/* parse pid from directory name */
		pid = strtol(entry->d_name, NULL, 10);
		if (pid == 0) {
			continue;
		}

		/* parse execute-file name */
		VK_MEM_Memset(name, 0, NAMELEN);
		VK_MEM_Memset(exename, 0, NAMELEN);
		VK_snprintf(name, NAMELEN-1, "/proc/%u/exe", pid);
		len = readlink(name, exename, NAMELEN-1);

		/* check fd lists that are open */
		VK_MEM_Memset(name, 0, NAMELEN);
		VK_snprintf(name, NAMELEN-1, "/proc/%u/fd/", pid);
		d_fd = opendir(name);
		if (!d_fd) {
			continue;
		}

		/* parse fd lists */
		while((entry = readdir(d_fd)) != NULL) {
			if (entry->d_type == DT_LNK) {
				VK_MEM_Memset(name, 0, sizeof(name));
				VK_MEM_Memset(linkname, 0, sizeof(linkname));
				VK_snprintf(name, sizeof(name), "/proc/%u/fd/%s", pid, entry->d_name);
				len = readlink(name, linkname, sizeof(name)-1);
				linkname[sizeof(name)-1] = 0;

				/* compare folder name with target dir for umount */
				if (VK_strstr(linkname, szMountedDir) != NULL) {
					VK_MEM_Memset(name, 0, NAMELEN);
					VK_snprintf(name, NAMELEN-1, "echo \"%d\t%s\t%s\" >> %s/.umount-proc-info\n", pid, exename, linkname, szMountedDir);
					VK_SYSTEM_Command(name);
				}
			}
		}
		closedir(d_fd);
	}
	closedir(dir);

	/* store process info */
	VK_MEM_Memset(name, 0, NAMELEN);
	VK_snprintf(name, NAMELEN-1, "ps >> %s/.umount-proc-info\n", szMountedDir);
	VK_SYSTEM_Command(name);

	return;
}

DI_ERR_CODE DI_FS_MountPartition (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType)
{
	return DI_FS_MountPartitionExt(eDevIdx, ulPartIdx, szDstMountDir, eFsType, 0);
}

DI_ERR_CODE DI_FS_MountPartitionExt(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType, DI_FS_MNTOPT_E eMntOpt)
{
	char			 szPartFileName[32];
	char			*szFsType = NULL, *szFilterOpts = NULL;
	unsigned long	 ulMountFlags = 0;
	int				 nRet;

	PrintEnter();
	if (szDstMountDir == NULL || szDstMountDir[0] == '\0') {
		return DI_ERR_ERROR;
	}

	// Make the source partition device file name
	DI_FS_GetPartitionFullFileName (eDevIdx, ulPartIdx, (HUINT8 *)szPartFileName);

	switch (eFsType) {
	case DI_FS_EXT2_FILESYSTEM:
		szFsType = "ext2";
		break;
	case DI_FS_EXT3_FILESYSTEM:
		szFsType = "ext3";
		break;
	case DI_FS_EXT4_FILESYSTEM:
		szFsType = "ext4";
		break;
	case DI_FS_XFS_FILESYSTEM:
		szFsType = "xfs";
		break;
	case DI_FS_FAT16_FILESYSTEM:
	case DI_FS_FAT32_FILESYSTEM:
		szFsType = "vfat";
		szFilterOpts = "iocharset=utf8";		// UTF-8 support
		break;
	case DI_FS_NTFS_FILESYSTEM:
		szFsType = "tntfs";
		szFilterOpts = "nls=utf8";				// UTF-8 support
		break;
	case DI_FS_MSDOS_FILESYSTEM:
#if defined(CONFIG_OP_NTVPLUS)
		// 러시안 OS로 포멧된 USB로 테스트해보니 아래와 같이 해야 러시아어가 정상적으로 표시됨
		szFsType = "vfat";
		szFilterOpts = "iocharset=utf8";		// UTF-8 support
		DI_FS_GetDeviceFileFullName (eDevIdx, (HUINT8 *)szPartFileName);
#else
		// MS-DOS File System일 경우에는 Partition이 아니라 그 Device명으로 해야 함.
		// 기존 코드에서 있던 내용. 아직 reference는 찾지 못했습니다.
		szFsType = "msdos";
		DI_FS_GetDeviceFileFullName (eDevIdx, (HUINT8 *)szPartFileName);
#endif
		break;
	case DI_FS_EXFAT_FILESYSTEM:
		PrintError("[%s] exFAT is not supported\n", __FUNCTION__);
		return DI_ERR_NOT_SUPPORTED;

	default:
		PrintError("[DI_FS_MountPartition] wrong FS Type (%d)\n", eFsType);
		return DI_ERR_ERROR;
	}

	/* set mount option */
	if (eMntOpt & DI_FS_MNTOPT_MANDLOCK)	ulMountFlags |= MS_MANDLOCK;
	if (eMntOpt & DI_FS_MNTOPT_NOATIME)		ulMountFlags |= MS_NOATIME;
	if (eMntOpt & DI_FS_MNTOPT_NODEV)		ulMountFlags |= MS_NODEV;
	if (eMntOpt & DI_FS_MNTOPT_NODIRATIME) 	ulMountFlags |= MS_NODIRATIME;
	if (eMntOpt & DI_FS_MNTOPT_NOEXEC)		ulMountFlags |= MS_NOEXEC;
	if (eMntOpt & DI_FS_MNTOPT_NOSUID)		ulMountFlags |= MS_NOSUID;
	if (eMntOpt & DI_FS_MNTOPT_RDONLY)		ulMountFlags |= MS_RDONLY;
	if (eMntOpt & DI_FS_MNTOPT_REMOUNT)		ulMountFlags |= MS_REMOUNT;
	if (eMntOpt & DI_FS_MNTOPT_SYNCHRONOUS)	ulMountFlags |= MS_SYNCHRONOUS;

	switch(eMntOpt) {
	case DI_FS_MNTOPT_BIND:
	case DI_FS_MNTOPT_MOVE:
		PrintError("[%s] Ignored! option(0x%x) is not supported in this function\n", __FUNCTION__, eMntOpt);
		break;
	default:
		break;
	}

	/* create directory if not exist */
	if (FALSE == DI_FS_IsDirExist ((char *)szDstMountDir)) {
		DI_FS_Mkdir ((char *)szDstMountDir);
	}

	PrintDebug("[%s] mount %s to %s, with FsType(%d)\n", __FUNCTION__, szPartFileName, szDstMountDir, eFsType);

	if (VK_strncmp(szFsType, "xfs", 3)==0) {
		nRet = local_difs_Hdd_XFS_Mount(szPartFileName, (char *)szDstMountDir, szFsType, NULL, XFS_REPAIR_CNT);
	} else {
		nRet = mount (szPartFileName, (char *)szDstMountDir, szFsType, ulMountFlags, szFilterOpts);

		/* Run check-disk first, if needed */
		if (nRet < 0) {
			switch(eFsType) {
			case DI_FS_FAT16_FILESYSTEM:
			case DI_FS_FAT32_FILESYSTEM:
			case DI_FS_MSDOS_FILESYSTEM:
#if defined(CONFIG_DOSFSTOOLS)
				if (errno == EROFS) {
					//excute dosfsck first.
					char szCmdLine[64] = {0,};

					PrintError("[%s] Excute dosfsck\n", __FUNCTION__);
					VK_snprintf(szCmdLine, sizeof(szCmdLine)-1, "dosfsck -a %s", szPartFileName);
					VK_SYSTEM_Command(szCmdLine);

					/* Retry */
					nRet = mount (szPartFileName, (char *)szDstMountDir, szFsType, ulMountFlags, szFilterOpts);
					PrintError("[%s] Try to mount again (ret=%d)\n", __FUNCTION__, nRet);
				}
#endif
				break;
			case DI_FS_NTFS_FILESYSTEM:
#if defined(CONFIG_NTFS_RW_LIB)
				if (errno == EROFS || errno == EINVAL) {
					//excute ntfsck first.
					char szCmdLine[64] = {0,};

					PrintError("[%s] Excute ntfsck\n", __FUNCTION__);
					VK_snprintf(szCmdLine, sizeof(szCmdLine)-1, "ntfsck -a %s", szPartFileName);
					VK_SYSTEM_Command(szCmdLine);

					/* Retry */
					nRet = mount (szPartFileName, (char *)szDstMountDir, szFsType, ulMountFlags, szFilterOpts);
					PrintError("[%s] Try to mount again (ret=%d)\n", __FUNCTION__, nRet);
				}
#endif
				break;
			default:
				break;
			}
		}

		/* Retry to mount with other options */
		if (-1 == nRet && EROFS == errno)
		{
			/* if the 1st mount try fails, try to mount as a read-only again */
			ulMountFlags |= MS_RDONLY;
			nRet = mount (szPartFileName, (char *)szDstMountDir, szFsType, ulMountFlags, szFilterOpts);
			PrintError("[%s] Try to mount as read-only (ret=%d)\n", __FUNCTION__, nRet);
		}
		else if (-1 == nRet && EINVAL == errno && DI_FS_EXT3_FILESYSTEM == eFsType)
		{
			PrintDebug("[DI_FS_MountPartition] re-mount by changing FS_Type EXT3->EXT2 %d\n", 0);
			szFsType = "ext2";
			nRet = mount (szPartFileName, (char *)szDstMountDir, szFsType, ulMountFlags, szFilterOpts);
		}
		else if (-1 == nRet)
		{
			PrintError("[DI_FS_MountPartition] mount failed. errno(0x%08x, %s)\n", errno, strerror(errno));
		}
#if !defined(CONFIG_TEMP_HMX4KJP)
		if(-1 != nRet)
		{
			/* workaround : Ext4: Slow performance on first write after mount */
			if(eFsType == DI_FS_EXT4_FILESYSTEM)
			{
				char workaroundCmd[128];

				sync();	/* warning : if remove, small index file become zero length */
				VK_snprintf(workaroundCmd, sizeof(workaroundCmd)-1, "cat /proc/fs/ext4/%s/mb_groups > /dev/null", (char *)&szPartFileName[5]);
				VK_SYSTEM_Command(workaroundCmd);
			}
		}
#endif
	}

	PrintError("[%s] Mount Done!! ret(%d) option(0x%x)\n", __FUNCTION__, nRet, eMntOpt);
	PrintExit();
	return (-1 == nRet) ? DI_ERR_ERROR : DI_ERR_OK;
}

DI_ERR_CODE DI_FS_MountPartitionOpt(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType, DI_FS_MNTOPT_E eMntFlags, const void *pMntOpts)
{
	char			 szPartFileName[32];
	char			*szFsType = NULL;
	unsigned long	 ulMountFlags = 0;
	int				 nRet;

	PrintEnter();
	if (szDstMountDir == NULL || szDstMountDir[0] == '\0') {
		return DI_ERR_ERROR;
	}

	// Make the source partition device file name
	DI_FS_GetPartitionFullFileName (eDevIdx, ulPartIdx, (HUINT8 *)szPartFileName);

	switch (eFsType) {
	case DI_FS_EXT2_FILESYSTEM:
		szFsType = "ext2";
		break;
	case DI_FS_EXT3_FILESYSTEM:
		szFsType = "ext3";
		break;
	case DI_FS_EXT4_FILESYSTEM:
		szFsType = "ext4";
		break;
	case DI_FS_XFS_FILESYSTEM:
		szFsType = "xfs";
		break;
	case DI_FS_FAT16_FILESYSTEM:
	case DI_FS_FAT32_FILESYSTEM:
		szFsType = "vfat";
		break;
	case DI_FS_NTFS_FILESYSTEM:
		szFsType = "tntfs";
		break;
	case DI_FS_MSDOS_FILESYSTEM:
#if defined(CONFIG_OP_NTVPLUS)
		szFsType = "vfat";
		DI_FS_GetDeviceFileFullName (eDevIdx, (HUINT8 *)szPartFileName);
#else
		// MS-DOS File System일 경우에는 Partition이 아니라 그 Device명으로 해야 함.
		// 기존 코드에서 있던 내용. 아직 reference는 찾지 못했습니다.
		szFsType = "msdos";
		DI_FS_GetDeviceFileFullName (eDevIdx, (HUINT8 *)szPartFileName);
#endif
		break;

	case DI_FS_EXFAT_FILESYSTEM:
		PrintError("[%s] exFAT is not supported\n", __FUNCTION__);
		return DI_ERR_NOT_SUPPORTED;

	default:
		PrintError("[DI_FS_MountPartition] wrong FS Type (%d)\n", eFsType);
		return DI_ERR_ERROR;
	}

	/* set mount option */
	if (eMntFlags & DI_FS_MNTOPT_MANDLOCK)	ulMountFlags |= MS_MANDLOCK;
	if (eMntFlags & DI_FS_MNTOPT_NOATIME)		ulMountFlags |= MS_NOATIME;
	if (eMntFlags & DI_FS_MNTOPT_NODEV)		ulMountFlags |= MS_NODEV;
	if (eMntFlags & DI_FS_MNTOPT_NODIRATIME) 	ulMountFlags |= MS_NODIRATIME;
	if (eMntFlags & DI_FS_MNTOPT_NOEXEC)		ulMountFlags |= MS_NOEXEC;
	if (eMntFlags & DI_FS_MNTOPT_NOSUID)		ulMountFlags |= MS_NOSUID;
	if (eMntFlags & DI_FS_MNTOPT_RDONLY)		ulMountFlags |= MS_RDONLY;
	if (eMntFlags & DI_FS_MNTOPT_REMOUNT)		ulMountFlags |= MS_REMOUNT;
	if (eMntFlags & DI_FS_MNTOPT_SYNCHRONOUS)	ulMountFlags |= MS_SYNCHRONOUS;

	switch(eMntFlags) {
	case DI_FS_MNTOPT_BIND:
	case DI_FS_MNTOPT_MOVE:
		PrintError("[%s] Ignored! option(0x%x) is not supported in this function\n", __FUNCTION__, eMntFlags);
		break;
	default:
		break;
	}

	/* create directory if not exist */
	if (FALSE == DI_FS_IsDirExist ((char *)szDstMountDir)) {
		DI_FS_Mkdir ((char *)szDstMountDir);
	}

	PrintDebug("[%s] mount %s to %s, with FsType(%d)\n", __FUNCTION__, szPartFileName, szDstMountDir, eFsType);

	if (VK_strncmp(szFsType, "xfs", 3)==0) {
		nRet = local_difs_Hdd_XFS_Mount(szPartFileName, (char *)szDstMountDir, szFsType, NULL, XFS_REPAIR_CNT);
	} else {
		nRet = mount (szPartFileName, (char *)szDstMountDir, szFsType, ulMountFlags, pMntOpts);

		/* Run check-disk first, if needed */
		if (nRet < 0) {
			switch(eFsType) {
			case DI_FS_FAT16_FILESYSTEM:
			case DI_FS_FAT32_FILESYSTEM:
			case DI_FS_MSDOS_FILESYSTEM:
#if defined(CONFIG_DOSFSTOOLS)
				if (errno == EROFS) {
					//excute dosfsck first.
					char szCmdLine[64] = {0,};

					PrintError("[%s] Excute dosfsck\n", __FUNCTION__);
					VK_snprintf(szCmdLine, sizeof(szCmdLine)-1, "dosfsck -a %s", szPartFileName);
					VK_SYSTEM_Command(szCmdLine);

					/* Retry */
					nRet = mount (szPartFileName, (char *)szDstMountDir, szFsType, ulMountFlags, pMntOpts);
					PrintError("[%s] Try to mount again (ret=%d)\n", __FUNCTION__, nRet);
				}
#endif
				break;
			case DI_FS_NTFS_FILESYSTEM:
#if defined(CONFIG_NTFS_RW_LIB)
				if (errno == EROFS || errno == EINVAL) {
					//excute ntfsck first.
					char szCmdLine[64] = {0,};

					PrintError("[%s] Excute ntfsck\n", __FUNCTION__);
					VK_snprintf(szCmdLine, sizeof(szCmdLine)-1, "ntfsck -a %s", szPartFileName);
					VK_SYSTEM_Command(szCmdLine);

					/* Retry */
					nRet = mount (szPartFileName, (char *)szDstMountDir, szFsType, ulMountFlags, pMntOpts);
					PrintError("[%s] Try to mount again (ret=%d)\n", __FUNCTION__, nRet);
				}
#endif
				break;
			default:
				break;
			}
		}

		/* Retry to mount with other options */
		if (-1 == nRet && EROFS == errno)
		{
			/* if the 1st mount try fails, try to mount as a read-only again */
			ulMountFlags |= MS_RDONLY;
			nRet = mount (szPartFileName, (char *)szDstMountDir, szFsType, ulMountFlags, pMntOpts);
			PrintError("[%s] Try to mount as read-only (ret=%d)\n", __FUNCTION__, nRet);
		}
		else if (-1 == nRet && EINVAL == errno && DI_FS_EXT3_FILESYSTEM == eFsType)
		{
			PrintDebug("[DI_FS_MountPartition] re-mount by changing FS_Type EXT3->EXT2 %d\n", 0);
			szFsType = "ext2";
			nRet = mount (szPartFileName, (char *)szDstMountDir, szFsType, ulMountFlags, pMntOpts);
		}
		else if (-1 == nRet)
		{
			PrintError("[DI_FS_MountPartition] mount failed. errno(0x%08x, %s)\n", errno, strerror(errno));
		}
#if !defined(CONFIG_TEMP_HMX4KJP)
		if(-1 != nRet)
		{
			/* workaround : Ext4: Slow performance on first write after mount */
			if(eFsType == DI_FS_EXT4_FILESYSTEM)
			{
				char workaroundCmd[128];

				sync();	/* warning : if remove, small index file become zero length */
				VK_snprintf(workaroundCmd, sizeof(workaroundCmd)-1, "cat /proc/fs/ext4/%s/mb_groups > /dev/null", (char *)&szPartFileName[5]);
				VK_SYSTEM_Command(workaroundCmd);
			}
		}
#endif
	}

	PrintError("[%s] Mount Done!! ret(%d) option(0x%x)\n", __FUNCTION__, nRet, eMntFlags);
	PrintExit();
	return (-1 == nRet) ? DI_ERR_ERROR : DI_ERR_OK;
}

DI_ERR_CODE DI_FS_UnmountPartition (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType)
{
	int nStdErr;
	char			*szMountedDir = NULL;
	DI_ERR_CODE		hErr, hResult = DI_ERR_ERROR;
#if defined(CONFIG_PRODUCT_IMAGE_DEBUG)
	char			cmd[256] = {0,};
#endif

	PrintEnter();
	szMountedDir = (char *)FS_MEM_Alloc (80);
	if (szMountedDir == NULL)
	{
		PrintError("[DI_FS_UmountPartition] eDevIdx(%d) ulPartIdx(%d) eFsType(%d) error : lack of memory!!\n", eDevIdx, ulPartIdx, eFsType);

		return DI_ERR_ERROR;
	}

	// Mount 여부 확인 시, FS Type에 따라 방법이 다를 수 있으므로 File System Type을 내려주어야 한다.
	hErr = local_difs_FindMountedPathOfPartition (eDevIdx, ulPartIdx, eFsType, szMountedDir, 80);		// prevent fix
	if (hErr == DI_ERR_OK)
	{
		if (umount(szMountedDir) == -1)
		{
			PrintError("[DI_FS_UnmountPartition] umount(%s) failed(0x%x)\n", szMountedDir, errno);
			nStdErr = errno;

#if defined(CONFIG_PRODUCT_IMAGE_DEBUG)
			/* print messages of file lists */
			PrintError("[DI_FS_UnmountPartition] some files are open in %s directory \n", szMountedDir);
			VK_snprintf(cmd, sizeof(cmd)-1, "lsof | grep %s", szMountedDir);
			VK_SYSTEM_Command(cmd);
#endif

			/* store messages of "lsof | grep" in target folder for debugging */
			P_FS_StoreUmountInfo(szMountedDir, VK_strlen(szMountedDir));

			/* When unmount return errors due to Bad File Decriptor EBADF
			* look up a path to be umounted fail
			* (it happens when unplug USB while playing video),
			* fource umount with MNT_DETACH
			*/
			if (nStdErr == EBADF)
			{
				PrintError("[%s,%d] Do lazy umount forcely!!\n", __FUNCTION__, __LINE__);
				if (umount2(szMountedDir, MNT_DETACH) == -1)
				{
					PrintError("[DI_FS_UmountPartition] lazy umount(%s) failed(0x%x)\n", szMountedDir, errno);
				}
				else
				{
					hResult = DI_ERR_OK;
				}
			}
#if 0
			/* Disable lazy unmount */
			if (errno == EBUSY)
			{
				/* Device is busy, do lazy umount */
				if (umount2(szMountedDir, MNT_DETACH) == -1)
				{
					PrintError("[DI_FS_UmountPartition] lazy umount(%s) failed(0x%x)\n", szMountedDir, errno);
				}
				else
				{
					hResult = DI_ERR_OK;
				}
			}
#endif
		}
		else
		{
			hResult = DI_ERR_OK;
		}
	}

	FS_MEM_Free (szMountedDir);
	PrintExit();
	return hResult;
}

// File System에 따라 Access 하는 방식이 다르므로 일단 이렇게 argument를 받아서 구분시킨다.
// 차후에 어떻게 분리할 것인가에 대해 다른 방안을 모색해야 한다.
DI_ERR_CODE DI_FS_GetMountedPartitionStatus (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType, HUINT64 *pullTotalKbSize, HUINT64 *pullUsedKbSize, HBOOL *pbWritable)
{
	HBOOL		 bWritable;
	HUINT64		 ullTotalSize, ullUsableSize;
	HUINT8		 szPartDevName[32];
	DI_ERR_CODE		 hErr = DI_ERR_ERROR;

	PrintEnter();
	if (pullTotalKbSize != NULL)			{ *pullTotalKbSize	= 0; }
	if (pullUsedKbSize != NULL) 			{ *pullUsedKbSize	= 0; }
	if (pbWritable != NULL) 				{ *pbWritable		= FALSE; }

	// Make the source partition device file name

	VK_MEM_Memset(szPartDevName,0,sizeof(szPartDevName));

	switch (eFsType)
	{
	case DI_FS_MSDOS_FILESYSTEM:
	case DI_FS_OTHER_FILESYSTEM:
		DI_FS_GetDeviceFileFullName (eDevIdx, szPartDevName);
		break;

	default:
		DI_FS_GetPartitionFullFileName (eDevIdx, ulPartIdx, szPartDevName);
		break;
	}

	hErr = local_difs_GetMountedInfoFromMtab (szPartDevName, &ullTotalSize, &ullUsableSize, &bWritable);
	if (hErr == DI_ERR_OK)
	{
		if (pullTotalKbSize != NULL)			{ *pullTotalKbSize	= ullTotalSize; }
		if (pullUsedKbSize != NULL)				{ *pullUsedKbSize	= ullUsableSize; }
		if (pbWritable != NULL)					{ *pbWritable		= bWritable; }

		return DI_ERR_OK;
	}

	PrintError("[DI_FS_GetMountedPartitionStatus] eDevIdx %d partition (index : %d) szPartDevName %s failed.", eDevIdx,ulPartIdx,szPartDevName);
	PrintExit();
	return DI_ERR_ERROR;
}

//
// BD Board에서 Internal HDD를 USB HDD로 인식시키기 위해 필요한 기능
//
DI_ERR_CODE DI_FS_EnableHddToBdModule (DI_FS_StorDevIdx_t eDevIdx)
{
#if !defined(WIN32)
	char	 szDevName[32];
	char	 szCmdStr[80];
	DI_ERR_CODE	 hErr = DI_ERR_ERROR;

	PrintEnter();
	hErr = DI_FS_GetDeviceFileFullName (eDevIdx, (HUINT8 *) szDevName);
	if (hErr != DI_ERR_OK)
	{
		PrintError("error : lack of resource!! %d\n", 0);
		return DI_ERR_ERROR;
	}

#if 0	// USB HDD 처리를 일부 DI에서 실행 하도록 변경
	hErr = VK_SYSTEM_Command ("insmod /lib/modules/brcm_dev.ko");
	if (hErr)
	{
		PrintError("[DI_FS_EnableHddToBdModule] [insmod /lib/modules/brcm_dev.ko] err(0x%08x)\n", hErr);
	}
#endif

	//prevent : defect Fix / 2011.03.27 mhpark3
	//VK_sprintf (szCmdStr, "insmod /lib/modules/g_file_storage.ko file=%s", szDevName);
	VK_snprintf (szCmdStr, sizeof(szCmdStr), "insmod /lib/modules/g_file_storage.ko file=%s", szDevName);
	hErr = VK_SYSTEM_Command ((char *)szCmdStr);
	if (hErr)
	{
		PrintError("[DI_FS_EnableHddToBdModule] [%s] err(0x%08x)\n", szCmdStr, hErr);
	}
#endif
	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE DI_FS_DisableHddToBdModule (DI_FS_StorDevIdx_t eDevIdx)
{

	UNUSED(eDevIdx);
	PrintEnter();
#if !defined(WIN32)
	DI_ERR_CODE	 hErr = DI_ERR_ERROR;

	hErr = VK_SYSTEM_Command ("rmmod g_file_storage.ko");
	if (hErr)
	{
		PrintError("[DI_FS_DisableHddToBdModule] [rmmod g_file_storage.ko] err(0x%08x)\n", hErr);
	}

#if 0	// USB HDD 처리를 일부 DI에서 실행 하도록 변경
	hErr = VK_SYSTEM_Command ("rmmod brcm_dev.ko");
	if (hErr)
	{
		PrintError("[DI_FS_DisableHddToBdModule] [rmmod brcm_dev.ko] err(0x%08x)\n", hErr);
	}
#endif
#endif
	PrintExit();
	return DI_ERR_OK;
}

/*  HDD의 Volume name을 읽어 오는 함수.   */
DI_ERR_CODE DI_FS_GetVolumeName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 bufsize)
{
	FILE *fp;
	char *p;
	char szBuf[128];
	HCHAR szDevName[32] = {0,};
	DI_ERR_CODE hErr = DI_ERR_ERROR;

	hErr = DI_FS_GetPartitionFullFileName (eDevIdx, ulPartIdx, (HUINT8 *)szDevName);
	if (hErr!=DI_ERR_OK)
	{
		PrintError("DI_FS_GetPartitionFullFileName fail. (%s)\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	VK_snprintf(szBuf, sizeof(szBuf), "/sbin/probefs -l %s", (char*)szDevName);
	fp = popen(szBuf, "r");
	if (!fp)
		return DI_ERR_ERROR;

	VK_MEM_Memset(szBuf, 0, sizeof(szBuf));
	fgets(szBuf, sizeof(szBuf)-1, fp);
	pclose(fp);

	/* remove char '\n' and add null byte */
	p = strrchr(szBuf, '\n');
	if (p) *p = 0;

	VK_snprintf(buf, bufsize, "%s", szBuf);

	return DI_ERR_OK;
}

/*  HDD의 Volume name을 write 하는 함수.   */
DI_ERR_CODE DI_FS_SetVolumeName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf)
{
	HCHAR szDevName[32] = {0,};
	char szBuf[128];
	DI_ERR_CODE hErr = DI_ERR_ERROR;

	if (buf == NULL || buf[0] =='\0')
	{
		PrintError("invalid function param. (%s)\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	hErr = DI_FS_GetPartitionFullFileName (eDevIdx, ulPartIdx, (HUINT8*)szDevName);
	if (hErr!=DI_ERR_OK)
	{
		PrintError("DI_FS_GetPartitionFullFileName fail. (%s)\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	VK_snprintf(szBuf, sizeof(szBuf), "/sbin/probefs -L \"%s\" %s", buf, (char*)szDevName);
	if (VK_SYSTEM_Command(szBuf) != VK_OK)
		return DI_ERR_ERROR;

	return DI_ERR_OK;
}

/*  HDD의 Volume uuid를 가져오는 함수.
di_fs.c 에 DI_FS_GetVolumeNtfsUuid, DI_FS_GetVolumeXfsUuid로 구분되어 있어서 하나로 통합했습니다.
다만, NTFS 파일 시스템의 경우 로컬 PC에서 UUID를 확인하면 '6A5C78955C785DAF'로 되지만,
이 함수를 이용하면 'AF5D785C95785C6A'로 출력되어 확인이 필요합니다. */
DI_ERR_CODE DI_FS_GetVolumeUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 bufsize, HUINT32 *uuidsize)
{
	FILE *fp;
	HCHAR szDevName[32] = {0,};
	char szBuf[128];
	char u8buf[3] = {0,};
	char *p;
	unsigned int i;
	unsigned int len;
	DI_ERR_CODE hErr = DI_ERR_ERROR;

	hErr = DI_FS_GetPartitionFullFileName (eDevIdx, ulPartIdx, (HUINT8 *)szDevName);
	if (hErr!=DI_ERR_OK)
	{
		PrintError("DI_FS_GetPartitionFullFileName fail. (%s)\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	VK_snprintf(szBuf, sizeof(szBuf), "/sbin/probefs -u %s", (char*)szDevName);
	fp = popen(szBuf, "r");
	if (!fp)
		return DI_ERR_ERROR;

	VK_MEM_Memset(szBuf, 0, sizeof(szBuf));
	fgets(szBuf, sizeof(szBuf)-1, fp);
	pclose(fp);

	/* remove char '\n' and add null byte */
	p = strrchr(szBuf, '\n');
	if (p) *p = 0;

	*uuidsize = 0;
	/* convert string to hex */
	for (i=0, len=VK_strlen(szBuf); i<len && (*uuidsize)<bufsize; i+=2)
	{
		u8buf[0] = szBuf[i];
		u8buf[1] = szBuf[i+1];
		buf[(*uuidsize)++] = strtol(u8buf, NULL, 16);
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_FS_GetFsType (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t *eFsType )
{
	DI_ERR_CODE 			hErr = DI_ERR_ERROR;
	HUINT8			 		szPartFileName[32];

	PrintEnter();

	*eFsType = DI_FS_OTHER_FILESYSTEM;

	VK_MEM_Memset(szPartFileName, 0, 32);
	hErr = DI_FS_GetPartitionFullFileName (eDevIdx, ulPartIdx, (HUINT8 *)szPartFileName);
	if (hErr != DI_ERR_OK)
	{
		PrintExit();
		return hErr;
	}

	*eFsType = local_difs_GetFSInfo(szPartFileName);

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_FS_ExecuteCheckDisk(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType)
{
	char			szPartFileName[32];
#if defined(CONFIG_DOSFSTOOLS) || defined(CONFIG_NTFS_RW_LIB)
	char 			szCmdLine[64] = {0,};
#endif
	char			szLineBuf[256] = {0,};
	DI_ERR_CODE		err;
	FILE			*fp;
	char			*p = NULL;

	err = DI_FS_GetPartitionFullFileName(eDevIdx, ulPartIdx, (HUINT8*)szPartFileName);
	if (err != DI_ERR_OK) {
		return err;
	}

	fp = fopen("/proc/mounts", "r");
	if (!fp) {
		PrintError("[%s] Error! can't open file\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
	while(fgets(szLineBuf, sizeof(szLineBuf)-1, fp)) {
		/* find target partition device */
		p = VK_strstr((const char*)szLineBuf, (const char*)szPartFileName);
		if (p) break;
	}
	fclose(fp);

	if (p) {
		/* this device is mounted. return error */
		PrintError("[%s] Error! Device %s is mounted. Do umount first for checking disk\n", __FUNCTION__, szPartFileName);
		return DI_ERR_ERROR;
	}

	err = DI_ERR_OK;

	switch(eFsType) {
	case DI_FS_FAT16_FILESYSTEM:
	case DI_FS_FAT32_FILESYSTEM:
	case DI_FS_MSDOS_FILESYSTEM:
#if defined(CONFIG_DOSFSTOOLS)
		//excute dosfsck first.
		PrintDebug("[%s] Excute dosfsck\n", __FUNCTION__);
		VK_snprintf(szCmdLine, sizeof(szCmdLine)-1, "dosfsck -a %s", szPartFileName);
		VK_SYSTEM_Command(szCmdLine);
#endif
		break;
	case DI_FS_NTFS_FILESYSTEM:
#if defined(CONFIG_NTFS_RW_LIB)
		PrintDebug("[%s] Excute ntfsck\n", __FUNCTION__);
		VK_snprintf(szCmdLine, sizeof(szCmdLine)-1, "ntfsck -a %s", szPartFileName);
		VK_SYSTEM_Command(szCmdLine);
#endif
		break;
	case DI_FS_EXFAT_FILESYSTEM:
		PrintError("[%s] exFAT is not supported\n", __FUNCTION__);
		return DI_ERR_NOT_SUPPORTED;
	default:
		PrintError("[%s] ERROR! Unknown fs type (%d)\n", __FUNCTION__, eFsType);
		err = DI_ERR_ERROR;
		break;
	}

	PrintError("[%s] check-disk completed! fstype(%d) ret(%d)\n", __FUNCTION__, eFsType, err);
	return err;
}

DI_ERR_CODE DI_FS_FLASH_MountPartitions(void)
{
#if defined(CONFIG_EMMC_FLASH)
	int rc;

	rc = local_difs_EmmcMountAllPartitions();
	if (rc < 0)
	{
		PrintError("[%s] local_difs_EmmcMountAllPartitions returned error !!!!!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
#elif defined(CONFIG_NAND_FLASH)
	int rc;

	rc = local_difs_NandflashMountPartitions();
	if (rc < 0)
	{
		PrintError("[%s] local_difs_NandflashMountPartitions returned error !!!!!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
#else
	PrintError("[%s] is called!!!!!\n", __FUNCTION__);
#endif
	return DI_ERR_OK;

}

DI_ERR_CODE DI_FS_FLASH_UmountPartitions(void)
{
#if defined(CONFIG_EMMC_FLASH)
	int rc;

	rc = local_difs_EmmcUmountAllPartitions();
	if (rc < 0)
	{
		PrintError("[%s] local_difs_EmmcUmountAllPartitions returned error !!!!!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
#elif defined(CONFIG_NAND_FLASH)
	int rc;

	rc = local_difs_NandflashUmountPartitions();
	if (rc < 0)
	{
		PrintError("[%s] local_difs_NandflashUmountPartitions returned error !!!!!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
#else
	PrintError("[%s] is called!!!!!\n", __FUNCTION__);
#endif

	return DI_ERR_OK;
}

#if defined(CONFIG_CRYPTSETUP)
DI_ERR_CODE DI_FS_ENCR_Format(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szPassword)
{
	char 		szCmdLine[256] = {0,};
	DI_ERR_CODE	Ret = DI_ERR_OK;

	if (!szPassword || eDevIdx >= eDI_FS_DEV_MAX) {
		return DI_ERR_INVALID_PARAM;
	}

	/* Cryptsetup Format*/
	VK_snprintf(szCmdLine, sizeof(szCmdLine) - 1, "cryptsetup --verbose --batch-mode --password=\"%s\" --password-size=%d luksFormat /dev/sd%c%d",
												szPassword,
												VK_strlen(szPassword),
												eDevIdx + 'a',
												ulPartIdx + 1);
	if (VK_SYSTEM_Command(szCmdLine) != 0) {
		PrintError("%s: Errors in luksFormat!\n", __FUNCTION__);
		Ret = DI_ERR_ERROR;
	}

	return Ret;
}

DI_ERR_CODE DI_FS_ENCR_Open(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMapperDeviceName, char *szPassword)
{
	char		szCmdLine[256] = {0,};
	DI_ERR_CODE	Ret = DI_ERR_OK;

	if (eDevIdx >= eDI_FS_DEV_MAX || !szMapperDeviceName || !szPassword) {
		return DI_ERR_INVALID_PARAM;
	}

	/* Cryptsetup Open*/
	VK_snprintf(szCmdLine, sizeof(szCmdLine) - 1, "cryptsetup --password=\"%s\" --password-size=%d luksOpen /dev/sd%c%d %s",
												szPassword,
												VK_strlen(szPassword),
												eDevIdx + 'a',
												ulPartIdx + 1,
												szMapperDeviceName);
	if (VK_SYSTEM_Command(szCmdLine) != 0) {
		PrintError("%s: Errors in luksOpen!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	/* Save mapping information */
	VK_snprintf(szCmdLine, sizeof(szCmdLine)-1, "fs.encr.sd%c%d", eDevIdx+'a',  ulPartIdx+1);
	if (setenv(szCmdLine, szMapperDeviceName, 1) < 0) {
		Ret = DI_ERR_ERROR;
	}

	return Ret;
}

DI_ERR_CODE DI_FS_ENCR_Close(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	char 		szCmdLine[256] = {0,};
	char		szDev[32] = {0,};
	char		*pMapperDev;

	if (eDevIdx >= eDI_FS_DEV_MAX) {
		return DI_ERR_INVALID_PARAM;
	}

	VK_snprintf(szDev, sizeof(szDev)-1, "fs.encr.sd%c%d", eDevIdx+'a', ulPartIdx+1);
	pMapperDev = getenv(szDev);
	if (!pMapperDev) {
		return DI_ERR_ERROR;
	}

	/* Cryptsetup Close*/
	VK_snprintf(szCmdLine, sizeof(szCmdLine) - 1, "cryptsetup luksClose %s", pMapperDev);
	if (VK_SYSTEM_Command(szCmdLine) != 0) {
		PrintError("%s: Errors in luksClose!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	if (unsetenv(szDev) < 0) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_FS_ENCR_MakeFS(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szVolumeLable, DI_FileSystemType_t eFsType)
{
	char FullPathMapperDeviceName[256] = {0,};
	char szDev[32] = {0,};
	char *pMapperDev;
	int rc;

	if (eDevIdx >= eDI_FS_DEV_MAX || !szVolumeLable)
	{
		return DI_ERR_INVALID_PARAM;
	}

	VK_snprintf(szDev, sizeof(szDev)-1, "fs.encr.sd%c%d", eDevIdx+'a', ulPartIdx+1);
	pMapperDev = getenv(szDev);
	if (!pMapperDev)
	{
		return DI_ERR_ERROR;
	}
	VK_snprintf(FullPathMapperDeviceName, sizeof(FullPathMapperDeviceName), "/dev/mapper/%s", pMapperDev);

	rc = P_FS_FormatDisk(eFsType, FullPathMapperDeviceName, NULL);
	if (rc < 0)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_FS_ENCR_Mount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMountDir, DI_FileSystemType_t eFsType)
{
	char			*szFsType = NULL;
	char			*szFilterOpts = NULL;
	unsigned long	ulMountFlags = 0;
	int				err = -1;
	char			FullPathMapperDeviceName[256] = {0,};
	char			szDev[32] = {0,};
	char			*pMapperDev;

	if (eDevIdx >= eDI_FS_DEV_MAX || !szMountDir) {
		return DI_ERR_INVALID_PARAM;
	}

	VK_snprintf(szDev, sizeof(szDev)-1, "fs.encr.sd%c%d", eDevIdx+'a', ulPartIdx+1);
	pMapperDev = getenv(szDev);
	if (!pMapperDev) {
		return DI_ERR_ERROR;
	}
	VK_snprintf(FullPathMapperDeviceName, sizeof(FullPathMapperDeviceName)-1, "/dev/mapper/%s", pMapperDev);

	switch (eFsType) {
	case DI_FS_EXT3_FILESYSTEM:
		szFsType = "ext3";
		break;
	case DI_FS_EXT4_FILESYSTEM:
		szFsType = "ext4";
		break;
	case DI_FS_XFS_FILESYSTEM:
		szFsType = "xfs";
		break;
	case DI_FS_NTFS_FILESYSTEM:
		szFsType = "tntfs";
		szFilterOpts = "nls=utf8";
		break;
	default:
		PrintError("%s: wrong FS Type (%d)\n", __FUNCTION__, eFsType);
		return DI_ERR_ERROR;
	}

	if (DI_FS_IsDirExist(szMountDir) == FALSE) {
		DI_FS_Mkdir(szMountDir);
	}

	if (VK_strncmp(szFsType, "xfs", 3) == 0) {
		err = local_difs_Hdd_XFS_Mount(FullPathMapperDeviceName, szMountDir, szFsType, szFilterOpts, XFS_REPAIR_CNT);
	}
	else {
		err = mount(FullPathMapperDeviceName, szMountDir, szFsType, ulMountFlags, szFilterOpts);
		if (err == -1 && EROFS == errno) {
			/* if the 1st mount try fails, try to mount as a read-only again */
			PrintError("%s: re-mount as read-only\n", __FUNCTION__);
			ulMountFlags |= MS_RDONLY;
			err = mount(FullPathMapperDeviceName, szMountDir, szFsType, ulMountFlags, szFilterOpts);
		}
		else if (err == -1 && EINVAL == errno && DI_FS_EXT3_FILESYSTEM == eFsType) {
			PrintError("%s: re-mount as ext2\n", __FUNCTION__);
			szFsType = "ext2";
			err = mount(FullPathMapperDeviceName, szMountDir, szFsType, ulMountFlags, szFilterOpts);
		}
		else if (err == -1) {
			PrintError("%s: mount fail\n", __FUNCTION__);
		}
	}

	return (err < 0) ? DI_ERR_ERROR : DI_ERR_OK;
}

DI_ERR_CODE DI_FS_ENCR_Umount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	int				err = -1;
	char			FullPathMapperDeviceName[256] = {0,};
	FILE			*fp;
	char			szDev[256] = {0,};
	char			szMapperDev[256] = {0,};
	char			szMountDir[256] = {0,};
	char			*pMapperDev;

	if (eDevIdx >= eDI_FS_DEV_MAX) {
		return DI_ERR_INVALID_PARAM;
	}

	VK_snprintf(szDev, sizeof(szDev)-1, "fs.encr.sd%c%d", eDevIdx+'a', ulPartIdx+1);
	pMapperDev = getenv(szDev);
	if (!pMapperDev) {
		return DI_ERR_ERROR;
	}
	VK_snprintf(FullPathMapperDeviceName, sizeof(FullPathMapperDeviceName) - 1, "/dev/mapper/%s", pMapperDev);

	fp = fopen("/proc/mounts", "r");
	if (!fp) {
		PrintError("%s: Can't open mounts file!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
	while(fgets(szDev, sizeof(szDev), fp)) {
		VK_memset(szMapperDev, 0, 256);
		VK_memset(szMountDir, 0, 256);
		VK_sscanf(szDev, "%254s %254s", szMapperDev, szMountDir);

		/* Find mount dir */
		if (VK_strncmp(szMapperDev, FullPathMapperDeviceName, VK_strlen(FullPathMapperDeviceName)) == 0) {
			err = umount(szMountDir);
			if (err < 0) {
				PrintError("%s: umount error(%d)! (%s)\n", __FUNCTION__, errno, szMountDir);
			}
			break;
		}
	}
	fclose(fp);

	return (err < 0) ? DI_ERR_ERROR : DI_ERR_OK;
}
#endif /*defined(CONFIG_CRYPTSETUP)*/
