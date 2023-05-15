/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include	<ondk.h>
#include	<ondk_type.h>
#include	<ondk_gfx.h>
#include	<hlib.h>
#include	<ondk.h>
#include	<oapi.h>
#include	<hapi.h>
#include	<apk.h>
#include 	<prism.h>

#if defined(CONFIG_APUI_NATIVE_APP)
/*
 *
 */
#else

#include "ap_simplekeyboard.h"
#include "arabic_support.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

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

#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	HUINT8			szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH];
#endif

	HxSTR_t			*strbuf;
} Simple_Key_Contents_t;

HCHAR *s_SimpleCharSet_Using[2][SIMPLE_KEYBOARD_CHAR_COUNT];

HCHAR *s_SimpleNumericSet_Using[2][SIMPLE_KEYBOARD_NUM_BTN_COUNT][SIMPLE_KEYBOARD_NUMERIC_COUNT];

HUINT16 s_SimpleNumericSet_Eng[2][SIMPLE_KEYBOARD_NUM_BTN_COUNT][SIMPLE_KEYBOARD_NUMERIC_COUNT] =
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

HUINT16 s_SimpleNumericSet_Ger[2][SIMPLE_KEYBOARD_NUM_BTN_COUNT][SIMPLE_KEYBOARD_NUMERIC_COUNT] =
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


HUINT16 s_SimpleCharSet_Eng[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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

HUINT16 s_SimpleCharSet_Ger[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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

HUINT16 s_SimpleCharSet_Tur[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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

HUINT16 s_SimpleCharSet_Rus[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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

HUINT16 s_SimpleCharSet_Fre[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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

HUINT16 s_SimpleCharSet_Ara[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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

HUINT16 s_SimpleCharSet_Per[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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

HUINT16 s_SimpleCharSet_Ita[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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

HUINT16 s_SimpleCharSet_Spa[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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

HUINT16 s_SimpleCharSet_Gre[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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

HUINT16 s_SimpleCharSet_Swe[2][SIMPLE_KEYBOARD_CHAR_COUNT] =
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


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static Simple_Key_Contents_t		s_Simple_Keyboard_Contents;
static HUINT32 						s_Simple_CursorInterval = 0;
static HUINT32 						s_Simple_StartInterval = 0;
static HBOOL						s_bIsNewWord = FALSE;
static HUINT32						s_nNumericKeyValue =0;
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define _________PROTO_TYPE__________

static Simple_Key_Contents_t*	 local_simplekeyboard_KeyboardGetContents(void);

static ONDK_Result_t	local_simplekeyboard_MsgGwmCreate(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_simplekeyboard_MsgGwmDestroy(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_simplekeyboard_InitData(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_simplekeyboard_FreeData(void);
static ONDK_Result_t 	local_simplekeyboard_RemoteAppMsg(Simple_Key_Contents_t *pSimpleKeyContexts, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_simplekeyboard_SettingUsingTable(Simple_Key_Contents_t *pSimpleKeyContexts);

static ONDK_Result_t 	local_simplekeyboard_SetLanguage(Simple_Key_Contents_t *pSimpleKeyContexts);

static ONDK_Result_t 	local_simplekeyboard_SetInputObject(Simple_Key_Contents_t *pSimpleKeyContexts);

static ONDK_Result_t 	local_simplekeyboard_MsgGwmKeyDown(Simple_Key_Contents_t *pSimpleKeyContexts, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_simplekeyboard_MsgGwmTimer(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_simplekeyboard_SetInputBoxString(Simple_Key_Contents_t *pSimpleKeyContexts);

static HINT32 			local_simplekeyboard_GetByteSize(HxSTR_t *str, HUINT32 start, HUINT32 cnt);
static HINT32 			local_simplekeyboard_GetStringWidth(Simple_Key_Contents_t *pSimpleKeyContexts, HxSTR_t *str);
static void 			local_simplekeyboard_SetCharSet(Simple_Key_Contents_t *pSimpleKeyContexts);
static ONDK_Result_t 	local_simplekeyboard_KeyArrowDown(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId);
static ONDK_Result_t	local_simplekeyboard_KeyArrowUp(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId);
#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
static ONDK_Result_t	local_simplekeyboard_KeyArrowDownWithLanguageBtn(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId);
static ONDK_Result_t	local_simplekeyboard_KeyArrowUpWithLanguageBtn(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId);
static void 			local_simplekeyboard_SetCurLanguage(Simple_Key_Contents_t *pSimpleKeyContexts);
#endif

static void				local_simplekeyboard_ChangeLanguage(Simple_Key_Contents_t *pSimpleKeyContexts);

#define _________GLOBAL_FUNC__________
/*******************************************************************
 *
 * usage:
 *		hHandle = (option)
 *		lParam1 = (option)
 *		lParam2 = (option)
 *		lParam3 = (option)
 *
 *******************************************************************/
ONDK_Result_t		AP_SimpleKeyboardMain_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = GWM_RET_OK;
	static ONDK_Result_t	s_upkey =ONDK_RET_OK;
	Simple_Key_Contents_t	*pSimpleKeyContexts = local_simplekeyboard_KeyboardGetContents();
	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(0x%x) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			lRet = local_simplekeyboard_MsgGwmCreate(pSimpleKeyContexts, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			lRet = local_simplekeyboard_MsgGwmTimer(pSimpleKeyContexts, p1, p2, p3);
			break;

		case MSG_GWM_KEYDOWN:
			lRet = local_simplekeyboard_MsgGwmKeyDown(pSimpleKeyContexts, hHandle, p1, p2, p3);
			s_upkey = lRet;
			break;

		case MSG_GWM_KEYUP:
			lRet = s_upkey;
			break;

		case MSG_GWM_DESTROY:
			lRet = local_simplekeyboard_MsgGwmDestroy(pSimpleKeyContexts, p1, p2, p3);
			break;

		case MSG_SIGNAL_REMOTESTING:
			lRet = local_simplekeyboard_RemoteAppMsg(pSimpleKeyContexts, hHandle, p1, p2, p3);
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

#define _________LOCAL_FUNC__________

static Simple_Key_Contents_t*	 local_simplekeyboard_KeyboardGetContents(void)
{
	return &s_Simple_Keyboard_Contents;
}


static ONDK_Result_t local_simplekeyboard_MsgGwmCreate(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		hRet = ONDK_RET_OK;

	s_Simple_StartInterval = VK_TIMER_GetSystemTick();

	hRet = local_simplekeyboard_SetLanguage(pSimpleKeyContexts);
	hRet = local_simplekeyboard_InitData(pSimpleKeyContexts, p1,p2,p3);

	hRet = local_simplekeyboard_SettingUsingTable(pSimpleKeyContexts);

	hRet = local_simplekeyboard_SetInputObject(pSimpleKeyContexts);
	hRet = local_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_SIMPLEKEYBOARD, SIMPLE_KEYBOARD_CURSOR_INTERVAL);

	return hRet;
}

static ONDK_Result_t local_simplekeyboard_MsgGwmDestroy(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_simplekeyboard_FreeData();

	return GWM_RET_OK;
}

static ONDK_Result_t local_simplekeyboard_InitData(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			i,j,k;
	ONDK_Result_t	lRet = ONDK_RET_OK;

	pSimpleKeyContexts->selectedCharSet = SIMPLE_KEYBOARD_CHARSET_CHAR;
	pSimpleKeyContexts->firstVisibleOffset = 0;
	pSimpleKeyContexts->currentPtr = 0;
	pSimpleKeyContexts->currentFocus = 0;

	for(i=0;i<2;i++)
	{
		for(j=0;j<SIMPLE_KEYBOARD_CHAR_COUNT;j++)
		{
			s_SimpleCharSet_Using[i][j] = AP_Malloc(sizeof(HUINT8)*8);
			HxSTD_MemSet(s_SimpleCharSet_Using[i][j], 0, sizeof(HUINT8)*8);		//	Memory치는 경우 존재, 넉넉히 받자.
		}
	}

	//numeric array setting
	for(k=0;k<2;k++)
	{
		for(i=0;i<SIMPLE_KEYBOARD_NUM_BTN_COUNT;i++)
		{
			for(j=0;j<SIMPLE_KEYBOARD_NUMERIC_COUNT;j++)
			{
				s_SimpleNumericSet_Using[k][i][j] = AP_Malloc(sizeof(HUINT8)*8);
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

static ONDK_Result_t local_simplekeyboard_FreeData(void)
{
	HINT32			i,j;
	ONDK_Result_t	lRet = ONDK_RET_OK;

	for(i=0;i<2;i++)
	{
		for(j=0;j<SIMPLE_KEYBOARD_CHAR_COUNT;j++)
		{
			AP_Free(s_SimpleCharSet_Using[i][j]);
		}
	}

	return lRet;
}
static ONDK_Result_t local_simplekeyboard_RemoteAppMsg(Simple_Key_Contents_t *pSimpleKeyContexts, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32					nNumChar = 0;
	HINT8					*str_val = (HINT8 *)p1;

	if(HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,str_val) > SIMPLE_KEYBOARD_MAX_CHAR_LENGTH-1)
	{
		return GWM_MESSAGE_BREAK;
	}
	if(pSimpleKeyContexts->strbuf != NULL)
		HLIB_STR_Delete(pSimpleKeyContexts->strbuf);
	pSimpleKeyContexts->strbuf = HLIB_STR_New(str_val);
	nNumChar = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
	pSimpleKeyContexts->currentPtr = (nNumChar > 0) ? (HUINT32)nNumChar : 0;
	(void)local_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);

	return GWM_MESSAGE_BREAK;
}

static ONDK_Result_t	local_simplekeyboard_KeyArrowDown(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;

	// 0~11 : input, 12~15 : back, 16~18 : clear, 19~22 : space, 23~25 : toggle

	switch(objId)
	{
		case SIMPLE_KEY_INPUTBOX_ID:
			if(pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+11 )
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+5);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case SIMPLE_KEY_BACK_ID:
			if(pSimpleKeyContexts->selectedCharSet == SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+15) || (pSimpleKeyContexts->currentFocus < SIMPLE_KEY_CHAR_ID+12))
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+12);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case SIMPLE_KEY_CLEAR_ID:
			if(pSimpleKeyContexts->selectedCharSet == SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+18) || (pSimpleKeyContexts->currentFocus < SIMPLE_KEY_CHAR_ID+16))
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+16);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case SIMPLE_KEY_SPACE_ID:
			if(pSimpleKeyContexts->selectedCharSet == SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+22) || (pSimpleKeyContexts->currentFocus < SIMPLE_KEY_CHAR_ID+19))
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+19);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case SIMPLE_KEY_TOGGLE_ID:
			if(pSimpleKeyContexts->selectedCharSet == SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+25) || (pSimpleKeyContexts->currentFocus < SIMPLE_KEY_CHAR_ID+23))
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+23);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;
	}

	return lRet;
}

static ONDK_Result_t local_simplekeyboard_KeyArrowUp(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId)
{
	ONDK_Result_t		lRet = ONDK_MESSAGE_CONSUMED;

	// 0~11 : input, 12~15 : back, 16~18 : clear, 19~22 : space, 23~25 : toggle

	if(objId < SIMPLE_KEY_CHAR_ID+12 && objId > SIMPLE_KEY_CHAR_ID-1 )
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,SIMPLE_KEY_INPUTBOX_ID);
	}
	else if(objId < SIMPLE_KEY_CHAR_ID+16 && objId > SIMPLE_KEY_CHAR_ID+11)
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,SIMPLE_KEY_BACK_ID);
	}
	else if(objId < SIMPLE_KEY_CHAR_ID+19 && objId > SIMPLE_KEY_CHAR_ID+15)
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,SIMPLE_KEY_CLEAR_ID);
	}
	else if(objId < SIMPLE_KEY_CHAR_ID+23 && objId > SIMPLE_KEY_CHAR_ID+18)
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,SIMPLE_KEY_SPACE_ID);
	}
	else if(objId < SIMPLE_KEY_CHAR_ID+26 && objId > SIMPLE_KEY_CHAR_ID+22)
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,SIMPLE_KEY_TOGGLE_ID);
	}
	else
	{
		HxLOG_Debug("[%s:%d] not available KEY_UP on Simple Keyboard at this time! (pSimpleKeyContexts->currentFocus : [%d], objId : [%d])\n", __FUNCTION__, __LINE__, pSimpleKeyContexts->currentFocus, objId);
		lRet = ONDK_MESSAGE_NONCONSUMED;
	}

	return lRet;
}


#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
static ONDK_Result_t local_simplekeyboard_KeyArrowDownWithLanguageBtn(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;

	switch(objId)
	{
		case SIMPLE_KEY_INPUTBOX_ID:
			if(pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+9 )
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+5);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case SIMPLE_KEY_BACK_ID:
			if(pSimpleKeyContexts->selectedCharSet == SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+13) || (pSimpleKeyContexts->currentFocus < SIMPLE_KEY_CHAR_ID+10))
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+10);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case SIMPLE_KEY_CLEAR_ID:
			if(pSimpleKeyContexts->selectedCharSet == SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+15) || (pSimpleKeyContexts->currentFocus < SIMPLE_KEY_CHAR_ID+13))
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+13);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case SIMPLE_KEY_SPACE_ID:
			if(pSimpleKeyContexts->selectedCharSet == SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+18) || (pSimpleKeyContexts->currentFocus < SIMPLE_KEY_CHAR_ID+15))
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+15);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case SIMPLE_KEY_TOGGLE_ID:
			if(pSimpleKeyContexts->selectedCharSet == SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+9);
				break;
			}
			if((pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+20) || (pSimpleKeyContexts->currentFocus < SIMPLE_KEY_CHAR_ID+18))
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+18);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;

		case SIMPLE_KEY_LANGUAGE_ID:
			if(pSimpleKeyContexts->selectedCharSet == SIMPLE_KEYBOARD_CHARSET_NUM)
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+9);
				break;
			}

			if((pSimpleKeyContexts->currentFocus > SIMPLE_KEY_CHAR_ID+22) || (pSimpleKeyContexts->currentFocus < SIMPLE_KEY_CHAR_ID+20))
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CHAR_ID+20);
			}
			else
			{
				ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, pSimpleKeyContexts->currentFocus);
			}
			break;
	}

	return lRet;
}

static ONDK_Result_t local_simplekeyboard_KeyArrowUpWithLanguageBtn(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 objId)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;

	if((objId < SIMPLE_KEY_CHAR_ID+10) && (objId > SIMPLE_KEY_CHAR_ID-1))
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_INPUTBOX_ID);
	}
	else if((objId < SIMPLE_KEY_CHAR_ID+13) && (objId > SIMPLE_KEY_CHAR_ID+9))
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_BACK_ID);
	}
	else if((objId < SIMPLE_KEY_CHAR_ID+15) && (objId > SIMPLE_KEY_CHAR_ID+12))
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_CLEAR_ID);
	}
	else if((objId < SIMPLE_KEY_CHAR_ID+18) && (objId > SIMPLE_KEY_CHAR_ID+14))
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_SPACE_ID);
	}
	else if((objId < SIMPLE_KEY_CHAR_ID+20) && (objId > SIMPLE_KEY_CHAR_ID+17))
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_TOGGLE_ID);
	}
	else if((objId <= SIMPLE_KEY_CHAR_ID+22) && (objId > SIMPLE_KEY_CHAR_ID+19))
	{
		ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ, SIMPLE_KEY_LANGUAGE_ID);
	}

	return lRet;
}
#endif


static void		local_simplekeyboard_ChangeLanguage(Simple_Key_Contents_t *pSimpleKeyContexts)
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
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "English", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_German:
#if defined(CONFIG_PROD_LANG_GERMAN)
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "German", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Turkish:
#if defined(CONFIG_PROD_LANG_TURKISH)
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Turkish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Russian:
#if defined(CONFIG_PROD_LANG_RUSSIAN)
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Russian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Arabic:
#if defined(CONFIG_PROD_LANG_ARABIC)
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Arabic", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_French:
#if defined(CONFIG_PROD_LANG_FRENCH)
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "French", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Italian:
#if defined(CONFIG_PROD_LANG_ITALIAN)
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Italian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Spanish:
#if defined(CONFIG_PROD_LANG_SPANISH)
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Spanish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Greek:
#if defined(CONFIG_PROD_LANG_GREEK)
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Greek", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Persian:
#if defined(CONFIG_PROD_LANG_PERSIAN)
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Persian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
			#endif
				bFoundNextCharSet = TRUE;
#endif
				break;
			case eSIMPLE_CHARSET_Swedish:
#if defined(CONFIG_PROD_LANG_SWEDISH)
			#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
			#if defined(CONFIG_OP_TELENOR) // Requested from OP
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Svenska", ONDK_LANGUAGE_STRING_MAX_LENGTH);
			#else
				HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Swedish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
			#endif
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


static ONDK_Result_t local_simplekeyboard_SettingUsingTable(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	HUINT8			utfItem[8] = {0, };
	HUINT16 		ucsitem[2] = {0, };
	HINT32			len = 0;
	HINT32			i,j,k;

	HINT16 			tmp_Charset[2][SIMPLE_KEYBOARD_CHAR_COUNT];
	HINT16			tmp_Numericset[2][SIMPLE_KEYBOARD_NUM_BTN_COUNT][SIMPLE_KEYBOARD_NUMERIC_COUNT];

	ONDK_Result_t	lRet = ONDK_RET_OK;

	for(i=0;i<2;i++)
	{
		 for(j=0;j<SIMPLE_KEYBOARD_CHAR_COUNT;j++)
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
		for(j=0;j<SIMPLE_KEYBOARD_CHAR_COUNT;j++)
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
				if(len)
				{
					// no work
				}
				//alen = HLIB_CP_NumOfBytes (eHxCP_UNICODE_UTF8, utfItem);
				//ONDK_Debug("[%s:%d] len(%d) utfItem(, 0x%02d, 0x%02d, 0x%02d, 0x%02d)\r\n", __FUNCTION__, __LINE__, len, utfItem[0], utfItem[1], utfItem[2], utfItem[3]);
				//HxLOG_Print("[%s:%d] len(%d) utfItem(, 0x%02d, 0x%02d, 0x%02d, 0x%02d)\r\n", __FUNCTION__, __LINE__, len, utfItem[0], utfItem[1], utfItem[2], utfItem[3]);
			}

			HxSTD_StrNCpy(s_SimpleCharSet_Using[i][j], utfItem,HxSTD_StrLen(utfItem));
			s_SimpleCharSet_Using[i][j][HxSTD_StrLen(utfItem)] = '\0';

		}
	}


	for(k=0;k<2;k++)
	{
		for(i=0;i<SIMPLE_KEYBOARD_NUM_BTN_COUNT;i++)
		{
			for(j=0;j<SIMPLE_KEYBOARD_NUMERIC_COUNT;j++)
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
		for(i=0;i<SIMPLE_KEYBOARD_NUM_BTN_COUNT;i++)
		{
			for(j=0;j<SIMPLE_KEYBOARD_NUMERIC_COUNT;j++)
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
					if(len)
					{
						// no work
					}
				}

				HxSTD_StrNCpy(s_SimpleNumericSet_Using[k][i][j], utfItem,HxSTD_StrLen(utfItem));
				s_SimpleNumericSet_Using[k][i][j][HxSTD_StrLen(utfItem)] = '\0';
			}
		}
	}

	return lRet;
}

static SIMPLE_Key_Charset_e	local_simplekeyboard_convertLanguageStringToType(HCHAR *pLangFullName)
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

static ONDK_Result_t 	local_simplekeyboard_SetLanguage(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	HERROR		hErr = ERR_OK;
	HCHAR		*pLangFullName = NULL;
	HCHAR		szLang639[8] ={0,};
	HxLANG_Id_e langId;

	hErr = APK_SCENARIO_GetMainLanguage(szLang639);
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

	pSimpleKeyContexts->language = local_simplekeyboard_convertLanguageStringToType(pLangFullName);

#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
#if defined(CONFIG_OP_TELENOR) // Requested from OP
	if (langId == eLangID_Swedish)
	{
		HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, "Svenska", ONDK_LANGUAGE_STRING_MAX_LENGTH);
	}
	else
	{
		HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, pLangFullName, ONDK_LANGUAGE_STRING_MAX_LENGTH);
	}
#else
	HxSTD_StrNCpy(pSimpleKeyContexts->szLanguageBtn, pLangFullName, ONDK_LANGUAGE_STRING_MAX_LENGTH);
#endif
	pSimpleKeyContexts->szLanguageBtn[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
#endif
	//ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ONDK_RET_OK;
}


static ONDK_Result_t 	local_simplekeyboard_SetInputObject(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	ONDK_Rect_t 		stRect;
	ONDK_Result_t		lRet = ONDK_RET_OK;

	ONDK_GWM_APP_SetAppArea(AP_SIMPLE_KEYBOARD_FRAME_X, AP_SIMPLE_KEYBOARD_FRAME_Y, AP_SIMPLE_KEYBOARD_FRAME_W, AP_SIMPLE_KEYBOARD_FRAME_H);

	stRect = ONDK_Rect(AP_SIMPLE_KEYBOARD_FRAME_X,AP_SIMPLE_KEYBOARD_FRAME_Y,AP_SIMPLE_KEYBOARD_FRAME_W,AP_SIMPLE_KEYBOARD_FRAME_H);

#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
	ONDK_GWM_SimpleKeyboard_Create(SIMPLE_KEY_OBJ,&stRect, SIMPLE_KEY_CHAR_ID, pSimpleKeyContexts->szLanguageBtn);
#else
	ONDK_GWM_SimpleKeyboard_Create(SIMPLE_KEY_OBJ,&stRect, SIMPLE_KEY_CHAR_ID, NULL);
#endif

	local_simplekeyboard_SetCharSet(pSimpleKeyContexts);

	//inputbox text
	stRect = ONDK_Rect(AP_SIMPLE_KEYBOARD_INPUTBOX_X+61,AP_SIMPLE_KEYBOARD_INPUTBOX_Y+17,AP_SIMPLE_KEYBOARD_INPUTBOX_W,AP_SIMPLE_KEYBOARD_INPUTBOX_H);
	ONDK_GWM_COM_Text_Create(SIMPLE_KEY_INPUTBOX_TEXT_ID,&stRect,"");
	ONDK_GWM_COM_Text_SetFont(SIMPLE_KEY_INPUTBOX_TEXT_ID,eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(SIMPLE_KEY_INPUTBOX_TEXT_ID,FONT_NORMAL_SIZE);
	ONDK_GWM_COM_Text_ChangeColor(SIMPLE_KEY_INPUTBOX_TEXT_ID,FONT_COLOR,FONT_COLOR);

	//inputbox cursor
	stRect = ONDK_Rect(AP_SIMPLE_KEYBOARD_INPUTBOX_X+64,AP_SIMPLE_KEYBOARD_INPUTBOX_Y+17,AP_SIMPLE_KEYBOARD_INPUTBOXCUSOR_W,AP_SIMPLE_KEYBOARD_INPUTBOXCUSOR_H);
	ONDK_GWM_COM_Image_Create(SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID, SIMPLE_KEYBOARD_INPUTBOX_CURSOR);

	return lRet;

}

static ONDK_Result_t local_simplekeyboard_MsgGwmKeyDown(Simple_Key_Contents_t *pSimpleKeyContexts, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 	nKeyId = p1;
	HINT32	len;
	HINT32	ptr_len;
	HINT32	total_len;
	HINT32	i;
	HUINT8	inputStr[2] = {0,};

	HInputEvent_t	*inputEvent = (HInputEvent_t *)hHandle;

	HINT32	objId = ONDK_GWM_SimpleKeyboard_GetCurFocus(SIMPLE_KEY_OBJ);
	ONDK_GWM_GuiObject_t cursor_obj = NULL;

	static HINT32	numericKeyValue = -1 ;
	static HUINT32 s_Simple_LimitedInterval = 0;

	ONDK_Assert(pSimpleKeyContexts);

	cursor_obj = ONDK_GWM_Obj_GetObjectByID(SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID);

	if(s_Simple_CursorInterval + SIMPLE_KEYBOARD_CURSOR_INTERVAL <  VK_TIMER_GetSystemTick())
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

	if(s_Simple_LimitedInterval + SIMPLE_KEYBOARD_LIMITED_INTERVAL > VK_TIMER_GetSystemTick() || s_Simple_StartInterval+500 > VK_TIMER_GetSystemTick())
	{
		return ONDK_MESSAGE_CONSUMED;
	}

	switch (nKeyId)
	{
		case KEY_ARROWLEFT:

			if(objId < SIMPLE_KEY_CHAR_ID+SIMPLE_KEYBOARD_SHOW_CHAR_COUNT+1 && objId > SIMPLE_KEY_CHAR_ID-1)
			{
				if(objId == SIMPLE_KEY_CHAR_ID)
				{
					if(pSimpleKeyContexts->firstVisibleOffset > 0)
					{
						pSimpleKeyContexts->firstVisibleOffset--;
						local_simplekeyboard_SetCharSet(pSimpleKeyContexts);
						ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					}
					else if(pSimpleKeyContexts->firstVisibleOffset == 0)
					{
						for(i=0;i<SIMPLE_KEYBOARD_CHAR_COUNT-pSimpleKeyContexts->firstVisibleOffset-1;i++)
						{
							if(HxSTD_StrCmp(s_SimpleCharSet_Using[pSimpleKeyContexts->selectedCharSet][pSimpleKeyContexts->firstVisibleOffset+i+1]," ") == 0)
							{
								if(i > SIMPLE_KEYBOARD_SHOW_CHAR_COUNT)
								{
									pSimpleKeyContexts->firstVisibleOffset = i-SIMPLE_KEYBOARD_SHOW_CHAR_COUNT;
									local_simplekeyboard_SetCharSet(pSimpleKeyContexts);
									ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,objId+SIMPLE_KEYBOARD_SHOW_CHAR_COUNT);
								}
								else
								{
									ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,objId+i);
								}
								ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
								break;
							}
						}
					}
				}
				else
				{
					ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,objId-1);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				}
			}
			else
			{
				if(objId > 1)
				{
					ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,objId-1);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				}
			}
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWRIGHT:

			if(objId < SIMPLE_KEY_CHAR_ID+SIMPLE_KEYBOARD_SHOW_CHAR_COUNT+1&& objId > SIMPLE_KEY_CHAR_ID-1)
			{
				if(HxSTD_StrCmp(s_SimpleCharSet_Using[pSimpleKeyContexts->selectedCharSet][pSimpleKeyContexts->firstVisibleOffset+objId+1-SIMPLE_KEY_CHAR_ID]," ") != 0)
				{
					if(ONDK_GWM_SimpleKeyboard_GetCurFocus(SIMPLE_KEY_OBJ) == SIMPLE_KEY_CHAR_ID + SIMPLE_KEYBOARD_SHOW_CHAR_COUNT)
					{
						if(HxSTD_StrCmp(s_SimpleCharSet_Using[pSimpleKeyContexts->selectedCharSet][pSimpleKeyContexts->firstVisibleOffset+SIMPLE_KEYBOARD_SHOW_CHAR_COUNT+1]," ") != 0)
						{
							pSimpleKeyContexts->firstVisibleOffset++;
							local_simplekeyboard_SetCharSet(pSimpleKeyContexts);
							ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
						}
					}
					else
					{
						ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,objId+1);
						ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					}
				}
				else
				{
					pSimpleKeyContexts->firstVisibleOffset=0;
					local_simplekeyboard_SetCharSet(pSimpleKeyContexts);
					ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,SIMPLE_KEY_CHAR_ID);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				}
			}
			else
			{
#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				if(objId < SIMPLE_KEY_LANGUAGE_ID )
#else
				if(objId < SIMPLE_KEY_TOGGLE_ID )
#endif
				{
					ONDK_GWM_SimpleKeyboard_SetCurFocus(SIMPLE_KEY_OBJ,objId+1);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				}
			}
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWDOWN:

			if(objId < SIMPLE_KEY_CHAR_ID+SIMPLE_KEYBOARD_SHOW_CHAR_COUNT+1 && objId > SIMPLE_KEY_CHAR_ID-1)
			{
				//if (HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf)) > 0)
				{
					HAPI_BroadcastSignal("signal:onOSKResult", 0, "s", HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
					HAPI_BroadcastSignal("signal:onOSKDestroy", 0, "i", 0);
					ONDK_GWM_APP_Destroy(0);
#if 1	// to satisfy a navigation scenario (should change focus directly by using KEY_DOWN)
					return GWM_MESSAGE_PASS;
#endif				
				}
			}
			else
			{
#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				local_simplekeyboard_KeyArrowDownWithLanguageBtn(pSimpleKeyContexts, objId);
#else
				local_simplekeyboard_KeyArrowDown(pSimpleKeyContexts, objId);
#endif
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			}
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWUP:

#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
			local_simplekeyboard_KeyArrowUpWithLanguageBtn(pSimpleKeyContexts, objId);
#else
			if (local_simplekeyboard_KeyArrowUp(pSimpleKeyContexts, objId) == ONDK_MESSAGE_CONSUMED)
			{
				// update currentFocus when this KEY is really consumed!
				pSimpleKeyContexts->currentFocus = objId;
			}
#endif
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

			return ONDK_MESSAGE_CONSUMED;

		case KEY_BACK:
			if(pSimpleKeyContexts->currentPtr== 0)
			{
				HAPI_BroadcastSignal("signal:onOSKResult", 0, "s", HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
				//ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				HAPI_BroadcastSignal("signal:onOSKDestroy", 0, "s", 0);
				ONDK_GWM_APP_Destroy(0);
				return GWM_MESSAGE_PASS;				
			}
			len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr-1);
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
			local_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
			HAPI_BroadcastSignal("signal:onSimpleKeyboardStr", 0, "s", HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
			return ONDK_MESSAGE_CONSUMED;

		case KEY_EXIT:
		case KEY_STANDBY:
		case KEY_CUSTOM_SMART_SEARCH:
			HAPI_BroadcastSignal("signal:onOSKResult", 0, "s", HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
			//ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			HAPI_BroadcastSignal("signal:onOSKDestroy", 0, "s", 0);
			ONDK_GWM_APP_Destroy(0);
			return GWM_MESSAGE_PASS;
			
		case KEY_UNI_RETURN:
		{
			switch(objId)
			{
				//back
				case SIMPLE_KEY_BACK_ID:
					
					if(pSimpleKeyContexts->currentPtr== 0)
						break;
					len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr-1);
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
				case SIMPLE_KEY_SPACE_ID:

					len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
					if(local_simplekeyboard_GetStringWidth(pSimpleKeyContexts,pSimpleKeyContexts->strbuf) > SIMPLE_KEYBOARD_MAX_CHAR_LENGTH)
					{
						ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
						return ONDK_MESSAGE_CONSUMED;
					}
					pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf," ",len);
					pSimpleKeyContexts->currentPtr++;

					break;

				//clear
				case SIMPLE_KEY_CLEAR_ID:

					len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);

					for(i=0;i<len;i++)
					{
						HLIB_STR_RemoveAtBuffer(pSimpleKeyContexts->strbuf,0);
					}
					HLIB_STR_SetAtBuffer(pSimpleKeyContexts->strbuf,0,0);
					pSimpleKeyContexts->currentPtr=0;
					break;

				//toggle
				case SIMPLE_KEY_TOGGLE_ID:

					if(pSimpleKeyContexts->selectedCharSet == SIMPLE_KEYBOARD_CHARSET_CHAR)
					{
						pSimpleKeyContexts->selectedCharSet =SIMPLE_KEYBOARD_CHARSET_NUM;
						pSimpleKeyContexts->currentFocus = SIMPLE_KEY_CHAR_ID+9;
					}
					else
					{
						pSimpleKeyContexts->selectedCharSet =SIMPLE_KEYBOARD_CHARSET_CHAR;
						pSimpleKeyContexts->currentFocus = SIMPLE_KEY_CHAR_ID+SIMPLE_KEYBOARD_SHOW_CHAR_COUNT;
					}

					pSimpleKeyContexts->firstVisibleOffset=0;
					local_simplekeyboard_SetCharSet(pSimpleKeyContexts);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

					return ONDK_MESSAGE_CONSUMED;


				case SIMPLE_KEY_INPUTBOX_ID:
					HAPI_BroadcastSignal("signal:onOSKResult", 0, "s", HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
					HAPI_BroadcastSignal("signal:onOSKDestroy", 0, "i", 0);
					ONDK_GWM_APP_Destroy(0);
#if 1	// to satisfy a navigation scenario (should change focus directly by using KEY_DOWN)
					return GWM_MESSAGE_PASS;
#else
					break;
#endif

#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
				case SIMPLE_KEY_LANGUAGE_ID:
					local_simplekeyboard_ChangeLanguage(pSimpleKeyContexts);
					local_simplekeyboard_SettingUsingTable(pSimpleKeyContexts);
					local_simplekeyboard_SetCharSet(pSimpleKeyContexts);
					local_simplekeyboard_SetCurLanguage(pSimpleKeyContexts);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					break;
#endif

				//char button
				default:
					if(local_simplekeyboard_GetStringWidth(pSimpleKeyContexts,pSimpleKeyContexts->strbuf) > SIMPLE_KEYBOARD_MAX_CHAR_LENGTH)
						return ONDK_MESSAGE_CONSUMED;
//					if(HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf)) > SIMPLE_KEYBOARD_MAX_CHAR_LENGTH-1)
	//					return ONDK_MESSAGE_CONSUMED;

					len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
					pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf,s_SimpleCharSet_Using[pSimpleKeyContexts->selectedCharSet][pSimpleKeyContexts->firstVisibleOffset+objId-SIMPLE_KEY_CHAR_ID],len);
					pSimpleKeyContexts->currentPtr++;

			}

			local_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
			HAPI_BroadcastSignal("signal:onSimpleKeyboardStr", 0, "s", HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
			//ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

		}
		return ONDK_MESSAGE_CONSUMED;

		case KEY_MUTE:
		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
			return GWM_MESSAGE_PASS;

		case KEY_UNI_SPACE:
			len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
			pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf," ",len);
			pSimpleKeyContexts->currentPtr++;
			local_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
			HAPI_BroadcastSignal("signal:onSimpleKeyboardStr", 0, "s", HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
			return ONDK_MESSAGE_CONSUMED;

		case KEY_UNI_BACKSPACE:

			if(pSimpleKeyContexts->currentPtr== 0)
				break;
			len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr-1);
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
			local_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
			HAPI_BroadcastSignal("signal:onSimpleKeyboardStr", 0, "s", HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
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


				if(s_bIsNewWord == FALSE && s_nNumericKeyValue == numericKeyValue)
				{
					HxLOG_Print("[%s:%d](-)change string[%d]\n",__FUNCTION__,__LINE__,pSimpleKeyContexts->currentPtr);
					//change string
					//delete

					if(pSimpleKeyContexts->currentPtr!= 0)
					{
						len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr-1);
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
					{
						pSimpleKeyContexts->smsInput = 0;
					}
					len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
					pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf,s_SimpleNumericSet_Using[1][numericKeyValue][pSimpleKeyContexts->smsInput++],len);
					pSimpleKeyContexts->currentPtr++;
					HxLOG_Print("[%s:%d](+)change string[%d]\n",__FUNCTION__,__LINE__,pSimpleKeyContexts->currentPtr);
				}
				else
				{
					//write
					len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
					if(numericKeyValue == 1 && pSimpleKeyContexts->smsInput == 0)
					{
						pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf," ",len);
						pSimpleKeyContexts->smsInput++;
					}
					else pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf,s_SimpleNumericSet_Using[1][numericKeyValue][0],len);

					pSimpleKeyContexts->smsInput++;
					pSimpleKeyContexts->currentPtr++;
					HxLOG_Print("[%s:%d]write [%d] \n",__FUNCTION__,__LINE__,pSimpleKeyContexts->currentPtr);
				}
				s_bIsNewWord = FALSE;
				s_nNumericKeyValue = numericKeyValue;
				ONDK_GWM_KillTimer(GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL);
				ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL, SIMPLE_KEYBOARD_NUMERIC_INTERVAL);

/*
				if(pSimpleKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
				{
					pSimpleKeyContexts->pwOnOff = 1;
					ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW,KEYBOARD_PW_INTERVAL);
				}
*/


				local_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
				HAPI_BroadcastSignal("signal:onSimpleKeyboardStr", 0, "s", HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				return ONDK_MESSAGE_CONSUMED;
			}

			if(nKeyId >0x0020 && nKeyId < 0x007F)//unicode 값 직접 사용
			{
				inputStr[0] = nKeyId;
				inputStr[1] = '\0';

				len = local_simplekeyboard_GetByteSize(pSimpleKeyContexts->strbuf,0,pSimpleKeyContexts->currentPtr);
				pSimpleKeyContexts->strbuf = HLIB_STR_Insert(pSimpleKeyContexts->strbuf,inputStr,len);

				pSimpleKeyContexts->currentPtr++;

				if(local_simplekeyboard_GetStringWidth(pSimpleKeyContexts,pSimpleKeyContexts->strbuf) > SIMPLE_KEYBOARD_MAX_CHAR_LENGTH)
				{
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					return ONDK_MESSAGE_CONSUMED;
				}

				local_simplekeyboard_SetInputBoxString(pSimpleKeyContexts);
				HAPI_BroadcastSignal("signal:onSimpleKeyboardStr", 0, "s", HLIB_STR_GetBuffer(pSimpleKeyContexts->strbuf));
			}

			return ONDK_MESSAGE_CONSUMED;

	}

	return GWM_RET_OK;
}

//MSG_GWM_TIMER
static ONDK_Result_t local_simplekeyboard_MsgGwmTimer(Simple_Key_Contents_t *pSimpleKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulTimerID = p1;
	ONDK_GWM_GuiObject_t obj;

	if(ulTimerID == GWMPOPUP_TIMER_SIMPLEKEYBOARD)
	{
		obj = ONDK_GWM_Obj_GetObjectByID(SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID);

		if(ONDK_GWM_Obj_GetVisibleByObj(obj) == TRUE)
		{
			ONDK_GWM_Obj_SetVisibleByObj(obj,FALSE);
			//ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			ONDK_GWM_APP_InvalidateObject(SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID);
		}
		else
		{
			ONDK_GWM_Obj_SetVisibleByObj(obj,TRUE);
			//ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			ONDK_GWM_APP_InvalidateObject(SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID);
		}
	}
	else if(ulTimerID == GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL)
	{
		HxLOG_Debug("GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL \n");
		s_bIsNewWord = TRUE;
		pSimpleKeyContexts->smsInput =0;

		ONDK_GWM_KillTimer(GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL);
	}

	ONDK_Assert(pSimpleKeyContexts);

	return GWM_RET_OK;
}

static ONDK_Result_t 	local_simplekeyboard_SetInputBoxString(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;
	ONDK_Rect_t			rect;
	static HxSTR_t		*s_pstString = NULL;
	HUINT32 			str_width = 0;

	if(s_pstString != NULL)
		HLIB_STR_Delete(s_pstString);
	s_pstString = HLIB_STR_Clone(pSimpleKeyContexts->strbuf);

	if(s_pstString != NULL) {
		str_width = local_simplekeyboard_GetStringWidth(pSimpleKeyContexts,s_pstString);
	} else str_width = 0;

	// arabic
	if (ARABIC_SUPPORT_CheckUnicodeArabicCharacterInString(HLIB_STR_toUCS2(s_pstString)))
	{
		HxSTR_t *pTempStr = NULL;
		pTempStr = ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(s_pstString);

		if (pTempStr == NULL)
		{
			HxLOG_Debug("[%s,%d] Failed Convert To VisualAra!!\n",__FUNCTION__,__LINE__);
		}
		else
		{
			s_pstString = pTempStr;
			str_width = local_simplekeyboard_GetStringWidth(pSimpleKeyContexts,s_pstString);
			ONDK_GWM_COM_Text_SetAlign(SIMPLE_KEY_INPUTBOX_TEXT_ID, TEXT_ALIGN_RIGHT);
			// calc cursor position
			rect.x = AP_SIMPLE_KEYBOARD_INPUTBOX_W + AP_SIMPLE_KEYBOARD_INPUTBOX_X + 57 - str_width;
		}
	}
	else	// NOT ARABIC
	{
		ONDK_GWM_COM_Text_SetAlign(SIMPLE_KEY_INPUTBOX_TEXT_ID, TEXT_ALIGN_LEFT);
		// calc cursor position
		rect.x = AP_SIMPLE_KEYBOARD_INPUTBOX_X+64 + str_width;
	}

	ONDK_GWM_COM_Text_SetText(SIMPLE_KEY_INPUTBOX_TEXT_ID,HLIB_STR_GetBuffer(s_pstString));

	rect.y = AP_SIMPLE_KEYBOARD_INPUTBOX_Y+17;
	rect.w = AP_SIMPLE_KEYBOARD_INPUTBOXCUSOR_W;
	rect.h = AP_SIMPLE_KEYBOARD_INPUTBOXCUSOR_H;
	ONDK_GWM_Obj_Resize(SIMPLE_KEY_IMG_INPUTBOX_CURSOR_ID, &rect);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return lRet;
}


static HINT32 local_simplekeyboard_GetByteSize(HxSTR_t *str, HUINT32 start, HUINT32 cnt)
{
	HINT32		lRet = 0;
	HINT32 		i;

	for(i=start;i<cnt;i++)
	{
		lRet += HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(str,i));
	}

	return lRet;
}

static HINT32		local_simplekeyboard_GetStringWidth(Simple_Key_Contents_t *pSimpleKeyContexts,HxSTR_t *str)
{
	HINT32		i=0;
	HINT32		str_width=0;
	HINT32		ptr_len=0;
	HINT32		len=0;
	ONDKFontHandle		fontTempHandle;

	fontTempHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, FONT_NORMAL_SIZE);

	ptr_len = local_simplekeyboard_GetByteSize(str,0,pSimpleKeyContexts->currentPtr);
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

static void 	local_simplekeyboard_SetCharSet(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	HINT32 		i = 0;

	for(i=0;i<SIMPLE_KEYBOARD_SHOW_CHAR_COUNT+1;i++)
	{
		ONDK_GWM_SimpleKeyboard_SetCharValue(SIMPLE_KEY_OBJ,i,s_SimpleCharSet_Using[pSimpleKeyContexts->selectedCharSet][i+pSimpleKeyContexts->firstVisibleOffset]);
	}
}
#if defined(SIMPLE_KEY_SUPPORT_LANGUAGE_BTN)
static void 	local_simplekeyboard_SetCurLanguage(Simple_Key_Contents_t *pSimpleKeyContexts)
{
	ONDK_GWM_SimpleKeyboard_SetCurLanguage(SIMPLE_KEY_OBJ, pSimpleKeyContexts->szLanguageBtn);
}
#endif

#endif
/*********************** End of File ******************************/

