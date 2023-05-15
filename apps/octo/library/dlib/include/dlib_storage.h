/* $Header: $ */

#ifndef DLIB_STORAGE_H__
#define DLIB_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	DxSTORAGE_NAME_LEN		64
#define	DxSTORAGE_PARTITION_NUM	8
#define	DxSTORAGE_LABEL_SIZE		16+1

#define	DxSTORAGE_CHECK_FLAG(flag, type)		(flag&type)? #type" : TRUE":#type" : FALSE"
#define	DxSTORAGE_INVALID_PVR_INDEX (0xffff)
typedef enum
{
	eDxSTORAGE_NOTI_Unknown = 0,
	eDxSTORAGE_NOTI_DevChange,			// Connect or Disconnect device
	eDxSTORAGE_NOTI_NeedFormat,
	eDxSTORAGE_NOTI_OverCurrent,
	eDxSTORAGE_NOTI_FormatState,
	eDxSTORAGE_NOTI_DstState,
	eDxSTORAGE_NOTI_PvrReady,		// device의 연결/해제, 혹은 Format으로 인해 PVR device 정보가 바뀌었을때 발생
	eDxSTORAGE_NOTI_PlaybackReady,
	eDxSTORAGE_NOTI_UnmountFail, 	//HDD 제거시 Unmount 실패했을때 올려주는 이벤트
	eDxSTORAGE_NOTI_CheckDiskStarted,	//Check disk 시작을 알림
	eDxSTORAGE_NOTI_CheckUsbSpeed,		// USB 속도 채크 결과를 알림
	eDxSTORAGE_NOTI_Result,		 //API Call에 대한 결과를 리턴
	eDxSTORAGE_NOTI_DefaultHddInfoChanged,
	eDxSTORAGE_NOTI_Init
} DxStorage_NotiType_e;

typedef enum
{
	eDxSTORAGE_PROGRESS_Unknown = 0,
	eDxSTORAGE_PROGRESS_Format,
	eDxSTORAGE_PROGRESS_Dst,
} DxStorage_ProgressType_e;

// eDxSTORAGE_NOTI_Result 에 대한 API type
typedef enum
{
	eDxSTORAGE_RESULT_Format = 0,
	eDxSTORAGE_RESULT_Dst,
	eDxSTORAGE_RESULT_SetDefaultHdd,
	eDxSTORAGE_RESULT_SetName,
	eDxSTORAGE_RESULT_DisableHdd,
	eDxSTORAGE_RESULT_Pairing,
	eDxSTORAGE_RESULT_UnPairing,
	eDxSTORAGE_RESULT_SafeRemove,
	eDxSTORAGE_RESULT_CheckUsbSpeed
}DxStorage_ResultType_e;

typedef enum
{
	// 1st Nibble
	eDxSTORAGE_FILTER_TypeAll		= 0x0000000F,
	eDxSTORAGE_FILTER_Internal		= 0x00000001,
	eDxSTORAGE_FILTER_External		= 0x00000002,
	// 2nd Nibble
	eDxSTORAGE_FILTER_ConnectionAll	= 0x000000F0,
	eDxSTORAGE_FILTER_Connected		= 0x00000010,
	eDxSTORAGE_FILTER_Disconnected	= 0x00000020,
	// 3rd Nibble
	eDxSTORAGE_FILTER_MountAll		= 0x00000F00,
	eDxSTORAGE_FILTER_Mounted		= 0x00000100,
	eDxSTORAGE_FILTER_Unmounted		= 0x00000200,
	// 4th Nibble
	eDxSTORAGE_FILTER_UsageAll		= 0x0000F000,
	eDxSTORAGE_FILTER_Storage		= 0x00001000,
	eDxSTORAGE_FILTER_PVR			= 0x00002000,
	eDxSTORAGE_FILTER_TSR			= 0x00004000,
	eDxSTORAGE_FILTER_Playback		= 0x00008000,

	// 5th Nibble
	eDxSTORAGE_FILTER_KindAll		= 0x000F0000,
	eDxSTORAGE_FILTER_Hdd		= 0x00010000,		// eDxSTORAGE_DEV_KIND_HDD
	eDxSTORAGE_FILTER_Mem		= 0x00020000,			// eDxSTORAGE_DEV_KIND_USBMEM, eDxSTORAGE_DEV_KIND_SDMEM

	eDxSTORAGE_FILTER_All			= 0xFFFFFFFF,
	eDxSTORAGE_FILTER_None			= 0x00000000
} DxStorage_LIST_Filter_e;

typedef enum
{
	eDxSTORAGE_FS_NONE					= 0,
	eDxSTORAGE_FS_EXT2					= 2,
	eDxSTORAGE_FS_EXT3					= 3,
	eDxSTORAGE_FS_EXT4,
	eDxSTORAGE_FS_XFS,
	eDxSTORAGE_FS_FAT16,
	eDxSTORAGE_FS_FAT32,
	eDxSTORAGE_FS_NTFS,
	eDxSTORAGE_FS_DOS,
	eDxSTORAGE_FS_EXFAT,
	eDxSTORAGE_FS_OTHER 				= 0xFF
} DxStorage_FsType_e;		//SvcFs_FileSystemType_e 와 반드시 같아야한다.


typedef enum
{
	eDxSTORAGE_DEV_STATUS_DstResultUnknown	= 0,
	eDxSTORAGE_DEV_STATUS_DstResultOk,
	eDxSTORAGE_DEV_STATUS_DstResultOnGoing,
	eDxSTORAGE_DEV_STATUS_DstResultFailNoHDD,
	eDxSTORAGE_DEV_STATUS_DstResultFailTryAgain,
	eDxSTORAGE_DEV_STATUS_DstResultFailFatalError,
	eDxSTORAGE_DEV_STATUS_DstResultFailTestElementFailed,
	eDxSTORAGE_DEV_STATUS_DstResultFailElectricalElementFailed,
	eDxSTORAGE_DEV_STATUS_DstResultFailServoElementFailed,
	eDxSTORAGE_DEV_STATUS_DstResultFailReadElementFailed,
	eDxSTORAGE_DEV_STATUS_DstResultFailHadlingDamange,
	eDxSTORAGE_DEV_STATUS_DstResultFailUnknown
} DxStorage_STATUS_DST_Result_e;

typedef enum
{
	eDxSTORAGE_DEV_STATUS_FormatUnknown	= 0,
	eDxSTORAGE_DEV_STATUS_NeedFormat,
	eDxSTORAGE_DEV_STATUS_Formatting,
	eDxSTORAGE_DEV_STATUS_FormatDone,
	eDxSTORAGE_DEV_STATUS_FormatFail
} DxStorage_STATUS_Format_e;


/* SQC Mass Storage */

enum
{
	eDxSTORAGE_FLAG_NONE					= 0x00000000,
	eDxSTORAGE_FLAG_CONNECTED				= 0x00000001,
	eDxSTORAGE_FLAG_MOUNTED					= 0x00000002,
	eDxSTORAGE_FLAG_WRITEPROTECTED			= 0x00000004,
	eDxSTORAGE_FLAG_SECURED					= 0x00000008,
	eDxSTORAGE_FLAG_INTERNAL				= 0x00010000,
	eDxSTORAGE_FLAG_PVRDEVICE				= 0x00020000,
	eDxSTORAGE_FLAG_NEEDFORMAT				= 0x00040000,
	eDxSTORAGE_FLAG_PAIRED					= 0x00080000,
	eDxSTORAGE_FLAG_DEFAULTHDD				= 0x00100000,
	eDxSTORAGE_FLAG_SUBPVRDEVICE			= 0x00200000,
	eDxSTORAGE_FLAG_NEW2NDHDD				= 0x00400000,	// 처음 연결된 E-SATA(Detachable) HDD 인 경우
	eDxSTORAGE_FLAG_TSRDEVICE				= 0x00800000,
	eDxSTORAGE_FLAG_RECLIMIT				= 0x01000000,	// Rec Instance Limit (not using Max Rec Instance)
	eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE		= 0x02000000,	// recording list만 가질 수 있는 type ( not pvr )
	eDxSTORAGE_FLAG_SYSTEM					= 0x04000000,
};


typedef enum
{
	eDxSTORAGE_DEVTYPE_NONE	= 0,
	eDxSTORAGE_DEVTYPE_INTHDD,
	eDxSTORAGE_DEVTYPE_DETACHABLE,
	eDxSTORAGE_DEVTYPE_ESATAHDD,
	eDxSTORAGE_DEVTYPE_USBHDD,
	eDxSTORAGE_DEVTYPE_USBMEM,
	eDxSTORAGE_DEVTYPE_MMC,
	eDxSTORAGE_DEVTYPE_ALL			= 0xFF
} DxStorage_DevType_e;

typedef void (*DxStorageNotifier_t)(DxStorage_NotiType_e eType, HINT32	p1, HINT32 p2, HINT32 p3);

typedef struct
{
	DxStorage_ProgressType_e		eType;
	HUINT32						ulDeviceId;
	HUINT32						ulTotal;
	HUINT32						ulCurrent;
} DxStorage_Progress_t;

typedef struct
{
	HUINT64				ullTotalSizeKb;
	HUINT64				ullAvailableSizeKb;
	HUINT64				ullUsedSizeKb;
	HUINT64				ullReservedSizeKb;
	HUINT64				ullAvailablePvrSizeKb;
} DxStorage_Size_t;

typedef struct
{
	HBOOL						bAvailable;
	HUINT32						ulStartUnit, ulEndUnit;
	HINT32						nPvrIdx;
	HINT32						nPlaybackIdx;
	HUINT32						ulFlags;			//	MOUNT/PVRDEVICE/NEEDFORMAT/...
	DxStorage_FsType_e			eFsType;
	HUINT64						ullPartitionSizeKb;
	HCHAR						szMountPath[DxSTORAGE_NAME_LEN];
	HCHAR						szVolumeName[DxSTORAGE_NAME_LEN];
	HCHAR						szTsrPath[DxSTORAGE_NAME_LEN];	//size 얻기위해 필요함
	HCHAR						szRecordPath[DxSTORAGE_NAME_LEN];
	HCHAR						szMediaPath[DxSTORAGE_NAME_LEN];
	HCHAR						szMediaVideo[DxSTORAGE_NAME_LEN];
	HCHAR						szMediaAudio[DxSTORAGE_NAME_LEN];
	HCHAR						szMediaPic[DxSTORAGE_NAME_LEN];
	HCHAR						szMediaDownload[DxSTORAGE_NAME_LEN];
} DxStorage_PartitionInfo_t;

typedef struct
{
	HUINT32		ulDeviceId;
	HUINT32		ulFlags;							//	eDxSTORAGE_FLAG_NONE, ....
	HUINT32		ulPortNum;
	HUINT64		ullTotalSizeKb;
	HUINT64		ullReservedSizeKb;		// TSR Reserved size
	HUINT64		ullDeviceWriteSpeedBps;				// R/W speed Byte per second
	DxStorage_DevType_e		eDevType;

	HCHAR		szUUID[DxSTORAGE_NAME_LEN];
	HCHAR		szLabel[DxSTORAGE_LABEL_SIZE];
	HUINT8		szMountPath[DxSTORAGE_NAME_LEN];
	HUINT8		szDevName[DxSTORAGE_NAME_LEN];	// ex: "dev/sda" ... size 얻기위해 필요함

	HINT32		nPartitionNum;
	DxStorage_PartitionInfo_t	astPartitionInfos[DxSTORAGE_PARTITION_NUM];
	HUINT32		ulMaxRecInstance;
} DxStorage_Info_t;

#ifdef __cplusplus
}
#endif

#endif // DLIB_STORAGE_H__

