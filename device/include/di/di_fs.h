/**
 * @file	di_fs.h
 * @brief	File System module DI layer header file
 * @version $Revision: 0.9.0 $
 * @date 	$Date: 2010/01/12 12:00:00 $
 * @author
 */

/** @defgroup	DI_FS	FileSystem COMMON IF OF DI LAYER
 *  This is FileSystem module of DI layer.
 *  @{
 */

#ifndef __DI_FS_H__
#define __DI_FS_H__

/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"
#include "di_err.h"

/**
 * Define BOOL
 */
#ifndef DI_FS_BOOL
typedef int	DI_FS_BOOL;
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

/******************************************************************************
 * Start for bootloader
 ******************************************************************************/
/**
 * Define File Handle
 */
typedef void * DI_FS_HANDLE;
typedef void * DI_FS_DIRENT_HANDLE;

/**
 * Define File Size
 */
#if defined(CONFIG_NOT_SUPPORT_BIGINT)
typedef HUINT32 DI_FS_SIZE;
#else
typedef HUINT64 DI_FS_SIZE;
#endif
/******************************************************************************
 * End for bootloader
 ******************************************************************************/

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/

/* partition related */
#define MAX_SEC_SIZE							2048
#define MIN_SEC_SIZE							512

#define DI_FS_MAX_DEV_NAME_LEN					9
#define DI_FS_DEVFILE_LEN						16			/* /dev/sda1 /dev/sda2 등 */
#define DI_FS_MOUNT_PATH_LEN					32			/* /mnt/hd1, /media/drive1 등 */
#define DI_FS_DIRENT_MAX_NAME_LEN				256			/* ref dirent.h, readdir name */

#define DI_MAX_PARTITION						10			/* USB partitions support */
#define VOLNAMSZ 								16

#define DI_DEV_NODE_LEN							8

#define NTFS_UUID_IN_SUPERBLOCK_OFFSET			(0x48)		/* the offset of UUID in the each partition's superblock */
#define NTFS_UUID_IN_SUPERBLOCK_SIZE			8			/* the size of ntfs uuid */ 

#define XFS_UUID_IN_SUPERBLOCK_OFFSET			(0x20)		/* the offset of UUID in the each partition's superblock */
#define XFS_UUID_IN_SUPERBLOCK_SIZE				16			/* the size of xfs uuid */ 



typedef enum
{
	eDI_FS_DEV_NULL						= -1,
	eDI_FS_DEV_sda						= 0,
	eDI_FS_DEV_sdb,
	eDI_FS_DEV_sdc,
	eDI_FS_DEV_sdd,
	eDI_FS_DEV_sde,
	eDI_FS_DEV_sdf,
	eDI_FS_DEV_sdg,
	eDI_FS_DEV_sdh,
	eDI_FS_DEV_sdi,

	eDI_FS_DEV_MAX
} DI_FS_StorDevIdx_t;

/**
 * Define origin
 */
typedef enum
{
	DI_FS_SEEK_SET = 0,
	DI_FS_SEEK_CUR,
	DI_FS_SEEK_END
} DI_FS_ORIGIN_e;

/** @brief file system type */
typedef enum
{
	DI_FS_EXT2_FILESYSTEM				= 2,
	DI_FS_EXT3_FILESYSTEM,
	DI_FS_EXT4_FILESYSTEM,
	DI_FS_XFS_FILESYSTEM,
	DI_FS_FAT16_FILESYSTEM,
	DI_FS_FAT32_FILESYSTEM,
	DI_FS_NTFS_FILESYSTEM,
	DI_FS_MSDOS_FILESYSTEM,
	DI_FS_EXFAT_FILESYSTEM,
	DI_FS_OTHER_FILESYSTEM				= 0xFF
} DI_FileSystemType_t;

enum
{
	DI_FS_EVENT_BASE				    = 10,
	DI_FS_EVENT_DISKFRESSSPACEFLOW  	= DI_FS_EVENT_BASE,
	DI_FS_EVENT_INT_HDD_ON,
	DI_FS_EVENT_INT_HDD_OFF,
	DI_FS_EVENT_DISKINTITSTART,
	DI_FS_EVENT_DISKINITEND,
	DI_FS_EVENT_DISKERROR,
	DI_FS_EVENT_HDDLOCKED,
	DI_FS_EVENT_HDDUNLOCKED,
	DI_FS_EVENT_USB_CONNECTED,
	DI_FS_EVENT_USB_DISCONNECTED,
	DI_FS_EVENT_USB_SCSI_ADDED,
	DI_FS_EVENT_USB_SCSI_REMOVED,
	DI_FS_EVENT_USB_OVERCURRENT,
	DI_FS_EVENT_FORMAT_PROGRESS,
	DI_FS_EVENT_FORMAT_DONE,
	DI_FS_EVENT_FORMAT_FAIL,
	DI_FS_EVENT_FS_TASK_ADD_FILEOP,		 /* task add할 때 너무 오래 걸리면 위쪽으로 알려준다. */
	DI_FS_EVENT_COPY_PROGRESS,
	DI_FS_EVENT_COPY_DONE,
	DI_FS_EVENT_DELETE_PROGRESS,
	DI_FS_EVENT_MOVE_DONE,
	DI_FS_EVENT_MAX_NUMBER,
	
	DI_MP3_EVENT_BASE					= 100,
	DI_MP3_EVENT_MAX,
	
	DI_JPG_EVENT_BASE					= 200,
	DI_JPG_EVENT_DECODING_SUCCESS,		/* no problem on decoding */
	DI_JPG_EVENT_DECODING_PREVIEW,
	DI_JPG_EVENT_DECODING_FAIL,			/* decoding failure */
	DI_JPG_EVENT_ROTATED,
	DI_JPG_EVENT_MAX,
};

enum
{
	DI_FS_NO_PARTITION 					= -2,
	DI_FS_EXTEDED_PARTITION 			= -1,
	DI_FS_PRIMARY_PARTITION_1 			= 0,
	DI_FS_PRIMARY_PARTITION_2,
	DI_FS_PRIMARY_PARTITION_3,
	DI_FS_PRIMARY_PARTITION_4,
	DI_FS_LOGICAL_PARTITION_5,
	DI_FS_LOGICAL_PARTITION_6,
	DI_FS_LOGICAL_PARTITION_7,
	DI_FS_LOGICAL_PARTITION_8,
	DI_FS_LOGICAL_PARTITION_9,
	DI_FS_LOGICAL_PARTITION_10,
	DI_FS_MAX_PARTITIONS
};

enum
{
	DI_FS_EXT_OPTION_DEFAULT,
	DI_FS_EXT_OPTION_DEBUG
};

typedef enum
{
	DI_FS_MNTOPT_RDONLY      = 0x0001,
	DI_FS_MNTOPT_NOSUID      = 0x0002,
	DI_FS_MNTOPT_NODEV       = 0x0004,
	DI_FS_MNTOPT_NOEXEC      = 0x0008,
	DI_FS_MNTOPT_SYNCHRONOUS = 0x0010,
	DI_FS_MNTOPT_REMOUNT     = 0x0020,
	DI_FS_MNTOPT_MANDLOCK    = 0x0040,
	DI_FS_MNTOPT_NOATIME     = 0x0080,
	DI_FS_MNTOPT_NODIRATIME  = 0x0100,
	DI_FS_MNTOPT_BIND        = 0x0200,
	DI_FS_MNTOPT_MOVE        = 0x0400
} DI_FS_MNTOPT_E;

enum
{
	/* HDD status */
	DI_HDD_STAT_OK					= 0x5000, 			/**<HDD status' OK */
	DI_HDD_STAT_NOHDD,									/**<NO HDD */
	DI_HDD_STAT_LOCKED, 
	DI_HDD_STAT_UNLOCKED,
	DI_HDD_STAT_NOT_FORMATTED,							/** <need to format */

	/* device errors */
	DI_ERR_INVALID_PARAMTER			= 0x5201,			// ERR_DCM_INVALID_ARGUMNET
	DI_ERR_EEPROM_FAIL				= 0x5225, 			// ERR_DCM_EEPROM_FAIL
	DI_ERR_MALLOC_FAIL				= 0x5202, 			// ERR_DCM_MALLOC_FAIL

	/* errors related to HDD */
	DI_ERR_HDD_SECURITY_FAIL		= 0x5232, 			// ERR_DCM_HDD_SECURITY_FAIL
	DI_ERR_HDD_SAME_PARTITION		= 0x5233,			// ERR_DCM_HDD_SAME_PARTITION
};

/** @brief HDD Status Type */
typedef enum
{
	eHddStatus_OK					= 0,			/**< HDD ok */
	eHddStatus_NoHDD,								/**< NO HDD */
	eHddStatus_NoINIT,								/**< handler not initialized */
	eHddStatus_InvalidPartition, 					/**< need to format */
	eHddStatus_Malfunctioning,						/**< might be out of order */
	eHddStatus_InFormat, 							/**< hdd is in formatting */

	eHddStatus_FDISK_HDD1			= 10,
	eHddStatus_FDISK_HDD2,
	eHddStatus_FDISK_HDD3,
	eHddStatus_FDISK_USER, 			// hd2
	eHddStatus_FDISK_ALL, 			// all hdds

	eHddStatus_FORMAT_HDD1			= 20,
	eHddStatus_FORMAT_HDD2, 
	eHddStatus_FORMAT_HDD3,
	eHddStatus_FORMAT_USER,			// hd2
	eHddStatus_FORMAT_ALL, 			// all hdds

	eHddStatus_MAX
} DI_PVR_HddStatus_t;

/** @brief partition information */
typedef struct
{
	HBOOL				not_used;							/**< is cleared */
	HUINT64				start_unit;							/**< # of starting cylinder */
	HUINT64				end_unit;							/**< of end cylinder */
	DI_FileSystemType_t	fs_type;							/**< partition type */
} DI_PartitionInfo_t;

/** @brief partition list */
typedef struct
{
	HUINT32				unit_size;									/**< device unit size */
	HUINT32				unit_num;									/**< total cylinder number */
	HUINT32				partition_num;								/**< current partition number */
	HUINT8				device_name[DI_FS_MAX_DEV_NAME_LEN + 1];
	DI_PartitionInfo_t	partition_info[DI_FS_MAX_PARTITIONS];
} DI_PartitionList_t;

// Mount Info
typedef struct
{
	HUINT32					 ulPartIdx;

	HUINT8					 szMountPath[DI_FS_MOUNT_PATH_LEN];
	HBOOL					 bWritable;
	DI_FileSystemType_t		 eFsType;

} DI_FS_MountPartInfo_t;

typedef struct
{
	DI_FS_StorDevIdx_t		 eDevIdx;

	HUINT32					 ulNumPartition;
	DI_FS_MountPartInfo_t	 astMntPartInfo[DI_MAX_PARTITION];
} DI_FS_MountInfo_t;

typedef struct
{
	// Format 할 Partition의 정보 : Device Index & Partition Number
	DI_FS_StorDevIdx_t		 eDevIdx;
	unsigned int			 ulPartitionNo;

	char					 szMountPath[32];
	char					 szVolumeName[VOLNAMSZ];

	unsigned char			 ePartFsType;
	unsigned long			 ulOption;
} DI_FS_FormatItem_t;

typedef struct
{
	int		 nNumFormatPartition, nMaxFormatPartition;
	int		 nNumCurrPartition;

	DI_FS_FormatItem_t		*pstPartParam;
} DI_FS_FormatList_t;

typedef struct
{
	HBOOL			 bAvailable;
	HUINT64			 ulStartUnit, ulEndUnit;	// 여기서 Unit는 보통 Cylinder 임.

	DI_FileSystemType_t	 eFsType;
} DI_FS_PartitionInfo_t;

typedef struct
{
	DI_FS_StorDevIdx_t		 eDevIdx;
	HUINT8					 szDevFileName[DI_FS_DEVFILE_LEN];

	HUINT64					 ullTotalSize;
	HUINT64					 ullUnitSize;
	HUINT32					 ulSectorSize;
	HUINT32					 ulNumUnit;
	HUINT32					 ulNumPartition;
	DI_FS_PartitionInfo_t	 astPartition[DI_FS_MAX_PARTITIONS];
} DI_FS_PartitionList_t;

typedef struct hddformat_info_s
{	
	HINT32 partitionNumber;								// It's partionNumber in HDD
	HINT32 devIdx;										// It's device index
	HINT32 group_desc_count;							// It's the total group descriptor 
	HINT32 progress;									// It's the progress on the format
	HUINT8 progress_state[32];							// It's state on the format
	HUINT8 device_name[DI_FS_MAX_DEV_NAME_LEN + 1];		// It's the device name on the format
	HUINT8 reserved[32];								// reserved
}hddformat_info_t;

typedef struct
{
	HUINT32 node;
	HUINT32 offset;
	HUINT32 reclen;
	HUINT8 type;
	HCHAR name[DI_FS_DIRENT_MAX_NAME_LEN];
} DI_FS_DIRENT_INFO_t;

/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

/******************************************************************************
 * Extern Variables
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/

/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/
/*
	di_fs.c
*/
DI_ERR_CODE DI_FS_GetDeviceFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName);
DI_ERR_CODE DI_FS_GetDeviceFileFullName (DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName);
DI_ERR_CODE DI_FS_GetPartitionFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName, HUINT8 nSizeOfFileName);
DI_ERR_CODE DI_FS_GetPartitionFullFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName);

DI_ERR_CODE DI_FS_Mkdir(const HCHAR *dirname);

DI_FS_BOOL DI_FS_IsDirExist(const HCHAR *dirname);
DI_FS_BOOL DI_FS_IsFileExist(const HCHAR *pathname);
DI_FS_BOOL DI_FS_HDDExist(const HCHAR *pucDeviceName);

/*
	di_fs_partition.c
*/
typedef void	(*DI_FS_Event_Callback_Func)(HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

DI_ERR_CODE DI_FS_RegisterFormatProgressCallback (DI_FS_Event_Callback_Func pfEvtCallback);

// DI_FS_XxxxDevMbrBuf 함수들을 사용하는데 반드시 필요한 초기 함수
// di_fs 내의 Device MBR Buffer를 열어두어 Device MBR을 수정할 수 있도록 한다.
DI_ERR_CODE DI_FS_OpenDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx);

// 열려 있는 Device MBR buffer 에서 특정 Partition 정보를 지우는 함수.
DI_ERR_CODE DI_FS_DeletePartitionInDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx);

// 열려 있는 Device MBR buffer 에서 모든 Partition 정보를 지우는 함수.
DI_ERR_CODE DI_FS_DeleteAllPartitionInDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx);

// 열려 있는 Device MBR buffer 에서 특정 Partition 정보를 덮어 쓰는 함수.
DI_ERR_CODE DI_FS_UpdatePartitionInDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, DI_FileSystemType_t eFsType);
DI_ERR_CODE DI_FS_SetPartitionInDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT64 ullPartSizeKb, DI_FileSystemType_t eFsType);

// Device MBR buffer 의 내용을 실제 Storage Device의 MBR 영역에 덮어 쓰는 함수.
// DI_FS_UpdatePartitionInDevMbrBuf 등으로 내용을 수정한 후에 실제로 덮어 쓰는 것이다.
DI_ERR_CODE DI_FS_SyncDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx);
DI_ERR_CODE DI_FS_CloseDevMbrBuf (DI_FS_StorDevIdx_t eDevIdx);

// 특정 Partition 정보를 읽어 오는 함수.
DI_ERR_CODE DI_FS_GetPartitionFromDeviceMbr (DI_FS_StorDevIdx_t eDevIdx, DI_FS_PartitionList_t *pstPartList);
DI_ERR_CODE DI_FS_EraseAllPartitionInDeviceMbr (DI_FS_StorDevIdx_t eDevIdx);

DI_ERR_CODE DI_FS_InitFormatList (DI_FS_FormatList_t *pstList);
DI_ERR_CODE DI_FS_AddFormatItemToList (DI_FS_FormatList_t *pstList, DI_FS_FormatItem_t *pstItem);
DI_ERR_CODE DI_FS_ClearFormatList (DI_FS_FormatList_t *pstList);
DI_ERR_CODE DI_FS_FormatLists (DI_FS_FormatList_t *pstFormatList, HUINT32 nonblock);

DI_ERR_CODE DI_FS_MountPartition (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType);
DI_ERR_CODE DI_FS_MountPartitionExt (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType, DI_FS_MNTOPT_E eMntOpt);
DI_ERR_CODE DI_FS_MountPartitionOpt (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType, DI_FS_MNTOPT_E eMntFlags, const void *pMntOpts);
DI_ERR_CODE DI_FS_UnmountPartition (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType);
DI_ERR_CODE DI_FS_GetMountedPartitionStatus (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eDiFsType, HUINT64 *pulTotalSize, HUINT64 *pulUsableSize, HBOOL *pbWritable);

DI_ERR_CODE DI_FS_EnableHddToBdModule (DI_FS_StorDevIdx_t eDevIdx);
DI_ERR_CODE DI_FS_DisableHddToBdModule (DI_FS_StorDevIdx_t eDevIdx);

DI_ERR_CODE DI_FS_Parted_Mklabel(const HCHAR *dev, const HCHAR *labelType);
DI_ERR_CODE DI_FS_Parted(const HCHAR *dev, HINT32 partition, HUINT8 format_type, HUINT32 secsize, HUINT64 start, HUINT64 end);
DI_ERR_CODE DI_FS_Parted_DeletePartition(const HCHAR *disk, HINT32 partition);

DI_ERR_CODE DI_FS_GetVolumeNtfsUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 size);
DI_ERR_CODE DI_FS_GetVolumeXfsUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 size);
DI_ERR_CODE DI_FS_GetVolumeUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 bufsize, HUINT32 *uuidsize);

DI_ERR_CODE DI_FS_GetVolumeName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 bufsize);
DI_ERR_CODE DI_FS_SetVolumeName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf);
DI_ERR_CODE DI_FS_GetFsType (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t *eFsType );
DI_ERR_CODE DI_FS_ExecuteCheckDisk(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType);

DI_ERR_CODE DI_FS_FLASH_MountPartitions(void);
DI_ERR_CODE DI_FS_FLASH_UmountPartitions(void);

DI_ERR_CODE DI_FS_ENCR_Format(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szPassword);
DI_ERR_CODE DI_FS_ENCR_Open(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMapperDeviceName, char *szPassword);
DI_ERR_CODE DI_FS_ENCR_Close(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx);
DI_ERR_CODE DI_FS_ENCR_MakeFS(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szVolumeLable, DI_FileSystemType_t eFsType);
DI_ERR_CODE DI_FS_ENCR_Mount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMountDir, DI_FileSystemType_t eFsType);
DI_ERR_CODE DI_FS_ENCR_Umount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx);
DI_ERR_CODE DI_FS_ENCR_Init(void);


/******** For UCOS ********/
DI_ERR_CODE DI_FS_Mount(const HCHAR *pucDeviceName, const HCHAR *pucMountPtr, HINT32 partition, HINT32 type);
DI_ERR_CODE DI_FS_UMount(const HCHAR *pucMountPtr);
DI_FS_HANDLE DI_FS_FOpen(const char *pucFileName, const char *pucMode);
DI_FS_SIZE DI_FS_FWrite(void *pvBufferPtr, DI_FS_SIZE ulSize, HUINT32 ulCount, DI_FS_HANDLE hFileHandle);
DI_FS_SIZE DI_FS_FRead(void *pvBufferPtr, DI_FS_SIZE ulSize, HUINT32 ulCount, DI_FS_HANDLE hFileHandle);
DI_ERR_CODE DI_FS_FClose(DI_FS_HANDLE hFileHandle);
DI_ERR_CODE DI_FS_FSeek(DI_FS_HANDLE hFileHandle, DI_FS_SIZE offset, DI_FS_ORIGIN_e eOrigin);
DI_FS_SIZE DI_FS_FTell(DI_FS_HANDLE hFileHandle);
DI_FS_DIRENT_HANDLE DI_FS_FOpenDir(const HCHAR *pucPath);
DI_ERR_CODE DI_FS_FReadDir(DI_FS_DIRENT_HANDLE hDirentHandle, DI_FS_DIRENT_INFO_t *pDirentInfo);
DI_ERR_CODE DI_FS_FCloseDir(DI_FS_DIRENT_HANDLE hDirentHandle);

#endif /* _DI_FS_H_ */
/** @} */ // end of DI_FS



