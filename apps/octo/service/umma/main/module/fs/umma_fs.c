/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_fs.c
	@brief

	Description:  									\n
	Module: UMMA File System				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding \uc744 \uc0ac\uc6a9\ud558\ub294 source file \uc785\ub2c8\ub2e4.
* MS949 encoding\u3092\u4f7f\u7528\u3057\u3066 source file\u3067\u3059\u3002
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <hlib.h>
#include <dlib.h>
#include <uapi.h>
#include <oapi.h>
#include <hapi.h>
#include <thapi.h>
#include <papi.h>
#include <vkernel.h>

#include <umma_int.h>
#include "umma_fs_int.h"
#include "se_uapi.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define UMMA_FS_OPEN_LIST_FUNCTION(name)  HLIB_STD_ListOpenFile_ByProcessName(name,pszMountPath,umma_fs_openfiles_callback,name);
#endif

#define		BLOCK_KB		8192
#define		FILESIZE_MB		8
#define		TEST_COUNT		5

#define		MEGA_BYTE	(1024*1024*1)
#define		KILO_BYTE	(1024)

#if defined(WIN32)
#define O_SYNC	0x01
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HxList_t		*s_pstStorageList = NULL;
static UMMA_t		*s_pstUmmaFs = NULL;
static HULONG		s_ulUmmaFsMsgQId;
static HUINT32		s_nReqId[eFS_Dev_MAX] = {0,};
static HBOOL	s_bJobRunning=FALSE;
static HBOOL	s_bDevPower=FALSE;
static HUINT32	s_unSignalSema = 0;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

static void		umma_fs_mainLoop(void *arg);
static	HERROR	umma_fs_do_Format (UMMA_FS_DevIdx_e eDevIdx, HINT32 nPartition, HCHAR *pszName);
static 	HERROR umma_fs_do_CheckUsbSpeed(HCHAR *pszPath, HUINT32 ulBlockKb, HUINT32 ulFileSizeMb, HUINT32 ulTestCount, HUINT32 *pulReadMbps, HUINT32 *pulWriteMbps);

#define ____LOCAL__________
static UMMA_t*	umma_fs_getInstance(void)
{
	if (s_pstUmmaFs == NULL)
	{
		HERROR hErr;
		unsigned long	tid;

		s_pstUmmaFs = (UMMA_t*)UMMA_Calloc(sizeof(UMMA_t));
		HxSEMT_Create(&s_pstUmmaFs->ulSemId, "ummafssem", 0);

		HxSEMT_Get(s_pstUmmaFs->ulSemId);
		hErr = UMMA_RpcProtocol_GetRpcHandle(&s_pstUmmaFs->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error!! (Get RPC Handle)\n");
			HxLOG_Assert(s_pstUmmaFs->nRpcHandle);
		}

		//	UMMA MainTask의 Wakeup을 위한 Event.
		HLIB_MSG_Create(128, sizeof(UMMA_EVT_t), "ummafsEvt", (unsigned long*)&s_pstUmmaFs->ulMsgQueue, eHLIB_SUSPENDTYPE_FIFO);

		s_ulUmmaFsMsgQId = s_pstUmmaFs->ulMsgQueue;

		VK_TASK_Create(umma_fs_mainLoop, 70, SIZE_64K, "ummafsMainLoop", (void*)s_pstUmmaFs, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);
#ifdef	DUMP_STACK
		s_tid = tid;
		HxLOG_Print("---> tid : [%p]\n", tid);
#endif

	}
	else
	{
		HxSEMT_Get(s_pstUmmaFs->ulSemId);
	}

	return s_pstUmmaFs;
}

static void		umma_fs_releaseInstance(UMMA_t *pstUmma)
{
	HxSEMT_Release(pstUmma->ulSemId);
}

static void	umma_fs_sendEvent(HINT32 nEvt, HINT32 nP1, HINT32 nP2, HINT32 nP3)
{
	UMMA_EVT_t	stEvt;
	HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));

	stEvt.nEvent = nEvt;
	stEvt.nParam1 = nP1;
	stEvt.nParam2 = nP2;
	stEvt.nParam3 = nP3;

	HLIB_MSG_Send(s_ulUmmaFsMsgQId, &stEvt, sizeof(UMMA_EVT_t));
}

static void	umma_fs_sendData (HINT32 nEvt, void *nP1, HINT32 nP1Size, void *nP2, HINT32 nP2Size, HINT32 nP3)
{
	UMMA_EVT_t	stEvt;
	void		*pBuf1 = NULL;
	void		*pBuf2 = NULL;
	HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));

	if (nP1Size > 0)
	{
		pBuf1 = (void *)UMMA_Calloc(nP1Size);
	}

	if (nP2Size >0)
	{
		pBuf2 = (void *)UMMA_Calloc(nP2Size);
	}

	if (pBuf1 == NULL || pBuf2 == NULL)
	{
		if (pBuf1 != NULL) {UMMA_Free(pBuf1);}
		if (pBuf2 != NULL) {UMMA_Free(pBuf2);}
		HxLOG_Error("Alloc fail! \n");
		return ;
	}

	stEvt.nEvent = nEvt;
	HLIB_STD_MemCpySafe(pBuf1, nP1Size, nP1, nP1Size);
	stEvt.nParam1 = (HINT32)pBuf1;
	HLIB_STD_MemCpySafe(pBuf2, nP2Size, nP2, nP2Size);
	stEvt.nParam2 = (HINT32)pBuf2;
	stEvt.nParam3 = nP3;

	HLIB_MSG_Send(s_ulUmmaFsMsgQId, &stEvt, sizeof(UMMA_EVT_t));
}

static void		umma_fs_freeInfo(void *pvData)
{
	UMMA_Free(pvData);
}

static DxStorage_Info_t*	umma_fs_GetStorageInfo(HUINT32 ulDeviceId)
{
	HxList_t	*list = s_pstStorageList;
	DxStorage_Info_t	*pstInfo;
	HINT32	nStorageNum = HLIB_LIST_Length(s_pstStorageList);

	if (nStorageNum <= 0)
	{
		return NULL;
	}

	while (list)
	{
		pstInfo = (DxStorage_Info_t*)HLIB_LIST_Data(list);
		if (pstInfo != NULL)
		{
			if (pstInfo->ulDeviceId == ulDeviceId)
			{
				return pstInfo;
			}
		}

		list = list->next;
	}
	return NULL;
}

static DxStorage_Info_t*	umma_fs_GetStorageInfoByPath(const HCHAR *pPath)
{
	HxList_t	        *list = s_pstStorageList;
	DxStorage_Info_t	*pstInfo = NULL;

	HINT32	nStorageNum = HLIB_LIST_Length(s_pstStorageList);

	if (nStorageNum <= 0)
	{
		return NULL;
	}

	while (list)
	{
		pstInfo = (DxStorage_Info_t*)HLIB_LIST_Data(list);
		if (pstInfo != NULL)
		{
			if (HLIB_STD_StrStartWith(pPath,pstInfo->szMountPath) ==TRUE)
			{
				return pstInfo;
			}
		}

		list = list->next;
	}
	return NULL;
}

static HERROR	umma_fs_RemoveStorageInfo(HUINT32 ulDeviceId)
{
	HxList_t	*list = s_pstStorageList;
	DxStorage_Info_t	*pstInfo;

	while (list)
	{
		pstInfo = (DxStorage_Info_t*)HLIB_LIST_Data(list);
		if (pstInfo != NULL)
		{
			if (pstInfo->ulDeviceId == ulDeviceId)
			{
				s_pstStorageList = HLIB_LIST_Remove(s_pstStorageList, pstInfo);
				umma_fs_freeInfo(pstInfo);
				break;
			}
		}
		list = list->next;
	}
	return ERR_OK;
}

static void		umma_fs_updateStorageList(void)
{
	HUINT32					i, j;
	HUINT32					ulNum, ulPartitionNum;
	UMMA_FS_PartitionInfo_t	*pstPartitions;
	DxStorage_Info_t		*pstStorageInfo;
	UMMA_FS_DevInfo_t		stDevInfo;
	UMMA_FS_DevIdx_e		*peDevIdxArray = NULL;
	HERROR					hErr;
	HUINT64 				ullResSizeKb;
	UMMA_FS_PvrType_e		ePvrType;
	s_pstStorageList = HLIB_LIST_RemoveAllFunc(s_pstStorageList, umma_fs_freeInfo);

	hErr = umma_fs_dev_GetDeviceIndexArray (&ulNum, &peDevIdxArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Warning("empty \n");
		return ;
	}

	HxLOG_Warning("Storage Num : %d\n", ulNum);

	for (i = 0; i < ulNum; i++)
	{
		pstStorageInfo = umma_fs_GetStorageInfo(peDevIdxArray[i]);
		if (pstStorageInfo == NULL)
		{
			pstStorageInfo = (DxStorage_Info_t*)UMMA_Calloc(sizeof(DxStorage_Info_t));
			s_pstStorageList = HLIB_LIST_Append(s_pstStorageList, (void*)pstStorageInfo);
		}

		hErr = umma_fs_svc_GetDeviceInfo(peDevIdxArray[i], &stDevInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("umma_fs_svc_GetDeviceInfo fail \n");
			continue;
		}

		ePvrType = umma_fs_config_GetPvrType(stDevInfo.eDevType);

		pstStorageInfo->ulDeviceId = (HUINT32)stDevInfo.eDevIdx;
		switch (stDevInfo.eDevType)
		{
			case eFS_DevType_IntHdd:		pstStorageInfo->eDevType = eDxSTORAGE_DEVTYPE_INTHDD;	break;
			case eFS_DevType_Detachable:	pstStorageInfo->eDevType = eDxSTORAGE_DEVTYPE_DETACHABLE;	break;
			case eFS_DevType_eSataHdd:	pstStorageInfo->eDevType = eDxSTORAGE_DEVTYPE_ESATAHDD;	break;
			case eFS_DevType_UsbHdd:	pstStorageInfo->eDevType = eDxSTORAGE_DEVTYPE_USBHDD;	break;
			case eFS_DevType_UsbMem:	pstStorageInfo->eDevType = eDxSTORAGE_DEVTYPE_USBMEM;	break;
			case eFS_DevType_MMC:		pstStorageInfo->eDevType = eDxSTORAGE_DEVTYPE_MMC;		break;
			case eFS_DevType_ALL:		pstStorageInfo->eDevType = eDxSTORAGE_DEVTYPE_ALL;		break;
			case eFS_DevType_None:
			default:
				pstStorageInfo->eDevType = eDxSTORAGE_DEVTYPE_NONE;
				break;
		}
		pstStorageInfo->ulFlags = 0;

		if (stDevInfo.bConnected)			{pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_CONNECTED;	}
		if (stDevInfo.bMounted)				{pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_MOUNTED;	}
		if (stDevInfo.bWriteProtected)		{pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_WRITEPROTECTED;	}
		if (stDevInfo.eDevStatus == eFS_DevStatus_NeedFormat)				{pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_NEEDFORMAT;	}
		if (pstStorageInfo->eDevType == eDxSTORAGE_DEVTYPE_INTHDD)	{ pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_INTERNAL;}	// ESATA HDD는 Internal이 아니다.

		if (pstStorageInfo->eDevType == eDxSTORAGE_DEVTYPE_DETACHABLE)
		{
			if (umma_fs_hdd_CheckIsNewDetachableHDD())		{pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_NEW2NDHDD;	}
		}
		if (ePvrType & eFS_PVR_RecStorage) {pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE;}


		if (stDevInfo.eDevType == eFS_DevType_UsbHdd)
		{
			switch (stDevInfo.eDevUsbSpeed)
			{
				case eFS_DevUsbSpeed_5G:
					pstStorageInfo->ullDeviceWriteSpeedBps = 5*1024*1024*1024;
					break;
				case eFS_DevUsbSpeed_480M:
					pstStorageInfo->ullDeviceWriteSpeedBps = 480*1024*1024;
					break;
				case eFS_DevUsbSpeed_12M:
					pstStorageInfo->ullDeviceWriteSpeedBps = 12*1024*1024;
					break;
				case eFS_DevUsbSpeed_1M:
					pstStorageInfo->ullDeviceWriteSpeedBps = 1*1024*1024;
					break;
				default:
					pstStorageInfo->ullDeviceWriteSpeedBps = 0;
					break;
			}
		}
		else
		{
			pstStorageInfo->ullDeviceWriteSpeedBps = 0;
		}

#if defined(CONFIG_MW_MM_LIMIT_REC)
		if((stDevInfo.eDevType == eFS_DevType_UsbHdd) && (stDevInfo.eDevUsbSpeed < eFS_DevUsbSpeed_5G))
		{
			pstStorageInfo->ulMaxRecInstance = CONFIG_MW_MM_LIMIT_REC_NUM;
		}
		else
#endif
		{
			pstStorageInfo->ulMaxRecInstance = CONFIG_MW_MM_MAX_REC_NUM;
		}

		pstStorageInfo->ulPortNum= stDevInfo.ulPortNum;
		pstStorageInfo->ullTotalSizeKb = stDevInfo.ullTotalSizeKb;


		umma_fs_dev_GetDeviceUuid (peDevIdxArray[i], pstStorageInfo->szUUID);
		HLIB_STD_StrNCpySafe(pstStorageInfo->szLabel, stDevInfo.szLabel, DxSTORAGE_LABEL_SIZE);
		HLIB_STD_StrNCpySafe(pstStorageInfo->szMountPath, stDevInfo.szMountPath, DxSTORAGE_NAME_LEN);

		umma_fs_dev_GetDeviceFileFullName(stDevInfo.eDevIdx, pstStorageInfo->szDevName);

		if (umma_fs_svc_GetPartitionInfoArray(stDevInfo.eDevIdx, &ulPartitionNum, &pstPartitions) == ERR_OK)
		{
			pstStorageInfo->nPartitionNum = ulPartitionNum;
			for (j = 0; j < ulPartitionNum; j++)
			{
				pstStorageInfo->astPartitionInfos[j].eFsType = (DxStorage_FsType_e)pstPartitions[j].ePartFsType;	// 두 enum이 달라서 converting이 필요한데.. 일단 값이 같아서 pass
				pstStorageInfo->astPartitionInfos[j].bAvailable = pstPartitions[j].bAvailable;
				pstStorageInfo->astPartitionInfos[j].ulStartUnit = pstPartitions[j].ulStartUnit;
				pstStorageInfo->astPartitionInfos[j].ulEndUnit   = pstPartitions[j].ulEndUnit;
				pstStorageInfo->astPartitionInfos[j].nPvrIdx     = pstPartitions[j].ulPvrIdx;
				pstStorageInfo->astPartitionInfos[j].nPlaybackIdx   = pstPartitions[j].ulPlaybackIdx;
				pstStorageInfo->astPartitionInfos[j].ulFlags = 0;
				if (pstPartitions[j].bMounted)
					pstStorageInfo->astPartitionInfos[j].ulFlags |= eDxSTORAGE_FLAG_MOUNTED;
				if (pstPartitions[j].bWritable == FALSE)
					pstStorageInfo->astPartitionInfos[j].ulFlags |= eDxSTORAGE_FLAG_WRITEPROTECTED;
				if (pstPartitions[j].bSecured)
					pstStorageInfo->astPartitionInfos[j].ulFlags |= eDxSTORAGE_FLAG_SECURED;

				if (pstPartitions[j].bDefaultPvr){
					pstStorageInfo->astPartitionInfos[j].ulFlags |= eDxSTORAGE_FLAG_DEFAULTHDD;
					pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_DEFAULTHDD;		// device의 대표값으로 활용
				}

				if (pstPartitions[j].bPvrFlag)
				{
					if (ePvrType & eFS_PVR_Main)
					{
						pstStorageInfo->astPartitionInfos[j].ulFlags |= eDxSTORAGE_FLAG_PVRDEVICE;
						pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_PVRDEVICE;
					}
					else if (ePvrType & eFS_PVR_Sub)
					{
						pstStorageInfo->astPartitionInfos[j].ulFlags |= eDxSTORAGE_FLAG_SUBPVRDEVICE;
						pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_SUBPVRDEVICE;
					}
					else if (ePvrType & eFS_PVR_Tsr)
					{
						pstStorageInfo->astPartitionInfos[j].ulFlags |= eDxSTORAGE_FLAG_TSRDEVICE;
						pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_TSRDEVICE;

					}
					pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_PAIRED;

					umma_fs_svc_PvrGetPvrPath (pstPartitions[j].ulPvrIdx, eFS_UsageType_Tsr, pstStorageInfo->astPartitionInfos[j].szTsrPath);
				}

				if (pstPartitions[j].bSystemFlag)
				{
					pstStorageInfo->ulFlags |= eDxSTORAGE_FLAG_SYSTEM;
					pstStorageInfo->astPartitionInfos[j].ulFlags |= eDxSTORAGE_FLAG_SYSTEM;
				}

				// TODO: 하나의 partition 에 대해서만 recording storage flag를 켜준다.
				if (ePvrType & eFS_PVR_RecStorage) {pstStorageInfo->astPartitionInfos[j].ulFlags |= eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE;}

				if (pstStorageInfo->ulFlags & eDxSTORAGE_FLAG_PVRDEVICE)
				{
					umma_fs_svc_GetReservedSizeKb(stDevInfo.eDevType, stDevInfo.ullTotalSizeKb, &ullResSizeKb);
					pstStorageInfo->ullReservedSizeKb = ullResSizeKb;
				}

				HLIB_STD_StrNCpySafe(pstStorageInfo->astPartitionInfos[j].szMountPath, pstPartitions[j].szMountPath, DxSTORAGE_NAME_LEN);
				HLIB_STD_StrNCpySafe(pstStorageInfo->astPartitionInfos[j].szVolumeName, pstPartitions[j].szVolumeName, DxSTORAGE_NAME_LEN);
				HLIB_STD_StrNCpySafe(pstStorageInfo->astPartitionInfos[j].szRecordPath, pstPartitions[j].szRecordPath, DxSTORAGE_NAME_LEN);
				HLIB_STD_StrNCpySafe(pstStorageInfo->astPartitionInfos[j].szMediaPath, pstPartitions[j].szMediaPath, DxSTORAGE_NAME_LEN);
				HLIB_STD_StrNCpySafe(pstStorageInfo->astPartitionInfos[j].szMediaVideo, pstPartitions[j].szMediaVideo, DxSTORAGE_NAME_LEN);
				HLIB_STD_StrNCpySafe(pstStorageInfo->astPartitionInfos[j].szMediaAudio, pstPartitions[j].szMediaAudio, DxSTORAGE_NAME_LEN);
				HLIB_STD_StrNCpySafe(pstStorageInfo->astPartitionInfos[j].szMediaPic, pstPartitions[j].szMediaPic, DxSTORAGE_NAME_LEN);
				HLIB_STD_StrNCpySafe(pstStorageInfo->astPartitionInfos[j].szMediaDownload, pstPartitions[j].szMediaDownload, DxSTORAGE_NAME_LEN);

				pstStorageInfo->astPartitionInfos[j].ullPartitionSizeKb = pstPartitions[j].ullTotalSizeKb;

				if (HxSTD_StrEmpty(pstStorageInfo->szUUID) == TRUE)
					HLIB_STD_StrNCpySafe(pstStorageInfo->szUUID, pstPartitions[j].szVolumeName, DxSTORAGE_NAME_LEN);
			}
		}

		if (pstPartitions)
			UMMA_Free((void *)pstPartitions);
	}

	if (peDevIdxArray)
	{
		umma_fs_dev_FreeDeviceIndexArray(peDevIdxArray);
	}
}

static HERROR	umma_fs_sendDummyEvent(UMMA_t *pstUmma)
{
	if (UMMA_OperationQueueEmpty(pstUmma))
	{
		UMMA_EVT_t	stEvt;
		HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));
		HLIB_MSG_Send(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t));
	}

	return ERR_OK;
}


static HERROR	umma_fs_InitDevice(void)
{
	UMMA_FS_DevType_e	aeDevList[eFS_DevType_MAX] = {0,};
	HUINT32	ulCount = 0, ulIdx;
	HBOOL	bHddInit = FALSE;	// HddInit는 한번만 하면 되어 채크함
	HBOOL	bUsbInit = FALSE;	// HddInit는 한번만 하면 되어 채크함

	umma_fs_svc_registerNotifier(umma_fs_sendEvent);
	umma_fs_svc_registerDataNotifier(umma_fs_sendData);
	umma_fs_dev_Init();

	ulCount = umma_fs_config_GetSupportedDeviceList(aeDevList);
	if (ulCount == 0)
	{
		HxLOG_Warning("This model doesn't support any type of storages... \n");
		return ERR_OK;
	}

	umma_fs_svc_PvrInit();

	for (ulIdx = 0 ; ulIdx < ulCount ; ulIdx++)
	{
//		HxLOG_Warning("aeDevList[ulIdx] = %d\n", aeDevList[ulIdx]);
		if (aeDevList[ulIdx] == eFS_DevType_IntHdd ||
			aeDevList[ulIdx] == eFS_DevType_Detachable ||
			aeDevList[ulIdx] == eFS_DevType_eSataHdd	)
		{
			if (bHddInit == FALSE)
			{
				umma_fs_hdd_Init();
				bHddInit = TRUE;
			}
			umma_fs_hdd_PowerOn(aeDevList[ulIdx]);
		}
		else if (aeDevList[ulIdx] == eFS_DevType_UsbHdd  ||
			aeDevList[ulIdx] == eFS_DevType_UsbMem)
		{
			if (bUsbInit == FALSE)
			{
				umma_fs_usb_Init();
				bUsbInit = TRUE;
			}
		}
		else if (aeDevList[ulIdx] == eFS_DevType_MMC)
		{
			umma_fs_mmc_Init();
		}
	}

	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
static DxStorage_PartitionInfo_t*	umma_fs_GetPvrPartitionInfo(DxStorage_Info_t *pstStorageInfo)
{
	HUINT32		i;

	for (i = 0; i < pstStorageInfo->nPartitionNum; i++)
	{
		if ((pstStorageInfo->astPartitionInfos[i].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) ||
			(pstStorageInfo->astPartitionInfos[i].ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE))
			return &pstStorageInfo->astPartitionInfos[i];
	}

	return NULL;
}
static void umma_fs_openfiles_callback(const HCHAR * path,HINT32 fd,void * userdata)
{
	HxLOG_Error("*FILE OPEN!!!*****************************\n");
	HxLOG_Error("processname: %s\n",(HCHAR*)userdata);
	HxLOG_Error("PATH: %s\n",path);
	HxLOG_Error("FD: %d\n",fd);
	HxLOG_Error("*****************************************\n");

	//	TODO: 열려저 있는 파일에 대한 처리
}

// 다음과 같은 프로세스 이름에서 /mnt/hd2 에 해당하는 FD 값이 있는지 찾는다.
static void umma_fs_GetOpenFiles(const HCHAR* pszMountPath)
{
	UMMA_FS_OPEN_LIST_FUNCTION("obama");
	UMMA_FS_OPEN_LIST_FUNCTION("startlogo");
	UMMA_FS_OPEN_LIST_FUNCTION("pama");
	UMMA_FS_OPEN_LIST_FUNCTION("ipepg");
	UMMA_FS_OPEN_LIST_FUNCTION("gwmpopup");
	UMMA_FS_OPEN_LIST_FUNCTION("subtitleplayer");
	UMMA_FS_OPEN_LIST_FUNCTION("remoteapp");
	UMMA_FS_OPEN_LIST_FUNCTION("mhegib");
	UMMA_FS_OPEN_LIST_FUNCTION("screensaver");
	UMMA_FS_OPEN_LIST_FUNCTION("corsair");
	UMMA_FS_OPEN_LIST_FUNCTION("homma");
	UMMA_FS_OPEN_LIST_FUNCTION("umma");
	UMMA_FS_OPEN_LIST_FUNCTION("dama");
	UMMA_FS_OPEN_LIST_FUNCTION("thma");
	UMMA_FS_OPEN_LIST_FUNCTION("nima");
	UMMA_FS_OPEN_LIST_FUNCTION("sama");
	HxLOG_Print("----------------------------------");
}
STATIC int umma_fs_CmdShowOpenFiles (void *szArgList)
{
	HUINT32	i;
	HxList_t	*list = s_pstStorageList;
	DxStorage_Info_t	*pstInfo;

	HLIB_CMD_Printf("[%s] [%d] Enter~\n",__FUNCTION__,__LINE__);

	while (list)
	{
		pstInfo = (DxStorage_Info_t*)HLIB_LIST_Data(list);
		if (pstInfo != NULL)
		{
			for (i=0 ; i<pstInfo->nPartitionNum ; i++)
			{
				HLIB_CMD_Printf("[%s] [%d] path : %s\n",__FUNCTION__,__LINE__, pstInfo->astPartitionInfos[i].szMountPath);
				umma_fs_GetOpenFiles(pstInfo->astPartitionInfos[i].szMountPath);
			}
		}

		list = list->next;
	}
	return 0;
}

STATIC int umma_fs_CmdPrintConfig (void *szArgList)
{
	umma_fs_config_Print();

	return 0;
}

STATIC int umma_fs_CmdFormatStorage (void *szArgList)
{
	HUINT8				*aucArg=NULL;
	HUINT32				ulDevId = 0;
	HUINT8				aucArg2[32];
	UMMA_SIGNAL_t		*pstSignal;

	HxSTD_memset(aucArg2, 0, 32);

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulDevId) == 0 ) return HxCMD_ERR;

    /********************
     * get 2nd argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
	if( aucArg != NULL ) {
		if( sscanf(aucArg, "%s", (char *)&aucArg2) == 0 ) return HxCMD_ERR;
	}
	else
	{
		HxSTD_StrNCpy(aucArg2, "HDD\0", 4);
	}

	HxLOG_Warning("CMD FORMAT!! ulDeviceId : %d, Name : %s \n",ulDevId, aucArg2);

	HxLOG_Trace();
	// UMMA Main task에 알림
	UMMA_SendEvent(eUMMA_EVT_STORAGE_REMOVED, ulDevId, 0, 0);

	// UMMA_FS task에 알림
	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_FORMAT;
	pstSignal->ulParam1 = (HUINT32)ulDevId;
	pstSignal->ulParam2 = (HUINT32)UMMA_StrDup(aucArg2);
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId   = 0;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return 0;
}

#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
STATIC int umma_fs_CmdHddPower (void *szArgList)
{
	HUINT8	 *aucArg=NULL;
	int		nOnOff= 0;
	HUINT8	 aucArg2[32];

	HxSTD_memset(aucArg2, 0, 32);

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &nOnOff) == 0 ) return HxCMD_ERR;

	SEUAPI_HDD_PWR_Ctrl(nOnOff);

	return 0;
}
#endif

STATIC int umma_fs_CmdtestUsbSpeed (void *szArgList)
{
	HINT32		nBlockSizeKb, nCopySizeMB, nCount;
	HUINT8	 *aucArg=NULL;
	HxList_t	*list = s_pstStorageList;
	DxStorage_Info_t	*pstInfo;
	HCHAR	szUsbPath[64] = {0,};
	HUINT32	ulReadMbps, ulWriteMbps;

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &nBlockSizeKb) == 0 ) return HxCMD_ERR;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &nCopySizeMB) == 0 ) return HxCMD_ERR;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &nCount) == 0 ) return HxCMD_ERR;

	while (list)
	{
		pstInfo = (DxStorage_Info_t*)HLIB_LIST_Data(list);
		if (pstInfo != NULL)
		{
			if (pstInfo->eDevType == eDxSTORAGE_DEVTYPE_USBMEM)
			{
				HxSTD_StrNCpy(szUsbPath, pstInfo->szMountPath, DxSTORAGE_NAME_LEN);
				HLIB_CMD_Printf("USB Flash path : %s\n", szUsbPath);
				break;
			}
		}
		list = list->next;
	}

	umma_fs_do_CheckUsbSpeed(szUsbPath, nBlockSizeKb, nCopySizeMB, nCount, &ulReadMbps, &ulWriteMbps);

	return 0;
}



//Simple test
STATIC int umma_fs_CmdSimpleTest (void *szArgList)
{
	HINT32		nNumber;
	HUINT8	 *aucArg=NULL;

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &nNumber) == 0 ) return HxCMD_ERR;


	switch(nNumber)
	{
		case 1:
			HLIB_CMD_Printf("test1\n");
			HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_RebootDone);
		break;

		case 2:
			//2 Add your simple test
			break;

		default :
			HLIB_CMD_Printf("Unknown test (%d)\n", nNumber);
		break;
	}

	return 0;
}

STATIC int umma_fs_CmdParingPvrHDD (void *szArgList)
{
	HUINT8	 *aucArg=NULL;
	int		nMode = 0;
	HUINT8	 aucUUID[64];
	UMMA_t	*pstUmmaFs;
	UMMA_SIGNAL_t	*pstSignal;

	HxSTD_memset(aucUUID, 0, 64);

	/********************
	* get 1st argument *
	********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &nMode) == 0 ) return HxCMD_ERR;

	/********************
	* get 2nd argument *
	********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
	if( aucArg != NULL ) {
		if( sscanf(aucArg, "%s", (char *)&aucUUID) == 0 ) return HxCMD_ERR;
	}
	else
		return HxCMD_ERR;

	HxLOG_Warning("CMD HDD paring or unparing!! Mode : %d, UUID : %s \n",nMode, aucUUID);
	HxLOG_Trace();

	// UMMA_FS task에 알림
	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = nMode ? eFS_SIGNAL_PAIRING : eFS_SIGNAL_UNPAIRING;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(aucUUID);
	pstSignal->ulParam2 = 0;
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= 0;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return 0;
}

STATIC void umma_fs_svc_CmdInit(void)
{
#define	hCmdHandle		"FS"
	HLIB_CMD_RegisterWord(hCmdHandle,
							umma_fs_CmdShowOpenFiles,
							"open", 						/* keyword */
							"show opened file list ",			/*	 help  */
							"open"
							);

	HLIB_CMD_RegisterWord (hCmdHandle, umma_fs_CmdFormatStorage, "format", "Format storage", "format [ulDeviceId szName]");

	HLIB_CMD_RegisterWord (hCmdHandle, umma_fs_CmdPrintConfig, "config", "Show config", "config");
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	HLIB_CMD_RegisterWord (hCmdHandle, umma_fs_CmdHddPower, "hddpower", "Format storage", "hddpower [0(off) or 1(on)]");
#endif
	HLIB_CMD_RegisterWord (hCmdHandle, umma_fs_CmdtestUsbSpeed, "speed", "Test usb speed", "speed [BLOCK_KB FILESIZE_MB TEST_COUNT]");

	HLIB_CMD_RegisterWord (hCmdHandle, umma_fs_CmdSimpleTest, "test", "Add your simple test", "test [number]");
	HLIB_CMD_RegisterWord (hCmdHandle, umma_fs_CmdParingPvrHDD, "paring", "paring external hdd for PVR", "paring [0(unparing) or 1(paring)] UUID");
}

#endif

#define _______CMD_______________

static	HERROR	umma_fs_do_Format (UMMA_FS_DevIdx_e eDevIdx, HINT32 nPartition, HCHAR *pszName)
{
	UMMA_FS_FormatPartition_e ePartition = (UMMA_FS_FormatPartition_e)nPartition;
	HUINT8					szVolumeName[UMMA_FS_VOLUME_LABEL_SIZE];
	UMMA_FS_DevInfo_t			 stDevInfo;
	HERROR					 hErr;

	HxLOG_Trace();
	if ((pszName == NULL) || (pszName[0] == '\0'))
	{
		HxSTD_StrNCpy(szVolumeName, "hmx_int_stor", UMMA_FS_VOLUME_LABEL_SIZE-1);
	}
	else
	{
		HxSTD_StrNCpy(szVolumeName, pszName, UMMA_FS_VOLUME_LABEL_SIZE-1);
	}

	hErr = umma_fs_svc_GetDeviceInfo (eDevIdx, &stDevInfo);
	if (hErr == ERR_OK)
	{
#if defined(CONFIG_DEBUG)
{
	UMMA_FS_PartitionInfo_t	*pstPartitions;
	HUINT32		ulPartitionNum;
	HUINT32		ulIdx;
	if (umma_fs_svc_GetPartitionInfoArray(stDevInfo.eDevIdx, &ulPartitionNum, &pstPartitions) == ERR_OK)
	{
		for(ulIdx = 0 ; ulIdx < stDevInfo.ulNumPartition; ulIdx++)
		{
			umma_fs_GetOpenFiles(pstPartitions[ulIdx].szMountPath);
		}
	}

	if (pstPartitions)
		UMMA_Free((void *)pstPartitions);
}
#endif
		HxLOG_Print("[format devType [%d] , partition[%d]]\n",stDevInfo.eDevType,ePartition);

		switch (stDevInfo.eDevType)
		{
		case eFS_DevType_IntHdd:
		case eFS_DevType_Detachable:
			if (ePartition == eFORMAT_PARTITION_ALL)
			{
				hErr = umma_fs_svc_PvrFormatIntPvrHdd (eDevIdx, szVolumeName);
			}
			else if (ePartition == eFORMAT_PARTITION_SYSTEM)
			{
				hErr = umma_fs_svc_PvrFormatIntHddPartition (eDevIdx, eFS_PvrPartitionType_System, szVolumeName);
			}
			else if (ePartition == eFORMAT_PARTITION_PVR)
			{
				hErr = umma_fs_svc_PvrFormatIntHddPartition (eDevIdx, eFS_PvrPartitionType_PvrMedia, szVolumeName);
			}
			else if (ePartition == eFORMAT_PARTITION_IP)
			{
				hErr = umma_fs_svc_PvrFormatIntHddPartition (eDevIdx, eFS_PvrPartitionType_WebBrowse, szVolumeName);
			}
			else if (ePartition == eFORMAT_PARTITION_TVAPPS)
			{
				hErr = umma_fs_svc_PvrFormatIntHddPartition (eDevIdx, eFS_PvrPartitionType_TvApps, szVolumeName);
			}
			else
			{
				hErr = ERR_FAIL;
			}

			if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
			break;

		case eFS_DevType_eSataHdd:
		case eFS_DevType_UsbHdd:
		case eFS_DevType_UsbMem:
			hErr = umma_fs_svc_PvrFormatExtPvrHdd (eDevIdx, szVolumeName);
			if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
			break;

		case eFS_DevType_MMC:
			hErr = umma_fs_mmc_Format();
			if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
			break;

		default:
			hErr = ERR_FAIL;
			break;
		}
	}

	//--------------------------------------------------------
	return ERR_OK;		// 이 메시지는 여기서 Format 까지 명령 넣고 막아야 하므로 break 이다.
}

static HERROR umma_fs_do_Unmount(unsigned long TimerId, HINT32	nMsg, UMMA_FS_DevIdx_e eDevIdx)
{
	UMMA_FS_DevInfo_t 	stDevInfo;
	HERROR	hErr;
	HBOOL	bSafeRemove = FALSE;
	HUINT32		ulCnt, ulMaxCnt = 10;

	HxLOG_Trace();
	hErr = umma_fs_svc_GetDeviceInfo(eDevIdx, &stDevInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Unknown device! (%d)  \n", eDevIdx);
		return ERR_FAIL;
	}

	bSafeRemove =  (nMsg == eFS_MSG_SAFE_REMOVE) ? TRUE : FALSE;

	for (ulCnt = 0 ; ulCnt < ulMaxCnt ; ulCnt++)
	{
		if (eFS_DevType_UsbHdd == stDevInfo.eDevType || eFS_DevType_UsbMem == stDevInfo.eDevType)
		{
			hErr = umma_fs_usb_UnmountDevice(stDevInfo.eDevIdx, bSafeRemove);
		}
		else if (stDevInfo.eDevType == eFS_DevType_IntHdd || stDevInfo.eDevType == eFS_DevType_Detachable || stDevInfo.eDevType == eFS_DevType_eSataHdd)
		{
			hErr = umma_fs_svc_PvrUnmountIntPvrHdd(stDevInfo.eDevIdx, bSafeRemove);
		}
		else if (stDevInfo.eDevType == eFS_DevType_MMC)
		{
			hErr = umma_fs_mmc_UnMountDevice(stDevInfo.eDevIdx, bSafeRemove);
		}
		else
		{
			HxLOG_Error("eDevType[0x%x] is not allowed to unmount! >>>\n", stDevInfo.eDevType);
			hErr = ERR_FAIL;
		}

		//////// Process result ///////////
		if (hErr != ERR_OK)
		{
			HxLOG_Warning("fail to unmount -- retry [%d/10]\n", ulCnt);
			HLIB_STD_TaskSleep(1000);	// 1초간 쉰다.
		}
		else		// ERR_OK
		{
			break;
		}
	}


	if (hErr == ERR_FAIL)
	{
		HxLOG_Trace();
		umma_fs_sendEvent(eFS_EVT_UNMOUNT_FAIL, eDevIdx, 0, hErr);
	}

	if (s_nReqId[eDevIdx] !=0)
	{
		HxLOG_Trace();
		umma_fs_sendEvent(eFS_MSG_SEND_RESULT, eDevIdx, s_nReqId[eDevIdx], hErr);
		 s_nReqId[eDevIdx]= 0;
	}

	return hErr;
}

// pszPath : Mount path
// ulBlockKb : 한번에 복사하는 block 단위 (Kbyte)
// ulFileSizeMb : 복사하는 Test file Size (Mbyte), 즉, ulBlockKb/ulFileSizeMb회 만큼 loop 돌며 수행함
// ulTestCount : 테스트 횟수, 횟수 만큼 수행하여 평균을 낸다.
// pulReadMbps : Read 평균 속도 (Mbps)
// pulWriteMbps : Write 평균 속도 (Mbps)
static HERROR umma_fs_do_CheckUsbSpeed(HCHAR *pszPath, HUINT32 ulBlockKb, HUINT32 ulFileSizeMb, HUINT32 ulTestCount, HUINT32 *pulReadMbps, HUINT32 *pulWriteMbps)
{
	HUINT32	i,j, ulCnt;
	HxFILE_t	hFile;
	HULONG	ulStart, ulEnd, ulTime;
	HUINT32	ulReadMbps, ulWriteMbps;
	HUINT8	*pBuf;
	HFLOAT64	byte_ps, mbit_ps=0, mbit_ps_t=0;
	HCHAR		szTestFile[64] = {0,};
	int fd, len;

	if (ulBlockKb == 0)
	{
		return ERR_FAIL;
	}

	// Make dummy data
	pBuf = (HUINT8 *)HLIB_STD_MemAlloc(ulBlockKb*1024);
	if (pBuf == NULL)
	{
		return ERR_FAIL;
	}
	for(i=0 ; i< ulBlockKb*1024; i++)
	{
		pBuf[i] = i % 0xff;
	}

	ulCnt = (MEGA_BYTE*ulFileSizeMb)/(ulBlockKb*1024);

	if (ulCnt <= 0)
	{
		HxLOG_Warning("Please check your parameters...(Maybe BlockSize is too big..)");
		HLIB_STD_MemFree(pBuf);
		return ERR_FAIL;
	}

	// Write test
	for(i=0 ; i< ulTestCount; i++)
	{
		if (HLIB_FILE_Exist(szTestFile))
		{
			HLIB_FILE_Delete(szTestFile);
		}

		HxSTD_snprintf(szTestFile, sizeof(szTestFile), "%s/testfile%d",pszPath,i);

		ulStart = VK_TIMER_GetSystemTick();

		fd = open(szTestFile, O_CREAT | O_SYNC | O_TRUNC | O_WRONLY);
		if (fd == -1)
		{
			HxLOG_Error("\n");
			HLIB_STD_MemFree(pBuf);
			return ERR_FAIL;
		}

		for(j=0 ; j< ulCnt ; j++)
		{
			len = write(fd, pBuf, (ulBlockKb*KILO_BYTE));
			if (len != ulBlockKb*KILO_BYTE)
			{
				HxLOG_Error("\n");
				HLIB_STD_MemFree(pBuf);
				close(fd);
				return ERR_FAIL;
			}
		}

		close(fd);

		ulEnd = VK_TIMER_GetSystemTick();
		ulTime = ulEnd-ulStart;

		byte_ps = ((MEGA_BYTE*ulFileSizeMb)/ulTime)*1000;
		mbit_ps = (byte_ps * 8) / MEGA_BYTE;

		HxLOG_Warning("[%d] Duration : %d msec, %4.2f Mbps (byte_ps : %f)\n",i , ulTime, mbit_ps, byte_ps);

		mbit_ps_t += mbit_ps;
		VK_TASK_Sleep(300);
	}

	ulWriteMbps = (HUINT32)(mbit_ps_t/ulTestCount);

	HxLOG_Warning("Write Average :: %4.2f Mbps \n", (mbit_ps_t/ulTestCount));

	mbit_ps_t = mbit_ps =0;

	HxLOG_Warning("============== Read Test ==============\n");

	// Read test
	for(i=0 ; i< ulTestCount; i++)
	{
		hFile = HLIB_FILE_Open(szTestFile, "r");
		if (hFile == NULL)
		{
			HxLOG_Error("\n");
			if (pBuf)
			{
				HLIB_STD_MemFree(pBuf);
			}
			return ERR_FAIL;
		}

		ulStart = VK_TIMER_GetSystemTick();

		for(j=0 ; j< ulCnt ; j++)
		{
			(void)HLIB_FILE_Read(hFile, pBuf, 1, (ulBlockKb*1024));
		}

		HLIB_FILE_Close(hFile);

		ulEnd = VK_TIMER_GetSystemTick();
		ulTime = ulEnd-ulStart;

		byte_ps = ((MEGA_BYTE*ulFileSizeMb)/ulTime)*1000;
		mbit_ps = (byte_ps * 8) / MEGA_BYTE;

		HxLOG_Warning("[%d] Duration : %d msec, %4.2f Mbps (byte_ps : %f)\n",i , ulTime, mbit_ps, byte_ps);

		mbit_ps_t += mbit_ps;

		VK_TASK_Sleep(300);
	}

	HxLOG_Warning("Read Average :: %4.2f Mbps\n", mbit_ps_t/ulTestCount);

	ulReadMbps = (HUINT32)(mbit_ps_t/ulTestCount);

	if (HLIB_FILE_Exist(szTestFile))
	{
		HLIB_FILE_Delete(szTestFile);
	}

	if (pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	if (pulReadMbps)
	{
		*pulReadMbps = ulReadMbps;
	}

	if (pulWriteMbps)
	{
		*pulWriteMbps = ulWriteMbps;
	}

	return ERR_OK;
}


static HERROR umma_fs_request_SafeRemoveDevice(HUINT32 ulDeviceId, HBOOL bForce, HINT32 nReqId)
{
	//2 todo: Signal other processes & Stop PVR

	s_nReqId[ulDeviceId]= nReqId;

	umma_fs_sendEvent(eFS_MSG_SAFE_REMOVE, (HINT32)ulDeviceId, (HINT32)bForce, 0);

	return ERR_OK;
}

static HERROR umma_fs_request_UnmountDevice(HUINT32 ulDeviceId)
{
	umma_fs_sendEvent(eFS_MSG_UNMOUNT, (HINT32)ulDeviceId, 0, 0);

	return ERR_OK;
}

static HERROR umma_fs_request_FormatDevice(HUINT32 ulDeviceId, HCHAR * szVolumeName, HINT32 nReqId)
{
	HERROR	hErr;
	UMMA_FS_DevInfo_t stDevInfo;
	HUINT32 ulPartitionNum;
	UMMA_FS_PartitionInfo_t *pstPartitions;
	HUINT32	ulIdx;

	hErr = umma_fs_svc_GetDeviceInfo (ulDeviceId, &stDevInfo);
	if (umma_fs_svc_GetPartitionInfoArray(stDevInfo.eDevIdx, &ulPartitionNum, &pstPartitions) == ERR_OK)
	{
		if (ulPartitionNum > 1)
		{
			for(ulIdx = 0 ;ulIdx < ulPartitionNum ; ulIdx++)
			{
				HxLOG_Warning("[signal:onFormatOccurred] ulDeviceId : %d, path : %s\n", ulDeviceId, pstPartitions[ulIdx].szMountPath);
				HAPI_BroadcastSignal("signal:onFormatOccurred", 0, "is", ulDeviceId, pstPartitions[ulIdx].szMountPath);
			}
		}
		else
		{
			HxLOG_Warning("[signal:onFormatOccurred] ulDeviceId : %d, path : %s\n", ulDeviceId, stDevInfo.szMountPath);
			HAPI_BroadcastSignal("signal:onFormatOccurred", 0, "is", ulDeviceId, stDevInfo.szMountPath);
		}
	}

	if (pstPartitions)
		UMMA_Free((void *)pstPartitions);

	HLIB_STD_TaskSleep(1000);	// 한박자 쉬고..

	hErr = umma_fs_do_Format(ulDeviceId, eFORMAT_PARTITION_ALL, szVolumeName);

	if (HxSTD_StrLen((HCHAR *)szVolumeName) != 0)
	{
		UMMA_Free((void *)szVolumeName);
	}

	// 이 시점에서 MW_FS에 Format 명령이 내려가서 Device 상태가 Formatting이 되어 있어야 한다.
	hErr = umma_fs_svc_GetDeviceInfo (ulDeviceId, &stDevInfo);
	if (hErr == ERR_OK && stDevInfo.eDevStatus == eFS_DevStatus_Formatting)
	{
		umma_fs_sendEvent(eFS_EVT_FORMATTING, ulDeviceId, 1, 100);
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("format fail!!  \n");
		umma_fs_sendEvent(eFS_EVT_FORMAT_FAIL, ulDeviceId, 0, 0);
	}

	return hErr;
}

static HERROR umma_fs_do_unmount_all_devices(void)
{
	HUINT32	ulNum, ulIdx=0;
	HERROR 	hErr;
	UMMA_FS_DevIdx_e	*peDevIdxArray = NULL;

	HxLOG_Warning("Unmount all devices !! \n");
	hErr = umma_fs_dev_GetDeviceIndexArray (&ulNum, &peDevIdxArray);
	if (hErr == ERR_OK)
	{
		for(ulIdx = 0 ; ulIdx < ulNum ; ulIdx++)
		{
			hErr |= umma_fs_do_Unmount(0, eFS_MSG_SAFE_REMOVE, (UMMA_FS_DevIdx_e)peDevIdxArray[ulIdx]);
		}
	}
	if (peDevIdxArray)	{umma_fs_dev_FreeDeviceIndexArray(peDevIdxArray);}

	return hErr;
}

static void umma_fs_request_powerOn(void)
{
	// HDD
	umma_fs_hdd_PowerOn(eFS_DevType_ALL);

	// USB
	umma_fs_usb_PowerOn();

	// MMC
	umma_fs_mmc_PowerOn();
}

static void umma_fs_request_powerOff(void)
{
	// HDD
	umma_fs_hdd_PowerOff(eFS_DevType_ALL);

	// USB
	umma_fs_usb_PowerOff();

	// MMC
	umma_fs_mmc_PowerOff();
}

static HERROR umma_fs_cmd_StartDst(HUINT32 ulDeviceId)
{
	UMMA_FS_DevIdx_e	eDevIdx = ulDeviceId;
	UMMA_FS_DevInfo_t	stInfo;
	HERROR hErr;

	HxLOG_Trace();
	hErr = umma_fs_svc_GetDeviceInfo(eDevIdx, &stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if (FALSE == stInfo.bMounted)
	{
		HxLOG_Error("eDevIdx[%d] is not mounted Device! >>>\n", stInfo.eDevIdx);
		return ERR_FAIL;
	}

	if (stInfo.eDevType == eFS_DevType_IntHdd || stInfo.eDevType == eFS_DevType_Detachable || stInfo.eDevType == eFS_DevType_eSataHdd)
	{
		hErr = umma_fs_svc_PvrStartDst(stInfo.eDevIdx);
	}

	if (ERR_OK == hErr)
	{
		HxLOG_Print("DST Started! (ulDevceId:%d)\n", ulDeviceId);
		umma_fs_sendEvent(eFS_EVT_DST_ON_GOING, ulDeviceId, 1, 100);
	}
	else
	{
		HxLOG_Error("DST Failed! (ulDevceId:%d)\n", ulDeviceId);
		umma_fs_sendEvent(eFS_EVT_DST_FAIL, ulDeviceId, 0, 0);
	}

	return ERR_OK;
}

static HERROR	umma_fs_cmd_SetDefaultPVRStorage(HCHAR *szStorageId)
{
	HUINT32 ulPvrIdx;
	HERROR hErr = ERR_FAIL;

	HxLOG_Trace();
	// storageId를 PvrIdx로 변환한다.
	hErr = umma_fs_svc_PvrGetPvrIdxByDevUuid(&ulPvrIdx, (HUINT8 *)szStorageId);

	if(hErr != ERR_OK)
	{
		return hErr;
	}
	// Default HDD로 설정하는 MW API 호출
	hErr = umma_fs_svc_PvrSetPvrDefaultHdd(ulPvrIdx, TRUE);

	return hErr;
}

static HERROR	umma_fs_cmd_CheckUsbSpeed(HCHAR *szStorageId)
{
	HERROR hErr = ERR_FAIL;
	UMMA_FS_DevIdx_e		*peDevIdxArray = NULL;
	HUINT32		i, ulNum;
	DxStorage_Info_t	*pstStorageInfo;
	HCHAR		szMountPath[DxSTORAGE_NAME_LEN];
	HUINT32		ulDeviceId = 0xFFFF;
	HUINT32		ulReadMbps = 0, ulWriteMbps = 0;

	HxLOG_Trace();

	hErr = umma_fs_dev_GetDeviceIndexArray (&ulNum, &peDevIdxArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Warning("empty \n");
		return ERR_OK;
	}

	for (i = 0; i < ulNum; i++)
	{
		pstStorageInfo = umma_fs_GetStorageInfo(peDevIdxArray[i]);
		if (pstStorageInfo == NULL)
			continue;

		if(HxSTD_StrNCmp(pstStorageInfo->szUUID, szStorageId, UMMA_FS_DEV_UUID_BUFSIZE) == 0)
		{
			ulDeviceId = pstStorageInfo->ulDeviceId;
			HxSTD_StrNCpy(szMountPath, pstStorageInfo->szMountPath, DxSTORAGE_NAME_LEN);
			break;
		}

	}
	umma_fs_do_CheckUsbSpeed(szMountPath, BLOCK_KB, FILESIZE_MB, TEST_COUNT, &ulReadMbps, &ulWriteMbps);

	if (ulDeviceId != 0xFFFF)
	{
	umma_fs_sendEvent(eFS_MSG_CHECK_USB_SPEED_DONE, ulDeviceId, ulReadMbps, ulWriteMbps);
	}

	if (peDevIdxArray)
	{
		umma_fs_dev_FreeDeviceIndexArray(peDevIdxArray);
	}

	return hErr;
}



static HERROR	umma_fs_cmd_SetStorageName(HCHAR *szStorageId, HCHAR *szName)
{
	UMMA_FS_DevIdx_e	eDevIdx;
	HERROR hErr;

	HxLOG_Trace();
	hErr = umma_fs_dev_GetDevIdxByDevUuid(&eDevIdx, szStorageId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_FS_GetDevIdxByDevUuid FAIL!! \n");
		return ERR_FAIL;
	}

	// EXT HDD의 경우 Partition이 하나이기 때문에 0으로 넣어주지만, INT HDD의 이름을 변경하려 한다면 아래 PartIdx를 적절히 수정해야 한다.
	hErr = umma_fs_dev_SetDeviceVolumeName(eDevIdx, 0, (HUINT8 *)szName);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("SVC_FS_SetStorageVolumeName Error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	umma_fs_cmd_DisablePvrHdd(HCHAR *szStorageId)
{
	HUINT32 ulPvrIdx;
	UMMA_FS_DevIdx_e eDevIdx;
	HERROR hErr = ERR_FAIL;

	HxLOG_Trace();
	// storageId를 PvrIdx로 변환한다.
	hErr = umma_fs_svc_PvrGetPvrIdxByDevUuid(&ulPvrIdx, (HUINT8 *)szStorageId);
	hErr |= umma_fs_dev_GetDevIdxByDevUuid(&eDevIdx, (HUINT8 *)szStorageId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail..\n");
		return hErr;
	}

	HxLOG_Warning("ulPvrIdx : %d, eDevIdx : %d\n", ulPvrIdx, eDevIdx);

	// External HDD만 지원한다.
	hErr = umma_fs_svc_PvrDisableExtPvrHdd(ulPvrIdx);

	return hErr;
}

static HERROR	umma_fs_cmd_PairingPvrHdd(HCHAR *szStorageId)
{
	UMMA_FS_DevIdx_e eDevIdx;
	HERROR hErr = ERR_FAIL;

	HxLOG_Trace();
	hErr = umma_fs_dev_GetDevIdxByDevUuid(&eDevIdx, (HUINT8 *)szStorageId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail..(szStorageId:%s)\n", szStorageId);
		return hErr;
	}
	HxLOG_Warning("eDevIdx : %d\n", eDevIdx);

	// External HDD만 지원한다.
	hErr = umma_fs_svc_PvrSwitchExtPvrHddDev(eDevIdx);
	return hErr;
}

static HERROR	umma_fs_cmd_UnPairingPvrHdd(HCHAR *szStorageId)
{
	UMMA_FS_DevIdx_e eDevIdx;
	HUINT32 ulPvrIdx = 0xff;
	HERROR hErr = ERR_FAIL;

	HxLOG_Trace();
	// storageId를 PvrIdx로 변환한다.
	hErr = umma_fs_dev_GetDevIdxByDevUuid(&eDevIdx, (HUINT8 *)szStorageId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail..\n");
		return hErr;
	}
	hErr = umma_fs_svc_PvrGetPvrIdxByDevIdx(&ulPvrIdx ,eDevIdx);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail..\n");
		return hErr;
	}

	HxLOG_Warning("eDevIdx : %d\n", eDevIdx);

	hErr = umma_fs_svc_PvrUnRegisterExtPvrHdd(eDevIdx);
	return hErr;
}

#define _______EVENT_CALLBACK_______
static void		umma_fs_onInit(UMMA_t*pstInstance)
{
	HxLOG_Warning("umma_fs_onInit()\n");
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onInit,  NULL);
}

static void		umma_fs_onChangeDevice(UMMA_t*pstInstance)
{
	HINT32	nStorageNum = HLIB_LIST_Length(s_pstStorageList);
	DxStorage_Info_t	*pstStorageInfo, *pstHead;

	if (nStorageNum)
	{
		HxList_t	*list;
		pstStorageInfo = NULL;

		pstStorageInfo = UMMA_Malloc(sizeof(DxStorage_Info_t) * nStorageNum);

		if (pstStorageInfo == NULL)
		{
			HxLOG_Critical("UMMA_Malloc fail!!! \n");
			return ;
		}

		list = s_pstStorageList;
		pstHead = pstStorageInfo;
		while (list)
		{
			HxSTD_MemCopy(pstStorageInfo++, HLIB_LIST_Data(list), sizeof(DxStorage_Info_t));
			list = list->next;
		}
		HxLOG_Warning("StorageNum : %d \n", nStorageNum);

		HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onChangeDevice,  "ib", nStorageNum, (void*)pstHead, sizeof(DxStorage_Info_t) * nStorageNum);
		UMMA_Free((void *)pstHead);
	}
	else
	{
		HxLOG_Warning("StorageNum : %d \n", nStorageNum);
		HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onChangeDevice,  "ib", nStorageNum, "Empty", 5);
	}
}

static void		umma_fs_onDeviceConnected(HUINT32 ulDeviceId, HBOOL bConnected, DxStorage_Info_t	*pstInfo, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onDeviceConnected, "iib", ulDeviceId, (bConnected ? 1 : 0), pstInfo, sizeof(DxStorage_Info_t));
}

static void		umma_fs_onNeedFormatEvent(HUINT32 ulDeviceId, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onNeedFormat,  "i", ulDeviceId);
}

static void		umma_fs_onOverCurrentEvent(HUINT32 ulDeviceId, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onOverCurrent,  "i", ulDeviceId);
}

static void		umma_fs_onFormatStateChange(HUINT32 ulDeviceId, HINT32 nProgress, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onFormatStateChange, "ii", ulDeviceId, nProgress);
}

static void		umma_fs_onDSTStateChange(HUINT32 ulDeviceId, HINT32 nProgress, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onDSTStateChange, "ii", ulDeviceId, nProgress);
}

static void		umma_fs_onPVRReady(HUINT32 ulDeviceId, HBOOL bReady, HBOOL bDefaultPvr, HUINT32 ulPvrIdx,UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onPVRReady,  "iiii", ulDeviceId, (bReady ? 1 : 0), (bDefaultPvr ? 1 : 0), ulPvrIdx);
}

static void		umma_fs_onTSRReady(HUINT32 ulDeviceId, HBOOL bReady,UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onTSRReady,  "ii", ulDeviceId, (bReady ? 1 : 0));
}

static void		umma_fs_onPlaybackReady(HUINT32 ulDeviceId, HBOOL bReady, HBOOL bDefaultPvr, HUINT32 ulPlaybackIdx, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onPlaybackReady,  "iiii", ulDeviceId, (bReady ? 1 : 0), 0, ulPlaybackIdx);
}

static void		umma_fs_onUmountFail(HUINT32 ulDeviceId, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onUnmountFail, "i", ulDeviceId);
}

static void		umma_fs_onCheckUsbSpeed(HUINT32 ulDeviceId, HUINT32 ulReadKbps, HUINT32 ulWriteKbps, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onCheckUsbSpeed, "iii", ulDeviceId, ulReadKbps, ulWriteKbps);
}

static void		umma_fs_onResult(HINT32 nRequestId, HERROR hErr, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onResult, "ii", nRequestId, hErr);
}

static void		umma_fs_onCheckDiskStarted(HUINT32 ulDeviceId, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onCheckDiskStarted, "i", ulDeviceId);
}

static void		umma_fs_onStoragePowerOff(UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onStoragePowerOff, NULL);
}

static void		umma_fs_onDefaultHddInfoChanged(HCHAR *szOldUuid, HCHAR *szNewUuid, HUINT32 ulOldRec, HUINT32 ulNewRec, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_FS_onDefaultHddInfoChanged, "ssii",szOldUuid, szNewUuid, ulOldRec, ulNewRec);
}

#define _____RPC_FUNCTION_____

//	nArgc == 0
static HERROR umma_fs_rpc_Init(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	UMMA_ASSERT(nArgc == 0);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_INIT;
	pstSignal->ulParam1 = 0;
	pstSignal->ulParam2 = 0;
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}

//	apArgv[0] : deviceid (int)
//	apArgv[1] : volumelabel (str)
static HERROR	umma_fs_rpc_Format(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_t		*pstUmmaFs;

	HxLOG_Trace();
	UMMA_ASSERT(nArgc == 2);

	pstUmmaFs = umma_fs_getInstance();
	pstUmmaFs->bTryFormat	= TRUE;
	pstUmmaFs->nRequestId	= nRequestId;
	pstUmmaFs->ulDevId		= (HUINT32)HxOBJECT_INT(apArgv[0]);
	pstUmmaFs->ulVolLab 	= (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[1]));

	HxLOG_Debug("umma_fs_rpc_Format : devid(%d), vollab(%s), req(0x%x)\n", pstUmmaFs->ulDevId, pstUmmaFs->ulVolLab, pstUmmaFs->nRequestId);

	// UMMA Main task에 알림
	UMMA_SendEvent(eUMMA_EVT_STORAGE_FORMAT_START, HxOBJECT_INT(apArgv[0]), 0, 0);

	umma_fs_releaseInstance(pstUmmaFs);

	return ERR_OK;
}

//	apArgv[0] : deviceid (int)
static HERROR	umma_fs_rpc_Dst(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	UMMA_ASSERT(nArgc == 1);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_DST;
	pstSignal->ulParam1 = (HUINT32)HxOBJECT_INT(apArgv[0]);
	pstSignal->ulParam2 = 0;
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}

//	apArgv[0] : uuid (str)
static HERROR	umma_fs_rpc_SetDefaultHdd(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	UMMA_ASSERT(nArgc == 1);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_SET_DEFAULTHDD;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->ulParam2 = 0;
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}

//	apArgv[0] : uuid (str)
static HERROR	umma_fs_rpc_CheckUsbSpeed(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	UMMA_ASSERT(nArgc == 1);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_CHECK_USB_SPEED;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->ulParam2 = 0;
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}



//	apArgv[0] : uuid (str)
//	apArgv[1] : storagename (str)
static HERROR	umma_fs_rpc_SetName(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	UMMA_ASSERT(nArgc == 2);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_SET_NAME;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->ulParam2 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[1]));
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}

//	apArgv[0] : uuid (str)
static HERROR	umma_fs_rpc_DisableHdd(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	UMMA_ASSERT(nArgc == 1);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_DISABLE_PVR;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->ulParam2 = 0;
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}

//	apArgv[0] : uuid (str)
static HERROR	umma_fs_rpc_Pairing(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	UMMA_ASSERT(nArgc == 1);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_PAIRING;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->ulParam2 = 0;
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}

//	apArgv[0] : uuid (str)
static HERROR	umma_fs_rpc_UnPairing(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	UMMA_ASSERT(nArgc == 1);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_UNPAIRING;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->ulParam2 = 0;
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}


//	apArgv[0] : nDeviceId (i)
//	apArgv[1] : nForced (i)
static HERROR	umma_fs_rpc_SafeRemove(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	UMMA_ASSERT(nArgc == 2);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_SAFE_REMOVE;
	pstSignal->ulParam1 = (HUINT32)HxOBJECT_INT(apArgv[0]);
	pstSignal->ulParam2 = (HUINT32)HxOBJECT_INT(apArgv[1]);
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}

static HERROR	umma_fs_rpc_MainApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eFS_SIGNAL_APPREADY;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}

static HERROR	umma_fs_rpc_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	PxSTATUS_e		ePapiStatus;
	UMMA_SIGNAL_t	 *pstSignal;

	HxLOG_Assert(nArgc == 1);

	ePapiStatus = HxOBJECT_INT(apArgv[0]);
	HxLOG_Warning("********************************************\n");
	HxLOG_Warning("ePapiStatus[%d]\n", ePapiStatus);
	HxLOG_Warning("********************************************\n");

	HxLOG_Trace();
	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));

	if (ePapiStatus == ePAMA_STATUS_Shutdown)
	{
		pstSignal->ulSignal = eFS_SIGNAL_SHUTDOWN;
	}
	else if (ePapiStatus == ePAMA_STATUS_Operation)
	{
		pstSignal->ulSignal = eFS_SIGNAL_OPERATION;
	}
	else if (ePapiStatus == ePAMA_STATUS_Sync)
	{
		pstSignal->ulSignal = eFS_SIGNAL_SYNC;
	}
	else if (ePapiStatus == ePAMA_STATUS_ShutdownDone)
	{
		pstSignal->ulSignal = eFS_SIGNAL_SHUTDOWNDONE;
	}
	else if (ePapiStatus == ePAMA_STATUS_RebootDone)
	{
		pstSignal->ulSignal = eFS_SIGNAL_REBOOTDONE;
	}
	else
	{
		UMMA_Free(pstSignal);
		return ERR_OK;
	}

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fs_sendDummyEvent(s_pstUmmaFs);

	return ERR_OK;
}

#define ___________TASK___________________

static void		umma_fs_eventProcess(UMMA_t *pstUmma, UMMA_EVT_t *pstEvent)
{
	HUINT32		ulDeviceId;
	HINT32		nEvt, nP1, nP2, nP3;
	HERROR		hErr;

	if (!pstEvent)
	{
		HxLOG_Error("pstEvent is null.. \n");
		return;
	}

	nEvt = pstEvent->nEvent;
	nP1 = pstEvent->nParam1;
	nP2 = pstEvent->nParam2;
	nP3 = pstEvent->nParam3;

	ulDeviceId = (HUINT32)nP1;
	switch (nEvt)
	{
		case eFS_EVT_CONNECTED:
			{
				DxStorage_Info_t	*pstStorageInfo;

				HxLOG_Warning("eFS_EVT_CONNECTED \n");
				umma_fs_updateStorageList();

				pstStorageInfo = umma_fs_GetStorageInfo(ulDeviceId);
				if (pstStorageInfo == NULL)
					return;

				pstStorageInfo = (DxStorage_Info_t *)UMMA_MemDup(pstStorageInfo, sizeof(DxStorage_Info_t));

				UMMA_SendEvent(eUMMA_EVT_STORAGE_ADDED, ulDeviceId, 0, 0);

				umma_fs_onChangeDevice(pstUmma);
				umma_fs_onDeviceConnected(ulDeviceId, TRUE, pstStorageInfo, pstUmma);

				UMMA_Free(pstStorageInfo);
			}
			break;

		case eFS_EVT_DISCONNECTED:
			{
				DxStorage_Info_t	*pstStorageInfo;

				HxLOG_Warning("eFS_EVT_DISCONNECTED \n");
				pstStorageInfo = umma_fs_GetStorageInfo(ulDeviceId);
				if (pstStorageInfo == NULL)
				{
					/* Fast USB attach/detach or mount failure can have NULL storageInfo
					 * It cause event precedure conflict between CONNECTED and DISCONNECTED
					 */
					UMMA_SendEvent(eUMMA_EVT_STORAGE_REMOVED, ulDeviceId, 0, 0);
					umma_fs_RemoveStorageInfo(ulDeviceId);
					umma_fs_onChangeDevice(pstUmma);
					/* NULL storageInfo cause other process crash */
					/* umma_fs_onDeviceConnected(ulDeviceId, FALSE, NULL, pstUmma); */
					return;
				}

				pstStorageInfo = (DxStorage_Info_t *)UMMA_MemDup(pstStorageInfo, sizeof(DxStorage_Info_t));

				// unmount
				umma_fs_request_UnmountDevice(ulDeviceId);

				UMMA_SendEvent(eUMMA_EVT_STORAGE_REMOVED, ulDeviceId, (HINT32)UMMA_StrDup(pstStorageInfo->szMountPath), 0);

				umma_fs_RemoveStorageInfo(ulDeviceId);
				umma_fs_onChangeDevice(pstUmma);
				umma_fs_onDeviceConnected(ulDeviceId, FALSE, pstStorageInfo, pstUmma);

				UMMA_Free(pstStorageInfo);
			}
			break;

		case eFS_EVT_NEED_FORMAT:
			HxLOG_Warning("eFS_EVT_NEED_FORMAT \n");
			umma_fs_onNeedFormatEvent(ulDeviceId, pstUmma);
			break;

		case eFS_EVT_USB_OVERCURRENT:
			HxLOG_Warning("eFS_EVT_USB_OVERCURRENT \n");
			umma_fs_onOverCurrentEvent(ulDeviceId, pstUmma);
			break;

		case eFS_EVT_LIST_UPDATE:
			HxLOG_Warning("eFS_EVT_LIST_UPDATE \n");
			umma_fs_updateStorageList();
			umma_fs_onChangeDevice(pstUmma);
			break;

		case eFS_EVT_FORMAT_DONE:
			HxLOG_Warning("eFS_EVT_FORMAT_DONE \n");
			hErr = umma_fs_svc_PvrMountDevice((UMMA_FS_DevIdx_e)ulDeviceId);
			if (ERR_OK == hErr)
			{
				umma_fs_sendEvent(eFS_EVT_CONNECTED, ulDeviceId, 0, 0);
			}
			umma_fs_onFormatStateChange(ulDeviceId, 100, pstUmma);
			UMMA_SendEvent(eUMMA_EVT_STORAGE_FORMAT_DONE, ulDeviceId, 0, 0);
			break;

		case eFS_EVT_FORMAT_FAIL:
			HxLOG_Warning("eFS_EVT_FORMAT_FAIL \n");
			umma_fs_onFormatStateChange(ulDeviceId, -1, pstUmma);
			UMMA_SendEvent(eUMMA_EVT_STORAGE_FORMAT_DONE, ulDeviceId, 0, 0);
			break;

		case eFS_EVT_FORMATTING:
			HxLOG_Warning("eFS_EVT_FORMATTING \n");
			umma_fs_onFormatStateChange(ulDeviceId, nP2, pstUmma);
			break;

		case eFS_EVT_PVR_ENABLED:
			{
				HUINT32	ulDefaultPvrIdx;
				HxLOG_Warning("eFS_EVT_PVR_ENABLED \n");

				ulDefaultPvrIdx = umma_fs_svc_PvrGetDefaultPvrIdx();
				umma_fs_onPVRReady(ulDeviceId, TRUE, (ulDefaultPvrIdx == nP2)?TRUE:FALSE, nP3, pstUmma);
			}
			break;

		case eFS_EVT_PVR_DISABLED:
			HxLOG_Warning("eFS_EVT_PVR_DISABLED \n");
			umma_fs_onPVRReady(ulDeviceId, FALSE, nP2, nP3, pstUmma);
			break;

		case eFS_EVT_PLAYBACK_ENABLED:
			HxLOG_Warning("eFS_EVT_PLAYBACK_ENABLED \n");
			umma_fs_onPlaybackReady(ulDeviceId, TRUE, nP2, nP3, pstUmma);
			break;

		case eFS_EVT_PLAYBACK_DISABLED:
			HxLOG_Warning("eFS_EVT_PLAYBACK_DISABLED \n");
			umma_fs_onPlaybackReady(ulDeviceId, FALSE, nP2, nP3, pstUmma);
			break;

		case eFS_EVT_TSR_ENABLED:
			HxLOG_Warning("eFS_EVT_TSR_ENABLED \n");
			umma_fs_onTSRReady(ulDeviceId, TRUE, pstUmma);
			break;

		case eFS_EVT_TSR_DISABLED:
			HxLOG_Warning("eFS_EVT_TSR_DISABLED \n");
			umma_fs_onTSRReady(ulDeviceId, FALSE, pstUmma);
			break;

		case eFS_EVT_DST_DONE:
			HxLOG_Warning("eFS_EVT_DST_DONE \n");
			umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultOk, pstUmma);
			break;

		case eFS_EVT_DST_FAIL:
			HxLOG_Warning("eFS_EVT_DST_FAIL \n");
			switch ((UMMA_FS_DstResult_e)nP2)
			{
				case eFS_DST_HDD_NOT_DETECTED:						umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultFailNoHDD, pstUmma);		break;
				case eFS_DST_FAILED_TRY_AGAIN_LATER:				umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultFailTryAgain, pstUmma);	break;
				case eFS_DST_COMPLETED_FATAL_ERROR:					umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultFailFatalError, pstUmma);	break;
				case eFS_DST_COMPLETED_TEST_ELEMENT_FAILED:			umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultFailTestElementFailed, pstUmma);	break;
				case eFS_DST_COMPLETED_ELECTRICAL_ELEMENT_FAILED:	umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultFailElectricalElementFailed, pstUmma);	break;
				case eFS_DST_COMPLETED_SERVO_ELEMENT_FAILED:		umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultFailServoElementFailed, pstUmma);	break;
				case eFS_DST_COMPLETED_READ_ELEMENT_FAILED:			umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultFailReadElementFailed, pstUmma);	break;
				case eFS_DST_COMPLETED_HANDLING_DAMAGE:				umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultFailHadlingDamange, pstUmma);	break;
				case eFS_DST_COMPLETED_UNKNOWN:
				default:
					umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultFailTryAgain, pstUmma);	break;
			}
			break;
		case eFS_EVT_DST_ON_GOING:
			HxLOG_Warning("eFS_EVT_DST_ON_GOING \n");
			umma_fs_onDSTStateChange(ulDeviceId, eDxSTORAGE_DEV_STATUS_DstResultOnGoing, pstUmma); //(nP2 * 100) / nP3
			break;

		case eFS_EVT_UNMOUNT_FAIL:
			HxLOG_Warning("eFS_EVT_UNMOUNT_FAIL \n");
			umma_fs_onUmountFail(ulDeviceId, pstUmma);
			//디바이스에 대한 DB는 살아있기에, disconnected 이벤트를 강제로보내
			//FM 에게는 DB 삭제, APP 들에게는 디바이스 제거를 알려 접근을 막음
			umma_fs_sendEvent(eFS_EVT_DISCONNECTED, ulDeviceId, 0, 0);
			break;

		case eFS_EVT_CHECK_DISK_STARTED:
			HxLOG_Warning("eFS_MSG_CHECK_DISK_STARTED \n");
			umma_fs_onCheckDiskStarted(ulDeviceId, pstUmma);
			break;

		case eFS_MSG_SEND_RESULT:			//	p1 : ulDeviceId, p2 : sessionid, p3 : hErr;
			HxLOG_Warning("eFS_MSG_SEND_RESULT \n");
			HLIB_RPC_Notify(pstUmma->nRpcHandle, RPC_UMMA_FS_onResult, "ii", nP2, nP3);
			break;

		//Retry routine
		case eFS_MSG_SAFE_REMOVE:
		case eFS_MSG_UNMOUNT:
			HxLOG_Warning("%s \n", (nEvt == eFS_MSG_SAFE_REMOVE) ? "eFS_MSG_SAFE_REMOVE" :"eFS_MSG_UNMOUNT");
			(void)umma_fs_do_Unmount(0, pstEvent->nEvent, (UMMA_FS_DevIdx_e)pstEvent->nParam1);
			break;

		case eFS_MSG_CHECK_USB_SPEED_DONE:
			HxLOG_Warning("eFS_MSG_CHECK_USB_SPEED_DONE(ulDeviceId : %d, ReadMbps : %d, WriteMbps : %d\n", ulDeviceId, nP2, nP3);
			umma_fs_onCheckUsbSpeed(ulDeviceId, (HUINT32)nP2, (HUINT32)nP3, pstUmma);

			break;

		case	eFS_EVT_HDD_ADDED:
			HxLOG_Warning(" eFS_EVT_HDD_ADDED (eDevIdx : %d, nPortNum : %d)\n", nP1, nP2);
			umma_fs_hdd_AddedPostProcess((UMMA_FS_DevIdx_e)nP1, nP2);
			break;

		case	eFS_EVT_HDD_REMOVED:
			HxLOG_Warning("eFS_EVT_HDD_REMOVED (eDevIdx : %d, nPortNum : %d)\n", nP1, nP2);
			umma_fs_hdd_RemovedPostProcess((UMMA_FS_DevIdx_e)nP1, nP2);
			break;

		case	eFS_EVT_USB_ADDED:
			HxLOG_Warning("eFS_EVT_USB_ADDED\n");
			umma_fs_usb_AddedPostProcess(nP1, nP2, nP3);
			break;

		case	eFS_EVT_USB_REMOVED:
			HxLOG_Warning("eFS_EVT_USB_REMOVED\n");
			umma_fs_usb_RemovedPostProcess(nP1, nP2, nP3);
			break;

		case eFS_EVT_POWER_OFF_DONE:
			HxLOG_Warning("eFS_EVT_POWER_OFF_DONE\n");
			umma_fs_onStoragePowerOff(pstUmma);
			break;

		case eFS_EVT_DEFAULTHDD_INFO_CHANGED:
			{
				HCHAR *szOldDeviceUuid = (HCHAR *)nP1;
				HCHAR *szNewDeviceUuid = (HCHAR *)nP2;
				HUINT32	ulPackedRec = (HUINT32)nP3;

				HUINT32 ulOldRec = ulPackedRec>>16;
				HUINT32 ulNewRec = ulPackedRec&0xFFFF;

				umma_fs_onDefaultHddInfoChanged(szOldDeviceUuid,szNewDeviceUuid,ulOldRec,ulNewRec,pstUmma);

				if (szOldDeviceUuid != NULL){ UMMA_Free(szOldDeviceUuid); }
				if (szNewDeviceUuid != NULL){ UMMA_Free(szNewDeviceUuid); }
			}
			break;

		default:
			HxLOG_Critical("Unknown event!!! Check ignored event!!!(%d) \n", nEvt);
			break;
	}
}

static void		umma_fs_signal_Process(UMMA_t *pstUmma, UMMA_SIGNAL_t *pstSignal)
{
	HCHAR	*pszSourceDir = NULL;
	HERROR		hErr;
	HINT32	nRequestId;

	nRequestId = pstSignal->nReqId;

	switch (pstSignal->ulSignal)
	{
		case eFS_SIGNAL_APPREADY:
			break;

		case eFS_SIGNAL_DST:
			HxLOG_Warning("eFS_SIGNAL_DST (id : %d)\n", pstSignal->ulParam1);

			hErr = umma_fs_cmd_StartDst(pstSignal->ulParam1);
			umma_fs_onResult(nRequestId, hErr, pstUmma);

			break;

		case eFS_SIGNAL_SET_DEFAULTHDD:
			HxLOG_Warning("eFS_SIGNAL_SET_DEFAULTHDD (uuid : %s)\n", pstSignal->ulParam1);

			hErr = umma_fs_cmd_SetDefaultPVRStorage((HCHAR *)pstSignal->ulParam1);
			if (ERR_OK == hErr)
			{
				umma_fs_updateStorageList();
				umma_fs_onChangeDevice(pstUmma);
			}
			umma_fs_onResult(nRequestId, hErr, pstUmma);

			UMMA_Free((void *)pstSignal->ulParam1);

			break;

		case eFS_SIGNAL_CHECK_USB_SPEED:
			HxLOG_Warning("eFS_SIGNAL_CHECK_USB_SPEED (uuid : %s)\n", pstSignal->ulParam1);

			hErr = umma_fs_cmd_CheckUsbSpeed((HCHAR *)pstSignal->ulParam1);
			umma_fs_onResult(nRequestId, hErr, pstUmma);

			UMMA_Free((void *)pstSignal->ulParam1);

			break;


		case eFS_SIGNAL_SET_NAME:
			HxLOG_Warning("eFS_SIGNAL_SET_NAME (uuid : %s, name : %s)\n", pstSignal->ulParam1, pstSignal->ulParam2);
			hErr = umma_fs_cmd_SetStorageName((HCHAR *)pstSignal->ulParam1, (HCHAR *)pstSignal->ulParam2);

			if (ERR_OK == hErr)
			{
				umma_fs_updateStorageList();
				umma_fs_onChangeDevice(pstUmma);
			}
			umma_fs_onResult(nRequestId, hErr, pstUmma);

			UMMA_Free((void *)pstSignal->ulParam1);
			UMMA_Free((void *)pstSignal->ulParam2);
			break;

		case eFS_SIGNAL_DISABLE_PVR:
			HxLOG_Warning("eFS_SIGNAL_DISABLE_PVR (uuid : %s)\n", pstSignal->ulParam1);

			hErr = umma_fs_cmd_DisablePvrHdd((HCHAR *)pstSignal->ulParam1);
			if (ERR_OK == hErr)
			{
				umma_fs_updateStorageList();
				umma_fs_onChangeDevice(pstUmma);
			}
			umma_fs_onResult(nRequestId, hErr, pstUmma);

			UMMA_Free((void *)pstSignal->ulParam1);
			break;

		case eFS_SIGNAL_PAIRING:
			HxLOG_Warning("eFS_SIGNAL_PAIRING (uuid : %s)\n", pstSignal->ulParam1);

			hErr = umma_fs_cmd_PairingPvrHdd((HCHAR *)pstSignal->ulParam1);
			if (ERR_OK == hErr)
			{
				umma_fs_updateStorageList();
				umma_fs_onChangeDevice(pstUmma);
			}
			umma_fs_onResult(nRequestId, hErr, pstUmma);

			UMMA_Free((void *)pstSignal->ulParam1);

			break;

		case eFS_SIGNAL_UNPAIRING:
			HxLOG_Warning("eFS_SIGNAL_UNPAIRING (uuid : %s)\n", pstSignal->ulParam1);

			hErr = umma_fs_cmd_UnPairingPvrHdd((HCHAR *)pstSignal->ulParam1);
			if (ERR_OK == hErr)
			{
				umma_fs_updateStorageList();
				umma_fs_onChangeDevice(pstUmma);
			}
			umma_fs_onResult(nRequestId, hErr, pstUmma);

			UMMA_Free((void *)pstSignal->ulParam1);
			break;

		case eFS_SIGNAL_SAFE_REMOVE:
			HxLOG_Warning("eFS_SIGNAL_SAFE_REMOVE (id : %d, force : %d) \n", pstSignal->ulParam1, pstSignal->ulParam2);

			hErr = umma_fs_request_SafeRemoveDevice(pstSignal->ulParam1, pstSignal->ulParam2, nRequestId);

			break;

		case eFS_SIGNAL_FORMAT:
			HxLOG_Warning("eFS_SIGNAL_FORMAT (id : %d, name : %s)\n", pstSignal->ulParam1, pstSignal->ulParam2);

			THAPI_Quit();
			// unmount -> format
			hErr = umma_fs_request_FormatDevice(pstSignal->ulParam1, (HCHAR *)pstSignal->ulParam2, nRequestId);

			umma_fs_onResult(nRequestId, hErr, pstUmma);
			break;

		default:
			HxLOG_Warning("Unknown signal (%d) \n", pstSignal->ulSignal);
			break;
	}

	if (pszSourceDir)
		UMMA_Free((void *)pszSourceDir);

}


static void		umma_fs_mainLoop(void *arg)
{
	UMMA_t		*pstUmma;
	UMMA_EVT_t	stEvt;

	HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));

	while (1)
	{
		HBOOL bCheckSignal;
		// Start JOB wait
		pstUmma = umma_fs_getInstance();

		bCheckSignal = FALSE;
		if(stEvt.nEvent == 0 && pstUmma->pstSignalHandler)
		{
			bCheckSignal = TRUE;
		}

		if (bCheckSignal == TRUE)	// signal
		{
			UMMA_SIGNAL_t	*pstSignal;

			HxSEMT_Get(s_unSignalSema);
			pstSignal = (UMMA_SIGNAL_t*)HLIB_LIST_Data(s_pstUmmaFs->pstSignalHandler);
			s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Remove (s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
			HxSEMT_Release(s_unSignalSema);

			umma_fs_releaseInstance(pstUmma);

			if(pstSignal)
			{
				// UMMA_FS Job 시작과 관련된 signal
				if (pstSignal->ulSignal == eFS_SIGNAL_APPREADY)
				{
					if (s_bJobRunning != TRUE)
					{
						s_bJobRunning = TRUE;
						HxLOG_Warning("############ UMMA FS Job Init!!############\n");
						OAPI_PLATFORM_KeyInit();
						umma_fs_InitDevice();
						umma_fs_signal_Process(pstUmma, pstSignal);

						s_bDevPower = TRUE;	// svc_init으로 전원이 인가됨
					}
				}
				else if (pstSignal->ulSignal == eFS_SIGNAL_SHUTDOWN)
				{
					if (s_bJobRunning == FALSE && s_bDevPower == FALSE)
					{
						// booting시 바로 진입
						s_bJobRunning = TRUE;
						HxLOG_Warning("############ UMMA FS Job Init!!############\n");
						OAPI_PLATFORM_KeyInit();
						umma_fs_InitDevice();

						s_bDevPower = TRUE;	// svc_init으로 전원이 인가됨
					}
					else if (s_bJobRunning == TRUE && s_bDevPower == FALSE)
					{
						// booting시 바로 진입
						s_bJobRunning = TRUE;
						HxLOG_Warning("S3 mode booting ... On device power.\n");
						umma_fs_request_powerOn();
						s_bDevPower = TRUE;	// svc_init으로 전원이 인가됨
					}

					// else : nothing...
				}
				else if (pstSignal->ulSignal == eFS_SIGNAL_OPERATION)
				{
					if (s_bJobRunning == TRUE && s_bDevPower == FALSE)
					{
						// S3 mode booting --> On devices power
						HxLOG_Warning("S3 mode booting ... On device power.\n");
						umma_fs_request_powerOn();
						umma_fs_flash_MountPartition();
						s_bDevPower = TRUE;
					}
					else
					{
						//  booting시 바로 진입 등, eFS_SIGNAL_APPREADY 받기 전엔 동작하지 않는다.
					}
				}
				else if (pstSignal->ulSignal == eFS_SIGNAL_SYNC )
				{
					// Off devices power
					if(s_bDevPower == TRUE)
					{
						HERROR hErr;
						HxLOG_Warning("Off device power.....\n");
						PAPI_WaitForSync(30000);
						hErr = umma_fs_do_unmount_all_devices();
#if (1) /*rhgweon : workaround of S3 warmbooting issue */
						if (hErr == ERR_FAIL)
						{ /*rhgweon : Check Device is available or not. if HDD Device malfunction, this code prevents infinite rebooting*/
							VK_SYSTEM_Command("lsof | grep /mnt");
							VK_TASK_Sleep(1000);
							HxLOG_Error("HDD umount fail.... This System will reboot!!!!\n");
							VK_SYSTEM_Command("reboot");
						}
#endif

						umma_fs_request_powerOff();
						umma_fs_sendEvent(eFS_EVT_POWER_OFF_DONE, 0, 0, 0);

						umma_fs_dev_EraseAllDeviceInfo();
						umma_fs_flash_UnmountPartition();
						s_bDevPower = FALSE;
						PAPI_SyncDone();
					}
				}
				else if(pstSignal->ulSignal == eFS_SIGNAL_INIT )
				{
					HxLOG_Warning("eFS_SIGNAL_INIT \n");
					umma_fs_updateStorageList();
					umma_fs_onChangeDevice(pstUmma);
					umma_fs_onInit(pstUmma);
				}
				else if(s_bJobRunning == TRUE)
				{
					pstUmma = umma_fs_getInstance();

					// From RPC
					umma_fs_signal_Process(pstUmma, pstSignal);
					umma_fs_releaseInstance(pstUmma);
				}
				else
				{
					HxLOG_Warning("############ UMMA FS job not yet started!!(signal : %d)############\n", pstSignal->ulSignal);
				}

				UMMA_Free((void *)pstSignal);
			}
		}
		else	if (stEvt.nEvent != 0)// Event
		{
			pstUmma->pstEventList = HLIB_LIST_Append(pstUmma->pstEventList, UMMA_MemDup((void *)&stEvt, sizeof(UMMA_EVT_t)));
			umma_fs_releaseInstance(pstUmma);
		}
		else
		{
			umma_fs_releaseInstance(pstUmma);
		}


		if (s_bJobRunning == FALSE)
		{
			if (stEvt.nEvent != 0) {
				HxLOG_Warning("UMMA FS job not yet started...  keep event (%d)\n", stEvt.nEvent);
			}

			// wait msg (cmd, event)
			if (HLIB_MSG_Receive(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t)) == ERR_OK)
			{
				continue;
			}
		}
		else
		{
			// Event list 처리
			UMMA_EVT_t		*pstEvt;

			// From umma_fs
			pstUmma = umma_fs_getInstance();

			while(pstUmma->pstEventList)
			{
				pstEvt = (UMMA_EVT_t*)HLIB_LIST_Data(pstUmma->pstEventList);
				umma_fs_eventProcess(pstUmma, pstEvt);
				pstUmma->pstEventList = HLIB_LIST_Remove(pstUmma->pstEventList, (void *)pstEvt);
				UMMA_Free((void *)pstEvt);
			}

			umma_fs_releaseInstance(pstUmma);

			// wait msg (cmd, event)
			if (HLIB_MSG_ReceiveTimeout(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t), 1000)  == ERR_OK)
			{
				continue;
			}
			else
			{
				HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));
			}
		}
	}
}

#define ___________API____________
HERROR	UMMA_FS_Init(void)
{
	HERROR	err;
	UMMA_t		*pstUmma;

	HxLOG_Trace();
	HxSEMT_Create(&s_unSignalSema, "umma_fs_signal_sem", 0);
	pstUmma = umma_fs_getInstance();

	// API
	err  = HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FS_Init, NULL, umma_fs_rpc_Init, NULL,	"Activate the filemgr_storage module: (void)");
	err |= HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FS_Format, "is", umma_fs_rpc_Format, NULL,	"Format storage with (HINT32 nDevId, HCHAR *pszLabel)");
	err |= HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FS_Dst, "i", umma_fs_rpc_Dst, NULL, "DST storage with (HINT32 nDevId)");
	err |= HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FS_SetDefaultHdd, "s", umma_fs_rpc_SetDefaultHdd, NULL,	"Set default HDD (for PVR) (HCHAR *pszUUID)");
	err |= HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FS_SetName, "ss", umma_fs_rpc_SetName, NULL,	"Set Storage Name(HCHAR *pszUUID, HCHAR *pszStorageName)");
	err |= HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FS_DisableHdd, "s", umma_fs_rpc_DisableHdd, NULL,	"Disable PVR-HDD (HCHAR *pszUUID)");
	err |= HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FS_Pairing, "s", umma_fs_rpc_Pairing, NULL,	"Pairing HDD (HCHAR *pszUUID)");
	err |= HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FS_UnPairing, "s", umma_fs_rpc_UnPairing, NULL,	"UnPairing HDD (HCHAR *pszUUID)");
	err |= HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FS_SafeRemove, "ii", umma_fs_rpc_SafeRemove, NULL, "Try safe remove device (HINT32 nDevId, HINT32 nForced)");
	err |= HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FS_CheckUsbSpeed, "s", umma_fs_rpc_CheckUsbSpeed, NULL, "Check USB Speed (HCHAR *pszUUID)");

	// Event callback
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onInit, NULL, "Send to client complete of fs_init");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onDeviceConnected, "iib", "Storage is connected or disconnected(HUINT32 ulDeviceId, HBOOL bConnected)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onChangeDevice, "ib", "Storage devices are changed (attached / detached) (HINT32 nDeviceId, DxStorage_Info_t *pstStorageInfo)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onNeedFormat, "i", "Storage needs format first (HINT32 nDeviceId)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onOverCurrent, "i", "Storage is stopped cause over current (HINT32 nDeviceId)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onFormatStateChange, "ii", "Format state is changed (HINT32 nDeviceId, HINT32 nProgress)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onDSTStateChange, "ii", "DiskSelfTest state is changed (HINT32 nDeviceId, HINT32 nProgress)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onPVRReady,  "iiii", "PVR is ready event (HINT32 nDeviceId, HBOOL bPvr, HBOOL bDefault, HUINT32 ulPvrIdx)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onTSRReady,  "ii", "TSR-Only device is ready event (HINT32 nDeviceId, HBOOL bReady)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onPlaybackReady,   "iiii", "Playback is ready event (HINT32 nDeviceId, HBOOL bReady, HBOOL dummy, HUINT32 ulPlaybackIdx)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onUnmountFail, "i", "Unmount is failed(HUINT32 ulDeviceId)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onCheckDiskStarted, "i", "Check disk is started(HUINT32 ulDeviceId)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onCheckUsbSpeed, "iii", "Check usb speed(HUINT32 ulDeviceId, HUINT32 ulReadMbps, HUINT32 ulWriteMbps)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onStoragePowerOff, NULL, "Storage power off.");

	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onResult, "ii", "Result of RPC (HINT32 nRequestId, HERROR hErr)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FS_onDefaultHddInfoChanged, "ssii", "Default HDD's uid or recording capacity has changed");

	HAPI_ConnectSignal("signal:onMainApplicationReady", NULL, umma_fs_rpc_MainApplicationReady);
	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, umma_fs_rpc_onSystemStateChanged);

	SEUAPI_Init();
	umma_fs_config_Init();

	umma_fs_releaseInstance(pstUmma);

#if defined(CONFIG_DEBUG)
	umma_fs_svc_CmdInit();
#endif

	return err;
}

HERROR	UMMA_FS_GetStorageInfo(HUINT32 ulDeviceId, DxStorage_Info_t* pstStorageInfo)
{
	DxStorage_Info_t* pstInfo = NULL;
	UMMA_t	*pstUmmaFs;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Trace();

	if (!pstStorageInfo)
	{
		HxLOG_Error("Empty.. \n");
		return ERR_FAIL;
	}

	pstUmmaFs = umma_fs_getInstance();
	pstInfo = umma_fs_GetStorageInfo(ulDeviceId);
	if (pstInfo)
	{
		HxSTD_memcpy(pstStorageInfo, pstInfo, sizeof(DxStorage_Info_t));
		hErr = ERR_OK;
	}
	else
	{
		hErr = ERR_FAIL;
	}
	umma_fs_releaseInstance(pstUmmaFs);

	return hErr;
}

HERROR	UMMA_FS_FormatStroage(void)
{
	UMMA_t *pstUmmaFs;

	pstUmmaFs = umma_fs_getInstance();
	if (pstUmmaFs->bTryFormat)
	{
		UMMA_SIGNAL_t *pstSignal;

		// UMMA_FS task에 알림
		pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
		pstSignal->ulSignal = eFS_SIGNAL_FORMAT;
		pstSignal->ulParam1 = pstUmmaFs->ulDevId;
		pstSignal->ulParam2 = (HUINT32)pstUmmaFs->ulVolLab;
		pstSignal->ulParam3 = 0;
		pstSignal->nReqId   = pstUmmaFs->nRequestId;

		HxLOG_Debug("UMMA_FS_FormatStroage : devid(%d), vollab(%s), req(0x%x)\n",  pstUmmaFs->ulDevId, pstUmmaFs->ulVolLab, pstUmmaFs->nRequestId);

		pstUmmaFs->bTryFormat = FALSE;
		HxSEMT_Get(s_unSignalSema);
		s_pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFs->pstSignalHandler, (void*)pstSignal);
		HxSEMT_Release(s_unSignalSema);

		umma_fs_sendDummyEvent(pstUmmaFs);
	}

	umma_fs_releaseInstance(pstUmmaFs);
	return ERR_OK;
}

HERROR	UMMA_FS_GetStorageInfoByPath(const HCHAR *pszPath, DxStorage_Info_t* pstStorageInfo)
{
	DxStorage_Info_t* pstInfo = NULL;
	UMMA_t	        *pstUmmaFs;
    HERROR          hErr = ERR_FAIL;
	HxLOG_Trace();

	if (!pstStorageInfo)
	{
		HxLOG_Error("Empty.. \n");
		return ERR_FAIL;
	}

	pstUmmaFs = umma_fs_getInstance();
	pstInfo = umma_fs_GetStorageInfoByPath(pszPath);

    if (pstInfo)
	{
		HxSTD_memcpy(pstStorageInfo, pstInfo, sizeof(DxStorage_Info_t));
        hErr = ERR_OK;
	}
	umma_fs_releaseInstance(pstUmmaFs);

    return hErr;
}

HBOOL UMMA_FS_IsPvrFile(const HCHAR *pszPath)
{
	HBOOL bRet = FALSE;
	HCHAR *pszNomalpath = NULL;

	pszNomalpath = UMMA_StrDup(pszPath);
	HLIB_PATH_Normalize(pszNomalpath);

	if(HxSTD_StrStr(pszNomalpath,PVR_PATH_NORMAL_RECORD_INT) || HxSTD_StrStr(pszNomalpath,PVR_PATH_NORMAL_RECORD_EXT))
	{
		HCHAR pszMountPath[256]={0,};

		bRet = TRUE;
		if(HLIB_DIR_GetMountPath(pszNomalpath,pszMountPath,256) == ERR_OK)
		{
			HCHAR pszPvrPath[512]={0,};

			HLIB_PATH_Join(pszMountPath,PVR_PATH_NORMAL_RECORD_INT,pszPvrPath,512);
			if(HxSTD_StrCmp(pszPvrPath,pszNomalpath)== 0 ) // ignore same path
			{
				bRet = FALSE;
			}
			else
			{
				HxSTD_MemSet(pszPvrPath,0x0,sizeof(pszPvrPath));
				HLIB_PATH_Join(pszMountPath,PVR_PATH_NORMAL_RECORD_EXT,pszPvrPath,512);
				if(HxSTD_StrCmp(pszPvrPath,pszNomalpath)== 0 ) // ignore same path
					bRet = FALSE;
			}
		}
	}
	UMMA_Free(pszNomalpath);
	return bRet;
}

