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

#ifndef	__KEYBOARD_MAIN_H__
#define	__KEYBOARD_MAIN_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/****************************     ID     ***************************/
/*******************************************************************/
#if !defined(CONFIG_PROD_FVP4000T)
	#define		KEY_SUPPORT_LANGUAGE_BTN	1
#endif

#define 		KEY_FRAME_ID						1
#define			KEY_OBJ								2

#define 		KEY_IMG_INPUTBOX_CURSOR_ID			3

#define 		KEY_INPUTBOX1_TEXT_ID				33
#define 		KEY_INPUTBOX2_TEXT_ID				37
#define 		KEY_INPUTBOX3_TEXT_ID				41

/*******************************************************************/
/*********************     SIZE & POSITION     *********************/
/*******************************************************************/

#define 		AP_KEYBOARD_FRAME_X					0
#define 		AP_KEYBOARD_FRAME_Y					0
#define			AP_KEYBOARD_FRAME_W					1280
#define			AP_KEYBOARD_FRAME_H					720

#define 		AP_KEYBOARD_INPUTBOX1_X				60
#define 		AP_KEYBOARD_INPUTBOX1_Y				216

#define 		AP_KEYBOARD_INPUTBOX2_X				60
#define 		AP_KEYBOARD_INPUTBOX2_Y				182

#define 		AP_KEYBOARD_INPUTBOX3_X				60
#define 		AP_KEYBOARD_INPUTBOX3_Y				148

#define			AP_KEYBOARD_MAX_INPUT_WIDTH			1100

#define 		AP_KEYBOARD_INPUTBOX_W				1150
#define 		AP_KEYBOARD_INPUTBOX_H				35

#define 		AP_KEYBOARD_INPUTBOXCUSOR_W			3
#define 		AP_KEYBOARD_INPUTBOXCUSOR_H			41

/*******************************************************************/
/***************************     PATH     **************************/
/*******************************************************************/

#define			KEYBOARD_INPUTBOX_CURSOR			"/usr/image/570_KEY_Input_Cursor.png"

/*******************************************************************/
/**************************     SYMBOL     *************************/
/*******************************************************************/

#define			KEYBOARD_INPUT_NORMAL				0
#define			KEYBOARD_INPUT_PASSWD				1

#define			KEYBOARD_CHARSET_COUNT				2

#define			KEYBOARD_BUFFSIZE					256

#define			KEYBOARD_STRING_LENGTH				1024
#define			KEYBOARD_MAX_LINE_LENGTH			47
#define			KEYBOARD_MAX_TOTAL_LENGTH			140

#define			KEYBOARD_CHAR_BTN_COUNT				28
#define			KEYBOARD_NUM_BTN_COUNT				10
#define			KEYBOARD_NUMERIC_COUNT				5

#define			KEYBOARD_LIMITED_INTERVAL			100
#define			KEYBOARD_NUMERIC_INTERVAL			1500

#define			KEYBOARD_PW_INTERVAL				1000
#define			KEYBOARD_CURSOR_INTERVAL			800

#define			KEYBOARD_ROW_0						0
#define			KEYBOARD_ROW_1						1
#define			KEYBOARD_ROW_2						2
#define			KEYBOARD_ROW_3						3
#define			KEYBOARD_ROW_4						4
#define			KEYBOARD_ROW_5						5
#define			KEYBOARD_ROW_INPUTBOX				6

#define			KEYBOARD_COL_0						0
#define			KEYBOARD_COL_1						1
#define			KEYBOARD_COL_2						2
#define			KEYBOARD_COL_3						3
#define			KEYBOARD_COL_4						4
#define			KEYBOARD_COL_5						5
#define			KEYBOARD_COL_6						6
#define			KEYBOARD_COL_7						7
#define			KEYBOARD_COL_8						8
#define			KEYBOARD_COL_9						9

#define			KEYBOARD_ROW_COUNT					6
#define			KEYBOARD_COL_COUNT					10

#define			KEYBOARD_KEYPADSET_COUNT			5


#define 		FONT_PATH							"/usr/fonts/HelveticaNeueLTW1G-MdCnUpdv3.001.otf"
//#define 		FONT_POSITION_X						(KEYBOARDBG_x + 13)
//#define 		FONT_POSITION_Y						(KEYBOARDBG_Y + 472)
#define			FONT_NORMAL_SIZE							28
#define			FONT_COLOR							0xffaaaaaa


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#endif /* !__KEYBOARD_MAIN_H__ */

