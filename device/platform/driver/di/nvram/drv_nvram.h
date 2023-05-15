/**
 * drv_nvram.h
 */

/**
 * @defgroup		DRV_NVRAM DRV_NVRAM : flashmemory r/w module
 * @author		Kevin Woo
 * @note			DRV_NVRAM Read/write APIs
 * @brief			Define NVRAM APIs
 * @file 			drv_nvram.h
 */

#ifndef __DRV_NVRAM_H__
#define __DRV_NVRAM_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"
#include "di_nvram.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define NVRAM_SAVED_TEXT_SIZE	32

#define MAX_NVRAM_FILENAME_LENGTH	32


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum DRV_NVRAM_PARTITION_e
{
	DRV_NVRAM_PARTITION_RO,
	DRV_NVRAM_PARTITION_RW,
	DRV_NVRAM_PARTITION_LOADER,
	DRV_NVRAM_PARTITION_W_RAWFS,
	DRV_NVRAM_PARTITION_MAX
}DRV_NVRAM_PARTITION_E;

typedef struct
{
	DRV_Error (*Write)(int handle, unsigned long address, unsigned char *data, unsigned long data_size);
	DRV_Error (*Read)(int handle, unsigned long address, unsigned char *data, unsigned long data_size);
	DRV_Error (*Erase)(int handle, unsigned long address);
	DRV_Error (*Sync)(void);
	DRV_Error (*GetHandle)(int *handle);
	DRV_Error (*GetBackupHandle)(int *handle);
	DRV_Error (*GetBlockSize)(int handle, HUINT32 *block_size);
}DRV_NVRAM_OPS_T;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DRV_Error	DRV_NVRAM_Init(void);
DRV_Error	DRV_NVRAM_Write(DRV_NVRAM_PARTITION_E partition, HUINT8 *pName, HUINT32 offset, HUINT8 *pValue, HUINT32 ulSize);
DRV_Error	DRV_NVRAM_Read(DRV_NVRAM_PARTITION_E partition, HUINT8 *pName, HUINT32 offset, HUINT8 *pValue, HUINT32 ulSize);
DRV_Error	DRV_NVRAM_GetLength(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name, HUINT32 *pLen);
DRV_Error	DRV_NVRAM_GetField(DI_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize);
DRV_Error	DRV_NVRAM_SetField(DI_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize);
DRV_Error	DRV_NVRAM_GetFieldLength(DI_NVRAM_FIELD_T field, HUINT32 *pLen);
DRV_Error	DRV_NVRAM_Remove(DRV_NVRAM_PARTITION_E partition, HUINT8 *pName);
DRV_Error	DRV_NVRAM_Reload(DRV_NVRAM_PARTITION_E partition);
DRV_Error	DRV_NVRAM_CheckData(DRV_NVRAM_PARTITION_E partition, HBOOL *orgData, HBOOL *backData, HBOOL *blockCheck);
DRV_Error	DRV_NVRAM_Dir(void);

// for nvram core
DRV_Error drv_NVRAM_GetOffset(DRV_NVRAM_PARTITION_E partition, HUINT32 *pOffset);
DRV_Error drv_NVRAM_GetSize(DRV_NVRAM_PARTITION_E partition, HUINT32 *pSize);
DRV_Error drv_NVRAM_ScanDir(DRV_NVRAM_PARTITION_E partition, HUINT32 idx, HUINT8 *name, HUINT32 *namelen, HUINT8 *val, HUINT32 *vallen);
DRV_Error drv_NVRAM_Delete(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name);
DRV_Error drv_NVRAM_GetEnv(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name, const HUINT32 offset, HUINT8 *value, HUINT32 readLen);
DRV_Error drv_NVRAM_GetLength(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name, HUINT32 *pLen);
DRV_Error drv_NVRAM_SetEnv(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name, HUINT8 *value, HUINT32 valueLen, HBOOL loading, HUINT32 flashOffset);
DRV_Error drv_NVRAM_Load(DRV_NVRAM_PARTITION_E partition);
DRV_Error drv_NVRAM_Save(DRV_NVRAM_PARTITION_E partition);
DRV_Error drv_NVRAM_CheckPartition(DRV_NVRAM_PARTITION_E partition, HBOOL *orgData, HBOOL *backData, HBOOL *blockCheck);

// for nvram for video resolution
DRV_Error DRV_NVRAM_ConvertVideoResolutionText(DI_NVRAM_VIDEO_RESOLUTION_E videoResolution, HUINT8 *pszNvramResolution);
DRV_Error DRV_NVRAM_ConvertVideoResolutionField(const HUINT8 *pszNvramResolution, DI_NVRAM_VIDEO_RESOLUTION_E *pVideoResolution);
DRV_Error DRV_NVRAM_ConvertFieldRateText(DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E fieldRate, HUINT8 *pszNvramResolution);
DRV_Error DRV_NVRAM_ConvertFieldRate(const HUINT8 *pszNvramResolution, DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E *pFieldRate);

// for nvram for video resolution
DRV_Error DRV_NVRAM_ConvertSystemId(HUINT32 *data);
DRV_Error DRV_NVRAM_DirData(void);

#if defined(CONFIG_NVRAM_IN_NORFLASH)
DRV_Error drv_NVRAM_CheckOverflow(DRV_NVRAM_PARTITION_E partition, HBOOL *pIsOverFlow);
DRV_Error drv_NVRAM_Compact(DRV_NVRAM_PARTITION_E partition, HBOOL *pNeedFormat, HUINT32 *pFormatOffset);
DRV_Error drv_NVRAM_SetJunkBlock(DRV_NVRAM_PARTITION_E partition, HUINT32 offset);
DRV_Error drv_NVRAM_Erase(DRV_NVRAM_PARTITION_E partition, HUINT32 offset);
DRV_Error drv_NVRAM_VerifyErased(DRV_NVRAM_PARTITION_E partition, HBOOL *pErased, HUINT32 *eraseOffset);
DRV_Error drv_NVRAM_SetBlockSize(DRV_NVRAM_PARTITION_E partition);
DRV_Error drv_NVRAM_GetBlockSize(DRV_NVRAM_PARTITION_E partition, HUINT32 *flashBlockSize, HUINT32 *nvramBlockSize);
#endif


#endif /* !__DI_FLASH_H__ */



