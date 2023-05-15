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


//#define EPG_GRID_OBJ_CHECK(x) HxLOG_Print("[%s] %d\n", x, VK_TIMER_GetSystemTick())
#define EPG_GRID_OBJ_CHECK(x)

/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <string.h>
#include <octo_common.h>

#include <util.h>
#include <treeutil.h>

#include <osd_gfx.h>

#include <gwm_objdef.h>
#include <gwm_appobj.h>

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define	MAX_GRID_OBJECT						2
#define	MAX_GRID_DEPTH						2


#define EPGGRID_EVENTNAME_POPUP_H	50





/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */
typedef struct		tagGridObjectItem
{
	HUINT32				ulItemStatus;
}GRIDOBJ_ITEMINFO;



#define CODE_COMPLETE
typedef struct	tagGridObject
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
	CellAddDirection_t			eCellAddDirection;

	HTREEMGR					hTree;					// treeitem structure

	HTREEITEM					hDispStart;				//display start item pointer
	HTREEITEM					hDispEnd;				//display end item pointer
	HTREEITEM					hSelectedItem;			//selected item (always event layer item)

	HUINT16						usSelectedIndex;		// index of selected item
	HUINT16 						usItemsPerPage;		// display items(items per a page)

	ONDK_Rect_t					stFrameMargin;			// grid frame margin

	ONDK_Rect_t					stItemRect;				// grid item rect prototype(total row)
	ONDK_Rect_t					stItemMargin;
	ONDK_Rect_t					stItemRect1;
	ONDK_Rect_t					stItemMargin1;
	ONDK_Rect_t					stItemRect2;
	ONDK_Rect_t					stItemMargin2;

	GWM_User_t					objGetDrawItemRect;
	GWM_User_t					objProcUpDown;
	GWM_User_t					objProcLeftRight;
	GWM_User_t					objProcOK;

	GWM_ObjDraw_t				objDrawItemMethod;

}GRID_OBJECT;


typedef struct	tagGridObjectItemDrawInfo
{
	HUINT32 					ulIndexAtPage;		// item index at page
	HUINT32 					ulStatus;			// selected? ... 기타등등
	HUINT32					ulDepth;
	void						*pLocalData;

}GRIDOBJ_ITEM_DRAWINFO;




typedef struct	tagGridObjectManager
{
	HUINT8			ucIsAlloced[MAX_GRID_OBJECT];
	GRID_OBJECT		astGridObject[MAX_GRID_OBJECT];
}GRID_MANAGER;

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static GRID_MANAGER		s_stGridManager = {{0, },{{0, }, }};



/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */
static void				local_gridobject_InitClass(void);
static ONDK_Result_t		local_gridobject_Create(HINT32 ulId, OBJ_CREATE_t *pstObjCreateParam);
static ONDK_Result_t		local_gridobject_GetObject(HINT32 ulId, GRID_OBJECT **ppGridObj);
static ONDK_Result_t		local_gridobject_AllocNewObject(GRID_OBJECT **ppGridObj);
static ONDK_Result_t		local_gridobject_defaultProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg);
static ONDK_Result_t		local_gridobject_MoveKeyProc(PGRID_OBJECT pGridObject, HINT32 lParam);
static ONDK_Result_t		local_gridobject_CleanObject(HINT32 ulId);
static ONDK_Result_t		local_gridobject_FreeObjectById(HINT32 ulId);
static ONDK_Result_t		local_gridobject_FreeObject(PGRID_OBJECT pGridObj);
static ONDK_Result_t		local_gridobject_InsertItem(HINT32 lId, HTREEITEM hParent, HTREEITEM hPrevLink, HTREEITEM hNextLink, TREEITEM_PARAM_t *pParam, HTREEITEM *phNewItem);
static ONDK_Result_t 		local_gridobject_DeleteItem(HINT32 ulId, HTREEITEM hDelItem);
static ONDK_Result_t 		local_gridobject_DeleteEventItems(HINT32 ulId, HTREEITEM hSvcItem);
static ONDK_Result_t		local_gridobject_GetNextItem(HTREEITEM hItem, HTREEITEM *phNextItem);
static ONDK_Result_t		local_gridobject_GetPrevItem(HTREEITEM hItem, HTREEITEM *phPrevItem);
static ONDK_Result_t		local_gridobject_GetEventItem(HTREEITEM hSvcItem, HTREEITEM *phEventItem);
static ONDK_Result_t		local_gridobject_GetItemData(HTREEITEM hItem, void **ppData);
static ONDK_Result_t		local_gridobject_GetServiceItemByIndex(HUINT32 ulId, HUINT32 ulSvcIndex, HTREEITEM *phSvcItem);
// not used
//static GWM_Result_t		local_gridobject_GetLayerEndItem(HTREEITEM hItem, HTREEITEM *phLayerEndItem);
//static GWM_Result_t		local_gridobject_GetLayerStartItem(HTREEITEM hItem, HTREEITEM *phLayerStartItem);
static ONDK_Result_t		local_gridobject_GetSelectedItem(PGRID_OBJECT pGridObject, HTREEITEM *phSelectedItem);
static ONDK_Result_t		local_gridobject_SelectItem(PGRID_OBJECT pGridObject, HTREEITEM hItem);
static ONDK_Result_t		local_gridobject_UpdateFocus(HUINT32 ulId);
static ONDK_Result_t		local_gridobject_GetItemRect(PGRID_OBJECT pGridObject, HTREEITEM hItem, ONDK_Rect_t *pItemRect);
static ONDK_Result_t		local_gridobject_GetDrawItemRect(PGRID_OBJECT pGridObject, HTREEITEM hItem, HUINT16 usIndex, HUINT32 ulDepth, ONDK_Rect_t *pItemRect);
static ONDK_Result_t		local_gridobject_GetItemParamInfo(HUINT32 ulParam, HBOOL *pbSelected, HBOOL *pbFront, HUINT8 *pucDepth, HUINT8 *pucIndex);
static ONDK_Result_t		local_gridobject_UpdatePageItems(PGRID_OBJECT pGridObject, HBOOL bFront);
STATIC ONDK_Result_t 	local_gridobject_SetAddDirection(HINT32 lId, CellAddDirection_t eCellAddDi);
STATIC ONDK_Result_t 	local_gridobject_GetAddDirection(HINT32 lId, CellAddDirection_t *peCellAddDi);




#define ________GLOBAL_FUNC________

void		gwm_grid_InitClass(void)
{
	local_gridobject_InitClass();

}

ONDK_Result_t		GWM_GRID_Create(HINT32 ulId, OBJ_CREATE_t *pstObjCreateParam)
{
	return local_gridobject_Create(ulId, pstObjCreateParam);
}

ONDK_Result_t		GWM_GRID_FreeObjectById(HINT32 ulId)
{
	return local_gridobject_FreeObjectById(ulId);
}


ONDK_Result_t		GWM_GRID_CleanObject(HINT32 ulId)
{
	return local_gridobject_CleanObject(ulId);
}


ONDK_Result_t		GWM_GRID_GetNextItem(Handle_t hItem, Handle_t *phNextItem)
{
	return local_gridobject_GetNextItem((HTREEITEM)hItem, (HTREEITEM*)phNextItem);

}


ONDK_Result_t		GWM_GRID_GetPrevItem(Handle_t hItem, Handle_t *phPrevItem)
{
	return local_gridobject_GetPrevItem((HTREEITEM)hItem, (HTREEITEM*)phPrevItem);
}


ONDK_Result_t		GWM_GRID_GetEventItem(Handle_t hSvcItem, Handle_t *phEventItem)
{
	return local_gridobject_GetEventItem((HTREEITEM)hSvcItem, (HTREEITEM*)phEventItem);
}


ONDK_Result_t		GWM_GRID_GetItemData(Handle_t hItem, void **ppData)
{
	return local_gridobject_GetItemData((HTREEITEM)hItem, ppData);
}


ONDK_Result_t		GWM_GRID_GetServiceItemByIndex(HUINT32 ulId, HUINT32 ulSvcIndex, Handle_t *phSvcItem)
{
	return local_gridobject_GetServiceItemByIndex(ulId, ulSvcIndex, (HTREEITEM*)phSvcItem);
}


ONDK_Result_t		GWM_GRID_UpdateFocus(HUINT32 ulId)
{
	return local_gridobject_UpdateFocus(ulId);
}

ONDK_Result_t		GWM_GRID_SelectItem(HINT32 ulId, Handle_t hItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	PGRID_OBJECT		pGridObj = NULL;


	pGridObj = (PGRID_OBJECT)ONDK_GWM_APP_GetObject(ulId);
	if(pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_GRID_SelectItem] GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hResult = local_gridobject_SelectItem(pGridObj, (HTREEITEM)hItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_GRID_SelectItem] local_gridobject_SelectItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}


ONDK_Result_t		GWM_GRID_GetSelectedItem(HINT32 ulId, Handle_t *phSelectedItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	PGRID_OBJECT		pGridObj = NULL;
	HTREEITEM			hTempItem = NULL;

	if(phSelectedItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_GRID_GetSelectedItem] local_gridobject_SelectItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	pGridObj = (PGRID_OBJECT)ONDK_GWM_APP_GetObject(ulId);
	if(pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_GRID_GetSelectedItem] GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	hResult = local_gridobject_GetSelectedItem(pGridObj, &hTempItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[GWM_GRID_GetSelectedItem] local_gridobject_GetSelectedItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*phSelectedItem = (Handle_t)hTempItem;

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;

}


ONDK_Result_t		GWM_GRID_GetItemParamInfo(HUINT32 ulParam, HBOOL *pbSelected, HBOOL *pbFront, HUINT8 *pucDepth, HUINT8 *pucIndex)
{
	return local_gridobject_GetItemParamInfo(ulParam, pbSelected, pbFront, pucDepth, pucIndex);
}


ONDK_Result_t		GWM_GRID_InsertItem(HINT32 lId, Handle_t hParent, Handle_t hPrevLink, Handle_t hNextLink, void *pParam, Handle_t *phNewItem)
{
	return local_gridobject_InsertItem(lId, (HTREEITEM)hParent, (HTREEITEM)hPrevLink, (HTREEITEM)hNextLink, pParam, (HTREEITEM*)phNewItem);
}

ONDK_Result_t		GWM_GRID_DeleteItem(HINT32 lId, Handle_t hDelItem)
{
	return local_gridobject_DeleteItem(lId, (HTREEITEM)hDelItem);
}

ONDK_Result_t		GWM_GRID_DeleteEventItems(HINT32 ulId, Handle_t hSvclItem)
{
	return local_gridobject_DeleteEventItems(ulId, (HTREEITEM)hSvclItem);
}

ONDK_Result_t			GWM_GRID_SetAddDirection(HINT32 lId, CellAddDirection_t eCellAddDi)
{
	return local_gridobject_SetAddDirection(lId, eCellAddDi);
}

ONDK_Result_t			GWM_GRID_GetAddDirection(HINT32 lId, CellAddDirection_t *peCellAddDi)
{
	return local_gridobject_GetAddDirection(lId, peCellAddDi);
}


#define ________LOCAL_FUNC________
static void		local_gridobject_InitClass(void)
{
	HINT32		lIndex = 0;

	for (lIndex=0; lIndex < MAX_GRID_OBJECT; lIndex++)
	{
		s_stGridManager.astGridObject[lIndex].link = NULL;
		VK_memset( &(s_stGridManager.astGridObject[lIndex].rect), 0x00, sizeof(ONDK_Rect_t));
		s_stGridManager.astGridObject[lIndex].ID = -1;
		s_stGridManager.astGridObject[lIndex].type = GUIOBJ_GRID;
		s_stGridManager.astGridObject[lIndex].attr = 0;
		s_stGridManager.astGridObject[lIndex].focus = GUIOBJ_DISABLED;
		s_stGridManager.astGridObject[lIndex].Proc = local_gridobject_defaultProc;
		s_stGridManager.astGridObject[lIndex].objDrawMethod = NULL;

		s_stGridManager.ucIsAlloced[lIndex] = 0;
	}
}




static ONDK_Result_t		local_gridobject_Create(HINT32 ulId, OBJ_CREATE_t *pstObjCreateParam)
{
	PGRID_OBJECT		pNewGridObject = NULL;
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;


/**************************************************************************************
 *
 * object alloc
 *
 **************************************************************************************/
	hResult = local_gridobject_GetObject(ulId, &pNewGridObject);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_Create] Param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hResult = local_gridobject_AllocNewObject(&pNewGridObject);
	if (hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_Create] AllocNewObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

/**************************************************************************************
 *
 * object basic info setting
 *
 **************************************************************************************/
	pNewGridObject->link = NULL;
	pNewGridObject->ID = ulId;
	pNewGridObject->type = GUIOBJ_GRID;
	pNewGridObject->focus = GUIOBJ_NORMAL;

/**************************************************************************************
 *
 * main frame rect setting
 *
 **************************************************************************************/
	VK_memcpy(&(pNewGridObject->rect), &pstObjCreateParam->stFrameRect, sizeof(ONDK_Rect_t));
	VK_memcpy(&(pNewGridObject->stFrameMargin), &pstObjCreateParam->stMarginRect, sizeof(ONDK_Rect_t));

/**************************************************************************************
 *
 * item rect setting
 *
 **************************************************************************************/
	VK_memcpy(&(pNewGridObject->stItemRect), &pstObjCreateParam->stItemRect, sizeof(ONDK_Rect_t));
	VK_memcpy(&(pNewGridObject->stItemRect1), &pstObjCreateParam->stItemRect1, sizeof(ONDK_Rect_t));
	VK_memcpy(&(pNewGridObject->stItemRect2), &pstObjCreateParam->stItemRect2, sizeof(ONDK_Rect_t));
	VK_memcpy(&(pNewGridObject->stItemMargin1), &pstObjCreateParam->stItemMargin1, sizeof(ONDK_Rect_t));
	VK_memcpy(&(pNewGridObject->stItemMargin2), &pstObjCreateParam->stItemMargin2, sizeof(ONDK_Rect_t));

/**************************************************************************************
 *
 * object callback setting
 *
 **************************************************************************************/
 	pNewGridObject->Proc = (GWM_Proc_t)pstObjCreateParam->pfnProc;
	if(pstObjCreateParam->pfnProc == NULL)
	{
		pNewGridObject->Proc = local_gridobject_defaultProc;
	}
	pNewGridObject->objDrawItemMethod = (GWM_ObjDraw_t)pstObjCreateParam->pfnObjDrawMethod;
	pNewGridObject->objGetDrawItemRect = (GWM_User_t)pstObjCreateParam->pfnObjUserMethod1;
	pNewGridObject->objProcUpDown = (GWM_User_t)pstObjCreateParam->pfnObjUserMethod2;
	pNewGridObject->objProcLeftRight = (GWM_User_t)pstObjCreateParam->pfnObjUserMethod3;
	pNewGridObject->objProcOK = (GWM_User_t)pstObjCreateParam->pfnObjUserMethod4;


/**************************************************************************************
 *
 * object etc info setting
 *
 **************************************************************************************/
	pNewGridObject->usItemsPerPage = pstObjCreateParam->usItemsPerPage;
	pNewGridObject->usSelectedIndex = pstObjCreateParam->usSelectedIndex;

	pNewGridObject->eCellAddDirection	= eCellAddDirection_Horizontal;

/**************************************************************************************
 *
 * object create & register
 *
 **************************************************************************************/
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)pNewGridObject, 0);

	INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)pNewGridObject);

/**************************************************************************************
 *
 * object area invalidation
 *
 **************************************************************************************/
	ONDK_GWM_APP_InvalidateRect(	pNewGridObject->rect.x,
						pNewGridObject->rect.y,
						pNewGridObject->rect.w,
						pNewGridObject->rect.h);

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_gridobject_GetObject(HINT32 ulId, GRID_OBJECT **ppGridObj)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	ONDK_GWM_GuiObject_t		pObjectList = NULL;
	PGRID_OBJECT		pGridObj = NULL;


	if(ppGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetObject] Param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*ppGridObj = NULL;

	GWM_assert(g_pstCurrentApp);

	pObjectList = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);
	if(pObjectList == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetObject] GetObjectList fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	pGridObj = (PGRID_OBJECT)INT_ONDK_GWM_OBJ_FindObject(pObjectList, (int)ulId);
	if(pObjectList == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetObject] FindObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	*ppGridObj = pGridObj;
	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}



static ONDK_Result_t		local_gridobject_AllocNewObject(GRID_OBJECT **ppGridObj)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HUINT32				ulIndex = 0;
	HTREEMGR			hTree = NULL;
	PGRID_OBJECT		pGridObj = NULL;
	HERROR				hError = ERR_FAIL;


	if(ppGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_AllocNewObject] Param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*ppGridObj = NULL;


	for (ulIndex = 0; ulIndex < MAX_GRID_OBJECT; ulIndex++)
	{
		if (s_stGridManager.ucIsAlloced[ulIndex] == 0)
		{
			break;
		}
	}

	if (ulIndex >= MAX_GRID_OBJECT)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_AllocNewObject] Fild available slot fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}


	hError = UTIL_TREEUTIL_CreateTree(&hTree);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_AllocNewObject] CreateTree fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;

	}

	pGridObj = &(s_stGridManager.astGridObject[ulIndex]);
	pGridObj->hTree = hTree;
	*ppGridObj = pGridObj;

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}




static ONDK_Result_t		local_gridobject_CleanObject(HINT32 ulId)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HUINT32				ulIndex = 0;
	PGRID_OBJECT		pGridObj = NULL;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hFirstChild = NULL;


	pGridObj = (PGRID_OBJECT)ONDK_GWM_APP_GetObject(ulId);
	if(pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_CleanObject] GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	for (ulIndex = 0; ulIndex < MAX_GRID_OBJECT; ulIndex++)
	{
		if (pGridObj == &(s_stGridManager.astGridObject[ulIndex]))
		{
			break;
		}
	}

	if (ulIndex >= MAX_GRID_OBJECT)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_CleanObject] Fild available slot fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetFirstChildItem(pGridObj->hTree, &hFirstChild);
	if(hError != ERR_OK || hFirstChild == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_CleanObject] UTIL_TREEUTIL_GetFirstChildItem fail!!\n"));
		hResult = GWM_RET_OK;
		goto EXIT_GRIDOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_RemoveItems(pGridObj->hTree, hFirstChild);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_CleanObject] UTIL_TREEUTIL_RemoveItems fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}






static ONDK_Result_t		local_gridobject_FreeObjectById(HINT32 ulId)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HUINT32				ulIndex = 0;
	PGRID_OBJECT		pGridObj = NULL;
	HERROR				hError = ERR_FAIL;
	EPG_GRID_OBJ_CHECK("(0)local_gridobject_FreeObjectById");


	pGridObj = (PGRID_OBJECT)ONDK_GWM_APP_GetObject(ulId);
	if(pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_FreeObject] GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	for (ulIndex = 0; ulIndex < MAX_GRID_OBJECT; ulIndex++)
	{
		if (pGridObj == &(s_stGridManager.astGridObject[ulIndex]))
		{
			break;
		}
	}

	if (ulIndex >= MAX_GRID_OBJECT)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_FreeObject] Fild available slot fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}


	hError = UTIL_TREEUTIL_DestroyTree(pGridObj->hTree);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_FreeObject] DestroyTree fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	s_stGridManager.ucIsAlloced[ulIndex] = 0;
	EPG_GRID_OBJ_CHECK("(1)local_gridobject_FreeObjectById");

	VK_memset(&(s_stGridManager.astGridObject[ulIndex]), 0x00, sizeof(GRID_OBJECT));
	INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(s_stGridManager.astGridObject[ulIndex]));

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(2)local_gridobject_FreeObjectById");

	return hResult;
}



static ONDK_Result_t		local_gridobject_FreeObject(PGRID_OBJECT pGridObj)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HUINT32				ulIndex = 0;
	HERROR				hError = ERR_FAIL;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_FreeObject");

	if(pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_FreeObject] GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	for (ulIndex = 0; ulIndex < MAX_GRID_OBJECT; ulIndex++)
	{
		if (pGridObj == &(s_stGridManager.astGridObject[ulIndex]))
		{
			break;
		}
	}

	if (ulIndex >= MAX_GRID_OBJECT)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_FreeObject] Fild available slot fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}


	hError = UTIL_TREEUTIL_DestroyTree(pGridObj->hTree);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_FreeObject] DestroyTree fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	s_stGridManager.ucIsAlloced[ulIndex] = 0;

	VK_memset(&(s_stGridManager.astGridObject[ulIndex]), 0x00, sizeof(GRID_OBJECT));
	INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(s_stGridManager.astGridObject[ulIndex]));

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(1)local_gridobject_FreeObject");

	return hResult;
}


STATIC ONDK_Result_t 	local_gridobject_SetAddDirection(HINT32 lId, CellAddDirection_t eCellAddDi)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
 	PGRID_OBJECT		pGridObj = NULL;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_SetAddDirection");

	pGridObj = (PGRID_OBJECT)ONDK_GWM_APP_GetObject(lId);
	if(pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_SetAddDirection] GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	pGridObj->eCellAddDirection = eCellAddDi;
	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	EPG_GRID_OBJ_CHECK("(1)local_gridobject_SetAddDirection");
	return hResult;
}


STATIC ONDK_Result_t 	local_gridobject_GetAddDirection(HINT32 lId, CellAddDirection_t *peCellAddDi)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	PGRID_OBJECT		pGridObj = NULL;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_GetAddDirection");

	if(peCellAddDi  == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetAddDirection] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	pGridObj = (PGRID_OBJECT)ONDK_GWM_APP_GetObject(lId);
	if(pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetAddDirection] GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	*peCellAddDi = pGridObj->eCellAddDirection;
	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	EPG_GRID_OBJ_CHECK("(1)local_gridobject_GetAddDirection");
	return hResult;

}

static ONDK_Result_t 	local_gridobject_DeleteItem(HINT32 ulId, HTREEITEM hDelItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	GRID_OBJECT 		*pGridObj = NULL;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_DeleteItem");

	if(hDelItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_DeleteItem] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;

	}
	hResult = local_gridobject_GetObject(ulId, &pGridObj);
	if(hResult != GWM_RET_OK || pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_DeleteItem] GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_RemoveItem(pGridObj->hTree, hDelItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_DeleteItem] RemoveItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(1)local_gridobject_DeleteItem");

	return hResult;

}

static ONDK_Result_t 	local_gridobject_DeleteEventItems(HINT32 ulId, HTREEITEM hSvcItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	GRID_OBJECT 			*pGridObj = NULL;
	HTREEITEM			hTempItem = NULL;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_DeleteEventItems");

	if(hSvcItem == NULL )
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_DeleteEventItems] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hResult = local_gridobject_GetObject(ulId, &pGridObj);
	if(hResult != GWM_RET_OK || pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_DeleteEventItems] GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hError  = UTIL_TREEUTIL_GetChildItem( hSvcItem, &hTempItem );
	if( hError != ERR_OK )
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_DeleteEventItems] GetEventItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_RemoveItems(pGridObj->hTree, hTempItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_DeleteEventItems] InsertItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(1)local_gridobject_DeleteEventItems");

	return hResult;

}

static ONDK_Result_t		local_gridobject_InsertItem(HINT32 ulId, HTREEITEM hParent, HTREEITEM hPrevLink, HTREEITEM hNextLink, TREEITEM_PARAM_t *pParam, HTREEITEM *phNewItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	GRID_OBJECT 		*pGridObj = NULL;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_InsertItem");

	if(pParam == NULL || phNewItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_InsertItem] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;

	}
	hResult = local_gridobject_GetObject(ulId, &pGridObj);
	if(hResult != GWM_RET_OK || pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemRect] GetDisplayItemRect fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_InsertItem(pGridObj->hTree, hParent, hPrevLink, hNextLink, pParam, phNewItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_InsertItem] InsertItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(1)local_gridobject_InsertItem");

	return hResult;

}




static ONDK_Result_t		local_gridobject_GetNextItem(HTREEITEM hItem, HTREEITEM *phNextItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hTempItem = NULL;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_GetNextItem");

	if(hItem == NULL || phNextItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetNextItem] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*phNextItem = NULL;

	hError = UTIL_TREEUTIL_GetNextItem(hItem, &hTempItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetNextItem] GetNextItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	*phNextItem = hTempItem;

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(1)local_gridobject_GetNextItem");

	return hResult;
}




static ONDK_Result_t		local_gridobject_GetPrevItem(HTREEITEM hItem, HTREEITEM *phPrevItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hTempItem = NULL;
	EPG_GRID_OBJ_CHECK("(0)local_gridobject_GetPrevItem");

	if(hItem == NULL || phPrevItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetPrevItem] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*phPrevItem = NULL;

	hError = UTIL_TREEUTIL_GetPrevItem(hItem, &hTempItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetPrevItem] GetNextItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	*phPrevItem = hTempItem;

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(1)local_gridobject_GetPrevItem");

	return hResult;
}


static ONDK_Result_t		local_gridobject_GetEventItem(HTREEITEM hSvcItem, HTREEITEM *phEventItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hTempItem = NULL;
	EPG_GRID_OBJ_CHECK("(0)local_gridobject_GetEventItem");

	if(hSvcItem == NULL || phEventItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetEventItem] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*phEventItem = NULL;

	hError = UTIL_TREEUTIL_GetChildItem(hSvcItem, &hTempItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetPrevItem] GetNextItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*phEventItem = hTempItem;

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(1)local_gridobject_GetEventItem");

	return hResult;
}



static ONDK_Result_t		local_gridobject_GetItemData(HTREEITEM hItem, void **ppData)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	void					*pLocalData = NULL;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_GetItemData");

	if(hItem == NULL || ppData == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemData] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*ppData = NULL;

	hError = UTIL_TREEUTIL_GetItemData(hItem, &pLocalData);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemData] GetNextItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*ppData = pLocalData;
	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(1)local_gridobject_GetItemData");

	return hResult;
}



static ONDK_Result_t		local_gridobject_GetServiceItemByIndex(HUINT32 ulId, HUINT32 ulSvcIndex, HTREEITEM *phSvcItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hTempItem = NULL;
	HTREEITEM			hFirstChild = NULL;
	HUINT16				usIndex = 0;
	GRID_OBJECT 		*pGridObj = NULL;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_GetServiceItemByIndex");

	if(phSvcItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetServiceItemByIndex] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*phSvcItem = NULL;

	hResult = local_gridobject_GetObject(ulId, &pGridObj);
	if(hResult != GWM_RET_OK || pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetServiceItemByIndex] local_gridobject_GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetFirstChildItem(pGridObj->hTree, &hFirstChild);
	if(hError != ERR_OK || hFirstChild == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetServiceItemByIndex] UTIL_TREEUTIL_GetFirstChildItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hTempItem = hFirstChild;
	for(usIndex = 0; usIndex < ulSvcIndex; usIndex++)
	{
		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hTempItem == NULL || hError != ERR_OK)
		{
			break;
		}
	}

	*phSvcItem = hTempItem;

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(1)local_gridobject_GetServiceItemByIndex");

	return hResult;
}




#if 0	/* not used */
static ONDK_Result_t		local_gridobject_GetLayerEndItem(HTREEITEM hItem, HTREEITEM *phLayerEndItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hEndItem = NULL;
	HTREEITEM			hTempItem = NULL;

	if(hItem == NULL || phLayerEndItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetLayerEndItem] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*phLayerEndItem = NULL;

	hEndItem = hItem;
	hError = UTIL_TREEUTIL_GetNextItem(hItem, &hTempItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetLayerEndItem] GetNextItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	while(hTempItem)
	{
		hEndItem = hTempItem;
		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetLayerEndItem] GetNextItem fail!!\n"));
		}
	}
	*phLayerEndItem = hEndItem;

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}
#endif


#if 0	/* not used */
static ONDK_Result_t		local_gridobject_GetLayerStartItem(HTREEITEM hItem, HTREEITEM *phLayerStartItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM			hStartItem = NULL;
	HTREEITEM			hTempItem = NULL;

	if(hItem == NULL || phLayerStartItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetLayerStartItem] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*phLayerStartItem = NULL;

	hStartItem = hItem;
	hError = UTIL_TREEUTIL_GetPrevItem(hItem, &hTempItem);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetLayerStartItem] GetPrevItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	while(hTempItem)
	{
		hStartItem = hTempItem;
		hError = UTIL_TREEUTIL_GetPrevItem(hTempItem, &hTempItem);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetLayerStartItem] GetPrevItem fail!!\n"));
		}
	}

	*phLayerStartItem = hStartItem;

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}
#endif



static ONDK_Result_t		local_gridobject_GetSelectedItem(PGRID_OBJECT pGridObject, HTREEITEM *phSelectedItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;

	if(pGridObject == NULL || phSelectedItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetSelectedItem] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	*phSelectedItem = pGridObject->hSelectedItem;

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}




static ONDK_Result_t		local_gridobject_SelectItem(PGRID_OBJECT pGridObject, HTREEITEM hItem)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;

	if(pGridObject == NULL || hItem == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_SelectItem] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	pGridObject->hSelectedItem = hItem;

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}




static ONDK_Result_t		local_gridobject_GetItemRect(PGRID_OBJECT pGridObject, HTREEITEM hItem, ONDK_Rect_t *pItemRect)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HERROR				hError = ERR_FAIL;
	HTREEITEM 			hStart = NULL;
	HTREEITEM 			hFirstChild = NULL;
	HTREEITEM 			hTemp = NULL;
	HTREEITEM 			hIndexItem = NULL;
	HUINT16				usIndex = 0;
	ONDK_Rect_t			stItemRect;
	HBOOL				bDetected = FALSE;
	HUINT32				ulDepth = 0;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_GetItemRect");

	if(pGridObject == NULL || pItemRect == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemRect] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	VK_memset(pItemRect, 0x00, sizeof(ONDK_Rect_t));
	VK_memset(&stItemRect, 0x00, sizeof(ONDK_Rect_t));
	hError = UTIL_TREEUTIL_GetFirstChildItem(pGridObject->hTree, &hFirstChild);
	if(hError != ERR_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemRect] GetFirstChildItem fail!!\n"));
	}
	hStart = hFirstChild;

	hTemp = hStart;
	if(hTemp == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemRect] tree haven't any child!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	EPG_GRID_OBJ_CHECK("(1)local_gridobject_GetItemRect");

	hIndexItem = hItem;
	hResult = UTIL_TREEUTIL_GetItemDepth(hItem, &ulDepth);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemRect] GetItemDepth fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	if(ulDepth == MAX_GRID_DEPTH)
	{
		hResult = UTIL_TREEUTIL_GetParentItem(hItem, &hIndexItem);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemRect] GetParentItem fail!!\n"));
			goto EXIT_GRIDOBJ_FUNC;
		}
	}

	EPG_GRID_OBJ_CHECK("(2)local_gridobject_GetItemRect");

	while(hTemp)
	{
		if(hTemp == hIndexItem)
		{
			hResult = local_gridobject_GetDrawItemRect(pGridObject, hItem, usIndex, ulDepth, &stItemRect);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemRect] GetDisplayItemRect fail!!\n"));
				goto EXIT_GRIDOBJ_FUNC;
			}
			VK_memcpy(pItemRect, &stItemRect, sizeof(ONDK_Rect_t));
			bDetected = TRUE;
			break;
		}
		usIndex++;
		hResult = local_gridobject_GetNextItem(hTemp, &hTemp);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemRect] FindNextItem fail!!\n"));
		}
		if(usIndex >= pGridObject->usItemsPerPage)
		{
			break;
		}
	}

	EPG_GRID_OBJ_CHECK("(3)local_gridobject_GetItemRect");

	if(bDetected == TRUE)
	{
		hResult = GWM_RET_OK;
	}

EXIT_GRIDOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_gridobject_UpdateFocus(HUINT32 ulId)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HTREEITEM			hItem = NULL;
	PGRID_OBJECT		pGridObj = NULL;
	ONDK_Rect_t			stSelectedRect;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_UpdateFocus");

	pGridObj = (PGRID_OBJECT)ONDK_GWM_APP_GetObject(ulId);
	if(pGridObj == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_UpdateFocus] ONDK_GWM_APP_GetObject fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	EPG_GRID_OBJ_CHECK("(1)local_gridobject_UpdateFocus");

	hResult = local_gridobject_SelectItem(pGridObj, &hItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_UpdateFocus] local_gridobject_SelectItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	EPG_GRID_OBJ_CHECK("(2)local_gridobject_UpdateFocus");

	hResult = local_gridobject_GetItemRect(pGridObj, hItem, &stSelectedRect);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_UpdateFocus] local_gridobject_GetItemRect fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

#if 0
	ONDK_GWM_APP_InvalidateRelativeRect(stSelectedRect.x , stSelectedRect.y , stSelectedRect.w , stSelectedRect.h );
#elif  0
	if(stSelectedRect.h < EPGGRID_EVENTNAME_POPUP_H)
	{
		stSelectedRect.h = EPGGRID_EVENTNAME_POPUP_H;
	}
	ONDK_GWM_APP_InvalidateRelativeRect(stSelectedRect.x, stSelectedRect.y, stSelectedRect.w, stSelectedRect.h);

#else
	ONDK_GWM_APP_InvalidateRelativeRect(stSelectedRect.x - 4, stSelectedRect.y - 8, stSelectedRect.w + 16, stSelectedRect.h + 24);
#endif
	EPG_GRID_OBJ_CHECK("(3)local_gridobject_UpdateFocus");

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}


static ONDK_Result_t		local_gridobject_GetDrawItemRect(PGRID_OBJECT pGridObject, HTREEITEM hItem, HUINT16 usIndex, HUINT32 ulDepth, ONDK_Rect_t *pItemRect)
{
	ONDK_Result_t		hResult = GWM_RET_INITIAL_FAILED;
	ONDK_Rect_t			stBaseRect;
	ONDK_Rect_t			stDrawRect;
	HUINT32 			ulY = 0, ulX = 0, ulW = 0;
	void				*pLocalData = NULL;

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_GetDrawItemRect");

	if(pGridObject == NULL ||pItemRect == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetDrawItemRect] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
	if(usIndex >= pGridObject->usItemsPerPage)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetDrawItemRect] param index fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	ulX = ulY = ulW = 0;

	switch(pGridObject->eCellAddDirection)
	{
	case eCellAddDirection_Vertically:
		if(ulDepth == MAX_GRID_DEPTH)
		{
			ulX = (pGridObject->stItemMargin2.w * (usIndex));
			if(pGridObject->usItemsPerPage)
			{
				ulW = pGridObject->stItemRect2.w / pGridObject->usItemsPerPage;
			}

			stBaseRect.y = pGridObject->stItemRect2.y + pGridObject->stItemMargin2.y;
			stBaseRect.x = ulX + pGridObject->stItemRect2.x + (ulW*usIndex);
			stBaseRect.h = pGridObject->stItemRect2.h;
			if(pGridObject->usItemsPerPage == usIndex + 1)
			{
				stBaseRect.w = (pGridObject->stItemRect2.x + pGridObject->stItemRect2.w - 1) - stBaseRect.x + 1;
			}
			else
			{
				stBaseRect.w = ulW;
			}
		}
		else
		{
			ulX = (pGridObject->stItemMargin1.w * (usIndex));
			if(pGridObject->usItemsPerPage)
			{
				ulW = pGridObject->stItemRect1.w / pGridObject->usItemsPerPage;
			}

			stBaseRect.y = pGridObject->stItemRect1.y + pGridObject->stItemMargin1.y;
			stBaseRect.x = ulX+ pGridObject->stItemRect1.x + (ulW*usIndex);
			stBaseRect.h = pGridObject->stItemRect1.h;

			if(pGridObject->usItemsPerPage == usIndex + 1)
			{
				stBaseRect.w = (pGridObject->stItemRect1.x + pGridObject->stItemRect1.w - 1) - stBaseRect.x + 1;
			}
			else
			{
				stBaseRect.w = ulW;
			}
		}
		break;

	default:
	case eCellAddDirection_Horizontal:
		if(ulDepth == MAX_GRID_DEPTH)
		{
			ulY = (pGridObject->stItemMargin2.h * (usIndex));

			stBaseRect.x = pGridObject->stItemRect2.x + pGridObject->stItemMargin2.x;
			stBaseRect.y = ulY + pGridObject->stItemRect2.y + (pGridObject->stItemRect2.h*usIndex);
			stBaseRect.w = pGridObject->stItemRect2.w;
			stBaseRect.h = pGridObject->stItemRect2.h;
		}
		else
		{
			ulY = (pGridObject->stItemMargin1.h * (usIndex));

			stBaseRect.x = pGridObject->stItemRect1.x + pGridObject->stItemMargin1.x;
			stBaseRect.y = ulY+ pGridObject->stItemRect1.y + (pGridObject->stItemRect1.h*usIndex);
			stBaseRect.w = pGridObject->stItemRect1.w;
			stBaseRect.h = pGridObject->stItemRect1.h;
		}
		break;
	}

	EPG_GRID_OBJ_CHECK("(1)local_gridobject_GetDrawItemRect");

	if(hItem != NULL)
	{
		hResult = UTIL_TREEUTIL_GetItemData(hItem, &pLocalData);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetDrawItemRect] GetItemData fail!!\n"));
			goto EXIT_GRIDOBJ_FUNC;
		}

		switch(pGridObject->eCellAddDirection)
		{
		case eCellAddDirection_Vertically:
			ulDepth = (usIndex << 16) | ulDepth;
			break;

		case eCellAddDirection_Horizontal:
		default:
			break;
		}

		hResult = pGridObject->objGetDrawItemRect((HUINT32)pLocalData, (HUINT32)&stBaseRect, (HUINT32)&stDrawRect, (HUINT32)ulDepth);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetDrawItemRect] objGetDrawItemRect fail!!\n"));
			goto EXIT_GRIDOBJ_FUNC;
		}
	}
	else
	{
		OSD_GFX_CopyRect(&stDrawRect, &stBaseRect);
	}
	EPG_GRID_OBJ_CHECK("(2)local_gridobject_GetDrawItemRect");

	hResult = ONDK_Rect(pItemRect, stDrawRect.x, stDrawRect.y, stDrawRect.w, stDrawRect.h);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetDrawItemRect] ONDK_Rect fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(3)local_gridobject_GetDrawItemRect");

	return hResult;
}


static ONDK_Result_t		local_gridobject_GetItemParamInfo(HUINT32 ulParam, HBOOL *pbSelected, HBOOL *pbFront, HUINT8 *pucDepth, HUINT8 *pucIndex)
{
	ONDK_Result_t				hResult = GWM_RET_INITIAL_FAILED;


	if(pbSelected == NULL || pucDepth == NULL || pucIndex == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_GetItemDrawInfo] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	*pbFront = (HBOOL)((ulParam & 0x00f00000)>>20);
	*pbSelected = (HBOOL)((ulParam & 0x000f0000)>>16);
	*pucDepth = (HUINT8)((ulParam & 0x0000ff00)>>8);
	*pucIndex = (HUINT8)(ulParam & 0x000000ff);

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :

	return hResult;
}


#define GRID_FRONT		0x00100000
#define GRID_SELECTED	0x00010000
#define GRID_DEPTH1		0x00000100
#define GRID_DEPTH2		0x00000200

static ONDK_Result_t		local_gridobject_UpdatePageItems(PGRID_OBJECT pGridObject, HBOOL bFront)
{
	HTREEITEM 				hTempDepth1 = NULL;
	HTREEITEM 				hTempDepth2 = NULL;
	HTREEITEM				hSelectedSvc = NULL;
	HUINT8					ucIndex = 0;
	HUINT8					ucSelectedIndex = 0;
	ONDK_Rect_t				stSelectedRect1;
	ONDK_Rect_t				stSelectedRect2;
	ONDK_Rect_t				stItemRect1;
	ONDK_Rect_t				stItemRect2;
	ONDK_Result_t				hResult = GWM_RET_INITIAL_FAILED;
	HERROR					hError = ERR_FAIL;
	ONDK_Region_t 				stAppArea;
	void						*pLocalData = NULL;
	void						*pDepth1Data = NULL;
	void						*pDepth2Data = NULL;
	void						*pSelected1Data = NULL;
	void						*pSelected2Data = NULL;
	HUINT32					ulArg = 0;

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &stAppArea);

	EPG_GRID_OBJ_CHECK("(0)local_gridobject_UpdatePageItems");

	if(pGridObject == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] param fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	hError = UTIL_TREEUTIL_GetFirstChildItem(pGridObject->hTree, &hTempDepth1);
	if(hError != ERR_OK)
	{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] GetFirstChildItem fail!!\n"));
	}
	if(hTempDepth1 == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] tree have no child!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	EPG_GRID_OBJ_CHECK("(1)local_gridobject_UpdatePageItems");

	hError = UTIL_TREEUTIL_GetParentItem(pGridObject->hSelectedItem, &hSelectedSvc);
	if(hSelectedSvc == NULL)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] UTIL_TREEUTIL_GetParentItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	if(bFront)
	{
		ulArg |= GRID_FRONT;
	}
	EPG_GRID_OBJ_CHECK("(2)local_gridobject_UpdatePageItems");

	do
	{
		pLocalData = NULL;
#if		1
		if(hTempDepth1 == NULL)
		{
			hResult = local_gridobject_GetDrawItemRect(pGridObject, NULL, ucIndex, MAX_GRID_DEPTH - 1, &stItemRect1);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] GetDrawItemRect fail 1!!\n"));
				goto EXIT_GRIDOBJ_DEPTH1;
			}
			if(pGridObject->objDrawItemMethod != NULL)
			{
				pGridObject->objDrawItemMethod(	ONDK_GWM_GetSurface(),
												&stAppArea,
												&stItemRect1,
												(ONDK_GWM_GuiObject_t)pGridObject,
												(void*)NULL,
												(ulArg|GRID_DEPTH1|ucIndex));
			}

			hResult = local_gridobject_GetDrawItemRect(pGridObject, NULL, ucIndex, MAX_GRID_DEPTH, &stItemRect2);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] GetDrawItemRect fail 2!!\n"));
				goto EXIT_GRIDOBJ_DEPTH1;
			}
			if(pGridObject->objDrawItemMethod != NULL)
			{
				pGridObject->objDrawItemMethod(	ONDK_GWM_GetSurface(),
												&stAppArea,
												&stItemRect2,
												(ONDK_GWM_GuiObject_t)pGridObject,
												(void*)NULL,
												(ulArg|GRID_DEPTH2|ucIndex));
			}
			goto EXIT_GRIDOBJ_DEPTH1;
		}
#endif
		if(ucIndex == 0)
		{
			pGridObject->hDispStart = hTempDepth1;
		}
		pGridObject->hDispEnd = hTempDepth1;

/**************************************************************************************
 *
 * service draw
 *
 **************************************************************************************/
		hResult = local_gridobject_GetDrawItemRect(pGridObject, hTempDepth1, ucIndex, MAX_GRID_DEPTH - 1, &stItemRect1);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] GetDrawItemRect fail 1!!\n"));
			goto EXIT_GRIDOBJ_DEPTH1;
		}
		hResult = UTIL_TREEUTIL_GetItemData(hTempDepth1, &pLocalData);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] GetItemData fail!!\n"));
			goto EXIT_GRIDOBJ_DEPTH1;
		}
		pDepth1Data = pLocalData;
		if(pGridObject->objDrawItemMethod != NULL)
		{
			pGridObject->objDrawItemMethod(	ONDK_GWM_GetSurface(),
											&stAppArea,
											&stItemRect1,
											(ONDK_GWM_GuiObject_t)pGridObject,
											(void*)pDepth1Data,
											(ulArg|GRID_DEPTH1|ucIndex));
		}


/**************************************************************************************
 *
 * event list draw
 *
 **************************************************************************************/
		hTempDepth2 = NULL;
		hError = UTIL_TREEUTIL_GetChildItem(hTempDepth1, &hTempDepth2);
		if(hError != ERR_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] GetChildItem fail!!\n"));
		}

		while(hTempDepth2)
		{
			pLocalData = NULL;
			hResult = local_gridobject_GetDrawItemRect(pGridObject, hTempDepth2, ucIndex, MAX_GRID_DEPTH, &stItemRect2);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] GetDrawItemRect fail 2!!\n"));
				goto EXIT_GRIDOBJ_DEPTH2;
			}
			hResult = UTIL_TREEUTIL_GetItemData(hTempDepth2, &pLocalData);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] GetItemData fail!!\n"));
				goto EXIT_GRIDOBJ_DEPTH2;
			}
			pDepth2Data = pLocalData;
			if(pGridObject->objDrawItemMethod != NULL)
			{
				if(pGridObject->hSelectedItem == hTempDepth2)
				{
					ucSelectedIndex = ucIndex;
					pSelected1Data = pDepth1Data;
					pSelected2Data = pDepth2Data;
					VK_memcpy32(&stSelectedRect1, &stItemRect1, sizeof(ONDK_Rect_t));
					VK_memcpy32(&stSelectedRect2, &stItemRect2, sizeof(ONDK_Rect_t));
				}
				pGridObject->objDrawItemMethod(	ONDK_GWM_GetSurface(),
												&stAppArea,
												&stItemRect2,
												(ONDK_GWM_GuiObject_t)pGridObject,
												(void*)pDepth2Data,
												(ulArg|GRID_DEPTH2|ucIndex));
			}

EXIT_GRIDOBJ_DEPTH2 :
			hResult = local_gridobject_GetNextItem(hTempDepth2, &hTempDepth2);
			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] GetNextItem fail!!\n"));
			}
		}


EXIT_GRIDOBJ_DEPTH1 :
		hResult = local_gridobject_GetNextItem(hTempDepth1, &hTempDepth1);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_PageItemUpdate] GetNextItem fail!!\n"));
		}

		ucIndex++;
#if		0
		if(ucIndex >= pGridObject->usItemsPerPage)
		{
			break;
		}
#endif
	}while(ucIndex <= pGridObject->usItemsPerPage);

/**************************************************************************************
 *
 * focus draw
 *
 **************************************************************************************/

	EPG_GRID_OBJ_CHECK("(3)local_gridobject_UpdatePageItems");
	if(pSelected1Data)
	{
		if(pGridObject->objDrawItemMethod != NULL)
		{
			pGridObject->objDrawItemMethod(	ONDK_GWM_GetSurface(),
											&stAppArea,
											&stSelectedRect1,
											(ONDK_GWM_GuiObject_t)pGridObject,
											(void*)pSelected1Data,
											(ulArg|GRID_SELECTED | GRID_DEPTH1 |ucSelectedIndex));
		}
	}
	EPG_GRID_OBJ_CHECK("(4)local_gridobject_UpdatePageItems");

	if(pSelected2Data)
	{
		if(pGridObject->objDrawItemMethod != NULL)
		{
			pGridObject->objDrawItemMethod(	ONDK_GWM_GetSurface(),
											&stAppArea,
											&stSelectedRect2,
											(ONDK_GWM_GuiObject_t)pGridObject,
											(void*)pSelected2Data,
											(ulArg|GRID_SELECTED | GRID_DEPTH2 | ucSelectedIndex));
		}
	}

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(5)local_gridobject_UpdatePageItems");

	return hResult;
}


static ONDK_Result_t		local_gridobject_MoveKeyProc(PGRID_OBJECT pGridObject, HINT32 lParam)
{
	ONDK_Result_t			hResult = GWM_RET_INITIAL_FAILED;
	HBOOL				bUpdateAll = FALSE;
	HTREEITEM			hOldSelectedItem = NULL;
	HTREEITEM			hNewSelectedItem = NULL;
	HTREEITEM			hOldSvcItem = NULL;
	HTREEITEM			hNewSvcItem = NULL;
	ONDK_Rect_t			stOldItemRect;
	ONDK_Rect_t			stNewItemRect;
	ONDK_Rect_t			stOldParentItemRect;
	ONDK_Rect_t			stNewParentItemRect;

	GWM_User_t			pobjArrowProcUpDown 	= (GWM_User_t)NULL;
	GWM_User_t			pobjArrowProcLeftRight 	= (GWM_User_t)NULL;


	EPG_GRID_OBJ_CHECK("(0)local_gridobject_MoveKeyProc");

	hResult = local_gridobject_GetSelectedItem(pGridObject, &hOldSelectedItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] local_gridobject_GetSelectedItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

	pobjArrowProcUpDown 	= pGridObject->objProcUpDown;
	pobjArrowProcLeftRight 	= pGridObject->objProcLeftRight;
	switch (lParam)
	{
		case KEY_ARROWUP :
		case KEY_ARROWDOWN :
		case KEY_PAGEUP :
		case KEY_PAGEDOWN :
			if(pobjArrowProcUpDown != NULL)
			{
				hResult = pobjArrowProcUpDown(lParam, (HUINT32)&bUpdateAll, 0, 0);
			}

			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] KEY_ARROWUP/KEY_ARROWDOWN fail!!\n"));
				goto EXIT_GRIDOBJ_FUNC;
			}

			break;

#if 0
		case KEY_PREV :
		case KEY_NEXT :
		case KEY_BACKWARD :
		case KEY_FORWARD :
#endif
		case KEY_ARROWLEFT :
		case KEY_ARROWRIGHT :
			if(pobjArrowProcLeftRight != NULL)
			{
				hResult = pobjArrowProcLeftRight(lParam, (HUINT32)&bUpdateAll, 0, 0);
			}

			if(hResult != GWM_RET_OK)
			{
				GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] KEY_ARROWLEFT/KEY_ARROWRIGHT fail!!\n"));
				goto EXIT_GRIDOBJ_FUNC;
			}
			break;
		default:
			break;
	}

	hResult = local_gridobject_GetSelectedItem(pGridObject, &hNewSelectedItem);
	if(hResult != GWM_RET_OK)
	{
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] local_gridobject_GetSelectedItem fail!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}

#if  0
	if( hNewSelectedItem == hOldSelectedItem )
	{/* 같은거다 괜히 그릴 필요 없다. */
		GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] Same Event Item!!\n"));
		goto EXIT_GRIDOBJ_FUNC;
	}
#endif

	EPG_GRID_OBJ_CHECK("(1)local_gridobject_MoveKeyProc");

	if(bUpdateAll == TRUE)
	{
		ONDK_GWM_APP_InvalidateRelativeRect(pGridObject->rect.x, pGridObject->rect.y, pGridObject->rect.w, pGridObject->rect.h);
	}
	else
	{
		HINT32 nBaseY2 = 0;

		nBaseY2 = pGridObject->rect.y + pGridObject->rect.h - 1;

		UTIL_TREEUTIL_GetParentItem(hOldSelectedItem, &hOldSvcItem);
		if(hOldSvcItem == NULL)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] UTIL_TREEUTIL_GetParentItem fail!!\n"));
			goto EXIT_GRIDOBJ_FUNC;
		}

		UTIL_TREEUTIL_GetParentItem(hNewSelectedItem, &hNewSvcItem);
		if(hNewSvcItem == NULL)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] UTIL_TREEUTIL_GetParentItem fail!!\n"));
			goto EXIT_GRIDOBJ_FUNC;
		}

		hResult = local_gridobject_GetItemRect(pGridObject, hOldSvcItem, &stOldParentItemRect);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] local_gridobject_GetItemRect fail!!\n"));
			goto EXIT_GRIDOBJ_FUNC;
		}
		hResult = local_gridobject_GetItemRect(pGridObject, hNewSvcItem, &stNewParentItemRect);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] local_gridobject_GetItemRect fail!!\n"));
			goto EXIT_GRIDOBJ_FUNC;
		}

#if  0
		/* +1 은 float 계산의  오차 */
		if(nBaseY2 <= stOldParentItemRect.y + stOldParentItemRect.h)
		{
			stOldParentItemRect.h = nBaseY2 - stOldParentItemRect.y;
		}
		ONDK_GWM_APP_InvalidateRelativeRect(stOldParentItemRect.x, stOldParentItemRect.y, stOldParentItemRect.w, stOldParentItemRect.h);

		if(nBaseY2 <= stNewParentItemRect.y + stNewParentItemRect.h)
		{
			stNewParentItemRect.h = nBaseY2 - stNewParentItemRect.y;
		}
		ONDK_GWM_APP_InvalidateRelativeRect(stNewParentItemRect.x, stNewParentItemRect.y, stNewParentItemRect.w, stNewParentItemRect.h);
#else
		ONDK_GWM_APP_InvalidateRelativeRect(stOldParentItemRect.x, stOldParentItemRect.y, stOldParentItemRect.w, stOldParentItemRect.h);
		ONDK_GWM_APP_InvalidateRelativeRect(stNewParentItemRect.x, stNewParentItemRect.y, stNewParentItemRect.w, stNewParentItemRect.h );
#endif
		hResult = local_gridobject_GetItemRect(pGridObject, hOldSelectedItem, &stOldItemRect);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] local_gridobject_GetItemRect fail!!\n"));
			goto EXIT_GRIDOBJ_FUNC;
		}

		hResult = local_gridobject_GetItemRect(pGridObject, hNewSelectedItem, &stNewItemRect);
		if(hResult != GWM_RET_OK)
		{
			GWM_DBG_Print(DBG_PRINT, ("[local_gridobject_MoveKeyProc] local_gridobject_GetItemRect fail!!\n"));
			goto EXIT_GRIDOBJ_FUNC;
		}

		ONDK_GWM_APP_InvalidateRelativeRect(stNewItemRect.x - 4, stNewItemRect.y - 8, stNewItemRect.w + 12, stNewItemRect.h + 24);
		ONDK_GWM_APP_InvalidateRelativeRect(stOldItemRect.x - 4, stOldItemRect.y - 8, stOldItemRect.w + 12, stOldItemRect.h + 24);
	}

	hResult = GWM_RET_OK;

EXIT_GRIDOBJ_FUNC :
	EPG_GRID_OBJ_CHECK("(2)local_gridobject_MoveKeyProc");

	return hResult;
}

#if 0
static HUINT32 s_nEpgDrawTick = 0;
#endif /* #if defined(CONFIG_DEBUG) */

static ONDK_Result_t		local_gridobject_defaultProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Region_t			area;
	ONDK_Rect_t			rect;
	PGRID_OBJECT		pGridObject = NULL;


	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);

	INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

	pGridObject = (PGRID_OBJECT)object;


	switch (proc_msg)
	{
		case MT_DESTROY:
			local_gridobject_FreeObject(pGridObject);
			return GWM_MESSAGE_BREAK;
		case MT_DRAW:
#if 0
			HxLOG_Print("[%s] %d\n", "Grid Object MT_DRAW", VK_TIMER_GetSystemTick()-s_nEpgDrawTick);
			s_nEpgDrawTick = VK_TIMER_GetSystemTick();
#endif /* #if defined(CONFIG_DEBUG) */
			local_gridobject_UpdatePageItems(pGridObject, (HBOOL)arg);
			return GWM_MESSAGE_BREAK;

		case MT_KEYDOWN:
			switch (arg)
			{
				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
				case KEY_PAGEUP :
				case KEY_PAGEDOWN :
				case KEY_ARROWLEFT:
				case KEY_ARROWRIGHT:

#if 0
				case KEY_PREV :
				case KEY_NEXT :
				case KEY_BACKWARD :
				case KEY_FORWARD :
#endif
					local_gridobject_MoveKeyProc(pGridObject, arg);
					return GWM_MESSAGE_BREAK;

				/* Front OK 키에 대한 처리가 없어서 Object가 프론트 키에 반응을 안해서 추가한다.
			현재 Navy(UI) 기준이고.. 다른향에서 문제되면 해당 부분에 수정이 필요할 것 같다. */
				case KEY_FRONT_OK:
				case KEY_OK:
					if(pGridObject->objProcOK != NULL)
					{
						pGridObject->objProcOK(0, 0, 0, 0);
					}
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

	return GWM_MESSAGE_PASS;
}





/* End of File. ---------------------------------------------------------- */

