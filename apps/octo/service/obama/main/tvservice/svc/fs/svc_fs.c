/**
	@file     mw_fs.c
	@brief    repository service	\n
			  This implements rec/pb & media(photo/music) services.	\n

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

#include <hapi.h>
#include <vkernel.h>
#include <isosvc.h>

#include <db_common.h>
//#include <mwc_util.h>

#include <svc_fs.h>
#include <dlib.h>
#include <uapi.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#undef	COMMAND_NAME
#define	COMMAND_NAME			OCTO_SHELL_NAME

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define ___LOCAL_FUNCTION___

SvcFs_DevType_e svc_fs_convertDevTypeDx2SvcFs(DxStorage_DevType_e eDxType)
{
	SvcFs_DevType_e eSvcType;

	switch (eDxType)
	{
		case eDxSTORAGE_DEVTYPE_INTHDD:		eSvcType = eSVC_FS_DevType_IntHdd;		break;
		case eDxSTORAGE_DEVTYPE_DETACHABLE:	eSvcType = eSVC_FS_DevType_Detachable;		break;
		case eDxSTORAGE_DEVTYPE_ESATAHDD:	eSvcType = eSVC_FS_DevType_eSataHdd;		break;
		case eDxSTORAGE_DEVTYPE_USBHDD:		eSvcType = eSVC_FS_DevType_UsbHdd;		break;
		case eDxSTORAGE_DEVTYPE_USBMEM:		eSvcType = eSVC_FS_DevType_UsbMem;		break;
		case eDxSTORAGE_DEVTYPE_MMC:			eSvcType = eSVC_FS_DevType_MMC;		break;
		case eDxSTORAGE_DEVTYPE_ALL:			eSvcType = eSVC_FS_DevType_ALL;		break;

		default :								eSvcType = eSVC_FS_DevType_None;		break;
	}

	return eSvcType;
}

HERROR svc_fs_ConvertDevDxInfo2SvcFs (DxStorage_Info_t *pstDxDevInfo, SvcFs_DevInfo_t *pstSvcDevInfo)
{
	if (pstDxDevInfo == NULL || pstSvcDevInfo == NULL)
	{
		HxLOG_Error("Error \n");
		return ERR_FAIL;
	}

	HxSTD_memset (pstSvcDevInfo, 0, sizeof(SvcFs_DevInfo_t));

	pstSvcDevInfo->eDevIdx			= pstDxDevInfo->ulDeviceId;
	pstSvcDevInfo->eDevType			= svc_fs_convertDevTypeDx2SvcFs(pstDxDevInfo->eDevType);
	pstSvcDevInfo->eDevStatus		= (pstDxDevInfo->ulFlags & eDxSTORAGE_FLAG_NEEDFORMAT) ? eDevStatus_NeedFormat : eDevStatus_OK;
	pstSvcDevInfo->eDevSubStatus 	= 0;	// unused
	pstSvcDevInfo->bConnected		= (pstDxDevInfo->ulFlags & eDxSTORAGE_FLAG_CONNECTED) ? TRUE : FALSE;
	pstSvcDevInfo->bMounted			=(pstDxDevInfo->ulFlags & eDxSTORAGE_FLAG_MOUNTED) ? TRUE : FALSE;
	pstSvcDevInfo->eFsType			= eFileSystem_NONE;	// unused
	pstSvcDevInfo->bWriteProtected	= (pstDxDevInfo->ulFlags & eDxSTORAGE_FLAG_WRITEPROTECTED) ? TRUE : FALSE;
	pstSvcDevInfo->bPvr		= ((pstDxDevInfo->ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) ||(pstDxDevInfo->ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE)) ? TRUE : FALSE;
	pstSvcDevInfo->ulPortNum	= pstDxDevInfo->ulPortNum;
	pstSvcDevInfo->ulMaxRecInstance = pstDxDevInfo->ulMaxRecInstance;


	HLIB_STD_StrNCpySafe(pstSvcDevInfo->szMountPath, pstDxDevInfo->szMountPath, SVC_FS_MOUNT_PATH_LEN);
	HLIB_STD_StrNCpySafe(pstSvcDevInfo->szLabel, pstDxDevInfo->szLabel, SVC_FS_VOLUME_LABEL_SIZE);

	pstSvcDevInfo->ulNumPartition	= (HUINT32)pstDxDevInfo->nPartitionNum;

	HxLOG_Print("[%s (%04d)] stDxInfo.nPartitionNum [%ld] >>>\n", __FUNCTION__, __LINE__, pstDxDevInfo->nPartitionNum);

	return ERR_OK;
}

HERROR svc_fs_ConvertPartDxInfo2SvcFs (DxStorage_PartitionInfo_t *pstDxPartInfo, SvcFs_PartitionInfo_t *pstSvcPartInfo)
{
	if (pstDxPartInfo == NULL || pstSvcPartInfo == NULL)
	{
		HxLOG_Error("Error \n");
		return ERR_FAIL;
	}
	HxSTD_memset (pstSvcPartInfo, 0, sizeof(SvcFs_PartitionInfo_t));

	pstSvcPartInfo->bAvailable = pstDxPartInfo->bAvailable;
	pstSvcPartInfo->bDefaultPvr = (pstDxPartInfo->ulFlags & eDxSTORAGE_FLAG_DEFAULTHDD) ? TRUE: FALSE;
	pstSvcPartInfo->bMounted =   (pstDxPartInfo->ulFlags & eDxSTORAGE_FLAG_MOUNTED) ? TRUE: FALSE;
	pstSvcPartInfo->bPvrFlag = (pstDxPartInfo->ulFlags & (eDxSTORAGE_FLAG_PVRDEVICE |eDxSTORAGE_FLAG_SUBPVRDEVICE)) ? TRUE: FALSE;
	pstSvcPartInfo->bSecured = (pstDxPartInfo->ulFlags & eDxSTORAGE_FLAG_SECURED) ? TRUE: FALSE;
	pstSvcPartInfo->bWritable = (pstDxPartInfo->ulFlags & eDxSTORAGE_FLAG_WRITEPROTECTED) ? TRUE: FALSE;
	pstSvcPartInfo->ePartFsType = pstDxPartInfo->eFsType;

	HLIB_STD_StrNCpySafe(pstSvcPartInfo->szMediaAudio, pstDxPartInfo->szMediaAudio, SVC_FS_MOUNT_PATH_LEN);
	HLIB_STD_StrNCpySafe(pstSvcPartInfo->szMediaPic, pstDxPartInfo->szMediaPic, SVC_FS_MOUNT_PATH_LEN);
	HLIB_STD_StrNCpySafe(pstSvcPartInfo->szMediaVideo, pstDxPartInfo->szMediaVideo, SVC_FS_MOUNT_PATH_LEN);
	HLIB_STD_StrNCpySafe(pstSvcPartInfo->szMountPath, pstDxPartInfo->szMountPath, SVC_FS_MOUNT_PATH_LEN);
	HLIB_STD_StrNCpySafe(pstSvcPartInfo->szRecordPath, pstDxPartInfo->szRecordPath, SVC_FS_MOUNT_PATH_LEN);
	HLIB_STD_StrNCpySafe(pstSvcPartInfo->szTsrPath, pstDxPartInfo->szTsrPath, SVC_FS_MOUNT_PATH_LEN);
	HLIB_STD_StrNCpySafe(pstSvcPartInfo->szVolumeName, pstDxPartInfo->szVolumeName, SVC_FS_VOLUME_LABEL_SIZE);

	pstSvcPartInfo->ulDriveNo = 0;	// unused
	pstSvcPartInfo->ulEndUnit = 0;	// unused
	pstSvcPartInfo->ullTotalSizeKb = 0; // unused
	pstSvcPartInfo->ulPvrIdx = (HUINT32)pstDxPartInfo->nPvrIdx;
	pstSvcPartInfo->ulPlaybackIdx = (HUINT32)pstDxPartInfo->nPlaybackIdx;

	pstSvcPartInfo->ulStartUnit = 0;// unused

	return ERR_OK;
}

#define ___GLOBAL_API____


HERROR SVC_FS_Init (void)
{
	UAPI_FS_Init();

	return ERR_OK;
}

HERROR SVC_FS_ConvertDevDxInfo2SvcFs  (DxStorage_Info_t *pstDxDevInfo, SvcFs_DevInfo_t *pstSvcDevInfo)
{
	return svc_fs_ConvertDevDxInfo2SvcFs (pstDxDevInfo, pstSvcDevInfo);
}

HERROR SVC_FS_GetDeviceInfo (SvcFs_DevIdx_e eDevIdx, SvcFs_DevInfo_t *pstMwDevInfo)
{
	HERROR	hErr = ERR_OK;
	DxStorage_Info_t 	stDxInfo;

	hErr = UAPI_FS_GetInfo(eDevIdx, &stDxInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Fail to getting storage info... (idx : %d) \n", eDevIdx);
		return ERR_FAIL;
	}

	HxSTD_memset (pstMwDevInfo, 0, sizeof(SvcFs_DevInfo_t));

	hErr = svc_fs_ConvertDevDxInfo2SvcFs(&stDxInfo, pstMwDevInfo);

	return hErr;
}

HERROR SVC_FS_GetPartitionInfoArray (SvcFs_DevIdx_e eDevIdx, HUINT32 *pulPartNum, SvcFs_PartitionInfo_t **ppstPartArray)
{
	HERROR	hErr = ERR_OK;
	DxStorage_Info_t	stDxInfo;
	SvcFs_PartitionInfo_t *pstArray, *pCur, stPartInfo;
	HUINT32		ulIdx;

	if (ppstPartArray == NULL)
	{
		HxLOG_Error("Error !!!! \n");
		return ERR_FAIL;
	}

	hErr = UAPI_FS_GetInfo(eDevIdx, &stDxInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Fail to getting storage info... (idx : %d) \n", eDevIdx);
		return ERR_FAIL;
	}

	if (stDxInfo.nPartitionNum == 0)
	{
		HxLOG_Error("Partition num = 0 .. \n");
		return ERR_FAIL;
	}

	pstArray = (SvcFs_PartitionInfo_t *)OxFS_Malloc (sizeof(SvcFs_PartitionInfo_t) * stDxInfo.nPartitionNum);
	if (pstArray == NULL)
	{
		HxLOG_Error("Malloc fail!!!! \n");
		return ERR_FAIL;
	}
	pCur = pstArray;

	for (ulIdx = 0 ; ulIdx < stDxInfo.nPartitionNum ; ulIdx++)
	{
		hErr = svc_fs_ConvertPartDxInfo2SvcFs ( &stDxInfo.astPartitionInfos[ulIdx], &stPartInfo);
		if (ERR_OK == hErr)
		{
			HxSTD_MemCopy(pCur++, &stPartInfo, sizeof(SvcFs_PartitionInfo_t));
		}
	}

	if (pulPartNum != NULL)
	{
		*pulPartNum = stDxInfo.nPartitionNum;
	}

	if (ppstPartArray != NULL)
	{
		*ppstPartArray = pstArray;
	}

	return ERR_OK;
}

HERROR SVC_FS_FreePartitionInfoArray (SvcFs_PartitionInfo_t *pstPartArray)
{
	if (pstPartArray == NULL)						{ return ERR_FAIL; }

	OxFS_Free (pstPartArray);
	return ERR_OK;
}

HBOOL SVC_FS_CheckPathIsValid (SvcFs_DevIdx_e eDevIdx, HUINT8 *szPath)
{
	 //용도에 따라서 다르지만, 제거될 시점에서는 리스트를 가져올수가 없다.
	HERROR	hErr = ERR_OK;
	DxStorage_Info_t 	stDxInfo;

	hErr = UAPI_FS_GetInfo(eDevIdx, &stDxInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Fail to getting storage info... (idx : %d) \n", eDevIdx);
		return FALSE;
	}

	return HLIB_STD_StrStartWith(szPath, stDxInfo.szMountPath);
}

/*
	Partition File System 과 남은 공간 등으로 PVR 동작을 할 문제가 없음을 체크하는 함수.
	주의 : 실제로 PVR용 HDD로 사용되는 HDD가 아니라 PVR용 HDD로 쓸 요건이 되는 device를 얻어내는 함수이다.
	물론 현재 PVR HDD로 쓰고 있는 device도 해당되나, 현재 PVR HDD가 아니어도 이 함수에서 결과로 알려준다.
*/
HBOOL SVC_FS_IsDevicePvrPossible (SvcFs_DevIdx_e eDevIdx)
{
	 //용도에 따라서 다르지만, 제거될 시점에서는 리스트를 가져올수가 없다.
	HERROR	hErr = ERR_OK;
	DxStorage_Info_t	stDxInfo;
	HUINT32		ulIdx;
	HUINT64		ullAvailablePvrKbSize;

	hErr = UAPI_FS_GetInfo(eDevIdx, &stDxInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Fail to getting storage info... (idx : %d) \n", eDevIdx);
		return FALSE;
	}

	if (stDxInfo.ulFlags & eDxSTORAGE_FLAG_PVRDEVICE || stDxInfo.ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE)
	{
		for (ulIdx = 0 ; ulIdx < stDxInfo.nPartitionNum ; ulIdx++)
		{
			if (stDxInfo.astPartitionInfos[ulIdx].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE || stDxInfo.astPartitionInfos[ulIdx].ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE)
			{
				hErr = UAPI_FS_GetSize(eDevIdx, NULL, NULL, NULL, NULL, &ullAvailablePvrKbSize);
				if (hErr == ERR_OK && ullAvailablePvrKbSize > DxSVC_FS_PVR_SAFE_AREA_SIZE_KB)
				{
					HxLOG_Print("[SVC_FS_IsDevicePvrPossible] Dev (%d) avialble for PVR (ullAvailablePvrKbSize : %lld)\n", eDevIdx, ullAvailablePvrKbSize);
					return TRUE;
				}
				break;
			}
		}
	}
	else if ( stDxInfo.ulFlags & eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE )
	{
		HxLOG_Error("recording pvr type is for recording list only \n");
	}

	return FALSE;
}

/*
	PVR 기능이 동작하는 Storage Device Index 와 Partition Index를 얻어오는 함수
	Storage Device Index만 알고 싶다던가 Partition Index만 얻고 싶다던가 등의 경우엔 알아내고 싶지 않은 곳을 NULL로 하면 된다.
*/
HERROR SVC_FS_GetPvrDevIdPartIdx (HUINT32 ulPvrIdx, SvcFs_DevIdx_e *peDevIdx, HUINT32 *pulPartIdx)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i,j;
	HBOOL	bFind = FALSE;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_PVR, &pstInfoList, &ulDevNum);
	HxLOG_Print("[%s] [%d] Storage num : %d\n",__FUNCTION__,__LINE__, ulDevNum);

	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		for(j = 0 ; j < pstInfo->nPartitionNum ; j++)
		{
			if ( ((pstInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE)
				|| (pstInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE))
			  && (pstInfo->astPartitionInfos[j].nPvrIdx == ulPvrIdx))
			{
				if (peDevIdx)		*peDevIdx = (SvcFs_DevIdx_e)pstInfo->ulDeviceId;
				if (pulPartIdx)		*pulPartIdx = j;

				bFind = TRUE;
				break;
			}
			else if (pstInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE)
			{
				HxLOG_Error("recording pvr type is for recording list only \n");
			}
		}

		if (bFind)		break;
	}

	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	if (bFind)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

HBOOL	SVC_FS_IsUsbConnected(void)
{
	DxStorage_Info_t *pstInfoList = NULL, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i;
	HBOOL	bFind = FALSE;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);

	if(NULL == pstInfoList)
	{
		HxLOG_Error("pstInfoList is NULL\n");
		return FALSE;
	}

	HxLOG_Print("Storage num : %d\n", ulDevNum);

	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		if (pstInfo->eDevType == eDxSTORAGE_DEVTYPE_USBHDD || pstInfo->eDevType == eDxSTORAGE_DEVTYPE_USBMEM)
		{
			bFind = TRUE;
			break;
		}
	}
	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	return bFind;
}

HERROR SVC_FS_GetPvrPath (HUINT32 ulPvrIdx, SvcFs_UsageType_e eUsage, HCHAR *pszPath)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i,j;
	HBOOL	bFind = FALSE;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_PVR, &pstInfoList, &ulDevNum);
	HxLOG_Print("[%s] [%d] Storage num : %d\n",__FUNCTION__,__LINE__, ulDevNum);

	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		for(j = 0 ; j < pstInfo->nPartitionNum ; j++)
		{
			if (( (pstInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE)
				|| (pstInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE))
			  && (pstInfo->astPartitionInfos[j].nPvrIdx == (HINT32)ulPvrIdx))
			{

				switch (eUsage)
				{
					case eSVC_FS_UsageType_Root:
						HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[j].szMountPath, DxSTORAGE_NAME_LEN);
						break;

					case eSVC_FS_UsageType_Record:
						HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[j].szRecordPath, DxSTORAGE_NAME_LEN);
						break;

					case eSVC_FS_UsageType_Tsr:
						HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[j].szTsrPath, DxSTORAGE_NAME_LEN);
						break;

					case eSVC_FS_UsageType_MediaPath:
						HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[j].szMediaPath, DxSTORAGE_NAME_LEN);
						break;

					case eSVC_FS_UsageType_MediaVideo:
						HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[j].szMediaVideo, DxSTORAGE_NAME_LEN);
						break;

					case eSVC_FS_UsageType_MediaAudio:
						HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[j].szMediaAudio, DxSTORAGE_NAME_LEN);
						break;

					case eSVC_FS_UsageType_MediaPic:
						HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[j].szMediaPic, DxSTORAGE_NAME_LEN);
						break;

					case eSVC_FS_UsageType_MediaDownload:
						HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[j].szMediaDownload, DxSTORAGE_NAME_LEN);
						break;

					default:
						break;
				}

				bFind = TRUE;
				break;
			}
		}

		if (bFind)		break;
	}

	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	if (bFind)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR SVC_FS_GetPlaybackPath (HUINT32 ulPlaybackIdx, SvcFs_UsageType_e eUsage, HCHAR *pszPath)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i,j;
	HBOOL	bFind = FALSE;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_Playback, &pstInfoList, &ulDevNum);
	HxLOG_Print("[%s] [%d] Storage num : %d\n",__FUNCTION__,__LINE__, ulDevNum);

	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		for(j = 0 ; j < pstInfo->nPartitionNum ; j++)
		{
			if ( (pstInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE)
				&& (pstInfo->astPartitionInfos[j].nPlaybackIdx == (HINT32)ulPlaybackIdx))
			{
				switch (eUsage)
				{
					case eSVC_FS_UsageType_Record:
						HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[j].szRecordPath, DxSTORAGE_NAME_LEN);
						bFind = TRUE;
						break;
					default:
						HxLOG_Error("Recording pvr type is for recording list only. \n");
						break;
				}
			}
		}

		if (bFind)		break;
	}

	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	if (bFind)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

/*
	현재 PVR용으로 사용되는지 확인하는 함수
*/
HBOOL SVC_FS_IsDevicePvrUsed (SvcFs_DevIdx_e eDevIdx)
{
	HERROR	hErr = ERR_OK;
	HUINT32	ulIdx;
	DxStorage_Info_t 	stDxInfo;
	HBOOL	bFind = FALSE;

	hErr = UAPI_FS_GetInfo(eDevIdx, &stDxInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Fail to getting storage info... (idx : %d) \n", eDevIdx);
		return FALSE;
	}

	for(ulIdx= 0 ; ulIdx < stDxInfo.nPartitionNum ; ulIdx++)
	{
		if ( (stDxInfo.astPartitionInfos[ulIdx].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) ||  (stDxInfo.astPartitionInfos[ulIdx].ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE))
		{
			bFind = TRUE;
			break;
		}
	}

	return bFind;
}

/*
	현재 Recordign Storage용으로 사용되는지 확인하는 함수
*/
HBOOL SVC_FS_IsDeviceRecStorageUsed (SvcFs_DevIdx_e eDevIdx)
{
	HERROR	hErr = ERR_OK;
	HUINT32	ulIdx;
	DxStorage_Info_t 	stDxInfo;
	HBOOL	bFind = FALSE;

	hErr = UAPI_FS_GetInfo(eDevIdx, &stDxInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Fail to getting storage info... (idx : %d) \n", eDevIdx);
		return FALSE;
	}

	for(ulIdx= 0 ; ulIdx < stDxInfo.nPartitionNum ; ulIdx++)
	{
		if (stDxInfo.astPartitionInfos[ulIdx].ulFlags & eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE)
		{
			bFind = TRUE;
			break;
		}
	}

	return bFind;
}

HERROR SVC_FS_GetPvrIdxByDevIdx(HUINT32 *pulPvrIdx, SvcFs_DevIdx_e eDevIdx)
{
	HERROR	hErr = ERR_OK;
	HUINT32 ulIdx;
	DxStorage_Info_t	stDxInfo;
	HBOOL	bFind = FALSE;

	hErr = UAPI_FS_GetInfo(eDevIdx, &stDxInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Fail to getting storage info... (idx : %d) \n", eDevIdx);
		return ERR_FAIL;
	}

	for(ulIdx= 0 ; ulIdx < stDxInfo.nPartitionNum ; ulIdx++)
	{
		if ( (stDxInfo.astPartitionInfos[ulIdx].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE)
			|| (stDxInfo.astPartitionInfos[ulIdx].ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE))
		{
			if (pulPvrIdx)		*pulPvrIdx = (HUINT32)stDxInfo.astPartitionInfos[ulIdx].nPvrIdx;

			bFind = TRUE;
			break;
		}
		else if (stDxInfo.astPartitionInfos[ulIdx].ulFlags & eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE)
		{
			HxLOG_Error("Recording pvr type is for recording list only. \n");
		}

	}

	if (bFind)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR	SVC_FS_GetPvrStorageNum(HUINT32 *pulCount)
{
	HUINT32 ulDevNum = 0;
	HERROR hErr;

	hErr = UAPI_FS_GetList(eDxSTORAGE_FILTER_PVR, NULL, &ulDevNum);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error \n");
		return ERR_FAIL;
	}

	if (pulCount) *pulCount = ulDevNum;

	UAPI_FS_FreeList(NULL, ulDevNum);

	return ERR_OK;
}

HERROR	SVC_FS_GetUuidByPath(HUINT8 *pszPath, HUINT8 *pszUuid)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i;
	HBOOL	bFind = FALSE;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);
	HxLOG_Print("[%s] [%d] Storage num : %d\n",__FUNCTION__,__LINE__, ulDevNum);

	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		if (HLIB_STD_StrStartWith(pszPath, pstInfo->szMountPath) == TRUE)
		{
			HLIB_STD_StrNCpySafe(pszUuid,pstInfo->szUUID, DxSTORAGE_NAME_LEN);
			bFind = TRUE;
			break;
		}
	}

	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	if (bFind)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

HUINT32 SVC_FS_GetDefaultPvrIdx(void)
{
	DxStorage_Info_t *pstInfoList = NULL, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i,j, ulPvrIdx = 0xffff;
	HBOOL	bFind = FALSE;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_PVR, &pstInfoList, &ulDevNum);

	if(NULL == pstInfoList)
	{
		HxLOG_Error("pstInfoList is NULL\n");
		return ulPvrIdx;
	}

	HxLOG_Print("Storage num : %d\n", ulDevNum);

	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		for (j = 0 ; j < pstInfo->nPartitionNum ; j++)
		{
			if ( pstInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_DEFAULTHDD)
			{
				ulPvrIdx = (HUINT32)pstInfo->astPartitionInfos[j].nPvrIdx;
				bFind = TRUE;
				break;
			}
		}
		if (bFind == TRUE)		break;
	}

	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	return ulPvrIdx;
}

HERROR SVC_FS_GetDeviceUuidByPvrIdx (HUINT32 ulPvrIdx, HUINT8 *szUuid)
{
	// Check not used...
	return ERR_FAIL;
}

HERROR SVC_FS_GetDevPartIndexByPath (HUINT8 *szDrivePath, SvcFs_DevIdx_e *peDevIdx, HUINT32 *pulPartIdx)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i,j;
	HBOOL	bFind = FALSE;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);
	HxLOG_Print("[%s] [%d] Storage num : %d\n",__FUNCTION__,__LINE__, ulDevNum);

	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		for (j = 0 ; j < pstInfo->nPartitionNum ; j++)
		{
			if (HLIB_STD_StrStartWith(szDrivePath, pstInfo->astPartitionInfos[j].szMountPath) == TRUE)
			{
				if (peDevIdx) *peDevIdx = (SvcFs_DevIdx_e)pstInfo->ulDeviceId;
				if (pulPartIdx) *pulPartIdx = j;

				bFind = TRUE;
				break;
			}
		}
		if (bFind == TRUE)		break;
	}

	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	if (bFind)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}


SvcFs_DevStatus_e SVC_FS_GetPvrHddStatus (HUINT32 ulPvrIdx)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i,j;
	HBOOL	bFind = FALSE;
	SvcFs_DevStatus_e		eStatus = eDevStatus_NoDevice;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_PVR, &pstInfoList, &ulDevNum);
	HxLOG_Print("[%s] [%d] Storage num : %d\n",__FUNCTION__,__LINE__, ulDevNum);

	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		for (j = 0 ; j < pstInfo->nPartitionNum ; j++)
		{
			if ( (pstInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) && (ulPvrIdx == (HUINT32)pstInfo->astPartitionInfos[j].nPvrIdx))
			{
				if (pstInfo->ulFlags & eDxSTORAGE_FLAG_NEEDFORMAT)
				{
					eStatus = eDevStatus_NeedFormat;
				}
				else if (pstInfo->ulFlags & eDxSTORAGE_FLAG_MOUNTED)
				{
					eStatus = eDevStatus_OK;
				}
				else
				{
					eStatus = eDevStatus_Malfunction;
				}

				bFind = TRUE;
				break;
			}
		}
		if (bFind == TRUE)		break;
	}

	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	return eStatus;
}

HERROR	SVC_FS_GetSize(SvcFs_DevIdx_e eDevIdx, HUINT64 *pullTotalKbSize, HUINT64 *pullAvailableKbSize, HUINT64 *pullUsedKbSize, HUINT64 *pullReservedKbSize, HUINT64 *pullAvailablePvrKbSize)
{
	return UAPI_FS_GetSize(eDevIdx, pullTotalKbSize, pullAvailableKbSize, pullUsedKbSize, pullReservedKbSize, pullAvailablePvrKbSize);
}

HERROR	SVC_FS_GetPvrReservedSize(SvcFs_DevIdx_e eDevIdx, HUINT64 *pullReservedKbSize)
{
	return UAPI_FS_GetPvrReservedSize(eDevIdx, pullReservedKbSize);
}

HBOOL SVC_FS_CheckStoragePowerOff(void)
{
	return UAPI_FS_CheckStoragePowerOff();
}

HERROR SVC_FS_GetTsrOnlyPath (HCHAR *pszPath)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i;
	HBOOL	bFind = FALSE;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);
	HxLOG_Print("[%s] [%d] Storage num : %d\n",__FUNCTION__,__LINE__, ulDevNum);

	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		if( pstInfo->ulFlags & eDxSTORAGE_FLAG_TSRDEVICE )
		{
			HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[0].szTsrPath, DxSTORAGE_NAME_LEN);
			bFind = TRUE;
			break;
		}
	}

	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	HxLOG_Debug("[SVC_FS_GetTsrOnlyPath] find( %d ) pszPath = %s\n", bFind, pszPath);
	if (bFind)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

HBOOL SVC_FS_UseTsrOnlyDevice(void)
{
	return UAPI_FS_UseTsrOnlyDevice();
}

HUINT32 SVC_FS_GetTsrOnlyDeviceIdx(void)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i,ulTsrDeviceId = 0xffff;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_UsageAll, &pstInfoList, &ulDevNum);
	HxLOG_Print("[%s] [%d] Storage num : %d\n",__FUNCTION__,__LINE__, ulDevNum);

	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		if( pstInfo->ulFlags & eDxSTORAGE_FLAG_TSRDEVICE )
		{
			ulTsrDeviceId = pstInfo->ulDeviceId;
			break;
		}
	}
	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	HxLOG_Print("TSROnly Device Idx( %d )\n", ulTsrDeviceId);
	return ulTsrDeviceId;
}

/*********************** End of File ******************************/
