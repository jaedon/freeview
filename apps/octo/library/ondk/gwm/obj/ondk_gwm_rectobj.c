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
#include "../ondk_gwm_objdef.h"
#include "../ondk_gwm_internal.h"

/**********************************************************************************************************/
#define ____STATIC_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

struct	tagRectObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.
	HUINT32				color;
	ONDKSurfaceHandle  	surHandle;
	ONDK_Rect_t			surRect;
    
   	HUINT32				shape;
    HUINT32             strokeColor;
    HUINT32             strokeR;
};

STATIC struct	tagRectObject			s_defRectObj;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC RECT_OBJECT	local_gwmrect_AllocNewRectObj(void)
{
	struct tagRectObject	*pObj = &s_defRectObj;

	pObj = (struct tagRectObject *)ONDK_Malloc(sizeof(struct tagRectObject));
	ONDK_Assert(pObj);

	if (pObj == NULL)
		return NULL;

	HxSTD_MemCopy(pObj, &s_defRectObj, sizeof(struct tagRectObject));

	return pObj;
}

STATIC void	local_gwmrect_FreeRectObj(RECT_OBJECT obj)
{
	ONDK_Assert(obj);
	INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)obj);
	if(obj->surHandle)
	{
		ONDK_SURFACE_Destroy(obj->surHandle);
	}
	ONDK_Free(obj);

	return;
}

/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC ONDK_Result_t	local_gwmrect_DefaultMethod_Destory(RECT_OBJECT	obj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(obj);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);

	local_gwmrect_FreeRectObj(obj);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmrect_DefaultMethod_Draw(RECT_OBJECT obj, HINT32 arg)
{
	ONDK_Region_t			area;
	ONDK_Rect_t				rect;
	ONDKSurfaceHandle 		screen = ONDK_GWM_GetCurSurface();
    HINT32 x1 = 0, y1 = 0, x2 = 0, y2 = 0, r = 0;
    
    

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)obj, &rect);

	if(obj->surHandle)
	{
		ONDK_SURFACE_StretchBlit(obj->surHandle, screen, obj->surRect, ONDK_Rect(area.x1+rect.x, area.y1+rect.y,rect.w, rect.h));
	}
	else
	{
		ONDK_DRAW_FillRectElement(screen, area.x1+rect.x, area.y1+rect.y,rect.w, rect.h, obj->color);
	}

    if(RECT_SHAPE_STROKE == obj->shape){
        //draw stroke
        r = obj->strokeR;                       
        
        x1 = area.x1 + rect.x;
        y1 = area.y1 + rect.y;        
        x2 = x1 + rect.w;
        y2 = y1 + rect.h;        

    	ONDK_DRAW_StrokeRoundRect(screen, x1, y1, (x2 - r ),(y2 - r), r, obj->strokeColor); 
    }

	return ONDK_MESSAGE_BREAK;
}



STATIC ONDK_Result_t	local_gwmrect_defaultRectProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	RECT_OBJECT		rectObj;
	ONDK_Result_t	lRet = ONDK_RET_OK;

	GWM_assert(object->type == GUIOBJ_RECTANGLE);
	rectObj = (RECT_OBJECT)object;

	switch(proc_msg)
	{
		case MT_DESTROY:
			lRet = local_gwmrect_DefaultMethod_Destory((RECT_OBJECT)object, arg);
			break;
		case MT_DRAW:
			if(NULL == object->objDrawMethod)
			{
				if(COL_A(rectObj->color) != 0)
				{
					lRet = local_gwmrect_DefaultMethod_Draw((RECT_OBJECT)object, arg);
				}
			}
			break;
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(rectObj->rect.x, rectObj->rect.y, rectObj->rect.w, rectObj->rect.h);
			VK_memcpy( &(rectObj->rect), new_rect, sizeof(ONDK_Rect_t) );
			ONDK_GWM_APP_InvalidateRelativeRect(rectObj->rect.x, rectObj->rect.y, rectObj->rect.w, rectObj->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_SET_VALUE:
		{
			rectObj->color= arg;
			ONDK_GWM_APP_InvalidateRelativeRect(rectObj->rect.x, rectObj->rect.y, rectObj->rect.w, rectObj->rect.h);
			return GWM_MESSAGE_BREAK;
		}
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

void 		INT_ONDK_GWM_COM_Rect_Init(void)
{
	struct tagRectObject	*pObj = &s_defRectObj;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagRectObject));

	pObj->link = NULL;
	pObj->ID = -1;
	pObj->type = GUIOBJ_RECTANGLE;
	pObj->focus = GUIOBJ_DISABLED;
	pObj->Proc = local_gwmrect_defaultRectProc;
	pObj->objDrawMethod = NULL;

	pObj->color = 0;
    
    pObj->shape = RECT_SHAPE_NORMAL;
    pObj->strokeR = 0;
    pObj->strokeColor = 0;

}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/
ONDK_Result_t		ONDK_GWM_COM_Rect_Create(HINT32 id, ONDK_Rect_t *rect,HUINT32 color)
{
	ONDK_GWM_GuiObject_t		objlist;
	RECT_OBJECT		new_rectObj;
	ONDK_Result_t			hresult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_rectObj = (RECT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_rectObj != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_rectObj = local_gwmrect_AllocNewRectObj();
	if (new_rectObj == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	new_rectObj->link = NULL;
	VK_memcpy(&(new_rectObj->rect), rect, sizeof(ONDK_Rect_t));
	new_rectObj->ID = id;
	new_rectObj->color = color;
	new_rectObj->surHandle = NULL;

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_rectObj);

	#if 0//defined(CONFIG_APUI_NATIVE_APP)
	{
		ONDKSurfaceHandle  h =	ONDK_SURFACE_Create(1, 1, "ARGB8888");

		new_rectObj->surHandle 	= h;
		new_rectObj->surRect 	= ONDK_Rect(0,0, 1,1);

		ONDK_DRAW_FillCopyRect (h, new_rectObj->surRect, color);
	}
	#endif

	return hresult;
}


ONDK_Result_t		ONDK_GWM_COM_Rect_ChangeColor(HINT32 id, HUINT32 new_color)
{
	RECT_OBJECT		obj;

	obj = (RECT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_RECTANGLE)
		return GWM_RET_NO_OBJECT;


	if(obj->surHandle)
	{
		ONDK_DRAW_FillCopyRect (obj->surHandle, obj->surRect, new_color);
	}

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_SET_VALUE, (ONDK_GWM_GuiObject_t)obj, new_color);

	return GWM_RET_OK;
}


ONDK_Color_t		ONDK_GWM_COM_Rect_GetColor(RECT_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_RECTANGLE);
	return obj->color;
}

ONDK_Result_t		ONDK_GWM_COM_Rect_ChangeSize(HINT32 id,HINT32 lx,HINT32 ly,HINT32 lw,HINT32 lh)
{
	RECT_OBJECT		obj;
    ONDK_Rect_t     rect = {0,0,0,0};
    
	obj = (RECT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_RECTANGLE)
		return GWM_RET_NO_OBJECT;
    
    if(lx>=0){
        rect.x = lx;
    }
    
    if(ly>=0){
        rect.y = ly;
    }
    
    if(lw>=0){
        rect.w = lw;
    }
    
    if(lh>=0){
        rect.h = lh;
    }
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_RELOCATE, (ONDK_GWM_GuiObject_t)obj,(int)&rect);

	return GWM_RET_OK;
}

ONDK_Result_t		ONDK_GWM_COM_Rect_SetStroke(HINT32 id, HINT32 r, HUINT32 strokeColor)
{
	RECT_OBJECT		obj;
    
	obj = (RECT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_RECTANGLE)
		return GWM_RET_NO_OBJECT;

    obj->shape = RECT_SHAPE_STROKE;
    obj->strokeColor = strokeColor;
    obj->strokeR = r;
    
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_DRAW, (ONDK_GWM_GuiObject_t)obj,0);    
	return GWM_RET_OK;
}


/* End of File. ---------------------------------------------------------- */

