/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_fs_svc.h
	@brief	  umma_fs ������ ���������� �����Ǵ� ���� �� �Լ�

	Description: 		\n

	Module: Layer�� ��� ���� �����ϴ�.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__INT_FS_H__
#define	__INT_FS_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <umma_fs_pal_fs.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

extern unsigned long		 g_ulFsDevice_SemId;



/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#if defined(WIN32)
#define FS_PATH_SEPARATOR					'\\'
#else
#define FS_PATH_SEPARATOR					'/'
#endif

#define SVC_FS_SEM_GET							VK_SEM_Get (g_ulFsDevice_SemId)
#define SVC_FS_SEM_RELEASE						VK_SEM_Release (g_ulFsDevice_SemId)

#define	INT2NDPVR_PARTIDX		0


/*
	Directory Profile : Base
*/
#define PVR_PATH_NORMAL_RECORD_INT				"Recordings"
#define PVR_PATH_NORMAL_TSR_INT					".tsr"
#define PVR_PATH_NORMAL_MEDIA_INT					"Media"
#define PVR_PATH_NORMAL_VIDEO_INT					"Media/Video"
#define PVR_PATH_NORMAL_AUDIO_INT					"Media/Music"
#define PVR_PATH_NORMAL_PHOTO_INT					"Media/Photo"
#define PVR_PATH_NORMAL_DOWNLOAD_INT				"Media/Download"

#define PVR_PATH_NORMAL_RECORD_EXT				".recordings"
#define PVR_PATH_NORMAL_TSR_EXT					".tsr"
#define PVR_PATH_NORMAL_MEDIA_EXT					"Media"
#define PVR_PATH_NORMAL_VIDEO_EXT					"Media/Video"
#define PVR_PATH_NORMAL_AUDIO_EXT					"Media/Music"
#define PVR_PATH_NORMAL_PHOTO_EXT					"Media/Photo"
#define PVR_PATH_NORMAL_DOWNLOAD_EXT				"Media/Download"


/*
	Directory Profile : Legacy
*/
#define PVR_PATH_LEGACY_RECORD_INT					"My Video"
#define PVR_PATH_LEGACY_RECORD_EXT					"Video"
#define PVR_PATH_LEGACY_TSR_INT					".tsr"
#define PVR_PATH_LEGACY_TSR_EXT					".tsr"
#define PVR_PATH_LEGACY_MEDIA_INT					"My Media"
#define PVR_PATH_LEGACY_VIDEO_INT					"My Media"
#define PVR_PATH_LEGACY_AUDIO_INT					"My Music"
#define PVR_PATH_LEGACY_PHOTO_INT					"My Photo"

/*
	Directory Profile : Media In Video
*/
#define PVR_PATH_MIV_RECORD_INT					"My Video"
#define PVR_PATH_MIV_RECORD_EXT					"Video"
#define PVR_PATH_MIV_TSR_INT						".tsr"
#define PVR_PATH_MIV_TSR_EXT						".tsr"
#define PVR_PATH_MIV_MEDIA_INT						"My Video"
#define PVR_PATH_MIV_VIDEO_INT						"My Video"
#define PVR_PATH_MIV_AUDIO_INT						"My Music"
#define PVR_PATH_MIV_PHOTO_INT						"My Photo"


/*
	Partition Profile : Normal / TV APPS
*/

#define	HDD_BASICPROFILE_SYSTEM_PARTSIZE			1				// /mnt/hd1 , 1 GB
#define	HDD_BASICPROFILE_IP_PARTSIZE				10				// /mnt/hd3 , 10 GB
#define	HDD_BASICPROFILE_TVAPPS_PARTSIZE			10				// /mnt/hd4 , 10 GB

#define PVR_PATH_NORMAL_SYSTEM_ROOT					"/mnt/hd1"
#define PVR_PATH_NORMAL_MEDIA_ROOT					"/mnt/hd2"
#define PVR_PATH_NORMAL_RESERVED_ROOT				"/mnt/hd3"
#define PVR_PATH_NORMAL_TVAPPS_ROOT					"/mnt/hd4"
#define PVR_PATH_EXTENDED_MEDIA_ROOT				"/media/ehd1"



#define UMMA_FS_DEVICE_PATH_LEN				16
#define UMMA_FS_MOUNT_PATH_LEN				64
#define UMMA_FS_PVR_DIR_PATH_LEN				64
#define UMMA_FS_VOLUME_LABEL_SIZE				32+1	// 16+ NULL
#define UMMA_FS_MAX_PARTITION					12		// PAL_FS_MAX_PARTITIONS

#ifdef WIN32
#define UMMA_FS_EXTHDD_REGFILE			".\\.pvr_reg_info"
#define UMMA_FS_SAFELY_REMOVE_CHK_FILE	".\\.mountchk"
#else
#define UMMA_FS_EXTHDD_REGFILE			"/.pvr_reg_info"
#define UMMA_FS_SAFELY_REMOVE_CHK_FILE	"/.mountchk"
#endif

#define UMMA_FS_SAFELY_REMOVE_CHK_DATA	"MOUNTED TO STB"

#define UMMA_FS_EXTHDD_REGFILE_LEN		15

#define UMMA_FS_EXT_PVR_PART_IDX			0	// USB HDD�� PVR�� partition index (����: ���� USB HDD�� ���ؼ��� 1���� Partition�� ����ϵ��� �����Ǿ��ִ�.)

#define UMMA_FS_DEV_UUID_BUFSIZE				(VOLUUIDSZ+1) 		// pal_fs.h�� VOLUUIDSZ�� 36 + null char �ؼ� 37 �̻� �־�� �Ѵ�.

#define UMMA_FS_STB_UNIQUE_BUFSIZE			9		// 8 + null

#define MAX_CONNECTABLE_PVR_STORAGE_NUM		CONFIG_MW_MM_MAX_CONNECTABLE_PVR_STORAGE_NUM	// ���ÿ� �����Ͽ� �̿밡���� �ִ� PVR Storage ����
#if defined(CONFIG_MW_MM_MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM)
#define MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM		CONFIG_MW_MM_MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM	// ���ÿ� �����Ͽ� �̿밡���� �ִ� Playback Storage ����
#else
#define MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM		1
#endif

#define UMMA_FS_EVT_START 		0
#define UMMA_FS_CMD_START		0x100
#define UMMA_FS_MSG_START		0x200

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	// TODO: main,sub should be devided into record + playback + tsr + tsr pb flags
	// TODO: Rec storage should becomes playback flag
	// TODO: when loading recording, the device should have playback flag (not pvr flag)
	eFS_PVR_None		= 0x0000,
	eFS_PVR_Main		= 0x0001,
	eFS_PVR_Sub			= 0x0002,
	eFS_PVR_Tsr			= 0x0004,
	eFS_PVR_RecStorage	= 0x0008,
}UMMA_FS_PvrType_e;


typedef void	(*UxSendEvtCallback_t)(HINT32 nEvt, HINT32 nP1, HINT32 nP2, HINT32 nP3);
typedef void	(*UxSendDataCallback_t)(HINT32 nEvt, void *nP1, HINT32 nP1Size, void *nP2, HINT32 nP2Size, HINT32 nP3);

typedef enum
{
	eFS_EVT_NONE = UMMA_FS_EVT_START,

	eFS_EVT_CONNECTED,		// Mount Ok / Mount Fail /
	eFS_EVT_DISCONNECTED,	// Physically disconnected / Unmount Ok / Unmount Fail
//	eFS_EVT_STATUS_INVALID,			// Need Format, Over Current
	eFS_EVT_FORMAT_DONE,
	eFS_EVT_FORMAT_FAIL,
	eFS_EVT_FORMATTING,
	eFS_EVT_NEED_FORMAT,
	eFS_EVT_PVR_ENABLED,		// Ready To Use HDD for PVR device
	eFS_EVT_PVR_DISABLED,		// HDD is disabled for PVR device
	eFS_EVT_DST_DONE,			// DST Completed and Result OK
	eFS_EVT_DST_FAIL,			// DST Completed and Result Fail
	eFS_EVT_DST_ON_GOING,		// DST is on going
	eFS_EVT_LIST_UPDATE,
	eFS_EVT_UNMOUNT_FAIL,
	eFS_EVT_USB_OVERCURRENT,
	eFS_EVT_CHECK_DISK_STARTED,

	eFS_EVT_HDD_ADDED,
	eFS_EVT_HDD_REMOVED,

	eFS_EVT_USB_ADDED,
	eFS_EVT_USB_REMOVED,

	eFS_EVT_MMC_ADDED,
	eFS_EVT_MMC_REMOVED,

	eFS_EVT_POWER_OFF_DONE,

	eFS_EVT_TSR_ENABLED,
	eFS_EVT_TSR_DISABLED,

	eFS_EVT_PLAYBACK_ENABLED,
	eFS_EVT_PLAYBACK_DISABLED,

	eFS_EVT_DEFAULTHDD_INFO_CHANGED,

	eFS_EVT_LAST
}UMMA_FS_EVENT_e;

typedef enum
{
	eFS_SIGNAL_NONE		= UMMA_FS_CMD_START,
	eFS_SIGNAL_INIT,
	eFS_SIGNAL_FORMAT,
	eFS_SIGNAL_DST,
	eFS_SIGNAL_SET_DEFAULTHDD,
	eFS_SIGNAL_SET_NAME,
	eFS_SIGNAL_DISABLE_PVR,
	eFS_SIGNAL_PAIRING,
	eFS_SIGNAL_UNPAIRING,
	eFS_SIGNAL_SAFE_REMOVE,
	eFS_SIGNAL_STARTJOB,
	eFS_SIGNAL_CHECK_USB_SPEED,

	eFS_SIGNAL_APPREADY,
	eFS_SIGNAL_SHUTDOWN,
	eFS_SIGNAL_SYNC,
	eFS_SIGNAL_SHUTDOWNDONE,
	eFS_SIGNAL_OPERATION,
	eFS_SIGNAL_REBOOTDONE,

	eFS_SIGNAL_LAST
} UMMA_FS_SIGNAL_e;

typedef enum
{
	eFS_MSG_NONE		= UMMA_FS_MSG_START,
	eFS_MSG_UNMOUNT,
	eFS_MSG_SAFE_REMOVE,
	eFS_MSG_FORMAT,
	eFS_MSG_SEND_RESULT,
	eFS_MSG_CHECK_USB_SPEED_DONE,

	eFS_MSG_LAST

} UMMA_FS_MSG_e;


typedef enum
{
	eFORMAT_PARTITION_NONE						= 0,

	eFORMAT_PARTITION_SYSTEM					= 0x00000001,
	eFORMAT_PARTITION_PVR 						= 0x00000002,
	eFORMAT_PARTITION_IP 						= 0x00000004,
	eFORMAT_PARTITION_TVAPPS					= 0x00000008,

	eFORMAT_PARTITION_ALL						= 0xFFFFFFFF
} UMMA_FS_FormatPartition_e;


/** @brief file system type */
typedef enum
{
	eFS_FileSystem_NONE					= 0,
	eFS_FileSystem_EXT2					= 2,
	eFS_FileSystem_EXT3 					= 3,
	eFS_FileSystem_EXT4,
	eFS_FileSystem_XFS,
	eFS_FileSystem_FAT16,
	eFS_FileSystem_FAT32,
	eFS_FileSystem_NTFS,
	eFS_FileSystem_DOS,
	eFS_FileSystem_EXFAT,
	eFS_FileSystem_OTHER 					= 0xFF
} UMMA_FS_FsType_e;		//DxStorage_FsType_e �� �ݵ�� ���ƾ� �Ѵ�.
// Mount ����

typedef enum
{
	eFS_Dev_NULL 	= -1,
	eFS_Dev_sda		=  0,			/* might be internal SATA HDD */
	eFS_Dev_sdb,
	eFS_Dev_sdc,
	eFS_Dev_sdd,
	eFS_Dev_sde,
	eFS_Dev_sdf,
	eFS_Dev_sdg,
	eFS_Dev_sdh,
	eFS_Dev_sdi,

	eFS_Dev_mmc,					// MMC

	eFS_Dev_MAX
} UMMA_FS_DevIdx_e;

typedef enum
{
	eFS_DevType_None			= 0x00,
	eFS_DevType_IntHdd		= 0x01,				// Internal HDD : PVR model ��� ����� HDD
	eFS_DevType_Detachable		= 0x02,				// Detachable HDD : SATA interface�� Ż�� ������ HDD
	eFS_DevType_eSataHdd		= 0x03,				// eSATA HDD : eSATA interface�� ������ HDD
	eFS_DevType_UsbHdd		= 0x04,				// External USB HDD : USB�� �ɴ� ������ HDD
	eFS_DevType_UsbMem		= 0x05,				// USB Memory : HDD�� �ƴ� USB �޸�.  "USB�� ���� ��"�� ã������ ���� ExtHdd�� UsbMem ���� �� ã�ƾ� �Ѵ�.
	eFS_DevType_MMC			= 0x06,				// Multi Media Card Storage Device...
	eFS_DevType_MAX,
	eFS_DevType_ALL			= 0xFF
} UMMA_FS_DevType_e;

/** @brief usb device speed */
typedef enum
{
	eFS_DevUsbSpeed_NONE = 0,					// USB Speed NONE
	eFS_DevUsbSpeed_1M,						// USB Speed 1Mbps
	eFS_DevUsbSpeed_12M,						// USB Speed 12Mbps
	eFS_DevUsbSpeed_480M,						// USB Speed 480Mbps
	eFS_DevUsbSpeed_5G						// USB Speed 5G(5000M)bps
} UMMA_FS_DevUsbSpeed_e;

/** @brief result of HDD diagonostic self test */
typedef enum
{
	eFS_DST_HDD_NOT_DETECTED = 0,
	eFS_DST_FAILED_TRY_AGAIN_LATER,
	eFS_DST_IS_ON_GOING,
	eFS_DST_COMPLETED_OK,
	eFS_DST_COMPLETED_FATAL_ERROR,
	eFS_DST_COMPLETED_TEST_ELEMENT_FAILED,
	eFS_DST_COMPLETED_ELECTRICAL_ELEMENT_FAILED,
	eFS_DST_COMPLETED_SERVO_ELEMENT_FAILED,
	eFS_DST_COMPLETED_READ_ELEMENT_FAILED,
	eFS_DST_COMPLETED_HANDLING_DAMAGE,
	eFS_DST_COMPLETED_UNKNOWN
} UMMA_FS_DstResult_e;


typedef enum
{
	eFS_PvrEnableReason_Format,	 //format �� PVR HDD�� �����
	eFS_PvrEnableReason_User,	 //����ڰ� Pairing API�� ���ؼ� Enable ��Ŵ
	eFS_PvrEnableReason_Connection,	 //PVR �� HDD�� ����Ǿ���
	eFS_PvrEnableReason_UnKnown
}UMMA_FS_PvrEnableReason_e;


typedef enum
{
	eFS_UsageType_None		= 0,
	eFS_UsageType_Root,					// Mount �� root directory
	eFS_UsageType_Record,
	eFS_UsageType_Tsr,
	eFS_UsageType_MediaPath,
	eFS_UsageType_MediaVideo,
	eFS_UsageType_MediaAudio,
	eFS_UsageType_MediaPic,
	eFS_UsageType_MediaDownload,

	eFS_UsageType_END
} UMMA_FS_UsageType_e;

#define UMMA_FS_DEV_IDX_START				eFS_Dev_sda
#define UMMA_FS_DEV_IDX_END					(eFS_Dev_MAX - 1)

/** @brief HDD Status Type */
typedef enum
{
	eFS_DevStatus_NoDevice						= 0x0000,
	eFS_DevStatus_NotInitiated,
	eFS_DevStatus_Malfunction,
	eFS_DevStatus_Formatting,

	eFS_DevStatus_NeedFormat					= 0x0010,	// need the format (All the partition)
	eFS_DevStatus_SafeRemoved					= 0x0011,	// Unmounted(��������..) device for USB HDD

	eFS_DevStatus_OK							= 0x00FF
} UMMA_FS_DevStatus_e;

typedef enum
{
	eFS_DevSubStatus_DstMask					= 0x000F,
	eFS_DevSubStatus_DstNone					= 0x0000,
	eFS_DevSubStatus_DstOnGoing				= 0x0001,
	eFS_DevSubStatus_DstOk						= 0x0002,
	eFS_DevSubStatus_DstFail_Unknown			= 0x0003,
	eFS_DevSubStatus_DstFail_NoHdd				= 0x0004,
	eFS_DevSubStatus_DstFail_TryAgain 			= 0x0005,
	eFS_DevSubStatus_DstFail_FatalErr			= 0x0006,
	eFS_DevSubStatus_DstFail_TestElem			= 0x0007,
	eFS_DevSubStatus_DstFail_ElectricalElem	= 0x0008,
	eFS_DevSubStatus_DstFail_ServoElem			= 0x0009,
	eFS_DevSubStatus_DstFail_ReadElem			= 0x000A,
	eFS_DevSubStatus_DstFail_HadlingElem		= 0x000B,

	eFS_DevSubStatus_None						= 0x0000
} UMMA_FS_DevSubStatus_e;


// Internal HDD Partition define
typedef enum
{
	eFS_PvrPartitionType_None				= 0,

	// Normal PVR HDD profile:
	eFS_PvrPartitionType_System				= 0x00000001,
	eFS_PvrPartitionType_PvrMedia			= 0x00000002,			// Internal HDD���� PVR ����� �� ��Ƽ�ǿ��� �ݵ�� �� Type�� ��������� �Ѵ�.
	eFS_PvrPartitionType_WebBrowse			= 0x00000004,
	eFS_PvrPartitionType_TvApps				= 0x00000008,

	// External PVR HDD
	eFS_PvrPartitionType_ExtHdd				= 0x00000100,

	eFS_PvrPartitionType_All				= 0xFFFFFFFF
} UMMA_FS_PvrPartitionType_e;


typedef struct
{
	// Connect �� ���� MBR �� �޶����� �ʴ� �� �����Ǵ� argument
	// Mount/Unmount�� ���� �޶����� �ʴ´�.
	HBOOL					bAvailable;
	UMMA_FS_FsType_e		ePartFsType;							// Partition File System Type
	HUINT32					ulStartUnit, ulEndUnit;

	// Mount/Unmount ���¿� ���� �޶����� arguments
	// Mount/Unmount �ÿ� �� �������� ����ų� �����ؾ� �Ѵ�.
	HBOOL					bMounted;				// �ش� Partition�� Mount �Ǿ������� �˷��ִ� Flag
	HBOOL					bPvrFlag;
	HBOOL					bSystemFlag;
	HUINT32					ulPvrIdx;				// Pvr�� Storage ���� Index
	HUINT32					ulPlaybackIdx;			// Playback�� Storage ���� Index
	HBOOL					bDefaultPvr;			// Multi PVR HDD�� ���� default PVR HDD ���� ǥ��
	HBOOL					bWritable;
	HBOOL					bSecured;				// FALSE : Normal File system, TRUE : Encrypted File system // MOON_encr
	HUINT32 				 	ulDriveNo; 				// ���� Partition�� Drive No.
	HUINT64 				 	ullTotalSizeKb;
	HUINT8					szMountPath[UMMA_FS_MOUNT_PATH_LEN];
	HUINT8					szVolumeName[UMMA_FS_VOLUME_LABEL_SIZE];
	HUINT8					szRecordPath[UMMA_FS_MOUNT_PATH_LEN];
	HUINT8					szTsrPath[UMMA_FS_MOUNT_PATH_LEN];
	HUINT8					szMediaPath[UMMA_FS_MOUNT_PATH_LEN];
	HUINT8					szMediaVideo[UMMA_FS_MOUNT_PATH_LEN];
	HUINT8					szMediaAudio[UMMA_FS_MOUNT_PATH_LEN];
	HUINT8					szMediaPic[UMMA_FS_MOUNT_PATH_LEN];
	HUINT8					szMediaDownload[UMMA_FS_MOUNT_PATH_LEN];
} UMMA_FS_PartitionInfo_t;



typedef struct
{
	HBOOL					 bDevAvailable;			// Device �� ��ȿ�Ѱ��� Ȯ���ϴ� Flag

	HBOOL					 bDevConnected;			// Device �� ���������� ���� �ִ°��� ��Ÿ���� Flag
	HBOOL					 bDevMounted;			// Device �� �������� mount �Ǿ� �ִ°��� ��Ÿ���� Flag

	// Device ��ü�� property
	UMMA_FS_DevIdx_e			 eDevIdx;				// Device Index. ������ ��� ���� �̰� ����Ͽ� access�ؾ� �Ѵ�.
	UMMA_FS_DevType_e			 eDevType;
	UMMA_FS_DevStatus_e		 eDevStatus;				// Device Status
	UMMA_FS_DevSubStatus_e	 eDevSubStatus;			// DTS �� Sub Status
	HUINT32					 ulPortNum;
	UMMA_FS_DevUsbSpeed_e			eDevUsbSpeed;			// If Dev is usb type, get usb speed.(1M,12M,480M,5G)

	// Device �� ��ǥ�� partition�� property
	UMMA_FS_FsType_e		 eFsType;				// File System Type
	HBOOL					 bWriteProtected;		// FALSE : Write Enabled,  TRUE : Write Protected
	HUINT8					 szLabel[UMMA_FS_VOLUME_LABEL_SIZE];
	HUINT8					 szUUID[UMMA_FS_DEV_UUID_BUFSIZE];

	HUINT32					 ulNumPartition, ulMaxPartition;
	UMMA_FS_PartitionInfo_t	*pstPartArray;

	// size check�� �����ϴ� �Ϻδ� cache�� ���� �Ẹ��..
	HUINT32					 ulPartIdx;
	UNIXTIME				 utCheckTime;
	HBOOL					 bValidPvr;

	HUINT64					 ullDevSizeKb;			// device size�� runtime���� ����Ǵ� ���� �ƴϴ� �Ź� Ȯ���������� ����ص���
	// Ư�� ��ɿ��� ��Ͽ����� ����ϴ� ����
	HBOOL					 bIsExPvrModule;		// �� ��� Format �ϱ� ������ PVR�� ���� Device������ ����Ű�� Flag (EX-PVR : ���� PVR)
													// Format�� ���� ���� �̰��� ������ ������ Format �� ��ó���� ��������.
} UMMA_FS_DevContext_t;


typedef struct
{
	// ���� Device Index
	UMMA_FS_DevType_e			 eDevIdx;

	// Storage Unit (Cluster, Block, etc) ����
	HUINT32					 ulUnitSize;
	HUINT32					 ulTotalUnitNum;
	HUINT32					 ulPartitionNum;

	// Device Name
	HUINT8					 szDeviceName[UMMA_FS_DEVICE_PATH_LEN];

	// Partition Array
	UMMA_FS_PartitionInfo_t	 astPartArray[UMMA_FS_MAX_PARTITION];
} UMMA_FS_PartitionList_t;

/*
	Format�� ���� �䱸�ϴ� ����ü
	UMMA_FS_PartitionList_t �� ��������� ���� ȥ���ؼ��� �� �ȴ�!
*/
typedef struct
{
	HUINT32					ulPartIdx;

	HUINT32					ulStartUnit, ulEndUnit;
	UMMA_FS_FsType_e		ePartFsType;							// Partition File System Type
	UMMA_FS_PvrPartitionType_e 	ePartType;								// Partition Type
	HBOOL					bSecured;								// securiry file system MOON_encr
	HUINT8					szMountPath[UMMA_FS_MOUNT_PATH_LEN];
	HUINT8					szVolumeName[UMMA_FS_VOLUME_LABEL_SIZE];
} UMMA_FS_FormatPartInfo_t;

typedef struct
{
	UMMA_FS_DevIdx_e			 eDevIdx;

	HUINT64					 ullUnitSize;
	HUINT32 				 ulPartitionNum;

	// Partition Array
	UMMA_FS_FormatPartInfo_t	 astPartArray[UMMA_FS_MAX_PARTITION];
} UMMA_FS_FormatPartList_t;


/*
	AP���� M/W FS Device�� ���¸� �˱� ���� �ʿ��� structure.
	M/W �������� API �̿ܿ��� �� structure�� "�����" ������� ���� UMMA_FS_DevContext_t �� ����ϵ��� �Ѵ�.
*/
typedef struct
{
	HBOOL					bConnected;			// ���������� ����Ǿ����� ��Ÿ���� Flag
	HBOOL					bMounted;				// �������� mount �Ǿ����� ��Ÿ���� Flag

	UMMA_FS_DevIdx_e			eDevIdx;				// Device Index. ������ ��� ���� �̰� ����Ͽ� access�ؾ� �Ѵ�.
	UMMA_FS_DevType_e			eDevType;				// Device Type.
	UMMA_FS_DevStatus_e		 eDevStatus;			// Device Status
	UMMA_FS_DevSubStatus_e	 eDevSubStatus;			// Device Sub Status
	UMMA_FS_DevUsbSpeed_e			eDevUsbSpeed;			// If Dev is usb type, get usb speed.(1M,12M,480M,5G)
	UMMA_FS_FsType_e		eFsType;				// File System Type
	HBOOL					bWriteProtected;		// FALSE : Write Enabled,  TRUE : Write Protected
	HUINT64					ullTotalSizeKb;		// Device�� Total Size (KB ����)
	HUINT32					ulDriveNo;			// driver No
	HINT32					ulPortNum;

	HUINT8					szMountPath[UMMA_FS_MOUNT_PATH_LEN];
	HUINT8					szLabel[UMMA_FS_VOLUME_LABEL_SIZE];

	HUINT32					ulNumPartition;
} UMMA_FS_DevInfo_t;


typedef struct
{
	HBOOL				 bInitiated;

	HUINT32				 ulSemId;
	HUINT32				 ulTaskQueueId;
	HUINT32				 ulTaskId;

	// Multi PVR Storage ������ ���� Slot
	HBOOL				abConnectedPvrSlot[MAX_CONNECTABLE_PVR_STORAGE_NUM];		// ��� �� ����� PVR Storage slot (FALSE = �������)
	HBOOL				abConnectedPlaybackSlot[MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM];		// ��� �� ����� PVR Storage slot (FALSE = �������)
	HUINT32				ulTargetPvrIdx;		// ���� PVR �� �ڵ����� recording target storage�� ����

	UMMA_FS_DevContext_t		 astDevice[eFS_Dev_MAX];
} UMMA_FS_DevInfoMap_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/************************************************************
  Located in umma_fs_device.c
*************************************************************/
HERROR umma_fs_svc_GetDeviceInfo (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_DevInfo_t *pstMwDevInfo);
void	umma_fs_svc_registerNotifier(UxSendEvtCallback_t pfCb);
void	umma_fs_svc_registerDataNotifier(UxSendDataCallback_t pfCb);


HERROR umma_fs_svc_GetPartitionInfoArray (UMMA_FS_DevIdx_e eDevIdx, HUINT32 *pulPartNum, UMMA_FS_PartitionInfo_t **ppstPartArray);
HERROR umma_fs_svc_SendEventToMainloop(HINT32 nEvt, HINT32 nP1, HINT32 nP2, HINT32 nP3);
HERROR umma_fs_svc_SendDataToMainloop (HINT32 nEvt, void *nP1, HINT32 nP1Size, void *nP2, HINT32 nP2Size, HINT32 nP3);

HERROR				umma_fs_dev_Init (void);
UMMA_FS_DevInfoMap_t*	umma_fs_dev_GetContext (void);
UMMA_FS_DevIdx_e		umma_fs_dev_ConvertPalDevIdxToMwDevIdx (PAL_FS_StorDevIdx_t ePalIdx);
PAL_FS_StorDevIdx_t	umma_fs_dev_ConvertMwDevIdxToPalDevIdx (UMMA_FS_DevIdx_e eMwIdx);
UMMA_FS_FsType_e	umma_fs_dev_ConvertPalFsTypeToMwFsType (PAL_FS_FsType_t ePalFsType);
PAL_FS_FsType_t		umma_fs_dev_ConvertMwFsTypeToPalFsType (UMMA_FS_FsType_e eMwFsType);
HBOOL				umma_fs_dev_IsFsTypeWritable (UMMA_FS_FsType_e eFsType);
HERROR				umma_fs_dev_AppendSeparatorToPath (HUINT8 *szPath);
HERROR				umma_fs_dev_RemoveSeparatorFromPath (HUINT8 *szPath);
HERROR				umma_fs_dev_MakeMountPathByDriveNo (HUINT32 ulDriveNo, HUINT8 *szPath);
HERROR				umma_fs_dev_GetDriveNoFromMountPath (HUINT8 *szPath, HUINT32 *pulDriveNo);
HERROR				umma_fs_dev_InitFsDeviceInfo (UMMA_FS_DevContext_t *pstDevice);
HERROR				umma_fs_dev_ClearMountInfoInFsDeviceInfo (UMMA_FS_DevContext_t *pstDevice);
HERROR				umma_fs_dev_FindEmptyDeviceInfoSlot (UMMA_FS_DevContext_t **ppstDevice);
HERROR				umma_fs_dev_GetNewDriveNo (HUINT32 *pulNewDriveNo);
HBOOL 				umma_fs_dev_IsDeviceExist (UMMA_FS_DevIdx_e eDevIdx);
HERROR				umma_fs_dev_GetDeviceInfo (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_DevContext_t **ppstDevice);
HERROR				umma_fs_dev_GetDeviceIndexArray (HUINT32 *pulDeviceNum, UMMA_FS_DevIdx_e **ppeIdxArray);
HERROR				umma_fs_dev_FreeDeviceIndexArray (UMMA_FS_DevIdx_e *peIdxArray);
HUINT32				umma_fs_dev_CountMountedDevice (UMMA_FS_DevType_e eTypeFlag);
HERROR				umma_fs_dev_GetDrivePath (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szDrivePath);
HBOOL				umma_fs_dev_CheckPathIsValid (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szPath);
HERROR				umma_fs_dev_GetDeviceUuid (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szUuid);
HERROR 				umma_fs_dev_GetDeviceUuidByPvrIdx (HUINT32 ulPvrIdx, HUINT8 *szUuid);
HERROR 				umma_fs_dev_GetDeviceFileFullName (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szFileName);

HERROR				umma_fs_dev_GetDeviceIdByDriveNo (HUINT32 ulDriveNo, UMMA_FS_DevIdx_e *peDevIdx);
HERROR				umma_fs_dev_ConnectNewDevice (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_DevContext_t **ppstDevice);

HERROR				umma_fs_dev_GetDevPartIndexByPath (HUINT8 *szDrivePath, UMMA_FS_DevIdx_e *peDevIdx, HUINT32 *pulPartIdx);
HERROR				umma_fs_dev_GetPartitionListFromPal (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_PartitionList_t *pstMwPartList);
HERROR				umma_fs_dev_ReinitPartitionInfo (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_FormatPartList_t *pstFormatList, HBOOL bTryFormat); // MOON_encr
HERROR				umma_fs_dev_MountPartition (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx, HBOOL bSecured, HUINT8 *szMountPath, UMMA_FS_PartitionInfo_t **ppstDstPartInfo);
HERROR				umma_fs_dev_UnmountPartition (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx);
HERROR				umma_fs_dev_UnmountDevice (UMMA_FS_DevIdx_e eDevIdx);
HERROR				umma_fs_dev_GetMountedPartitionStatus (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx,
													HUINT64 *pullTotalKbSize, HUINT64 *pullUsedKbSize, HBOOL *pbWritable);
HERROR				umma_fs_dev_GetDirectoryStatus (HUINT8 *szDirPath, HUINT64 *pullTotalKbSize, HUINT64 *pullUsedKbSize);
HERROR				umma_fs_dev_GetPhysicalDeviceSize (UMMA_FS_DevIdx_e eDevIdx, HUINT64 *pullDevSizeKb);
HERROR 				umma_fs_dev_GetDevIdxByDevUuid(UMMA_FS_DevIdx_e *pulDevIdx, HUINT8 *szUuid);
HERROR				umma_fs_dev_SetDeviceVolumeName(UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx, HUINT8 *szName);
HERROR				umma_fs_dev_GetUuidByPath(HUINT8 *pszPath, HUINT8 *pszUuid);
HERROR 				umma_fs_dev_GetPhysicalMMCDeviceSize(UMMA_FS_DevIdx_e eDevIdx, HUINT64 *pullDevSizeKb,HUINT64 *pullUsedSizeKb,HUINT64 *pullAvailSizeKb);
HERROR				umma_fs_dev_GetDevSize(HCHAR *pszDevName, HUINT64 *pullTotalKb);
HERROR				umma_fs_dev_GetPathSize(HCHAR *pszPathName, HUINT64 *pullSizeKb);
HERROR 				umma_fs_dev_EraseAllDeviceInfo(void);


/************************************************************
  Located in umma_fs_hdd.c
*************************************************************/
HERROR 	umma_fs_hdd_PowerOn(UMMA_FS_DevType_e eType);
HERROR 	umma_fs_hdd_PowerOff(UMMA_FS_DevType_e eType);
HERROR	umma_fs_hdd_Init(void);
HERROR	umma_fs_hdd_AddedPostProcess(UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPortNum);
HERROR	umma_fs_hdd_RemovedPostProcess(UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPortNum);
HERROR	umma_fs_hdd_MountAndCheckIntHdd (UMMA_FS_DevIdx_e eDevIdx,HBOOL bAfterFormat);

/************************************************************
  Located in umma_fs_usb.c
*************************************************************/
HERROR				umma_fs_usb_Init (void);
HERROR				umma_fs_usb_MountDevice (UMMA_FS_DevIdx_e eDevIdx, HBOOL bAfterFormat);
HERROR				umma_fs_usb_MountAllDevice (void);
HERROR 				umma_fs_usb_UnmountDevice (UMMA_FS_DevIdx_e eDevIdx, HBOOL bSafeRemove);
HERROR				umma_fs_usb_UnmountDeviceAll (void);
HERROR				umma_fs_usb_PowerOff (void);
HERROR				umma_fs_usb_PowerOn (void);
HERROR 			umma_fs_usb_AddedPostProcess(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
HERROR 			umma_fs_usb_RemovedPostProcess(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);


/************************************************************
  Located in umma_fs_mmc.c
*************************************************************/
HERROR				umma_fs_mmc_Init (void);
HERROR 				umma_fs_mmc_MountDevice (void);
HERROR 				umma_fs_mmc_UnMountDevice (UMMA_FS_DevIdx_e eDevIdx, HBOOL bSafeRemove);
HERROR				umma_fs_mmc_Format(void);
HERROR 		umma_fs_mmc_PowerOn (void);
HERROR 		umma_fs_mmc_PowerOff (void);


/************************************************************
  Located in umma_fs_flash.c
*************************************************************/
HERROR umma_fs_flash_MountPartition(void);
HERROR umma_fs_flash_UnmountPartition(void);


/************************************************************
  Located in fs_pvr.c
*************************************************************/

HERROR				umma_fs_svc_PvrInit (void);
UMMA_FS_DevStatus_e	umma_fs_svc_PvrGetPvrHddStatus (HUINT32 ulPvrIdx);
HERROR				umma_fs_svc_PvrFormatIntPvrHdd (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName);
HERROR				umma_fs_svc_PvrFormatExtPvrHdd (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName);
HERROR				umma_fs_svc_PvrFormatIntHddPartition (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_PvrPartitionType_e ePartType, HUINT8 *szVolumeName);
HERROR				umma_fs_svc_PvrUnmountIntPvrHdd (UMMA_FS_DevIdx_e eDevIdx, HBOOL bSafeRemove);
HERROR				umma_fs_svc_PvrMakePartition (UMMA_FS_FormatPartList_t *pstFormatList);
HERROR				umma_fs_svc_PvrMountDevice(UMMA_FS_DevIdx_e eDevIdx);
HERROR 				umma_fs_svc_PvrFindPvrDevIdPartIdx (HUINT32 ulPvrIdx, UMMA_FS_DevIdx_e *peDevIdx, HUINT32 *pulPartIdx);
HERROR				umma_fs_svc_PvrFindPvrPossiblePartitionInDevice (UMMA_FS_DevIdx_e eDevIdx, HUINT32 *pulPartIdx);
HERROR				umma_fs_svc_PvrGetPvrPossibleDeviceIndexArray (HUINT32 *pulDevNum, UMMA_FS_DevIdx_e **ppeDevIdxArray);
HERROR 				umma_fs_svc_GetReservedSizeKb(UMMA_FS_DevType_e	 eDevType, HUINT64 ullTotalSizeKb, HUINT64 *pullReservedSizeKb);

HERROR 				umma_fs_svc_PvrGetPvrPath (HUINT32 ulPvrIdx, UMMA_FS_UsageType_e eUsage, HCHAR *szPath);
HERROR				umma_fs_svc_PvrReleasePvrIdx(HUINT32 ulPvrIdx);
HERROR				umma_fs_svc_PvrReleasePlaybackIdx (HUINT32 ulPlaybackIdx);
HERROR				umma_fs_svc_PvrRegisterPvrHdd (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx);
HERROR				umma_fs_svc_PvrCheckRegisteredPvr (UMMA_FS_DevIdx_e eDevIdx);
HERROR				umma_fs_svc_PvrUnregisterExtPvr (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx);
HERROR				umma_fs_svc_PvrDisableExtPvrHdd(HUINT32	ulPvrIdx);
HERROR 				umma_fs_svc_PvrUnRegisterExtPvrHdd (UMMA_FS_DevIdx_e eDevIdx);

HERROR				umma_fs_svc_PvrSwitchExtPvrHddDev (UMMA_FS_DevIdx_e eDevIdx);
HERROR				umma_fs_svc_PvrMakePlaybackInfo (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx);
HERROR				umma_fs_svc_PvrCheckDeviceIsExtPvrPaired (UMMA_FS_DevIdx_e eDevIdx, HBOOL bAfterFormat, HUINT32 ulPartIdx);
HERROR 				umma_fs_svc_PvrGetStbUniqueInfo(HUINT8 *pszInfo);
HERROR				umma_fs_svc_PvrSetPvrDefaultHdd(HUINT32 ulPvrIdx, HBOOL bSync);
HERROR 				umma_fs_svc_PvrSelectNewDefaultPvrIdx(HUINT32 *pulPvrIdx);
HBOOL 				umma_fs_svc_PvrCheckDeviceIsDefaultPvrHdd (UMMA_FS_DevIdx_e eDevIdx);
HUINT32 			umma_fs_svc_PvrGetDefaultPvrIdx(void);
#if 0
#if defined(CONFIG_PROD_DEVICE_2ND_INTERNAL_HDD)
HUINT32				umma_fs_svc_PvrGetTargetPvrIdx(void);
HERROR				umma_fs_svc_PvrSetTargetPvrIdx(HUINT32 ulPvrIdx);
#endif
#endif
HERROR				umma_fs_svc_PvrGetAllPvrPath(UMMA_FS_DevType_e eDevType,UMMA_FS_PartitionInfo_t *PartitionInfo );
HERROR				umma_fs_svc_PvrGetPlaybackPvrPath(UMMA_FS_DevType_e eDevType,UMMA_FS_PartitionInfo_t *PartitionInfo );

HERROR				umma_fs_svc_PvrStartDst (UMMA_FS_DevIdx_e eDevIdx);
HERROR 				umma_fs_svc_PvrGetPvrIdxByDevUuid(HUINT32 *pulPvrIdx, HUINT8 *szUuid);
HERROR 				umma_fs_svc_PvrGetPvrIdxByDevIdx(HUINT32 *pulPvrIdx, UMMA_FS_DevIdx_e eDevIdx);
HERROR				umma_fs_svc_PvrGetPlaybackIdxByDevIdx(HUINT32 *pulPlaybackIdx, UMMA_FS_DevIdx_e eDevIdx);
HBOOL 				umma_fs_svc_PvrIsPvrStorageRegistrable (void);
HBOOL				umma_fs_svc_PvrIsPlaybackStorageRegistrable (void);
HERROR				umma_fs_svc_PvrAssignPvrIdx (HUINT32 *pulPvrIdx);
HERROR				umma_fs_svc_PvrAssignPlaybackIdx (HUINT32 * pulPlaybackIdx);
HBOOL				umma_fs_svc_PvrIsDevicePvrUsed (UMMA_FS_DevIdx_e eDevIdx);
HBOOL				umma_fs_svc_PvrIsPVRAvailable (void);
HERROR				umma_fs_svc_PvrGetConnectedPvrStorageCount (HUINT32 *pulPvrCnt);
HERROR 			umma_fs_svc_WritePvrRegisterInfoToHDD(UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx);
HERROR				umma_fs_svc_PvrGetDefaultHddInfo(HUINT8 *szUuid, HUINT32 *ulMaxRec);

HBOOL				umma_fs_hdd_CheckIsNewDetachableHDD(void);
HERROR				umma_fs_hdd_SetNewDetachableHDD(HBOOL bFlag);

/************************************************************
  Located in fs_dirpath.c
*************************************************************/
HERROR				umma_fs_svc_DirGetHddPartitionSetting (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName, UMMA_FS_FormatPartList_t *pstFormatList);
HERROR				umma_fs_svc_DirGetPathFromMountedRootPath (HUINT8 *szMountedRoot, UMMA_FS_DevType_e eDevType, UMMA_FS_UsageType_e eUsage, HUINT8 *szPath, HUINT32 ulPathMax);
HERROR 				umma_fs_svc_DirMakeDefaultDirectory (UMMA_FS_DevIdx_e	eDevIdx, HUINT32	 ulPartIdx);
HERROR				umma_fs_svc_DirMakePlaybackDirectory (UMMA_FS_DevIdx_e	eDevIdx, HUINT32 ulPartIdx);
void				umma_fs_svc_DirMakeDirectoryByUsage (UMMA_FS_UsageType_e eDirUsageType, UMMA_FS_DevContext_t *pstDevice, HUINT32 ulPartIdx);
HERROR 				umma_fs_svc_DirGetRecordDirName (HCHAR *pszDir, HUINT32 ulSize, HBOOL bInternal);

HERROR				umma_fs_svc_GetHddSizeCheckInterval(void);


/************************************************************
  Located in umma_fs_config.c
*************************************************************/
HERROR			umma_fs_config_Init(void);
HUINT32 umma_fs_config_GetSupportedDeviceList(UMMA_FS_DevType_e *peList);
UMMA_FS_FsType_e	umma_fs_config_GetDefaultFS(UMMA_FS_DevType_e eDevType);
UMMA_FS_PvrType_e 	umma_fs_config_GetPvrType(UMMA_FS_DevType_e eDevType);
void 			umma_fs_config_Print(void);


#endif /* !__INT_FS_H__ */
