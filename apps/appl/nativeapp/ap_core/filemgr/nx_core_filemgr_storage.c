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

/******************************************************************************/
/**
 * @file	  		nx_core_filemgr_storage.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_core_filemgr_storage.h>
#include <nx_core_message.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	HxVector_t	*devList;
} NX_StorageContext_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NX_StorageContext_t g_StorageContext;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NX_StorageContext_t *nx_filemgr_Storage_GetContext(void);

static void nx_filemgr_Storage_RcvMsgDevChange(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);
static void nx_filemgr_Storage_RcvMsgNeedFormat(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);
static void nx_filemgr_Storage_RcvMsgFormatState(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);
static void nx_filemgr_Storage_RcvMsgDstState(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);
static void nx_filemgr_Storage_RcvMsgOverCurrent(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);
static void nx_filemgr_Storage_RcvMsgUnmountFail(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);
static void nx_filemgr_Storage_RcvMsgCheckDiskStarted(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);
static void nx_filemgr_Storage_RcvMsgCheckUsbSpeed(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);
static void nx_filemgr_Storage_RcvMsgPvrReady(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);
static void nx_filemgr_Storage_RcvMsgResult(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);

static void nx_filemgr_StorageInfoNotifier(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);

static NX_StoragePairing_e 			nx_filemgr_StorageConvertDevicePairing(const HUINT32 ulFlag);
static NX_StorageInterface_e 		nx_filemgr_StorageConvertDeviceInterface(const DxStorage_DevType_e eType);
static NX_StorageInterfaceVersion_e nx_filemgr_StorageConvertDeviceInterfaceVersion(const DxStorage_DevType_e eType, const HUINT64 ullDeviceWriteSpeedBps);
static NX_StorageType_e 			nx_filemgr_StorageConvertDeviceType(const DxStorage_DevType_e eType);
static NX_StorageKind_e 			nx_filemgr_StorageConvertDeviceKind(const DxStorage_DevType_e eKind);
static NX_StorageUsage_e 			nx_filemgr_StorageConvertDeviceUsage(const HUINT32 ulFlag);
static NX_StorageUsage_e 			nx_filemgr_StorageGetDeviceSort(NX_PhysicalStorageData_t *pstPhysicalStorageData);
static NX_PhysicalStorageData_t		*nx_filemgr_StorageCreatePhysicalStorageData(const DxStorage_Info_t *pstDevInfo);

static HBOOL 	nx_filemgr_Storage_FindStorageInfoCB(void *data, void *userdata);
static HERROR	nx_filemgr_Storage_UpdateStorageInfoData(NX_PhysicalStorageData_t *pstPhysicalStorageData);
static HERROR	nx_filemgr_Storage_AddStorageInfoData(NX_PhysicalStorageData_t *pstPhysicalStorageData);
static HERROR	nx_filemgr_Storage_RemoveStorageInfoData(HUINT32 ulDeviceId);
static HBOOL 	nx_filemgr_Storage_FindStorageDevId(HUINT32 storageDevId);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static NX_StorageContext_t *nx_filemgr_Storage_GetContext(void)
{
	return &g_StorageContext;
}

#define __nx_filemgr_Storage_RcvMsgXXXXXXX__________
static HERROR	nx_filemgr_Storage_RcvMsgInit(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 					ulIndex, ulListLength = 0;
	HERROR						hErr;
	DxStorage_Info_t			*pstStorageList = NULL;
	NX_PhysicalStorageData_t	*pstPhysicalStorageData = NULL;

	hErr = NX_PORT_FILEMGR_STORAGE_GetStorageList(&pstStorageList, &ulListLength);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("NX_PORT_FILEMGR_STORAGE_GetPhysicalStorageList failed. \n");
		return ERR_FAIL;
	}

	if (ulListLength > 0)
	{
		for (ulIndex = 0; ulIndex < ulListLength; ulIndex++)
		{
			pstPhysicalStorageData = nx_filemgr_StorageCreatePhysicalStorageData(&pstStorageList[ulIndex]);
			if (pstPhysicalStorageData)
			{
				nx_filemgr_Storage_AddStorageInfoData(pstPhysicalStorageData);
			}
		}
	}
	NX_PORT_FILEMGR_STORAGE_FreeStorageList(pstStorageList, ulListLength);

	return ERR_OK;
}

static void nx_filemgr_Storage_RcvMsgDevChange(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL	bConnect = (HBOOL)p1;
	HBOOL	bDevIdExist = FALSE;
	DxStorage_Info_t	*pstInfo = (DxStorage_Info_t	*)p2;

	NX_PhysicalStorageData_t	*pstPhysicalStorageData = NULL;

	HxLOG_Warning("ulDeviceId : %d, bConnect : %s \n", pstInfo->ulDeviceId, (bConnect)?"TRUE":"FALSE");

	bDevIdExist = nx_filemgr_Storage_FindStorageDevId(pstInfo->ulDeviceId);
	if (bConnect == TRUE)
	{
		if (bDevIdExist == TRUE)
		{
			pstPhysicalStorageData 	= nx_filemgr_StorageCreatePhysicalStorageData(pstInfo);
			nx_filemgr_Storage_UpdateStorageInfoData(pstPhysicalStorageData);
			ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_DEVICE_CHANGE, 0, pstInfo->ulDeviceId, eNxStorageState_Update, 0);
		}
		else
		{
			pstPhysicalStorageData 	= nx_filemgr_StorageCreatePhysicalStorageData(pstInfo);
			nx_filemgr_Storage_AddStorageInfoData(pstPhysicalStorageData);
			ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_DEVICE_CHANGE, 0, pstInfo->ulDeviceId, eNxStorageState_Attach, 0);
		}
	}
	else
	{
		if (bDevIdExist == TRUE) {
			nx_filemgr_Storage_RemoveStorageInfoData(pstInfo->ulDeviceId);
			ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_DEVICE_CHANGE, 0, pstInfo->ulDeviceId, eNxStorageState_Detach, 0);
		}
	}

	if(bConnect == FALSE && (pstInfo->eDevType == eDxSTORAGE_DEVTYPE_INTHDD  || pstInfo->eDevType == eDxSTORAGE_DEVTYPE_DETACHABLE))
	{
		HxLOG_Warning("ID[%d] Detach event!! APK_META_RECORD_RemoveByStorageID() \n", pstInfo->ulDeviceId);

	}
}

static void nx_filemgr_Storage_RcvMsgNeedFormat(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulDeviceId = (HUINT32)p1;
	HxLOG_Warning("Need format.... deviceId  : %d \n", p1);
	ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_NEED_FORMAT, 0, ulDeviceId, 0, 0);
}

static void nx_filemgr_Storage_RcvMsgFormatState(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulDeviceId = (HUINT32)p2;
	HxLOG_Warning("Format state : %d \n", p1);
	switch (p1)
	{
		case eDxSTORAGE_DEV_STATUS_Formatting:
			{
				DxStorage_Progress_t	*pstProgress = (DxStorage_Progress_t *)p2;
				if (pstProgress)
				{
					ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_FORMAT_PROGRESS, 0, ulDeviceId, (HINT32)pstProgress, 0);
				}
			}
			break;

		case eDxSTORAGE_DEV_STATUS_FormatFail:
			ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_FORMAT_FAILED, 0, ulDeviceId, 0, 0);
			break;

		case eDxSTORAGE_DEV_STATUS_FormatDone:
			{
				DxStorage_STATUS_Format_e	eFormatStatue = (DxStorage_STATUS_Format_e)p1;
				ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_FORMAT_DONE, 0, ulDeviceId, eFormatStatue, 0);
			}
			break;

		default:
			break;
	}
}

static void nx_filemgr_Storage_RcvMsgDstState(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxStorage_STATUS_DST_Result_e eDstResult = (DxStorage_STATUS_DST_Result_e)p1;
	HxLOG_Warning("DST State : %d\n", eDstResult);
	switch (p1)
	{
		case eDxSTORAGE_DEV_STATUS_DstResultOk:
			{
				HUINT32 	ulDeviceId = (HUINT32)p2;
				// notify Recovery progress (finished)
				ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_DST_STATE, 0, ulDeviceId, eDstResult, 100);

			}
			break;

		case eDxSTORAGE_DEV_STATUS_DstResultOnGoing:
			{
				DxStorage_Progress_t	*pstProgress = (DxStorage_Progress_t *)p2;
				ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_DST_STATE, 0, 0, eDstResult, (HINT32)pstProgress);
			}
			break;

		case eDxSTORAGE_DEV_STATUS_DstResultFailNoHDD:
		case eDxSTORAGE_DEV_STATUS_DstResultFailTryAgain:
		case eDxSTORAGE_DEV_STATUS_DstResultFailFatalError:
		case eDxSTORAGE_DEV_STATUS_DstResultFailTestElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailElectricalElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailServoElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailReadElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailHadlingDamange:
		case eDxSTORAGE_DEV_STATUS_DstResultFailUnknown:
			{
				HUINT32 	ulDeviceId = (HUINT32)p2;
				// notify Recovery progress (Error)
				ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_DST_STATE, 0, ulDeviceId, eDstResult, 0);
			}
			break;

		default:
			break;
	}
}

static void nx_filemgr_Storage_RcvMsgOverCurrent(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulDeviceId = (HUINT32)p1;
	HBOOL 		bOccur = p2 ? TRUE : FALSE;

	HxLOG_Warning("eOPL_STORAGE_EVENT_OverCurrent!! deviceId(%d), Occurred(%d)\n", p1, bOccur);
	if (bOccur)
	{
		ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_OVER_CURRENT, 0, ulDeviceId, 0, 0);
	}
	else
	{
		ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_OVER_CURRENT_DONE, 0, ulDeviceId, 0, 0);
	}
}

static void nx_filemgr_Storage_RcvMsgUnmountFail(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulDeviceId = (HUINT32)p1;

	HxLOG_Warning("Unmount Fail!! deviceId : %d \n", p1);
	ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_FAILED_UNMOUNT, 0, ulDeviceId, 0, 0);
}

static void nx_filemgr_Storage_RcvMsgCheckDiskStarted(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulDeviceId = (HUINT32)p1;

	HxLOG_Warning("Check disk startedil!! deviceId : %d \n", ulDeviceId);
	ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_STARTED_CHECKDISK, 0, ulDeviceId, 0, 0);
}

static void nx_filemgr_Storage_RcvMsgCheckUsbSpeed(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulDeviceId = (HUINT32)p1;
	HUINT32 	ulReadMbps = (HUINT32)p2;
	HUINT32 	ulWriteMbps = (HUINT32)p3;

	HxLOG_Warning("Check usb speed done!! deviceId : %d, Read : %dMbps, Write : %dMbps\n", ulDeviceId, ulReadMbps, ulWriteMbps);
	ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_CHECK_USB_SPEED_DONE, 0, ulDeviceId, ulReadMbps, ulWriteMbps);
}

static void nx_filemgr_Storage_RcvMsgPvrReady(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// device의 연결/해제, 혹은 Format으로 인해 PVR device 정보가 바뀌었을때 발생
	HBOOL	bPvr = (HBOOL)p1;	//TRUE : PVR storage, FALSE : Not a PVR storage (disable or unpaired)
	HUINT32 	ulDeviceId = (HUINT32)p2;

	HxLOG_Warning("PvrReady : %s, deviceId : %d \n", (bPvr)?"T":"F", ulDeviceId);
	if (bPvr == TRUE)
	{
		ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_PAIRING_DONE, 0, ulDeviceId, 0, 0);
	}
	else
	{
		ONDK_GWM_PostMessage(NULL, MSG_CORE_STORAGE_UNPAIRING_DONE, 0, ulDeviceId, 0, 0);
	}
}

static void nx_filemgr_Storage_RcvMsgResult(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// API Call에 대한 결과를 리턴
	DxStorage_ResultType_e eResultType = (DxStorage_ResultType_e)p1;
	HUINT32 	ulDeviceId = (HUINT32)p2;
	HERROR	hErr = (HERROR)p3;				// API 결과 (ERR_OK:0, ERR_FAIL:1)
	HINT32	resultEvtMsg = MSG_CORE_STORAGE_EVT_NONE;
	NX_PhysicalStorageData_t *physicalStorageInfo = NULL;

	HxLOG_Warning("eResultType : %d, ulDeviceId : %d, Result : %d\n", eResultType, ulDeviceId, hErr);
	switch(eResultType)
	{
		case eDxSTORAGE_RESULT_Format:
			if (hErr != ERR_OK)
			{
				resultEvtMsg = MSG_CORE_STORAGE_FORMAT_FAILED;
			}
			else
			{
				resultEvtMsg = MSG_CORE_STORAGE_EVT_NONE;
			}
			break;

		case eDxSTORAGE_RESULT_Dst:
			// Add event if it is necessary.
			resultEvtMsg = MSG_CORE_STORAGE_EVT_NONE;
			break;

		case eDxSTORAGE_RESULT_SetDefaultHdd:
			// Add event if it is necessary.
			resultEvtMsg = MSG_CORE_STORAGE_EVT_NONE;
			break;

		case eDxSTORAGE_RESULT_SetName:
			if (hErr != ERR_OK)
			{
				resultEvtMsg = MSG_CORE_STORAGE_SETNAME_FAILED;
			}
			else
			{
				resultEvtMsg = MSG_CORE_STORAGE_SETNAME_DONE;
			}
			break;

		case eDxSTORAGE_RESULT_DisableHdd:
			// Add event if it is necessary.
			resultEvtMsg = MSG_CORE_STORAGE_EVT_NONE;
			break;

		case eDxSTORAGE_RESULT_Pairing:
			if (hErr != ERR_OK)
			{
				resultEvtMsg = MSG_CORE_STORAGE_PAIRING_FAILED;
			}
			else
			{
				hErr = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(ulDeviceId, &physicalStorageInfo);
				if (hErr != ERR_OK)
				{
					resultEvtMsg = MSG_CORE_STORAGE_PAIRING_FAILED;
				}
				else
				{
					if (physicalStorageInfo != NULL)
					{
						physicalStorageInfo->ePairing = eStoragePairing_OK;
						physicalStorageInfo->eUsage = eStorageUsage_PVR;
					}
					resultEvtMsg = MSG_CORE_STORAGE_PAIRING_DONE;
				}
			}
			break;

		case eDxSTORAGE_RESULT_UnPairing:
			if (hErr != ERR_OK)
			{
				resultEvtMsg = MSG_CORE_STORAGE_UNPAIRING_FAILED;
			}
			else
			{
				hErr = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(ulDeviceId, &physicalStorageInfo);
				if (hErr != ERR_OK)
				{
					resultEvtMsg = MSG_CORE_STORAGE_UNPAIRING_FAILED;
				}
				else
				{
					if (physicalStorageInfo != NULL)
					{
						physicalStorageInfo->ePairing = eStoragePairing_Fail;
						physicalStorageInfo->eUsage = eStorageUsage_Storage;
					}
					resultEvtMsg = MSG_CORE_STORAGE_UNPAIRING_DONE;
				}
			}
			break;

		case eDxSTORAGE_RESULT_SafeRemove:
			if (hErr != ERR_OK)
			{
				resultEvtMsg = MSG_CORE_STORAGE_SAFE_REMOVE_FAILED;
			}
			else
			{
				resultEvtMsg = MSG_CORE_STORAGE_EVT_NONE;
			}
			break;

		default:
			resultEvtMsg = MSG_CORE_STORAGE_EVT_NONE;
			break;
	}

	if (resultEvtMsg != MSG_CORE_STORAGE_EVT_NONE)
	{
		ONDK_GWM_PostMessage(NULL, resultEvtMsg, 0, ulDeviceId, eResultType, 0);
	}
}


#define __end_nx_filemgr_Storage_RcvMsgXXXXXXX______

static NX_StoragePairing_e nx_filemgr_StorageConvertDevicePairing(const HUINT32 ulFlag)
{
	NX_StoragePairing_e 	eRetPair = eStoragePairing_Unknown;

	if (ulFlag & eDxSTORAGE_FLAG_PAIRED)
	{
		if (ulFlag & eDxSTORAGE_FLAG_NEW2NDHDD)
			eRetPair = eStoragePairing_Now;
		else
			eRetPair = eStoragePairing_OK;
	}
	else
	{
		eRetPair = eStoragePairing_Fail;
	}

	return eRetPair;
}

static NX_StorageInterface_e nx_filemgr_StorageConvertDeviceInterface(const DxStorage_DevType_e eType)
{
	NX_StorageInterface_e	eRetInterface = eStorageInterface_Unknown;

	switch(eType)
	{
		case eDxSTORAGE_DEVTYPE_INTHDD:
		case eDxSTORAGE_DEVTYPE_ESATAHDD:
		case eDxSTORAGE_DEVTYPE_DETACHABLE:
			eRetInterface = eStorageInterface_SATA;
			break;
		case eDxSTORAGE_DEVTYPE_USBHDD:
		case eDxSTORAGE_DEVTYPE_USBMEM:
			eRetInterface = eStorageInterface_USB;
			break;
		case eDxSTORAGE_DEVTYPE_MMC:
			eRetInterface = eStorageInterface_SD;
			break;
		case eDxSTORAGE_DEVTYPE_NONE:
		default:
			break;
	}
	return eRetInterface;
}

static NX_StorageInterfaceVersion_e nx_filemgr_StorageConvertDeviceInterfaceVersion(const DxStorage_DevType_e eType, const HUINT64 ullDeviceWriteSpeedBps)
{
	NX_StorageInterfaceVersion_e	eInterfaceVersion = eStorageInterfaceVersion_Unknown;
	HUINT64		ullFiveGB = 5LL*1024LL*1024LL*1024LL; // 5 GB per second

	switch (eType)
	{
		case eDxSTORAGE_DEVTYPE_USBHDD:
		case eDxSTORAGE_DEVTYPE_USBMEM:
			if (ullDeviceWriteSpeedBps >= ullFiveGB)
			{
				eInterfaceVersion = eStorageInterfaceVersion_USB3;
			}
			else
			{
				eInterfaceVersion = eStorageInterfaceVersion_USB2;
			}
			break;
		default:
			break;
	}

	return eInterfaceVersion;
}

static NX_StorageType_e nx_filemgr_StorageConvertDeviceType(const DxStorage_DevType_e eType)
{
	NX_StorageType_e	eRetType = eStorageType_Unknown;

	switch (eType)
	{
		case eDxSTORAGE_DEVTYPE_INTHDD:
			eRetType = eStorageType_Internal;
			break;
		case eDxSTORAGE_DEVTYPE_USBHDD:
		case eDxSTORAGE_DEVTYPE_ESATAHDD:
		case eDxSTORAGE_DEVTYPE_DETACHABLE:
			eRetType = eStorageType_External;
			break;
		case eDxSTORAGE_DEVTYPE_NONE:
		default:
			break;
	}
	return eRetType;
}

static NX_StorageKind_e nx_filemgr_StorageConvertDeviceKind(const DxStorage_DevType_e eKind)
{
	NX_StorageKind_e	eRetKind = eStorageKind_Unknown;

	switch (eKind)
	{
		case eDxSTORAGE_DEVTYPE_INTHDD:
		case eDxSTORAGE_DEVTYPE_USBHDD:
		case eDxSTORAGE_DEVTYPE_DETACHABLE:
		case eDxSTORAGE_DEVTYPE_ESATAHDD:
			eRetKind = eStorageKind_HDD;
			break;
		case eDxSTORAGE_DEVTYPE_USBMEM:
			eRetKind = eStorageKind_USBMemory;
			break;
		case eDxSTORAGE_DEVTYPE_MMC:
			eRetKind = eStorageKind_SDMemory;
			break;
		case eDxSTORAGE_DEVTYPE_NONE:
		default:
			break;
	}
	return eRetKind;
}

static NX_StorageUsage_e nx_filemgr_StorageConvertDeviceUsage(const HUINT32 ulFlag)
{
	NX_StorageUsage_e	eRetUsage = eStorageUsage_Unknown;

	if (ulFlag & eDxSTORAGE_FLAG_SUBPVRDEVICE)
	{
		eRetUsage = eStorageUsage_SubPVR;
	}
	else if (ulFlag & eDxSTORAGE_FLAG_PVRDEVICE)
	{
		eRetUsage = eStorageUsage_PVR;
	}
	else if(ulFlag & eDxSTORAGE_FLAG_MOUNTED)
	{
		eRetUsage = eStorageUsage_Storage;
	}
	else if(ulFlag & eDxSTORAGE_FLAG_NEEDFORMAT)
	{
		eRetUsage = eStorageUsage_NeedFormat;
	}
	return eRetUsage;
}

static NX_StorageUsage_e nx_filemgr_StorageGetDeviceSort(NX_PhysicalStorageData_t *pstPhysicalStorageData)
{
	NX_StorageUsage_e eRetSort = eStorageSort_Unknown;

	if (pstPhysicalStorageData == NULL)
	{
		HxLOG_Debug("NX_PhysicalStorageData_t is NULL!\n");
		return eRetSort;
	}

	switch(pstPhysicalStorageData->eKind)
	{
		case eStorageKind_HDD:
			{
				switch(pstPhysicalStorageData->eUsage)
				{
					case eStorageUsage_PVR:
						if (pstPhysicalStorageData->eInterface == eStorageInterface_SATA)
						{
							eRetSort = eStorageSort_InternalPVRHDD;
						}
						else if (pstPhysicalStorageData->eInterface == eStorageInterface_USB)
						{
							eRetSort = eStorageSort_externalPVRHDD;
						}
						else
						{
							eRetSort = eStorageSort_Unknown;
						}
						break;
					case eStorageUsage_NeedFormat:
						if (pstPhysicalStorageData->eType == eStorageType_Internal)
						{
							eRetSort = eStorageSort_InternalPVRHDD;
						}
						else if (pstPhysicalStorageData->eType == eStorageType_External)
						{
							eRetSort = eStorageSort_externalPVRHDD;
						}
						else
						{
							eRetSort = eStorageSort_Unknown;
						}
						break;
					case eStorageUsage_SubPVR:
						eRetSort = eStorageSort_externalSubPVRHDD;
						break;
					default:
						if (pstPhysicalStorageData->eInterface == eStorageInterface_SATA)
						{
							eRetSort = eStorageSort_InternalPVRHDD;
						}
						else
						{
							eRetSort = eStorageSort_externalHDD;
						}
						break;
				}
			}
			break;
		case eStorageKind_USBMemory:
			eRetSort = eStorageSort_UsbMemory;
			break;
		case eStorageKind_SDMemory:
			eRetSort = eStorageSort_SdMemory;
			break;
		default:
			eRetSort = eStorageSort_Unknown;
			break;
	}

	return eRetSort;
}

static NX_PhysicalStorageData_t *nx_filemgr_StorageCreatePhysicalStorageData(const DxStorage_Info_t *pstDevInfo)
{
	NX_PhysicalStorageData_t *pstData = (NX_PhysicalStorageData_t*)NX_APP_Calloc(sizeof(NX_PhysicalStorageData_t));

	pstData->ulDeviceId			= pstDevInfo->ulDeviceId;
	pstData->ePairing			= nx_filemgr_StorageConvertDevicePairing(pstDevInfo->ulFlags);
	pstData->eInterface			= nx_filemgr_StorageConvertDeviceInterface(pstDevInfo->eDevType);
	pstData->eInterfaceVersion	= nx_filemgr_StorageConvertDeviceInterfaceVersion(pstDevInfo->eDevType, pstDevInfo->ullDeviceWriteSpeedBps);
	pstData->eType				= nx_filemgr_StorageConvertDeviceType(pstDevInfo->eDevType);
	pstData->eKind				= nx_filemgr_StorageConvertDeviceKind(pstDevInfo->eDevType);
	pstData->eUsage				= nx_filemgr_StorageConvertDeviceUsage(pstDevInfo->ulFlags);
	pstData->pszUuid			= HLIB_STD_StrDup((const HCHAR*)pstDevInfo->szUUID);
	pstData->pszLabel			= HLIB_STD_StrDup((const HCHAR*)pstDevInfo->szLabel);
	pstData->eSort 				= nx_filemgr_StorageGetDeviceSort(pstData);

	NX_PORT_FILEMGR_STORAGE_GetSize(pstDevInfo->ulDeviceId,
		&pstData->ullTotalSize, 	&pstData->ullAvailableSize,		&pstData->ullUsedSize,
		&pstData->ullReservedSize,	&pstData->ullAvailablePvrSize);
	pstData->pszMountpath		= HLIB_STD_StrDup((const HCHAR*)pstDevInfo->szMountPath);

	pstData->bIsmount	 = FALSE;
	if(pstDevInfo->ulFlags & eDxSTORAGE_FLAG_MOUNTED)
		pstData->bIsmount	 = TRUE;

	HxLOG_Print("id     : [%d]\n", pstData->ulDeviceId);
	HxLOG_Print("eDevType     : [%d]\n", pstDevInfo->eDevType);
	HxLOG_Print("ulFlags     : [%d]\n", pstDevInfo->ulFlags);
	HxLOG_Print("Total     : [%f Kb]\n", pstData->ullTotalSize);
	HxLOG_Print("Available : [%f Kb]\n", pstData->ullAvailableSize);
	HxLOG_Print("AvailablePvr  : [%f Kb]\n", pstData->ullAvailablePvrSize);
	HxLOG_Print("Used      : [%f Kb]\n", pstData->ullUsedSize);
	HxLOG_Print("Reserved  : [%f Kb]\n", pstData->ullReservedSize);

	return pstData;
}


static void nx_filemgr_StorageInfoNotifier(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();

	switch(eType)
	{
		case eDxSTORAGE_NOTI_Init:			nx_filemgr_Storage_RcvMsgInit(eType, p1, p2, p3);			break;
		case eDxSTORAGE_NOTI_DevChange:		nx_filemgr_Storage_RcvMsgDevChange(eType, p1, p2, p3);		break;
		case eDxSTORAGE_NOTI_NeedFormat:	nx_filemgr_Storage_RcvMsgNeedFormat(eType, p1, p2, p3);		break;
		case eDxSTORAGE_NOTI_FormatState:	nx_filemgr_Storage_RcvMsgFormatState(eType, p1, p2, p3);	break;
		case eDxSTORAGE_NOTI_DstState:		nx_filemgr_Storage_RcvMsgDstState(eType, p1, p2, p3);		break;
		case eDxSTORAGE_NOTI_OverCurrent:	nx_filemgr_Storage_RcvMsgOverCurrent(eType, p1, p2, p3);	break;
		case eDxSTORAGE_NOTI_UnmountFail:	nx_filemgr_Storage_RcvMsgUnmountFail(eType, p1, p2, p3);	break;
		case eDxSTORAGE_NOTI_CheckDiskStarted:	nx_filemgr_Storage_RcvMsgCheckDiskStarted(eType, p1, p2, p3);	break;
		case eDxSTORAGE_NOTI_CheckUsbSpeed:	nx_filemgr_Storage_RcvMsgCheckUsbSpeed(eType, p1, p2, p3);	break;
		case eDxSTORAGE_NOTI_PvrReady:		nx_filemgr_Storage_RcvMsgPvrReady(eType, p1, p2 ,p3);		break;
		case eDxSTORAGE_NOTI_Result:		nx_filemgr_Storage_RcvMsgResult(eType, p1, p2, p3);			break;
		default:
			HxLOG_Error("Unknown Type : %d \n", eType);
			break;
	}
}

static HBOOL nx_filemgr_Storage_FindStorageInfoCB(void *data, void *userdata)
{
	NX_PhysicalStorageData_t 	*pstStorageData	= (NX_PhysicalStorageData_t *)data;
	HUINT32 					ulDeviceId 		= (HUINT32)userdata;

	if (pstStorageData->ulDeviceId == ulDeviceId)
		return TRUE;
	else
		return FALSE;
}

static HERROR	nx_filemgr_Storage_UpdateStorageInfoData(NX_PhysicalStorageData_t *pstPhysicalStorageData)
{
	HINT32			i, ulStorageItems;
	HBOOL			findDevId = FALSE;
	NX_StorageContext_t 		*pstContext = nx_filemgr_Storage_GetContext();
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;
	NX_PhysicalStorageData_t	*oldPhysicalStorageData = NULL;

	if (pstContext == NULL || pstContext->devList == NULL)
	{
		HxLOG_Debug("NX_StorageContext_t Error!\n");
		return ERR_FAIL;
	}

	ulStorageItems = HLIB_VECTOR_Length(pstContext->devList);
	for (i = 0; i < ulStorageItems; i++)
	{
		physicalStorageData = HLIB_VECTOR_ItemAt(pstContext->devList, i);
		if (physicalStorageData == NULL)
		{
			continue;
		}
		if (physicalStorageData->ulDeviceId == pstPhysicalStorageData->ulDeviceId)
		{
			findDevId = TRUE;
			oldPhysicalStorageData = HLIB_VECTOR_Replace(pstContext->devList, i, pstPhysicalStorageData);
			if (oldPhysicalStorageData != NULL)
			{
				NX_APP_Free(oldPhysicalStorageData);
			}
		}
	}

	if (findDevId == FALSE)
	{
		NX_APP_Free(pstPhysicalStorageData);
	}

	return ERR_OK;
}


static HERROR	nx_filemgr_Storage_AddStorageInfoData(NX_PhysicalStorageData_t *pstPhysicalStorageData)
{
	NX_StorageContext_t *pstContext = nx_filemgr_Storage_GetContext();

	if(pstPhysicalStorageData == NULL)
	{
		HxLOG_Debug("NX_PhysicalStorageData_t is NULL!\n");
		return ERR_FAIL;
	}

	HLIB_VECTOR_Add(pstContext->devList, pstPhysicalStorageData);
	return ERR_OK;
}

static HERROR	nx_filemgr_Storage_RemoveStorageInfoData(HUINT32 ulDeviceId)
{
	HINT32 nRemovedCount = 0;
	NX_StorageContext_t *pstContext = nx_filemgr_Storage_GetContext();

	nRemovedCount = HLIB_VECTOR_RemoveIf(pstContext->devList, nx_filemgr_Storage_FindStorageInfoCB, (void*)ulDeviceId);

	if (nRemovedCount > 0)
		return ERR_OK;
	return ERR_FAIL;
}

static HBOOL nx_filemgr_Storage_FindStorageDevId(HUINT32 storageDevId)
{
	HINT32			i, ulStorageItems;
	HBOOL			findDevId = FALSE;
	NX_StorageContext_t 		*pstContext = nx_filemgr_Storage_GetContext();
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;

	if (pstContext == NULL || pstContext->devList == NULL)
	{
		HxLOG_Debug("NX_StorageContext_t Error!\n");
		return FALSE;
	}

	ulStorageItems = HLIB_VECTOR_Length(pstContext->devList);
	for (i = 0; i < ulStorageItems; i++)
	{
		physicalStorageData = HLIB_VECTOR_ItemAt(pstContext->devList, i);
		if (physicalStorageData == NULL)
		{
			continue;
		}
		if (physicalStorageData->ulDeviceId == storageDevId)
		{
			findDevId = TRUE;
		}
	}

	return findDevId;
}
/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
HERROR	NX_FILEMGR_STORAGE_Init(void)
{
	NX_StorageContext_t 		*pstContext = nx_filemgr_Storage_GetContext();

	NX_PORT_FILEMGR_STORAGE_RegisterNotifier(nx_filemgr_StorageInfoNotifier);

	HxSTD_MemSet(pstContext, 0, sizeof(NX_StorageContext_t));
	pstContext->devList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);

	return ERR_OK;
}

HERROR	NX_FILEMGR_STORAGE_GetPhysicalStorageListLength(HUINT32	*storageLength)
{
	NX_StorageContext_t 		*pstContext = nx_filemgr_Storage_GetContext();

	if (pstContext == NULL || pstContext->devList == NULL)
	{
		HxLOG_Debug("NX_StorageContext_t Error!\n");
		return ERR_FAIL;
	}

	*storageLength = HLIB_VECTOR_Length(pstContext->devList);

	return ERR_OK;
}

HERROR	NX_FILEMGR_STORAGE_GetPhysicalStorageDevIdList(HUINT32 **storageDevIdList, HUINT32 *storageLength)
{
	HINT32			i, ulStorageItems;
	HUINT32			*pDevIdList = NULL;
	NX_StorageContext_t 		*pstContext = nx_filemgr_Storage_GetContext();
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;

	if (pstContext == NULL || pstContext->devList == NULL)
	{
		HxLOG_Debug("NX_StorageContext_t Error!\n");
		return ERR_FAIL;
	}

	ulStorageItems = HLIB_VECTOR_Length(pstContext->devList);
	*storageLength = ulStorageItems;

	pDevIdList = (HUINT32 *)NX_APP_Malloc (sizeof(HUINT32) * ulStorageItems);
	if (pDevIdList == NULL)
	{
		NX_APP_Error("pDevIdList is NULL!!\n");
		return	ERR_FAIL;
	}
	for (i = 0; i < ulStorageItems; i++)
	{
		physicalStorageData = HLIB_VECTOR_ItemAt(pstContext->devList, i);
		if (physicalStorageData == NULL)
		{
			continue;
		}
		pDevIdList[i] = physicalStorageData->ulDeviceId;
	}

	*storageDevIdList = pDevIdList;

	return ERR_OK;
}

HERROR 	NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(HUINT32 storageDevId, NX_PhysicalStorageData_t **physicalStorageInfo)
{
	HINT32			i, ulStorageItems;
	NX_StorageContext_t 		*pstContext = nx_filemgr_Storage_GetContext();
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;

	if (pstContext == NULL || pstContext->devList == NULL)
	{
		HxLOG_Debug("NX_StorageContext_t Error!\n");
		return ERR_FAIL;
	}

	ulStorageItems = HLIB_VECTOR_Length(pstContext->devList);

	for (i = 0; i < ulStorageItems; i++)
	{
		physicalStorageData = HLIB_VECTOR_ItemAt(pstContext->devList, i);
		if (physicalStorageData == NULL)
		{
			continue;
		}
		if (physicalStorageData->ulDeviceId == storageDevId)
		{
			*physicalStorageInfo = physicalStorageData;
			break;
		}
	}

	return ERR_OK;
}

HERROR  NX_FILEMGR_STORAGE_FormatAll(HUINT32 ulDeviceId)
{
	return NX_PORT_FILEMGR_STORAGE_FormatAll(ulDeviceId);
}

HERROR	NX_FILEMGR_STORAGE_SafeRemove(HUINT32 ulDeviceId)
{
	return NX_PORT_FILEMGR_STORAGE_SafeRemove(ulDeviceId);
}

HERROR	NX_FILEMGR_STORAGE_Recovery(HUINT32 ulDeviceId)
{
	return NX_PORT_FILEMGR_STORAGE_Recovery(ulDeviceId);
}

HERROR	NX_FILEMGR_STORAGE_SetStorageName(HUINT32 ulDeviceId, HCHAR *pszLabel)
{
	HERROR		hErr = ERR_FAIL;
	NX_StorageContext_t 		*pstContext = nx_filemgr_Storage_GetContext();
	NX_PhysicalStorageData_t	*physicalStorageData = NULL;

	if (pstContext == NULL || pstContext->devList == NULL)
	{
		HxLOG_Debug("NX_StorageContext_t Error!\n");
		return ERR_FAIL;
	}

	hErr = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(ulDeviceId, &physicalStorageData);
	if (hErr == ERR_OK)
	{
		NX_PORT_FILEMGR_STORAGE_SetStorageName(physicalStorageData->pszUuid, pszLabel);
		physicalStorageData->pszLabel[0] = '\0';
		HxSTD_sprintf((char*)physicalStorageData->pszLabel, "%s", (char*)pszLabel);
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	NX_FILEMGR_STORAGE_Check_HDD_Paired(HBOOL *pbPaired, NX_PhysicalStorageData_t **physicalStorageInfo)
{
	HINT32			i, ulStorageItems;
	NX_StorageContext_t 		*pstContext = nx_filemgr_Storage_GetContext();
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;

	if (pstContext == NULL || pstContext->devList == NULL)
	{
		HxLOG_Debug("NX_StorageContext_t Error!\n");
		return ERR_FAIL;
	}

	ulStorageItems = HLIB_VECTOR_Length(pstContext->devList);
	*pbPaired = FALSE;

	/* find other pvr assigned hdd */
	for (i = 0; i < ulStorageItems; i++)
	{
		physicalStorageData = HLIB_VECTOR_ItemAt(pstContext->devList, i);
		if (physicalStorageData == NULL)
		{
			continue;
		}
		if (physicalStorageData->eUsage == eStorageUsage_PVR)
		{
			*pbPaired = TRUE;
			*physicalStorageInfo = physicalStorageData;
		}
	}

	return ERR_OK;
}

HERROR	NX_FILEMGR_STORAGE_PAIRING_HDD(HUINT32 ulDeviceId, HBOOL bPaired)
{
	HINT32			i, ulStorageItems;
	HBOOL			bOtherAssignPvr = FALSE;
	NX_StorageContext_t 		*pstContext = nx_filemgr_Storage_GetContext();
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;
	NX_PhysicalStorageData_t 	*otherPhysicalStorageData = NULL;

	if (pstContext == NULL || pstContext->devList == NULL)
	{
		HxLOG_Debug("NX_StorageContext_t Error!\n");
		return ERR_FAIL;
	}

	ulStorageItems = HLIB_VECTOR_Length(pstContext->devList);

	/* find other pvr assigned hdd */
	if (bPaired == TRUE)
	{
		NX_FILEMGR_STORAGE_Check_HDD_Paired(&bOtherAssignPvr, &otherPhysicalStorageData);
	}

	for (i = 0; i < ulStorageItems; i++)
	{
		physicalStorageData = HLIB_VECTOR_ItemAt(pstContext->devList, i);
		if (physicalStorageData == NULL)
		{
			continue;
		}
		if (physicalStorageData->ulDeviceId == ulDeviceId)
		{
			if (bPaired == TRUE)
			{
				if (bOtherAssignPvr == TRUE)
				{
					NX_PORT_FILEMGR_STORAGE_DisablePVRHDD(otherPhysicalStorageData->pszUuid);
				}
				NX_PORT_FILEMGR_STORAGE_PairingHdd(physicalStorageData->pszUuid);
			}
			else
			{
				NX_PORT_FILEMGR_STORAGE_UnPairingHdd(physicalStorageData->pszUuid);
			}
		}
	}

	return ERR_OK;
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/


