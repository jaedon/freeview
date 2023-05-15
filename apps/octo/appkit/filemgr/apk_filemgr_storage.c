/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr_storage.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

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
#include	<hlib.h>
#include	<apk.h>

#include <uapi.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	DxStorageNotifier_t	pfNotifier;
}apkFilemgrStorage_Ctx_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static	apkFilemgrStorage_Ctx_t	s_StorageCtx;
/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/

#define	________LOCAL___Prototype______


#define	________LOCAL___Body______

apkFilemgrStorage_Ctx_t* apk_filemgr_storage_GetContext(void)
{
	return &s_StorageCtx;
}

static void	apk_filemgr_storage_freeEvent(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;
	HINT32	p1;
	DxStorage_NotiType_e	eNotiType;

	eNotiType = (DxStorage_NotiType_e)apArgV[1];
	switch (eNotiType)
	{
		case	eDxSTORAGE_NOTI_DevChange:
			pvData = apArgV[3];
			if (pvData)
				HLIB_STD_MemFree(pvData);
			break;
		case	eDxSTORAGE_NOTI_FormatState:
			p1 = (HINT32)apArgV[2];
			if (p1 == eDxSTORAGE_DEV_STATUS_Formatting)
			{
				pvData = apArgV[3];
				if (pvData)
					HLIB_STD_MemFree(pvData);
			}
			break;
		case	eDxSTORAGE_NOTI_DstState:
			p1 = (HINT32)apArgV[2];
			if (p1 == eDxSTORAGE_DEV_STATUS_DstResultOnGoing)
			{
				pvData = apArgV[3];
				if (pvData)
					HLIB_STD_MemFree(pvData);
			}
			break;
		default:
			break;
	}
}

void	apk_filemgr_storage_Notifier(DxStorage_NotiType_e eNotiType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	apkFilemgrStorage_Ctx_t* pstCtx;
	DxStorageNotifier_t pfNoti;
	APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};
	pstCtx = apk_filemgr_storage_GetContext();

	pfNoti = pstCtx->pfNotifier;
	if(pfNoti)
	{
		switch (eNotiType)
		{
			case	eDxSTORAGE_NOTI_DevChange:
				p2 = (HINT32)HLIB_MEM_MemDup((void *)p2, p3);
				p3 = 0;
				break;
			case	eDxSTORAGE_NOTI_FormatState:
				if (p1 == eDxSTORAGE_DEV_STATUS_Formatting)
				{
					p2 = (HINT32)HLIB_MEM_MemDup((void *)p2, p3);
					p3 = 0;
				}
				break;
			case eDxSTORAGE_NOTI_DstState:
				if(p1 == eDxSTORAGE_DEV_STATUS_DstResultOnGoing)
				{
					p2 = (HINT32)HLIB_MEM_MemDup((void *)p2, p3);
					p3 = 0;
				}
			default:
				break;
		}

		stApkEvtArgs.pfnFreeFunc = apk_filemgr_storage_freeEvent;
		stApkEvtArgs.ulArgCount = 5;
		stApkEvtArgs.apArgV[0] = (void *)pfNoti;
		stApkEvtArgs.apArgV[1] = (void *)eNotiType;
		stApkEvtArgs.apArgV[2] = (void *)p1;
		stApkEvtArgs.apArgV[3] = (void *)p2;
		stApkEvtArgs.apArgV[4] = (void *)p3;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

#if defined(CONFIG_DEBUG)

HINT32 apk_filemgr_storage_cmdFormat(void *szArgList)
{
	HCHAR	*aucArg=NULL;
	HCHAR	aucArg2[32];
	HUINT32	nDevId = 0;

	HxSTD_memset(aucArg2, 0, 32);

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &nDevId) == 0 ) return HxCMD_ERR;

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

	HxLOG_Warning("[%s] [%d] CMD FORMAT!! ulDeviceId : %d, Name : %s \n",__FUNCTION__,__LINE__,nDevId, aucArg2);

	UAPI_FS_Format(nDevId, aucArg2);
	return 0;
}

HINT32 apk_filemgr_storage_cmdGetHddSize(void *pvArg)
{
	DxStorage_Info_t *pstInfoList;
	HUINT32 ulDevNum;
	HUINT32 i;
	HERROR	hErr;
	HUINT64		ullTotalKbSize, pullAvailableKbSize, pullUsedKbSize, pullReservedKbSize, pullAvailablePvrKbSize;

	hErr = UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s] [%d] Empty.. \n",__FUNCTION__,__LINE__);
		return 0;
	}
	HLIB_CMD_Printf("[%s] [%d] -------- Get Storage Size ----------\n", __FUNCTION__,__LINE__);

	for (i=0 ; i<ulDevNum ; i++)
	{
		HLIB_CMD_Printf("ulDeviceId : %d\n", pstInfoList[i].ulDeviceId);
		UAPI_FS_GetSize(pstInfoList[i].ulDeviceId, &ullTotalKbSize, &pullAvailableKbSize, &pullUsedKbSize, &pullReservedKbSize, &pullAvailablePvrKbSize);

		HLIB_CMD_Printf(" ullTotalKbSize: %lld\n", ullTotalKbSize);
		HLIB_CMD_Printf(" pullAvailableKbSize: %lld\n", pullAvailableKbSize);
		HLIB_CMD_Printf(" pullUsedKbSize: %lld\n", pullUsedKbSize);
		HLIB_CMD_Printf(" pullReservedKbSize: %lld\n", pullReservedKbSize);
		HLIB_CMD_Printf(" pullAvailablePvrKbSize: %lld\n\n", pullAvailablePvrKbSize);
	}
	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	return 0;
}

HINT32 apk_filemgr_storage_cmdShowHddList(void *pvArg)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum;
	HUINT32 ulIdx, i;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);
	HxLOG_Warning("[%s] [%d] Storage num : %d\n",__FUNCTION__,__LINE__, ulDevNum);

	for (ulIdx = 0 ; ulIdx < ulDevNum ; ulIdx++)
	{
		pstInfo = &pstInfoList[ulIdx];

		HLIB_CMD_Printf("================ Storage Info ================ \n");
		HLIB_CMD_Printf("ulDeviceId : [%d]\n", pstInfo->ulDeviceId);
		HLIB_CMD_Printf("DevType : [%s]\n", UAPI_FS_GetDevTypeString(pstInfo->eDevType));
		HLIB_CMD_Printf("szMountPath : [%s]\n", pstInfo->szMountPath);
		HLIB_CMD_Printf("szDevName : [%s] \n", pstInfo->szDevName);
		HLIB_CMD_Printf("ullTotalSizeKb : [%lld] \n", pstInfo->ullTotalSizeKb);
		HLIB_CMD_Printf("szLabel : [%s]\n", pstInfo->szLabel);
		HLIB_CMD_Printf("szUUID : [%s]\n", pstInfo->szUUID);
		HLIB_CMD_Printf("ulFlags : [0x%x]\n", pstInfo->ulFlags);
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_CONNECTED));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_MOUNTED));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_SECURED));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_INTERNAL));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_NEEDFORMAT));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_PAIRED));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE));
		HLIB_CMD_Printf("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_NEW2NDHDD));

		HLIB_CMD_Printf("\n--------Partition Info--------\n");
		for (i=0; i < pstInfo->nPartitionNum ; i++)
		{
			HLIB_CMD_Printf("\n   [%d] \n",i);
			HLIB_CMD_Printf("	bAvailable : [%d]\n", pstInfo->astPartitionInfos[i].bAvailable);
			HLIB_CMD_Printf("	FsType: [%s]\n", UAPI_FS_GetFsString(pstInfo->astPartitionInfos[i].eFsType));
			HLIB_CMD_Printf("	ullPartitionSizeKb : [%lld]\n", pstInfo->astPartitionInfos[i].ullPartitionSizeKb);
			HLIB_CMD_Printf("	nPvrIdx : [%d]\n", pstInfo->astPartitionInfos[i].nPvrIdx);
			HLIB_CMD_Printf("	nPlaybackIdx : [%d]\n", pstInfo->astPartitionInfos[i].nPlaybackIdx);
			HLIB_CMD_Printf("	szMountPath : [%s]\n", pstInfo->astPartitionInfos[i].szMountPath);
			HLIB_CMD_Printf("	szTsrPath : [%s]\n", pstInfo->astPartitionInfos[i].szTsrPath);
			HLIB_CMD_Printf("	szVolumeName : [%s]\n", pstInfo->astPartitionInfos[i].szVolumeName);
			HLIB_CMD_Printf("	szRecordPath : [%s]\n", pstInfo->astPartitionInfos[i].szRecordPath);
			HLIB_CMD_Printf("	szMediaPath : [%s]\n", pstInfo->astPartitionInfos[i].szMediaPath);
			HLIB_CMD_Printf("	szMediaVideo : [%s]\n", pstInfo->astPartitionInfos[i].szMediaVideo);
			HLIB_CMD_Printf("	szMediaAudio : [%s]\n", pstInfo->astPartitionInfos[i].szMediaAudio);
			HLIB_CMD_Printf("	szMediaPic : [%s]\n", pstInfo->astPartitionInfos[i].szMediaPic);
			HLIB_CMD_Printf("	szMediaDownload : [%s]\n", pstInfo->astPartitionInfos[i].szMediaDownload);
			HLIB_CMD_Printf("	ulStartUnit : [%d], ulEndUnit : [%d]\n", pstInfo->astPartitionInfos[i].ulStartUnit, pstInfo->astPartitionInfos[i].ulEndUnit);
			HLIB_CMD_Printf("	ulFlags : [0x%x]\n", pstInfo->astPartitionInfos[i].ulFlags);
			HLIB_CMD_Printf("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_MOUNTED));
			HLIB_CMD_Printf("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
			HLIB_CMD_Printf("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SECURED));
			HLIB_CMD_Printf("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
			HLIB_CMD_Printf("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
			HLIB_CMD_Printf("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));
			HLIB_CMD_Printf("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE));
		}
		HLIB_CMD_Printf("===========================================\n");

	}
	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	return 0;
}

HINT32 apk_filemgr_storage_cmdCheckUsbSpeed(void *pvArg)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 		ulDevNum;
	HUINT32 		ulIdx;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);
	HxLOG_Warning("[%s] [%d] Storage num : %d\n",__FUNCTION__,__LINE__, ulDevNum);

	for (ulIdx = 0 ; ulIdx < ulDevNum ; ulIdx++)
	{
		pstInfo = &pstInfoList[ulIdx];

		if (pstInfo->eDevType == eDxSTORAGE_DEVTYPE_USBMEM)
		{
			HLIB_CMD_Printf("deviceId : %d, path : %s\n", pstInfo->ulDeviceId, pstInfo->szMountPath);
			APK_FILEMGR_STORAGE_CheckUsbSpeed(pstInfo->szUUID);
			break;
		}
	}

	return 0;
}

void	apk_filemgr_storage_InitCommand(void)
{
#define	hCmdHandle		"APPKIT"
	HLIB_CMD_RegisterWord (hCmdHandle, apk_filemgr_storage_cmdGetHddSize, "gethddsize", "get storage size", "gethddsize");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_filemgr_storage_cmdShowHddList, "showhddlist", "show hdd size", "showhddlist");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_filemgr_storage_cmdFormat, "format", "Format storage", "format [ulDeviceId szName]");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_filemgr_storage_cmdCheckUsbSpeed, "checkusbspeed", "Check USB Speed", "checkusbspeed");
}
#endif


#define ________GLOBAL________________
HERROR	APK_FILEMGR_STORAGE_Init(void)
{
#ifdef CONFIG_DEBUG
	apk_filemgr_storage_InitCommand();
#endif

	return UAPI_FS_Init();
}

void	APK_FILEMGR_STORAGE_RegisterNotifier(DxStorageNotifier_t pfnObjectHandler)
{
	apkFilemgrStorage_Ctx_t* pstCtx;
	pstCtx = apk_filemgr_storage_GetContext();

	pstCtx->pfNotifier = pfnObjectHandler;

	UAPI_FS_SetNotifier(apk_filemgr_storage_Notifier);
}

const HCHAR* APK_FILEMGR_STORAGE_GetFsString(const DxStorage_FsType_e	eFsType)
{
	return UAPI_FS_GetFsString(eFsType);
}

const HCHAR* APK_FILEMGR_STORAGE_GetDevTypeString(const DxStorage_DevType_e	eDevType)
{
	return UAPI_FS_GetDevTypeString(eDevType);
}


HERROR APK_FILEMGR_STORAGE_GetSize(HUINT32 ulDeviceId, HUINT64 *pullTotalKbSize, HUINT64 *pullAvailableKbSize, HUINT64 *pullUsedKbSize, HUINT64 *pullReservedKbSize, HUINT64 *pullAvailablePvrKbSize)
{
	return UAPI_FS_GetSize(ulDeviceId, pullTotalKbSize, pullAvailableKbSize, pullUsedKbSize, pullReservedKbSize, pullAvailablePvrKbSize);
}

HERROR APK_FILEMGR_STORAGE_GetPvrReservedSize(HUINT32 ulDeviceId, HUINT64 *pullReservedKbSize)
{
	return UAPI_FS_GetPvrReservedSize(ulDeviceId, pullReservedKbSize);
}

HERROR	APK_FILEMGR_STORAGE_GetList(HUINT32 ulFilter, DxStorage_Info_t **pastInfoList, HUINT32 *pulDevNum)
{
	return UAPI_FS_GetList(ulFilter, pastInfoList, pulDevNum);
}

HERROR	APK_FILEMGR_STORAGE_FreeList(DxStorage_Info_t *pastInfoList, HUINT32 ulDevNum)
{
	return UAPI_FS_FreeList(pastInfoList, ulDevNum);
}

HERROR	APK_FILEMGR_STORAGE_Format(HUINT32 ulDeviceId, const HCHAR *pszVolume)
{
	return UAPI_FS_Format(ulDeviceId, pszVolume);
}


HERROR	APK_FILEMGR_STORAGE_StartDST(HUINT32 ulDeviceId)
{
	return UAPI_FS_Dst(ulDeviceId);
}

HERROR	APK_FILEMGR_STORAGE_SafeRemove(HUINT32 ulDeviceId, HBOOL bForced)
{
	return UAPI_FS_SafeRemove(ulDeviceId, bForced);
}

HERROR	APK_FILEMGR_STORAGE_SetDefaultHdd(const HCHAR *pszStorageId)
{
	return UAPI_FS_SetDefaultHdd(pszStorageId);
}

HERROR	APK_FILEMGR_STORAGE_DisablePVRHdd(const HCHAR *pszStorageId)
{
	return UAPI_FS_DisableHdd(pszStorageId);
}

HERROR	APK_FILEMGR_STORAGE_PairingHdd(const HCHAR *pszStorageId)
{
	return UAPI_FS_Pairing(pszStorageId);
}

HERROR	APK_FILEMGR_STORAGE_UnPairingHdd(const HCHAR *pszStorageId)
{
	return UAPI_FS_UnPairing(pszStorageId);
}

HERROR	APK_FILEMGR_STORAGE_SetName(const HCHAR *pszStorageId, const HCHAR *pszName)
{
	return UAPI_FS_SetName(pszStorageId, pszName);
}

HERROR	APK_FILEMGR_STORAGE_CheckUsbSpeed(const HCHAR *pszStorageId)
{
	return UAPI_FS_CheckUsbSpeed(pszStorageId);
}

