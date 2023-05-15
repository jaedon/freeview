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
/* Include. 																*/
/* ======================================================================== */
#include <ondk.h>

#include <ondk_gwm_appobj.h>


#include "../../ondk_gwm_objdef.h"
#include "../../ondk_gwm_internal.h"


/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

#if !defined(CONFIG_PROD_HMS1000T) && !defined(CONFIG_PROD_FVP4000T)
	#define 		NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN		1
#endif

#define	MAX_KEYBOARD_OBJECT					128

#define SIMPLE_KEYBOARD_FRAME				"/usr/image/590_SS_BG01.png"
#define SIMPLE_KEYBOARD_FRAME2				"/usr/image/590_SS_BG02.png"

#define	SIMPLE_KEYBOARD_INPUTBOX_N			"/usr/image/590_SS_Input_N.png"
#define	SIMPLE_KEYBOARD_INPUTBOX_C			"/usr/image/590_SS_Input_C.png"

#define	SIMPLE_KEYBOARD_TEXT_N				"/usr/image/590_SS_Key_BG.png"
#define	SIMPLE_KEYBOARD_TEXT_C				"/usr/image/592_SS_Cursor.png"

#define	SIMPLE_KEYBOARD_SEARCHICON			"/usr/image/591_SS_Search.png"

#define	SIMPLE_KEYBOARD_ARROW_L				"/usr/image/106_Arror_smartSearch_left.png"
#define	SIMPLE_KEYBOARD_ARROW_R				"/usr/image/106_Arror_smartSearch_right.png"




#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)

#define	SIMPLE_KEYBOARD_BACKSPACE_N			"/usr/image/592_SS_BTN_106_Backspace_N.png"
#define	SIMPLE_KEYBOARD_BACKSPACE_C			"/usr/image/592_SS_BTN_106_Backspace_C.png"

#define	SIMPLE_KEYBOARD_CLEAR_N				"/usr/image/592_SS_BTN_106_Clear_N.png"
#define	SIMPLE_KEYBOARD_CLEAR_C				"/usr/image/592_SS_BTN_106_Clear_C.png"

#define	SIMPLE_KEYBOARD_SPACE_N				"/usr/image/592_SS_BTN_106_Space_N.png"
#define	SIMPLE_KEYBOARD_SPACE_C				"/usr/image/592_SS_BTN_106_Space_C.png"

#define	SIMPLE_KEYBOARD_TOGGLE_N			"/usr/image/592_SS_BTN_106_Change_N.png"
#define	SIMPLE_KEYBOARD_TOGGLE_C			"/usr/image/592_SS_BTN_106_Change_C.png"


#define	SIMPLE_KEYBOARD_LANGUAGE_N			"/usr/image/592_SS_BTN_Language_N.png"
#define	SIMPLE_KEYBOARD_LANGUAGE_C			"/usr/image/592_SS_BTN_Language_C.png"

#else

#define	SIMPLE_KEYBOARD_BACKSPACE_N			"/usr/image/613_SS_BTN_140_Backspace_N.png"
#define	SIMPLE_KEYBOARD_BACKSPACE_C			"/usr/image/613_SS_BTN_140_Backspace_C.png"

#define	SIMPLE_KEYBOARD_CLEAR_N				"/usr/image/613_SS_BTN_140_Clear_N.png"
#define	SIMPLE_KEYBOARD_CLEAR_C				"/usr/image/613_SS_BTN_140_Clear_C.png"

#define	SIMPLE_KEYBOARD_SPACE_N				"/usr/image/613_SS_BTN_140_Space_N.png"
#define	SIMPLE_KEYBOARD_SPACE_C				"/usr/image/613_SS_BTN_140_Space_C.png"

#define	SIMPLE_KEYBOARD_TOGGLE_N			"/usr/image/613_SS_BTN_140_Change_N.png"
#define	SIMPLE_KEYBOARD_TOGGLE_C			"/usr/image/613_SS_BTN_140_Change_C.png"

#endif

#define SIMPLE_KEYBOARD_BG_X				0
#define SIMPLE_KEYBOARD_BG_Y				0

#define SIMPLE_KEYBOARD_INPUT_X				74
#define SIMPLE_KEYBOARD_INPUT_Y				37

#define SIMPLE_KEYBOARD_SEARCH_X			92
#define SIMPLE_KEYBOARD_SEARCH_Y			54



#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)

#define SIMPLE_KEYBOARD_BACK_X				613
#define SIMPLE_KEYBOARD_BACK_Y				45

#define SIMPLE_KEYBOARD_CLEAR_X				613+106+6
#define SIMPLE_KEYBOARD_CLEAR_Y				45

#define SIMPLE_KEYBOARD_SPACE_X				613+106+6+106+6
#define SIMPLE_KEYBOARD_SPACE_Y				45

#define SIMPLE_KEYBOARD_TOGGLE_X			613+106+6+106+6+106+6
#define SIMPLE_KEYBOARD_TOGGLE_Y			45

#define SIMPLE_KEYBOARD_LANGUAGE_X			613+106+6+106+6+106+6+106+6
#define SIMPLE_KEYBOARD_LANGUAGE_Y			45

#else

#define SIMPLE_KEYBOARD_BACK_X				616
#define SIMPLE_KEYBOARD_BACK_Y				45

#define SIMPLE_KEYBOARD_CLEAR_X				616+140+10
#define SIMPLE_KEYBOARD_CLEAR_Y				45

#define SIMPLE_KEYBOARD_SPACE_X				616+140+10+140+10
#define SIMPLE_KEYBOARD_SPACE_Y				45

#define SIMPLE_KEYBOARD_TOGGLE_X			616+140+10+140+10+140+10
#define SIMPLE_KEYBOARD_TOGGLE_Y			45

#endif

#define SIMPLE_KEYBOARD_BGIMAGE_X			0
#define SIMPLE_KEYBOARD_BGIMAGE_Y			110

#define SIMPLE_KEYBOARD_LEFTARROW_X			49
#define SIMPLE_KEYBOARD_LEFTARROW_Y			122

#define SIMPLE_KEYBOARD_RIGHTARROW_X		1200
#define SIMPLE_KEYBOARD_RIGHTARROW_Y		122


#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
#define SIMPLE_KEYBOARD_KEY_CURSOR_IDX		11

#define	FONT_NORMAL_COLOR					0xffffffff
#define	FONT_FOCUS_COLOR					0xff000000

#define	SIMPLE_KEYBOARD_BTN_WIDTH_GAP		70
#define	SIMPLE_KEYBOARD_BTN_HEIGHT_GAP		34

#define	SIMPLE_KEYBOARD_NORMAL_WIDTH			167
#define	SIMPLE_KEYBOARD_LONG_WIDTH				284

#else
#define SIMPLE_KEYBOARD_KEY_CURSOR_IDX		10
#endif

#define	DEF_BTN_STR_LEN							128
#define	DEF_CHAR_SET_LEN						3

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */

typedef enum {
	eSimpleKeyboard_Input = 1,
	eSimpleKeyboard_BackSpace,
	eSimpleKeyboard_Clear,
	eSimpleKeyboard_Space,
	eSimpleKeyboard_Change,
#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	eSimpleKeyboard_Language,
#endif
	eSimpleKeyboard_Keyboard,		// 7
	eSimpleKeyboard_Max
} SimpleKeyboardItem_t;

struct tagNativeSimpleKeyboardObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.

	HINT32	curFocus;

	HCHAR charSet[22][DEF_CHAR_SET_LEN];

	ONDKFontHandle	fontSmallHandle;
	ONDKFontHandle	fontBigHandle;
	ONDKFontHandle	fontExSmallHandle;

	ONDKImageHandle	hBackgroundImg1;
	ONDKImageHandle hBackgroundImg2;

	ONDKImageHandle hFocusImg;

	ONDKImageHandle	hLeftArrowImg;
	ONDKImageHandle	hRightArrowImg;

	ONDKImageHandle hInputBox_C_Img;
	ONDKImageHandle hInputBox_N_Img;

	ONDKImageHandle	hSearchIconImg;

	ONDKImageHandle	hBack_C_Img;
	ONDKImageHandle	hBack_N_Img;
	ONDKImageHandle	hClear_C_Img;
	ONDKImageHandle	hClear_N_Img;
	ONDKImageHandle	hSpace_C_Img;
	ONDKImageHandle	hSpace_N_Img;
	ONDKImageHandle	hToggle_C_Img;
	ONDKImageHandle	hToggle_N_Img;
#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	ONDKImageHandle	hLanguage_C_Img;
	ONDKImageHandle	hLanguage_N_Img;

	HCHAR szLanguageBtn[DEF_BTN_STR_LEN];
#endif
};

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
static struct tagNativeSimpleKeyboardObject	s_defSimpleKeyboard;


/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */

static NATIVE_SIMPLEKEYBOARD_OBJECT		local_gwm_com_AllocNewSimpleKeyboardObj(void)
{
	struct tagNativeSimpleKeyboardObject	*pObj = &s_defSimpleKeyboard;

	pObj = (struct tagNativeSimpleKeyboardObject *)ONDK_Malloc(sizeof(struct tagNativeSimpleKeyboardObject));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defSimpleKeyboard, sizeof(struct tagNativeSimpleKeyboardObject));

	return pObj;
}

static void		local_gwm_com_FreeSimpleKeyboardObj(NATIVE_SIMPLEKEYBOARD_OBJECT obj)
{
	ONDK_Assert(obj);

	if(obj->fontSmallHandle)	ONDK_FONT_Release(obj->fontSmallHandle);
	if(obj->fontBigHandle)		ONDK_FONT_Release(obj->fontBigHandle);

	if(obj->hBackgroundImg1)	ONDK_IMG_Destroy(obj->hBackgroundImg1);
	if(obj->hBackgroundImg2)	ONDK_IMG_Destroy(obj->hBackgroundImg2);
	if(obj->hSearchIconImg)		ONDK_IMG_Destroy(obj->hSearchIconImg);
	if(obj->hFocusImg)			ONDK_IMG_Destroy(obj->hFocusImg);
	if(obj->hLeftArrowImg)		ONDK_IMG_Destroy(obj->hLeftArrowImg);
	if(obj->hRightArrowImg)		ONDK_IMG_Destroy(obj->hRightArrowImg);
	if(obj->hInputBox_C_Img)	ONDK_IMG_Destroy(obj->hInputBox_C_Img);
	if(obj->hInputBox_N_Img)	ONDK_IMG_Destroy(obj->hInputBox_N_Img);

	if(obj->hBack_C_Img)		ONDK_IMG_Destroy(obj->hBack_C_Img);
	if(obj->hBack_N_Img)		ONDK_IMG_Destroy(obj->hBack_N_Img);
	if(obj->hClear_C_Img)		ONDK_IMG_Destroy(obj->hClear_C_Img);
	if(obj->hClear_N_Img)		ONDK_IMG_Destroy(obj->hClear_N_Img);
	if(obj->hSpace_C_Img)		ONDK_IMG_Destroy(obj->hSpace_C_Img);
	if(obj->hSpace_N_Img)		ONDK_IMG_Destroy(obj->hSpace_N_Img);
	if(obj->hToggle_C_Img)		ONDK_IMG_Destroy(obj->hToggle_C_Img);
	if(obj->hToggle_N_Img)		ONDK_IMG_Destroy(obj->hToggle_N_Img);

#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	if(obj->fontExSmallHandle)	ONDK_FONT_Release(obj->fontExSmallHandle);
	if(obj->hLanguage_C_Img)	ONDK_IMG_Destroy(obj->hLanguage_C_Img);
	if(obj->hLanguage_N_Img)	ONDK_IMG_Destroy(obj->hLanguage_N_Img);
#endif

	ONDK_Free(obj);

	return;
}


static ONDK_Result_t		local_gwm_com_defaultDrawSimpleKeyboardMethod(ONDKWindowHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																	ONDK_GWM_GuiObject_t object, void *arg, int param1)
{
	ONDK_Rect_t 		drawRect;
	HINT32	i;
	NATIVE_SIMPLEKEYBOARD_OBJECT	simpleKeyboard = (NATIVE_SIMPLEKEYBOARD_OBJECT)object;

	ONDK_Assert(screen);
	ONDK_Assert(area);
	ONDK_Assert(rect);
	ONDK_Assert(object);

	drawRect = ONDK_Rect(area->x1 + rect->x, area->y1 + rect->y, rect->w, rect->h);

	ONDK_IMG_Blit(simpleKeyboard->hBackgroundImg1, screen, SIMPLE_KEYBOARD_BG_X, SIMPLE_KEYBOARD_BG_Y);

	if(simpleKeyboard->curFocus == eSimpleKeyboard_Input)
		ONDK_IMG_Blit(simpleKeyboard->hInputBox_C_Img, screen, SIMPLE_KEYBOARD_INPUT_X, SIMPLE_KEYBOARD_INPUT_Y);
	else ONDK_IMG_Blit(simpleKeyboard->hInputBox_N_Img, screen, SIMPLE_KEYBOARD_INPUT_X, SIMPLE_KEYBOARD_INPUT_Y);

	ONDK_IMG_Blit(simpleKeyboard->hSearchIconImg, screen, SIMPLE_KEYBOARD_SEARCH_X, SIMPLE_KEYBOARD_SEARCH_Y);

	if(simpleKeyboard->curFocus == eSimpleKeyboard_BackSpace)
		ONDK_IMG_Blit(simpleKeyboard->hBack_C_Img, screen, SIMPLE_KEYBOARD_BACK_X, SIMPLE_KEYBOARD_BACK_Y);
	else ONDK_IMG_Blit(simpleKeyboard->hBack_N_Img, screen, SIMPLE_KEYBOARD_BACK_X, SIMPLE_KEYBOARD_BACK_Y);

	if(simpleKeyboard->curFocus == eSimpleKeyboard_Clear)
		ONDK_IMG_Blit(simpleKeyboard->hClear_C_Img, screen, SIMPLE_KEYBOARD_CLEAR_X, SIMPLE_KEYBOARD_CLEAR_Y);
	else ONDK_IMG_Blit(simpleKeyboard->hClear_N_Img, screen, SIMPLE_KEYBOARD_CLEAR_X, SIMPLE_KEYBOARD_CLEAR_Y);

	if(simpleKeyboard->curFocus == eSimpleKeyboard_Space)
		ONDK_IMG_Blit(simpleKeyboard->hSpace_C_Img, screen, SIMPLE_KEYBOARD_SPACE_X, SIMPLE_KEYBOARD_SPACE_Y);
	else ONDK_IMG_Blit(simpleKeyboard->hSpace_N_Img, screen, SIMPLE_KEYBOARD_SPACE_X, SIMPLE_KEYBOARD_SPACE_Y);

	if(simpleKeyboard->curFocus == eSimpleKeyboard_Change)
		ONDK_IMG_Blit(simpleKeyboard->hToggle_C_Img, screen, SIMPLE_KEYBOARD_TOGGLE_X, SIMPLE_KEYBOARD_TOGGLE_Y);
	else ONDK_IMG_Blit(simpleKeyboard->hToggle_N_Img, screen, SIMPLE_KEYBOARD_TOGGLE_X, SIMPLE_KEYBOARD_TOGGLE_Y);

#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	if(simpleKeyboard->curFocus == eSimpleKeyboard_Language)
	{
		ONDK_IMG_Blit(simpleKeyboard->hLanguage_C_Img, screen, SIMPLE_KEYBOARD_LANGUAGE_X, SIMPLE_KEYBOARD_LANGUAGE_Y);

		ONDK_FONT_DrawStringAlign(screen,simpleKeyboard->fontExSmallHandle, simpleKeyboard->szLanguageBtn, 982 + SIMPLE_KEYBOARD_BTN_WIDTH_GAP, 26 + SIMPLE_KEYBOARD_BTN_HEIGHT_GAP, SIMPLE_KEYBOARD_NORMAL_WIDTH, FONT_FOCUS_COLOR, ONDKSTF_CENTER);
	}
	else
	{
		ONDK_IMG_Blit(simpleKeyboard->hLanguage_N_Img, screen, SIMPLE_KEYBOARD_LANGUAGE_X, SIMPLE_KEYBOARD_LANGUAGE_Y);
		ONDK_FONT_DrawStringAlign(screen,simpleKeyboard->fontExSmallHandle, simpleKeyboard->szLanguageBtn, 982 + SIMPLE_KEYBOARD_BTN_WIDTH_GAP, 26 + SIMPLE_KEYBOARD_BTN_HEIGHT_GAP, SIMPLE_KEYBOARD_NORMAL_WIDTH, FONT_NORMAL_COLOR, ONDKSTF_CENTER);
	}

	ONDK_IMG_Blit(simpleKeyboard->hBackgroundImg2, screen, SIMPLE_KEYBOARD_BGIMAGE_X, SIMPLE_KEYBOARD_BGIMAGE_Y);
	ONDK_IMG_Blit(simpleKeyboard->hLeftArrowImg, screen, SIMPLE_KEYBOARD_LEFTARROW_X, SIMPLE_KEYBOARD_LEFTARROW_Y);
	ONDK_IMG_Blit(simpleKeyboard->hRightArrowImg, screen, SIMPLE_KEYBOARD_RIGHTARROW_X, SIMPLE_KEYBOARD_RIGHTARROW_Y);

	for(i=0;i<22;i++)
	{
		if(HxSTD_StrCmp(simpleKeyboard->charSet[i]," ") == 0)
			break;

		if(simpleKeyboard->curFocus == i+SIMPLE_KEYBOARD_KEY_CURSOR_IDX)
		{
			ONDK_IMG_Blit(simpleKeyboard->hFocusImg, screen, i*51+60,95);
			ONDK_FONT_DrawStringAlign(screen,simpleKeyboard->fontBigHandle,simpleKeyboard->charSet[i],25+(51*i),118, SIMPLE_KEYBOARD_NORMAL_WIDTH, 0xff000000,ONDKSTF_CENTER);
		}
		else
		{
			ONDK_FONT_DrawStringAlign(screen,simpleKeyboard->fontSmallHandle,simpleKeyboard->charSet[i],25+(51*i),120, SIMPLE_KEYBOARD_NORMAL_WIDTH, 0xffffffff,ONDKSTF_CENTER);
		}
	}
#endif

	NOT_USED_PARAM(param1);
	NOT_USED_PARAM(arg);

	return GWM_RET_OK;
}


static ONDK_Result_t	local_gwm_com_defaultSimpleKeyboardProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	NATIVE_SIMPLEKEYBOARD_OBJECT	keyboard;
	ONDK_Region_t			area;
	ONDK_Rect_t				rect;
	HBOOL					isFrontMost = FALSE;

//	ONDK_Assert(object->type == GUIOBJ_KEYBOARD);
	keyboard = (NATIVE_SIMPLEKEYBOARD_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwm_com_FreeSimpleKeyboardObj(keyboard);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			isFrontMost = (arg > 0) ? TRUE : FALSE;

			if (object->objDrawMethod != NULL)
			{
				object->objDrawMethod(ONDK_GWM_GetCurSurface(), &area, &rect, object, (NATIVE_SIMPLEKEYBOARD_OBJECT)keyboard, arg);
			}
			else
			{
				local_gwm_com_defaultDrawSimpleKeyboardMethod(ONDK_GWM_GetCurSurface(), &area, &rect, object, (NATIVE_SIMPLEKEYBOARD_OBJECT)keyboard, arg);
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

void local_gwm_com_SetValues(HINT32 id)
{
	NATIVE_SIMPLEKEYBOARD_OBJECT obj;

	obj = (NATIVE_SIMPLEKEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_NATIVESIMPLEKEYBOARD)
	{
		return ;
	}

	obj->fontSmallHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold ,28);
	obj->fontBigHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold,34);
#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	obj->fontExSmallHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold ,22);
#endif

	//bg1
	if(obj->hBackgroundImg1 != NULL)
	{
		ONDK_IMG_Destroy(obj->hBackgroundImg1);
		obj->hBackgroundImg1 = NULL;
	}

	obj->hBackgroundImg1 = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_FRAME);

	//bg2
	if(obj->hBackgroundImg2 != NULL)
	{
		ONDK_IMG_Destroy(obj->hBackgroundImg2);
		obj->hBackgroundImg2 = NULL;
	}

	obj->hBackgroundImg2 = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_FRAME2);

	//search icon
	if(obj->hSearchIconImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hSearchIconImg);
		obj->hSearchIconImg = NULL;
	}

	obj->hSearchIconImg = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_SEARCHICON);


	//input box
	if(obj->hInputBox_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hInputBox_C_Img);
		obj->hInputBox_C_Img = NULL;
	}

	obj->hInputBox_C_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_INPUTBOX_C);
	if(obj->hInputBox_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hInputBox_N_Img);
		obj->hInputBox_N_Img = NULL;
	}

	obj->hInputBox_N_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_INPUTBOX_N);

	//backspace
	if(obj->hBack_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hBack_C_Img);
		obj->hBack_C_Img = NULL;
	}

	obj->hBack_C_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_BACKSPACE_C);
	if(obj->hBack_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hBack_N_Img);
		obj->hBack_N_Img = NULL;
	}

	obj->hBack_N_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_BACKSPACE_N);

	//clear
	if(obj->hClear_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hClear_C_Img);
		obj->hClear_C_Img = NULL;
	}

	obj->hClear_C_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_CLEAR_C);
	if(obj->hClear_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hClear_N_Img);
		obj->hClear_N_Img = NULL;
	}

	obj->hClear_N_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_CLEAR_N);

	//space
	if(obj->hSpace_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hSpace_C_Img);
		obj->hSpace_C_Img = NULL;
	}

	obj->hSpace_C_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_SPACE_C);
	if(obj->hSpace_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hSpace_N_Img);
		obj->hSpace_N_Img = NULL;
	}

	obj->hSpace_N_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_SPACE_N);

	//toggle
	if(obj->hToggle_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hToggle_C_Img);
		obj->hToggle_C_Img = NULL;
	}

	obj->hToggle_C_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_TOGGLE_C);
	if(obj->hToggle_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hToggle_N_Img);
		obj->hToggle_N_Img = NULL;
	}

	obj->hToggle_N_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_TOGGLE_N);

#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	//Language
	if(obj->hLanguage_C_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hLanguage_C_Img);
		obj->hLanguage_C_Img = NULL;
	}
	obj->hLanguage_C_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_LANGUAGE_C);

	if(obj->hLanguage_N_Img!= NULL)
	{
		ONDK_IMG_Destroy(obj->hLanguage_N_Img);
		obj->hLanguage_N_Img = NULL;
	}
	obj->hLanguage_N_Img = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_LANGUAGE_N);
#endif
	//left arrow
	if(obj->hLeftArrowImg!= NULL)
	{
		ONDK_IMG_Destroy(obj->hLeftArrowImg);
		obj->hLeftArrowImg = NULL;
	}

	obj->hLeftArrowImg = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_ARROW_L);

	//right arrow
	if(obj->hRightArrowImg!= NULL)
	{
		ONDK_IMG_Destroy(obj->hRightArrowImg);
		obj->hRightArrowImg = NULL;
	}

	obj->hRightArrowImg = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_ARROW_R);

	//focus img
	if(obj->hFocusImg!= NULL)
	{
		ONDK_IMG_Destroy(obj->hFocusImg);
		obj->hFocusImg = NULL;
	}

	obj->hFocusImg = ONDK_IMG_CreateSelf(SIMPLE_KEYBOARD_TEXT_C);

}


/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

void 			INT_ONDK_GWM_COM_SimpleKeyboard_Init(void)
{
	struct tagNativeSimpleKeyboardObject	*pObj = &s_defSimpleKeyboard;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagNativeSimpleKeyboardObject));

	pObj->ID = -1;
	pObj->type = GUIOBJ_NATIVESIMPLEKEYBOARD;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwm_com_defaultSimpleKeyboardProc;
	pObj->objDrawMethod = NULL;

	pObj->curFocus = SIMPLE_KEYBOARD_KEY_CURSOR_IDX;
}

ONDK_Result_t				ONDK_GWM_COM_SimpleKeyboard_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 curFocus, HCHAR *str)
{
	ONDK_GWM_GuiObject_t		objlist;
	NATIVE_SIMPLEKEYBOARD_OBJECT	new_keyboard;
	ONDK_Result_t			hresult;

	ONDK_Assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_keyboard = (NATIVE_SIMPLEKEYBOARD_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_keyboard != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_keyboard = local_gwm_com_AllocNewSimpleKeyboardObj();
	if (new_keyboard == NULL)
	{
		ONDK_Error("[%s:%d] obj GWM_RET_OUT_OF_MEMORY\n", __FUNCTION__, __LINE__);
		return GWM_RET_OUT_OF_MEMORY;
	}

	new_keyboard->ID = id;
	new_keyboard->rect = *rect;
	new_keyboard->curFocus = curFocus;

#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	if (str != NULL)
	{
		HLIB_STD_StrNCpySafe(new_keyboard->szLanguageBtn, str, DEF_BTN_STR_LEN);
		new_keyboard->szLanguageBtn[HxSTD_StrLen(str)] = '\0';
	}
#endif

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_keyboard);

	ONDK_GWM_APP_InvalidateRelativeRect(new_keyboard->rect.x, new_keyboard->rect.y, new_keyboard->rect.w, new_keyboard->rect.h);
	local_gwm_com_SetValues(id);

	return hresult;
}

HINT32 ONDK_GWM_COM_SimpleKeyboard_GetCurFocus(HINT32 id)
{
	NATIVE_SIMPLEKEYBOARD_OBJECT		obj;

	obj = (NATIVE_SIMPLEKEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_NATIVESIMPLEKEYBOARD)
	{
		return -1;
	}

	return obj->curFocus;
}

void ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(HINT32 id, HINT32 cur)
{
	NATIVE_SIMPLEKEYBOARD_OBJECT		obj;

	obj = (NATIVE_SIMPLEKEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_NATIVESIMPLEKEYBOARD)
	{
		return ;
	}

	obj->curFocus = cur;
}

void ONDK_GWM_COM_SimpleKeyboard_SetCharValue(HINT32 id, HINT32 pos, HCHAR *str)
{
	NATIVE_SIMPLEKEYBOARD_OBJECT		obj;

	obj = (NATIVE_SIMPLEKEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_NATIVESIMPLEKEYBOARD)
	{
		return ;
	}

	HLIB_STD_StrNCpySafe(obj->charSet[pos],str,DEF_CHAR_SET_LEN);
	obj->charSet[pos][HxSTD_StrLen(str)] = '\0';

}
#if defined(NATIVE_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
void ONDK_GWM_COM_SimpleKeyboard_SetCurLanguage(HINT32 id, HCHAR *str)
{
	NATIVE_SIMPLEKEYBOARD_OBJECT		obj;

	obj = (NATIVE_SIMPLEKEYBOARD_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_NATIVESIMPLEKEYBOARD)
	{
		return ;
	}

	HLIB_STD_StrNCpySafe(obj->szLanguageBtn, str, DEF_BTN_STR_LEN);
	obj->szLanguageBtn[HxSTD_StrLen(str)] = '\0';
}
#endif

/* End of File. ---------------------------------------------------------- */


