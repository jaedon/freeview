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
 * @file	  		nx_prism_simplekeyboard.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


#ifndef	__NX_PRISM_SIMPLEKEYBOARD_H__
#define	__NX_PRISM_SIMPLEKEYBOARD_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#if !defined(CONFIG_PROD_HMS1000T) && !defined(CONFIG_PROD_HDR4000T)
#define	NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN		1
#endif

#define	NX_PRISM_SIMPLE_KEYBOARD_FRAME_X			0
#define	NX_PRISM_SIMPLE_KEYBOARD_FRAME_Y			0
#define	NX_PRISM_SIMPLE_KEYBOARD_FRAME_W			1280

#define	NX_PRISM_SIMPLE_KEYBOARD_FRAME_H			170

#define	NX_PRISM_SIMPLE_KEYBOARD_INPUTBOXCUSOR_W	3
#define	NX_PRISM_SIMPLE_KEYBOARD_INPUTBOXCUSOR_H	31

#define	NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_X			74
#define	NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_Y			37
#define	NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_W			451
#define	NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_H			30

#define	NX_PRISM_SIMPLE_KEY_OBJ						26

#define	NX_PRISM_SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID	3
#define	NX_PRISM_SIMPLE_KEY_INPUTBOX_TEXT_ID		2
#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
#define	NX_PRISM_SIMPLE_KEY_CHAR_ID					11
#else
#define	NX_PRISM_SIMPLE_KEY_CHAR_ID					10
#endif

#define	NX_PRISM_SIMPLE_KEY_INPUTBOX_ID				1
#define	NX_PRISM_SIMPLE_KEY_BACK_ID					2
#define	NX_PRISM_SIMPLE_KEY_CLEAR_ID				3
#define	NX_PRISM_SIMPLE_KEY_SPACE_ID				4
#define	NX_PRISM_SIMPLE_KEY_TOGGLE_ID				5
#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
#define	NX_PRISM_SIMPLE_KEY_LANGUAGE_ID				6
#endif

#define	NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT		2

#define	NX_PRISM_SIMPLE_KEYBOARD_SHOW_CHAR_COUNT	21
#define	NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT			50
#define	NX_PRISM_SIMPLE_KEYBOARD_NUMERIC_COUNT		12
#define	NX_PRISM_SIMPLE_KEYBOARD_NUM_BTN_COUNT		10

#define	NX_PRISM_SIMPLE_KEYBOARD_CURSOR_INTERVAL	800
#define	NX_PRISM_SIMPLE_KEYBOARD_LIMITED_INTERVAL	100
#define	NX_PRISM_SIMPLE_KEYBOARD_NUMERIC_INTERVAL	1500


#define	NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM		1
#define	NX_PRISM_SIMPLE_KEYBOARD_CHARSET_CHAR		0

#define	NX_PRISM_SIMPLE_KEYBOARD_MAX_CHAR_LENGTH	435 //150

#define	NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_CURSOR	RES_590_SS_INPUT_CURSOR_PNG

#define	NX_PRISM_SIMPLE_FONT_NORMAL_SIZE			28

#define	NX_PRISM_SIMPLE_KEYBOARD_FONT_COLOR			0xffaaaaaa


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#endif /* !__NX_PRISM_SIMPLEKEYBOARD_H__ */

