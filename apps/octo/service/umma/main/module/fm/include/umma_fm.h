/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_fm.h
	@brief

	Description:  									\n
	Module: Umma Internal Header                    \n

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

#ifndef __UMMA_FM_H__
#define __UMMA_FM_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<hlib.h>
#include <dlib.h>
#include	<sys/stat.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	UMMA_UNDEFINED			0
#define	UMMA_RECYCLEBIN_NAME	".recycleBin"

#define	UMMA_FM_EXCLUDE_SYSPATH ".umma;.recycleBin;.cache;.tsr;"
#define UMMA_FM_EXCLUDE_RECYCLEBIN "$RECYCLE.BIN"
#define UMMA_FM_THUMBNAIL_EXTRACT_TIME	7 /*sec*/

#define UMMA_VIDEO_EXT  "3gp|3g2|mov|flv|swf|m2ts|rm|ts|mkv|avi|mp4|asf|wmv|tp|m2v|m4v|mts|mpg|vob|divx|mpeg|mpe|dat|ogm|ogg|rmvb"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eUMMA_CMD_NONE		= 0,

	eUMMA_CMD_CANCEL,
	eUMMA_CMD_DIRECTDIR,
	eUMMA_CMD_DIR,
	eUMMA_CMD_GETINFO_PRIO,
	eUMMA_CMD_LAZYDIR,

	eUMMA_CMD_OPER_GROUP,

	eUMMA_CMD_MOVE,
	eUMMA_CMD_MAKEDIR,

	eUMMA_CMD_COPY,
	eUMMA_CMD_REMOVE,

	eUMMA_CMD_REMOVEDIR_CHKSUB,
	eUMMA_CMD_REMOVEDIR,
	eUMMA_CMD_REMOVEDIR_CHKEND,

	eUMMA_CMD_GETINFO,
	eUMMA_CMD_MAX
} UMMA_CMD_e;


typedef enum
{
	eUMMA_UPDATE_REMOVED,
	eUMMA_UPDATE_ADDED,
	eUMMA_UPDATE_REMOVED_LIKE
} UMMA_UPDATE_e;

typedef struct
{
	HBOOL	bOpsReady;
	HBOOL	bChunked;
	HBOOL	bOnGoing;

	HxFILE_t	*pstSourceHandle;
	HxFILE_t	*pstDstHandle;

	union
	{
		HINT32	nChunkNo;
	} ex;
} UMMA_OPS_DETAIL_t;

typedef struct
{
	HUINT32	ulErr;
	HINT32	nRequestId;
	HUINT32	ulLastCallback;

	HINT32	nPreProcessDone;
	HINT32	nOperationCount;
	HINT32	nOperationDoneCount;
} UMMA_REQINFO_t;

typedef struct
{
	HUINT32			ulErr;
	UMMA_CMD_e		eCmd;
	UMMA_CMD_e		ePostCmd;
	HINT32			nRequestId;
	void			*pvUser;				//	eUMMA_CMD_CANCEL : target request id
											//	eUMMA_CMD_DIR/DIRDIRECT/LAZYDIR/GETFILEINFO : mount info

	HINT32			nGiveUp;
	HINT32			nRecursive;
	HINT32			nFlags;

	HCHAR			*pszSourceName;
	HCHAR			*pszDestName;
	HCHAR			*pszExtFilter;

	UMMA_REQINFO_t		*pstReqInfo;
	UMMA_OPS_DETAIL_t	stDetail;
} UMMA_OPS_t;

typedef struct
{
	HCHAR	szMountPath[512];
	HCHAR	szUUID[512];
	HBOOL	bIsWritable;
	HCHAR	szFileSystem[64];
	HBOOL	bIsNetworkDevice;
	DxStorageInterface_e	eInterface;
	HUINT64	ullReservedSizeKb;
} UMMA_MOUNTINFO_t;

typedef struct
{
	HUINT32 eBrowseType;

	void *pvUmma;
	HERROR (*cbResult)(void *pvUmma, HINT32 nReqId, void *pvResult);

	HCHAR szFilter[256];
	HUINT32 nOptFlags;
} UMMA_FM_BROWSE_OPEN_t;

typedef struct
{
	HCHAR			szExtension[8];
	DxFileType_e	eType;
	HERROR			(*getFileInfo)(const HCHAR *pszFileName, void **ppvInfo, HINT32 *pnSize);
	HxFreeFunc_t	freeInfo;
} UMMA_FM_IFACE_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
extern int 		UMMA_FM_Init(void);
extern HERROR	UMMA_FM_AddStorage(HCHAR* pszMountPath, HCHAR* pszUUID, DxStorageInterface_e eInterface, HUINT64 ullReservedSizeKb);
extern HERROR	UMMA_FM_RemoveStorage(HCHAR* pszMountPath);
extern HERROR	UMMA_FM_ReportFormatStart(HUINT32 ulDeviceId);
extern HERROR	UMMA_FM_ReportFormatDone(void);

extern HERROR	UMMA_FM_IFACE_AddFileInfoWoDetail(const void *pvUser, const HCHAR *pszNewFullPath, struct stat *fstat);
extern HERROR	UMMA_FM_IFACE_AddFileInfo(const void *pvUser, const HCHAR *pszSourceName, HBOOL bThumbnailUrgent);
extern HERROR	UMMA_FM_IFACE_AddFileInfoDirect(void *pvInfo);
extern HUINT64	UMMA_FM_IFACE_GetNewUid(struct stat *fstat);
extern UMMA_FM_IFACE_t*	UMMA_FM_IFACE_GetIfaceInfo(const HCHAR *pszExt);

extern HERROR	UMMA_FM_TOOLKIT_GetSizeKB(const HCHAR *pszMountPath, HUINT64 *pullTotalKBytes, HUINT64 *pullUsedKBytes, HUINT64 *pullFreeKBytes);
extern HUINT32	UMMA_FM_TOOLKIT_Copy(UMMA_OPS_t *pstOps, HBOOL bStatus, HUINT64 ullReservedKb);
extern HUINT32	UMMA_FM_TOOLKIT_Delete(const HCHAR *pszFileName);
extern HERROR	UMMA_FM_TOOLKIT_GetDeviceSizeKB(const HCHAR *pszMountPath, HUINT64 *pullTotalKBytes, HUINT64 *pullUsedKBytes, HUINT64 *pullFreeKBytes);
extern HCHAR**	UMMA_FM_TOOLKIT_GetExtRec (void);
extern HBOOL	UMMA_FM_TOOLKIT_ExtSelector (HCHAR *pszName, HCHAR *filterstrarray[]);


extern void		UMMA_FM_CACHE_CheckVersion(const HCHAR *pszMountPath);
extern void*	UMMA_FM_CACHE_GetFileInfo(UMMA_MOUNTINFO_t *pstMountInfo, const HCHAR *pszSourceName, struct stat *fstat);
extern void		UMMA_FM_CACHE_AddFileInfo(void *pvFileInfo, HINT32 nSize);

extern HERROR UMMA_FM_BROWSE_Init(void);
extern HUINT32 UMMA_FM_BROWSE_Open( const HCHAR *pszPath, UMMA_FM_BROWSE_OPEN_t *pstOpt, HINT32 nReqId );
extern HERROR UMMA_FM_BROWSE_Close( HUINT32 nHandleId );
extern HERROR UMMA_FM_BROWSE_GetItems( HUINT32 nHandleId, HUINT32 nIdx, HUINT32 nReqCnt, HINT32 nOption );
extern HERROR UMMA_FM_BROWSE_ScanItems( HUINT32 nHandleId, HUINT32 nReqCnt, HINT32 nOption );
extern HERROR UMMA_FM_BROWSE_Sort( HUINT32 nHandleId, HUINT32 eSortType );

extern HUINT32 UMMA_FM_GROUP_CreateHandle( const HCHAR *pszExtFilter );
extern HINT32 UMMA_FM_GROUP_ReleaseHandle( HUINT32 nHandleId );
extern HINT32 UMMA_FM_GROUP_AddPath( HUINT32 nHandleId, HCHAR *pszPath );
extern HINT32 UMMA_FM_GROUP_RemovePath( HUINT32 nHandleId, HCHAR *pszPath );
extern HxList_t * UMMA_FM_GROUP_GetList( HUINT32 nHandleId );
extern HINT32 UMMA_FM_GROUP_GetItemCount( HUINT32 nHandleId, HINT32 *pnCount );
extern HINT32 UMMA_FM_GROUP_GetTotalSize( HUINT32 nHandleId, HUINT64 *pullSize );
extern HUINT32 UMMA_FM_GROUP_CheckFreeSpace( HUINT32 nHandleId, const HCHAR *pszDestPath );
extern HINT32 UMMA_FM_GROUP_SetOperation( HUINT32 nHandleId, HINT32 eOper, HINT32 nReqId );
extern HINT32 UMMA_FM_GROUP_GetOperation( HUINT32 nHandleId, HINT32 *peOper, HINT32 *pnReqId );
extern HBOOL UMMA_FM_GROUP_IsWildcardPath( const HCHAR *pszPath, HCHAR *pszGetDirPath, HINT32 nBufSize );
extern const HCHAR * UMMA_FM_GROUP_GetExtFilter( HUINT32 nHandleId );
#endif
/* end of file */


