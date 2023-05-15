
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

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <trauma_int.h>
#include <directfb.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
//1 기존 Splunk 에 보냈던데이터들와 포맷을 맞추기 위해 아래와 같이..
//1 corsair 의 일부 코드를 가져옮...............


#define	TRAUMA_GOGI_SYMBOL(diks, gogi) \
	case	diks: \
	eRetKey = (TRAUMA_GOGI_VIRTUAL_KEY_e)gogi; \
	break

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
// gogi_opera_api.h ...
typedef enum
{
	GOGI_FIRST_KEY = 0,

	GOGI_KEY_CANCEL = GOGI_FIRST_KEY + 0x3,
	GOGI_KEY_BACKSPACE = GOGI_FIRST_KEY + 0x8,
	GOGI_KEY_TAB,
	GOGI_KEY_CLEAR = GOGI_FIRST_KEY + 0xc,
	GOGI_KEY_ENTER,
	GOGI_KEY_SHIFT = GOGI_FIRST_KEY + 0x10,
	GOGI_KEY_CTRL,
	GOGI_KEY_ALT,
	GOGI_KEY_PAUSE,
	GOGI_KEY_CAPS_LOCK,
	GOGI_KEY_KANA = GOGI_FIRST_KEY + 0x15,
	GOGI_KEY_FINAL = GOGI_FIRST_KEY + 0x18,
	GOGI_KEY_KANJI = GOGI_FIRST_KEY + 0x19,
	GOGI_KEY_ESCAPE = GOGI_FIRST_KEY + 0x1b,
	GOGI_KEY_CONVERT,
	GOGI_KEY_NONCONVERT,
	GOGI_KEY_ACCEPT,
	GOGI_KEY_MODECHANGE,
	GOGI_KEY_SPACE,
	GOGI_KEY_PAGEUP,
	GOGI_KEY_PAGEDOWN,
	GOGI_KEY_END,
	GOGI_KEY_HOME,
	GOGI_KEY_LEFT,
	GOGI_KEY_UP,
	GOGI_KEY_RIGHT,
	GOGI_KEY_DOWN,
	GOGI_KEY_SELECT,
	GOGI_KEY_PRINT,
	GOGI_KEY_EXECUTE,
	GOGI_KEY_PRINTSCREEN,
	GOGI_KEY_INSERT,
	GOGI_KEY_DELETE,
	GOGI_KEY_HELP,
	GOGI_KEY_0 = GOGI_FIRST_KEY + 0x30,
	GOGI_KEY_1,
	GOGI_KEY_2,
	GOGI_KEY_3,
	GOGI_KEY_4,
	GOGI_KEY_5,
	GOGI_KEY_6,
	GOGI_KEY_7,
	GOGI_KEY_8,
	GOGI_KEY_9,
	/* 0x3a - 0x40: unassigned. */
	GOGI_KEY_A = GOGI_FIRST_KEY + 0x41,
	GOGI_KEY_B,
	GOGI_KEY_C,
	GOGI_KEY_D,
	GOGI_KEY_E,
	GOGI_KEY_F,
	GOGI_KEY_G,
	GOGI_KEY_H,
	GOGI_KEY_I,
	GOGI_KEY_J,
	GOGI_KEY_K,
	GOGI_KEY_L,
	GOGI_KEY_M,
	GOGI_KEY_N,
	GOGI_KEY_O,
	GOGI_KEY_P,
	GOGI_KEY_Q,
	GOGI_KEY_R,
	GOGI_KEY_S,
	GOGI_KEY_T,
	GOGI_KEY_U,
	GOGI_KEY_V,
	GOGI_KEY_W,
	GOGI_KEY_X,
	GOGI_KEY_Y,
	GOGI_KEY_Z,
	/* 0x41 - 0x5a: letters A-Z. */
	GOGI_KEY_WINDOW = GOGI_FIRST_KEY + 0x5b,
	GOGI_KEY_MAC_CTRL = GOGI_FIRST_KEY + 0x5c,
	/* 0x5e: reserved. */
	GOGI_KEY_CONTEXT_MENU = GOGI_FIRST_KEY + 0x5d,
	GOGI_KEY_SLEEP = GOGI_FIRST_KEY + 0x5f,
	GOGI_KEY_NUMPAD0,
	GOGI_KEY_NUMPAD1,
	GOGI_KEY_NUMPAD2,
	GOGI_KEY_NUMPAD3,
	GOGI_KEY_NUMPAD4,
	GOGI_KEY_NUMPAD5,
	GOGI_KEY_NUMPAD6,
	GOGI_KEY_NUMPAD7,
	GOGI_KEY_NUMPAD8,
	GOGI_KEY_NUMPAD9,
	/* The "*" on the numeric pad. */
	GOGI_KEY_MULTIPLY,
	/* The "+" on the numeric pad. */
	GOGI_KEY_ADD,
	/* The "." on the numeric pad. */
	GOGI_KEY_SEPARATOR,
	/* The "-" on the numeric pad. */
	GOGI_KEY_SUBTRACT,
	/* The "," on the numeric pad. */
	GOGI_KEY_DECIMAL,
	/* The "/" on the numeric pad. */
	GOGI_KEY_DIVIDE,

	GOGI_KEY_F1,
	GOGI_KEY_F2,
	GOGI_KEY_F3,
	GOGI_KEY_F4,
	GOGI_KEY_F5,
	GOGI_KEY_F6,
	GOGI_KEY_F7,
	GOGI_KEY_F8,
	GOGI_KEY_F9,
	GOGI_KEY_F10,
	GOGI_KEY_F11,
	GOGI_KEY_F12,
	GOGI_KEY_F13,
	GOGI_KEY_F14,
	GOGI_KEY_F15,
	GOGI_KEY_F16,
	GOGI_KEY_F17,
	GOGI_KEY_F18,
	GOGI_KEY_F19,
	GOGI_KEY_F20,
	GOGI_KEY_F21,
	GOGI_KEY_F22,
	GOGI_KEY_F23,
	GOGI_KEY_F24,
	/* 0x88 - 0x8f: unassigned. */
	GOGI_KEY_NUM_LOCK = GOGI_FIRST_KEY + 0x90,
	GOGI_KEY_SCROLL_LOCK,
	/* 0x92 - 0x96: OEM specific. */
	/* NOTE: 0x97 is unassigned, but appropriated by Opera
	   to represent Meta as it doesn't have an Windows virtual
	   keycode equivalent. */
	GOGI_KEY_META = GOGI_FIRST_KEY + 0x97,
	/* 0x98 - 0x9f: unassigned. */
	/* 0xa0 - 0xb7: unmapped. */
	/* 0xb8 - 0xb9: reserved. */

	/* Block of "OEM keys" that keyboard layouts map to a variety of
	   keys, punctuation mostly. Comments next to them make the
	   correspondence with what they're mapped to on a 101-key
	   US keyboard (when mapped.) */

	/* The ";:" key */
	GOGI_KEY_OEM_1 = GOGI_FIRST_KEY + 0xba,
	GOGI_KEY_COLON = GOGI_KEY_OEM_1,
	/* The "=+" key */
	GOGI_KEY_OEM_PLUS,
	/* The ",<" key */
	GOGI_KEY_OEM_COMMA,
	/* The "-_" key */
	GOGI_KEY_OEM_MINUS,
	/* The ".>" key */
	GOGI_KEY_OEM_PERIOD,
	/* The "/?" key */
	GOGI_KEY_OEM_2,
	GOGI_KEY_QUESTION = GOGI_KEY_OEM_2,
	/* The "`~" key */
	GOGI_KEY_OEM_3,
	GOGI_KEY_TILDE = GOGI_KEY_OEM_3,
	/* 0xc1 - 0xd7: reserved. */
	/* 0xd8 - 0xda: unassigned. */
	/* The "[{" key */
	GOGI_KEY_OEM_4 = GOGI_FIRST_KEY + 0xdb,
	GOGI_KEY_OPEN_BRACKET = GOGI_KEY_OEM_4,
	/* The "\\|" key */
	GOGI_KEY_OEM_5,
	GOGI_KEY_PIPE = GOGI_KEY_OEM_5,
	/* The "]}" key */
	GOGI_KEY_OEM_6,
	GOGI_KEY_CLOSE_BRACKET = GOGI_KEY_OEM_6,
	/* The "'\"" key */
	GOGI_KEY_OEM_7,
	GOGI_KEY_QUOTE = GOGI_KEY_OEM_7,
	/* No US mapping (U+00A7 / "!" on some keyboards.) */
	GOGI_KEY_OEM_8,
	/* 0xe0: reserved. */
	/* 0xe1: OEM specific. */
	/* No US mapping ("><" on some keyboards.) */
	GOGI_KEY_OEM_102 = GOGI_FIRST_KEY + 0xe2,
	/* 0xe2 - 0xe4: OEM specific. */

	/* IME process key. If possible, a platform should inject a
	   GOGI_KEYDOWN event with GOGI_KEY_PROCESSKEY for every character pressed
	   during IME composition. The corresponding GOGI_KEYUP should then have
	   the virtual key of key involved. Web compatibility (and DOM Events
	   Level 3) dictates that this event pattern helps the implementation
	   of script-level IME implementations.

	   IME may not be supported by a platform, internally or externally, in
	   which case this key is effectively unmapped. */
	GOGI_KEY_PROCESSKEY = GOGI_FIRST_KEY + 0xe5,

	/* 0xe6: unmapped. */
	/* Virtual key for communicating a Unicode character. */
	GOGI_KEY_UNICODE = GOGI_FIRST_KEY + 0xe7,
	/* 0xe8: unassigned. */
	/* 0xe9 - 0xf5: OEM specific. */
	/* 0xf6-0xff: unassigned. */
	GOGI_KEY_FIRST_EXTENDED = GOGI_FIRST_KEY + 0x100,

	GOGI_KEY_F25,
	GOGI_KEY_F26,
	GOGI_KEY_F27,
	GOGI_KEY_F28,
	GOGI_KEY_F29,
	GOGI_KEY_F30,
	GOGI_KEY_F31,
	GOGI_KEY_F32,
	GOGI_KEY_F33,
	GOGI_KEY_F34,
	GOGI_KEY_F35,
	GOGI_KEY_F36,

	GOGI_KEY_GP_ANALOG_NORTH,
	GOGI_KEY_GP_ANALOG_SOUTH,
	GOGI_KEY_GP_ANALOG_WEST,
	GOGI_KEY_GP_ANALOG_EAST,
	GOGI_KEY_GP_DIGITAL_NORTH,
	GOGI_KEY_GP_DIGITAL_SOUTH,
	GOGI_KEY_GP_DIGITAL_WEST,
	GOGI_KEY_GP_DIGITAL_EAST,
	GOGI_KEY_GP_A,
	GOGI_KEY_GP_B,
	GOGI_KEY_GP_C_NORTH,
	GOGI_KEY_GP_C_SOUTH,
	GOGI_KEY_GP_C_WEST,
	GOGI_KEY_GP_C_EAST,
	GOGI_KEY_GP_L,
	GOGI_KEY_GP_R,
	GOGI_KEY_GP_X,
	GOGI_KEY_GP_Y,
	GOGI_KEY_GP_Z,
	GOGI_KEY_GP_START,

	GOGI_KEY_MAGIC_INPUT_1,
	GOGI_KEY_MAGIC_INPUT_2,
	GOGI_KEY_MAGIC_INPUT_3,
	GOGI_KEY_MAGIC_INPUT_4,
	GOGI_KEY_MAGIC_INPUT_5,
	GOGI_KEY_MAGIC_INPUT_6,
	GOGI_KEY_MAGIC_INPUT_7,
	GOGI_KEY_MAGIC_INPUT_8,
	GOGI_KEY_MAGIC_INPUT_9,
	GOGI_KEY_MAGIC_INPUT_10,

    GOGI_LAST_TRANSLATED_KEY,

	/* Constants specified by CEA-2014A for consumer electronics
	   devices. The DOM Events Level3 has entries for these (as
	   keynames),  but the value assignments match those of the
	   CEA-2014A specification. */
	GOGI_KEY_RED = GOGI_FIRST_KEY + 403,
	GOGI_KEY_GREEN,
	GOGI_KEY_YELLOW,
	GOGI_KEY_BLUE,
	GOGI_KEY_GREY,
	GOGI_KEY_BROWN,
	GOGI_KEY_POWER,
	GOGI_KEY_DIMMER,
	GOGI_KEY_WINK,
	GOGI_KEY_REWIND,
	GOGI_KEY_STOP,
	GOGI_KEY_EJECT_TOGGLE,
	GOGI_KEY_PLAY,
	GOGI_KEY_RECORD,
	GOGI_KEY_FAST_FWD,
	GOGI_KEY_PLAY_SPEED_UP,
	GOGI_KEY_PLAY_SPEED_DOWN,
	GOGI_KEY_PLAY_SPEED_RESET,
	GOGI_KEY_RECORD_SPEED_NEXT,
	GOGI_KEY_GO_TO_START,
	GOGI_KEY_GO_TO_END,
	GOGI_KEY_TRACK_PREV,
	GOGI_KEY_TRACK_NEXT,
	GOGI_KEY_RANDOM_TOGGLE,
	GOGI_KEY_CHANNEL_UP,
	GOGI_KEY_CHANNEL_DOWN,
	GOGI_KEY_STORE_FAVORITE_0,
	GOGI_KEY_STORE_FAVORITE_1,
	GOGI_KEY_STORE_FAVORITE_2,
	GOGI_KEY_STORE_FAVORITE_3,
	GOGI_KEY_RECALL_FAVORITE_0,
	GOGI_KEY_RECALL_FAVORITE_1,
	GOGI_KEY_RECALL_FAVORITE_2,
	GOGI_KEY_RECALL_FAVORITE_3,
	GOGI_KEY_CLEAR_FAVORITE_0,
	GOGI_KEY_CLEAR_FAVORITE_1,
	GOGI_KEY_CLEAR_FAVORITE_2,
	GOGI_KEY_CLEAR_FAVORITE_3,
	GOGI_KEY_SCAN_CHANNELS_TOGGLE,
	GOGI_KEY_PINP_TOGGLE,
	GOGI_KEY_SPLIT_SCREEN_TOGGLE,
	GOGI_KEY_DISPLAY_SWAP,
	GOGI_KEY_SCREEN_MODE_NEXT,
	GOGI_KEY_VIDEO_MODE_NEXT,
	GOGI_KEY_VOLUME_UP,
	GOGI_KEY_VOLUME_DOWN,
	GOGI_KEY_MUTE,
	GOGI_KEY_SURROUND_MODE_NEXT,
	GOGI_KEY_BALANCE_RIGHT,
	GOGI_KEY_BALANCE_LEFT,
	GOGI_KEY_FADER_FRONT,
	GOGI_KEY_FADER_REAR,
	GOGI_KEY_BASS_BOOST_UP,
	GOGI_KEY_BASS_BOOST_DOWN,
	GOGI_KEY_INFO,
	GOGI_KEY_GUIDE,
	GOGI_KEY_TELETEXT,
	GOGI_KEY_SUBTITLE,
	GOGI_KEY_BACK,
	GOGI_KEY_MENU,
	GOGI_KEY_PLAY_PAUSE,

	GOGI_LAST_KEY_EXTENDED,
	GOGI_LAST_KEY = GOGI_LAST_KEY_EXTENDED,

} TRAUMA_GOGI_VIRTUAL_KEY_e;

enum {
	GOGI_KEY_TVRADIO_TOGGLE = GOGI_KEY_NUMPAD2,
	GOGI_KEY_LIST,
	GOGI_KEY_SETTING,
	GOGI_KEY_OPTION,
	GOGI_KEY_PLAYER,
	GOGI_KEY_WIDE,
	GOGI_KEY_SEARCH,
	GOGI_KEY_RECORDINGS,
	//JAPAN
	GOGI_KEY_11,
	GOGI_KEY_12,
	GOGI_KEY_PROGRAM,
	GOGI_KEY_DATA_BROADCAST,
	GOGI_KEY_DIGITS,
	GOGI_KEY_VOD,
	GOGI_KEY_BROWSER,
	GOGI_KEY_DTT,
	GOGI_KEY_BS,
	GOGI_KEY_CATV,
	GOGI_KEY_LAST,
	GOGI_KEY_RECOMMAND,
	GOGI_KEY_ETC,
	GOGI_KEY_BROADCAST_NETWORK,
	GOGI_KEY_WPS,
	GOGI_KEY_HIDDEN_MENU,
	// MULTI-CUSTOM KEY
	GOGI_KEY_MULTI_CUSTOM,
	GOGI_KEY_MULTI_CUSTOM_SUCCESS,
	GOGI_KEY_MULTI_CUSTOM_FAIL,
	// Newly defined JAPAN
	GOGI_KEY_REPLAY,
	GOGI_KEY_SKIP,
	// front key
	GOGI_KEY_FRONT_UP ,    //GOGI_KEY_F16
	GOGI_KEY_FRONT_DOWN,
	GOGI_KEY_FRONT_LEFT,
	GOGI_KEY_FRONT_RIGHT,
	GOGI_KEY_FRONT_SW_REBOOT,   //

	//GOGI_KEY_HUMAX_CUSTOM_LAST =GOGI_KEY_RED  // GOGI_FIRST_KEY + 403
};

/*
	추후에 UI 시나리오가 달라지면 여기의 State 우선이 달라질 가능성이 높습니다.
	그래서 일단 함수로 따로 만들어 놓았고 추후 이에 따라 분기 여부를 결정하도록 하세요.
*/
typedef enum {
	OPLBROADCAST_CHANNEL_ERROR_CHANNEL_NOT_SUPPORTED	= 0,
	OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE,
	OPLBROADCAST_CHANNEL_ERROR_TUNER_LOCK,
	OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK,
	OPLBROADCAST_CHANNEL_ERROR_ENCRYPTED_CHANNEL,
	OPLBROADCAST_CHANNEL_ERROR_UNKNOWN_CHANNEL,				//5
	OPLBROADCAST_CHANNEL_ERROR_CHANNEL_SWITCH_INTERRUPTED,
	OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE_DUETO_RECORD,
	OPLBROADCAST_CHANNEL_ERROR_CANNOT_RESOLVE_IP_CHANNEL,
	OPLBROADCAST_CHANNEL_ERROR_BANDWIDTH,
	OPLBROADCAST_CHANNEL_ERROR_CANNOT_SUPPORT_CHANNEL_LIST, //10
	OPLBROADCAST_CHANNEL_ERROR_INSUFFICIENT_RESOURCE,
	OPLBROADCAST_CHANNEL_ERROR_NOT_FOUND_IN_TRANSPORT,
	/*humax 확장 start*/
	OPLBROADCAST_CHANNEL_ERROR_MOTOR_MOVING,
	OPLBROADCAST_CHANNEL_ERROR_CAS_BLOCK,
	OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED,					//15
	OPLBROADCAST_CHANNEL_ERROR_AV_UNDERRUN,
	OPLBROADCAST_CHANNEL_ERROR_HDMI_BLOCK,
	OPLBROADCAST_CHANNEL_ERROR_7DAY_TIMER_BLOCK,
	OPLBROADCAST_CHANNEL_ERROR_NO_VIDEO,
	OPLBROADCAST_CHANNEL_ERROR_NO_AV,						//20
	OPLBROADCAST_CHANNEL_ERROR_NOT_RUNNING,
	OPLBROADCAST_CHANNEL_ERROR_NO_EIT,
	OPLBROADCAST_CHANNEL_ERROR_5_TIMES_PIN_FAIL,
	OPLBROADCAST_CHANNEL_ERROR_60DAY_FTA_LIMIT,
	OPLBROADCAST_CHANNEL_ERROR_EMPTY_CHANNEL,				//25
	OPLBROADCAST_CHANNEL_ERROR_NOK_ANTENNA,
	OPLBROADCAST_CHANNEL_ERROR_NO_ERROR,					//27
	/*humax 확장 End*/

	OPLBROADCAST_CHANNEL_ERROR_UNDEFINED					= 100,
	/*JLabs 확장 start*/
	OPLBROADCAST_CHANNEL_ERROR_TSMF_LOW					= 400,
	OPLBROADCAST_CHANNEL_ERROR_NO_SERVICE_IN_PAT			= 500,
	OPLBROADCAST_CHANNEL_ERROR_PMT_TIMEOUT					= 501,
	OPLBROADCAST_CHANNEL_ERROR_NO_PAT_IN_TEMP_SVC			= 502,
	OPLBROADCAST_CHANNEL_ERROR_PAT_TIMEOUT					= 503,
	/*JLabs 확장 End*/

	/* Humax 확장 */
	OPLBROADCAST_CHANNEL_ERROR_NOERROR						= 2000,

	OPLBROADCAST_CHANNEL_ERROR_ENDOFCASE
	/* Humax End */
} TRAUMA_PlaybackError;

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */

TRAUMA_GOGI_VIRTUAL_KEY_e	trauma_tool_dfbkey_to_gogikey_symbol_base(HINT32 nSysmbol)
{
	TRAUMA_GOGI_VIRTUAL_KEY_e eRetKey;

	switch (nSysmbol)
	{
		TRAUMA_GOGI_SYMBOL(DIKS_INTERNET,		GOGI_KEY_HOME);
		TRAUMA_GOGI_SYMBOL(DIKS_PLAYER,			GOGI_KEY_PLAYER);
		TRAUMA_GOGI_SYMBOL(DIKS_RED,			GOGI_KEY_RED);
		TRAUMA_GOGI_SYMBOL(DIKS_GREEN, 			GOGI_KEY_GREEN);
		TRAUMA_GOGI_SYMBOL(DIKS_YELLOW,			GOGI_KEY_YELLOW);
		TRAUMA_GOGI_SYMBOL(DIKS_BLUE,			GOGI_KEY_BLUE);
		TRAUMA_GOGI_SYMBOL(DIKS_VOLUME_UP, 		GOGI_KEY_VOLUME_UP);
		TRAUMA_GOGI_SYMBOL(DIKS_VOLUME_DOWN,	GOGI_KEY_VOLUME_DOWN);
		TRAUMA_GOGI_SYMBOL(DIKS_OPTION,			GOGI_KEY_OPTION);
		TRAUMA_GOGI_SYMBOL(DIKS_INFO,			GOGI_KEY_INFO);
		TRAUMA_GOGI_SYMBOL(DIKS_CHANNEL_UP,		GOGI_KEY_CHANNEL_UP);
		TRAUMA_GOGI_SYMBOL(DIKS_CHANNEL_DOWN,	GOGI_KEY_CHANNEL_DOWN);
		TRAUMA_GOGI_SYMBOL(DIKS_EXIT,			GOGI_KEY_ESCAPE);
#if defined(CONFIG_PROD_HD9000I)
		TRAUMA_GOGI_SYMBOL(DIKS_LAST,			GOGI_KEY_VOD);
#elif defined(CONFIG_PROD_CDIG2000C)
		TRAUMA_GOGI_SYMBOL(DIKS_LAST,			GOGI_KEY_BACK);
#else
		TRAUMA_GOGI_SYMBOL(DIKS_LAST,			GOGI_KEY_LAST);
#endif
		TRAUMA_GOGI_SYMBOL(DIKS_CURSOR_DOWN,	GOGI_KEY_DOWN);
		TRAUMA_GOGI_SYMBOL(DIKS_CURSOR_UP, 		GOGI_KEY_UP);
		TRAUMA_GOGI_SYMBOL(DIKS_CURSOR_LEFT,	GOGI_KEY_LEFT);
		TRAUMA_GOGI_SYMBOL(DIKS_CURSOR_RIGHT,	GOGI_KEY_RIGHT);
		TRAUMA_GOGI_SYMBOL(DIKS_MENU,			GOGI_KEY_MENU);
		TRAUMA_GOGI_SYMBOL(DIKS_HOME,			GOGI_KEY_MENU);
		TRAUMA_GOGI_SYMBOL(DIKS_EPG,			GOGI_KEY_GUIDE);
		TRAUMA_GOGI_SYMBOL(DIKS_PREVIOUS,		GOGI_KEY_TRACK_PREV);
		TRAUMA_GOGI_SYMBOL(DIKS_STOP,			GOGI_KEY_STOP);
		TRAUMA_GOGI_SYMBOL(DIKS_NEXT,			GOGI_KEY_TRACK_NEXT);
		TRAUMA_GOGI_SYMBOL(DIKS_REWIND,			GOGI_KEY_REWIND);
		TRAUMA_GOGI_SYMBOL(DIKS_PLAY,			GOGI_KEY_PLAY);
		TRAUMA_GOGI_SYMBOL(DIKS_PAUSE, 			GOGI_KEY_PAUSE);
		TRAUMA_GOGI_SYMBOL(DIKS_PLAYPAUSE, 		GOGI_KEY_PLAY_PAUSE);
		TRAUMA_GOGI_SYMBOL(DIKS_FASTFORWARD,	GOGI_KEY_FAST_FWD);
		TRAUMA_GOGI_SYMBOL(DIKS_MUTE,			GOGI_KEY_MUTE);
		TRAUMA_GOGI_SYMBOL(DIKS_RECORD,			GOGI_KEY_RECORD);
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM4,		GOGI_KEY_LIST);			//	Channel List
		TRAUMA_GOGI_SYMBOL(DIKS_TV,				GOGI_KEY_TVRADIO_TOGGLE);
		TRAUMA_GOGI_SYMBOL(DIKS_ZOOM,			GOGI_KEY_SCREEN_MODE_NEXT);		//	Wide
		TRAUMA_GOGI_SYMBOL(DIKS_TEXT,			GOGI_KEY_TELETEXT);
		TRAUMA_GOGI_SYMBOL(DIKS_POWER, 			GOGI_KEY_POWER);
		TRAUMA_GOGI_SYMBOL(DIKS_ENTER, 			GOGI_KEY_ENTER);
		TRAUMA_GOGI_SYMBOL(DIKS_AUDIO, 			GOGI_KEY_SURROUND_MODE_NEXT);
		TRAUMA_GOGI_SYMBOL(DIKS_SUBTITLE,		GOGI_KEY_SUBTITLE);
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM5,		GOGI_KEY_SETTING);
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM6,		GOGI_KEY_SEARCH);
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM7,		GOGI_KEY_MULTI_CUSTOM);
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM8,		GOGI_KEY_MULTI_CUSTOM_SUCCESS);
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM9,		GOGI_KEY_MULTI_CUSTOM_FAIL);
		// JAPAN
		TRAUMA_GOGI_SYMBOL(DIKS_FAVORITES, 		GOGI_KEY_RECALL_FAVORITE_0);	// HKS_FAVORITES
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM61,		GOGI_KEY_DATA_BROADCAST); 	// HKS_DATA_BROADCAST
		TRAUMA_GOGI_SYMBOL(DIKS_PROGRAM,		GOGI_KEY_PROGRAM);				// HKS_PROGRAM
		TRAUMA_GOGI_SYMBOL(DIKS_DIGITS,			GOGI_KEY_DIGITS); 			// HKS_DIGITS
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM45,		GOGI_KEY_VOD);					// HKS_VOD
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM23,		GOGI_KEY_BROWSER);				// HKS_BROWSER, FULL_BROWSER
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM56,		GOGI_KEY_DTT);					// HKS_DTT
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM57,		GOGI_KEY_BS); 				// HKS_BS
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM58,		GOGI_KEY_CATV);				// HKS_CATV
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM10,		GOGI_KEY_11); 				// HKS_11
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM11,		GOGI_KEY_12); 				// HKS_12
		TRAUMA_GOGI_SYMBOL(DIKS_BACK,			GOGI_KEY_BACK);				// HKS_BACK
		TRAUMA_GOGI_SYMBOL(DIKS_SCREEN,			GOGI_KEY_SCREEN_MODE_NEXT);	// HKS_MODE
		TRAUMA_GOGI_SYMBOL(DIKS_SETUP, 			GOGI_KEY_SETTING);				// HKS_SETUP
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM16,		GOGI_KEY_STORE_FAVORITE_0);	// HKS_PRESET_CH
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM54,		GOGI_KEY_RECOMMAND);			// HKS_RECOMMAND
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM53,		GOGI_KEY_ETC);					// HKS_ETC
		TRAUMA_GOGI_SYMBOL(DIKS_SHIFT, 			GOGI_KEY_SHIFT);				// HKS_SHIFT
		TRAUMA_GOGI_SYMBOL(DIKS_CLEAR, 			GOGI_KEY_CLEAR);				// HKS_CLEAR
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM59,		GOGI_KEY_BROADCAST_NETWORK);	// 방송전화(FRONT 버튼)
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM98,		GOGI_KEY_WPS);					// WPS(FRONT 버튼)
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM99,		GOGI_KEY_HIDDEN_MENU);			// Hidden(CH+, CH-) (FRONT 버튼)
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM65,		GOGI_KEY_REPLAY); 		// replay
		TRAUMA_GOGI_SYMBOL(DIKS_CUSTOM66,		GOGI_KEY_SKIP);			// skip
		default:
			{
				if (DFB_KEY_TYPE(nSysmbol) == DIKT_IDENTIFIER)
				{
					eRetKey = (TRAUMA_GOGI_VIRTUAL_KEY_e)((nSysmbol&0x00ff)+0x40);
				}
				else if (DFB_KEY_TYPE(nSysmbol) == DIKT_UNICODE)
				{
					eRetKey = (TRAUMA_GOGI_VIRTUAL_KEY_e)DFB_UPPER_CASE(nSysmbol);
				}
				else
				{
					eRetKey = (TRAUMA_GOGI_VIRTUAL_KEY_e)(nSysmbol & 0xFF);
				}
			}
			break;
	}
	return eRetKey;
}


static HBOOL trauma_tool_scheudle_isRecord(DxRsvType_e eRsvType)
{
	HBOOL bRet = FALSE;

	switch(eRsvType)
	{
	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_INSTANT:
	case DxRSVTYPE_RECORD_DELAYED:
	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		bRet = TRUE;
		break;
	default:
		break;
	}
	return bRet;
}

static HBOOL trauma_tool_scheudle_isRemind(DxRsvType_e eRsvType)
{
	HBOOL bRet = FALSE;

	switch(eRsvType)
	{
	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_WATCHING_TBR:
	case DxRSVTYPE_WATCHING_SERIES_EBR:
		bRet = TRUE;
		break;
	default:
		break;
	}
	return bRet;
}

static HBOOL trauma_tool_scheudle_isStream(DxRsvType_e eRsvType)
{
	HBOOL bRet = FALSE;

	switch(eRsvType)
	{
	case DxRSVTYPE_NETSTREAMING_LIVE:
		bRet = TRUE;
		break;
	default:
		break;
	}
	return bRet;
}

/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

HINT32 TRAUMA_TOOL_DFBKeyConvertGogiKey(HINT32 nSysmbol)
{
	HINT32 nRetKey = 0;

	nRetKey = (HINT32)trauma_tool_dfbkey_to_gogikey_symbol_base(nSysmbol);

	HxLOG_Debug("[0x%x]to[%d]\n",nSysmbol,nRetKey);
	return nRetKey;
}

HCHAR* TRAUMA_TOOL_BoolConvertString(HBOOL bBool)
{
	if(bBool == TRUE)
		return "true";

	return "false";
}

HCHAR* TRAUMA_TOOL_AspectRatioConvertString(DxAspectRatio_e eTVAspectRatio)
{
	switch(eTVAspectRatio)
	{
	case eDxASPECTRATIO_4X3: 		return "4:3";
	case eDxASPECTRATIO_16X9: 		return "16:9";
	case eDxASPECTRATIO_14X9: 		return "14:9";
	case eDxASPECTRATIO_UNKNOWN: 	return "NULL";
	default: 						return "NULL";
	}
	return "NULL";
}

HCHAR* TRAUMA_TOOL_DFCConvertString(DxDfcSelect_e eDfc)
{
	switch(eDfc)
	{
	case eDxDFC_SELECT_DEFAULT: 		return "Auto";
	case eDxDFC_SELECT_WIDESIGNAL: 		return "wide_signal";
	case eDxDFC_SELECT_SQUEEZE: 		return "Sqeeze";
	case eDxDFC_SELECT_SQUEEZE149: 		return "Sqeeze_14x9";
	case eDxDFC_SELECT_CENTERCUTOUT: 	return "CenterCutOut";
	case eDxDFC_SELECT_CENTERCUTOUT149: return "CenterCutOut_14x9";
	case eDxDFC_SELECT_FULLSCREEN: 		return "Anamorphic";
	default: 							return "NULL";
	}
	return "NULL";
}

HCHAR* TRAUMA_TOOL_ResolutionConvertString(DxDfcSelect_e eDfc)
{
	switch(eDfc)
	{
	case eDxRESOLUTION_SELECT_480I: 		return "480i";
	case eDxRESOLUTION_SELECT_480P: 		return "480p";
	case eDxRESOLUTION_SELECT_576I: 		return "576i";
	case eDxRESOLUTION_SELECT_576P: 		return "576p";
	case eDxRESOLUTION_SELECT_720P: 		return "720p";
	case eDxRESOLUTION_SELECT_1080I: 		return "1080i";
	case eDxRESOLUTION_SELECT_1080P: 		return "1080p";
	case eDxRESOLUTION_SELECT_2160P: 		return "2160p";
	case eDxRESOLUTION_SELECT_ORIGINAL: 	return "Original";
	case eDxRESOLUTION_SELECT_EDID_MAX: 	return "Native";
	default: 								return "NULL";
	}
	return "NULL";
}

HCHAR* TRAUMA_TOOL_DigitalAudioFormatConvertString(DxDigitalAudioFormat_e	eSpdifOut)
{
	switch(eSpdifOut)
	{
	case eDxDIGITALAUDIO_FORMAT_PCM: 				return "PCM";
	case eDxDIGITALAUDIO_FORMAT_MULTICHANNEL: 		return "Multi";
	default: 										return "NULL";
	}
	return "NULL";
}

HCHAR* TRAUMA_TOOL_SoundModeConvertString(DxStereoSelect_e	eStereoSelect)
{
	switch(eStereoSelect)
	{
	case eDxSTEREO_SELECT_STEREO: 			return "Stereo";
	case eDxSTEREO_SELECT_MONOLEFT: 		return "Left";
	case eDxSTEREO_SELECT_MONORIGHT: 		return "Right";
	case eDxSTEREO_SELECT_MONOMIXED: 		return "Mix";
	default: 								return "NULL";
	}
	return "NULL";
}

HCHAR* TRAUMA_TOOL_RepeatConvertString(DxRsvRepeat_e	eRepeat)
{
	switch(eRepeat)
	{
	case DxRSVREPEAT_ONCE: 			return "once";
	case DxRSVREPEAT_DAILY: 		return "daily";
	case DxRSVREPEAT_WEEKLY: 		return "weekly";
	default: 						return "NULL";
	}
	return "NULL";
}

HCHAR* TRAUMA_TOOL_AntennaTypeConvertString(DxAntennaType_e	eAntType)
{
	switch(eAntType)
	{
	case eDxANT_TYPE_LNB_ONLY: 		return "LNB Only";
	case eDxANT_TYPE_DISEQC: 		return "DiSEqC";
	case eDxANT_TYPE_SCD: 			return "SCD";
	default: 						return "NULL";
	}
	return "NULL";
}


HCHAR* TRAUMA_TOOL_SourceTypeConvertString(DxSwUpdate_Source_e	eSource)
{
	switch(eSource)
	{
	case eDxSWUPDATE_SOURCE_Rf: 		return "RF";
	case eDxSWUPDATE_SOURCE_Ip: 		return "IP";
	case eDxSWUPDATE_SOURCE_File: 		return "FILE";
	case eDxSWUPDATE_SOURCE_Usb: 		return "USB";
	default: 							return "NULL";
	}
	return "NULL";
}

HINT32 TRAUMA_TOOL_GetErrorByStates (OxMediaPlay_LiveViewState_e eViewState, OxMediaPlay_LiveLockState_e eLockState)
{
	// 지상파 경우 antenna short-circuit 이 0 순위 가짐 : MOON
	if (eOxMP_LiveViewState_NOK_ANTENNA == eViewState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_NOK_ANTENNA;
	}

	// 우선순위 1: Channel Unlock
	if ((eOxMP_LiveViewState_NO_SIGNAL == eViewState) ||
		(eOxMP_LiveViewState_NO_SIGNAL_TIMEOUT == eViewState))
	{
		return OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE;
	}

	if (eOxMP_LiveViewState_MOTOR_MOVING == eViewState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_MOTOR_MOVING;
	}

	// 우선순위 2: Empty Service
	if (eOxMP_LiveViewState_EMPTY_SVC == eViewState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_EMPTY_CHANNEL;
	}

	// 우선순위 3: User Lock 관련
	if (eOxMP_LiveLockState_LOCK_PIN == eLockState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK;
	}


	if (eOxMP_LiveLockState_LOCK_PIN_DENIED == eLockState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED;
	}

	// eOxMP_LiveViewState_REMOVE_CH 관련은 어느 우선순위에 있는지 구현해 주십시오.
	/*
	if (eOxMP_LiveViewState_REMOVE_CH == hBcast->eViewState)
	{
	}
	*/

	// 우선순위 4: CAS
	if (eOxMP_LiveViewState_CAS == eViewState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_CAS_BLOCK;
	}

	// 우선순위 5: SVC Lock 외의 Lock State들:
	switch (eLockState)
	{
	case eOxMP_LiveLockState_RATING_PIN:
	case eOxMP_LiveLockState_SC_PIN:
		return OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK;

	case eOxMP_LiveLockState_RATING_PIN_DENIED:
	case eOxMP_LiveLockState_SC_PIN_DENIED:
		return OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED;

	case eOxMP_LiveLockState_NO_EIT:
		return OPLBROADCAST_CHANNEL_ERROR_NO_EIT;

	default:
		break;
	}

	// 우선순위 6: No SVC, A/V 관련
	switch (eViewState)
	{
	case eOxMP_LiveViewState_AV_UNDER_RUN:
		return OPLBROADCAST_CHANNEL_ERROR_AV_UNDERRUN;

	case eOxMP_LiveViewState_HDMI_BLOCKED:
		return OPLBROADCAST_CHANNEL_ERROR_HDMI_BLOCK;

	case eOxMP_LiveViewState_NO_VIDEO:
		return OPLBROADCAST_CHANNEL_ERROR_NO_VIDEO;

	case eOxMP_LiveViewState_NO_AV:
		return OPLBROADCAST_CHANNEL_ERROR_NO_AV;

	case eOxMP_LiveViewState_NOT_RUNNING:
		return OPLBROADCAST_CHANNEL_ERROR_NOT_RUNNING;

	default:
		break;
	}

	if (((eOxMP_LiveViewState_OK == eViewState) || (eOxMP_LiveViewState_NONE == eViewState)) &&
		((eOxMP_LiveLockState_OK == eLockState) || (eOxMP_LiveLockState_NONE == eLockState)))
	{
		return OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
	}

	return OPLBROADCAST_CHANNEL_ERROR_UNDEFINED;
}

TRAUMA_Msg_e TRAUMA_TOOL_GetCodeByScheduleType(APK_META_SCH_MsgType_e eMsgType, DxRsvType_e eRsvType)
{
	TRAUMA_Msg_e eRetMSg = eTRAUMA_MSG_START;

	switch (eMsgType)
	{
		case eAPK_METASCH_MSG_TimeUpStart:
			HxLOG_Debug("eAPK_METASCH_MSG_TimeUpStart\n");
			if(trauma_tool_scheudle_isRecord(eRsvType) == TRUE)
				eRetMSg = eTRAUMA_MSG_STATISTICS_CODE_ERECS;
			else if(trauma_tool_scheudle_isRemind(eRsvType) == TRUE)
				eRetMSg = eTRAUMA_MSG_STATISTICS_CODE_EREMIND;
			else if(trauma_tool_scheudle_isStream(eRsvType) == TRUE)
				eRetMSg = eTRAUMA_MSG_STATISTICS_CODE_ELSS;
			else
			{
				;
			}
			break;
		case eAPK_METASCH_MSG_ScheduleRemoved:
			HxLOG_Debug("eAPK_METASCH_MSG_TimeUpStart\n");
			if(trauma_tool_scheudle_isRecord(eRsvType) == TRUE)
				eRetMSg = eTRAUMA_MSG_STATISTICS_CODE_ERECE;
			else if(trauma_tool_scheudle_isStream(eRsvType) == TRUE)
				eRetMSg = eTRAUMA_MSG_STATISTICS_CODE_ELSE;
			else
			{
				;
			}
			break;
		default:
			break;
	}
	return eRetMSg;
}


