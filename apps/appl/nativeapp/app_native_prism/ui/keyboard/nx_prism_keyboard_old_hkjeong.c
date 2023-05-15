
/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_keyboard.h>

/**********************************************************************************************************/
#define ____DEFINE_______________________
/**********************************************************************************************************/
#define KEYBOARD_X 63
#define KEYBOARD_Y 130
#define KEYBOARD_W	90
#define KEYBOARD_H	43

#define KEYBOARD_ENTER_X	1037
#define KEYBOARD_ENTER_Y	63
#define KEYBOARD_ENTER_W	186
#define KEYBOARD_ENTER_H	50

#define KEYBOARD_BACKSPACE_X	KEYBOARD_ENTER_X
#define KEYBOARD_BACKSPACE_Y	179
#define KEYBOARD_BACKSPACE_W	186
#define KEYBOARD_BACKSPACE_H	43

#define KEYBOARD_CANCEL_X	KEYBOARD_ENTER_X
#define KEYBOARD_CANCEL_Y	228
#define KEYBOARD_CANCEL_W	186
#define KEYBOARD_CANCEL_H	43

#define KEYBOARD_CHANGE_X	KEYBOARD_ENTER_X
#define KEYBOARD_CHANGE_Y 277
#define KEYBOARD_CHANGE_W	186
#define KEYBOARD_CHANGE_H	43

#define KEYBOARD_INPUTBOX_X	63
#define KEYBOARD_INPUTBOX_Y	60
#define KEYBOARD_INPUTBOX_W	954
#define KEYBOARD_INPUTBOX_H	55


#define KEYBOARD_KEYCAP_SIZE	22
#define KEYBOARD_INPUTTEXT_BUFSIZE_512	512

#define	KEYBOARD_SPACE_N_IMG				"/usr/image/102_BTN_SS_Space_N.png"
#define	KEYBOARD_SPACE_C_IMG				"/usr/image/102_BTN_SS_Space_C.png"
#define	KEYBOARD_BACKSPACE_N_IMG			"/usr/image/102_BTN_SS_Back_N.png"
#define	KEYBOARD_BACKSPACE_C_IMG			"/usr/image/102_BTN_SS_Back_C.png"
#define	KEYBOARD_CANCEL_N_IMG				"/usr/image/102_BTN_SS_Delete_N.png"
#define	KEYBOARD_CANCEL_C_IMG				"/usr/image/102_BTN_SS_Delete_C.png"

/**********************************************************************************************************/
#define ____STATIC_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
typedef struct Key_Contents
{
	HINT32			currentFocus_Row;  // current focus object
	HINT32			currentFocus_Col;  // current focus object
	HINT32			modeUpLow;		// upper or lower
	HINT32			modeToggle;      // character or extend or special
	HINT32			currentPtr;      // cursor point
	HINT32			inputboxSize;    //
	HINT32			inputMode;
	HINT32			charCount;
	HINT32			pwOnOff;
	HINT32			smsInput;
	HINT32			beforeInput;
	HINT32			isEnglishArea;  // TRUE : hide language btn --  FALSE : display language btn

	HINT8				*input1;
	HINT8				*input2;
	HINT8				*input3;
};


typedef struct Key_Contents *KEYBOARD_CONTENTS;
STATIC struct Key_Contents	s_defKeyboardContents;

enum
{
	eKeyboard_charKey_ID = 0x00000011,
	eKeyboard_specialKey_ID = 0x00000058,
	eKeyboard_numKeyID = 0x00000060,
	eKeyboard_enterKey_ID = 0x00000070,
	eKeyboard_spaceKey_ID,
	eKeyboard_cancelKey_ID,
	eKeyboard_backspaceKey_ID,
	eKeyboard_changeKey_ID,
	eKeyboard_frame_ID,
	eKeyboard_inputBox_ID,
};

STATIC HUINT8	s_inputBuf[KEYBOARD_INPUTTEXT_BUFSIZE_512];

STATIC HUINT16 s_ucs2CharSet[KEYBOARD_CHAR_BTN_COUNT] =
{
	0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A,
	0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054,
	0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0020, 0x0020
};

STATIC HUINT16 s_ucs2NumSet[KEYBOARD_NUM_BTN_COUNT] =
{
	0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039
};
/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/


STATIC  KEYBOARD_CONTENTS	 nx_keyboard_KeyboardGetContents()
{
	struct Key_Contents	*pObj = &s_defKeyboardContents;

	pObj = (struct Key_Contents *)NX_APP_Calloc(sizeof(struct Key_Contents));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defKeyboardContents, sizeof(struct Key_Contents));

	return pObj;
}

STATIC ONDK_Result_t 	nx_keyboard_InitData(KEYBOARD_CONTENTS pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxSTD_memset(s_inputBuf,0x00,KEYBOARD_INPUTTEXT_BUFSIZE_512);

	return ONDK_RET_OK;
}

STATIC void local_testkeyboard_SetUiObject_CreateCharCodeButton(HUINT32 id, HUINT16 ucs2, ONDK_Rect_t rect)
{
	HUINT8	utf8Code[4] ={0,};

	HxSTD_memset(utf8Code , 0x00, 4);

	if(NULL != ucs2)
	{
		HLIB_CP_ConvertUCS2ToCodepage (eHxCP_UNICODE_UTF8,&ucs2, utf8Code, 4);
	}
	ONDK_GWM_COM_Keyboard_Create(id , &rect, utf8Code);
	ONDK_GWM_COM_Keyboard_SetKeycapStrSize(id , KEYBOARD_KEYCAP_SIZE);
	ONDK_GWM_COM_Keyboard_SetAlign(id,ONDKSTF_CENTER);
	ONDK_GWM_COM_Keyboard_SetFontColor(id , COL(C_T_Whistle_100),COL(C_T_Bang_100));
	ONDK_GWM_COM_Keyboard_SetKeyCapColor(id , COL(C_T_Base01),COL(C_T_Gong08) , COL(C_T_Base01), COL(C_T_Gong08));

	if(ucs2 == 0x0020)
	{
		ONDK_GWM_COM_Keyboard_SetDisableKeyCap(id,TRUE);
	}
	else
	{
		ONDK_GWM_COM_Keyboard_SetDisableKeyCap(id,FALSE);
	}
}

STATIC void local_testkeyboard_SetUiObject_CreateTextButton(HUINT32 id, HCHAR *text, ONDK_Rect_t rect)
{
	ONDK_GWM_COM_Keyboard_Create(id , &rect, text);
	ONDK_GWM_COM_Keyboard_SetKeycapStrSize(id , KEYBOARD_KEYCAP_SIZE);
	ONDK_GWM_COM_Keyboard_SetAlign(id,ONDKSTF_CENTER);
	ONDK_GWM_COM_Keyboard_SetFontColor(id , COL(C_T_Whistle_100),COL(C_T_Bang_100));
	ONDK_GWM_COM_Keyboard_SetKeyCapColor(id , COL(C_T_Base01),COL(C_T_Gong08) , COL(C_T_Base01), COL(C_T_Gong08));

}


STATIC ONDK_Result_t nx_keyboard_DrawInputbox(HINT32 id)
{
	ONDK_Rect_t 	stRect;
	stRect = ONDK_Rect(KEYBOARD_INPUTBOX_X,KEYBOARD_INPUTBOX_Y,KEYBOARD_INPUTBOX_W,KEYBOARD_INPUTBOX_H);
	ONDK_GWM_COM_Text_Create(id,&stRect,"");
	ONDK_GWM_COM_Text_SetFont(id,eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(id,FONT_NORMAL_SIZE);
	ONDK_GWM_COM_Text_ChangeColor(id,COL(C_T_Whistle_100),COL(C_T_Gong08));

	return GWM_RET_OK;
}

STATIC ONDK_Result_t 	nx_keyboard_SetKeyCapDraw(KEYBOARD_CONTENTS pKeyContexts)
{
	ONDK_Rect_t 		stRect;
	ONDK_Result_t		lRet = ONDK_RET_OK;
	HINT32	i = 0;
	HINT32  keyboard_x = KEYBOARD_X;
	HINT32	keyboard_y = KEYBOARD_Y;

	ONDK_GWM_APP_SetAppArea(AP_KEYBOARD_FRAME_X, AP_KEYBOARD_FRAME_Y, AP_KEYBOARD_FRAME_W, AP_KEYBOARD_FRAME_H);

	//뒷판
	stRect = ONDK_Rect(0, 0, AP_KEYBOARD_FRAME_W, AP_KEYBOARD_FRAME_H);
	ONDK_GWM_COM_Rect_Create(eKeyboard_frame_ID, &stRect, COL(C_G_Bang_90));

	//영문 키자판
	for(i = 1; i <= KEYBOARD_CHAR_BTN_COUNT; i++)
	{
		stRect = ONDK_Rect(keyboard_x, keyboard_y, KEYBOARD_W, KEYBOARD_H);
		local_testkeyboard_SetUiObject_CreateCharCodeButton(eKeyboard_charKey_ID+i, s_ucs2CharSet[i-1], stRect);

		keyboard_x+=KEYBOARD_W+6;

		if(i % 10 == 0)
		{
			keyboard_y+=KEYBOARD_H+6;
			keyboard_x = KEYBOARD_X;
		}
	}

	//공백 자판
	stRect = ONDK_Rect(keyboard_x, keyboard_y, KEYBOARD_BACKSPACE_W, KEYBOARD_BACKSPACE_H);
	local_testkeyboard_SetUiObject_CreateCharCodeButton(eKeyboard_spaceKey_ID, NULL, stRect);
	ONDK_GWM_COM_Keyboard_SetBtnImage(eKeyboard_spaceKey_ID, KEYBOARD_SPACE_N_IMG, KEYBOARD_SPACE_C_IMG, NULL, NULL);

	keyboard_x = KEYBOARD_X;
	keyboard_y += KEYBOARD_H+6;

	for(i = 1; i<= KEYBOARD_NUM_BTN_COUNT; i++)
	{
		stRect = ONDK_Rect(keyboard_x, keyboard_y, KEYBOARD_W, KEYBOARD_H);
		local_testkeyboard_SetUiObject_CreateCharCodeButton(eKeyboard_numKeyID+i, s_ucs2NumSet[i-1], stRect);

		keyboard_x+=KEYBOARD_W+6;
	}

	//enter,
	stRect = ONDK_Rect(KEYBOARD_ENTER_X, KEYBOARD_ENTER_Y, KEYBOARD_ENTER_W, KEYBOARD_ENTER_H);
	local_testkeyboard_SetUiObject_CreateTextButton(eKeyboard_enterKey_ID,"Enter",stRect);

	keyboard_x = KEYBOARD_ENTER_X;
	keyboard_y = KEYBOARD_ENTER_Y+KEYBOARD_ENTER_H+17;

	for(i = 0; i <2; i++)
	{
		stRect = ONDK_Rect(keyboard_x, keyboard_y, KEYBOARD_W, KEYBOARD_H);
		local_testkeyboard_SetUiObject_CreateTextButton(eKeyboard_specialKey_ID+i,"a/A", stRect);
		keyboard_x+=KEYBOARD_W+6;
	}

	//backspace
	stRect = ONDK_Rect(KEYBOARD_BACKSPACE_X, KEYBOARD_BACKSPACE_Y, KEYBOARD_BACKSPACE_W, KEYBOARD_BACKSPACE_H);
	local_testkeyboard_SetUiObject_CreateCharCodeButton(eKeyboard_backspaceKey_ID, NULL, stRect);
	ONDK_GWM_COM_Keyboard_SetBtnImage(eKeyboard_backspaceKey_ID, KEYBOARD_BACKSPACE_N_IMG, KEYBOARD_BACKSPACE_C_IMG, NULL, NULL);

	//cancel
	stRect = ONDK_Rect(KEYBOARD_CANCEL_X, KEYBOARD_CANCEL_Y, KEYBOARD_CANCEL_W, KEYBOARD_CANCEL_H);
	local_testkeyboard_SetUiObject_CreateCharCodeButton(eKeyboard_cancelKey_ID, NULL, stRect);
	ONDK_GWM_COM_Keyboard_SetBtnImage(eKeyboard_cancelKey_ID, KEYBOARD_CANCEL_N_IMG, KEYBOARD_CANCEL_C_IMG, NULL, NULL);

	//언어 변환 키
	stRect = ONDK_Rect(KEYBOARD_CHANGE_X, KEYBOARD_CHANGE_Y, KEYBOARD_CHANGE_W, KEYBOARD_CHANGE_H);
	local_testkeyboard_SetUiObject_CreateTextButton(eKeyboard_changeKey_ID,"German",stRect);

	//input box  draw
	nx_keyboard_DrawInputbox(eKeyboard_inputBox_ID);

	ONDK_GWM_Obj_SetFocus(eKeyboard_charKey_ID+1);

	return GWM_RET_OK;

}

STATIC ONDK_Result_t nx_keyboard_DrawMultiObject(HINT32 id,  HUINT8 *nImgPath, HUINT8* fImgPath, HUINT8 *leftStr, HUINT8 *rightStr, ONDK_Rect_t rect)
{

	ONDK_GWM_COM_Multi_Create(id,&rect);
	ONDK_GWM_COM_Multi_SetFontHandle(id,eFont_SystemNormal,22);
	ONDK_GWM_COM_Multi_SetContents(id,nImgPath,fImgPath , leftStr, rightStr,0);
	ONDK_GWM_COM_Multi_SetFontColor(id,COL(C_T_Whistle_100),COL(C_T_Bang_100));
	ONDK_GWM_COM_Multi_SetFocusColor(id , COL(C_T_Base01),COL(C_T_Gong08) , COL(C_T_Base01), COL(C_T_Gong08));
	return GWM_RET_OK;
}



STATIC ONDK_Result_t nx_keyboard_SetmultiObjectDraw(KEYBOARD_CONTENTS pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Rect_t stRect;

	stRect.x = 900;
	stRect.y = 63;
	stRect.w = 258;
	stRect.h = 60;

	nx_keyboard_DrawMultiObject(110,RES_216_OPT_ANTE_N_PNG, RES_216_OPT_ANTE_C_PNG,  "Antenna Type", NULL, stRect);

	stRect.y = 63+60;
	nx_keyboard_DrawMultiObject(111,RES_216_OPT_SEARCH_N_PNG, RES_216_OPT_SEARCH_C_PNG,  "Search Option", NULL, stRect);

	ONDK_GWM_Obj_SetFocus(110);
	return GWM_RET_OK;
}

STATIC ONDK_Result_t nx_keyboard_MsgGwmCreate(KEYBOARD_CONTENTS pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		hRet = ONDK_RET_OK;

	hRet = nx_keyboard_InitData(pKeyContexts, p1,p2,p3);
#if 1
	hRet = nx_keyboard_SetKeyCapDraw(pKeyContexts);
#else
	hRet = nx_keyboard_SetmultiObjectDraw(pKeyContexts,p1,p2,p3);
#endif

	return hRet;
}

/**********************************************************************************************************/
#define ____INTERNAL_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t		NX_PRISM_KeyBoard_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	ONDK_Result_t			lRet = GWM_RET_OK;
	KEYBOARD_CONTENTS	pKeyContexts = nx_keyboard_KeyboardGetContents();
	KEYBOARD_OBJECT keyboardObj;
	HINT32	objId;
	HCHAR	*keyboardStr = NULL;
	HINT32	keyboardStrLen = 0;
	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			lRet = nx_keyboard_MsgGwmCreate(pKeyContexts, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			break;
		case MSG_GWM_KEYDOWN:
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_CLICKED:
			objId = p1;
			keyboardStr = ONDK_GWM_COM_Keyboard_GetKeycapStr(objId);
			keyboardStrLen = HxSTD_StrLen(keyboardStr);

			HxSTD_StrNCat(s_inputBuf,keyboardStr, keyboardStrLen);
			ONDK_GWM_COM_Text_SetText(eKeyboard_inputBox_ID,s_inputBuf);
			break;
		case MSG_GWM_DESTROY:
			break;
		default:
			break;


	}

	if(lRet != GWM_RET_OK)
	{
		return lRet;
	}

	lRet  =  ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);

	if(lRet == GWM_RET_NO_OBJECT || lRet == GWM_MESSAGE_BREAK)
	{
		lRet = ONDK_MESSAGE_CONSUMED;
	}
	return lRet;

}



#if 0

STATIC ONDK_Result_t nx_keyboard_RemoteAppMsg(KEYBOARD_CONTENTS pKeyContexts, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	return GWM_MESSAGE_BREAK;
}

STATIC Key_Charset_e	nx_keyboard_convertLanguageStringToType(HCHAR *pLangFullName)
{
	return eCHARSET_Max;
}

STATIC ONDK_Result_t	nx_keyboard_SetLanguage(KEYBOARD_CONTENTS pKeyContexts)
{

	return ONDK_RET_OK;
}

STATIC ONDK_Result_t nx_keyboard_MsgGwmKeyDown(KEYBOARD_CONTENTS pKeyContexts, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return GWM_RET_OK;
}


STATIC ONDK_Result_t nx_keyboard_MsgGwmTimer(KEYBOARD_CONTENTS pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return GWM_RET_OK;
}

STATIC ONDK_Result_t	nx_keyboard_SetInputBoxString(KEYBOARD_CONTENTS pKeyContexts)
{
	return ONDK_RET_OK;
}

STATIC ONDK_Result_t nx_keyboard_SettingUsingTable(KEYBOARD_CONTENTS pKeyContexts)
{
	return ONDK_RET_OK;
}

STATIC HINT32 nx_keyboard_GetByteSize(HxSTR_t *str, HINT32 start, HINT32 cnt)
{
	return ONDK_RET_OK;
}

STATIC void	nx_keyboard_SetCharSet(KEYBOARD_CONTENTS pKeyContexts)
{

}

STATIC void	nx_keyboard_SetNumSet(KEYBOARD_CONTENTS pKeyContexts)
{

}

STATIC void	nx_keyboard_SetCharButtonCount(KEYBOARD_CONTENTS pKeyContexts)
{

}

STATIC void	nx_keyboard_ChangeLanguage(KEYBOARD_CONTENTS pKeyContexts)
{


}


STATIC void	nx_keyboard_ChangeLanguageForDefault(KEYBOARD_CONTENTS pKeyContexts)
{

}

#endif
