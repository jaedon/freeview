#include "htype.h"

#include "vkernel.h"
#include "di_err.h"

#include "drv_nvram.h"
#include "di_nvram.h"
#include "di_uart.h"

DI_ERR_CODE DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_E partition, HUINT8 *pFileName, HUINT32 offset, void *data, HINT32 nDataSize)
{
	DRV_NVRAM_PARTITION_E drv_partition;

	switch (partition)
	{
	case DI_NVRAM_FS_PARTITION_RO :
		drv_partition = DRV_NVRAM_PARTITION_RO;
		break;
	case DI_NVRAM_FS_PARTITION_RW :
		drv_partition = DRV_NVRAM_PARTITION_RW;
		break;
	case DI_NVRAM_FS_PARTITION_LOADER :
		drv_partition = DRV_NVRAM_PARTITION_LOADER;
		break;
	case DI_NVRAM_PARTITION_W_RAWFS :
		drv_partition = DRV_NVRAM_PARTITION_W_RAWFS;
		break;
	default :
		return DI_ERR_ERROR;
	}

	return DRV_NVRAM_Write(drv_partition, pFileName, offset, data, nDataSize);
}

DI_ERR_CODE DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_E partition, HUINT8 *pFileName, HUINT32 offset, void *data, HINT32 nDataSize)
{
	DRV_NVRAM_PARTITION_E drv_partition;

	switch (partition)
	{
	case DI_NVRAM_FS_PARTITION_RO :
		drv_partition = DRV_NVRAM_PARTITION_RO;
		break;
	case DI_NVRAM_FS_PARTITION_RW :
		drv_partition = DRV_NVRAM_PARTITION_RW;
		break;
	case DI_NVRAM_FS_PARTITION_LOADER :
		drv_partition = DRV_NVRAM_PARTITION_LOADER;
		break;
	case DI_NVRAM_PARTITION_W_RAWFS :
		drv_partition = DRV_NVRAM_PARTITION_W_RAWFS;
		break;
	default :
		return DI_ERR_ERROR;
	}

	return DRV_NVRAM_Read(drv_partition, pFileName, offset, data, nDataSize);
}

DI_ERR_CODE DI_NVRAM_GetSize(DI_NVRAM_FS_PARTITION_E partition, HUINT8 *pFileName, HINT32 *pnDataSize)
{
	DRV_NVRAM_PARTITION_E drv_partition;

	switch (partition)
	{
	case DI_NVRAM_FS_PARTITION_RO :
		drv_partition = DRV_NVRAM_PARTITION_RO;
		break;
	case DI_NVRAM_FS_PARTITION_RW :
		drv_partition = DRV_NVRAM_PARTITION_RW;
		break;
	case DI_NVRAM_FS_PARTITION_LOADER :
		drv_partition = DRV_NVRAM_PARTITION_LOADER;
		return DI_ERR_ERROR;
		break;
	case DI_NVRAM_PARTITION_W_RAWFS :
		drv_partition = DRV_NVRAM_PARTITION_W_RAWFS;
		break;
	default :
		return DI_ERR_ERROR;
	}

	return DRV_NVRAM_GetLength(drv_partition, pFileName, (HUINT32*)pnDataSize);
}

DI_ERR_CODE DI_NVRAM_Remove(DI_NVRAM_FS_PARTITION_E partition, HUINT8 *pFileName)
{
	DRV_NVRAM_PARTITION_E drv_partition;

	switch (partition)
	{
	case DI_NVRAM_FS_PARTITION_RO :
		drv_partition = DRV_NVRAM_PARTITION_RO;
		break;
	case DI_NVRAM_FS_PARTITION_RW :
		drv_partition = DRV_NVRAM_PARTITION_RW;
		break;
	case DI_NVRAM_FS_PARTITION_LOADER :
		drv_partition = DRV_NVRAM_PARTITION_LOADER;
		return DI_ERR_ERROR;
		break;
	case DI_NVRAM_PARTITION_W_RAWFS :
		drv_partition = DRV_NVRAM_PARTITION_W_RAWFS;
		break;
	default :
		return DI_ERR_ERROR;
	}

	return DRV_NVRAM_Remove(drv_partition, pFileName);
}

DI_ERR_CODE DI_NVRAM_GetField(DI_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize)
{
	return DRV_NVRAM_GetField(field, offset, data, nDataSize);
}

DI_ERR_CODE DI_NVRAM_SetField(DI_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize)
{
	return DRV_NVRAM_SetField(field, offset, data, nDataSize);
}

DI_ERR_CODE DI_NVRAM_Reload(void)
{
	DRV_Error drv_err;

	drv_err = DRV_NVRAM_Reload(DRV_NVRAM_PARTITION_RO);
	if (drv_err != DRV_OK)
	{
		return drv_err;
	}
	drv_err = DRV_NVRAM_Reload(DRV_NVRAM_PARTITION_RW);
	if (drv_err != DRV_OK)
	{
		return drv_err;
	}
	return drv_err;
}

DI_ERR_CODE	DI_NVRAM_CheckData(DI_NVRAM_FS_PARTITION_E partition, HBOOL *orgData, HBOOL *backData, HBOOL *blockCheck)
{
	DRV_NVRAM_PARTITION_E drv_partition;

	switch (partition)
	{
	case DI_NVRAM_FS_PARTITION_RO :
		drv_partition = DRV_NVRAM_PARTITION_RO;
		break;
	case DI_NVRAM_FS_PARTITION_RW :
		drv_partition = DRV_NVRAM_PARTITION_RW;
		break;
	case DI_NVRAM_FS_PARTITION_LOADER :
		drv_partition = DRV_NVRAM_PARTITION_LOADER;
		return DI_ERR_ERROR;
		break;
	case DI_NVRAM_PARTITION_W_RAWFS :
		drv_partition = DRV_NVRAM_PARTITION_W_RAWFS;
		break;
	default :
		return DI_ERR_ERROR;
	}

	return DRV_NVRAM_CheckData(drv_partition, orgData, backData, blockCheck);
}

#if defined(CONFIG_SHARED_COMPILE)
DI_ERR_CODE	DI_NVRAM_Init(void)
{
	if (DRV_NVRAM_Init() != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#endif
