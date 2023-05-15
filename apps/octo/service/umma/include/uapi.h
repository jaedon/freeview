/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  uapi.h
	@brief

	Description:  									\n
	Module: Umma API Module Header			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding ÏùÑ ÏÇ¨Ïö©ÌïòÎäî source file ÏûÖÎãàÎã§.
* MS949 encoding„Çí‰ΩøÁî®„Åó„Å¶ source file„Åß„Åô„ÄÇ
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

#ifndef __UAPI_H__
#define __UAPI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#ifdef	__cplusplus
extern "C" {
#endif

#include	<hlib.h>
#include <dlib_storage.h>
#include <dlib_fileinfo.h>
#include <dlib_hotplug.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define UAPI_Malloc					HLIB_STD_MemAlloc
#define UAPI_Calloc					HLIB_STD_MemCalloc
#define	UAPI_Realloc				HLIB_STD_MemRealloc
#define	UAPI_StrDup					HLIB_STD_StrDup
#define	UAPI_MemDup					HLIB_STD_MemDup
#define UAPI_Free					HLIB_STD_MemFree
#define	UAPI_Malloc_CB				HLIB_STD_MemAlloc_CB
#define	UAPI_Free_CB				HLIB_STD_MemFree_CB


// File Manager
#define RPC_UAPI_FM_Scan		"uapi.fm.scan"
#define RPC_UAPI_FM_Remove		"uapi.fm.remove"
#define RPC_UAPI_FM_Move		"uapi.fm.move"
#define RPC_UAPI_FM_Copy		"uapi.fm.copy"
#define RPC_UAPI_FM_Cancel		"uapi.fm.cancel"
#define RPC_UAPI_FM_Mkdir		"uapi.fm.mkdir"
#define RPC_UAPI_FM_Flush		"uapi.fm.flush"

#define RPC_UMMA_FM_onStatusChange				"umma.fm.onStatusChange"
#define RPC_UMMA_FM_onItemChanged				"umma.fm.onItemChanged"

#define RPC_UAPI_FM_BROWSE_OPEN					"uapi.fm.browse.open"
#define RPC_UAPI_FM_BROWSE_CLOSE				"uapi.fm.browse.close"
#define RPC_UAPI_FM_BROWSE_SORT					"uapi.fm.browse.sort"
#define RPC_UAPI_FM_BROWSE_GETITEMS				"uapi.fm.browse.getitems"
#define RPC_UAPI_FM_BROWSE_SCANITEMS			"uapi.fm.browse.scanitems"
#define RPC_UMMA_FM_BROWSE_onResult				"umma.fm.browse.onResult"

#define RPC_UAPI_FM_GROUP_Create				"uapi.fm.group.create"
#define RPC_UAPI_FM_GROUP_Release				"uapi.fm.group.release"
#define RPC_UAPI_FM_GROUP_Operation				"uapi.fm.group.operation"
#define RPC_UAPI_FM_GROUP_SetPath				"uapi.fm.group.setpath"
#define RPC_UMMA_FM_GROUP_onUpdate				"umma.fm.group.onUpdate"

// File System
#define	RPC_UAPI_FS_Init						"uapi.fs.Init"
#define	RPC_UAPI_FS_Format						"uapi.fs.format"
#define	RPC_UAPI_FS_Dst							"uapi.fs.dst"
#define	RPC_UAPI_FS_SetDefaultHdd				"uapi.fs.setdefault"
#define	RPC_UAPI_FS_SetName						"uapi.fs.setname"
#define	RPC_UAPI_FS_DisableHdd					"uapi.fs.disable"
#define	RPC_UAPI_FS_Pairing						"uapi.fs.pairing"
#define	RPC_UAPI_FS_UnPairing					"uapi.fs.unpairing"
#define	RPC_UAPI_FS_SafeRemove					"uapi.fs.saferemove"
#define	RPC_UAPI_FS_CheckUsbSpeed				"uapi.fs.checkusbspeed"

#define	RPC_UMMA_FS_onInit						"umma.fs.onInit"
#define	RPC_UMMA_FS_onDeviceConnected			"umma.fs.onDeviceConnected"
#define	RPC_UMMA_FS_onChangeDevice				"umma.fs.onChangeDevice"
#define	RPC_UMMA_FS_onNeedFormat				"umma.fs.onNeedFormat"
#define	RPC_UMMA_FS_onOverCurrent				"umma.fs.onOverCurrent"
#define	RPC_UMMA_FS_onFormatStateChange			"umma.fs.onFormatStateChange"
#define	RPC_UMMA_FS_onDSTStateChange			"umma.fs.onDSTStateChange"
#define	RPC_UMMA_FS_onPVRReady					"umma.fs.onPVRReady"
#define	RPC_UMMA_FS_onTSRReady					"umma.fs.onTSRReady"
#define	RPC_UMMA_FS_onPlaybackReady				"umma.fs.onPlaybackReady"
#define	RPC_UMMA_FS_onUnmountFail				"umma.fs.onUnmountFail"
#define	RPC_UMMA_FS_onCheckUsbSpeed				"umma.fs.onCheckUsbSpeed"
#define	RPC_UMMA_FS_onDefaultHddInfoChanged		"umma.fs.onDefaultHddInfoChanged"

#define	RPC_UMMA_FS_onResult					"umma.fs.onResult"
#define	RPC_UMMA_FS_onCheckDiskStarted			"umma.fs.onCheckDiskStarted"
#define	RPC_UMMA_FS_onStoragePowerOff			"umma.fs.onStoragePowerOff"

// Samba Client
#define RPC_UAPI_SMBC_ScanStart					"uapi.smbc.scanStart"
#define RPC_UAPI_SMBC_ScanStop					"uapi.smbc.scanStop"
#define RPC_UAPI_SMBC_MountedFolder				"uapi.smbc.mountedFolder"
#define RPC_UAPI_SMBC_Mount						"uapi.smbc.mount"
#define RPC_UAPI_SMBC_Unmount					"uapi.smbc.unmount"
#define RPC_UAPI_SMBC_UnmountAll				"uapi.smbc.unmountAll"

#define RPC_UMMA_SMBC_onScanResult				"umma.smbc.onScanResult"
#define RPC_UMMA_SMBC_onMountResult				"umma.smbc.onMountResult"

// Samba Server
#define RPC_UAPI_SMBS_Start						"uapi.smbs.serverStart"
#define RPC_UAPI_SMBS_Stop						"uapi.smbs.serverStop"
#define RPC_UAPI_SMBS_SetConfig					"uapi.smbs.setConfig"
#define RPC_UAPI_SMBS_GetServerInfo				"uapi.smbs.getServerInfo"
#define RPC_UAPI_SMBS_SetPasswd					"uapi.smbs.setPasswd"
#define RPC_UAPI_SMBS_CheckPasswd				"uapi.smbs.checkPasswd"
#define RPC_UAPI_SMBS_SetPermission				"uapi.smbs.setPermission"
#define RPC_UAPI_SMBS_GetSharedList				"uapi.smbs.getSharedList"
#define RPC_UAPI_SMBS_SetPublicAccess			"uapi.smbs.setPublicAccess"
#define RPC_UAPI_SMBS_GetPublicAccess			"uapi.smbs.getPublicAccess"

#define RPC_UMMA_SMBS_onRequestResult			"umma.smbs.onRequestResult"

// Ftp Server
#define RPC_UAPI_FTPS_Start						"uapi.ftps.serverStart"
#define RPC_UAPI_FTPS_Stop						"uapi.ftps.serverStop"
#define RPC_UAPI_FTPS_SetConfig					"uapi.ftps.setConfig"
#define RPC_UAPI_FTPS_GetServerInfo				"uapi.ftps.getServerInfo"
#define RPC_UAPI_FTPS_SetPasswd					"uapi.ftps.setPasswd"
#define RPC_UAPI_FTPS_CheckPasswd				"uapi.ftps.checkPasswd"
#define RPC_UAPI_FTPS_SetPermission				"uapi.ftps.setPermission"
#define RPC_UAPI_FTPS_GetSharedList				"uapi.ftps.getSharedList"
#define RPC_UAPI_FTPS_SetPublicAccess			"uapi.ftps.setPublicAccess"
#define RPC_UAPI_FTPS_GetPublicAccess			"uapi.ftps.getPublicAccess"

#define RPC_UMMA_FTPS_onRequestResult			"umma.ftps.onRequestResult"

#define	RPC_UAPI_HOTPLUG_Init					"uapi.hotplug.Init"

#define	RPC_UMMA_HOTPLUG_onDeviceConnected		"umma.hotplug.onDeviceConnected"
#define	RPC_UMMA_HOTPLUG_onChangeDevice			"umma.HOTPLUG.onChangeDevice"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define	UMERR_OK							0x00000000

#define	UMERR_REASON_NOT_ENOUGH_SPACE		0x00000001
#define	UMERR_REASON_CANCELED				0x00000002
#define	UMERR_REASON_INVALID_ARG			0x00000004
#define	UMERR_REASON_FAIL_TO_WRITE			0x00000008
#define	UMERR_REASON_FAIL_TO_READ			0x00000010
#define	UMERR_REASON_FAIL_TO_STAT			0x00000020
#define	UMERR_REASON_SIZE_LIMIT_4G			0x00000040

#define	UMERR_ON_GOING						0x80000000
#define	UMERR_ERR							0x7FFFFFFF

#define	UAPI_COPY_FLAG_RECORDINGS			0x00000001	// Recording File¿ª PVR Path¿« Storage ∑ŒCopy / Move «œ±‚ ¿ß«‘ ...
#define	UAPI_COPY_FLAG_DECRYPT				0x00000002	// Encrypted File¿ª Decrypt «œ±‚ ¿ß«‘...
#define	UAPI_COPY_FLAG_CHUNKFORMEDIA		0x00000004	// Chunked Recording File¿ªPVR Path∞° æ∆¥— Media Path(ex: USB Drive∞∞¿∫...)ø° Copy/Move∏¶ «œ±‚ ¿ß«‘....

typedef struct
{
	HUINT32			ulErr;
	HBOOL			bDonePreProcess;
	HINT32			nTotalOperationNum;
	HINT32			nDoneOperationNum;
} UAPI_STATUS_t;

#define UMMA_SMBC_IP_LEN			16
#define UMMA_SMBC_ID_LEN			64
#define UMMA_SMBC_PATH_LEN			512
#define UMMA_SMBC_PASSWORD_LEN		64
#define UMMA_SMBC_LOG_LEN			128
#define UMMA_SMBC_FOLDER_LEN		128
#define UMMA_SMBC_SRVNAME_LEN		64
#define UMMA_SMBC_WORKGROUP_LEN		64

typedef struct
{
	HCHAR	szName[UMMA_SMBC_FOLDER_LEN];
	HCHAR 	szMountPath[UMMA_SMBC_PATH_LEN];
	HCHAR	szLogMsg[UMMA_SMBC_LOG_LEN];
	HCHAR 	szId[UMMA_SMBC_ID_LEN];
	HCHAR 	szPassword[UMMA_SMBC_PASSWORD_LEN];
	HINT32	nState;
	HBOOL  	bMounted;
} UAPI_SMBC_SHARED_t;

typedef struct
{
	HCHAR				szWorkGroup[UMMA_SMBC_WORKGROUP_LEN];
	HCHAR				szName[UMMA_SMBC_FOLDER_LEN];
	HCHAR				szIp[UMMA_SMBC_IP_LEN];
	HINT32				nSharedCnt;
	UAPI_SMBC_SHARED_t	*pstSharedList;
} UAPI_SMBC_SERVER_t;

typedef struct
{
	HCHAR	szServerName[UMMA_SMBC_FOLDER_LEN];
	HCHAR	szSharedName[UMMA_SMBC_FOLDER_LEN];
	HCHAR	szIp[UMMA_SMBC_IP_LEN];
	HCHAR	szPath[UMMA_SMBC_PATH_LEN];
	HCHAR	szId[UMMA_SMBC_ID_LEN];
	HCHAR	szPassword[UMMA_SMBC_PASSWORD_LEN];
	HCHAR	szLogMsg[UMMA_SMBC_LOG_LEN];
	HINT32	nError;
} UAPI_SMBC_MOUNT_t;

typedef struct
{
	HCHAR	szName[UMMA_SMBC_FOLDER_LEN];
	HCHAR 	szPath[UMMA_SMBC_PATH_LEN];
	HINT32	nState;
	HBOOL	bWritable;
	HBOOL  	bMounted;
} UAPI_SHARED_t;

typedef UAPI_SHARED_t UAPI_FTPS_SHARED_t;
typedef UAPI_SHARED_t UAPI_SMBS_SHARED_t;

typedef enum
{
	eUAPI_SMBS_EVT_SERVER_START = 0,
	eUAPI_SMBS_EVT_SERVER_STOP,
	eUAPI_SMBS_EVT_CONFIG_CHANGE,
	eUAPI_SMBS_EVT_PASSWD_CHANGE,
	eUAPI_SMBS_EVT_PERMISSION_CHANGE,
	eUAPI_SMBS_EVT_RESPONE_SERVERINFO,
	eUAPI_SMBS_EVT_RESPONE_SHAREDLIST,
	eUAPI_SMBS_EVT_RESPONE_CHKPASSWD,
	eUAPI_SMBS_EVT_STATUS_CHANGE,
	eUAPI_SMBS_EVT_MAX
} UAPI_SMBS_EVT_e;

typedef struct
{
	HCHAR	szWorkgroup[128];
	HCHAR	szServerName[128];
	HCHAR	szServerDesc[128];

} UAPI_SMBS_CONF_t;

typedef struct
{
	HINT32				bStarted;
	HINT32				nClientCnt;
	UAPI_SMBS_CONF_t	stConf;
} UAPI_SMBS_INFO_t;

typedef enum
{
	eUAPI_FTPS_EVT_SERVER_START = 0,
	eUAPI_FTPS_EVT_SERVER_STOP,
	eUAPI_FTPS_EVT_CONFIG_CHANGE,
	eUAPI_FTPS_EVT_PASSWD_CHANGE,
	eUAPI_FTPS_EVT_PERMISSION_CHANGE,
	eUAPI_FTPS_EVT_RESPONE_SERVERINFO,
	eUAPI_FTPS_EVT_RESPONE_SHAREDLIST,
	eUAPI_FTPS_EVT_RESPONE_CHKPASSWD,
	eUAPI_FTPS_EVT_STATUS_CHANGE,
	eUAPI_FTPS_EVT_MAX
} UAPI_FTPS_EVT_e;

typedef struct
{
	HCHAR	szServerName[128];
	HINT32	nPort;
} UAPI_FTPS_CONF_t;

typedef struct
{
	HINT32				bStarted;
	HINT32				nClientCnt;
	UAPI_FTPS_CONF_t	stConf;
} UAPI_FTPS_INFO_t;

#define	UAPI_FM_BROWSE_FILE					0x10000000
#define	UAPI_FM_BROWSE_DIR					0x20000000
#define	UAPI_FM_BROWSE_VIDEO				0x01000000 // subtype of UAPI_FM_BROWSE_FILE.
#define	UAPI_FM_BROWSE_MUSIC				0x02000000 // subtype of UAPI_FM_BROWSE_FILE.
#define	UAPI_FM_BROWSE_PHOTO				0x04000000 // subtype of UAPI_FM_BROWSE_FILE.

#define UAPI_FM_BROWSE_MASK_FILE_SUBTYPE	0x0F000000

#define UAPI_FM_BROWSE_OPT_DIRTYPE_IGNORE	0x00000001
#define UAPI_FM_BROWSE_OPT_HIDDEN_IGNORE	0x00000010
#define UAPI_FM_BROWSE_OPT_META_IGNORE		0x00000100
#define UAPI_FM_BROWSE_OPT_THUMB_WAIT		0x00001000

#define UAPI_FM_BROWSE_SORT_ABS
typedef enum
{
	eUAPI_FM_BROWSE_SORT_NAME_ALPHA = 0,
	eUAPI_FM_BROWSE_SORT_NAME_RALPHA,
	eUAPI_FM_BROWSE_SORT_TITLE_ALPHA,
	eUAPI_FM_BROWSE_SORT_TITLE_RALPHA,
	eUAPI_FM_BROWSE_SORT_SIZE_LARGE,
	eUAPI_FM_BROWSE_SORT_SIZE_SMALL,
} UAPI_FM_BROWSE_SORT_e;

typedef enum
{
	eUAPI_FM_BROWSE_OPEN = 0,
	eUAPI_FM_BROWSE_CLOSE,
	eUAPI_FM_BROWSE_GETITEMS,
	eUAPI_FM_BROWSE_SCANITEMS,
	eUAPI_FM_BROWSE_SORT,
	eUAPI_FM_BROWSE_THUMB,
	eUAPI_FM_BROWSE_ADD,
	eUAPI_FM_BROWSE_REMOVE,
	eUAPI_FM_BROWSE_MAX,
} UAPI_FM_BROWSE_EVT_e;

typedef struct
{
	HUINT32		nHandleId;
	HINT32		eEvent;
	HUINT32		ulErr;
	HINT32		nTotalNum;
	HINT32		nReqIdx;
	HINT32		nDoneNum;

	HINT32		nItemSize;
	HCHAR		Itemdata[]; //flexible array member

} UAPI_BROWSE_RESULT_t;

typedef struct
{
	#if defined(WIN32)
	HUINT8 ucTemp;
	#endif
} UAPI_GROUP_RESULT_t;
typedef enum
{
	eUAPI_FM_GROUP_OPER_COPY = 0,
	eUAPI_FM_GROUP_OPER_MOVE,
	eUAPI_FM_GROUP_OPER_REMOVE,
	eUAPI_FM_GROUP_OPER_CANCEL,
	eUAPI_FM_GROUP_OPER_UNKNOWN,
} UAPI_FM_GROUP_OPER_e;

typedef enum
{
	eUAPI_FM_GROUP_PATH_ADD = 0,
	eUAPI_FM_GROUP_PATH_REMOVE,
	eUAPI_FM_GROUP_PATH_EXCLUDE, // not supported
	eUAPI_FM_GROUP_PATH_UNKNOWN,
} UAPI_FM_GROUP_SETPATH_e;

typedef HERROR	(* UAPI_Noti_t) (void *pvUserData, HINT32 nRequestId, UAPI_STATUS_t *pstInfo);
typedef HERROR	(* UAPI_NotiBrowse_t) (HUINT32 nHandleId, HINT32 nRequestId, UAPI_BROWSE_RESULT_t *pstInfo );
typedef HERROR	(* UAPI_NotiGROUP_t) (HUINT32 nHandleId, HINT32 nRequestId, UAPI_GROUP_RESULT_t *pstInfo );
typedef HERROR	(* UAPI_NotiSmbc_t) ( HINT32 eEvent, void *pszData, void *pvUserData );
typedef HERROR	(* UAPI_NotiSmbs_t) ( HINT32 eEvent, void *pszData, void *pvUserData );
typedef HERROR	(* UAPI_NotiFtps_t) ( HINT32 eEvent, void *pszData, void *pvUserData );
//	nItemState > 0 --> Added
//	nItemState < 0 --> Removed
typedef HERROR	(* UAPI_Func_t)	(HINT32 nItemState, const HCHAR *pszName, void *pvUser);


typedef struct
{
	HINT32	nRequestId;
	void	*pvUser;
	union
	{
		UAPI_Noti_t			pfnEventCallback;
		UAPI_Func_t			pfnEventFunction;
		UAPI_NotiBrowse_t	pfnBrowseCallback;
		UAPI_NotiGROUP_t	pfnGroupCallback;
	} cb;
} UAPI_CBINFO_t;

typedef struct
{
	HUINT32		ulSemId;
	HINT32		nClientHandle;
	HINT32		nRpcHandle;

	HxHASH_t	*pstCallbackTable;
	HxList_t	*pstMonitorTable;
} UAPI_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
extern void		UAPI_Init(void);
extern HERROR	UAPI_RpcProtocol_GetRpcHandle (HINT32 *pnRpcHandle);

extern void		UAPI_FM_Init(void);
extern HERROR	UAPI_FM_SetGlobalListener(void *pvUser, UAPI_Func_t onStateChanged);
extern HERROR	UAPI_FM_AddListener(HINT32 nRequestId, void *pvUser, UAPI_Noti_t onEventOccurred);
extern HINT32	UAPI_FM_Scan(const HCHAR *pszScanDir, const HCHAR *pszExtFilter, HBOOL bRecursive, void *pvUser, UAPI_Noti_t onScanStatusChange);
extern HINT32	UAPI_FM_Remove(const HCHAR *pszSource, const HCHAR *pszExtFilter, void *pvUser, UAPI_Noti_t onRemoveStatusChange);
extern HINT32	UAPI_FM_Move(const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter, HINT32 nFlag, void *pvUser, UAPI_Noti_t onMoveStatusChange);
extern HINT32	UAPI_FM_Copy(const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter, HINT32 nFlag, void *pvUser, UAPI_Noti_t onCopyStatusChange);
extern HINT32	UAPI_FM_Cancel(HINT32 nJobId, void *pvUser, UAPI_Noti_t onCancelStatusChange);
extern HINT32	UAPI_FM_MkDir(const HCHAR *pszDir, void *pvUser, UAPI_Noti_t onMkDirStatusChange);
extern HINT32	UAPI_FM_RmDir(const HCHAR *pszDir, void *pvUser, UAPI_Noti_t onRmDirStatusChange);
extern void		UAPI_FM_Flush(void);

extern HCHAR*	UAPI_FM_GetDefaultURL(HCHAR *pszDefaultURL, HCHAR *pszURLFileName);
extern HCHAR*	UAPI_FM_GetInnerURL(HCHAR *pszURLFileName);
extern HERROR   UAPI_FM_SetInnerURL(HCHAR *pszSrcFileName, HCHAR *pszDestFileName);
extern HBOOL    UAPI_FM_URLFILEExist(HCHAR *pszURLFileName);
extern HERROR   UAPI_FM_INNERURLRemove(HCHAR *InnerURLFileName);
extern HCHAR*	UAPI_FM_GetFILEPath(HCHAR *pszFileName);

extern HUINT32	UAPI_FM_BROWSE_Open(const HCHAR *pszPath, HINT32 nCategory, const HCHAR *pszExtFilter, HINT32 nOptFlag, UAPI_NotiBrowse_t onBrowseResult);
extern HERROR	UAPI_FM_BROWSE_Close( HUINT32 nHandleId );
extern HERROR	UAPI_FM_BROWSE_GetItems( HUINT32 nHandleId, HUINT32 nIndex, HUINT32 nReqCount, HINT32 nOption );
extern HERROR	UAPI_FM_BROWSE_ScanItems( HUINT32 nHandleId, HUINT32 nReqCount, HINT32 nOption );
extern HERROR	UAPI_FM_BROWSE_Sort( HUINT32 nHandleId, HINT32 nSortType );

extern HUINT32	UAPI_FM_GROUP_Create( const HCHAR *pszExtFilter, UAPI_NotiGROUP_t onGroupUpdated );
extern HERROR	UAPI_FM_GROUP_Release( HUINT32 nHandleId  );
extern HERROR	UAPI_FM_GROUP_SetPath( HUINT32 nHandleId, UAPI_FM_GROUP_SETPATH_e ePathOper, const HCHAR *pszPath  );
extern HINT32	UAPI_FM_GROUP_Operation( HUINT32 nHandleId, UAPI_FM_GROUP_OPER_e eOper, const HCHAR *pszPath );

extern HERROR	UAPI_FS_Init(void);
extern HERROR	UAPI_FS_SetNotifier(DxStorageNotifier_t pfnApkListener);
extern HERROR	UAPI_FS_Format(HUINT32 ulDeviceId, const HCHAR *pszLabel);
extern HERROR	UAPI_FS_Unmount(HUINT32 ulDeviceId);
extern HERROR	UAPI_FS_Dst(HUINT32 ulDeviceId);
extern HERROR	UAPI_FS_SetDefaultHdd(const HCHAR *pszUUID);
extern HERROR	UAPI_FS_SetName(const HCHAR *pszUUID, const HCHAR *pszStorageName);
extern HERROR	UAPI_FS_DisableHdd(const HCHAR *pszUUID);
extern HERROR	UAPI_FS_Pairing(const HCHAR *pszUUID);
extern HERROR	UAPI_FS_UnPairing(const HCHAR *pszUUID);
extern HERROR	UAPI_FS_SafeRemove(HUINT32 ulDeviceId, HBOOL bForced);
extern HERROR	UAPI_FS_GetList(HUINT32 ulFilter, DxStorage_Info_t **pastInfoList, HUINT32 *pulDevNum);
extern HERROR	UAPI_FS_FreeList(DxStorage_Info_t *pastInfoList, HUINT32 ulDevNum);
extern HERROR	UAPI_FS_GetInfo(HUINT32 ulDeviceId, DxStorage_Info_t	*pstInfo);
extern HERROR	UAPI_FS_GetPvrReservedSize(HUINT32 ulDeviceId, HUINT64 *pullReservedKbSize);
extern HERROR	UAPI_FS_GetSize(HUINT32 ulDeviceId, HUINT64 *pullTotalKbSize, HUINT64 *pullAvailableKbSize, HUINT64 *pullUsedKbSize, HUINT64 *pullReservedKbSize, HUINT64 *pullAvailablePvrKbSize);
extern const HCHAR* UAPI_FS_GetFsString(const DxStorage_FsType_e	eFsType);
extern const HCHAR* UAPI_FS_GetDevTypeString(const DxStorage_DevType_e	eDevType);
extern HINT32	UAPI_FS_GetHddSerialNumber(const HCHAR *dev, HCHAR **hddsn);
extern HBOOL	UAPI_FS_CheckStoragePowerOff(void);
extern HBOOL	UAPI_FS_UseTsrOnlyDevice(void);
extern HERROR	UAPI_FS_GetAvailablePvrPath (HCHAR *pszPath);

extern HERROR	UAPI_FS_CheckUsbSpeed(const HCHAR *pszUUID);

extern void		UAPI_SMBC_Init(void);
extern HERROR	UAPI_SMBC_SetNotifier( UAPI_NotiSmbc_t pfnApkListener );
extern HINT32	UAPI_SMBC_StartScan(void);
extern HINT32	UAPI_SMBC_StopScan(void);
extern HERROR	UAPI_SMBC_Mount(const HCHAR *pszIp, const HCHAR *pszSharedName, const HCHAR *pszId, const HCHAR *pszPasswd, const HCHAR *pszPath );

extern HERROR	UAPI_SMBC_Unmount(const HCHAR *pszIp, const HCHAR *pszSharedName, const HCHAR *pszPath);
extern HERROR	UAPI_SMBC_UnmountAll(void);


extern void		UAPI_SMBS_Init(void);
extern HERROR	UAPI_SMBS_SetNotifier( UAPI_NotiSmbs_t pfnApkListener );
extern HINT32	UAPI_SMBS_StartServer(void);
extern HINT32	UAPI_SMBS_StopServer(void);
extern HERROR	UAPI_SMBS_SetConfig( const UAPI_SMBS_CONF_t *pstConfig );
extern HERROR	UAPI_SMBS_GetServerInfo( UAPI_SMBS_INFO_t *pstConfig );
extern HERROR	UAPI_SMBS_SetPermission(const HCHAR *pszUserName, UAPI_SMBS_SHARED_t *pstShared );
extern HERROR	UAPI_SMBS_GetSharedList(const HCHAR *pszUserName, HINT32 *pnCount, UAPI_SMBS_SHARED_t **ppstList);
extern HERROR	UAPI_SMBS_SetPassword(const HCHAR *pszUserName, const HCHAR *pszCurPasswd, const HCHAR *pszNewPasswd );
extern HERROR	UAPI_SMBS_CheckPassword(const HCHAR *pszUserName, const HCHAR *pszPasswd, HBOOL *pbIsValid);
extern HERROR	UAPI_SMBS_SetPublicAccess(const HCHAR *pszUserName,  HBOOL bIsAccess);
extern HERROR	UAPI_SMBS_GetPublicAccess(const HCHAR *pszUserName, HBOOL *pbIsAccess);

extern void		UAPI_FTPS_Init(void);
extern HERROR	UAPI_FTPS_SetNotifier( UAPI_NotiFtps_t pfnApkListener );
extern HINT32	UAPI_FTPS_StartServer(void);
extern HINT32	UAPI_FTPS_StopServer(void);
extern HERROR	UAPI_FTPS_SetConfig( const UAPI_FTPS_CONF_t *pstConfig );
extern HERROR	UAPI_FTPS_GetServerInfo( UAPI_FTPS_INFO_t *pstConfig );
extern HERROR	UAPI_FTPS_SetPermission(const HCHAR *pszUserName, UAPI_FTPS_SHARED_t *pstShared );
extern HERROR	UAPI_FTPS_GetSharedList(const HCHAR *pszUserName, HINT32 *pnCount, UAPI_FTPS_SHARED_t **ppstList);
extern HERROR	UAPI_FTPS_SetPassword(const HCHAR *pszUserName, const HCHAR *pszCurPasswd, const HCHAR *pszNewPasswd );
extern HERROR	UAPI_FTPS_CheckPassword(const HCHAR *pszUserName, const HCHAR *pszPasswd, HBOOL *pbIsValid);
extern HERROR	UAPI_FTPS_SetPublicAccess(const HCHAR *pszUserName,  HBOOL bIsAccess);
extern HERROR	UAPI_FTPS_GetPublicAccess(const HCHAR *pszUserName, HBOOL *pbIsAccess);


extern HERROR	UAPI_HOTPLUG_Init(void);
extern HERROR	UAPI_HOTPLUG_SetNotifier(DxHotplugNotifier_t pfnApkListener);
extern HERROR	UAPI_HOTPLUG_GetDevCount(DxHotplug_DevType_e eType, HUINT32 *pulCount);

#ifdef __cplusplus
}
#endif

#endif
/* end of file */

