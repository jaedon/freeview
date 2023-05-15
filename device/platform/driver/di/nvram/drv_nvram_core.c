#if defined(CONFIG_SHARED_COMPILE)
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif
#endif
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<string.h>
#endif

#include "htype.h"

#include "vkernel.h"
#include "di_err.h"

#include "di_nvram.h"
#include "drv_nvram.h"
#include "drv_flash.h"
#if !defined(CONFIG_SHARED_COMPILE)
#include "di_uart.h"
#endif
#include "lib_queue.h"

#include "flash_map.h"

#if defined(CONFIG_NAND_FLASH)
#define WORKAROUND_NVRAM_UBI_OFFSET_ALIGN
#define UBI_BLOCK_SIZE	(LOGICAL_BLOCK_SIZE)

/* In case of using separated NVRAM volume */
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
#define CheckNVRAMPartition(offset) (offset < FLASH_NVRAMB_VOL_START ? 1 : 0)
#define GET_UBI_BLOCK_START_OFFSET(p,start) ((HUINT32)(p) - ((HUINT32)(p)-start)%UBI_BLOCK_SIZE)
#else
#define GET_UBI_BLOCK_START_OFFSET(p) ((HUINT32)(p) - ((HUINT32)(p)-FLASH_UBI_AREA_START)%UBI_BLOCK_SIZE)
#endif
#endif

#if defined(CONFIG_NVRAM_IN_NORFLASH)
#define NVRAM_CRC
#endif

#define NVRAM_TYPE_LEN			1
#define NVRAM_RECORD_LENG_LEN	4
#define NVRAM_NAME_LENG_LEN		1
#define NVRAM_DATA_LENG_LEN		4
#if defined(NVRAM_CRC)
#define NVRAM_CRC32_LEN			4
#else
#define NVRAM_CRC32_LEN			0
#endif

#if defined(CONFIG_NVRAM_IN_NORFLASH)
typedef enum NVRAM_ATTR_e
{
	NVRAM_ATTR_NON	=	0x00,
	NVRAM_ATTR_DEL	=	0x01,
	NVRAM_ATTR_ADD	=	0x02,
	NVRAM_ATTR_CHG	=	0x04
} NVRAM_ATTR_E;
#define ALIGN32(x)			(((x)+3)&(~0x3))

#define NVRAM_BULK_INDEX_LEN	4
#define NVRAM_OVERFLOW_MARGIN	256

#define GET_BASE_VALUE(p, size)	((p) & (~(size-1)))
#define GET_LIMIT_VALUE(p, size)	((p) & (size-1))

#define MAX_NAME_LEN		(256)
#define MAX_DATA_LEN		((128-8)*1024)
#define MAX_FIELD_LEN		(MAX_DATA_LEN+MAX_NAME_LEN*2)

#define NVRAM_TLV_TYPE_END	0xFF
#define NVRAM_TLV_TYPE_ENV	0x01
#define NVRAM_TLV_TYPE_JUNK	0x00
#else
#define MAX_NAME_LEN		(256)
#define MAX_DATA_LEN		(256*1024)
#define MAX_FIELD_LEN		(MAX_DATA_LEN+MAX_NAME_LEN*2)
#define NVRAM_TLV_TYPE_END	0x00
#define NVRAM_TLV_TYPE_ENV	0x01
#define NVRAM_TLV_TYPE_JUNK	0x11
#endif

typedef struct NVRAM_EVNVAR_t{
	queue_t qb;
	HUINT32 recLen;
	HUINT8 *name;
	HUINT8 nameLen;
	HUINT8 *value;
	HUINT32 valueLen;
#if defined(CONFIG_NVRAM_IN_NORFLASH)
	NVRAM_ATTR_E nvram_attr;
	HUINT32 flash_offset;
	HUINT32 old_flash_offset;
#endif
} NVRAM_EVNVAR_T;

#define get8bit(q) (HUINT8)((q)[0])
#define get32bit(q) (HUINT32)(((*(HUINT8 *)(q)) << 24) | (*((HUINT8 *)(q)+1) << 16) | (*((HUINT8 *)(q)+2) << 8) | (*((HUINT8 *)(q)+3)) )

#if defined(CONFIG_SHARED_COMPILE)
#define DI_UART_Print	VK_printf
#endif

#if defined(CONFIG_NVRAM_IN_NORFLASH)
extern const DRV_NVRAM_OPS_T nvram_ops;
extern unsigned long DRV_UTIL_GetCrc32(void *pvBuf, unsigned long n);

static HUINT32 s_nvram_lastpos_offset[DRV_NVRAM_PARTITION_MAX];
static HUINT16	s_nvram_bulk_index[DRV_NVRAM_PARTITION_MAX];
static HUINT32	s_flashBlockSize[DRV_NVRAM_PARTITION_MAX];
static HUINT32	s_nvramBlockSize[DRV_NVRAM_PARTITION_MAX];
#endif

/*  *********************************************************************
 *  Globals
 ********************************************************************* */

static queue_t s_nvram_envvars[] = {
	{&s_nvram_envvars[0],&s_nvram_envvars[0]},	// for DRV_NVRAM_PARTITION_RO
	{&s_nvram_envvars[1],&s_nvram_envvars[1]},	// for DRV_NVRAM_PARTITION_RW
	{&s_nvram_envvars[2],&s_nvram_envvars[2]},	// for DRV_NVRAM_PARTITION_LOADER
	{&s_nvram_envvars[3],&s_nvram_envvars[3]},	// for DRV_NVRAM_PARTITION_W_RAWFS
	{&s_nvram_envvars[4],&s_nvram_envvars[4]},	// for DRV_NVRAM_PARTITION_MAX
};

static HUINT32 s_nvram_offset[DRV_NVRAM_PARTITION_MAX] = {
#if defined(CONFIG_NVRAM_IN_NORFLASH)
	N_NVRAM_RO_OFFSET,
	N_NVRAM_RW_OFFSET,
	0,							// for DRV_NVRAM_PARTITION_LOADER
	0
#elif defined(CONFIG_NAND_FLASH)
		N_UBI_NVRAM_RO_OFFSET,		// for DRV_NVRAM_PARTITION_RO
	N_UBI_NVRAM_RW_OFFSET,		// for DRV_NVRAM_PARTITION_RW
	0,							// for DRV_NVRAM_PARTITION_LOADER
	N_UBI_RAW_FS_OFFSET			// for DRV_NVRAM_PARTITION_W_RAWFS
#elif defined(CONFIG_EMMC_FLASH)
		N_PART_NVRAM_RO_OFFSET,		// for DRV_NVRAM_PARTITION_RO
	N_PART_NVRAM_RW_OFFSET,		// for DRV_NVRAM_PARTITION_RW
	0,
	N_PART_IMAGE1_OFFSET,		// for DRV_NVRAM_PARTITION_W_RAWFS
#endif
};

static HUINT32 s_nvram_backup_offset[DRV_NVRAM_PARTITION_MAX] = {
#if defined(CONFIG_NVRAM_IN_NORFLASH)
	N_NVRAM_ROB_OFFSET,
	N_NVRAM_RWB_OFFSET,
	0,
	0
#elif defined(CONFIG_NAND_FLASH)
		N_UBI_NVRAM_ROB_OFFSET,		// for DRV_NVRAM_PARTITION_RO
	N_UBI_NVRAM_RWB_OFFSET,		// for DRV_NVRAM_PARTITION_RW
	0,							// for DRV_NVRAM_PARTITION_LOADER
	0							// for DRV_NVRAM_PARTITION_W_RAWFS
#elif defined(CONFIG_EMMC_FLASH)
		N_PART_NVRAM_ROB_OFFSET,	// for DRV_NVRAM_PARTITION_RO
	N_PART_NVRAM_RWB_OFFSET,	// for DRV_NVRAM_PARTITION_RW
	0,
	0
#endif
};

static HUINT32 s_nvram_size[DRV_NVRAM_PARTITION_MAX] = {
#if defined(CONFIG_NVRAM_IN_NORFLASH)
	N_NVRAM_RO_SIZE,
	N_NVRAM_RW_SIZE,
	0,
	0
#elif defined(CONFIG_NAND_FLASH)
		N_UBI_NVRAM_RO_SIZE,		// for DRV_NVRAM_PARTITION_RO
	N_UBI_NVRAM_RW_SIZE,		// for DRV_NVRAM_PARTITION_RW
	0,							// for DRV_NVRAM_PARTITION_LOADER
	N_UBI_RAW_FS_SIZE			// for DRV_NVRAM_PARTITION_W_RAWFS
#elif defined(CONFIG_EMMC_FLASH)
		N_PART_NVRAM_RO_SIZE,
	N_PART_NVRAM_RW_SIZE,
	0,
	N_PART_IMAGE1_SIZE,			// for DRV_NVRAM_PARTITION_W_RAWFS
#endif
};
#define DEBUG_ERR DI_UART_Print
#define DEBUG_INFO DI_UART_Print
#define UNUSED(x) (void)(x)

extern const DRV_NVRAM_OPS_T nvram_ops;

/*  *********************************************************************0
 *  drv_NVRAM_Read(buffer,offset,length)0
 *  0
 *  Read data from the NVRAM device0
 *
 *  Input parameters:
 *  	   buffer - destination buffer
 *  	   offset - offset of data to read
 *  	   length - number of bytes to read
 *
 *  Return value:
 *  	   number of bytes read, or <0 if error occured
 ********************************************************************* */
static DRV_Error drv_NVRAM_Read(DRV_NVRAM_PARTITION_E partition, HUINT8 *buffer,HUINT32 offset,HUINT32 length)
{
	DRV_Error	drv_error;
	int 		nvramHandle;

#if defined(CONFIG_NVRAM_IN_NORFLASH)
	HAPPY(partition);

	drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
	if (drv_error != DRV_OK)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	drv_error = (nvram_ops.Read) ? nvram_ops.Read(nvramHandle, offset, buffer, length) : DRV_ERR;
	if (drv_error != 0)
	{
		return DRV_ERR;
	}
#else
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
	HBOOL bNVRAMOrgPartition;

	HAPPY(partition);

	bNVRAMOrgPartition = CheckNVRAMPartition(offset);

	if(bNVRAMOrgPartition)
	{
		drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
		if (drv_error != DRV_OK)
		{
			return DRV_ERR_INVALID_PARAMETER;
		}
	}
	else
	{
		drv_error = (nvram_ops.GetBackupHandle) ? nvram_ops.GetBackupHandle(&nvramHandle) : DRV_ERR;
		if (drv_error != DRV_OK)
		{
			return DRV_ERR_INVALID_PARAMETER;
		}
	}
#else
	HAPPY(partition);

	drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
	if (drv_error != DRV_OK)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}
#endif

	drv_error = (nvram_ops.Read) ? nvram_ops.Read(nvramHandle, offset, buffer, length) : DRV_ERR;
	if (drv_error != 0)
	{
		DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, drv_error);
		return DRV_ERR;
	}
#endif

	return DRV_OK;
}

/*  *********************************************************************
 *  drv_NVRAM_Write(buffer,offset,length)
 *
 *  Write data to the NVRAM device
 *
 *  Input parameters:
 *  	   buffer - source buffer
 *  	   offset - offset of data to write
 *  	   length - number of bytes to write
 *
 *  Return value:
 *  	   number of bytes written, or -1 if error occured
 ********************************************************************* */
static DRV_Error drv_NVRAM_Write(DRV_NVRAM_PARTITION_E partition, HUINT8 *buffer,HUINT32 offset,HUINT32 length)
{
	DRV_Error	drv_error;
	int 		nvramHandle;
	HUINT32		apply_offset;
	HAPPY(partition);

	apply_offset = offset;
#if defined(CONFIG_NVRAM_IN_NORFLASH)
	drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
	if (drv_error != DRV_OK)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	drv_error = (nvram_ops.Write) ? nvram_ops.Write(nvramHandle, apply_offset, buffer, length) : DRV_ERR;
	if (drv_error != 0)
	{
		return DRV_ERR;
	}
#else
	/* NAND, eMMC */
#if defined(WORKAROUND_NVRAM_UBI_OFFSET_ALIGN)
	{
		HUINT32 offsetBlockStart;
		HUINT32	lengthBlockStartToBuferStart;
		HUINT32 offsetNextBlockStart;
		HUINT32 lengthBlocks;
		HUINT8	*p;
		HUINT32	eraseCount, i;
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
		/* In case of using separated NVRAM volume */
		HBOOL bNVRAMOrgPartition;

		bNVRAMOrgPartition = CheckNVRAMPartition(offset);

		if(bNVRAMOrgPartition)
		{
			drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
			if (drv_error != DRV_OK)
			{
				return DRV_ERR_INVALID_PARAMETER;
			}
		}
		else
		{
			drv_error = (nvram_ops.GetBackupHandle) ? nvram_ops.GetBackupHandle(&nvramHandle) : DRV_ERR;
			if (drv_error != DRV_OK)
			{
				return DRV_ERR_INVALID_PARAMETER;
			}
		}

		offsetBlockStart = bNVRAMOrgPartition ? GET_UBI_BLOCK_START_OFFSET(offset,FLASH_NVRAM_VOL_START) : GET_UBI_BLOCK_START_OFFSET(offset,FLASH_NVRAMB_VOL_START);
		lengthBlockStartToBuferStart = offset - offsetBlockStart;
		offsetNextBlockStart = bNVRAMOrgPartition ? GET_UBI_BLOCK_START_OFFSET(offset + length + UBI_BLOCK_SIZE - 1, FLASH_NVRAM_VOL_START) : GET_UBI_BLOCK_START_OFFSET(offset + length + UBI_BLOCK_SIZE - 1, FLASH_NVRAMB_VOL_START);
		lengthBlocks = offsetNextBlockStart - offsetBlockStart;
#else
		drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
		if (drv_error != DRV_OK)
		{
			return DRV_ERR_INVALID_PARAMETER;
		}

		offsetBlockStart = GET_UBI_BLOCK_START_OFFSET(offset);
		lengthBlockStartToBuferStart = offset - offsetBlockStart;
		offsetNextBlockStart = GET_UBI_BLOCK_START_OFFSET(offset + length + UBI_BLOCK_SIZE - 1);
		lengthBlocks = offsetNextBlockStart - offsetBlockStart;

#endif	/* N_HMX_MTD2UBINVRAM_NODE && N_HMX_MTD2UBI_NVRAMB_NODE */
		DI_UART_Print("offset(%08X) offsetBlockStart(%08X) lengthBlockStartToBuferStart(%08X) orgLen(%08X) lengthBlocks(%08X)\n", offset, offsetBlockStart, lengthBlockStartToBuferStart, length, lengthBlocks);

		p = DD_MEM_Alloc(lengthBlocks);
		if (p == NULL)
		{
			return DRV_ERR;
		}
		VK_memset(p, 0xFF, lengthBlocks);
		VK_memcpy(p+lengthBlockStartToBuferStart, buffer, length);
		drv_error = (nvram_ops.Write) ? nvram_ops.Write(nvramHandle, offsetBlockStart, p, lengthBlocks) : DRV_ERR;
		if (drv_error != DRV_OK)
		{
			eraseCount = lengthBlocks/UBI_BLOCK_SIZE;
			for (i=0; i<eraseCount; i++)
			{
				drv_error = (nvram_ops.Erase) ? nvram_ops.Erase(nvramHandle, offsetBlockStart+UBI_BLOCK_SIZE*i) : DRV_ERR;
				if(drv_error != DRV_OK)
					DI_UART_Print("[drv_NVRAM_Write] erase block offset(%08X) error !!\n", offsetBlockStart+UBI_BLOCK_SIZE*i);
				else
					DI_UART_Print("[drv_NVRAM_Write] erase block offset(%08X)\n", offsetBlockStart+UBI_BLOCK_SIZE*i);
			}
			drv_error = (nvram_ops.Write) ? nvram_ops.Write(nvramHandle, offsetBlockStart, p, lengthBlocks) : DRV_ERR;
			if (drv_error != DRV_OK)
			{
				DI_UART_Print("[drv_NVRAM_Write] error nvram_ops.Write retried. offset(%08X)\n", offsetBlockStart);
			}
		}
		DD_MEM_Free(p);

		if (drv_error != 0)
		{
			return DRV_ERR;
		}

		drv_error = (nvram_ops.Sync) ? nvram_ops.Sync() : DRV_ERR;
		if (drv_error != 0)
		{
			return DRV_ERR;
		}
	}
#else
	DI_UART_Print("[%s] offset(0x%08X) length(0x%08X)\n", __FUNCTION__, offset, length);
	drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
	if (drv_error != DRV_OK)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}
	drv_error = (nvram_ops.Write) ? nvram_ops.Write(nvramHandle, offset, buffer, length) : DRV_ERR;
	if (drv_error != 0)
	{
		return DRV_ERR;
	}
	drv_error = (nvram_ops.Sync) ? nvram_ops.Sync() : DRV_ERR;
	if (drv_error != 0)
	{
		return DRV_ERR;
	}
#endif	/* WORKAROUND_NVRAM_UBI_OFFSET_ALIGN */
#endif

	return DRV_OK;
}

DRV_Error drv_NVRAM_GetOffset(DRV_NVRAM_PARTITION_E partition, HUINT32 *pOffset)
{
	if (DRV_NVRAM_PARTITION_MAX <= partition)
		return DRV_ERR_INVALID_PARAMETER;

	*pOffset = s_nvram_offset[partition];
	return DRV_OK;
}

DRV_Error drv_NVRAM_GetSize(DRV_NVRAM_PARTITION_E partition, HUINT32 *pSize)
{
	if (DRV_NVRAM_PARTITION_MAX <= partition)
		return DRV_ERR_INVALID_PARAMETER;

	*pSize = s_nvram_size[partition];
	return DRV_OK;
}

/*  *********************************************************************
 *  P_NVRAM_FindEnv(name)
 *
 *  Locate an environment variable in the in-memory list
 *
 *  Input parameters:
 *  	   name - name of env var to find
 *
 *  Return value:
 *  	   NVRAM_EVNVAR_T pointer, or NULL if not found
 ********************************************************************* */

static NVRAM_EVNVAR_T *P_NVRAM_FindEnv(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name)
{
	HUINT32 namelen;
	queue_t *qb;
	NVRAM_EVNVAR_T *env;
	
	namelen = VK_strlen((char*)name);
	
	for (qb = s_nvram_envvars[partition].q_next; qb != &s_nvram_envvars[partition]; qb = qb->q_next)
	{
		env = (NVRAM_EVNVAR_T *) qb;
		if (env->nameLen == namelen && (VK_memcmp((char*)env->name, (char*)name, env->nameLen) == 0)
#if defined(CONFIG_NVRAM_IN_NORFLASH)
				&& env->nvram_attr != NVRAM_ATTR_DEL
#endif
		   )
			break;
	}

	if (qb == &s_nvram_envvars[partition])
		return NULL;

	return (NVRAM_EVNVAR_T *) qb;

}

/*  *********************************************************************
 *  drv_NVRAM_ScanDir(idx,name,namelen,val,vallen)
 *
 *  Enumerate environment variables.  This routine locates
 *  the nth environment variable and copies its name and value
 *  to user buffers.
 *
 *  The namelen and vallen variables must be preinitialized to
 *  the maximum size of the output buffer.
 *
 *  Input parameters:
 *  	   idx - variable index to find (starting with zero)
 *  	   name,namelen - name buffer and length
 *  	   val,vallen - value buffer and length
 *
 *  Return value:
 *  	   0 if ok
 *  	   else error code
 ********************************************************************* */

DRV_Error drv_NVRAM_ScanDir(DRV_NVRAM_PARTITION_E partition, HUINT32 idx, HUINT8 *name, HUINT32 *namelen, HUINT8 *val, HUINT32 *vallen)
{
	queue_t *qb;
	NVRAM_EVNVAR_T *env;

	for (qb = s_nvram_envvars[partition].q_next; qb != &s_nvram_envvars[partition]; qb = qb->q_next) {
		if (idx == 0)
			break;
		idx--;
	}

	if (qb == &s_nvram_envvars[partition])
	{
		return DRV_ERR;
	}
	env = (NVRAM_EVNVAR_T *) qb;

	if (name != NULL)
	{
		VK_strncpy((char*)name, (char*)env->name, env->nameLen);
		name[env->nameLen] = 0;
	}
	*namelen = env->nameLen;
	if (val != NULL)
	{
		VK_memcpy(val, env->value, env->valueLen);
	}
	*vallen  = env->valueLen;

	return DRV_OK;
}

/*  *********************************************************************
 *  drv_NVRAM_Delete(name)
 *
 *  Delete an environment variable
 *
 *  Input parameters:
 *  	   name - environment variable to delete
 *
 *  Return value:
 *  	   0 if ok
 *  	   else error code
 ********************************************************************* */

DRV_Error drv_NVRAM_Delete(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name)
{
	NVRAM_EVNVAR_T *env;

	env = P_NVRAM_FindEnv(partition, name);
	if (env == NULL)
	{
		return DRV_ERR;
	}


#if defined(CONFIG_NVRAM_IN_NORFLASH)
	env->nvram_attr = NVRAM_ATTR_DEL;
	env->old_flash_offset = env->flash_offset;
	env->flash_offset = 0;
#else
	q_dequeue((queue_t *) env);
	DD_MEM_Free(env);
#endif
	return DRV_OK;
}

/*  *********************************************************************
 *  drv_NVRAM_GetEnv(name)
 *
 *  Retrieve the value of an environment variable
 *
 *  Input parameters:
 *  	   name - name of environment variable to find
 *
 *  Return value:
 *  	   value, or NULL if variable is not found
 ********************************************************************* */

DRV_Error drv_NVRAM_GetEnv(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name, const HUINT32 offset, HUINT8 *value, HUINT32 readLen)
{
	NVRAM_EVNVAR_T	*env;
	HUINT32	len;

	VK_memset(value, 0, readLen);
	env = P_NVRAM_FindEnv(partition, name);
	if (env != NULL)
	{
		if (readLen > env->valueLen - offset)
			len = env->valueLen - offset;
		else
			len = readLen;

		VK_memcpy(value, env->value+offset, len);
	}
	/*
	else
	{
		DI_UART_Print("[drv_NVRAM_GetEnv] there is no file (%s), set to all zero\n", name);
	}
	*/

	return DRV_OK;
}

/*  *********************************************************************
 *  drv_NVRAM_GetLength(name)
 *
 *  Retrieve the value of an environment variable
 *
 *  Input parameters:
 *  	   name - name of environment variable to find
 *
 *  Return value:
 *  	   value, or NULL if variable is not found
 ********************************************************************* */

DRV_Error drv_NVRAM_GetLength(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name, HUINT32 *pLen)
{
	NVRAM_EVNVAR_T	*env;

	if (pLen == NULL)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	env = P_NVRAM_FindEnv(partition, name);
	if (env != NULL)
	{
		*pLen = env->valueLen;
		return DRV_OK;
	}

	return DRV_ERR_INVALID_PARAMETER;
}

/*  *********************************************************************
 *  drv_NVRAM_SetEnv(name,value,flags)
 *
 *  Set the value of an environment variable
 *
 *  Input parameters:
 *  	   name - name of variable
 *  	   value - value of variable
 *  	   flags - flags for variable (ENV_FLG_xxx)
 *
 *  Return value:
 *  	   0 if ok
 *  	   else error code
 ********************************************************************* */

DRV_Error drv_NVRAM_SetEnv(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name, HUINT8 *value, HUINT32 valueLen, HBOOL loading, HUINT32 flashOffset)
{
	NVRAM_EVNVAR_T *env;
	HUINT32 namelen;
#if defined(CONFIG_NVRAM_IN_NORFLASH)
	HUINT32	old_flash_offset = 0;
	HBOOL	changed = FALSE;
#endif
	HAPPY(flashOffset);
	HAPPY(loading);

	if (name == NULL || value == NULL)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	if (valueLen > MAX_DATA_LEN)
	{
		DI_UART_Print("drv_NVRAM_SetEnv error value length is too long %d\n", valueLen);
		return DRV_ERR;
	}

	env = P_NVRAM_FindEnv(partition, name);
	if (env)
	{
		if (VK_memcmp(env->value, value, valueLen) == 0)
		{
			return DRV_OK;
		}

#if defined(CONFIG_NVRAM_IN_NORFLASH)
		if (loading == FALSE)
		{
			old_flash_offset = env->flash_offset;
			changed = TRUE;
		}
#endif
		q_dequeue((queue_t *) env);
		DD_MEM_Free(env);
	}

	namelen = VK_strlen((char*)name);
	if (namelen > MAX_NAME_LEN)
	{
		DI_UART_Print("drv_NVRAM_SetEnv error name length is too long %d\n", namelen);
		return DRV_ERR;
	}
	env = DD_MEM_Alloc(sizeof(NVRAM_EVNVAR_T) + namelen + 1 + valueLen);
	if (!env)
	{
		return DRV_ERR_OUTOFMEMORY;
	}
	env->name = (HUINT8 *) (env+1);
	env->value = env->name + namelen + 1;
	env->nameLen = namelen;
	env->valueLen = valueLen;
	env->recLen = (NVRAM_NAME_LENG_LEN + namelen + NVRAM_DATA_LENG_LEN + valueLen + NVRAM_CRC32_LEN);

	VK_strncpy((char*)env->name, (char*)name, namelen);
	env->name[namelen] = 0;
#if defined(CONFIG_NVRAM_IN_NORFLASH)
	env->old_flash_offset = old_flash_offset;
	if (loading == TRUE)
		env->nvram_attr = NVRAM_ATTR_NON;
	else
		env->nvram_attr = changed ? NVRAM_ATTR_CHG : NVRAM_ATTR_ADD;
	env->flash_offset = flashOffset;
#endif
	VK_memcpy(env->value, value, valueLen);

	q_enqueue(&s_nvram_envvars[partition],(queue_t *) env);

	return DRV_OK;
}

#if defined(CONFIG_NVRAM_IN_NORFLASH)
static DRV_Error drv_NVRAM_GetBulkIndex(DRV_NVRAM_PARTITION_E partition, HUINT32 offset, HUINT32 *pBulkIndex1, HUINT32 *pBulkIndex2)
{
	DRV_Error	retval = DRV_ERR;
	HUINT8		ptr[NVRAM_BULK_INDEX_LEN];
	HUINT32		bulkIndex1, bulkIndex2;

	offset = GET_BASE_VALUE(offset, s_nvramBlockSize[partition]);

	//DI_UART_Print("[%s:%d] offset(%08X) addr(%08X)\n", __FUNCTION__, __LINE__, offset, offset + s_nvram_offset[partition] + s_nvramBlockSize[partition] - NVRAM_BULK_INDEX_LEN);

	retval = drv_NVRAM_Read(partition, ptr, offset + s_nvram_offset[partition] + s_nvramBlockSize[partition] - NVRAM_BULK_INDEX_LEN, NVRAM_BULK_INDEX_LEN);
	if (retval != DRV_OK)
	{
		DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
		retval = DRV_ERR_EXTERNAL_ERROR;
		goto error;
	}

	bulkIndex1 = get32bit(ptr);
	if (s_nvram_backup_offset[partition] != 0)
	{
		retval = drv_NVRAM_Read(partition, ptr, offset + s_nvram_backup_offset[partition] + s_nvramBlockSize[partition] - NVRAM_BULK_INDEX_LEN, NVRAM_BULK_INDEX_LEN);
		if (retval != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}
		bulkIndex2 = get32bit(ptr);
	}
	else
	{
		bulkIndex2 = bulkIndex1;
	}

	if (bulkIndex1 != bulkIndex2)
	{
		DI_UART_Print("bulk value mismatch (%08X) (%08X)\n", bulkIndex1, bulkIndex2);
	}

	*pBulkIndex1 = bulkIndex1;
	*pBulkIndex2 = bulkIndex2;

error:

	return retval;
}

static DRV_Error drv_NVRAM_SetBulkIndex(DRV_NVRAM_PARTITION_E partition, HUINT32 offset, const HUINT32 *pBulkIndex)
{
	DRV_Error	retval = DRV_ERR;
	HUINT8		ptr[NVRAM_BULK_INDEX_LEN];

	offset = GET_BASE_VALUE(offset, s_nvramBlockSize[partition]);
	ptr[0] = (HUINT8)(((*pBulkIndex)>>24) & 0xFF);
	ptr[1] = (HUINT8)(((*pBulkIndex)>>16) & 0xFF);
	ptr[2] = (HUINT8)(((*pBulkIndex)>>8) & 0xFF);
	ptr[3] = (HUINT8)(((*pBulkIndex)>>0) & 0xFF);

	retval = drv_NVRAM_Write(partition, ptr, offset + s_nvram_offset[partition] + s_nvramBlockSize[partition] - NVRAM_BULK_INDEX_LEN, NVRAM_BULK_INDEX_LEN);
	if (retval != DRV_OK)
	{
		DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
		retval = DRV_ERR_EXTERNAL_ERROR;
		goto error;
	}

	if (s_nvram_backup_offset[partition] != 0)
	{
		retval = drv_NVRAM_Write(partition, ptr, offset + s_nvram_backup_offset[partition] + s_nvramBlockSize[partition] - NVRAM_BULK_INDEX_LEN, NVRAM_BULK_INDEX_LEN);
		if (retval != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}
	}

error:

	return retval;
}

/*  *********************************************************************
 *  drv_NVRAM_LoadByAddress()
 *
 *  Load the environment from the NVRAM device.
 *
 *  Input parameters:
 *  	   nothing
 *
 *  Return value:
 *  	   0 if ok
 *  	   else error code
 ********************************************************************* */


static DRV_Error drv_NVRAM_LoadByAddress(DRV_NVRAM_PARTITION_E partition, HUINT32 address, HUINT32 size, HBOOL setEnv)
{
	HUINT32 nvram_size;
	HUINT8 *buffer;
	HUINT8 *ptr;
	HUINT32 recLen;
	HUINT32 recType;
	HUINT32 offset;
	HUINT32 retval = DRV_OK;
	HUINT8 fileName[MAX_NAME_LEN];
	HUINT8 nameLen;
	HUINT32 dataLen;
	HUINT32	flashOffset;
	DRV_Error	drvErr;
	HUINT32	nvramBulkIndex1, nvramBulkIndex2;
	HUINT16	nvram_bulk_index_min = 0xFFFF;
	HUINT32	nNormalBulkCount;
#if defined(NVRAM_CRC)
	HUINT32	calcCrc32, readCrc32;
#endif

	nvram_size = size;
	buffer = DD_MEM_Alloc(MAX_FIELD_LEN);
	if (buffer == NULL)
	{
		return DRV_ERR_OUTOFMEMORY;
	}

	ptr = buffer;
	nNormalBulkCount = 0;
	for (offset = 0; offset < size; offset += s_nvramBlockSize[partition])
	{
		retval = drv_NVRAM_GetBulkIndex(partition, offset, &nvramBulkIndex1, &nvramBulkIndex2);
		if (retval != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}

		if (nvramBulkIndex1 == 0)
		{
			DI_UART_Print("[%s:%d] found bulkIndex == 0 : non error\n", __FUNCTION__, __LINE__);
			continue;
		}

//		DI_UART_Print("[%s:%d] bulk index (%08X)\n", __FUNCTION__, __LINE__, nvramBulkIndex1);
		if (nvramBulkIndex1 != 0xFFFFFFFF)
		{
			nNormalBulkCount++;
			if (nvram_bulk_index_min > (HUINT16)(nvramBulkIndex1 & 0xFFFF))
			{
				nvram_bulk_index_min = (HUINT16)(nvramBulkIndex1 & 0xFFFF);
			}
		}
	}

	if (nNormalBulkCount > 1)
	{
		for (offset = 0; offset < size; offset += s_nvramBlockSize[partition])
		{
			retval = drv_NVRAM_GetBulkIndex(partition, offset, &nvramBulkIndex1, &nvramBulkIndex2);
			if (retval != DRV_OK)
			{
				DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
				retval = DRV_ERR_EXTERNAL_ERROR;
				goto error;
			}

			if (nvramBulkIndex1 == 0)
			{
				DI_UART_Print("[%s:%d] found bulkIndex == 0 : non error\n", __FUNCTION__, __LINE__);
				continue;
			}

			DI_UART_Print("[%s:%d] bulk index (%08X)\n", __FUNCTION__, __LINE__, nvramBulkIndex1);
			if (nvramBulkIndex1 != 0xFFFFFFFF)
			{
				if (nvram_bulk_index_min == (HUINT16)(nvramBulkIndex1 & 0xFFFF))
				{
					s_nvram_bulk_index[partition] = (HUINT16)nvramBulkIndex1 & 0xFFFF;
					DI_UART_Print("[drv_NVRAM_LoadByAddress] minimun bulkindex selected (%04X)\n", nvram_bulk_index_min);
					break;
				}
			}
		}
	}
	else
	{
		for (offset = 0; offset < size; offset += s_nvramBlockSize[partition])
		{
			retval = drv_NVRAM_GetBulkIndex(partition, offset, &nvramBulkIndex1, &nvramBulkIndex2);
			if (retval != DRV_OK)
			{
				DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
				retval = DRV_ERR_EXTERNAL_ERROR;
				goto error;
			}

			if (nvramBulkIndex1 == 0)
			{
				DI_UART_Print("[%s:%d] found bulkIndex == 0 : non error\n", __FUNCTION__, __LINE__);
				continue;
			}

//			DI_UART_Print("[%s:%d] bulk index (%08X)\n", __FUNCTION__, __LINE__, nvramBulkIndex1);
			if (nvramBulkIndex1 != 0xFFFFFFFF)
			{
				s_nvram_bulk_index[partition] = nvramBulkIndex1 & 0xFFFF;
				break;
			}
		}
	}

	if (offset >= size)
	{
		offset = 0;
	}

	/* Read the record type and length */
	retval = drv_NVRAM_Read(partition, ptr,offset+address,NVRAM_TYPE_LEN);
	if (retval != DRV_OK)
	{
		DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
		retval = DRV_ERR_EXTERNAL_ERROR;
		goto error;
	}
	flashOffset = offset;

	while ((*ptr != NVRAM_TLV_TYPE_END)  && (nvram_size > NVRAM_TYPE_LEN))
	{
		/* Adjust pointer for TLV type */
		recType = get8bit(ptr);
		nvram_size -= NVRAM_TYPE_LEN;
		offset += NVRAM_TYPE_LEN;

		/* Read the record type and length - 16 bits, MSB first */
		drvErr = drv_NVRAM_Read(partition, ptr, offset+address, NVRAM_RECORD_LENG_LEN);
		if (drvErr != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, drvErr);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}
		recLen = get32bit(ptr);
		nvram_size -= NVRAM_RECORD_LENG_LEN;
		offset += NVRAM_RECORD_LENG_LEN;

		if (recLen > MAX_FIELD_LEN)
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] error(%d), recType : (%d) recLen : (%d)\n", __FUNCTION__, __LINE__, retval, recType, recLen);
			goto error;
		}
		if (recLen > nvram_size)
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] error(%d) recType : (%d) recLen : (%d) > nvram_size : (%d) \n", __FUNCTION__, __LINE__, retval, recType, recLen, nvram_size);
			goto error; /* should not happen, bad NVRAM */
		}

		if (recType == NVRAM_TLV_TYPE_JUNK)
		{
			nvram_size -= (HUINT32)recLen;
			offset += recLen;
			offset = ALIGN32(offset);
			goto read_next;
		}

		if (recType != NVRAM_TLV_TYPE_ENV)
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			goto error;
		}

		/* Read the TLV data */
		drvErr = drv_NVRAM_Read(partition, ptr,offset+address,recLen);
		if (drvErr != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, drvErr);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}

#if defined(NVRAM_CRC)
		calcCrc32 = DRV_UTIL_GetCrc32(ptr, recLen-NVRAM_CRC32_LEN);
#endif
		nameLen = get8bit(ptr);
		ptr += NVRAM_NAME_LENG_LEN;
		if (nameLen)
		{
			VK_memcpy(fileName, ptr, nameLen);
		}
		fileName[nameLen] = 0;
		ptr += nameLen;

		dataLen = get32bit(ptr);
		ptr += NVRAM_DATA_LENG_LEN;
		if (dataLen > MAX_DATA_LEN)
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			goto error;
		}

#if defined(NVRAM_CRC)
		readCrc32 = get32bit(ptr+dataLen);
		if (readCrc32 != calcCrc32)
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] worng crc value read(0x%08X) calc(0x%08X)\n", __FUNCTION__, __LINE__, readCrc32, calcCrc32);
			goto error;
		}
#endif
		if (recLen != (NVRAM_NAME_LENG_LEN + nameLen + NVRAM_DATA_LENG_LEN + dataLen + NVRAM_CRC32_LEN))
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			goto error;
		}

		if (setEnv != FALSE)
		{
			drv_NVRAM_SetEnv(partition, fileName, ptr, dataLen, TRUE, flashOffset);
		}

		nvram_size -= (HUINT32)recLen;
		offset += recLen;
		offset = ALIGN32(offset);

read_next:
		/* Read the next record type */
		ptr = buffer;
		if (drv_NVRAM_Read(partition, ptr,offset+address, NVRAM_TYPE_LEN) != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error\n", __FUNCTION__, __LINE__);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}
		flashOffset = offset;
	}

	s_nvram_lastpos_offset[partition] = offset;
error:
	DD_MEM_Free(buffer);

	return retval;

}
#else

static DRV_Error drv_NVRAM_LoadByAddress(DRV_NVRAM_PARTITION_E partition, HUINT32 address, HUINT32 size, HBOOL setEnv)
{
	HUINT32 nvram_size;
	HUINT8 *buffer;
	HUINT8 *ptr;
	HUINT32 recLen;
	HUINT32 recType;
	HUINT32 offset;
	HUINT32 retval = DRV_OK;
	HUINT8 fileName[MAX_NAME_LEN];
	HUINT8 nameLen;
	HUINT32 dataLen;
	DRV_Error	drvErr;


	nvram_size = size;
	buffer = DD_MEM_Alloc(MAX_FIELD_LEN);
	if (buffer == NULL)
	{
		return DRV_ERR_OUTOFMEMORY;
	}

	ptr = buffer;
	offset = 0;

	/* Read the record type and length */
	drvErr = drv_NVRAM_Read(partition, ptr, offset+address, NVRAM_RECORD_LENG_LEN);
	if (retval != DRV_OK)
	{
		DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
		retval = DRV_ERR_EXTERNAL_ERROR;
		goto error;
	}

	while ((*ptr != NVRAM_TLV_TYPE_END)  && (nvram_size > NVRAM_TYPE_LEN))
	{
		/* Adjust pointer for TLV type */
		recType = get8bit(ptr);
		nvram_size -= NVRAM_TYPE_LEN;
		offset += NVRAM_TYPE_LEN;

		if (recType != NVRAM_TLV_TYPE_ENV)
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			goto error;
		}

		/* Read the record type and length - 16 bits, MSB first */
		drvErr = drv_NVRAM_Read(partition, ptr, offset+address, NVRAM_RECORD_LENG_LEN);
		if (drvErr != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, drvErr);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}
		recLen = get32bit(ptr);
		nvram_size -= NVRAM_RECORD_LENG_LEN;
		offset += NVRAM_RECORD_LENG_LEN;

		if (recLen > MAX_FIELD_LEN)
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] error(%d), recLen : (%d)\n", __FUNCTION__, __LINE__, retval, recLen);
			goto error;
		}
		if (recLen > nvram_size)
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			goto error; /* should not happen, bad NVRAM */
		}

		/* Read the TLV data */
		drvErr = drv_NVRAM_Read(partition, ptr,offset+address,recLen);
		if (drvErr != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, drvErr);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}

		nameLen = get8bit(ptr);
		ptr += NVRAM_NAME_LENG_LEN;
		if (nameLen)
		{
			VK_memcpy(fileName, ptr, nameLen);
		}
		fileName[nameLen] = 0;
		ptr += nameLen;

		dataLen = get32bit(ptr);
		ptr += NVRAM_DATA_LENG_LEN;
		if (dataLen > MAX_DATA_LEN)
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			goto error;
		}

		if (recLen != (NVRAM_NAME_LENG_LEN + nameLen + NVRAM_DATA_LENG_LEN + dataLen))
		{
			retval = DRV_ERR_EVENT_INITIALIZATION;
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			goto error;
		}

		if (setEnv != FALSE)
		{
			drv_NVRAM_SetEnv(partition, fileName, ptr, dataLen, FALSE, 0);
		}

		nvram_size -= (HUINT32)recLen;
		offset += recLen;

		/* Read the next record type */
		ptr = buffer;
		if (drv_NVRAM_Read(partition, ptr, offset+address, NVRAM_TYPE_LEN) != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error\n", __FUNCTION__, __LINE__);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}
	}

error:
	DD_MEM_Free(buffer);

	return retval;

}

#endif


DRV_Error drv_NVRAM_Load(DRV_NVRAM_PARTITION_E partition)
{
	DRV_Error	drv_error;

	drv_error  = drv_NVRAM_LoadByAddress(partition, s_nvram_offset[partition], s_nvram_size[partition], TRUE);
	if (drv_error != DRV_OK)
	{
		DI_UART_Print("[drv_NVRAM_Load] error(%d) loading partition (%d)\n", drv_error, partition);
		if (s_nvram_backup_offset[partition] != 0)
		{
			drv_error  = drv_NVRAM_LoadByAddress(partition, s_nvram_backup_offset[partition], s_nvram_size[partition], TRUE);
			if (drv_error != DRV_OK)
			{
				DI_UART_Print("[drv_NVRAM_Load] error(%d) loading backup partition (%d)\n", drv_error, partition);
			}
			else
			{
				DI_UART_Print("[drv_NVRAM_Load] load OK loading backup partition (%d)\n", partition);
			}
		}
	}
	return drv_error;
}

#if defined(CONFIG_NVRAM_IN_NORFLASH)
/*  *********************************************************************
 *  drv_NVRAM_CheckOverflow()
 *
 *  Write the environment to the NVRAM device.
 *
 *  Input parameters:
 *  	   nothing
 *
 *  Return value:
 *  	   0 if ok, else error code
 ********************************************************************* */

DRV_Error drv_NVRAM_CheckOverflow(DRV_NVRAM_PARTITION_E partition, HBOOL *pIsOverFlow)
{
	queue_t *qb;
	NVRAM_EVNVAR_T *env;
	HUINT32	flashOffset;
	HUINT32	totalLength;

	totalLength = 0;
	for (qb = s_nvram_envvars[partition].q_next; qb != &s_nvram_envvars[partition]; qb = qb->q_next)
	{
		env = (NVRAM_EVNVAR_T *) qb;

		if (env->nvram_attr == NVRAM_ATTR_DEL)
			continue;
		if (env->nvram_attr == NVRAM_ATTR_NON)
			continue;

		totalLength += ALIGN32(1 + env->recLen);
	}

	*pIsOverFlow = FALSE;
	flashOffset = GET_LIMIT_VALUE(s_nvram_lastpos_offset[partition], s_nvramBlockSize[partition]);
	if (s_nvramBlockSize[partition] <= flashOffset + totalLength + NVRAM_BULK_INDEX_LEN + NVRAM_OVERFLOW_MARGIN)
	{
		*pIsOverFlow = TRUE;
	}
	return DRV_OK;
}

/*  *********************************************************************
 *  drv_NVRAM_Compact()
 *
 *  Write the environment to the NVRAM device.
 *
 *  Input parameters:
 *  	   nothing
 *
 *  Return value:
 *  	   0 if ok, else error code
 ********************************************************************* */

DRV_Error drv_NVRAM_Compact(DRV_NVRAM_PARTITION_E partition, HBOOL *pNeedFormat, HUINT32 *pFormatOffset)
{
	queue_t *qb;
	NVRAM_EVNVAR_T *env;
	HUINT32	totalLength;

	totalLength = 0;

	if(pNeedFormat != NULL)
		*pNeedFormat = FALSE;
	if(pFormatOffset != NULL)
		*pFormatOffset = 0;

	for (qb = s_nvram_envvars[partition].q_next; qb != &s_nvram_envvars[partition]; qb = qb->q_next)
	{
		env = (NVRAM_EVNVAR_T *) qb;

		if (env->nvram_attr == NVRAM_ATTR_DEL)
		{
			qb = qb->q_prev;
			q_dequeue((queue_t *) env);
			continue;
		}
		env->nvram_attr = NVRAM_ATTR_ADD;
		env->flash_offset = 0;
		env->old_flash_offset = 0;
	}

	DI_UART_Print("[drv_NVRAM_Compact] partition %d, s_nvram_lastpos_offset %08X, s_nvramBlockSize[partition] %08X s_nvramBlockSize[partition]Mask %08X \n", partition, s_nvram_lastpos_offset[partition], s_nvramBlockSize[partition], (~(s_nvramBlockSize[partition]-1)));

	if (pFormatOffset != NULL)
	{
		*pFormatOffset = GET_BASE_VALUE(s_nvram_lastpos_offset[partition], s_nvramBlockSize[partition]);
		DI_UART_Print("[drv_NVRAM_Compact] FormatOffset %008X\n", *pFormatOffset);
	}

	s_nvram_lastpos_offset[partition] = GET_BASE_VALUE(s_nvram_lastpos_offset[partition] + s_nvramBlockSize[partition], s_nvramBlockSize[partition]);
	if (s_nvram_lastpos_offset[partition] >= s_nvram_size[partition])
	{
		s_nvram_lastpos_offset[partition] = 0;
	}
	if (pNeedFormat != NULL && s_nvram_size[partition] == s_nvramBlockSize[partition])
	{
		*pNeedFormat = TRUE;
	}
	return DRV_OK;
}
#endif

/*  *********************************************************************
 *  drv_NVRAM_Save()
 *
 *  Write the environment to the NVRAM device.
 *
 *  Input parameters:
 *  	   nothing
 *
 *  Return value:
 *  	   0 if ok, else error code
 ********************************************************************* */

DRV_Error drv_NVRAM_Save(DRV_NVRAM_PARTITION_E partition)
{
	DRV_Error write_error = DRV_OK, write_error_backup = DRV_OK, final_error = DRV_ERR_EXTERNAL_ERROR;
	HUINT8 *buffer;
	HUINT8 *buffer_end;
	HUINT8 *ptr;
	queue_t *qb;
	NVRAM_EVNVAR_T *env;
	HUINT8 namelen;
	HUINT32 valuelen;
	HUINT32 recLen;

#if defined(CONFIG_NVRAM_IN_NORFLASH)
	DRV_Error write_type_err = DRV_OK, write_type_err_backup = DRV_OK;
	DRV_Error retval;
	HUINT32	nvramBulkIndex1, nvramBulkIndex2;
	HBOOL	isBulkIndexWrite = FALSE;
	HUINT32	flashOffset;
	NVRAM_ATTR_E attr_bak;
#if defined(NVRAM_CRC)
	HUINT8	*pCrc32Start;
	HUINT32	calcCrc32;
#endif

	flashOffset = s_nvram_lastpos_offset[partition];
	retval = drv_NVRAM_GetBulkIndex(partition, flashOffset, &nvramBulkIndex1, &nvramBulkIndex2);
	if (retval != DRV_OK)
	{
		DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
		return DRV_ERR_OUTOFMEMORY;
	}
	if (nvramBulkIndex1 != nvramBulkIndex2)
	{
		DI_UART_Print("[%s:%d] error bulk index mismatch!(%d) Do compact\n", __FUNCTION__, __LINE__, retval);
		return DRV_ERR;
	}
	if (nvramBulkIndex1 == 0xFFFFFFFF)
		isBulkIndexWrite = TRUE;

	buffer = DD_MEM_Alloc(MAX_FIELD_LEN);
	if (buffer == NULL)
	{
		return DRV_ERR_OUTOFMEMORY;
	}
	buffer_end = buffer + MAX_FIELD_LEN - 1;

	ptr = buffer;
	for (qb = s_nvram_envvars[partition].q_next; qb != &s_nvram_envvars[partition]; qb = qb->q_next)
	{
		env = (NVRAM_EVNVAR_T *) qb;

		if (env->nvram_attr == NVRAM_ATTR_NON)
			continue;

		VK_memset(buffer, 0xFF, MAX_FIELD_LEN);

		namelen = env->nameLen;
		valuelen = env->valueLen;
		recLen = env->recLen;

		if ((ptr + NVRAM_TYPE_LEN + NVRAM_RECORD_LENG_LEN + recLen) > buffer_end - 1)
		{
			break;
		}

		*ptr++ = NVRAM_TLV_TYPE_ENV;		/* TLV record type */

		*ptr++ = (recLen>>24) & 0xFF;			/* TLV record length */
		*ptr++ = (recLen>>16) & 0xFF;			/* TLV record length */
		*ptr++ = (recLen>>8) & 0xFF;			/* TLV record length */
		*ptr++ = (recLen>>0) & 0xFF;			/* TLV record length */

#if defined(NVRAM_CRC)
		pCrc32Start = ptr;
#endif
		*ptr++ = (namelen>>0) & 0xFF;			/* NAME record length */

		VK_memcpy(ptr, env->name, namelen);		/* NAME record data */
		ptr += namelen;

		*ptr++ = (valuelen>>24) & 0xFF; 		/* VALUE record length */
		*ptr++ = (valuelen>>16) & 0xFF; 		/* VALUE record length */
		*ptr++ = (valuelen>>8) & 0xFF;			/* VALUE record length */
		*ptr++ = (valuelen>>0) & 0xFF;			/* VALUE record length */

		VK_memcpy(ptr,env->value, valuelen);
		ptr += valuelen;

#if defined(NVRAM_CRC)
		calcCrc32 = DRV_UTIL_GetCrc32(pCrc32Start, recLen-NVRAM_CRC32_LEN);
		*ptr++ = (calcCrc32>>24) & 0xFF; 		/* CRC32 */
		*ptr++ = (calcCrc32>>16) & 0xFF; 		/* CRC32  */
		*ptr++ = (calcCrc32>>8) & 0xFF;			/* CRC32  */
		*ptr++ = (calcCrc32>>0) & 0xFF;			/* CRC32  */
#endif
		write_error_backup = DRV_OK;
		write_type_err_backup = DRV_OK;
		final_error = DRV_ERR_EXTERNAL_ERROR;
		if (env->nvram_attr == NVRAM_ATTR_ADD || env->nvram_attr == NVRAM_ATTR_CHG)
		{
			DI_UART_Print("nvram original write offset(0x%08X) size(%d)\n", flashOffset, ALIGN32(ptr-buffer));
			write_error = drv_NVRAM_Write(partition, buffer, flashOffset+s_nvram_offset[partition], ALIGN32(ptr-buffer));
			if (s_nvram_backup_offset[partition] != 0)
			{
				DI_UART_Print("nvram backup write offset(0x%08X) size(%d)\n", flashOffset, ALIGN32(ptr-buffer));
				write_error_backup = drv_NVRAM_Write(partition, buffer, flashOffset+s_nvram_backup_offset[partition], ALIGN32(ptr-buffer));
			}
			if (isBulkIndexWrite == TRUE)
			{
				HUINT32	newBulkIndex;

				s_nvram_bulk_index[partition]++;
				if (s_nvram_bulk_index[partition] >= 0xFFFF || s_nvram_bulk_index[partition] < 2)
				{
					s_nvram_bulk_index[partition] = 2;
				}
				newBulkIndex = (s_nvram_bulk_index[partition] << 16) | s_nvram_bulk_index[partition];
				DI_UART_Print("s_nvram_bulk_index (%04X) s_nvram_lastpos_offset (%08X)\n", s_nvram_bulk_index[partition], s_nvram_lastpos_offset[partition]);
				drv_NVRAM_SetBulkIndex(partition, flashOffset, &newBulkIndex);
				isBulkIndexWrite = FALSE;
			}
		}
		if (env->nvram_attr == NVRAM_ATTR_DEL || env->nvram_attr == NVRAM_ATTR_CHG)
		{
			// write by env->old_flash_offset
			DI_UART_Print("nvram original 2 read offset(0x%08X) size(%d)\n", env->old_flash_offset, 2);
			write_type_err = drv_NVRAM_Read(partition, buffer, env->old_flash_offset+s_nvram_offset[partition], 2);
			if (write_type_err != DRV_OK)
			{
				DI_UART_Print("nvram backup 2 reade offset(0x%08X) size(%d)\n", env->old_flash_offset, 2);
				write_type_err_backup = drv_NVRAM_Read(partition, buffer, env->old_flash_offset+s_nvram_backup_offset[partition], 2);
			}
			buffer[0]=NVRAM_TLV_TYPE_JUNK;
			// write by env->old_flash_offset
			DI_UART_Print("nvram original 2 write offset(0x%08X) size(%d)\n", env->old_flash_offset, 2);
			write_type_err = drv_NVRAM_Write(partition, buffer, env->old_flash_offset+s_nvram_offset[partition], 2);
			if (s_nvram_backup_offset[partition] != 0)
			{
				DI_UART_Print("nvram backdup 2 write offset(0x%08X) size(%d)\n", env->old_flash_offset,2);
				write_type_err_backup = drv_NVRAM_Write(partition, buffer, env->old_flash_offset+s_nvram_backup_offset[partition], 2);
			}
		}

		attr_bak = env->nvram_attr;
		if (env->nvram_attr == NVRAM_ATTR_DEL)
		{
			qb = qb->q_prev;
			q_dequeue((queue_t *) env);
		}
		else
		{
			env->nvram_attr = NVRAM_ATTR_NON;
			env->old_flash_offset = 0;
			env->flash_offset = flashOffset;
		}

		if (attr_bak != NVRAM_ATTR_DEL)
		{
			flashOffset += NVRAM_TYPE_LEN + NVRAM_RECORD_LENG_LEN + recLen;
			flashOffset = ALIGN32(flashOffset);
		}
		ptr = buffer;

	}
	s_nvram_lastpos_offset[partition] = flashOffset;

	//	*ptr++ = NVRAM_TLV_TYPE_END;

	DD_MEM_Free(buffer);

	if ((write_error==DRV_OK) && (write_error_backup==DRV_OK)&&(write_type_err==DRV_OK) && (write_type_err_backup==DRV_OK))
	{
		final_error = DRV_OK;
	}
#else
	HUINT32 nvram_size;

	nvram_size = s_nvram_size[partition];
	buffer = DD_MEM_Alloc(nvram_size);
	if (buffer == NULL)
	{
		return DRV_ERR_OUTOFMEMORY;
	}
	buffer_end = buffer + nvram_size - 1;

	ptr = buffer;

	for (qb = s_nvram_envvars[partition].q_next; qb != &s_nvram_envvars[partition]; qb = qb->q_next)
	{
		env = (NVRAM_EVNVAR_T *) qb;

		namelen = env->nameLen;
		valuelen = env->valueLen;
		recLen = env->recLen;

		if ((ptr + NVRAM_TYPE_LEN + NVRAM_RECORD_LENG_LEN + recLen) > buffer_end - 1)
		{
			break;
		}

		*ptr++ = NVRAM_TLV_TYPE_ENV;		/* TLV record type */

		*ptr++ = (recLen>>24) & 0xFF;			/* TLV record length */
		*ptr++ = (recLen>>16) & 0xFF;			/* TLV record length */
		*ptr++ = (recLen>>8) & 0xFF;			/* TLV record length */
		*ptr++ = (recLen>>0) & 0xFF;			/* TLV record length */

		*ptr++ = (namelen>>0) & 0xFF;			/* NAME record length */

		VK_memcpy(ptr, env->name, namelen);		/* NAME record data */
		ptr += namelen;

		*ptr++ = (valuelen>>24) & 0xFF;			/* VALUE record length */
		*ptr++ = (valuelen>>16) & 0xFF;			/* VALUE record length */
		*ptr++ = (valuelen>>8) & 0xFF;			/* VALUE record length */
		*ptr++ = (valuelen>>0) & 0xFF;			/* VALUE record length */

		VK_memcpy(ptr,env->value, valuelen);
		ptr += valuelen;

	}

	*ptr++ = NVRAM_TLV_TYPE_END;

	write_error_backup = DRV_OK;
	final_error = DRV_ERR_EXTERNAL_ERROR;
	write_error = drv_NVRAM_Write(partition, buffer,0+s_nvram_offset[partition],ptr-buffer);
	if (s_nvram_backup_offset[partition] != 0)
	{
		write_error_backup = drv_NVRAM_Write(partition, buffer,0+s_nvram_backup_offset[partition],ptr-buffer);
	}

	DD_MEM_Free(buffer);

	if ((write_error==DRV_OK) && (write_error_backup==DRV_OK))
	{
		final_error = DRV_OK;
	}
#endif

	return final_error;
}

DRV_Error drv_NVRAM_CheckPartition(DRV_NVRAM_PARTITION_E partition, HBOOL *orgData, HBOOL *backData, HBOOL *blockCheck)
{
	DRV_Error	drv_org_error = DRV_ERR;
	DRV_Error	drv_backup_error = DRV_ERR;
	DRV_Error	drv_diff_error = DRV_ERR;
	DRV_Error	drv_error = DRV_ERR;
	HUINT8		*org_buffer, *backup_buffer;
	int			diff;

	if (orgData == NULL || backData == NULL || blockCheck == NULL)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	*orgData = FALSE;
	*backData = FALSE;
	*blockCheck = FALSE;
	drv_org_error  = drv_NVRAM_LoadByAddress(partition, s_nvram_offset[partition], s_nvram_size[partition], FALSE);
	if (drv_org_error == DRV_OK)
	{
		*orgData = TRUE;
	}
	else
	{
		DI_UART_Print("[drv_NVRAM_CheckPartition] error(%d) loading partition (%d)\n", drv_org_error, partition);
	}

	if (s_nvram_backup_offset[partition] != 0)
	{
		drv_backup_error  = drv_NVRAM_LoadByAddress(partition, s_nvram_backup_offset[partition], s_nvram_size[partition], FALSE);
		if (drv_backup_error == DRV_OK)
		{
			*backData = TRUE;
			DI_UART_Print("[drv_NVRAM_CheckPartition] load OK loading backup partition (%d)\n", partition);
		}
		else
		{
			DI_UART_Print("[drv_NVRAM_CheckPartition] error(%d) loading backup partition (%d)\n", drv_org_error, partition);
		}
	}

	if (s_nvram_backup_offset[partition] != 0)
	{
		org_buffer = DD_MEM_Alloc(s_nvram_size[partition]);
		if (org_buffer == NULL)
		{
			DI_UART_Print("[drv_NVRAM_CheckPartition] org_buffer = DD_MEM_Alloc(s_nvram_size[partition]) partition (%d)\n", partition);
			return DRV_ERR_OUTOFMEMORY;
		}
		backup_buffer = DD_MEM_Alloc(s_nvram_size[partition]);
		if (backup_buffer == NULL)
		{
			DI_UART_Print("[drv_NVRAM_CheckPartition] backup_buffer = DD_MEM_Alloc(s_nvram_size[partition]) partition (%d)\n", partition);
			DD_MEM_Free(org_buffer);
			return DRV_ERR_OUTOFMEMORY;
		}
		drv_error = drv_NVRAM_Read(partition, org_buffer, s_nvram_offset[partition], s_nvram_size[partition]);
		drv_error = drv_NVRAM_Read(partition, backup_buffer, s_nvram_backup_offset[partition], s_nvram_size[partition]);
		diff = VK_memcmp(org_buffer, backup_buffer, s_nvram_size[partition]);
		if (diff == 0)
		{
			DI_UART_Print("[drv_NVRAM_CheckPartition] DIFF OK partition (%d)\n", partition);
			drv_diff_error = DRV_OK;
			*blockCheck = TRUE;
		}
		DD_MEM_Free(backup_buffer);
		DD_MEM_Free(org_buffer);
	}

	return drv_org_error ;
}

#if defined(CONFIG_NVRAM_IN_NORFLASH)
DRV_Error	drv_NVRAM_SetJunkBlock(DRV_NVRAM_PARTITION_E partition, HUINT32 offset)
{
	HUINT32		newBulkIndex = 0;

	return drv_NVRAM_SetBulkIndex(partition, offset, &newBulkIndex);
}

DRV_Error	drv_NVRAM_Erase(DRV_NVRAM_PARTITION_E partition, HUINT32 offset)
{
	DRV_Error	drv_error;
	int			nvramHandle;
	HUINT32		apply_offset;

	drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
	if (drv_error != DRV_OK)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}


	apply_offset = GET_LIMIT_VALUE(offset, s_nvram_size[partition]);
	if (apply_offset != offset)
	{
		DI_UART_Print("[drv_NVRAM_Erase] notice offset value is overrrun %d -> %d\n", offset, apply_offset);
	}

	DI_UART_Print("[drv_NVRAM_Erase] erase original offset(%08X) = offset (%08X) + nvram_offset(%08X)\n", offset+s_nvram_offset[partition], offset, s_nvram_offset[partition]);
	drv_error = (nvram_ops.Erase) ? nvram_ops.Erase(nvramHandle, offset+s_nvram_offset[partition]) : DRV_ERR;
	if (drv_error != 0)
	{
		return DRV_ERR;
	}
	if (s_nvram_backup_offset[partition] != 0)
	{
		DI_UART_Print("[drv_NVRAM_Erase] erase backup offset(%08X) = offset (%08X) + nvram_offset(%08X)\n", offset+s_nvram_backup_offset[partition], offset, s_nvram_backup_offset[partition]);
		drv_error = (nvram_ops.Erase) ? nvram_ops.Erase(nvramHandle, offset+s_nvram_backup_offset[partition]) : DRV_ERR;
		if (drv_error != 0)
		{
			return DRV_ERR;
		}
	}
	return DRV_OK;
}

DRV_Error drv_NVRAM_VerifyErased(DRV_NVRAM_PARTITION_E partition, HBOOL *pErased, HUINT32 *eraseOffset)
{
	HUINT8 *buffer;
	HUINT32 offset;
	HUINT32 retval = DRV_OK;
	HUINT32 *pulData;
	HUINT32	ulIndex;

	*pErased = TRUE;
	*eraseOffset = 0;

	buffer = DD_MEM_Alloc(s_nvramBlockSize[partition]);
	if (buffer == NULL)
	{
		return DRV_ERR_OUTOFMEMORY;
	}

	for (offset = 0; offset < s_nvram_size[partition]; offset += s_nvramBlockSize[partition])
	{
		if ((s_nvram_lastpos_offset[partition] > offset)
				&& (s_nvram_lastpos_offset[partition] < offset+s_nvramBlockSize[partition]))
		{
			continue;
		}
		retval = drv_NVRAM_Read(partition, buffer, s_nvram_offset[partition]+offset, s_nvramBlockSize[partition]);
		if (retval != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}

		pulData = (HUINT32*)buffer;
		for (ulIndex = 0; ulIndex<(s_nvramBlockSize[partition]>>2); ulIndex++)
		{
			if (pulData[ulIndex] != 0xFFFFFFFF)
			{
				*pErased = FALSE;
				*eraseOffset = offset;
				retval = DRV_OK;
				goto error;
			}
		}

		retval = drv_NVRAM_Read(partition, buffer, s_nvram_backup_offset[partition]+offset, s_nvramBlockSize[partition]);
		if (retval != DRV_OK)
		{
			DI_UART_Print("[%s:%d] error(%d)\n", __FUNCTION__, __LINE__, retval);
			retval = DRV_ERR_EXTERNAL_ERROR;
			goto error;
		}

		pulData = (HUINT32*)buffer;
		for (ulIndex = 0; ulIndex<(s_nvramBlockSize[partition]>>2); ulIndex++)
		{
			if (pulData[ulIndex] != 0xFFFFFFFF)
			{
				*pErased = FALSE;
				*eraseOffset = offset;
				retval = DRV_OK;
				goto error;
			}
		}
	}

error:
	DD_MEM_Free(buffer);

	return retval;
}

void DRV_NVRAM_DI_UART_Print(void)
{
#if defined(CONFIG_NVRAM_IN_NORFLASH)
	HINT32		i;
	HUINT32		nvramBulkIndexRo;
	HUINT32		nvramBulkIndexRw;
	HUINT32		nvramBulkIndexRw2;
	HUINT32		temp;

	for (i=0; i<DRV_NVRAM_PARTITION_MAX; i++)
	{
		DI_UART_Print("s_nvram_bulk_index %d, s_nvram_lastpos_offset (%d)\n", s_nvram_bulk_index[i], s_nvram_lastpos_offset[i]);
		drv_NVRAM_GetBulkIndex(DRV_NVRAM_PARTITION_RW, s_nvramBlockSize[i], &nvramBulkIndexRw2, &temp);
	}

	drv_NVRAM_GetBulkIndex(DRV_NVRAM_PARTITION_RO, 0, &nvramBulkIndexRo, &temp);
	drv_NVRAM_GetBulkIndex(DRV_NVRAM_PARTITION_RW, 0, &nvramBulkIndexRw, &temp);

	DI_UART_Print("nvramBulkIndex1 RO(%08X) RW(%08X) RW2(%08X)\n", nvramBulkIndexRo, nvramBulkIndexRw, nvramBulkIndexRw2);
#endif

	return;
}

DRV_Error drv_NVRAM_SetBlockSize(DRV_NVRAM_PARTITION_E partition)
{
	int 		nvramHandle;
	HUINT32 ulBlockSize = 0;
	DRV_Error	drv_error;

	drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
	if (drv_error == DRV_OK)
	{
		drv_error = (nvram_ops.GetBlockSize) ? nvram_ops.GetBlockSize(nvramHandle, &ulBlockSize) : DRV_ERR;
		if(drv_error == DRV_OK && ulBlockSize > 0) {
			s_flashBlockSize[partition] = ulBlockSize;
		} else {
			s_flashBlockSize[partition] = 64*1024;
		}

		s_nvramBlockSize[partition] = ((s_nvram_size[partition]/ulBlockSize) * ulBlockSize)/2;
		DI_UART_Print("s_nvram_size[%d]:0x%x, s_nvramBlockSize : 0x%x, ulBlockSize:0x%x, s_flashBlockSize:0x%x\n", partition, s_nvram_size[partition], s_nvramBlockSize[partition], ulBlockSize, s_flashBlockSize[partition]);
	}

	return DRV_OK;
}

DRV_Error drv_NVRAM_GetBlockSize(DRV_NVRAM_PARTITION_E partition, HUINT32 *flashBlockSize, HUINT32 *nvramBlockSize)
{
	if(flashBlockSize)
		*flashBlockSize = s_flashBlockSize[partition];

	if(nvramBlockSize)
		*nvramBlockSize = s_nvramBlockSize[partition];

	return DRV_OK;
}
#endif
