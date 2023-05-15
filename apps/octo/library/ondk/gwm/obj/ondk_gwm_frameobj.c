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

#include "../ondk_gwm_lnklist.h"
#include "../ondk_gwm_internal.h"


/**********************************************************************************************************/
#define ____STATIC_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

struct	tagFrameObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.
	HCHAR			*title;

	HINT32			nParam1;
	HINT32			nParam2;

	ONDK_Color_t	frameColor;
	ONDK_Color_t	textColor;
};

STATIC struct	tagFrameObject			s_defFrameObj;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/


STATIC FRAME_OBJECT	local_gwmframeobj_AllocNewFrameObj(void)
{
	struct tagFrameObject	*pObj = &s_defFrameObj;

	pObj = (struct tagFrameObject *)ONDK_Malloc(sizeof(struct tagFrameObject));
	ONDK_Assert(pObj);
	if (pObj == NULL)
		return NULL;

	HxSTD_MemCopy(pObj, &s_defFrameObj, sizeof(struct tagFrameObject));

	return pObj;
}

STATIC void			local_gwmframeobj_FreeFrameObj(FRAME_OBJECT obj)
{
	ONDK_Assert(obj);

	ONDK_Free(obj);
}

/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC ONDK_Result_t	local_gwmframeobj_DefaultMethod_Destory(FRAME_OBJECT	frameObj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(frameObj);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&frameObj->rect);

	local_gwmframeobj_FreeFrameObj(frameObj);

	return ONDK_MESSAGE_BREAK;
}



STATIC ONDK_Result_t	local_gwmframeobj_DefaultMethod_Draw(FRAME_OBJECT frameObj, HINT32 arg)
{
	HINT32	x, y, w, h;

	ONDKSurfaceHandle 		screen = ONDK_GWM_GetCurSurface();
	HBOOL 					isFrontMost = FALSE;
	ONDKFontHandle			font;
	ONDK_Color_t			bright, face, shadow;
	ONDKSurfaceTextFlags	align;
	ONDK_Region_t			area;
	ONDK_Rect_t				rect;

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)frameObj, &rect);


	isFrontMost = arg;

	x = area.x1 + rect.x;
	y = area.y1 + rect.y;
	w = rect.w;
	h = rect.h;

	if(frameObj->frameColor)
	{
		bright 		= ONDK_GWM_GFX_GetSysDimmedColor(screen, frameObj->frameColor, 150);
		face 		= frameObj->frameColor;
		shadow 		= ONDK_GWM_GFX_GetSysDimmedColor(screen, frameObj->frameColor, 50);

		if(isFrontMost)
		{
			bright 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, bright, DEFAULT_DIMMED_LEVEL);
			face 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, face,	 DEFAULT_DIMMED_LEVEL);
			shadow 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, shadow, DEFAULT_DIMMED_LEVEL);
		}

		ONDK_GWM_GFX_DrawFrame(screen, x, y, w, h, bright, face, shadow);
	}

	/* draw frame's title */
	if (frameObj->title != NULL)
	{
		font = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, DEFAULT_OBJ_FONT_SIZE);
		if (font != NULL)
		{
			align = ONDKSTF_CENTER;
			ONDK_FONT_DrawStringAlignAbbr(screen, font, frameObj->title, x, y + 12, w, frameObj->textColor, align);
		}
		ONDK_FONT_Release(font);
	}

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmframeobj_DefaultMethod_ChangeText(FRAME_OBJECT frameObj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(frameObj);
	ONDK_UNUSED_ARGUMENT(arg);

	frameObj->title = (HCHAR *)arg;
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&frameObj->rect);

	return ONDK_MESSAGE_BREAK;
}


STATIC ONDK_Result_t	local_gwmframeobj_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t	lRet = ONDK_RET_OK;

	GWM_assert(object->type == GUIOBJ_FRAME);

	switch(proc_msg)
	{
	case MT_DESTROY:
		lRet = local_gwmframeobj_DefaultMethod_Destory((FRAME_OBJECT)object, arg);
		break;

	case MT_DRAW:
		if (NULL == object->objDrawMethod)
		{
			lRet = local_gwmframeobj_DefaultMethod_Draw((FRAME_OBJECT)object, arg);
		}
		break;

	case MT_CHANGE_TEXT:
		lRet = local_gwmframeobj_DefaultMethod_ChangeText((FRAME_OBJECT)object, arg);
		break;

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

void			INT_ONDK_GWM_COM_Frame_Init(void)
{
	struct tagFrameObject *pObj = &s_defFrameObj;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagFrameObject));

	pObj->link = NULL;
	pObj->ID = -1;
	pObj->type = GUIOBJ_FRAME;
	pObj->focus = GUIOBJ_DISABLED;
	pObj->Proc = local_gwmframeobj_DefaultMethod;
	pObj->objDrawMethod = NULL;

	pObj->frameColor = 0xFF414757;
	pObj->textColor	 = 0xFF9EB1D2;

	pObj->title = NULL;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t		ONDK_GWM_COM_Frame_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR*title)
{
	ONDK_GWM_GuiObject_t	objlist;
	FRAME_OBJECT			newFrameObj;
	ONDK_Result_t			hResult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	newFrameObj = (FRAME_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (newFrameObj != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	newFrameObj = local_gwmframeobj_AllocNewFrameObj();
	if (newFrameObj == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	HxSTD_MemCopy(&(newFrameObj->rect), rect, sizeof(ONDK_Rect_t));
	newFrameObj->ID = id;
	newFrameObj->title = title;

	hResult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)newFrameObj);

	return	hResult;
}


ONDK_Result_t		ONDK_GWM_COM_Frame_ChangeTitle(HINT32 id, HCHAR*new_title)
{
	ONDK_GWM_GuiObject_t	obj;
	FRAME_OBJECT			frame;

	obj = ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_FRAME)
	{
		return GWM_RET_NO_OBJECT;
	}

	frame = (FRAME_OBJECT)obj;

	frame->title = new_title;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, obj, 0);

	return GWM_RET_OK;
}


HCHAR * 			ONDK_GWM_COM_Frame_GetTitleByObject(FRAME_OBJECT frame)
{
	GWM_assert(frame->type == GUIOBJ_FRAME);

	return frame->title;
}

HCHAR *			ONDK_GWM_COM_Frame_GetTitle(HINT32 nId, HINT32 nParam)
{
	ONDK_GWM_GuiObject_t	obj;
	FRAME_OBJECT			frame;

	obj = ONDK_GWM_APP_GetObject(nId);
	if (obj == NULL || obj->type != GUIOBJ_FRAME)
	{
		return NULL;
	}

	frame = (FRAME_OBJECT)obj;

	return frame->title;
}

ONDK_Result_t		ONDK_GWM_COM_Frame_SetParam1(HINT32 nId, HINT32 nParam)
{
	ONDK_GWM_GuiObject_t		obj;
	FRAME_OBJECT		frame;

	obj = ONDK_GWM_APP_GetObject(nId);
	if (obj == NULL || obj->type != GUIOBJ_FRAME)
	{
		return GWM_RET_NO_OBJECT;
	}

	frame = (FRAME_OBJECT)obj;
	frame->nParam1 = nParam;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, obj, 0);

	return GWM_RET_OK;
}

ONDK_Result_t		ONDK_GWM_COM_Frame_SetParam2(HINT32 nId, HINT32 nParam)
{
	ONDK_GWM_GuiObject_t		obj;
	FRAME_OBJECT		frame;

	obj = ONDK_GWM_APP_GetObject(nId);
	if (obj == NULL || obj->type != GUIOBJ_FRAME)
	{
		return GWM_RET_NO_OBJECT;
	}

	frame = (FRAME_OBJECT)obj;
	frame->nParam2 = nParam;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, obj, 0);

	return GWM_RET_OK;

}

ONDK_Result_t		ONDK_GWM_COM_Frame_SetParam1ByObject(FRAME_OBJECT object, HINT32 nParam)
{
	FRAME_OBJECT		frame;

	GWM_assert(object->type == GUIOBJ_FRAME);

	frame = (FRAME_OBJECT)object;
	frame->nParam1 = nParam;

	return GWM_RET_OK;
}

ONDK_Result_t		ONDK_GWM_COM_Frame_SetParam2ByObject(FRAME_OBJECT object, HINT32 nParam)
{
	FRAME_OBJECT		frame;


	GWM_assert(object->type == GUIOBJ_FRAME);

	frame = (FRAME_OBJECT)object;
	frame->nParam2 = nParam;

	return GWM_RET_OK;

}

HINT32		ONDK_GWM_COM_Frame_GetParam1ByObject(FRAME_OBJECT object)
{
	FRAME_OBJECT		frame;

	GWM_assert(object != NULL);
	GWM_assert(object->type == GUIOBJ_FRAME);
	frame = (FRAME_OBJECT)object;

	return frame->nParam1;
}

HINT32		ONDK_GWM_COM_Frame_GetParam2ByObject(FRAME_OBJECT object)
{
	FRAME_OBJECT		frame;

	GWM_assert(object->type == GUIOBJ_FRAME);
	frame = (FRAME_OBJECT)object;

	return frame->nParam2;
}

ONDK_Result_t	ONDK_GWM_COM_Frame_SetColor(HINT32 nId, ONDK_Color_t textColor, ONDK_Color_t frameColor)
{
	ONDK_GWM_GuiObject_t		obj;
	FRAME_OBJECT		frame;

	obj = ONDK_GWM_APP_GetObject(nId);
	if (obj == NULL || obj->type != GUIOBJ_FRAME)
	{
		return GWM_RET_NO_OBJECT;
	}

	frame = (FRAME_OBJECT)obj;

	frame->textColor = textColor;
	frame->frameColor = frameColor;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGED_VALUE, obj, 0);

	return GWM_RET_OK;
}


/* End of File. ---------------------------------------------------------- */

