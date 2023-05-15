
/**************************************************************
 *	@file		mlib.h
 *	Minimized Graphic Library for DirectFB
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/07
 *	@author			humax
 *	@brief			Minimized Graphic Library for DirectFB
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


#ifndef ____ONDK_KEYTYPE_H____
#define ____ONDK_KEYTYPE_H____


typedef enum _ONDK_KEY_Code
{
/*******************************************************************/
/* RCU Keys                                                        */
/*******************************************************************/

/* 0x0001 */	KEY_STANDBY = 0x0001,			/* Garbage 로 0 들어가지 않도록... */

/*******************************************************************/
/* F1-12 Keys                                                        */
/*******************************************************************/

/* 0x0010 */	KEY_F1 = 0x0010,
/* 0x0011 */	KEY_F2 = 0x0011,
/* 0x0012 */	KEY_F3 = 0x0012,
/* 0x0013 */	KEY_F4 = 0x0013,
/* 0x0014 */	KEY_F5 = 0x0014,
/* 0x0015 */	KEY_F6 = 0x0015,
/* 0x0016 */	KEY_F7 = 0x0016,
/* 0x0017 */	KEY_F8 = 0x0017,
/* 0x0018 */	KEY_F9 = 0x0018,
/* 0x0019 */	KEY_F10 = 0x0019,
/* 0x001A */	KEY_F11 = 0x001A,
/* 0x001B */	KEY_F12 = 0x001B,

/* Input Key Code *********************************************************/
/* Enum 값을 바로 16진수로 인식하여 사용하므로 임의 순서 변경 하지 마세요 */

/* 0x0021 */	KEY_EXCLAMATION_MARK	= 0x0021,
/* 0x0022 */	KEY_QUOTATION			= 0x0022,
/* 0x0023 */	KEY_NUMBER_SIGN			= 0x0023,
/* 0x0024 */	KEY_DOLLAR_SIGN			= 0x0024,
/* 0x0025 */	KEY_PERCENT_SIGN		= 0x0025,
/* 0x0026 */	KEY_AMPERSAND			= 0x0026,
/* 0x0027 */	KEY_APOSTROPHE			= 0x0027,
/* 0x0028 */	KEY_PARENTHESIS_LEFT	= 0x0028,
/* 0x0029 */	KEY_PARENTHESIS_RIGHT	= 0x0029,
/* 0x002A */	KEY_ASTERISK			= 0x002A,
/* 0x002B */	KEY_PLUS_SIGN			= 0x002B,
/* 0x002C */	KEY_COMMA				= 0x002C,
/* 0x002D */	KEY_MINUS_SIGN			= 0x002D,
/* 0x002E */	KEY_PERIOD 				= 0x002E,
/* 0x002F */	KEY_SLASH 				= 0x002F,

/* 0x0030 */	KEY_0 					= 0x0030,
/* 0x0031 */	KEY_1,
/* 0x0032 */	KEY_2,
/* 0x0033 */	KEY_3,
/* 0x0034 */	KEY_4,
/* 0x0035 */	KEY_5,
/* 0x0036 */	KEY_6,
/* 0x0037 */	KEY_7,
/* 0x0038 */	KEY_8,
/* 0x0039 */	KEY_9,

/* 0x003A */	KEY_COLON				= 0x003A,
/* 0x003B */	KEY_SEMICOLON			= 0x003B,
/* 0x003C */	KEY_LESS_THAN_SIGN		= 0x003C,
/* 0x003D */	KEY_EQUALS_SIGN 		= 0x003D,
/* 0x003E */	KEY_GREATER_THAN_SIGN	= 0x003E,
/* 0x003F */	KEY_QUESTION_MARK		= 0x003F,
/* 0x0040 */	KEY_AT					= 0x0040,

/* 0x0041 */	KEY_CAPITAL_A 			= 0x0041,
/* 0x0042 */	KEY_CAPITAL_B 			= 0x0042,
/* 0x0043 */	KEY_CAPITAL_C 			= 0x0043,
/* 0x0044 */	KEY_CAPITAL_D 			= 0x0044,
/* 0x0045 */	KEY_CAPITAL_E 			= 0x0045,
/* 0x0046 */	KEY_CAPITAL_F 			= 0x0046,
/* 0x0047 */	KEY_CAPITAL_G 			= 0x0047,
/* 0x0048 */	KEY_CAPITAL_H 			= 0x0048,
/* 0x0049 */	KEY_CAPITAL_I 			= 0x0049,
/* 0x004A */	KEY_CAPITAL_J 			= 0x004A,
/* 0x004B */	KEY_CAPITAL_K 			= 0x004B,
/* 0x004C */	KEY_CAPITAL_L 			= 0x004C,
/* 0x004D */	KEY_CAPITAL_M 			= 0x004D,
/* 0x004E */	KEY_CAPITAL_N 			= 0x004E,
/* 0x004F */	KEY_CAPITAL_O 			= 0x004F,
/* 0x0050 */	KEY_CAPITAL_P 			= 0x0050,
/* 0x0051 */	KEY_CAPITAL_Q			= 0x0051,
/* 0x0052 */	KEY_CAPITAL_R 			= 0x0052,
/* 0x0053 */	KEY_CAPITAL_S 			= 0x0053,
/* 0x0054 */	KEY_CAPITAL_T 			= 0x0054,
/* 0x0055 */	KEY_CAPITAL_U 			= 0x0055,
/* 0x0056 */	KEY_CAPITAL_V 			= 0x0056,
/* 0x0057 */	KEY_CAPITAL_W 			= 0x0057,
/* 0x0058 */	KEY_CAPITAL_X 			= 0x0058,
/* 0x0059 */	KEY_CAPITAL_Y 			= 0x0059,
/* 0x005A */	KEY_CAPITAL_Z 			= 0x005A,

/* 0x005B */	KEY_SQUARE_BRACKET_LEFT 	= 0x005B,
/* 0x005C */	KEY_BACKSLASH				= 0x005C,
/* 0x005D */	KEY_SQUARE_BRACKET_RIGHT	= 0x005D,
/* 0x005E */	KEY_CIRCUMFLEX_ACCENT		= 0x005E,
/* 0x005F */	KEY_UNDERSCORE				= 0x005F,
/* 0x0060 */	KEY_GRAVE_ACCENT			= 0x0060,

/* 0x0061 */	KEY_SMALL_A = 0x0061,
/* 0x0062 */	KEY_SMALL_B = 0x0062,
/* 0x0063 */	KEY_SMALL_C = 0x0063,
/* 0x0064 */	KEY_SMALL_D = 0x0064,
/* 0x0065 */	KEY_SMALL_E = 0x0065,
/* 0x0066 */	KEY_SMALL_F = 0x0066,
/* 0x0067 */	KEY_SMALL_G = 0x0067,
/* 0x0068 */	KEY_SMALL_H = 0x0068,
/* 0x0069 */	KEY_SMALL_I = 0x0069,
/* 0x006A */	KEY_SMALL_J = 0x006A,
/* 0x006B */	KEY_SMALL_K = 0x006B,
/* 0x006C */	KEY_SMALL_L = 0x006C,
/* 0x006D */	KEY_SMALL_M = 0x006D,
/* 0x006E */	KEY_SMALL_N = 0x006E,
/* 0x006F */	KEY_SMALL_O = 0x006F,
/* 0x0070 */	KEY_SMALL_P = 0x0070,
/* 0x0071 */	KEY_SMALL_Q = 0x0071,
/* 0x0072 */	KEY_SMALL_R = 0x0072,
/* 0x0073 */	KEY_SMALL_S = 0x0073,
/* 0x0074 */	KEY_SMALL_T = 0x0074,
/* 0x0075 */	KEY_SMALL_U = 0x0075,
/* 0x0076 */	KEY_SMALL_V = 0x0076,
/* 0x0077 */	KEY_SMALL_W = 0x0077,
/* 0x0078 */	KEY_SMALL_X = 0x0078,
/* 0x0079 */	KEY_SMALL_Y = 0x0079,
/* 0x007A */	KEY_SMALL_Z = 0x007A,

/* 0x007B */	KEY_CURLY_BRACKET_LEFT 	= 0x007B,
/* 0x007C */	KEY_VERTICAL_BAR 		= 0x007C,
/* 0x007D */	KEY_CURLY_BRACKET_RIGHT = 0x007D,
/* 0x007E */	KEY_TILDE				= 0x007E,

/**************************************************************************/

/* Teletext 관련 keys **********************************************/
/* 0x0080 */	KEY_TEXT = 0x0080,				/* Teletext or MHEG launch. 다른 이름 : DATA, MHEG, ... */
/* 0x0081 */	KEY_TEXT_OPTION,				/* Teletext menu launching */
/* 0x0082 */	KEY_TTX_HOLD,
/* 0x0083 */	KEY_TTX_REVEAL,
/* 0x0084 */	KEY_TTX_DOUBLE,
/* 0x0085 */	KEY_TTX_CANCEL,
/* 0x0086 */	KEY_TTX_INDEX,
/* 0x0087 */	KEY_TTX_SUBPAGE,
/* 0x0088 */	KEY_TTX_UPDATE,

/* PVR & media keys ************************************************/
/* 0x0090 */	KEY_PLAY = 0x0090,
/* 0x0091 */	KEY_PAUSE,
/* 0x0092 */	KEY_STOP,
/* 0x0093 */	KEY_RECORD,
/* 0x0094 */	KEY_FORWARD,
/* 0x0095 */	KEY_BACKWARD,
/* 0x0096 */	KEY_PREV,
/* 0x0097 */	KEY_NEXT,
/* 0x0098 */	KEY_REPLAY,
/* 0x0099 */	KEY_SKIP,
/* 0x009A */	KEY_ADD_BOOKMARK,
/* 0x009B */	KEY_BOOKMARK_LIST,
/* 0x009C */	KEY_SLOW,
/* 0x009D */	KEY_TSR,

/* OP Specific Keys *****************************************************/
/* LG specific *****************************************************/
/* 0x00A0 */	KEY_QVIEW = 0x00A0,				/* Quick view. */
/* 0x00A1 */	KEY_ENERGY_GREEN,
/* 0x00A2 */	KEY_PSM,						/* Picture mode */
/* 0x00A3 */	KEY_SSM,						/* Sound mode */
/* 0x00A4 */	KEY_AD,							/* Audio description */

/* Truevision specific  *******************************************/
/* 0x00A5 */	KEY_ALERT,
/* 0x00A6 */	KEY_RECALL,
/* 0x00A7 */	KEY_SYSTEM,
/* 0x00A8 */	KEY_SETTING,

/* Digiturk Spec key ********************************************/
/* 0x00A9 */	KEY_OP_MODE,

/* AlMajd Spec key ********************************************/
/* 0x00AA */	KEY_WELCOM,
/* 0x00AB */	KEY_MAIL,

/* Test  key ********************************************/
/* 0x00B2 */	KEY_PIP_MOVE,

/* 부가 기능 keys. 주로 virtual keys or multi-press keys ***********/
/* 0x00C0 */	KEY_HOTELMENU,					/* Hotel menu launch */
/* 0x00C1 */	KEY_SERVICEMODE,
/* 0x00C2 */	KEY_LANGUAGE,
/* 0x00CA */	KEY_SHIFT,
/* 0x00CB */	KEY_CLEAR,
/* 0x00CC */    KEY_PROMOTION,
/* 0x00CD */    KEY_NETFLIX,

/* DVD/BD Keys  *******************************************/
/* 0x00D0 */	KEY_DISC = 0x00D0,
/* 0x00D1 */	KEY_TRAY_OPEN,
/* 0x00D2 */	KEY_DISC_MENU,
/* 0x00D3 */	KEY_DUB,

/* Mode  key ********************************************/
/* 0x00E0 */	KEY_MODE_PVR = 0x00E0,
/* 0x00E1 */	KEY_MODE_TV,
/* 0x00E2 */	KEY_MODE_DVD,
/* 0x00E3 */	KEY_MODE_AUDIO,
/* 0x00E4 */	KEY_MODE_WARNING,

/* Channel change keys *********************************************/
/* 0x00F0 */	KEY_CH_PLUS = 0x00F0,
/* 0x00F1 */	KEY_CH_MINUS,
/* 0x00F2 */	KEY_LAST,						/* Last channel tuning. */
/* 0x00F3 */	KEY_TVRADIO,					/* TV <-> Radio service change. */
/* 0x00F4 */	KEY_COMBO,						/* Delivery type or Network select. 다른 이름 : TV/DTV(JVC) */
/* 0x00F5 */	KEY_FREESAT,					/* Freesat selection */
/* 0x00F6 */	KEY_CH_INPUT,					/* Direct Channel Input */
/* 0x00F7 */	KEY_NETWORK_TER,				/* Terrestrial */
/* 0x00F8 */	KEY_NETWORK_BS, 				/* BS */
/* 0x00F9 */	KEY_NETWORK_CATV,				/* CATV */
/* 0x00FA */	KEY_DATA,						/* Data Channel, BML start */
/* 0x00FB */	KEY_DIGITS,						/* 3행 입력 */

/* AV signal 및 관련 UI keys ***************************************/
/* 0x0100 */	KEY_VOLUMEUP = 0x0100,
/* 0x0101 */	KEY_VOLUMEDOWN,
/* 0x0102 */	KEY_MUTE,
/* 0x0103 */	KEY_EXT_AV,						/* DTV/ATV/EXT launch or selection. */
/* 0x0104 */	KEY_EXT_AV_PLUS,				/* DTV/ATV/EXT launch or selection. */
/* 0x0105 */	KEY_EXT_AV_MINUS,				/* DTV/ATV/EXT launch or selection. */
/* 0x0106 */	KEY_AVMODE,						/* AV mode launch or selection. */
/* 0x0107 */	KEY_FREEZE,						/* Video freeze. */
/* 0x0108 */	KEY_WIDE,						/* Aspect ratio 변경 키. 다른 이름 : ZOOM(JVC) */
/* 0x0109 */	KEY_3D,							/* 3D surrround 모드 변경 키. 다른 이름 : SURROUND */
/* 0x010A */	KEY_SOURCE,						/* STB에서만 사용하는 decoder/SCART 출력 전환 키. KEY_EXT_AV랑 다름. */
/* 0x010B */	KEY_MHP,						/* ITALY : MHP Enable Key for RMG08 & RME09*/
/* 0x010C */	KEY_MYEPG,						/* ITALY : MYEPG Enable Key for RMG08 */
/* 0x010D */	KEY_TIVUEPG,					/* ITALY :TIVUEPG Enable Key for RMG08 & RME09*/
/* 0x010E */	KEY_OTTV,						/* ITALY : OTTV execution Key for RMG08 */


/* UI menu 관련 keys ***********************************************/
/* 0x0110 */	KEY_INFO = 0x0110,				/* i-plate or other EPG use. */
/* 0x0111 */	KEY_GUIDE,						/* EPG launch. 다른 이름 : EPG, ... */
/* 0x0112 */	KEY_MENU,						/* Portal launch. 다른 이름 : POTAL , KEY_HUB*/
/* 0x0113 */	KEY_QMENU,						/* Quick menu launch. */
/* 0x0114 */	KEY_FAVORITE,					/* Favorite channel function. */
/* 0x0115 */	KEY_CHLIST,						/* Channel list launching. */
/* 0x0116 */	KEY_SETUP,						/* Setup menu launching. */
/* 0x0117 */	KEY_VFORMAT,					/* Current resolution information. */
/* 0x0118 */	KEY_AUDIO,						/* Audio track or dual-mono selection. */
/* 0x0119 */	KEY_SUBTITLE,					/* Subtitle selection. */
/* 0x011A */	KEY_SLEEP,						/* Sleep timer function. */
/* 0x011B */	KEY_OPT,						/* Option + and KEY_PLUS*/
/* 0x011C */	KEY_SCHEDULE,					/* 예약 관련 기능. 예약 리스트 등 ... */
/* 0x011D */	KEY_MEDIA,						/* Media direct launch or other media related... */
/* 0x011E */	KEY_IP_BROWSER,					/* for web-browser */
/* 0x011F */	KEY_SELECT,						/* for SKY D spec */
/* 0x0120 */	KEY_RESERVATION = 0x0120,		/* 예약 화면 */
/* 0x0121 */	KEY_DLNA,						/* DLNA */
/* 0x0122 */	KEY_HDD,						/* HDD */
/* 0x0123 */	KEY_VOD,						/* VOD */
/* 0x0124 */ 	KEY_DISP_ON,					/* Display On */
/* 0x0125 */ 	KEY_HOTKEY,						/* Hot Key, One Touch, etc */
/* 0x0127 */ 	KEY_SMART_SEARCH,
/* 0x0128 */ 	KEY_AUDIO_LANGUAGE,
/* 0x0129 */ 	KEY_FAVORITES,
/* 0x012A */ 	KEY_PROGRAM,
/* 0x012B */ 	KEY_PLAYLIST,


/* Menu Navigation keys ********************************************/
/* 0x0130 */	KEY_PAGEUP = 0x0130,
/* 0x0131 */	KEY_PAGEDOWN,
/* 0x0132 */	KEY_ARROWLEFT, 				/* DFB 에서는 LEFT RIGHT UP DOWN 순서로  */
/* 0x0133 */	KEY_ARROWRIGHT,
/* 0x0134 */	KEY_ARROWUP,
/* 0x0135 */	KEY_ARROWDOWN,
/* 0x0136 */	KEY_BACK,						/* Previous menu or Last service tuning. */
/* 0x0137 */	KEY_EXIT,						/* Clear current UI and goto TV watching. */
/* 0x0138 */	KEY_OK		=0xF253 ,  		/* KEY_UNI_RETURN =0xF253 사용으로 인한 설정*/
/* 0x0139 */	KEY_ERASE,


/* Color keys ******************************************************/
/* 0x0140 */	KEY_RED = 0x0140,
/* 0x0141 */	KEY_GREEN,
/* 0x0142 */	KEY_YELLOW,
/* 0x0143 */	KEY_BLUE,


/*******************************************************************/
/* FRONT Keys                                                      */
/*******************************************************************/




/*******************************************************************/
/* FRONT Keys                                                      */
/*******************************************************************/

/* 0x0800 */	KEY_FRONT_STANDBY = 0x0800,
/* 0x0801 */	KEY_FRONT_EXT_AV,
/* 0x0802 */	KEY_FRONT_MENU,
/* 0x0803 */	KEY_FRONT_CH_PLUS,
/* 0x0804 */	KEY_FRONT_CH_MINUS,
/* 0x0805 */	KEY_FRONT_VOLUME_UP,
/* 0x0806 */	KEY_FRONT_VOLUME_DOWN,
/* 0x0807 */	KEY_FRONT_OK,
/* 0x0808 */	KEY_FRONT_ARROW_UP,
/* 0x0809 */	KEY_FRONT_ARROW_DOWN,
/* 0x080A */	KEY_FRONT_ARROW_LEFT,
/* 0x080B */	KEY_FRONT_ARROW_RIGHT,
/* 0x080C */	KEY_FRONT_TVRADIO,
/* 0x080D */	KEY_FRONT_GUIDE,
/* 0x080E */	KEY_FRONT_BACK,
/* 0x080F */	KEY_FRONT_INFO,

/* DVD Combo Front Keys  *******************************************/
/* 0x0810 */	KEY_FRONT_STOP = 0x0810,		/* DVD Stop */
/* 0x0811 */	KEY_FRONT_PLAY_PAUSE,			/* DVD Play/Pause */
/* 0x0812 */	KEY_FRONT_OPEN_CLOSE,			/* DVD Open/Close */
/* 0x0813 */	KEY_FRONT_HDD_DISC,				/* HDD/DISC(STB/DISC swap) */
/* 0x0814 */	KEY_FRONT_TER_BS_CATV,			/* Network Ter, BS, CATV */
/* 0x0815 */	KEY_FRONT_PLAY_LIST,				/* Paly List */
/* 0x0816 */	KEY_FRONT_RECORD,				/* record */

/* 부가 기능 keys. 주로 virtual keys or multi-press keys ***********/
/* 0x0880 */	KEY_FRONT_LOCK_OFF = 0x0880,	/* Front key lock off */
/* 0x0881 */	KEY_FRONT_HIDDEN,				/* into Hidden multi key ( HDD/DISC + Play ) */



/*******************************************************************/
/* SPECIAL Keys ****************************************************/
/*******************************************************************/
/* 0xF005 */	KEY_SPECIAL_INSERT		= 0xF004,
/* 0xF005 */	KEY_SPECIAL_HOME		= 0xF005,
/* 0xF005 */	KEY_SPECIAL_END			= 0xF006,

/* 0xF034 */	KEY_SPECIAL_AUX			= 0xF034,
/* 0xF059 */	KEY_SPECIAL_REWIND 		= 0xF059,
/* 0xF05A */	KEY_SPECIAL_FASTFORWARD	= 0xF05A,

/* 0xF10E */	KEY_SPECIAL_CLEAR		= 0xF10E,
/* 0xF123 */	KEY_SPECIAL_MODE		= 0xF123,
/* 0xF124 */	KEY_SPECIAL_KEYBOARD	= 0xF124,
/* 0xF125 */	KEY_SPECIAL_PC			= 0xF125,
/* 0xF126 */	KEY_SPECIAL_SCREEN		= 0xF126,
/* 0xF126 */	KEY_SPECIAL_PRINT		= 0xF127,

/*******************************************************************/
/* UNICODE Keys ****************************************************/
/*******************************************************************/

/* 0xF201 */	KEY_UNI_SHIFT		= 0xF201 ,
/* 0xF202 */	KEY_UNI_CONTROL		= 0xF202 ,
/* 0xF204 */	KEY_UNI_ALT			= 0xF204 ,
/* 0xF208 */	KEY_UNI_ALTGR		= 0xF208 ,

/* 0xF210 */	KEY_UNI_META		= 0xF210 ,

/* 0xF220 */	KEY_UNI_SUPER		= 0xF220 ,

/* 0xF240 */	KEY_UNI_HYPER		= 0xF240 ,

/* 0xF250 */	KEY_UNI_NULL		= 0xF250 ,
/* 0xF251 */	KEY_UNI_BACKSPACE	= 0xF251 ,
/* 0xF252 */	KEY_UNI_TAB			= 0xF252 ,
/* 0xF253 */	KEY_UNI_RETURN		= 0xF253 ,
/* 0xF254 */	KEY_UNI_CANCEL		= 0xF254 ,
/* 0xF255 */	KEY_UNI_ESCAPE		= 0xF255 ,
/* 0xF256 */	KEY_UNI_SPACE		= 0xF256 ,
/* 0xF257 */	KEY_UNI_DELETE		= 0xF257 ,

/*******************************************************************/
/* LOCK Keys ****************************************************/
/*******************************************************************/

/* 0xF300 */	KEY_CAPS_LOCK		= 0xF300 ,
/* 0xF301 */	KEY_NUM_LOCK		= 0xF301 ,
/* 0xF302 */	KEY_SCROLL_LOCK		= 0xF302 ,

/*******************************************************************/
/* CUSTOM Keys ****************************************************/
/*******************************************************************/

/* 0xF506 */ 	KEY_CUSTOM_SMART_SEARCH	= 0xF506,
/* 0xF507 */	KEY_CUSTOM_MULTI_CUSTOM = 0xF507,
/* 0xF508 */	KEY_CUSTOM_MULTI_CUSTOM_SUCCESS = 0xF508,
/* 0xF509 */	KEY_CUSTOM_MULTI_CUSTOM_FAIL	= 0xF509,
/* 0xF516 */	KEY_CUSTOM_PRESET_CH	= 0xF516,
/* 0xF553 */	KEY_CUSTOM_ETC			= 0xF553,
/* 0xF554 */	KEY_CUSTOM_RECOMMAND	= 0xF554,




/* 0xFFFF */	KEY_UNKNOWN = 0x00FFFF
} ONDK_KEY_Code_t;


/* system fonts ID enumeration */
typedef enum _ONDK_FontId{
	eFont_SystemNormal		= 0,
	eFont_SystemFixed,
	eFont_SystemBold,

	eFont_IdMax
} ONDK_FontId_t;


#endif





