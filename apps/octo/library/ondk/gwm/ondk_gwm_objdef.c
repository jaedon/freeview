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
#include <ondk.h>
#include <ondk_gwm_appobj.h>

#include "ondk_gwm_objdef.h"
#include "ondk_gwm_internal.h"

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */

/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */

/* ======================================================================== */
/* Exported Functions. 														*/
/* ======================================================================== */

// Application 내에서 사용될 함수.
GWM_ObjDraw_t	GWM_Obj_GetCustomDrawMethod(ONDK_GWM_GuiObject_t obj)
{
	return obj->objDrawMethod;
}

ONDK_Result_t	ONDK_GWM_Obj_GetObjectSize(HINT32 id, HINT32 *x, HINT32 *y, HINT32 *w, HINT32 *h)
{
	ONDK_GWM_GuiObject_t		obj;

	obj = ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return	GWM_RET_NO_OBJECT;
	*x = obj->rect.x;
	*y = obj->rect.y;
	*w = obj->rect.w;
	*h = obj->rect.h;
	return	GWM_RET_OK;
}


ONDK_Result_t		ONDK_GWM_Obj_GetObjectRect(HINT32 id, ONDK_Rect_t *rc)
{
	ONDK_GWM_GuiObject_t	obj;

	obj = ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	rc->x = obj->rect.x;
	rc->y = obj->rect.y;
	rc->w = obj->rect.w;
	rc->h = obj->rect.h;

	return GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_Obj_ChangeText(HINT32 id, HCHAR *text)
{
	ONDK_GWM_GuiObject_t		obj;

	obj = ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return	GWM_RET_NO_OBJECT;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CHANGE_TEXT, obj, (HINT32)text);

	return	GWM_RET_OK;
}


GWM_ObjectType_t	ONDK_GWM_Obj_GetObjectType(HINT32 id)
{
	ONDK_GWM_GuiObject_t		obj;

	obj = ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return	GUIOBJ_UNKNOWN;
	return	obj->type;
}


ONDK_Result_t	ONDK_GWM_Obj_SetValue(HINT32 id, HINT32 value)
{
	ONDK_GWM_GuiObject_t		obj;

	obj = ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return	GWM_RET_NO_OBJECT;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_SET_VALUE, obj, value);

	return	GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_Obj_Resize(HINT32 id, ONDK_Rect_t *rect)
{
	ONDK_GWM_GuiObject_t		obj;

	obj = ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return	GWM_RET_NO_OBJECT;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_RELOCATE, obj, (int)rect);

	return	GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_Obj_EnableObject(HINT32 ID)
{
	ONDK_GWM_GuiObject_t		obj;

	obj = ONDK_GWM_APP_GetObject(ID);
	if (obj == NULL)
		return	GWM_RET_NO_OBJECT;
	obj->focus &= ~GUIOBJ_DISABLED;

	ONDK_GWM_APP_InvalidateRect (obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return	GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_Obj_DisableObject(HINT32 ID)
{
	ONDK_GWM_GuiObject_t		obj;

	obj = ONDK_GWM_APP_GetObject(ID);
	if (obj == NULL)
		return	GWM_RET_NO_OBJECT;

	obj->focus = GUIOBJ_DISABLED;

	ONDK_GWM_APP_InvalidateRect (obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return	GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_Obj_SetObjectAttribute(HINT32 ID, HINT32 attribute)
{
	ONDK_GWM_GuiObject_t	obj;
	obj = ONDK_GWM_APP_GetObject(ID);
	return	INT_ONDK_GWM_OBJ_SetObjectAttribute(obj, attribute);
}

ONDK_GWM_GuiObject_t	ONDK_GWM_Obj_GetObject(HINT32 ID)
{
	return ONDK_GWM_APP_GetObject(ID);
}

ONDK_Result_t		ONDK_GWM_Obj_ChangeObjectRect(HINT32 ID, ONDK_Rect_t *rect)
{
	ONDK_GWM_GuiObject_t		obj;

	obj = ONDK_GWM_APP_GetObject(ID);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	if(rect == NULL || rect->x < 0 || rect->y < 0 || rect->w < 0 || rect->h < 0)
	{
		GWM_DBG_Print(DBGMSG_WARNING, ("[ONDK_GWM_APP_ChangeObjectRect] param fail!!"));
		return ERR_FAIL;
	}

	obj->rect.x = rect->x;
	obj->rect.y = rect->y;
	obj->rect.w = rect->w;
	obj->rect.h = rect->h;

	return ERR_OK;
}

GWM_Proc_t	ONDK_GWM_Obj_ChangeObjectProc(HINT32 ID, GWM_Proc_t new_procfunc)
{
	return	ONDK_GWM_APP_ChangeObjectProc(ID, new_procfunc);
}

ONDK_Result_t		ONDK_GWM_Obj_SetObjectDrawMethod(HINT32 ID, GWM_ObjDraw_t objDrawFunc)
{
	return ONDK_GWM_APP_SetObjectDrawMethod(ID, objDrawFunc);
}


ONDK_Result_t	ONDK_GWM_Obj_SetFocus(HINT32 ID)
{
	ONDK_GWM_GuiObject_t	objList;
	HINT32					found = 0;

	GWM_assert(g_pstCurrentApp);
	objList = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	while(objList != NULL)
	{
		// DISABLE 처리
		if ( (objList->focus & GUIOBJ_DISABLED) )
		{
			objList = (ONDK_GWM_GuiObject_t)gwm_lnklst_GetNext((LINK_OBJECT)objList);
			continue;
		}

		if (objList->ID == ID)
		{
			if ( !(objList->focus & GUIOBJ_HILIGHTED) )	// 아직 Focus 를 갖지 않은 Object라면,
			{
				objList->focus |= GUIOBJ_HILIGHTED;
				INT_ONDK_GWM_OBJ_CallObjectProc(MT_GET_FOCUS, objList, 0);
				// Invalidate 는 메소드 함수에서 처리할 것.
			}
			found = 1;
		}
		else
		{
			if ( objList->focus & GUIOBJ_HILIGHTED )	// 이미 Focus 를 갖고 있는 Object는 Focus를 없앤다.
			{
				objList->focus &= ~GUIOBJ_HILIGHTED;
				INT_ONDK_GWM_OBJ_CallObjectProc(MT_LOST_FOCUS, objList, 0);
				// Invalidate 는 메소드 함수에서 처리할 것.
			}
		}
		objList = (ONDK_GWM_GuiObject_t)gwm_lnklst_GetNext((LINK_OBJECT)objList);
	}

	if (found)
	{
		return	GWM_RET_OK;
	}

	return	GWM_RET_NO_OBJECT;
}


HINT32		ONDK_GWM_Obj_GetObjectStatus(ONDK_GWM_GuiObject_t	obj)
{
	if (obj == NULL)
		return -1;
	return	obj->focus;
}


ONDK_GWM_GuiObject_t	ONDK_GWM_Obj_GetFocusedObject(void)
{
	ONDK_GWM_GuiObject_t	objList;

	GWM_assert(g_pstCurrentApp);
	objList = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	while(objList != NULL)
	{
		if ( !(objList->focus & GUIOBJ_DISABLED) )		// FOCUS를 가질 수 있는 Object에 대해서만 처리.
		{
			if ( objList->focus & GUIOBJ_HILIGHTED )	// 이미 Focus 를 갖고 있는 Object는 Focus를 없앤다.
				return	objList;
		}
		objList = (ONDK_GWM_GuiObject_t)gwm_lnklst_GetNext((LINK_OBJECT)objList);
	}
	return	NULL;
}


ONDK_GWM_GuiObject_t	ONDK_GWM_Obj_GetObjectByID(HINT32 ID)
{
	ONDK_GWM_GuiObject_t	objList;

	GWM_assert(g_pstCurrentApp);
	objList = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	while(objList != NULL)
	{
		if(objList->ID == ID)
		{
			return objList;
		}
		objList = (ONDK_GWM_GuiObject_t)gwm_lnklst_GetNext((LINK_OBJECT)objList);
	}
	return	NULL;
}


HINT32		ONDK_GWM_Obj_GetObjectID(ONDK_GWM_GuiObject_t object)
{
	if (object == NULL)
		return -1;
	return	object->ID;
}


HINT32		ONDK_GWM_Obj_GetObjectAttr(ONDK_GWM_GuiObject_t object)
{
	if (object == NULL)
		return -1;
	return object->attr;
}


ONDK_Result_t	ONDK_GWM_Obj_Destroy(HINT32 id)
{
	ONDK_GWM_App_t			curr_app;
	ONDK_GWM_GuiObject_t	objList, obj;
	ONDK_Result_t			hResult;

	GWM_assert(g_pstCurrentApp);
	curr_app = g_pstCurrentApp;
	objList = INT_ONDK_GWM_APP_GetObjectList(curr_app);

	obj = INT_ONDK_GWM_OBJ_FindObject(objList, id);
	if (obj == NULL)
		return	GWM_RET_NO_OBJECT;

	ONDK_GWM_APP_InvalidateRect (obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	hResult = INT_ONDK_GWM_APP_RemoveObject(curr_app, obj);
	// Linked List 에서 먼저 빼낸 다음에 Object 를 삭제해야만 한다.
	// 이 안에서 메모리 해제가 이루어 지기 때문.
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_DESTROY, obj, 0);

	ONDK_GWM_APP_CheckAppUiOrderStacking();

	return	hResult;
}


ONDK_Result_t	ONDK_GWM_Obj_DestroyAllGuiObject(void)
{
	ONDK_GWM_App_t           curr_app;
	ONDK_GWM_GuiObject_t		objList, obj;
	ONDK_Result_t		hResult;

	GWM_assert(g_pstCurrentApp);
	curr_app = g_pstCurrentApp;
	objList = INT_ONDK_GWM_APP_GetObjectList(curr_app);
	while(objList != NULL)
	{
		obj = objList;
		ONDK_GWM_APP_InvalidateRect (obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);
		hResult = INT_ONDK_GWM_APP_RemoveObject(curr_app, obj);
		INT_ONDK_GWM_OBJ_CallObjectProc(MT_DESTROY, obj, 0);
		objList = INT_ONDK_GWM_APP_GetObjectList(curr_app);
	}

	ONDK_GWM_APP_CheckAppUiOrderStacking();

	return	GWM_RET_OK;
}

void	ONDK_GWM_Obj_SetVisible(HINT32 id, HBOOL visible)
{
	ONDK_GWM_Obj_SetHide(id, !visible);
}

HBOOL	ONDK_GWM_Obj_GetHide(HINT32 id)
{
	ONDK_GWM_GuiObject_t		objList, obj;

	GWM_assert(g_pstCurrentApp);
	objList = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	obj = INT_ONDK_GWM_OBJ_FindObject(objList, id);

	return ONDK_GWM_Obj_GetHideByObj(obj);
}

HBOOL	ONDK_GWM_Obj_GetVisible(HINT32 id)
{
	return !ONDK_GWM_Obj_GetHide(id);
}

void	ONDK_GWM_Obj_SetHideByObj(ONDK_GWM_GuiObject_t obj, HBOOL hide)
{
	if (obj == NULL)
		return;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_VISIBLE, obj, hide);
}

void	ONDK_GWM_Obj_SetHide(HINT32 id, HBOOL hide)
{
	ONDK_GWM_GuiObject_t		objList, obj;

	GWM_assert(g_pstCurrentApp);
	objList = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	obj = INT_ONDK_GWM_OBJ_FindObject(objList, id);
	if (obj == NULL)
		return;

	ONDK_GWM_Obj_SetHideByObj(obj, hide);
}


void	ONDK_GWM_Obj_SetVisibleByObj(ONDK_GWM_GuiObject_t obj, HBOOL visible)
{
	ONDK_GWM_Obj_SetHideByObj(obj, !visible);
}

HBOOL	ONDK_GWM_Obj_GetHideByObj(ONDK_GWM_GuiObject_t obj)
{
	if (obj == NULL)
		return FALSE;
	return obj->isHide;
}

HBOOL	ONDK_GWM_Obj_GetVisibleByObj(ONDK_GWM_GuiObject_t obj)
{
	return !ONDK_GWM_Obj_GetHideByObj(obj);
}

// GWM 내에서만 사용될 함수.

void	INT_ONDK_GWM_OBJ_BasicSetting(ONDK_GWM_GuiObject_t object)
{
	object->link = NULL;
	object->rect.x = -1;
	object->rect.y = -1;
	object->rect.w = 0;
	object->rect.h = 0;
	object->type = GUIOBJ_FRAME;
	object->ID = -1;
	object->focus = GUIOBJ_DISABLED;

	object->Proc = NULL;
	object->objDrawMethod = NULL;
}


ONDK_Result_t	INT_ONDK_GWM_OBJ_CallObjectProc(GWM_Obj_MethodType_t method, ONDK_GWM_GuiObject_t object, int arg)
{
	ONDK_Result_t eResult = ERR_OK;

	GWM_assert(object);
	if (object->Proc != NULL)
	{
		eResult = object->Proc(method, object, arg);
		return eResult;
	}
	return	GWM_RET_NO_OBJECT;
}



ONDK_Result_t	INT_ONDK_GWM_OBJ_SetObjectAttribute(ONDK_GWM_GuiObject_t object, int attribute)
{
	if (object == NULL)
		return GWM_RET_NO_OBJECT;
	object->focus = attribute;
	return	GWM_RET_OK;
}


void			INT_ONDK_GWM_OBJ_SetObjectAnimated(ONDK_GWM_GuiObject_t object)
{
	if (object == NULL)
		return;
	object->attr = 1;
}


void 		INT_ONDK_GWM_OBJ_ResetObjectAnimated(ONDK_GWM_GuiObject_t object)
{
	if (object == NULL)
		return;
	object->attr = 0;
}


void	INT_ONDK_GWM_OBJ_GetObjectRect(ONDK_GWM_GuiObject_t object, ONDK_Rect_t *rect)
{
	if (object == NULL)
	{
		VK_memset(rect, 0, sizeof(ONDK_Rect_t));
		return;
	}
	VK_memcpy(rect, &(object->rect), sizeof(ONDK_Rect_t));
}

GWM_Proc_t	INT_ONDK_GWM_OBJ_ChangeObjectProc(ONDK_GWM_GuiObject_t object, GWM_Proc_t new_procfunc)
{
	GWM_Proc_t	old_func;

	GWM_assert(object);
	GWM_assert(new_procfunc);

	old_func = object->Proc;
	object->Proc = new_procfunc;

	return old_func;
}


void				INT_ONDK_GWM_OBJ_SetDrawMethod(ONDK_GWM_GuiObject_t object, GWM_ObjDraw_t drawFunc)
{
	GWM_assert(object);
	object->objDrawMethod = drawFunc;
}


ONDK_GWM_GuiObject_t	INT_ONDK_GWM_OBJ_GetNextObject(ONDK_GWM_GuiObject_t object)
{
	if (object == NULL)
	{
		return	NULL;
	}

	return	(ONDK_GWM_GuiObject_t)gwm_lnklst_GetNext((LINK_OBJECT)object);//(GWM_GuiObject_t)object->link.nextObject;
}


ONDK_GWM_GuiObject_t	INT_ONDK_GWM_OBJ_FindObject(ONDK_GWM_GuiObject_t objlst, int ID)
{
	ONDK_GWM_GuiObject_t	traverse;

	traverse = objlst;
	while(traverse != NULL)
	{
		if (traverse->ID == ID)
			break;
		traverse = (ONDK_GWM_GuiObject_t)gwm_lnklst_GetNext((LINK_OBJECT)traverse);
	}
	return	traverse;
}



//#define	ABS(a)	(((a)>0)?(a):-(a))
#define	REPLACE_NEAREST_OBJECT						\
{													\
	if ( ABS(ox-cx)+ABS(oy-cy) < min_distance)		\
	{												\
		min_distance = ABS(ox-cx)+ABS(oy-cy);		\
		nextFocus = objList;						\
	}												\
}

ONDK_Result_t	INT_ONDK_GWM_OBJ_MoveFocus(ONDK_GWM_GuiObject_t obj, HINT32 direction)
{
	ONDK_GWM_GuiObject_t	objList, nextFocus;
	int		cx, cy, ox, oy;		// current focused,  objList 각각의 중심점.
	int		min_distance;

	ONDK_Print("[%s:%d] (+)\r\n", __FUNCTION__, __LINE__);

	cx = obj->rect.x + obj->rect.w/2;					// 현재 Focusd Object의 중심점을 계산
	cy = obj->rect.y + obj->rect.h/2;

	GWM_assert(g_pstCurrentApp);
	objList = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	nextFocus = NULL;
	min_distance = GWM_SCREEN_WIDTH+GWM_SCREEN_HEIGHT;	// 가장 먼 거리를 설정.

	while(objList != NULL)
	{
		if (objList == obj)								// 같은 Object는 비교하지 않는다.
		{
			objList = (ONDK_GWM_GuiObject_t)gwm_lnklst_GetNext((LINK_OBJECT)objList);
			continue;
		}

		if ( !(objList->focus & GUIOBJ_DISABLED) )		// Focus가 가능한 Object를 대상으로
		{
			ox = objList->rect.x + objList->rect.w/2;	// 중심점을 계산
			oy = objList->rect.y + objList->rect.h/2;

			switch(direction)
			{
			case MOVE_FOCUS_LEFT:
				if (ox < cx)						// 왼쪽에 위치한 Object를 대상으로..
				{
					if ( ABS(ox-cx) > ABS(oy-cy)*3 )// 가로로 배치된 Object 라면, (가로 거리가 세로거리의 3배 이상 = 약 30도 각도이내..)
						REPLACE_NEAREST_OBJECT;
				}
				break;
			case MOVE_FOCUS_RIGHT:
				if (ox > cx)						// 오른쪽에 위치한 Object를 대상으로..
				{
					if ( ABS(ox-cx) > ABS(oy-cy)*3 )// 가로로 배치된 Object 라면, (가로 거리가 세로거리의 3배 이상 = 약 30도 각도이내..)
						REPLACE_NEAREST_OBJECT;
				}
				break;
			case MOVE_FOCUS_UP:
				if (oy < cy)						// 윗쪽에 위치한 Object를 대상으로..
				{
					if ( ABS(ox-cx) < ABS(oy-cy)*3 )// 세로로 배치된 Object 라면, (가로 거리가 세로거리의 3배 이하 = 약 30도 각도이상..)
						REPLACE_NEAREST_OBJECT;
				}
				break;
			case MOVE_FOCUS_DOWN:
				if (oy > cy)						// 아랫쪽에 위치한 Object를 대상으로..
				{
					if ( ABS(ox-cx) < ABS(oy-cy)*3 )// 세로로 배치된 Object 라면, (가로 거리가 세로거리의 3배 이하 = 약 30도 각도이상..)
						REPLACE_NEAREST_OBJECT;
				}
				break;
			default:
				return	GWM_RET_OUT_OF_RANGE;
			}
		}
		objList = (ONDK_GWM_GuiObject_t)gwm_lnklst_GetNext((LINK_OBJECT)objList);
	}

	if ( nextFocus == NULL)								// 다음 Object를 찾지 못했다.
	{
		return	GWM_RET_NO_OBJECT;
	}



	obj->focus &= ~GUIOBJ_HILIGHTED;					// Focus 를 없앤다.
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_LOST_FOCUS, obj, direction);

	nextFocus->focus |= GUIOBJ_HILIGHTED;				// 새로운 Focus 를 준다.
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_GET_FOCUS, nextFocus, direction);

	/* 어플에게 자신의 obj의 focus가 변경되었다고 알린다. */
	ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_OBJ_FOCUS_CHANGED, 0, nextFocus->ID, obj->ID, direction, NULL);

	ONDK_Print("[%s:%d] (-)\r\n", __FUNCTION__, __LINE__);

	return	ONDK_MESSAGE_CONSUMED;
}


/**********************************************************************************************************/
#define ____DEFAULT_PROC_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC	ONDK_Result_t local_ondk_gwmobj_BaseDefaultMethod_Create(ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(object);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&object->rect);

	return GWM_MESSAGE_BREAK;
}

STATIC	ONDK_Result_t local_ondk_gwmobj_BaseDefaultMethod_Relocate(ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Rect_t *rect;
	ONDK_Rect_t updateRect;

	ONDK_UNUSED_ARGUMENT(object);
	ONDK_UNUSED_ARGUMENT(arg);

	rect = (ONDK_Rect_t*)arg;
	updateRect = ONDK_UnionRect(rect, &object->rect);
	HxSTD_MemCopy(&(object->rect), rect, sizeof(ONDK_Rect_t));
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&updateRect);

	return GWM_MESSAGE_BREAK;
}

STATIC	ONDK_Result_t local_ondk_gwmobj_BaseDefaultMethod_Visible(ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(object);
	ONDK_UNUSED_ARGUMENT(arg);

	object->isHide = arg;
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&object->rect);

	return GWM_MESSAGE_BREAK;
}

STATIC	ONDK_Result_t local_ondk_gwmobj_BaseDefaultMethod_GetFocus(ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(object);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&object->rect);

	return GWM_MESSAGE_BREAK;
}

STATIC	ONDK_Result_t local_ondk_gwmobj_BaseDefaultMethod_LostFocus(ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(object);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&object->rect);

	return GWM_MESSAGE_BREAK;
}

STATIC	ONDK_Result_t local_ondk_gwmobj_BaseDefaultMethod_ChangedValue(ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(object);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&object->rect);

	return GWM_MESSAGE_BREAK;
}

STATIC	ONDK_Result_t local_ondk_gwmobj_BaseDefaultMethod_SetValue(ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(object);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&object->rect);

	return GWM_MESSAGE_BREAK;
}

STATIC	ONDK_Result_t local_ondk_gwmobj_BaseDefaultMethod_Draw(ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t 		lRet = ONDK_RET_OK;
 	ONDK_Region_t		area;
	ONDKWindowHandle 	*screen = ONDK_GWM_GetCurSurface();

	ONDK_UNUSED_ARGUMENT(object);
	ONDK_UNUSED_ARGUMENT(arg);

	if( NULL == screen || NULL == g_pstCurrentApp)
	{
		return GWM_MESSAGE_BREAK;
	}

	if (NULL != object->objDrawMethod)
	{
		ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);

		lRet = object->objDrawMethod(screen, &area, &object->rect, (void *)object, (void *)object, arg);
	}

	return lRet;
}


ONDK_Result_t		INT_ONDK_GWM_OBJ_BaseDefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t lRet = ONDK_RET_OK;

	switch(proc_msg)
	{
	/*
	case MT_DESTROY:
		//Base Default Method를 실행하기 전에, object가 소멸되어 있다.
		break;
	*/

	case MT_CREATE:
		lRet = local_ondk_gwmobj_BaseDefaultMethod_Create(object, arg);
		break;

	case MT_RELOCATE:
		lRet = local_ondk_gwmobj_BaseDefaultMethod_Relocate(object, arg);
		break;

	case MT_VISIBLE:
		lRet = local_ondk_gwmobj_BaseDefaultMethod_Visible(object, arg);
		break;

	case MT_GET_FOCUS:
		lRet = local_ondk_gwmobj_BaseDefaultMethod_GetFocus(object, arg);
		break;

	case MT_LOST_FOCUS:
		lRet = local_ondk_gwmobj_BaseDefaultMethod_LostFocus(object, arg);
		break;

	case MT_CHANGED_VALUE:
		lRet = local_ondk_gwmobj_BaseDefaultMethod_ChangedValue(object, arg);
		break;

	case MT_SET_VALUE:
		lRet = local_ondk_gwmobj_BaseDefaultMethod_SetValue(object, arg);
		break;

	case MT_DRAW:
		lRet = local_ondk_gwmobj_BaseDefaultMethod_Draw(object, arg);
		break;

	default:
		break;
	}

	return lRet;
}


/* End of File. ---------------------------------------------------------- */


