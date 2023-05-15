/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2014/11/22
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <htype.h>
#include <stdarg.h>

#include "linden_trace.h"

#include <drv_err.h>
#include <drv_bootloader.h>
#include "drv_flash.h"

#include <di_err.h>
#include <di_flash.h>
#include "di_uart.h"

#include <flash_map.h>
#include <vkernel.h>

#include "humax_bsu.h"
#if defined(CONFIG_ARM)
#include <ioctl.h>
#else
#include <cfe_ioctl.h>
#endif

#include "taskdef.h"

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/
#define UNUSED(x) (void)(x)

#define DEBUG_MODULE				TRACE_MODULE_DI_FLASH


#define FLASH_DEVICE_NAME	"flash0"

#define MAX_NAME_LENGTH	32

#define FLASH_PARTITION_DEBUG 0

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/
#if defined(CONFIG_ARM)
typedef struct flash_info		flash_info_t;
#endif

/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
extern void ubidrv_init(int nUbiNum);

#if defined(CONFIG_NOR_FLASH)
DI_FLASH_DEVICE_INFO s_diFlashInfo;
#endif
#if defined(CONFIG_EMMC_FLASH)
static HULONG s_ulEmmcOpenCloseSema;
#endif

char szNvramName[30];

#if 0
STATIC DRV_Error _drv_Flash_GetPartition(FlashType_e eFlashType, unsigned long ulOffset, unsigned long ulSize, flash_partition *partition)
{
	HINT32		i;
	HULONG		lobound = 0;
	HULONG		hibound = 0;
	HINT32		num_flash_partition = 0;

	num_flash_partition = sizeof(flash_partition_map) / sizeof(flash_partition_map[0]);

#if FLASH_PARTITION_DEBUG
	PrintError("[%s:%d] num_flash_partition : %d\n", __FUNCTION__, __LINE__,
													num_flash_partition);
#endif

	for(i=0; i < num_flash_partition; i++)
	{
		if(eFlashType != flash_partition_map[i].eFlashType)
			continue;

		lobound = flash_partition_map[i].offset;
		hibound = flash_partition_map[i].offset + flash_partition_map[i].size;

		if( (ulOffset >= lobound) && (ulOffset + ulSize < hibound))
		{
			*partition = flash_partition_map[i];
#if FLASH_PARTITION_DEBUG
			PrintError("[%s:%d] flashtype : %d\n", __FUNCTION__, __LINE__,
													partition->eFlashType);
			PrintError("[%s:%d] partition.device : %s\n", __FUNCTION__,
										__LINE__, partition->device);
			PrintError("[%s:%d] partition.offset : 0x%X\n", __FUNCTION__,
										__LINE__, partition->offset);
			PrintError("[%s:%d] partition.size : 0x%X\n", __FUNCTION__,
										__LINE__, partition->size);
#endif
			return DRV_OK;
		}
	}

	return DRV_ERR;
}
#endif

#if defined(CONFIG_EMMC_FLASH)
DRV_Error	DRV_EMMC_Erase(int fd, unsigned long ulOffset)
{
	UNUSED(fd);
	UNUSED(ulOffset);

	return DRV_OK;
}

#if 0
DRV_Error	DRV_EMMC_GetInfo(DI_NANDFLASH_DEVICE_INFO *pNandDevInfo)
{
	UNUSED(pNandDevInfo);

	PrintDebug("[%s:%d] called\n", __FUNCTION__, __LINE__);
	return BxRET_OK;
}
#endif

DRV_Error	DRV_EMMC_GetNvramHandle(int *pNvramHandle)
{
	UNUSED(pNvramHandle);

	return DRV_OK;
}

DRV_Error	DRV_EMMC_Sync(void)
{
	return DRV_OK;
}

STATIC DRV_Error	_drv_Emmcflash_GetPartition(unsigned long ulOffset, unsigned long ulSize, emmc_partition *partition)
{
	HINT32		i;
	HULONG		lobound = 0;
	HULONG		hibound = 0;
	HINT32		num_emmc_partition = 0;

	num_emmc_partition = sizeof(emmc_partition_map) / sizeof(emmc_partition_map[0]);

	for(i=0; i < num_emmc_partition; i++)
	{
		lobound = emmc_partition_map[i].offset;
		hibound = emmc_partition_map[i].offset + emmc_partition_map[i].size;

		if( (ulOffset >= lobound) && ((ulOffset + ulSize - 1) < hibound))
		{
			*partition = emmc_partition_map[i];
			return DRV_OK;
		}
	}

	return DRV_ERR;
}
#endif

#if defined(CONFIG_MIPS)
static int FlashOpenFieldDevice( DI_FLASH_FIELD_t field )
{
	int sfd;

	switch ( field )
	{
		case DI_FLASH_FIELD_SERIAL_NO :
			sfd = BSU_open("flash0.serial");
			break;

		case DI_FLASH_FIELD_DTCP_KEY :
			sfd = BSU_open("flash0.dtcp");
			break;

		case DI_FLASH_FIELD_CI_PLUS_KEY :
			sfd = BSU_open("flash0.ci+");
			break;

		case DI_FLASH_FIELD_MAC_ADDR :
			sfd = BSU_open("flash0.macadr");
			break;

		case DI_FLASH_FIELD_HDCP_KEY :
			sfd = BSU_open("flash0.hdcp");
			break;

		case DI_FLASH_FIELD_EEP_BACKUP :
			sfd = BSU_open("flash0.eep_back");
			break;

		case DI_FLASH_FIELD_PRAVATE :
			sfd = BSU_open("flash0.pravate");
			break;

		case DI_FLASH_FIELD_BOOT_LOADER :
			sfd = BSU_open("flash0.cfe");
			break;

		case DI_FLASH_FIELD_NVRAM :
			sfd = BSU_open("flash0.nvram");
			break;

		case DI_FLASH_FIELD_SPLASH :
			sfd = BSU_open("flash0.splash");
			break;

		default :
			PrintError("nvalid field type %d\n", field);
			return -1;
	}

	return sfd;
}
#endif

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/
#if defined(CONFIG_EMMC_FLASH)
#if defined(CONFIG_FLASH_ACCESS_BY_TYPE)
DI_ERR_CODE DI_EMMC_Write(HUINT32 address, HUINT8 *data, HUINT32 data_size)
{
	DRV_Error DrvErr;

	if (!data || !data_size)
	{
		PrintError("[%s] Error. invalid parametes. data(%p), data_size(%d)\n", __FUNCTION__, data, data_size);
		return DI_ERR_ERROR;
	}

	DrvErr = DRV_EMMC_Write(0, address, data, data_size);
	if (DrvErr != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_EMMC_Read(HUINT32 address, HUINT8 *data, HUINT32 data_size)
{
	DRV_Error DrvErr;

	if (!data || !data_size)
	{
		PrintError("[%s] Error. invalid parametes. data(%p), data_size(%d)\n", __FUNCTION__, data, data_size);
		return DI_ERR_ERROR;
	}

	DrvErr = DRV_EMMC_Read(0, address, data, data_size);
	if (DrvErr != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#endif

DRV_Error DRV_EMMC_Read(int fd, unsigned long ulOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	int retVal = DRV_OK;
	int emmc_fd, res;
	emmc_partition partition;
	unsigned long emmcOffset = 0;

	UNUSED(fd);

	/* find partition */
	res = _drv_Emmcflash_GetPartition(ulOffset, ulSize, &partition);
	if( res != DRV_OK)
	{
		PrintError("[%s] can't find partition... offset=0x%x, ulSize=0x%x\n", __FUNCTION__, ulOffset, ulSize);
		return DRV_ERR;
	}

	VK_SEM_Get(s_ulEmmcOpenCloseSema);

	/* open */
	emmc_fd = BSU_open(partition.device);
	if( emmc_fd < 0 )
	{
		PrintError("[%s] can't open %s... \n", __FUNCTION__, partition.device);
		VK_SEM_Release(s_ulEmmcOpenCloseSema);
		return DRV_ERR;
	}

	/* calculate offset */
	emmcOffset = ulOffset - partition.offset;

	retVal = BSU_readblk(emmc_fd, (HUINT64)emmcOffset, pucBuffer, (int)ulSize);
	if (retVal < 0)
	{
		PrintError("[%s] can't open %s... \n", __FUNCTION__, partition.device);
		BSU_close(emmc_fd);
		VK_SEM_Release(s_ulEmmcOpenCloseSema);
		return DRV_ERR;
	}

	BSU_close(emmc_fd);
	VK_SEM_Release(s_ulEmmcOpenCloseSema);
	return DRV_OK;
}

DRV_Error DRV_EMMC_Write(int fd, unsigned long ulOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	int retVal = DRV_OK;
	int emmc_fd, res;
	emmc_partition partition;
	unsigned long emmcOffset = 0;

	UNUSED(fd);

	/* find partition */
	res = _drv_Emmcflash_GetPartition(ulOffset, ulSize, &partition);
	if( res != DRV_OK)
	{
		PrintError("[%s] can't find partition... \n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_SEM_Get(s_ulEmmcOpenCloseSema);

	/* open */
	emmc_fd = BSU_open(partition.device);
	if( emmc_fd < 0 )
	{
		PrintError("[%s] can't open %s... \n", __FUNCTION__, partition.device);
		VK_SEM_Release(s_ulEmmcOpenCloseSema);
		return DRV_ERR;
	}

	/* calculate offset */
	emmcOffset = ulOffset - partition.offset;

	retVal = BSU_writeblk(emmc_fd, (HUINT64)emmcOffset, pucBuffer, (int)ulSize);
	if (retVal < 0)
	{
		PrintError("[%s] can't open %s... \n", __FUNCTION__, partition.device);
		BSU_close(emmc_fd);
		VK_SEM_Release(s_ulEmmcOpenCloseSema);
		return DRV_ERR;
	}

	BSU_close(emmc_fd);
	VK_SEM_Release(s_ulEmmcOpenCloseSema);
	return DRV_OK;
}

DRV_Error DRV_EMMC_GetInfo(void)
{
	PrintError("not supported");
	return DRV_OK;
}

DRV_Error DRV_EMMC_Init(void)
{
	HINT32 err = DRV_OK;
	err = VK_SEM_Create(&s_ulEmmcOpenCloseSema,"emmc_openclose", DEFAULT_SUSPEND_TYPE);
	if (err != VK_OK)
	{
		PrintError("[%s] emmc_openclose VK_SEM_Create Error!!. (%d)\n", __FUNCTION__, err);
		return DI_ERR_ERROR;
	}

	return DRV_OK;
}
#endif

#if defined (CONFIG_NOR_FLASH)
STATIC DRV_Error _drv_Norflash_GetPartition(unsigned long ulOffset, unsigned long ulSize, flash_partition *partition)
{
	HINT32		i;
	HULONG		lobound = 0;
	HULONG		hibound = 0;
	HINT32		num_norflash_partition = 0;

	num_norflash_partition = sizeof(nor_partition_map) / sizeof(nor_partition_map[0]);

	for(i=0; i < num_norflash_partition; i++)
	{
		lobound = nor_partition_map[i].offset;
		hibound = nor_partition_map[i].offset + nor_partition_map[i].size;

		if( (ulOffset >= lobound) && ((ulOffset + ulSize - 1) < hibound))
		{
			*partition = nor_partition_map[i];
			return DRV_OK;
		}
	}

	return DRV_ERR;
}

DRV_Error DRV_NORFLASH_Read(HUINT32 ulOffset, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	int retVal = DRV_OK;
	int fd, res;
	flash_partition partition;
	unsigned long offset = 0;

	/* find partition */
	res = _drv_Norflash_GetPartition(ulOffset, ulSize, &partition);
	if( res != DRV_OK)
	{
		PrintError("[%s] can't find partition... \n", __FUNCTION__);
		return DRV_ERR;
	}

	/* open */
	fd = BSU_open(partition.device);
	if( fd < 0 )
	{
		PrintError("[%s] can't open %s... \n", __FUNCTION__, partition.device);
		return DRV_ERR;
	}

	/* calculate offset */
	offset = ulOffset - partition.offset;

	retVal = BSU_readblk(fd, (HUINT64)offset, pucBuffer, (int)ulSize);
	if (retVal < 0)
	{
		PrintError("[%s] can't open %s... \n", __FUNCTION__, partition.device);
		BSU_close(fd);
		return DRV_ERR;
	}

	BSU_close(fd);
	return DRV_OK;
}

DRV_Error DRV_NORFLASH_Write(HUINT32 ulOffset, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	int retVal = DRV_OK;
	int fd, res;
	flash_partition partition;
	unsigned long offset = 0;

	/* find partition */
	res = _drv_Norflash_GetPartition(ulOffset, ulSize, &partition);
	if( res != DRV_OK)
	{
		PrintError("[%s] can't find partition... \n", __FUNCTION__);
		return DRV_ERR;
	}

	/* open */
	fd = BSU_open(partition.device);
	if( fd < 0 )
	{
		PrintError("[%s] can't open %s... \n", __FUNCTION__, partition.device);
		return DRV_ERR;
	}

	/* calculate offset */
	offset = ulOffset - partition.offset;

	retVal = BSU_writeblk(fd, (HUINT64)offset, pucBuffer, (int)ulSize);
	if (retVal < 0)
	{
		PrintError("[%s] writeblk fail %s... \n", __FUNCTION__, partition.device);
		retVal = DRV_ERR;
	}
	else
	{
		retVal = DRV_OK;
	}

	BSU_close(fd);

	return retVal;
}

DRV_Error DRV_NORFLASH_Erase(HINT32 flashdevice, HUINT32 ulOffset)
{
	int fd, res, retlen;
	flash_sector_t sector;
	flash_partition partition;
	flash_range_t range;

	HAPPY(flashdevice);

	VK_memset(&sector, 0, sizeof(flash_sector_t));

	/* get flash device name from arbitrary partition */
	res = _drv_Norflash_GetPartition(ulOffset, 1, &partition);
	if( res != DRV_OK)
	{
		PrintError("[%s] can't find partition... \n", __FUNCTION__);
		return DRV_ERR;
	}

	/* open */
	fd = BSU_open(partition.device);
	if( fd < 0 )
	{
		PrintError("[%s] can't open %s... \n", __FUNCTION__, partition.device);
		return DRV_ERR;
	}

	/* calculate offset */
	res = BSU_ioctl(fd, IOCTL_FLASH_GETSECTORS, (HUINT8*)&sector, sizeof(flash_sector_t), (HUINT32*)&retlen, 0);
	if (res < 0)
	{
		PrintError("[%s] can't get sector info of %s, offset 0x%X, size 0x%X ... \n", __FUNCTION__, partition.device, ulOffset, 1);
		BSU_close(fd);
		return DRV_ERR;
	}

	/* calculate offset */
	range.range_base = ulOffset -  partition.offset;
	range.range_length = sector.flash_sector_size;

	res = BSU_ioctl(fd, IOCTL_FLASH_ERASE_RANGE, &range, sizeof(range), NULL, 0);
	if (res != 0)
	{
		PrintError("[%s] err %x ulOffset : 0x%x sector.flash_sector_size: 0x%x\n", __FUNCTION__, res, ulOffset, sector.flash_sector_size);
		BSU_close(fd);
		return DRV_ERR;
	}
	BSU_close(fd);

	return DRV_OK;
}

DRV_Error DRV_NOR_Init(void)
{
	HUINT32			i;
	HINT32			fd;
	HINT32			res;
	HUINT32			retlen;
	DRV_Error		retVal = DRV_OK;
	flash_info_t		info;
	flash_sector_t		sector;


	fd = BSU_open(nor_partition_map[0].device);
	if( fd < 0 )
	{
		PrintError("[%s] Could not open nor flash device %s (fd = %d)\n", __FUNCTION__, nor_partition_map[0].device, fd);
		return DRV_ERR;
	}

	res = BSU_ioctl( fd, IOCTL_FLASH_GETINFO, &info, sizeof(flash_info_t), &retlen, 0);
	if( res != 0 )
	{
		PrintError("[%s] ioctl fail : %s (fd = %d)\n", __FUNCTION__, nor_partition_map[0].device, fd);
		retVal = DRV_ERR;
		goto ERR_EXIT;
	}

	VK_memset( &sector, 0x0, sizeof(flash_sector_t) );
	sector.flash_sector_idx = 0;
	res = BSU_ioctl( fd, IOCTL_FLASH_GETSECTORS, (HUINT8 *)&sector, sizeof(flash_sector_t), &retlen, 0);
	if (res != 0 || sector.flash_sector_status == FLASH_SECTOR_INVALID)
	{
		PrintError("[%s] IOCTL_FLASH_GETSECTORS 0 sector fail : %s (fd = %d)\n", __FUNCTION__, nor_partition_map[0].device, fd);
	}

	s_diFlashInfo.ulFlashSize = info.flash_size;
	s_diFlashInfo.ulNumOfBlock = (HINT32)(info.flash_size / sector.flash_sector_size);
	s_diFlashInfo.pstFlashBlockInfo = VK_MEM_Alloc(s_diFlashInfo.ulNumOfBlock*sizeof(FLASH_BLOCK_INFO));
	if (s_diFlashInfo.pstFlashBlockInfo == NULL)
	{
		PrintError("[%s] ioctl fail : %s (fd = %d)\n", __FUNCTION__, nor_partition_map[0].device, fd);
		retVal = DRV_ERR_OUTOFMEMORY;
		goto ERR_EXIT;
	}
	s_diFlashInfo.pstFlashBlockInfo[0].ulBlockNumber = 0;
	s_diFlashInfo.pstFlashBlockInfo[0].ulBlockSize = sector.flash_sector_size;
	s_diFlashInfo.pstFlashBlockInfo[0].ulBlockOffset = info.flash_base;
	for (i = 1; i<s_diFlashInfo.ulNumOfBlock; i++)
	{
		sector.flash_sector_idx = i;

		res = BSU_ioctl( fd, IOCTL_FLASH_GETSECTORS, (HUINT8 *)&sector, sizeof(flash_sector_t), &retlen, 0);
		if (res != 0 || sector.flash_sector_status == FLASH_SECTOR_INVALID)
		{
			break;
		}
		s_diFlashInfo.pstFlashBlockInfo[i].ulBlockNumber = i;
		s_diFlashInfo.pstFlashBlockInfo[i].ulBlockSize = sector.flash_sector_size;
		s_diFlashInfo.pstFlashBlockInfo[i].ulBlockOffset = info.flash_base + i*s_diFlashInfo.pstFlashBlockInfo[i-1].ulBlockSize;
	}

	s_diFlashInfo.ulNumOfBlock = i;

ERR_EXIT:
	BSU_close(fd);
	return retVal;
}
#endif // CONFIG_NOR_FLASH

#if defined(CONFIG_NAND_FLASH)
STATIC DRV_Error _drv_Nandflash_GetPartition(unsigned long ulOffset, unsigned long ulSize, nand_flash_partition *partition)
{
	HINT32		i;
	HULONG		lobound = 0;
	HULONG		hibound = 0;
	HINT32		num_nand_partition = 0;

	num_nand_partition = sizeof(nand_partition_map) / sizeof(nand_partition_map[0]);
#if FLASH_PARTITION_DEBUG
	PrintError("[%s:%d] num_nand_partition : %d\n", __FUNCTION__, __LINE__,
													num_nand_partition);
#endif

	for(i=0; i < num_nand_partition; i++)
	{
		lobound = nand_partition_map[i].offset;
		hibound = nand_partition_map[i].offset + nand_partition_map[i].size;

		if( (ulOffset >= lobound) && ((ulOffset + ulSize - 1) < hibound))
		{
			*partition = nand_partition_map[i];
#if FLASH_PARTITION_DEBUG
			PrintError("[%s:%d] partition.device : %s\n", __FUNCTION__,
										__LINE__, partition->device);
#endif
			return DRV_OK;
		}
	}

	return DRV_ERR;
}

DRV_Error DRV_NANDFLASH_Init(void)
{
#if defined(CONFIG_UBI)
#if defined(CONFIG_UBI_NVRAM_MTD_PART)
	/* MTD for nvram exists */
	VK_sprintf(szNvramName, "ubi2.%s", NAND_FLASH_UBINVRAM_DEVICE_NAME);
#else
	/* Volume for nvram exists */
	VK_sprintf(szNvramName, "ubi0.%s", NAND_FLASH_UBI_DEVICE_NAME);
#endif
#endif

	return DRV_OK;
}


DRV_Error DRV_NANDFLASH_GetNvramHandle(int *pNvramHandle)
{
	*pNvramHandle = 0;
	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_GetNvramBackupHandle(int *pNvramBHandle)
{
	*pNvramBHandle = 0;
	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_Write(int fd, unsigned long ulOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	int retVal = DI_ERR_OK;
	int ubi_fd;
	int res;
	nand_flash_partition partition;
	HUINT32 offset = 0;

	HAPPY(fd);

	/* find partition */
	res = _drv_Nandflash_GetPartition(ulOffset, ulSize, &partition);
	if( res != DRV_OK)
	{
		PrintError("[%s] can't find partition... \n", __FUNCTION__);
		return DRV_ERR;
	}

	ubi_fd = BSU_open(partition.device);
	if (ubi_fd < 0) {
		PrintError("[%s:%d] error : %d, cannot open %s\n", __FUNCTION__,
						__LINE__, ubi_fd, partition.device);
		return DI_ERR_ERROR;
	}

	/* calculate offset */
	offset = ulOffset - partition.offset;

	retVal = BSU_writeblk(ubi_fd, (HUINT64)offset, pucBuffer, (int)ulSize);
	if (retVal < 0)
	{
		PrintError("error write nand address(0x%08X)\n", ulOffset);
		BSU_close(ubi_fd);
		return DI_ERR_ERROR;
	}

	BSU_close(ubi_fd);

	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_Read(int fd, unsigned long ulOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	int retVal = DI_ERR_OK;
	int ubi_fd;
	int res;
	nand_flash_partition partition;
	HUINT32 offset = 0;

	HAPPY(fd);

	/* find partition */
	res = _drv_Nandflash_GetPartition(ulOffset, ulSize, &partition);
	if( res != DRV_OK)
	{
		PrintError("[%s] can't find partition... \n", __FUNCTION__);
		return DRV_ERR;
	}

	ubi_fd = BSU_open(partition.device);
	if (ubi_fd < 0) {
		PrintError("[%s:%d] error : %d, cannot open %s\n", __FUNCTION__,
						__LINE__, ubi_fd, partition.device);
		return DI_ERR_ERROR;
	}

	/* calculate offset */
	offset = ulOffset - partition.offset;

	retVal = BSU_readblk(ubi_fd, (HUINT64)offset, pucBuffer, (HINT32)ulSize);
	if (retVal < 0)
	{
		PrintError("error write nand address(0x%08X)\n", ulOffset);
		BSU_close(ubi_fd);
		return DI_ERR_ERROR;
	}

	BSU_close(ubi_fd);

	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_Erase(int fd, unsigned long ulOffset)
{
	HAPPY(fd);
	HAPPY(ulOffset);
	// to be compatible with main driver
	return DRV_OK;
}

DRV_Error	DRV_NANDFLASH_Sync(void)
{
	// no need for cfe loader.
	return DRV_OK;
}
#endif //CONFIG_NAND_FLASH

DRV_Error	DRV_FLASH_EraseBlock(int fd, unsigned long ulOffset)
{
#if defined(CONFIG_NOR_FLASH)
	HAPPY(fd);
	HAPPY(ulOffset);
	return DRV_NORFLASH_Erase(fd, ulOffset);
#else
	UNUSED(fd);
	UNUSED(ulOffset);

	return DRV_OK;
#endif
}

DRV_Error DRV_FLASH_Sync(void)
{
	// no need for cfe loader.
	return DRV_OK;
}

DRV_Error DRV_FLASH_Write(int fd, unsigned long ulDestOffset, unsigned char *pucData, unsigned long ulSize)
{
	UNUSED(fd);

#if defined(CONFIG_NOR_FLASH) && defined(CONFIG_NAND_FLASH)
	if (ulDestOffset & 0x80000000)
		return DRV_NANDFLASH_Write(0, ulDestOffset & 0x7FFFFFFF, pucData, ulSize);
	else
		return DRV_NORFLASH_Write(ulDestOffset, pucData, ulSize);
#elif defined(CONFIG_NOR_FLASH) && defined(CONFIG_EMMC_FLASH)
	if (ulDestOffset < 0x00400000)
		return DRV_NORFLASH_Write(ulDestOffset, pucData, ulSize);
	else
		return DRV_EMMC_Write(0, ulDestOffset, pucData, ulSize);
#elif defined(CONFIG_EMMC_FLASH)
	return DRV_EMMC_Write(0, ulDestOffset, pucData, ulSize);
#elif defined(CONFIG_NAND_FLASH)
	return DRV_NANDFLASH_Write(0, ulDestOffset & 0x7FFFFFFF, pucData, ulSize);
#else
#error	"flash is not selected"
#endif
}


DRV_Error DRV_FLASH_Read(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	UNUSED(fd);

#if defined(CONFIG_NOR_FLASH) && defined(CONFIG_NAND_FLASH)
	if (ulSrcOffset & 0x80000000)
		return DRV_NANDFLASH_Read(0, ulSrcOffset & 0x7FFFFFFF, pucBuffer, ulSize);
	else
		return DRV_NORFLASH_Read(ulSrcOffset, pucBuffer, ulSize);
#elif defined(CONFIG_NOR_FLASH) && defined(CONFIG_EMMC_FLASH)
	if (ulSrcOffset < 0x00400000)
		return DRV_NORFLASH_Read(ulSrcOffset, pucBuffer, ulSize);
	else
		return DRV_EMMC_Read(0, ulSrcOffset, pucBuffer, ulSize);
#elif defined(CONFIG_EMMC_FLASH)
	return DRV_EMMC_Read(0, ulSrcOffset, pucBuffer, ulSize);
#elif defined(CONFIG_NAND_FLASH)
	return DRV_NANDFLASH_Read(0, ulSrcOffset & 0x7FFFFFFF, pucBuffer, ulSize);
#else
#error	"flash is not selected"
#endif
}

DRV_Error DRV_FLASH_GetNvramHandle(int *pNvramHandle)
{
	*pNvramHandle = 0;
	return DRV_OK;
}

#if defined(CONFIG_NVRAM_IN_NORFLASH)
DRV_Error DRV_FLASH_GetBlockSize(int handle, HUINT32 *pulBlockSize)
{
	int fd, res, retlen;
	int offset = 0;
	int size = 1024;
	flash_sector_t sector;
	flash_partition partition;

	VK_memset(&sector, 0, sizeof(flash_sector_t));

	/* get flash device name from arbitrary partition */
	res = _drv_Norflash_GetPartition(offset, size, &partition);
	if( res != DRV_OK)
	{
		PrintError("[%s] can't find partition... \n", __FUNCTION__);
		return DRV_ERR;
	}

	/* open */
	fd = BSU_open(partition.device);
	if( fd < 0 )
	{
		PrintError("[%s] can't open %s... \n", __FUNCTION__, partition.device);
		return DRV_ERR;
	}

	/* calculate offset */
	res = BSU_ioctl(fd,
						IOCTL_FLASH_GETSECTORS,
						(HUINT8*)&sector,
						sizeof(flash_sector_t),
						&retlen,
						0);
	if (res < 0)
	{
		PrintError("[%s] can't get sector info of %s, offset 0x%X, size 0x%X ... \n",
														__FUNCTION__,
														partition.device,
														offset,
														size);
		BSU_close(fd);
		return DRV_ERR;
	}

	PrintDebug("[%s] Sector size : 0x%X\n", __FUNCTION__, sector.flash_sector_size);

#if 0
	PrintDebug("[%s] Sector offset : 0x%X\n", __FUNCTION__, sector.flash_sector_offset);
	PrintDebug("[%s] Sector idx : 0x%d\n", __FUNCTION__, sector.flash_sector_idx);
	PrintDebug("[%s] Sector status : 0x%d\n", __FUNCTION__, sector.flash_sector_status);
#endif

	*pulBlockSize = sector.flash_sector_size;

	BSU_close(fd);

	UNUSED(handle);

	return DRV_OK;
}
#endif

#if defined(CONFIG_MIPS)
unsigned long DRV_FLASH_GetFieldOffset( DI_FLASH_FIELD_t field )
{
	int sfd, res;
	flash_info_t info;



	sfd = FlashOpenFieldDevice(field);
	if (sfd < 0)
	{
		PrintError("can not open device!\n");
		return -1;
	}
	if (BSU_ioctl(sfd,IOCTL_FLASH_GETPARTINFO, (unsigned char *) &info, sizeof(info), &res,0) != 0)
	{
		PrintError("cannot get flash info\n");
		return -1;
	}
	BSU_close(sfd);

	return (unsigned long)info.flash_base;
}

unsigned long DRV_FLASH_GetFieldSize( DI_FLASH_FIELD_t field )
{
	int sfd, res;
	flash_info_t info;

	sfd = FlashOpenFieldDevice(field);
	if (sfd < 0)
	{
		PrintError("can not open device!\n");
		return -1;
	}
	if (BSU_ioctl(sfd,IOCTL_FLASH_GETPARTINFO, (unsigned char *) &info, sizeof(info), &res,0) != 0)
	{
		PrintError("cannot get flash info\n");
		return -1;
	}
	BSU_close(sfd);

	return (unsigned long)info.flash_size;
}
#endif

DI_ERR_CODE  DI_FLASH_GetInfo(DI_FLASH_DEVICE_INFO *pstFlashInfo)
{
#if defined (CONFIG_NOR_FLASH)
	*pstFlashInfo = s_diFlashInfo;
	return ERR_OK;
#else
	UNUSED(pstFlashInfo);
	return ERR_FAIL;
#endif
}

DI_ERR_CODE DI_FLASH_Write(HUINT32 ulOffset, HUINT8 *pucData, HUINT32 ulSize)
{
	PrintDebug("[%s] offset : (0x%x), buffer : (0x%x) size: (0x%x)\n",
			__FUNCTION__, ulOffset, pucData, ulSize);
	return DRV_FLASH_Write(0, ulOffset, pucData, ulSize);
}

DI_ERR_CODE DI_FLASH_Read(HUINT32 ulOffset, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	PrintDebug("[%s] offset : (0x%x), buffer : (0x%x) size: (0x%x)\n",
			__FUNCTION__, ulOffset, pucBuffer, ulSize);
	return DRV_FLASH_Read(0, ulOffset, pucBuffer, ulSize);
}

DI_ERR_CODE DI_FLASH_EraseBlock(HUINT32 ulBlockNum)
{
	PrintDebug("[%s] ulBlockNum : (0x%x)\n", __FUNCTION__, ulBlockNum);
	return DRV_FLASH_EraseBlock(0, ulBlockNum);
}

DRV_Error DRV_FLASH_Init(void)
{
#if defined(CONFIG_NOR_FLASH)
	int err;
	err = DRV_NOR_Init();
	if (err != VK_OK)
	{
		PrintError("[%s] DRV_NOR_Init Error!!. (%d)\n", __FUNCTION__, err);
		return DI_ERR_ERROR;
	}
#endif
	return DI_ERR_OK;
}

#if defined(CONFIG_NAND_FLASH)
DI_ERR_CODE DI_NANDFLASH_GetInfo(DI_NANDFLASH_DEVICE_INFO *pNandDevInfo)
{
	VK_MEM_Memset(pNandDevInfo, 0, sizeof(DI_NANDFLASH_DEVICE_INFO));
	pNandDevInfo->ulFlashBootBlockSize = FLASH_LOADER_AREA_SIZE;
	pNandDevInfo->stUbiInfo.ulLebSize = LOGICAL_BLOCK_SIZE;

	return DI_ERR_OK;
}
#endif
