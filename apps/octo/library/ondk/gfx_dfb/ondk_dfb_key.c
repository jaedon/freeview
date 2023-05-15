/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>
#include 	<hapi.h>

#include 	"ondk_dfb.h"


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

typedef struct _ONDK_HEvtKey
{
	HBOOL				loop;
	HULONG				thdId;
	ONDKWindowHandle	mWindow;
	HINT32			 	mWindowId;

	HBOOL				bIsGWM;
	ONDK_KEY_KeyNotifyCallback_t	pfnKeyCallback;

	IDirectFBEventBuffer *pDFBEvtBuffer;
} ONDK_HEvtKey_t;


static	HxList_t				*s_pEventBuffer = 0; // ONDK_HEvtKey_t
static  HULONG					s_ulEventSem = 0;
static 	HBOOL				s_bKeyActive = FALSE;
static HUINT32				 	s_ulKeyTaskId = 0;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
#ifdef	CONFIG_DEBUG
static char * local_dfb_DBG_GetKeystring(DFBInputDeviceKeySymbol key_symbol)
{
	switch(key_symbol)
	{
	ENUM_TO_STR(DIKS_NULL);
	ENUM_TO_STR(DIKS_BACKSPACE);
	ENUM_TO_STR(DIKS_TAB);
	ENUM_TO_STR(DIKS_RETURN);
	ENUM_TO_STR(DIKS_CANCEL);
	ENUM_TO_STR(DIKS_ESCAPE);
	ENUM_TO_STR(DIKS_SPACE);
	ENUM_TO_STR(DIKS_EXCLAMATION_MARK);
	ENUM_TO_STR(DIKS_QUOTATION);
	ENUM_TO_STR(DIKS_NUMBER_SIGN);
	ENUM_TO_STR(DIKS_DOLLAR_SIGN);
	ENUM_TO_STR(DIKS_PERCENT_SIGN);
	ENUM_TO_STR(DIKS_AMPERSAND);
	ENUM_TO_STR(DIKS_APOSTROPHE);
	ENUM_TO_STR(DIKS_PARENTHESIS_LEFT);
	ENUM_TO_STR(DIKS_PARENTHESIS_RIGHT);
	ENUM_TO_STR(DIKS_PLUS_SIGN);
	ENUM_TO_STR(DIKS_COMMA);
	ENUM_TO_STR(DIKS_MINUS_SIGN);
	ENUM_TO_STR(DIKS_PERIOD);
	ENUM_TO_STR(DIKS_SLASH);
	ENUM_TO_STR(DIKS_0);
	ENUM_TO_STR(DIKS_1);
	ENUM_TO_STR(DIKS_2);
	ENUM_TO_STR(DIKS_3);
	ENUM_TO_STR(DIKS_4);
	ENUM_TO_STR(DIKS_5);
	ENUM_TO_STR(DIKS_6);
	ENUM_TO_STR(DIKS_7);
	ENUM_TO_STR(DIKS_8);
	ENUM_TO_STR(DIKS_9);
	ENUM_TO_STR(DIKS_COLON);
	ENUM_TO_STR(DIKS_SEMICOLON);
	ENUM_TO_STR(DIKS_LESS_THAN_SIGN);
	ENUM_TO_STR(DIKS_EQUALS_SIGN);
	ENUM_TO_STR(DIKS_GREATER_THAN_SIGN);
	ENUM_TO_STR(DIKS_QUESTION_MARK);
	ENUM_TO_STR(DIKS_AT);
	ENUM_TO_STR(DIKS_CAPITAL_A);
	ENUM_TO_STR(DIKS_CAPITAL_B);
	ENUM_TO_STR(DIKS_CAPITAL_C);
	ENUM_TO_STR(DIKS_CAPITAL_D);
	ENUM_TO_STR(DIKS_CAPITAL_E);
	ENUM_TO_STR(DIKS_CAPITAL_F);
	ENUM_TO_STR(DIKS_CAPITAL_G);
	ENUM_TO_STR(DIKS_CAPITAL_H);
	ENUM_TO_STR(DIKS_CAPITAL_I);
	ENUM_TO_STR(DIKS_CAPITAL_J);
	ENUM_TO_STR(DIKS_CAPITAL_K);
	ENUM_TO_STR(DIKS_CAPITAL_L);
	ENUM_TO_STR(DIKS_CAPITAL_M);
	ENUM_TO_STR(DIKS_CAPITAL_N);
	ENUM_TO_STR(DIKS_CAPITAL_O);
	ENUM_TO_STR(DIKS_CAPITAL_P);
	ENUM_TO_STR(DIKS_CAPITAL_Q);
	ENUM_TO_STR(DIKS_CAPITAL_R);
	ENUM_TO_STR(DIKS_CAPITAL_S);
	ENUM_TO_STR(DIKS_CAPITAL_T);
	ENUM_TO_STR(DIKS_CAPITAL_U);
	ENUM_TO_STR(DIKS_CAPITAL_V);
	ENUM_TO_STR(DIKS_CAPITAL_W);
	ENUM_TO_STR(DIKS_CAPITAL_X);
	ENUM_TO_STR(DIKS_CAPITAL_Y);
	ENUM_TO_STR(DIKS_CAPITAL_Z);
	ENUM_TO_STR(DIKS_SQUARE_BRACKET_LEFT);
	ENUM_TO_STR(DIKS_BACKSLASH);
	ENUM_TO_STR(DIKS_SQUARE_BRACKET_RIGHT);
	ENUM_TO_STR(DIKS_CIRCUMFLEX_ACCENT);
	ENUM_TO_STR(DIKS_UNDERSCORE);
	ENUM_TO_STR(DIKS_GRAVE_ACCENT);
	ENUM_TO_STR(DIKS_SMALL_A);
	ENUM_TO_STR(DIKS_SMALL_B);
	ENUM_TO_STR(DIKS_SMALL_C);
	ENUM_TO_STR(DIKS_SMALL_D);
	ENUM_TO_STR(DIKS_SMALL_E);
	ENUM_TO_STR(DIKS_SMALL_F);
	ENUM_TO_STR(DIKS_SMALL_G);
	ENUM_TO_STR(DIKS_SMALL_H);
	ENUM_TO_STR(DIKS_SMALL_I);
	ENUM_TO_STR(DIKS_SMALL_J);
	ENUM_TO_STR(DIKS_SMALL_K);
	ENUM_TO_STR(DIKS_SMALL_L);
	ENUM_TO_STR(DIKS_SMALL_M);
	ENUM_TO_STR(DIKS_SMALL_N);
	ENUM_TO_STR(DIKS_SMALL_O);
	ENUM_TO_STR(DIKS_SMALL_P);
	ENUM_TO_STR(DIKS_SMALL_Q);
	ENUM_TO_STR(DIKS_SMALL_R);
	ENUM_TO_STR(DIKS_SMALL_S);
	ENUM_TO_STR(DIKS_SMALL_T);
	ENUM_TO_STR(DIKS_SMALL_U);
	ENUM_TO_STR(DIKS_SMALL_V);
	ENUM_TO_STR(DIKS_SMALL_W);
	ENUM_TO_STR(DIKS_SMALL_X);
	ENUM_TO_STR(DIKS_SMALL_Y);
	ENUM_TO_STR(DIKS_SMALL_Z);

	ENUM_TO_STR(DIKS_CURLY_BRACKET_LEFT);
	ENUM_TO_STR(DIKS_VERTICAL_BAR);
	ENUM_TO_STR(DIKS_CURLY_BRACKET_RIGHT);
	ENUM_TO_STR(DIKS_TILDE);
	ENUM_TO_STR(DIKS_DELETE);

	ENUM_TO_STR(DIKS_CURSOR_LEFT);
	ENUM_TO_STR(DIKS_CURSOR_RIGHT);
	ENUM_TO_STR(DIKS_CURSOR_UP);
	ENUM_TO_STR(DIKS_CURSOR_DOWN);
	ENUM_TO_STR(DIKS_INSERT);
	ENUM_TO_STR(DIKS_HOME);
	ENUM_TO_STR(DIKS_END);
	ENUM_TO_STR(DIKS_PAGE_UP);
	ENUM_TO_STR(DIKS_PAGE_DOWN);
	ENUM_TO_STR(DIKS_PRINT);
	ENUM_TO_STR(DIKS_PAUSE);
	ENUM_TO_STR(DIKS_OK);
	ENUM_TO_STR(DIKS_SELECT);
	ENUM_TO_STR(DIKS_GOTO);
	ENUM_TO_STR(DIKS_CLEAR);
	ENUM_TO_STR(DIKS_POWER);
	ENUM_TO_STR(DIKS_POWER2);
	ENUM_TO_STR(DIKS_OPTION);
	ENUM_TO_STR(DIKS_MENU);
	ENUM_TO_STR(DIKS_HELP);
	ENUM_TO_STR(DIKS_INFO);
	ENUM_TO_STR(DIKS_TIME);
	ENUM_TO_STR(DIKS_VENDOR);

	ENUM_TO_STR(DIKS_ARCHIVE);
	ENUM_TO_STR(DIKS_PROGRAM);
	ENUM_TO_STR(DIKS_CHANNEL);
	ENUM_TO_STR(DIKS_FAVORITES);
	ENUM_TO_STR(DIKS_EPG);
	ENUM_TO_STR(DIKS_PVR);
	ENUM_TO_STR(DIKS_MHP);
	ENUM_TO_STR(DIKS_LANGUAGE);
	ENUM_TO_STR(DIKS_TITLE);
	ENUM_TO_STR(DIKS_SUBTITLE);
	ENUM_TO_STR(DIKS_ANGLE);
	ENUM_TO_STR(DIKS_ZOOM);
	ENUM_TO_STR(DIKS_MODE);
	ENUM_TO_STR(DIKS_KEYBOARD);
	ENUM_TO_STR(DIKS_PC);
	ENUM_TO_STR(DIKS_SCREEN);
	ENUM_TO_STR(DIKS_TV);
	ENUM_TO_STR(DIKS_TV2);
	ENUM_TO_STR(DIKS_VCR);
	ENUM_TO_STR(DIKS_VCR2);
	ENUM_TO_STR(DIKS_SAT);
	ENUM_TO_STR(DIKS_SAT2);
	ENUM_TO_STR(DIKS_CD);
	ENUM_TO_STR(DIKS_TAPE);
	ENUM_TO_STR(DIKS_RADIO);
	ENUM_TO_STR(DIKS_TUNER);
	ENUM_TO_STR(DIKS_PLAYER);
	ENUM_TO_STR(DIKS_TEXT);
	ENUM_TO_STR(DIKS_DVD);
	ENUM_TO_STR(DIKS_AUX);
	ENUM_TO_STR(DIKS_MP3);
	ENUM_TO_STR(DIKS_PHONE);
	ENUM_TO_STR(DIKS_AUDIO);
	ENUM_TO_STR(DIKS_VIDEO);

	ENUM_TO_STR(DIKS_INTERNET);
	ENUM_TO_STR(DIKS_MAIL);
	ENUM_TO_STR(DIKS_NEWS);
	ENUM_TO_STR(DIKS_DIRECTORY);
	ENUM_TO_STR(DIKS_LIST);
	ENUM_TO_STR(DIKS_CALCULATOR);
	ENUM_TO_STR(DIKS_MEMO);
	ENUM_TO_STR(DIKS_CALENDAR);
	ENUM_TO_STR(DIKS_EDITOR);
	ENUM_TO_STR(DIKS_RED);
	ENUM_TO_STR(DIKS_GREEN);
	ENUM_TO_STR(DIKS_YELLOW);
	ENUM_TO_STR(DIKS_BLUE);

	ENUM_TO_STR(DIKS_CHANNEL_UP);
	ENUM_TO_STR(DIKS_CHANNEL_DOWN);
	ENUM_TO_STR(DIKS_BACK);
	ENUM_TO_STR(DIKS_FORWARD);
	ENUM_TO_STR(DIKS_FIRST);
	ENUM_TO_STR(DIKS_LAST);
	ENUM_TO_STR(DIKS_VOLUME_UP);
	ENUM_TO_STR(DIKS_VOLUME_DOWN);
	ENUM_TO_STR(DIKS_MUTE);
	ENUM_TO_STR(DIKS_AB);
	ENUM_TO_STR(DIKS_PLAYPAUSE);
	ENUM_TO_STR(DIKS_PLAY);
	ENUM_TO_STR(DIKS_STOP);
	ENUM_TO_STR(DIKS_RESTART);
	ENUM_TO_STR(DIKS_SLOW);
	ENUM_TO_STR(DIKS_FAST);
	ENUM_TO_STR(DIKS_RECORD);
	ENUM_TO_STR(DIKS_EJECT);
	ENUM_TO_STR(DIKS_SHUFFLE);
	ENUM_TO_STR(DIKS_REWIND);
	ENUM_TO_STR(DIKS_FASTFORWARD);
	ENUM_TO_STR(DIKS_PREVIOUS);
	ENUM_TO_STR(DIKS_NEXT);
	ENUM_TO_STR(DIKS_BEGIN);
	ENUM_TO_STR(DIKS_DIGITS);
	ENUM_TO_STR(DIKS_TEEN);
	ENUM_TO_STR(DIKS_TWEN);
	ENUM_TO_STR(DIKS_BREAK);
	ENUM_TO_STR(DIKS_EXIT);
	ENUM_TO_STR(DIKS_SETUP);
	ENUM_TO_STR(DIKS_CURSOR_LEFT_UP);
	ENUM_TO_STR(DIKS_CURSOR_LEFT_DOWN);
	ENUM_TO_STR(DIKS_CURSOR_UP_RIGHT);
	ENUM_TO_STR(DIKS_CURSOR_DOWN_RIGHT);
	ENUM_TO_STR(DIKS_PIP);
	ENUM_TO_STR(DIKS_SWAP);
	ENUM_TO_STR(DIKS_MOVE);
	ENUM_TO_STR(DIKS_FREEZE);
	ENUM_TO_STR(DIKS_F1);
	ENUM_TO_STR(DIKS_F2);
	ENUM_TO_STR(DIKS_F3);
	ENUM_TO_STR(DIKS_F4);
	ENUM_TO_STR(DIKS_F5);
	ENUM_TO_STR(DIKS_F6);
	ENUM_TO_STR(DIKS_F7);
	ENUM_TO_STR(DIKS_F8);
	ENUM_TO_STR(DIKS_F9);
	ENUM_TO_STR(DIKS_F10);
	ENUM_TO_STR(DIKS_F11);
	ENUM_TO_STR(DIKS_F12);
	ENUM_TO_STR(DIKS_SHIFT);
	ENUM_TO_STR(DIKS_CONTROL);
	ENUM_TO_STR(DIKS_ALT);
	ENUM_TO_STR(DIKS_ALTGR);
	ENUM_TO_STR(DIKS_META);
	ENUM_TO_STR(DIKS_SUPER);
	ENUM_TO_STR(DIKS_HYPER);
	ENUM_TO_STR(DIKS_CAPS_LOCK);
	ENUM_TO_STR(DIKS_NUM_LOCK);
	ENUM_TO_STR(DIKS_SCROLL_LOCK);
	ENUM_TO_STR(DIKS_DEAD_ABOVEDOT);
	ENUM_TO_STR(DIKS_DEAD_ABOVERING);
	ENUM_TO_STR(DIKS_DEAD_ACUTE);
	ENUM_TO_STR(DIKS_DEAD_BREVE);
	ENUM_TO_STR(DIKS_DEAD_CARON);
	ENUM_TO_STR(DIKS_DEAD_CEDILLA);
	ENUM_TO_STR(DIKS_DEAD_CIRCUMFLEX);
	ENUM_TO_STR(DIKS_DEAD_DIAERESIS);
	ENUM_TO_STR(DIKS_DEAD_DOUBLEACUTE);
	ENUM_TO_STR(DIKS_DEAD_GRAVE);
	ENUM_TO_STR(DIKS_DEAD_IOTA);
	ENUM_TO_STR(DIKS_DEAD_MACRON);
	ENUM_TO_STR(DIKS_DEAD_OGONEK);
	ENUM_TO_STR(DIKS_DEAD_SEMIVOICED_SOUND);
	ENUM_TO_STR(DIKS_DEAD_TILDE);
	ENUM_TO_STR(DIKS_DEAD_VOICED_SOUND);


	ENUM_TO_STR( DIKS_CUSTOM0);
	ENUM_TO_STR( DIKS_CUSTOM1);
	ENUM_TO_STR( DIKS_CUSTOM2);
	ENUM_TO_STR( DIKS_CUSTOM3);
	ENUM_TO_STR( DIKS_CUSTOM4);
	ENUM_TO_STR( DIKS_CUSTOM5);
	ENUM_TO_STR( DIKS_CUSTOM6);
	ENUM_TO_STR( DIKS_CUSTOM7);
	ENUM_TO_STR( DIKS_CUSTOM8);
	ENUM_TO_STR( DIKS_CUSTOM9);
	ENUM_TO_STR( DIKS_CUSTOM10);
	ENUM_TO_STR( DIKS_CUSTOM11);
	ENUM_TO_STR( DIKS_CUSTOM12);
	ENUM_TO_STR( DIKS_CUSTOM13);
	ENUM_TO_STR( DIKS_CUSTOM14);
	ENUM_TO_STR( DIKS_CUSTOM15);
	ENUM_TO_STR( DIKS_CUSTOM16);
	ENUM_TO_STR( DIKS_CUSTOM17);
	ENUM_TO_STR( DIKS_CUSTOM18);
	ENUM_TO_STR( DIKS_CUSTOM19);
	ENUM_TO_STR( DIKS_CUSTOM20);
	ENUM_TO_STR( DIKS_CUSTOM21);
	ENUM_TO_STR( DIKS_CUSTOM22);
	ENUM_TO_STR( DIKS_CUSTOM23);
	ENUM_TO_STR( DIKS_CUSTOM24);
	ENUM_TO_STR( DIKS_CUSTOM25);
	ENUM_TO_STR( DIKS_CUSTOM26);
	ENUM_TO_STR( DIKS_CUSTOM27);
	ENUM_TO_STR( DIKS_CUSTOM28);
	ENUM_TO_STR( DIKS_CUSTOM29);
	ENUM_TO_STR( DIKS_CUSTOM30);
	ENUM_TO_STR( DIKS_CUSTOM31);
	ENUM_TO_STR( DIKS_CUSTOM32);
	ENUM_TO_STR( DIKS_CUSTOM33);
	ENUM_TO_STR( DIKS_CUSTOM34);
	ENUM_TO_STR( DIKS_CUSTOM35);
	ENUM_TO_STR( DIKS_CUSTOM36);
	ENUM_TO_STR( DIKS_CUSTOM37);
	ENUM_TO_STR( DIKS_CUSTOM38);
	ENUM_TO_STR( DIKS_CUSTOM39);
	ENUM_TO_STR( DIKS_CUSTOM40);
	ENUM_TO_STR( DIKS_CUSTOM41);
	ENUM_TO_STR( DIKS_CUSTOM42);
	ENUM_TO_STR( DIKS_CUSTOM43);
	ENUM_TO_STR( DIKS_CUSTOM44);
	ENUM_TO_STR( DIKS_CUSTOM45);
	ENUM_TO_STR( DIKS_CUSTOM46);
	ENUM_TO_STR( DIKS_CUSTOM47);
	ENUM_TO_STR( DIKS_CUSTOM48);
	ENUM_TO_STR( DIKS_CUSTOM49);
	ENUM_TO_STR( DIKS_CUSTOM50);
	ENUM_TO_STR( DIKS_CUSTOM51);
	ENUM_TO_STR( DIKS_CUSTOM52);
	ENUM_TO_STR( DIKS_CUSTOM53);
	ENUM_TO_STR( DIKS_CUSTOM54);
	ENUM_TO_STR( DIKS_CUSTOM55);
	ENUM_TO_STR( DIKS_CUSTOM56);
	ENUM_TO_STR( DIKS_CUSTOM57);
	ENUM_TO_STR( DIKS_CUSTOM58);
	ENUM_TO_STR( DIKS_CUSTOM59);
	ENUM_TO_STR( DIKS_CUSTOM60);
	ENUM_TO_STR( DIKS_CUSTOM61);
	ENUM_TO_STR( DIKS_CUSTOM62);
	ENUM_TO_STR( DIKS_CUSTOM63);
	ENUM_TO_STR( DIKS_CUSTOM64);
	ENUM_TO_STR( DIKS_CUSTOM65);
	ENUM_TO_STR( DIKS_CUSTOM66);
	ENUM_TO_STR( DIKS_CUSTOM67);
	ENUM_TO_STR( DIKS_CUSTOM68);
	ENUM_TO_STR( DIKS_CUSTOM69);
	ENUM_TO_STR( DIKS_CUSTOM70);
	ENUM_TO_STR( DIKS_CUSTOM71);
	ENUM_TO_STR( DIKS_CUSTOM72);
	ENUM_TO_STR( DIKS_CUSTOM73);
	ENUM_TO_STR( DIKS_CUSTOM74);
	ENUM_TO_STR( DIKS_CUSTOM75);
	ENUM_TO_STR( DIKS_CUSTOM76);
	ENUM_TO_STR( DIKS_CUSTOM77);
	ENUM_TO_STR( DIKS_CUSTOM78);
	ENUM_TO_STR( DIKS_CUSTOM79);
	ENUM_TO_STR( DIKS_CUSTOM80);
	ENUM_TO_STR( DIKS_CUSTOM81);
	ENUM_TO_STR( DIKS_CUSTOM82);
	ENUM_TO_STR( DIKS_CUSTOM83);
	ENUM_TO_STR( DIKS_CUSTOM84);
	ENUM_TO_STR( DIKS_CUSTOM85);
	ENUM_TO_STR( DIKS_CUSTOM86);
	ENUM_TO_STR( DIKS_CUSTOM87);
	ENUM_TO_STR( DIKS_CUSTOM88);
	ENUM_TO_STR( DIKS_CUSTOM89);
	ENUM_TO_STR( DIKS_CUSTOM90);
	ENUM_TO_STR( DIKS_CUSTOM91);
	ENUM_TO_STR( DIKS_CUSTOM92);
	ENUM_TO_STR( DIKS_CUSTOM93);
	ENUM_TO_STR( DIKS_CUSTOM94);
	ENUM_TO_STR( DIKS_CUSTOM95);
	ENUM_TO_STR( DIKS_CUSTOM96);
	ENUM_TO_STR( DIKS_CUSTOM97);
	ENUM_TO_STR( DIKS_CUSTOM98);
	ENUM_TO_STR( DIKS_CUSTOM99);

	default:
		break;
	}
	return "unknown";
}
#endif

static ONDK_KEY_PressMode_t local_dfb_EventTypeToONDKType(DFBWindowEventType input)
{

	switch((HINT32)input)
	{
		case DIET_KEYPRESS	:	return eKeyPress_DOWN;
		case DIET_KEYRELEASE:	return eKeyPress_UP;
		case DIET_BUTTONPRESS:	return eKeyPress_MAX;
		case DIET_BUTTONRELEASE:return eKeyPress_MAX;
		default:
#if defined(CONFIG_DEBUG)
			ONDK_Error("[%s:%d] ONDK Unknown input(%X)\n", __FUNCTION__, __LINE__, input);
#endif
			return eKeyPress_MAX;
	}
}

static ONDK_KEY_Code_t local_dfb_EventKeyToONDKKey(DFBInputDeviceKeySymbol input)
{
	ONDK_KEY_Code_t nkey;

	switch((HINT32)input)
	{
	case DIKS_NULL: nkey = KEY_UNI_NULL; break;
	case DIKS_BACKSPACE: nkey = KEY_UNI_BACKSPACE; break;
	case DIKS_TAB: nkey = KEY_UNI_TAB; break;
	case DIKS_RETURN: nkey = KEY_UNI_RETURN; break;
	case DIKS_CANCEL: nkey = KEY_UNI_CANCEL; break;
	case DIKS_ESCAPE: nkey = KEY_UNI_ESCAPE; break;
	case DIKS_SPACE: nkey = KEY_UNI_SPACE; break;
	case DIKS_EXCLAMATION_MARK: nkey = KEY_EXCLAMATION_MARK; break;
	case DIKS_QUOTATION: nkey = KEY_QUOTATION; break;
	case DIKS_NUMBER_SIGN: nkey = KEY_NUMBER_SIGN; break;
	case DIKS_DOLLAR_SIGN: nkey = KEY_DOLLAR_SIGN; break;
	case DIKS_PERCENT_SIGN: nkey = KEY_PERCENT_SIGN; break;
	case DIKS_AMPERSAND: nkey = KEY_AMPERSAND; break;
	case DIKS_APOSTROPHE: nkey = KEY_APOSTROPHE; break;
	case DIKS_PARENTHESIS_LEFT: nkey = KEY_PARENTHESIS_LEFT; break;
	case DIKS_PARENTHESIS_RIGHT: nkey = KEY_PARENTHESIS_RIGHT; break;
	case DIKS_ASTERISK: nkey = KEY_ASTERISK; break;
	case DIKS_PLUS_SIGN: nkey = KEY_PLUS_SIGN; break;
	case DIKS_COMMA: nkey = KEY_COMMA; break;
	case DIKS_MINUS_SIGN: nkey = KEY_MINUS_SIGN; break;
	case DIKS_PERIOD: nkey = KEY_PERIOD; break;
	case DIKS_SLASH: nkey = KEY_SLASH; break;
	case DIKS_0: nkey = KEY_0 ; break;
	case DIKS_1: nkey = KEY_1; break;
	case DIKS_2: nkey = KEY_2; break;
	case DIKS_3: nkey = KEY_3; break;
	case DIKS_4: nkey = KEY_4; break;
	case DIKS_5: nkey = KEY_5; break;
	case DIKS_6: nkey = KEY_6; break;
	case DIKS_7: nkey = KEY_7; break;
	case DIKS_8: nkey = KEY_8; break;
	case DIKS_9: nkey = KEY_9; break;
	case DIKS_COLON: nkey = KEY_COLON; break;
	case DIKS_SEMICOLON: nkey = KEY_SEMICOLON; break;
	case DIKS_LESS_THAN_SIGN: nkey = KEY_LESS_THAN_SIGN; break;
	case DIKS_EQUALS_SIGN: nkey = KEY_EQUALS_SIGN; break;
	case DIKS_GREATER_THAN_SIGN: nkey = KEY_GREATER_THAN_SIGN; break;
	case DIKS_QUESTION_MARK: nkey = KEY_QUESTION_MARK; break;
	case DIKS_AT: nkey = KEY_AT; break;
	case DIKS_CAPITAL_A: nkey = KEY_CAPITAL_A; break;
	case DIKS_CAPITAL_B: nkey = KEY_CAPITAL_B; break;
	case DIKS_CAPITAL_C: nkey = KEY_CAPITAL_C; break;
	case DIKS_CAPITAL_D: nkey = KEY_CAPITAL_D; break;
	case DIKS_CAPITAL_E: nkey = KEY_CAPITAL_E; break;
	case DIKS_CAPITAL_F: nkey = KEY_CAPITAL_F; break;
	case DIKS_CAPITAL_G: nkey = KEY_CAPITAL_G; break;
	case DIKS_CAPITAL_H: nkey = KEY_CAPITAL_H; break;
	case DIKS_CAPITAL_I: nkey = KEY_CAPITAL_I; break;
	case DIKS_CAPITAL_J: nkey = KEY_CAPITAL_J; break;
	case DIKS_CAPITAL_K: nkey = KEY_CAPITAL_K; break;
	case DIKS_CAPITAL_L: nkey = KEY_CAPITAL_L; break;
	case DIKS_CAPITAL_M: nkey = KEY_CAPITAL_M; break;
	case DIKS_CAPITAL_N: nkey = KEY_CAPITAL_N; break;
	case DIKS_CAPITAL_O: nkey = KEY_CAPITAL_O; break;
	case DIKS_CAPITAL_P: nkey = KEY_CAPITAL_P; break;
	case DIKS_CAPITAL_Q: nkey = KEY_CAPITAL_Q; break;
	case DIKS_CAPITAL_R: nkey = KEY_CAPITAL_R; break;
	case DIKS_CAPITAL_S: nkey = KEY_CAPITAL_S; break;
	case DIKS_CAPITAL_T: nkey = KEY_CAPITAL_T; break;
	case DIKS_CAPITAL_U: nkey = KEY_CAPITAL_U; break;
	case DIKS_CAPITAL_V: nkey = KEY_CAPITAL_V; break;
	case DIKS_CAPITAL_W: nkey = KEY_CAPITAL_W; break;
	case DIKS_CAPITAL_X: nkey = KEY_CAPITAL_X; break;
	case DIKS_CAPITAL_Y: nkey = KEY_CAPITAL_Y; break;
	case DIKS_CAPITAL_Z: nkey = KEY_CAPITAL_Z; break;
	case DIKS_SQUARE_BRACKET_LEFT: nkey = KEY_SQUARE_BRACKET_LEFT; break;
	case DIKS_BACKSLASH: nkey = KEY_BACKSLASH; break;
	case DIKS_SQUARE_BRACKET_RIGHT: nkey = KEY_SQUARE_BRACKET_RIGHT; break;
	case DIKS_CIRCUMFLEX_ACCENT: nkey = KEY_CIRCUMFLEX_ACCENT; break;
	case DIKS_UNDERSCORE: nkey = KEY_UNDERSCORE; break;
	case DIKS_GRAVE_ACCENT: nkey = KEY_GRAVE_ACCENT; break;
	case DIKS_SMALL_A: nkey = KEY_SMALL_A; break;
	case DIKS_SMALL_B: nkey = KEY_SMALL_B; break;
	case DIKS_SMALL_C: nkey = KEY_SMALL_C; break;
	case DIKS_SMALL_D: nkey = KEY_SMALL_D; break;
	case DIKS_SMALL_E: nkey = KEY_SMALL_E; break;
	case DIKS_SMALL_F: nkey = KEY_SMALL_F; break;
	case DIKS_SMALL_G: nkey = KEY_SMALL_G; break;
	case DIKS_SMALL_H: nkey = KEY_SMALL_H; break;
	case DIKS_SMALL_I: nkey = KEY_SMALL_I; break;
	case DIKS_SMALL_J: nkey = KEY_SMALL_J; break;
	case DIKS_SMALL_K: nkey = KEY_SMALL_K; break;
	case DIKS_SMALL_L: nkey = KEY_SMALL_L; break;
	case DIKS_SMALL_M: nkey = KEY_SMALL_M; break;
	case DIKS_SMALL_N: nkey = KEY_SMALL_N; break;
	case DIKS_SMALL_O: nkey = KEY_SMALL_O; break;
	case DIKS_SMALL_P: nkey = KEY_SMALL_P; break;
	case DIKS_SMALL_Q: nkey = KEY_SMALL_Q; break;
	case DIKS_SMALL_R: nkey = KEY_SMALL_R; break;
	case DIKS_SMALL_S: nkey = KEY_SMALL_S; break;
	case DIKS_SMALL_T: nkey = KEY_SMALL_T; break;
	case DIKS_SMALL_U: nkey = KEY_SMALL_U; break;
	case DIKS_SMALL_V: nkey = KEY_SMALL_V; break;
	case DIKS_SMALL_W: nkey = KEY_SMALL_W; break;
	case DIKS_SMALL_X: nkey = KEY_SMALL_X; break;
	case DIKS_SMALL_Y: nkey = KEY_SMALL_Y; break;
	case DIKS_SMALL_Z: nkey = KEY_SMALL_Z; break;
	case DIKS_CURLY_BRACKET_LEFT: nkey = KEY_CURLY_BRACKET_LEFT; break;
	case DIKS_VERTICAL_BAR: nkey = KEY_VERTICAL_BAR; break;
	case DIKS_CURLY_BRACKET_RIGHT: nkey = KEY_CURLY_BRACKET_RIGHT; break;
	case DIKS_TILDE: nkey = KEY_TILDE; break;
	case DIKS_DELETE: nkey = KEY_UNI_DELETE; break;

	case DIKS_CURSOR_LEFT: 		nkey = KEY_ARROWLEFT; break;
	case DIKS_CURSOR_RIGHT: 	nkey = KEY_ARROWRIGHT; break;
	case DIKS_CURSOR_UP: 		nkey = KEY_ARROWUP; break;
	case DIKS_CURSOR_DOWN: 		nkey = KEY_ARROWDOWN; break;
	case DIKS_INSERT: nkey = KEY_SPECIAL_INSERT; break;
	case DIKS_HOME: nkey = KEY_SPECIAL_HOME; break;
	case DIKS_END: nkey = KEY_SPECIAL_END; break;
	case DIKS_PAGE_UP: nkey = KEY_PAGEUP ; break;
	case DIKS_PAGE_DOWN: nkey = KEY_PAGEDOWN; break;
	case DIKS_PRINT: nkey = KEY_SPECIAL_PRINT; break;
	case DIKS_PAUSE: nkey = KEY_PAUSE; break;
	case DIKS_OK: nkey = KEY_OK; break;
	case DIKS_SELECT: nkey = KEY_UNKNOWN; break;
	case DIKS_GOTO: nkey = KEY_UNKNOWN; break;
	case DIKS_CLEAR: nkey = KEY_SPECIAL_CLEAR; break;
	case DIKS_POWER: nkey = KEY_STANDBY; break;
	case DIKS_POWER2: nkey = KEY_UNKNOWN; break;
	case DIKS_OPTION: nkey = KEY_OPT; break;
	case DIKS_MENU: nkey = KEY_MENU; break;
	//case DIKS_HELP: nkey = KEY_HELP; break;
	case DIKS_INFO: nkey = KEY_INFO ; break;
	case DIKS_TIME: nkey = KEY_UNKNOWN; break;
	case DIKS_VENDOR: nkey = KEY_UNKNOWN; break;

	case DIKS_ARCHIVE: nkey = KEY_UNKNOWN; break;
	case DIKS_PROGRAM: nkey = KEY_PROGRAM; break;
	case DIKS_CHANNEL: nkey = KEY_UNKNOWN; break;
	case DIKS_FAVORITES: nkey = KEY_FAVORITES; break;
	case DIKS_EPG: nkey = KEY_GUIDE; break;
	case DIKS_PVR: nkey = KEY_MODE_PVR; break;
	case DIKS_MHP: nkey = KEY_MHP; break;
	//case DIKS_LANGUAGE: nkey = KEY_LANGUAGE; break;
	case DIKS_TITLE: nkey = KEY_UNKNOWN; break;
	case DIKS_SUBTITLE: nkey = KEY_SUBTITLE; break;
	case DIKS_ANGLE: nkey = KEY_UNKNOWN; break;
	case DIKS_ZOOM: nkey = KEY_WIDE; break;
	case DIKS_MODE: nkey = KEY_SPECIAL_MODE; break;
	case DIKS_KEYBOARD: nkey = KEY_SPECIAL_KEYBOARD; break;
	case DIKS_PC: nkey = KEY_SPECIAL_PC; break;
	case DIKS_SCREEN: nkey = KEY_SPECIAL_SCREEN; break;

//	case DIKS_TV: nkey = KEY_MODE_TV; break;
	case DIKS_TV: nkey = KEY_TVRADIO; break;
	case DIKS_TV2: nkey = KEY_UNKNOWN; break;
	case DIKS_VCR: nkey = KEY_UNKNOWN; break;
	case DIKS_VCR2: nkey = KEY_UNKNOWN; break;
	case DIKS_SAT: nkey = KEY_UNKNOWN; break;
	case DIKS_SAT2: nkey = KEY_UNKNOWN; break;
	case DIKS_CD: nkey = KEY_UNKNOWN; break;
	case DIKS_TAPE: nkey = KEY_UNKNOWN; break;
	case DIKS_RADIO: nkey = KEY_UNKNOWN; break;
	case DIKS_TUNER: nkey = KEY_UNKNOWN; break;
	case DIKS_PLAYER: nkey = KEY_PLAYLIST; break;
	case DIKS_TEXT: nkey = KEY_TEXT; break;
	case DIKS_DVD: nkey = KEY_MODE_DVD; break;
	case DIKS_AUX: nkey = KEY_SPECIAL_AUX; break;
	case DIKS_MP3: nkey = KEY_UNKNOWN; break;
	case DIKS_PHONE: nkey = KEY_UNKNOWN; break;
	case DIKS_AUDIO: nkey = KEY_AUDIO; break;
	case DIKS_VIDEO: nkey = KEY_UNKNOWN; break;

	case DIKS_INTERNET: nkey = KEY_UNKNOWN; break;
	case DIKS_MAIL: nkey = KEY_UNKNOWN; break;
	case DIKS_NEWS: nkey = KEY_UNKNOWN; break;
	case DIKS_DIRECTORY: nkey = KEY_UNKNOWN; break;
	case DIKS_LIST: nkey = KEY_UNKNOWN; break;
	case DIKS_CALCULATOR: nkey = KEY_UNKNOWN; break;
	case DIKS_MEMO: nkey = KEY_UNKNOWN; break;
	case DIKS_CALENDAR: nkey = KEY_UNKNOWN; break;
	case DIKS_EDITOR: nkey = KEY_UNKNOWN; break;

	case DIKS_RED: nkey = KEY_RED ; break;
	case DIKS_GREEN: nkey = KEY_GREEN; break;
	case DIKS_YELLOW: nkey = KEY_YELLOW; break;
	case DIKS_BLUE: nkey = KEY_BLUE; break;

	case DIKS_CHANNEL_UP: nkey = KEY_CH_PLUS; break;
	case DIKS_CHANNEL_DOWN: nkey = KEY_CH_MINUS; break;
	case DIKS_BACK: nkey = KEY_BACK; break;
	case DIKS_FORWARD: nkey = KEY_FORWARD; break;
	case DIKS_FIRST: nkey = KEY_UNKNOWN; break;
	case DIKS_LAST: nkey = KEY_BACK; break;
	case DIKS_VOLUME_UP: nkey = KEY_VOLUMEUP ; break;
	case DIKS_VOLUME_DOWN: nkey = KEY_VOLUMEDOWN; break;
	case DIKS_MUTE: nkey = KEY_MUTE; break;
	case DIKS_AB: nkey = KEY_UNKNOWN; break;
	case DIKS_PLAYPAUSE: nkey = KEY_PAUSE; break;
	case DIKS_PLAY: nkey = KEY_PLAY; break;
	case DIKS_STOP: nkey = KEY_STOP; break;
	case DIKS_RESTART: nkey = KEY_UNKNOWN; break;
	case DIKS_SLOW: nkey = KEY_SLOW; break;
	case DIKS_FAST: nkey = KEY_UNKNOWN; break;
	case DIKS_RECORD: nkey = KEY_RECORD; break;
	case DIKS_EJECT: nkey = KEY_UNKNOWN; break;
	case DIKS_SHUFFLE: nkey = KEY_UNKNOWN; break;
	case DIKS_REWIND: nkey = KEY_SPECIAL_REWIND; break;
	case DIKS_FASTFORWARD: nkey = KEY_SPECIAL_FASTFORWARD; break;
	case DIKS_PREVIOUS: nkey = KEY_UNKNOWN; break;
	case DIKS_NEXT: nkey = KEY_NEXT; break;
	case DIKS_BEGIN: nkey = KEY_UNKNOWN; break;

	case DIKS_DIGITS: nkey = KEY_DIGITS; break;
	case DIKS_TEEN: nkey = KEY_UNKNOWN; break;
	case DIKS_TWEN: nkey = KEY_UNKNOWN; break;

	case DIKS_BREAK: nkey = KEY_UNKNOWN; break;
	case DIKS_EXIT: nkey = KEY_EXIT; break;
	case DIKS_SETUP: nkey = KEY_SETUP; break;

	case DIKS_CURSOR_LEFT_UP: 		 nkey = KEY_ARROWLEFT; break;
	case DIKS_CURSOR_LEFT_DOWN: 	 nkey = KEY_ARROWRIGHT; break;
	case DIKS_CURSOR_UP_RIGHT: 		 nkey = KEY_ARROWUP; break;
	case DIKS_CURSOR_DOWN_RIGHT: 	 nkey = KEY_ARROWDOWN; break;

	case DIKS_PIP: nkey = KEY_UNKNOWN; break;
	case DIKS_SWAP: nkey = KEY_UNKNOWN; break;
	case DIKS_MOVE: nkey = KEY_UNKNOWN; break;
	case DIKS_FREEZE: nkey = KEY_FREEZE; break;

	case DIKS_F1: nkey = KEY_F1; break;
	case DIKS_F2: nkey = KEY_F2; break;
	case DIKS_F3: nkey = KEY_F3; break;
	case DIKS_F4: nkey = KEY_F4; break;
	case DIKS_F5: nkey = KEY_F5; break;
	case DIKS_F6: nkey = KEY_F6; break;
	case DIKS_F7: nkey = KEY_F7; break;
	case DIKS_F8: nkey = KEY_F8; break;
	case DIKS_F9: nkey = KEY_F9; break;
	case DIKS_F10: nkey = KEY_F10; break;
	case DIKS_F11: nkey = KEY_F11; break;
	case DIKS_F12: nkey = KEY_F12; break;

	case DIKS_SHIFT: nkey = KEY_UNI_SHIFT; break;
	case DIKS_CONTROL: nkey = KEY_UNI_CONTROL; break;
	case DIKS_ALT: nkey = KEY_UNI_ALT; break;
	case DIKS_ALTGR: nkey = KEY_UNI_ALTGR; break;
	case DIKS_META: nkey = KEY_UNI_META; break;
	case DIKS_SUPER: nkey = KEY_UNI_SUPER; break;
	case DIKS_HYPER: nkey = KEY_UNI_HYPER; break;

	case DIKS_CAPS_LOCK: nkey = KEY_CAPS_LOCK; break;
	case DIKS_NUM_LOCK: nkey = KEY_NUM_LOCK; break;
	case DIKS_SCROLL_LOCK: nkey = KEY_SCROLL_LOCK; break;

	case DIKS_DEAD_ABOVEDOT: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_ABOVERING: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_ACUTE: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_BREVE: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_CARON: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_CEDILLA: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_CIRCUMFLEX: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_DIAERESIS: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_DOUBLEACUTE: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_GRAVE: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_IOTA: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_MACRON: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_OGONEK: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_SEMIVOICED_SOUND: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_TILDE: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_VOICED_SOUND: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM0: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM1: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM2: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM3: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM4: nkey = KEY_CHLIST; break;
	case DIKS_CUSTOM5: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM6: nkey = KEY_CUSTOM_SMART_SEARCH; break;
	case DIKS_CUSTOM7: nkey = KEY_CUSTOM_MULTI_CUSTOM; break;
	case DIKS_CUSTOM8: nkey = KEY_CUSTOM_MULTI_CUSTOM_SUCCESS; break;
	case DIKS_CUSTOM9: nkey = KEY_CUSTOM_MULTI_CUSTOM_FAIL; break;
	case DIKS_CUSTOM10: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM11: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM12: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM13: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM14: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM15: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM16: nkey = KEY_CUSTOM_PRESET_CH; break;
	case DIKS_CUSTOM17: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM18: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM19: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM20: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM21: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM22: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM23: nkey = KEY_IP_BROWSER; break;
	case DIKS_CUSTOM24: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM25: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM26: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM27: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM28: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM29: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM30: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM31: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM32: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM33: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM34: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM35: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM36: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM37: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM38: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM39: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM40: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM41: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM42: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM43: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM44: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM45: nkey = KEY_VOD; break;
	case DIKS_CUSTOM46: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM47: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM48: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM49: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM50: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM51: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM52: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM53: nkey = KEY_CUSTOM_ETC; break;
	case DIKS_CUSTOM54: nkey = KEY_CUSTOM_RECOMMAND; break;
	case DIKS_CUSTOM55: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM56: nkey = KEY_NETWORK_TER; break;
	case DIKS_CUSTOM57: nkey = KEY_NETWORK_BS; break;
	case DIKS_CUSTOM58: nkey = KEY_NETWORK_CATV; break;
	case DIKS_CUSTOM59: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM60: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM61: nkey = KEY_DATA; break;
	case DIKS_CUSTOM62: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM63: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM64: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM65: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM66: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM67: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM68: nkey = KEY_NETFLIX; break;
	case DIKS_CUSTOM69: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM70: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM71: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM72: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM73: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM74: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM75: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM76: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM77: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM78: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM79: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM80: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM81: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM82: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM83: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM84: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM85: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM86: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM87: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM88: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM89: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM90: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM91: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM92: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM93: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM94: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM95: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM96: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM97: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM98: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM99: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM117: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM118: nkey = KEY_UNKNOWN; break;

	default:
		nkey = KEY_UNKNOWN;
		break;
	}


#if defined(CONFIG_DEBUG)
	if(nkey == KEY_UNKNOWN)
	{
		printf("[%s:%d] ODNK Unknown key(%X) - %s\n", __FUNCTION__, __LINE__, input, local_dfb_DBG_GetKeystring(input));
		ONDK_Error("\n\n\n\n\n\n\n\n\n\n\n\n===============================\n");
		ONDK_Error("[%s:%d] ONDK Unknown key(%X) - %s\n", __FUNCTION__, __LINE__, input, local_dfb_DBG_GetKeystring(input));
		ONDK_Error("[%s:%d] ONDK Unknown key(%X) - %s\n", __FUNCTION__, __LINE__, input, local_dfb_DBG_GetKeystring(input));
		ONDK_Error("[%s:%d] ONDK Unknown key(%X) - %s\n", __FUNCTION__, __LINE__, input, local_dfb_DBG_GetKeystring(input));
		ONDK_Error("[%s:%d] ONDK Unknown key(%X) - %s\n", __FUNCTION__, __LINE__, input, local_dfb_DBG_GetKeystring(input));
		ONDK_Error("[%s:%d] ONDK Unknown key(%X) - %s\n", __FUNCTION__, __LINE__, input, local_dfb_DBG_GetKeystring(input));
		ONDK_Error("===============================\n\n\n\n\n\n\n\n\n\n");
	}
	else
	{
		ONDK_Print("[%s:%d] ONDK DFB key(%X) - %s\n", __FUNCTION__, __LINE__, input, local_dfb_DBG_GetKeystring(input));
	}
#endif

	return nkey;
}


static HBOOL local_dfb_Event_FindWindow(void *data, void *userdata)
{
	ONDK_HEvtKey_t *pEvt = (ONDK_HEvtKey_t *)data;
	ONDKWindowHandle hWindow = (ONDKWindowHandle)userdata;

	if(pEvt->mWindow == hWindow)
	{
		return TRUE;
	}
	return FALSE;
}

static HInputRetType_e local_dfb_Event_KeyDownForeachHEvt(void *data, void *userdata, HINT32 dWindowID)
{
	ONDK_Result_t    		oErr = ONDK_RET_OK;
	ONDK_KEY_Code_t 		ulKeyCode = KEY_UNKNOWN;
	ONDK_KEY_PressMode_t	ulKeyMode = eKeyPress_MAX;
	ONDK_WINDOW_Position_e	ePosition = ONDK_MAX_WINDOW;
	HInputEvent_t	*pstEvt = (HInputEvent_t	*)userdata;
    ONDK_KEY_KeyNotifyCallback_t fn = (ONDK_KEY_KeyNotifyCallback_t)data;

	if(fn == NULL)
	{
		return eHINPUTRET_NONE;
	}

	ulKeyCode = local_dfb_EventKeyToONDKKey(pstEvt->key_symbol);
	ulKeyMode = local_dfb_EventTypeToONDKType(pstEvt->type);
    ONDK_Print("[%s:%d] window ID[%d]\n", __FUNCTION__, __LINE__,pstEvt->window_id);
	if(ulKeyMode == eKeyPress_MAX)
	{
		return eHINPUTRET_NONE;
	}
	if(dWindowID == ONDK_UPPER_WINDOWID)
		ePosition = ONDK_UPPER_WINDOW;
	else if(dWindowID == ONDK_LOWER_WINDOWID)
		ePosition = ONDK_LOWER_WINDOW;

	if((ePosition != ONDK_MAX_WINDOW) || (dWindowID == eAWINDOW_MHEGIB))
		oErr = fn(ePosition,ulKeyCode, ulKeyMode, eKeyDevice_RCU, (void*)pstEvt);

	switch(oErr)
	{
	case ONDK_MESSAGE_NONCONSUMED:
        ONDK_Print("[%s:%d]ONDK_MESSAGE_NONCONSUMED\n", __FUNCTION__, __LINE__);
 		return eHINPUTRET_PASS;

	case ONDK_MESSAGE_CONSUMED:
       ONDK_Print("[%s:%d]ONDK_MESSAGE_CONSUMED\n", __FUNCTION__, __LINE__);
 		return eHINPUTRET_CONSUMED;

	case ONDK_MESSAGE_BREAK: //gwm message 는 여기의 리턴 결과로 키를 처리하지 않는다.
    case ONDK_RET_OK: //gwm message 는 여기의 리턴 결과로 키를 처리하지 않는다.
    default:
		break;
	}
    ONDK_Print("[%s:%d]eHEVENT_NOT_USED\n", __FUNCTION__, __LINE__);

	return eHINPUTRET_NONE;
}


static void local_dfb_EventTask(void* args)
{
	ONDK_HEvtKey_t 	     *pEvt;
	HInputEvent_t		 stEvt;
	HInputRetType_e		consumed = eHINPUTRET_CONSUMED;
	HxList_t			*pList ;

	HUINT32				 ulCnt, ulEvtNum;
	HUINT32				 ulEvtArrayLen = 0;
	ONDK_HEvtKey_t		*pstEvtArray = NULL;

	pstEvtArray = (ONDK_HEvtKey_t *)HLIB_STD_MemCalloc(sizeof(ONDK_HEvtKey_t) * 16);
	if (NULL == pstEvtArray)
	{
		HxLOG_Error(">>> Memory allocation failed:\n");
		return;
	}
	ulEvtArrayLen = 16;

	s_ulKeyTaskId = HLIB_STD_GetTID();

	while(TRUE)
	{
		VK_SEM_Get(s_ulEventSem);

		ulEvtNum = HLIB_LIST_Length(s_pEventBuffer);
		if (ulEvtNum > ulEvtArrayLen)
		{
			ONDK_HEvtKey_t			*pstNewArray = NULL;

			pstNewArray = (ONDK_HEvtKey_t *)HLIB_STD_MemCalloc(sizeof(ONDK_HEvtKey_t) * (ulEvtNum + 16));
			if (NULL != pstNewArray)
			{
				ulEvtArrayLen = ulEvtNum + 16;
				if (NULL != pstEvtArray)
				{
					HLIB_STD_MemFree(pstEvtArray);
				}
				pstEvtArray = pstNewArray;
			}
		}

		for (ulCnt = 0, pList = s_pEventBuffer; (NULL != pList) && (ulCnt < ulEvtArrayLen); pList = HLIB_LIST_Next(pList))
		{
			pEvt = (ONDK_HEvtKey_t*)HLIB_LIST_Data(pList);
			if (NULL != pEvt)
			{
				HxSTD_MemCopy(&(pstEvtArray[ulCnt]), pEvt, sizeof(ONDK_HEvtKey_t));
				ulCnt++;
			}
		}

		VK_SEM_Release(s_ulEventSem);

		ulEvtNum = ulCnt;
		for (ulCnt = 0; ulCnt < ulEvtNum; ulCnt++)
		{
			pEvt = &(pstEvtArray[ulCnt]);

			HxSTD_MemSet(&stEvt, 0, sizeof(HInputEvent_t));
			if (HAPI_GetInputEvent(pEvt->mWindowId, &stEvt, 0) == TRUE)
			{
				HxLOG_Print("[%s:%d] Window ID[%d]\n",__FUNCTION__,__LINE__,pEvt->mWindowId);

				if(s_bKeyActive)
				{
					consumed = local_dfb_Event_KeyDownForeachHEvt(pEvt->pfnKeyCallback, &stEvt, pEvt->mWindowId);
#if defined(CONFIG_APUI_NATIVE_APP)
					// TODO: Temp code to resolve key lag
					consumed = eHINPUTRET_CONSUMED;
#endif
					stEvt.eInputReturnType = consumed;
				}
				else
				{
					stEvt.eInputReturnType = eHINPUTRET_PASS;
					consumed = eHINPUTRET_PASS;
					HxLOG_Print(HxANSI_COLOR_PURPLE("[%s:%d] Window ID[%d] eHINPUTRET_PASS..\n"),__FUNCTION__,__LINE__,pEvt->mWindowId);
				}
				//printf("_id%d = c%d \n", pEvt->mWindowId, consumed);
				HAPI_ReturnKeyEvent(pEvt->mWindowId, consumed, &stEvt);
			}
		}

		VK_TASK_Sleep(10);
	}
}

STATIC HBOOL	local_dfb_CompWindow(void *pvUser, void *pvData)
{
	ONDKWindowHandle	mWindow = (ONDKWindowHandle)pvUser;
	ONDK_HEvtKey_t		*evtInfo = (ONDK_HEvtKey_t*)pvData;

	if (evtInfo->mWindow == mWindow)
		return TRUE;

	return FALSE;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/


ONDK_Result_t	ONDK_KEY_Init(HINT32 argc, HCHAR **argv)
{
	HERROR	hErr = ERR_OK;

	hErr = VK_SEM_Create(&s_ulEventSem, "DFB_EVT_SEM", VK_SUSPENDTYPE_PRIORITY);
	if (hErr != VK_OK)
	{
		HxLOG_Critical("Error:: sem creation fail err = 0x%x\n", hErr);
		return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}

HUINT32	ONDK_KEY_RegisterKeyCallback(ONDKWindowHandle mWindow, ONDK_KEY_KeyNotifyCallback_t pfnKeyNotifyCallback)
{
	ONDK_HEvtKey_t 	*pEvt = (ONDK_HEvtKey_t*)ONDK_KEY_GetBuffer(mWindow);

	if(pEvt == NULL)
	{
		return 0;
	}

	VK_SEM_Get(s_ulEventSem);
	pEvt->pfnKeyCallback = pfnKeyNotifyCallback;
     ONDK_Print("[%s:%d] id [%d] \n",__FUNCTION__,__LINE__,pEvt->mWindowId);
	VK_SEM_Release(s_ulEventSem);

    return pEvt->mWindowId;
}

ONDKHumaxKeyHandle ONDK_KEY_GetBuffer(ONDKWindowHandle hWindow)
{
	HxList_t 		*pList = NULL;
	ONDK_HEvtKey_t 	*pEvt = NULL;

	VK_SEM_Get(s_ulEventSem);
	pList = HLIB_LIST_ForeachEx(s_pEventBuffer, local_dfb_Event_FindWindow, hWindow);
	if(pList != NULL)
	{
		pEvt = (ONDK_HEvtKey_t *)HLIB_LIST_Data(pList);
		ONDK_Assert(pEvt);
	}

	VK_SEM_Release(s_ulEventSem);

	return (ONDKHumaxKeyHandle)pEvt;
}


void ONDK_KEY_ReleaseBuffer(ONDKWindowHandle mWindow)
{
	VK_SEM_Get(s_ulEventSem);

	s_pEventBuffer = HLIB_LIST_RemoveIf(s_pEventBuffer, (void*)mWindow, local_dfb_CompWindow, HLIB_STD_MemFree_CB);

	VK_SEM_Release(s_ulEventSem);
}

void ONDK_KEY_SetBuffer(ONDKWindowHandle mWindow)
{
	HUINT32 	windowId = 0;
	HERROR		hErr = ERR_OK;
	HCHAR		name[64] = {0, };
	static 		HBOOL bTaskStart= FALSE;
	IDirectFBEventBuffer	*evtBuf = NULL;
	ONDK_HEvtKey_t 			*pEvt = NULL;

	evtBuf = NULL;
	pEvt = (ONDK_HEvtKey_t *)ONDK_Malloc(sizeof(ONDK_HEvtKey_t));
	ONDK_Assert(pEvt);

	pEvt->mWindow = mWindow;
	pEvt->mWindowId = ONDK_WINDOW_GetIDFromWindow(mWindow);
	pEvt->loop = TRUE;
	pEvt->pfnKeyCallback = NULL;
	pEvt->pDFBEvtBuffer = evtBuf;

	HxSTD_snprintf(name, 63, "task_%08x", windowId);

	if(bTaskStart == FALSE)
	{
		bTaskStart = TRUE;
		hErr = VK_TASK_Create(local_dfb_EventTask, ONDK_TASK_PRIORITY, ONDK_TASK_STACK_SIZE, name, NULL, &pEvt->thdId, 0);
	    if(hErr != VK_OK)
	    {
	    	HxLOG_Critical("Error:: task create error = %d\n", hErr);
			ONDK_Free(pEvt);
			return;
	    }
		VK_TASK_Start(pEvt->thdId);
	}
	VK_SEM_Get(s_ulEventSem);
	s_pEventBuffer = HLIB_LIST_Append(s_pEventBuffer, pEvt);
	ONDK_Assert(s_pEventBuffer);
	VK_SEM_Release(s_ulEventSem);

    ONDK_Print("[%s:%d] pEvt->mWindowId (%d)\n", __FUNCTION__, __LINE__,pEvt->mWindowId );
}


void	ONDK_KEY_Active(HBOOL bActive)
{
	VK_SEM_Get(s_ulEventSem);
	s_bKeyActive = bActive;
	VK_SEM_Release(s_ulEventSem);

}

HUINT32 ONDK_KEY_GetTaskId(void)
{
	return s_ulKeyTaskId;
}

