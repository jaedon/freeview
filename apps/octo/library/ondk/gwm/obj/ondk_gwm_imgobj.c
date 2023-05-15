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
struct	tagImgObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.


	HCHAR			*imgPath;
	ONDKImageHandle	hImg;
	HINT32			alpha;
	ONDK_Rect_t		orgRect;
};

STATIC struct tagImgObject	s_defImgObj;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC IMAGE_OBJECT	local_gwmimg_AllocNewObj(void)
{
	struct tagImgObject	*pObj = &s_defImgObj;

	pObj = (struct tagImgObject *)ONDK_Malloc(sizeof(struct tagImgObject));
	ONDK_Assert(pObj);
	if (pObj == NULL)
		return NULL;

	HxSTD_MemCopy(pObj, &s_defImgObj, sizeof(struct tagImgObject));

	return pObj;

}

STATIC void	local_gwmimg_FreeObj(IMAGE_OBJECT obj)
{
	ONDK_Assert(obj);

	if(obj->hImg)	ONDK_IMG_Destroy(obj->hImg);

	ONDK_Free(obj);

	return;
}


/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC ONDK_Result_t	local_gwmimg_DefaultMethod_Destory(IMAGE_OBJECT	obj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(obj);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);

	local_gwmimg_FreeObj(obj);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmimg_DefaultMethod_Draw(IMAGE_OBJECT	obj, HINT32 arg)
{
	ONDK_Rect_t 		drawRect;
	HINT32				alpha;
	ONDK_Region_t		area;
	ONDK_Rect_t			rect;
	ONDKWindowHandle 	*screen = ONDK_GWM_GetCurSurface();
	HINT32				x, y, w, h;

	ONDK_UNUSED_ARGUMENT(obj);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)obj, &rect);

	x = area.x1 + obj->rect.x;
	y = area.y1 + obj->rect.y;
	w = obj->rect.w;
	h = obj->rect.h;

	drawRect = ONDK_Rect(x, y, w, h);
	if(obj->hImg)
	{
		alpha = obj->alpha;
		if(0xFF != alpha)
		{
			ONDK_IMG_StretchBlitAlpha(obj->hImg, screen, drawRect, alpha);
		}
		else
		{
			ONDK_IMG_StretchBlit(obj->hImg, screen, drawRect);
		}
	}

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmimg_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t 		lRet = ONDK_MESSAGE_OK;

	GWM_assert(object->type == GUIOBJ_IMAGE);

	switch(proc_msg)
	{
	case MT_DESTROY:
		lRet = local_gwmimg_DefaultMethod_Destory((IMAGE_OBJECT)object, arg);
		break;

	case MT_DRAW:
		if (NULL == object->objDrawMethod)
		{
			lRet = local_gwmimg_DefaultMethod_Draw((IMAGE_OBJECT)object, arg);
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

void 		INT_ONDK_GWM_COM_Image_Init(void)
{
	struct tagImgObject	*pObj = &s_defImgObj;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagImgObject));

	pObj->link = NULL;
	pObj->ID = -1;
	pObj->type = GUIOBJ_IMAGE;
	pObj->focus = GUIOBJ_DISABLED;
	pObj->Proc = local_gwmimg_DefaultMethod;
	pObj->objDrawMethod = NULL;

	pObj->hImg = NULL;
	pObj->imgPath = NULL;
	pObj->alpha = 0xFF;
}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t		ONDK_GWM_COM_Image_Create(HINT32 id, ONDK_Rect_t *rect)
{
	ONDK_GWM_GuiObject_t	objlist;
	IMAGE_OBJECT			newImgObj;
	ONDK_Result_t			hResult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	newImgObj = (IMAGE_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (newImgObj != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	newImgObj = local_gwmimg_AllocNewObj();
	if (newImgObj == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	newImgObj->ID = id;
	HxSTD_MemCopy(&(newImgObj->rect), rect, sizeof(ONDK_Rect_t));
	HxSTD_MemCopy(&(newImgObj->orgRect), rect, sizeof(ONDK_Rect_t));

	hResult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)newImgObj);
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&newImgObj->rect);

	return hResult;
}


void		ONDK_GWM_COM_Image_SetImage(HINT32 id, HCHAR *imgPath)
{
	IMAGE_OBJECT		obj;

	obj = (IMAGE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_IMAGE)
	{
		return ;
	}

	obj->imgPath = imgPath;

	if(obj->hImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hImg);
		obj->hImg = NULL;
	}

	obj->imgPath = imgPath;
	obj->hImg = ONDK_IMG_CreateSelf(imgPath);

	return;
}

void		ONDK_GWM_COM_Image_ChangeAlpha(HINT32 id, HINT32 alpha)
{
	IMAGE_OBJECT		obj;

	obj = (IMAGE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_IMAGE)
	{
		return ;
	}

	obj->alpha = alpha;

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);

	return;
}

void		ONDK_GWM_COM_Image_GetOrgRect(HINT32 id, ONDK_Rect_t *ret_pRect)
{
	IMAGE_OBJECT		obj;

	obj = (IMAGE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_IMAGE)
	{
		return ;
	}

	*ret_pRect = obj->orgRect;

	return;
}

void		ONDK_GWM_COM_Image_GetRect(HINT32 id, ONDK_Rect_t *ret_pRect)
{
	IMAGE_OBJECT		obj;

	obj = (IMAGE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_IMAGE)
	{
		return ;
	}

	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)obj, ret_pRect);

	return;
}


void		ONDK_GWM_COM_Image_ChangeRect(HINT32 id, const ONDK_Rect_t *pRect)
{
	IMAGE_OBJECT		obj;
	ONDK_Rect_t			rt;

	obj = (IMAGE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_IMAGE)
	{
		return ;
	}

	rt = ONDK_UnionRect (&obj->rect, pRect);
	obj->rect = *pRect;

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&rt);

	return;
}


HCHAR	*ONDK_GWM_COM_Image_GetImage(HINT32 id)
{
	IMAGE_OBJECT		obj;

	obj = (IMAGE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_IMAGE)
	{
		return NULL;
	}

	return obj->imgPath;

}

void	ONDK_GWM_COM_Image_Change(HINT32 id,ONDKImageHandle *hHandle)
{
	IMAGE_OBJECT		obj;

	obj = (IMAGE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_IMAGE )
	{
		return;
	}
	if(hHandle == NULL)
		obj->hImg = NULL;
	else
		obj->hImg = *hHandle;

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}


/* End of File. ---------------------------------------------------------- */

