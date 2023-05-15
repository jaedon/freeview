/**
	@file     	mgr_storage.c
	@brief    							\n

	Description:  						\n
	Module: 		mgr/storage			 	\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <octo_common.h>
#include <stdio.h>	/* For snprintf(3) */
#include <hlib.h>
#include <octo_common.h>
#include <db_param.h>
#include <svc_fs.h>
#include <svc_sys.h>

#include <bus.h>
#include <mgr_common.h>


#include <mgr_action.h>
#include <mgr_pvr_action.h>
#include <mgr_storage.h>


#include <hapi.h>
#include <uapi.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined (CONFIG_PROD_HGS1000S)
// change recording loading trigger from PVR-enabled to playback-device-connected(=RecStorage)
#define RECORDING_LOAD_ON_PLAYBACK_DEVICE
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

void	mgr_storage_umma_Notifier(DxStorage_NotiType_e eType, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HxLOG_Trace();
	if (eDxSTORAGE_NOTI_DevChange == eType)
	{
		HBOOL	bConnect = (HBOOL)nParam1;
		DxStorage_Info_t	*pstDxInfo = (DxStorage_Info_t	*)nParam2;
		HBOOL	bInternal = FALSE, bPvr = FALSE;
		HUINT32	ulPvrIdx = 0xff, i;
		SvcFs_DevInfo_t	stDevInfo;
		SvcFs_DevIdx_e	eDevIdx = (SvcFs_DevIdx_e)pstDxInfo->ulDeviceId;

		HxLOG_Warning("eDevIdx : %d, bConnect : %s \n", eDevIdx, (bConnect)?"TRUE":"FALSE");

		if (pstDxInfo->eDevType == eDxSTORAGE_DEVTYPE_INTHDD || pstDxInfo->eDevType == eDxSTORAGE_DEVTYPE_ESATAHDD)
		{
			bInternal = TRUE;
		}

		if ((pstDxInfo->ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) || (pstDxInfo->ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE))
		{
			bPvr = TRUE;

			for (i = 0 ; i < pstDxInfo->nPartitionNum ; i++)
			{
				if ( (pstDxInfo->astPartitionInfos[i].ulFlags& eDxSTORAGE_FLAG_PVRDEVICE)  || (pstDxInfo->astPartitionInfos[i].ulFlags& eDxSTORAGE_FLAG_SUBPVRDEVICE))
				{
					ulPvrIdx = (HUINT32)pstDxInfo->astPartitionInfos[i].nPvrIdx;
					break;
				}
			}
		}

		if (TRUE == bConnect)
		{
			BUS_PostMessage(NULL, eMEVT_UMMA_NOTIFY_STORAGE_CONNECTED, HANDLE_NULL, (HINT32)eDevIdx, (HINT32)bInternal, (HINT32)bPvr);
		}
		else
		{
			SVC_FS_ConvertDevDxInfo2SvcFs(pstDxInfo, &stDevInfo);
			BUS_PostData(NULL, eMEVT_UMMA_NOTIFY_STORAGE_DISCONNECTED, (Handle_t)NULL, (HINT32)eDevIdx, (HINT32)ulPvrIdx, (void *)&stDevInfo, sizeof(SvcFs_DevInfo_t));
		}
	}

	else if (eDxSTORAGE_NOTI_PvrReady == eType)
	{
		// device의 연결/해제, 혹은 Format으로 인해 PVR device 정보가 바뀌었을때 발생
		HBOOL	bPvr = (HBOOL)nParam1;	//TRUE : PVR storage, FALSE : Not a PVR storage (disable or unpaired)
		SvcFs_DevIdx_e	eDevIdx = (SvcFs_DevIdx_e)nParam2;
		HUINT32	ulPvrIdx =(HUINT32)nParam3;

		if (bPvr == TRUE)
		{
			BUS_PostMessage(NULL, eMEVT_UMMA_NOTIFY_STORAGE_PVR_ENABLED, HANDLE_NULL, (HINT32)eDevIdx, (HINT32)ulPvrIdx, (HINT32)NULL);
		}
		else
		{
			BUS_PostMessage(NULL, eMEVT_UMMA_NOTIFY_STORAGE_PVR_DISABLED, HANDLE_NULL, (HINT32)eDevIdx, (HINT32)ulPvrIdx, (HINT32)NULL);
		}

		HxLOG_Warning("PvrReady : %s, eDevIdx : %d \n", (bPvr)?"T":"F", eDevIdx);

	}
	else if(eDxSTORAGE_NOTI_FormatState == eType)
	{
#if defined(CONFIG_MW_HDD_FORMAT_JIGSW)
		DxStorage_STATUS_Format_e eResultType = (DxStorage_STATUS_Format_e)nParam1;

		switch (eResultType)
		{
			case eDxSTORAGE_DEV_STATUS_Formatting:
				HxLOG_Error("eDxSTORAGE_DEV_STATUS_Formatting\n");
				BUS_PostMessage(NULL, eMEVT_STORAGE_NOTIFY_FORMAT_PROCESS, HANDLE_NULL, (HINT32)NULL, (HINT32)NULL, (HINT32)NULL);
				break;

			case eDxSTORAGE_DEV_STATUS_FormatFail:
				HxLOG_Error("eDxSTORAGE_DEV_STATUS_FormatFail\n");
				BUS_PostMessage(NULL, eMEVT_STORAGE_NOTIFY_FORMAT_FAIL, HANDLE_NULL, (HINT32)NULL, (HINT32)NULL, (HINT32)NULL);
				break;

			case eDxSTORAGE_DEV_STATUS_FormatDone:
				HxLOG_Error("eDxSTORAGE_DEV_STATUS_FormatDone\n");
				BUS_PostMessage(NULL, eMEVT_STORAGE_NOTIFY_FORMAT_DONE, HANDLE_NULL, (HINT32)NULL, (HINT32)NULL, (HINT32)NULL);
				break;

			default:
				break;
		}
#endif
	}

	else if (eDxSTORAGE_NOTI_Result == eType)
	{
		// API Call에 대한 결과를 리턴
		DxStorage_ResultType_e eResultType = (DxStorage_ResultType_e)nParam1;
		HUINT32 	ulDeviceId = (HUINT32)nParam2;	// == eDevIdx
		HERROR	hErr = (HERROR)nParam3;				// API 결과 (ERR_OK:0, ERR_FAIL:1)

		HxLOG_Warning("eResultType : %d, eDevIdx : %d, Result : %d\n", eResultType, ulDeviceId, hErr);
		switch(eResultType)
		{
			case	eDxSTORAGE_RESULT_Format:
				// Add event if it is necessary.
				break;

			case	eDxSTORAGE_RESULT_Dst:
				// Add event if it is necessary.
				break;

			case	eDxSTORAGE_RESULT_SetDefaultHdd:
				// Add event if it is necessary.
				break;

			case	eDxSTORAGE_RESULT_SetName:
				break;

			case	eDxSTORAGE_RESULT_DisableHdd:
				// Add event if it is necessary.
				break;

			case	eDxSTORAGE_RESULT_Pairing:
				break;

			case	eDxSTORAGE_RESULT_UnPairing:
				break;

			case	eDxSTORAGE_RESULT_SafeRemove:
				break;

			default:
				break;
		}
	}
	else if (eDxSTORAGE_NOTI_PlaybackReady == eType)
	{
		HBOOL	bReady = (HBOOL)nParam1;
		SvcFs_DevIdx_e	eDevIdx = (SvcFs_DevIdx_e)nParam2;
		HUINT32	ulPlaybackIdx =(HUINT32)nParam3;

		HxLOG_Warning("PlaybackReady : %s, eDevIdx : %d \n", (bReady)?"T":"F", eDevIdx);

		if (bReady == TRUE)
		{
			BUS_PostMessage(NULL, eMEVT_UMMA_NOTIFY_STORAGE_RECORDING_ENABLED, HANDLE_NULL, (HINT32)eDevIdx, (HINT32)ulPlaybackIdx, (HINT32)NULL);
		}
		else
		{
			BUS_PostMessage(NULL, eMEVT_UMMA_NOTIFY_STORAGE_RECORDING_DISABLED, HANDLE_NULL, (HINT32)eDevIdx, (HINT32)ulPlaybackIdx, (HINT32)NULL);
		}
	}
	else
	{
		HxLOG_Error("Unknown Type : %d \n", eType);
	}

}

STATIC BUS_Result_t mgr_storage_MsgGwmCreate (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Register UMMA_FS Notifier
	UAPI_FS_SetNotifier(mgr_storage_umma_Notifier);

	SVC_SYS_SetLedOnOff (eDxLED_ID_USB, SVC_FS_IsUsbConnected());

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t mgr_storage_MsgGwmDestroy (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}


STATIC BUS_Result_t proc_Storage (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eRet = ERR_BUS_NO_ERROR;
	HERROR				hErr;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Print("[%s (%04d)] eMEVT_BUS_CREATE, p1:[%d], p2:[%d], p3:[%d] >>>\n", __FUNCTION__, __LINE__, p1, p2, p3);
		eRet = mgr_storage_MsgGwmCreate (hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Warning("eMEVT_BUS_DESTROY \n");
		eRet = mgr_storage_MsgGwmDestroy (hAction, p1, p2, p3);
		break;

	/*	p1 : eDevIdx
		p2 : bInternal
		p3 : bPvr */
	case eMEVT_UMMA_NOTIFY_STORAGE_CONNECTED:
	{
		SvcFs_DevInfo_t	stDevInfo;
		HxLOG_Warning("eMEVT_UMMA_NOTIFY_STORAGE_CONNECTED \n");

		hErr = SVC_FS_GetDeviceInfo((SvcFs_DevIdx_e)p1, &stDevInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error(" SVC_FS_GetDeviceInfo Error!!\n");
			break;
		}

		//다른 Proc 들에게 필요한 메시지 보냄
		BUS_PostMessage(NULL, eMEVT_STORAGE_NOTIFY_PLUGGED, (Handle_t)NULL, p1, p2, 0);

		// 기기에 따른 LED control
		if ((stDevInfo.eDevType == eSVC_FS_DevType_UsbHdd) || (stDevInfo.eDevType == eSVC_FS_DevType_UsbMem))
		{
			HLIB_CMD_CaptureLog("USB Device Insertion, eDevIdx=%d", p1);

			// Under fast USB attach/detach, prevent led error by connected/disconnected event conflict
			if (HLIB_DIR_IsMountedPath(stDevInfo.szMountPath) == TRUE)
			{
				SVC_SYS_SetLedOnOff (eDxLED_ID_USB, TRUE);
			}
			else
			{
				HxLOG_Error(" HLIB_DIR_IsMountedPath Error!!\n");
			}
		}

		eRet = MESSAGE_PASS;
		break;
	}

	/* 	   p1: eDevIdx
		   p2: ulPvrIdx
		   p3: SvcFs_DevInfo_t		*/
	case eMEVT_UMMA_NOTIFY_STORAGE_DISCONNECTED:
	{
		HUINT32		ulPvrIdx = (HUINT32)p2;
		SvcFs_DevInfo_t * pstDevInfo = (SvcFs_DevInfo_t *)p3;

		HxLOG_Warning("eMEVT_UMMA_NOTIFY_STORAGE_DISCONNECTED \n");

		if (pstDevInfo->eDevType == eSVC_FS_DevType_IntHdd)
		{
			BUS_PostMessage(NULL, eMEVT_STORAGE_NOTIFY_UNPLUGGED, (Handle_t)NULL, p1, TRUE, ulPvrIdx);
			BUS_PostMessage(NULL, eMEVT_STORAGE_NOTIFY_PVR_DISABLED, (Handle_t)NULL, p1, TRUE, ulPvrIdx);

		}
		else if (pstDevInfo->bPvr == TRUE)
		{
			BUS_PostMessage(NULL, eMEVT_STORAGE_NOTIFY_UNPLUGGED, (Handle_t)NULL, p1, FALSE, ulPvrIdx);
			BUS_PostMessage(NULL, eMEVT_STORAGE_NOTIFY_PVR_DISABLED, (Handle_t)NULL, p1, FALSE, ulPvrIdx);
		}

#if !defined(RECORDING_LOAD_ON_PLAYBACK_DEVICE)
		//PVR device 라면 Recording list scan 삭제 요청
		if (pstDevInfo->bPvr)
		{
			BUS_PostDataToOM(NULL, eMEVT_META_RECLIST_HDD_REMOVED, HANDLE_NULL, ulPvrIdx, (HINT32)DxSTORAGE_INVALID_PVR_INDEX, (void *)pstDevInfo->szMountPath, HxSTD_StrLen((HCHAR*)pstDevInfo->szMountPath)+1);
		}
#endif

		// 기기에 따른 LED control
		if ((pstDevInfo->eDevType == eSVC_FS_DevType_UsbHdd) || (pstDevInfo->eDevType == eSVC_FS_DevType_UsbMem))
		{
			HLIB_CMD_CaptureLog("USB Device Extration, eDevIdx=%d", p1);

			SVC_SYS_SetLedOnOff (eDxLED_ID_USB, FALSE);
		}

		eRet = MESSAGE_PASS;
		break;
	}

	/* 	   p1: eDevIdx
		   p2: ulPvrIdx
		   p3: none		*/
	case eMEVT_UMMA_NOTIFY_STORAGE_PVR_ENABLED:
	{
		HUINT32 ulPvrIdx = (HUINT32)p2;
		SvcFs_DevInfo_t	stDevInfo;
		HxLOG_Warning("eMEVT_UMMA_NOTIFY_STORAGE_PVR_ENABLED \n");

		hErr = SVC_FS_GetDeviceInfo((SvcFs_DevIdx_e)p1, &stDevInfo);
		if (ERR_OK == hErr)
		{
			BUS_PostMessage(NULL, eMEVT_STORAGE_NOTIFY_PVR_ENABLED, (Handle_t)NULL, p1, FALSE, ulPvrIdx);

#if !defined(RECORDING_LOAD_ON_PLAYBACK_DEVICE)
			// Recording list scan
			BUS_PostMessageToOM(NULL, eMEVT_META_RECLIST_HDD_SCANABLE, HANDLE_NULL, ulPvrIdx, DxSTORAGE_INVALID_PVR_INDEX, 0);
#endif
		}


		eRet = MESSAGE_PASS;
		break;
	}
	/* 	   p1: eDevIdx
		   p2: none
		   p3: none		*/
	case eMEVT_UMMA_NOTIFY_STORAGE_PVR_DISABLED:
	{
		HUINT32 ulPvrIdx = (HUINT32)p2;
		SvcFs_DevInfo_t	stDevInfo;
		HxLOG_Warning("eMEVT_UMMA_NOTIFY_STORAGE_PVR_DISABLED \n");

		hErr = SVC_FS_GetDeviceInfo((SvcFs_DevIdx_e)p1, &stDevInfo);
		if (ERR_OK == hErr)
		{
			if(stDevInfo.eDevType == eSVC_FS_DevType_UsbHdd || stDevInfo.eDevType == eSVC_FS_DevType_eSataHdd)
			{
				BUS_PostMessage(NULL, eMEVT_STORAGE_NOTIFY_PVR_DISABLED, (Handle_t)NULL, p1, FALSE, ulPvrIdx);
			}

#if !defined(RECORDING_LOAD_ON_PLAYBACK_DEVICE)
			// Remove recording list
			BUS_PostDataToOM(NULL, eMEVT_META_RECLIST_HDD_REMOVED, HANDLE_NULL, (HINT32)ulPvrIdx,(HINT32)DxSTORAGE_INVALID_PVR_INDEX, (void *)stDevInfo.szMountPath , HxSTD_StrLen((HCHAR*)stDevInfo.szMountPath)+1);
#endif
		}
		eRet = MESSAGE_PASS;
		break;
	}

	case eMEVT_UMMA_NOTIFY_STORAGE_RECORDING_ENABLED:
	{
		SvcFs_DevIdx_e	eDevIdx = (SvcFs_DevIdx_e)p1;
		SvcFs_DevInfo_t	stDevInfo;
		HUINT32			ulPlaybackIdx = (HUINT32)p2;
		HxLOG_Warning("eMEVT_UMMA_NOTIFY_STORAGE_RECORDING_ENABLED \n");

		hErr = SVC_FS_GetDeviceInfo((SvcFs_DevIdx_e)eDevIdx, &stDevInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_FS_GetDeviceInfo failed. devIdx(%d) playbackIdx(%d) \n",eDevIdx,ulPlaybackIdx);
			eRet = MESSAGE_PASS;
			break;
		}

		BUS_PostMessageToOM(NULL, eMEVT_META_RECLIST_HDD_SCANABLE, HANDLE_NULL, DxSTORAGE_INVALID_PVR_INDEX, (HINT32)ulPlaybackIdx, 0);
		eRet = MESSAGE_PASS;
		break;
	}

	case eMEVT_UMMA_NOTIFY_STORAGE_RECORDING_DISABLED:
	{
		HUINT32 ulPlaybackIdx = (HUINT32)p2;
		HxLOG_Warning("eMEVT_UMMA_NOTIFY_STORAGE_RECORDING_DISABLED \n");

		BUS_PostMessageToOM(NULL, eMEVT_META_RECLIST_HDD_REMOVED, HANDLE_NULL, DxSTORAGE_INVALID_PVR_INDEX,(HINT32)ulPlaybackIdx, 0);
		eRet = MESSAGE_PASS;
		break;
	}

	case eMEVT_BUS_READY_SHUTDOWN:
		// 맘에 안들지만 일단 이걸 그대로 두어서 죽어버리면 Storage Enable 메세지가 안올라가 레코딩이 안됨....
		return MESSAGE_PASS;

	default:
		eRet = ERR_BUS_NO_ERROR;
		break;
	}

	if (eRet != ERR_BUS_NO_ERROR)
	{
		return eRet;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#define __________GLOBAL_FUNCTIONS__________

HERROR MGR_STORAGE_Start(void)
{

	BUS_MGR_Create("proc_Storage", APP_MONITORING_MGR_PRIORITY, proc_Storage, 0, 0, 0, 0);
	return ERR_OK;
}

/* end of file */
