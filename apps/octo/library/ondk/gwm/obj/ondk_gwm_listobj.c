/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/

#include <ondk.h>

#include "../ondk_gwm_objdef.h"
#include <ondk_gwm_appobj.h>
#include "../ondk_gwm_internal.h"

#define	MAX_HORLIST_OBJECT		8
#define	LIST_BTN_H				34
#define	LIST_BTN_W				100
#define	NO_ENABLE_ITEM		-1
#define	NO_MORE_MOVE		-2

/**********************************************************************************************************/
#define ____STATIC_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

struct tagListObject
{
	GUIOBJ_BASIC_PROPERTY						// 반드시 맨 앞에 존재하도록 할 것.
	HCHAR				*title;					// List 이름.
	HINT32				cell_height;
	HINT32				cell_width;
	HINT32				item_count;				// 전체 선택 가능한 Item의 갯수.
	HINT32				page_count;				// number of items in 1 page
	HINT32				start_index;				// 리스트 표시의 첫번째.
	HINT32				current_index;			// 현재 커서가 있는 Item의 인덱스.
	HINT32				selected_index;			// 현재 선택되어 있는 Item의 인덱스.
	HULONG				item_addr;					// Item array를 가리키는 address 값
	GWM_GetItem_t		get_item_proc;			// Item에서 string을 얻어 내는 procedure.
	HUINT8 *			bDisabled;				// 각 리스트 항목의 비활성화 프로퍼티
	HUINT8 				bCircularList;					// default circularlist (1)
	HUINT32				invalidate_offset_y;			// key 처리시 invalidate 영역 offset, inflate offset value  ( UNSIGNED !! )
	HCHAR				bQuickDraw;				// Quick draw 여부.
	ONDK_Rect_t			firstCell;				// 첫번째 list Rect!
	ONDK_Rect_t			scrollRect;				// scroll rect
	HINT32				line_interval;			// list간 line 간격.
	HINT32				textOffset_x;
	HINT32				textOffset_y;
	HINT32				cursorOffset;
	ONDK_Rect_t			invalidRect;				// scroll rect

	ONDK_Color_t		color_text;
	ONDK_Color_t		color_focusText;
	ONDK_Color_t		color_disableText;
	ONDK_Color_t		color_cursor;

	ONDKFontHandle		*hFontHandle;

	HBOOL				cursorEnable;			//cursor을 dafault로 사용할 것인지 결정
};

STATIC struct	tagListObject			s_defListObj;

#if 0

struct tagHorListObject
{
	GUIOBJ_BASIC_PROPERTY						// 반드시 맨 앞에 존재하도록 할 것.
	char					*title;					// List 이름.
	int					cell_width;
	int					item_count;				// 전체 선택 가능한 Item의 갯수.
	int					page_count;				// number of items in 1 page
	int					start_index;				// 리스트 표시의 첫번째.
	int					current_index;			// 현재 커서가 있는 Item의 인덱스.
	int					selected_index;			// 현재 선택되어 있는 Item의 인덱스.
	unsigned long		item_addr;				// Item array를 가리키는 address 값
	GWM_GetItem_t		get_item_proc;			// Item에서 string을 얻어 내는 procedure.
	unsigned char *		bDisabled;				// 각 리스트 항목의 비활성화 프로퍼티
	unsigned char 		bCircularList;			// default circularlist (1)
	unsigned char			bPageList;				// Start/End 지점에서 좌우 키를 누를 경우 Page/Item단위로 화면 전환을 할 것인가. default(item)
	unsigned int		invalidate_offset_y;	// key 처리시 invalidate 영역 offset, inflate offset value  ( UNSIGNED !! )
	char				bQuickDraw;				// Quick draw 여부.
	ONDK_Rect_t			firstCell;				// 첫번째 list Rect!
	ONDK_Rect_t			scrollRect;				// scroll rect
	int					line_interval;			// list간 line 간격.

	ONDK_Color_t		color_focusFace;
	ONDK_Color_t		color_disableshadow;
	ONDK_Color_t		color_hilighttext;
	ONDK_Color_t		color_inactivelight;
	ONDK_Color_t		color_inactive_darkshadow;
	ONDK_Color_t		color_inactive_face;
};

static struct tagHorListObject			sHorListArray[MAX_HORLIST_OBJECT];
static char						isHorAllocated[MAX_HORLIST_OBJECT];
#endif

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC ONDK_Result_t local_gwmlist_defaultGetItemString (unsigned long ulItemArray, int nIndex, char **pszOutputString);


STATIC LIST_OBJECT		local_gwmlist_GetListObject(int id)
{
	ONDK_GWM_GuiObject_t		objlist;
	LIST_OBJECT			list;

	GWM_assert(g_pstCurrentApp);

	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);
	list = (LIST_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);

	return list;
}

STATIC LIST_OBJECT		local_gwmlist_AllocNewListObj(void)
{
	struct tagListObject	*pObj = &s_defListObj;

	pObj = (struct tagListObject *)ONDK_Malloc(sizeof(struct tagListObject));
	ONDK_Assert(pObj);
	if (pObj == NULL)
		return NULL;

	HxSTD_MemCopy(pObj, &s_defListObj, sizeof(struct tagListObject));

	return pObj;
}


STATIC void				local_gwmlist_FreeListObj(LIST_OBJECT obj)
{
	ONDK_Assert(obj);

	if(obj->hFontHandle != NULL)
	{
		ONDK_FONT_Release (obj->hFontHandle);
	}

	ONDK_Free(obj);
}

/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC void	local_gwmlist_DrawFirstItemCell(ONDKSurfaceHandle screen, int x, int y, int w, int h, char *text, int facecolor, int fontcolor, int shadow)
{
	ONDK_DRAW_FillRoundRect(screen, x+10, y+10, x+10, y+10, 10, facecolor);
	ONDK_DRAW_FillRectElement(screen, x, y + 10, x + 10, y + h - 1, facecolor);
	ONDK_DRAW_FillRectElement(screen, x + 10, y, x + w - 1, y + h - 3, facecolor);
	ONDK_DRAW_FillRectElement(screen, x, y + h - 2, x + w - 1, y + h - 1, shadow);
}


STATIC void	local_gwmlist_DrawMiddleItemCell(ONDKSurfaceHandle screen, int x, int y, int w, int h, char *text, int facecolor, int fontcolor, int shadow)
{
	ONDK_DRAW_FillRectElement(screen, x, y, x + w - 1, y + h - 1, facecolor);
	ONDK_DRAW_FillRectElement(screen, x, y + h - 2, x + w - 1, y + h - 1, shadow);
}


STATIC void	local_gwmlist_DrawLastItemCell(ONDKSurfaceHandle screen, int x, int y, int w, int h, char *text, int facecolor, int fontcolor)
{
	ONDK_DRAW_FillRectElement(screen, x, y, x + 10, y + h - 11, facecolor);
	ONDK_DRAW_FillRoundRect(screen, x+10, y+h-11, x, y+h, 10, facecolor);
	ONDK_DRAW_FillRectElement(screen, x + 10, y, x + w - 1, y + h - 1, facecolor);
}

STATIC void	local_gwmlist_DrawListScrollbar(ONDKSurfaceHandle screen, LIST_OBJECT list, int x, int y, int w, int h, int total, int start, int range)
{
#if 0
	int		ww;

	x = x + w - 28;			ww = 26;
	ONDK_DRAW_FillRectElement(screen, x, y, x + 4, y + h - 1, list->color_inactive_darkshadow);
	x += 2;
	ONDK_DRAW_FillRectElement(screen, x, y, x + ww - 11, y + 10, list->color_inactive_face);
	ONDK_DRAW_FillRoundRect(screen, x+ww-10, y+10, x+ww, y, 10, list->color_inactive_face);
	ONDK_DRAW_FillRectElement(screen, x, y + 10, x + ww - 1, y + h - 10, list->color_inactive_face);
	ONDK_DRAW_FillRectElement(screen, x, y + h - 11, x + ww - 11, y + h - 1, list->color_inactive_face);
	ONDK_DRAW_FillRoundRect(screen, x+ww-10, y+h-11, x+ww, y+h, 10, list->color_inactive_face);
#endif
}

STATIC ONDK_Result_t	local_gwmlist_DefaultMethod_Destory(LIST_OBJECT	list, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(list);
	ONDK_UNUSED_ARGUMENT(list);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&list->rect);

	local_gwmlist_FreeListObj(list);

	return ONDK_MESSAGE_BREAK;
}

STATIC  void	local_gwmlist_SetListByCurrentIndex(LIST_OBJECT list, HINT32 index)
{
	HINT32 	disp_num;

	list->current_index = index;

	if (index < list->start_index)
		list->start_index = index;

	disp_num = list->page_count;

	if (index >= list->start_index + disp_num)
		list->start_index = (index - disp_num) + 1;
}


STATIC void local_gwmlist_DrawCurListQuickly(LIST_OBJECT pList, HINT32 oldCurIndex, HINT32 curIndex)
{
	ONDK_Rect_t	ivdRectOld, ivdRectCur;

	/* Invalidate Scroll. */
	ONDK_GWM_APP_InvalidateRelativeRect(pList->scrollRect.x, pList->scrollRect.y, pList->scrollRect.w, pList->scrollRect.h);

	oldCurIndex -= pList->start_index ;
	curIndex -= pList->start_index;

	VK_memcpy(&ivdRectOld, &pList->firstCell, sizeof(ONDK_Rect_t));
	ivdRectOld.y = pList->firstCell.y + (oldCurIndex * pList->line_interval);
	ONDK_GWM_APP_InvalidateRelativeRect(ivdRectOld.x, ivdRectOld.y, ivdRectOld.w, ivdRectOld.h);

	if ( oldCurIndex == curIndex )
	{
		return;
	}

	VK_memcpy(&ivdRectCur, &pList->firstCell, sizeof(ONDK_Rect_t));
	ivdRectCur.y = pList->firstCell.y + (curIndex * pList->line_interval);
	ONDK_GWM_APP_InvalidateRelativeRect(ivdRectCur.x, ivdRectCur.y, ivdRectCur.w, ivdRectCur.h);
}

ONDK_Result_t	local_gwmlist_SetListInvalidRectObj(LIST_OBJECT list, HINT32 x, HINT32  y, HINT32 w, HINT32 h)
{
	list->invalidRect.x = x;
	list->invalidRect.y = y;
	list->invalidRect.w = w;
	list->invalidRect.h = h;

	return GWM_RET_OK;
}

STATIC ONDK_Result_t local_gwmlist_defaultGetItemString (unsigned long ulItemArray, HINT32 nIndex, HCHAR **pszOutputString)
{
	HCHAR **szItemArray;

	szItemArray = (HCHAR **)ulItemArray;
	if (szItemArray)
	{
		*pszOutputString = szItemArray[nIndex];
		return GWM_RET_OK;
	}

	return GWM_RET_OUT_OF_RANGE;
}

STATIC HINT32	local_gwmlist_GetNextEnableItem(LIST_OBJECT list, int step)
{
	HINT32	cur_idx, find_idx = NO_ENABLE_ITEM;

	for (cur_idx = list->current_index + step; ; cur_idx += step)
	{
		if (cur_idx < 0 ||cur_idx >= list->item_count)
		{
			if (list->bCircularList == 0)
			{
				find_idx = NO_MORE_MOVE;
				break;
			}

			if (cur_idx < 0)	cur_idx = list->item_count - 1;
			else				cur_idx = 0;
		}

		if (list->bDisabled == NULL || (list->bDisabled != NULL && list->bDisabled[cur_idx] == 0))
		{
			find_idx = cur_idx;
			break;
		}
	}
	return find_idx;
}

STATIC ONDK_Result_t	local_gwmlist_listMoveActionObj(LIST_OBJECT list, HINT32 step)
{
	HINT32	find_idx;

	find_idx = local_gwmlist_GetNextEnableItem(list, step);

	if (find_idx != NO_ENABLE_ITEM && find_idx != NO_MORE_MOVE)
	{
		local_gwmlist_SetListByCurrentIndex(list, find_idx);
	}

	ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );

	return GWM_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmlist_listPageUpActionObj(LIST_OBJECT list)
{
	HINT32 old_currentIndex, old_startIndex;
	HINT32 new_currentIndex, new_startIndex;
	HINT32 step = 0;

	if(list->item_count < list->page_count)
	{
		return GWM_MESSAGE_BREAK;
	}

	old_currentIndex = list->current_index;
	old_startIndex = list->start_index;
	step = list->page_count;

	new_startIndex = old_startIndex - step;
	new_currentIndex = old_currentIndex - step;

	if((new_currentIndex < 0) || (new_startIndex < 0))
	{
		new_startIndex = 0;
		new_currentIndex = 0;
	}

	if(old_currentIndex == 0)
	{
		new_startIndex = (list->item_count - step);
		new_currentIndex = (list->item_count -1);
	}

	list->start_index = new_startIndex;
	list->current_index = new_currentIndex;

	ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, (HUINT32)NULL, list->ID, list->current_index, (int)list, NULL);
	ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );

	return GWM_MESSAGE_BREAK;
}


STATIC ONDK_Result_t	local_gwmlist_listPageDownActionObj(LIST_OBJECT list)
{
	int old_currentIndex, old_startIndex;
	int new_currentIndex, new_startIndex;
	int step = 0;

	if(list->item_count < list->page_count)
	{
		return GWM_MESSAGE_BREAK;
	}

	old_currentIndex = list->current_index;
	old_startIndex = list->start_index;
	step = list->page_count;

	new_startIndex = old_startIndex + step;
	new_currentIndex = old_currentIndex + step;

	if((new_currentIndex > (list->item_count - 1)) || (new_startIndex > (list->item_count - step)))
	{
		new_startIndex = (list->item_count - step);
		new_currentIndex = (list->item_count - 1);
	}

	if(old_currentIndex == (list->item_count - 1))
	{
		new_startIndex = 0;
		new_currentIndex = 0;
	}

	list->start_index = new_startIndex;
	list->current_index = new_currentIndex;

	ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, (HUINT32)NULL, list->ID, list->current_index, (int)list, NULL);
	ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );

	return GWM_MESSAGE_BREAK;
}


STATIC ONDK_Result_t	local_gwmlist_DefaultMethod_Draw(LIST_OBJECT list, HINT32 arg)
{
	ONDKSurfaceHandle 		screen = ONDK_GWM_GetCurSurface();
	ONDK_Region_t		area;
	ONDK_Rect_t 		rect;
	ONDKFontHandle		hFontHandle;
	ONDK_Color_t		title_fontColor = 0x00000000;
	HINT32	i = 0;
	HINT32	x1 = 0, y1 = 0;
	HINT32	x = 0, y = 0, w = 0, h = 0;
	HINT32	lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lPageCount = 0, lLastIdx = 0;
	HINT32	lCellHeight = 0, lCursor_y_off = 0, lText_y_off = 0;
	HUINT8	**pList = NULL;
	HUINT8	*pDisable = NULL;
	HBOOL	cursorEnable = TRUE;

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)list, &rect);

	x1 = area.x1 + rect.x;
	y1 = area.y1 + rect.y;
	w = rect.w;
	h = rect.h;

	lTotal = list->item_count;
	lPageCount = list->page_count;
	lStart = list->start_index;
	lCurrent = list->current_index;
	lSelect = list->selected_index;
	lCellHeight = list->cell_height;
	pDisable = list->bDisabled;
	cursorEnable = list->cursorEnable;

	pList = (HUINT8**)list->item_addr;
	if ((pList == NULL) || (lTotal == 0) )
	{
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(list, &hFontHandle);

	if (hFontHandle == NULL)
	{
		GWM_assert(0);
		return	ONDK_RET_FAIL;
	}

	lText_y_off = list->textOffset_y;

	if(lPageCount < lTotal)
	{
		lLastIdx = lPageCount;
	}
	else
	{
		lLastIdx = lTotal;
	}

	for(i = 0; i < lLastIdx; i++)
	{
		if(TRUE == cursorEnable)
		{
			if (i + lStart == lCurrent)
			{
				title_fontColor = list->color_focusText;
				x = x1;
				y = y1 + lCursor_y_off;

				ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - list->cursorOffset, list->color_cursor);
			}
			else
			{
				title_fontColor = list->color_text;
				if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
				{
					title_fontColor = list->color_disableText;
				}
			}
		}
		else
		{
			title_fontColor = list->color_text;
			if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
			{
				title_fontColor = list->color_disableText;
			}
		}

		x = x1 + list->textOffset_x;
		y = y1 + lCursor_y_off + lText_y_off;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)pList[lStart+i], x, y, w, title_fontColor, ONDKSTF_LEFT);
		lCursor_y_off += lCellHeight;
	}

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmlist_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t	lRet = ONDK_RET_OK;

	LIST_OBJECT			list;
	ONDKSurfaceHandle	screen = ONDK_GWM_GetCurSurface();
	ONDK_Assert(screen);

	GWM_assert(object->type == GUIOBJ_LIST);
	list = (LIST_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
			lRet = local_gwmlist_DefaultMethod_Destory((LIST_OBJECT)object, arg);
			break;
		case MT_DRAW:
			if (object->objDrawMethod == NULL)
			{
				lRet = local_gwmlist_DefaultMethod_Draw((LIST_OBJECT)object, arg);
			}
		break;

		case MT_CHANGE_TEXT:
		{
			char 		*new_text;
			new_text = (char *)arg;
			list->title= new_text;
			ONDK_GWM_APP_InvalidateRelativeRect(list->rect.x, list->rect.y, list->rect.w, list->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_SET_VALUE:
		{
			list->selected_index = arg;
			ONDK_GWM_APP_InvalidateRelativeRect(list->rect.x, list->rect.y, list->rect.w, list->rect.h);
			return GWM_MESSAGE_BREAK;
		}

		case MT_KEYDOWN:
		{
			switch (arg)
			{

				case KEY_ARROWUP:
					lRet = local_gwmlist_listMoveActionObj((LIST_OBJECT)object, -1);
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, (HUINT32)NULL, list->ID, list->current_index, (int)list, NULL);
					break;
				case KEY_ARROWDOWN:
					lRet = local_gwmlist_listMoveActionObj((LIST_OBJECT)object, 1);
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, (HUINT32)NULL, list->ID, list->current_index, (int)list, NULL);
					break;
				case KEY_FRONT_OK:
				case KEY_OK:
				{
					list->selected_index = list->current_index;
					ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_SELECTED, (unsigned int)NULL, object->ID, list->selected_index, (int)list, NULL);
					return GWM_MESSAGE_BREAK;
				}
				case KEY_PAGEUP:
					lRet = local_gwmlist_listPageUpActionObj((LIST_OBJECT)object);
					break;
				case KEY_PAGEDOWN:
					lRet = local_gwmlist_listPageDownActionObj((LIST_OBJECT)object);
					break;
				default:
					break;
			}
		}
			break;

		case MT_KEYUP:
			return GWM_MESSAGE_BREAK;

		default:
			break;
	}
	if(ONDK_RET_OK != lRet)
	{
		return lRet;
	}

	return INT_ONDK_GWM_OBJ_BaseDefaultMethod(proc_msg, object, arg);
}


/**********************************************************************************************************/
#define ____INTERNAL_FUNCTIONS_______________________
/**********************************************************************************************************/

void	INT_ONDK_GWM_COM_List_Init(void)
{

	struct tagListObject *pObj = &s_defListObj;
	HxSTD_MemSet(pObj, 0, sizeof(struct tagListObject));

	pObj->link = NULL;
	pObj->ID = -1;
	pObj->type = GUIOBJ_LIST;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwmlist_DefaultMethod;
	pObj->objDrawMethod = NULL;

	pObj->title = NULL;
	pObj->cell_height = LIST_BTN_H;
	pObj->item_count = 0;
	pObj->page_count = 0;
	pObj->start_index = 0;
	pObj->current_index = 0;
	pObj->selected_index = -1;
	pObj->item_addr = (unsigned long)NULL;	// Item 들의 배열을 가리키는 Address, 혹은 특정 Index 등.
	pObj->get_item_proc = NULL;
	pObj->bDisabled = NULL;
	pObj->bCircularList = 1;
	pObj->invalidate_offset_y = 0;

	pObj->textOffset_x = 0;
	pObj->textOffset_y = 0;
	pObj->cursorOffset = 0;
	pObj->color_text = 0xFFFFFFFF;
	pObj->color_focusText = 0xFF000000;
	pObj->color_disableText = 0x00000000;
	pObj->color_cursor = 0xff2eb2ff;

	pObj->cursorEnable = TRUE;

}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/
ONDK_Result_t			ONDK_GWM_List_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title, HINT32 cell_height,
												HINT32 item_count, HINT32 page_count, GWM_LIST_ITEMS item_array)
{
	LIST_OBJECT			new_list;
	ONDK_Result_t			hresult;

	new_list = local_gwmlist_GetListObject(id);
	if (new_list != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_list = local_gwmlist_AllocNewListObj();
	if (new_list == NULL)
		return GWM_RET_OUT_OF_MEMORY;


	HxSTD_MemCopy(&(new_list->rect), rect, sizeof(ONDK_Rect_t));

	new_list->ID = id;
	new_list->title = title;
	new_list->cell_height = cell_height;
	new_list->item_count = item_count;
	new_list->page_count = page_count;
	new_list->item_addr = (unsigned long)item_array;

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_list);

	return hresult;
}


void	ONDK_GWM_List_GetProperty(LIST_OBJECT object, HINT32 *out_total, HINT32 *out_start, HINT32 *out_curr, HINT32 *out_select, HINT32 *out_pagecount)
{
	if (object == NULL)
	{
		*out_total = 0;
		*out_start = 0;
		*out_curr = 0;
		*out_select = 0;

		return;
	}

	*out_total = object->item_count;
	*out_start = object->start_index;
	*out_curr = object->current_index;
	*out_select = object->selected_index;
	*out_pagecount = object->page_count;
}

void	ONDK_GWM_List_ChangeGetItemProc (LIST_OBJECT object, GWM_GetItem_t pfGetItemProc)
{
	if (object == NULL)
	{
		return;
	}

	object->get_item_proc = pfGetItemProc;
}

void	ONDK_GWM_List_ChangeItems(LIST_OBJECT object, HINT32 total, GWM_LIST_ITEMS item_array)
{
	HINT32		disp_num;
	HINT32		temp_mg  = 0;

	if (object == NULL)
		return;

	disp_num = object->page_count;
	object->item_count = total;
	object->item_addr = item_array;


	if (total <= (object->start_index + disp_num))
	{
		object->start_index = total - disp_num;
		if (object->start_index < 0)
			object->start_index = 0;
	}
	if (total <= object->current_index)
	{
		object->current_index = total - 1;
		if (object->current_index < 0)
			object->current_index = 0;
	}

	// TODO: 나중에 꼭 수정해야 한다.
	if (object->rect.y >= 60)
	{
		temp_mg = 60;
	}
	ONDK_GWM_APP_InvalidateRelativeRect(object->rect.x, object->rect.y - temp_mg , object->rect.w, object->rect.h);
}


void	ONDK_GWM_List_ChangeNumOfPage (LIST_OBJECT object, HINT32 page_count)
{
	if (object == NULL)
	{
		return;
	}
	if (object->type != GUIOBJ_LIST)
	{
		return;
	}

	object->page_count = page_count;
}

ONDK_Result_t	ONDK_GWM_List_GetTotalItem(HINT32 id, HINT32 *number)
{
	LIST_OBJECT	list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		*number = 0;
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		*number = 0;
		return GWM_RET_NO_OBJECT;
	}

	*number = list->item_count;
	return GWM_RET_OK;
}

HINT32	ONDK_GWM_List_GetCurrentIndex(HINT32 id)
{
	LIST_OBJECT	list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return -1;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return -1;
	}

	return list->current_index;
}


HINT32	ONDK_GWM_List_GetSelectedIndex(HINT32 id)
{
	LIST_OBJECT	list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return -1;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return -1;
	}

	return list->selected_index;
}

HINT32	ONDK_GWM_List_GetStartIndex(HINT32 id)
{
	LIST_OBJECT	list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return -1;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return -1;
	}

	return list->start_index;
}

HINT32	ONDK_GWM_List_GetCellHeight (HINT32 id)
{
	LIST_OBJECT	list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return -1;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return -1;
	}

	return list->cell_height;
}

HINT32	ONDK_GWM_List_GetCellWidth (HINT32 id)
{
	LIST_OBJECT	list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return -1;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return -1;
	}

	return list->cell_width;
}

HINT32	ONDK_GWM_List_SetCellWidth (HINT32 id, HINT32 item_width)
{
	LIST_OBJECT 		list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (item_width < 0)
		return GWM_RET_OUT_OF_RANGE;

	list->cell_width = item_width;

	return GWM_RET_OK;
}


HINT32	ONDK_GWM_List_GetNumOfPage (HINT32 id)
{
	LIST_OBJECT	list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return -1;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return -1;
	}

	return list->page_count;
}



ONDK_Result_t	ONDK_GWM_List_SetCurrentIndex(HINT32 id, HINT32 nIndex)
{
	LIST_OBJECT			list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (nIndex >= list->item_count)
		return GWM_RET_OUT_OF_RANGE;
	if (nIndex < 0)
		return GWM_RET_OUT_OF_RANGE;

	local_gwmlist_SetListByCurrentIndex(list, nIndex);

	ONDK_GWM_APP_InvalidateRelativeRect(	list->rect.x, list->rect.y, list->rect.w, list->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_List_SetSelectedIndex(HINT32 id, HINT32 nIndex)
{
	LIST_OBJECT			list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (nIndex >= list->item_count)
		return GWM_RET_OUT_OF_RANGE;
	if (nIndex < 0)
		return GWM_RET_OUT_OF_RANGE;

	list->selected_index = nIndex;

	ONDK_GWM_APP_InvalidateRelativeRect(	list->rect.x, list->rect.y, list->rect.w, list->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_List_SetStartIndex(HINT32 id, HINT32 nIndex)
{
	LIST_OBJECT			list;
	HINT32					disp_num;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	disp_num = list->page_count;

	if (nIndex >= (list->item_count - disp_num))
		nIndex = list->item_count - disp_num;
	if (nIndex < 0)
		nIndex = 0;

	list->start_index = nIndex;

	ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, (unsigned int)NULL, list->ID, list->current_index, (int)list, NULL);

	ONDK_GWM_APP_InvalidateRelativeRect(list->rect.x, list->rect.y, list->rect.w, list->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_List_SetQuickDraw(HINT32 id, ONDK_Rect_t *pFirstRect, ONDK_Rect_t *pScrollRect, HINT32 interval)
{
	LIST_OBJECT			list;

	list = local_gwmlist_GetListObject(id);
	if ( list == NULL )
	{
		return GWM_RET_NO_OBJECT;
	}

	if ( list->type != GUIOBJ_LIST )
	{
		return GWM_RET_NO_OBJECT;
	}

	if ( pFirstRect == NULL )
	{
		return GWM_RET_SOURCE_NULL;
	}

	list->bQuickDraw = TRUE;
	VK_memcpy(&list->firstCell, pFirstRect, sizeof(ONDK_Rect_t));
	VK_memcpy(&list->scrollRect, pScrollRect, sizeof(ONDK_Rect_t));
	list->line_interval = interval;

	return GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_List_SetTitle(HINT32 id, HCHAR *title)
{
	LIST_OBJECT			list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	list->title	= title;

	ONDK_GWM_APP_InvalidateRelativeRect(	list->rect.x, list->rect.y, list->rect.w, list->rect.h);

	return GWM_RET_OK;
}

HCHAR*	ONDK_GWM_List_GetTitlePtr(LIST_OBJECT list)
{
	if(list == NULL)
		return (char*)NULL;

	GWM_assert(list->type == GUIOBJ_LIST);

	return list->title;
}

GWM_LIST_ITEMS			ONDK_GWM_List_GetArrayPtr(LIST_OBJECT list)
{
	if (list == NULL)
		return (GWM_LIST_ITEMS)NULL;

	GWM_assert(list->type == GUIOBJ_LIST);

	return list->item_addr;
}

ONDK_Result_t	ONDK_GWM_List_GetFontHandleObj(LIST_OBJECT object, ONDKFontHandle *handle)
{
	if (object == NULL)
	{
		*handle = NULL;

		return GWM_RET_FAIL;
	}

	if(object->hFontHandle != NULL)
	{
		*handle = object->hFontHandle;
	}
	else
	{
		*handle = NULL;
	}
	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_List_SetItemDisable(HINT32 id, HCHAR *bDisabled)
{
	LIST_OBJECT			list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	list->bDisabled = (unsigned char *)bDisabled;

	ONDK_GWM_APP_InvalidateRelativeRect(	list->rect.x, list->rect.y, list->rect.w, list->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_List_SetCircularMove(HINT32 id, unsigned char bCircularList)
{
	LIST_OBJECT			list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	list->bCircularList = bCircularList;
	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_List_SetOffset(HINT32	id, HINT32	textOffset_X, HINT32	textOffset_Y, HINT32	CursorOffset)
{
	LIST_OBJECT			list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	list->textOffset_x = textOffset_X;
	list->textOffset_y = textOffset_Y;
	list->cursorOffset = CursorOffset;

	return GWM_RET_OK;
}

ONDK_Result_t			ONDK_GWM_List_SetInvalidateOffset(HINT32 id, HUINT32 offset_y)
{
	LIST_OBJECT			list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	list->invalidate_offset_y = offset_y;
	return GWM_RET_OK;
}

ONDK_Result_t			ONDK_GWM_List_GetNextEnabledItem (HINT32 nListObjId, HINT32 step, HINT32 *pnNextItem)
{
	LIST_OBJECT		 pstList;
	HINT32				 nIndex;


	if (pnNextItem == NULL)			{ return GWM_RET_TARGET_NULL; }

	pstList = (LIST_OBJECT)ONDK_GWM_APP_GetObject (nListObjId);
	if (pstList == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (pstList->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	nIndex = local_gwmlist_GetNextEnableItem (pstList, step);
	if (nIndex == NO_MORE_MOVE)
	{
		return GWT_RET_LIST_NOMORE_MOVE;
	}
	else if (nIndex == NO_ENABLE_ITEM)
	{
		return GWT_RET_LIST_NOENABLE_ITEM;
	}

	*pnNextItem = nIndex;
	return GWM_RET_OK;
}

HUINT32				ONDK_GWM_List_GetStatus(LIST_OBJECT list)
{
	GWM_assert(list->type == GUIOBJ_LIST);
	return list->focus;
}

unsigned char*		ONDK_GWM_List_GetItemDisable(LIST_OBJECT list)
{
	GWM_assert(list->type == GUIOBJ_LIST);
	return list->bDisabled;
}

void ONDK_GWM_List_FreeList (HINT32 nObjId)
{
	LIST_OBJECT		 pstListObj;

	pstListObj = (LIST_OBJECT)ONDK_GWM_APP_GetObject (nObjId);
	if (pstListObj)
	{
		local_gwmlist_FreeListObj (pstListObj);
	}
}

/* local_gwmlist_defaultListProc() MT_KEYDOWN KEY_ARROWDOWN와 동일 */
void		ONDK_GWM_List_ScrollDown(HINT32 nObjId)
{
	LIST_OBJECT		list;

	HINT32	find_idx;
	HINT32	old_currentIndex, old_startIndex;

	list = (LIST_OBJECT)ONDK_GWM_APP_GetObject(nObjId);
	if (list == NULL)
	{
		return;
	}

	old_currentIndex = list->current_index;
	old_startIndex = list->start_index;

	find_idx = local_gwmlist_GetNextEnableItem(list, 1);
	if (find_idx != NO_ENABLE_ITEM && find_idx != NO_MORE_MOVE)
	{	//list->current_index = find_idx;
		local_gwmlist_SetListByCurrentIndex(list, find_idx);
	}
	else
	{
		if(find_idx == NO_MORE_MOVE)
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_DOWN_NOMORE, (unsigned int)NULL, nObjId, list->current_index, (int)list, NULL);
		}
		return;
	}
	ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, (unsigned int)NULL, nObjId, list->current_index, (int)list, NULL);

	if ( (old_startIndex != list->start_index) || (list->bQuickDraw == FALSE)  )
	{
		ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );
	}
	else
	{
		local_gwmlist_DrawCurListQuickly(list, old_currentIndex, list->current_index);
	}
}

/* local_gwmlist_defaultListProc() MT_KEYDOWN KEY_ARROWUP와 동일 */
void		ONDK_GWM_List_ScrollUp(HINT32 nObjId)
{
	LIST_OBJECT		list;

	HINT32	find_idx;
	HINT32	old_currentIndex, old_startIndex;

	list = (LIST_OBJECT)ONDK_GWM_APP_GetObject(nObjId);
	if (list == NULL)
	{
		return;
	}

	old_currentIndex = list->current_index;
	old_startIndex = list->start_index;

	find_idx = local_gwmlist_GetNextEnableItem(list, -1);
	if (find_idx != NO_ENABLE_ITEM && find_idx != NO_MORE_MOVE)
	{	//list->current_index = find_idx;
		local_gwmlist_SetListByCurrentIndex(list, find_idx);
	}
	else
	{
		if (find_idx == NO_MORE_MOVE)
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_UP_NOMORE, (unsigned int)NULL, nObjId, list->current_index, (HINT32)list, NULL);
		}

		return;
	}

	ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, (unsigned int)NULL, nObjId, list->current_index, (HINT32)list, NULL);

	if ( (old_startIndex != list->start_index) || (list->bQuickDraw == FALSE)  )
	{
		ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );
	}
	else
	{
		local_gwmlist_DrawCurListQuickly(list, old_currentIndex, list->current_index);
	}
}

/* local_gwmlist_defaultListProc() MT_KEYDOWN KEY_PAGEUP와 동일 */
void		ONDK_GWM_List_PageUpPage(HINT32 nObjId)
{
	LIST_OBJECT		list;

	HINT32 old_currentIndex, old_startIndex;
	HINT32 new_currentIndex, new_startIndex;
	HINT32 step = 0;

	list = (LIST_OBJECT)ONDK_GWM_APP_GetObject(nObjId);
	if (list == NULL)
	{
		return;
	}

	if(list->item_count < list->page_count)
	{
		return;
	}

	old_currentIndex = list->current_index;
	old_startIndex = list->start_index;
	step = list->page_count;

	new_startIndex = old_startIndex - step;
	new_currentIndex = old_currentIndex - step;

	if((new_currentIndex < 0) || (new_startIndex < 0))
	{
		new_startIndex = 0;
		new_currentIndex = 0;
	}

	if(old_currentIndex == 0)
	{
		new_startIndex = (list->item_count - step);
		new_currentIndex = (list->item_count -1);
	}

	list->start_index = new_startIndex;
	list->current_index = new_currentIndex;

	ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, (unsigned int)NULL, nObjId, list->current_index, (int)list, NULL);
	ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );
}

/* local_gwmlist_defaultListProc() MT_KEYDOWN KEY_PAGEDOWN와 동일 */
void		ONDK_GWM_List_PageDownPage(HINT32 nObjId)
{
	LIST_OBJECT		list;

	HINT32 old_currentIndex, old_startIndex;
	HINT32 new_currentIndex, new_startIndex;
	HINT32 step = 0;

	list = (LIST_OBJECT)ONDK_GWM_APP_GetObject(nObjId);
	if (list == NULL)
	{
		return;
	}

	if(list->item_count < list->page_count)
	{
		return;
	}

	old_currentIndex = list->current_index;
	old_startIndex = list->start_index;
	step = list->page_count;

	new_startIndex = old_startIndex + step;
	new_currentIndex = old_currentIndex + step;

	if((new_currentIndex > (list->item_count - 1)) || (new_startIndex > (list->item_count - step)))
	{
		new_startIndex = (list->item_count - step);
		new_currentIndex = (list->item_count - 1);
	}

	if(old_currentIndex == (list->item_count - 1))
	{
		new_startIndex = 0;
		new_currentIndex = 0;
	}

	list->start_index = new_startIndex;
	list->current_index = new_currentIndex;

	ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, (unsigned int)NULL, nObjId, list->current_index, (int)list, NULL);
	ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );
}


ONDK_Result_t	ONDK_GWM_List_SetColorById (HINT32 Id, ONDK_Color_t textColor, ONDK_Color_t focusTextColor,
	ONDK_Color_t disableTextColor, ONDK_Color_t cursorColor)
{
	LIST_OBJECT		list;

	list = (LIST_OBJECT)ONDK_GWM_APP_GetObject(Id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}


	list->color_text = textColor;
	list->color_focusText = focusTextColor;
	list->color_disableText = disableTextColor;
	list->color_cursor = cursorColor;
	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_List_SetFontHandle(HINT32 Id, ONDK_FontId_t fontId, HINT32 fontSize)
{
	LIST_OBJECT		list;
	ONDKFontHandle	*handle;
	list = (LIST_OBJECT)ONDK_GWM_APP_GetObject(Id);
	if (list == NULL)
	{
		return GWM_RET_FAIL;
	}

	if(list->hFontHandle != NULL)
	{
		ONDK_FONT_Release (list->hFontHandle);

		handle = ONDK_FONT_CreateSystemFont(fontId, fontSize);
	}
	else
	{
		handle = ONDK_FONT_CreateSystemFont(fontId, fontSize);
	}

	if(handle == NULL)
	{
		return GWM_RET_FAIL;
	}
	else
	{
		list->hFontHandle = handle;
	}

	return GWM_RET_OK;

}

ONDK_Result_t	ONDK_GWM_List_SetCursorEnable(HINT32 Id, HBOOL enable)
{
	LIST_OBJECT		list;

	list = (LIST_OBJECT)ONDK_GWM_APP_GetObject(Id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	list->cursorEnable = enable;

	return GWM_RET_OK;
}


HINT32	ONDK_GWM_List_GetLineInterval (HINT32 id)
{
	LIST_OBJECT	list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return -1;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return -1;
	}

	return list->line_interval;
}

ONDK_Result_t	ONDK_GWM_List_SetLineInterval (HINT32 id, HINT32 lLineInterval)
{
	LIST_OBJECT 		list;

	list = local_gwmlist_GetListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (lLineInterval < 0)
		return GWM_RET_OUT_OF_RANGE;

	list->line_interval = lLineInterval;

	return GWM_RET_OK;
}


#if 0		// legacy
ONDK_Result_t	ListObjectCheckedDraw(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{	LIST_OBJECT		list;
	int				i;
	int				x, y, w, h, ww;
	ONDK_Region_t		area;
	int				total, start, curr, focus, select;
	char			**item;
	unsigned char	index_max, facecolor, fontcolor;
	unsigned char *bDisabled;

	GWM_assert(proc_msg==MT_DRAW);
	GWM_assert(object->type==GUIOBJ_LIST);

	list = (LIST_OBJECT)object;

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	x = area.x1+list->rect.x;		y = area.y1+list->rect.y;			w = list->rect.w;			h = list->rect.h;
	ww = w - 28;		// Scroller 의 넓이 +SPACE(2)
	total = list->item_count;
	start = list->start_index;
	curr = list->current_index;
	select = list->selected_index;
	focus = list->focus;
	item = list->item;
	bDisabled = list->bDisabled;

	GWM_assert(h>LIST_BTN_H*2);

	index_max = (h/LIST_BTN_H);

	if (focus)	//==GUIOBJ_HILIGHTED)
	{	facecolor = g_stSystemColor.focus_face;
		fontcolor = g_stSystemColor.hilighted_text;
	}
	else //if (focus==GUIOBJ_DISABLED)
	{	facecolor = g_stSystemColor.inactive_light;
		fontcolor = g_stSystemColor.hilighted_text;
	}

	if (curr == start)		// 첫번째 셀이 포커스
	{	local_gwmlist_DrawFirstItemCell(x, y, ww, LIST_BTN_H, item[start], facecolor, fontcolor);

		if(bDisabled != NULL && bDisabled[start] != 0)
			ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+6, item[start], g_stSystemColor.disabled_shadow, ONDKSTF_LEFT);
		else
			ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+6, item[start], fontcolor, ONDKSTF_LEFT);
	}else
	{	local_gwmlist_DrawFirstItemCell(x, y, ww, LIST_BTN_H, item[start], g_stSystemColor.inactive_light, g_stSystemColor.hilighted_text);

		if(bDisabled != NULL && bDisabled[start] != 0)
			ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+6, item[start], g_stSystemColor.disabled_shadow, ONDKSTF_LEFT);
		else
			ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+6, item[start], g_stSystemColor.hilighted_text, ONDKSTF_LEFT);
	}
	if (select == start)
	{	GWM_GFX_DrawBWBitmap(ONDK_GWM_GetSurface(), x+16, y+8, 16, 16, check_mark, g_stSystemColor.hilighted_text);
	}

	for (i=1; i<(index_max-1); i++)
	{	if ( i>= (total))
			goto CLOSE_MAIN_MENU;
		if (curr == start+i)
		{	local_gwmlist_DrawMiddleItemCell(x, y+i*LIST_BTN_H, ww, LIST_BTN_H, item[start+i], facecolor, fontcolor);

			if(bDisabled != NULL && bDisabled[start+i] != 0)
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+i*LIST_BTN_H+6, item[start+i], g_stSystemColor.disabled_shadow, ONDKSTF_LEFT);
			else
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+i*LIST_BTN_H+6, item[start+i], fontcolor, ONDKSTF_LEFT);
		}else
		{	local_gwmlist_DrawMiddleItemCell(x, y+i*LIST_BTN_H, ww, LIST_BTN_H, item[start+i], g_stSystemColor.inactive_light, g_stSystemColor.hilighted_text);
			if(bDisabled != NULL && bDisabled[start+i] != 0)
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+i*LIST_BTN_H+6, item[start+i], g_stSystemColor.disabled_shadow, ONDKSTF_LEFT);
			else
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+i*LIST_BTN_H+6, item[start+i], g_stSystemColor.hilighted_text, ONDKSTF_LEFT);
		}
		if (select == start+i)
		{	GWM_GFX_DrawBWBitmap(ONDK_GWM_GetSurface(), x+16, y+i*LIST_BTN_H+8, 16, 16, check_mark, g_stSystemColor.hilighted_text);
		}
	}

	if (i<total)
	{	if (curr == (start+i))		// 마지막째 셀이 포커스
		{	local_gwmlist_DrawLastItemCell(x, y+i*LIST_BTN_H, ww, LIST_BTN_H, item[start+i], facecolor, fontcolor);
			if(bDisabled != NULL && bDisabled[start+i] != 0)
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+i*LIST_BTN_H+6, item[start+i], g_stSystemColor.disabled_shadow, ONDKSTF_LEFT);
			else
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+i*LIST_BTN_H+6, item[start+i], fontcolor, ONDKSTF_LEFT);
		}else
		{	local_gwmlist_DrawLastItemCell(x, y+i*LIST_BTN_H, ww, LIST_BTN_H, item[start+i], g_stSystemColor.inactive_light, g_stSystemColor.hilighted_text);
			if(bDisabled != NULL && bDisabled[start+i] != 0)
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+i*LIST_BTN_H+6, item[start+i], g_stSystemColor.disabled_shadow, ONDKSTF_LEFT);
			else
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), &gSystemFont, x+60, y+i*LIST_BTN_H+6, item[start+i], g_stSystemColor.hilighted_text, ONDKSTF_LEFT);
		}
		if (select == start+i)
		{	GWM_GFX_DrawBWBitmap(ONDK_GWM_GetSurface(), x+16, y+i*LIST_BTN_H+8, 16, 16, check_mark, g_stSystemColor.hilighted_text);
		}
		goto SCROLL_BAR_DRAW;		//return;
	}

CLOSE_MAIN_MENU:
	local_gwmlist_DrawLastItemCell(x, y+(i*LIST_BTN_H), ww, h-(i*LIST_BTN_H), " ", g_stSystemColor.inactive_light, g_stSystemColor.hilighted_text);

SCROLL_BAR_DRAW:
	local_gwmlist_DrawListScrollbar(x, y, w, h, total, start, index_max );
	return GWM_MESSAGE_BREAK;
}
#endif

#if 0
static HOR_LIST_OBJECT		local_gwmlist_AllocNewHorListObj(void)
{
	HINT32		i;

	for (i = 0; i < MAX_HORLIST_OBJECT; i++)
	{
		if (!isHorAllocated[i])
			break;
	}
	if (i < MAX_HORLIST_OBJECT)
	{
		isHorAllocated[i] = 1;
		return &sHorListArray[i];
	}
	return NULL;
}

static void				local_gwmlist_FreeHorListObj(HOR_LIST_OBJECT obj)
{
	HINT32		i;

	GWM_assert(obj);
#ifdef CONFIG_DEBUG
	if (obj->link != NULL)
		GWM_DBG_Print(DBGMSG_OBJ, ("[ WARNING ] Object(%d):%08X is currently linked.\n", obj->ID, (int)obj));
#endif
	for (i = 0; i < MAX_HORLIST_OBJECT; i++)
	{
		if (obj == &(sHorListArray[i]))
			break;
	}

	if (i < MAX_HORLIST_OBJECT)
	{
		isHorAllocated[i] = 0;
		INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(sHorListArray[i]));
		sHorListArray[i].title = NULL;					// List 이름.
		sHorListArray[i].item_count = 0;				// 전체 선택 가능한 Item의 갯수.
		sHorListArray[i].cell_width= LIST_BTN_W;
		sHorListArray[i].page_count = 0;
		sHorListArray[i].start_index = 0;
		sHorListArray[i].current_index = 0;				// 현재 선택되어 있는 Item의 인덱스.
		sHorListArray[i].selected_index = -1;
		sHorListArray[i].item_addr = (unsigned long)NULL;	// Item 들의 배열을 가리키는 Address, 혹은 특정 Index 등.
		sHorListArray[i].get_item_proc = NULL;
		sHorListArray[i].bDisabled = NULL;				// 각 리스트 항목의 비활성화 프로퍼티
		sHorListArray[i].bCircularList = 1;
		sHorListArray[i].bPageList = 0;
		sHorListArray[i].invalidate_offset_y = 0;

	}
	return;
}


static void	local_gwmhorlist_DrawFirstItemCell(ONDKSurfaceHandle screen, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HCHAR *text, HINT32 facecolor, HINT32 fontcolor, HINT32 shadow)
{
	ONDK_DRAW_FillRoundRect(screen, x+10, y+10, x, y, 10, facecolor);
	ONDK_DRAW_FillRectElement(screen, x, y + 10, x + 10, y + h - 1, facecolor);
	ONDK_DRAW_FillRectElement(screen, x + 10, y, x + w - 1, y + h - 3, facecolor);
	ONDK_DRAW_FillRectElement(screen, x, y + h - 2, x + w - 1, y + h - 1, shadow);
}


static void	local_gwmhorlist_DrawMiddleItemCell(ONDKSurfaceHandle screen, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HCHAR *text, HINT32 facecolor, HINT32 fontcolor, HINT32 shadow)
{
	ONDK_DRAW_FillRectElement(screen, x, y, x + w - 1, y + h - 1, facecolor);
	ONDK_DRAW_FillRectElement(screen, x, y + h - 2, x + w - 1, y + h - 1, shadow);
}


static void	local_gwmhorlist_DrawLastItemCell(ONDKSurfaceHandle screen, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HCHAR *text, HINT32 facecolor, HINT32 fontcolor)
{
	ONDK_DRAW_FillRectElement(screen, x, y, x + 10, y + h - 11, facecolor);
	ONDK_DRAW_FillRoundRect(screen, x+10, y+h-11, x, y+h, 10, facecolor);
	ONDK_DRAW_FillRectElement(screen, x + 10, y, x + w - 1, y + h - 1, facecolor);
}

static void			local_gwmlist_DrawDefaultHorListMethod(ONDKSurfaceHandle screen, ONDK_Region_t *area, HOR_LIST_OBJECT list)
{
	HINT32				i;
	HINT32				x, y, w, h;
	HINT32				total, start, curr, focus, select;
	HCHAR				*szStr;
	HUINT32		        item_addr;
	unsigned char		*bDisabled;
	HINT32				 index_max;
	HINT32				 facecolor, fontcolor;
	ONDKFontHandle		*font;
	GWM_GetItem_t	 get_item_proc;
	ONDK_Result_t	 hRet;

	x = area->x1+list->rect.x;
	y = area->y1+list->rect.y;
	w = list->rect.w;
	h = list->rect.h;
	//ww = w - 28;		// Scroller 의 넓이 +SPACE(2)
	total = list->item_count;
	start = list->start_index;
	curr = list->current_index;
	select = list->selected_index;
	focus = list->focus;

	item_addr = list->item_addr;
	get_item_proc = (list->get_item_proc == NULL) ? (GWM_GetItem_t)local_gwmlist_defaultGetItemString : list->get_item_proc;

	bDisabled = list->bDisabled;

	GWM_assert(w > list->cell_width * 2);

	index_max = list->page_count;

	font = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, 22);
	if (font == NULL)
	{
		ONDK_FONT_Release(font);
		GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
		return;
	}

	if (focus)	//==GUIOBJ_HILIGHTED)
	{
		facecolor = list->color_focusFace;
		fontcolor = list->color_hilighttext;
	}
	else //if (focus==GUIOBJ_DISABLED)
	{
		facecolor = list->color_disableshadow;
		fontcolor = list->color_hilighttext;
	}

#if 1
	if (curr == start)		// 첫번째 셀이 포커스
	{
		hRet = get_item_proc (item_addr, start, &szStr);
		if (hRet != GWM_RET_OK)			{ szStr = NULL; }

		local_gwmhorlist_DrawFirstItemCell(screen, x, y, list->cell_width, y, szStr, facecolor, fontcolor, list->color_inactive_darkshadow);
	}
	else
	{
		hRet = get_item_proc (item_addr, start, &szStr);
		if (hRet != GWM_RET_OK)			{ szStr = NULL; }

		local_gwmhorlist_DrawFirstItemCell(screen, x, y, list->cell_width, y, szStr, list->color_inactivelight, list->color_hilighttext,list->color_inactive_darkshadow);
	}

	for (i = 1; i < (index_max - 1); i++)
	{
		hRet = get_item_proc (item_addr, start + i, &szStr);
		if (hRet != GWM_RET_OK)			{ szStr = NULL; }

		if (curr == start + i)
		{
			local_gwmhorlist_DrawMiddleItemCell(screen, x, y + i * list->cell_width, list->cell_width, y, szStr, facecolor, fontcolor,list->color_inactive_darkshadow);

			//if (bDisabled != NULL && bDisabled[start + i] != 0)
			//	ONDK_FONT_DrawString(screen, font, x + 20, y + i * list->cell_width + 6, szStr, g_stSystemColor.disabled_shadow, ONDKSTF_LEFT);
			//else
			//	ONDK_FONT_DrawString(screen, font, x + 20, y + i * list->cell_width + 6, szStr, fontcolor, ONDKSTF_LEFT);
		}
		else
		{
			local_gwmhorlist_DrawMiddleItemCell(screen, x, y + i * list->cell_width, list->cell_width, y, szStr, list->color_inactivelight, list->color_hilighttext,list->color_inactive_darkshadow);

			//if (bDisabled != NULL && bDisabled[start + i] != 0)
			//	ONDK_FONT_DrawString(screen, font, x + 20, y + i * list->cell_width + 6, szStr, g_stSystemColor.disabled_shadow, ONDKSTF_LEFT);
			//else
			//	ONDK_FONT_DrawString(screen, font, x + 20, y + i * list->cell_width + 6, szStr, g_stSystemColor.hilighted_text, ONDKSTF_LEFT);
		}
	}

	if (i < total)
	{
		hRet = get_item_proc (item_addr, start + i, &szStr);
		if (hRet != GWM_RET_OK)			{ szStr = NULL; }

		if (curr == (start + i))		// 마지막째 셀이 포커스
		{
			local_gwmhorlist_DrawLastItemCell(screen, x, y + i * list->cell_width, list->cell_width, y, szStr, facecolor, fontcolor);

			//if (bDisabled != NULL && bDisabled[start + i] != 0)
			//	ONDK_FONT_DrawString(screen, font, x + 20, y + i * list->cell_width + 6, szStr, g_stSystemColor.disabled_shadow, ONDKSTF_LEFT);
			//else
			//	ONDK_FONT_DrawString(screen, font, x + 20, y + i * list->cell_width + 6, szStr, fontcolor, ONDKSTF_LEFT);
		}

		else
		{
			local_gwmhorlist_DrawLastItemCell(screen, x, y + i * list->cell_width, list->cell_width, y, szStr, list->color_inactivelight, list->color_hilighttext);

			//if (bDisabled != NULL && bDisabled[start + i] != 0)
			//	ONDK_FONT_DrawString(screen, font, x + 20, y + i * list->cell_width + 6, szStr, g_stSystemColor.disabled_shadow, ONDKSTF_LEFT);
			//else
			//	ONDK_FONT_DrawString(screen, font, x + 20, y + i * list->cell_width + 6, szStr, g_stSystemColor.hilighted_text, ONDKSTF_LEFT);
		}
		//goto SCROLL_BAR_DRAW;		//return;
	}
//CLOSE_MAIN_MENU:
//	local_gwmlist_DrawLastItemCell(screen, x, y + (i * list->cell_width), ww, h - (i * list->cell_width), " ", g_stSystemColor.inactive_light, g_stSystemColor.hilighted_text);

//SCROLL_BAR_DRAW:
//	local_gwmlist_DrawListScrollbar(screen, x, y, w, h, total, start, index_max);
#endif

	ONDK_FONT_Release(font);
	return;
}


static  void		local_gwmlist_SetHorListByCurrentIndex(HOR_LIST_OBJECT list, HINT32 index)
{
	HINT32 			disp_num;

	list->current_index = index;

	if(list->bPageList)
	{
		if(index < list->start_index || index >= list->start_index + list->page_count)
		{
			list->start_index = (index / list->page_count) * list->page_count;
		}
	}
	else
	{
		if (index < list->start_index)
			list->start_index = index;

#if 0
		disp_num = ((list->rect.w + list->cell_width- 1) / list->cell_width);
#else
			disp_num = list->page_count;
#endif
		if (index >= list->start_index + disp_num)
			list->start_index = (index - disp_num) + 1;
	}
}


static HINT32			local_gwmhorlist_GetNextEnableItem(HOR_LIST_OBJECT list, HINT32 step)
{
	HINT32         cur_idx, find_idx = NO_ENABLE_ITEM;

	for (cur_idx = list->current_index + step; ; cur_idx += step)
	{
		if (cur_idx < 0 ||cur_idx >= list->item_count)
		{
			if (list->bCircularList == 0)
			{
				find_idx = NO_MORE_MOVE;
				break;
			}

			if (cur_idx < 0)    cur_idx = list->item_count - 1;
			else                cur_idx = 0;
		}

		if (list->bDisabled == NULL || (list->bDisabled != NULL && list->bDisabled[cur_idx] == 0))
		{
			find_idx = cur_idx;
			break;
		}
	}
	return find_idx;
}

static ONDK_Result_t	local_gwmlist_defaultHorListProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	HOR_LIST_OBJECT		list;
	ONDK_Region_t		area;
	ONDK_Rect_t		rect;
	ONDKSurfaceHandle	screen = ONDK_GWM_GetCurSurface();
	ONDK_Assert(screen);

	GWM_assert(object->type == GUIOBJ_LIST);
	list = (HOR_LIST_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwmlist_FreeHorListObj(list);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			if (object->objDrawMethod != NULL)
			{	/* draw with customized drawing method */
				object->objDrawMethod(screen, &area, &rect, object, (HOR_LIST_OBJECT)list, arg);
			}
			else
			{	/* draw with default method */
				local_gwmlist_DrawDefaultHorListMethod(screen, &area, (HOR_LIST_OBJECT)object);
			}
			return GWM_MESSAGE_BREAK;
		}
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(list->rect.x, list->rect.y, list->rect.w, list->rect.h);
			VK_memcpy(&(list->rect), new_rect, sizeof(ONDK_Region_t));
			ONDK_GWM_APP_InvalidateRelativeRect(list->rect.x, list->rect.y, list->rect.w, list->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_CHANGE_TEXT:
		{
			char 		*new_text;
			new_text = (char *)arg;
			list->title= new_text;
			ONDK_GWM_APP_InvalidateRelativeRect(list->rect.x, list->rect.y, list->rect.w, list->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_SET_VALUE:
		{
			list->selected_index = arg;
			ONDK_GWM_APP_InvalidateRelativeRect(list->rect.x, list->rect.y, list->rect.w, list->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_KEYDOWN:
		{
			int				index_max;

			index_max = ((list->rect.w + list->cell_width- 1) / list->cell_width);
			switch (arg)
			{
				case KEY_ARROWRIGHT:
				{
					int	find_idx;

					find_idx = local_gwmhorlist_GetNextEnableItem(list, 1);
					if (find_idx != NO_ENABLE_ITEM && find_idx != NO_MORE_MOVE)
					{	//list->current_index = find_idx;
						local_gwmlist_SetHorListByCurrentIndex(list, find_idx);
					}
					else
					{
						if(find_idx == NO_MORE_MOVE)
						{
							ONDK_GWM_APP_Call(g_pstCurrentApp, LIST_RIGHT_NOMORE, (unsigned int)NULL, object->ID, list->current_index, (int)list, NULL);
						}
						return GWM_MESSAGE_BREAK;
					}
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_RIGHT_CHANGED, (unsigned int)NULL, object->ID, list->current_index, (int)list, NULL);
					ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );

					return GWM_MESSAGE_BREAK;
				}


				case KEY_ARROWLEFT:
				{
					int	find_idx;

					find_idx = local_gwmhorlist_GetNextEnableItem(list, -1);
					if (find_idx != NO_ENABLE_ITEM && find_idx != NO_MORE_MOVE)
					{	//list->current_index = find_idx;
						local_gwmlist_SetHorListByCurrentIndex(list, find_idx);
					}
					else
					{
						if(find_idx == NO_MORE_MOVE)
						{
							ONDK_GWM_APP_Call(g_pstCurrentApp, LIST_LEFT_NOMORE, (unsigned int)NULL, object->ID, list->current_index, (int)list, NULL);
						}
						return GWM_MESSAGE_BREAK;
					}
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_LEFT_CHANGED, (unsigned int)NULL, object->ID, list->current_index, (int)list, NULL);
					ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );
					return GWM_MESSAGE_BREAK;
				}

				case KEY_OK:
				{
					list->selected_index = list->current_index;
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_SELECTED, (unsigned int)NULL, object->ID, list->selected_index, (int)list, NULL);
					ONDK_GWM_APP_InvalidateRelativeRect( list->rect.x, list->rect.y, list->rect.w, list->rect.h );
					return GWM_MESSAGE_BREAK;
				}
				default:
					break;
			}

			break;	// prevent fix : missing 'break' statement
		}
		case MT_GET_FOCUS:
			ONDK_GWM_APP_InvalidateRelativeRect(list->rect.x, list->rect.y, list->rect.w, list->rect.h);
			break;
		case MT_LOST_FOCUS:
			ONDK_GWM_APP_InvalidateRelativeRect(list->rect.x, list->rect.y, list->rect.w, list->rect.h);
			break;
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}

void			gwm_horlist_InitClass(void)
{
	HINT32		i;

	for (i = 0; i < MAX_HORLIST_OBJECT; i++)
	{
		sHorListArray[i].link = NULL;
		VK_memset(&(sHorListArray[i].rect), 0, sizeof(ONDK_Rect_t));
		sHorListArray[i].ID = -1;
		sHorListArray[i].type = GUIOBJ_LIST;
		sHorListArray[i].focus = GUIOBJ_NORMAL;
		sHorListArray[i].Proc = local_gwmlist_defaultHorListProc;
		sHorListArray[i].objDrawMethod = NULL;

		sHorListArray[i].title = NULL;
		sHorListArray[i].cell_width = LIST_BTN_W;
		sHorListArray[i].item_count = 0;
		sHorListArray[i].page_count = 0;
		sHorListArray[i].start_index = 0;
		sHorListArray[i].current_index = 0;
		sHorListArray[i].selected_index = -1;
		sHorListArray[i].item_addr = (unsigned long)NULL;	// Item 들의 배열을 가리키는 Address, 혹은 특정 Index 등.
		sHorListArray[i].get_item_proc = NULL;
		sHorListArray[i].bDisabled = NULL;
		sHorListArray[i].bCircularList = 1;
		sHorListArray[i].bPageList = 0;
		sHorListArray[i].invalidate_offset_y = 0;

		sHorListArray[i].color_focusFace = 0xFFDEB921;
		sHorListArray[i].color_disableshadow = 0xFF303549;
		sHorListArray[i].color_hilighttext = 0xFFA4ACC1;
		sHorListArray[i].color_inactivelight = 0xFFC6CDDD;
		sHorListArray[i].color_inactive_darkshadow = 0xFF7D8997;
		sHorListArray[i].color_inactive_face = 0xFFA0ABBD;

		isHorAllocated[i] = 0;

	}
}

static HOR_LIST_OBJECT		local_gwmlist_GetHorListObject(HINT32 id)
{
	ONDK_GWM_GuiObject_t		objlist;
	HOR_LIST_OBJECT			list;

	GWM_assert(g_pstCurrentApp);

	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);
	list = (HOR_LIST_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);

	return list;
}


ONDK_Result_t			ONDK_GWM_Horizontal_List_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title, HINT32 cell_width,
												HINT32 item_count, HINT32 page_count, GWM_LIST_ITEMS item_array)
{
	HOR_LIST_OBJECT			new_list;
	ONDK_Result_t			hresult;

	new_list = local_gwmlist_GetHorListObject(id);
	if (new_list != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_list = local_gwmlist_AllocNewHorListObj();
	if (new_list == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	new_list->link = NULL;
	VK_memcpy(&(new_list->rect), rect, sizeof(ONDK_Rect_t));
	new_list->ID = id;
	new_list->type = GUIOBJ_LIST;
	new_list->focus = GUIOBJ_NORMAL;
	new_list->Proc = local_gwmlist_defaultHorListProc;
	new_list->objDrawMethod = NULL;

	new_list->title = title;
	new_list->cell_width = (cell_width > 0 && cell_width * 2 < rect->w) ? cell_width : LIST_BTN_W;
	new_list->item_count = item_count;
	new_list->page_count = page_count;
	new_list->start_index = 0;
	new_list->current_index = 0;
	new_list->selected_index = -1;
	new_list->item_addr = (unsigned long)item_array;
	new_list->get_item_proc = NULL;
	new_list->bDisabled = NULL;
	new_list->bCircularList = 1;
	new_list->bPageList = 0;
	new_list->invalidate_offset_y = 0;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)new_list, 0);

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_list);
	ONDK_GWM_APP_InvalidateRelativeRect(	new_list->rect.x,
						new_list->rect.y,
						new_list->rect.w,
						new_list->rect.h);

	return hresult;
}



void					ONDK_GWM_HorList_GetProperty(HOR_LIST_OBJECT object, HINT32 *out_total, HINT32 *out_start, HINT32 *out_curr, HINT32 *out_select)
{
	if (object == NULL)
	{
		*out_total = 0;
		*out_start = 0;
		*out_curr = 0;
		*out_select = 0;
		return;
	}
	*out_total = object->item_count;
	*out_start = object->start_index;
	*out_curr = object->current_index;
	*out_select = object->selected_index;
}

GWM_LIST_ITEMS			ONDK_GWM_HorList_GetArrayPtr(HOR_LIST_OBJECT list)
{
	if(list == NULL)
		return (GWM_LIST_ITEMS)NULL;

	GWM_assert(list->type == GUIOBJ_LIST);

	return list->item_addr;
}


ONDK_Result_t			ONDK_GWM_HorList_GetTotalItem(HINT32 id, HINT32 *number)
{
	HOR_LIST_OBJECT	list;

	list = local_gwmlist_GetHorListObject(id);
	if (list == NULL)
	{
		*number = 0;
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		*number = 0;
		return GWM_RET_NO_OBJECT;
	}

	*number = list->item_count;
	return GWM_RET_OK;
}



ONDK_Result_t			ONDK_GWM_HorList_SetCircularMove(HINT32 id, unsigned char bCircularList)
{
	HOR_LIST_OBJECT			list;

	list = local_gwmlist_GetHorListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	list->bCircularList = bCircularList;
	return GWM_RET_OK;
}

ONDK_Result_t			ONDK_GWM_HorList_SetPageMove(HINT32 id, unsigned char bPageList)
{
	HOR_LIST_OBJECT			list;

	list = local_gwmlist_GetHorListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	list->bPageList = bPageList;
	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_HorList_SetCurrentIndex(HINT32 id, HINT32 nIndex)
{
	HOR_LIST_OBJECT			list;

	list = local_gwmlist_GetHorListObject(id);
	if (list == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (nIndex >= list->item_count)
		return GWM_RET_OUT_OF_RANGE;
	if (nIndex < 0)
		return GWM_RET_OUT_OF_RANGE;

	local_gwmlist_SetHorListByCurrentIndex(list, nIndex);

	ONDK_GWM_APP_InvalidateRelativeRect(list->rect.x, list->rect.y, list->rect.w, list->rect.h);

	return GWM_RET_OK;
}


int					ONDK_GWM_HorList_GetCurrentIndex(HINT32 id)
{
	HOR_LIST_OBJECT	list;

	list = local_gwmlist_GetHorListObject(id);
	if (list == NULL)
	{
		return -1;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return -1;
	}

	return list->current_index;
}

int					ONDK_GWM_HorList_GetStartIndex(HINT32 id)
{
	HOR_LIST_OBJECT	list;

	list = local_gwmlist_GetHorListObject(id);
	if (list == NULL)
	{
		return -1;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return -1;
	}

	return list->start_index;
}

void					ONDK_GWM_HorList_ChangeItems(HOR_LIST_OBJECT object, HINT32 total, GWM_LIST_ITEMS item_array)
{
	if (object == NULL)
		return;

	object->item_count = total;
	object->item_addr = item_array;
	object->start_index = 0;
	object->current_index = 0;

	ONDK_GWM_APP_InvalidateRelativeRect(	object->rect.x, object->rect.y, object->rect.w, object->rect.h);
}

HINT32					ONDK_GWM_HorList_GetNumOfPage (HINT32 id)
{
	HOR_LIST_OBJECT	list;

	list = local_gwmlist_GetHorListObject(id);
	if (list == NULL)
	{
		return -1;
	}
	if (list->type != GUIOBJ_LIST)
	{
		return -1;
	}

	return list->page_count;
}

ONDK_Result_t			ONDK_GWM_HorList_SetColorById (HINT32 Id, ONDK_Color_t focus_face, ONDK_Color_t disabled_shadow, ONDK_Color_t hilighted_text,
										ONDK_Color_t inactive_light, ONDK_Color_t inactive_darkshadow, ONDK_Color_t inactive_face)
{
	HOR_LIST_OBJECT		list;

	list = (HOR_LIST_OBJECT)ONDK_GWM_APP_GetObject(Id);
	if (list == NULL)
	{
		return GWM_RET_FAIL;
	}

	list->color_focusFace = focus_face;
	list->color_disableshadow = disabled_shadow;
	list->color_hilighttext = hilighted_text;
	list->color_inactivelight = inactive_light;
	list->color_inactive_darkshadow = inactive_darkshadow;
	list->color_inactive_face = inactive_face;

	return GWM_RET_OK;
}
#endif

//====================================================================================//

/* End of File. ---------------------------------------------------------- */

