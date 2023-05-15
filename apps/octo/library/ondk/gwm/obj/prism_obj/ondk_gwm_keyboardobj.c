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



/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <ondk.h>

#include <ondk_gwm_appobj.h>


#include "../../ondk_gwm_objdef.h"
#include "../../ondk_gwm_internal.h"


/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define			MAX_BUF_SIZE					128

#define			MAX_CHAR_SIZE					4
#define			MAX_NUM_SIZE					2

#define			MAX_KEYBOARD_OBJECT				128

#define			KEYBOARD_CHAR_BTN_COUNT			28
#define			KEYBOARD_NUM_BTN_COUNT			10

#define			FONT_NORMAL_SIZE				28
#define			FONT_BOLD_SIZE					34

#define			FONT_NORMAL_COLOR				0xffffffff
#define			FONT_FOCUS_COLOR				0xff000000
#define			FONT_HELP_COLOR					0xff2eb2ff

#define			KEYBOARD_ROW_COUNT				6
#define			KEYBOARD_COL_COUNT				10

#define			KEYBOARD_ROW_0					0
#define			KEYBOARD_ROW_1					1
#define			KEYBOARD_ROW_2					2
#define			KEYBOARD_ROW_3					3
#define			KEYBOARD_ROW_4					4
#define			KEYBOARD_ROW_5					5
#define			KEYBOARD_ROW_INPUTBOX			6

#define			KEYBOARD_COL_0					0
#define			KEYBOARD_COL_1					1
#define			KEYBOARD_COL_2					2
#define			KEYBOARD_COL_3					3
#define			KEYBOARD_COL_4					4
#define			KEYBOARD_COL_5					5
#define			KEYBOARD_COL_6					6
#define			KEYBOARD_COL_7					7
#define			KEYBOARD_COL_8					8
#define			KEYBOARD_COL_9					9

#define			KEYBOADR_BTN_X					60
#define			KEYBOADR_BTN_Y					298

#define			KEYBOADR_L_TEXT_X				173
#define			KEYBOADR_L_TEXT_Y				335

#define			KEYBOADR_N_TEXT_X				110
#define			KEYBOADR_N_TEXT_Y				335

#define			KEYBOADR_LANGUAGE_TEXT_X		74
#define			KEYBOADR_LANGUAGE_TEXT_Y		335


#define			KEYBOARD_BTN_WIDTH_GAP			117
#define			KEYBOARD_BTN_HEIGHT_GAP			65

#define			KEYBOARD_BTN_DIFFSIZE			30

#define			KEYBOADR_INPUTBOX3_X			60
#define			KEYBOADR_INPUTBOX3_Y			148

#define			KEYBOADR_HELPARROW3_X			1105
#define			KEYBOADR_HELPARROW3_Y			112

#define			KEYBOADR_HELPTEXT3_X			1180
#define			KEYBOADR_HELPTEXT3_Y			137

#define			KEYBOADR_INPUTBOX_HEIGHT_GAP	34

#define 		KEYBOARD_FRAME					"/usr/image/570_KEY_BG_1px.png"

#define			KEYBOARD_INPUTBOX_CURSOR		"/usr/image/570_KEY_Input_Cursor.png"

#define			KEYBOARD_INPUTBOX1_N			"/usr/image/572_KEY_Input01_N.png"
#define			KEYBOARD_INPUTBOX1_C			"/usr/image/572_KEY_Input01_C.png"

#define			KEYBOARD_INPUTBOX2_N			"/usr/image/572_KEY_Input02_N.png"
#define			KEYBOARD_INPUTBOX2_C			"/usr/image/572_KEY_Input02_C.png"

#define			KEYBOARD_INPUTBOX3_N			"/usr/image/572_KEY_Input03_N.png"
#define			KEYBOARD_INPUTBOX3_C			"/usr/image/572_KEY_Input03_C.png"

#define			KEYBOARD_BACKSPACE_N			"/usr/image/573_KEY_BTN_Backspace_N.png"
#define			KEYBOARD_BACKSPACE_C			"/usr/image/573_KEY_BTN_Backspace_C.png"

#define			KEYBOARD_CLEAR_N				"/usr/image/573_KEY_BTN_Clear_N.png"
#define			KEYBOARD_CLEAR_C				"/usr/image/573_KEY_BTN_Clear_C.png"

#define			KEYBOARD_SPACE_N				"/usr/image/573_KEY_BTN_Space_N.png"
#define			KEYBOARD_SPACE_C				"/usr/image/573_KEY_BTN_Space_C.png"

#define			KEYBOARD_TOGGLE_N				"/usr/image/592_SS_BTN_Change_N.png"
#define			KEYBOARD_TOGGLE_C				"/usr/image/592_SS_BTN_Change_C.png"

#define			KEYBOARD_ARROW					"/usr/image/571_KEY_Arrow.png"

#define			KEYBOARD_LONG_N					"/usr/image/573_KEY_BTN_224_N.png"
#define			KEYBOARD_LONG_C					"/usr/image/573_KEY_BTN_224_C.png"

#define			KEYBOARD_NORMAL_N				"/usr/image/573_KEY_BTN_107_N.png"
#define			KEYBOARD_NORMAL_C				"/usr/image/573_KEY_BTN_107_C.png"
#define			KEYBOARD_NORMAL_D				"/usr/image/573_KEY_BTN_107_D.png"



/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */
struct tagKeyboardObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.

	HINT32	curFocus_Row;
	HINT32	curFocus_Col;
	HINT32	inputboxSize;
	HINT32	isEnglishArea;

	HCHAR charSet[KEYBOARD_CHAR_BTN_COUNT][MAX_CHAR_SIZE];
	HCHAR numSet[KEYBOARD_NUM_BTN_COUNT][MAX_NUM_SIZE];
	HCHAR language[MAX_BUF_SIZE];
	HCHAR szLanguageBtn[MAX_BUF_SIZE];
	HBOOL bUpperCase;

	ONDKFontHandle	fontSmallHandle;
	ONDKFontHandle	fontBigHandle;

	ONDKImageHandle	hBackgroundImg;

	ONDKImageHandle	hHelpArrowImg;

	ONDKImageHandle hInputBox1_C_Img;
	ONDKImageHandle hInputBox1_N_Img;
	ONDKImageHandle hInputBox2_C_Img;
	ONDKImageHandle hInputBox2_N_Img;
	ONDKImageHandle hInputBox3_C_Img;
	ONDKImageHandle hInputBox3_N_Img;

	ONDKImageHandle	hBack_C_Img;
	ONDKImageHandle	hBack_N_Img;

	ONDKImageHandle	hClear_C_Img;
	ONDKImageHandle	hClear_N_Img;

	ONDKImageHandle	hSpace_C_Img;
	ONDKImageHandle	hSpace_N_Img;

	ONDKImageHandle	hLong_C_Img;
	ONDKImageHandle	hLong_N_Img;
	ONDKImageHandle	hNormal_C_Img;
	ONDKImageHandle	hNormal_N_Img;
	ONDKImageHandle	hNormal_D_Img;

};

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static struct tagKeyboardObject	s_defKeyboard;

/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */

static KEYBOARD_OBJECT		local_gwm_AllocNewKeyboardObj(void)
{
	struct tagKeyboardObject	*pObj = &s_defKeyboard;

	pObj = (struct tagKeyboardObject *)ONDK_Malloc(sizeof(struct tagKeyboardObject));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defKeyboard, sizeof(struct tagKeyboardObject));

	return pObj;
}

static void		local_gwm_FreeKeyboardObj(KEYBOARD_OBJECT obj)
{
	ONDK_Assert(obj);

	if(obj->fontSmallHandle)	ONDK_FONT_Release(obj->fontSmallHandle);
	if(obj->fontBigHandle)		ONDK_FONT_Release(obj->fontBigHandle);

	if(obj->hBackgroundImg)		ONDK_IMG_Destroy(obj->hBackgroundImg);

	if(obj->hHelpArrowImg)		ONDK_IMG_Destroy(obj->hHelpArrowImg);

	if(obj->hInputBox1_C_Img)	ONDK_IMG_Destroy(obj->hInputBox1_C_Img);
	if(obj->hInputBox1_N_Img)	ONDK_IMG_Destroy(obj->hInputBox1_N_Img);
	if(obj->hInputBox2_C_Img)	ONDK_IMG_Destroy(obj->hInputBox2_C_Img);
	if(obj->hInputBox2_N_Img)	ONDK_IMG_Destroy(obj->hInputBox2_N_Img);
	if(obj->hInputBox3_C_Img)	ONDK_IMG_Destroy(obj->hInputBox3_C_Img);
	if(obj->hInputBox3_N_Img)	ONDK_IMG_Destroy(obj->hInputBox3_N_Img);

	if(obj->hBack_C_Img)		ONDK_IMG_Destroy(obj->hBack_C_Img);
	if(obj->hBack_N_Img)		ONDK_IMG_Destroy(obj->hBack_N_Img);
	if(obj->hClear_C_Img)		ONDK_IMG_Destroy(obj->hClear_C_Img);
	if(obj->hClear_N_Img)		ONDK_IMG_Destroy(obj->hClear_N_Img);
	if(obj->hSpace_C_Img)		ONDK_IMG_Destroy(obj->hSpace_C_Img);
	if(obj->hSpace_N_Img)		ONDK_IMG_Destroy(obj->hSpace_N_Img);

	if(obj->hLong_C_Img)		ONDK_IMG_Destroy(obj->hLong_C_Img);
	if(obj->hLong_N_Img)		ONDK_IMG_Destroy(obj->hLong_N_Img);

	if(obj->hNormal_C_Img)		ONDK_IMG_Destroy(obj->hNormal_C_Img);
	if(obj->hNormal_N_Img)		ONDK_IMG_Destroy(obj->hNormal_N_Img);
	if(obj->hNormal_D_Img)		ONDK_IMG_Destroy(obj->hNormal_D_Img);

	ONDK_Free(obj);

	return;
}


static ONDK_Result_t		local_gwm_defaultDrawKeyboardMethod(ONDKWindowHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																	ONDK_GWM_GuiObject_t object, void *arg, int param1)
{
	ONDK_Rect_t 		drawRect;
	ONDK_Rect_t 		tmpRect;
	HINT32				i = 0, j = 0, k = 0, l = 0;
	HCHAR				*pszStr = NULL;

	KEYBOARD_OBJECT	keyboard = (KEYBOARD_OBJECT)object;

	ONDK_Assert(screen);
	ONDK_Assert(area);
	ONDK_Assert(rect);
	ONDK_Assert(object);

	drawRect = ONDK_Rect(area->x1 + rect->x, area->y1 + rect->y, rect->w, rect->h);
	tmpRect = ONDK_Rect(0,0,1280,720);
//	ONDK_IMG_Blit(keyboard->hBackgroundImg, screen, 0,0);
	ONDK_IMG_StretchBlit(keyboard->hBackgroundImg,screen,tmpRect);

	ONDK_FONT_DrawString(screen,keyboard->fontSmallHandle,keyboard->language,KEYBOADR_LANGUAGE_TEXT_X,KEYBOADR_LANGUAGE_TEXT_Y,FONT_NORMAL_COLOR,ONDKSTF_LEFT);

	//setting inputbox
	if(KEYBOARD_ROW_INPUTBOX==keyboard->curFocus_Row)
	{
		switch(keyboard->inputboxSize)
		{
			case 1:
				ONDK_IMG_Blit(keyboard->hHelpArrowImg, screen, KEYBOADR_HELPARROW3_X,KEYBOADR_HELPARROW3_Y+KEYBOADR_INPUTBOX_HEIGHT_GAP*2);
				ONDK_FONT_DrawString(screen,keyboard->fontSmallHandle,"Move",KEYBOADR_HELPTEXT3_X,KEYBOADR_HELPTEXT3_Y+KEYBOADR_INPUTBOX_HEIGHT_GAP*2,FONT_HELP_COLOR,ONDKSTF_CENTER);
				ONDK_IMG_Blit(keyboard->hInputBox1_C_Img, screen, KEYBOADR_INPUTBOX3_X,KEYBOADR_INPUTBOX3_Y+KEYBOADR_INPUTBOX_HEIGHT_GAP*2);
				break;
			case 2:
				ONDK_IMG_Blit(keyboard->hHelpArrowImg, screen, KEYBOADR_HELPARROW3_X,KEYBOADR_HELPARROW3_Y+KEYBOADR_INPUTBOX_HEIGHT_GAP);
				ONDK_FONT_DrawString(screen,keyboard->fontSmallHandle,"Move",KEYBOADR_HELPTEXT3_X,KEYBOADR_HELPTEXT3_Y+KEYBOADR_INPUTBOX_HEIGHT_GAP,FONT_HELP_COLOR,ONDKSTF_CENTER);
				ONDK_IMG_Blit(keyboard->hInputBox2_C_Img, screen, KEYBOADR_INPUTBOX3_X,KEYBOADR_INPUTBOX3_Y+KEYBOADR_INPUTBOX_HEIGHT_GAP);
				break;
			case 3:
				ONDK_IMG_Blit(keyboard->hHelpArrowImg, screen, KEYBOADR_HELPARROW3_X,KEYBOADR_HELPARROW3_Y);
				ONDK_FONT_DrawString(screen,keyboard->fontSmallHandle,"Move",KEYBOADR_HELPTEXT3_X,KEYBOADR_HELPTEXT3_Y,0xff2eb2ff,ONDKSTF_CENTER);
				ONDK_IMG_Blit(keyboard->hInputBox3_C_Img, screen, KEYBOADR_INPUTBOX3_X,KEYBOADR_INPUTBOX3_Y);
				break;
		}
	}

	else
	{
		switch(keyboard->inputboxSize)
		{
			case 1:
				ONDK_IMG_Blit(keyboard->hInputBox1_N_Img, screen, KEYBOADR_INPUTBOX3_X,KEYBOADR_INPUTBOX3_Y+KEYBOADR_INPUTBOX_HEIGHT_GAP*2);
				break;
			case 2:
				ONDK_IMG_Blit(keyboard->hInputBox2_N_Img, screen, KEYBOADR_INPUTBOX3_X,KEYBOADR_INPUTBOX3_Y+KEYBOADR_INPUTBOX_HEIGHT_GAP);
				break;
			case 3:
				ONDK_IMG_Blit(keyboard->hInputBox3_N_Img, screen, KEYBOADR_INPUTBOX3_X,KEYBOADR_INPUTBOX3_Y);
				break;
		}
	}


	k=0;
	l=0;
	for(i=0;i<KEYBOARD_ROW_COUNT;i++)
	{
		for(j=0;j<KEYBOARD_COL_COUNT;j++)
		{
			if(i==KEYBOARD_ROW_0)
			{
				j=KEYBOARD_COL_8;

				if(keyboard->isEnglishArea==TRUE)
					break;

				if(i==keyboard->curFocus_Row && j==keyboard->curFocus_Col)
				{
					ONDK_IMG_Blit(keyboard->hLong_C_Img, screen, KEYBOADR_BTN_X-KEYBOARD_BTN_DIFFSIZE+ KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_HEIGHT_GAP*i);
					ONDK_FONT_DrawString(screen,keyboard->fontBigHandle, keyboard->szLanguageBtn,KEYBOADR_L_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_L_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_FOCUS_COLOR,ONDKSTF_CENTER);
				}
				else
				{
					ONDK_IMG_Blit(keyboard->hLong_N_Img, screen, KEYBOADR_BTN_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y + KEYBOARD_BTN_HEIGHT_GAP*i);
					ONDK_FONT_DrawString(screen,keyboard->fontSmallHandle, keyboard->szLanguageBtn,KEYBOADR_L_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_L_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_NORMAL_COLOR,ONDKSTF_CENTER);
				}
				break;
			}

			if(i==KEYBOARD_ROW_1)
			{
				switch(j)
				{
					case KEYBOARD_COL_0:
						if (keyboard->isEnglishArea == TRUE)
						{
							pszStr = (keyboard->bUpperCase == TRUE) ? "abc" : "ABC" ;
						}
						else
						{
							pszStr = "a - A";
						}
						if(i==keyboard->curFocus_Row && j==keyboard->curFocus_Col)
						{
							ONDK_IMG_Blit(keyboard->hLong_C_Img, screen, KEYBOADR_BTN_X-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_HEIGHT_GAP*i);
							ONDK_FONT_DrawString(screen,keyboard->fontBigHandle, pszStr,KEYBOADR_L_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_L_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_FOCUS_COLOR,ONDKSTF_CENTER);
						}
						else
						{
							ONDK_IMG_Blit(keyboard->hLong_N_Img, screen, KEYBOADR_BTN_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y + KEYBOARD_BTN_HEIGHT_GAP*i);
							ONDK_FONT_DrawString(screen,keyboard->fontSmallHandle, pszStr,KEYBOADR_L_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_L_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_NORMAL_COLOR,ONDKSTF_CENTER);
						}
						break;
					case KEYBOARD_COL_2:
						if (keyboard->isEnglishArea == TRUE)
						{
							pszStr = "ENG - @";
						}
						else
						{
							pszStr = "a - @";
						}
						if(i==keyboard->curFocus_Row && j==keyboard->curFocus_Col)
						{
							ONDK_IMG_Blit(keyboard->hLong_C_Img, screen, KEYBOADR_BTN_X-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_HEIGHT_GAP*i);
							ONDK_FONT_DrawString(screen,keyboard->fontBigHandle, pszStr,KEYBOADR_L_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_L_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_FOCUS_COLOR,ONDKSTF_CENTER);
						}
						else
						{
							ONDK_IMG_Blit(keyboard->hLong_N_Img, screen, KEYBOADR_BTN_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y + KEYBOARD_BTN_HEIGHT_GAP*i);
							ONDK_FONT_DrawString(screen,keyboard->fontSmallHandle, pszStr,KEYBOADR_L_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_L_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_NORMAL_COLOR,ONDKSTF_CENTER);
						}
						break;
					case KEYBOARD_COL_4:
						if(i==keyboard->curFocus_Row && j==keyboard->curFocus_Col)
						{
							ONDK_IMG_Blit(keyboard->hBack_C_Img, screen, KEYBOADR_BTN_X-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_HEIGHT_GAP*i);
						}
						else
						{
							ONDK_IMG_Blit(keyboard->hBack_N_Img, screen, KEYBOADR_BTN_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y + KEYBOARD_BTN_HEIGHT_GAP*i);
						}
						break;
					case KEYBOARD_COL_6:
						if(i==keyboard->curFocus_Row && j==keyboard->curFocus_Col)
						{
							ONDK_IMG_Blit(keyboard->hClear_C_Img, screen, KEYBOADR_BTN_X-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_HEIGHT_GAP*i);
						}
						else
						{
							ONDK_IMG_Blit(keyboard->hClear_N_Img, screen, KEYBOADR_BTN_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y + KEYBOARD_BTN_HEIGHT_GAP*i);
						}
						break;
					case KEYBOARD_COL_8:
						if(i==keyboard->curFocus_Row && j==keyboard->curFocus_Col)
						{
							ONDK_IMG_Blit(keyboard->hLong_C_Img, screen, KEYBOADR_BTN_X-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_HEIGHT_GAP*i);
							ONDK_FONT_DrawString(screen,keyboard->fontBigHandle,"OK",KEYBOADR_L_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_L_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_FOCUS_COLOR,ONDKSTF_CENTER);
						}
						else
						{
							ONDK_IMG_Blit(keyboard->hLong_N_Img, screen, KEYBOADR_BTN_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y + KEYBOARD_BTN_HEIGHT_GAP*i);
							ONDK_FONT_DrawString(screen,keyboard->fontSmallHandle,"OK",KEYBOADR_L_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_L_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_NORMAL_COLOR,ONDKSTF_CENTER);
						}
						break;
				}
				j++;
				continue;
			}
			if(i==KEYBOARD_ROW_4 && j==KEYBOARD_COL_8)
			{
				if(i==keyboard->curFocus_Row && j==keyboard->curFocus_Col)
				{
					ONDK_IMG_Blit(keyboard->hSpace_C_Img, screen, KEYBOADR_BTN_X-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_HEIGHT_GAP*i);
				}
				else
				{
					ONDK_IMG_Blit(keyboard->hSpace_N_Img, screen, KEYBOADR_BTN_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y + KEYBOARD_BTN_HEIGHT_GAP*i);
				}
				j++;
				continue;
			}
			if(i==keyboard->curFocus_Row && j==keyboard->curFocus_Col)
			{
				ONDK_IMG_Blit(keyboard->hNormal_C_Img, screen, KEYBOADR_BTN_X-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y-KEYBOARD_BTN_DIFFSIZE + KEYBOARD_BTN_HEIGHT_GAP*i);
				if(i==KEYBOARD_ROW_5)
					ONDK_FONT_DrawString(screen,keyboard->fontBigHandle,keyboard->numSet[l++],KEYBOADR_N_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_N_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_FOCUS_COLOR,ONDKSTF_CENTER);
				else
					ONDK_FONT_DrawString(screen,keyboard->fontBigHandle,keyboard->charSet[k++],KEYBOADR_N_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_N_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_FOCUS_COLOR,ONDKSTF_CENTER);
			}
			else
			{
				if(HxSTD_StrCmp(keyboard->charSet[k]," ")==0)
					ONDK_IMG_Blit(keyboard->hNormal_D_Img, screen, KEYBOADR_BTN_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y + KEYBOARD_BTN_HEIGHT_GAP*i);
				else
					ONDK_IMG_Blit(keyboard->hNormal_N_Img, screen, KEYBOADR_BTN_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_BTN_Y + KEYBOARD_BTN_HEIGHT_GAP*i);
				if(i==KEYBOARD_COL_5)
					ONDK_FONT_DrawString(screen,keyboard->fontSmallHandle,keyboard->numSet[l++],KEYBOADR_N_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_N_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_NORMAL_COLOR,ONDKSTF_CENTER);
				else
					ONDK_FONT_DrawString(screen,keyboard->fontSmallHandle,keyboard->charSet[k++],KEYBOADR_N_TEXT_X + KEYBOARD_BTN_WIDTH_GAP*j, KEYBOADR_N_TEXT_Y + KEYBOARD_BTN_HEIGHT_GAP*i,FONT_NORMAL_COLOR,ONDKSTF_CENTER);
			}
		}
	}

/*
	s_keyboardwindow = ONDK_GWM_GetWindow();

    ONDK_WINDOW_SetShow( s_keyboardwindow, TRUE);
    ONDK_WINDOW_Update( s_keyboardwindow, NULL);
	*/
	NOT_USED_PARAM(param1);
	NOT_USED_PARAM(arg);

	return GWM_RET_OK;
}



static ONDK_Result_t	local_gwm_defaultKeyboardProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	KEYBOARD_OBJECT			keyboard;
	ONDK_Region_t			area;
	ONDK_Rect_t				rect;
	HBOOL					isFrontMost = FALSE;

//	ONDK_Assert(object->type == GUIOBJ_KEYBOARD);
	keyboard = (KEYBOARD_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwm_FreeKeyboardObj(keyboard);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			isFrontMost = (arg > 0) ? TRUE : FALSE;

			if (object->objDrawMethod != NULL)
			{
				object->objDrawMethod(ONDK_GWM_GetCurSurface(), &area, &rect, object, (KEYBOARD_OBJECT)keyboard, arg);
			}
			else
			{
				local_gwm_defaultDrawKeyboardMethod(ONDK_GWM_GetCurSurface(), &area, &rect, object, (KEYBOARD_OBJECT)keyboard, arg);
			}

			return GWM_MESSAGE_BREAK;
		}
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(keyboard->rect.x, keyboard->rect.y, keyboard->rect.w, keyboard->rect.h);
			VK_memcpy(&(keyboard->rect), new_rect, sizeof(ONDK_Region_t));
			ONDK_GWM_APP_InvalidateRelativeRect(keyboard->rect.x, keyboard->rect.y, keyboard->rect.w, keyboard->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_CHANGE_TEXT:
		{
			return GWM_MESSAGE_PASS;
		}
		/*
		case MT_KEYDOWN:
		{
			return GWM_MESSAGE_PASS;
		}
		case MT_KEYUP:
		{
			return GWM_MESSAGE_PASS;
		}
		*/
		case MT_GET_FOCUS:
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_GET, (Handle_t)NULL, object->ID, arg, (int)keyboard, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(keyboard->rect.x, keyboard->rect.y, keyboard->rect.w, keyboard->rect.h);
			break;

		case MT_LOST_FOCUS:
			keyboard->focus &= ~GUIOBJ_PUSHED;
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_LOSE, (Handle_t)NULL, object->ID, arg, (int)keyboard, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(keyboard->rect.x, keyboard->rect.y, keyboard->rect.w, keyboard->rect.h);
			break;
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}

void local_gwm_Keyboard_SetValues(HINT32 id)
{
	KEYBOARD_OBJECT obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_KEYBOARD)
	{
		return ;
	}

	obj->fontSmallHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold ,FONT_NORMAL_SIZE);
	obj->fontBigHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold,FONT_BOLD_SIZE);


	//bg
	if(obj->hBackgroundImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hBackgroundImg);
		obj->hBackgroundImg = NULL;
	}

	obj->hBackgroundImg = ONDK_IMG_CreateSelf(KEYBOARD_FRAME);

	//help arrow
	if(obj->hHelpArrowImg!= NULL)
	{
		ONDK_IMG_Destroy(obj->hHelpArrowImg);
		obj->hHelpArrowImg = NULL;
	}

	obj->hHelpArrowImg = ONDK_IMG_CreateSelf(KEYBOARD_ARROW);

	//input box1
	if(obj->hInputBox1_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hInputBox1_C_Img);
		obj->hInputBox1_C_Img = NULL;
	}

	obj->hInputBox1_C_Img = ONDK_IMG_CreateSelf(KEYBOARD_INPUTBOX1_C);

	if(obj->hInputBox1_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hInputBox1_N_Img);
		obj->hInputBox1_N_Img = NULL;
	}

	obj->hInputBox1_N_Img = ONDK_IMG_CreateSelf(KEYBOARD_INPUTBOX1_N);

	//input box2
	if(obj->hInputBox2_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hInputBox2_C_Img);
		obj->hInputBox2_C_Img = NULL;
	}

	obj->hInputBox2_C_Img = ONDK_IMG_CreateSelf(KEYBOARD_INPUTBOX2_C);

	if(obj->hInputBox2_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hInputBox2_N_Img);
		obj->hInputBox2_N_Img = NULL;
	}

	obj->hInputBox2_N_Img = ONDK_IMG_CreateSelf(KEYBOARD_INPUTBOX2_N);

	//input box3
	if(obj->hInputBox3_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hInputBox3_C_Img);
		obj->hInputBox3_C_Img = NULL;
	}

	obj->hInputBox3_C_Img = ONDK_IMG_CreateSelf(KEYBOARD_INPUTBOX3_C);

	if(obj->hInputBox3_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hInputBox3_N_Img);
		obj->hInputBox3_N_Img = NULL;
	}

	obj->hInputBox3_N_Img = ONDK_IMG_CreateSelf(KEYBOARD_INPUTBOX3_N);

	//backspace
	if(obj->hBack_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hBack_C_Img);
		obj->hBack_C_Img = NULL;
	}

	obj->hBack_C_Img = ONDK_IMG_CreateSelf(KEYBOARD_BACKSPACE_C);
	if(obj->hBack_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hBack_N_Img);
		obj->hBack_N_Img = NULL;
	}

	obj->hBack_N_Img = ONDK_IMG_CreateSelf(KEYBOARD_BACKSPACE_N);

	//clear
	if(obj->hClear_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hClear_C_Img);
		obj->hClear_C_Img = NULL;
	}

	obj->hClear_C_Img = ONDK_IMG_CreateSelf(KEYBOARD_CLEAR_C);
	if(obj->hClear_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hClear_N_Img);
		obj->hClear_N_Img = NULL;
	}

	obj->hClear_N_Img = ONDK_IMG_CreateSelf(KEYBOARD_CLEAR_N);

	//space
	if(obj->hSpace_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hSpace_C_Img);
		obj->hSpace_C_Img = NULL;
	}

	obj->hSpace_C_Img = ONDK_IMG_CreateSelf(KEYBOARD_SPACE_C);
	if(obj->hSpace_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hSpace_N_Img);
		obj->hSpace_N_Img = NULL;
	}

	obj->hSpace_N_Img = ONDK_IMG_CreateSelf(KEYBOARD_SPACE_N);

	//long
	if(obj->hLong_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hLong_C_Img);
		obj->hLong_C_Img = NULL;
	}

	obj->hLong_C_Img = ONDK_IMG_CreateSelf(KEYBOARD_LONG_C);
	if(obj->hLong_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hLong_N_Img);
		obj->hLong_N_Img = NULL;
	}

	obj->hLong_N_Img = ONDK_IMG_CreateSelf(KEYBOARD_LONG_N);

	//normal
	if(obj->hNormal_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hNormal_C_Img);
		obj->hNormal_C_Img = NULL;
	}

	obj->hNormal_C_Img = ONDK_IMG_CreateSelf(KEYBOARD_NORMAL_C);
	if(obj->hNormal_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hNormal_N_Img);
		obj->hNormal_N_Img = NULL;
	}

	obj->hNormal_N_Img = ONDK_IMG_CreateSelf(KEYBOARD_NORMAL_N);
	if(obj->hNormal_D_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hNormal_D_Img);
		obj->hNormal_D_Img = NULL;
	}

	obj->hNormal_D_Img = ONDK_IMG_CreateSelf(KEYBOARD_NORMAL_D);

}


/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

void			gwm_keyboard_InitClass(void)
{
	struct tagKeyboardObject	*pObj = &s_defKeyboard;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagKeyboardObject));

	pObj->ID = -1;
	pObj->type = GUIOBJ_KEYBOARD;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwm_defaultKeyboardProc;
	pObj->objDrawMethod = NULL;

	pObj->inputboxSize = 1;
	pObj->curFocus_Row = 2;
	pObj->curFocus_Col = 0;

}

ONDK_Result_t				ONDK_GWM_Keyboard_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 curFocus, HINT32 isEnglishArea, HCHAR *str)
{
	ONDK_GWM_GuiObject_t		objlist;
	KEYBOARD_OBJECT			new_keyboard;
	ONDK_Result_t			hresult;
	HINT32					strlength;

	ONDK_Assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_keyboard = (KEYBOARD_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_keyboard != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_keyboard = local_gwm_AllocNewKeyboardObj();
	if (new_keyboard == NULL)
	{
		ONDK_Error("[%s:%d] obj GWM_RET_OUT_OF_MEMORY\n", __FUNCTION__, __LINE__);
		return GWM_RET_OUT_OF_MEMORY;
	}

	new_keyboard->ID = id;
	new_keyboard->rect = *rect;
	new_keyboard->isEnglishArea = isEnglishArea;

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_keyboard);

	//ONDK_GWM_APP_InvalidateRelativeRect(new_keyboard->rect.x, new_keyboard->rect.y, new_keyboard->rect.w, new_keyboard->rect.h);
	local_gwm_Keyboard_SetValues(id);

	if (str != NULL)
	{
		strlength = HxSTD_StrLen(str);

		if(strlength < MAX_BUF_SIZE)
		{
			HxSTD_StrNCpy(new_keyboard->szLanguageBtn, str, strlength);
			new_keyboard->szLanguageBtn[strlength] = '\0';
		}
		else
		{
			HxSTD_StrNCpy(new_keyboard->szLanguageBtn, str, MAX_BUF_SIZE-1);
			new_keyboard->szLanguageBtn[MAX_BUF_SIZE-1] = '\0';
		}
	}

	new_keyboard->bUpperCase = FALSE;

	return hresult;
}


HINT32 ONDK_GWM_Keyboard_GetCurFocus_Row(HINT32 id)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_KEYBOARD)
	{
		return -1;
	}

	return obj->curFocus_Row;
}

HINT32 ONDK_GWM_Keyboard_GetCurFocus_Col(HINT32 id)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_KEYBOARD)
	{
		return -1;
	}

	return obj->curFocus_Col;
}



void ONDK_GWM_Keyboard_SetCurFocus_Row(HINT32 id, HINT32 cur)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_KEYBOARD)
	{
		return ;
	}

	obj->curFocus_Row= cur;
}

void ONDK_GWM_Keyboard_SetCurFocus_Col(HINT32 id, HINT32 cur)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_KEYBOARD)
	{
		return ;
	}

	obj->curFocus_Col= cur;
}


void ONDK_GWM_Keyboard_SetCharValue(HINT32 id, HINT32 pos, HCHAR *str)
{
	KEYBOARD_OBJECT		obj;
	HINT32 strlength;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_KEYBOARD)
	{
		return ;
	}

	if(str != NULL)
	{
		strlength = HxSTD_StrLen(str);

		if(strlength < MAX_CHAR_SIZE)
		{
			HxSTD_StrNCpy(obj->charSet[pos], str, strlength);
			obj->charSet[pos][strlength] = '\0';
		}
	}
}

void ONDK_GWM_Keyboard_SetNumValue(HINT32 id, HINT32 pos, HCHAR *str)
{
	KEYBOARD_OBJECT		obj;
	HINT32 strlength;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_KEYBOARD)
	{
		return ;
	}


		if(str != NULL)
		{
			strlength = HxSTD_StrLen(str);

			if(strlength < MAX_NUM_SIZE)
			{
				HxSTD_StrNCpy(obj->numSet[pos],str,strlength);
				obj->numSet[pos][strlength] = '\0';
			}
		}
}

void ONDK_GWM_Keyboard_SetInputBoxSize(HINT32 id, HINT32 size)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_KEYBOARD)
	{
		return ;
	}
	obj->inputboxSize = size;
}

void ONDK_GWM_Keyboard_SetLanguage(HINT32 id, HCHAR *str)
{
	KEYBOARD_OBJECT		obj;
	HINT32 strlength;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_KEYBOARD)
	{
		return ;
	}

	if(str != NULL)
	{
		strlength = HxSTD_StrLen(str);

		if(strlength < MAX_BUF_SIZE)
		{
			HxSTD_StrNCpy(obj->language,str,strlength);
			obj->language[strlength] = '\0';
		}
	}
}


void 	ONDK_GWM_Keyboard_SetUppercaseAndLowercaseLetters(HINT32 id, HBOOL bUpperCase)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return ;
	}

	obj->bUpperCase = bUpperCase;
}


/* End of File. ---------------------------------------------------------- */


