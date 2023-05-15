/*
 * (c) 2011-2012 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
//#include <octo_common.h>
#include <htype.h>
#include <di_err.h>
#include <di_flash.h>
#include <di_eeprom.h>
#include <di_uart.h>
#include <di_crypt.h>
#include <drv_flash.h>
#include <nvram_map.h>

#include <pal_sys.h>
#include <pal_param.h>

//#include "_svc_cas_mgr_nvram.h"


#include "tir_platform.h"
//#include "ir_loader_v5.h"
#include "irv5_dev.h"

#include "cas_ir_module_v5.h"
#include "di_nvram.h"
#include "tir_nvr.h"

#define _______________ACCESS_NVRAM____________________

#if defined(CONFIG_DEBUG)
#define PrintDebug	printf
#define PrintErr	printf
#define TraceNvr	printf("TRANCE: %s, %d\n", __FUNCTION__, __LINE__ )
#else
#define PrintDebug(...)
#define PrintErr(...)
#endif

#define PATTERN_SIZE (0x100)
#define IR_FLASH_NAND_ADDRESS_MASK					0x80000000

Boot_Block_Control_Block g_tIr_bbcb;
extern DI_ERR_CODE	DI_NANDFLASH_Write(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize);


HUINT32 TIR_FLASH_LOADER_Init(void)
{
	if(HIR_OK !=	TIR_MOD_Read_BBCB(&g_tIr_bbcb) )
	{
		printf("bbcb read fail\n") ;
		return HIR_NOT_OK;
	}
#if 0
	if(HIR_OK !=	TIR_FLASH_Read_CPCB(g_tIr_cpcb) )
	{
		printf("cpcb or spcb read fail \n" );
		return HIR_NOT_OK;
	}
#endif
	return HIR_OK;
}

HIR_ERR TIR_MOD_Read_BBCB(Boot_Block_Control_Block *pBBCB)
{
	HINT32	ret;
	HINT16	mod_id;
	HINT32 payload_length=0;
	HCHAR fileName[80]={0,};
	HUINT32	result=ERR_OK;
	lc_bbcb_st bbcb;
	lc_bool flagCRCmatched;
	lc_result lc_ret;

	HxLOG_Print("%s\n", __FUNCTION__ );

#if 1
	lc_ret = BBCB_GetBBCB(&bbcb, &flagCRCmatched);
	dbgprt1("BBCB_GetBBCB ret %d\n", lc_ret );

	*pBBCB = bbcb; // obj copy.
#else
	payload_length = sizeof(bbcb);
	mod_id = MODULE_DRV_ID_BBCB;
	sprintf(fileName, NVRAM_CAS_IRV5_MPL_FILENAME_PFX"%d", mod_id);
	dbgprt3("mod_id = %d, payload_length = %d, NVR filename = %s\n", mod_id, payload_length, fileName);

	ret = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RW, fileName, 0, (void *)&bbcb, payload_length);
	if (ERR_OK != ret)
	{
		printf("%s, DI_NVRAM_Read %08X", __FUNCTION__, result  );
		//HxLOG_CRITICAL("");
		return HIR_NOT_OK;
	}

	dbgdump("bbcb", &bbcb, payload_length);

	memcpy(pBBCB, &bbcb, sizeof(bbcb) );
#endif

	dbgprt0("***** v5 BBCB *****\n");
	dbgprt3("OUI[3] : [%02x][%02x][%02x]\n", pBBCB->OUI[0], pBBCB->OUI[1], pBBCB->OUI[2] );
	dbgprt1("Manufacture Id : [%04x]\n", pBBCB->manufacturer_id );
	dbgprt1("HW_Version : [%04x]\n", pBBCB->hardware_version);
	dbgprt1("LDR_Version_Major : %d\n", pBBCB->loader_major_version);
	dbgprt1("LDR_Version_Minor : %d\n", pBBCB->loader_minor_version);
	dbgprt1("LDR_version_build : %d\n", pBBCB->loader_version_buildnumber );
	dbgprt1("Boot Check Alg. : %d\n", pBBCB->boot_check_algorithm);
	dbgprt4("SerialNumber : [%02x %02x %02x %02x]\n", pBBCB->serial_number[0], pBBCB->serial_number[1], pBBCB->serial_number[2], pBBCB->serial_number[3] );
	dbgprt4("RandomNumber : [%02x %02x %02x %02x]\n", pBBCB->random_number[0], pBBCB->random_number[1], pBBCB->random_number[2], pBBCB->random_number[3] );
	dbgprt1("CRC32 : [%08x]\n", pBBCB->crc32 );
	dbgprt0("*********************\n");

	return HIR_OK;

}





extern DI_ERR_CODE	DI_NANDFLASH_Read(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize);
HIR_ERR TIR_FLASH_Read(HUINT32 ulOffset, HUINT8 * pData, HUINT32 ulSize)
{
	HUINT32	result;
	PrintDebug("%s\n", __FUNCTION__);

	result = DI_NANDFLASH_Read( ulOffset , pData, ulSize );
	if (ERR_OK != result)
	{
		PrintErr("%s,  DI_FLASH_Read %08X \n",__FUNCTION__,  result );

		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HIR_ERR TIR_FLASH_Write(HUINT32 ulOffset, HUINT8 * pData, HUINT32 ulSize)
{
	HUINT32	result = 0;

//	result = DI_NANDFLASH_Write( ulOffset , pData, ulSize );
//	ulOffset |= IR_FLASH_NAND_ADDRESS_MASK;

	result = DI_NANDFLASH_Write( ulOffset , pData, ulSize );
	if (DI_ERR_OK != result)
	{
		PrintErr("%s, DI_FLASH_Write Failed.(%d) (offset [%x], size [%x]) \n", __FUNCTION__, result, ulOffset, ulSize );

		return HIR_NOT_OK;
	}

//	PrintDebug("%s, DI_FLASH_Write ok. (offset [%x], size [%x]) \n", __FUNCTION__, ulOffset, ulSize );

	return HIR_OK;
}


HUINT32	TIR_MOD_CountPattern(HUINT16 usModule_id, HUINT32 *pulNumPattern)
{
	DI_ERR_CODE di_err;
	HUINT32	ulFlashOffset;
	unsigned long	i;
	unsigned char* pucFlashBuf;
	unsigned char* pucPatternCmp;
	unsigned long module_max_size;
	int numberOfTryingCnt;
	unsigned long numberOfMatchedPatternCnt=0;

	PrintDebug("%s, usModule_id = %d\n", __FUNCTION__, usModule_id);

	if( usModule_id < HX_MOD_ID_START)
	{
		PrintErr("it's below Start module[%x]\n", usModule_id);
		return HIR_NOT_OK;
	}

	if(usModule_id > HX_MOD_ID_LAST)
	{
		PrintErr("it's over last module[%x]\n", usModule_id);
		return HIR_NOT_OK;
	}

	switch(usModule_id)
	{
	case HX_MOD_ID_20_KERNEL:
		module_max_size = HX_MOD_ID_20_KERNEL_SIZE_MAX;
		break;
	case HX_MOD_ID_21_ROOTFS:
		module_max_size = HX_MOD_ID_21_ROOTFS_SIZE_MAX;
		break;
	}
	pucFlashBuf = VK_MEM_Alloc(module_max_size);
	if(NULL== pucFlashBuf)
	{
		dbgerr0("Alloc failed\n");
		return HIR_NOT_OK;
	}

	TIR_MOD_ConvertFlashOffset(usModule_id, &ulFlashOffset);

	di_err = TIR_FLASH_Read( ulFlashOffset , pucFlashBuf , module_max_size );
	if( di_err )
	{
		PrintErr("[dl_FLASH_Read ERROR : %d]\n", di_err);
		VK_MEM_Free(pucFlashBuf);
		return HIR_INVALID_MEMORY;
	}

	/* Make compare value */
	pucPatternCmp = VK_MEM_Alloc(PATTERN_SIZE);
	if(NULL== pucPatternCmp)
	{
		dbgerr0("Alloc failed\n");
		return HIR_NOT_OK;
	}
	for(i =0; i< PATTERN_SIZE; i++)
	{
		pucPatternCmp[i] = i;
	}

	numberOfTryingCnt = module_max_size / PATTERN_SIZE;
	dbgprt4("module_max_size = %d[%x], numberOfTryingCnt = %d[%x]\n",  \
		module_max_size, module_max_size, numberOfTryingCnt, numberOfTryingCnt );

	for(i = 0; i < numberOfTryingCnt ; i++)
	{
		unsigned char *p;
		p = pucFlashBuf + (i*PATTERN_SIZE);
		if(0 == memcmp( p , pucPatternCmp, PATTERN_SIZE ) )
		{
			numberOfMatchedPatternCnt++;
		}
	}

	dbgprt2("numberOfMatchedPatternCnt = %d[%x]\n", numberOfMatchedPatternCnt, numberOfMatchedPatternCnt );
	*pulNumPattern = numberOfMatchedPatternCnt;

	VK_MEM_Free(pucPatternCmp);
	VK_MEM_Free(pucFlashBuf);

	return HIR_OK;



}

HUINT32	TIR_MOD_WritePattern(HUINT16 usModule_id)
{
	DI_ERR_CODE di_err;
	HUINT32	ulFlashOffset;
	unsigned long	i;
	unsigned char* pucPatternCmp;
	unsigned long module_max_size;
	int numberOfTryingCnt;
	unsigned long numberOfMatchedPatternCnt=0;

	PrintDebug("%s, usModule_id = %d\n", __FUNCTION__, usModule_id);

	if( usModule_id < HX_MOD_ID_START)
	{
		PrintErr("it's below Start module[%x]\n", usModule_id);
		return HIR_NOT_OK;
	}

	if(usModule_id > HX_MOD_ID_LAST)
	{
		PrintErr("it's over last module[%x]\n", usModule_id);
		return HIR_NOT_OK;
	}

	switch(usModule_id)
	{
	case HX_MOD_ID_20_KERNEL:
		module_max_size = HX_MOD_ID_20_KERNEL_SIZE_MAX;
		break;
	case HX_MOD_ID_21_ROOTFS:
		module_max_size = HX_MOD_ID_21_ROOTFS_SIZE_MAX;
		break;
	}


	TIR_MOD_ConvertFlashOffset(usModule_id, &ulFlashOffset);

	/* Make pattern */
	pucPatternCmp = VK_MEM_Alloc(PATTERN_SIZE);
	if(NULL== pucPatternCmp)
	{
		dbgerr0("Alloc failed\n");
		return HIR_NOT_OK;
	}
	for(i =0; i< PATTERN_SIZE; i++)
	{
		pucPatternCmp[i] = i;
	}

	numberOfTryingCnt = module_max_size / PATTERN_SIZE;
	dbgprt4("module_max_size = %d[%x], numberOfTryingCnt = %d[%x]\n",  \
		module_max_size, module_max_size, numberOfTryingCnt, numberOfTryingCnt );

	for(i = 0; i < numberOfTryingCnt ; i++)
	{
		unsigned long offset=0;
		offset = ulFlashOffset + (i * PATTERN_SIZE);

		di_err = TIR_FLASH_Write( offset , pucPatternCmp , PATTERN_SIZE );
		if( di_err )
		{
			PrintErr("[TIR_FLASH_Write ERROR : %d]\n", di_err);
			return HIR_INVALID_MEMORY;
		}
	}

	VK_MEM_Free(pucPatternCmp);
	return HIR_OK;
}

HUINT32	TIR_MOD_ModifyLastBit(HUINT16 usModule_id)
{
	DI_ERR_CODE di_err;
	HUINT32	ulFlashOffset;
	unsigned long	i;
	unsigned char* pucPatternBuf;
	unsigned char* pucPatternCmp;
	unsigned long module_max_size;
	int numberOfTryingCnt;
	unsigned long numberOfMatchedPatternCnt=0;

	PrintDebug("%s, usModule_id = %d\n", __FUNCTION__, usModule_id);

	if( usModule_id < HX_MOD_ID_START)
	{
		PrintErr("it's below Start module[%x]\n", usModule_id);
		return HIR_NOT_OK;
	}

	if(usModule_id > HX_MOD_ID_LAST)
	{
		PrintErr("it's over last module[%x]\n", usModule_id);
		return HIR_NOT_OK;
	}

	switch(usModule_id)
	{
	case HX_MOD_ID_20_KERNEL:
		module_max_size = HX_MOD_ID_20_KERNEL_SIZE_MAX;
		break;
	case HX_MOD_ID_21_ROOTFS:
		module_max_size = HX_MOD_ID_21_ROOTFS_SIZE_MAX;
		break;
	}
	pucPatternBuf = VK_MEM_Alloc(module_max_size);
	if(NULL== pucPatternBuf)
	{
		dbgerr0("Alloc failed\n");
		return HIR_NOT_OK;
	}
	TIR_MOD_ConvertFlashOffset(usModule_id, &ulFlashOffset);

	di_err = TIR_FLASH_Read( ulFlashOffset , pucPatternBuf , module_max_size );
	if( di_err )
	{
		PrintErr("[dl_FLASH_Read ERROR : %d]\n", di_err);
		VK_MEM_Free(pucPatternBuf);
		return HIR_INVALID_MEMORY;
	}

	pucPatternBuf[module_max_size-1] ^= 0x01;

	di_err = TIR_FLASH_Write( ulFlashOffset , pucPatternBuf , module_max_size );
	if( di_err )
	{
		PrintErr("[TIR_FLASH_Write ERROR : %d]\n", di_err);
		return HIR_INVALID_MEMORY;
	}

	VK_MEM_Free(pucPatternBuf);

	return HIR_OK;

}

HUINT32	TIR_MOD_Version(HUINT16 usModule_id, HUINT16 *pusModule_ver)
{
	lc_module_info_header stModuleInfoHeader;
	lc_buffer_st ByteModuleInfoHeader={NULL, 0};

	if(LC_SUCCESS == LC_ReadAndParseModuleInfoHeader(usModule_id, &stModuleInfoHeader, &ByteModuleInfoHeader) )
	{
		*pusModule_ver = stModuleInfoHeader.module_version;
		VK_MEM_Free(ByteModuleInfoHeader.bytes);
		return HIR_OK;
	}
	return HIR_NOT_OK;
}

HUINT32	TIR_MOD_Erase(HUINT16 usModule_id)
{
	DI_ERR_CODE	nErrCode;
	HUINT32	ulFlashOffset;
	unsigned long module_max_size;
	unsigned char *p;

	PrintDebug("%s, usModule_id = %d\n", __FUNCTION__, usModule_id);

	TIR_MOD_ConvertFlashOffset(usModule_id, &ulFlashOffset);

	switch(usModule_id)
	{
	case HX_MOD_ID_20_KERNEL:
		module_max_size = HX_MOD_ID_20_KERNEL_SIZE_MAX;
		break;
	case HX_MOD_ID_21_ROOTFS:
		module_max_size = HX_MOD_ID_21_ROOTFS_SIZE_MAX;
		break;
	}

	p =	VK_MEM_Alloc(module_max_size);
	if(NULL== p)
	{
		dbgerr0("Alloc failed\n");
		return HIR_NOT_OK;
	}

	memset(p, 0x00, module_max_size );

	nErrCode = TIR_FLASH_Write( ulFlashOffset, p  , module_max_size);
	if (nErrCode)
	{
		PrintErr("[TIR_FLASH_Write ERROR : %d]\n", nErrCode);
		VK_MEM_Free(p);
		return HIR_NOT_OK;
	}
	VK_MEM_Free(p);
	return HIR_OK;
}

HUINT32	TIR_MOD_CountModuleInPlatform(HUINT16 *pusModuleCount)
{
	*pusModuleCount = HX_MOD_TOTAL_COUNT;

	return HIR_OK;

}

void TIR_MOD_ConvertFlashOffset(HUINT16 usMod_id, HUINT32 * pulOffsetOut )
{
	dbgprt1("usMod_id = [%x]\n", usMod_id);

	switch(usMod_id)
	{
	case HX_MOD_ID_20_KERNEL:
		*pulOffsetOut = HX_MOD_ID_20_KERNEL_OFFSET;
		break;
	case HX_MOD_ID_21_ROOTFS:
		*pulOffsetOut = HX_MOD_ID_21_ROOTFS_OFFSET;
		break;
	default:
		dbgassert(0);
		break;
	}

	PrintDebug("%s, mod id[%x], *pulOffsetOut = [%x]\n", __FUNCTION__, usMod_id, *pulOffsetOut );

}


HINT32 TIR_MEM_Manipulation_Erase( TIRNVR_Type NvrType)
{
	unsigned char *pucData;
	HINT32 ret_result = 0;

	switch(NvrType)
	{
		case TIRNVR_LOADER:
			/* Loader partition */
			ret_result = LoaderCoreSPI_Module_Delete(MODULE_DRV_ID_LOADER_PT);
			if(LC_SUCCESS != ret_result )
			{
				PrintErr("[TIR_MEM_Manipulation_Erase MODULE_DRV_ID_LOADER_PT ERROR : %d]\n", ret_result);
			}
			break;

		case TIRNVR_IRDETO:
			/* Irdeto partition */
			ret_result = LoaderCoreSPI_Module_Delete(MODULE_DRV_ID_UK);
			if(LC_SUCCESS != ret_result )
			{
				PrintErr("[TIR_MEM_Manipulation_Erase MODULE_DRV_ID_UK ERROR : %d]\n", ret_result);
			}
			ret_result = LoaderCoreSPI_Module_Delete(MODULE_DRV_ID_VARIANT);
			if(LC_SUCCESS != ret_result )
			{
				PrintErr("[TIR_MEM_Manipulation_Erase MODULE_DRV_ID_VARIANT ERROR : %d]\n", ret_result);
			}

			break;

		case TIRNVR_CA:
			/* CA partition */
			pucData = VK_MEM_Alloc( IR_PARTITION_CA_SIZE );
			memset( pucData , 0x00 , IR_PARTITION_CA_SIZE );
			ret_result = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, IRDETO_CA_PARTITION, 0, pucData, IR_PARTITION_CA_SIZE);
			if (ERR_OK != ret_result)
			{
				PrintErr("[TIR_MEM_Manipulation_Erase CA ERROR : %d]\n", ret_result);
			}
			VK_MEM_Free(pucData);
			break;

		case TIRNVR_ENTIRE_NVRAM:
			/* IRDETO / CA / LOADER Field Erase */
			/* Irdeto partition */
			ret_result = LoaderCoreSPI_Module_Delete(MODULE_DRV_ID_UK);
			if(LC_SUCCESS != ret_result )
			{
				PrintErr("[TIR_MEM_Manipulation_Erase MODULE_DRV_ID_UK ERROR : %d]\n", ret_result);
			}
			ret_result = LoaderCoreSPI_Module_Delete(MODULE_DRV_ID_VARIANT);
			if(LC_SUCCESS != ret_result )
			{
				PrintErr("[TIR_MEM_Manipulation_Erase MODULE_DRV_ID_VARIANT ERROR : %d]\n", ret_result);
			}

			/* Loader partition */
			ret_result = LoaderCoreSPI_Module_Delete(MODULE_DRV_ID_LOADER_PT);
			if(LC_SUCCESS != ret_result )
			{
				PrintErr("[TIR_MEM_Manipulation_Erase MODULE_DRV_ID_LOADER_PT ERROR : %d]\n", ret_result);
			}

			/* CA partition */
			pucData = VK_MEM_Alloc( IR_PARTITION_CA_SIZE );
			memset( pucData , 0x00 , IR_PARTITION_CA_SIZE );
			ret_result = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, IRDETO_CA_PARTITION, 0, pucData, IR_PARTITION_CA_SIZE);
			if (ERR_OK != ret_result)
			{
				PrintErr("[TIR_MEM_Manipulation_Erase CA ERROR : %d]\n", ret_result);
			}
			VK_MEM_Free(pucData);
			break;

		case TIRNVR_RESET_TO_DEFAULT:
			// TODO: jichoi
			/*
			Selection of option ¡®Reset To Defaults¡¯ shall erase necessary data to restore the decoder to default state.
			The data includes, but is not limited to, DSN, all NVRAM Partitions, module images and module info headers.
			*/

			break;

		default:
			PrintDebug("%s, %d, what??? %d\n", __FILE__, __LINE__, NvrType);
			break;
	}

	return (DI_ERR_OK != ret_result )? 1: 0;

}

