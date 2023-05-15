/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  uapi.c
	@brief

	Description:  									\n
	Module: Umma API Module			\n

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
#include	<hlib.h>
#include	<vkernel.h>

#include	<uapi.h>
#include	<umma_int.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HxList_t *s_pstBrowseHndList = NULL;
static HUINT32	s_ulHndListSemId = 0;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
static HERROR	uapi_fm_browse_onResult(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
static HERROR	uapi_fm_group_onUpdated(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);

static HERROR	uapi_fm_event_AddEventListener(UAPI_t *uapi, HINT32 nRequestId, void *pvUser, UAPI_Noti_t onEventOccurred)
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

static HERROR	uapi_fm_onStatusChange(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UAPI_t	*pstUapi = (UAPI_t*)pvUserData;
	UAPI_CBINFO_t	*pstCbInfo;

	UAPI_STATUS_t	stStatus;
	HINT32			_nRequestId;

	UAPI_Noti_t			 pfnEventCallback = NULL;
	void				*pvUser = NULL;
	

	if (nArgc != 2)
	{
		HxLOG_Error("nArgc is not valid [%d:2]\n", nArgc);
		return ERR_FAIL;
	}

	HxSEMT_Get(pstUapi->ulSemId);

	_nRequestId = HxOBJECT_INT(apArgv[0]);
	pstCbInfo = HLIB_HASH_Lookup(pstUapi->pstCallbackTable, (void*)_nRequestId);
	if (pstCbInfo)
	{
		HxSTD_MemCopy(&stStatus, HxOBJECT_BIN_DATA(apArgv[1]), HxOBJECT_BIN_SIZE(apArgv[1]));

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

static HERROR	uapi_fm_onItemChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UAPI_t		*uapi = (UAPI_t *)pvUserData;
	HINT32		nItemAddRemove;
	HCHAR		*pszName;
	HxList_t	*list;

	UAPI_CBINFO_t	*pstCbInfo;

	if (nArgc != 2)
	{
		HxLOG_Error("nArgc is not valid [%d:2]\n", nArgc);
		return ERR_FAIL;
	}

	nItemAddRemove = HxOBJECT_INT(apArgv[0]);
	pszName = HxOBJECT_STR(apArgv[1]);

	list = uapi->pstMonitorTable;
	while (list)
	{
		pstCbInfo = (UAPI_CBINFO_t*)HLIB_LIST_Data(list);
		pstCbInfo->cb.pfnEventFunction(nItemAddRemove, pszName, pstCbInfo->pvUser);

		list = list->next;
	}

	return ERR_OK;
}

static UAPI_t*	uapi_fm_getInstance(void)
{
	static UAPI_t	*s_uapi = NULL;

	if (s_uapi == NULL)
	{
		HERROR	hErr;

		s_uapi = (UAPI_t*)UAPI_Calloc(sizeof(UAPI_t));
		if (s_uapi == NULL)
		{
			HxLOG_Error("Calloc Fail!!! \n");
			return NULL;
		}

		HxSEMT_Create(&s_uapi->ulSemId, "uapisem", 0);
		HxSEMT_Get(s_uapi->ulSemId);
		HxSEMT_Create(&s_ulHndListSemId, "uapihndsem", 0);

		hErr = UAPI_RpcProtocol_GetRpcHandle(&s_uapi->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error !! (GetRpcHandle)\n");
			HxLOG_Assert(s_uapi->nRpcHandle);
		}

		s_uapi->pstCallbackTable = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, UAPI_Free_CB);

		//	Register UMMA Event Notifier
		HLIB_RPC_RegisterNotifyCallback(s_uapi->nRpcHandle, RPC_UMMA_FM_onStatusChange,   "ib", (void*)s_uapi, uapi_fm_onStatusChange);
		HLIB_RPC_RegisterNotifyCallback(s_uapi->nRpcHandle, RPC_UMMA_FM_BROWSE_onResult,   "ibb", (void*)s_uapi, uapi_fm_browse_onResult);
		HLIB_RPC_RegisterNotifyCallback(s_uapi->nRpcHandle, RPC_UMMA_FM_GROUP_onUpdate,   "iib", (void*)s_uapi, uapi_fm_group_onUpdated);

	}
	else
	{
		HxSEMT_Get(s_uapi->ulSemId);
	}

	return s_uapi;
}

static void		uapi_fm_releaseInstance(UAPI_t *uapi)
{
	if ( uapi )
		HxSEMT_Release(uapi->ulSemId);
}

static HERROR		uapi_fm_GetMountedPath(HUINT32 ulCurIdx, HINT8 *szReturnPath, HINT8 *szMountPath)
{
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum = 0;
	HUINT32 ulIdx = 0, i = 0;

	UAPI_FS_GetList(eDxSTORAGE_FILTER_Mem, &pstInfoList, &ulDevNum);
	HxLOG_Error("eDxSTORAGE_FILTER_Mem Storage num : %d\n", ulDevNum);

	ulIdx = ulCurIdx;
	for (ulIdx ; ulIdx < ulDevNum ; ulIdx++)
	{
		pstInfo = &pstInfoList[ulIdx];

		HxLOG_Error("================ Storage Info ================ \n");
		HxLOG_Debug("ulDeviceId : [%d]\n", pstInfo->ulDeviceId);
		HxLOG_Error("DevType : [%s]\n", UAPI_FS_GetDevTypeString(pstInfo->eDevType));
		HxLOG_Error("szMountPath : [%s]\n", pstInfo->szMountPath);
		HxLOG_Debug("szDevName : [%s] \n", pstInfo->szDevName);
		HxLOG_Error("ullTotalSizeKb : [%lld] \n", pstInfo->ullTotalSizeKb);
		HxLOG_Error("szLabel : [%s]\n", pstInfo->szLabel);

		HLIB_CMD_Printf("\n--------Partition Info--------\n");
		for (i=0; i < pstInfo->nPartitionNum ; i++)
		{
			HxLOG_Debug("\n   [%d] \n",i);
			HxLOG_Debug("	bAvailable : [%d]\n", pstInfo->astPartitionInfos[i].bAvailable);
			HxLOG_Error("	FsType: [%s]\n", UAPI_FS_GetFsString(pstInfo->astPartitionInfos[i].eFsType));
			HxLOG_Error("	ullPartitionSizeKb : [%lld]\n", pstInfo->astPartitionInfos[i].ullPartitionSizeKb);
			HxLOG_Debug("	szMountPath : [%s]\n", pstInfo->astPartitionInfos[i].szMountPath);
			HxLOG_Error("	szVolumeName : [%s]\n", pstInfo->astPartitionInfos[i].szVolumeName);
		}
		HxLOG_Error("===========================================\n\n\n");

		HxSTD_MemSet(szReturnPath, 0x00, 2048);
		HxSTD_snprintf(szReturnPath, 2048, "%s%s", pstInfo->szMountPath, szMountPath);
		HxLOG_Error("szReturnPath = [%s] \n", szReturnPath);
		if (UAPI_FM_URLFILEExist((HCHAR *)szReturnPath) == TRUE)
		{
			HxLOG_Error("[%s] exists !!!\n", szReturnPath);
			return ERR_OK;
		}
	}
	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	return ERR_FAIL;
}

void	UAPI_FM_Init(void)
{
	uapi_fm_releaseInstance(uapi_fm_getInstance());
}

HERROR	UAPI_FM_SetGlobalListener(void *pvUser, UAPI_Func_t onStateChanged)
{
	UAPI_t	*uapi = NULL;
	HERROR	err = ERR_FAIL;
	UAPI_CBINFO_t	*pstCbInfo;

	HxLOG_Assert(onStateChanged);

	uapi = uapi_fm_getInstance();

	if (uapi)
	{
		if (uapi->pstMonitorTable == NULL) {
			HLIB_RPC_RegisterNotifyCallback(uapi->nRpcHandle, RPC_UMMA_FM_onItemChanged, "is", (void*)uapi, uapi_fm_onItemChanged);
		}

		pstCbInfo = (UAPI_CBINFO_t*)UMMA_Calloc(sizeof(UAPI_CBINFO_t));
		pstCbInfo->pvUser = pvUser;
		pstCbInfo->cb.pfnEventFunction = onStateChanged;

		uapi->pstMonitorTable = HLIB_LIST_Append(uapi->pstMonitorTable, (void*)pstCbInfo);

		uapi_fm_releaseInstance(uapi);
	}

	return err;
}

HERROR	UAPI_FM_AddListener(HINT32 nRequestId, void *pvUser, UAPI_Noti_t onEventOccurred)
{
	UAPI_t	*uapi = NULL;
	HERROR	err = ERR_FAIL;

	HxLOG_Assert(onEventOccurred);

	uapi = uapi_fm_getInstance();
	if (uapi) {
		err = uapi_fm_event_AddEventListener(uapi, nRequestId, pvUser, onEventOccurred);
		uapi_fm_releaseInstance(uapi);
	}

	return err;
}

//	Return Value : Token
//	pszScanDir   : /mnt/hd2 --> find hd2 file or directory in /mnt folder
//	pszScanDir   : /mnt/hd2/ --> find all files in /mnt/hd2/ folder
//	pszScanDir   : /mnt/hd2/* --> find all files in /mnt/hd2/ folder
//	onScanStatusChange : notify callback if scan status is changed. <-- auto remove when scan is finished.
HINT32	UAPI_FM_Scan(const HCHAR *pszScanDir, const HCHAR *pszExtFilter, HBOOL bRecursive, void *pvUser, UAPI_Noti_t onScanStatusChange)
{
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_Scan, "ssi", pszScanDir, pszExtFilter, bRecursive);
		if (onScanStatusChange){
			uapi_fm_event_AddEventListener(uapi, nReqId, pvUser, onScanStatusChange);
		}

		uapi_fm_releaseInstance(uapi);
	}

	return nReqId;
}

//	Return Value : Token
//	pszSource    : /mnt/hd2  --> remove /mnt/hd2 file or folder
//	pszSource    : /mnt/hd2/ --> remove /mnt/hd2/* files or folder
//	pszSource    : /mnt/hd2/* --> same with /mnt/hd2/
//	pszSource    : /mnt/hd2/*.avi --> remove file or folder in /mnt/hd2/* matched with *.avi
HINT32	UAPI_FM_Remove(const HCHAR *pszSource, const HCHAR *pszExtFilter, void *pvUser, UAPI_Noti_t onRemoveStatusChange)
{
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_Remove, "ss", pszSource, pszExtFilter);
		if (onRemoveStatusChange) {
			uapi_fm_event_AddEventListener(uapi, nReqId, pvUser, onRemoveStatusChange);
		}
		uapi_fm_releaseInstance(uapi);
	}

	return nReqId;
}

//	Return Value : Token
//	pszSource    : if pszSource is Directory, pszDest should be Directory.
//	pszSource    : if pszSource is files(filtered?), pszDest should be Directory
//	pszSource    : if pszSource is file, pszDest can be file or directory.
//	ex > src - /mnt/hd2/* (/mnt/hd2/) dst should be directory. (/mnt/hd2/test, if not, make directory with test)
//	     src - /mnt/hd2/a.ts, dst /mnt/hd2/b.ts, /mnt/hd2
HINT32	UAPI_FM_Move(const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter, HINT32 nFlag, void *pvUser, UAPI_Noti_t onMoveStatusChange)
{
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_Move, "sssi", pszSource, pszDest, pszExtFilter, nFlag);
		if (onMoveStatusChange){
			uapi_fm_event_AddEventListener(uapi, nReqId, pvUser, onMoveStatusChange);
		}
		uapi_fm_releaseInstance(uapi);
	}

	return nReqId;
}

HINT32	UAPI_FM_Copy(const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter, HINT32 nFlag, void *pvUser, UAPI_Noti_t onCopyStatusChange)
{
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_Copy, "sssi", pszSource, pszDest, pszExtFilter, nFlag);
		if (onCopyStatusChange) {
			uapi_fm_event_AddEventListener(uapi, nReqId, pvUser, onCopyStatusChange);
		}

		uapi_fm_releaseInstance(uapi);
	}

	return nReqId;
}

HINT32	UAPI_FM_Cancel(HINT32 nJobId, void *pvUser, UAPI_Noti_t onCancelStatusChange)
{
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_Cancel, "i", nJobId);
		if (onCancelStatusChange) {
			uapi_fm_event_AddEventListener(uapi, nReqId, pvUser, onCancelStatusChange);
		}
		uapi_fm_releaseInstance(uapi);
	}

	return nReqId;
}

HINT32	UAPI_FM_MkDir(const HCHAR *pszDir, void *pvUser, UAPI_Noti_t onMkDirStatusChange)
{
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_Mkdir, "s", pszDir);
		if (onMkDirStatusChange) {
			uapi_fm_event_AddEventListener(uapi, nReqId, pvUser, onMkDirStatusChange);
		}

		uapi_fm_releaseInstance(uapi);
	}

	return nReqId;
}

HINT32	UAPI_FM_RmDir(const HCHAR *pszDir, void *pvUser, UAPI_Noti_t onRmDirStatusChange)
{
	return UAPI_FM_Remove(pszDir, NULL, pvUser, onRmDirStatusChange);
}

void	UAPI_FM_Flush(void)
{
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		(void)HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_Flush, "i", TRUE);
		uapi_fm_releaseInstance(uapi);
	}
}

/*
	Will be moved to System DB.
*/
HCHAR*	UAPI_FM_GetDefaultURL(HCHAR *pszDefaultURL, HCHAR *pszURLFileName)
{
	static HCHAR	s_szURLName[2048];
	FILE	*fUrlFile;
	static HCHAR	szDefaultURLName[2048];
	HERROR	err = ERR_FAIL;

	HxSTD_MemSet(szDefaultURLName, 0x00, 2048);

#ifdef CONFIG_DEBUG
// 개발시, USB 메모리 스틱을 꼽지 않고도 원하는 url 에 접근 가능하도록 추가.
// 아래 경로에 url 경로를 넣으면 됨.
// /tmp/VDIR/url/url_hbbtv.txt
#define VIRTUAL_PORTAL_DIR "/tmp/VDIR"
	if(HLIB_DIR_IsExist(VIRTUAL_PORTAL_DIR))
	{
		HxSTD_snprintf(szDefaultURLName, 2048, "%s%s", VIRTUAL_PORTAL_DIR, pszURLFileName);
	}
	else
#endif
	{
		err = uapi_fm_GetMountedPath(0, szDefaultURLName, pszURLFileName);
		if (err == ERR_FAIL)
		{
			HxLOG_Error("NOT EXIST!!! pszDefaultURL = [%s] \n", pszDefaultURL);
			return pszDefaultURL;
		}
	}

	fUrlFile = fopen(szDefaultURLName, "r");
	if (fUrlFile)
	{
		fgets(s_szURLName, 2048, fUrlFile);
		s_szURLName[2047] = 0;
		fclose(fUrlFile);
		if (HxSTD_StrLen(s_szURLName) > 0)
			return s_szURLName;
	}

	return pszDefaultURL;
}

HCHAR*	UAPI_FM_GetInnerURL(HCHAR *pszURLFileName)
{
	static HCHAR	s_szURLName[128];
	FILE	*fUrlFile;
	fUrlFile = fopen(pszURLFileName, "r");
	if (fUrlFile)
	{
		fgets(s_szURLName, 128, fUrlFile);
		s_szURLName[127] = 0;
		fclose(fUrlFile);
		if (HxSTD_StrLen(s_szURLName) > 0)
			return s_szURLName;
	}
	else
	{
		return NULL;
	}

	return s_szURLName;
}

HERROR   UAPI_FM_SetInnerURL(HCHAR *pszURL, HCHAR *pszDestFileName)
{
	HERROR hErr = ERR_FAIL;

	FILE *fDstFile;
	HCHAR	s_szURLName[128];

	fDstFile = fopen(pszDestFileName, "wt");
	if (fDstFile)
	{
		HxSTD_StrNCpy(s_szURLName, pszURL, 128);
		s_szURLName[127] = 0;
		fwrite(s_szURLName, 1, 128, fDstFile);
		fclose(fDstFile);
		hErr = ERR_OK;
	}
	return hErr;
}


HBOOL    UAPI_FM_URLFILEExist(HCHAR *pszURLFileName)
{
  HBOOL bExist = FALSE;

  bExist  =  HLIB_FILE_Exist(pszURLFileName);

  return bExist;
}

HERROR   UAPI_FM_INNERURLRemove(HCHAR *InnerURLFileName)
{
	HERROR hErr = ERR_FAIL;

	hErr = HLIB_FILE_Delete(InnerURLFileName);

	return hErr;
}

HCHAR*	UAPI_FM_GetFILEPath(HCHAR *pszFileName)
{
	static HCHAR	szDefaultName[2048];
	HERROR	err = ERR_FAIL;

	HxSTD_MemSet(szDefaultName, 0x00, 2048);

	err = uapi_fm_GetMountedPath(0, szDefaultName, pszFileName);
	if (err == ERR_FAIL)
	{
		HxLOG_Error("NOT EXIST!!! pszFileName = [%s] \n", szDefaultName);
		return NULL;
	}

	return szDefaultName;
}

/*
	UMMA Test Function.
*/
HINT32	UAPI_FM_TEST_MountDevice(const HCHAR *pszMountPath, const HCHAR *pszUUID)
{
	UAPI_t	*uapi = NULL;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, "umma.test.mount", "ss", pszMountPath, pszUUID);
		uapi_fm_releaseInstance(uapi);
	}

	return 0;
}

/*
	UMMA File Browse Function
*/

static HERROR	uapi_fm_browse_getHandleIdCb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_MSG_t *pData = (UMMA_MSG_t *)pvUserData;
	HINT32 nHndId = 0;
	if (nArgc != 1)
	{
		HxLOG_Error("nArgc is not valid [%d:1]\n", nArgc);
		return ERR_FAIL;
	}

	nHndId = HxOBJECT_INT(apArgv[0]);
	pData->nParam1 = nHndId;
	HxLOG_Print("[UAPI_FM_BROWSE] GetHandle(%d), RequsetId(%d)\n", nHndId, nRequestId);
	return ERR_OK;
}

static inline void uapi_fm_browse_handleList_add( HUINT32 nHndId )
{
	HxSEMT_Get(s_ulHndListSemId);
	if ( nHndId )
		s_pstBrowseHndList = HLIB_LIST_Append( s_pstBrowseHndList, (void *)nHndId );
	HxSEMT_Release(s_ulHndListSemId);
}

static inline void uapi_fm_browse_handleList_remove( HUINT32 nHndId )
{
	HxSEMT_Get(s_ulHndListSemId);
	s_pstBrowseHndList = HLIB_LIST_Remove( s_pstBrowseHndList, (const void *)nHndId );
	HxSEMT_Release(s_ulHndListSemId);
}

static inline HBOOL	uapi_fm_browse_handleList_checkId( HUINT32 nHandleId )
{
	HxList_t *pHndItem = NULL;

	HxSEMT_Get(s_ulHndListSemId);
	pHndItem = HLIB_LIST_Find( s_pstBrowseHndList, (const void *) nHandleId );

	HxSEMT_Release(s_ulHndListSemId);

	if (pHndItem)
		return TRUE;

	HxLOG_Error( "[UAPI_FM_BROWSE] Not have handle Id : %d !!\n", nHandleId );
	return FALSE;
}

static HERROR	uapi_fm_browse_onResult(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UAPI_t	*pstUapi = (UAPI_t*)pvUserData;
	UAPI_CBINFO_t	*pstCbInfo;

	UAPI_BROWSE_RESULT_t	*pResult=NULL;
	HUINT32 	nHndId = 0;

	UAPI_NotiBrowse_t	 pfnBrowseCallback = NULL;

	HxLOG_Debug("[UAPI_FM_BROWSE] uapi_fm_browse_onResult(), reqId(%d)\n", nRequestId);

	if (nArgc != 3)
	{
		HxLOG_Error("[UAPI_FM_BROWSE] OnResult -  nArgc is not valid [%d:3]\n", nArgc);
		return ERR_FAIL;
	}

	HxSEMT_Get(pstUapi->ulSemId);

	pstCbInfo = HLIB_HASH_Lookup(pstUapi->pstCallbackTable, (void*)nRequestId);
	if (pstCbInfo)
	{
		pResult = (UAPI_BROWSE_RESULT_t*)HLIB_MEM_Calloc(sizeof(UAPI_BROWSE_RESULT_t) + HxOBJECT_BIN_SIZE(apArgv[2]));
		HxSTD_MemCopy(pResult, HxOBJECT_BIN_DATA(apArgv[1]), HxOBJECT_BIN_SIZE(apArgv[1]));
		if ( pResult->ulErr )
		{
			HLIB_MEM_Free(pResult);
			HxSEMT_Release(pstUapi->ulSemId);
			return ERR_FAIL;
		}
		HxSTD_MemCopy(pResult->Itemdata, HxOBJECT_BIN_DATA(apArgv[2]),pResult->nItemSize );
		nHndId = pResult->nHandleId;

		pfnBrowseCallback = pstCbInfo->cb.pfnBrowseCallback;
		
		if ( eUAPI_FM_BROWSE_CLOSE == pResult->eEvent && ERR_OK == pResult->ulErr )
		{
			HLIB_HASH_Remove(pstUapi->pstCallbackTable, (void*)nRequestId);
			uapi_fm_browse_handleList_remove( nHndId );
			HxLOG_Debug("[UAPI_FM_BROWSE] Remove registered function : HandleId[%d]\n", nHndId);
		}
	}

	HxSEMT_Release(pstUapi->ulSemId);

	HxLOG_Debug("[UAPI_FM_BROWSE] Call(%d) registered function : [%d]\n", nHndId, nRequestId);
	if (NULL != pfnBrowseCallback && NULL != pResult)
	{
		pfnBrowseCallback(nHndId, nRequestId, pResult);
	}

	if(pResult)
	{
		HLIB_MEM_Free(pResult);
	}

	return ERR_OK;
}

HUINT32	UAPI_FM_BROWSE_Open( const HCHAR *pszPath, HINT32 nCategory, const HCHAR *pszExtFilter, HINT32 nOptFlag, UAPI_NotiBrowse_t onBrowseResult)
{
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;
	HUINT32	nHndId = 0;
	UMMA_MSG_t *pUserData = NULL;

	pUserData =  (UMMA_MSG_t *)HLIB_MEM_Calloc(sizeof(UMMA_MSG_t));
	if(pUserData == NULL)
	{
		HxLOG_Error("Error, Allocation Fail !!!\n");
		return 0;
	}

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, uapi_fm_browse_getHandleIdCb, pUserData,
					RPC_UAPI_FM_BROWSE_OPEN, "sisi", pszPath, nCategory, pszExtFilter, nOptFlag);
		if ( -1 == nReqId )
		{
			uapi_fm_releaseInstance(uapi);
			HxLOG_Error("Error, RPC Request Fail !!!\n");
			HLIB_MEM_Free(pUserData);
			return 0;
		}

		if ( onBrowseResult)
		{
			uapi_fm_event_AddEventListener(uapi, nReqId, (void *)nHndId, (UAPI_Noti_t)onBrowseResult);
		}
		uapi_fm_releaseInstance(uapi);
		if(HLIB_RPC_Join (uapi->nRpcHandle, nReqId, 1000) == ERR_FAIL)
		{
			HxLOG_Error("Error, RPC Timeout !!!\n");
			HLIB_MEM_Free(pUserData);
			return 0;
		}
		nHndId = (HUINT32)pUserData->nParam1;
	}

	HxLOG_Print( "[UAPI_BROWSE_OPEN] nHandleId(%ld), nRequestId(%d)\n", nHndId, nReqId );
	uapi_fm_browse_handleList_add( nHndId );

	HLIB_MEM_Free(pUserData);

	return nHndId;
}

HERROR	UAPI_FM_BROWSE_Close( HUINT32 nHandleId )
{
	UAPI_t	*uapi = NULL;

	if ( TRUE != uapi_fm_browse_handleList_checkId( nHandleId ) )
		return ERR_FAIL;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_BROWSE_CLOSE, "i", nHandleId );
		uapi_fm_releaseInstance(uapi);
	}
	HxLOG_Print( "[UAPI_BROWSE_CLOSE] nHandleId(%ld)\n", nHandleId );
	return ERR_OK;
}

HERROR	UAPI_FM_BROWSE_GetItems( HUINT32 nHandleId, HUINT32 nIndex, HUINT32 nReqCount, HINT32 nOption )
{
	UAPI_t	*uapi = NULL;

	if ( TRUE != uapi_fm_browse_handleList_checkId( nHandleId ) )
		return ERR_FAIL;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_BROWSE_GETITEMS, "iiii", nHandleId, nIndex, nReqCount, nOption );
		uapi_fm_releaseInstance(uapi);
	}
	return ERR_OK;
}

HERROR	UAPI_FM_BROWSE_ScanItems( HUINT32 nHandleId, HUINT32 nReqCount, HINT32 nOption )
{
	UAPI_t	*uapi = NULL;

	if ( TRUE != uapi_fm_browse_handleList_checkId( nHandleId ) )
		return ERR_FAIL;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_BROWSE_SCANITEMS, "iii", nHandleId, nReqCount, nOption );
		uapi_fm_releaseInstance(uapi);
	}
	return ERR_OK;
}

HERROR	UAPI_FM_BROWSE_Sort( HUINT32 nHandleId, HINT32 nSortType )
{
	UAPI_t	*uapi = NULL;

	if ( TRUE != uapi_fm_browse_handleList_checkId( nHandleId ) )
		return ERR_FAIL;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_BROWSE_SORT, "ii", nHandleId, nSortType );
		uapi_fm_releaseInstance(uapi);
	}
	return ERR_OK;
}

static HERROR	uapi_fm_group_getHandleIdCb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_MSG_t *pData = (UMMA_MSG_t *)pvUserData;
	HUINT32 nHndId = 0;
	if (nArgc != 1)
	{
		HxLOG_Error("nArgc is not valid [%d:1]\n", nArgc);
		return ERR_FAIL;
	}

	nHndId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	pData->nParam1 = (HINT32)nHndId;
	HxLOG_Print("[UAPI_FM_BROWSE] GetHandle(%d), RequsetId(%d)\n", nHndId, nRequestId);
	return ERR_OK;
}

static inline void uapi_fm_group_handleList_add( HUINT32 nHndId )
{
	HxSEMT_Get(s_ulHndListSemId);
	if ( nHndId )
		s_pstBrowseHndList = HLIB_LIST_Append( s_pstBrowseHndList, (void *)nHndId );
	HxSEMT_Release(s_ulHndListSemId);
}

static inline void uapi_fm_group_handleList_remove( HUINT32 nHndId )
{
	HxSEMT_Get(s_ulHndListSemId);
	s_pstBrowseHndList = HLIB_LIST_Remove( s_pstBrowseHndList, (const void *)nHndId );
	HxSEMT_Release(s_ulHndListSemId);
}

static inline HBOOL	uapi_fm_group_handleList_checkId( HUINT32 nHandleId )
{
	HxList_t *pHndItem = NULL;

	HxSEMT_Get(s_ulHndListSemId);
	pHndItem = HLIB_LIST_Find( s_pstBrowseHndList, (const void *) nHandleId );
	HxSEMT_Release(s_ulHndListSemId);

	if (pHndItem)
		return TRUE;

	HxLOG_Error( "[UAPI_FM_BROWSE] Not have handle Id : %d !!\n", nHandleId );
	return FALSE;
}

static HERROR	uapi_fm_group_onUpdated(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Debug("[UAPI_FM_GROUP] uapi_fm_group_onUpdated(), reqId(%d)\n", nRequestId);

	if (nArgc != 3)
	{
		HxLOG_Error("[UAPI_FM_GROUP] OnResult -  nArgc is not valid [%d:3]\n", nArgc);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HUINT32	UAPI_FM_GROUP_Create( const HCHAR *pszExtFilter, UAPI_NotiGROUP_t onGroupUpdated )
{
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;
	HUINT32	nHndId = 0;
	UMMA_MSG_t *pUserData = NULL;

	pUserData =  (UMMA_MSG_t *)HLIB_MEM_Calloc(sizeof(UMMA_MSG_t));
	if(pUserData == NULL)
	{
		HxLOG_Error("Error, Allocation Fail !!!\n");
		return 0;
	}

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, uapi_fm_group_getHandleIdCb, pUserData,
					RPC_UAPI_FM_GROUP_Create, "s", pszExtFilter );
		if ( -1 == nReqId )
		{
			HxLOG_Error("Error, RPC Request Fail !!!\n");
			uapi_fm_releaseInstance(uapi);
			HLIB_MEM_Free(pUserData);
			return 0;
		}

		if ( 0 != nReqId && onGroupUpdated)
		{
			uapi_fm_event_AddEventListener(uapi, nReqId, (void *)nHndId, (UAPI_Noti_t)onGroupUpdated);
		}
		uapi_fm_releaseInstance(uapi);

		if(HLIB_RPC_Join (uapi->nRpcHandle, nReqId, 3000) == ERR_FAIL)
		{
			HxLOG_Error("Error, RPC Timeout !!!\n");
			HLIB_MEM_Free(pUserData);
			return 0;
		}
		nHndId = (HUINT32)pUserData->nParam1;
	}

	HxLOG_Print( "[UAPI_GROUP_CREATE] nHandleId(%ld)\n", nHndId );
	uapi_fm_group_handleList_add( nHndId );
	HLIB_MEM_Free(pUserData);

	return nHndId;
}

HERROR	UAPI_FM_GROUP_Release( HUINT32 nHandleId  )
{
	UAPI_t	*uapi = NULL;

	if ( TRUE != uapi_fm_group_handleList_checkId( nHandleId ) )
		return ERR_FAIL;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_GROUP_Release, "i", nHandleId );
		uapi_fm_releaseInstance(uapi);
	}
	HxLOG_Print( "[UAPI_GROUP_RELEASE] nHandleId(%ld)\n", nHandleId );
	return ERR_OK;
}

HERROR	UAPI_FM_GROUP_SetPath( HUINT32 nHandleId, UAPI_FM_GROUP_SETPATH_e ePathOper, const HCHAR *pszPath  )
{
	UAPI_t	*uapi = NULL;

	if ( TRUE != uapi_fm_group_handleList_checkId( nHandleId ) )
		return ERR_FAIL;

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_GROUP_SetPath, "iis",
					nHandleId, (HINT32)ePathOper, pszPath );
		uapi_fm_releaseInstance(uapi);
	}
	return ERR_OK;
}

HINT32	UAPI_FM_GROUP_Operation( HUINT32 nHandleId, UAPI_FM_GROUP_OPER_e eOper, const HCHAR *pszPath )
{
	UAPI_t	*uapi = NULL;
	HINT32	nReqId = 0;

	if ( TRUE != uapi_fm_group_handleList_checkId( nHandleId ) )
		return ERR_FAIL;

	if ( !pszPath )
		pszPath = "";

	uapi = uapi_fm_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_FM_GROUP_Operation, "iis",
				nHandleId, (HINT32)eOper, pszPath );
		uapi_fm_releaseInstance(uapi);
	}
	return nReqId;
}

