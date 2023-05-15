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
#define	MAX_INPUTSELECTOR_OBJECT					64
#define	MAX_INPUT_DIGIT								8

/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */
struct tagInputSelectorObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.

	/* selector property */
	char			*title;					// Selector 이름.
	int				item_count;				// 전체 선택 가능한 Item의 갯수.
	int				current_index;			// 현재 선택되어 있는 Item의 인덱스.
	int				input_index;			// input으로 동작하는 index
	char			**item;					// Item들의 배열.

	/* input property */
	int				digit;					// 입력받을 글자 수(자릿수).
	int				pos;					// 현재 입력받는 위치(자릿수).
	int				selectorType;			// 0(default) : *item 에 존재하는 string만 선택 가능.  1: Left, Right 키로 1씩 감소 혹은 증가.
	char			buf[MAX_INPUT_DIGIT+1];	// 현재까지 입력받은 내용

	/* UI property */
	int				item_x;
	int				item_w;

	ONDK_Color_t	focus_light;
	ONDK_Color_t	focus_face;
	ONDK_Color_t	focus_shadow;
	ONDK_Color_t	hilight_text;

	ONDK_Color_t	disable_light;
	ONDK_Color_t	disable_face;
	ONDK_Color_t	disable_shadow;
	ONDK_Color_t	disable_text;

	ONDK_Color_t	inactive_shadow;
	ONDK_Color_t	inactive_hilight;
};

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static struct tagInputSelectorObject	sInputSelectorArray[MAX_INPUTSELECTOR_OBJECT];
static char								isAllocated[MAX_INPUTSELECTOR_OBJECT];

/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */

static INPUTSELECTOR_OBJECT		local_gwminputselector_GetSelectorObject(int id)
{
	ONDK_GWM_GuiObject_t			objlist;
	INPUTSELECTOR_OBJECT	selector;

	GWM_assert(g_pstCurrentApp);

	objlist  = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);
	selector = (INPUTSELECTOR_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);

	return selector;
}


static INPUTSELECTOR_OBJECT		local_gwminputselector_AllocNewSelectorObj(void)
{
	int		i;

	for (i = 0; i < MAX_INPUTSELECTOR_OBJECT; i++)
	{
		if (!isAllocated[i])
			break;
	}
	if (i < MAX_INPUTSELECTOR_OBJECT)
	{
		isAllocated[i] = 1;
		return &sInputSelectorArray[i];
	}
	return NULL;
}

static void					local_gwminputselector_FreeSelectorObj(INPUTSELECTOR_OBJECT obj)
{
	int		i;

	GWM_assert(obj);
#ifdef CONFIG_DEBUG
	if (obj->link != NULL)
		GWM_DBG_Print(DBGMSG_OBJ, ("[ WARNING ] Object(%d):%08X is currently linked.\n", obj->ID, (int)obj));
#endif
	for (i = 0; i < MAX_INPUTSELECTOR_OBJECT; i++)
	{
		if (obj == &(sInputSelectorArray[i]))
			break;
	}

	if (i < MAX_INPUTSELECTOR_OBJECT)
	{
		isAllocated[i] = 0;
		INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(sInputSelectorArray[i]));
		sInputSelectorArray[i].title = NULL;				// Selector 이름.
		sInputSelectorArray[i].item_count = 0;				// 전체 선택 가능한 Item의 갯수.
		sInputSelectorArray[i].current_index = 0;			// 현재 선택되어 있는 Item의 인덱스.
		sInputSelectorArray[i].input_index = 0;
		sInputSelectorArray[i].item = NULL;					// Item들의 배열.
		sInputSelectorArray[i].digit = 0;					// 입력받을 글자 수(자릿수).
		sInputSelectorArray[i].pos = 0;						// 현재 입력받는 위치(자릿수).
		sInputSelectorArray[i].selectorType = 0;			// Selector LR 동작 모드. (default: eGUI_INPUTSELECTOROBJ_MODE_Normal)
		sInputSelectorArray[i].item_x	= 0;
		sInputSelectorArray[i].item_w	= 0;

		sInputSelectorArray[i].focus_light = 0xFFE8CE6F;
		sInputSelectorArray[i].focus_face = 0xFFDEB921;
		sInputSelectorArray[i].focus_shadow = 0xFFDBB106;
		sInputSelectorArray[i].hilight_text = 0xFF442900;

		sInputSelectorArray[i].disable_light = 0xFF33374A;
		sInputSelectorArray[i].disable_face = 0xFF33374A;
		sInputSelectorArray[i].disable_shadow = 0xFF303549;
		sInputSelectorArray[i].disable_text = 0xFF7A8698;

		sInputSelectorArray[i].inactive_shadow = 0xFF8A93A4;
		sInputSelectorArray[i].inactive_hilight = 0xFFCBD7E3;




		VK_memset(sInputSelectorArray[i].buf, 0x00, MAX_INPUT_DIGIT+1);
	}
	return;
}


static void			local_gwminputselector_defaultDrawSelectorMethod(INPUTSELECTOR_OBJECT select, ONDK_Region_t *area, ONDK_Rect_t *rect,
										char *title, char *value, int status, HBOOL isFrontMost)
{
	int		bright, face, shadow, fontcolor, left_more, right_more;
	int		length, x, y, w, h;

	ONDKFontHandle	font;
	ONDK_Color_t	col1, col2, col3, fontCol, leftCol, rightCol;
	ONDKSurfaceHandle		screen = ONDK_GWM_GetSurface();

	if (status & GUIOBJ_HILIGHTED)
	{

		bright = select.focus_light;
		face = select.focus_face;
		shadow = select.focus_shadow;
		fontcolor = select.hilighted_text;
	}
	else if (status & GUIOBJ_DISABLED)
	{
		bright = select.disabled_light;
		face = select.disabled_face;
		shadow = select.disabled_shadow;
		fontcolor = select.disabled_text;
	}
	else
	{
		bright = select.btn_light;
		face = select.btn_face;
		shadow = select.btn_shadow;
		fontcolor = select.default_text;
	}

	// 선택 가능한 화살표 표시
	if (status & SELECTOR_LEFT_NOMORE)
		left_more = select.inactive_shadow;
	else
		left_more = select.inactive_hilight;

	if (status & SELECTOR_RIGHT_NOMORE)
		right_more = select.inactive_shadow;
	else
		right_more = select.inactive_hilight;

	if (isFrontMost)
	{
		col1 = bright;
		col2 = face;
		col3 = shadow;
		fontCol = fontcolor;

		leftCol = left_more;
		rightCol = right_more;
	}
	else
	{
		col1 = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, bright, DEFAULT_DIMMED_LEVEL);
		col2 = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, face, DEFAULT_DIMMED_LEVEL);
		col3 = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, shadow, DEFAULT_DIMMED_LEVEL);
#if 0
		fontCol = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, fontcolor, DEFAULT_DIMMED_LEVEL);
#else
		fontCol = fontcolor;
#endif
		leftCol = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, left_more, DEFAULT_DIMMED_LEVEL);
		rightCol = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, right_more, DEFAULT_DIMMED_LEVEL);
	}

	w = rect->w;
	h = rect->h;
	ONDK_GWM_GFX_DrawFrame(screen, area->x1 + rect->x, area->y1 + rect->y, w, h, col1, col2, col3);

	font = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, DEFAULT_OBJ_FONT_SIZE);
	if (font != NULL)
	{
		y = (area->y1 + rect->y) + (h - font->height) / 2;
		x = area->x1 + rect->x;

		// 일단 제목 표시.
		OSD_GFX_DrawStringWithAbbr(screen, font, x + 12, y, w / 2 - 12, title, fontCol, ONDKSTF_LEFT);

		// 선택된 값의 표시.
		x = x + w / 2 + 6;
		w = w / 2 - 12 - 12;				// 12 은 오른쪽 끝 SPACE, 추가된 12는 양쪽 화살표의 크기

		length = ONDK_GWM_GFX_GetStringWidth(font, value);
		if (length > w )
		{
			OSD_GFX_DrawStringWithAbbr(screen, font, x, y, w, value, fontCol, ONDKSTF_LEFT);
		}
		else
		{
			x = x + (w - length) / 2;
			ONDK_FONT_DrawString(screen, font, x, y, value, fontCol, ONDKSTF_LEFT);
		}
	}
	ONDK_FONT_Release(font);
}


static	void	local_gwminputselector_CheckInput(INPUTSELECTOR_OBJECT selector)
{
	int		i, previous_index, nNum = 0;
	char	szBuf[MAX_INPUT_DIGIT+1] = {0,};

	if( selector->pos == 0 )
		return;	// 숫자 키 입력 모드가 아님!

	/*
	 * 입력된 숫자가 Valid한 값인지 확인한다. 즉, Selector Item 값들 중에 있는지
	 * 확인한다.
	 */
	nNum = atoi(selector->buf);
	snprintf(szBuf, MAX_INPUT_DIGIT+1, "%d", nNum);

	previous_index = selector->current_index;

	for( i=0 ; i<selector->item_count ; i++ )
	{
		if( MWC_UTIL_DvbStrcmp(szBuf, selector->item[i]) == 0 )
		{
			selector->current_index = i;

			break;
		}
	}

	/*
	 * 두 자리 이상의 숫자가 입력된 경우, 입력된 숫자가 Valid하지 않으면 마지막
	 * 숫자를 입력하기 전의 값이 Valid한지 확인한다.
	 */
	VK_memset(szBuf, 0, MAX_INPUT_DIGIT+1);
	nNum = atoi(selector->buf) / 10;
	snprintf(szBuf, MAX_INPUT_DIGIT+1, "%d", nNum);

	if( (selector->pos > 1) && (i == selector->item_count) )
	{
		for( i=0 ; i<selector->item_count ; i++ )
		{
			if( MWC_UTIL_DvbStrcmp(szBuf, selector->item[i]) == 0 )
			{
				selector->current_index = i;

				break;
			}
		}
	}

	selector->pos = 0;
	VK_memset(selector->buf, 0x00, MAX_INPUT_DIGIT+1);

	if( previous_index != selector->current_index )
	{
		ONDK_GWM_APP_Call(	g_pstCurrentApp, MSG_GWM_SELECTOR_CHANGED,
						(Handle_t)NULL, selector->ID, selector->current_index, (int)selector, NULL);
	}
#if	0
	else
	{
		ONDK_GWM_APP_Call(	g_pstCurrentApp, MSG_GWM_INPUTCANCELD,
						(Handle_t)NULL, selector->ID, selector->current_index, (int)selector, NULL);
	}
#endif
}

static	void local_gwminputselector_HandleNumKey(INPUTSELECTOR_OBJECT selector, char inputCh)
{
	int i;

	selector->buf[selector->pos] = inputCh;
	selector->pos++;

	if( selector->pos != 0 )
	{
		for( i=selector->pos; i<selector->digit; i++ )
		{
			selector->buf[i] = '-';
		}
	}
	if( selector->pos >= selector->digit )
	{
		selector->pos = 0;
		HxSTD_StrCpy((HUINT8 *)selector->item, (const HUINT8 *)selector->buf); //MWC_UTIL_DvbStrcpy 아이고...
		VK_memset(selector->buf, 0x00, MAX_INPUT_DIGIT+1);

		ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, selector->ID, (int)(selector->item), 0, NULL);
	}

	ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);

}

static	void	local_gwminputselector_HandleInput(INPUTSELECTOR_OBJECT selector, int key)
{
	int	nMax = 0, nCurNum = 0;

	switch (selector->digit)
	{
		case 1:	nMax = 9;		break;
		case 2:	nMax = 99;		break;
		case 3:	nMax = 999;		break;
		case 4:	nMax = 9999;		break;
		case 5:	nMax = 99999;	break;
		case 6:	nMax = 999999;	break;
		case 7:	nMax = 9999999;	break;
		case 8:	nMax = 99999999;	break;

		default:
			break;
	}

	nCurNum = atoi((const HUINT8 *)selector->item[selector->current_index]);

	if (key == KEY_ARROWLEFT)
	{
		if (nCurNum > 0)
			nCurNum--;
		else
			nCurNum = nMax;
	}
	else
	{
		if (nCurNum < nMax)
			nCurNum++;
		else
			nCurNum = 0;
	}

	HxSTD_snprintf(selector->item[selector->current_index], 16, "%d", nCurNum);

	ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);

	ONDK_GWM_APP_Call(	g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE,
					(Handle_t)NULL, selector->ID, (int)(selector->item), (int)selector, NULL);
}


static	ONDK_Result_t	local_gwminputselector_IndexedInput_Proc(GWM_Obj_MethodType_t proc_msg, INPUTSELECTOR_OBJECT obj, int arg)
{
	if (proc_msg == MT_LOST_FOCUS)
	{
		if (obj->current_index == obj->input_index)
		{
			obj->pos = obj->digit;
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_LOSE, (Handle_t)NULL, obj->ID, arg, 0, NULL);
		}
		ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);
		return GWM_MESSAGE_BREAK;
	}

	if (proc_msg == MT_KEYDOWN)
	{
		switch (arg)
		{
		case KEY_ARROWLEFT:
			if (obj->current_index == obj->input_index && obj->pos < obj->digit)
			{
				if (obj->pos == 0)
				{
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_LEFT_NOMORE, (Handle_t)NULL, obj->ID, obj->pos, (int)obj, NULL);
				}
				else
				{
					obj->buf[obj->pos--] = '\0';
					obj->buf[obj->pos] = (obj->pos == 0) ? '0' : '\0';
				}
				ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);
			}
			else if (obj->current_index > 0)
			{
				obj->current_index--;
				ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_CHANGED, (Handle_t)NULL, obj->ID, obj->current_index, (int)obj, NULL);
			}
			else
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_LEFT_NOMORE, (Handle_t)NULL, obj->ID, obj->item_count, (int)obj, NULL);
			}
			break;

		case KEY_ARROWRIGHT:
			if (obj->current_index < obj->item_count-1)
			{
				obj->current_index++;
				ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_CHANGED, (Handle_t)NULL, obj->ID, obj->current_index, (int)obj, NULL);
			}
			else
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_RIGHT_NOMORE, (Handle_t)NULL, obj->ID, obj->item_count, (int)obj, NULL);
			}
			break;

		case KEY_OK:
			if (obj->current_index == obj->input_index)
			{
				obj->pos = obj->digit;
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, obj->ID, (int)(obj->item), (int)obj, NULL);
			}
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_CLICKED, (Handle_t)NULL, obj->ID, obj->item_count, (int)obj, NULL);
			break;

		case KEY_0: case KEY_1: case KEY_2: case KEY_3: case KEY_4:
		case KEY_5: case KEY_6: case KEY_7: case KEY_8: case KEY_9:
			if (obj->current_index == obj->input_index)
			{
				if (obj->pos == 0 && arg == KEY_0)
				{
					break;
				}
				if (obj->pos == obj->digit)
				{
					obj->pos = 0;
				}

				obj->buf[obj->pos++] = '0' + arg - KEY_0;
				obj->buf[obj->pos] = '\0';
				if (obj->pos == obj->digit)
				{
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, obj->ID, (int)(obj->item), (int)obj, NULL);
				}
				ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);
			}
			break;

		default:
			return GWM_MESSAGE_PASS;
		}
		return GWM_MESSAGE_BREAK;
	}
	return GWM_MESSAGE_PASS;
}


static	ONDK_Result_t	local_gwminputselector_defaultSelectorProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	INPUTSELECTOR_OBJECT	selector;
	ONDK_Region_t				area;
	ONDK_Rect_t				rect;
	char					ch;
	ONDK_Result_t			eResult;
	ONDKSurfaceHandle		screen = ONDK_GWM_GetSurface();

	GWM_assert(object->type == GUIOBJ_INPUTSELECTOR);
	selector = (INPUTSELECTOR_OBJECT)object;

	switch (selector->selectorType)
	{
	case eGUI_INPUTSELECTOROBJ_MODE_IndexedInput:
		eResult = local_gwminputselector_IndexedInput_Proc(proc_msg, selector, arg);
		break;

	default:
		eResult = GWM_MESSAGE_PASS;
		break;
	}
	if (eResult == GWM_MESSAGE_BREAK)
	{
		return eResult;
	}

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwminputselector_FreeSelectorObj(selector);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			if (object->objDrawMethod != NULL)
			{	/* draw with customized drawing method */
				object->objDrawMethod(screen, &area, &rect, object, (INPUTSELECTOR_OBJECT)selector, arg);
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

				if(selector->pos != 0)
				{
					tmp = selector->buf;
				}
				else if(selector->item != NULL && selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_Normal)
				{
					tmp = selector->item[selector->current_index];
				}

				local_gwminputselector_defaultDrawSelectorMethod(selector, &area, &rect,
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
					if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_Normal)
					{
						local_gwminputselector_CheckInput(selector);
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
					}
					else if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_SPECIAL)
					{
						local_gwminputselector_HandleInput(selector, arg);
					}
					return GWM_MESSAGE_BREAK;
				}

				case KEY_ARROWRIGHT:
				{
					if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_Normal)
					{
						local_gwminputselector_CheckInput(selector);
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
					}
					else if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_SPECIAL)
					{
						local_gwminputselector_HandleInput(selector, arg);
					}
					return GWM_MESSAGE_BREAK;
				}

				case KEY_OK:
				{
					if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_Normal)
					{
						local_gwminputselector_CheckInput(selector);
						ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_CLICKED,
										(Handle_t)NULL, object->ID, selector->item_count, (int)selector, NULL);
						//GWM_APP_Call(g_pstCurrentApp, MSG_GWM_CLICKED,
						//				(Handle_t)NULL, object->ID, selector->item_count, (int)selector);
					}
					else if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_SPECIAL)
					{
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_SELECTOR_CLICKED,
										(Handle_t)NULL, object->ID, selector->item_count, (int)selector, NULL);
					}
					return GWM_MESSAGE_BREAK;
				}

				case KEY_0:
				case KEY_1:
				case KEY_2:
				case KEY_3:
				case KEY_4:
				case KEY_5:
				case KEY_6:
				case KEY_7:
				case KEY_8:
				case KEY_9:
				{
					if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_Normal)
					{
						ch = '0' + arg - KEY_0;

						selector->buf[selector->pos] = ch;
						selector->pos++;
						if( selector->pos >= selector->digit )
						{
							local_gwminputselector_CheckInput(selector);
						}
						ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
					}
					else if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_SPECIAL)
					{
						ch = '0' + arg - KEY_0;
						local_gwminputselector_HandleNumKey(selector, ch);

					}
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
			if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_Normal)
			{
				local_gwminputselector_CheckInput(selector);
			}
			else if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_SPECIAL)
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_LOSE, (Handle_t)NULL, object->ID, arg, 0, NULL);
				selector->pos = 0;
			}
			ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);
			break;
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}


void			gwm_inputselector_initClass(void)
{
	int		i;

	for (i = 0; i < MAX_INPUTSELECTOR_OBJECT; i++)
	{
		sInputSelectorArray[i].link = NULL;
		VK_memset(&(sInputSelectorArray[i].rect), 0, sizeof(ONDK_Rect_t));
		sInputSelectorArray[i].ID = -1;
		sInputSelectorArray[i].type = GUIOBJ_INPUTSELECTOR;
		sInputSelectorArray[i].focus = GUIOBJ_NORMAL;
		sInputSelectorArray[i].Proc = local_gwminputselector_defaultSelectorProc;
		sInputSelectorArray[i].objDrawMethod = NULL;

		sInputSelectorArray[i].title = NULL;
		sInputSelectorArray[i].item_count = 0;
		sInputSelectorArray[i].current_index = 0;
		sInputSelectorArray[i].input_index = 0;
		sInputSelectorArray[i].item = NULL;
		sInputSelectorArray[i].digit = 0;
		sInputSelectorArray[i].pos = 0;
		sInputSelectorArray[i].item_x	= 0;
		sInputSelectorArray[i].item_w	= 0;
		sInputSelectorArray[i].selectorType = eGUI_INPUTSELECTOROBJ_MODE_Normal;
		VK_memset(sInputSelectorArray[i].buf, 0x00, MAX_INPUT_DIGIT+1);
		isAllocated[i] = 0;
	}
}


/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

void					GWM_InputSelector_GetProperty(INPUTSELECTOR_OBJECT object,
													int *status, int *total, int *curr, int *digit, int *pos, char **title)
{
	if (status)
		*status = object->focus;
	if (title)
		*title = object->title;
	if (total)
		*total = object->item_count;
	if (curr)
		*curr = object->current_index;
	if (digit)
		*digit = object->digit;
	if (pos)
		*pos = object->pos;
}


ONDK_Result_t 		GWM_InputSelector_SetProperty(HINT32 id, char *title, int digit, int item_count, char **item_array)
{
	INPUTSELECTOR_OBJECT	pObj;

	if (title == NULL || digit == 0 || item_count == 0 || item_array == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	pObj = (INPUTSELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pObj->type != GUIOBJ_INPUTSELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	pObj->title	= title;
	pObj->digit	= digit;
	pObj->item_count	= item_count;
	pObj->item	= item_array;

	if (pObj->pos > pObj->digit)
		pObj->pos = 0;
	if (pObj->current_index >= pObj->item_count)
		pObj->current_index = 0;

	ONDK_GWM_APP_InvalidateRelativeRect(pObj->rect.x, pObj->rect.y, pObj->rect.w, pObj->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t			GWM_InputSelector_Create(int id, ONDK_Rect_t *rect, char *title, int digit,
													int item_count, char **item_array)
{
	INPUTSELECTOR_OBJECT	new_slct;
	ONDK_Result_t			hresult;

	new_slct = local_gwminputselector_GetSelectorObject(id);
	if (new_slct != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	if (digit > MAX_INPUT_DIGIT )
	{
		return GWM_RET_INITIAL_FAILED;
	}

	new_slct = local_gwminputselector_AllocNewSelectorObj();
	if (new_slct == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	new_slct->link = NULL;
	VK_memcpy(&(new_slct->rect), rect, sizeof(ONDK_Rect_t));
	new_slct->ID = id;
	new_slct->type = GUIOBJ_INPUTSELECTOR;
	new_slct->focus = GUIOBJ_NORMAL;
	new_slct->Proc = local_gwminputselector_defaultSelectorProc;
	new_slct->objDrawMethod = NULL;

	new_slct->title = title;
	new_slct->item_count = item_count;
	new_slct->current_index = 0;
	new_slct->item = item_array;
	new_slct->digit = digit;
	new_slct->pos = 0;
	new_slct->input_index	= item_count;
	new_slct->item_x	= 0;
	new_slct->item_w	= 0;
	new_slct->selectorType = eGUI_INPUTSELECTOROBJ_MODE_Normal;
	VK_memset(new_slct->buf, 0x00, MAX_INPUT_DIGIT+1);
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)new_slct, 0);

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_slct);
	ONDK_GWM_APP_InvalidateRelativeRect(	new_slct->rect.x,
						new_slct->rect.y,
						new_slct->rect.w,
						new_slct->rect.h);
	return hresult;
}


ONDK_Result_t			GWM_InputSelector_GetTotalItem(int id, int *number)
{
	INPUTSELECTOR_OBJECT	selector;

	selector = local_gwminputselector_GetSelectorObject(id);
	if (selector == NULL)
	{
		*number = 0;
		return GWM_RET_NO_OBJECT;
	}
	if (selector->type != GUIOBJ_INPUTSELECTOR)
	{
		*number = 0;
		return GWM_RET_NO_OBJECT;
	}

	*number = selector->item_count;
	return GWM_RET_OK;
}


int					GWM_InputSelector_GetCurrentIndex(int id)
{
	INPUTSELECTOR_OBJECT	selector;

	selector = local_gwminputselector_GetSelectorObject(id);
	if (selector == NULL)
	{
		return -1;
	}
	if (selector->type != GUIOBJ_INPUTSELECTOR)
	{
		return -1;
	}

	return selector->current_index;
}


ONDK_Result_t			GWM_InputSelector_SetCurrentIndex(int id, int nIndex)
{
	INPUTSELECTOR_OBJECT	selector;

	selector = local_gwminputselector_GetSelectorObject(id);
	if (selector == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (selector->type != GUIOBJ_INPUTSELECTOR)
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


HINT32		GWM_InputSelector_IsFocused(INPUTSELECTOR_OBJECT selector)
{
	GWM_assert(selector->type == GUIOBJ_INPUTSELECTOR);

	return selector->focus;
}


char *				GWM_InputSelector_GetSelectedItemTextPtr(INPUTSELECTOR_OBJECT selector)
{
	HUINT8	*pTmpStr = NULL;

	GWM_assert(selector->type == GUIOBJ_INPUTSELECTOR);
	GWM_assert(selector->current_index < selector->item_count);
	GWM_assert(selector->current_index >= 0);

	if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_Normal)
	{
		if( selector->pos != 0 )
		{
			pTmpStr = selector->buf;
		}
		else
		{
			pTmpStr = selector->item[selector->current_index];
		}
	}
	else if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_SPECIAL)
	{
		if( selector->pos != 0 )
		{
			pTmpStr = selector->buf;
		}
		else
		{
			pTmpStr = (char *)selector->item;
		}
	}
	else if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_IndexedInput)	// 위랑 아래랑 머가 다른거야 ??
	{
		if ( selector->current_index == selector->input_index )
		{
			pTmpStr	= selector->buf;
		}
		else
		{
			pTmpStr = selector->item[selector->current_index];
		}
	}

	return pTmpStr;
}


char *				GWM_InputSelector_GetCurItemTextPtr(INPUTSELECTOR_OBJECT selector)
{
	HUINT8	*pTmpStr = NULL;

	/* 숫자 키 입력 중인 동안에는 Item Array 내의 Text가 아닌 입력 중인 Text를 리턴함! */
	GWM_assert(selector->type == GUIOBJ_INPUTSELECTOR);

	if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_Normal)
	{
		if( selector->pos != 0 )
		{
			pTmpStr = selector->buf;
		}
		else
		{
			pTmpStr = selector->item[selector->current_index];
		}
	}
	else if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_SPECIAL)
	{
		if( selector->pos != 0 )
		{
			pTmpStr = selector->buf;
		}
		else
		{
			pTmpStr = (char *)selector->item;
		}
	}
	else if (selector->selectorType == eGUI_INPUTSELECTOROBJ_MODE_IndexedInput)
	{
		if ( selector->current_index == selector->input_index )
		{
			pTmpStr	= selector->buf;
		}
		else
		{
			pTmpStr = selector->item[selector->current_index];
		}
	}

	return pTmpStr;
}


char *				GWM_InputSelector_GetTitlePtr(INPUTSELECTOR_OBJECT selector)
{
	GWM_assert(selector->type == GUIOBJ_INPUTSELECTOR);

	return selector->title;
}


ONDK_Result_t		GWM_InputSelector_SetTitlePtr(int id, char *title)
{
	INPUTSELECTOR_OBJECT	selector;

	selector = local_gwminputselector_GetSelectorObject(id);
	if (selector == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (selector->type != GUIOBJ_INPUTSELECTOR)
	{
		return GWM_RET_NO_OBJECT;
	}

	selector->title	= title;

	return GWM_RET_OK;
}



int					GWM_InputSelector_GetSelectedIndex(INPUTSELECTOR_OBJECT selector)
{
	GWM_assert(selector->type == GUIOBJ_INPUTSELECTOR);

	return selector->current_index;
}


void					GWM_InputSelector_GetPosition(INPUTSELECTOR_OBJECT object, int *digit, int *pos)
{
	*digit = object->digit;
	*pos = object->pos;
}


ONDK_Result_t			GWM_InputSelector_CheckInput(int id)
{
	INPUTSELECTOR_OBJECT	selector;

	selector = local_gwminputselector_GetSelectorObject(id);
	if (selector == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	local_gwminputselector_CheckInput(selector);

	ONDK_GWM_APP_InvalidateRelativeRect(selector->rect.x, selector->rect.y, selector->rect.w, selector->rect.h);

	return ERR_OK;
}

ONDK_Result_t			GWM_InputSelector_SetSelectorType(int ID, int type)
{
	ONDK_GWM_GuiObject_t		objlist;
	INPUTSELECTOR_OBJECT	inselobj;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	if(type >= eGUI_INPUTSELECTOROBJ_MODE_MAX)
	{
		return GWM_RET_SOURCE_NULL;
	}

	inselobj = (INPUTSELECTOR_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, ID);
	if (inselobj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (inselobj->type != GUIOBJ_INPUTSELECTOR)
	{
		return GWM_RET_NO_OBJECT;
	}
	inselobj->selectorType = type;
	return GWM_RET_OK;
}


int	GWM_InputSelector_GetSelectorType(INPUTSELECTOR_OBJECT inselobj)
{
	GWM_assert(inselobj->type == GUIOBJ_INPUTSELECTOR);
	return inselobj->selectorType;
}



ONDK_Result_t	GWM_InputSelector_SetInputIndex (HINT32 id, HINT32 nIndex)
{
	INPUTSELECTOR_OBJECT	pObj;

	pObj = (INPUTSELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pObj->type != GUIOBJ_INPUTSELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	if (pObj->selectorType != eGUI_INPUTSELECTOROBJ_MODE_IndexedInput)
	{
		return ERR_FAIL;
	}

	if (nIndex < 0 || pObj->item_count <= nIndex)
	{
		return ERR_FAIL;
	}

	pObj->input_index	= nIndex;

	return GWM_RET_OK;
}


ONDK_Result_t	GWM_InputSelector_GetInputIndex (HINT32 id, HINT32 *pnIndex)
{
	INPUTSELECTOR_OBJECT	pObj;

	if (pnIndex == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	pObj = (INPUTSELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pObj->type != GUIOBJ_INPUTSELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	if (pObj->selectorType != eGUI_INPUTSELECTOROBJ_MODE_IndexedInput)
	{
		return ERR_FAIL;
	}

	*pnIndex	= pObj->input_index;
	return GWM_RET_OK;
}


ONDK_Result_t	GWM_InputSelector_SetInputValue (HINT32 id, const HUINT8 *szValue)
{
	ONDK_Rect_t	rc;
	INPUTSELECTOR_OBJECT	pObj;

	if (szValue == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	pObj = (INPUTSELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pObj->type != GUIOBJ_INPUTSELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	if (pObj->selectorType != eGUI_INPUTSELECTOROBJ_MODE_IndexedInput)
	{
		return ERR_FAIL;
	}

	pObj->pos	= pObj->digit;
	HxSTD_StrCpy(pObj->buf, szValue); //MWC_UTIL_DvbStrcpy... 아이고..


	ONDK_GWM_Obj_GetObjectRect(id, &rc);
	ONDK_GWM_APP_InvalidateRelativeRect(rc.x, rc.y, rc.w, rc.h);
	return GWM_RET_OK;
}


ONDK_Result_t	GWM_InputSelector_SetItemDimension (HINT32 id, HINT32 xoffset, HINT32 width)
{
	INPUTSELECTOR_OBJECT	pObj;
	ONDK_Rect_t	rc;

	pObj = (INPUTSELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pObj->type != GUIOBJ_INPUTSELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	pObj->item_x	= xoffset;
	pObj->item_w	= width;

	ONDK_GWM_Obj_GetObjectRect(id, &rc);
	ONDK_GWM_APP_InvalidateRelativeRect(rc.x, rc.y, rc.w, rc.h);

	return	GWM_RET_OK;
}


ONDK_Result_t	GWM_InputSelector_GetItemDimension (HINT32 id, HINT32 *xoffset, HINT32 *width)
{
	INPUTSELECTOR_OBJECT	pObj;

	if (xoffset == NULL || width == NULL)
	{
		return ERR_FAIL;
	}

	pObj = (INPUTSELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pObj->type != GUIOBJ_INPUTSELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	*xoffset	= pObj->item_x;
	*width		= pObj->item_w;
	return	GWM_RET_OK;
}


ONDK_Result_t	GWM_InputSelector_SetColor(HINT32 id,
		ONDK_Color_t focus_light, ONDK_Color_t focus_face, ONDK_Color_t focus_shadow, ONDK_Color_t hilight_text,
		ONDK_Color_t disable_light, ONDK_Color_t disable_face, ONDK_Color_t disable_shadow,
		ONDK_Color_t disable_text, ONDK_Color_t inactive_shadow, ONDK_Color_t inactive_hilight)
{
	/* 위의 코드는 object를 정리하면서 다시 수정할 예정*/
	INPUTSELECTOR_OBJECT	pObj;


	pObj = (INPUTSELECTOR_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pObj->type != GUIOBJ_INPUTSELECTOR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	pObj->focus_light = focus_light;
	pObj->focus_face = focus_face;
	pObj->focus_shadow = focus_shadow;
	pObj->hilight_text	= hilight_text;
	pObj->disable_light = disable_light;
	pObj->disable_face = disable_face;
	pObj->disable_shadow = disable_shadow;
	pObj->disable_text = disable_text;
	pObj->inactive_shadow = inactive_shadow;
	pObj->inactive_hilight	 = inactive_hilight;
	return GWM_RET_OK;
}


/* End of File. ---------------------------------------------------------- */
