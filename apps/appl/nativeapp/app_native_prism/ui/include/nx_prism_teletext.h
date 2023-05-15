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
 * @file	  		nx_prism_subtitle.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef ____NX_PRISM_TELETEXT_H_
#define ____NX_PRISM_TELETEXT_H_
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include   	<ondk.h>
#include	<hlib.h>
#include	<oapi.h>
#include	<hapi.h>

#include <nx_common.h>
#include <nx_prism_app.h>

#include "nx_prism_definition.h"

/*******************************************************************/
/********************      Defines         			*************************/
/*******************************************************************/
#define		TELETEXT_SCREENWIDTH				720
#define		TELETEXT_SCREENHEIGHT				576


#define	TTX_NULL_MAGAZINE	0xFF
#define	TTX_NULL_PAGE		0xFF
#define	TTX_NULL_PAGENO		0xFFFF
#define	TTX_NULL_SUBCODE	0x3F7F


#define DATA_STREAMER_EBU_SVCSTART		11111
#define DATA_STREAMER_EBU_SVCSTOP		22222

/*******************************************************************/
/********************      Typedef        		 	 *************************/
/*******************************************************************/
/**@Brief Teletext Type. */
typedef enum tagTTX_TYPE
{
	eTTX_TYPE_DIGITAL = 0,		/**< Settop에서 경우 Normal로만 한다. (VBI Insertion 포함)*/
#if defined (CONFIG_MW_CH_ANALOG)
	eTTX_TYPE_ANALOG,			/**< DTV의 경우 Analog channel에서는 VBI data를 위해 VBI type으로 start 한다. */
#endif
	eTTX_TYPE_LAST
} TTX_Type_t;
/** @brief Context status */
typedef enum
{
	eTTX_Status_None = 0,
	eTTX_Status_Waiting,	/**< Start 시 Master Action이 아니여서 기다리는 상태 */
	eTTX_Status_Ready,		/**< Start 시 Master Action이기 때문에 PID Filter는 걸어 놓은 상태 */
	eTTX_Status_Running		/**< Master Action이 Show가 될 때 동작하는 상태로 PID filter가 걸린 상태에서
    								 Callback을 등록하여 Packet를 처리하는 상태 */
} TTX_Status_t;


typedef enum
{
	// Set: digit
	// Get: X
	eTTX_CONTROL_NUMERIC,

	// Set: red(0), green(1), yellow(2), blue(3)
	// Get: X
	eTTX_CONTROL_COLORKEY,

	// Set: up(0), down(1), left(2), right(3)
	// Get: X
	eTTX_CONTROL_ARROWKEY,

	// Set: On/Off Toggle
	// Get: HBOOL type
	eTTX_CONTROL_CANCEL,

	// Set: TTX_Mix_t type
	// Get: TTX_Mix_t type
	eTTX_CONTROL_MIX,

	// Set: On/Off Toggle
	// Get: HBOOL type
	eTTX_CONTROL_REVEAL,

	// Set: On/Off Toggle
	// Get: HBOOL type
	eTTX_CONTROL_SUBPAGE,

	// Set: TTX_DoubleSize_t type
	// Get: TTX_DoubleSize_t type
	eTTX_CONTROL_DOUBLE,

	// Set: On/Off Toggle
	// Get: HBOOL type
	eTTX_CONTROL_HOLD,

	// Set: Only request
	// Get: X
	eTTX_CONTROL_INDEX,

	// Set: TTX_Navigation_t type
	// Get: TTX_Navigation_t type
	eTTX_CONTROL_NAVI,

	// Set: TTX_Language_t type
	// Get: TTX_Language_t type
	eTTX_CONTROL_LANGUAGE,

	eTTX_CONTROL_LAST
} TTX_Control_t;



typedef enum
{
	eTTX_NAVI_DEFAULT,

	// "FASTEXT"
	eTTX_NAVI_FASTEXT	= eTTX_NAVI_DEFAULT,

	// Full level on feature. FLOF is the commercial name of FASTEXT.
	eTTX_NAVI_FLOF		= eTTX_NAVI_FASTEXT,

	// Table of Pages
	eTTX_NAVI_TOP,

	// Cycle Trace
	eTTX_NAVI_TRACE,

	eTTX_NAVI_LAST
} TTX_Navigation_t;	///< TTX Page navigation type!


typedef enum
{
	eTTX_LANG_LATIN_WEST_EU,
	eTTX_LANG_LATIN_EAST_EU,
	eTTX_LANG_HEBREW_ARABIC,
	eTTX_LANG_CYRILLIC,

	eTTX_LANG_LAST
} TTX_Language_t;

typedef enum
{
	eTTX_MIX_ON				= 0x00,

	eTTX_MIX_100_PERCENT	= 0x00,
	eTTX_MIX_90_PERCENT		= 0x20,
	eTTX_MIX_80_PERCENT		= 0x33,
	eTTX_MIX_70_PERCENT		= 0x4C,
	eTTX_MIX_60_PERCENT		= 0x66,
	eTTX_MIX_50_PERCENT		= 0x7F,
	eTTX_MIX_40_PERCENT		= 0x99,
	eTTX_MIX_30_PERCENT		= 0xB2,
	eTTX_MIX_20_PERCENT		= 0xCC,
	eTTX_MIX_10_PERCENT		= 0xE5,
	eTTX_MIX_0_PERCENT		= 0xFF,

	eTTX_MIX_OFF			= 0xFF,

	eTTX_MIX_LAST
} TTX_Mix_t;


typedef enum
{
	eTTX_DOUBLE_OFF,
	eTTX_DOUBLE_ON,

	eTTX_DOUBLE_FOCUS_ON_TOP	= eTTX_DOUBLE_ON,
	eTTX_DOUBLE_FOCUS_ON_BTM,

	eTTX_DOUBLE_LAST
} TTX_DoubleSize_t;


/**
 * 페이지를 화면에 그릴 때, 갱신 영역
 */
typedef enum
{
	eTTX_REFRESH_NONE			= 0x00000000,
	eTTX_REFRESH_CTRL_PANEL		= 0x00000001,	///< row = 0, col =  0 ~  7
	eTTX_REFRESH_TITLE			= 0x00000002,	///< row = 0, col =  8 ~ 31
	eTTX_REFRESH_TIME			= 0x00000004,	///< row = 0, col = 31 ~ 39
	eTTX_REFRESH_HEADER			= 0x00000007,	///< ctrl panel | title | time
	eTTX_REFRESH_BODY			= 0x00000010,	///< flash | conceal | normal
	eTTX_REFRESH_FLASH			= 0x00000100,	///< flash cell
	eTTX_REFRESH_CONCEAL		= 0x00000200,	///< conceal cell
	eTTX_REFRESH_ROW24			= 0x00001000,
	eTTX_REFRESH_SUB_BAR		= 0x00010000,
	eTTX_REFRESH_ALL			= 0xFFFFFFFF
} TTX_Refresh_t;


/**
 * TTX State
 */
typedef enum
{
	eTTX_STATE_NULL,

	eTTX_STATE_NORMAL			= 0x00000000,	// Normal 상태
	eTTX_STATE_SUBCODE			= 0x00000001,	// Subcode 모드 상태
	eTTX_STATE_INPUT			= 0x00000002,	// 사용자 입력(번호) 상태
	eTTX_STATE_SEARCH			= 0x00000004,	// 검색 상태
	eTTX_STATE_HOLD				= 0x00000008,	// Hold 상태(Pause or Picture)
	eTTX_STATE_CANCEL			= 0x00000010,	// Cancel Page
	eTTX_STATE_FLASH			= 0x00000020,	// Flash On/Off
	eTTX_STATE_REVEAL			= 0x00000040,	// Reveal
	eTTX_STATE_ROTATE_SUBPAGE	= 0x00000080,	// Sub-page Rotate 상태

	eTTX_STATE_EBU_SUBTITLE		= 0x00010000,	// EBU Subtitle Mode
	eTTX_STATE_VAR_FONT			= 0x00100000,	// Variable Font

	eTTX_STATE_TAKE_SCREEN		= 0x01000000,	// Screen을 차지하다.

	eTTX_STATE_LAST
} TTX_State_t;


/**
 * Model/View/Controller 구조의 Controller에 해당.
 *
 * mw_ttx.c(Controller)에서 컨트롤러를 상태에 맞게 조정하면,
 * ttx_graphics.c(View)에서 조정된 컨트롤러를 보고 그에 맞게 화면에 렌더링 한다.
 */
typedef struct tagTTX_Controller
{
	TTX_State_t			state;				///< TTX State

	TTX_Mix_t			mix;
	TTX_DoubleSize_t	doubleSize;
	TTX_Navigation_t	navi;
	TTX_Language_t		langGroup;

	// Input
	HUINT8				aucInput[5];		///< 사용자 입력

	TTX_Refresh_t		refresh;			///< refresh type
	HUINT16 			usFilterPageNum;	///< 현재 페이지
	HUINT16 			usFilterSubcode;	///< 현재 페이지의 서브코드

	HINT32				nMarginW;
	HINT32				nMarginH;
	HINT32				nCellWidth;
	HINT32				nCellHeight;

	HUINT32				ulElapsedSec;		///< User가 Key를 누른후 경과 시간

	HUINT32				ulProgress;
}TTX_Controller_t;



extern ONDKWindowHandle		g_hTeletextWinHandle;
/*******************************************************************/
/********************      API Functions   		*************************/
/*******************************************************************/
#define ____________API_FUNCTIONS_____________
ONDK_Result_t	NX_PRISM_Teletext_Proc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
ONDK_Result_t	NX_PRISM_TtxMenuUi_Proc (HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

void			NX_PRISM_TTxMenuObject_Create(void);// 리소스를 미리 로드한다.
void			NX_PRISM_Teletext_Init(void);
HERROR			NX_PRISM_Teletext_StartService(void);
HERROR			NX_PRISM_Teletext_StopService(void);
void			NX_PRISM_Teletext_deliveryPacket(HCHAR* datatype, HINT32 len, HUINT8 *p);
HERROR			NX_PRISM_Teletext_Decoding (HUINT8 *pucPES);

HERROR			NX_PRISM_Teletext_SetControl (TTX_Control_t control, HUINT32 ulArg);
HERROR			NX_PRISM_Teletext_GetControl (TTX_Control_t control, void *pvValue);

HERROR			NX_PRISM_Teletext_Show(HUINT8 ucMagNum,HUINT16 usPageNum, HUINT16 usSubcode, HBOOL bSbtl);
HERROR			NX_PRISM_Teletext_Hide(void);
HERROR			NX_PRISM_Teletext_Search (HUINT16 usPageNum, HUINT16 usSubcode, HINT32 offset, HUINT16 *pusPageNum, HUINT16 *pusSubcode );
HERROR			NX_PRISM_Teletext_GetPageAddress (HUINT8 *pucM, HUINT8 *pucX, HUINT16 *pusSubcode);
HERROR			NX_PRISM_Teletext_SetSutitleFontType(HBOOL bVariableFont);
HERROR			NX_PRISM_Teletext_FONT(HBOOL bVariableFont);
Handle_t 		NX_PRISM_Teletext_GetDecoderHandle (void);

void			NX_PRISM_Teletext_SetDataUnitType(HUINT8 EBU);

void			NX_PRISM_Teletext_CMD_Register(HUINT8 *cmdname);



#endif	// ____NX_PRISM_TELETEXT_H_

