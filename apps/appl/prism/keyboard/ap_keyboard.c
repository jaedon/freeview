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

#include "ap_keyboard.h"
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
} Key_Charset_e;

typedef struct _Key_Contents
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

	Key_Charset_e	language;

	HINT8			strLang[KEYBOARD_BUFFSIZE];
	HINT8			infoLang[KEYBOARD_BUFFSIZE];

#if defined(KEY_SUPPORT_LANGUAGE_BTN)
	HUINT8			szLanguageBtn[KEYBOARD_BUFFSIZE];
#endif

//	HCHAR			*inputboxStr;
	HxSTR_t			*strbuf;

	HINT8				*input1;
	HINT8				*input2;
	HINT8				*input3;

} Key_Contents_t;

HCHAR	s_NumSet[10] =
{

	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'

};

HCHAR *s_CharSet_Using[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT];
HCHAR *s_NumSet_Using[KEYBOARD_NUM_BTN_COUNT];

HCHAR *s_NumericSet_Using[2][KEYBOARD_NUM_BTN_COUNT][KEYBOARD_NUMERIC_COUNT];

HUINT16 s_NumericSet[2][KEYBOARD_NUM_BTN_COUNT][KEYBOARD_NUMERIC_COUNT] =
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

HUINT16 s_CharSet_Eng[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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

HUINT16 s_CharSet_Ger[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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

HUINT16 s_CharSet_Tur[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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

HUINT16 s_CharSet_Rus[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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

HUINT16 s_CharSet_Ara[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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

HUINT16 s_CharSet_Per[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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

HUINT16 s_CharSet_Fre[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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

HUINT16 s_CharSet_Ita[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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

HUINT16 s_CharSet_Spa[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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

HUINT16 s_CharSet_Gre[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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

HUINT16 s_CharSet_Swe[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT] =
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



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static Key_Contents_t		s_Keyboard_Contents;
static HUINT32 				s_cursorInterval = 0;
static HUINT32 				s_startInterval = 0;
static HxLANG_Id_e			s_curKeyboardLang = eLangID_NONE;
static HBOOL				s_bIsNewWord = FALSE;
static HUINT32				s_nNumericKeyValue =0;
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define _________PROTO_TYPE__________

static Key_Contents_t*	 local_keyboard_KeyboardGetContents(void);

static ONDK_Result_t	local_keyboard_MsgGwmCreate(Key_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_keyboard_InitData(Key_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	local_keyboard_RemoteAppMsg(Key_Contents_t *pKeyContexts, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_keyboard_SetLanguage(Key_Contents_t *pKeyContexts);

static ONDK_Result_t 	local_keyboard_SetInputObject(Key_Contents_t *pKeyContexts);

static ONDK_Result_t 	local_keyboard_MsgGwmKeyDown(Key_Contents_t *pKeyContexts, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_keyboard_MsgGwmTimer(Key_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_keyboard_SetInputBoxString(Key_Contents_t *pKeyContexts);

static ONDK_Result_t 	local_keyboard_SettingUsingTable(Key_Contents_t *pKeyContexts);

static HINT32 			local_keyboard_GetByteSize(HxSTR_t *str, HINT32 start, HINT32 cnt);

static void 			local_keyboard_SetCharSet(Key_Contents_t *pSimpleKeyContexts);
static void 			local_keyboard_SetNumSet(Key_Contents_t *pKeyContexts);

static void				local_keyboard_ChangeLanguage(Key_Contents_t *pKeyContexts);
static void				local_keyboard_SetCharButtonCount(Key_Contents_t *pKeyContexts);


#if defined(CONFIG_OP_ERTELECOM)
static void				local_keyboard_ChangeLanguageForErtelcom(Key_Contents_t *pKeyContexts);
#else
static void				local_keyboard_ChangeLanguageForDefault(Key_Contents_t *pKeyContexts);
#endif


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
ONDK_Result_t		AP_KeyboardMain_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	ONDK_Result_t			lRet = GWM_RET_OK;
	static ONDK_Result_t s_upkey =ONDK_RET_OK;
	Key_Contents_t	*pKeyContexts = local_keyboard_KeyboardGetContents();

	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(0x%x) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			lRet = local_keyboard_MsgGwmCreate(pKeyContexts, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			lRet = local_keyboard_MsgGwmTimer(pKeyContexts, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			lRet = local_keyboard_MsgGwmKeyDown(pKeyContexts, hHandle, p1, p2, p3);
			s_upkey = lRet;
			break;
		case MSG_GWM_KEYUP:
			lRet = s_upkey;
			break;

		case MSG_SIGNAL_REMOTESTING:
			lRet = local_keyboard_RemoteAppMsg(pKeyContexts, hHandle, p1, p2, p3);
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



#define _________LOCAL_FUNC__________

static Key_Contents_t*	 local_keyboard_KeyboardGetContents(void)
{
	return &s_Keyboard_Contents;
}


static ONDK_Result_t local_keyboard_MsgGwmCreate(Key_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		hRet = ONDK_RET_OK;
	s_startInterval = VK_TIMER_GetSystemTick();

	hRet = local_keyboard_InitData(pKeyContexts, p1,p2,p3);
	hRet = local_keyboard_SetLanguage(pKeyContexts);

	hRet = local_keyboard_SettingUsingTable(pKeyContexts);
	local_keyboard_SetCharButtonCount(pKeyContexts);

	hRet = local_keyboard_SetInputObject(pKeyContexts);

	if(pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
		pKeyContexts->pwOnOff = 0;

	hRet = local_keyboard_SetInputBoxString(pKeyContexts);

	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD, KEYBOARD_CURSOR_INTERVAL);

	//if(pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
	//	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW, 0);

	//local_keyboard_SetInputBoxString(pKeyContexts);

//	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
/*	ONDK_GWM_APP_MoveAppArea(0,300,1279,719);
	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_MOVE, 20);
*/
	return hRet;
}

static ONDK_Result_t 	local_keyboard_InitData(Key_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t lRet = ONDK_RET_OK;
	HUINT8			*str = NULL;
	HINT32	i,j,k;

//	pKeyContexts->inputboxStr = AP_Malloc(sizeof(HCHAR)*KEYBOARD_STRING_LENGTH);
//	HxSTD_MemSet(pKeyContexts->inputboxStr, 0, sizeof(pKeyContexts->inputboxStr));

	pKeyContexts->modeToggle = 0;
	pKeyContexts->modeUpLow = 1;

	pKeyContexts->currentFocus_Row= 2;
	pKeyContexts->currentFocus_Col= 0;

	pKeyContexts->currentPtr = 0;
	pKeyContexts->charCount = 0;

	pKeyContexts->inputboxSize = 1;
	pKeyContexts->inputMode = p2;
	pKeyContexts->pwOnOff = 1;
//	pKeyContexts->speed = 0;

#if defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_FVP4000T)
	pKeyContexts->isEnglishArea = TRUE;
#else
	pKeyContexts->isEnglishArea = FALSE;
#endif
/*
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_OP_MIDDLE_EAST)
	pKeyContexts->isEnglishArea = FALSE;
#else
	pKeyContexts->isEnglishArea = TRUE;
#endif
*/



	for(i=0;i<2;i++)
	{
		for(j=0;j<KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT;j++)
		{
			s_CharSet_Using[i][j] = AP_Malloc(sizeof(HUINT8)*8);
			HxSTD_MemSet(s_CharSet_Using[i][j], 0, sizeof(HUINT8)*8);
		}
	}

	//num array setting

	for(i=0;i<KEYBOARD_NUM_BTN_COUNT;i++)
	{
		s_NumSet_Using[i] = AP_Malloc(sizeof(HUINT8)*8);
		HxSTD_MemSet(s_NumSet_Using[i], 0, sizeof(HUINT8)*8);
	}

	//numeric array setting
	for(k=0;k<2;k++)
	{
		for(i=0;i<KEYBOARD_NUM_BTN_COUNT;i++)
		{
			for(j=0;j<KEYBOARD_NUMERIC_COUNT;j++)
			{
				s_NumericSet_Using[k][i][j] = AP_Malloc(sizeof(HUINT8)*8);
				HxSTD_MemSet(s_NumericSet_Using[k][i][j], 0, sizeof(HUINT8)*8);
			}
		}
	}

	if((void *)p3 == NULL)
	{
		pKeyContexts->strbuf = HLIB_STR_New("");
	}
	else
	{
		pKeyContexts->strbuf = HLIB_STR_New((HINT8 *)p3);
		pKeyContexts->currentPtr = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pKeyContexts->strbuf));
	}


	pKeyContexts->input1 = AP_Malloc(sizeof(HINT8)*KEYBOARD_STRING_LENGTH);
	HxSTD_MemSet(pKeyContexts->input1, 0, sizeof(HINT8)*KEYBOARD_STRING_LENGTH);

	pKeyContexts->input2 = AP_Malloc(sizeof(HINT8)*KEYBOARD_STRING_LENGTH);
	HxSTD_MemSet(pKeyContexts->input2, 0, sizeof(HINT8)*KEYBOARD_STRING_LENGTH);

	pKeyContexts->input3 = AP_Malloc(sizeof(HINT8)*KEYBOARD_STRING_LENGTH);
	HxSTD_MemSet(pKeyContexts->input3, 0, sizeof(HINT8)*KEYBOARD_STRING_LENGTH);

#if defined(KEY_SUPPORT_LANGUAGE_BTN)
	str = ONDK_GetString("STR_LANGUAGE_ID");
	if(str == NULL)
	{
		HxSTD_snprintf(pKeyContexts->szLanguageBtn, sizeof(pKeyContexts->szLanguageBtn)-1, "%s", (HUINT8*)"LANGUAGE");
	}
	else
	{
		HxSTD_snprintf(pKeyContexts->szLanguageBtn, sizeof(pKeyContexts->szLanguageBtn)-1, "%s", str);
	}
#endif

	return lRet;
}

static ONDK_Result_t local_keyboard_RemoteAppMsg(Key_Contents_t *pKeyContexts, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT8					*str_val = (HINT8 *)p1;

	if(pKeyContexts->strbuf != NULL)
		HLIB_STR_Delete(pKeyContexts->strbuf);

	pKeyContexts->strbuf = HLIB_STR_New(str_val);
	pKeyContexts->currentPtr = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pKeyContexts->strbuf));

	(void)local_keyboard_SetInputBoxString(pKeyContexts);

	return GWM_MESSAGE_BREAK;
}

static Key_Charset_e	local_keyboard_convertLanguageStringToType(HCHAR *pLangFullName)
{
	if(HxSTD_StrCmp(pLangFullName,"English")==0)		return eCHARSET_English;
	if(HxSTD_StrCmp(pLangFullName,"Scottish Gaelic")==0)		return eCHARSET_English;
	if(HxSTD_StrCmp(pLangFullName,"Irish")==0)		return eCHARSET_English;
	if(HxSTD_StrCmp(pLangFullName,"Welsh")==0)		return eCHARSET_English;
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

static ONDK_Result_t	local_keyboard_SetLanguage(Key_Contents_t *pKeyContexts)
{
	HERROR 	hErr = ERR_OK;
	HCHAR	*pLangFullName = NULL;
	HCHAR	szLang639[8] ={0,};
	HxLANG_Id_e langId;

#if defined(KEY_SUPPORT_LANGUAGE_BTN)
	hErr = APK_SCENARIO_GetMainLanguage(szLang639);
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
	s_curKeyboardLang  = langId;

	HxSTD_StrNCpy(pKeyContexts->strLang, pLangFullName, ONDK_LANGUAGE_STRING_MAX_LENGTH);
	pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';

	pKeyContexts->language = local_keyboard_convertLanguageStringToType(pLangFullName);

//	ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ONDK_RET_OK;
}

static ONDK_Result_t 	local_keyboard_SetInputObject(Key_Contents_t *pKeyContexts)
{
	ONDK_Rect_t 		stRect;
	ONDK_Result_t		lRet = ONDK_RET_OK;

	ONDK_GWM_APP_SetAppArea(AP_KEYBOARD_FRAME_X, AP_KEYBOARD_FRAME_Y, AP_KEYBOARD_FRAME_W, AP_KEYBOARD_FRAME_H);

	stRect = ONDK_Rect(AP_KEYBOARD_FRAME_X,AP_KEYBOARD_FRAME_Y,AP_KEYBOARD_FRAME_W,AP_KEYBOARD_FRAME_H);
#if defined(KEY_SUPPORT_LANGUAGE_BTN)
	ONDK_GWM_Keyboard_Create(KEY_OBJ,&stRect,0,pKeyContexts->isEnglishArea, pKeyContexts->szLanguageBtn);
#else
	ONDK_GWM_Keyboard_Create(KEY_OBJ,&stRect,0,pKeyContexts->isEnglishArea, NULL);
#endif

	local_keyboard_SetCharSet(pKeyContexts);
	local_keyboard_SetNumSet(pKeyContexts);

	//inputbox1 text
	stRect = ONDK_Rect(AP_KEYBOARD_INPUTBOX1_X+20,AP_KEYBOARD_INPUTBOX1_Y+12,AP_KEYBOARD_MAX_INPUT_WIDTH,AP_KEYBOARD_INPUTBOX_H);
	ONDK_GWM_COM_Text_Create(KEY_INPUTBOX1_TEXT_ID,&stRect,"");
	ONDK_GWM_COM_Text_SetSize(KEY_INPUTBOX1_TEXT_ID,FONT_NORMAL_SIZE);
	ONDK_GWM_COM_Text_ChangeColor(KEY_INPUTBOX1_TEXT_ID,FONT_COLOR,FONT_COLOR);
	ONDK_GWM_COM_Text_SetFont(KEY_INPUTBOX1_TEXT_ID,eFont_SystemBold);

	//inputbox2 text
	stRect = ONDK_Rect(AP_KEYBOARD_INPUTBOX2_X+20,AP_KEYBOARD_INPUTBOX2_Y+7,AP_KEYBOARD_MAX_INPUT_WIDTH,AP_KEYBOARD_INPUTBOX_H);
	ONDK_GWM_COM_Text_Create(KEY_INPUTBOX2_TEXT_ID,&stRect,"");
	ONDK_GWM_COM_Text_SetSize(KEY_INPUTBOX2_TEXT_ID,FONT_NORMAL_SIZE);
	ONDK_GWM_COM_Text_ChangeColor(KEY_INPUTBOX2_TEXT_ID,FONT_COLOR,FONT_COLOR);
	ONDK_GWM_COM_Text_SetFont(KEY_INPUTBOX2_TEXT_ID,eFont_SystemBold);

	//inputbox3 text
	stRect = ONDK_Rect(AP_KEYBOARD_INPUTBOX3_X+18,AP_KEYBOARD_INPUTBOX3_Y+2,AP_KEYBOARD_MAX_INPUT_WIDTH,AP_KEYBOARD_INPUTBOX_H);
	ONDK_GWM_COM_Text_Create(KEY_INPUTBOX3_TEXT_ID,&stRect,"");
	ONDK_GWM_COM_Text_SetSize(KEY_INPUTBOX3_TEXT_ID,FONT_NORMAL_SIZE);
	ONDK_GWM_COM_Text_ChangeColor(KEY_INPUTBOX3_TEXT_ID,FONT_COLOR,FONT_COLOR);
	ONDK_GWM_COM_Text_SetFont(KEY_INPUTBOX3_TEXT_ID,eFont_SystemBold);

	//inputbox cursor
	stRect = ONDK_Rect(AP_KEYBOARD_INPUTBOX1_X+15,AP_KEYBOARD_INPUTBOX1_Y+12,AP_KEYBOARD_INPUTBOXCUSOR_W,AP_KEYBOARD_INPUTBOXCUSOR_H);
	ONDK_GWM_COM_Image_Create(KEY_IMG_INPUTBOX_CURSOR_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(KEY_IMG_INPUTBOX_CURSOR_ID, KEYBOARD_INPUTBOX_CURSOR);

	ONDK_GWM_Obj_SetHide((KEY_INPUTBOX1_TEXT_ID),FALSE);
	ONDK_GWM_Obj_SetHide((KEY_INPUTBOX2_TEXT_ID),FALSE);
	ONDK_GWM_Obj_SetHide((KEY_INPUTBOX3_TEXT_ID),FALSE);

	return lRet;

}

static ONDK_Result_t local_keyboard_MsgGwmKeyDown(Key_Contents_t *pKeyContexts, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 	nKeyId = p1;
	HINT32	len;
	HINT32	ptr_len;
	HINT32	total_len;
	HINT32	i;

	HInputEvent_t	*inputEvent = (HInputEvent_t *)hHandle;

	HINT32	currentRow;
	HINT32	currentCol;

	HUINT8 inputStr[2] = {0,};
	HBOOL	bUpperCase = FALSE;

	static HINT32	numericKeyValue = -1 ;
	static HUINT32 s_limitedInterval = 0;
	ONDK_GWM_GuiObject_t cursor_obj = NULL;

	currentRow = ONDK_GWM_Keyboard_GetCurFocus_Row(KEY_OBJ);
	currentCol = ONDK_GWM_Keyboard_GetCurFocus_Col(KEY_OBJ);
	cursor_obj = ONDK_GWM_Obj_GetObjectByID(KEY_IMG_INPUTBOX_CURSOR_ID);

/*
	if(pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
	{
		pKeyContexts->pwOnOff = 0;
		local_keyboard_SetInputBoxString(pKeyContexts);
		ONDK_GWM_KillTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
	}
*/
	if(s_cursorInterval + KEYBOARD_CURSOR_INTERVAL <  VK_TIMER_GetSystemTick())
	{
         s_cursorInterval = VK_TIMER_GetSystemTick();
         if(ONDK_GWM_Obj_GetVisibleByObj(cursor_obj) == TRUE)
         {
				ONDK_GWM_Obj_SetVisibleByObj(cursor_obj,FALSE);
         }
         else
         {
				ONDK_GWM_Obj_SetVisibleByObj(cursor_obj,TRUE);
         }
	}

	if(s_limitedInterval + KEYBOARD_LIMITED_INTERVAL >  VK_TIMER_GetSystemTick() || s_startInterval+500 > VK_TIMER_GetSystemTick())
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
	switch (nKeyId)
	{
		case KEY_ARROWLEFT:

			for(i=0;i<KEYBOARD_COL_COUNT;i++)
			{
				switch(currentRow)
				{
					case KEYBOARD_ROW_INPUTBOX:
						if(pKeyContexts->currentPtr > 0 )
						{
							pKeyContexts->currentPtr--;
							local_keyboard_SetInputBoxString(pKeyContexts);
						}
						return ONDK_MESSAGE_CONSUMED;
					case KEYBOARD_ROW_0:
						return ONDK_MESSAGE_CONSUMED;
					case KEYBOARD_ROW_1:
						currentCol = currentCol-2;
						if(currentCol < KEYBOARD_COL_0)
							currentCol = KEYBOARD_COL_8;
						break;
					case KEYBOARD_ROW_4:
						currentCol--;
						if(currentCol < KEYBOARD_COL_0)
							currentCol = KEYBOARD_COL_8;
						break;
					case KEYBOARD_ROW_2:
					case KEYBOARD_ROW_3:
					case KEYBOARD_ROW_5:
						currentCol--;
						if(currentCol < KEYBOARD_COL_0)
							currentCol = KEYBOARD_COL_9;
						break;
				}
				if(currentRow == KEYBOARD_ROW_0 || currentRow == KEYBOARD_ROW_1 || currentRow == KEYBOARD_ROW_5)
					break;

				if(HxSTD_StrCmp(s_CharSet_Using[pKeyContexts->modeUpLow][pKeyContexts->modeToggle+(currentRow-2)*10+currentCol]," ")!=0)
					break;
			}

			ONDK_GWM_Keyboard_SetCurFocus_Col(KEY_OBJ, currentCol);
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWRIGHT:
			for(i=0;i<KEYBOARD_COL_COUNT;i++)
			{
				switch(currentRow)
				{
					case KEYBOARD_ROW_INPUTBOX:
						if(pKeyContexts->currentPtr < HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pKeyContexts->strbuf)))
						{
							pKeyContexts->currentPtr++;
							local_keyboard_SetInputBoxString(pKeyContexts);
						}
						return ONDK_MESSAGE_CONSUMED;
					case KEYBOARD_ROW_0:
						return ONDK_MESSAGE_CONSUMED;
					case KEYBOARD_ROW_1:
						currentCol = currentCol+2;
						if(currentCol > KEYBOARD_COL_9)
							currentCol = KEYBOARD_COL_0;
						break;
					case KEYBOARD_ROW_4:
						currentCol++;
						if(currentCol > KEYBOARD_COL_8)
							currentCol = KEYBOARD_COL_0;
						break;
					case KEYBOARD_ROW_2:
					case KEYBOARD_ROW_3:
					case KEYBOARD_ROW_5:
						currentCol++;
						if(currentCol > KEYBOARD_COL_9)
							currentCol = KEYBOARD_COL_0;
						break;
				}
				if(currentRow == KEYBOARD_ROW_0 || currentRow == KEYBOARD_ROW_1 || currentRow == KEYBOARD_ROW_5)
					break;

				if(HxSTD_StrCmp(s_CharSet_Using[pKeyContexts->modeUpLow][pKeyContexts->modeToggle+(currentRow-2)*10+currentCol]," ")!=0)
					break;
			}
			ONDK_GWM_Keyboard_SetCurFocus_Col(KEY_OBJ, currentCol);
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWUP:
			for(i=0;i<KEYBOARD_ROW_COUNT;i++)
			{
				switch(currentRow)
				{
					case KEYBOARD_ROW_INPUTBOX:
						return ONDK_MESSAGE_CONSUMED;
					case KEYBOARD_ROW_0:
							currentRow = KEYBOARD_ROW_INPUTBOX;
						break;
					case KEYBOARD_ROW_1:
						if(currentCol == KEYBOARD_COL_8)
						{
							if(pKeyContexts->isEnglishArea==TRUE)
								currentRow = KEYBOARD_ROW_INPUTBOX;
							else
								currentRow--;
						}
						else
						{
							currentRow = KEYBOARD_ROW_5;

							if(currentCol == pKeyContexts->currentFocus_Col || currentCol+1 == pKeyContexts->currentFocus_Col)
								currentCol = pKeyContexts->currentFocus_Col;
						}
						break;
					case KEYBOARD_ROW_2:
						pKeyContexts->currentFocus_Row = currentRow;
						pKeyContexts->currentFocus_Col = currentCol;
						currentRow--;
						if(currentCol % 2 != 0)
							currentCol--;
						break;

					case KEYBOARD_ROW_3:
						pKeyContexts->currentFocus_Row = currentRow;
						pKeyContexts->currentFocus_Col = currentCol;
						currentRow--;
						break;

					case KEYBOARD_ROW_4:
						if(currentCol > KEYBOARD_COL_7)
						{
							if(pKeyContexts->currentFocus_Col < KEYBOARD_COL_8)
							{
								currentCol= KEYBOARD_COL_8;
								pKeyContexts->currentFocus_Col = KEYBOARD_COL_8;
							}
							else
							{
								currentCol = pKeyContexts->currentFocus_Col;
							}
						}
						currentRow--;
						break;

					case KEYBOARD_ROW_5:
						pKeyContexts->currentFocus_Row = currentRow;
						pKeyContexts->currentFocus_Col = currentCol;
						currentRow--;
						if(currentCol > KEYBOARD_COL_7)
						{
							if(currentCol % 2 != 0)
								currentCol--;
						}
						break;

				}
				if(currentRow == KEYBOARD_ROW_0 || currentRow == KEYBOARD_ROW_1 || currentRow == KEYBOARD_ROW_5 || currentRow == KEYBOARD_ROW_INPUTBOX)
					break;

				if(HxSTD_StrCmp(s_CharSet_Using[pKeyContexts->modeUpLow][pKeyContexts->modeToggle+(currentRow-2)*10+currentCol]," ")!=0)
					break;
				}
			ONDK_GWM_Keyboard_SetCurFocus_Row(KEY_OBJ, currentRow);
			ONDK_GWM_Keyboard_SetCurFocus_Col(KEY_OBJ, currentCol);
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWDOWN:
			for(i=0;i<KEYBOARD_ROW_COUNT;i++)
			{
				switch(currentRow)
				{
					case KEYBOARD_ROW_INPUTBOX:
						if(pKeyContexts->isEnglishArea==FALSE)
							currentRow =KEYBOARD_ROW_0;
						else currentRow =KEYBOARD_ROW_1;
						break;
					case KEYBOARD_ROW_0:
						currentRow++;
						break;
					case KEYBOARD_ROW_1:
						currentRow++;
						if(currentCol == pKeyContexts->currentFocus_Col || currentCol+1 == pKeyContexts->currentFocus_Col)
							currentCol = pKeyContexts->currentFocus_Col;
						break;

					case KEYBOARD_ROW_5:
						if(currentCol < KEYBOARD_COL_8)
						{
							pKeyContexts->currentFocus_Row = currentRow;
							pKeyContexts->currentFocus_Col = currentCol;

							currentRow=KEYBOARD_ROW_1;
							if(currentCol % 2 != 0)
								currentCol--;
							break;
						}
						else
						{
							pKeyContexts->currentFocus_Row = currentRow;
							pKeyContexts->currentFocus_Col = currentCol;
							if(pKeyContexts->isEnglishArea==TRUE)
								currentRow=KEYBOARD_ROW_1;
							else currentRow=KEYBOARD_ROW_0;
							if(currentCol % 2 != 0)
								currentCol--;
							break;
						}
					case 2:
						currentRow++;
						break;

					case 3:
						if(currentCol > KEYBOARD_COL_7)
						{
							pKeyContexts->currentFocus_Col = currentCol;
							if(currentCol % 2 != 0)
							{
								currentCol--;
							}

						}
						currentRow++;
						break;

					case 4:
						if(currentCol > KEYBOARD_COL_7)
						{
							if(pKeyContexts->currentFocus_Col < KEYBOARD_COL_8)
							{
								currentCol = KEYBOARD_COL_8;
								pKeyContexts->currentFocus_Col = KEYBOARD_COL_8;
							}
							else
							{
								currentCol = pKeyContexts->currentFocus_Col;
							}
						}
						currentRow++;
						break;
				}
				if(currentRow == KEYBOARD_ROW_0 || currentRow == KEYBOARD_ROW_1 || currentRow == KEYBOARD_ROW_5)
					break;
				//HxLOG_Warning ( "char : %s  \r\n",s_CharSet_Using[pKeyContexts->modeUpLow][pKeyContexts->modeToggle+(currentRow-2)*10+currentCol]);
				if(HxSTD_StrCmp(s_CharSet_Using[pKeyContexts->modeUpLow][pKeyContexts->modeToggle+(currentRow-2)*10+currentCol]," ")!=0)
				{
					//HxLOG_Warning ( "not space \r\n");
					break;
				}
			}
			ONDK_GWM_Keyboard_SetCurFocus_Row(KEY_OBJ, currentRow);
			ONDK_GWM_Keyboard_SetCurFocus_Col(KEY_OBJ, currentCol);
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			return ONDK_MESSAGE_CONSUMED;

		case KEY_BACK:
		case KEY_EXIT:
			HAPI_BroadcastSignal("signal:onOSKDestroy", 0, "s", 0);
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_CONSUMED;

		case KEY_UNI_RETURN:
		{
			switch(currentRow)
			{
				case KEYBOARD_ROW_INPUTBOX:
					return ONDK_MESSAGE_CONSUMED;
				case KEYBOARD_ROW_0:
					local_keyboard_ChangeLanguage(pKeyContexts);
					local_keyboard_SettingUsingTable(pKeyContexts);
					local_keyboard_SetCharSet(pKeyContexts);
					local_keyboard_SetNumSet(pKeyContexts);
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					return ONDK_MESSAGE_CONSUMED;

				case KEYBOARD_ROW_1:
					switch(currentCol)
					{
						case KEYBOARD_COL_0:

							if(pKeyContexts->modeUpLow == 0)
								pKeyContexts->modeUpLow = 1;
							else pKeyContexts->modeUpLow = 0;

							bUpperCase = (pKeyContexts->modeUpLow == 1) ? FALSE : TRUE;
							ONDK_GWM_Keyboard_SetUppercaseAndLowercaseLetters(KEY_OBJ, bUpperCase);

							local_keyboard_SetCharSet(pKeyContexts);

							ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
							return ONDK_MESSAGE_CONSUMED;

						case KEYBOARD_COL_2:

							pKeyContexts->modeToggle += KEYBOARD_CHAR_BTN_COUNT;
							local_keyboard_SetCharButtonCount(pKeyContexts);
							local_keyboard_SetCharSet(pKeyContexts);

							ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

							return ONDK_MESSAGE_CONSUMED;

						//back
						case KEYBOARD_COL_4:

							if(pKeyContexts->currentPtr== 0)
								break;

							len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr-1);
							ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(pKeyContexts->strbuf,pKeyContexts->currentPtr-1));
							total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pKeyContexts->strbuf));

							if(ptr_len==2)
							{
								HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf,len);
								HLIB_STR_SetAtBuffer(pKeyContexts->strbuf,total_len-2,0);
							}
							else
							{
								HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf,len);
								HLIB_STR_SetAtBuffer(pKeyContexts->strbuf,total_len-1,0);
							}

							pKeyContexts->currentPtr--;
							break;


						//clear
						case KEYBOARD_COL_6:
							len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr);

							for(i=0;i<len;i++)
							{
								HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf,0);
							}
							HLIB_STR_SetAtBuffer(pKeyContexts->strbuf,0,0);
							pKeyContexts->currentPtr=0;

							break;

						case KEYBOARD_COL_8:

							HAPI_BroadcastSignal("signal:onOSKResult", 0, "s", HLIB_STR_GetBuffer(pKeyContexts->strbuf));
//							ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
							HAPI_BroadcastSignal("signal:onOSKDestroy", 0, "i", 0);
							ONDK_GWM_APP_Destroy(0);


							return ONDK_MESSAGE_CONSUMED;
					}
					break;
				//char button
				default:

					//space
					if(currentRow == KEYBOARD_ROW_4 && currentCol == KEYBOARD_COL_8)
					{
						len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr);
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf," ",len);
						pKeyContexts->currentPtr++;

						if(pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
						{
							pKeyContexts->pwOnOff = 1;
							//ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
							ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW,KEYBOARD_PW_INTERVAL);
						}

						break;
					}

					if(HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pKeyContexts->strbuf)) > KEYBOARD_MAX_LINE_LENGTH-2 && 	pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
					{
						ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
						return ONDK_MESSAGE_CONSUMED;
					}
#if 0
					if(HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pKeyContexts->strbuf)) > KEYBOARD_MAX_TOTAL_LENGTH-1)
					{
						ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
						return ONDK_MESSAGE_CONSUMED;
					}
#endif

					len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr);
					if(currentRow == KEYBOARD_ROW_5)
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf,s_NumSet_Using[currentCol],len);
					else pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf,s_CharSet_Using[pKeyContexts->modeUpLow][pKeyContexts->modeToggle+(currentRow-2)*10+currentCol],len);
					pKeyContexts->currentPtr++;

					if(pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
					{
						pKeyContexts->pwOnOff = 1;
						//ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
						ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW,KEYBOARD_PW_INTERVAL);
					}
			}

			if (local_keyboard_SetInputBoxString(pKeyContexts) == ONDK_MESSAGE_CONSUMED)
			{
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				return ONDK_MESSAGE_CONSUMED;
			}
			HAPI_BroadcastSignal("signal:onKeyboardStr", 0, "s", HLIB_STR_GetBuffer(pKeyContexts->strbuf));
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
		}
		return ONDK_MESSAGE_CONSUMED;

		case KEY_MUTE:
		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
		case KEY_STANDBY:
			return GWM_MESSAGE_PASS;

		case KEY_UNI_SPACE:
			len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr);
			pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf," ",len);
			pKeyContexts->currentPtr++;

			if(pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
			{
				pKeyContexts->pwOnOff = 1;
				//ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
				ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW,KEYBOARD_PW_INTERVAL);
			}

			if (local_keyboard_SetInputBoxString(pKeyContexts) == ONDK_MESSAGE_CONSUMED)
			{
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				return ONDK_MESSAGE_CONSUMED;
			}
			HAPI_BroadcastSignal("signal:onKeyboardStr", 0, "s", HLIB_STR_GetBuffer(pKeyContexts->strbuf));
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			return ONDK_MESSAGE_CONSUMED;

		case KEY_UNI_BACKSPACE:
			if(pKeyContexts->currentPtr== 0)
				break;

			len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr-1);
			ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(pKeyContexts->strbuf,pKeyContexts->currentPtr-1));
			total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pKeyContexts->strbuf));

			if(ptr_len==2)
			{
				HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf,len);
				HLIB_STR_SetAtBuffer(pKeyContexts->strbuf,total_len-2,0);
			}
			else
			{
				HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf,len);
				HLIB_STR_SetAtBuffer(pKeyContexts->strbuf,total_len-1,0);
			}

			pKeyContexts->currentPtr--;
			if (local_keyboard_SetInputBoxString(pKeyContexts) == ONDK_MESSAGE_CONSUMED)
			{
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				return ONDK_MESSAGE_CONSUMED;
			}
			HAPI_BroadcastSignal("signal:onKeyboardStr", 0, "s", HLIB_STR_GetBuffer(pKeyContexts->strbuf));
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			return ONDK_MESSAGE_CONSUMED;

		default:
			if(nKeyId >0x0029 && nKeyId < 0x0040 && inputEvent->device_id == eKeyInput_RCU)
			{
				switch(nKeyId)
				{
					case KEY_0:
						if(numericKeyValue != 0)
							pKeyContexts->smsInput = 0;
						numericKeyValue = 0;
						break;
					case KEY_1:
						if(numericKeyValue != 1)
							pKeyContexts->smsInput = 0;
						numericKeyValue = 1;
						break;
					case KEY_2:
						if(numericKeyValue != 2)
							pKeyContexts->smsInput = 0;
						numericKeyValue = 2;
						break;
					case KEY_3:
						if(numericKeyValue != 3)
							pKeyContexts->smsInput = 0;
						numericKeyValue = 3;
						break;
					case KEY_4:
						if(numericKeyValue != 4)
							pKeyContexts->smsInput = 0;
						numericKeyValue = 4;
						break;
					case KEY_5:
						if(numericKeyValue != 5)
							pKeyContexts->smsInput = 0;
						numericKeyValue = 5;
						break;
					case KEY_6:
						if(numericKeyValue != 6)
							pKeyContexts->smsInput = 0;
						numericKeyValue = 6;
						break;
					case KEY_7:
						if(numericKeyValue != 7)
							pKeyContexts->smsInput = 0;
						numericKeyValue = 7;
						break;
					case KEY_8:
						if(numericKeyValue != 8)
							pKeyContexts->smsInput = 0;
						numericKeyValue = 8;
						break;
					case KEY_9:
						if(numericKeyValue != 9)
							pKeyContexts->smsInput = 0;
						numericKeyValue = 9;
						break;
				}

				if(s_bIsNewWord == FALSE && s_nNumericKeyValue == numericKeyValue)
				{
					//change string
					//delete
					if(pKeyContexts->currentPtr!= 0)
					{
						len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr-1);
						ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(pKeyContexts->strbuf,pKeyContexts->currentPtr-1));
						total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pKeyContexts->strbuf));

						if(ptr_len==2)
						{
							HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf,len);
							HLIB_STR_SetAtBuffer(pKeyContexts->strbuf,total_len-2,0);
						}
						else
						{
							HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf,len);
							HLIB_STR_SetAtBuffer(pKeyContexts->strbuf,total_len-1,0);
						}

						pKeyContexts->currentPtr--;
					}
					//write
					len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr);
					if(numericKeyValue == 1 && pKeyContexts->smsInput == 0)
					{
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf," ",len);
						pKeyContexts->smsInput++;
					}
					else
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf,s_NumericSet_Using[pKeyContexts->modeUpLow][numericKeyValue][pKeyContexts->smsInput++],len);

					pKeyContexts->currentPtr++;

					if(numericKeyValue == 7 || numericKeyValue == 9) // 5 char key
					{
						if(pKeyContexts->smsInput>KEYBOARD_NUMERIC_COUNT-1)
							pKeyContexts->smsInput = 0;
					}
					else // 4 char key
					{
						if(pKeyContexts->smsInput>KEYBOARD_NUMERIC_COUNT-2)
							pKeyContexts->smsInput = 0;
					}

				}
				else
				{
					//write
					len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr);
					if(numericKeyValue == 1 && pKeyContexts->smsInput == 0)
					{
						pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf," ",len);
						pKeyContexts->smsInput++;
					}
					else pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf,s_NumericSet_Using[pKeyContexts->modeUpLow][numericKeyValue][0],len);
					pKeyContexts->currentPtr++;
					pKeyContexts->smsInput++;
				}
				s_bIsNewWord = FALSE;
				s_nNumericKeyValue = numericKeyValue;
				ONDK_GWM_KillTimer(GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL);
				ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL, KEYBOARD_NUMERIC_INTERVAL);


				if(pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
				{
					pKeyContexts->pwOnOff = 1;
					ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW,KEYBOARD_PW_INTERVAL);
				}

				local_keyboard_SetInputBoxString(pKeyContexts);
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				return ONDK_MESSAGE_CONSUMED;
			}

			if(nKeyId >0x0020 && nKeyId < 0x007F)//unicode 값 직접 사용
			{
				inputStr[0] = (HUINT8)nKeyId;
				inputStr[1] = '\0';

				len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr);
				pKeyContexts->strbuf = HLIB_STR_Insert(pKeyContexts->strbuf, (const HUINT8*)inputStr,len);

				pKeyContexts->currentPtr++;

				if(HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8,HLIB_STR_GetBuffer(pKeyContexts->strbuf)) > KEYBOARD_MAX_LINE_LENGTH-2 &&	pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
				{
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					return ONDK_MESSAGE_CONSUMED;
				}

				if(pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)
				{
					pKeyContexts->pwOnOff = 1;
					//ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
					ONDK_GWM_SetTimer(GWMPOPUP_TIMER_KEYBOARD_PW,KEYBOARD_PW_INTERVAL);
				}

				if (local_keyboard_SetInputBoxString(pKeyContexts) == ONDK_MESSAGE_CONSUMED)
				{
					ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
					return ONDK_MESSAGE_CONSUMED;
				}
				HAPI_BroadcastSignal("signal:onKeyboardStr", 0, "s", HLIB_STR_GetBuffer(pKeyContexts->strbuf));
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
			}

			return ONDK_MESSAGE_CONSUMED;

	}

	s_limitedInterval = VK_TIMER_GetSystemTick();

	return GWM_RET_OK;
}

//MSG_GWM_TIMER
static ONDK_Result_t local_keyboard_MsgGwmTimer(Key_Contents_t *pKeyContexts, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 ulTimerID = p1;
	ONDK_GWM_GuiObject_t obj;

	if(ulTimerID == GWMPOPUP_TIMER_KEYBOARD)
	{
		obj = ONDK_GWM_Obj_GetObjectByID(KEY_IMG_INPUTBOX_CURSOR_ID);

		if(ONDK_GWM_Obj_GetVisibleByObj(obj) == TRUE)
		{
			ONDK_GWM_Obj_SetVisibleByObj(obj,FALSE);
			ONDK_GWM_APP_InvalidateObject(KEY_IMG_INPUTBOX_CURSOR_ID);
		}
		else
		{
			ONDK_GWM_Obj_SetVisibleByObj(obj,TRUE);
			ONDK_GWM_APP_InvalidateObject(KEY_IMG_INPUTBOX_CURSOR_ID);
		}

		s_cursorInterval = VK_TIMER_GetSystemTick();
	}

	else if(ulTimerID == GWMPOPUP_TIMER_KEYBOARD_PW)
	{
		pKeyContexts->pwOnOff = 0;
		local_keyboard_SetInputBoxString(pKeyContexts);
		ONDK_GWM_KillTimer(GWMPOPUP_TIMER_KEYBOARD_PW);
	}
	else if(ulTimerID == GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL)
	{
		HxLOG_Debug("GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL \n");
		s_bIsNewWord = TRUE;
		pKeyContexts->smsInput =0;

		ONDK_GWM_KillTimer(GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL);
	}

	/*
	else if(ulTimerID == GWMPOPUP_TIMER_KEYBOARD_MOVE)
	{
		static int arr[20] = {10, 20, 0, 30, 0, 40,0, 40, 0, 40, 0, 40, 10, 50, 10, 50, 10, 60, 10};
		ONDK_GWM_APP_GetAppArea(ONDK_GWM_APP_GetThis(),&area);
		ONDK_GWM_APP_MoveAppArea(0,area.y-arr[pKeyContexts->speed],area.w,area.h);
		ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());


//		HxLOG_Print("@@@@ %d (%d)\r\n",pKeyContexts->speed, VK_TIMER_GetSystemTick());
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
	return GWM_RET_OK;
}

static const size_t utf8_skip_data[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

static ONDK_Result_t	local_keyboard_SetInputBoxString(Key_Contents_t *pKeyContexts)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;
	ONDK_Rect_t			rect;

	HINT32 				i=0;
	HINT32 				str_width=0;
	HINT32 				len;
	HINT32 				ptr_len;
	HINT32				str_len;

	//ONDK_GWM_GuiObject_t cursor_obj = NULL;

	HxSTR_t *total_str = NULL;
	HUINT8 *pUtfStr		= NULL;
	HUINT8 				tmpBuf[KEYBOARD_STRING_LENGTH] = {'\0',};

	HBOOL	bArabic = FALSE;
	HINT32 	j = 0;
	ONDKFontHandle		fontTempHandle;
	size_t utf8_size;

	ONDK_GWM_Keyboard_SetInputBoxSize(KEY_OBJ, 1 );

//	currentRow = ONDK_GWM_Keyboard_GetCurFocus_Row(KEY_OBJ);
//	currentCol = ONDK_GWM_Keyboard_GetCurFocus_Col(KEY_OBJ);
	//cursor_obj = ONDK_GWM_Obj_GetObjectByID(KEY_IMG_INPUTBOX_CURSOR_ID);

	total_str = HLIB_STR_Clone(pKeyContexts->strbuf);

	if (total_str == NULL)
	{
		HxLOG_Error("Failed Allocated!\n");
		return ONDK_RET_FAIL;
	}

	if (pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD)	;
	else if (ARABIC_SUPPORT_CheckUnicodeArabicCharacterInString(HLIB_STR_toUCS2(total_str)))
	{
		HxLOG_Debug("\n[%s.%d] Current string is Arabic ! \n\n",__FUNCTION__,__LINE__);
		bArabic = TRUE;
	}

	pUtfStr = HLIB_STR_GetBuffer(total_str);
	str_len = HxSTD_StrLen(pUtfStr);

	fontTempHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold ,FONT_NORMAL_SIZE);

	for (i=0; (*pUtfStr != '\0') && i < 3; i++)
	{
		HxSTR_t *strline 	= NULL;
		HUINT8 	*pTemp 		= NULL;
		j = 0;
		str_width	=	0;
		memset(tmpBuf, '\0', KEYBOARD_STRING_LENGTH);

		while( *pUtfStr != '\0' && str_width < AP_KEYBOARD_MAX_INPUT_WIDTH )
		{
			utf8_size = utf8_skip_data[(int)*pUtfStr];

			if(pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD && pKeyContexts->pwOnOff == 0)
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
				str_width = ONDK_FONT_GetStringWidth(fontTempHandle,tmpBuf);
			}
			else if(pKeyContexts->inputMode == KEYBOARD_INPUT_PASSWD && pKeyContexts->pwOnOff == 1)
			{
				if(str_len-1 == j)
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

				if (bArabic)
				{
					HxSTR_t *pAra = NULL;
					pAra = (HxSTR_t *)HLIB_STR_New(tmpBuf);
					if (pAra == NULL)
					{
						HxLOG_Error("Failed Allocated!\n");
						if(total_str)
							HLIB_STR_Delete(total_str);
						ONDK_FONT_Release(fontTempHandle);
						return ONDK_RET_FAIL;
					}
					pAra = (HxSTR_t *)ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(pAra);
					if (pAra)
					{
						str_width = ONDK_FONT_GetStringWidth(fontTempHandle,HLIB_STR_GetBuffer(pAra));
						HLIB_STR_Delete(pAra);
					}
				}
				else
				{
					str_width = ONDK_FONT_GetStringWidth(fontTempHandle,tmpBuf);
				}
			}
			else if(pKeyContexts->inputMode == KEYBOARD_INPUT_NORMAL)
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
				if (bArabic)
				{
					HxSTR_t *pAra = NULL;
					pAra = (HxSTR_t *)HLIB_STR_New(tmpBuf);
					if (pAra == NULL)
					{
						HxLOG_Error("Failed Allocated!\n");
						if(total_str)
						{
							HLIB_STR_Delete(total_str);
						}
						ONDK_FONT_Release(fontTempHandle);
						return ONDK_RET_FAIL;
					}
					pAra = (HxSTR_t *)ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(pAra);
					if(pAra)
					{
						str_width = ONDK_FONT_GetStringWidth(fontTempHandle,HLIB_STR_GetBuffer(pAra));
						if (pAra)
						{
							HLIB_STR_Delete(pAra);
						}
					}
				}
				else
				{
					str_width = ONDK_FONT_GetStringWidth(fontTempHandle,tmpBuf);
				}
			}
		}// end while
		tmpBuf[j] = '\0';

		pTemp = (HUINT8 *)HxSTR_MemAlloc( (j + 1) * sizeof(HUINT8));

		if (pTemp != NULL)
		{
			// copy string of current line
			HxSTD_StrNCpy(pTemp, tmpBuf , (sizeof(HUINT8) * (j + 1)) );

			strline = HLIB_STR_New(pTemp);

			if (strline == NULL)
			{
				if (pTemp)
					HxSTR_MemFree(pTemp);
				continue;
			}

			if (bArabic)
			{
				HxSTR_t *pTempStr = NULL;

				pTempStr = (HxSTR_t *)ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(strline);
				if (pTempStr == NULL)
				{
					HxLOG_Debug("[%s,%d] Failed Convert To VisualAra!!\n",__FUNCTION__,__LINE__);
				}
				else
				{
					HxLOG_Debug("\n\n[%s,%d] Saved Visual Arabic !!!\n\n",__FUNCTION__,__LINE__);
					strline = pTempStr;
				}
			}

			switch(i)
			{
				case 0:			// input line 1
					HxSTD_MemSet(pKeyContexts->input1, '\0', (sizeof(HUINT8) * KEYBOARD_STRING_LENGTH));
					HxSTD_StrNCpy(pKeyContexts->input1, HLIB_STR_GetBuffer(strline), HxSTD_StrLen(HLIB_STR_GetBuffer(strline)));
					pKeyContexts->input1[HxSTD_StrLen(HLIB_STR_GetBuffer(strline))] = '\0';

					break;

				case 1:			// input line 2
					HxSTD_MemSet(pKeyContexts->input2, '\0', (sizeof(HUINT8) * KEYBOARD_STRING_LENGTH));
					HxSTD_StrNCpy(pKeyContexts->input2, HLIB_STR_GetBuffer(strline), HxSTD_StrLen(HLIB_STR_GetBuffer(strline)));
					pKeyContexts->input2[HxSTD_StrLen(HLIB_STR_GetBuffer(strline))] = '\0';

					break;

				case 2:			// input line 3
					HxSTD_MemSet(pKeyContexts->input3, '\0', (sizeof(HUINT8) * KEYBOARD_STRING_LENGTH));
					HxSTD_StrNCpy(pKeyContexts->input3, HLIB_STR_GetBuffer(strline), HxSTD_StrLen(HLIB_STR_GetBuffer(strline)));
					pKeyContexts->input3[HxSTD_StrLen(HLIB_STR_GetBuffer(strline))] = '\0';

					break;

				default:
					break;
			}

			// previous 나 after 나 값이 같아 무의미..
			//str_width = local_keyboard_GetTotalStringWidth(strline);
		#if 0
			if (bArabic)
			{
				rect.x = AP_KEYBOARD_MAX_INPUT_WIDTH + 79 - str_width;

			}
			else
			{
				rect.x = AP_KEYBOARD_INPUTBOX3_X+19 + str_width;
			}
		#endif

			if (strline)
				HLIB_STR_Delete(strline);
		}
		else
		{
			HxLOG_Error("Failed Allocated!\n");
			if(total_str)
				HLIB_STR_Delete(total_str);
			ONDK_FONT_Release(fontTempHandle);
			return ONDK_RET_FAIL;
		}

		if (pTemp != NULL)
			HxSTR_MemFree(pTemp);
	}
	ONDK_FONT_Release(fontTempHandle);

	ONDK_GWM_Keyboard_SetInputBoxSize(KEY_OBJ, i );

	// edit input 창이 3개 만 지원 하되게 코드가 되어 잇네... ㅠ.ㅠ
	if (*pUtfStr == '\0' && total_str != NULL)
	{
		HxLOG_Debug("[%s,%d]Copy  total_str to pKeyContexts->strbuf. i : %d\n",__FUNCTION__,__LINE__, i);
		HLIB_STR_Delete(total_str);

		if(i==0)
			ONDK_GWM_Keyboard_SetInputBoxSize(KEY_OBJ, 1 );
	}
	else if (i >= 3 && *pUtfStr != '\0')		// input 3 line 이 넘어가면 마지막 입력은 지움.. 흠...
	{
		HINT32 total_len = 0;

		HxLOG_Debug("[%s,%d] input size over 3\n",__FUNCTION__,__LINE__);
		if(total_str)
			HLIB_STR_Delete(total_str);

		len = local_keyboard_GetByteSize(pKeyContexts->strbuf,0,pKeyContexts->currentPtr-1);
		ptr_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(pKeyContexts->strbuf,pKeyContexts->currentPtr-1));
		total_len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, HLIB_STR_GetBuffer(pKeyContexts->strbuf));

		if(ptr_len==2)
		{
			HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf,len);
			HLIB_STR_SetAtBuffer(pKeyContexts->strbuf,total_len-2,0);
		}
		else
		{
			HLIB_STR_RemoveAtBuffer(pKeyContexts->strbuf,len);
			HLIB_STR_SetAtBuffer(pKeyContexts->strbuf,total_len-1,0);
		}

		pKeyContexts->currentPtr--;
	}
	else
		HxLOG_Debug("\n\n[%s,%d] what the.....\n\n",__FUNCTION__,__LINE__);		// 이안에 들어오면 머지...?

	if(bArabic)
	{
		ONDK_GWM_COM_Text_SetAlign(KEY_INPUTBOX1_TEXT_ID, TEXT_ALIGN_RIGHT);
		ONDK_GWM_COM_Text_SetAlign(KEY_INPUTBOX2_TEXT_ID, TEXT_ALIGN_RIGHT);
		ONDK_GWM_COM_Text_SetAlign(KEY_INPUTBOX3_TEXT_ID, TEXT_ALIGN_RIGHT);
	}
	else
	{
		ONDK_GWM_COM_Text_SetAlign(KEY_INPUTBOX1_TEXT_ID, TEXT_ALIGN_LEFT);
		ONDK_GWM_COM_Text_SetAlign(KEY_INPUTBOX2_TEXT_ID, TEXT_ALIGN_LEFT);
		ONDK_GWM_COM_Text_SetAlign(KEY_INPUTBOX3_TEXT_ID, TEXT_ALIGN_LEFT);
	}

	//... 가슴이 아프넹...ㅠ.ㅠ
	switch(i)
	{
		case 1:
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX1_TEXT_ID,pKeyContexts->input1);
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX2_TEXT_ID,"");
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX3_TEXT_ID,"");
			break;

		case 2:
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX1_TEXT_ID,pKeyContexts->input2);
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX2_TEXT_ID,pKeyContexts->input1);
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX3_TEXT_ID,"");
			break;

		case 3:
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX1_TEXT_ID,pKeyContexts->input3);
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX2_TEXT_ID,pKeyContexts->input2);
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX3_TEXT_ID,pKeyContexts->input1);
			break;

		default:
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX1_TEXT_ID,"");
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX2_TEXT_ID,"");
			ONDK_GWM_COM_Text_SetText(KEY_INPUTBOX3_TEXT_ID,"");
			rect.x = AP_KEYBOARD_INPUTBOX3_X+19;
			break;
	}

	// align cursor
	if (s_curKeyboardLang == eLangID_Arabic ||
			s_curKeyboardLang == eLangID_Persian)
	{
		if (str_width == 0)
		{	// string 이 없는 경우 default 처리
			rect.x = AP_KEYBOARD_MAX_INPUT_WIDTH + 79 - str_width;
		}
		else
		{
			if (bArabic != TRUE)  // keyboard 상태가 arabic 인데도 영어만 포함된 경우.
				rect.x = AP_KEYBOARD_INPUTBOX3_X+19 + str_width;
			else
				rect.x = AP_KEYBOARD_MAX_INPUT_WIDTH + 79 - str_width;
		}
	}
	else
	{
		if (str_width == 0)
		{	// string 이 없는 경우 default 처리
			rect.x = AP_KEYBOARD_INPUTBOX3_X+19 + str_width;
		}
		else
		{
			if (bArabic == TRUE)  // keyboard 상태가 arabic 아니지만, string 에 arabic 가 포함된경우.
				rect.x = AP_KEYBOARD_MAX_INPUT_WIDTH + 79 - str_width;
			else
				rect.x = AP_KEYBOARD_INPUTBOX3_X+19 + str_width;
		}
	}

	rect.y = AP_KEYBOARD_INPUTBOX3_Y+8 + 74;
	rect.w = AP_KEYBOARD_INPUTBOXCUSOR_W;
	rect.h = AP_KEYBOARD_INPUTBOXCUSOR_H;
	ONDK_GWM_Obj_Resize(KEY_IMG_INPUTBOX_CURSOR_ID, &rect);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return lRet;
}

static ONDK_Result_t local_keyboard_SettingUsingTable(Key_Contents_t *pKeyContexts)
{
	ONDK_Result_t             lRet = ONDK_RET_OK;
	HUINT8                    utfItem[8] = {0, };
	HUINT16                   ucsitem[2] = {0, };
	HINT32			len = 0;
	HINT32			i = 0, j = 0, k = 0;
	HBOOL			bUpperCase = FALSE;
	HINT16 tmp_Charset[2][KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT];
	HINT16 tmp_Numericset[2][KEYBOARD_NUM_BTN_COUNT][KEYBOARD_NUMERIC_COUNT];

	pKeyContexts->modeToggle = 0;
	pKeyContexts->modeUpLow = 1;

	bUpperCase = (pKeyContexts->modeUpLow == 1) ? FALSE : TRUE;
	ONDK_GWM_Keyboard_SetUppercaseAndLowercaseLetters(KEY_OBJ, bUpperCase);

	for(i=0;i<2;i++)
	{
         for(j=0;j<KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT;j++)
         {
#if defined(CONFIG_PROD_LANG_ENGLISH)
			if(pKeyContexts->language == eCHARSET_English)
			{
				tmp_Charset[i][j] = s_CharSet_Eng[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_GERMAN)
			else if(pKeyContexts->language == eCHARSET_German)
			{
			    tmp_Charset[i][j] = s_CharSet_Ger[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_TURKISH)
			else if(pKeyContexts->language == eCHARSET_Turkish)
			{
			    tmp_Charset[i][j] = s_CharSet_Tur[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_RUSSIAN)
			else if(pKeyContexts->language == eCHARSET_Russian)
			{
			    tmp_Charset[i][j] = s_CharSet_Rus[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_ARABIC)
			else if(pKeyContexts->language == eCHARSET_Arabic)
			{
			    tmp_Charset[i][j] = s_CharSet_Ara[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_PERSIAN)
			else if(pKeyContexts->language == eCHARSET_Persian)
			{
			    tmp_Charset[i][j] = s_CharSet_Per[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_FRENCH)
			else if(pKeyContexts->language == eCHARSET_French)
			{
			    tmp_Charset[i][j] = s_CharSet_Fre[i][j];
	        }
#endif
#if defined(CONFIG_PROD_LANG_ITALIAN)
			else if(pKeyContexts->language == eCHARSET_Italian)
			{
				tmp_Charset[i][j] = s_CharSet_Ita[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_SPANISH)
			else if(pKeyContexts->language == eCHARSET_Spanish)
			{
				tmp_Charset[i][j] = s_CharSet_Spa[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_GREEK)
			else if(pKeyContexts->language == eCHARSET_Greek)
			{
				tmp_Charset[i][j] = s_CharSet_Gre[i][j];
			}
#endif
#if defined(CONFIG_PROD_LANG_SWEDISH)
			else if(pKeyContexts->language == eCHARSET_Swedish)
			{
				tmp_Charset[i][j] = s_CharSet_Swe[i][j];
			}
#endif
			else
			{
				//HxLOG_Print("=======> Error local_keyboard_SettingUsingTable : %s\n", pKeyContexts->language);
			}
         }
	}

	for(i=0;i<2;i++)
	{
		for(j=0;j<KEYBOARD_CHAR_BTN_COUNT*KEYBOARD_KEYPADSET_COUNT;j++)
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
				if(len )
				{
					// no work
				}
				//alen = HLIB_CP_NumOfBytes (eHxCP_UNICODE_UTF8, utfItem);
				//ONDK_Debug("[%s:%d] len(%d) utfItem(, 0x%02d, 0x%02d, 0x%02d, 0x%02d)\r\n", __FUNCTION__, __LINE__, len, utfItem[0], utfItem[1], utfItem[2], utfItem[3]);
			}

			HxSTD_StrNCpy(s_CharSet_Using[i][j], utfItem,HxSTD_StrLen(utfItem));
			s_CharSet_Using[i][j][HxSTD_StrLen(utfItem)] = '\0';
		}
	}

	for(k=0;k<2;k++)
	{
		for(i=0;i<KEYBOARD_NUM_BTN_COUNT;i++)
		{
			for(j=0;j<KEYBOARD_NUMERIC_COUNT;j++)
		    {
				tmp_Numericset[k][i][j] = s_NumericSet[k][i][j];
			}
		}
	}

	for(k=0;k<2;k++)
	{
		for(i=0;i<KEYBOARD_NUM_BTN_COUNT;i++)
		{
			for(j=0;j<KEYBOARD_NUMERIC_COUNT;j++)
		    {
				if(tmp_Numericset[k][i][j] == 0x0020)
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

				HxSTD_StrNCpy(s_NumericSet_Using[k][i][j], utfItem,HxSTD_StrLen(utfItem));
				s_NumericSet_Using[k][i][j][HxSTD_StrLen(utfItem)] = '\0';
			}
		}
	}

	//num table setting
	for(i=0;i<KEYBOARD_NUM_BTN_COUNT;i++)
	{
         s_NumSet_Using[i][0] = s_NumSet[i];
         s_NumSet_Using[i][1] = '\0';
	}

	return lRet;
}

static HINT32 local_keyboard_GetByteSize(HxSTR_t *str, HINT32 start, HINT32 cnt)
{
	HINT32 lRet = 0 ;
	HINT32 i;

	for(i=start;i<cnt;i++)
	{
		lRet += HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,HLIB_STR_GetAt(str,i));
	}

	return lRet;
}

static void 		local_keyboard_SetCharSet(Key_Contents_t *pKeyContexts)
{
	HINT32 i, len;

	for(i=0;i<KEYBOARD_CHAR_BTN_COUNT;i++)
	{
		ONDK_GWM_Keyboard_SetCharValue(KEY_OBJ,i,s_CharSet_Using[pKeyContexts->modeUpLow][i+pKeyContexts->modeToggle]);
	}

	len = HxSTD_StrLen(s_CharSet_Using[pKeyContexts->modeUpLow][0]);
	if(len < KEYBOARD_BUFFSIZE)
	{
		HxSTD_StrNCpy(pKeyContexts->infoLang,s_CharSet_Using[pKeyContexts->modeUpLow][0], len);
		pKeyContexts->infoLang[HxSTD_StrLen(s_CharSet_Using[pKeyContexts->modeUpLow][0])] = '\0';
	}
	else
	{
		HxLOG_Error("Cannot copy a key array to pKeyContexts->infoLang !!!");
		return;
	}

	len = HxSTD_StrLen(pKeyContexts->infoLang)+HxSTD_StrLen(s_CharSet_Using[pKeyContexts->modeUpLow][1]);
	if(len < KEYBOARD_BUFFSIZE)
	{
		HxSTD_StrNCat(pKeyContexts->infoLang,s_CharSet_Using[pKeyContexts->modeUpLow][1],HxSTD_StrLen(s_CharSet_Using[pKeyContexts->modeUpLow][1]));
		len = HxSTD_StrLen(pKeyContexts->infoLang)+HxSTD_StrLen(s_CharSet_Using[pKeyContexts->modeUpLow][1]);
		if (len >=KEYBOARD_BUFFSIZE) len = KEYBOARD_BUFFSIZE-1;
		pKeyContexts->infoLang[len] = '\0';
	}
	else return;

	len = HxSTD_StrLen(pKeyContexts->infoLang)+HxSTD_StrLen(s_CharSet_Using[pKeyContexts->modeUpLow][2]);
	if(len < KEYBOARD_BUFFSIZE)
	{
		HxSTD_StrNCat(pKeyContexts->infoLang,s_CharSet_Using[pKeyContexts->modeUpLow][2],HxSTD_StrLen(s_CharSet_Using[pKeyContexts->modeUpLow][2]));
		len = HxSTD_StrLen(pKeyContexts->infoLang)+HxSTD_StrLen(s_CharSet_Using[pKeyContexts->modeUpLow][2]);
		if (len >=KEYBOARD_BUFFSIZE) len = KEYBOARD_BUFFSIZE-1;
		pKeyContexts->infoLang[len] = '\0';
	}
	else return;

	len = HxSTD_StrLen(pKeyContexts->infoLang)+HxSTD_StrLen(" - ");
	if(len < KEYBOARD_BUFFSIZE)
	{
		HxSTD_StrNCat(pKeyContexts->infoLang," - ",HxSTD_StrLen(" - "));
		len = HxSTD_StrLen(pKeyContexts->infoLang)+HxSTD_StrLen(" - ");
		if(len >= KEYBOARD_BUFFSIZE) len = KEYBOARD_BUFFSIZE-1;
		pKeyContexts->infoLang[len] = '\0';
	}
	else return;

	len = HxSTD_StrLen(pKeyContexts->infoLang)+HxSTD_StrLen(pKeyContexts->strLang);
	if(len < KEYBOARD_BUFFSIZE)
	{
		HxSTD_StrNCat(pKeyContexts->infoLang,pKeyContexts->strLang,HxSTD_StrLen(pKeyContexts->strLang));
		len = HxSTD_StrLen(pKeyContexts->infoLang)+HxSTD_StrLen(pKeyContexts->strLang);
		if (len >=KEYBOARD_BUFFSIZE) len = KEYBOARD_BUFFSIZE-1;
		pKeyContexts->infoLang[len] = '\0';
	}
	else return;

	ONDK_GWM_Keyboard_SetLanguage(KEY_OBJ,pKeyContexts->infoLang);

}

static void 		local_keyboard_SetNumSet(Key_Contents_t *pKeyContexts)
{
	HINT32 i;

	for(i=0;i<10;i++)
	{
		ONDK_GWM_Keyboard_SetNumValue(KEY_OBJ,i,s_NumSet_Using[i]);
	}
}

static void		local_keyboard_SetCharButtonCount(Key_Contents_t *pKeyContexts)
{
	if(pKeyContexts->language == eCHARSET_English)
	{
		if(pKeyContexts->modeToggle > KEYBOARD_CHAR_BTN_COUNT*2)
			pKeyContexts->modeToggle = 0 ;
	}
	else if(pKeyContexts->language == eCHARSET_German ||
		pKeyContexts->language == eCHARSET_Turkish ||
		pKeyContexts->language == eCHARSET_French ||
		pKeyContexts->language == eCHARSET_Italian ||
		pKeyContexts->language == eCHARSET_Spanish ||
		pKeyContexts->language == eCHARSET_Greek ||
		pKeyContexts->language == eCHARSET_Swedish)
	{
		if(pKeyContexts->modeToggle > KEYBOARD_CHAR_BTN_COUNT*3)
			pKeyContexts->modeToggle = 0 ;
	}
	else if(pKeyContexts->language == eCHARSET_Russian)
	{
		if(pKeyContexts->modeToggle > KEYBOARD_CHAR_BTN_COUNT*4)
			pKeyContexts->modeToggle = 0 ;
	}
	else if(pKeyContexts->language == eCHARSET_Arabic)
	{
		if(pKeyContexts->modeToggle > KEYBOARD_CHAR_BTN_COUNT*1)
			pKeyContexts->modeToggle = 0 ;
	}
	else if(pKeyContexts->language == eCHARSET_Persian)
	{
		if(pKeyContexts->modeToggle > KEYBOARD_CHAR_BTN_COUNT*1)
			pKeyContexts->modeToggle = 0 ;
	}
	else
	{
		if(pKeyContexts->modeToggle > KEYBOARD_CHAR_BTN_COUNT*4)
			pKeyContexts->modeToggle = 0 ;
	}
}

static void		local_keyboard_ChangeLanguage(Key_Contents_t *pKeyContexts)
{
#if defined(CONFIG_OP_ERTELECOM)
	local_keyboard_ChangeLanguageForErtelcom(pKeyContexts);
#else
	local_keyboard_ChangeLanguageForDefault(pKeyContexts);
#endif
}


static void		local_keyboard_ChangeLanguageForDefault(Key_Contents_t *pKeyContexts)
{
	HBOOL		bFoundNextCharSet = FALSE;
	Key_Charset_e	eNextLanguage = pKeyContexts->language + 1;

	while (pKeyContexts->language != eNextLanguage)
	{
		if(eNextLanguage == eCHARSET_Max)	eNextLanguage = eCHARSET_English;

		switch (eNextLanguage)
		{
			case eCHARSET_English:
#if defined(CONFIG_PROD_LANG_ENGLISH)
				HxSTD_StrNCpy(pKeyContexts->strLang, "English", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_English;
#endif
				break;
			case eCHARSET_German:
#if defined(CONFIG_PROD_LANG_GERMAN)
				HxSTD_StrNCpy(pKeyContexts->strLang, "German", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_German;
#endif
				break;
			case eCHARSET_Turkish:
#if defined(CONFIG_PROD_LANG_TURKISH)
				HxSTD_StrNCpy(pKeyContexts->strLang, "Turkish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_Turkish;
#endif
				break;
			case eCHARSET_Russian:
#if defined(CONFIG_PROD_LANG_RUSSIAN)
				HxSTD_StrNCpy(pKeyContexts->strLang, "Russian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_Russian;
#endif
				break;
			case eCHARSET_Arabic:
#if defined(CONFIG_PROD_LANG_ARABIC)
				HxSTD_StrNCpy(pKeyContexts->strLang, "Arabic", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_Arabic;
#endif
				break;
			case eCHARSET_French:
#if defined(CONFIG_PROD_LANG_FRENCH)
				HxSTD_StrNCpy(pKeyContexts->strLang, "French", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_French;
#endif
				break;
			case eCHARSET_Italian:
#if defined(CONFIG_PROD_LANG_ITALIAN)
				HxSTD_StrNCpy(pKeyContexts->strLang, "Italian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_Italian;
#endif
				break;
			case eCHARSET_Spanish:
#if defined(CONFIG_PROD_LANG_SPANISH)
				HxSTD_StrNCpy(pKeyContexts->strLang, "Spanish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_Spanish;
#endif
				break;
			case eCHARSET_Greek:
#if defined(CONFIG_PROD_LANG_GREEK)
				HxSTD_StrNCpy(pKeyContexts->strLang, "Greek", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_Greek;
#endif
				break;
			case eCHARSET_Persian:
#if defined(CONFIG_PROD_LANG_PERSIAN)
				HxSTD_StrNCpy(pKeyContexts->strLang, "Persian", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_Persian;
#endif
				break;
			case eCHARSET_Swedish:
#if defined(CONFIG_PROD_LANG_SWEDISH)
				HxSTD_StrNCpy(pKeyContexts->strLang, "Swedish", ONDK_LANGUAGE_STRING_MAX_LENGTH);
				pKeyContexts->strLang[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';
				bFoundNextCharSet = TRUE;
				s_curKeyboardLang = eLangID_Swedish;
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

#if defined(CONFIG_OP_ERTELECOM)
static void		local_keyboard_ChangeLanguageForErtelcom(Key_Contents_t *pKeyContexts)
{
#if 0

#endif
}
#endif

