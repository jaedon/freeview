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
#include <string.h>
#include <osd_font.h>
#include <osd_gfx.h>
#include <gwm_objdef.h>
#include <gwm_appobj.h>

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define	MAX_TAB_OBJECT					10

/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */
struct tagTabObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.
	HCHAR				**item_title;				// 각 tab Item들의  title 배열
	HINT32				item_count;				// 전체 선택 가능한 tab Item의 갯수.
	HINT32				total_width;				//전체 tab이 그려질 width , 이 width에 다라 각 tab의 width가 결정됨.
	HINT32				current_index;			// 현재 선택되어 있는 tab Item의 인덱스.

	ONDK_Color_t		color_light;
	ONDK_Color_t		color_face;
	ONDK_Color_t		color_shadow;
	ONDK_Color_t		color_text;

	ONDK_Color_t		color_btnlight;
	ONDK_Color_t		color_btnface;
	ONDK_Color_t		color_btnshadow;
};

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
STATIC struct tagTabObject		sTabArray[MAX_TAB_OBJECT];
STATIC HCHAR					isAllocated[MAX_TAB_OBJECT];

/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */

STATIC TAB_OBJECT		local_gwmtab_GetTabObject(HINT32 id)
{
	ONDK_GWM_GuiObject_t		objlist;
	TAB_OBJECT	 tab;

	GWM_assert(g_pstCurrentApp);

	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);
	tab = (TAB_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);

	return tab;
}


STATIC TAB_OBJECT		local_gwmtab_AllocNewTabObj(void)
{
	HINT32		i;

	for (i = 0; i < MAX_TAB_OBJECT; i++)
	{
		if (!isAllocated[i])
			break;
	}
	if (i < MAX_TAB_OBJECT)
	{
		isAllocated[i] = 1;
		return &sTabArray[i];
	}
	return NULL;
}

STATIC void					local_gwmtab_FreeTabObj(TAB_OBJECT obj)
{
	int		i;

	GWM_assert(obj);
#ifdef CONFIG_DEBUG
	if (obj->link != NULL)
		GWM_DBG_Print(DBGMSG_OBJ, ("[ WARNING ] Object(%d):%08X is currently linked.\n", obj->ID, (int)obj));
#endif
	for (i = 0; i < MAX_TAB_OBJECT; i++)
	{
		if (obj == &(sTabArray[i]))
			break;
	}

	if (i < MAX_TAB_OBJECT)
	{
		isAllocated[i] = 0;
		INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(sTabArray[i]));
		sTabArray[i].item_title = NULL;					// ?tab Item들의  title 배열
		sTabArray[i].item_count = 0;				// 전체 선택 가능한 tab Item의 갯수.
		sTabArray[i].total_width = 0;				// 豁?tab이 그려질 width
		sTabArray[i].current_index = 0;					// 현재 선택되어 있는 tab Item의 인덱스.
	}
	return;
}


//static void			local_gwmtab_defaultDrawTabMethod(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect,
//										char *item_title, int item_count , int total_width, int status, HBOOL isFrontMost)
STATIC void			local_gwmtab_defaultDrawTabMethod(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect, TAB_OBJECT tab
																			, HINT32 status, HBOOL isFrontMost, )
{
	HINT32		bright, face, shadow, fontcolor;
	HINT32		x, y, w, h;
	HINT32		i;
	HINT32		count, 	curr , t_w , index_t = 0;

	ONDKFontHandle	font;
	ONDK_Color_t	fontCol;

	x = area->x1+tab->rect.x;
	y = area->y1+tab->rect.y;
	h = tab->rect.h;
	t_w = tab->total_width;
	count = tab->item_count;
	curr = tab->current_index;

	fontcolor = tab->color_text;

	w= (t_w/count);
	h = rect->h;

	for(i= 0; i<count; i++)
	{
		if(i == curr)
		{
			bright = tab->color_light;
			face = tab->color_face;
			shadow = tab->color_shadow;
			ONDK_GWM_GFX_DrawFrame(screen, area->x1 + rect->x + (i*w), area->y1 + rect->y , w, h, bright, face, shadow);
		}
		else
		{
			bright = tab->color_btnlight;
			face = tab->color_btnface;
			shadow = tab->color_btnshadow;
			ONDK_GWM_GFX_DrawFrame(screen, area->x1 + rect->x + (i*w), area->y1 + rect->y , w, h, bright, face, shadow);
		}

	}

	font = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, DEFAULT_OBJ_FONT_SIZE);
	if (font != NULL)
	{
		y = (area->y1 + rect->y) + (h - font->height) / 2;
		x = area->x1 + rect->x;

		for(i= 0; i<count; i++)
		{
			OSD_GFX_DrawStringWithAbbr(screen, font, x+w/2+(i*w),  y, w, (HUINT8 *)tab->item_title[index_t], fontcolor, ONDKSTF_LEFT);
			index_t ++;
		}

	}
	ONDK_FONT_Release(font);
}

STATIC	ONDK_Result_t	local_gwmtab_defaultTabProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	TAB_OBJECT			tab;
	ONDK_Region_t		area;
	ONDK_Rect_t			rect;
	ONDKSurfaceHandle	screen = ONDK_GWM_GetSurface();
	GWM_assert(object->type == GUIOBJ_TAB);
	tab = (TAB_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwmtab_FreeTabObj(tab);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			if (object->objDrawMethod != NULL)
			{	/* draw with customized drawing method */
				object->objDrawMethod(screen, &area, &rect, object, (TAB_OBJECT)tab, arg);
			}
			else
			{	/* draw with default method */
				if (tab->current_index == 0 )//first
					tab->focus |= TAB_RIGHT_NOMORE;
				else if (tab->current_index == (tab->item_count-1))//last
					tab->focus |= TAB_LEFT_NOMORE;
				else //middle
				{
					tab->focus |= TAB_RIGHT_NOMORE;
					tab->focus |= TAB_LEFT_NOMORE;
				}

				local_gwmtab_defaultDrawTabMethod(screen, &area, &rect,(TAB_OBJECT)object, tab->focus,(arg > 0) ? TRUE : FALSE);
			}
			return GWM_MESSAGE_BREAK;
		}
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(tab->rect.x, tab->rect.y, tab->rect.w, tab->rect.h);
			memcpy( &(tab->rect), new_rect, sizeof(ONDK_Region_t));
			ONDK_GWM_APP_InvalidateRelativeRect(tab->rect.x, tab->rect.y, tab->rect.w, tab->rect.h);
			return GWM_MESSAGE_BREAK;
		}

		case MT_SET_VALUE:
		{
			tab->current_index = arg;
			ONDK_GWM_APP_InvalidateRelativeRect(tab->rect.x, tab->rect.y, tab->rect.w, tab->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_KEYDOWN:
		{
			switch (arg)
			{
				case KEY_ARROWLEFT:
				{
					if (tab->current_index > 0)
					{
						tab->current_index--;
						ONDK_GWM_APP_InvalidateRelativeRect(tab->rect.x, tab->rect.y, tab->rect.w, tab->rect.h);
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_TAB_CHANGED,
									(Handle_t)NULL, object->ID, tab->current_index, (int)tab, NULL);
					}
					else
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_TAB_LEFT_NOMORE,
									(Handle_t)NULL, object->ID, tab->item_count, (int)tab, NULL);
					return GWM_MESSAGE_BREAK;
				}

				case KEY_ARROWRIGHT:
				{
					if (tab->current_index < tab->item_count-1)
					{
						tab->current_index++;
						ONDK_GWM_APP_InvalidateRelativeRect(tab->rect.x, tab->rect.y, tab->rect.w, tab->rect.h);
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_TAB_CHANGED,
									(Handle_t)NULL, object->ID, tab->current_index, (int)tab, NULL);
					}
					else
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_TAB_RIGHT_NOMORE,
									(Handle_t)NULL, object->ID, tab->item_count, (int)tab, NULL);
					return GWM_MESSAGE_BREAK;
				}
/*
				case KEY_ARROWUP:
				{
					GWM_APP_Call(g_pstCurrentApp, MSG_GWM_TAB_ENTER_APP,
								(Handle_t)NULL, object->ID, tab->current_index, tab->AppCallback[ tab->current_index]);
					return GWM_MESSAGE_BREAK;
				}


				case KEY_ARROWDOWN:
				{
					GWM_APP_Call(g_pstCurrentApp, MSG_GWM_TAB_ENTER_APP,
								(Handle_t)NULL, object->ID, tab->current_index, tab->AppCallback[ tab->current_index]);
					return GWM_MESSAGE_BREAK;
				}
*/
				default:
					break;
			}
			break;	//MESSAGE_NOTUSED;
		}

		case MT_KEYUP:
			/* return it as used one if this has no releasing effect */
			return GWM_MESSAGE_BREAK;

		case MT_GET_FOCUS:
			ONDK_GWM_APP_InvalidateRelativeRect(tab->rect.x, tab->rect.y, tab->rect.w, tab->rect.h);
			break;
		case MT_LOST_FOCUS:
			ONDK_GWM_APP_InvalidateRelativeRect(tab->rect.x, tab->rect.y, tab->rect.w, tab->rect.h);
			break;
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}


void			gwm_tab_initClass(void)
{
	HINT32		i;

	for (i = 0; i < MAX_TAB_OBJECT; i++)
	{
		sTabArray[i].link = NULL;
		VK_memset(&(sTabArray[i].rect), 0, sizeof(ONDK_Rect_t));
		sTabArray[i].ID = -1;
		sTabArray[i].type = GUIOBJ_TAB;
		sTabArray[i].focus = GUIOBJ_NORMAL;
		sTabArray[i].Proc = local_gwmtab_defaultTabProc;
		sTabArray[i].objDrawMethod = NULL;

		sTabArray[i].item_title= NULL;
		sTabArray[i].item_count = 0;
		sTabArray[i].total_width = 0;
		sTabArray[i].current_index = 0;


		sTabArray[i].color_light = 0xFFE8CE6F;
		sTabArray[i].color_face = 0xFFDEB921;
		sTabArray[i].color_shadow = 0xFFDBB106;

		sTabArray[i].color_btntext = 0xFF8A93A4;
		sTabArray[i].color_btnface = 0xFF5D7496;
		sTabArray[i].color_btnshadow 0xFF4C6486;

		sTabArray[i].color_text = 0xFF365B78;

		isAllocated[i] = 0;
	}
}


/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

#if 0
void					GWM_Tab_GetProperty(TAB_OBJECT object,
													int *status, int *total, int *curr, char **title)
{
	*status = object->focus;
	*title = object->title;
	*total = object->item_count;
	*curr = object->current_index;
}
#endif

ONDK_Result_t			GWM_Tab_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 item_count, HINT32 total_width, HCHAR **title_array)
{
	TAB_OBJECT				new_tab;
	ONDK_Result_t			hresult;

	new_tab = local_gwmtab_GetTabObject(id);
	if (new_tab != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_tab = local_gwmtab_AllocNewTabObj();
	if (new_tab == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	new_tab->link = NULL;
	VK_memcpy(&(new_tab->rect), rect, sizeof(ONDK_Rect_t));
	new_tab->ID = id;
	new_tab->type = GUIOBJ_TAB;
	new_tab->focus = GUIOBJ_NORMAL;
	new_tab->Proc = local_gwmtab_defaultTabProc;
	new_tab->objDrawMethod = NULL;

	new_tab->item_title = title_array;
	new_tab->item_count = item_count;
	new_tab->total_width= total_width;
	new_tab->current_index = 0;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)new_tab, 0);

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_tab);
	ONDK_GWM_APP_InvalidateRelativeRect(	new_tab->rect.x,
						new_tab->rect.y,
						new_tab->rect.w,
						new_tab->rect.h);

	return hresult;
}

ONDK_Result_t			GWM_Tab_GetTotalItem(HINT32 id, HINT32 *number)
{
	TAB_OBJECT	tab;

	tab = local_gwmtab_GetTabObject(id);
	if (tab == NULL)
	{
		*number = 0;
		return GWM_RET_NO_OBJECT;
	}
	if (tab->type != GUIOBJ_TAB)
	{
		*number = 0;
		return GWM_RET_NO_OBJECT;
	}

	*number = tab->item_count;
	return GWM_RET_OK;
}


HINT32					GWM_Tab_GetCurrentIndex(HINT32 id)
{
	TAB_OBJECT	tab;

	tab = local_gwmtab_GetTabObject(id);
	if (tab == NULL)
	{
		return -1;
	}
	if (tab->type != GUIOBJ_TAB)
	{
		return -1;
	}

	return tab->current_index;
}


ONDK_Result_t			GWM_Tab_SetCurrentIndex(HINT32 id, HINT32 nIndex)
{
	TAB_OBJECT	tab;

	tab = local_gwmtab_GetTabObject(id);
	if (tab == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (tab->type != GUIOBJ_TAB)
	{
		return GWM_RET_NO_OBJECT;
	}

	if ((nIndex < 0) || (nIndex >= tab->item_count))
		tab->current_index = 0;
	else
		tab->current_index = nIndex;
	return GWM_RET_OK;
}


HCHAR *				GWM_Tab_GetSelectedItemTextPtr(TAB_OBJECT tab)
{
	GWM_assert(tab->type == GUIOBJ_TAB);
	GWM_assert(tab->current_index < tab->item_count);
	GWM_assert(tab->current_index >= 0);

	return tab->item_title[tab->current_index];
}


HCHAR *				GWM_Tab_GetCurItemTextPtr(TAB_OBJECT tab)
{
	GWM_assert(tab->type == GUIOBJ_TAB);

	return tab->item_title[tab->current_index];
}



HINT32					GWM_Tab_GetSelectedIndex(TAB_OBJECT tab)
{
	GWM_assert(tab->type == GUIOBJ_TAB);

	return tab->current_index;
}

void					GWM_Tab_GetProperty(TAB_OBJECT object, int *out_total, int *out_curr, int *out_focus)
{
	if (object == NULL)
	{
		*out_total = 0;
		*out_curr = 0;
		*out_focus = 0;
		return;
	}
	*out_total = object->item_count;
	*out_curr = object->current_index;
	*out_focus = object->focus;
}

HINT32 GWM_Tab_SetColorById(HINT32 id, ONDK_Color_t light, ONDK_Color_t face, ONDK_Color_t shadow,
									ONDK_Color_t bLight, ONDK_Color_t bFace, ONDK_Color_t bShadow, ONDK_Color_t text)
{
	TAB_OBJECT	tab;

	tab = local_gwmtab_GetTabObject(id);
	if (tab == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (tab->type != GUIOBJ_TAB)
	{
		return GWM_RET_NO_OBJECT;
	}

	tab->color_light = light;
	tab->color_face = face;
	tab->color_shadow = shadow;
	tab->color_text = text;

	tab->color_btnlight = bLight;
	tab->color_btnface = bFace;
	tab->color_btnshadow = bShadow;

	return GWM_RET_OK;
}
/* End of File. ---------------------------------------------------------- */

