

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <ondk.h>

#include "../ondk_gwm_objdef.h"
#include <ondk_gwm_appobj.h>
#include "../ondk_gwm_internal.h"
/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define MAX_FMT_OBJECT						2
#define FMT_ITEM_FONT_SIZE					24

#define COL_ARROW_NOMORE					0xFFFFFFFF
#define COL_ARROW_SOMEMORE				0xFF0000FF


/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */

struct	tagFMTObject
{
	GUIOBJ_BASIC_PROPERTY
	int					attrib;						// list attribute
	int					page_num;					// # of items per page
	ONDK_Rect_t			items_rect;					// list item bounds
	int					item_num;					// total number of items
	int					select_index;				// selected index
	int					start_index;					// start index of the list
	HBOOL				visible;						// visibility
	FMT_Item_t			item[MAX_FMT_ITEM];			// item list

	FMT_DrawItem_Func	pDrawItemFunc;
	FMT_Notify_Func		pNotifyFunc;
	FMT_DrawScroll_Func	pDrawScrollFunc;

	ONDK_Color_t		inactive_face;
	ONDK_Color_t		hilighted_text;
	ONDK_Color_t		frame_light;
	ONDK_Color_t		frame_face;
	ONDK_Color_t		focus_face;
	ONDK_Color_t		disabled_shadow;

};


/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */

static struct 	tagFMTObject					sFMTArray[MAX_FMT_OBJECT];
static char								isAllocated[MAX_FMT_OBJECT];


/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */

void 		_setRect(ONDK_Rect_t *rect, int x, int y, int w, int h)
{
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
}

static FMT_OBJECT			local_gwmfmt_AllocNewObj(void)
{
	int		i;

	for (i = 0; i < MAX_FMT_OBJECT; i++)
	{
		if (!isAllocated[i])
			break;
	}
	if (i < MAX_FMT_OBJECT)
	{
		isAllocated[i] = 1;
		return &sFMTArray[i];
	}
	return NULL;
}

static void				local_gwmfmt_FreeObj(FMT_OBJECT obj)
{
	int				i, j;
	FMT_Item_t		*pItem;

	GWM_assert(obj);
#ifdef CONFIG_DEBUG
	if (obj->link != NULL)
		GWM_DBG_Print(DBGMSG_OBJ, ("[ WARNING ] Object(%d):%08X is currently linked.\n", obj->ID, (int)obj));
#endif
	for (i = 0; i < MAX_FMT_OBJECT; i++)
	{
		if (obj == &(sFMTArray[i]))
			break;
	}

	if (i < MAX_FMT_OBJECT)
	{
		isAllocated[i] = 0;
		INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(sFMTArray[i]));

		// free allocated string storages
		for (j = 0; j < obj->item_num; j++)
		{
			pItem = &obj->item[j];
			if (pItem->itemStr != NULL)
			{
				ONDK_Free(pItem->itemStr);
			}
			VK_memset(pItem, 0x00, sizeof(FMT_Item_t));
		}

		// free extended properties
		sFMTArray[i].attrib = 0x00;
		sFMTArray[i].page_num = 0;
		VK_memset(&sFMTArray[i].items_rect, 0x00, sizeof(ONDK_Rect_t));
		sFMTArray[i].item_num = 0;
		sFMTArray[i].select_index = 0;
		sFMTArray[i].start_index = 0;
		sFMTArray[i].visible = FALSE;
		VK_memset(&sFMTArray[i].item, 0x00, sizeof(FMT_Item_t) * MAX_FMT_ITEM);
	}
	return;
}


static void 	local_gwmfmt_drawScroll(ONDKSurfaceHandle pixmap, FMT_OBJECT obj)
{
	HINT32			x, y, w, h;
	ONDK_Region_t	appArea;
	ONDK_Color_t	colorUp, colorDown;

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &appArea);

	x = appArea.x1 + obj->rect.x;
	y = appArea.y1 + obj->rect.y;
	w = obj->rect.w;
	h = obj->rect.h;

	// draw scroll bar
	if ((obj->attrib & FMT_SCROLL_ON))
	{
		// for default/typical scroll bar
		ONDK_DRAW_FillRectElement(pixmap, x + w - FMT_SCROLL_W, y, x + w -11, y + 10, obj->inactive_face);
		ONDK_DRAW_FillRoundRect(pixmap, x+w-10, y+10, x+w, y, 10, obj->inactive_face);
		ONDK_DRAW_FillRectElement(pixmap, x + w - FMT_SCROLL_W, y + 10, x + w - 1, y + h - 10, obj->inactive_face);
		ONDK_DRAW_FillRectElement(pixmap, x + w - FMT_SCROLL_W, y + h - 10, x + w - 11, y + h - 1, obj->inactive_face);
		ONDK_DRAW_FillRoundRect(pixmap, x+w-10, y+h-11, x+w, y+h, 10, obj->inactive_face);

		// draw scroll arrows
		if (obj->start_index > 0)
			colorUp = COL_ARROW_SOMEMORE;
		else
			colorUp = COL_ARROW_NOMORE;

		if (obj->start_index + obj->page_num < obj->item_num)
			colorDown = COL_ARROW_SOMEMORE;
		else
			colorDown = COL_ARROW_NOMORE;

	}
}


static ONDK_Result_t	local_gwmfmt_drawItemDefault(int ID, ONDKSurfaceHandle pixmap, FMT_DrawItem_t *pDrawItem)
{
	FMT_OBJECT		obj = NULL;
	ONDK_Rect_t		*rc;
	HINT32			yOff, height;
	ONDKFontHandle		*pFont = NULL;

	obj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (obj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (pDrawItem == NULL)
	{
		return ERR_FAIL;
	}


	rc = &pDrawItem->itemRect;

	// draw a list item
	pFont = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, FMT_ITEM_FONT_SIZE);
    height = ONDK_FONT_GetHeight(pFont);
	if (pFont != NULL)
	{
		yOff = (rc->h / 2) - (height / 2);
		OSD_GFX_DrawStringWithAbbr(pixmap, pFont, pDrawItem->itemRect.x + 18, pDrawItem->itemRect.y + yOff,
								pDrawItem->itemRect.w - 18, pDrawItem->str, obj->hilighted_text);
	}
	ONDK_FONT_Release(pFont);

	return GWM_RET_OK;
}


static void			local_gwmfmt_drawFirstItem(ONDKSurfaceHandle pixmap, int x, int y, int w, int h, int facecolor, FMT_OBJECT obj)
{

	ONDK_DRAW_FillRoundRect(pixmap, x+10, y+10, x, y, 10, facecolor);
	ONDK_DRAW_FillRectElement(pixmap, x, y + 10, x + 10, y + h - 1, facecolor);
	ONDK_DRAW_FillRectElement(pixmap, x + 10, y, x + w - 1, y + h - 3, facecolor);
	ONDK_DRAW_FillRectElement(pixmap, x, y + h - 2, x + w - 1, y + h - 1, obj->frame_face);
}


static void			local_gwmfmt_drawMiddleItem(ONDKSurfaceHandle pixmap, int x, int y, int w, int h, int facecolor, FMT_OBJECT obj)
{
	ONDK_DRAW_FillRectElement(pixmap, x, y, x + w - 1, y + h - 1, facecolor);
	ONDK_DRAW_FillRectElement(pixmap, x, y + h - 2, x + w - 1, y + h - 1, obj->frame_face);
}


static void			local_gwmfmt_drawLastItem(ONDKSurfaceHandle pixmap, int x, int y, int w, int h, int facecolor, FMT_OBJECT obj)
{

	ONDK_DRAW_FillRectElement(pixmap, x, y, x + 10, y + h - 11, facecolor);
	ONDK_DRAW_FillRoundRect(pixmap, x+10, y+h-11, x, y+h, 10, facecolor);
	ONDK_DRAW_FillRectElement(pixmap, x + 10, y, x + w - 1, y + h - 1, facecolor);
}


static void 			local_gwmfmt_drawItems(ONDKSurfaceHandle pixmap, FMT_OBJECT obj)
{
	ONDK_Rect_t			*itemRc = &obj->items_rect;
	FMT_Item_t			*item = NULL;
	FMT_DrawItem_t		drawItem;
	int					id = obj->ID;
	int					page_num;
	int					i, y = 0, index = 0, start = 0, end;
	int					fillColor = obj->frame_light;
	ONDK_Region_t			appArea;

	/* init local vars */
	memset(&drawItem, 0, sizeof(FMT_DrawItem_t));

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &appArea);          // By AhnSik.

	page_num = obj->page_num;
	start = obj->start_index;
	end = start + page_num;
	y = itemRc->y;

	// draw each items
	for (i = start; i < end; i++)
	{
		item = &obj->item[i];
		if (item != NULL)
		{
			_setRect(&drawItem.itemRect, appArea.x1 + itemRc->x, appArea.y1 + y,
					itemRc->w, itemRc->h / page_num);

			drawItem.status = item->status;
			drawItem.list_index = index;
			drawItem.item_index = i;
			drawItem.str = item->itemStr;

			drawItem.stTextProperty.ulFontSize = item->stTextProperty.ulFontSize;
			drawItem.stTextProperty.ulFontAlign = item->stTextProperty.ulFontAlign;
			drawItem.stTextProperty.cFontColor = item->stTextProperty.cFontColor;

			y += itemRc->h / page_num;
			index++;

			// draw each item's background (default)
			if (obj->attrib & FMT_DEFAULT_LIST)
			{
				if (obj->select_index == drawItem.item_index && obj->focus && obj->item_num > 0)
					fillColor = obj->focus_face;
				else if (obj->select_index == drawItem.item_index && !obj->focus)
					fillColor = obj->disabled_shadow;
				else
					fillColor = obj->frame_light;

				if (i == start)
					local_gwmfmt_drawFirstItem(pixmap, drawItem.itemRect.x, drawItem.itemRect.y, drawItem.itemRect.w -2, drawItem.itemRect.h, fillColor, obj);
				else if (i == end - 1)
					local_gwmfmt_drawLastItem(pixmap, drawItem.itemRect.x, drawItem.itemRect.y, drawItem.itemRect.w - 2, drawItem.itemRect.h, fillColor, obj);
				else
					local_gwmfmt_drawMiddleItem(pixmap, drawItem.itemRect.x, drawItem.itemRect.y, drawItem.itemRect.w - 2, drawItem.itemRect.h, fillColor, obj);
			}

			// draw item string
			if (obj->attrib & FMT_OWN_DRAWITEM)
			{
				if (obj->pDrawItemFunc)
					(*obj->pDrawItemFunc)(id, pixmap, &drawItem);
			}
			else
			{
				// draw item string
				if (i < obj->item_num)
					local_gwmfmt_drawItemDefault(id, pixmap, &drawItem);
			}
		}
	}
}

#define FMT_SCROLL_MARGIN			3
STATIC ONDK_Result_t	local_gwmfmt_defaultProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	FMT_OBJECT		fmtObj;
	ONDK_Region_t		area;
	ONDK_Rect_t		rect;
	HINT32				linePerPage = 0;
	HINT32				linePerScroll = 0;
	ONDKSurfaceHandle	screen = NULL;
	// WRAP AROUND하지 않고, min/max index상태에서 Notify method를 호출하지 않도록 하는 flag (vertical list에만 적용)
	HBOOL			needNotify = FALSE;

	GWM_assert(object->type == GUIOBJ_FMT);

	fmtObj = (FMT_OBJECT)object;
	screen = ONDK_GWM_GetCurSurface();
	linePerPage = fmtObj->page_num;
	if(linePerPage >  FMT_SCROLL_MARGIN)
	{
		linePerScroll = linePerPage - FMT_SCROLL_MARGIN;
	}
	else
	{
		linePerScroll = 1;
	}

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwmfmt_FreeObj(fmtObj);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			if (fmtObj->visible == FALSE)
				return GWM_MESSAGE_BREAK;

			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);

			if (object->objDrawMethod != NULL)
			{	/* draw via customized drawing method */
				object->objDrawMethod(screen, &area, &rect, object, (FMT_OBJECT)fmtObj, arg);
			}

			// draw scroll
			if (fmtObj->pDrawScrollFunc == NULL)
			{
				local_gwmfmt_drawScroll(screen, fmtObj);
			}
			else
			{
				if (fmtObj->attrib & FMT_SCROLL_ON)
				{
					INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
					fmtObj->pDrawScrollFunc(screen, &area, &rect, fmtObj);
				}
			}

			// draw list items
			local_gwmfmt_drawItems(screen, fmtObj);
			return GWM_MESSAGE_BREAK;
		}

		case MT_GET_FOCUS:
			object->focus = GUIOBJ_HILIGHTED;
			ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);
			return GWM_MESSAGE_BREAK;

		case MT_LOST_FOCUS:
			object->focus = GUIOBJ_NORMAL;
			ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);
			return GWM_MESSAGE_BREAK;

		case MT_CHANGE_TEXT:
			ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);
			return GWM_MESSAGE_BREAK;

		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);
			VK_memcpy(&(fmtObj->rect), new_rect, sizeof(ONDK_Rect_t));
			ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);
			return GWM_MESSAGE_BREAK;
		}

		case MT_SET_VALUE:
		{
			ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);
			return GWM_MESSAGE_BREAK;
		}

		case MT_KEYDOWN:
			switch (arg)
			{
				case KEY_ARROWUP:
					if (object->focus && fmtObj->visible)
					{
						// 1 page item => ignore
						if ( (fmtObj->item_num > linePerPage)
							&& (fmtObj->start_index > 0) )
						{
							if(fmtObj->start_index - linePerScroll >= 0)
							{
								fmtObj->start_index -= linePerScroll;
							}
							else if(fmtObj->start_index - linePerScroll < 0)
							{
								fmtObj->start_index = 0;
							}
							needNotify = TRUE;

							ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);
							// process the notify action if defined
							if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL && needNotify)
								fmtObj->pNotifyFunc(object->ID, fmtObj->select_index);
						}
					}
					return GWM_MESSAGE_BREAK;

				case KEY_ARROWDOWN:
					if (object->focus && fmtObj->visible)
					{
						if ( (fmtObj->item_num > linePerPage)							/* 1 page 이하 무시 */
							&& (fmtObj->start_index + linePerPage < fmtObj->item_num) )	/* 전체 라인수보다 커도 무시 */
						{
							if(fmtObj->start_index + linePerPage +  linePerScroll > fmtObj->item_num)
							{
								fmtObj->start_index = fmtObj->item_num - linePerPage;
							}
							else if(fmtObj->start_index + linePerPage +  linePerScroll <= fmtObj->item_num)
							{
								fmtObj->start_index += linePerScroll;
							}

							needNotify = TRUE;

							ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);

							// process the notify action if defined
							if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL && needNotify)
								fmtObj->pNotifyFunc(object->ID, fmtObj->select_index);
						}
					}
					return GWM_MESSAGE_BREAK;

				case KEY_ARROWLEFT:
					if (object->focus && fmtObj->visible)
					{
						if (fmtObj->item_num > 0)
						{
							if (fmtObj->item_num <= linePerPage)
							{
								// move to the first item
								fmtObj->select_index = 0;
								needNotify = TRUE;
							}
							else
							{
								if (fmtObj->select_index - fmtObj->start_index == 0)
								{
									if (fmtObj->start_index > linePerPage)
									{
										// scroll up a page
										fmtObj->start_index = fmtObj->select_index - (linePerPage - 1);
										fmtObj->select_index = fmtObj->start_index;
										needNotify = TRUE;
									}
									else
									{
										// move to the first item of the whole list
										fmtObj->start_index = 0;
										fmtObj->select_index = 0;
										needNotify = TRUE;
									}
								}
								else
								{
									// move to the first item
									fmtObj->select_index = fmtObj->start_index;
									needNotify = TRUE;
								}
							}
						}
						ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);
						// process the notify action if defined
						if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL && needNotify)
							fmtObj->pNotifyFunc(object->ID, fmtObj->select_index);
					}
					return GWM_MESSAGE_BREAK;

				case KEY_ARROWRIGHT:
					if (object->focus && fmtObj->visible)
					{
						if (fmtObj->item_num > 0)
						{
							if (fmtObj->item_num <= linePerPage)
							{
								// move to the last item
								fmtObj->select_index = fmtObj->item_num - 1;
								needNotify = TRUE;
							}
							else
							{
								if (fmtObj->select_index - fmtObj->start_index == (HUINT16)(linePerPage - 1))
								{
									if (fmtObj->select_index + linePerPage < fmtObj->item_num)
									{
										// scroll down a page
										fmtObj->start_index = fmtObj->select_index;
										fmtObj->select_index = fmtObj->start_index + (linePerPage - 1);
										needNotify = TRUE;
									}
									else
									{
										// move to the last item of the whole list
										fmtObj->start_index = fmtObj->item_num - linePerPage;
										fmtObj->select_index = fmtObj->item_num - 1;
										needNotify = TRUE;
									}
								}
								else
								{
									// move to the last item
									fmtObj->select_index = fmtObj->start_index + (linePerPage - 1);
									needNotify = TRUE;
								}
							}
						}
						ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);
						// process the notify action if defined
						if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL && needNotify)
							fmtObj->pNotifyFunc(object->ID, fmtObj->select_index);
					}
					return GWM_MESSAGE_BREAK;
			}
			return GWM_MESSAGE_PASS;

		default:
			break;
	}
	return GWM_MESSAGE_PASS;
}



/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

void 		gwm_fmt_InitClass(void)
{
	int		i;

	for (i = 0; i < MAX_FMT_OBJECT; i++)
	{
		sFMTArray[i].link = NULL;
		VK_memset(&(sFMTArray[i].rect), 0, sizeof(ONDK_Rect_t));
		sFMTArray[i].ID = -1;
		sFMTArray[i].type = GUIOBJ_FMT;
		sFMTArray[i].focus = GUIOBJ_DISABLED;
		sFMTArray[i].Proc = local_gwmfmt_defaultProc;
		sFMTArray[i].objDrawMethod = NULL;

		sFMTArray[i].inactive_face = 0xFFA0ABBD;
		sFMTArray[i].hilighted_text = 0xFF442900;
		sFMTArray[i].frame_light = 0xFF5A6579;
		sFMTArray[i].frame_face = 0xFF414757;
		sFMTArray[i].focus_face = 0xFFDEB921;
		sFMTArray[i].disabled_shadow = 0xFF303549;

		isAllocated[i] = 0;
	}
}


ONDK_Result_t			ONDK_GWM_FMT_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 attrib, HINT32 pageNum)
{
	ONDK_GWM_GuiObject_t		objlist;
	FMT_OBJECT			new_fmtObj;
	ONDK_Result_t			hresult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_fmtObj = (FMT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_fmtObj != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_fmtObj = local_gwmfmt_AllocNewObj();
	if (new_fmtObj == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	new_fmtObj->link = NULL;
	VK_memcpy(&(new_fmtObj->rect), rect, sizeof(ONDK_Rect_t));
	new_fmtObj->ID = id;
	new_fmtObj->type = GUIOBJ_FMT;
	new_fmtObj->focus = GUIOBJ_NORMAL;

	new_fmtObj->Proc = local_gwmfmt_defaultProc;
	new_fmtObj->objDrawMethod = NULL;

	new_fmtObj->attrib		= attrib;
	new_fmtObj->page_num	= pageNum;

	new_fmtObj->items_rect.x 	= rect->x + FMT_BOUND_THICK;
	new_fmtObj->items_rect.y 	= rect->y + FMT_BOUND_THICK;
	if ((attrib & FMT_SCROLL_ON))
		new_fmtObj->items_rect.w	= rect->w - (FMT_SCROLL_W + FMT_BOUND_THICK);
	else
		new_fmtObj->items_rect.w	= rect->w - FMT_BOUND_THICK * 2;

	if ((attrib & FMT_OWN_DRAWITEM))
	{
		new_fmtObj->items_rect.w	= rect->w - FMT_BOUND_THICK * 2;
	}

	new_fmtObj->items_rect.h	= rect->h - FMT_BOUND_THICK * 2;

	new_fmtObj->item_num 	= 0;
	new_fmtObj->select_index	= 0;
	new_fmtObj->start_index 	= 0;
	new_fmtObj->visible 		= TRUE;

	new_fmtObj->pDrawItemFunc 	= NULL;
	new_fmtObj->pNotifyFunc		= NULL;
	new_fmtObj->pDrawScrollFunc	= NULL;

	// initialize item array
	VK_memset(new_fmtObj->item, 0x00, sizeof(FMT_Item_t) * MAX_FMT_ITEM);

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_fmtObj);
	ONDK_GWM_APP_InvalidateRelativeRect(	new_fmtObj->rect.x,
						new_fmtObj->rect.y,
						new_fmtObj->rect.w,
						new_fmtObj->rect.h);

	return hresult;
}


ONDK_Result_t 		ONDK_GWM_FMT_ChangeItemDrawMethod(HINT32 ID, FMT_DrawItem_Func method)
{
	FMT_OBJECT		fmtObj = NULL;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	fmtObj->pDrawItemFunc = method;

	return GWM_RET_OK;
}


ONDK_Result_t 		ONDK_GWM_FMT_ChangeNotifyMethod(HINT32 ID, FMT_Notify_Func method)
{
	FMT_OBJECT		fmtObj = NULL;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	fmtObj->pNotifyFunc = method;

	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_FMT_ChangeScrollDrawMethod(HINT32 ID, FMT_DrawScroll_Func method)
{
	FMT_OBJECT		fmtObj = NULL;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	fmtObj->pDrawScrollFunc = method;

	return GWM_RET_OK;
}


void					ONDK_GWM_FMT_GetProperty(FMT_OBJECT obj, HUINT32 *attrib, HUINT32 *startIndex, HUINT32 *pageNum, HUINT32 *itemNum)
{
	GWM_assert(obj->type == GUIOBJ_FMT);

	*attrib = obj->attrib;
	*startIndex = obj->start_index;
	*pageNum = obj->page_num;
	*itemNum = obj->item_num;
}


ONDK_Result_t			ONDK_GWM_FMT_AddItem(HINT32 ID, HCHAR *item_str, Text_Property_t *pstTextProperty)
{
	char				*str;
	int				strLen, size;
	FMT_Item_t		*item = NULL;
	FMT_OBJECT		fmtObj = NULL;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (fmtObj->item_num < 0 || fmtObj->item_num >= MAX_FMT_ITEM)
	{
		return GWM_RET_OUT_OF_RANGE;
	}

	strLen = HxSTD_StrLen(item_str);
	size = strLen + 2;
	str = (char *)ONDK_Malloc(sizeof(HUINT8)*size);
	VK_memcpy(str, item_str, strLen);
	str[size-1] = str[size-2] = 0x00;

	item = &(fmtObj->item[fmtObj->item_num]);
	item->size = size;
	item->itemStr	 = str;

	if (pstTextProperty != NULL)
	{
		item->stTextProperty.ulFontSize = pstTextProperty->ulFontSize;
		item->stTextProperty.ulFontAlign = pstTextProperty->ulFontAlign;
		item->stTextProperty.cFontColor = pstTextProperty->cFontColor;
	}

	fmtObj->item_num++;

	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_FMT_SelectItem(HINT32 ID, HINT32 selIndex)
{
	FMT_OBJECT		fmtObj = NULL;
	HINT32			num, page_num;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	if (selIndex < 0 || selIndex > MAX_FMT_ITEM)
	{
		fmtObj->select_index = 0;
		fmtObj->start_index = 0;
		return GWM_RET_OUT_OF_RANGE;
	}

	num = fmtObj->item_num;
	page_num = fmtObj->page_num;
	if (fmtObj->item_num < page_num)
		page_num = fmtObj->item_num;

	// out of range
	if (selIndex > (fmtObj->item_num - 1))
	{
		fmtObj->start_index = 0;
		fmtObj->select_index = 0;
		return GWM_RET_OUT_OF_RANGE;
	}

	fmtObj->select_index = selIndex;

	if (selIndex >= 0 && selIndex < page_num)
	{
		fmtObj->start_index = 0;
	}
	else if (selIndex >= page_num && selIndex < (fmtObj->item_num - page_num))
	{
		fmtObj->start_index = selIndex;
	}
	else
	{
		fmtObj->start_index = fmtObj->item_num - page_num;
	}

	// when item selected, call notify method if defined
	if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL)
		fmtObj->pNotifyFunc(fmtObj->ID, fmtObj->select_index);

	return GWM_RET_OK;
}


ONDK_Result_t 		ONDK_GWM_FMT_GetItem(HINT32 ID, HINT32 item_index, HCHAR **outString)
{
	FMT_OBJECT		fmtObj = NULL;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	if (item_index < 0 || item_index >= MAX_FMT_ITEM)
	{
		*outString = NULL;
		return GWM_RET_OUT_OF_RANGE;
	}

	*outString = (char *)fmtObj->item[item_index].itemStr;

	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_FMT_Delete(HINT32 ID)
{
	FMT_Item_t		*pItem;
	FMT_OBJECT		fmtObj = NULL;
	int				i;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	// delete list items
	for (i = 0; i < fmtObj->item_num; i++)
	{
		pItem = &fmtObj->item[i];
		if (pItem->itemStr != NULL)
		{
			ONDK_Free(pItem->itemStr);
		}
		VK_memset(pItem, 0x00, sizeof(FMT_Item_t));
	}
	fmtObj->item_num = 0;
	fmtObj->select_index = 0;
	fmtObj->start_index = 0;

	return GWM_RET_OK;
}

ONDK_Result_t			ONDK_GWM_FMT_SetStartIndex(HINT32 ID, HINT32 startIndex)
{
	FMT_OBJECT		fmtObj = NULL;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	fmtObj->start_index = startIndex;

	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_FMT_SetPageNum(HINT32 ID, HINT32 pageNum)
{
	FMT_OBJECT		fmtObj = NULL;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	fmtObj->page_num = pageNum;

	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_FMT_ProcessKey(HINT32 ID, HINT32 key)
{
	FMT_OBJECT		fmtObj = NULL;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_KEYDOWN, (ONDK_GWM_GuiObject_t)fmtObj, key);
	ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);

	return GWM_RET_OK;
}

void 				ONDK_GWM_FMT_ScrollUp(HINT32 objID)
{
	FMT_OBJECT		fmtObj = NULL;

	// scroll a page up of the text
	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(objID);
	if (fmtObj == NULL)
		return;

	if (fmtObj->start_index - 1 > 0)
	{
		fmtObj->start_index -=  1;
		fmtObj->select_index = fmtObj->start_index;
	}
	else
	{
		fmtObj->start_index = 0;
		fmtObj->select_index = 0;
	}
	ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);

	if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL )
	{
		fmtObj->pNotifyFunc(objID, fmtObj->select_index);
	}
}

void 				ONDK_GWM_FMT_ScrollDown(HINT32 objID)
{
	FMT_OBJECT		fmtObj = NULL;
	HUINT16			pageNum = 0;

	// scroll a page down of the text
	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(objID);
	if (fmtObj == NULL)
		return;
	pageNum = fmtObj->page_num;

	if( fmtObj->item_num - pageNum <= 0 )
		return;

	if (fmtObj->start_index + pageNum < fmtObj->item_num -1)
	{
		fmtObj->start_index += 1;
		fmtObj->select_index = fmtObj->start_index;
	}
	else
	{
		fmtObj->start_index = fmtObj->item_num - pageNum;
		fmtObj->select_index = fmtObj->start_index;
	}
	ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);

	if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL )
	{
		fmtObj->pNotifyFunc(objID, fmtObj->select_index);
	}
}

void 				ONDK_GWM_FMT_ScrollUpPage(HINT32 objID)
{
	FMT_OBJECT		fmtObj = NULL;

	// scroll a page up of the text
	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(objID);
	if (fmtObj == NULL)
		return;

	if (fmtObj->start_index - (fmtObj->page_num - 1) > 0)
	{
		fmtObj->start_index -= (fmtObj->page_num - 1);
		fmtObj->select_index = fmtObj->start_index;
	}
	else
	{
		fmtObj->start_index = 0;
		fmtObj->select_index = 0;
	}
	ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);

	if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL )
	{
		fmtObj->pNotifyFunc(objID, fmtObj->select_index);
	}

}


void 				ONDK_GWM_FMT_ScrollDownPage(HINT32 objID)
{
	FMT_OBJECT		fmtObj = NULL;
	HUINT16			pageNum = 0;

	// scroll a page down of the text
	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(objID);
	if (fmtObj == NULL)
		return;

	pageNum = fmtObj->page_num;
	if (fmtObj->item_num < pageNum)
		return;
	if (fmtObj->start_index + pageNum < fmtObj->item_num - 1)
	{
		if (fmtObj->start_index + pageNum <= fmtObj->item_num - pageNum)
		{
			fmtObj->start_index += (pageNum - 1);
			fmtObj->select_index = fmtObj->start_index;
		}
		else
		{
			fmtObj->start_index = fmtObj->item_num - pageNum;
			fmtObj->select_index = fmtObj->start_index;
		}
	}
	else
	{
		fmtObj->start_index = fmtObj->item_num - pageNum;
		fmtObj->select_index = fmtObj->start_index;
	}
	ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);

	if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL )
	{
		fmtObj->pNotifyFunc(objID, fmtObj->select_index);
	}
}

void 				ONDK_GWM_FMT_ScrollUpPageStrict(HINT32 objID)
{
	FMT_OBJECT		fmtObj = NULL;

	// scroll a page up of the text
	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(objID);
	if (fmtObj == NULL)
		return;

	if (fmtObj->start_index - (fmtObj->page_num) >= 0)
	{
		fmtObj->start_index -= (fmtObj->page_num);
		fmtObj->select_index = fmtObj->start_index;
	}
	else
	{
		return;
	}
	ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);

	if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL )
	{
		fmtObj->pNotifyFunc(objID, fmtObj->select_index);
	}

}


void 				ONDK_GWM_FMT_ScrollDownPageStrict(HINT32 objID)
{
	FMT_OBJECT		fmtObj = NULL;
	HUINT16			pageNum = 0;

	// scroll a page down of the text
	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(objID);
	if (fmtObj == NULL)
		return;

	pageNum = fmtObj->page_num;
	if (fmtObj->item_num < pageNum)
		return;
	if (fmtObj->start_index + pageNum < fmtObj->item_num)
	{
		fmtObj->start_index += (pageNum);
		fmtObj->select_index = fmtObj->start_index;
	}
	else
	{
		return;
	}
	ONDK_GWM_APP_InvalidateRelativeRect(fmtObj->rect.x, fmtObj->rect.y, fmtObj->rect.w, fmtObj->rect.h);

	if ((fmtObj->attrib & FMT_NOTIFY_ACTION) && fmtObj->pNotifyFunc != NULL )
	{
		fmtObj->pNotifyFunc(objID, fmtObj->select_index);
	}
}

#define	________FOR_CUSTOM_TAG____________________________

ONDK_Result_t			ONDK_GWM_FMT_AddnItem(HINT32 ID, HUINT8 *pItemStr, HUINT32 ulLength, Text_Property_t *pstTextProperty)
{
	HUINT8			*pStr;
	HINT32			lSize;
	FMT_Item_t		*pItem = NULL;
	FMT_OBJECT		fmtObj = NULL;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	if ( (pItemStr == NULL) || (ulLength == 0) )
		return GWM_RET_NO_OBJECT;

	if (fmtObj->item_num < 0 || fmtObj->item_num >= MAX_FMT_ITEM)
		return GWM_RET_OUT_OF_RANGE;

	lSize = ulLength + 2;
	pStr = (HUINT8*)ONDK_Malloc(sizeof(HUINT8)*lSize);
	VK_memcpy(pStr, pItemStr, ulLength);
	pStr[lSize-1] = pStr[lSize-2] = 0x00;

	pItem = &(fmtObj->item[fmtObj->item_num]);
	pItem->size = lSize;
	pItem->itemStr = pStr;

	if (pstTextProperty != NULL)
	{
		pItem->stTextProperty.ulFontSize = pstTextProperty->ulFontSize;
		pItem->stTextProperty.ulFontAlign = pstTextProperty->ulFontAlign;
		pItem->stTextProperty.cFontColor = pstTextProperty->cFontColor;
	}

	fmtObj->item_num++;

	return GWM_RET_OK;
}

// custom tag string을 사용하려면 반드시 GWM_FMT_AddnItem()와 customDrawFunc(with parser)를 같이 사용할 것..!!
ONDK_Result_t			ONDK_GWM_FMT_MakeCustomTagString(HUINT8 ucMainGenreIndex, HUINT8 *pszRetStr,  HUINT8 *pucLen)
{
	ONDK_Result_t		hRet = GWM_RET_INITIAL_FAILED;
	HUINT8				aucTmp[8];
	HUINT32				ulStrLen;

	if ( (pszRetStr == NULL) || (pucLen == NULL) )
	{
#ifdef CONFIG_DEBUG
		GWM_DBG_Print(DBGMSG_OBJ, ("[ ERROR ] null pointer param \n"));
#endif
		goto EXIT_FMT_FUNC;
	}

	VK_memset(aucTmp, 0x00, (sizeof(HUINT8)*8));

	snprintf(aucTmp, 8, "%c%c%c%d%c",	FMT_CUSTOMTAG_ON,
									FMT_CUSTOMTAG_IMAGE,
									FMT_CUSTOMTAG_DATA_START,
									ucMainGenreIndex,
									FMT_CUSTOMTAG_DATA_END);

	ulStrLen = HxSTD_StrLen(aucTmp);
	HxSTD_StrNCpy(pszRetStr, aucTmp, ulStrLen); //MWC_UTIL_DvbStrncpy ..일단 이렇게...
	*pucLen = (HUINT8)ulStrLen;

	hRet = GWM_RET_OK;

EXIT_FMT_FUNC:

	return hRet;
}

ONDK_Result_t	ONDK_GWM_FMT_SetColor(HINT32 ID, ONDK_Color_t inactive_face,	ONDK_Color_t hilighted_text,
					ONDK_Color_t frame_light, ONDK_Color_t frame_face, ONDK_Color_t focus_face, ONDK_Color_t disabled_shadow)
{
	/* FMT object 정리시 해당 코드도 정리 예정 */
	FMT_OBJECT		fmtObj = NULL;

	fmtObj = (FMT_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (fmtObj == NULL)
		return GWM_RET_NO_OBJECT;

	fmtObj->inactive_face = inactive_face;
	fmtObj->hilighted_text = hilighted_text;
	fmtObj->frame_light = frame_light;
	fmtObj->frame_face = frame_face;
	fmtObj->focus_face = focus_face;
	fmtObj->disabled_shadow = disabled_shadow;
	return GWM_RET_OK;
}

/* End of File. ---------------------------------------------------------- */


