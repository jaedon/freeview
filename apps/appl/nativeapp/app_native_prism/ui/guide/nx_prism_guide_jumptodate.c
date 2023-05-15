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
 * @file	  		nx_prism_guide_jumptodate.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_core_metadata_epg.h>
#include <nx_core_metadata_pf.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_PRISM_GUIDE_JUMPDATE_ITEM_ID							(eMenuFrame_Window_GUIDE + 200)

#define	NX_PRISM_GUIDE_JUMPDATE_WINDOW_X						258
#define	NX_PRISM_GUIDE_JUMPDATE_WINDOW_Y						270
#define	NX_PRISM_GUIDE_JUMPDATE_WINDOW_W						(765 + 40)
#define	NX_PRISM_GUIDE_JUMPDATE_WINDOW_H						180

#define	NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_ID				(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 1)
#define	NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_X				6
#define	NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_Y				0
#define	NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_W				765
#define	NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_H				NX_PRISM_GUIDE_JUMPDATE_WINDOW_H

#define	NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_ID					(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 2)
#define	NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_X					30
#define	NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_Y					(46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_W					705
#define	NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_H					(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_ID						(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 3)
#define	NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_X						0
#define	NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_Y						108
#define	NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_W						13
#define	NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_H						20

#define	NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_ID					(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 4)
#define	NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_X						(758 + NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_X)
#define	NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_Y						108
#define	NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_W						13
#define	NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_H						20

#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_1_ID				(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 5)
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_2_ID				(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 6)
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_3_ID				(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 7)
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_4_ID				(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 8)
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_5_ID				(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 9)
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_6_ID				(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 10)
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_7_ID				(NX_PRISM_GUIDE_JUMPDATE_ITEM_ID + 11)

#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_X					15
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_Y					76
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_W					105
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_H					84

#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_TEXT_X_TAB				10
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_TEXT_Y_TAB				(38 - NX_PRISM_FONT_SIZE_26)
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_TEXT_WIDTH				85
#define	NX_PRISM_GUIDE_JUMPDATE_BUTTON_TEXT_LS					(28 + NX_PRISM_FONT_VSPACE)


#define	NX_PRISM_GUIDE_JUMPDATE_DATE_PERIOD						14		// 2주.
#define	NX_PRISM_GUIDE_JUMPDATE_DATE_ITEM_NUM					7


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct tagJumpDateItem {
	HINT32		dateTextID;
	HINT32		lineImgID;
	HINT32		weekTextID;
	HCHAR		szDate[NX_TEXT_SIZE_8];
	HCHAR		szWeekDay[NX_TEXT_SIZE_16];
	HBOOL		bFocused;
} Nx_JumpToDateItem_t;

typedef struct tagJumpToDateContext {

	HUINT32			unSearchUnitPerHalfHourTime;
	HUINT32			unStandardSearchTime;
	HxDATETIME_t	*pArrayDateTime;		// 2주치 다들고 시작. 매번, 정확한 date 를 다시 구하지 말자.
	HxDATETIME_t	searchUnitDateTime;
	HxDATETIME_t	searchStandardDateTime;
	HINT32			nDateIndex, nDisDateMin, nDisDateMax;

	Nx_JumpToDateItem_t	stJumpDateItem[NX_PRISM_GUIDE_JUMPDATE_DATE_ITEM_NUM];
} Nx_JumpToDateContext_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_JumpToDateContext_t		s_stJumpToDateContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_JumpToDateContext_t	*nx_jumpToDate_GetContents(void);

static ONDK_Result_t	nx_jumpToDate_CreateObject(Nx_JumpToDateContext_t *pstContents, HINT32 lIndex);
static ONDK_Result_t	nx_jumpToDate_UpdateObject(Nx_JumpToDateContext_t *pstContents, HINT32 lIndex, HxDATETIME_t *pstDateTime, HBOOL bFocused);
static void 			nx_jumpToDate_UpdateDateInform(Nx_JumpToDateContext_t *pstContents);

static ONDK_Result_t	nx_jumpToDate_DrawButton_Cursor_A1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static void				nx_jumpToDate_DrawBackground(void);
static ONDK_Result_t 	nx_jumpToDate_SetUiObject(Nx_JumpToDateContext_t *pstContents);

static ONDK_Result_t	nx_jumpToDate_MsgGwmCreate(Nx_JumpToDateContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_jumpToDate_MsgGwmDestroy(Nx_JumpToDateContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_jumpToDate_MsgGwmKeyDown(Nx_JumpToDateContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline Nx_JumpToDateContext_t	*nx_jumpToDate_GetContents(void)
{
	return	&s_stJumpToDateContent;
}


static void 	nx_jumpToDate_UpdateDateInform(Nx_JumpToDateContext_t *pstContents)
{
	HINT32		i = 0 ,  j = 0, loop = NX_PRISM_GUIDE_JUMPDATE_DATE_ITEM_NUM;
	HINT32		lCurIndex = pstContents->nDateIndex;
	HINT32		lDisDateMin = pstContents->nDisDateMin;
	HINT32		lDisDateMax = pstContents->nDisDateMax;
	HBOOL		bFocused = FALSE;

	if ((lCurIndex >= lDisDateMin) && (lCurIndex <= lDisDateMax))
	{
		i = lDisDateMin;
		loop = lDisDateMax;
	}
	else
	{
		if (lCurIndex > (NX_PRISM_GUIDE_JUMPDATE_DATE_ITEM_NUM-1))
		{
			i = lCurIndex - (NX_PRISM_GUIDE_JUMPDATE_DATE_ITEM_NUM-1);
			loop = i + (NX_PRISM_GUIDE_JUMPDATE_DATE_ITEM_NUM - 1);//lCurIndex;
		}
		else if (lCurIndex > 0)		// 이경우는 left 로 인해 감산되는 경우에만 온다.
		{
			i = pstContents->nDisDateMin - 1;
			loop = pstContents->nDisDateMax - 1;
		}
		else
		{
			i = 0;
			loop = (NX_PRISM_GUIDE_JUMPDATE_DATE_ITEM_NUM - 1);
		}
		pstContents->nDisDateMin = i;
		pstContents->nDisDateMax = loop;
	}

	for (i = i, j = 0; i <= loop; i++, j++)
	{
		bFocused = (i == lCurIndex) ? TRUE : FALSE;
		nx_jumpToDate_UpdateObject(pstContents, j, &pstContents->pArrayDateTime[i], bFocused);
	}

	if (pstContents->nDisDateMin > 0)
	{
		ONDK_GWM_Obj_SetVisible(NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_ID, TRUE);
	}
	else
	{
		ONDK_GWM_Obj_SetVisible(NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_ID, FALSE);
	}

	if (pstContents->nDisDateMax >= (NX_PRISM_GUIDE_JUMPDATE_DATE_PERIOD - 1))
	{
		ONDK_GWM_Obj_SetVisible(NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_ID, FALSE);
	}
	else
	{
		ONDK_GWM_Obj_SetVisible(NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_ID, TRUE);
	}
}


static ONDK_Result_t	nx_jumpToDate_CreateObject(Nx_JumpToDateContext_t *pstContents, HINT32 lIndex)
{
	HUINT32			ulWeekIndex = 0;
	ONDK_Rect_t		stRect;
	Nx_JumpToDateItem_t *pstCurItem = &pstContents->stJumpDateItem[lIndex];


	stRect = ONDK_Rect(NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_X + (lIndex*NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_W),
			NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_Y, NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_W, NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_H);

	HxSTD_sprintf(pstCurItem->szDate, "%02d.%02d", pstContents->pArrayDateTime[lIndex].stDate.ucDay,
													pstContents->pArrayDateTime[lIndex].stDate.ucMonth);

	ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(pstContents->pArrayDateTime[lIndex].stDate);
	HxSTD_sprintf(pstCurItem->szWeekDay, "%s", ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex)));

	ONDK_GWM_COM_Button_Create(NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_1_ID + lIndex, &stRect, (HCHAR*)pstCurItem->szDate);
	ONDK_GWM_COM_Button_SetExtraText(NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_1_ID + lIndex, (HUINT8*)pstCurItem->szWeekDay);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_1_ID + lIndex, nx_jumpToDate_DrawButton_Cursor_A1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_jumpToDate_UpdateObject(Nx_JumpToDateContext_t *pstContents, HINT32 lIndex, HxDATETIME_t *pstDateTime, HBOOL bFocused)
{
	HUINT32			ulWeekIndex = 0;
	ONDK_Color_t	textColor = C_T_Whistle_100;
	Nx_JumpToDateItem_t *pstCurItem = &pstContents->stJumpDateItem[lIndex];

	pstCurItem->bFocused = bFocused;

	ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(pstDateTime->stDate);
	HxSTD_sprintf(pstCurItem->szWeekDay, "%s", ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex)));
	ONDK_GWM_COM_Button_SetExtraText(NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_1_ID + lIndex, (HUINT8*)pstCurItem->szWeekDay);

	HxSTD_sprintf(pstCurItem->szDate, "%02d.%02d", pstDateTime->stDate.ucDay, pstDateTime->stDate.ucMonth);
	ONDK_GWM_COM_Button_ChangeTitle(NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_1_ID + lIndex, (HCHAR*)pstCurItem->szDate);

	if (pstCurItem->bFocused)
	{
		textColor = C_T_Base01;
		ONDK_GWM_Obj_SetFocus(NX_PRISM_GUIDE_JUMPDATE_BUTTON_ITEM_1_ID + lIndex);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_jumpToDate_DrawButton_Cursor_A1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
															ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HUINT8		*szTitle = NULL, *szExtraText = NULL;

	ONDK_Color_t	date_fontColor = 0x00000000, week_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* get text  - Item Day*/
	szTitle = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);

	/* get text  - Item Weekly*/
	szExtraText = (HUINT8*)ONDK_GWM_COM_Button_GetExtraTextByObj(objBtn);
	if (szExtraText == NULL)
	{
		NX_APP_Error("Assert!!! Please Check!! \n");
		return ERR_FAIL;
	}

	date_fontColor = COL (C_T_Whistle_100);
	week_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Whistle_15);

	eFontId = eFont_SystemNormal;
	lCursorMg = 0;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			date_fontColor = COL(C_T_Gong08_40);
			week_fontColor = COL(C_T_Gong08_40);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			date_fontColor = COL (C_T_Bang_100);
			week_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			date_fontColor = COL (C_T_Bang_100);
			week_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			date_fontColor = COL (C_T_Whistle_100);
			week_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_GUIDE_JUMPDATE_BUTTON_TEXT_X_TAB;
	lText_y_offset = NX_PRISM_GUIDE_JUMPDATE_BUTTON_TEXT_Y_TAB;

	ulFontSize = NX_PRISM_FONT_SIZE_26;
	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, NX_PRISM_GUIDE_JUMPDATE_BUTTON_TEXT_WIDTH, date_fontColor, ONDKSTF_CENTER);
		ONDK_FONT_Release(hFontHandle);
	}

	lText_y_offset = NX_PRISM_GUIDE_JUMPDATE_BUTTON_TEXT_Y_TAB + NX_PRISM_GUIDE_JUMPDATE_BUTTON_TEXT_LS;
	ulFontSize = NX_PRISM_FONT_SIZE_20;
	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szExtraText, x, y, NX_PRISM_GUIDE_JUMPDATE_BUTTON_TEXT_WIDTH, week_fontColor, ONDKSTF_CENTER);
		ONDK_FONT_Release(hFontHandle);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static void		nx_jumpToDate_DrawBackground(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	/* background start */
	stRect = ONDK_Rect(NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_X, NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_Y, NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_W, NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_ID, &stRect, COL(C_G_Bang_90));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_GUIDE_JUMPDATE_BACKGROUND_BOX_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG_TBLine);
}


static ONDK_Result_t 	nx_jumpToDate_SetUiObject(Nx_JumpToDateContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	ONDK_GWM_APP_SetAppArea(NX_PRISM_GUIDE_JUMPDATE_WINDOW_X, NX_PRISM_GUIDE_JUMPDATE_WINDOW_Y,
								NX_PRISM_GUIDE_JUMPDATE_WINDOW_W, NX_PRISM_GUIDE_JUMPDATE_WINDOW_H);

	// Draw Backgroud
	nx_jumpToDate_DrawBackground();

	// Message
	stRect = ONDK_Rect(NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_X, NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_Y,
						NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_W, NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_ID, &stRect, (HCHAR*)ONDK_GetString(RES_MESG_5660_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_GUIDE_JUMPDATE_MESSAGE_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// Draw Arrow Left
	stRect = ONDK_Rect(NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_X, NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_Y,
						NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_W, NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_GUIDE_JUMPDATE_LEFT_IMG_ID, (HCHAR*)RES_106_00_ARROW_LEFT_PNG);

	// Draw Arrow Right
	stRect = ONDK_Rect(NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_X, NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_Y,
						NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_W, NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_GUIDE_JUMPDATE_RIGHT_IMG_ID, (HCHAR*)RES_106_00_ARROW_RIGHT_PNG);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_jumpToDate_MsgGwmCreate(Nx_JumpToDateContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		i = 0, compareDate = -100;
	HUINT32		unStandardSearchTime = 0, unUnixtimePerDay = (SECONDS_PER_HOUR * 24);
	HUINT32		ulPerHalfHourTime = (HUINT32)p1, ulStandTime = (HUINT32)p2;
	HUINT8		*pszPanelStr = NULL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}
	else
	{
		HxSTD_MemSet(pstContents, 0, sizeof(Nx_JumpToDateContext_t));
		pstContents->pArrayDateTime = NULL;
	}

	pstContents->unSearchUnitPerHalfHourTime = ulPerHalfHourTime;
	unStandardSearchTime = pstContents->unStandardSearchTime = ulStandTime;

	pstContents->pArrayDateTime = (HxDATETIME_t*)NX_APP_Calloc(sizeof(HxDATETIME_t) * NX_PRISM_GUIDE_JUMPDATE_DATE_PERIOD);
	if (pstContents->pArrayDateTime == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_NOLOCALMEMORY;
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unStandardSearchTime, &pstContents->searchStandardDateTime);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unSearchUnitPerHalfHourTime, &pstContents->searchUnitDateTime);

	// 채우기
	for(i = 0; i < NX_PRISM_GUIDE_JUMPDATE_DATE_PERIOD; i++)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(unStandardSearchTime, &pstContents->pArrayDateTime[i]);

		if (pstContents->pArrayDateTime[i].stDate.ucMonth == pstContents->searchUnitDateTime.stDate.ucMonth &&
			pstContents->pArrayDateTime[i].stDate.ucDay == pstContents->searchUnitDateTime.stDate.ucDay)
			pstContents->nDateIndex = i;

		unStandardSearchTime += unUnixtimePerDay;
	}
	compareDate = HLIB_DATETIME_CompareDate(pstContents->searchUnitDateTime.stDate, pstContents->searchStandardDateTime.stDate);
	if (pstContents->unSearchUnitPerHalfHourTime == 0)
	{
		compareDate = 0;
	}
	pstContents->nDateIndex = compareDate;
	pstContents->nDisDateMin = 0;
	pstContents->nDisDateMax = (NX_PRISM_GUIDE_JUMPDATE_DATE_ITEM_NUM - 1);

	nx_jumpToDate_SetUiObject(pstContents);

	for (i = 0; i < NX_PRISM_GUIDE_JUMPDATE_DATE_ITEM_NUM; i++)
	{
		nx_jumpToDate_CreateObject(pstContents, i);
	}

	nx_jumpToDate_UpdateDateInform(pstContents);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Guide);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Guide !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);


	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_jumpToDate_MsgGwmDestroy(Nx_JumpToDateContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->pArrayDateTime)
	{
		HLIB_MEM_Free(pstContents->pArrayDateTime);
		pstContents->pArrayDateTime = NULL;
	}

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_jumpToDate_MsgGwmKeyDown(Nx_JumpToDateContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
			ONDK_GWM_PostMessage(NX_PRISM_GUIDE_Proc, MSG_CORE_SERVICE_EPG_JUMPTODATE_UPDATE, 0,
					(HINT32)&pstContents->pArrayDateTime[pstContents->nDateIndex],0, 0);
				//ONDK_GWM_APP_Destroy(0);	// Nx_Prism_Guide Proc 에서 메세지 수신 후 destroy 하게 된다...
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWLEFT:
			pstContents->nDateIndex--;
			if (pstContents->nDateIndex < 0)
				pstContents->nDateIndex = 0;
			nx_jumpToDate_UpdateDateInform(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			pstContents->nDateIndex++;
			if (pstContents->nDateIndex > 13)
				pstContents->nDateIndex = 13;
			nx_jumpToDate_UpdateDateInform(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_guide_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_GUIDE_JUMPTODATE_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_JumpToDateContext_t	*pstContents = NULL;

	pstContents = nx_jumpToDate_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_jumpToDate_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_jumpToDate_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_jumpToDate_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Guide)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		default:
			break;
	}

	if (eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

