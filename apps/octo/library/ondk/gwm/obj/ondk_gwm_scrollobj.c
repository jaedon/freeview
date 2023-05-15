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
#define	MAX_SCROLLER_OBJECT						64

/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */
struct tagScrollerObject
{	GUIOBJ_BASIC_PROPERTY							// 반드시 맨 앞에 존재하도록 할 것.
	HINT32                  flag;					// 방향지시 화살표의 Hilight 를 위한 것.
	HINT32					range, start, size;
	HINT32					horizontal;				// Horiz or Vert.

	ONDK_Color_t			color_inactiveFace;
	ONDK_Color_t			color_disabled_shadow;
};

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static struct tagScrollerObject		sScrollerArray[MAX_SCROLLER_OBJECT];
static HCHAR						    isAllocated[MAX_SCROLLER_OBJECT];

/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */

static SCROLLER_OBJECT	local_gwmscroll_AllocNewScrollerObj(void)
{
	HINT32		i;

	for (i = 0; i < MAX_SCROLLER_OBJECT; i++)
	{
		if (!isAllocated[i])
			break;
	}
	if (i < MAX_SCROLLER_OBJECT)
	{
		isAllocated[i] = 1;
		return &sScrollerArray[i];
	}
	return NULL;
}

static void				local_gwmscroll_FreeScrollerObj(SCROLLER_OBJECT obj)
{
	HINT32		i;

	GWM_assert(obj);
#ifdef CONFIG_DEBUG
	if (obj->link != NULL)
		GWM_DBG_Print(DBGMSG_OBJ, ("[ WARNING ] Object(%d):%08X is currently linked.\n", obj->ID, (int)obj));
#endif
	for (i = 0; i < MAX_SCROLLER_OBJECT; i++)
	{
		if (obj == &(sScrollerArray[i]))
			break;
	}

	if (i < MAX_SCROLLER_OBJECT)
	{
		isAllocated[i] = 0;
		INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(sScrollerArray[i]));
		sScrollerArray[i].flag = 0;
		sScrollerArray[i].range = 0;
		sScrollerArray[i].start = 0;
		sScrollerArray[i].size = 0;
		sScrollerArray[i].horizontal = 0;
	}
	return;
}


static ONDK_Result_t		local_gwmscroll_defaultScrollerProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	SCROLLER_OBJECT	    scrlObj;
	ONDK_Region_t		area;
	ONDK_Rect_t			rect;
	ONDK_Color_t		shadow, inativeFace;
	HINT32				x, y, w, h, position, length;
	ONDKSurfaceHandle	screen = ONDK_GWM_GetCurSurface();
	ONDK_Assert(screen);

	GWM_assert(object->type == GUIOBJ_SCROLLER);
	scrlObj = (SCROLLER_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwmscroll_FreeScrollerObj(scrlObj);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			if (object->objDrawMethod != NULL)
			{	/* draw with customized drawing method */
				object->objDrawMethod(screen, &area, &rect, object, (SCROLLER_OBJECT)scrlObj, arg);
			}
			else
			{	/* draw with default method */
				x = area.x1+scrlObj->rect.x;
				y = area.y1+scrlObj->rect.y;
				w = scrlObj->rect.w;
				h = scrlObj->rect.h;

				shadow = scrlObj->color_disabled_shadow;
				inativeFace = scrlObj->color_inactiveFace;

				if (scrlObj->horizontal)
				{	// 바탕
					ONDK_DRAW_FillRectElement(screen, x, y, x + w - 1, y + 16, shadow);
					// 프로그래스
					w -= 4;
					position = w * scrlObj->start / (scrlObj->range + scrlObj->size);
					length = w * scrlObj->size / scrlObj->range;
					x += (position + 2);

					// TODO: should re-define the colors of default thumb
					ONDK_DRAW_FillRectElement(screen, x, y + 2, x + length, y + 3, 239);
					ONDK_DRAW_FillRectElement(screen, x, y + 4, x + length, y + 5, 245);
					ONDK_DRAW_FillRectElement(screen, x, y + 6, x + length, y + 7, 246);
					ONDK_DRAW_FillRectElement(screen, x, y + 8, x + length, y + 9, 247);
					ONDK_DRAW_FillRectElement(screen, x, y + 10, x + length, y + 11, 248);
					ONDK_DRAW_FillRectElement(screen, x, y + 12, x + length, y + 13, 12);
				}
				else
				{

					ONDK_DRAW_FillRectElement(screen, x, y, x + w - 11, y + 10, inativeFace);
					ONDK_DRAW_FillRoundRect(screen, x+w-10, y+10, x+w, y, 10, inativeFace);
					ONDK_DRAW_FillRectElement(screen, x, y + 10, x + w - 1, y + h - 10, inativeFace);
					ONDK_DRAW_FillRectElement(screen, x, y + h - 10, x + w - 11, y + h - 1, inativeFace);
					ONDK_DRAW_FillRoundRect(screen, x+w-10, y+h-10, x+w, y+h, 10, inativeFace);

				}
			}
			return GWM_MESSAGE_BREAK;
		}
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*rc;

			rc = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(scrlObj->rect.x, scrlObj->rect.y, scrlObj->rect.w, scrlObj->rect.h);
			VK_memcpy(&(scrlObj->rect), rc, sizeof(ONDK_Rect_t));
			ONDK_GWM_APP_InvalidateRelativeRect(scrlObj->rect.x, scrlObj->rect.y, scrlObj->rect.w, scrlObj->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_SET_VALUE:
		{
			scrlObj->start = arg;
			ONDK_GWM_APP_InvalidateRelativeRect(scrlObj->rect.x, scrlObj->rect.y, scrlObj->rect.w, scrlObj->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}


void			gwm_scroll_InitClass(void)
{
	HINT32		i;

	for (i = 0; i < MAX_SCROLLER_OBJECT; i++)
	{
		sScrollerArray[i].link = NULL;
		VK_memset(&(sScrollerArray[i].rect), 0, sizeof(ONDK_Rect_t));
		sScrollerArray[i].ID = -1;
		sScrollerArray[i].type = GUIOBJ_SCROLLER;
		sScrollerArray[i].focus = GUIOBJ_DISABLED;
		sScrollerArray[i].Proc = local_gwmscroll_defaultScrollerProc;
		sScrollerArray[i].objDrawMethod = NULL;

		sScrollerArray[i].flag = 0;
		sScrollerArray[i].range = 0;
		sScrollerArray[i].start = 0;
		sScrollerArray[i].size = 0;
		sScrollerArray[i].horizontal = 0;
		sScrollerArray[i].color_inactiveFace = 0xFFA0ABBD;
		isAllocated[i] = 0;
	}
}


/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

static ONDK_Result_t		local_gwmscroll_CreateScroller(int id, ONDK_Rect_t *rect, HINT32 range, HINT32 start, HINT32 size, HINT32 type)
{
	ONDK_GWM_GuiObject_t			objlist;
	SCROLLER_OBJECT		new_scrlObj;
	ONDK_Result_t				hresult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_scrlObj = (SCROLLER_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_scrlObj != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_scrlObj = local_gwmscroll_AllocNewScrollerObj();
	if (new_scrlObj == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	new_scrlObj->link = NULL;
	VK_memcpy(&(new_scrlObj->rect), rect, sizeof(ONDK_Rect_t));
	new_scrlObj->ID = id;
	new_scrlObj->type = GUIOBJ_SCROLLER;
	new_scrlObj->focus = GUIOBJ_DISABLED;
	new_scrlObj->Proc = local_gwmscroll_defaultScrollerProc;
	new_scrlObj->objDrawMethod = NULL;

	new_scrlObj->flag = 0;
	new_scrlObj->range = range;
	new_scrlObj->start = start;
	new_scrlObj->size = size;
	new_scrlObj->horizontal = type;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)new_scrlObj, 0);

	hresult = INT_ONDK_GWM_APP_AddObject( g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_scrlObj);
	ONDK_GWM_APP_InvalidateRelativeRect(	new_scrlObj->rect.x,
						new_scrlObj->rect.y,
						new_scrlObj->rect.w,
						new_scrlObj->rect.h);

	return	hresult;
}


ONDK_Result_t			GWM_Scroll_CreateVertical(HINT32 id, ONDK_Rect_t *rect, HINT32 range, HINT32 start, HINT32 size)
{
	return local_gwmscroll_CreateScroller(id, rect, range, start, size, 0);	// type = Verti..
}


ONDK_Result_t			GWM_Scroll_CreateHorizontal(HINT32 id, ONDK_Rect_t *rect, HINT32 range, HINT32 start, HINT32 size)
{
	return local_gwmscroll_CreateScroller(id, rect, range, start, size, 1);	// type = Horiz..
}


void					GWM_Scroll_GetProperty(SCROLLER_OBJECT object,
												HINT32 *type, HINT32 *range, HINT32 *start, HINT32 *size)
{
	*range = object->range;
	*start = object->start;
	*size = object->size;
	*type = object->horizontal;
}


ONDK_Result_t			GWM_Scroll_SetValue(HINT32 id, HINT32 range, HINT32 start, HINT32 size)
{
	SCROLLER_OBJECT		obj;

	obj = (SCROLLER_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_SCROLLER)
		return GWM_RET_NO_OBJECT;

	obj->range = range;
	obj->start = start;
	obj->size = size;

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);
	return GWM_RET_OK;
}

ONDK_Result_t	GWM_Scroll_SetColor(HINT32 id, ONDK_Color_t face, ONDK_Color_t shadow)
{
	SCROLLER_OBJECT		obj;

	obj = (SCROLLER_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_SCROLLER)
		return GWM_RET_NO_OBJECT;

	obj->color_inactiveFace = face;
	obj->color_disabled_shadow = shadow;

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);
	return GWM_RET_OK;
}

/* End of File. ---------------------------------------------------------- */

