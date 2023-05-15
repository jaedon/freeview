/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_param_cache.c
	@brief

	Description:  \n
	Module: db/param \n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <dapi.h>
#include <hapi.h>

#include <db_param.h>
#include "../local_include/_db_main.h"
#include "./local_include/_db_param_default.h"
#include "_db_param.h"
#include <pdb_param.h>

//	the field key value is defined in oapi_scenario.h
//	we may change the key value defined file.

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#if	defined(CONFIG_DEBUG)
	#define	CHECK_DBTABLE_VALID(a, b)		do {if (a != b->uid) {HxLOG_Critical("ENUM [%x != %x] is changed !!! Abort\n", a, b->uid); HxLOG_Assert(0);} } while(0)
#else
	#define	CHECK_DBTABLE_VALID(a, b)
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Variables      ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define	___CACHE__________________

/*	DB PARAM Cache (Tree) Functions
 *
 */

STATIC HINT32	db_param_CacheTreeCompareKey(const void *pvData1, const void *pvData2, void *pvUserData)
{
	return (HINT32)((HINT32)pvData1 - (HINT32)pvData2);
}

STATIC void		db_param_CacheTreeDestroyValue(void *pvData)
{
	if (pvData)
		HLIB_STD_MemFree(pvData);
}

Handle_t	db_param_CacheInit(void)
{
	HxTREE_t *pstSystemTree = HLIB_TREE_NewFull(db_param_CacheTreeCompareKey, NULL, NULL, db_param_CacheTreeDestroyValue);

	if (NULL == pstSystemTree)
	{
		HxLOG_Critical("pstSystemTree is NULL\n");
		return HANDLE_NULL;
	}

	return (Handle_t)pstSystemTree;
}

HxTREE_t	*db_param_CacheGetTreeFromHandle(Handle_t hCache)
{
	if (HANDLE_NULL == hCache)
	{
		HxLOG_Critical("hCache is Invalid [0x%x]\n", hCache);
		return NULL;
	}

	return (HxTREE_t *)hCache;
}


HERROR db_param_CacheRemoveAll(Handle_t hCache)
{
	HxTREE_t *pstTree = db_param_CacheGetTreeFromHandle(hCache);

	if (NULL == pstTree)
	{
		HxLOG_Warning("pstTree is NULL\n");
		return ERR_FAIL;
	}

	HLIB_TREE_RemoveAll(pstTree);

	return ERR_OK;
}

STATIC DxDBSetupItem_t *db_param_CacheSetConfigItem(HxTREE_t *pstTree, DxDBSETUP_Item_e eItem, dbParam_CfgItemInfo_t *pItemInfo)
{
	DxDBSetupItem_t		*pstItem;
	HUINT32				 nLen = 0;

	if (HLIB_TREE_Lookup(pstTree, (void*)eItem))
		HLIB_TREE_Remove(pstTree, (void*)eItem);

	nLen = HxSTD_StrLen(pItemInfo->szItemText);

	if (pItemInfo->nDataSize)
	{
		pstItem = (DxDBSetupItem_t*)HLIB_STD_MemCalloc(sizeof(DxDBSetupItem_t) + pItemInfo->nDataSize);
		pstItem->eItemType = eDxTYPE_BINARY;
		pstItem->nSize = pItemInfo->nDataSize;
		HxSTD_MemCopy(pstItem->pucData, pItemInfo->pucData, pstItem->nSize);
	}
	else if (nLen > 0)
	{
		pstItem = (DxDBSetupItem_t*)HLIB_STD_MemCalloc(sizeof(DxDBSetupItem_t) + nLen + 1);
		pstItem->eItemType = eDxTYPE_STRING;
		pstItem->nSize = nLen + 1;
		HxSTD_StrNCpy((HCHAR*)pstItem->pucData, pItemInfo->szItemText, nLen);
	}
	else
	{
		pstItem = (DxDBSetupItem_t*)HLIB_STD_MemCalloc(sizeof(DxDBSetupItem_t));
		pstItem->eItemType = eDxTYPE_INTEGER;
		pstItem->nValue = pItemInfo->nItemValue;
	}
	pstItem->uid = eItem;
	HLIB_TREE_Replace(pstTree, (void*)eItem, (void*)pstItem);

	return pstItem;
}

STATIC DxDBSetupItem_t *db_param_CacheWriteDefaultValue(HxTREE_t *pstTree, DxDBSETUP_Item_e eItem)
{
	dbParam_DefaultItem_t	*pstItem = NULL;

	pstItem = db_param_DefaultValueGetItemByDxKey(eItem);
	if (pstItem)
	{
		return db_param_CacheSetConfigItem(pstTree, eItem, &pstItem->stInfo);
	}

	HxLOG_Critical("Item Value is not defined YET!!! - [%x]\n", eItem);
	return NULL;
}


#if 0
STATIC HERROR db_param_CacheRemoveConfigItem(HxTREE_t *pstTree, DxDBSETUP_Item_e eItem)
{
	HLIB_TREE_Remove(pstTree, (void*)eItem);

	return ERR_OK;
}
#endif

DxDBSetupItem_t *db_param_CacheSetItemValue(Handle_t hCache, DxDBSETUP_Item_e eItem, HINT32 nValue)
{
	DxDBSetupItem_t		*pstItem;
	HxTREE_t			*pstTree;

	pstTree = db_param_CacheGetTreeFromHandle(hCache);
	if (NULL == pstTree)
	{
		HxLOG_Warning("pstTree is NULL\n");
		return NULL;
	}

	pstItem = (DxDBSetupItem_t*)HLIB_TREE_Lookup(pstTree, (void*)eItem);
	if (pstItem)
	{
		CHECK_DBTABLE_VALID(eItem, pstItem);
		HxLOG_Assert(pstItem->eItemType == eDxTYPE_INTEGER);
		pstItem->nValue = nValue;
	}
	else
	{
		pstItem = (DxDBSetupItem_t*)HLIB_STD_MemCalloc(sizeof(DxDBSetupItem_t));
		pstItem->uid       = eItem;
		pstItem->eItemType = eDxTYPE_INTEGER;
		pstItem->nValue    = nValue;
		HLIB_TREE_Replace(pstTree, (void*)eItem, (void*)pstItem);
	}

	return pstItem;
}

DxDBSetupItem_t *db_param_CacheGetItemValue(Handle_t hCache, DxDBSETUP_Item_e eItem, HINT32 *pnValue)
{
	DxDBSetupItem_t		*pstItem;
	HxTREE_t			*pstTree;

	pstTree = db_param_CacheGetTreeFromHandle(hCache);
	if (NULL == pstTree)
	{
		HxLOG_Warning("pstTree is NULL\n");
		return NULL;
	}

	pstItem = (DxDBSetupItem_t*)HLIB_TREE_Lookup(pstTree, (void*)eItem);
	if (pstItem == NULL)
	{
		pstItem = db_param_CacheWriteDefaultValue(pstTree, eItem);
	}

	if (pstItem)
	{
		CHECK_DBTABLE_VALID(eItem, pstItem);
		*pnValue = pstItem->nValue;
	}
	else
	{
		*pnValue = 0;
	}

	return pstItem;
}

DxDBSetupItem_t *db_param_CacheSetItemText(Handle_t hCache, DxDBSETUP_Item_e eItem, HCHAR *pszText)
{
	DxDBSetupItem_t		*pstItem;
	HINT32				 nLen = 0;
	HxTREE_t			*pstTree;

	pstTree = db_param_CacheGetTreeFromHandle(hCache);
	if (NULL == pstTree)
	{
		HxLOG_Warning("pstTree is NULL\n");
		return NULL;
	}

	nLen = HxSTD_StrLen(pszText);

	pstItem = (DxDBSetupItem_t*)HLIB_TREE_Lookup(pstTree, (void*)eItem);
	if (pstItem)
	{
		CHECK_DBTABLE_VALID(eItem, pstItem);
		HxLOG_Assert(pstItem->eItemType == eDxTYPE_STRING);
		if (nLen < pstItem->nSize)
		{
			HxSTD_StrNCpy((HCHAR*)pstItem->pucData, pszText, pstItem->nSize);
			return pstItem;
		}
		else
		{
			HLIB_TREE_Remove(pstTree, (void*)eItem);
		}
	}

	pstItem = (DxDBSetupItem_t*)HLIB_STD_MemCalloc(sizeof(DxDBSetupItem_t) + nLen + 1);
	pstItem->uid       = eItem;
	pstItem->eItemType = eDxTYPE_STRING;
	pstItem->nSize     = nLen + 1;
	HLIB_STD_StrNCpySafe((HCHAR*)pstItem->pucData, pszText, pstItem->nSize);

	HLIB_TREE_Replace(pstTree, (void*)eItem, (void*)pstItem);

	return pstItem;
}

DxDBSetupItem_t *db_param_CacheGetItemText(Handle_t hCache, DxDBSETUP_Item_e eItem, HINT32 nLimit, HCHAR *pszText)
{
	DxDBSetupItem_t		*pstItem;
	HxTREE_t			*pstTree;

	pstTree = db_param_CacheGetTreeFromHandle(hCache);
	if (NULL == pstTree)
	{
		HxLOG_Warning("pstTree is NULL\n");
		return NULL;
	}

	pstItem = (DxDBSetupItem_t*)HLIB_TREE_Lookup(pstTree, (void*)eItem);
	if (pstItem == NULL)
		pstItem = db_param_CacheWriteDefaultValue(pstTree, eItem);

	if (pstItem)
	{
		HINT32	nLen;

		CHECK_DBTABLE_VALID(eItem, pstItem);
		if (nLimit)
		{
			nLen = (nLimit <= pstItem->nSize) ? nLimit - 1 : pstItem->nSize;

			HxSTD_StrNCpy(pszText, (HCHAR*)pstItem->pucData, nLen);
			pszText[nLen] = 0;
		}
		else
		{
			nLen = (pstItem->nSize > 1) ? pstItem->nSize - 1 : 0;

			if (nLen > 0)
			{
				HxLOG_Warning("Limited size is not defined!!! (%d)\n", nLen);
				HxSTD_StrNCpy(pszText, (HCHAR*)pstItem->pucData, nLen);
				pszText[nLen] = 0;
			}
		}
	}

	return pstItem;
}

DxDBSetupItem_t *db_param_CacheSetItemBinary(Handle_t hCache, DxDBSETUP_Item_e eItem, HINT32 nSize, void *pvData)
{
	DxDBSetupItem_t		*pstItem;
	HxTREE_t			*pstTree;

	pstTree = db_param_CacheGetTreeFromHandle(hCache);
	if (NULL == pstTree)
	{
		HxLOG_Warning("pstTree is NULL\n");
		return NULL;
	}

	pstItem = (DxDBSetupItem_t*)HLIB_TREE_Lookup(pstTree, (void*)eItem);
	if (pstItem)
	{
		CHECK_DBTABLE_VALID(eItem, pstItem);
		HxLOG_Assert(pstItem->eItemType == eDxTYPE_BINARY);
		if (nSize < pstItem->nSize)
		{
			HxSTD_MemCopy(pstItem->pucData, pvData, nSize);
			return pstItem;
		}
		else
		{
			HLIB_TREE_Remove(pstTree, (void*)eItem);
		}
	}

	pstItem = (DxDBSetupItem_t*)HLIB_STD_MemCalloc(sizeof(DxDBSetupItem_t) + nSize);
	pstItem->uid       = eItem;
	pstItem->eItemType = eDxTYPE_BINARY;
	pstItem->nSize	   = nSize;
	if (NULL != pvData && nSize > 0)
	{
		HxSTD_MemCopy(pstItem->pucData, pvData, nSize);
	}

	HLIB_TREE_Replace(pstTree, (void*)eItem, (void*)pstItem);

	return pstItem;
}

DxDBSetupItem_t *db_param_CacheGetItemBinary(Handle_t hCache, DxDBSETUP_Item_e eItem, HINT32 nLimit, HINT32 *pnDataSize, void *pvData)
{
	HINT32				 nLen;
	DxDBSetupItem_t		*pstItem;
	HxTREE_t			*pstTree;

	pstTree = db_param_CacheGetTreeFromHandle(hCache);
	if (NULL == pstTree)
	{
		HxLOG_Warning("pstTree is NULL\n");
		return NULL;
	}

	pstItem = (DxDBSetupItem_t*)HLIB_TREE_Lookup(pstTree, (void*)eItem);
	if (pstItem == NULL)
		pstItem = db_param_CacheWriteDefaultValue(pstTree, eItem);
	if (pstItem)
	{
		CHECK_DBTABLE_VALID(eItem, pstItem);
		if (nLimit > 0)
		{
			nLen = (nLimit < pstItem->nSize) ? nLimit : pstItem->nSize;
		}
		else
		{
			HxLOG_Warning("Limited size is not defined!!!\n");
			nLen = pstItem->nSize;
		}

		if (nLen > 0)
		{
			HxSTD_MemCopy(pvData, pstItem->pucData, nLen);
		}
		*pnDataSize = nLen;
	}

	return pstItem;
}

/* end of file */
