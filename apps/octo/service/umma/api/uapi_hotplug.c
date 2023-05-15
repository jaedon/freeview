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
#define	ENTER_UAPI_HOTPLUG_CACHE		HxSEMT_Get(s_ulUapiFsSemId)
#define	LEAVE_UAPI_HOTPLUG_CACHE		HxSEMT_Release(s_ulUapiFsSemId)


#define	NAME(type)	#type

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
STATIC HINT32					s_nHotplugNum = 0;
STATIC DxHotplug_Info_t		*s_pstHotplugInfos = NULL;
STATIC DxHotplugNotifier_t		s_pfnUpaiHpListener = NULL;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
STATIC UAPI_t*	uapi_hotplug_getInstance(void);
STATIC void		uapi_hotplug_releaseInstance(UAPI_t *uapi);

STATIC HCHAR * uapi_hotplug_getDevTypeStr(DxHotplug_DevType_e eType)
{
	switch(eType)
	{
		case eDxHOTPLUG_DEVTYPE_HDD:
			return NAME(eDxHOTPLUG_DEVTYPE_HDD);

		case eDxHOTPLUG_DEVTYPE_USB:
			return NAME(eDxHOTPLUG_DEVTYPE_USB);

		case eDxHOTPLUG_DEVTYPE_USB_STOR:
			return NAME(eDxHOTPLUG_DEVTYPE_USB_STOR);

		case eDxHOTPLUG_DEVTYPE_USB_INPUT:
			return NAME(eDxHOTPLUG_DEVTYPE_USB_INPUT);

		case eDxHOTPLUG_DEVTYPE_USB_WIRELESS:
			return NAME(eDxHOTPLUG_DEVTYPE_USB_WIRELESS);

		case eDxHOTPLUG_DEVTYPE_SD:
			return NAME(eDxHOTPLUG_DEVTYPE_SD);

		case eDxHOTPLUG_DEVTYPE_MIC:
			return NAME(eDxHOTPLUG_DEVTYPE_MIC);

		case eDxHOTPLUG_DEVTYPE_HID:
			return NAME(eDxHOTPLUG_DEVTYPE_HID);

		default :
			return NAME(eDxHOTPLUG_DEVTYPE_NONE);
	}
}

#define _____NOTIFICATION_CALLBACKS_____
//	apArgv[0] : nDeviceId (i)
//	apArgv[1] : nConnected (i)
//	apArgv[2] : DxHotplug_Info_t * pstItem (b)
//STATIC void	uapi_hotplug_onDeviceConnected(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], void *userdata)
STATIC HERROR	uapi_hotplug_onDeviceConnected(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 	ulDeviceId;
	HBOOL		bConnected;
	DxHotplug_Info_t	*pstItem = NULL;
	HINT32	nSize;

	ulDeviceId = HxOBJECT_INT(apArgv[0]);
	bConnected = HxOBJECT_INT(apArgv[1]);
	pstItem = (DxHotplug_Info_t *)UAPI_MemDup(HxOBJECT_BIN_DATA(apArgv[2]), HxOBJECT_BIN_SIZE(apArgv[2]));
	nSize = HxOBJECT_BIN_SIZE(apArgv[2]);

	if (pstItem == NULL)
	{
		HxLOG_Error("MemDup Fail!! \n");
		return ERR_FAIL;
	}

	HxLOG_Warning("ulDeviceId : %d, bConnected : %d, eDevType : %s\n", ulDeviceId, bConnected, uapi_hotplug_getDevTypeStr(pstItem->eDevType));

	if (s_pfnUpaiHpListener)
	{
		HxLOG_Debug("ulDeviceId : %d Notified \n", pstItem->ulDeviceId);
		s_pfnUpaiHpListener(eDxHOTPLUG_NOTI_DevChange, (HINT32)bConnected, (HINT32)pstItem, nSize);
	}
	if(pstItem) {UAPI_Free((void *)pstItem);}

	return ERR_OK;
}

//	apArgv[0] : nStorageNum (i)
//	apArgv[1] : OxFileMgrStorageInfo_t * nStorageNum (b)   (if only	nStorageNum > 0)
STATIC HERROR	uapi_hotplug_onChangeDevice(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32	nStorageNum;
	UAPI_t	*uapi;

	uapi = uapi_hotplug_getInstance();

	if (s_pstHotplugInfos)
		UAPI_Free(s_pstHotplugInfos);

	s_nHotplugNum  = 0;
	s_pstHotplugInfos = NULL;

	nStorageNum = HxOBJECT_INT(apArgv[0]);

	if (nStorageNum)
	{
		s_nHotplugNum = HxOBJECT_INT(apArgv[0]);
		s_pstHotplugInfos = (DxHotplug_Info_t *)UAPI_MemDup(HxOBJECT_BIN_DATA(apArgv[1]), HxOBJECT_BIN_SIZE(apArgv[1]));
		HxLOG_Debug("s_nHotplugNum : %d\n", s_nHotplugNum);
	}
	else
	{
		HxLOG_Debug("Empty list~! \n");
		// Empty list
	}

#if defined(CONFIG_DEBUG)
	{
		HUINT32	ulIdx;
		DxHotplug_Info_t	*info;

		HxLOG_Debug("========== Update =========\n");
		for(ulIdx = 0 ; ulIdx < s_nHotplugNum ; ulIdx++)
		{
			info = &s_pstHotplugInfos[ulIdx];
			HxLOG_Debug("DeviceId : %d, DevType : %s, PortNum : %d\n", info->ulDeviceId, uapi_hotplug_getDevTypeStr(info->eDevType), info->nPortNum);
		}
		HxLOG_Debug("\n");
	}
#endif

	uapi_hotplug_releaseInstance(uapi);

	return ERR_OK;
}

static UAPI_t	*s_pstUapiHp = NULL;

STATIC UAPI_t*	uapi_hotplug_getInstanceWO(void)
{
	return s_pstUapiHp;
}

STATIC UAPI_t*	uapi_hotplug_getInstance(void)
{
	if (s_pstUapiHp == NULL)
	{
		HERROR	hErr;

		s_pstUapiHp = (UAPI_t*)UAPI_Calloc(sizeof(UAPI_t));

		HxSEMT_Create(&s_pstUapiHp->ulSemId, "uapihpsem", 0);

		HxSEMT_Get(s_pstUapiHp->ulSemId);
		hErr = UAPI_RpcProtocol_GetRpcHandle(&s_pstUapiHp->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error !! (GetRpcHandle)\n");
			HxLOG_Assert(s_pstUapiHp->nRpcHandle);
		}

		s_pstUapiHp->pstCallbackTable = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, UAPI_Free_CB);

		//	Register UMMA Event Notifier
		HLIB_RPC_RegisterNotifyCallback(s_pstUapiHp->nRpcHandle, RPC_UMMA_HOTPLUG_onDeviceConnected,  "iib" ,(void*)s_pstUapiHp, uapi_hotplug_onDeviceConnected);
		HLIB_RPC_RegisterNotifyCallback(s_pstUapiHp->nRpcHandle, RPC_UMMA_HOTPLUG_onChangeDevice,  "ib", (void*)s_pstUapiHp, uapi_hotplug_onChangeDevice);
	}
	else
	{
		HxSEMT_Get(s_pstUapiHp->ulSemId);
	}

	return s_pstUapiHp;
}

STATIC void		uapi_hotplug_releaseInstance(UAPI_t *uapi)
{
	HxSEMT_Release(uapi->ulSemId);
}

#define _____


#define ___GLOBAL_FUNCTIONS___
HERROR	UAPI_HOTPLUG_Init(void)
{
	UAPI_t *uapi;

	HxLOG_Trace();
	uapi = uapi_hotplug_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_HOTPLUG_Init, NULL);
		uapi_hotplug_releaseInstance(uapi);
	}

	return ERR_OK;
}
HERROR	UAPI_HOTPLUG_SetNotifier(DxHotplugNotifier_t pfnApkListener)
{
	HxLOG_Trace();
	s_pfnUpaiHpListener = pfnApkListener;

	return ERR_OK;
}

HERROR	UAPI_HOTPLUG_GetDevCount(DxHotplug_DevType_e eType, HUINT32 *pulCount)
{
	HUINT32		ulIdx;
	UAPI_t	*uapi;
	HUINT32 ulCnt = 0;
	DxHotplug_Info_t *pstInfo = NULL;

	uapi = uapi_hotplug_getInstance();

	for(ulIdx = 0 ; ulIdx < s_nHotplugNum ; ulIdx++)
	{
		pstInfo = &s_pstHotplugInfos[ulIdx];
		if (pstInfo->eDevType == eType ||eDxHOTPLUG_DEVTYPE_ALL == eType)
		{
			ulCnt++;
		}
	}
	uapi_hotplug_releaseInstance(uapi);

	if (pulCount)
	{
		*pulCount = ulCnt;
	}

	return ERR_OK;
}
