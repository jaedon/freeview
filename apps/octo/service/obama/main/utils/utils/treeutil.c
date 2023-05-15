/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdlib.h>
#include <octo_common.h>

#include "vkernel.h"
#include "util.h"
#include "treeutil.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define TREE_POOL_NUM							15
#define DEFAULT_MAXTREEITEM					4000							// default max of items(can be inserted)
#define TR(p) 									((PTREEMGR)p)
#define TRITEM(p) 								((PTREEITEM)p)






/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


typedef struct	tagTREEITEMPOOL
{
	HUINT32					ulSemId;
	HUINT32					ulPoolSize;
	HUINT32					ulFreeCount;
	PTREEITEM				pBuffer[TREE_POOL_NUM];
	PTREEITEM				pPoolHead;
}TREEITEMPOOL;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static	TREEITEMPOOL	sTreeItemPool = {0, 0, 0, {NULL, }, NULL};


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
static HERROR			local_treeutil_InitItemPool(void);
static HERROR			local_treeutil_CreateItemPool(void);
static HERROR			local_treeutil_AllocItem(TREEITEM **ppTreeItem);
static HERROR			local_treeutil_FreeItem(PTREEITEM pItem);
static HERROR			local_treeutil_CreateTree(TREEMGR **ppTreeMgr);
static HERROR			local_treeutil_DestroyTree(PTREEMGR pTreeMgr);
static HERROR			local_treeutil_RemoveItems(PTREEMGR pTree, PTREEITEM pItem);
static HERROR			local_treeutil_InsertItem(	PTREEMGR pTree,
												PTREEITEM pParent,
												PTREEITEM pPrevLink,
												PTREEITEM pNextLink,
												TREEITEM_PARAM_t *pParam,
												TREEITEM **ppNewItem);


static HERROR			local_treeutil_GetItemUserInfo(PTREEITEM pItem, HUINT32 ulSize, void *pUserInfo);
static HERROR			local_treeutil_SetItemUserInfo(PTREEITEM pItem, HUINT32 ulSize, void *pUserInfo);
static HERROR			local_treeutil_LinkItem(PTREEITEM pNewItem, PTREEITEM pParent, PTREEITEM pPrev, PTREEITEM pNext);
static HERROR			local_treeutil_GetLayerStartItem(PTREEITEM pItem, TREEITEM **ppStartItem);
static HERROR			local_treeutil_FindLastChild(PTREEITEM pParent, TREEITEM **ppLastChild);
static HERROR			local_treeutil_GetPrevItem(PTREEITEM pItem, TREEITEM **ppPrevItem);
static HERROR			local_treeutil_GetNextItem(PTREEITEM pItem, TREEITEM **ppNextItem);
static HERROR			local_treeutil_GetChildItem(PTREEITEM pItem, TREEITEM **ppChildItem);
static HERROR			local_treeutil_GetChildNum(PTREEITEM pItem, HUINT32 *pulNumOfChild);
static HERROR			local_treeutil_GetParentItem(PTREEITEM pItem, TREEITEM **ppParent);
//static HERROR			local_treeutil_MoveItemToOtherParent(PTREEITEM pParentItem, PTREEITEM pNewChildItem);
static HERROR			local_treeutil_GetItemDepth(PTREEITEM pItem, HUINT32 *pulDepth);
//static HERROR			local_treeutil_GetItemByOffset(PTREEITEM pItem, HINT32 lOffset, TREEITEM **ppDestItem);
static HERROR			local_treeutil_GetItemData(PTREEITEM pItem, void **ppLocalData);
//static HERROR			local_treeutil_SetItemData(PTREEITEM pItem, void* pData);
static HERROR			local_treeutil_RemoveItem(PTREEMGR pTree, PTREEITEM pItem);
//static HERROR			local_treeutil_RemoveChlildItems(PTREEMGR pTree, PTREEITEM pItem);
static HERROR			local_treeutil_GetFirstChildItem(PTREEMGR pTree, TREEITEM **ppFirstChild);
static HERROR			local_treeutil_GetRootItem(PTREEMGR pTree, TREEITEM **ppRootItem);
static HERROR			local_treeutil_GetLayerStartItem(PTREEITEM pItem, TREEITEM **ppStartItem);
static HERROR			local_treeutil_GetLayerEndItem(PTREEITEM pItem, TREEITEM **ppEndItem);





#define _________GLOBAL_FUNC_________

HERROR			UTIL_TREEUTIL_InitItemPool(void)
{
	return local_treeutil_InitItemPool();

}

HERROR			UTIL_TREEUTIL_CreateTree(HTREEMGR *phTreeMgr)
{
	return local_treeutil_CreateTree((TREEMGR**)phTreeMgr);
}



HERROR			UTIL_TREEUTIL_DestroyTree(HTREEMGR hTreeMgr)
{
	return local_treeutil_DestroyTree((PTREEMGR)hTreeMgr);
}


HERROR			UTIL_TREEUTIL_RemoveItems(HTREEMGR hTree, HTREEITEM hItem)
{
	return local_treeutil_RemoveItems((PTREEMGR)hTree, (PTREEITEM)hItem);
}


HERROR			UTIL_TREEUTIL_RemoveItem(HTREEMGR hTree, HTREEITEM hItem)
{
	return local_treeutil_RemoveItem((PTREEMGR)hTree, (PTREEITEM)hItem);
}


HERROR			UTIL_TREEUTIL_InsertItem(	HTREEMGR hTree,
												HTREEITEM hParent,
												HTREEITEM hPrevLink,
												HTREEITEM hNextLink,
												TREEITEM_PARAM_t *pParam,
												HTREEITEM *phNewItem)
{
	return local_treeutil_InsertItem(	(PTREEMGR)hTree,
									(PTREEITEM)hParent,
									(PTREEITEM)hPrevLink,
									(PTREEITEM)hNextLink,
									pParam,
									(PTREEITEM*)phNewItem);
}


HERROR		UTIL_TREEUTIL_GetParentItem(HTREEITEM hItem, HTREEITEM *phParent)
{

	return local_treeutil_GetParentItem((PTREEITEM)hItem, (TREEITEM**)phParent);
}


HERROR		UTIL_TREEUTIL_GetPrevItem(HTREEITEM hItem, HTREEITEM *phPrevItem)
{
	return local_treeutil_GetPrevItem((PTREEITEM)hItem, (PTREEITEM*)phPrevItem);

}


HERROR		UTIL_TREEUTIL_GetNextItem(HTREEITEM hItem, HTREEITEM *phNextItem)
{

	return local_treeutil_GetNextItem((PTREEITEM)hItem, (PTREEITEM*)phNextItem);
}


HERROR		UTIL_TREEUTIL_GetChildItem(HTREEITEM hItem, HTREEITEM *phChildItem)
{

	return local_treeutil_GetChildItem((PTREEITEM)hItem, (TREEITEM**)phChildItem);
}

HERROR		UTIL_TREEUTIL_GetChildNum(HTREEITEM hItem, HUINT32 *pulNumOfChild)
{

	return local_treeutil_GetChildNum((PTREEITEM)hItem, pulNumOfChild);
}


HERROR		UTIL_TREEUTIL_FindLastChild(HTREEITEM hParent, HTREEITEM *phLastChild)
{
	return local_treeutil_FindLastChild((PTREEITEM)hParent, (TREEITEM**)phLastChild);
}


HERROR		UTIL_TREEUTIL_GetItemUserInfo(HTREEITEM hItem, HUINT32 ulSize, void *pUserInfo)
{

	return local_treeutil_GetItemUserInfo((PTREEITEM)hItem, ulSize, pUserInfo);
}


HERROR		UTIL_TREEUTIL_SetItemUserInfo(HTREEITEM hItem, HUINT32 ulSize, void *pUserInfo)
{

	return local_treeutil_SetItemUserInfo((PTREEITEM)hItem, ulSize, pUserInfo);
}


HERROR		UTIL_TREEUTIL_GetItemDepth(HTREEITEM hItem, HUINT32 *pulDepth)
{
	return local_treeutil_GetItemDepth((PTREEITEM)hItem, pulDepth);
}


HERROR		UTIL_TREEUTIL_GetRootItem(HTREEMGR hTree, HTREEITEM *phRootItem)
{

	return local_treeutil_GetRootItem((PTREEMGR)hTree, (TREEITEM**)phRootItem);
}


HERROR		UTIL_TREEUTIL_GetFirstChildItem(HTREEMGR hTree, HTREEITEM *phFirstChild)
{

	return local_treeutil_GetFirstChildItem((PTREEMGR)hTree, (TREEITEM**)phFirstChild);
}


HERROR		UTIL_TREEUTIL_GetLayerEndItem(HTREEITEM pItem, HTREEITEM *phEndItem)
{
	return local_treeutil_GetLayerEndItem((PTREEITEM)pItem, (TREEITEM**)phEndItem);
}



HERROR		UTIL_TREEUTIL_GetItemData(HTREEITEM hItem, void **ppLocalData)
{
	return local_treeutil_GetItemData((PTREEITEM)hItem, ppLocalData);
}



#define _________LOCAL_FUNC_________
static HERROR			local_treeutil_InitItemPool(void)
{
	HERROR			hError;

	HxSTD_memset(&sTreeItemPool, 0x00, sizeof(TREEITEMPOOL));

	hError = VK_SEM_Create((unsigned long*)&sTreeItemPool.ulSemId, (const char*)"tree_pool_sem", VK_SUSPENDTYPE_PRIORITY);

	return (VK_OK == hError) ? ERR_TREEUTIL_OK : ERR_TREEUTIL_FAIL;
}


static HERROR			local_treeutil_CreateItemPool(void)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;
	HUINT32			ulIndex = 0;
	HUINT32			ulBufIndex = 0;
	PTREEITEM		pTempItem = NULL;
	PTREEITEM		pNewBuffer = NULL;
	PTREEITEM		pNextItem = NULL;


	for(ulIndex = 0; ulIndex < TREE_POOL_NUM ; ulIndex++)
	{
		if(sTreeItemPool.pBuffer[ulIndex] == NULL)
		{
			ulBufIndex = ulIndex;
			break;
		}
	}
	if(ulBufIndex == TREE_POOL_NUM)
	{
		HxLOG_Print("[local_treeutil_CreateItemPool] Buffer full!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	pNewBuffer = (PTREEITEM)OxMW_Malloc(sizeof(TREEITEM)*DEFAULT_MAXTREEITEM);
	if(pNewBuffer == NULL)
	{
		HxLOG_Print("[local_treeutil_CreateItemPool] Buffer null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	HxSTD_memset(pNewBuffer, 0x00, sizeof(TREEITEM)*DEFAULT_MAXTREEITEM);
	sTreeItemPool.pBuffer[ulBufIndex] = pNewBuffer;

	for(ulIndex = 0; ulIndex < DEFAULT_MAXTREEITEM - 1; ulIndex++)
	{
		pTempItem = &(pNewBuffer[ulIndex]);
		pTempItem->pNextItem = &(pNewBuffer[ulIndex + 1]);
	}
	sTreeItemPool.ulFreeCount = DEFAULT_MAXTREEITEM;
	sTreeItemPool.ulPoolSize += DEFAULT_MAXTREEITEM;

	//swap
	pNextItem = sTreeItemPool.pPoolHead;
	sTreeItemPool.pPoolHead = pNewBuffer;
	pNewBuffer[DEFAULT_MAXTREEITEM-1].pNextItem = pNextItem;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;

}


static HERROR			local_treeutil_AllocItem(TREEITEM **ppTreeItem)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;
	PTREEITEM		pFreeItem = NULL;


	VK_SEM_Get(sTreeItemPool.ulSemId);
	if(ppTreeItem == NULL)
	{
		HxLOG_Print("[local_treeutil_AllocItem] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*ppTreeItem = NULL;

	if(sTreeItemPool.ulFreeCount == 0)
	{
		HxLOG_Print("[local_treeutil_AllocItem] ulFreeCount zero!!\n");
		hError = local_treeutil_CreateItemPool();
		if(hError != ERR_TREEUTIL_OK)
		{
			HxLOG_Print("[local_treeutil_AllocItem] ulFreeCount zero!!\n");
			goto EXIT_TREEUTIL_FUNC;
		}
	}

	pFreeItem = sTreeItemPool.pPoolHead;
	sTreeItemPool.pPoolHead = pFreeItem->pNextItem;
	sTreeItemPool.ulFreeCount--;

	HxSTD_memset(pFreeItem, 0x00, sizeof(TREEITEM));
	*ppTreeItem = pFreeItem;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :
	VK_SEM_Release(sTreeItemPool.ulSemId);

	return hError;
}


static HERROR			local_treeutil_FreeItem(PTREEITEM pItem)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;
	PTREEITEM		pPrevItem = NULL;

	VK_SEM_Get(sTreeItemPool.ulSemId);
	if(pItem == NULL)
	{
		HxLOG_Print("[local_treeutil_AllocItem] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	HxSTD_memset(pItem, 0x00, sizeof(TREEITEM));

	pPrevItem = sTreeItemPool.pPoolHead;
	sTreeItemPool.pPoolHead = pItem;
	pItem->pNextItem = pPrevItem;
	sTreeItemPool.ulFreeCount++;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :
	VK_SEM_Release(sTreeItemPool.ulSemId);
	return hError;

}



static HERROR			local_treeutil_CreateTree(TREEMGR **ppTreeMgr)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;
	PTREEMGR		pTree = NULL;


	if (ppTreeMgr == NULL)
	{
		HxLOG_Error("[local_treeutil_CreateTree] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*ppTreeMgr = NULL;

	pTree = (PTREEMGR)OxMW_Malloc(sizeof(TREEMGR));
	if(pTree == NULL)
	{
		HxLOG_Error("[local_treeutil_CreateTree] alloc fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	HxSTD_memset(pTree, 0x00, sizeof(TREEMGR));

	hError = local_treeutil_AllocItem(&pTree->pRootItem);
	if (hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Error("[local_treeutil_CreateTree] pRootItem null!!\n");
		OxMW_Free(pTree);
		goto EXIT_TREEUTIL_FUNC;
	}

	*ppTreeMgr = pTree;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;

}



static HERROR			local_treeutil_DestroyTree(PTREEMGR pTreeMgr)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if (pTreeMgr == NULL)
	{
		HxLOG_Error("[local_treeutil_DestroyTree] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	hError = local_treeutil_RemoveItems(pTreeMgr, pTreeMgr->pRootItem);
	if (hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_treeutil_DestroyTree] RemoveItems fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	OxMW_Free(pTreeMgr);

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;

}



static HERROR			local_treeutil_RemoveItems(PTREEMGR pTree, PTREEITEM pItem)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;
	PTREEITEM 		pParent = NULL;
	PTREEITEM		pDeleteItem = NULL;
	PTREEITEM		pNext = NULL;
	PTREEITEM		pPrev = NULL;

	if (pItem == NULL || pTree == NULL)
	{
		HxLOG_Error("[local_treeutil_RemoveItems] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	pNext = pPrev = pDeleteItem = pItem;
	while(pDeleteItem)
	{

////////////////////////////////////////////////////////////////////////////////
//
// 1. child items destroy(recursive call)
//
////////////////////////////////////////////////////////////////////////////////
		if( pDeleteItem->pChildItem != NULL)
		{
			hError = local_treeutil_RemoveItems(pTree, pDeleteItem->pChildItem);
			if(hError != ERR_TREEUTIL_OK)
			{
				HxLOG_Print("[local_treeutil_RemoveItems] RemoveItems fail!!\n");
				pDeleteItem = pDeleteItem->pNextItem;
				continue;
			}
		}

////////////////////////////////////////////////////////////////////////////////
//
// 2. new link create & old link destroy (if root item, skip~)
//
////////////////////////////////////////////////////////////////////////////////
		pPrev = pDeleteItem->pPrevItem;
		pNext = pDeleteItem->pNextItem;
		pParent = pDeleteItem->pParentItem;
		if(pParent != NULL)
		{
			pParent->ulNumOfChild--;
			//if delete item is first child
			if(pParent->pChildItem == pDeleteItem)
			{
				pParent->pChildItem = pNext;
				if(pNext != NULL)
				{
					pNext->pPrevItem = NULL;
				}
			}
			else
			{
				if(pPrev != NULL)
				{
					pPrev->pNextItem = pNext;
				}
				if(pNext != NULL)
				{
					pNext->pPrevItem = pPrev;
				}
			}
		}

////////////////////////////////////////////////////////////////////////////////
//
// 3. local data free
//
////////////////////////////////////////////////////////////////////////////////
		if(pDeleteItem->pfnFreeFunc != NULL)
		{
			pDeleteItem->pfnFreeFunc(pDeleteItem->pLocalData);
		}

////////////////////////////////////////////////////////////////////////////////
//
// 4. item free
//
////////////////////////////////////////////////////////////////////////////////
		hError = local_treeutil_FreeItem(pDeleteItem);
		if(hError != ERR_TREEUTIL_OK)
		{
			HxLOG_Print("[local_treeutil_RemoveItems] FreeItem fail!!\n");
			pDeleteItem = pDeleteItem->pNextItem;
			continue;
		}

		pTree->ulNumOfItem--;
		pDeleteItem = pNext;
	}

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}




static HERROR			local_treeutil_InsertItem(	PTREEMGR pTree,
												PTREEITEM pParent,
												PTREEITEM pPrevLink,
												PTREEITEM pNextLink,
												TREEITEM_PARAM_t *pParam,
												TREEITEM **ppNewItem)
{
	HERROR				hError = ERR_TREEUTIL_FAIL;
	PTREEITEM			pNewItem = NULL;

	if(pTree == NULL || ppNewItem == NULL)
	{
		HxLOG_Print("[local_treeutil_InsertItem] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;

	}
	*ppNewItem = NULL;


////////////////////////////////////////////////////////////////////////////////
//
// 2. alloc new item
//
////////////////////////////////////////////////////////////////////////////////
	hError = local_treeutil_AllocItem(&pNewItem);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_treeutil_InsertItem] AllocItem fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	HxSTD_memset(pNewItem, 0x00, sizeof(TREEITEM));


////////////////////////////////////////////////////////////////////////////////
//
// 2. link new item
//
////////////////////////////////////////////////////////////////////////////////
	if(pParent == NULL)
	{
		pParent = pTree->pRootItem;
	}
	hError = local_treeutil_LinkItem(pNewItem, pParent, pPrevLink, pNextLink);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_treeutil_InsertItem] LinkItem fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

////////////////////////////////////////////////////////////////////////////////
//
// 3. set new item info
//
////////////////////////////////////////////////////////////////////////////////
	pNewItem->pLocalData = pParam->pLocalData;
	pNewItem->pfnFreeFunc = pParam->pfnFreeFunc;
	pNewItem->ulDepth = pParent->ulDepth+1;


	pParent->ulNumOfChild++;
	pTree->ulNumOfItem++;

	*ppNewItem = pNewItem;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}




static HERROR			local_treeutil_LinkItem(PTREEITEM pNewItem, PTREEITEM pParent, PTREEITEM pPrev, PTREEITEM pNext)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;
	PTREEITEM		pTempItem = NULL;

	if(pNewItem == NULL || pParent == NULL)
	{
		HxLOG_Print("[local_treeutil_LinkItem] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;

	}

	pNewItem->pParentItem = pParent;
	pNewItem->pChildItem = NULL;

	if(pPrev != NULL || pNext != NULL)
	{
		pNewItem->pPrevItem = pPrev;
		pNewItem->pNextItem = pNext;
	}
	else
	{
		pNewItem->pNextItem = NULL;
		hError = local_treeutil_FindLastChild(pParent, &pTempItem);
		if(hError != ERR_TREEUTIL_OK)
		{
			HxLOG_Print("[local_treeutil_LinkItem] FindLastChild fail!!\n");
		}
		pNewItem->pPrevItem = pTempItem;
	}

	if (pNewItem->pNextItem != NULL)
	{
		pNewItem->pNextItem->pPrevItem	= pNewItem;
	}
	if (pNewItem->pPrevItem != NULL)
	{
		pNewItem->pPrevItem->pNextItem	= pNewItem;
	}

	hError = local_treeutil_GetLayerStartItem(pNewItem, &pParent->pChildItem);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_treeutil_LinkItem] GetLayerStartItem fail !!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}



//-------------------------------------------------------------------------------------------------
//	pParent의 자녀 아이템 중에서 가장 끝(pNext)에 있는 아이템을 찾는다.
//-------------------------------------------------------------------------------------------------
static HERROR		local_treeutil_FindLastChild(PTREEITEM pParent, TREEITEM **ppLastChild)
{
	HERROR		hError = ERR_TREEUTIL_FAIL;
	PTREEITEM	pItem = NULL;

	if(pParent == NULL)
	{
		HxLOG_Print("[local_treeutil_FindLastChild] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*ppLastChild =  NULL;

	pItem = pParent->pChildItem;
	if(pItem == NULL)
	{
		hError = ERR_TREEUTIL_OK;
		goto EXIT_TREEUTIL_FUNC;
	}

	while (pItem->pNextItem)
	{
		pItem = pItem->pNextItem;
	}

	*ppLastChild =  pItem;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}



static HERROR		local_treeutil_GetPrevItem(PTREEITEM pItem, TREEITEM **ppPrevItem)
{
	HERROR		hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || ppPrevItem == NULL)
	{
		HxLOG_Print("[local_treeutil_GetPrevItem] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	*ppPrevItem = pItem->pPrevItem;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;

}


static HERROR		local_treeutil_GetNextItem(PTREEITEM pItem, TREEITEM **ppNextItem)
{
	HERROR		hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || ppNextItem == NULL)
	{
		HxLOG_Print("[local_treeutil_GetNextItem] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	*ppNextItem = pItem->pNextItem;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;

}


static HERROR			local_treeutil_GetChildItem(PTREEITEM pItem, TREEITEM **ppChildItem)
{
	HERROR		hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || ppChildItem == NULL)
	{
		HxLOG_Print("[local_treeutil_GetChildItem] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	*ppChildItem = pItem->pChildItem;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;

}



static HERROR			local_treeutil_GetChildNum(PTREEITEM pItem, HUINT32 *pulNumOfChild)
{
	HERROR		hError = ERR_TREEUTIL_FAIL;

	if(pulNumOfChild == NULL)
	{
		HxLOG_Print("[local_treeutil_GetChildNum] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*pulNumOfChild = pItem->ulNumOfChild;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}





static HERROR			local_treeutil_GetParentItem(PTREEITEM pItem, TREEITEM **ppParent)
{
	HERROR		hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL|| ppParent == NULL)
	{
		HxLOG_Print("[local_treeutil_GetParentItem] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	*ppParent = pItem->pParentItem;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;

}


#if 0 //Not used..
static HERROR			local_treeutil_MoveItemToOtherParent(PTREEITEM pParentItem, PTREEITEM pNewChildItem)
{
	HERROR				hError = ERR_TREEUTIL_FAIL;
	PTREEITEM			pTempChild= NULL;


	if(pParentItem == NULL|| pNewChildItem == NULL)
	{
		HxLOG_Print("[local_treeutil_MoveItemToOtherParent] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	pTempChild = pParentItem->pChildItem;
	if(pTempChild)
	{
		while(pTempChild->pNextItem)
		{
			pTempChild = pTempChild->pNextItem;
		}

		pTempChild->pNextItem = pNewChildItem;
		pNewChildItem->pPrevItem = pTempChild;

		pNewChildItem->pParentItem->pChildItem = NULL;
		pNewChildItem->pParentItem = pParentItem;

		pNewChildItem = pNewChildItem->pNextItem;
		while(pNewChildItem)
		{
			pNewChildItem->pParentItem = pParentItem;
			pNewChildItem = pNewChildItem->pNextItem;
		}
	}
	else
	{
		pNewChildItem->pParentItem->pChildItem = NULL;
		pNewChildItem->pPrevItem = NULL;

		pParentItem->pChildItem = pNewChildItem;
		pNewChildItem->pParentItem = pParentItem;

		pNewChildItem = pNewChildItem->pNextItem;
		while(pNewChildItem)
		{
			pNewChildItem->pParentItem = pParentItem;
			pNewChildItem = pNewChildItem->pNextItem;
		}
	}

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;

}

#endif


static HERROR			local_treeutil_GetItemDepth(PTREEITEM pItem, HUINT32 *pulDepth)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || pulDepth == NULL)
	{
		HxLOG_Print("[local_treeutil_GetItemDepth] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*pulDepth = 0;

	*pulDepth = pItem->ulDepth;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}


#if 0 //Not used..
static HERROR			local_treeutil_GetItemByOffset(PTREEITEM pItem, HINT32 lOffset, TREEITEM **ppDestItem)
{
	PTREEITEM		pTemp = NULL;
	PTREEITEM		pMoveItem = NULL;
	HINT32			lOff = 0;
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || ppDestItem == NULL)
	{
		HxLOG_Print("[local_treeutil_GetItemByOffset] Param null!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*ppDestItem = NULL;

	pTemp = pItem;
	lOff = ABS(lOffset);
	while(lOff)
	{
		if(lOffset >= 0)
		{
			hError = local_treeutil_GetNextItem(pTemp, &pMoveItem);
			if(hError != ERR_TREEUTIL_OK)
			{
				HxLOG_Print("[local_treeutil_GetItemByOffset] GetNextItem fail!!\n");
			}
		}
		else
		{
			hError = local_treeutil_GetPrevItem(pTemp, &pMoveItem);
			if(hError != ERR_TREEUTIL_OK)
			{
				HxLOG_Print("[local_treeutil_GetItemByOffset] GetPrevItem fail!!\n");
			}
		}

		if(pMoveItem == NULL)
		{
			break;
		}
		pTemp = pMoveItem;
		lOff--;
	}
	*ppDestItem = pTemp;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}
#endif


static HERROR			local_treeutil_GetItemData(PTREEITEM pItem, void **ppLocalData)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || ppLocalData == NULL)
	{
		HxLOG_Print("[local_treeutil_GetItemData] Param fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*ppLocalData = NULL;


	*ppLocalData = pItem->pLocalData;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;

}

#if 0 //Not used...
static HERROR			local_treeutil_SetItemData(PTREEITEM pItem, void* pData)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || pData == NULL)
	{
		HxLOG_Print("[local_treeutil_SetItemData] Param fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	pItem->pLocalData = pData;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}

#endif

static HERROR			local_treeutil_GetItemUserInfo(PTREEITEM pItem, HUINT32 ulSize, void *pUserInfo)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || pUserInfo == NULL || ulSize > USERDEFINEINFO_SIZE)
	{
		HxLOG_Print("[local_treeutil_GetItemUserInfo] Param fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	HxSTD_memcpy(pUserInfo, pItem->aucUserDefineInfo, sizeof(HUINT8)*ulSize);


	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;

}


static HERROR			local_treeutil_SetItemUserInfo(PTREEITEM pItem, HUINT32 ulSize, void *pUserInfo)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || pUserInfo == NULL || ulSize > USERDEFINEINFO_SIZE)
	{
		HxLOG_Print("[local_treeutil_SetItemData] Param fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	HxSTD_memcpy(pItem->aucUserDefineInfo, pUserInfo, sizeof(HUINT8)*ulSize);

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}






static HERROR			local_treeutil_RemoveItem(PTREEMGR pTree, PTREEITEM pItem)
{
	PTREEITEM			pParent = NULL;
	PTREEITEM			pNext = NULL;
	PTREEITEM			pPrev = NULL;
	HERROR				hError = ERR_TREEUTIL_FAIL;

	if(pTree == NULL || pItem == NULL)
	{
		HxLOG_Print("[local_treeutil_RemoveItem] Param fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

////////////////////////////////////////////////////////////////////////////////
//
// 1. child items destroy(recursive call)
//
////////////////////////////////////////////////////////////////////////////////
	hError = local_treeutil_RemoveItems(pTree, pItem->pChildItem);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_treeutil_RemoveItem] RemoveItems fail!!\n");
	}

////////////////////////////////////////////////////////////////////////////////
//
// 2. new link create & old link destroy
//
////////////////////////////////////////////////////////////////////////////////
	pParent = pItem->pParentItem;
	pNext = pItem->pNextItem;
	pPrev = pItem->pPrevItem;

	//if parent is layer root
	if(pParent->pChildItem == pItem)
	{
		pParent->pChildItem = pNext;
		if(pNext != NULL)
		{
			pNext->pPrevItem = NULL;
		}
	}
	else
	{
		if(pPrev != NULL)
		{
			pPrev->pNextItem = pNext;
		}
		if(pNext != NULL)
		{
			pNext->pPrevItem = pPrev;
		}
	}

////////////////////////////////////////////////////////////////////////////////
//
// 3. local data free
//
////////////////////////////////////////////////////////////////////////////////
	if(pItem->pfnFreeFunc != NULL)
	{
		pItem->pfnFreeFunc(pItem->pLocalData);
	}


////////////////////////////////////////////////////////////////////////////////
//
// 4. item free
//
////////////////////////////////////////////////////////////////////////////////
	hError = local_treeutil_FreeItem(pItem);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_treeutil_RemoveItem] FreeItem fail!!\n");
	}

	pParent->ulNumOfChild--;
	pTree->ulNumOfItem--;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}


#if 0 //Not Used..
static HERROR			local_treeutil_RemoveChlildItems(PTREEMGR pTree, PTREEITEM pItem)
{
	HERROR				hError = ERR_TREEUTIL_FAIL;

	if(pTree == NULL || pItem == NULL)
	{
		HxLOG_Print("[local_treeutil_RemoveChlildItems] Param fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}


	hError = local_treeutil_RemoveItems(pTree, pItem->pChildItem);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_treeutil_RemoveChlildItems] RemoveItems fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}

	pItem->ulNumOfChild = 0;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}

#endif

static HERROR			local_treeutil_GetFirstChildItem(PTREEMGR pTree, TREEITEM **ppFirstChild)
{
	PTREEITEM		pRoot = NULL;
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if(pTree == NULL)
	{
		HxLOG_Print("[local_treeutil_GetFirstChildItem] Param fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*ppFirstChild = NULL;

	pRoot = pTree->pRootItem;

	*ppFirstChild = pRoot->pChildItem;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}


static HERROR			local_treeutil_GetRootItem(PTREEMGR pTree, TREEITEM **ppRootItem)
{
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if(pTree == NULL || ppRootItem == NULL)
	{
		HxLOG_Print("[local_treeutil_GetFirstChildItem] Param fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*ppRootItem = NULL;

	*ppRootItem = pTree->pRootItem;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}



static HERROR			local_treeutil_GetLayerStartItem(PTREEITEM pItem, TREEITEM **ppStartItem)
{
	PTREEITEM		pTemp = NULL;
	PTREEITEM		pStart = NULL;
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || ppStartItem == NULL)
	{
		HxLOG_Print("[local_treeutil_GetLayerStartItem] Param fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*ppStartItem = NULL;

	pStart = pItem;
	hError = local_treeutil_GetPrevItem(pItem, &pTemp);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_treeutil_GetLayerStartItem] GetPrevItem fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	while(pTemp)
	{
		pStart = pTemp;
		hError = local_treeutil_GetPrevItem(pTemp, &pTemp);
		if(hError != ERR_TREEUTIL_OK)
		{
			HxLOG_Print("[local_treeutil_GetLayerStartItem] GetPrevItem fail!!\n");
			goto EXIT_TREEUTIL_FUNC;
		}
	}

	*ppStartItem = pStart;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}





static HERROR			local_treeutil_GetLayerEndItem(PTREEITEM pItem, TREEITEM **ppEndItem)
{
	PTREEITEM		pTemp = NULL;
	PTREEITEM		pEnd = NULL;
	HERROR			hError = ERR_TREEUTIL_FAIL;

	if(pItem == NULL || ppEndItem == NULL)
	{
		HxLOG_Print("[local_treeutil_GetLayerStartItem] Param fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	*ppEndItem = NULL;

	pEnd = pItem;
	hError = local_treeutil_GetNextItem(pItem, &pTemp);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_treeutil_GetLayerStartItem] GetPrevItem fail!!\n");
		goto EXIT_TREEUTIL_FUNC;
	}
	while(pTemp)
	{
		pEnd = pTemp;
		hError = local_treeutil_GetNextItem(pTemp, &pTemp);
		if(hError != ERR_TREEUTIL_OK)
		{
			HxLOG_Print("[local_treeutil_GetLayerStartItem] GetPrevItem fail!!\n");
			goto EXIT_TREEUTIL_FUNC;
		}
	}

	*ppEndItem = pEnd;

	hError = ERR_TREEUTIL_OK;

EXIT_TREEUTIL_FUNC :

	return hError;
}






