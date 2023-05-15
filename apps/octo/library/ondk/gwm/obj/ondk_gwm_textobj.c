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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <ondk.h>
#include <ondk_gfx.h>
#include "../ondk_gwm_objdef.h"
#include "../ondk_gwm_internal.h"

/**********************************************************************************************************/
#define ____STATIC_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#define	MAX_TEXT_OBJECT				64
#define	LINE_SPACE						8			// MULTI LINE TEXT 의 줄간격

struct tagTextObject
{
	GUIOBJ_BASIC_PROPERTY					/**< GUI object 기본 속성들. 반드시 맨 앞에 존재하도록 할 것. */
	HCHAR						*text;		/**< string values. */
	ONDK_Color_t				color;		/**< font color */
	ONDK_Color_t				backcolor;	/**< text rect color */
	HINT32						align;		/**< text alignment in text rect */

	HINT32						size;		/**< font size, in pixel unit */
	ONDK_FontId_t 				font;
};

STATIC struct	tagTextObject			s_defTextObj;


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC TEXT_OBJECT	local_gwmtext_AllocNewTextObj(void)
{
	struct tagTextObject	*pObj = &s_defTextObj;

	pObj = (struct tagTextObject *)ONDK_Malloc(sizeof(struct tagTextObject));
	ONDK_Assert(pObj);
	if (pObj == NULL)
		return NULL;

	HxSTD_MemCopy(pObj, &s_defTextObj, sizeof(struct tagTextObject));

	return pObj;
}

STATIC void			local_gwmtext_FreeTextObj(TEXT_OBJECT obj)
{
	ONDK_Assert(obj);

	ONDK_Free(obj);
}

/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC ONDK_Result_t	local_gwmtext_DefaultMethod_Destory(TEXT_OBJECT			textObj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(textObj);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&textObj->rect);

	local_gwmtext_FreeTextObj(textObj);

	return ONDK_MESSAGE_BREAK;
}



STATIC ONDK_Result_t	local_gwmtext_DefaultMethod_Draw(TEXT_OBJECT textObj, HINT32 arg)
{
	ONDK_Region_t		area;
	ONDK_Rect_t			rect;
	HINT32				x, y, w, h;
	ONDKSurfaceHandle	screen = ONDK_GWM_GetCurSurface();
	ONDK_Color_t		foreCol, backCol;
	ONDKFontHandle		*pFont = NULL;
	ONDKSurfaceTextFlags textflag = 0;

	ONDK_UNUSED_ARGUMENT(textObj);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)textObj, &rect);

	/* draw with default method */
	x = area.x1 + textObj->rect.x;
	y = area.y1 + textObj->rect.y;
	w = textObj->rect.w;
	h = textObj->rect.h;

	foreCol = textObj->color;
	backCol = textObj->backcolor;

	if(textObj->text == NULL)
	{
		return GWM_MESSAGE_BREAK;
	}

	pFont = ONDK_FONT_CreateSystemFont(textObj->font, textObj->size);
	if (pFont == NULL)
	{
		return GWM_MESSAGE_BREAK;
	}

    if(textObj->align & TEXT_WORDWRAP){
        textflag |= ONDKSTF_WORDWRAP;
    }

	if (textObj->align & TEXT_MULTILINED)
	{
		if (textObj->align & TEXT_ALIGN_CENTER) 	textflag |= ONDKSTF_CENTER;
		else if (textObj->align & TEXT_ALIGN_RIGHT) textflag |= ONDKSTF_RIGHT;

		if (textObj->align & TEXT_ALIGN_BOTTOM) 	textflag |= ONDKSTF_BOTTOM;

		ONDK_FONT_DrawMultiLineAlign(screen, pFont, x, y, w, h, textObj->text, LINE_SPACE, foreCol, textflag);
	}
	else
	{
		if (textObj->align & TEXT_ALIGN_CENTER) 	textflag |= ONDKSTF_CENTER;
		else if (textObj->align & TEXT_ALIGN_RIGHT) textflag |= ONDKSTF_RIGHT;

		if (textObj->align & TEXT_ALIGN_MIDDLE)
		{
			y += ((h - (ONDK_FONT_GetHeight(pFont))) / 2);
		}
		else if(textObj->align & TEXT_ALIGN_BOTTOM)
		{
			y += ((h - (ONDK_FONT_GetHeight(pFont))));
		}

		ONDK_FONT_DrawStringAlignAbbr(screen, pFont, textObj->text, x, y, w, foreCol, textflag);
	}

	ONDK_FONT_Release(pFont);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmtext_DefaultMethod_ChangeText(TEXT_OBJECT			textObj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(textObj);
	ONDK_UNUSED_ARGUMENT(arg);

	textObj->text = (HCHAR *)arg;
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&textObj->rect);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmtext_DefaultMethod_SetValue(TEXT_OBJECT			textObj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(textObj);
	ONDK_UNUSED_ARGUMENT(arg);

	textObj->color = arg;
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&textObj->rect);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmtext_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t 		lRet = ONDK_MESSAGE_OK;

	GWM_assert(object->type == GUIOBJ_TEXT);

	switch (proc_msg)
	{
	case MT_DESTROY:
		lRet = local_gwmtext_DefaultMethod_Destory((TEXT_OBJECT)object, arg);
		break;

	case MT_DRAW:
		if (NULL == object->objDrawMethod)
		{
			lRet = local_gwmtext_DefaultMethod_Draw((TEXT_OBJECT)object, arg);
		}
		break;

	case MT_CHANGE_TEXT:
		lRet = local_gwmtext_DefaultMethod_ChangeText((TEXT_OBJECT)object, arg);
		break;

	case MT_SET_VALUE:
		lRet = local_gwmtext_DefaultMethod_SetValue((TEXT_OBJECT)object, arg);
		break;

	default:
		break;
	}

	if(ONDK_MESSAGE_OK != lRet)
	{
		return lRet;
	}

	return INT_ONDK_GWM_OBJ_BaseDefaultMethod(proc_msg, object, arg);
}

/**********************************************************************************************************/
#define ____INTERNAL_FUNCTIONS_______________________
/**********************************************************************************************************/

void			INT_ONDK_GWM_COM_Text_Init(void)
{
	struct tagTextObject *pObj = &s_defTextObj;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagTextObject));

	pObj->link = NULL;
	VK_memset(&(pObj->rect), 0, sizeof(ONDK_Rect_t));
	pObj->ID = -1;
	pObj->type = GUIOBJ_TEXT;
	pObj->focus = GUIOBJ_DISABLED;
	pObj->Proc = local_gwmtext_DefaultMethod;
	pObj->objDrawMethod = NULL;

	pObj->text = NULL;
	pObj->color = 0xFF000000;
	pObj->backcolor = 0;
	pObj->align = TEXT_ALIGN_LEFT;
	pObj->font = eFont_SystemNormal;
	pObj->size = DEFAULT_OBJ_FONT_SIZE;
	pObj->isHide = FALSE;
}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/


ONDK_Result_t			ONDK_GWM_COM_Text_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *text)
{
	ONDK_GWM_GuiObject_t	objList;
	TEXT_OBJECT				new_textObj;
	ONDK_Result_t			lRet;

	GWM_assert(g_pstCurrentApp);
	objList = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_textObj = (TEXT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objList, id);
	if (new_textObj != NULL)
		return GWM_RET_LINK_CONFLICT;

	new_textObj = local_gwmtext_AllocNewTextObj();
	if (new_textObj == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	HxSTD_MemCopy(&(new_textObj->rect), rect, sizeof(ONDK_Rect_t));
	new_textObj->ID = id;
	new_textObj->text = text;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)new_textObj, 0);

	lRet = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_textObj);

	return lRet;
}


void					ONDK_GWM_COM_Text_GetProperty(TEXT_OBJECT object,
								ONDK_Color_t *font_color, ONDK_Color_t *bg_color, HUINT32 *align, HUINT32 *size)
{
	GWM_assert(object->type == GUIOBJ_TEXT);

//	*pFont = object->font;
	*font_color = object->color;
	*bg_color = object->backcolor;
	*align = (HUINT32)object->align;
	*size = (HUINT32)object->size;
}


HUINT8 *				ONDK_GWM_COM_Text_GetPtrByObject(TEXT_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_TEXT);
	return obj->text;
}


ONDK_Color_t			ONDK_GWM_COM_Text_GetForeColorByObject(TEXT_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_TEXT);
	return obj->color;
}

ONDK_Color_t			ONDK_GWM_COM_Text_GetBackColorByObject(TEXT_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_TEXT);
	return obj->backcolor;
}

HINT32				ONDK_GWM_COM_Text_GetSizeByObject(TEXT_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_TEXT);
	return obj->size;
}

HINT32				ONDK_GWM_COM_Text_GetAlignByObject(TEXT_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_TEXT);
	return obj->align;
}

ONDK_Result_t			ONDK_GWM_COM_Text_ChangeColor(HINT32 id, ONDK_Color_t color, ONDK_Color_t background)
{
	TEXT_OBJECT		obj;

	obj = (TEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	switch(obj->type)
	{
	case GUIOBJ_TEXT:
	case GUIOBJ_MULTITEXT:
	case GUIOBJ_EXTTEXT:
		break;
	default:
		return GWM_RET_NO_OBJECT;
	}

	obj->backcolor 	= background;
	obj->color		= color;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_COM_Text_SetSize(HUINT32 id, HINT32 size)
{
	TEXT_OBJECT	obj;

	obj = (TEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	switch(obj->type)
	{
	case GUIOBJ_TEXT:
	case GUIOBJ_MULTITEXT:
	case GUIOBJ_EXTTEXT:
		break;
	default:
		return GWM_RET_NO_OBJECT;
	}

	obj->size = (HINT32)size;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);


	return GWM_RET_OK;
}

ONDK_Result_t			ONDK_GWM_COM_Text_SetAlign(HINT32 id, HINT32 align)
{
	TEXT_OBJECT		obj;

	obj = (TEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	switch(obj->type)
	{
	case GUIOBJ_TEXT:
	case GUIOBJ_MULTITEXT:
	case GUIOBJ_EXTTEXT:
		break;
	default:
		return GWM_RET_NO_OBJECT;
	}

	obj->align = align;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}

ONDK_Result_t		ONDK_GWM_COM_Text_SetText(HINT32 id, HCHAR *str)
{

	TEXT_OBJECT		obj;

	obj = (TEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	switch(obj->type)
	{
	case GUIOBJ_TEXT:
	case GUIOBJ_MULTITEXT:
	case GUIOBJ_EXTTEXT:
		break;
	default:
		return GWM_RET_NO_OBJECT;
	}

	obj->text=str;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Text_SetFont (HINT32 id, ONDK_FontId_t fontId)
{
	TEXT_OBJECT		obj;

	obj = (TEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	switch(obj->type)
	{
	case GUIOBJ_TEXT:
	case GUIOBJ_MULTITEXT:
	case GUIOBJ_EXTTEXT:
		break;
	default:
		return GWM_RET_NO_OBJECT;
	}

	obj->font= fontId;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}

ONDK_FontId_t	ONDK_GWM_COM_Text_GetFont (HINT32 id)
{
	TEXT_OBJECT		obj;

	obj = (TEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	switch(obj->type)
	{
	case GUIOBJ_TEXT:
	case GUIOBJ_MULTITEXT:
	case GUIOBJ_EXTTEXT:
		break;
	default:
		return GWM_RET_NO_OBJECT;
	}

	return obj->font;
}


HINT32	ONDK_GWM_COM_Text_GetStringWidth(ONDK_FontId_t eFontId, HINT32 lFontsize, HCHAR *str)
{
	HINT32				lStrWidth = 0;
	ONDKFontHandle		*pFont = NULL;

	pFont = ONDK_FONT_CreateSystemFont(eFontId, lFontsize);
	if (pFont == NULL)
	{
		return 0;
	}

	lStrWidth = ONDK_FONT_GetStringWidth(pFont, str);

	ONDK_FONT_Release(pFont);

	return	lStrWidth;
}

ONDK_Result_t			ONDK_GWM_COM_Text_ChangeLocation(HINT32 id, const ONDK_Rect_t *rect)
{
	TEXT_OBJECT		obj;
	ONDK_Rect_t		rt;

	obj = (TEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	rt = ONDK_UnionRect (&obj->rect, rect);
	obj->rect = *rect;

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&rt);

	return GWM_RET_OK;
}



/* End of File. ---------------------------------------------------------- */


