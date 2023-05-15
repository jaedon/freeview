/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	umma_fs_pal_fs.h
	@brief 		file I/O wrapper

	Description: 		\n
	Module: pal/include			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __UMMA_FS_PAL_FS_H__
#define __UMMA_FS_PAL_FS_H__

#include <htype.h>
#include <hlib.h>

#define	CONV_PALFS_2_DIFS_TEST

#if WIN32
#define BLKSSZGET			0
#define BLKGETSIZE64		0
#else
#define BLKSSZGET			_IO(0x12,104)
#define BLKGETSIZE64		_IOR(0x12,114,size_t)
#endif

/* partition related */
#define MAX_SEC_SIZE		2048
#define MIN_SEC_SIZE		512

#define PAL_FS_FILE_DIRECTORY		0x00000001
#define PAL_FS_FILE_STREAM			0x00000002
#define PAL_FS_FILE_CYCLIC			0x00000004
#define PAL_FS_FILE_BOOT			0x00000008
#define PAL_FS_MODE_READ			0x00000010
#define PAL_FS_MODE_WRITE			0x00000020
#define PAL_FS_MODE_CYCLIC			PAL_FS_FILE_CYCLIC
#define PAL_FS_MODE_PATH			0x00000080
#define PAL_FS_MODE_RDWR			0x00000100
#define PAL_FS_MODE_APPEND			0x00000200
#define PAL_FS_MODE_TRUNC			0x00000400
#define PAL_FS_MODE_NONBLOCK		0x00000800
#define PAL_FS_MODE_O_DIRECT		0x00001000
#define PAL_FS_MODE_O_LARGEFILE		0x00002000

#define	PAL_FS_NULL_FILE_HANDLE		(~0U)

/* directory mask */
#define PAL_FS_DIRINFO_ALL							0xFFFFFFFF
#define PAL_FS_DIRINFO_FILE							0x0000FFFF
#define PAL_FS_DIRINFO_DIR							0xFFFF0000

#define PAL_FS_DIRINFO_VIDEO						0x00000001
#define PAL_FS_DIRINFO_PLAYLIST						0x00000002
#define PAL_FS_DIRINFO_MOVIE						0x00000010
#define PAL_FS_DIRINFO_MUSIC						0x00000020
#define PAL_FS_DIRINFO_IMAGE						0x00000040


#if WIN32
#define PAL_FS_EXT_STORAGE_MOUNT_PATH	".\\media\\"
#else
#define PAL_FS_EXT_STORAGE_MOUNT_PATH	"/media/"
#endif
#define PAL_FS_EXT_STOR_INTER_PATH		"drive"

#define PAL_FS_USB_MOUNT_PATH_LEN			4

#define PAL_MAX_PARTITION				10			// USB partitions support
#define PAL_FS_LOGICAL_PARTITION_NUM	4

#define MEGA_BYTES						(1000 * 1000) /* XXX no binary prefix */

#define EXT2_SUPER_MAGIC 				0xEF53
#define VOLNAMSZ 						16
#define VOLUUIDSZ						36			/* maybe 36, unparsed */

#define PAL_FS_MAX_COPY_INST_NUM		4096

#define PAL_FS_DEVFILE_LEN				16			/* /dev/sda1 /dev/sda2 등 */
#define PAL_FS_MOUNT_PATH_LEN			32			/* /mnt/hd1, /media/drive1 등 */

enum
{
	PAL_FS_FILE_TYPE_FILE,
	PAL_FS_FILE_TYPE_DIR,
	PAL_FS_FILE_TYPE_CHAR_DEV,
	PAL_FS_FILE_TYPE_BLOCK,
	PAL_FS_FILE_TYPE_FIFO,
	PAL_FS_FILE_TYPE_LINK,
	PAL_FS_FILE_TYPE_SOCKET,
	PAL_FS_FILE_TYPE_UNKNOWN
};

enum
{
	PAL_FS_POS_START,
	PAL_FS_POS_CURRENT,
	PAL_FS_POS_END
};

enum
{
	PAL_FS_INFO_NAMETYPE,
	PAL_FS_INFO_EXCEPTHIDDEN,
	PAL_FS_INFO_ALLLIST,
	PAL_FS_INFO_RECURSIVEALL,
	PAL_FS_INFO_TIME
};

enum
{
	PAL_FS_CMD_GET_DISK_INFO,
	PAL_FS_CMD_GET_FILE_INFO,
	PAL_FS_CMD_GET_FILE_INFO_INDIR,
	PAL_FS_CMD_GET_DIR_ENTRY,
	PAL_FS_CMD_RENAME,
	PAL_FS_CMD_FSMAP_GET,
	PAL_FS_CMD_FSMAP_SET,
	/* custom ioctl command */
	PAL_FS_CMD_GET_WRITE_PROTECTION = 0x5388,
};

enum
{
	PAL_FS_EVENT_BASE 				= 10,
	PAL_FS_EVENT_DISKFREESPACEFLOW 	= PAL_FS_EVENT_BASE,
	PAL_FS_EVENT_HDD_ON,
	PAL_FS_EVENT_HDD_OFF,
	PAL_FS_EVENT_DISKINITSTART,
	PAL_FS_EVENT_DISKINITEND,
	PAL_FS_EVENT_DISKERROR,
	PAL_FS_EVENT_HDDLOCKED,
	PAL_FS_EVENT_HDDUNLOCKED,
	PAL_FS_EVENT_USB_CONNECTED,
	PAL_FS_EVENT_USB_DISCONNECTED,
	PAL_FS_EVENT_USB_SCSI_ADDED,
	PAL_FS_EVENT_USB_SCSI_REMOVED,
	PAL_FS_EVENT_USB_OVERCURRENT,
	PAL_FS_EVENT_FORMAT_PROGRESS,
	PAL_FS_EVENT_FORMAT_DONE,
	PAL_FS_EVENT_FORMAT_FAIL,
	PAL_FS_EVENT_FS_TASK_ADD_FILEOP,		/* task add할 때 너무 오래 걸리면 위쪽으로 알려준다. */
	PAL_FS_EVENT_COPY_PROGRESS,
	PAL_FS_EVENT_COPY_DONE,
	PAL_FS_EVENT_DELETE_PROGRESS,
	PAL_FS_EVENT_MOVE_DONE,
	PAL_FS_EVENT_MAX_NUMBER
};

// File System Error codes
enum
{
	PAL_FS_EOF_ERR					= 0xA100,
	PAL_FS_DISK_ERROR,
	PAL_FS_REACH_READ_POINTER,
	PAL_FS_DISK_FULL,
	PAL_FS_SEEK_OUTSIDE_FILE,
	PAL_FS_NOT_FORMATTED_DISK_ERR,
	PAL_FS_DISK_NOT_EXIST_ERR,
	PAL_FS_FILE_NOT_EXIST_ERR,
	PAL_FS_FOLDER_NOT_EXIST_ERR,
	PAL_FS_ERROR_FILE_IN_USE,
	PAL_FS_NOT_SUPPORTED_CMD_ERR,
	PAL_FS_BAD_PARAMETER_ERR,
	PAL_FS_OPERATION_PROTECT_ERR,
	PAL_FS_READ_ONLY_ERR,
	PAL_FS_ERROR_INVALID_HANDLE,
	PAL_FS_ERROR_INVALID_OPENMODE,
	PAL_FS_ERROR_NO_MEMORY,
	PAL_FS_ERROR_NO_MORE_RESOURCES,
	PAL_FS_ERROR_SECURITY_LOCKED,
	PAL_FS_ERROR_INVALID_VALUE,
	PAL_FS_ERROR_FORK,
	PAL_FS_ERROR_FORK_PID,
	PAL_FS_ERROR_FILE_OPEN,
	PAL_FS_ERROR_FILE_READ,
	PAL_FS_ERROR_FILE_WRITE,
	PAL_FS_ERROR_FILE_CLOSE,
	PAL_FS_ERROR_IO,					// EIO
	PAL_FS_ERR_OUTOFMEMORY,
} PAL_FS_ERR_CODE;

typedef enum
{
	PAL_FS_EXT2_FILESYSTEM = 2,
	PAL_FS_EXT3_FILESYSTEM,
	PAL_FS_EXT4_FILESYSTEM,
	PAL_FS_XFS_FILESYSTEM,
	PAL_FS_FAT16_FILESYSTEM,
	PAL_FS_FAT32_FILESYSTEM,
	PAL_FS_NTFS_FILESYSTEM,
	PAL_FS_MSDOS_FILESYSTEM,
	PAL_FS_EXFAT_FILESYSTEM,
	PAL_FS_OTHER_FILESYSTEM = 0xFF
} PAL_FS_FsType_t;

typedef enum
{
	ePAL_FS_DEV_NULL	= -1,
	ePAL_FS_DEV_sda		= 0,
	ePAL_FS_DEV_sdb,
	ePAL_FS_DEV_sdc,
	ePAL_FS_DEV_sdd,
	ePAL_FS_DEV_sde,
	ePAL_FS_DEV_sdf,
	ePAL_FS_DEV_sdg,
	ePAL_FS_DEV_sdh,
	ePAL_FS_DEV_sdi,

	ePAL_FS_DEV_mmc,

	ePAL_FS_DEV_MAX
} PAL_FS_StorDevIdx_t;

#define PAL_FS_DEV_IDX_START				ePAL_FS_DEV_sda
#define PAL_FS_DEV_IDX_END					(ePAL_FS_DEV_MAX - 1)

enum
{
	PAL_FS_EXT_OPTION_DEFAULT,
	PAL_FS_EXT_OPTION_DEBUG
};

enum
{
	PAL_FS_CHKRECMODE_NORMAL,
	PAL_FS_CHKRECMODE_DELAY
};

enum
{
	PAL_FS_EDIT_SPLIT = 1,
	PAL_FS_EDIT_PARTIAL_SAVE,
	PAL_FS_EDIT_PARTIAL_DELETE,
	PAL_FS_EDIT_PARTIAL_MAX_NUMBER
};

enum
{
	PAL_FS_NO_PARTITION = -2,
	PAL_FS_EXTEDED_PARTITION = - 1,
	PAL_FS_PRIMARY_PARTITION_1 = 0,
	PAL_FS_PRIMARY_PARTITION_2,
	PAL_FS_PRIMARY_PARTITION_3,
	PAL_FS_PRIMARY_PARTITION_4,
	PAL_FS_LOGICAL_PARTITION_5,
	PAL_FS_LOGICAL_PARTITION_6,
	PAL_FS_LOGICAL_PARTITION_7,
	PAL_FS_LOGICAL_PARTITION_8,
	PAL_FS_LOGICAL_PARTITION_9,
	PAL_FS_LOGICAL_PARTITION_10,
//	PAL_FS_LOGICAL_PARTITION_11,
//	PAL_FS_LOGICAL_PARTITION_12,
	PAL_FS_MAX_PARTITIONS
};

enum
{
	PAL_FS_FDISK_MAKE_PARTITION = 1,
	PAL_FS_FDISK_DELETE_PARTITION,
	PAL_FS_FDISK_FORMAT_PARTITION,
	PAL_FS_FDISK_FORMAT_ALL_PARTITION
};

typedef enum
{
	ePAL_FS_DST_HDD_NOT_DETECTED			= 0,
	ePAL_FS_DST_FAILED_TRY_AGAIN_LATER,
	ePAL_FS_DST_IS_ON_GOING,
	ePAL_FS_DST_COMPLETED_OK,
	ePAL_FS_DST_COMPLETED_FATAL_ERROR,
	ePAL_FS_DST_COMPLETED_TEST_ELEMENT_FAILED,
	ePAL_FS_DST_COMPLETED_ELECTRICAL_ELEMENT_FAILED,
	ePAL_FS_DST_COMPLETED_SERVO_ELEMENT_FAILED,
	ePAL_FS_DST_COMPLETED_READ_ELEMENT_FAILED,
	ePAL_FS_DST_COMPLETED_HANDLING_DAMAGE,
	ePAL_FS_DST_COMPLETED_UNKNOWN
} PAL_FS_HDD_DST_RESULT_e;

typedef enum
{
	ePAL_FS_MNTOPT_RDONLY      = 0x0001,
	ePAL_FS_MNTOPT_NOSUID      = 0x0002,
	ePAL_FS_MNTOPT_NODEV       = 0x0004,
	ePAL_FS_MNTOPT_NOEXEC      = 0x0008,
	ePAL_FS_MNTOPT_SYNCHRONOUS = 0x0010,
	ePAL_FS_MNTOPT_REMOUNT     = 0x0020,
	ePAL_FS_MNTOPT_MANDLOCK    = 0x0040,
	ePAL_FS_MNTOPT_NOATIME     = 0x0080,
	ePAL_FS_MNTOPT_NODIRATIME  = 0x0100,
	ePAL_FS_MNTOPT_BIND        = 0x0200,
	ePAL_FS_MNTOPT_MOVE        = 0x0400
} PAL_FS_MNTOPT_e;



typedef char				PAL_FS_INT8;
typedef short				PAL_FS_INT16;
typedef long				PAL_FS_INT32;
typedef long long			PAL_FS_INT64;
typedef unsigned char		PAL_FS_UINT8;
typedef unsigned short		PAL_FS_UINT16;
typedef unsigned long		PAL_FS_UINT32;
typedef unsigned long long	PAL_FS_UINT64;
typedef int					PAL_FS_BOOL;
typedef int					PAL_FS_ResultCode_t;
typedef int					PAL_FS_PARTITION_NUMBER_t;

typedef void	(*PAL_FS_ScanDir_Callback_Func)(char *);
typedef int		(*PAL_FS_CopyDir_Callback_Func)(char *, char *);
typedef void	(*PAL_FS_Event_Callback_Func)(HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
typedef void	(*PAL_FS_DST_Callback_Func)(PAL_FS_StorDevIdx_t eDevIdx, PAL_FS_HDD_DST_RESULT_e eDstResult, HUINT32 ulProgress);


#if WIN32
#define NAME_MAX			256
#endif

#ifndef NAME_MAX
#include <limits.h>
#endif

#define PAL_FS_MAX_NAME_LEN	NAME_MAX

typedef struct
{
	PAL_FS_UINT8		aucFileName[NAME_MAX];
	PAL_FS_UINT32		fileType;
	PAL_FS_UINT64		ulFileSize;
	PAL_FS_UINT32		aTime;
	PAL_FS_UINT32		mTime;
	PAL_FS_UINT32		cTime;
} PAL_FS_FileStat_t;

typedef struct
{
	PAL_FS_UINT32		unFileHandle;
	PAL_FS_UINT8		*szFileName;
	PAL_FS_UINT64		ulFileSize;
} PAL_FS_FileInfo_t;

typedef struct
{
	PAL_FS_UINT8		aucSrcName[NAME_MAX];
	PAL_FS_UINT8		aucDestName[NAME_MAX];
} PAL_FS_Rename_t;

typedef struct
{
	PAL_FS_UINT64		ulFreeSpace;
	PAL_FS_UINT64		ulTotalSpace;
} PAL_FS_DiskInfo_t;

typedef struct
{
	PAL_FS_UINT32		ulDiskTotalKByteSize;
	PAL_FS_UINT32		ulDiskFreeKByteSize;
} PAL_FS_DiskStat_t;

typedef struct
{
	PAL_FS_INT32		fileNumber;
	PAL_FS_FileStat_t	*s_astfilestats;
} PAL_FS_DirStat_t;

typedef struct
{
	PAL_FS_UINT8		aucName[NAME_MAX];
	PAL_FS_BOOL			bFlagDir;
	PAL_FS_UINT64		ulFileSize;
} PAL_FS_DirEntry_t;

typedef struct
{
	PAL_FS_UINT32		unFileHandle;
	char				*szFileName;
	PAL_FS_UINT32		unUsage;
} PAL_FS_TSR_Usage_t;

typedef struct
{
	int					iscleared;
	unsigned long long	cyl;
	unsigned long long	end_cyl;
	unsigned char		sys_ind;
} PAL_FS_PARTITION_INFO;

typedef struct
{
	char					*deviceName;
	unsigned long long		deviceTotalSize;
	unsigned long long		deviceUnitSize;
	unsigned int			deviceSectorSize;
	unsigned int			deviceTotalCylinder;
	PAL_FS_PARTITION_INFO	partition[PAL_FS_MAX_PARTITIONS];
	int						numberOfCurentPartitions;
} PAL_FS_PARTITION_LISTS;

typedef struct
{
	int					partitionNumber;
	unsigned char		type;
	unsigned long		option;
} PAL_FS_PARTITION_PARAMS;

typedef struct
{
	char						*deviceName;
	char						*mountName;
	char						*volumeLabel;
	PAL_FS_PARTITION_PARAMS 	fsParam[PAL_FS_MAX_PARTITIONS];
	int							numberOfFormatPartitions;
	int							numberOfCurentPartitions;
} PAL_FS_PARTITION_FORMAT_LISTS;

typedef struct
{
	char			*sourceName;
	char			*destName;
	int				flag;
} PAL_FS_FILE_COMMAND_INFO;

#if 0
// Mount Info
typedef struct
{
	HUINT32					 ulPartIdx;

	HUINT8					 szMountPath[PAL_FS_MOUNT_PATH_LEN];
	HBOOL					 bWritable;
	PAL_FS_FsType_t			 eFsType;

} PAL_FS_MountPartInfo_t;

typedef struct
{
	PAL_FS_StorDevIdx_t		 eDevIdx;

	HUINT32					 ulNumPartition;
	PAL_FS_MountPartInfo_t	 astMntPartInfo[PAL_MAX_PARTITION];
} PAL_FS_MountInfo_t;
#endif

// 차후 PAL_FS_PARTITION_PARAMS 를 대체할 Item.
typedef struct
{
	// Format 할 Partition의 정보 : Device Index & Partition Number
	PAL_FS_StorDevIdx_t		 eDevIdx;
	unsigned int			 ulPartitionNo;

	char					 szMountPath[32];
	char					 szVolumeName[VOLNAMSZ];

	unsigned char			 ePartFsType;
	unsigned long			 ulOption;
} PAL_FS_FormatItem_t;

// 차후 PAL_FS_PARTITION_FORMAT_LISTS 를 대체할 Item
typedef struct
{
	int		 nNumFormatPartition, nMaxFormatPartition;
	int		 nNumCurrPartition;

	PAL_FS_FormatItem_t		*pstPartParam;
} PAL_FS_FormatList_t;


// Partition Info : 차후 PAL_FS_PARTITION_INFO 를 대체한다.
typedef struct
{
	HBOOL			 bAvailable;
	HUINT64			 ulStartUnit, ulEndUnit;	// 여기서 Unit는 보통 Cylinder 임.

	PAL_FS_FsType_t	 eFsType;
} PAL_FS_PartitionInfo_t;

// Partition List : 차후 PAL_FS_PARTITION_LISTS 를 대체한다.
typedef struct
{
	PAL_FS_StorDevIdx_t		 eDevIdx;
	HUINT8					 szDevFileName[PAL_FS_DEVFILE_LEN];

	HUINT64					 ullTotalSize;
	HUINT64					 ullUnitSize;
	HUINT32					 ulSectorSize;
	HUINT32					 ulNumUnit;
	HUINT32					 ulNumPartition;
	PAL_FS_PartitionInfo_t	 astPartition[PAL_FS_MAX_PARTITIONS];
} PAL_FS_PartitionList_t;

#define USBHDD_VENDOR_LEN		50
#define USBHDD_PRODUCT_LEN		50
#define USBHDD_SERIALNUM_LEN		50


typedef struct
{
	HUINT8 aucUsbHddVendor[USBHDD_VENDOR_LEN];
	HUINT8 aucUsbHddProduct[USBHDD_PRODUCT_LEN];
	HUINT8 aucUsbHddSerialNum[USBHDD_SERIALNUM_LEN];
} PAL_HDD_USBHDD_INFO_t;


typedef enum{
	ePvrEncryptNone		= 0,
	ePvrEncrypt3Des 	= 1,
	ePvrEncryptAes		= 2,
	ePvrEncryptUnknown	= 3,
}PvrEncryptMode_t;


HERROR PAL_FS_GetDeviceFileName (PAL_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName);
HERROR PAL_FS_GetDeviceFileFullName (PAL_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName);
HERROR PAL_FS_GetPartitionFileName (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName, HUINT8 nSizeOfFileName);
HERROR PAL_FS_GetPartitionFullFileName (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName);

int PAL_FS_Init(void);
int PAL_FS_RegCallback(PAL_FS_UINT32, void (*)(void *));

int PAL_FS_Open(int, const char *, PAL_FS_UINT32, PAL_FS_UINT32 *);
int PAL_FS_Close(PAL_FS_UINT32);
int PAL_FS_Read(PAL_FS_UINT32, PAL_FS_UINT8 *, PAL_FS_UINT32, PAL_FS_UINT32 *);
int PAL_FS_Write(PAL_FS_UINT32, const PAL_FS_UINT8 *, PAL_FS_UINT32, PAL_FS_UINT32 *);
int PAL_FS_LSeek(PAL_FS_UINT32, PAL_FS_INT64, PAL_FS_UINT32, PAL_FS_INT64 *);
int PAL_FS_Seek(PAL_FS_UINT32, PAL_FS_INT64, PAL_FS_UINT32, PAL_FS_BOOL);
int PAL_FS_Tell (PAL_FS_UINT32, PAL_FS_INT64 *);
int PAL_FS_SetDmaEnable(PAL_FS_UINT32, PAL_FS_BOOL);
int PAL_FS_Truncate(PAL_FS_UINT32, PAL_FS_INT64);



int PAL_FS_GetFileInfo(const char *, PAL_FS_UINT32, PAL_FS_FileStat_t *);
int PAL_FS_Rename(int, const char *, const char *);
int PAL_FS_Move(int, const char *, const char *);
int PAL_FS_MoveWithoutCallback(int pttype, const char *oldname, const char *newname);
void PAL_FS_OpenFileList(void);

int PAL_FS_GetDiskInfo(const char *, PAL_FS_DiskStat_t *);
PAL_FS_BOOL PAL_FS_HDDExist(const char *);
int PAL_FS_IsDiskError(const char *);
int PAL_FS_GoToStandby(void);
int PAL_FS_WakeupFromStandby(void);
PAL_FS_BOOL PAL_FS_IsHDDSecurityLocked(void);
int PAL_FS_ResetSecurity(void);
int PAL_FS_Ioctl(int, int, void *);

PAL_FS_BOOL PAL_FS_IsDirExist(const char *);
int PAL_FS_OpenDir(int, const char *, PAL_FS_UINT32 *);
int PAL_FS_CloseDir(PAL_FS_UINT32);
int PAL_FS_ReadDir(PAL_FS_UINT32, PAL_FS_DirEntry_t *);
int PAL_FS_Mkdir(const char *);
int PAL_FS_ChangeDir(const char *);
int PAL_FS_GetDirInfo(const char *, PAL_FS_UINT8 *, PAL_FS_UINT32, PAL_FS_DirStat_t *);
int PAL_FS_GetDirInfoFree(PAL_FS_DirStat_t *);
int PAL_FS_GetNumOfDirList(int, const char *, PAL_FS_UINT32 *);
int PAL_FS_ScanDirRecursive(char *, PAL_FS_ScanDir_Callback_Func);
int PAL_FS_CopyDirRecursive(char *, char *, PAL_FS_CopyDir_Callback_Func callback, int *);
int PAL_FS_CountFile(const char *, char **, unsigned int *);
int PAL_FS_FindFile(const char *, char **, unsigned int *, unsigned int *);
int PAL_FS_ScanDir(int, const char *, PAL_FS_UINT8 **, PAL_FS_UINT32 *, void ***);
int PAL_FS_ReleaseScanDir(PAL_FS_UINT32, void **);
int PAL_FS_GetScanDir(PAL_FS_UINT32, void **, PAL_FS_UINT8 *);
PAL_FS_BOOL PAL_FS_IsAvailable2Rec(int, int);

int PAL_FS_EstimateSpace(const char *, PAL_FS_UINT64 *);
int PAL_FS_EstimateUsage(const char *, PAL_FS_UINT8 **, PAL_FS_UINT64 *);

int PAL_FS_CopySyncMode(const char *, const char *);
int PAL_FS_Copy(const char *, const char *);
int PAL_FS_CopyWithUserdata(const char *src, const char *dst, void *userdata);
int PAL_FS_DecryptCopy(const char *, const char *);
int PAL_FS_StopDeleteProcess(void);
int PAL_FS_StopCopyProcess(void);
int PAL_FS_StopProcessSync(void);
int PAL_FS_Delete(int, const char *);
int PAL_FS_DeleteAsynch(int, const char *);
int PAL_FS_Flush(void);
int	PAL_FS_ClearRecycleBin (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx);

//int PAL_FS_Umount(const char *);
int PAL_FS_Umount(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, PAL_FS_FsType_t eFsType);
int PAL_FS_UmountAll(const char *, int);

int PAL_FS_GetVolumeUUID (PAL_FS_StorDevIdx_t, int, char *);
int PAL_FS_GetExt3VolumeLabel(PAL_FS_StorDevIdx_t, int, char *);
void PAL_FS_ChangeExt3VolumeLabel(char *, char *);

// for crypted api
HERROR PAL_FS_Secure_InitCryptoInfo(PvrEncryptMode_t eEncType, HUINT8 *pucKey, HUINT32 ulKeySize, HUINT8 *pucIv, HUINT32 ulIvSize);
int PAL_FS_Secure_Open(const char *pathname, PAL_FS_UINT32 flags, PAL_FS_UINT32 *filedes);
int PAL_FS_Secure_Close(PAL_FS_UINT32 filedes);
int PAL_FS_Secure_Read(PAL_FS_UINT32 filedes, PAL_FS_UINT8 *buf, PAL_FS_UINT32 size, PAL_FS_UINT32 *sizep);
int PAL_FS_Secure_Seek(PAL_FS_UINT32 filedes, PAL_FS_INT64 offset, PAL_FS_UINT32 whence);
int PAL_FS_Secure_Tell(PAL_FS_UINT32 filedes, PAL_FS_INT64 *poffset);
int PAL_FS_Secure_IsEOF(PAL_FS_UINT32  filedes);
int PAL_FS_Secure_GetFileSize(PAL_FS_UINT32 filedes, PAL_FS_INT64 *pulSize);



/*
	pal_fs_partition.c
*/
HERROR PAL_FS_RegisterFormatCallback (PAL_FS_Event_Callback_Func pfEvtCallback);

// PAL_FS_XxxxDevMbrBuf 함수들을 사용하는데 반드시 필요한 초기 함수
// pal_fs 내의 Device MBR Buffer를 열어두어 Device MBR을 수정할 수 있도록 한다.
HERROR PAL_FS_OpenDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx);

// 열려 있는 Device MBR buffer 에서 특정 Partition 정보를 지우는 함수.
HERROR PAL_FS_DeletePartitionInDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx);

// 열려 있는 Device MBR buffer 에서 모든 Partition 정보를 지우는 함수.
HERROR PAL_FS_DeleteAllPartitionInDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx);

// 열려 있는 Device MBR buffer 에서 특정 Partition 정보를 덮어 쓰는 함수.
HERROR PAL_FS_UpdatePartitionInDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, PAL_FS_FsType_t fsType);

HERROR PAL_FS_SetPartitionInDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT64 ullPartSizeKb, PAL_FS_FsType_t fsType);

// Device MBR buffer 의 내용을 실제 Storage Device의 MBR 영역에 덮어 쓰는 함수.
// PAL_FS_UpdatePartitionInDevMbrBuf 등으로 내용을 수정한 후에 실제로 덮어 쓰는 것이다.
HERROR PAL_FS_SyncDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx);
HERROR PAL_FS_CloseDevMbrBuf (PAL_FS_StorDevIdx_t eDevIdx);


// 특정 Partition 정보를 읽어 오는 함수.
HERROR PAL_FS_GetPartitionFromDeviceMbr (PAL_FS_StorDevIdx_t eDevIdx, PAL_FS_PartitionList_t *pstPartList);
HERROR PAL_FS_EraseAllPartitionInDeviceMbr (PAL_FS_StorDevIdx_t eDevIdx);

HERROR PAL_FS_InitFormatList (PAL_FS_FormatList_t *pstList);
HERROR PAL_FS_AddFormatItemToList (PAL_FS_FormatList_t *pstList, PAL_FS_FormatItem_t *pstItem);
HERROR PAL_FS_ClearFormatList (PAL_FS_FormatList_t *pstList);
HERROR PAL_FS_FormatLists (PAL_FS_FormatList_t *pstFormatList);

HERROR PAL_FS_MountPartition (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, PAL_FS_FsType_t eFsType, HBOOL bSecured); // MOON_encr
HERROR PAL_FS_MountPartitionExt (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, PAL_FS_FsType_t eFsType, HBOOL bSecured, PAL_FS_MNTOPT_e eMntOpt);
HERROR PAL_FS_UnmountPartition (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, PAL_FS_FsType_t eFsType, HBOOL bSecured); // MOON_encr
HERROR PAL_FS_GetMountedPartitionStatus (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, PAL_FS_FsType_t ePalFsType, HUINT64 *pulTotalSize, HUINT64 *pulUsableSize, HBOOL *pbWritable);
HERROR PAL_FS_GetRecycleDir (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HCHAR *szRecycleDir);
HERROR PAL_FS_GetRecycleDirByFileName (HCHAR *szFullFileName, HCHAR *szRecycleDir);
HERROR PAL_FS_GetRemovingFileNameToRecycleDir (HCHAR *szFullFileName, HCHAR *szMovedFileName);
HERROR PAL_FS_GetMountedFileSystemByFileName (HCHAR *szFullFileName, PAL_FS_FsType_t *peFsType);
HERROR PAL_FS_GetMountRootByFileName (const HCHAR *szFullFileName, HCHAR *szMountRoot, HUINT32 n);
HERROR PAL_FS_PartitionCheckDisk(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, PAL_FS_FsType_t eFsType);

#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM) // MOON_encr
HERROR PAL_FS_ENCR_Format (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szPassword);
HERROR PAL_FS_ENCR_Open (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMapperDeviceName, char *szPassword);
HERROR PAL_FS_ENCR_Close (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx);
#endif

// DST	: Disk Self Test
HERROR PAL_FS_DstStart(PAL_FS_StorDevIdx_t eDevIdx, PAL_FS_DST_Callback_Func pfnDstNotify);

HERROR PAL_FS_GetVolumeUuid(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf, HUINT32 ulBufsize, HUINT32 *pulUuidsize, PAL_FS_FsType_t eFsType);

HERROR PAL_FS_SetVolumeName(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf);
HERROR PAL_FS_GetVolumeName(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf, HUINT32 ulSize);

HERROR PAL_FS_GetFsType(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, PAL_FS_FsType_t *pFsType);
HERROR	PAL_FS_USBHDD_GetProductInfo(HUINT32 ulDeviceId, HUINT8* paucUsbHddVendor, HUINT8* paucUsbHddProduct, HUINT8* paucUsbHddSerialNum);


/*
	CHUNKED File을 위한 Layer
*/
HBOOL		PAL_FS_CHUNK_IsChunkedFile (HCHAR *szPath);
Handle_t	PAL_FS_CHUNK_Open (HCHAR *szPath, HCHAR *szMode);
HERROR		PAL_FS_CHUNK_Close (Handle_t hChunk);
HERROR		PAL_FS_CHUNK_Read (Handle_t hChunk, HUINT8 *pucBuf, HULONG ulSize, HULONG *pulSizeRead);
HERROR		PAL_FS_CHUNK_GetFileSize (Handle_t hChunk, HINT64 *pnSize);
HERROR		PAL_FS_CHUNK_Seek (Handle_t hChunk, HINT64 nOffset, HULONG ulWhence, HINT64 *pnAbsOffset);
HERROR		PAL_FS_CHUNK_Tell (Handle_t hChunk, HINT64 *pnAbsOffset);
HBOOL		PAL_FS_CHUNK_IsEof (Handle_t hChunk);
#endif /* !__UMMA_FS_PAL_FS_H__ */
