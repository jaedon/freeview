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
#include <osd_font.h>
#include <osd_gfx.h>
#include <gwm_objdef.h>

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define MAX_PICTURE_OBJECT					64

/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */

struct	tagPictureObject
{
	GUIOBJ_BASIC_PROPERTY	// 반드시 맨 앞에 존재하도록 할 것.

	HINT32				image;
	OSD_BmpBlendMode_t	blend;
	HBOOL				bHasCaption;
	PictureCaption_t	caption;
};

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static struct	tagPictureObject	sPictureArray[MAX_PICTURE_OBJECT];
static char						isAllocated[MAX_PICTURE_OBJECT];
static GWM_ObjDraw_t			s_defaultDraw;

/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */


static PICTURE_OBJECT	local_gwmpicture_AllocNewPictureObj(void)
{
	int		i;

	for (i = 0; i < MAX_PICTURE_OBJECT; i++)
	{
		if (!isAllocated[i])
			break;
	}
	if (i < MAX_PICTURE_OBJECT)
	{
		isAllocated[i] = 1;
		return &sPictureArray[i];
	}
	return NULL;
}

static void		local_gwmpicture_FreePictureObj(PICTURE_OBJECT obj)
{
	int		i;

	GWM_assert(obj);
#if defined(CONFIG_DEBUG)
	if (obj->link != NULL)
	{
		GWM_DBG_Print(DBGMSG_OBJ, ("[ WARNING ] Object(%d):%08X is currently linked.\n", obj->ID, (int)obj ));
	}
#endif
	for (i = 0; i < MAX_PICTURE_OBJECT; i++)
	{
		if (obj == &(sPictureArray[i]))
			break;
	}

	if (i < MAX_PICTURE_OBJECT)
	{
		isAllocated[i] = 0;
		INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(sPictureArray[i]));
		sPictureArray[i].image		= 0;
		sPictureArray[i].blend		= 0;
		sPictureArray[i].bHasCaption	= FALSE;
		VK_memset(&sPictureArray[i].caption, 0, sizeof(PictureCaption_t));
	}
}



static ONDK_Result_t	local_gwmpicture_defaultPictureProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	PICTURE_OBJECT	picture;
	ONDK_Region_t		area;
	ONDK_Rect_t		rect;

	GWM_assert(object->type == GUIOBJ_PICTURE);
	picture = (PICTURE_OBJECT)object;

	switch(proc_msg)
	{
		case MT_DESTROY:
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwmpicture_FreePictureObj(picture);
			return GWM_MESSAGE_BREAK;

		case MT_DRAW:
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			if (object->objDrawMethod != NULL)
			{	/* draw picture with customized callback */
				object->objDrawMethod(ONDK_GWM_GetSurface(), &area, &rect, object, (void *)picture->blend, arg);
			}
			else if (s_defaultDraw)
			{
				s_defaultDraw(ONDK_GWM_GetSurface(), &area, &rect, object, (void *)picture->blend, arg);
			}
			else
			{
				/**
				 * Picture Object는 Stock Image의 사용을 하기 때문에, AP(HD또는 SD)에 따라 ap와 종속성이 생기기
				 * 때문에, default draw를 따로 제공하지 않는다.
				 * ex) AP_StockImage가 common module이 아닌, 각 ui별 모듈(물론 모두 쓰이긴 하지만.. 그래도 종속성을 줄이기 위함)
				 */
			 	GWM_assert(FALSE);
			}
			return GWM_MESSAGE_BREAK;

		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(picture->rect.x, picture->rect.y, picture->rect.w, picture->rect.h);
			VK_memcpy(&(picture->rect), new_rect, sizeof(ONDK_Rect_t));
			ONDK_GWM_APP_InvalidateRelativeRect(picture->rect.x, picture->rect.y, picture->rect.w, picture->rect.h);
			return GWM_MESSAGE_BREAK;
		}

		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}



/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

void			gwm_picture_InitClass(void)
{
	int		i;

	for (i=0; i<MAX_PICTURE_OBJECT; i++)
	{
		sPictureArray[i].link = NULL;
		VK_memset( &(sPictureArray[i].rect), 0, sizeof(ONDK_Rect_t));
		sPictureArray[i].ID = -1;
		sPictureArray[i].type = GUIOBJ_PICTURE;
		sPictureArray[i].attr = 0;
		sPictureArray[i].focus = GUIOBJ_DISABLED;
		sPictureArray[i].Proc = local_gwmpicture_defaultPictureProc;
		sPictureArray[i].objDrawMethod = NULL;

		isAllocated[i] = 0;
	}
}


ONDK_Result_t	GWM_Picture_SetDefaultDraw (/*GWM_ObjDraw_t*/void * defaultDraw)
{
	s_defaultDraw	= (GWM_ObjDraw_t)defaultDraw;
	return GWM_RET_OK;
}


ONDK_Result_t	GWM_Picture_Create(int id, ONDK_Rect_t *rect, HINT32 nImgEnum, OSD_BmpBlendMode_t mode, PictureCaption_t *pCaption)
{
	ONDK_GWM_GuiObject_t	objlist;
	PICTURE_OBJECT	picture;
	ONDK_Result_t	result;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	if (rect == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	picture = (PICTURE_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (picture)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	picture = local_gwmpicture_AllocNewPictureObj();
	if (picture == NULL)
	{
		return GWM_RET_OUT_OF_MEMORY;
	}

	picture->link	= NULL;
	VK_memcpy(&(picture->rect), rect, sizeof(ONDK_Rect_t));
	picture->ID	= id;
	picture->type	= GUIOBJ_PICTURE;
	picture->attr	= 0;
	picture->focus	= GUIOBJ_DISABLED;
	picture->Proc	= local_gwmpicture_defaultPictureProc;
	picture->objDrawMethod	= NULL;

	picture->image	= nImgEnum;
	picture->blend	= mode;
	picture->bHasCaption	= FALSE;

	(void) GWM_Picture_SetCaption(picture, pCaption);

	result	= INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)picture);
	ONDK_GWM_APP_InvalidateRelativeRect(picture->rect.x, picture->rect.y, picture->rect.w, picture->rect.h);

	return	result;
}


ONDK_Result_t	GWM_Picture_SetImage (PICTURE_OBJECT picture, HINT32 nImage)
{
	if (picture == NULL || picture->type != GUIOBJ_PICTURE)
	{
		return GWM_RET_SOURCE_NULL;
	}

	picture->image	= nImage;

	ONDK_GWM_APP_InvalidateObject(ONDK_GWM_Obj_GetObjectID((ONDK_GWM_GuiObject_t)picture));

	return GWM_RET_OK;
}


ONDK_Result_t	GWM_Picture_GetImage (PICTURE_OBJECT picture, HINT32 *pnImage)
{
	if (picture == NULL || pnImage == NULL || picture->type != GUIOBJ_PICTURE)
	{
		return GWM_RET_SOURCE_NULL;
	}

	*pnImage	= picture->image;

	return GWM_RET_OK;
}


ONDK_Result_t	GWM_Picture_SetCaption (PICTURE_OBJECT picture, PictureCaption_t *pCaption)
{
	if (picture == NULL || pCaption == NULL || picture->type != GUIOBJ_PICTURE)
	{
		return GWM_RET_SOURCE_NULL;
	}

	VK_memcpy(&picture->caption, pCaption, sizeof(PictureCaption_t));
	picture->bHasCaption	= TRUE;

	ONDK_GWM_APP_InvalidateObject(ONDK_GWM_Obj_GetObjectID((ONDK_GWM_GuiObject_t)picture));

	return GWM_RET_OK;
}


ONDK_Result_t	GWM_Picture_GetCaption (PICTURE_OBJECT picture, PictureCaption_t *pCaption)
{
	if (picture == NULL || pCaption == NULL || picture->type != GUIOBJ_PICTURE)
	{
		return GWM_RET_SOURCE_NULL;
	}

	if (picture->bHasCaption == FALSE)
	{
		return ERR_FAIL;
	}

	VK_memcpy(pCaption, &picture->caption, sizeof(PictureCaption_t));

	return GWM_RET_OK;
}


/* End of File. ---------------------------------------------------------- */


