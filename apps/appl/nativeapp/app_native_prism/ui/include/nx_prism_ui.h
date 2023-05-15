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
 * @file	  		nx_prism_ui.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_UI_H__
#define __NX_PRISM_UI_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <nx_common.h>
#include <nx_prism_live.h>
#include <nx_prism_search.h>
#include <nx_system_startlogo.h>
#include <nx_prism_definition.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/
typedef enum
{
	/* Menu Setting is used 0x00000000 ~ 0x00050000 */
	//eNxSettingDepth_0
	//eNxSettingDepth_1,
	//eNxSettingDepth_2,
	//eNxSettingDepth_3,
	//eNxSettingDepth_4,
	eMenuFrame_Window_Base				= 0x00060000,
	eMenuFrame_Window_PopUp				= 0x00070000,
	eMenuFrame_Window_Compnent			= 0x00080000,
	eMenuFrame_Window_Settings			= 0x00090000,
	eMenuFrame_Window_ProDetail			= 0x000A0000,
	eMenuFrame_Window_InfoHub			= 0x000B0000,
	eMenuFrame_Window_CHList			= 0x000C0000,
	eMenuFrame_Window_Live				= 0x000D0000,
	eMenuFrame_Window_Volume			= eMenuFrame_Window_Live,
	eMenuFrame_Window_Mute				= (eMenuFrame_Window_Live + 0x19),	//eMenuFrame_Window_Live + 25
	eMenuFrame_Window_Wide				= (eMenuFrame_Window_Live + 0x32),	//eMenuFrame_Window_Live + 50
	eMenuFrame_Window_Dolby				= (eMenuFrame_Window_Live + 0x4B),	//eMenuFrame_Window_Live + 75
	eMenuFrame_Window_SubttlUI			= (eMenuFrame_Window_Live + 0x64),	//eMenuFrame_Window_Live + 100
	eMenuFrame_Window_AudioUI			= (eMenuFrame_Window_Live + 0x7D),	//eMenuFrame_Window_Live + 125
	eMenuFrame_Window_Radio				= (eMenuFrame_Window_Live + 0x96),	//eMenuFrame_Window_Live + 150
	eMenuFrame_Window_LiveOpt			= 0x000E0000,
	eMenuFrame_Window_GUIDE				= 0x000F0000,
	eMenuFrame_Window_Dialogue 			= 0x00100000,
	eMenuFrame_Window_Dialogue_Statue 	= (eMenuFrame_Window_Dialogue),			//eMenuFrame_Window_Dialogue
	eMenuFrame_Window_Dialogue_Response	= (eMenuFrame_Window_Dialogue + 0x32),	//eMenuFrame_Window_Dialogue + 50
	eMenuFrame_Window_Dialogue_Confirm	= (eMenuFrame_Window_Dialogue + 0x64),	//eMenuFrame_Window_Dialogue + 100
	eMenuFrame_Window_Dialogue_Process	= (eMenuFrame_Window_Dialogue + 0x96),	//eMenuFrame_Window_Dialogue + 150
	eMenuFrame_Window_Dialogue_Rsv		= (eMenuFrame_Window_Dialogue + 0xC8),	//eMenuFrame_Window_Dialogue + 200
	eMenuFrame_Window_Dialogue_RsvConflict		= (eMenuFrame_Window_Dialogue + 0xFA),	//eMenuFrame_Window_Dialogue + 250
	eMenuFrame_Window_KeyBoard			= 0x00200000,
	eMenuFrame_Window_Schedule			= 0x00300000,
	eMenuFrame_Window_ScheduleOpt		= (eMenuFrame_Window_Schedule + 0x64),	//eMenuFrame_Window_Schedule + 100
	eMenuFrame_Window_ScheduleRepeat 	= (eMenuFrame_Window_Schedule + 0xC8),	//eMenuFrame_Window_Schedule + 200
	eMenuFrame_Window_ScheduleAdd		= 0x00400000,
	eMenuFrame_Window_Recordings		= 0x00500000,
	eMenuFrame_Window_Install			= 0x00600000,
	eMenuFrame_Window_System			= 0x00700000,
	eMenuFrame_Window_Playbar			= 0x00800000,
	eMenuFrame_Window_Media				= 0x00900000,
	eMenuFrame_Window_Shahid			= 0x00A00000,
	eMenuFrame_Window_Shahid_Detail		= 0x00B00000,
	eMenuFrame_Window_Shahid_Player		= 0x00C00000,
	eMenuFrame_Window_System_ScreenSaver	= (eMenuFrame_Window_System), 		//eMenuFrame_Window_System ~ 49
	eMenuFrame_Window_SmartSearch		= (eMenuFrame_Window_System + 0x10),	//eMenuFrame_Window_System + 16
	eMenuFrame_Window_HiddenFusing		= (eMenuFrame_Window_System + 0x20),	//eMenuFrame_Window_System + 10
	eMenuFrame_Window_HiddenCountry		= (eMenuFrame_Window_System + 0x2A),	//eMenuFrame_Window_System + 10
	eMenuFrame_Window_KeywordSearch		= (eMenuFrame_Window_System + 0x3A),	//eMenuFrame_Window_System + 16

	eMenuMainItem_Last,
} MenuFrameId_t;


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/

typedef enum {
	NX_ALIALIGN_NONE = 0x00000000,
	NX_ALIALIGN_LEFT = 0x00000001,
	NX_ALIALIGN_RIGHT = 0x00000002,
	NX_ALIALIGN_TOP	= 0x00000004,
	NX_ALIALIGN_BOTTOM = 0x00000008,
	NX_ALIALIGN_Max
} NX_ANI_AlignType;


typedef enum
{
	eNX_DLG_CLICKED_BTN_LEFT,
	eNX_DLG_CLICKED_BTN_MIDDLE,
	eNX_DLG_CLICKED_BTN_RIGHT,
	eNX_DLG_CLICKED_BTN_NONE,
} NX_DLG_ClickedButton_t;


typedef enum
{
	eNX_DLG_MultiSel_OSD,
	eNX_DLG_MultiSel_List,
} NX_DLG_MultiSelRect_t;


typedef enum
{
	eNX_UCT_None = 0,
	eNX_UCT_Live,
	eNX_UCT_Chlist,
	eNX_UCT_Infohub,
	eNX_UCT_Menu,
	eNX_UCT_Guide,
	eNX_UCT_Meida,
	eNX_UCT_Record,
	eNX_UCT_TSR,
	eNX_UCT_Schedule,
	eNX_UCT_Schedule_Edit,
	eNX_UCT_Menu_FavChList,
	eNX_UCT_Menu_EditChList,
	eNX_UCT_Shahid,

	// TODO: Add Here

} NX_UI_CallerType_t;


typedef enum
{
	eNX_PANEL_STR_Starting,
	eNX_PANEL_STR_NoChannel,
	eNX_PANEL_STR_Mute,
	eNX_PANEL_STR_Volume,
	eNX_PANEL_STR_DCN,
	eNX_PANEL_STR_Wizard,
	eNX_PANEL_STR_Guide,
	eNX_PANEL_STR_Menu,
	eNX_PANEL_STR_ChList,
	eNX_PANEL_STR_Settings,

	eNX_PANEL_STR_Recordings,
	eNX_PANEL_STR_StartRec,

	eNX_PANEL_STR_Media,
	eNX_PANEL_STR_Video,
	eNX_PANEL_STR_Music,
	eNX_PANEL_STR_Photo,
	eNX_PANEL_STR_MediaPlay,

	eNX_PANEL_STR_Play,
	eNX_PANEL_STR_Pause,
	eNX_PANEL_STR_FRX,
	eNX_PANEL_STR_FFX,

	eNX_PANEL_STR_Portal,
	eNX_PANEL_STR_TVApps,

	eNX_PANEL_STR_Max
} NXUI_PANEL_STR_Type_t;

typedef struct
{
	/* message Dialog�� ����ϱ�?�� �� ä���ֿ��� �ϴ� ����. */
	HUINT32			ulDlgWhoseId;			// ������ app ���� Dialog â�� 2�� �뵵�� �������?�� ���?���? ex> medialist ���� ���� ����, ���� Ȯ��
	HUINT32			ulDlgAttributes;		/**< MODAL, Timer, Title ��. */
	HUINT32			ulProgressStatus;		// AP_ProcessingProgressMessage_ProtoProc ������ ���ǹǷ� ���� UI�� �ƴϸ� �ʱ�ȭ ���� �ʾƵ� ����
	HUINT8			*pszDialogTitle;		/**< string of title */
	HUINT8			*pszDialogMsg;			/**< string of message */
	ONDK_GWM_Callback_t	parentAppCallback;	/**< parent application */

	/* �ڵ����� ������. ���� �ʿ����? */
	ONDK_GWM_Callback_t	thisAppCallback;	/**< self */
	HUINT32			ulTimeout;				/**< time for kill timer */
	HUINT8			*pszButtonL;			/**< button text */
	HUINT8			*pszButtonR;			/**< button text */
	HUINT8			*pszButtonM;			/**< button text */
} NXUI_DialogInstance_t;


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

#define	NX_COMMON_DIALOGUE_EXP_TIMEOUT			(3*NX_COMMON_TIME_SECOND)	/* 3 sec */

/* Dialog Mask Value */
// Time Out
#define	NX_DLG_TIMEOUT_MASK						0x000000FF
#define	NX_DLG_TIMEOUT_NOTIMEOUT				0x00000000
#define	NX_DLG_TIMEOUT_1SEC						0x00000001
#define	NX_DLG_TIMEOUT_2SEC						0x00000002
#define	NX_DLG_TIMEOUT_3SEC						0x00000003
#define	NX_DLG_TIMEOUT_4SEC						0x00000004
#define	NX_DLG_TIMEOUT_5SEC						0x00000005
#define	NX_DLG_TIMEOUT_10SEC					0x0000000A
#define	NX_DLG_TIMEOUT_20SEC					0x00000014
#define	NX_DLG_TIMEOUT_30SEC					0x0000001E
#define	NX_DLG_TIMEOUT_40SEC					0x00000028
#define	NX_DLG_TIMEOUT_60SEC					0x0000003C
#define	NX_DLG_TIMEOUT_2MIN						0x00000078

// Button Type
#define	NX_DLG_BUTTONTYPE_MASK					0x00000F00
#define	NX_DLG_NO_BUTTON						0x00000000
#define	NX_DLG_OK								0x00000100
#define	NX_DLG_CANCEL							0x00000200
#define	NX_DLG_OK_CANCEL						0x00000300
#define	NX_DLG_CUSTOM_1B						0x00000400
#define	NX_DLG_CUSTOM_LR						0x00000500
#define	NX_DLG_CUSTOM_3B						0x00000600
#define	NX_DLG_YES_NO							0x00000C00
#define	NX_DLG_CLOSE							0x00000E00

//
#define	NX_DLG_TYPE_NO_EXIT						0x00001000
#define	NX_DLG_TYPE_TITLE						0x00002000
#define	NX_DLG_TYPE_MESSAGE						0x00004000
#define	NX_DLG_TYPE_BLKPINCODE					0x00008000
#define	NX_DLG_TYPE_FOCUS_NO					0x00010000
#define	NX_DLG_TYPE_FOCUS_CANCEL				0x00020000
#define	NX_DLG_TYPE_FOCUS_RIGHT					0x00040000
#define	NX_DLG_TYPE_NOT_BTN_CLICK_MASK			0x00080000

#define	NX_DLG_TYPE_OSD_FULL					0x00100000	// FULL OSD
#define	NX_DLG_TYPE_BG_DIM						0x00200000	// Background DIM
#define	NX_DLG_TYPE_NO_STANDBY					0x00400000
#define	NX_DLG_SUICIDE_MODE						0x01000000

// Processing Type
#define	NX_DLG_TYPE_PM_ANIMAION					0x02000000
#define	NX_DLG_TYPE_PM_BAR						0x04000000
#define	NX_DLG_TYPE_PM_SMALL					0x08000000


/******************************************************************************/
#define __TO_SETTING_OBJECTS________________________________________
/******************************************************************************/

#define	NX_PRISM_SETTINGS_WINDOW_WIDTH						NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_SETTINGS_WINDOW_HEIGHT						NX_PRISM_OSD_VERTICAL
#define	NX_PRISM_SETTINGS_WINDOW_X							(NX_PRISM_OSD_HORIZONTAL/2-NX_PRISM_SETTINGS_WINDOW_WIDTH/2)
#define	NX_PRISM_SETTINGS_WINDOW_Y							(NX_PRISM_OSD_VERTICAL/2-NX_PRISM_SETTINGS_WINDOW_HEIGHT/2)

#define	NX_PRISM_SETTINGS_TEXT_LS_60						60 // TEXT Line Space
#define	NX_PRISM_SETTINGS_CURSOR_MG							5


/******************************************************************************/
#define __TO_1DEPTH_BACKGROUND_OBJECTS________________________________________
/******************************************************************************/

#define	NX_PRISM_UI_FRAME_BASE_ID							eMenuFrame_Window_Base

#define	NX_PRISM_UI_BACKGROUND_BOX_ID						(NX_PRISM_UI_FRAME_BASE_ID + 1)

#define	NX_PRISM_UI_BACKGROUND_BOX_X						0
#define	NX_PRISM_UI_BACKGROUND_BOX_Y						0
#define	NX_PRISM_UI_BACKGROUND_BOX_WIDTH					NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_UI_BACKGROUND_BOX_HEIGHT					NX_PRISM_OSD_VERTICAL


/******************************************************************************/
#define __TO_X_DEPTH_NAVIGATION_OBJECTS_______________________________________
/******************************************************************************/
#define	NX_PRISM_UI_SET_WINDOW_W							NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_UI_SET_WINDOW_H							NX_PRISM_OSD_VERTICAL
#define	NX_PRISM_UI_SET_WINDOW_X							0
#define	NX_PRISM_UI_SET_WINDOW_Y							0

#define	NX_PRISM_UI_SET_TITLE_ID							(NX_PRISM_UI_FRAME_BASE_ID + 2)
#define	NX_PRISM_UI_SET_TITLE_X								60
#define	NX_PRISM_UI_SET_TITLE_Y								28
#define	NX_PRISM_UI_SET_TITLE_WIDTH							1020
#define	NX_PRISM_UI_SET_TITLE_HEIGHT						53

#define	NX_PRISM_UI_SET_CUR_DATE_ID							(NX_PRISM_UI_FRAME_BASE_ID + 3)
#define	NX_PRISM_UI_SET_CUR_DATE_X							1100
#define	NX_PRISM_UI_SET_CUR_DATE_Y							29
#define	NX_PRISM_UI_SET_CUR_DATE_WIDTH						120
#define	NX_PRISM_UI_SET_CUR_DATE_HEIGHT						20

#define	NX_PRISM_UI_SET_CUR_TIME_ID							(NX_PRISM_UI_FRAME_BASE_ID + 4)
#define	NX_PRISM_UI_SET_CUR_TIME_X							1050
#define	NX_PRISM_UI_SET_CUR_TIME_Y							51
#define	NX_PRISM_UI_SET_CUR_TIME_WIDTH						170
#define	NX_PRISM_UI_SET_CUR_TIME_HEIGHT						24

#define	NX_PRISM_UI_SET_TITLE_LINE_ID						(NX_PRISM_UI_FRAME_BASE_ID + 5)
#define	NX_PRISM_UI_SET_TITLE_LINE_X						60
#define	NX_PRISM_UI_SET_TITLE_LINE_Y						87
#define	NX_PRISM_UI_SET_TITLE_LINE_WIDTH					1160
#define	NX_PRISM_UI_SET_TITLE_LINE_HEIGHT					1

#define	NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID					(NX_PRISM_UI_FRAME_BASE_ID + 6)

#define NX_PRISM_UI_SET_HELP_TEXT_ID						(NX_PRISM_UI_FRAME_BASE_ID + 7)
#define NX_PRISM_UI_SET_HELP_TEXT_X							70
#define NX_PRISM_UI_SET_HELP_TEXT_Y							670
#define NX_PRISM_UI_SET_HELP_TEXT_WIDTH						740
#define NX_PRISM_UI_SET_HELP_TEXT_HEIGHT					0
#define NX_PRISM_UI_SET_HELP_TEXT_LS						32


/******************************************************************************/
#define __TO_X_DEPTH_NAVIGATION_CURSOR_OBJECTS________________________________
/******************************************************************************/
#define	NX_PRISM_UI_MENU_ID									eMenuFrame_Window_Settings

#define	NX_PRISM_UI_MENU_MAINITEM_CURSOR_ID					(NX_PRISM_UI_MENU_ID + 1)
#define	NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH				730
#define	NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT				55
#define	NX_PRISM_UI_MENU_MAINITEM_CURSOR_LINESPACE			NX_PRISM_SETTINGS_TEXT_LS_60

#define	NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB		430

/* mainitem 01 position */
#define	NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_ID			(NX_PRISM_UI_MENU_ID + 2)
#define	NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_WIDTH		NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH
#define	NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_HEIGHT		NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT
#define	NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X			60
#define	NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_Y			103

/* mainitem 02 position */
#define	NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_ID			(NX_PRISM_UI_MENU_ID + 3)
#define	NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_WIDTH		NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH
#define	NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_HEIGHT		NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT
#define	NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_X			NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X
#define	NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_Y			(NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_Y+NX_PRISM_UI_MENU_MAINITEM_CURSOR_LINESPACE)

/* mainitem 03 position */
#define	NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_ID			(NX_PRISM_UI_MENU_ID + 4)
#define	NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_WIDTH		NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH
#define	NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_HEIGHT		NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT
#define	NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_X			NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X
#define	NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_Y			(NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_Y+NX_PRISM_UI_MENU_MAINITEM_CURSOR_LINESPACE)

/* mainitem 04 position */
#define	NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_ID			(NX_PRISM_UI_MENU_ID + 5)
#define	NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_WIDTH		NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH
#define	NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_HEIGHT		NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT
#define	NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_X			NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X
#define	NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_Y			(NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_Y+NX_PRISM_UI_MENU_MAINITEM_CURSOR_LINESPACE)

/* mainitem 05 position */
#define	NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_ID			(NX_PRISM_UI_MENU_ID + 6)
#define	NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_WIDTH		NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH
#define	NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_HEIGHT		NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT
#define	NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_X			NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X
#define	NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_Y			(NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_Y+NX_PRISM_UI_MENU_MAINITEM_CURSOR_LINESPACE)

/* mainitem 06 position */
#define	NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_ID			(NX_PRISM_UI_MENU_ID + 7)
#define	NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_WIDTH		NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH
#define	NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_HEIGHT		NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT
#define	NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_X			NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X
#define	NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_Y			(NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_Y+NX_PRISM_UI_MENU_MAINITEM_CURSOR_LINESPACE)

/* mainitem 07 position */
#define	NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_ID			(NX_PRISM_UI_MENU_ID + 8)
#define	NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_WIDTH		NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH
#define	NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_HEIGHT		NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT
#define	NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_X			NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X
#define	NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_Y			(NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_Y+NX_PRISM_UI_MENU_MAINITEM_CURSOR_LINESPACE)

/* mainitem 08 position */
#define	NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_ID			(NX_PRISM_UI_MENU_ID + 9)
#define	NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_WIDTH		NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH
#define	NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_HEIGHT		NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT
#define	NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_X			NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X
#define	NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_Y			(NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_Y+NX_PRISM_UI_MENU_MAINITEM_CURSOR_LINESPACE)

/* mainitem 09 position */
#define	NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_ID			(NX_PRISM_UI_MENU_ID + 10)
#define	NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_WIDTH		NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH
#define	NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_HEIGHT		NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT
#define	NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_X			NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X
#define	NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_Y			(NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_Y+NX_PRISM_UI_MENU_MAINITEM_CURSOR_LINESPACE)

/* mainitem 10 position */
#define	NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_ID			(NX_PRISM_UI_MENU_ID + 11)
#define	NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_WIDTH		NX_PRISM_UI_MENU_MAINITEM_CURSOR_WIDTH
#define	NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_HEIGHT		NX_PRISM_UI_MENU_MAINITEM_CURSOR_HEIGHT
#define	NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_X			NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X
#define	NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_Y			(NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_Y+NX_PRISM_UI_MENU_MAINITEM_CURSOR_LINESPACE)

#define	NX_PRISM_UI_MENU_MAINITEM_TEXT_ID					1
#define	NX_PRISM_UI_MENU_MAINITEM_TEXT_SUB_ID				2

#define	NX_PRISM_UI_MENU_MAINITEM_TEXT_X_TAB				18
#define	NX_PRISM_UI_MENU_MAINITEM_TEXT_Y_TAB				(37 - 25)
#define	NX_PRISM_UI_MENU_MAINITEM_TEXT_WIDTH				670

#define	NX_PRISM_UI_MENU_MAINITEM_ICON_X_TAB				28
#define	NX_PRISM_UI_MENU_MAINITEM_ICON_Y_TAB				18
#define	NX_PRISM_UI_MENU_MAINITEM_ICON_WIDTH				13
#define	NX_PRISM_UI_MENU_MAINITEM_ICON_HEIGHT				20

#define	NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_ID			(NX_PRISM_UI_MENU_ID + 12)
#define	NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_X_TAB			702
#define	NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_Y_TAB			18
#define	NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_WIDTH			13
#define	NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_HEIGHT		20

#define	NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID				(NX_PRISM_UI_MENU_ID + 13)
#define	NX_PRISM_UI_MENU_MAINITEM_SUBLIST_X					810
#define	NX_PRISM_UI_MENU_MAINITEM_SUBLIST_Y					103
#define	NX_PRISM_UI_MENU_MAINITEM_SUBLIST_WIDTH				425
#define	NX_PRISM_UI_MENU_MAINITEM_SUBLIST_HEIGHT			600

#define	NX_PRISM_UI_MENU_MAINITEM_SUBLIST_TEXT_OFFSET_X		18
#define	NX_PRISM_UI_MENU_MAINITEM_SUBLIST_TEXT_OFFSET_Y		(37-27)

#define	NX_PRISM_UI_MENU_MAINITEM2DTH_SUBLIST_TAB			750


/******************************************************************************/
#define __TO_X_DEPTH_MENU_CURSOR_OBJECTS______________________________________
/******************************************************************************/

/* leaf application */
#define	NX_PRISM_UI_MENU_SUBITEM_WIDTH						750
#define	NX_PRISM_UI_MENU_SUBITEM_HEIGHT						60

/* sub button 01 */
#define	NX_PRISM_UI_MENU_SUBITEM_BTN01_ID					(NX_PRISM_UI_MENU_ID + 14)

#define	NX_PRISM_UI_MENU_SUBITEM_BTN01_WIDTH				NX_PRISM_UI_MENU_SUBITEM_WIDTH
#define	NX_PRISM_UI_MENU_SUBITEM_BTN01_HEIGHT				NX_PRISM_UI_MENU_SUBITEM_HEIGHT
#define	NX_PRISM_UI_MENU_SUBITEM_BTN01_X					60
#define	NX_PRISM_UI_MENU_SUBITEM_BTN01_Y					103

/* sub button 02 */
#define	NX_PRISM_UI_MENU_SUBITEM_BTN02_ID					(NX_PRISM_UI_MENU_ID + 15)

#define	NX_PRISM_UI_MENU_SUBITEM_BTN02_WIDTH				NX_PRISM_UI_MENU_SUBITEM_WIDTH
#define	NX_PRISM_UI_MENU_SUBITEM_BTN02_HEIGHT				NX_PRISM_UI_MENU_SUBITEM_HEIGHT
#define	NX_PRISM_UI_MENU_SUBITEM_BTN02_X					NX_PRISM_UI_MENU_SUBITEM_BTN01_X
#define	NX_PRISM_UI_MENU_SUBITEM_BTN02_Y					(NX_PRISM_UI_MENU_SUBITEM_BTN01_Y + NX_PRISM_UI_MENU_SUBITEM_HEIGHT)

/* sub button 03 */
#define	NX_PRISM_UI_MENU_SUBITEM_BTN03_ID					(NX_PRISM_UI_MENU_ID + 16)

#define	NX_PRISM_UI_MENU_SUBITEM_BTN03_WIDTH				NX_PRISM_UI_MENU_SUBITEM_WIDTH
#define	NX_PRISM_UI_MENU_SUBITEM_BTN03_HEIGHT				NX_PRISM_UI_MENU_SUBITEM_HEIGHT
#define	NX_PRISM_UI_MENU_SUBITEM_BTN03_X					NX_PRISM_UI_MENU_SUBITEM_BTN01_X
#define	NX_PRISM_UI_MENU_SUBITEM_BTN03_Y					(NX_PRISM_UI_MENU_SUBITEM_BTN02_Y + NX_PRISM_UI_MENU_SUBITEM_HEIGHT)

/* sub button 04 */
#define	NX_PRISM_UI_MENU_SUBITEM_BTN04_ID					(NX_PRISM_UI_MENU_ID + 17)

#define	NX_PRISM_UI_MENU_SUBITEM_BTN04_WIDTH				NX_PRISM_UI_MENU_SUBITEM_WIDTH
#define	NX_PRISM_UI_MENU_SUBITEM_BTN04_HEIGHT				NX_PRISM_UI_MENU_SUBITEM_HEIGHT
#define	NX_PRISM_UI_MENU_SUBITEM_BTN04_X					NX_PRISM_UI_MENU_SUBITEM_BTN01_X
#define	NX_PRISM_UI_MENU_SUBITEM_BTN04_Y					(NX_PRISM_UI_MENU_SUBITEM_BTN03_Y + NX_PRISM_UI_MENU_SUBITEM_HEIGHT)

/* sub button 05 */
#define	NX_PRISM_UI_MENU_SUBITEM_BTN05_ID					(NX_PRISM_UI_MENU_ID + 18)

#define	NX_PRISM_UI_MENU_SUBITEM_BTN05_WIDTH				NX_PRISM_UI_MENU_SUBITEM_WIDTH
#define	NX_PRISM_UI_MENU_SUBITEM_BTN05_HEIGHT				NX_PRISM_UI_MENU_SUBITEM_HEIGHT
#define	NX_PRISM_UI_MENU_SUBITEM_BTN05_X					NX_PRISM_UI_MENU_SUBITEM_BTN01_X
#define	NX_PRISM_UI_MENU_SUBITEM_BTN05_Y					(NX_PRISM_UI_MENU_SUBITEM_BTN04_Y + NX_PRISM_UI_MENU_SUBITEM_HEIGHT)

/* sub button 06 */
#define	NX_PRISM_UI_MENU_SUBITEM_BTN06_ID					(NX_PRISM_UI_MENU_ID + 19)

#define	NX_PRISM_UI_MENU_SUBITEM_BTN06_WIDTH				NX_PRISM_UI_MENU_SUBITEM_WIDTH
#define	NX_PRISM_UI_MENU_SUBITEM_BTN06_HEIGHT				NX_PRISM_UI_MENU_SUBITEM_HEIGHT
#define	NX_PRISM_UI_MENU_SUBITEM_BTN06_X					NX_PRISM_UI_MENU_SUBITEM_BTN01_X
#define	NX_PRISM_UI_MENU_SUBITEM_BTN06_Y					(NX_PRISM_UI_MENU_SUBITEM_BTN05_Y + NX_PRISM_UI_MENU_SUBITEM_HEIGHT)

/* sub button 07 */
#define	NX_PRISM_UI_MENU_SUBITEM_BTN07_ID					(NX_PRISM_UI_MENU_ID + 20)

#define	NX_PRISM_UI_MENU_SUBITEM_BTN07_WIDTH				NX_PRISM_UI_MENU_SUBITEM_WIDTH
#define	NX_PRISM_UI_MENU_SUBITEM_BTN07_HEIGHT				NX_PRISM_UI_MENU_SUBITEM_HEIGHT
#define	NX_PRISM_UI_MENU_SUBITEM_BTN07_X					NX_PRISM_UI_MENU_SUBITEM_BTN01_X
#define	NX_PRISM_UI_MENU_SUBITEM_BTN07_Y					(NX_PRISM_UI_MENU_SUBITEM_BTN06_Y + NX_PRISM_UI_MENU_SUBITEM_HEIGHT)

/* sub button 08 */
#define	NX_PRISM_UI_MENU_SUBITEM_BTN08_ID					(NX_PRISM_UI_MENU_ID + 21)

#define	NX_PRISM_UI_MENU_SUBITEM_BTN08_WIDTH				NX_PRISM_UI_MENU_SUBITEM_WIDTH
#define	NX_PRISM_UI_MENU_SUBITEM_BTN08_HEIGHT				NX_PRISM_UI_MENU_SUBITEM_HEIGHT
#define	NX_PRISM_UI_MENU_SUBITEM_BTN08_X					NX_PRISM_UI_MENU_SUBITEM_BTN01_X
#define	NX_PRISM_UI_MENU_SUBITEM_BTN08_Y					(NX_PRISM_UI_MENU_SUBITEM_BTN07_Y + NX_PRISM_UI_MENU_SUBITEM_HEIGHT)

/* sub button 09 */
#define	NX_PRISM_UI_MENU_SUBITEM_BTN09_ID					(NX_PRISM_UI_MENU_ID + 22)

#define	NX_PRISM_UI_MENU_SUBITEM_BTN09_WIDTH				NX_PRISM_UI_MENU_SUBITEM_WIDTH
#define	NX_PRISM_UI_MENU_SUBITEM_BTN09_HEIGHT				NX_PRISM_UI_MENU_SUBITEM_HEIGHT
#define	NX_PRISM_UI_MENU_SUBITEM_BTN09_X					NX_PRISM_UI_MENU_SUBITEM_BTN01_X
#define	NX_PRISM_UI_MENU_SUBITEM_BTN09_Y					(NX_PRISM_UI_MENU_SUBITEM_BTN08_Y + NX_PRISM_UI_MENU_SUBITEM_HEIGHT)

/* sub button 10 */
#define	NX_PRISM_UI_MENU_SUBITEM_BTN10_ID					(NX_PRISM_UI_MENU_ID + 23)

#define	NX_PRISM_UI_MENU_SUBITEM_BTN10_WIDTH				NX_PRISM_UI_MENU_SUBITEM_WIDTH
#define	NX_PRISM_UI_MENU_SUBITEM_BTN10_HEIGHT				NX_PRISM_UI_MENU_SUBITEM_HEIGHT
#define	NX_PRISM_UI_MENU_SUBITEM_BTN10_X					NX_PRISM_UI_MENU_SUBITEM_BTN01_X
#define	NX_PRISM_UI_MENU_SUBITEM_BTN10_Y					(NX_PRISM_UI_MENU_SUBITEM_BTN09_Y + NX_PRISM_UI_MENU_SUBITEM_HEIGHT)

/* sub item Text  */
#define	NX_PRISM_UI_MENU_SUBITEM_CURSOR_MG					5
#define	NX_PRISM_UI_MENU_SUBITEM_TEXT_MG					10

#define	NX_PRISM_UI_MENU_SUBITEM_MAINTXT_X_TAB				18
#define	NX_PRISM_UI_MENU_SUBITEM_MAINTXT_Y_TAB				(37 - 25)
#define	NX_PRISM_UI_MENU_SUBITEM_MAINTXT_WIDTH				490

#define	NX_PRISM_UI_MENU_SUBITEM_SUBTXT_X_TAB				528
#define	NX_PRISM_UI_MENU_SUBITEM_SUBTXT_Y_TAB				(36 - 24)
#define	NX_PRISM_UI_MENU_SUBITEM_SUBTXT_WIDTH				(204 - NX_PRISM_UI_MENU_SUBITEM_MAINTXT_X_TAB)

#define	NX_PRISM_UI_MENU_SUBITEM_ICON_MG					20
#define	NX_PRISM_UI_MENU_SUBITEM_ICON_X_TAB					7
#define	NX_PRISM_UI_MENU_SUBITEM_ICON_Y_TAB					19
#define	NX_PRISM_UI_MENU_SUBITEM_ICON_WIDTH					13
#define	NX_PRISM_UI_MENU_SUBITEM_ICON_HEIGHT				20

/* Menu PopUp Window Title */
#define	NX_PRISM_UI_MENU_POPUP_SET_TITLE_ID					(NX_PRISM_UI_MENU_ID + 24)
#define	NX_PRISM_UI_MENU_POPUP_SET_TITLE_X					60
#define	NX_PRISM_UI_MENU_POPUP_SET_TITLE_Y					38
#define	NX_PRISM_UI_MENU_POPUP_SET_TITLE_WIDTH				750
#define	NX_PRISM_UI_MENU_POPUP_SET_TITLE_HEIGHT				48

#define	NX_PRISM_UI_MENU_POPUP_BACKGROUND_BOX_ID			(NX_PRISM_UI_MENU_ID + 25)
#define	NX_PRISM_UI_MENU_POPUP_BACKGROUND_BOX_X				0
#define	NX_PRISM_UI_MENU_POPUP_BACKGROUND_BOX_Y				0
#define	NX_PRISM_UI_MENU_POPUP_BACKGROUND_BOX_WIDTH			NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_UI_MENU_POPUP_BACKGROUND_BOX_HEIGHT		NX_PRISM_OSD_VERTICAL

#define	NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_X_TAB		18
#define	NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_Y_TAB		(37 - 25)
#define	NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_WIDTH		380

#define	NX_PRISM_UI_MENU_POPUP_SUBITEM_SUBTXT_X_TAB			423
#define	NX_PRISM_UI_MENU_POPUP_SUBITEM_SUBTXT_Y_TAB			(36 - 24)
#define	NX_PRISM_UI_MENU_POPUP_SUBITEM_SUBTXT_WIDTH			320

/******************************************************************************/
#define __TO_MEDIA_OBJECTS______________________________________
/******************************************************************************/
#define	NX_PRISM_UI_MEDIA_ID								eMenuFrame_Window_Media

#define	NX_PRISM_UI_MEDIA_TITLE_ID							(NX_PRISM_UI_MEDIA_ID + 1)
#define	NX_PRISM_UI_MEDIA_TITLE_LINE_ID						(NX_PRISM_UI_MEDIA_ID + 2)

#define	NX_PRISM_UI_MEDIA_CUR_DATE_ID						(NX_PRISM_UI_MEDIA_ID + 3)
#define	NX_PRISM_UI_MEDIA_CUR_TIME_ID						(NX_PRISM_UI_MEDIA_ID + 4)

#define	NX_PRISM_UI_MEDIA_SERVER_LIST_ID					(NX_PRISM_UI_MEDIA_ID + 5)
#define	NX_PRISM_UI_MEDIA_CONTENT_LIST_ID					(NX_PRISM_UI_MEDIA_ID + 6)
#define	NX_PRISM_UI_MEDIA_CONTENT_BG_ID						(NX_PRISM_UI_MEDIA_ID + 7)
#define	NX_PRISM_UI_MEDIA_CONTENT_NOITEM_IMG_ID				(NX_PRISM_UI_MEDIA_ID + 8)
#define	NX_PRISM_UI_MEDIA_CONTENT_NOITEM_MAINTXT_ID			(NX_PRISM_UI_MEDIA_ID + 9)
#define	NX_PRISM_UI_MEDIA_CONTENT_NOITEM_SUBTXT_ID			(NX_PRISM_UI_MEDIA_ID + 10)

#define	NX_PRISM_UI_MEDIA_THUMB_TIMER_ID					(NX_PRISM_UI_MEDIA_ID + 11)
#define	NX_PRISM_UI_MEDIA_THUMB_ITEM1_ID					(NX_PRISM_UI_MEDIA_ID + 12)
#define	NX_PRISM_UI_MEDIA_THUMB_ITEM2_ID					(NX_PRISM_UI_MEDIA_ID + 13)
#define	NX_PRISM_UI_MEDIA_THUMB_ITEM3_ID					(NX_PRISM_UI_MEDIA_ID + 14)
#define	NX_PRISM_UI_MEDIA_THUMB_ITEM4_ID					(NX_PRISM_UI_MEDIA_ID + 15)
#define	NX_PRISM_UI_MEDIA_THUMB_ITEM5_ID					(NX_PRISM_UI_MEDIA_ID + 16)
#define	NX_PRISM_UI_MEDIA_THUMB_ITEM6_ID					(NX_PRISM_UI_MEDIA_ID + 17)
#define	NX_PRISM_UI_MEDIA_THUMB_ITEM7_ID					(NX_PRISM_UI_MEDIA_ID + 18)

#define	NX_PRISM_UI_MEDIA_OPTION_ID							(NX_PRISM_UI_MEDIA_ID + 19)

#define	NX_PRISM_UI_MEDIA_OPERATING_BG_ID					(NX_PRISM_UI_MEDIA_ID + 20)
#define	NX_PRISM_UI_MEDIA_OPERATING_IMG_ID					(NX_PRISM_UI_MEDIA_ID + 21)
#define	NX_PRISM_UI_MEDIA_OPERATING_TXT_ID					(NX_PRISM_UI_MEDIA_ID + 22)

#define	NX_PRISM_UI_MEDIA_PATH_ID							(NX_PRISM_UI_MEDIA_ID + 23)

#define	NX_PRISM_UI_MEDIA_OPTION_DIM_ID						(NX_PRISM_UI_MEDIA_ID + 24)
#define	NX_PRISM_UI_MEDIA_BOTTOM_DIM_ID						(NX_PRISM_UI_MEDIA_ID + 25)

#define	NX_PRISM_UI_MEDIA_SELECTMODE_DIM_ID					(NX_PRISM_UI_MEDIA_ID + 26)
#define	NX_PRISM_UI_MEDIA_SELECTMODE_HELPSTR_ID				(NX_PRISM_UI_MEDIA_ID + 27)

/******************************************************************************/
#define __LIPSYNC_DELAY_COORDINATE____________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_MENU_LIPSYNC_DELAY_ID					(NX_PRISM_UI_MENU_ID + 26)
#define	NX_PRISM_UI_MENU_LIPSYNC_DELAY_X					0
#define	NX_PRISM_UI_MENU_LIPSYNC_DELAY_Y					320
#define	NX_PRISM_UI_MENU_LIPSYNC_DELAY_WIDTH				NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_UI_MENU_LIPSYNC_DELAY_HEIGHT				400

#define	NX_PRISM_UI_MENU_LIPSYNC_BAR_WINDOW_X				289
#define	NX_PRISM_UI_MENU_LIPSYNC_BAR_WINDOW_Y				320
#define	NX_PRISM_UI_MENU_LIPSYNC_BAR_WINDOW_WIDTH			NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_UI_MENU_LIPSYNC_BAR_WINDOW_HEIGHT			400

#define	NX_PRISM_UI_MENU_LIPSYNC_TXT_X						120
#define	NX_PRISM_UI_MENU_LIPSYNC_TXT_Y						320
#define	NX_PRISM_UI_MENU_LIPSYNC_TXT_WIDTH					150
#define	NX_PRISM_UI_MENU_LIPSYNC_TXT_HEIGHT					24

#define	NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_ID				(NX_PRISM_UI_MENU_ID + 27)
#define	NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_X				1050
#define	NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_Y				640
#define	NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_WIDTH			140
#define	NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_HEIGHT			24

#define	NX_PRISM_UI_MENU_LIPSYNC_BAR_X						290
#define	NX_PRISM_UI_MENU_LIPSYNC_BAR_Y						650
#define	NX_PRISM_UI_MENU_LIPSYNC_BAR_WIDTH					720
#define	NX_PRISM_UI_MENU_LIPSYNC_BAR_HEIGHT					5

/******************************************************************************/
#define __EDIT_CHANNEL_LIST_COORDINATE____________________________________________
/******************************************************************************/
#define NX_PRISM_UI_MENU_EDIT_CHLIST_BG_ID					(NX_PRISM_UI_MENU_ID + 28)
#define NX_PRISM_UI_MENU_EDIT_CHLIST_BG_X					200
#define NX_PRISM_UI_MENU_EDIT_CHLIST_BG_Y					162
#define NX_PRISM_UI_MENU_EDIT_CHLIST_BG_W					880
#define NX_PRISM_UI_MENU_EDIT_CHLIST_BG_H					470
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID					(NX_PRISM_UI_MENU_ID + 29)
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_X					(NX_PRISM_UI_MENU_EDIT_CHLIST_BG_X + 10)
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_Y					(NX_PRISM_UI_MENU_EDIT_CHLIST_BG_Y + 10)
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_W					(NX_PRISM_UI_MENU_EDIT_CHLIST_BG_W - 20)
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_H					(NX_PRISM_UI_MENU_EDIT_CHLIST_BG_H - 20)
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_PAGE_COUNT			9
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_H 				50
#define NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE		4
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_CHECKBOX_X		10
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_CHECKBOX_Y		8
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_CHECKBOX_W		34
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_CHECKBOX_H		34
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_IMG_X		18
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_IMG_Y		13
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_IMG_W		18
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_IMG_H		24
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_GROUP_IMG_Y	17
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_GROUP_IMG_W	14
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_GROUP_IMG_H	23
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_GROUP_IMG_X	22
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_TEXT_W			470
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_TEXT_OFFSET_X		59
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_TEXT_OFFSET_Y		12
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_X			820
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_Y			12
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W			36
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_H			28
#define NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_MG			5
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_ID					(NX_PRISM_UI_MENU_ID + 30)
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_X					820
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_Y					674
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_W					400
#define NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_H					30


/******************************************************************************/
#define __FAVOURITE_CHANNEL_LIST_COORDINATE____________________________________________
/******************************************************************************/
#define NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_ID		(NX_PRISM_UI_MENU_ID + 31)
#define NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_X			60
#define NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_Y			162
#define NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_W			570
#define NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_H			470
#define NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_ID			(NX_PRISM_UI_MENU_ID + 32)
#define NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_X			650
#define NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_Y			162
#define NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_W			570
#define NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_H			470
#define NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID			(NX_PRISM_UI_MENU_ID + 33)
#define NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_X				(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_X + 10)
#define NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_Y				(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_Y + 10)
#define NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_W				(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_W - 20)
#define NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_H				(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_H - 20)
#define NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID				(NX_PRISM_UI_MENU_ID + 34)
#define NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_X				(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_X + 10)
#define NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_Y				(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_Y + 10)
#define NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_W				(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_W - 20)
#define NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_H				(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_H - 20)
#define NX_PRISM_UI_MENU_FAV_CHLIST_OBJ_PAGE_COUNT			9
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_H 					50
#define NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE		4
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_CHECKBOX_X			10
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_CHECKBOX_Y			8
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_CHECKBOX_W			34
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_CHECKBOX_H			34
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_IMG_X			18
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_IMG_Y			13
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_IMG_W			18
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_IMG_H			24
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_GROUP_IMG_Y	17
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_GROUP_IMG_W	14
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_GROUP_IMG_H	23
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_GROUP_IMG_X	22
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_TEXT_W				470
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_TEXT_OFFSET_X		59
#define NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_TEXT_OFFSET_Y		12
#define NX_PRISM_UI_MENU_FAV_CHLIST_OPT_ID					(NX_PRISM_UI_MENU_ID + 35)
#define NX_PRISM_UI_MENU_FAV_CHLIST_OPT_X					820
#define NX_PRISM_UI_MENU_FAV_CHLIST_OPT_Y					674
#define NX_PRISM_UI_MENU_FAV_CHLIST_OPT_W					400
#define NX_PRISM_UI_MENU_FAV_CHLIST_OPT_H					30


/******************************************************************************/
#define __LEAF_POPUP_COORDINATE_______________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT			60
#define	NX_PRISM_UI_MENU_SUBITEM_POPUP_WIDTH				405
#define	NX_PRISM_UI_MENU_SUBITEM_POPUP_HEIGHT				600
#define	NX_PRISM_UI_MENU_SUBITEM_POPUP_X					815
#define	NX_PRISM_UI_MENU_SUBITEM_POPUP_Y					103


/******************************************************************************/
#define __PIN_DIALOG_COORDINATE_______________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_PIN_DIALOG_WIDTH						700
#define	NX_PRISM_UI_PIN_DIALOG_HEIGHT						156
#define	NX_PRISM_UI_PIN_DIALOG_X							290
#define	NX_PRISM_UI_PIN_DIALOG_Y							282

#define	NX_PRISM_UI_CHANGEPIN_DIALOG_WIDTH					700
#define	NX_PRISM_UI_CHANGEPIN_DIALOG_HEIGHT					210
#define	NX_PRISM_UI_CHANGEPIN_DIALOG_X						304
#define	NX_PRISM_UI_CHANGEPIN_DIALOG_Y						245

/******************************************************************************/
#define __TIME_DIALOG_COORDINATE_______________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_TIME_DIALOG_WIDTH						700
#define	NX_PRISM_UI_TIME_DIALOG_HEIGHT						156
#define	NX_PRISM_UI_TIME_DIALOG_X							290
#define	NX_PRISM_UI_TIME_DIALOG_Y							282

/******************************************************************************/
#define __LEAF_POPUP_OPTION_COORDINATE_________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_FRAME_POPUP_ID							eMenuFrame_Window_PopUp

#define	NX_PRISM_UI_OPT_POPUP_L_X							0
#define	NX_PRISM_UI_OPT_POPUP_L_Y							0
#define	NX_PRISM_UI_OPT_POPUP_R_X							620
#define	NX_PRISM_UI_OPT_POPUP_R_Y							0
#define	NX_PRISM_UI_OPT_POPUP_WIDTH							660
#define	NX_PRISM_UI_OPT_POPUP_HEIGHT						720

#define	NX_PRISM_UI_OPT_POPUP_LIST_L_X						60
#define	NX_PRISM_UI_OPT_POPUP_LIST_R_X						910
#define	NX_PRISM_UI_OPT_POPUP_LIST_WIDTH					310
#define	NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT					55
#define	NX_PRISM_UI_OPT_POPUP_CELL_MG						3

#define	NX_PRISM_UI_OPT_POPUP_POINTER_W						29
#define	NX_PRISM_UI_OPT_POPUP_POINTER_H						20


/******************************************************************************/
#define __LIST_POPUP_PUBLIC_POSITION___________________________________________
/******************************************************************************/

#define	NX_PRISM_UI_LISTDLG_FRAME_ID						(NX_PRISM_UI_FRAME_POPUP_ID + 1)
#define	NX_PRISM_UI_LISTDLG_LIST_BOX_ID						(NX_PRISM_UI_FRAME_POPUP_ID + 2)
#define	NX_PRISM_UI_LISTDLG_LIST_ID							(NX_PRISM_UI_FRAME_POPUP_ID + 3)
#define	NX_PRISM_UI_LISTDLG_LIST_POINTER_ID					(NX_PRISM_UI_FRAME_POPUP_ID + 4)
#define	NX_PRISM_UI_LISTDLG_LIST_TITLE_ID					(NX_PRISM_UI_FRAME_POPUP_ID + 5)
#define	NX_PRISM_UI_LISTDLG_LIST_LINE_TOP_ID				(NX_PRISM_UI_FRAME_POPUP_ID + 6)
#define	NX_PRISM_UI_LISTDLG_LIST_LINE_BOTTOM_ID				(NX_PRISM_UI_FRAME_POPUP_ID + 7)
#define	NX_PRISM_UI_LISTDLG_LIST_SCROLL_BG_ID				(NX_PRISM_UI_FRAME_POPUP_ID + 8)


/******************************************************************************/
#define __BUTTON_PUBLIC_POSITION___________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_BUTTON_ID								(NX_PRISM_UI_FRAME_POPUP_ID + 9)
#define	NX_PRISM_UI_BUTTION_TEXT_X							15
#define	NX_PRISM_UI_BUTTION_TEXT_Y							(34 -22)
#define	NX_PRISM_UI_BUTTION_TEXT_WIDTH						158
#define	NX_PRISM_UI_BUTTION_TEXT_HEIGHT						30


/******************************************************************************/
#define __HELP_PUBLIC_POSITION___________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_HELP_R_ICON_MG							10
#define	NX_PRISM_UI_HELP_OPT_ICON_W							26
#define	NX_PRISM_UI_HELP_OPT_ICON_H							30


/******************************************************************************/
#define __BAR_PUBLIC_POSITION___________________________________________
/******************************************************************************/

#define	NX_PRISM_UI_FRAME_COMPONENT_ID						eMenuFrame_Window_Compnent

#define	NX_PRISM_UI_BAR_STRENGTH_TITLE_ID					(NX_PRISM_UI_FRAME_COMPONENT_ID + 1)
#define	NX_PRISM_UI_BAR_STRENGTH_BAR_ID						(NX_PRISM_UI_FRAME_COMPONENT_ID + 2)
#define	NX_PRISM_UI_BAR_STRENGTH_VALUE_ID					(NX_PRISM_UI_FRAME_COMPONENT_ID + 3)

#define	NX_PRISM_UI_BAR_QUALITY_TITLE_ID					(NX_PRISM_UI_FRAME_COMPONENT_ID + 4)
#define	NX_PRISM_UI_BAR_QUALITY_BAR_ID						(NX_PRISM_UI_FRAME_COMPONENT_ID + 5)
#define	NX_PRISM_UI_BAR_QUALITY_VALUE_ID					(NX_PRISM_UI_FRAME_COMPONENT_ID + 6)

#define	NX_PRISM_UI_BAR_BER_TITLE_ID						(NX_PRISM_UI_FRAME_COMPONENT_ID + 7)
#define	NX_PRISM_UI_BAR_BER_BAR_ID							(NX_PRISM_UI_FRAME_COMPONENT_ID + 8)
#define	NX_PRISM_UI_BAR_BER_VALUE_ID						(NX_PRISM_UI_FRAME_COMPONENT_ID + 9)

#define	NX_PRISM_UI_BAR_A_1_BG_H							2
#define	NX_PRISM_UI_BAR_A_1_BG_MG							1

#define	NX_PRISM_UI_BAR_B_123_BG_H							2
#define	NX_PRISM_UI_BAR_B_123_BG_MG							1

#define	NX_PRISM_UI_BAR_B_4_BG_H							2
#define	NX_PRISM_UI_BAR_B_4_BG_MG							2

#define	NX_PRISM_UI_BAR_B_5_BG_H							4
#define	NX_PRISM_UI_BAR_B_5_BG_MG							0


/******************************************************************************/
#define __DIALOGUE_PUBLIC_POSITION___________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_DIALOG_TITLE_ID							(NX_PRISM_UI_FRAME_COMPONENT_ID + 10)
#define	NX_PRISM_UI_DIALOG_MESSAGE_ID						(NX_PRISM_UI_FRAME_COMPONENT_ID + 11)

#define	NX_PRISM_UI_DIALOG_LINE_H							1

#define	NX_PRISM_UI_DIALOG_BUTTON_INPUT_ID					(NX_PRISM_UI_FRAME_COMPONENT_ID + 12)

#define	NX_PRISM_UI_DIALOG_BUTTON_INPUT_W					40
#define	NX_PRISM_UI_DIALOG_BUTTON_INPUT_H					50

/******************************************************************************/
#define __DCN_PUBLIC_POSITION_________________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_DCN_BUTTON_INPUT_W						30
#define	NX_PRISM_UI_DCN_BUTTON_INPUT_H						50


/******************************************************************************/
#define __PINDIALOG_PUBLIC_POSITION___________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_PIN_CODE_MESSAGE_ID						(NX_PRISM_UI_FRAME_COMPONENT_ID + 13)
#define	NX_PRISM_UI_PIN_CODE_ID								(NX_PRISM_UI_FRAME_COMPONENT_ID + 14)

#define	NX_PRISM_UI_PIN_CODE_W								50
#define	NX_PRISM_UI_PIN_CODE_H								50

#define	NX_PRISM_UI_PIN_BTN_GAP								15

#define	NX_PRISM_UI_CHANGEPIN_BTN_ID						(NX_PRISM_UI_FRAME_COMPONENT_ID + 18)
#define	NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID 					(NX_PRISM_UI_FRAME_COMPONENT_ID + 22)

#define	NX_PRISM_UI_CHANGEPIN_NEWPIN_ID						(NX_PRISM_UI_FRAME_COMPONENT_ID + 26)
#define	NX_PRISM_UI_CHANGEPIN_VERIFYPIN_ID 					(NX_PRISM_UI_FRAME_COMPONENT_ID + 27)
#define	NX_PRISM_UI_PIN_CODE_WRONG_ID						(NX_PRISM_UI_FRAME_COMPONENT_ID + 28)

/******************************************************************************/
#define __TIMEDIALOG_PUBLIC_POSITION___________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_TIME_MESSAGE_ID							(NX_PRISM_UI_FRAME_COMPONENT_ID + 29)
#define NX_PRISM_UI_TIME_COLON_ID							(NX_PRISM_UI_FRAME_COMPONENT_ID + 30)
#define	NX_PRISM_UI_TIME_DATA_ID							(NX_PRISM_UI_FRAME_COMPONENT_ID + 31)

#define	NX_PRISM_UI_TIME_DATA_W								40
#define	NX_PRISM_UI_TIME_DATA_H								50

/******************************************************************************/
#define __IPDIALOG_PUBLIC_POSITION_____________________________________________
/******************************************************************************/

#define	NX_PRISM_UI_IP_MESSAGE_ID							(NX_PRISM_UI_FRAME_COMPONENT_ID + 32)
#define	NX_PRISM_UI_IP_DATA_ID						   		(NX_PRISM_UI_FRAME_COMPONENT_ID + 33)
#define	NX_PRISM_UI_IP_DOT_W			        			20

/******************************************************************************/
#define __HIDDEN_AP_DIALOG_PUBLIC_POSITION_____________________________________
/******************************************************************************/

#define NX_PRISM_UI_HIDDEN_AP_TEXT_TITLE_ID                 (NX_PRISM_UI_FRAME_COMPONENT_ID + 34)
#define NX_PRISM_UI_HIDDEN_AP_TEXT_EXPLAIN_ID               (NX_PRISM_UI_FRAME_COMPONENT_ID + 35)
#define NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_ID          (NX_PRISM_UI_FRAME_COMPONENT_ID + 36)
#define NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_ID              (NX_PRISM_UI_FRAME_COMPONENT_ID + 37)
#define NX_PRISM_UI_HIDDEN_AP_TEXT_NETWORK_NAME_ID          (NX_PRISM_UI_FRAME_COMPONENT_ID + 38)
#define NX_PRISM_UI_HIDDEN_AP_TEXT_PASSWORD_ID              (NX_PRISM_UI_FRAME_COMPONENT_ID + 39)

#define NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_ID         (NX_PRISM_UI_FRAME_COMPONENT_ID + 40)
#define NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_ID             (NX_PRISM_UI_FRAME_COMPONENT_ID + 41)

#define NX_PRISM_UI_HIDDEN_AP_BTN_CONNECT_ID                (NX_PRISM_UI_FRAME_COMPONENT_ID + 42)
#define NX_PRISM_UI_HIDDEN_AP_BTN_CANCEL_ID                 (NX_PRISM_UI_FRAME_COMPONENT_ID + 43)
#define NX_PRISM_UI_HIDDEN_AP_LIST_SECURITY_ID              (NX_PRISM_UI_FRAME_COMPONENT_ID + 44)


/******************************************************************************/
#define __FRAME_PUBLIC_POSITION________________________________________________
/******************************************************************************/
#define	NX_PRISM_UI_FRAME_BOX_MG							6
#define	NX_PRISM_UI_FRAME_BOX_ROUND_PX						1
#define	NX_PRISM_UI_FRAME_BOX_LINE_H						1


/******************************************************************************/
#define __OPT_MENU_PUBLIC_POSITION_____________________________________________
/******************************************************************************/

#define	NX_PRISM_UI_OPT_MENU_ICON_MG						20
#define	NX_PRISM_UI_OPT_MENU_ICON_X_TAB						7
#define	NX_PRISM_UI_OPT_MENU_ICON_Y_TAB						19
#define	NX_PRISM_UI_OPT_MENU_ICON_WIDTH						13
#define	NX_PRISM_UI_OPT_MENU_ICON_HEIGHT					20

#define	NX_PRISM_UI_OPT_MENU_CURSOR_MG						5

#define	NX_PRISM_UI_OPT_MENU_MAINTXT_X_TAB					18
#define	NX_PRISM_UI_OPT_MENU_MAINTXT_Y_TAB					(37 - 25)


/******************************************************************************/
#define __DIALOGUE_STATUE_WINDOW_POSITION______________________________________
/******************************************************************************/
#define	NX_PRISM_UI_DLG_STATUE_MSG_ITEM_ID						eMenuFrame_Window_Dialogue_Statue	// Statue Message 0 ~ 49

#define	NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_X						290
#define	NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_Y						312
#define	NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_W						700
#define	NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_H						96

#define	NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_MULTI_LINE_X			290
#define	NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_MULTI_LINE_Y			306
#define	NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_MULTI_LINE_W			NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_W
#define	NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_MULTI_LINE_H			108

#define	NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_Y_MG					16
#define	NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_H_MG					32

#define	NX_PRISM_UI_DLG_STATUE_MSG_TEXT_MULTI_LINE				2

#define	NX_PRISM_UI_DLG_STATUE_MSG_BG_ID						(NX_PRISM_UI_DLG_STATUE_MSG_ITEM_ID + 1)
#define	NX_PRISM_UI_DLG_STATUE_MSG_LINE_TOP_ID					(NX_PRISM_UI_DLG_STATUE_MSG_ITEM_ID + 2)
#define	NX_PRISM_UI_DLG_STATUE_MSG_LINE_BOTTOM_ID				(NX_PRISM_UI_DLG_STATUE_MSG_ITEM_ID + 3)
#define	NX_PRISM_UI_DLG_STATUE_MSG_LINE_H						1

#define	NX_PRISM_UI_DLG_STATUE_MSG_ICON_ID						(NX_PRISM_UI_DLG_STATUE_MSG_ITEM_ID + 4)
#define	NX_PRISM_UI_DLG_STATUE_MSG_ICON_X						30
#define	NX_PRISM_UI_DLG_STATUE_MSG_ICON_Y						24
#define	NX_PRISM_UI_DLG_STATUE_MSG_ICON_MULTI_LINE_Y			30
#define	NX_PRISM_UI_DLG_STATUE_MSG_ICON_W						55
#define	NX_PRISM_UI_DLG_STATUE_MSG_ICON_H						48
#define	NX_PRISM_UI_DLG_STATUE_MSG_ICON_Y_MG					16

#define	NX_PRISM_UI_DLG_STATUE_MSG_TEXT_ID						(NX_PRISM_UI_DLG_STATUE_MSG_ITEM_ID + 5)
#define	NX_PRISM_UI_DLG_STATUE_MSG_TEXT_X						115
#define	NX_PRISM_UI_DLG_STATUE_MSG_TEXT_Y						(56 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_STATUE_MSG_TEXT_MULTI_LINE_Y			(46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_STATUE_MSG_TEXT_W						555
#define	NX_PRISM_UI_DLG_STATUE_MSG_TEXT_LS						32


/******************************************************************************/
#define __DIALOGUE_RESPONSE_WINDOW_POSITION____________________________________
/******************************************************************************/
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_ITEM_ID					eMenuFrame_Window_Dialogue_Response	// Statue Message 50 ~ 99

#define	NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_X					290
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_Y					0
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_W					700
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_H					101

#define	NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_MULTI_LINE_X		290
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_MULTI_LINE_Y		0
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_MULTI_LINE_W		NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_W
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_MULTI_LINE_H		133

#define	NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_H_MG				32
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_MULTI_LINE			2

#define	NX_PRISM_UI_DLG_RESPONSE_MSG_BG_ID						(NX_PRISM_UI_DLG_RESPONSE_MSG_ITEM_ID + 1)
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_LINE_BOTTOM_ID				(NX_PRISM_UI_DLG_RESPONSE_MSG_ITEM_ID + 2)
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_LINE_H						1

#define	NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_ID					(NX_PRISM_UI_DLG_RESPONSE_MSG_ITEM_ID + 3)
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_X						30
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_Y						(71 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_W						640
#define	NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_LS					32

#define	NX_PRISM_UI_DLG_RESPONSE_MSG_EXP_TIMER_ID				(NX_PRISM_UI_DLG_RESPONSE_MSG_ITEM_ID + 4)


/******************************************************************************/
#define __DIALOGUE_CONFIRM_WINDOW_POSITION______________________________________
/******************************************************************************/
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_ITEM_ID						eMenuFrame_Window_Dialogue_Confirm		// Statue Message 100 ~ 149

#define	NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_X					290
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_Y					282
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_W					700
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_H					156

#define	NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_MULTI_LINE_X			290
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_MULTI_LINE_Y			266
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_MULTI_LINE_W			NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_W
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_MULTI_LINE_H			188

#define	NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_Y_MG					16
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_H_MG					32

#define	NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_MULTI_LINE				2

#define	NX_PRISM_UI_DLG_CONFIRM_MSG_BG_ID						(NX_PRISM_UI_DLG_CONFIRM_MSG_ITEM_ID + 1)
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_LINE_TOP_ID					(NX_PRISM_UI_DLG_CONFIRM_MSG_ITEM_ID + 2)
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_LINE_BOTTOM_ID				(NX_PRISM_UI_DLG_CONFIRM_MSG_ITEM_ID + 3)
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_LINE_H						1

#define	NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_ID						(NX_PRISM_UI_DLG_CONFIRM_MSG_ITEM_ID + 4)
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_X						30
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_Y						(46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_MULTI_LINE_Y			(46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_W						640
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_LS						32

#define	NX_PRISM_UI_DLG_CONFIRM_MSG_EXP_TIMER_ID				(NX_PRISM_UI_DLG_CONFIRM_MSG_ITEM_ID + 6)

#define	NX_PRISM_UI_DLG_CONFIRM_MSG_BUTTON_Y_MG					32
#define	NX_PRISM_UI_DLG_CONFIRM_MSG_BUTTON_MG					15

#define	NX_PRISM_UI_DLG_CONFIRM_BTN_X							240
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_Y							76
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_MULTI_LINE_Y				108
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_W							220
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_H							50

#define	NX_PRISM_UI_DLG_CONFIRM_BTN_L_X							123
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_L_Y							76
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_L_MULTI_LINE_Y				108
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_L_W							NX_PRISM_UI_DLG_CONFIRM_BTN_W
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_L_H							NX_PRISM_UI_DLG_CONFIRM_BTN_H

#define	NX_PRISM_UI_DLG_CONFIRM_BTN_R_X							358
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_R_Y							76
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_R_MULTI_LINE_Y				108
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_R_W							NX_PRISM_UI_DLG_CONFIRM_BTN_W
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_R_H							NX_PRISM_UI_DLG_CONFIRM_BTN_H

#define	NX_PRISM_UI_DLG_CONFIRM_BTN_L_ID						(NX_PRISM_UI_DLG_CONFIRM_MSG_ITEM_ID + 7)
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID						(NX_PRISM_UI_DLG_CONFIRM_MSG_ITEM_ID + 8)
#define	NX_PRISM_UI_DLG_CONFIRM_BTN_R_ID						(NX_PRISM_UI_DLG_CONFIRM_MSG_ITEM_ID + 9)


/******************************************************************************/
#define __DIALOGUE_PROCESS_WINDOW_POSITION______________________________________
/******************************************************************************/
#define	NX_PRISM_UI_DLG_PROCESS_MSG_ITEM_ID						eMenuFrame_Window_Dialogue_Process	// Statue Message 150 ~ 199

#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_X					290
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_Y					0
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_W					700
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_H					100

#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BAR_Y				310
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BAR_H				100

#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BTN_Y				282
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BTN_H				156

#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BAR_BTN_Y			275
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BAR_BTN_H			170

#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_Y			0
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_H			133

#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BAR_Y		310
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BAR_H		100

#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BTN_Y		266
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BTN_H		188

#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BAR_BTN_Y	259
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BAR_BTN_H	202

#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_Y_MG					16
#define	NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_H_MG					32

#define	NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_MULTI_LINE				2

#define	NX_PRISM_UI_DLG_PROCESS_MSG_BG_ID						(NX_PRISM_UI_DLG_PROCESS_MSG_ITEM_ID + 1)
#define	NX_PRISM_UI_DLG_PROCESS_MSG_LINE_TOP_ID					(NX_PRISM_UI_DLG_PROCESS_MSG_ITEM_ID + 2)
#define	NX_PRISM_UI_DLG_PROCESS_MSG_LINE_BOTTOM_ID				(NX_PRISM_UI_DLG_PROCESS_MSG_ITEM_ID + 3)
#define	NX_PRISM_UI_DLG_PROCESS_MSG_LINE_H						1

#define	NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_ID						(NX_PRISM_UI_DLG_PROCESS_MSG_ITEM_ID + 4)
#define	NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_X						30
#define	NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_Y						(51 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_MULTI_LINE_Y			(46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_W						640
#define	NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_LS						32

#define	NX_PRISM_UI_DLG_PROCESS_MSG_BUTTON_Y_MG					32
#define	NX_PRISM_UI_DLG_PROCESS_MSG_BUTTON_MG					15

#define	NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_ID					(NX_PRISM_UI_DLG_PROCESS_MSG_ITEM_ID + 5)
#define	NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_X					30
#define	NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_Y					20
#define	NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_MULTI_LINE_Y		31
#define	NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_W					46
#define	NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_H					46

#define	NX_PRISM_UI_DLG_PROCESS_MSG_BAR_ID						(NX_PRISM_UI_DLG_PROCESS_MSG_ITEM_ID + 6)
#define	NX_PRISM_UI_DLG_PROCESS_MSG_BAR_X						106
#define	NX_PRISM_UI_DLG_PROCESS_MSG_BAR_Y						71
#define	NX_PRISM_UI_DLG_PROCESS_MSG_BAR_MULTI_LINE_Y			98
#define	NX_PRISM_UI_DLG_PROCESS_MSG_BAR_W						534
#define	NX_PRISM_UI_DLG_PROCESS_MSG_BAR_H						4

#define	NX_PRISM_UI_DLG_PROCESS_BTN_ID							(NX_PRISM_UI_DLG_PROCESS_MSG_ITEM_ID + 7)
#define	NX_PRISM_UI_DLG_PROCESS_BTN_X							240
#define	NX_PRISM_UI_DLG_PROCESS_BTN_Y							81
#define	NX_PRISM_UI_DLG_PROCESS_BTN_MULTI_LINE_Y				108
#define	NX_PRISM_UI_DLG_PROCESS_BTN_W							220
#define	NX_PRISM_UI_DLG_PROCESS_BTN_H							50

#define	NX_PRISM_UI_DLG_PROCESS_BTN_BAR_Y						95
#define	NX_PRISM_UI_DLG_PROCESS_BTN_BAR_MULTI_LINE_Y			122

#define	NX_PRISM_UI_DLG_PROCESS_ANIMATION_ICON_NUM				4
#define	NX_PRISM_UI_DLG_PROCESS_ANIMATION_TIMER_ID				(NX_PRISM_UI_DLG_PROCESS_MSG_ITEM_ID + 8)
#define	NX_PRISM_UI_DLG_PROCESS_ANIMATION_TIMER_PERIOD			(700)	/* 700msec */

#define	NX_PRISM_UI_DLG_PROCESS_MSG_EXP_TIMER_ID				(NX_PRISM_UI_DLG_CONFIRM_MSG_ITEM_ID + 9)


#if 0
#define	PMSG_ANIMATION_ICON_NUM			12	// number of animation icon
#define	PMSG_ANIMATION_TIMER_ID			(0x11050145)
#define	PMSG_ANIMATION_TIMER_PERIOD		(100)	/* 100msec */
#endif

/******************************************************************************/
#define __DIALOGUE_MULTI_SELECT_WINDOW_POSITION_______________________________
/******************************************************************************/

#define	NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_X					290
#define	NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_Y					449
#define	NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_W					700
#define	NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_H					221

#define	NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_MG					55
#define	NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_MULTI_MG			32

#define	NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_MULTI_LINE			1
#define	NX_PRISM_UI_DLG_MULTISEL_MSG_DEFAULT_ITEM				2

#define	NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_W						640
#define	NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_W						640

#define	NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_X						30
#define	NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_Y						76
#define	NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_W						640
#define	NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H						55

#define	NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG					3

#define	NX_PRISM_UI_DLG_MULTISEL_H_LIST_ITEM_TAB				40


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/


#endif // __NX_PRISM_UI_H__

