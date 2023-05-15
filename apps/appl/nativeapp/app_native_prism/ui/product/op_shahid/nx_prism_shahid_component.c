/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_prism_shahid_home.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/
#if defined(CONFIG_OP_SHAHID_APPS)
/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_menu_contents.h>

#include <nx_prism_shahid.h>



/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
ContentsInfo_t *pstContents;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_SHAHID_COM_SetContentsItem(ContentsInfo_t *contentsInfo)
{
	pstContents = contentsInfo;
	return ONDK_RET_OK;
}

ONDK_Result_t	NX_SHAHID_COM_DrawSquareTemplate(HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex)
{
	ONDK_Color_t	value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	hFontHandle;
	HUINT32			ulFontSize;
	HUINT32			objStatus;
	HINT32			currentIndex=0, thickness=0;
	ONDK_Rect_t		rect;

	currentIndex = ONDK_GWM_COM_GridList_GetCurrentIndexByObjID(id);
	value_fontColor = COL(C_T_Gong02);

#if !defined(CONFIG_PROD_OS_EMULATOR)
	if (HLIB_FILE_Exist(pstContents[drawItem->item_index].pszThumbnailPath) == TRUE )
	{
		ONDK_Rect_t srcRect;
		ONDK_Size_t size;
		ONDKImageHandle hThum = NULL;

		hThum = ONDK_IMG_CreateSelf((HUINT8*)pstContents[drawItem->item_index].pszThumbnailPath);

		size = ONDK_IMG_GetSize(hThum);
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = size.w;
		srcRect.h = size.h;

		rect = ONDK_Rect(drawItem->itemRect.x,drawItem->itemRect.y,181,157);
		ONDK_SURFACE_StretchBlit(hThum, screen, srcRect, rect);

	}
#endif

	objStatus = ONDK_GWM_Obj_GetObjectStatus(ONDK_GWM_Obj_GetObjectByID(id));
	if(objStatus == GUIOBJ_HILIGHTED)
	{
		if(currentIndex == drawItem->item_index)
		{
			value_fontColor = COL(C_G_Shahid_03);
			eFontId = eFont_SystemBold;
			thickness = 4;
			cursorColor = COL(C_G_Shahid_03);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x, drawItem->itemRect.y, 181, thickness, cursorColor);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x, drawItem->itemRect.y+thickness, thickness, 157-thickness*2, cursorColor);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x+181-thickness, drawItem->itemRect.y+thickness, thickness, 157-thickness*2, cursorColor);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x, drawItem->itemRect.y+157-thickness, 181, thickness, cursorColor);
		}
	}
	rect = ONDK_Rect(drawItem->itemRect.x+3, drawItem->itemRect.y+3,36,36);
	ONDK_GUICOM_IMG_Blit(screen,rect,RES_600_11_PLUS_PNG);

	rect = ONDK_Rect(drawItem->itemRect.x, drawItem->itemRect.y+165,18,18);
	ONDK_GUICOM_IMG_Blit(screen,rect,RES_602_11_SHAHID_NEW_PNG);
	ulFontSize = NX_PRISM_FONT_SIZE_18;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pstContents[drawItem->item_index].pszProgramName,
						drawItem->itemRect.x+29, drawItem->itemRect.y+164,180, value_fontColor, ONDKSTF_LEFT);
		//ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pstContext->stArrayOfShahidShow[drawItem->item_index].name,
		//				drawItem->itemRect.x+29, drawItem->itemRect.y+167,181, title_fontColor, ONDKSTF_LEFT);
		ONDK_FONT_Release(hFontHandle);
	}

	return ONDK_RET_OK;

}

ONDK_Result_t	NX_SHAHID_COM_Draw16by9Template(HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex)
{
	ONDK_Color_t	value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	hFontHandle;
	HUINT32			ulFontSize;
	HUINT32			objStatus;
	HINT32			currentIndex=0, thickness=0;
	ONDK_Rect_t		rect;

	currentIndex = ONDK_GWM_COM_GridList_GetCurrentIndexByObjID(id);

	value_fontColor = COL(C_T_Gong02);

#if !defined(CONFIG_PROD_OS_EMULATOR)
	if (HLIB_FILE_Exist(pstContents[drawItem->item_index].pszThumbnailPath) == TRUE )
	{
		ONDK_Rect_t srcRect;
		ONDK_Size_t size;
		ONDKImageHandle hThum = NULL;

		hThum = ONDK_IMG_CreateSelf((HUINT8*)pstContents[drawItem->item_index].pszThumbnailPath);

		size = ONDK_IMG_GetSize(hThum);
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = size.w;
		srcRect.h = size.h;

		rect = ONDK_Rect(drawItem->itemRect.x,drawItem->itemRect.y,179,101);
		ONDK_SURFACE_StretchBlit(hThum, screen, srcRect, rect);

	}
#endif

	objStatus = ONDK_GWM_Obj_GetObjectStatus(ONDK_GWM_Obj_GetObjectByID(id));
	if(objStatus == GUIOBJ_HILIGHTED)
	{
		if(currentIndex == drawItem->item_index)
		{
			value_fontColor = COL(C_G_Shahid_03);
			eFontId = eFont_SystemBold;
			thickness = 4;
			cursorColor = COL(C_G_Shahid_03);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x, drawItem->itemRect.y, 179, thickness, cursorColor);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x, drawItem->itemRect.y+thickness, thickness, 101-thickness*2, cursorColor);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x+179-thickness, drawItem->itemRect.y+thickness, thickness, 101-thickness*2, cursorColor);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x, drawItem->itemRect.y+101-thickness, 179, thickness, cursorColor);
		}
	}

	rect = ONDK_Rect(drawItem->itemRect.x+3, drawItem->itemRect.y+3,36,36);
	ONDK_GUICOM_IMG_Blit(screen,rect,RES_600_11_PLUS_PNG);

	rect = ONDK_Rect(drawItem->itemRect.x, drawItem->itemRect.y+109,18,18);
	ONDK_GUICOM_IMG_Blit(screen,rect,RES_602_11_SHAHID_NEW_PNG);
	ulFontSize = NX_PRISM_FONT_SIZE_18;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pstContents[drawItem->item_index].pszProgramName,
						drawItem->itemRect.x+29, drawItem->itemRect.y+107,179, value_fontColor, ONDKSTF_LEFT);
		//ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pstContext->stArrayOfShahidShow[drawItem->item_index].name,
		//				drawItem->itemRect.x+29, drawItem->itemRect.y+167,181, title_fontColor, ONDKSTF_LEFT);
		ONDK_FONT_Release(hFontHandle);
	}

	return ONDK_RET_OK;
}

ONDK_Result_t	NX_SHAHID_COM_DrawPosterTemplate(HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex)
{
	ONDK_Color_t	value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	hFontHandle;
	HUINT32			ulFontSize;
	HINT32			objStatus;
	HINT32			currentIndex=0, thickness=0;
	ONDK_Rect_t		rect;

	currentIndex = ONDK_GWM_COM_GridList_GetCurrentIndexByObjID(id);
	value_fontColor = COL(C_T_Gong02);

#if !defined(CONFIG_PROD_OS_EMULATOR)
	if (HLIB_FILE_Exist(pstContents[drawItem->item_index].pszThumbnailPath) == TRUE )
	{
		ONDK_Rect_t srcRect;
		ONDK_Size_t size;
		ONDKImageHandle hThum = NULL;

		hThum = ONDK_IMG_CreateSelf((HUINT8*)pstContents[drawItem->item_index].pszThumbnailPath);

		size = ONDK_IMG_GetSize(hThum);
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = size.w;
		srcRect.h = size.h;

		rect = ONDK_Rect(drawItem->itemRect.x,drawItem->itemRect.y,126,178);
		ONDK_SURFACE_StretchBlit(hThum, screen, srcRect, rect);

	}
#endif
	objStatus = ONDK_GWM_Obj_GetObjectStatus(ONDK_GWM_Obj_GetObjectByID(id));
	if(objStatus == GUIOBJ_HILIGHTED)
	{
		if(currentIndex == drawItem->item_index)
		{
			value_fontColor = COL(C_G_Shahid_03);
			eFontId = eFont_SystemBold;
			thickness = 4;
			cursorColor = COL(C_G_Shahid_03);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x, drawItem->itemRect.y, 126, thickness, cursorColor);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x, drawItem->itemRect.y+thickness, thickness, 178-thickness*2, cursorColor);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x+126-thickness, drawItem->itemRect.y+thickness, thickness, 178-thickness*2, cursorColor);
			ONDK_DRAW_FillRectElement(screen, drawItem->itemRect.x, drawItem->itemRect.y+178-thickness, 126, thickness, cursorColor);
		}
	}

	rect = ONDK_Rect(drawItem->itemRect.x+3, drawItem->itemRect.y+3,36,36);
	ONDK_GUICOM_IMG_Blit(screen,rect,RES_600_11_PLUS_PNG);

	rect = ONDK_Rect(drawItem->itemRect.x, drawItem->itemRect.y+186,18,18);
	ONDK_GUICOM_IMG_Blit(screen,rect,RES_602_11_SHAHID_NEW_PNG);

	ulFontSize = NX_PRISM_FONT_SIZE_18;
	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pstContents[drawItem->item_index].pszProgramName,
						drawItem->itemRect.x+29, drawItem->itemRect.y+184,126, value_fontColor, ONDKSTF_LEFT);
		//ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pstContext->stArrayOfShahidShow[drawItem->item_index].name,
		//				drawItem->itemRect.x+29, drawItem->itemRect.y+167,181, title_fontColor, ONDKSTF_LEFT);
		ONDK_FONT_Release(hFontHandle);
	}

	return ONDK_RET_OK;
}

ONDK_Result_t NX_SHAHID_COM_DrawBtnSeeMore(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, text_w = 0;

	HUINT16		usFontSize = 0;
	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL;

	HUINT32		push_status;
	ONDK_Size_t stImgSize;
	ONDKImageHandle		hNormalImg, hFocusedImg, hDisabledImg, hPushedImg;

	ONDK_Color_t	title_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL (C_T_Gong02);
	cursorColor = COL(C_G_Gong01);
	ulFontSize = NX_PRISM_FONT_SIZE_22;
	lCursorMg = 0;

	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);
	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);
	ONDK_GWM_COM_Button_GetTitleSizeByObj(objBtn, &usFontSize);
	if (usFontSize < 1)
	{
		ulFontSize = NX_PRISM_FONT_SIZE_24;
	}
	else
	{
		ulFontSize = (HUINT32)usFontSize;
	}

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong02);
			cursorColor = COL(C_G_Gong01);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Shahid_Black);
			cursorColor = COL(C_G_Shahid_03);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Shahid_Black);
			cursorColor = COL(C_G_Shahid_03);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	lText_x_offset = 330;
	lText_y_offset = 13;
	text_w = 130;

	// TEXT - TP Info Title
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	if (hFontHandle)
	{
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, w, title_fontColor, ONDKSTF_LEFT);
		ONDK_FONT_Release(hFontHandle);
	}

	// Draw image in button
	ONDK_GWM_COM_Button_GetImageHandleByObj(objBtn, &hNormalImg, &hFocusedImg, &hDisabledImg, &hPushedImg);
	push_status = ONDK_GWM_Obj_GetObjectStatus((ONDK_GWM_GuiObject_t)objBtn);

	if (( push_status& GUIOBJ_DISABLED) && (hDisabledImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hDisabledImg);
		ONDK_IMG_Blit(hDisabledImg, screen, x+text_w, (y1+(h/2)-(stImgSize.h/2)));
		return ONDK_MESSAGE_BREAK;
	}
	else if ((push_status & GUIOBJ_PUSHED) && (hPushedImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hPushedImg);
		ONDK_IMG_Blit(hPushedImg, screen, x+text_w, (y1+(h/2)-(stImgSize.h/2)));
		return ONDK_MESSAGE_BREAK;
	}
	else if ((push_status & GUIOBJ_HILIGHTED) && (hFocusedImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hFocusedImg);
		ONDK_IMG_Blit(hFocusedImg, screen, x+text_w, (y1+(h/2)-(stImgSize.h/2)));
		return ONDK_MESSAGE_BREAK;
	}
	else if (hNormalImg!= NULL)
	{
		stImgSize = ONDK_IMG_GetSize(hNormalImg);
		ONDK_IMG_Blit(hNormalImg, screen, x+text_w, (y1+(h/2)-(stImgSize.h/2)));
		return ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
#endif
