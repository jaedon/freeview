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

struct tagSlideTextObject
{
	GUIOBJ_BASIC_PROPERTY		/**< GUI object 기본 속성들. 반드시 맨 앞에 존재하도록 할 것. */
	HCHAR			*text;		/**< string values. */
	HCHAR			*drawText;
	ONDK_Color_t	color;		/**< font color */
	HINT32			align;		/**< text alignment in text rect */
	HINT32			curPosX;
	HINT32			startPosX;

	HINT32			size;		/**< font size, in pixel unit */
	ONDK_FontId_t 	font;

	ONDKImageHandle	hBgImage;
	ONDK_Color_t	bgColor;
};

STATIC struct	tagSlideTextObject			s_defSlideTextObj;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC SLIDETEXT_OBJECT	local_gwmslidetext_AllocNewSlideTextObj(void)
{
	struct tagSlideTextObject	*pObj = &s_defSlideTextObj;

	pObj = (struct tagSlideTextObject *)ONDK_Calloc(sizeof(struct tagSlideTextObject));
	ONDK_Assert(pObj);
	if (pObj == NULL)
		return NULL;

	HxSTD_MemCopy(pObj, &s_defSlideTextObj, sizeof(struct tagSlideTextObject));

	return pObj;
}

STATIC void			local_gwmslidetext_FreeSlideTextObj(SLIDETEXT_OBJECT obj)
{
	ONDK_Assert(obj);

	if(NULL != obj->hBgImage)
	{
		ONDK_IMG_Destroy(obj->hBgImage);
		obj->hBgImage = NULL;
	}

	ONDK_Free(obj);
}

STATIC void			local_gwmslidetext_ClearPositionData(SLIDETEXT_OBJECT obj)
{
	ONDK_Assert(obj);

	obj->curPosX = obj->startPosX;
	obj->drawText = NULL;
}


/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC ONDK_Result_t	local_gwmslidetext_DefaultMethod_Destroy(SLIDETEXT_OBJECT textObj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(textObj);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&textObj->rect);
	local_gwmslidetext_FreeSlideTextObj(textObj);

	return GWM_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmslidetext_DefaultMethod_Draw(SLIDETEXT_OBJECT textObj, HINT32 arg)
{
	ONDK_Region_t			area;
	HBOOL					bIsReset = FALSE;
	HINT32					x, y, w, h, tx, tw;
	ONDKSurfaceHandle		screen = ONDK_GWM_GetCurSurface();
	ONDKFontHandle			*pFont = NULL;
	ONDKSurfaceTextFlags 	textflag = 0;
	HUINT32 				ulUsedBytes = 0;
	HINT32					nCharWidth = 0;
	HINT32					txtWidth = 0;

	ONDK_UNUSED_ARGUMENT(textObj);
	ONDK_UNUSED_ARGUMENT(arg);


	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);

	if (textObj->objDrawMethod != NULL)
	{	/* draw with customized drawing method */
		return textObj->objDrawMethod(screen, &area, &textObj->rect, (ONDK_GWM_GuiObject_t)textObj, (SLIDETEXT_OBJECT)textObj, arg);
	}

	x = area.x1 + textObj->rect.x;
	y = area.y1 + textObj->rect.y;
	w = textObj->rect.w;
	h = textObj->rect.h;

	if(textObj->text == NULL)
	{
		return GWM_MESSAGE_BREAK;
	}

	if(0 != COL_A(textObj->bgColor))
	{
		ONDK_DRAW_FillRect (screen, ONDK_Rect(x,y,w,h), textObj->bgColor);
	}
	if(NULL != textObj->hBgImage)
	{
		ONDK_IMG_StretchBlit(textObj->hBgImage, screen, ONDK_Rect(x,y,w,h));
	}

	pFont = ONDK_FONT_CreateSystemFont(textObj->font, textObj->size);
	if (pFont == NULL)
	{
		return GWM_MESSAGE_BREAK;
	}

	if (textObj->align & TEXT_ALIGN_CENTER) 	textflag |= ONDKSTF_CENTER;
	else if (textObj->align & TEXT_ALIGN_RIGHT) textflag |= ONDKSTF_RIGHT;

	if (textObj->align & TEXT_ALIGN_BOTTOM) 	textflag |= ONDKSTF_BOTTOM;

	tx = MAX((x-textObj->curPosX), 0);
	tw = MIN((GWM_SCREEN_WIDTH-tx), (w+10));

	if(NULL == textObj->drawText)
	{
		textObj->drawText = textObj->text;
	}

	nCharWidth = ONDK_FONT_GetOneCharacterWidth(pFont, textObj->drawText, &ulUsedBytes);

	txtWidth = (w+textObj->curPosX);

	bIsReset = ONDK_FONT_DrawStringPosition(screen, pFont, textObj->drawText, 0, tw, tx, y, txtWidth+10, textObj->color, textflag);

	if((x-nCharWidth) >= tx)
	{
		textObj->curPosX -= nCharWidth;
		textObj->drawText += ulUsedBytes;
	}

	if(FALSE == bIsReset)
	{
		textObj->curPosX = textObj->startPosX;
		textObj->drawText = NULL;
	}

	ONDK_FONT_Release(pFont);

	return GWM_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmslidetext_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t 		lRet = ONDK_MESSAGE_OK;

	GWM_assert(object->type == GUIOBJ_SLIDETEXT);

	switch (proc_msg)
	{
	case MT_DESTROY:
		lRet = local_gwmslidetext_DefaultMethod_Destroy((SLIDETEXT_OBJECT)object, arg);
		break;

	case MT_DRAW:
		if (NULL == object->objDrawMethod)
		{
			lRet = local_gwmslidetext_DefaultMethod_Draw((SLIDETEXT_OBJECT)object, arg);
		}
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
void			INT_ONDK_GWM_COM_SlideText_Init(void)
{
	struct tagSlideTextObject *pObj = &s_defSlideTextObj;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagSlideTextObject));

	pObj->link = NULL;
	VK_memset(&(pObj->rect), 0, sizeof(ONDK_Rect_t));
	pObj->ID = -1;
	pObj->type = GUIOBJ_SLIDETEXT;
	pObj->focus = GUIOBJ_DISABLED;
	pObj->Proc = local_gwmslidetext_DefaultMethod;
	pObj->objDrawMethod = NULL;

	pObj->text = NULL;
	pObj->drawText = NULL;
	pObj->color = 0xFF000000;
	pObj->align = TEXT_ALIGN_LEFT;
	pObj->font = eFont_SystemNormal;
	pObj->size = DEFAULT_OBJ_FONT_SIZE;
	pObj->isHide = FALSE;
	pObj->hBgImage = NULL;
	pObj->bgColor = 0x00000000;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/



ONDK_Result_t			ONDK_GWM_COM_SlideText_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *text)
{
	ONDK_GWM_GuiObject_t		objlist;
	SLIDETEXT_OBJECT			new_textObj;
	ONDK_Result_t			hResult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_textObj = (SLIDETEXT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_textObj != NULL)
		return GWM_RET_LINK_CONFLICT;

	new_textObj = local_gwmslidetext_AllocNewSlideTextObj();
	if (new_textObj == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	HxSTD_MemCopy(&(new_textObj->rect), rect, sizeof(ONDK_Rect_t));
	new_textObj->ID = id;
	new_textObj->text = text;
	new_textObj->startPosX = (rect->w/2) * -1;
	new_textObj->curPosX = new_textObj->startPosX;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)new_textObj, 0);

	hResult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_textObj);

	return hResult;
}

HUINT8 *				ONDK_GWM_COM_SlideText_GetPtrByObject(SLIDETEXT_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_SLIDETEXT);
	return obj->text;
}


ONDK_Color_t			ONDK_GWM_COM_SlideText_GetColorByObject(SLIDETEXT_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_SLIDETEXT);
	return obj->color;
}

HINT32				ONDK_GWM_COM_SlideText_GetSizeByObject(SLIDETEXT_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_SLIDETEXT);
	return obj->size;
}

HINT32				ONDK_GWM_COM_SlideText_GetAlignByObject(SLIDETEXT_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_SLIDETEXT);
	return obj->align;
}

ONDK_Result_t			ONDK_GWM_COM_SlideText_ChangeColor(HINT32 id, ONDK_Color_t color)
{
	SLIDETEXT_OBJECT		obj;

	obj = (SLIDETEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_SLIDETEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	obj->color = color;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_COM_SlideText_SetSize(HUINT32 id, HINT32 size)
{
	SLIDETEXT_OBJECT	obj;

	obj = (SLIDETEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_SLIDETEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	local_gwmslidetext_ClearPositionData(obj);

	obj->size = size;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}

ONDK_Result_t			ONDK_GWM_COM_SlideText_SetAlign(HINT32 id, HINT32 align)
{
	SLIDETEXT_OBJECT		obj;

	obj = (SLIDETEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_SLIDETEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	local_gwmslidetext_ClearPositionData(obj);

	obj->align = align;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}

ONDK_Result_t		ONDK_GWM_COM_SlideText_SetText(HINT32 id, HCHAR *str)
{

	SLIDETEXT_OBJECT		obj;

	obj = (SLIDETEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_SLIDETEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	local_gwmslidetext_ClearPositionData(obj);

	obj->text=str;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_SlideText_SetFont (HINT32 id, ONDK_FontId_t fontId)
{
	SLIDETEXT_OBJECT		obj;

	obj = (SLIDETEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_SLIDETEXT)
	{
		return GWM_RET_NO_OBJECT;
	}
	local_gwmslidetext_ClearPositionData(obj);

	obj->font= fontId;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}

ONDK_FontId_t	ONDK_GWM_COM_SlideText_GetFont (HINT32 id)
{
	SLIDETEXT_OBJECT		obj;

	obj = (SLIDETEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_SLIDETEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	return obj->font;
}

ONDK_Result_t	ONDK_GWM_COM_SlideText_Update (HINT32 id, HINT32 speed)
{
	SLIDETEXT_OBJECT		obj;

	obj = (SLIDETEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_SLIDETEXT)
	{
		return GWM_RET_NO_OBJECT;
	}


	obj->curPosX += speed;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_SlideText_BGImage (HINT32 id, HCHAR *backgroundImg)
{
	SLIDETEXT_OBJECT		obj;

	obj = (SLIDETEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_SLIDETEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	if(NULL != obj->hBgImage)
	{
		ONDK_IMG_Destroy(obj->hBgImage);
		obj->hBgImage = NULL;
	}

	obj->hBgImage = ONDK_IMG_CreateSelf(backgroundImg);
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_SlideText_BGColor (HINT32 id, ONDK_Color_t color)
{
	SLIDETEXT_OBJECT		obj;

	obj = (SLIDETEXT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_SLIDETEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	obj->bgColor = color;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}


/* End of File. ---------------------------------------------------------- */


