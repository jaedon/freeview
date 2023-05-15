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


#define DEF_ONDK_TEST_FONT_X  0
#define DEF_ONDK_TEST_FONT_Y  0
#define DEF_ONDK_TEST_FONT_W  1280
#define DEF_ONDK_TEST_FONT_H  720

#define DEF_ONDK_TEST_FONT_DATA_X  20
#define DEF_ONDK_TEST_FONT_DATA_Y  200
#define DEF_ONDK_TEST_FONT_DATA_W  1220
#define DEF_ONDK_TEST_FONT_DATA_H  500


#define DEF_ONDK_TEST_FONT_BTN_W  120
#define DEF_ONDK_TEST_FONT_BTN_H  60

#define DEF_ONDK_TEST_FONT_BTN0_X  10
#define DEF_ONDK_TEST_FONT_BTN0_Y  10

#define DEF_ONDK_TEST_FONT_BTN1_X  (DEF_ONDK_TEST_FONT_BTN0_X+DEF_ONDK_TEST_FONT_BTN_W+10)
#define DEF_ONDK_TEST_FONT_BTN1_Y  10

#define DEF_ONDK_TEST_FONT_BTN2_X  (DEF_ONDK_TEST_FONT_BTN1_X+DEF_ONDK_TEST_FONT_BTN_W+10)
#define DEF_ONDK_TEST_FONT_BTN2_Y  10

#define DEF_ONDK_TEST_FONT_BTN3_X  (DEF_ONDK_TEST_FONT_BTN2_X+DEF_ONDK_TEST_FONT_BTN_W+10)
#define DEF_ONDK_TEST_FONT_BTN3_Y  10

#define DEF_ONDK_TEST_FONT_BTN4_X  (DEF_ONDK_TEST_FONT_BTN3_X+DEF_ONDK_TEST_FONT_BTN_W+10)
#define DEF_ONDK_TEST_FONT_BTN4_Y  10

#define DEF_ONDK_TEST_FONT_BTN5_X  (DEF_ONDK_TEST_FONT_BTN4_X+DEF_ONDK_TEST_FONT_BTN_W+10)
#define DEF_ONDK_TEST_FONT_BTN5_Y  10


typedef enum tagTESTFONT_Object
{
	eTESTFONT_Object_FRAME_Background = 0x991,
	eTESTFONT_Object_RECT_DATA,
	eTESTFONT_Object_RECT_DataOutLine,

	eTESTFONT_Object_BTN_DrawCharCode,
	eTESTFONT_Object_BTN_DrawSingle,
	eTESTFONT_Object_BTN_DrawMulti1,
	eTESTFONT_Object_BTN_DrawMulti2,
	eTESTFONT_Object_BTN_DrawMulti3,

	eTESTFONT_Object_BTN_MAX
}TESTFONT_Object_e;

typedef struct tagTESTFONT_Contents_t
{
	HCHAR	szText[2048];
	HUINT16		ucs2Code[4];
} TESTFONT_Contents_t;

STATIC TESTFONT_Contents_t *s_pstTestFontContents = NULL;


STATIC ONDK_Result_t  local_testfont_FontFileTest(ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC TESTFONT_Contents_t *local_testfont_GetContents(void)
{
	if(NULL == s_pstTestFontContents)
	{
		s_pstTestFontContents = ONDK_Calloc(sizeof(TESTFONT_Contents_t));
	}
	return s_pstTestFontContents;
}


STATIC void local_testfont_SetUiObject_CreateButton(HUINT32 id, HCHAR *name, ONDK_Rect_t rect)
{
	ONDK_GWM_COM_Button_Create(id , &rect, name);
	ONDK_GWM_COM_Button_SetTitleSize(id , 14);
	ONDK_GWM_COM_Button_SetFontColor(id , 0xFF00FF00, 0xFF00FF00);
	ONDK_GWM_COM_Button_SetButtonColor(id , 0xFF0000A0, 0xFF000060, 0xFF0000FF, 0xFF000020);
}


ONDK_Result_t local_testfont_DrawSingleString (ONDKSurfaceHandle screen, ONDK_Region_t *area,
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

ONDK_Result_t local_testfont_DrawMultiString1 (ONDKSurfaceHandle screen, ONDK_Region_t *area,
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


ONDK_Result_t local_testfont_DrawMultiString2 (ONDKSurfaceHandle screen, ONDK_Region_t *area,
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

ONDK_Result_t local_testfont_DrawMultiString3 (ONDKSurfaceHandle screen, ONDK_Region_t *area,
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

STATIC void local_testfont_SetInitData(TESTFONT_Contents_t *pstContents)
{
	pstContents->ucs2Code[0]=0x0021;
}

STATIC void local_testfont_SetUiObject(TESTFONT_Contents_t *pstContents)
{
	ONDK_Rect_t rect;

	ONDK_GWM_APP_SetAppArea(DEF_ONDK_TEST_FONT_X, DEF_ONDK_TEST_FONT_Y, DEF_ONDK_TEST_FONT_W, DEF_ONDK_TEST_FONT_H);

#if 0
	rect = ONDK_Rect(DEF_ONDK_TEST_FONT_X, DEF_ONDK_TEST_FONT_Y, DEF_ONDK_TEST_FONT_W, DEF_ONDK_TEST_FONT_H);
	ONDK_GWM_COM_Rect_Create(eTESTFONT_Object_FRAME_Background, &rect, 0xFFFFFFFF);
#endif
	rect = ONDK_Rect(DEF_ONDK_TEST_FONT_BTN0_X, DEF_ONDK_TEST_FONT_BTN0_Y, DEF_ONDK_TEST_FONT_BTN_W, DEF_ONDK_TEST_FONT_BTN_H);
	local_testfont_SetUiObject_CreateButton(eTESTFONT_Object_BTN_DrawCharCode, "DrawCharCode", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_FONT_BTN1_X, DEF_ONDK_TEST_FONT_BTN1_Y, DEF_ONDK_TEST_FONT_BTN_W, DEF_ONDK_TEST_FONT_BTN_H);
	local_testfont_SetUiObject_CreateButton(eTESTFONT_Object_BTN_DrawSingle, "DrawSingleLine", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_FONT_BTN2_X, DEF_ONDK_TEST_FONT_BTN2_Y, DEF_ONDK_TEST_FONT_BTN_W, DEF_ONDK_TEST_FONT_BTN_H);
	local_testfont_SetUiObject_CreateButton(eTESTFONT_Object_BTN_DrawMulti1, "DrawMultiLine(1)", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_FONT_BTN3_X, DEF_ONDK_TEST_FONT_BTN3_Y, DEF_ONDK_TEST_FONT_BTN_W, DEF_ONDK_TEST_FONT_BTN_H);
	local_testfont_SetUiObject_CreateButton(eTESTFONT_Object_BTN_DrawMulti2, "DrawMultiLine(2)", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_FONT_BTN4_X, DEF_ONDK_TEST_FONT_BTN4_Y, DEF_ONDK_TEST_FONT_BTN_W, DEF_ONDK_TEST_FONT_BTN_H);
	local_testfont_SetUiObject_CreateButton(eTESTFONT_Object_BTN_DrawMulti3, "DrawMultiLine(3)", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_FONT_DATA_X, DEF_ONDK_TEST_FONT_DATA_Y, DEF_ONDK_TEST_FONT_DATA_W, DEF_ONDK_TEST_FONT_DATA_H);
	ONDK_GWM_COM_Rect_Create(eTESTFONT_Object_RECT_DataOutLine, &rect, 0xFFAAAAAA);

	rect = ONDK_Rect(DEF_ONDK_TEST_FONT_DATA_X, DEF_ONDK_TEST_FONT_DATA_Y, DEF_ONDK_TEST_FONT_DATA_W, DEF_ONDK_TEST_FONT_DATA_H);
	ONDK_GWM_COM_Rect_Create(eTESTFONT_Object_RECT_DATA, &rect, 0x00000000);

	//DRAW Font Test
	ONDK_GWM_APP_SetObjectDrawMethod(eTESTFONT_Object_RECT_DATA, local_testfont_FontFileTest);
	
	ONDK_GWM_Obj_SetFocus (eTESTFONT_Object_BTN_DrawCharCode);
}



STATIC ONDK_Result_t  local_testfont_FontFileTest(ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, tw = 0;
	HINT32		lineX = 0, lineY=0,fontX=0,fontY=0;
#if 0
	HINT32		idx = 0,th = 0;;
	HINT32		charWidth = 0;

	HUINT8		utf8Code[4] ={0,};
	ONDK_Color_t	title_fontColor = 0xFFFFFFFF;
#endif	
	ONDKFontHandle	*hFontHandle;


	if(s_pstTestFontContents->ucs2Code[0] == 0xFBAC)
	{
		s_pstTestFontContents->ucs2Code[0] = 0x0021;
	}
	
	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, 40);
	if (hFontHandle == NULL)
	{
		ONDK_Assert(0);
		return	ONDK_RET_FAIL;
	}

	x = x1; y += y1;
	tw = w;
	lineX = x;
	lineY = y;
	fontX=x;
	fontY=y;
#if 0	
	for(idx = 0; idx < 200; idx++)
	{
		if( (idx % 20) == 0) 
		{
			//가로 line
			y+=th;
			x = x1;				
			
			ONDK_DRAW_RectElement(screen, x, y, tw, th, 0xFF000000);				
		}					

		if( (idx % 5) == 0) 
		{//세로 line
			ONDK_DRAW_RectElement(screen, lineX, lineY, 31, 500, 0xFF000000);
			
			lineX=lineX+30;							
		}

		
			if( (idx % 40) == 0) 
			{
				fontX=x1;		
				fontY=fontY+th;
			}
		if( (s_pstTestFontContents->ucs2Code[0] < 0x0080) || (s_pstTestFontContents->ucs2Code[0] > 0x009F) 
			|| (s_pstTestFontContents->ucs2Code[0] < 0x05EB) || (s_pstTestFontContents->ucs2Code[0] > 0x05EF)
			|| (s_pstTestFontContents->ucs2Code[0] < 0x05F5) || (s_pstTestFontContents->ucs2Code[0] > 0x0530) 
			|| (s_pstTestFontContents->ucs2Code[0] < 0x05C8) || (s_pstTestFontContents->ucs2Code[0] > 0x05CF)
			|| (s_pstTestFontContents->ucs2Code[0] < 0x07B2) || (s_pstTestFontContents->ucs2Code[0] > 0x089F)
			|| (s_pstTestFontContents->ucs2Code[0] < 0x08AD) || (s_pstTestFontContents->ucs2Code[0] > 0x08E3)
			|| (s_pstTestFontContents->ucs2Code[0] < 0x09FC) || (s_pstTestFontContents->ucs2Code[0] > 0x0A00)
			|| (s_pstTestFontContents->ucs2Code[0] < 0x0A76) || (s_pstTestFontContents->ucs2Code[0] > 0x0A80)
			|| (s_pstTestFontContents->ucs2Code[0] < 0x0AD1) || (s_pstTestFontContents->ucs2Code[0] > 0x0ADF)
			|| (s_pstTestFontContents->ucs2Code[0] < 0x0AF2) || (s_pstTestFontContents->ucs2Code[0] < 0x0AFF)
			/*|| (s_pstTestFontContents->ucs2Code[0] < 0x08AD)*/ )
		{
			HLIB_CP_ConvertUCS2ToCodepage (eHxCP_UNICODE_UTF8, (HUINT16 *)s_pstTestFontContents->ucs2Code, utf8Code, 4);
			ONDK_FONT_GetLineCount(screen, hFontHandle, fontX, fontY, tw, utf8Code, 7, ONDKSTF_LEFT, &th);				
			charWidth =ONDK_FONT_GetStringWidth(hFontHandle, utf8Code);
			ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, fontX, fontY, tw, th, utf8Code, 7, title_fontColor, ONDKSTF_LEFT);
			fontX+=30;
		}
		s_pstTestFontContents->ucs2Code[0] = s_pstTestFontContents->ucs2Code[0]+0x0001;
		
	}
#endif
	if (hFontHandle)
		ONDK_FONT_Release (hFontHandle);
	
	return	ONDK_RET_OK;

}

/**************************************************************************************************/
#define _________AP_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/


//MSG_GWM_CREATE
STATIC ONDK_Result_t local_testfont_MsgGwmCreate(TESTFONT_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	local_testfont_SetInitData(pstContents);
	local_testfont_SetUiObject(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_testfont_MsgGwmKeyDown(TESTFONT_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
STATIC ONDK_Result_t local_testfont_MsgGwmClicked(TESTFONT_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 nCmdId = (HUINT32)p1;

	switch(nCmdId)
	{
	case eTESTFONT_Object_BTN_DrawCharCode:
		HLIB_CMD_Printf("[%s:%d] eTESTFONT_Object_BTN_DrawCharCode Clicked!\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTFONT_Object_RECT_DATA, local_testfont_FontFileTest);
		ONDK_GWM_APP_InvalidateObject (eTESTFONT_Object_RECT_DATA);
		break;
	case eTESTFONT_Object_BTN_DrawSingle:
		HLIB_CMD_Printf("[%s:%d] eTESTFONT_Object_BTN_DrawSingle Clicked!\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTFONT_Object_RECT_DATA, local_testfont_DrawSingleString);
		ONDK_GWM_APP_InvalidateObject (eTESTFONT_Object_RECT_DATA);
		break;

	case eTESTFONT_Object_BTN_DrawMulti1:
		HLIB_CMD_Printf("[%s:%d] eTESTFONT_Object_BTN_DrawMulti Clicked!\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTFONT_Object_RECT_DATA, local_testfont_DrawMultiString1);
		ONDK_GWM_APP_InvalidateObject (eTESTFONT_Object_RECT_DATA);
		break;

	case eTESTFONT_Object_BTN_DrawMulti2:
		HLIB_CMD_Printf("[%s:%d] eTESTFONT_Object_BTN_DrawMulti Clicked!\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTFONT_Object_RECT_DATA, local_testfont_DrawMultiString2);
		ONDK_GWM_APP_InvalidateObject (eTESTFONT_Object_RECT_DATA);
		break;

	case eTESTFONT_Object_BTN_DrawMulti3:
		HLIB_CMD_Printf("[%s:%d] eTESTFONT_Object_BTN_DrawMulti Clicked!\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTFONT_Object_RECT_DATA, local_testfont_DrawMultiString3);		
		
		ONDK_GWM_APP_InvalidateObject (eTESTFONT_Object_RECT_DATA);
		break;

	default:
		break;
	}

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testfont_MsgGwmDestroy(TESTFONT_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);


	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testfont_MsgGwmScrollRight(TESTFONT_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus (eTESTFONT_Object_BTN_DrawCharCode);

	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testfont_MsgGwmScrollLeft(TESTFONT_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus ((eTESTFONT_Object_BTN_MAX-1));

	return GWM_RET_OK;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t ONDK_TEST_FONT_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;
	TESTFONT_Contents_t 	*pstContents = local_testfont_GetContents();

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_testfont_MsgGwmCreate(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_KEYDOWN:
		eResult = local_testfont_MsgGwmKeyDown(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_CLICKED:
		eResult = local_testfont_MsgGwmClicked(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_DESTROY:
		eResult = local_testfont_MsgGwmDestroy(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLRIGHT:
		eResult = local_testfont_MsgGwmScrollRight(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLLEFT:
		eResult = local_testfont_MsgGwmScrollLeft(pstContents, p1, p2, p3);
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

void ONDK_TEST_FONT_Start(void)
{
	if(NULL == ONDK_GWM_APP_Get(ONDK_TEST_FONT_Proc))
	{
		HLIB_CMD_Printf("[%s:%d] Create \r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "ONDK_TEST_FONT_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, ONDK_TEST_FONT_Proc, 0, 0, 0, 0);
	}
	else
	{
		HLIB_CMD_Printf("[%s:%d] Destroy\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Destroy(ONDK_TEST_FONT_Proc);
	}
}



