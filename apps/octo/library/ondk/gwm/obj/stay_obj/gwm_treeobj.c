/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


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
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


#if 1
/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <string.h>
#include <octo_common.h>

#include "util.h"
#include "treeutil.h"

#include <gwm_objdef.h>
#include <gwm_appobj.h>

#include <osd_gfx.h>

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define	MAX_TREE_OBJECT						16








/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */
typedef struct		tagTreeObjectItem
{
	HUINT32			ulItemStatus;
}TREEOBJ_ITEMINFO;



#define CODE_COMPLETE
typedef struct	tagTreeObject
{
#if	defined(CODE_COMPLETE)
	GUIOBJ_BASIC_PROPERTY								// 반드시 맨 앞에 존재하도록 할 것.
#else
	LINK_OBJECT					link;
	GWM_ObjectType_t			type;
	HINT32						ID;
	ONDK_Rect_t					rect;					// tree frame rect
	HINT32						focus;
	GWM_Proc_t					Proc;
	GWM_ObjDraw_t				objDrawMethod;
#endif

	HTREEMGR					hTree;					// treeitem structure

	HTREEITEM					hDispStart;				//display start item pointer
	HTREEITEM					hDispEnd;				//display end item pointer
	HTREEITEM					hSelectedItem;			//selected item
	HTREEITEM					hLastExpandItem;		//last expand item

	HUINT16						usNumOfVisible;			// visible number of items
	HUINT16						usSelectedIndex;		// index of selected item
	HUINT16 					usItemsPerPage;		// display items(items per a page)

	ONDK_Rect_t					stFrameMargin;			// tree item rect prototype
	ONDK_Rect_t					stItemRect;				// tree item rect prototype

	GWM_ObjDraw_t				objDrawItemMethod;
	HBOOL						bObjDisabled;			// key input / draw를 막을 state flag
	HBOOL						bOnlyOneItemExpand;

}TREE_OBJECT;


typedef struct	tagTreeObjectItemDrawInfo
{
	HUINT32 					ulIndexAtTotal;		// current visible item index at total
	HUINT32 					ulIndexAtPage;		// item index at page
	HUINT32 					ulStatus;			// selected? ... 기타등등
	HUINT32					ulDepth;
	HUINT32					ulNumOfChild;
	void						*pLocalData;

}TREEOBJ_ITEM_DRAWINFO;




typedef struct	tagTreeObjectManager
{
	HUINT8			ucIsAlloced[MAX_TREE_OBJECT];
	TREE_OBJECT		astTreeObject[MAX_TREE_OBJECT];
}TREE_MANAGER;

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static TREE_MANAGER		s_stTreeManager = {{0, },{{0, }, }};



/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */
static ONDK_Result_t		local_treeobject_Create(HINT32 ulId, OBJ_CREATE_t *pstObjCreateParam);
static ONDK_Result_t		local_treeobject_GetObject(HINT32 ulId, TREE_OBJECT **ppTreeObj);
static ONDK_Result_t		local_treeobject_AllocNewObject(TREE_OBJECT **ppTreeObj);
static ONDK_Result_t		local_treeobject_FreeObject(HINT32 ulId);
static ONDK_Result_t		local_treeobject_FreeObject_ByPObj(PTREE_OBJECT pTreelObj);
static ONDK_Result_t		local_treeobject_InsertItem(HTREEMGR hTree, HTREEITEM hParent, HTREEITEM hPrevLink, HTREEITEM hNextLink, TREEITEM_PARAM_t *pParam, HTREEITEM *phNewItem);
static ONDK_Result_t		local_treeobject_RemoveItem(HTREEMGR hTree, HTREEITEM hItem);
static ONDK_Result_t		local_treeobject_FindNextItem(HTREEITEM hItem, HTREEITEM *phFindItem);
static ONDK_Result_t		local_treeobject_FindLastChild(HTREEITEM hParent, HTREEITEM *phFindItem);
static ONDK_Result_t		local_treeobject_FindLastDescItem(HTREEITEM hItem, HTREEITEM *phFindItem);
static ONDK_Result_t		local_treeobject_FindPrevItem(HTREEITEM hItem, HTREEITEM *phFindItem);

/* 아래 3 함수는 EXPAND 상태에 상관 없이 모든 ITEM에서 FIND */
static ONDK_Result_t		local_treeobject_FindNextItem_InAllItem(HTREEITEM hItem, HTREEITEM *phFindItem);
static ONDK_Result_t		local_treeobject_FindLastDescItem_InAllItem(HTREEITEM hItem, HTREEITEM *phFindItem);
static ONDK_Result_t		local_treeobject_FindPrevItem_InAllItem(HTREEITEM hItem, HTREEITEM *phFindItem);

static ONDK_Result_t		local_treeobject_FindNextItem_InSameLevel(HTREEITEM hItem, HTREEITEM *phFindItem);
static ONDK_Result_t		local_treeobject_FindPrevItem_InSameLevel(HTREEITEM hItem, HTREEITEM *phFindItem);

static ONDK_Result_t		local_treeobject_Collapse(PTREE_OBJECT pTreeObject, HTREEITEM hItem, HBOOL bSubCollapse);
static ONDK_Result_t		local_treeobject_CollapseAll(PTREE_OBJECT pTreeObject, HTREEITEM hItem);
static ONDK_Result_t		local_treeobject_SetDisplayStartItem(PTREE_OBJECT pTreeObject, HTREEITEM hItem);
static ONDK_Result_t		local_treeobject_GetDisplayStartItem(PTREE_OBJECT pTreeObject, HTREEITEM *phItem);

// definded.. but not used..
#if 0
static ONDK_Result_t		local_treeobject_SetDisplayFocusAtMiddle(PTREE_OBJECT pTreeObject, HTREEITEM hItem);
static ONDK_Result_t		local_treeobject_GetLayerEndItem(HTREEITEM hItem, HTREEITEM *phLayerEndItem);
static ONDK_Result_t		local_treeobject_GetLayerStartItem(HTREEITEM hItem, HTREEITEM *phLayerStartItem);
static ONDK_Result_t		local_treeobject_GetDispStartItem(PTREE_OBJECT pTreeObject, HTREEITEM *phDispStartItem);
static ONDK_Result_t		local_treeobject_GetNextPageItem(	PTREE_OBJECT pTreeObject,
																HTREEITEM hItem,
																HBOOL bAllLayer,
																HTREEITEM *phNextItem);
static ONDK_Result_t		local_treeobject_GetLastExpand(PTREE_OBJECT pTreeObject, HTREEITEM *phLastExpandItem);
static ONDK_Result_t		local_treeobject_SetLastExpand(PTREE_OBJECT pTreeObject,  HTREEITEM hItem);
static ONDK_Result_t		local_treeobject_GetItemRect(PTREE_OBJECT pTreeObject, HTREEITEM hItem, ONDK_Rect_t *pItemRect);
#endif

static ONDK_Result_t		local_treeobject_SetReverseDisplayStartItem(PTREE_OBJECT pTreeObject, HTREEITEM hItem);
static ONDK_Result_t		local_treeobject_GetDisplayLastItem(PTREE_OBJECT pTreeObject, HTREEITEM *phDispLastItem);
static ONDK_Result_t		local_treeobject_GetSelectedItem(PTREE_OBJECT pTreeObject, HTREEITEM *phSelectedItem);
static ONDK_Result_t		local_treeobject_ExpandItem(	PTREE_OBJECT pTreeObject,
															HBOOL bSubExpand,
															HBOOL bAllExpand,
															HUINT32 *pulExpandStatus);

static ONDK_Result_t		local_treeobject_Expand(PTREE_OBJECT pTreeObject,  HTREEITEM hItem, HBOOL bSubExpand);
static ONDK_Result_t		local_treeobject_ExpandAll(HTREEITEM hItem);
static ONDK_Result_t		local_treeobject_IsExpandItem(PTREE_OBJECT pTreeObject,  HTREEITEM hItem, HBOOL *pbExpand);
static ONDK_Result_t		local_treeobject_SelectItem(PTREE_OBJECT pTreeObject, HTREEITEM hItem);
static ONDK_Result_t		local_treeobject_SelectNextItem(PTREE_OBJECT pTreeObject, HTREEITEM *phSelectedItem);
static ONDK_Result_t		local_treeobject_SelectPrevItem(PTREE_OBJECT pTreeObject, HTREEITEM *phSelectedItem);
static ONDK_Result_t		local_treeobject_GetVisibleItemCount(	PTREE_OBJECT pTreeObject,
																	HTREEITEM hSelectedItem,
																	HUINT16 *pusTotalNumber,
																	HUINT16 *pusSelectedIndex);

static ONDK_Result_t		local_treeobject_SetDisplayPage(PTREE_OBJECT pTreeObject,  HTREEITEM hSelectedItem);
static ONDK_Result_t		local_treeobject_IsPageItem(PTREE_OBJECT pTreeObject, HTREEITEM hItem, HBOOL *pbIsPage);
static ONDK_Result_t		local_treeobject_SetPageFocus(PTREE_OBJECT pTreeObject, HUINT32 ulFocusIndex, HBOOL bAllLayer);
static ONDK_Result_t		local_treeobject_SetLastItemOnLastIdx(PTREE_OBJECT pTreeObject, HBOOL bAllLayer, HUINT32 ulSelectIndex);
static ONDK_Result_t		local_treeobject_PrevPage(PTREE_OBJECT pTreeObject, HBOOL bAllLayer, HTREEITEM *phPrevPage);
static ONDK_Result_t		local_treeobject_NextPage(PTREE_OBJECT pTreeObject, HBOOL bAllLayer, HTREEITEM *phNextPage);
static ONDK_Result_t		local_treeobject_GetPrevPageItem(	PTREE_OBJECT pTreeObject,
																HTREEITEM hItem,
																HBOOL bAllLayer,
																HTREEITEM *phPrevItem);
static ONDK_Result_t		local_treeobject_GetItemData(HTREEITEM hItem, void **ppData);
static ONDK_Result_t		local_treeobject_FindItem_ByData(HTREEMGR hTree, HTREEITEM *phFindItem, void *pTargetData,
											 FP_GetNextItemFunc naviNextItemFunc, FP_CompFunc compFunc);
static ONDK_Result_t		local_treeobject_GetCondMatchItemCount_ByData(HTREEMGR		hTree, HUINT32 *pulCount, void *pTargetData,
																	  FP_GetNextItemFunc naviNextItemFunc, FP_CompFunc compFunc);
static ONDK_Result_t		local_treeobject_GetFirstChildItem(HTREEMGR hTree, HTREEITEM *phFirstChild);
static ONDK_Result_t		local_treeobject_GetRootItem(HTREEMGR hTree, HTREEITEM *phRootItem);
static ONDK_Result_t		local_treeobject_GetChildNum(HTREEITEM hItem, HUINT32 *pulNumOfChild);
static ONDK_Result_t		local_treeobject_GetChildItem(HTREEITEM hItem, HTREEITEM *phChildItem);
static ONDK_Result_t		local_treeobject_GetIndexOfItem(HTREEMGR hTree, HTREEITEM hTargetItem,
														FP_GetNextItemFunc naviNextItemFun, HUINT32 *pulIndex);

static ONDK_Result_t		local_treeobject_GetDisplayItemRect(PTREE_OBJECT pTreeObject, HUINT16 usIndex, ONDK_Rect_t *pItemRect);
static ONDK_Result_t		local_treeobject_UpdatePageItem(PTREE_OBJECT pTreeObject, HINT32 arg);
static ONDK_Result_t		local_treeobject_defaultProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg);

static ONDK_Result_t		local_treeobject_SetObjDisable(PTREE_OBJECT pTreeObject, HBOOL	bDisable);
static ONDK_Result_t		local_treeobject_SetOnlyOneItemExpandFlag(PTREE_OBJECT pTreeObject, HBOOL	bAllExpand);


/* APIs */
	// create & destroy //
ONDK_Result_t			GWM_TREE_Create(HINT32 ulId, OBJ_CREATE_t *pstObjCreateParam)
{
	return local_treeobject_Create(ulId, pstObjCreateParam);
}


ONDK_Result_t		GWM_TREE_FreeObjectById(HINT32 ulId)
{
	return local_treeobject_FreeObject(ulId);
}


	// insert & remove //
ONDK_Result_t		GWM_TREE_InsertItem(HINT32 ulId, HTREEITEM hParent, HTREEITEM hPrevLink, HTREEITEM hNextLink, TREEITEM_PARAM_t *pParam, HTREEITEM *phNewItem)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEMGR		hTreeMgr;

	hResult = GWM_TREE_GetTreeMgr(ulId, &hTreeMgr);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_InsertItem(hTreeMgr, hParent, hPrevLink, hNextLink, pParam, phNewItem);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_RemoveItem(HINT32 ulId, HTREEITEM hItem)
{
	HTREEITEM		hOrgSelectedItem = NULL, hNewSelectItem = NULL;
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEMGR		hTreeMgr;
	HBOOL			bOrgSelectedItemRemoved = FALSE;
	PTREE_OBJECT	pTreeObject = NULL;

	/* get PTREE Object */
	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult != GWM_RET_OK)
	{
		return hResult;
	}

	hResult = GWM_TREE_GetTreeMgr(ulId, &hTreeMgr);
	if (hResult == GWM_RET_OK)
	{
		/* selected item이 remove 된다면 -> selected item의 next item을 select */
		hOrgSelectedItem = pTreeObject->hSelectedItem;
		if (hOrgSelectedItem == hItem)
		{
			bOrgSelectedItemRemoved = TRUE;
			hResult = local_treeobject_FindNextItem(hItem, &hNewSelectItem);
			if (hResult != GWM_RET_OK || NULL == hNewSelectItem)
			{	// next item이 없다면 젤 처음 Item으로 select
				hResult = local_treeobject_GetFirstChildItem(hTreeMgr, &hNewSelectItem);
				if (hResult != GWM_RET_OK || NULL == hNewSelectItem)
				{
					return hResult;
				}
			}
		}


		hResult = local_treeobject_RemoveItem(hTreeMgr, hItem);

		/* 기존 선택된 Item을 지운 후에 새로운 Item으로 선택 (선택이 안되면 hSelectedItem을 NULL로 설정) */
		if (bOrgSelectedItemRemoved)
		{
			hResult = GWM_TREE_SelectItem(ulId, hNewSelectItem);
			if (hResult != GWM_RET_OK)
			{
				pTreeObject->hSelectedItem = NULL;
			}
		}
	}
	return hResult;
}


	// get //
ONDK_Result_t		GWM_TREE_GetTreeObject(HINT32 ulId, PTREE_OBJECT *pTreeObject)
{
	return local_treeobject_GetObject(ulId, pTreeObject);
}


ONDK_Result_t		GWM_TREE_GetTreeMgr(HINT32 ulId, HTREEMGR *phTreeMgr)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObj = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObj);
	if (hResult == GWM_RET_OK)
	{
		*phTreeMgr = pTreeObj->hTree;
	}

	return hResult;
}


ONDK_Result_t		GWM_TREE_GetTreeTotalItemCount(HINT32 ulId, HUINT32 *pulCnt)
{
	HTREEMGR		hTree = NULL;
	PTREEMGR		pTree = NULL;
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;

	hResult = GWM_TREE_GetTreeMgr(ulId, &hTree);
	if (hResult == GWM_RET_OK)
	{
		pTree	= (PTREEMGR)hTree;
		*pulCnt	= pTree->ulNumOfItem;
	}

	return hResult;
}

ONDK_Result_t		GWM_TREE_GetDrawItemInfo(PTREEOBJ_ITEM_DRAWINFO	pItem, HUINT32 *pulIndexAtTotal, HUINT32 *pulIndexAtPage,
											 HUINT32 *pulStatus, HUINT32 *pulDepth, HUINT32 *pulNumOfChild, void **ppLocalData)
{
	if (pItem == NULL)
	{
		return GWM_RET_INITIAL_FAILED;
	}

	*pulIndexAtTotal= pItem->ulIndexAtTotal;
	*pulIndexAtPage = pItem->ulIndexAtPage;
	*pulStatus		= pItem->ulStatus;
	*pulDepth		= pItem->ulDepth;
	*pulNumOfChild	= pItem->ulNumOfChild;
	*ppLocalData	= pItem->pLocalData;

	return GWM_RET_OK;
}


ONDK_Result_t		GWM_TREE_GetScrollInfo(HINT32 ulId, HUINT16 *pusTotalNumber, HUINT16 *pusSelectedIndexAtTotal)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObj = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObj);
	if (hResult != GWM_RET_OK)
	{
		return GWM_RET_INITIAL_FAILED;
	}

	if (pusTotalNumber)
	{
		*pusTotalNumber				= pTreeObj->usNumOfVisible;
	}

	if (pusSelectedIndexAtTotal)
	{
		*pusSelectedIndexAtTotal	= pTreeObj->usSelectedIndex;
	}

	return hResult;
}



ONDK_Result_t		GWM_TREE_SetLastItemOnLastIdx(HINT32 ulId, HBOOL bAllLayer, HUINT32 ulSelectIndex)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_TREE_FindItem_ByData] GetTreeMgr fail!!\n"));
	}
	else
	{
		hResult = local_treeobject_SetLastItemOnLastIdx(pTreeObject, bAllLayer, ulSelectIndex);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_GetItemData(HTREEITEM hItem, void **ppData)
{
	return local_treeobject_GetItemData(hItem, ppData);
}


ONDK_Result_t		GWM_TREE_FindItem_ByData(HINT32 ulId, HTREEITEM *phFindItem, void *pTargetData, FP_CompFunc compFunc)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEMGR		hTree = NULL;

	hResult = GWM_TREE_GetTreeMgr(ulId, &hTree);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_TREE_FindItem_ByData] GetTreeMgr fail!!\n"));
	}
	else
	{
		hResult = local_treeobject_FindItem_ByData(hTree, phFindItem, pTargetData, local_treeobject_FindNextItem, compFunc);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_FindItem_ByData_InAllItem(HINT32 ulId, HTREEITEM *phFindItem, void *pTargetData, FP_CompFunc compFunc)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEMGR		hTree = NULL;

	hResult = GWM_TREE_GetTreeMgr(ulId, &hTree);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_TREE_FindItem_ByData] GetTreeMgr fail!!\n"));
	}
	else
	{
		hResult = local_treeobject_FindItem_ByData(hTree, phFindItem, pTargetData, local_treeobject_FindNextItem_InAllItem, compFunc);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_GetCondMatchItemCount_ByData(HINT32 ulId, HUINT32 *pulCount, void *pTargetData, FP_CompFunc compFunc)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEMGR		hTree = NULL;

	hResult = GWM_TREE_GetTreeMgr(ulId, &hTree);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_TREE_GetCondMatchItemCount_ByData] GetTreeMgr fail!!\n"));
	}
	else
	{
		hResult = local_treeobject_GetCondMatchItemCount_ByData(hTree, pulCount, pTargetData, local_treeobject_FindNextItem, compFunc);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_GetCondMatchItemCount_ByData_InAllItem(HINT32 ulId, HUINT32 *pulCount, void *pTargetData, FP_CompFunc compFunc)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEMGR		hTree = NULL;

	hResult = GWM_TREE_GetTreeMgr(ulId, &hTree);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_TREE_GetCondMatchItemCount_ByData_InAllItem] GetTreeMgr fail!!\n"));
	}
	else
	{
		hResult = local_treeobject_GetCondMatchItemCount_ByData(hTree, pulCount, pTargetData, local_treeobject_FindNextItem_InAllItem, compFunc);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_GetDisplayStartItem(HINT32 ulId, HTREEITEM *phItem)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_GetDisplayStartItem(pTreeObject, phItem);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_SetDisplayStartItem(HINT32 ulId, HTREEITEM hItem)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_SetDisplayStartItem(pTreeObject, hItem);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_GetFirstChildItem(HINT32 ulId, HTREEITEM *phFirstChildItem)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEMGR		hTree = NULL;

	hResult = GWM_TREE_GetTreeMgr(ulId, &hTree);
	if (hResult == GWM_RET_OK)
	{
		local_treeobject_GetFirstChildItem(hTree, phFirstChildItem);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_GetRootItem(HINT32 ulId, HTREEITEM *phRootItem)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEMGR		hTree = NULL;

	hResult = GWM_TREE_GetTreeMgr(ulId, &hTree);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_GetRootItem(hTree, phRootItem);
	}
	return hResult;
}




ONDK_Result_t		GWM_TREE_GetChildNum(HTREEITEM hItem, HUINT32 *pulNumOfChild)
{
	return local_treeobject_GetChildNum(hItem, pulNumOfChild);
}

ONDK_Result_t		GWM_TREE_GetChildItem(HTREEITEM hItem, HTREEITEM *phChildItem)
{
	return local_treeobject_GetChildItem(hItem, phChildItem);
}


ONDK_Result_t		GWM_TREE_GetIndexOfItem(HINT32 ulId, HTREEITEM hTargetItem, HUINT32 *pulIndex)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEMGR		hTree = NULL;

	hResult = GWM_TREE_GetTreeMgr(ulId, &hTree);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_GetIndexOfItem(hTree, hTargetItem, local_treeobject_FindNextItem, pulIndex);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_GetSelectedItem(HINT32 ulId, HTREEITEM *phSelectedItem)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_GetSelectedItem(pTreeObject, phSelectedItem);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_FindNextItem(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	return local_treeobject_FindNextItem(hItem, phFindItem);
}


ONDK_Result_t		GWM_TREE_FindPrevItem(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	return local_treeobject_FindPrevItem(hItem, phFindItem);
}


ONDK_Result_t		GWM_TREE_FindNextItem_InAllItem(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	return local_treeobject_FindNextItem_InAllItem(hItem, phFindItem);
}


ONDK_Result_t		GWM_TREE_FindPrevItem_InAllItem(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	return local_treeobject_FindPrevItem_InAllItem(hItem, phFindItem);
}



ONDK_Result_t		GWM_TREE_FindNextItem_InSameLevel(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	return local_treeobject_FindNextItem_InSameLevel(hItem, phFindItem);
}


ONDK_Result_t		GWM_TREE_FindPrevItem_InSameLevel(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	return local_treeobject_FindPrevItem_InSameLevel(hItem, phFindItem);
}


	// functions //
ONDK_Result_t		GWM_TREE_SelectItem(HINT32 ulId, HTREEITEM hItem)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_SelectItem(pTreeObject, hItem);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_SelectNextItem(HINT32 ulId,  HTREEITEM *phSelectedItem)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_SelectNextItem(pTreeObject, phSelectedItem);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_SelectPrevItem(HINT32 ulId,  HTREEITEM *phSelectedItem)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_SelectPrevItem(pTreeObject, phSelectedItem);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_Collapse(HINT32 ulId, HTREEITEM hItem, HBOOL bSubCollapse)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_Collapse(pTreeObject, hItem, bSubCollapse);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_Expand(HINT32 ulId, HTREEITEM hItem, HBOOL bSubExpand)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_Expand(pTreeObject, hItem, bSubExpand);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_IsExpandItem(HINT32 ulId, HTREEITEM hItem, HBOOL *pbExpand)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_IsExpandItem(pTreeObject, hItem, pbExpand);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_SetObjDisable(HINT32 ulId, HBOOL bDisable)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_SetObjDisable(pTreeObject, bDisable);
	}
	return hResult;
}


ONDK_Result_t		GWM_TREE_SetOnlyOneItemExpandFlag(HINT32 ulId, HBOOL bOnlyOneItemExpand)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	PTREE_OBJECT	pTreeObject = NULL;

	hResult = local_treeobject_GetObject(ulId, &pTreeObject);
	if (hResult == GWM_RET_OK)
	{
		hResult = local_treeobject_SetOnlyOneItemExpandFlag(pTreeObject, bOnlyOneItemExpand);
	}
	return hResult;
}


#define ________LOCAL_FUNC________
static ONDK_Result_t		local_treeobject_Create(HINT32 ulId, OBJ_CREATE_t *pstObjCreateParam)
{
	PTREE_OBJECT		pNewTreeObject = NULL;
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;


/**************************************************************************************
 * object alloc
 **************************************************************************************/
	hResult = local_treeobject_GetObject(ulId, &pNewTreeObject);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Create] Param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = local_treeobject_AllocNewObject(&pNewTreeObject);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Create] AllocNewObject fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}


/**************************************************************************************
 * item rect setting
 **************************************************************************************/
	VK_memcpy(&(pNewTreeObject->rect), &pstObjCreateParam->stFrameRect, sizeof(ONDK_Rect_t));
	VK_memcpy(&(pNewTreeObject->stItemRect), &pstObjCreateParam->stItemRect, sizeof(ONDK_Rect_t));
	VK_memcpy(&(pNewTreeObject->stFrameMargin), &pstObjCreateParam->stMarginRect, sizeof(ONDK_Rect_t));


/**************************************************************************************
 * object basic info setting
 **************************************************************************************/
	pNewTreeObject->link = NULL;
	pNewTreeObject->ID = ulId;
	pNewTreeObject->type = GUIOBJ_TREE;
	pNewTreeObject->focus = GUIOBJ_NORMAL;


/**************************************************************************************
 * object callback setting
 **************************************************************************************/
	pNewTreeObject->Proc = (GWM_Proc_t)pstObjCreateParam->pfnProc;
	if(pstObjCreateParam->pfnProc == NULL)
	{
		pNewTreeObject->Proc = local_treeobject_defaultProc;
	}
	pNewTreeObject->objDrawItemMethod   = (GWM_ObjDraw_t)pstObjCreateParam->pfnObjDrawMethod;
	/*pNewTreeObject->objGetDrawItemRect  = (GWM_User_t)pstObjCreateParam->pfnObjUserMethod1;
	pNewTreeObject->objProcUpDown		= (GWM_User_t)pstObjCreateParam->pfnObjUserMethod2;
	pNewTreeObject->objProcLeftRight	= (GWM_User_t)pstObjCreateParam->pfnObjUserMethod3;
	pNewTreeObject->objProcOK			= (GWM_User_t)pstObjCreateParam->pfnObjUserMethod4;*/


/**************************************************************************************
 * object etc info setting
 **************************************************************************************/
	pNewTreeObject->usItemsPerPage = pstObjCreateParam->usItemsPerPage;


/**************************************************************************************
 * object create & register, invalidate
 **************************************************************************************/
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)pNewTreeObject, 0);

	hResult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)pNewTreeObject);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Create] AddObject fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}


	ONDK_GWM_APP_InvalidateRelativeRect(	pNewTreeObject->rect.x,
						pNewTreeObject->rect.y,
						pNewTreeObject->rect.w,
						pNewTreeObject->rect.h);

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_GetObject(HINT32 ulId, TREE_OBJECT **ppTreeObj)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	ONDK_GWM_GuiObject_t		pObjectList = NULL;
	PTREE_OBJECT		pTreeObj = NULL;


	if(ppTreeObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_explorer_GetObject] Param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*ppTreeObj = NULL;

	GWM_assert(g_pstCurrentApp);

	pObjectList = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);
	if(pObjectList == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_explorer_GetObject] GetObjectList fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	pTreeObj = (PTREE_OBJECT)INT_ONDK_GWM_OBJ_FindObject(pObjectList, (int)ulId);
	if(pObjectList == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_explorer_GetObject] FindObject fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	*ppTreeObj = pTreeObj;
	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_AllocNewObject(TREE_OBJECT **ppTreeObj)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HUINT32				ulIndex = 0;
	HTREEMGR			hTree = NULL;
	PTREE_OBJECT		pTreeObj = NULL;
	HERROR				hError = ERR_FAIL;


	if(ppTreeObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_explorer_AllocNewObject] Param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*ppTreeObj = NULL;


	for (ulIndex = 0; ulIndex < MAX_TREE_OBJECT; ulIndex++)
	{
		if (s_stTreeManager.ucIsAlloced[ulIndex] == 0)
		{
			break;
		}
	}

	if (ulIndex >= MAX_TREE_OBJECT)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_explorer_AllocNewObject] Fild available slot fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}


	hError = UTIL_TREEUTIL_CreateTree(&hTree);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_explorer_AllocNewObject] CreateTree fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;

	}

	pTreeObj = &(s_stTreeManager.astTreeObject[ulIndex]);
	pTreeObj->hTree = hTree;
	*ppTreeObj = pTreeObj;
	s_stTreeManager.ucIsAlloced[ulIndex] = 1;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}




static ONDK_Result_t		local_treeobject_FreeObject(HINT32 ulId)
{
	PTREE_OBJECT		pTreelObj = NULL;

	pTreelObj = (PTREE_OBJECT)ONDK_GWM_APP_GetObject(ulId);

	return local_treeobject_FreeObject_ByPObj(pTreelObj);
}


static ONDK_Result_t		local_treeobject_FreeObject_ByPObj(PTREE_OBJECT pTreelObj)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HUINT32				ulIndex = 0;
	HERROR				hError = ERR_FAIL;

	if(pTreelObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FreeObject] GetObject fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	for (ulIndex = 0; ulIndex < MAX_TREE_OBJECT; ulIndex++)
	{
		if (pTreelObj == &(s_stTreeManager.astTreeObject[ulIndex]))
		{
			break;
		}
	}

	if (ulIndex >= MAX_TREE_OBJECT)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FreeObject] Fild available slot fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}


	hError = UTIL_TREEUTIL_DestroyTree(pTreelObj->hTree);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FreeObject] DestroyTree fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	s_stTreeManager.ucIsAlloced[ulIndex] = 0;

	VK_memset(&(s_stTreeManager.astTreeObject[ulIndex]), 0x00, sizeof(TREE_OBJECT));
	INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(s_stTreeManager.astTreeObject[ulIndex]));

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_treeobject_InsertItem(HTREEMGR hTree, HTREEITEM hParent, HTREEITEM hPrevLink, HTREEITEM hNextLink, TREEITEM_PARAM_t *pParam, HTREEITEM *phNewItem)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;

	if(hTree == NULL || pParam == NULL || phNewItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_InsertItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;

	}

	hError = UTIL_TREEUTIL_InsertItem(hTree, hParent, hPrevLink, hNextLink, pParam, phNewItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_InsertItem] InsertItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}

static ONDK_Result_t		local_treeobject_RemoveItem(HTREEMGR hTree, HTREEITEM hItem)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;

	if(hTree == NULL || hItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_RemoveItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;

	}

	hError = UTIL_TREEUTIL_RemoveItem(hTree, hItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_RemoveItem] RemoveItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}






static ONDK_Result_t		local_treeobject_FindNextItem(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	TREEOBJ_ITEMINFO	stItemInfo;
	HTREEITEM			hChildItem = NULL;
	HTREEITEM			hParentItem = NULL;
	HTREEITEM			hNextItem = NULL;
	HTREEITEM			hFindItem = NULL;
	HUINT32				ulDepth = 0;

	if(hItem == NULL || phFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phFindItem = NULL;

	VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
	hError = UTIL_TREEUTIL_GetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem] GetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	hError = UTIL_TREEUTIL_GetChildItem(hItem, &hChildItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem] GetChildItem fail!!\n"));
	}
	hError = UTIL_TREEUTIL_GetNextItem(hItem, &hNextItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem] GetNextItem fail!!\n"));
	}
	hError = UTIL_TREEUTIL_GetItemDepth(hItem, &ulDepth);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem] GetItemDepth fail!!\n"));
	}

	if (stItemInfo.ulItemStatus & TREEITEM_EXPANDED)
	{
		hFindItem = hChildItem;
	}
	if (hFindItem == NULL)
	{
		hFindItem = hNextItem;
	}

	//if parent is root and item is layer end item
	if (hFindItem == NULL && ulDepth == 1)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem] parent is root and item is layer end item !!\n"));

		hResult = GWM_RET_OK;
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetParentItem(hItem, &hParentItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem] GetParentItem fail !!\n"));
	}

	/* child(expand case) or next(collapse case) item not exit => parent's next item */
	if (hFindItem == NULL)
	{
		while (hParentItem)
		{
			hError = UTIL_TREEUTIL_GetNextItem(hParentItem, &hNextItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem] GetNextItem fail !!\n"));
			}

			if(hNextItem != NULL)
			{
				hFindItem = hNextItem;
				break;
			}

			hError = UTIL_TREEUTIL_GetParentItem(hParentItem, &hParentItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem] GetParentItem fail !!\n"));
			}
		}	// end while (hParentItem)
	}

	*phFindItem = hFindItem;

	if (hFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem] Can't search next item !!\n"));
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}





///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Parent의 자녀 아이템 중에서 가장 끝(Next)에 있는 아이템을 찾는다.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
static ONDK_Result_t		local_treeobject_FindLastChild(HTREEITEM hParent, HTREEITEM *phFindItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hTempItem = NULL;
	HTREEITEM			hNextItem = NULL;
	HTREEITEM			hFindItem = NULL;

	if(hParent == NULL || phFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastChild] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phFindItem = NULL;

	hError = UTIL_TREEUTIL_GetChildItem(hParent, &hTempItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastChild] GetChildItem fail!!\n"));
	}
	if (hTempItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastChild] no child!!\n"));

		hResult = GWM_RET_OK;
		goto EXIT_TREEOBJ_FUNC;
	}
	hFindItem = hTempItem;

	hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hNextItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastChild] GetNextItem fail!!\n"));
	}

	while (hNextItem != NULL)
	{
		hFindItem = hNextItem;
		hError = UTIL_TREEUTIL_GetNextItem(hNextItem, &hNextItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastChild] GetNextItem fail!!\n"));
		}
	}

	*phFindItem = hFindItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}





///////////////////////////////////////////////////////////////////////////////////////////////////
//
// tree의 가장 하층에서 최종 하위 아이템을 찾는다.
// 상위로 올라갈경우 상위 형제의 가장 아랫단 child를 호출할경우
//
///////////////////////////////////////////////////////////////////////////////////////////////////
static ONDK_Result_t		local_treeobject_FindLastDescItem(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	TREEOBJ_ITEMINFO	stItemInfo;
	HTREEITEM			hLastChildItem = NULL;
	HTREEITEM			hTempItem = NULL;
	HTREEITEM			hFindItem = NULL;

	if(hItem == NULL || phFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastDescItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phFindItem = NULL;

	VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
	hError = UTIL_TREEUTIL_GetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastDescItem] GetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	if (stItemInfo.ulItemStatus & TREEITEM_EXPANDED)
	{
		hError = local_treeobject_FindLastChild(hItem, &hLastChildItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastDescItem] FindLastChild fail!!\n"));
		}
		hFindItem = hLastChildItem;
		if(hLastChildItem != NULL)
		{
			hError = local_treeobject_FindLastDescItem(hLastChildItem, &hTempItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastDescItem] FindLastDescItem fail!!\n"));
				goto EXIT_TREEOBJ_FUNC;
			}
			if (hTempItem != NULL)
			{	// 찾아진 경우에만 대입
				hFindItem = hTempItem;
			}
		}
	}	// end if (stItemInfo.ulItemStatus & TREEITEM_EXPANDED)

	*phFindItem = hFindItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}





///////////////////////////////////////////////////////////////////////////////////////////////////
//
// prev가 펼쳐진경우 (Extended) prev의 최하단 last child가 focus된다
//
///////////////////////////////////////////////////////////////////////////////////////////////////
static ONDK_Result_t		local_treeobject_FindPrevItem(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hParentItem = NULL;
	HTREEITEM			hPrevItem = NULL;
	HTREEITEM			hTempItem = NULL;
	HTREEITEM			hFindItem = NULL;
	HUINT32				ulDepth = 0;

	if(hItem == NULL || phFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phFindItem = NULL;

	hError = UTIL_TREEUTIL_GetPrevItem(hItem, &hPrevItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem] GetPrevItem fail!!\n"));
	}
	hFindItem = hPrevItem;

	if(hPrevItem != NULL)
	{
		hError = local_treeobject_FindLastDescItem(hPrevItem, &hTempItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem] FindLastDescItem fail!!\n"));
		}

		if (hTempItem != NULL)
		{	/* local_treeobject_FindLastDescItem()에서 찾아진 것이 있을 경우에만 대입 */
			hFindItem = hTempItem;
		}
	}	// end if(hPrevItem != NULL)

	if(hFindItem == NULL)
	{
		hError = UTIL_TREEUTIL_GetParentItem(hItem, &hParentItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem] GetParentItem fail !!\n"));
		}
		hError = UTIL_TREEUTIL_GetItemDepth(hParentItem, &ulDepth);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem] GetItemDepth fail!!\n"));
		}

		if(	hParentItem != NULL
			&& ulDepth != 0)
		{
			hFindItem = hParentItem;
		}
	}

	*phFindItem = hFindItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_FindNextItem_InAllItem(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hChildItem = NULL;
	HTREEITEM			hParentItem = NULL;
	HTREEITEM			hNextItem = NULL;
	HTREEITEM			hFindItem = NULL;
	HUINT32				ulDepth = 0;

	if(hItem == NULL || phFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InAllItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phFindItem = NULL;

	hError = UTIL_TREEUTIL_GetChildItem(hItem, &hChildItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InAllItem] GetChildItem fail!!\n"));
	}
	hError = UTIL_TREEUTIL_GetNextItem(hItem, &hNextItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InAllItem] GetNextItem fail!!\n"));
	}
	hError = UTIL_TREEUTIL_GetItemDepth(hItem, &ulDepth);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InAllItem] GetItemDepth fail!!\n"));
	}

	hFindItem = hChildItem;
	if (hFindItem == NULL)
	{
		hFindItem = hNextItem;
	}

	//if parent is root and item is layer end item
	if (hFindItem == NULL && ulDepth == 1)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InAllItem] parent is root and item is layer end item !!\n"));

		hResult = GWM_RET_OK;
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetParentItem(hItem, &hParentItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InAllItem] GetParentItem fail !!\n"));
	}

	/* child or next item not exit => parent's next item */
	if (hFindItem == NULL)
	{
		while (hParentItem)
		{
			hError = UTIL_TREEUTIL_GetNextItem(hParentItem, &hNextItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InAllItem] GetNextItem fail !!\n"));
			}

			if(hNextItem != NULL)
			{
				hFindItem = hNextItem;
				break;
			}

			hError = UTIL_TREEUTIL_GetParentItem(hParentItem, &hParentItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InAllItem] GetParentItem fail !!\n"));
			}
		}	// end while (hParentItem)
	}

	*phFindItem = hFindItem;

	if (hFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InAllItem] Can't search next item !!\n"));
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_treeobject_FindLastDescItem_InAllItem(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hLastChildItem = NULL;
	HTREEITEM			hTempItem = NULL;
	HTREEITEM			hFindItem = NULL;

	if(hItem == NULL || phFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastDescItem_InAllItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phFindItem = NULL;

	{
		hError = local_treeobject_FindLastChild(hItem, &hLastChildItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastDescItem_InAllItem] FindLastChild fail!!\n"));
		}
		hFindItem = hLastChildItem;
		if(hLastChildItem != NULL)
		{
			hError = local_treeobject_FindLastDescItem_InAllItem(hLastChildItem, &hTempItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindLastDescItem_InAllItem] FindLastDescItem fail!!\n"));
				goto EXIT_TREEOBJ_FUNC;
			}
			if (hTempItem != NULL)
			{	// 찾아진 경우에만 대입
				hFindItem = hTempItem;
			}
		}
	}

	*phFindItem = hFindItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_treeobject_FindPrevItem_InAllItem(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hParentItem = NULL;
	HTREEITEM			hPrevItem = NULL;
	HTREEITEM			hTempItem = NULL;
	HTREEITEM			hFindItem = NULL;
	HUINT32				ulDepth = 0;

	if(hItem == NULL || phFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem_InAllItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phFindItem = NULL;

	hError = UTIL_TREEUTIL_GetPrevItem(hItem, &hPrevItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem_InAllItem] GetPrevItem fail!!\n"));
	}
	hFindItem = hPrevItem;

	if(hPrevItem != NULL)
	{
		hError = local_treeobject_FindLastDescItem(hPrevItem, &hTempItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem_InAllItem] FindLastDescItem_InAllItem fail!!\n"));
		}

		if (hTempItem != NULL)
		{	/* local_treeobject_FindLastDescItem()에서 찾아진 것이 있을 경우에만 대입 */
			hFindItem = hTempItem;
		}
	}	// end if(hPrevItem != NULL)

	if(hFindItem == NULL)
	{
		hError = UTIL_TREEUTIL_GetParentItem(hItem, &hParentItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem_InAllItem] GetParentItem fail !!\n"));
		}
		hError = UTIL_TREEUTIL_GetItemDepth(hParentItem, &ulDepth);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem_InAllItem] GetItemDepth fail!!\n"));
		}

		if(	hParentItem != NULL
			&& ulDepth != 0)
		{
			hFindItem = hParentItem;
		}
	}

	*phFindItem = hFindItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_treeobject_FindNextItem_InSameLevel(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hFindItem = NULL;

	if(hItem == NULL || phFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InSameLevel] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phFindItem = NULL;

	hError = UTIL_TREEUTIL_GetNextItem(hItem, &hFindItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindNextItem_InSameLevel] GetNextItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	*phFindItem = hFindItem;
	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :
	return hResult;
}


static ONDK_Result_t		local_treeobject_FindPrevItem_InSameLevel(HTREEITEM hItem, HTREEITEM *phFindItem)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hFindItem = NULL;

	if(hItem == NULL || phFindItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem_InSameLevel] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phFindItem = NULL;

	hError = UTIL_TREEUTIL_GetPrevItem(hItem, &hFindItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_FindPrevItem_InSameLevel] GetNextItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	*phFindItem = hFindItem;
	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :
	return hResult;
}



static ONDK_Result_t		local_treeobject_Collapse(PTREE_OBJECT pTreeObject, HTREEITEM hItem, HBOOL bSubCollapse)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	TREEOBJ_ITEMINFO	stItemInfo;
	HTREEITEM			hChildItem = NULL;


	if(pTreeObject == NULL || hItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Collapse] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetChildItem(hItem, &hChildItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Collapse] GetChildItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
	hError = UTIL_TREEUTIL_GetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Collapse] GetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	stItemInfo.ulItemStatus &= ~TREEITEM_EXPANDED;
	hError = UTIL_TREEUTIL_SetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)(&stItemInfo));
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Collapse] SetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	if (bSubCollapse == TRUE)
	{
		local_treeobject_CollapseAll(pTreeObject, hItem);
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_CollapseAll(PTREE_OBJECT pTreeObject, HTREEITEM hItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	TREEOBJ_ITEMINFO	stItemInfo;
	HTREEITEM			hChildItem = NULL;


	if(pTreeObject == NULL || hItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_CollapseAll] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetChildItem(hItem, &hChildItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_CollapseAll] GetChildItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	while (hChildItem)
	{
		hResult = local_treeobject_CollapseAll(pTreeObject, hChildItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_CollapseAll] CollapseAll fail!!\n"));
		}

		hError = UTIL_TREEUTIL_GetNextItem(hChildItem, &hChildItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_CollapseAll] GetNextItem fail!!\n"));
		}
	}

	VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
	hError = UTIL_TREEUTIL_GetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_CollapseAll] GetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	stItemInfo.ulItemStatus &= ~TREEITEM_EXPANDED;
	hError = UTIL_TREEUTIL_SetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)(&stItemInfo));
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_CollapseAll] SetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = local_treeobject_SetDisplayStartItem(pTreeObject, hItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_CollapseAll] SetDisplayStartitem fail!!\n"));
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}





static ONDK_Result_t		local_treeobject_SetDisplayStartItem(PTREE_OBJECT pTreeObject, HTREEITEM hItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hRootItem = NULL;
	HTREEITEM			hChildItem = NULL;


	if(pTreeObject == NULL || hItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetDisplayStartItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}


	hError = UTIL_TREEUTIL_GetRootItem(pTreeObject->hTree, &hRootItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetDisplayStartItem] GetRootItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	if(hRootItem != hItem)
	{
		pTreeObject->hDispStart = hItem;
	}
	else
	{
		hError = UTIL_TREEUTIL_GetChildItem(hItem, &hChildItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetDisplayStartItem] GetChildItem fail!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}
		pTreeObject->hDispStart = hChildItem;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_GetDisplayStartItem(PTREE_OBJECT pTreeObject, HTREEITEM *phItem)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;

	if(pTreeObject == NULL || phItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetDisplayStartItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	*phItem = pTreeObject->hDispStart;
	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


// definded.. but not used..
#if 0
static ONDK_Result_t		local_treeobject_SetDisplayFocusAtMiddle(PTREE_OBJECT pTreeObject, HTREEITEM hItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HTREEITEM			hStartItem = NULL;
	HUINT16				usIndex = 0;


	if(pTreeObject == NULL || hItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetDisplayFocusAtMiddle] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hStartItem = hItem;
	for(usIndex = 0; usIndex < pTreeObject->usItemsPerPage/2; usIndex++)
	{
		hResult = local_treeobject_SetDisplayStartItem(pTreeObject, hStartItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetDisplayFocusAtMiddle] SetDisplayStartitem fail!!\n"));
		}

		hResult = local_treeobject_FindPrevItem(hStartItem, &hStartItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetDisplayFocusAtMiddle] FindPrevItem fail!!\n"));
		}

		if(hStartItem == NULL)
		{
			break;
		}
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}

static ONDK_Result_t		local_treeobject_GetLayerEndItem(HTREEITEM hItem, HTREEITEM *phLayerEndItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hEndItem = NULL;
	HTREEITEM			hTempItem = NULL;

	if(hItem == NULL || phLayerEndItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetLayerEndItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phLayerEndItem = NULL;

	hEndItem = hItem;
	hError = UTIL_TREEUTIL_GetNextItem(hItem, &hTempItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetLayerEndItem] GetNextItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	while(hTempItem)
	{
		hEndItem = hTempItem;
		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetLayerEndItem] GetNextItem fail!!\n"));
		}
	}
	*phLayerEndItem = hEndItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}

static ONDK_Result_t		local_treeobject_GetLayerStartItem(HTREEITEM hItem, HTREEITEM *phLayerStartItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hStartItem = NULL;
	HTREEITEM			hTempItem = NULL;

	if(hItem == NULL || phLayerStartItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetLayerStartItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phLayerStartItem = NULL;

	hStartItem = hItem;
	hError = UTIL_TREEUTIL_GetPrevItem(hItem, &hTempItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetLayerStartItem] GetPrevItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	while(hTempItem)
	{
		hStartItem = hTempItem;
		hError = UTIL_TREEUTIL_GetPrevItem(hTempItem, &hTempItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetLayerStartItem] GetPrevItem fail!!\n"));
		}
	}

	*phLayerStartItem = hStartItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}

static ONDK_Result_t		local_treeobject_GetDispStartItem(PTREE_OBJECT pTreeObject, HTREEITEM *phDispStartItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;

	if(pTreeObject == NULL || phDispStartItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetDispStartItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phDispStartItem = pTreeObject->hDispStart;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}

static ONDK_Result_t		local_treeobject_GetNextPageItem(	PTREE_OBJECT pTreeObject,
																HTREEITEM hItem,
																HBOOL bAllLayer,
																HTREEITEM *phNextItem)
{
	HTREEITEM			hSelected = NULL;
	HTREEITEM			hTempItem = NULL;
	HUINT16				usItemsPerPage = 0;
	HUINT16				usIndex = 0;
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HBOOL				bIsPage = FALSE;


	if(pTreeObject == NULL || phNextItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetNextPageItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phNextItem = hItem;


	hTempItem = hItem;
	usItemsPerPage = pTreeObject->usItemsPerPage;

	for(usIndex = 0; usIndex < usItemsPerPage-1; usIndex++)
	{
		if(bAllLayer == FALSE)
		{
			hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetNextPageItem] GetNextItem fail!!\n"));
			}
		}
		else
		{
			hResult = local_treeobject_FindNextItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetNextPageItem] FindNextItem fail!!\n"));
			}
		}

		hResult = local_treeobject_IsPageItem(pTreeObject, hTempItem, &bIsPage);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetNextPageItem] IsPageItem fail!!\n"));
		}

		if(hTempItem != NULL && bIsPage == FALSE)
		{
			hSelected = hTempItem;
		}
	}

	*phNextItem = hSelected;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;

}


static ONDK_Result_t		local_treeobject_GetLastExpand(PTREE_OBJECT pTreeObject, HTREEITEM *phLastExpandItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;

	if(pTreeObject == NULL || phLastExpandItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetEnableItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	*phLastExpandItem = pTreeObject->hLastExpandItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}

static ONDK_Result_t		local_treeobject_SetLastExpand(PTREE_OBJECT pTreeObject,  HTREEITEM hItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;

	if(pTreeObject == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetEnableItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	pTreeObject->hLastExpandItem = hItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_treeobject_GetItemRect(PTREE_OBJECT pTreeObject, HTREEITEM hItem, ONDK_Rect_t *pItemRect)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM 			hStart = NULL;
	HTREEITEM 			hFirstChild = NULL;
	HTREEITEM 			hTemp = NULL;
	HUINT16				usIndex = 0;
	ONDK_Rect_t			stItemRect;
	HBOOL				bDetected = FALSE;

	if(pTreeObject == NULL || pItemRect == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetItemRect] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	VK_memset(pItemRect, 0x00, sizeof(ONDK_Rect_t));
	VK_memset(&stItemRect, 0x00, sizeof(ONDK_Rect_t));
	hStart = pTreeObject->hDispStart;

	if(hStart == NULL)
	{
		hError = UTIL_TREEUTIL_GetFirstChildItem(pTreeObject->hTree, &hFirstChild);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetItemRect] GetFirstChildItem fail!!\n"));
		}
		hStart = hFirstChild;
	}

	hTemp = hStart;
	if(hTemp == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetItemRect] tree haven't any child!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	do
	{
		if(hTemp == hItem)
		{
			hResult = local_treeobject_GetDisplayItemRect(pTreeObject, usIndex, &stItemRect);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetItemRect] GetDisplayItemRect fail!!\n"));
				goto EXIT_TREEOBJ_FUNC;
			}
			VK_memcpy(pItemRect, &stItemRect, sizeof(ONDK_Rect_t));
			bDetected = TRUE;
			break;
		}

		hResult = local_treeobject_FindNextItem(hTemp, &hTemp);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetItemRect] FindNextItem fail!!\n"));
		}
		usIndex++;
		if(usIndex >= pTreeObject->usItemsPerPage)
		{
			break;
		}

	}while(hTemp != NULL);

	if(bDetected == TRUE)
	{
		hResult = GWM_RET_OK;
	}

EXIT_TREEOBJ_FUNC :

	return hResult;
}

#endif


static ONDK_Result_t		local_treeobject_SetReverseDisplayStartItem(PTREE_OBJECT pTreeObject, HTREEITEM hItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hLastItem = NULL;
	HTREEITEM			hDispItem = NULL;
	HUINT16				usIndex = 0;
	HUINT32				ulDepth = 0;

	if(pTreeObject == NULL || hItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetReverseDisplayStartItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hDispItem = hLastItem = hItem;

	GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetReverseDisplayStartItem] usItemsPerPage(%d)\n", pTreeObject->usItemsPerPage));

	for(usIndex = 0; usIndex < pTreeObject->usItemsPerPage; usIndex++)
	{
		hLastItem = hDispItem;
		hResult = local_treeobject_FindPrevItem(hDispItem, &hDispItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetReverseDisplayStartItem] FindPrevItem fail!!\n"));
		}

		hError = UTIL_TREEUTIL_GetItemDepth(hDispItem, &ulDepth);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetReverseDisplayStartItem] GetItemDepth fail!!\n"));
		}

		//if tree root
		if(hDispItem == NULL || ulDepth == 0)
		{
			break;
		}
	}

	pTreeObject->hDispEnd = hItem;
	pTreeObject->hDispStart= hLastItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;

}




static ONDK_Result_t		local_treeobject_GetDisplayLastItem(PTREE_OBJECT pTreeObject, HTREEITEM *phDispLastItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hLastItem	  = NULL;
	HTREEITEM			hLastDescItem = NULL;
	HTREEITEM			hRootItem	  = NULL;

	if(pTreeObject == NULL || phDispLastItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetDisplayLastItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phDispLastItem = NULL;

	hError = UTIL_TREEUTIL_GetRootItem(pTreeObject->hTree, &hRootItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetDisplayLastItem] GetRootItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = local_treeobject_FindLastChild(hRootItem, &hLastItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetDisplayLastItem] FindLastChild fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = local_treeobject_FindLastDescItem(hLastItem, &hLastDescItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetDisplayLastItem] FindLastDescItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	if (hLastDescItem != NULL)
	{
		hLastItem = hLastDescItem;
	}
	*phDispLastItem = hLastItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}

static ONDK_Result_t		local_treeobject_GetSelectedItem(PTREE_OBJECT pTreeObject, HTREEITEM *phSelectedItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;

	if(pTreeObject == NULL || phSelectedItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetSelectedItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phSelectedItem = pTreeObject->hSelectedItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_ExpandItem(	PTREE_OBJECT pTreeObject,
															HBOOL bSubExpand,
															HBOOL bAllExpand,
															HUINT32 *pulExpandStatus)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hSelectedItem = NULL;
	TREEOBJ_ITEMINFO	stItemInfo;
	HUINT32				ulExpandStatus = 0;
	HTREEITEM 			hRootItem = NULL;

	if(pTreeObject == NULL || pulExpandStatus == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*pulExpandStatus = 0;
	hSelectedItem = pTreeObject->hSelectedItem;

	VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
	hError = UTIL_TREEUTIL_GetItemUserInfo(hSelectedItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandItem] GetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	if(stItemInfo.ulItemStatus & TREEITEM_EXPANDED)
	{
		hResult = local_treeobject_Collapse(pTreeObject, hSelectedItem, bSubExpand);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandItem] Collaps fail!!\n"));
		}
	}
	else
	{
		//only one item expand
		if(bAllExpand == TRUE)
		{
			hError = UTIL_TREEUTIL_GetRootItem(pTreeObject->hTree, &hRootItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandItem] GetRootItem fail!!\n"));
			}
			hResult = local_treeobject_CollapseAll(pTreeObject, hRootItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandItem] CollapseAll fail!!\n"));
			}
		}

		hResult = local_treeobject_Expand(pTreeObject, hSelectedItem, bSubExpand);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandItem] Expand fail!!\n"));
		}
		ulExpandStatus = TREEITEM_EXPANDED;
	}

	hResult = local_treeobject_SetDisplayPage(pTreeObject, hSelectedItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandItem] SetDisplayPage fail!!\n"));
	}
	hResult = local_treeobject_GetVisibleItemCount(	pTreeObject,
													pTreeObject->hSelectedItem,
													&pTreeObject->usNumOfVisible,
													&pTreeObject->usSelectedIndex);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandItem] GetVisibleItemCount fail!!\n"));
	}

	*pulExpandStatus = ulExpandStatus;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}




static ONDK_Result_t		local_treeobject_Expand(PTREE_OBJECT pTreeObject,  HTREEITEM hItem, HBOOL bSubExpand)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hChildItem = NULL;
	HTREEITEM			hRootItem = NULL;
	HTREEITEM			hParentItem = NULL;
	TREEOBJ_ITEMINFO	stItemInfo;

	if(pTreeObject == NULL || hItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetChildItem(hItem, &hChildItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] GetChildItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetRootItem(pTreeObject->hTree, &hRootItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] GetRootItem fail!!\n"));
	}

	if(hItem ==hRootItem)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] Item is root!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	// hItem 자신 및 모든 조상이 TREEITEM_EXPANDED가 되도록 만든다.
	if(hChildItem)
	{
		VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
		hError = UTIL_TREEUTIL_GetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] GetItemUserInfo fail!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}

		stItemInfo.ulItemStatus |= TREEITEM_EXPANDED;
		hError = UTIL_TREEUTIL_SetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)(&stItemInfo));
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] SetItemUserInfo fail!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}

		 //last expand item
		pTreeObject->hLastExpandItem = hItem;
	}


	hError = UTIL_TREEUTIL_GetParentItem(hItem, &hParentItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] GetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	while (hParentItem)
	{
		VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
		hError = UTIL_TREEUTIL_GetItemUserInfo(hParentItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] GetItemUserInfo fail!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}

		stItemInfo.ulItemStatus |= TREEITEM_EXPANDED;
		hError = UTIL_TREEUTIL_SetItemUserInfo(hParentItem, sizeof(TREEOBJ_ITEMINFO), (void*)(&stItemInfo));
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] SetItemUserInfo fail!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}

		hError = UTIL_TREEUTIL_GetParentItem(hParentItem, &hParentItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] GetParentItem fail!!\n"));
		}
	}

	//recursive item expand
	if (bSubExpand == TRUE)
	{
		hResult = local_treeobject_ExpandAll(hItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_Expand] ExpandAll fail!!\n"));
		}
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_treeobject_ExpandAll(HTREEITEM hItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hChildItem = NULL;
	TREEOBJ_ITEMINFO	stItemInfo;

	if(hItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetChildItem(hItem, &hChildItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] GetChildItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	while (hChildItem)
	{
		hResult = local_treeobject_ExpandAll(hChildItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] ExpandAll fail!!\n"));
		}

		hError = UTIL_TREEUTIL_GetNextItem(hChildItem, &hChildItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] GetChildItem fail!!\n"));
		}
	}

	VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
	hError = UTIL_TREEUTIL_GetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] GetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	stItemInfo.ulItemStatus |= TREEITEM_EXPANDED;
	hError = UTIL_TREEUTIL_SetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)(&stItemInfo));
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] SetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;

}



static ONDK_Result_t		local_treeobject_IsExpandItem(PTREE_OBJECT pTreeObject,  HTREEITEM hItem, HBOOL *pbExpand)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hChildItem = NULL;
	HTREEITEM			hRootItem = NULL;
	TREEOBJ_ITEMINFO	stItemInfo;

	if(pTreeObject == NULL || hItem == NULL || pbExpand == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_IsExpandItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*pbExpand = FALSE;

	hError = UTIL_TREEUTIL_GetRootItem(pTreeObject->hTree, &hRootItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_IsExpandItem] GetRootItem fail!!\n"));
	}

	if(hItem == hRootItem)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_IsExpandItem] item is root!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetChildItem(hItem, &hChildItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_IsExpandItem] GetChildItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
	hError = UTIL_TREEUTIL_GetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_IsExpandItem] GetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	if(	stItemInfo.ulItemStatus & TREEITEM_EXPANDED
		&& hChildItem !=NULL )
	{
		*pbExpand = TRUE;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;

}




static ONDK_Result_t		local_treeobject_SelectItem(PTREE_OBJECT pTreeObject, HTREEITEM hItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hParentItem = NULL;
	TREEOBJ_ITEMINFO	stItemInfo;

	if(pTreeObject == NULL || hItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	// 상위 아이템이 있다면 상위 아이템이 확장된 형태로 상태를 만든다
	/////////////////////////////////////////////////////////////////////////////////////////////////
	hError = UTIL_TREEUTIL_GetParentItem(hItem, &hParentItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectItem] GetParentItem fail!!\n"));
	}

	hResult = local_treeobject_Expand(pTreeObject, hParentItem, FALSE);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] ExpandAll fail!!\n"));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	// XOR
	////////////////////////////////////////////////////////////////////////////////////////////////
	if(pTreeObject->hSelectedItem)
	{
		VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
		hError = UTIL_TREEUTIL_GetItemUserInfo(pTreeObject->hSelectedItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] GetItemUserInfo fail!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}
		stItemInfo.ulItemStatus &= ~TREEITEM_SELECTED;
		hError = UTIL_TREEUTIL_SetItemUserInfo(pTreeObject->hSelectedItem, sizeof(TREEOBJ_ITEMINFO), (void*)(&stItemInfo));
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] SetItemUserInfo fail!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}
	}

	VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
	hError = UTIL_TREEUTIL_GetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] GetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	stItemInfo.ulItemStatus |= TREEITEM_SELECTED;
	hError = UTIL_TREEUTIL_SetItemUserInfo(hItem, sizeof(TREEOBJ_ITEMINFO), (void*)(&stItemInfo));
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_ExpandAll] SetItemUserInfo fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	pTreeObject->hSelectedItem = hItem;

	hResult = local_treeobject_SetDisplayPage(pTreeObject, hItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectItem] SetDisplayPage fail!!\n"));
	}
	// current scroll setting
	hError = local_treeobject_GetVisibleItemCount(	pTreeObject,
												pTreeObject->hSelectedItem,
												&pTreeObject->usNumOfVisible,
												&pTreeObject->usSelectedIndex);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectItem] GetVisibleItemCount fail!!\n"));
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_SelectNextItem(PTREE_OBJECT pTreeObject, HTREEITEM *phSelectedItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hItem = NULL;
	HTREEITEM			hSelectedItem = NULL;
	HBOOL				bIsPage = FALSE;

	if(pTreeObject == NULL || phSelectedItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectNextItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phSelectedItem = pTreeObject->hSelectedItem;

	hSelectedItem	= pTreeObject->hSelectedItem;
	if (hSelectedItem == NULL)
	{
		hError = UTIL_TREEUTIL_GetFirstChildItem(pTreeObject->hTree, &hItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectNextItem] GetFirstChildItem fail!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}
		pTreeObject->hDispStart = hItem;
		if(hItem == NULL)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectNextItem] no item!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}
	}
	else
	{
		hResult = local_treeobject_FindNextItem(hSelectedItem, &hItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectNextItem] FindNextItem fail!!\n"));
		}
		//if last item => return ERR_FAIL
		if(hItem == NULL)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectNextItem] last item!!\n"));
			hResult = ERR_FAIL;
			goto EXIT_TREEOBJ_FUNC;
		}
	}

////////////////////////////////////////////////////////////////////////////////////////////////
// when moving next page
/////////////////////////////////////////////////////////////////////////////////////////////////
	hResult = local_treeobject_IsPageItem(pTreeObject, hItem, &bIsPage);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectNextItem] IsPageItem fail!!\n"));
	}


	if(bIsPage == FALSE)
	{
		hResult = local_treeobject_FindNextItem(pTreeObject->hDispStart, &(pTreeObject->hDispStart));
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectNextItem] FindNextItem fail!!\n"));
		}
	}
	hResult = local_treeobject_SelectItem(pTreeObject, hItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectNextItem] SelectItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	*phSelectedItem = pTreeObject->hSelectedItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;

}



//-------------------------------------------------------------------------------------------------
static ONDK_Result_t		local_treeobject_SelectPrevItem(PTREE_OBJECT pTreeObject, HTREEITEM *phSelectedItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hItem = NULL;
	HTREEITEM			hSelectedItem = NULL;
	HTREEITEM			hFirstChild = NULL;
	HBOOL				bIsPage = FALSE;

	if(pTreeObject == NULL || phSelectedItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectNextItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phSelectedItem = NULL;

	hSelectedItem	= pTreeObject->hSelectedItem;

	hError = UTIL_TREEUTIL_GetFirstChildItem(pTreeObject->hTree, &hFirstChild);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectPrevItem] GetFirstChildItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

////////////////////////////////////////////////////////////////////////////////////////////////
// 선택된 아이템이 존재하지 않으면(최초상태) rootItem을 지정한다.
/////////////////////////////////////////////////////////////////////////////////////////////////
	if (hSelectedItem == NULL)
	{
		pTreeObject->hDispStart = hItem = hFirstChild;
		if(hItem == NULL)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectPrevItem] no item!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}
	}
	else
	{
		if (hSelectedItem == hFirstChild)
		{
////////////////////////////////////////////////////////////////////////////////////////////////
// 트리의 최하위 아이템부터 다시 올라가는 식으로 탐색한다.(circular)
// 06.07.19) 현재 지원하지 않는다
/////////////////////////////////////////////////////////////////////////////////////////////////
#if			0
			hError = UTIL_TREEUTIL_GetRootItem(pTreeObject->hTree, &hRootItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectPrevItem] GetRootItem fail!!\n"));
			}
			hResult = local_treeobject_FindLastDescItem(hRootItem, &hItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectPrevItem] FindLastDescItem fail!!\n"));
				goto EXIT_TREEOBJ_FUNC;
			}
#endif
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectPrevItem] hSelectedItem is root !!\n"));
			hResult = GWM_RET_OK;
			goto EXIT_TREEOBJ_FUNC;
		}
		else
		{
			hResult = local_treeobject_FindPrevItem(hSelectedItem, &hItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectPrevItem] FindNextItem fail!!\n"));
			}
		}
	}

	hResult = local_treeobject_IsPageItem(pTreeObject, hItem, &bIsPage);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectPrevItem] IsPageItem fail!!\n"));
	}
	if(bIsPage == FALSE)
	{
		hResult = local_treeobject_FindPrevItem(pTreeObject->hDispStart, &(pTreeObject->hDispStart));
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectPrevItem] FindPrevItem fail!!\n"));
		}
	}

	hResult = local_treeobject_SelectItem(pTreeObject, hItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SelectPrevItem] SelectItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	*phSelectedItem = pTreeObject->hSelectedItem;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}









static ONDK_Result_t		local_treeobject_GetVisibleItemCount(	PTREE_OBJECT pTreeObject,
																	HTREEITEM hSelectedItem,
																	HUINT16 *pusTotalNumber,
																	HUINT16 *pusSelectedIndex)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hFirstItem = NULL;
	HTREEITEM			hTempItem = NULL;
	HUINT16				usVisibleCount = 0;

	if(pTreeObject == NULL || hSelectedItem == NULL || pusTotalNumber == NULL || pusSelectedIndex == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetVisibleItemCount] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*pusTotalNumber = *pusSelectedIndex = 0;

	hError = UTIL_TREEUTIL_GetFirstChildItem(pTreeObject->hTree, &hFirstItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetVisibleItemCount] GetFirstChildItem fail!!\n"));
	}


	hTempItem = hFirstItem;
	while(hTempItem)
	{
		usVisibleCount++;
		if(hTempItem == hSelectedItem)
		{
			*pusSelectedIndex = usVisibleCount -1;
		}
		hResult = local_treeobject_FindNextItem(hTempItem, &hTempItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetVisibleItemCount] FindNextItem fail!!\n"));
		}
	}
	*pusTotalNumber = usVisibleCount;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_SetDisplayPage(PTREE_OBJECT pTreeObject,  HTREEITEM hSelectedItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hRootItem = NULL;
	HBOOL				bIsPage = FALSE;

	if(pTreeObject == NULL || hSelectedItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetDisplayPage] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetRootItem(pTreeObject->hTree, &hRootItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetDisplayPage] GetRootItem fail!!\n"));
	}

	if(hSelectedItem == hRootItem)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetDisplayPage] selected item is root!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = local_treeobject_IsPageItem(pTreeObject, hSelectedItem, &bIsPage);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_IsPageItem] IsPageItem fail!!\n"));
	}
	if(bIsPage == FALSE)
	{
		pTreeObject->hDispStart = hSelectedItem;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;}



static ONDK_Result_t		local_treeobject_IsPageItem(PTREE_OBJECT pTreeObject, HTREEITEM hItem, HBOOL *pbIsPage)
{
	HUINT32				ulCount = 0;
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HTREEITEM			hTempItem = NULL;

	if(pTreeObject == NULL || hItem == NULL || pbIsPage == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_IsPageItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*pbIsPage = FALSE;

	hTempItem = pTreeObject->hDispStart;
	while(hTempItem != NULL && ulCount < pTreeObject->usItemsPerPage)
	{
		if(hTempItem == hItem)
		{
			*pbIsPage = TRUE;
			break;
		}
		hResult = local_treeobject_FindNextItem(hTempItem, &hTempItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_IsPageItem] FindNextItem fail!!\n"));
		}
		ulCount++;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_treeobject_SetPageFocus(PTREE_OBJECT pTreeObject, HUINT32 ulFocusIndex, HBOOL bAllLayer)
{
	HTREEITEM			hTempItem = NULL;
	HUINT32				ulIndex = 0;
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;

	if(pTreeObject == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetPageFocus] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	//new select item setting
	hTempItem = pTreeObject->hDispStart;
	for(ulIndex = 0; ulIndex <= ulFocusIndex; ulIndex++)
	{
		if(hTempItem != NULL)
		{
			hResult = local_treeobject_SelectItem(pTreeObject, hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetPageFocus] SelectItem fail!!\n"));
			}
		}
		else
		{
			break;
		}

		if(bAllLayer == TRUE)
		{
			hResult = local_treeobject_FindNextItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetPageFocus] FindNextItem fail!!\n"));
			}
		}
		else
		{
			hResult = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_SetPageFocus] GetNextItem fail!!\n"));
			}
		}
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;

}


/* 마지막 아이템을 마지막 위치에 가도록 설정한다. (Page Item만큼 가득 차면서) */
static ONDK_Result_t		local_treeobject_SetLastItemOnLastIdx(PTREE_OBJECT pTreeObject, HBOOL bAllLayer, HUINT32 ulSelectIndex)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HTREEITEM			hLastItem = NULL;

	hResult= local_treeobject_GetDisplayLastItem(pTreeObject, &hLastItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] GetDisplayLastItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	hResult = local_treeobject_SetReverseDisplayStartItem(pTreeObject, hLastItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] SetReverseDisplayStartItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	hResult = local_treeobject_SetPageFocus(pTreeObject, ulSelectIndex, bAllLayer);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] SetPageFocus fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}


	EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_PrevPage(PTREE_OBJECT pTreeObject, HBOOL bAllLayer, HTREEITEM *phPrevPage)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hStartItem = NULL;
	HTREEITEM			hSelectedItem = NULL;
	HTREEITEM			hFirstItem = NULL;
	HTREEITEM			hTempItem = NULL;
	HUINT16				usItemsPerPage = 0;
	HUINT32				ulIndex = 0;
	HUINT32				ulSelectIndex = 0;
	HBOOL				bCursorMoveOnFirstItem = FALSE, bCursorMoveOnFirstPage = FALSE;


	/* 동작 원칙 : 첫 아이템에서 처리시는 맨 마지막 아이템으로 looping (맨 마지막 아이템에 커서 위치)
				   첫 페이지에서 처리시는 젤 처음 아이템으로 이동
				   그 외에 경우는 커서 위치는 유지 */

	if(pTreeObject == NULL || phPrevPage == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phPrevPage = NULL;


	hTempItem = hStartItem = pTreeObject->hDispStart;
	hSelectedItem = pTreeObject->hSelectedItem;
	usItemsPerPage = pTreeObject->usItemsPerPage;



	hError = UTIL_TREEUTIL_GetFirstChildItem(pTreeObject->hTree, &hFirstItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] GetFirstChildItem fail!!\n"));
	}

	if (hFirstItem == hSelectedItem)
	{
		bCursorMoveOnFirstItem = TRUE;
	}
	else if (pTreeObject->usSelectedIndex - usItemsPerPage < 0)
	{
		bCursorMoveOnFirstPage = TRUE;
	}

	/* 맨 처음 위치에서 UP시에는 맨 마지막 위치, 맨 마지막 아이템으로 이동한다. (loop) */
	if(TRUE == bCursorMoveOnFirstItem)
	{	// ~ hLastItem 까지 표시하도록 설정
		ulSelectIndex = usItemsPerPage - 1;

		local_treeobject_SetLastItemOnLastIdx(pTreeObject, bAllLayer, ulSelectIndex);
		hResult = GWM_RET_OK;
		*phPrevPage = pTreeObject->hDispStart;

		goto EXIT_TREEOBJ_FUNC;
	}

	/* 첫 페이지에서 이동 시에는 첫 번째 항목에 커서를 위치 시킨다. */
	else if (TRUE == bCursorMoveOnFirstPage)
	{
		ulSelectIndex = 0;
		pTreeObject->hDispStart = hFirstItem;

		hResult = local_treeobject_SetPageFocus(pTreeObject, ulSelectIndex, bAllLayer);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] SetPageFocus fail!!\n"));
		}
		hResult = GWM_RET_OK;

		*phPrevPage = pTreeObject->hDispStart;

		goto EXIT_TREEOBJ_FUNC;
	}


	/* 커서의 위치는 이동시키고 데이터를 교체한다 ^^ */
	// select index setting - page 내의 위치 확인
	for(ulIndex = 0; ulIndex < usItemsPerPage; ulIndex++)
	{
		if(hTempItem == hSelectedItem)
		{
			ulSelectIndex = ulIndex;
			break;
		}

		if(bAllLayer)
		{
			hResult = local_treeobject_FindNextItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] FindNextItem fail!!\n"));
			}
		}
		else
		{
			hResult = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] GetNextItem fail!!\n"));
			}
		}
	}

	// new start item setting
	hTempItem = hStartItem;
	for(ulIndex = 0; ulIndex < usItemsPerPage; ulIndex++)
	{
		if(bAllLayer)
		{
			hResult = local_treeobject_FindPrevItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] FindPrevItem fail!!\n"));
			}
		}
		else
		{
			hResult = UTIL_TREEUTIL_GetPrevItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] GetPrevItem fail!!\n"));
			}
		}

		if(hTempItem != NULL)
		{
			hStartItem = hTempItem;
		}
		else
		{
			break;
		}
	}
	pTreeObject->hDispStart = hStartItem;

	//new select item setting
	hResult = local_treeobject_SetPageFocus(pTreeObject, ulSelectIndex, bAllLayer);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PrevPage] SetPageFocus fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phPrevPage = pTreeObject->hDispStart;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;

}



static ONDK_Result_t		local_treeobject_NextPage(PTREE_OBJECT pTreeObject, HBOOL bAllLayer, HTREEITEM *phNextPage)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hStartItem = NULL;
	HTREEITEM			hSelectedItem = NULL;
	HTREEITEM			hFirstItem = NULL;
	HTREEITEM			hTempItem = NULL;
	HTREEITEM			hLastItem = NULL;
	HUINT16				usItemsPerPage = 0;
	HUINT32				ulIndex = 0, ulLastPageStartIdx = 0;
	HUINT32				ulSelectIndex = 0;
	HBOOL				bCursorMoveOnLastItem = FALSE,  bCursorMoveOnLastPage = FALSE, bNeedDispStartItemChange = FALSE;

	/* 동작 원칙 : 마지막 아이템에서 처리시는 젤 처음 아이템으로 looping (첫 아이템에 커서 위치)
				   마지막 페이지에서 처리시는 젤 마지막 아이템으로 이동
				   그 외에 경우는 커서 위치는 유지 */

	if(pTreeObject == NULL || phNextPage == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_NextPage] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phNextPage = NULL;


	hTempItem = hStartItem = pTreeObject->hDispStart;
	hSelectedItem = pTreeObject->hSelectedItem;
	usItemsPerPage = pTreeObject->usItemsPerPage;


	hResult = local_treeobject_GetDisplayLastItem(pTreeObject, &hLastItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_NextPage] GetDisplayLastItem fail!!\n"));
	}


	// select index setting - page 내 커서 위치 조사
	for(ulIndex = 0; ulIndex < usItemsPerPage; ulIndex++)
	{
		if(hTempItem == hSelectedItem)
		{
			ulSelectIndex = ulIndex;
			break;
		}

		if(bAllLayer)
		{
			hResult = local_treeobject_FindNextItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_NextPage] FindNextItem fail!!\n"));
			}
		}
		else
		{
			hResult = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_NextPage] GetNextItem fail!!\n"));
			}
		}
	}

	ulLastPageStartIdx = (pTreeObject->usNumOfVisible / usItemsPerPage) * usItemsPerPage;
	if(		(hLastItem == pTreeObject->hDispEnd && ulSelectIndex == usItemsPerPage - 1)
			|| (hLastItem == pTreeObject->hDispEnd && ulSelectIndex == pTreeObject->usNumOfVisible - 1)
	  )
	{
		bCursorMoveOnLastItem = TRUE;
	}
	else if (hLastItem == pTreeObject->hDispEnd || ulLastPageStartIdx <= pTreeObject->usSelectedIndex)
	{
		bCursorMoveOnLastPage = TRUE;
	}
	else if (pTreeObject->usNumOfVisible - usItemsPerPage <= pTreeObject->usSelectedIndex + usItemsPerPage)
	{
		bNeedDispStartItemChange = TRUE;
	}

	if(TRUE == bCursorMoveOnLastItem)
	{	/* First Item 부터 표시 (loop) */
		ulSelectIndex = 0;

		hError = UTIL_TREEUTIL_GetFirstChildItem(pTreeObject->hTree, &hFirstItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_NextPage] GetFirstChildItem fail!!\n"));
		}

		pTreeObject->hDispStart = hFirstItem;
		hResult = local_treeobject_SetPageFocus(pTreeObject, ulSelectIndex, bAllLayer);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_NextPage] SetPageFocus fail!!\n"));
			goto EXIT_TREEOBJ_FUNC;
		}
		*phNextPage = pTreeObject->hDispStart;
		goto EXIT_TREEOBJ_FUNC;
	}

	/* 마지막 페이지에서 이동 시에는 마지막 항목에 커서를 위치 시킨다. */
	else if (TRUE == bCursorMoveOnLastPage)
	{
		ulSelectIndex = usItemsPerPage - 1;
		local_treeobject_SetLastItemOnLastIdx(pTreeObject, bAllLayer, ulSelectIndex);

		hResult = GWM_RET_OK;
		*phNextPage = pTreeObject->hDispEnd;

		goto EXIT_TREEOBJ_FUNC;
	}

	/* 마지막 아이템을 끝으로 하여 표시해야 하는 경우 (첫 표시 아이템 변경) */
	else if (TRUE == bNeedDispStartItemChange)
	{
		local_treeobject_SetLastItemOnLastIdx(pTreeObject, bAllLayer, ulSelectIndex);

		hResult = GWM_RET_OK;
		*phNextPage = pTreeObject->hSelectedItem;

		goto EXIT_TREEOBJ_FUNC;
	}


	// new start item setting
	hTempItem = hStartItem;
	for(ulIndex = 0; ulIndex < usItemsPerPage; ulIndex++)
	{
		if(bAllLayer)
		{
			hResult = local_treeobject_FindNextItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_NextPage] FindNextItem fail!!\n"));
			}
		}
		else
		{
			hResult = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_NextPage] GetNextItem fail!!\n"));
			}
		}

		if(hTempItem != NULL)
		{
			hStartItem = hTempItem;
		}
		else
		{
			break;
		}
	}

	pTreeObject->hDispStart = hStartItem;

	//new select item setting
	hResult = local_treeobject_SetPageFocus(pTreeObject, ulSelectIndex, bAllLayer);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_NextPage] SetPageFocus fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phNextPage = pTreeObject->hDispStart;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}




static ONDK_Result_t		local_treeobject_GetPrevPageItem(	PTREE_OBJECT pTreeObject,
																HTREEITEM hItem,
																HBOOL bAllLayer,
																HTREEITEM *phPrevItem)
{
	HTREEITEM			hSelected = NULL;
	HTREEITEM			hTempItem = NULL;
	HUINT16				usItemsPerPage = 0;
	HUINT16				usIndex = 0;
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HBOOL				bIsPage = FALSE;


	if(pTreeObject == NULL || phPrevItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetPrevPageItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*phPrevItem = hItem;


	hTempItem = hItem;
	usItemsPerPage = pTreeObject->usItemsPerPage;

	for(usIndex = 0; usIndex < usItemsPerPage-1; usIndex++)
	{
		if(bAllLayer == FALSE)
		{
			hError = UTIL_TREEUTIL_GetPrevItem(hTempItem, &hTempItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetPrevPageItem] GetPrevItem fail!!\n"));
			}
		}
		else
		{
			hResult = local_treeobject_FindPrevItem(hTempItem, &hTempItem);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetPrevPageItem] FindPrevItem fail!!\n"));
			}
		}

		hResult = local_treeobject_IsPageItem(pTreeObject, hTempItem, &bIsPage);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetPrevPageItem] IsPageItem fail!!\n"));
		}

		if(hTempItem != NULL && bIsPage == FALSE)
		{
			hSelected = hTempItem;
		}
	}

	*phPrevItem = hSelected;

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}

static ONDK_Result_t		local_treeobject_GetItemData(HTREEITEM hItem, void **ppData)
{
	ONDK_Result_t		hResult	 = GWM_RET_INITIAL_FAILED;
	HERROR			hError	 = ERR_FAIL;
	void			*pLocalData = NULL;

	if(hItem == NULL || ppData == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetItemData] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*ppData = NULL;

	hError = UTIL_TREEUTIL_GetItemData(hItem, &pLocalData);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetItemData] GetItemData fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	*ppData = pLocalData;
	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_treeobject_FindItem_ByData(HTREEMGR hTree, HTREEITEM *phFindItem, void *pTargetData,
											 FP_GetNextItemFunc naviNextItemFunc, FP_CompFunc compFunc)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEITEM		hItem = NULL;
	void			*pData;

	hResult = local_treeobject_GetFirstChildItem(hTree, &hItem);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_TREE_FindItem_ByData] GetTreeMgr fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	/* navigate items and compare by compFunc */
	*phFindItem = NULL;
	while (hItem)
	{
		HTREEITEM hNextItem = NULL;
		HERROR	  hErr		= ERR_FAIL;

		local_treeobject_GetItemData(hItem, (void**)&pData);
		hErr = compFunc(pData, pTargetData);
		if (ERR_OK == hErr)
		{	// match !
			*phFindItem = hItem;
			return GWM_RET_OK;
		}

		naviNextItemFunc(hItem, &hNextItem);
		hItem = hNextItem;
	}

EXIT_TREEOBJ_FUNC:
	return hResult;
}


static ONDK_Result_t		local_treeobject_GetCondMatchItemCount_ByData(HTREEMGR hTree, HUINT32 *pulCount, void *pTargetData,
																	  FP_GetNextItemFunc naviNextItemFunc, FP_CompFunc compFunc)
{
	ONDK_Result_t	hResult = GWM_RET_INITIAL_FAILED;
	HTREEITEM		hItem = NULL;
	void			*pData;

	hResult = local_treeobject_GetFirstChildItem(hTree, &hItem);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_TREE_FindItem_ByData] GetTreeMgr fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	/* navigate items and compare by compFunc */
	*pulCount = 0;
	while (hItem)
	{
		HTREEITEM hNextItem = NULL;
		HERROR	  hErr		= ERR_FAIL;

		local_treeobject_GetItemData(hItem, (void**)&pData);
		hErr = compFunc(pData, pTargetData);
		if (ERR_OK == hErr)
		{	// match !
			(*pulCount)++;
		}

		naviNextItemFunc(hItem, &hNextItem);
		hItem = hNextItem;
	}

EXIT_TREEOBJ_FUNC:
	return hResult;
}




static ONDK_Result_t		local_treeobject_GetFirstChildItem(HTREEMGR hTree, HTREEITEM *phFirstChild)
{
	ONDK_Result_t		hResult	 = GWM_RET_INITIAL_FAILED;
	HERROR				hError	 = ERR_FAIL;

	if(hTree == NULL || phFirstChild == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetFirstChildItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	*phFirstChild = NULL;
	hError = UTIL_TREEUTIL_GetFirstChildItem(hTree, phFirstChild);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetFirstChildItem] GetFirstItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_GetRootItem(HTREEMGR hTree, HTREEITEM *phRootItem)
{
	ONDK_Result_t		hResult	 = GWM_RET_INITIAL_FAILED;
	HERROR				hError	 = ERR_FAIL;

	if(hTree == NULL || phRootItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetRootItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetRootItem(hTree, phRootItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetRootItem] GetRootItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_treeobject_GetChildNum(HTREEITEM hItem, HUINT32 *pulNumOfChild)
{
	ONDK_Result_t		hResult	 = GWM_RET_INITIAL_FAILED;
	HERROR				hError	 = ERR_FAIL;

	if(hItem == NULL || pulNumOfChild == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetChildNum] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetChildNum(hItem, pulNumOfChild);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetChildNum] GetChildNum fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}

static ONDK_Result_t		local_treeobject_GetChildItem(HTREEITEM hItem, HTREEITEM *phChildItem)
{
	ONDK_Result_t		hResult	 = GWM_RET_INITIAL_FAILED;
	HERROR				hError	 = ERR_FAIL;

	if(hItem == NULL || phChildItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetChildItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetChildItem(hItem, phChildItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetChildItem] GetChilItem fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_treeobject_GetIndexOfItem(HTREEMGR hTree, HTREEITEM hTargetItem, FP_GetNextItemFunc naviNextItemFunc, HUINT32 *pulIndex)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	HTREEITEM			hItem;

	if(hTree == NULL || hTargetItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetIndexOfItem] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;

	}

	hResult = local_treeobject_GetFirstChildItem(hTree, &hItem);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetIndexOfItem] Get First Child fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	/* navigate items and compare by compFunc */
	*pulIndex	= 0;
	while (hItem)
	{
		HTREEITEM hNextItem = NULL;

		if (hTargetItem == hItem)
		{
			return GWM_RET_OK;
		}

		naviNextItemFunc(hItem, &hNextItem);
		(*pulIndex)++;
		hItem = hNextItem;
	}

	/* index 없다 => Error */
	hResult = GWM_RET_INITIAL_FAILED;

EXIT_TREEOBJ_FUNC:
	return hResult;
}


static ONDK_Result_t		local_treeobject_GetDisplayItemRect(PTREE_OBJECT pTreeObject, HUINT16 usIndex, ONDK_Rect_t *pItemRect)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HUINT32 				ulX = 0;
	HUINT32 				ulY = 0;
	HUINT32				ulWidth = 0;
	HUINT32				ulHeight = 0;

	if(pTreeObject == NULL || pItemRect == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetDisplayItemRect] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	if(usIndex >= pTreeObject->usItemsPerPage)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetDisplayItemRect] param index fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	ulX = pTreeObject->rect.x + pTreeObject->stFrameMargin.w;
	ulY = pTreeObject->rect.y + pTreeObject->stFrameMargin.h + (pTreeObject->stItemRect.h * usIndex);
	ulWidth = pTreeObject->stItemRect.w;
	ulHeight = pTreeObject->stItemRect.h;

	hResult = ONDK_Rect(pItemRect, ulX, ulY, ulWidth, ulHeight);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetDisplayItemRect] ONDK_Rect fail!!\n"));
	}

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



#define TEMP_DRAW		1
static ONDK_Result_t		local_treeobject_UpdatePageItem(PTREE_OBJECT pTreeObject, HINT32 arg)
{
	HTREEITEM 				hStart = NULL;
	HTREEITEM 				hTemp = NULL;
	HTREEITEM 				hChildItem = NULL;
	HUINT16					usIndex = 0;
	ONDK_Rect_t				stItemRect;
	ONDK_Result_t				hResult = GWM_RET_INITIAL_FAILED;
	HERROR					hError = ERR_FAIL;
	TREEOBJ_ITEMINFO		stItemInfo;
	HUINT16 					usTotalNumber = 0;
#if defined(TEMP_DRAW)
	TREEOBJ_ITEM_DRAWINFO	stItemDrawInfo;
	ONDK_Region_t 				stAppArea;

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &stAppArea);
#endif


	if(pTreeObject == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] param fail!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}
	hStart = pTreeObject->hDispStart;
	if(hStart == NULL)
	{
		hError = UTIL_TREEUTIL_GetFirstChildItem(pTreeObject->hTree, &hChildItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] GetFirstChildItem fail!!\n"));
		}
		hStart = hChildItem;
	}

	hTemp = hStart;
	if(hTemp == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] tree have no child!!\n"));
		goto EXIT_TREEOBJ_FUNC;
	}

	do
	{
		// get item draw rect
		VK_memset(&stItemRect, 0x00, sizeof(ONDK_Rect_t));
		hResult = local_treeobject_GetDisplayItemRect(pTreeObject, usIndex, &stItemRect);

		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] GetDisplayItemRect fail!!\n"));
		}
		else
		{
			// step 1. display item pointer setting
			if(usIndex == 0)
			{
				pTreeObject->hDispStart = hTemp;
			}
			pTreeObject->hDispEnd = hTemp;

			hError = UTIL_TREEUTIL_GetChildItem(hTemp, &hChildItem);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] GetChildItem fail!!\n"));
			}

			// step 2. item status setting
			VK_memset(&stItemInfo, 0x00, sizeof(TREEOBJ_ITEMINFO));
			hError = UTIL_TREEUTIL_GetItemUserInfo(hTemp, sizeof(TREEOBJ_ITEMINFO), (void*)&stItemInfo);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] GetItemUserInfo fail!!\n"));
				goto EXIT_TREEOBJ_FUNC;
			}
			if(hChildItem != NULL)
			{
				stItemInfo.ulItemStatus |= TREEITEM_ENABLE;
			}
			else
			{
				stItemInfo.ulItemStatus  &= ~TREEITEM_ENABLE;
				stItemInfo.ulItemStatus  |= TREEITEM_DISABLE;
			}
			hError = UTIL_TREEUTIL_SetItemUserInfo(hTemp, sizeof(TREEOBJ_ITEMINFO), (void*)(&stItemInfo));
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] SetItemUserInfo fail!!\n"));
				goto EXIT_TREEOBJ_FUNC;
			}

			// step 3. item draw info setting
			VK_memset(&stItemDrawInfo, 0x00, sizeof(TREEOBJ_ITEM_DRAWINFO));

			hResult = local_treeobject_GetVisibleItemCount(pTreeObject, hTemp, &usTotalNumber, (HUINT16*)&stItemDrawInfo.ulIndexAtTotal);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] GetVisibleItemCount fail!!\n"));
			}
			hError = UTIL_TREEUTIL_GetItemDepth(hTemp, &stItemDrawInfo.ulDepth);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] GetItemDepth fail!!\n"));
			}
			hError = UTIL_TREEUTIL_GetChildNum(hTemp, &stItemDrawInfo.ulNumOfChild);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] GetChildNum fail!!\n"));
			}

			hError = local_treeobject_GetItemData(hTemp, (void**)&stItemDrawInfo.pLocalData);
			if(hError != ERR_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_GetItemData] Get local data fail!!\n"));
			}

			stItemDrawInfo.ulStatus      = stItemInfo.ulItemStatus;
			stItemDrawInfo.ulIndexAtPage = usIndex;

			// step 4. draw callback
			if(pTreeObject->objDrawItemMethod != NULL)
			{
				pTreeObject->objDrawItemMethod(	ONDK_GWM_GetSurface(),
												&stAppArea,
												&stItemRect,
												(ONDK_GWM_GuiObject_t)pTreeObject,
												(void*)(&stItemDrawInfo),
												arg);
			}
		}

		hResult = local_treeobject_FindNextItem(hTemp, &hTemp);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_PageItemUpdate] FindNextItem fail!!\n"));
		}

		usIndex++;
		if(usIndex >= pTreeObject->usItemsPerPage)
		{
			break;
		}
	}while(hTemp != NULL);

	hResult = GWM_RET_OK;

EXIT_TREEOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_treeobject_SetObjDisable(PTREE_OBJECT pTreeObject, HBOOL	bDisable)
{
	pTreeObject->bObjDisabled	= bDisable;
	return GWM_RET_OK;
}

static ONDK_Result_t		local_treeobject_SetOnlyOneItemExpandFlag(PTREE_OBJECT pTreeObject, HBOOL	bOnlyOneItemExpand)
{
	pTreeObject->bOnlyOneItemExpand	= bOnlyOneItemExpand;
	return GWM_RET_OK;
}


static ONDK_Result_t		local_treeobject_defaultProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	ONDK_Region_t			area;
	ONDK_Rect_t			rect;
	HTREEITEM			hSelectedItem = NULL;
	PTREE_OBJECT		pTreeObject = NULL;
	HUINT32				ulItemStatus = 0;

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);

	INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

	pTreeObject = (PTREE_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
			local_treeobject_FreeObject_ByPObj(pTreeObject);
			return GWM_MESSAGE_BREAK;
		case MT_DRAW:
			if (pTreeObject->bObjDisabled)
			{
				return GWM_MESSAGE_PASS;
			}

			if (pTreeObject->objDrawItemMethod != NULL)
			{
				/*hResult = object->objDrawMethod(ONDK_GWM_GetSurface(), &area, &rect, object, (void*)NULL, arg);
				if(hResult != GWM_RET_OK)
				{
					GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] objDrawMethod fail!!\n"));
				}*/
				hResult = local_treeobject_UpdatePageItem(pTreeObject, arg);
				if(hResult != GWM_RET_OK)
				{
					GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] UpdatePageItem fail!!\n"));
				}
			}
			else
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] objDrawMethod NULL!!\n"));
			}
			return GWM_MESSAGE_BREAK;

	case MT_KEYDOWN:
		if (pTreeObject->bObjDisabled)
		{
			return GWM_MESSAGE_PASS;
		}

		switch (arg)
		{
			case KEY_ARROWUP:
				hResult = local_treeobject_SelectPrevItem(pTreeObject, &hSelectedItem);
				if(hResult != GWM_RET_OK)
				{
					GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] SelectPrevItem fail!!\n"));
				}
				if(hSelectedItem == NULL || hResult != GWM_RET_OK)
				{	// rounding case
					HTREEITEM	hPrevItem;
					hResult = local_treeobject_GetDisplayLastItem(pTreeObject, &hSelectedItem);
					if(hResult != GWM_RET_OK)
					{
						GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] GetDisplayLastItem fail!!\n"));
					}
					hResult = local_treeobject_SelectItem(pTreeObject, hSelectedItem);
					if(hResult != GWM_RET_OK)
					{
						GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] SelectItem fail!!\n"));
					}
					hResult = local_treeobject_GetPrevPageItem(pTreeObject, hSelectedItem, TRUE, &hPrevItem);
					if(hResult == GWM_RET_OK)
					{
						hResult = local_treeobject_SetDisplayStartItem(pTreeObject, hPrevItem);
					}
				}
				ONDK_GWM_APP_InvalidateObject(pTreeObject->ID);
				return GWM_MESSAGE_BREAK;

			case KEY_ARROWDOWN:
				hResult = local_treeobject_SelectNextItem(pTreeObject, &hSelectedItem);
				if(hResult != GWM_RET_OK)
				{
					GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] SelectPrevItem fail!!\n"));
				}
				if(hSelectedItem == NULL || hResult != GWM_RET_OK)
				{	// rounding case
					hError = UTIL_TREEUTIL_GetFirstChildItem(pTreeObject->hTree, &hSelectedItem);
					if(hError != ERR_OK)
					{
						GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] GetFirstChildItem fail!!\n"));
					}
					hResult = local_treeobject_SelectItem(pTreeObject, hSelectedItem);
					if(hResult != GWM_RET_OK)
					{
						GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] SelectItem fail!!\n"));
					}
				}
				ONDK_GWM_APP_InvalidateObject(pTreeObject->ID);
				return GWM_MESSAGE_BREAK;


			case KEY_PAGEUP:
				hResult = local_treeobject_PrevPage(pTreeObject, TRUE, &hSelectedItem);
				if(hResult != GWM_RET_OK)
				{
					GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] SelectPrevItem fail!!\n"));
				}
				ONDK_GWM_APP_InvalidateObject(pTreeObject->ID);
				return GWM_MESSAGE_BREAK;


			case KEY_PAGEDOWN:
				hResult = local_treeobject_NextPage(pTreeObject, TRUE, &hSelectedItem);
				if(hResult != GWM_RET_OK)
				{
					GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] SelectPrevItem fail!!\n"));
				}
				ONDK_GWM_APP_InvalidateObject(pTreeObject->ID);
				return GWM_MESSAGE_BREAK;

			case KEY_ARROWLEFT:
				return GWM_MESSAGE_BREAK;

			case KEY_ARROWRIGHT:
				return GWM_MESSAGE_BREAK;

			case KEY_OK:
				hResult = local_treeobject_ExpandItem(pTreeObject, FALSE, pTreeObject->bOnlyOneItemExpand, &ulItemStatus);
				if(hResult != GWM_RET_OK)
				{
					GWM_DBG_Print(DBG_PRINT, ("[local_treeobject_defaultProc] ExpandItem fail!!\n"));
				}
				ONDK_GWM_APP_InvalidateObject(pTreeObject->ID);
				return GWM_MESSAGE_BREAK;
			default:
				break;
		}
		break;

		case MT_RELOCATE:
			return GWM_MESSAGE_BREAK;

		case MT_CHANGE_TEXT:
			return GWM_MESSAGE_BREAK;

		case MT_SET_VALUE:
			return GWM_MESSAGE_BREAK;
		default:
			break;
	}

	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}
#endif



/* End of File. ---------------------------------------------------------- */

