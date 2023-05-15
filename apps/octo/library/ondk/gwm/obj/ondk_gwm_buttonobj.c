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

/**********************************************************************************************************/
#define ____STATIC_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

struct tagButtonObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.
	HUINT32							title_bufsize;
	HUINT8	*						title;
	HUINT16							size;
	HUINT8	*						extraText;

	ONDKSurfaceTextFlags 			align;
    ONDKSurfaceTextFlags 			extraTextAlign;

	ONDK_Color_t					color_over;
	ONDK_Color_t					color_normal;
	ONDK_Color_t					color_click;
	ONDK_Color_t					color_disable;
	ONDK_Color_t					color_fontcolor;
	ONDK_Color_t					color_fontfocus;
    ONDK_Color_t					color_fontDisable;

	HCHAR							*normalImgPath;
	HCHAR							*focusedImgPath;
	HCHAR							*disabledImgPath;
	HCHAR							*pushedImgPath;

	ONDKImageHandle					hNormalImg;
	ONDKImageHandle					hFocusedImg;
	ONDKImageHandle					hDisabledImg;
	ONDKImageHandle					hPushedImg;

	HBOOL 							simple; //버튼 테두리 없이 그리는 flag
	HUINT32                         dimmedAlphaLevel;
    ONDK_FontId_t                   fontId;
};


#define	KEY_SYNC_OFF	0
#define	KEY_SYNC_ON		1

STATIC HINT32 keySyncPair=KEY_SYNC_OFF;
STATIC struct	tagButtonObject			s_defButtonObj;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC BUTTON_OBJECT	local_gwmbtn_AllocNewButtonObj(void)
{
	struct tagButtonObject	*pObj = &s_defButtonObj;

	pObj = (struct tagButtonObject *)ONDK_Malloc(sizeof(struct tagButtonObject));
	ONDK_Assert(pObj);
	if (pObj == NULL)
		return NULL;

	HxSTD_MemCopy(pObj, &s_defButtonObj, sizeof(struct tagButtonObject));

	return pObj;
}

STATIC void		local_gwmbtn_FreeButtonObj(BUTTON_OBJECT obj)
{
	ONDK_Assert(obj);

	if(obj->hNormalImg)		ONDK_IMG_Destroy(obj->hNormalImg);
	if(obj->hFocusedImg)	ONDK_IMG_Destroy(obj->hFocusedImg);
	if(obj->hDisabledImg)	ONDK_IMG_Destroy(obj->hDisabledImg);
	if(obj->hPushedImg)		ONDK_IMG_Destroy(obj->hPushedImg);

	ONDK_Free(obj);
	obj = NULL;
}

/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC ONDK_Result_t	local_gwmbtn_DefaultMethod_Destory(BUTTON_OBJECT btn, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(btn);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&btn->rect);
	local_gwmbtn_FreeButtonObj(btn);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmbtn_DefaultMethod_Draw(BUTTON_OBJECT btn, HINT32 arg)
{
	HINT32		x=0, y=0, w=0, h=0, offset = 0, fw=0;
	HINT32		x1, y1;
	HINT32		push_status;
	HCHAR 		*title = NULL;
	HBOOL		simpleFlag;
	HBOOL 		isFrontMost = FALSE;
	ONDK_Color_t		btnColor=0;
	ONDK_Color_t		bright, face, shadow, fontcolor;
	ONDKFontHandle		*font;
	ONDK_Region_t		area;
	ONDK_Rect_t			rect, drawRect;
	ONDKSurfaceHandle 	screen = ONDK_GWM_GetCurSurface();
    HINT32      dimmedAlphaLevel = -1;
    ONDK_FontId_t       fontId;

    HCHAR 		*extraTitle = NULL;

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)btn, &rect);

	drawRect = ONDK_Rect(area.x1 + rect.x, area.y1 + rect.y, rect.w, rect.h);

	push_status = btn->focus;

	if (( push_status& GUIOBJ_DISABLED) && (btn->hDisabledImg != NULL))
	{
		ONDK_IMG_Blit(btn->hDisabledImg, screen, drawRect.x, drawRect.y);
		return ONDK_MESSAGE_BREAK;
	}
	else if ((push_status & GUIOBJ_PUSHED) && (btn->hPushedImg != NULL))
	{
		ONDK_IMG_Blit(btn->hPushedImg, screen, drawRect.x, drawRect.y);
		return ONDK_MESSAGE_BREAK;
	}
	else if ((push_status & GUIOBJ_HILIGHTED) && (btn->hFocusedImg != NULL))
	{
		ONDK_IMG_Blit(btn->hFocusedImg, screen, drawRect.x, drawRect.y);
		return ONDK_MESSAGE_BREAK;
	}
	else if (btn->hNormalImg!= NULL)
	{
		ONDK_IMG_Blit(btn->hNormalImg, screen, drawRect.x, drawRect.y);
		return ONDK_MESSAGE_BREAK;
	}

	x1 = drawRect.x;
	y1 = drawRect.y;
	w = rect.w;
	h = rect.h;


	isFrontMost = arg;

	fontcolor = btn->color_fontcolor;
	simpleFlag = btn->simple;
	title = btn->title;

    dimmedAlphaLevel = btn->dimmedAlphaLevel;
    fontId = btn->fontId;

    extraTitle = btn->extraText;

	if (push_status & GUIOBJ_PUSHED)					/* button feedback */
	{
		btnColor = btn->color_click;
	}
	else if (push_status & GUIOBJ_DISABLED)			/* disabled */
	{
		btnColor = btn->color_disable;
        fontcolor = btn->color_fontDisable;
	}
	else if (push_status & GUIOBJ_HILIGHTED)			/* get focused */
	{
		btnColor = btn->color_over;
		fontcolor = btn->color_fontfocus;
	}
	else												/* normal without a focus */
	{
		btnColor = btn->color_normal;
	}

  	bright 	= ONDK_GWM_GFX_GetSysDimmedColor(screen, btnColor, 150);
	face 	= btnColor;
	shadow 	= ONDK_GWM_GFX_GetSysDimmedColor(screen, btnColor, 50);

    if(dimmedAlphaLevel > -1)
    {
        bright = ONDK_GWM_GFX_GetSysDimmedColor(screen, btnColor, dimmedAlphaLevel);
    }


	if (FALSE == isFrontMost)
	{	/* deactivated or under-covered button status */
		bright 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, bright, DEFAULT_DIMMED_LEVEL);
		face 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, face,	 DEFAULT_DIMMED_LEVEL);
		shadow 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, shadow, DEFAULT_DIMMED_LEVEL);
	}
//ONDK_DRAW_FillRectElement(screen, x1, y1, w, h, 0xff2eb2ff);
	if(FALSE == simpleFlag) //번튼에 테두리 효과가 있으려면 해당 flag는 false
	{
		ONDK_GWM_GFX_DrawFrame(screen, x1, y1, w, h, bright, face, shadow);

		if (push_status & GUIOBJ_PUSHED)
		{
			ONDK_DRAW_RectElement(screen, x1, y1, w, h, shadow);
		}
	}
	else
	{
		ONDK_DRAW_FillRectElement(screen, x1, y1, w, h, bright);
	}

	if(NULL != title)
	{
		font = ONDK_FONT_CreateSystemFont(fontId, btn->size);
		if (font != NULL)
		{
		    fw = ONDK_GWM_GFX_GetStringWidth(font, "0");
		    if(NULL != extraTitle)
            {
                x = x1 + (fw);
    			w = w - (fw*2);
            }
            else
            {
                if(btn->align & ONDKSTF_CENTER)
                {
                    x = x1;
                    w = w;
                }
                else
                {
                    x = x1 + (fw/2);
                    w = w - (fw*2);
                }
            }

			y = y1 + (h/2 - ONDK_FONT_GetHeight(font)/2) + offset;
			ONDK_FONT_DrawStringAlignAbbr(screen, font, title, x, y, w, fontcolor, btn->align);
		}
		ONDK_FONT_Release(font);
	}

    if(NULL != extraTitle && ONDKSTF_TextFlags_MAX != btn->extraTextAlign){
        font = ONDK_FONT_CreateSystemFont(fontId, btn->size);
		if (font != NULL)
		{
		    fw = ONDK_GWM_GFX_GetStringWidth(font, "0");
            x = x1 + (fw);
   			w = w - (fw/2);
			y = y1 + (h/2 - ONDK_FONT_GetHeight(font)/2) + offset;
			ONDK_FONT_DrawStringAlignAbbr(screen, font, extraTitle, x, y, w, fontcolor, btn->extraTextAlign);
		}
		ONDK_FONT_Release(font);
    }

	return ONDK_MESSAGE_BREAK;
}


STATIC ONDK_Result_t	local_gwmbtn_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t	lRet = ONDK_RET_OK;

	BUTTON_OBJECT		btn;

	GWM_assert(object);
	GWM_assert(object->type == GUIOBJ_BUTTON);
	btn = (BUTTON_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
			lRet = local_gwmbtn_DefaultMethod_Destory((BUTTON_OBJECT)btn, arg);
			return GWM_MESSAGE_BREAK;
		case MT_DRAW:

			if (btn->objDrawMethod == NULL)
			{
				lRet = local_gwmbtn_DefaultMethod_Draw((BUTTON_OBJECT)btn, arg);
			}
			break;
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			(void)ONDK_GWM_APP_InvalidateRelativeRect(btn->rect.x, btn->rect.y, btn->rect.w, btn->rect.h);
			VK_memcpy(&(btn->rect), new_rect, sizeof(ONDK_Region_t));
			(void)ONDK_GWM_APP_InvalidateRelativeRect(btn->rect.x, btn->rect.y, btn->rect.w, btn->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_CHANGE_TEXT:
		{
			HCHAR 	*new_text;
			new_text = (HCHAR *)arg;
			btn->title= new_text;
			(void)ONDK_GWM_APP_InvalidateRelativeRect(btn->rect.x, btn->rect.y, btn->rect.w, btn->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_KEYDOWN:
		{
			if (arg == KEY_OK || arg == KEY_FRONT_OK)
			{
				keySyncPair=KEY_SYNC_ON;
				btn->focus |= GUIOBJ_PUSHED;
				(void)ONDK_GWM_APP_InvalidateRelativeRect(btn->rect.x, btn->rect.y, btn->rect.w, btn->rect.h);
				return ONDK_MESSAGE_CONSUMED;
			}
			return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
		}
		case MT_KEYUP:
			if ( ( arg == KEY_OK || arg == KEY_FRONT_OK )&&keySyncPair==KEY_SYNC_ON)
			{
				keySyncPair=KEY_SYNC_OFF;
				btn->focus &= ~GUIOBJ_PUSHED;
				(void)ONDK_GWM_APP_InvalidateRelativeRect(btn->rect.x, btn->rect.y, btn->rect.w, btn->rect.h);

				/* current app usually doesn't need specific session handle fo key-down event */
				(void)ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_CLICKED, (Handle_t)NULL, btn->ID, arg, (HINT32)btn, NULL);
				return ONDK_MESSAGE_CONSUMED;
			}
			return GWM_MESSAGE_BREAK;

		case MT_GET_FOCUS:
			(void)ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_GET, (Handle_t)NULL, btn->ID, arg, (HINT32)btn, NULL);
			(void)ONDK_GWM_APP_InvalidateRelativeRect(btn->rect.x, btn->rect.y, btn->rect.w, btn->rect.h);
			break;

		case MT_LOST_FOCUS:
			keySyncPair=KEY_SYNC_OFF;
			btn->focus &= ~GUIOBJ_PUSHED;
			(void)ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_LOSE, (Handle_t)NULL, btn->ID, arg, (HINT32)btn, NULL);
			(void)ONDK_GWM_APP_InvalidateRelativeRect(btn->rect.x, btn->rect.y, btn->rect.w, btn->rect.h);
			break;

		default:
			break;
	}

	return INT_ONDK_GWM_OBJ_BaseDefaultMethod(proc_msg, object, arg);
}


/**********************************************************************************************************/
#define ____INTERNAL_FUNCTIONS_______________________
/**********************************************************************************************************/
void	INT_ONDK_GWM_COM_Button_InitClass(void)
{
	struct tagButtonObject *pObj = &s_defButtonObj;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagButtonObject));

	pObj->link = NULL;
	pObj->ID = -1;
	pObj->type = GUIOBJ_BUTTON;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwmbtn_DefaultMethod;
	pObj->objDrawMethod = NULL;

	pObj->title_bufsize = 0;
	pObj->title = NULL;
	pObj->extraText = NULL;
	pObj->align = ONDKSTF_LEFT;

	pObj->hNormalImg = NULL;
	pObj->hFocusedImg = NULL;
	pObj->hDisabledImg = NULL;
	pObj->hPushedImg = NULL;

	pObj->color_over 		= 0xFF0000C0;
	pObj->color_normal 		= 0xFF000080;
	pObj->color_click 		= 0xFF0000FF;
	pObj->color_disable		= 0xFF000040;
	pObj->color_fontcolor 	= 0xFF820610;
	pObj->color_fontfocus	= 0xFF820610;
    pObj->color_fontDisable = 0xFF414141;

	pObj->simple			= FALSE;
    pObj->dimmedAlphaLevel  = -1;
    pObj->fontId            = eFont_SystemNormal;

    pObj->extraTextAlign = ONDKSTF_TextFlags_MAX;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t	ONDK_GWM_COM_Button_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title)
{
	ONDK_GWM_GuiObject_t		objlist;
	BUTTON_OBJECT		new_btn;
	ONDK_Result_t			hresult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_btn = (BUTTON_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_btn != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_btn = local_gwmbtn_AllocNewButtonObj();
	if (new_btn == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	HxSTD_MemCopy(&(new_btn->rect), rect, sizeof(ONDK_Rect_t));
	new_btn->ID = id;
	new_btn->title = title;

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_btn);
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&new_btn->rect);


	return hresult;
}

ONDK_Result_t	ONDK_GWM_COM_Button_ChangeImage(HINT32 id, HCHAR *normal, HCHAR *focused,  HCHAR *disabled, HCHAR *pushed)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;


	if(normal != NULL)
	{
		if(obj->normalImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->normalImgPath);
		}
		obj->normalImgPath = normal;
		obj->hNormalImg = ONDK_IMG_CreateSelf(normal);
	}

	if(focused != NULL)
	{
		if(obj->focusedImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->focusedImgPath);
		}
		obj->focusedImgPath = focused;
		obj->hFocusedImg= ONDK_IMG_CreateSelf(focused);
	}

	if(disabled != NULL)
	{
		if(obj->disabledImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->disabledImgPath);
		}

		obj->disabledImgPath = disabled;
		obj->hDisabledImg= ONDK_IMG_CreateSelf(disabled);
	}

	if(pushed != NULL)
	{
		if(obj->pushedImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->pushedImgPath);
		}
		obj->pushedImgPath = pushed;
		obj->hPushedImg= ONDK_IMG_CreateSelf(pushed);
	}

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_COM_Button_GetImage(HINT32 id, HCHAR **normal, HCHAR **focused, HCHAR **disabled, HCHAR **pushed)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	*normal = obj->normalImgPath;
	*focused = obj->focusedImgPath;
	*disabled = obj->disabledImgPath;
	*pushed = obj->pushedImgPath;

	return ONDK_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_GetImageByObj(BUTTON_OBJECT obj, HCHAR **normal, HCHAR **focused, HCHAR **disabled, HCHAR **pushed)
{
	if (obj == NULL)
		return	GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return	GWM_RET_NO_OBJECT;

	*normal = obj->normalImgPath;
	*focused = obj->focusedImgPath;
	*disabled = obj->disabledImgPath;
	*pushed = obj->pushedImgPath;

	return ONDK_RET_OK;
}

ONDK_Result_t				ONDK_GWM_COM_Button_GetImageHandleByObj(BUTTON_OBJECT obj,ONDKImageHandle *normal,ONDKImageHandle *focused, ONDKImageHandle *disabled, ONDKImageHandle *pushed)
{
	if(obj == NULL)
		return GWM_RET_NO_OBJECT;
	if(obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	*normal = obj->hNormalImg;
	*focused = obj->hFocusedImg;
	*disabled = obj->hDisabledImg;
	*pushed = obj->hPushedImg;

	return ONDK_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_SetButtonColor(HINT32 id, ONDK_Color_t over, ONDK_Color_t normal, ONDK_Color_t click, ONDK_Color_t disable)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	obj->color_over = over;
	obj->color_normal = normal;
	obj->color_click = click;
	obj->color_disable = disable;
	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_SetFontColor(HINT32 id, ONDK_Color_t fontColor, ONDK_Color_t fontfocus)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	obj->color_fontcolor = fontColor;
	obj->color_fontfocus = fontfocus;
	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_SetStatus(HINT32 id, HINT32 push_status)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	obj->focus &= ~GUIOBJ_PUSHED;
	obj->focus &= ~GUIOBJ_HILIGHTED;
	obj->focus &= ~GUIOBJ_DISABLED;
	obj->focus &= ~GUIOBJ_NORMAL;

#if 0
	if (push_status & GUIOBJ_NORMAL)
	{
	}
#endif

	if(push_status & GUIOBJ_HILIGHTED)
	{
		obj->focus |= GUIOBJ_HILIGHTED;
	}
	if(push_status & GUIOBJ_PUSHED)
	{
		obj->focus |= GUIOBJ_PUSHED;
	}
	if(push_status & GUIOBJ_DISABLED)
	{
		obj->focus |= GUIOBJ_DISABLED;
	}

	//INT_ONDK_GWM_OBJ_CallObjectProc(MT_DRAW, (GWM_GuiObject_t)obj, 0);
	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

HCHAR *		ONDK_GWM_COM_Button_GetExtraTextByObj(BUTTON_OBJECT btn)
{
	GWM_assert(btn->type == GUIOBJ_BUTTON);
	return btn->extraText;
}


ONDK_Result_t	ONDK_GWM_COM_Button_SetExtraText(HINT32 id, HUINT8 *text)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	GWM_assert(obj->type == GUIOBJ_BUTTON);

	if(text==NULL)
		return GWM_RET_INITIAL_FAILED;

	obj->extraText=text;

	return GWM_RET_OK;
}

HCHAR *		ONDK_GWM_COM_Button_GetTitle(HINT32 id)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return NULL;
	if (obj->type != GUIOBJ_BUTTON)
		return NULL;

	GWM_assert(obj->type == GUIOBJ_BUTTON);

	return obj->title;
}


HCHAR *		ONDK_GWM_COM_Button_GetTitleByObj(BUTTON_OBJECT btn)
{
	GWM_assert(btn->type == GUIOBJ_BUTTON);
	return btn->title;
}


HUINT32		ONDK_GWM_COM_Button_GetStatusByObj(BUTTON_OBJECT btn)
{
	GWM_assert(btn->type == GUIOBJ_BUTTON);
	return btn->focus;
}

ONDK_Result_t	ONDK_GWM_COM_Button_ChangeTitle (HINT32 id, HCHAR *szNewText)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	if (szNewText)
	{
		obj->title = szNewText;
	}

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_ChangeTitleWithoutUpdate (HINT32 id, HCHAR *szNewText)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	if (szNewText)
	{
		obj->title = szNewText;
	}

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_SetTitleSize(HINT32 id, HUINT16 size)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	obj->size=size;

	//INT_ONDK_GWM_OBJ_CallObjectProc(MT_DRAW, (GWM_GuiObject_t)obj, size);
	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_SetAlign(HINT32 id, ONDKSurfaceTextFlags align)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	obj->align=align;

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_SetExtraAlign(HINT32 id, ONDKSurfaceTextFlags align)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	obj->extraTextAlign=align;

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_COM_Button_GetTitleSize(HINT32 id, HUINT16 *size)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	*size = obj->size;

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_GetTitleSizeByObj(BUTTON_OBJECT	obj, HUINT16 *size)
{
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	*size = obj->size;

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_SetSimpleButton(HINT32 id, HBOOL simple)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	obj->simple = simple;
	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_SetDisableButton(HINT32 id)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	obj->focus = GUIOBJ_DISABLED;
	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_SetButtonDimmedAlpha(HINT32 id,HINT32 level)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	obj->dimmedAlphaLevel = level;
	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Button_SetFont(HINT32 id,ONDK_FontId_t fontId)
{
	BUTTON_OBJECT		obj;

	obj = (BUTTON_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BUTTON)
		return GWM_RET_NO_OBJECT;

	obj->fontId = fontId;
	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

/* End of File. ---------------------------------------------------------- */
