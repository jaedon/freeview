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
 * @file	  		nx_prism_infohub.c
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
#include <nx_core.h>
#include <nx_port.h>

#include <nx_common.h>
#include <nx_core_message.h>
#include <nx_prism_app.h>

#include "nx_prism_definition.h"
#include <nx_prism_channellist.h>

#include <nx_core_metadata_epg.h>
#include <nx_core_metadata_pf.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_INFOHUB_SHORT_STR_LEN					64
#define	NX_PRISM_INFOHUB_DEF_STR_LEN					128
#define	NX_PRISM_INFOHUB_LONG_STR_LEN					512

#define	NX_PRISM_INFOHUB_FIRST_INDEX					0
#define	NX_PRISM_INFOHUB_ITEM_MAX						1

#define	NX_PRISM_INFOHUB_ITEM_ID						eMenuFrame_Window_InfoHub

// timer
#define	NX_PRISM_INFOHUB_PROGRAMME_TIMER_ID				(NX_PRISM_INFOHUB_ITEM_ID + 1)
#define	NX_PRISM_INFOHUB_PROGRAMME_UPDATE_TIME			(1 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_INFOHUB_EXIT_TIMER_ID					(NX_PRISM_INFOHUB_ITEM_ID + 2)
#define	NX_PRISM_INFOHUB_EXIT_EXPIRE_TIME				(30 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_INFOHUB_UPDATE_TIMER_ID				(NX_PRISM_INFOHUB_ITEM_ID + 3)
#define	NX_PRISM_INFOHUB_UPDATE_TIME					(10 * NX_COMMON_TIME_SECOND)

// BG
#define	NX_PRISM_INFOHUB_BG_ID							(NX_PRISM_INFOHUB_ITEM_ID + 4)
#define	NX_INFOHUB_BG_X									0
#define	NX_INFOHUB_BG_Y									0
#define	NX_INFOHUB_BG_W									NX_INFOHUB_FRAME_W
#define	NX_INFOHUB_BG_H									NX_INFOHUB_FRAME_H

#define	NX_PRISM_INFOHUB_ID								(NX_PRISM_INFOHUB_ITEM_ID + 5)

// area
#define	NX_INFOHUB_FRAME_X								0
#define	NX_INFOHUB_FRAME_Y								0
#define	NX_INFOHUB_FRAME_W								900
#define	NX_INFOHUB_FRAME_H								NX_PRISM_OSD_VERTICAL

// base channel info
#define	NX_INFOHUB_CHBOX_ID								(NX_PRISM_INFOHUB_ITEM_ID + 6)
#define	NX_INFOHUB_CHBOX_X								0
#define	NX_INFOHUB_CHBOX_Y								132
#define	NX_INFOHUB_CHBOX_W								490
#define	NX_INFOHUB_CHBOX_H								66

#define	NX_INFOHUB_CHBOX_LCN_TEXT_ID					(NX_PRISM_INFOHUB_ITEM_ID + 7)
#define	NX_INFOHUB_CHBOX_LCN_TEXT_X						(NX_INFOHUB_CHBOX_X + 50)
#define	NX_INFOHUB_CHBOX_LCN_TEXT_Y						(NX_INFOHUB_CHBOX_Y + 46 - NX_PRISM_FONT_SIZE_36)
#define	NX_INFOHUB_CHBOX_LCN_TEXT_W						85
#define	NX_INFOHUB_CHBOX_LCN_TEXT_H						(NX_PRISM_FONT_SIZE_36 + 8)

#define	NX_INFOHUB_CHBOX_LINE_ID						(NX_PRISM_INFOHUB_ITEM_ID + 8)
#define	NX_INFOHUB_CHBOX_LINE_X							(NX_INFOHUB_CHBOX_X + 147)
#define	NX_INFOHUB_CHBOX_LINE_Y							(NX_INFOHUB_CHBOX_Y + 8)
#define	NX_INFOHUB_CHBOX_LINE_W							1
#define	NX_INFOHUB_CHBOX_LINE_H							42

#define	NX_INFOHUB_CHBOX_NAME_TEXT_ID					(NX_PRISM_INFOHUB_ITEM_ID + 9)
#define	NX_INFOHUB_CHBOX_NAME_TEXT_X					(NX_INFOHUB_CHBOX_X + 161)
#define	NX_INFOHUB_CHBOX_NAME_TEXT_Y					(NX_INFOHUB_CHBOX_Y + 46 - NX_PRISM_FONT_SIZE_36)
#define	NX_INFOHUB_CHBOX_NAME_TEXT_W					314
#define	NX_INFOHUB_CHBOX_NAME_TEXT_H					(NX_PRISM_FONT_SIZE_36 + 8)

// event info
#define	NX_INFOHUB_EVENTNAME_TEXT_ID					(NX_PRISM_INFOHUB_ITEM_ID + 10)
#define	NX_INFOHUB_EVENTNAME_TEXT_X						60
#define	NX_INFOHUB_EVENTNAME_TEXT_Y						(230 - NX_PRISM_FONT_SIZE_32)
#define	NX_INFOHUB_EVENTNAME_TEXT_W						430
#define	NX_INFOHUB_EVENTNAME_TEXT_H						(46 *2)

#define	NX_INFOHUB_PROGRAMME_IMG_START_X				60
#define	NX_INFOHUB_PROGRAMME_IMG_START_Y				(NX_INFOHUB_CHBOX_Y + 147)
#define	NX_INFOHUB_PROGRAMME_IMG_W						36
#define	NX_INFOHUB_PROGRAMME_IMG_H						30
#define	NX_INFOHUB_PROGRAMME_ICON_MG					3

#define	NX_INFOHUB_PROGRAMME_ICON_RECORDED_W			36
#define	NX_INFOHUB_PROGRAMME_ICON_HD_W					36
#define	NX_INFOHUB_PROGRAMME_ICON_RATE_W				35
#define	NX_INFOHUB_PROGRAMME_ICON_CAS_W					36
#define	NX_INFOHUB_PROGRAMME_ICON_LOCK_W				22
#define	NX_INFOHUB_PROGRAMME_ICON_DRM_W					57
#define	NX_INFOHUB_PROGRAMME_ICON_MULTI_AUDIO_W			33
#define	NX_INFOHUB_PROGRAMME_ICON_SUBTITLE_W			28
#define	NX_INFOHUB_PROGRAMME_ICON_TTX_W					28
#define	NX_INFOHUB_PROGRAMME_ICON_RESERVATION_W			28
#define	NX_INFOHUB_PROGRAMME_ICON_ONDEMAND_W			36

#define	NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID			(NX_PRISM_INFOHUB_ITEM_ID + 31)
#define	NX_INFOHUB_PROGRAMME_DURATION_TEXT_X			60
#define	NX_INFOHUB_PROGRAMME_DURATION_TEXT_Y			(326 - NX_PRISM_FONT_SIZE_20)
#define	NX_INFOHUB_PROGRAMME_DURATION_TEXT_W			410
#define	NX_INFOHUB_PROGRAMME_DURATION_TEXT_H			(NX_PRISM_FONT_SIZE_20 + 8)

#define	NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID		(NX_PRISM_INFOHUB_ITEM_ID + 32)
#define	NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_X		80
#define	NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_Y		(326 - NX_PRISM_FONT_SIZE_20)
#define	NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_W		390
#define	NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_H		(NX_PRISM_FONT_SIZE_20 + 8)

#define	NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID		(NX_PRISM_INFOHUB_ITEM_ID + 33)
#define	NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_X		60
#define	NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_Y		331
#define	NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_W		410
#define	NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_H		3

// date time info
#define	NX_INFOHUB_DATE_TEXT_ID							(NX_PRISM_INFOHUB_ITEM_ID + 34)
#define	NX_INFOHUB_DATE_TEXT_X							60
#define	NX_INFOHUB_DATE_TEXT_Y							31
#define	NX_INFOHUB_DATE_TEXT_W							150
#define	NX_INFOHUB_DATE_TEXT_H							26

#define	NX_INFOHUB_TIME_TEXT_ID							(NX_PRISM_INFOHUB_ITEM_ID + 35)
#define	NX_INFOHUB_TIME_TEXT_X							60
#define	NX_INFOHUB_TIME_TEXT_Y							53
#define	NX_INFOHUB_TIME_TEXT_W							150
#define	NX_INFOHUB_TIME_TEXT_H							30

// arrow image
#define	NX_INFOHUB_BASEARROW_UP_IMG_ID					(NX_PRISM_INFOHUB_ITEM_ID + 36)
#define	NX_INFOHUB_BASEARROW_UP_IMG_X					260
#define	NX_INFOHUB_BASEARROW_UP_IMG_Y					104
#define	NX_INFOHUB_BASEARROW_UP_IMG_W					20
#define	NX_INFOHUB_BASEARROW_UP_IMG_H					13

#define	NX_INFOHUB_BASEARROW_RIGHT_IMG_ID				(NX_PRISM_INFOHUB_ITEM_ID + 37)
#define	NX_INFOHUB_BASEARROW_RIGHT_IMG_X				497
#define	NX_INFOHUB_BASEARROW_RIGHT_IMG_Y				152
#define	NX_INFOHUB_BASEARROW_RIGHT_IMG_W				13
#define	NX_INFOHUB_BASEARROW_RIGHT_IMG_H				20

#define	NX_INFOHUB_BASEARROW_DOWN_IMG_ID				(NX_PRISM_INFOHUB_ITEM_ID + 38)
#define	NX_INFOHUB_BASEARROW_DOWN_IMG_X					260
#define	NX_INFOHUB_BASEARROW_DOWN_IMG_Y					349
#define	NX_INFOHUB_BASEARROW_DOWN_IMG_W					20
#define	NX_INFOHUB_BASEARROW_DOWN_IMG_H					13

// programme image
#define	NX_INFOHUB_ICON_INTERVAL						5


#define	NX_INFOHUB_OPTION_ITEM_LS						40

#define	NX_INFOHUB_DETAIL_OPTION_TEXT_ID				(NX_PRISM_INFOHUB_ITEM_ID + 39)
#define	NX_INFOHUB_DETAIL_OPTION_TEXT_X					60
#define	NX_INFOHUB_DETAIL_OPTION_TEXT_Y					600
#define	NX_INFOHUB_DETAIL_OPTION_TEXT_W					400
#define	NX_INFOHUB_DETAIL_OPTION_TEXT_H					28
#define	NX_INFOHUB_DETAIL_OPTION_IMG_MG					10

#define	NX_INFOHUB_LIVESTREAMING_ITEM_ID				(NX_PRISM_INFOHUB_ITEM_ID + 40)
#define	NX_INFOHUB_LIVESTREAMING_ITEM_X					60
#define	NX_INFOHUB_LIVESTREAMING_ITEM_Y					(NX_INFOHUB_DETAIL_OPTION_TEXT_Y - NX_INFOHUB_OPTION_ITEM_LS)
#define	NX_INFOHUB_LIVESTREAMING_ITEM_W					400
#define	NX_INFOHUB_LIVESTREAMING_ITEM_H					28
#define	NX_INFOHUB_LIVESTREAMING_ITEM_MG				10

#define	NX_INFOHUB_RECORDING_ITEM_ID					(NX_PRISM_INFOHUB_ITEM_ID + 41)
#define	NX_INFOHUB_RECORDING_ITEM_X						60
#define	NX_INFOHUB_RECORDING_ITEM_Y						(NX_INFOHUB_LIVESTREAMING_ITEM_Y - NX_INFOHUB_OPTION_ITEM_LS)
#define	NX_INFOHUB_RECORDING_ITEM_W						400
#define	NX_INFOHUB_RECORDING_ITEM_H						28
#define	NX_INFOHUB_RECORDING_ITEM_MG					10

#define	NX_INFOHUB_ICON_LIST_ID							(NX_PRISM_INFOHUB_ITEM_ID + 42)
#define	NX_INFOHUB_ICON_LIST_X							60
#define	NX_INFOHUB_ICON_LIST_Y							(NX_INFOHUB_CHBOX_Y + 147)
#define	NX_INFOHUB_ICON_LIST_W							410
#define	NX_INFOHUB_ICON_LIST_H							30


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum
{
	eNxHubKey_NONE,
	eNxHubKey_LEFT_DOWN,
	eNxHubKey_RIGHT_DOWN,

}NxInfoHubKey_e;

typedef struct
{
	HCHAR			pszChDis[NX_PRISM_INFOHUB_SHORT_STR_LEN];
	HCHAR			pszEpisodeNumber[EPG_HUMAX_NAME_LEN];
	HCHAR 			pszLongDes[EPG_HUMAX_DETAIL_LEN];
	HCHAR			pszGenres[NX_PRISM_INFOHUB_DEF_STR_LEN];
}NxInfoHubDetailData_t;

typedef struct
{
	HINT32 				lcn;
	HxDATETIME_t 		datetime;
	HCHAR 				pszDate[NX_TEXT_SIZE_32];
	HCHAR 				pszTime[NX_TEXT_SIZE_32];
	HCHAR 				*pszDayOfWeek;
	HCHAR 				*pszChName;
	HCHAR				szChName[DxNAME_LEN];
	HCHAR 				pszLcn[NX_TEXT_SIZE_8];
	HCHAR 				pszEventName[EPG_HUMAX_NAME_LEN];
	HBOOL				isHD;
	HINT32				subtitleType;
	HBOOL				hasTeletext;
	HBOOL				bIsLock;
	HINT32				audioType;
	HINT32				rating;
	HINT32				casId;
	HUINT32				startTime;
	HUINT32				duration;
	HCHAR				pszDuration[NX_TEXT_SIZE_64];
	HCHAR				pszStartTime[NX_TEXT_SIZE_64];
	DxRsvType_e			eRsvType;
	NxInfoHubDetailData_t *pstDetailData;
}NxInfoHubData_t;

typedef struct
{
	HINT32				nChannelNum;
	HINT32				nCurChannelIndex;			// channel list 에서 리스트의 channel 목록 유지 및 처리를 위해.
	HINT32				nProgrammeIndex;			// 현재 programme info index
	NX_Channel_t 		*pstChannels;				// live proc 에서 얻어옴..
	NxInfoHubData_t		*pstInfoHubData;
	HUINT32				lastSearchStartTime;		// 마지막으로 search 한 epg query time 저장
	NxInfoHubKey_e		pushKey;					// epg update 시 , 이전 키값을 처리하기 위해.. 이방법 외에 없는가...
	NX_ChListGroup_t	eChangedGroup;
	eNxChlist_OptItem_t	ePinCodeActiontItem;			// Pin-Code Ation Item

	HCHAR				szOptionText[NX_TEXT_SIZE_64];
	HCHAR				szRecordingText[NX_TEXT_SIZE_64];
	HCHAR				szLiveStremingText[NX_TEXT_SIZE_64];
	HCHAR				szRenameText[DxNAME_LEN];
}NxUiInfoHubContents_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static NxUiInfoHubContents_t	s_UiInfoHubContents;
static NX_PinDialogInstance_t	s_stUiInfoHub_PinCodeDialog;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_infohub_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_infohub_DestroyPinCodeDialog(void);
static void				nx_infohub_CreatePinCodeDialog(void);

static ONDK_Result_t	nx_infohub_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_infohub_CreateResponseDialouge(HUINT8* pszString);

/* Info Hub Option */
static ONDK_Result_t	nx_infohub_OptionActionAddFavourite(NxUiInfoHubContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_infohub_OptionActionDelete(NxUiInfoHubContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_infohub_OptionActionLockUnLock(NxUiInfoHubContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_infohub_OptionActionLock(NxUiInfoHubContents_t *pstContents);
static ONDK_Result_t	nx_infohub_OptionActionUnLock(NxUiInfoHubContents_t *pstContents);
static ONDK_Result_t	nx_infohub_OptionActionRename(NxUiInfoHubContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_infohub_OptionActionSaveRename(NxUiInfoHubContents_t *pstContents);

/* Info Hub */
static NxUiInfoHubContents_t* 	nx_infohub_GetContents(void);

static ONDK_Result_t	nx_infohub_ResetProgrammeTimer(NxUiInfoHubContents_t *pstContents);
static ONDK_Result_t	nx_infohub_ResetExitInfoHubTimer(NxUiInfoHubContents_t *pstContents);

static HERROR 			nx_infohub_SearchEpg(NxUiInfoHubContents_t *pstContents, NX_QUERY_t query);
static HERROR			nx_infohub_RebuildingProgrammes(NxUiInfoHubContents_t *pstContents);
static HERROR			nx_infohub_UpdateTimeAndEventTime(NxUiInfoHubContents_t *pstContents);
static HERROR			nx_infohub_UpdateObjects(NxUiInfoHubContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static NX_Programme_t * nx_infohub_GetCurrentProgramme(NxUiInfoHubContents_t *pstContents);

static	ONDK_Result_t	nx_infohub_DrawIconLists(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
													ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

#if defined(CONFIG_MW_MM_PVR)
static void				nx_infohub_SetUiRecordingStatue(NxUiInfoHubContents_t *pstContents);
#endif
#if defined(CONFIG_MW_MM_DLNA_LIVESTREAMING)
static void				nx_infohub_SetUiLiveStreamingStatue(NxUiInfoHubContents_t *pstContents);
#endif
static void				nx_infohub_SetUiCreateOptionMenu(NxUiInfoHubContents_t *pstContents);
static HERROR 			nx_infohub_SetUiObject(NxUiInfoHubContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static HERROR			nx_infohub_SetInfoHubData (NxUiInfoHubContents_t *pstContents, HBOOL bProgramme);


static ONDK_Result_t 	nx_infohub_LeftKeyDown(NxUiInfoHubContents_t *pstContents);
static ONDK_Result_t 	nx_infohub_RightKeyDown(NxUiInfoHubContents_t *pstContents);
static ONDK_Result_t	nx_infohub_Destroy(NxUiInfoHubContents_t *pstContents);
static ONDK_Result_t	nx_infohub_ReCreate(NxUiInfoHubContents_t *pstContents);

static ONDK_Result_t 	nx_infohub_MsgGwmCreate(NxUiInfoHubContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_infohub_MsgGwmDestroy(NxUiInfoHubContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_infohub_MsgGwmTimer(NxUiInfoHubContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_infohub_MsgGwmKeyDown(NxUiInfoHubContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_infohub_MsgCoreScheduleNotify(NxUiInfoHubContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_infohub_MsgPopUpOptValueChange(NxUiInfoHubContents_t *pstContents, HINT32 p2, HINT32 p3);

static void 			nx_infohub_SetVisibleMainInfoHubUI(NxUiInfoHubContents_t *pstContents, HBOOL visible);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Pin_Code_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_infohub_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stUiInfoHub_PinCodeDialog, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_infohub_DestroyPinCodeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_infohub_DisplayPinCodeDialog);
}


static void		nx_infohub_CreatePinCodeDialog(void)
{
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0;

	stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
	NX_PRISM_COMP_PinDlalog_SetDialog(&s_stUiInfoHub_PinCodeDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(RES_MESG_3684_ID),
								eNxPinCodeDataType_ParentalControl);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_infohub_DisplayPinCodeDialog", APP_DEFAULT_PRIORITY, nx_infohub_DisplayPinCodeDialog, 0, 0, 0, 0);
}


/******************************************************************************/
#define __Response_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_infohub_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	switch (lMessage)
	{
		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon (&lParam1);
			switch (lParam1)
			{
				case KEY_RED:
				case KEY_GREEN:
				case KEY_YELLOW:
				case KEY_BLUE:
				case KEY_ARROWUP :
				case KEY_ARROWDOWN:
				case KEY_TVRADIO:
				case KEY_EXT_AV:
				case KEY_COMBO:
				case KEY_TEXT:
				case KEY_TTX_HOLD:
				case KEY_TTX_SUBPAGE:
				case KEY_AUDIO:
				case KEY_SUBTITLE:
				case KEY_WIDE:
				case KEY_3D:
				case KEY_INFO:
				case KEY_BACK:
				case KEY_EXT_AV_PLUS:
				case KEY_EXT_AV_MINUS:
					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_BREAK;
				default:
					break;
			}
			break;

		case MSG_APP_SYSTEM_SVC_CHANGED :
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		default :
			break;
	}

	return	NX_PRISM_DIALOGUE_ResponseMessage_Proc(lMessage, hHandle, lParam1, lParam2, lParam3);
}


static void		nx_infohub_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_infohub_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_infohub_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


/******************************************************************************/
#define __Option_Functions______________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_infohub_OptionActionAddFavourite(NxUiInfoHubContents_t *pstContents, HINT32 p3)
{
	// Alreay Saved Action, so Just update list if in favourite list

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_OptionActionDelete(NxUiInfoHubContents_t *pstContents, HINT32 p3)
{
	HERROR				hError = ERR_FAIL;
	NX_Channel_t		*stSelectChannel = NULL, *stCurPlayChannel = NULL;
	NX_ChListGroup_t	eCurPlayChListGroup = eNxChListGroup_TV;
	DxService_t			*stSvcInfo = NULL, *stFindSvcInfo = NULL;
	NX_FavGroupID_e 	eFavGroupID = eNxFavGroupID_None;

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurChannelIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurChannelIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return ONDK_RET_FAIL;
	}

	// 1. Check it is delete Live Service
	stCurPlayChannel = NX_PRISM_LIVE_GetMasterChannel();
	if ((stCurPlayChannel != NULL) && (stCurPlayChannel->svcUid == stSelectChannel->svcUid))
	{
		eCurPlayChListGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
		// 1.1 Find Next Service
		stFindSvcInfo = NX_CHANNEL_GROUP_FindServiceInGroup(eCurPlayChListGroup, eNxFindSvcDir_Next, stCurPlayChannel);
		if (stFindSvcInfo)
		{
			NX_APP_Info("Find Next Service eCurPlayChListGroup:(0x%x), SvcUID:(0x%x), Name:(%s)!!\n", eCurPlayChListGroup, stFindSvcInfo->uid, stFindSvcInfo->name);
		}

		// 1.2 Find Prev Service
		if (stFindSvcInfo == NULL)
		{
			NX_APP_Error("Error!!! Do not find Next Service eCurPlayChListGroup:(0x%x), Svc UID:(0x%x)!!\n", eCurPlayChListGroup, stCurPlayChannel->svcUid);
			stFindSvcInfo = NX_CHANNEL_GROUP_FindServiceInGroup(eCurPlayChListGroup, eNxFindSvcDir_Prev, stCurPlayChannel);
			if (stFindSvcInfo)
			{
				NX_APP_Info("Find Prev Service eCurPlayChListGroup:(0x%x), SvcUID:(0x%x), Name:(%s)!!\n", eCurPlayChListGroup, stFindSvcInfo->uid, stFindSvcInfo->name);
			}
		}

		// 1.3 Find First Service
		if (stFindSvcInfo == NULL)
		{
			NX_APP_Error("Error!!! Do not find Prev Service eCurPlayChListGroup:(0x%x), Svc UID:(0x%x)!!\n", eCurPlayChListGroup, stCurPlayChannel->svcUid);
			stFindSvcInfo = NX_CHANNEL_GROUP_FindServiceInGroup(eCurPlayChListGroup, eNxFindSvcDir_First, stCurPlayChannel);
			if (stFindSvcInfo)
			{
				NX_APP_Info("Find First Service eCurPlayChListGroup:(0x%x), SvcUID:(0x%x), Name:(%s)!!\n", eCurPlayChListGroup, stFindSvcInfo->uid, stFindSvcInfo->name);
			}
		}

		if (stFindSvcInfo != NULL)
		{
			NX_PRISM_LIVE_ServiceStartMainView(TRUE, stFindSvcInfo->uid, eNxChListGroup_CURRENT, 0);
		}
		else
		{
			NX_APP_Error("Error!!! Do not find Next tunning Service eCurPlayChListGroup:(0x%x)!!\n", eCurPlayChListGroup);
		}
	}

	/* Update Service Info */
	if ((eCurPlayChListGroup >= eNxChListGroup_Favorite1) && (eCurPlayChListGroup <= eNxChListGroup_Favorite5))
	{
		eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eCurPlayChListGroup);
		if (eFavGroupID != eNxFavGroupID_None)
		{
			hError = NX_CHANNEL_FAV_RemoveFavouriteItem(eFavGroupID, stSelectChannel);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_CHANNEL_FAV_RemoveFavouriteItem() - eFavGroupID :(%d)!!\n", eFavGroupID);
			}
		}
	}
	else
	{
		hError = NX_CHANNEL_RemoveService(stSvcInfo);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_CHANNEL_RemoveService() - Svc Uid :(x0%x)!!\n", stSvcInfo->uid);
		}
	}

	/* Noti - System Service Update */
	ONDK_GWM_SendMessage(NULL, MSG_APP_SYSTEM_UPDATE_SERVICE, 0, 0, 0, 0);

	/* Show update Info Message */
	nx_infohub_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5653_ID));

	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_OptionActionLockUnLock(NxUiInfoHubContents_t *pstContents, HINT32 p3)
{
	eNxUiUtil_OptItem_t 	eUtilOptItem = (eNxUiUtil_OptItem_t)p3;

	if (eUtilOptItem == eNxUtil_OptItem_Lock)
	{
		nx_infohub_OptionActionLock(pstContents);
	}
	else
	{
		pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Lock;
		nx_infohub_CreatePinCodeDialog();
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_OptionActionLock(NxUiInfoHubContents_t *pstContents)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t 	*stSvcInfo = NULL;


	NX_APP_Info("Action - eNxUtil_OptItem_Lock !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurChannelIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurChannelIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return ONDK_RET_FAIL;
	}

	if (stSvcInfo->locked == TRUE)
	{
		NX_APP_Info("Alreay Locked Service:(%s) !!\n", stSvcInfo->name);
		return ONDK_RET_OK;
	}

	stSvcInfo->locked = TRUE;

	/* Update Service Info */
	NX_CHANNEL_UpdateService(stSvcInfo);

	/* Update Focus Item lock Info */
	nx_infohub_ResetProgrammeTimer(pstContents);

	/* Show update Info Message */
	nx_infohub_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5654_ID));

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_OptionActionUnLock(NxUiInfoHubContents_t *pstContents)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t 	*stSvcInfo = NULL;


	NX_APP_Info("Action - eNxUtil_OptItem_UnLock !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurChannelIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurChannelIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return ONDK_RET_FAIL;
	}

	if (stSvcInfo->locked == FALSE)
	{
		NX_APP_Info("Alreay UnLocked Service:(%s) !!\n", stSvcInfo->name);
		return ONDK_RET_OK;
	}

	stSvcInfo->locked = FALSE;

	/* Update Service Info */
	NX_CHANNEL_UpdateService(stSvcInfo);

	/* Update Focus Item lock Info */
	nx_infohub_ResetProgrammeTimer(pstContents);

	/* Show update Info Message */
	nx_infohub_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5655_ID));

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_OptionActionRename(NxUiInfoHubContents_t *pstContents, HINT32 p3)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t 	*stSvcInfo = NULL;


	NX_APP_Info("Action - eNxChlist_OptItem_Rename !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurChannelIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurChannelIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_PRISM_INFOHUB_EXIT_TIMER_ID);
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_POPUP_PRIORITY,
				NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, NX_PRISM_KEYBOARD_INPUT_NORMAL, (HINT32)stSvcInfo->name);

	NX_COMMON_NOT_USED_PARAM(p3);
	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_OptionActionSaveRename(NxUiInfoHubContents_t *pstContents)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t 	*stSvcInfo = NULL;
	HCHAR			*pszStr = NULL;
	HUINT8			*pszStringBuf = NULL;


	HxSTD_MemSet(pstContents->szRenameText, 0, sizeof(HUINT8) * DxNAME_LEN);
	pstContents->szRenameText[0] = '\0';
	pszStr = NX_PRISM_KeyBoard_GetResultString();
	HxSTD_StrNCpy(pstContents->szRenameText, pszStr, DxNAME_LEN);
	if(pszStr)
	{
		NX_APP_Free(pszStr);
		pszStr = NULL;
	}
	NX_PRISM_KeyBoard_Destroy();

	NX_APP_Info("Action - eNxChlist_OptItem_Rename - Save !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurChannelIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurChannelIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return ONDK_RET_FAIL;
	}

	if (HxSTD_StrCmp((char*)stSvcInfo->name, (char*)pstContents->szRenameText) == 0)
	{
		NX_APP_Info("Not Change Service Name stSvcInfo->name:(%s), pstContents->szRenameText(%s) !!\n", stSvcInfo->name, pstContents->szRenameText);
	}
	else
	{
		HxSTD_StrNCpy(stSvcInfo->name, pstContents->szRenameText, DxNAME_LEN);

		/* Update Service Info */
		NX_CHANNEL_UpdateService(stSvcInfo);

		/* Update Focus Item lock Info */
		nx_infohub_ResetProgrammeTimer(pstContents);
	}

	/* String Buffer 초기화. */
	pszStringBuf = NX_PRISM_GetTextBuffer(NX_TEXT_SIZE_512);
	if (pszStringBuf)
	{
		HxSTD_MemSet(pszStringBuf, 0x00, NX_TEXT_SIZE_512);
		HxSTD_sprintf((char*)pszStringBuf, (char*)ONDK_GetString(RES_MESG_6113_ID), pstContents->szRenameText);

		/* Show update Info Message */
		nx_infohub_CreateResponseDialouge(pszStringBuf);
	}
	else
	{
		/* Show update Info Message */
		nx_infohub_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_6113_ID));
	}

	return ONDK_RET_OK;
}



/******************************************************************************/
#define __InfoHub_Functions______________________________________________
/******************************************************************************/

static NxUiInfoHubContents_t*	nx_infohub_GetContents(void)
{
	return &s_UiInfoHubContents;
}


static ONDK_Result_t	nx_infohub_ResetProgrammeTimer(NxUiInfoHubContents_t *pstContents)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;
	// set timer
	retTimer = ONDK_GWM_SetTimer(NX_PRISM_INFOHUB_PROGRAMME_TIMER_ID, NX_PRISM_INFOHUB_PROGRAMME_UPDATE_TIME);

	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_PRISM_INFOHUB_PROGRAMME_TIMER_ID);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return retTimer;
}


static ONDK_Result_t	nx_infohub_ResetExitInfoHubTimer(NxUiInfoHubContents_t *pstContents)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;
	// set timer
	retTimer = ONDK_GWM_SetTimer(NX_PRISM_INFOHUB_EXIT_TIMER_ID, NX_PRISM_INFOHUB_EXIT_EXPIRE_TIME);

	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_PRISM_INFOHUB_EXIT_TIMER_ID);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return retTimer;
}


static ONDK_Result_t nx_infohub_LeftKeyDown(NxUiInfoHubContents_t *pstContents)
{
	NX_QUERY_t	endQuery, startQuery, andQuery;
	HUINT32 	unixtime = 0, tempTime = 0, cur_unixtime = 0;
	HCHAR		value[NX_TEXT_SIZE_64] = {'\0',};

	if (pstContents->nProgrammeIndex <= NX_PRISM_INFOHUB_FIRST_INDEX)	// index 가 0 이라면..
	{
		NxInfoHubData_t *pstHubData = &pstContents->pstInfoHubData[0];
		if (pstHubData != NULL)
		{
			if (NX_SYSTEM_GetTime(&cur_unixtime) != ERR_OK)
			{
				HxLOG_Error("time is Invalid!!!\n");
				return ONDK_RET_FAIL;
			}

			// memory 사용을 늘리지 않기위해 , index 0 이 current time 과 같은 programme 이 아니라면.
			// else 를 통해 재쿼리를 한다.
			if (pstHubData->startTime < cur_unixtime &&
				(pstHubData->startTime + pstHubData->duration) > cur_unixtime)
			{
				pstContents->nProgrammeIndex = NX_PRISM_INFOHUB_FIRST_INDEX;
				pstContents->pushKey = eNxHubKey_NONE;
			}
			else
			{
				// 이전 시간ㄷ늘에 대해 query
				// endtime 을 구해야하는데,,
				tempTime = pstHubData->startTime - (SECONDS_PER_HOUR * 6);

				if (tempTime < cur_unixtime)
					unixtime = cur_unixtime;
				else
					unixtime = tempTime;

				HxSTD_sprintf(value, "%u", unixtime);
				endQuery = NX_METADATA_SEARCH_CreateQuery("endTime", ">", value);

				unixtime += SECONDS_PER_HOUR * 12; // plus 12 hour
				pstContents->lastSearchStartTime = unixtime;
				HxSTD_sprintf(value, "%u", unixtime);
				startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);

				andQuery = NX_PORT_META_QUERY_AND(endQuery, startQuery);

				NX_PORT_META_QUERY_DeleteQuery(endQuery);
				NX_PORT_META_QUERY_DeleteQuery(startQuery);

				// 아래 push key 는 epg update message 를 받은후 처리.
				pstContents->pushKey = eNxHubKey_LEFT_DOWN;

				nx_infohub_SearchEpg(pstContents, andQuery);	// andQuery 는 안에서 지움.
			}
		}
	}
	else
	{
		pstContents->nProgrammeIndex--;
		pstContents->pushKey = eNxHubKey_NONE;
		nx_infohub_SetInfoHubData(pstContents, TRUE);
		nx_infohub_UpdateObjects(pstContents, 0, 0, 0, 0);
	}
	return ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t nx_infohub_RightKeyDown(NxUiInfoHubContents_t *pstContents)
{
	HUINT32	 	lastSearchTime = 0, unixtime = 0;
	NX_QUERY_t	endQuery, startQuery, andQuery;
	HCHAR		value[NX_TEXT_SIZE_64] = {'\0',};


	// epg query 를 다시 해야 하는지 판단.
	NxInfoHubData_t *pstHubData = &pstContents->pstInfoHubData[0];
	if (pstHubData != NULL)
	{
		lastSearchTime = pstContents->lastSearchStartTime - (SECONDS_PER_HOUR * 3);

		if ( lastSearchTime < pstHubData->startTime )
		{
			unixtime = pstHubData->startTime;
			HxSTD_sprintf(value, "%u", unixtime);
			endQuery = NX_METADATA_SEARCH_CreateQuery("endTime", ">", value);

			unixtime += SECONDS_PER_HOUR * 12; // plus 12 hour
			pstContents->lastSearchStartTime = unixtime;
			HxSTD_sprintf(value, "%u", unixtime);
			startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);

			andQuery = NX_PORT_META_QUERY_AND(endQuery, startQuery);

			NX_PORT_META_QUERY_DeleteQuery(endQuery);
			NX_PORT_META_QUERY_DeleteQuery(startQuery);

			// 아래 push key 는 epg update message 를 받은후 처리.
			pstContents->pushKey = eNxHubKey_RIGHT_DOWN;

			nx_infohub_SearchEpg(pstContents, andQuery);	// andQuery 는 안에서 지움.
		}
		else
		{
			pstContents->nProgrammeIndex++;
			pstContents->pushKey = eNxHubKey_NONE;
			nx_infohub_SetInfoHubData(pstContents, TRUE);
			nx_infohub_UpdateObjects(pstContents, 0, 0, 0, 0);
		}
	}

	return	ONDK_RET_OK;
}


static HERROR	nx_infohub_SearchEpg(NxUiInfoHubContents_t *pstContents, NX_QUERY_t query)
{
	NX_MetadataSearch_t *pstMetaSearch = NULL;
	pstMetaSearch = NX_METADATA_SEARCH_New(eNX_META_SEARCH_TARGET_SCHEDULED);
	if (pstMetaSearch == NULL)
	{
		return ERR_FAIL;
	}
	NX_PORT_META_QUERY_SetQuery(pstMetaSearch->context, query);
	NX_PORT_META_QUERY_DeleteQuery(query);

	NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, &pstContents->pstChannels[pstContents->nCurChannelIndex]);
	NX_METADATA_EPG_SearchEPG(pstMetaSearch);

	return ERR_OK;
}


static HERROR	nx_infohub_RebuildingProgrammes(NxUiInfoHubContents_t *pstContents)
{
	NX_Channel_t *channel	= NULL;
	// epg update 가 온후, index 0 을 setting 하기 위한 작업.
	HINT32 num = 0, i = 0;
	HxVector_t *result = HLIB_VECTOR_New(NULL,NULL);
	NxInfoHubData_t *pstHubData = &pstContents->pstInfoHubData[0];

	if (pstHubData == NULL)
	{
		HxLOG_Error("pstHubData is NULL! \n");
		return ERR_FAIL;
	}

	channel = &pstContents->pstChannels[pstContents->nCurChannelIndex];

	NX_METADATA_EPG_GetResultProgrammeBySvcUid(channel->svcUid, result, &num);

	if (result != NULL)
	{
		for (i=0; i < num; )
		{
			NX_Programme_t *item = HLIB_VECTOR_ItemAt(result, i);

			if (item)		// 일단.. current time 에 대해서만..
			{
				// start Time
				HUINT32 startTime = NX_METADATA_PROGRAMMES_GetStartTime(item);

				// duration
				HUINT32 duration  = NX_METADATA_PROGRAMMES_GetDuration(item);

				if ( (startTime >= pstHubData->startTime) 							&&
					 startTime < (pstHubData->startTime + pstHubData->duration)		&&
					 duration == pstHubData->duration)
				{
					pstContents->nProgrammeIndex = i;
				}
			}
			i++;
		}
	}
	return ERR_OK;
}


static NX_Programme_t * nx_infohub_GetCurrentProgramme(NxUiInfoHubContents_t *pstContents)
{
	NX_Channel_t *channel 	= NULL;
	HINT32 num = 0;
	HxVector_t *result = HLIB_VECTOR_New(NULL,NULL);

	channel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
	if (channel != NULL)
	{
		NX_METADATA_EPG_GetResultProgrammeBySvcUid(channel->svcUid, result, &num);
		if (result)
		{
			int i = 0;
			for (i=0; i < num; i++)
			{
				NX_Programme_t *item = (NX_Programme_t*)HLIB_VECTOR_ItemAt(result, i);

				if (item && (pstContents->nProgrammeIndex == i))		// 일단.. current time 에 대해서만..
				{
					HLIB_VECTOR_Delete(result);		// new list 만 날리고, core epg 의 data 는 날리면 안됨.
					return item;
				}
			}
		}

		HLIB_VECTOR_Delete(result);		// new list 만 날리고, core epg 의 data 는 날리면 안됨.
	}
	return NULL;
}


static HERROR	nx_infohub_UpdateTimeAndEventTime(NxUiInfoHubContents_t *pstContents)
{
	HBOOL			bCurrentEvent = FALSE;
	HUINT32			unixtime = 0, weekIndex = 0;
	HxDATETIME_t	datetime;
	ONDK_GWM_GuiObject_t	programmeImg;
	NxInfoHubData_t			*pstHubData = &pstContents->pstInfoHubData[0];


	if (NX_SYSTEM_GetTime(&unixtime) == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(unixtime, &datetime);
		weekIndex = HLIB_DATETIME_GetWeekDayIndex(datetime.stDate);
		pstHubData->datetime = datetime;
		pstHubData->pszDayOfWeek = (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(weekIndex));

		HxSTD_sprintf(pstHubData->pszDate, "%02d.%02d.%04d", datetime.stDate.ucDay,
						datetime.stDate.ucMonth, datetime.stDate.usYear);
		HxSTD_sprintf(pstHubData->pszTime, "%s %02d:%02d", pstHubData->pszDayOfWeek,
						datetime.stTime.ucHour, datetime.stTime.ucMinute);

		HxLOG_Debug("\ndatetime : %4d.%2d.%2d  %s %2d:%2d\n\n",	datetime.stDate.usYear, datetime.stDate.ucMonth,
			datetime.stDate.ucDay, ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(weekIndex)),
			datetime.stTime.ucHour, datetime.stTime.ucMinute);
	}

	// Update Time
	programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_DATE_TEXT_ID);
	if (programmeImg != NULL)
	{
		ONDK_GWM_COM_Text_SetText(NX_INFOHUB_DATE_TEXT_ID, pstHubData->pszDate);
	}
	programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_TIME_TEXT_ID);
	if (programmeImg != NULL)
	{
		ONDK_GWM_COM_Text_SetText(NX_INFOHUB_TIME_TEXT_ID, pstHubData->pszTime);
	}

	// Event Progress
	if (HLIB_DATETIME_IsValidUnixTime(pstHubData->startTime) == ERR_OK)
	{
		bCurrentEvent = (pstHubData->startTime < unixtime) ? TRUE : FALSE ;
	}
	if (bCurrentEvent == TRUE)
	{
		programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID);
		if (programmeImg != NULL)
		{
			ONDK_GWM_COM_Status_SetValue(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID, (unixtime - pstHubData->startTime));
		}
	}

	return	ERR_OK;
}


static HERROR	nx_infohub_UpdateObjects(NxUiInfoHubContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bCurrentEvent = FALSE;
	HUINT32			unixtime = 0, weekIndex = 0, calcMin = 0, totalMin = 0, calcHour = 0, ulProgress = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HxDATETIME_t	StartTime;
	HxDATETIME_Time_t		DurationTime;
	NxInfoHubData_t			*pstHubData = &pstContents->pstInfoHubData[0];
	ONDK_GWM_GuiObject_t	programmeImg;


	//****** date & time info box ******//
	// set date time
	hError = NX_SYSTEM_GetTime(&unixtime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetTime() Fail !!\n");
		return	hError;
	}

	nx_infohub_UpdateTimeAndEventTime(pstContents);

	ONDK_GWM_COM_Text_SetText(NX_INFOHUB_CHBOX_LCN_TEXT_ID, pstHubData->pszLcn);
	ONDK_GWM_COM_Text_SetText(NX_INFOHUB_CHBOX_NAME_TEXT_ID, pstHubData->szChName);
	ONDK_GWM_COM_Text_SetText(NX_INFOHUB_EVENTNAME_TEXT_ID, pstHubData->pszEventName);

	// Event Progress
	if (HLIB_DATETIME_IsValidUnixTime(pstHubData->startTime) == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(pstHubData->startTime, &StartTime);
		HLIB_DATETIME_ConvertUnixTimeToTimeWithOutOffset(pstHubData->duration, &DurationTime);
		weekIndex = HLIB_DATETIME_GetWeekDayIndex(StartTime.stDate);
		totalMin = StartTime.stTime.ucMinute + DurationTime.ucMinute;
		calcHour = totalMin / 60;
		calcMin  = totalMin % 60;

		bCurrentEvent = (pstHubData->startTime < unixtime) ? TRUE : FALSE ;
		if (bCurrentEvent == TRUE)
		{
			HxSTD_snprintf(pstHubData->pszStartTime, NX_TEXT_SIZE_64, "%02d:%02d",
							StartTime.stTime.ucHour, StartTime.stTime.ucMinute);

			HxSTD_snprintf(pstHubData->pszDuration, NX_TEXT_SIZE_64, "%02d:%02d",
							(StartTime.stTime.ucHour + DurationTime.ucHour + calcHour), calcMin);
		}
		else
		{
			HxSTD_snprintf(pstHubData->pszStartTime, NX_TEXT_SIZE_64, "%02d:%02d ~ %02d:%02d, %s %02d %s (%dmin)",
							StartTime.stTime.ucHour, StartTime.stTime.ucMinute,
							(StartTime.stTime.ucHour + DurationTime.ucHour + calcHour), calcMin,
							ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(weekIndex)),
							StartTime.stDate.ucDay, ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByMonth(StartTime.stDate.ucMonth)),
							((DurationTime.ucHour *NX_COMMON_TIME_MINS_PER_HOUR) + DurationTime.ucMinute));

			HxSTD_MemSet(pstHubData->pszDuration, 0, NX_TEXT_SIZE_64);
			pstHubData->pszDuration[0] = '\0';
		}
		programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID);
		if (programmeImg != NULL)
		{
			ONDK_GWM_COM_Text_SetText(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, pstHubData->pszStartTime);
		}
		else
		{
			stRect = ONDK_Rect(NX_INFOHUB_PROGRAMME_DURATION_TEXT_X, NX_INFOHUB_PROGRAMME_DURATION_TEXT_Y,
								NX_INFOHUB_PROGRAMME_DURATION_TEXT_W, NX_INFOHUB_PROGRAMME_DURATION_TEXT_H);
			ONDK_GWM_COM_Text_Create(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, &stRect, pstHubData->pszStartTime);
			ONDK_GWM_COM_Text_SetFont(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, eFont_SystemNormal);
			ONDK_GWM_COM_Text_SetSize(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, NX_PRISM_FONT_SIZE_20);
			ONDK_GWM_COM_Text_SetAlign(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, TEXT_ALIGN_LEFT);
			ONDK_GWM_COM_Text_ChangeColor(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));
		}

		if (bCurrentEvent == TRUE)
		{
			programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID);
			if (programmeImg != NULL)
			{
				ONDK_GWM_COM_Text_SetText(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, pstHubData->pszDuration);
			}
			else
			{
				// End Time
				stRect = ONDK_Rect(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_X, NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_Y,
									NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_W, NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_H);
				ONDK_GWM_COM_Text_Create(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, &stRect, pstHubData->pszDuration);
				ONDK_GWM_COM_Text_SetFont(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, eFont_SystemNormal);
				ONDK_GWM_COM_Text_SetSize(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, NX_PRISM_FONT_SIZE_20);
				ONDK_GWM_COM_Text_SetAlign(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, TEXT_ALIGN_RIGHT);
				ONDK_GWM_COM_Text_ChangeColor(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));
			}

			programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID);
			if (programmeImg != NULL)
			{
				ONDK_GWM_Obj_Destroy(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID);
			}

			stRect = ONDK_Rect(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_X, NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_Y,
								NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_W, NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_H);
			if (unixtime > pstHubData->startTime)
				ulProgress = (unixtime - pstHubData->startTime);
			ONDK_GWM_COM_Status_Create(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID, &stRect, pstHubData->duration, ulProgress);
			ONDK_GWM_COM_Status_SetColor(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID, COL(C_G_Whistle_30), COL(C_G_Whistle_100));
			ONDK_GWM_APP_SetObjectDrawMethod(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID, NX_PRISM_COMP_DrawStatusBarA1);
			ONDK_GWM_COM_Status_SetValue(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID, ulProgress);
		}
		else
		{
			programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID);
			if (programmeImg != NULL)
			{
				ONDK_GWM_Obj_Destroy(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID);
			}
		}
	}

	ONDK_GWM_APP_InvalidateRect(NX_INFOHUB_FRAME_X, NX_INFOHUB_FRAME_Y, NX_INFOHUB_FRAME_W, NX_INFOHUB_FRAME_H);
	ONDK_GWM_APP_InvalidateObject(NX_INFOHUB_ICON_LIST_ID);
	NX_COMMON_NOT_USED_ARGUMENT(hAct, p1, p2, p3);

	return	ERR_OK;
}


static	ONDK_Result_t	nx_infohub_DrawIconLists(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
													ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HCHAR				*pszImgStr = NULL, *ratingRes = NULL, *casTypeRes = NULL, *resSubtitle = NULL, *resImage = NULL;
	HINT32				lOffsetX = 0;
	ONDK_Rect_t 		stRect;
	NxInfoHubData_t			*pstHubData = NULL;
	NxUiInfoHubContents_t	*pstContents = NULL;


	pstContents = nx_infohub_GetContents();
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	pstHubData = &pstContents->pstInfoHubData[0];

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;


	lOffsetX = stRect.x;
	// TODO: [JHLEE] Add Recored file Check
#if defined(CONFIG_MW_MM_PVR)
#if 0
	{
		stRect.x = lOffsetX;
		stRect.w = NX_INFOHUB_PROGRAMME_ICON_RECORDED_W;
		pszImgStr = RES_205_REC_RECORDING_WATCH_PNG;
		ONDK_GUICOM_IMG_Blit(screen, stRect, pszImgStr);
		lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
	}
#endif
#endif

	// TODO: [JHLEE] Add Ondemand Check
#if 0
	{
		stRect.x = lOffsetX;
		stRect.w = NX_INFOHUB_PROGRAMME_ICON_ONDEMAND_W;
		pszImgStr = NULL;
		ONDK_GUICOM_IMG_Blit(screen, stRect, pszImgStr);
		lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
	}
#endif

	if (pstHubData->isHD)
	{
		stRect.x = lOffsetX;
		stRect.w = NX_INFOHUB_PROGRAMME_ICON_HD_W;
		pszImgStr = RES_203_00_RESOLUTION_HD_PNG;
		ONDK_GUICOM_IMG_Blit(screen, stRect, pszImgStr);
		lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
	}

	// Rating
	ratingRes = NX_PRISM_UTIL_GetRatingResource(pstHubData->rating);
	if (ratingRes != NULL)
	{
		stRect.x = lOffsetX;
		stRect.w = NX_INFOHUB_PROGRAMME_ICON_RATE_W;
		ONDK_GUICOM_IMG_Blit(screen, stRect, ratingRes);
		lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
	}

	// cas type
	casTypeRes = NX_PRISM_UTIL_GetCasTypeResource(pstHubData->casId);
	if (casTypeRes != NULL)
	{
		stRect.x = lOffsetX;
		stRect.w = NX_INFOHUB_PROGRAMME_ICON_CAS_W;
		ONDK_GUICOM_IMG_Blit(screen, stRect, casTypeRes);
		lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
	}

	// TTX
	if (pstHubData->hasTeletext != 0)
	{
		stRect.x = lOffsetX;
		stRect.w = NX_INFOHUB_PROGRAMME_ICON_TTX_W;
		pszImgStr = RES_219_00_TELETEXT_PNG;
		ONDK_GUICOM_IMG_Blit(screen, stRect, pszImgStr);
		lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
	}

	// Subtitle
	if (pstHubData->subtitleType != 0)
	{
		resSubtitle = NX_PRISM_UTIL_GetSubtitleResource(pstHubData->subtitleType);
		if (resSubtitle != NULL)
		{
			stRect.x = lOffsetX;
			stRect.w = NX_INFOHUB_PROGRAMME_ICON_SUBTITLE_W;
			ONDK_GUICOM_IMG_Blit(screen, stRect, resSubtitle);
			lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
		}
	}

	// Multi Audio
	if (pstHubData->audioType == 0x04)
	{
		stRect.x = lOffsetX;
		stRect.w = NX_INFOHUB_PROGRAMME_ICON_MULTI_AUDIO_W;
		pszImgStr = RES_217_00_MULTIPLEAUDIO_PNG;
		ONDK_GUICOM_IMG_Blit(screen, stRect, pszImgStr);
		lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
	}

	// DRM
	// TODO: [JHLEE] Add DRM Check
#if 0
	{
		stRect.x = lOffsetX;
		stRect.w = NX_INFOHUB_PROGRAMME_ICON_DRM_W;
		pszImgStr = RES_211_LOCK_DRM_PNG;
		ONDK_GUICOM_IMG_Blit(screen, stRect, pszImgStr);
		lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
	}
#endif

	// Lock
	if (pstHubData->bIsLock == TRUE)
	{
		stRect.x = lOffsetX;
		stRect.w = NX_INFOHUB_PROGRAMME_ICON_LOCK_W;
		pszImgStr = RES_211_00_LOCK_PNG;
		ONDK_GUICOM_IMG_Blit(screen, stRect, pszImgStr);
		lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
	}

	// Reservaion
	if (pstContents->pstInfoHubData[0].eRsvType != DxRSVTYPE_NOTHING)
	{
		stRect.x = lOffsetX;
		stRect.w = NX_INFOHUB_PROGRAMME_ICON_RESERVATION_W;
		resImage = NX_PRISM_UTIL_GetReservationResource(pstContents->pstInfoHubData[0].eRsvType);
		if (resImage != NULL)
		{
			ONDK_GUICOM_IMG_Blit(screen, stRect, resImage);
			lOffsetX = (stRect.x + stRect.w + NX_INFOHUB_PROGRAMME_ICON_MG);
		}
	}

	return ONDK_RET_OK;
}


#if defined(CONFIG_MW_MM_PVR)
static void		nx_infohub_SetUiRecordingStatue(NxUiInfoHubContents_t *pstContents)
{
	ONDK_Rect_t			stRect;
	HUINT32				ulRecordingCount = 0;
	ONDK_GWM_GuiObject_t	programmeImg;


	stRect = ONDK_Rect(NX_INFOHUB_RECORDING_ITEM_X, NX_INFOHUB_RECORDING_ITEM_Y,
						NX_INFOHUB_RECORDING_ITEM_W, NX_INFOHUB_RECORDING_ITEM_H);

	// TODO: [JHLEE] Get Recording Count
	if (ulRecordingCount < 1)
	{
		programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_RECORDING_ITEM_ID);
		if (programmeImg != NULL)
		{
			ONDK_GWM_Obj_Destroy(NX_INFOHUB_RECORDING_ITEM_ID);
			return;
		}
		else
		{
			return;
		}
	}

	HxSTD_sprintf(pstContents->szRecordingText, "%d %s", ulRecordingCount, (HCHAR*)ONDK_GetString(RES_RECORDING_ID));
	programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_RECORDING_ITEM_ID);
	if (programmeImg == NULL)
	{
		ONDK_GWM_COM_Multi_Create(NX_INFOHUB_RECORDING_ITEM_ID, &stRect);
		ONDK_GWM_COM_Multi_SetContents(NX_INFOHUB_RECORDING_ITEM_ID, (HUINT8*)RES_205_04_REC_RECORDING_PNG,
									NULL, (HUINT8*)pstContents->szRecordingText, NULL, NX_INFOHUB_RECORDING_ITEM_MG);
		ONDK_GWM_COM_Multi_SetFontHandle(NX_INFOHUB_RECORDING_ITEM_ID, eFont_SystemNormal, NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Multi_SetFontColor(NX_INFOHUB_RECORDING_ITEM_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
		ONDK_GWM_COM_Multi_SetFocusFlag(NX_INFOHUB_RECORDING_ITEM_ID, FALSE);
	}
	else
	{
		ONDK_GWM_COM_Multi_ChangeLeftStr(NX_INFOHUB_RECORDING_ITEM_ID, pstContents->szRecordingText);
	}
}
#endif


#if defined(CONFIG_MW_MM_DLNA_LIVESTREAMING)
static void		nx_infohub_SetUiLiveStreamingStatue(NxUiInfoHubContents_t *pstContents)
{
	ONDK_Rect_t			stRect;
	HUINT32				ulLiveStreamingCount = 0;
	ONDK_GWM_GuiObject_t	programmeImg;


	stRect = ONDK_Rect(NX_INFOHUB_LIVESTREAMING_ITEM_X, NX_INFOHUB_LIVESTREAMING_ITEM_Y,
						NX_INFOHUB_LIVESTREAMING_ITEM_W, NX_INFOHUB_LIVESTREAMING_ITEM_H);

	// TODO: [JHLEE] Get Live Streaming Count

	if (ulLiveStreamingCount < 1)
	{
		programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_LIVESTREAMING_ITEM_ID);
		if (programmeImg != NULL)
		{
			ONDK_GWM_Obj_Destroy(NX_INFOHUB_LIVESTREAMING_ITEM_ID);
			return;
		}
		else
		{
			return;
		}
	}

	HxSTD_sprintf(pstContents->szLiveStremingText, "%d %s", ulLiveStreamingCount, (HCHAR*)ONDK_GetString(RES_LIVE_STREAMING_ID));
	programmeImg = ONDK_GWM_APP_GetObject(NX_INFOHUB_LIVESTREAMING_ITEM_ID);
	if (programmeImg == NULL)
	{
		ONDK_GWM_COM_Multi_Create(NX_INFOHUB_LIVESTREAMING_ITEM_ID, &stRect);
		ONDK_GWM_COM_Multi_SetContents(NX_INFOHUB_LIVESTREAMING_ITEM_ID, RES_205_REC_LIVE_PNG,
									NULL, (HUINT8*)pstContents->szLiveStremingText, NULL, NX_INFOHUB_LIVESTREAMING_ITEM_MG);
		ONDK_GWM_COM_Multi_SetFontHandle(NX_INFOHUB_LIVESTREAMING_ITEM_ID, eFont_SystemNormal, NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Multi_SetFontColor(NX_INFOHUB_LIVESTREAMING_ITEM_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
		ONDK_GWM_COM_Multi_SetFocusFlag(NX_INFOHUB_LIVESTREAMING_ITEM_ID, FALSE);
	}
	else
	{
		ONDK_GWM_COM_Multi_ChangeLeftStr(NX_INFOHUB_LIVESTREAMING_ITEM_ID, pstContents->szLiveStremingText);
	}
}
#endif


static void		nx_infohub_SetUiCreateOptionMenu(NxUiInfoHubContents_t *pstContents)
{
	HUINT8			stGroupStr[NX_TEXT_SIZE_128] = {'\0',};
	ONDK_Rect_t		stRect;
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;


	stRect = ONDK_Rect(NX_INFOHUB_DETAIL_OPTION_TEXT_X, NX_INFOHUB_DETAIL_OPTION_TEXT_Y,
						NX_INFOHUB_DETAIL_OPTION_TEXT_W, NX_INFOHUB_DETAIL_OPTION_TEXT_H);

	eCurGroup = pstContents->eChangedGroup;

	/* eNxChListGroup_AlphabetAll ~ eNxChListGroup_AlphabetEtc is included in the eNxChListGroup_ALPHABET Group */
	if ((eCurGroup >= eNxChListGroup_AlphabetAll) && (eCurGroup <= eNxChListGroup_AlphabetEtc))
	{
		eCurGroup = eNxChListGroup_ALPHABET;
	}
	NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(eCurGroup, (HCHAR*)stGroupStr);

	HxSTD_sprintf(pstContents->szOptionText, "%s - %s", (HCHAR*)ONDK_GetString(RES_OPTION_01_ID), (HCHAR*)stGroupStr);
	ONDK_GWM_COM_Multi_Create(NX_INFOHUB_DETAIL_OPTION_TEXT_ID, &stRect);
	ONDK_GWM_COM_Multi_SetContents(NX_INFOHUB_DETAIL_OPTION_TEXT_ID, (HUINT8*)RES_201_00_OPTION_PNG,
								NULL, (HUINT8*)pstContents->szOptionText, NULL, NX_INFOHUB_DETAIL_OPTION_IMG_MG);
	ONDK_GWM_COM_Multi_SetFontHandle(NX_INFOHUB_DETAIL_OPTION_TEXT_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Multi_SetFontColor(NX_INFOHUB_DETAIL_OPTION_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));
	ONDK_GWM_COM_Multi_SetFocusFlag(NX_INFOHUB_DETAIL_OPTION_TEXT_ID, FALSE);
}


static HERROR	nx_infohub_SetUiObject(NxUiInfoHubContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL				bCurrentEvent = FALSE;
	HUINT32				unixtime = 0, weekIndex = 0, calcHour = 0, calcMin = 0, totalMin = 0, ulProgress = 0;
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	HxDATETIME_t 		datetime, StartTime;
	HxDATETIME_Time_t	DurationTime;
	NxInfoHubData_t 	*pstHubData = &pstContents->pstInfoHubData[0];

	// frame
	ONDK_GWM_APP_SetAppArea(NX_INFOHUB_FRAME_X, NX_INFOHUB_FRAME_Y, NX_INFOHUB_FRAME_W, NX_INFOHUB_FRAME_H);

	// Draw - Background Frame
	stRect = ONDK_Rect(NX_INFOHUB_BG_X, NX_INFOHUB_FRAME_Y, NX_INFOHUB_FRAME_W, NX_INFOHUB_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_INFOHUB_BG_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_INFOHUB_BG_ID, NX_PRISM_COMP_DrawFrameBG_SS);

	//****** date & time info box ******//
	// set date time
	if (NX_SYSTEM_GetTime(&unixtime) == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(unixtime, &datetime);
		weekIndex = HLIB_DATETIME_GetWeekDayIndex(datetime.stDate);
		pstHubData->datetime = datetime;
		pstHubData->pszDayOfWeek = (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(weekIndex));

		HxSTD_sprintf(pstHubData->pszDate, "%02d.%02d.%04d", datetime.stDate.ucDay,	datetime.stDate.ucMonth,
										datetime.stDate.usYear);

		HxSTD_sprintf(pstHubData->pszTime, "%s %02d:%02d", pstHubData->pszDayOfWeek, datetime.stTime.ucHour,
												datetime.stTime.ucMinute);

		HxLOG_Debug("\ndatetime : %4d.%2d.%2d  %s %2d:%2d\n\n",	datetime.stDate.usYear, datetime.stDate.ucMonth, datetime.stDate.ucDay,
			ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(weekIndex)),	datetime.stTime.ucHour, datetime.stTime.ucMinute);
	}

	// date
	stRect = ONDK_Rect(NX_INFOHUB_DATE_TEXT_X, NX_INFOHUB_DATE_TEXT_Y, NX_INFOHUB_DATE_TEXT_W, NX_INFOHUB_DATE_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_INFOHUB_DATE_TEXT_ID, &stRect, pstHubData->pszDate);
	ONDK_GWM_COM_Text_SetFont(NX_INFOHUB_DATE_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_INFOHUB_DATE_TEXT_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_INFOHUB_DATE_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_INFOHUB_DATE_TEXT_ID, COL(C_T_Gong01), COL(C_T_Bang_100));

	// time
	stRect = ONDK_Rect(NX_INFOHUB_TIME_TEXT_X, NX_INFOHUB_TIME_TEXT_Y, NX_INFOHUB_TIME_TEXT_W, NX_INFOHUB_TIME_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_INFOHUB_TIME_TEXT_ID, &stRect, pstHubData->pszTime);
	ONDK_GWM_COM_Text_SetFont(NX_INFOHUB_TIME_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_INFOHUB_TIME_TEXT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_INFOHUB_TIME_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_INFOHUB_TIME_TEXT_ID, COL(C_T_Gong01), COL(C_T_Bang_100));

	//***** arrow image ******//
	// up arrow
	stRect = ONDK_Rect(NX_INFOHUB_BASEARROW_UP_IMG_X, NX_INFOHUB_BASEARROW_UP_IMG_Y,
						NX_INFOHUB_BASEARROW_UP_IMG_W, NX_INFOHUB_BASEARROW_UP_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_INFOHUB_BASEARROW_UP_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_INFOHUB_BASEARROW_UP_IMG_ID, (HCHAR*)RES_106_00_ARROW_UP_PNG);

	// right arrow
	stRect = ONDK_Rect(NX_INFOHUB_BASEARROW_RIGHT_IMG_X, NX_INFOHUB_BASEARROW_RIGHT_IMG_Y,
						NX_INFOHUB_BASEARROW_RIGHT_IMG_W, NX_INFOHUB_BASEARROW_RIGHT_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_INFOHUB_BASEARROW_RIGHT_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_INFOHUB_BASEARROW_RIGHT_IMG_ID, (HCHAR*)RES_106_00_ARROW_RIGHT_PNG);

	// Down arrow
	stRect = ONDK_Rect(NX_INFOHUB_BASEARROW_DOWN_IMG_X, NX_INFOHUB_BASEARROW_DOWN_IMG_Y,
						NX_INFOHUB_BASEARROW_DOWN_IMG_W, NX_INFOHUB_BASEARROW_DOWN_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_INFOHUB_BASEARROW_DOWN_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_INFOHUB_BASEARROW_DOWN_IMG_ID, (HCHAR*)RES_106_00_ARROW_DOWN_PNG);

	//****** channel box ******//
	// Cursor
	stRect = ONDK_Rect(NX_INFOHUB_CHBOX_X, NX_INFOHUB_CHBOX_Y, NX_INFOHUB_CHBOX_W, NX_INFOHUB_CHBOX_H);
	ONDK_GWM_COM_Rect_Create(NX_INFOHUB_CHBOX_ID, &stRect, COL(C_G_Base01_100));

	// Line
	stRect = ONDK_Rect(NX_INFOHUB_CHBOX_LINE_X, NX_INFOHUB_CHBOX_LINE_Y,
						NX_INFOHUB_CHBOX_LINE_W, NX_INFOHUB_CHBOX_LINE_H);
	ONDK_GWM_COM_Image_Create(NX_INFOHUB_CHBOX_LINE_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_INFOHUB_CHBOX_LINE_ID, (HCHAR*)RES_107_00_LINE_LTV_CH_IHUB_PNG);

	// lcn
	stRect = ONDK_Rect(NX_INFOHUB_CHBOX_LCN_TEXT_X, NX_INFOHUB_CHBOX_LCN_TEXT_Y,
						NX_INFOHUB_CHBOX_LCN_TEXT_W, NX_INFOHUB_CHBOX_LCN_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_INFOHUB_CHBOX_LCN_TEXT_ID, &stRect, pstHubData->pszLcn);
	ONDK_GWM_COM_Text_SetFont(NX_INFOHUB_CHBOX_LCN_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_INFOHUB_CHBOX_LCN_TEXT_ID, NX_PRISM_FONT_SIZE_36);
	ONDK_GWM_COM_Text_SetAlign(NX_INFOHUB_CHBOX_LCN_TEXT_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_INFOHUB_CHBOX_LCN_TEXT_ID, COL(C_T_Bang_100), COL(C_T_Bang_100));

	// channel name
	stRect = ONDK_Rect(NX_INFOHUB_CHBOX_NAME_TEXT_X, NX_INFOHUB_CHBOX_NAME_TEXT_Y,
						NX_INFOHUB_CHBOX_NAME_TEXT_W, NX_INFOHUB_CHBOX_NAME_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_INFOHUB_CHBOX_NAME_TEXT_ID, &stRect, pstHubData->szChName);
	ONDK_GWM_COM_Text_SetFont(NX_INFOHUB_CHBOX_NAME_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_INFOHUB_CHBOX_NAME_TEXT_ID, NX_PRISM_FONT_SIZE_36);
	ONDK_GWM_COM_Text_SetAlign(NX_INFOHUB_CHBOX_NAME_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_INFOHUB_CHBOX_NAME_TEXT_ID, COL(C_T_Bang_100), COL(C_T_Bang_100));

	//****** event info *******//
	// event name
	stRect = ONDK_Rect(NX_INFOHUB_EVENTNAME_TEXT_X, NX_INFOHUB_EVENTNAME_TEXT_Y, NX_INFOHUB_EVENTNAME_TEXT_W, NX_INFOHUB_EVENTNAME_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_INFOHUB_EVENTNAME_TEXT_ID, &stRect, pstHubData->pszEventName);
	ONDK_GWM_COM_Text_SetFont(NX_INFOHUB_EVENTNAME_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_INFOHUB_EVENTNAME_TEXT_ID, NX_PRISM_FONT_SIZE_32);
	ONDK_GWM_COM_Text_SetAlign(NX_INFOHUB_EVENTNAME_TEXT_ID, TEXT_ALIGN_LEFT | TEXT_MULTILINED | TEXT_WORDWRAP);
	ONDK_GWM_COM_Text_ChangeColor(NX_INFOHUB_EVENTNAME_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	stRect = ONDK_Rect(NX_INFOHUB_ICON_LIST_X, NX_INFOHUB_ICON_LIST_Y, NX_INFOHUB_ICON_LIST_W, NX_INFOHUB_ICON_LIST_H);
	ONDK_GWM_COM_Frame_Create(NX_INFOHUB_ICON_LIST_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_INFOHUB_ICON_LIST_ID, nx_infohub_DrawIconLists);

	// Draw Icon
	//nx_infohub_DrawIconList(pstContents);

	// duration
	if (HLIB_DATETIME_IsValidUnixTime(pstHubData->startTime) == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(pstHubData->startTime, &StartTime);
		HLIB_DATETIME_ConvertUnixTimeToTimeWithOutOffset(pstHubData->duration, &DurationTime);
		weekIndex = HLIB_DATETIME_GetWeekDayIndex(StartTime.stDate);
		totalMin = StartTime.stTime.ucMinute + DurationTime.ucMinute;
		calcHour = totalMin / 60;
		calcMin  = totalMin % 60;

		bCurrentEvent = (pstHubData->startTime < unixtime) ? TRUE : FALSE ;
		if (bCurrentEvent == TRUE)
		{
			HxSTD_snprintf(pstHubData->pszStartTime, NX_TEXT_SIZE_64, "%02d:%02d",
							StartTime.stTime.ucHour, StartTime.stTime.ucMinute);

			HxSTD_snprintf(pstHubData->pszDuration, NX_TEXT_SIZE_64, "%02d:%02d",
							(StartTime.stTime.ucHour + DurationTime.ucHour + calcHour), calcMin);
		}
		else
		{
			HxSTD_snprintf(pstHubData->pszStartTime, NX_TEXT_SIZE_64, "%02d:%02d ~ %02d:%02d, %s %02d %s (%dmin)",
							StartTime.stTime.ucHour, StartTime.stTime.ucMinute,
							(StartTime.stTime.ucHour + DurationTime.ucHour + calcHour),calcMin,
							ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(weekIndex)),
							StartTime.stDate.ucDay, ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByMonth(StartTime.stDate.ucMonth)),
							((DurationTime.ucHour *60) + DurationTime.ucMinute));

			HxSTD_MemSet(pstHubData->pszDuration, 0, NX_TEXT_SIZE_64);
			pstHubData->pszDuration[0] = '\0';
		}
		// Start Time
		stRect = ONDK_Rect(NX_INFOHUB_PROGRAMME_DURATION_TEXT_X, NX_INFOHUB_PROGRAMME_DURATION_TEXT_Y,
							NX_INFOHUB_PROGRAMME_DURATION_TEXT_W, NX_INFOHUB_PROGRAMME_DURATION_TEXT_H);
		ONDK_GWM_COM_Text_Create(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, &stRect, pstHubData->pszStartTime);
		ONDK_GWM_COM_Text_SetFont(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, eFont_SystemNormal);
		ONDK_GWM_COM_Text_SetSize(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, NX_PRISM_FONT_SIZE_20);
		ONDK_GWM_COM_Text_SetAlign(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, TEXT_ALIGN_LEFT);
		ONDK_GWM_COM_Text_ChangeColor(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));

		// End Time
		stRect = ONDK_Rect(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_X, NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_Y,
							NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_W, NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_H);
		ONDK_GWM_COM_Text_Create(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, &stRect, pstHubData->pszDuration);
		ONDK_GWM_COM_Text_SetFont(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, eFont_SystemNormal);
		ONDK_GWM_COM_Text_SetSize(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, NX_PRISM_FONT_SIZE_20);
		ONDK_GWM_COM_Text_SetAlign(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, TEXT_ALIGN_RIGHT);
		ONDK_GWM_COM_Text_ChangeColor(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));

		if (bCurrentEvent == TRUE)
		{
			stRect = ONDK_Rect(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_X, NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_Y,
								NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_W, NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_H);
			if (unixtime > pstHubData->startTime)
				ulProgress = (unixtime - pstHubData->startTime);
			ONDK_GWM_COM_Status_Create(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID, &stRect, pstHubData->duration, ulProgress);
			ONDK_GWM_COM_Status_SetColor(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID, COL(C_G_Whistle_30), COL(C_G_Whistle_100));
			ONDK_GWM_APP_SetObjectDrawMethod(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID, NX_PRISM_COMP_DrawStatusBarA1);
			ONDK_GWM_COM_Status_SetValue(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID, ulProgress);
		}
	}

	// Recording
#if defined(CONFIG_MW_MM_PVR)
	nx_infohub_SetUiRecordingStatue(pstContents);
#endif

	// Live Streaming
#if defined(CONFIG_MW_MM_DLNA_LIVESTREAMING)
	nx_infohub_SetUiLiveStreamingStatue(pstContents);
#endif

	// Option
	nx_infohub_SetUiCreateOptionMenu(pstContents);
	NX_COMMON_NOT_USED_ARGUMENT(hAct, p1, p2, p3);

	return ERR_OK;
}


static HERROR			nx_infohub_SetInfoHubData (NxUiInfoHubContents_t *pstContents, HBOOL bProgramme)
{
	HERROR			hError = ERR_FAIL;
	HINT32			i = 0, num = 0, lScheduleIdx = 0, lScheduleNum = 0;
	HUINT32			ulViewId = 0, ulCompMaxNum = 0;
	HCHAR			szEventName[EPG_HUMAX_NAME_LEN] = {'\0',};
	NX_Channel_t	*stChannel 	= NULL;
	DxService_t		*pstTempSvc = NULL;
	DxSchedule_t	*pstScheduleItem = NULL;
	HxVector_t		*pstScheduleList = HLIB_VECTOR_New(NULL, NULL);


	NX_MEDIA_PLAY_GetMainViewId(&ulViewId);

	if (pstContents->pstInfoHubData == NULL) // 처음 없으면 만든다..
	{
		pstContents->pstInfoHubData = NX_APP_Calloc(sizeof(NxInfoHubData_t) * NX_PRISM_INFOHUB_ITEM_MAX);	// info hub 이 스타일이 달라지면 여러개가 될수도...
		if (pstContents->pstInfoHubData == NULL)
		{
			NX_APP_Error("Error!!! NX_APP_Calloc() Fail!! \n");
			return	ERR_FAIL;
		}
		HxSTD_MemSet(pstContents->pstInfoHubData, 0x00, sizeof(NxInfoHubData_t) * NX_PRISM_INFOHUB_ITEM_MAX);
	}
	else
	{
		HxSTD_MemSet(pstContents->pstInfoHubData, 0x00, sizeof(NxInfoHubData_t) * NX_PRISM_INFOHUB_ITEM_MAX);
	}

	stChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
	HxSTD_MemSet(&pstContents->pstInfoHubData[0], 0, sizeof(NxInfoHubData_t));
	HxSTD_sprintf(pstContents->pstInfoHubData[0].pszLcn, "%04d", stChannel->lcn);
	pstContents->pstInfoHubData[0].lcn	= stChannel->lcn;
	pstTempSvc = NX_CHANNEL_FindServiceBySvcUid(stChannel->svcUid);
	if (pstTempSvc)
	{
		HxSTD_snprintf(pstContents->pstInfoHubData[0].szChName, DxNAME_LEN, "%s", pstTempSvc->name);
		//pstContents->pstInfoHubData[0].pszChName = pstTempSvc->name;
		pstContents->pstInfoHubData[0].bIsLock = pstTempSvc->locked;
	}
	else
	{
		HxSTD_snprintf(pstContents->pstInfoHubData[0].szChName, DxNAME_LEN, "%s", ONDK_GetString(RES_UNKNOWN_ID));
	}

	if (bProgramme)
	{
		HxVector_t *result = HLIB_VECTOR_New(NULL, NULL);
		NX_METADATA_EPG_GetResultProgrammeBySvcUid(stChannel->svcUid, result, &num);
		for (i = 0; i < num; i++)
		{
			NX_Programme_t *item = HLIB_VECTOR_ItemAt(result, i);
			if (item && pstContents->nProgrammeIndex == i)		// 일단.. current time 에 대해서만..
			{
				// event name
				if (NX_METADATA_PROGRAMMES_GetName(item, szEventName) == ERR_OK)
				{
					HxSTD_MemSet(pstContents->pstInfoHubData[0].pszEventName, 0, EPG_HUMAX_NAME_LEN);
					HLIB_STD_StrUtf8NCpy(pstContents->pstInfoHubData[0].pszEventName, szEventName, EPG_HUMAX_NAME_LEN);
				}

				// is HD
				pstContents->pstInfoHubData[0].isHD = NX_METADATA_PROGRAMMES_IsHD(item);
				if ((pstContents->pstInfoHubData[0].isHD == FALSE) &&(pstTempSvc != NULL) && (pstTempSvc->highDefinition == eDxVIDEO_TYPE_HD))
				{
					pstContents->pstInfoHubData[0].isHD = TRUE;
				}

				// rating
				pstContents->pstInfoHubData[0].rating = NX_METADATA_PROGRAMMES_GetParentalRating(item);

				// cas type
				if (pstTempSvc)
				{
					pstContents->pstInfoHubData[0].casId = pstTempSvc->casType;
				}

				// teletext
				pstContents->pstInfoHubData[0].hasTeletext = FALSE;
				if ((pstTempSvc != NULL) && (pstTempSvc->ttxPid != NX_CORE_PID_NULL) && (pstTempSvc->ttxPid != 0))
				{
					pstContents->pstInfoHubData[0].hasTeletext = TRUE;
				}

				// is subtitle
				pstContents->pstInfoHubData[0].subtitleType = NX_METADATA_PROGRAMMES_GetSubtitleTypes(item);
				// audio type
				pstContents->pstInfoHubData[0].audioType = NX_METADATA_PROGRAMMES_GetAudioType(item);
				// start Time
				pstContents->pstInfoHubData[0].startTime = NX_METADATA_PROGRAMMES_GetStartTime(item);
				// duration
				pstContents->pstInfoHubData[0].duration = NX_METADATA_PROGRAMMES_GetDuration(item);

				NX_SCHEDULER_GetScheduleListByProgrammeInfo(item, pstScheduleList, &lScheduleNum);
				NX_APP_Info("- [i] Schedule Num:(%d) \n", i, lScheduleNum);
				pstContents->pstInfoHubData[0].eRsvType = DxRSVTYPE_NOTHING;

				for (lScheduleIdx = 0; lScheduleIdx < lScheduleNum; lScheduleIdx++)
				{
					pstScheduleItem = (DxSchedule_t*)HLIB_VECTOR_ItemAt(pstScheduleList, lScheduleIdx);
					if (pstScheduleItem)
					{
						pstContents->pstInfoHubData[0].eRsvType = pstScheduleItem->eRsvType;
						NX_APP_Info("- [i] pstScheduleItem->ulSlot(0x%x) \n", i, pstScheduleItem->ulSlot);
						break;
					}
				}
				NX_APP_Info("pstContents->pstInfoHubData[0].eRsvType : (0x%x)\n", pstContents->pstInfoHubData[0].eRsvType);
				if (pstScheduleList)
				{
					HLIB_VECTOR_Delete(pstScheduleList);
					pstScheduleList = NULL;
				}

				NX_APP_Info("Info - Event :  isHD:(%d), rating:(%u), casId:(0x%x), TTX:(%d)  \n", pstContents->pstInfoHubData[0].isHD, pstContents->pstInfoHubData[0].rating,
								pstContents->pstInfoHubData[0].casId, pstContents->pstInfoHubData[0].hasTeletext);

				break;
			}
		}
		HLIB_VECTOR_Delete(result);		// new list 만 날리고, core epg 의 data 는 날리면 안됨.
	}
	else
	{
		// is HD
		if ((pstTempSvc != NULL) && (pstTempSvc->highDefinition == eDxVIDEO_TYPE_HD))
		{
			pstContents->pstInfoHubData[0].isHD = TRUE;
		}

		// cas type
		if (pstTempSvc != NULL)
		{
			pstContents->pstInfoHubData[0].casId = pstTempSvc->casType;
		}

		// teletext
		pstContents->pstInfoHubData[0].hasTeletext = FALSE;
		if ((pstTempSvc != NULL) && (pstTempSvc->ttxPid != NX_CORE_PID_NULL) && (pstTempSvc->ttxPid != 0))
		{
			pstContents->pstInfoHubData[0].hasTeletext = TRUE;
		}
		if (pstContents->pstInfoHubData[0].hasTeletext == FALSE)
		{
			hError = NX_MEDIA_PLAY_GetComponentNum(ulViewId, eOxMP_COMPONENT_TELETEXT, &ulCompMaxNum);
			if ((hError == ERR_OK) && (ulCompMaxNum > 0))
			{
				pstContents->pstInfoHubData[0].hasTeletext = TRUE;
			}
		}

		// Subtitle
		hError = NX_MEDIA_PLAY_GetComponentNum(ulViewId, eOxMP_COMPONENT_SUBTITLE, &ulCompMaxNum);
		if ((hError == ERR_OK) && (ulCompMaxNum > 0))
		{
			pstContents->pstInfoHubData[0].subtitleType = eNxMediaPlay_Subttl_None;
		}

		NX_APP_Info("Info No Event : isHD:(%d), rating:(%u), casId:(0x%x), TTX:(%d)  \n", pstContents->pstInfoHubData[0].isHD, pstContents->pstInfoHubData[0].rating,
						pstContents->pstInfoHubData[0].casId, pstContents->pstInfoHubData[0].hasTeletext);
	}

	ONDK_GWM_APP_InvalidateObject(NX_INFOHUB_ICON_LIST_ID);

	return ERR_OK;
}


static ONDK_Result_t	nx_infohub_Destroy(NxUiInfoHubContents_t *pstContents)
{
	ONDK_GWM_KillTimer(NX_PRISM_INFOHUB_PROGRAMME_TIMER_ID);
	ONDK_GWM_KillTimer(NX_PRISM_INFOHUB_EXIT_TIMER_ID);
	ONDK_GWM_KillTimer(NX_PRISM_INFOHUB_UPDATE_TIMER_ID);

	//ONDK_GWM_Obj_Destroy(NX_PRISM_INFOHUB_ID);
	if (pstContents->pstInfoHubData)
	{
		NX_APP_Free(pstContents->pstInfoHubData);
		pstContents->pstInfoHubData = NULL;
	}
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_ReCreate(NxUiInfoHubContents_t *pstContents)
{
	HERROR		hError = ERR_FAIL;
	HINT32		lChannelIndex = 0, lChannelListNum = 0;
	HUINT32 	unixtime = 0;
	HCHAR 		value[64] = {'\0',};
	NX_ChListGroup_t	ePrevChangedGroup = eNxChListGroup_ALL;
	NX_QUERY_t	endQuery, startQuery, andQuery;

	nx_infohub_Destroy(pstContents);

	hError = NX_PRISM_LIVE_GetCurChannelIndex(&lChannelIndex);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetCurChannelIndex() Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	hError = NX_PRISM_LIVE_GetChannelListNum(&lChannelListNum);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetCurChannelIndex() Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(NxUiInfoHubContents_t));

	pstContents->nCurChannelIndex = lChannelIndex;
	pstContents->nChannelNum = lChannelListNum;
	pstContents->pstChannels = NX_PRISM_LIVE_GetLiveChannels();
	ePrevChangedGroup = pstContents->eChangedGroup;
	pstContents->eChangedGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
	pstContents->nProgrammeIndex = NX_PRISM_INFOHUB_FIRST_INDEX;
	pstContents->pushKey = eNxHubKey_NONE;
	pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! channel data is NULL!!!\n");
		return ONDK_RET_FAIL;
	}

	nx_infohub_SetInfoHubData(pstContents, FALSE);
	if (ePrevChangedGroup != pstContents->eChangedGroup)
	{
		ONDK_GWM_Obj_Destroy(NX_INFOHUB_DETAIL_OPTION_TEXT_ID);
		nx_infohub_SetUiCreateOptionMenu(pstContents);
	}

	hError = NX_SYSTEM_GetTime(&unixtime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetTime() time is Invalid!!!\n");
		return ONDK_RET_FAIL;
	}

	HxSTD_sprintf(value, "%u", unixtime);
	endQuery = NX_METADATA_SEARCH_CreateQuery("endTime", ">", value);

	unixtime += SECONDS_PER_HOUR * 12; // plus 12 hour
	pstContents->lastSearchStartTime = unixtime;
	HxSTD_sprintf(value, "%u", unixtime);
	startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);
	andQuery = NX_PORT_META_QUERY_AND(endQuery, startQuery);

	NX_PORT_META_QUERY_DeleteQuery(endQuery);
	NX_PORT_META_QUERY_DeleteQuery(startQuery);

	nx_infohub_SearchEpg(pstContents, andQuery);	// andQuery 는 안에서 지움.

	ONDK_GWM_SetTimer(NX_PRISM_INFOHUB_UPDATE_TIMER_ID, NX_PRISM_INFOHUB_UPDATE_TIME);

	nx_infohub_ResetExitInfoHubTimer(pstContents);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_MsgGwmCreate(NxUiInfoHubContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	unixtime = 0;
	HCHAR 		value[64] = {'\0',};
	NX_QUERY_t	endQuery, startQuery, andQuery;


	HxSTD_MemSet(pstContents, 0, sizeof(NxUiInfoHubContents_t));

	pstContents->nCurChannelIndex = p1;
	pstContents->nChannelNum = p2;
	pstContents->pstChannels = NX_PRISM_LIVE_GetLiveChannels();
	pstContents->eChangedGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
	pstContents->nProgrammeIndex = NX_PRISM_INFOHUB_FIRST_INDEX;
	pstContents->pushKey = eNxHubKey_NONE;
	pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;

	if (pstContents->pstChannels == NULL)
	{
		HxLOG_Error("channel data is NULL!!!\n");
		return ONDK_RET_FAIL;
	}
	nx_infohub_SetInfoHubData(pstContents, FALSE);
	if (nx_infohub_SetUiObject(pstContents, hAct, p1, p2, p3) == ERR_OK)
	{
		if (NX_SYSTEM_GetTime(&unixtime) != ERR_OK)
		{
			HxLOG_Error("time is Invalid!!!\n");
			return ONDK_RET_FAIL;
		}

		HxSTD_sprintf(value, "%u", unixtime);
		endQuery = NX_METADATA_SEARCH_CreateQuery("endTime", ">", value);

		unixtime += SECONDS_PER_HOUR * 12; // plus 12 hour
		pstContents->lastSearchStartTime = unixtime;
		HxSTD_sprintf(value, "%u", unixtime);
		startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);
		andQuery = NX_PORT_META_QUERY_AND(endQuery, startQuery);

		NX_PORT_META_QUERY_DeleteQuery(endQuery);
		NX_PORT_META_QUERY_DeleteQuery(startQuery);

		nx_infohub_SearchEpg(pstContents, andQuery);	// andQuery 는 안에서 지움.
	}

	ONDK_GWM_SetTimer(NX_PRISM_INFOHUB_UPDATE_TIMER_ID, NX_PRISM_INFOHUB_UPDATE_TIME);

	nx_infohub_ResetExitInfoHubTimer(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_MsgGwmDestroy(NxUiInfoHubContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_INFOHUB_PROGRAMME_TIMER_ID);
	ONDK_GWM_KillTimer(NX_PRISM_INFOHUB_EXIT_TIMER_ID);
	ONDK_GWM_KillTimer(NX_PRISM_INFOHUB_UPDATE_TIMER_ID);

	nx_infohub_DestroyPinCodeDialog();

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	//ONDK_GWM_Obj_Destroy(NX_PRISM_INFOHUB_ID);
	if (pstContents->pstInfoHubData)
		NX_APP_Free(pstContents->pstInfoHubData);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_MsgGwmKeyDown(NxUiInfoHubContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	ret = ONDK_RET_FAIL;
	NX_Programme_t	*stNxPrograme = NULL;
	NX_Channel_t	*stSelectChannel = NULL;

	nx_infohub_ResetExitInfoHubTimer(pstContents);
	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
	switch (p1)
	{
		case KEY_CH_PLUS:
		case KEY_CH_MINUS:
			return ONDK_MESSAGE_PASS;

		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			ONDK_GWM_APP_Destroy(0);
			// message pass 를 통해, live proc 에서 channel list 를 실행하도록 함.
			return ONDK_MESSAGE_PASS;

		case KEY_ARROWLEFT:
			nx_infohub_LeftKeyDown(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			nx_infohub_RightKeyDown(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_OK:
			stNxPrograme = nx_infohub_GetCurrentProgramme(pstContents);
			if (stNxPrograme == NULL)
			{
				NX_PRISM_NotAvailable_Show();
				return ONDK_MESSAGE_BREAK;
			}
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "Nx_PRISM_ProgramDetail_Proc", APP_DEFAULT_POPUP_PRIORITY, Nx_PRISM_ProgramDetail_Proc, 0, stNxPrograme->svcuid, stNxPrograme->event->eventId, eNX_UCT_Infohub);
			return ONDK_MESSAGE_BREAK;

		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_BACK:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_OPT:
			ONDK_GWM_KillTimer(NX_PRISM_INFOHUB_EXIT_TIMER_ID);
			if (pstContents->pstChannels)
			{
				stSelectChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
				if (stSelectChannel)
				{
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_CHANNELLIST_OPT_Proc", APP_DEFAULT_PRIORITY,
								NX_PRISM_CHANNELLIST_OPT_Proc, 0, eNX_UCT_Infohub, stSelectChannel->svcUid, 0);
				}
				else
				{
					NX_APP_Error("Error!!! Current Channel list Data(%u) was NULL!!!\n", pstContents->nCurChannelIndex);
				}
			}
			else
			{
				NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!!\n");
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			ret = ONDK_MESSAGE_PASS;
			break;
	}

	return ret;
}


static ONDK_Result_t	nx_infohub_MsgGwmTimer(NxUiInfoHubContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_PRISM_INFOHUB_PROGRAMME_TIMER_ID)
	{
		// set programme info
		ONDK_GWM_KillTimer(NX_PRISM_INFOHUB_PROGRAMME_TIMER_ID);
		nx_infohub_SetInfoHubData(pstContents, TRUE);
		nx_infohub_UpdateObjects(pstContents, 0, 0, 0, 0);
	}
	else if (p1 == NX_PRISM_INFOHUB_EXIT_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_INFOHUB_UPDATE_TIMER_ID)
	{
		nx_infohub_UpdateTimeAndEventTime(pstContents);
	}
	else
	{
		HxLOG_Debug("\n[%s.%d] Invalid Timer ID:[0x%x] \n\n",__FUNCTION__,__LINE__, p1);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return GWM_RET_OK;
}


static ONDK_Result_t	nx_infohub_MsgPopUpOptValueChange(NxUiInfoHubContents_t *pstContents, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	eNxChlist_OptItem_t 	eCurOptItem = (eNxChlist_OptItem_t)p2;


	switch (eCurOptItem)
	{
		case eNxChlist_OptItem_AddFav:
			eResult = nx_infohub_OptionActionAddFavourite(pstContents, p3);
			break;
		case eNxChlist_OptItem_Delete:
			pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Delete;
			nx_infohub_CreatePinCodeDialog();
			break;
		case eNxChlist_OptItem_Lock:
			eResult = nx_infohub_OptionActionLockUnLock(pstContents, p3);
			break;
		case eNxChlist_OptItem_Rename:
			eResult = nx_infohub_OptionActionRename(pstContents, p3);
			break;
		case eNxChlist_OptItem_Group:
		case eNxChlist_OptItem_Sort:
		default:
			NX_APP_Info("Not Support Option Item [%d] \n", eCurOptItem);
			break;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_infohub_MsgCoreScheduleNotify(NxUiInfoHubContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			ulSlot = NX_CORE_SCHEDULE_SLOTID_NULL;
	APK_META_SCH_MsgType_e	eMsgType = eAPK_METASCH_MSG_None;


	eMsgType = (APK_META_SCH_MsgType_e)p1;
	switch(eMsgType)
	{
		case eAPK_METASCH_MSG_ScheduleAdded:
			ulSlot = (HUINT32)p2;
			break;

		case eAPK_METASCH_MSG_ScheduleRemoved:
			ulSlot = (HUINT32)p2;
			break;

		case eAPK_METASCH_MSG_ConflictScheduleAdded:
			break;

		case eAPK_METASCH_MSG_ScheduleChanged:
			ulSlot = (HUINT32)p2;
			break;

		case eAPK_METASCH_MSG_TimeUpStart:
		case eAPK_METASCH_MSG_TimeUpStop:
			ulSlot = (HUINT32)p3;
			break;

		default:
			NX_APP_Debug("Invalid Message - eMsgType:(0x%x) \n", eMsgType);
			break;
	}

	nx_infohub_SetInfoHubData(pstContents, TRUE);
	nx_infohub_UpdateObjects(pstContents, 0, 0, 0, 0);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}



#define _________Detail_info__________________________________________________
static void nx_infohub_SetVisibleMainInfoHubUI(NxUiInfoHubContents_t *pstContents, HBOOL visible)
{
	ONDK_GWM_GuiObject_t mainUiObj = NULL;

	mainUiObj = ONDK_GWM_APP_GetObject(NX_PRISM_INFOHUB_BG_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_PRISM_INFOHUB_BG_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_CHBOX_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_CHBOX_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_DATE_TEXT_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_DATE_TEXT_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_TIME_TEXT_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_TIME_TEXT_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_BASEARROW_UP_IMG_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_BASEARROW_UP_IMG_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_BASEARROW_RIGHT_IMG_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_BASEARROW_RIGHT_IMG_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_BASEARROW_DOWN_IMG_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_BASEARROW_DOWN_IMG_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_CHBOX_LCN_TEXT_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_CHBOX_LCN_TEXT_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_CHBOX_LINE_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_CHBOX_LINE_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_CHBOX_NAME_TEXT_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_CHBOX_NAME_TEXT_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_EVENTNAME_TEXT_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_EVENTNAME_TEXT_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_PROGRAMME_DURATION_TEXT_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_PROGRAMME_DURATION_END_TEXT_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_PROGRAMME_EVENT_PROGRESS_BAR_ID, visible);

	mainUiObj = ONDK_GWM_APP_GetObject(NX_INFOHUB_DETAIL_OPTION_TEXT_ID);
	if (mainUiObj)	ONDK_GWM_Obj_SetVisible(NX_INFOHUB_DETAIL_OPTION_TEXT_ID, visible);

	ONDK_GWM_APP_InvalidateRect(NX_INFOHUB_FRAME_X, NX_INFOHUB_FRAME_Y, NX_INFOHUB_FRAME_W, NX_INFOHUB_FRAME_H);
	NX_COMMON_NOT_USED_PARAM(pstContents);
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_INFOHUB_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	NX_UI_CallerType_t		eCurOptType = eNX_UCT_None;
//	NX_ChListGroup_t		eCurChannelGroup = eNxChListGroup_TV;	// Group Item
	NxUiInfoHubContents_t	*pstContents = nx_infohub_GetContents();


	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE\n");
			eResult = nx_infohub_MsgGwmCreate(pstContents, hHandle, p1, p2, p3);
			if (eResult != ONDK_RET_OK)
			{
				NX_APP_Error("Error!!! nx_infohub_MsgGwmCreate() Fail!! \n");
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		case MSG_GWM_TIMER:
			nx_infohub_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_APK_MESSAGE:
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = ONDK_MESSAGE_PASS;
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_infohub_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_infohub_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_APP_POPUP_OPT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_OPT_VALUE_CHANGED\n");
			eCurOptType = (NX_UI_CallerType_t)p1;
			if (eCurOptType == eNX_UCT_Infohub)
			{
				nx_infohub_ResetExitInfoHubTimer(pstContents);
				eResult = nx_infohub_MsgPopUpOptValueChange(pstContents, p2, p3);
			}
			break;

		case MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES:
			NX_APP_Message("MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES\n");
			{
				// EPG Changed event 가 오면, programme index 를 처음으로 변경한다.
				pstContents->nProgrammeIndex = NX_PRISM_INFOHUB_FIRST_INDEX;
				nx_infohub_RebuildingProgrammes(pstContents);

				// indexing 처리로 인해 이전 key 처리를 해야하는 경우라면..
				if (pstContents->pushKey == eNxHubKey_RIGHT_DOWN)
					nx_infohub_RightKeyDown(pstContents);
				else if (pstContents->pushKey == eNxHubKey_LEFT_DOWN)
					nx_infohub_LeftKeyDown(pstContents);
				pstContents->pushKey = eNxHubKey_NONE;

				nx_infohub_SetInfoHubData(pstContents, TRUE);
				nx_infohub_UpdateObjects(pstContents, hHandle, p1, p2, p3);
			}
			break;

		case MSG_CORE_SCHEDULE_NOTIFY:
			NX_APP_Message("MSG_CORE_SCHEDULE_NOTIFY\n");
			eResult = nx_infohub_MsgCoreScheduleNotify(pstContents, p1, p2, p3);
			break;
		// TODO: [JHLEE] Live Streaming Start End Msg 처리 추가

		case MSG_APP_SYSTEM_SVC_CHANGED:
			NX_APP_Message("MSG_APP_SYSTEM_SVC_CHANGED\n");
			//ONDK_GWM_APP_Destroy(0);
			nx_infohub_ReCreate(pstContents);
			break;

		case MSG_APP_SYSTEM_RELOAD_SERVICELIST:
			NX_APP_Message("MSG_APP_SYSTEM_UPDATE_SERVICE\n");
			nx_infohub_ReCreate(pstContents);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			NX_APP_Message("MSG_APP_SYSTEM_PIN_OK\n");
			eResult = ONDK_RET_OK;
			if (pstContents->ePinCodeActiontItem == eNxChlist_OptItem_Delete)
			{
				pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;
				nx_infohub_DestroyPinCodeDialog();
				nx_infohub_OptionActionDelete(pstContents, p3);
				eResult = ONDK_MESSAGE_BREAK;
			}
			else if (pstContents->ePinCodeActiontItem == eNxChlist_OptItem_Lock)
			{
				pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;
				nx_infohub_DestroyPinCodeDialog();
				nx_infohub_OptionActionUnLock(pstContents);
				eResult = ONDK_MESSAGE_BREAK;
			}
			else
			{
				NX_APP_Info("UnKnow PIN Process ePinCodeActiontItem(%d)\n", pstContents->ePinCodeActiontItem);
			}
			break;

		case MSG_APP_SYSTEM_PIN_DENIED:
			NX_APP_Message("MSG_APP_SYSTEM_PIN_DENIED\n");
			nx_infohub_DestroyPinCodeDialog();
			pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;
			eResult = ONDK_MESSAGE_BREAK;
			break;

		case MSG_APP_KEYBOARD_RESULT:
			NX_APP_Message("MSG_APP_KEYBOARD_RESULT\n");
			nx_infohub_ResetExitInfoHubTimer(pstContents);
			nx_infohub_OptionActionSaveRename(pstContents);
			eResult = ONDK_MESSAGE_BREAK;
			break;

		case MSG_CORE_SCHEDULE_UPDATE:
			NX_APP_Message("MSG_CORE_SCHEDULE_UPDATE\n");
			nx_infohub_SetInfoHubData(pstContents, TRUE);
			nx_infohub_UpdateObjects(pstContents, 0, 0, 0, 0);
			break;
		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Live)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;
		default:
			break;
	}

	if (eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}


HERROR		NX_PRISM_INFOHUB_Close(void)
{
	ONDK_GWM_App_t		pGwmApp = NULL;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_INFOHUB_Proc);
	if (pGwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(NX_PRISM_INFOHUB_Proc);
	}

	return	ERR_OK;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

