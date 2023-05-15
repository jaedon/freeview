/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_meta_record.c
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
#include	<oapi.h>
#include 	<dapi.h>
#include	<apk.h>
#include	<_apk_int.h>
#include	<vkernel.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#ifndef	APK_META_RECORD
#define	APK_META_RECORD

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HBOOL			bEnableLoadEvent;
	HBOOL			bFirstLoadDone;
	HxHASH_t		*recordings;
	HxHASH_t		*urlMap;

	HxHASH_t		*recvTable;	///< temporary hash  : 운용의 목적으로 잠깐씩 사용함
	HxVector_t			*cidVector;	///< temporary vector: 운용의 목적으로 잠깐씩 사용함

	APK_META_REC_Notifier_t		notifier;
	void				*userdata;

	HUINT32				semaphore;

	// for port
	HxVector_t			*tempCache;
} apkMetaRec_Ctx_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static ApkMetaRecord_Notifier_t	s_port_notifier;
static void *				s_port_userdata;
static HULONG 		s_ulScanTimerId = 0;


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define	________LOCAL___Prototype_______

static apkMetaRec_Ctx_t *	apk_meta_record_GetContext (void);
static void	apk_meta_record_ReleaseContext (apkMetaRec_Ctx_t *ctx);

#define	________LOCAL___Body_______
HCHAR *	apk_meta_record_GetURL (HUINT32 cid, HCHAR *buf, HUINT32 n);
HUINT32	apk_meta_record_GetCID (const HCHAR *url);


static const HCHAR *	apk_meta_record_EventToString (OxMetaRecord_Event_e evt)
{
	switch (evt)
	{
	case eOxMETA_RECORD_ADDED:   return HxNAMEOF(eOxMETA_RECORD_ADDED);
	case eOxMETA_RECORD_UPDATED: return HxNAMEOF(eOxMETA_RECORD_UPDATED);
	case eOxMETA_RECORD_REMOVED: return HxNAMEOF(eOxMETA_RECORD_REMOVED);
	case eOxMETA_RECORD_SCANDONE: return HxNAMEOF(eOxMETA_RECORD_SCANDONE);
	default:
		break;
	}
	return "Unknown";
}


static HINT32	apk_meta_record_CompareBookmark (const DxRecList_Bookmark_t *lhs, const DxRecList_Bookmark_t *rhs)
{
	if (lhs->ulPos > rhs->ulPos)
		return +1;
	if (lhs->ulPos < rhs->ulPos)
		return -1;
	return strcmp(lhs->szName, rhs->szName);
}

static void		apk_meta_record_SyncedReturn (HERROR err, void *userdata, ...)
{
	HERROR	**perr;

	 perr = (HERROR **)userdata;
	*perr = (HERROR  *)APK_MemDup(&err, sizeof(HERROR));
}

static DxRecListData_t *	apk_meta_record_Unpack (DxRecListData_t *rec)
{
	if (rec->ulExtEvtCnt)
		rec->pstExtEvts = (DxRecList_ExtEvt_t *)(rec + 1);
	else
		rec->pstExtEvts = NULL;
	return rec;
}

static HERROR	apk_meta_record_AddCid (apkMetaRec_Ctx_t *ctx, HUINT32 ulContentId, DxRecListData_t *rec)
{
	HUINT32 ulRecordingNum, ulUrlNum;

	if (rec == NULL)
	{
		HxLOG_Error("invalid param. \n ");
		return ERR_FAIL;
	}

	ulRecordingNum = HLIB_HASH_Size(ctx->recordings);
	ulUrlNum = HLIB_HASH_Size(ctx->urlMap);

	HLIB_HASH_Replace(ctx->urlMap, (void *)ulContentId, APK_StrDup(rec->szUrl));
	HLIB_HASH_Replace(ctx->recordings, (void *)ulContentId, (void *)rec);

	HxLOG_Debug("add cid(%d). recording num(%d)->(%d) \n",ulContentId,ulRecordingNum,HLIB_HASH_Size(ctx->recordings));
	HxLOG_Debug("add cid(%d). url num(%d)->(%d) \n",ulContentId,ulUrlNum,HLIB_HASH_Size(ctx->urlMap));

	return ERR_OK;
}

static HERROR	apk_meta_record_RemoveCid (apkMetaRec_Ctx_t *ctx, HUINT32 ulContentId)
{
	HBOOL bRemoved;
	HUINT32 ulRecordingNum, ulUrlNum;

	ulRecordingNum = HLIB_HASH_Size(ctx->recordings);
	bRemoved = HLIB_HASH_Remove(ctx->recordings, (void *)ulContentId);
	if (bRemoved != TRUE)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("fail to remove cid (%d) recording num (%d)\n"),ulContentId, ulRecordingNum);
	}
	else
	{
		HxLOG_Debug("removed cid(%d). recording num(%d)->(%d) \n",ulContentId,ulRecordingNum,HLIB_HASH_Size(ctx->recordings));
	}

	ulUrlNum = HLIB_HASH_Size(ctx->urlMap);
	bRemoved = HLIB_HASH_Remove(ctx->urlMap, (void *)ulContentId);
	if (bRemoved != TRUE)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("fail to remove cid (%d) url num (%d)\n"),ulContentId, ulUrlNum);
	}
	else
	{
		HxLOG_Debug("removed cid(%d). url num(%d)->(%d) \n",ulContentId,ulUrlNum,HLIB_HASH_Size(ctx->urlMap));
	}

	return ERR_OK;
}

static HERROR	apk_meta_record_UpdateCid (apkMetaRec_Ctx_t *ctx, HUINT32 ulContentId, DxRecListData_t *rec)
{
	HLIB_HASH_Replace(ctx->recordings, (void *)ulContentId, (void *)rec);
	HxLOG_Debug("Update cid(%d).  \n",ulContentId);

	return ERR_OK;
}

static HxVector_t *	apk_meta_record_GetRecordings (apkMetaRec_Ctx_t *ctx, HxEqualFunc_t filter, void *key)
{
	HxVector_t		*list;
	DxRecListData_t *rec;
	DxRecListData_t *clone;
	HxHASH_TableIter_t	iter;

	if (HLIB_HASH_Size(ctx->recordings) == 0)
	{
		HxLOG_Debug("%s() empty recordings!\n", __FUNCTION__);
		return NULL;
	}

	list = HLIB_VECTOR_NewEasy(0, (HxFreeFunc_t)DxRECLISTDATA_Delete, NULL);
	if (list == NULL)
	{
		HxLOG_Error("%s() out of memory!!\n", __FUNCTION__);
		return NULL;
	}

	HLIB_HASH_IterInit(&iter, ctx->recordings);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
	{
		if (filter == NULL || filter(rec, key))
		{
			clone = DxRECLISTDATA_Clone(rec);
			if (clone)
				HLIB_VECTOR_Add(list, clone);
		}
	}
	HxVECTOR_DELETE_IF_EMPTY(list);
	return list;
}

STATIC void	apk_meta_record_freeOnDataReceive(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 6);

	pvData = apArgV[4];
	if (pvData)
		APK_Free(pvData);
}

static void	apk_meta_record_OnDataReceive (const HCHAR *name, HUID uid, const void *data, HUINT32 size, HINT32 variation)
{
	apkMetaRec_Ctx_t	*ctx;
	DxRecListData_t	*rec;
	HxHASH_t	*recv;
	HxVector_t		*cids;
	HxHASH_TableIter_t	iter;
	APK_META_REC_Notifier_t	notifier;
	void			*userdata;

	HxLOG_Trace();

	ctx = apk_meta_record_GetContext();

	if (data && size)
	{
		DAxIterator_t	iter;

		recv = ctx->recvTable;
		DAPI_InitIterator(&iter, data, size, 0);
		while (DAPI_NextIterator(&iter))
		{
			rec = apk_meta_record_Unpack((DxRecListData_t *)iter.data);
			rec = DxRECLISTDATA_Clone(rec);
			if (rec)
				HLIB_HASH_Insert(recv, (void *)rec->ulContentId, (void *)rec);
		}
	}
	else recv = NULL;

	cids = ctx->cidVector;
	HLIB_VECTOR_RemoveAll(cids);

	if (variation > 0) // added
	{
		// Scan 이후 add 이벤트를 올리지 않고, Load 이벤트로 올리고,
		// web-app 에서 refresh 를 요청 할 것이라 아래 내용은 필요 없음
		/*
		if (recv)
		{
			HLIB_HASH_IterInit(&iter, recv);
			while (HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
			{
				if (HLIB_HASH_Lookup(ctx->recordings, (void *)rec->ulContentId) == NULL)
				{
					HLIB_VECTOR_Add(cids, (void *)rec->ulContentId);
					HxLOG_Debug("   Added[cid:%u, url:%s, name:\"%s\"]\n",  rec->ulContentId, rec->szUrl, rec->szName);
				}
			}
		}*/
	}
	else if (variation < 0) // removed
	{
		HLIB_HASH_IterInit(&iter, ctx->recordings);
		while (HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
		{
			if (recv && HLIB_HASH_Lookup(recv, (void *)rec->ulContentId))
				continue;

			HLIB_VECTOR_Add(cids, (void *)rec->ulContentId);
			HxLOG_Debug(" Removed[cid:%u, url:%s, name:\"%s\"]\n",  rec->ulContentId, rec->szUrl, rec->szName);
		}
	}
	else	// updated
	{
		if (recv)
		{
			DxRecListData_t	*old;

			HLIB_HASH_IterInit(&iter, recv);
			while (HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
			{
				old = (DxRecListData_t *)HLIB_HASH_Lookup(ctx->recordings, (void *)rec->ulContentId);
				if (old == NULL || DxRECLISTDATA_Compare(rec, old) != 0)
				{
					HLIB_VECTOR_Add(cids, (void *)rec->ulContentId);
					HxLOG_Debug(" Updated[cid:%u, url:%s, name:\"%s\"]\n",  rec->ulContentId, rec->szUrl, rec->szName);
				}
			}
		}
	}

	if (recv)
	{
		HLIB_HASH_IterInit(&iter, recv);
		while (HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
		{
			(void)apk_meta_record_AddCid(ctx, rec->ulContentId, rec);

			HxLOG_Message(" + [cid:%u, url:%s, name:\"%s\"]\n",  rec->ulContentId, rec->szUrl, rec->szName);
		}
		HLIB_HASH_RemoveAll(recv);
	}

	notifier = ctx->notifier;
	userdata = ctx->userdata;

	HxLOG_Debug(" variation:%d, changed:%d\n",  variation, HLIB_VECTOR_Length(cids));
	HxLOG_Debug(" recording num(%d), url num(%d) \n",HLIB_HASH_Size(ctx->recordings), HLIB_HASH_Size(ctx->urlMap));

	if(ctx->bFirstLoadDone == FALSE)
	{
		ctx->bFirstLoadDone = TRUE;
	}

	if (notifier)
	{
		HUINT32	*ids;
		HINT32	count;
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		count = HLIB_VECTOR_Length(cids);
		if (count == 0)
			ids = NULL;
		else
			ids = (HUINT32 *)APK_MemDup(cids->vector, sizeof(HUINT32) * count);

		apk_meta_record_ReleaseContext(ctx);


		stApkEvtArgs.pfnFreeFunc = apk_meta_record_freeOnDataReceive;
		stApkEvtArgs.ulArgCount = 6;
		stApkEvtArgs.apArgV[0] = (void *)notifier;
		stApkEvtArgs.apArgV[1] = (void *)userdata;
		stApkEvtArgs.apArgV[2] = (void *)variation;
		stApkEvtArgs.apArgV[3] = (void *)count;
		stApkEvtArgs.apArgV[4] = (void *)ids;
		stApkEvtArgs.apArgV[5] = (void *)0;
		APK_EVENT_Emit(&stApkEvtArgs);

		HxLOG_Info(" callback (userdata:0x%X, variation:%d, count:%d, ids:0x%X)\n"
				,  (int)userdata, variation, count, (int)ids);
	}
	else apk_meta_record_ReleaseContext(ctx);
}

static void	apk_meta_record_Load (apkMetaRec_Ctx_t *ctx, HINT32 variation)
{
	DAPI_GetAll(DxNAMEOF(DxRecListData_t), (DAPI_Getter)apk_meta_record_OnDataReceive, (void *)variation);
	(void)ctx;
}

STATIC void	apk_meta_record_freeOapiNotifier(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 6);

	pvData = apArgV[4];
	if (pvData)
		APK_Free(pvData);
}

static void apk_meta_record_OapiNotifier (
						OxMetaRecord_Event_e evt
						, HINT32 count
						, HUINT32 cid
						, const DxRecListData_t *rec
						, void *unused
					)
{
	apkMetaRec_Ctx_t	*ctx;
	APK_META_REC_Notifier_t	notifier;
	void			*userdata;
	HINT32			variation;
	DxRecListData_t	*clone;

	HxLOG_Debug("%s(%s, count:%d, cid:%u)\n", __FUNCTION__, apk_meta_record_EventToString(evt), count, cid);

	ctx = apk_meta_record_GetContext();
	if(NULL == ctx)
	{
		HxLOG_Error("Record context is NULL!");
		return;
	}

	if(evt == eOxMETA_RECORD_SCANDONE)
	{
		// If count is not 0, bFirstLoadDone flag will be changed to TRUE in apk_meta_record_Load().
		if(count == 0 && ctx->bFirstLoadDone == FALSE)
		{
			ctx->bFirstLoadDone = TRUE;
		}
		apk_meta_record_ReleaseContext(ctx);
		return;
	}

	variation = evt == eOxMETA_RECORD_ADDED   ? +count
			  : evt == eOxMETA_RECORD_REMOVED ? -count
			  : 0;

	if (count > 1 || cid == 0)
	{
		apk_meta_record_Load(ctx, variation);
		apk_meta_record_ReleaseContext(ctx);
		return;
	}

	if (count == 0)
	{
		HxLOG_Error("%s() count == 0!\n", __FUNCTION__);
		apk_meta_record_ReleaseContext(ctx);
		return;
	}

	notifier = ctx->notifier;
	userdata = ctx->userdata;

#if 0
	/**
	 * Delayed recording은 TSR이 이미 존재하는 상태에서 Recording으로 변경되기 때문에 Added가 아닌 Update로 이벤트가 온다.
	 */
	if (evt == eOxMETA_RECORD_UPDATED && HLIB_HASH_Lookup(ctx->recordings, (void *)cid) == NULL)
	{
		HxLOG_Debug("%s(cid:%u) maybe delayed recording\n", __FUNCTION__, cid);
		variation = +1;
		evt = eOxMETA_RECORD_ADDED;
	}
#endif
	switch (evt)
	{
	case eOxMETA_RECORD_ADDED:
		if (rec)
		{
			clone = DxRECLISTDATA_Clone(rec);
			if (clone)
			{
				HxHASH_TableIter_t	iter;
				HCHAR				*url;

				HLIB_HASH_IterInit(&iter, ctx->urlMap);
				while (HLIB_HASH_IterNext(&iter, NULL, (void **)&url))
				{
					if (HxSTD_StrCmp(url, clone->szUrl) == 0)
						HLIB_HASH_IterRemove(&iter);
				}

				(void)apk_meta_record_AddCid(ctx, cid, clone);
			}
		}
		break;

	case eOxMETA_RECORD_UPDATED:
		if (rec)
		{
			clone = DxRECLISTDATA_Clone(rec);
			if (clone)
			{
				(void)apk_meta_record_UpdateCid(ctx, cid, clone);
			}
		}
		break;

	case eOxMETA_RECORD_REMOVED:
		//HLIB_HASH_Remove(ctx->recordings, (void *)cid);
		break;

	default:
		break;
	}
	apk_meta_record_ReleaseContext(ctx);

	if (notifier)
	{
		HUINT32	ids[2];
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		ids[0] = cid;
		ids[1] = 0;

		stApkEvtArgs.pfnFreeFunc = apk_meta_record_freeOapiNotifier;
		stApkEvtArgs.ulArgCount = 6;
		stApkEvtArgs.apArgV[0] = (void *)notifier;
		stApkEvtArgs.apArgV[1] = (void *)userdata;
		stApkEvtArgs.apArgV[2] = (void *)variation;
		stApkEvtArgs.apArgV[3] = (void *)1;
		stApkEvtArgs.apArgV[4] = (void *)APK_MemDup(ids, sizeof(HUINT32) * 2);
		stApkEvtArgs.apArgV[5] = (void *)0;
		APK_EVENT_Emit(&stApkEvtArgs);

		HxLOG_Info("%s() callback (userdata:0x%X, variation:%d, count:%d, ids:0x%X)\n"
				, __FUNCTION__, (int)userdata, variation, 1, (int)ids);
	}
}

static apkMetaRec_Ctx_t *	apk_meta_record_GetContext (void)
{
	static apkMetaRec_Ctx_t	s_recs;

	if (s_recs.semaphore == 0)
	{
		HxSEMT_Create(&s_recs.semaphore, "ApkRecordings", HxSEMT_FIFO);
		HxSEMT_Get(s_recs.semaphore);

		s_recs.recordings = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual
									, NULL, (_HxDestroyNotify)DxRECLISTDATA_Delete);
		HxLOG_Assert(s_recs.recordings);
		s_recs.recvTable  = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, NULL);
		HxLOG_Assert(s_recs.recvTable);
		s_recs.urlMap     = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, (_HxDestroyNotify)APK_Free_CB);
		HxLOG_Assert(s_recs.urlMap);
		s_recs.cidVector  = HLIB_VECTOR_New(NULL, NULL);
		HxLOG_Assert(s_recs.cidVector);

		s_recs.bEnableLoadEvent = FALSE;
		s_recs.bFirstLoadDone = FALSE;		
	}
	else
	{
		HxSEMT_Get(s_recs.semaphore);
	}

	return &s_recs;
}

static void	apk_meta_record_ReleaseContext (apkMetaRec_Ctx_t *ctx)
{
	HxSEMT_Release(ctx->semaphore);
}

STATIC void	apk_meta_record_freeEmitEvent(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;
	HUINT32 	ulCid;

	HxLOG_Assert(nArgc == 5);

	pvData = apArgV[3];
	if (pvData)
	{
		ulCid =apk_meta_record_GetCID((HCHAR *)pvData);
		if(eAPK_META_REC_EVENT_DELETED ==(HUINT32)apArgV[2])
		{
			apkMetaRec_Ctx_t	*ctx;
			ctx = apk_meta_record_GetContext();

			(void)apk_meta_record_RemoveCid(ctx, ulCid);

			HxLOG_Debug("delete (cid:%u)\n", ulCid);
			apk_meta_record_ReleaseContext(ctx);
		}
		APK_Free(pvData);
	}
}

static void		apk_meta_record_EmitEvent (HINT32 event, const HCHAR *url)
{
	ApkMetaRecord_Notifier_t	notifier = s_port_notifier;
	void *				userdata = s_port_userdata;
	APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

	// 함수니까 그냥 세마포 무시하자
	if (notifier)
	{
		switch(event)
		{
		case eAPK_META_REC_DO_SCAN:
			HxLOG_Info("%s(eAPK_META_REC_DO_SCAN)\n", __FUNCTION__);
			break;

		case eAPK_META_REC_EVENT_ADDED:
			HxLOG_Info("%s(eAPK_META_REC_EVENT_ADDED, url:%s)\n", __FUNCTION__, url);
			break;

		case eAPK_META_REC_EVENT_DELETED:
			HxLOG_Info("%s(eAPK_META_REC_EVENT_DELETED, url:%s)\n", __FUNCTION__, url);
			break;

		case eAPK_META_REC_EVENT_UPDATED:
			HxLOG_Info("%s(eAPK_META_REC_EVENT_UPDATED, url:%s)\n", __FUNCTION__, url);
			break;
		default:
			HxLOG_Info("%s(event:%d, url:%s)\n", __FUNCTION__, event, url);
			break;
		}
		#if 0
		if (url)
		{
			DxRecListData_t *rec = APK_META_RECORD_GetByUrl(NULL, url);
			if (rec)
			{
				DLIB_Print(DxNAMEOF(DxRecListData_t), NULL, rec);
				APK_META_RECORD_Release(rec);
			}
		}
		#endif


		stApkEvtArgs.pfnFreeFunc = apk_meta_record_freeEmitEvent;
		stApkEvtArgs.ulArgCount = 5;
		stApkEvtArgs.apArgV[0] = (void *)notifier;
		stApkEvtArgs.apArgV[1] = (void *)NULL;
		stApkEvtArgs.apArgV[2] = (void *)event;
		stApkEvtArgs.apArgV[3] = (void *)APK_StrDup(url);
		stApkEvtArgs.apArgV[4] = (void *)userdata;
		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

HCHAR *	apk_meta_record_GetURL (HUINT32 cid, HCHAR *buf, HUINT32 n)
{
	apkMetaRec_Ctx_t	*ctx;
	HCHAR			*url;

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%u)\n", __FUNCTION__, cid);

	if (buf==NULL || n==0)
	{
		HxLOG_Error("invalid param. buf(0x%x) n(%d) \n",buf, n);
		return NULL;
	}

	ctx = apk_meta_record_GetContext();
	url = (HCHAR *)HLIB_HASH_Lookup(ctx->urlMap, (void *)cid);
	if (url)
	{
		url = HLIB_STD_StrNCpySafe(buf, url, n);
	}
	apk_meta_record_ReleaseContext(ctx);
	return url;
}

static HBOOL apk_meta_record_IsLoadEvent(APK_META_REC_CMD_e event, const HUINT32 *ids,HINT32 count)
{
	HINT32				i;
	apkMetaRec_Ctx_t	*ctx;

	ctx = apk_meta_record_GetContext();

	if(ctx->bEnableLoadEvent == FALSE)
	{
		HxLOG_Error("bEnableLoadEvent == FALSE!!\n");
		apk_meta_record_ReleaseContext(ctx);
		return FALSE;
	}
	//pvr-hdd 의 mount 이후 녹화물 scan 완료 이벤트
	if(event == eAPK_META_REC_EVENT_ADDED && ids == NULL)
	{
		HxLOG_Warning("**** eAPK_META_REC_EVENT_LOAD-(SCAN_DONE)****\n");
		apk_meta_record_ReleaseContext(ctx);
		apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_LOAD, NULL);
		return TRUE;
	}

	//pvr-hdd 의umount 으로 인한 녹화물의 reload 요청
	if(event == eAPK_META_REC_EVENT_DELETED && count > 1)
	{
	    for (i = 0 ; i < count ; i++)
		{
			apk_meta_record_RemoveCid(ctx, ids[i]);
	    }
		HxLOG_Warning("**** eAPK_META_REC_EVENT_LOAD-(PVR_REMOVED) ****\n");
		apk_meta_record_ReleaseContext(ctx);
		apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_LOAD, NULL);
	    return TRUE;
	}

	apk_meta_record_ReleaseContext(ctx);

	return FALSE;
}

static void 	apk_meta_record_PortNotifier (void *userdata, HINT32 variation, HINT32 count, const HUINT32 *ids, HINT32 unused, ...)
{
	HINT32	i;
	HINT32	event;
	HCHAR	*url, buf[DxRECLIST_URL];

	HxLOG_Trace();

/*
	// eAPK_META_REC_DO_SCAN 사용하지 않음.. ids == NULL 이것을 LOAD 이벤트로 사용함

	if (ids == NULL)	// ids 가 NULL이면 전부 업뎃 하라고 하자
	{
		apk_meta_record_EmitEvent(eAPK_META_REC_DO_SCAN, NULL);
		return;
	}
*/

	if (variation > 0)
	{
		event = eAPK_META_REC_EVENT_ADDED;
	}
	else if (variation < 0)
	{
		event = eAPK_META_REC_EVENT_DELETED;
	}
	else
	{
		event = eAPK_META_REC_EVENT_UPDATED;
	}

	if(apk_meta_record_IsLoadEvent(event,ids,count)== TRUE)
		return;

	for (i = 0 ; i < count ; i++)
	{
		url = apk_meta_record_GetURL(ids[i], buf, DxRECLIST_URL);
		if (url == NULL)
		{
			HxLOG_Error("%s() Cannot get URL by cid:%u\n", __FUNCTION__, ids[i]);
			continue;
		}
		apk_meta_record_EmitEvent(event, url);
	}
}

static void	apk_meta_record_ScanTimer (unsigned long TimerId, void *userdata)
{
	apk_meta_record_EmitEvent((HINT32)userdata, NULL);
}

static HBOOL	apk_meta_record_CompareSeriesID (const DxRecListData_t *rec, const HCHAR *seriesID)
{
	return strcmp(rec->stEvt.aucSeriesId, seriesID) == 0 ? TRUE : FALSE;
}

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
static HBOOL	apk_meta_record_CompareSeriesCRID (const DxRecListData_t *rec, const HCHAR *seriesCRID)
{
	HUINT32 i;
	for (i=0; i<DxMAX_CRID_DATA_NUM; i++)
	{
		if (rec->stMeta.astTvaCrids[i].ucType == DxCRID_TYPE_SERIES)
		{
			return strcmp(rec->stMeta.astTvaCrids[i].szCrid, seriesCRID) == 0? TRUE : FALSE;
		}
	}
	return FALSE;
}
#endif

static void	apk_meta_record_ASyncReturn (HERROR err, void *userdata, ...)
{
	if (err == ERR_OK)
	{
		apk_meta_record_EmitEvent((HINT32)userdata, NULL);
	}
	else
	{
		switch((HINT32)userdata)
		{
		case eAPK_META_REC_DO_COPY:
			HxLOG_Error("%s(eAPK_META_REC_DO_COPY) FAIL!!\n", __FUNCTION__);
			break;
		default:
			HxLOG_Error("%s(unknown:%d) FAIL!!\n", __FUNCTION__, (int)userdata);
			break;
		}
	}
}

HERROR	apk_meta_record_Lock (HUINT32 cid, HBOOL lockOrNot)
{
	apkMetaRec_Ctx_t	*ctx;
	DxRecListData_t *rec;

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%u, lock:%s)\n", __FUNCTION__, cid, lockOrNot ? "LOCK" : "UNLOCK");

	ctx = apk_meta_record_GetContext();
	if ((rec = (DxRecListData_t *)HLIB_HASH_Lookup(ctx->recordings, (void *)cid)) == NULL)
	{
		HxLOG_Error("%s() There is no recording matched with (cid:%u)!\n", __FUNCTION__, cid);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	if (rec->bBlocked == lockOrNot)
	{
		HxLOG_Error("%s() Nothing to be changed! (cid:%u)!\n", __FUNCTION__, cid);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	rec->bUserLock = TRUE;
	apk_meta_record_ReleaseContext(ctx);

	OAPI_META_RECORD_Lock(cid, lockOrNot);
	return ERR_OK;
}

HUINT32	apk_meta_record_GetCID (const HCHAR *url)
{
	apkMetaRec_Ctx_t	*ctx;
	HUINT32			cid;
	HCHAR			*save;
	HxHASH_TableIter_t	iter;

	HxLOG_Trace();
	HxLOG_Assert(url);
	HxLOG_Debug("%s(url:%s)\n", __FUNCTION__, url);

	cid = 0;
	ctx = apk_meta_record_GetContext();
	HLIB_HASH_IterInit(&iter, ctx->urlMap);
	while (HLIB_HASH_IterNext(&iter, (void **)&cid, (void **)&save))
	{
		if (HxSTD_StrCmp(url, save) == 0)
			break;
	}
	apk_meta_record_ReleaseContext(ctx);
	return cid;
}

HERROR	apk_meta_record_Rename (HUINT32 cid, const HCHAR *name)
{
	apkMetaRec_Ctx_t	*ctx;
	DxRecListData_t *rec;

	HxLOG_Trace();
	HxLOG_Assert(name);
	HxLOG_Debug("%s(cid:%u, name:%s)\n", __FUNCTION__, cid, name);

	ctx = apk_meta_record_GetContext();
	if ((rec = (DxRecListData_t *)HLIB_HASH_Lookup(ctx->recordings, (void *)cid)) == NULL)
	{
		HxLOG_Error("%s() There is no recording matched with (cid:%u)!\n", __FUNCTION__, cid);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	if (HxSTD_StrCmp(rec->szName, name) == 0)
	{
		HxLOG_Error("%s() name(%s) does not changed!\n", __FUNCTION__, name);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	HLIB_STD_StrNCpySafe(rec->szName, name, DxRECLIST_NAME_LEN);
	// TODO: file path 변경 될 수 있음
	apk_meta_record_ReleaseContext(ctx);

	OAPI_META_RECORD_Rename(cid, name);
	return ERR_OK;
}

DxRecListData_t *apk_meta_record_GetRecording (HUINT32 cid)
{
	apkMetaRec_Ctx_t	*ctx;
	DxRecListData_t	*rec;

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%u)\n", __FUNCTION__, cid);

	ctx  = apk_meta_record_GetContext();
	rec  = (DxRecListData_t *)HLIB_HASH_Lookup(ctx->recordings, (void *)cid);
	if (rec)
		rec = DxRECLISTDATA_Clone(rec);
	apk_meta_record_ReleaseContext(ctx);

	return rec;
}

HERROR	apk_meta_record_Update (const DxRecListData_t *rec)
{
	apkMetaRec_Ctx_t	*ctx;
	DxRecListData_t *clone;

	HxLOG_Trace();
	HxLOG_Assert(rec);
	HxLOG_Debug("%s(rec:%u.%s.\"%s\")\n", __FUNCTION__, rec->ulContentId, rec->szUrl, rec->szName);

	ctx = apk_meta_record_GetContext();
	if (HLIB_HASH_Lookup(ctx->recordings, (void *)rec->ulContentId) == NULL)
	{
		HxLOG_Error("%s() There is no recording matched with (rec:%u.%s.\"%s\")!\n"
					, __FUNCTION__, rec->ulContentId, rec->szUrl, rec->szName);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	clone = DxRECLISTDATA_Clone(rec);
	if (clone == NULL)
	{
		HxLOG_Error("%s() Out of memory! (rec:%u.%s.\"%s\")!\n"
					, __FUNCTION__, rec->ulContentId, rec->szUrl, rec->szName);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	(void)apk_meta_record_UpdateCid(ctx, clone->ulContentId, clone);
	apk_meta_record_ReleaseContext(ctx);

	OAPI_META_RECORD_Update(rec);
	return ERR_OK;
}

HERROR	apk_meta_record_RemoveAsync (HUINT32 cid,  void *userdata)
{
	apkMetaRec_Ctx_t	*ctx;
	HINT32				nRpcHandle = 0;

	OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%u)\n", __FUNCTION__, cid);

	ctx = apk_meta_record_GetContext();
	if (HLIB_HASH_Lookup(ctx->recordings, (void *)cid) == NULL)
	{
		HxLOG_Error("%s() There is no recording matched with (cid:%u)!\n", __FUNCTION__, cid);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	apk_meta_record_ReleaseContext(ctx);
	OAPI_META_RECORD_Remove(cid, NULL, userdata);

	return ERR_OK;
}

HERROR	apk_meta_record_Remove (HUINT32 cid, HxReturnFunc_t ret, void *userdata)
{
	apkMetaRec_Ctx_t	*ctx;
	HERROR			err;
	HINT32	nRpcHandle = 0;
	HINT32	nRequestId = 0;

	OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%u)\n", __FUNCTION__, cid);

	ctx = apk_meta_record_GetContext();
	if (HLIB_HASH_Lookup(ctx->recordings, (void *)cid) == NULL)
	{
		HxLOG_Error("%s() There is no recording matched with (cid:%u)!\n", __FUNCTION__, cid);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	(void)apk_meta_record_RemoveCid(ctx,cid);
	apk_meta_record_ReleaseContext(ctx);

	if (ret)
	{
		err = ERR_OK;
		OAPI_META_RECORD_Remove(cid, ret, userdata);
	}
	else
	{
		HERROR	*perr = NULL;

		err = ERR_FAIL;
		nRequestId = OAPI_META_RECORD_Remove(cid, apk_meta_record_SyncedReturn, &perr);
		if (HLIB_RPC_Join(nRpcHandle, nRequestId, 10000) == ERR_FAIL)
			return ERR_FAIL;

		if (perr)
		{
			err = *perr;
			APK_Free(perr);
		}
	}
	return err;
}

HERROR	apk_meta_record_AddBookmark (HUINT32 cid, HUINT32 pos, const HCHAR *name)
{
	apkMetaRec_Ctx_t	*ctx;
	DxRecListData_t *rec;
	DxRecList_Bookmark_t	*bookmark;

	HxLOG_Trace();
	HxLOG_Assert(name);
	HxLOG_Debug("%s(cid:%u, pos:%u, name:%s)\n", __FUNCTION__, cid, pos, name);

	ctx = apk_meta_record_GetContext();
	if ((rec = (DxRecListData_t *)HLIB_HASH_Lookup(ctx->recordings, (void *)cid)) == NULL)
	{
		HxLOG_Error("%s() There is no recording matched with (cid:%u)!\n", __FUNCTION__, cid);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}

	if (rec->usNumOfBookmark == DxRECLIST_NUM_BOOKMARK)
	{
		HxLOG_Error("%s() bookmark overflow!\n", __FUNCTION__);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}

	bookmark = rec->astBookmarks + rec->usNumOfBookmark++;
	bookmark->ulPos = pos;
	HLIB_STD_StrNCpySafe(bookmark->szName, name, DxRECLIST_BOOKMARK_NAME_LEN);

	if (rec->usNumOfBookmark > 1)
	{
		qsort(rec->astBookmarks, rec->usNumOfBookmark, sizeof(DxRecList_Bookmark_t), (void *)apk_meta_record_CompareBookmark);
	}
	apk_meta_record_ReleaseContext(ctx);

	OAPI_META_RECORD_AddBookmark(cid, pos, name);
	return ERR_OK;
}

HERROR	apk_meta_record_RemoveBookmark (HUINT32 cid, HUINT32 pos, const HCHAR *name)
{
	apkMetaRec_Ctx_t	*ctx;
	DxRecListData_t *rec;
	DxRecList_Bookmark_t	key;
	DxRecList_Bookmark_t	*bookmark;

	HxLOG_Trace();
	HxLOG_Assert(name);
	HxLOG_Debug("%s(cid:%u, pos:%u, name:%s)\n", __FUNCTION__, cid, pos, name);

	ctx = apk_meta_record_GetContext();
	if ((rec = (DxRecListData_t *)HLIB_HASH_Lookup(ctx->recordings, (void *)cid)) == NULL)
	{
		HxLOG_Error("%s() There is no recording matched with (cid:%u)!\n", __FUNCTION__, cid);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}

	if (rec->usNumOfBookmark == 0)
	{
		HxLOG_Error("%s() bookmark underflow!\n", __FUNCTION__);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}

	key.ulPos = pos;
	HLIB_STD_StrNCpySafe(key.szName, name, DxRECLIST_BOOKMARK_NAME_LEN);

	bookmark = (DxRecList_Bookmark_t *)bsearch(&key, rec->astBookmarks, rec->usNumOfBookmark
						, sizeof(DxRecList_Bookmark_t), (void *)apk_meta_record_CompareBookmark);
	if (bookmark == NULL)
	{
		HxLOG_Error("%s() There is no bookmark matched with (pos:%u, name:%s)!\n", __FUNCTION__, pos, name);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	bookmark->ulPos = 0xFFFFFFFF;
	if (rec->usNumOfBookmark > 1)
	{
		qsort(rec->astBookmarks, rec->usNumOfBookmark, sizeof(DxRecList_Bookmark_t), (void *)apk_meta_record_CompareBookmark);
	}
	rec->usNumOfBookmark--;
	apk_meta_record_ReleaseContext(ctx);

	OAPI_META_RECORD_RemoveBookmark(cid, pos, name);
	return ERR_OK;
}

DxRecListData_t* apk_meta_record_GetRecordingAt (HINT32 index)
{
	apkMetaRec_Ctx_t	*ctx;
	DxRecListData_t	*rec;
	DxRecListData_t	*clone;

	HxLOG_Trace();

	/**
	 * INDEX로 접근하다니.. ㅠㅠ
	 * 일단 새로운 appkit 코드 사용해서 포팅하며, 혹시나 퍼포먼스가 떨어질지 몰라
	 * 약간의 캐시를 넣음
	 */

	ctx = apk_meta_record_GetContext();
	if (ctx->tempCache == NULL)
	{
		ctx->tempCache = apk_meta_record_GetRecordings(ctx, NULL, NULL);
		if (ctx->tempCache == NULL)
		{
			apk_meta_record_ReleaseContext(ctx);
			return NULL;
		}
	}

	clone = NULL;
	rec   = HLIB_VECTOR_ItemAt(ctx->tempCache, index);
	if (rec)
		clone = DxRECLISTDATA_Clone(rec);

	if (rec == NULL || index == HLIB_VECTOR_Length(ctx->tempCache) - 1/*last item*/)
	{
		HLIB_VECTOR_Delete(ctx->tempCache);
		ctx->tempCache = NULL;
	}
	apk_meta_record_ReleaseContext(ctx);

	return clone;
}


#if defined(CONFIG_DEBUG)
typedef struct
{
	HINT32	nStartCount;
	HINT32	nRecordingCount;
	HINT32	nRecordedCount;
	HINT32	nIncompleteCount;
} apkMetaRec_RecCount_t;

HINT32 apk_meta_record_ShowRec(void *pvArg)
{
	apkMetaRec_Ctx_t	*ctx = NULL;
	HUINT32				cid = 0;
	HCHAR				*save = NULL;
	HxHASH_TableIter_t	iter;
	DxRecListData_t 	*rec = NULL;

	ctx = apk_meta_record_GetContext();
	HLIB_HASH_IterInit(&iter, ctx->urlMap);

	HLIB_CMD_Printf("\n----------------UrlMap-----------------\n");
	while (HLIB_HASH_IterNext(&iter, (void **)&cid, (void **)&save))
	{
		HLIB_CMD_Printf("ContentId : %d, URL : %s\n",cid, save);
	}

	HLIB_CMD_Printf("\n------------------ Recorings --------------\n");
	HLIB_HASH_IterInit(&iter, ctx->recordings);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
	{
		HLIB_CMD_Printf("ContentId : %d, Name : %s, URL : %s\n",rec->ulContentId, rec->szName, rec->szUrl);
	}
	apk_meta_record_ReleaseContext(ctx);

	return HxCMD_OK;

}

static HBOOL	apk_meta_PrintRecInfo(void *data, void *userData)
{
	DxRecListData_t 	*pRec = (DxRecListData_t *)data;
	apkMetaRec_RecCount_t *pRecCount = (apkMetaRec_RecCount_t *)userData;
	HxDATETIME_t	stStartTime, stEndTime;
	UNIXTIME		utStartTime, utEndTime;
	HCHAR			cStatus = ' ';

	utStartTime = pRec->ulRecStartTime;
	utEndTime	= pRec->ulRecStartTime + pRec->ulRecDuration;

	HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime,&stStartTime);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(utEndTime,&stEndTime);

	if( pRec->eRecStatus == eDxREC_STS_RECORDING )
	{
		cStatus = 'R';
		pRecCount->nRecordingCount++;
	}
	else if( pRec->eRecStatus == eDxREC_STS_RECORDED )
	{
		cStatus = 'C';
		pRecCount->nRecordedCount++;
	}
	else if( pRec->eRecStatus == eDxREC_STS_INCOMPLETED )
	{
		cStatus = 'I';
		pRecCount->nIncompleteCount++;
	}
	else
	{
		cStatus = 'S';
		pRecCount->nStartCount++;
	}

	HLIB_CMD_Printf("[%3d] [%c] [%02d] %04d/%02d/%02d (%02d:%02d:%02d ~ %02d:%02d:%02d) [%d][%-20s][%d]",
				pRec->ulContentId, cStatus, pRec->eRecEndReason,
				stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay,
				stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond,
				stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond,
				pRec->svcUId, pRec->szName, pRec->eCasType);

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HLIB_CMD_Printf("[%s][%d - %s]", pRec->stMeta.astTvaCrids[0].szCrid, pRec->nSvcNo, pRec->szSvcName);
#endif
	HLIB_CMD_Printf("\n");

	return FALSE;
}

static HINT32	apk_meta_record_SortByStartTime (const void *pvArg1, const void *pvArg2)
{
	DxRecListData_t *pstRec1 = *(DxRecListData_t **)pvArg1;
	DxRecListData_t *pstRec2 = *(DxRecListData_t **)pvArg2;

	return (pstRec1->ulRecStartTime - pstRec2->ulRecStartTime);
}

HINT32 apk_meta_record_RecInfo(void *pvArg)
{
	apkMetaRec_Ctx_t		*ctx = NULL;
	HxHASH_TableIter_t		iter;
	DxRecListData_t			*rec = NULL;
	DxRecListData_t			*clone = NULL;	
	HCHAR					*aucArg = NULL;
	HINT32					nTotalCount = 0;
	HCHAR					szCRID[DxPVR_TVANYTIME_CRID_TOTAL_LEN] = {0, };
	HxVector_t				*pstVector = HLIB_VECTOR_NewEasy(0, (HxFreeFunc_t)DxRECLISTDATA_Delete, NULL);
	apkMetaRec_RecCount_t	stRecCount = {0, };

	/* 1st argument - CRID Filter*/
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%s", szCRID) == 0 )
		{
			HLIB_CMD_Printf("USAGE : recinfo [series_all or CRID]\n");
			return HxCMD_ERR;
		}
	}

	HLIB_CMD_Printf("\n\n-------------------------------------------- Recorings -----------------------------------------------\n");
	HLIB_CMD_Printf("[CID] [Rec Status] [End Reason] Date (Start_Time ~ End_Time) [Svc UID][Name][CasType]");
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HLIB_CMD_Printf("[CRID][LCN - Svc Name]");
#endif
	HLIB_CMD_Printf("\n");
	HLIB_CMD_Printf("-------------------------------------------------------------------------------------------------------\n");

	ctx = apk_meta_record_GetContext();
	HLIB_HASH_IterInit(&iter, ctx->recordings);
	while(HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		// if( arg1 ) then CRID Filtering
		if(HxSTD_StrLen(szCRID) > 0)
		{
			if(HxSTD_StrCmp(szCRID, "series_all") == 0)
			{
				if(rec->bSeriesRec != TRUE)
					continue;
			}
			else
			{
				if(HxSTD_StrNCmp(szCRID, rec->stMeta.astTvaCrids[0].szCrid, DxPVR_TVANYTIME_CRID_TOTAL_LEN) != 0)
					continue;
			}
		}
#endif
		clone = DxRECLISTDATA_Clone(rec);
		if(clone)
		{
			(void)HLIB_VECTOR_Add(pstVector, (void *)clone);
			nTotalCount++;
		}
	}
	apk_meta_record_ReleaseContext(ctx);

	(void)HLIB_VECTOR_Sort(pstVector, apk_meta_record_SortByStartTime);
	(void)HLIB_VECTOR_ForEach(pstVector, (HBOOL(*)(void *,void *))apk_meta_PrintRecInfo, &stRecCount);

	HLIB_CMD_Printf("-------------------------------------------------------------------------------------------------------\n");
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	if(HxSTD_StrLen(szCRID) > 0)
	{
		HLIB_CMD_Printf("Filter CRID : [%s]\n", szCRID);
	}
#endif
	HLIB_CMD_Printf("Total:[%d] - [%d/%d/%d/%d]    [S]tart/[R]ecording/[C]omplete/[I]ncomplete\n", nTotalCount, stRecCount.nStartCount, stRecCount.nRecordingCount, stRecCount.nRecordedCount, stRecCount.nIncompleteCount);
	HLIB_CMD_Printf("-------------------------------------------------------------------------------------------------------\n\n");

	HLIB_VECTOR_Delete(pstVector);

	return HxCMD_OK;
}

void	apk_meta_record_InitCommand(void)
{
#define	hCmdHandle		"APPKIT"
	HLIB_CMD_RegisterWord (hCmdHandle, apk_meta_record_ShowRec, "showrec", "show recording list", "showrec");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_meta_record_RecInfo, "recinfo", "print recordings info", "recinfo [CRID]");
}
#endif


#define	________GLOBAL_________

HBOOL	APK_META_RECORD_Init(void)
{
	apkMetaRec_Ctx_t	*ctx;
	HBOOL			bMainAppReady=FALSE;

	HxLOG_Trace();

	ctx = apk_meta_record_GetContext();

	OAPI_META_RECORD_SetListener(apk_meta_record_OapiNotifier, NULL);

	APK_POWER_GetMainAppReady(&bMainAppReady);
	if(bMainAppReady == TRUE)
	{
		/* bMainAppReady flags is controlled in PAMA, and this becomes TRUE when WebApp(corsair) is started */
		/* If WebApp(corsair) is restarted because of unexpected reason except Reboot and ColdBoot in operation,*/
		/* recording list must be loaded this time */
		apk_meta_record_Load(ctx, 0);
	}
	apk_meta_record_ReleaseContext(ctx);

#if defined(CONFIG_DEBUG)
	apk_meta_record_InitCommand();
#endif

	return ERR_OK;
}

HBOOL	APK_META_RECORD_DeInit(void)
{
	apkMetaRec_Ctx_t	*ctx;

	HxLOG_Trace();

	ctx = apk_meta_record_GetContext();

	OAPI_META_RECORD_SetListener(NULL, NULL);

	HLIB_HASH_RemoveAll(ctx->recordings);
	HLIB_HASH_RemoveAll(ctx->recvTable);
	HLIB_HASH_RemoveAll(ctx->urlMap);
	HLIB_VECTOR_Delete(ctx->tempCache);
	ctx->tempCache = NULL;
	ctx->notifier = NULL;
	ctx->userdata = NULL;
	apk_meta_record_ReleaseContext(ctx);

	return TRUE;
}

HUINT32	APK_META_RECORD_GetCID (const HCHAR *url)
{
	return apk_meta_record_GetCID(url);
}

void	APK_META_RECORD_AddNotifier(ApkMetaRecord_Notifier_t notifier, void *userdata)
{
	apkMetaRec_Ctx_t	*ctx;
	HxLOG_Trace();

	s_port_notifier = notifier;
	s_port_userdata = userdata;

	HxLOG_Debug("%s(notifier:0x%X, userdata:0x%X)\n", __FUNCTION__, (int)notifier, (int)userdata);

	ctx = apk_meta_record_GetContext();

	ctx->notifier = (APK_META_REC_Notifier_t)apk_meta_record_PortNotifier;
	ctx->userdata = userdata;

	apk_meta_record_ReleaseContext(ctx);
}

HERROR	APK_META_RECORD_Scan(HCHAR *url, HBOOL bSyncmode)
{
	HINT32	event = eAPK_META_REC_DO_SCAN;
	HINT32	nResult = 0;

	HxLOG_Trace();
	HxLOG_Debug("%s(path:\"%s\")\n", __FUNCTION__, url);

	if (!bSyncmode)
	{
		// 항상 caching되어 있을 거임 (약 100ms 뒤에 호출하자)
		nResult = VK_TIMER_EventAfter(100, apk_meta_record_ScanTimer, &event, sizeof(event), &s_ulScanTimerId);
		if(nResult != VK_OK)
		{
			HxLOG_Error("VK_TIMER_EventAfter fail!! \n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}


HERROR	APK_META_RECORD_Lock(APK_META_REC_LOCK_t	*pstLock, HBOOL	bSyncmode)
{
	HERROR	err = ERR_OK;
	HUINT32	i;
	HUINT32	cid;

	HxLOG_Trace();
	HxLOG_Assert(pstLock && pstLock->ulParamCount > 0);

	for (i = 0 ; i < pstLock->ulParamCount ; i++)
	{
		cid = apk_meta_record_GetCID(pstLock->paramList[i].szUrl);
		if (cid == 0)
			continue;

		err |= apk_meta_record_Lock(cid, pstLock->paramList[i].bLock);

		if (err == ERR_OK/* && !bSyncmode*/)
		{
			apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_UPDATED, pstLock->paramList[i].szUrl);
		}
	}
	return err;
}


HERROR	APK_META_RECORD_SetSeriesLock(HCHAR	*pSeriesID, HBOOL	bSeriesLock, HBOOL	bSyncmode)
{
	HxVector_t		*list;
	DxRecListData_t	*rec;
	HERROR			err = ERR_OK;
	apkMetaRec_Ctx_t	*ctx;

	HxLOG_Trace();
	HxLOG_Assert(pSeriesID);


	ctx  = apk_meta_record_GetContext();
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	list = apk_meta_record_GetRecordings(ctx, (HxEqualFunc_t)apk_meta_record_CompareSeriesCRID, pSeriesID);
#else
	list = apk_meta_record_GetRecordings(ctx, (HxEqualFunc_t)apk_meta_record_CompareSeriesID, pSeriesID);
#endif
	apk_meta_record_ReleaseContext(ctx);
	if (list == NULL)
	{
		HxLOG_Error("%s(%s -> %s) Failed!\n", __FUNCTION__, pSeriesID, bSeriesLock ? "LOCK" : "UNLOCK");
		return ERR_FAIL;
	}

	HxVECTOR_EACH(list, DxRecListData_t *, rec, {
		if (rec->bBlocked != bSeriesLock)
		{
			err |= apk_meta_record_Lock(rec->ulContentId, bSeriesLock);
			if (err == ERR_OK/* && !bSyncmode*/)
			{
				apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_UPDATED, rec->szUrl);
			}
		}
	});
	HLIB_VECTOR_Delete(list);
	return err;
}

HERROR	APK_META_RECORD_Rename(APK_META_REC_RENAME_t	*pstRename, HBOOL	bSyncmode)
{
	HERROR	err = ERR_OK;
	HUINT32	i;
	HUINT32	cid;

	HxLOG_Trace();
	HxLOG_Assert(pstRename && pstRename->ulParamCount > 0);

	for (i = 0 ; i < pstRename->ulParamCount ; i++)
	{
		cid = apk_meta_record_GetCID(pstRename->paramList[i].szUrl);
		if (cid == 0)
			continue;

		err |= apk_meta_record_Rename(cid, pstRename->paramList[i].szRename);
		if (err == ERR_OK/* && !bSyncmode*/)
		{
			apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_UPDATED, pstRename->paramList[i].szUrl);
		}
	}

	(void)bSyncmode;
	return err;
}

HERROR	APK_META_RECORD_Update(APK_META_REC_UPDATE_t	*pstUpdate, HBOOL	bSyncmode)
{
	HERROR	err = ERR_OK;
	HUINT32	i,j;
	HUINT32	cid;
	DxRecListData_t	*rec;

	HxLOG_Trace();
	HxLOG_Assert(pstUpdate && pstUpdate->ulParamCount > 0);

	for (i = 0 ; i < pstUpdate->ulParamCount ; i++)
	{
		cid = apk_meta_record_GetCID(pstUpdate->paramList[i].szUrl);
		if (cid == 0)
			continue;

		rec = apk_meta_record_GetRecording(cid);
		if (rec == NULL)
			continue;

		for (j=0; j<pstUpdate->paramList[i].nNumOfTags; j++)
		{
			if (strcmp(pstUpdate->paramList[i].szTags[j],DxRECLIST_KEEP_TAG)==0)
			{
				rec->bDoNotDelete			= pstUpdate->paramList[i].recUpdateData.bDoNotDelete;
				rec->ulDoNotDeleteSetTime	= pstUpdate->paramList[i].recUpdateData.ulDoNotDeleteSetTime;
			}
			else if (strcmp(pstUpdate->paramList[i].szTags[j],DxRECLIST_LOCK_TAG) == 0)
			{
				rec->bUserLock  = pstUpdate->paramList[i].recUpdateData.bUserLock;
				rec->bBlocked  = pstUpdate->paramList[i].recUpdateData.bBlocked;
#if defined (CONFIG_OP_SES)
				rec->ucas.ucSCPinCodeFlag  = pstUpdate->paramList[i].recUpdateData.ucas.ucSCPinCodeFlag;
#endif
			}
		}

		err |= apk_meta_record_Update(rec);
		if (err == ERR_OK/* && !bSyncmode*/)
		{
			apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_UPDATED, rec->szUrl);
		}
		DxRECLISTDATA_Delete(rec);
	}

	(void)bSyncmode;
	return err;
}

HERROR	APK_META_RECORD_StopRecording(APK_META_REC_STOP_REC_t	*pstStopRec, HBOOL	bSyncmode)
{
	HERROR	err = ERR_OK;
	HUINT32	i;
	HUINT32	cid;
	DxRecListData_t	*rec;
	HUINT32	svcUId;

	HxLOG_Trace();
	HxLOG_Assert(pstStopRec && pstStopRec->ulParamCount > 0);

	for (i = 0 ; i < pstStopRec->ulParamCount ; i++)
	{
		cid = apk_meta_record_GetCID(pstStopRec->paramList[i].szUrl);
		if (cid == 0)
			continue;

		rec = apk_meta_record_GetRecording(cid);
		if (rec == NULL)
			continue;

		svcUId = rec->svcUId;
		DxRECLISTDATA_Delete(rec);
		if (svcUId  == 0x0000 )
			continue;

		err |= APK_MEDIA_RECORD_StopByUID(svcUId);
	}

	(void)bSyncmode;
	return err;
}

HERROR	APK_META_RECORD_Copy (HUINT32 cid, const HCHAR *path, HBOOL bDecrypt,HxReturnFunc_t ret, void *userdata)
{
	apkMetaRec_Ctx_t	*ctx;
	HERROR			err;
	HINT32	nRpcHandle = 0;
	HINT32	nRequestId = 0;

	OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%u, path:\'%s\')\n", __FUNCTION__, cid, path);

	ctx = apk_meta_record_GetContext();
	if (HLIB_HASH_Lookup(ctx->recordings, (void *)cid) == NULL)
	{
		HxLOG_Error("%s() There is no recording matched with (cid:%u)!\n", __FUNCTION__, cid);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	apk_meta_record_ReleaseContext(ctx);

	if (ret)
	{
		err = ERR_OK;
		OAPI_META_RECORD_Copy(cid, path, ret, userdata);
	}
	else
	{
		HERROR	*perr = NULL;

		err = ERR_FAIL;
		nRequestId = OAPI_META_RECORD_Copy(cid, path, apk_meta_record_SyncedReturn, &perr);
		if (HLIB_RPC_Join(nRpcHandle, nRequestId, 10000) == ERR_FAIL)
			return ERR_FAIL;

		if (perr)
		{
			err = *perr;
			APK_Free(perr);
		}
	}
	return err;
}

HERROR	APK_META_RECORD_Move (HUINT32 cid, const HCHAR *path, HBOOL bDecrypt,HxReturnFunc_t ret, void *userdata)
{
	apkMetaRec_Ctx_t	*ctx;
	HERROR			err;
	HINT32	nRpcHandle = 0;
	HINT32	nRequestId = 0;

	OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%u, path:\'%s\')\n", __FUNCTION__, cid, path);

	ctx = apk_meta_record_GetContext();
	if (HLIB_HASH_Lookup(ctx->recordings, (void *)cid) == NULL)
	{
		HxLOG_Error("%s() There is no recording matched with (cid:%u)!\n", __FUNCTION__, cid);
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}
	apk_meta_record_ReleaseContext(ctx);

	if (ret)
	{
		err = ERR_OK;
		OAPI_META_RECORD_Move(cid, path, ret, userdata);
	}
	else
	{
		HERROR	*perr = NULL;

		err = ERR_FAIL;
		nRequestId = OAPI_META_RECORD_Move(cid, path, apk_meta_record_SyncedReturn, &perr);
		if (HLIB_RPC_Join(nRpcHandle, nRequestId, 10000) == ERR_FAIL)
			return ERR_FAIL;

		if (perr)
		{
			err = *perr;
			APK_Free(perr);
		}
	}
	return err;
}

HERROR	APK_META_RECORD_Delete(APK_META_REC_DELETE_t	*pstDelete, HBOOL	bSyncmode)
{
	HERROR	err = ERR_OK;
	HUINT32	i;
	HUINT32	cid;

	HxLOG_Trace();
	HxLOG_Assert(pstDelete && pstDelete->ulParamCount > 0);

	for (i = 0 ; i < pstDelete->ulParamCount ; i++)
	{
		cid = apk_meta_record_GetCID(pstDelete->paramList[i].szUrl);
		if (cid == 0)
			continue;

		if (bSyncmode)
		{
			err |= apk_meta_record_Remove(cid, NULL, NULL);
			apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_DELETED, pstDelete->paramList[i].szUrl);
		}
		else
		{
			apk_meta_record_Remove(cid, apk_meta_record_ASyncReturn, (void *)eAPK_META_REC_DO_DELETE);
		}
	}
	return err;
}

HERROR	APK_META_RECORD_DeleteAsync(APK_META_REC_DELETE_t	*pstDelete)
{
	HERROR	err = ERR_OK;
	HUINT32	i;
	HUINT32	cid;

	HxLOG_Trace();
	HxLOG_Assert(pstDelete && pstDelete->ulParamCount > 0);

	for (i = 0 ; i < pstDelete->ulParamCount ; i++)
	{
		cid = apk_meta_record_GetCID(pstDelete->paramList[i].szUrl);
		if (cid == 0)
			continue;
		apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_DELETED, pstDelete->paramList[i].szUrl);
		err |= apk_meta_record_RemoveAsync(cid, NULL);

	}
	return err;
}

HERROR	APK_META_RECORD_AddBookmark(APK_META_REC_BOOKMARK_t	*pstBookmark, HBOOL	bSyncmode)
{
	HERROR	err = ERR_OK;
	HUINT32	i;
	HUINT32	cid;

	HxLOG_Trace();
	HxLOG_Assert(pstBookmark && pstBookmark->ulParamCount > 0);

	for (i = 0 ; i < pstBookmark->ulParamCount ; i++)
	{
		cid = apk_meta_record_GetCID(pstBookmark->paramList[i].szUrl);
		if (cid == 0)
			continue;

		err |= apk_meta_record_AddBookmark(cid, pstBookmark->paramList[i].stBookmark.ulPos, pstBookmark->paramList[i].stBookmark.szName);
		if (err == ERR_OK && !bSyncmode)
		{
			apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_UPDATED, pstBookmark->paramList[i].szUrl);
		}
	}

	(void)bSyncmode;
	return err;
}

HERROR	APK_META_RECORD_RemoveBookmark(APK_META_REC_BOOKMARK_t	*pstBookmark, HBOOL	bSyncmode)
{
	HERROR	err = ERR_OK;
	HUINT32	i;
	HUINT32	cid;

	HxLOG_Trace();
	HxLOG_Assert(pstBookmark && pstBookmark->ulParamCount > 0);

	for (i = 0 ; i < pstBookmark->ulParamCount ; i++)
	{
		cid = apk_meta_record_GetCID(pstBookmark->paramList[i].szUrl);
		if (cid == 0)
			continue;

		err |= apk_meta_record_RemoveBookmark(cid, pstBookmark->paramList[i].stBookmark.ulPos, pstBookmark->paramList[i].stBookmark.szName);
		if (err == ERR_OK && !bSyncmode)
		{
			apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_UPDATED, pstBookmark->paramList[i].szUrl);
		}
	}

	(void)bSyncmode;
	return err;
}

void	APK_META_RECORD_CancelFileOperation (HUINT32 cid)
{
	apkMetaRec_Ctx_t	*ctx;

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%u)\n", __FUNCTION__, cid);

	ctx = apk_meta_record_GetContext();
	if (HLIB_HASH_Lookup(ctx->recordings, (void *)cid) == NULL)
	{
		HxLOG_Error("%s() There is no recording matched with (cid:%u)!\n", __FUNCTION__, cid);
		apk_meta_record_ReleaseContext(ctx);
		return;
	}
	apk_meta_record_ReleaseContext(ctx);

	OAPI_META_RECORD_CancelFileOperation(cid);
}

HERROR	APK_META_RECORD_Cancel(void)
{
	HxLOG_Trace();	//1 DO NOT USE
	return ERR_FAIL;
}


HERROR	APK_META_RECORD_Clear(void)
{
	apkMetaRec_Ctx_t	*ctx;

	HxLOG_Trace();

	ctx  = apk_meta_record_GetContext();
	if (HLIB_HASH_Size(ctx->recordings) == 0)
	{
		apk_meta_record_ReleaseContext(ctx);
		return ERR_FAIL;
	}

	apk_meta_record_ReleaseContext(ctx);

	OAPI_META_RECORD_RemoveAll();

	return ERR_OK;
}

DxRecListData_t*		APK_META_RECORD_GetByCID (const HUINT32 cid)
{
	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%d)\n", __FUNCTION__, cid);

	return apk_meta_record_GetRecording(cid);
}

DxRecListData_t*		APK_META_RECORD_GetByUrl (const HCHAR *url)
{
	HxLOG_Trace();
	HxLOG_Assert(url);
	HxLOG_Debug("%s(url:%s)\n", __FUNCTION__, url);

	return apk_meta_record_GetRecording(apk_meta_record_GetCID(url));
}

HUINT32		APK_META_RECORD_GetContentsIdByUrl (const HCHAR *url)
{
	HxLOG_Trace();
	HxLOG_Assert(url);
	HxLOG_Debug("%s(url:%s)\n", __FUNCTION__, url);

	return apk_meta_record_GetCID(url);
}


DxRecListData_t*		APK_META_RECORD_GetByIdx (const HUINT32	iter)
{
	HxLOG_Trace();
	return apk_meta_record_GetRecordingAt(iter);
}

void		APK_META_RECORD_Release (DxRecListData_t *recData)
{
	if (recData)
		DxRECLISTDATA_Delete(recData);
}

HUINT32		APK_META_RECORD_Count (void)
{
	apkMetaRec_Ctx_t	*ctx;
	HUINT32			count;

	HxLOG_Trace();

	ctx   = apk_meta_record_GetContext();
	count = HLIB_HASH_Size(ctx->recordings);
	apk_meta_record_ReleaseContext(ctx);

	return count;
}

void APK_META_RECORD_RemoveByStorageID(HCHAR *pUUID)
{
	apkMetaRec_Ctx_t		*ctx;
	DxRecListData_t			*rec;
	HxVector_t				*cids;
	HxHASH_TableIter_t		iter;

	HxLOG_Trace();

	ctx = apk_meta_record_GetContext();

	cids = ctx->cidVector;
	HLIB_VECTOR_RemoveAll(cids);


	HLIB_HASH_IterInit(&iter, ctx->recordings);
	apk_meta_record_ReleaseContext(ctx);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
	{
		if ((pUUID == NULL) || (HxSTD_StrCmp(rec->szStorageId,pUUID)== 0))
		{
			apk_meta_record_EmitEvent(eAPK_META_REC_EVENT_DELETED,rec->szUrl);
			HxLOG_Debug("%s(): Removed[cid:%u, url:%s, name:\"%s\"]\n", __FUNCTION__, rec->ulContentId, rec->szUrl, rec->szName);
		}
	}
}

HxVector_t	*APK_META_RECORD_GetItems(void)
{
	apkMetaRec_Ctx_t	*ctx;
	HxVector_t			*ret;
	HxLOG_Trace();

	ctx = apk_meta_record_GetContext();
	ret = apk_meta_record_GetRecordings(ctx, NULL, NULL);
	apk_meta_record_ReleaseContext(ctx);

	return ret;
}


HxVector_t *APK_META_RECORD_GetRecordings (HxEqualFunc_t pfFilter, void *pvKey)
{
	apkMetaRec_Ctx_t		*pstCtx = apk_meta_record_GetContext();
	HxVector_t				*pstVector = NULL;

	if (NULL == pstCtx)
	{
		HxLOG_Error("apk_meta_record_GetContext NULL\n");
		return NULL;
	}

	pstVector = apk_meta_record_GetRecordings(pstCtx, pfFilter, pvKey);
	apk_meta_record_ReleaseContext(pstCtx);

	return pstVector;
}

void	APK_META_RECORD_EnableLoadEvent(void)
{
	apkMetaRec_Ctx_t	*ctx;
	HxLOG_Trace();

	ctx = apk_meta_record_GetContext();

	ctx->bEnableLoadEvent = TRUE;
	apk_meta_record_ReleaseContext(ctx);

}

HBOOL	APK_META_RECORD_IsFirstScanDone(void)
{
	apkMetaRec_Ctx_t	*ctx;
	HBOOL				bRet = FALSE;
	HxLOG_Trace();

	ctx = apk_meta_record_GetContext();
	bRet = ctx->bFirstLoadDone;
	apk_meta_record_ReleaseContext(ctx);

	return bRet;
}
#endif
/* end of file */
