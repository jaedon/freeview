/* $Header: $ */
/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_HDD_H_
#define _FDI_HDD_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#define FDI_HDD_MODEL_NAME_SIZE 128
#define FDI_HDD_SERIAL_NO_SIZE 64
#define FDI_HDD_FIRMWARE_VER_SIZE 8
#define FDI_HDD_INFO_MAX_SIZE 200
#define FDI_HDD_MAX_PARTITION_NUMBER 10

typedef enum {
    FDI_ATA_IDENTIFY_DEVICE			=0,
    FDI_ATA_SMART_READ_ATTR_VALUE,
    FDI_ATA_SMART_READ_ATTR_THRE_VALUE,
    FDI_ATA_SMART_ENABLE,
    FDI_ATA_SMART_EXECUTE_OFFLINE_IMMEDIATE,
    FDI_ATA_CMD_MAX	=255
} FDI_ATA_CMD;

typedef enum
{
	FDI_FS_EXT2_FILESYSTEM,
	FDI_FS_EXT3_FILESYSTEM,
	FDI_FS_EXT4_FILESYSTEM,
	FDI_FS_XFS_FILESYSTEM,
	FDI_FS_FAT16_FILESYSTEM,
	FDI_FS_FAT32_FILESYSTEM,
	FDI_FS_NTFS_FILESYSTEM,
	FDI_FS_MSDOS_FILESYSTEM,
	FDI_FS_EXFAT_FILESYSTEM,
} FDI_FS;

typedef enum
{
	FDI_HDD_EVENT_FAIL,
	FDI_HDD_EVENT_DONE,
	FDI_HDD_EVENT_MAX
} FDI_HDD_EVENT_e;

typedef struct 
{
	unsigned char aucModelName[FDI_HDD_MODEL_NAME_SIZE];
	unsigned char aucFirmwareVer[FDI_HDD_FIRMWARE_VER_SIZE];
	unsigned char aucSerialNo[FDI_HDD_SERIAL_NO_SIZE];
	unsigned long ulSizeMB;
	unsigned long ulNumPartitions;
	unsigned long long ullPartitionSize[FDI_HDD_MAX_PARTITION_NUMBER]; /* limit max allowed number of partitions to 10 */
} FDI_HDD_INFO_t;

typedef void (*FDI_HDD_CALLBACK)();

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_Open(void);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_GetInfo(FDI_HDD_INFO_t *tHddInfo);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_Write(unsigned long unOffset, unsigned char *pucData, unsigned long *pulSize);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_Read(unsigned long unOffset, unsigned char *pucData, unsigned long *pulSize);

FDI_ERR_CODE FDI_HDD_GetStatus(BOOL *bConnect);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_SetParking(int portnum, unsigned char ParkingOn);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_GetName(int portnum, char *pDevName);


#endif



