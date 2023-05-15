/**
 * di_flash_nand_null.c
*/

/**
 * @defgroup		DI_FLASH DI_FLASH : flashmemory r/w module
 * @author		dongseok shin
 * @note			Flash Read/write APIs
 * @brief			Define Flash Memory APIs
 * @file 			di_flash_nand_null.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "htype.h"
#include "di_err.h"
#include "di_flash.h"
#include "drv_flash.h"


/*******************************************************************/
/****************************** define *******************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ******************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *****************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables ******************************/
/*******************************************************************/

/*******************************************************************/
/*********************** Function Prototypes ***************************/
/*******************************************************************/
DRV_Error	DRV_NANDFLASH_Init(void)
{
	return DRV_OK;
}

DRV_Error	DRV_NANDFLASH_Read(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	(void)fd;
	(void)ulSrcOffset;
	(void)pucBuffer;
	(void)ulSize;
	
	return DRV_OK;
}

DRV_Error	DRV_NANDFLASH_Erase(int fd, unsigned long ulOffset)
{
	(void)fd;
	(void)ulOffset;
	
	return DRV_OK;
}

DRV_Error	DRV_NANDFLASH_Write(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	(void)fd;
	(void)ulSrcOffset;
	(void)pucBuffer;
	(void)ulSize;
	
	return DRV_OK;
}

DRV_Error	DRV_NANDFLASH_GetNvramHandle(int *pNvramHandle)
{
	(void)pNvramHandle;
	
	return DRV_OK;
}

DRV_Error	DRV_NANDFLASH_GetNvramBackupHandle(int *pNvramBHandle)
{
	(void)pNvramBHandle;
	
	return DRV_OK;
}

DRV_Error 	DRV_NANDFLASH_GetUbiHandle(int *pUbiHandle)
{
	(void)pUbiHandle;
	
	return DRV_OK;
}

DRV_Error	DRV_NANDFLASH_Sync(void)
{
	return DRV_OK;
}

DRV_Error 	DRV_NANDFLASH_BOOT_Read(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	(void)fd;
	(void)ulSrcOffset;
	(void)pucBuffer;
	(void)ulSize;
	
	return DRV_OK;
}

DRV_Error 	DRV_NANDFLASH_BOOT_Erase(int fd, unsigned long ulSrcOffset, unsigned long ulSize)
{
	(void)fd;
	(void)ulSrcOffset;
	(void)ulSize;
	
	return DRV_OK;
}

DRV_Error 	DRV_NANDFLASH_BOOT_Write(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	(void)fd;
	(void)ulSrcOffset;
	(void)pucBuffer;
	(void)ulSize;
	
	return DRV_OK;
}

DRV_Error 	DRV_NANDFLASH_BOOT_GetHandle(int *pBootHandle)
{
	(void)pBootHandle;
	
	return DRV_OK;
}

DRV_Error 	DRV_NANDFLASH_BOOT_Sync(int fd)
{
	(void)fd;
	
	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_UBI_IsBad(HUINT32 ulAddress, HUINT32 ulSize, HINT32 *piBadStatus)
{
	(void)ulAddress;
	(void)ulSize;
	(void)piBadStatus;
		
	return DRV_OK;
}

DI_ERR_CODE DI_NANDFLASH_GetInfo(DI_NANDFLASH_DEVICE_INFO *pNandDevInfo)
{
	(void)pNandDevInfo;
	
	return DI_ERR_OK;
}

DI_ERR_CODE DI_NANDFLASH_MountPartitions(void)
{
	return DI_ERR_OK;
}

DI_ERR_CODE DI_NANDFLASH_UmountPartitions(void)
{
	return DI_ERR_OK;
}

DI_ERR_CODE DI_NANDFLASH_RecreatePartition(DI_FLASH_Partition_t ePartition)
{
	(void)ePartition;
	
	return DI_ERR_OK;
}

DI_ERR_CODE DI_NANDFLASH_CheckReadonlyMount(DI_FLASH_Partition_t ePartition, HBOOL* pbReadOnly)
{
	(void)ePartition;
	(void)pbReadOnly;
	
	return DI_ERR_OK;
}

DI_ERR_CODE DI_NANDFLASH_CreateRecoveryPartition(HUINT32 unSize)
{
	(void)unSize;
	
	return DI_ERR_OK;
}

DI_ERR_CODE DI_NANDFLASH_DeleteRecoveryPartition(void)
{
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_NANDFLASH_GetBadblkCount(DI_NANDFLASH_PARTITION_TYPE_E partition, HUINT32 *count)
{
	(void)partition;
	(void)count;
	
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_NANDFLASH_Write(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	(void)ulAddress;
	(void)pucBuffer;
	(void)ulSize;
	
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_NANDFLASH_Read(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	(void)ulAddress;
	(void)pucBuffer;
	(void)ulSize;
	
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_NANDFLASH_Sync(void)
{
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_NANDFLASH_ReadForCRC(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	(void)ulAddress;
	(void)pucBuffer;
	(void)ulSize;
	
	return DI_ERR_OK;
}
