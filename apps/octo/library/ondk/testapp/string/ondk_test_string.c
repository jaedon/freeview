/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
#include 	<ondk.h>






/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#define ONDK_TEST_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)			\
			{															\
			(void)arg1;													\
			(void)arg2;													\
			(void)arg3;													\
			(void)arg4;							 						\
			}


#define DEF_ONDK_TEST_STRING_X  0
#define DEF_ONDK_TEST_STRING_Y  0
#define DEF_ONDK_TEST_STRING_W  1280
#define DEF_ONDK_TEST_STRING_H  720

#define DEF_ONDK_TEST_STRING_DATA_X  20
#define DEF_ONDK_TEST_STRING_DATA_Y  200
#define DEF_ONDK_TEST_STRING_DATA_W  1220
#define DEF_ONDK_TEST_STRING_DATA_H  500


#define DEF_ONDK_TEST_STRING_BTN_W  120
#define DEF_ONDK_TEST_STRING_BTN_H  60

#define DEF_ONDK_TEST_STRING_BTN1_X  10
#define DEF_ONDK_TEST_STRING_BTN1_Y  10

#define DEF_ONDK_TEST_STRING_BTN2_X  (DEF_ONDK_TEST_STRING_BTN1_X+DEF_ONDK_TEST_STRING_BTN_W+10)
#define DEF_ONDK_TEST_STRING_BTN2_Y  10

#define DEF_ONDK_TEST_STRING_BTN3_X  (DEF_ONDK_TEST_STRING_BTN2_X+DEF_ONDK_TEST_STRING_BTN_W+10)
#define DEF_ONDK_TEST_STRING_BTN3_Y  10

#define DEF_ONDK_TEST_STRING_BTN4_X  (DEF_ONDK_TEST_STRING_BTN3_X+DEF_ONDK_TEST_STRING_BTN_W+10)
#define DEF_ONDK_TEST_STRING_BTN4_Y  10

#define DEF_ONDK_TEST_STRING_BTN5_X  (DEF_ONDK_TEST_STRING_BTN4_X+DEF_ONDK_TEST_STRING_BTN_W+10)
#define DEF_ONDK_TEST_STRING_BTN5_Y  10

#define DEF_ONDK_TEST_SLIDETEXT_TIMER_ID 0x820610

typedef enum tagTESTSTRING_Object
{
	eTESTSTRING_Object_FRAME_Background = 0x991,
	eTESTSTRING_Object_RECT_DATA,
	eTESTSTRING_Object_RECT_DataOutLine,
	eTESTSTRING_Object_SlideText,

	eTESTSTRING_Object_BTN_DrawSingle,
	eTESTSTRING_Object_BTN_DrawMulti1,
	eTESTSTRING_Object_BTN_DrawMulti2,
	eTESTSTRING_Object_BTN_DrawMulti3,

	eTESTSTRING_Object_BTN_MAX
}TESTSTRING_Object_e;

typedef struct tagTESTSTRING_Contents_t
{
	HCHAR	szText[2048];
} TESTSTRING_Contents_t;

STATIC TESTSTRING_Contents_t *s_pstTestFontContents = NULL;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC TESTSTRING_Contents_t *local_teststring_GetContents(void)
{
	if(NULL == s_pstTestFontContents)
	{
		s_pstTestFontContents = ONDK_Calloc(sizeof(TESTSTRING_Contents_t));
	}
	return s_pstTestFontContents;
}


STATIC void local_teststring_SetUiObject_CreateButton(HUINT32 id, HCHAR *name, ONDK_Rect_t rect)
{
	ONDK_GWM_COM_Button_Create(id , &rect, name);
	ONDK_GWM_COM_Button_SetTitleSize(id , 14);
	ONDK_GWM_COM_Button_SetFontColor(id , 0xFF00FF00, 0xFF00FF00);
	ONDK_GWM_COM_Button_SetButtonColor(id , 0xFF0000A0, 0xFF000060, 0xFF0000FF, 0xFF000020);
}


ONDK_Result_t local_teststring_DrawSingleString (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, tw = 0;

	HCHAR		*pDrawString = "HUMAX is getting excellent reviews with the latest products bundled";

	ONDK_Color_t	title_fontColor = 0xFFFFFFFF;
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

	tw = w/3;
	x = x1; y += y1;
	ONDK_FONT_DrawStringAbbr(screen, hFontHandle, "DrawStringAlignAbbr Test (left, right, center)", x, y, w, 0xFF0000FF);
	ONDK_DRAW_Line(screen, (x+tw), (y+24), (x+tw), (y+96), 0xFFFF00FF);
//	ONDK_SURFACE_SetClip(screen, ONDK_Region((x ), (y ), (x+tw), (y+96)));

	x = x1; y += 24;
	ONDK_DRAW_RectElement(screen, x, y, w, 24, 0xFF820610);
	ONDK_DRAW_RectElement(screen, x, y, w, 20, 0xFF820610);
	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pDrawString, x, y, w, title_fontColor, ONDKSTF_LEFT);
	x = x1; y += 24;
	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pDrawString, x, y, w, title_fontColor, ONDKSTF_RIGHT);
	x = x1; y += 24;
	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pDrawString, x, y, w, title_fontColor, ONDKSTF_CENTER);

	x = x1; y += 48;
	ONDK_FONT_DrawStringAbbr(screen, hFontHandle, "DrawStringAlignAbbr Test (left, right, center)", x, y, w, 0xFF0000FF);
	ONDK_DRAW_Line(screen, (x+tw), (y+24), (x+tw), (y+96), 0xFFFF00FF);
	x = x1; y += 24;
	ONDK_DRAW_RectElement(screen, x, y, tw, 24, 0xFF820610);
	ONDK_DRAW_RectElement(screen, x, y, tw, 20, 0xFF820610);
	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pDrawString, x, y, tw, title_fontColor, ONDKSTF_LEFT);
	x = x1; y += 24;
	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pDrawString, x, y, tw, title_fontColor, ONDKSTF_RIGHT);
	x = x1; y += 24;
	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, pDrawString, x, y, tw, title_fontColor, ONDKSTF_CENTER);

	x = x1; y += 48;
	ONDK_FONT_DrawStringAbbr(screen, hFontHandle, "DrawStringAlign Test (left, right, center)", x, y, w, 0xFF0000FF);
	ONDK_DRAW_Line(screen, (x+tw), (y+24), (x+tw), (y+96), 0xFFFF00FF);
	x = x1; y += 24;
	ONDK_DRAW_RectElement(screen, x, y, tw, 24, 0xFF820610);
	ONDK_DRAW_RectElement(screen, x, y, tw, 20, 0xFF820610);
	ONDK_FONT_DrawStringAlign(screen, hFontHandle, pDrawString, x, y, tw, title_fontColor, ONDKSTF_LEFT);
	x = x1; y += 24;
	ONDK_FONT_DrawStringAlign(screen, hFontHandle, pDrawString, x, y, tw, title_fontColor, ONDKSTF_RIGHT);
	x = x1; y += 24;
	ONDK_FONT_DrawStringAlign(screen, hFontHandle, pDrawString, x, y, tw, title_fontColor, ONDKSTF_CENTER);

	x = x1; y += 48;
	ONDK_FONT_DrawStringAbbr(screen, hFontHandle, "ONDK_FONT_DrawStringAbbr Test (left, right, center)", x, y, w, 0xFF0000FF);
	ONDK_DRAW_Line(screen, (x+tw), (y+24), (x+tw), (y+96), 0xFFFF00FF);
	x = x1; y += 24;
	ONDK_DRAW_RectElement(screen, x, y, tw, 24, 0xFF820610);
	ONDK_DRAW_RectElement(screen, x, y, tw, 20, 0xFF820610);
	ONDK_FONT_DrawStringAbbr(screen, hFontHandle, pDrawString, x, y, tw, title_fontColor);


	if (hFontHandle)
		ONDK_FONT_Release (hFontHandle);

	return	ONDK_RET_OK;
}

ONDK_Result_t local_teststring_DrawMultiString1 (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, tw = 0, th = 0;
	HINT32		lineCount = 0;

	HCHAR		*pDrawString = "The International CES is the world's gathering place for all who thrive on the business of consumer technologies.\nHeld in Las Vegas every year, it has served as the proving ground for innovators and breakthrough technologies for more than 40 years?the global stage where next-generation innovations are introduced to the marketplace. CES showcases more than 3,200 exhibitors, including manufacturers, developers and suppliers of consumer technology hardware, content, technology delivery systems and more; a conference program with more than 300 sessions; and more than 152,000 attendees from more than 150 countries.";

	ONDK_Color_t	title_fontColor = 0xFFFFFFFF;
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

	x = x1; y += y1;
	tw = w;
	//th = h / 2;
	//ONDK_FONT_DrawStringForMultiLine(screen, hFontHandle, x, y, tw, th, pDrawString, 3, title_fontColor);
	lineCount = ONDK_FONT_GetLineCount(screen, hFontHandle, x, y, tw, pDrawString, 7, ONDKSTF_LEFT, &th);
	ONDK_DRAW_RectElement(screen, x, y, tw, th, 0xFF820610);
	ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x, y, tw, th, pDrawString, 7, title_fontColor, ONDKSTF_LEFT);

	y += th;
	y += 10;
	ONDK_DRAW_RectElement(screen, x, y, tw, th, 0xFF820610);
	ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x, y, tw, th, pDrawString, 7, title_fontColor, ONDKSTF_CENTER);

	y += th;
	y += 10;
	ONDK_DRAW_RectElement(screen, x, y, tw, th, 0xFF820610);
	ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x, y, tw, th, pDrawString, 7, title_fontColor, ONDKSTF_RIGHT);


	if (hFontHandle)
		ONDK_FONT_Release (hFontHandle);

	return	ONDK_RET_OK;
}


ONDK_Result_t local_teststring_DrawMultiString2 (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, tw = 0, th = 0;
	HINT32		lineCount = 0;

	HCHAR		*pDrawString = "The International CES is the world's gathering place for all who thrive on the business of consumer technologies.\nHeld in Las Vegas every year, it has served as the proving ground for innovators and breakthrough technologies for more than 40 years?the global stage where next-generation innovations are introduced to the marketplace. CES showcases more than 3,200 exhibitors, including manufacturers, developers and suppliers of consumer technology hardware, content, technology delivery systems and more; a conference program with more than 300 sessions; and more than 152,000 attendees from more than 150 countries.";

	ONDK_Color_t	title_fontColor = 0xFFFFFFFF;
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

	x = x1; y += y1;
	tw = w;
	//th = h / 2;
	//ONDK_FONT_DrawStringForMultiLine(screen, hFontHandle, x, y, tw, th, pDrawString, 3, title_fontColor);
	lineCount = ONDK_FONT_GetLineCount(screen, hFontHandle, x, y, tw, pDrawString, 7, ONDKSTF_LEFT|ONDKSTF_WORDWRAP, &th);
	ONDK_DRAW_RectElement(screen, x, y, tw, th, 0xFF820610);
	ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x, y, tw, th, pDrawString, 7, title_fontColor, ONDKSTF_LEFT|ONDKSTF_WORDWRAP);

	y += th;
	y += 10;
	ONDK_DRAW_RectElement(screen, x, y, tw, th, 0xFF820610);
	ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x, y, tw, th, pDrawString, 7, title_fontColor, ONDKSTF_CENTER|ONDKSTF_WORDWRAP);

	y += th;
	y += 10;
	ONDK_DRAW_RectElement(screen, x, y, tw, th, 0xFF820610);
	ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x, y, tw, th, pDrawString, 7, title_fontColor, ONDKSTF_RIGHT|ONDKSTF_WORDWRAP);


	if (hFontHandle)
		ONDK_FONT_Release (hFontHandle);

	return	ONDK_RET_OK;
}

ONDK_Result_t local_teststring_DrawMultiString3 (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, tw = 0, th = 0;
	HINT32		lineCount = 0;

	HCHAR		*pDrawString = "The International CES is the world's gathering place for all who thrive on the business of consumer technologies.\nHeld in Las Vegas every year, it has served as the proving ground for innovators and breakthrough technologies for more than 40 years?the global stage where next-generation innovations are introduced to the marketplace. CES showcases more than 3,200 exhibitors, including manufacturers, developers and suppliers of consumer technology hardware, content, technology delivery systems and more; a conference program with more than 300 sessions; and more than 152,000 attendees from more than 150 countries.";

	ONDK_Color_t	title_fontColor = 0xFFFFFFFF;
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

	x = x1; y += y1;
	tw = w;
	//th = h / 2;
	//ONDK_FONT_DrawStringForMultiLine(screen, hFontHandle, x, y, tw, th, pDrawString, 3, title_fontColor);
	lineCount = ONDK_FONT_GetLineCount(screen, hFontHandle, x, y, tw, pDrawString, 7,ONDKSTF_EQUALSPACE, &th);
	ONDK_DRAW_RectElement(screen, x, y, tw, th, 0xFF820610);
	ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x, y, tw, th, pDrawString, 7, title_fontColor, ONDKSTF_EQUALSPACE);

	y += th;
	y += 10;
	lineCount = ONDK_FONT_GetLineCount(screen, hFontHandle, x, y, tw, pDrawString, 7, ONDKSTF_EQUALSPACE|ONDKSTF_WORDWRAP, &th);
	ONDK_DRAW_RectElement(screen, x, y, tw, th, 0xFF820610);
	ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x, y, tw, th, pDrawString, 7, title_fontColor, ONDKSTF_EQUALSPACE|ONDKSTF_WORDWRAP);



	if (hFontHandle)
		ONDK_FONT_Release (hFontHandle);

	return	ONDK_RET_OK;
}


STATIC void local_teststring_SetUiObject(TESTSTRING_Contents_t *pstContents)
{
	ONDK_Rect_t rect;

	ONDK_GWM_APP_SetAppArea(DEF_ONDK_TEST_STRING_X, DEF_ONDK_TEST_STRING_Y, DEF_ONDK_TEST_STRING_W, DEF_ONDK_TEST_STRING_H);
#if 0
	rect = ONDK_Rect(DEF_ONDK_TEST_STRING_X, DEF_ONDK_TEST_STRING_Y, DEF_ONDK_TEST_STRING_W, DEF_ONDK_TEST_STRING_H);
	ONDK_GWM_COM_Rect_Create(eTESTSTRING_Object_FRAME_Background, &rect, 0xFFFFFFFF);
#endif
	rect = ONDK_Rect(DEF_ONDK_TEST_STRING_BTN1_X, DEF_ONDK_TEST_STRING_BTN1_Y, DEF_ONDK_TEST_STRING_BTN_W, DEF_ONDK_TEST_STRING_BTN_H);
	local_teststring_SetUiObject_CreateButton(eTESTSTRING_Object_BTN_DrawSingle, "DrawSingleLine", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_STRING_BTN2_X, DEF_ONDK_TEST_STRING_BTN2_Y, DEF_ONDK_TEST_STRING_BTN_W, DEF_ONDK_TEST_STRING_BTN_H);
	local_teststring_SetUiObject_CreateButton(eTESTSTRING_Object_BTN_DrawMulti1, "DrawMultiLine(1)", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_STRING_BTN3_X, DEF_ONDK_TEST_STRING_BTN3_Y, DEF_ONDK_TEST_STRING_BTN_W, DEF_ONDK_TEST_STRING_BTN_H);
	local_teststring_SetUiObject_CreateButton(eTESTSTRING_Object_BTN_DrawMulti2, "DrawMultiLine(2)", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_STRING_BTN4_X, DEF_ONDK_TEST_STRING_BTN4_Y, DEF_ONDK_TEST_STRING_BTN_W, DEF_ONDK_TEST_STRING_BTN_H);
	local_teststring_SetUiObject_CreateButton(eTESTSTRING_Object_BTN_DrawMulti3, "DrawMultiLine(3)", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_STRING_BTN5_X, DEF_ONDK_TEST_STRING_BTN5_Y, (DEF_ONDK_TEST_STRING_BTN_W*4), DEF_ONDK_TEST_STRING_BTN_H);
	ONDK_GWM_COM_SlideText_Create(eTESTSTRING_Object_SlideText, &rect, "The International CES is the world's gathering place for all who thrive on the business of consumer technologies.\nHeld in Las Vegas every year, it has served as the proving ground for innovators and breakthrough technologies for more than 40 years?the global stage where next-generation innovations are introduced to the marketplace. CES showcases more than 3,200 exhibitors, including manufacturers, developers and suppliers of consumer technology hardware, content, technology delivery systems and more; a conference program with more than 300 sessions; and more than 152,000 attendees from more than 150 countries.");
	ONDK_GWM_COM_SlideText_ChangeColor(eTESTSTRING_Object_SlideText, 0xFF00FF00);

	rect = ONDK_Rect(DEF_ONDK_TEST_STRING_DATA_X, DEF_ONDK_TEST_STRING_DATA_Y, DEF_ONDK_TEST_STRING_DATA_W, DEF_ONDK_TEST_STRING_DATA_H);
	ONDK_GWM_COM_Rect_Create(eTESTSTRING_Object_RECT_DataOutLine, &rect, 0xFFAAAAAA);

	rect = ONDK_Rect(DEF_ONDK_TEST_STRING_DATA_X, DEF_ONDK_TEST_STRING_DATA_Y, DEF_ONDK_TEST_STRING_DATA_W, DEF_ONDK_TEST_STRING_DATA_H);
	ONDK_GWM_COM_Rect_Create(eTESTSTRING_Object_RECT_DATA, &rect, 0x00000000);

	ONDK_GWM_SetTimer(DEF_ONDK_TEST_SLIDETEXT_TIMER_ID, 50);

	ONDK_GWM_Obj_SetFocus (eTESTSTRING_Object_BTN_DrawSingle);
}

/**************************************************************************************************/
#define _________AP_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/


//MSG_GWM_CREATE
STATIC ONDK_Result_t local_teststring_MsgGwmCreate(TESTSTRING_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	local_teststring_SetUiObject(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_teststring_MsgGwmKeyDown(TESTSTRING_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 nKeyId = p1;

	switch(nKeyId)
	{
	case KEY_EXIT:
	case KEY_BACK:
		ONDK_GWM_APP_Destroy(NULL);
		return GWM_MESSAGE_BREAK;

	default:
		break;
	}

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_teststring_MsgGwmClicked(TESTSTRING_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 nCmdId = (HUINT32)p1;

	switch(nCmdId)
	{
	case eTESTSTRING_Object_BTN_DrawSingle:
		HLIB_CMD_Printf("[%s:%d] eTESTSTRING_Object_BTN_DrawSingle Clicked!\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTSTRING_Object_RECT_DATA, local_teststring_DrawSingleString);
		ONDK_GWM_APP_InvalidateObject (eTESTSTRING_Object_RECT_DATA);
		break;

	case eTESTSTRING_Object_BTN_DrawMulti1:
		HLIB_CMD_Printf("[%s:%d] eTESTSTRING_Object_BTN_DrawMulti Clicked!\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTSTRING_Object_RECT_DATA, local_teststring_DrawMultiString1);
		ONDK_GWM_APP_InvalidateObject (eTESTSTRING_Object_RECT_DATA);
		break;

	case eTESTSTRING_Object_BTN_DrawMulti2:
		HLIB_CMD_Printf("[%s:%d] eTESTSTRING_Object_BTN_DrawMulti Clicked!\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTSTRING_Object_RECT_DATA, local_teststring_DrawMultiString2);
		ONDK_GWM_APP_InvalidateObject (eTESTSTRING_Object_RECT_DATA);
		break;

	case eTESTSTRING_Object_BTN_DrawMulti3:
		HLIB_CMD_Printf("[%s:%d] eTESTSTRING_Object_BTN_DrawMulti Clicked!\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTSTRING_Object_RECT_DATA, local_teststring_DrawMultiString3);
		ONDK_GWM_APP_InvalidateObject (eTESTSTRING_Object_RECT_DATA);
		break;

	default:
		break;
	}

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_teststring_MsgGwmDestroy(TESTSTRING_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);


	return GWM_RET_OK;
}

//MSG_GWM_TIMER
STATIC ONDK_Result_t local_teststring_MsgGwmTimer(TESTSTRING_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	switch(p1)
	{
	case DEF_ONDK_TEST_SLIDETEXT_TIMER_ID:
		ONDK_GWM_COM_SlideText_Update(eTESTSTRING_Object_SlideText, 3);
		break;

	default:
		break;
	}

	return GWM_RET_OK;
}

STATIC ONDK_Result_t local_teststring_MsgGwmScrollRight(TESTSTRING_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus (eTESTSTRING_Object_BTN_DrawSingle);

	return GWM_RET_OK;
}

 STATIC ONDK_Result_t local_teststring_MsgGwmScrollLeft(TESTSTRING_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus ((eTESTSTRING_Object_BTN_MAX-1));

	return GWM_RET_OK;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t ONDK_TEST_STRING_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;
	TESTSTRING_Contents_t 	*pstContents = local_teststring_GetContents();

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_teststring_MsgGwmCreate(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_KEYDOWN:
		eResult = local_teststring_MsgGwmKeyDown(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_CLICKED:
		eResult = local_teststring_MsgGwmClicked(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_DESTROY:
		eResult = local_teststring_MsgGwmDestroy(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_TIMER:
		eResult = local_teststring_MsgGwmTimer(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLRIGHT:
		eResult = local_teststring_MsgGwmScrollRight(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLLEFT:
		eResult = local_teststring_MsgGwmScrollLeft(pstContents, p1, p2, p3);
		break;
	default:
		break;
	}

	if(eResult != GWM_RET_OK)
	{
		return eResult;
	}
	return ONDK_GWM_ProcessMessageDefault(nMessage, handle, p1, p2, p3);
}

void ONDK_TEST_STRING_Start(void)
{
	if(NULL == ONDK_GWM_APP_Get(ONDK_TEST_STRING_Proc))
	{
		HLIB_CMD_Printf("[%s:%d] Create \r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "ONDK_TEST_STRING_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, ONDK_TEST_STRING_Proc, 0, 0, 0, 0);
	}
	else
	{
		HLIB_CMD_Printf("[%s:%d] Destroy\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Destroy(ONDK_TEST_STRING_Proc);
	}
}



