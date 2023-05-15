/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  se_uapi.c
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
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#include	<fcntl.h>
#include	<sys/mount.h>

#include	<hlib.h>
#include 	<dlib.h>
#include	<vkernel.h>

#include	<se_uapi.h>
#include	<se_umma_int.h>
#include	<ummalib.h>
#include 	<di_fs.h>
#include	<di_usb.h>
#include	<di_hdd.h>
#include	"drv_hotplug.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_SEUAPI_CACHE		HxSEMT_Get(s_ulSeUapiSemId)
#define	LEAVE_SEUAPI_CACHE		HxSEMT_Release(s_ulSeUapiSemId)


#define SEUMMA_RPC_JOIN_TIMEOUT	60000

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC SEUAPI_t	*s_pstUapi = NULL;

STATIC DI_FS_Event_Callback_Func	s_pfFsEventCb = NULL;

#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
STATIC pfnDI_HDD_NotifyFUNC 	s_pfHddNotiCb = NULL;
STATIC pfnDI_HDD_DSTNotify		s_pfDstNotiCb = NULL;
#endif

#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
STATIC pfnDI_USB_NotifyFUNC	s_pfUsbNotiCb = NULL;
#endif

STATIC pfHotplug_NotifyFUNC		s_pfHotplugNotiCb = NULL;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

STATIC void		seuapi_event_listener(void *arg);
STATIC SEUAPI_t*	seuapi_getInstance(void);
STATIC void		seuapi_releaseInstance(SEUAPI_t *uapi);
STATIC void seuapi_CmdInit(void);



#define _____NOTIFICATION_CALLBACKS_____
//	apArgv[0] : nDeviceId (i)
//	apArgv[1] : nConnected (i)
//	apArgv[2] : DxStorage_Info_t * pstItem (b)
//STATIC void	seuapi_onDeviceConnected(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], void *userdata)

#define _______________

STATIC SEUAPI_t*	seuapi_getInstanceWO(void)
{
	return s_pstUapi;
}

STATIC SEUAPI_t*	seuapi_getInstance(void)
{
	if (s_pstUapi == NULL)
	{
		unsigned int	tid;

		s_pstUapi = (SEUAPI_t*)HLIB_MEM_Calloc(sizeof(SEUAPI_t));

		s_pstUapi->nClientHandle    = HLIB_SOCKET_OpenClient(HxSOCKET_UDS, (void *)SEUMMA_SIGNAL_UDS, 0);
		s_pstUapi->nRpcHandle       = HLIB_RPC_Open(s_pstUapi->nClientHandle, HLIB_STD_GetPID());

		VK_TASK_Create(seuapi_event_listener, 70, SIZE_64K, "seuapi_event_listener", (void*)s_pstUapi, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);

	}
	return s_pstUapi;
}

STATIC void		seuapi_releaseInstance(SEUAPI_t *uapi)
{
//	HxSEMT_Release(uapi->ulSemId);
}

STATIC void		seuapi_event_onPacketReceived(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	SEUAPI_t	*uapi;

	uapi = seuapi_getInstanceWO();

	HLIB_RPC_ProcessRPC(uapi->nRpcHandle, size, pvData, requestId, nSocketHandle);

//	seuapi_releaseInstance(uapi);
}

STATIC void		seuapi_event_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	(void)userData;
	(void)eStatus;
	(void)nClientHandle;
}

STATIC void		seuapi_event_listener(void *arg)
{
	SEUAPI_t	*uapi = (SEUAPI_t*)arg;

	HLIB_SOCKET_Listen(uapi->nClientHandle, TRUE, seuapi_event_onPacketReceived, seuapi_event_onStatusChanged, arg);
}


STATIC HERROR	seuapi_ConvertFormatListToSeumma(DI_FS_FormatList_t *pstIn, SEUMMA_FormatList_t *pstOut)
{
	HUINT32	ulIdx;

	if (pstIn == NULL || pstOut == NULL)
	{
		return ERR_FAIL;
	}

	pstOut->nMaxFormatPartition = pstIn->nMaxFormatPartition;
	pstOut->nNumCurrPartition= pstIn->nNumCurrPartition;
	pstOut->nNumFormatPartition = pstIn->nNumFormatPartition;

	for (ulIdx = 0; ulIdx < pstIn->nNumFormatPartition ; ulIdx++)
	{
		HxSTD_memcpy(&pstOut->stPartParam[ulIdx], &pstIn->pstPartParam[ulIdx], sizeof(DI_FS_FormatItem_t));
	}

	return ERR_OK;
}


#define  _____CB_____________

// 0 : hErr
// 1 : DI_FS_PartitionList_t
STATIC HERROR seuapi_GetPartitionFromDeviceMbr_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_PartitionList_t *pstPartList, *pstDest;
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	HxPair_t	*pstPair = (HxPair_t *)pvUserData;

	HxLOG_Trace();

	pstPartList = (DI_FS_PartitionList_t *)HxOBJECT_BIN_DATA(apArgv[1]);
	pstDest =  (DI_FS_PartitionList_t *)HLIB_MEM_Malloc(sizeof(DI_FS_PartitionList_t));

	if (pstDest == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstDest, pstPartList, sizeof(DI_FS_PartitionList_t));

	pstPair->left = (void *)pstDest;

	if (hErr != ERR_OK)
	{
		HxLOG_Trace();
		pstPair->right = (void *)ERR_FAIL;
		return ERR_FAIL;
	}
	pstPair->right = (void *)ERR_OK;

	return ERR_OK;
}


// apArgv[0] : SE_UMMA_RPC_Item_t  (ulP1 : hErr, ulP2 : ullTotalKbSize, ulP3 : ullUsedKbSize, ulP4 : bWritable)
STATIC HERROR seuapi_GetMountedPartitionStatus_Cb(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	SE_UMMA_RPC_Item_t 	*pstInfo = (SE_UMMA_RPC_Item_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstInfo == NULL || pstItem == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstItem, pstInfo, sizeof(SE_UMMA_RPC_Item_t));

	return ERR_OK;
}

// apArgv[0] : SE_UMMA_RPC_Item_t  (ulP1 : hErr, ulP2 : ullTotalKbSize)
STATIC HERROR seuapi_GetDevSize_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	SE_UMMA_RPC_Item_t 	*pstInfo = (SE_UMMA_RPC_Item_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstInfo == NULL || pstItem == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstItem, pstInfo, sizeof(SE_UMMA_RPC_Item_t));

	return ERR_OK;
}

// apArgv[0] : SE_UMMA_RPC_Item_t  (ulP1 : hErr, ulP2 : ullKbSize)
STATIC HERROR seuapi_GetPathSize_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	SE_UMMA_RPC_Item_t 	*pstInfo = (SE_UMMA_RPC_Item_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstInfo == NULL || pstItem == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstItem, pstInfo, sizeof(SE_UMMA_RPC_Item_t));

	return ERR_OK;
}

// 0 : hErr
// 1 : uuidsize
// 2 : szUUID
STATIC HERROR seuapi_GetVolumeUuid_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	HUINT32	ulSize = HxOBJECT_INT(apArgv[1]);
	HCHAR *szUUID =  HxOBJECT_STR(apArgv[2]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (szUUID == NULL || pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;
	pstItem->ulP2 = ulSize;
	pstItem->pszStr = (HCHAR *)HLIB_STD_StrDup(szUUID);

	return ERR_OK;
}

// 0 : hErr
// 1 : szUUID
STATIC HERROR seuapi_GetVolumeName_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	HCHAR *szUUID =  HxOBJECT_STR(apArgv[1]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();

	if (szUUID == NULL || pstItem == NULL)
	{
		return ERR_FAIL;
	}

	pstItem->ulP1 = hErr;
	pstItem->pszStr = (HCHAR *)HLIB_STD_StrDup(szUUID);

	return ERR_OK;
}

// 0 : hErr
// 1 : szKey
STATIC HERROR seuapi_GetEncryptionKey_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	HCHAR *szKey =  HxOBJECT_STR(apArgv[1]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (szKey == NULL || pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;
	pstItem->pszStr = (HCHAR *)HLIB_STD_StrDup(szKey);

	return ERR_OK;
}

// 0 : hErr
// 1 : eFsType
STATIC HERROR seuapi_GetFsType_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	HUINT32 ulFsType =  HxOBJECT_INT(apArgv[1]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}

	pstItem->ulP1 = hErr;
	pstItem->ulP2 = ulFsType;

	return ERR_OK;
}


// apArgv[0] : SE_UMMA_RPC_Item_t  (ulP1 : hErr, ulP2 : bRemovable)
STATIC HERROR seuapi_UsbIsDeviceRemovable_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	SE_UMMA_RPC_Item_t	*pstInfo = (SE_UMMA_RPC_Item_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstInfo == NULL || pstItem == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstItem, pstInfo, sizeof(SE_UMMA_RPC_Item_t));

	return ERR_OK;
}


// 0: HUINT32 ulEvent
// 1: HUINT32 ulParam1
// 2: HUINT32 ulParam2
// 3: HUINT32 ulParam3
static HERROR	seuapi_onFormatProgress(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 ulEvent, ulParam1, ulParam2, ulParam3;

	if (nArgc != 4)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}
	HxLOG_Trace();

	ulEvent = HxOBJECT_INT(apArgv[0]);
	ulParam1 = HxOBJECT_INT(apArgv[1]);
	ulParam2 = HxOBJECT_INT(apArgv[2]);
	ulParam3 = HxOBJECT_INT(apArgv[3]);

	if (s_pfFsEventCb)
	{
		s_pfFsEventCb (ulEvent, ulParam1, ulParam2, ulParam3);
	}

	return ERR_OK;
}

STATIC HERROR seuapi_MountPartition_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}

STATIC HERROR seuapi_UmountPartition_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}

STATIC HERROR seuapi_MountPartition_Smbc_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}

STATIC HERROR seuapi_UmountPartition_Smbc_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}

STATIC HERROR seuapi_MountPartition_Ftps_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}

STATIC HERROR seuapi_MountPartition_Smbs_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}


STATIC HERROR seuapi_DoCommand_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}


STATIC HERROR seuapi_Start_Ftps_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}

STATIC HERROR seuapi_PopenCmd_Smbc_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}


STATIC HERROR seuapi_Start_Smbs_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}

STATIC HERROR seuapi_SetPassword_Smbs_Cb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = HxOBJECT_INT(apArgv[0]);
	SE_UMMA_RPC_Item_t	*pstItem = (SE_UMMA_RPC_Item_t *)pvUserData;

	HxLOG_Trace();
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}
	pstItem->ulP1 = hErr;

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___

#define ___FS___

HERROR	SEUAPI_Init(void)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	HxLOG_Trace();
	uapi = seuapi_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_Init, NULL);
		seuapi_releaseInstance(uapi);
	}

#if defined(CONFIG_DEBUG)
	seuapi_CmdInit();
#endif

	return ERR_OK;

#else
	DRV_HOTPLUG_Init();

	return ERR_OK;
#endif


}

HERROR SEUAPI_RegisterFormatProgressCallback (DI_FS_Event_Callback_Func pfEvtCallback)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	s_pfFsEventCb = pfEvtCallback;

	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_RegisterNotifyCallback(uapi->nRpcHandle, RPC_SEUMMA_onFormatProgress, "iiii", (void*)uapi, seuapi_onFormatProgress);
		seuapi_releaseInstance(uapi);
	}
	return ERR_OK;
#else
	return DI_FS_RegisterFormatProgressCallback(pfEvtCallback);
#endif
}

HERROR SEUAPI_FormatLists(DI_FS_FormatList_t *pstFormatList, HUINT32 nonblock)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	SEUMMA_FormatList_t	stList;
	HxSTD_memset(&stList, 0, sizeof(SEUMMA_FormatList_t));

	HxLOG_Trace();
	seuapi_ConvertFormatListToSeumma(pstFormatList, &stList);

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_Format, "ib", nonblock, (void *)&stList, sizeof(SEUMMA_FormatList_t));
		seuapi_releaseInstance(uapi);
	}
	return ERR_OK;
#else
	return DI_FS_FormatLists((DI_FS_FormatList_t*)pstFormatList, TRUE);
#endif
}


HERROR SEUAPI_MountPartition(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_MountPartition_Cb, (void *)pstItem, RPC_SEUAPI_Mount, "iisi", eDevIdx, ulPartIdx, szDstMountDir, eFsType);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance \n");
		hErr = ERR_FAIL;
		goto DONE;
	}
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	return DI_FS_MountPartition((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, szDstMountDir, eFsType);
#endif
}

HERROR SEUAPI_UmountPartition(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_UmountPartition_Cb, (void *)pstItem, RPC_SEUAPI_Umount, "iii", eDevIdx, ulPartIdx, eFsType);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance !!!\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	return DI_FS_UnmountPartition((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, eFsType);
#endif
}

HERROR SEUAPI_MountPartition_Ftps(HUINT8 *pszStorPath, HUINT8 *pszBindPath)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_MountPartition_Ftps_Cb, (void *)pstItem, RPC_SEUAPI_MountFtps, "ss", pszStorPath, pszBindPath);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance !!!\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	HCHAR szCmd[512] = { 0 };

	HLIB_DIR_Make(pszBindPath);
	HxSTD_snprintf(szCmd, sizeof(szCmd), "mount --bind %s %s", pszStorPath, pszBindPath);
	VK_SYSTEM_Command(szCmd );

	return ERR_OK;
#endif
}

HERROR SEUAPI_MountPartition_Smbs(HUINT8 *pszStorPath, HUINT8 *pszBindPath)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_MountPartition_Smbs_Cb, (void *)pstItem, RPC_SEUAPI_MountSmbs, "ss", pszStorPath, pszBindPath);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance !!!\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	HCHAR szCmd[512] = { 0 };

	HLIB_DIR_Make(pszBindPath);
	HxSTD_snprintf(szCmd, sizeof(szCmd), "mount --bind %s %s", pszStorPath, pszBindPath);
	VK_SYSTEM_Command(szCmd );

	return ERR_OK;
#endif
}

HERROR SEUAPI_DoCommand(HUINT8 *pszCmd, HUINT8 *pszBindPath)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_DoCommand_Cb, (void *)pstItem, RPC_SEUAPI_DoCommand, "ss", pszCmd, pszBindPath);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance !!!\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	HCHAR szCmd[512] = { 0 };

	HxSTD_snprintf( szCmd, sizeof(szCmd), "%s %s", pszCmd, pszBindPath );
	VK_SYSTEM_Command( szCmd );

	return ERR_OK;
#endif
}

HERROR SEUAPI_Start_Ftps(HUINT8 *pszCmdFtpd, HUINT8 *pszConfigFile)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_Start_Ftps_Cb, (void *)pstItem, RPC_SEUAPI_StartFtps, "ss", pszCmdFtpd, pszConfigFile);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance !!!\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	HCHAR szCmd[512] = { 0 };

	HxSTD_snprintf(szCmd, sizeof(szCmd)-1, "%s%s -c %s -S %s", "/usr/bin/", pszCmdFtpd, pszConfigFile, "0.0.0.0" );
	// use for debug FTP
	//HxSTD_StrNCat(szCmd, " -n -d 20", sizeof(szCmd)-1);

	VK_SYSTEM_Command (szCmd);

	return ERR_OK;
#endif
}

HERROR SEUAPI_Start_Smbs(HUINT8 *pszCmd, HUINT8 *pszConfigFile)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_Start_Smbs_Cb, (void *)pstItem, RPC_SEUAPI_StartSmbs, "ss", pszCmd, pszConfigFile);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance !!!\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	HCHAR szCmd[512] = { 0 };

	HxSTD_snprintf( szCmd, sizeof(szCmd), "%s -s %s", pszCmd, pszConfigFile );
	VK_SYSTEM_Command (szCmd);

	return ERR_OK;
#endif
}

HERROR SEUAPI_SetPassword_Smbs(HUINT8 *pszCmd, HUINT8 *pszNewPasswd)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_SetPassword_Smbs_Cb, (void *)pstItem, RPC_SEUAPI_SetPasswordSmbs, "ss", pszCmd, pszNewPasswd);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance !!!\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	FILE	*pipeFd = NULL;

	pipeFd = popen( pszCmd, "w" );
	if (NULL == pipeFd)
	{
		return ERR_FAIL;
	}
	// insert new password
	VK_TASK_Sleep(200);
	fprintf( pipeFd, "%s\n", pszNewPasswd );
	// confirm password
	VK_TASK_Sleep(200);
	fprintf( pipeFd, "%s\n", pszNewPasswd );

	pclose(pipeFd);

	return ERR_OK;
#endif
}


HERROR SEUAPI_MountPartition_Smbc(HUINT8 *pszIp, HUINT8 *pszSharedName, HUINT8 *pszPath, HUINT8 *pszId, HUINT8 *pszPassword)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_MountPartition_Smbc_Cb, (void *)pstItem, RPC_SEUAPI_MountSmbc, "sssss", pszIp, pszSharedName, pszPath, pszId, pszPassword);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance !!!\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	HCHAR szCmd[512] = { 0 };

	HLIB_DIR_Make( pszPath );
	HxSTD_PrintToStrN(szCmd, sizeof(szCmd), "mount.cifs \"//%s/%s\" \"%s\" -o user=%s,pass=%s,nounix,noserverino,iocharset=utf8,ro > /tmp/.result_mount",
							pszIp, pszSharedName, pszPath, pszId, pszPassword);
	HxLOG_Debug("\n====> Mount Cmd : %s\n", szCmd);
	VK_SYSTEM_Command( szCmd );

	return ERR_OK;
#endif
}

HERROR SEUAPI_UmountPartition_Smbc(HUINT8 *pszPath)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_UmountPartition_Smbc_Cb, (void *)pstItem, RPC_SEUAPI_UmountSmbc, "s", pszPath);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance !!!\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
#if defined(CONFIG_PROD_OS_EMULATOR)
	return (HERROR)umount2( pszPath, 0x00000001 | 0x00000002);
#else
	return (HERROR)umount2( pszPath, MNT_FORCE | MNT_DETACH);
#endif /* CONFIG_PROD_OS_EMULATOR */

#endif /* CONFIG_ENHANCED_SECURITY */
}

HERROR SEUAPI_PopenCmd_Smbc(HUINT8 *pszCmd, HUINT8 *pszResultFile)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_PopenCmd_Smbc_Cb, (void *)pstItem, RPC_SEUAPI_PopenCmdSmbc, "ss", pszCmd, pszResultFile);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Can't get instance !!!\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	FILE	*pipeFd = NULL;
	FILE	*pfile = NULL;
	HCHAR	szBuf[128] = { 0 } ;

	pipeFd = popen(pszCmd, "r" );
	if( NULL == pipeFd )
	{
		HxLOG_Error("[SMBC] pOpenCmd failed.\n");
		return ERR_FAIL;
	}
	/* 2. parsing output data from pipe  */
	pfile = fopen(pszResultFile, "w");
	if( NULL == pfile )
	{
		HxLOG_Error("[SMBC] Failed to open %s\n", pszResultFile);
		pclose(pipeFd);
		return ERR_FAIL;
	}

	while (fgets( szBuf, sizeof(szBuf), pipeFd ) )
	{
		fprintf(pfile, szBuf);
	}
	fclose(pfile);
	pclose(pipeFd);

	return ERR_OK;
#endif
}

HERROR SEUAPI_MountFlashPartition(void)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_MountFlash, NULL);
		seuapi_releaseInstance(uapi);
	}

	return ERR_OK;
#else
	return DI_FS_FLASH_MountPartitions();
#endif
}

HERROR SEUAPI_UnmountFlashPartition(void)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_UnmountFlash, NULL);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	return DI_FS_FLASH_UmountPartitions();
#endif
}

HERROR SEUAPI_OpenDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)
{
#if defined(CONFIG_ENHANCED_SECURITY)

	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_OpenDevMbrBuf, "i", eDevIdx);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	return DI_FS_OpenDevMbrBuf(eDevIdx);
#endif
}


HERROR SEUAPI_DeleteAllPartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_DeleteAllPartitionInDevMbrBuf, "i", eDevIdx);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}
	return ERR_OK;
#else
	return DI_FS_DeleteAllPartitionInDevMbrBuf(eDevIdx);
#endif
}


HERROR SEUAPI_UpdatePartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, DI_FileSystemType_t eFsType)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_UpdatePartitionInDevMbrBuf, "iiiii", eDevIdx, ulPartIdx, ulStartUnit, ulEndUnit, eFsType);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	return DI_FS_UpdatePartitionInDevMbrBuf((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, ulStartUnit, ulEndUnit, eFsType);
#endif


}

HERROR SEUAPI_SetPartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT64 ullPartSizeKb, DI_FileSystemType_t fsType)
{

#if defined(CONFIG_ENHANCED_SECURITY)

	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_SetPartitionInDevMbrBuf, "iiiii", eDevIdx, ulPartIdx, (ullPartSizeKb >> 32), (ullPartSizeKb || 0xFFFFFFFF), fsType);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}
	return ERR_OK;
#else
	return DI_FS_SetPartitionInDevMbrBuf((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, ullPartSizeKb, fsType);
#endif
}

HERROR SEUAPI_SyncDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_SyncDevMbrBuf, "i", eDevIdx);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}
	return ERR_OK;
#else
	return DI_FS_SyncDevMbrBuf(eDevIdx);
#endif

}

HERROR	SEUAPI_CloseDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_CloseDevMbrBuf, "i", eDevIdx);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}
	return ERR_OK;
#else
	return DI_FS_CloseDevMbrBuf(eDevIdx);
#endif
}

HERROR SEUAPI_GetDevSize(HCHAR *pszDevName, HUINT64 *pullTotalKb)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	memset(pstItem, 0, sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle,  (HxRPC_Response_t)seuapi_GetDevSize_Cb, (void *)pstItem, RPC_SEUAPI_GetDevSize, "s", pszDevName);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	hErr = (HUINT32)pstItem->ulP1;

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! \n");
		goto DONE;
	}

	if (pullTotalKb)		{*pullTotalKb = pstItem->ulP2;	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	return HLIB_STORAGE_GetDevSize(pszDevName, pullTotalKb);
#endif


}

HERROR SEUAPI_GetPathSize(HCHAR *pszPathName, HUINT64 *pullSizeKb)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	memset(pstItem, 0, sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle,  (HxRPC_Response_t)seuapi_GetPathSize_Cb, (void *)pstItem, RPC_SEUAPI_GetPathSize, "s", pszPathName);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	hErr = (HUINT32)pstItem->ulP1;

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! \n");
		goto DONE;
	}

	if (pullSizeKb)		{*pullSizeKb = pstItem->ulP2;	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	return HLIB_STORAGE_GetPathSize(pszPathName, pullSizeKb);
#endif


}


HERROR SEUAPI_GetPartitionFromDeviceMbr(DI_FS_StorDevIdx_t eDevIdx, DI_FS_PartitionList_t *pstPartList)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	HxPair_t	*pstPair = NULL;

	HxLOG_Trace();

	pstPair = (HxPair_t *)HLIB_MEM_Malloc(sizeof(HxPair_t));
	if (pstPair == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	if (pstPair)
	{
		pstPair->left  = (void *)NULL;
	}

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HxLOG_Trace();
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_GetPartitionFromDeviceMbr_Cb, (void *)pstPair, RPC_SEUAPI_GetPartitionFromDeviceMbr, "i", eDevIdx);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	hErr = (HUINT32)pstPair->right;

	if (pstPair->left)		{
		HxSTD_memcpy(pstPartList, pstPair->left, sizeof(DI_FS_PartitionList_t));
	}

DONE:
	if (pstPair)
	{
		if (pstPair->left)
		{
			HLIB_MEM_Free(pstPair->left);
		}
		HLIB_MEM_Free(pstPair);
	}
	return hErr;
#else
	return DI_FS_GetPartitionFromDeviceMbr(eDevIdx, pstPartList);
#endif
}

HERROR	SEUAPI_InitFormatList(DI_FS_FormatList_t *pstList)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	if (pstList == NULL)
	{
		return ERR_FAIL;
	}

	VK_memset (pstList, 0, sizeof(DI_FS_FormatList_t));

	return ERR_OK;
#else
	return DI_FS_InitFormatList(pstList);
#endif

}

HERROR SEUAPI_AddFormatItemToList(DI_FS_FormatList_t *pstList, DI_FS_FormatItem_t *pstItem)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	DI_FS_FormatItem_t		*pstNewArray;

	if (pstList == NULL || pstItem == NULL) 		{ return ERR_FAIL; }

	if (pstList->nNumFormatPartition >= pstList->nMaxFormatPartition)
	{
		pstNewArray = HLIB_MEM_Malloc(sizeof(DI_FS_FormatItem_t) * (pstList->nMaxFormatPartition + 5));
		if (pstNewArray == NULL)					{ return ERR_FAIL; }

		if (pstList->nNumFormatPartition > 0 && pstList->pstPartParam != NULL)
		{
			VK_memcpy (pstNewArray, pstList->pstPartParam, sizeof(DI_FS_FormatItem_t) * pstList->nNumFormatPartition);
			HLIB_MEM_Free (pstList->pstPartParam);
		}

		pstList->nMaxFormatPartition += 5;
		pstList->pstPartParam = pstNewArray;
	}

	VK_memcpy (&(pstList->pstPartParam[pstList->nNumFormatPartition]), pstItem, sizeof(DI_FS_FormatItem_t));
	pstList->nNumFormatPartition ++;

	return ERR_OK;
#else
	return DI_FS_AddFormatItemToList(pstList, pstItem);
#endif

}

HERROR SEUAPI_ClearFormatList(DI_FS_FormatList_t *pstList)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	if (pstList == NULL)			{ return ERR_FAIL; }

	if (pstList->pstPartParam != NULL)
	{
		HLIB_MEM_Free (pstList->pstPartParam);
	}

	VK_memset (pstList, 0, sizeof(DI_FS_FormatList_t));
	return ERR_OK;
#else
	return DI_FS_ClearFormatList(pstList);
#endif
}

HERROR SEUAPI_GetMountedPartitionStatus(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType, HUINT64 *pullTotalKbSize, HUINT64 *pullUsedKbSize, HBOOL *pbWritable)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	memset(pstItem, 0, sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_GetMountedPartitionStatus_Cb, (void *)pstItem, RPC_SEUAPI_GetMountedPartitionStatus, "iii", eDevIdx, ulPartIdx, eFsType);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	hErr = (HUINT32)pstItem->ulP1;

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! \n");
		goto DONE;
	}

	if (pullTotalKbSize)		{*pullTotalKbSize = pstItem->ulP2;	}
	if (pullUsedKbSize)		{*pullUsedKbSize = pstItem->ulP3;	}
	if (pbWritable)	{*pbWritable = (HBOOL)pstItem->ulP4;	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	return DI_FS_GetMountedPartitionStatus((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, eFsType, pullTotalKbSize, pullUsedKbSize, pbWritable);
#endif

}

HERROR SEUAPI_ExecuteCheckDisk(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_ExecuteCheckDisk, "iii", eDevIdx, ulPartIdx, eFsType);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, 10000);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}
	return ERR_OK;
#else
	return DI_FS_ExecuteCheckDisk(eDevIdx, ulPartIdx, eFsType);
#endif
}



HERROR SEUAPI_GetDeviceFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	if (szFileName == NULL) 		{ return ERR_FAIL; }

	//prevent : defect Fix / 2011.03.27 mhpakr3
	//sprintf (szFileName, "sd%c", 'a' + eDevIdx - eDI_FS_DEV_sda);
	snprintf ((char *)szFileName, DI_DEV_NODE_LEN ,"sd%c", 'a' + eDevIdx - eDI_FS_DEV_sda);
	return ERR_OK;
#else
	return DI_FS_GetDeviceFileName(eDevIdx, szFileName);
#endif
}

HERROR SEUAPI_GetDeviceFileFullName (DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	if (szFileName == NULL)			{ return ERR_FAIL; }

	//prevent : defect Fix / 2011.03.29 mhpark3
	//sprintf (szFileName, "/dev/sd%c", 'a' + eDevIdx - eDI_FS_DEV_sda);
	snprintf ((char *)szFileName, DI_FS_DEVFILE_LEN,"/dev/sd%c", 'a' + eDevIdx - eDI_FS_DEV_sda);
	return ERR_OK;
#else
	return DI_FS_GetDeviceFileFullName(eDevIdx, szFileName);
#endif

}


HERROR SEUAPI_GetPartitionFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName, HUINT8 nSizeOfFileName)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	if (szFileName == NULL) 		{ return ERR_FAIL; }
	//prevent : defect Fix / 2011.03.29 mhpark3
	//sprintf (szFileName, "sd%c%d", 'a' + eDevIdx - eDI_FS_DEV_sda, ulPartitionIdx + 1);
	snprintf ((char *)szFileName, nSizeOfFileName, "sd%c%d", 'a' + eDevIdx - eDI_FS_DEV_sda, ulPartitionIdx + 1);
	return ERR_OK;
#else
	return DI_FS_GetPartitionFileName(eDevIdx, ulPartitionIdx, szFileName, nSizeOfFileName);
#endif

}


HERROR SEUAPI_GetVolumeUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf, HUINT32 ulBufsize, HUINT32 *pulUuidsize)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_GetVolumeUuid_Cb, (void *)pstItem, RPC_SEUAPI_GetVolumeUuid, "iii", eDevIdx, ulPartIdx, ulBufsize);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT*2);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if(pulUuidsize)
	{
		*pulUuidsize = pstItem->ulP2;
	}
	if (pstItem->pszStr)
	{
		HxSTD_StrNCpy(pBuf, pstItem->pszStr, ulBufsize);
	}

DONE:
	if (pstItem)
	{
		if (pstItem->pszStr)
		{
			HLIB_MEM_Free((void *)pstItem->pszStr);
		}
		HLIB_MEM_Free(pstItem);
	}
	return hErr;
#else
	return DI_FS_GetVolumeUuid(eDevIdx, ulPartIdx, pBuf, ulBufsize, pulUuidsize);
#endif

}

HERROR SEUAPI_SetVolumeName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_SetVolumeName, "iis", eDevIdx, ulPartIdx, pBuf);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}
	return ERR_OK;

#else
	return DI_FS_SetVolumeName(eDevIdx, ulPartIdx, pBuf);
#endif
}

HERROR SEUAPI_GetVolumeName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf, HUINT32 ulBufsize)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_GetVolumeName_Cb, (void *)pstItem, RPC_SEUAPI_GetVolumeName, "iii", eDevIdx, ulPartIdx, ulBufsize);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (pstItem->pszStr)
	{
		HxSTD_StrNCpy(pBuf, pstItem->pszStr, ulBufsize);
	}

DONE:
	if (pstItem)
	{
		if (pstItem->pszStr)
		{
			HLIB_MEM_Free((void *)pstItem->pszStr);
		}
		HLIB_MEM_Free(pstItem);
	}
	return hErr;

#else
	return DI_FS_GetVolumeName(eDevIdx, ulPartIdx, pBuf, ulBufsize);
#endif

}


static DxHotplug_DevType_e seuapi_hotplug_convertDevType(UMMALIB_HP_DEV_EVT_e 	eDevType)
{
	DxHotplug_DevType_e		eHpDevType;

	switch (eDevType)
	{
		case eUMMALIB_HP_HDD:
			eHpDevType = eDxHOTPLUG_DEVTYPE_HDD;
			break;

		case eUMMALIB_HP_USB:
			eHpDevType = eDxHOTPLUG_DEVTYPE_USB;
			break;

		case	 eUMMALIB_HP_USB_STOR:
			eHpDevType = eDxHOTPLUG_DEVTYPE_USB_STOR;
			break;

		case  eUMMALIB_HP_USB_INPUT:
			eHpDevType = eDxHOTPLUG_DEVTYPE_USB_INPUT;
			break;

		case eUMMALIB_HP_USB_WIRELESS:	//		case eUMMALIB_HP_WIRELESS:
			eHpDevType = eDxHOTPLUG_DEVTYPE_USB_WIRELESS;
			break;

		case eUMMALIB_HP_SD:
			eHpDevType = eDxHOTPLUG_DEVTYPE_SD;
			break;

		case eUMMALIB_HP_MIC:
			eHpDevType = eDxHOTPLUG_DEVTYPE_MIC;
			break;

		case eUMMALIB_HP_HID:
			eHpDevType = eDxHOTPLUG_DEVTYPE_HID;
			break;

		default:
			eHpDevType = eDxHOTPLUG_DEVTYPE_NONE;
			break;
	}

	return eHpDevType;
}

static DxHotplug_ConnectType_e seuapi_hotplug_convertConnectType(UMMALIB_HP_CONNECT_EVT_e 	eConnectType)
{
	DxHotplug_ConnectType_e		eHpConType;

	switch (eConnectType)
	{
		case eUMMALIB_HP_CONN:
			eHpConType = eDxHOTPLUG_CONN;
			break;

		case eUMMALIB_HP_DISCONN:
			eHpConType = eDxHOTPLUG_DISCONN;
			break;

		case	 eUMMALIB_HP_CONN_BY_PWR:
			eHpConType = eDxHOTPLUG_CONN_BY_PWR;
			break;

		case  eUMMALIB_HP_DISCONN_BY_PWR:
			eHpConType = eDxHOTPLUG_DISCONN_BY_PWR;
			break;

		default:
			eHpConType = eDxHOTPLUG_CONN_ALL;
			break;
	}

	return eHpConType;

}

static HERROR seuapi_hotplug_Callback(HUINT32 ulSlotIdx, HUINT32 eDevType, void *pvEventInfo)
{
	UMMALIB_HP_Info_t 	*pstSeUmmaHpInfo = (UMMALIB_HP_Info_t *)pvEventInfo;
	DxHotplug_Info_t		stHpInfo;

	stHpInfo.ulDeviceId = ulSlotIdx;
	stHpInfo.eDevType = seuapi_hotplug_convertDevType(eDevType);
	stHpInfo.nPortNum = pstSeUmmaHpInfo->nPortnum;

	s_pfHotplugNotiCb(ulSlotIdx, seuapi_hotplug_convertConnectType(pstSeUmmaHpInfo->eConnEvt), &stHpInfo);

	return ERR_OK;
}

// 0: HINT32 nSlotIdx
// 1: DI_USB_Event_t eDevType,
// 2: void *pucHotplugInfo
STATIC HERROR seuapi_onHotplugNotiCb(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMALIB_HP_Info_t 	*pstSeUmmaHpInfo, stHpInfo;
	UMMALIB_HP_DEV_EVT_e 	eDevType;
	HUINT32		ulSlotIdx;

	if (nArgc != 3)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}
	ulSlotIdx = HxOBJECT_INT(apArgv[0]);
	eDevType = HxOBJECT_INT(apArgv[1]);
	pstSeUmmaHpInfo = (UMMALIB_HP_Info_t *)HxOBJECT_BIN_DATA(apArgv[2]);

	HxSTD_memcpy(&stHpInfo, pstSeUmmaHpInfo, sizeof(UMMALIB_HP_Info_t));

	if (s_pfHotplugNotiCb)
	{
		seuapi_hotplug_Callback(ulSlotIdx, eDevType, (void *)&stHpInfo);
	}

	return ERR_OK;
}

HERROR SEUAPI_Hotplug_Init(void)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	HxLOG_Trace();
	uapi = seuapi_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_HotplugInit, NULL);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;

#else
	return UMMALIB_Hotplug_Init();
#endif
}


HERROR SEUAPI_Hotplug_RegisterCallback(pfHotplug_NotifyFUNC pfnNotify)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	HxLOG_Trace();

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_RegisterNotifyCallback(uapi->nRpcHandle, RPC_SEUMMA_onHotplugNotifier, "iib", (void*)uapi, seuapi_onHotplugNotiCb);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

#else
	UMMALIB_Hotplug_RegisterCallback(seuapi_hotplug_Callback);
#endif

	s_pfHotplugNotiCb = pfnNotify;
	return ERR_OK;
}

HERROR SEUAPI_GetFsType (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t *peFsType)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_GetFsType_Cb, (void *)pstItem, RPC_SEUAPI_GetFsType, "ii", eDevIdx, ulPartIdx);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (peFsType)
	{
		*peFsType = pstItem->ulP2;
	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem);	}
	return hErr;

#else
	return DI_FS_GetFsType(eDevIdx, ulPartIdx, peFsType);
#endif
}


#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
HERROR SEUAPI_ENCR_Format(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szPassword)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_EncrFormat, "iis", eDevIdx, ulPartIdx, szPassword);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}
	return ERR_OK;
#else
	return DI_FS_ENCR_Format((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, szPassword);
#endif
}


HERROR SEUAPI_ENCR_Open(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMapperDeviceName, char *szPassword)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr;

	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_EncrOpen, "iiss", eDevIdx, ulPartIdx, szMapperDeviceName, szPassword);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}
	return ERR_OK;
#else
	return DI_FS_ENCR_Open((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, szMapperDeviceName, szPassword);
#endif
}

HERROR SEUAPI_ENCR_Close(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_EncrClose, "ii", eDevIdx, ulPartIdx);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}
	return ERR_OK;
#else
	return DI_FS_ENCR_Close((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx);
#endif
}


HERROR SEUAPI_ENCR_Mount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMountDir, DI_FileSystemType_t eFsType)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_EncrMount, "iisi", eDevIdx, ulPartIdx, szMountDir, eFsType);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}
	return ERR_OK;
#else
	return DI_FS_ENCR_Mount(eDevIdx, ulPartIdx, szMountDir, eFsType);
#endif
}

HERROR SEUAPI_ENCR_Umount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_EncrUmount, "ii", eDevIdx, ulPartIdx);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Call is not finished..\n");
		return ERR_FAIL;
	}
	return ERR_OK;
#else
	return DI_FS_ENCR_Umount(eDevIdx, ulPartIdx);
#endif
}
#endif

#define		_______HDD___________
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)

// 0: DI_HDD_Event_t etHddEventType,
// 1: void *pucHddInfo
STATIC HERROR seuapi_onHddNotiCb(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_HDD_Event_t etHddEventType;
	void *pucHddInfo;

	if (nArgc != 2)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxLOG_Warning(" \n");

	etHddEventType = HxOBJECT_INT(apArgv[0]);
	pucHddInfo = HxOBJECT_BIN_DATA(apArgv[1]);

	if (s_pfHddNotiCb)
	{
		s_pfHddNotiCb (etHddEventType, pucHddInfo);
	}

	return ERR_OK;

}



// 0: HDD_DST_RESULT dstResult,
// 1: int progressStatus
STATIC HERROR seuapi_onDstNotiCb(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HDD_DST_RESULT eDstResult;
	HUINT32	ulProgress;

	if (nArgc != 2)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	eDstResult = HxOBJECT_INT(apArgv[0]);
	ulProgress = HxOBJECT_INT(apArgv[1]);

	if (s_pfUsbNotiCb)
	{
		s_pfDstNotiCb (eDstResult, ulProgress);
	}

	return ERR_OK;
}

HERROR SEUAPI_RegisterDstCallback(pfnDI_HDD_DSTNotify pfnNotify)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	s_pfDstNotiCb = pfnNotify;

	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_RegisterNotifyCallback(uapi->nRpcHandle, RPC_SEUMMA_onDstNotifier, "ii", (void*)uapi, seuapi_onDstNotiCb);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	DI_HDD_RegisterDstCallback(pfnNotify);
	return ERR_OK;
#endif

}

HERROR SEUAPI_StartDst(void)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_StartDst, NULL);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	DI_HDD_DstStart();
	return ERR_OK;
#endif
}

HERROR SEUAPI_HDD_Initialize(void)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_HddInit, NULL);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	return DI_HDD_Init();
#endif
}

HERROR SEUAPI_HDD_RegisterCallback(pfnDI_HDD_NotifyFUNC pfnNotify)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	HxLOG_Trace();
	s_pfHddNotiCb = pfnNotify;

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_RegisterNotifyCallback(uapi->nRpcHandle, RPC_SEUMMA_onHddNotifier, "ib", (void*)uapi, seuapi_onHddNotiCb);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	DI_HDD_RegisterCallback(pfnNotify);
	return ERR_OK;
#endif


}


HERROR SEUAPI_HDD_GetEncryptionKey(DI_HDD_PORT_NUM_E portnum, char *pKey)
{
#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_OK;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;
	HxLOG_Trace();

	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, (HxRPC_Response_t)seuapi_GetEncryptionKey_Cb, (void *)pstItem, RPC_SEUAPI_HddGetEncryptionKey, "i", portnum);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	if (pstItem->ulP1 != ERR_OK)
	{
		HxLOG_Error("ERROR..\n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (pstItem->pszStr)
	{
		HxSTD_StrNCpy(pKey, pstItem->pszStr, HxSTD_StrLen(pstItem->pszStr));
	}

DONE:
	if (pstItem)
	{
		if (pstItem->pszStr)
		{
			HLIB_MEM_Free((void *)pstItem->pszStr);
		}
		HLIB_MEM_Free(pstItem);
	}
	return hErr;

#else
	DI_HDD_GetEncryptionKey(portnum, pKey);
	return ERR_OK;
#endif

#else // #if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
	return ERR_FAIL;
#endif
}


HERROR SEUAPI_HDD_ControlPower(DI_HDD_PORT_NUM_E portnum, DI_HDD_PWR on)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_HddControlPower, "ii", portnum, on);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	return DI_HDD_ControlPower(portnum, on);
#endif
}

HERROR SEUAPI_HDD_PWR_Ctrl(DI_HDD_PWR OnOff)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HxLOG_Trace();

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_HddPwCtrl, "i", OnOff);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	return DI_HDD_PWR_Ctrl(OnOff);
#endif
}
#endif

#define		______USB_______

#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
// 0: DI_USB_Event_t etUsbEventType,
// 1: void *pucUsbInfo
STATIC HERROR seuapi_onUsbNotiCb(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_USB_Event_t etUsbEventType;
	usb_info_t stDiUsbInfo;
	SEUMMA_USB_Info_t	*pstSeUmmaUsbInfo;
	HUINT32		ulIdx;

	if (nArgc != 2)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	etUsbEventType = HxOBJECT_INT(apArgv[0]);
	pstSeUmmaUsbInfo = (SEUMMA_USB_Info_t	*)HxOBJECT_BIN_DATA(apArgv[1]);

	stDiUsbInfo.devCount = pstSeUmmaUsbInfo->ulDevCount;
	stDiUsbInfo.devNameSize = pstSeUmmaUsbInfo->ulDevNameSize;
	stDiUsbInfo.ePortNum = (DI_USB_PORT_Num_t)pstSeUmmaUsbInfo->ulPortNum;
	stDiUsbInfo.eSpeed = (DI_USB_SPEED_t)pstSeUmmaUsbInfo->ulSpeed;
	stDiUsbInfo.isOverCurrent = pstSeUmmaUsbInfo->bOverCurrent;

	stDiUsbInfo.devName = (char **)HLIB_STD_MemAlloc( sizeof(char *) * stDiUsbInfo.devCount );
	for (ulIdx=0; ulIdx<stDiUsbInfo.devCount; ulIdx++)
	{
		if (stDiUsbInfo.devNameSize > 0)
		{
			stDiUsbInfo.devName[ulIdx] = (char *)HLIB_STD_MemAlloc( sizeof(char) * stDiUsbInfo.devNameSize);
			HxSTD_StrNCpy(stDiUsbInfo.devName[ulIdx], &pstSeUmmaUsbInfo->szDevNameBuf[ulIdx*4], 4);
		}
	}

	if (s_pfUsbNotiCb)
	{
		s_pfUsbNotiCb (etUsbEventType, (void *)&stDiUsbInfo);
	}

	return ERR_OK;
}


HERROR SEUAPI_USB_Initialize(void)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_UsbInit, NULL);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	return DI_USB_Init();
#endif
}

HERROR SEUAPI_USB_RegisterCallback(pfnDI_USB_NotifyFUNC pfnNotify)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	HxLOG_Trace();
	s_pfUsbNotiCb = pfnNotify;

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_RegisterNotifyCallback(uapi->nRpcHandle, RPC_SEUMMA_onUsbNotifier, "ib", (void*)uapi, seuapi_onUsbNotiCb);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	DI_USB_RegisterCallback(pfnNotify);
	return ERR_OK;
#endif

}

HERROR SEUAPI_USB_PWR_Ctrl(DI_USB_PWR_t OnOff)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;

	HxLOG_Trace();
	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_SEUAPI_UsbPwCtrl, "i", OnOff);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	return DI_USB_PWR_Ctrl(OnOff);
#endif
}

HERROR SEUAPI_USB_IsDeviceRemovable(DI_FS_StorDevIdx_t eDevIdx, HBOOL *pbRemovable)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	SEUAPI_t *uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_FAIL;
	SE_UMMA_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();
	pstItem = (SE_UMMA_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(SE_UMMA_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	memset(pstItem, 0, sizeof(SE_UMMA_RPC_Item_t));

	uapi = seuapi_getInstanceWO();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle,  (HxRPC_Response_t)seuapi_UsbIsDeviceRemovable_Cb, (void *)pstItem, RPC_SEUAPI_UsbIsDeviceRemovable, "i", eDevIdx);
		hErr = HLIB_RPC_Join(uapi->nRpcHandle, nReqId, SEUMMA_RPC_JOIN_TIMEOUT);
		seuapi_releaseInstance(uapi);
	}
	else
	{
		HxLOG_Error("Error!!! Can't get instance \n");
		hErr = ERR_FAIL;
		goto DONE;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! HLIB_RPC_Request is not finished..\n");
		goto DONE;
	}

	hErr = (HUINT32)pstItem->ulP1;

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!! \n");
	}
	HxLOG_Debug("pstItem->ulP2 : %lld\n",pstItem->ulP2);

	if (pbRemovable)		{*pbRemovable = (HBOOL)pstItem->ulP2;	}

DONE:
	if (pstItem)	{	HLIB_MEM_Free(pstItem); }
	return hErr;
#else
	int			 nRmvFileDsr, nRet;
	int			 nRemovable;
	char		 szNumString[8], szRmvFileName[32];

	nRemovable = 0;

	MACRO_GetDevRmvFileName (szRmvFileName, eDevIdx);
	nRmvFileDsr = open (szRmvFileName, O_RDONLY);
	if (nRmvFileDsr >= 0)
	{
		nRet = read (nRmvFileDsr, szNumString, sizeof(char));
		if (nRet > 0)
		{
			szNumString[1] = '\0';
			nRemovable = atoi (szNumString);
		}

		close (nRmvFileDsr);
	}

	if (pbRemovable)		{*pbRemovable = (nRemovable != 0) ? TRUE : FALSE;	}

	return ERR_OK;
#endif
}
#endif

#if defined(CONFIG_DEBUG)
STATIC int umma_fs_svc_CmdTestSeUmma (void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20]; //aucArg2[256], cid[256];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	// To be implemented...
	// Add cmd if you need...

	if(HxSTD_StrCmp(aucStr, "1") == 0)
	{

	}
	if(HxSTD_StrCmp(aucStr, "2") == 0)
	{
	}
	if(HxSTD_StrCmp(aucStr, "3") == 0)
	{
	}
	if(HxSTD_StrCmp(aucStr, "4") == 0)
	{
	}
	if(HxSTD_StrCmp(aucStr, "5") == 0)
	{
	}
	if(HxSTD_StrCmp(aucStr, "6") == 0)
	{
	}
	if(HxSTD_StrCmp(aucStr, "7") == 0)
	{
	}
	if(HxSTD_StrCmp(aucStr, "8") == 0)
	{
	}
	if(HxSTD_StrCmp(aucStr, "9") == 0)
	{
	}
	if(HxSTD_StrCmp(aucStr, "10") == 0)
	{
	}
	if(HxSTD_StrCmp(aucStr, "11") == 0)
	{
	}
	else
	{

	}

	return ERR_OK;
}

STATIC void seuapi_CmdInit(void)
{
#define	hCmdHandle		"SEUMMA"
	HLIB_CMD_RegisterWord(hCmdHandle,
							umma_fs_svc_CmdTestSeUmma,
							"test", 						/* keyword */
							"test SE_UMMA module",			/*	 help  */
							"test"
							);
}

#endif


