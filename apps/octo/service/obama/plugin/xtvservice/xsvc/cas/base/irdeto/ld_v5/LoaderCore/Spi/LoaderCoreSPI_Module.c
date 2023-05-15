//#include "LoaderCore_Types.h"
//#include "LoaderCorePrivate.h"
#include "LoaderCoreSPI_Module.h"
#include "htype.h"
//#include "cas_ir_util_v5.h"
#include "cas_ir_module_v5.h"
//#include "uocto.h"
//#include "pal_nvram.h"
#include "di_nvram.h"
#include "irv5_dev.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PAL_FLASH_Read		DI_FLASH_Read
#define PAL_FLASH_Write		DI_FLASH_Write
#define PAL_NVRAM_Read		DI_NVRAM_Read
#define PAL_NVRAM_Write		DI_NVRAM_Write
#define PAL_NVRAM_Remove		DI_NVRAM_Remove
#define PAL_NVRAM_PARTITION_RW DI_NVRAM_FS_PARTITION_RW
#define PAL_NVRAM_PARTITION_RO DI_NVRAM_FS_PARTITION_RO



extern hx_ir_v5_mod_master g_irdeto_module_master;


static void loc_ir_GetModuleHeaderNVRAMFileName(lc_uint16 mod_id, lc_char * inout_name)
{
	dbgassert(inout_name);

	sprintf(inout_name, "%s%d", NVRAM_CAS_IRV5_MIH_FILENAME_PFX, mod_id );
}


HERROR	ir_spi_ReadMH( HINT32 mod_id, HUINT8 *pucData ,HINT32 mih_length )
{
	char fileName[128] = {0,};
	int ret = ERR_OK;

	dbgprt2("mod_id = %d, mih_length = %d\n", mod_id, mih_length);

	switch(mod_id)
	{
	case MODULE_DRV_ID_BBCB:
		loc_ir_GetModuleHeaderNVRAMFileName(mod_id, fileName);
		ret = PAL_NVRAM_Read(PAL_NVRAM_PARTITION_RO, fileName, 0, pucData, mih_length);
		break;

	case HX_MOD_ID_20_KERNEL:
	case HX_MOD_ID_21_ROOTFS:

	case MODULE_DRV_ID_UK:
	case MODULE_DRV_ID_LOADER_CORE:
	case MODULE_DRV_ID_LOADER_PT:
	case MODULE_DRV_ID_VARIANT:
		loc_ir_GetModuleHeaderNVRAMFileName(mod_id, fileName);
		ret = PAL_NVRAM_Read(PAL_NVRAM_PARTITION_RW, fileName, 0, pucData, mih_length);
		break;

	default :
		dbgassert(0);
		ret = ERR_FAIL;
		break;
	}

	if(ret)
	{
		dbgerr2("mod id[%x] Read error %d\n", mod_id, ret);
	}
	return ret;
}

HERROR	ir_spi_WriteMH(HINT32 mod_id, HUINT8 *pucData, HINT32 mih_length )
{
	char fileName[128] = {0,};
	unsigned char *p;
	int ret = ERR_OK;

	dbgprt2("mod_id = %d, mih_length = %d\n", mod_id, mih_length);

	p = pucData;

	loc_ir_GetModuleHeaderNVRAMFileName(mod_id, fileName);

	switch(mod_id)
	{
	case MODULE_DRV_ID_LOADER_CORE:
		dbgassert(0);
		break;

	case HX_MOD_ID_20_KERNEL:
	case HX_MOD_ID_21_ROOTFS:
		dbgwarn1("access Hx mod [%x]\n", mod_id);
	case MODULE_DRV_ID_UK:
	case MODULE_DRV_ID_BBCB:
	case MODULE_DRV_ID_LOADER_PT:
	case MODULE_DRV_ID_VARIANT:
		ret = PAL_NVRAM_Write(PAL_NVRAM_PARTITION_RW, fileName, 0, p, mih_length);
		break;

	default :
		dbgassert(0);
		ret = ERR_FAIL;
		break;
	}

	if(ret)
	{
		dbgerr2("mod id[%x] write error %d\n", mod_id, ret);
	}
	return ret;
}

HERROR	ir_spi_ReadNvramMP(HINT32 mod_id, HUINT8 *pucData, HINT32 payload_length)
{
	char fileName[128] = {0,};
	int ret = ERR_OK;

	sprintf(fileName, NVRAM_CAS_IRV5_MPL_FILENAME_PFX"%d", mod_id);
	dbgprt3("mod_id = %d, payload_length = %d, NVR filename = %s\n", mod_id, payload_length, fileName);

	switch(mod_id)
	{
	case MODULE_DRV_ID_BBCB:
		ret = PAL_NVRAM_Read(PAL_NVRAM_PARTITION_RO, fileName, 0, pucData, payload_length);
		break;

	case MODULE_DRV_ID_UK:
	case MODULE_DRV_ID_LOADER_PT:
	case MODULE_DRV_ID_VARIANT:
		ret = PAL_NVRAM_Read(PAL_NVRAM_PARTITION_RW, fileName, 0, pucData, payload_length);
//		dbgdump("read payload", p, l);
		break;
	default :
		dbgassert(0);
		ret = ERR_FAIL;
		break;
	}

	if(ret)
	{
		dbgerr2("mod id[%x] read error %d\n", mod_id, ret);
	}
	return ret;
}

HERROR	ir_spi_WriteNvramMP(HINT32 mod_id, HUINT8 *pucData, HINT32 payload_length)
{
	char fileName[128] = {0,};
	int ret = ERR_OK;

	sprintf(fileName, NVRAM_CAS_IRV5_MPL_FILENAME_PFX"%d", mod_id);
	dbgprt3("mod_id = %d, payload_length = %d, NVR filename = %s\n", mod_id, payload_length, fileName);

	switch(mod_id)
	{
	case MODULE_DRV_ID_UK:
	case MODULE_DRV_ID_BBCB:
	case MODULE_DRV_ID_LOADER_PT:
	case MODULE_DRV_ID_VARIANT:
		ret = PAL_NVRAM_Write(PAL_NVRAM_PARTITION_RW, fileName, 0, pucData, payload_length);
		break;
	default :
		dbgassert(0);
		ret = ERR_FAIL;
		break;
	}

	if(ret)
	{
		dbgerr2("mod id[%x] write error %d\n", mod_id, ret);
	}
	return ret;
}

HERROR	ir_spi_DeleteModule(HINT32 mod_id)
{
	char fileName[128] = {0,};
	HERROR ret = ERR_OK;

	/* Erase Module Payload */
	switch(mod_id)
	{
	case MODULE_DRV_ID_UK:
	case MODULE_DRV_ID_BBCB:
	case MODULE_DRV_ID_LOADER_PT:
	case MODULE_DRV_ID_VARIANT:
		//erase payload file.
		sprintf(fileName, NVRAM_CAS_IRV5_MPL_FILENAME_PFX"%d", mod_id);
		ret = PAL_NVRAM_Remove(PAL_NVRAM_PARTITION_RW, fileName);
		dbgwarn3("delete:irdeto mod-payload [%x]:NVRAM: %d : (%s)\n", mod_id, ret, fileName);
		break;
#ifdef USED_LOADER_ONLY
	case HX_MOD_ID_20_KERNEL:
	case HX_MOD_ID_21_ROOTFS:
		ret = ir_spi_EraseFlashModulePayload(mod_id);
		dbgwarn1("delete:hx mod-payload[%x]\n", mod_id);
		break;
#endif
	default :
		dbgassert(0);
		return ERR_FAIL;

	}
	if(ret)
	{
		dbgerr0("################################################\n");
		dbgerr2("delete Module Payload [%x] error occured.(%d)\n", mod_id, ret);
		dbgerr0("Force set state \"OK\" !!!\n");
		dbgerr0("################################################\n");
		ret = ERR_OK;
	}

	/* Erase Module Header info */
	switch(mod_id)
	{
	case HX_MOD_ID_20_KERNEL:
	case HX_MOD_ID_21_ROOTFS:

	case MODULE_DRV_ID_UK:
	case MODULE_DRV_ID_BBCB:
	case MODULE_DRV_ID_LOADER_PT:
	case MODULE_DRV_ID_VARIANT:
		// erase module info file
		sprintf(fileName, NVRAM_CAS_IRV5_MIH_FILENAME_PFX"%d", mod_id);
		ret = PAL_NVRAM_Remove(DI_NVRAM_FS_PARTITION_RW, fileName);
		dbgwarn3("delete: MH [%x]:NVRAM: %d : (%s)\n", mod_id, ret, fileName);
		break;

	default :
		dbgassert(0);
		return ERR_FAIL;
	}

	if(ret)
	{
		dbgerr0("################################################\n");
		dbgerr2("delete Module header [%x] error occured.(%d)\n", mod_id, ret);
		dbgerr0("Force set state \"OK\" !!!\n");
		dbgerr0("################################################\n");
		ret = ERR_OK;
	}
	return ret;
}
#ifdef USED_LOADER_ONLY
HERROR	ir_spi_ReadFlashMP(HUINT16 mod_id, HUINT8 *pucReadBuffer, HUINT32 ulDataSize)
{
	HERROR ret;
	HUINT32 ulOffset;
	HUINT32 ulMaxSize;

	if(pucReadBuffer == NULL)
	{
		HxLOG_Error("%s, Target is null\n", __FUNCTION__);
		return ERR_FAIL;
	}

	switch(mod_id)
	{
	case HX_MOD_ID_20_KERNEL:
		ulMaxSize = HX_MOD_ID_20_KERNEL_SIZE_MAX;
		ulOffset = HX_MOD_ID_20_KERNEL_OFFSET;
		break;

	case HX_MOD_ID_21_ROOTFS:
		ulMaxSize = HX_MOD_ID_21_ROOTFS_SIZE_MAX;
		ulOffset = HX_MOD_ID_21_ROOTFS_OFFSET;
		break;

	default :
		dbgassert(0);
		return ERR_FAIL;
	}

	if(ulDataSize > ulMaxSize)
	{
		dbgerr2("Error: target size [%x], Max size [%x]\n", ulDataSize, ulMaxSize);
		dbgassert(0);
		return ERR_FAIL;
	}

	ret = PAL_FLASH_Read(ulOffset, pucReadBuffer, ulDataSize);
	if( ERR_OK != ret )
	{
		dbgassert(0);
		return ERR_FAIL;
	}
	return ERR_OK;

}

HERROR	ir_spi_WriteFlashMP( HUINT16 mod_id, HUINT8 *pucData, HUINT32 ulDataSize)
{
	HERROR ret;
	HUINT32 ulMaxSize;
	HUINT32	ulOffset;

	dbgprt3("mod_id [%x], pucData=[%p], ulDataSize=[%x]\n", mod_id, pucData, ulDataSize );

	if(pucData == NULL)
	{
		HxLOG_Error("%s, Target is null\n", __FUNCTION__);
		return ERR_FAIL;
	}

	switch(mod_id)
	{
	case HX_MOD_ID_20_KERNEL:
		ulMaxSize = HX_MOD_ID_20_KERNEL_SIZE_MAX;
		ulOffset = HX_MOD_ID_20_KERNEL_OFFSET;
		dbgdump("HX_MOD_ID_20: payload(forced 48 bytes", pucData, 48);
		break;

	case HX_MOD_ID_21_ROOTFS:
		ulMaxSize = HX_MOD_ID_21_ROOTFS_SIZE_MAX;
		ulOffset = HX_MOD_ID_21_ROOTFS_OFFSET;
		dbgdump("HX_MOD_ID_21: payload(forced 48 bytes", pucData, 48);
		break;
	default :
		dbgerr1("Unknown ID : [%x]\n", mod_id);
		dbgassert(0);
		break;
	}

	if(ulDataSize > ulMaxSize)
	{
		dbgerr2("Error: target size [%x], Max size [%x]\n", ulDataSize, ulMaxSize);
		dbgassert(0);
		return ERR_FAIL;
	}

	ulOffset |= IR_FLASH_NAND_ADDRESS_MASK; /* NAND Access */

	dbgprt1("flash write ulOffset(masked) [%x]\n", ulOffset );
	ret = PAL_FLASH_Write(ulOffset, pucData, ulDataSize);
	if( ERR_OK != ret )
	{
		dbgassert(0);
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR	ir_spi_EraseFlashModulePayload(HUINT16 mod_id)
{
	HERROR	ret;
	HUINT32 ulSize;
	HUINT32	ulOffset;
	HUINT8 *pucBuffer = NULL;

	switch(mod_id)
	{
	case HX_MOD_ID_20_KERNEL:
		ulSize  = HX_MOD_ID_20_KERNEL_SIZE_MAX;
		ulOffset= HX_MOD_ID_20_KERNEL_OFFSET;
		break;

	case HX_MOD_ID_21_ROOTFS:
		ulSize  = HX_MOD_ID_21_ROOTFS_SIZE_MAX;
		ulOffset= HX_MOD_ID_21_ROOTFS_OFFSET;
		break;

	default :
		dbgassert(0);
		break;
	}

	pucBuffer =	cas_ir_Malloc(ulSize);
	dbgassert(pucBuffer);

	VK_MEM_Memset(pucBuffer, 0xFF, ulSize);

	ulOffset |= IR_FLASH_NAND_ADDRESS_MASK; /* NAND Access */

	ret = PAL_FLASH_Write(ulOffset, pucBuffer, ulSize);
	if( ERR_OK != ret )
	{
		dbgassert(0);
		return ERR_FAIL;
	}
	return ERR_OK;
}

#endif
/**
 * Retrieve a list containing all the modules of high level application already owned by this device .
 *
 * The SPI gets all module IDs of high level application owned by this device. The module list
 * has a variable number of modules and must be well managed, being updated immediately at any changes
 * (i.e., at receiving a new module or after deleting a useless module).
 * The SPI must allocate the memory to hold all module IDs and set the module ID count correctly.
 * Loader Core will free the memory after using it.
 *
 * @param ppModuleIdList    [OUT] Pointer to an lc_uint16 array of module ids
 * @param pModuleIdCount    [OUT] Total number of modules in the module list
 *
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     ::LC_ERROR_MODULE_NOT_FOUND
 * @retval
 *     ::LC_ERROR_MODULE_NOT_SUPPORTED
 * @retval
 *     ::LC_ERROR_MODULE_ACCESS_ERROR
 * @retval
 *     ::LC_ERROR_ILLEGAL_MODULE_CONTENT
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
lc_result LoaderCoreSPI_Module_GetModuleIdList(lc_uint16 **ppModuleIdList, lc_uint32 *pModuleIdCount)
{
	lc_uint16 *ps;
	lc_sint32 i;

	dbgtrc;

	if( (NULL == ppModuleIdList) || (NULL == pModuleIdCount) )
	{
		return LC_ERROR_NULL_PARAM;
	}

	if( ( g_irdeto_module_master.total_module_count < 1) || ( g_irdeto_module_master.total_module_count > HX_MOD_ID_CNT_MAX) )
	{
		dbgassert(0);
		return LC_FAILURE;
	}

	ps = VK_malloc(g_irdeto_module_master.total_module_count * sizeof(lc_uint16) );
	if( NULL == ps)
	{
		return LC_FAILURE;
	}

	dbgprt1("g_irdeto_module_master.total_module_count = %d\n", g_irdeto_module_master.total_module_count);

	for	(i=0; i < g_irdeto_module_master.total_module_count ; i++)
	{
		ps[i] = g_irdeto_module_master.array_module_ids[i];
		dbgprt2("mod_master.array_module_ids(%d) = %d\n", i, g_irdeto_module_master.array_module_ids[i]);
	}

	*ppModuleIdList = ps;
	*pModuleIdCount = g_irdeto_module_master.total_module_count;

	return LC_SUCCESS;
}


/**
 * Retrieve a module's header.
 *
 * This SPI reads in the module header. Loader Core allocates the memory for pModuleHeader. SPI driver
 * shall allocate the buffer pModuleHeader->bytes and set pModuleHeader->length correctly to indicate
 * the length of the whole module header
 * Loader Core will free the memory after using it.
 *
 * @param moduleId      [IN] ID of module to read.
 * @param pModuleHeader [IN, OUT] pData Buffer to read module header data into.
 *     On input, the lc_buffer_st::bytes member points to a buffer in memory, and the lc_buffer_st::length member
 *     to the length of the buffer, in bytes. The function will read lc_buffer_st::length bytes from the resource
 *     and store them in lc_buffer_st::bytes.
 *
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     ::LC_ERROR_MODULE_NOT_FOUND
 * @retval
 *     ::LC_ERROR_MODULE_NOT_SUPPORTED
 * @retval
 *     ::LC_ERROR_MODULE_ACCESS_ERROR
 * @retval
 *     ::LC_ERROR_ILLEGAL_MODULE_CONTENT
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
lc_result LoaderCoreSPI_Module_ReadModuleHeader(lc_uint16 moduleId, lc_buffer_st *pModuleHeader)
{
	lc_result ret= LC_SUCCESS;
	lc_uchar mih_8byte[8];
	lc_uint16	header_whole_size;

	dev_going;
	dbgprt1("moduleID %d\n", moduleId);

	if(	NULL == pModuleHeader )
	{
		return LC_ERROR_NULL_PARAM;
	}

	if(	NULL != pModuleHeader->bytes )
	{
		dbgprt0("Warning buffer bytes is not null\n");
	}

	switch(moduleId)
	{
	case HX_MOD_ID_20_KERNEL:
	case HX_MOD_ID_21_ROOTFS:
		dbgwarn1("Access HX_MOD_ID_x [%04X]\n", moduleId );
	//for reserved irdeto modules.
	case MODULE_DRV_ID_UK:  //  0x0000,
	case MODULE_DRV_ID_LOADER_CORE: // 0x0001
	case MODULE_DRV_ID_BBCB:  //0x0002
	case MODULE_DRV_ID_LOADER_PT:  // 0x0003,
	case MODULE_DRV_ID_VARIANT:  // 0x0004

		/* 1st, Read modue header Length item */
		if( ERR_OK != ir_spi_ReadMH(moduleId, mih_8byte,  sizeof(mih_8byte)) )
		{
			dbgassert(0);
			ret = LC_FAILURE;
			break;
		}

		dbgdump("mih_8byte", &mih_8byte[0], sizeof(mih_8byte) );

#ifdef USED_LOADER_ONLY
		header_whole_size = ir_getbits16(&mih_8byte[2]);
		pModuleHeader->bytes = VK_malloc(header_whole_size);
#else
		header_whole_size = get16bit(&mih_8byte[2]);
		pModuleHeader->bytes = malloc(header_whole_size);
#endif


		dbgprt2("header_whole_size = %d [%x]\n", header_whole_size, header_whole_size);

		if( ERR_OK != ir_spi_ReadMH(moduleId, pModuleHeader->bytes,  header_whole_size) )
		{
			dbgassert(0);
//			VK_MEM_free(pModuleHeader->bytes);
			free(pModuleHeader->bytes);
			ret = LC_FAILURE;
			break;
		}
		pModuleHeader->length = header_whole_size;

		break;

	default:
		dbgassert(0);
		ret= LC_FAILURE;
		break;
	}
	return ret;
}


/**
 * Retrieve a module's data, without the header.
 *
 * It is up to the user to free the memory used by the data.
 *
 * @param moduleId       [IN] ID of module to read.
 * @param pModulePayload [IN, OUT] pData Buffer to read module payload data into.
 *     On input, the lc_buffer_st::bytes member points to a buffer in memory, and the lc_buffer_st::length member
 *     to the length of the buffer, in bytes. The function will read lc_buffer_st::length bytes from the resource
 *     and store them in lc_buffer_st::bytes.
 *
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     ::LC_ERROR_MODULE_NOT_FOUND
 * @retval
 *     ::LC_ERROR_MODULE_NOT_SUPPORTED
 * @retval
 *     ::LC_ERROR_MODULE_ACCESS_ERROR
 * @retval
 *     ::LC_ERROR_ILLEGAL_MODULE_CONTENT
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */

lc_result LoaderCoreSPI_Module_ReadModulePayload(lc_uint16 moduleId, lc_buffer_st *pModulePayload)
{
	lc_result ret= LC_SUCCESS;
//	lc_uchar * p;
	lc_uchar mih_8byte[8] = {0,}; // to get module_size.
	lc_uint32 module_size;

//	p = pModulePayload->bytes;

	dbgprt1("moduleID %d\n", moduleId);

	if(	NULL == pModulePayload )
	{
		return LC_ERROR_NULL_PARAM;
	}

	if(	NULL != pModulePayload->bytes )
	{
		dbgerr0("Warning buffer bytes is not null\n");
		dbgassert(0);
	}

	switch(moduleId)
	{
	//for reserved irdeto modules.
	case MODULE_DRV_ID_UK:  //  0x0000,
	case MODULE_DRV_ID_BBCB:  //0x0002
	case MODULE_DRV_ID_LOADER_PT:  // 0x0003,
	case MODULE_DRV_ID_VARIANT:  // 0x0004

		if( ERR_OK != ir_spi_ReadMH(moduleId, mih_8byte,  sizeof(mih_8byte)) )
		{
			ret = LC_ERROR_MODULE_NOT_FOUND;
			dbgerr1("Read MH [%x] error occured!\n", moduleId);
			break;
		}

//		module_size = ir_getbits32(&mih_8byte[4]);
		module_size = get32bit(&mih_8byte[4]);


//		dbgdump("mih_8byte", &mih_8byte[0], 8);
		dbgprt3("mod id[%x]-target mp size %d [%x]\n", moduleId, module_size, module_size);
		pModulePayload->length = module_size;

		if(module_size > 0)
		{
			pModulePayload->bytes= malloc(module_size);

			dbgassert(pModulePayload->bytes);
			if( ERR_OK != ir_spi_ReadNvramMP(moduleId, pModulePayload->bytes, module_size ) )
			{
				dbgerr2("Read MP [%x],len=[%x] error occured!\n", moduleId, module_size);
				ret = LC_FAILURE;
			}
		}
		else
		{
			dbgerr0("module size 'zero' detected!\n");
			ret = LC_ERROR_MODULE_NOT_FOUND;
		}
		break;

	case MODULE_DRV_ID_LOADER_CORE: // 0x0001

		if( ERR_OK != ir_spi_ReadMH( moduleId, mih_8byte, sizeof(mih_8byte)) )
		{
			ret = LC_ERROR_MODULE_NOT_FOUND;
			dbgerr1("Read MH [%x] error occured!\n", moduleId);
			break;
		}
		module_size = ir_getbits32(&mih_8byte[4]);
		dbgprt3("mod id[%x]-target mp size %d [%x]\n", moduleId, module_size, module_size);

		pModulePayload->bytes = malloc(module_size);
		dbgassert(pModulePayload->bytes);

		dbgwarn1("manually copy loader sw binary to mem.(module_size %d)\n", module_size);
#if 0
		LoaderCoreSPI_Stdlib_memcpy(pModulePayload->bytes, &bin_mod1_ld[0], module_size );
#else
{
		LoaderCoreSPI_Stdlib_memset(pModulePayload->bytes, 0x00, module_size );
}
#endif

		pModulePayload->length = module_size;
		break;
#ifdef USED_ONLY_IRV5_LOADER
	case HX_MOD_ID_20_KERNEL:
	case HX_MOD_ID_21_ROOTFS:
		if( ERR_OK != ir_spi_ReadMH(moduleId, mih_8byte,  sizeof(mih_8byte)) )
		{
			ret = LC_ERROR_MODULE_NOT_FOUND;
			dbgerr1("Read MH [%x] error occured!\n", moduleId);
			break;
		}
		module_size = ir_getbits32(&mih_8byte[4]);
		dbgprt3("mod id[%x]-target mp size %d [%x]\n", moduleId, module_size, module_size);

		pModulePayload->length = module_size;

		if(module_size > 0)
		{
			pModulePayload->bytes = malloc(module_size);

			dbgassert(pModulePayload->bytes);

			if(ERR_OK != ir_spi_ReadFlashMP(moduleId, pModulePayload->bytes, module_size) )
			{
				ret = LC_FAILURE;
				dbgerr2("Read MP [%x],len=[%x] error occured!\n", moduleId, module_size);
			}
		}
		else
		{
			dbgerr0("module size 'zero' detected!\n");
			ret = LC_ERROR_MODULE_NOT_FOUND;
		}

		break;
#endif
	default:
		dbgerr1("not supported moduleId [%x] accessed\n", moduleId);
		dbgassert(0);
		ret = LC_ERROR_MODULE_NOT_SUPPORTED;
		break;
	}

	if(LC_SUCCESS != ret)
	{
		dbgerr2("Read Module [%x] error occured! [%x]\n", moduleId, ret);
	}
	return ret;
}

/**
 * Write the entire module.
 *
 * This SPI writes the entire module to driver, including module header and module payload.
 *
 * @param moduleId  [IN] ID of module to write.
 * @param pModule   [IN] pData Buffer containing module data to write to the resource. The lc_buffer_st::bytes member
 *     points to the start of the buffer, and the lc_buffer_st::length member contains the number of bytes in the buffer
 *     to write.
 *
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     ::LC_ERROR_MODULE_NOT_FOUND
 * @retval
 *     ::LC_ERROR_MODULE_NOT_SUPPORTED
 * @retval
 *     ::LC_ERROR_MODULE_ACCESS_ERROR
 * @retval
 *     ::LC_ERROR_ILLEGAL_MODULE_CONTENT
 * @retval
 *     ::LC_ERROR_MODULE_UPDATE_DISALLOWED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
lc_result LoaderCoreSPI_Module_WriteModule(lc_uint16 moduleId, lc_buffer_st *pModule)
{
	lc_result ret= LC_SUCCESS;
	// TODO: Module header 와 Payload 가 같이 Write 되어야 한다.

	lc_uint32 mod_length, mod_header_whole_length;
	lc_uchar	* p;

	p = pModule->bytes;

	mod_header_whole_length = ir_getbits16( &pModule->bytes[2] );
	mod_length = ir_getbits32( &pModule->bytes[4] );

	dbgprt4("moduleId = 0x%X, mod_header_whole_length = %d, mod_length = %d, pModule->length = %d\n",
		moduleId, mod_header_whole_length , mod_length, 	pModule->length );

	switch(moduleId)
	{

	case MODULE_DRV_ID_UK: // UK
	case MODULE_DRV_ID_LOADER_PT: // loader partition.
	case MODULE_DRV_ID_VARIANT:	// variant.

		// write payload
		if(ERR_OK != ir_spi_WriteNvramMP( moduleId, p + mod_header_whole_length, mod_length ) )
		{
			dbgerr0("write error payload.\n");
			ret = LC_FAILURE;
			break;
		}
		// write header
		if(ERR_OK != ir_spi_WriteMH( moduleId, p,  mod_header_whole_length ) )
		{
			dbgerr0("write error header\n");
			ret = LC_FAILURE;
			break;
		}

		if(moduleId == MODULE_DRV_ID_LOADER_PT)
		{
			dbgdump("loader pt data dump", p + mod_header_whole_length, mod_length);
			dbgdump("loader pt mih  dump", p , mod_header_whole_length);
		}
		ret = LC_SUCCESS;
		break;
#ifdef USE_ONLY_LOADER
	case HX_MOD_ID_20_KERNEL:
	case HX_MOD_ID_21_ROOTFS:
		// write payload
		dbgprt2("Write module payload [%x], mod_header_whole_length = %d\n", moduleId, mod_header_whole_length);
		dbgdump("pay 128 byte", p + mod_header_whole_length , 128 );
		if(ERR_OK != ir_spi_WriteFlashMP( moduleId, p + mod_header_whole_length, mod_length ) )
		{
			dbgerr0("write error payload.\n");
			ret = LC_FAILURE;
			break;
		}
		dbgprt1("Write module [%x] Payload end\n", moduleId);

		// write header
		dbgprt1("Write module Header [%x]\n", moduleId);
		dbgdump("header 16 byte", p , 16 );
		if(ERR_OK != ir_spi_WriteMH( moduleId, p, mod_header_whole_length ) )
		{
			dbgerr0("write error header\n");
			ret = LC_FAILURE;
			break;
		}
		dbgprt1("Write module [%x] Header end\n", moduleId);
		ret = LC_SUCCESS;
		break;
#endif
	default:
		ret = LC_FAILURE;
		dbgassert(0);
		break;
	}

	return ret;
}


/**
 * Delete a module completely.
 *
 * @param moduleId [IN] ID of module to delete.
 *
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     ::LC_ERROR_MODULE_NOT_FOUND
 * @retval
 *     ::LC_ERROR_MODULE_NOT_SUPPORTED
 * @retval
 *     ::LC_ERROR_MODULE_ACCESS_ERROR
 * @retval
 *     ::LC_ERROR_ILLEGAL_MODULE_CONTENT
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
lc_result LoaderCoreSPI_Module_Delete(lc_uint16 moduleId)
{
	lc_result ret= LC_SUCCESS;

	dbgprt1("try delete moduleId [%x]\n",	moduleId );

	switch(moduleId)
	{
	case MODULE_DRV_ID_UK: // UK
	case MODULE_DRV_ID_LOADER_PT: // loader partition.
	case MODULE_DRV_ID_VARIANT:	// variant.
	case HX_MOD_ID_20_KERNEL:
	case HX_MOD_ID_21_ROOTFS:
		if(ERR_OK != ir_spi_DeleteModule( moduleId) )
		{
			dbgerr1("delete err, moduleId [%x]\n", moduleId);
			ret = LC_FAILURE;
		}
		break;

	default:
		dbgerr1("delete err, moduleId [%x]\n", moduleId);
		ret = LC_FAILURE;
		dbgassert(0);
		break;
	}
	return ret;

}


/** @} */ /* End of Loader Core Module SPIs */

/** @} */ /* End of Loader Core SPIs */


#ifdef __cplusplus
}
#endif



