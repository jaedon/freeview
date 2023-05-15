/**
 * drv_flash.h
*/

/**
 * @defgroup		DRV_FLASH DRV_FLASH : flashmemory r/w module
 * @author		Jin-hong Kim
 * @note			DRV_FLASH Read/write APIs
 * @brief			Define Flash Memory APIs
 * @file 			drv_flash.h
*/

#ifndef __DRV_FLASH_H__
#define __DRV_FLASH_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define CFE_BASE					0x00000
#define CONFIG_BASE					0xC0000
#define CAS_ETC_BASE				0x160000


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DRV_Error	DRV_FLASH_Init(void);
DRV_Error	DRV_FLASH_EraseBlock(int fd, unsigned long ulDestOffset);
DRV_Error	DRV_FLASH_Read(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize);
DRV_Error	DRV_FLASH_Write(int fd, unsigned long ulDestOffset, unsigned char *pucData, unsigned long ulSize);
DRV_Error	DRV_FLASH_SetField(int type, void *data, HUINT32  ulSize);
DRV_Error	DRV_FLASH_GetField(int type, void *data, HUINT32  ulSize);
DRV_Error	DRV_FLASH_GetCapabilities(HUINT32 *pulBlockSize, HUINT32* pulBlockNumber);
DRV_Error	DRV_FLASH_GetCFE_Fd(int *fd);
DRV_Error	DRV_FLASH_GetOTPBlock(unsigned long ulOffset, unsigned char *pucOTPFlag);
DRV_Error	DRV_FLASH_SetOTPBlock(unsigned long ulOffset);

DRV_Error	DRV_NANDFLASH_Init(void);
DRV_Error	DRV_NANDFLASH_Read(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize);
DRV_Error	DRV_NANDFLASH_Erase(int fd, unsigned long ulOffset);
DRV_Error	DRV_NANDFLASH_Write(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize);
DRV_Error	DRV_NANDFLASH_GetNvramHandle(int *pNvramHandle);
DRV_Error	DRV_NANDFLASH_GetNvramBackupHandle(int *pNvramBHandle);
DRV_Error 	DRV_NANDFLASH_GetUbiHandle(int *pUbiHandle);
DRV_Error	DRV_NANDFLASH_Sync(void);

DRV_Error 	DRV_NANDFLASH_BOOT_Read(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize);
DRV_Error 	DRV_NANDFLASH_BOOT_Erase(int fd, unsigned long ulSrcOffset, unsigned long ulSize);
DRV_Error 	DRV_NANDFLASH_BOOT_Write(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize);
DRV_Error 	DRV_NANDFLASH_BOOT_GetHandle(int *pBootHandle);
DRV_Error 	DRV_NANDFLASH_BOOT_Sync(int fd);

DRV_Error DRV_EMMC_Init(void);
DRV_Error DRV_EMMC_Read(int handle, unsigned long addr, unsigned char *data, unsigned long data_size);
DRV_Error DRV_EMMC_Erase(int handle, unsigned long addr);
DRV_Error DRV_EMMC_Write(int handle, unsigned long addr, unsigned char *data, unsigned long data_size);
DRV_Error DRV_EMMC_GetNvramHandle(int *handle);
DRV_Error DRV_EMMC_Sync(void);

#endif /* !__DI_FLASH_H__ */



