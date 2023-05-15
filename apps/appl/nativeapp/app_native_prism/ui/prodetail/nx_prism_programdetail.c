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
 * @file	  		nx_prism_programdetail.c
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
#include <nx_common.h>
#include <nx_core.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/
#define	NX_PRISM_UI_FRAME_PRODETAIL_ID					eMenuFrame_Window_ProDetail


#define	NX_PRODETAIL_SERIES_LIST_LS						55
#define	NX_PRODETAIL_SERIES_LIST_ITEM_MAX				11

#define	NX_PRODETAIL_SERIES_LIST_ID						(NX_PRISM_UI_FRAME_PRODETAIL_ID + 1)
#define	NX_PRODETAIL_SERIES_LIST_X						0
#define	NX_PRODETAIL_SERIES_LIST_Y						35
#define	NX_PRODETAIL_SERIES_LIST_W						205
#define	NX_PRODETAIL_SERIES_LIST_H						(NX_PRODETAIL_SERIES_LIST_LS * NX_PRODETAIL_SERIES_LIST_ITEM_MAX)

#define	NX_PRODETAIL_SERIES_LIST_CURSOR_MG				5
#define	NX_PRODETAIL_SERIES_LIST_IMG_OFFSET_X			10
#define	NX_PRODETAIL_SERIES_LIST_TEXT_OFFSET_X			60
#define	NX_PRODETAIL_SERIES_LIST_TEXT_OFFSET_Y			(32 - NX_PRISM_FONT_SIZE_22)

#define	NX_PRODETAIL_SINGLE_EVETNT_TEXT_ID				(NX_PRISM_UI_FRAME_PRODETAIL_ID + 2)
#define	NX_PRODETAIL_SINGLE_EVETNT_TEXT_X				60
#define	NX_PRODETAIL_SINGLE_EVETNT_TEXT_Y				(68 - NX_PRISM_FONT_SIZE_36)
#define	NX_PRODETAIL_SINGLE_EVETNT_TEXT_W				550
#define	NX_PRODETAIL_SINGLE_EVETNT_TEXT_H				(42 *2)

#define	NX_PRODETAIL_SINGLE_SERVICE_TEXT_ID				(NX_PRISM_UI_FRAME_PRODETAIL_ID + 3)
#define	NX_PRODETAIL_SINGLE_SERVICE_TEXT_X				60
#define	NX_PRODETAIL_SINGLE_SERVICE_TEXT_Y				(146 - NX_PRISM_FONT_SIZE_24)
#define	NX_PRODETAIL_SINGLE_SERVICE_TEXT_W				550
#define	NX_PRODETAIL_SINGLE_SERVICE_TEXT_H				(NX_PRISM_FONT_SIZE_24 + NX_PRISM_FONT_VSPACE)

#define	NX_PRODETAIL_SINGLE_DURATION_TEXT_ID			(NX_PRISM_UI_FRAME_PRODETAIL_ID + 4)
#define	NX_PRODETAIL_SINGLE_DURATION_TEXT_X				60
#define	NX_PRODETAIL_SINGLE_DURATION_TEXT_Y				(NX_PRODETAIL_SINGLE_SERVICE_TEXT_Y + NX_PRODETAIL_SINGLE_SERVICE_TEXT_H + 8)
#define	NX_PRODETAIL_SINGLE_DURATION_TEXT_W				550
#define	NX_PRODETAIL_SINGLE_DURATION_TEXT_H				(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)

#define	NX_PRODETAIL_SINGLE_ICON_ID						(NX_PRISM_UI_FRAME_PRODETAIL_ID + 5)
#define	NX_PRODETAIL_SINGLE_ICON_X						60
#define	NX_PRODETAIL_SINGLE_ICON_Y						192// 205
#define	NX_PRODETAIL_SINGLE_ICON_W						550
#define	NX_PRODETAIL_SINGLE_ICON_H						30
#define	NX_PRODETAIL_SINGLE_ICON_MG						3

#define	NX_PRODETAIL_SINGLE_ICON_01_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 6)
#define	NX_PRODETAIL_SINGLE_ICON_02_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 7)
#define	NX_PRODETAIL_SINGLE_ICON_03_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 8)
#define	NX_PRODETAIL_SINGLE_ICON_04_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 9)
#define	NX_PRODETAIL_SINGLE_ICON_05_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 10)
#define	NX_PRODETAIL_SINGLE_ICON_06_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 11)
#define	NX_PRODETAIL_SINGLE_ICON_07_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 12)
#define	NX_PRODETAIL_SINGLE_ICON_08_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 13)
#define	NX_PRODETAIL_SINGLE_ICON_09_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 14)
#define	NX_PRODETAIL_SINGLE_ICON_10_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 15)

#define	NX_PRODETAIL_SINGLE_ICON_RECORDED_W				36
#define	NX_PRODETAIL_SINGLE_ICON_HD_W					36
#define	NX_PRODETAIL_SINGLE_ICON_RATE_W					35
#define	NX_PRODETAIL_SINGLE_ICON_CAS_W					36
#define	NX_PRODETAIL_SINGLE_ICON_LOCK_W					22
#define	NX_PRODETAIL_SINGLE_ICON_DRM_W					57
#define	NX_PRODETAIL_SINGLE_ICON_MULTI_AUDIO_W			33
#define	NX_PRODETAIL_SINGLE_ICON_SUBTITLE_W				28
#define	NX_PRODETAIL_SINGLE_ICON_TTX_W					28
#define	NX_PRODETAIL_SINGLE_ICON_RESERVATION_W			28
#define	NX_PRODETAIL_SINGLE_ICON_ONDEMAND_W				36

#define	NX_PRODETAIL_SINGLE_GENRE_TEXT_ID				(NX_PRISM_UI_FRAME_PRODETAIL_ID + 16)
#define	NX_PRODETAIL_SINGLE_GENRE_TEXT_X				60
#define	NX_PRODETAIL_SINGLE_GENRE_TEXT_Y				(248 - NX_PRISM_FONT_SIZE_20)
#define	NX_PRODETAIL_SINGLE_GENRE_TEXT_W				550
#define	NX_PRODETAIL_SINGLE_GENRE_TEXT_H				(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)

#define	NX_PRODETAIL_SINGLE_DESC_BOX_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 17)
#define	NX_PRODETAIL_SINGLE_DESC_BOX_X					60
#define	NX_PRODETAIL_SINGLE_DESC_BOX_Y					261
#define	NX_PRODETAIL_SINGLE_DESC_BOX_W					550
#define	NX_PRODETAIL_SINGLE_DESC_BOX_H					249

#define	NX_PRODETAIL_SINGLE_DESC_TEXT_ID				(NX_PRISM_UI_FRAME_PRODETAIL_ID + 18)
#define	NX_PRODETAIL_SINGLE_DESC_TEXT_LS				32
#define	NX_PRODETAIL_SINGLE_DESC_TEXT_LINE_CNT			6
#define	NX_PRODETAIL_SINGLE_DESC_TEXT_X					(NX_PRODETAIL_SINGLE_DESC_BOX_X + 15)
#define	NX_PRODETAIL_SINGLE_DESC_TEXT_Y					(NX_PRODETAIL_SINGLE_DESC_BOX_Y + 10)
#define	NX_PRODETAIL_SINGLE_DESC_TEXT_W					(NX_PRODETAIL_SINGLE_DESC_BOX_W - 30)
#define	NX_PRODETAIL_SINGLE_DESC_TEXT_H					(NX_PRODETAIL_SINGLE_DESC_TEXT_LS * NX_PRODETAIL_SINGLE_DESC_TEXT_LINE_CNT)

#define	NX_PRODETAIL_SINGLE_SCROLL_L_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 19)
#define	NX_PRODETAIL_SINGLE_SCROLL_L_X					266
#define	NX_PRODETAIL_SINGLE_SCROLL_L_Y					(NX_PRODETAIL_SINGLE_DESC_TEXT_Y + NX_PRODETAIL_SINGLE_DESC_TEXT_H)
#define	NX_PRODETAIL_SINGLE_SCROLL_L_W					26
#define	NX_PRODETAIL_SINGLE_SCROLL_L_H					30

#define	NX_PRODETAIL_SINGLE_SCROLL_PAGE_ID				(NX_PRISM_UI_FRAME_PRODETAIL_ID + 20)
#define	NX_PRODETAIL_SINGLE_SCROLL_PAGE_X				(NX_PRODETAIL_SINGLE_SCROLL_L_X + NX_PRODETAIL_SINGLE_SCROLL_L_W)
#define	NX_PRODETAIL_SINGLE_SCROLL_PAGE_Y				(NX_PRODETAIL_SINGLE_DESC_TEXT_Y + NX_PRODETAIL_SINGLE_DESC_TEXT_H)
#define	NX_PRODETAIL_SINGLE_SCROLL_PAGE_W				95
#define	NX_PRODETAIL_SINGLE_SCROLL_PAGE_H				30

#define	NX_PRODETAIL_SINGLE_SCROLL_R_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 21)
#define	NX_PRODETAIL_SINGLE_SCROLL_R_X					(NX_PRODETAIL_SINGLE_SCROLL_PAGE_X + NX_PRODETAIL_SINGLE_SCROLL_PAGE_W)
#define	NX_PRODETAIL_SINGLE_SCROLL_R_Y					(NX_PRODETAIL_SINGLE_DESC_TEXT_Y + NX_PRODETAIL_SINGLE_DESC_TEXT_H)
#define	NX_PRODETAIL_SINGLE_SCROLL_R_W					26
#define	NX_PRODETAIL_SINGLE_SCROLL_R_H					30

#define	NX_PRODETAIL_FRAME_X							0
#define	NX_PRODETAIL_FRAME_Y							0
#define	NX_PRODETAIL_FRAME_W							900
#define	NX_PRODETAIL_FRAME_H							NX_PRISM_UI_BACKGROUND_BOX_HEIGHT

#define	NX_PRODETAIL_SVC_LCN_TEXT_LEN					5
#define	NX_PRODETAIL_PAGE_NUM_MAX						6

#define NX_PRODETAIL_MAX_FMT_ITEM						500			// the maxuimum number of list items which can handle

#define	NX_PRODETAIL_SERIES_LIST_PAGE_ID				(NX_PRISM_UI_FRAME_PRODETAIL_ID + 22)
#define	NX_PRODETAIL_SERIES_LIST_PAGE_X					97
#define	NX_PRODETAIL_SERIES_LIST_PAGE_Y					(664 - NX_PRISM_FONT_SIZE_20)
#define	NX_PRODETAIL_SERIES_LIST_PAGE_W					100
#define	NX_PRODETAIL_SERIES_LIST_PAGE_H					(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)

#define	NX_PRODETAIL_SERIES_DIVIDER_ID					(NX_PRISM_UI_FRAME_PRODETAIL_ID + 23)
#define	NX_PRODETAIL_SERIES_DIVIDER_X					219
#define	NX_PRODETAIL_SERIES_DIVIDER_Y					30
#define	NX_PRODETAIL_SERIES_DIVIDER_W					1
#define	NX_PRODETAIL_SERIES_DIVIDER_H					690

#define	NX_PRISM_UI_ACTION_BTN_ID						(NX_PRISM_UI_FRAME_PRODETAIL_ID + 128) // until + eNxPRODetail_BTNItem_Max

#define	NX_PRODETAIL_ACTON_BTN_X						60
#define	NX_PRODETAIL_ACTON_BTN_Y						520
#define	NX_PRODETAIL_ACTON_BTN_W						550
#define	NX_PRODETAIL_ACTON_BTN_H						55


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxPRODetail_DispMode_Single = 0,
	eNxPRODetail_DispMode_Series,
} NxPRODetail_DispMode_t;


typedef enum {
	eNxPRODetail_BTN_Future_Series = 0,
	eNxPRODetail_BTN_Future_Single,
	eNxPRODetail_BTN_OnAir_Series,
	eNxPRODetail_BTN_OnAir_Single,
	eNxPRODetail_BTN_Resored_Series,
	eNxPRODetail_BTN_Resored_Single,
	eNxPRODetail_BTN_NoneRecorded,
} NxPRODetail_Button_t;


typedef enum {
	eNxPRODetail_BTNItem_Record = 0,
	eNxPRODetail_BTNItem_SetReminder,
	eNxPRODetail_BTNItem_CancelRecord,
	eNxPRODetail_BTNItem_CancelReminder,
	eNxPRODetail_BTNItem_WatchLiveTv,
	eNxPRODetail_BTNItem_PlayRecording,
	eNxPRODetail_BTNItem_StopRecording,
	eNxPRODetail_BTNItem_PlayThisEpisode,
	eNxPRODetail_BTNItem_DeleteThisEpisode,
	eNxPRODetail_BTNItem_PlayThisProgramme,
	eNxPRODetail_BTNItem_DeleteThisProgramme,
	eNxPRODetail_BTNItem_Max
} NxPRODetail_BTNItem_t;


typedef enum
{
	eNxPRODetail_Attr_All				= 0x00000000,

	eNxPRODetail_Attr_AD				= 0x00000001,
	eNxPRODetail_Attr_Dolby				= 0x00000002,
	eNxPRODetail_Attr_Multiaudio		= 0x00000004,
	eNxPRODetail_Attr_Multifeed			= 0x00000008,
	eNxPRODetail_Attr_DolbyPlus 		= 0x00000010,

	eNxPRODetail_Attr_HD				= 0x00000020,
	eNxPRODetail_Attr_Teletext			= 0x00000040,
	eNxPRODetail_Attr_Subtitle			= 0x00000080,
	eNxPRODetail_Attr_Lock				= 0x00000100,

	eNxPRODetail_Attr_Compressed		= 0x00000200,
	eNxPRODetail_Attr_Evt_SD_Linkage	= 0x00000400,
	eNxPRODetail_Attr_Evt_HD_Linkage	= 0x00000800,

	eNxPRODetail_Attr_Rsv_Watch			= 0x00001000,
	eNxPRODetail_Attr_Rsv_Record		= 0x00002000,
	eNxPRODetail_Attr_Record			= 0x00004000,

	eNxPRODetail_Attr_OnDemand			= 0x00008000,
	eNxPRODetail_Attr_DRM				= 0x00010000,
	eNxPRODetail_Attr_Series			= 0x00020000,

	eNxPRODetail_Attr_Max
} NxPRODetail_Attr_t;


typedef enum {
	eNxSeries_None = 0,
	eNxSeries_Record,
	eNxSeries_Schedule,
	eNxSeries_Event,
} NxSeriesItem_t;


typedef struct
{
	NxSeriesItem_t	eSeriesType;
	HCHAR			szItemTitle[NX_TEXT_SIZE_16];

	// Schecule
	HUINT32			ulSlot;
	DxRsvType_e		eRsvType;
	DxRsvRepeat_e	eRepeat;

#if defined(CONFIG_MW_MM_PVR)
	// Recording
	HUINT32			ulContentId;
	DxRecStatus_e	eResStatus;
#endif

	// Event
	HINT32			lEventId;

} NX_UiSeriseItem_t;


typedef struct tag_NX_PRODETAIL_SET_CONTENT
{
	HUINT32			ulSvcUid;
	HINT32			lEventId;

	DxService_t		*stSvcInfo;
	HINT32			ulChannelNumber;
	HCHAR			szSvcInfo[DxNAME_LEN + NX_PRODETAIL_SVC_LCN_TEXT_LEN];
	HCHAR			szEventName[EPG_HUMAX_NAME_LEN];
	HCHAR			szDuration[NX_TEXT_SIZE_64];
	HCHAR			szPageScroll[NX_TEXT_SIZE_8];
	HCHAR			szGenres[NX_TEXT_SIZE_256];
	HCHAR 			szLongDes[EPG_HUMAX_DETAIL_LEN];

	HCHAR			szSeriesPage[NX_TEXT_SIZE_8];

	HINT32			lCurPage;
	HINT32			lMaxPage;

	HINT32			lRating;
	HINT32			lCasId;
	HINT32			lSubtitleType;
	HUINT32			ulStartTime;
	HUINT32			ulDuration;
	HINT32			lSeriesRecordingNum;
	HINT32			lSeriesScheduleNum;
	HINT32			lSeriesEventNum;

	HUINT32			ulEventAttribute;

	HINT32					lSeriesItemNum;
	HINT32					lSeriesCurIndex;
	NX_UiSeriseItem_t		*stSeritemsList;

	HUINT32 		ulSchSlot;
	HUINT32			ulAfterScheduleAction;

	NX_UI_CallerType_t		eCallerUiType;

	NxPRODetail_DispMode_t	eDispMode;
	NxPRODetail_Button_t	eBtnMode;

	HINT32			lPrevFocusObjId;
} NxUiProDetailContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxUiProDetailContent_t			s_stProDetailContent;
static NXUI_DialogInstance_t			s_stProDetailDialogMessageInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_programDetail_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_programDetail_CreateResponseDialouge(HUINT8* pszString);

static NXUI_DialogInstance_t	*nx_programDetail_GetDialogPopupContents(void);
static ONDK_Result_t	nx_programDetail_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_programDetail_ProcessingMsgDestroy(void);
static void				nx_programDetail_ProcessingMsgCreate(void);

static inline NxUiProDetailContent_t* nx_programDetail_GetContents(void);

#if defined(CONFIG_MW_MM_PVR)
static ONDK_Result_t	nx_programDetail_MakeDetailInfoRecordingList(NxUiProDetailContent_t *pstContents, NX_Programme_t *pProgItem);
#endif

static HERROR			nx_programDetail_CheckScheduleLimite(void);

static HINT32			nx_programDetail_ConvertObjIdToActionItemIdx(HINT32 lObjId);
static HINT32			nx_programDetail_ConvertActionItemIdxToObjId(HINT32 lItemId);

static ONDK_Result_t	nx_programDetail_ActionButtonByWatchLiveTv(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButtonByRecord(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButtonByCancelRecord(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButtonByPlayRecording(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButtonByStopRecording(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButtonByPlayThisEpisode(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButtonByDeleteThisEpisode(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButtonByPlayThisProgramme(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButtonByDeleteThisProgramme(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButtonBySetReminder(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButtonByCancelReminder(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_ActionButton(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_programDetail_ReloadButtonMode(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_MakeActionButtonMode(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_MakeDetailInfoScheduleList(NxUiProDetailContent_t *pstContents, NX_Programme_t *pProgItem);
static ONDK_Result_t	nx_programDetail_MakeDetailInfoProgramList(NxUiProDetailContent_t *pstContents, NX_Programme_t *pProgItem);
static ONDK_Result_t	nx_programDetail_MakeDetailInfoByEvent(NxUiProDetailContent_t *pstContents, HBOOL bMakeSeries);
static ONDK_Result_t	nx_programDetail_MakeFMTData(NxUiProDetailContent_t *pstContents);

static ONDK_Result_t	nx_programDetail_DrawList_Cursor_A2(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
										ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_programDetail_DrawSeriesList(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_DrawFMTItem(HINT32 ID, ONDKSurfaceHandle screen, FMT_DrawItem_t *drawItem);
static ONDK_Result_t	nx_programDetail_DrawFMTScroll(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect, FMT_OBJECT Object);
static ONDK_Result_t	nx_programDetail_DrawIconList(NxUiProDetailContent_t *pstContents);

static void				nx_programDetail_DeleteAllActionItem(void);
static ONDK_Result_t	nx_programDetail_AddActionItem_FutureSeries(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_AddActionItem_FutureSingle(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_AddActionItem_OnAirSeries(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_AddActionItem_OnAirSingle(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_AddActionItem_RecordedSeries(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_AddActionItem_RecordedSingle(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_SetActionBtnUiObject(NxUiProDetailContent_t *pstContents);
static ONDK_Result_t	nx_programDetail_SetUiObject(NxUiProDetailContent_t *pstContents);

static ONDK_Result_t	nx_programDetail_Initialize(NxUiProDetailContent_t *pstContents, HUINT32 p1, HINT32 p2, HUINT32 lCallerUiType);

static ONDK_Result_t	nx_programDetail_MsgGwmCreate(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_programDetail_MsgGwmDestroy(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_programDetail_MsgGwmHapiMsg(NxUiProDetailContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_programDetail_MsgGwmKeyDown(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_programDetail_MsgGwmClick(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_programDetail_MsgGwmListChange(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_programDetail_MsgGwmListSelected(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_programDetail_MsgCoreScheduleNotify(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Response_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_programDetail_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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


static void		nx_programDetail_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_programDetail_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_programDetail_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


/******************************************************************************/
#define __Process_Dialog_Functions_____________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_programDetail_GetDialogPopupContents(void)
{
	return &s_stProDetailDialogMessageInstance;
}


static ONDK_Result_t	nx_programDetail_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_programDetail_GetDialogPopupContents();

	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(pstDialog, message, hAction, p1, p2, p3);
}


static void		nx_programDetail_ProcessingMsgDestroy(void)
{
	ONDK_GWM_APP_Destroy(nx_programDetail_ProcessingMsgProc);
}


static void		nx_programDetail_ProcessingMsgCreate(void)
{
	HINT8			*pszMsg = NULL;
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_programDetail_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);
	pszMsg = RES_PLEASE_WAIT_ID;

	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)(HINT32)ONDK_GetString(pszMsg));

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_programDetail_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_programDetail_ProcessingMsgProc, 0, 0, 0, 0);
}


/******************************************************************************/
#define __Local_Functions_____________________________________________________
/******************************************************************************/

static inline NxUiProDetailContent_t* nx_programDetail_GetContents(void)
{
	return	&s_stProDetailContent;
}


static HERROR	nx_programDetail_CheckScheduleLimite(void)
{
	HBOOL		bZepperOnly = FALSE;
	HINT8		*pszStrId = NULL;
	HUINT32		ulNumSch = 0;


	NX_SCHEDULER_CountSchedule(&ulNumSch);
	if (ulNumSch < NX_CORE_SCHEDULE_SUPPORTED_SCHEDULE_NUM)
	{
		return	ERR_OK;
	}

	NX_APP_Info("NX_SCHEDULER_CountSchedule() - (%d) \n", ulNumSch);

	bZepperOnly = NX_PRISM_SPEC_GetUiSupportZapperOnly();
	if (bZepperOnly == TRUE)
	{
		pszStrId = RES_MESG_5979_ID;
	}
	else
	{
#if defined(CONFIG_MW_MM_PVR)
		pszStrId = RES_MESG_4543_ID;
#endif
	}

	if (pszStrId)
	{
		nx_programDetail_CreateResponseDialouge(ONDK_GetString(pszStrId));
	}

	return	ERR_FAIL;
}


#if defined(CONFIG_MW_MM_PVR)
static ONDK_Result_t	nx_programDetail_MakeDetailInfoRecordingList(NxUiProDetailContent_t *pstContents, NX_Programme_t *pProgItem)
{
	HINT32			i = 0, lRecNum = 0, lTotal = 0;
	HUINT32			ulSeriesId = 0;
	DxRecListData_t	*stRecordItem = NULL;
	HxDATETIME_Date_t stDate;
	HxVector_t		*pstRecordList = HLIB_VECTOR_New(NULL, NULL);

	ulSeriesId = NX_METADATA_PROGRAMMES_GetSeriesID(pProgItem);
	if (ulSeriesId == 0)
	{
		NX_APP_Info("None Series item!!\n");
		return	ONDK_RET_OK;
	}

	lTotal = (pstContents->lSeriesScheduleNum + pstContents->lSeriesRecordingNum + pstContents->lSeriesEventNum);

	// TODO: [JHLEE] Add Find Recrod list
	for (i = 0; i < lRecNum; i++)
	{
		stRecordItem = (DxRecListData_t*)HLIB_VECTOR_ItemAt(pstRecordList, i);
		if ((pstContents->stSeritemsList) && (pstContents->lSeriesItemNum < lTotal))
		{
			pstContents->stSeritemsList[pstContents->lSeriesItemNum].eSeriesType = eNxSeries_Schedule;
			pstContents->stSeritemsList[pstContents->lSeriesItemNum].ulContentId = stRecordItem->ulContentId;
			pstContents->stSeritemsList[pstContents->lSeriesItemNum].eResStatus = stRecordItem->eRecStatus;

			if ((stRecordItem->stEvt.ulSeason > 0) && (stRecordItem->stEvt.ulEpisodeNum > 0))
			{
				HxSTD_sprintf(pstContents->stSeritemsList[pstContents->lSeriesItemNum].szItemTitle, "S%d E%d", (int)stRecordItem->stEvt.ulSeason, (int)stRecordItem->stEvt.ulEpisodeNum);
			}
			else if (stRecordItem->stEvt.ulSeason > 0)
			{
				HxSTD_sprintf(pstContents->stSeritemsList[pstContents->lSeriesItemNum].szItemTitle, "S%d", (int)stRecordItem->stEvt.ulSeason);
			}
			else if (stRecordItem->stEvt.ulEpisodeNum > 0)
			{
				HxSTD_sprintf(pstContents->stSeritemsList[pstContents->lSeriesItemNum].szItemTitle, "E%d", (int)stRecordItem->stEvt.ulEpisodeNum);
			}
			else
			{
				HxSTD_memset(&stDate, 0x00, sizeof(HxDATETIME_Date_t));
				HLIB_DATETIME_ConvertUnixTimeToDate(stRecordItem->ulRecStartTime, &stDate);
				HxSTD_sprintf(pstContents->stSeritemsList[pstContents->lSeriesItemNum].szItemTitle, "%02d.%02d", (int)stDate.ucDay, (int)stDate.ucMonth);
			}
		}

		stRecordItem = NULL;
	}

	HLIB_VECTOR_Delete(pstRecordList);
	pstRecordList = NULL;

	return	ONDK_RET_OK;
}

#endif


static HINT32	nx_programDetail_ConvertObjIdToActionItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~NX_PRISM_UI_ACTION_BTN_ID);

	if ((lItemId < 0) || (lItemId >= eNxPRODetail_BTNItem_Max))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_programDetail_ConvertActionItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxPRODetail_BTNItem_Max))
	{
		return 0;
	}

	return	(NX_PRISM_UI_ACTION_BTN_ID | lItemId);
}


static ONDK_Result_t	nx_programDetail_ReloadButtonMode(NxUiProDetailContent_t *pstContents)
{
	HINT32		lObjId = -1, lItemIdx = -1, lFindObjId = -1;
	ONDK_GWM_GuiObject_t	gwmGuiObject = NULL;


	if (pstContents->eCallerUiType == eNX_UCT_Schedule)
	{
		NX_APP_Error("Not Support Button mode !!\n");
		return	ONDK_RET_OK;
	}

	lObjId = GWM_GetFocusedObjectID();

	nx_programDetail_DeleteAllActionItem();
	nx_programDetail_MakeActionButtonMode(pstContents);
	nx_programDetail_SetActionBtnUiObject(pstContents);

	gwmGuiObject = ONDK_GWM_Obj_GetObject(lObjId);
	if (gwmGuiObject != NULL)
	{
		ONDK_GWM_Obj_SetFocus(lObjId);
	}
	else
	{
		lItemIdx = nx_programDetail_ConvertObjIdToActionItemIdx(lObjId);
		switch (lItemIdx)
		{
			case eNxPRODetail_BTNItem_Record:
				lFindObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_CancelRecord);
				break;
			case eNxPRODetail_BTNItem_SetReminder:
				lFindObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_CancelReminder);
				break;
			case eNxPRODetail_BTNItem_CancelRecord:
				lFindObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_Record);
				break;
			case eNxPRODetail_BTNItem_CancelReminder:
				lFindObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_SetReminder);
				break;
			case eNxPRODetail_BTNItem_PlayRecording:
				lFindObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_StopRecording);
				break;
			case eNxPRODetail_BTNItem_StopRecording:
				lFindObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_PlayRecording);
				break;
			default:
				lFindObjId = -1;
				break;
		}

		gwmGuiObject = ONDK_GWM_Obj_GetObject(lFindObjId);
		if (gwmGuiObject != NULL)
		{
			ONDK_GWM_Obj_SetFocus(lFindObjId);
		}
	}

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MakeActionButtonMode(NxUiProDetailContent_t *pstContents)
{
	HBOOL		bSystemTime = FALSE, bSeries = FALSE, bFuture = FALSE;
	HUINT32 	ulUnixTime = 0;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	// Check Time
	if (NX_SYSTEM_GetTime(&ulUnixTime) != ERR_OK)
	{
		NX_APP_Error("time is Invalid!!!\n");
		bSystemTime = FALSE;
	}
	else
	{
		bSystemTime = TRUE;
		bFuture = (pstContents->ulStartTime > ulUnixTime) ? TRUE : FALSE ;
	}

	// Check Episode & Season
	if (pstContents->lSeriesItemNum > 0)
	{
		bSeries = TRUE;
	}

	NX_APP_Info("- bSystemTime[%d], bFuture[%d], lSeriesRecordingNum[%d], lSeriesScheduleNum[%d], lSeriesEventNum[%d] \n",
					bSystemTime, bFuture, pstContents->lSeriesRecordingNum, pstContents->lSeriesScheduleNum, pstContents->lSeriesEventNum);

	// Set Button Mode
	if ((bSeries == TRUE) && (bSystemTime == TRUE) && (bFuture == TRUE))
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_Future_Series;
	}
	else if ((bSeries == TRUE) && (bSystemTime == TRUE) && (bFuture == FALSE))
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_OnAir_Series;
	}
	else if ((bSeries == TRUE) && (bSystemTime == FALSE) && (bFuture == TRUE))
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_OnAir_Series;
	}
	else if ((bSeries == TRUE) && (bSystemTime == FALSE) && (bFuture == FALSE))
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_OnAir_Series;
	}
	else if ((bSeries == FALSE) && (bSystemTime == TRUE) && (bFuture == TRUE))
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_Future_Single;
	}
	else if ((bSeries == FALSE) && (bSystemTime == TRUE) && (bFuture == FALSE))
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_OnAir_Single;
	}
	else if ((bSeries == FALSE) && (bSystemTime == FALSE) && (bFuture == TRUE))
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_OnAir_Single;
	}
	else if ((bSeries == FALSE) && (bSystemTime == FALSE) && (bFuture == FALSE))
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_OnAir_Single;
	}
	else
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_OnAir_Single;
	}

	if (pstContents->eCallerUiType == eNX_UCT_TSR)
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_NoneRecorded;
	}
	else if ((pstContents->eCallerUiType == eNX_UCT_Record) && (bSeries == FALSE))
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_Resored_Single;
	}
	else if ((pstContents->eCallerUiType == eNX_UCT_Record) && (bSeries == TRUE))
	{
		pstContents->eBtnMode = eNxPRODetail_BTN_Resored_Series;
	}

	NX_APP_Info("eBtnMode [%d] \n", pstContents->eBtnMode);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MakeDetailInfoScheduleList(NxUiProDetailContent_t *pstContents, NX_Programme_t *pProgItem)
{
	HINT32			i = 0, lSchNum = 0, lTotal = 0;
	HUINT32			ulSeriesId = 0;
	DxSchedule_t 	*stScheduleItem = NULL;
	HxVector_t		*pstScheduleList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);

	ulSeriesId = NX_METADATA_PROGRAMMES_GetSeriesID(pProgItem);
	if (ulSeriesId == 0)
	{
		NX_APP_Info("None Series item!!\n");
		return	ONDK_RET_OK;
	}

	lTotal = (pstContents->lSeriesScheduleNum + pstContents->lSeriesRecordingNum + pstContents->lSeriesEventNum);

	NX_SCHEDULER_GetScheduleListBySeriesIdAndServiceUid(ulSeriesId, pstContents->ulSvcUid, pstScheduleList, &lSchNum);
	for (i = 0; i < lSchNum; i++)
	{
		stScheduleItem = (DxSchedule_t*)HLIB_VECTOR_ItemAt(pstScheduleList, i);
		if ((pstContents->stSeritemsList) && (pstContents->lSeriesItemNum < lTotal))
		{
			pstContents->stSeritemsList[pstContents->lSeriesItemNum].eSeriesType = eNxSeries_Schedule;
			pstContents->stSeritemsList[pstContents->lSeriesItemNum].ulSlot = stScheduleItem->ulSlot;
			pstContents->stSeritemsList[pstContents->lSeriesItemNum].eRsvType = stScheduleItem->eRsvType;
			pstContents->stSeritemsList[pstContents->lSeriesItemNum].eRepeat = stScheduleItem->eRepeat;

			// TODO: Schedule Item do not has Season Number & Episode Number
			HxSTD_sprintf(pstContents->stSeritemsList[pstContents->lSeriesItemNum].szItemTitle, "%02d.%02d",
						(int)stScheduleItem->stStartTime.stDate.ucDay, (int)stScheduleItem->stStartTime.stDate.ucMonth);

			pstContents->lSeriesItemNum++;
		}

		stScheduleItem = NULL;
	}

	HLIB_VECTOR_Delete(pstScheduleList);
	pstScheduleList = NULL;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MakeDetailInfoProgramList(NxUiProDetailContent_t *pstContents, NX_Programme_t *pProgItem)
{
	HERROR			hError = ERR_FAIL;
	HINT32			i = 0, lSeriesEventNum = 0, lTotal = 0;
	HUINT32 		ulSeriesId = 0, ulCurrentTime = 0, ulSeason = 0, ulEpisode = 0;
	NX_Programme_t	*stEventItem = NULL;
	HxDATETIME_Date_t stDate;
	HxVector_t		*pstEventList = HLIB_VECTOR_New(NULL, NULL);


	ulSeriesId = NX_METADATA_PROGRAMMES_GetSeriesID(pProgItem);
	if (ulSeriesId == 0)
	{
		NX_APP_Info("None Series item ulSeriesId-(0x%x)!!\n", ulSeriesId);
		return	ONDK_RET_OK;
	}

	hError = NX_SYSTEM_GetTime(&ulCurrentTime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! Time is Invalid!!!\n");
		ulCurrentTime = 0;
	}

	lTotal = (pstContents->lSeriesScheduleNum + pstContents->lSeriesRecordingNum + pstContents->lSeriesEventNum);

	NX_METADATA_EPG_GetResultProgrammeBySeriesId(pstContents->ulSvcUid, ulSeriesId, ulCurrentTime, pstEventList, &lSeriesEventNum);
	for (i = 0; i < lSeriesEventNum; i++)
	{
		stEventItem = (NX_Programme_t*)HLIB_VECTOR_ItemAt(pstEventList, i);
		if ((pstContents->stSeritemsList) && (pstContents->lSeriesItemNum < lTotal))
		{
			pstContents->stSeritemsList[pstContents->lSeriesItemNum].eSeriesType = eNxSeries_Event;
			pstContents->stSeritemsList[pstContents->lSeriesItemNum].lEventId = stEventItem->event->eventId;
			if (stEventItem->event->type == eDxEPG_TYPE_IP)
			{
				ulSeason = stEventItem->event->extension.ipEpg.season;
				ulEpisode = stEventItem->event->extension.ipEpg.episodeNum;
			}
#if defined(CONFIG_MW_EPG_TVTV)
			if (stEventItem->event->type == eDxEPG_TYPE_TVTV)
			{
				ulSeason = stEventItem->event->extension.tvtvEpg.season;
				ulEpisode = stEventItem->event->extension.tvtvEpg.episodeNum;
			}
#endif

			if ((ulSeason > 0) && (ulEpisode > 0))
			{
				HxSTD_sprintf(pstContents->stSeritemsList[pstContents->lSeriesItemNum].szItemTitle, "S%d E%d", (int)ulEpisode, (int)ulSeason);
			}
			else if (ulSeason > 0)
			{
				HxSTD_sprintf(pstContents->stSeritemsList[pstContents->lSeriesItemNum].szItemTitle, "S%d", (int)ulSeason);
			}
			else if (ulEpisode > 0)
			{
				HxSTD_sprintf(pstContents->stSeritemsList[pstContents->lSeriesItemNum].szItemTitle, "E%d", (int)ulEpisode);
			}
			else
			{
				HxSTD_memset(&stDate, 0x00, sizeof(HxDATETIME_Date_t));
				HLIB_DATETIME_ConvertUnixTimeToDate(stEventItem->event->startTime, &stDate);
				HxSTD_sprintf(pstContents->stSeritemsList[pstContents->lSeriesItemNum].szItemTitle, "%02d.%02d", (int)stDate.ucDay, (int)stDate.ucMonth);
			}

			if (pstContents->lEventId == stEventItem->event->eventId)
			{
				pstContents->lSeriesCurIndex = pstContents->lSeriesItemNum;
			}

			pstContents->lSeriesItemNum++;
		}

		stEventItem = NULL;
	}

	if (pstEventList)
	{
		HLIB_VECTOR_Delete(pstEventList);
		pstEventList = NULL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MakeDetailInfoByEvent(NxUiProDetailContent_t *pstContents, HBOOL bMakeSeries)
{
	HBOOL			bIsTrue = FALSE;
	HERROR			hError = ERR_FAIL;
	HCHAR			szEventName[EPG_HUMAX_NAME_LEN] = {'\0',};
	HINT8			*szNibbleLevel1 = NULL, *szNibbleLevel2 = NULL;
	HUINT8			ucNibblelevel1 = 0, ucNibblelevel2 = 0;
	HINT32			i = 0, lNum = 0, lScheduleIdx = 0, lScheduleNum = 0, lAudioType = eNX_META_AUDIO_TYPE_UNKNOW;
	HINT32			lSeriesSchNum = 0, lSeriesRecNum = 0, lSeriesEventNum = 0;
	HUINT32 		ulCalcMin = 0, ulTotalMin = 0, ulCalcHour = 0, ulWeekIndex = 0;
	HUINT32			ulSeriesId = 0, ulCurrentTime = 0;
	NX_Programme_t		*stItem = NULL;
	DxSchedule_t		*pstScheduleItem = NULL;
	HxDATETIME_t		stStartTime;
	HxDATETIME_Time_t	stDurationTime;
	HxVector_t			*pstResult = HLIB_VECTOR_New(NULL, NULL);
	HxVector_t			*pstScheduleList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);


	if (pstContents == NULL)
	{
		NX_APP_Error("[%s:%d] Error!! pstContents is NULL!! \n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_INITFAIL;
	}

	if (pstContents->stSvcInfo == NULL)
	{
		NX_APP_Error("[%s:%d] Error!! pstContents->stSvcInfo is NULL!! \n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_INITFAIL;
	}

	/* Make Service Info - SVC Number + Service Name */
	HxSTD_sprintf(pstContents->szSvcInfo, "%04d %s", pstContents->ulChannelNumber, pstContents->stSvcInfo->name);

	pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_Lock);
	if (pstContents->stSvcInfo->locked)
	{
		pstContents->ulEventAttribute |= eNxPRODetail_Attr_Lock;
	}

	hError = NX_SYSTEM_GetTime(&ulCurrentTime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! Time is Invalid!!!\n");
		ulCurrentTime = 0;
	}

	if (pstContents->lEventId > 0)
	{
		hError = NX_METADATA_EPG_GetResultProgrammeBySvcUid(pstContents->ulSvcUid, pstResult, &lNum);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_METADATA_EPG_GetResultProgrammeBySvcUid() Fail !!\n");
		}

		NX_APP_Info("NX_METADATA_EPG_GetResultProgrammeBySvcUid - lNum [%d] \n", lNum);
		for (i = 0; i < lNum; i++)
		{
			stItem = (NX_Programme_t*)HLIB_VECTOR_ItemAt(pstResult, i);
			if (stItem && (stItem->event->eventId == pstContents->lEventId))		// 일단.. current time 에 대해서만..
			{
				/* Make Event Name */
				hError = NX_METADATA_PROGRAMMES_GetName(stItem, szEventName);
				if (hError == ERR_OK)
				{
					HxSTD_MemSet(pstContents->szEventName, 0, EPG_HUMAX_NAME_LEN);
					HLIB_STD_StrUtf8NCpy(pstContents->szEventName, szEventName, EPG_HUMAX_NAME_LEN);
					NX_APP_Info("- [i] Event Name [%s] \n", i, pstContents->szEventName);
				}

				/* Make Extended Event Info */
				HxSTD_MemSet(pstContents->szLongDes, 0, EPG_HUMAX_DETAIL_LEN);
				NX_METADATA_PROGRAMMES_GetLongDescription(stItem, pstContents->szLongDes);

				// Chekc HD
				pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_HD);
				bIsTrue = NX_METADATA_PROGRAMMES_IsHD(stItem);
				if (bIsTrue)
				{
					NX_APP_Info("- [i] HD \n", i);
					pstContents->ulEventAttribute |= eNxPRODetail_Attr_HD;
				}

				// Check rating
				pstContents->lRating = NX_METADATA_PROGRAMMES_GetParentalRating(stItem);
				NX_APP_Info("- [i] Rating(%d) \n", i, pstContents->lRating);

				// Check cas type
				pstContents->lCasId = pstContents->stSvcInfo->casType;
				NX_APP_Info("- [i] CAS ID (0x%x) \n", i, pstContents->lCasId);

				// Check teletext
				pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_Teletext);
				if ((pstContents->stSvcInfo->ttxPid != NX_CORE_PID_NULL) && (pstContents->stSvcInfo->ttxPid != 0))
				{
					pstContents->ulEventAttribute |= eNxPRODetail_Attr_Teletext;
					NX_APP_Info("- [i] TELETEXT \n", i);
				}

				// Check subtitle
				pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_Subtitle);
				pstContents->lSubtitleType = NX_METADATA_PROGRAMMES_GetSubtitleTypes(stItem);
				if (pstContents->lSubtitleType)
				{
					pstContents->ulEventAttribute |= eNxPRODetail_Attr_Subtitle;
					NX_APP_Info("[i] SUBTITEL(0x%x) \n", i, pstContents->lSubtitleType);
				}

				// Check audio type - component descriptor
				lAudioType = NX_METADATA_PROGRAMMES_GetAudioType(stItem);
				if (lAudioType == eNX_META_AUDIO_TYPE_MULTI_CHANNEL)
				{
					pstContents->ulEventAttribute |= eNxPRODetail_Attr_Multiaudio;
					NX_APP_Info("- [i] MULTI Audio \n", i);
				}

				// Check Schedule;
				pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_Rsv_Watch);
				pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_Rsv_Record);
				NX_SCHEDULER_GetScheduleListByProgrammeInfo(stItem, pstScheduleList, &lScheduleNum);
				NX_APP_Info("- [i] Schedule Num:(%d) \n", i, lScheduleNum);
				for (lScheduleIdx = 0; lScheduleIdx < lScheduleNum; lScheduleIdx++)
				{
					pstScheduleItem = (DxSchedule_t*)HLIB_VECTOR_ItemAt(pstScheduleList, lScheduleIdx);
					if (pstScheduleItem)
					{
						switch (pstScheduleItem->eRsvType)
						{
							case DxRSVTYPE_WATCHING_EBR:
							case DxRSVTYPE_WATCHING_TBR:
							case DxRSVTYPE_WATCHING_SERIES_EBR:
								pstContents->ulSchSlot = pstScheduleItem->ulSlot;
								pstContents->ulEventAttribute |= eNxPRODetail_Attr_Rsv_Watch;
								break;

							case DxRSVTYPE_RECORD_EBR:
							case DxRSVTYPE_RECORD_TBR:
							case DxRSVTYPE_RECORD_SERIES_EBR:
							case DxRSVTYPE_RECORD_SATRECORD_TBR:
								pstContents->ulSchSlot = pstScheduleItem->ulSlot;
								pstContents->ulEventAttribute |= eNxPRODetail_Attr_Rsv_Record;
								break;
							default:
								break;
						}
						NX_APP_Info("- [i] pstScheduleItem->ulSlot(0x%x) \n", i, pstScheduleItem->ulSlot);
						break;
					}
				}
				if (pstScheduleList)
				{
					HLIB_VECTOR_Delete(pstScheduleList);
					pstScheduleList = NULL;
				}

				// TODO: [JHLEE] component descriptor 없을 경우에 대한 처리 필요.

				// start Time
				pstContents->ulStartTime = NX_METADATA_PROGRAMMES_GetStartTime(stItem);
				NX_APP_Info("- [i] pstContents->ulStartTime(%ld) \n", i, pstContents->ulStartTime);

				// duration
				pstContents->ulDuration = NX_METADATA_PROGRAMMES_GetDuration(stItem);
				NX_APP_Info("- [i] pstContents->ulDuration(%ld) \n", i, pstContents->ulDuration);

				// Genre
				ucNibblelevel1 = NX_METADATA_PROGRAMMES_GetGenreNibbleLevel1(stItem);
				ucNibblelevel2 = NX_METADATA_PROGRAMMES_GetGenreNibbleLevel2(stItem);

				NX_APP_Info("- [i] Content Nibble Level1(0x%x), Level2(0x%x) \n", i, ucNibblelevel1, ucNibblelevel2);
				szNibbleLevel1 = NX_PRISM_UTIL_GetGenreNibbleLevel1String(ucNibblelevel1);
				szNibbleLevel2 = NX_PRISM_UTIL_GetGenreNibbleLevel2String(ucNibblelevel1, ucNibblelevel2);
				HxSTD_MemSet(pstContents->szGenres, 0, sizeof(HCHAR) * NX_TEXT_SIZE_256);
				pstContents->szDuration[0] = '\0';
				if (szNibbleLevel1)
				{
					HxSTD_StrNCat(pstContents->szGenres, (char*)ONDK_GetString(szNibbleLevel1), NX_TEXT_SIZE_256);
					HxSTD_StrNCat(pstContents->szGenres, (char*)"/", NX_TEXT_SIZE_256);
				}
				if (szNibbleLevel2)
					HxSTD_StrNCat(pstContents->szGenres, (char*)ONDK_GetString(szNibbleLevel2), NX_TEXT_SIZE_256);

				NX_APP_Info("- [i] pstContents->szGenres(%s) \n", i, pstContents->szGenres);

				// Add Series Items - Recording + Schedule + Event
				ulSeriesId = NX_METADATA_PROGRAMMES_GetSeriesID(stItem);
				if ((bMakeSeries == TRUE) && (ulSeriesId > 0))
				{
					// Check count the Schedule by Series
					lSeriesSchNum = 0;
					NX_SCHEDULER_GetScheduleListBySeriesIdAndServiceUid(ulSeriesId, pstContents->ulSvcUid, NULL, &lSeriesSchNum);
					pstContents->lSeriesScheduleNum = lSeriesSchNum;

#if defined(CONFIG_MW_MM_PVR)
					// Check count the recorded file by Series
					lSeriesRecNum = 0;
					// TODO: [JHLEE] Add Count Series Recorded items
					pstContents->lSeriesRecordingNum = lSeriesRecNum;
#endif

					// Check count the Event by Series
					lSeriesEventNum = 0;
					NX_METADATA_EPG_GetResultProgrammeBySeriesId(pstContents->ulSvcUid, ulSeriesId, ulCurrentTime, NULL, &lSeriesEventNum);
					pstContents->lSeriesEventNum = lSeriesEventNum;

					if (pstContents->stSeritemsList)
					{
						NX_APP_Free(pstContents->stSeritemsList);
						pstContents->stSeritemsList = NULL;
					}

					pstContents->stSeritemsList = (NX_UiSeriseItem_t*)NX_APP_Calloc(sizeof(NX_UiSeriseItem_t) * (lSeriesSchNum + lSeriesRecNum + lSeriesEventNum));
					if (pstContents->stSeritemsList == NULL)
					{
						NX_APP_Error(" Error!!! NX_APP_Calloc() Fail !!\n");
						break;
					}
					else
					{
#if defined(CONFIG_MW_MM_PVR)
						if (lSeriesRecNum > 0)
						{
							nx_programDetail_MakeDetailInfoRecordingList(pstContents, stItem);
						}
#endif

						if (lSeriesSchNum > 0)
						{
							nx_programDetail_MakeDetailInfoScheduleList(pstContents, stItem);
						}

						if (lSeriesEventNum > 0)
						{
							nx_programDetail_MakeDetailInfoProgramList(pstContents, stItem);
						}
					}
				}
				else
				{
					pstContents->lSeriesScheduleNum = 0;
					pstContents->lSeriesRecordingNum = 0;
					pstContents->lSeriesEventNum = 0;
				}

				break;
			}
		}
	}
	else
	{
		if (pstContents->eCallerUiType == eNX_UCT_Schedule)
		{
			pstScheduleItem = NX_APP_Calloc(sizeof(DxSchedule_t));
			if (pstScheduleItem == NULL)
			{
				NX_APP_Error("ERROR!!! NX_APP_Calloc() Fail !!\n");
			}

			hError = NX_SCHEDULER_GetScheduleBySlotID(pstContents->ulSchSlot, pstScheduleItem);
			if (hError != ERR_OK)
			{
				NX_APP_Error("ERROR!!! NX_SCHEDULER_GetScheduleBySlotID() - Slot ID:(0x%x) Fail !!\n", pstContents->ulSchSlot);
			}
			else
			{
				HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstScheduleItem->stStartTime, &pstContents->ulStartTime);
				if (hError != ERR_OK)
				{
					NX_APP_Error("ERROR!!! HLIB_DATETIME_ConvertDateTimeToUnixTime() Fail !!\n");
				}

				pstContents->ulDuration = pstScheduleItem->ulDuration;
			}

			if (pstScheduleItem)
			{
				NX_APP_Free(pstScheduleItem);
				pstScheduleItem = NULL;
			}
		}
	}

	if (pstResult)
	{
		HLIB_VECTOR_Delete(pstResult); 	// new list 만 날리고, core epg 의 data 는 날리면 안됨.
		pstResult = NULL;
	}

	/* Make Duration */
	if (HLIB_DATETIME_IsValidUnixTime(pstContents->ulStartTime) == ERR_OK)
	{
		HxSTD_memset(&stStartTime, 0x00, sizeof(HxDATETIME_t));
		HxSTD_memset(&stDurationTime, 0x00, sizeof(HxDATETIME_Time_t));
		HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->ulStartTime, &stStartTime);
		HLIB_DATETIME_ConvertUnixTimeToTimeWithOutOffset(pstContents->ulDuration, &stDurationTime);

		ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stStartTime.stDate);
		ulTotalMin = stStartTime.stTime.ucMinute + stDurationTime.ucMinute;
		ulCalcHour = ulTotalMin / 60;
		ulCalcMin  = ulTotalMin % 60;
		HxSTD_sprintf(pstContents->szDuration, "%02d:%02d ~ %02d:%02d, %s %02d %s (%dmin)",
						stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute,
						(stStartTime.stTime.ucHour + stDurationTime.ucHour + ulCalcHour), ulCalcMin,
						ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex)),
						stStartTime.stDate.ucDay, ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByMonth(stStartTime.stDate.ucMonth)),
						((stDurationTime.ucHour *60) + stDurationTime.ucMinute));
	}
	else
	{
		HxSTD_memset(pstContents->szDuration, 0x00, sizeof(HCHAR) * NX_TEXT_SIZE_64);
		pstContents->szDuration[0] = '\0';
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MakeFMTData(NxUiProDetailContent_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HUINT32			h = 0;
	HINT32			lSeries = 0, i = 0, lLineCnt = 0;
	ONDKFontHandle		*hFontHandle = NULL;
	Text_Property_t 	stTextPropertyHead = {0, };
	FontMultiLine_Item_t	*arrItem = NULL;

	lSeries = (pstContents->eDispMode == eNxPRODetail_DispMode_Series) ? NX_PRODETAIL_SERIES_LIST_W : 0 ;
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_DESC_TEXT_X + lSeries, NX_PRODETAIL_SINGLE_DESC_TEXT_Y,
						NX_PRODETAIL_SINGLE_DESC_TEXT_W, NX_PRODETAIL_SINGLE_DESC_TEXT_H);

	stTextPropertyHead.ulFontSize = NX_PRISM_FONT_SIZE_20;
	stTextPropertyHead.ulFontAlign = TEXT_ALIGN_LEFT;
	stTextPropertyHead.cFontColor = COL(C_T_Orgel02);

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_20);

	arrItem = (FontMultiLine_Item_t *)NX_APP_Malloc(sizeof(FontMultiLine_Item_t) * NX_PRODETAIL_MAX_FMT_ITEM);
	lLineCnt = ONDK_FONT_GetLineCountWithParseMultiLine(hFontHandle, stRect.x, stRect.y, stRect.w, stRect.h, (HUINT8*)pstContents->szLongDes,
											8, NX_PRODETAIL_MAX_FMT_ITEM, ONDKSTF_EQUALSPACE, &h, arrItem);
	if (hFontHandle)
	{
		ONDK_FONT_Release (hFontHandle);
	}

	for (i = 0; i < lLineCnt; i++)
	{
		ONDK_GWM_FMT_AddItem(NX_PRODETAIL_SINGLE_DESC_TEXT_ID, arrItem[i].pszString, &stTextPropertyHead);
	}

	if (arrItem)
	{
		NX_APP_Free(arrItem);
		arrItem = NULL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonByWatchLiveTv(NxUiProDetailContent_t *pstContents)
{
	HBOOL				bChangable = FALSE;
	HERROR				hError = ERR_FAIL;
	NX_Channel_t		*pstLiveChannel = NULL;
	NX_ChListGroup_t	eCurChlistGroup = eNxChListGroup_ALL;
	NX_Channel_t		stChannel;


	/* Get Live Chanenl info */
	pstLiveChannel = NX_PRISM_LIVE_GetMasterChannel();
	if (pstLiveChannel == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetMasterChannel() Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	/* Check Live service */
	if (pstLiveChannel->svcUid == pstContents->ulSvcUid)
	{
		NX_APP_Info(" Play service not changed. Live SvcUid(0x%x), Play SvcUid(0x%x) \n", pstLiveChannel->svcUid, pstContents->ulSvcUid);
		ONDK_GWM_PostMessage(NULL, MSG_CORE_GUIDE_DONE_WATCH_LIVE_TV, 0, 0, 0, 0);
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_OK;
	}

	/* Get play service Info */
	eCurChlistGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
	HxSTD_memset(&stChannel, 0x00, sizeof(NX_Channel_t));
	hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eCurChlistGroup, pstContents->ulSvcUid, &stChannel);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() Fail - ulSvcUid(0x%x), eCurChlistGroup(0x%x)!! \n", pstContents->ulSvcUid, eCurChlistGroup);
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	/* Check is Tune avaliable */
	if (NX_CHANNEL_CheckPlayChangable(&stChannel, &bChangable) != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CHANNEL_CheckPlayChangable() Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_FAIL;
	}

	if (bChangable != TRUE)
	{
#if defined(CONFIG_MW_MM_PVR)
		nx_programDetail_CreateResponseDialouge(ONDK_GetString(RES_MESG_215_ID));
#else
		// TODO: [JHLEE] Add Zapper String
#endif
		return ONDK_RET_FAIL;
	}

	if (bChangable)
	{
		NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstContents->ulSvcUid, eCurChlistGroup, 0);
	}

	ONDK_GWM_APP_Destroy(0);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonByRecord(NxUiProDetailContent_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
	HERROR				hError = ERR_OK;
	HINT32				i = 0, lNum = 0;
	HUINT32 			ulUnixTime = 0;
	NX_Programme_t		*stItem = NULL;
	DxSchedule_t		*pstSchedule = NULL;
	HxVector_t			*pstProResults = HLIB_VECTOR_New(NULL, NULL);


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	// Check Time
	hError = NX_SYSTEM_GetTime(&ulUnixTime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetTime() Fail!! \n");
		return ONDK_RET_FAIL;
	}
	else
	{
		if (pstContents->ulStartTime <= ulUnixTime)
		{
			NX_APP_Error("Error!!! Not Set past Time Fail!! \n");
			return ONDK_RET_FAIL;
		}
	}

	if ((pstContents->ulEventAttribute & eNxPRODetail_Attr_Rsv_Watch) && (pstContents->ulSchSlot != NX_CORE_SCHEDULE_SLOTID_NULL))
	{
		hError = NX_SCHEDULER_CancelSchedule(pstContents->ulSchSlot);
		if (hError == ERR_OK)
		{
			pstContents->ulAfterScheduleAction = 0;
			pstContents->ulAfterScheduleAction = eNxPRODetail_Attr_Rsv_Record;
			return	ONDK_RET_OK;
		}
		else
		{
			NX_APP_Error("Error!!! NX_SCHEDULER_CancelSchedule() - ulSchSlot(0x%x) Fail !! \n", pstContents->ulSchSlot);
			return	ONDK_RET_FAIL;
		}
	}

	hError = nx_programDetail_CheckScheduleLimite();
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! nx_programDetail_CheckScheduleLimite() Access Schedule Limited!!\n");
		return	ONDK_RET_FAIL;
	}

	hError = NX_METADATA_EPG_GetResultProgrammeBySvcUidAndEventId(pstContents->ulSvcUid, pstContents->lEventId, pstProResults, &lNum);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_METADATA_EPG_GetResultProgrammeBySvcUidAndEventId() Fail - ulSvcUid:(0x%x), lEventId:(0x%x)!!\n", pstContents->ulSvcUid, pstContents->lEventId);
		return	ONDK_RET_FAIL;
	}

	pstSchedule = NX_APP_Calloc(sizeof(DxSchedule_t));
	NX_APP_Info("NX_METADATA_EPG_GetResultProgrammeBySvcUid - lNum [%d] \n", lNum);
	for (i = 0; i < lNum; i++)
	{
		stItem = (NX_Programme_t*)HLIB_VECTOR_ItemAt(pstProResults, i);
		if (stItem)
		{
			HxSTD_memset(pstSchedule, 0x00, sizeof(DxSchedule_t));
			hError = NX_SCHEDULER_CreateScheduleWithProgrammeInfo(DxRSVTYPE_RECORD_EBR, stItem, pstSchedule);
			if (hError == ERR_OK)
			{
				hError = NX_SCHEDULER_AddSchedule(pstSchedule);
				if (hError != ERR_OK)
				{
					NX_APP_Error("Error!!! NX_SCHEDULER_AddSchedule() - DxRSVTYPE_RECORD_EBR Fail !!\n");
				}
			}
			else
			{
				NX_APP_Error("Error!!! NX_SCHEDULER_CreateScheduleWithProgrammeInfo() - DxRSVTYPE_RECORD_EBR Fail !!\n");
			}
			break;
		}
	}

	if (pstSchedule)
	{
		NX_APP_Free(pstSchedule);
		pstSchedule = NULL;
	}

	if (pstProResults)
	{
		HLIB_VECTOR_Delete(pstProResults);	// new list 만 날리고, core epg 의 data 는 날리면 안됨.
		pstProResults = NULL;
	}
#endif

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonByCancelRecord(NxUiProDetailContent_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
	HERROR		hError = ERR_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	hError = NX_SCHEDULER_CancelSchedule(pstContents->ulSchSlot);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SCHEDULER_CancelSchedule() - Slot:(0x%x) Fail !!\n", pstContents->ulSchSlot);
		return	ONDK_RET_FAIL;
	}
#endif

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonByPlayRecording(NxUiProDetailContent_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
		// TODO: [JHLL] Add Action Button
#endif

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonByStopRecording(NxUiProDetailContent_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
		// TODO: [JHLL] Add Action Button
#endif

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonByPlayThisEpisode(NxUiProDetailContent_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
		// TODO: [JHLL] Add Action Button
#endif

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonByDeleteThisEpisode(NxUiProDetailContent_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
		// TODO: [JHLL] Add Action Button
#endif

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonByPlayThisProgramme(NxUiProDetailContent_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
		// TODO: [JHLL] Add Action Button
#endif

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonByDeleteThisProgramme(NxUiProDetailContent_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
		// TODO: [JHLL] Add Action Button
#endif

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonBySetReminder(NxUiProDetailContent_t *pstContents)
{
	HERROR				hError = ERR_OK;
	HINT32				i = 0, lNum = 0;
	HUINT32 			ulUnixTime = 0;
	NX_Programme_t		*stItem = NULL;
	DxSchedule_t 		*pstSchedule = NULL;
	HxVector_t			*pstProResults = HLIB_VECTOR_New(NULL, NULL);


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	// Check Time
	hError = NX_SYSTEM_GetTime(&ulUnixTime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetTime() Fail!! \n");
		return ONDK_RET_FAIL;
	}
	else
	{
		if (pstContents->ulStartTime <= ulUnixTime)
		{
			NX_APP_Error("Error!!! Not Set past Time Fail!! \n");
			return ONDK_RET_FAIL;
		}
	}

	if ((pstContents->ulEventAttribute & eNxPRODetail_Attr_Rsv_Record) && (pstContents->ulSchSlot != NX_CORE_SCHEDULE_SLOTID_NULL))
	{
		hError = NX_SCHEDULER_CancelSchedule(pstContents->ulSchSlot);
		if (hError == ERR_OK)
		{
			pstContents->ulAfterScheduleAction = 0;
			pstContents->ulAfterScheduleAction = eNxPRODetail_Attr_Rsv_Watch;
			return	ONDK_RET_OK;
		}
		else
		{
			NX_APP_Error("Error!!! NX_SCHEDULER_CancelSchedule() - ulSchSlot(0x%x) Fail !! \n", pstContents->ulSchSlot);
			return	ONDK_RET_FAIL;
		}
	}

	hError = nx_programDetail_CheckScheduleLimite();
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! nx_programDetail_CheckScheduleLimite() Access Schedule Limited!!\n");
		return	ONDK_RET_FAIL;
	}

	hError = NX_METADATA_EPG_GetResultProgrammeBySvcUidAndEventId(pstContents->ulSvcUid, pstContents->lEventId, pstProResults, &lNum);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_METADATA_EPG_GetResultProgrammeBySvcUidAndEventId() Fail - ulSvcUid:(0x%x), lEventId:(0x%x)!!\n", pstContents->ulSvcUid, pstContents->lEventId);
		return	ONDK_RET_FAIL;
	}

	pstSchedule = NX_APP_Calloc(sizeof(DxSchedule_t));
	NX_APP_Info("NX_METADATA_EPG_GetResultProgrammeBySvcUid - lNum [%d] \n", lNum);
	for (i = 0; i < lNum; i++)
	{
		stItem = (NX_Programme_t*)HLIB_VECTOR_ItemAt(pstProResults, i);
		if (stItem)
		{
			HxSTD_memset(pstSchedule, 0x00, sizeof(DxSchedule_t));
			hError = NX_SCHEDULER_CreateScheduleWithProgrammeInfo(DxRSVTYPE_WATCHING_EBR, stItem, pstSchedule);
			if (hError == ERR_OK)
			{
				hError = NX_SCHEDULER_AddSchedule(pstSchedule);
				if (hError != ERR_OK)
				{
					NX_APP_Error("Error!!! NX_SCHEDULER_AddSchedule() - DxRSVTYPE_WATCHING_EBR Fail !!\n");
				}
			}
			else
			{
				NX_APP_Error("Error!!! NX_SCHEDULER_CreateScheduleWithProgrammeInfo() - DxRSVTYPE_WATCHING_EBR Fail !!\n");
			}
			break;
		}
	}

	if (pstSchedule)
	{
		NX_APP_Free(pstSchedule);
		pstSchedule = NULL;
	}

	if (pstProResults)
	{
		HLIB_VECTOR_Delete(pstProResults); 	// new list 만 날리고, core epg 의 data 는 날리면 안됨.
		pstProResults = NULL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButtonByCancelReminder(NxUiProDetailContent_t *pstContents)
{
	HERROR		hError = ERR_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	hError = NX_SCHEDULER_CancelSchedule(pstContents->ulSchSlot);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SCHEDULER_CancelSchedule() - Slot:(0x%x) Fail !!\n", pstContents->ulSchSlot);
		return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_ActionButton(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		litemIdx = -1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	litemIdx = nx_programDetail_ConvertObjIdToActionItemIdx(p1);
	switch (litemIdx)
	{
		case eNxPRODetail_BTNItem_Record:
			nx_programDetail_ProcessingMsgCreate();
			nx_programDetail_ActionButtonByRecord(pstContents);
			break;
		case eNxPRODetail_BTNItem_CancelRecord:
			nx_programDetail_ProcessingMsgCreate();
			nx_programDetail_ActionButtonByCancelRecord(pstContents);
			break;
		case eNxPRODetail_BTNItem_PlayRecording:
			nx_programDetail_ActionButtonByPlayRecording(pstContents);
			break;
		case eNxPRODetail_BTNItem_StopRecording:
			nx_programDetail_ActionButtonByStopRecording(pstContents);
			break;
		case eNxPRODetail_BTNItem_PlayThisEpisode:
			nx_programDetail_ActionButtonByPlayThisEpisode(pstContents);
			break;
		case eNxPRODetail_BTNItem_DeleteThisEpisode:
			nx_programDetail_ActionButtonByDeleteThisEpisode(pstContents);
			break;
		case eNxPRODetail_BTNItem_PlayThisProgramme:
			nx_programDetail_ActionButtonByPlayThisProgramme(pstContents);
			break;
		case eNxPRODetail_BTNItem_DeleteThisProgramme:
			nx_programDetail_ActionButtonByDeleteThisProgramme(pstContents);
			break;
		case eNxPRODetail_BTNItem_SetReminder:
			nx_programDetail_ProcessingMsgCreate();
			nx_programDetail_ActionButtonBySetReminder(pstContents);
			break;
		case eNxPRODetail_BTNItem_CancelReminder:
			nx_programDetail_ProcessingMsgCreate();
			nx_programDetail_ActionButtonByCancelReminder(pstContents);
			break;
		case eNxPRODetail_BTNItem_WatchLiveTv:
			nx_programDetail_ActionButtonByWatchLiveTv(pstContents);
			break;
		default:
			NX_APP_Error("[%s:%d] Error!! UnKnow Item  Button ID:[0x%x], litemIdx:[%d]\n", __FUNCTION__, __HxLINE__, p1, litemIdx);
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_DrawList_Cursor_A2(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
										ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0, x = 0, y = 0, w = 0, h = 0, lItem = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = 0, ulStatus = GUIOBJ_NORMAL;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lText_y_off = 0, lImage_y_off = 0;
	HUINT8		*pDisable = NULL, *szImage = NULL;
	ONDK_Rect_t			stImgRect;
	ONDK_Size_t 		stImgSize = {0, 0};
	ONDKImageHandle 	hImgHandle = NULL;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDKFontHandle		hFontHandle;
	LIST_OBJECT			obj = (LIST_OBJECT)object;
	NxUiProDetailContent_t	*pstContents = NULL;


	pstContents = nx_programDetail_GetContents();
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	/* make coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* set properties */
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if (lTotal == 0)
	{
		NX_APP_Error("ERROR!!! (lTotal == 0) !!\n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("ERROR!!! ONDK_GWM_List_GetFontHandleObj() was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	ulStatus = ONDK_GWM_List_GetStatus(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	lText_y_off = NX_PRODETAIL_SERIES_LIST_TEXT_OFFSET_Y;
	lLastIdx = (lNumOfPage < lTotal) ? lNumOfPage : lTotal ;

	for (i = 0; i < lLastIdx; i++)
	{
		lItem = (lStart + i);
		if (lItem == lCurrent)	/* focus */
		{
			title_fontColor = COL(C_T_Bang_100);
			x = x1;
			y = y1 + lCursor_y_off;

			if (bUiTop > 0)
			{
				if ((ulStatus & GUIOBJ_HILIGHTED) || (ulStatus & GUIOBJ_PUSHED))
				{
					ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRODETAIL_SERIES_LIST_CURSOR_MG, COL(C_G_Base01_100));
				}
				else
				{
					title_fontColor = (lItem == lSelect) ? COL(C_T_Base01) : COL(C_T_Whistle_100) ;
				}
			}
			else
			{
				title_fontColor = COL(C_T_Base02);
			}
		}
		else
		{
			title_fontColor = (lItem == lSelect) ? COL(C_T_Base01) : COL(C_T_Whistle_100) ;
		}

		// Draw String
		x = x1 + NX_PRODETAIL_SERIES_LIST_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off + lText_y_off;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pstContents->stSeritemsList[lItem].szItemTitle, x, y,
					(w - NX_PRODETAIL_SERIES_LIST_TEXT_OFFSET_X), title_fontColor, ONDKSTF_LEFT);

		// Draw Icon
		switch (pstContents->stSeritemsList[lItem].eSeriesType)
		{
#if defined(CONFIG_MW_MM_PVR)
			case eNxSeries_Record:
				if ((pstContents->stSeritemsList[lItem].eResStatus == eDxREC_STS_RECORD_START) ||
					(pstContents->stSeritemsList[lItem].eResStatus == eDxREC_STS_RECORDING))
				{
					szImage = (HUINT8*)RES_205_04_REC_RECORDING_PNG;
				}
				else
				{
					szImage = (HUINT8*)RES_205_00_PLAY_PNG;
				}
				break;
#endif
			case eNxSeries_Schedule:
				switch (pstContents->stSeritemsList[lItem].eRsvType)
				{
					case DxRSVTYPE_WATCHING_EBR:
					case DxRSVTYPE_WATCHING_TBR:
					case DxRSVTYPE_WATCHING_SERIES_EBR:
						szImage = (HUINT8*)RES_205_00_RESERVATION_WATCH_PNG;
						break;
#if defined(CONFIG_MW_MM_PVR)
					case DxRSVTYPE_RECORD_EBR:
					case DxRSVTYPE_RECORD_TBR:
					case DxRSVTYPE_RECORD_INSTANT:
					case DxRSVTYPE_RECORD_DELAYED:
					case DxRSVTYPE_RECORD_SATRECORD_TBR:
						szImage = (HUINT8*)RES_205_04_REC_RESERVATION_RECORDING_PNG;
						break;
					case DxRSVTYPE_RECORD_SERIES_EBR:
						szImage = (HUINT8*)RES_205_04_REC_RESERVATION_SERIES_PNG;
						break;
#endif
					default:
						break;
				}
				break;
			default:
				szImage = NULL;
				break;
		}

		if (szImage != NULL)
		{
			hImgHandle = ONDK_IMG_CreateSelf(szImage);
			stImgSize = ONDK_IMG_GetSize(hImgHandle);
			x = x1 + w - NX_PRODETAIL_SERIES_LIST_IMG_OFFSET_X - stImgSize.w;
			y = y1 + lCursor_y_off;
			lImage_y_off = (lCellHeight - stImgSize.h)/2;
			lImage_y_off = (lImage_y_off < 0) ? 0 : lImage_y_off ;
			stImgRect = ONDK_Rect(x, y + lImage_y_off - 2, stImgSize.w, stImgSize.h);
			ONDK_GUICOM_IMG_Blit(screen, stImgRect, (HCHAR*)szImage);
			szImage = NULL;
		}

		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_DrawSeriesList(NxUiProDetailContent_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	if (pstContents->eDispMode != eNxPRODetail_DispMode_Series)
	{
		return	ONDK_RET_UNSUPPORTED;
	}

	/* Create List */
	stRect = ONDK_Rect(NX_PRODETAIL_SERIES_LIST_X, NX_PRODETAIL_SERIES_LIST_Y,
						NX_PRODETAIL_SERIES_LIST_W, NX_PRODETAIL_SERIES_LIST_H);
	ONDK_GWM_List_Create(NX_PRODETAIL_SERIES_LIST_ID, &stRect, "NxSeriesList", NX_PRODETAIL_SERIES_LIST_LS,
							pstContents->lSeriesItemNum, NX_PRODETAIL_SERIES_LIST_ITEM_MAX, 0);
	ONDK_GWM_List_SetCurrentIndex(NX_PRODETAIL_SERIES_LIST_ID, pstContents->lSeriesCurIndex);
	ONDK_GWM_List_SetSelectedIndex(NX_PRODETAIL_SERIES_LIST_ID, pstContents->lSeriesCurIndex);
	ONDK_GWM_List_SetFontHandle(NX_PRODETAIL_SERIES_LIST_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRODETAIL_SERIES_LIST_ID, (GWM_ObjDraw_t)nx_programDetail_DrawList_Cursor_A2);

	/* Draw List Scroll Page */
	HxSTD_sprintf(pstContents->szSeriesPage, "%d / %d", (pstContents->lSeriesCurIndex + 1), pstContents->lSeriesItemNum);
	stRect = ONDK_Rect(NX_PRODETAIL_SERIES_LIST_PAGE_X, NX_PRODETAIL_SERIES_LIST_PAGE_Y,
						NX_PRODETAIL_SERIES_LIST_PAGE_W, NX_PRODETAIL_SERIES_LIST_PAGE_H);
	ONDK_GWM_COM_Text_Create(NX_PRODETAIL_SERIES_LIST_PAGE_ID, &stRect, (HCHAR*)pstContents->szSeriesPage);
	ONDK_GWM_COM_Text_SetFont(NX_PRODETAIL_SERIES_LIST_PAGE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRODETAIL_SERIES_LIST_PAGE_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRODETAIL_SERIES_LIST_PAGE_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRODETAIL_SERIES_LIST_PAGE_ID, COL(C_T_Gong06), COL(C_T_Bang_100));

	/* Devider Line */
	stRect = ONDK_Rect(NX_PRODETAIL_SERIES_DIVIDER_X, NX_PRODETAIL_SERIES_DIVIDER_Y,
						NX_PRODETAIL_SERIES_DIVIDER_W, NX_PRODETAIL_SERIES_DIVIDER_H);
	ONDK_GWM_COM_Rect_Create(NX_PRODETAIL_SERIES_DIVIDER_ID, &stRect, COL(C_G_Whistle_15));

	return	ONDK_RET_OK;
}


static ONDK_Result_t		nx_programDetail_DrawFMTItem(HINT32 ID, ONDKSurfaceHandle screen, FMT_DrawItem_t *drawItem)
{
	HINT32			x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;
	HUINT8			*pucTempString = NULL;
	ONDKFontHandle	*hFontHandle;
	ONDK_Rect_t		stItemRect = { 0, 0, 0, 0 };

	pucTempString = (HUINT8*)drawItem->str;
	if (pucTempString == NULL)
	{
		NX_APP_Error("[%s:%d] Error!! No Extended Info Text\n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_INITFAIL;
	}

	ONDK_CopyRect(&stItemRect, &(drawItem->itemRect));

	/* init coordinate */
	x1 = stItemRect.x;
	y1 = stItemRect.y;
	x2 = stItemRect.x + stItemRect.w;
	y2 = stItemRect.y + stItemRect.h;

	w = stItemRect.w;
	h = stItemRect.h;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_20);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	//ONDK_DRAW_RectElement(screen, x1, y1, w, h, 0xFF820610);
	ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, x1, y1, w, h, pucTempString, NX_PRISM_FONT_VSPACE, COL(C_T_Whistle_100), ONDKSTF_EQUALSPACE);
	//ONDK_FONT_DrawStringAlign(screen, hFontHandle, (char*)pucTempString, x, y, w, COL(C_T_Whistle_100), ONDKSTF_LEFT);

	if (hFontHandle)
	{
		ONDK_FONT_Release (hFontHandle);
	}

	NX_COMMON_NOT_USED_PARAM(ID);
	return	ONDK_RET_OK;
}


static ONDK_Result_t		nx_programDetail_DrawFMTScroll(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect, FMT_OBJECT Object)
{
	HINT32			lTotalPage = 0;
	HUINT32			ulAttrib = 0, ulStartIndex = 0, ulLinePerPage = 0, ulItemNum = 0;
	HINT32			lSeries = 0;
	NxUiProDetailContent_t	*pstContents = NULL;

	pstContents = nx_programDetail_GetContents();
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	lSeries = (pstContents->eDispMode == eNxPRODetail_DispMode_Series) ? NX_PRODETAIL_SERIES_LIST_W : 0 ;
	ONDK_GWM_FMT_GetProperty(Object, &ulAttrib, &ulStartIndex, &ulLinePerPage, &ulItemNum);
	if (ulItemNum <= ulLinePerPage)
	{
		return	ONDK_RET_OK;
	}

	if (ulItemNum < ulLinePerPage)
	{
		lTotalPage = 1;
	}
	else
	{
		lTotalPage = (ulItemNum/ulLinePerPage) + ((ulItemNum%ulLinePerPage) ? 1 : 0);
	}

	pstContents->lMaxPage = lTotalPage;
	HxSTD_sprintf(pstContents->szPageScroll, "%d / %d", pstContents->lCurPage, lTotalPage);
	ONDK_GWM_COM_Text_SetText (NX_PRODETAIL_SINGLE_SCROLL_PAGE_ID, (HCHAR*)pstContents->szPageScroll);
	ONDK_GWM_APP_InvalidateObject(NX_PRODETAIL_SINGLE_SCROLL_PAGE_ID);

	NX_COMMON_NOT_USED_PARAM(rect);
	NX_COMMON_NOT_USED_PARAM(screen);
	NX_COMMON_NOT_USED_PARAM(area);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_DrawIconList(NxUiProDetailContent_t *pstContents)
{
	HCHAR 			*szSubtitleTypeImg = NULL, *szCasTypeRes = RES_202_00_CAS_MULTI_PNG;
	HCHAR			*szRatingRes = NULL;
	HINT32			lSeries = 0, ulOffsetX = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 }, stImgRect = { 0, 0, 0, 0 };

	lSeries = (pstContents->eDispMode == eNxPRODetail_DispMode_Series) ? NX_PRODETAIL_SERIES_LIST_W : 0 ;
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_ICON_X + lSeries, NX_PRODETAIL_SINGLE_ICON_Y, NX_PRODETAIL_SINGLE_ICON_W, NX_PRODETAIL_SINGLE_ICON_H);
	ONDK_CopyRect(&stImgRect, &stRect);
	ulOffsetX = stImgRect.x;

#if defined(CONFIG_MW_MM_PVR)
	// Recorded Icon
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Record)
	{
		stImgRect.x = ulOffsetX;
		stImgRect.w = NX_PRODETAIL_SINGLE_ICON_RECORDED_W;
		ONDK_GWM_COM_Image_Create(NX_PRODETAIL_SINGLE_ICON_01_ID, &stImgRect);
		ONDK_GWM_COM_Image_SetImage(NX_PRODETAIL_SINGLE_ICON_01_ID, (HCHAR*)RES_205_04_REC_RECORDING_WATCH_PNG);
		ulOffsetX = (stImgRect.x + stImgRect.w + NX_PRODETAIL_SINGLE_ICON_MG);
	}
#endif

	// On Demand
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_OnDemand)
	{
		stImgRect.x = ulOffsetX;
		stImgRect.w = NX_PRODETAIL_SINGLE_ICON_ONDEMAND_W;
		// TODO: [JHLEE] Add Image
		//ONDK_GWM_COM_Image_Create(NX_PRODETAIL_SINGLE_ICON_02_ID, &stImgRect);
		//ONDK_GWM_COM_Image_SetImage(NX_PRODETAIL_SINGLE_ICON_02_ID, (HCHAR*)RES_217_00_MULTIPLEAUDIO_PNG);
		ulOffsetX = (stImgRect.x + stImgRect.w + NX_PRODETAIL_SINGLE_ICON_MG);
	}
	// HD
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_HD)
	{
		stImgRect.x = ulOffsetX;
		stImgRect.w = NX_PRODETAIL_SINGLE_ICON_HD_W;
		ONDK_GWM_COM_Image_Create(NX_PRODETAIL_SINGLE_ICON_03_ID, &stImgRect);
		ONDK_GWM_COM_Image_SetImage(NX_PRODETAIL_SINGLE_ICON_03_ID, (HCHAR*)RES_203_00_RESOLUTION_HD_PNG);
		ulOffsetX = (stImgRect.x + stImgRect.w + NX_PRODETAIL_SINGLE_ICON_MG);
	}
	// Rating
	if (pstContents->lRating)
	{
		stImgRect.x = ulOffsetX;
		stImgRect.w = NX_PRODETAIL_SINGLE_ICON_RATE_W;
		szRatingRes = NX_PRISM_UTIL_GetRatingResource(pstContents->lRating);
		if (szRatingRes)
		{
			ONDK_GWM_COM_Image_Create(NX_PRODETAIL_SINGLE_ICON_04_ID, &stImgRect);
			ONDK_GWM_COM_Image_SetImage(NX_PRODETAIL_SINGLE_ICON_04_ID, (HCHAR*)szRatingRes);
			ulOffsetX = (stImgRect.x + stImgRect.w + NX_PRODETAIL_SINGLE_ICON_MG);
		}
	}
	// CAS
	if (pstContents->lCasId)
	{
		stImgRect.x = ulOffsetX;
		stImgRect.w = NX_PRODETAIL_SINGLE_ICON_CAS_W;
		szCasTypeRes = NX_PRISM_UTIL_GetCasTypeResource(pstContents->lCasId);
		if (szCasTypeRes)
		{
			ONDK_GWM_COM_Image_Create(NX_PRODETAIL_SINGLE_ICON_05_ID, &stImgRect);
			ONDK_GWM_COM_Image_SetImage(NX_PRODETAIL_SINGLE_ICON_05_ID, (HCHAR*)szCasTypeRes);
			ulOffsetX = (stImgRect.x + stImgRect.w + NX_PRODETAIL_SINGLE_ICON_MG);
		}
	}
	// Teletext
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Teletext)
	{
		stImgRect.x = ulOffsetX;
		stImgRect.w = NX_PRODETAIL_SINGLE_ICON_TTX_W;
		ONDK_GWM_COM_Image_Create(NX_PRODETAIL_SINGLE_ICON_06_ID, &stImgRect);
		ONDK_GWM_COM_Image_SetImage(NX_PRODETAIL_SINGLE_ICON_06_ID, (HCHAR*)RES_219_00_TELETEXT_PNG);
		ulOffsetX = (stImgRect.x + stImgRect.w + NX_PRODETAIL_SINGLE_ICON_MG);
	}
	// Subtitle
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Subtitle)
	{
		stImgRect.x = ulOffsetX;
		stImgRect.w = NX_PRODETAIL_SINGLE_ICON_SUBTITLE_W;
		szSubtitleTypeImg = NX_PRISM_UTIL_GetSubtitleResource(pstContents->lSubtitleType);
		if (szSubtitleTypeImg)
		{
			ONDK_GWM_COM_Image_Create(NX_PRODETAIL_SINGLE_ICON_07_ID, &stImgRect);
			ONDK_GWM_COM_Image_SetImage(NX_PRODETAIL_SINGLE_ICON_07_ID, (HCHAR*)szSubtitleTypeImg);
			ulOffsetX = (stImgRect.x + stImgRect.w + NX_PRODETAIL_SINGLE_ICON_MG);
		}
	}
	// Multiple Audio
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Multiaudio)
	{
		stImgRect.x = ulOffsetX;
		stImgRect.w = NX_PRODETAIL_SINGLE_ICON_MULTI_AUDIO_W;
		ONDK_GWM_COM_Image_Create(NX_PRODETAIL_SINGLE_ICON_08_ID, &stImgRect);
		ONDK_GWM_COM_Image_SetImage(NX_PRODETAIL_SINGLE_ICON_08_ID, (HCHAR*)RES_217_00_MULTIPLEAUDIO_PNG);
		ulOffsetX = (stImgRect.x + stImgRect.w + NX_PRODETAIL_SINGLE_ICON_MG);
	}
	// DRM
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_DRM)
	{
		stImgRect.x = ulOffsetX;
		stImgRect.w = NX_PRODETAIL_SINGLE_ICON_DRM_W;
		ONDK_GWM_COM_Image_Create(NX_PRODETAIL_SINGLE_ICON_09_ID, &stImgRect);
		ONDK_GWM_COM_Image_SetImage(NX_PRODETAIL_SINGLE_ICON_09_ID, (HCHAR*)RES_211_00_LOCK_DRM_PNG);
		ulOffsetX = (stImgRect.x + stImgRect.w + NX_PRODETAIL_SINGLE_ICON_MG);
	}
	// LOCK
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Lock)
	{
		stImgRect.x = ulOffsetX;
		stImgRect.w = NX_PRODETAIL_SINGLE_ICON_LOCK_W;
		ONDK_GWM_COM_Image_Create(NX_PRODETAIL_SINGLE_ICON_10_ID, &stImgRect);
		ONDK_GWM_COM_Image_SetImage(NX_PRODETAIL_SINGLE_ICON_10_ID, (HCHAR*)RES_211_00_LOCK_PNG);
	}

	return	ONDK_RET_OK;
}


static void		nx_programDetail_DeleteAllActionItem(void)
{
	HINT32		lObjId = -1;
	HUINT32		i = 0;

	for (i = 0; i < eNxPRODetail_BTNItem_Max; i++)
	{
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(i);
		ONDK_GWM_Obj_Destroy(lObjId);
	}
}


static ONDK_Result_t	nx_programDetail_AddActionItem_FutureSeries(NxUiProDetailContent_t *pstContents)
{
	HBOOL			bSetFocus = FALSE;
	HINT32			lObjId = -1, lSeries = 0, lFocusObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };


	lSeries = (pstContents->eDispMode == eNxPRODetail_DispMode_Series) ? NX_PRODETAIL_SERIES_LIST_W : 0 ;
	stRect = ONDK_Rect(NX_PRODETAIL_ACTON_BTN_X + lSeries, NX_PRODETAIL_ACTON_BTN_Y, NX_PRODETAIL_ACTON_BTN_W, NX_PRODETAIL_ACTON_BTN_H);

#if defined(CONFIG_MW_MM_PVR)
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Rsv_Record)
	{
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_CancelRecord);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_RECORDING_01_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
		if (bSetFocus == FALSE)
		{
			lFocusObjId = lObjId;
			bSetFocus = TRUE;
		}
	}
	else
	{
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_Record);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_RECORD_46_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
		if (bSetFocus == FALSE)
		{
			lFocusObjId = lObjId;
			bSetFocus = TRUE;
		}
	}
#endif

	stRect.y += NX_PRODETAIL_ACTON_BTN_H;
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Rsv_Watch)
	{
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_CancelReminder);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_REMINDER_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
		if (bSetFocus == FALSE)
		{
			lFocusObjId = lObjId;
			bSetFocus = TRUE;
		}
	}
	else
	{
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_SetReminder);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_SET_REMINDER_01_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
		if (bSetFocus == FALSE)
		{
			lFocusObjId = lObjId;
			bSetFocus = TRUE;
		}
	}

	// Set Focus
	ONDK_GWM_Obj_SetFocus(lFocusObjId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_AddActionItem_FutureSingle(NxUiProDetailContent_t *pstContents)
{
	HBOOL			bSetFocus = FALSE;
	HINT32			lObjId = -1, lSeries = 0, lFocusObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };


	lSeries = (pstContents->eDispMode == eNxPRODetail_DispMode_Series) ? NX_PRODETAIL_SERIES_LIST_W : 0 ;
	stRect = ONDK_Rect(NX_PRODETAIL_ACTON_BTN_X + lSeries, NX_PRODETAIL_ACTON_BTN_Y, NX_PRODETAIL_ACTON_BTN_W, NX_PRODETAIL_ACTON_BTN_H);

#if defined(CONFIG_MW_MM_PVR)
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Rsv_Record)
	{
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_CancelRecord);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_RECORDING_01_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
		if (bSetFocus == FALSE)
		{
			lFocusObjId = lObjId;
			bSetFocus = TRUE;
		}
	}
	else
	{
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_Record);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_RECORD_46_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
		if (bSetFocus == FALSE)
		{
			lFocusObjId = lObjId;
			bSetFocus = TRUE;
		}
	}
#endif

	stRect.y += NX_PRODETAIL_ACTON_BTN_H;
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Rsv_Watch)
	{
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_CancelReminder);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_THIS_PROGRAMME_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
		if (bSetFocus == FALSE)
		{
			lFocusObjId = lObjId;
			bSetFocus = TRUE;
		}
	}
	else
	{
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_SetReminder);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_SET_REMINDER_01_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
		if (bSetFocus == FALSE)
		{
			lFocusObjId = lObjId;
			bSetFocus = TRUE;
		}
	}

	// Set Focus
	ONDK_GWM_Obj_SetFocus(lFocusObjId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_AddActionItem_OnAirSeries(NxUiProDetailContent_t *pstContents)
{
	HBOOL			bSetFocus = FALSE;
	HINT32			lObjId = -1, lSeries = 0, lFocusObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };


	lSeries = (pstContents->eDispMode == eNxPRODetail_DispMode_Series) ? NX_PRODETAIL_SERIES_LIST_W : 0 ;
	stRect = ONDK_Rect(NX_PRODETAIL_ACTON_BTN_X + lSeries, NX_PRODETAIL_ACTON_BTN_Y, NX_PRODETAIL_ACTON_BTN_W, NX_PRODETAIL_ACTON_BTN_H);

	lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_WatchLiveTv);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_WATCH_LIVE_TV_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);

	// TODO: Add Focus Check
	if (bSetFocus == FALSE)
	{
		lFocusObjId = lObjId;
		bSetFocus = TRUE;
	}

#if defined(CONFIG_MW_MM_PVR)
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Record)
	{
		stRect.y += NX_PRODETAIL_ACTON_BTN_H;
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_PlayRecording);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_PLAY_RECORDING_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
		if (bSetFocus == FALSE)
		{
			lFocusObjId = lObjId;
			bSetFocus = TRUE;
		}

		stRect.y += NX_PRODETAIL_ACTON_BTN_H;
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_StopRecording);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_STOP_RECORDING_01_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
	}
	else
#endif
	{
#if defined(CONFIG_MW_MM_PVR)
		stRect.y += NX_PRODETAIL_ACTON_BTN_H;
		if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Rsv_Record)
		{
			lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_CancelRecord);
			ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_RECORDING_01_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
			if (bSetFocus == FALSE)
			{
				lFocusObjId = lObjId;
				bSetFocus = TRUE;
			}
		}
		else
		{
			lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_Record);
			ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_RECORD_46_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
			if (bSetFocus == FALSE)
			{
				lFocusObjId = lObjId;
				bSetFocus = TRUE;
			}
		}
#endif
		stRect.y += NX_PRODETAIL_ACTON_BTN_H;
		if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Rsv_Watch)
		{
			lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_CancelReminder);
			ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_REMINDER_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
			if (bSetFocus == FALSE)
			{
				lFocusObjId = lObjId;
				bSetFocus = TRUE;
			}
		}
		else
		{
			lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_SetReminder);
			ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_SET_REMINDER_01_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
			if (bSetFocus == FALSE)
			{
				lFocusObjId = lObjId;
				bSetFocus = TRUE;
			}
		}
	}

	// Set Focus
	ONDK_GWM_Obj_SetFocus(lFocusObjId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_AddActionItem_OnAirSingle(NxUiProDetailContent_t *pstContents)
{
	HBOOL			bSetFocus = FALSE;
	HINT32			lObjId = -1, lSeries = 0, lFocusObjId = -1;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };


	lSeries = (pstContents->eDispMode == eNxPRODetail_DispMode_Series) ? NX_PRODETAIL_SERIES_LIST_W : 0 ;
	stRect = ONDK_Rect(NX_PRODETAIL_ACTON_BTN_X + lSeries, NX_PRODETAIL_ACTON_BTN_Y, NX_PRODETAIL_ACTON_BTN_W, NX_PRODETAIL_ACTON_BTN_H);

	lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_WatchLiveTv);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_WATCH_LIVE_TV_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);

	if (bSetFocus == FALSE)
	{
		lFocusObjId = lObjId;
		bSetFocus = TRUE;
	}

#if defined(CONFIG_MW_MM_PVR)
	if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Record)
	{
		stRect.y += NX_PRODETAIL_ACTON_BTN_H;
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_PlayRecording);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_PLAY_RECORDING_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);

		stRect.y += NX_PRODETAIL_ACTON_BTN_H;
		lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_StopRecording);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_STOP_RECORDING_01_ID));
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);
	}
#endif

	// Set Focus
	ONDK_GWM_Obj_SetFocus(lFocusObjId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_AddActionItem_RecordedSeries(NxUiProDetailContent_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
	HBOOL			bSetFocus = FALSE;
	HINT32			lObjId = -1, lSeries = 0, lFocusObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	lSeries = (pstContents->eDispMode == eNxPRODetail_DispMode_Series) ? NX_PRODETAIL_SERIES_LIST_W : 0 ;
	stRect = ONDK_Rect(NX_PRODETAIL_ACTON_BTN_X + lSeries, NX_PRODETAIL_ACTON_BTN_Y, NX_PRODETAIL_ACTON_BTN_W, NX_PRODETAIL_ACTON_BTN_H);

	lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_PlayThisEpisode);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_PLAY_THIS_EPISODE_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);

	if (bSetFocus == FALSE)
	{
		lFocusObjId = lObjId;
		bSetFocus = TRUE;
	}

	stRect.y += NX_PRODETAIL_ACTON_BTN_H;
	lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_DeleteThisEpisode);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_DELETE_THIS_EPISODE_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);

	// Set Focus
	ONDK_GWM_Obj_SetFocus(lFocusObjId);
#else
	ONDK_GWM_Obj_SetFocus(NX_PRODETAIL_SINGLE_SCROLL_L_ID);
#endif

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_AddActionItem_RecordedSingle(NxUiProDetailContent_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
	HBOOL			bSetFocus = FALSE;
	HINT32			lObjId = -1, lSeries = 0, lFocusObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	lSeries = (pstContents->eDispMode == eNxPRODetail_DispMode_Series) ? NX_PRODETAIL_SERIES_LIST_W : 0 ;
	stRect = ONDK_Rect(NX_PRODETAIL_ACTON_BTN_X + lSeries, NX_PRODETAIL_ACTON_BTN_Y, NX_PRODETAIL_ACTON_BTN_W, NX_PRODETAIL_ACTON_BTN_H);

	lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_PlayThisProgramme);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_PLAY_THIS_PROGRAMME_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);

	if (bSetFocus == FALSE)
	{
		lFocusObjId = lObjId;
		bSetFocus = TRUE;
	}

	stRect.y += NX_PRODETAIL_ACTON_BTN_H;
	lObjId = nx_programDetail_ConvertActionItemIdxToObjId(eNxPRODetail_BTNItem_DeleteThisProgramme);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_DELETE_THIS_PROGRAMME_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawButton_C1);

	// Set Focus
	ONDK_GWM_Obj_SetFocus(lFocusObjId);
#else
	ONDK_GWM_Obj_SetFocus(NX_PRODETAIL_SINGLE_SCROLL_L_ID);
#endif

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_SetActionBtnUiObject(NxUiProDetailContent_t *pstContents)
{
	NX_APP_Info("eBtnMode [%d] \n", pstContents->eBtnMode);

	if (pstContents->eCallerUiType == eNX_UCT_Schedule)
	{
		ONDK_GWM_Obj_SetFocus(NX_PRODETAIL_SINGLE_SCROLL_L_ID);
		NX_APP_Error("Not Support Button mode !!\n");
		return	ONDK_RET_OK;
	}

	switch (pstContents->eBtnMode)
	{
		case eNxPRODetail_BTN_Future_Series:
			nx_programDetail_AddActionItem_FutureSeries(pstContents);
			break;
		case eNxPRODetail_BTN_Future_Single:
			nx_programDetail_AddActionItem_FutureSingle(pstContents);
			break;
		case eNxPRODetail_BTN_OnAir_Series:
			nx_programDetail_AddActionItem_OnAirSeries(pstContents);
			break;
		case eNxPRODetail_BTN_OnAir_Single:
			nx_programDetail_AddActionItem_OnAirSingle(pstContents);
			break;
		case eNxPRODetail_BTN_Resored_Series:
			nx_programDetail_AddActionItem_RecordedSeries(pstContents);
			break;
		case eNxPRODetail_BTN_Resored_Single:
			nx_programDetail_AddActionItem_RecordedSingle(pstContents);
			break;
		case eNxPRODetail_BTN_NoneRecorded:
			nx_programDetail_AddActionItem_OnAirSingle(pstContents);
			break;
		default:
			NX_APP_Error("[%s:%d] UnKnow Action button mode (%d)\n", __FUNCTION__, __HxLINE__, pstContents->eBtnMode);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_SetUiObject(NxUiProDetailContent_t *pstContents)
{
	HINT32			lSeries = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };


	ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_BACKGROUND_BOX_X, NX_PRISM_UI_BACKGROUND_BOX_Y,
					NX_PRISM_UI_BACKGROUND_BOX_WIDTH, NX_PRISM_UI_BACKGROUND_BOX_HEIGHT);

	lSeries = (pstContents->eDispMode == eNxPRODetail_DispMode_Series) ? NX_PRODETAIL_SERIES_LIST_W : 0 ;

	/* Draw BG Frame */
	stRect = ONDK_Rect(NX_PRODETAIL_FRAME_X, NX_PRODETAIL_FRAME_Y, NX_PRODETAIL_FRAME_W, NX_PRODETAIL_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_BACKGROUND_BOX_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_BACKGROUND_BOX_ID, NX_PRISM_COMP_DrawFrameBG_SS);

	/* Draw Series */
	nx_programDetail_DrawSeriesList(pstContents);

	/* Event Name */
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_EVETNT_TEXT_X + lSeries, NX_PRODETAIL_SINGLE_EVETNT_TEXT_Y,
						NX_PRODETAIL_SINGLE_EVETNT_TEXT_W, NX_PRODETAIL_SINGLE_EVETNT_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRODETAIL_SINGLE_EVETNT_TEXT_ID, &stRect, (HCHAR*)pstContents->szEventName);
	ONDK_GWM_COM_Text_SetFont(NX_PRODETAIL_SINGLE_EVETNT_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRODETAIL_SINGLE_EVETNT_TEXT_ID, NX_PRISM_FONT_SIZE_36);
	ONDK_GWM_COM_Text_SetAlign(NX_PRODETAIL_SINGLE_EVETNT_TEXT_ID, (TEXT_ALIGN_LEFT | TEXT_MULTILINED));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRODETAIL_SINGLE_EVETNT_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Service Name */
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_SERVICE_TEXT_X + lSeries, NX_PRODETAIL_SINGLE_SERVICE_TEXT_Y,
						NX_PRODETAIL_SINGLE_SERVICE_TEXT_W, NX_PRODETAIL_SINGLE_SERVICE_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRODETAIL_SINGLE_SERVICE_TEXT_ID, &stRect, (HCHAR*)pstContents->szSvcInfo);
	ONDK_GWM_COM_Text_SetFont(NX_PRODETAIL_SINGLE_SERVICE_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRODETAIL_SINGLE_SERVICE_TEXT_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_PRODETAIL_SINGLE_SERVICE_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRODETAIL_SINGLE_SERVICE_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));

	/* Duration */
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_DURATION_TEXT_X + lSeries, NX_PRODETAIL_SINGLE_DURATION_TEXT_Y,
						NX_PRODETAIL_SINGLE_DURATION_TEXT_W, NX_PRODETAIL_SINGLE_DURATION_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRODETAIL_SINGLE_DURATION_TEXT_ID, &stRect, (HCHAR*)pstContents->szDuration);
	ONDK_GWM_COM_Text_SetFont(NX_PRODETAIL_SINGLE_DURATION_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRODETAIL_SINGLE_DURATION_TEXT_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRODETAIL_SINGLE_DURATION_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRODETAIL_SINGLE_DURATION_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));

	/* Draw Icon List */
	nx_programDetail_DrawIconList(pstContents);

	/* Draw Genre */
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_GENRE_TEXT_X + lSeries, NX_PRODETAIL_SINGLE_GENRE_TEXT_Y,
						NX_PRODETAIL_SINGLE_GENRE_TEXT_W, NX_PRODETAIL_SINGLE_GENRE_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRODETAIL_SINGLE_GENRE_TEXT_ID, &stRect, (HCHAR*)pstContents->szGenres);
	ONDK_GWM_COM_Text_SetFont(NX_PRODETAIL_SINGLE_GENRE_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRODETAIL_SINGLE_GENRE_TEXT_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRODETAIL_SINGLE_GENRE_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRODETAIL_SINGLE_GENRE_TEXT_ID, COL(C_T_Orgel02), COL(C_T_Bang_100));

	/* Draw Extended Info BOX*/
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_DESC_BOX_X + lSeries, NX_PRODETAIL_SINGLE_DESC_BOX_Y,
						NX_PRODETAIL_SINGLE_DESC_BOX_W, NX_PRODETAIL_SINGLE_DESC_BOX_H);
	ONDK_GWM_COM_Rect_Create(NX_PRODETAIL_SINGLE_DESC_BOX_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRODETAIL_SINGLE_DESC_BOX_ID, NX_PRISM_COMP_DrawFrameBox_A);

	/* Draw Extended Info */
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_DESC_TEXT_X + lSeries, NX_PRODETAIL_SINGLE_DESC_TEXT_Y,
						NX_PRODETAIL_SINGLE_DESC_TEXT_W, NX_PRODETAIL_SINGLE_DESC_TEXT_H);
	ONDK_GWM_FMT_Create(NX_PRODETAIL_SINGLE_DESC_TEXT_ID, &stRect, FMT_OWN_DRAWITEM | FMT_SCROLL_ON, NX_PRODETAIL_PAGE_NUM_MAX);
	ONDK_GWM_FMT_ChangeItemDrawMethod(NX_PRODETAIL_SINGLE_DESC_TEXT_ID, nx_programDetail_DrawFMTItem);
	ONDK_GWM_FMT_ChangeScrollDrawMethod(NX_PRODETAIL_SINGLE_DESC_TEXT_ID, nx_programDetail_DrawFMTScroll);
	ONDK_GWM_Obj_DisableObject(NX_PRODETAIL_SINGLE_DESC_TEXT_ID);

	/* Draw Scroll Left */
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_SCROLL_L_X + lSeries, NX_PRODETAIL_SINGLE_SCROLL_L_Y,
						NX_PRODETAIL_SINGLE_SCROLL_L_W, NX_PRODETAIL_SINGLE_SCROLL_L_H);
	ONDK_GWM_COM_Button_Create(NX_PRODETAIL_SINGLE_SCROLL_L_ID, &stRect, (HCHAR*)NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRODETAIL_SINGLE_SCROLL_L_ID, NX_PRISM_COMP_DrawButton_Scroll_ArrowLBox);

	/* Draw Scroll Page */
	HxSTD_sprintf(pstContents->szPageScroll, "%d / %d", pstContents->lCurPage, pstContents->lMaxPage);
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_SCROLL_PAGE_X + lSeries, NX_PRODETAIL_SINGLE_SCROLL_PAGE_Y,
						NX_PRODETAIL_SINGLE_SCROLL_PAGE_W, NX_PRODETAIL_SINGLE_SCROLL_PAGE_H);
	ONDK_GWM_COM_Text_Create(NX_PRODETAIL_SINGLE_SCROLL_PAGE_ID, &stRect, (HCHAR*)pstContents->szPageScroll);
	ONDK_GWM_COM_Text_SetFont(NX_PRODETAIL_SINGLE_SCROLL_PAGE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRODETAIL_SINGLE_SCROLL_PAGE_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRODETAIL_SINGLE_SCROLL_PAGE_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRODETAIL_SINGLE_SCROLL_PAGE_ID, COL(C_T_Gong05), COL(C_T_Bang_100));

	/* Draw Scroll Right */
	stRect = ONDK_Rect(NX_PRODETAIL_SINGLE_SCROLL_R_X + lSeries, NX_PRODETAIL_SINGLE_SCROLL_R_Y,
						NX_PRODETAIL_SINGLE_SCROLL_R_W, NX_PRODETAIL_SINGLE_SCROLL_R_H);
	ONDK_GWM_COM_Button_Create(NX_PRODETAIL_SINGLE_SCROLL_R_ID, &stRect, (HCHAR*)NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRODETAIL_SINGLE_SCROLL_R_ID, NX_PRISM_COMP_DrawButton_Scroll_ArrowRBox);

	nx_programDetail_SetActionBtnUiObject(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_Initialize(NxUiProDetailContent_t *pstContents, HUINT32 p1, HINT32 p2, HUINT32 lCallerUiType)
{
	HERROR			hError = ERR_FAIL;
	DxService_t		*pstService = NULL;
	HINT32			lEventId = p2;
	HUINT32			ulSvcUid = (HUINT32)p1, ulSchSlot = NX_CORE_SCHEDULE_SLOTID_NULL;
	NX_Channel_t	stCurChannel;
	NX_ChListGroup_t	eCurChlistGroup = eNxChListGroup_ALL;


	if (pstContents == NULL)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(NxUiProDetailContent_t));

	pstContents->eCallerUiType = lCallerUiType;
	if (pstContents->eCallerUiType == eNX_UCT_Schedule)
	{
		ulSchSlot = (HUINT32)p1;
		ulSvcUid = 0;
		hError = NX_SCHEDULER_GetSeriveUidBySlotID(ulSchSlot, &ulSvcUid);
		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!!! NX_SCHEDULER_GetSeriveUidBySlotID() - Slot ID:(0x%x) Fail !!\n", ulSchSlot);
		}

		pstContents->ulSchSlot = ulSchSlot;
	}

	pstContents->eDispMode = eNxPRODetail_DispMode_Single;
	pstContents->ulSvcUid = ulSvcUid;
	pstContents->lEventId = lEventId;
	pstContents->ulEventAttribute = 0;
	pstContents->lCurPage = 1;
	pstContents->lMaxPage = 1;
	pstContents->lSeriesItemNum = 0;
	pstContents->lSeriesCurIndex = 0;
	pstContents->stSeritemsList = NULL;
	pstContents->lPrevFocusObjId = -1;
	NX_APP_Info("ulSvcUid[0x%x], lEventId[0x%x]\n", ulSvcUid, lCallerUiType);

	eCurChlistGroup = NX_PRISM_LIVE_GetCurrentChGroupType();

	NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eCurChlistGroup, ulSvcUid, &stCurChannel);
	pstService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eCurChlistGroup, eNxFindSvcDir_Current, ulSvcUid);
	if (pstService == NULL)
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() Fail - ulSvcUid(0x%x), eCurChlistGroup(0x%x)!! \n", ulSvcUid, eCurChlistGroup);
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}
	else
	{
		pstContents->ulChannelNumber = stCurChannel.lcn;
		pstContents->stSvcInfo = pstService;
	}

	nx_programDetail_MakeDetailInfoByEvent(pstContents, TRUE);

	// Check Display Mode
	if (pstContents->lSeriesItemNum > 0)
	{
		pstContents->eDispMode = eNxPRODetail_DispMode_Series;
		pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_Series);
		pstContents->ulEventAttribute |= eNxPRODetail_Attr_Series;
	}

	nx_programDetail_MakeActionButtonMode(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MsgGwmCreate(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_programDetail_Initialize(pstContents, p1, p2, p3);
	nx_programDetail_SetUiObject(pstContents);
	nx_programDetail_MakeFMTData(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MsgGwmDestroy(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->stSeritemsList)
	{
		NX_APP_Free(pstContents->stSeritemsList);
		pstContents->stSeritemsList = NULL;
	}

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MsgGwmHapiMsg(NxUiProDetailContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	return	GWM_MESSAGE_PASS;
}


static ONDK_Result_t	nx_programDetail_MsgGwmKeyDown(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lObjId = -1;
	ONDK_GWM_GuiObject_t	gwmGuiObject = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_MENU:
		case KEY_GUIDE:
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_ARROWRIGHT:
			lObjId = GWM_GetFocusedObjectID();
			if (lObjId == NX_PRODETAIL_SERIES_LIST_ID)
			{
				if (pstContents->lPrevFocusObjId >= 0)
				{
					ONDK_GWM_Obj_SetFocus(pstContents->lPrevFocusObjId);
				}
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_ARROWLEFT:
			lObjId = GWM_GetFocusedObjectID();
			if ((lObjId != NX_PRODETAIL_SERIES_LIST_ID) && (lObjId != NX_PRODETAIL_SINGLE_SCROLL_R_ID))
			{
				gwmGuiObject = ONDK_GWM_Obj_GetObject(NX_PRODETAIL_SERIES_LIST_ID);
				if (gwmGuiObject != NULL)
				{
					ONDK_GWM_Obj_SetFocus(NX_PRODETAIL_SERIES_LIST_ID);
					pstContents->lPrevFocusObjId = lObjId;
				}
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_OK:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			if (pstContents->eCallerUiType == eNX_UCT_Schedule)
			{
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_RED:
		case KEY_GREEN:
		case KEY_YELLOW:
		case KEY_BLUE:
		case KEY_OPT:
		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
		case KEY_VFORMAT:
		case KEY_CH_PLUS:
		case KEY_CH_MINUS:
		case KEY_0:
		case KEY_1:
		case KEY_2:
		case KEY_3:
		case KEY_4:
		case KEY_5:
		case KEY_6:
		case KEY_7:
		case KEY_8:
		case KEY_9:
		case KEY_FRONT_TVRADIO:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_programDetail_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_RET_OK;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MsgGwmClick(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lObjId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	if (lObjId == NX_PRODETAIL_SINGLE_SCROLL_L_ID)
	{
		if (pstContents->lCurPage > 1)
		{
			pstContents->lCurPage--;
		}

		ONDK_GWM_FMT_ScrollUpPage(NX_PRODETAIL_SINGLE_DESC_TEXT_ID);
	}
	else if (lObjId == NX_PRODETAIL_SINGLE_SCROLL_R_ID)
	{
		if (pstContents->lCurPage < pstContents->lMaxPage)
		{
			pstContents->lCurPage++;
		}

		ONDK_GWM_FMT_ScrollDownPage(NX_PRODETAIL_SINGLE_DESC_TEXT_ID);
	}
	else
	{
		nx_programDetail_ActionButton(pstContents, p1, p2, p3);

		// Re-Create Button
		nx_programDetail_ReloadButtonMode(pstContents);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MsgGwmListChange(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lObjId = p1, lListIdx = (p2 + 1);

	if (lObjId == NX_PRODETAIL_SERIES_LIST_ID)
	{
		HxSTD_sprintf(pstContents->szSeriesPage, "%d / %d", lListIdx, pstContents->lSeriesItemNum);
		ONDK_GWM_COM_Text_SetText (NX_PRODETAIL_SERIES_LIST_PAGE_ID, (HCHAR*)pstContents->szSeriesPage);
		ONDK_GWM_APP_InvalidateObject(NX_PRODETAIL_SERIES_LIST_PAGE_ID);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MsgGwmListSelected(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lObjId = p1, lListIdx = p2;
	NxSeriesItem_t	eSeriesType = eNxSeries_None;

	if (lListIdx >= pstContents->lSeriesItemNum)
	{
		NX_APP_Error("Error!!! Invalid Index(%d), lSeriesItemNum(%d)\n", lListIdx, pstContents->lSeriesItemNum);
		return	ONDK_RET_FAIL;
	}

	if (lObjId == NX_PRODETAIL_SERIES_LIST_ID)
	{
		eSeriesType = pstContents->stSeritemsList[lListIdx].eSeriesType;
		switch (eSeriesType)
		{
			case eNxSeries_Record:
				// TODO: [JHLEE] Add Update code
				break;
			case eNxSeries_Schedule:
				// TODO: [JHLEE] Add Update code
				break;
			case eNxSeries_Event:
				pstContents->lSeriesCurIndex = lListIdx;
				ONDK_GWM_List_SetSelectedIndex(NX_PRODETAIL_SERIES_LIST_ID, pstContents->lSeriesCurIndex);

				// Re-make data
				pstContents->lEventId = pstContents->stSeritemsList[lListIdx].lEventId;
				nx_programDetail_MakeDetailInfoByEvent(pstContents, FALSE);

				ONDK_GWM_FMT_Delete(NX_PRODETAIL_SINGLE_DESC_TEXT_ID);
				nx_programDetail_MakeFMTData(pstContents);

				// Re-Create Button
				nx_programDetail_ReloadButtonMode(pstContents);
				ONDK_GWM_Obj_SetFocus(NX_PRODETAIL_SERIES_LIST_ID);
				break;
			default:
				NX_APP_Error("Error!!! Invalid item eSeriesType(%d)!!\n", eSeriesType);
				break;
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_programDetail_MsgCoreScheduleNotify(NxUiProDetailContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hError = ERR_FAIL;
	HUINT32			ulSlot = NX_CORE_SCHEDULE_SLOTID_NULL;
	DxSchedule_t	*pstSchedule = NULL;
	APK_META_SCH_MsgType_e	eMsgType = eAPK_METASCH_MSG_None;


	eMsgType = (APK_META_SCH_MsgType_e)p1;
	switch(eMsgType)
	{
		case eAPK_METASCH_MSG_ScheduleAdded:
			nx_programDetail_ProcessingMsgDestroy();
			ulSlot = (HUINT32)p2;
			pstContents->ulSchSlot = ulSlot;

			pstSchedule = NX_APP_Calloc(sizeof(DxSchedule_t));
			if (pstSchedule == NULL)
			{
				NX_APP_Error("ERROR!!! NX_APP_Calloc() Fail !!\n");
				break;
			}

			hError = NX_SCHEDULER_GetScheduleBySlotID(ulSlot, pstSchedule);
			if (hError != ERR_OK)
			{
				NX_APP_Error("ERROR!!! NX_SCHEDULER_GetScheduleBySlotID() - Slot ID:(0x%x) Fail !!\n", ulSlot);
			}

			switch(pstSchedule->eRsvType)
			{
				case DxRSVTYPE_WATCHING_EBR:
				case DxRSVTYPE_WATCHING_TBR:
					pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_Rsv_Watch);
					pstContents->ulSchSlot = pstSchedule->ulSlot;
					pstContents->ulEventAttribute |= eNxPRODetail_Attr_Rsv_Watch;
					break;

				case DxRSVTYPE_RECORD_EBR:
				case DxRSVTYPE_RECORD_TBR:
					pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_Rsv_Record);
					pstContents->ulSchSlot = pstSchedule->ulSlot;
					pstContents->ulEventAttribute |= eNxPRODetail_Attr_Rsv_Record;
					break;

				default:
					pstContents->ulSchSlot = NX_CORE_SCHEDULE_SLOTID_NULL;
					NX_APP_Error("ERROR!!! pstSchedule->eRsvType - (0x%x) Fail !!\n", pstSchedule->eRsvType);
					break;
			}

			// Re-Create Button
			nx_programDetail_ReloadButtonMode(pstContents);
			break;

		case eAPK_METASCH_MSG_ScheduleRemoved:
			nx_programDetail_ProcessingMsgDestroy();
			ulSlot = (HUINT32)p2;
			pstContents->ulSchSlot = NX_CORE_SCHEDULE_SLOTID_NULL;
			if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Rsv_Watch)
			{
				pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_Rsv_Watch);
			}
			else if (pstContents->ulEventAttribute & eNxPRODetail_Attr_Rsv_Record)
			{
				pstContents->ulEventAttribute &= ~(eNxPRODetail_Attr_Rsv_Record);
			}
			else
			{
				// Not to do
			}

			if (pstContents->ulAfterScheduleAction == eNxPRODetail_Attr_Rsv_Record)
			{
				pstContents->ulAfterScheduleAction = 0;
				nx_programDetail_ProcessingMsgCreate();
				nx_programDetail_ActionButtonByRecord(pstContents);
			}

			if (pstContents->ulAfterScheduleAction == eNxPRODetail_Attr_Rsv_Watch)
			{
				pstContents->ulAfterScheduleAction = 0;
				nx_programDetail_ProcessingMsgCreate();
				nx_programDetail_ActionButtonBySetReminder(pstContents);
			}

			// Re-Create Button
			nx_programDetail_ReloadButtonMode(pstContents);
			break;

		case eAPK_METASCH_MSG_ConflictScheduleAdded:
			nx_programDetail_ProcessingMsgDestroy();
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

	if (pstSchedule)
	{
		NX_APP_Free(pstSchedule);
		pstSchedule = NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	Nx_PRISM_ProgramDetail_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	NxUiProDetailContent_t	*pstContents = NULL;

	pstContents = nx_programDetail_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE\n");
			eResult = nx_programDetail_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			NX_APP_Message("MSG_GWM_KEYDOWN\n");
			eResult = nx_programDetail_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY\n");
			eResult = nx_programDetail_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_programDetail_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED\n");
			eResult = nx_programDetail_MsgGwmClick(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_LIST_CHANGED:
			NX_APP_Message("MSG_GWM_LIST_CHANGED\n");
			eResult = nx_programDetail_MsgGwmListChange(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_LIST_SELECTED:
			NX_APP_Message("MSG_GWM_LIST_SELECTED\n");
			eResult = nx_programDetail_MsgGwmListSelected(pstContents, p1, p2, p3);
			break;
		case MSG_CORE_SCHEDULE_NOTIFY:
			NX_APP_Message("MSG_CORE_SCHEDULE_NOTIFY\n");
			eResult = nx_programDetail_MsgCoreScheduleNotify(pstContents, p1, p2, p3);
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

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

