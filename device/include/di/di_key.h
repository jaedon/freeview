/* $Header:   //DTV/////.   1.0   July 28 2008 13:25:52      jhson$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_key.h
// Original Author: Son Jung Hee
// File Description: IR & Key Header File.
// Module:
// Remarks:
//
//     input:
//
//     output:
//
//
/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_KEY_H__
#define __DI_KEY_H__

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files

#include "di_err.h"
#include "htype.h"

// End Including Headers

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define

// End #define

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee

typedef enum
{
	DI_FRONT_KEY = 0,
	DI_RCU_KEY,
	DI_KEY_DEVICE_MAX
} DI_KEY_DEVICE;

typedef enum
{
	DI_KEY_PRESSED = 0,
	DI_KEY_REPEATED,
	DI_KEY_RELEASED,
	DI_KEY_TOGGLED,
	DI_KEY_TYPE_MAX
} DI_KEY_TYPE;


typedef enum
{
/*******************************************************************/
/* RCU Keys                                                        */
/*******************************************************************/

/* 0x01 */	DI_KEY_REMOCON_STANDBY = 0x01, /* Garbage 로 0 들어가지 않도록... */

/* Numeric keys ****************************************************/
/* 0x10 */	DI_KEY_REMOCON_0 = 0x10,
/* 0x11 */	DI_KEY_REMOCON_1,
/* 0x12 */	DI_KEY_REMOCON_2,
/* 0x13 */	DI_KEY_REMOCON_3,
/* 0x14 */	DI_KEY_REMOCON_4,
/* 0x15 */	DI_KEY_REMOCON_5,
/* 0x16 */	DI_KEY_REMOCON_6,
/* 0x17 */	DI_KEY_REMOCON_7,
/* 0x18 */	DI_KEY_REMOCON_8,
/* 0x19 */	DI_KEY_REMOCON_9,
/* 0x1a */	DI_KEY_REMOCON_11 = 0x1a,
/* 0x1b */	DI_KEY_REMOCON_12 = 0x1b,

/* Color keys ******************************************************/
/* 0x20 */	DI_KEY_REMOCON_RED = 0x20,
/* 0x21 */	DI_KEY_REMOCON_GREEN,
/* 0x22 */	DI_KEY_REMOCON_YELLOW,
/* 0x23 */	DI_KEY_REMOCON_BLUE,

/* UI menu 관련 keys ***********************************************/
/* 0x30 */	DI_KEY_REMOCON_INFO = 0x30, /* i-plate or other EPG use. */
/* 0x31 */	DI_KEY_REMOCON_GUIDE, /* EPG launch. 다른 이름 : EPG, ... */
/* 0x32 */	DI_KEY_REMOCON_MENU, /* Portal launch. 다른 이름 : POTAL */
/* 0x33 */	DI_KEY_REMOCON_QMENU,	/* Quick menu launch. */
/* 0x34 */	DI_KEY_REMOCON_FAVORITE, /* Favorite channel function. */
/* 0x35 */	DI_KEY_REMOCON_CHLIST, /* Channel list launching. */
/* 0x36 */	DI_KEY_REMOCON_SETUP, /* Setup menu launching. */
/* 0x37 */	DI_KEY_REMOCON_VFORMAT, /* Current resolution information. */
/* 0x38 */	DI_KEY_REMOCON_AUDIO, /* Audio track or dual-mono selection. */
/* 0x39 */	DI_KEY_REMOCON_SUBTITLE, /* Subtitle selection. */
/* 0x3A */	DI_KEY_REMOCON_SLEEP, /* Sleep timer function. */
/* 0x3B */	DI_KEY_REMOCON_OPT, /* Option + */
/* 0x3C */	DI_KEY_REMOCON_SCHEDULE, /* 예약 관련 기능. 예약 리스트 등 ... */
/* 0x3D */	DI_KEY_REMOCON_MEDIA, /* Media direct launch or other media related... */
/* 0x3E */	DI_KEY_REMOCON_IP_BROWSER, /* for web-browser */
/* 0x3F */	DI_KEY_REMOCON_HELP,

/* Menu Navigation keys ********************************************/
/* 0x40 */	DI_KEY_REMOCON_PAGEUP = 0x40,
/* 0x41 */	DI_KEY_REMOCON_PAGEDOWN,
/* 0x42 */	DI_KEY_REMOCON_ARROWUP,
/* 0x43 */	DI_KEY_REMOCON_ARROWDOWN,
/* 0x44 */	DI_KEY_REMOCON_ARROWLEFT,
/* 0x45 */	DI_KEY_REMOCON_ARROWRIGHT,
/* 0x46 */	DI_KEY_REMOCON_BACK, /* Previous menu or Last service tuning. */
/* 0x47 */	DI_KEY_REMOCON_EXIT, /* Clear current UI and goto TV watching. */
/* 0x48 */	DI_KEY_REMOCON_OK,
/* 0x49 */	DI_KEY_REMOCON_SELECT,
/* 0x4A */	DI_KEY_REMOCON_PAGEPREV,/* Previous page */
/* 0x4B */	DI_KEY_REMOCON_PAGENEXT,    /* Next page */

/* Channel change keys *********************************************/
/* 0x50 */	DI_KEY_REMOCON_CH_PLUS = 0x50,
/* 0x51 */	DI_KEY_REMOCON_CH_MINUS,
/* 0x52 */	DI_KEY_REMOCON_LAST, /* Last channel tuning. */
/* 0x53 */	DI_KEY_REMOCON_TVRADIO, /* TV <-> Radio service change. */
/* 0x54 */	DI_KEY_REMOCON_COMBO,	/* Delivery type or Network select. 다른 이름 : TV/DTV(JVC) */
/* 0x55 */	DI_KEY_REMOCON_FREESAT, /* Freesat selection */
/* 0x56 */	DI_KEY_REMOCON_ONE_TOUCH, /* Channel shortcut key JAPAN*/
/* 0x57 */	DI_KEY_REMOCON_CH_EDIT,   /* enter 3 rows (채널 변경 위해 번호를 입력하면 되지만 일본의 경우 3단 입력을 선택 후 채널 번호를 넣는다.) JAPAN */

/* AV signal 및 관련 UI keys ***************************************/
/* 0x60 */	DI_KEY_REMOCON_VOLUMEUP = 0x60,
/* 0x61 */	DI_KEY_REMOCON_VOLUMEDOWN,
/* 0x62 */	DI_KEY_REMOCON_MUTE,
/* 0x63 */	DI_KEY_REMOCON_EXT_AV, /* DTV/ATV/EXT launch or selection. */
/* 0x64 */	DI_KEY_REMOCON_EXT_AV_PLUS, /* DTV/ATV/EXT launch or selection. */
/* 0x65 */	DI_KEY_REMOCON_EXT_AV_MINUS, /* DTV/ATV/EXT launch or selection. */
/* 0x66 */	DI_KEY_REMOCON_AVMODE, /* AV mode launch or selection. */
/* 0x67 */	DI_KEY_REMOCON_FREEZE, /* Video freeze. */
/* 0x68 */	DI_KEY_REMOCON_WIDE, /* Aspect ratio 변경 키. 다른 이름 : ZOOM(JVC) */
/* 0x69 */	DI_KEY_REMOCON_3D, /* 3D surrround 모드 변경 키. 다른 이름 : SURROUND */
/* 0x6A */	DI_KEY_REMOCON_SOURCE, /* STB에서만 사용하는 decoder/SCART 출력 전환 키. KEY_EXT_AV랑 다름. */

/* Teletext 관련 keys **********************************************/
/* 0x70 */	DI_KEY_REMOCON_TELETEXT = 0x70, /* Teletext or MHEG launch. 다른 이름 : DATA, MHEG, ... */
/* 0x71 */	DI_KEY_REMOCON_TELETEXT_OPTION, /* Teletext menu launching */
/* 0x72 */	DI_KEY_REMOCON_TTX_HOLD,
/* 0x73 */	DI_KEY_REMOCON_TTX_REVEAL,
/* 0x74 */	DI_KEY_REMOCON_TTX_DOUBLE,
/* 0x75 */	DI_KEY_REMOCON_TTX_CANCEL,
/* 0x76 */	DI_KEY_REMOCON_TTX_INDEX,
/* 0x77 */	DI_KEY_REMOCON_TTX_SUBPAGE,
/* 0x78 */	DI_KEY_REMOCON_TTX_UPDATE,
/* 0x79 */	DI_KEY_REMOCON_TTX_MIX,

//	아래 키는 생산용리모컨에 있는 히든메뉴로 들어가는 붉은색 Short-Cut Key값에 대한 것입니다.
//	삭제하지 마세요.. 전체 공통으로 사용됨.
/* 0x7F */	DI_KEY_REMOCON_HIDDEN4MP = 0x7F,	//사용하지 않는키......


/* PVR & media keys ************************************************/
/* 0x80 */	DI_KEY_REMOCON_PLAYPAUSE= 0x80,
/* 0x81 */	DI_KEY_REMOCON_PLAY,
/* 0x82 */	DI_KEY_REMOCON_PAUSE,
/* 0x83 */	DI_KEY_REMOCON_STOP,
/* 0x84 */	DI_KEY_REMOCON_RECORD,
/* 0x85 */	DI_KEY_REMOCON_FORWARD,
/* 0x86 */	DI_KEY_REMOCON_BACKWARD,
/* 0x87 */	DI_KEY_REMOCON_PREV,
/* 0x88 */	DI_KEY_REMOCON_NEXT,
/* 0x89 */	DI_KEY_REMOCON_REPLAY,
/* 0x8A */	DI_KEY_REMOCON_SKIP,
/* 0x8B */	DI_KEY_REMOCON_ADD_BOOKMARK,
/* 0x8C */	DI_KEY_REMOCON_BOOKMARK_LIST,
/* 0x8D */	DI_KEY_REMOCON_SLOW,
/* 0x8E */	DI_KEY_REMOCON_HOME,
/* 0x8F */	DI_KEY_REMOCON_START_OVER,


/* LG specific *****************************************************/
/* 0x90 */	DI_KEY_REMOCON_QVIEW = 0x90, /* Quick view. */
/* 0x91 */	DI_KEY_REMOCON_ENERGY_GREEN,
/* 0x92 */	DI_KEY_REMOCON_PSM, /* Picture mode */
/* 0x93 */	DI_KEY_REMOCON_SSM, /* Sound mode */
/* 0x94 */	DI_KEY_REMOCON_AD, /* Audio description */

/* 부가 기능 keys. 주로 virtual keys or multi-press keys ***********/
/* 0xA0 */	DI_KEY_REMOCON_FRONT_LOCK_OFF = 0xA0, /* Front key lock off */
/* 0xA1 */	DI_KEY_REMOCON_HOTELMENU, /* Hotel menu launch */
/* 0xA2 */	DI_KEY_REMOCON_SERVICEMODE,
/* 0xA3 */	DI_KEY_REMOCON_GAME,
/* 0xA4 */	DI_KEY_REMOCON_VOD,
/* 0xA5 */	DI_KEY_REMOCON_VODPART_FORMAT,
/* 0xA6 */	DI_KEY_REMOCON_MULTI_VIEW,  /* Video track  */

/* 0xAA */	DI_KEY_REMOCON_TV_PORTAL = 0xAA,

/* PIP **********************************************/
/* 0xB0 */	DI_KEY_REMOCON_PIP_CHLIST = 0xB0,
/* 0xB1 */	DI_KEY_REMOCON_PIPDISP,
/* 0xB2 */	DI_KEY_REMOCON_PIPMOVE,
/* 0xB3 */	DI_KEY_REMOCON_PIPSWAP,

/* Special keys **********************************************/
/* 0xC0 */	DI_KEY_REMOCON_MYEPG = 0xC0,
/* 0xC1 */	DI_KEY_REMOCON_MHP,
/* 0xC2 */	DI_KEY_REMOCON_LANGUAGE,
/* 0xC3 */	DI_KEY_REMOCON_ALERT,
/* 0xC4 */	DI_KEY_REMOCON_RECALL,
/* 0xC5 */	DI_KEY_REMOCON_SYSTEM,
/* 0xC6 */	DI_KEY_REMOCON_SETTING,
/* 0xC7 */	DI_KEY_REMOCON_TIVUEPG,
/* 0xC8 */	DI_KEY_REMOCON_EPGSEARCH,
/* 0xC9 */	DI_KEY_REMOCON_FUNCTION, /* Function menu (Instant recording, PIP/TWIN, Sleep Timer)*/
/* 0xCA */	DI_KEY_REMOCON_SHIFT,    /* change the character */
/* 0xCB */	DI_KEY_REMOCON_CLEAR,	 /* Clear */
/* 0xCC */	DI_KEY_REMOCON_PROMOTION,/* move to Promotion channel */
/* 0xCD */	DI_KEY_REMOCON_NETFLIX,  /* Netflix Key */
/* 0xCE */	DI_KEY_REMOCON_SHAHID,  /* shahid Key [M1] */


/* Mode keys **********************************************/
/* 0xD0 */	DI_KEY_REMOCON_MODE_PVR = 0xD0,
/* 0xD1 */	DI_KEY_REMOCON_MODE_TV,
/* 0xD2 */	DI_KEY_REMOCON_MODE_DVD,
/* 0xD3 */	DI_KEY_REMOCON_MODE_AUDIO,

/* Function keys **********************************************/
/* 0xE0 */	DI_KEY_REMOCON_ENTER = 0xE0,	/*	ENTER				*/
/* 0xE1 */	DI_KEY_REMOCON_CAPS,			/*	Capital letters, CAPS	*/
/* 0xE2 */	DI_KEY_REMOCON_ALT,				/*	Altermate key, ALT	*/
/* 0xE3 */	DI_KEY_REMOCON_SPACE,			/*	SPACE				*/
/* 0xE4 */	DI_KEY_REMOCON_DEL,

/* PVR & media keys ************************************************/
/* 0xF0 */	DI_KEY_REMOCON_PLAYBAR = 0xF0,	/*	PLAYBAR				*/


/* Althabet, Small Letter **********************************************/
/* 0x100 */	DI_KEY_REMOCON_a = 0x100,
/* 0x101 */	DI_KEY_REMOCON_b,
/* 0x102 */	DI_KEY_REMOCON_c,
/* 0x103 */	DI_KEY_REMOCON_d,
/* 0x104 */	DI_KEY_REMOCON_e,
/* 0x105 */	DI_KEY_REMOCON_f,
/* 0x106 */	DI_KEY_REMOCON_g,
/* 0x107 */	DI_KEY_REMOCON_h,
/* 0x108 */	DI_KEY_REMOCON_i,
/* 0x109 */	DI_KEY_REMOCON_j,
/* 0x10a */	DI_KEY_REMOCON_k,
/* 0x10b */	DI_KEY_REMOCON_l,
/* 0x10c */	DI_KEY_REMOCON_m,
/* 0x10d */	DI_KEY_REMOCON_n,
/* 0x10e */	DI_KEY_REMOCON_o,
/* 0x10f */	DI_KEY_REMOCON_p,
/* 0x110 */	DI_KEY_REMOCON_q = 0x110,
/* 0x111 */	DI_KEY_REMOCON_r,
/* 0x112 */	DI_KEY_REMOCON_s,
/* 0x113 */	DI_KEY_REMOCON_t,
/* 0x114 */	DI_KEY_REMOCON_u,
/* 0x115 */	DI_KEY_REMOCON_v,
/* 0x116 */	DI_KEY_REMOCON_w,
/* 0x117 */	DI_KEY_REMOCON_x,
/* 0x118 */	DI_KEY_REMOCON_y,
/* 0x119 */	DI_KEY_REMOCON_z,

/* Alphabet, Capital **********************************************/
/* 0x120 */	DI_KEY_REMOCON_A = 0x120,
/* 0x121 */	DI_KEY_REMOCON_B,
/* 0x122 */	DI_KEY_REMOCON_C,
/* 0x123 */	DI_KEY_REMOCON_D,
/* 0x124 */	DI_KEY_REMOCON_E,
/* 0x125 */	DI_KEY_REMOCON_F,
/* 0x126 */	DI_KEY_REMOCON_G,
/* 0x127 */	DI_KEY_REMOCON_H,
/* 0x128 */	DI_KEY_REMOCON_I,
/* 0x129 */	DI_KEY_REMOCON_J,
/* 0x12a */	DI_KEY_REMOCON_K,
/* 0x12b */	DI_KEY_REMOCON_L,
/* 0x12c */	DI_KEY_REMOCON_M,
/* 0x12d */	DI_KEY_REMOCON_N,
/* 0x12e */	DI_KEY_REMOCON_O,
/* 0x12f */	DI_KEY_REMOCON_P,
/* 0x130 */	DI_KEY_REMOCON_Q = 0x130,
/* 0x131 */	DI_KEY_REMOCON_R,
/* 0x132 */	DI_KEY_REMOCON_S,
/* 0x133 */	DI_KEY_REMOCON_T,
/* 0x134 */	DI_KEY_REMOCON_U,
/* 0x135 */	DI_KEY_REMOCON_V,
/* 0x136 */	DI_KEY_REMOCON_W,
/* 0x137 */	DI_KEY_REMOCON_X,
/* 0x138 */	DI_KEY_REMOCON_Y,
/* 0x139 */	DI_KEY_REMOCON_Z,

/* Special Character **********************************************/
/* 0x140 */	DI_KEY_REMOCON_EXCLAMATION = 0x140,	/* 	! 	:	EXCLAMATION MARK		*/
/* 0x141 */	DI_KEY_REMOCON_SEMICOLON,			/* 	;	:	SEMICOLON				*/
/* 0x142 */	DI_KEY_REMOCON_SHARP,				/* 	#	:	SHARP		 			*/
/* 0x143 */	DI_KEY_REMOCON_DOLLAR_SIGN,			/* 	$	:	DOLLAR_SIGN			*/
/* 0x144 */	DI_KEY_REMOCON_PERCENT,				/* 	%	:	PERCENT SIGN			*/
/* 0x145 */	DI_KEY_REMOCON_QUOTATION,			/* 	"	:	DOUBLE QUOTATION MARK	*/
/* 0x146 */	DI_KEY_REMOCON_AMPERSAND,			/* 	& 	:	AMPERSAND				*/
/* 0x147 */	DI_KEY_REMOCON_PARENTHESES_LEFT,	/* 	(	:	PARENTHESES LEFT 		*/
/* 0x148 */	DI_KEY_REMOCON_PARENTHESES_RIGHT,	/* 	)	:	PARENTHESES RIGHT		*/
/* 0x149 */	DI_KEY_REMOCON_UNDERSCORE,			/* 	_ 	: 	UNDERSCORE				*/
/* 0x14a */	DI_KEY_REMOCON_LINE,				/* 	-	:	LINE / MINUS				*/
/* 0x14b */	DI_KEY_REMOCON_PLUS,				/* 	+ 	:	PLUS					*/
/* 0x14c */	DI_KEY_REMOCON_BRACKETS_LEFT,		/* 	[ 	:	SQUARE BRACKETS LEFT	*/
/* 0x14d */	DI_KEY_REMOCON_BRACKETS_RIGHT,		/* 	] 	:	SQUARE BRACKETS RIGHT	*/
/* 0x14e */	DI_KEY_REMOCON_ASTERISK,			/* 	* 	:	ASTERISK				*/
/* 0x14f */	DI_KEY_REMOCON_QUESTION,			/*	?	:	QUESTION MARK			*/
/* 0x150 */	DI_KEY_REMOCON_PERIOD = 0x150,		/* 	.	: 	PERIOD					*/
/* 0x151 */	DI_KEY_REMOCON_COMMA,				/*	,	: 	COMMA					*/
/* 0x152 */	DI_KEY_REMOCON_AT,					/* 	@	:	AT						*/

/* JAPAN **********************************************/
/* 0x1000 */	DI_KEY_REMOCON_DT = 0x1000,
/* 0x1001 */	DI_KEY_REMOCON_BS,
/* 0x1002 */	DI_KEY_REMOCON_CATV,
/* 0x1003 */	DI_KEY_REMOCON_NET,
/* 0x1004 */	DI_KEY_REMOCON_VID_AUD,
/* 0x1005 */	DI_KEY_REMOCON_DATA,
/* 0x1006 */	DI_KEY_REMOCON_DETAIL_INFO,	/* Detail information */

/* DIGITURK **********************************************/
/* 0x1100 */	DI_KEY_REMOCON_DIGITURK_ARCH = 0x1100,
/* 0x1101 */	DI_KEY_REMOCON_DIGITURK_RECORD_TIMER,		/* RECORD TIMER */

/* OTTV **********************************************/
/* 0x1200 */	DI_KEY_REMOCON_OTTV_ONDEMAND = 0x1200,

/* TDC ***********************************************/
/* 0x1300 */	DI_KEY_REMOCON_SKIP_BACKWARD = 0x1300,
/* 0x1301 */	DI_KEY_REMOCON_SKIP_FORWARD = 0x1301,


/* 0xFFFFFFFF */	DI_KEY_REMOCON_UNKNOWN = 0x7FFFFFFF
} DI_KEY_REMOCON;

typedef enum
{
	DI_KEY_FRONT_VOL_DOWN 				= 0x00000001,
	DI_KEY_FRONT_VOL_UP					= 0x00000002,
	DI_KEY_FRONT_CH_DOWN				= 0x00000004,
	DI_KEY_FRONT_CH_UP					= 0x00000008,
	DI_KEY_FRONT_MENU					= 0x00000010,
	DI_KEY_FRONT_INPUT					= 0x00000020,
	DI_KEY_FRONT_PWR					= 0x00000040,
	DI_KEY_FRONT_OK						= 0x00000080,
	DI_KEY_FRONT_DVD_PLAY_PAUSE			= 0x00000100,
	DI_KEY_FRONT_DVD_STOP				= 0x00000200,
	DI_KEY_FRONT_DVD_OPEN_CLOSE			= 0x00000400,
	DI_KEY_FRONT_TV_RADIO				= 0x00000800,
	DI_KEY_FRONT_UP						= 0x00001000,
	DI_KEY_FRONT_DOWN					= 0x00002000,
	DI_KEY_FRONT_LEFT					= 0x00004000,
	DI_KEY_FRONT_RIGHT					= 0x00008000,
	DI_KEY_FRONT_GUIDE					= 0x00010000,
	DI_KEY_FRONT_BACK					= 0x00020000,
	DI_KEY_FRONT_DETAIL_INFO			= 0x00040000,
	DI_KEY_FRONT_HIDDEN					= 0x00080000, /* jc3500, for hidden menu key */
	DI_KEY_FRONT_WPS					= 0x00100000, /* WiFi WPS */
	DI_KEY_FRONT_NETWORK				= 0x00200000, /* Broadcase Network */
	DI_KEY_FRONT_SW_REBOOT			= 0x00400000,
	DI_KEY_FRONT_RESERVED_01			= 0x00800000,
	DI_KEY_FRONT_RESERVED_02			= 0x01000000,
	DI_KEY_FRONT_RESERVED_03			= 0x02000000,
	DI_KEY_FRONT_RESERVED_04			= 0x04000000,
	DI_KEY_FRONT_RESERVED_05			= 0x08000000,
	DI_KEY_FRONT_RESERVED_06			= 0x10000000,
	DI_KEY_FRONT_RESERVED_07			= 0x20000000,
	DI_KEY_FRONT_MAX					= 0x40000000
} DI_KEY_FRONT ;

typedef enum
{
	DI_KEY_WAKEUP_KEY_TYPE_NUMBER,				// rcu number key = 0x03 ~ 0x0C
	DI_KEY_WAKEUP_KEY_TYPE_CH_UP_DOWN,			// rcu CH up/down key = 0x0F, 0x10
	DI_KEY_WAKEUP_KEY_TYPE_ARROW,				// rcu ARROW + OK key = 0x11 ~ 0x15
	DI_KEY_WAKEUP_KEY_TYPE_SINGLE,				// rcu any single key
	DI_KEY_WAKEUP_KEY_TYPE_MAX
} DI_KEY_WAKEUP_KEY_TYPE ;

// End global variable

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

typedef HINT32 (* pfnDI_KEY_Notify)(DI_KEY_DEVICE eDevice, HUINT32 ulKeyCode, DI_KEY_TYPE eKeyType);
typedef int (* KeyDebug) (const char *format, ...) ;

// End typedef

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee


// End static variable


/*******************************************************************/
/*********************** Function Prototype ************************/
/*******************************************************************/
// Start Function Prototype

void DI_KEY_MultiCustomCode_RegisterCallBack(void (*NotifyFunc)(HUINT8 msgType, HUINT8	mode));
void DI_KEY_RegisterKeyEventCallback (pfnDI_KEY_Notify pfnNotify);
void DI_KEY_RegisterKeyPreProcessingCallback(pfnDI_KEY_Notify pfnNotify);
DI_ERR_CODE DI_KEY_GetCapability (void *pCapability) ;
DI_ERR_CODE DI_KEY_SetKeyRepeatTime (HUINT32 ulRepeatTime) ;
DI_ERR_CODE DI_KEY_GetKeyRepeatTime (HUINT32 *ulRepeatTime) ;
DI_ERR_CODE DI_KEY_SetKeyHoldTime (HUINT32 ulHoldTime) ;
DI_ERR_CODE DI_KEY_GetKeyHoldTime (HUINT32 *ulHoldTime) ;
DI_ERR_CODE DI_KEY_SetWakeupRcuKey(DI_KEY_WAKEUP_KEY_TYPE nKeyType, DI_KEY_REMOCON nRcuKey);
DI_ERR_CODE DI_KEY_GetWakeupRcuKey(HUINT8 *pucKeyCode);
DI_ERR_CODE DI_KEY_GetCurrentCustomCode(HUINT16 *pCustomCode);
DI_ERR_CODE DI_KEY_ClearWakeupRcuKey(void);
HUINT32 DI_KEY_ConvertDecodeToKey(HUINT8 ucDecodedCode);
DI_ERR_CODE DI_KEY_GetTouchKeyThreshold(HUINT8 *pucThreshold, HUINT8 *KeyNum);
DI_ERR_CODE DI_KEY_SetTouchKeyThreshold(HUINT8 *pucThreshold, HUINT8 KeyNum);
HBOOL DI_KEY_IsPressedFrontKey(DI_KEY_FRONT key);

// End Function Prototype

#endif /* __DI_KEY_H__ */

