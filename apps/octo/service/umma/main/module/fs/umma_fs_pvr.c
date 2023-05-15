/**
	@file     fs_pvr.c
	@brief    repository service	\n

	Description: 		\n
	Module: mw/pvr			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vkernel.h>

#include <umma_fs_pal_fs.h>
#include <umma_fs_pal_hdd.h>
#include <umma_fs_pal_usb.h>
#include <umma_fs_pal_mmc.h>

#include <dlib.h>
#include <hlib.h>
#include <oapi.h>
#include <umma_int.h>

#include "umma_fs_int.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define	LOG					HxLOG_Print
#define INFO_LOG			HxLOG_Info
#define	ERR_LOG				HxLOG_Error
#define MSG_LOG				HxLOG_Message
#define	FuncENTER			HxLOG_Info("(+)Enter!!\n");
#define	FuncLEAVE(X)		HxLOG_Info("(-)Leave -- (0x%X)\n",X);
#else
#define	LOG					OCTO_NULL_PRINT
#define INFO_LOG			OCTO_NULL_PRINT
#define	ERR_LOG				OCTO_NULL_PRINT
#define MSG_LOG				OCTO_NULL_PRINT
#define	FuncENTER			OCTO_NULL_PRINT
#define	FuncLEAVE(X)		OCTO_NULL_PRINT
#endif


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Functions     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define ________PVR_GET_ALLPATH________

HERROR umma_fs_svc_PvrGetAllPvrPath(UMMA_FS_DevType_e eDevType,UMMA_FS_PartitionInfo_t *PartitionInfo )
{
	HERROR hErr;

	hErr = umma_fs_svc_DirGetPathFromMountedRootPath (PartitionInfo->szMountPath, eDevType, eFS_UsageType_Record, PartitionInfo->szRecordPath, UMMA_FS_MOUNT_PATH_LEN-1);
	if(hErr != ERR_OK)			VK_memset(PartitionInfo->szRecordPath,0,UMMA_FS_MOUNT_PATH_LEN);

	hErr = umma_fs_svc_DirGetPathFromMountedRootPath (PartitionInfo->szMountPath, eDevType, eFS_UsageType_Tsr, PartitionInfo->szTsrPath, UMMA_FS_MOUNT_PATH_LEN-1);
	if(hErr != ERR_OK)			VK_memset(PartitionInfo->szTsrPath,0,UMMA_FS_MOUNT_PATH_LEN);

	hErr = umma_fs_svc_DirGetPathFromMountedRootPath (PartitionInfo->szMountPath, eDevType, eFS_UsageType_MediaPath, PartitionInfo->szMediaPath, UMMA_FS_MOUNT_PATH_LEN-1);
	if(hErr != ERR_OK)			VK_memset(PartitionInfo->szMediaPath,0,UMMA_FS_MOUNT_PATH_LEN);

	hErr = umma_fs_svc_DirGetPathFromMountedRootPath (PartitionInfo->szMountPath, eDevType, eFS_UsageType_MediaVideo, PartitionInfo->szMediaVideo, UMMA_FS_MOUNT_PATH_LEN-1);
	if(hErr != ERR_OK)			VK_memset(PartitionInfo->szMediaVideo,0,UMMA_FS_MOUNT_PATH_LEN);

	hErr = umma_fs_svc_DirGetPathFromMountedRootPath (PartitionInfo->szMountPath, eDevType, eFS_UsageType_MediaAudio, PartitionInfo->szMediaAudio, UMMA_FS_MOUNT_PATH_LEN-1);
	if(hErr != ERR_OK)			VK_memset(PartitionInfo->szMediaAudio,0,UMMA_FS_MOUNT_PATH_LEN);

	hErr = umma_fs_svc_DirGetPathFromMountedRootPath (PartitionInfo->szMountPath, eDevType, eFS_UsageType_MediaPic, PartitionInfo->szMediaPic, UMMA_FS_MOUNT_PATH_LEN-1);
	if(hErr != ERR_OK)			VK_memset(PartitionInfo->szMediaPic,0,UMMA_FS_MOUNT_PATH_LEN);

	hErr = umma_fs_svc_DirGetPathFromMountedRootPath (PartitionInfo->szMountPath, eDevType, eFS_UsageType_MediaDownload, PartitionInfo->szMediaDownload, UMMA_FS_MOUNT_PATH_LEN-1);
	if(hErr != ERR_OK)			VK_memset(PartitionInfo->szMediaDownload,0,UMMA_FS_MOUNT_PATH_LEN);

	HxLOG_Print("[%s:%d]bPvrFlag [%d]\n",__FUNCTION__,__LINE__, PartitionInfo->bPvrFlag);
	HxLOG_Print("[%s:%d]bSystemFlag [%d]\n",__FUNCTION__,__LINE__, PartitionInfo->bSystemFlag);
	HxLOG_Print("[%s:%d]szMountPath [%s]\n",__FUNCTION__,__LINE__, PartitionInfo->szMountPath);
	HxLOG_Print("[%s:%d]szVolumeName [%s]\n",__FUNCTION__,__LINE__, PartitionInfo->szVolumeName);
	HxLOG_Print("[%s:%d]eFS_UsageType_Record [%s]\n",__FUNCTION__,__LINE__, PartitionInfo->szRecordPath);
	HxLOG_Print("[%s:%d]eFS_UsageType_Tsr [%s]\n",__FUNCTION__,__LINE__, PartitionInfo->szTsrPath);
	HxLOG_Print("[%s:%d]eFS_UsageType_MediaPath [%s]\n",__FUNCTION__,__LINE__,  PartitionInfo->szMediaPath);
	HxLOG_Print("[%s:%d]eFS_UsageType_MediaVideo [%s]\n",__FUNCTION__,__LINE__,  PartitionInfo->szMediaVideo);
	HxLOG_Print("[%s:%d]eFS_UsageType_MediaAudio [%s]\n",__FUNCTION__,__LINE__,  PartitionInfo->szMediaAudio);
	HxLOG_Print("[%s:%d]eFS_UsageType_MediaPic [%s]\n",__FUNCTION__,__LINE__,  PartitionInfo->szMediaPic);
	HxLOG_Print("[%s:%d]eFS_UsageType_Download [%s]\n",__FUNCTION__,__LINE__,  PartitionInfo->szMediaDownload);

	return ERR_OK;
}
HERROR umma_fs_svc_PvrGetPlaybackPvrPath(UMMA_FS_DevType_e eDevType,UMMA_FS_PartitionInfo_t *PartitionInfo )
{
	HERROR hErr;

	hErr = umma_fs_svc_DirGetPathFromMountedRootPath (PartitionInfo->szMountPath, eDevType, eFS_UsageType_Record, PartitionInfo->szRecordPath, UMMA_FS_MOUNT_PATH_LEN-1);
	if(hErr != ERR_OK)			VK_memset(PartitionInfo->szRecordPath,0,UMMA_FS_MOUNT_PATH_LEN);

	HxLOG_Print("[%s:%d]bPvrFlag [%d]\n",__FUNCTION__,__LINE__, PartitionInfo->bPvrFlag);
	HxLOG_Print("[%s:%d]bSystemFlag [%d]\n",__FUNCTION__,__LINE__, PartitionInfo->bSystemFlag);
	HxLOG_Print("[%s:%d]szMountPath [%s]\n",__FUNCTION__,__LINE__, PartitionInfo->szMountPath);
	HxLOG_Print("[%s:%d]szVolumeName [%s]\n",__FUNCTION__,__LINE__, PartitionInfo->szVolumeName);
	HxLOG_Print("[%s:%d]eFS_UsageType_Record [%s]\n",__FUNCTION__,__LINE__, PartitionInfo->szRecordPath);

	return ERR_OK;
}

#define _____INTERNAL_FUNCTIONS_____

#define HDD_DEFAULT_INFO_NAME		CONFIG_PRODUCT_DATA_DIR "/hdd_default_info"
static HERROR _umma_fs_svc_PvrGetDefaultHddInfo(HUINT8 *pszInfo, HUINT32 *ulMaxRec)
{
	PAL_FS_UINT32	ulExtFileDsc, ulSize;
	HINT8		szMaxRec[5]={0, };
	HERROR			hErr;

	hErr = HLIB_FILE_IsValid(HDD_DEFAULT_INFO_NAME);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = PAL_FS_Open (0, HDD_DEFAULT_INFO_NAME, PAL_FS_MODE_READ, &ulExtFileDsc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[_umma_fs_svc_PvrGetDefaultHddInfo] PAL_FS_Open err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	hErr = PAL_FS_Read (ulExtFileDsc, pszInfo, UMMA_FS_DEV_UUID_BUFSIZE-1, &ulSize);
	if (hErr != ERR_OK || ulSize <= 0)
	{
		HxLOG_Error("[_umma_fs_svc_PvrGetDefaultHddInfo] PAL_FS_Read err(0x%08x)\n", hErr);
		PAL_FS_Close (ulExtFileDsc);
		return ERR_FAIL;
	}

	hErr = PAL_FS_Seek (ulExtFileDsc, UMMA_FS_DEV_UUID_BUFSIZE + 1, 0, FALSE);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_svc_PvrRegisterPvrHdd] PAL_FS_Seek err(0x%08x)\n", hErr);
		PAL_FS_Close (ulExtFileDsc);
		return ERR_FAIL;
	}

	hErr = PAL_FS_Read (ulExtFileDsc, szMaxRec, 4, &ulSize);
	if (hErr != ERR_OK || ulSize <= 0)
	{
		HxLOG_Error("[_umma_fs_svc_PvrGetDefaultHddInfo] PAL_FS_Read err(0x%08x)\n", hErr);
		PAL_FS_Close (ulExtFileDsc);
		return ERR_FAIL;
	}

	*ulMaxRec = HLIB_STD_StrToUINT32(szMaxRec);

	if(*ulMaxRec > CONFIG_MW_MM_MAX_REC_NUM)
		*ulMaxRec = CONFIG_MW_MM_MAX_REC_NUM;

	PAL_FS_Close (ulExtFileDsc);

	return ERR_OK;
}
static HERROR umma_fs_svc_PvrSetDefaultHddInfo(HUINT8 *pszInfo, HUINT32 ulMaxRec)
{
	PAL_FS_UINT32	ulExtFileDsc, ulSize;
	HINT8		szMaxRec[5]={ 0, };
	HERROR			hErr;

	// HDD에 등록 파일을 생성하여 Unique info를 write.
	hErr = PAL_FS_Open (0, HDD_DEFAULT_INFO_NAME, PAL_FS_MODE_RDWR, &ulExtFileDsc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_svc_PvrRegisterPvrHdd] PAL_FS_Open err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	hErr = PAL_FS_Write (ulExtFileDsc, pszInfo, UMMA_FS_DEV_UUID_BUFSIZE, &ulSize);
	if (hErr != ERR_OK || ulSize <= 0)
	{
		HxLOG_Error("[umma_fs_svc_PvrRegisterPvrHdd] PAL_FS_Write err(0x%08x)\n", hErr);
		PAL_FS_Close (ulExtFileDsc);
		return ERR_FAIL;
	}

	hErr = PAL_FS_Seek (ulExtFileDsc, UMMA_FS_DEV_UUID_BUFSIZE + 1, 0, FALSE);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_svc_PvrRegisterPvrHdd] PAL_FS_Seek err(0x%08x)\n", hErr);
		PAL_FS_Close (ulExtFileDsc);
		return ERR_FAIL;
	}

	if(ulMaxRec > CONFIG_MW_MM_MAX_REC_NUM)
		ulMaxRec = CONFIG_MW_MM_MAX_REC_NUM;

	HxSTD_snprintf(szMaxRec, 4, "%d", ulMaxRec);

	hErr = PAL_FS_Write (ulExtFileDsc, szMaxRec, 4, &ulSize);
	if (hErr != ERR_OK || ulSize <= 0)
	{
		HxLOG_Error("[umma_fs_svc_PvrRegisterPvrHdd] PAL_FS_Write err(0x%08x)\n", hErr);
		PAL_FS_Close (ulExtFileDsc);
		return ERR_FAIL;
	}

	PAL_FS_Close (ulExtFileDsc);

	return ERR_OK;
}

HERROR umma_fs_svc_PvrConvertMwFormatListToPalFormatList (UMMA_FS_FormatPartList_t *pstMwFormatList, PAL_FS_FormatList_t *pstPalFormatList)
{
	HUINT32					 ulIndex;
	PAL_FS_FormatItem_t		 stPalFormatItem;

	if (pstMwFormatList == NULL || pstPalFormatList == NULL)		{ return ERR_FAIL; }

	PAL_FS_InitFormatList (pstPalFormatList);

	for (ulIndex = 0; ulIndex < pstMwFormatList->ulPartitionNum; ulIndex++)
	{
		stPalFormatItem.eDevIdx			= umma_fs_dev_ConvertMwDevIdxToPalDevIdx (pstMwFormatList->eDevIdx);
		stPalFormatItem.ulPartitionNo	= pstMwFormatList->astPartArray[ulIndex].ulPartIdx;
		VK_strncpy (stPalFormatItem.szMountPath, pstMwFormatList->astPartArray[ulIndex].szMountPath, 32-1);
		stPalFormatItem.szMountPath[32-1] = '\0';
		VK_strncpy (stPalFormatItem.szVolumeName, pstMwFormatList->astPartArray[ulIndex].szVolumeName, VOLNAMSZ-1);
		stPalFormatItem.szVolumeName[VOLNAMSZ-1] = '\0';
		stPalFormatItem.ePartFsType		= umma_fs_dev_ConvertMwFsTypeToPalFsType (pstMwFormatList->astPartArray[ulIndex].ePartFsType);
		stPalFormatItem.ulOption		= PAL_FS_EXT_OPTION_DEFAULT;

		HxLOG_Print("[%s:%d] idx(%d), partno(%d), partType(%d)\n", __FUNCTION__,__LINE__,
			ulIndex, stPalFormatItem.ulPartitionNo, stPalFormatItem.ePartFsType);

		PAL_FS_AddFormatItemToList (pstPalFormatList, &stPalFormatItem);
	}

	return ERR_OK;
}

HERROR umma_fs_svc_WritePvrRegisterInfoToHDD(UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx)
{
	PAL_FS_UINT32	ulExtFileDsc, ulSize = 0;
	HUINT8			szStbInfo[UMMA_FS_STB_UNIQUE_BUFSIZE];
	HUINT8			szFileName[UMMA_FS_MOUNT_PATH_LEN+UMMA_FS_EXTHDD_REGFILE_LEN] = {0,};
	HERROR			hErr;
	UMMA_FS_DevContext_t		*pstDevice = NULL;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_PrintToStrN(szFileName, UMMA_FS_MOUNT_PATH_LEN+UMMA_FS_EXTHDD_REGFILE_LEN,"%s%s", pstDevice->pstPartArray[ulPartIdx].szMountPath, UMMA_FS_EXTHDD_REGFILE);

	HxLOG_Print("[umma_fs_svc_PvrRegisterPvrHdd] filename : %s\n", szFileName);

	VK_memset (szStbInfo, 0, UMMA_FS_STB_UNIQUE_BUFSIZE);

	// STB unique 정보를 받아온다.
	hErr =umma_fs_svc_PvrGetStbUniqueInfo(szStbInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_svc_PvrRegisterPvrHdd] umma_fs_svc_PvrGetStbUniqueInfo err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}
	HxLOG_Debug("szStbInfo : %x %x %x %x %x %x %x %x \n", szStbInfo[0], szStbInfo[1], szStbInfo[2], szStbInfo[3], szStbInfo[4], szStbInfo[5], szStbInfo[6], szStbInfo[7] );

	// HDD에 등록 파일을 생성하여 Unique info를 write.
	hErr = PAL_FS_Open (0, szFileName, PAL_FS_MODE_RDWR, &ulExtFileDsc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_svc_PvrRegisterPvrHdd] PAL_FS_Open err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	hErr = PAL_FS_Write (ulExtFileDsc, szStbInfo, UMMA_FS_STB_UNIQUE_BUFSIZE, &ulSize);
	if (hErr != ERR_OK || ulSize <= 0)
	{
		HxLOG_Error("[umma_fs_svc_PvrRegisterPvrHdd] PAL_FS_Write err(0x%08x)\n", hErr);
		PAL_FS_Close (ulExtFileDsc);
		return ERR_FAIL;
	}
	PAL_FS_Close (ulExtFileDsc);

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	if (HLIB_EXTRA_ChownWithParam(szFileName, UID_umma, GID_pvr) != ERR_OK)
	{
		HxLOG_Error("HLIB_EXTRA_ChownWithParam (%s) err (0x%08x)\n", szFileName, hErr);
	}

	if (HLIB_DIR_Chmod(szFileName, 0770) != ERR_OK)
	{
		HxLOG_Error ("HLIB_DIR_Chmod (%s) err (0x%08x)\n", szFileName, hErr);
	}
#endif

	return ERR_OK;

}

#define _____PVR_INT_CHECK_____


#define _____PVR_EVENT_NOTIFIER_____

// ulParam1 : Partition Index
// ulParam2 : PAL_FS_StorDevIdx_t
STATIC HERROR umma_fs_svc_PvrProcessEventFormatProgress (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HUINT32				 ulPartIdx;
	UMMA_FS_DevIdx_e		 eDevIdx;
	HUINT32 			ulPartionNum, ulTotalPartionNum, ulProgress;

	eDevIdx = umma_fs_dev_ConvertPalDevIdxToMwDevIdx ((PAL_FS_StorDevIdx_t)ulParam1);
	ulPartIdx = ulParam2;

	HxLOG_Print("[umma_fs_svc_PvrProcessEventFormatProgress] EVT_FS_FORMAT_PROGRESS (%d)(%d) >>>\n", eDevIdx, ulPartIdx);

	ulPartionNum = (HUINT32)ulParam2;
	ulTotalPartionNum = (HUINT32)ulParam3;
	ulProgress = (0 == ulTotalPartionNum) ? 0 : (ulPartionNum * 100) / ulTotalPartionNum;

	(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_FORMATTING, eDevIdx, ulProgress, 0);

	NOT_USED_PARAM (ulParam1);
	NOT_USED_PARAM (ulParam2);
	NOT_USED_PARAM (ulParam3);
	return ERR_OK;
}

STATIC HERROR umma_fs_svc_PvrProcessEventFormatDone (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	UMMA_FS_DevIdx_e			 eDevIdx;

	eDevIdx = umma_fs_dev_ConvertPalDevIdxToMwDevIdx ((PAL_FS_StorDevIdx_t)ulParam1);

	HxLOG_Print("[umma_fs_svc_PvrProcessEventFormatDone] EVT_FS_FORMAT_DONE (%d) >>>\n", eDevIdx);

	(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_FORMAT_DONE, (HINT32)eDevIdx, 0, 0);

	NOT_USED_PARAM (ulParam1);
	NOT_USED_PARAM (ulParam2);
	NOT_USED_PARAM (ulParam3);

	return ERR_OK;
}

STATIC HERROR umma_fs_svc_PvrProcessEventFormatFail (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	UMMA_FS_DevIdx_e			 eDevIdx;

	eDevIdx = umma_fs_dev_ConvertPalDevIdxToMwDevIdx ((PAL_FS_StorDevIdx_t)ulParam1);

	HxLOG_Warning("EVT_FS_FORMAT_FAIL (%d) >>>\n", eDevIdx);

	(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_FORMAT_FAIL, (HINT32)eDevIdx, 0, 0);

	NOT_USED_PARAM (ulParam1);
	NOT_USED_PARAM (ulParam2);
	NOT_USED_PARAM (ulParam3);

	return ERR_OK;
}

STATIC void umma_fs_svc_PvrFormatEventNotifier (HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HERROR		 hErr;

	switch (ulEvent)
	{
	case PAL_FS_EVENT_FORMAT_PROGRESS:
		hErr = umma_fs_svc_PvrProcessEventFormatProgress (ulParam1, ulParam2, ulParam3);
		HxLOG_Warning("[%s] PAL_FS_EVENT_FORMAT_PROGRESS (%d:%d:%d) Err (0x%08x)\n",__FUNCTION__, ulParam1, ulParam2, ulParam3, hErr);
		break;

	case PAL_FS_EVENT_FORMAT_DONE:
		hErr = umma_fs_svc_PvrProcessEventFormatDone (ulParam1, ulParam2, ulParam3);
		HxLOG_Warning("[%s] PAL_FS_EVENT_FORMAT_DONE (%d:%d:%d) Err (0x%08x)\n",__FUNCTION__, ulParam1, ulParam2, ulParam3, hErr);
		break;

	case PAL_FS_EVENT_FORMAT_FAIL:
		hErr = umma_fs_svc_PvrProcessEventFormatFail (ulParam1, ulParam2, ulParam3);
		HxLOG_Warning("[%s] PAL_FS_EVENT_FORMAT_FAIL (%d:%d:%d) Err (0x%08x)\n",__FUNCTION__, ulParam1, ulParam2, ulParam3, hErr);
		break;

	default:
		HxLOG_Critical("\nCritical Error!!!\n\n");
		break;
	}

}

#define _____PARTITION_MODIFIER_____

STATIC HERROR umma_fs_svc_PvrSelectMwFormatListByPartType (UMMA_FS_PvrPartitionType_e ePartType, UMMA_FS_FormatPartList_t *pstFormatList)
{
	HUINT32					 ulIndex, ulDstIndex, ulNewNumber;
	UMMA_FS_FormatPartInfo_t	*pstFormatItem, *pstDstFormatItem;

	if (ePartType == eFS_PvrPartitionType_None || pstFormatList == NULL)
	{
		return ERR_FAIL;
	}

	ulNewNumber = 0;
	for (ulIndex = 0; ulIndex < pstFormatList->ulPartitionNum; ulIndex++)
	{
		pstFormatItem = &(pstFormatList->astPartArray[ulIndex]);
		if ((pstFormatItem->ePartType & ePartType) == 0)
		{
			VK_memset (pstFormatItem, 0, sizeof(UMMA_FS_FormatPartInfo_t));
			pstFormatItem->ePartType = eFS_PvrPartitionType_None;
		}
		else
		{
			ulNewNumber ++;
		}
	}

	if (ulNewNumber == 0)
	{
		pstFormatList->ulPartitionNum = 0;
		return ERR_FAIL;
	}

	// 해당되지 않아 삭제한 Item들의 자리를 다 shift 시켜준다.
	for (ulIndex = 0, ulDstIndex = 0; ulIndex < pstFormatList->ulPartitionNum; ulIndex++)
	{
		pstFormatItem 	 = &(pstFormatList->astPartArray[ulIndex]);
		pstDstFormatItem = &(pstFormatList->astPartArray[ulDstIndex]);

		if (pstFormatItem->ePartType == eFS_PvrPartitionType_None)
		{
			continue;
		}

		if (ulIndex != ulDstIndex)
		{
			VK_memcpy (pstDstFormatItem, pstFormatItem, sizeof(UMMA_FS_FormatPartInfo_t));
		}

		ulDstIndex++;
	}

	pstFormatList->ulPartitionNum = ulDstIndex;
	return ERR_OK;
}

#define _____CHECK_PVR_POSSIBILITY_____

STATIC HBOOL umma_fs_svc_PvrCheckRemainSize (HUINT8 *pucMountPath, HUINT64 ullTotalKb, HUINT64 ullUsedKb, HUINT64 ullReservedKb, HBOOL bCheckTsr)
{
	HBOOL		 bExist;
	HUINT8		*szMountedPath = NULL, *szTsrPath = NULL;
	HERROR		 hErr;
	HBOOL		 bResult = FALSE;
	PAL_FS_UINT64		 ullTsrTotalSize = 0;

	if (bCheckTsr == TRUE)
	{
		if (pucMountPath == NULL)				{ return FALSE; }

		szMountedPath = (HUINT8 *)UMMA_Malloc (64);
		szTsrPath = (HUINT8 *)UMMA_Malloc (64);
		if (szMountedPath == NULL || szTsrPath == NULL)
		{
			goto ERROR;
		}

		VK_strncpy (szMountedPath, pucMountPath, 64);
		szMountedPath[63] = '\0';

		umma_fs_dev_AppendSeparatorToPath (szMountedPath);
		bExist = HLIB_DIR_IsExist (szMountedPath);
		if (bExist == FALSE)
		{
			HxLOG_Error("[umma_fs_svc_PvrCheckRemainSize] PAL_FS_IsDirExist (%s) err(FALSE)\n", szMountedPath);
			goto ERROR;
		}

		hErr = umma_fs_svc_DirGetPathFromMountedRootPath (szMountedPath, eFS_DevType_IntHdd, eFS_UsageType_Tsr, szTsrPath, 64);
		HxLOG_Print("[%s/%d] mount Path (%s) TSR Path (%s) \n", __FUNCTION__, __LINE__, szMountedPath, szTsrPath);
		if (hErr == ERR_OK)
		{
			PAL_FS_EstimateSpace (szTsrPath, &ullTsrTotalSize);
			ullTsrTotalSize /= SIZE_1K;			// Byte 단위를 KB 단위로 바꾼다.  : 다른 수치들이 다 KB 단위이다.
			if (ullTsrTotalSize > 0)
			{
				//실제 TSR 파일의 크기를 minus 후, RsvdSize(TSR 용 20GB or 40GB) 를 plus 해야 정확한 usage_size 가 나온다.
				HxLOG_Print("[%s/%d] org usedMB (%d) TsrMB  	(%d)\n"
					, __FUNCTION__, __LINE__, (HUINT32)(ullUsedKb/1024L), (HUINT32)(ullTsrTotalSize/1024L));
				if ((HUINT64)ullTsrTotalSize <= ullUsedKb)
				{
					ullUsedKb -= (HUINT64)ullTsrTotalSize;
				}
			}
		}

		// Reserved Size가 남아 있는가?
		HxLOG_Print("[%s/%d] reservedMB	(%d)\n", __FUNCTION__, __LINE__, (HUINT32)(ullReservedKb/1024L));
		HxLOG_Print("[%s/%d] totlaMB 	(%d)\n", __FUNCTION__, __LINE__, (HUINT32)(ullTotalKb/1024L));
		HxLOG_Print("[%s/%d] usedMB  	(%d)\n", __FUNCTION__, __LINE__, (HUINT32)(ullUsedKb/1024L));
		HxLOG_Print("[%s/%d] TsrMB  		(%d)\n", __FUNCTION__, __LINE__, (HUINT32)(ullTsrTotalSize/1024L));

		if (ullUsedKb < ullTotalKb && (ullReservedKb + DxSVC_FS_PVR_SAFE_AREA_SIZE_KB) <= (ullTotalKb - ullUsedKb) )
		{
			HxLOG_Print("[%s/%d] avaliable (%s) !!!\n", __FUNCTION__, __LINE__, pucMountPath);
			bResult = TRUE;
		}
	}
	else
	{
		// Reserved Size가 남아 있는가?
		HxLOG_Print("[%s/%d] totlaMB 	(%d)\n", __FUNCTION__, __LINE__, (HUINT32)(ullTotalKb/1024L));
		HxLOG_Print("[%s/%d] usedMB  	(%d)\n", __FUNCTION__, __LINE__, (HUINT32)(ullUsedKb/1024L));

		if (ullUsedKb < ullTotalKb &&  DxSVC_FS_PVR_SAFE_AREA_SIZE_KB <= (ullTotalKb - ullUsedKb) )
		{
			HxLOG_Print("[%s/%d] avaliable (%s) !!!\n", __FUNCTION__, __LINE__, pucMountPath);
			bResult = TRUE;
		}
	}

ERROR:
	if (szMountedPath != NULL)					{ UMMA_Free (szMountedPath); }
	if (szTsrPath != NULL)						{ UMMA_Free (szTsrPath); }

	return bResult;
}


#define _____FS_LOCAL_FUNCTIONS_____

HERROR umma_fs_svc_PvrInit (void)
{
	PAL_FS_RegisterFormatCallback (umma_fs_svc_PvrFormatEventNotifier);

	return ERR_OK;
}

UMMA_FS_DevStatus_e umma_fs_svc_PvrGetPvrHddStatus (HUINT32 ulPvrIdx)
{
	HUINT32				 ulPartIdx;
	UMMA_FS_DevIdx_e		 ePvrDevIdx = eFS_Dev_NULL;
//	UMMA_FS_DevInfoMap_t		*pstContext;
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	HERROR				 hErr;

//	pstContext = umma_fs_dev_GetContext();

	hErr = umma_fs_svc_PvrFindPvrDevIdPartIdx (ulPvrIdx, &ePvrDevIdx, &ulPartIdx);
	if (hErr != ERR_OK || ePvrDevIdx == eFS_Dev_NULL)
	{
		// PVR HDD를 Format 하는 중이라 PVR Device로 등록되어 있지 않을 수도 있다.
		return eFS_DevStatus_NoDevice;
	}

	hErr = umma_fs_dev_GetDeviceInfo (ePvrDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		return eFS_DevStatus_NoDevice;
	}

	switch (pstDevice->eDevStatus)
	{
	case eFS_DevStatus_OK:
		if (pstDevice->bDevConnected == TRUE && pstDevice->bDevMounted == TRUE)
		{
			return eFS_DevStatus_OK;
		}

		break;

	default:
		return pstDevice->eDevStatus;
	}

	return eFS_DevStatus_NoDevice;
}

HERROR umma_fs_svc_PvrFormatIntPvrHdd (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName)
{
	HUINT32					 	ulDevCnt;
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	UMMA_FS_FormatPartList_t	*pstFormatList = NULL;
	UMMA_FS_DevInfoMap_t		*pstContext;
	HERROR					 	hErr, hResult = ERR_FAIL;

	HxLOG_Print("[%s:%d] called with volume(%s)\n",__FUNCTION__,__LINE__,szVolumeName);

	if (eDevIdx == eFS_Dev_NULL)
	{
		// 내용 중 Internal HDD를 찾는다.
		pstContext = umma_fs_dev_GetContext();

		for (ulDevCnt = 0; ulDevCnt < eFS_Dev_MAX; ulDevCnt++)
		{
			pstDevice = &(pstContext->astDevice[ulDevCnt]);
			HxLOG_Print("[%s:%d] [%02d] eDevType:0x%X bDevAvailable:%d bDevConnected:%d\n",__FUNCTION__,__LINE__,
					ulDevCnt,pstDevice->eDevType,pstDevice->bDevAvailable,pstDevice->bDevConnected);
 			if (pstDevice->bDevAvailable != TRUE || pstDevice->bDevConnected != TRUE)		{ continue; }

			if (pstDevice->eDevType == eFS_DevType_IntHdd)
			{
				eDevIdx = pstDevice->eDevIdx;
				break;
			}
		}
	}

	if (eDevIdx == eFS_Dev_NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	pstFormatList = (UMMA_FS_FormatPartList_t *)UMMA_Malloc (sizeof(UMMA_FS_FormatPartList_t));

	if (pstFormatList == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	/*
		Partition Sizing, Mount Info 등에 정보를 얻어오도록 한다.
	*/
	hErr = umma_fs_svc_DirGetHddPartitionSetting (eDevIdx, szVolumeName, pstFormatList);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	// To make the partition, the values shall be set.
	hErr = umma_fs_svc_PvrMakePartition (pstFormatList);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		HxLOG_Print("[%s] EVT_FS_FORMAT_FAIL (%d) >>>\n", __FUNCTION__, eDevIdx);
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_FORMAT_FAIL,(HINT32)eDevIdx, 0, 0);
		goto ERROR;
	}

	hResult = ERR_OK;

ERROR:
	if (pstFormatList != NULL)			{ UMMA_Free (pstFormatList); }

	return hResult;
}

HERROR umma_fs_svc_PvrFormatExtPvrHdd (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName)
{
	HUINT32					 ulNewDriveNo;
	HUINT8					 szMountPath[UMMA_FS_MOUNT_PATH_LEN];
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	UMMA_FS_FormatPartList_t	*pstFormatList = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	HxLOG_Warning("Enter +++\n");

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		// Mount는 안되어 있더라도, Connect조차 되어 있지 않은 Device를 Format 할 수는 없다.
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	if (pstDevice->bDevMounted == TRUE)
	{
		umma_fs_dev_GetDrivePath (eDevIdx, szMountPath);
	}
	else
	{
		umma_fs_dev_GetNewDriveNo (&ulNewDriveNo);
		umma_fs_dev_MakeMountPathByDriveNo (ulNewDriveNo, szMountPath);
	}

	pstFormatList	= (UMMA_FS_FormatPartList_t *)UMMA_Malloc (sizeof(UMMA_FS_FormatPartList_t));
	if (pstFormatList == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	/*
		Partition Sizing, Mount Info 등에 정보를 얻어오도록 한다.
	*/
	hErr = umma_fs_svc_DirGetHddPartitionSetting (eDevIdx, szVolumeName, pstFormatList);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	// To make the partition, the values shall be set.
	hErr = umma_fs_svc_PvrMakePartition (pstFormatList);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	hResult = ERR_OK;

ERROR:
	if (pstFormatList != NULL)			{ UMMA_Free (pstFormatList); }


	HxLOG_Warning("Quit ---(hResult:%d)\n", hResult);

	return hResult;
}

HERROR umma_fs_svc_PvrFormatIntHddPartition (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_PvrPartitionType_e ePartType, HUINT8 *szVolumeName)
{
	UMMA_FS_FormatPartList_t	*pstFormatList = NULL;
	PAL_FS_FormatList_t		 stPalFormatList;
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	if (eDevIdx == eFS_Dev_NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		// Mount는 안되어 있더라도, Connect조차 되어 있지 않은 Device를 Format 할 수는 없다.
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	pstFormatList	= (UMMA_FS_FormatPartList_t *)UMMA_Malloc (sizeof(UMMA_FS_FormatPartList_t));
	if (pstFormatList == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	hErr = umma_fs_svc_DirGetHddPartitionSetting (eDevIdx, szVolumeName, pstFormatList);
	if (hErr != ERR_OK || pstFormatList->ulPartitionNum == 0)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	// Format List 중에 있는 Partition 중 ePartType 에 해당하는 내용만 남긴다.
	hErr = umma_fs_svc_PvrSelectMwFormatListByPartType (ePartType, pstFormatList);
	if (hErr != ERR_OK || pstFormatList->ulPartitionNum == 0)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	// Format 하기 전에 Unmount 한다.
	hErr = umma_fs_dev_UnmountDevice (eDevIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
	}

	umma_fs_svc_PvrConvertMwFormatListToPalFormatList (pstFormatList, &stPalFormatList);

	hErr = PAL_FS_FormatLists (&stPalFormatList);
	PAL_FS_ClearFormatList (&stPalFormatList);

	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	pstDevice->eDevStatus = eFS_DevStatus_Formatting;

	hResult = ERR_OK;

ERROR:
	if (pstFormatList != NULL)					{ UMMA_Free (pstFormatList); }

	return hResult;
}

HERROR umma_fs_svc_PvrUnmountIntPvrHdd (UMMA_FS_DevIdx_e eDevIdx, HBOOL bSafeRemove)
{
	HUINT32					 ulDevCnt;
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	UMMA_FS_DevInfoMap_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Print("[%s:%d]\n",__FUNCTION__,__LINE__);

	if (eDevIdx == eFS_Dev_NULL)
	{
		// 내용 중 Internal HDD를 찾는다.
		pstContext = umma_fs_dev_GetContext();

		for (ulDevCnt = 0; ulDevCnt < eFS_Dev_MAX; ulDevCnt++)
		{
			pstDevice = &(pstContext->astDevice[ulDevCnt]);
			HxLOG_Print("[%s:%d] [%02d] eDevType:0x%X bDevAvailable:%d bDevConnected:%d\n",__FUNCTION__,__LINE__,
					ulDevCnt,pstDevice->eDevType,pstDevice->bDevAvailable,pstDevice->bDevConnected);
 			if (pstDevice->bDevAvailable != TRUE || pstDevice->bDevConnected != TRUE)		{ continue; }

			if (pstDevice->eDevType == eFS_DevType_IntHdd)
			{
				eDevIdx = pstDevice->eDevIdx;
				break;
			}
		}
	}

	hErr = umma_fs_dev_UnmountDevice (eDevIdx);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	if (hErr == ERR_OK && TRUE == bSafeRemove)
	{
		// safe removed 인경우 disconnect 처리를 한다.
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_DISCONNECTED, (HINT32)eDevIdx, 0, 0);
	}

	return hErr;
}

static void umma_fs_svc_PvrGetMwDstResultAndDevSubStatus(PAL_FS_HDD_DST_RESULT_e eDstResult, UMMA_FS_DstResult_e *peMwDstResult, UMMA_FS_DevSubStatus_e *peDevSubStatus)
{
	UMMA_FS_DstResult_e		eMwDstResult = eFS_DST_COMPLETED_UNKNOWN;
	UMMA_FS_DevSubStatus_e	eDevSubStatus = eFS_DevSubStatus_None;

	switch(eDstResult)
	{
	case ePAL_FS_DST_HDD_NOT_DETECTED:
		eMwDstResult	= eFS_DST_HDD_NOT_DETECTED;
		eDevSubStatus	= eFS_DevSubStatus_DstFail_NoHdd;
		break;
	case ePAL_FS_DST_FAILED_TRY_AGAIN_LATER:
		eMwDstResult	= eFS_DST_FAILED_TRY_AGAIN_LATER;
		eDevSubStatus	= eFS_DevSubStatus_DstFail_TryAgain;
		break;
	case ePAL_FS_DST_IS_ON_GOING:
		eMwDstResult	= eFS_DST_IS_ON_GOING;
		eDevSubStatus	= eFS_DevSubStatus_DstOnGoing;
		break;
	case ePAL_FS_DST_COMPLETED_OK:
		eMwDstResult	= eFS_DST_COMPLETED_OK;
		eDevSubStatus	= eFS_DevSubStatus_DstOk;
		break;
	case ePAL_FS_DST_COMPLETED_FATAL_ERROR:
		eMwDstResult	= eFS_DST_COMPLETED_FATAL_ERROR;
		eDevSubStatus	= eFS_DevSubStatus_DstFail_FatalErr;
		break;
	case ePAL_FS_DST_COMPLETED_TEST_ELEMENT_FAILED:
		eMwDstResult	= eFS_DST_COMPLETED_TEST_ELEMENT_FAILED;
		eDevSubStatus	= eFS_DevSubStatus_DstFail_TestElem;
		break;
	case ePAL_FS_DST_COMPLETED_ELECTRICAL_ELEMENT_FAILED:
		eMwDstResult	= eFS_DST_COMPLETED_ELECTRICAL_ELEMENT_FAILED;
		eDevSubStatus	= eFS_DevSubStatus_DstFail_ElectricalElem;
		break;
	case ePAL_FS_DST_COMPLETED_SERVO_ELEMENT_FAILED:
		eMwDstResult	= eFS_DST_COMPLETED_SERVO_ELEMENT_FAILED;
		eDevSubStatus	= eFS_DevSubStatus_DstFail_ServoElem;
		break;
	case ePAL_FS_DST_COMPLETED_READ_ELEMENT_FAILED:
		eMwDstResult	= eFS_DST_COMPLETED_READ_ELEMENT_FAILED;
		eDevSubStatus	= eFS_DevSubStatus_DstFail_ReadElem;
		break;
	case ePAL_FS_DST_COMPLETED_HANDLING_DAMAGE:
		eMwDstResult	= eFS_DST_COMPLETED_HANDLING_DAMAGE;
		eDevSubStatus	= eFS_DevSubStatus_DstFail_HadlingElem;
		break;
	case ePAL_FS_DST_COMPLETED_UNKNOWN:
		eMwDstResult	= eFS_DST_COMPLETED_UNKNOWN;
		eDevSubStatus	= eFS_DevSubStatus_DstFail_Unknown;
		break;
	default:
		eMwDstResult	= eFS_DST_COMPLETED_UNKNOWN;
		eDevSubStatus	= eFS_DevSubStatus_DstFail_Unknown;
		break;
	}

	if (peMwDstResult)
	{
		*peMwDstResult = eMwDstResult;
	}

	if (peDevSubStatus)
	{
//		printf("[%s:%04d] eDevSubStatus [0x%08x] => \n", __FUNCTION__, __LINE__, *peDevSubStatus);

		*peDevSubStatus = ((*peDevSubStatus & ~eFS_DevSubStatus_DstMask) | (eFS_DevSubStatus_DstMask & eDevSubStatus));

//		printf("[%s:%04d] eDevSubStatus [0x%08x]\n", __FUNCTION__, __LINE__, *peDevSubStatus);
	}
}

static void umma_fs_svc_Pvrdst_event_notifier(PAL_FS_StorDevIdx_t eDevIdx, PAL_FS_HDD_DST_RESULT_e eDstResult, HUINT32 ulProgress)
{
	UMMA_FS_DstResult_e		eMwDstResult = eFS_DST_COMPLETED_UNKNOWN;
	UMMA_FS_DevIdx_e		eMwDevIdx;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR					hErr = ERR_FAIL;
	HINT32					nEvent = eFS_EVT_NONE;

//	printf("[%s:%04d] eDevIdx [%d], eDstResult [%d], ulProgress [%d]\n", __FUNCTION__, __LINE__, eDevIdx, eDstResult, ulProgress);

	eMwDevIdx = umma_fs_dev_ConvertPalDevIdxToMwDevIdx(eDevIdx);
//	printf("[%s:%04d] eMwDevIdx [%d]\n", __FUNCTION__, __LINE__, eMwDevIdx);

	hErr = umma_fs_dev_GetDeviceInfo (eMwDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		// Mount는 안되어 있더라도, Connect조차 되어 있지 않은 Device를 Format 할 수는 없다.
		HxLOG_Error("umma_fs_dev_GetDeviceInfo() Error! Device Is Not Connect or Invalid State! \n");
		return;
	}

//	printf("[%s:%04d] pstDevice->eDevSubStatus [0x%08x]\n", __FUNCTION__, __LINE__, pstDevice->eDevSubStatus);
	umma_fs_svc_PvrGetMwDstResultAndDevSubStatus(eDstResult, &eMwDstResult, &pstDevice->eDevSubStatus);
//	printf("[%s:%04d] pstDevice->eDevSubStatus [0x%08x], eMwDstResult [%d]\n", __FUNCTION__, __LINE__, pstDevice->eDevSubStatus, eMwDstResult);
#if 0

	if(eMwDstResult == eFS_DST_IS_ON_GOING)
	{
		UMMA_FS_SendMsg((HINT32)EVT_FS_NOTIFY_HDD_DST_STATUS, HANDLE_NULL, (HINT32)eMwDevIdx, (HINT32)eFS_DST_IS_ON_GOING, (HINT32)ulProgress);
	}
	else
	{
		UMMA_FS_SendMsg((HINT32)EVT_FS_NOTIFY_HDD_DST_STATUS, HANDLE_NULL, (HINT32)eMwDevIdx, (HINT32)eMwDstResult, (HINT32)TRUE);
	}
#else
	switch(eMwDstResult)
	{
		case eFS_DST_HDD_NOT_DETECTED:
		case eFS_DST_FAILED_TRY_AGAIN_LATER:
			nEvent = eFS_EVT_DST_FAIL;
			break;

		case eFS_DST_IS_ON_GOING:
			nEvent = eFS_EVT_DST_ON_GOING;
			break;

		case eFS_DST_COMPLETED_OK:
			nEvent = eFS_EVT_DST_DONE;
			HxLOG_Print("DST Completed : Result OK >>\n");
			break;

		case eFS_DST_COMPLETED_FATAL_ERROR:
		case eFS_DST_COMPLETED_TEST_ELEMENT_FAILED:
		case eFS_DST_COMPLETED_ELECTRICAL_ELEMENT_FAILED:
		case eFS_DST_COMPLETED_SERVO_ELEMENT_FAILED:
		case eFS_DST_COMPLETED_READ_ELEMENT_FAILED:
		case eFS_DST_COMPLETED_HANDLING_DAMAGE:
		case eFS_DST_COMPLETED_UNKNOWN:
			nEvent = eFS_EVT_DST_FAIL;
			break;
	}

	if (eFS_EVT_DST_ON_GOING == nEvent)
	{
		(void)umma_fs_svc_SendEventToMainloop(nEvent, (HINT32)eMwDevIdx, (HINT32)ulProgress, (HINT32)100);
	}
	else
	{
		(void)umma_fs_svc_SendEventToMainloop(nEvent, (HINT32)eMwDevIdx, (HUINT32)eMwDstResult, 0);
	}
#endif
}


HERROR umma_fs_svc_PvrStartDst(UMMA_FS_DevIdx_e eDevIdx)
{
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	HERROR					 hErr;
	PAL_FS_StorDevIdx_t		 ePalDevIdx;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		// Mount는 안되어 있더라도, Connect조차 되어 있지 않은 Device를 Format 할 수는 없다.
		HxLOG_Error("umma_fs_dev_GetDeviceInfo() Error! Device Is Not Connect or Invalid State! \n");
		return hErr;
	}

	if (!pstDevice->bDevConnected)
	{
		HxLOG_Error("Device Is Not Connected! \n");
	}

	if(eFS_DevStatus_OK != pstDevice->eDevStatus)
	{
		HxLOG_Error("Can not perform DST!! [eDevStatus:0x%08x] \n", pstDevice->eDevStatus);
	}

	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx(eDevIdx);
	pstDevice->eDevSubStatus		= (pstDevice->eDevSubStatus & ~eFS_DevSubStatus_DstMask) | (eFS_DevSubStatus_DstMask & eFS_DevSubStatus_DstNone);
	hErr = PAL_FS_DstStart(ePalDevIdx, umma_fs_svc_Pvrdst_event_notifier);
	if (ERR_OK != hErr)
	{
		pstDevice->eDevSubStatus	= (pstDevice->eDevSubStatus & ~eFS_DevSubStatus_DstMask) | (eFS_DevSubStatus_DstMask & eFS_DevSubStatus_DstFail_Unknown);
	}

	return hErr;
}

HERROR umma_fs_svc_PvrMakePartition (UMMA_FS_FormatPartList_t *pstFormatList)
{
	HBOOL					 	bUpdated;
	HUINT32					 	ulIndex;
	UMMA_FS_DevIdx_e			ePvrDevIdx = eFS_Dev_NULL;
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	UMMA_FS_FormatPartInfo_t	*pstFormatInfo;
	UMMA_FS_PartitionInfo_t		*pstPartInfo = NULL;
	PAL_FS_StorDevIdx_t		 	ePalDevIdx;
	PAL_FS_FormatList_t		 	stPalFormatList;
	HERROR					 	hErr;
	HBOOL						bMounted = FALSE;

	HUINT32						ulMaxCnt = 10, ulCnt;

	HxLOG_Warning("Enter +++\n");

	if (pstFormatList == NULL || pstFormatList->ulPartitionNum == 0)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	hErr = umma_fs_dev_GetDeviceInfo (pstFormatList->eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		// Mount는 안되어 있더라도, Connect조차 되어 있지 않은 Device를 Format 할 수는 없다.
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	bMounted = pstDevice->bDevMounted;

	bUpdated = FALSE;
	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (pstFormatList->eDevIdx);

	// 0. Unmount 이전에 현재 이 device가 PVR로 쓰이고 있는지 체크한다.
	pstDevice->bIsExPvrModule = FALSE; // default
	for(ulIndex=0 ; ulIndex < MAX_CONNECTABLE_PVR_STORAGE_NUM; ulIndex++)
	{
		hErr = umma_fs_svc_PvrFindPvrDevIdPartIdx (ulIndex, &ePvrDevIdx, NULL);
		if (hErr == ERR_OK && ePvrDevIdx != eFS_Dev_NULL && pstFormatList->eDevIdx == ePvrDevIdx)
		{
			pstDevice->bIsExPvrModule = TRUE;
			break;
		}
	}

	for (ulCnt = 0 ; ulCnt < ulMaxCnt ; ulCnt++)
	{
		// 1. 해당 Device에 해당하는 모든 Device를 다 Unmount 한다.
		hErr = umma_fs_dev_UnmountDevice (pstFormatList->eDevIdx);
		if (hErr == ERR_OK)
		{
			break;
		}
		else
		{
			HxLOG_Warning("Umount Fail (devIdx = %d) -------> Retry : %d \n", pstFormatList->eDevIdx, ulCnt);
			HLIB_STD_TaskSleep(1000);	// 1초간 쉰다.
		}
	}

	if (hErr != ERR_OK && bMounted == TRUE)
	{
		// mount가 되었었던 device인데 unmount가 fail되면 format 불가능..
		HxLOG_Critical("\nFinally umount Fail (devIdx = %d)!!!\n\n", pstFormatList->eDevIdx);
		return ERR_FAIL;
	}

	// 2. Make the partition:
	hErr = PAL_FS_OpenDevMbrBuf (ePalDevIdx);
	if (hErr == ERR_OK)
	{
		// 2-1. 기존 Partition 정보를 MBR에서 모두 지운다.
		PAL_FS_DeleteAllPartitionInDevMbrBuf (ePalDevIdx);

		for (ulIndex = 0; ulIndex < pstFormatList->ulPartitionNum; ulIndex++)
		{
			UMMA_FS_FsType_e		eMwFsType = eFS_FileSystem_OTHER;
			PAL_FS_FsType_t			ePalFsType= PAL_FS_OTHER_FILESYSTEM;

			pstFormatInfo = &(pstFormatList->astPartArray[ulIndex]);
			eMwFsType = pstFormatInfo->ePartFsType;
			ePalFsType= umma_fs_dev_ConvertMwFsTypeToPalFsType(eMwFsType);

			HxLOG_Print("[%s (%04d)] pstFormatInfo->ulStartUnit [%d], pstFormatInfo->ulEndUnit [%d] mw fsType [%d] -> pal fs type[%d]>>>\n",
				__FUNCTION__, __LINE__, pstFormatInfo->ulStartUnit, pstFormatInfo->ulEndUnit, eMwFsType, ePalFsType);

	#if !defined(CONFIG_DROP_ROOT_PRIVILEGES)
			{
				HUINT64 ullPartSizeByte;

				if (pstFormatList->ullUnitSize != 0L)
				{
					ullPartSizeByte = (HUINT64)((pstFormatInfo->ulEndUnit - pstFormatInfo->ulStartUnit) * pstFormatList->ullUnitSize);
					hErr = PAL_FS_SetPartitionInDevMbrBuf(ePalDevIdx, ulIndex, (ullPartSizeByte/1024), ePalFsType);
					if (hErr == ERR_OK)
					{
						bUpdated = TRUE;
					}
					else
					{
						HxLOG_Error(" \n");
					}
					HxLOG_Print("[%s (%04d)] pstFormatInfo->ulStartUnit [%d], pstFormatInfo->ulEndUnit [%d] mw fsType [%d] -> pal fs type[%d] ullPartSizeKb=%lldKb>>>\n",
					__FUNCTION__, __LINE__, pstFormatInfo->ulStartUnit, pstFormatInfo->ulEndUnit, eMwFsType, ePalFsType, ullPartSizeByte/1024);
				}
			}
	#else
			hErr = PAL_FS_UpdatePartitionInDevMbrBuf (ePalDevIdx, ulIndex, pstFormatInfo->ulStartUnit, pstFormatInfo->ulEndUnit, ePalFsType);
			if (hErr == ERR_OK)
			{
				bUpdated = TRUE;
			}
			else
			{
				HxLOG_Error(" \n");
			}
	#endif
		}

		if (bUpdated == TRUE)
		{
			hErr = PAL_FS_SyncDevMbrBuf (ePalDevIdx);
			if (hErr != ERR_OK)
			{
				bUpdated = FALSE;
			}
		}

		hErr = PAL_FS_CloseDevMbrBuf (ePalDevIdx);
		if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
	}

	// 3. Format the new partitions.
	if (bUpdated == TRUE)
	{
		// MBR이 바뀌었으므로 다시 읽어서 확인한다.
		umma_fs_dev_ReinitPartitionInfo (pstFormatList->eDevIdx, pstFormatList, TRUE);

		umma_fs_svc_PvrConvertMwFormatListToPalFormatList (pstFormatList, &stPalFormatList);

		HxLOG_Warning("Start format -> call PAL_FS_FormatLists()\n");
		hErr = PAL_FS_FormatLists (&stPalFormatList);
		PAL_FS_ClearFormatList (&stPalFormatList);

		pstDevice->eDevStatus = eFS_DevStatus_Formatting;

		if (hErr != ERR_OK)
		{
			HxLOG_Critical("\nCritical Error!!!\n\n");
			return ERR_FAIL;
		}
	}
	else
	{
		// Unmount 한 것을 모두 재 mount 해야 한다.
		for (ulIndex = 0; ulIndex < pstFormatList->ulPartitionNum; ulIndex++)
		{
			pstFormatInfo = &(pstFormatList->astPartArray[ulIndex]);
			if (pstFormatInfo == NULL)
			{
				HxLOG_Error("[umma_fs_svc_PvrMakePartition] Format info is null...\n");
				continue;
			}

			hErr = umma_fs_dev_MountPartition (pstFormatList->eDevIdx, ulIndex, pstFormatInfo->bSecured, pstFormatInfo->szMountPath, &pstPartInfo); // MOON_encr
			if (hErr == ERR_OK && pstPartInfo != NULL)
			{
			}
		}
	}

	HxLOG_Warning("Quit ---\n");
	return ERR_OK;
}

HERROR umma_fs_svc_PvrMountDevice(UMMA_FS_DevIdx_e eDevIdx)
{
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR					hErr, hMountRet;
	HUINT32			 		ulPartIdx;
	UMMA_FS_PvrType_e		ePvrType;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("[umma_fs_svc_PvrMountDevice] umma_fs_dev_GetDeviceInfo err(%08x)\n", hErr);
		return ERR_FAIL;
	}

	switch (pstDevice->eDevType)
	{
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	case eFS_DevType_IntHdd:
	case eFS_DevType_Detachable:
	case eFS_DevType_eSataHdd:
		hMountRet = umma_fs_hdd_MountAndCheckIntHdd (eDevIdx,TRUE);
		break;
#endif

#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
	case eFS_DevType_UsbHdd:
	case eFS_DevType_UsbMem:
		hMountRet = umma_fs_usb_MountDevice (eDevIdx, TRUE);
		break;
#endif


#if defined(CONFIG_PROD_DEVICE_STORAGE_MMC)
	case eFS_DevType_MMC:
		hMountRet = umma_fs_mmc_MountDevice ();
		break;
#endif

	default:
		hMountRet = ERR_FAIL;
		break;
	}

	// Format 후 Mount한 다음에는 bIsExPvrModule Flag를 없앤다.
	pstDevice->bIsExPvrModule = FALSE;
	pstDevice->eDevSubStatus	= eFS_DevSubStatus_None;

	if (hMountRet == ERR_OK)
	{
		pstDevice->eDevStatus = eFS_DevStatus_OK;
		for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
		{
			if (pstDevice->pstPartArray[ulPartIdx].bPvrFlag == TRUE)
			{
				umma_fs_svc_DirMakeDefaultDirectory(eDevIdx, ulPartIdx);

				if (pstDevice->eDevType == eFS_DevType_Detachable)
				{
					umma_fs_hdd_SetNewDetachableHDD(FALSE);
					hErr = umma_fs_svc_WritePvrRegisterInfoToHDD(eDevIdx, ulPartIdx);
					if(hErr != ERR_OK)
					{
						HxLOG_Critical("Error, Fail to regist PVR Info to HDD.\n");
					}
				}
			}
		}

		if (pstDevice->eDevType == eFS_DevType_eSataHdd)
		{
			// 해당 모델이 지원하는 이 Device의 PVR type을 가져온다.
			ePvrType = umma_fs_config_GetPvrType(eFS_DevType_eSataHdd);

			// record (PVR)
			if ((ePvrType & eFS_PVR_Main) || (ePvrType & eFS_PVR_Sub) || (ePvrType & eFS_PVR_Tsr))	// Main or Sub
			{
				HBOOL	bOk;
				bOk = umma_fs_svc_PvrIsPvrStorageRegistrable();
				if(bOk == TRUE)
				{
					hErr = umma_fs_svc_PvrCheckDeviceIsExtPvrPaired (eDevIdx, TRUE, UMMA_FS_EXT_PVR_PART_IDX);
					if (hErr != ERR_OK)
					{
						HxLOG_Debug("umma_fs_svc_PvrCheckDeviceIsExtPvrPaired failed. eDevIdx(%d) \n",eDevIdx);
					}
					umma_fs_svc_DirMakeDefaultDirectory(eDevIdx, UMMA_FS_EXT_PVR_PART_IDX);
				}
			}

			// playback
			if (ePvrType & eFS_PVR_RecStorage)
			{
				HUINT32 	ulPlaybackIdx = DxSTORAGE_INVALID_PVR_INDEX;
				HBOOL bPlaybackStorageRegistrable = umma_fs_svc_PvrIsPlaybackStorageRegistrable();
				if (bPlaybackStorageRegistrable)
				{
					(void)umma_fs_svc_PvrMakePlaybackInfo(eDevIdx,UMMA_FS_EXT_PVR_PART_IDX);
					hErr = umma_fs_svc_PvrGetPlaybackIdxByDevIdx(&ulPlaybackIdx, eDevIdx);
					if (ERR_OK == hErr)
					{
						(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_PLAYBACK_ENABLED, (HINT32)eDevIdx, (HINT32)0, (HINT32)ulPlaybackIdx);
					}
				}
			}
		}
	}
	else
	{
		pstDevice->eDevStatus = eFS_DevStatus_Malfunction;

		return ERR_FAIL;
	}

	return (hMountRet == ERR_OK) ? ERR_OK : ERR_FAIL;
}

HERROR umma_fs_svc_PvrFindPvrDevIdPartIdx (HUINT32 ulPvrIdx, UMMA_FS_DevIdx_e *peDevIdx, HUINT32 *pulPartIdx)
{
	HUINT32				 ulIndex, ulPartIdx;
	UMMA_FS_DevInfoMap_t		*pstContext;
	UMMA_FS_DevContext_t		*pstSrcDevInfo;

	pstContext = umma_fs_dev_GetContext();

	// Check if the device is already mounted
	for (ulIndex = 0; ulIndex < eFS_Dev_MAX; ulIndex++)
	{
		pstSrcDevInfo = &(pstContext->astDevice[ulIndex]);
		if (pstSrcDevInfo->bDevAvailable != TRUE)			{ continue; }

		for (ulPartIdx = 0; ulPartIdx < pstSrcDevInfo->ulNumPartition; ulPartIdx++)
		{
			if (pstSrcDevInfo->pstPartArray[ulPartIdx].bPvrFlag == TRUE && pstSrcDevInfo->pstPartArray[ulPartIdx].ulPvrIdx == ulPvrIdx)
			{
					if (peDevIdx != NULL)				{ *peDevIdx = pstSrcDevInfo->eDevIdx; }
					if (pulPartIdx != NULL) 			{ *pulPartIdx = ulPartIdx; }

					return ERR_OK;
			}
		}
	}

	HxLOG_Error("[umma_fs_svc_PvrFindPvrDevIdPartIdx] No PVR device (PvrIdx[%d])\n", ulPvrIdx);
	return ERR_FAIL;
}


/*
	주의 : 실제로 PVR용 HDD로 사용되는 HDD가 아니라 PVR용 HDD로 쓸 요건이 되는 device를 얻어내는 함수이다.
	물론 현재 PVR HDD로 쓰고 있는 device도 해당되나, 현재 PVR HDD가 아니어도 이 함수에서 결과로 알려준다.
*/

HERROR umma_fs_svc_GetReservedSizeKb(UMMA_FS_DevType_e	 eDevType, HUINT64 ullTotalSizeKb, HUINT64 *pullReservedSizeKb)
{
	HUINT64 ullReservedSizeKb;

	// Check USB Flash or small size HDD (default : under 64GB)
	if (eDevType == eFS_DevType_UsbMem || ullTotalSizeKb < (DxSVC_FS_PVR_MIN_HDD_SIZE_KB))
	{
		// Modify reserved size
		ullReservedSizeKb = ullTotalSizeKb * DxSVC_FS_PVR_USB_FLASH_RESERVED_SIZE_RATIO;

		// Check threshold
		if (ullReservedSizeKb > DxSVC_FS_PVR_USB_FLASH_MAX_RESERVED_SIZE_KB)
		{
			ullReservedSizeKb = DxSVC_FS_PVR_USB_FLASH_MAX_RESERVED_SIZE_KB;
		}
	}
	else	// The others
	{
		ullReservedSizeKb = DxSVC_FS_PVR_RESERVED_SIZE_KB;
	}

	if (pullReservedSizeKb)
	{
		*pullReservedSizeKb = ullReservedSizeKb;
	}

	HxLOG_Debug("ullTotalKbSize: %lld, ullReservedSizeKb : %lld\n",ullTotalSizeKb, ullReservedSizeKb);

	return ERR_OK;
}

HERROR umma_fs_svc_PvrFindPvrPossiblePartitionInDevice (UMMA_FS_DevIdx_e eDevIdx, HUINT32 *pulPartIdx)
{
	HBOOL					 bWritable;
	HUINT32					 ulPartIdx;
	HUINT64					 ullTotalKbSize, ullUsedKbSize, ullReservedKbSize;
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	UMMA_FS_PartitionInfo_t	*pstPartInfo;
	HERROR					 hErr;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL) 			{ return ERR_FAIL; }

	for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
	{
		pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);

		if (pstPartInfo->ePartFsType == eFS_FileSystem_EXT2 ||
			pstPartInfo->ePartFsType == eFS_FileSystem_EXT3 ||
			pstPartInfo->ePartFsType == eFS_FileSystem_EXT4 ||
			pstPartInfo->ePartFsType == eFS_FileSystem_NTFS ||
			pstPartInfo->ePartFsType == eFS_FileSystem_XFS)
		{
			hErr = HLIB_STORAGE_GetPartitionSize(pstPartInfo->szMountPath, &ullTotalKbSize, &ullUsedKbSize, &bWritable);
			if (hErr == ERR_OK)
			{
				if (ullTotalKbSize < DxSVC_FS_PVR_MIN_SIZE_KB)
				{
					// DxSVC_FS_PVR_MIN_SIZE_KB 보다 작은 Storage는 PVR용으로 사용할 수 없다.
					continue;
				}

				// Get reserved size (TSR, etc....)
				umma_fs_svc_GetReservedSizeKb(pstDevice->eDevType, ullTotalKbSize, &ullReservedKbSize);

				if (bWritable == TRUE)
				{
					if (pstDevice->eDevType == eFS_DevType_IntHdd)
					{
						if (pstPartInfo->bPvrFlag == TRUE)	// partiting no. 2
						{
							// default PVR인 경우만 TSR 사이즈를 포함해서 계산
							if (umma_fs_svc_PvrCheckRemainSize (pstPartInfo->szMountPath, ullTotalKbSize, ullUsedKbSize, ullReservedKbSize, pstPartInfo->bDefaultPvr) == TRUE)
							{
								HxLOG_Print("[umma_fs_svc_PvrIsDevicePvrPossible] Dev (%d:%d) avialble for PVR\n", eDevIdx, ulPartIdx);
								if (pulPartIdx != NULL) 				{ *pulPartIdx = ulPartIdx; }

								return ERR_OK;
							}
						}
					}
					else
					{
						// 다른 device는 이 function이 불리는 시점에 bPvrFlag가 FALSE 이다.
						if (umma_fs_svc_PvrCheckRemainSize (pstPartInfo->szMountPath, ullTotalKbSize, ullUsedKbSize, ullReservedKbSize, TRUE) == TRUE)
						{
							HxLOG_Print("[umma_fs_svc_PvrIsDevicePvrPossible] Dev (%d:%d) avialble for PVR\n", eDevIdx, ulPartIdx);
							if (pulPartIdx != NULL) 				{ *pulPartIdx = ulPartIdx; }

							return ERR_OK;
						}
					}
				}
				else
				{
					HxLOG_Print("[umma_fs_svc_PvrIsDevicePvrPossible] Dev (%d:%d) isn't the writable partition\n", eDevIdx, ulPartIdx);
				}
			}
		}
		else
		{	// invalid file systems
			HxLOG_Error("invalid file system (%d) \n", pstPartInfo->ePartFsType);
		}
	}

	return ERR_FAIL;
}

HERROR umma_fs_svc_PvrGetPvrPossibleDeviceIndexArray (HUINT32 *pulDevNum, UMMA_FS_DevIdx_e **ppeDevIdxArray)
{
	HUINT32					 ulIndex, ulPvrDevNum = 0;
	UMMA_FS_DevIdx_e			*pePvrDevIdxArray = NULL;
	UMMA_FS_DevContext_t			*pstDevice;
	UMMA_FS_DevInfoMap_t			*pstContext;
	HERROR					 hResult = ERR_FAIL;

	if (pulDevNum != NULL)				{ *pulDevNum = 0; }
	if (ppeDevIdxArray != NULL)			{ *ppeDevIdxArray = NULL; }

	pstContext = umma_fs_dev_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("[umma_fs_svc_PvrFindPvrPossibleDevice] umma_fs_dev_GetContext NULL\n");
		goto ERROR;
	}

	ulPvrDevNum = 0;
	for (ulIndex = 0; ulIndex < eFS_Dev_MAX; ulIndex++)
	{
		pstDevice = &(pstContext->astDevice[ulIndex]);
		if (pstDevice->bDevAvailable == FALSE)				{ continue; }

		if (umma_fs_svc_PvrFindPvrPossiblePartitionInDevice (pstDevice->eDevIdx, NULL) == ERR_OK)
		{
			ulPvrDevNum ++;
		}
	}

	if (ulPvrDevNum == 0)
	{
		HxLOG_Error("[umma_fs_svc_PvrFindPvrPossibleDevice] No PVR possible device\n");
		goto ERROR;
	}

	pePvrDevIdxArray = (UMMA_FS_DevIdx_e *)UMMA_Malloc (sizeof(UMMA_FS_DevIdx_e) * ulPvrDevNum);
	if (pePvrDevIdxArray == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	ulPvrDevNum = 0;
	for (ulIndex = 0; ulIndex < eFS_Dev_MAX; ulIndex++)
	{
		pstDevice = &(pstContext->astDevice[ulIndex]);
		if (pstDevice->bDevAvailable == FALSE)				{ continue; }

		if (umma_fs_svc_PvrFindPvrPossiblePartitionInDevice (pstDevice->eDevIdx, NULL) == ERR_OK)
		{
			pePvrDevIdxArray[ulPvrDevNum] = pstDevice->eDevIdx;
			ulPvrDevNum ++;
		}
	}

	*pulDevNum = ulPvrDevNum;
	*ppeDevIdxArray = pePvrDevIdxArray;
	pePvrDevIdxArray = NULL;

	hResult = ERR_OK;

ERROR:
	if (pePvrDevIdxArray != NULL)		{ UMMA_Free (pePvrDevIdxArray); }

	return hResult;
}


#define ________PVR_DEFAULT_DIRECTORY________

HERROR umma_fs_svc_PvrGetPvrPath (HUINT32 ulPvrIdx, UMMA_FS_UsageType_e eUsage, HCHAR *szPath)
{
	HUINT32				 ulPartIdx;
	UMMA_FS_DevIdx_e		 eDevIdx;
	UMMA_FS_DevType_e		 eDevType;
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	HERROR				 hErr;

	hErr = umma_fs_svc_PvrFindPvrDevIdPartIdx (ulPvrIdx, &eDevIdx, &ulPartIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	// Sub Path가 필요 없는 경우는 Mount된 위치만을 보내줌.
	if (eUsage == eFS_UsageType_Root || eUsage == eFS_UsageType_None)
	{
		VK_strncpy (szPath, pstDevice->pstPartArray[ulPartIdx].szMountPath, UMMA_FS_PVR_DIR_PATH_LEN);
		umma_fs_dev_AppendSeparatorToPath (szPath);
		return ERR_OK;
	}

	eDevType = pstDevice->eDevType;

	hErr = umma_fs_svc_DirGetPathFromMountedRootPath (pstDevice->pstPartArray[ulPartIdx].szMountPath, eDevType, eUsage, szPath, UMMA_FS_PVR_DIR_PATH_LEN-1);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}


#define _____EXT_PVR_REGISTRATION_____

HERROR umma_fs_svc_PvrGetPvrIdxByDevUuid(HUINT32 *pulPvrIdx, HUINT8 *szUuid)
{
	HERROR			 hErr;
	UMMA_FS_DevIdx_e 	eDevIdx;
	HUINT32			ulIdx=0;
	HUINT8			szPalUuid[UMMA_FS_DEV_UUID_BUFSIZE];


	// PvrIdx를 루프로 devIdx를 얻어서 HDD의 UUID를 받아서 szUuid와 비교한다.
	for(ulIdx = 0 ; ulIdx < MAX_CONNECTABLE_PVR_STORAGE_NUM ; ulIdx++)
	{
		VK_memset (szPalUuid, 0, UMMA_FS_DEV_UUID_BUFSIZE);

		hErr = umma_fs_svc_PvrFindPvrDevIdPartIdx (ulIdx, &eDevIdx, NULL);
		if (hErr != ERR_OK)
		{
			continue;
		}

		hErr = umma_fs_dev_GetDeviceUuid(eDevIdx, szPalUuid);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Get UUID Error!!\n");
			continue;
		}

		if(VK_strncmp(szPalUuid, szUuid, UMMA_FS_DEV_UUID_BUFSIZE) == 0)
		{
			if(pulPvrIdx != NULL)		*pulPvrIdx = ulIdx;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


HERROR umma_fs_svc_PvrGetPvrIdxByDevIdx(HUINT32 *pulPvrIdx, UMMA_FS_DevIdx_e eDevIdx)
{
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR			 hErr;
	HUINT32		ulPartIdx;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Print("[%s][%d] umma_fs_dev_GetDeviceInfo err(0x%08x)\n",__FUNCTION__,__LINE__, hErr);
		return ERR_FAIL;
	}

	for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
	{
		if (pstDevice->pstPartArray[ulPartIdx].bPvrFlag == TRUE)
		{
			*pulPvrIdx = pstDevice->pstPartArray[ulPartIdx].ulPvrIdx;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR umma_fs_svc_PvrGetPlaybackIdxByDevIdx(HUINT32 *pulPlaybackIdx, UMMA_FS_DevIdx_e eDevIdx)
{
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR			 hErr;
	HUINT32		ulPartIdx;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Print("[%s][%d] umma_fs_dev_GetDeviceInfo err(0x%08x)\n",__FUNCTION__,__LINE__, hErr);
		return ERR_FAIL;
	}

	for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
	{
		if (pstDevice->pstPartArray[ulPartIdx].ulPlaybackIdx != DxSTORAGE_INVALID_PVR_INDEX)
		{
			*pulPlaybackIdx = pstDevice->pstPartArray[ulPartIdx].ulPlaybackIdx;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HUINT32 umma_fs_svc_PvrGetDefaultPvrIdx(void)
{
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HUINT32 	ulPartIdx, ulDevCnt, ulPvrIdx = 0xffff;
	UMMA_FS_DevInfoMap_t		*pstContext;

	pstDevice = NULL;
	pstContext = umma_fs_dev_GetContext();

	for (ulDevCnt = 0; ulDevCnt < eFS_Dev_MAX; ulDevCnt++)
	{
		pstDevice = &(pstContext->astDevice[ulDevCnt]);

		if (pstDevice->bDevAvailable != TRUE || pstDevice->bDevConnected != TRUE)		{ continue; }
		{
			for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
			{
				if(pstDevice->pstPartArray[ulPartIdx].bMounted != TRUE)		{ continue; }

				if (pstDevice->pstPartArray[ulPartIdx].bPvrFlag == TRUE &&
					pstDevice->pstPartArray[ulPartIdx].bDefaultPvr == TRUE)
				{
					ulPvrIdx = pstDevice->pstPartArray[ulPartIdx].ulPvrIdx;
					return ulPvrIdx;
				}
			}
		}
	}

	return ulPvrIdx;
}
#if 0

#if defined(CONFIG_PROD_DEVICE_2ND_INTERNAL_HDD)
// detachable HDD를 지원할 경우를 위한 함수.
// default로 internal을 선택. internal full되면 detachable로 변경
//detachable을 쓰다가 internal이 80% 이하로 공간 생기거나 detachable full되면 internal로 다시 변경
// return 값이 0xfffffff면, hdd full 상태..
HUINT32 umma_fs_svc_PvrGetTargetPvrIdx(void)
{
	UMMA_FS_DevContext_t	*pstDevice = NULL, *pstDeviceSelected = NULL,  *pstDeviceOther = NULL;
	HUINT32 	ulPartIdx, ulDevCnt, ulPvrIdxOther = 0xffffffff;
	UMMA_FS_DevInfoMap_t		*pstContext;

	pstDevice = NULL;
	pstContext = umma_fs_dev_GetContext();

	for (ulDevCnt = 0; ulDevCnt < eFS_Dev_MAX; ulDevCnt++)
	{
		pstDevice = &(pstContext->astDevice[ulDevCnt]);

		if (pstDevice->bDevAvailable != TRUE || pstDevice->bDevConnected != TRUE)		{ continue; }
		{
			for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
			{
				if(pstDevice->pstPartArray[ulPartIdx].bMounted != TRUE)		{ continue; }

				if (pstDevice->pstPartArray[ulPartIdx].bPvrFlag == TRUE &&
					pstDevice->eDevType == eFS_DevType_IntHdd)
				{
					if (pstDevice->pstPartArray[ulPartIdx].ulPvrIdx == pstContext->ulTargetPvrIdx)
					{
						pstDeviceSelected = pstDevice;
					}
					else
					{
						pstDeviceOther = pstDevice; // detachable HDD 한개만 고려
						ulPvrIdxOther = pstDevice->pstPartArray[ulPartIdx].ulPvrIdx;
					}
				}
			}
		}
	}

	if (pstDeviceSelected != NULL)
	{
		if (pstDevice->eDevType == eFS_DevType_IntHdd)
		{
			if (umma_fs_svc_PvrFindPvrPossiblePartitionInDevice(pstDeviceSelected->eDevIdx, NULL) != ERR_OK)
			{
				// tsr buffer를 고려했을 때 여유 공간이 없는 상태
				// hdd full 상태이므로 다른 게 있으면 지정 (둘다 full이면 그냥 internal 유지)
				if (pstDeviceOther != NULL)
				{
					if (umma_fs_svc_PvrFindPvrPossiblePartitionInDevice(pstDeviceOther->eDevIdx, NULL) == ERR_OK)
					{
						pstContext->ulTargetPvrIdx = ulPvrIdxOther;
					}
					else
					{
						pstContext->ulTargetPvrIdx = 0xffffffff;	// hdd full 처리 위해서
					}
				}
			}
		}
		else
		{
			// 여기서 pstDeviceOther는 internal HDD일 것..

			if (umma_fs_svc_PvrFindPvrPossiblePartitionInDevice(pstDeviceSelected->eDevIdx, NULL) != ERR_OK)
			{
				// tsr buffer를 고려했을 때 여유 공간이 없는 상태
				// hdd full 상태이므로 다른 게 있으면 지정
				if (pstDeviceOther != NULL)
				{
					if (umma_fs_svc_PvrFindPvrPossiblePartitionInDevice(pstDeviceOther->eDevIdx, NULL) == ERR_OK)
					{
						pstContext->ulTargetPvrIdx = ulPvrIdxOther;
					}
					else
					{
						pstContext->ulTargetPvrIdx = 0xffffffff;	// hdd full 처리 위해서
					}
				}
			}
			else
			{
				// full은 아니지만 internal이 80% 이하 사용 상태인지 확인
				if (pstDeviceOther != NULL)
				{
					if (umma_fs_svc_PvrCheckHddAvailable(pstDeviceOther) == TRUE)
					{
						// 여유 공간이 있다면 변경
						pstContext->ulTargetPvrIdx = ulPvrIdxOther;
					}
				}
			}
		}
	}
	else
	{
		// 선택된 pvr이 없다. 다른 pvr이 있으면 지정한다.
		if (pstDeviceOther != NULL)
		{
			if (umma_fs_svc_PvrFindPvrPossiblePartitionInDevice(pstDeviceOther->eDevIdx, NULL) == ERR_OK)
			{
				pstContext->ulTargetPvrIdx = ulPvrIdxOther;
			}
			else
			{
				pstContext->ulTargetPvrIdx = 0xffffffff;	// hdd full 처리 위해서
			}
		}
		else
		{
			pstContext->ulTargetPvrIdx = 0xffffffff;	// no hdd 처리 위해서
		}

	}

	return pstContext->ulTargetPvrIdx;
}

HERROR umma_fs_svc_PvrSetTargetPvrIdx(HUINT32 ulPvrIdx)
{
	UMMA_FS_DevInfoMap_t		*pstContext;

	pstContext = umma_fs_dev_GetContext();

	pstContext->ulTargetPvrIdx = ulPvrIdx;
	return ERR_OK;
}

#endif
#endif

// PVR 용 Storage로 등록가능한지 확인 하는 함수
HBOOL umma_fs_svc_PvrIsPvrStorageRegistrable(void)
{
	HUINT32					 ulIdx;
	UMMA_FS_DevInfoMap_t			*pstContext;

	pstContext = umma_fs_dev_GetContext();

	for(ulIdx = 0 ; ulIdx < MAX_CONNECTABLE_PVR_STORAGE_NUM ; ulIdx++)
	{
		if(pstContext->abConnectedPvrSlot[ulIdx] == FALSE)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// Playback 용 Storage로 등록가능한지 확인 하는 함수
HBOOL umma_fs_svc_PvrIsPlaybackStorageRegistrable (void)
{
	HUINT32					 ulIdx;
	UMMA_FS_DevInfoMap_t			*pstContext;

	pstContext = umma_fs_dev_GetContext();

	for(ulIdx = 0 ; ulIdx < MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM ; ulIdx++)
	{
		if(pstContext->abConnectedPlaybackSlot[ulIdx] == FALSE)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// PvrIdx 할당 함수 : 가장 먼저 FALSE인 놈을 assign
HERROR	umma_fs_svc_PvrAssignPvrIdx(HUINT32 *pulPvrIdx)
{
	HUINT32					 ulIdx;
	UMMA_FS_DevInfoMap_t			*pstContext;

	pstContext = umma_fs_dev_GetContext();

	for(ulIdx = 0 ; ulIdx < MAX_CONNECTABLE_PVR_STORAGE_NUM ; ulIdx++)
	{
		if(pstContext->abConnectedPvrSlot[ulIdx] == FALSE)	// 비어있음
		{
			*pulPvrIdx = ulIdx;
			pstContext->abConnectedPvrSlot[ulIdx] = TRUE;
			HxLOG_Error("Assigned PVR Index [%d]: %d\n", ulIdx, pstContext->abConnectedPvrSlot[ulIdx]);
			return ERR_OK;
		}
	}
	HxLOG_Error("Fail to Assign PVR Index.\n");

	return ERR_FAIL;
}

HERROR	umma_fs_svc_PvrAssignPlaybackIdx (HUINT32 *pulPlaybackIdx)
{
	HUINT32					 ulIdx;
	UMMA_FS_DevInfoMap_t			*pstContext;

	pstContext = umma_fs_dev_GetContext();

	for(ulIdx = 0 ; ulIdx < MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM ; ulIdx++)
	{
		if(pstContext->abConnectedPlaybackSlot[ulIdx] == FALSE)	// 비어있음
		{
			*pulPlaybackIdx = ulIdx;
			pstContext->abConnectedPlaybackSlot[ulIdx] = TRUE;
			return ERR_OK;
		}
	}

	return ERR_FAIL;

}

// PvrIdx 해제 함수
HERROR	umma_fs_svc_PvrReleasePvrIdx(HUINT32 ulPvrIdx)
{
	UMMA_FS_DevInfoMap_t			*pstContext;

	pstContext = umma_fs_dev_GetContext();

	if(pstContext->abConnectedPvrSlot[ulPvrIdx] == TRUE)
	{
		pstContext->abConnectedPvrSlot[ulPvrIdx] = FALSE;
		HxLOG_Error("Released PVR Index [%d]: %d\n", ulPvrIdx, pstContext->abConnectedPvrSlot[ulPvrIdx]);
		return ERR_OK;
	}
	HxLOG_Error("Fail to release PVR Index[%d].\n", ulPvrIdx);
	return ERR_FAIL;
}

HERROR	umma_fs_svc_PvrReleasePlaybackIdx (HUINT32 ulPlaybackIdx)
{
	UMMA_FS_DevInfoMap_t			*pstContext;

	if (ulPlaybackIdx >= MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM)
	{
		HxLOG_Error("ulPlaybackIdx(%d) > MAX(%d) \n",ulPlaybackIdx,MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM);
		return ERR_FAIL;
	}

	pstContext = umma_fs_dev_GetContext();
	if(pstContext->abConnectedPlaybackSlot[ulPlaybackIdx] == TRUE)
	{
		pstContext->abConnectedPlaybackSlot[ulPlaybackIdx] = FALSE;
		return ERR_OK;
	}

	return ERR_FAIL;
}


HBOOL	umma_fs_svc_PvrIsDevicePvrUsed (UMMA_FS_DevIdx_e eDevIdx)
{
	HERROR			 hErr;
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	HUINT32			ulPartIdx;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr == ERR_OK && pstDevice != NULL)
	{
		for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
		{
			if (pstDevice->pstPartArray[ulPartIdx].bPvrFlag == TRUE)
			{
				return TRUE;
			}
		}
	}

	return FALSE;

}
HBOOL	umma_fs_svc_PvrIsPVRAvailable (void)
{
	HERROR					 hErr;
	HUINT32					 ulCnt=0;

	// 등록/연결된 PVR  Storage의 개수가 0보다 많으면 ERR_OK를 리턴한다.
	hErr = umma_fs_svc_PvrGetConnectedPvrStorageCount(&ulCnt);
	if(hErr == ERR_OK)
	{
		return TRUE;
	}
	return FALSE;
}

// PVR용으로 등록되어있고, 현재 연결된 (즉 지금 사용가능한) PVR Storage의 개수를 리턴한다.
HERROR	umma_fs_svc_PvrGetConnectedPvrStorageCount (HUINT32 *pulPvrCnt)
{
	UMMA_FS_DevInfoMap_t			*pstContext;
	HUINT32					ulIdx, ulCnt=0;

	if(pulPvrCnt	== NULL)
	{
		return ERR_FAIL;
	}

	pstContext = umma_fs_dev_GetContext();
	for(ulIdx = 0; ulIdx < MAX_CONNECTABLE_PVR_STORAGE_NUM ; ulIdx++)
	{
		if(pstContext->abConnectedPvrSlot[ulIdx] == TRUE)
		{
			ulCnt++;
		}
	}

	if(ulCnt != 0)
	{
		*pulPvrCnt = ulCnt;
		return ERR_OK;
	}

	return ERR_FAIL;
}

/*
	 device가 PVR용 HDD로 등록되어있는지 확인하는 함수
*/
HERROR umma_fs_svc_PvrCheckRegisteredPvr (UMMA_FS_DevIdx_e eDevIdx)
{
	PAL_FS_UINT32	 ulExtFileDsc, ulSize = 0;
	HUINT8			szStbInfo[UMMA_FS_STB_UNIQUE_BUFSIZE], szHddInfo[UMMA_FS_STB_UNIQUE_BUFSIZE];
	HCHAR			szFileName[UMMA_FS_MOUNT_PATH_LEN+UMMA_FS_EXTHDD_REGFILE_LEN] = {0,};
	HERROR			 hErr;
	HUINT32			ulPartIdx=0;
	UMMA_FS_DevContext_t		*pstDevice = NULL;

	HxLOG_Print("(%s) (%d)------------- Check routine -------------\n",__FUNCTION__,__LINE__);

	// mount path를 얻고 오픈할 파일 이름을 만든다
	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);

	if (hErr != ERR_OK || pstDevice->pstPartArray == NULL)
	{
		HxLOG_Critical("\n GetDeviceInfo Error. \n\n");
		return ERR_FAIL;
	}

	HxSTD_PrintToStrN(szFileName, UMMA_FS_MOUNT_PATH_LEN+UMMA_FS_EXTHDD_REGFILE_LEN,"%s%s", pstDevice->pstPartArray[ulPartIdx].szMountPath, UMMA_FS_EXTHDD_REGFILE);

	// File 존재 확인
	if (HLIB_FILE_Exist(szFileName) == FALSE)
	{
		HxLOG_Error("\nRegister file does not exist !\n\n");
		return ERR_FAIL;
	}

	// 존재 하면 파일 읽어서 STB와 Unique 정보 비교
	VK_memset(szHddInfo, 0, UMMA_FS_STB_UNIQUE_BUFSIZE);
	VK_memset(szStbInfo, 0, UMMA_FS_STB_UNIQUE_BUFSIZE);

	hErr = PAL_FS_Open (0, szFileName, PAL_FS_MODE_READ, &ulExtFileDsc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_svc_PvrCheckRegisteredPvr] PAL_FS_Open err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	hErr = PAL_FS_Read (ulExtFileDsc, szHddInfo, UMMA_FS_STB_UNIQUE_BUFSIZE-1, &ulSize);
	if (hErr != ERR_OK || ulSize <= 0)
	{
		HxLOG_Error("[umma_fs_svc_PvrCheckRegisteredPvr] PAL_FS_Read err(0x%08x)\n", hErr);
		PAL_FS_Close (ulExtFileDsc);
		return ERR_FAIL;
	}

	PAL_FS_Close (ulExtFileDsc);

	// STB unique 정보를 얻어온다.
	hErr = umma_fs_svc_PvrGetStbUniqueInfo(szStbInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_svc_PvrCheckRegisteredPvr] umma_fs_svc_PvrGetStbUniqueInfo err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	HxLOG_Debug("szStbInfo : %x %x %x %x %x %x %x %x \n", szStbInfo[0], szStbInfo[1], szStbInfo[2], szStbInfo[3], szStbInfo[4], szStbInfo[5], szStbInfo[6], szStbInfo[7] );
	HxLOG_Debug("szHddInfo : %x %x %x %x %x %x %x %x \n", szHddInfo[0], szHddInfo[1], szHddInfo[2], szHddInfo[3], szHddInfo[4], szHddInfo[5], szHddInfo[6], szHddInfo[7] );

	// 비교
	if(VK_memcmp(szHddInfo, szStbInfo,8) == 0)
	{
		HxLOG_Print("(%s) (%d)++++++ Registered PVR HDD ++++++\n",__FUNCTION__,__LINE__);
		return ERR_OK;
	}
	HxLOG_Print("(%s) (%d)++++++ Unregistered PVR HDD ++++++\n",__FUNCTION__,__LINE__);

	return ERR_FAIL;
}


HERROR umma_fs_svc_PvrRegisterPvrHdd (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx)
{
	HERROR					hErr;
	HUINT32					ulPvrIdx = 0;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HUINT8					szDeviceUuid[VOLUUIDSZ + 1];
#if !defined(CONFIG_MW_MM_STORAGE_HOLD_PAIRING)
	HUINT32					ulMaxRec=0, ulCount = 0;
#endif
	VK_memset(szDeviceUuid, 0, VOLUUIDSZ + 1);

	/* Create File */
	HxLOG_Print("[umma_fs_svc_PvrRegisterPvrHdd] ======= Register Ext HDD =======\n");

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if(hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("umma_fs_dev_GetDeviceInfo() Error\n");
		return ERR_FAIL;
	}

	// PVR Index 부여
	hErr = umma_fs_svc_PvrAssignPvrIdx(&ulPvrIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n Can't assign PVR idx. \n\n");
		return ERR_FAIL;
	}

	pstDevice->pstPartArray[ulPartIdx].bPvrFlag = TRUE;
	pstDevice->pstPartArray[ulPartIdx].ulPvrIdx= ulPvrIdx;
#if defined(CONFIG_MW_MM_STORAGE_HOLD_PAIRING)
	umma_fs_svc_PvrSetPvrDefaultHdd(ulPvrIdx, TRUE);	// ulPvrIdx는 아마 0일 것이다.
#else
	// default hdd DB field가 비어 있으면 default로 설정한다.
	hErr = _umma_fs_svc_PvrGetDefaultHddInfo(szDeviceUuid,&ulMaxRec);
	if (hErr != ERR_OK)
	{
		umma_fs_svc_PvrSetPvrDefaultHdd(ulPvrIdx, TRUE);	// ulPvrIdx는 아마 0일 것이다.
	}
	else
	{
		// 이것만 등록된 HDD 라면 default HDD로 설정
		umma_fs_svc_PvrGetConnectedPvrStorageCount(&ulCount);
		if(ulCount == 1)
		{
			umma_fs_svc_PvrSetPvrDefaultHdd(ulPvrIdx, FALSE);
		}
	}
#endif
	umma_fs_svc_PvrGetAllPvrPath(pstDevice->eDevType, &pstDevice->pstPartArray[ulPartIdx]);
	HxLOG_Print("[umma_fs_svc_PvrRegisterPvrHdd] pvrIdx : %d\n",ulPvrIdx);


	hErr = umma_fs_svc_WritePvrRegisterInfoToHDD(eDevIdx, ulPartIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("umma_fs_svc_WritePvrRegisterInfoToHDD err(0x%08x) \n",hErr);
		umma_fs_svc_PvrReleasePvrIdx(ulPvrIdx);
		return ERR_FAIL;
	}

	hErr = umma_fs_svc_DirMakeDefaultDirectory(eDevIdx, ulPartIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_svc_PvrRegisterPvrHdd] umma_fs_svc_DirMakeDefaultDirectory err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}

// 동작중인 PVR HDD의 기능을 Disable
HERROR	umma_fs_svc_PvrDisableExtPvrHdd(HUINT32	ulPvrIdx)
{
	UMMA_FS_DevIdx_e 		eDevIdx;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR				hErr;
	HUINT32				ulIdx;
	HUINT32			ulOldPvrIdx = DxSTORAGE_INVALID_PVR_INDEX;
	UMMA_FS_PvrType_e	ePvrType;

	hErr = umma_fs_svc_PvrFindPvrDevIdPartIdx(ulPvrIdx, &eDevIdx, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("umma_fs_svc_PvrFindPvrDevIdPartIdx() Error (ulPvrIdx : %d)\n", ulPvrIdx);
		return hErr;
	}

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if(hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("umma_fs_dev_GetDeviceInfo() Error\n");
		return ERR_FAIL;
	}

	for (ulIdx = 0 ; ulIdx < pstDevice->ulNumPartition ; ulIdx++)
	{
		if (pstDevice->pstPartArray[ulIdx].bPvrFlag == TRUE)
		{
			break;
		}
	}

	if (ulIdx == pstDevice->ulNumPartition)
	{
		HxLOG_Error("There are no pvr partitions!\n");
		return ERR_FAIL;
	}

	ulOldPvrIdx = pstDevice->pstPartArray[ulIdx].ulPvrIdx;

	pstDevice->pstPartArray[ulIdx].bPvrFlag = FALSE;
	pstDevice->pstPartArray[ulIdx].ulPvrIdx = DxSTORAGE_INVALID_PVR_INDEX;
	pstDevice->pstPartArray[ulIdx].bDefaultPvr = FALSE;
	pstDevice->pstPartArray[ulIdx].bSystemFlag = FALSE;

	ePvrType = umma_fs_config_GetPvrType(pstDevice->eDevType);
	// if playback path is not needed
	if ( !(ePvrType & eFS_PVR_RecStorage) )
	{
		// reset playback path (=record path)
		HxSTD_MemSet(pstDevice->pstPartArray[ulIdx].szRecordPath, 0x00, UMMA_FS_MOUNT_PATH_LEN);
	}

	HxSTD_MemSet(pstDevice->pstPartArray[ulIdx].szTsrPath, 0x00, UMMA_FS_MOUNT_PATH_LEN);

	umma_fs_svc_PvrReleasePvrIdx(ulPvrIdx);

	(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_PVR_DISABLED, (HINT32)eDevIdx, 0, ulOldPvrIdx);

	return ERR_OK;
}

HERROR umma_fs_svc_PvrUnRegisterExtPvrHdd (UMMA_FS_DevIdx_e eDevIdx)
{
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR				hErr;
	HCHAR			szFileName[UMMA_FS_MOUNT_PATH_LEN+UMMA_FS_EXTHDD_REGFILE_LEN] = {0,};
	HUINT32			ulIdx, ulPvrIdx;
	HBOOL			bPvr = FALSE;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if(hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("umma_fs_dev_GetDeviceInfo() Error\n");
		return ERR_FAIL;
	}

	for (ulIdx = 0 ; ulIdx < pstDevice->ulNumPartition ; ulIdx++)
	{
		if (pstDevice->pstPartArray[ulIdx].bPvrFlag == TRUE)
		{
			HxSTD_PrintToStrN(szFileName, UMMA_FS_MOUNT_PATH_LEN+UMMA_FS_EXTHDD_REGFILE_LEN,"%s%s", pstDevice->pstPartArray[ulIdx].szMountPath, UMMA_FS_EXTHDD_REGFILE);
			ulPvrIdx = pstDevice->pstPartArray[ulIdx].ulPvrIdx;
			bPvr = TRUE;
			break;
		}
	}

	if (!bPvr)
	{
		if (pstDevice->ulNumPartition == 1)
		{
			HxSTD_PrintToStrN(szFileName, UMMA_FS_MOUNT_PATH_LEN+UMMA_FS_EXTHDD_REGFILE_LEN,"%s%s", pstDevice->pstPartArray[0].szMountPath, UMMA_FS_EXTHDD_REGFILE);
		}
	}

	hErr = HLIB_FILE_Delete(szFileName);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Delete file error!! (%s)\n", szFileName);
	}

	if (bPvr)
	{
		return umma_fs_svc_PvrDisableExtPvrHdd(ulPvrIdx);
	}

	return ERR_OK;
}


HERROR umma_fs_svc_PvrUnregisterExtPvr (UMMA_FS_DevIdx_e eDevIdx,	HUINT32 ulPartIdx)
{
	// 구조상 사용하지 않는 Code, 삭제 예정
#if 0
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR			 hErr;
	HUINT8			szFileName[50] = {0,};
	HUINT32 		ulRegIdx;

	hErr = umma_fs_svc_PvrGetRegIdxByDevIdx(&ulRegIdx, eDevIdx);

	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_svc_PvrUnregisterExtPvr] umma_fs_svc_PvrGetRegIdxByDevIdx err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	HxSTD_PrintToStr(szFileName, "%s_%d\0", SVC_FS_EXTHDD_UIID_REGFILE, ulRegIdx);


	/* delete the previous ext_hdd_info file */
	PAL_FS_Delete(0, szFileName);
	MWC_FILE_Flush();

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr == ERR_OK && pstDevice != NULL && pstDevice->eDevType == eFS_DevType_UsbHdd)
	{
		// Release Slot
		umma_fs_svc_PvrReleasePvrIdx(pstDevice->pstPartArray[ulPartIdx].ulPvrIdx);

		pstDevice->pstPartArray[ulPartIdx].bPvrFlag = FALSE;
		pstDevice->pstPartArray[ulPartIdx].ulPvrIdx= 0;
	}
#endif

	return ERR_OK;
}


HERROR umma_fs_svc_PvrSwitchExtPvrHddDev (UMMA_FS_DevIdx_e eDevIdx)
{
	HUINT32			 ulPartIdx;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR			 hErr;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	hErr = umma_fs_svc_PvrFindPvrPossiblePartitionInDevice (eDevIdx, &ulPartIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("\n There are no available partition for PVR!!(eDevIdx : %d)\n\n", eDevIdx);
		return ERR_FAIL;
	}

	hErr = umma_fs_svc_PvrRegisterPvrHdd (eDevIdx, ulPartIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n [umma_fs_svc_PvrSwitchExtPvrHddDev] umma_fs_svc_PvrRegisterPvrHdd error\n\n");
		return ERR_FAIL;
	}

	// EXT-HDD일 경우에만 PVR partition을 System Partition과 동일하게 쓴다.
	if ((eFS_DevType_eSataHdd == pstDevice->eDevType) ||
		(eFS_DevType_UsbHdd == pstDevice->eDevType))
	{
		pstDevice->pstPartArray[ulPartIdx].bSystemFlag = TRUE;
	}

	(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_PVR_ENABLED, (HINT32)eDevIdx, 0, pstDevice->pstPartArray[ulPartIdx].ulPvrIdx);

	return ERR_OK;
}

HERROR umma_fs_svc_PvrMakePlaybackInfo (UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPartIdx)
{
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	UMMA_FS_PartitionInfo_t 	*pstPartInfo = NULL;
	HERROR						hErr;
	HUINT32						ulPlaybackIdx = DxSTORAGE_INVALID_PVR_INDEX;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("umma_fs_dev_GetDeviceInfo err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	hErr = umma_fs_svc_PvrAssignPlaybackIdx(&ulPlaybackIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("umma_fs_svc_PvrAssignPlaybackIdx failed. eDevIdx(%d) ulPartIdx(%d) \n",eDevIdx,ulPartIdx);
		return ERR_FAIL;
	}

	pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);
	if(pstPartInfo == NULL)
	{
		HxLOG_Error("Error, There is no Partition Info(%d).\n", ulPartIdx);
		return ERR_FAIL;
	}

	pstPartInfo->ulPlaybackIdx = ulPlaybackIdx;

	// Pvr은 아니지만 recording list를 담아 놓기 위해 record path를 생성해둔다.
	umma_fs_svc_DirMakePlaybackDirectory(eDevIdx, ulPartIdx);
	umma_fs_svc_PvrGetPlaybackPvrPath(pstDevice->eDevType, pstPartInfo);

	return ERR_OK;
}

HERROR umma_fs_svc_PvrCheckDeviceIsExtPvrPaired (UMMA_FS_DevIdx_e eDevIdx, HBOOL bAfterFormat, HUINT32 ulPartIdx)
{
	HUINT32					ulPvrIdx =0;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR					hErr;
	HBOOL					bResult=FALSE, bDefault;
#if !defined (CONFIG_MW_MM_STORAGE_HOLD_PAIRING)
	HUINT32					ulCount=0;
#endif

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("[umma_fs_svc_PvrCheckDeviceIsExtPvrPaired] umma_fs_dev_GetDeviceInfo err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	hErr = umma_fs_svc_PvrCheckRegisteredPvr(eDevIdx);
	if (hErr == ERR_OK)
	{
#if defined (CONFIG_MW_MM_STORAGE_HOLD_PAIRING)
		//default HDD로 등록된 device인지 확인
		bDefault = umma_fs_svc_PvrCheckDeviceIsDefaultPvrHdd(eDevIdx);
		if(bDefault == TRUE)
		{
			HxLOG_Print("[umma_fs_svc_PvrCheckDeviceIsExtPvrPaired] Dev (%d:%d) to PVR Partition\n", eDevIdx, ulPartIdx);
			pstDevice->pstPartArray[ulPartIdx].bPvrFlag = TRUE;
			pstDevice->pstPartArray[ulPartIdx].bSystemFlag = TRUE;

			//PvrIdx 할당
			umma_fs_svc_PvrAssignPvrIdx(&ulPvrIdx);
			pstDevice->pstPartArray[ulPartIdx].ulPvrIdx = ulPvrIdx;

			// 새로 할당 받은 놈을 default Pvr로 교체
			umma_fs_svc_PvrSetPvrDefaultHdd(ulPvrIdx, TRUE);
			umma_fs_svc_PvrGetAllPvrPath(pstDevice->eDevType, &pstDevice->pstPartArray[ulPartIdx]);
			//만약 Default로 MMC를 Set에서 Format안한다면 아래코드 필요함 (현재는 diable)
			//hErr = umma_fs_svc_DirMakeDefaultDirectory(eDevIdx, ulPartIdx);
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}
#else
		HxLOG_Print("++++++ Already registered HDD ++++++\n");

		// 이미 등록된 HDD
		HxLOG_Print("[umma_fs_svc_PvrCheckDeviceIsExtPvrPaired] Dev (%d:%d) to PVR Partition\n", eDevIdx, ulPartIdx);
		pstDevice->pstPartArray[ulPartIdx].bPvrFlag = TRUE;
		pstDevice->pstPartArray[ulPartIdx].bSystemFlag = TRUE;

		//PvrIdx 할당
		umma_fs_svc_PvrAssignPvrIdx(&ulPvrIdx);
		pstDevice->pstPartArray[ulPartIdx].ulPvrIdx = ulPvrIdx;

		//default HDD로 등록된 device인지 확인
		bDefault = umma_fs_svc_PvrCheckDeviceIsDefaultPvrHdd(eDevIdx);
		if(bDefault == TRUE)
		{
			// 새로 할당 받은 놈을 default Pvr로 교체
			umma_fs_svc_PvrSetPvrDefaultHdd(ulPvrIdx, FALSE);
		}
		else
		{
			// 이것만 등록된 HDD 라면 default HDD로 설정
			umma_fs_svc_PvrGetConnectedPvrStorageCount(&ulCount);
			if(ulCount == 1)
			{
				umma_fs_svc_PvrSetPvrDefaultHdd(ulPvrIdx, FALSE);
			}
		}

		umma_fs_svc_PvrGetAllPvrPath(pstDevice->eDevType, &pstDevice->pstPartArray[ulPartIdx]);
		//만약 Default로 MMC를 Set에서 Format안한다면 아래코드 필요함 (현재는 diable)
		//hErr = umma_fs_svc_DirMakeDefaultDirectory(eDevIdx, ulPartIdx);
		bResult = TRUE;
#endif

	}
	else
	{
		// Device가 실제로 PVR 가능한지도 체크한다.
		hErr = umma_fs_svc_PvrFindPvrPossiblePartitionInDevice (eDevIdx, NULL);
		if ( (ERR_OK == hErr) && (pstDevice->bIsExPvrModule == TRUE || bAfterFormat == TRUE))
		{
			HxLOG_Warning("[umma_fs_svc_PvrCheckDeviceIsExtPvrPaired] Previous PVR HDD: register the new one.\n");

			// Register the HDD as the PVR
			umma_fs_svc_PvrRegisterPvrHdd (eDevIdx, ulPartIdx);
			//	umma_fs_svc_PvrRegisterPvrHdd 함수 내에서 ulPvrIdx를 할당한다.
			// EXT-HDD일 경우에만 PVR partition을 System Partition과 동일하게 쓴다.

			if ((eFS_DevType_eSataHdd == pstDevice->eDevType) ||
				(eFS_DevType_UsbHdd == pstDevice->eDevType))
			{
				if (TRUE == pstDevice->pstPartArray[ulPartIdx].bPvrFlag)
				{
					pstDevice->pstPartArray[ulPartIdx].bSystemFlag = TRUE;
				}
			}

			(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_PVR_ENABLED, (HINT32)eDevIdx, 0, pstDevice->pstPartArray[ulPartIdx].ulPvrIdx);

			bResult = TRUE;
		}
	}

	if(bResult == TRUE)
	{
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL;
	}

}

// PVR HDD 등록을 위한 STB unique 정보
HERROR umma_fs_svc_PvrGetStbUniqueInfo(HUINT8 *pszInfo)
{
	HERROR			 hErr;
	HUINT8			szKey[UMMA_FS_STB_UNIQUE_BUFSIZE],  szOut[UMMA_FS_STB_UNIQUE_BUFSIZE];

	VK_memset (szKey, 0, UMMA_FS_STB_UNIQUE_BUFSIZE);

	// Chip ID를 이용해서 8byte key 생성
	hErr = OAPI_PLATFORM_GetKey(szKey);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("OAPI_PLATFORM_GetKey Error \n");
		return ERR_FAIL;
	}
	HxLOG_Print("[%s] [%d] Key !! (%x %x %x %x %x %x %x %x)\n",__FUNCTION__,__LINE__, szKey[0], szKey[1], szKey[2], szKey[3], szKey[4], szKey[5], szKey[6], szKey[7]);

	VK_memset (szOut, 0, UMMA_FS_STB_UNIQUE_BUFSIZE);

	hErr = HLIB_CRYPT_DES_EncryptData(szKey, "PvrHddOk", pszInfo, UMMA_FS_STB_UNIQUE_BUFSIZE-1 );
	if(hErr != ERR_OK)
	{
		HxLOG_Error("HLIB_CRYPT_DES_EncryptData Error!! \n");
		return ERR_FAIL;
	}
	//HxLOG_Print("[%s] [%d] pszInfo : %8s\n",__FUNCTION__,__LINE__, pszInfo);

#if 0	// for test
	{
		HUINT8	szDecOut[9];
		VK_memset (szDecOut, 0, 9);
		HLIB_CRYPT_DES_DecryptData(szKey, pszInfo, szDecOut, UMMA_FS_STB_UNIQUE_BUFSIZE -1);

		HxLOG_Print("[%s] [%d]szDecOut : %s\n",__FUNCTION__,__LINE__, szDecOut);

	}
#endif
	return ERR_OK;
}

// 해당 device가 default PVR HDD로 등록되어있는지 확인
HBOOL umma_fs_svc_PvrCheckDeviceIsDefaultPvrHdd (UMMA_FS_DevIdx_e eDevIdx)
{
	// HDD의 UUID와 DB의 UUID를 비교해서 같으면 True
	HUINT8		 szDeviceUuid[VOLUUIDSZ + 1], szDefaultUuid[VOLUUIDSZ + 1];
	HUINT32		 ulMaxRec=0;
	HERROR		 hErr;

	VK_memset(szDeviceUuid, 0, VOLUUIDSZ + 1);

	hErr = umma_fs_dev_GetDeviceUuid (eDevIdx, szDeviceUuid);
	if (hErr != ERR_OK)
	{
		return FALSE;
	}

	hErr = _umma_fs_svc_PvrGetDefaultHddInfo(szDefaultUuid,&ulMaxRec);
	if (hErr != ERR_OK)
	{
		return FALSE;
	}

	if (HxSTD_StrNCmp(szDeviceUuid, szDefaultUuid,VOLUUIDSZ) == 0)
	{
		HxLOG_Print("Default PVR HDD\n");
		return TRUE;
	}

	HxLOG_Print("Not default PVR HDD\n");
	return FALSE;

}

// 해당 Pvridx에 대항하는 device를 default PVR HDD로 설정
HERROR umma_fs_svc_PvrSetPvrDefaultHdd(HUINT32 ulPvrIdx, HBOOL bSync)
{
	UMMA_FS_DevIdx_e			 eDevIdx_NewDefault;
	HUINT8		 szDeviceUuid[VOLUUIDSZ + 1];
	HERROR		 hErr;
	UMMA_FS_DevInfoMap_t		*pstContext;
	HUINT32				ulDevCnt, ulPartIdx;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	UMMA_FS_PvrType_e	ePvrType;
	HUINT32		ulMaxRec;

	// UUID를 얻어서 DB에 저장
	hErr = umma_fs_svc_PvrFindPvrDevIdPartIdx (ulPvrIdx, &eDevIdx_NewDefault, NULL);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx_NewDefault, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("[umma_fs_svc_PvrSetPvrDefaultHdd] umma_fs_dev_GetDeviceInfo err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	ePvrType = umma_fs_config_GetPvrType(pstDevice->eDevType);
	if (!(ePvrType & eFS_PVR_Main))
	{
		HxLOG_Error("DevType(%d) can't default PVR device!! pvrType(%d) \n", pstDevice->eDevType,ePvrType);
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_MM_LIMIT_REC)
	if(pstDevice->eDevUsbSpeed < eFS_DevUsbSpeed_5G && pstDevice->eDevType == eFS_DevType_UsbHdd)
	{
		ulMaxRec = CONFIG_MW_MM_LIMIT_REC_NUM;
	}
	else
#endif
	{
		ulMaxRec = CONFIG_MW_MM_MAX_REC_NUM;
	}

	// Default Pvr 정보 update
	pstDevice = NULL;
	pstContext = umma_fs_dev_GetContext();

	for (ulDevCnt = 0; ulDevCnt < eFS_Dev_MAX; ulDevCnt++)
	{
		pstDevice = &(pstContext->astDevice[ulDevCnt]);
		HxLOG_Print("[%s:%d] [%02d] eDevType:0x%X bDevAvailable:%d bDevConnected:%d \n",__FUNCTION__,__LINE__,
				ulDevCnt,pstDevice->eDevType,pstDevice->bDevAvailable,pstDevice->bDevConnected);
		if (pstDevice->bDevAvailable != TRUE || pstDevice->bDevConnected != TRUE)		{ continue; }
		{
			for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
			{
				if(pstDevice->pstPartArray[ulPartIdx].bMounted != TRUE)		{ continue; }

				if (pstDevice->pstPartArray[ulPartIdx].bPvrFlag == TRUE &&
					ulPvrIdx == pstDevice->pstPartArray[ulPartIdx].ulPvrIdx)
				{
					pstDevice->pstPartArray[ulPartIdx].bDefaultPvr = TRUE;
				}
				else
				{
					pstDevice->pstPartArray[ulPartIdx].bDefaultPvr = FALSE;
				}
			}
		}
	}

	// check changed
	{
		HUINT32 ulOldMaxRec = 0;
		HUINT8	szOldDeviceUuid[VOLUUIDSZ + 1];
		VK_memset(szOldDeviceUuid, 0, VOLUUIDSZ + 1);
		hErr = _umma_fs_svc_PvrGetDefaultHddInfo(szOldDeviceUuid,&ulOldMaxRec);
		szOldDeviceUuid[VOLUUIDSZ] = '\0';
		hErr |= umma_fs_dev_GetDeviceUuid (eDevIdx_NewDefault, szDeviceUuid);
		if (hErr == ERR_OK)
		{
			if (HxSTD_StrNCmp(szOldDeviceUuid,szDeviceUuid,VOLUUIDSZ) != 0
				|| (ulOldMaxRec != ulMaxRec))
			{
				HCHAR *szOldDevUuid = NULL;
				HCHAR *szNewDevUuid = NULL;
				HUINT32	ulPackedRec = 0;
				HUINT16	usNewRec = (HUINT16)ulMaxRec;

				HxLOG_Warning("default hdd info changed. old uid[%s] new uid[%s] \n",szOldDeviceUuid,szDeviceUuid);
				HxLOG_Warning("default hdd info changed. old rec(%d) new rec(%d) \n",ulOldMaxRec,ulMaxRec);

				szOldDevUuid = (HCHAR *)UMMA_Malloc(sizeof(HCHAR)*(VOLUUIDSZ+1));
				szNewDevUuid = (HCHAR *)UMMA_Malloc(sizeof(HCHAR)*(VOLUUIDSZ+1));
				ulPackedRec = (HUINT32)((HUINT32)ulOldMaxRec << 16) | (HUINT32)usNewRec;
				if (szOldDevUuid != NULL && szNewDevUuid != NULL)
				{
					HxSTD_snprintf(szOldDevUuid,VOLUUIDSZ+1,"%s",szOldDeviceUuid);
					HxSTD_snprintf(szNewDevUuid,VOLUUIDSZ+1,"%s",szDeviceUuid);
					(void)umma_fs_svc_SendDataToMainloop(eFS_EVT_DEFAULTHDD_INFO_CHANGED, (void *)szOldDevUuid, (HINT32)HLIB_STD_StrNLen(szOldDevUuid,VOLUUIDSZ+1)
																						, (void *)szNewDevUuid, (HINT32)HLIB_STD_StrNLen(szNewDevUuid,VOLUUIDSZ+1)
																						, (HINT32)ulPackedRec);
				}

				if (szOldDevUuid != NULL) {HLIB_STD_MemFree(szOldDevUuid); }
				if (szNewDevUuid != NULL) {HLIB_STD_MemFree(szNewDevUuid); }
			}
		}
	}

	// DB 저장 여부 확인
	if(bSync)
	{
		VK_memset(szDeviceUuid, 0, VOLUUIDSZ + 1);

		hErr = umma_fs_dev_GetDeviceUuid (eDevIdx_NewDefault, szDeviceUuid);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		// DB에 저장
		umma_fs_svc_PvrSetDefaultHddInfo(szDeviceUuid, ulMaxRec);
#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if (HLIB_EXTRA_ChownWithParam(HDD_DEFAULT_INFO_NAME, UID_umma, GID_pvr) != ERR_OK)
		{
			HxLOG_Error("HLIB_EXTRA_ChownWithParam (%s) err (0x%08x)\n", HDD_DEFAULT_INFO_NAME, hErr);
		}

		if (HLIB_DIR_Chmod(HDD_DEFAULT_INFO_NAME, 0770) != ERR_OK)
		{
			HxLOG_Error ("HLIB_DIR_Chmod (%s) err (0x%08x)\n", HDD_DEFAULT_INFO_NAME, hErr);
		}
#endif

	}

	return ERR_OK;
}

 //설명 : 기존에 연결된 default PVR HDD가 제거 되었을때 남은(연결된) HDD 중 default PVR HDD를 선택하는 함수
HERROR umma_fs_svc_PvrSelectNewDefaultPvrIdx(HUINT32 *pulPvrIdx)
{
	HUINT32			aulIdxSlot[5] = {0xffff, 0xffff, 0xffff, 0xffff, 0xffff};
	UMMA_FS_DevInfoMap_t		*pstContext;
	HUINT32				ulDevCnt, ulPartIdx, i;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	pstDevice = NULL;
	pstContext = umma_fs_dev_GetContext();

	/* Default HDD 자동 선정 방식 */
	//우선순위 : Internal > detachable HDD > eSATA HDD > USB HDD > USB stick
	 //같은 type이 2개 이상이면 먼저 찾은 것으로 선택함
	for (ulDevCnt = 0; ulDevCnt < eFS_Dev_MAX; ulDevCnt++)
	{
		pstDevice = &(pstContext->astDevice[ulDevCnt]);
		HxLOG_Print("[%s:%d] [%02d] eDevType:0x%X bDevAvailable:%d bDevConnected:%d\n",__FUNCTION__,__LINE__,
				ulDevCnt,pstDevice->eDevType,pstDevice->bDevAvailable,pstDevice->bDevConnected);
		if (pstDevice->bDevAvailable != TRUE || pstDevice->bDevConnected != TRUE)		{ continue; }
		{
			for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
			{
				if(pstDevice->pstPartArray[ulPartIdx].bMounted != TRUE || pstDevice->pstPartArray[ulPartIdx].bPvrFlag != TRUE )		{ continue; }

				 //1st : internal HDD
				if (pstDevice->eDevType == eFS_DevType_IntHdd)
				{
					if(aulIdxSlot[0] == 0xffff) {aulIdxSlot[0] = pstDevice->pstPartArray[ulPartIdx].ulPvrIdx;}
				}
				//2nd : detachable HDD
				else if (pstDevice->eDevType == eFS_DevType_Detachable)
				{
					if(aulIdxSlot[1] == 0xffff) {aulIdxSlot[1] = pstDevice->pstPartArray[ulPartIdx].ulPvrIdx;}
				}
				//3rd : eSATA HDD
				else if (pstDevice->eDevType == eFS_DevType_eSataHdd)
				{
					if(aulIdxSlot[2] == 0xffff) {aulIdxSlot[2] = pstDevice->pstPartArray[ulPartIdx].ulPvrIdx;}
				}
				//4th : USB HDD
				else if (pstDevice->eDevType == eFS_DevType_UsbHdd)
				{
					if(aulIdxSlot[3] == 0xffff) {aulIdxSlot[3] = pstDevice->pstPartArray[ulPartIdx].ulPvrIdx;}
				}
				//5th : USB stick
				else if (pstDevice->eDevType == eFS_DevType_UsbMem)
				{
					if(aulIdxSlot[4] == 0xffff) {aulIdxSlot[4] = pstDevice->pstPartArray[ulPartIdx].ulPvrIdx;}
				}
			}
		}
	}

	 //Slot을 조사해서 채워져있는 것이 발견되면 값 저장하고 리턴
	for(i = 0 ; i < 5 ; i++)
	{
		if(aulIdxSlot[i] != 0xffff)
		{
			if(pulPvrIdx)		*pulPvrIdx = aulIdxSlot[i];
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR	umma_fs_svc_PvrGetDefaultHddInfo(HUINT8 *szUuid, HUINT32 *ulMaxRec)
{
	HERROR		 hErr=ERR_FAIL;

	hErr=_umma_fs_svc_PvrGetDefaultHddInfo(szUuid, ulMaxRec);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("_umma_fs_svc_PvrGetDefaultHddInfo error\n");
	}

	return hErr;
}

#define __________________FS_PVR_DIR________________________
INLINE STATIC void umma_fs_svc_DirAddPartition (UMMA_FS_FormatPartList_t *pstFormatList, HUINT32 ulPartIdx, UMMA_FS_FsType_e eFsType, UMMA_FS_PvrPartitionType_e ePartType, HBOOL bSecured, HUINT32 ulStartUnit, HUINT32 ulUnitSize, HUINT8 *szMountPath, HUINT8 *szVolName, HUINT32 *pulNextUnit)
{
	UMMA_FS_FormatPartInfo_t	*pstFormatInfo;

	if (pstFormatList->ulPartitionNum < UMMA_FS_MAX_PARTITION)
	{
		pstFormatInfo = &(pstFormatList->astPartArray[pstFormatList->ulPartitionNum]);

		pstFormatInfo->ulPartIdx		= ulPartIdx;
		pstFormatInfo->ulStartUnit		= ulStartUnit;
		pstFormatInfo->ulEndUnit		= ulStartUnit + ulUnitSize;
		pstFormatInfo->ePartFsType		= eFsType;
		pstFormatInfo->ePartType		= ePartType;
		pstFormatInfo->bSecured		= bSecured;	//MOON_encr
		HLIB_STD_StrNCpySafe(pstFormatInfo->szMountPath, szMountPath, UMMA_FS_MOUNT_PATH_LEN);
		HLIB_STD_StrNCpySafe(pstFormatInfo->szVolumeName, szVolName, UMMA_FS_VOLUME_LABEL_SIZE);

		if (pulNextUnit != NULL)		{ *pulNextUnit = pstFormatInfo->ulEndUnit + 1; }

		pstFormatList->ulPartitionNum ++;
	}
}

#define _____PARTITION_SETTING__NORMAL_PROFILE___

#if defined(CONFIG_MW_MM_PVR_PARTITION_PROFILE_NORMAL)
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
STATIC HERROR umma_fs_svc_DirGetIntPvrHddPartSetting_BasicProfile (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName, UMMA_FS_FormatPartList_t *pstFormatList)
{
	PAL_FS_StorDevIdx_t		 ePalDevIdx;
	HUINT32					 ulStartUnit;
	HUINT32					 ulP1UnitNum, ulP2UnitNum, ulP3UnitNum;
	HUINT64					 ullSizePart1, ullSizePart3;
	PAL_FS_PartitionList_t	*pstPalPartList = NULL;
	HERROR					 hResult = ERR_FAIL;
	UMMA_FS_FsType_e	eFileSystem;

	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);

	pstPalPartList	= (PAL_FS_PartitionList_t *)UMMA_Malloc (sizeof(PAL_FS_PartitionList_t));
	if (pstPalPartList == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	VK_memset (pstPalPartList, 0, sizeof(PAL_FS_PartitionList_t));
	PAL_FS_GetPartitionFromDeviceMbr (ePalDevIdx, pstPalPartList);
	if (pstPalPartList->ullUnitSize == 0L || pstPalPartList->ulNumUnit == 0)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	//	한번에 계산 하면 엉뚱한 값이 들어간다. 주의 !!! (ulSizePart1 = 2 * 1024 * 1024 *  1024)로 하면 안됨.)
	ullSizePart1  = HDD_BASICPROFILE_SYSTEM_PARTSIZE;
	ullSizePart1 *= (1024 * 1024 * 1024);

	ullSizePart3  = HDD_BASICPROFILE_IP_PARTSIZE;
	ullSizePart3 *= (1024 * 1024 * 1024);

	ulP1UnitNum = (HUINT32)(ullSizePart1 / (HUINT64)pstPalPartList->ullUnitSize);
	ulP3UnitNum = (HUINT32)(ullSizePart3 / (HUINT64)pstPalPartList->ullUnitSize);
	ulP2UnitNum = (HUINT32)(pstPalPartList->ulNumUnit - ulP1UnitNum - ulP3UnitNum - 3);		// 3을 더 빼는  이유는 원래 코드에서부터 온 것이고, 빼기 없이 그대로 계산할 때의 문제점을 차후 체크하기로 한다.

	pstFormatList->eDevIdx			= eDevIdx;
	pstFormatList->ullUnitSize		= pstPalPartList->ullUnitSize;

	eFileSystem=  umma_fs_config_GetDefaultFS(eFS_DevType_IntHdd);
	if (eFileSystem == eFS_FileSystem_NONE)
	{
		HxLOG_Error("Unavailable File system !! (%d) \n", eFileSystem);
		hResult = ERR_FAIL;
		goto ERROR;
	}

	// Partition 0 : System Partition (TVTV, etc)
	ulStartUnit = 1;
	umma_fs_svc_DirAddPartition (pstFormatList, 0, eFileSystem, eFS_PvrPartitionType_System, FALSE, ulStartUnit, ulP1UnitNum, PVR_PATH_NORMAL_SYSTEM_ROOT, szVolumeName, &ulStartUnit);

	// Partition 1 : Media Partition
	umma_fs_svc_DirAddPartition (pstFormatList, 1, eFileSystem, eFS_PvrPartitionType_PvrMedia, FALSE, ulStartUnit, ulP2UnitNum, PVR_PATH_NORMAL_MEDIA_ROOT, szVolumeName, &ulStartUnit);

	// Partition 2 : Reservation Partition (IPTV, etc)
	umma_fs_svc_DirAddPartition (pstFormatList, 2, eFileSystem, eFS_PvrPartitionType_WebBrowse, FALSE, ulStartUnit, ulP3UnitNum, PVR_PATH_NORMAL_RESERVED_ROOT, szVolumeName, &ulStartUnit);

	hResult = ERR_OK;

ERROR:
	if (pstPalPartList != NULL)				{ UMMA_Free (pstPalPartList); }
	return hResult;
}
#endif

#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
STATIC HERROR umma_fs_svc_DirGetExtPvrHddPartSetting_BasicProfile (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName, UMMA_FS_FormatPartList_t *pstFormatList)
{
	HBOOL					 bFound;
	HUINT32					 ulNewDriveNo;
	HUINT32					 ulStartUnit;
	PAL_FS_StorDevIdx_t		 ePalDevIdx;
	PAL_FS_PartitionList_t	*pstPalPartList = NULL;
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	HUINT8					 szMountPath[UMMA_FS_MOUNT_PATH_LEN];
	HERROR					 hErr, hResult = ERR_FAIL;
	UMMA_FS_FsType_e	eFileSystem;

	bFound = FALSE;
	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (pstDevice == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}
	if (hErr == ERR_OK && pstDevice != NULL)
	{
		if (pstDevice->bDevMounted == TRUE)
		{
			bFound = TRUE;
			umma_fs_dev_GetDrivePath (eDevIdx, szMountPath);
		}
	}

	if (bFound == FALSE)
	{
		umma_fs_dev_GetNewDriveNo (&ulNewDriveNo);
		umma_fs_dev_MakeMountPathByDriveNo (ulNewDriveNo, szMountPath);
	}

	pstPalPartList	= (PAL_FS_PartitionList_t *)UMMA_Malloc (sizeof(PAL_FS_PartitionList_t));
	if (pstPalPartList == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	VK_memset (pstPalPartList, 0, sizeof(PAL_FS_PartitionList_t));
	PAL_FS_GetPartitionFromDeviceMbr (ePalDevIdx, pstPalPartList);
	if (pstPalPartList->ullUnitSize == 0L || pstPalPartList->ulNumUnit == 0)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	pstFormatList->eDevIdx			= eDevIdx;
	pstFormatList->ullUnitSize		= pstPalPartList->ullUnitSize;

	eFileSystem=  umma_fs_config_GetDefaultFS(pstDevice->eDevType);
	if (eFileSystem == eFS_FileSystem_NONE)
	{
		HxLOG_Error("Unavailable File system !! (%d) \n", eFileSystem);
		hResult = ERR_FAIL;
		goto ERROR;
	}

	// Partition 0 : Media Partition
	ulStartUnit = 1;
	umma_fs_svc_DirAddPartition (pstFormatList, 0, eFileSystem, eFS_PvrPartitionType_ExtHdd, FALSE, ulStartUnit, pstPalPartList->ulNumUnit - 1, szMountPath, szVolumeName, &ulStartUnit);

	hResult = ERR_OK;

ERROR:
	if (pstPalPartList != NULL)				{ UMMA_Free (pstPalPartList); }
	return hResult;
}
#endif
#endif

STATIC HERROR umma_fs_svc_DirGetIntPvrHddPartSetting_2ndBasicProfile (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName, UMMA_FS_FormatPartList_t *pstFormatList)
{
	PAL_FS_StorDevIdx_t		 ePalDevIdx;
	HUINT32					 ulStartUnit;
	PAL_FS_PartitionList_t	*pstPalPartList = NULL;
	HERROR					 hResult = ERR_FAIL;
	UMMA_FS_FsType_e	eFileSystem;

	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);

	pstPalPartList	= (PAL_FS_PartitionList_t *)UMMA_Malloc (sizeof(PAL_FS_PartitionList_t));
	if (pstPalPartList == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	VK_memset (pstPalPartList, 0, sizeof(PAL_FS_PartitionList_t));
	PAL_FS_GetPartitionFromDeviceMbr (ePalDevIdx, pstPalPartList);
	if (pstPalPartList->ullUnitSize == 0L || pstPalPartList->ulNumUnit == 0)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	pstFormatList->eDevIdx			= eDevIdx;
	pstFormatList->ullUnitSize		= pstPalPartList->ullUnitSize;

	eFileSystem=  umma_fs_config_GetDefaultFS(eFS_DevType_Detachable);
	if (eFileSystem == eFS_FileSystem_NONE)
	{
		HxLOG_Error("Unavailable File system !! (%d) \n", eFileSystem);
		hResult = ERR_FAIL;
		goto ERROR;
	}


	ulStartUnit = 1;
	umma_fs_svc_DirAddPartition (pstFormatList, INT2NDPVR_PARTIDX, eFileSystem, eFS_PvrPartitionType_PvrMedia, FALSE, ulStartUnit, pstPalPartList->ulNumUnit - 1, PVR_PATH_EXTENDED_MEDIA_ROOT, szVolumeName, &ulStartUnit);

	hResult = ERR_OK;

ERROR:
	if (pstPalPartList != NULL)				{ UMMA_Free (pstPalPartList); }
	return hResult;
}


#define _____PARTITION_SETTING__TVAPPS_PROFILE___

#if defined(CONFIG_MW_MM_PVR_PARTITION_PROFILE_TVAPPS)
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
// TV apps를 위해 별도 partition 추가 지원
STATIC HERROR umma_fs_svc_DirGetIntPvrHddPartSetting_TvAppsProfile (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName, UMMA_FS_FormatPartList_t *pstFormatList)
{
	PAL_FS_StorDevIdx_t		 ePalDevIdx;
	HUINT32					 ulStartUnit;
	HUINT32					 ulP1UnitNum, ulP2UnitNum, ulP3UnitNum, ulP4UnitNum;
	HUINT64					 ullSizePart1, ullSizePart3, ullSizePart4;
	PAL_FS_PartitionList_t		*pstPalPartList = NULL;
	HERROR					 hResult = ERR_FAIL;
	UMMA_FS_FsType_e		eFileSystem;

	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);

	pstPalPartList	= (PAL_FS_PartitionList_t *)UMMA_Malloc (sizeof(PAL_FS_PartitionList_t));
	if (pstPalPartList == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	VK_memset (pstPalPartList, 0, sizeof(PAL_FS_PartitionList_t));
	PAL_FS_GetPartitionFromDeviceMbr (ePalDevIdx, pstPalPartList);
	if (pstPalPartList->ullUnitSize == 0L || pstPalPartList->ulNumUnit == 0)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	//	한번에 계산 하면 엉뚱한 값이 들어간다. 주의 !!! (ulSizePart1 = 2 * 1024 * 1024 *  1024)로 하면 안됨.)
	ullSizePart1  = HDD_BASICPROFILE_SYSTEM_PARTSIZE;
	ullSizePart1 *= (1024 * 1024 * 1024);

	ullSizePart3  = HDD_BASICPROFILE_IP_PARTSIZE;
	ullSizePart3 *= (1024 * 1024 * 1024);

	ullSizePart4  = HDD_BASICPROFILE_TVAPPS_PARTSIZE;
	ullSizePart4 *= (1024 * 1024 * 1024);

	ulP1UnitNum = (HUINT32)(ullSizePart1 / (HUINT64)pstPalPartList->ullUnitSize);
	ulP3UnitNum = (HUINT32)(ullSizePart3 / (HUINT64)pstPalPartList->ullUnitSize);
	ulP4UnitNum = (HUINT32)(ullSizePart4 / (HUINT64)pstPalPartList->ullUnitSize);
	ulP2UnitNum = (HUINT32)(pstPalPartList->ulNumUnit - ulP1UnitNum - ulP3UnitNum - ulP4UnitNum - 4);		// 3을 더 빼는  이유는 원래 코드에서부터 온 것이고, 빼기 없이 그대로 계산할 때의 문제점을 차후 체크하기로 한다.

	pstFormatList->eDevIdx 			= eDevIdx;
	pstFormatList->ullUnitSize		= pstPalPartList->ullUnitSize;

	eFileSystem=  umma_fs_config_GetDefaultFS(eFS_DevType_IntHdd);
	if (eFileSystem == eFS_FileSystem_NONE)
	{
		HxLOG_Error("Unavailable File system !! (%d) \n", eFileSystem);
		hResult = ERR_FAIL;
		goto ERROR;
	}

	// Partition 0 : System Partition (TVTV, etc)
	ulStartUnit = 1;
	umma_fs_svc_DirAddPartition (pstFormatList, 0, eFileSystem, eFS_PvrPartitionType_System, FALSE, ulStartUnit, ulP1UnitNum, PVR_PATH_NORMAL_SYSTEM_ROOT, szVolumeName, &ulStartUnit);

	// Partition 1 : Media Partition
	umma_fs_svc_DirAddPartition (pstFormatList, 1, eFileSystem, eFS_PvrPartitionType_PvrMedia, FALSE, ulStartUnit, ulP2UnitNum, PVR_PATH_NORMAL_MEDIA_ROOT, szVolumeName, &ulStartUnit);

	// Partition 2 : Reservation Partition (IPTV, etc)
	umma_fs_svc_DirAddPartition (pstFormatList, 2, eFileSystem, eFS_PvrPartitionType_WebBrowse, FALSE, ulStartUnit, ulP3UnitNum, PVR_PATH_NORMAL_RESERVED_ROOT, szVolumeName, &ulStartUnit);

	// Partition 3 : TV Apps Partition (TV Apps, etc)
	umma_fs_svc_DirAddPartition (pstFormatList, 3, eFileSystem, eFS_PvrPartitionType_TvApps, TRUE, ulStartUnit, ulP4UnitNum, PVR_PATH_NORMAL_TVAPPS_ROOT, szVolumeName, &ulStartUnit);

	hResult = ERR_OK;

ERROR:
	if (pstPalPartList != NULL)				{ UMMA_Free (pstPalPartList); }
	return hResult;
}
#endif

#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
STATIC HERROR umma_fs_svc_DirGetExtPvrHddPartSetting_TvAppsProfile (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName, UMMA_FS_FormatPartList_t *pstFormatList)
{
	HBOOL					 bFound;
	HUINT32					 ulNewDriveNo;
	HUINT32					 ulStartUnit;
	PAL_FS_StorDevIdx_t		 ePalDevIdx;
	PAL_FS_PartitionList_t	*pstPalPartList = NULL;
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	HUINT8					 szMountPath[UMMA_FS_MOUNT_PATH_LEN];
	HERROR					 hErr, hResult = ERR_FAIL;
	UMMA_FS_FsType_e		eFileSystem;

	bFound = FALSE;
	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr == ERR_OK && pstDevice != NULL)
	{
		if (pstDevice->bDevMounted == TRUE)
		{
			bFound = TRUE;
			umma_fs_dev_GetDrivePath (eDevIdx, szMountPath);
		}
	}

	if (bFound == FALSE)
	{
		umma_fs_dev_GetNewDriveNo (&ulNewDriveNo);
		umma_fs_dev_MakeMountPathByDriveNo (ulNewDriveNo, szMountPath);
	}

	pstPalPartList	= (PAL_FS_PartitionList_t *)UMMA_Malloc (sizeof(PAL_FS_PartitionList_t));
	if (pstPalPartList == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	VK_memset (pstPalPartList, 0, sizeof(PAL_FS_PartitionList_t));
	PAL_FS_GetPartitionFromDeviceMbr (ePalDevIdx, pstPalPartList);
	if (pstPalPartList->ullUnitSize == 0L || pstPalPartList->ulNumUnit == 0)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		goto ERROR;
	}

	pstFormatList->eDevIdx			= eDevIdx;
	pstFormatList->ullUnitSize		= pstPalPartList->ullUnitSize;

	eFileSystem = umma_fs_config_GetDefaultFS(pstDevice->eDevType);
	if (eFileSystem == eFS_FileSystem_NONE)
	{
		HxLOG_Error("Unavailable File system !! (%d) \n", eFileSystem);
		hResult = ERR_FAIL;
		goto ERROR;
	}

	// Partition 0 : Media Partition
	ulStartUnit = 1;
	umma_fs_svc_DirAddPartition (pstFormatList, 0, eFileSystem, eFS_PvrPartitionType_ExtHdd, FALSE, ulStartUnit, pstPalPartList->ulNumUnit - 1, szMountPath, szVolumeName, &ulStartUnit);

	hResult = ERR_OK;

ERROR:
	if (pstPalPartList != NULL)				{ UMMA_Free (pstPalPartList); }
	return hResult;
}
#endif
#endif

#define _____PVR_DEFAULT_DIRECTORY_____
#if defined(CONFIG_MW_MM_PVR_DIR_PROFILE_BASE)
STATIC HERROR umma_fs_svc_DirGetSubPath_BaseProfile (UMMA_FS_UsageType_e eUsage, UMMA_FS_DevContext_t *pstDevice, HUINT8 *szSubPath, HBOOL *pbShare)
{
	UMMA_FS_DevType_e	 eDevType;
	HERROR			 hResult = ERR_FAIL;

	szSubPath[0] = '\0';

	if (pbShare)
	{
		*pbShare = FALSE;
	}

	eDevType = pstDevice->eDevType;

	switch (eUsage)
	{
	case eFS_UsageType_Record:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_RECORD_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_RECORD_EXT, 64);
			hResult = ERR_OK;
			break;

		default:
			break;
		}
		break;

	case eFS_UsageType_Tsr:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_TSR_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
		case eFS_DevType_MMC:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_TSR_EXT, 64);
			hResult = ERR_OK;
			break;

		default:
			break;
		}
		break;

	case eFS_UsageType_MediaPath:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_MEDIA_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_MEDIA_EXT, 64);
			hResult = ERR_OK;
			break;

		default:
			hResult = ERR_OK;
			break;
		}
		break;

	case eFS_UsageType_MediaVideo:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_VIDEO_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_VIDEO_EXT, 64);
			hResult = ERR_OK;
			break;

		default:
			hResult = ERR_OK;
			break;
		}
		break;

	case eFS_UsageType_MediaAudio:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_AUDIO_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_AUDIO_EXT, 64);
			hResult = ERR_OK;
			break;

		default:
			hResult = ERR_OK;
			break;
		}
		break;

	case eFS_UsageType_MediaPic:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_PHOTO_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_PHOTO_EXT, 64);
			hResult = ERR_OK;
			break;

		default:
			hResult = ERR_OK;
			break;
		}
		break;

	case eFS_UsageType_MediaDownload:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_DOWNLOAD_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_NORMAL_DOWNLOAD_EXT, 64);
			hResult = ERR_OK;
			break;

		default:
			hResult = ERR_OK;
			break;
		}

		if (pbShare)
		{
			*pbShare = TRUE;
		}
		break;

	default:
		break;
	}

	return hResult;
}
#elif defined(CONFIG_MW_MM_PVR_DIR_PROFILE_LEGACY)
STATIC HERROR umma_fs_svc_DirGetSubPath_LegacyProfile (UMMA_FS_UsageType_e eUsage, UMMA_FS_DevContext_t *pstDevice, HUINT8 *szSubPath)
{
	UMMA_FS_DevType_e	 eDevType = pstDevice->eDevType;
	HERROR			 hResult = ERR_FAIL;

	szSubPath[0] = '\0';

	switch (eUsage)
	{
	case eFS_UsageType_Record:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_LEGACY_RECORD_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_LEGACY_RECORD_EXT, 64);
			hResult = ERR_OK;
			break;

		default:
			break;
		}
		break;

	case eFS_UsageType_Tsr:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_LEGACY_TSR_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_LEGACY_TSR_EXT, 64);
			hResult = ERR_OK;
			break;

		default:
			break;
		}
		break;

	case eFS_UsageType_MediaPath:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_LEGACY_MEDIA_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
		default:
			hResult = ERR_OK;
			break;
		}
		break;

	case eFS_UsageType_MediaVideo:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_LEGACY_VIDEO_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
		default:
			hResult = ERR_OK;
			break;
		}
		break;

	case eFS_UsageType_MediaAudio:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_LEGACY_AUDIO_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
		default:
			hResult = ERR_OK;
			break;
		}
		break;

	case eFS_UsageType_MediaPic:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_LEGACY_PHOTO_INT, 64);
			hResult = ERR_OK;
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
		default:
			hResult = ERR_OK;
			break;
		}
		break;

	case eFS_UsageType_MediaDownload:
		hResult = ERR_OK;
		break;

	default:
		break;
	}

	return hResult;
}

#elif defined(CONFIG_MW_MM_PVR_DIR_PROFILE_MEDIA_IN_VIDEO)
STATIC HERROR umma_fs_svc_DirGetSubPath_MediaInVideoProfile (UMMA_FS_UsageType_e eUsage, UMMA_FS_DevContext_t *pstDevice, HUINT8 *szSubPath)
{
	UMMA_FS_DevType_e	 eDevType = pstDevice->eDevType;
	szSubPath[0] = '\0';

	switch (eUsage)
	{
	case eFS_UsageType_Record:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_MIV_RECORD_INT, 16);
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_MIV_RECORD_EXT, 16);
			break;

		default:
			break;
		}
		break;

	case eFS_UsageType_Tsr:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_MIV_TSR_INT, 16);
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
		case eFS_DevType_MMC:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_MIV_TSR_EXT, 16);
			break;

		default:
			break;
		}
		break;

	case eFS_UsageType_MediaPath:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_MIV_MEDIA_INT, 16);
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
		default:
			break;
		}
		break;

	case eFS_UsageType_MediaVideo:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_MIV_VIDEO_INT, 16);
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
		default:
			break;
		}
		break;

	case eFS_UsageType_MediaAudio:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_MIV_AUDIO_INT, 16);
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
		default:
			break;
		}
		break;

	case eFS_UsageType_MediaPic:
		switch (eDevType)
		{
		case eFS_DevType_IntHdd:
			HxSTD_StrNCpy (szSubPath, PVR_PATH_MIV_PHOTO_INT, 16);
			break;

		case eFS_DevType_UsbHdd:
		case eFS_DevType_Detachable:
		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbMem:
		default:
			break;
		}
		break;

	default:
		break;
	}

	return ERR_OK;
}
#endif

#define ________FS_INTERNAL_FUNCTIONS________

#define _____PARTITION_SETTING_____

HERROR umma_fs_svc_DirGetHddPartitionSetting (UMMA_FS_DevIdx_e eDevIdx, HUINT8 *szVolumeName, UMMA_FS_FormatPartList_t *pstFormatList)
{
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR		hErr;

	if (pstFormatList == NULL)				{ return ERR_FAIL; }

	VK_memset (pstFormatList, 0, sizeof(UMMA_FS_FormatPartList_t));

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	switch (pstDevice->eDevType)
	{
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	case eFS_DevType_IntHdd:
#if defined(CONFIG_MW_MM_PVR_PARTITION_PROFILE_TVAPPS)
		hErr = umma_fs_svc_DirGetIntPvrHddPartSetting_TvAppsProfile (eDevIdx, szVolumeName, pstFormatList);
#elif defined(CONFIG_MW_MM_PVR_PARTITION_PROFILE_NORMAL)
		hErr = umma_fs_svc_DirGetIntPvrHddPartSetting_BasicProfile (eDevIdx, szVolumeName, pstFormatList);
#endif
		break;
#endif	// CONFIG_PROD_DEVICE_STORAGE_HDD

	case eFS_DevType_Detachable:
		hErr = umma_fs_svc_DirGetIntPvrHddPartSetting_2ndBasicProfile (eDevIdx, szVolumeName, pstFormatList);
		break;

#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
	case eFS_DevType_UsbHdd:
	case eFS_DevType_eSataHdd:
	case eFS_DevType_UsbMem:
#if defined(CONFIG_MW_MM_PVR_PARTITION_PROFILE_TVAPPS)
		hErr = umma_fs_svc_DirGetExtPvrHddPartSetting_TvAppsProfile (eDevIdx, szVolumeName, pstFormatList);
#elif defined(CONFIG_MW_MM_PVR_PARTITION_PROFILE_NORMAL)
		hErr = umma_fs_svc_DirGetExtPvrHddPartSetting_BasicProfile (eDevIdx, szVolumeName, pstFormatList);
#endif
		break;
#endif	// CONFIG_PROD_DEVICE_STORAGE_USB

	default:
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	return hErr;
}

#define _____DIRECTORY_SETTING_____

HERROR umma_fs_svc_DirGetPathFromMountedRootPath (HUINT8 *szMountedRoot, UMMA_FS_DevType_e eDevType, UMMA_FS_UsageType_e eUsage, HUINT8 *szPath, HUINT32 ulPathMax)
{
	HBOOL			 bFound = FALSE;
	HUINT32			 ulCount, ulDeviceNum = 0;
	HCHAR			*szRootPath = NULL, *szDevicePath = NULL, *szSubPath = NULL;
	UMMA_FS_DevIdx_e	*peIdxArray = NULL;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR			 hErr, hResult = ERR_FAIL;

	if (szMountedRoot == NULL)					{ return ERR_FAIL; }

	szRootPath		= (HCHAR *)HLIB_STD_MemCalloc (HxPATH_MAX + 1);
	szDevicePath	= (HCHAR *)HLIB_STD_MemCalloc (HxPATH_MAX + 1);
	szSubPath		= (HCHAR *)HLIB_STD_MemCalloc (HxPATH_MAX + 1);

	if ((NULL == szRootPath) || (NULL == szDevicePath) || (NULL == szSubPath))
	{
		HxLOG_Error ("Memory allocation failed:\n");
		goto END_FUNC;
	}

	HxSTD_StrNCpy (szRootPath, szMountedRoot, HxPATH_MAX);
	HLIB_PATH_Normalize (szRootPath);

	ulDeviceNum = 0;	peIdxArray = NULL;
	umma_fs_dev_GetDeviceIndexArray (&ulDeviceNum, &peIdxArray);
	if ((0 == ulDeviceNum) || (NULL == peIdxArray))
	{
		HxLOG_Error ("No device found...\n");
		goto END_FUNC;
	}

	for (ulCount = 0; (ulCount < ulDeviceNum) && (FALSE == bFound); ulCount++)
	{
		pstDevice = NULL;

		hErr = umma_fs_dev_GetDeviceInfo (peIdxArray[ulCount], &pstDevice);
		if ((ERR_OK == hErr) && (NULL != pstDevice))
		{
			HUINT32			 ulPartCnt;

			for (ulPartCnt = 0; ulPartCnt < pstDevice->ulNumPartition; ulPartCnt++)
			{
				HxSTD_StrNCpy (szDevicePath, pstDevice->pstPartArray[ulPartCnt].szMountPath, HxPATH_MAX);
				HLIB_PATH_Normalize (szDevicePath);

				if (HxSTD_StrNCmp (szRootPath, szDevicePath, HxPATH_MAX) == 0)
				{
					bFound = TRUE;
					break;
				}
			}
		}
	}

	if ((TRUE != bFound) || (NULL == pstDevice))
	{
		HxLOG_Critical ("No matching path!!! (%s)\n", szRootPath);
		goto END_FUNC;
	}

	/*
		나중에 sub path 만드는 방식이 달라지는 모델이 나오면 여기를 profile화 하도록 한다. 현재는 basic profile.
	*/
#if defined(CONFIG_MW_MM_PVR_DIR_PROFILE_BASE)
	hErr = umma_fs_svc_DirGetSubPath_BaseProfile (eUsage, pstDevice, szSubPath, NULL);
#elif defined(CONFIG_MW_MM_PVR_DIR_PROFILE_LEGACY)
	hErr = umma_fs_svc_DirGetSubPath_LegacyProfile (eUsage, pstDevice, szSubPath);
#elif defined(CONFIG_MW_MM_PVR_DIR_PROFILE_MEDIA_IN_VIDEO)
	hErr = umma_fs_svc_DirGetSubPath_MediaInVideoProfile (eUsage, pstDevice, szSubPath);
#else
	hErr = ERR_FAIL;
#endif

	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\nCritical Error!!!(eUsage : %d, szSubPath : %s)\n\n", eUsage, szSubPath);
		goto END_FUNC;
	}

	HLIB_PATH_Join (szRootPath, szSubPath, szPath, ulPathMax);
	HxSTD_StrNCat (szPath, "/", ulPathMax);
	szPath[ulPathMax - 1] = '\0';

	hResult = ERR_OK;
END_FUNC:
	if (NULL != szRootPath)					{ HLIB_STD_MemFree (szRootPath); }
	if (NULL != szDevicePath)				{ HLIB_STD_MemFree (szDevicePath); }
	if (NULL != szSubPath)					{ HLIB_STD_MemFree (szSubPath); }
	if (NULL != peIdxArray)					{ umma_fs_dev_FreeDeviceIndexArray (peIdxArray); }

	return hResult;
}

#if defined(CONFIG_MW_MM_PVR)
HERROR umma_fs_svc_DirGetRecordDirName (HCHAR *pszDir, HUINT32 ulSize, HBOOL bInternal)
{
	HERROR	hErr = ERR_OK;

	if(NULL == pszDir)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(pszDir, 0, ulSize);

	if(bInternal)
	{
#if defined(CONFIG_MW_MM_PVR_DIR_PROFILE_BASE)
		HxSTD_snprintf(pszDir, ulSize, "%s",PVR_PATH_NORMAL_RECORD_INT);
#elif defined(CONFIG_MW_MM_PVR_DIR_PROFILE_LEGACY)
		HxSTD_snprintf(pszDir, ulSize, "%s",PVR_PATH_LEGACY_RECORD_INT);
#elif defined(CONFIG_MW_MM_PVR_DIR_PROFILE_MEDIA_IN_VIDEO)
		HxSTD_snprintf(pszDir, ulSize, "%s",PVR_PATH_MIV_RECORD_INT);
#else
		hErr = ERR_FAIL;
#endif
	}
	else
	{
#if defined(CONFIG_MW_MM_PVR_DIR_PROFILE_BASE)
		HxSTD_snprintf(pszDir, ulSize, "%s",PVR_PATH_NORMAL_RECORD_EXT);
#elif defined(CONFIG_MW_MM_PVR_DIR_PROFILE_LEGACY)
		HxSTD_snprintf(pszDir, ulSize, "%s",PVR_PATH_LEGACY_RECORD_EXT);
#elif defined(CONFIG_MW_MM_PVR_DIR_PROFILE_MEDIA_IN_VIDEO)
		HxSTD_snprintf(pszDir, ulSize, "%s",PVR_PATH_MIV_RECORD_EXT);
#else
		hErr = ERR_FAIL;
#endif
	}

	return hErr;
}


HERROR umma_fs_svc_DirMakePlaybackDirectory (UMMA_FS_DevIdx_e	eDevIdx, HUINT32 ulPartIdx)
{
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	HERROR				 hErr;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	umma_fs_svc_DirMakeDirectoryByUsage(eFS_UsageType_Record, pstDevice,ulPartIdx);

	return ERR_OK;
}

HERROR umma_fs_svc_DirMakeDefaultDirectory (UMMA_FS_DevIdx_e	eDevIdx, HUINT32 ulPartIdx)
{
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	UMMA_FS_UsageType_e	 eDirUsageType;

	HERROR				 hErr;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	for (eDirUsageType = eFS_UsageType_None; eDirUsageType < eFS_UsageType_END; eDirUsageType++)
	{
		umma_fs_svc_DirMakeDirectoryByUsage(eDirUsageType, pstDevice,ulPartIdx);
	}

	return ERR_OK;
}

void umma_fs_svc_DirMakeDirectoryByUsage (UMMA_FS_UsageType_e eDirUsageType, UMMA_FS_DevContext_t *pstDevice, HUINT32 ulPartIdx)
{
	HBOOL				 bFound;
	HERROR hErr;
	HUINT8				 szPath[92 + 1], szSubPath[64];

	HBOOL				bShare = FALSE;
#if defined(CONFIG_MW_MM_PVR_DIR_PROFILE_BASE)
	hErr = umma_fs_svc_DirGetSubPath_BaseProfile (eDirUsageType, pstDevice, szSubPath, &bShare);
#elif defined(CONFIG_MW_MM_PVR_DIR_PROFILE_LEGACY)
	hErr = umma_fs_svc_DirGetSubPath_LegacyProfile (eDirUsageType, pstDevice, szSubPath);
#elif defined(CONFIG_MW_MM_PVR_DIR_PROFILE_MEDIA_IN_VIDEO)
	hErr = umma_fs_svc_DirGetSubPath_MediaInVideoProfile (eDirUsageType, pstDevice, szSubPath);
#else
	hErr = ERR_FAIL;
#endif
	if (hErr == ERR_OK)
	{
		HLIB_PATH_Join (pstDevice->pstPartArray[ulPartIdx].szMountPath, szSubPath, szPath, 92);
		HxSTD_StrNCat (szPath, "/", 92);

		bFound = HLIB_DIR_IsExist (szPath);
		if (bFound == FALSE)
		{
			hErr = HLIB_DIR_Make(szPath);
			if (hErr != ERR_OK)
			{
				HxLOG_Error ("HLIB_DIR_Make (%s) err (0x%08x)\n", szPath, hErr);
			}

			// Share folder 권한 재설정
			if (bShare == TRUE)
			{
#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
				if (HLIB_EXTRA_ChownWithParam(szPath, UID_umma, GID_pvr) != ERR_OK)
				{
					HxLOG_Error("HLIB_EXTRA_ChownWithParam (%s) err (0x%08x)\n", szPath, hErr);
				}
#endif
				if (HLIB_DIR_Chmod(szPath, 0777) != ERR_OK)
				{
					HxLOG_Error ("HLIB_DIR_Chmod (%s) err (0x%08x)\n", szPath, hErr);
				}
			}
#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
			else
			{
				if (HLIB_EXTRA_ChownWithParam(szPath, UID_umma, GID_pvr) != ERR_OK)
				{
					HxLOG_Error("HLIB_EXTRA_ChownWithParam (%s) err (0x%08x)\n", szPath, hErr);
				}

				if (HLIB_DIR_Chmod(szPath, 0770) != ERR_OK)
				{
					HxLOG_Error ("HLIB_DIR_Chmod (%s) err (0x%08x)\n", szPath, hErr);
				}
			}
#endif
		}
		else
		{
			HxLOG_Print("[%s:%d] Dir (%s) already exists\n", __FUNCTION__, __LINE__, szPath);
		}
	}
}
#endif



