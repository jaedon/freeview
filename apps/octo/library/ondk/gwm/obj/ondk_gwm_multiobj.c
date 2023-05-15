
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

struct tagMultiObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.
	HUINT32							title_bufsize;
	HUINT8	*						leftStr;
	HUINT8	*						rightStr;
    HUINT8	*						centerStr;
	HUINT16							size;
	HUINT8	*						extraText;

	ONDKSurfaceTextFlags 			align;

	ONDK_Color_t					color_over;
	ONDK_Color_t					color_normal;
	ONDK_Color_t					color_click;
	ONDK_Color_t					color_disable;
	ONDK_Color_t					color_fontcolor;
	ONDK_Color_t					color_fontfocus;

	HCHAR							*focusImgPath;
	HCHAR							*normalImgPath;

	ONDKImageHandle					hfocusImg;
	ONDKImageHandle					hnormalImg;

	HBOOL 							focusFlag; //버튼 테두리 없이 그리는 flag
	ONDKFontHandle					*hFontHandle;
	HUINT32							imgStrOffset; //image와 string사이의 간력
	ONDK_Rect_t                     imgRect; //image position
	ONDK_Rect_t                     strRect; //image position
};

STATIC struct	tagMultiObject			s_defMultiObj;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC MULTI_OBJECT		local_gwmmulti_AllocNewMultiObj(void)
{
	struct tagMultiObject	*pObj = &s_defMultiObj;

	pObj = (struct tagMultiObject *)ONDK_Malloc(sizeof(struct tagMultiObject));
	ONDK_Assert(pObj);
	if (pObj == NULL)
		return NULL;

	HxSTD_MemCopy(pObj, &s_defMultiObj, sizeof(struct tagMultiObject));

	return pObj;
}

STATIC void		local_gwmmulti_FreeMultiObj(MULTI_OBJECT obj)
{
	ONDK_Assert(obj);

	if(obj->hfocusImg)		ONDK_IMG_Destroy(obj->hfocusImg);

	if(obj->hFontHandle != NULL)
	{
		ONDK_FONT_Release (obj->hFontHandle);
	}
	ONDK_Free(obj);
}

/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC ONDK_Result_t	local_gwmmulti_DefaultMethod_Destory(MULTI_OBJECT btn, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(btn);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&btn->rect);
	local_gwmmulti_FreeMultiObj(btn);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwmmulti_DefaultMethod_Draw(MULTI_OBJECT obj, HINT32 arg)
{
	HINT32		x = 0, y = 0, w = 0, h = 0, x1 = 0, y1 = 0;
	HINT32		push_status = 0;
	HCHAR 		*leftStr = NULL;
	HCHAR 		*rightStr = NULL;
   	HCHAR 		*centerStr = NULL;
	ONDK_Color_t		btnColor=0;
	ONDK_Color_t		bright, fontcolor;
	ONDKFontHandle		*font;
	ONDK_Region_t		area;
	ONDK_Rect_t			rect, drawRect;
	ONDKSurfaceHandle 	screen = ONDK_GWM_GetCurSurface();
	ONDKImageHandle		img = NULL;
	ONDK_Size_t			imgSize = {0, 0};
	HBOOL				focusFlag = FALSE;
	HUINT32				offset = 0;
    HINT32      imgX=0,imgY =0;
    HINT32      strX=0,strY =0;
	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)obj, &rect);

	drawRect = ONDK_Rect(area.x1 + rect.x, area.y1 + rect.y, rect.w, rect.h);

	push_status = obj->focus;

	x1 = drawRect.x;
	y1 = drawRect.y;
	w = drawRect.w;
	h = drawRect.h;

	leftStr = (HCHAR*)obj->leftStr;
	rightStr = (HCHAR*)obj->rightStr;
    centerStr = (HCHAR*)obj->centerStr;
	fontcolor = obj->color_fontcolor;
	focusFlag = obj->focusFlag;
	btnColor = obj->color_normal;
	img = obj->hnormalImg;
	font = obj->hFontHandle;
	offset = obj->imgStrOffset;

    imgX = obj->imgRect.x;
    imgY = obj->imgRect.y;

    strX = obj->strRect.x;
    strY = obj->strRect.y;


	if(TRUE == focusFlag)
	{
		if (push_status & GUIOBJ_PUSHED)
		{
			btnColor = obj->color_click;
		}
		else if (push_status & GUIOBJ_DISABLED)
		{
			btnColor = obj->color_disable;
		}
		else if (push_status & GUIOBJ_HILIGHTED)
		{
			btnColor = obj->color_over;
			fontcolor = obj->color_fontfocus;
			img = obj->hfocusImg;
		}
		bright = ONDK_GWM_GFX_GetSysDimmedColor(screen, btnColor, 150);

		ONDK_DRAW_FillRectElement(screen, x1, y1, w, h, bright);
	}


	if (img)
	{
		if(NULL != leftStr)
		{
			imgSize = ONDK_IMG_GetSize(img);
			ONDK_IMG_Blit(img, screen, x1, y1 + (h-imgSize.h)/2);
		}

		if(NULL != rightStr)
		{
			imgSize = ONDK_IMG_GetSize(img);
			ONDK_IMG_Blit(img, screen, (x1 + w - imgSize.w), y1 + (h-imgSize.h)/2);
		}

        if(NULL != centerStr)
        {
			imgSize = ONDK_IMG_GetSize(img);
            if(imgX!=-1 && imgY!=-1){
			    ONDK_IMG_Blit(img, screen, x1+imgX, y1 + imgY);
            }else{
                ONDK_IMG_Blit(img, screen, x1, y1 + (h-imgSize.h)/2);
            }


        }
	}

	if (NULL != font)
	{
		if(NULL != leftStr)
		{
			x = x1 + imgSize.w + offset;
			y = y1 + (h/2) - (ONDK_FONT_GetHeight(font)/2);
			ONDK_FONT_DrawStringAlignAbbr(screen, font, leftStr, x, y, w, fontcolor, ONDKSTF_LEFT);
		}
		if(NULL != rightStr)
		{
			x = x1;
			y = y1 + (h/2) - (ONDK_FONT_GetHeight(font)/2);
			w -= (imgSize.w + offset);
			ONDK_FONT_DrawStringAlignAbbr(screen, font, rightStr, x, y, w, fontcolor, ONDKSTF_RIGHT);
		}

		if(NULL != centerStr)
		{
            if(strX!=-1 && strY!=-1)
            {
                x = x1 + strX;
                y = y1 + strY;
                w = imgSize.w;
                ONDK_FONT_DrawStringAlignAbbr(screen, font, centerStr, x, y, w, fontcolor, ONDKSTF_MIDDLE);
            }
            else if(imgX!=-1 && imgY!=-1)
            {
                x = x1 + imgX;
    			y = y1 + imgY +(imgSize.h/2) - (ONDK_FONT_GetHeight(font)/2);
                w = (imgSize.w + offset);
                ONDK_FONT_DrawStringAlignAbbr(screen, font, centerStr, x, y, w, fontcolor, ONDKSTF_CENTER);
            }
            else
            {
       			x = x1;
    			y = y1 + (h/2) - (ONDK_FONT_GetHeight(font)/2);
                w = (imgSize.w + offset);
                ONDK_FONT_DrawStringAlignAbbr(screen, font, centerStr, x, y, w, fontcolor, ONDKSTF_CENTER);
            }

		}

	}

	ONDK_UNUSED_ARGUMENT(arg);

	return ONDK_MESSAGE_BREAK;
}


STATIC ONDK_Result_t	local_gwmmulti_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	ONDK_Result_t	lRet = ONDK_RET_OK;

	MULTI_OBJECT		multi;
	GWM_assert(object);
	GWM_assert(object->type == GUIOBJ_MULTI);
	multi = (MULTI_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
			lRet = local_gwmmulti_DefaultMethod_Destory((MULTI_OBJECT)multi, arg);
			return GWM_MESSAGE_BREAK;
		case MT_DRAW:

			if (multi->objDrawMethod == NULL)
			{
				lRet = local_gwmmulti_DefaultMethod_Draw((MULTI_OBJECT)multi, arg);
			}
			break;
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(multi->rect.x, multi->rect.y, multi->rect.w, multi->rect.h);
			VK_memcpy(&(multi->rect), new_rect, sizeof(ONDK_Region_t));
			ONDK_GWM_APP_InvalidateRelativeRect(multi->rect.x, multi->rect.y, multi->rect.w, multi->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_CHANGE_TEXT:
		{
			HCHAR 	*new_text;
			new_text = (HCHAR *)arg;
			//btn->title= new_text;
			ONDK_GWM_APP_InvalidateRelativeRect(multi->rect.x, multi->rect.y, multi->rect.w, multi->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_KEYDOWN:
		{
			if (arg == KEY_OK || arg == KEY_FRONT_OK)
			{
				multi->focus |= GUIOBJ_PUSHED;
				ONDK_GWM_APP_InvalidateRelativeRect(multi->rect.x, multi->rect.y, multi->rect.w, multi->rect.h);
				return ONDK_MESSAGE_CONSUMED;
			}
			return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
		}
		case MT_KEYUP:
			if ( ( arg == KEY_OK || arg == KEY_FRONT_OK ))
			{
				multi->focus &= ~GUIOBJ_PUSHED;
				ONDK_GWM_APP_InvalidateRelativeRect(multi->rect.x, multi->rect.y, multi->rect.w, multi->rect.h);

				/* current app usually doesn't need specific session handle fo key-down event */
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_CLICKED, (Handle_t)NULL, multi->ID, arg, (HINT32)multi, NULL);
				return ONDK_MESSAGE_CONSUMED;
			}
			return GWM_MESSAGE_BREAK;

		default:
			break;
	}

	return INT_ONDK_GWM_OBJ_BaseDefaultMethod(proc_msg, object, arg);
}



/**********************************************************************************************************/
#define ____INTERNAL_FUNCTIONS_______________________
/**********************************************************************************************************/
void	INT_ONDK_GWM_COM_Multi_InitClass(void)
{
	struct tagMultiObject *pObj = &s_defMultiObj;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagMultiObject));

	pObj->link = NULL;
	pObj->ID = -1;
	pObj->type = GUIOBJ_MULTI;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwmmulti_DefaultMethod;
	pObj->objDrawMethod = NULL;

	pObj->title_bufsize = 0;
	pObj->leftStr = NULL;
	pObj->rightStr = NULL;
   	pObj->centerStr = NULL;
	pObj->extraText = NULL;
	pObj->align = ONDKSTF_LEFT;

	pObj->hfocusImg = NULL;
	pObj->hnormalImg = NULL;

	pObj->color_over 		= 0xFF0000C0;
	pObj->color_normal 		= 0xFF000080;
	pObj->color_click 		= 0xFF0000FF;
	pObj->color_disable		= 0xFF000040;
	pObj->color_fontcolor 	= 0xFF820610;
	pObj->color_fontfocus	= 0xFF820610;

	pObj->focusFlag			=  TRUE;
	pObj->hFontHandle		= NULL;
	pObj->imgStrOffset		= 0;

    pObj->imgRect.x = -1;
    pObj->imgRect.y = -1;

    pObj->strRect.x = -1;
    pObj->strRect.y = -1;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t	ONDK_GWM_COM_Multi_Create(HINT32 id, ONDK_Rect_t *rect)
{
	ONDK_GWM_GuiObject_t		objlist;
	MULTI_OBJECT		new_btn;
	ONDK_Result_t			hresult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_btn = (MULTI_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_btn != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_btn = local_gwmmulti_AllocNewMultiObj();
	if (new_btn == NULL)
		return GWM_RET_OUT_OF_MEMORY;

	HxSTD_MemCopy(&(new_btn->rect), rect, sizeof(ONDK_Rect_t));
	new_btn->ID = id;

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_btn);
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&new_btn->rect);


	return hresult;
}

ONDK_Result_t				ONDK_GWM_COM_Multi_ChangeFocusImage(HINT32 id, HCHAR *imagePath)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;


	if(imagePath != NULL)
	{
		if(obj->focusImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->focusImgPath);
		}
		obj->focusImgPath = imagePath;
		obj->hfocusImg = ONDK_IMG_CreateSelf((HUINT8*)imagePath);
	}

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t				ONDK_GWM_COM_Multi_ChangeNormalImage(HINT32 id, HCHAR *imagePath)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	if(NULL != imagePath)
	{
		if(obj->normalImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->normalImgPath);
		}

		obj->normalImgPath = imagePath;
		obj->hnormalImg = ONDK_IMG_CreateSelf((HUINT8*)imagePath);
	}

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_COM_Multi_GetImage(HINT32 id, HCHAR **normal)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	*normal = obj->focusImgPath;

	return ONDK_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_GetImageByObj(MULTI_OBJECT obj, HCHAR **normal)
{
	if (obj == NULL)
		return	GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return	GWM_RET_NO_OBJECT;

	*normal = obj->focusImgPath;

	return ONDK_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_SetFocusColor(HINT32 id, ONDK_Color_t over, ONDK_Color_t normal, ONDK_Color_t click, ONDK_Color_t disable)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	obj->color_over = over;
	obj->color_normal = normal;
	obj->color_click = click;
	obj->color_disable = disable;
	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t				ONDK_GWM_COM_Multi_SetFontColor(HINT32 id, ONDK_Color_t fontColor, ONDK_Color_t fontfocus)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	obj->color_fontcolor = fontColor;
	obj->color_fontfocus = fontfocus;
	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t				ONDK_GWM_COM_Multi_SetStatus(HINT32 id, HINT32 push_status)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	obj->focus &= ~GUIOBJ_PUSHED;
	obj->focus &= ~GUIOBJ_HILIGHTED;
	obj->focus &= ~GUIOBJ_DISABLED;
	obj->focus &= ~GUIOBJ_NORMAL;

#if 0
	if (push_status & GUIOBJ_NORMAL)
	{
		;
	}
#endif

	if(push_status & GUIOBJ_HILIGHTED)
	{
		obj->focus |= GUIOBJ_HILIGHTED;
	}
	if(push_status & GUIOBJ_PUSHED)
	{
		obj->focus |= GUIOBJ_PUSHED;
	}
	if(push_status & GUIOBJ_DISABLED)
	{
		obj->focus |= GUIOBJ_DISABLED;
	}

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_GetStr(HINT32 id, HUINT8 **leftStr, HUINT8 **rightStr)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	GWM_assert(obj->type == GUIOBJ_MULTI);

	*leftStr = obj->leftStr;
	*rightStr = obj->rightStr;

	return GWM_RET_OK;
}

HUINT32	ONDK_GWM_COM_Multi_GetStatusByObj(MULTI_OBJECT obj)
{
	GWM_assert(obj->type == GUIOBJ_MULTI);
	return obj->focus;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeLeftStr (HINT32 id, HCHAR *szNewText)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	if (szNewText)
	{
		obj->leftStr = (HUINT8*)szNewText;
	}

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeRightStr (HINT32 id, HCHAR *szNewText)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	if (szNewText)
	{
		obj->rightStr = (HUINT8*)szNewText;
	}

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeCenterStr (HINT32 id, HCHAR *szNewText){
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

    if(obj->centerStr!=NULL){
        ONDK_Free(obj->centerStr);
    }

	if (szNewText)
	{
		obj->centerStr = (HUINT8*)szNewText;
	}

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}



ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeImagePosition (HINT32 id, 	HINT32	x,	HINT32	y){
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

    obj->imgRect.x=x;
    obj->imgRect.y=y;

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeStrPosition (HINT32 id, 	HINT32	x,	HINT32	y){
    MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

    obj->strRect.x=x;
    obj->strRect.y=y;

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_SetAlign(HINT32 id, ONDKSurfaceTextFlags align)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	obj->align=align;

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_COM_Multi_GetTitleSize(HINT32 id, HUINT16 *size)
{
	MULTI_OBJECT		obj;

	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	*size = obj->size;

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_GetTitleSizeByObj(MULTI_OBJECT	obj, HUINT16 *size)
{
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	*size = obj->size;

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_SetContents(HUINT32 id, HUINT8 *normalPath, HUINT8 *focusPath, HUINT8 *leftStr, HUINT8 *rightStr, HUINT32 offset)
{

	MULTI_OBJECT		obj;
	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	if(NULL != normalPath)
	{
		if(obj->normalImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->normalImgPath);
		}

		obj->normalImgPath = (HCHAR*)normalPath;
		obj->hnormalImg = ONDK_IMG_CreateSelf(normalPath);
	}

	if(NULL != focusPath)
	{
		if(obj->focusImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->focusImgPath);
		}

		obj->focusImgPath = (HCHAR*)focusPath;
		obj->hfocusImg = ONDK_IMG_CreateSelf(focusPath);
	}

	obj->leftStr = leftStr;
	obj->rightStr = rightStr;
	obj->imgStrOffset = offset;
	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_SetFocusFlag(HUINT32 id, HBOOL flag)
{
	MULTI_OBJECT		obj;
	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;

	obj->focusFlag = flag;

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Multi_SetFontHandle(HINT32 id, ONDK_FontId_t fontId, HINT32 fontSize)
{
	MULTI_OBJECT		obj;
	ONDKFontHandle	*handle;
	obj = (MULTI_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_MULTI)
		return GWM_RET_NO_OBJECT;


	if(obj->hFontHandle != NULL)
	{
		ONDK_FONT_Release (obj->hFontHandle);

		handle = ONDK_FONT_CreateSystemFont(fontId, fontSize);
	}
	else
	{
		handle = ONDK_FONT_CreateSystemFont(fontId, fontSize);
	}

	if(handle == NULL)
	{
		return GWM_RET_FAIL;
	}
	else
	{
		obj->hFontHandle = handle;
	}
	return GWM_RET_OK;

}



/* End of File. ---------------------------------------------------------- */


