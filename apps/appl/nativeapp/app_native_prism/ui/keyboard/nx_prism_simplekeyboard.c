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
 * @file	  		nx_prism_simplekeyboard.c
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
#include <nx_prism_simplekeyboard.h>
#include "nx_prism_arabic_support.h"


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


typedef enum
{
	eSIMPLE_CHARSET_English,
	eSIMPLE_CHARSET_German,
	eSIMPLE_CHARSET_Turkish,
	eSIMPLE_CHARSET_Russian,
	eSIMPLE_CHARSET_Arabic,
	eSIMPLE_CHARSET_French,
	eSIMPLE_CHARSET_Italian,
	eSIMPLE_CHARSET_Spanish,
	eSIMPLE_CHARSET_Greek,
	eSIMPLE_CHARSET_Persian,
	eSIMPLE_CHARSET_Swedish,
	eSIMPLE_CHARSET_Max
} SIMPLE_Key_Charset_e;

typedef struct _Simple_Key_Contents
{
	HUINT32			currentFocus;
	HUINT32			selectedCharSet;
	HUINT32			firstVisibleOffset;
	HUINT32			currentPtr;
	SIMPLE_Key_Charset_e	language;

	HINT32			smsInput;

#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	HUINT8			szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH];
#endif

	HxSTR_t			*strbuf;
} Simple_Key_Contents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static HCHAR	*s_SimpleCharSet_Using[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT];
static HCHAR	*s_SimpleNumericSet_Using[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_NUM_BTN_COUNT][NX_PRISM_SIMPLE_KEYBOARD_NUMERIC_COUNT];
static const HUINT16	s_SimpleNumericSet_Eng[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_NUM_BTN_COUNT][NX_PRISM_SIMPLE_KEYBOARD_NUMERIC_COUNT] =
{
	//capital
	{
		{0x0026, 0x0040,0x005F,0x0030,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 0
		{0x0020, 0x002E,0x002C,0x002D,0x003F,0x0021,0x0027,0x003A,0x003B,0x002F,0x0031,0x0000}, // 1
		{0x0041, 0x0042,0x0043,0x0032,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 2
		{0x0044, 0x0045,0x0046,0x0033,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 3
		{0x0047, 0x0048,0x0049,0x0034,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 4
		{0x004A, 0x004B,0x004C,0x0035,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 5
		{0x004D, 0x004E,0x004F,0x0036,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 6
		{0x0050, 0x0051,0x0052,0x0053,0x0037,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 7
		{0x0054, 0x0055,0x0056,0x0038,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 8
		{0x0057, 0x0058,0x0059,0x005A,0x0039,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}  // 9
	},
	//small
	{
		{0x0026, 0x0040,0x005F,0x0030,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 0
		{0x0020, 0x002E,0x002C,0x002D,0x003F,0x0021,0x0027,0x003A,0x003B,0x002F,0x0031,0x0000}, // 1
		{0x0061, 0x0062,0x0063,0x0032,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 2
		{0x0064, 0x0065,0x0066,0x0033,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 3
		{0x0067, 0x0068,0x0069,0x0034,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 4
		{0x006A, 0x006B,0x006C,0x0035,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 5
		{0x006D, 0x006E,0x006F,0x0036,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 6
		{0x0070, 0x0071,0x0072,0x0073,0x0037,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 7
		{0x0074, 0x0075,0x0076,0x0038,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 8
		{0x0077, 0x0078,0x0079,0x007A,0x0039,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}  // 9
	}
};

static const HUINT16 s_SimpleNumericSet_Ger[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_NUM_BTN_COUNT][NX_PRISM_SIMPLE_KEYBOARD_NUMERIC_COUNT] =
{
	//capital
	{
		{0x0026, 0x0040,0x005F,0x0030,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 0
		{0x0020, 0x002E,0x002C,0x002D,0x003F,0x0021,0x0027,0x003A,0x003B,0x002F,0x0031,0x0000}, // 1
		{0x0041, 0x0042,0x0043,0x0032,0x00C4,0x00C0,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 2
		{0x0044, 0x0045,0x0046,0x0033,0x00C9,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 3
		{0x0047, 0x0048,0x0049,0x0034,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 4
		{0x004A, 0x004B,0x004C,0x0035,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 5
		{0x004D, 0x004E,0x004F,0x0036,0x00D6,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 6
		{0x0050, 0x0051,0x0052,0x0053,0x0037,0x00DF,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 7
		{0x0054, 0x0055,0x0056,0x0038,0x00DC,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 8
		{0x0057, 0x0058,0x0059,0x005A,0x0039,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}  // 9
	},
	//small
	{
		{0x0026, 0x0040,0x005F,0x0030,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 0
		{0x0020, 0x002E,0x002C,0x002D,0x003F,0x0021,0x0027,0x003A,0x003B,0x002F,0x0031,0x0000}, // 1
		{0x0061, 0x0062,0x0063,0x0032,0x00E4,0x00E0,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 2
		{0x0064, 0x0065,0x0066,0x0033,0x00E9,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 3
		{0x0067, 0x0068,0x0069,0x0034,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 4
		{0x006A, 0x006B,0x006C,0x0035,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 5
		{0x006D, 0x006E,0x006F,0x0036,0x00F6,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 6
		{0x0070, 0x0071,0x0072,0x0073,0x0037,0x00DF,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 7
		{0x0074, 0x0075,0x0076,0x0038,0x00FC,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}, // 8
		{0x0077, 0x0078,0x0079,0x007A,0x0039,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}  // 9
	}
};

static const HUINT16 s_SimpleCharSet_Eng[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
	{
		//Small 26
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020
	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16 s_SimpleCharSet_Ger[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
	{
		//Small 26
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A,
		//Extra 6
		0x00E4, 0x00E0, 0x00E9, 0x00F6, 0x00F9, 0x00FC,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020


	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16 s_SimpleCharSet_Tur[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
	{
		//Small 26
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A,
		//Extra 8
		0x00E2, 0x00E7, 0x011F, 0x0131, 0x00F6, 0x015F, 0x00FB, 0x00FC,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16 s_SimpleCharSet_Rus[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
 	{
	  	//Small 28
		0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0451, 0x0436, 0x0437, 0x0438,
		0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F, 0x0440, 0x0441, 0x0442,
		0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A,
		//Small Ext 18
		0x044B, 0x044C, 0x044D, 0x044E, 0x044F, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456,
		0x0457, 0x0458, 0x0459, 0x045A, 0x045B,	0x045C, 0x045E, 0x045F,

		0x0020, 0x0020, 0x0020, 0x0020
	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16 s_SimpleCharSet_Fre[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
	{
		//Small 26
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A,
		//Extra 16
		0x00E0, 0x00E2, 0x00E6, 0x00E7, 0x00E9, 0x00E8, 0x00EA, 0x00EB, 0x00EE, 0x00EF,
		0x00F1, 0x00F4, 0x0153, 0x00F9, 0x00FB, 0x00FF,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16 s_SimpleCharSet_Ara[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
 	{
		0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627, 0x0628, 0x0629, 0x062A,
		0x062B, 0x062C, 0x062D, 0x062E, 0x062F, 0x0630, 0x0631, 0x0632, 0x0633, 0x0634,
		0x0635, 0x0636, 0x0637, 0x0638, 0x0639, 0x063A,
		//(Based on ISO 8859-6, 0x0640 ~ 0x064A)
		0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647, 0x0648, 0x0649,
		0x064A,
		//(Points from ISO 8859-6, 0x064B ~ 0x0652)
		0x064B, 0x064C, 0x064E, 0x064F, 0x0650, 0x0651, 0x0652,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16 s_SimpleCharSet_Per[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
 	{
		//(Based on ISO 8859-6, 0x0621 ~ 0x063A)
		0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627, 0x0628, 0x0629, 0x062A,
		0x062B, 0x062C, 0x062D, 0x062E, 0x062F, 0x0630, 0x0631, 0x0632, 0x0633, 0x0634,
		0x0635, 0x0636, 0x0637, 0x0638, 0x0639, 0x063A,
		//(Based on ISO 8859-6, 0x0640 ~ 0x064A)
		0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647, 0x0648, 0x0649,
		0x064A,
		//(Points from ISO 8859-6, 0x064B ~ 0x0652)
		0x064B, 0x064C, 0x064E, 0x064F, 0x0650, 0x0651, 0x0652,
		//(ISO 8859-6에 존재하지 않는 Persian Characters)
		0x067E, 0x0686, 0x0698, 0x06AF,
		0x0020, 0x0020
	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}

};

static const HUINT16 s_SimpleCharSet_Ita[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
	{
		//Small 26
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A,
		//Extra 6
		0x00E0, 0x00E9, 0x00E8, 0x00EC, 0x00F2, 0x00F9,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16 s_SimpleCharSet_Spa[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
	{
		//Small 26
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A,
		//Extra 12
		0x00E1, 0x00E0, 0x00E7, 0x00E9, 0x00E8, 0x00ED, 0x00EF, 0x00F1, 0x00F3, 0x00F2,
		0x00FA, 0x00FC,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020
	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16 s_SimpleCharSet_Gre[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
	{
		//Small 24
		0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7, 0x03B8, 0x03B9, 0x03BA,
		0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF, 0x03C0, 0x03C1, 0x03C3, 0x03C4, 0x03C5,
		0x03C6, 0x03C7, 0x03C8, 0x03C9,
		//Extra 12
		0x03AC, 0x03AD, 0x03AE, 0x03AF, 0x03CA, 0x0390, 0x03CC, 0x03C2, 0x03CD, 0x03CB,
		0x03B0, 0x03CE,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020
	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static const HUINT16 s_SimpleCharSet_Swe[NX_PRISM_SIMPLE_KEYBOARD_CHARSET_COUNT][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT] =
{
	{
		//Small 26
		0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
		0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
		0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A,
		//Extra 16
		0x00E1, 0x00E0, 0x00E6, 0x010D, 0x0111, 0x00E9, 0x00EF, 0x014B, 0x0161, 0x0167,
		0x00FC, 0x017E, 0x00E5, 0x00E4, 0x00F6, 0x00F8,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	},
	{
		//Num 10
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,

		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
		0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
	}
};

static Simple_Key_Contents_t		s_Simple_Keyboard_Contents;
static HUINT32 						s_Simple_CursorInterval = 0;
static HUINT32 						s_Simple_StartInterval = 0;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static Simple_Key_Contents_t*	 nx_simplekeyboard_KeyboardGetContents(void);

static ONDK_Result_t	nx_simplekeyboard_MsgGwmCreate(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_simplekeyboard_MsgGwmDestroy(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	nx_simplekeyboard_InitData(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_simplekeyboard_FreeData(void);
static ONDK_Result_t 	nx_simplekeyboard_RemoteAppMsg(Simple_Key_Contents_t *pSimpleKeyContexts, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	nx_simplekeyboard_SettingUsingTable(Simple_Key_Contents_t *pSimpleKeyContexts);

static ONDK_Result_t 	nx_simplekeyboard_SetLanguage(Simple_Key_Contents_t *pSimpleKeyContexts);

static ONDK_Result_t 	nx_simplekeyboard_SetInputObject(Simple_Key_Contents_t *pSimpleKeyContexts);

static ONDK_Result_t 	nx_simplekeyboard_MsgGwmKeyDown(Simple_Key_Contents_t *pSimpleKeyContexts, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	nx_simplekeyboard_MsgGwmTimer(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_simplekeyboard_SetInputBoxString(Simple_Key_Contents_t *pSimpleKeyContexts);

static HINT32 			nx_simplekeyboard_GetByteSize(HxSTR_t *str, HUINT32 start, HUINT32 cnt);
static HINT32 			nx_simplekeyboard_GetStringWidth(Simple_Key_Contents_t *pSimpleKeyContexts, HxSTR_t *str);
static void 			nx_simplekeyboard_SetCharSet(Simple_Key_Contents_t *pSimpleKeyContexts);
static ONDK_Result_t 	nx_simplekeyboard_KeyArrowDown(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId);
static ONDK_Result_t	nx_simplekeyboard_KeyArrowUp(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId);
#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
static ONDK_Result_t	nx_simplekeyboard_KeyArrowDownWithLanguageBtn(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId);
static ONDK_Result_t	nx_simplekeyboard_KeyArrowUpWithLanguageBtn(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId);
static void 			nx_simplekeyboard_SetCurLanguage(Simple_Key_Contents_t *pSimpleKeyContexts);
#endif

static void				nx_simplekeyboard_ChangeLanguage(Simple_Key_Contents_t *pSimpleKeyContexts);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static Simple_Key_Contents_t*	 nx_simplekeyboard_KeyboardGetContents(void)
{
	return &s_Simple_Keyboard_Contents;
}


static ONDK_Result_t nx_simplekeyboard_MsgGwmCreate(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		hRet = ONDK_RET_OK;

	s_Simple_StartInterval = VK_TIMER_GetSystemTick();

	hRet = nx_simplekeyboard_SetLanguage(pSimpleKeyContexts);
	hRet = nx_simplekeyboard_InitData(pSimpleKeyContexts, p1,p2,p3);

	hRet = nx_simplekeyboard_SettingUsingTable(pSimpleKeyContexts);

	hRet = nx_simplekeyboard_SetInputObject(pSimpleKeyContexts);
	hRet = nx_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_SIMPLEKEYBOARD, NX_PRISM_SIMPLE_KEYBOARD_CURSOR_INTERVAL);

	return hRet;
}

static ONDK_Result_t nx_simplekeyboard_MsgGwmDestroy(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pSimpleKeyContexts->strbuf)
	{
		HLIB_STR_Delete(pSimpleKeyContexts->strbuf);
		pSimpleKeyContexts->strbuf = NULL;
	}

	nx_simplekeyboard_FreeData();

	return GWM_RET_OK;
}

static ONDK_Result_t nx_simplekeyboard_InitData(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			i,j,k;
	ONDK_Result_t	lRet = ONDK_RET_OK;

	pSimpleKeyContexts->selectedCharSet = NX_PRISM_SIMPLE_KEYBOARD_CHARSET_CHAR;
	pSimpleKeyContexts->firstVisibleOffset = 0;
	pSimpleKeyContexts->currentPtr = 0;
	pSimpleKeyContexts->currentFocus = 0;

	for(i=0;i<2;i++)
	{
		for(j=0;j<NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT;j++)
		{
			s_SimpleCharSet_Using[i][j] = NX_APP_Malloc(sizeof(HUINT8)*8);
			HxSTD_MemSet(s_SimpleCharSet_Using[i][j], 0, sizeof(HUINT8)*8);		//	Memory치는 경우 존재, 넉넉히 받자.
		}
	}

	//numeric array setting
	for(k=0;k<2;k++)
	{
		for(i=0;i<NX_PRISM_SIMPLE_KEYBOARD_NUM_BTN_COUNT;i++)
		{
			for(j=0;j<NX_PRISM_SIMPLE_KEYBOARD_NUMERIC_COUNT;j++)
			{
				s_SimpleNumericSet_Using[k][i][j] = NX_APP_Malloc(sizeof(HUINT8)*8);
				HxSTD_MemSet(s_SimpleNumericSet_Using[k][i][j], 0, sizeof(HUINT8)*8);
			}
		}
	}

	if((void *)p3 == NULL)
	{
		pSimpleKeyContexts->strbuf = HLIB_STR_New("");
	}
	else
	{
		pSimpleKeyContexts->strbuf = HLIB_STR_New((HINT8 *)p3);
		pSimpleKeyContexts->currentPtr = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
	}

	return lRet;

}

static ONDK_Result_t nx_simplekeyboard_FreeData(void)
{
	HINT32			i,j;
	ONDK_Result_t	lRet = ONDK_RET_OK;

	for(i=0;i<2;i++)
	{
		for(j=0;j<NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT;j++)
		{
			NX_APP_Free(s_SimpleCharSet_Using[i][j]);
		}
	}

	return lRet;
}
static ONDK_Result_t nx_simplekeyboard_RemoteAppMsg(Simple_Key_Contents_t *pSimpleKeyContexts, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32					nNumChar = 0;
	ONDK_Result_t			ret = ONDK_RET_OK;
	HINT8					*str_val = (HINT8 *)p1;

	if(HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,str_val) > NX_PRISM_SIMPLE_KEYBOARD_MAX_CHAR_LENGTH-1)
	{
		return GWM_MESSAGE_BREAK;
	}
	if(pSimpleKeyContexts->strbuf != NULL)
		HLIB_STR_Delete(pSimpleKeyContexts->strbuf);
	pSimpleKeyContexts->strbuf = HLIB_STR_New(str_val);
	nNumChar = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
	pSimpleKeyContexts->currentPtr = (nNumChar > 0) ? (HUINT32)nNumChar : 0;
	ret = nx_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);

	return GWM_MESSAGE_BREAK;
}

static ONDK_Result_t	nx_simplekeyboard_KeyArrowDown(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;

	switch(objId)
	{
		case NX_PRISM_SIMPLE_KEY_INPUTBOX_ID:
			if(pSimpleKeyContexts->currentFocus > NX_PRISM_SIMPLE_KEY_CHAR_ID+9 )
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CHAR_ID+5);
			}
			else ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			break;

		case NX_PRISM_SIMPLE_KEY_BACK_ID:
			if(pSimpleKeyContexts->selectedCharSet == NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if(pSimpleKeyContexts->currentFocus > NX_PRISM_SIMPLE_KEY_CHAR_ID+12 || pSimpleKeyContexts->currentFocus < NX_PRISM_SIMPLE_KEY_CHAR_ID+10)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CHAR_ID+11);
			}
			else ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			break;

		case NX_PRISM_SIMPLE_KEY_CLEAR_ID:
			if(pSimpleKeyContexts->selectedCharSet == NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if(pSimpleKeyContexts->currentFocus > NX_PRISM_SIMPLE_KEY_CHAR_ID+15 || pSimpleKeyContexts->currentFocus < NX_PRISM_SIMPLE_KEY_CHAR_ID+13 )
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CHAR_ID+14);
			}
			else ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			break;

		case NX_PRISM_SIMPLE_KEY_SPACE_ID:
			if(pSimpleKeyContexts->selectedCharSet == NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if(pSimpleKeyContexts->currentFocus > NX_PRISM_SIMPLE_KEY_CHAR_ID+18 || pSimpleKeyContexts->currentFocus < NX_PRISM_SIMPLE_KEY_CHAR_ID+16 )
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CHAR_ID+17);
			}
			else ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			break;

		case NX_PRISM_SIMPLE_KEY_TOGGLE_ID:
			if(pSimpleKeyContexts->selectedCharSet == NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if(pSimpleKeyContexts->currentFocus < NX_PRISM_SIMPLE_KEY_CHAR_ID+19)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CHAR_ID+20);
			}
			else ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			break;

	}

	return lRet;
}

static ONDK_Result_t nx_simplekeyboard_KeyArrowUp(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;

	if(objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+10 && objId > NX_PRISM_SIMPLE_KEY_CHAR_ID-1 )
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_INPUTBOX_ID);
	}
	else if(objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+13 && objId > NX_PRISM_SIMPLE_KEY_CHAR_ID+9)
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_BACK_ID);
	}
	else if(objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+16 && objId > NX_PRISM_SIMPLE_KEY_CHAR_ID+12)
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CLEAR_ID);
	}
	else if(objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+20 && objId > NX_PRISM_SIMPLE_KEY_CHAR_ID+15)
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_SPACE_ID);
	}
	else if(objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+22 && objId > NX_PRISM_SIMPLE_KEY_CHAR_ID+19)
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_TOGGLE_ID);
	}

	return lRet;
}


#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
static ONDK_Result_t nx_simplekeyboard_KeyArrowDownWithLanguageBtn(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;

	switch(objId)
	{
		case NX_PRISM_SIMPLE_KEY_INPUTBOX_ID:
			if(pSimpleKeyContexts->currentFocus > NX_PRISM_SIMPLE_KEY_CHAR_ID+9 )
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+5);
			}
			else
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case NX_PRISM_SIMPLE_KEY_BACK_ID:
			if(pSimpleKeyContexts->selectedCharSet == NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > NX_PRISM_SIMPLE_KEY_CHAR_ID+13) || (pSimpleKeyContexts->currentFocus < NX_PRISM_SIMPLE_KEY_CHAR_ID+10))
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+10);
			}
			else
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case NX_PRISM_SIMPLE_KEY_CLEAR_ID:
			if(pSimpleKeyContexts->selectedCharSet == NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > NX_PRISM_SIMPLE_KEY_CHAR_ID+15) || (pSimpleKeyContexts->currentFocus < NX_PRISM_SIMPLE_KEY_CHAR_ID+13))
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+13);
			}
			else
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case NX_PRISM_SIMPLE_KEY_SPACE_ID:
			if(pSimpleKeyContexts->selectedCharSet == NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > NX_PRISM_SIMPLE_KEY_CHAR_ID+18) || (pSimpleKeyContexts->currentFocus < NX_PRISM_SIMPLE_KEY_CHAR_ID+15))
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+15);
			}
			else
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case NX_PRISM_SIMPLE_KEY_TOGGLE_ID:
			if(pSimpleKeyContexts->selectedCharSet == NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > NX_PRISM_SIMPLE_KEY_CHAR_ID+20) || (pSimpleKeyContexts->currentFocus < NX_PRISM_SIMPLE_KEY_CHAR_ID+18))
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+18);
			}
			else
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case NX_PRISM_SIMPLE_KEY_LANGUAGE_ID:
			if(pSimpleKeyContexts->selectedCharSet == NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+9);
				break;
			}

			if((pSimpleKeyContexts->currentFocus > NX_PRISM_SIMPLE_KEY_CHAR_ID+22) || (pSimpleKeyContexts->currentFocus < NX_PRISM_SIMPLE_KEY_CHAR_ID+20))
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CHAR_ID+20);
			}
			else
			{
				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;
	}

	return lRet;
}

static ONDK_Result_t nx_simplekeyboard_KeyArrowUpWithLanguageBtn(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;

	if((objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+10) && (objId > NX_PRISM_SIMPLE_KEY_CHAR_ID-1))
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_INPUTBOX_ID);
	}
	else if((objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+13) && (objId > NX_PRISM_SIMPLE_KEY_CHAR_ID+9))
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_BACK_ID);
	}
	else if((objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+15) && (objId > NX_PRISM_SIMPLE_KEY_CHAR_ID+12))
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_CLEAR_ID);
	}
	else if((objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+18) && (objId > NX_PRISM_SIMPLE_KEY_CHAR_ID+14))
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_SPACE_ID);
	}
	else if((objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+20) && (objId > NX_PRISM_SIMPLE_KEY_CHAR_ID+17))
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_TOGGLE_ID);
	}
	else if((objId <= NX_PRISM_SIMPLE_KEY_CHAR_ID+22) && (objId > NX_PRISM_SIMPLE_KEY_CHAR_ID+19))
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ, NX_PRISM_SIMPLE_KEY_LANGUAGE_ID);
	}

	return lRet;
}
#endif


static void		nx_simplekeyboard_ChangeLanguage(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	HBOOL		bFoundNextCharSet = FALSE;
	SIMPLE_Key_Charset_e	eNextLanguage = pSimpleKeyContexts->language + 1;

	while (pSimpleKeyContexts->language != eNextLanguage)
	{
		if(eNextLanguage == eSIMPLE_CHARSET_Max)	eNextLanguage = eSIMPLE_CHARSET_English;

		switch (eNextLanguage)
		{
			case eSIMPLE_CHARSET_English:
#if defined(CONFIG_PROD_LANG_ENGLISH)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "English", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_German:
#if defined(CONFIG_PROD_LANG_GERMAN)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "German", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Turkish:
#if defined(CONFIG_PROD_LANG_TURKISH)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Turkish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Russian:
#if defined(CONFIG_PROD_LANG_RUSSIAN)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Russian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Arabic:
#if defined(CONFIG_PROD_LANG_ARABIC)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Arabic", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_French:
#if defined(CONFIG_PROD_LANG_FRENCH)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "French", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Italian:
#if defined(CONFIG_PROD_LANG_ITALIAN)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Italian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Spanish:
#if defined(CONFIG_PROD_LANG_SPANISH)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Spanish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Greek:
#if defined(CONFIG_PROD_LANG_GREEK)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Greek", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Persian:
#if defined(CONFIG_PROD_LANG_PERSIAN)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Persian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Swedish:
#if defined(CONFIG_PROD_LANG_SWEDISH)
			#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Swedish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
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

	pSimpleKeyContexts->language = eNextLanguage;
}


static ONDK_Result_t nx_simplekeyboard_SettingUsingTable(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	HUINT8			utfItem[8] = {0, };
	HUINT16 		ucsitem[2] = {0, };
	HINT32			len = 0;
	HINT32			i,j,k;

	HINT16 			tmp_Charset[2][NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT];
	HINT16			tmp_Numericset[2][NX_PRISM_SIMPLE_KEYBOARD_NUM_BTN_COUNT][NX_PRISM_SIMPLE_KEYBOARD_NUMERIC_COUNT];

	ONDK_Result_t	lRet = ONDK_RET_OK;

	for(i=0;i<2;i++)
	{
		 for(j=0;j<NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT;j++)
		 {
#if defined(CONFIG_PROD_LANG_ENGLISH)
			if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_English)
			{
				tmp_Charset[i][j] = s_SimpleCharSet_Eng[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_GERMAN)
			else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_German)
			{
				tmp_Charset[i][j] = s_SimpleCharSet_Ger[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_TURKISH)
			else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Turkish)
			{
				tmp_Charset[i][j] = s_SimpleCharSet_Tur[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_RUSSIAN)
			else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Russian)
			{
				tmp_Charset[i][j] = s_SimpleCharSet_Rus[i][j];
		 }
#endif
#if defined(CONFIG_PROD_LANG_ARABIC)
			else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Arabic)
			{
			    tmp_Charset[i][j] = s_SimpleCharSet_Ara[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_PERSIAN)
			else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Persian)
			{
			    tmp_Charset[i][j] = s_SimpleCharSet_Per[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_FRENCH)
			else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_French)
			{
			    tmp_Charset[i][j] = s_SimpleCharSet_Fre[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_ITALIAN)
			 else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Italian)
			 {
				 tmp_Charset[i][j] = s_SimpleCharSet_Ita[i][j];
			 }
#endif
#if defined(CONFIG_PROD_LANG_SPANISH)
			 else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Spanish)
			 {
				 tmp_Charset[i][j] = s_SimpleCharSet_Spa[i][j];
			 }
#endif
#if defined(CONFIG_PROD_LANG_GREEK)
			 else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Greek)
			 {
				 tmp_Charset[i][j] = s_SimpleCharSet_Gre[i][j];
			 }
#endif
#if defined(CONFIG_PROD_LANG_SWEDISH)
			  else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Swedish)
			  {
				  tmp_Charset[i][j] = s_SimpleCharSet_Swe[i][j];
			  }
#endif


		 }
	}

	for(i=0;i<2;i++)
	{
		for(j=0;j<NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT;j++)
		{
			if(tmp_Charset[i][j] == 0x0020)
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
				//NX_APP_Debug("[%s:%d] len(%d) utfItem(, 0x%02d, 0x%02d, 0x%02d, 0x%02d)\r\n", __FUNCTION__, __LINE__, len, utfItem[0], utfItem[1], utfItem[2], utfItem[3]);
			}

			HxSTD_StrNCpy(s_SimpleCharSet_Using[i][j], utfItem,HxSTD_StrLen(utfItem));
			s_SimpleCharSet_Using[i][j][HxSTD_StrLen(utfItem)] = '\0';

		}
	}


	for(k=0;k<2;k++)
	{
		for(i=0;i<NX_PRISM_SIMPLE_KEYBOARD_NUM_BTN_COUNT;i++)
		{
			for(j=0;j<NX_PRISM_SIMPLE_KEYBOARD_NUMERIC_COUNT;j++)
			{
#if defined(CONFIG_PROD_LANG_ENGLISH)
				if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_English)
				{
					tmp_Numericset[k][i][j] = s_SimpleNumericSet_Eng[k][i][j];
				}
#endif
#if defined(CONFIG_PROD_LANG_GERMAN)
				else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_German)
				{
					tmp_Numericset[k][i][j] = s_SimpleNumericSet_Ger[k][i][j];
				}
#endif
#if defined(CONFIG_PROD_LANG_TURKISH)
				else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Turkish)
				{
					tmp_Numericset[k][i][j] = s_SimpleNumericSet_Eng[k][i][j];
				}
#endif
#if defined(CONFIG_PROD_LANG_RUSSIAN)
				else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Russian)
				{
					tmp_Numericset[k][i][j] = s_SimpleNumericSet_Eng[k][i][j];
			 	}
#endif
#if defined(CONFIG_PROD_LANG_ARABIC)
				else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Arabic)
				{
					tmp_Numericset[k][i][j] = s_SimpleNumericSet_Eng[k][i][j];
				}
#endif
#if defined(CONFIG_PROD_LANG_PERSIAN)
				else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Persian)
				{
					tmp_Numericset[k][i][j] = s_SimpleNumericSet_Eng[k][i][j];
				}
#endif
#if defined(CONFIG_PROD_LANG_FRENCH)
				else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_French)
				{
					tmp_Numericset[k][i][j] = s_SimpleNumericSet_Eng[k][i][j];
				}
#endif
#if defined(CONFIG_PROD_LANG_ITALIAN)
				 else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Italian)
				 {
					 tmp_Numericset[k][i][j] = s_SimpleNumericSet_Eng[k][i][j];
				 }
#endif
#if defined(CONFIG_PROD_LANG_SPANISH)
				 else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Spanish)
				 {
					 tmp_Numericset[k][i][j] = s_SimpleNumericSet_Eng[k][i][j];
				 }
#endif
#if defined(CONFIG_PROD_LANG_GREEK)
				 else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Greek)
				 {
					 tmp_Numericset[k][i][j] = s_SimpleNumericSet_Eng[k][i][j];
				 }
#endif
#if defined(CONFIG_PROD_LANG_SWEDISH)
				 else if(pSimpleKeyContexts->language == eSIMPLE_CHARSET_Swedish)
				 {
					 tmp_Numericset[k][i][j] = s_SimpleNumericSet_Eng[k][i][j];
				 }
#endif

			}
		}
	}

	for(k=0;k<2;k++)
	{
		for(i=0;i<NX_PRISM_SIMPLE_KEYBOARD_NUM_BTN_COUNT;i++)
		{
			for(j=0;j<NX_PRISM_SIMPLE_KEYBOARD_NUMERIC_COUNT;j++)
			{
				if(tmp_Numericset[k][i][j] == 0x0000)
				{
					utfItem[0] = '\0';
					utfItem[1] = '\0';
				}
				else
				{
					HxSTD_MemCopy(ucsitem,&tmp_Numericset[k][i][j],sizeof(HUINT16));
					HxSTD_MemSet(utfItem,0,sizeof(HUINT8) * 8);

					len = HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8,(HUINT16 *)ucsitem,(void *)utfItem, 8);
				}

				HxSTD_StrNCpy(s_SimpleNumericSet_Using[k][i][j], utfItem,HxSTD_StrLen(utfItem));
				s_SimpleNumericSet_Using[k][i][j][HxSTD_StrLen(utfItem)] = '\0';
			}
		}
	}

	return lRet;
}

static SIMPLE_Key_Charset_e	nx_simplekeyboard_convertLanguageStringToType(HCHAR *pLangFullName)
{
	if(HxSTD_StrCmp(pLangFullName,"English")==0)		return eSIMPLE_CHARSET_English;
	else if(HxSTD_StrCmp(pLangFullName,"German")==0)	return eSIMPLE_CHARSET_German;
	else if(HxSTD_StrCmp(pLangFullName,"Turkish")==0)	return eSIMPLE_CHARSET_Turkish;
	else if(HxSTD_StrCmp(pLangFullName,"Russian")==0)	return eSIMPLE_CHARSET_Russian;
	else if(HxSTD_StrCmp(pLangFullName,"Arabic")==0)	return eSIMPLE_CHARSET_Arabic;
	else if(HxSTD_StrCmp(pLangFullName,"French")==0)	return eSIMPLE_CHARSET_French;
	else if(HxSTD_StrCmp(pLangFullName,"Italian")==0)	return eSIMPLE_CHARSET_Italian;
	else if(HxSTD_StrCmp(pLangFullName,"Spanish")==0)	return eSIMPLE_CHARSET_Spanish;
	else if(HxSTD_StrCmp(pLangFullName,"Greek")==0)		return eSIMPLE_CHARSET_Greek;
	else if(HxSTD_StrCmp(pLangFullName,"Persian")==0)	return eSIMPLE_CHARSET_Persian;
	else if(HxSTD_StrCmp(pLangFullName,"Swedish")==0)	return eSIMPLE_CHARSET_Swedish;
#if defined (CONFIG_OP_UK_DTT)
	else if(HxSTD_StrCmp(pLangFullName,"Scottish Gaelic")==0)	return eSIMPLE_CHARSET_English;
	else if(HxSTD_StrCmp(pLangFullName,"Welsh")==0)		return eSIMPLE_CHARSET_English;
	else if(HxSTD_StrCmp(pLangFullName,"Irish")==0)		return eSIMPLE_CHARSET_English;
#endif
	else	return eSIMPLE_CHARSET_Max;
}

static ONDK_Result_t 	nx_simplekeyboard_SetLanguage(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	HERROR		hErr = ERR_OK;
	HCHAR		*pLangFullName = NULL;
	HCHAR		szLang639[8] ={0,};
	HxLANG_Id_e langId;

	hErr = NX_PORT_SYSTEM_GetMainLanguage(szLang639);
	if(hErr != ERR_OK)
	{
		//ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
		return ONDK_RET_IO;
	}

	langId = HLIB_LANG_639ToId(szLang639);
	pLangFullName = (HCHAR *)HLIB_LANG_IdToFullName(langId);
	if(pLangFullName == NULL)
	{
		//ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
		return ONDK_RET_IO;
	}

	pSimpleKeyContexts->language = nx_simplekeyboard_convertLanguageStringToType(pLangFullName);

#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, pLangFullName, ONDK_LANGUAGE_STRING_MAX_LENGTH);
	pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
#endif
	//ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ONDK_RET_OK;
}


static ONDK_Result_t 	nx_simplekeyboard_SetInputObject(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	ONDK_Rect_t 		stRect;
	ONDK_Result_t		lRet = ONDK_RET_OK;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SIMPLE_KEYBOARD_FRAME_X, NX_PRISM_SIMPLE_KEYBOARD_FRAME_Y, NX_PRISM_SIMPLE_KEYBOARD_FRAME_W, NX_PRISM_SIMPLE_KEYBOARD_FRAME_H);

	stRect = ONDK_Rect(NX_PRISM_SIMPLE_KEYBOARD_FRAME_X, NX_PRISM_SIMPLE_KEYBOARD_FRAME_Y, NX_PRISM_SIMPLE_KEYBOARD_FRAME_W, NX_PRISM_SIMPLE_KEYBOARD_FRAME_H);

#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	ONDK_GWM_COM_SimpleKeyboard_Create(NX_PRISM_SIMPLE_KEY_OBJ,&stRect, NX_PRISM_SIMPLE_KEY_CHAR_ID, pSimpleKeyContexts->szLanguageBtn);
#else
	ONDK_GWM_COM_SimpleKeyboard_Create(NX_PRISM_SIMPLE_KEY_OBJ,&stRect, NX_PRISM_SIMPLE_KEY_CHAR_ID, NULL);
#endif

	nx_simplekeyboard_SetCharSet(pSimpleKeyContexts);

	//inputbox text
	stRect = ONDK_Rect(NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_X+61, NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_Y+17, NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_W, NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_SIMPLE_KEY_INPUTBOX_TEXT_ID,&stRect,"");
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_SIMPLE_KEY_INPUTBOX_TEXT_ID,eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_SIMPLE_KEY_INPUTBOX_TEXT_ID,NX_PRISM_SIMPLE_FONT_NORMAL_SIZE);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SIMPLE_KEY_INPUTBOX_TEXT_ID,NX_PRISM_SIMPLE_KEYBOARD_FONT_COLOR,NX_PRISM_SIMPLE_KEYBOARD_FONT_COLOR);

	//inputbox cursor
	stRect = ONDK_Rect(NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_X+64, NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_Y+17, NX_PRISM_SIMPLE_KEYBOARD_INPUTBOXCUSOR_W, NX_PRISM_SIMPLE_KEYBOARD_INPUTBOXCUSOR_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID, NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_CURSOR);

	return lRet;

}

static ONDK_Result_t nx_simplekeyboard_MsgGwmKeyDown(Simple_Key_Contents_t *pSimpleKeyContexts, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 	nKeyId = p1;
	HINT32	len;
	HINT32	ptr_len;
	HINT32	total_len;
	HINT32	i;
	HUINT8	inputStr[2] = {0,};

	HInputEvent_t	*inputEvent = (HInputEvent_t *)hHandle;

	HINT32	objId = ONDK_GWM_COM_SimpleKeyboard_GetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ);
	ONDK_GWM_GuiObject_t cursor_obj = NULL;

	static HINT32	numericKeyValue = -1 ;
	static HUINT32 s_Simple_LimitedInterval = 0;
	static HUINT32 	s_numericInterval = 0;

	ONDK_Assert(pSimpleKeyContexts);

	cursor_obj = ONDK_GWM_Obj_GetObjectByID(NX_PRISM_SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID);

	if(s_Simple_CursorInterval + NX_PRISM_SIMPLE_KEYBOARD_CURSOR_INTERVAL <  VK_TIMER_GetSystemTick())
	{
		s_Simple_CursorInterval = VK_TIMER_GetSystemTick();
		if(ONDK_GWM_Obj_GetVisibleByObj(cursor_obj) == TRUE)
		{
			ONDK_GWM_Obj_SetVisibleByObj(cursor_obj,FALSE);
		}
		else
		{
			ONDK_GWM_Obj_SetVisibleByObj(cursor_obj,TRUE);
		}
	}

	if(s_Simple_LimitedInterval + NX_PRISM_SIMPLE_KEYBOARD_LIMITED_INTERVAL > VK_TIMER_GetSystemTick() || s_Simple_StartInterval+500 > VK_TIMER_GetSystemTick())
	{
		return ONDK_MESSAGE_CONSUMED;
	}

	switch (nKeyId)
	{
		case KEY_ARROWLEFT:

			if(objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+NX_PRISM_SIMPLE_KEYBOARD_SHOW_CHAR_COUNT+1 && objId > NX_PRISM_SIMPLE_KEY_CHAR_ID-1)
			{
				if(objId == NX_PRISM_SIMPLE_KEY_CHAR_ID)
				{
					if(pSimpleKeyContexts->firstVisibleOffset > 0)
					{
						pSimpleKeyContexts->firstVisibleOffset--;
						nx_simplekeyboard_SetCharSet(pSimpleKeyContexts);
						ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					}
					else if(pSimpleKeyContexts->firstVisibleOffset == 0)
					{
						for(i=0;i<NX_PRISM_SIMPLE_KEYBOARD_CHAR_COUNT-pSimpleKeyContexts->firstVisibleOffset-1;i++)
						{
							if(HxSTD_StrCmp(s_SimpleCharSet_Using[pSimpleKeyContexts->selectedCharSet][pSimpleKeyContexts->firstVisibleOffset+i+1]," ") == 0)
							{
								if(i > NX_PRISM_SIMPLE_KEYBOARD_SHOW_CHAR_COUNT)
								{
									pSimpleKeyContexts->firstVisibleOffset = i-NX_PRISM_SIMPLE_KEYBOARD_SHOW_CHAR_COUNT;
									nx_simplekeyboard_SetCharSet(pSimpleKeyContexts);
									ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,objId+NX_PRISM_SIMPLE_KEYBOARD_SHOW_CHAR_COUNT);
								}
								else
								{
									ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,objId+i);
								}
								ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
								break;
							}
						}
					}
				}
				else
				{
					ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,objId-1);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				}
			}
			else
			{
				if(objId > 1)
				{
					ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,objId-1);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				}
			}
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWRIGHT:

			if(objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+NX_PRISM_SIMPLE_KEYBOARD_SHOW_CHAR_COUNT+1&& objId > NX_PRISM_SIMPLE_KEY_CHAR_ID-1)
			{
				if(HxSTD_StrCmp(s_SimpleCharSet_Using[pSimpleKeyContexts->selectedCharSet][pSimpleKeyContexts->firstVisibleOffset+objId+1-NX_PRISM_SIMPLE_KEY_CHAR_ID]," ") != 0)
				{
					if(ONDK_GWM_COM_SimpleKeyboard_GetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ) == NX_PRISM_SIMPLE_KEY_CHAR_ID + NX_PRISM_SIMPLE_KEYBOARD_SHOW_CHAR_COUNT)
					{
						if(HxSTD_StrCmp(s_SimpleCharSet_Using[pSimpleKeyContexts->selectedCharSet][pSimpleKeyContexts->firstVisibleOffset+NX_PRISM_SIMPLE_KEYBOARD_SHOW_CHAR_COUNT+1]," ") != 0)
						{
							pSimpleKeyContexts->firstVisibleOffset++;
							nx_simplekeyboard_SetCharSet(pSimpleKeyContexts);
							ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
						}
					}
					else
					{
						ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,objId+1);
						ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					}
				}
				else
				{
					pSimpleKeyContexts->firstVisibleOffset=0;
					nx_simplekeyboard_SetCharSet(pSimpleKeyContexts);
					ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,NX_PRISM_SIMPLE_KEY_CHAR_ID);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				}
			}
			else
			{
#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				if(objId < NX_PRISM_SIMPLE_KEY_LANGUAGE_ID )
#else
				if(objId < NX_PRISM_SIMPLE_KEY_TOGGLE_ID )
#endif
				{
					ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(NX_PRISM_SIMPLE_KEY_OBJ,objId+1);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				}
			}
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWDOWN:

			if(objId < NX_PRISM_SIMPLE_KEY_CHAR_ID+NX_PRISM_SIMPLE_KEYBOARD_SHOW_CHAR_COUNT+1 && objId > NX_PRISM_SIMPLE_KEY_CHAR_ID-1)
			{
				ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_RESULT, (Handle_t)NULL, 0, 0, 2);
			}
			else
			{
#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				nx_simplekeyboard_KeyArrowDownWithLanguageBtn(pSimpleKeyContexts, objId);
#else
				nx_simplekeyboard_KeyArrowDown(pSimpleKeyContexts, objId);
#endif
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			}
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWUP:

			pSimpleKeyContexts->currentFocus = objId;
#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
			nx_simplekeyboard_KeyArrowUpWithLanguageBtn(pSimpleKeyContexts, objId);
#else
			nx_simplekeyboard_KeyArrowUp(pSimpleKeyContexts, objId);
#endif
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

			return ONDK_MESSAGE_CONSUMED;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_BACK:
			ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_RESULT, (Handle_t)NULL, 0, 0, 1);
			return ONDK_MESSAGE_CONSUMED;

		case KEY_STANDBY:
		case KEY_EXIT:
		case KEY_GUIDE:
		case KEY_MENU:
			ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_RESULT, (Handle_t)NULL, 0, 0, 0);
			return GWM_MESSAGE_PASS;

		case KEY_UNI_RETURN:
		{
			switch(objId)
			{
				//back
				case NX_PRISM_SIMPLE_KEY_BACK_ID:

					if(pSimpleKeyContexts->currentPtr== 0)
						break;
					len = nx_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr-1);
					ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(pSimpleKeyContexts->strbuf,pSimpleKeyContexts->currentPtr-1));
					total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));

					if(ptr_len==2)
					{
						HLIB_STR_RemoveAtBuffer(pSimpleKeyContexts->strbuf,len);
						HLIB_STR_SetAtBuffer(pSimpleKeyContexts->strbuf,total_len-2,0);
					}
					else
					{
						HLIB_STR_RemoveAtBuffer(pSimpleKeyContexts->strbuf,len);
						HLIB_STR_SetAtBuffer(pSimpleKeyContexts->strbuf,total_len-1,0);
					}

					pSimpleKeyContexts->currentPtr--;

					break;
				//space
				case NX_PRISM_SIMPLE_KEY_SPACE_ID:

					len = nx_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
					pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf," ",len);
					pSimpleKeyContexts->currentPtr++;

					break;

				//clear
				case NX_PRISM_SIMPLE_KEY_CLEAR_ID:

					len = nx_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);

					for(i=0;i<len;i++)
					{
						HLIB_STR_RemoveAtBuffer(pSimpleKeyContexts->strbuf,0);
					}
					HLIB_STR_SetAtBuffer(pSimpleKeyContexts->strbuf,0,0);
					pSimpleKeyContexts->currentPtr=0;
					break;

				//toggle
				case NX_PRISM_SIMPLE_KEY_TOGGLE_ID:

					if(pSimpleKeyContexts->selectedCharSet == NX_PRISM_SIMPLE_KEYBOARD_CHARSET_CHAR)
					{
						pSimpleKeyContexts->selectedCharSet =NX_PRISM_SIMPLE_KEYBOARD_CHARSET_NUM;
						pSimpleKeyContexts->currentFocus = NX_PRISM_SIMPLE_KEY_CHAR_ID+9;
					}
					else
					{
						pSimpleKeyContexts->selectedCharSet =NX_PRISM_SIMPLE_KEYBOARD_CHARSET_CHAR;
						pSimpleKeyContexts->currentFocus = NX_PRISM_SIMPLE_KEY_CHAR_ID+21;
					}

					pSimpleKeyContexts->firstVisibleOffset=0;
					nx_simplekeyboard_SetCharSet(pSimpleKeyContexts);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

					return ONDK_MESSAGE_CONSUMED;


				case NX_PRISM_SIMPLE_KEY_INPUTBOX_ID:

					break;

#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				case NX_PRISM_SIMPLE_KEY_LANGUAGE_ID:
					nx_simplekeyboard_ChangeLanguage(pSimpleKeyContexts);
					nx_simplekeyboard_SettingUsingTable(pSimpleKeyContexts);
					nx_simplekeyboard_SetCharSet(pSimpleKeyContexts);
					nx_simplekeyboard_SetCurLanguage(pSimpleKeyContexts);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					break;
#endif

				//char button
				default:
					if(nx_simplekeyboard_GetStringWidth(pSimpleKeyContexts,pSimpleKeyContexts->strbuf) > NX_PRISM_SIMPLE_KEYBOARD_MAX_CHAR_LENGTH)
						return ONDK_MESSAGE_CONSUMED;
//					if(HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf)) > NX_PRISM_SIMPLE_KEYBOARD_MAX_CHAR_LENGTH-1)
	//					return ONDK_MESSAGE_CONSUMED;

					len = nx_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
					pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf,s_SimpleCharSet_Using[pSimpleKeyContexts->selectedCharSet][pSimpleKeyContexts->firstVisibleOffset+objId-NX_PRISM_SIMPLE_KEY_CHAR_ID],len);
					pSimpleKeyContexts->currentPtr++;

			}

			nx_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
			ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_RESULT, (Handle_t)NULL, 0, 0, 0);

		}
		return ONDK_MESSAGE_CONSUMED;

		case KEY_MUTE:
		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
			return GWM_MESSAGE_PASS;

		case KEY_UNI_SPACE:
			len = nx_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
			pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf," ",len);
			pSimpleKeyContexts->currentPtr++;
			nx_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
			ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_RESULT, (Handle_t)NULL, 0, 0, 0);
			return ONDK_MESSAGE_CONSUMED;

		case KEY_UNI_BACKSPACE:

			if(pSimpleKeyContexts->currentPtr== 0)
				break;
			len = nx_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr-1);
			ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(pSimpleKeyContexts->strbuf,pSimpleKeyContexts->currentPtr-1));
			total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));

			if(ptr_len==2)
			{
				HLIB_STR_RemoveAtBuffer(pSimpleKeyContexts->strbuf,len);
				HLIB_STR_SetAtBuffer(pSimpleKeyContexts->strbuf,total_len-2,0);
			}
			else
			{
				HLIB_STR_RemoveAtBuffer(pSimpleKeyContexts->strbuf,len);
				HLIB_STR_SetAtBuffer(pSimpleKeyContexts->strbuf,total_len-1,0);
			}

			pSimpleKeyContexts->currentPtr--;
			nx_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
			ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_RESULT, (Handle_t)NULL, 0, 0, 0);
			return ONDK_MESSAGE_CONSUMED;

		default:
			if(nKeyId >0x0029 && nKeyId < 0x0040 && inputEvent->device_id == eKeyInput_RCU)
			{
				switch(nKeyId)
				{
					case KEY_0:
						if(numericKeyValue != 0)
							pSimpleKeyContexts->smsInput = 0;
						numericKeyValue = 0;
						break;
					case KEY_1:
						if(numericKeyValue != 1)
							pSimpleKeyContexts->smsInput = 0;
						numericKeyValue = 1;
						break;
					case KEY_2:
						if(numericKeyValue != 2)
							pSimpleKeyContexts->smsInput = 0;
						numericKeyValue = 2;
						break;
					case KEY_3:
						if(numericKeyValue != 3)
							pSimpleKeyContexts->smsInput = 0;
						numericKeyValue = 3;
						break;
					case KEY_4:
						if(numericKeyValue != 4)
							pSimpleKeyContexts->smsInput = 0;
						numericKeyValue = 4;
						break;
					case KEY_5:
						if(numericKeyValue != 5)
							pSimpleKeyContexts->smsInput = 0;
						numericKeyValue = 5;
						break;
					case KEY_6:
						if(numericKeyValue != 6)
							pSimpleKeyContexts->smsInput = 0;
						numericKeyValue = 6;
						break;
					case KEY_7:
						if(numericKeyValue != 7)
							pSimpleKeyContexts->smsInput = 0;
						numericKeyValue = 7;
						break;
					case KEY_8:
						if(numericKeyValue != 8)
							pSimpleKeyContexts->smsInput = 0;
						numericKeyValue = 8;
						break;
					case KEY_9:
						if(numericKeyValue != 9)
							pSimpleKeyContexts->smsInput = 0;
						numericKeyValue = 9;
						break;
				}

				if(s_numericInterval + NX_PRISM_SIMPLE_KEYBOARD_NUMERIC_INTERVAL >	VK_TIMER_GetSystemTick())
				{
					//change string
					//delete
					if(pSimpleKeyContexts->currentPtr!= 0)
					{
						len = nx_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr-1);
						ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(pSimpleKeyContexts->strbuf,pSimpleKeyContexts->currentPtr-1));
						total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));

						if(ptr_len==2)
						{
							HLIB_STR_RemoveAtBuffer(pSimpleKeyContexts->strbuf,len);
							HLIB_STR_SetAtBuffer(pSimpleKeyContexts->strbuf,total_len-2,0);
						}
						else
						{
							HLIB_STR_RemoveAtBuffer(pSimpleKeyContexts->strbuf,len);
							HLIB_STR_SetAtBuffer(pSimpleKeyContexts->strbuf,total_len-1,0);
						}

						pSimpleKeyContexts->currentPtr--;
					}

					//write
					if(s_SimpleNumericSet_Using[1][numericKeyValue][pSimpleKeyContexts->smsInput][0]=='\0')
						pSimpleKeyContexts->smsInput = 0;

					len = nx_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
					pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf,s_SimpleNumericSet_Using[1][numericKeyValue][pSimpleKeyContexts->smsInput++],len);

					pSimpleKeyContexts->currentPtr++;

				}
				else
				{
					//write
					len = nx_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
					if(numericKeyValue == 1 && pSimpleKeyContexts->smsInput == 0)
					{
						pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf," ",len);
						pSimpleKeyContexts->smsInput++;
					}
					else pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf,s_SimpleNumericSet_Using[1][numericKeyValue][0],len);

					pSimpleKeyContexts->currentPtr++;
					pSimpleKeyContexts->smsInput = 0;
				}
/*
				if(pSimpleKeyContexts->inputMode == NX_PRISM_KEYBOARD_INPUT_PASSWD)
				{
					pSimpleKeyContexts->pwOnOff = 1;
					ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW,KEYBOARD_PW_INTERVAL);
				}
*/
				s_numericInterval = VK_TIMER_GetSystemTick();

				nx_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				return ONDK_MESSAGE_CONSUMED;
			}

			if(nKeyId >0x0020 && nKeyId < 0x007F)//unicode 값 직접 사용
			{
				inputStr[0] = nKeyId;
				inputStr[1] = '\0';

				len = nx_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
				pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf,inputStr,len);

				pSimpleKeyContexts->currentPtr++;

				if(nx_simplekeyboard_GetStringWidth(pSimpleKeyContexts,pSimpleKeyContexts->strbuf) > NX_PRISM_SIMPLE_KEYBOARD_MAX_CHAR_LENGTH)
				{
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					return ONDK_MESSAGE_CONSUMED;
				}

				nx_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
				ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_RESULT, (Handle_t)NULL, 0, 0, 0);
			}

			return ONDK_MESSAGE_CONSUMED;

	}

	return GWM_RET_OK;
}

//MSG_GWM_TIMER
static ONDK_Result_t nx_simplekeyboard_MsgGwmTimer(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulTimerID = p1;
	ONDK_GWM_GuiObject_t obj;

	if(ulTimerID == GWMPOPUP_TIMER_SIMPLEKEYBOARD)
	{
		obj = ONDK_GWM_Obj_GetObjectByID(NX_PRISM_SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID);

		if(ONDK_GWM_Obj_GetVisibleByObj(obj) == TRUE)
		{
			ONDK_GWM_Obj_SetVisibleByObj(obj,FALSE);
			//ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			ONDK_GWM_APP_InvalidateObject(NX_PRISM_SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID);
		}
		else
		{
			ONDK_GWM_Obj_SetVisibleByObj(obj,TRUE);
			//ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			ONDK_GWM_APP_InvalidateObject(NX_PRISM_SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID);
		}
	}

	ONDK_Assert(pSimpleKeyContexts);

	return GWM_RET_OK;
}

static ONDK_Result_t 	nx_simplekeyboard_SetInputBoxString(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;
	ONDK_Rect_t			rect;
	static HxSTR_t		*s_pstString = NULL;
	HUINT32 			str_width = 0;

	if(s_pstString != NULL)
		HLIB_STR_Delete(s_pstString);
	s_pstString = HLIB_STR_Clone(pSimpleKeyContexts->strbuf);

	if(s_pstString != NULL)
		str_width = nx_simplekeyboard_GetStringWidth(pSimpleKeyContexts,s_pstString);
	else str_width = 0;

#if defined(CONFIG_PROD_LANG_ARABIC)
	// arabic
	if (NX_PRISM_ARABIC_SUPPORT_CheckUnicodeArabicCharacterInString(HLIB_STR_toUCS2(s_pstString)))
	{
		HxSTR_t *pTempStr = NULL;
		pTempStr = NX_PRISM_ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(s_pstString);

		if (pTempStr == NULL)
		{
			HxLOG_Debug("[%s,%d] Failed Convert To VisualAra!!\n",__FUNCTION__,__LINE__);
		}
		else
		{
			s_pstString = pTempStr;
			str_width = nx_simplekeyboard_GetStringWidth(pSimpleKeyContexts,s_pstString);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SIMPLE_KEY_INPUTBOX_TEXT_ID, TEXT_ALIGN_RIGHT);
			// calc cursor position
			rect.x = NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_W + NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_X + 57 - str_width;
		}
	}
	else	// NOT ARABIC
#endif
	{
		ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SIMPLE_KEY_INPUTBOX_TEXT_ID, TEXT_ALIGN_LEFT);
		// calc cursor position
		rect.x = NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_X+64 + str_width;
	}

	ONDK_GWM_COM_Text_SetText(NX_PRISM_SIMPLE_KEY_INPUTBOX_TEXT_ID,HLIB_STR_GetBuffer(s_pstString));

	rect.y = NX_PRISM_SIMPLE_KEYBOARD_INPUTBOX_Y+17;
	rect.w = NX_PRISM_SIMPLE_KEYBOARD_INPUTBOXCUSOR_W;
	rect.h = NX_PRISM_SIMPLE_KEYBOARD_INPUTBOXCUSOR_H;
	ONDK_GWM_Obj_Resize(NX_PRISM_SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID, &rect);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return lRet;
}


static HINT32 nx_simplekeyboard_GetByteSize(HxSTR_t *str, HUINT32 start, HUINT32 cnt)
{
	HINT32		lRet = 0;
	HINT32 		i;

	for(i=start;i<cnt;i++)
	{
		lRet += HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(str,i));
	}

	return lRet;
}

static HINT32		nx_simplekeyboard_GetStringWidth(Simple_Key_Contents_t *pSimpleKeyContexts,HxSTR_t *str)
{
	HINT32		i=0;
	HINT32		str_width=0;
	HINT32		ptr_len=0;
	HINT32		len=0;
	ONDKFontHandle		fontTempHandle;

	fontTempHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold,28);

	ptr_len = nx_simplekeyboard_GetByteSize(str,0,pSimpleKeyContexts->currentPtr);
	len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(str));

	for(i=ptr_len;i<len;i++)
	{
		HLIB_STR_RemoveAtBuffer(str,ptr_len);
	}
	HLIB_STR_SetAtBuffer(str,ptr_len,0);

	str_width = ONDK_FONT_GetStringWidth(fontTempHandle,HLIB_STR_GetBuffer(str));

	ONDK_FONT_Release(fontTempHandle);

	return str_width;
}

static void 	nx_simplekeyboard_SetCharSet(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	HINT32 		i = 0;

	for(i=0;i<NX_PRISM_SIMPLE_KEYBOARD_SHOW_CHAR_COUNT+1;i++)
	{
		ONDK_GWM_COM_SimpleKeyboard_SetCharValue(NX_PRISM_SIMPLE_KEY_OBJ,i,s_SimpleCharSet_Using[pSimpleKeyContexts->selectedCharSet][i+pSimpleKeyContexts->firstVisibleOffset]);
	}
}

#if defined(NX_PRISM_SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
static void 	nx_simplekeyboard_SetCurLanguage(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	ONDK_GWM_COM_SimpleKeyboard_SetCurLanguage(NX_PRISM_SIMPLE_KEY_OBJ, pSimpleKeyContexts->szLanguageBtn);
}
#endif


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t		NX_PRISM_SimpleKeyboard_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = GWM_RET_OK;
	static ONDK_Result_t	s_upkey =ONDK_RET_OK;
	Simple_Key_Contents_t	*pSimpleKeyContexts = nx_simplekeyboard_KeyboardGetContents();
	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(0x%x) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			lRet = nx_simplekeyboard_MsgGwmCreate(pSimpleKeyContexts, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			lRet = nx_simplekeyboard_MsgGwmTimer(pSimpleKeyContexts, p1, p2, p3);
			break;

		case MSG_GWM_KEYDOWN:
			lRet = nx_simplekeyboard_MsgGwmKeyDown(pSimpleKeyContexts, hHandle, p1, p2, p3);
			s_upkey = lRet;
			break;

		case MSG_GWM_KEYUP:
			lRet = s_upkey;
			break;

		case MSG_APP_KEYBOARD_DESTROY_KEYBOARD:
			ONDK_GWM_APP_Destroy(0);
			break;

		case MSG_GWM_DESTROY:
			lRet = nx_simplekeyboard_MsgGwmDestroy(pSimpleKeyContexts, p1, p2, p3);
			break;

		case MSG_SIGNAL_REMOTESTING:
			lRet = nx_simplekeyboard_RemoteAppMsg(pSimpleKeyContexts, hHandle, p1, p2, p3);
			break;

	}

	if(lRet != GWM_RET_OK)
	{
		return lRet;
	}

	lRet = ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
	if(lRet == GWM_RET_NO_OBJECT || lRet == GWM_MESSAGE_BREAK)
	{
		lRet = ONDK_MESSAGE_CONSUMED;
	}

	return lRet;

}


HCHAR	*NX_PRISM_SimpleKeyBoard_GetResultString(void)
{
	Simple_Key_Contents_t	*pSimpleKeyContexts = nx_simplekeyboard_KeyboardGetContents();

	if((pSimpleKeyContexts == NULL) || (pSimpleKeyContexts->strbuf == NULL))
	{
		HxLOG_Debug("Simple KeyBoard String is NULL!!\n");
		return NULL;
	}

	return HLIB_STD_StrDup(HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

