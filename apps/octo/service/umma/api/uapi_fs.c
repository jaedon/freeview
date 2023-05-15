/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  uapi_fs.c
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
//#include <vkernel.h>
//#include <hlib.h>
//#include <oapi.h>
//#include <obama_int.h>
//
//#include "../../oapi_mem.h"

#include	<hlib.h>
#include 	<dlib.h>
#include	<vkernel.h>

#include	<uapi.h>
#include	<umma_int.h>

#include <fcntl.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/mount.h>
#include <arpa/inet.h>



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_UAPI_FS_CACHE		HxSEMT_Get(s_ulUapiFsSemId)
#define	LEAVE_UAPI_FS_CACHE		HxSEMT_Release(s_ulUapiFsSemId)

#if defined(CONFIG_PROD_CPU_BCM)
#define SMART_BUF_SIZE 				(516)		// 512+4
#define SMART_CMD					(0x031F)
#define SMART_BLKGETSIZE64		_IOR(0x12,114,size_t)
#define SERIAL_NUM_INDEX 			(10*2)
#define ATA_SERIAL_NUM_LEN			(20)

static HUINT8 pszHddSn[ATA_SERIAL_NUM_LEN];
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
//STATIC HUINT32				s_ulUapiFsSemId = 0;
STATIC HINT32					s_nStorageNum = 0;
STATIC DxStorage_Info_t		*s_pstStorageInfos = NULL;
STATIC DxStorageNotifier_t		s_pfnUpaiFsListener = NULL;
STATIC HBOOL					s_bStoragePowerOff = FALSE;
STATIC const HCHAR* s_NullStr = "";
STATIC HBOOL					s_bUseTsrOnlyDevice = FALSE;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

STATIC void 		uapi_fs_printDeviceInfo(DxStorage_Info_t *pstInfo);
STATIC UAPI_t*	uapi_fs_getInstance(void);
STATIC void		uapi_fs_releaseInstance(UAPI_t *uapi);



#define _____NOTIFICATION_CALLBACKS_____
STATIC HERROR	uapi_fs_onInit(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	if (s_pfnUpaiFsListener)
	{
		HxLOG_Debug("eDxSTORAGE_NOTI_Init \n");
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_Init, 0,0,0);
	}
	return ERR_OK;
}

//	apArgv[0] : nDeviceId (i)
//	apArgv[1] : nConnected (i)
//	apArgv[2] : DxStorage_Info_t * pstItem (b)
//STATIC void	uapi_fs_onDeviceConnected(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], void *userdata)
STATIC HERROR	uapi_fs_onDeviceConnected(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 	ulDeviceId;
	HBOOL		bConnected;
	DxStorage_Info_t	*pstItem = NULL;
	HINT32	nSize;

	ulDeviceId = HxOBJECT_INT(apArgv[0]);
	bConnected = HxOBJECT_INT(apArgv[1]);
	pstItem = (DxStorage_Info_t *)UAPI_MemDup(HxOBJECT_BIN_DATA(apArgv[2]), HxOBJECT_BIN_SIZE(apArgv[2]));
	nSize = HxOBJECT_BIN_SIZE(apArgv[2]);

	if (pstItem == NULL)
	{
		HxLOG_Error("MemDup Fail!! \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("[uapi_fs_onDeviceConnected] ulDeviceId : %d, bConnected : %d\n", pstItem->ulDeviceId, bConnected);

	if (s_pfnUpaiFsListener)
	{
		HxLOG_Debug("ulDeviceId : %d Notified \n", pstItem->ulDeviceId);
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_DevChange, (HINT32)bConnected, (HINT32)pstItem, nSize);
	}
	if(pstItem) {UAPI_Free((void *)pstItem);}

	return ERR_OK;
}

//	apArgv[0] : nStorageNum (i)
//	apArgv[1] : OxFileMgrStorageInfo_t * nStorageNum (b)   (if only	nStorageNum > 0)
STATIC HERROR	uapi_fs_onChangeDevice(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32	nStorageNum;
	UAPI_t	*uapi;

	uapi = uapi_fs_getInstance();

	if (s_pstStorageInfos)
		UAPI_Free(s_pstStorageInfos);

	s_nStorageNum  = 0;
	s_pstStorageInfos = NULL;



	nStorageNum = HxOBJECT_INT(apArgv[0]);

	if (nStorageNum)
	{
		s_nStorageNum = HxOBJECT_INT(apArgv[0]);
		s_pstStorageInfos = (DxStorage_Info_t *)UAPI_MemDup(HxOBJECT_BIN_DATA(apArgv[1]), HxOBJECT_BIN_SIZE(apArgv[1]));

		HxLOG_Debug("s_nStorageNum : %d\n", s_nStorageNum);

	}
	else
	{
		HxLOG_Debug("Empty list~! \n");
		// Empty list
	}

#if defined(CONFIG_DEBUG)
	{
		HUINT32	ulIdx;

		for(ulIdx = 0 ; ulIdx < s_nStorageNum ; ulIdx++)
		{
			uapi_fs_printDeviceInfo(&s_pstStorageInfos[ulIdx]);
		}
	}
#endif

	uapi_fs_releaseInstance(uapi);

	return ERR_OK;
}

//	apArgv[0] : nDeviceId (i)
STATIC HERROR	uapi_fs_onNeedFormat(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32		ulDeviceId;

	ulDeviceId = HxOBJECT_INT(apArgv[0]);

	if (s_pfnUpaiFsListener)
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_NeedFormat, (HINT32)ulDeviceId, (HINT32)NULL, (HINT32)NULL);
	}
	return ERR_OK;
}

//	apArgv[0] : nDeviceId (i)
STATIC HERROR	uapi_fs_onOverCurrent(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32		ulDeviceId;

	ulDeviceId = HxOBJECT_INT(apArgv[0]);

	if (s_pfnUpaiFsListener)
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_OverCurrent, (HINT32)ulDeviceId, (HINT32)NULL, (HINT32)NULL);
	}

	return ERR_OK;

}

//	apArgv[0] : nDeviceId (i)
//	apArgv[1] : nProgress (i)
STATIC HERROR	uapi_fs_onFormatStateChange(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32		ulDeviceId;
	HINT32		nProgress;

	ulDeviceId = HxOBJECT_INT(apArgv[0]);
	nProgress = HxOBJECT_INT(apArgv[1]);

	if (s_pfnUpaiFsListener == NULL)
	{
		return ERR_FAIL;
	}

	if (nProgress == 100)	// format done
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_FormatState, (HINT32)eDxSTORAGE_DEV_STATUS_FormatDone, (HINT32)ulDeviceId, (HINT32)NULL);
	}
	else if (nProgress == -1)	// format fail
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_FormatState, (HINT32)eDxSTORAGE_DEV_STATUS_FormatFail, (HINT32)ulDeviceId, (HINT32)NULL);
	}
	else	// formating
	{
		DxStorage_Progress_t			 *pstProgress = NULL;

		pstProgress = (DxStorage_Progress_t *)UAPI_Calloc(sizeof(DxStorage_Progress_t));

		if (pstProgress == NULL)
		{
			HxLOG_Error("Calloc Fail!!! \n");
			return ERR_FAIL;
		}

		pstProgress->eType =eDxSTORAGE_PROGRESS_Format;
		pstProgress->ulDeviceId = ulDeviceId;
		pstProgress->ulCurrent = nProgress;
		pstProgress->ulTotal = 100;

		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_FormatState, (HINT32)eDxSTORAGE_DEV_STATUS_Formatting, (HINT32)pstProgress, (HINT32)sizeof(DxStorage_Progress_t));
		if (pstProgress)		{	UAPI_Free((void *)pstProgress);	}
	}


	return ERR_OK;

}

//	apArgv[0] : nDeviceId (i)
//	apArgv[1] : nProgress (i) -- eOxFILEMGRSTORAGE_DST_OK, ....
STATIC HERROR	uapi_fs_onDSTStateChange(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32		ulDeviceId;
	HINT32		nStatus;

	ulDeviceId = HxOBJECT_INT(apArgv[0]);
	nStatus = HxOBJECT_INT(apArgv[1]);

	if (s_pfnUpaiFsListener == NULL)
	{
		return ERR_FAIL;
	}

	if (nStatus == eDxSTORAGE_DEV_STATUS_DstResultOnGoing)
	{
		DxStorage_Progress_t			 *pstProgress = NULL;

		pstProgress = (DxStorage_Progress_t *)UAPI_Calloc(sizeof(DxStorage_Progress_t));

		if (pstProgress == NULL)
		{
			HxLOG_Error("Calloc Fail!!! \n");
			return ERR_FAIL;
		}

		pstProgress->eType =eDxSTORAGE_PROGRESS_Dst;
		pstProgress->ulDeviceId = ulDeviceId;
		pstProgress->ulCurrent = 0;
		pstProgress->ulTotal = 100;

		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_DstState, (HINT32)eDxSTORAGE_DEV_STATUS_DstResultOnGoing, (HINT32)pstProgress, (HINT32)sizeof(DxStorage_Progress_t));
		if (pstProgress)		{	UAPI_Free((void *)pstProgress);	}
	}
	else	// formating
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_DstState, (HINT32)nStatus, (HINT32)ulDeviceId, (HINT32)NULL);
	}

	return ERR_OK;
}

//	apArgv[0] : nDeviceId (i)
//	apArgv[1] : nReady (i)
//	apArgv[2] : nDefaultPvr (i)
//	apArgv[3] : ulPvrIdx (i)
STATIC HERROR	uapi_fs_onPVRReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32		ulDeviceId, ulIdx, ulPvrIdx;
	HBOOL		bReady, bDefaultPvr;
	UAPI_t	*uapi;
	uapi = uapi_fs_getInstance();

	ulDeviceId = HxOBJECT_INT(apArgv[0]);
	bReady = HxOBJECT_INT(apArgv[1]);
	bDefaultPvr = HxOBJECT_INT(apArgv[2]);
	ulPvrIdx = HxOBJECT_INT(apArgv[3]);

//	ENTER_UAPI_FS_CACHE;
	HxLOG_Debug("ulDeviceId : %d, bReady : %d, bDefaultPvr : %d, ulPvrIdx : %d \n",ulDeviceId, bReady, bDefaultPvr, ulPvrIdx);

	for(ulIdx = 0 ; ulIdx < s_nStorageNum ; ulIdx++)
	{
		if (s_pstStorageInfos[ulIdx].ulDeviceId == ulDeviceId)
		{
			if(bReady)
			{
				s_pstStorageInfos[ulIdx].ulFlags |= eDxSTORAGE_FLAG_PVRDEVICE;
				s_pstStorageInfos[ulIdx].ulFlags |= eDxSTORAGE_FLAG_PAIRED;
				if(bDefaultPvr)
				{
					s_pstStorageInfos[ulIdx].ulFlags |= eDxSTORAGE_FLAG_DEFAULTHDD;
				}
			}
			else
			{
				s_pstStorageInfos[ulIdx].ulFlags &= (~eDxSTORAGE_FLAG_PVRDEVICE);
				s_pstStorageInfos[ulIdx].ulFlags &= (~eDxSTORAGE_FLAG_PAIRED);
				s_pstStorageInfos[ulIdx].ulFlags &= (~eDxSTORAGE_FLAG_DEFAULTHDD);
			}
			break;
		}
	}

	uapi_fs_releaseInstance(uapi);
//	LEAVE_UAPI_FS_CACHE;

	HxLOG_Debug("[%s] [%d] ulDeviceId : %d, bReady : %d \n",__FUNCTION__,__LINE__, ulDeviceId, bReady);

	if (s_pfnUpaiFsListener)
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_PvrReady, (HINT32)bReady, (HINT32)ulDeviceId, (HINT32)ulPvrIdx);
	}

	return ERR_OK;
}

//	apArgv[0] : nDeviceId (i)
//	apArgv[1] : nReady (i)
STATIC HERROR	uapi_fs_onTSRReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32		ulDeviceId, ulIdx;
	//HUINT32		ulPvrIdx;
	HBOOL		bReady;
	UAPI_t	*uapi;
	uapi = uapi_fs_getInstance();

	ulDeviceId = HxOBJECT_INT(apArgv[0]);
	bReady = HxOBJECT_INT(apArgv[1]);

	HxLOG_Debug("[uapi_fs_onTSRReady] ulDeviceId : %d, bReady : %d\n",ulDeviceId, bReady);
	for(ulIdx = 0 ; ulIdx < s_nStorageNum ; ulIdx++)
	{
		if (s_pstStorageInfos[ulIdx].ulDeviceId == ulDeviceId)
		{
			if(bReady)
			{
				s_pstStorageInfos[ulIdx].ulFlags |= eDxSTORAGE_FLAG_TSRDEVICE;
				s_pstStorageInfos[ulIdx].ulFlags |= eDxSTORAGE_FLAG_PAIRED;
				s_bUseTsrOnlyDevice = TRUE;
			}
			else
			{
				s_pstStorageInfos[ulIdx].ulFlags &= (~eDxSTORAGE_FLAG_TSRDEVICE);
				s_pstStorageInfos[ulIdx].ulFlags &= (~eDxSTORAGE_FLAG_PAIRED);
				s_bUseTsrOnlyDevice = FALSE;
			}
			break;
		}
	}
	uapi_fs_releaseInstance(uapi);
	HxLOG_Debug("[%s] [%d] s_bUseTsrOnlyDevice = %d\n",__FUNCTION__,__LINE__, s_bUseTsrOnlyDevice);

	if (s_pfnUpaiFsListener)
	{
		// TSR Device is not notify event
		//s_pfnUpaiFsListener(eDxSTORAGE_NOTI_PvrReady, (HINT32)bReady, (HINT32)ulDeviceId, (HINT32)ulPvrIdx);
		HxLOG_Warning("[uapi_fs_onTSRReady] TSR Device is not notify event!!!\n");
	}

	return ERR_OK;
}

STATIC HERROR	uapi_fs_onPlaybackReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32		ulDeviceId, ulIdx, ulPlaybackIdx;
	HBOOL		bReady;
	UAPI_t	*uapi;
	uapi = uapi_fs_getInstance();

	ulDeviceId = HxOBJECT_INT(apArgv[0]);
	bReady = HxOBJECT_INT(apArgv[1]);
	ulPlaybackIdx = HxOBJECT_INT(apArgv[3]);

	HxLOG_Debug("ulDeviceId : %d, bReady : %d\n",ulDeviceId, bReady);
	for(ulIdx = 0 ; ulIdx < s_nStorageNum ; ulIdx++)
	{
		if (s_pstStorageInfos[ulIdx].ulDeviceId == ulDeviceId)
		{
			if(bReady)
			{
				s_pstStorageInfos[ulIdx].ulFlags |= eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE;
			}
			else
			{
				s_pstStorageInfos[ulIdx].ulFlags &= (~eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE);
			}
			break;
		}
	}
	uapi_fs_releaseInstance(uapi);

	if (s_pfnUpaiFsListener)
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_PlaybackReady, (HINT32)bReady, (HINT32)ulDeviceId, (HINT32)ulPlaybackIdx);
	}

	return ERR_OK;
}

STATIC HERROR	uapi_fs_onUnmountFail(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32		ulDeviceId;

	ulDeviceId = HxOBJECT_INT(apArgv[0]);

	HxLOG_Debug("Unmount Fail!!  ulDeviceId : %d \n", ulDeviceId);

	if (s_pfnUpaiFsListener)
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_UnmountFail, (HINT32)ulDeviceId, (HINT32)NULL, (HINT32)NULL);
	}

	return ERR_OK;
}


STATIC HERROR	uapi_fs_onCheckDiskStarted(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32		ulDeviceId;

	ulDeviceId = HxOBJECT_INT(apArgv[0]);

	HxLOG_Debug("Check disk is started!!  ulDeviceId : %d \n", ulDeviceId);

	if (s_pfnUpaiFsListener)
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_CheckDiskStarted, (HINT32)ulDeviceId, (HINT32)NULL, (HINT32)NULL);
	}

	return ERR_OK;
}


STATIC HERROR	uapi_fs_onCheckUsbSpeed(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32		ulDeviceId, ulReadMbps, ulWriteMbps;

	ulDeviceId = HxOBJECT_INT(apArgv[0]);
	ulReadMbps = HxOBJECT_INT(apArgv[1]);
	ulWriteMbps = HxOBJECT_INT(apArgv[2]);

	HxLOG_Warning("ulDeviceId : %d, ulReadMbps : %d, ulWriteMbps : %d\n", ulDeviceId, ulReadMbps, ulWriteMbps);

	if (s_pfnUpaiFsListener)
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_CheckUsbSpeed, (HINT32)ulDeviceId, (HINT32)ulReadMbps, (HINT32)ulWriteMbps);
	}

	return ERR_OK;
}

STATIC HERROR	uapi_fs_onStoragePowerOff(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UAPI_t	*uapi;
	uapi = uapi_fs_getInstance();
	HxLOG_Warning("bStoragePowerOff= TRUE\n");

	s_bStoragePowerOff= TRUE;

	uapi_fs_releaseInstance(uapi);

	return ERR_OK;
}

// apArgv[0] : hErr
// apArgv[1] : DxStorage_ResultType_e
// userdata : storageId
//STATIC void	uapi_fs_onResultNotifier(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
STATIC HERROR	uapi_fs_onResultNotifier(void *pvUserData, HINT32 nRequestId, UAPI_STATUS_t *pstInfo)
{
	HUINT32		ulDeviceId;
	DxStorage_ResultType_e		eResultType;
	HxPair_t	*pstPair = (HxPair_t *)pvUserData;
	HERROR		hErr;
	hErr = pstInfo->ulErr;

	eResultType = (DxStorage_ResultType_e)pstPair->left;
	ulDeviceId = (HUINT32)pstPair->right;

	UAPI_Free(pstPair);

	HxLOG_Debug("[%s] [%d]eResultType : %d, ulDeviceId : %d, hErr : %d\n",__FUNCTION__,__LINE__, eResultType, ulDeviceId, hErr);
	if (s_pfnUpaiFsListener)
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_Result, (HINT32)eResultType, (HINT32)ulDeviceId, (HINT32)hErr);
	}

	return ERR_OK;
}


STATIC HERROR	uapi_fs_onResult(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UAPI_t	*pstUapi = (UAPI_t*)pvUserData;
	UAPI_CBINFO_t	*pstCbInfo;

	UAPI_STATUS_t	stStatus;
	HINT32			_nRequestId;

	UAPI_Noti_t 		 pfnEventCallback = NULL;
	void				*pvUser = NULL;

	HERROR		hErr;

	if (nArgc != 2)
	{
		HxLOG_Error("nArgc is not valid [%d:2]\n", nArgc);
		return ERR_FAIL;
	}

	_nRequestId = HxOBJECT_INT(apArgv[0]);

	HxSEMT_Get(pstUapi->ulSemId);
	pstCbInfo = HLIB_HASH_Lookup(pstUapi->pstCallbackTable, (void*)_nRequestId);
	if (pstCbInfo)
	{
		hErr = HxOBJECT_INT(apArgv[1]);
		stStatus.ulErr = (HUINT32)hErr;	// only used
		stStatus.nDoneOperationNum = 0;
		stStatus.nTotalOperationNum = 0;

		pfnEventCallback = pstCbInfo->cb.pfnEventCallback;
		pvUser = pstCbInfo->pvUser;

		if (stStatus.nTotalOperationNum == stStatus.nDoneOperationNum)
		{
			HLIB_HASH_Remove(pstUapi->pstCallbackTable, (void*)_nRequestId);
		}
	}
	HxSEMT_Release(pstUapi->ulSemId);
	
	HxLOG_Debug("Call registered function : [%d:%d]\n", _nRequestId, nRequestId);
	if (NULL != pfnEventCallback)
	{
		pfnEventCallback(pvUser, _nRequestId, &stStatus);
	}

	return ERR_OK;
}

STATIC HERROR	uapi_fs_onDefaultHddInfoChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HCHAR		*szOldUuid;
	HCHAR		*szNewUuid;
	HUINT16		usOldRec;
	HUINT16		usNewRec;
	HUINT32		ulPackedRec;

	if (nArgc != 4)
	{
		HxLOG_Error("nArgc is not valid [%d:4]\n", nArgc);
		return ERR_FAIL;
	}

	szOldUuid = (HINT8 *)UAPI_StrDup((const HCHAR*)HxOBJECT_STR(apArgv[0]));
	szNewUuid = (HINT8 *)UAPI_StrDup((const HCHAR*)HxOBJECT_STR(apArgv[1]));
	usOldRec = (HUINT16)HxOBJECT_INT(apArgv[2]);
	usNewRec = (HUINT16)HxOBJECT_INT(apArgv[3]);

	ulPackedRec = (HUINT32)((HUINT32)usOldRec << 16) | (HUINT32)usNewRec;

	if (s_pfnUpaiFsListener)
	{
		s_pfnUpaiFsListener(eDxSTORAGE_NOTI_DefaultHddInfoChanged, (HINT32)szOldUuid, (HINT32)szNewUuid, (HINT32)ulPackedRec);
	}

	if (szOldUuid != NULL)
	{
		UAPI_Free(szOldUuid);
	}
	if (szNewUuid != NULL)
	{
		UAPI_Free(szNewUuid);
	}
	return ERR_OK;
}

#define _______________
static HCHAR *uapi_fs_strip (HCHAR *s)
{
	HCHAR *e;

	while (*s == ' ')
	{
		++s;
	}

	if (*s)
	{
		for (e = s + strlen(s); *--e == ' '; *e = '\0')
		{
			;
		}
	}
	return s;
}

STATIC HERROR	uapi_fs_event_AddEventListener(UAPI_t *uapi, HINT32 nRequestId, void *pvUser, UAPI_Noti_t onEventOccurred)
{
	UAPI_CBINFO_t	*pstCbInfo;

	pstCbInfo = (UAPI_CBINFO_t*)HLIB_HASH_Lookup(uapi->pstCallbackTable, (void*)nRequestId);
	if (pstCbInfo)
	{
		HxLOG_Error("Callback is already registered! [%d]\n", nRequestId);
		return ERR_FAIL;
	}

	pstCbInfo = (UAPI_CBINFO_t*)UAPI_Calloc(sizeof(UAPI_CBINFO_t));
	pstCbInfo->nRequestId = nRequestId;
	pstCbInfo->cb.pfnEventCallback = onEventOccurred;
	pstCbInfo->pvUser = pvUser;
	HLIB_HASH_Insert(uapi->pstCallbackTable, (void*)nRequestId, (void*)pstCbInfo);

	return ERR_OK;
}

static UAPI_t	*s_pstUapi = NULL;

STATIC UAPI_t*	uapi_fs_getInstanceWO(void)
{
	return s_pstUapi;
}

STATIC UAPI_t*	uapi_fs_getInstance(void)
{
	if (s_pstUapi == NULL)
	{
		HERROR	hErr;

		s_pstUapi = (UAPI_t*)UAPI_Calloc(sizeof(UAPI_t));

		HxSEMT_Create(&s_pstUapi->ulSemId, "uapifssem", 0);

		HxSEMT_Get(s_pstUapi->ulSemId);
		hErr = UAPI_RpcProtocol_GetRpcHandle(&s_pstUapi->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error !! (GetRpcHandle)\n");
			HxLOG_Assert(s_pstUapi->nRpcHandle);
		}

		s_pstUapi->pstCallbackTable = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, UAPI_Free_CB);

		//	Register UMMA Event Notifier
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onInit,  NULL ,(void*)s_pstUapi, uapi_fs_onInit);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onDeviceConnected,  "iib" ,(void*)s_pstUapi, uapi_fs_onDeviceConnected);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onChangeDevice,  "ib", (void*)s_pstUapi, uapi_fs_onChangeDevice);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onNeedFormat, "i", (void*)s_pstUapi,  uapi_fs_onNeedFormat);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onOverCurrent,    "i"   , (void*)s_pstUapi, uapi_fs_onOverCurrent);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onFormatStateChange, "ii", (void*)s_pstUapi, uapi_fs_onFormatStateChange);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onDSTStateChange,  "ii"  , (void*)s_pstUapi, uapi_fs_onDSTStateChange);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onPVRReady,       "iiii"   , (void*)s_pstUapi, uapi_fs_onPVRReady);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onTSRReady,       "ii"   , (void*)s_pstUapi, uapi_fs_onTSRReady);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onPlaybackReady,  "iiii"   , (void*)s_pstUapi, uapi_fs_onPlaybackReady);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onUnmountFail,     "i"     , (void*)s_pstUapi, uapi_fs_onUnmountFail);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onCheckDiskStarted,     "i"     , (void*)s_pstUapi, uapi_fs_onCheckDiskStarted);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onCheckUsbSpeed,     "iii"     , (void*)s_pstUapi, uapi_fs_onCheckUsbSpeed);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onStoragePowerOff,     NULL     , (void*)s_pstUapi, uapi_fs_onStoragePowerOff);

		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onResult,   "ii", (void*)s_pstUapi, uapi_fs_onResult);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapi->nRpcHandle, RPC_UMMA_FS_onDefaultHddInfoChanged,   "ssii", (void*)s_pstUapi, uapi_fs_onDefaultHddInfoChanged);
	}
	else
	{
		HxSEMT_Get(s_pstUapi->ulSemId);
	}

	return s_pstUapi;
}

STATIC void		uapi_fs_releaseInstance(UAPI_t *uapi)
{
	HxSEMT_Release(uapi->ulSemId);
}

#define _____


STATIC HBOOL	uapi_fs_Filter(DxStorage_Info_t *pstDevInfo, HUINT32 ulFilter)
{
	HBOOL	bCheckType, bCheckConnection, bCheckMount, bCheckUsage, bCheckKind;
	HBOOL	bMounted, bPvr, bHdd, bPlayback;
	HBOOL	bAdd;

	HxLOG_Debug("MountPath : %s  ulFilter : 0x%x\n", pstDevInfo->szMountPath,ulFilter);
	if (NULL == pstDevInfo)
		return FALSE;

	if (eDxSTORAGE_FILTER_None == ulFilter)
		return FALSE;

	if (eDxSTORAGE_FILTER_All == ulFilter)
		return TRUE;

	bCheckType			= (ulFilter & eDxSTORAGE_FILTER_TypeAll) ? TRUE : FALSE;
	bCheckConnection	= (ulFilter & eDxSTORAGE_FILTER_ConnectionAll) ? TRUE : FALSE;
	bCheckMount 		= (ulFilter & eDxSTORAGE_FILTER_MountAll) ? TRUE : FALSE;
	bCheckUsage			= (ulFilter & eDxSTORAGE_FILTER_UsageAll) ? TRUE : FALSE;
	bCheckKind		= (ulFilter & eDxSTORAGE_FILTER_KindAll) ? TRUE : FALSE;

	HxLOG_Debug("bCheckType [%s], bCheckConnection [%s], bCheckMount [%s], bCheckUsage [%s] >>>\n",
							bCheckType ? "TRUE" : "FALSE", bCheckConnection ? "TRUE" : "FALSE",
							bCheckMount ? "TRUE" : "FALSE", bCheckUsage ? "TRUE" : "FALSE");
	if (TRUE == bCheckType)
	{
		if (eDxSTORAGE_FILTER_TypeAll != (ulFilter & eDxSTORAGE_FILTER_TypeAll))
		{
			if (pstDevInfo->eDevType == eDxSTORAGE_DEVTYPE_NONE)
			{
				return FALSE;
			}

			if (ulFilter & eDxSTORAGE_FILTER_Internal ||ulFilter & eDxSTORAGE_FILTER_External)
			{
				switch (pstDevInfo->eDevType)
				{
					case eDxSTORAGE_DEVTYPE_INTHDD:
					case eDxSTORAGE_DEVTYPE_ESATAHDD:
					case eDxSTORAGE_DEVTYPE_DETACHABLE:
					case eDxSTORAGE_DEVTYPE_USBHDD:
						bHdd = TRUE;
						break;
					default:
						bHdd = FALSE;
						break;
				}
				return bHdd;
			}
			else
			{
				return FALSE;
			}
		}
	}

	if (TRUE == bCheckConnection)
	{
		if (eDxSTORAGE_FILTER_ConnectionAll != (ulFilter & eDxSTORAGE_FILTER_ConnectionAll))
		{
			if (pstDevInfo->ulFlags & eDxSTORAGE_FLAG_NONE)
			{
				return FALSE;
			}

			if (ulFilter & eDxSTORAGE_FILTER_Connected)
			{
				if ((pstDevInfo->ulFlags & eDxSTORAGE_FLAG_CONNECTED) == FALSE)
					return FALSE;
			}
			else if (ulFilter & eDxSTORAGE_FILTER_Disconnected)
			{
				if (pstDevInfo->ulFlags & eDxSTORAGE_FLAG_CONNECTED)
					return FALSE;
			}
		}
	}

	if (TRUE == bCheckMount)
	{
		if (eDxSTORAGE_FILTER_MountAll != (ulFilter & eDxSTORAGE_FILTER_MountAll))
		{
			if ((pstDevInfo->ulFlags & eDxSTORAGE_FLAG_MOUNTED) == FALSE)
			{
				return FALSE;
			}

			bMounted = (pstDevInfo->ulFlags & eDxSTORAGE_FLAG_MOUNTED) ? TRUE : FALSE;

			bAdd = (ulFilter & eDxSTORAGE_FILTER_Mounted) ? bMounted : !bMounted;
			if (FALSE == bAdd)
			{
				return FALSE;
			}
		}
	}

	if (TRUE == bCheckUsage)
	{
		if (eDxSTORAGE_FILTER_UsageAll != (ulFilter & eDxSTORAGE_FILTER_UsageAll))
		{
			if (((pstDevInfo->ulFlags & eDxSTORAGE_FLAG_CONNECTED) == TRUE)
				&& ((pstDevInfo->ulFlags & eDxSTORAGE_FLAG_MOUNTED) == FALSE))
			{
				return FALSE;
			}

			bPvr = (pstDevInfo->ulFlags & (eDxSTORAGE_FLAG_PVRDEVICE|eDxSTORAGE_FLAG_SUBPVRDEVICE)) ? TRUE : FALSE;
			bPlayback = (pstDevInfo->ulFlags & (eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE)) ? TRUE : FALSE;

			if (ulFilter & eDxSTORAGE_FILTER_PVR)
			{
				bAdd = bPvr;
			}
			else if (ulFilter & eDxSTORAGE_FILTER_Playback)
			{
				bAdd = bPlayback;
			}
			else
			{
				bAdd = FALSE;
			}

			if (FALSE == bAdd)
			{
				return FALSE;
			}
		}
	}


	if (TRUE == bCheckKind)
	{
		if (eDxSTORAGE_FILTER_KindAll != (ulFilter & eDxSTORAGE_FILTER_KindAll))
		{
			if (pstDevInfo->eDevType == eDxSTORAGE_DEVTYPE_NONE)
			{
				return FALSE;
			}

			switch (pstDevInfo->eDevType)
			{
				case eDxSTORAGE_DEVTYPE_INTHDD:
				case eDxSTORAGE_DEVTYPE_DETACHABLE:
				case eDxSTORAGE_DEVTYPE_ESATAHDD:
				case eDxSTORAGE_DEVTYPE_USBHDD:
					bHdd = TRUE;
					break;
				default:
					bHdd = FALSE;
					break;
			}
			bAdd = (ulFilter & eDxSTORAGE_FILTER_Hdd) ? bHdd : !bHdd;

			if (FALSE == bAdd)
			{
				return FALSE;
			}
		}
	}

	HxLOG_Debug("Add Device Id [%d] >>>\n", pstDevInfo->ulDeviceId);
	return TRUE;

}


STATIC HERROR uapi_fs_GetList(HUINT32 ulFilter, DxStorage_Info_t **pastInfoList, HUINT32 *pulDevNum)
{
	HUINT32 			ulCount, ulIdx;
	HUINT32 			ulStorageNum;
	DxStorage_Info_t	*astList;
	HBOOL			 bAdd;
	UAPI_t	*uapi;

	uapi = uapi_fs_getInstance();

	ulStorageNum = s_nStorageNum;

	if (pastInfoList) 	*pastInfoList = NULL;
	if (pulDevNum)	*pulDevNum = 0;

	ulIdx = 0;
	if (ulStorageNum > 0)
	{
		astList = UAPI_Malloc(sizeof(DxStorage_Info_t) * ulStorageNum);
		if (NULL == astList)
		{
			uapi_fs_releaseInstance(uapi);
			return ERR_FAIL;
		}

		HxSTD_MemSet(astList, 0, sizeof(DxStorage_Info_t) * ulStorageNum);

		for (ulCount=0, ulIdx=0; ulCount<ulStorageNum; ulCount++)
		{
			bAdd = uapi_fs_Filter(&s_pstStorageInfos[ulCount], (const HUINT32)ulFilter);
			if (TRUE == bAdd)
			{
				HxSTD_MemCopy(&astList[ulIdx], &s_pstStorageInfos[ulCount], sizeof(DxStorage_Info_t));
				ulIdx++;
			}
		}

		if (ulIdx == 0)
		{
			UAPI_Free(astList);
			astList = NULL;
		}

		if (pastInfoList)
		{
			*pastInfoList = astList;
		}
		else
		{
			UAPI_Free(astList);
		}
	}

	if (pulDevNum) 	*pulDevNum = ulIdx;

	uapi_fs_releaseInstance(uapi);

	return ERR_OK;
}

STATIC HERROR	uapi_fs_GetInfo(HUINT32 ulDeviceId, DxStorage_Info_t   *pstInfo)
{
	HUINT32		ulIdx;
	HBOOL		bFind = FALSE;
	UAPI_t	*uapi;

	if (pstInfo == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	uapi = uapi_fs_getInstance();

	for(ulIdx = 0 ; ulIdx < s_nStorageNum ; ulIdx++)
	{
		if (s_pstStorageInfos[ulIdx].ulDeviceId == ulDeviceId)
		{
			HxSTD_MemCopy(pstInfo, &s_pstStorageInfos[ulIdx], sizeof(DxStorage_Info_t));
			bFind = TRUE;
			break;
		}
	}
	uapi_fs_releaseInstance(uapi);

	if (bFind)
	{
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL;
	}

}


STATIC HERROR	uapi_fs_GetDeviceIdByStorageId(const HCHAR *szStorageId, HUINT32   *pulDeviceId)
{
	HUINT32		ulIdx;
	HBOOL		bFind = TRUE;
	UAPI_t		*uapi;
	uapi = uapi_fs_getInstance();

	for(ulIdx = 0 ; ulIdx < s_nStorageNum ; ulIdx++)
	{
		if ( HxSTD_StrNCmp(s_pstStorageInfos[ulIdx].szUUID, szStorageId, HxSTD_StrLen(szStorageId)) == 0 )
		{
			if(pulDeviceId) *pulDeviceId = s_pstStorageInfos[ulIdx].ulDeviceId;
			bFind = TRUE;
			break;
		}
	}
	uapi_fs_releaseInstance(uapi);

	if (bFind)		{return ERR_OK;}
	else			{return ERR_FAIL;}
}


STATIC HERROR	uapi_fs_GetMmcSize(HUINT32 ulDeviceId, HUINT64 *pullTotalKbSize, HUINT64 *pullAvailableKbSize, HUINT64 *pullUsedKbSize, HUINT64 *pullReservedKbSize, HUINT64 *pullAvailablePvrKbSize)
{
	HERROR	hErr;
	DxStorage_Info_t	stInfo;

	hErr = uapi_fs_GetInfo(ulDeviceId, &stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("FAIL!! \n");
		return ERR_FAIL;
	}

	if (pullTotalKbSize)			*pullTotalKbSize = 0;
	if (pullAvailableKbSize)		*pullAvailableKbSize = 0;
	if (pullUsedKbSize) 		*pullUsedKbSize = 0;
	if (pullReservedKbSize) 		*pullReservedKbSize = 0;
	if (pullAvailablePvrKbSize) 	*pullAvailablePvrKbSize = 0;

	hErr = HLIB_STORAGE_GetMMCSize(stInfo.ulPortNum, stInfo.szMountPath, pullTotalKbSize, pullUsedKbSize, pullAvailableKbSize);

	return hErr;

}

const HCHAR* uapi_fs_GetFsString(const DxStorage_FsType_e	eFsType)
{
	const HCHAR* ret = s_NullStr;
	switch (eFsType)
	{
		case eDxSTORAGE_FS_EXT2:		ret = "EXT2";		break;
		case eDxSTORAGE_FS_EXT3:		ret = "EXT3";		break;
		case eDxSTORAGE_FS_EXT4:		ret = "EXT4";		break;
		case eDxSTORAGE_FS_XFS:		ret = "XFS";		break;
		case eDxSTORAGE_FS_FAT16:		ret = "FAT16";		break;
		case eDxSTORAGE_FS_FAT32:		ret = "FAT32";		break;
		case eDxSTORAGE_FS_NTFS:		ret = "NTFS";		break;
		case eDxSTORAGE_FS_DOS:		ret = "DOS";		break;
		case eDxSTORAGE_FS_EXFAT:		ret = "exFAT";		break;
		case eDxSTORAGE_FS_OTHER:
		case eDxSTORAGE_FS_NONE:
		default:
			break;
	}
	return ret;
}

const HCHAR* uapi_fs_GetDevTypeString(const DxStorage_DevType_e	eDevType)
{
	const HCHAR* ret = s_NullStr;
	switch (eDevType)
	{
		case eDxSTORAGE_DEVTYPE_INTHDD:		ret = "Internal HDD";		break;
		case eDxSTORAGE_DEVTYPE_DETACHABLE:	ret = "Detachable HDD";		break;
		case eDxSTORAGE_DEVTYPE_ESATAHDD:	ret = "eSATA HDD";		break;
		case eDxSTORAGE_DEVTYPE_USBHDD:		ret = "USB HDD";		break;
		case eDxSTORAGE_DEVTYPE_USBMEM:		ret = "USB Memory";		break;
		case eDxSTORAGE_DEVTYPE_MMC:			ret = "MMC";				break;
		case eDxSTORAGE_DEVTYPE_ALL:
		case eDxSTORAGE_DEVTYPE_NONE:
		default:
			break;
	}
	return ret;
}


HERROR	uapi_fs_GetPvrReservedSize(HUINT32 ulDeviceId, HUINT64 *pullReservedKbSize)
{
	HERROR		hErr = ERR_FAIL;
	DxStorage_Info_t	stInfo;
	HUINT64	ullKbSize = 0;

	HxLOG_Trace();

	/*	 --- PVR Reserved Size 계산 방법 ---
	   - USB Flash : 전체 용량의 1/3을 Reserved Size로 정한다. 단, 이 크기가 4GB를 넘을 경우 4GB로 한정한다.
	   - 64GB 이하의 HDD : USB Flash와 마찬가지로 1/3을 Reserved Size로 정한다.
	   - 그 외 : DxSVC_FS_PVR_RESERVED_SIZE_KB로 정의된 값을 그대로 사용한다.  					*/

	hErr = uapi_fs_GetInfo(ulDeviceId, &stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("FAIL!! \n");
		return ERR_FAIL;
	}

	// PVR 용 Storage에 한하여 계산한다. 일반 Storage는 0
	if (stInfo.ulFlags & eDxSTORAGE_FLAG_PVRDEVICE || stInfo.ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE)
	{
		// Check USB Flash or Total size
		if (stInfo.eDevType == eDxSTORAGE_DEVTYPE_USBMEM || stInfo.ullTotalSizeKb < DxSVC_FS_PVR_MIN_HDD_SIZE_KB)
		{
			// Modify reserved size
			ullKbSize = stInfo.ullTotalSizeKb * DxSVC_FS_PVR_USB_FLASH_RESERVED_SIZE_RATIO;

			// Check threshold
			if (ullKbSize > DxSVC_FS_PVR_USB_FLASH_MAX_RESERVED_SIZE_KB)
			{
				ullKbSize = DxSVC_FS_PVR_USB_FLASH_MAX_RESERVED_SIZE_KB;
			}
			HxLOG_Debug("ullTotalSizeKb: %lld, ullReservedKbSize : %lld\n", stInfo.ullTotalSizeKb, ullKbSize);
		}
		else	// The others
		{
			ullKbSize = DxSVC_FS_PVR_RESERVED_SIZE_KB;
		}
		HxLOG_Debug("ullKbReservedSize: %lld \n",ullKbSize);
	}

	if (pullReservedKbSize) 	*pullReservedKbSize = ullKbSize;

	return ERR_OK;

}

// From mgr_storage_getHddSizeInfo()
HERROR	uapi_fs_GetHddSize(HUINT32 ulDeviceId, HUINT64 *pullTotalKbSize, HUINT64 *pullAvailableKbSize, HUINT64 *pullUsedKbSize, HUINT64 *pullReservedKbSize, HUINT64 *pullAvailablePvrKbSize)
{
	HERROR	hErr;
	DxStorage_Info_t	stInfo;
	HUINT32		ulIdx, ulPvrPartIdx = 0xff;
	HUINT64		ullPhysicalDevSizeKb, ullUsedSizeKb, ullTsrSizeKb;
	HUINT64		ullPvrPartitionTotalKb;
	HUINT64		ullTotalKbSize_Out, ullAvailableKbSize_Out, ullUsedKbSize_Out, ullReservedKbSize_Out, ullAvailablePvrKbSize_Out;
	HUINT64		ullTotalReservedKbSize = 0;

	/*
	[HDD size]
		Internal / External 공용으로 아래와 같이 정의함
		1.	Total Size (ullTotalSizeKb) = 디스크 드라이브의 물리적 사이즈. 모든 파티션 포함해서의 전체 사이즈.
		2.	Available Size (ullAvailableSizeKb) = 디스크 중 PVR / media로 쓸 수 있는 영역 (Internal HDD라면 Partition 2, External HDD나 USB면 Partition 0)의 "남은 용량"
		3.	User Usage (ullUsedSizeKb)= 디스크 중 PVR / media로 쓸 수 있는 영역에서 PVR / media contents Size, 혹은 그 Partition 영역에서 발견된 Used Size (TSR 사용량 포함)
		4.	System Usage (ullReservedSizeKb) = Total Size - Available Size - User Usage (Internal의 경우 여기에 다른 용도의 Partition size까지 모두 포함됨)
	*/
	ullPvrPartitionTotalKb = ullPhysicalDevSizeKb = ullUsedSizeKb = ullTsrSizeKb = 0LL;
	ullTotalKbSize_Out = ullAvailableKbSize_Out = ullUsedKbSize_Out = ullReservedKbSize_Out = ullAvailablePvrKbSize_Out = 0LL;

	hErr = uapi_fs_GetInfo(ulDeviceId, &stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("FAIL!! \n");
		return ERR_FAIL;
	}

	// Find PVR partition
	for (ulIdx= 0 ; ulIdx < stInfo.nPartitionNum ; ulIdx++)
	{
		if (stInfo.astPartitionInfos[ulIdx].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE)
		{
			ulPvrPartIdx = ulIdx;
		}
	}


	for(ulIdx=0; ulIdx< s_nStorageNum;ulIdx++)
	{
		if(ulDeviceId ==s_pstStorageInfos[ulIdx].ulDeviceId)
		{
			ullPhysicalDevSizeKb = s_pstStorageInfos[ulIdx].ullTotalSizeKb;
		}
	}
	if(ullPhysicalDevSizeKb == 0)
	{
		HxLOG_Error("Can't find device id [%d]..\n",ulDeviceId);
		return ERR_FAIL;
	}

	HxLOG_Debug("[%s] [%d]ullPhysicalDevSizeKb : %lld, ullPvrPartitionTotalKb : %lld, ullUsedSizeKb : %lld \n",__FUNCTION__,__LINE__, ullPhysicalDevSizeKb, ullPvrPartitionTotalKb, ullUsedSizeKb);

	if (FALSE == (stInfo.ulFlags & eDxSTORAGE_FLAG_MOUNTED))
	{
		HxLOG_Warning("Not mounted device!! \n");

		ullTotalKbSize_Out = ullPhysicalDevSizeKb;
		ullAvailableKbSize_Out = ullPhysicalDevSizeKb;

		goto END_FUNC;
	}

	hErr = uapi_fs_GetPvrReservedSize(ulDeviceId, &ullTotalReservedKbSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Cannot get PVR Reserved size!!(ulDeviceId : %d) \n", ulDeviceId);
		return ERR_FAIL;
	}

	if (ulPvrPartIdx != 0xff)
	{
		HxLOG_Debug("[%s] [%d]MountPath : %s\n",__FUNCTION__,__LINE__,stInfo.astPartitionInfos[ulPvrPartIdx].szMountPath);
		HLIB_STORAGE_GetPartitionSize (stInfo.astPartitionInfos[ulPvrPartIdx].szMountPath, &ullPvrPartitionTotalKb, &ullUsedSizeKb, NULL);
		if ((hErr != ERR_OK) || (0 == ullPvrPartitionTotalKb) || (ullPvrPartitionTotalKb < ullUsedSizeKb))
		{
			HxLOG_Error("HLIB_STORAGE_GetPartitionSize error\n");
			goto END_FUNC;
		}
		HxLOG_Debug("[%s] [%d] ullPvrPartitionTotalKb : %lld, ullUsedSizeKb : %lld\n",__FUNCTION__,__LINE__, ullPvrPartitionTotalKb, ullUsedSizeKb);

		// TSR을 수행하는 Partition인지 확인
		if (stInfo.astPartitionInfos[ulPvrPartIdx].ulFlags & eDxSTORAGE_FLAG_DEFAULTHDD)
		{
			// 실제 tsr 파일이 없더라도 tsr size가 뭔가 잡히도록 1K를 더해준다. (ullAvailablePvrSizeKb 계산 위해서)
			hErr = HLIB_STORAGE_GetPathSize (stInfo.astPartitionInfos[ulPvrPartIdx].szTsrPath, &ullTsrSizeKb);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("HLIB_STORAGE_GetPathSize error\n");
			}
			else
			{
				ullTsrSizeKb += SIZE_1K;
				ullTsrSizeKb /= SIZE_1K;
				HxLOG_Debug("[%s] [%d] TsrPath : %s, ullTsrSizeKb : %lld \n",__FUNCTION__,__LINE__, stInfo.astPartitionInfos[ulPvrPartIdx].szTsrPath, ullTsrSizeKb);
			}
		}
	}
	else		// 없는 경우
	{
		//어떻게 되어있는지 알수 없으니 Partition을 순회하여 size 얻기
		for (ulIdx = 0; ulIdx < stInfo.nPartitionNum; ulIdx++)
		{
			HUINT64 	ullStorageKb = 0, ullUsedKb = 0;

			HxLOG_Debug("[%s] [%d]MountPath : %s\n",__FUNCTION__,__LINE__,stInfo.astPartitionInfos[ulIdx].szMountPath);
			HLIB_STORAGE_GetPartitionSize (stInfo.astPartitionInfos[ulIdx].szMountPath, &ullStorageKb, &ullUsedKb, NULL);
			if (hErr == ERR_OK)
			{
				ullPvrPartitionTotalKb += ullStorageKb;
				ullUsedSizeKb += ullUsedKb;
			}
			else
			{
				HxLOG_Error("HLIB_STORAGE_GetPartitionSize error\n");
			}
			HxLOG_Debug("[%s] [%d]ullPvrPartitionTotalKb : %lld, ullUsedSizeKb : %lld\n",__FUNCTION__,__LINE__, ullPvrPartitionTotalKb, ullUsedSizeKb);
		}
	}

	// Size 넣기
	if ( (ERR_OK == hErr) && (ullPhysicalDevSizeKb != 0LL) && (ullPhysicalDevSizeKb > ullUsedSizeKb))
	{
		ullTotalKbSize_Out		= ullPhysicalDevSizeKb;
		ullAvailableKbSize_Out	= ullPvrPartitionTotalKb - ullUsedSizeKb;
		ullUsedKbSize_Out		= ullUsedSizeKb;
		ullReservedKbSize_Out	= ullTotalKbSize_Out - ullAvailableKbSize_Out - ullUsedKbSize_Out;

		// 실제 available 영역에 TSR 버퍼 사이즈만큼 보정 해준다 (used에 포함되었던 실제 TSR 사이즈는 빼고, TSR 버퍼 사이즈를 뺌)
		ullAvailablePvrKbSize_Out = ullAvailableKbSize_Out;

		if (ullTsrSizeKb > 0)
		{
			ullAvailablePvrKbSize_Out +=  ullTsrSizeKb;
			if (ullAvailablePvrKbSize_Out > ullTotalReservedKbSize)
			{
				ullAvailablePvrKbSize_Out -= ullTotalReservedKbSize;
			}
			else
			{
				ullAvailablePvrKbSize_Out = 0;
			}
		}
	}
	else
	{
		//뭔가 문제가 생겼다..
		HxLOG_Error("Error ullPhysicalDevSizeKb : %lld, ullPvrPartitionTotalKb : %lld, ullUsedSizeKb : %lld\n", ullPhysicalDevSizeKb, ullPvrPartitionTotalKb, ullUsedSizeKb);
		return ERR_FAIL;
	}

END_FUNC:

	if (pullTotalKbSize)				*pullTotalKbSize = ullTotalKbSize_Out;
	if (pullAvailableKbSize)			*pullAvailableKbSize = ullAvailableKbSize_Out;
	if (pullUsedKbSize)				*pullUsedKbSize = ullUsedKbSize_Out;
	if (pullReservedKbSize)			*pullReservedKbSize = ullReservedKbSize_Out;
	if (pullAvailablePvrKbSize)		*pullAvailablePvrKbSize = ullAvailablePvrKbSize_Out;

	HxLOG_Debug("[%s:%04d] [Total]:[%lld], [Available]:[%lld], [Used]:[%lld], [Reserved]:[%lld], [AvailableForPVR]:[%lld]\n", __FUNCTION__, __LINE__,
		ullTotalKbSize_Out, ullAvailableKbSize_Out, ullUsedKbSize_Out, ullReservedKbSize_Out, ullAvailablePvrKbSize_Out);


	return ERR_OK;
}

STATIC void uapi_fs_printDeviceInfo(DxStorage_Info_t *pstInfo)
{
	HINT32	i;

	HxLOG_Print("================ Storage Info ================ \n");
	HxLOG_Print("ulDeviceId : [%d]\n", pstInfo->ulDeviceId);
	HxLOG_Print("DevType : [%s]\n", uapi_fs_GetDevTypeString(pstInfo->eDevType));
	HxLOG_Print("szMountPath : [%s]\n", pstInfo->szMountPath);
	HxLOG_Print("szDevName : [%s] \n", pstInfo->szDevName);
	HxLOG_Print("szLabel : [%s]\n", pstInfo->szLabel);
	HxLOG_Print("szUUID : [%s]\n", pstInfo->szUUID);
	HxLOG_Print("ulFlags : [0x%x]\n", pstInfo->ulFlags);
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_CONNECTED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_MOUNTED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_SECURED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_INTERNAL));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_NEEDFORMAT));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_PAIRED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_NEW2NDHDD));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_TSRDEVICE));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE));

	HxLOG_Print("\n--------Partition Info--------\n");
	for (i=0; i < pstInfo->nPartitionNum ; i++)
	{
		HxLOG_Print("\n   [%d] \n",i);
		HxLOG_Print("   bAvailable : [%d]\n", pstInfo->astPartitionInfos[i].bAvailable);
		HxLOG_Print("   FsType: [%s]\n", uapi_fs_GetFsString(pstInfo->astPartitionInfos[i].eFsType));
		HxLOG_Print("   nPvrIdx : [%d]\n", pstInfo->astPartitionInfos[i].nPvrIdx);
		HxLOG_Print("   nPlaybackIdx : [%d]\n", pstInfo->astPartitionInfos[i].nPlaybackIdx);
		HxLOG_Print("   szMountPath : [%s]\n", pstInfo->astPartitionInfos[i].szMountPath);
		HxLOG_Print("   szTsrPath : [%s]\n", pstInfo->astPartitionInfos[i].szTsrPath);
		HxLOG_Print("   szVolumeName : [%s]\n", pstInfo->astPartitionInfos[i].szVolumeName);
		HxLOG_Print("   szRecordPath : [%s]\n", pstInfo->astPartitionInfos[i].szRecordPath);
		HxLOG_Print("   szMediaPath : [%s]\n", pstInfo->astPartitionInfos[i].szMediaPath);
		HxLOG_Print("   szMediaVideo : [%s]\n", pstInfo->astPartitionInfos[i].szMediaVideo);
		HxLOG_Print("   szMediaAudio : [%s]\n", pstInfo->astPartitionInfos[i].szMediaAudio);
		HxLOG_Print("   szMediaPic : [%s]\n", pstInfo->astPartitionInfos[i].szMediaPic);
		HxLOG_Print("   szMediaDownload : [%s]\n", pstInfo->astPartitionInfos[i].szMediaDownload);
		HxLOG_Print("   ulStartUnit : [%d], ulEndUnit : [%d]\n", pstInfo->astPartitionInfos[i].ulStartUnit, pstInfo->astPartitionInfos[i].ulEndUnit);
		HxLOG_Print("   ulFlags : [0x%x]\n", pstInfo->astPartitionInfos[i].ulFlags);
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_MOUNTED));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SECURED));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE));
	}
	HxLOG_Print("===========================================\n");
}


#define ___GLOBAL_FUNCTIONS___
HERROR	UAPI_FS_Init(void)
{
	UAPI_t *uapi;

	HxLOG_Trace();
	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FS_Init, NULL);
		uapi_fs_releaseInstance(uapi);
	}
	return ERR_OK;
}
HERROR	UAPI_FS_SetNotifier(DxStorageNotifier_t pfnApkListener)
{
	HxLOG_Trace();
	s_pfnUpaiFsListener = pfnApkListener;

	return ERR_OK;
}

// ulFilter : DxStorage_LIST_Filter_e
HERROR	UAPI_FS_GetList(HUINT32 ulFilter, DxStorage_Info_t **pastInfoList, HUINT32 *pulDevNum)
{
	HxLOG_Trace();
	return uapi_fs_GetList(ulFilter, pastInfoList, pulDevNum);
}

HERROR UAPI_FS_GetInfo(HUINT32 ulDeviceId, DxStorage_Info_t   *pstInfo)
{
	HxLOG_Trace();
	return uapi_fs_GetInfo(ulDeviceId, pstInfo);
}


HERROR	UAPI_FS_FreeList(DxStorage_Info_t *pastInfoList, HUINT32 ulDevNum)
{
	HxLOG_Trace();
	if (pastInfoList == NULL)
	{
		HxLOG_Debug("pastInfoList is NULL\n");
		return ERR_OK;
	}

	UAPI_Free(pastInfoList);
	return ERR_OK;
}


HERROR	UAPI_FS_Format(HUINT32 ulDeviceId, const HCHAR *pszLabel)
{
	HxPair_t	*pstPair = NULL;
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	HxLOG_Trace();
	pstPair = (HxPair_t *)UAPI_Malloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)eDxSTORAGE_RESULT_Format;
		pstPair->right = (void *)ulDeviceId;
	}

	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FS_Format, "is", ulDeviceId, pszLabel);
//		if (onMoveStatusChange)
			uapi_fs_event_AddEventListener(uapi, nReqId, (void *)pstPair, uapi_fs_onResultNotifier);

		uapi_fs_releaseInstance(uapi);
	}

	return nReqId;
}

HERROR	UAPI_FS_Dst(HUINT32 ulDeviceId)
{
	HxPair_t	*pstPair = NULL;
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	HxLOG_Trace();
	pstPair = (HxPair_t *)UAPI_Malloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)eDxSTORAGE_RESULT_Dst;
		pstPair->right = (void *)ulDeviceId;
	}

	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FS_Dst, "i", ulDeviceId);
//		if (onMoveStatusChange)
			uapi_fs_event_AddEventListener(uapi, nReqId, (void *)pstPair, uapi_fs_onResultNotifier);

		uapi_fs_releaseInstance(uapi);
	}

	return nReqId;
}

HERROR	UAPI_FS_SetDefaultHdd(const HCHAR *pszUUID)
{
	HUINT32	ulDeviceId = 0;
	HERROR 	hErr = ERR_FAIL;
	HxPair_t	*pstPair = NULL;

	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	HxLOG_Trace();
	hErr = uapi_fs_GetDeviceIdByStorageId(pszUUID, &ulDeviceId);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Unexpected Error!!\n");
	}

	pstPair = (HxPair_t *)UAPI_Malloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)eDxSTORAGE_RESULT_SetDefaultHdd;
		pstPair->right = (void *)ulDeviceId;
	}

	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FS_SetDefaultHdd, "s", pszUUID);
//		if (onMoveStatusChange)
			uapi_fs_event_AddEventListener(uapi, nReqId, (void *)pstPair, uapi_fs_onResultNotifier);

		uapi_fs_releaseInstance(uapi);
	}

	return nReqId;
}

HERROR	UAPI_FS_CheckUsbSpeed(const HCHAR *pszUUID)
{
	HUINT32 ulDeviceId = 0;
	HERROR	hErr = ERR_FAIL;
	HxPair_t	*pstPair = NULL;

	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	HxLOG_Trace();
	hErr = uapi_fs_GetDeviceIdByStorageId(pszUUID, &ulDeviceId);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Unexpected Error!!\n");
	}

	pstPair = (HxPair_t *)UAPI_Malloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)eDxSTORAGE_RESULT_CheckUsbSpeed;
		pstPair->right = (void *)ulDeviceId;
	}

	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FS_CheckUsbSpeed, "s", pszUUID);
//		if (onMoveStatusChange)
			uapi_fs_event_AddEventListener(uapi, nReqId, (void *)pstPair, uapi_fs_onResultNotifier);

		uapi_fs_releaseInstance(uapi);
	}

	return nReqId;
}


HERROR	UAPI_FS_SetName(const HCHAR *pszUUID, const HCHAR *pszStorageName)
{
	HUINT32	ulDeviceId;
	HERROR 	hErr;
	HxPair_t	*pstPair = NULL;
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	HxLOG_Trace();
	hErr = uapi_fs_GetDeviceIdByStorageId(pszUUID, &ulDeviceId);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Unexpected Error!!\n");
	}

	pstPair = (HxPair_t *)UAPI_Malloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)eDxSTORAGE_RESULT_SetName;
		pstPair->right = (void *)ulDeviceId;
	}

	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FS_SetName, "ss", pszUUID, pszStorageName);
//		if (onMoveStatusChange)
			uapi_fs_event_AddEventListener(uapi, nReqId, (void *)pstPair, uapi_fs_onResultNotifier);
		uapi_fs_releaseInstance(uapi);
	}

	return nReqId;
}

HERROR	UAPI_FS_DisableHdd(const HCHAR *pszUUID)
{
	HUINT32	ulDeviceId = 0;
	HERROR 	hErr = ERR_FAIL;
	HxPair_t	*pstPair = NULL;
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	HxLOG_Trace();
	hErr = uapi_fs_GetDeviceIdByStorageId(pszUUID, &ulDeviceId);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Unexpected Error!!\n");
	}

	pstPair = (HxPair_t *)UAPI_Malloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)eDxSTORAGE_RESULT_DisableHdd;
		pstPair->right = (void *)ulDeviceId;
	}

	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FS_DisableHdd, "s", pszUUID);
//		if (onMoveStatusChange)
			uapi_fs_event_AddEventListener(uapi, nReqId, (void *)pstPair, uapi_fs_onResultNotifier);
		uapi_fs_releaseInstance(uapi);
	}


	return nReqId;
}

HERROR	UAPI_FS_Pairing(const HCHAR *pszUUID)
{
	HUINT32	ulDeviceId = 0;
	HERROR 	hErr = ERR_FAIL;
	HxPair_t	*pstPair = NULL;
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	HxLOG_Trace();
	hErr = uapi_fs_GetDeviceIdByStorageId(pszUUID, &ulDeviceId);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Unexpected Error!!\n");
	}

	pstPair = (HxPair_t *)UAPI_Malloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)eDxSTORAGE_RESULT_Pairing;
		pstPair->right = (void *)ulDeviceId;
	}

	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FS_Pairing, "s", pszUUID);
//		if (onMoveStatusChange)
			uapi_fs_event_AddEventListener(uapi, nReqId, (void *)pstPair, uapi_fs_onResultNotifier);

		uapi_fs_releaseInstance(uapi);
	}

	return nReqId;
}

HERROR	UAPI_FS_UnPairing(const HCHAR *pszUUID)
{
	HUINT32	ulDeviceId = 0;
	HERROR 	hErr = ERR_FAIL;
	HxPair_t	*pstPair = NULL;
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	HxLOG_Trace();
	hErr = uapi_fs_GetDeviceIdByStorageId(pszUUID, &ulDeviceId);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Unexpected Error!!\n");
	}

	pstPair = (HxPair_t *)UAPI_Malloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)eDxSTORAGE_RESULT_UnPairing;
		pstPair->right = (void *)ulDeviceId;
	}
	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FS_UnPairing, "s", pszUUID);
//		if (onMoveStatusChange)
			uapi_fs_event_AddEventListener(uapi, nReqId, (void *)pstPair, uapi_fs_onResultNotifier);

		uapi_fs_releaseInstance(uapi);
	}

	return nReqId;
}

HERROR	UAPI_FS_SafeRemove(HUINT32 ulDeviceId, HBOOL bForced)
{
	HxPair_t	*pstPair = NULL;
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	HxLOG_Trace();

	pstPair = (HxPair_t *)UAPI_Malloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)eDxSTORAGE_RESULT_SafeRemove;
		pstPair->right = (void *)ulDeviceId;
	}

	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FS_SafeRemove, "ii", ulDeviceId, bForced ? 1 : 0);
//		if (onMoveStatusChange)
			uapi_fs_event_AddEventListener(uapi, nReqId, (void *)pstPair, uapi_fs_onResultNotifier);

		uapi_fs_releaseInstance(uapi);
	}

	return nReqId;
}

// TSR 등을 위해 잡아둔 Reserved Size를 리턴한다.
HERROR	UAPI_FS_GetPvrReservedSize(HUINT32 ulDeviceId, HUINT64 *pullReservedKbSize)
{
	return	uapi_fs_GetPvrReservedSize(ulDeviceId, pullReservedKbSize);
}

//Note: Size 정보는 get하는 시점의 최신 정보를 sync로 주는 것이 가장 이상적이므로 appkit에서 구현함.
HERROR	UAPI_FS_GetSize(HUINT32 ulDeviceId, HUINT64 *pullTotalKbSize, HUINT64 *pullAvailableKbSize, HUINT64 *pullUsedKbSize, HUINT64 *pullReservedKbSize, HUINT64 *pullAvailablePvrKbSize)
{
	HERROR		hErr = ERR_FAIL;
	DxStorage_Info_t	stInfo;

	HxLOG_Trace();

	hErr = uapi_fs_GetInfo(ulDeviceId, &stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("FAIL!! \n");
		return ERR_FAIL;
	}
	if ((stInfo.ulFlags & eDxSTORAGE_FLAG_CONNECTED) == FALSE)
	{
		HxLOG_Error("DISCONNECTED!! \n");
		return ERR_FAIL;
	}

	switch (stInfo.eDevType)
	{
		case	eDxSTORAGE_DEVTYPE_INTHDD:
		case	eDxSTORAGE_DEVTYPE_DETACHABLE:
		case	eDxSTORAGE_DEVTYPE_ESATAHDD:
		case	eDxSTORAGE_DEVTYPE_USBHDD:
		case	eDxSTORAGE_DEVTYPE_USBMEM:
			hErr = uapi_fs_GetHddSize(ulDeviceId, pullTotalKbSize, pullAvailableKbSize, pullUsedKbSize, pullReservedKbSize, pullAvailablePvrKbSize);
			break;

		case eDxSTORAGE_DEVTYPE_MMC:
			hErr = uapi_fs_GetMmcSize(ulDeviceId, pullTotalKbSize, pullAvailableKbSize, pullUsedKbSize, pullReservedKbSize, pullAvailablePvrKbSize);
			break;

		default:
			break;
	}

	return hErr;

}

const HCHAR* UAPI_FS_GetFsString(const DxStorage_FsType_e	eFsType)
{
	return uapi_fs_GetFsString(eFsType);
}

const HCHAR* UAPI_FS_GetDevTypeString(const DxStorage_DevType_e	eDevType)
{
	return uapi_fs_GetDevTypeString(eDevType);
}


HINT32 UAPI_FS_GetHddSerialNumber(const HCHAR *dev, HCHAR **hddsn)
{
#if 0
#elif defined(CONFIG_PROD_OS_EMULATOR)
#elif defined(CONFIG_PROD_CPU_BCM)
	/**
	  *	BRCM Chip (BXR-HD+)
	  *
	  *	refer DRV_SMART_IdentifyDevice()
	**/

	HINT32 i;
	HINT32 smartfd = -1;
	HUINT16 *pusbuff;
	static HUINT8 smartbuf[SMART_BUF_SIZE];
	HUINT64 disksize;
	if (hddsn == NULL)
	{
		HxLOG_Error("invalid parameter !\n");
		return ERR_FAIL;
	}

	smartfd=open(dev, O_RDONLY | O_NONBLOCK);
	if(smartfd < 0)
	{
		HxLOG_Error("open error!\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pszHddSn, 0x00, ATA_SERIAL_NUM_LEN);
	HxSTD_memset(smartbuf, 0x00, SMART_BUF_SIZE);
	smartbuf[0] = 0xec;
	smartbuf[3] = 1;

	if(ioctl(smartfd,SMART_CMD,&smartbuf)!= 0)
	{
		HxLOG_Error("[%d]%s ioctl error!!! \n",__LINE__,__func__);
		close(smartfd);
		return ERR_FAIL;
	}

	/* Get Block Size */
	disksize = 0;
	if (ioctl(smartfd, SMART_BLKGETSIZE64, &disksize) != 0)
	{
		HxLOG_Error("[%d]%s ioctl error!!! \n",__LINE__,__func__);
		close(smartfd);
		return ERR_FAIL;
	}

	/* swap data */
	pusbuff = (unsigned short *)smartbuf;
	for(i=0; i<(SMART_BUF_SIZE-4)/2; i++)
	{
		pusbuff[i] = ntohs(pusbuff[i+2]);
	}

	HxSTD_memcpy(pszHddSn, &smartbuf[SERIAL_NUM_INDEX], ATA_SERIAL_NUM_LEN-1);
	*hddsn = uapi_fs_strip(pszHddSn);

	close(smartfd);

#else

	/**
	  *	ST Chip (VHDR-3000S)
	**/
	HINT32 fd;
	HUINT16 id[256];

	if (fd = open(dev, O_RDONLY), -1 == fd)
	{
		HxLOG_Error("inactive device!\n");
		return ERR_FAIL;
	}

	if(ioctl(fd, HDIO_GET_IDENTITY, s_dev_param))
	{
		HxLOG_Error("ioctl() error\n");
		close(fd);
		return ERR_FAIL;
	}

	*hddsn = uapi_fs_strip(strndup((HCHAR *)&s_dev_param[10], 20));

	close(fd);

#endif

	return ERR_OK;
}


HBOOL UAPI_FS_CheckStoragePowerOff(void)
{
	UAPI_t	*uapi = NULL;
	HBOOL	bOff = TRUE;

	uapi = uapi_fs_getInstance();
	if (uapi)
	{
		bOff = s_bStoragePowerOff;
	}
	uapi_fs_releaseInstance(uapi);

	return bOff;
}

HBOOL UAPI_FS_UseTsrOnlyDevice(void)
{
	return s_bUseTsrOnlyDevice;
}

HERROR UAPI_FS_GetAvailablePvrPath (HCHAR *pszPath)
{
	HERROR		hErr = ERR_FAIL;
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 i, j;
	uapi_fs_GetList(eDxSTORAGE_FILTER_PVR, &pstInfoList, &ulDevNum);
	HxLOG_Debug("[%s] [%d] Storage Count : [%d]\n",__FUNCTION__,__LINE__, ulDevNum);
	for (i = 0 ; i < ulDevNum ; i++)
	{
		pstInfo = &pstInfoList[i];
		for(j = 0 ; j < pstInfo->nPartitionNum ; j++)
		{
			if ((pstInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) || (pstInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_DEFAULTHDD))
			{
				HLIB_STD_StrNCpySafe(pszPath, pstInfo->astPartitionInfos[j].szMountPath, DxSTORAGE_NAME_LEN);
				HxLOG_Debug("[%s] [%d] Available PVR Path : [%s]\n",__FUNCTION__,__LINE__, pszPath);
				hErr =  ERR_OK;
			}
		}
	}
	if (pstInfoList != NULL)
	{
		UAPI_Free(pstInfoList);
	}
	return hErr;
}
