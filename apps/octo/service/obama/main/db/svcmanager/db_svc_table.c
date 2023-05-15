/**
	@file     db_svc_table.c
	@brief    default setting 관련 API

	Description: db별, 모델 별 달라지는 default 값을 처리하는 API 모음 \n
	Module: DB/default \n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <_db_svc_table.h>
#include "_db_svc_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if	0
#define	DBSM_CALLOC(size)			HLIB_MEMDBG_Calloc(size, sizeof(char), 1, __FILE__, __LINE__);
#define	DBSM_FREE(p)				HLIB_MEMDBG_Free(p, __FILE__, __LINE__);
#define	DBSM_MEMDUP(p, size)		HLIB_MEMDBG_MemDup(p, size, 1, __FILE__, __LINE__);
#else
#define	DBSM_CALLOC(size)			HLIB_STD_MemCalloc(size)
#define	DBSM_FREE(p)				HLIB_STD_MemFree(p)
#define	DBSM_MEMDUP(p, size)		HLIB_STD_MemDup(p, size)
#endif

#define	return_if_void(expr)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return;}}while(0)
#define	return_if(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define	goto_if(expr, pos)		do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

#define	DB_SM_TABLE_HEAD_REVISION	(-1)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/**************    local function prototypes    ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC HBOOL	db_svc_TableMatchInt (const dbSvc_Table_t *pstSelf, const dbSvc_TableFilter_t *pstFilter)
{
	HINT32	i;

	if (pstFilter->ulFlags)
	{
		if (!db_svc_TableHasFlag(pstSelf, pstFilter->ulFlags))
			return FALSE;
	}
	if (pstFilter->nKey >= 0)
	{
		if (db_svc_TableGetKey(pstSelf) != pstFilter->nKey)
			return FALSE;
	}
	for (i = 0 ; i < DB_SVC_TABLE_MAX_EXT_FILTER && pstFilter->astExt[i].fnMatch ; i++)
	{
		if (!pstFilter->astExt[i].fnMatch(pstSelf, pstFilter->astExt[i].pvUserData))
			return FALSE;
	}

	return TRUE;
}

STATIC void	db_svc_TableListRemoveIndexMap (dbSvc_TableList_t *pstSelf)
{
	HINT32	i;
	for (i = 0 ; i < DB_SVC_TABLE_MAX_INDEX_MAP ; i++)
	{
		if (pstSelf->apstIndexMap[i])
			HLIB_HASH_Destroy(pstSelf->apstIndexMap[i]);
		pstSelf->apstIndexMap[i] = NULL;
	}
}

STATIC HINT32	db_svc_TableListCompare (const void *pvArg1,const void *pvArg2)
{
	HINT32 nLhs = db_svc_TableGetKey(*(dbSvc_Table_t **)pvArg1);
	HINT32 nRhs = db_svc_TableGetKey(*(dbSvc_Table_t **)pvArg2);
	return nLhs - nRhs;
}

STATIC HINT32	db_svc_TableListCompareKey (HINT32 nLhs, HINT32 nRhs)
{
	return nLhs - nRhs;
}

STATIC void		db_svc_TableListEmit (dbSvc_TableList_t *pstSelf, HINT32 nVariation, HINT32 nKey)
{
	if (pstSelf->fnListener)
		pstSelf->fnListener(pstSelf, nVariation, nKey, pstSelf->pvUserData);
	db_svc_TableListRemoveIndexMap(pstSelf);
}

STATIC void		db_svc_TableListRemoveTable (dbSvc_Table_t *pstTable, dbSvc_TableList_t *pstSelf)
{
	HINT32	nKey;

	if (pstTable)
	{
		nKey = db_svc_TableGetKey(pstTable);

		HLIB_TREE_Replace(pstSelf->pstTrash, (void *)nKey, db_svc_TableIncref(pstTable));
		db_svc_TableListEmit(pstSelf, -1, nKey);

		db_svc_TableDecref(pstTable);
	}
}


STATIC HxVector_t *	db_svc_TableListDoFilteringInt (HxTREE_t *tree, const dbSvc_TableFilter_t *pstFilter, HBOOL match)
{
	HxVector_t		*pstList;
	dbSvc_Table_t	*pstItem;
	void			*pvIter;

	pstList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void * (*)(void *))db_svc_TableDecref, db_svc_TableListCompare);
	if (pstList == NULL)
	{
		HxLOG_Trace();
		return NULL;
	}

	HxTREE_EACH(tree, pvIter, {
		pstItem = (dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstItem == NULL)
			continue;
		if (db_svc_TableMatch(pstItem, pstFilter) == match)
		{
			HLIB_VECTOR_Add(pstList, db_svc_TableIncref(pstItem));
			if (pstFilter->nCount <= HLIB_VECTOR_Length(pstList))
				break;
		}
	});
	if (HLIB_VECTOR_Length(pstList) <= 0)
	{
		HLIB_VECTOR_Delete(pstList);
		return NULL;
	}
	return pstList;
}



STATIC HBOOL db_svc_TableListHasID (HUINT32 newId, void *pvUserData)
{
	if (db_svc_TableListFind((dbSvc_TableList_t *)pvUserData, (HINT32)newId))
		return TRUE;
	return FALSE;
}

/*******************************************************************/
/********************      Public Functions     ********************/
/********************      Table                ********************/
/*******************************************************************/

dbSvc_Table_t * db_svc_TableInit (const dbSvc_TableClass_t *pstClazz, dbSvc_Table_t *pstSelf, void *pvArg)
{
	pstSelf->pstClazz = pstClazz;
	if (pstClazz->fnCtor)
	{
		return pstClazz->fnCtor(pstSelf, pvArg);
	}
	return pstSelf;
}


dbSvc_TableFilter_t *	db_svc_TableInitFilter (dbSvc_TableFilter_t *pstFilter)
{
	if (pstFilter)
	{
		pstFilter->ulSize  = sizeof(dbSvc_Table_t);
		pstFilter->ulFlags = 0;
		pstFilter->nKey   = -1;
		pstFilter->nCount = 0x7FFFFFFF;
		HxSTD_MemSet(pstFilter->astExt, 0, sizeof(dbSvc_TableFilterEx_t) * DB_SVC_TABLE_MAX_EXT_FILTER);
	}
	return pstFilter;
}


dbSvc_TableFilter_t *	db_svc_TableAddFilterEx (dbSvc_TableFilter_t *pstFilter, HBOOL (* match)(const dbSvc_Table_t *, void *), void *pvUserData)
{
	if (pstFilter && match)
	{
		HINT32	i = 0;

		for (i = 0 ; i < DB_SVC_TABLE_MAX_EXT_FILTER && match ; i++)
		{
			if (pstFilter->astExt[i].fnMatch == NULL)
			{
				pstFilter->astExt[i].fnMatch = match;
				pstFilter->astExt[i].pvUserData = pvUserData;
				break;
			}
		}
		if (i == DB_SVC_TABLE_MAX_EXT_FILTER)
			return NULL;
	}
	return pstFilter;
}


dbSvc_Table_t *	db_svc_TableNew (const dbSvc_TableClass_t *pstClazz, void *pvArg)
{
	dbSvc_Table_t	*pstSelf, *pstRet;

	HxLOG_Assert(pstClazz);

	pstSelf = (dbSvc_Table_t *)DBSM_CALLOC(pstClazz->ulSize);
	if (pstSelf == NULL)
	{
		return NULL;
	}
	pstRet  = db_svc_TableInit(pstClazz, pstSelf, pvArg);
	if (pstRet == NULL)
	{
		DBSM_FREE(pstSelf);
		return NULL;
	}
	return db_svc_TableIncref(pstSelf);
}

void	db_svc_TableDelete (dbSvc_Table_t *pstSelf)
{
	HxLOG_Assert(pstSelf && pstSelf->pstClazz);

	if (pstSelf->pstClazz->fnDtor)
	{
		pstSelf->pstClazz->fnDtor(pstSelf);	// 실제 함수는 선언부 주석 참고
	}
	DBSM_FREE(pstSelf);
}

dbSvc_Table_t *	db_svc_TableClone (const dbSvc_Table_t *pstSelf)
{
	dbSvc_Table_t *pstClone;

	pstClone = (dbSvc_Table_t *)DBSM_MEMDUP(pstSelf, pstSelf->pstClazz->ulSize);
	if (pstClone)
	{
		pstClone->nRefCount = 1;
	}
	return pstClone;
}

dbSvc_Table_t *	db_svc_TableIncref (dbSvc_Table_t *pstSelf)
{
	if (pstSelf)
	{
		pstSelf->nRefCount++;
	}
	return pstSelf;
}


dbSvc_Table_t *	db_svc_TableDecref (dbSvc_Table_t *pstSelf)
{
	if (pstSelf && --pstSelf->nRefCount <= 0)
	{
		db_svc_TableDelete(pstSelf);
		return NULL;
	}
	return pstSelf;
}

HBOOL		db_svc_TableIsUpdated (dbSvc_Table_t *pstSelf, const void *pvData)
{
	if (pstSelf && pvData
		&& pstSelf->pstClazz
		&& HxSTD_MemCmp(pstSelf + 1, pvData, pstSelf->pstClazz->ulDataSize) != 0)
	{
		return TRUE;
	}
	return FALSE;
}

void *		db_svc_TableGetData (dbSvc_Table_t *pstSelf)
{
	if (pstSelf)
	{
		pstSelf++;
	}
	return (void *)pstSelf;
}


void *		db_svc_TableSetData (dbSvc_Table_t *pstSelf, void *pvData)
{
	HINT32	nKey;

	if (pstSelf == NULL || pstSelf->pstClazz == NULL || pvData == NULL)
		return NULL;

	nKey = db_svc_TableGetKey(pstSelf);
	HxSTD_MemCopy(pstSelf + 1, pvData, pstSelf->pstClazz->ulDataSize);
	db_svc_TableSetKey(pstSelf, nKey);

	return (void *)(pstSelf + 1);
}


HINT32		db_svc_TableGetKey (const dbSvc_Table_t *pstSelf)
{
	if (pstSelf == NULL
		|| pstSelf->pstClazz == NULL
		|| pstSelf->pstClazz->fnGetKey == NULL)
	{
		return 0;
	}
	return pstSelf->pstClazz->fnGetKey(pstSelf);	// 실제 함수는 선언부 주석 참고
}

void		db_svc_TableSetKey (dbSvc_Table_t *pstSelf, HINT32 nKey)
{
	if (pstSelf
		&& pstSelf->pstClazz
		&& pstSelf->pstClazz->fnSetKey)
	{
		pstSelf->pstClazz->fnSetKey(pstSelf, nKey);	// 실제 함수는 선언부 주석 참고
	}
}


HBOOL		db_svc_TableHasFlag (const dbSvc_Table_t *pstTable, HUINT32 ulFlag)
{
	if (pstTable && (pstTable->ulFlags & ulFlag))
		return TRUE;
	return FALSE;
}


dbSvc_Table_t *	db_svc_TableSetFlag (dbSvc_Table_t *pstTable, HUINT32 ulFlag, HBOOL bSet)
{
	if (pstTable)
	{
		if (bSet)
			pstTable->ulFlags |= ulFlag;
		else
			pstTable->ulFlags &= ~ulFlag;
	}
	return pstTable;
}

HBOOL		db_svc_TableMatch (const dbSvc_Table_t *pstSelf, const dbSvc_TableFilter_t *pstFilter)
{
	if (pstSelf && pstFilter)
	{
		if (db_svc_TableMatchInt(pstSelf, pstFilter))
		{
			if (pstSelf->pstClazz
				&& pstSelf->pstClazz->ulSize == pstFilter->ulSize
				&& pstSelf->pstClazz->fnMatch)	// 실제 함수는 선언부 주석 참고
			{
				return pstSelf->pstClazz->fnMatch(pstSelf, pstFilter);
			}
			return TRUE;
		}
	}
	return FALSE;
}

HINT32		db_svc_TableToString (const dbSvc_Table_t *pstSelf, FILE *pfStream)
{
	if (pstSelf && pstSelf->pstClazz && pstSelf->pstClazz->fnToStr)
	{
		return pstSelf->pstClazz->fnToStr(pstSelf, pfStream);	// 실제 함수는 선언부 주석 참고
	}
	return -1;
}


/*******************************************************************/
/********************      Public Functions     ********************/
/********************      Table List           ********************/
/*******************************************************************/

dbSvc_TableList_t *	db_svc_TableListNew (HUINT32 ulMaxId)
{
	dbSvc_TableList_t *pstSelf;

	HxLOG_Trace();

	pstSelf = (dbSvc_TableList_t *)DBSM_CALLOC(sizeof(dbSvc_TableList_t));
	return_if(pstSelf == NULL, NULL);

	pstSelf->pstTree = HLIB_TREE_NewFull(
					(HxCompareDataFunc)db_svc_TableListCompareKey
					, (void *)pstSelf
					, NULL
					, (HxDestroyNotify)db_svc_TableListRemoveTable
				);
	goto_if(pstSelf->pstTree == NULL, FAILED);

	pstSelf->pstTrash = HLIB_TREE_NewFull(
					(HxCompareDataFunc)db_svc_TableListCompareKey
					, NULL
					, NULL
					, (HxDestroyNotify)db_svc_TableDecref
				);
	goto_if(pstSelf->pstTrash == NULL, FAILED);

	pstSelf->hIdMgr = db_svc_IdmanagerNew(ulMaxId, 0, db_svc_TableListHasID, pstSelf);
	goto_if(pstSelf->hIdMgr == (Handle_t)NULL, FAILED);

	return pstSelf;

FAILED:
	db_svc_TableListDelete(pstSelf);
	return NULL;
}

void		db_svc_TableListDelete (dbSvc_TableList_t *pstSelf)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		db_svc_TableListRemoveIndexMap(pstSelf);

		if (pstSelf->pstTree)
			HLIB_TREE_Destroy(pstSelf->pstTree);
		if (pstSelf->pstTrash)
			HLIB_TREE_Destroy(pstSelf->pstTrash);
		if (pstSelf->hIdMgr)
			db_svc_IdmanagerDelete(pstSelf->hIdMgr);
		DBSM_FREE(pstSelf);
	}
}


HERROR		db_svc_TableListPut (dbSvc_TableList_t *pstSelf, dbSvc_Table_t *pstData)
{
	HINT32	nKey;
	HUINT32 ulId;

	if (pstSelf == NULL || pstData == NULL)
		return ERR_FAIL;

	nKey = db_svc_TableGetKey(pstData);
	ulId  = db_svc_IdmanagerGetLastId(pstSelf->hIdMgr);
	if (ulId < (HUINT32)nKey)
	{
		db_svc_IdmanagerSetLastId(pstSelf->hIdMgr, (HUINT32)nKey);
	}

#ifdef	CONFIG_DEBUG
	if (HLIB_TREE_Lookup(pstSelf->pstTree, (void *)nKey))
	{
		HxLOG_Warning("%s: Key duplicated!!!!\n", __FUNCTION__);
		return ERR_FAIL;
	}
#endif

	HLIB_TREE_Insert(pstSelf->pstTree, (void *)nKey, db_svc_TableSetFlag(pstData, eDbSvc_TableFlag_CHANGED, TRUE));
	db_svc_TableListEmit(pstSelf, +1, nKey);
	return ERR_OK;
}

HERROR		db_svc_TableListAdd (dbSvc_TableList_t *pstSelf, dbSvc_Table_t *pstData)
{
	HINT32	nKey;

	if (pstSelf == NULL || pstData == NULL)
		return ERR_FAIL;

	nKey = (HINT32)db_svc_IdmanagerNextId(pstSelf->hIdMgr);
	db_svc_TableSetKey(pstData, nKey);

#ifdef	CONFIG_DEBUG
	if (HLIB_TREE_Lookup(pstSelf->pstTree, (void *)nKey))
	{
		HxLOG_Warning("%s: Key duplicated!!!!\n", __FUNCTION__);
		return ERR_FAIL;
	}
#endif

	HLIB_TREE_Insert(pstSelf->pstTree, (void *)nKey, db_svc_TableSetFlag(pstData, eDbSvc_TableFlag_CHANGED, TRUE));
	db_svc_TableListEmit(pstSelf, +1, nKey);
	return ERR_OK;
}


HERROR		db_svc_TableListRemove (dbSvc_TableList_t *pstSelf, HINT32 nKey)
{
	HxLOG_Assert(pstSelf);

	if (HLIB_TREE_Remove(pstSelf->pstTree, (void *)nKey))
		return ERR_OK;
	return ERR_FAIL;
}


HERROR		db_svc_TableListRemoveAll (dbSvc_TableList_t *pstSelf) //by Moon
{
	HxLOG_Assert(pstSelf);

	HLIB_TREE_RemoveAll(pstSelf->pstTree);
	return ERR_OK;
}


HERROR		db_svc_TableListUpdate (dbSvc_TableList_t *pstSelf, HINT32 nKey, void *pvData)
{
	dbSvc_Table_t	*pstTable;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf && pvData);

	pstTable = (dbSvc_Table_t *)HLIB_TREE_Lookup(pstSelf->pstTree, (const void *)nKey);
	return_if(pstTable == NULL, ERR_FAIL);

	return_if(!db_svc_TableIsUpdated(pstTable, pvData), ERR_OK);

	db_svc_TableSetData(pstTable, pvData);
	db_svc_TableSetFlag(pstTable, eDbSvc_TableFlag_CHANGED, TRUE);
	db_svc_TableListEmit(pstSelf, 0, nKey);

	return ERR_OK;
}

dbSvc_Table_t *	db_svc_TableListFind (dbSvc_TableList_t *pstSelf, HINT32 nKey)
{
	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	return (dbSvc_Table_t *)HLIB_TREE_Lookup(pstSelf->pstTree, (const void *)nKey);
}

void		db_svc_TableListClear (dbSvc_TableList_t *pstSelf)
{
	HxLOG_Trace();

	return_if_void(pstSelf == NULL);

	HLIB_TREE_RemoveAll(pstSelf->pstTree);
	HLIB_TREE_RemoveAll(pstSelf->pstTrash);
	db_svc_IdmanagerSetLastId(pstSelf->hIdMgr, 0);
}


HINT32		db_svc_TableListCount (dbSvc_TableList_t *pstSelf)
{
	if (pstSelf == NULL)
		return 0;
	return HLIB_TREE_Nnodes(pstSelf->pstTree);
}


HBOOL		db_svc_TableListHasChanged (dbSvc_TableList_t *pstSelf)
{
	void	*pvIter;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	if (HLIB_TREE_Nnodes(pstSelf->pstTrash) > 0)
		return TRUE;

	HxTREE_EACH(pstSelf->pstTree, pvIter, {
		if (db_svc_TableHasFlag((dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter), eDbSvc_TableFlag_CHANGED))
		{
			return TRUE;
		}
	});
	return FALSE;
}

HxVector_t	*db_svc_TableListGetChanged (dbSvc_TableList_t *pstSelf)
{
	void			*pvIter;
	dbSvc_Table_t	*pstTable;
	HxVector_t		*pstList;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	pstList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void * (*)(void *))db_svc_TableDecref, db_svc_TableListCompare);
	return_if(pstList == NULL, NULL);

	HxTREE_EACH(pstSelf->pstTree, pvIter, {
		pstTable = (dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter);
		if (db_svc_TableHasFlag(pstTable, eDbSvc_TableFlag_CHANGED))
		{
			HLIB_VECTOR_Add(pstList, db_svc_TableIncref(pstTable));
		}
	});
	if (HLIB_VECTOR_Length(pstList) == 0)
	{
		HLIB_VECTOR_Delete(pstList);
		return NULL;
	}
	return pstList;
}

HxVector_t	*db_svc_TableListGetRemoved (dbSvc_TableList_t *pstSelf)
{
	void			*pvIter;
	dbSvc_Table_t	*pstTable;
	HxVector_t		*pstList;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	return_if(HLIB_TREE_Nnodes(pstSelf->pstTrash) == 0, NULL);

	pstList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void * (*)(void *))db_svc_TableDecref, db_svc_TableListCompare);
	return_if(pstList == NULL, NULL);

	HxTREE_EACH(pstSelf->pstTrash, pvIter, {
		pstTable = (dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstTable)
		{
			HLIB_VECTOR_Add(pstList, db_svc_TableIncref(pstTable));
		}
	});
	if (HLIB_VECTOR_Length(pstList) == 0)
	{
		HLIB_VECTOR_Delete(pstList);
		return NULL;
	}
	return pstList;
}


void		db_svc_TableListCommit (dbSvc_TableList_t *pstSelf)
{
	void			*pvIter;
	dbSvc_Table_t	*pstTable;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	HxTREE_EACH(pstSelf->pstTree, pvIter, {
		pstTable = (dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstTable)
		{
			db_svc_TableSetFlag(pstTable, eDbSvc_TableFlag_CHANGED, FALSE);
		}
	});
	HLIB_TREE_RemoveAll(pstSelf->pstTrash);
}


HxVector_t	*db_svc_TableListDoFiltering (dbSvc_TableList_t *pstSelf, const dbSvc_TableFilter_t *pstFilter)
{
	HxLOG_Assert(pstSelf && pstFilter);

	return db_svc_TableListDoFilteringInt(pstSelf->pstTree, pstFilter, TRUE);
}


HUINT32		db_svc_TableListRemoveIf (dbSvc_TableList_t *pstSelf, const dbSvc_TableFilter_t *pstFilter)
{
	dbSvc_Table_t	*pstItem;
	HxVector_t		*pstList;
	HUINT32			 ulCount;

	HxLOG_Assert(pstSelf && pstFilter);

	pstList = db_svc_TableListDoFilteringInt(pstSelf->pstTree, pstFilter, TRUE);
	if (pstList == NULL)
		return 0;

	ulCount = 0;
	HxVECTOR_EACH(pstList, dbSvc_Table_t *, pstItem, {
		if (HLIB_TREE_Remove(pstSelf->pstTree, (void *)db_svc_TableGetKey(pstItem)))
			ulCount++;
	});
	HLIB_VECTOR_Delete(pstList);
	return ulCount;
}


void		db_svc_TableListSetListener (dbSvc_TableList_t *pstSelf, dbSvc_TableListenerCb_t fnListener, void *pvUserData)
{
	if (pstSelf)
	{
		pstSelf->fnListener = fnListener;
		pstSelf->pvUserData = pvUserData;
	}
}

HxVector_t	*db_svc_TableListToVector (dbSvc_TableList_t *pstSelf)
{
	HxVector_t		*pstVector;
	void			*pvIter;
	dbSvc_Table_t	*pstItem;

	HxLOG_Assert(pstSelf);

	pstVector = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))db_svc_TableDecref, db_svc_TableListCompare);
	if (pstVector == NULL)
		return NULL;

	HxTREE_EACH(pstSelf->pstTree, pvIter, {
		pstItem = (dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstItem == NULL)
			continue;

		HLIB_VECTOR_Add(pstVector, (void *)db_svc_TableIncref(pstItem));
	});

	if (HLIB_VECTOR_Length(pstVector) == 0)
	{
		HLIB_VECTOR_Delete(pstVector);
		return NULL;
	}
	return pstVector;
}

HINT32		db_svc_TableListDump (dbSvc_TableList_t *pstSelf, FILE *pfStream)
{
	HINT32			 nRet = 0;
	dbSvc_Table_t	*pstItem;
	void			*pvIter;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	HxTREE_EACH(pstSelf->pstTree, pvIter, {
		pstItem = (dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstItem == NULL)
			continue;

		nRet += db_svc_TableToString(pstItem, pfStream ? pfStream : stdout);
	});
	return nRet;
}

#if 0
STATIC HxTREE_t *	db_svc_TableListGetRevision (dbSvc_TableList_t *pstSelf, HINT32 nIndex, HINT32 *pnRevision)
{
	void	*pvIter;

	pvIter = HLIB_TREE_Iterator(pstSelf->revision);
	while (nIndex--)
	{
		pvIter = HLIB_TREE_NextIterator(pvIter);
	}
	if (pnRevision)
	{
		*pnRevision = (HINT32)HLIB_TREE_IteratorKey(pvIter);
	}
	return (HxTREE_t *)HLIB_TREE_IteratorValue(pvIter);
}

STATIC HxTREE_t *	db_svc_TableListPrevRevision (dbSvc_TableList_t *pstSelf, HINT32 nRevision, HINT32 *prevRev)
{
	void	*pvIter;

	if (prevRev)
		*prevRev = 0;

	pvIter = HLIB_TREE_Iterator(pstSelf->revision);
	while (pvIter)
	{
		if ((HINT32)HLIB_TREE_IteratorKey(pvIter) == nRevision)
		{
			pvIter = HLIB_TREE_NextIterator(pvIter);

			if (prevRev)
			{
				*prevRev = (HINT32)HLIB_TREE_IteratorKey(pvIter);
			}
			return (HxTREE_t *)HLIB_TREE_IteratorValue(pvIter);
		}
		pvIter = HLIB_TREE_NextIterator(pvIter);
	}
	return NULL;
}

STATIC HxTREE_t	*	db_svc_TableListLastRevision (dbSvc_TableList_t *pstSelf, HINT32 *pnRevision)
{
	return db_svc_TableListGetRevision(pstSelf, 0, pnRevision);
}

HERROR		OxTABLELIST_CommitToRevision (dbSvc_TableList_t *pstSelf, HINT32 nRevision)
{
	void			*pvIter;
	dbSvc_Table_t	*pstTable;
	HxTREE_t		*pstRev;
	HINT32			 nLastRev;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	if (nRevision == DB_SM_TABLE_HEAD_REVISION)
	{
		nRevision = pstSelf->lastRev + 1;
	}
	else
	{
		db_svc_TableListLastRevision(pstSelf, &nLastRev);
		return_if (nRevision <= nLastRev, ERR_FAIL);
	}

	HxTREE_EACH(pstSelf->tree, pvIter, {
		pstTable = (dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter);
		if (!db_svc_TableHasFlag(pstTable, eDbSvc_TableFlag_CHANGED))
			continue;
		db_svc_TableSetFlag(pstTable, eDbSvc_TableFlag_CHANGED, FALSE);

		pstTable->revision = nRevision;
	});

	pstRev = HLIB_TREE_Clone(pstSelf->tree, NULL, (HxCloneFunc)db_svc_TableIncref);
	return_if(pstRev == NULL, ERR_FAIL);

	HLIB_TREE_Replace(pstSelf->revision, (void *)nRevision, pstRev);

	HxLOG_Info("\n\%s(%d), nRevision:%d\n\n", __FUNCTION__, __LINE__, nRevision);
	pstSelf->lastRev = nRevision;

	return ERR_OK;
}

HERROR		OxTABLELIST_Revert (dbSvc_TableList_t *pstSelf, HINT32 nRevision)
{
	// TODO:
	return ERR_FAIL;
}

void		OxTABLELIST_Flush (dbSvc_TableList_t *pstSelf)
{
	HINT32		 nLastRev;
	HxTREE_t	*pstLast;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	if (HLIB_TREE_Nnodes(pstSelf->revision) <= 1)
		return;

	// 마지막 리비전만 빼고 다 지운다.
	pstLast = db_svc_TableListLastRevision(pstSelf, &nLastRev);
	return_if_void(pstLast == NULL);

	HLIB_TREE_Steal(pstSelf->revision, (void *)nLastRev);
	HLIB_TREE_RemoveAll(pstSelf->revision);
	HLIB_TREE_Insert(pstSelf->revision, (void *)nLastRev, pstLast);
}

HxVector_t	*OxTABLELIST_GetRevision (dbSvc_TableList_t *pstSelf, HINT32 nRevision, HBOOL trash)
{
	HxVector_t		*pstList;
	dbSvc_Table_t	*pstItem;
	void			*pvIter;
	HxTREE_t		*pstTree;
	HxTREE_t		*pstPrev;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	if (nRevision == DB_SM_TABLE_HEAD_REVISION)
		pstTree = db_svc_TableListLastRevision(pstSelf, NULL);
	else
		pstTree = HLIB_TREE_Lookup(pstSelf->revision, (void *)nRevision);
	return_if(pstTree == NULL, NULL);

	pstList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void * (*)(void *))db_svc_TableDecref, db_svc_TableListCompare);
	return_if(pstList == NULL, NULL);

	if (trash)
	{
		// 이전 리비전 리스트 룹을 돌면서, 현재 리비전에 해당 데이터가 없으면 지워진 것임!!
		pstPrev = db_svc_TableListPrevRevision(pstSelf, nRevision, NULL);
		if (pstPrev)
		{
			HxTREE_EACH(pstPrev, pvIter, {
				if (!HLIB_TREE_Lookup(pstTree , HLIB_TREE_IteratorKey(pvIter)))
				{
					HLIB_VECTOR_Add(pstList, db_svc_TableIncref(HLIB_TREE_IteratorValue(pvIter)));
				}
			});
		}
	}
	else
	{
		HxTREE_EACH(pstTree, pvIter, {
			pstItem = (dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem == NULL)
				continue;

			if (nRevision == DB_SM_TABLE_HEAD_REVISION || nRevision == pstItem->revision)
			{
				HLIB_VECTOR_Add(pstList, db_svc_TableIncref(pstItem));
			}
		});
	}

	if (HLIB_VECTOR_Length(pstList) <= 0)
	{
		HLIB_VECTOR_Delete(pstList);
		return NULL;
	}
	return pstList;
}
#endif

