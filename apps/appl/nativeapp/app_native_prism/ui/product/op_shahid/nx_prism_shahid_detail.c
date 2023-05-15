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
 * @file	  		nx_prism_shahid_detail.c
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
#define	NX_PRISM_SHAHID_DETAIL_ITEM_ID					eMenuFrame_Window_Shahid_Detail

#define	NX_SHAHID_DETAIL_WINDOW_X						0
#define	NX_SHAHID_DETAIL_WINDOW_Y						0
#define	NX_SHAHID_DETAIL_WINDOW_W						NX_PRISM_OSD_HORIZONTAL
#define	NX_SHAHID_DETAIL_WINDOW_H						NX_PRISM_OSD_VERTICAL

#define NX_SHAHID_DETAIL_DIM_ID							NX_PRISM_SHAHID_DETAIL_ITEM_ID+1

#define NX_SHAHID_DETAIL_LOGO_IMG_ID					NX_PRISM_SHAHID_DETAIL_ITEM_ID+2
#define NX_SHAHID_DETAIL_LOGO_IMG_X						57
#define NX_SHAHID_DETAIL_LOGO_IMG_Y						33
#define NX_SHAHID_DETAIL_LOGO_IMG_W						202
#define NX_SHAHID_DETAIL_LOGO_IMG_H						45

#define NX_SHAHID_DETAIL_IMG_AREA_X						59
#define NX_SHAHID_DETAIL_IMG_AREA_Y						103
#define NX_SHAHID_DETAIL_IMG_AREA_W						321
#define NX_SHAHID_DETAIL_IMG_AREA_H						368

#define NX_SHAHID_DETAIL_THUMBNAIL_ID					NX_PRISM_SHAHID_DETAIL_ITEM_ID+3

#define NX_SHAHID_DETAIL_INFO_AREA_X					420
#define NX_SHAHID_DETAIL_INFO_AREA_Y					103
#define NX_SHAHID_DETAIL_INFO_AREA_W					800
#define NX_SHAHID_DETAIL_INFO_AREA_H					368

#define NX_SHAHID_DETAIL_TITLE_ID						NX_PRISM_SHAHID_DETAIL_ITEM_ID+4
#define NX_SHAHID_DETAIL_TITLE_X						NX_SHAHID_DETAIL_INFO_AREA_X
#define NX_SHAHID_DETAIL_TITLE_Y						NX_SHAHID_DETAIL_INFO_AREA_Y
#define NX_SHAHID_DETAIL_TITLE_W						800
#define NX_SHAHID_DETAIL_TITLE_H						40

#define NX_SHAHID_DETAIL_EPISODE_INFO_ID				NX_PRISM_SHAHID_DETAIL_ITEM_ID+5
#define NX_SHAHID_DETAIL_EPISODE_INFO_X					NX_SHAHID_DETAIL_INFO_AREA_X
#define NX_SHAHID_DETAIL_EPISODE_INFO_Y					NX_SHAHID_DETAIL_INFO_AREA_Y+38
#define NX_SHAHID_DETAIL_EPISODE_INFO_W					800
#define NX_SHAHID_DETAIL_EPISODE_INFO_H					30

#define NX_SHAHID_DETAIL_HD_ICON_ID						NX_PRISM_SHAHID_DETAIL_ITEM_ID+6
#define NX_SHAHID_DETAIL_HD_ICON_X						NX_SHAHID_DETAIL_INFO_AREA_X
#define NX_SHAHID_DETAIL_HD_ICON_Y						NX_SHAHID_DETAIL_INFO_AREA_Y+73
#define NX_SHAHID_DETAIL_HD_ICON_W						30
#define NX_SHAHID_DETAIL_HD_ICON_H						24

#define NX_SHAHID_DETAIL_GENRE_ID						NX_PRISM_SHAHID_DETAIL_ITEM_ID+7
#define NX_SHAHID_DETAIL_GENRE_X						NX_SHAHID_DETAIL_INFO_AREA_X+NX_SHAHID_DETAIL_HD_ICON_W+10
#define NX_SHAHID_DETAIL_GENRE_Y						NX_SHAHID_DETAIL_INFO_AREA_Y+73
#define NX_SHAHID_DETAIL_GENRE_W						760
#define NX_SHAHID_DETAIL_GENRE_H						30

#define NX_SHAHID_DETAIL_SYNOPSIS_ID					NX_PRISM_SHAHID_DETAIL_ITEM_ID+8
#define NX_SHAHID_DETAIL_SYNOPSIS_X						NX_SHAHID_DETAIL_INFO_AREA_X
#define NX_SHAHID_DETAIL_SYNOPSIS_Y						NX_SHAHID_DETAIL_INFO_AREA_Y+129
#define NX_SHAHID_DETAIL_SYNOPSIS_W						765
#define NX_SHAHID_DETAIL_SYNOPSIS_H						152

#define NX_SHAHID_DETAIL_SYNOPSIS_LINE01_ID				NX_PRISM_SHAHID_DETAIL_ITEM_ID+9
#define NX_SHAHID_DETAIL_SYNOPSIS_LINE01_X				NX_SHAHID_DETAIL_INFO_AREA_X
#define NX_SHAHID_DETAIL_SYNOPSIS_LINE01_Y				NX_SHAHID_DETAIL_INFO_AREA_Y+115
#define NX_SHAHID_DETAIL_SYNOPSIS_LINE01_W				800
#define NX_SHAHID_DETAIL_SYNOPSIS_LINE01_H				2

#define NX_SHAHID_DETAIL_SYNOPSIS_LINE02_ID				NX_PRISM_SHAHID_DETAIL_ITEM_ID+10
#define NX_SHAHID_DETAIL_SYNOPSIS_LINE02_X				NX_SHAHID_DETAIL_INFO_AREA_X
#define NX_SHAHID_DETAIL_SYNOPSIS_LINE02_Y				NX_SHAHID_DETAIL_INFO_AREA_Y+293
#define NX_SHAHID_DETAIL_SYNOPSIS_LINE02_W				800
#define NX_SHAHID_DETAIL_SYNOPSIS_LINE02_H				2

#define NX_SHAHID_DETAIL_BTN_PLAY_ID					NX_PRISM_SHAHID_DETAIL_ITEM_ID+11
#define NX_SHAHID_DETAIL_BTN_PLAY_X						NX_SHAHID_DETAIL_INFO_AREA_X
#define NX_SHAHID_DETAIL_BTN_PLAY_Y						NX_SHAHID_DETAIL_INFO_AREA_Y+314
#define NX_SHAHID_DETAIL_BTN_PLAY_W						220
#define NX_SHAHID_DETAIL_BTN_PLAY_H						54

#define NX_SHAHID_DETAIL_SCROLL_UP_ID					NX_PRISM_SHAHID_DETAIL_ITEM_ID+12
#define NX_SHAHID_DETAIL_SCROLL_UP_X					NX_SHAHID_DETAIL_INFO_AREA_X+762
#define NX_SHAHID_DETAIL_SCROLL_UP_Y					NX_SHAHID_DETAIL_INFO_AREA_Y+126
#define NX_SHAHID_DETAIL_SCROLL_UP_W					36
#define NX_SHAHID_DETAIL_SCROLL_UP_H					36

#define NX_SHAHID_DETAIL_SCROLL_DOWN_ID					NX_PRISM_SHAHID_DETAIL_ITEM_ID+13
#define NX_SHAHID_DETAIL_SCROLL_DOWN_X					NX_SHAHID_DETAIL_INFO_AREA_X+762
#define NX_SHAHID_DETAIL_SCROLL_DOWN_Y					NX_SHAHID_DETAIL_INFO_AREA_Y+248
#define NX_SHAHID_DETAIL_SCROLL_DOWN_W					36
#define NX_SHAHID_DETAIL_SCROLL_DOWN_H					36

#define NX_SHAHID_DETAIL_SCROLL_BG_ID					NX_PRISM_SHAHID_DETAIL_ITEM_ID+14
#define NX_SHAHID_DETAIL_SCROLL_BG_X					NX_SHAHID_DETAIL_INFO_AREA_X+779
#define NX_SHAHID_DETAIL_SCROLL_BG_Y					NX_SHAHID_DETAIL_INFO_AREA_Y+162
#define NX_SHAHID_DETAIL_SCROLL_BG_W					2
#define NX_SHAHID_DETAIL_SCROLL_BG_H					88

#define NX_SHAHID_DETAIL_SCROLL_BAR_ID					NX_PRISM_SHAHID_DETAIL_ITEM_ID+15
//#define NX_SHAHID_DETAIL_SCROLL_BAR_X
//#define NX_SHAHID_DETAIL_SCROLL_BAR_Y
#define NX_SHAHID_DETAIL_SCROLL_BAR_W					4
#define NX_SHAHID_DETAIL_SCROLL_BAR_H					16


#define NX_SHAHID_DETAIL_EPISODE_AREA_X					60
#define NX_SHAHID_DETAIL_EPISODE_AREA_Y					500
#define NX_SHAHID_DETAIL_EPISODE_AREA_W					1160














/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct tagShahidDetail {
	HINT32	currentIndex;
} Nx_ShahidDetailContext_t;



/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
Nx_ShahidDetailContext_t s_stShahidDetailContext;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_ShahidDetailContext_t	*nx_shahid_detail_GetContext(void);
static ONDK_Result_t	nx_shahid_detail_MsgGwmCreate(Nx_ShahidDetailContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_shahid_detail_MsgGwmKeyDown(Nx_ShahidDetailContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_shahid_detail_SetUiObject(Nx_ShahidDetailContext_t *pstContext);
static ONDK_Result_t 	nx_shahid_detail_MultilineDrawFunc (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t 	nx_shahid_detail_BtnPlayDrawFunc(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);





/******************************************************************************/
#define _________Private_functions____________________________________________
static inline Nx_ShahidDetailContext_t	*nx_shahid_detail_GetContext(void)
{
	return &s_stShahidDetailContext;
}

static ONDK_Result_t	nx_shahid_detail_MsgGwmCreate(Nx_ShahidDetailContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{

	if (pstContext == NULL)
	{
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(pstContext, 0, sizeof(Nx_ShahidDetailContext_t));



	ONDK_GWM_APP_SetAppArea(NX_SHAHID_DETAIL_WINDOW_X, NX_SHAHID_DETAIL_WINDOW_Y,
							NX_SHAHID_DETAIL_WINDOW_W, NX_SHAHID_DETAIL_WINDOW_H);
	//Set UI object
	nx_shahid_detail_SetUiObject(pstContext);

	ONDK_GWM_Obj_SetFocus(NX_SHAHID_DETAIL_BTN_PLAY_ID);


	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_detail_MsgGwmKeyDown(Nx_ShahidDetailContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//ONDK_Result_t res = ONDK_RET_FAIL;

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
	NX_COMMON_NOT_USED_ARGUMENT(pstContext, p1, p2, p3);

	switch (p1)
	{
		/* keys to exit */
		case KEY_BACK:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_STANDBY:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
			break;
		case KEY_ARROWLEFT:
			break;
		case KEY_ARROWRIGHT:
			break;
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		default:
			NX_APP_Print("nx_shahid_detail_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", p1);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_MESSAGE_BREAK;
}

static ONDK_Result_t	nx_shahid_detail_MsgGwmDestroy(Nx_ShahidDetailContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{

	NX_COMMON_NOT_USED_PARAM(pstContext);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t 	nx_shahid_detail_MultilineDrawFunc (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		w = 0, h = 0, th = 0;
	HINT32		lineCount = 0;

	HCHAR		*pDrawString = "The International CES is the world's gathering place for all who thrive on the business of consumer technologies.\nHeld in Las Vegas every year, it has served as the proving ground for innovators and breakthrough technologies for more than 40 years?the global stage where next-generation innovations are introduced to the marketplace. CES showcases more than 3,200 exhibitors, including manufacturers, developers and suppliers of consumer technology hardware, content, technology delivery systems and more; a conference program with more than 300 sessions; and more than 152,000 attendees from more than 150 countries.The International CES is the world's gathering place for all who thrive on the business of consumer technologies.\nHeld in Las Vegas every year, it has served as the proving ground for innovators and breakthrough technologies for more than 40 years?the global stage where next-generation innovations are introduced to the marketplace. CES showcases more than 3,200 exhibitors, including manufacturers, developers and suppliers of consumer technology hardware, content, technology delivery systems and more; a conference program with more than 300 sessions; and more than 152,000 attendees from more than 150 countries.";

	ONDK_Color_t	title_fontColor = COL(C_T_Gong02);
	ONDKFontHandle	*hFontHandle;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, 20);
	if (hFontHandle == NULL)
	{
		ONDK_Assert(0);
		return	ONDK_RET_FAIL;
	}


	lineCount = ONDK_FONT_GetLineCount(screen, hFontHandle, x1, y1, w, pDrawString, 7, ONDKSTF_LEFT, &th);

	ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x1, y1, w, 152, pDrawString, 7, title_fontColor, ONDKSTF_LEFT);


	if (hFontHandle)
		ONDK_FONT_Release (hFontHandle);

	return	ONDK_RET_OK;
}

static ONDK_Result_t nx_shahid_detail_BtnPlayDrawFunc(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, text_w = 0, lImgOffset = 0;

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

	// Draw image in button
	lImgOffset = 55;
	ONDK_GWM_COM_Button_GetImageHandleByObj(objBtn, &hNormalImg, &hFocusedImg, &hDisabledImg, &hPushedImg);
	push_status = ONDK_GWM_Obj_GetObjectStatus((ONDK_GWM_GuiObject_t)objBtn);

	if (( push_status& GUIOBJ_DISABLED) && (hDisabledImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hDisabledImg);
		ONDK_IMG_Blit(hDisabledImg, screen, x1+lImgOffset, (y1+(h/2)-(stImgSize.h/2)));
	}
	else if ((push_status & GUIOBJ_PUSHED) && (hPushedImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hPushedImg);
		ONDK_IMG_Blit(hPushedImg, screen, x1+lImgOffset, (y1+(h/2)-(stImgSize.h/2)));
	}
	else if ((push_status & GUIOBJ_HILIGHTED) && (hFocusedImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hFocusedImg);
		ONDK_IMG_Blit(hFocusedImg, screen, x1+lImgOffset, (y1+(h/2)-(stImgSize.h/2)));
	}
	else if (hNormalImg!= NULL)
	{
		stImgSize = ONDK_IMG_GetSize(hNormalImg);
		ONDK_IMG_Blit(hNormalImg, screen, x1+lImgOffset, (y1+(h/2)-(stImgSize.h/2)));
	}

	lText_x_offset = 32;
	lText_y_offset = 14;
	text_w = 110;

	// TEXT - TP Info Title
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	if (hFontHandle)
	{
		x = x1 + lText_x_offset + lImgOffset;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, w, title_fontColor, ONDKSTF_LEFT);
		ONDK_FONT_Release(hFontHandle);
	}



	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}



static ONDK_Result_t	nx_shahid_detail_SetUiObject(Nx_ShahidDetailContext_t * pstContext)
{
	ONDK_Rect_t stRect;

	if(pstContext == NULL)
	{
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	//Dim color
	stRect = ONDK_Rect(NX_SHAHID_DETAIL_WINDOW_X, NX_SHAHID_DETAIL_WINDOW_Y, NX_SHAHID_DETAIL_WINDOW_W, NX_SHAHID_DETAIL_WINDOW_H);
	ONDK_GWM_COM_Rect_Create(NX_SHAHID_DETAIL_DIM_ID, &stRect, COL(C_G_Bang_85));

	//Logo image
	stRect = ONDK_Rect(NX_SHAHID_DETAIL_LOGO_IMG_X, NX_SHAHID_DETAIL_LOGO_IMG_Y, NX_SHAHID_DETAIL_LOGO_IMG_W, NX_SHAHID_DETAIL_LOGO_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_SHAHID_DETAIL_LOGO_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_SHAHID_DETAIL_LOGO_IMG_ID, RES_602_11_SHAHID_LOGO_PNG);

	//Draw Title TEXT
	stRect = ONDK_Rect(NX_SHAHID_DETAIL_TITLE_X, NX_SHAHID_DETAIL_TITLE_Y, NX_SHAHID_DETAIL_TITLE_W, NX_SHAHID_DETAIL_TITLE_H);
	ONDK_GWM_COM_Text_Create(NX_SHAHID_DETAIL_TITLE_ID, &stRect, "TEST TITLE");
	ONDK_GWM_COM_Text_SetFont(NX_SHAHID_DETAIL_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_SHAHID_DETAIL_TITLE_ID, NX_PRISM_FONT_SIZE_36);
	ONDK_GWM_COM_Text_SetAlign(NX_SHAHID_DETAIL_TITLE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_SHAHID_DETAIL_TITLE_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));

	//Draw Title TEXT
	stRect = ONDK_Rect(NX_SHAHID_DETAIL_EPISODE_INFO_X, NX_SHAHID_DETAIL_EPISODE_INFO_Y, NX_SHAHID_DETAIL_EPISODE_INFO_W,
	NX_SHAHID_DETAIL_EPISODE_INFO_H);
	ONDK_GWM_COM_Text_Create(NX_SHAHID_DETAIL_EPISODE_INFO_ID, &stRect, "TEST Episode info");
	ONDK_GWM_COM_Text_SetFont(NX_SHAHID_DETAIL_EPISODE_INFO_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_SHAHID_DETAIL_EPISODE_INFO_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_SHAHID_DETAIL_EPISODE_INFO_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_SHAHID_DETAIL_EPISODE_INFO_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));

	//Draw HD icon
	stRect = ONDK_Rect(NX_SHAHID_DETAIL_HD_ICON_X, NX_SHAHID_DETAIL_HD_ICON_Y, NX_SHAHID_DETAIL_HD_ICON_W, NX_SHAHID_DETAIL_HD_ICON_H);
	ONDK_GWM_COM_Image_Create(NX_SHAHID_DETAIL_HD_ICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_SHAHID_DETAIL_HD_ICON_ID, RES_203_00_RESOLUTION_HD_PNG);

	//draw genre
	stRect = ONDK_Rect(NX_SHAHID_DETAIL_GENRE_X, NX_SHAHID_DETAIL_GENRE_Y, NX_SHAHID_DETAIL_GENRE_W, NX_SHAHID_DETAIL_GENRE_H);
	ONDK_GWM_COM_Text_Create(NX_SHAHID_DETAIL_GENRE_ID, &stRect, "TEST Genre, / test");
	ONDK_GWM_COM_Text_SetFont(NX_SHAHID_DETAIL_GENRE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_SHAHID_DETAIL_GENRE_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_SHAHID_DETAIL_GENRE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_SHAHID_DETAIL_GENRE_ID, COL(C_T_Orgel02), COL(C_T_Orgel02));

	//draw synopsis, line
	stRect = ONDK_Rect(NX_SHAHID_DETAIL_SYNOPSIS_X, NX_SHAHID_DETAIL_SYNOPSIS_Y, NX_SHAHID_DETAIL_SYNOPSIS_W, NX_SHAHID_DETAIL_SYNOPSIS_H);
	ONDK_GWM_COM_Rect_Create(NX_SHAHID_DETAIL_SYNOPSIS_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_SHAHID_DETAIL_SYNOPSIS_ID, nx_shahid_detail_MultilineDrawFunc);

	stRect = ONDK_Rect(NX_SHAHID_DETAIL_SYNOPSIS_LINE01_X, NX_SHAHID_DETAIL_SYNOPSIS_LINE01_Y, NX_SHAHID_DETAIL_SYNOPSIS_LINE01_W, NX_SHAHID_DETAIL_SYNOPSIS_LINE01_H);
	ONDK_GWM_COM_Rect_Create(NX_SHAHID_DETAIL_SYNOPSIS_LINE01_ID, &stRect, COL(C_G_Whistle_10));
	stRect = ONDK_Rect(NX_SHAHID_DETAIL_SYNOPSIS_LINE02_X, NX_SHAHID_DETAIL_SYNOPSIS_LINE02_Y, NX_SHAHID_DETAIL_SYNOPSIS_LINE02_W, NX_SHAHID_DETAIL_SYNOPSIS_LINE02_H);
	ONDK_GWM_COM_Rect_Create(NX_SHAHID_DETAIL_SYNOPSIS_LINE02_ID, &stRect, COL(C_G_Whistle_10));

	// play button
	stRect = ONDK_Rect(NX_SHAHID_DETAIL_BTN_PLAY_X, NX_SHAHID_DETAIL_BTN_PLAY_Y, NX_SHAHID_DETAIL_BTN_PLAY_W, NX_SHAHID_DETAIL_BTN_PLAY_H);
	ONDK_GWM_COM_Button_Create(NX_SHAHID_DETAIL_BTN_PLAY_ID, &stRect, "PLAY");
	ONDK_GWM_COM_Button_ChangeImage(NX_SHAHID_DETAIL_BTN_PLAY_ID, RES_602_11_SHAHID_BTN_PLAY01_N_PNG,RES_602_11_SHAHID_BTN_PLAY01_C_PNG,NULL,NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_SHAHID_DETAIL_BTN_PLAY_ID, nx_shahid_detail_BtnPlayDrawFunc);

	//scroll
	stRect = ONDK_Rect(NX_SHAHID_DETAIL_SCROLL_UP_X, NX_SHAHID_DETAIL_SCROLL_UP_Y, NX_SHAHID_DETAIL_SCROLL_UP_W, NX_SHAHID_DETAIL_SCROLL_UP_H);
	ONDK_GWM_COM_Button_Create(NX_SHAHID_DETAIL_SCROLL_UP_ID, &stRect, NULL);
	ONDK_GWM_COM_Button_ChangeImage(NX_SHAHID_DETAIL_SCROLL_UP_ID, RES_602_11_SHAHID_SCROLL02_UP_N_PNG,RES_602_11_SHAHID_SCROLL02_UP_C_PNG,NULL,NULL);

	stRect = ONDK_Rect(NX_SHAHID_DETAIL_SCROLL_BG_X, NX_SHAHID_DETAIL_SCROLL_BG_Y, NX_SHAHID_DETAIL_SCROLL_BG_W, NX_SHAHID_DETAIL_SCROLL_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_SHAHID_DETAIL_SCROLL_BG_ID, &stRect, COL(C_G_Whistle_10));

	stRect = ONDK_Rect(NX_SHAHID_DETAIL_SCROLL_BG_X, NX_SHAHID_DETAIL_SCROLL_BG_Y, NX_SHAHID_DETAIL_SCROLL_BAR_W, NX_SHAHID_DETAIL_SCROLL_BAR_H);
	ONDK_GWM_COM_Image_Create(NX_SHAHID_DETAIL_SCROLL_BAR_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_SHAHID_DETAIL_SCROLL_BAR_ID, RES_602_11_SHAHID_SCROLL02_BAR_PNG);

	stRect = ONDK_Rect(NX_SHAHID_DETAIL_SCROLL_DOWN_X, NX_SHAHID_DETAIL_SCROLL_DOWN_Y, NX_SHAHID_DETAIL_SCROLL_DOWN_W, NX_SHAHID_DETAIL_SCROLL_DOWN_H);
	ONDK_GWM_COM_Button_Create(NX_SHAHID_DETAIL_SCROLL_DOWN_ID, &stRect, NULL);
	ONDK_GWM_COM_Button_ChangeImage(NX_SHAHID_DETAIL_SCROLL_DOWN_ID, RES_602_11_SHAHID_SCROLL02_DOWN_N_PNG,RES_602_11_SHAHID_SCROLL02_DOWN_C_PNG,NULL,NULL);

	return ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SHAHID_DETAIL_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_ShahidDetailContext_t	*pstContext = NULL;

	pstContext = nx_shahid_detail_GetContext();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_shahid_detail_MsgGwmCreate(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_shahid_detail_MsgGwmKeyDown(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_shahid_detail_MsgGwmDestroy(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			break;

		case MSG_GWM_CLICKED:
			break;

		case MSG_GWM_TIMER:
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

#endif

