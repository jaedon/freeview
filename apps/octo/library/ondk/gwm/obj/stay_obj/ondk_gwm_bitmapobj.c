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
#include <osd_gfx.h>
#include <gwm_objdef.h>

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define	MAX_BITMAP_OBJECT						64

/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */
struct tagBitmapObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.
	unsigned char			*bitmap;
};

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static struct tagBitmapObject			sBitmapArray[MAX_BITMAP_OBJECT];
static char							isAllocated[MAX_BITMAP_OBJECT];

/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */

static BITMAP_OBJECT		local_gwmbmp_AllocNewBitmapObj(void)
{
	int		i;

	for (i = 0; i < MAX_BITMAP_OBJECT; i++)
	{
		if (! isAllocated[i])
			break;
	}
	if (i < MAX_BITMAP_OBJECT)
	{
		isAllocated[i] = 1;
		return &sBitmapArray[i];
	}
	return NULL;
}

static void				local_gwmbmp_FreeBitmapObj(BITMAP_OBJECT obj)
{
	int		i;

	GWM_assert(obj);
#ifdef CONFIG_DEBUG
	if (obj->link != NULL)
		GWM_DBG_Print(DBGMSG_OBJ, ("[ WARNING ] Object(%d):%08X is currently linked.\n", obj->ID, (int)obj));
#endif
	for (i = 0; i < MAX_BITMAP_OBJECT; i++)
	{
		if (obj == &(sBitmapArray[i]))
			break;
	}

	if (i < MAX_BITMAP_OBJECT)
	{
		isAllocated[i] = 0;
		INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(sBitmapArray[i]));
		sBitmapArray[i].bitmap = 0;
	}
	return;
}


static ONDK_Result_t		local_gwmbmp_DefaultBitmapProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	BITMAP_OBJECT	bitmap;
	ONDK_Region_t		area;
	ONDK_Rect_t		rect;

	GWM_assert(object->type == GUIOBJ_BITMAP);
	bitmap = (BITMAP_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwmbmp_FreeBitmapObj(bitmap);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			if (object->objDrawMethod != NULL)
			{	/* draw with customized drawing method */
				object->objDrawMethod(ONDK_GWM_GetSurface(), &area, &rect, object, (BITMAP_OBJECT)bitmap, arg);
			}
			else
			{	/* draw via default method */
				if (bitmap->bitmap != NULL)
				{
#if 0
				OSD_GFX_DrawStockImage(ONDK_GWM_GetSurface(), area.x1 + bitmap->rect.x,
										area.y1 + bitmap->rect.y, bitmap->bitmap, 100);
#else
					if (arg > 0)
						ONDK_GWM_GFX_DrawBitmap(ONDK_GWM_GetSurface(), area.x1 + bitmap->rect.x,
										area.y1 + bitmap->rect.y, bitmap->bitmap, eBmpBlend_COPY);
					else
						OSD_GFX_DrawDimmedBitmap(ONDK_GWM_GetSurface(), area.x1 + bitmap->rect.x,
										area.y1 + bitmap->rect.y, bitmap->bitmap, eBmpBlend_COPY, DEFAULT_DIMMED_LEVEL);
#endif /* CONFIG_MWC_OSD_16BIT_TRUECOLOR */
				}
			}
			return GWM_MESSAGE_BREAK;
		}
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*rc;

			rc = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(bitmap->rect.x, bitmap->rect.y, bitmap->rect.w, bitmap->rect.h);
			VK_memcpy(&(bitmap->rect), rc, sizeof(ONDK_Rect_t));
			ONDK_GWM_APP_InvalidateRelativeRect(bitmap->rect.x, bitmap->rect.y, bitmap->rect.w, bitmap->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_SET_VALUE:
		{
			bitmap->bitmap = (unsigned char *)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(bitmap->rect.x, bitmap->rect.y, bitmap->rect.w, bitmap->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}


void		gwm_bitmap_InitClass(void)
{
	int		i;

	for (i = 0; i < MAX_BITMAP_OBJECT; i++)
	{
		sBitmapArray[i].link = NULL;
		memset(&(sBitmapArray[i].rect), 0, sizeof(ONDK_Rect_t));
		sBitmapArray[i].ID = -1;
		sBitmapArray[i].type = GUIOBJ_BITMAP;
		sBitmapArray[i].focus = GUIOBJ_DISABLED;
		sBitmapArray[i].Proc = local_gwmbmp_DefaultBitmapProc;
		sBitmapArray[i].objDrawMethod = NULL;

		sBitmapArray[i].bitmap = NULL;
		isAllocated[i] = 0;
	}
}

/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

ONDK_Result_t			GWM_Bitmap_Create(int id, ONDK_Rect_t *rect, unsigned char *bitmap)
{
	ONDK_GWM_GuiObject_t		objlist;
	BITMAP_OBJECT		new_bitmap;
	ONDK_Result_t			hresult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_bitmap = (BITMAP_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_bitmap != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_bitmap = local_gwmbmp_AllocNewBitmapObj();
	if (new_bitmap == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	new_bitmap->link = NULL;
	VK_memcpy(&(new_bitmap->rect), rect, sizeof(ONDK_Rect_t));
	new_bitmap->ID = id;
	new_bitmap->type = GUIOBJ_BITMAP;
	new_bitmap->focus = GUIOBJ_DISABLED;
	new_bitmap->Proc = local_gwmbmp_DefaultBitmapProc;
	new_bitmap->objDrawMethod = NULL;

	new_bitmap->bitmap = bitmap;
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)new_bitmap, 0);

	hresult = INT_ONDK_GWM_APP_AddObject( g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_bitmap);
	ONDK_GWM_APP_InvalidateRelativeRect(	new_bitmap->rect.x,
						new_bitmap->rect.y,
						new_bitmap->rect.w,
						new_bitmap->rect.h);

	return hresult;
}


ONDK_Result_t			GWM_Bitmap_ChangeBmp(int id, unsigned char *bitmap)
{
	ONDK_GWM_GuiObject_t		obj;

	obj = ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_BITMAP)
		return GWM_RET_NO_OBJECT;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_SET_VALUE, obj, (int)bitmap);
	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}


char *				GWM_Bitmap_GetBmpPtrByObject(BITMAP_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_BITMAP);
	return obj->bitmap;
}


/* End of File. ---------------------------------------------------------- */

