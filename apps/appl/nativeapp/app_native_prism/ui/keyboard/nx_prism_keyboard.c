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
 * @file	  		nx_prism_keyboard.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

#include <nx_main.h>
#include <nx_common.h>
#include <nx_core.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_keyboard.h>
#include "nx_prism_arabic_support.h"


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum
{
	eCHARSET_English,
	eCHARSET_German,
	eCHARSET_Turkish,
	eCHARSET_Russian,
	eCHARSET_Arabic,
	eCHARSET_French,
	eCHARSET_Italian,
	eCHARSET_Spanish,
	eCHARSET_Greek,
	eCHARSET_Persian,
	eCHARSET_Swedish,
	eCHARSET_Max
} NxKeyboard_Charset_e;

typedef struct _Key_Contents
{
	HINT32		lCurrentFocus_Row;  // current focus object
	HINT32		lCurrentFocus_Col;  // current focus object
	HINT32		lModeUpLow;			// upper or lower
	HINT32		lModeToggle;     	// character or extend or special
	HINT32		lCurrentPtr;      	// cursor point
	HINT32		lInputboxSize;    	//
	HINT32		lInputMode;
	HINT32		lCharCount;
	HINT32		lPwdOnOff;
	HINT32		lSmsInput;
	HINT32		lBeforeInput;
	HINT32		lIsEnglishArea;		// TRUE : hide language btn --  FALSE : display language btn

	NxKeyboard_Charset_e	language;

	HINT8		ucStrLang[NX_PRISM_KEYBOARD_BUFFSIZE];
	HINT8		ucInfoLang[NX_PRISM_KEYBOARD_BUFFSIZE];

#if defined(LOCAL_CONFIG_APUI_KEYBOARD_SUPPORT_LANGUAGE_BTN)
	HUINT8		szLanguageBtn[NX_PRISM_KEYBOARD_BUFFSIZE];
#endif

//	HCHAR		*inputboxStr;
	HxSTR_t		*strbuf;

	HINT8		*input1;
	HINT8		*input2;
	HINT8		*input3;

} NxKeyboard_Contents_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static const HCHAR	s_aucNumSet[NX_PRISM_KEYBOARD_NUMBER] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

static const size_t utf8_skip_data[NX_TEXT_SIZE_256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

static const HUINT16	s_ausNumericSet[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_NUM_BTN_COUNT][NX_PRISM_KEYBOARD_NUMERIC_COUNT] =
{
	//capital
	{
		{0x0026, 0x0040,0x005F,0x0030,0x0020}, // 0
		{0x0020, 0x002E,0x002C,0x0031,0x0020}, // 1
		{0x0041, 0x0042,0x0043,0x0032,0x0020}, // 2
		{0x0044, 0x0045,0x0046,0x0033,0x0020}, // 3
		{0x0047, 0x0048,0x0049,0x0034,0x0020}, // 4
		{0x004A, 0x004B,0x004C,0x0035,0x0020}, // 5
		{0x004D, 0x004E,0x004F,0x0036,0x0020}, // 6
		{0x0050, 0x0051,0x0052,0x0053,0x0037}, // 7
		{0x0054, 0x0055,0x0056,0x0038,0x0020}, // 8
		{0x0057, 0x0058,0x0059,0x005A,0x0039}  // 9
	},
	//small
	{
		{0x0026, 0x0040,0x005F,0x0030,0x0020}, // 0
		{0x0020, 0x002E,0x002C,0x0031,0x0020}, // 1
		{0x0061, 0x0062,0x0063,0x0032,0x0020}, // 2
		{0x0064, 0x0065,0x0066,0x0033,0x0020}, // 3
		{0x0067, 0x0068,0x0069,0x0034,0x0020}, // 4
		{0x006A, 0x006B,0x006C,0x0035,0x0020}, // 5
		{0x006D, 0x006E,0x006F,0x0036,0x0020}, // 6
		{0x0070, 0x0071,0x0072,0x0073,0x0037}, // 7
		{0x0074, 0x0075,0x0076,0x0038,0x0020}, // 8
		{0x0077, 0x0078,0x0079,0x007A,0x0039}  // 9
	}
};

// 0x0020 space value

static const HUINT16	s_ausCharSet_Eng[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
		//Capital
		0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A,
		0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054,
		0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x002B, 0x0060,
		0x0026, 0x20AC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0000, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},
	{
		//Small
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x002B, 0x0060,
		0x0026, 0x20AC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0000, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16	s_ausCharSet_Ger[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
		//Capital
		0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A,
		0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054,
		0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0020, 0x0020,
		//Extra
		0x00C4, 0x00C0, 0x00C9, 0x00D6, 0x00DF, 0x00DC, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x002B, 0x0060,
		0x0026, 0x20AC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0000, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},

	{
		//Small
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x0020, 0x0020,
		//Extra
		0x00E4, 0x00E0, 0x00E9, 0x00F6, 0x00F9, 0x00FC, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x002B, 0x0060,
		0x0026, 0x20AC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0000, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16	s_ausCharSet_Tur[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
		//Capital
		0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A,
		0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054,
		0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0020, 0x0020,
		//Extra
		0x00C2, 0x00C7, 0x011E, 0x0130, 0x00D6, 0x015E, 0x00DB, 0x00DC, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x002B, 0x0060,
		0x0026, 0x20AC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},

	{
		//Small
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x0020, 0x0020,
		//Extra
		0x00E2, 0x00E7, 0x011F, 0x0131, 0x00F6, 0x015F, 0x00FB, 0x00FC, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x002B, 0x0060,
		0x0026, 0x20AC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16	s_ausCharSet_Rus[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
	 	//Capital
		0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0401, 0x0416, 0x0417, 0x0418,
		0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 0x0420, 0x0421, 0x0422,
		0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A,
		//Capital Ext
		0x042B, 0x042C, 0x042D, 0x042E, 0x042F, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Extra
		0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409, 0x040A, 0x040B,
		0x040C, 0x040E, 0x040F, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020


	},

	{
	  	//Small
		0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0451, 0x0436, 0x0437, 0x0438,
		0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F, 0x0440, 0x0441, 0x0442,
		0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A,
		//Small Ext
		0x044B, 0x044C, 0x044D, 0x044E, 0x044F, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Extra
		0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, 0x0458, 0x0459, 0x045A, 0x045B,
		0x045C, 0x045E, 0x045F, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020

	}
};

static const HUINT16	s_ausCharSet_Ara[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
		//(Based on ISO 8859-6, 0x0621 ~ 0x063A)
		0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627, 0x0628, 0x0629, 0x062A,
		0x062B, 0x062C, 0x062D, 0x062E, 0x062F, 0x0630, 0x0631, 0x0632, 0x0633, 0x0634,
		0x0635, 0x0636, 0x0637, 0x0638, 0x0639, 0x063A, 0x0020, 0x0020,
		//(Based on ISO 8859-6, 0x0640 ~ 0x064A)
		0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647, 0x0648, 0x0649,
		0x064A,
		//(Points from ISO 8859-6, 0x064B ~ 0x0652)
		0x064B, 0x064C, 0x064E, 0x064F, 0x0650, 0x0651, 0x0652, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},

	{
		//(Based on ISO 8859-6, 0x0621 ~ 0x063A)
		0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627, 0x0628, 0x0629, 0x062A,
		0x062B, 0x062C, 0x062D, 0x062E, 0x062F, 0x0630, 0x0631, 0x0632, 0x0633, 0x0634,
		0x0635, 0x0636, 0x0637, 0x0638, 0x0639, 0x063A, 0x0020, 0x0020,
		//(Based on ISO 8859-6, 0x0640 ~ 0x064A)
		0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647, 0x0648, 0x0649,
		0x064A,
		//(Points from ISO 8859-6, 0x064B ~ 0x0652)
		0x064B, 0x064C, 0x064E, 0x064F, 0x0650, 0x0651, 0x0652, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16	s_ausCharSet_Per[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
		//(Based on ISO 8859-6, 0x0621 ~ 0x063A)
		0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627, 0x0628, 0x0629, 0x062A,
		0x062B, 0x062C, 0x062D, 0x062E, 0x062F, 0x0630, 0x0631, 0x0632, 0x0633, 0x0634,
		0x0635, 0x0636, 0x0637, 0x0638, 0x0639, 0x063A, 0x0020, 0x0020,
		//(Based on ISO 8859-6, 0x0640 ~ 0x064A)
		0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647, 0x0648, 0x0649,
		0x064A,
		//(Points from ISO 8859-6, 0x064B ~ 0x0652)
		0x064B, 0x064C, 0x064E, 0x064F, 0x0650, 0x0651, 0x0652, 0x067E, 0x0686,
		0x0698, 0x06AF, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},

	{
		//(Based on ISO 8859-6, 0x0621 ~ 0x063A)
		0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627, 0x0628, 0x0629, 0x062A,
		0x062B, 0x062C, 0x062D, 0x062E, 0x062F, 0x0630, 0x0631, 0x0632, 0x0633, 0x0634,
		0x0635, 0x0636, 0x0637, 0x0638, 0x0639, 0x063A, 0x0020, 0x0020,
		//(Based on ISO 8859-6, 0x0640 ~ 0x064A)
		0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647, 0x0648, 0x0649,
		0x064A,
		//(Points from ISO 8859-6, 0x064B ~ 0x0652)
		0x064B, 0x064C, 0x064E, 0x064F, 0x0650, 0x0651, 0x0652, 0x067E, 0x0686,
		0x0698, 0x06AF, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16	s_ausCharSet_Fre[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
		//Capital
		0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A,
		0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054,
		0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0020, 0x0020,
		//Extra
		0x00C0, 0x00C2, 0x00C6, 0x00C7, 0x00C9, 0x00C8, 0x00CA, 0x00CB, 0x00CE, 0x00CF,
		0x00D1, 0x00D4, 0x0152, 0x00D9, 0x00DB, 0x0178, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},

	{
		//Small
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x0020, 0x0020,
		//Extra
		0x00E0, 0x00E2, 0x00E6, 0x00E7, 0x00E9, 0x00E8, 0x00EA, 0x00EB, 0x00EE, 0x00EF,
		0x00F1, 0x00F4, 0x0153, 0x00F9, 0x00FB, 0x00FF, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16	s_ausCharSet_Ita[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
		//Capital
		0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A,
		0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054,
		0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0020, 0x0020,
		//Extra
		0x00C0, 0x00C9, 0x00C8, 0x00CC, 0x00D2, 0x00D9, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},

	{
		//Small
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x0020, 0x0020,
		//Extra
		0x00E0, 0x00E9, 0x00E8, 0x00EC, 0x00F2, 0x00F9, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16	s_ausCharSet_Spa[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
		//Capital
		0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A,
		0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054,
		0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0020, 0x0020,
		//Extra
		0x00C1, 0x00C0, 0x00C7, 0x00C9, 0x00C8, 0x00CD, 0x00CF, 0x00D1, 0x00D3, 0x00D2,
		0x00DA, 0x00DC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},

	{
		//Small
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x0020, 0x0020,
		//Extra
		0x00E1, 0x00E0, 0x00E7, 0x00E9, 0x00E8, 0x00ED, 0x00EF, 0x00F1, 0x00F3, 0x00F2,
		0x00FA, 0x00FC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16	s_ausCharSet_Gre[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
		//Capital
		0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039A,
		0x039B, 0x039C, 0x039D, 0x039E, 0x039F, 0x03A0, 0x03A1, 0x03A3, 0x03A4, 0x03A5,
		0x03A6, 0x03A7, 0x03A8, 0x03A9, 0x0020, 0x0020, 0x0020, 0x0020,
		//Extra
		0x0386, 0x0388, 0x0389, 0x038A, 0x03AA, 0x0390, 0x038C, 0x03C2, 0x038E, 0x03AB,
		0x03B0, 0x038F, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},

	{
		//Small
		0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7, 0x03B8, 0x03B9, 0x03BA,
		0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF, 0x03C0, 0x03C1, 0x03C3, 0x03C4, 0x03C5,
	 	0x03C6, 0x03C7, 0x03C8, 0x03C9, 0x0020, 0x0020, 0x0020, 0x0020,
		//Extra
		0x03AC, 0x03AD, 0x03AE, 0x03AF, 0x03CA, 0x0390, 0x03CC, 0x03C2, 0x03CD, 0x03CB,
		0x03B0, 0x03CE, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16	s_ausCharSet_Swe[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT] =
{
	{
		//Capital
		0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A,
		0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054,
		0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0020, 0x0020,
		//Extra
		0x00C1, 0x00C0, 0x00C6, 0x010C, 0x0110, 0x00C9, 0x00CF, 0x014A, 0x0160, 0x0166,
		0x00DC, 0x017D, 0x00C5, 0x00C4, 0x00D6, 0x00D8, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x002B, 0x0060,
		0x0026, 0x20AC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0000, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},

	{
		//Small
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x0020, 0x0020,
		//Extra
		0x00E1, 0x00E0, 0x00E6, 0x010D, 0x0111, 0x00E9, 0x00EF, 0x014B, 0x0161, 0x0167,
		0x00FC, 0x017E, 0x00E5, 0x00E4, 0x00F6, 0x00F8, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		//Special 1
		0x0040, 0x002D, 0x002E, 0x005F, 0x0021, 0x003F, 0x0025, 0x002A, 0x007E, 0x0024,
		0x0023, 0x002F, 0x007C, 0x005C, 0x005E, 0x003D, 0x2018, 0x2019, 0x0022, 0x002C,
		0x003A, 0x003B, 0x0028, 0x0029, 0x007B, 0x007D, 0x005B, 0x005D,
		//Special 2
		0x003C, 0x003E, 0x00BF, 0x00A1, 0x00A3, 0x00A4, 0x00A5, 0x00A7, 0x002B, 0x0060,
		0x0026, 0x20AC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,

		0x0000, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static HCHAR	*s_aucCharSet_Using[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT];
static HCHAR	*s_aucNumSet_Using[NX_PRISM_KEYBOARD_NUM_BTN_COUNT];
static HCHAR	*s_aucNumericSet_Using[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_NUM_BTN_COUNT][NX_PRISM_KEYBOARD_NUMERIC_COUNT];

static HUINT32 			s_ulCursorInterval = 0;
static HUINT32 			s_ulStartInterval = 0;
static HxLANG_Id_e		s_eCurKeyboardLang = eLangID_NONE;
static NxKeyboard_Contents_t	s_stNxKeyboard_Contents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NxKeyboard_Contents_t*	 nx_keyboard_KeyboardGetContents(void);

static HBOOL			nx_keyboard_IsEmptyString(NxKeyboard_Contents_t *pKeyContexts, HINT32 lRow, HINT32 lCol);
static HBOOL			nx_keyboard_FindNextMoveString(NxKeyboard_Contents_t *pKeyContexts, HINT32 lkey, HINT32 lRow,
														HINT32 lCol, HINT32 *lNextRow, HINT32 *lNextCol);

static ONDK_Result_t 	nx_keyboard_SetLanguage(NxKeyboard_Contents_t *pKeyContexts);
static ONDK_Result_t 	nx_keyboard_SetInputBoxString(NxKeyboard_Contents_t *pKeyContexts);
static ONDK_Result_t 	nx_keyboard_SettingUsingTable(NxKeyboard_Contents_t *pKeyContexts);
static HINT32 			nx_keyboard_GetByteSize(HxSTR_t *str, HINT32 start, HINT32 cnt);
static void 			nx_keyboard_SetCharSet(NxKeyboard_Contents_t *pSimpleKeyContexts);
static void 			nx_keyboard_SetNumSet(NxKeyboard_Contents_t *pKeyContexts);
static void				nx_keyboard_ChangeLanguage(NxKeyboard_Contents_t *pKeyContexts);
static void				nx_keyboard_SetCharButtonCount(NxKeyboard_Contents_t *pKeyContexts);
static void				nx_keyboard_ChangeLanguageForDefault(NxKeyboard_Contents_t *pKeyContexts);
static ONDK_Result_t	nx_keyboard_MoveButtonLeft(NxKeyboard_Contents_t *pKeyContexts, HINT32 lKey);
static ONDK_Result_t	nx_keyboard_MoveButtonRight(NxKeyboard_Contents_t *pKeyContexts, HINT32 lKey);
static ONDK_Result_t	nx_keyboard_MoveButtonUp(NxKeyboard_Contents_t *pKeyContexts, HINT32 lKey);
static ONDK_Result_t	nx_keyboard_MoveButtonDown(NxKeyboard_Contents_t *pKeyContexts, HINT32 lKey);
static ONDK_Result_t 	nx_keyboard_SetInputObject(NxKeyboard_Contents_t *pKeyContexts);
static ONDK_Result_t 	nx_keyboard_InitData(NxKeyboard_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_keyboard_MsgGwmCreate(NxKeyboard_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_keyboard_MsgGwmDestroy(NxKeyboard_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_keyboard_MsgGwmKeyDown(NxKeyboard_Contents_t *pKeyContexts, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_keyboard_MsgGwmTimer(NxKeyboard_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_keyboard_MsgRemoteApp(NxKeyboard_Contents_t *pKeyContexts, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxKeyboard_Contents_t*	 nx_keyboard_KeyboardGetContents(void)
{
	return &s_stNxKeyboard_Contents;
}


static HBOOL	nx_keyboard_IsEmptyString(NxKeyboard_Contents_t *pKeyContexts, HINT32 lRow, HINT32 lCol)
{
	HBOOL		bFound = FALSE;
	HINT32		lFindArray_0 = 0, lFindArray_1 = 0;

	lFindArray_0 = pKeyContexts->lModeUpLow;
	if (lRow == NX_PRISM_KEYBOARD_ROW_NUMBER)
	{
		lFindArray_1 = lCol;
		if (HxSTD_StrCmp((char*)s_aucNumericSet_Using[lFindArray_0][lFindArray_1], (char*)" ") == 0)
		{
			bFound = TRUE;
		}
		else
		{
			bFound = FALSE;
		}
	}
	else
	{
		lFindArray_1 = pKeyContexts->lModeToggle + (lRow - NX_PRISM_KEYBOARD_ROW_1) * NX_PRISM_KEYBOARD_COL_KEY_COUNT + lCol;
		if (HxSTD_StrCmp(s_aucCharSet_Using[lFindArray_0][lFindArray_1], (char*)" ") == 0)
		{
			bFound = TRUE;
		}
		else
		{
			bFound = FALSE;
		}
	}

	return	bFound;
}


static HBOOL	nx_keyboard_FindNextMoveString(NxKeyboard_Contents_t *pKeyContexts, HINT32 lkey, HINT32 lRow, HINT32 lCol, HINT32 *lNextRow, HINT32 *lNextCol)
{
	HBOOL		bFound = FALSE;
	HINT32		lFindArray_0 = 0, lFindArray_1 = 0, i = 0;


	if (lkey == KEY_ARROWLEFT)
	{
		for (i = lCol; i >= NX_PRISM_KEYBOARD_COL_0; i--)
		{
			lFindArray_0 = pKeyContexts->lModeUpLow;
			if (lRow == NX_PRISM_KEYBOARD_ROW_NUMBER)
			{
				lFindArray_1 = i;
				if (HxSTD_StrCmp((char*)s_aucNumericSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
				{
					*lNextRow = i;
					*lNextCol = lCol;
					bFound = TRUE;
					break;
				}
			}
			else
			{
				lFindArray_1 = pKeyContexts->lModeToggle + (lRow - NX_PRISM_KEYBOARD_ROW_1) * NX_PRISM_KEYBOARD_COL_KEY_COUNT + i;
				if (HxSTD_StrCmp(s_aucCharSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
				{
					*lNextRow = lRow;
					*lNextCol = i;
					bFound = TRUE;
					break;
				}
			}
		}

		if ((bFound == FALSE) && (i < 0))
		{
			*lNextRow = lRow;
			if ((lRow == NX_PRISM_KEYBOARD_ROW_ULCASE) || (lRow == NX_PRISM_KEYBOARD_ROW_SPCHAR))
			{
				*lNextCol = NX_PRISM_KEYBOARD_COL_SPCHAR;
				bFound = TRUE;
				return	bFound;
			}
			else
			{
				*lNextCol = NX_PRISM_KEYBOARD_COL_10;
				bFound = TRUE;
				return	bFound;
			}
		}

		if (bFound == FALSE)
		{
			for (i = lCol; i < NX_PRISM_KEYBOARD_COL_KEY_COUNT; i++)
			{
				lFindArray_0 = pKeyContexts->lModeUpLow;
				lFindArray_1 = pKeyContexts->lModeToggle + (lRow - NX_PRISM_KEYBOARD_ROW_1) * NX_PRISM_KEYBOARD_COL_KEY_COUNT + i;

				if (HxSTD_StrCmp(s_aucCharSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
				{
					*lNextRow = lRow;
					*lNextCol = i;
					bFound = TRUE;
					break;
				}
			}
		}

		if (bFound == FALSE)
		{
			*lNextRow = -1;
			*lNextCol = -1;
		}
	}
	else if (lkey == KEY_ARROWRIGHT)
	{
		for (i = lCol; i < NX_PRISM_KEYBOARD_COL_KEY_COUNT; i++)
		{
			lFindArray_0 = pKeyContexts->lModeUpLow;
			if (lRow == NX_PRISM_KEYBOARD_ROW_NUMBER)
			{
				lFindArray_1 = i;
				if (HxSTD_StrCmp((char*)s_aucNumericSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
				{
					*lNextRow = i;
					*lNextCol = lCol;
					bFound = TRUE;
					break;
				}
			}
			else
			{
				lFindArray_1 = pKeyContexts->lModeToggle + (lRow - NX_PRISM_KEYBOARD_ROW_1) * NX_PRISM_KEYBOARD_COL_KEY_COUNT + i;
				if (HxSTD_StrCmp(s_aucCharSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
				{
					*lNextRow = lRow;
					*lNextCol = i;
					bFound = TRUE;
					break;
				}
			}
		}

		if ((bFound == FALSE) && ((i == NX_PRISM_KEYBOARD_COL_ULCASE) || (i == NX_PRISM_KEYBOARD_COL_DELETE_0) || (i == NX_PRISM_KEYBOARD_COL_DELETE_ALL_0) || (i == NX_PRISM_KEYBOARD_COL_LANGUAGE_0)))
		{
			*lNextRow = lRow;
			*lNextCol = i;
			bFound = TRUE;
			return	bFound;
		}

		if (bFound == FALSE)
		{
			for (i = lCol; i >= 0; i--)
			{
				lFindArray_0 = pKeyContexts->lModeUpLow;
				lFindArray_1 = pKeyContexts->lModeToggle + (lRow - NX_PRISM_KEYBOARD_ROW_1) * NX_PRISM_KEYBOARD_COL_KEY_COUNT + i;
				if (HxSTD_StrCmp(s_aucCharSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
				{
					*lNextRow = lRow;
					*lNextCol = i;
					bFound = TRUE;
					break;
				}
			}
		}

		if (bFound == FALSE)
		{
			*lNextRow = -1;
			*lNextCol = -1;
		}
	}
	else if (lkey == KEY_ARROWUP)
	{
		for (i = lRow; i > NX_PRISM_KEYBOARD_ROW_0; i--)
		{
			if ((lRow == NX_PRISM_KEYBOARD_ROW_SPACE) && ((lCol == NX_PRISM_KEYBOARD_COL_SPACE_0) || (lCol == NX_PRISM_KEYBOARD_COL_SPACE_1)))
			{
				*lNextRow = i;
				*lNextCol = NX_PRISM_KEYBOARD_COL_SPACE_0;
				bFound = TRUE;
				break;
			}

			lFindArray_0 = pKeyContexts->lModeUpLow;
			lFindArray_1 = pKeyContexts->lModeToggle + (i - NX_PRISM_KEYBOARD_ROW_1) * NX_PRISM_KEYBOARD_COL_KEY_COUNT + lCol;
			if (HxSTD_StrCmp(s_aucCharSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
			{
				*lNextRow = i;
				*lNextCol = lCol;
				bFound = TRUE;
				break;
			}
		}

		if (bFound == FALSE)
		{
			for (i = lRow; i < NX_PRISM_KEYBOARD_ROW_INPUTBOX; i++)
			{
				lFindArray_0 = pKeyContexts->lModeUpLow;
				lFindArray_1 = pKeyContexts->lModeToggle + (i - NX_PRISM_KEYBOARD_ROW_1) * NX_PRISM_KEYBOARD_COL_KEY_COUNT + lCol;

				if (HxSTD_StrCmp(s_aucCharSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
				{
					*lNextRow = i;
					*lNextCol = lCol;
					bFound = TRUE;
					break;
				}
			}
		}

		if (bFound == FALSE)
		{
			*lNextRow = -1;
			*lNextCol = -1;
		}
	}
	else if (lkey == KEY_ARROWDOWN)
	{
		for (i = lRow; i < NX_PRISM_KEYBOARD_ROW_INPUTBOX; i++)
		{
			lFindArray_0 = pKeyContexts->lModeUpLow;
			if (i == NX_PRISM_KEYBOARD_ROW_NUMBER)
			{
				lFindArray_1 = lCol;
				if (HxSTD_StrCmp((char*)s_aucNumericSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
				{
					*lNextRow = i;
					*lNextCol = lCol;
					bFound = TRUE;
					break;
				}
			}
			else if ((i == NX_PRISM_KEYBOARD_ROW_SPACE) && ((lCol == NX_PRISM_KEYBOARD_COL_SPACE_0) || (lCol == NX_PRISM_KEYBOARD_COL_SPACE_1)))
			{
				*lNextRow = i;
				*lNextCol = NX_PRISM_KEYBOARD_COL_SPACE_0;
				bFound = TRUE;
				break;
			}
			else
			{
				lFindArray_1 = pKeyContexts->lModeToggle + (i - NX_PRISM_KEYBOARD_ROW_1) * NX_PRISM_KEYBOARD_COL_KEY_COUNT + lCol;
				if (HxSTD_StrCmp(s_aucCharSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
				{
					*lNextRow = i;
					*lNextCol = lCol;
					bFound = TRUE;
					break;
				}
			}
		}

		if (bFound == FALSE)
		{
			for (i = lRow; i > NX_PRISM_KEYBOARD_ROW_0; i--)
			{
				lFindArray_0 = pKeyContexts->lModeUpLow;
				lFindArray_1 = pKeyContexts->lModeToggle + (i - NX_PRISM_KEYBOARD_ROW_1) * NX_PRISM_KEYBOARD_COL_KEY_COUNT + lCol;
				if (HxSTD_StrCmp(s_aucCharSet_Using[lFindArray_0][lFindArray_1], (char*)" ") != 0)
				{
					*lNextRow = i;
					*lNextCol = lCol;
					bFound = TRUE;
					break;
				}
			}
		}

		if (bFound == FALSE)
		{
			*lNextRow = -1;
			*lNextCol = -1;
		}
	}
	else
	{
		*lNextRow = -1;
		*lNextCol = -1;
		bFound = FALSE;
	}

	return bFound;
}


static NxKeyboard_Charset_e		nx_keyboard_convertLanguageStringToType(HCHAR *pLangFullName)
{
	if (HxSTD_StrCmp(pLangFullName,"English")==0)		return eCHARSET_English;
	if (HxSTD_StrCmp(pLangFullName,"Scottish Gaelic")==0)		return eCHARSET_English;
	if (HxSTD_StrCmp(pLangFullName,"Irish")==0)		return eCHARSET_English;
	if (HxSTD_StrCmp(pLangFullName,"Welsh")==0)		return eCHARSET_English;
	else if(HxSTD_StrCmp(pLangFullName,"German")==0)	return eCHARSET_German;
	else if(HxSTD_StrCmp(pLangFullName,"Turkish")==0)	return eCHARSET_Turkish;
	else if(HxSTD_StrCmp(pLangFullName,"Russian")==0)	return eCHARSET_Russian;
	else if(HxSTD_StrCmp(pLangFullName,"Arabic")==0)	return eCHARSET_Arabic;
	else if(HxSTD_StrCmp(pLangFullName,"French")==0)	return eCHARSET_French;
	else if(HxSTD_StrCmp(pLangFullName,"Italian")==0)	return eCHARSET_Italian;
	else if(HxSTD_StrCmp(pLangFullName,"Spanish")==0)	return eCHARSET_Spanish;
	else if(HxSTD_StrCmp(pLangFullName,"Greek")==0)		return eCHARSET_Greek;
	else if(HxSTD_StrCmp(pLangFullName,"Persian")==0)	return eCHARSET_Persian;
	else if(HxSTD_StrCmp(pLangFullName,"Swedish")==0)	return eCHARSET_Swedish;

	else	return eCHARSET_Max;
}

static ONDK_Result_t	nx_keyboard_SetLanguage(NxKeyboard_Contents_t *pKeyContexts)
{
	HERROR 	hErr = ERR_OK;
	HCHAR	*pLangFullName = NULL;
	HCHAR	szLang639[8] ={0,};
	HxLANG_Id_e langId;

#if defined(LOCAL_CONFIG_APUI_KEYBOARD_SUPPORT_LANGUAGE_BTN)
	hErr = NX_PORT_SYSTEM_GetMainLanguage(szLang639);
	if(hErr != ERR_OK)
	{
//		ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
		return ONDK_RET_IO;
	}
#else
	HLIB_STD_StrNCpySafe((HCHAR*)szLang639, "eng", 4);
#endif
	langId = HLIB_LANG_639ToId(szLang639);
	pLangFullName = (HCHAR *)HLIB_LANG_IdToFullName(langId);
	if(pLangFullName == NULL)
	{
//		ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
		return ONDK_RET_IO;
	}

	// 처음 language 를 읽어와 저장하고, keyboard 에서 변경시 저장.
	// cursor 초기 위치 및 기타처리 등에 사용
	s_eCurKeyboardLang  = langId;

	HxSTD_StrNCpy(pKeyContexts->ucStrLang, pLangFullName, ONDK_LANGUAGE_STRING_MAX_LENGTH);
	pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';

	pKeyContexts->language = nx_keyboard_convertLanguageStringToType(pLangFullName);

//	ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_keyboard_SetInputBoxString(NxKeyboard_Contents_t *pKeyContexts)
{
	HINT32 			i = 0, j = 0, str_width = 0, len = 0, ptr_len = 0, str_len = 0;
	HINT32			lCurrentRow = 0, lCurrentCol = 0, total_len = 0;
	HxSTR_t			*pstTotal_str = NULL, *pstStrline = NULL;
	HUINT8			*pUtfStr = NULL, *pTemp	= NULL;
	HUINT8			tmpBuf[NX_PRISM_KEYBOARD_STRING_LENGTH] = {'\0',};
	size_t 			utf8_size;
	ONDK_Result_t		lRet = ONDK_RET_OK;
	ONDK_Rect_t			rect;
	ONDKFontHandle		fontTempHandle;
#if defined(CONFIG_PROD_LANG_ARABIC)
	HBOOL			bArabic = FALSE;
	HxSTR_t			*pAra = NULL, *pTempStr = NULL;
#endif


	ONDK_GWM_COM_Keyboard_SetInputBoxSize(NX_PRISM_KEYBOARD_KEY_OBJ, 1);

	lCurrentRow = ONDK_GWM_COM_Keyboard_GetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ);
	lCurrentCol = ONDK_GWM_COM_Keyboard_GetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ);

	pstTotal_str = HLIB_STR_Clone(pKeyContexts->strbuf);

	if (pstTotal_str == NULL)
	{
		HxLOG_Error("Failed Allocated!\n");
		return ONDK_RET_FAIL;
	}

	if (pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
	{
	}
#if defined(CONFIG_PROD_LANG_ARABIC)
	else if (NX_PRISM_ARABIC_SUPPORT_CheckUnicodeArabicCharacterInString(HLIB_STR_toUCS2(pstTotal_str)))
	{
		HxLOG_Debug("\n[%s.%d] Current string is Arabic ! \n\n",__FUNCTION__,__LINE__);
		bArabic = TRUE;
	}
#endif

	pUtfStr = HLIB_STR_GetBuffer(pstTotal_str);
	str_len = HxSTD_StrLen((HCHAR*)pUtfStr);
	fontTempHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);

	for (i = 0; (*pUtfStr != '\0') && i < 1; i++)
	{
		j = 0;
		str_width = 0;
		HxSTD_memset(tmpBuf, '\0', NX_PRISM_KEYBOARD_STRING_LENGTH);

		while( *pUtfStr != '\0' && str_width < NX_PRISM_KEYBOARD_MAX_INPUT_WIDTH )
		{
			utf8_size = utf8_skip_data[(int)*pUtfStr];

			if ((pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD) && (pKeyContexts->lPwdOnOff == 0))
			{
				switch(utf8_size) {
		            case 6: pUtfStr ++;
		            case 5: pUtfStr ++;
		            case 4: pUtfStr ++;
		            case 3: pUtfStr ++;
		            case 2: pUtfStr ++;
		            case 1: tmpBuf[j++] = '*';	pUtfStr ++;	break;
					default: tmpBuf[j++] = '*';	pUtfStr ++;	break;


	        	}
				str_width = ONDK_FONT_GetStringWidth(fontTempHandle, (HCHAR*)tmpBuf);
			}
			else if ((pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD) && (pKeyContexts->lPwdOnOff == 1))
			{
				if ((str_len - 1) == j)
				{
					switch(utf8_size) {
			            case 6: tmpBuf[j++] = *pUtfStr ++;
			            case 5: tmpBuf[j++] = *pUtfStr ++;
			            case 4: tmpBuf[j++] = *pUtfStr ++;
			            case 3: tmpBuf[j++] = *pUtfStr ++;
			            case 2: tmpBuf[j++] = *pUtfStr ++;
			            case 1: tmpBuf[j++] = *pUtfStr ++;	break;
						default: tmpBuf[j++] = *pUtfStr ++;	break;
	        		}
				}
				else
				{
					switch(utf8_size) {
			            case 6: pUtfStr ++;
			            case 5: pUtfStr ++;
			            case 4: pUtfStr ++;
			            case 3: pUtfStr ++;
			            case 2: pUtfStr ++;
			            case 1: tmpBuf[j++] = '*';	pUtfStr ++;	break;
						default: tmpBuf[j++] = '*';	pUtfStr ++;	break;
	        		}
				}
		#if defined(CONFIG_PROD_LANG_ARABIC)
				if (bArabic)
				{
					pAra = (HxSTR_t *)HLIB_STR_New(tmpBuf);
					if (pAra == NULL)
					{
						HxLOG_Error("Failed Allocated!\n");
						if(pstTotal_str)
							HLIB_STR_Delete(pstTotal_str);
						ONDK_FONT_Release(fontTempHandle);
						return ONDK_RET_FAIL;
					}
					pAra = (HxSTR_t *)NX_PRISM_ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(pAra);
					if (pAra)
					{
						str_width = ONDK_FONT_GetStringWidth(fontTempHandle, (HCHAR*)HLIB_STR_GetBuffer(pAra));
						HLIB_STR_Delete(pAra);
					}
				}
				else
		#endif
				{
					str_width = ONDK_FONT_GetStringWidth(fontTempHandle, (HCHAR*)tmpBuf);
				}
			}
			else if (pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_NORMAL)
			{
				switch (utf8_size) {
		            case 6: tmpBuf[j++] = *pUtfStr ++;
		            case 5: tmpBuf[j++] = *pUtfStr ++;
		            case 4: tmpBuf[j++] = *pUtfStr ++;
		            case 3: tmpBuf[j++] = *pUtfStr ++;
		            case 2: tmpBuf[j++] = *pUtfStr ++;
		            case 1: tmpBuf[j++] = *pUtfStr ++;	break;
					default: tmpBuf[j++] = *pUtfStr ++;	break;
	        	}
		#if defined(CONFIG_PROD_LANG_ARABIC)
				if (bArabic)
				{
					HxSTR_t *pAra = NULL;
					pAra = (HxSTR_t *)HLIB_STR_New(tmpBuf);
					if (pAra == NULL)
					{
						HxLOG_Error("Failed Allocated!\n");
						if(pstTotal_str)
						{
							HLIB_STR_Delete(pstTotal_str);
						}
						ONDK_FONT_Release(fontTempHandle);
						return ONDK_RET_FAIL;
					}
					pAra = (HxSTR_t *)NX_PRISM_ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(pAra);
					if (pAra)
					{
						str_width = ONDK_FONT_GetStringWidth(fontTempHandle, (HCHAR*)HLIB_STR_GetBuffer(pAra));
						if (pAra)
						{
							HLIB_STR_Delete(pAra);
						}
					}
				}
				else
		#endif
				{
					str_width = ONDK_FONT_GetStringWidth(fontTempHandle, (HCHAR*)tmpBuf);
				}
			}
		}// end while
		tmpBuf[j] = '\0';

		pTemp = (HUINT8 *)NX_APP_Malloc( (j + 1) * sizeof(HUINT8));
		if (pTemp != NULL)
		{
			// copy string of current line
			HxSTD_StrNCpy((HCHAR*)pTemp, (HCHAR*)tmpBuf , (sizeof(HUINT8) * (j + 1)) );
			pstStrline = HLIB_STR_New(pTemp);

			if (pstStrline == NULL)
			{
				if (pTemp)
					HLIB_MEM_Free(pTemp);
				continue;
			}

	#if defined(CONFIG_PROD_LANG_ARABIC)
			if (bArabic)
			{
				pTempStr = (HxSTR_t *)NX_PRISM_ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(pstStrline);
				if (pTempStr == NULL)
				{
					HxLOG_Debug("[%s,%d] Failed Convert To VisualAra!!\n",__FUNCTION__,__LINE__);
				}
				else
				{
					HxLOG_Debug("\n\n[%s,%d] Saved Visual Arabic !!!\n\n",__FUNCTION__,__LINE__);
					pstStrline = pTempStr;
				}
			}
	#endif
			switch(i)
			{
				case 0:			// input line 1
					HxSTD_MemSet(pKeyContexts->input1, '\0', (sizeof(HUINT8) * NX_PRISM_KEYBOARD_STRING_LENGTH));
					HxSTD_StrNCpy(pKeyContexts->input1, (char*)HLIB_STR_GetBuffer(pstStrline), HxSTD_StrLen((char*)HLIB_STR_GetBuffer(pstStrline)));
					pKeyContexts->input1[HxSTD_StrLen((char*)HLIB_STR_GetBuffer(pstStrline))] = '\0';
					break;
#if 0
				case 1:			// input line 2
					HxSTD_MemSet(pKeyContexts->input2, '\0', (sizeof(HUINT8) * NX_PRISM_KEYBOARD_STRING_LENGTH));
					HxSTD_StrNCpy(pKeyContexts->input2, (char*)HLIB_STR_GetBuffer(pstStrline), HxSTD_StrLen((char*)HLIB_STR_GetBuffer(pstStrline)));
					pKeyContexts->input2[HxSTD_StrLen((char*)HLIB_STR_GetBuffer(pstStrline))] = '\0';
					break;

				case 2:			// input line 3
					HxSTD_MemSet(pKeyContexts->input3, '\0', (sizeof(HUINT8) * NX_PRISM_KEYBOARD_STRING_LENGTH));
					HxSTD_StrNCpy(pKeyContexts->input3, (char*)HLIB_STR_GetBuffer(pstStrline), HxSTD_StrLen((char*)HLIB_STR_GetBuffer(pstStrline)));
					pKeyContexts->input3[HxSTD_StrLen((char*)HLIB_STR_GetBuffer(pstStrline))] = '\0';
					break;
				default:
					break;
#endif
			}

			// previous 나 after 나 값이 같아 무의미..
			//str_width = nx_keyboard_GetTotalStringWidth(pstStrline);
		#if 0
			if (bArabic)
			{
				rect.x = NX_PRISM_KEYBOARD_MAX_INPUT_WIDTH + 79 - str_width;

			}
			else
			{
				rect.x = AP_KEYBOARD_INPUTBOX3_X+19 + str_width;
			}
		#endif

			if (pstStrline)
				HLIB_STR_Delete(pstStrline);
		}
		else
		{
			HxLOG_Error("Failed Allocated!\n");
			if(pstTotal_str)
				HLIB_STR_Delete(pstTotal_str);
			ONDK_FONT_Release(fontTempHandle);
			return ONDK_RET_FAIL;
		}

		if (pTemp != NULL)
			HLIB_MEM_Free(pTemp);
	}

	ONDK_FONT_Release(fontTempHandle);
	ONDK_GWM_COM_Keyboard_SetInputBoxSize(NX_PRISM_KEYBOARD_KEY_OBJ, i );

	// edit input 창이 3개 만 지원 하되게 코드가 되어 잇네... ㅠ.ㅠ
	if (*pUtfStr == '\0' && pstTotal_str != NULL)
	{
		HxLOG_Debug("[%s,%d]Copy  pstTotal_str to pKeyContexts->strbuf. i : %d\n",__FUNCTION__,__LINE__, i);
		HLIB_STR_Delete(pstTotal_str);

		if (i == 0)
			ONDK_GWM_COM_Keyboard_SetInputBoxSize(NX_PRISM_KEYBOARD_KEY_OBJ, 1);
	}
	else if (i >= 3 && *pUtfStr != '\0')		// input 3 line 이 넘어가면 마지막 입력은 지움.. 흠...
	{
		HxLOG_Debug("[%s,%d] input size over 3\n",__FUNCTION__,__LINE__);
		if (pstTotal_str)
			HLIB_STR_Delete(pstTotal_str);

		len = nx_keyboard_GetByteSize(pKeyContexts->strbuf, 0, pKeyContexts->lCurrentPtr-1);
		ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetAt(pKeyContexts->strbuf, pKeyContexts->lCurrentPtr-1));
		total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pKeyContexts->strbuf));

		if (ptr_len == 2)
		{
			HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf, len);
			HLIB_STR_SetAtBuffer(pKeyContexts->strbuf, total_len-2, 0);
		}
		else
		{
			HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf, len);
			HLIB_STR_SetAtBuffer(pKeyContexts->strbuf, total_len-1, 0);
		}

		pKeyContexts->lCurrentPtr--;
	}
	else
	{
		HxLOG_Debug("\n\n[%s,%d] what the.....\n\n",__FUNCTION__,__LINE__);		// 이안에 들어오면 머지...?
	}

#if defined(CONFIG_PROD_LANG_ARABIC)
	if(bArabic)
	{
		ONDK_GWM_COM_Text_SetAlign(NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID, TEXT_ALIGN_RIGHT);
//		ONDK_GWM_COM_Text_SetAlign(NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID, TEXT_ALIGN_RIGHT);
//		ONDK_GWM_COM_Text_SetAlign(NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID, TEXT_ALIGN_RIGHT);
	}
	else
#endif
	{
		ONDK_GWM_COM_Text_SetAlign(NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID, TEXT_ALIGN_LEFT);
//		ONDK_GWM_COM_Text_SetAlign(NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID, TEXT_ALIGN_LEFT);
//		ONDK_GWM_COM_Text_SetAlign(NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID, TEXT_ALIGN_LEFT);
	}

	//... 가슴이 아프넹...ㅠ.ㅠ
	switch(i)
	{
		case 1:
			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID, pKeyContexts->input1);
//			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID, "");
//			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID, "");
			break;

		case 2:
			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID, pKeyContexts->input2);
//			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID, pKeyContexts->input1);
//			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID, "");
			break;

		case 3:
			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID, pKeyContexts->input3);
//			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID, pKeyContexts->input2);
//			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID, pKeyContexts->input1);
			break;

		default:
			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID, "");
//			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID, "");
//			ONDK_GWM_COM_Text_SetText(NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID, "");
			rect.x = NX_PRISM_KEYBOARD_INPUTBOX3_X+19;
			break;
	}

	// align cursor
	if ((s_eCurKeyboardLang == eLangID_Arabic) || (s_eCurKeyboardLang == eLangID_Persian))
	{
		if (str_width == 0)
		{	// string 이 없는 경우 default 처리
			rect.x = NX_PRISM_KEYBOARD_MAX_INPUT_WIDTH + 79 - str_width;
		}
		else
		{
	#if defined(CONFIG_PROD_LANG_ARABIC)
			if (bArabic != TRUE)  // keyboard 상태가 arabic 인데도 영어만 포함된 경우.
				rect.x = NX_PRISM_KEYBOARD_INPUTBOX3_X+19 + str_width;
			else
	#endif
			{
				rect.x = NX_PRISM_KEYBOARD_MAX_INPUT_WIDTH + 79 - str_width;
			}
		}
	}
	else
	{
		if (str_width == 0)
		{	// string 이 없는 경우 default 처리
			rect.x = NX_PRISM_KEYBOARD_INPUTBOX3_X+19 + str_width;
		}
		else
		{
	#if defined(CONFIG_PROD_LANG_ARABIC)
			if (bArabic == TRUE)  // keyboard 상태가 arabic 아니지만, string 에 arabic 가 포함된경우.
				rect.x = NX_PRISM_KEYBOARD_MAX_INPUT_WIDTH + 79 - str_width;
			else
	#endif
			{
				rect.x = NX_PRISM_KEYBOARD_INPUTBOX3_X+19 + str_width;
			}
		}
	}

	// TODO: Multi Line일 경우 Position 변경이 필요.
	rect.y = NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_Y;
	rect.w = NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_W;
	rect.h = NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_H;
	ONDK_GWM_Obj_Resize(NX_PRISM_KEYBOARD_IMG_INPUTBOX_CURSOR_ID, &rect);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return lRet;
}

static ONDK_Result_t nx_keyboard_SettingUsingTable(NxKeyboard_Contents_t *pKeyContexts)
{
	ONDK_Result_t             lRet = ONDK_RET_OK;
	HUINT8                    utfItem[8] = {0, };
	HUINT16                   ucsitem[2] = {0, };
	HINT32			len = 0;
	HINT32			i = 0, j = 0, k = 0;
	HBOOL			bUpperCase = FALSE;
	HINT16			tmp_Charset[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT];
	HINT16			tmp_Numericset[NX_PRISM_KEYBOARD_CHARSET_COUNT][NX_PRISM_KEYBOARD_NUM_BTN_COUNT][NX_PRISM_KEYBOARD_NUMERIC_COUNT];

	pKeyContexts->lModeToggle = 0;
	pKeyContexts->lModeUpLow = 1;

	bUpperCase = (pKeyContexts->lModeUpLow == 1) ? FALSE : TRUE;
	ONDK_GWM_COM_Keyboard_SetUppercaseAndLowercaseLetters(NX_PRISM_KEYBOARD_KEY_OBJ, bUpperCase);

	for (i = 0; i < NX_PRISM_KEYBOARD_CHARSET_COUNT; i++)
	{
         for (j = 0; j < (NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT); j++)
         {
#if defined(CONFIG_PROD_LANG_ENGLISH)
			if(pKeyContexts->language == eCHARSET_English)
			{
				tmp_Charset[i][j] = s_ausCharSet_Eng[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_GERMAN)
			else if(pKeyContexts->language == eCHARSET_German)
			{
			    tmp_Charset[i][j] = s_ausCharSet_Ger[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_TURKISH)
			else if(pKeyContexts->language == eCHARSET_Turkish)
			{
			    tmp_Charset[i][j] = s_ausCharSet_Tur[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_RUSSIAN)
			else if(pKeyContexts->language == eCHARSET_Russian)
			{
			    tmp_Charset[i][j] = s_ausCharSet_Rus[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_ARABIC)
			else if(pKeyContexts->language == eCHARSET_Arabic)
			{
			    tmp_Charset[i][j] = s_ausCharSet_Ara[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_PERSIAN)
			else if(pKeyContexts->language == eCHARSET_Persian)
			{
			    tmp_Charset[i][j] = s_ausCharSet_Per[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_FRENCH)
			else if(pKeyContexts->language == eCHARSET_French)
			{
			    tmp_Charset[i][j] = s_ausCharSet_Fre[i][j];
	        }
#endif
#if defined(CONFIG_PROD_LANG_ITALIAN)
			else if(pKeyContexts->language == eCHARSET_Italian)
			{
				tmp_Charset[i][j] = s_ausCharSet_Ita[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_SPANISH)
			else if(pKeyContexts->language == eCHARSET_Spanish)
			{
				tmp_Charset[i][j] = s_ausCharSet_Spa[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_GREEK)
			else if(pKeyContexts->language == eCHARSET_Greek)
			{
				tmp_Charset[i][j] = s_ausCharSet_Gre[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_SWEDISH)
			else if(pKeyContexts->language == eCHARSET_Swedish)
			{
				tmp_Charset[i][j] = s_ausCharSet_Swe[i][j];
			}
#endif
			else
			{
				//NX_APP_Debug("=======> Error nx_keyboard_SettingUsingTable : %s\n", pKeyContexts->language);
			}
         }
	}

	for ( i = 0; i < NX_PRISM_KEYBOARD_CHARSET_COUNT; i++)
	{
		for (j = 0; j < (NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT); j++)
        {
			if (tmp_Charset[i][j] == 0x0020)
			{
				utfItem[0] = ' ';
				utfItem[1] = '\0';
			}
            else
			{
				HxSTD_MemCopy(ucsitem,&tmp_Charset[i][j],sizeof(HUINT16));
				HxSTD_MemSet(utfItem,0,sizeof(HUINT8) * 8);

				len = HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8,(HUINT16 *)ucsitem,(void *)utfItem, 8);
				//alen = HLIB_CP_NumOfBytes (eHxCP_UNICODE_UTF8, utfItem);
				//ONDK_Debug("[%s:%d] len(%d) utfItem(, 0x%02d, 0x%02d, 0x%02d, 0x%02d)\r\n", __FUNCTION__, __LINE__, len, utfItem[0], utfItem[1], utfItem[2], utfItem[3]);
			}

			HxSTD_StrNCpy((HCHAR*)s_aucCharSet_Using[i][j], (HCHAR*)utfItem, HxSTD_StrLen((HCHAR*)utfItem));
			s_aucCharSet_Using[i][j][HxSTD_StrLen((HCHAR*)utfItem)] = '\0';
		}
	}

	for (k = 0; k < NX_PRISM_KEYBOARD_CHARSET_COUNT; k++)
	{
		for (i = 0; i < NX_PRISM_KEYBOARD_NUM_BTN_COUNT; i++)
		{
			for (j = 0; j < NX_PRISM_KEYBOARD_NUMERIC_COUNT; j++)
		    {
				tmp_Numericset[k][i][j] = s_ausNumericSet[k][i][j];
			}
		}
	}

	for (k = 0; k < NX_PRISM_KEYBOARD_CHARSET_COUNT; k++)
	{
		for (i = 0; i < NX_PRISM_KEYBOARD_NUM_BTN_COUNT; i++)
		{
			for (j= 0; j < NX_PRISM_KEYBOARD_NUMERIC_COUNT; j++)
		    {
				if (tmp_Numericset[k][i][j] == 0x0020)
				{
					utfItem[0] = ' ';
					utfItem[1] = '\0';
				}
		        else
				{
					HxSTD_MemCopy(ucsitem,&tmp_Numericset[k][i][j],sizeof(HUINT16));
					HxSTD_MemSet(utfItem,0,sizeof(HUINT8) * 8);

					len = HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8,(HUINT16 *)ucsitem,(void *)utfItem, 8);
				}

				HxSTD_StrNCpy((HCHAR*)s_aucNumericSet_Using[k][i][j], (HCHAR*)utfItem, HxSTD_StrLen((HCHAR*)utfItem));
				s_aucNumericSet_Using[k][i][j][HxSTD_StrLen((HCHAR*)utfItem)] = '\0';
			}
		}
	}

	//num table setting
	for (i = 0; i < NX_PRISM_KEYBOARD_NUM_BTN_COUNT; i++)
	{
		s_aucNumSet_Using[i][0] = s_aucNumSet[i];
		s_aucNumSet_Using[i][1] = '\0';
	}

	return lRet;
}


static HINT32 nx_keyboard_GetByteSize(HxSTR_t *str, HINT32 start, HINT32 cnt)
{
	HINT32	lRet = 0, i = 0;

	for (i = start; i < cnt; i++)
	{
		lRet += HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(str,i));
	}

	return lRet;
}

static void 	nx_keyboard_SetCharSet(NxKeyboard_Contents_t *pKeyContexts)
{
	HINT32	i = 0, len = 0;

	for (i = 0; i < NX_PRISM_KEYBOARD_CHAR_BTN_COUNT; i++)
	{
		ONDK_GWM_COM_Keyboard_SetCharValue(NX_PRISM_KEYBOARD_KEY_OBJ, i, s_aucCharSet_Using[pKeyContexts->lModeUpLow][i+pKeyContexts->lModeToggle]);
	}

#if 0
	len = HxSTD_StrLen(s_aucCharSet_Using[pKeyContexts->lModeUpLow][0]);
	if(len < NX_PRISM_KEYBOARD_BUFFSIZE)
	{
		HxSTD_StrNCpy(pKeyContexts->ucInfoLang,s_aucCharSet_Using[pKeyContexts->lModeUpLow][0], len);
		pKeyContexts->ucInfoLang[HxSTD_StrLen(s_aucCharSet_Using[pKeyContexts->lModeUpLow][0])] = '\0';
	}
	else
	{
		HxLOG_Error("Cannot copy a key array to pKeyContexts->ucInfoLang !!!");
		return;
	}

	len = HxSTD_StrLen(pKeyContexts->ucInfoLang)+HxSTD_StrLen(s_aucCharSet_Using[pKeyContexts->lModeUpLow][1]);
	if(len < NX_PRISM_KEYBOARD_BUFFSIZE)
	{
		HxSTD_StrNCat(pKeyContexts->ucInfoLang,s_aucCharSet_Using[pKeyContexts->lModeUpLow][1],HxSTD_StrLen(s_aucCharSet_Using[pKeyContexts->lModeUpLow][1]));
		len = HxSTD_StrLen(pKeyContexts->ucInfoLang)+HxSTD_StrLen(s_aucCharSet_Using[pKeyContexts->lModeUpLow][1]);
		if (len >=NX_PRISM_KEYBOARD_BUFFSIZE) len = NX_PRISM_KEYBOARD_BUFFSIZE-1;
		pKeyContexts->ucInfoLang[len] = '\0';
	}
	else return;

	len = HxSTD_StrLen(pKeyContexts->ucInfoLang)+HxSTD_StrLen(s_aucCharSet_Using[pKeyContexts->lModeUpLow][2]);
	if(len < NX_PRISM_KEYBOARD_BUFFSIZE)
	{
		HxSTD_StrNCat(pKeyContexts->ucInfoLang,s_aucCharSet_Using[pKeyContexts->lModeUpLow][2],HxSTD_StrLen(s_aucCharSet_Using[pKeyContexts->lModeUpLow][2]));
		len = HxSTD_StrLen(pKeyContexts->ucInfoLang)+HxSTD_StrLen(s_aucCharSet_Using[pKeyContexts->lModeUpLow][2]);
		if (len >=NX_PRISM_KEYBOARD_BUFFSIZE) len = NX_PRISM_KEYBOARD_BUFFSIZE-1;
		pKeyContexts->ucInfoLang[len] = '\0';
	}
	else return;

	len = HxSTD_StrLen(pKeyContexts->ucInfoLang)+HxSTD_StrLen(" - ");
	if(len < NX_PRISM_KEYBOARD_BUFFSIZE)
	{
		HxSTD_StrNCat(pKeyContexts->ucInfoLang," - ",HxSTD_StrLen(" - "));
		len = HxSTD_StrLen(pKeyContexts->ucInfoLang)+HxSTD_StrLen(" - ");
		if(len >= NX_PRISM_KEYBOARD_BUFFSIZE) len = NX_PRISM_KEYBOARD_BUFFSIZE-1;
		pKeyContexts->ucInfoLang[len] = '\0';
	}
	else return;
#endif
	len = HxSTD_StrLen(pKeyContexts->ucStrLang);
	if (len < NX_PRISM_KEYBOARD_BUFFSIZE)
	{
		ONDK_GWM_COM_Keyboard_SetLanguage(NX_PRISM_KEYBOARD_KEY_OBJ, pKeyContexts->ucStrLang);
	}
}

static void 		nx_keyboard_SetNumSet(NxKeyboard_Contents_t *pKeyContexts)
{
	HINT32	i = 0;

	for (i = 0 ; i < NX_PRISM_KEYBOARD_NUM_BTN_COUNT; i++)
	{
		ONDK_GWM_COM_Keyboard_SetNumValue(NX_PRISM_KEYBOARD_KEY_OBJ, i, s_aucNumSet_Using[i]);
	}

	NX_COMMON_NOT_USED_PARAM(pKeyContexts);
}

static void		nx_keyboard_SetCharButtonCount(NxKeyboard_Contents_t *pKeyContexts)
{
	if(pKeyContexts->language == eCHARSET_English)
	{
		if(pKeyContexts->lModeToggle > NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*2)
			pKeyContexts->lModeToggle = 0 ;
	}
	else if(pKeyContexts->language == eCHARSET_German ||
		pKeyContexts->language == eCHARSET_Turkish ||
		pKeyContexts->language == eCHARSET_French ||
		pKeyContexts->language == eCHARSET_Italian ||
		pKeyContexts->language == eCHARSET_Spanish ||
		pKeyContexts->language == eCHARSET_Greek ||
		pKeyContexts->language == eCHARSET_Swedish)
	{
		if(pKeyContexts->lModeToggle > NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*3)
			pKeyContexts->lModeToggle = 0 ;
	}
	else if(pKeyContexts->language == eCHARSET_Russian)
	{
		if(pKeyContexts->lModeToggle > NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*4)
			pKeyContexts->lModeToggle = 0 ;
	}
	else if(pKeyContexts->language == eCHARSET_Arabic)
	{
		if(pKeyContexts->lModeToggle > NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*1)
			pKeyContexts->lModeToggle = 0 ;
	}
	else if(pKeyContexts->language == eCHARSET_Persian)
	{
		if(pKeyContexts->lModeToggle > NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*1)
			pKeyContexts->lModeToggle = 0 ;
	}
	else
	{
		if(pKeyContexts->lModeToggle > NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*4)
			pKeyContexts->lModeToggle = 0 ;
	}
}


static void		nx_keyboard_ChangeLanguage(NxKeyboard_Contents_t *pKeyContexts)
{
	nx_keyboard_ChangeLanguageForDefault(pKeyContexts);
}


static ONDK_Result_t	nx_keyboard_MoveButtonLeft(NxKeyboard_Contents_t *pKeyContexts, HINT32 lKey)
{
	HINT32		lKeyId = lKey;
	HINT32		lCurrentRow = 0, lCurrentCol = 0, lNextRow = 0, lNextCol = 0, lPrevRow = 0, lPrevCol = 0;
	HBOOL		bEmptyStr = FALSE, bFoundStr = FALSE;

	lCurrentRow = ONDK_GWM_COM_Keyboard_GetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ);
	lCurrentCol = ONDK_GWM_COM_Keyboard_GetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ);

	lPrevRow = lCurrentRow;
	lPrevCol = lCurrentCol;

	switch (lCurrentRow)
	{
		case NX_PRISM_KEYBOARD_ROW_INPUTBOX:
			if (pKeyContexts->lCurrentPtr > 0)
			{
				pKeyContexts->lCurrentPtr--;
				nx_keyboard_SetInputBoxString(pKeyContexts);
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_ENTER:	// Enter Btn
			// Set Focus Input Box
			lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
			lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
			break;

		case NX_PRISM_KEYBOARD_ROW_1:	// a ~ j, a/A, a/@
			lCurrentCol--;
			if (lCurrentCol < NX_PRISM_KEYBOARD_COL_0)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_SPCHAR;
			}
			if ((lCurrentCol >= NX_PRISM_KEYBOARD_COL_0) && (lCurrentCol <= NX_PRISM_KEYBOARD_COL_9))
			{
				bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
				if (bEmptyStr == TRUE)
				{
					bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
					if (bFoundStr == TRUE)
					{
						lCurrentCol = lNextCol;
					}
					else
					{
						lCurrentCol = NX_PRISM_KEYBOARD_COL_SPCHAR;
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_2:	// k ~ t, Delete(<-)
			lCurrentCol--;
			if (lCurrentCol < NX_PRISM_KEYBOARD_COL_0)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_DELETE_0;
			}
			if ((lCurrentCol >= NX_PRISM_KEYBOARD_COL_0) && (lCurrentCol <= NX_PRISM_KEYBOARD_COL_9))
			{
				bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
				if (bEmptyStr == TRUE)
				{
					bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
					if (bFoundStr == TRUE)
					{
						lCurrentCol = lNextCol;
					}
					else
					{
						lCurrentCol = NX_PRISM_KEYBOARD_COL_DELETE_0;
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_3:	// u ~ z, Empty 0, Empty 1, Space, X
			lCurrentCol--;
			if (lCurrentCol < NX_PRISM_KEYBOARD_COL_0)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_DELETE_ALL_0;
			}

			if (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_0)
			{
				lCurrentCol--;
			}

			if (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_1)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_SPACE_0;
			}

			if ((lCurrentCol >= NX_PRISM_KEYBOARD_COL_0) && (lCurrentCol <= NX_PRISM_KEYBOARD_COL_7))
			{
				bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
				if (bEmptyStr == TRUE)
				{
					bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
					if (bFoundStr == TRUE)
					{
						lCurrentCol = lNextCol;
					}
					else
					{
						lCurrentCol = NX_PRISM_KEYBOARD_COL_DELETE_ALL_0;
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_4:	// 0 ~ 9, Language
			lCurrentCol--;
			if (lCurrentCol < NX_PRISM_KEYBOARD_COL_0)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_LANGUAGE_0;
			}

			if ((lCurrentCol >= NX_PRISM_KEYBOARD_COL_0) && (lCurrentCol <= NX_PRISM_KEYBOARD_COL_9))
			{
				bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
				if (bEmptyStr == TRUE)
				{
					bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
					if (bFoundStr == TRUE)
					{
						lCurrentCol = lNextCol;
					}
					else
					{
						lCurrentCol = NX_PRISM_KEYBOARD_COL_LANGUAGE_0;
					}
				}
			}
			break;
	}

	pKeyContexts->lCurrentFocus_Col = lCurrentCol;
	pKeyContexts->lCurrentFocus_Row = lCurrentRow;
	ONDK_GWM_COM_Keyboard_SetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ, lCurrentCol);
	ONDK_GWM_COM_Keyboard_SetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ, lCurrentRow);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return ONDK_MESSAGE_CONSUMED;
}


static ONDK_Result_t	nx_keyboard_MoveButtonRight(NxKeyboard_Contents_t *pKeyContexts, HINT32 lKey)
{
	HINT32		lKeyId = lKey;
	HINT32		lCurrentRow = 0, lCurrentCol = 0, lNextRow = 0, lNextCol = 0, lPrevRow = 0, lPrevCol = 0;
	HBOOL		bEmptyStr = FALSE, bFoundStr = FALSE;

	lCurrentRow = ONDK_GWM_COM_Keyboard_GetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ);
	lCurrentCol = ONDK_GWM_COM_Keyboard_GetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ);

	lPrevRow = lCurrentRow;
	lPrevCol = lCurrentCol;

	switch (lCurrentRow)
	{
		case NX_PRISM_KEYBOARD_ROW_INPUTBOX:
			if (pKeyContexts->lCurrentPtr < HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pKeyContexts->strbuf)))
			{
				pKeyContexts->lCurrentPtr++;
				nx_keyboard_SetInputBoxString(pKeyContexts);
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_ENTER:
			lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
			lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
			break;

		case NX_PRISM_KEYBOARD_ROW_1:	// a ~ j, a/A, a/@
			lCurrentCol++;
			if (lCurrentCol > NX_PRISM_KEYBOARD_COL_SPCHAR)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
			}

			if ((lCurrentCol >= NX_PRISM_KEYBOARD_COL_0) && (lCurrentCol <= NX_PRISM_KEYBOARD_COL_9))
			{
				bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
				if (bEmptyStr == TRUE)
				{
					bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
					if (bFoundStr == TRUE)
					{
						lCurrentCol = lNextCol;
					}
					else
					{
						lCurrentCol = NX_PRISM_KEYBOARD_COL_ULCASE;
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_2:	// k ~ t, Delete(<-)
			lCurrentCol++;
			if (lCurrentCol > NX_PRISM_KEYBOARD_COL_DELETE_0)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
			}

			if ((lCurrentCol >= NX_PRISM_KEYBOARD_COL_0) && (lCurrentCol <= NX_PRISM_KEYBOARD_COL_9))
			{
				bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
				if (bEmptyStr == TRUE)
				{
					bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
					if (bFoundStr == TRUE)
					{
						lCurrentCol = lNextCol;
					}
					else
					{
						lCurrentCol = NX_PRISM_KEYBOARD_COL_DELETE_0;
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_3:	// u ~ z, Empty 0, Empty 1, Space(|___|), Delete ALL(X)
			lCurrentCol++;
			if (lCurrentCol > NX_PRISM_KEYBOARD_COL_DELETE_ALL_0)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
			}

			if (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_0)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_SPACE_0;
			}
			else if (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_1)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_DELETE_ALL_0;
			}
			else
			{
				// Continue
			}

			if ((lCurrentCol >= NX_PRISM_KEYBOARD_COL_0) && (lCurrentCol <= NX_PRISM_KEYBOARD_COL_7))
			{
				bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
				if (bEmptyStr == TRUE)
				{
					bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
					if (bFoundStr == TRUE)
					{
						lCurrentCol = lNextCol;
						if (lCurrentCol >= NX_PRISM_KEYBOARD_COL_SPACE_0)
						{
							lCurrentCol = NX_PRISM_KEYBOARD_COL_SPACE_0;
						}
					}
					else
					{
						lCurrentCol = NX_PRISM_KEYBOARD_COL_SPACE_0;
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_4:	// 0 ~ 9, Language
			lCurrentCol++;
			if (lCurrentCol > NX_PRISM_KEYBOARD_COL_LANGUAGE_0)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
			}

			if ((lCurrentCol >= NX_PRISM_KEYBOARD_COL_0) && (lCurrentCol <= NX_PRISM_KEYBOARD_COL_9))
			{
				bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
				if (bEmptyStr == TRUE)
				{
					bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
					if (bFoundStr == TRUE)
					{
						lCurrentCol = lNextCol;
					}
					else
					{
						lCurrentCol = NX_PRISM_KEYBOARD_COL_LANGUAGE_0;
					}
				}
			}
			break;
	}

	pKeyContexts->lCurrentFocus_Col = lCurrentCol;
	pKeyContexts->lCurrentFocus_Row = lCurrentRow;
	ONDK_GWM_COM_Keyboard_SetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ, lCurrentCol);
	ONDK_GWM_COM_Keyboard_SetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ, lCurrentRow);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return ONDK_MESSAGE_CONSUMED;
}


static ONDK_Result_t	nx_keyboard_MoveButtonUp(NxKeyboard_Contents_t *pKeyContexts, HINT32 lKey)
{
	HINT32		lKeyId = lKey;
	HINT32		lCurrentRow = 0, lCurrentCol = 0, lNextRow = 0, lNextCol = 0, lPrevRow = 0, lPrevCol = 0;
	HBOOL		bEmptyStr = FALSE, bFoundStr = FALSE;

	lCurrentRow = ONDK_GWM_COM_Keyboard_GetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ);
	lCurrentCol = ONDK_GWM_COM_Keyboard_GetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ);
	if (lCurrentCol < NX_PRISM_KEYBOARD_COL_0)
	{
		NX_APP_Error("Error!!! ONDK_GWM_COM_Keyboard_GetCurFocus_Col() Fail!!\n");
		lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
	}

	lPrevRow = lCurrentRow;
	lPrevCol = lCurrentCol;

	switch (lCurrentRow)
	{
		case NX_PRISM_KEYBOARD_ROW_INPUTBOX:	// Input Box
			// Set Focus Number Row
			lCurrentRow = NX_PRISM_KEYBOARD_ROW_4;
			if (lCurrentCol >= NX_PRISM_KEYBOARD_COL_KEY_COUNT)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_ENTER:	// Enter BTN
			// Set Focus Language
			lCurrentRow = NX_PRISM_KEYBOARD_ROW_4;
			lCurrentCol = NX_PRISM_KEYBOARD_COL_LANGUAGE_0;
			break;

		case NX_PRISM_KEYBOARD_ROW_1:	// a ~ j, a/A, a/@
			if ((lCurrentCol == NX_PRISM_KEYBOARD_COL_ULCASE) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPCHAR))
			{
				// Set Focus Enter
				lCurrentRow = NX_PRISM_KEYBOARD_ROW_ENTER;
				lCurrentCol = NX_PRISM_KEYBOARD_COL_ENTER;
			}
			else
			{
				// Set Focus Input BOX
				lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_2:	// k ~ t, Delete(<-)
			if (lCurrentCol == NX_PRISM_KEYBOARD_COL_DELETE_0)
			{
				// Set Focus U&L Case
				lCurrentRow--;
				lCurrentCol = NX_PRISM_KEYBOARD_COL_ULCASE;
			}
			else
			{
				lCurrentRow--;
				if ((lCurrentRow >= NX_PRISM_KEYBOARD_ROW_1) && (lCurrentRow <= NX_PRISM_KEYBOARD_ROW_4))
				{
					bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
					if (bEmptyStr == TRUE)
					{
						bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
						if (bFoundStr == TRUE)
						{
							lCurrentRow = lNextRow;
						}
						else
						{
							lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
						}
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_3:	// u ~ z, Space(|___|), Delete All(X)
			if (lCurrentCol == NX_PRISM_KEYBOARD_COL_DELETE_ALL_0)
			{
				// Set Focus U&L Case
				lCurrentRow--;
				lCurrentCol = NX_PRISM_KEYBOARD_COL_DELETE_0;
			}
			else
			{
				lCurrentRow--;
				if ((lCurrentRow >= NX_PRISM_KEYBOARD_ROW_1) && (lCurrentRow <= NX_PRISM_KEYBOARD_ROW_4))
				{
					bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
					if (bEmptyStr == TRUE)
					{
						bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
						if (bFoundStr == TRUE)
						{
							lCurrentRow = lNextRow;
						}
						else
						{
							lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
						}
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_4:	// 0 ~ 9, Language
			if (lCurrentCol == NX_PRISM_KEYBOARD_COL_LANGUAGE_0)
			{
				// Set Focus U&L Case
				lCurrentRow--;
				lCurrentCol = NX_PRISM_KEYBOARD_COL_DELETE_ALL_0;
			}
			else
			{
				lCurrentRow--;
				if ((lCurrentRow >= NX_PRISM_KEYBOARD_ROW_1) && (lCurrentRow <= NX_PRISM_KEYBOARD_ROW_4))
				{
					bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
					if (bEmptyStr == TRUE)
					{
						bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
						if (bFoundStr == TRUE)
						{
							lCurrentRow = lNextRow;
							if ((lCurrentRow == NX_PRISM_KEYBOARD_ROW_SPACE) && ((lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_1)))
							{
								lCurrentCol = lNextCol;
							}
						}
						else
						{
							lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
							lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
						}
					}
					else
					{
						if ((lCurrentRow == NX_PRISM_KEYBOARD_ROW_SPACE) && ((lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_1)))
						{
							lCurrentCol = NX_PRISM_KEYBOARD_COL_SPACE_0;
						}
					}
				}
			}
			break;
	}

	pKeyContexts->lCurrentFocus_Col = lCurrentCol;
	pKeyContexts->lCurrentFocus_Row = lCurrentRow;
	ONDK_GWM_COM_Keyboard_SetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ, lCurrentRow);
	ONDK_GWM_COM_Keyboard_SetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ, lCurrentCol);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return ONDK_MESSAGE_CONSUMED;
}


static ONDK_Result_t	nx_keyboard_MoveButtonDown(NxKeyboard_Contents_t *pKeyContexts, HINT32 lKey)
{
	HINT32		lKeyId = lKey;
	HINT32		lCurrentRow = 0, lCurrentCol = 0, lNextRow = 0, lNextCol = 0, lPrevRow = 0, lPrevCol = 0;
	HBOOL		bEmptyStr = FALSE, bFoundStr = FALSE;

	lCurrentRow = ONDK_GWM_COM_Keyboard_GetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ);
	lCurrentCol = ONDK_GWM_COM_Keyboard_GetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ);

	if (lCurrentRow < 0)
	{
		NX_APP_Error("Error!!! Invalid Cursor Index lCurrentRow:(%d) Fail!! \n", lCurrentRow);
		lCurrentRow = NX_PRISM_KEYBOARD_ROW_1;
	}

	if (lCurrentCol < 0)
	{
		NX_APP_Error("Error!!! Invalid Cursor Index lCurrentCol:(%d) Fail!! \n", lCurrentCol);
		lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
	}

	lPrevRow = lCurrentRow;
	lPrevCol = lCurrentCol;

	switch (lCurrentRow)
	{
		case NX_PRISM_KEYBOARD_ROW_INPUTBOX:
			// Set Focus a ~ j, a/A, a/@
			lCurrentRow = NX_PRISM_KEYBOARD_ROW_1;
			if (lCurrentCol >= NX_PRISM_KEYBOARD_COL_KEY_COUNT)
			{
				lCurrentCol = NX_PRISM_KEYBOARD_COL_0;
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_ENTER:
			if (lCurrentCol == NX_PRISM_KEYBOARD_COL_ENTER)
			{
				// Set Focus U&L Case
				lCurrentRow++;
				lCurrentCol = NX_PRISM_KEYBOARD_COL_ULCASE;
			}
			else
			{
				lCurrentRow++;
				if ((lCurrentRow >= NX_PRISM_KEYBOARD_ROW_1) && (lCurrentRow <= NX_PRISM_KEYBOARD_ROW_4))
				{
					bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
					if (bEmptyStr == TRUE)
					{
						bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
						if (bFoundStr == TRUE)
						{
							lCurrentRow = lNextRow;
						}
						else
						{
							lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
						}
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_1:		// a ~ j, a/A, a/@
			if ((lCurrentCol == NX_PRISM_KEYBOARD_COL_ULCASE) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPCHAR))
			{
				// Set Focus Delete(<-)
				lCurrentRow++;
				lCurrentCol = NX_PRISM_KEYBOARD_COL_DELETE_0;
			}
			else
			{
				lCurrentRow++;
				if ((lCurrentRow >= NX_PRISM_KEYBOARD_ROW_1) && (lCurrentRow <= NX_PRISM_KEYBOARD_ROW_4))
				{
					bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
					if (bEmptyStr == TRUE)
					{
						bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
						if (bFoundStr == TRUE)
						{
							lCurrentRow = lNextRow;
							if ((lCurrentRow == NX_PRISM_KEYBOARD_ROW_SPACE) && ((lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_1)))
							{
								lCurrentCol = lNextCol;
							}
						}
						else
						{
							lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
						}
					}
				}
			}
			break;
		case NX_PRISM_KEYBOARD_ROW_2:		// k ~ t, Delete(<-)
			if ((lCurrentCol == NX_PRISM_KEYBOARD_COL_DELETE_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_DELETE_1))
			{
				// Set Focus Delete All(X)
				lCurrentRow++;
				lCurrentCol = NX_PRISM_KEYBOARD_COL_DELETE_ALL_0;
			}
			else
			{
				lCurrentRow++;
				if ((lCurrentRow >= NX_PRISM_KEYBOARD_ROW_1) && (lCurrentRow <= NX_PRISM_KEYBOARD_ROW_4))
				{
					bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
					if (bEmptyStr == TRUE)
					{
						bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
						if (bFoundStr == TRUE)
						{
							lCurrentRow = lNextRow;
						}
						else
						{
							lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
						}
					}
					else
					{
						if ((lCurrentRow == NX_PRISM_KEYBOARD_ROW_SPACE) && ((lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_1)))
						{
							lCurrentCol = NX_PRISM_KEYBOARD_COL_SPACE_0;
						}
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_3:		// u ~ z, Space(|___|), Delete All(X)
			if ((lCurrentCol == NX_PRISM_KEYBOARD_COL_DELETE_ALL_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_DELETE_ALL_1))
			{
				// Set Focus Language
				lCurrentRow++;
				lCurrentCol = NX_PRISM_KEYBOARD_COL_LANGUAGE_0;
			}
			else
			{
				lCurrentRow++;
				if ((lCurrentRow >= NX_PRISM_KEYBOARD_ROW_1) && (lCurrentRow <= NX_PRISM_KEYBOARD_ROW_4))
				{
					bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
					if (bEmptyStr == TRUE)
					{
						bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
						if (bFoundStr == TRUE)
						{
							lCurrentRow = lNextRow;
						}
						else
						{
							lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
						}
					}
				}
			}
			break;

		case NX_PRISM_KEYBOARD_ROW_4:
			if ((lCurrentCol == NX_PRISM_KEYBOARD_COL_LANGUAGE_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_LANGUAGE_1))
			{
				// Set Focus Enter
				lCurrentRow = NX_PRISM_KEYBOARD_ROW_ENTER;
				lCurrentCol = NX_PRISM_KEYBOARD_COL_ENTER;
			}
			else
			{
				lCurrentRow++;
#if 0
				if ((lCurrentRow >= NX_PRISM_KEYBOARD_ROW_1) && (lCurrentRow <= NX_PRISM_KEYBOARD_ROW_4))
				{
					bEmptyStr = nx_keyboard_IsEmptyString(pKeyContexts, lCurrentRow, lCurrentCol);
					if (bEmptyStr == TRUE)
					{
						bFoundStr = nx_keyboard_FindNextMoveString(pKeyContexts, lKeyId, lCurrentRow, lCurrentCol, &lNextRow, &lNextCol);
						if (bFoundStr == TRUE)
						{
							lCurrentRow = lNextRow;
						}
						else
						{
							lCurrentRow = NX_PRISM_KEYBOARD_ROW_INPUTBOX;
						}
					}
				}
#endif
			}
			break;
	}

	pKeyContexts->lCurrentFocus_Col = lCurrentCol;
	pKeyContexts->lCurrentFocus_Row = lCurrentRow;
	ONDK_GWM_COM_Keyboard_SetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ, lCurrentRow);
	ONDK_GWM_COM_Keyboard_SetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ, lCurrentCol);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return ONDK_MESSAGE_CONSUMED;
}


static void		nx_keyboard_ChangeLanguageForDefault(NxKeyboard_Contents_t *pKeyContexts)
{
	HBOOL		bFoundNextCharSet = FALSE;
	NxKeyboard_Charset_e	eNextLanguage = pKeyContexts->language + 1;

	while (pKeyContexts->language != eNextLanguage)
	{
		if(eNextLanguage == eCHARSET_Max)	eNextLanguage = eCHARSET_English;

		switch (eNextLanguage)
		{
			case eCHARSET_English:
#if defined(CONFIG_PROD_LANG_ENGLISH)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "English", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_English;
#endif
				break;
			case eCHARSET_German:
#if defined(CONFIG_PROD_LANG_GERMAN)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "German", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_German;
#endif
				break;
			case eCHARSET_Turkish:
#if defined(CONFIG_PROD_LANG_TURKISH)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "Turkish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_Turkish;
#endif
				break;
			case eCHARSET_Russian:
#if defined(CONFIG_PROD_LANG_RUSSIAN)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "Russian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_Russian;
#endif
				break;
			case eCHARSET_Arabic:
#if defined(CONFIG_PROD_LANG_ARABIC)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "Arabic", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_Arabic;
#endif
				break;
			case eCHARSET_French:
#if defined(CONFIG_PROD_LANG_FRENCH)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "French", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_French;
#endif
				break;
			case eCHARSET_Italian:
#if defined(CONFIG_PROD_LANG_ITALIAN)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "Italian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_Italian;
#endif
				break;
			case eCHARSET_Spanish:
#if defined(CONFIG_PROD_LANG_SPANISH)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "Spanish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_Spanish;
#endif
				break;
			case eCHARSET_Greek:
#if defined(CONFIG_PROD_LANG_GREEK)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "Greek", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_Greek;
#endif
				break;
			case eCHARSET_Persian:
#if defined(CONFIG_PROD_LANG_PERSIAN)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "Persian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_Persian;
#endif
				break;
			case eCHARSET_Swedish:
#if defined(CONFIG_PROD_LANG_SWEDISH)
				HxSTD_StrNCpy(pKeyContexts->ucStrLang, "Swedish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->ucStrLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_eCurKeyboardLang = eLangID_Swedish;
#endif
				break;

			default :
				break;
		}

		if(bFoundNextCharSet)
		{
			break;
		}
		else
		{
			eNextLanguage++;
		}

	}

	pKeyContexts->language = eNextLanguage;
	return;

}


static ONDK_Result_t 	nx_keyboard_SetInputObject(NxKeyboard_Contents_t *pKeyContexts)
{
	ONDK_Rect_t 		stRect;
	ONDK_Result_t		lRet = ONDK_RET_OK;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_KEYBOARD_WINDOW_X, NX_PRISM_KEYBOARD_WINDOW_Y,
						NX_PRISM_KEYBOARD_WINDOW_W, NX_PRISM_KEYBOARD_WINDOW_H);

	/* Draw Background */
	stRect = ONDK_Rect(NX_PRISM_KEYBOARD_FRAME_X, NX_PRISM_KEYBOARD_FRAME_Y, NX_PRISM_KEYBOARD_FRAME_W, NX_PRISM_KEYBOARD_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_KEYBOARD_FRAME_ID, &stRect, COL(C_G_Bang_90));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_KEYBOARD_FRAME_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG_TopLine);

	/* Keyboard Rect */
	stRect = ONDK_Rect(NX_PRISM_KEYBOARD_FRAME_X, NX_PRISM_KEYBOARD_FRAME_Y, NX_PRISM_KEYBOARD_FRAME_W, NX_PRISM_KEYBOARD_FRAME_H);
#if defined(LOCAL_CONFIG_APUI_KEYBOARD_SUPPORT_LANGUAGE_BTN)
	ONDK_GWM_COM_Keyboard_Create(NX_PRISM_KEYBOARD_KEY_OBJ, &stRect, 0, pKeyContexts->lIsEnglishArea, (HCHAR*)pKeyContexts->szLanguageBtn);
#else
	ONDK_GWM_COM_Keyboard_Create(NX_PRISM_KEYBOARD_KEY_OBJ, &stRect, 0, pKeyContexts->lIsEnglishArea, NULL);
#endif

	/* Input Box */
	stRect = ONDK_Rect(NX_PRISM_KEYBOARD_INPUT_BOX_X, NX_PRISM_KEYBOARD_INPUT_BOX_Y,
						NX_PRISM_KEYBOARD_INPUT_BOX_W, NX_PRISM_KEYBOARD_INPUT_BOX_H);
	ONDK_GWM_COM_Keyboard_SetInputBoxRect(NX_PRISM_KEYBOARD_KEY_OBJ, &stRect);

	/* Key BTN Rect */
	stRect = ONDK_Rect(NX_PRISM_KEYBOARD_KEY_BTN_X, NX_PRISM_KEYBOARD_KEY_BTN_Y,
						NX_PRISM_KEYBOARD_KEY_BTN_W, NX_PRISM_KEYBOARD_KEY_BTN_H);
	ONDK_GWM_COM_Keyboard_SetBtnKeyRect(NX_PRISM_KEYBOARD_KEY_OBJ, &stRect);

	/* Action BTN Rect */
	stRect = ONDK_Rect(NX_PRISM_KEYBOARD_ACTION_BTN_X, NX_PRISM_KEYBOARD_ACTION_BTN_Y,
						NX_PRISM_KEYBOARD_ACTION_BTN_W, NX_PRISM_KEYBOARD_ACTION_BTN_H);
	ONDK_GWM_COM_Keyboard_SetBtnActRect(NX_PRISM_KEYBOARD_KEY_OBJ, &stRect);

	/* BTN Color */
	ONDK_GWM_COM_Keyboard_SetBtnChangeColor(NX_PRISM_KEYBOARD_KEY_OBJ, COL(C_G_Gong01), COL(C_T_Gong02), COL(C_G_Base01_100), COL(C_T_Bang_100));

	ONDK_GWM_COM_Keyboard_SetImageBack(NX_PRISM_KEYBOARD_KEY_OBJ, RES_102_00_BTN_SS_BACK_C_PNG, RES_102_00_BTN_SS_BACK_N_PNG);
	ONDK_GWM_COM_Keyboard_SetImageClear(NX_PRISM_KEYBOARD_KEY_OBJ, RES_102_00_BTN_SS_DELETE_C_PNG, RES_102_00_BTN_SS_DELETE_N_PNG);
	ONDK_GWM_COM_Keyboard_SetImageSpace(NX_PRISM_KEYBOARD_KEY_OBJ, RES_102_00_BTN_SS_SPACE_C_PNG, RES_102_00_BTN_SS_SPACE_N_PNG);

	nx_keyboard_SetCharSet(pKeyContexts);
	nx_keyboard_SetNumSet(pKeyContexts);

	/* Enter Button String */
	// TODO: [JHLEE] Please Change String OK -> Enter
	ONDK_GWM_COM_Keyboard_SetEnterBtnStr(NX_PRISM_KEYBOARD_KEY_OBJ, (HCHAR*)ONDK_GetString(RES_OK_ID));

	if (pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
	{
		/* Keyboard Title Message */
		stRect = ONDK_Rect(NX_PRISM_KEYBOARD_MSG_TEXT_X, NX_PRISM_KEYBOARD_MSG_TEXT_Y,
							NX_PRISM_KEYBOARD_MSG_TEXT_W, NX_PRISM_KEYBOARD_MSG_TEXT_H);
		ONDK_GWM_COM_Text_Create(NX_PRISM_KEYBOARD_MSG_TEXT, &stRect, (HCHAR*)ONDK_GetString(RES_ENTER_YOUR_PASSWORD_ID));
		ONDK_GWM_COM_Text_SetSize(NX_PRISM_KEYBOARD_MSG_TEXT, NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_KEYBOARD_MSG_TEXT, COL(C_T_Yoyo01), COL(C_T_Bang_100));
		ONDK_GWM_COM_Text_SetFont(NX_PRISM_KEYBOARD_MSG_TEXT, eFont_SystemNormal);
	}

	//inputbox1 text
	stRect = ONDK_Rect(NX_PRISM_KEYBOARD_INPUTBOX1_X, NX_PRISM_KEYBOARD_INPUTBOX1_Y, NX_PRISM_KEYBOARD_MAX_INPUT_WIDTH, NX_PRISM_KEYBOARD_INPUT_BOX_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID, &stRect, "");
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID, eFont_SystemBold);

	// TODO: [JHLEE] 왜?  3개나 만들었을 까요?
#if 0
	//inputbox2 text
	stRect = ONDK_Rect(NX_PRISM_KEYBOARD_INPUTBOX2_X, NX_PRISM_KEYBOARD_INPUTBOX2_Y, NX_PRISM_KEYBOARD_MAX_INPUT_WIDTH, NX_PRISM_KEYBOARD_INPUT_BOX_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID, &stRect, "");
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID, eFont_SystemBold);

	//inputbox3 text
	stRect = ONDK_Rect(NX_PRISM_KEYBOARD_INPUTBOX3_X, NX_PRISM_KEYBOARD_INPUTBOX3_Y, NX_PRISM_KEYBOARD_MAX_INPUT_WIDTH, NX_PRISM_KEYBOARD_INPUT_BOX_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID, &stRect, "");
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID, eFont_SystemBold);
#endif

	//inputbox cursor
	stRect = ONDK_Rect(NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_X, NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_Y, NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_W, NX_PRISM_KEYBOARD_INPUTBOX_CURSOR_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_KEYBOARD_IMG_INPUTBOX_CURSOR_ID, &stRect, COL(C_G_Base01_100));

	ONDK_GWM_Obj_SetHide((NX_PRISM_KEYBOARD_INPUTBOX1_TEXT_ID),FALSE);
//	ONDK_GWM_Obj_SetHide((NX_PRISM_KEYBOARD_INPUTBOX2_TEXT_ID),FALSE);
//	ONDK_GWM_Obj_SetHide((NX_PRISM_KEYBOARD_INPUTBOX3_TEXT_ID),FALSE);

	ONDK_GWM_COM_Keyboard_SetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ, pKeyContexts->lCurrentFocus_Row);
	ONDK_GWM_COM_Keyboard_SetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ, pKeyContexts->lCurrentFocus_Col);

	return lRet;

}


static ONDK_Result_t 	nx_keyboard_InitData(NxKeyboard_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t lRet = ONDK_RET_OK;
	HUINT8			*str = NULL;
	HINT32	i,j,k;

//	pKeyContexts->inputboxStr = NX_APP_Malloc(sizeof(HCHAR)*NX_PRISM_KEYBOARD_STRING_LENGTH);
//	HxSTD_MemSet(pKeyContexts->inputboxStr, 0, sizeof(pKeyContexts->inputboxStr));

	pKeyContexts->lModeToggle = 0;
	pKeyContexts->lModeUpLow = 1;

	pKeyContexts->lCurrentFocus_Row = NX_PRISM_KEYBOARD_ROW_1;
	pKeyContexts->lCurrentFocus_Col = NX_PRISM_KEYBOARD_COL_0;

	pKeyContexts->lCurrentPtr = 0;
	pKeyContexts->lCharCount = 0;

	pKeyContexts->lInputboxSize = 1;
	pKeyContexts->lInputMode = p2;
	pKeyContexts->lPwdOnOff = 1;
//	pKeyContexts->speed = 0;

#if defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_HDR4000T)
	pKeyContexts->lIsEnglishArea = TRUE;
#else
	pKeyContexts->lIsEnglishArea = FALSE;
#endif
/*
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_OP_MIDDLE_EAST)
	pKeyContexts->lIsEnglishArea = FALSE;
#else
	pKeyContexts->lIsEnglishArea = TRUE;
#endif
*/

	for (i = 0; i < NX_PRISM_KEYBOARD_CHARSET_COUNT; i++)
	{
		for (j = 0; j < (NX_PRISM_KEYBOARD_CHAR_BTN_COUNT*NX_PRISM_KEYBOARD_KEYPADSET_COUNT); j++)
		{
			s_aucCharSet_Using[i][j] = NX_APP_Malloc(sizeof(HUINT8)*8);
			HxSTD_MemSet(s_aucCharSet_Using[i][j], 0, sizeof(HUINT8)*8);
		}
	}

	//num array setting
	for (i = 0; i < NX_PRISM_KEYBOARD_NUM_BTN_COUNT; i++)
	{
		s_aucNumSet_Using[i] = NX_APP_Malloc(sizeof(HUINT8)*8);
		HxSTD_MemSet(s_aucNumSet_Using[i], 0, sizeof(HUINT8)*8);
	}

	//numeric array setting
	for (k = 0; k < NX_PRISM_KEYBOARD_CHARSET_COUNT; k++)
	{
		for (i = 0; i < NX_PRISM_KEYBOARD_NUM_BTN_COUNT; i++)
		{
			for (j = 0; j < NX_PRISM_KEYBOARD_NUMERIC_COUNT; j++)
			{
				s_aucNumericSet_Using[k][i][j] = NX_APP_Malloc(sizeof(HUINT8)*8);
				HxSTD_MemSet(s_aucNumericSet_Using[k][i][j], 0, sizeof(HUINT8)*8);
			}
		}
	}

	if ((void *)p3 == NULL)
	{
		pKeyContexts->strbuf = HLIB_STR_New((HUINT8*)"");
	}
	else
	{
		pKeyContexts->strbuf = HLIB_STR_New((HUINT8 *)p3);
		pKeyContexts->lCurrentPtr = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pKeyContexts->strbuf));
	}

	pKeyContexts->input1 = NX_APP_Malloc(sizeof(HINT8)*NX_PRISM_KEYBOARD_STRING_LENGTH);
	HxSTD_MemSet(pKeyContexts->input1, 0, sizeof(HINT8)*NX_PRISM_KEYBOARD_STRING_LENGTH);

	pKeyContexts->input2 = NX_APP_Malloc(sizeof(HINT8)*NX_PRISM_KEYBOARD_STRING_LENGTH);
	HxSTD_MemSet(pKeyContexts->input2, 0, sizeof(HINT8)*NX_PRISM_KEYBOARD_STRING_LENGTH);

	pKeyContexts->input3 = NX_APP_Malloc(sizeof(HINT8)*NX_PRISM_KEYBOARD_STRING_LENGTH);
	HxSTD_MemSet(pKeyContexts->input3, 0, sizeof(HINT8)*NX_PRISM_KEYBOARD_STRING_LENGTH);

#if defined(LOCAL_CONFIG_APUI_KEYBOARD_SUPPORT_LANGUAGE_BTN)
	str = ONDK_GetString("STR_LANGUAGE_ID");
	if(str == NULL)
	{
		HxSTD_snprintf((char*)pKeyContexts->szLanguageBtn, sizeof(pKeyContexts->szLanguageBtn)-1, (char*)"%s", (char*)"LANGUAGE");
	}
	else
	{
		HxSTD_snprintf((char*)pKeyContexts->szLanguageBtn, sizeof(pKeyContexts->szLanguageBtn)-1, (char*)"%s", (char*)str);
	}
#endif

	NX_COMMON_NOT_USED_PARAM(p1);

	return lRet;
}


static ONDK_Result_t	nx_keyboard_MsgGwmCreate(NxKeyboard_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		hRet = ONDK_RET_OK;
	s_ulStartInterval = VK_TIMER_GetSystemTick();

	hRet = nx_keyboard_InitData(pKeyContexts, p1,p2,p3);
	hRet = nx_keyboard_SetLanguage(pKeyContexts);

	hRet = nx_keyboard_SettingUsingTable(pKeyContexts);
	nx_keyboard_SetCharButtonCount(pKeyContexts);

	hRet = nx_keyboard_SetInputObject(pKeyContexts);

	if(pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
		pKeyContexts->lPwdOnOff = 0;

	hRet = nx_keyboard_SetInputBoxString(pKeyContexts);

	ONDK_GWM_SetTimer(NX_PRISM_KEYBOARD_TIMER_ID, NX_PRISM_KEYBOARD_CURSOR_INTERVAL);

	//if(pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
	//	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW, 0);

	//nx_keyboard_SetInputBoxString(pKeyContexts);

//	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
/*	ONDK_GWM_APP_MoveAppArea(0,300,1279,719);
	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_MOVE, 20);
*/
	return hRet;
}


static ONDK_Result_t	nx_keyboard_MsgGwmDestroy(NxKeyboard_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pKeyContexts->strbuf)
	{
		HLIB_STR_Delete(pKeyContexts->strbuf);
		pKeyContexts->strbuf = NULL;
	}

	if (pKeyContexts->input1){		NX_APP_Free(pKeyContexts->input1);		pKeyContexts->input1 = NULL;	}
	if (pKeyContexts->input2){		NX_APP_Free(pKeyContexts->input2);		pKeyContexts->input2 = NULL;	}
	if (pKeyContexts->input3){		NX_APP_Free(pKeyContexts->input3);		pKeyContexts->input3 = NULL;	}


	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_keyboard_MsgGwmKeyDown(NxKeyboard_Contents_t *pKeyContexts, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1, len = 0, ptr_len = 0, total_len = 0, i = 0;
	HINT32		lCurrentRow = 0, lCurrentCol = 0, lFindArray_0 = 0, lFindArray_1 = 0;
	HUINT32		ulSystemTick = 0;
	HUINT8		inputStr[2] = {0,};
	HBOOL		bUpperCase = FALSE;
	HInputEvent_t	*inputEvent = (HInputEvent_t *)hHandle;

	static HINT32	numericKeyValue = -1 ;
	static HUINT32 s_limitedInterval = 0;
	static HUINT32 s_numericInterval = 0;
	ONDK_GWM_GuiObject_t cursor_obj = NULL;

	lCurrentRow = ONDK_GWM_COM_Keyboard_GetCurFocus_Row(NX_PRISM_KEYBOARD_KEY_OBJ);
	lCurrentCol = ONDK_GWM_COM_Keyboard_GetCurFocus_Col(NX_PRISM_KEYBOARD_KEY_OBJ);
	cursor_obj = ONDK_GWM_Obj_GetObjectByID(NX_PRISM_KEYBOARD_IMG_INPUTBOX_CURSOR_ID);

/*
	if(pKeyContexts->lInputMode == KEYBOARD_INPUT_PASSWD)
	{
		pKeyContexts->lPwdOnOff = 0;
		nx_keyboard_SetInputBoxString(pKeyContexts);
		ONDK_GWM_KillTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
	}
*/

	ulSystemTick = VK_TIMER_GetSystemTick();
	if ((s_ulCursorInterval + NX_PRISM_KEYBOARD_CURSOR_INTERVAL) < ulSystemTick)
	{
		s_ulCursorInterval = VK_TIMER_GetSystemTick();
		if (ONDK_GWM_Obj_GetVisibleByObj(cursor_obj) == TRUE)
		{
			ONDK_GWM_Obj_SetVisibleByObj(cursor_obj,FALSE);
		}
		else
		{
			ONDK_GWM_Obj_SetVisibleByObj(cursor_obj,TRUE);
		}
	}

	ulSystemTick = VK_TIMER_GetSystemTick();
	if (((s_limitedInterval + NX_PRISM_KEYBOARD_LIMITED_INTERVAL) > ulSystemTick) || ((s_ulStartInterval + 500) > ulSystemTick))
	{
		s_limitedInterval = VK_TIMER_GetSystemTick();
		return ONDK_MESSAGE_CONSUMED;
	}

/******************************************************************************************************************
여기서 사용하는 row 및 col 값은 keyboard UI 상 Language 버튼을 row 0 으로 시작하고 아래로 내려갈수록 1씩 증가 한다.
col 값의 지정은 가장 좌측을 0으로 가져가는데 Long 버튼의 경우는 2씩 증가하는 것으로 하고 떨어져서 시작하는 것은
해당 빈칸 만큼 더해서 시작된다.

Language = (0,8) ,,  a-A = (1,0) ,, a-@ = (1,2) back = (1,4) clear = (1,6) ok = (1,8)

예외적으로 input box는 다른 값을 갖게 하기 위하여 row가 6으로 간주하여 작성함.
*******************************************************************************************************************/

	ONDK_Assert(pKeyContexts);
	switch (lKeyId)
	{
		case KEY_ARROWLEFT:
			nx_keyboard_MoveButtonLeft(pKeyContexts, lKeyId);
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWRIGHT:
			nx_keyboard_MoveButtonRight(pKeyContexts, lKeyId);
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWUP:
			nx_keyboard_MoveButtonUp(pKeyContexts, lKeyId);
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWDOWN:
			nx_keyboard_MoveButtonDown(pKeyContexts, lKeyId);
			return ONDK_MESSAGE_CONSUMED;

		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_BACK:
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_CONSUMED;

		/* Button Action */
		case KEY_UNI_RETURN:
			{
				if (lCurrentRow == NX_PRISM_KEYBOARD_ROW_ENTER)	// Enter
				{
					ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_RESULT, (Handle_t)NULL, 0, 0, 0);
					ONDK_GWM_APP_Destroy(0);

					return ONDK_MESSAGE_CONSUMED;
				}
				else if (lCurrentRow == NX_PRISM_KEYBOARD_ROW_INPUTBOX)	// Input Box
				{
					return ONDK_MESSAGE_CONSUMED;
				}
				else if ((lCurrentRow == NX_PRISM_KEYBOARD_ROW_ULCASE) && (lCurrentCol== NX_PRISM_KEYBOARD_COL_ULCASE))	// a/A
				{
					pKeyContexts->lModeUpLow = (pKeyContexts->lModeUpLow == 0) ? 1 : 0 ;
					bUpperCase = (pKeyContexts->lModeUpLow == 1) ? FALSE : TRUE;
					ONDK_GWM_COM_Keyboard_SetUppercaseAndLowercaseLetters(NX_PRISM_KEYBOARD_KEY_OBJ, bUpperCase);
					nx_keyboard_SetCharSet(pKeyContexts);
				}
				else if ((lCurrentRow == NX_PRISM_KEYBOARD_ROW_SPCHAR) && (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPCHAR))	// a/@
				{
					pKeyContexts->lModeToggle += NX_PRISM_KEYBOARD_CHAR_BTN_COUNT;
					nx_keyboard_SetCharButtonCount(pKeyContexts);
					nx_keyboard_SetCharSet(pKeyContexts);
				}
				else if ((lCurrentRow == NX_PRISM_KEYBOARD_ROW_DELETE) && ((lCurrentCol == NX_PRISM_KEYBOARD_COL_DELETE_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_DELETE_1)))	// <-
				{
					if (pKeyContexts->lCurrentPtr == 0)
						return ONDK_MESSAGE_CONSUMED;;

					len = nx_keyboard_GetByteSize(pKeyContexts->strbuf, 0, (pKeyContexts->lCurrentPtr - 1));
					ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetAt(pKeyContexts->strbuf, (pKeyContexts->lCurrentPtr - 1)));
					total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pKeyContexts->strbuf));
					if (ptr_len == 2)
					{
						HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf, len);
						HLIB_STR_SetAtBuffer(pKeyContexts->strbuf, (total_len - 2), 0);
					}
					else
					{
						HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf, len);
						HLIB_STR_SetAtBuffer(pKeyContexts->strbuf, (total_len - 1), 0);
					}

					pKeyContexts->lCurrentPtr--;
				}
				else if ((lCurrentRow == NX_PRISM_KEYBOARD_ROW_DELETE_ALL) && ((lCurrentCol == NX_PRISM_KEYBOARD_COL_DELETE_ALL_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_DELETE_ALL_1)))	// X
				{
					len = nx_keyboard_GetByteSize(pKeyContexts->strbuf, 0, pKeyContexts->lCurrentPtr);
					for (i = 0; i < len; i++)
					{
						HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf, 0);
					}
					HLIB_STR_SetAtBuffer(pKeyContexts->strbuf, 0, 0);
					pKeyContexts->lCurrentPtr = 0;
				}
				else if ((lCurrentRow == NX_PRISM_KEYBOARD_ROW_LANGUAGE) && ((lCurrentCol == NX_PRISM_KEYBOARD_COL_LANGUAGE_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_LANGUAGE_1)))	// Language
				{
					nx_keyboard_ChangeLanguage(pKeyContexts);
					nx_keyboard_SettingUsingTable(pKeyContexts);
					nx_keyboard_SetCharSet(pKeyContexts);
					nx_keyboard_SetNumSet(pKeyContexts);
				}
				else if ((lCurrentRow == NX_PRISM_KEYBOARD_ROW_SPACE) && ((lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_0) || (lCurrentCol == NX_PRISM_KEYBOARD_COL_SPACE_1)))	// |___|
				{
					len = nx_keyboard_GetByteSize(pKeyContexts->strbuf, 0, pKeyContexts->lCurrentPtr);
					pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf, (HUINT8*)" ", len);
					pKeyContexts->lCurrentPtr++;

					if (pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
					{
						pKeyContexts->lPwdOnOff = 1;
						//ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
						ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW, NX_PRISM_KEYBOARD_PW_INTERVAL);
					}
				}
				else
				{
					if ((HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pKeyContexts->strbuf)) > (NX_PRISM_KEYBOARD_MAX_LINE_LENGTH - 2)) && (pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD))
					{
						ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
						return ONDK_MESSAGE_CONSUMED;
					}
#if 0
					if(HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pKeyContexts->strbuf)) > NX_PRISM_KEYBOARD_MAX_TOTAL_LENGTH-1)
					{
						ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
						return ONDK_MESSAGE_CONSUMED;
					}
#endif

					len = nx_keyboard_GetByteSize(pKeyContexts->strbuf, 0, pKeyContexts->lCurrentPtr);
					lFindArray_0 = pKeyContexts->lModeUpLow;
					lFindArray_1 = pKeyContexts->lModeToggle + (lCurrentRow - NX_PRISM_KEYBOARD_ROW_1) * NX_PRISM_KEYBOARD_COL_KEY_COUNT + lCurrentCol;

					if (lCurrentRow == NX_PRISM_KEYBOARD_ROW_NUMBER)
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf, (HUINT8*)s_aucNumSet_Using[lCurrentCol], len);
					else
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf, (HUINT8*)s_aucCharSet_Using[lFindArray_0][lFindArray_1], len);

					pKeyContexts->lCurrentPtr++;

					if (pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
					{
						pKeyContexts->lPwdOnOff = 1;
						//ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
						ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW,NX_PRISM_KEYBOARD_PW_INTERVAL);
					}
				}

				if (nx_keyboard_SetInputBoxString(pKeyContexts) == ONDK_MESSAGE_CONSUMED)
				{
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					return ONDK_MESSAGE_CONSUMED;
				}

				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			}
			return ONDK_MESSAGE_CONSUMED;

		case KEY_MUTE:
		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
			return GWM_MESSAGE_PASS;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return GWM_MESSAGE_PASS;

		case KEY_UNI_SPACE:
			len = nx_keyboard_GetByteSize(pKeyContexts->strbuf, 0, pKeyContexts->lCurrentPtr);
			pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf, (HUINT8*)" ", len);
			pKeyContexts->lCurrentPtr++;

			if (pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
			{
				pKeyContexts->lPwdOnOff = 1;
				//ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
				ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW,NX_PRISM_KEYBOARD_PW_INTERVAL);
			}

			if (nx_keyboard_SetInputBoxString(pKeyContexts) == ONDK_MESSAGE_CONSUMED)
			{
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				return ONDK_MESSAGE_CONSUMED;
			}
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			return ONDK_MESSAGE_CONSUMED;

		case KEY_UNI_BACKSPACE:
			if (pKeyContexts->lCurrentPtr == 0)
				break;

			len = nx_keyboard_GetByteSize(pKeyContexts->strbuf, 0, pKeyContexts->lCurrentPtr-1);
			ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetAt(pKeyContexts->strbuf, (pKeyContexts->lCurrentPtr - 1)));
			total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pKeyContexts->strbuf));

			if (ptr_len == 2)
			{
				HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf, len);
				HLIB_STR_SetAtBuffer(pKeyContexts->strbuf, (total_len - 2), 0);
			}
			else
			{
				HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf, len);
				HLIB_STR_SetAtBuffer(pKeyContexts->strbuf, (total_len - 1), 0);
			}

			pKeyContexts->lCurrentPtr--;
			if (nx_keyboard_SetInputBoxString(pKeyContexts) == ONDK_MESSAGE_CONSUMED)
			{
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				return ONDK_MESSAGE_CONSUMED;
			}

			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			return ONDK_MESSAGE_CONSUMED;

		default:
			if ((lKeyId > 0x0029) && (lKeyId < 0x0040) && (inputEvent->device_id == eKeyInput_RCU))
			{
				switch (lKeyId)
				{
					case KEY_0:
						if(numericKeyValue != 0)
							pKeyContexts->lSmsInput = 0;
						numericKeyValue = 0;
						break;
					case KEY_1:
						if(numericKeyValue != 1)
							pKeyContexts->lSmsInput = 0;
						numericKeyValue = 1;
						break;
					case KEY_2:
						if(numericKeyValue != 2)
							pKeyContexts->lSmsInput = 0;
						numericKeyValue = 2;
						break;
					case KEY_3:
						if(numericKeyValue != 3)
							pKeyContexts->lSmsInput = 0;
						numericKeyValue = 3;
						break;
					case KEY_4:
						if(numericKeyValue != 4)
							pKeyContexts->lSmsInput = 0;
						numericKeyValue = 4;
						break;
					case KEY_5:
						if(numericKeyValue != 5)
							pKeyContexts->lSmsInput = 0;
						numericKeyValue = 5;
						break;
					case KEY_6:
						if(numericKeyValue != 6)
							pKeyContexts->lSmsInput = 0;
						numericKeyValue = 6;
						break;
					case KEY_7:
						if(numericKeyValue != 7)
							pKeyContexts->lSmsInput = 0;
						numericKeyValue = 7;
						break;
					case KEY_8:
						if(numericKeyValue != 8)
							pKeyContexts->lSmsInput = 0;
						numericKeyValue = 8;
						break;
					case KEY_9:
						if(numericKeyValue != 9)
							pKeyContexts->lSmsInput = 0;
						numericKeyValue = 9;
						break;
				}

				if ((s_numericInterval + NX_PRISM_KEYBOARD_NUMERIC_INTERVAL) >  VK_TIMER_GetSystemTick())
				{
					//change string
					//delete
					if (pKeyContexts->lCurrentPtr != 0)
					{
						len = nx_keyboard_GetByteSize(pKeyContexts->strbuf, 0, (pKeyContexts->lCurrentPtr - 1));
						ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetAt(pKeyContexts->strbuf, (pKeyContexts->lCurrentPtr - 1)));
						total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pKeyContexts->strbuf));

						if (ptr_len == 2)
						{
							HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf, len);
							HLIB_STR_SetAtBuffer(pKeyContexts->strbuf, (total_len - 2), 0);
						}
						else
						{
							HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf,len);
							HLIB_STR_SetAtBuffer(pKeyContexts->strbuf, (total_len - 1), 0);
						}

						pKeyContexts->lCurrentPtr--;
					}
					//write
					len = nx_keyboard_GetByteSize(pKeyContexts->strbuf, 0, pKeyContexts->lCurrentPtr);
					if ((numericKeyValue == 1) && (pKeyContexts->lSmsInput == 0))
					{
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf, (HUINT8*)" ", len);
						pKeyContexts->lSmsInput++;
					}
					else
					{
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf, (HUINT8*)s_aucNumericSet_Using[pKeyContexts->lModeUpLow][numericKeyValue][pKeyContexts->lSmsInput++], len);
					}

					pKeyContexts->lCurrentPtr++;

					if ((numericKeyValue == 7) || (numericKeyValue == 9)) // 5 char key
					{
						if (pKeyContexts->lSmsInput > NX_PRISM_KEYBOARD_NUMERIC_COUNT-1)
							pKeyContexts->lSmsInput = 0;
					}
					else // 4 char key
					{
						if (pKeyContexts->lSmsInput > NX_PRISM_KEYBOARD_NUMERIC_COUNT-2)
							pKeyContexts->lSmsInput = 0;
					}

				}
				else
				{
					//write
					len = nx_keyboard_GetByteSize(pKeyContexts->strbuf, 0, pKeyContexts->lCurrentPtr);
					if ((numericKeyValue == 1) && (pKeyContexts->lSmsInput == 0))
					{
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf, (HUINT8*)" ", len);
						pKeyContexts->lSmsInput++;
					}
					else
					{
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf, (HUINT8*)s_aucNumericSet_Using[pKeyContexts->lModeUpLow][numericKeyValue][0], len);
					}
					pKeyContexts->lCurrentPtr++;
					pKeyContexts->lSmsInput = 0;
				}

				if (pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
				{
					pKeyContexts->lPwdOnOff = 1;
					ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW, NX_PRISM_KEYBOARD_PW_INTERVAL);
				}

				s_numericInterval = VK_TIMER_GetSystemTick();
				nx_keyboard_SetInputBoxString(pKeyContexts);
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

				return ONDK_MESSAGE_CONSUMED;
			}

			if ((lKeyId > 0x0020) && (lKeyId < 0x007F))//unicode 값 직접 사용
			{
				inputStr[0] = (HUINT8)lKeyId;
				inputStr[1] = '\0';

				len = nx_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->lCurrentPtr);
				pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf, (const HUINT8*)inputStr,len);

				pKeyContexts->lCurrentPtr++;

				if(HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pKeyContexts->strbuf)) > NX_PRISM_KEYBOARD_MAX_LINE_LENGTH-2 &&	pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
				{
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					return ONDK_MESSAGE_CONSUMED;
				}

				if(pKeyContexts->lInputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
				{
					pKeyContexts->lPwdOnOff = 1;
					//ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
					ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW,NX_PRISM_KEYBOARD_PW_INTERVAL);
				}

				if (nx_keyboard_SetInputBoxString(pKeyContexts) == ONDK_MESSAGE_CONSUMED)
				{
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					return ONDK_MESSAGE_CONSUMED;
				}
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			}

			return ONDK_MESSAGE_CONSUMED;

	}

	s_limitedInterval = VK_TIMER_GetSystemTick();

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return GWM_RET_OK;
}

//MSG_GWM_TIMER
static ONDK_Result_t	nx_keyboard_MsgGwmTimer(NxKeyboard_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			ulTimerID = p1;
	ONDK_GWM_GuiObject_t	obj;

	if (ulTimerID == NX_PRISM_KEYBOARD_TIMER_ID)
	{
		obj = ONDK_GWM_Obj_GetObjectByID(NX_PRISM_KEYBOARD_IMG_INPUTBOX_CURSOR_ID);

		if(ONDK_GWM_Obj_GetVisibleByObj(obj) == TRUE)
		{
			ONDK_GWM_Obj_SetVisibleByObj(obj,FALSE);
			ONDK_GWM_APP_InvalidateObject(NX_PRISM_KEYBOARD_IMG_INPUTBOX_CURSOR_ID);
		}
		else
		{
			ONDK_GWM_Obj_SetVisibleByObj(obj,TRUE);
			ONDK_GWM_APP_InvalidateObject(NX_PRISM_KEYBOARD_IMG_INPUTBOX_CURSOR_ID);
		}

		s_ulCursorInterval = VK_TIMER_GetSystemTick();
	}
	else if (ulTimerID == GWMPOPUP_TIMER_KEYBOARD_PW)
	{
		pKeyContexts->lPwdOnOff = 0;
		nx_keyboard_SetInputBoxString(pKeyContexts);
		ONDK_GWM_KillTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
	}

	/*
	else if(ulTimerID == GWMPOPUP_TIMER_KEYBOARD_MOVE)
	{
		static int arr[20] = {10, 20, 0, 30, 0, 40,0, 40, 0, 40, 0, 40, 10, 50, 10, 50, 10, 60, 10};
		ONDK_GWM_APP_GetAppArea(ONDK_GWM_APP_GetThis(),&area);
		ONDK_GWM_APP_MoveAppArea(0,area.y-arr[pKeyContexts->speed],area.w,area.h);
		ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());


//		NX_APP_Debug("@@@@ %d (%d)\r\n",pKeyContexts->speed, VK_TIMER_GetSystemTick());
		if(pKeyContexts->speed >= 20)
		{
			ONDK_GWM_KillTimer(GWMPOPUP_TIMER_KEYBOARD_MOVE);
		}
		else
		{
			pKeyContexts->speed++;
		}
	}
	*/

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return GWM_RET_OK;
}


static ONDK_Result_t	nx_keyboard_MsgRemoteApp(NxKeyboard_Contents_t *pKeyContexts, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT8				*str_val = (HINT8 *)p1;
	ONDK_Result_t		ret = ONDK_RET_OK;

	if (pKeyContexts->strbuf != NULL)
	{
		HLIB_STR_Delete(pKeyContexts->strbuf);
	}

	pKeyContexts->strbuf = HLIB_STR_New((HUINT8*)str_val);
	pKeyContexts->lCurrentPtr = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pKeyContexts->strbuf));

	ret = nx_keyboard_SetInputBoxString(pKeyContexts);

	NX_COMMON_NOT_USED_ARGUMENT(handle, p1, p2, p3);

	return GWM_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_KeyBoard_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = GWM_RET_OK;
	static ONDK_Result_t	s_upkey = ONDK_RET_OK;
	NxKeyboard_Contents_t			*pKeyContexts = nx_keyboard_KeyboardGetContents();

	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(0x%x) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			lRet = nx_keyboard_MsgGwmCreate(pKeyContexts, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			lRet = nx_keyboard_MsgGwmTimer(pKeyContexts, p1, p2, p3);
			break;

		case MSG_GWM_KEYDOWN:
			lRet = nx_keyboard_MsgGwmKeyDown(pKeyContexts, hHandle, p1, p2, p3);
			s_upkey = lRet;
			break;

		case MSG_GWM_KEYUP:
			lRet = s_upkey;
			break;

		case MSG_SIGNAL_REMOTESTING:
			lRet = nx_keyboard_MsgRemoteApp(pKeyContexts, hHandle, p1, p2, p3);
			break;

		case MSG_APP_KEYBOARD_DESTROY_KEYBOARD:
			ONDK_GWM_APP_Destroy(0);
			break;

		case MSG_GWM_DESTROY:
			lRet = nx_keyboard_MsgGwmDestroy(pKeyContexts, p1, p2, p3);
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


ONDK_Result_t	NX_PRISM_KeyBoard_Destroy(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_KeyBoard_Proc);
	if (pGwmApp != NULL)
	{
		return ONDK_GWM_APP_Destroy(NX_PRISM_KeyBoard_Proc);
	}
	else
	{
		return	ONDK_RET_OK;
	}
}


HCHAR *NX_PRISM_KeyBoard_GetResultString(void)
{
	NxKeyboard_Contents_t	*pKeyContexts = nx_keyboard_KeyboardGetContents();

	if((pKeyContexts == NULL) || (pKeyContexts->strbuf == NULL))
	{
		HxLOG_Debug("KeyBoard String is NULL!!\n");
		return NULL;
	}

	return HLIB_STD_StrDup((HCHAR*)HLIB_STR_GetBuffer(pKeyContexts->strbuf));
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

