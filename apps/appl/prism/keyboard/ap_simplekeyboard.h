/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SIMPLE_KEYBOARD_MAIN_H__
#define	__SIMPLE_KEYBOARD_MAIN_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#if !defined(CONFIG_PROD_HMS1000T) && !defined(CONFIG_PROD_FVP4000T)
	#define 		SIMPLE_KEY_SUPPORT_LANGUAGE_BTN		1
#endif

#define 		AP_SIMPLE_KEYBOARD_FRAME_X			0
#define 		AP_SIMPLE_KEYBOARD_FRAME_Y			0
#define			AP_SIMPLE_KEYBOARD_FRAME_W			1280
#define			AP_SIMPLE_KEYBOARD_FRAME_H			450

#define 		AP_SIMPLE_KEYBOARD_INPUTBOXCUSOR_W	3
#define 		AP_SIMPLE_KEYBOARD_INPUTBOXCUSOR_H	31

#define 		AP_SIMPLE_KEYBOARD_INPUTBOX_X		74
#define 		AP_SIMPLE_KEYBOARD_INPUTBOX_Y		37
#define 		AP_SIMPLE_KEYBOARD_INPUTBOX_W		451
#define 		AP_SIMPLE_KEYBOARD_INPUTBOX_H		30

#define			SIMPLE_KEY_OBJ						26

#define 		SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID	3
#define 		SIMPLE_KEY_INPUTBOX_TEXT_ID			2
#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
#define 		SIMPLE_KEY_CHAR_ID					11
#else
#define 		SIMPLE_KEY_CHAR_ID					10
#endif

#define 		SIMPLE_KEY_INPUTBOX_ID				1
#define 		SIMPLE_KEY_BACK_ID					2
#define 		SIMPLE_KEY_CLEAR_ID					3
#define 		SIMPLE_KEY_SPACE_ID					4
#define 		SIMPLE_KEY_TOGGLE_ID				5
#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
#define 		SIMPLE_KEY_LANGUAGE_ID				6
#endif

#define			SIMPLE_KEYBOARD_SHOW_CHAR_COUNT		25
#define			SIMPLE_KEYBOARD_CHAR_COUNT			50
#define			SIMPLE_KEYBOARD_NUMERIC_COUNT		12
#define			SIMPLE_KEYBOARD_NUM_BTN_COUNT		10

#define			SIMPLE_KEYBOARD_CURSOR_INTERVAL		800
#define			SIMPLE_KEYBOARD_LIMITED_INTERVAL	100
//#define			SIMPLE_KEYBOARD_NUMERIC_INTERVAL			1500
#define			SIMPLE_KEYBOARD_NUMERIC_INTERVAL			1000



#define			SIMPLE_KEYBOARD_CHARSET_NUM			1
#define			SIMPLE_KEYBOARD_CHARSET_CHAR		0

#define			SIMPLE_KEYBOARD_MAX_CHAR_LENGTH		435 //150

#define			SIMPLE_KEYBOARD_INPUTBOX_CURSOR		"/usr/image/590_SS_Input_Cursor.png"

#define			FONT_NORMAL_SIZE					26

#define			FONT_COLOR							0xffaaaaaa

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#endif /* !__SIMPLE_KEYBOARD_MAIN_H__ */

