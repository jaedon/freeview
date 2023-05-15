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
 * @file	  		nx_prism_keyboard.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_KEYBOARD_H__
#define __NX_PRISM_KEYBOARD_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <nx_common.h>
#include <nx_prism_ui.h>
#include <nx_prism_definition.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/****************************     ID     ***************************/
/*******************************************************************/
#if !defined(CONFIG_PROD_HDR4000T)
#define	LOCAL_CONFIG_APUI_KEYBOARD_SUPPORT_LANGUAGE_BTN		1
#endif

#define	NX_PRISM_KEYBOARD_ITEM_ID						eMenuFrame_Window_KeyBoard

#define	NX_PRISM_KEYBOARD_FRAME_ID						(NX_PRISM_KEYBOARD_ITEM_ID + 1)
#define	NX_PRISM_KEYBOARD_KEY_OBJ						(NX_PRISM_KEYBOARD_ITEM_ID + 2)
#define	NX_PRISM_KEYBOARD_MSG_TEXT						(NX_PRISM_KEYBOARD_ITEM_ID + 3)
#define	NX_PRISM_KEYBOARD_IMG_INPUTBOX_CURSOR_ID		(NX_PRISM_KEYBOARD_ITEM_ID + 4)

#define	NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID				(NX_PRISM_KEYBOARD_ITEM_ID + 5)
#define	NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID				(NX_PRISM_KEYBOARD_ITEM_ID + 6)
#define	NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID				(NX_PRISM_KEYBOARD_ITEM_ID + 7)

#define	NX_PRISM_KEYBOARD_TIMER_ID						(NX_PRISM_KEYBOARD_ITEM_ID + 8)


/*******************************************************************/
/*********************     SIZE & POSITION     *********************/
/*******************************************************************/

#define	NX_PRISM_KEYBOARD_WINDOW_X						0
#define	NX_PRISM_KEYBOARD_WINDOW_Y						335
#define	NX_PRISM_KEYBOARD_WINDOW_W						1280
#define	NX_PRISM_KEYBOARD_WINDOW_H						365

#define	NX_PRISM_KEYBOARD_FRAME_X						0
#define	NX_PRISM_KEYBOARD_FRAME_Y						0
#define	NX_PRISM_KEYBOARD_FRAME_W						NX_PRISM_KEYBOARD_WINDOW_W
#define	NX_PRISM_KEYBOARD_FRAME_H						NX_PRISM_KEYBOARD_WINDOW_H

#define	NX_PRISM_KEYBOARD_MSG_TEXT_X					63
#define	NX_PRISM_KEYBOARD_MSG_TEXT_Y					(41 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_KEYBOARD_MSG_TEXT_W					1100
#define	NX_PRISM_KEYBOARD_MSG_TEXT_H					(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_KEYBOARD_INPUT_BOX_X					63
#define	NX_PRISM_KEYBOARD_INPUT_BOX_Y					60
#define	NX_PRISM_KEYBOARD_INPUT_BOX_W					954
#define	NX_PRISM_KEYBOARD_INPUT_BOX_H					55

#define	NX_PRISM_KEYBOARD_BTN_START_MG_H				15

#define	NX_PRISM_KEYBOARD_KEY_BTN_ROW_W					954

#define	NX_PRISM_KEYBOARD_KEY_BTN_X						63
#define	NX_PRISM_KEYBOARD_KEY_BTN_Y						(NX_PRISM_KEYBOARD_INPUT_BOX_Y + NX_PRISM_KEYBOARD_INPUT_BOX_H + NX_PRISM_KEYBOARD_BTN_START_MG_H)
#define	NX_PRISM_KEYBOARD_KEY_BTN_W						90
#define	NX_PRISM_KEYBOARD_KEY_BTN_H						43
#define	NX_PRISM_KEYBOARD_KEY_BTN_H_MG					6

#define	NX_PRISM_KEYBOARD_ACTION_BTN_X					(NX_PRISM_KEYBOARD_INPUT_BOX_X + NX_PRISM_KEYBOARD_INPUT_BOX_W + 17)
#define	NX_PRISM_KEYBOARD_ACTION_BTN_Y					(NX_PRISM_KEYBOARD_INPUT_BOX_Y + 3)
#define	NX_PRISM_KEYBOARD_ACTION_BTN_W					186
#define	NX_PRISM_KEYBOARD_ACTION_BTN_H					43

#define	NX_PRISM_KEYBOARD_INPUTBOX1_X					(NX_PRISM_KEYBOARD_INPUT_BOX_X + 18)
#define	NX_PRISM_KEYBOARD_INPUTBOX1_Y					(NX_PRISM_KEYBOARD_INPUT_BOX_Y + 36 - NX_PRISM_FONT_SIZE_24)

#define	NX_PRISM_KEYBOARD_INPUTBOX2_X					(NX_PRISM_KEYBOARD_INPUT_BOX_X + 18)
#define	NX_PRISM_KEYBOARD_INPUTBOX2_Y					(NX_PRISM_KEYBOARD_INPUT_BOX_Y + 36 - NX_PRISM_FONT_SIZE_24)

#define	NX_PRISM_KEYBOARD_INPUTBOX3_X					(NX_PRISM_KEYBOARD_INPUT_BOX_X + 18)
#define	NX_PRISM_KEYBOARD_INPUTBOX3_Y					(NX_PRISM_KEYBOARD_INPUT_BOX_Y + 36 - NX_PRISM_FONT_SIZE_24)

#define	NX_PRISM_KEYBOARD_MAX_INPUT_WIDTH				954

#define	NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_X				(NX_PRISM_KEYBOARD_INPUT_BOX_X)
#define	NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_Y				(NX_PRISM_KEYBOARD_INPUT_BOX_Y + 13)
#define	NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_W				3
#define	NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_H				30


/*******************************************************************/
/***************************     PATH     **************************/
/*******************************************************************/

//#define			KEYBOARD_INPUTBOX_CURSOR			"/usr/image/570_KEY_Input_Cursor.png"

/*******************************************************************/
/**************************     SYMBOL     *************************/
/*******************************************************************/

#define	NX_PRISM_KEYBOARD_INPUT_NORMAL					0
#define	NX_PRISM_KEYBOARD_INPUT_PASSWD					1

#define	NX_PRISM_KEYBOARD_CHARSET_COUNT					2
#define	NX_PRISM_KEYBOARD_NUMBER						10

#define	NX_PRISM_KEYBOARD_BUFFSIZE						256

#define	NX_PRISM_KEYBOARD_STRING_LENGTH					1024
#define	NX_PRISM_KEYBOARD_MAX_LINE_LENGTH				47
#define	NX_PRISM_KEYBOARD_MAX_TOTAL_LENGTH				140

#define	NX_PRISM_KEYBOARD_CHAR_BTN_COUNT				28
#define	NX_PRISM_KEYBOARD_NUM_BTN_COUNT					10
#define	NX_PRISM_KEYBOARD_NUMERIC_COUNT					5

#define	NX_PRISM_KEYBOARD_LIMITED_INTERVAL				100
#define	NX_PRISM_KEYBOARD_NUMERIC_INTERVAL				1500

#define	NX_PRISM_KEYBOARD_PW_INTERVAL					1000
#define	NX_PRISM_KEYBOARD_CURSOR_INTERVAL				800

#define	NX_PRISM_KEYBOARD_ROW_0							0
#define	NX_PRISM_KEYBOARD_ROW_1							1
#define	NX_PRISM_KEYBOARD_ROW_2							2
#define	NX_PRISM_KEYBOARD_ROW_3							3
#define	NX_PRISM_KEYBOARD_ROW_4							4
#define	NX_PRISM_KEYBOARD_ROW_5							5

#define	NX_PRISM_KEYBOARD_COL_0							0
#define	NX_PRISM_KEYBOARD_COL_1							1
#define	NX_PRISM_KEYBOARD_COL_2							2
#define	NX_PRISM_KEYBOARD_COL_3							3
#define	NX_PRISM_KEYBOARD_COL_4							4
#define	NX_PRISM_KEYBOARD_COL_5							5
#define	NX_PRISM_KEYBOARD_COL_6							6
#define	NX_PRISM_KEYBOARD_COL_7							7
#define	NX_PRISM_KEYBOARD_COL_8							8
#define	NX_PRISM_KEYBOARD_COL_9							9
#define	NX_PRISM_KEYBOARD_COL_10						10
#define	NX_PRISM_KEYBOARD_COL_11						11

#define	NX_PRISM_KEYBOARD_ROW_COUNT						5
#define	NX_PRISM_KEYBOARD_COL_COUNT						12
#define	NX_PRISM_KEYBOARD_COL_KEY_COUNT					10

// Input Box
#define	NX_PRISM_KEYBOARD_ROW_INPUTBOX					NX_PRISM_KEYBOARD_ROW_5

// Enter
#define	NX_PRISM_KEYBOARD_ROW_ENTER						NX_PRISM_KEYBOARD_ROW_0
#define	NX_PRISM_KEYBOARD_COL_ENTER						NX_PRISM_KEYBOARD_COL_10

// Uppercase and lowercase letters
#define	NX_PRISM_KEYBOARD_ROW_ULCASE					NX_PRISM_KEYBOARD_ROW_1
#define	NX_PRISM_KEYBOARD_COL_ULCASE					NX_PRISM_KEYBOARD_COL_10

// Special character
#define	NX_PRISM_KEYBOARD_ROW_SPCHAR					NX_PRISM_KEYBOARD_ROW_1
#define	NX_PRISM_KEYBOARD_COL_SPCHAR					NX_PRISM_KEYBOARD_COL_11

// Delete
#define	NX_PRISM_KEYBOARD_ROW_DELETE					NX_PRISM_KEYBOARD_ROW_2
#define	NX_PRISM_KEYBOARD_COL_DELETE_0					NX_PRISM_KEYBOARD_COL_10
#define	NX_PRISM_KEYBOARD_COL_DELETE_1					NX_PRISM_KEYBOARD_COL_11

// Space
#define	NX_PRISM_KEYBOARD_ROW_SPACE						NX_PRISM_KEYBOARD_ROW_3
#define	NX_PRISM_KEYBOARD_COL_SPACE_0					NX_PRISM_KEYBOARD_COL_8
#define	NX_PRISM_KEYBOARD_COL_SPACE_1					NX_PRISM_KEYBOARD_COL_9

// Delete All
#define	NX_PRISM_KEYBOARD_ROW_DELETE_ALL				NX_PRISM_KEYBOARD_ROW_3
#define	NX_PRISM_KEYBOARD_COL_DELETE_ALL_0				NX_PRISM_KEYBOARD_COL_10
#define	NX_PRISM_KEYBOARD_COL_DELETE_ALL_1				NX_PRISM_KEYBOARD_COL_11

// Language Btn
#define	NX_PRISM_KEYBOARD_ROW_LANGUAGE					NX_PRISM_KEYBOARD_ROW_4
#define	NX_PRISM_KEYBOARD_COL_LANGUAGE_0				NX_PRISM_KEYBOARD_COL_10
#define	NX_PRISM_KEYBOARD_COL_LANGUAGE_1				NX_PRISM_KEYBOARD_COL_11

// Number
#define	NX_PRISM_KEYBOARD_ROW_NUMBER					NX_PRISM_KEYBOARD_ROW_4

#define	NX_PRISM_KEYBOARD_KEYPADSET_COUNT				5


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#endif /* !__NX_PRISM_KEYBOARD_H__ */

