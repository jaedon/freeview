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

#include <ondk_gwm_appobj.h>


#include "../ondk_gwm_objdef.h"
#include "../ondk_gwm_internal.h"


#define MAX_VPROGRESS_OBJECT						128

/**********************************************************************************************************/
#define ____STATIC_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

struct tagVProgressObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.

	void	*userdata;

	HINT32	maxPos;
	HINT32	minPos;
	HINT32	curPos;

	HINT32	vspace;

	HCHAR	*blankImg;
	HCHAR	*fillImg;
	HCHAR	*backgroundImg;

	ONDKImageHandle	hBlankImg;
	ONDKImageHandle	hFillImg;
	ONDKImageHandle	hBackgroundImg;
};

STATIC struct tagVProgressObject	s_defVProgress;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC VPROGRESS_OBJECT		local_gwmVprogressobj_AllocNewVProgressObj(void)
{
	struct tagVProgressObject	*pObj = &s_defVProgress;

	pObj = (struct tagVProgressObject *)ONDK_Malloc(sizeof(struct tagVProgressObject));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defVProgress, sizeof(struct tagVProgressObject));

	return pObj;
}

STATIC void		local_gwmVprogressobj_FreeVProgressObj(VPROGRESS_OBJECT obj)
{
	ONDK_Assert(obj);

	if(obj->hBackgroundImg)	ONDK_IMG_Destroy(obj->hBackgroundImg);
	if(obj->hBlankImg)		ONDK_IMG_Destroy(obj->hBlankImg);
	if(obj->hFillImg)		ONDK_IMG_Destroy(obj->hFillImg);

	ONDK_Free(obj);

	return;
}

/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC ONDK_Result_t	local_gwmVprogressobj_DefaultMethod_Destory(VPROGRESS_OBJECT	object, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(object);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&object->rect);

	local_gwmVprogressobj_FreeVProgressObj(object);

	return ONDK_MESSAGE_BREAK;
}


STATIC ONDK_Result_t		local_gwmVprogressobj_DefaultMethod_Draw(VPROGRESS_OBJECT object, HINT32 arg)
{
	ONDKSurfaceHandle 	screen = ONDK_GWM_GetCurSurface();
	HINT32				y = 0;
	HINT32				per = 0;
	HINT32				wholeCellHeight = 0, aCellHeight =  0;
	HBOOL 				isFrontMost = FALSE;
	ONDK_Size_t			fillSize = {0, 0}, blankSize = {0, 0};
	ONDK_Rect_t 		drawRect;
	ONDK_Region_t			area;
	ONDK_Rect_t				rect;

	VPROGRESS_OBJECT	vProgress = (VPROGRESS_OBJECT)object;

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)object, &rect);

	isFrontMost = arg;

	drawRect = ONDK_Rect(area.x1 + rect.x, area.y1 + rect.y, rect.w, rect.h);

	if(vProgress->hBackgroundImg)
	{
		ONDK_IMG_StretchBlit(vProgress->hBackgroundImg, screen, drawRect);
	}

	if(vProgress->hFillImg)
	{
		fillSize = ONDK_IMG_GetSize(vProgress->hFillImg);
	}

	if(vProgress->hBlankImg)
	{
		blankSize = ONDK_IMG_GetSize(vProgress->hBlankImg);
	}

	y = drawRect.y + drawRect.h;
	aCellHeight = fillSize.h + vProgress->vspace;
	if(aCellHeight > 0)
	{
		wholeCellHeight = y - (aCellHeight * vProgress->curPos);
	}
	else
	{
		per = (vProgress->curPos*100)/(vProgress->maxPos);
		wholeCellHeight =  drawRect.h * per / 100;
		wholeCellHeight =  (drawRect.h - wholeCellHeight) + drawRect.y;
	}

	if(vProgress->hFillImg)
	{
		while(y > wholeCellHeight)
		{
#if defined(CONFIG_APUI_NATIVE_APP)
			//ONDK_IMG_Blit(vProgress->hFillImg, screen, drawRect.x, y - fillSize.h);
			if(vProgress->hBlankImg)
			{
				ONDK_IMG_Blit(vProgress->hBlankImg, screen, drawRect.x, y - fillSize.h);
			}
			ONDK_IMG_BlitAlpha(vProgress->hFillImg, screen, drawRect.x, y - fillSize.h, ((vProgress->curPos*12)));
#else
			ONDK_IMG_Blit(vProgress->hFillImg, screen, drawRect.x, y - aCellHeight);
#endif
			y -= fillSize.h;
			y -= vProgress->vspace;
		}
	}
	else
	{
		y -= wholeCellHeight;
	}

	if (vProgress->hBlankImg)
	{
#if defined(CONFIG_APUI_NATIVE_APP)
		while(y >= drawRect.y)
		{
			ONDK_IMG_Blit(vProgress->hBlankImg, screen, drawRect.x, y - blankSize.h);
			y -= blankSize.h;
			y -= vProgress->vspace;
		}
#else
		while((y - blankSize.h) >= drawRect.y)
		{
			ONDK_IMG_Blit(vProgress->hBlankImg, screen, drawRect.x, y - aCellHeight);
			y -= blankSize.h;
			y -= vProgress->vspace;
		}
#endif
	}

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmVprogressobj_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	VPROGRESS_OBJECT	vprogress;
	ONDK_Result_t		lRet = ONDK_RET_OK;

	ONDK_Assert(object->type == GUIOBJ_VPROGRESS);
	vprogress = (VPROGRESS_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
			lRet = local_gwmVprogressobj_DefaultMethod_Destory((VPROGRESS_OBJECT)object, arg);
			break;
		case MT_DRAW:
			if(NULL == object->objDrawMethod)
			{
				lRet = local_gwmVprogressobj_DefaultMethod_Draw((VPROGRESS_OBJECT)object, arg);
			}
			break;
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(vprogress->rect.x, vprogress->rect.y, vprogress->rect.w, vprogress->rect.h);
			VK_memcpy(&(vprogress->rect), new_rect, sizeof(ONDK_Region_t));
			ONDK_GWM_APP_InvalidateRelativeRect(vprogress->rect.x, vprogress->rect.y, vprogress->rect.w, vprogress->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_CHANGE_TEXT:
		{
			return GWM_MESSAGE_PASS;
		}

		case MT_GET_FOCUS:
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_GET, (Handle_t)NULL, object->ID, arg, (int)vprogress, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(vprogress->rect.x, vprogress->rect.y, vprogress->rect.w, vprogress->rect.h);
			break;

		case MT_LOST_FOCUS:
			vprogress->focus &= ~GUIOBJ_PUSHED;
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_LOSE, (Handle_t)NULL, object->ID, arg, (int)vprogress, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(vprogress->rect.x, vprogress->rect.y, vprogress->rect.w, vprogress->rect.h);
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

void			INT_ONDK_GWM_COM_VProgress_Init(void)
{
	struct tagVProgressObject	*pObj = &s_defVProgress;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagVProgressObject));

	pObj->ID = -1;
	pObj->type = GUIOBJ_VPROGRESS;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwmVprogressobj_DefaultMethod;
	pObj->objDrawMethod = NULL;

	pObj->maxPos = 100;
	pObj->minPos = 0;
	pObj->curPos = 50;
	pObj->vspace = 0;

}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t				ONDK_GWM_COM_VProgress_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 minPos, HINT32 maxPos, HINT32 curPos)
{
	ONDK_GWM_GuiObject_t		objlist;
	VPROGRESS_OBJECT		new_vprogress;
	ONDK_Result_t			hresult;

	ONDK_Assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_vprogress = (VPROGRESS_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_vprogress != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_vprogress = local_gwmVprogressobj_AllocNewVProgressObj();
	if (new_vprogress == NULL)
	{
		ONDK_Error("[%s:%d] obj GWM_RET_OUT_OF_MEMORY\n", __FUNCTION__, __LINE__);
		return GWM_RET_OUT_OF_MEMORY;
	}

	new_vprogress->ID = id;
	new_vprogress->rect = *rect;
	new_vprogress->maxPos = maxPos;
	new_vprogress->minPos = minPos;
	new_vprogress->curPos = curPos;

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_vprogress);
	ONDK_GWM_APP_InvalidateRelativeRect(new_vprogress->rect.x, new_vprogress->rect.y, new_vprogress->rect.w, new_vprogress->rect.h);

	return hresult;
}


void	*ONDK_GWM_COM_VProgress_GetUserdata(HINT32 id)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return NULL;
	}

	return obj->userdata;
}

void ONDK_GWM_COM_VProgress_SetUserdata(HINT32 id, void	*userdata)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return ;
	}

	obj->userdata = userdata;

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}

HINT32 ONDK_GWM_COM_VProgress_GetMax(HINT32 id)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return -1;
	}

	return obj->maxPos;
}

void ONDK_GWM_COM_VProgress_SetMax(HINT32 id, HINT32 maxPos)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return ;
	}

	obj->maxPos = maxPos;

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}

HINT32 ONDK_GWM_COM_VProgress_GetMin(HINT32 id)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return -1;
	}

	return obj->minPos;
}

void ONDK_GWM_COM_VProgress_SetMin(HINT32 id, HINT32 minPos)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return ;
	}

	obj->minPos = minPos;

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}

HINT32 ONDK_GWM_COM_VProgress_GetPos(HINT32 id)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return -1;
	}

	return obj->curPos;
}

void ONDK_GWM_COM_VProgress_SetPos(HINT32 id, HINT32 curPos)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return;
	}

	obj->curPos = curPos;

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}

HINT32 ONDK_GWM_COM_VProgress_GetVspace(HINT32 id)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return -1;
	}

	return obj->vspace;
}

void ONDK_GWM_COM_VProgress_SetVspace(HINT32 id, HINT32 vspace)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return ;
	}

	obj->vspace = vspace;
}

HCHAR	*ONDK_GWM_COM_VProgress_GetBlankImg(HINT32 id)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return NULL;
	}

	return obj->blankImg;
}

void ONDK_GWM_COM_VProgress_SetBlankImg(HINT32 id, HCHAR	*blankImg)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return ;
	}

	if(obj->hBlankImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hBlankImg);
		obj->hBlankImg = NULL;
	}

	obj->blankImg = blankImg;
	obj->hBlankImg = ONDK_IMG_CreateSelf(blankImg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}

HCHAR	*ONDK_GWM_COM_VProgress_GetFillImg(HINT32 id)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return NULL;
	}

	return obj->fillImg;
}

void ONDK_GWM_COM_VProgress_SetFillImg(HINT32 id, HCHAR	*fillImg)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return ;
	}

	if(obj->hFillImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hFillImg);
		obj->hFillImg = NULL;
	}

	obj->fillImg = fillImg;
	obj->hFillImg = ONDK_IMG_CreateSelf(fillImg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}

HCHAR	*ONDK_GWM_COM_VProgress_GetBackgroundImg(HINT32 id)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return NULL;
	}

	return obj->backgroundImg;
}

void ONDK_GWM_COM_VProgress_SetBackgroundImg(HINT32 id, HCHAR	*backgroundImg)
{
	VPROGRESS_OBJECT		obj;

	obj = (VPROGRESS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_VPROGRESS)
	{
		return ;
	}

	obj->backgroundImg = backgroundImg;

	if(obj->hBackgroundImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hBackgroundImg);
		obj->hBackgroundImg = NULL;
	}

	obj->backgroundImg = backgroundImg;
	obj->hBackgroundImg = ONDK_IMG_CreateSelf(backgroundImg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}

/* End of File. ---------------------------------------------------------- */


