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
#include <ondk_gwm_appobj.h>
#include "../ondk_gwm_internal.h"


/**********************************************************************************************************/
#define ____STATIC_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

struct	tagStatusObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.
	HINT32			        max;				// boundary of the value
	HINT32			        value;				// current value
	ONDK_Color_t			back_color;
	ONDK_Color_t			fore_color;
	HUINT8				    *text;
};

STATIC struct	tagStatusObject			s_defStatusObj;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC STATUS_OBJECT		local_gwmstatusobj_AllocNewStatusObj(void)
{
	struct tagStatusObject	*pObj = &s_defStatusObj;

	pObj = (struct tagStatusObject *)ONDK_Malloc(sizeof(struct tagStatusObject));
	ONDK_Assert(pObj);
	if (pObj == NULL)
		return NULL;

	HxSTD_MemCopy(pObj, &s_defStatusObj, sizeof(struct tagStatusObject));

	return pObj;
}

STATIC void	local_gwmstatusobj_FreeStatusObj(STATUS_OBJECT obj)
{
	ONDK_Assert(obj);
	ONDK_Free(obj);
}

/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC ONDK_Result_t	local_gwmstatusobj_DefaultMethod_Destory(STATUS_OBJECT	Obj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(Obj);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&Obj->rect);

	local_gwmstatusobj_FreeStatusObj(Obj);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmstatusobj_DefaultMethod_Draw(STATUS_OBJECT statusObj, HINT32 arg)
{
	HINT32	x, y, w, h;
	HINT32	value;
	HINT32	max;
	ONDK_Color_t			foreCol, backCol;
	ONDKSurfaceHandle 		screen = ONDK_GWM_GetCurSurface();
	ONDK_Region_t			area;
	ONDK_Rect_t				rect;
	
	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)statusObj, &rect);

	x = area.x1 + rect.x;
	y = area.y1 + rect.y;
	w = rect.w;
	h = rect.h;
	value = statusObj->value;
	max = statusObj->max;

	if (arg > 0)
	{	/* activated */
		foreCol = statusObj->fore_color;
		backCol = statusObj->back_color;
	}
	else
	{	/* dimmed */
		foreCol = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, statusObj->fore_color, DEFAULT_DIMMED_LEVEL);
		backCol =ONDK_GWM_GFX_GetSysDimmedAlpha(screen, statusObj->back_color, DEFAULT_DIMMED_LEVEL);
	}
	
	ONDK_DRAW_FillRectElement(screen, x, y, w, h, backCol);
	ONDK_DRAW_FillRectElement(screen, x, y, ( value * (w - 4) /max),  h -1, foreCol);

	return ONDK_MESSAGE_BREAK;
}



static ONDK_Result_t	local_gwmstatusobj_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;
	STATUS_OBJECT		statusObj;

	GWM_assert(object->type == GUIOBJ_STATUS);
	statusObj = (STATUS_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
			lRet = local_gwmstatusobj_DefaultMethod_Destory((STATUS_OBJECT)object, arg);				
			break;
		case MT_DRAW:
			if (NULL == object->objDrawMethod)
			{
				lRet = local_gwmstatusobj_DefaultMethod_Draw((STATUS_OBJECT)object, arg);
			}
			break;

		case MT_SET_VALUE:	
			statusObj->value = arg;
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
void		INT_ONDK_GWM_COM_Status_Init(void)
{
	struct tagStatusObject *pObj = &s_defStatusObj;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagStatusObject));

	pObj->link = NULL;
	pObj->ID = -1;
	pObj->type = GUIOBJ_STATUS;
	pObj->focus = GUIOBJ_DISABLED;
	pObj->Proc = local_gwmstatusobj_DefaultMethod;
	pObj->objDrawMethod = NULL;

	pObj->max = 0;
	pObj->value = 0;
	pObj->back_color = 0xFF8000FF;
	pObj->fore_color = 0xFF000080;
	pObj->text	= NULL;

}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t		ONDK_GWM_COM_Status_Create(HINT32 id, ONDK_Rect_t *rect, HUINT32 max,HUINT32 value)
{
	ONDK_GWM_GuiObject_t		objlist;
	STATUS_OBJECT		new_statusObj;
	ONDK_Result_t			hresult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_statusObj = (STATUS_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_statusObj != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_statusObj = local_gwmstatusobj_AllocNewStatusObj();
	if (new_statusObj == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	new_statusObj->link = NULL;
	VK_memcpy(&(new_statusObj->rect), rect, sizeof(ONDK_Rect_t));
	new_statusObj->ID = id;
	new_statusObj->max = max;
	new_statusObj->value = value;

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_statusObj);

	return hresult;
}


ONDK_Result_t		ONDK_GWM_COM_Status_SetValue(HINT32 id, HUINT32 new_value)
{
	STATUS_OBJECT		obj;
	static unsigned int		value;

	obj = (STATUS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_STATUS)
		return GWM_RET_NO_OBJECT;

	if (new_value > obj->max)
		value = obj->max;
	else
		value = new_value;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_SET_VALUE, (ONDK_GWM_GuiObject_t)obj, value);

	return GWM_RET_OK;
}


ONDK_Result_t		ONDK_GWM_COM_Status_GetValue(HINT32 id, HUINT32 *value)
{
	STATUS_OBJECT		obj;

	obj = (STATUS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_STATUS)
		return GWM_RET_NO_OBJECT;

	*value = obj->value;

	return GWM_RET_OK;
}


ONDK_Result_t		ONDK_GWM_COM_Status_GetProperty(STATUS_OBJECT obj, HUINT32 *max, HUINT32 *value, ONDK_Color_t *ulBackColor, ONDK_Color_t *ulForeColor)
{
	GWM_assert(obj->type == GUIOBJ_STATUS);

	*max = (HUINT32)obj->max;
	*value = (HUINT32)obj->value;
	*ulBackColor = obj->back_color;
	*ulForeColor = obj->fore_color;

	return GWM_RET_OK;
}


ONDK_Result_t		ONDK_GWM_COM_Status_SetText (HINT32 id, HUINT8 *text)
{
	STATUS_OBJECT		obj;

	obj = (STATUS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_STATUS)
		return GWM_RET_NO_OBJECT;

	obj->text	= text;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_SET_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}

HUINT8 *		ONDK_GWM_COM_Status_GetText (HINT32 id)
{
	STATUS_OBJECT		obj;

	obj = (STATUS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return NULL;
	if (obj->type != GUIOBJ_STATUS)
		return NULL;

	return obj->text;
}


ONDK_Result_t		ONDK_GWM_COM_Status_SetColor (HINT32 id, ONDK_Color_t ulBackColor, ONDK_Color_t ulForeColor)
{
	STATUS_OBJECT		obj;

	obj = (STATUS_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_STATUS)
		return GWM_RET_NO_OBJECT;

	obj->back_color = ulBackColor;
	obj->fore_color = ulForeColor;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_SET_VALUE, (ONDK_GWM_GuiObject_t)obj, 0);

	return GWM_RET_OK;
}


/* End of File. ---------------------------------------------------------- */

