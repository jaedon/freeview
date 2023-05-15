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
#define SVC_FS_DEV_UUID_BUFSIZE				(VOLUUIDSZ+1) 		// pal_fs.h�� VOLUUIDSZ�� 36 + null char �ؼ� 37 �̻� �־�� �Ѵ�.
#define SVC_FS_MAX_PARTITION				12		// PAL_FS_MAX_PARTITIONS

#define SVC_FS_USB_PVR_PART_IDX				0	// USB HDD�� PVR�� partition index (����: ���� USB HDD�� ���ؼ��� 1���� Partition�� ����ϵ��� �����Ǿ��ִ�.)
#define MAX_CONNECTABLE_PVR_STORAGE_NUM		CONFIG_MW_MM_MAX_CONNECTABLE_PVR_STORAGE_NUM	// ���ÿ� �����Ͽ� �̿밡���� �ִ� PVR Storage ����
#if defined CONFIG_MW_MM_MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM
#define MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM		CONFIG_MW_MM_MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM	// ���ÿ� �����Ͽ� �̿밡���� �ִ� Playback Storage ����
#else
#define MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM		1
#endif



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

// Mount ����
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
	eSVC_FS_DevType_IntHdd		= 0x01,				// Internal HDD : PVR model ��� ����� HDD
	eSVC_FS_DevType_Detachable		= 0x02,				// Detachable HDD : SATA interface�� Ż�� ������ HDD
	eSVC_FS_DevType_eSataHdd		= 0x03,				// eSATA HDD : eSATA interface�� ������ HDD
	eSVC_FS_DevType_UsbHdd		= 0x04,				// External USB HDD : USB�� �ɴ� ������ HDD
	eSVC_FS_DevType_UsbMem		= 0x05,				// USB Memory : HDD�� �ƴ� USB �޸�.  "USB�� ���� ��"�� ã������ ���� ExtHdd�� UsbMem ���� �� ã�ƾ� �Ѵ�.
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
//DxStorage_FsType_e �� �ݵ�� ���ƾ� �Ѵ�.

typedef enum
{
	eDevStatus_NoDevice						= 0x0000,
	eDevStatus_NotInitiated,
	eDevStatus_Malfunction,
	eDevStatus_Formatting,

	eDevStatus_NeedFormat					= 0x0010,	// need the format (All the partition)
	eDevStatus_SafeRemoved					= 0x0011,	// Unmounted(��������..) device for USB HDD

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
	eSVC_FS_UsageType_Root,					// Mount �� root directory
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
	AP���� M/W FS Device�� ���¸� �˱� ���� �ʿ��� structure.
	M/W �������� API �̿ܿ��� �� structure�� "�����" ������� ���� SvcFs_DevContext_t �� ����ϵ��� �Ѵ�.
*/

typedef struct
{
	// Connect �� ���� MBR �� �޶����� �ʴ� �� �����Ǵ� argument
	// Mount/Unmount�� ���� �޶����� �ʴ´�.
	HBOOL					bAvailable;
	SvcFs_FileSystemType_e	ePartFsType;							// Partition File System Type
	HUINT32					ulStartUnit, ulEndUnit;

	// Mount/Unmount ���¿� ���� �޶����� arguments
	// Mount/Unmount �ÿ� �� �������� ����ų� �����ؾ� �Ѵ�.
	HBOOL					bMounted;				// �ش� Partition�� Mount �Ǿ������� �˷��ִ� Flag
	HBOOL					bPvrFlag;
	HUINT32					ulPvrIdx;				// Pvr�� Storage ���� Index
	HUINT32					ulPlaybackIdx;			// Playback�� Storage ���� Index
	HBOOL					bDefaultPvr;			// Multi PVR HDD�� ���� default PVR HDD ���� ǥ��
	HBOOL					bWritable;
	HBOOL					bSecured;				// FALSE : Normal File system, TRUE : Encrypted File system // MOON_encr
	HUINT32 				 	ulDriveNo; 				// ���� Partition�� Drive No.
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
	HBOOL					 bConnected;			// ���������� ����Ǿ����� ��Ÿ���� Flag
	HBOOL					 bMounted;				// �������� mount �Ǿ����� ��Ÿ���� Flag
	HBOOL					bPvr;
	SvcFs_DevIdx_e			 eDevIdx;				// Device Index. ������ ��� ���� �̰� ����Ͽ� access�ؾ� �Ѵ�.
	SvcFs_DevType_e			 eDevType;				// Device Type.
	SvcFs_DevStatus_e		 eDevStatus;			// Device Status
	SvcFs_DevSubStatus_e	 eDevSubStatus;			// Device Sub Status
	SvcFs_FileSystemType_e	 eFsType;				// File System Type
	HBOOL					 bWriteProtected;		// FALSE : Write Enabled,  TRUE : Write Protected
//	HUINT64					 ullTotalSizeKb;		// Device�� Total Size (KB ����)
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
