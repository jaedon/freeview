/*
 * @defgroup        			DI_FS : DI_FS module
 * @author						lslee
 * @note						DI_FS APIs
 * @brief						Define DI_FS APIs
 * @file						di_fs.c											*/
/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<malloc.h>
#endif

#include "htype.h"
#include "di_fs.h"
#include "drv_fs.h"
#include "taskdef.h"
#include "di_uart.h"
#include "di_err.h"
#include "drv_err.h"
#include "vkernel.h"

#include <linden_trace.h>

#include <humax_bsu.h>
#include <bsu_error.h>
#include <drv_bootloader.h>

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define UNUSED(x)		(void)(x)
#define DEBUG_MODULE	TRACE_MODULE_DI_FS

#define FILE_SEEK_BEGINNING	0
#define FILE_SEEK_CURRENT	1
#define FILE_SEEK_END		2

#define FILE_MODE_READ	1
#define FILE_MODE_WRITE	2

/*******************************************************************/
/******************************* extern ****************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/

#if defined(CONFIG_OS_UCOS)
#define INVALID_FILE_HANDLE					1
#define UNKNOWN_DEVICE						2
#define INVALID_DEVICE_HANDLE				3
#define FS_READ_FAIL						4
#define FS_READ_LESS_THAN_REQUESTED			5
#define FS_WRITTEN_LESS_THAN_REQUESTED		6
#define FILESYS_NOT_INITIALIZED				7
#define NO_MORE_FILE_HANDLE					8
#define FS_WRITE_FAIL						9

#define INIT_REQUIRED 						0
#define INITIALIZED							1

typedef enum tagDI_FS_Kind_t
{
	DI_FS_KindOfDevice_NONE,
	DI_FS_KindOfDevice_USB,
	DI_FS_KindOfDevice_TFTP,

	DI_FS_KindOfDevice_MAX
} DI_FS_KindOfDevice_t;

typedef enum tagDI_FS_KindOfFS_t
{
	DI_FS_KindOfFS_NONE,
	DI_FS_KindOfFS_FAT,
	DI_FS_KindOfFS_RAW,
	DI_FS_KindOfFS_ZIP,

	DI_FS_KindOfFS_MAX
} DI_FS_KindOfFS_t;


#define MAX_FILE_OPEN_CNT 					MAX_FS_MOUNT_COUNT
#define MAX_DEVICENAME_LENGTH 				32
#define MAX_MOUNTNAME_LENGTH 				32
#define MAX_FS_NUMBER_OF_DEVICETYPE			8
#define MAX_FS_MOUNT_COUNT					(MAX_FS_NUMBER_OF_DEVICETYPE*DI_FS_KindOfDevice_MAX)

typedef struct tagDI_FS_FILE_CTX_t
{
	void 	*file_prop;
	HUINT32	 fio_index;
} DI_FS_FILE_CTX_t;

typedef struct tagDI_FS_MOUNT_INFO_t
{
	DI_FS_KindOfFS_t 		eKindFs;
	DI_FS_KindOfDevice_t	eKindDev;
	HULONG					ulSemId;

	HINT32	nPartitionIndex;
	HINT32	nMountNameLen;
	HBOOL	bIsMount;
	HCHAR 	szDeviceName[MAX_DEVICENAME_LENGTH];
	HCHAR 	szMountName[MAX_MOUNTNAME_LENGTH];

	/* bolt or cfe include */
	BSU_FileIOCtx_t	pFileIoCtx;

} DI_FS_MOUNT_INFO_t;

static DI_FS_MOUNT_INFO_t 	s_stMountDevices[MAX_FS_MOUNT_COUNT];
static HBOOL 			s_bInitFs;

#endif

DI_ERR_CODE
DI_FS_GetDeviceFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName)
{
	/*
	 * not implement
	 */
	UNUSED(eDevIdx);
	UNUSED(szFileName);
	return 0;
}

DI_ERR_CODE
DI_FS_GetDeviceFileFullName (DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName)
{
	/*
	 * not implement
	 */
	UNUSED(eDevIdx);
	UNUSED(szFileName);
	return 0;
}

DI_ERR_CODE
DI_FS_GetPartitionFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName, HUINT8 nSizeOfFileName)
{
	/*
	 * not implement
	 */
	UNUSED(eDevIdx);
	UNUSED(ulPartitionIdx);
	UNUSED(szFileName);
	UNUSED(nSizeOfFileName);
	return 0;
}

DI_ERR_CODE
DI_FS_GetPartitionFullFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName)
{
	/*
	 * not implement
	 */
	UNUSED(eDevIdx);
	UNUSED(ulPartitionIdx);
	UNUSED(szFileName);
	return 0;
}

/*
 * directory operation
 */
DI_FS_BOOL
DI_FS_IsDirExist(const HCHAR *dirname)
{
	/*
	 * not implement
	 */
	UNUSED(dirname);
	return 0;
}

DI_ERR_CODE
DI_FS_Mkdir(const HCHAR *dirname)
{
	/*
	 * not implement
	 */
	UNUSED(dirname);
	return 0;
}

DI_FS_BOOL
DI_FS_HDDExist(const HCHAR *pucDeviceName)
{
	/*
	 * not implement
	 */
	UNUSED(pucDeviceName);
	return 0;
}

DI_FS_BOOL
DI_FS_IsFileExist(const HCHAR *pathname)
{
	/*
	 * not implement
	 */
	UNUSED(pathname);
	return 0;
}

#if defined(CONFIG_NTFS_RW) || defined(CONFIG_XFS)
STATIC DI_ERR_CODE
local_difs_Read_SuperBlock(const HUINT8 *dev, HUINT32 offset, HUINT32 size, HUINT8 *buf)
{
	/*
	 * not implement
	 */
	UNUSED(dev);
	UNUSED(offset);
	UNUSED(size);
	UNUSED(buf);
	return 0;
}
#endif

#if defined(CONFIG_NTFS_RW)
DI_ERR_CODE
DI_FS_GetVolumeNtfsUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 size)
{
	/*
	 * not implement
	 */
	UNUSED(eDevIdx);
	UNUSED(ulPartIdx);
	UNUSED(buf);
	UNUSED(size);
	return 0;
}
#endif

#if defined(CONFIG_XFS)
DI_ERR_CODE
DI_FS_GetVolumeXfsUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 size)
{
	/*
	 * not implement
	 */
	UNUSED(eDevIdx);
	UNUSED(ulPartIdx);
	UNUSED(buf);
	UNUSED(size);
	return 0;
}
#endif

/***********************************************************************************************************************/
/*************************************************     UCOS    ************************************************************/
/***********************************************************************************************************************/

static HULONG _di_fs_GetSemID(HINT32 idx)
{
	HULONG semId = 0;

	if(MAX_FS_MOUNT_COUNT > idx && idx >= 0)
	{
		semId = s_stMountDevices[idx].ulSemId;
	}

	return semId;
}

static DI_FS_MOUNT_INFO_t *_di_fs_GetMountPartitionInfo(HINT32 idx)
{
	if(MAX_FS_MOUNT_COUNT > idx && idx >= 0)
	{
		return &s_stMountDevices[idx];
	}

	return NULL;
}


static HINT32 _di_fs_GetPartitionIndexByString(const HCHAR *pszPath)
{
	HINT32 i = 0;
	HINT32 idx = 0;

	for(i=0; i < MAX_FS_MOUNT_COUNT;i++)
	{
		VK_SEM_Get(s_stMountDevices[i].ulSemId);
		if(TRUE == s_stMountDevices[i].bIsMount)
		{
			if(0 == VK_strncmp(pszPath, s_stMountDevices[i].szMountName, s_stMountDevices[i].nMountNameLen))
			{
				idx = s_stMountDevices[i].nPartitionIndex;
				VK_SEM_Release(s_stMountDevices[i].ulSemId);

				return idx;
			}
		}
		VK_SEM_Release(s_stMountDevices[i].ulSemId);
	}

	return -1;
}


static HCHAR	*_di_fs_GetTypeText(DI_FS_KindOfDevice_t	 eKindDev, DI_FS_KindOfFS_t eType)
{
	switch(eKindDev)
	{
	case DI_FS_KindOfDevice_TFTP:
		return "tftp";

	case DI_FS_KindOfDevice_USB:
		switch(eType)
		{
		case DI_FS_KindOfFS_RAW:
			return "raw";
		case DI_FS_KindOfFS_FAT:
			return "fat";
		case DI_FS_KindOfFS_ZIP:
			return "z";
		default:
			break;
		}
		break;

	default:
		break;
	}

	return NULL;
}

static DI_ERR_CODE _di_fs_Mount(DI_FS_MOUNT_INFO_t *pInfo)
{
	HINT32			ret;
	HCHAR			*pFSType = NULL;
	BSU_FileIOCtx_t	pFileIo = 0;


	pFSType = _di_fs_GetTypeText(pInfo->eKindDev, pInfo->eKindFs);
	if(NULL == pFSType)
	{
		PrintError("[%s:%d] do not support file system: dev(%d) fs(%d)\n",__FUNCTION__, __LINE__, pInfo->eKindDev, pInfo->eKindFs);
		return DI_ERR_ERROR;
	}

	ret = BSU_FS_init(pFSType, &pFileIo, pInfo->szDeviceName);
	if (ret < 0)
	{
		PrintError("[%s:%d] Could not init file system: %s\n", __FUNCTION__, __LINE__, bsu_errortext(ret));
		return DI_ERR_ERROR;
	}
	pInfo->pFileIoCtx = pFileIo;

	VK_TASK_Sleep(2000);

	return DI_ERR_OK;
}


static DI_ERR_CODE _di_fs_UMount(DI_FS_MOUNT_INFO_t *pInfo)
{
	HINT32 			res = 0;

	res =  BSU_FS_uninit(pInfo->pFileIoCtx);
	if (res < 0)
	{
		PrintError("INT_FS_UMout: error fs_uninit: %s\n", bsu_errortext(res));
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DRV_Error DRV_FS_Init(void)
{
	HINT32	i;
	HUINT32	result;
	HCHAR	semName[32];

	if (s_bInitFs != FALSE)
		return DRV_OK;

	for(i=0; i < MAX_FS_MOUNT_COUNT; i++)
	{
		VK_sprintf(semName, "sem_difs_%x", i);
		result = VK_SEM_Create(&s_stMountDevices[i].ulSemId, semName, VK_SUSPENDTYPE_FIFO);
		if( VK_OK != result)
		{
			PrintError("[DRV_FS_Init] Error(0x%x) in VK_SEM_Create()\n", result);
			return DRV_ERR;
		}

		s_stMountDevices[i].bIsMount = FALSE;
		s_stMountDevices[i].nPartitionIndex = -1;
	}
	s_bInitFs = TRUE;

	return DRV_OK;

}

DI_ERR_CODE DI_FS_Mount(const HCHAR *pucDeviceName, const HCHAR *pucMountPtr, HINT32 partition, HINT32 type /* DI_FS_KindOfDevice_t */)
{
	HULONG		ulSemID = 0;
	DI_ERR_CODE	eDiErr = DI_ERR_OK;

	DI_FS_MOUNT_INFO_t *pInfo = NULL;

	HAPPY(type);

	if(DI_FS_KindOfDevice_NONE > type || type > DI_FS_KindOfDevice_MAX)
	{
		PrintError("[%s:%d] out of type number\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if(MAX_FS_NUMBER_OF_DEVICETYPE <= partition || NULL == pucDeviceName || NULL == pucDeviceName)
	{
		PrintError("[%s:%d] out of partition number or bad argument, partition(%d)\n", __FUNCTION__, __LINE__, partition);
		return DI_ERR_ERROR;
	}

	partition += ((type-1)*MAX_FS_NUMBER_OF_DEVICETYPE);
	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	VK_SEM_Get(ulSemID);

	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo) 
	{
		PrintError("[%s:%d] unknown Partition pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return DI_ERR_ERROR;
	}

	if(TRUE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] Already Mounted, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return DI_ERR_OK;
	}

	VK_strncpy(pInfo->szDeviceName, pucDeviceName, (MAX_DEVICENAME_LENGTH-1));
	VK_strncpy(pInfo->szMountName, pucMountPtr, (MAX_MOUNTNAME_LENGTH-1));
	pInfo->nPartitionIndex = partition;
	pInfo->eKindFs 		= DI_FS_KindOfFS_FAT;
	pInfo->eKindDev 	= (DI_FS_KindOfDevice_t)type;
	pInfo->nMountNameLen= VK_strlen(pInfo->szMountName);

	eDiErr = _di_fs_Mount(pInfo);
	if(DI_ERR_OK == eDiErr)
	{
		pInfo->bIsMount 	= TRUE;
	}

	VK_SEM_Release(ulSemID);

	return eDiErr;
}



DI_ERR_CODE DI_FS_UMount(const HCHAR *pucMountPtr)
{
	HINT32		partition = 0;
	HULONG		ulSemID = 0;
	DI_ERR_CODE	eDiErr = DI_ERR_OK;

	DI_FS_MOUNT_INFO_t *pInfo = NULL;

	partition = _di_fs_GetPartitionIndexByString(pucMountPtr);
	if(0 > partition)
	{
		PrintError("[%s:%d] can not find mount partition index or do not mount partition (%s)\n", __FUNCTION__, __LINE__, pucMountPtr);
		return DI_ERR_INVALID_PARAM;
	}

	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	VK_SEM_Get(ulSemID);

	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo || FALSE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] unknown Partition Info, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return DI_ERR_ERROR;
	}

	eDiErr = _di_fs_UMount(pInfo);
	if(eDiErr != DI_ERR_OK)
	{
		PrintError("[%s:%d] _di_fs_UMount returned Error\n", __FUNCTION__, __LINE__);
		VK_SEM_Release(ulSemID);
		return DI_ERR_ERROR;
	}

	VK_memset(pInfo->szDeviceName, 0, (MAX_DEVICENAME_LENGTH));
	VK_memset(pInfo->szMountName, 0, (MAX_MOUNTNAME_LENGTH));
	pInfo->nPartitionIndex = -1;
	pInfo->eKindFs 			= DI_FS_KindOfDevice_NONE;
	pInfo->eKindDev 		= DI_FS_KindOfDevice_NONE;
	pInfo->pFileIoCtx		= 0;
	pInfo->bIsMount			= FALSE;
	
	VK_SEM_Release(ulSemID);

	return DI_ERR_OK;
}


DI_FS_HANDLE DI_FS_FOpen(const char *pucFileName, const char *pucMode)
{
	HINT32		res = 0;
	HINT32		partition = 0;
	HULONG		ulSemID = 0;
	HINT32		mode = 0;
	void 		*file_prop = NULL;

	const HCHAR		*pOnlyFileName = NULL;
	DI_FS_HANDLE	fileHandle = NULL;

	DI_FS_MOUNT_INFO_t 	*pInfo = NULL;
	DI_FS_FILE_CTX_t	*pFileCtx = NULL;


	if (VK_strstr(pucMode, "w"))
	{
		mode |= FILE_MODE_WRITE;
	}

	if (VK_strstr(pucMode, "r"))
	{
		mode |= FILE_MODE_READ;
	}

	partition = _di_fs_GetPartitionIndexByString(pucFileName);
	if(0 > partition)
	{
		PrintError("[%s:%d] can not find mount partition index or do not mount partition (%s)\n", __FUNCTION__, __LINE__, pucFileName);
		return NULL;
	}

	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	VK_SEM_Get(ulSemID);

	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo || FALSE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] unknown Partition Info, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return NULL;
	}

	pOnlyFileName = pucFileName + pInfo->nMountNameLen;
	if('/' == pOnlyFileName[0])
	{
		pOnlyFileName++;
	}

	res =  BSU_FS_open(pInfo->pFileIoCtx, &file_prop, (char *)pOnlyFileName, mode);
	if (res != 0)
	{
		PrintError("filed to open file %s: %s\n", pucFileName, bsu_errortext(res));
		VK_SEM_Release(ulSemID);
		return NULL;
	}

	fileHandle = 0;

	pFileCtx = VK_MEM_Alloc(sizeof(DI_FS_FILE_CTX_t));
	if(pFileCtx)
	{
		pFileCtx->file_prop = file_prop;
		pFileCtx->fio_index = partition;
		fileHandle  =  (DI_FS_HANDLE)pFileCtx;
	}

	VK_SEM_Release(ulSemID);

	return fileHandle;

}



DI_FS_SIZE DI_FS_FWrite(void *pvBufferPtr, DI_FS_SIZE ulSize, HUINT32 ulCount, DI_FS_HANDLE hFileHandle)
{
	HINT32		res = -1;
	HINT32		partition = 0;
	HULONG		ulSemID = 0;

	DI_FS_MOUNT_INFO_t 	*pInfo = NULL;
	DI_FS_FILE_CTX_t	*pFileCtx = NULL;

	if (NULL == hFileHandle )
	{
		PrintError("[%s:%d] (hFileHandle == NULL)\n", __FUNCTION__, __LINE__);
		return 0;
	}

	pFileCtx = (DI_FS_FILE_CTX_t *)(hFileHandle);
	partition = pFileCtx->fio_index;

	if (pFileCtx->file_prop == NULL)
	{
		PrintError("[%s:%d] pFileCtx->file_prop == NULL\n", __FUNCTION__, __LINE__);
		return 0;
	}

	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return 0;
	}

	ulSize *= ulCount;

	VK_SEM_Get(ulSemID);
	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo || FALSE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] unknown Partition Info, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return 0;
	}

	res = BSU_FS_write(pInfo->pFileIoCtx, pFileCtx->file_prop, pvBufferPtr, ulSize);
	if (res < 0)
	{
		PrintError("failed to write file: %s\n",bsu_errortext(res));
	}
	VK_SEM_Release(ulSemID);

	return (DI_FS_SIZE)res;

}


DI_FS_SIZE DI_FS_FRead(void *pvBufferPtr, DI_FS_SIZE ulSize, HUINT32 ulCount, DI_FS_HANDLE hFileHandle)
{
	HINT32		res = -1;
	HINT32		partition = 0;
	HULONG		ulSemID = 0;

	DI_FS_MOUNT_INFO_t 	*pInfo = NULL;
	DI_FS_FILE_CTX_t	*pFileCtx = NULL;

	if (NULL == hFileHandle )
	{
		PrintError("[%s:%d] (hFileHandle == NULL)\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	pFileCtx = (DI_FS_FILE_CTX_t *)(hFileHandle);
	partition = pFileCtx->fio_index;

	if (pFileCtx->file_prop == NULL)
	{
		PrintError("[%s:%d] pFileCtx->file_prop == NULL\n", __FUNCTION__, __LINE__);
		return 0;
	}

	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return 0;
	}

	ulSize *= ulCount;

	VK_SEM_Get(ulSemID);
	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo || FALSE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] unknown Partition Info, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return 0;
	}

	res = BSU_FS_read(pInfo->pFileIoCtx, pFileCtx->file_prop, pvBufferPtr, ulSize);
	if (res < 0)
	{
		PrintError("failed to read file: %s\n",bsu_errortext(res));
	}
	VK_SEM_Release(ulSemID);

	return (DI_FS_SIZE)res;
}


DI_ERR_CODE DI_FS_FClose(DI_FS_HANDLE hFileHandle)
{
	HINT32		res = -1;
	HINT32		partition = 0;
	HULONG		ulSemID = 0;

	DI_FS_MOUNT_INFO_t 	*pInfo = NULL;
	DI_FS_FILE_CTX_t	*pFileCtx = NULL;

	if (NULL == hFileHandle )
	{
		PrintError("[%s:%d] (hFileHandle == NULL)\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	pFileCtx = (DI_FS_FILE_CTX_t *)(hFileHandle);
	partition = pFileCtx->fio_index;

	if (pFileCtx->file_prop == NULL)
	{
		PrintError("[%s:%d] pFileCtx->file_prop == NULL\n", __FUNCTION__, __LINE__);
		return 0;
	}

	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return 0;
	}

	VK_SEM_Get(ulSemID);
	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo || FALSE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] unknown Partition Info, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return DI_ERR_ERROR;
	}

	res = BSU_FS_close(pInfo->pFileIoCtx, pFileCtx->file_prop);
	if (res != 0)
	{
		PrintError("[DI_FS_FClose] to open file %s\n", bsu_errortext(res));
	}

	VK_SEM_Release(ulSemID);

	return DI_ERR_OK;
}


DI_ERR_CODE DI_FS_FSeek(DI_FS_HANDLE hFileHandle, DI_FS_SIZE offset, DI_FS_ORIGIN_e eOrigin)
{
	HINT32		res = -1;
	HINT32		partition = 0;
	HINT32		seekHow = 0;
	HULONG		ulSemID = 0;
	DI_ERR_CODE	eErrCode = DI_ERR_OK;

	DI_FS_MOUNT_INFO_t 	*pInfo = NULL;
	DI_FS_FILE_CTX_t	*pFileCtx = NULL;

	if (NULL == hFileHandle )
	{
		PrintError("[%s:%d] (hFileHandle == NULL)\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	switch (eOrigin)
	{
	case DI_FS_SEEK_SET :
		seekHow = FILE_SEEK_BEGINNING;
		break;

	case DI_FS_SEEK_CUR :
		seekHow = FILE_SEEK_CURRENT;
		break;

	case DI_FS_SEEK_END :
 		seekHow = FILE_SEEK_END;
		break;

	default :
		return INVALID_DEVICE_HANDLE;
	}

	pFileCtx = (DI_FS_FILE_CTX_t *)(hFileHandle);
	partition = pFileCtx->fio_index;

	if (pFileCtx->file_prop == NULL)
	{
		PrintError("[%s:%d] pFileCtx->file_prop == NULL\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	VK_SEM_Get(ulSemID);
	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo || FALSE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] unknown Partition Info, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return DI_ERR_ERROR;
	}

	eErrCode = DI_ERR_OK;
	res = BSU_FS_seek(pInfo->pFileIoCtx, pFileCtx->file_prop, offset, seekHow);
	if (res < 0)
	{
		PrintDebug("failed to read file: %s\n", bsu_errortext(res));
		eErrCode = DI_ERR_ERROR;
	}

	VK_SEM_Release(ulSemID);

	return eErrCode;

}


DI_FS_SIZE DI_FS_FTell(DI_FS_HANDLE hFileHandle)
{
	HINT32		partition = 0;
	HINT32		seekHow = 0;
	DI_FS_SIZE	curroffset = 0;
	HULONG		ulSemID = 0;

	DI_FS_MOUNT_INFO_t 	*pInfo = NULL;
	DI_FS_FILE_CTX_t	*pFileCtx = NULL;

	if (NULL == hFileHandle )
	{
		PrintError("[%s:%d] (hFileHandle == NULL)\n", __FUNCTION__, __LINE__);
		return 0;
	}

	pFileCtx = (DI_FS_FILE_CTX_t *)(hFileHandle);
	partition = pFileCtx->fio_index;

	if (pFileCtx->file_prop == NULL)
	{
		PrintError("[%s:%d] pFileCtx->file_prop == NULL\n", __FUNCTION__, __LINE__);
		return 0;
	}

	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return 0;
	}

	seekHow = FILE_SEEK_CURRENT;

	VK_SEM_Get(ulSemID);
	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo || FALSE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] unknown Partition Info, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return 0;
	}

	curroffset = BSU_FS_seek(pInfo->pFileIoCtx, pFileCtx->file_prop, 0, seekHow);

	VK_SEM_Release(ulSemID);

	return curroffset;

}

DI_FS_DIRENT_HANDLE DI_FS_FOpenDir(const HCHAR *pucPath)
{
	HINT32		res = 0;
	HINT32		partition = 0;
	HULONG		ulSemID = 0;
	HINT32		mode = 0;
	void 		*file_prop = NULL;

	const HCHAR		*pOnlyPath = NULL;
	DI_FS_DIRENT_HANDLE	direntHandle = NULL;

	DI_FS_MOUNT_INFO_t 	*pInfo = NULL;
	DI_FS_FILE_CTX_t	*pFileCtx = NULL;

	partition = _di_fs_GetPartitionIndexByString(pucPath);
	if(0 > partition)
	{
		PrintError("[%s:%d] can not find mount partition index or do not mount partition (%s)\n", __FUNCTION__, __LINE__, pucPath);
		return NULL;
	}

	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	VK_SEM_Get(ulSemID);

	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo || FALSE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] unknown Partition Info, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return NULL;
	}
	
	pOnlyPath = pucPath + pInfo->nMountNameLen;
	if('/' == pOnlyPath[0])
	{
		pOnlyPath++;
	}
	
	if((pOnlyPath[0] == '\0') || 
		((pOnlyPath[0] == '/' || pOnlyPath[0] == '\\') && pOnlyPath[1] == '\0') )
	{
		pOnlyPath = NULL;
	}
	res =  BSU_FS_opendir(pInfo->pFileIoCtx, &file_prop, (char *)pOnlyPath);
	
	if (res != 0)
	{
		PrintError("filed to open file %s: %s\n", pucPath, bsu_errortext(res));
		VK_SEM_Release(ulSemID);
		return NULL;
	}

	pFileCtx = VK_MEM_Alloc(sizeof(DI_FS_FILE_CTX_t));
	if(pFileCtx)
	{
		pFileCtx->file_prop = file_prop;
		pFileCtx->fio_index = partition;
		direntHandle  =  (DI_FS_DIRENT_HANDLE)pFileCtx;
	}

	VK_SEM_Release(ulSemID);
	
	return direntHandle;
}

DI_ERR_CODE DI_FS_FReadDir(DI_FS_DIRENT_HANDLE hDirentHandle, DI_FS_DIRENT_INFO_t *pDirentInfo)
{
	DI_ERR_CODE	eDiErr = DI_ERR_ERROR;
	HINT32		res = -1;
	HINT32		partition = 0;
	HULONG		ulSemID = 0;

	DI_FS_MOUNT_INFO_t 	*pInfo = NULL;
	DI_FS_FILE_CTX_t	*pFileCtx = NULL;

	HUINT32 node = 0;
	HUINT32 offset = 0;
	HUINT32 reclen = 0;
	HUINT8 type = 0;
	HCHAR name[DI_FS_DIRENT_MAX_NAME_LEN];
			
	if (NULL == hDirentHandle )
	{
		PrintError("[%s:%d] (hDirentHandle == NULL)\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	pFileCtx = (DI_FS_FILE_CTX_t *)(hDirentHandle);
	partition = pFileCtx->fio_index;

	if (pFileCtx->file_prop == NULL)
	{
		PrintError("[%s:%d] pFileCtx->file_prop == NULL\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	VK_SEM_Get(ulSemID);
	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo || FALSE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] unknown Partition Info, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return DI_ERR_ERROR;
	}
	
 	res = BSU_FS_readdir(pInfo->pFileIoCtx, pFileCtx->file_prop, &node, &offset, &reclen, &type, &name);

	if (res == 0) 
	{
		pDirentInfo->node = node;
		pDirentInfo->offset = offset;
		pDirentInfo->reclen = reclen;
		pDirentInfo->type = type;
		VK_snprintf(pDirentInfo->name, DI_FS_DIRENT_MAX_NAME_LEN, "%s", name);
		eDiErr = DI_ERR_OK;
	}
	
	VK_SEM_Release(ulSemID);
	
	return eDiErr;	
}

DI_ERR_CODE DI_FS_FCloseDir(DI_FS_DIRENT_HANDLE hDirentHandle)
{
	HINT32		res = -1;
	HINT32		partition = 0;
	HULONG		ulSemID = 0;

	DI_FS_MOUNT_INFO_t 	*pInfo = NULL;
	DI_FS_FILE_CTX_t	*pFileCtx = NULL;
	
	if (NULL == hDirentHandle )
	{
		PrintError("[%s:%d] (hDirentHandle == NULL)\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	pFileCtx = (DI_FS_FILE_CTX_t *)(hDirentHandle);
	partition = pFileCtx->fio_index;

	if (pFileCtx->file_prop == NULL)
	{
		PrintError("[%s:%d] pFileCtx->file_prop == NULL\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	ulSemID = _di_fs_GetSemID(partition);
	if(0 == ulSemID)
	{
		PrintError("[%s:%d] Error Sem ID\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	VK_SEM_Get(ulSemID);
	pInfo = _di_fs_GetMountPartitionInfo(partition);
	if(NULL == pInfo || FALSE == pInfo->bIsMount)
	{
		PrintError("[%s:%d] unknown Partition Info, pInfo(0x%x)\n", __FUNCTION__, __LINE__, (HUINT32)pInfo);
		VK_SEM_Release(ulSemID);
		return DI_ERR_ERROR;
	}
	
	res = BSU_FS_closedir(pInfo->pFileIoCtx, pFileCtx->file_prop);
	if (res != 0)
	{
		PrintError("[DI_FS_FClose] to open file %s\n", bsu_errortext(res));
	}
	
	VK_SEM_Release(ulSemID);
	
	return DI_ERR_OK;
}
