/**
	@file     umma_fs_hdd.c
	@brief    repository service	\n
			  This implements rec/pb & media(photo/music) services.	\n

	Description: 		\n
	Module: 			 	\n
	Remarks : 										\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
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

#include <umma_int.h>

#include "umma_fs_int.h"

#include <uapi.h>
#include <hapi.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define WAITING_TIME_FOR_DETACHABLE_HDD		30000		//60초로 늘림, kernel에서 시간이 더 걸리는 경우도 있음 //30초로 늘림: 다른 init 작업으로 detachable 인식이 매우 늦어지는 경우도 발생하는 것으로 보임 // 5초간 2nd internal HDD (detachable HDD)가 발견되지 않으면 2nd int hdd는 power down 시킨다.


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HBOOL  s_bNew2ndHDD = FALSE;
static HUINT32	s_ulTimerId = 0;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define ________LOCAL_STATIC_FUNCTION________

STATIC void umma_fs_hdd_EventNotifier (HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	UMMA_FS_DevIdx_e			 eDevIdx;
	HUINT32					ulPortNum;

	switch (ulEvent)
	{
	case PAL_FS_EVENT_HDD_ON:
		HxLOG_Print("[umma_fs_hdd_EventNotifier] PAL_FS_EVENT_INT_HDD_ON (%d:%d:%d)\n", ulParam1, ulParam2, ulParam3);
		eDevIdx	 = umma_fs_dev_ConvertPalDevIdxToMwDevIdx ((PAL_FS_StorDevIdx_t)ulParam1);
		ulPortNum = ulParam2;
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_HDD_ADDED, (HINT32)eDevIdx, (HINT32)ulPortNum, (HINT32)ulParam3);
		break;

	case PAL_FS_EVENT_HDD_OFF:
		HxLOG_Print("[umma_fs_hdd_EventNotifier] PAL_FS_EVENT_INT_HDD_OFF (%d:%d:%d)\n", ulParam1, ulParam2, ulParam3);
		eDevIdx	 = umma_fs_dev_ConvertPalDevIdxToMwDevIdx ((PAL_FS_StorDevIdx_t)ulParam1);
		ulPortNum = ulParam2;
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_HDD_REMOVED, (HINT32)eDevIdx, (HINT32)ulPortNum, (HINT32)ulParam3);
		break;

	default:
		HxLOG_Critical("\n Unknown event (ulEvent : %d)\n", ulEvent);
		break;
	}

}

static void umma_fs_hdd_DetachableTimerCallback(unsigned long nTimerId,  void *pArg)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	if (s_ulTimerId != nTimerId)
	{
		return;
	}
	UMMA_FS_PAL_HDD_Check2ndIntHdd();

	s_ulTimerId = 0;
#else
	return;
#endif
}

static void	umma_fs_hdd_SetDetachableCheckTimer(void)
{
	// 일정 시간 연결이 안되면 전원 Off를 위해 timer를 돌린다.
	VK_TIMER_EventAfter(WAITING_TIME_FOR_DETACHABLE_HDD, umma_fs_hdd_DetachableTimerCallback, NULL, 0, (unsigned long *)&s_ulTimerId);
}
STATIC HERROR umma_fs_hdd_MountIntHddStorage (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_FormatPartList_t *pstFormatList,HBOOL bAfterFormat)
{
	HBOOL				bSystemFlagFound = FALSE;
	HUINT32				ulPartIdx, ulPvrIdx;
	UMMA_FS_FormatPartInfo_t	*pstFormatItem;
	UMMA_FS_PartitionInfo_t	*pstPartInfo;
	UMMA_FS_PartitionInfo_t	*pstDefaultPvrPartition = NULL;
	HERROR			 	 hErr;
	HUINT8		 		szDeviceUuid[VOLUUIDSZ + 1];
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	HCHAR		szDevName[32];
	HUINT64		ullTotalSizeKb = 0;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		return ERR_FAIL;
	}

	for (ulPartIdx = 0; ulPartIdx < pstFormatList->ulPartitionNum; ulPartIdx++)
	{
		pstFormatItem	= &(pstFormatList->astPartArray[ulPartIdx]);
		pstPartInfo		= NULL;

		hErr = umma_fs_dev_MountPartition (eDevIdx, ulPartIdx, pstFormatItem->bSecured, pstFormatItem->szMountPath, &pstPartInfo); // MOON_encr
		if (hErr == ERR_OK && pstPartInfo != NULL)
		{
			// [HMS] 원래 drive No는 external에서만 쓰는데, internal/detachable 구분을 위해서 사용
			umma_fs_dev_GetNewDriveNo (&pstPartInfo->ulDriveNo);

			pstPartInfo->bPvrFlag		= FALSE;
			pstPartInfo->bSystemFlag	= FALSE;

			if (pstFormatItem->ePartType == eFS_PvrPartitionType_PvrMedia)
			{
				pstPartInfo->bPvrFlag = TRUE;

				//PvrIdx를 할당한다.
				if(umma_fs_svc_PvrIsPvrStorageRegistrable() == TRUE)
				{
					hErr = umma_fs_svc_PvrAssignPvrIdx(&ulPvrIdx);
					if (hErr == ERR_OK)
					{
						pstPartInfo->ulPvrIdx = ulPvrIdx;
					}
					VK_memset(szDeviceUuid, 0, VOLUUIDSZ + 1);

					if (pstDevice->eDevType == eFS_DevType_IntHdd)	// internal만 default PVR로 설정
					{
						HBOOL	bSync = FALSE;

						hErr = umma_fs_svc_PvrCheckDeviceIsDefaultPvrHdd(eDevIdx);
						if (hErr != ERR_OK)
						{
							bSync = TRUE;		 //포멧을 했다면 UUID가 달라서 bSync TRUE가 될것이다.
						}
						umma_fs_svc_PvrSetPvrDefaultHdd(ulPvrIdx, bSync);
						pstDefaultPvrPartition = pstPartInfo;
					}
#ifdef UMMA_CHECK_AVAILABLE_MOUNTPATH
					{
						HUINT32					 ulTrialCnt;
						HUINT32					 ulCheckStartTick;
						HCHAR					 szSubPath[UMMA_FS_MOUNT_PATH_LEN] = {0,};

						ulTrialCnt = 0;
						ulCheckStartTick = HLIB_STD_GetSystemTick();

						hErr = umma_fs_svc_DirGetPathFromMountedRootPath(pstPartInfo->szMountPath, pstDevice->eDevType,eFS_UsageType_Record, szSubPath, UMMA_FS_MOUNT_PATH_LEN-1);
						if(hErr == ERR_OK && HxSTD_StrEmpty(szSubPath) == FALSE)
						{
							HLIB_PATH_Normalize(szSubPath);
							while(HLIB_DIR_IsExist(szSubPath) == FALSE)
							{
								HxLOG_Error("Retry Count[%d] Path[%s]\n",ulTrialCnt,szSubPath);
								if(HLIB_STD_GetSystemTick() > ulCheckStartTick + UMMA_FS_CHECK_TIMEOUT)
								{
									HxLOG_Error("Retry Fail..Count[%d] Path[%s]\n",ulTrialCnt,szSubPath);
									break;
								}
								VK_TASK_Sleep(20);
								ulTrialCnt++;
							}
						}
					}
#endif
					umma_fs_svc_DirMakeDefaultDirectory(eDevIdx, ulPartIdx);
					umma_fs_svc_PvrGetAllPvrPath(pstDevice->eDevType, pstPartInfo);
				}
			}
			else if (pstFormatItem->ePartType == eFS_PvrPartitionType_System)
			{
				bSystemFlagFound = TRUE;
				pstPartInfo->bSystemFlag = TRUE;
			}
		}
	}

	if ((TRUE != bSystemFlagFound) && (NULL != pstDefaultPvrPartition))
	{
		pstDefaultPvrPartition->bSystemFlag = TRUE;
	}

	umma_fs_dev_GetDeviceFileFullName(eDevIdx, szDevName);
	hErr = umma_fs_dev_GetDevSize(szDevName, &ullTotalSizeKb);
	if (hErr == ERR_OK)
	{
		pstDevice->ullDevSizeKb = ullTotalSizeKb;
	}

	return ERR_OK;
}

STATIC HERROR umma_fs_hdd_CheckHddStorage (UMMA_FS_DevIdx_e eDevIdx, UMMA_FS_FormatPartList_t *pstFormatList)
{
	HUINT32			 ulCount, ulPartIdx;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR			 hErr;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("[umma_fs_hdd_CheckHddStorage] umma_fs_dev_GetDeviceInfo err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	// 최소한 Format List (PVR HDD 관련으로 잡아줘야 하는  partition 정보) 내의 내용보다 실제 partition이 적으면 안된다.
	if (pstFormatList->ulPartitionNum <= pstDevice->ulNumPartition)
	{
		// Format List에 기록되어 있는 partition 들 만큼은 제대로 mount 되어 있어야 한다.
		for (ulCount = 0; ulCount < pstFormatList->ulPartitionNum; ulCount++)
		{
			ulPartIdx = pstFormatList->astPartArray[ulCount].ulPartIdx;
			if ((ulPartIdx >= pstDevice->ulNumPartition) ||
				(pstDevice->pstPartArray[ulPartIdx].bAvailable == FALSE) ||
				(pstDevice->pstPartArray[ulPartIdx].bMounted == FALSE))
			{
				pstDevice->eDevStatus = eFS_DevStatus_NeedFormat;
				return ERR_FAIL;
			}
		}
	}
	else
	{
		pstDevice->eDevStatus = eFS_DevStatus_NeedFormat;
		return ERR_FAIL;
	}

	pstDevice->eDevStatus = eFS_DevStatus_OK;
	return ERR_OK;
}

#define ____EXTERNAL_FUNCTION______


HERROR umma_fs_hdd_PowerOnAll(void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	return UMMA_FS_PAL_HDD_PWR_On();
#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_hdd_PowerOffAll(void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	return UMMA_FS_PAL_HDD_PWR_Off();
#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_hdd_PowerOn(UMMA_FS_DevType_e eType)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	PAL_HDD_PortNum_t	ePortNum = PAL_HDD_PORT_NUM_NONE;

	// Select device
	if (eType == eFS_DevType_IntHdd)
	{
		ePortNum = PAL_HDD_PORT_NUM_1;
	}
	else if (eType == eFS_DevType_Detachable)
	{
		ePortNum = PAL_HDD_PORT_NUM_2;
		umma_fs_hdd_SetDetachableCheckTimer();
	}
	else if (eType == eFS_DevType_eSataHdd)
	{
		ePortNum = PAL_HDD_PORT_NUM_3;
	}
	else if (eType == eFS_DevType_ALL)
	{
		ePortNum = PAL_HDD_PORT_NUM_MAX;
	}

	if (ePortNum != 0)
	{
		// Power on
		if (ePortNum != PAL_HDD_PORT_NUM_MAX)
		{
			return UMMA_FS_PAL_HDD_ControlPower(ePortNum, TRUE);
		}
		else
		{
			// All device
			return UMMA_FS_PAL_HDD_PWR_On();
		}
	}
	return ERR_FAIL;

#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_hdd_PowerOff(UMMA_FS_DevType_e eType)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	PAL_HDD_PortNum_t	ePortNum = PAL_HDD_PORT_NUM_NONE;

	if (eType == eFS_DevType_IntHdd)
	{
		ePortNum = PAL_HDD_PORT_NUM_1;
	}
	else if (eType == eFS_DevType_Detachable)
	{
		ePortNum = PAL_HDD_PORT_NUM_2;
	}
	else if (eType == eFS_DevType_eSataHdd)
	{
		ePortNum = PAL_HDD_PORT_NUM_3;
	}
	else if (eType == eFS_DevType_ALL)
	{
		ePortNum = PAL_HDD_PORT_NUM_MAX;
	}

	// Power off
	if (ePortNum != 0)
	{
		// Power on
		if (ePortNum != PAL_HDD_PORT_NUM_MAX)
		{
			return UMMA_FS_PAL_HDD_ControlPower(ePortNum, FALSE);
		}
		else
		{
			// All device
			return UMMA_FS_PAL_HDD_PWR_Off();
		}
	}
	return ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_hdd_Init(void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	UMMA_FS_PAL_HDD_RegisterCallback (umma_fs_hdd_EventNotifier);
	UMMA_FS_PAL_HDD_Init();

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


HERROR umma_fs_hdd_MountAndCheckIntHdd (UMMA_FS_DevIdx_e eDevIdx,HBOOL bAfterFormat)
{
	UMMA_FS_FormatPartList_t	*pstFormatList = NULL;
	HERROR				hErr, hResult = ERR_FAIL;

	// 2. Mount 하려는 방식 등을 알아와야 mount가 가능하다.
	// mount 방식 (path 등)은 FS_PART_GetHddPartitionSetting 에서 통일적으로 다룬다.
	pstFormatList = UMMA_Malloc (sizeof(UMMA_FS_FormatPartList_t));
	if (pstFormatList == NULL)
	{
		HxLOG_Error("[umma_fs_hdd_MountAndCheckIntHdd] UMMA_Malloc (pstFormatList) Failed\n");
		goto ERROR;
	}

	hErr = umma_fs_svc_DirGetHddPartitionSetting (eDevIdx, "", pstFormatList);
	if (hErr != ERR_OK || pstFormatList->ulPartitionNum == 0)
	{
		HxLOG_Error("[umma_fs_hdd_MountAndCheckIntHdd] FS_PART_GetHddPartitionSetting err(0x%08x)\n", hErr);
		goto ERROR;
	}

	umma_fs_dev_ReinitPartitionInfo(eDevIdx, pstFormatList, FALSE);
	// 3. 실제로 마운트 한다.
	umma_fs_hdd_MountIntHddStorage (eDevIdx, pstFormatList,bAfterFormat);

	// 마운트 후 실제 마운트 내용을 체크해서 제대로 마운트 되었는지 확인한다.
	hErr = umma_fs_hdd_CheckHddStorage (eDevIdx, pstFormatList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[umma_fs_hdd_MountAndCheckIntHdd] umma_fs_hdd_CheckHddStorage err(0x%08x)\n", hErr);
		goto ERROR;
	}

	hResult = ERR_OK;

ERROR:
	if (pstFormatList != NULL)				{ UMMA_Free (pstFormatList); }
	return hResult;
}


HERROR umma_fs_hdd_AddedPostProcess(UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPortNum)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	HERROR			hErr,hErrPairable=ERR_FAIL;
	HBOOL			bNeedFormat = FALSE;
	HUINT32 		ulPvrIdx;
	UMMA_FS_PvrType_e ePvrType = eFS_PVR_None;

	HxLOG_Trace();
	HxLOG_Print("[%s] [%d] eDevIdx(%d) \n",__FUNCTION__,__LINE__, eDevIdx);

	// Device를 connect 작업한다.
	hErr = umma_fs_dev_ConnectNewDevice (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("[umma_fs_hdd_ProcessEventIntHddOn] umma_fs_dev_ConnectNewDevice (%d) err(0x%08x)\n", eDevIdx, hErr);
		return ERR_FAIL;
	}

	// device type
	if (ulPortNum == PAL_HDD_PORT_NUM_1)	{	pstDevice->eDevType = eFS_DevType_IntHdd;		}
	else	if (ulPortNum == PAL_HDD_PORT_NUM_2)	{	pstDevice->eDevType = eFS_DevType_Detachable;		}
	else	if (ulPortNum == PAL_HDD_PORT_NUM_3)	{	pstDevice->eDevType = eFS_DevType_eSataHdd;		}
	else
	{
		HxLOG_Error("Not supported ulPortNum!!! (%d) \n", ulPortNum);
		return ERR_FAIL;
	}
	HxLOG_Warning("eDevType : %d \n", pstDevice->eDevType);

	if (pstDevice->eDevType == eFS_DevType_Detachable)
	{
		// detachable HDD가 detect 안된 경우에 power down 시키는 처리를 위해서 pal단에 detect 결과를 알려줘야 한다.
		UMMA_FS_PAL_HDD_2ndIntHddDetected(TRUE);
	}
	hErr = umma_fs_hdd_MountAndCheckIntHdd (eDevIdx,FALSE);
	if (hErr == ERR_OK)
	{
		if (pstDevice->eDevType == eFS_DevType_Detachable)
		{
			 //처음 연결된 HDD인지 확인
			 hErr = umma_fs_svc_PvrCheckRegisteredPvr(eDevIdx);
			 if (hErr != ERR_OK)
			 {
				umma_fs_hdd_SetNewDetachableHDD(TRUE);
				umma_fs_svc_WritePvrRegisterInfoToHDD(eDevIdx, INT2NDPVR_PARTIDX);
			 }
			 else
			 {
				 umma_fs_hdd_SetNewDetachableHDD(FALSE);
			 }
		}
		else if (pstDevice->eDevType == eFS_DevType_eSataHdd)
		{
			// 해당 모델이 지원하는 이 Device의 PVR type을 가져온다.
			ePvrType = umma_fs_config_GetPvrType(eFS_DevType_eSataHdd);

			// record (=PVR)
			if ((ePvrType & eFS_PVR_Main) || (ePvrType & eFS_PVR_Sub) || (ePvrType & eFS_PVR_Tsr))	// Main or Sub or TSR
			{
				HBOOL	bOk;
				bOk = umma_fs_svc_PvrIsPvrStorageRegistrable();
				if(bOk == TRUE)
				{
					hErrPairable=umma_fs_svc_PvrCheckDeviceIsExtPvrPaired (eDevIdx, FALSE, UMMA_FS_EXT_PVR_PART_IDX);
					if(hErrPairable != ERR_OK)
					{
						HxLOG_Error("umma_fs_svc_PvrCheckDeviceIsExtPvrPaired eDevIdx(%d) error!\n",eDevIdx);
					}
				}
			}

			// playback
			if (ePvrType & eFS_PVR_RecStorage)
			{
				HBOOL		bPlaybackStorageRegistrable  = umma_fs_svc_PvrIsPlaybackStorageRegistrable();
				if (bPlaybackStorageRegistrable)
				{
					(void)umma_fs_svc_PvrMakePlaybackInfo(eDevIdx,UMMA_FS_EXT_PVR_PART_IDX);
				}
			}
		}
	}
	else
	{
		if (pstDevice->eDevType == eFS_DevType_Detachable)
		{
			umma_fs_hdd_SetNewDetachableHDD(TRUE);
		}
		bNeedFormat = TRUE;
	}

	(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_CONNECTED, eDevIdx, 0, 0);

	hErr = umma_fs_svc_PvrGetPvrIdxByDevIdx(&ulPvrIdx, eDevIdx);
	if (hErr == ERR_OK)
	{
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_PVR_ENABLED, (HINT32)eDevIdx, (HINT32)0, (HINT32)ulPvrIdx);
	}

	if (ePvrType & eFS_PVR_RecStorage)
	{
		HUINT32 	ulPlaybackIdx = DxSTORAGE_INVALID_PVR_INDEX;

		hErr = umma_fs_svc_PvrGetPlaybackIdxByDevIdx(&ulPlaybackIdx, eDevIdx);
		if (ERR_OK == hErr)
		{
			(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_PLAYBACK_ENABLED, (HINT32)eDevIdx, (HINT32)0, (HINT32)ulPlaybackIdx);
		}
	}

	if (bNeedFormat)
	{
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_NEED_FORMAT, eDevIdx, 0, 0);
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_hdd_RemovedPostProcess(UMMA_FS_DevIdx_e eDevIdx, HUINT32 ulPortNum)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	UMMA_FS_DevContext_t			*pstDevice = NULL;
	HERROR					 hErr;
	HUINT32 			ulPartIdx, ulPvrIdx=0xff, ulCount=0;
	UMMA_FS_PartitionInfo_t	*pstPartInfo;
	HBOOL				bDefaultPvr = FALSE;
	HUINT8				szMountPath[UMMA_FS_MOUNT_PATH_LEN];
	HBOOL			bSafeRemoved = FALSE;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("[umma_fs_svc_PvrProcessEventIntHddOff] umma_fs_dev_ConnectNewDevice (%d) err(0x%08x)\n", eDevIdx, hErr);
		return ERR_FAIL;
	}

	pstDevice->bDevConnected	= FALSE;

	for (ulPartIdx = 0; ulPartIdx < pstDevice->ulNumPartition; ulPartIdx++)
	{
		pstPartInfo = &(pstDevice->pstPartArray[ulPartIdx]);

		if(pstPartInfo->bPvrFlag == TRUE)
		{
			ulPvrIdx = pstPartInfo->ulPvrIdx;
			bDefaultPvr = pstPartInfo->bDefaultPvr;
			HxSTD_StrNCpy(szMountPath, pstPartInfo->szMountPath, UMMA_FS_MOUNT_PATH_LEN);
		}

		// MOON_encr
#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
		if (pstPartInfo->bSecured == TRUE)
		{
			PAL_FS_ENCR_Close(eDevIdx, ulPartIdx);
			pstPartInfo->bSecured = FALSE;
		}
#endif
	}
	if(pstDevice->eDevStatus == eFS_DevStatus_SafeRemoved)
	{
		bSafeRemoved = TRUE;
	}

	if (pstDevice->bDevConnected == FALSE && pstDevice->bDevMounted == FALSE)
	{
		umma_fs_dev_InitFsDeviceInfo (pstDevice);
		HxLOG_Print("[%s:%d] Set Session [%d]\n",__FUNCTION__,__LINE__, eDevIdx);
	}

	HxLOG_Print("[umma_fs_svc_PvrProcessEventIntHddOff] EVT_FS_INT_HDD_UNPLUGGED (%d) >>>\n", eDevIdx);
	if (!bSafeRemoved)	 //이미 umount 되었으니 올릴필요 없다.
	{
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_DISCONNECTED, eDevIdx, 0, 0);
	}

	if (pstDevice->eDevType == eFS_DevType_Detachable)
	{
		// detachable HDD가 제거된 경우 power down 시키는 처리를 위해서 pal단에 detect 결과를 알려줘야 한다.
		UMMA_FS_PAL_HDD_2ndIntHddDetected(FALSE);
	}

	if(ulPvrIdx != 0xff)
	{
		umma_fs_svc_PvrReleasePvrIdx(ulPvrIdx);

		 //Off된 HDD가 default PVR HDD였다면 남은 연결된 HDD 중에서 default 를 선택한다.
		if(bDefaultPvr == TRUE)
		{
			umma_fs_svc_PvrGetConnectedPvrStorageCount(&ulCount);
			if(ulCount != 0)	// 남은게 없으면 진행X
			{
				hErr = umma_fs_svc_PvrSelectNewDefaultPvrIdx(&ulPvrIdx);
				if(hErr == ERR_OK)
				{
					umma_fs_svc_PvrSetPvrDefaultHdd(ulPvrIdx, FALSE);
				}
			}
		}
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


HBOOL	umma_fs_hdd_CheckIsNewDetachableHDD(void)
{
	return s_bNew2ndHDD;
}

HERROR	umma_fs_hdd_SetNewDetachableHDD(HBOOL bFlag)
{
	s_bNew2ndHDD = bFlag;

	return ERR_OK;
}



/*********************** End of File ******************************/
