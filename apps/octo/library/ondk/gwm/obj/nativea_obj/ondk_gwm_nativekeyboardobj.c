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
#define	ONDK_NKEYBOARD_MAX_BUF_SIZE				128
#define	ONDK_NKEYBOARD_BUF_SIZE_32				32

#define	ONDK_NKEYBOARD_MAX_CHAR_SIZE			4
#define	ONDK_NKEYBOARD_MAX_NUM_SIZE				2

//#define	ONDK_NKEYBOARD_MAX_KEYBOARD_OBJECT		128

#define	ONDK_NKEYBOARD_CHAR_BTN_COUNT			28
#define	ONDK_NKEYBOARD_NUM_BTN_COUNT			10

#define	ONDK_NKEYBOARD_FONT_SIZE_NORMAL			22
#define	ONDK_NKEYBOARD_FONT_SIZE_BOLD			24

#define	ONDK_NKEYBOARD_ROW_COUNT				5
#define	ONDK_NKEYBOARD_COL_COUNT				12

#define	ONDK_NKEYBOARD_ROW_0					0
#define	ONDK_NKEYBOARD_ROW_1					1
#define	ONDK_NKEYBOARD_ROW_2					2
#define	ONDK_NKEYBOARD_ROW_3					3
#define	ONDK_NKEYBOARD_ROW_4					4
#define	ONDK_NKEYBOARD_ROW_INPUTBOX				5
#define	ONDK_NKEYBOARD_ROW_KEY_START			ONDK_NKEYBOARD_ROW_1

#define	ONDK_NKEYBOARD_COL_0					0
#define	ONDK_NKEYBOARD_COL_1					1
#define	ONDK_NKEYBOARD_COL_2					2
#define	ONDK_NKEYBOARD_COL_3					3
#define	ONDK_NKEYBOARD_COL_4					4
#define	ONDK_NKEYBOARD_COL_5					5
#define	ONDK_NKEYBOARD_COL_6					6
#define	ONDK_NKEYBOARD_COL_7					7
#define	ONDK_NKEYBOARD_COL_8					8
#define	ONDK_NKEYBOARD_COL_9					9
#define	ONDK_NKEYBOARD_COL_10					10
#define	ONDK_NKEYBOARD_COL_11					11

#define	ONDK_NKEYBOARD_COL_ACTON_BTN			ONDK_NKEYBOARD_COL_10

// Numaric Key
#define	ONDK_NKEYBOARD_ROW_NUMERIC				ONDK_NKEYBOARD_ROW_4

// Add Space
#define	ONDK_NKEYBOARD_ROW_SPACE				ONDK_NKEYBOARD_ROW_3
#define	ONDK_NKEYBOARD_COL_SPACE_0				ONDK_NKEYBOARD_COL_8
#define	ONDK_NKEYBOARD_COL_SPACE_1				ONDK_NKEYBOARD_COL_9

// Uppercase and lowercase letters
#define	ONDK_NKEYBOARD_ROW_ULCASE				ONDK_NKEYBOARD_ROW_1
#define	ONDK_NKEYBOARD_COL_ULCASE				ONDK_NKEYBOARD_COL_10

// Special character
#define	ONDK_NKEYBOARD_ROW_SPCHAR				ONDK_NKEYBOARD_ROW_1
#define	ONDK_NKEYBOARD_COL_SPCHAR				ONDK_NKEYBOARD_COL_11

// Delete
#define	ONDK_NKEYBOARD_ROW_DELETE				ONDK_NKEYBOARD_ROW_2
#define	ONDK_NKEYBOARD_COL_DELETE_0				ONDK_NKEYBOARD_COL_10
#define	ONDK_NKEYBOARD_COL_DELETE_1				ONDK_NKEYBOARD_COL_11

// Delete All
#define	ONDK_NKEYBOARD_ROW_DELETE_ALL			ONDK_NKEYBOARD_ROW_3
#define	ONDK_NKEYBOARD_COL_DELETE_ALL_0			ONDK_NKEYBOARD_COL_10
#define	ONDK_NKEYBOARD_COL_DELETE_ALL_1			ONDK_NKEYBOARD_COL_11

// Language Btn
#define	ONDK_NKEYBOARD_ROW_LANGUAGE				ONDK_NKEYBOARD_ROW_4
#define	ONDK_NKEYBOARD_COL_LANGUAGE_0			ONDK_NKEYBOARD_COL_10
#define	ONDK_NKEYBOARD_COL_LANGUAGE_1			ONDK_NKEYBOARD_COL_11

#define	ONDK_NKEYBOARD_ROW_INPUTBOX_LINE_MG		6
#define	ONDK_NKEYBOARD_BTN_MG_6					6
#define	ONDK_NKEYBOARD_BTN_MG_14				14
#define	ONDK_NKEYBOARD_BTN_TEXT_Y_MG			8
#define	ONDK_NKEYBOARD_BTN_ACTION_STR			"BTN"

#define	ONDK_KEYBOARD_COLOR_G_WHITE_50			0x80ffffff	//C_G_Whistle_50,
#define	ONDK_KEYBOARD_COLOR_G_BLUE_01_100		0xff2eb2ff	//C_G_Base01_100,
#define	ONDK_KEYBOARD_COLOR_G_GREY_04			0xff0f0f0f	//C_G_Gong04,


/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */
struct tagKeyboardObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.

	HINT32		lCurFocus_Row;
	HINT32		lCurFocus_Col;
	HINT32		lInputboxSize;
	HINT32		lIsEnglishArea;

	HCHAR		aucCharSet[ONDK_NKEYBOARD_CHAR_BTN_COUNT][ONDK_NKEYBOARD_MAX_CHAR_SIZE];
	HCHAR		aucNumSet[ONDK_NKEYBOARD_NUM_BTN_COUNT][ONDK_NKEYBOARD_MAX_NUM_SIZE];
	HCHAR		szLanguage[ONDK_NKEYBOARD_MAX_BUF_SIZE];
	HCHAR		szLanguageBtn[ONDK_NKEYBOARD_MAX_BUF_SIZE];
	HCHAR		szEnter[ONDK_NKEYBOARD_BUF_SIZE_32];
	HBOOL		bUpperCase;

	ONDK_Rect_t		stInputBoxRect;
	ONDK_Rect_t		stBtnKeyStartRect;		// Key Button Info
	ONDK_Rect_t		stBtnActStartRect;		// Action Button Info

	ONDK_Color_t	ulBNColor;				// BTN Normal color
	ONDK_Color_t	ulTNColor;				// BTN TEXT Normal color

	ONDK_Color_t 	ulBCColor;				// BTN Cursor colodr
	ONDK_Color_t 	ulTCColor;				// BTN TEXT Cursor colodr

	ONDKFontHandle	fontSmallHandle;
	ONDKFontHandle	fontBigHandle;

	HCHAR			*szimgPathBackC;
	ONDKImageHandle	hBack_C_Img;
	HCHAR			*szimgPathBackN;
	ONDKImageHandle	hBack_N_Img;

	HCHAR			*szimgPathClearC;
	ONDKImageHandle	hClear_C_Img;
	HCHAR			*szimgPathClearN;
	ONDKImageHandle	hClear_N_Img;

	HCHAR			*szimgPathSpaceC;
	ONDKImageHandle	hSpace_C_Img;
	HCHAR			*szimgPathSpaceN;
	ONDKImageHandle	hSpace_N_Img;

};

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
static struct tagKeyboardObject	s_defKeyboard;

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */

static KEYBOARD_OBJECT		local_gwm_Native_AllocNewKeyboardObj(void)
{
	struct tagKeyboardObject	*pObj = &s_defKeyboard;

	pObj = (struct tagKeyboardObject *)ONDK_Malloc(sizeof(struct tagKeyboardObject));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defKeyboard, sizeof(struct tagKeyboardObject));

	return pObj;
}


static void		local_gwm_Native_FreeKeyboardObj(KEYBOARD_OBJECT obj)
{
	ONDK_Assert(obj);

	if(obj->fontSmallHandle)
		ONDK_FONT_Release(obj->fontSmallHandle);
	if(obj->fontBigHandle)
		ONDK_FONT_Release(obj->fontBigHandle);

	if(obj->hBack_C_Img)
		ONDK_IMG_Destroy(obj->hBack_C_Img);
	if(obj->hBack_N_Img)
		ONDK_IMG_Destroy(obj->hBack_N_Img);
	if(obj->hClear_C_Img)
		ONDK_IMG_Destroy(obj->hClear_C_Img);
	if(obj->hClear_N_Img)
		ONDK_IMG_Destroy(obj->hClear_N_Img);
	if(obj->hSpace_C_Img)
		ONDK_IMG_Destroy(obj->hSpace_C_Img);
	if(obj->hSpace_N_Img)
		ONDK_IMG_Destroy(obj->hSpace_N_Img);

	ONDK_Free(obj);
}


static ONDK_Result_t		local_gwm_Native_defaultDrawKeyboardMethod(ONDKWindowHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																	ONDK_GWM_GuiObject_t object, void *arg, int param1)
{
	HINT32				i = 0, j = 0, k = 0, l = 0;
	HINT32				x = 0, y = 0, lIndex = 0;
	HCHAR				*pszStr = NULL;
	HBOOL				bShowIcon = FALSE, bFocusBtn = FALSE;
	ONDKImageHandle		hImg = NULL;
	ONDK_Color_t		ulCursorColor = 0, ulTextColor = 0;
	ONDK_Size_t			stImgSize = {0, 0};
	ONDK_Rect_t 		drawRect, stRect, stRectKeyBtn, stRectActionBtn;
	KEYBOARD_OBJECT		keyboard = (KEYBOARD_OBJECT)object;

	ONDK_Assert(screen);
	ONDK_Assert(area);
	ONDK_Assert(rect);
	ONDK_Assert(object);

	drawRect = ONDK_Rect(area->x1 + rect->x, area->y1 + rect->y, rect->w, rect->h);

	/* Draw Input BOX */
	stRect = ONDK_Rect(drawRect.x + keyboard->stInputBoxRect.x, drawRect.y + keyboard->stInputBoxRect.y,
							keyboard->stInputBoxRect.w, keyboard->stInputBoxRect.h);
	if (ONDK_NKEYBOARD_ROW_INPUTBOX == keyboard->lCurFocus_Row)
	{
		// - Cursor
		ONDK_DRAW_StrokeRect(screen, stRect.x, stRect.y, stRect.x + stRect.w, stRect.y + stRect.h, ONDK_NKEYBOARD_ROW_INPUTBOX_LINE_MG, ONDK_KEYBOARD_COLOR_G_BLUE_01_100);
	}
	else
	{
		// - Normal
		ONDK_DRAW_StrokeRect(screen, stRect.x, stRect.y, stRect.x + stRect.w, stRect.y + stRect.h, ONDK_NKEYBOARD_ROW_INPUTBOX_LINE_MG, ONDK_KEYBOARD_COLOR_G_WHITE_50);
	}

	/* Draw Keyboard */
	stRectActionBtn = ONDK_Rect(drawRect.x + keyboard->stBtnActStartRect.x, drawRect.y + keyboard->stBtnActStartRect.y,
						keyboard->stBtnActStartRect.w, keyboard->stBtnActStartRect.h);
	stRectKeyBtn = ONDK_Rect(drawRect.x + keyboard->stBtnKeyStartRect.x, drawRect.y + keyboard->stBtnKeyStartRect.y,
						keyboard->stBtnKeyStartRect.w, keyboard->stBtnKeyStartRect.h);

	k = 0;
	l = 0;
	for (i = 0; i < ONDK_NKEYBOARD_ROW_COUNT; i++)
	{
		for (j = 0; j < ONDK_NKEYBOARD_COL_COUNT; j++)
		{
			/* Enter BTN*/
			if (i == ONDK_NKEYBOARD_ROW_0)
			{
				j = ONDK_NKEYBOARD_COL_ACTON_BTN;	/* Enter Opsition */

				if (keyboard->lIsEnglishArea == TRUE)
					break;

				if ((i == keyboard->lCurFocus_Row) && (j == keyboard->lCurFocus_Col))
				{
					ulCursorColor = keyboard->ulBCColor;
					ulTextColor = keyboard->ulTCColor;
					bFocusBtn = TRUE;
				}
				else
				{
					ulCursorColor = keyboard->ulBNColor;
					ulTextColor = keyboard->ulTNColor;
					bFocusBtn = FALSE;
				}

				ONDK_DRAW_FillRect(screen, stRectActionBtn, ulCursorColor);
				ONDK_FONT_DrawStringAlign(screen, keyboard->fontSmallHandle, keyboard->szEnter, stRectActionBtn.x,
											stRectActionBtn.y + ONDK_NKEYBOARD_BTN_TEXT_Y_MG, stRectActionBtn.w, ulTextColor, ONDKSTF_CENTER);
				break;
			}

			/* KeyBoard */
			if ((i == keyboard->lCurFocus_Row) && (j == keyboard->lCurFocus_Col))
			{
				// Focus Color
				ulCursorColor = keyboard->ulBCColor;
				ulTextColor = keyboard->ulTCColor;
				bFocusBtn = TRUE;
			}
			else
			{
				// Normal Color
				ulCursorColor = keyboard->ulBNColor;
				ulTextColor = keyboard->ulTNColor;
				bFocusBtn = FALSE;
			}

			// Set Key String
			if (j < ONDK_NKEYBOARD_COL_ACTON_BTN)
			{
				// Numeric String
				if (i == ONDK_NKEYBOARD_ROW_NUMERIC)
				{
					pszStr = keyboard->aucNumSet[l++];
				}
				else
				{
					pszStr = keyboard->aucCharSet[k++];
				}
			}
			else // Action Button
			{
				pszStr = (HCHAR*)ONDK_NKEYBOARD_BTN_ACTION_STR;
			}

			/* Find Emtpy BTN */
			if (HxSTD_StrCmp(pszStr, (HCHAR*)" ") == 0)
			{
				// Change BTN Color
				ulCursorColor = ONDK_KEYBOARD_COLOR_G_GREY_04;
			}

			/* Set Button Position */
			if (j == ONDK_NKEYBOARD_COL_0)
			{
				// BTN Start Positon
				ONDK_CopyRect(&stRect, &stRectKeyBtn);
				lIndex = (i - ONDK_NKEYBOARD_ROW_KEY_START);
				stRect.y = stRectKeyBtn.y + (lIndex * stRectKeyBtn.h) + (lIndex * ONDK_NKEYBOARD_BTN_MG_6);
			}
			else
			{
				// BTN Next Position
				stRect.x = stRectKeyBtn.x + (j * stRectKeyBtn.w) + (j * ONDK_NKEYBOARD_BTN_MG_6);
			}

			/* Action BTN */
			bShowIcon = FALSE;
			if ((i == ONDK_NKEYBOARD_ROW_SPACE) && ((j == ONDK_NKEYBOARD_COL_SPACE_0) || (j == ONDK_NKEYBOARD_COL_SPACE_1)))
			{
				// Space BTN (|____|)
				if (j == ONDK_NKEYBOARD_COL_SPACE_0)
				{
					stRect.w = stRectActionBtn.w;
					bShowIcon = TRUE;
					hImg = (bFocusBtn == TRUE) ? keyboard->hSpace_C_Img : keyboard->hSpace_N_Img ;
				}
				else if (j == ONDK_NKEYBOARD_COL_SPACE_1)
				{
					continue;
				}
			}
			else if ((i == ONDK_NKEYBOARD_ROW_ULCASE) && (j == ONDK_NKEYBOARD_COL_ULCASE))	// a/A
			{
				// Uppercase and lowercase letters (a/A)
				stRect.x += ONDK_NKEYBOARD_BTN_MG_14;
				pszStr = "a/A";
			}
			else if ((i == ONDK_NKEYBOARD_ROW_SPCHAR) && (j == ONDK_NKEYBOARD_COL_SPCHAR))	// a/@
			{
				// Special character (a/@)
				stRect.x += ONDK_NKEYBOARD_BTN_MG_14;
				pszStr = "a/@";
			}
			else if ((i == ONDK_NKEYBOARD_ROW_DELETE) && ((j == ONDK_NKEYBOARD_COL_DELETE_0) || (j == ONDK_NKEYBOARD_COL_DELETE_1)))	// Delete(<-)
			{
				// Delete (<-)
				stRect.x += ONDK_NKEYBOARD_BTN_MG_14;
				if (j == ONDK_NKEYBOARD_COL_DELETE_0)
				{
					stRect.w = stRectActionBtn.w;
					bShowIcon = TRUE;
					hImg = (bFocusBtn == TRUE) ? keyboard->hBack_C_Img: keyboard->hBack_N_Img ;
				}
				else if (j == ONDK_NKEYBOARD_COL_DELETE_1)
				{
					continue;
				}
			}
			else if ((i == ONDK_NKEYBOARD_ROW_DELETE_ALL) && ((j == ONDK_NKEYBOARD_COL_DELETE_ALL_0) || (j == ONDK_NKEYBOARD_COL_DELETE_ALL_1)))	// Delete All(X)
			{
				// Delete ALL (X)
				stRect.x += ONDK_NKEYBOARD_BTN_MG_14;
				if (j == ONDK_NKEYBOARD_COL_DELETE_ALL_0)
				{
					stRect.w = stRectActionBtn.w;
					bShowIcon = TRUE;
					hImg = (bFocusBtn == TRUE) ? keyboard->hClear_C_Img: keyboard->hClear_N_Img ;
				}
				else if (j == ONDK_NKEYBOARD_COL_DELETE_ALL_1)
				{
					continue;
				}
			}
			else if ((i == ONDK_NKEYBOARD_ROW_LANGUAGE) && ((j == ONDK_NKEYBOARD_COL_LANGUAGE_0) || (j == ONDK_NKEYBOARD_COL_LANGUAGE_1)))	// Lanaguage
			{
				// Language
				stRect.x += ONDK_NKEYBOARD_BTN_MG_14;
				if (j == ONDK_NKEYBOARD_COL_LANGUAGE_0)
				{
					stRect.w = stRectActionBtn.w;
					pszStr = keyboard->szLanguage;
				}
				else if (j == ONDK_NKEYBOARD_COL_LANGUAGE_1)
				{
					continue;
				}
			}
			else
			{
				// Display normal BTN
			}

			/* Draw BTN - Button - A1 */
			ONDK_DRAW_FillRect(screen, stRect, ulCursorColor);

			/* Draw Image & Text */
			if (bShowIcon == TRUE)
			{
				if (hImg)
				{
					// Center Align
					x = y = 0;
					stImgSize = ONDK_IMG_GetSize(hImg);
					if (stRect.w > stImgSize.w)
					{
						x = (stRect.w - stImgSize.w) / 2;
					}
					if (stRect.h > stImgSize.h)
					{
						y = (stRect.h - stImgSize.h) / 2;
					}
					ONDK_IMG_Blit(hImg, screen, stRect.x + x, stRect.y + y);
				}
			}
			else
			{
				ONDK_FONT_DrawStringAlign(screen, keyboard->fontSmallHandle, pszStr, stRect.x,
											stRect.y + ONDK_NKEYBOARD_BTN_TEXT_Y_MG, stRect.w, ulTextColor, ONDKSTF_CENTER);
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



static ONDK_Result_t	local_gwm_Native_defaultKeyboardProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
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
			local_gwm_Native_FreeKeyboardObj(keyboard);
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
				local_gwm_Native_defaultDrawKeyboardMethod(ONDK_GWM_GetCurSurface(), &area, &rect, object, (KEYBOARD_OBJECT)keyboard, arg);
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


static void		local_gwm_Native_Keyboard_SetValues(HINT32 id)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	obj->fontSmallHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_NKEYBOARD_FONT_SIZE_NORMAL);
	obj->fontBigHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_NKEYBOARD_FONT_SIZE_BOLD);
}


/* ======================================================================== */
/* Exported Functions. 														*/
/* ======================================================================== */

void	INT_ONDK_GWM_COM_Keyboard_Init(void)
{
	struct tagKeyboardObject	*pObj = &s_defKeyboard;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagKeyboardObject));

	pObj->ID = -1;
	pObj->type = GUIOBJ_KEYBOARD;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwm_Native_defaultKeyboardProc;
	pObj->objDrawMethod = NULL;

	pObj->lInputboxSize = 1;
	pObj->lCurFocus_Row = 2;
	pObj->lCurFocus_Col = 0;

	pObj->szimgPathBackC = NULL;
	pObj->hBack_C_Img = NULL;
	pObj->szimgPathBackN = NULL;
	pObj->hBack_N_Img = NULL;

	pObj->szimgPathClearC = NULL;
	pObj->hClear_C_Img = NULL;
	pObj->szimgPathClearN = NULL;
	pObj->hClear_N_Img = NULL;

	pObj->szimgPathSpaceC = NULL;
	pObj->hSpace_C_Img = NULL;
	pObj->szimgPathSpaceN = NULL;
	pObj->hSpace_N_Img = NULL;
}


ONDK_Result_t		ONDK_GWM_COM_Keyboard_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 curFocus, HINT32 lIsEnglishArea, HCHAR *str)
{
	HINT32					strlength;
	ONDK_Result_t			hresult;
	KEYBOARD_OBJECT			new_keyboard;
	ONDK_GWM_GuiObject_t	objlist;


	ONDK_Assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_keyboard = (KEYBOARD_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_keyboard != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_keyboard = local_gwm_Native_AllocNewKeyboardObj();
	if (new_keyboard == NULL)
	{
		ONDK_Error("[%s:%d] obj GWM_RET_OUT_OF_MEMORY\n", __FUNCTION__, __LINE__);
		return GWM_RET_OUT_OF_MEMORY;
	}

	new_keyboard->ID = id;
	new_keyboard->rect = *rect;
	new_keyboard->lIsEnglishArea = lIsEnglishArea;

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_keyboard);
	local_gwm_Native_Keyboard_SetValues(id);

	if (str != NULL)
	{
		strlength = HxSTD_StrLen(str);

		if (strlength < ONDK_NKEYBOARD_MAX_BUF_SIZE)
		{
			HxSTD_StrNCpy(new_keyboard->szLanguageBtn, str, strlength);
			new_keyboard->szLanguageBtn[strlength] = '\0';
		}
		else
		{
			HxSTD_StrNCpy(new_keyboard->szLanguageBtn, str, ONDK_NKEYBOARD_MAX_BUF_SIZE-1);
			new_keyboard->szLanguageBtn[ONDK_NKEYBOARD_MAX_BUF_SIZE-1] = '\0';
		}
	}

	new_keyboard->bUpperCase = FALSE;

	return hresult;
}


HINT32	ONDK_GWM_COM_Keyboard_GetCurFocus_Row(HINT32 id)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return -1;
	}

	return obj->lCurFocus_Row;
}


HINT32	ONDK_GWM_COM_Keyboard_GetCurFocus_Col(HINT32 id)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return -1;
	}

	return obj->lCurFocus_Col;
}


void	ONDK_GWM_COM_Keyboard_SetCurFocus_Row(HINT32 id, HINT32 cur)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	obj->lCurFocus_Row = cur;
}


void	ONDK_GWM_COM_Keyboard_SetCurFocus_Col(HINT32 id, HINT32 cur)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	obj->lCurFocus_Col= cur;
}


void	ONDK_GWM_COM_Keyboard_SetCharValue(HINT32 id, HINT32 pos, HCHAR *str)
{
	HINT32				strlength = 0;
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	if (str != NULL)
	{
		strlength = HxSTD_StrLen(str);
		if (strlength < ONDK_NKEYBOARD_MAX_CHAR_SIZE)
		{
			HxSTD_StrNCpy(obj->aucCharSet[pos], str, strlength);
			obj->aucCharSet[pos][strlength] = '\0';
		}
	}
}


void	ONDK_GWM_COM_Keyboard_SetNumValue(HINT32 id, HINT32 pos, HCHAR *str)
{
	HINT32 				strlength = 0;
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	if (str != NULL)
	{
		strlength = HxSTD_StrLen(str);
		if (strlength < ONDK_NKEYBOARD_MAX_NUM_SIZE)
		{
			HxSTD_StrNCpy(obj->aucNumSet[pos],str,strlength);
			obj->aucNumSet[pos][strlength] = '\0';
		}
	}
}


void	ONDK_GWM_COM_Keyboard_SetInputBoxSize(HINT32 id, HINT32 size)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	obj->lInputboxSize = size;
}


void	ONDK_GWM_COM_Keyboard_SetInputBoxRect(HINT32 id, ONDK_Rect_t *stRect)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	ONDK_CopyRect(&obj->stInputBoxRect, stRect);
}


void	ONDK_GWM_COM_Keyboard_SetBtnKeyRect(HINT32 id, ONDK_Rect_t *stRect)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	ONDK_CopyRect(&obj->stBtnKeyStartRect, stRect);
}


void	ONDK_GWM_COM_Keyboard_SetBtnActRect(HINT32 id, ONDK_Rect_t *stRect)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	ONDK_CopyRect(&obj->stBtnActStartRect, stRect);
}


void	ONDK_GWM_COM_Keyboard_SetBtnChangeColor(HINT32 id, ONDK_Color_t ulBNColor, ONDK_Color_t ulTNColor, ONDK_Color_t ulBCColor, ONDK_Color_t ulTCColor)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	obj->ulBNColor = ulBNColor;
	obj->ulTNColor = ulTNColor;

	obj->ulBCColor = ulBCColor;
	obj->ulTCColor = ulTCColor;
}


void	ONDK_GWM_COM_Keyboard_SetLanguage(HINT32 id, HCHAR *str)
{
	KEYBOARD_OBJECT		obj;
	HINT32 strlength;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	if(str != NULL)
	{
		strlength = HxSTD_StrLen(str);
		if (strlength < ONDK_NKEYBOARD_MAX_BUF_SIZE)
		{
			HxSTD_StrNCpy(obj->szLanguage, str, strlength);
			obj->szLanguage[strlength] = '\0';
		}
	}
}


void	ONDK_GWM_COM_Keyboard_SetEnterBtnStr(HINT32 id, HCHAR *str)
{
	KEYBOARD_OBJECT		obj;
	HINT32 strlength;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	if (str != NULL)
	{
		strlength = HxSTD_StrLen(str);
		if (strlength < ONDK_NKEYBOARD_BUF_SIZE_32)
		{
			HxSTD_StrNCpy(obj->szEnter, str, strlength);
			obj->szEnter[strlength] = '\0';
		}
	}
}


void 	ONDK_GWM_COM_Keyboard_SetUppercaseAndLowercaseLetters(HINT32 id, HBOOL bUpperCase)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	obj->bUpperCase = bUpperCase;
}


void	ONDK_GWM_COM_Keyboard_SetImageBack(HINT32 id, HCHAR *imgPathC, HCHAR *imgPathN)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	if (obj->hBack_C_Img != NULL)
	{
		ONDK_IMG_Destroy(obj->hBack_C_Img);
		obj->hBack_C_Img = NULL;
	}

	obj->szimgPathBackC = imgPathC;
	obj->hBack_C_Img = ONDK_IMG_CreateSelf((HUINT8*)imgPathC);

	if (obj->hBack_N_Img != NULL)
	{
		ONDK_IMG_Destroy(obj->hBack_N_Img);
		obj->hBack_N_Img = NULL;
	}

	obj->szimgPathBackN = imgPathN;
	obj->hBack_N_Img = ONDK_IMG_CreateSelf((HUINT8*)imgPathN);
}


void	ONDK_GWM_COM_Keyboard_SetImageClear(HINT32 id, HCHAR *imgPathC, HCHAR *imgPathN)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	if (obj->hClear_C_Img != NULL)
	{
		ONDK_IMG_Destroy(obj->hClear_C_Img);
		obj->hClear_C_Img = NULL;
	}

	obj->szimgPathClearC = imgPathC;
	obj->hClear_C_Img = ONDK_IMG_CreateSelf((HUINT8*)imgPathC);

	if (obj->hClear_N_Img != NULL)
	{
		ONDK_IMG_Destroy(obj->hClear_N_Img);
		obj->hClear_N_Img = NULL;
	}

	obj->szimgPathClearN = imgPathN;
	obj->hClear_N_Img = ONDK_IMG_CreateSelf((HUINT8*)imgPathN);
}


void	ONDK_GWM_COM_Keyboard_SetImageSpace(HINT32 id, HCHAR *imgPathC, HCHAR *imgPathN)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_KEYBOARD))
	{
		return;
	}

	if (obj->hSpace_C_Img != NULL)
	{
		ONDK_IMG_Destroy(obj->hSpace_C_Img);
		obj->hSpace_C_Img = NULL;
	}

	obj->szimgPathSpaceC = imgPathC;
	obj->hSpace_C_Img = ONDK_IMG_CreateSelf((HUINT8*)imgPathC);

	if (obj->hSpace_N_Img != NULL)
	{
		ONDK_IMG_Destroy(obj->hSpace_N_Img);
		obj->hSpace_N_Img = NULL;
	}

	obj->szimgPathSpaceN = imgPathN;
	obj->hSpace_N_Img = ONDK_IMG_CreateSelf((HUINT8*)imgPathN);
}


/* End of File. ---------------------------------------------------------- */



#if 0
/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/

#include <ondk.h>

#include <ondk_gwm_appobj.h>


#include "../../ondk_gwm_objdef.h"
#include "../../ondk_gwm_internal.h"


/**********************************************************************************************************/
#define ____DEFINE_______________________
/**********************************************************************************************************/
#define	KEYBOARD_SYNC_OFF		0
#define	KEYBOARD_SYNC_ON		1
#define	KEYBOARD_UTF8_MAXBUF 3
/**********************************************************************************************************/
#define ____STATIC_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

struct tagKeyboardObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.

	HUINT8 keycapStr[KEYBOARD_UTF8_MAXBUF];
	HUINT32 strSize;

	ONDKSurfaceTextFlags	align;

	ONDK_Color_t	color_over;
	ONDK_Color_t	color_normal;
	ONDK_Color_t	color_click;
	ONDK_Color_t	color_disable;
	ONDK_Color_t	color_fontcolor;
	ONDK_Color_t	color_fontfocus;

	//이미지를 사용할 경우
	HCHAR			*normalImgPath;
	HCHAR			*focusedImgPath;
	HCHAR			*disabledImgPath;
	HCHAR			*pushedImgPath;

	ONDKImageHandle	hNormalImg;
	ONDKImageHandle	hFocusedImg;
	ONDKImageHandle	hDisabledImg;
	ONDKImageHandle	hPushedImg;

	HBOOL			disableKeyFlag;

};

STATIC struct tagKeyboardObject	s_defKeyboard;
STATIC HINT32 keyboardSyncPair = KEYBOARD_SYNC_OFF;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC KEYBOARD_OBJECT	local_gwmkeyboardobj_AllocNewKeyboardObj(void)
{
	struct tagKeyboardObject	*pObj = &s_defKeyboard;

	pObj = (struct tagKeyboardObject *)ONDK_Malloc(sizeof(struct tagKeyboardObject));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defKeyboard, sizeof(struct tagKeyboardObject));

	return pObj;
}

STATIC void	local_gwmkeyboardobj_FreeKeyboardObj(KEYBOARD_OBJECT obj)
{
	ONDK_Assert(obj);

	if(obj)
	{
		if(obj->hNormalImg)		ONDK_IMG_Destroy(obj->hNormalImg);
		if(obj->hFocusedImg)	ONDK_IMG_Destroy(obj->hFocusedImg);
		if(obj->hDisabledImg)	ONDK_IMG_Destroy(obj->hDisabledImg);
		if(obj->hPushedImg)		ONDK_IMG_Destroy(obj->hPushedImg);
		ONDK_Free(obj);
	}

	return;
}

/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC ONDK_Result_t	local_gwmkeyboardobj_defaultMethod_Draw(KEYBOARD_OBJECT keyboardObj, HINT32 arg)

{
	HINT32		x, y, w, h, offset = 0;
	HINT32		x1, y1;
	HINT32		push_status;
	HCHAR 		*title = NULL;
	HBOOL		disableFlag;
	HBOOL 		isFrontMost = FALSE;
	ONDK_Color_t		btnColor=0;
	ONDK_Color_t		bright, face, shadow, fontcolor;
	ONDKFontHandle		*font;
	ONDK_Region_t		area;
	ONDK_Rect_t			rect, drawRect;
	ONDKSurfaceHandle 	screen = ONDK_GWM_GetCurSurface();

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)keyboardObj, &rect);

	drawRect = ONDK_Rect(area.x1 + rect.x, area.y1 + rect.y, rect.w, rect.h);

	push_status = keyboardObj->focus;

	if (( push_status& GUIOBJ_DISABLED) && (keyboardObj->hDisabledImg != NULL))
	{
		ONDK_IMG_StretchBlit(keyboardObj->hDisabledImg, screen, drawRect);
		return ONDK_MESSAGE_BREAK;
	}
	else if ((push_status & GUIOBJ_PUSHED) && (keyboardObj->hPushedImg != NULL))
	{
		ONDK_IMG_StretchBlit(keyboardObj->hPushedImg, screen, drawRect);
		return ONDK_MESSAGE_BREAK;
	}
	else if ((push_status & GUIOBJ_HILIGHTED) && (keyboardObj->hFocusedImg != NULL))
	{
		ONDK_IMG_StretchBlit(keyboardObj->hFocusedImg, screen, drawRect);
		return ONDK_MESSAGE_BREAK;
	}
	else if (keyboardObj->hNormalImg!= NULL)
	{
		ONDK_IMG_StretchBlit(keyboardObj->hNormalImg, screen, drawRect);
		return ONDK_MESSAGE_BREAK;
	}

	x1 = drawRect.x;
	y1 = drawRect.y;
	w = rect.w;
	h = rect.h;

	isFrontMost = arg;
	fontcolor = keyboardObj->color_fontcolor;
	title = keyboardObj->keycapStr;
	disableFlag = keyboardObj->disableKeyFlag;

	if (push_status & GUIOBJ_PUSHED)					/* button feedback */
	{
		btnColor = keyboardObj->color_click;
	}
	else if (push_status & GUIOBJ_DISABLED)			/* disabled */
	{
		btnColor = keyboardObj->color_disable;
	}
	else if (push_status & GUIOBJ_HILIGHTED)			/* get focused */
	{
		btnColor = keyboardObj->color_over;
		fontcolor = keyboardObj->color_fontfocus;
	}
	else												/* normal without a focus */
	{
		btnColor = keyboardObj->color_normal;
	}

	bright 	= ONDK_GWM_GFX_GetSysDimmedColor(screen, btnColor, 150);
	face 	= btnColor;
	shadow 	= ONDK_GWM_GFX_GetSysDimmedColor(screen, btnColor, 50);

	if (FALSE == isFrontMost)
	{	/* deactivated or under-covered button status */
		bright 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, bright, DEFAULT_DIMMED_LEVEL);
		face 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, face,	 DEFAULT_DIMMED_LEVEL);
		shadow 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, shadow, DEFAULT_DIMMED_LEVEL);
	}

	if(TRUE == disableFlag)
	{
		bright = keyboardObj->color_disable;
	}

	ONDK_DRAW_FillRectElement(screen, x1, y1, w, h, bright);

	if(NULL != title)
	{
		font = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, keyboardObj->strSize);
		if (font != NULL)
		{
			x = x1 + 5 + offset;
			w = w - 10;
			y = y1 + (h/2 - ONDK_FONT_GetHeight(font)/2) + offset;
			ONDK_FONT_DrawStringAlignAbbr(screen, font, title, x, y, w, fontcolor, keyboardObj->align);
		}
		ONDK_FONT_Release(font);
	}

	return ONDK_MESSAGE_BREAK;
}


STATIC ONDK_Result_t	local_gwmkeyboardobj_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	KEYBOARD_OBJECT			keyboard;
	ONDK_Rect_t				rect;
	ONDK_Result_t			lRet = ONDK_RET_OK;

	keyboard = (KEYBOARD_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwmkeyboardobj_FreeKeyboardObj(keyboard);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
			if(NULL == object->objDrawMethod)
			{
				lRet = local_gwmkeyboardobj_defaultMethod_Draw((KEYBOARD_OBJECT)object, arg);
			}
			break;

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
			HCHAR 	*new_text;
			new_text = (HCHAR *)arg;
			//keyboard->keycapStr= new_text;
			ONDK_GWM_APP_InvalidateRelativeRect(keyboard->rect.x, keyboard->rect.y, keyboard->rect.w, keyboard->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_KEYDOWN:
		{
			if (arg == KEY_OK || arg == KEY_FRONT_OK)
			{
				keyboardSyncPair=KEYBOARD_SYNC_ON;
				keyboard->focus |= GUIOBJ_PUSHED;
				ONDK_GWM_APP_InvalidateRelativeRect(keyboard->rect.x, keyboard->rect.y, keyboard->rect.w, keyboard->rect.h);
				return ONDK_MESSAGE_CONSUMED;
			}
			return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
		}
		case MT_KEYUP:
			if ( ( arg == KEY_OK || arg == KEY_FRONT_OK )&&keyboardSyncPair==KEYBOARD_SYNC_ON)
			{
				keyboardSyncPair=KEYBOARD_SYNC_OFF;
				keyboard->focus &= ~GUIOBJ_PUSHED;
				ONDK_GWM_APP_InvalidateRelativeRect(keyboard->rect.x, keyboard->rect.y, keyboard->rect.w, keyboard->rect.h);

				/* current app usually doesn't need specific session handle fo key-down event */
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_CLICKED, (Handle_t)NULL, object->ID, arg, (HINT32)keyboard, NULL);
				return ONDK_MESSAGE_CONSUMED;
			}
			return GWM_MESSAGE_BREAK;

		case MT_GET_FOCUS:
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_GET, (Handle_t)NULL, object->ID, arg, (HINT32)keyboard, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(keyboard->rect.x, keyboard->rect.y, keyboard->rect.w, keyboard->rect.h);
			break;

		case MT_LOST_FOCUS:
			keyboardSyncPair=KEYBOARD_SYNC_OFF;
			keyboard->focus &= ~GUIOBJ_PUSHED;
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_LOSE, (Handle_t)NULL, object->ID, arg, (HINT32)keyboard, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(keyboard->rect.x, keyboard->rect.y, keyboard->rect.w, keyboard->rect.h);
			break;

		default:
			break;
	}

	return INT_ONDK_GWM_OBJ_BaseDefaultMethod(proc_msg, object, arg);
}


/**********************************************************************************************************/
#define ____INTERNAL_FUNCTIONS_______________________
/**********************************************************************************************************/

void	INT_ONDK_GWM_COM_Keyboard_Init(void)
{
	struct tagKeyboardObject	*pObj = &s_defKeyboard;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagKeyboardObject));

	pObj->ID = -1;
	pObj->type = GUIOBJ_KEYBOARD;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwmkeyboardobj_DefaultMethod;
	pObj->objDrawMethod = NULL;


	VK_memset(pObj->keycapStr,0x00,KEYBOARD_UTF8_MAXBUF);
	pObj->align = ONDKSTF_LEFT;

	pObj->hNormalImg = NULL;
	pObj->hFocusedImg = NULL;
	pObj->hDisabledImg = NULL;
	pObj->hPushedImg = NULL;

	pObj->color_over 		= 0xFF0000C0;
	pObj->color_normal 		= 0xFF000080;
	pObj->color_click 		= 0xFF0000FF;
	pObj->color_disable		= 0xFF000040;
	pObj->color_fontcolor 	= 0xFF820610;
	pObj->color_fontfocus	= 0xFF820610;

	pObj->disableKeyFlag	= FALSE;
}



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t	ONDK_GWM_COM_Keyboard_Create(HINT32 id, ONDK_Rect_t *rect, HUINT8 *str)
{
	ONDK_GWM_GuiObject_t		objlist;
	KEYBOARD_OBJECT			new_keyboard;
	ONDK_Result_t			hresult;

	ONDK_Assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_keyboard = (KEYBOARD_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_keyboard != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_keyboard = local_gwmkeyboardobj_AllocNewKeyboardObj();
	if (new_keyboard == NULL)
	{
		return GWM_RET_OUT_OF_MEMORY;
	}
	HxSTD_MemCopy(&(new_keyboard->rect), rect, sizeof(ONDK_Rect_t));

	new_keyboard->ID = id;
	memcpy(new_keyboard->keycapStr,str,KEYBOARD_UTF8_MAXBUF);

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_keyboard);
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&new_keyboard->rect);

	return hresult;
}

ONDK_Result_t	ONDK_GWM_COM_Keyboard_SetKeyCapColor(HINT32 id, ONDK_Color_t over, ONDK_Color_t normal, ONDK_Color_t click, ONDK_Color_t disable)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_KEYBOARD)
		return GWM_RET_NO_OBJECT;

	obj->color_over = over;
	obj->color_normal = normal;
	obj->color_click = click;
	obj->color_disable = disable;
	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_COM_Keyboard_SetFontColor(HINT32 id, ONDK_Color_t fontColor, ONDK_Color_t fontfocus)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_KEYBOARD)
		return GWM_RET_NO_OBJECT;

	obj->color_fontcolor = fontColor;
	obj->color_fontfocus = fontfocus;

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Keyboard_SetStatus(HINT32 id, HINT32 push_status)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_KEYBOARD)
		return GWM_RET_NO_OBJECT;

	obj->focus &= ~GUIOBJ_PUSHED;
	obj->focus &= ~GUIOBJ_HILIGHTED;
	obj->focus &= ~GUIOBJ_DISABLED;
	obj->focus &= ~GUIOBJ_NORMAL;

	if (push_status & GUIOBJ_NORMAL)
	{
		;
	}
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

	return GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_COM_Keyboard_ChangeKeyCapStr (HINT32 id, HUINT8 *szNewStr)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_KEYBOARD)
		return GWM_RET_NO_OBJECT;

	if (szNewStr)
	{
		memcpy(obj->keycapStr,szNewStr,KEYBOARD_UTF8_MAXBUF);
	}
	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Keyboard_SetKeycapStrSize(HINT32 id, HUINT32 size)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_KEYBOARD)
		return GWM_RET_NO_OBJECT;

	obj->strSize=size;

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Keyboard_SetAlign(HINT32 id, ONDKSurfaceTextFlags align)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_KEYBOARD)
		return GWM_RET_NO_OBJECT;

	obj->align=align;

	return GWM_RET_OK;
}

ONDK_Result_t	ONDK_GWM_COM_Keyboard_SetDisableKeyCap(HINT32 id, HBOOL flag)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_KEYBOARD)
		return GWM_RET_NO_OBJECT;

	obj->disableKeyFlag=flag;

	/* 필요하다면 나중에 함수로 빼내자*/
	if(flag == TRUE)
	{
		obj->focus = GUIOBJ_DISABLED;
	}

	return GWM_RET_OK;

}





ONDK_Result_t	ONDK_GWM_COM_Keyboard_SetBtnImage(HINT32 id, HCHAR *normal, HCHAR *focused,  HCHAR *disabled, HCHAR *pushed)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;
	if (obj->type != GUIOBJ_KEYBOARD)
		return GWM_RET_NO_OBJECT;


	if(normal != NULL)
	{
		if(obj->normalImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->normalImgPath);
		}
		obj->normalImgPath = normal;
		obj->hNormalImg = ONDK_IMG_CreateSelf(normal);
	}

	if(focused != NULL)
	{
		if(obj->focusedImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->focusedImgPath);
		}
		obj->focusedImgPath = focused;
		obj->hFocusedImg= ONDK_IMG_CreateSelf(focused);
	}

	if(disabled != NULL)
	{
		if(obj->disabledImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->disabledImgPath);
		}

		obj->disabledImgPath = disabled;
		obj->hDisabledImg= ONDK_IMG_CreateSelf(disabled);
	}

	if(pushed != NULL)
	{
		if(obj->pushedImgPath != NULL)
		{
			ONDK_IMG_Destroy(obj->pushedImgPath);
		}
		obj->pushedImgPath = pushed;
		obj->hPushedImg= ONDK_IMG_CreateSelf(pushed);
	}

	return GWM_RET_OK;
}

HCHAR *	ONDK_GWM_COM_Keyboard_GetKeycapStr(HINT32 id)
{
	KEYBOARD_OBJECT		obj;

	obj = (KEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL)
		return NULL;
	if (obj->type != GUIOBJ_KEYBOARD)
		return NULL;

	GWM_assert(obj->type == GUIOBJ_KEYBOARD);

	return obj->keycapStr;
}

/* End of File. ---------------------------------------------------------- */

#endif
