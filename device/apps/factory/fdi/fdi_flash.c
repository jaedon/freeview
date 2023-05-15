/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fdi_common.h"
#include "fdi_flash.h"

#include "htype.h"
#include "di_err.h"
#include "di_flash.h"
#include "drv_err.h"

#include "vkernel.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
#if !defined(FACTORY_USE_DI20) || (defined(FACTORY_USE_DI20) && defined(FACTORY_USE_NOR_FLASH))
HUINT32 g_ulNumOfBlocks;
HUINT32 g_ulBlockSize;

extern DRV_Error DRV_FLASH_GetCapabilities(HUINT32 *pulBlockSize, HUINT32* pulBlockNumber);
/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_FLASH_Open(void)
{
	int i;
	DI_ERR_CODE eDiErr;
	
	DRV_FLASH_GetCapabilities(&g_ulBlockSize, &g_ulNumOfBlocks);
	FDI_PRINT(0, ("[FDI_FLASH_Open] : Number of Blocks=%x, Block Size=%x\n", g_ulNumOfBlocks, g_ulBlockSize));

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_FLASH_Write(unsigned long ulAddr, unsigned char *pucBuf, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;
	
	eDiErr = DI_FLASH_Write(ulAddr, pucBuf, ulSize);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_FLASH_Write] : DI_FLASH_Write Error\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;	
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_FLASH_WriteAfterErase(unsigned long ulAddr, unsigned char *pucBuf, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;
	unsigned long ulStartBlock, ulEndBlock, ulNumOfWriteBlocks, ulBlockIdx, ulDataStartOffset, ulDataEndOffset, ulWriteSize;
	unsigned char *pucBlockBuf = NULL, *pucComBuf = NULL;
	FDI_ERR_CODE	ret = FDI_ERR;

	ulStartBlock = ulAddr/g_ulBlockSize;
	ulEndBlock = (ulAddr+ulSize-1)/g_ulBlockSize;
	ulNumOfWriteBlocks = ulEndBlock - ulStartBlock + 1;
		
	/* malloc a block */	
	pucBlockBuf = (unsigned char*)FVK_MEM_Alloc(g_ulBlockSize);
	if( pucBlockBuf == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_FLASH_WriteAfterErase] : FVK_MEM_Alloc Error\n"));
		goto done;
	}
	pucComBuf = (unsigned char*)FVK_MEM_Alloc(g_ulBlockSize);
	if( pucComBuf == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_FLASH_WriteAfterErase] : FVK_MEM_Alloc Error\n"));
		goto done;
	}
	FVK_MEM_Set(pucComBuf, 0xff, g_ulBlockSize);
	
	/* loop blocks */
	for( ulBlockIdx = ulStartBlock; ulBlockIdx <= ulEndBlock; ulBlockIdx++, pucBuf += ulWriteSize )
	{
		/* get write size */
		ulDataStartOffset = (ulBlockIdx == ulStartBlock) ?  ulAddr-(ulStartBlock*g_ulBlockSize) : 0;
		ulDataEndOffset = (ulBlockIdx == ulEndBlock ) ? (ulAddr+ulSize)%g_ulBlockSize : g_ulBlockSize;
		if( ulDataEndOffset == 0 ) ulDataEndOffset = g_ulBlockSize;
		ulWriteSize = ulDataEndOffset - ulDataStartOffset;
			
		/* read a block */
		eDiErr = DI_FLASH_Read(ulBlockIdx*g_ulBlockSize, pucBlockBuf, g_ulBlockSize);
		if( eDiErr != DI_ERR_OK )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_FLASH_WriteAfterErase] : DI_FLASH_Read Error\n"));
			goto done;
		}
		
		/* data compare: if same data, skip */
		if( FVK_MEM_Compare(pucBuf, pucBlockBuf+ulDataStartOffset, ulWriteSize) == 0 )
		{
			FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_FLASH_WriteAfterErase] : same data!\n"));
			continue;
		}
		
		/* if already erased, skip erase process */
		if( FVK_MEM_Compare(pucComBuf, pucBlockBuf+ulDataStartOffset, ulWriteSize) != 0 )
		{
			/* erase block */
			eDiErr = DI_FLASH_EraseBlock(ulBlockIdx);
			if( eDiErr != DI_ERR_OK )
			{
				FDI_PRINT(FDI_PRT_ERROR, ("[FDI_FLASH_WriteAfterErase] : DI_FLASH_EraseBlock(%x) Error\n", ulBlockIdx));
				goto done;
			}
			/* write a whole block */
			FVK_MEM_Copy(pucBlockBuf+ulDataStartOffset, pucBuf, ulWriteSize);
			eDiErr = DI_FLASH_Write(ulBlockIdx*g_ulBlockSize, pucBlockBuf, g_ulBlockSize);
			if( eDiErr != DI_ERR_OK )
			{
				FDI_PRINT(FDI_PRT_ERROR, ("[FDI_FLASH_WriteAfterErase] : DI_FLASH_Write Error\n"));
				goto done;
			}
		}
		else
		{
			/* write data only */
			FVK_MEM_Copy(pucBlockBuf+ulDataStartOffset, pucBuf, ulWriteSize);
			eDiErr = DI_FLASH_Write(ulBlockIdx*g_ulBlockSize+ulDataStartOffset, 
										pucBlockBuf+ulDataStartOffset, 
										ulWriteSize);
			if( eDiErr != DI_ERR_OK )
			{
				FDI_PRINT(FDI_PRT_ERROR, ("[FDI_FLASH_WriteAfterErase] : DI_FLASH_Write Error\n"));
				goto done;
			}
		}
	}

	ret = FDI_NO_ERR;
done:
	if (pucBlockBuf)
		FVK_MEM_Free(pucBlockBuf);
	if (pucComBuf)
		FVK_MEM_Free(pucComBuf);
	
	return ret;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_FLASH_Read(unsigned long ulAddr, unsigned char *pucBuf, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;
	
	eDiErr = DI_FLASH_Read(ulAddr, pucBuf, ulSize);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_FLASH_Read] : DI_FLASH_Read Error\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;		
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_FLASH_EraseBlock(unsigned long ulBlockNumber)
{
	DI_ERR_CODE eDiErr;
	
	eDiErr = DI_FLASH_EraseBlock(ulBlockNumber);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_FLASH_EraseBlock] : DI_FLASH_EraseBlock Error\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}
#endif

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_NANDFLASH_Read(unsigned long ulAddr, unsigned char *pucBuf, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	eDiErr = DI_NANDFLASH_Read(ulAddr, pucBuf, ulSize);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_NANDFLASH_Read] : DRV_NANDFLASH_DIAG_Read Error\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;		
}

extern DI_ERR_CODE	DI_NANDFLASH_ReadForCRC(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_NANDFLASH_ReadForCRC(unsigned long ulAddr, unsigned char *pucBuf, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	eDiErr = DI_NANDFLASH_ReadForCRC(ulAddr, pucBuf, ulSize);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_NANDFLASH_ReadForCRC] : DI_NANDFLASH_ReadForCRC Error\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_NANDFLASH_Write(unsigned long ulAddr, unsigned char *pucBuf, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	eDiErr = DI_NANDFLASH_Write(ulAddr, pucBuf, ulSize);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_NANDFLASH_Write] : DRV_NANDFLASH_DIAG_Write Error\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;	
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_NANDFLASH_Sync(void)
{
	DI_ERR_CODE eDiErr;

	eDiErr = DI_NANDFLASH_Sync();
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_NANDFLASH_Write] : DRV_NANDFLASH_DIAG_Sync Error\n"));
		return FDI_ERR;
	}
	VK_SYSTEM_Command("echo 3 > /proc/sys/vm/drop_caches");

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_FLASH_GetInfo(unsigned long *pulNumOfBlock, unsigned long *pulBlokSize)
{
#if !defined(FACTORY_USE_DI20) || (defined(FACTORY_USE_DI20) && defined(FACTORY_USE_NOR_FLASH))
    *pulNumOfBlock = g_ulNumOfBlocks;
    *pulBlokSize = g_ulBlockSize;
#else
	DI_NANDFLASH_DEVICE_INFO	tInfo;

	(void)DI_NANDFLASH_GetInfo(&tInfo);
	*pulBlokSize = tInfo.stUbiInfo.ulLebSize;
	*pulNumOfBlock = 0;	
#endif

	return FDI_NO_ERR;
}

DRV_Error DRV_NANDFLASH_UBI_IsBad(HUINT32 ulAddress, HUINT32 ulSize, HINT32 *piBadStatus);
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_NANDFLASH_CheckBadBlock(unsigned long ulAddr,unsigned long ulSize, int *piBadStatus)
{
	DRV_Error	eDrvErr;
	int status = 0;
	eDrvErr = DRV_NANDFLASH_UBI_IsBad(ulAddr,ulSize,&status);
	if( eDrvErr != DRV_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_NANDFLASH_CheckBadBlock] : FDI_NANDFLASH_CheckBadBlock Error\n"));
		return FDI_ERR;
	}

	*piBadStatus = status;

	return FDI_NO_ERR;
}

#if defined(FACTORY_USE_DI20) && defined(FACTORY_USE_EMMC_FLASH)
FDI_ERR_CODE FDI_EMMC_Read(HUINT32 address, HUINT8 *data, HUINT32 data_size)
{
	DI_ERR_CODE eDiErr;

	if (!data || data_size == 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] Error. invalid parameters. data(%p), data_size(%d)\n", data, data_size));
		return FDI_ERR;
	}

	eDiErr = DI_EMMC_Read(address, data, data_size);
	if (eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_EMMC_Write(HUINT32 address, HUINT8 *data, HUINT32 data_size)
{
	DI_ERR_CODE eDiErr;

	if (!data || data_size == 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] Error. invalid parameters. data(%p), data_size(%d)\n", data, data_size));
		return FDI_ERR;
	}

	eDiErr = DI_EMMC_Write(address, data, data_size);
	if (eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}
#endif
