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

#include "../ondk_gwm_objdef.h"
#include <ondk_gwm_appobj.h>
#include "../ondk_gwm_internal.h"

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define	MAX_SELECTOR_OBJECT					64

/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */
struct tagSelectorObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.
	HCHAR				*title;					// Selector 이름.
	HINT32				item_count;				// 전체 선택 가능한 Item의 갯수.
	HINT32				current_index;			// 현재 선택되어 있는 Item의 인덱스.
	HCHAR				**item;					// Item들의 배열.
	HINT32              item_x;
	HINT32				item_w;

	ONDK_Color_t		color_fontcolor;
	ONDK_Color_t		color_over;
	ONDK_Color_t		color_normal;
	ONDK_Color_t		color_click;
	ONDK_Color_t		color_disable;
	ONDK_Color_t		color_shadow;
	ONDK_Color_t		color_hilight;

};

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static struct tagSelectorObject		sSelectorArray[MAX_SELECTOR_OBJECT];
static HCHAR						    isAllocated[MAX_SELECTOR_OBJECT];

/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */

static SELECTOR_OBJECT		local_gwmselector_GetSelectorObject(HINT32 id)
{
	ONDK_GWM_GuiObject_t		objlist;
	SELECTOR_OBJECT	selector;

	GWM_assert(g_pstCurrentApp);

	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);
	selector = (SELECTOR_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);

	return selector;
}


static SELECTOR_OBJECT		local_gwmselector_AllocNewSelectorObj(void)
{
	int		i;

	for (i = 0; i < MAX_SELECTOR_OBJECT; i++)
	{
		if (!isAllocated[i])
			break;
	}
	if (i < MAX_SELECTOR_OBJECT)
	{
		isAllocated[i] = 1;
		return &sSelectorArray[i];
	}
	return NULL;
}

static void					local_gwmselector_FreeSelectorObj(SELECTOR_OBJECT obj)
{
	int		i;

	GWM_assert(obj);
#ifdef CONFIG_DEBUG
	if (obj->link != NULL)
		GWM_DBG_Print(DBGMSG_OBJ, ("[ WARNING ] Object(%d):%08X is currently linked.\n", obj->ID, (int)obj));
#endif
	for (i = 0; i < MAX_SELECTOR_OBJECT; i++)
	{
		if (obj == &(sSelectorArray[i]))
			break;
	}

	if (i < MAX_SELECTOR_OBJECT)
	{
		isAllocated[i] = 0;
		INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(sSelectorArray[i]));
		sSelectorArray[i].title = NULL;					// Selector 이름.
		sSelectorArray[i].item_count = 0;				// 전체 선택 가능한 Item의 갯수.
		sSelectorArray[i].current_index = 0;				// 현재 선택되어 있는 Item의 인덱스.
		sSelectorArray[i].item = NULL;					// Item들의 배열.
		sSelectorArray[i].item_x	= 0;
		sSelectorArray[i].item_w	= 0;
	}
	return;
}


static void			local_gwmselector_defaultDrawSelectorMethod(SELECTOR_OBJECT select, ONDK_Region_t *area, ONDK_Rect_t *rect,
										HCHAR *title, HCHAR *value, HINT32  status, HBOOL isFrontMost)
{
	HINT32		bright = 0, face = 0, shadow = 0, fontcolor = 0, left_more = 0, right_more = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, height = 0;

	ONDKFontHandle	*font = NULL;
	ONDK_Color_t	fontCol = 0, leftCol = 0, rightCol = 0;
	ONDK_Color_t	btnColor = 0;
	ONDKSurfaceHandle 	screen = ONDK_GWM_GetCurSurface();

	fontcolor = select->color_fontcolor;

	if (status & GUIOBJ_HILIGHTED)
	{
		btnColor = select->color_over;
	}
	else if (status & GUIOBJ_DISABLED)
	{
		btnColor = select->color_disable;
	}
	else
	{
		btnColor = select->color_normal;
	}

	// 선택 가능한 화살표 표시
	if (status & SELECTOR_LEFT_NOMORE)
		left_more = select->color_shadow;
	else
		left_more = select->color_hilight;

	if (status & SELECTOR_RIGHT_NOMORE)
		right_more = select->color_shadow;
	else
		right_more = select->color_hilight;

	if (isFrontMost)
	{
		bright 	= ONDK_GWM_GFX_GetSysDimmedColor(screen, btnColor, 150);
		shadow 	= ONDK_GWM_GFX_GetSysDimmedColor(screen, btnColor, 50);
		face 	= btnColor;
		fontCol	= fontcolor;
		leftCol	= left_more;
		rightCol	= right_more;
	}
	else
	{
		bright = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, bright, DEFAULT_DIMMED_LEVEL);
		face = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, face, DEFAULT_DIMMED_LEVEL);
		shadow = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, shadow, DEFAULT_DIMMED_LEVEL);
		leftCol = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, left_more, DEFAULT_DIMMED_LEVEL);
		rightCol = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, right_more, DEFAULT_DIMMED_LEVEL);
	}

	w = rect->w;
	h = rect->h;
	ONDK_GWM_GFX_DrawFrame(screen, area->x1 + rect->x, area->y1 + rect->y, w, h, bright, face, shadow);

	font = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, DEFAULT_OBJ_FONT_SIZE);
    height = ONDK_FONT_GetHeight(font);
	if (font != NULL)
	{
		y = (area->y1 + rect->y) + (h - height) / 2;
		x = area->x1 + rect->x;

		// 일단 제목 표시.
		OSD_GFX_DrawStringWithAbbr(screen, font, x + 12, y, w / 2 - 12, title, fontCol);


		// 선택된 값의 표시.
		x = x + w / 2 + 6;
		w = w / 2 - 12 - 12;				// 12 은 오른쪽 끝 SPACE, 추가된 12는 양쪽 화살표의 크기

		if(value)
		{
			OSD_GFX_DrawStringWithAbbr(screen, font, x, y, w, value, fontCol);
		}
	}
	ONDK_FONT_Release(font);
}


static	ONDK_Result_t	local_gwmselector_defaultSelectorProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	SELECTOR_OBJECT	        selector;
	ONDK_Region_t			area;
	ONDK_Rect_t			    rect;

	GWM_assert(object->type == GUIOBJ_SELECTOR);
	selector = (SELECTOR_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwmselector_FreeSelectorObj(selector);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			if (object->objDrawMethod != NULL)
			{	/* draw with customized drawing method */
				object->objDrawMethod(ONDK_GWM_GetCurSurface(), &area, &rect, object, (SELECTOR_OBJECT)selector, arg);
			}
			else
			{	/* draw with default method */

				char * tmp = NULL;

				if (selector->current_index == 0 )
					selector->focus |= SELECTOR_LEFT_NOMORE;
				else
					selector->focus &= ~SELECTOR_LEFT_NOMORE;

				if (selector->current_index == (selector->item_count-1))
					selector->focus |= SELECTOR_RIGHT_NOMORE;
				else
					selector->focus &= ~SELECTOR_RIGHT_NOMORE;

				if(selector->item != NULL)
				{
					tmp = selector->item[selector->current_index];
				}

				local_gwmselector_defaultDrawSelectorMethod(selector, &area, &rect,
								selector->title, tmp, selector->focus, (arg > 0) ? TRUE : FALSE);
			}
			return GWM_MESSAGE_BREAK;
		}
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
			memcpy( &(selector->rect), new_rect, sizeof(ONDK_Region_t));
			ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_CHANGE_TEXT:
		{
			char 	*new_text;
			new_text = (char *)arg;
			selector->title= new_text;
			ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_SET_VALUE:
		{
			selector->current_index = arg;
			ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_KEYDOWN:
		{
			switch (arg)
			{
				case KEY_ARROWLEFT:
				{
					if (selector->current_index > 0)
					{
						selector->current_index--;
						ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_CHANGED,
									(Handle_t)NULL, object->ID, selector->current_index, (int)selector, NULL);
					}
					else
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_LEFT_NOMORE,
									(Handle_t)NULL, object->ID, selector->item_count, (int)selector, NULL);
					return GWM_MESSAGE_BREAK;
				}

				case KEY_ARROWRIGHT:
				{
					if (selector->current_index < selector->item_count-1)
					{
						selector->current_index++;
						ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_CHANGED,
									(Handle_t)NULL, object->ID, selector->current_index, (int)selector, NULL);
					}
					else
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_RIGHT_NOMORE,
									(Handle_t)NULL, object->ID, selector->item_count, (int)selector, NULL);
					return GWM_MESSAGE_BREAK;
				}

				case KEY_OK:
				{
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_CLICKED,
									(Handle_t)NULL, object->ID, selector->item_count, (int)selector, NULL);
					return GWM_MESSAGE_BREAK;
			}

				default:
					break;
			}
			break;	//MESSAGE_NOTUSED;
		}

		case MT_KEYUP:
			/* return it as used one if this has no releasing effect */
			return GWM_MESSAGE_BREAK;

		case MT_GET_FOCUS:
			ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
			break;
		case MT_LOST_FOCUS:
			ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
			break;
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}


void			gwm_selector_initClass(void)
{
	int		i;

	for (i = 0; i < MAX_SELECTOR_OBJECT; i++)
	{
		sSelectorArray[i].link = NULL;
		VK_memset(&(sSelectorArray[i].rect), 0, sizeof(ONDK_Rect_t));
		sSelectorArray[i].ID = -1;
		sSelectorArray[i].type = GUIOBJ_SELECTOR;
		sSelectorArray[i].focus = GUIOBJ_NORMAL;
		sSelectorArray[i].Proc = local_gwmselector_defaultSelectorProc;
		sSelectorArray[i].objDrawMethod = NULL;

		sSelectorArray[i].title = NULL;
		sSelectorArray[i].item_count = 0;
		sSelectorArray[i].current_index = 0;
		sSelectorArray[i].item = NULL;
		sSelectorArray[i].item_x	= 0;
		sSelectorArray[i].item_w	= 0;

		sSelectorArray[i].color_over	= 0xFF0000C0;
		sSelectorArray[i].color_normal	= 0xFF000080;
		sSelectorArray[i].color_click	= 0xFF0000FF;
		sSelectorArray[i].color_disable	= 0xFF000040;
		sSelectorArray[i].color_fontcolor	= 0xFF820610;

		sSelectorArray[i].color_shadow	=	0xFF8A93A4;
		sSelectorArray[i].color_hilight	=	0xFFCBD7E3;

		isAllocated[i] = 0;
	}
}


/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

void					ONDK_GWM_Selector_GetProperty(SELECTOR_OBJECT object,
													HINT32 *status, HINT32 *total, HINT32 *curr, HCHAR **title)
{
	*status = object->focus;
	*title = object->title;
	*total = object->item_count;
	*curr = object->current_index;
}


ONDK_Result_t			ONDK_GWM_Selector_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title,
													HINT32 item_count, HCHAR **item_array)
{
	SELECTOR_OBJECT	new_slct;
	ONDK_Result_t			hresult;

	new_slct = local_gwmselector_GetSelectorObject(id);
	if (new_slct != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_slct = local_gwmselector_AllocNewSelectorObj();
	if (new_slct == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	new_slct->link = NULL;
	VK_memcpy(&(new_slct->rect), rect, sizeof(ONDK_Rect_t));
	new_slct->ID = id;
	new_slct->type = GUIOBJ_SELECTOR;
	new_slct->focus = GUIOBJ_NORMAL;
	new_slct->Proc = local_gwmselector_defaultSelectorProc;
	new_slct->objDrawMethod = NULL;

	new_slct->title = title;
	new_slct->item_count = item_count;
	new_slct->current_index = 0;
	new_slct->item = item_array;
	new_slct->item_x	= 0;
	new_slct->item_w	= 0;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)new_slct, 0);

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_slct);
	ONDK_GWM_APP_InvalidateRelativeRect(	new_slct->rect.x,
						new_slct->rect.y,
						new_slct->rect.w,
						new_slct->rect.h);

	return hresult;
}


ONDK_Result_t			ONDK_GWM_Selector_GetTotalItem(HINT32 id, HINT32 *number)
{
	SELECTOR_OBJECT	selector;

	selector = local_gwmselector_GetSelectorObject(id);
	if (selector == NULL)
	{
		*number = 0;
		return GWM_RET_NO_OBJECT;
	}
	if (selector->type != GUIOBJ_SELECTOR)
	{
		*number = 0;
		return GWM_RET_NO_OBJECT;
	}

	*number = selector->item_count;
	return GWM_RET_OK;
}


HINT32					ONDK_GWM_Selector_GetCurrentIndex(HINT32 id)
{
	SELECTOR_OBJECT	selector;

	selector = local_gwmselector_GetSelectorObject(id);
	if (selector == NULL)
	{
		return -1;
	}
	if (selector->type != GUIOBJ_SELECTOR)
	{
		return -1;
	}

	return selector->current_index;
}


ONDK_Result_t			ONDK_GWM_Selector_SetCurrentIndex(HINT32 id, HINT32 nIndex)
{
	SELECTOR_OBJECT	selector;

	selector = local_gwmselector_GetSelectorObject(id);
	if (selector == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (selector->type != GUIOBJ_SELECTOR)
	{
		return GWM_RET_NO_OBJECT;
	}

	if ((nIndex < 0) || (nIndex >= selector->item_count))
		selector->current_index = 0;
	else
		selector->current_index = nIndex;

	ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);

	return GWM_RET_OK;
}


HINT32		ONDK_GWM_Selector_IsFocused(SELECTOR_OBJECT selector)
{
	GWM_assert(selector->type == GUIOBJ_SELECTOR);

	return selector->focus;
}


HCHAR *				ONDK_GWM_Selector_GetSelectedItemTextPtr(SELECTOR_OBJECT selector)
{
	GWM_assert(selector->type == GUIOBJ_SELECTOR);
	GWM_assert(selector->current_index < selector->item_count);
	GWM_assert(selector->current_index >= 0);

	return selector->item[selector->current_index];
}


HCHAR *				ONDK_GWM_Selector_GetCurItemTextPtr(SELECTOR_OBJECT selector)
{
	GWM_assert(selector->type == GUIOBJ_SELECTOR);

	return selector->item[selector->current_index];
}


HCHAR *				ONDK_GWM_Selector_GetTitlePtr(SELECTOR_OBJECT selector)
{
	GWM_assert(selector->type == GUIOBJ_SELECTOR);

	return selector->title;
}

HINT32					ONDK_GWM_Selector_GetSelectedIndex(SELECTOR_OBJECT selector)
{
	GWM_assert(selector->type == GUIOBJ_SELECTOR);

	return selector->current_index;
}

ONDK_Result_t 		ONDK_GWM_Selector_ChangeProperty(HINT32 id, HCHAR *title, HINT32 item_count, HCHAR **item_array)
{
	SELECTOR_OBJECT	pSelectorObj;

	pSelectorObj = (SELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pSelectorObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pSelectorObj->type != GUIOBJ_SELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	pSelectorObj->ID = id;
	pSelectorObj->title = title;
	pSelectorObj->item_count = item_count;
	pSelectorObj->item = item_array;

	ONDK_GWM_APP_InvalidateRelativeRect( pSelectorObj->rect.x, pSelectorObj->rect.y, pSelectorObj->rect.w, pSelectorObj->rect.h );

	return	GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_Selector_SetItemDimension (HINT32 id, HINT32 xoffset, HINT32 width)
{
	SELECTOR_OBJECT	pSelectorObj;
	ONDK_Rect_t	rc;

	pSelectorObj = (SELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pSelectorObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pSelectorObj->type != GUIOBJ_SELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	pSelectorObj->item_x	= xoffset;
	pSelectorObj->item_w	= width;

	ONDK_GWM_Obj_GetObjectRect(id, &rc);
	ONDK_GWM_APP_InvalidateRelativeRect(rc.x, rc.y, rc.w, rc.h);

	return	GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_Selector_GetItemDimension (HINT32 id, HINT32 *xoffset, HINT32 *width)
{
	SELECTOR_OBJECT	pSelectorObj;

	if (xoffset == NULL || width == NULL)
	{
		return ERR_FAIL;
	}

	pSelectorObj = (SELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pSelectorObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pSelectorObj->type != GUIOBJ_SELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	*xoffset	= pSelectorObj->item_x;
	*width	= pSelectorObj->item_w;
	return	GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_Selector_SetColor(HINT32 id, ONDK_Color_t over, ONDK_Color_t normal, ONDK_Color_t click,
						ONDK_Color_t disable, ONDK_Color_t shadow, ONDK_Color_t hilight)
{
	SELECTOR_OBJECT	pSelectorObj;

	pSelectorObj = (SELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pSelectorObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pSelectorObj->type != GUIOBJ_SELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	pSelectorObj->color_over = over;
	pSelectorObj->color_normal = normal;
	pSelectorObj->color_click = click;
	pSelectorObj->color_disable = disable;
	pSelectorObj->color_shadow = shadow;
	pSelectorObj->color_hilight = hilight;

	ONDK_GWM_APP_InvalidateRelativeRect(pSelectorObj->rect.x, pSelectorObj->rect.y, pSelectorObj->rect.w, pSelectorObj->rect.h);

	return GWM_RET_OK;

}
/* End of File. ---------------------------------------------------------- */

