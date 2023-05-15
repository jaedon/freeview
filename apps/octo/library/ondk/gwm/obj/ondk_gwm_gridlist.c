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



/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <ondk.h>

#include <ondk_gwm_appobj.h>
#include <ondk_gwm_component.h>


#include "../ondk_gwm_objdef.h"
#include "../ondk_gwm_internal.h"

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define	GWMGRID_RETURN_CHECK_OBJ(eRetCheck)	do{if(eRetCheck != ONDK_RET_OK){HxLOG_Error("No GridList Object!\n");return GWM_RET_NO_OBJECT;}}while(0)

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
#define GRIDLIST_ANI_MOVE_INTERVAL	32
#endif

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */

typedef enum
{
	eGridListAniType_NONE,
	eGridListAniType_MOVEUP,
	eGridListAniType_MOVEDOWN,
}ONDK_GridListAniType_e;

struct tagGridListObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.

	HCHAR				*title; 			// List 이름.
	HINT32				cellHeight;			// Grid Item height
	HINT32				cellWidth;			// Grid Item Width
	HINT32				surfaceWidth;
	HINT32				maxColCount;		// Grid List 최대 열 갯수.
	HINT32				maxRowCount;		// Grid List 최대 행 갯수.
	HINT32				itemCount; 			// 전체 선택 가능한 Item의 갯수.
	HINT32				startIndex;			// Grid List 표시의 첫번째.
	HINT32				currentIndex;		// 현재 커서가 있는 Item의 인덱스.
	HINT32				selectedIndex; 		// 현재 선택되어 있는 Item의 인덱스.
	HINT32				surfaceNumber;		// Surface 갯수.
	GRID_ItemDraw_t		itemDrawMethod;		// Grid Item Draw Function
	LINKED_SURFACE_OBJECT	headLinkedSurfaceObj;  //Linked List의 Head Surface

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	pfnGridListAnimationCtl		fnSetAnimationCtl;
	ONDK_Rect_t					stRectInScreen;
	ONDK_Region_t				stClearRegion;
	HINT32						nAniYPos;
	ONDK_GridListAniType_e		eAniType;
	HBOOL						bDoingAni;
#endif
};

struct tagLinkedSurfaceObject
{
	LINK_OBJECT					nextLink;
	ONDKSurfaceHandle			surfaceHandle;
};



/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static struct tagGridListObject			s_defGridList;
static struct tagLinkedSurfaceObject	s_defLinkedSurface;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	local_grid_CheckObj(GRID_LIST_OBJECT obj);
static ONDK_Result_t	local_grid_CreateSurface(GRID_LIST_OBJECT gridlist);
static void		local_grid_ArrowUp(GRID_LIST_OBJECT gridListObj);
static void		local_grid_ArrowDown(GRID_LIST_OBJECT gridListObj);
static void		local_grid_ArrowLeft(GRID_LIST_OBJECT gridListObj);
static void		local_grid_ArrowRight(GRID_LIST_OBJECT gridListObj);
static HINT32	local_grid_always_ReturnOne(LINK_OBJECT node1, LINK_OBJECT node2);
static GRID_LIST_OBJECT	local_grid_AllocNewGridListObj(void);
static LINKED_SURFACE_OBJECT	local_grid_AllocNewLinkedSurfaceObj(void);
static GRID_LIST_OBJECT	local_grid_GetGridListObject(int id);
static  void	local_grid_SetGridListByCurrentIndex(GRID_LIST_OBJECT gridObj, HINT32 index);
static ONDK_Result_t	local_grid_defaultDestroyGridList(GRID_LIST_OBJECT gridObj);
static ONDK_Result_t	local_grid_defaultDrawGridListMethod(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																			ONDK_GWM_GuiObject_t object, GRID_LIST_OBJECT gridObject, int param1);
static ONDK_Result_t	local_grid_defaultGridListProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg);

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
static void	local_grid_StartAnimationUp(GRID_LIST_OBJECT gridObject);
static void	local_grid_StartAnimationDown(GRID_LIST_OBJECT gridObject);
#endif

/******************************************************************************/
#define _________Public_Prototype____________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Function____________________________________________
/******************************************************************************/
static ONDK_Result_t	local_grid_CheckObj(GRID_LIST_OBJECT obj)
{
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_GRID)
		return GWM_RET_NO_OBJECT;
	return ONDK_RET_OK;
}

static ONDK_Result_t	local_grid_CreateSurface(GRID_LIST_OBJECT gridlist)
{
	HINT32		index = 0;
	LINKED_SURFACE_OBJECT	new_linkedSurfaceObj = NULL;

	for (index = 0; index < gridlist->surfaceNumber; index++)
	{
		if (index == 0)
		{
			gridlist->headLinkedSurfaceObj = local_grid_AllocNewLinkedSurfaceObj();
			if (gridlist->headLinkedSurfaceObj == NULL)
			{
				ONDK_Error("[%s:%d] obj GWM_RET_OUT_OF_MEMORY\n", __FUNCTION__, __LINE__);
				local_grid_defaultDestroyGridList(gridlist);
				return GWM_RET_OUT_OF_MEMORY;
			}
			gridlist->headLinkedSurfaceObj->surfaceHandle = ONDK_SURFACE_Create(gridlist->surfaceWidth, gridlist->cellHeight, "ARGB8888");
			ONDK_SURFACE_Clear(gridlist->headLinkedSurfaceObj->surfaceHandle);
		}
		else
		{
			new_linkedSurfaceObj = NULL;
			new_linkedSurfaceObj = local_grid_AllocNewLinkedSurfaceObj();
			if (new_linkedSurfaceObj == NULL)
			{
				ONDK_Error("[%s:%d] obj GWM_RET_OUT_OF_MEMORY\n", __FUNCTION__, __LINE__);
				local_grid_defaultDestroyGridList(gridlist);
				return GWM_RET_OUT_OF_MEMORY;
			}
			new_linkedSurfaceObj->surfaceHandle = ONDK_SURFACE_Create(gridlist->surfaceWidth, gridlist->cellHeight, "ARGB8888");
			ONDK_SURFACE_Clear(new_linkedSurfaceObj->surfaceHandle);

			gwm_lnklst_AddItemWithCompareDescend( (LINK_OBJECT*)&(gridlist->headLinkedSurfaceObj), (LINK_OBJECT)new_linkedSurfaceObj, local_grid_always_ReturnOne );
		}
	}

	return ONDK_RET_OK;
}

static void		local_grid_ArrowUp(GRID_LIST_OBJECT gridListObj)
{
	if(gridListObj->currentIndex - gridListObj->maxColCount < 0)
	{
		return;
	}
	gridListObj->currentIndex -= (gridListObj->maxColCount);

	if(gridListObj->currentIndex < gridListObj->startIndex)
	{
	#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
		local_grid_StartAnimationUp(gridListObj);
		return ;
	#else
		gridListObj->startIndex -= gridListObj->maxColCount;
	#endif
	}

	ONDK_GWM_APP_InvalidateRelativeRect(gridListObj->rect.x, gridListObj->rect.y, gridListObj->rect.w, gridListObj->rect.h);
}

static void		local_grid_ArrowDown(GRID_LIST_OBJECT gridListObj)
{
	if((gridListObj->currentIndex)+(gridListObj->maxColCount) > gridListObj->itemCount - 1)
	{
		gridListObj->currentIndex = gridListObj->itemCount - 1;
	}
	else
	{
		gridListObj->currentIndex +=(gridListObj->maxColCount);
	}

	if(gridListObj->currentIndex >= gridListObj->startIndex + gridListObj->maxColCount*gridListObj->maxRowCount)
	{
	#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
		gridListObj->currentIndex -=(gridListObj->maxColCount);
		local_grid_StartAnimationDown(gridListObj);

		return;
	#else
		gridListObj->startIndex += gridListObj->maxColCount;
	#endif
	}
	ONDK_GWM_APP_InvalidateRelativeRect(gridListObj->rect.x, gridListObj->rect.y, gridListObj->rect.w, gridListObj->rect.h);
}

static void		local_grid_ArrowLeft(GRID_LIST_OBJECT gridListObj)
{
	if(gridListObj->currentIndex % gridListObj->maxColCount == 0)
	{
		return;
	}
	(gridListObj->currentIndex)--;

	ONDK_GWM_APP_InvalidateRelativeRect(gridListObj->rect.x, gridListObj->rect.y, gridListObj->rect.w, gridListObj->rect.h);
}

static void		local_grid_ArrowRight(GRID_LIST_OBJECT gridListObj)
{
	if((gridListObj->currentIndex % gridListObj->maxColCount == gridListObj->maxColCount-1) || (gridListObj->currentIndex == gridListObj->itemCount-1))
	{
		return;
	}
	(gridListObj->currentIndex)++;

	ONDK_GWM_APP_InvalidateRelativeRect(gridListObj->rect.x, gridListObj->rect.y, gridListObj->rect.w, gridListObj->rect.h);

}

static HINT32	local_grid_always_ReturnOne(LINK_OBJECT node1, LINK_OBJECT node2)
{						// 링크트 리스트에서 항상 맨 마지막에 추가할 수 있도록 하기위한 함수.
	GWM_assert(node1);
	GWM_assert(node2);
	return	1;
}

static GRID_LIST_OBJECT		local_grid_AllocNewGridListObj(void)
{
	struct tagGridListObject	*pObj = &s_defGridList;

	pObj = (struct tagGridListObject *)ONDK_Malloc(sizeof(struct tagGridListObject));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defGridList, sizeof(struct tagGridListObject));

	return pObj;
}

static LINKED_SURFACE_OBJECT	local_grid_AllocNewLinkedSurfaceObj(void)
{
	struct	tagLinkedSurfaceObject	*pObj = &s_defLinkedSurface;

	pObj = (struct tagLinkedSurfaceObject *)ONDK_Malloc(sizeof(struct tagLinkedSurfaceObject));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defLinkedSurface, sizeof(struct tagLinkedSurfaceObject));

	return pObj;
}

static GRID_LIST_OBJECT		local_grid_GetGridListObject(int id)
{
	ONDK_GWM_GuiObject_t		objlist;
	GRID_LIST_OBJECT			gridObj;

	GWM_assert(g_pstCurrentApp);

	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);
	gridObj = (GRID_LIST_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);

	return gridObj;
}

static  void	local_grid_SetGridListByCurrentIndex(GRID_LIST_OBJECT gridObj, HINT32 index)
{
	HINT32 	disp_num;

	gridObj->currentIndex = index;

	if (index < gridObj->startIndex)
		gridObj->currentIndex = gridObj->startIndex;

	disp_num = gridObj->maxColCount * gridObj->maxRowCount;

	if (index >= gridObj->startIndex + disp_num)
		gridObj->currentIndex = (index - disp_num) + 1;
}

static ONDK_Result_t	local_grid_defaultDestroyGridList(GRID_LIST_OBJECT gridObj)
{
	LINKED_SURFACE_OBJECT		currentSurfaceObj = NULL;
	LINKED_SURFACE_OBJECT		tmpNextSurfaceObj = NULL;

	ONDK_Assert(gridObj);

	currentSurfaceObj = gridObj->headLinkedSurfaceObj;

	while(currentSurfaceObj != NULL)
	{
		if (currentSurfaceObj->surfaceHandle)
		{
			ONDK_SURFACE_Destroy(currentSurfaceObj->surfaceHandle);
			currentSurfaceObj->surfaceHandle = NULL;
		}

		tmpNextSurfaceObj = (LINKED_SURFACE_OBJECT)gwm_lnklst_GetNext((LINK_OBJECT)currentSurfaceObj);
		ONDK_Free(currentSurfaceObj);
		currentSurfaceObj = tmpNextSurfaceObj;
	}

	ONDK_Free(gridObj);

	return ONDK_RET_OK;
}

static ONDK_Result_t	local_grid_defaultDrawGridListMethod(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																			ONDK_GWM_GuiObject_t object, GRID_LIST_OBJECT gridObject, int param1)
{
	HINT32						drawIndex = 0;
	HINT32						itemIndex = 0;
	HINT32						surfacePosition = 0;
	LINKED_SURFACE_OBJECT		currentSurfaceObj = NULL;
	GRID_DrawItem_t				drawItem;
	ONDK_Rect_t					stRect;
	ONDK_Region_t				stRegion;

	if (gridObject->headLinkedSurfaceObj == NULL)
	{
		if (local_grid_CreateSurface(gridObject) != ONDK_RET_OK)
		{
			HxLOG_Error("Failed Create Surface for gridlist!\n");
			return ONDK_RET_FAIL;
		}
	}

	if (gridObject->startIndex < 0)
	{
		return ONDK_RET_FAIL;
	}
	else if (gridObject->startIndex == 0)
	{
		currentSurfaceObj = (LINKED_SURFACE_OBJECT)gwm_lnklst_GetNext((LINK_OBJECT)gridObject->headLinkedSurfaceObj);
	}
	else
	{
		currentSurfaceObj = gridObject->headLinkedSurfaceObj;
	}

	itemIndex = gridObject->startIndex;
	surfacePosition = 0;

	while(currentSurfaceObj != NULL)
	{
		if (itemIndex >= gridObject->itemCount)
		{
			break;
		}
		/* Draw In Surface */
		stRegion.x1 = 0;
		stRegion.x2 = gridObject->surfaceWidth;//(gridObject->cellWidth * gridObject->maxColCount);
		stRegion.y1 = 0;
		stRegion.y2 = gridObject->cellHeight;
		ONDK_SURFACE_ClearRect(currentSurfaceObj->surfaceHandle, stRegion);

		for (drawIndex = 0; drawIndex < gridObject->maxColCount; drawIndex++)
		{
			drawItem.itemRect = ONDK_Rect(gridObject->cellWidth * drawIndex, 0, gridObject->cellWidth, gridObject->cellHeight);
			drawItem.item_index = itemIndex;
			gridObject->itemDrawMethod(gridObject->ID, currentSurfaceObj->surfaceHandle, &drawItem, drawIndex);
			itemIndex++;
			if (itemIndex >= gridObject->itemCount)
			{
				break;
			}
		}

		if(surfacePosition < gridObject->maxRowCount)
		{
			stRect = ONDK_Rect(0, 0, gridObject->surfaceWidth, gridObject->cellHeight);
			ONDK_SURFACE_Blit(currentSurfaceObj->surfaceHandle, screen, stRect, gridObject->rect.x, gridObject->rect.y + (surfacePosition * gridObject->cellHeight));
			surfacePosition++;
		}
		/* Blit Surface to Screen */
		//if (itemIndex <= (gridObject->maxColCount * gridObject->maxRowCount) )
		//{
		//	stRect = ONDK_Rect(0, 0, (gridObject->cellWidth * gridObject->maxColCount), gridObject->cellHeight);
		//	ONDK_SURFACE_BlitWithoutBlend(currentSurfaceObj->surfaceHandle, screen, stRect, gridObject->rect.x, gridObject->rect.y + (surfacePosition * gridObject->cellHeight));
		//	surfacePosition++;
		//}

		currentSurfaceObj = (LINKED_SURFACE_OBJECT)gwm_lnklst_GetNext((LINK_OBJECT)currentSurfaceObj);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	local_grid_defaultGridListProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	GRID_LIST_OBJECT	gridListObj;
	ONDK_Region_t		area;
	ONDK_Rect_t		rect;
	HBOOL			isFrontMost = FALSE;

	ONDK_Assert(object->type == GUIOBJ_GRID);
	gridListObj = (GRID_LIST_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_grid_defaultDestroyGridList(gridListObj);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			isFrontMost = (arg > 0) ? TRUE : FALSE;

			if (object->objDrawMethod != NULL)
			{
				object->objDrawMethod(ONDK_GWM_GetSurface(ONDK_GWM_APP_GetPosition(g_pstCurrentApp)), &area, &rect, object, (GRID_LIST_OBJECT)gridListObj, arg);
			}
			else
			{
				local_grid_defaultDrawGridListMethod(ONDK_GWM_GetSurface(ONDK_GWM_APP_GetPosition(g_pstCurrentApp)), &area, &rect, object, (GRID_LIST_OBJECT)gridListObj, arg);
			}
			return GWM_MESSAGE_BREAK;
		}
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(gridListObj->rect.x, gridListObj->rect.y, gridListObj->rect.w, gridListObj->rect.h);
			memcpy( &(gridListObj->rect), new_rect, sizeof(ONDK_Region_t));
			ONDK_GWM_APP_InvalidateRelativeRect(gridListObj->rect.x, gridListObj->rect.y, gridListObj->rect.w, gridListObj->rect.h);

			return GWM_MESSAGE_BREAK;
		}
		case MT_CHANGE_TEXT:
		{
			return GWM_MESSAGE_PASS;
		}

		case MT_KEYDOWN:
			if(ONDK_GWM_Obj_GetObjectStatus((ONDK_GWM_GuiObject_t)gridListObj) != GUIOBJ_HILIGHTED)
			{
				return GWM_MESSAGE_BREAK;
				break;
			}
		#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
			if (gridListObj->bDoingAni)
			{
				HxLOG_Debug("doing animation update now..\n");
				return GWM_MESSAGE_BREAK;
			}
		#endif
			switch(arg)
			{
				case KEY_ARROWUP:
					local_grid_ArrowUp(gridListObj);
					break;
				case KEY_ARROWDOWN:
					local_grid_ArrowDown(gridListObj);
					break;
				case KEY_ARROWLEFT:
					local_grid_ArrowLeft(gridListObj);
					return GWM_MESSAGE_BREAK;
				case KEY_ARROWRIGHT:
					local_grid_ArrowRight(gridListObj);
					return GWM_MESSAGE_BREAK;
				default:

					break;
			}
			return GWM_MESSAGE_BREAK;

		case MT_GET_FOCUS:
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_GET, (Handle_t)NULL, object->ID, arg, (int)gridListObj, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(gridListObj->rect.x, gridListObj->rect.y, gridListObj->rect.w, gridListObj->rect.h);
			break;

		case MT_LOST_FOCUS:
			gridListObj->focus &= ~GUIOBJ_PUSHED;
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_LOSE, (Handle_t)NULL, object->ID, arg, (int)gridListObj, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(gridListObj->rect.x, gridListObj->rect.y, gridListObj->rect.w, gridListObj->rect.h);
			break;
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
static void		local_grid_StartAnimationUp(GRID_LIST_OBJECT gridObject)
{
	HINT32						drawIndex = 0;
	HINT32						itemIndex = 0;
	LINKED_SURFACE_OBJECT		currentSurfaceObj = NULL;
	GRID_DrawItem_t				drawItem;
	ONDK_Region_t				stRegion;
	HINT32						nBltCnt = 0;//gridObject->maxRowCount + 1;

	currentSurfaceObj = gridObject->headLinkedSurfaceObj;

	// 안보이던 현재 index 기준으로 윗 라인의 item 들을 그리기 위해...
	itemIndex = gridObject->startIndex - gridObject->maxColCount;

	while(nBltCnt < (gridObject->maxRowCount + 1))//currentSurfaceObj != NULL)
	{
		/* Draw In Surface */
		stRegion.x1 = 0;
		stRegion.x2 = gridObject->surfaceWidth;
		stRegion.y1 = 0;
		stRegion.y2 = gridObject->cellHeight;

		ONDK_SURFACE_ClearRect(currentSurfaceObj->surfaceHandle, stRegion);

		for (drawIndex = 0; drawIndex < gridObject->maxColCount; drawIndex++)
		{
			drawItem.itemRect = ONDK_Rect(gridObject->cellWidth * drawIndex, 0, gridObject->cellWidth, gridObject->cellHeight);
			drawItem.item_index = itemIndex;
			gridObject->itemDrawMethod(gridObject->ID, currentSurfaceObj->surfaceHandle, &drawItem, drawIndex);
			itemIndex++;
			if (itemIndex >= gridObject->itemCount)
			{
				break;
			}
		}
		currentSurfaceObj = (LINKED_SURFACE_OBJECT)gwm_lnklst_GetNext((LINK_OBJECT)currentSurfaceObj);
		nBltCnt++;
	}

	gridObject->nAniYPos = 0;
	gridObject->eAniType = eGridListAniType_MOVEUP;
	gridObject->bDoingAni= TRUE;
	gridObject->fnSetAnimationCtl();
}

static void		local_grid_StartAnimationDown(GRID_LIST_OBJECT gridObject)
{
	HINT32						drawIndex = 0;
	HINT32						itemIndex = 0;
	LINKED_SURFACE_OBJECT		currentSurfaceObj = NULL;
	GRID_DrawItem_t				drawItem;
	ONDK_Region_t				stRegion;
	HINT32						nBltCnt = 0;//gridObject->maxRowCount + 1;

	currentSurfaceObj = gridObject->headLinkedSurfaceObj;

	itemIndex = gridObject->startIndex;

	while(nBltCnt < (gridObject->maxRowCount + 1))//currentSurfaceObj != NULL)
	{
		/* Draw In Surface */
		stRegion.x1 = 0;
		stRegion.x2 = gridObject->surfaceWidth;
		stRegion.y1 = 0;
		stRegion.y2 = gridObject->cellHeight;

		ONDK_SURFACE_ClearRect(currentSurfaceObj->surfaceHandle, stRegion);

		for (drawIndex = 0; drawIndex < gridObject->maxColCount; drawIndex++)
		{
			drawItem.itemRect = ONDK_Rect(gridObject->cellWidth * drawIndex, 0, gridObject->cellWidth, gridObject->cellHeight);
			drawItem.item_index = itemIndex;
			gridObject->itemDrawMethod(gridObject->ID, currentSurfaceObj->surfaceHandle, &drawItem, drawIndex);
			itemIndex++;
			if (itemIndex >= gridObject->itemCount)
			{
				break;
			}
		}
		currentSurfaceObj = (LINKED_SURFACE_OBJECT)gwm_lnklst_GetNext((LINK_OBJECT)currentSurfaceObj);
		nBltCnt++;
	}

	gridObject->nAniYPos = 0;
	gridObject->eAniType = eGridListAniType_MOVEDOWN;
	gridObject->bDoingAni= TRUE;
	gridObject->fnSetAnimationCtl();
}
#endif

/******************************************************************************/
#define _________Public_Function____________________________________________
/******************************************************************************/

void	ONDK_GWM_COM_GridList_Init(void)
{
	struct tagGridListObject	*pObj = &s_defGridList;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagGridListObject));

	pObj->ID = -1;
	pObj->type = GUIOBJ_GRID;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_grid_defaultGridListProc;
	pObj->objDrawMethod = NULL;
	pObj->headLinkedSurfaceObj = NULL;
}

ONDK_Result_t	ONDK_GWM_COM_GridList_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title, HINT32 cellWidth, HINT32 cellHeight,
												HINT32 itemCount, HINT32 maxColCount, HINT32 maxRowCount, HINT32 startIndex)
{
	GRID_LIST_OBJECT		new_gridlist;
	ONDK_Result_t			hresult;

	new_gridlist = local_grid_AllocNewGridListObj();
	if (new_gridlist == NULL)
	{
		ONDK_Error("[%s:%d] obj GWM_RET_OUT_OF_MEMORY\n", __FUNCTION__, __LINE__);
		return GWM_RET_OUT_OF_MEMORY;
	}

	HxSTD_MemCopy(&(new_gridlist->rect), rect, sizeof(ONDK_Rect_t));

	new_gridlist->ID 			= id;
	new_gridlist->title 		= title;
	new_gridlist->cellHeight 	= cellHeight;
	new_gridlist->cellWidth 	= cellWidth;
	new_gridlist->itemCount 	= itemCount;
	new_gridlist->maxColCount 	= maxColCount;
	new_gridlist->maxRowCount 	= maxRowCount;
	new_gridlist->startIndex 	= startIndex;
	new_gridlist->surfaceNumber = new_gridlist->maxRowCount + 2;
	new_gridlist->surfaceWidth 	= new_gridlist->cellWidth * new_gridlist->maxColCount;

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	HxSTD_MemCopy(&(new_gridlist->stRectInScreen), rect, sizeof(ONDK_Rect_t));
	new_gridlist->stClearRegion.x1 = rect->x;
	new_gridlist->stClearRegion.y1 = rect->y;
	new_gridlist->stClearRegion.x2 = rect->x + new_gridlist->surfaceWidth - 1;
	new_gridlist->stClearRegion.y2 = rect->y + rect->h - 1;
#endif

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_gridlist);
	//ONDK_GWM_APP_InvalidateRelativeRect(new_gridlist->rect.x, new_gridlist->rect.y,
	//							new_gridlist->rect.w, new_gridlist->rect.h);

	return hresult;
}

ONDK_Result_t	ONDK_GWM_COM_GridList_SetCurrentIndex(HINT32 id, HINT32 nIndex)
{
	GRID_LIST_OBJECT			gridObj;
	gridObj = local_grid_GetGridListObject(id);
	GWMGRID_RETURN_CHECK_OBJ(local_grid_CheckObj(gridObj));

	if (nIndex >= gridObj->itemCount)
		return GWM_RET_OUT_OF_RANGE;
	if (nIndex < 0)
		return GWM_RET_OUT_OF_RANGE;

	local_grid_SetGridListByCurrentIndex(gridObj, nIndex);

	ONDK_GWM_APP_InvalidateRelativeRect(gridObj->rect.x, gridObj->rect.y, gridObj->rect.w, gridObj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_GridList_ResetItemNum(HINT32 id, HINT32 nItemNum)
{
	GRID_LIST_OBJECT			obj;
	obj = (GRID_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return -1;
	if (obj->type != GUIOBJ_GRID)
		return -1;

	obj->itemCount = nItemNum;
	// 다른것도 모두 초기화
	obj->startIndex = 0;
	local_grid_SetGridListByCurrentIndex(obj, 0);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_GridList_SetItemDrawMethod(HINT32 id, GRID_ItemDraw_t itemDrawFunc)
{
	GRID_LIST_OBJECT			gridObj;
	gridObj = local_grid_GetGridListObject(id);
	GWMGRID_RETURN_CHECK_OBJ(local_grid_CheckObj(gridObj));

	gridObj->itemDrawMethod = itemDrawFunc;

	return GWM_RET_OK;
}

HINT32	ONDK_GWM_COM_GridList_GetCurrentIndexByObjID(HINT32 id)
{
	GRID_LIST_OBJECT			obj;
	obj = (GRID_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return -1;
	if (obj->type != GUIOBJ_GRID)
		return -1;

	return obj->currentIndex;
}

HBOOL	ONDK_GWM_COM_GridList_IsFirstColumn(HINT32 id)
{
	GRID_LIST_OBJECT			obj;
	obj = (GRID_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return -1;
	if (obj->type != GUIOBJ_GRID)
		return -1;

	if(obj->currentIndex % obj->maxColCount == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

ONDK_Result_t	ONDK_GWM_COM_GridList_ChangeProperty(HINT32 id, HINT32 cellWidth, HINT32 cellHeight,
												HINT32 itemCount, HINT32 maxColCount, HINT32 maxRowCount, HINT32 startIndex)
{
	GRID_LIST_OBJECT			obj;
	obj = (GRID_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return -1;
	if (obj->type != GUIOBJ_GRID)
		return -1;

	obj->cellHeight	= cellHeight;
	obj->cellWidth 	= cellWidth;
	obj->itemCount 	= itemCount;
	obj->maxColCount	= maxColCount;
	obj->maxRowCount	= maxRowCount;
	obj->startIndex	= startIndex;
	obj->surfaceNumber = obj->maxRowCount + 2;
	obj->surfaceWidth	= obj->cellWidth * obj->maxColCount;

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	obj->stClearRegion.x2 = obj->rect.x + obj->surfaceWidth - 1;
#endif

	return ONDK_RET_OK;
}

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)

ONDK_Result_t	ONDK_GWM_COM_GridList_SetAnimationCtl(HINT32 id, pfnGridListAnimationCtl fnSetAnimationCtl)
{
	GRID_LIST_OBJECT			obj;
	obj = (GRID_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return -1;
	if (obj->type != GUIOBJ_GRID)
		return -1;

	obj->fnSetAnimationCtl = fnSetAnimationCtl;

	return ONDK_RET_OK;
}

HINT32	ONDK_GWM_COM_GridList_UpdateAnimation(HINT32 id)
{
	LINKED_SURFACE_OBJECT		currentSurfaceObj = NULL;
	ONDK_Rect_t					stRect, stRectDrawUpdate;
	ONDKSurfaceHandle			screen = NULL;
	HINT32						nBltCnt = 0;//gridObject->maxRowCount + 1;

	GRID_LIST_OBJECT			gridObject;

	HINT32 		dstYPos = 0;

	gridObject = (GRID_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (gridObject == NULL)
		return -1;
	if (gridObject->type != GUIOBJ_GRID)
		return -1;

	if (gridObject->eAniType == eGridListAniType_NONE)
		return -1;

	screen = ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW);

	ONDK_SURFACE_ClearRect(screen, gridObject->stClearRegion);

	// 이거 어케하지...?
	#if 1
	ONDK_DRAW_FillRectElement(screen, gridObject->stRectInScreen.x, gridObject->stRectInScreen.y,
										gridObject->surfaceWidth, gridObject->stRectInScreen.h, 0xcc000000);
	ONDK_DRAW_FillRectElement(screen, gridObject->stRectInScreen.x, gridObject->stRectInScreen.y,
										gridObject->surfaceWidth, gridObject->stRectInScreen.h, 0x0dffffff);

	#endif
	currentSurfaceObj = gridObject->headLinkedSurfaceObj;

	while(nBltCnt < (gridObject->maxRowCount + 1))
	{
		stRect = ONDK_Rect(0, 0, gridObject->surfaceWidth, gridObject->cellHeight);

		if (gridObject->eAniType == eGridListAniType_MOVEDOWN)		// down 시 animation 이면..
			dstYPos = gridObject->rect.y + ((nBltCnt * gridObject->cellHeight) - gridObject->nAniYPos);
		else if (gridObject->eAniType == eGridListAniType_MOVEUP)	// up 시 animation 이면..
			dstYPos = gridObject->rect.y + ((nBltCnt * gridObject->cellHeight) + gridObject->nAniYPos - gridObject->cellHeight);

		if (dstYPos <= gridObject->stRectInScreen.y)// 그려지는 위치보다 더위로 영역을 벗어나는 경우..
		{
			stRect.y = gridObject->stRectInScreen.y - dstYPos;		// 그려져야 할 영역의 y 를 구하고.
			stRect.h -= (gridObject->stRectInScreen.y - dstYPos);	// 그려져야 할 영역의 h 를 구하고..
			dstYPos = gridObject->stRectInScreen.y;					// screen 의 y 복사위치를 set.
		}

		if (gridObject->cellHeight + dstYPos > gridObject->stClearRegion.y2) // 그려지는 영역이 grid object 의 영역을아래로  벗어나면..
		{
			stRect.h = (gridObject->stClearRegion.y2 - dstYPos); // src 의 h 를 구하고.
		}
		if (stRect.h > 0 && dstYPos > 0)	// dst 영역의 좌표가, screen 의 grid object 범위 내라면 복사.
		{
			ONDK_SURFACE_Blit(currentSurfaceObj->surfaceHandle, screen, stRect, gridObject->rect.x, dstYPos);
		}

		currentSurfaceObj = (LINKED_SURFACE_OBJECT)gwm_lnklst_GetNext((LINK_OBJECT)currentSurfaceObj);

		nBltCnt++;
	}
	stRectDrawUpdate.x = gridObject->stRectInScreen.x;
	stRectDrawUpdate.y = gridObject->stRectInScreen.y;
	stRectDrawUpdate.w = gridObject->surfaceWidth;
	stRectDrawUpdate.h = gridObject->stRectInScreen.h;
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW), &stRectDrawUpdate );

	if (gridObject->eAniType == eGridListAniType_MOVEDOWN)
	{

		if (gridObject->nAniYPos >= gridObject->cellHeight || 	// col surface 의 높이 만큼만 animation 한다.
			(gridObject->nAniYPos + GRIDLIST_ANI_MOVE_INTERVAL) >= gridObject->cellHeight ) // 다음을 한번 구해보고, 넘으면 중단한다..
		{
			gridObject->currentIndex +=(gridObject->maxColCount);
			gridObject->startIndex += gridObject->maxColCount;

			gridObject->eAniType = eGridListAniType_NONE;
			gridObject->bDoingAni= FALSE;
			ONDK_GWM_APP_InvalidateRelativeRect(gridObject->rect.x, gridObject->rect.y, gridObject->rect.w, gridObject->rect.h);
			return -1;
		}
		else
		{
			gridObject->nAniYPos += GRIDLIST_ANI_MOVE_INTERVAL;
			return 0;
		}
	}
	else if (gridObject->eAniType == eGridListAniType_MOVEUP)
	{
		if (gridObject->nAniYPos >= gridObject->cellHeight || (gridObject->nAniYPos + GRIDLIST_ANI_MOVE_INTERVAL) >= gridObject->cellHeight )
		{
			gridObject->startIndex -= gridObject->maxColCount;

			gridObject->eAniType = eGridListAniType_NONE;
			gridObject->bDoingAni= FALSE;
			ONDK_GWM_APP_InvalidateRelativeRect(gridObject->rect.x, gridObject->rect.y, gridObject->rect.w, gridObject->rect.h);
			return -1;
		}
		else
		{
			gridObject->nAniYPos += GRIDLIST_ANI_MOVE_INTERVAL;
			return 0;
		}
	}
	return 0;
}

#endif
