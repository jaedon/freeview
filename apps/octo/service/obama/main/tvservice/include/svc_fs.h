/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 	svc_fs.h
	@brief		PVR common definition

	Description: 	defines common APIs for PVR operations.\n
	Module: 		mw/pvr			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_FS_H__
#define	__SVC_FS_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define SVC_FS_MOUNT_PATH_LEN				64
#define SVC_FS_PVR_DIR_PATH_LEN				64
#define SVC_FS_VOLUME_LABEL_SIZE			32+1	// 16+ NULL
#define SVC_FS_DEV_UUID_BUFSIZE				(VOLUUIDSZ+1) 		// pal_fs.h의 VOLUUIDSZ는 36 + null char 해서 37 이상 있어야 한다.
#define SVC_FS_MAX_PARTITION				12		// PAL_FS_MAX_PARTITIONS

#define SVC_FS_USB_PVR_PART_IDX				0	// USB HDD의 PVR용 partition index (참고: 현재 USB HDD에 대해서는 1개의 Partition만 허용하도록 구현되어있다.)
#define MAX_CONNECTABLE_PVR_STORAGE_NUM		CONFIG_MW_MM_MAX_CONNECTABLE_PVR_STORAGE_NUM	// 동시에 연결하여 이용가능한 최대 PVR Storage 개수
#if defined CONFIG_MW_MM_MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM
#define MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM		CONFIG_MW_MM_MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM	// 동시에 연결하여 이용가능한 최대 Playback Storage 개수
#else
#define MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM		1
#endif



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

// Mount 관련
typedef enum
{
	eSVC_FS_DEV_NULL 	= -1,
	eSVC_FS_DEV_sda		=  0,			/* might be internal SATA HDD */
	eSVC_FS_DEV_sdb,
	eSVC_FS_DEV_sdc,
	eSVC_FS_DEV_sdd,
	eSVC_FS_DEV_sde,
	eSVC_FS_DEV_sdf,
	eSVC_FS_DEV_sdg,
	eSVC_FS_DEV_sdh,
	eSVC_FS_DEV_sdi,

	eSVC_FS_DEV_mmc,					// MMC

	eSVC_FS_DEV_MAX
} SvcFs_DevIdx_e;

typedef enum
{
	eSVC_FS_DevType_None			= 0x00,
	eSVC_FS_DevType_IntHdd		= 0x01,				// Internal HDD : PVR model 등에서 내장된 HDD
	eSVC_FS_DevType_Detachable		= 0x02,				// Detachable HDD : SATA interface로 탈착 가능한 HDD
	eSVC_FS_DevType_eSataHdd		= 0x03,				// eSATA HDD : eSATA interface의 외장형 HDD
	eSVC_FS_DevType_UsbHdd		= 0x04,				// External USB HDD : USB로 꽃는 외장형 HDD
	eSVC_FS_DevType_UsbMem		= 0x05,				// USB Memory : HDD가 아닌 USB 메모리.  "USB로 꽃인 것"을 찾으려면 위의 ExtHdd와 UsbMem 둘을 다 찾아야 한다.
	eSVC_FS_DevType_MMC			= 0x06,				// Multi Media Card Storage Device...
	eSVC_FS_DevType_MAX,
	eSVC_FS_DevType_ALL			= 0xFF
} SvcFs_DevType_e;


/** @brief file system type */
typedef enum
{
	eFileSystem_NONE			= 0,
	eFileSystem_EXT2			= 2,
	eFileSystem_EXT3 			= 3,
	eFileSystem_EXT4,
	eFileSystem_XFS,
	eFileSystem_FAT16,
	eFileSystem_FAT32,
	eFileSystem_NTFS,
	eFileSystem_DOS,
	eFileSystem_OTHER 			= 0xFF
} SvcFs_FileSystemType_e;
//DxStorage_FsType_e 와 반드시 같아야 한다.

typedef enum
{
	eDevStatus_NoDevice						= 0x0000,
	eDevStatus_NotInitiated,
	eDevStatus_Malfunction,
	eDevStatus_Formatting,

	eDevStatus_NeedFormat					= 0x0010,	// need the format (All the partition)
	eDevStatus_SafeRemoved					= 0x0011,	// Unmounted(안전제거..) device for USB HDD

	eDevStatus_OK							= 0x00FF
} SvcFs_DevStatus_e;

typedef enum
{
	eDevSubStatus_DstMask					= 0x000F,
	eDevSubStatus_DstNone					= 0x0000,
	eDevSubStatus_DstOnGoing				= 0x0001,
	eDevSubStatus_DstOk						= 0x0002,
	eDevSubStatus_DstFail_Unknown			= 0x0003,
	eDevSubStatus_DstFail_NoHdd				= 0x0004,
	eDevSubStatus_DstFail_TryAgain 			= 0x0005,
	eDevSubStatus_DstFail_FatalErr			= 0x0006,
	eDevSubStatus_DstFail_TestElem			= 0x0007,
	eDevSubStatus_DstFail_ElectricalElem	= 0x0008,
	eDevSubStatus_DstFail_ServoElem			= 0x0009,
	eDevSubStatus_DstFail_ReadElem			= 0x000A,
	eDevSubStatus_DstFail_HadlingElem		= 0x000B,

	eDevSubStatus_None						= 0x0000
} SvcFs_DevSubStatus_e;

typedef enum
{
	eSVC_FS_UsageType_None		= 0,
	eSVC_FS_UsageType_Root,					// Mount 된 root directory
	eSVC_FS_UsageType_Record,
	eSVC_FS_UsageType_Tsr,
	eSVC_FS_UsageType_MediaPath,
	eSVC_FS_UsageType_MediaVideo,
	eSVC_FS_UsageType_MediaAudio,
	eSVC_FS_UsageType_MediaPic,
	eSVC_FS_UsageType_MediaDownload,

	eSVC_FS_UsageType_END
} SvcFs_UsageType_e;

/*
	AP에서 M/W FS Device의 상태를 알기 위해 필요한 structure.
	M/W 내에서는 API 이외에는 이 structure를 "절대로" 사용하지 말고 SvcFs_DevContext_t 를 사용하도록 한다.
*/

typedef struct
{
	// Connect 된 이후 MBR 이 달라지지 않는 한 고정되는 argument
	// Mount/Unmount에 의해 달라지지 않는다.
	HBOOL					bAvailable;
	SvcFs_FileSystemType_e	ePartFsType;							// Partition File System Type
	HUINT32					ulStartUnit, ulEndUnit;

	// Mount/Unmount 상태에 의해 달라지는 arguments
	// Mount/Unmount 시에 이 정보만을 지우거나 수정해야 한다.
	HBOOL					bMounted;				// 해당 Partition이 Mount 되었는지를 알려주는 Flag
	HBOOL					bPvrFlag;
	HUINT32					ulPvrIdx;				// Pvr용 Storage 구분 Index
	HUINT32					ulPlaybackIdx;			// Playback용 Storage 구분 Index
	HBOOL					bDefaultPvr;			// Multi PVR HDD를 위한 default PVR HDD 여부 표시
	HBOOL					bWritable;
	HBOOL					bSecured;				// FALSE : Normal File system, TRUE : Encrypted File system // MOON_encr
	HUINT32 				 	ulDriveNo; 				// 현재 Partition의 Drive No.
	HUINT64 				 	ullTotalSizeKb;
	HUINT8					szMountPath[SVC_FS_MOUNT_PATH_LEN];
	HUINT8					szVolumeName[SVC_FS_VOLUME_LABEL_SIZE];
	HUINT8					szRecordPath[SVC_FS_MOUNT_PATH_LEN];
	HUINT8					szTsrPath[SVC_FS_MOUNT_PATH_LEN];
	HUINT8					szMediaVideo[SVC_FS_MOUNT_PATH_LEN];
	HUINT8					szMediaAudio[SVC_FS_MOUNT_PATH_LEN];
	HUINT8					szMediaPic[SVC_FS_MOUNT_PATH_LEN];
} SvcFs_PartitionInfo_t;

typedef struct
{
	HBOOL					 bConnected;			// 물리적으로 연결되었음을 나타내는 Flag
	HBOOL					 bMounted;				// 논리적으로 mount 되었음을 나타내는 Flag
	HBOOL					bPvr;
	SvcFs_DevIdx_e			 eDevIdx;				// Device Index. 앞으로 모든 것은 이걸 사용하여 access해야 한다.
	SvcFs_DevType_e			 eDevType;				// Device Type.
	SvcFs_DevStatus_e		 eDevStatus;			// Device Status
	SvcFs_DevSubStatus_e	 eDevSubStatus;			// Device Sub Status
	SvcFs_FileSystemType_e	 eFsType;				// File System Type
	HBOOL					 bWriteProtected;		// FALSE : Write Enabled,  TRUE : Write Protected
//	HUINT64					 ullTotalSizeKb;		// Device의 Total Size (KB 단위)
	HUINT32					 ulDriveNo;			// driver No
	HUINT32					ulPortNum;

	HUINT8					 szMountPath[SVC_FS_MOUNT_PATH_LEN];
	HUINT8					 szLabel[SVC_FS_VOLUME_LABEL_SIZE];

	HUINT32					 ulNumPartition;
	HUINT32					 ulMaxRecInstance;
} SvcFs_DevInfo_t;

HERROR SVC_FS_Init (void);
HERROR SVC_FS_ConvertDevDxInfo2SvcFs  (DxStorage_Info_t *pstDxDevInfo, SvcFs_DevInfo_t *pstSvcDevInfo);
HERROR SVC_FS_GetDeviceInfo (SvcFs_DevIdx_e eDevIdx, SvcFs_DevInfo_t *pstMwDevInfo);
HERROR SVC_FS_GetPartitionInfoArray (SvcFs_DevIdx_e eDevIdx, HUINT32 *pulPartNum, SvcFs_PartitionInfo_t **ppstPartArray);
HERROR SVC_FS_FreePartitionInfoArray (SvcFs_PartitionInfo_t *pstPartArray);
HBOOL SVC_FS_CheckPathIsValid (SvcFs_DevIdx_e eDevIdx, HUINT8 *szPath);
HBOOL SVC_FS_IsDevicePvrPossible (SvcFs_DevIdx_e eDevIdx);
HERROR SVC_FS_GetPvrDevIdPartIdx (HUINT32 ulPvrIdx, SvcFs_DevIdx_e *peDevIdx, HUINT32 *pulPartIdx);
HBOOL	SVC_FS_IsUsbConnected(void);
HERROR SVC_FS_GetPvrPath (HUINT32 ulPvrIdx, SvcFs_UsageType_e eUsage, HCHAR *pszPath);
HERROR SVC_FS_GetPlaybackPath (HUINT32 ulPlaybackIdx, SvcFs_UsageType_e eUsage, HCHAR *pszPath);
HBOOL SVC_FS_IsDevicePvrUsed (SvcFs_DevIdx_e eDevIdx);
HBOOL SVC_FS_IsDeviceRecStorageUsed (SvcFs_DevIdx_e eDevIdx);
HERROR SVC_FS_GetPvrIdxByDevIdx(HUINT32 *pulPvrIdx, SvcFs_DevIdx_e eDevIdx);
HERROR	SVC_FS_GetPvrStorageNum(HUINT32 *pulCount);
HUINT32 SVC_FS_GetDefaultPvrIdx(void);
HERROR SVC_FS_GetDevPartIndexByPath (HUINT8 *szDrivePath, SvcFs_DevIdx_e *peDevIdx, HUINT32 *pulPartIdx);
HERROR SVC_FS_GetDeviceUuidByPvrIdx (HUINT32 ulPvrIdx, HUINT8 *szUuid);
SvcFs_DevStatus_e SVC_FS_GetPvrHddStatus (HUINT32 ulPvrIdx);
HERROR	SVC_FS_GetUuidByPath(HUINT8 *pszPath, HUINT8 *pszUuid);
HERROR	SVC_FS_GetSize(SvcFs_DevIdx_e eDevIdx, HUINT64 *pullTotalKbSize, HUINT64 *pullAvailableKbSize, HUINT64 *pullUsedKbSize, HUINT64 *pullReservedKbSize, HUINT64 *pullAvailablePvrKbSize);
HERROR	SVC_FS_GetPvrReservedSize(SvcFs_DevIdx_e eDevIdx, HUINT64 *pullReservedKbSize);
HBOOL SVC_FS_CheckStoragePowerOff(void);
HERROR SVC_FS_GetTsrOnlyPath (HCHAR *pszPath);
HBOOL SVC_FS_UseTsrOnlyDevice(void);
HUINT32 SVC_FS_GetTsrOnlyDeviceIdx(void);



#endif /* !__SVC_FS_H__ */
