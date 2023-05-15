/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  key_params.h
	@brief	  octo system wide RCU/Front key definition

	Description: commonly used enum values on ap, mw, pal layers of octo are defined

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__KEY_PARAMS_H__
#define	__KEY_PARAMS_H__

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*
	필독!
	여기에 정의된 키들은 virtual key로 실제 지원되지 않는 키들도 포함된 super set key list 입니다.
	제품별로는 실제 key code에 아래 키들중 지원되는 키들이 mapping 되어 있고, application으로는 아래 키들이 전송됩니다.
	그러므로 application은 실제 키는 상관하지 말고 아래 virtual key에 대한 기능을 구현하면 되고, 지원하지 않는 기능들은
	물리적으로 key가 날아오지 않기 때문에 단지 기능을 안할 뿐입니다.
	실제 키의 이름과 다를 수도 있으므로 실제 RCU에 프린트된 이름에 집착하지 말고, 조금 불편하더라도 아래 키들 중에서
	원하는 기능키가 있는지 먼저 찾아보고, 없으면 추가하는 식으로 key set을 늘려나가야 합니다.
	실제 키와 기능키가 다르거나 두가지 기능이 하나의 키를 공유하는 경우에는 제품별로 있는 ap_keyconv_xxx.c의
	key remapping 함수를 정의하여 사용하고 application body에서는 다음 키들을 기준으로 coding 하여 주시면 감사하겠습니다.
*/
#if 0 // used ondk_keytype.h
typedef enum
{
/*******************************************************************/
/* RCU Keys                                                        */
/*******************************************************************/

/* 0x0001 */	KEY_STANDBY = 0x0001,			/* Garbage 로 0 들어가지 않도록... */

/* Numeric keys ****************************************************/
/* 0x0010 */	KEY_0 = 0x0010,
/* 0x0011 */	KEY_1,
/* 0x0012 */	KEY_2,
/* 0x0013 */	KEY_3,
/* 0x0014 */	KEY_4,
/* 0x0015 */	KEY_5,
/* 0x0016 */	KEY_6,
/* 0x0017 */	KEY_7,
/* 0x0018 */	KEY_8,
/* 0x0019 */	KEY_9,
/* 0x001A */	KEY_11,
/* 0x001B */	KEY_12,

/* Color keys ******************************************************/
/* 0x0020 */	KEY_RED = 0x0020,
/* 0x0021 */	KEY_GREEN,
/* 0x0022 */	KEY_YELLOW,
/* 0x0023 */	KEY_BLUE,

/* UI menu 관련 keys ***********************************************/
/* 0x0030 */	KEY_INFO = 0x0030,				/* i-plate or other EPG use. */
/* 0x0031 */	KEY_GUIDE,						/* EPG launch. 다른 이름 : EPG, ... */
/* 0x0032 */	KEY_MENU,						/* Portal launch. 다른 이름 : POTAL , KEY_HUB*/
/* 0x0033 */	KEY_QMENU,						/* Quick menu launch. */
/* 0x0034 */	KEY_FAVORITE,					/* Favorite channel function. */
/* 0x0035 */	KEY_CHLIST,						/* Channel list launching. */
/* 0x0036 */	KEY_SETUP,						/* Setup menu launching. */
/* 0x0037 */	KEY_VFORMAT,					/* Current resolution information. */
/* 0x0038 */	KEY_AUDIO,						/* Audio track or dual-mono selection. */
/* 0x0039 */	KEY_SUBTITLE,					/* Subtitle selection. */
/* 0x003A */	KEY_SLEEP,						/* Sleep timer function. */
/* 0x003B */	KEY_OPT,						/* Option + and KEY_PLUS*/
/* 0x003C */	KEY_SCHEDULE,					/* 예약 관련 기능. 예약 리스트 등 ... */
/* 0x003D */	KEY_MEDIA,						/* Media direct launch or other media related... */
/* 0x003E */	KEY_IP_BROWSER,					/* for web-browser */
/* 0x003F */	KEY_SELECT,						/* for SKY D spec */
/* 0x0040 */	KEY_RESERVATION,				/* 예약 화면 */
/* 0x0041 */	KEY_DLNA,						/* DLNA */
/* 0x0042 */	KEY_HDD,						/* HDD */
/* 0x0043 */	KEY_VOD,						/* VOD */
/* 0x0044 */ KEY_DISP_ON,					/* Display On */
/* 0x0045 */ KEY_HOTKEY,						/* Hot Key, One Touch, etc */
/* 0x0047 */ KEY_SMART_SEARCH = 0x0047,
/* 0x0048 */ KEY_AUDIO_LANGUAGE,
/* Menu Navigation keys ********************************************/
/* 0x0050 */	KEY_PAGEUP = 0x0050,
/* 0x0051 */	KEY_PAGEDOWN,
/* 0x0052 */	KEY_ARROWUP,
/* 0x0053 */	KEY_ARROWDOWN,
/* 0x0054 */	KEY_ARROWLEFT,
/* 0x0055 */	KEY_ARROWRIGHT,
/* 0x0056 */	KEY_BACK,						/* Previous menu or Last service tuning. */
/* 0x0057 */	KEY_EXIT,						/* Clear current UI and goto TV watching. */
/* 0x0058 */	KEY_OK,
/* 0x0059 */	KEY_ERASE,

/* Channel change keys *********************************************/
/* 0x0060 */	KEY_CH_PLUS = 0x0060,
/* 0x0061 */	KEY_CH_MINUS,
/* 0x0062 */	KEY_LAST,						/* Last channel tuning. */
/* 0x0063 */	KEY_TVRADIO,					/* TV <-> Radio service change. */
/* 0x0064 */	KEY_COMBO,						/* Delivery type or Network select. 다른 이름 : TV/DTV(JVC) */
/* 0x0065 */	KEY_FREESAT,					/* Freesat selection */
/* 0x0066 */	KEY_CH_INPUT,					/* Direct Channel Input */
/* 0x0067 */	KEY_NETWORK_TER,				/* Terrestrial */
/* 0x0068 */	KEY_NETWORK_BS, 				/* BS */
/* 0x0069 */	KEY_NETWORK_CATV,				/* CATV */
/* 0x006A */	KEY_DATA,						/* Data Channel, BML start */

/* AV signal 및 관련 UI keys ***************************************/
/* 0x0070 */	KEY_VOLUMEUP = 0x0070,
/* 0x0071 */	KEY_VOLUMEDOWN,
/* 0x0072 */	KEY_MUTE,
/* 0x0073 */	KEY_EXT_AV,						/* DTV/ATV/EXT launch or selection. */
/* 0x0074 */	KEY_EXT_AV_PLUS,				/* DTV/ATV/EXT launch or selection. */
/* 0x0075 */	KEY_EXT_AV_MINUS,				/* DTV/ATV/EXT launch or selection. */
/* 0x0076 */	KEY_AVMODE,						/* AV mode launch or selection. */
/* 0x0077 */	KEY_FREEZE,						/* Video freeze. */
/* 0x0078 */	KEY_WIDE,						/* Aspect ratio 변경 키. 다른 이름 : ZOOM(JVC) */
/* 0x0079 */	KEY_3D,							/* 3D surrround 모드 변경 키. 다른 이름 : SURROUND */
/* 0x007A */	KEY_SOURCE,						/* STB에서만 사용하는 decoder/SCART 출력 전환 키. KEY_EXT_AV랑 다름. */
/* 0x007B */	KEY_MHP,						/* ITALY : MHP Enable Key for RMG08 & RME09*/
/* 0x007C */	KEY_MYEPG,						/* ITALY : MYEPG Enable Key for RMG08 */
/* 0x007D */	KEY_TIVUEPG,					/* ITALY :TIVUEPG Enable Key for RMG08 & RME09*/
/* 0x007E */	KEY_OTTV,						/* ITALY : OTTV execution Key for RMG08 */

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

/* 0xFFFF */	REMOTEAPP_KEY_UNKNOWN = 0x00FFFF
} KEY_Code_t;
#else

#define NELEM(x)  ((int)(sizeof((x)) / sizeof((x)[0])))

typedef enum
{
/*******************************************************************/
/* RCU Keys                                                        */
/*******************************************************************/

/* 0x0001 */	REMOTEAPP_KEY_STANDBY = 0x0001,			/* Garbage 로 0 들어가지 않도록... */

/* Numeric keys ****************************************************/
/* 0x0010 */	REMOTEAPP_KEY_0 = 0x0010,
/* 0x0011 */	REMOTEAPP_KEY_1,
/* 0x0012 */	REMOTEAPP_KEY_2,
/* 0x0013 */	REMOTEAPP_KEY_3,
/* 0x0014 */	REMOTEAPP_KEY_4,
/* 0x0015 */	REMOTEAPP_KEY_5,
/* 0x0016 */	REMOTEAPP_KEY_6,
/* 0x0017 */	REMOTEAPP_KEY_7,
/* 0x0018 */	REMOTEAPP_KEY_8,
/* 0x0019 */	REMOTEAPP_KEY_9,
/* 0x001A */	REMOTEAPP_KEY_11,
/* 0x001B */	REMOTEAPP_KEY_12,

/* Color keys ******************************************************/
/* 0x0020 */	REMOTEAPP_KEY_RED = 0x0020,

/* 0x0021 */	REMOTEAPP_KEY_GREEN,
/* 0x0022 */	REMOTEAPP_KEY_YELLOW,
/* 0x0023 */	REMOTEAPP_KEY_BLUE,

/* UI menu 관련 keys ***********************************************/
/* 0x0030 */	REMOTEAPP_KEY_INFO = 0x0030,				/* i-plate or other EPG use. */
/* 0x0031 */	REMOTEAPP_KEY_GUIDE,						/* EPG launch. 다른 이름 : EPG, ... */
/* 0x0032 */	REMOTEAPP_KEY_MENU,						/* Portal launch. 다른 이름 : POTAL , KEY_HUB*/
/* 0x0033 */	REMOTEAPP_KEY_QMENU,						/* Quick menu launch. */
/* 0x0034 */	REMOTEAPP_KEY_FAVORITE,					/* Favorite channel function. */
/* 0x0035 */	REMOTEAPP_KEY_CHLIST,						/* Channel list launching. */
/* 0x0036 */	REMOTEAPP_KEY_SETUP,						/* Setup menu launching. */
/* 0x0037 */	REMOTEAPP_KEY_VFORMAT,					/* Current resolution information. */
/* 0x0038 */	REMOTEAPP_KEY_AUDIO,						/* Audio track or dual-mono selection. */
/* 0x0039 */	REMOTEAPP_KEY_SUBTITLE,					/* Subtitle selection. */
/* 0x003A */	REMOTEAPP_KEY_SLEEP,						/* Sleep timer function. */
/* 0x003B */	REMOTEAPP_KEY_OPT,						/* Option + and KEY_PLUS*/
/* 0x003C */	REMOTEAPP_KEY_SCHEDULE,					/* 예약 관련 기능. 예약 리스트 등 ... */
/* 0x003D */	REMOTEAPP_KEY_MEDIA,						/* Media direct launch or other media related... */
/* 0x003E */	REMOTEAPP_KEY_IP_BROWSER,					/* for web-browser */
/* 0x003F */	REMOTEAPP_KEY_SELECT,						/* for SKY D spec */
/* 0x0040 */	REMOTEAPP_KEY_RESERVATION,				/* 예약 화면 */
/* 0x0041 */	REMOTEAPP_KEY_DLNA,						/* DLNA */
/* 0x0042 */	REMOTEAPP_KEY_HDD,						/* HDD */
/* 0x0043 */	REMOTEAPP_KEY_VOD,						/* VOD */
/* 0x0044 */  REMOTEAPP_KEY_DISP_ON,					/* Display On */
/* 0x0045 */  REMOTEAPP_KEY_HOTKEY,						/* Hot Key, One Touch, etc */
/* 0x0047 */  REMOTEAPP_KEY_SMART_SEARCH = 0x0047,
/* 0x0048 */  REMOTEAPP_KEY_AUDIO_LANGUAGE,
/* Menu Navigation keys ********************************************/
/* 0x0050 */	REMOTEAPP_KEY_PAGEUP = 0x0050,
/* 0x0051 */	REMOTEAPP_KEY_PAGEDOWN,
/* 0x0052 */	REMOTEAPP_KEY_ARROWLEFT,
/* 0x0053 */	REMOTEAPP_KEY_ARROWRIGHT,
/* 0x0054 */	REMOTEAPP_KEY_ARROWUP,
/* 0x0055 */	REMOTEAPP_KEY_ARROWDOWN,
/* 0x0056 */	REMOTEAPP_KEY_BACK,						/* Previous menu or Last service tuning. */
/* 0x0057 */	REMOTEAPP_KEY_EXIT,						/* Clear current UI and goto TV watching. */
/* 0x0058 */	REMOTEAPP_KEY_OK,
/* 0x0059 */	REMOTEAPP_KEY_ERASE,
/* Channel change keys *********************************************/
/* 0x0060 */	REMOTEAPP_KEY_CH_PLUS = 0x0060,
/* 0x0061 */	REMOTEAPP_KEY_CH_MINUS,
/* 0x0062 */	REMOTEAPP_KEY_LAST,						/* Last channel tuning. */
/* 0x0063 */	REMOTEAPP_KEY_TVRADIO,					/* TV <-> Radio service change. */
/* 0x0064 */	REMOTEAPP_KEY_COMBO,						/* Delivery type or Network select. 다른 이름 : TV/DTV(JVC) */
/* 0x0065 */	REMOTEAPP_KEY_FREESAT,					/* Freesat selection */
/* 0x0066 */	REMOTEAPP_KEY_CH_INPUT,					/* Direct Channel Input */
/* 0x0067 */	REMOTEAPP_KEY_NETWORK_TER,				/* Terrestrial */
/* 0x0068 */	REMOTEAPP_KEY_NETWORK_BS, 				/* BS */
/* 0x0069 */	REMOTEAPP_KEY_NETWORK_CATV,				/* CATV */
/* 0x006A */	REMOTEAPP_KEY_DATA,						/* Data Channel, BML start */

/* AV signal 및 관련 UI keys ***************************************/
/* 0x0070 */	REMOTEAPP_KEY_VOLUMEUP = 0x0070,
/* 0x0071 */	REMOTEAPP_KEY_VOLUMEDOWN,
/* 0x0072 */	REMOTEAPP_KEY_MUTE,
/* 0x0073 */	REMOTEAPP_KEY_EXT_AV,						/* DTV/ATV/EXT launch or selection. */
/* 0x0074 */	REMOTEAPP_KEY_EXT_AV_PLUS,				/* DTV/ATV/EXT launch or selection. */
/* 0x0075 */	REMOTEAPP_KEY_EXT_AV_MINUS,				/* DTV/ATV/EXT launch or selection. */
/* 0x0076 */	REMOTEAPP_KEY_AVMODE,						/* AV mode launch or selection. */
/* 0x0077 */	REMOTEAPP_KEY_FREEZE,						/* Video freeze. */
/* 0x0078 */	REMOTEAPP_KEY_WIDE,						/* Aspect ratio 변경 키. 다른 이름 : ZOOM(JVC) */
/* 0x0079 */	REMOTEAPP_KEY_3D,							/* 3D surrround 모드 변경 키. 다른 이름 : SURROUND */
/* 0x007A */	REMOTEAPP_KEY_SOURCE,						/* STB에서만 사용하는 decoder/SCART 출력 전환 키. KEY_EXT_AV랑 다름. */
/* 0x007B */	REMOTEAPP_KEY_MHP,						/* ITALY : MHP Enable Key for RMG08 & RME09*/
/* 0x007C */	REMOTEAPP_KEY_MYEPG,						/* ITALY : MYEPG Enable Key for RMG08 */
/* 0x007D */	REMOTEAPP_KEY_TIVUEPG,					/* ITALY :TIVUEPG Enable Key for RMG08 & RME09*/
/* 0x007E */	REMOTEAPP_KEY_OTTV,						/* ITALY : OTTV execution Key for RMG08 */

/* Teletext 관련 keys **********************************************/
/* 0x0080 */	REMOTEAPP_KEY_TEXT = 0x0080,				/* Teletext or MHEG launch. 다른 이름 : DATA, MHEG, ... */
/* 0x0081 */	REMOTEAPP_KEY_TEXT_OPTION,				/* Teletext menu launching */
/* 0x0082 */	REMOTEAPP_KEY_TTX_HOLD,
/* 0x0083 */	REMOTEAPP_KEY_TTX_REVEAL,
/* 0x0084 */	REMOTEAPP_KEY_TTX_DOUBLE,
/* 0x0085 */	REMOTEAPP_KEY_TTX_CANCEL,
/* 0x0086 */	REMOTEAPP_KEY_TTX_INDEX,
/* 0x0087 */	REMOTEAPP_KEY_TTX_SUBPAGE,
/* 0x0088 */	REMOTEAPP_KEY_TTX_UPDATE,

/* PVR & media keys ************************************************/
/* 0x0090 */	REMOTEAPP_KEY_PLAY = 0x0090,
/* 0x0091 */	REMOTEAPP_KEY_PAUSE,
/* 0x0092 */	REMOTEAPP_KEY_STOP,
/* 0x0093 */	REMOTEAPP_KEY_RECORD,
/* 0x0094 */	REMOTEAPP_KEY_FORWARD,
/* 0x0095 */	REMOTEAPP_KEY_BACKWARD,
/* 0x0096 */	REMOTEAPP_KEY_PREV,
/* 0x0097 */	REMOTEAPP_KEY_NEXT,
/* 0x0098 */	REMOTEAPP_KEY_REPLAY,
/* 0x0099 */	REMOTEAPP_KEY_SKIP,
/* 0x009A */	REMOTEAPP_KEY_ADD_BOOKMARK,
/* 0x009B */	REMOTEAPP_KEY_BOOKMARK_LIST,
/* 0x009C */	REMOTEAPP_KEY_SLOW,
/* 0x009D */	REMOTEAPP_KEY_TSR,

/* OP Specific Keys *****************************************************/
/* LG specific *****************************************************/
/* 0x00A0 */	REMOTEAPP_KEY_QVIEW = 0x00A0,				/* Quick view. */
/* 0x00A1 */	REMOTEAPP_KEY_ENERGY_GREEN,
/* 0x00A2 */	REMOTEAPP_KEY_PSM,						/* Picture mode */
/* 0x00A3 */	REMOTEAPP_KEY_SSM,						/* Sound mode */
/* 0x00A4 */	REMOTEAPP_KEY_AD,							/* Audio description */

/* Truevision specific  *******************************************/
/* 0x00A5 */	REMOTEAPP_KEY_ALERT,
/* 0x00A6 */	REMOTEAPP_KEY_RECALL,
/* 0x00A7 */	REMOTEAPP_KEY_SYSTEM,
/* 0x00A8 */	REMOTEAPP_KEY_SETTING,

/* Digiturk Spec key ********************************************/
/* 0x00A9 */	REMOTEAPP_KEY_OP_MODE,

/* AlMajd Spec key ********************************************/
/* 0x00AA */	REMOTEAPP_KEY_WELCOM,
/* 0x00AB */	REMOTEAPP_KEY_MAIL,

/* Test  key ********************************************/
/* 0x00B2 */	REMOTEAPP_KEY_PIP_MOVE,

/* 부가 기능 keys. 주로 virtual keys or multi-press keys ***********/
/* 0x00C0 */	REMOTEAPP_KEY_HOTELMENU,					/* Hotel menu launch */
/* 0x00C1 */	REMOTEAPP_KEY_SERVICEMODE,
/* 0x00C2 */	REMOTEAPP_KEY_LANGUAGE,
/* 0x00CA */	REMOTEAPP_KEY_SHIFT,
/* 0x00CB */	REMOTEAPP_KEY_CLEAR,

/* DVD/BD Keys  *******************************************/
/* 0x00D0 */	REMOTEAPP_KEY_DISC = 0x00D0,
/* 0x00D1 */	REMOTEAPP_KEY_TRAY_OPEN,
/* 0x00D2 */	REMOTEAPP_KEY_DISC_MENU,
/* 0x00D3 */	REMOTEAPP_KEY_DUB,

/* Mode  key ********************************************/
/* 0x00E0 */	REMOTEAPP_KEY_MODE_PVR = 0x00E0,
/* 0x00E1 */	REMOTEAPP_KEY_MODE_TV,
/* 0x00E2 */	REMOTEAPP_KEY_MODE_DVD,
/* 0x00E3 */	REMOTEAPP_KEY_MODE_AUDIO,
/* 0x00E4 */	REMOTEAPP_KEY_MODE_WARNING,


/*******************************************************************/
/* FRONT Keys                                                      */
/*******************************************************************/

/* 0x0800 */	REMOTEAPP_KEY_FRONT_STANDBY = 0x0800,
/* 0x0801 */	REMOTEAPP_KEY_FRONT_EXT_AV,
/* 0x0802 */	REMOTEAPP_KEY_FRONT_MENU,
/* 0x0803 */	REMOTEAPP_KEY_FRONT_CH_PLUS,
/* 0x0804 */	REMOTEAPP_KEY_FRONT_CH_MINUS,
/* 0x0805 */	REMOTEAPP_KEY_FRONT_VOLUME_UP,
/* 0x0806 */	REMOTEAPP_KEY_FRONT_VOLUME_DOWN,
/* 0x0807 */	REMOTEAPP_KEY_FRONT_OK,
/* 0x0808 */	REMOTEAPP_KEY_FRONT_ARROW_UP,
/* 0x0809 */	REMOTEAPP_KEY_FRONT_ARROW_DOWN,
/* 0x080A */	REMOTEAPP_KEY_FRONT_ARROW_LEFT,
/* 0x080B */	REMOTEAPP_KEY_FRONT_ARROW_RIGHT,
/* 0x080C */	REMOTEAPP_KEY_FRONT_TVRADIO,
/* 0x080D */	REMOTEAPP_KEY_FRONT_GUIDE,
/* 0x080E */	REMOTEAPP_KEY_FRONT_BACK,
/* 0x080F */	REMOTEAPP_KEY_FRONT_INFO,

/* DVD Combo Front Keys  *******************************************/
/* 0x0810 */	REMOTEAPP_KEY_FRONT_STOP = 0x0810,		/* DVD Stop */
/* 0x0811 */	REMOTEAPP_KEY_FRONT_PLAY_PAUSE,			/* DVD Play/Pause */
/* 0x0812 */	REMOTEAPP_KEY_FRONT_OPEN_CLOSE,			/* DVD Open/Close */
/* 0x0813 */	REMOTEAPP_KEY_FRONT_HDD_DISC,				/* HDD/DISC(STB/DISC swap) */
/* 0x0814 */	REMOTEAPP_KEY_FRONT_TER_BS_CATV,			/* Network Ter, BS, CATV */
/* 0x0815 */	REMOTEAPP_KEY_FRONT_PLAY_LIST,				/* Paly List */
/* 0x0816 */	REMOTEAPP_KEY_FRONT_RECORD,				/* record */

/* 부가 기능 keys. 주로 virtual keys or multi-press keys ***********/
/* 0x0880 */	REMOTEAPP_KEY_FRONT_LOCK_OFF = 0x0880,	/* Front key lock off */
/* 0x0881 */	REMOTEAPP_KEY_FRONT_HIDDEN,				/* into Hidden multi key ( HDD/DISC + Play ) */

/* 0xFFFF */	REMOTEAPP_KEY_UNKNOWN = 0x00FFFF
} KEY_Code_t;
#endif	 /* end if 0 */

#endif	/* __KEY_PARAMS_H__*/
