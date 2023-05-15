/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__TREEUTIL_H__
#define	__TREEUTIL_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define USERDEFINEINFO_SIZE					40					// 40bytes


typedef	enum
{
	ERR_TREEUTIL_OK 					= 0,
	ERR_TREEUTIL_FAIL

}TREEUTIL_ERR;


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef void*				HTREEMGR;
typedef void*				HTREEITEM;


typedef 	void					(*TREEITEM_FREE_FUNC)(void*);


typedef struct tagTreeItem
{
	HUINT8					aucUserDefineInfo[USERDEFINEINFO_SIZE];

	HUINT32					ulNumOfChild;
	HUINT32					ulDepth;

	TREEITEM_FREE_FUNC		pfnFreeFunc;
	void*					pLocalData;

	struct tagTreeItem*		pParentItem;
	struct tagTreeItem*		pChildItem;
	struct tagTreeItem*		pNextItem;
	struct tagTreeItem*		pPrevItem;
} TREEITEM, *PTREEITEM;


typedef struct tagTreeMgr
{
	HUINT8					aucUserDefineInfo[USERDEFINEINFO_SIZE];


	HUINT32 					ulNumOfItem;							// total number of items
	PTREEITEM				pRootItem;								//head item

} TREEMGR, *PTREEMGR;


typedef struct	tagTreeItemParam
{
	void						*pLocalData;
	TREEITEM_FREE_FUNC		pfnFreeFunc;
}TREEITEM_PARAM_t;




/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/* Public Function 마다 필히 기재하도록 함 */
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HERROR			UTIL_TREEUTIL_InitItemPool(void);
HERROR			UTIL_TREEUTIL_CreateItemPool(void);
HERROR			UTIL_TREEUTIL_CreateTree(HTREEMGR *phTreeMgr);
HERROR			UTIL_TREEUTIL_DestroyTree(HTREEMGR hTreeMgr);
HERROR			UTIL_TREEUTIL_RemoveItems(HTREEMGR hTree, HTREEITEM hItem);

HERROR			UTIL_TREEUTIL_RemoveItem(HTREEMGR hTree, HTREEITEM hItem);
HERROR			UTIL_TREEUTIL_InsertItem(	HTREEMGR hTree,
												HTREEITEM hParent,
												HTREEITEM hPrevLink,
												HTREEITEM hNextLink,
												TREEITEM_PARAM_t *pParam,
												HTREEITEM *phNewItem);
HERROR		UTIL_TREEUTIL_GetParentItem(HTREEITEM hItem, HTREEITEM *phParent);
HERROR		UTIL_TREEUTIL_GetPrevItem(HTREEITEM hItem, HTREEITEM *phPrevItem);
HERROR		UTIL_TREEUTIL_GetNextItem(HTREEITEM hItem, HTREEITEM *phNextItem);
HERROR		UTIL_TREEUTIL_GetChildItem(HTREEITEM hItem, HTREEITEM *phChildItem);
HERROR		UTIL_TREEUTIL_GetChildNum(HTREEITEM hItem, HUINT32 *pulNumOfChild);
HERROR		UTIL_TREEUTIL_FindLastChild(HTREEITEM hParent, HTREEITEM *phLastChild);
HERROR		UTIL_TREEUTIL_GetItemUserInfo(HTREEITEM hItem, HUINT32 ulSize, void *pUserInfo);
HERROR		UTIL_TREEUTIL_SetItemUserInfo(HTREEITEM hItem, HUINT32 ulSize, void *pUserInfo);
HERROR		UTIL_TREEUTIL_GetItemDepth(HTREEITEM hItem, HUINT32 *pulDepth);
HERROR		UTIL_TREEUTIL_GetRootItem(HTREEMGR hTree, HTREEITEM *phRootItem);
HERROR		UTIL_TREEUTIL_GetFirstChildItem(HTREEMGR hTree, HTREEITEM *phFirstChild);
HERROR		UTIL_TREEUTIL_GetLayerEndItem(HTREEITEM pItem, HTREEITEM *phEndItem);
HERROR		UTIL_TREEUTIL_GetItemData(HTREEITEM hItem, void **ppLocalData);

#endif /* !__TREEUTIL_H__ */

