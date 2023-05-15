/**
	@file     umma_fs_usb.c
	@brief    repository service	\n
			  This implements rec/pb & media(photo/music) services.	\n

	Description: 		\n
	Module: 			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
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




/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	UMMA_FS_DevIdx_e	eDevIdx;
	HUINT32			ulPartitionIdx;
	UMMA_FS_FsType_e	eMwFSType;
	HUINT32			ulDiDevIdx;
	HUINT32			ulPortNum;
	HUINT32			ulAttr;
	HCHAR			szChkFileName[50];
} FS_USB_CHKDISK_Info_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static 	HUINT32		s_ulCheckDiskTaskId = 0;
static 	HUINT32		s_ulCheckDiskMsgId = 0;
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC HERROR umma_fs_usb_MsgEvtUsbScsiAdded (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

#define ________LOCAL_FUNCTION________

STATIC HBOOL umma_fs_usb_IsUsbType (UMMA_FS_DevContext_t *pstDevice)
{
	if (pstDevice != NULL)
	{
		if (pstDevice->eDevType == eFS_DevType_UsbHdd ||
			pstDevice->eDevType == eFS_DevType_UsbMem)
		{
			return TRUE;
		}
	}

	return FALSE;
}

static void umma_fs_usb_GetUsbSpeed(HUINT32 ulAttribute, UMMA_FS_DevUsbSpeed_e *eUsbSpeed)
{
	HUINT32 ulSpeed, ulOcp;

	PAL_FS_USB_GetSpeedAndOcp (ulAttribute, &ulSpeed, &ulOcp);

	switch(ulSpeed)
	{
		case eSCSI_LowSpeed:
			*eUsbSpeed = eFS_DevUsbSpeed_1M;
			break;
		case eSCSI_HighSpeed:
			*eUsbSpeed = eFS_DevUsbSpeed_12M;
			break;
		case eSCSI_HighSpeed_480M:
			*eUsbSpeed = eFS_DevUsbSpeed_480M;
			break;
		case eSCSI_HighSpeed_5G:
			*eUsbSpeed = eFS_DevUsbSpeed_5G;
			break;
		default:
			*eUsbSpeed = eFS_DevUsbSpeed_NONE;
			break;
	}
}

static void umma_fs_usb_CheckDiskTask(void *pParam)
{
	HERROR hErr;
	FS_USB_CHKDISK_Info_t stChkDskInfo;
	PAL_FS_FsType_t 	ePalFsType;
	UNIXTIME	ulStartTime = 0, ulEndTime = 0;

	while (1)
	{
		hErr = VK_MSG_Receive (s_ulCheckDiskMsgId, &stChkDskInfo, sizeof (FS_USB_CHKDISK_Info_t));
		if (VK_OK != hErr)					{ continue; }


		hErr = VK_CLOCK_GetTime((unsigned long *)&ulStartTime);
		if(hErr != ERR_OK)
			ulStartTime = 0;

		ePalFsType = umma_fs_dev_ConvertMwFsTypeToPalFsType(stChkDskInfo.eMwFSType);

		HxLOG_Print("[%s] [%d]	\n",__FUNCTION__,__LINE__);
		PAL_FS_PartitionCheckDisk(stChkDskInfo.eDevIdx, stChkDskInfo.ulPartitionIdx, ePalFsType);

		hErr = VK_CLOCK_GetTime((unsigned long *)&ulEndTime);
		if(hErr != ERR_OK)
			ulEndTime = 0;

		if(ulStartTime != 0 && ulEndTime != 0 && ulStartTime <= ulEndTime && ulStartTime + 5 > ulEndTime)
		{
			VK_TASK_Sleep((5 -(ulEndTime - ulStartTime)) * 1000);
		}

		SVC_FS_SEM_GET;

		umma_fs_usb_MsgEvtUsbScsiAdded (stChkDskInfo.ulDiDevIdx, stChkDskInfo.ulPortNum, stChkDskInfo.ulAttr);
		SVC_FS_SEM_RELEASE;

	}
}


#define _____USB_EVENT_NOTIFIERS_____

STATIC HERROR umma_fs_usb_MsgEvtUsbScsiAdded (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HUINT32				 	ulPortNo = ulParam2;
	UMMA_FS_DevIdx_e		eDevIdx;
	UMMA_FS_DevContext_t	*pstDevice = NULL;
	HERROR					hErr,hErrPairable=ERR_FAIL;
	HBOOL					bMount;
	FS_USB_CHKDISK_Info_t 	stChkDskInfo;
	static HINT32			ulLastChkDevIdx = -1;
	HUINT8					szFileName[50] = {0,};
	PAL_FS_UINT32	 		ulExtFileDsc, ulSize = 0;

	HBOOL					bOk;
	HUINT32 				ulPvrIdx = 0xFFFF;
	UMMA_FS_PvrType_e 		ePvrType;
	UMMA_FS_DevUsbSpeed_e	eUsbSpeed;

	eDevIdx = umma_fs_dev_ConvertPalDevIdxToMwDevIdx (ulParam1);
	if (eDevIdx < UMMA_FS_DEV_IDX_START || eDevIdx > UMMA_FS_DEV_IDX_END)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	hErr = umma_fs_dev_ConnectNewDevice (eDevIdx, &pstDevice);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	pstDevice->ulPortNum = ulPortNo;

	/* Get Usb Speed type (1.1 or 2.0 or 3.0) by attribute */
	umma_fs_usb_GetUsbSpeed(ulParam3, &eUsbSpeed);
	pstDevice->eDevUsbSpeed = eUsbSpeed;

	hErr = umma_fs_usb_MountDevice (eDevIdx, FALSE);
	if (hErr == ERR_OK)
	{
		pstDevice->eDevStatus = eFS_DevStatus_OK;

		HxSTD_PrintToStrN(szFileName, 50, "%s%s", pstDevice->pstPartArray[UMMA_FS_EXT_PVR_PART_IDX].szMountPath, UMMA_FS_SAFELY_REMOVE_CHK_FILE);
		if(HLIB_FILE_Exist(szFileName) == TRUE && ulLastChkDevIdx != eDevIdx)
		{
			/* 즉시 삭제를 위해 remove 함수 사용 */
			remove(szFileName);

			if(umma_fs_dev_UnmountPartition(eDevIdx, UMMA_FS_EXT_PVR_PART_IDX) == ERR_OK)
			{
				pstDevice->bDevMounted = FALSE;

				ulLastChkDevIdx = eDevIdx;
				stChkDskInfo.eDevIdx = eDevIdx;
				stChkDskInfo.ulPartitionIdx = UMMA_FS_EXT_PVR_PART_IDX;
				stChkDskInfo.eMwFSType = pstDevice->pstPartArray[UMMA_FS_EXT_PVR_PART_IDX].ePartFsType;
				stChkDskInfo.ulDiDevIdx = ulParam1;
				stChkDskInfo.ulPortNum= ulParam2;
				stChkDskInfo.ulAttr = ulParam3;
				HxSTD_StrNCpy(stChkDskInfo.szChkFileName, szFileName, HxSTD_StrLen(szFileName));

				if (VK_MSG_Send(s_ulCheckDiskMsgId, &stChkDskInfo, sizeof(FS_USB_CHKDISK_Info_t)) == ERR_OK)
				{
#if 0 // "unsafely removed" message is displayed too often, so block this message.
					umma_fs_svc_SendEventToMainloop(eFS_EVT_CHECK_DISK_STARTED, eDevIdx, 0, 0);
#else
					HxLOG_Debug ("checkdisk msg sent \n");
#endif
				}
				else
				{
					HxLOG_Error ("Error to send message \n");
				}
				return ERR_FAIL;

			}
		}
		else
		{
			/* Safely remove 확인을 위한 파일 생성 */
			HxSTD_PrintToStrN(szFileName, 50, "%s%s", pstDevice->pstPartArray[UMMA_FS_EXT_PVR_PART_IDX].szMountPath, UMMA_FS_SAFELY_REMOVE_CHK_FILE);
			ulExtFileDsc = 0;
			if(HLIB_FILE_Exist(szFileName) == FALSE)
			{
				hErr = PAL_FS_Open(0, szFileName, PAL_FS_MODE_RDWR, &ulExtFileDsc);
				if (hErr == ERR_OK && ulExtFileDsc != 0)
				{
					PAL_FS_Write(ulExtFileDsc, UMMA_FS_SAFELY_REMOVE_CHK_DATA, sizeof(UMMA_FS_SAFELY_REMOVE_CHK_DATA), &ulSize);
				}
				if(ulExtFileDsc)
					PAL_FS_Close (ulExtFileDsc);
			}
		}
	}
	else
	{
		HxLOG_Error("\nMount fail!!!\n\n");
		pstDevice->eDevStatus = eFS_DevStatus_NeedFormat;
	}

	bMount = (pstDevice->eDevStatus == eFS_DevStatus_OK) ? TRUE : FALSE;

	pstDevice->eDevSubStatus = eFS_DevSubStatus_None;

	ulLastChkDevIdx = -1;

	// 해당 모델이 지원하는 이 Device의 PVR type을 가져온다.
	ePvrType = umma_fs_config_GetPvrType(pstDevice->eDevType);

	// record (=PVR)
	if ((ePvrType & eFS_PVR_Main) || (ePvrType & eFS_PVR_Sub) || (ePvrType & eFS_PVR_Tsr))	// Main or Sub or TSR
	{
		bOk = umma_fs_svc_PvrIsPvrStorageRegistrable();
		if(bOk == TRUE)
		{
			hErrPairable = umma_fs_svc_PvrCheckDeviceIsExtPvrPaired (eDevIdx, FALSE, UMMA_FS_EXT_PVR_PART_IDX);
			if(hErrPairable != ERR_OK)
			{
				HxLOG_Error("umma_fs_svc_PvrCheckDeviceIsExtPvrPaired eDevIdx(%d) error!\n",eDevIdx);
			}
		}

		HxLOG_Print("[umma_fs_usb_MsgEvtUsbScsiAdded] sent the EVT_FS_NOTIFY_USB_CONNECTED to AP >>>\n");
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

	(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_CONNECTED, (HINT32)eDevIdx, (HINT32)bMount, 0);

	if ( (ePvrType & eFS_PVR_Main) || (ePvrType & eFS_PVR_Sub) || (ePvrType & eFS_PVR_Tsr)	) // Main or Sub or TSR
	{
		// PVR용 HDD 인지 확인
		hErr = umma_fs_svc_PvrGetPvrIdxByDevIdx(&ulPvrIdx, eDevIdx);
		if(ERR_OK == hErr)
		{
			 //Ext HDD의 partition은 1개다. 따라서 0으로 내려준다. 추후 바뀌면 수정필요함
			(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_PVR_ENABLED, (HINT32)eDevIdx, (HINT32)0, (HINT32)ulPvrIdx);
		}
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

	return hErr;
}

STATIC HERROR umma_fs_usb_MsgEvtUsbScsiRemoved (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	UMMA_FS_DevIdx_e		 eDevIdx;
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	HERROR				 hErr;
	HBOOL			bSafeRemoved = FALSE;

	eDevIdx = umma_fs_dev_ConvertPalDevIdxToMwDevIdx (ulParam1);
	if (eDevIdx < UMMA_FS_DEV_IDX_START || eDevIdx > UMMA_FS_DEV_IDX_END)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr == ERR_OK && pstDevice != NULL)
	{
		// USB 메모리가 아니라면 올라와서도, 처리해서도 안되는 메시지이다.
		if (umma_fs_usb_IsUsbType (pstDevice) != TRUE)
		{
			HxLOG_Critical("\nCritical Error!!!\n\n");
			return ERR_FAIL;
		}

		// 장비가 뽑혔으니 이 Flag도 제거되어야 한다.
		pstDevice->bIsExPvrModule = FALSE;

		// Device가 Disconnect 되었다는 것은 기록해두어야 한다.
		pstDevice->bDevConnected = FALSE;

		 //이미 안전제거(umount)된 device인지 여부를 올려준다.
		if(pstDevice->eDevStatus == eFS_DevStatus_SafeRemoved)
		{
			bSafeRemoved = TRUE;
		}

		if (pstDevice->bDevConnected == FALSE && pstDevice->bDevMounted == FALSE)
		{
			umma_fs_dev_InitFsDeviceInfo (pstDevice);
			HxLOG_Print("[%s:%d] Set Session [%d]\n",__FUNCTION__,__LINE__, eDevIdx);
		}

		// Unmount는 AP까지 올라간 후에 USB manager가 직접 unmount 시켜야 한다.
		HxLOG_Print("[umma_fs_usb_MsgEvtUsbScsiRemoved] sent the EVT_FS_NOTIFY_USB_DISCONNECTED to AP\n");

		{
			HINT32 i;
			for(i=0; i< pstDevice->ulNumPartition; i++)
			{
//				UAPI_UnmountDevice(pstDevice->pstPartArray[i].szMountPath);
				HxLOG_Error(HxANSI_COLOR_YELLOW("[%s] HAPI_BroadcastSignal: szMountPath[%s]\n"),__FUNCTION__,pstDevice->pstPartArray[i].szMountPath);
			}
		}

		if (!bSafeRemoved)	 //이미 umount 되었으니 올릴필요 없다.
		{
			(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_DISCONNECTED, (HINT32)eDevIdx, 0, 0);
		}
		else
		{
			// 실제 동작상 에러는 아니지만, 동기화 처리 동작을 위해 FAIL 리턴
			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Error("\numma_fs_dev_GetDeviceInfo error\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR umma_fs_usb_MsgEvtUsbOverCurrent (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
	HUINT32			 ulSpeed, ulOcp;
	HUINT32			 ulPortNum = ulParam2;
	HUINT32			 ulDeviceNum;
	UMMA_FS_DevIdx_e	*peIdxArray, eDevIdx;
	HUINT32			 ulCount;
	HERROR			 hErr;
	UMMA_FS_DevContext_t	*pstDevice = NULL;

	ulDeviceNum = 0;
	peIdxArray = NULL;
	umma_fs_dev_GetDeviceIndexArray(&ulDeviceNum, &peIdxArray);

	if (0 < ulDeviceNum && peIdxArray)
	{
		PAL_FS_USB_GetSpeedAndOcp (ulParam3, &ulSpeed, &ulOcp);

		for (ulCount=0; ulCount<ulDeviceNum; ulCount++)
		{
			eDevIdx = peIdxArray[ulCount];
			pstDevice = NULL;
			hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
			if (hErr == ERR_OK && pstDevice != NULL)
			{
				if (eFS_DevType_UsbHdd == pstDevice->eDevType ||
					eFS_DevType_UsbMem == pstDevice->eDevType)
				{
					if (ulPortNum == pstDevice->ulPortNum)
					{
						if (ulOcp == eSCSI_PWR_OverCurrent)
						{
							HxLOG_Info("[umma_fs_usb_MsgEvtUsbOverCurrent] sent the EVT_FS_USB_OVERCURRENT (%d) to AP\n", eDevIdx);
							(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_USB_OVERCURRENT, (HINT32)eDevIdx, TRUE, 0);
						}
						else
						{
							HxLOG_Info("[umma_fs_usb_MsgEvtUsbOverCurrent] sent the EVT_FS_USB_OVERCURRENT (%d) to AP\n", eDevIdx);
							(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_USB_OVERCURRENT, (HINT32)eDevIdx, FALSE, 0);
						}
					}
				}
			}

		}
	}

	if (peIdxArray)
	{
		umma_fs_dev_FreeDeviceIndexArray(peIdxArray);
	}
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

STATIC void umma_fs_usb_EventNotifier (HUINT32 event, HUINT32 param1, HUINT32 param2, HUINT32 param3)
{
	SVC_FS_SEM_GET;

	switch (event)
	{
	case PAL_FS_EVENT_USB_SCSI_ADDED:
		HxLOG_Warning("[umma_fs_usb_EventNotifier] PAL_FS_EVENT_USB_SCSI_ADDED (0x%08x:0x%08x:0x%08x)\n", param1, param2, param3);
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_USB_ADDED, (HINT32)param1, (HINT32)param2, (HINT32)param3);
		break;

	case PAL_FS_EVENT_USB_SCSI_REMOVED:
		HxLOG_Warning("[umma_fs_usb_EventNotifier] PAL_FS_EVENT_USB_SCSI_REMOVED (0x%08x:0x%08x:0x%08x)\n", param1, param2, param3);
		(void)umma_fs_svc_SendEventToMainloop(eFS_EVT_USB_REMOVED, (HINT32)param1, (HINT32)param2, (HINT32)param3);
		break;

	case PAL_FS_EVENT_USB_CONNECTED:
		HxLOG_Warning("[umma_fs_usb_EventNotifier] PAL_FS_EVENT_USB_CONNECTED (0x%08x:0x%08x:0x%08x)\n", param1, param2, param3);
		break;

	case PAL_FS_EVENT_USB_DISCONNECTED:
		HxLOG_Warning("[umma_fs_usb_EventNotifier] PAL_FS_EVENT_USB_DISCONNECTED (0x%08x:0x%08x:0x%08x)\n", param1, param2, param3);
		break;

	case PAL_FS_EVENT_USB_OVERCURRENT:
		HxLOG_Warning("[umma_fs_usb_EventNotifier] PAL_FS_EVENT_USB_OVERCURRENT (0x%08x:0x%08x:0x%08x)\n", param1, param2, param3);
		umma_fs_usb_MsgEvtUsbOverCurrent (param1, param2, param3);
		break;

	default:
		HxLOG_Critical("\nCritical Error!!!\n\n");
		break;
	}

	SVC_FS_SEM_RELEASE;
}


#define ________FS_INTERNAL_FUNCTIONS________

HERROR umma_fs_usb_Init (void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
	HERROR	hErr;

	//	Task for check disk
	hErr = VK_MSG_Create(32, sizeof(FS_USB_CHKDISK_Info_t), "CheckDiskQ", (unsigned long*)&s_ulCheckDiskMsgId, eHLIB_SUSPENDTYPE_FIFO);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Create FAIL!!\n");
		return ERR_FAIL;
	}

	hErr = VK_TASK_Create(umma_fs_usb_CheckDiskTask,
					VK_TASK_PRIORITY_MW_REF, SIZE_64K,
					(const char*)"CheckDiskTask", NULL, (unsigned long*)&s_ulCheckDiskTaskId, 0);
	if(hErr == ERR_OK)
	{
		hErr = VK_TASK_Start(s_ulCheckDiskTaskId);
	}

	PAL_FS_USB_Initialize();
	PAL_FS_USB_RegisterNotifier (umma_fs_usb_EventNotifier);

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_usb_MountDevice (UMMA_FS_DevIdx_e eDevIdx, HBOOL bAfterFormat)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
	HBOOL						bIsUsbMemory;
	HUINT32						ulCnt=0, ulDriveNo=0;
	HUINT32						ulAvailablePartition = 0;
	PAL_FS_StorDevIdx_t			ePalDevIdx;
	UMMA_FS_PartitionInfo_t		*pstPartInfo = NULL;
	UMMA_FS_DevContext_t		*pstDevice = NULL;
	HUINT8						szMountPath[64];
	HERROR						hErr;
	UMMA_FS_FormatPartList_t	stFormatList;
	HBOOL						bOk = FALSE;
	UMMA_FS_PvrType_e 			ePvrType;
	HCHAR						szDevName[32];
	HUINT64						ullTotalSizeKb = 0;

	HxLOG_Print("[umma_fs_usb_MountDevice] (+)\n");

	VK_memset(&stFormatList, 0, sizeof(UMMA_FS_FormatPartList_t)); // MOON_encr
	if(bAfterFormat == TRUE) 	umma_fs_dev_ReinitPartitionInfo (eDevIdx, &stFormatList, FALSE);

	ePalDevIdx = umma_fs_dev_ConvertMwDevIdxToPalDevIdx (eDevIdx);

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Error("[umma_fs_usb_MountDevice] umma_fs_dev_ConnectNewDevice (%d) err(0x%08x)\n", eDevIdx, hErr);
		return ERR_FAIL;
	}

	// Mount 이전에 알 수 있는 정보는 여기서 체운다.
	bIsUsbMemory = PAL_FS_USB_IsDeviceRemovable (ePalDevIdx);
	pstDevice->eDevType 		= (bIsUsbMemory == TRUE) ? eFS_DevType_UsbMem : eFS_DevType_UsbHdd;

	// Device mount setting
	if (pstDevice->ulNumPartition > 0 && pstDevice->pstPartArray != NULL)
	{
		for (ulCnt = 0; ulCnt < pstDevice->ulNumPartition; ulCnt++)
		{
			if (pstDevice->pstPartArray[ulCnt].bAvailable != TRUE)
			{
				HxLOG_Warning("Not available partition!! (%d) \n", ulCnt);
				continue;
			}

			umma_fs_dev_GetNewDriveNo (&ulDriveNo);
			hErr = umma_fs_dev_MakeMountPathByDriveNo (ulDriveNo, szMountPath);
			if (hErr != ERR_OK) 				{ continue; }

			pstPartInfo = NULL;
			hErr = umma_fs_dev_MountPartition (eDevIdx, ulCnt, FALSE, szMountPath, &pstPartInfo); // MOON_encr
			if (hErr == ERR_OK && pstPartInfo != NULL)
			{
				pstPartInfo->ulDriveNo		= ulDriveNo;
			}
			else
			{
				HxLOG_Critical("\nMount fail! (cnt:%d)\n", ulCnt);
				return ERR_FAIL;
			}
			ulAvailablePartition++;
		}
	}
	else
	{
		HxLOG_Critical("\nMount fail!!\n");
		return ERR_FAIL;
	}

	if (!ulAvailablePartition) /* if (SQE failed USB) can't mount although there are partition infos, ... */
	{
		HxLOG_Error("Error> No available partition!\n");
		return ERR_FAIL;
	}

	// mount 후에 정보를 얻을 수 있는 device 관련.
	umma_fs_dev_GetDeviceFileFullName(eDevIdx, szDevName);
	hErr = umma_fs_dev_GetDevSize(szDevName, &ullTotalSizeKb);
	if (hErr == ERR_OK)
	{
		pstDevice->ullDevSizeKb = ullTotalSizeKb;
	}

	// Device가 Mount 후, PVR 로 될 수 있는지 확인한다.
	ePvrType = umma_fs_config_GetPvrType(pstDevice->eDevType);

	// record (PVR)
	if ((ePvrType & eFS_PVR_Main) || (ePvrType & eFS_PVR_Sub) || (ePvrType & eFS_PVR_Tsr))	// Main or Sub or TSR
	{
		if (bAfterFormat)
		{
			bOk = umma_fs_svc_PvrIsPvrStorageRegistrable();
			if(bOk == TRUE)
			{
				HxLOG_Warning("eDevIdx : [%d], bAfterFormat [%d]\n", eDevIdx, bAfterFormat);
				hErr = umma_fs_svc_PvrCheckDeviceIsExtPvrPaired (eDevIdx, bAfterFormat, UMMA_FS_EXT_PVR_PART_IDX);
				if (ERR_OK != hErr)
				{
					HxLOG_Print("umma_fs_svc_PvrCheckDeviceIsExtPvrPaired error (%d)\n", hErr);
				}
			}
			else
			{
				HxLOG_Print("[umma_fs_svc_PvrCheckDeviceIsExtPvrPaired] No Registered, and No PVR HDD.\n");
			}
		}
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_usb_MountAllDevice (void)
{
	// Connection만 되어 있고 아직 Mount 안 된 모든 USB Device를 전부 Mount 시키는 함수.
	// 현재 사용되는 곳이 없으니 차후 구현으로 남겨 놓는다.

	return ERR_OK;
}

// 기본적으로 umma_fs_dev_UnmountDevice 를 그대로 사용하나
// USB 모듈용이니 만큼 최소한 USB Device인지는 확인하고 진행시킨다.
HERROR umma_fs_usb_UnmountDevice (UMMA_FS_DevIdx_e eDevIdx, HBOOL bSafeRemove)
{
	UMMA_FS_DevContext_t			*pstDevice;
	HERROR					 hErr;

	hErr = umma_fs_dev_GetDeviceInfo (eDevIdx, &pstDevice);
	if (hErr != ERR_OK || pstDevice == NULL)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
	}

	if (umma_fs_usb_IsUsbType (pstDevice) != TRUE)
	{
		HxLOG_Critical("\nCritical Error!!!\n\n");
		return ERR_FAIL;
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

HERROR umma_fs_usb_UnmountDeviceAll (void)
{
	HUINT32			 ulIndex;
	UMMA_FS_DevInfoMap_t	*pstContext;
	UMMA_FS_DevContext_t	*pstDevice;

	pstContext = umma_fs_dev_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("[umma_fs_usb_UnmountDeviceAll] umma_fs_dev_GetContext NULL\n");
		return ERR_FAIL;
	}

	for (ulIndex = 0; ulIndex < eFS_Dev_MAX; ulIndex++)
	{
		pstDevice = &(pstContext->astDevice[ulIndex]);
		if (pstDevice->bDevAvailable == FALSE)			{ continue; }

		umma_fs_usb_UnmountDevice (pstDevice->eDevIdx, FALSE);
	}

	return ERR_OK;
}

HERROR umma_fs_usb_PowerOff (void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
	return PAL_FS_USB_PowerOff();
#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_usb_PowerOn (void)
{
#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
	return PAL_FS_USB_PowerOn();
#else
	return ERR_FAIL;
#endif
}

HERROR umma_fs_usb_AddedPostProcess(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	return umma_fs_usb_MsgEvtUsbScsiAdded(ulParam1, ulParam2, ulParam3);
}

HERROR umma_fs_usb_RemovedPostProcess(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	return umma_fs_usb_MsgEvtUsbScsiRemoved(ulParam1, ulParam2, ulParam3);
}




/*********************** End of File ******************************/
