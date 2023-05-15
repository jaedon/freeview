/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_meta_record.h
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

#ifndef ___APK_META_RECORD_H___
#define ___APK_META_RECORD_H___
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef void * ApkMetaRec_Handle_t;
typedef void (* APK_META_REC_Notifier_t) (void *userdata, HINT32 evt, HINT32 p1, HINT32 p2, HINT32 p3, ...);
typedef void (* ApkMetaRecord_Notifier_t) (ApkMetaRec_Handle_t self, HINT32 event, const HINT32 *args, void *userdata);

typedef	enum
{
	eAPK_META_REC_DoNothing,

	// set
	eAPK_META_REC_DO_SCAN,
	eAPK_META_REC_DO_COPY,
	eAPK_META_REC_DO_DELETE,


	// event
	eAPK_META_REC_EVENT_START = 100,
	eAPK_META_REC_EVENT_DELETED,
	eAPK_META_REC_EVENT_UPDATED,
	eAPK_META_REC_EVENT_ADDED,

	eAPK_META_REC_EVENT_LOAD,
	eAPK_META_REC_CmdEnd,
} APK_META_REC_CMD_e;

typedef	struct
{
	HCHAR	szUrl[DxRECLIST_URL+1];
	HBOOL	bLock;
} APK_META_REC_LOCK_PARAM_t;

typedef	struct
{
	HUINT32								ulParamCount;
	APK_META_REC_LOCK_PARAM_t	*paramList;
} APK_META_REC_LOCK_t;

typedef	struct
{
	HUINT32	ulSeriesID;
	HBOOL	bSeriesLock;
} APK_META_REC_SERIESLOCK_t;

typedef	struct
{
	HCHAR	szUrl[DxRECLIST_URL+1];
	HCHAR	szRename[DxRECLIST_NAME_LEN+1];
} APK_META_REC_RENAME_PARAM_t;


typedef	struct
{
	HUINT32								ulParamCount;
	APK_META_REC_RENAME_PARAM_t	*paramList;
} APK_META_REC_RENAME_t;

typedef	struct
{
	HCHAR	szUrl[DxRECLIST_URL+1];
} APK_META_REC_PARAM_t;

typedef	struct
{
	HCHAR	szUrl[DxRECLIST_URL+1];
	HINT32  nNumOfTags;
	HCHAR	szTags[DxRECLIST_MAX_UPDATE_CMD][DxRECLIST_MAX_UPDATE_TAG+1];
	DxRecListData_t		recUpdateData;
} APK_META_REC_UPDATE_PARAM_t;


typedef	struct
{
	HUINT32								ulParamCount;
	APK_META_REC_UPDATE_PARAM_t	*paramList;
} APK_META_REC_UPDATE_t;


typedef	struct
{
	HUINT32						ulParamCount;
	APK_META_REC_PARAM_t	*paramList;
} APK_META_REC_STOP_REC_t;


typedef	struct
{
	HCHAR	szDestUrl[DxRECLIST_URL+1];
	HUINT32							ulParamCount;
	APK_META_REC_PARAM_t		*paramList;
} APK_META_REC_COPY_t;

typedef	struct
{
	HUINT32						ulParamCount;
	APK_META_REC_PARAM_t	*paramList;
} APK_META_REC_DELETE_t;


typedef	struct
{
	HCHAR	szUrl[DxRECLIST_URL+1];
	DxRecList_Bookmark_t	stBookmark;
} APK_META_REC_BOOKMARK_PARAM_t;


typedef	struct
{
	HUINT32									ulParamCount;
	APK_META_REC_BOOKMARK_PARAM_t	*paramList;
} APK_META_REC_BOOKMARK_t;

typedef	struct
{
	HCHAR	szUrl[DxRECLIST_URL+1];
} APK_META_REC_URL_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HBOOL	APK_META_RECORD_Init(void);
HBOOL	APK_META_RECORD_DeInit(void);
HUINT32	APK_META_RECORD_GetCID (const HCHAR *url);

void	APK_META_RECORD_AddNotifier(ApkMetaRecord_Notifier_t notifier, void *userdata);
HERROR	APK_META_RECORD_Scan(HCHAR	*url, HBOOL	bSyncmode);
HERROR	APK_META_RECORD_Lock(APK_META_REC_LOCK_t	*pstLock, HBOOL	bSyncmode);
HERROR	APK_META_RECORD_SetSeriesLock(HCHAR	*pSeriesID, HBOOL	bSeriesLock, HBOOL	bSyncmode);
HERROR	APK_META_RECORD_Rename(APK_META_REC_RENAME_t	*pstRename, HBOOL	bSyncmode);
HERROR	APK_META_RECORD_Update(APK_META_REC_UPDATE_t	*pstUpdate, HBOOL	bSyncmode);
HERROR	APK_META_RECORD_StopRecording(APK_META_REC_STOP_REC_t	*pstStopRec, HBOOL	bSyncmode);
HERROR	APK_META_RECORD_Copy (HUINT32 cid, const HCHAR *path,HBOOL bDecrypt, HxReturnFunc_t ret, void *userdata);
HERROR	APK_META_RECORD_Delete(APK_META_REC_DELETE_t	*pstDelete, HBOOL	bSyncmode);
HERROR	APK_META_RECORD_DeleteAsync(APK_META_REC_DELETE_t	*pstDelete);
HERROR	APK_META_RECORD_AddBookmark(APK_META_REC_BOOKMARK_t		*pstBookmark, HBOOL	bSyncmode);
HERROR	APK_META_RECORD_RemoveBookmark(APK_META_REC_BOOKMARK_t	*pstBookmark, HBOOL	bSyncmode);
HERROR	APK_META_RECORD_Cancel(void);
HERROR	APK_META_RECORD_Clear(void);

DxRecListData_t*	APK_META_RECORD_GetByCID (const HUINT32 cid);
DxRecListData_t*	APK_META_RECORD_GetByUrl (const HCHAR *url);
HUINT32		APK_META_RECORD_GetContentsIdByUrl (const HCHAR *url);
DxRecListData_t*	APK_META_RECORD_GetByIdx (const HUINT32	iter);
void		APK_META_RECORD_Release (DxRecListData_t *recData);
HUINT32	APK_META_RECORD_Count (void);
void 	APK_META_RECORD_RemoveByStorageID(HCHAR *pUUID);
HxVector_t	*APK_META_RECORD_GetItems (void);
HERROR	APK_META_RECORD_Move (HUINT32 cid, const HCHAR *path, HBOOL bDecrypt,HxReturnFunc_t ret, void *userdata);
void		APK_META_RECORD_CancelFileOperation (HUINT32 cid);

HxVector_t *		APK_META_RECORD_GetRecordings (HxEqualFunc_t pfFilter, void *pvKey);
void				APK_META_RECORD_EnableLoadEvent(void);

HBOOL	APK_META_RECORD_IsFirstScanDone(void);

#endif

/* end of file */
