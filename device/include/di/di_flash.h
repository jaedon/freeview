/* $Header: $ */
/****************************************************************************
* $Workfile:   di_flash.h  $
* $Modtime:   Jul 31 2007 00:17:52  $
*
* Author:
* Description:
*
	*                                 Copyright (c) 2008 HUMAX Co., Ltd.
	*                                               All rights reserved.
	*****************************************************************************/

#ifndef _DI_FLASH_H_
#define _DI_FLASH_H_
#include "htype.h"
#include "di_err.h"

typedef enum
{
	FLASH_KERNEL= 0,
	FLASH_ROOTFS,
	FLASH_SHARE,
	FLASH_HUMAXTV,
	FLASH_OTA,
	FLASH_MHP,
	FLASH_DB,
	FLASH_CFE,
	FLASH_DBBACKUP,
	FLASH_DBUSER,
	FLASH_DBUSEREXT,
	FLASH_DBUSEREXT2
}DI_FLASH_Partition_t;


typedef enum tagDI_FLASH_FIELD
{
	DI_FLASH_FIELD_SERIAL_NO = 0,
	DI_FLASH_FIELD_DTCP_KEY,
	DI_FLASH_FIELD_CI_PLUS_KEY,
	DI_FLASH_FIELD_MAC_ADDR,
	DI_FLASH_FIELD_HDCP_KEY,
	DI_FLASH_FIELD_PARING_DATA,
	DI_FLASH_FIELD_SYSTEM_ID,
	DI_FLASH_FIELD_LOADER_VER,
	DI_FLASH_FIELD_APP_VER,
	DI_FLASH_FIELD_DSTB_ID,
	DI_FLASH_FIELD_CM_SERIAL_NO,
	DI_FLASH_FIELD_CM_MAC_ADDR,
	DI_FLASH_FIELD_CM_MAC_ADDR_ANOTHER,
	DI_FLASH_FIELD_EEP_BACKUP,
	DI_FLASH_FIELD_PRAVATE,
	DI_FLASH_FIELD_BOOT_LOADER,
	DI_FLASH_FIELD_NVRAM,
	DI_FLASH_FIELD_SPLASH,
	DI_FLASH_FIELD_SPLASH_ETC1,
	DI_FLASH_FIELD_SPLASH_ETC2,
	DI_FLASH_FIELD_SPLASH_ETC3,
	DI_FLASH_FIELD_CSC_KEY,
	DI_FLASH_FIELD_MAX
} DI_FLASH_FIELD_t;

typedef enum {
	DI_NANDFLASH_PARTITION_BOOTLOADER = 0,
	DI_NANDFLASH_PARTITION_UBI,
	DI_NANDFLASH_PARTITION_UBIFS,
	DI_NANDFLASH_PARTITION_MAX		
} DI_NANDFLASH_PARTITION_TYPE_E;

typedef struct {
	HUINT32 	ulBlockNumber;      // block index
	HUINT32 	ulBlockOffset;		// block base address
	HUINT32 	ulBlockSize;		// block size
} FLASH_BLOCK_INFO;

typedef struct
{
	HUINT32 	ulFlashSize;
	HUINT32 	ulNumOfBlock;
	FLASH_BLOCK_INFO *pstFlashBlockInfo;
} DI_FLASH_DEVICE_INFO, *pDI_FLASH_DEVICE_INFO;

typedef struct {
	HUINT32 	ulLebSize;
} UBI_BLOCK_INFO;

typedef struct
{
	HUINT32 	ulFlashSize;
	HUINT32 	ulNumOfBlock;
	HUINT32 	ulFlashBootBlockSize;
	UBI_BLOCK_INFO stUbiInfo;
} DI_NANDFLASH_DEVICE_INFO;

DI_ERR_CODE DI_FLASH_EraseBlock(HUINT32 ulBlockNumber);
DI_ERR_CODE DI_FLASH_Read(HUINT32  usAddress, HUINT8 *  pucBuffer, HUINT32  ulSize);
DI_ERR_CODE DI_FLASH_Write(HUINT32  usAddress, HUINT8 *  pucBuffer, HUINT32  ulSize);
DI_ERR_CODE	DI_FLASH_WriteNotAligned(HUINT32 usAddress, HUINT8* pucBuffer, HUINT32 ulSize);
DI_ERR_CODE DI_FLASH_GetInfo(DI_FLASH_DEVICE_INFO *pstFlashInfo);
DI_ERR_CODE DI_FLASH_GetDbInfo(DI_FLASH_DEVICE_INFO *pstFlashInfo);
DI_ERR_CODE DI_NANDFLASH_GetInfo(DI_NANDFLASH_DEVICE_INFO *pNandDevInfo);
DI_ERR_CODE DI_NANDFLASH_MountPartitions(void);
DI_ERR_CODE DI_NANDFLASH_UmountPartitions(void);
DI_ERR_CODE DI_NANDFLASH_RecreatePartition(DI_FLASH_Partition_t ePartition);
DI_ERR_CODE DI_NANDFLASH_CheckReadonlyMount(DI_FLASH_Partition_t ePartition, HBOOL* pbReadOnly);
DI_ERR_CODE DI_NANDFLASH_CreateRecoveryPartition(HUINT32 unSize);
DI_ERR_CODE DI_NANDFLASH_DeleteRecoveryPartition(void);
DI_ERR_CODE	DI_NANDFLASH_GetBadblkCount(DI_NANDFLASH_PARTITION_TYPE_E partition, HUINT32 *count);

DI_ERR_CODE	DI_NANDFLASH_Write(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize);
DI_ERR_CODE	DI_NANDFLASH_Read(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize);
DI_ERR_CODE	DI_NANDFLASH_Sync(void);

DI_ERR_CODE DI_EMMC_MountPartitions(void);
DI_ERR_CODE DI_EMMC_UmountPartitions(void);
DI_ERR_CODE DI_EMMC_CreatePartitions(void);
DI_ERR_CODE DI_EMMC_Write(HUINT32 address, HUINT8 *data, HUINT32 data_size);
DI_ERR_CODE DI_EMMC_Read(HUINT32 address, HUINT8 *data, HUINT32 data_size);
DI_ERR_CODE DI_EMMC_Sync(void);
DI_ERR_CODE DI_EMMC_ExcuteCheckDisk(void);
DI_ERR_CODE	DI_EMMC_CheckReadonlyMount(DI_FLASH_Partition_t ePartition, HBOOL* pbReadOnly);
DI_ERR_CODE	DI_EMMC_RecreatePartition(DI_FLASH_Partition_t ePartition);
DI_ERR_CODE	DI_EMMC_CreateRecoveryPartition(HUINT32 unSize);
DI_ERR_CODE	DI_EMMC_DeleteRecoveryPartition(void);

#endif	// _DI_FLASH_H_

