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
 * @file	  		nx_prism_component_popuplist.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>
#include <nx_prism_app.h>
#include <nx_prism_util.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_component.h>
#include <nx_prism_component_popuplist.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
//#define	NX_PRISM_COMP_LISTDLG_ITEM_MAX							9
//#define	NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX						NX_PRISM_COMP_LISTDLG_ITEM_MAX

#define	NX_PRISM_COMP_POPUPLIST_MENU_FRAME_X					NX_PRISM_OSD_X
#define	NX_PRISM_COMP_POPUPLIST_MENU_FRAME_Y					NX_PRISM_OSD_Y
#define	NX_PRISM_COMP_POPUPLIST_MENU_FRAME_WIDTH				NX_PRISM_SETTINGS_WINDOW_WIDTH
#define	NX_PRISM_COMP_POPUPLIST_MENU_FRAME_HEIGHT				NX_PRISM_SETTINGS_WINDOW_HEIGHT
#define	NX_PRISM_COMP_POPUPLIST_MENU_FRAME_TOP_LINE_H			1

#define	NX_PRISM_COMP_POPUPLIST_CURSOR_MG						5

#define	NX_PRISM_UI_LISTDLG_LIST_TITLE_X						30
#define	NX_PRISM_UI_LISTDLG_LIST_TITLE_Y						(46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_LISTDLG_LIST_TITLE_W						640
#define	NX_PRISM_UI_LISTDLG_LIST_TITLE_H						(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X					18
#define	NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_Y					(36 - NX_PRISM_FONT_SIZE_24)
#define	NX_PRISM_COMP_POPUPLIST_TEXT_WIDTH						370
#define	NX_PRISM_COMP_POPUPLIST_TEXT_HEIGHT						30

#define	NX_PRISM_COMP_POPUPLIST_OPT_POINTER_R_X					281
#define	NX_PRISM_COMP_POPUPLIST_OPT_POINTER_L_X					60

#define	NX_PRISM_COMP_POPUPLIST_OPT_IMG_X						0
#define	NX_PRISM_COMP_POPUPLIST_OPT_IMG_Y						0
#define	NX_PRISM_COMP_POPUPLIST_OPT_IMG_W						55
#define	NX_PRISM_COMP_POPUPLIST_OPT_IMG_H						55

#define	NX_PRISM_COMP_POPUPLIST_OPT_TEXT_OFFSET_X				55
#define	NX_PRISM_COMP_POPUPLIST_OPT_TEXT_OFFSET_Y				(38 - NX_PRISM_FONT_SIZE_24)
#define	NX_PRISM_COMP_POPUPLIST_OPT_TEXT_WIDTH					245
#define	NX_PRISM_COMP_POPUPLIST_OPT_TEXT_HEIGHT					30

#define	NX_PRISM_COMP_POPUPLIST_OPT_IMG_X_MG					8
#define	NX_PRISM_COMP_POPUPLIST_SELECT_LIST_MG					10
#define	NX_PRISM_COMP_POPUPLIST_SELECT_IMG_X_MG					18
#define	NX_PRISM_COMP_POPUPLIST_SELECT_IMG_Y_MG					4

#define	NX_PRISM_COMP_POPUPLIST_SCROLL_X_MG						8
#define	NX_PRISM_COMP_POPUPLIST_SCROLL_W						3
#define	NX_PRISM_COMP_POPUPLIST_SCROLL_H						5

#define	NX_PRISM_COMP_POPUPLIST_SCROLL_BAR_X					1
#define	NX_PRISM_COMP_POPUPLIST_SCROLL_BAR_W					1

#define	NX_PRISM_COMP_POPUPLIST_H_LIST_CURSOR_H					134
#define	NX_PRISM_COMP_POPUPLIST_H_LIST_VSPACE_H					4


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static	Nx_PrismListDialog_Content_t		s_NxPrismListDialog_Content;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_PrismListDialog_Content_t	*nx_comp_listdlg_GetContents(void);
static ONDK_Result_t	nx_comp_listdlg_CheckIsValueInitiated(Nx_PrismListDialog_Content_t *pstContents);

static ONDK_Result_t	nx_comp_listdlg_DrawDialogList_Cursor_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int param1);
static ONDK_Result_t	nx_comp_listdlg_DrawDialogListWithIcon_Cursor_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

static ONDK_Result_t	nx_comp_listdlg_DrawOptionList_Button_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_comp_listdlg_DrawOptionListWithIcon_Button_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int param1);

static ONDK_Result_t	nx_comp_listdlg_DrawMultiSelectVList_Button_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_comp_listdlg_DrawMultiSelectHList_Button_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

static ONDK_Result_t	nx_comp_listdlg_DrawSelectList_Cursor_A2(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

static ONDK_Result_t	nx_comp_listdlg_DrawDefaultUI(Nx_PrismListDialog_Content_t *pstContents);
static ONDK_Result_t	nx_comp_listdlg_MsgGwmCreate(Nx_PrismListDialog_Content_t *pstContents);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_PrismListDialog_Content_t	*nx_comp_listdlg_GetContents(void)
{
	return &s_NxPrismListDialog_Content;
}


static ONDK_Result_t	nx_comp_listdlg_CheckIsValueInitiated(Nx_PrismListDialog_Content_t *pstContents)
{
	Nx_PrismListDialog_Content_t *pLocalContent = NULL;

	pLocalContent = nx_comp_listdlg_GetContents();
	HxSTD_MemCopy(pLocalContent, pstContents, sizeof(Nx_PrismListDialog_Content_t));

	/* check parent application is indicated */
	if(pstContents->parentApp == NULL)
	{
		return ONDK_RET_INITFAIL;
	}

	/* check whether contents is */
	if(((HUINT8 **)pstContents->ulList == NULL) || (pstContents->ulNumOfList == 0)
		|| (pstContents->ulCurIdxOfList < 0) || (pstContents->ulCurIdxOfList >= pstContents->ulNumOfList))
	{
		return ONDK_RET_INITFAIL;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_comp_listdlg_DrawDialogList_Cursor_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lTextW = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lText_y_off = 0;
	HUINT32		ulItem_addr = 0;
	HUINT8		*szTitle = NULL, *pDisable = NULL;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDKFontHandle		hFontHandle;
	LIST_OBJECT	obj=(LIST_OBJECT)object;

	/* make coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* set properties */
	ulItem_addr = ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if ((ulItem_addr == 0) || (lTotal == 0))
	{
		NX_APP_Error("ERROR!!! (ulItem_addr == 0) || (lTotal == 0) !!\n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("ERROR!!! ONDK_GWM_List_GetFontHandleObj() was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));

	lText_y_off = NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_Y;

	if (lNumOfPage < lTotal)
	{
		lLastIdx = lNumOfPage;
	}
	else
	{
		lLastIdx = lTotal;
	}

	for (i = 0; i < lLastIdx; i++)
	{
		if (i + lStart == lCurrent)	/* focus */
		{
			title_fontColor = COL(C_T_Bang_100);
			x = x1;
			y = y1 + lCursor_y_off;

			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_COMP_POPUPLIST_CURSOR_MG, COL(C_G_Base01_100));
		}
		else
		{
			title_fontColor = COL(C_T_Whistle_100);
			if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
			{
				title_fontColor = COL(C_T_Gong08_40);
			}
		}

		szTitle = (HUINT8 *)(((HUINT8 **)ulItem_addr)[lStart+i]);
		if (szTitle == NULL)
		{
			NX_APP_Error("ERROR!!! szTitle was NULL!!\n");
			return ONDK_RET_FAIL;
		}

		NX_APP_Info("[%d][%s]\n", lStart+i, szTitle);

		x = x1 + NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off + lText_y_off;
		lTextW = (w - (NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X * 2));
		if (lTextW < 0)
		{
			lTextW = NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X;
		}

		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, lTextW, title_fontColor, ONDKSTF_LEFT);

		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_comp_listdlg_DrawDialogListWithIcon_Cursor_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0, x = 0, y = 0, w = 0, h = 0, lTextW = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lImage_x_off = 0, lImage_y_off = 0, lNumOfPage = NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lText_y_off = 0;
	HUINT32		ulItem_addr = 0;
	HUINT8		*szTitle = NULL, *pDisable = NULL, *szImage = NULL;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDK_Rect_t			stImgRect;
	ONDKFontHandle		hFontHandle;
	ONDK_Size_t			stImgSize = {0, 0};
	ONDKImageHandle		hImgHandle = NULL;
	LIST_OBJECT	obj=(LIST_OBJECT)object;
	Nx_PrismListDialog_Content_t	*pstContent = NX_PRISM_COMP_ListDlg_GetContents();

	/* make coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* set properties */
	ulItem_addr = ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if ((ulItem_addr == 0) || (lTotal == 0))
	{
		NX_APP_Error("ERROR!!! ((ulItem_addr == 0) || (lTotal == 0)) was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("ERROR!!! ONDK_GWM_List_GetFontHandleObj() was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));

	lText_y_off = NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_Y;

	if (lNumOfPage < lTotal)
	{
		lLastIdx = lNumOfPage;
	}
	else
	{
		lLastIdx = lTotal;
	}

	for (i = 0; i < lLastIdx; i++)
	{
		if (i + lStart == lCurrent)	/* focus */
		{
			title_fontColor = COL(C_T_Bang_100);
			x = x1;
			y = y1 + lCursor_y_off;

			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_COMP_POPUPLIST_CURSOR_MG, COL(C_G_Base01_100));
		}
		else
		{
			title_fontColor = COL(C_T_Whistle_100);
			if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
			{
				title_fontColor = COL(C_T_Gong08_40);
			}
		}

		szTitle = (HUINT8 *)(((HUINT8 **)ulItem_addr)[lStart+i]);
		if (szTitle == NULL)
		{
			NX_APP_Error("ERROR!!! szTitle was NULL!!\n");
			return ONDK_RET_FAIL;
		}

		NX_APP_Info("[%d][%s]\n", lStart+i, szTitle);

		x = x1 + NX_PRISM_COMP_POPUPLIST_OPT_IMG_X_MG;
		y = y1 + lCursor_y_off;

		if (pstContent->ulImageList)
		{
			szImage = (HUINT8 *)(((HUINT8 **)pstContent->ulImageList)[lStart+i]);
			if ((szImage != NULL) && (HxSTD_StrCmp(RES_NONE_PNG, (HCHAR*)szImage) != 0))
			{
				hImgHandle = ONDK_IMG_CreateSelf(szImage);
				stImgSize = ONDK_IMG_GetSize(hImgHandle);
				lImage_x_off = stImgSize.w;
				lImage_y_off = (lCellHeight - stImgSize.h)/2;
				stImgRect = ONDK_Rect(x, y + lImage_y_off, stImgSize.w, stImgSize.h);
				ONDK_GUICOM_IMG_Blit(screen, stImgRect, (HCHAR*)szImage);
			}
			szImage = NULL;
		}

		x = x1 + NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X + lImage_x_off;
		y = y1 + lCursor_y_off + lText_y_off;

		lTextW = (w - (NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X*2) - lImage_x_off);
		if (lTextW < 0)
		{
			lTextW = NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X;
		}

		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, lTextW, title_fontColor, ONDKSTF_LEFT);

		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_comp_listdlg_DrawOptionList_Button_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lTextW = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lText_y_off = 0;
	HUINT32 	ulItem_addr = 0;
	HUINT8		*szTitle = NULL, *pDisable = NULL;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDKFontHandle		hFontHandle;
	LIST_OBJECT 		obj = (LIST_OBJECT)object;


	/* make coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* set properties */
	ulItem_addr = ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage [%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if ((ulItem_addr == 0) || (lTotal == 0))
	{
		NX_APP_Error("ERROR!!! ((ulItem_addr == 0) || (lTotal == 0)) was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("ERROR!!! ONDK_GWM_List_GetFontHandleObj() was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));

	lText_y_off = NX_PRISM_COMP_POPUPLIST_OPT_TEXT_OFFSET_Y;

	if (lNumOfPage < lTotal)
	{
		lLastIdx = lNumOfPage;
	}
	else
	{
		lLastIdx = lTotal;
	}

	for (i = 0; i < lLastIdx; i++)
	{
		if (i + lStart == lCurrent) /* focus */
		{
			title_fontColor = COL(C_T_Bang_100);
			x = x1;
			y = y1 + lCursor_y_off;

			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_UI_OPT_POPUP_CELL_MG, COL(C_G_Base01_100));
		}
		else
		{
			title_fontColor = COL(C_T_Gong02);
			if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
			{
				title_fontColor = COL(C_T_Gong08_40);
			}
			x = x1;
			y = y1 + lCursor_y_off;
			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_UI_OPT_POPUP_CELL_MG, COL(C_G_Gong01));
		}

		szTitle = (HUINT8 *)(((HUINT8 **)ulItem_addr)[lStart+i]);
		if (szTitle == NULL)
		{
			NX_APP_Error("ERROR!!! szTitle was NULL!!\n");
			return ONDK_RET_FAIL;
		}

		NX_APP_Info("[%d][%s]\n", (lStart + i), szTitle);

		x = x1 + NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off + lText_y_off;
		lTextW = (w - (NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X * 2));
		if (lTextW < 0)
		{
			lTextW = NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X;
		}

		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, lTextW, title_fontColor, ONDKSTF_LEFT);

		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_comp_listdlg_DrawOptionListWithIcon_Button_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lTextW = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lText_y_off = 0;
	HUINT32 	ulItem_addr = 0;
	HUINT8		*szTitle = NULL, *pDisable = NULL, *szImage = NULL;
	ONDK_Rect_t			stImgRect;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDKFontHandle		hFontHandle;
	LIST_OBJECT 		obj = (LIST_OBJECT)object;
	Nx_PrismListDialog_Content_t	*pstContent = NX_PRISM_COMP_ListDlg_GetContents();

	/* make coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	lTextW = NX_PRISM_COMP_POPUPLIST_OPT_TEXT_WIDTH;

	/* set properties */
	ulItem_addr = ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage [%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if ((ulItem_addr == 0) || (lTotal == 0))
	{
		NX_APP_Error("ERROR!!! ((ulItem_addr == 0) || (lTotal == 0)) was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("ERROR!!! ONDK_GWM_List_GetFontHandleObj() was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));

	lText_y_off = NX_PRISM_COMP_POPUPLIST_OPT_TEXT_OFFSET_Y;

	if (lNumOfPage < lTotal)
	{
		lLastIdx = lNumOfPage;
	}
	else
	{
		lLastIdx = lTotal;
	}

	for (i = 0; i < lLastIdx; i++)
	{
		if (i + lStart == lCurrent) /* focus */
		{
			title_fontColor = COL(C_T_Bang_100);
			x = x1;
			y = y1 + lCursor_y_off;

			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_UI_OPT_POPUP_CELL_MG, COL(C_G_Base01_100));

			if (pstContent->ulImageExtList)
			{
				szImage = (HUINT8 *)(((HUINT8 **)pstContent->ulImageExtList)[lStart+i]);
				if ((szImage != NULL) && (HxSTD_StrCmp(RES_NONE_PNG, (HCHAR*)szImage) != 0))
				{
					stImgRect = ONDK_Rect(x, y, NX_PRISM_COMP_POPUPLIST_OPT_IMG_W, NX_PRISM_COMP_POPUPLIST_OPT_IMG_H);
					ONDK_GUICOM_IMG_Blit(screen, stImgRect, (HCHAR*)szImage);
				}
				szImage = NULL;
			}
		}
		else
		{
			title_fontColor = COL(C_T_Gong02);
			if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
			{
				title_fontColor = COL(C_T_Gong08_40);
			}
			x = x1;
			y = y1 + lCursor_y_off;
			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_UI_OPT_POPUP_CELL_MG, COL(C_G_Gong01));

			if (pstContent->ulImageList)
			{
				szImage = (HUINT8 *)(((HUINT8 **)pstContent->ulImageList)[lStart+i]);
				if ((szImage != NULL) && (HxSTD_StrCmp(RES_NONE_PNG, (HCHAR*)szImage) != 0))
				{
					stImgRect = ONDK_Rect(x, y, NX_PRISM_COMP_POPUPLIST_OPT_IMG_W, NX_PRISM_COMP_POPUPLIST_OPT_IMG_H);
					ONDK_GUICOM_IMG_Blit(screen, stImgRect, (HCHAR*)szImage);
				}
				szImage = NULL;
			}
		}

		szTitle = (HUINT8 *)(((HUINT8 **)ulItem_addr)[lStart+i]);
		if (szTitle == NULL)
		{
			NX_APP_Error("ERROR!!! szTitle was NULL!!\n");
			return ONDK_RET_FAIL;
		}

		NX_APP_Info("[%d][%s]\n", (lStart + i), szTitle);

		x = x1 + NX_PRISM_COMP_POPUPLIST_OPT_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off + lText_y_off;
		if (lTextW < 0)
		{
			lTextW = NX_PRISM_COMP_POPUPLIST_OPT_TEXT_OFFSET_X;
		}

		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, lTextW, title_fontColor, ONDKSTF_LEFT);

		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_comp_listdlg_DrawMultiSelectVList_Button_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lTextW = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lText_y_off = 0;
	HUINT32 	ulItem_addr = 0;
	HUINT8		*szTitle = NULL, *pDisable = NULL;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDKFontHandle		hFontHandle;
	LIST_OBJECT 		obj = (LIST_OBJECT)object;


	/* make coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* set properties */
	ulItem_addr = ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage [%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if ((ulItem_addr == 0) || (lTotal == 0))
	{
		NX_APP_Error("ERROR!!! ((ulItem_addr == 0) || (lTotal == 0)) was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("ERROR!!! ONDK_GWM_List_GetFontHandleObj() was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));

	lText_y_off = NX_PRISM_COMP_POPUPLIST_OPT_TEXT_OFFSET_Y;

	if (lNumOfPage < lTotal)
	{
		lLastIdx = lNumOfPage;
	}
	else
	{
		lLastIdx = lTotal;
	}

	for (i = 0; i < lLastIdx; i++)
	{
		if (i + lStart == lCurrent) /* focus */
		{
			title_fontColor = COL(C_T_Bang_100);
			x = x1;
			y = y1 + lCursor_y_off;

			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_UI_OPT_POPUP_CELL_MG, COL(C_G_Base01_100));
		}
		else
		{
			title_fontColor = COL(C_T_Gong02);
			if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
			{
				title_fontColor = COL(C_T_Gong08_40);
			}
			x = x1;
			y = y1 + lCursor_y_off;
			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_UI_OPT_POPUP_CELL_MG, COL(C_G_Gong01));
		}

		szTitle = (HUINT8 *)(((HUINT8 **)ulItem_addr)[lStart+i]);
		if (szTitle == NULL)
		{
			NX_APP_Error("ERROR!!! szTitle was NULL!!\n");
			return ONDK_RET_FAIL;
		}

		NX_APP_Info("[%d][%s]\n", (lStart + i), szTitle);

		x = x1 + NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off + lText_y_off;
		lTextW = (w - (NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X * 2));
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, lTextW, title_fontColor, ONDKSTF_LEFT);

		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_comp_listdlg_DrawMultiSelectHList_Button_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0, x = 0, y = 0, w = 0, h = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lHItemTab = 0, lText_x_off = 0;
	HINT32		lCursor_y_off = 0, lText_y_off = 0, lCursor_x_off = 0, lText_h_off = 0, lCellWidth = 0, lCursor_h = 0;
	HUINT32 	ulItem_addr = 0;
	HUINT8		*szTitle = NULL, *pDisable = NULL, *szImage = NULL;
	ONDK_Rect_t			stImgRect;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDK_Size_t			stImgSize = {0, 0};
	ONDKImageHandle		hImgHandle = NULL;
	ONDKFontHandle		hFontHandle;
	LIST_OBJECT 		obj = (LIST_OBJECT)object;
	Nx_PrismListDialog_Content_t	*pstContent = NX_PRISM_COMP_ListDlg_GetContents();

	/* make coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* set properties */
	ulItem_addr = ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage [%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if ((ulItem_addr == 0) || (lTotal == 0))
	{
		NX_APP_Error("ERROR!!! ((ulItem_addr == 0) || (lTotal == 0)) was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("ERROR!!! ONDK_GWM_List_GetFontHandleObj was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	lCellWidth = ONDK_GWM_List_GetCellWidth(ONDK_GWM_Obj_GetObjectID(object));

	lText_y_off = NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_Y;
	lText_x_off = NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X;
	lText_h_off = (NX_PRISM_COMP_POPUPLIST_TEXT_HEIGHT * 2);
	lHItemTab = (NX_PRISM_UI_DLG_MULTISEL_H_LIST_ITEM_TAB/2);

	if (lNumOfPage < lTotal)
	{
		lLastIdx = lNumOfPage;
	}
	else
	{
		lLastIdx = lTotal;
	}

	for (i = 0; i < lLastIdx; i++)
	{
		if (i + lStart == lCurrent) /* focus */
		{
			title_fontColor = COL(C_T_Base01);
			x = x1 + lCursor_x_off;
			y = y1 + lCursor_y_off;
			lCursor_h = (lCellHeight > lText_h_off) ? (lCellHeight - lText_h_off - NX_PRISM_COMP_POPUPLIST_H_LIST_VSPACE_H) : lCellHeight;

			ONDK_DRAW_FillRectElement(screen, x, y, (lCellWidth - lHItemTab), lCursor_h, COL(C_G_Base01_100));

			if (pstContent->ulImageExtList)
			{
				szImage = (HUINT8 *)(((HUINT8 **)pstContent->ulImageExtList)[lStart+i]);
				if ((szImage != NULL) && (HxSTD_StrCmp(RES_NONE_PNG, (HCHAR*)szImage) != 0))
				{
					hImgHandle = ONDK_IMG_CreateSelf(szImage);
					stImgSize = ONDK_IMG_GetSize(hImgHandle);
					stImgRect = ONDK_Rect(x, y, stImgSize.w, stImgSize.h);
					ONDK_GUICOM_IMG_Blit(screen, stImgRect, (HCHAR*)szImage);
				}
				szImage = NULL;
			}
		}
		else
		{
			title_fontColor = COL(C_T_Gong02);
			if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
			{
				title_fontColor = COL(C_T_Gong08_40);
			}
			x = x1 + lCursor_x_off;
			y = y1 + lCursor_y_off;
			lCursor_h = (lCellHeight > lText_h_off) ? (lCellHeight - lText_h_off- NX_PRISM_COMP_POPUPLIST_H_LIST_VSPACE_H) : lCellHeight;

			ONDK_DRAW_FillRectElement(screen, x, y, (lCellWidth - lHItemTab), lCursor_h, COL(C_G_Gong01));

			if (pstContent->ulImageList)
			{
				szImage = (HUINT8 *)(((HUINT8 **)pstContent->ulImageList)[lStart+i]);
				if ((szImage != NULL) && (HxSTD_StrCmp(RES_NONE_PNG, (HCHAR*)szImage) != 0))
				{
					hImgHandle = ONDK_IMG_CreateSelf(szImage);
					stImgSize = ONDK_IMG_GetSize(hImgHandle);
					stImgRect = ONDK_Rect(x, y, stImgSize.w, stImgSize.h);
					ONDK_GUICOM_IMG_Blit(screen, stImgRect, (HCHAR*)szImage);
				}
				szImage = NULL;
			}
		}

		szTitle = (HUINT8 *)(((HUINT8 **)ulItem_addr)[lStart+i]);
		if (szTitle == NULL)
		{
			NX_APP_Error("ERROR!!! szTitle was NULL!!\n");
			return ONDK_RET_FAIL;
		}

		NX_APP_Info("[%d][%s]\n", (lStart + i), szTitle);

		x = x1 + lCursor_x_off;
		y = y1 + lCellHeight - lText_h_off;

		ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x, y, (lCellWidth - lHItemTab), lText_h_off,
						(HUINT8*)szTitle, NX_PRISM_COMP_POPUPLIST_H_LIST_VSPACE_H, title_fontColor, ONDKSTF_CENTER|ONDKSTF_WORDWRAP);

		lCursor_x_off += lCellWidth;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_comp_listdlg_DrawSelectList_Cursor_A2(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
										ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HBOOL		bNeedScrollBar = FALSE;
	HINT32		i = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0, x = 0, y = 0, w = 0, h = 0, lItem = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lText_y_off = 0;
	HINT32		lImage_x_off = 0, lImage_y_off = 0, lStrWidth = 0, lScroll_w_off = 0, lScroll_height = 0;
	HUINT32		ulItem_addr = 0;
	HUINT8		*szTitle = NULL, *pDisable = NULL, *szImage = NULL;
	ONDK_Rect_t			stImgRect;
	ONDK_Size_t 		stImgSize = {0, 0};
	ONDKImageHandle 	hImgHandle = NULL;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDKFontHandle		hFontHandle;
	LIST_OBJECT	obj=(LIST_OBJECT)object;

	/* make coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* set properties */
	ulItem_addr = ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if ((ulItem_addr == 0) || (lTotal == 0))
	{
		NX_APP_Error("ERROR!!! (ulItem_addr == 0) || (lTotal == 0) !!\n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("ERROR!!! ONDK_GWM_List_GetFontHandleObj() was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	lText_y_off = NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_Y;

	if (lNumOfPage < lTotal)
	{
		lLastIdx = lNumOfPage;
		bNeedScrollBar = TRUE;
	}
	else
	{
		lLastIdx = lTotal;
		bNeedScrollBar = FALSE;
	}

	lScroll_w_off = 0;
	if (bNeedScrollBar == TRUE)
	{
		lScroll_w_off = (NX_PRISM_COMP_POPUPLIST_SCROLL_X_MG + NX_PRISM_COMP_POPUPLIST_SCROLL_W);
		lScroll_height = h;
	}
	w = w - lScroll_w_off;

	for (i = 0; i < lLastIdx; i++)
	{
		lItem = (lStart + i);
		if (lItem == lCurrent)	/* focus */
		{
			title_fontColor = COL(C_T_Bang_100);
			x = x1;
			y = y1 + lCursor_y_off;

			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_COMP_POPUPLIST_CURSOR_MG, COL(C_G_Base01_100));
		}
		else
		{
			title_fontColor = COL(C_T_Whistle_100);
			if ((pDisable != NULL) && (pDisable[lItem] == 1))
			{
				title_fontColor = COL(C_T_Gong08_40);
			}
		}

		szTitle = (HUINT8 *)(((HUINT8 **)ulItem_addr)[lItem]);
		if (szTitle == NULL)
		{
			NX_APP_Error("ERROR!!! szTitle was NULL!!\n");
			return ONDK_RET_FAIL;
		}

		NX_APP_Info("[%d][%s]\n", lItem, szTitle);

		x = x1 + NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off + lText_y_off;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y,
					(w - NX_PRISM_COMP_POPUPLIST_TEXT_OFFSET_X - lImage_x_off), title_fontColor, ONDKSTF_LEFT);

		lStrWidth = ONDK_FONT_GetStringWidth(hFontHandle, (HCHAR*)szTitle);
		if (lItem == lSelect)	/* Select Item */
		{
			szImage = (lItem == lCurrent) ? (HUINT8*)RES_112_00_CHECK04_C_PNG : (HUINT8*)RES_112_00_CHECK04_H_PNG ;
			x = x1 + NX_PRISM_COMP_POPUPLIST_OPT_IMG_X_MG + lStrWidth + NX_PRISM_COMP_POPUPLIST_SELECT_IMG_X_MG;
			y = y1 + lCursor_y_off;
			if (szImage != NULL)
			{
				hImgHandle = ONDK_IMG_CreateSelf(szImage);
				stImgSize = ONDK_IMG_GetSize(hImgHandle);
				lImage_x_off = stImgSize.w;
				lImage_y_off = (lCellHeight - stImgSize.h)/2;
				lImage_y_off = (lImage_y_off < 0) ? 0 : lImage_y_off ;
				stImgRect = ONDK_Rect(x, y + lImage_y_off - NX_PRISM_COMP_POPUPLIST_SELECT_IMG_Y_MG, stImgSize.w, stImgSize.h);
				ONDK_GUICOM_IMG_Blit(screen, stImgRect, (HCHAR*)szImage);
			}
			szImage = NULL;
		}
		else
		{
			lImage_x_off = 0;
		}

		lCursor_y_off += lCellHeight;
	}

	/* Scroll */
	if (bNeedScrollBar == TRUE)
	{
		HINT32	lScrollBall_step = 0, lHeight = 0;
		HINT32	lAllPage = 0, lScrollBall_height = 0, lScroll_y_offset = 0;

		lAllPage = (lTotal / lNumOfPage);
		lAllPage += (lTotal % lNumOfPage) ? 1 : 0;

		lScrollBall_height = lScroll_height / lAllPage;
		if (lScrollBall_height < NX_PRISM_COMP_POPUPLIST_SCROLL_H)
		{
			lScrollBall_height = NX_PRISM_COMP_POPUPLIST_SCROLL_H;
		}

		lHeight = (lScroll_height - lScrollBall_height);
		/* calculate vertical position */
		for (i = 1; lTotal > lHeight; i++)
		{
			lTotal /= i;
		} //while (total > (460 - lScrollBall_height));

		lScrollBall_step = lHeight / (lTotal - 1);
		lScroll_y_offset = (lCurrent / i) * lScrollBall_step;
		lScrollBall_height += (lHeight - ((lTotal - 1) * lScrollBall_step));

		x = x1 + w + NX_PRISM_COMP_POPUPLIST_SCROLL_X_MG;
		y = y1;

		/* Draw Scroll Bar */
		ONDK_DRAW_FillRectElement(screen, x + NX_PRISM_COMP_POPUPLIST_SCROLL_BAR_X, y, NX_PRISM_COMP_POPUPLIST_SCROLL_BAR_W, lScroll_height, COL(C_G_Whistle_20));

		/* Draw Scroll Bar Ball */
		stImgRect = ONDK_Rect(x, y + lScroll_y_offset, NX_PRISM_COMP_POPUPLIST_SCROLL_W, lScrollBall_height);
		ONDK_GUICOM_VBox(screen, stImgRect, RES_111_00_SCROLL_BAR_1U_PNG, RES_111_00_SCROLL_BAR_2M_PNG, RES_111_00_SCROLL_BAR_3D_PNG);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t		nx_comp_listdlg_DrawDefaultUI(Nx_PrismListDialog_Content_t *pstContents)
{
	HINT32			lFontSize = 0, y = 0, lLineCnt = 0;
	ONDK_Rect_t 	stBGRect, stListRect, stRect, stLineRect;
	GWM_ObjDraw_t	frameDrawMethod = NULL;
	GWM_ObjDraw_t	listDrawMethod = NULL;


	HxSTD_memset(&stBGRect, 0x00, sizeof(ONDK_Rect_t));
	HxSTD_memset(&stListRect, 0x00, sizeof(ONDK_Rect_t));

	/* Set Frame Rect */
	if (pstContents->eAttribute & eListDlg_BG_MenuSubItemApp)
	{
		stListRect = ONDK_Rect(pstContents->appArea.x, pstContents->appArea.y, pstContents->appArea.w, pstContents->appArea.h);
	}
	else if (pstContents->eAttribute & eListDlg_OSD_FULL)
	{
		stListRect = ONDK_Rect(pstContents->appArea.x, pstContents->appArea.y, pstContents->appArea.w, pstContents->appArea.h);
	}
	else if (pstContents->eAttribute & eListDlg_OSD_USER)
	{
		stListRect = ONDK_Rect((pstContents->appArea.x - pstContents->extArea.x), (pstContents->appArea.y - pstContents->extArea.y),
								pstContents->appArea.w, pstContents->appArea.h);
	}
	else
	{
		stListRect = ONDK_Rect(0, 0, pstContents->appArea.w, pstContents->appArea.h);
	}

	/* Set Draw Frame BG Function */
	if (pstContents->eAttribute & eListDlg_BG_MenuSubItemApp)
	{
		frameDrawMethod = NX_PRISM_COMP_DrawWindowPopUp;
	}
	else if ((pstContents->eAttribute & eListDlg_OSD_BG) && (pstContents->eAttribute & eListDlg_IMAGE_OptionLApp))
	{
		frameDrawMethod = NX_PRISM_COMP_DrawWindowOptionLeft;
	}
	else if ((pstContents->eAttribute & eListDlg_OSD_BG) && (pstContents->eAttribute & eListDlg_IMAGE_OptionRApp))
	{
		frameDrawMethod = NX_PRISM_COMP_DrawWindowOptionRight;
	}
	else if (pstContents->eAttribute & eListDlg_OSD_BG)
	{
		frameDrawMethod = NX_PRISM_COMP_DrawWindowPopUp;
	}
	else if ((pstContents->eAttribute & eListDlg_MultiSelectVApp) || (pstContents->eAttribute & eListDlg_MultiSelectHApp))
	{
		frameDrawMethod = NX_PRISM_COMP_DrawFrameDialogueWindowBG;
	}
	else
	{
		frameDrawMethod = NULL;
	}

	/* Set List Draw Item Function */
	if ((pstContents->eAttribute & eListDlg_BG_MenuSubItemApp) || (pstContents->eAttribute & eListDlg_SubItemApp))
	{
		if (pstContents->eAttribute & eListDlg_ICON_L)
		{
			listDrawMethod = (GWM_ObjDraw_t)nx_comp_listdlg_DrawDialogListWithIcon_Cursor_A1;
		}
		else
		{
			listDrawMethod = (GWM_ObjDraw_t)nx_comp_listdlg_DrawDialogList_Cursor_A1;
		}
	}
	else if ((pstContents->eAttribute & eListDlg_IMAGE_OptionLApp) || (pstContents->eAttribute & eListDlg_IMAGE_OptionRApp))
	{
		listDrawMethod = (GWM_ObjDraw_t)nx_comp_listdlg_DrawOptionListWithIcon_Button_A1;
	}
	else if (pstContents->eAttribute & eListDlg_MultiSelectVApp)
	{
		listDrawMethod = (GWM_ObjDraw_t)nx_comp_listdlg_DrawMultiSelectVList_Button_A1;
	}
	else if (pstContents->eAttribute & eListDlg_MultiSelectHApp)
	{
		listDrawMethod = (GWM_ObjDraw_t)nx_comp_listdlg_DrawMultiSelectHList_Button_A1;
	}
	else if (pstContents->eAttribute & eListDlg_SelectItemApp)
	{
		listDrawMethod = (GWM_ObjDraw_t)nx_comp_listdlg_DrawSelectList_Cursor_A2;
	}
	else
	{
		listDrawMethod = NULL;
	}

	/* Set Frame BackGround Rect */
	if (pstContents->eAttribute & eListDlg_BG_MenuSubItemApp)
	{
		stBGRect = ONDK_Rect(NX_PRISM_COMP_POPUPLIST_MENU_FRAME_X, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_Y, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_WIDTH, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_HEIGHT);
	}
	else if ((pstContents->eAttribute & eListDlg_OSD_BG) && (pstContents->eAttribute & eListDlg_OSD_FULL))
	{
		stBGRect = ONDK_Rect(NX_PRISM_COMP_POPUPLIST_MENU_FRAME_X, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_Y, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_WIDTH, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_HEIGHT);
	}
	else if ((pstContents->eAttribute & eListDlg_OSD_BG) && (pstContents->eAttribute & eListDlg_OSD_USER))
	{
		stBGRect = ONDK_Rect(0, 0, pstContents->extArea.w, pstContents->extArea.h);
	}
	else if (pstContents->eAttribute & eListDlg_OSD_BG)
	{
		stBGRect = ONDK_Rect(stListRect.x, stListRect.y, stListRect.w, stListRect.h);
	}
	else
	{
		// Not to do
	}

	/* Draw Frame Background */
	if ((pstContents->eAttribute & eListDlg_OSD_BG) ||
		(pstContents->eAttribute & eListDlg_BG_MenuSubItemApp) ||
		(pstContents->eAttribute & eListDlg_SelectItemApp))
	{
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_LISTDLG_FRAME_ID, &stBGRect, 0);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_LISTDLG_FRAME_ID, frameDrawMethod);
	}

	/* Draw Top Line */
	if ((pstContents->eAttribute & eListDlg_MultiSelectVApp)
		|| (pstContents->eAttribute & eListDlg_MultiSelectHApp)
		|| (pstContents->eAttribute & eListDlg_SelectItemApp))
	{
		stLineRect = ONDK_Rect(stBGRect.x, stBGRect.y, stBGRect.w, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_TOP_LINE_H);
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_LISTDLG_LIST_LINE_TOP_ID, &stLineRect, COL(C_G_Whistle_100));
	}

	/* Draw Bottom Line */
	if (pstContents->eAttribute & eListDlg_MultiSelectHApp)
	{
		stLineRect = ONDK_Rect(stBGRect.x, stBGRect.y + stBGRect.h - NX_PRISM_COMP_POPUPLIST_MENU_FRAME_TOP_LINE_H,
								stBGRect.w, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_TOP_LINE_H);
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_LISTDLG_LIST_LINE_BOTTOM_ID, &stLineRect, COL(C_G_Whistle_100));
	}

	if ((pstContents->eAttribute & eListDlg_IMAGE_OptionLApp) || (pstContents->eAttribute & eListDlg_IMAGE_OptionRApp))
	{
		if (pstContents->eAttribute & eListDlg_OSD_USER)
		{
			y = (pstContents->appArea.y + pstContents->appArea.h) - NX_PRISM_UI_OPT_POPUP_POINTER_H;
		}
		else
		{
			y = pstContents->appArea.h - NX_PRISM_UI_OPT_POPUP_POINTER_H;
		}
	}

	/* Draw Title */
	if (pstContents->eAttribute & eListDlg_Title)
	{
		lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pstContents->pszTitle, stListRect.w);
		if (pstContents->eAttribute & eListDlg_MultiSelectHApp)
		{
			stRect = ONDK_Rect((stBGRect.x + NX_PRISM_UI_LISTDLG_LIST_TITLE_X), NX_PRISM_UI_LISTDLG_LIST_TITLE_Y,
								(stBGRect.w - (NX_PRISM_UI_LISTDLG_LIST_TITLE_X*2)), (NX_PRISM_UI_DLG_STATUE_MSG_TEXT_LS * lLineCnt) + NX_PRISM_FONT_VSPACE);
		}
		else
		{
			stRect = ONDK_Rect(NX_PRISM_UI_LISTDLG_LIST_TITLE_X, NX_PRISM_UI_LISTDLG_LIST_TITLE_Y,
								stListRect.w, (NX_PRISM_UI_DLG_STATUE_MSG_TEXT_LS * lLineCnt) + NX_PRISM_FONT_VSPACE);
		}
		ONDK_GWM_COM_Text_Create(NX_PRISM_UI_LISTDLG_LIST_TITLE_ID, &stRect, (HCHAR*)pstContents->pszTitle);
		ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_LISTDLG_LIST_TITLE_ID, eFont_SystemNormal);
		ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_LISTDLG_LIST_TITLE_ID, NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_LISTDLG_LIST_TITLE_ID, (TEXT_ALIGN_CENTER|TEXT_MULTILINED));
		ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_LISTDLG_LIST_TITLE_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
	}

	/* Draw Option UI Pointer */
	if (pstContents->eAttribute & eListDlg_IMAGE_OptionLApp)
	{
		stRect = ONDK_Rect(NX_PRISM_COMP_POPUPLIST_OPT_POINTER_L_X, y, NX_PRISM_UI_OPT_POPUP_POINTER_W, NX_PRISM_UI_OPT_POPUP_POINTER_H);
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_LISTDLG_LIST_POINTER_ID, &stRect, 0);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_LISTDLG_LIST_POINTER_ID, NX_PRISM_COMP_DrawImageOptionLeftPointer);
	}
	else if (pstContents->eAttribute & eListDlg_IMAGE_OptionRApp)
	{
		stRect = ONDK_Rect(NX_PRISM_COMP_POPUPLIST_OPT_POINTER_R_X, y, NX_PRISM_UI_OPT_POPUP_POINTER_W, NX_PRISM_UI_OPT_POPUP_POINTER_H);
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_LISTDLG_LIST_POINTER_ID, &stRect, 0);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_LISTDLG_LIST_POINTER_ID, NX_PRISM_COMP_DrawImageOptionRightPointer);
	}
	else
	{
		// Not to do
	}

	/* Draw List BG */
	if (pstContents->eAttribute & eListDlg_SelectItemApp)
	{
		stRect = ONDK_Rect(stListRect.x - NX_PRISM_COMP_POPUPLIST_SELECT_LIST_MG, stListRect.y - NX_PRISM_COMP_POPUPLIST_SELECT_LIST_MG,
							stListRect.w + (NX_PRISM_COMP_POPUPLIST_SELECT_LIST_MG *2),
							stListRect.h + (NX_PRISM_COMP_POPUPLIST_SELECT_LIST_MG *2));
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_LISTDLG_LIST_BOX_ID, &stRect, 0);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_LISTDLG_LIST_BOX_ID, NX_PRISM_COMP_DrawFrameBox_A);

		/* Draw List Scroll BG */
		//stRect = ONDK_Rect(stListRect.x + stListRect.w - NX_PRISM_COMP_POPUPLIST_SCROLL_W + NX_PRISM_COMP_POPUPLIST_SCROLL_BAR_X,
		//					stListRect.y, NX_PRISM_COMP_POPUPLIST_SCROLL_BAR_W,	stListRect.h);
		//ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_LISTDLG_LIST_SCROLL_BG_ID, &stRect, 0);
		//ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_LISTDLG_LIST_SCROLL_BG_ID, NX_PRISM_COMP_DrawFrameScroll_BG);
	}

	lFontSize = (pstContents->lFontSize == 0) ? NX_PRISM_FONT_SIZE_24 : pstContents->lFontSize ;
	/* Create List */
	ONDK_GWM_List_Create(NX_PRISM_UI_LISTDLG_LIST_ID, &stListRect, "NxDialogList", pstContents->lLineSpace,
							pstContents->ulNumOfList, pstContents->ulNumOfPerPage, (HUINT32)(pstContents->ulList));
	ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_LISTDLG_LIST_ID, pstContents->ulCurIdxOfList);
	ONDK_GWM_List_SetSelectedIndex(NX_PRISM_UI_LISTDLG_LIST_ID, pstContents->ulCurIdxOfList);
	ONDK_GWM_List_SetFontHandle(NX_PRISM_UI_LISTDLG_LIST_ID, eFont_SystemBold, lFontSize);
	if (pstContents->eAttribute & eListDlg_MultiSelectHApp)
	{
		ONDK_GWM_List_SetCellWidth(NX_PRISM_UI_LISTDLG_LIST_ID, pstContents->lLineWSpace);
	}

	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_LISTDLG_LIST_ID, listDrawMethod);

	if (pstContents->pDisbleList != NULL)
	{
		ONDK_GWM_List_SetItemDisable(NX_PRISM_UI_LISTDLG_LIST_ID, (char*)pstContents->pDisbleList);
	}

	ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_LISTDLG_LIST_ID);

	return	ONDK_RET_OK;
}


static ONDK_Result_t		nx_comp_listdlg_MsgGwmCreate(Nx_PrismListDialog_Content_t *pstContents)
{
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	if (pstContents->eAttribute & eListDlg_BG_MenuSubItemApp)
	{
		stRect = ONDK_Rect(NX_PRISM_COMP_POPUPLIST_MENU_FRAME_X, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_Y, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_WIDTH, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_HEIGHT);
	}
	else if (pstContents->eAttribute & eListDlg_OSD_FULL)
	{
		stRect = ONDK_Rect(NX_PRISM_COMP_POPUPLIST_MENU_FRAME_X, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_Y, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_WIDTH, NX_PRISM_COMP_POPUPLIST_MENU_FRAME_HEIGHT);
	}
	else if (pstContents->eAttribute & eListDlg_OSD_USER)
	{
		stRect = ONDK_Rect(pstContents->extArea.x, pstContents->extArea.y, pstContents->extArea.w, pstContents->extArea.h);
	}
	else
	{
		stRect = ONDK_Rect(pstContents->appArea.x, pstContents->appArea.y, pstContents->appArea.w, pstContents->appArea.h);
	}

	/* set application area */
	ONDK_GWM_APP_SetAppArea(stRect.x, stRect.y, stRect.w, stRect.h);

	/* init UI */
	nx_comp_listdlg_DrawDefaultUI(pstContents);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
inline Nx_PrismListDialog_Content_t	*NX_PRISM_COMP_ListDlg_GetContents(void)
{
	return nx_comp_listdlg_GetContents();
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetTitle(Nx_PrismListDialog_Content_t *pstContents, HUINT8 *pszTitle)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pszTitle == NULL)
	{
		/* title은 NULL일 수도 있지. */
		pstContents->pszTitle = (HUINT8*)" ";
	}
	else
	{
		pstContents->pszTitle = pszTitle;
	}

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetParentApp(Nx_PrismListDialog_Content_t *pstContents, ONDK_GWM_Callback_t parentApp)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* check param */
	if (parentApp == NULL)
	{
		NX_APP_Error("ERROR!!! parentApp was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* set value */
	pstContents->parentApp = parentApp;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetAttribute(Nx_PrismListDialog_Content_t *pstContents, HUINT32 eAttribute)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* set value */
	pstContents->eAttribute = eAttribute;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetContentList(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ppList)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* check param */
	if (((HUINT8 **)ppList == NULL) && (*(HUINT8 **)ppList == NULL))
	{
		NX_APP_Error("ERROR!!! ppList was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* set value */
	pstContents->ulList = ppList;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetAppArea(Nx_PrismListDialog_Content_t *pstContents, ONDK_Rect_t *pRect)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* check param */
	if (pRect == NULL)
	{
		NX_APP_Error("ERROR!!! pRect was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* set value */
	HxSTD_memcpy(&(pstContents->appArea), pRect, sizeof(ONDK_Rect_t));

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetNumOfList(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ulNumOfList)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* check param */
	if (ulNumOfList <= 0)
	{
		NX_APP_Error("ERROR!!! ulNumOfList was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* set value */
	pstContents->ulNumOfList = ulNumOfList;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetNumOfPerPage(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ulNumOfPerPage)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* check param */
	if (ulNumOfPerPage <= 0)
	{
		NX_APP_Error("ERROR!!! ulNumOfPerPage was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* set value */
	pstContents->ulNumOfPerPage = ulNumOfPerPage;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetCurIdxOfList(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ulCurIdxOfList)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* check param */ /* 주의, number of list가 먼저 셋팅되어 있어야 한다. */
	if (ulCurIdxOfList >= pstContents->ulNumOfList)
	{
		NX_APP_Error("ERROR!!! pstContents->ulNumOfList[%d], ulCurIdxOfList[%d]!! \n", pstContents->ulNumOfList, ulCurIdxOfList);
		return ONDK_RET_FAIL;
	}

	/* set value */
	pstContents->ulCurIdxOfList = ulCurIdxOfList;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetZeroListDlg(Nx_PrismListDialog_Content_t *pstContents)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	HxSTD_memset(pstContents, 0, sizeof(Nx_PrismListDialog_Content_t));

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetDisable(Nx_PrismListDialog_Content_t *pstContents, HUINT8 *pDisableList)
{
	/* check contents */
	if ((pstContents == NULL) || (pDisableList == NULL))
	{
		NX_APP_Error("ERROR!!! pstContents or pDisableList was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	pstContents->pDisbleList = pDisableList;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetLineSpace(Nx_PrismListDialog_Content_t *pstContents, HINT32 lLineSpace)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	pstContents->lLineSpace = lLineSpace;

	return ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetLineWithSpace(Nx_PrismListDialog_Content_t *pstContents, HINT32 lLineWSpace)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	pstContents->lLineWSpace = lLineWSpace;

	return ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetFontSize(Nx_PrismListDialog_Content_t *pstContents, HINT32 lFontSize)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	pstContents->lFontSize = lFontSize;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetImageList(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ppImageList)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* check param */
	if (((HUINT8 **)ppImageList == NULL) && (*(HUINT8 **)ppImageList == NULL))
	{
		NX_APP_Error("ERROR!!! ppImageList was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	pstContents->ulImageList = ppImageList;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetImageExtList(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ppImageExtList)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* check param */
	if (((HUINT8 **)ppImageExtList == NULL) && (*(HUINT8 **)ppImageExtList == NULL))
	{
		NX_APP_Error("ERROR!!! ppImageExtList was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	pstContents->ulImageExtList = ppImageExtList;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetExternalArea(Nx_PrismListDialog_Content_t *pstContents, ONDK_Rect_t *pRect)
{
	/* check contents */
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* check param */
	if (pRect == NULL)
	{
		NX_APP_Error("ERROR!!! pRect was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	/* set value */
	HxSTD_memcpy(&(pstContents->extArea), pRect, sizeof(ONDK_Rect_t));

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetListPopup(Nx_PrismListDialog_Content_t *pstContents, ONDK_GWM_Callback_t parentApp, ONDK_Rect_t *pRect,
									HINT32 lLineSpace, HUINT32 eAttribute, HUINT32 ulNumOfList, HUINT32 ulNumOfPerPage, HUINT32 ulCurIdx, HUINT32 ppList)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	/* set structure zero */
	eResult = NX_PRISM_COMP_ListDlg_SetZeroListDlg(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_COMP_ListDlg_SetZeroListDlg()!!\n");
		goto ERR_SetListPopup_INIT;
	}

	eResult = NX_PRISM_COMP_ListDlg_SetParentApp(pstContents, parentApp);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_COMP_ListDlg_SetParentApp()!!\n");
		goto ERR_SetListPopup_INIT;
	}

	eResult = NX_PRISM_COMP_ListDlg_SetAppArea(pstContents, pRect);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_COMP_ListDlg_SetAppArea()!!\n");
		goto ERR_SetListPopup_INIT;
	}

	eResult = NX_PRISM_COMP_ListDlg_SetAttribute(pstContents, eAttribute);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_COMP_ListDlg_SetAttribute()!!\n");
		goto ERR_SetListPopup_INIT;
	}

	eResult = NX_PRISM_COMP_ListDlg_SetNumOfList(pstContents, ulNumOfList);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_COMP_ListDlg_SetNumOfList()!!\n");
		goto ERR_SetListPopup_INIT;
	}

	eResult = NX_PRISM_COMP_ListDlg_SetNumOfPerPage(pstContents, ulNumOfPerPage);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_COMP_ListDlg_SetNumOfPerPage()!!\n");
		goto ERR_SetListPopup_INIT;
	}

	eResult = NX_PRISM_COMP_ListDlg_SetCurIdxOfList(pstContents, ulCurIdx);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_COMP_ListDlg_SetCurIdxOfList()!!\n");
		goto ERR_SetListPopup_INIT;
	}

	eResult = NX_PRISM_COMP_ListDlg_SetContentList(pstContents, ppList);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_COMP_ListDlg_SetContentList()!!\n");
		goto ERR_SetListPopup_INIT;
	}

	eResult = NX_PRISM_COMP_ListDlg_SetLineSpace(pstContents, lLineSpace);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_COMP_ListDlg_SetLineSpace()!!\n");
		goto ERR_SetListPopup_INIT;
	}

ERR_SetListPopup_INIT:
	return eResult;

}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_COMP_ListDlg_ProtoProc(nx_comp_listdlg_GetContents(), message, hAction, p1, p2, p3);
}


ONDK_Result_t	NX_PRISM_COMP_ListDlg_ProtoProc(Nx_PrismListDialog_Content_t *pstListDlg_Content, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			ulCurIndex = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	NX_APP_Print("NX_PRISM_COMP_ListDlg_ProtoProc message(0x%x) +\n", message);

	if (pstListDlg_Content == NULL)
	{
		NX_APP_Error("Error!!! (pstListDlg_Content == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	switch (message)
	{
		case MSG_GWM_CREATE:
			/* check value must be specified to lStart application */
			eResult = nx_comp_listdlg_CheckIsValueInitiated(pstListDlg_Content);
			if(eResult != ONDK_RET_OK)
			{
				/* must inited value is not specified, so quit */
				NX_APP_Error("ERROR!!! nx_comp_listdlg_CheckIsValueInitiated() !!\n");
				ONDK_GWM_APP_Destroy(0);
			}

			/* do init application */
			eResult = nx_comp_listdlg_MsgGwmCreate(pstListDlg_Content);
			if(eResult == ONDK_MESSAGE_BREAK)
			{
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_LIST_SELECTED:
			/* item was selected, return index to parent application */
			if (NX_PRISM_UI_LISTDLG_LIST_ID == p1)
			{
				ulCurIndex=p2;
				ONDK_GWM_SendMessage(pstListDlg_Content->parentApp,	MSG_APP_SETTING_VALUE_CHANGED,
									(HINT32)NULL, ulCurIndex, 0, 0);

				ONDK_GWM_APP_Destroy(NULL);

				return ONDK_MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_LIST_CHANGED:
			return ONDK_MESSAGE_BREAK;

		case MSG_GWM_LIST_UP_NOMORE:
		case MSG_GWM_LIST_DOWN_NOMORE:
			return ONDK_MESSAGE_BREAK;

		case MSG_GWM_KEYUP:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);

			if (pstListDlg_Content->eAttribute & eListDlg_MultiSelectHApp)
			{
				if (p1 == KEY_ARROWLEFT)
				{
					p1 = KEY_ARROWUP;
				}
				else if (p1 == KEY_ARROWRIGHT)
				{
					p1 = KEY_ARROWDOWN;
				}
			}

			ONDK_GWM_SendMessage(pstListDlg_Content->parentApp, MSG_APP_POPUP_LIST_INPUTED_KEYDOWN, (HINT32)NULL, p1, 0, (HUINT32)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()));
			switch (p1)
			{
				case KEY_GUIDE:
				case KEY_SCHEDULE:
				case KEY_MENU:
				case KEY_STANDBY:
					ONDK_GWM_APP_Destroy(NULL);
					return ONDK_MESSAGE_PASS;

				/* popup이므로 해당 어플만 빠져 나가면 된다. */
				case KEY_ARROWLEFT:
					if ((pstListDlg_Content->eAttribute & eListDlg_BG_MenuSubItemApp) || (pstListDlg_Content->eAttribute & eListDlg_SubItemApp))
					{
						ONDK_GWM_SendMessage(pstListDlg_Content->parentApp, MSG_APP_POPUP_LIST_DESTROYED,
										(Handle_t)NULL, p1, 0, (HUINT32)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()));
						ONDK_GWM_APP_Destroy(NULL);
						return ONDK_MESSAGE_BREAK;
					}
					break;
				case KEY_BACK:
				case KEY_EXIT:
					ONDK_GWM_SendMessage(pstListDlg_Content->parentApp, MSG_APP_POPUP_LIST_DESTROYED,
									(Handle_t)NULL, p1, 0, (HUINT32)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()));
					ONDK_GWM_APP_Destroy(NULL);
					return ONDK_MESSAGE_BREAK;

				/* 먹어버릴 키들 추가 */
				case KEY_0:
				case KEY_1:
				case KEY_2:
				case KEY_3:
				case KEY_4:
				case KEY_5:
				case KEY_6:
				case KEY_7:
				case KEY_8:
				case KEY_9:
				case KEY_RED:
				case KEY_GREEN:
				case KEY_YELLOW:
				case KEY_BLUE:
				case KEY_VFORMAT:
				case KEY_SUBTITLE:
				case KEY_AUDIO:
				case KEY_TEXT:
					NX_PRISM_NotAvailable_Show();
					return	ONDK_MESSAGE_BREAK;

				case KEY_CH_PLUS:
					p1 = KEY_PAGEUP;
					break;

				case KEY_CH_MINUS:
					p1 = KEY_PAGEDOWN;
					break;

				case KEY_OK:
				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
					//ONDK_GWM_NEW_List_MsgKeyDown(NX_PRISM_UI_LISTDLG_LIST_ID, p1);
					//return ONDK_MESSAGE_BREAK;
					break;
			}
			break;

		case MSG_GWM_DESTROY:
			break;

		default:
			break;
	}

	NX_APP_Print("NX_PRISM_COMP_ListDlg_ProtoProc message(0x%x) -\n", message);

	return	ONDK_GWM_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

