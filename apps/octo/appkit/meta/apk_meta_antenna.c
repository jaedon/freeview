/**************************************************************
 *	@file		sqc_output.c
 *	appkit for SQC Output
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2011/10/24
 *	@author			humax
 *	@brief			appkit for SQC Output
 **************************************************************/
#include <oapi.h>
#include <dapi.h>
#include <oapi.h>
#include <apk.h>
#include <_apk_int.h>

#define 	ENTER_CRITICAL_APKMETAANT		do { HxSEMT_Get(s_ulMetaAntSemId); } while (0)
#define	LEAVE_CRITICAL_APKMETAANT		do { HxSEMT_Release(s_ulMetaAntSemId); } while (0)

#define	____PRIVATE_TYPE___________________________________________________________________________


typedef struct
{
	HxVector_t			*list;
	HCHAR				*path;
	HUINT32				notiId;

	HxVector_t			*lastSyncList;
} apkMetaAnt_Ctx_t;


STATIC HUINT32		s_ulMetaAntSemId;

#define	____PRIVATE_API___________________________________________________________________________

static HINT32		apk_meta_antenna_CompareHINT32 (const void *arg1, const void *arg2)
{
	return (HINT32)(*(const void **)arg1) - (HINT32)(*(const void **)arg2);
}

static void	*	apk_meta_antenna_CloneObject (const void *pvSelf)
{
	DxAntInfo_t			*pstSrcAnt = (DxAntInfo_t *)pvSelf;
	DxAntInfo_t			*pstClone;

	if (NULL == pstSrcAnt)
	{
		return NULL;
	}

	pstClone = (DxAntInfo_t *)APK_Malloc(sizeof(DxAntInfo_t));
	if (pstClone)
	{
		HxSTD_MemCopy(pstClone, pstSrcAnt, sizeof(DxAntInfo_t));
	}

	return (void *)pstClone;
}

static HINT32		apk_meta_antenna_GetUniqueID (HxVector_t *list)
{
	DxAntInfo_t	*info;
	HINT32				uid;

	HxLOG_Trace();

	uid = 0;
	HxVECTOR_EACH(list, DxAntInfo_t*, info, {
		if (info && info->uid > uid)
			uid = info->uid;
	});
//	if (uid == 0)
//		return 0;
	uid++;
	if (uid == 0)
		uid++;
	return uid;
}

static DxAntInfo_t *	apk_meta_antenna_FindAntenna (HxVector_t *list, HINT32 uid)
{
	DxAntInfo_t	*info = NULL;

	HxLOG_Trace();

	HxVECTOR_EACH(list, DxAntInfo_t*, info, {
		if (info && info->uid == uid)
			return info;
	});
	return NULL;
}

static apkMetaAnt_Ctx_t	*	apk_meta_antenna_GetContext (void)
{
	static apkMetaAnt_Ctx_t	s_context ={0,};
	if (s_context.list == NULL)
	{
		s_context.list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))APK_Free_CB, apk_meta_antenna_CompareHINT32);
		HxLOG_Assert(s_context.list != NULL);
	}

	if (s_context.lastSyncList == NULL)
	{
		s_context.lastSyncList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))APK_Free_CB, apk_meta_antenna_CompareHINT32);
		HxLOG_Assert(s_context.lastSyncList != NULL);
	}

	return &s_context;
}

static void		apk_meta_antenna_OnDataReceived(const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	HxVector_t		*list = NULL;
	DxAntInfo_t		*info = NULL;
	DxAntInfo_t		*antenna = NULL;
	DAxIterator_t	iter;
	apkMetaAnt_Ctx_t	*ctx = NULL;

	HxLOG_Trace();

	if (pvData == NULL || size == 0)
	{
		HxLOG_Error("[%s] Invalid param(s).\n", __FUNCTION__);
		return;
	}

	list  = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))APK_Free_CB, NULL);
	if (list == NULL)
	{
		HxLOG_Critical("[%s] Out of memory!\n", __FUNCTION__);
		return;
	}

	DAPI_InitIterator(&iter, pvData, size, sizeof(DxAntInfo_t));
	while (DAPI_NextIterator(&iter))
	{
		info    = (DxAntInfo_t*)iter.data;
		antenna = (DxAntInfo_t*)APK_MemDup(info, sizeof(DxAntInfo_t));
		if (antenna == NULL)
			continue;
		//DLIB_Print(DxNAMEOF(DxAntInfo_t), NULL, antenna);
		HLIB_VECTOR_Add(list, antenna);
	}
	if (HLIB_VECTOR_Length(list) == 0)
	{
		HLIB_VECTOR_Delete(list);
		list = NULL;
	}

	ENTER_CRITICAL_APKMETAANT;
	ctx = apk_meta_antenna_GetContext();
	if (ctx->list)
		HLIB_VECTOR_Delete(ctx->list);
	ctx->list = list;

	if (ctx->lastSyncList)
		HLIB_VECTOR_Delete(ctx->lastSyncList);
	ctx->lastSyncList = HLIB_VECTOR_CloneEx(ctx->list, (void *(*)(void *))apk_meta_antenna_CloneObject);

	HxLOG_Debug("[%s:%d] Antenna loading done!! count:%d\n", __FUNCTION__, __LINE__, list ? list->count : 0);
	LEAVE_CRITICAL_APKMETAANT;
}

static HERROR	apk_meta_antenna_Load (void)
{
	HUINT32	id;

	HxLOG_Trace();

	if ((id = DAPI_GetAll(DxNAMEOF(DxAntInfo_t), apk_meta_antenna_OnDataReceived, NULL)) == 0)
	{
		HxLOG_Error("[%s:%d] Query error\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	if (DAPI_Wait(60000, id) == DERR_TIMEOUT)
	{
		HxLOG_Critical("[%s:%d] DAPI_Wait() timeout! \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	return ERR_OK;
}

static HERROR	apk_meta_antenna_Save (apkMetaAnt_Ctx_t *ctx, const HCHAR *inpath)
{
//	DxAntInfo_t	*info;
	HBOOL			 bChanged;
	HUINT32			 ulLastIdx, ulCurrIdx;
	HUINT32			 ulLastLen=0;
        HUINT32                  ulCurrLen=0;
	DxAntInfo_t		*pstLastAnt, *pstCurrAnt;
	HxList_t		*pstRemoved = NULL;
	HxList_t		*pstAddUpdated = NULL;

	HxLOG_Trace();
	(void)inpath;

#if 0
	if (ctx->list == NULL || HLIB_VECTOR_Length(ctx->list) == 0)
	{
		HxLOG_Warning("[%s] Empty data\n", __FUNCTION__);
		return ERR_FAIL;
	}

	DAPI_ResetTable(DxNAMEOF(DxAntInfo_t));
	DAPI_BatchBegin(DxNAMEOF(DxAntInfo_t));
	HxVECTOR_EACH(ctx->list, DxAntInfo_t *, info, {
		//DLIB_Print(DxNAMEOF(DxAntInfo_t), NULL, info);
		DAPI_Set(DxNAMEOF(DxAntInfo_t), info->uid, info, sizeof(DxAntInfo_t));
	});
	DAPI_BatchEnd(DxNAMEOF(DxAntInfo_t));
#endif
        if(ctx->list)
                ulCurrLen = HLIB_VECTOR_Length(ctx->list);
        if(ctx->lastSyncList)
                ulLastLen = HLIB_VECTOR_Length(ctx->lastSyncList);

	// Removed Antenna
	for (ulLastIdx = 0; ulLastIdx < ulLastLen; ulLastIdx++)
	{
		pstLastAnt = (DxAntInfo_t *)HLIB_VECTOR_ItemAt(ctx->lastSyncList, ulLastIdx);
		if (NULL != pstLastAnt)
		{
			bChanged = TRUE;

			for (ulCurrIdx = 0; ulCurrIdx < ulCurrLen; ulCurrIdx++)
			{
				pstCurrAnt = (DxAntInfo_t *)HLIB_VECTOR_ItemAt(ctx->list, ulCurrIdx);
				if ((NULL != pstCurrAnt) && (pstCurrAnt->uid == pstLastAnt->uid))
				{
					bChanged = FALSE;
					break;
				}
			}

			if (TRUE == bChanged)
			{
				pstRemoved = HLIB_LIST_Append(pstRemoved, apk_meta_antenna_CloneObject((const void *)pstLastAnt));
			}
		}
	}

	// Added and Updated Antenna:
	for (ulCurrIdx = 0; ulCurrIdx < ulCurrLen; ulCurrIdx++)
	{
		pstCurrAnt = (DxAntInfo_t *)HLIB_VECTOR_ItemAt(ctx->list, ulCurrIdx);
		if (NULL != pstCurrAnt)
		{
			bChanged = TRUE;

			for (ulLastIdx = 0; ulLastIdx < ulLastLen; ulLastIdx++)
			{
				pstLastAnt = (DxAntInfo_t *)HLIB_VECTOR_ItemAt(ctx->lastSyncList, ulLastIdx);
				if ((NULL != pstLastAnt) && (HxSTD_MemCmp(pstCurrAnt, pstLastAnt, sizeof(DxAntInfo_t)) == 0))
				{
					bChanged = FALSE;
					break;
				}
			}

			if (TRUE == bChanged)
			{
				pstAddUpdated = HLIB_LIST_Append(pstAddUpdated, apk_meta_antenna_CloneObject((const void *)pstCurrAnt));
			}
		}
	}

	if ((NULL != pstAddUpdated) || (NULL != pstRemoved))
	{
		DAPI_BatchBegin(DxNAMEOF(DxAntInfo_t));
		{
			HxList_t		*pstItem;
			DxAntInfo_t		*pstAntItem;

			for (pstItem = pstRemoved; NULL != pstItem; pstItem = pstItem->next)
			{
				pstAntItem = (DxAntInfo_t *)HLIB_LIST_Data(pstItem);
				if (NULL != pstAntItem)
				{
					DAPI_Reset(DxNAMEOF(DxAntInfo_t), pstAntItem->uid);
				}
			}

			for (pstItem = pstAddUpdated; NULL != pstItem; pstItem = pstItem->next)
			{
				pstAntItem = (DxAntInfo_t *)HLIB_LIST_Data(pstItem);
				if (NULL != pstAntItem)
				{
					DAPI_Set(DxNAMEOF(DxAntInfo_t), pstAntItem->uid, pstAntItem, sizeof(DxAntInfo_t));
				}
			}
		}
		DAPI_BatchEnd(DxNAMEOF(DxAntInfo_t));

		if (ctx->lastSyncList)
			HLIB_VECTOR_Delete(ctx->lastSyncList);
		ctx->lastSyncList = HLIB_VECTOR_CloneEx(ctx->list, (void *(*)(void *))apk_meta_antenna_CloneObject);
	}

	pstAddUpdated = HLIB_LIST_RemoveAllFunc(pstAddUpdated, APK_Free_CB);
	pstRemoved = HLIB_LIST_RemoveAllFunc(pstRemoved, APK_Free_CB);

	return ERR_OK;
}

STATIC void		apk_meta_antenna_OnDamaAntennaUpdated (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAPI_GetAll(name, apk_meta_antenna_OnDataReceived, NULL);
}


#define	____PROTECTED_API___________________________________________________________________________


HBOOL	APK_META_ANTENNA_Init(HBOOL bMainApp)
{
	apkMetaAnt_Ctx_t *ctx = NULL;

	HxLOG_Trace();

	HxSEMT_Create(&s_ulMetaAntSemId, "sametaant", HxSEMT_FIFO);

	ctx = apk_meta_antenna_GetContext();	// for initializing...
	(void)ctx;

	apk_meta_antenna_Load();

	if (TRUE != bMainApp)
	{
		DAPI_AddTableNotifier(DxNAMEOF(DxAntInfo_t), apk_meta_antenna_OnDamaAntennaUpdated, NULL);
	}

	return TRUE;
}

HBOOL	APK_META_ANTENNA_DeInit(void)
{
	apkMetaAnt_Ctx_t *ctx = NULL;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETAANT;

	ctx = apk_meta_antenna_GetContext();
	if (ctx)
	{
		if (ctx->notiId)
			DAPI_RemoveNotifier(ctx->notiId);
		ctx->notiId = 0;
                if(ctx->list)
                        HLIB_VECTOR_RemoveAll(ctx->list);
	}
	LEAVE_CRITICAL_APKMETAANT;
	HxSEMT_Destroy(s_ulMetaAntSemId);

	return TRUE;
}


#define	____PUBLIC_API______________________________________________________________________________


HINT32		APK_META_ANTENNA_Add (const DxAntInfo_t *antenna)
{
	apkMetaAnt_Ctx_t	*ctx = NULL;
	DxAntInfo_t	*newObj = NULL;
	HINT32          uid = 0;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETAANT;
	ctx = apk_meta_antenna_GetContext();

	newObj = (DxAntInfo_t *)HLIB_STD_MemDupEx((const void *)antenna, sizeof(DxAntInfo_t), APK_Malloc_CB);
	if (newObj == NULL)
	{
		LEAVE_CRITICAL_APKMETAANT;
		return 0;
	}
	newObj->uid = uid = apk_meta_antenna_GetUniqueID(ctx->list);
	HLIB_VECTOR_Add(ctx->list, newObj);
	LEAVE_CRITICAL_APKMETAANT;

	return uid;
}


void		APK_META_ANTENNA_RemoveAll (void)
{
	apkMetaAnt_Ctx_t	*ctx = NULL;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETAANT;
	ctx = apk_meta_antenna_GetContext();

	HLIB_VECTOR_RemoveAll(ctx->list);
	LEAVE_CRITICAL_APKMETAANT;
}

HUINT32		APK_META_ANTENNA_Count (void)
{
	apkMetaAnt_Ctx_t	*ctx = NULL;
	HINT32	len = 0;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETAANT;
	ctx = apk_meta_antenna_GetContext();
	len = HLIB_VECTOR_Length(ctx->list);
	LEAVE_CRITICAL_APKMETAANT;

	HxLOG_Debug("[%s] return Length(%d) \n", __FUNCTION__, len);

	return (HUINT32)len;
}

HINT32		APK_META_ANTENNA_GetUidAt (HINT32 index)
{
	apkMetaAnt_Ctx_t	*ctx = NULL;
	DxAntInfo_t	*info = NULL;
	HINT32          uid = 0;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETAANT;
	ctx  = apk_meta_antenna_GetContext();
	info = (DxAntInfo_t *)HLIB_VECTOR_ItemAt(ctx->list, index);
	if (info)
		uid = info->uid;
	else
		uid = 0;
	LEAVE_CRITICAL_APKMETAANT;

	HxLOG_Debug("[%s] idx(%d) uid(%d) ", __FUNCTION__, index, uid);

	return uid;
}

DxAntInfo_t * APK_META_ANTENNA_GetInfo (HINT32 uid)
{
	apkMetaAnt_Ctx_t	*ctx;
	DxAntInfo_t	*info, *clone;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETAANT;
	ctx  = apk_meta_antenna_GetContext();
	info = apk_meta_antenna_FindAntenna(ctx->list, uid);
	if (info)
		clone = HLIB_STD_MemDupEx((void *)info, sizeof(DxAntInfo_t), APK_Malloc_CB);
	else
		clone = NULL;

	LEAVE_CRITICAL_APKMETAANT;

	if ((clone == NULL) || (clone->satName == NULL))
	{
		HxLOG_Debug("[%s] uid(%d) satName(NULL)", __FUNCTION__, uid);
	}
	else
	{
		HxLOG_Debug("[%s] uid(%d) satName(%s)", __FUNCTION__, uid, clone->satName);
	}

	return clone;
}

HxVector_t *	APK_META_ANTENNA_GetList (void)
{
	apkMetaAnt_Ctx_t	*ctx = NULL;
	HxVector_t              *pstResult = NULL;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETAANT;
	ctx  = apk_meta_antenna_GetContext();
	if (ctx->list)
	{
		pstResult = HLIB_VECTOR_CloneEx(ctx->list, (void *(*)(void *))apk_meta_antenna_CloneObject);
	}
	LEAVE_CRITICAL_APKMETAANT;

	if (NULL == pstResult)
	{
		return NULL;
	}

	if (HLIB_VECTOR_Length(pstResult) == 0)
	{
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}
	return pstResult;
}

void APK_META_ANTENNA_InfoFree (DxAntInfo_t *p)
{
	ENTER_CRITICAL_APKMETAANT;
	APK_Free(p);
	LEAVE_CRITICAL_APKMETAANT;
}

void		APK_META_ANTENNA_SetInfo(HINT32 uid, const DxAntInfo_t *info)
{
	apkMetaAnt_Ctx_t        *ctx = NULL;
	DxAntInfo_t             *base = NULL;

	HxLOG_Trace();
	HxLOG_Assert(info);

	ENTER_CRITICAL_APKMETAANT;
	ctx  = apk_meta_antenna_GetContext();
	base = apk_meta_antenna_FindAntenna(ctx->list, uid);
	if (base)
	{
		HxSTD_MemCopy(base, info, sizeof(DxAntInfo_t));
		base->uid = uid;
	}
	LEAVE_CRITICAL_APKMETAANT;

	HxLOG_Debug("[%s] uid(%d) satName(%s) ", __FUNCTION__, uid, info->satName);
}

HERROR		APK_META_ANTENNA_Load (void)
{
	HxLOG_Trace();

 	return apk_meta_antenna_Load();
 }

HERROR		APK_META_ANTENNA_Save (void)
{
	HERROR err;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETAANT;
	err = apk_meta_antenna_Save(apk_meta_antenna_GetContext(), NULL);
	LEAVE_CRITICAL_APKMETAANT;
	OAPI_META_ANT_Sync();
	return err;
}

