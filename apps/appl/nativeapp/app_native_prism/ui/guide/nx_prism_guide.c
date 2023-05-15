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
 * @file	  		nx_prism_guide.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_core_metadata_epg.h>
#include <nx_core_metadata_pf.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_GUIDE_ITEM_ID					eMenuFrame_Window_GUIDE

#define	NX_GUIDE_OBJECT_ID						(NX_PRISM_GUIDE_ITEM_ID)

#define NX_GUIDE_BACKGROUND_ID					NX_PRISM_GUIDE_ITEM_ID + 1
#define NX_GUIDE_CHANNELINFORM_ID				NX_GUIDE_BACKGROUND_ID + 1
#define NX_GUIDE_PROGRAMMEINFORM_ID				NX_GUIDE_CHANNELINFORM_ID + 1
#define NX_GUIDE_DISPLAY_STANDARD_ID			NX_GUIDE_PROGRAMMEINFORM_ID + 1
#define NX_GUIDE_DISPLAY_PLUS_ID				NX_GUIDE_DISPLAY_STANDARD_ID + 1
#define NX_GUIDE_DISPLAY_CHANGED_DATE_ID		NX_GUIDE_DISPLAY_PLUS_ID + 1

#define OG_GUIDE_COL_NUM						8		// customize

#define	NX_GUIDE_QUERY_TIMEOUT_ID				0xf010
#define	NX_GUIDE_QUERY_TIMEOUT					300

#define	NX_GUIDE_CHANGED_DATE_TIMEOUT_ID		0xf020
#define	NX_GUIDE_CHANGED_DATE_TIMEOUT 			5000

#define	NX_GUIDE_SAFETY_SEARCH_TERM_TIME		500

#define	NX_PRISM_GUIDE_OPTION_TEXT_ID			(NX_PRISM_GUIDE_ITEM_ID + 100)
#define	NX_PRISM_GUIDE_OPTION_TEXT_X			60
#define	NX_PRISM_GUIDE_OPTION_TEXT_Y			653
#define	NX_PRISM_GUIDE_OPTION_TEXT_W			400
#define	NX_PRISM_GUIDE_OPTION_TEXT_H			28
#define	NX_PRISM_GUIDE_OPTION_IMG_MG			10

#define	NX_PRISM_GUIDE_OPTION_FRAME_ID			(NX_PRISM_GUIDE_ITEM_ID + 101)
#define	NX_PRISM_GUIDE_OPTION_FRAME_X			0
#define	NX_PRISM_GUIDE_OPTION_FRAME_Y			630
#define	NX_PRISM_GUIDE_OPTION_FRAME_W			1280
#define	NX_PRISM_GUIDE_OPTION_FRAME_H			90

#define	NX_PRISM_GUIDE_OPTION_LIST_Y2			673
#define	NX_PRISM_GUIDE_OPTION_SELECTLIST_MAX	5

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
#define NX_PRISM_GUIDE_PROC_ANIMATION 			0xab230001
#define NX_PRISM_GUIDE_PROC_EXIT_ANIMATION	 	0xab230002
#endif


// UI 좌표

// time
#define		OG_FIRSTTIME_X						284										// customize
#define		OG_SECONDTIME_X						762										// customize
#define		OG_TIME_Y							66										// customize
#define		OG_TIME_W							90										// customize

// channel inform
#define		OG_CH_X								100										// customize
#define		OG_CH_Y								83										// customize
#define		OG_CH_ITEM_W						180										// customize
#define		OG_CH_ITEM_H						71										// customize
#define		OG_CH_LINE_H_MARGIN					2										// customize
#define		OG_CH_ITEM_AND_MARGIN_H				(OG_CH_ITEM_H + OG_CH_LINE_H_MARGIN)		// auto
#define		OG_CH_INFORM_H						(OG_CH_ITEM_AND_MARGIN_H * OG_GUIDE_COL_NUM)			// auto
#define		OG_CH_MOVE_BLT_H					(OG_CH_ITEM_AND_MARGIN_H * (OG_GUIDE_COL_NUM-1))		// auto
#define		OG_CH_LCN_X							10
#define		OG_CH_LCN_Y							26
#define		OG_CH_LCN_W							47
#define		OG_CH_NAME_X						62										// customize
#define		OG_CH_NAME_Y						11										// customize
#define		OG_CH_NAME_W						110										// customize
#define		OG_CH_LAST_PREV_Y					(OG_CH_ITEM_AND_MARGIN_H * (OG_GUIDE_COL_NUM-1))		// auto
#define		OG_CH_LAST_PREV_NUM					(OG_GUIDE_COL_NUM - 1)					// auto

// programme list
#define		OG_PRO_X							283										// customize
#define		OG_PRO_Y							82										// customize
#define		OG_PRO_ITEM_H						73										// customize
#define		OG_PRO_W							NX_PRISM_UI_BACKGROUND_BOX_WIDTH - OG_PRO_X
#define		OG_PRO_H							(OG_PRO_ITEM_H * OG_GUIDE_COL_NUM)
#define		OG_PRO_CELL_MARGIN					2
#define		OG_PRO_EVENT_NAME_X					13
#define		OG_PRO_EVENT_NAME_Y					(30 - ONDK_FONT_SIZE_22)
#define		OG_PRO_EVENT_TIME_X					13
#define		OG_PRO_EVENT_TIME_Y					(58 - ONDK_FONT_SIZE_18)
#define		OG_RSV_IMG_X						10
#define		OG_RSV_IMG_Y						9
#define		OG_RSV_IMG_WITH_NEXT_GAP			33

// display time
#define		OG_DISTIME_ST_X						284
#define		OG_DISTIME_PL_X						762
#define		OG_DISTIME_W						90
#define		OG_DISTIME_H						(ONDK_FONT_SIZE_24 + 8)
#define		OG_DISTIME_Y						66 - ONDK_FONT_SIZE_24
#define		OG_1HOUR_TO_MILLISECOND				3600

// display Changed Date
#define		OG_DIS_CHANGED_DATE_X				480
#define		OG_DIS_CHANGED_DATE_Y				300
#define		OG_DIS_CHANGED_DATE_W				400
#define		OG_DIS_CHANGED_DATE_H				100

// left cell dim
#define		OG_DIM_LCELL_X						280
#define		OG_DIM_LCELL_Y						83
#define		OG_DIM_LCELL_W						60
#define		OG_DIM_LCELL_H						634

// bottom dim
#define		OG_DIM_BOTTOM_X						0
#define		OG_DIM_BOTTOM_Y						630
#define		OG_DIM_BOTTOM_W						1280
#define		OG_DIM_BOTTOM_H						90

#define 	OG_W_PER_MIN						8
#define 	OG_2HOUR_SECOND						(60 * 60 * 2)
// draw 처리되는 총 minute 길이. (마지막 보정을 위해 + 1)
// ex) ((1280-283)/8) + 1 = 125 분. 마지막 1 분은 나머지값이 존재하는 경우, 보정을 위해..
//#define		OG_MIN_ALLITEM						(((NX_PRISM_UI_BACKGROUND_BOX_WIDTH - OG_PRO_X)/OG_W_PER_MIN) + 1)
#define		OG_MIN_ALLITEM						120
//#define		OG_W_ALLITEM						(OG_MIN_ALLITEM * OG_W_PER_MIN)
#define		OG_2HOUR_SECOND_ALLITEM				(OG_MIN_ALLITEM * 60)
#define		OG_W_ALLITEM						1000

// cell time line
#define		OG_PRO_CURSOR_INNER_LINE			1
#define		OG_PRO_CURSOR_DURATION_LINE			4

// vertical time line
#define		OG_TIME_LINE_SHADOW_W				1
#define		OG_TIME_LINE_W						2


// image resource
#define		OG_IMAGE_SURFACE_W					68		// resource 가 추가되면 조정해줘야함.
#define		OG_IMAGE_SURFACE_H					90
#define		OG_IMAGE_RES_SURFACE_W				67
#define		OG_IMAGE_RES_SURFACE_H				73
#define		OG_IMG_RES_GRID_IMG_Y				0
#define		OG_IMG_RES_GRID_IMG_H				73
#define		OG_IMG_RES_GRID_IMG_1L_X			0		// 3 X 73
#define		OG_IMG_RES_GRID_IMG_1L_W			3
#define		OG_IMG_RES_GRID_IMG_2C_X			3		// 1 X 73
#define		OG_IMG_RES_GRID_IMG_2C_W			1
#define		OG_IMG_RES_GRID_IMG_3R_X			4		// 3 X 73
#define		OG_IMG_RES_GRID_IMG_3R_W			3
#define		OG_IMG_RES_GRID_IMG_1L_3R_W			(OG_IMG_RES_GRID_IMG_1L_W + OG_IMG_RES_GRID_IMG_3R_W)
#define		OG_IMG_RES_DIM_LCELL_X				7
#define		OG_IMG_RES_DIM_LCELL_Y				0
#define		OG_IMG_RES_DIM_LCELL_W				60
#define		OG_IMG_RES_DIM_LCELL_H				1
#define		OG_IMG_RES_DIM_BOTTOM_X				76
#define		OG_IMG_RES_DIM_BOTTOM_Y				0
#define		OG_IMG_RES_DIM_BOTTOM_W				1
#define		OG_IMG_RES_DIM_BOTTOM_H				90
#define		OG_IMG_RES_RSV_WATCH_X				7
#define		OG_IMG_RES_RSV_WATCH_Y				1
#define		OG_IMG_RES_RSV_WATCH_W				26
#define		OG_IMG_RES_RSV_WATCH_H				26
#define		OG_IMG_RES_RSV_REC_X				33
#define		OG_IMG_RES_RSV_REC_Y				1
#define		OG_IMG_RES_RSV_REC_W				26
#define		OG_IMG_RES_RSV_REC_H				26

#define 	SECOND_REMOVE(var) ((var/60)*60)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum
{
	eGuideOptDepth_First,
	eGuideOptDepth_Second,
} Nx_GuideOptionDepth_e;

typedef enum
{
	eGuideOptItem_GroupChange,
	eGuideOptItem_Sort,
	eGuideOptItem_GenreSearch,
	eGuideOptItem_JumpToDate,
} Nx_GuideOptionCurItem_e;

typedef enum
{
	eNxGuideDir_NONE	= 0,
	eNxGuideDir_UP,
	eNxGuideDir_DOWN,
	eNxGuideDir_LEFT,
	eNxGuideDir_RIGHT,
	eNxGuideDir_PAGE_UP,
	eNxGuideDir_PAGE_DOWN
} Nx_GuideDir_e;

typedef enum
{
	eGuideFocusType_NONE,
	eGuideFocusType_FIRST,
	eGuideFocusType_LAST,
}NxGuideFocusingType_e;

typedef enum
{
	eGuideSearchType_Pf,
	eGuideSearchType_Prev,
	eGuideSearchType_Next,
	eGuideSearchType_Down,
	eGuideSearchType_Up,
	eGuideSearchType_PageDown,
	eGuideSearchType_PageUp,
	eGuideSearchType_ReQuery,		// search 기준 시간을 변경하지 않고 requery
}NxGuideSearchType_e;

typedef struct
{
	HCHAR	pszEventName[EPG_HUMAX_NAME_LEN];
	HUINT32	startTime;
	HUINT32 duration;
	HINT32	eventId;
	HINT32	svcUid;
	HCHAR	*pszDuration[NX_TEXT_SIZE_32];
	HBOOL	bFocus;
	HBOOL	bNoProgramme;
	HBOOL	bMidNoProgramme;
	DxRsvType_e	eRsvType;
}NxGuideData_t;

typedef struct tagGuideChannel {
	HUINT32	svcUid;
	HINT32	lcn;
	HINT32	programmeNum;
	HCHAR	*pszChannelName;
	NxGuideData_t *pstGuideData;

	HBOOL	bFocusUpdate;
	HBOOL	bPfFocus;
} Nx_GuideChannel_t;

typedef struct _Nx_GuideList_t
{
	NxGuideData_t	data;
	struct _Nx_GuideList_t	*next;
} Nx_GuideList_t;

typedef struct _Nx_GuideFocus_t
{
	HBOOL			bFocus;
	HINT32			nChannelListIndex;
	HINT32			nIndexArray;
	HINT32			eventId;
} Nx_GuideFocus_t;

typedef struct tagGuideContext {
	NX_ChListGroup_t	eCurGroupType;
	HINT32	nChannelNum;
	HINT32	nCurChannelIndex;				// channel list 에서 리스트의 channel 목록 유지 및 처리를 위해.
	NX_Channel_t *pstChannels;				// live proc 에서 얻어옴..

	HINT32	nSearchStartChannelIndex;		// search 기준이 되는 channel index(channel list 의 index 이다). up, down 시 가감됨.
	HINT32	nFocusChannelIndex;				// channel index 처리용(0 ~ OG_GUIDE_COL_NUM 안에서 움직임)
	NX_Channel_t stSearchStartChannel;

	HUINT32	unStandardStartTime;			// 최초 시작 time 저장.
	HUINT32	unSearchUnitTime;				// create 시 search 시작 시간을 기준으로 2시간 단위로 start time 저장.
	HUINT32	unSearchUnitPerHalfHourTime;	// unSearchUnitTime 에서 시간 단위로 게산된 시간, distplay time 에 사용.
	HUINT32	unDrawMaxTime;					// 처리되어야할 최대 시간으로 매번 새로 구해짐.(unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM)
	HUINT32 unQueryTime;					// 실재 query search 시에 사용되는 search 기준 time
	HUINT32	unCurrentTime;					// re draw 시 현재 시간을 항상 저장.

	NxGuideFocusingType_e eLastFocusType;
	Nx_GuideDir_e	eCurDirCom;
	Nx_GuideFocus_t	stFocusData;
	HxVector_t		*guideChannelsList;
	HBOOL	bIsReceivedResult;				// query 결과 event 를 받은건지..
	HBOOL	bChangedFocus;

	// time display
	HCHAR	szStandardDisTime[NX_TEXT_SIZE_32];
	HCHAR	szPlusDisTime[NX_TEXT_SIZE_32];
	HxDATETIME_t stSavedDateTime;
	HCHAR	szChangedDate[NX_TEXT_SIZE_32];

	HCHAR	*pszNoProgram;
	ONDKFontHandle		*eventNameFont, *durationFont;


	// Option Item
	HCHAR		szOptionText[NX_TEXT_SIZE_64];
	HUINT8		**szItemTextList;
	HUINT8		**szImageNItemTextList;
	HUINT8		**szImageCItemTextList;
	NX_Util_OPTIONList_t	stOptList;
	Nx_GuideOptionDepth_e	eOptCurDepth;
	Nx_GuideOptionCurItem_e	eOptCurItem;
	HBOOL				bChangedGroup;
	NX_ChListGroup_t	eChangedGroupType;
	eNxUiUtil_OptItem_t	eChangedSort;					// Sorting Item

	HINT32		bUiTop;

	// current time display		// common current time display
	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];

} Nx_GuideContext_t;

// related schedule
typedef struct tagGuideSchedItem
{
	DxRsvType_e		eRsvType;
	HINT32	nEvtId;
	DxExtReservation_u	uExtInfo;
}Nx_GuideSchedItem_t;

typedef struct tagSvcSchedule
{
	HUINT32			svcUid;
	HINT32 			nEventSchedNum;
	HxVector_t 		*eventSchedList;
} Nx_SvcSchedule_t;

typedef struct tagGuideSchedule
{
	HxVector_t		*schedList;
} Nx_GuideSchedule_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_GuideContext_t		s_stGuideContent;

static Nx_GuideSchedule_t		s_stGuideSchedule;

static HUINT32	s_ulGuideLastTick = 0;
static HUINT32	s_ulGuideCurTick = 0;

// channel
static ONDKSurfaceHandle 		s_hChInfoSurface 		= NULL;
static ONDKSurfaceHandle 		s_hTempChInfoSurface 	= NULL;

// programme
static ONDKSurfaceHandle		s_hProgrammeSurface[OG_GUIDE_COL_NUM];// 	= NULL;

// image resource
static ONDKSurfaceHandle		s_hImageResSurface		= NULL;

// option
static Nx_PrismListDialog_Content_t		s_stGuideOptContents;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_GuideContext_t	*nx_guide_GetContents(void);
static ONDK_Result_t	nx_guide_MsgGwmCreate(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_guide_MsgGwmDestroy(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_guide_MsgGwmKeyDown(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_guide_MsgGwmTimer(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_guide_UpdateCurrentTime(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_guide_DestroyListPopup(void);
static void				nx_guide_UpdateUiOptionMenu(Nx_GuideContext_t *pstContents);
static void				nx_guide_SetUiCreateOptionMenu(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_OptionChangedItemValue(Nx_GuideContext_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_guide_OptionInitData(Nx_GuideContext_t *pstContents);
static void				nx_guide_CreateListPopup(Nx_GuideContext_t *pstContents);
static void				nx_guide_SetJumpToDate(Nx_GuideContext_t *pstContents, HxDATETIME_t *pRcvJumpDateTime);
static ONDK_Result_t	nx_guide_CheckSvcChanged(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static void				nx_guide_OptionLoadGroupInitDataDefault(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_OptionChangeGroupInitData(Nx_GuideContext_t *pstContents);
static void				nx_guide_CreateOptionChangeGroupListPopup(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_OptionMakeListChangeGroup(Nx_GuideContext_t *pstContents);
static void				nx_guide_OptionLoadAlphabetGroupInitDataDefault(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_OptionChangeAlphabetGroupInitData(Nx_GuideContext_t *pstContents);
static void				nx_guide_CreateOptionChangeAlphabetGroupListPopup(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_OptionMakeListChangeAlphabetGroup(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_OptionExcuteSort(Nx_GuideContext_t *pstContents, eNxUiUtil_OptItem_t eOptItem);
static HUINT32			nx_guide_GetCurrentSort(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_OptionSortInitData(Nx_GuideContext_t *pstContents);
static void				nx_guide_CreateOptionSortListPopup(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_OptionMakeListSort(Nx_GuideContext_t *pstContents);

static ONDK_Result_t 	nx_guide_ReloadChannelListFromLive(Nx_GuideContext_t *pstContents);
static ONDK_Result_t 	nx_guide_ChangeGroup(Nx_GuideContext_t *pstContents, NX_ChListGroup_t eChagneGroup);
static void				nx_guide_SetReloadDataTimer(void);
static ONDK_Result_t	nx_guide_CleanFocus(Nx_GuideContext_t *pstContents);
static HINT32			nx_guide_NextFindSetFocus(Nx_GuideContext_t *pstContents);
static HINT32			nx_guide_PrevFindSetFocus(Nx_GuideContext_t *pstContents);
static HINT32			nx_guide_GetFocusIndex(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CellBackward(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CellForward(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CellPrev(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CellNext(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CellUp(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CellDown(Nx_GuideContext_t *pstContents);
static void				nx_guide_PageUp(Nx_GuideContext_t *pstContents);
static void				nx_guide_PageDown(Nx_GuideContext_t *pstContents);
static ONDK_Result_t 	nx_guide_MoveToDcnCh(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

// schedule
static Nx_GuideSchedule_t *nx_guide_GetScheduleContents(void);
static void 			nx_guide_ScheduleContentListDelete(void *item);
static void				nx_guide_ScheduleItem_Delete (void *item);
static void 			nx_guide_InitScheduleContent(void);
static void 			nx_guide_DestroyScheduleAll(void);
static void 			nx_guide_LoadScheduleAll(void);
static ONDK_Result_t 	nx_guide_RecvScheduleEvent(Nx_GuideContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

// draw
static void				nx_guide_draw_DrawBackground(void);
static ONDK_Result_t	nx_guide_draw_DrawItemChannel(Nx_GuideContext_t *pstContents, HINT32 channelIndex, ONDKSurfaceHandle surface, ONDK_Rect_t *rect);
static ONDK_Result_t	nx_guide_draw_CreateChInform(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_draw_UpdateDrawChInform(ONDKSurfaceHandle screen, ONDK_Region_t *area,
														ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_guide_draw_UpdateChInform(Nx_GuideContext_t *pstContents, Nx_GuideDir_e eDir);
static ONDK_Result_t	nx_guide_draw_DrawCellItem(Nx_GuideContext_t *pstContents, NxGuideData_t *pstGuideData, ONDKSurfaceHandle surface, HUINT32 cellW, ONDK_Rect_t drawRect);
static ONDK_Result_t	nx_guide_draw_DrawItemProgramme(Nx_GuideContext_t *pstContents, HINT32 channelIndex, HINT32 listIndex, ONDKSurfaceHandle surface, ONDK_Rect_t *rect);
static ONDK_Result_t	nx_guide_draw_CreateProgramme(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_draw_ReloadAllProgramme(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_draw_UpdateDrawProgrammeInform(ONDKSurfaceHandle screen, ONDK_Region_t *area,
														ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_guide_ResetTimeDisplay(Nx_GuideContext_t *pstContents);
static void				nx_guide_draw_DimLCell(Nx_GuideContext_t *pstContents, ONDKSurfaceHandle screen);
static void 			nx_guide_draw_DimBottom(Nx_GuideContext_t *pstContents, ONDKSurfaceHandle screen);
static ONDK_Result_t 	nx_guide_draw_CurrentVerticalLine(Nx_GuideContext_t *pstContents, ONDKSurfaceHandle screen);

// data
static void				nx_guide_SetChangedDateTimer(void);
static void	 			nx_guide_MakeChangedDate(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CalcUnitTimePerHalfHour(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_ReloadData(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, Nx_GuideDir_e eDir);
static ONDK_Result_t	nx_guide_SearchStartAll(Nx_GuideContext_t *pstContents, NxGuideSearchType_e eSearchType);
static NX_MetadataSearch_t* nx_guide_MakeMetadataSearch(Nx_GuideContext_t *pstContents);
static ONDK_Result_t 	nx_guide_SetFocus(Nx_GuideContext_t *pstContents);
static ONDK_Result_t 	nx_guide_SetFocusForcedLast(Nx_GuideContext_t *pstContents);
static void 	nx_guide_FillBlankGuideData(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel);
static void 	nx_guide_FillBlankGuideChannel(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel, HINT32 nChannelIndex);
static void		nx_guide_RemoveGuideChannelItem(Nx_GuideContext_t *pstContents, Nx_GuideDir_e eDir);
static HINT32	nx_guide_GetCollisionSize(Nx_GuideContext_t *pstContents, NxGuideData_t *pstCurItem, NxGuideData_t *pstCompareItem);
static HBOOL 	nx_guide_IsFirstItem(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel, NxGuideData_t *pstCurItem);
static HERROR 	nx_guide_GetFirstItemOfChannel(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel, HINT32 *pnFirstIndex);
static HBOOL 	nx_guide_IsLastItem(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel, NxGuideData_t *pstCurItem);
static HERROR 	nx_guide_GetLastItemOfChannel(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel, HINT32 *pnLastIndex);


// image resource
static void 	nx_guide_CreateImageResSurface(Nx_GuideContext_t *pstContext);
static void 	nx_guide_BltGridImg(ONDKSurfaceHandle dstSurface, ONDK_Rect_t dstRect);
static void 	nx_guide_BltRsvWatchImg(ONDKSurfaceHandle dstSurface, ONDK_Rect_t dstRect);
static void 	nx_guide_BltRsvRecImg(ONDKSurfaceHandle dstSurface, ONDK_Rect_t dstRect);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline Nx_GuideContext_t	*nx_guide_GetContents(void)
{
	return	&s_stGuideContent;
}

static ONDK_Result_t	nx_guide_MsgGwmCreate(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		*pszPanelStr = NULL;
	HINT32		i = 0;
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_GuideContext_t));
	nx_guide_InitScheduleContent();

	hError = NX_PRISM_LIVE_GetCurChannelIndex(&pstContents->nCurChannelIndex);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetCurChannelIndex() fail !!\n");
		return ONDK_RET_FAIL;
	}

	hError = NX_PRISM_LIVE_GetChannelListNum(&pstContents->nChannelNum);
	if (hError != ERR_OK || pstContents->nChannelNum <= 0)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetChannelListNum() fail !!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nChannelNum == 0)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetChannelListNum() fail --- channel num  0!!\n");
		return ONDK_RET_FAIL;
	}

	pstContents->pstChannels = NX_PRISM_LIVE_GetLiveChannels();
	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_LIVE_GetLiveChannels() is NULL!!! \n");
		return ONDK_RET_FAIL;
	}

	// save current group
	pstContents->eCurGroupType = NX_PRISM_LIVE_GetCurrentChGroupType();
#if 0
	hError = NX_CHANNEL_GetLastServiceGroup(&pstContents->eCurGroupType);
	if (hError != ERR_OK)
	{
		pstContents->eCurGroupType = eNxChListGroup_TV;
		NX_APP_Error("Error!! NX_CHANNEL_GetLastServiceGroup() Fail !!! \n");
	}
#endif

	if (pstContents->nChannelNum <= OG_GUIDE_COL_NUM)
		pstContents->nCurChannelIndex = 0;

	// load schedule list
	nx_guide_LoadScheduleAll();

///////////////////////////////////////////////////////////////////////
	pstContents->guideChannelsList = NULL;
	pstContents->nSearchStartChannelIndex = pstContents->nCurChannelIndex;
	pstContents->nFocusChannelIndex = 0;
	pstContents->bChangedFocus = FALSE;
	pstContents->pszNoProgram = (HCHAR*)" ";//ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID);

	if (pstContents->guideChannelsList == NULL)
	{
		pstContents->guideChannelsList = HLIB_VECTOR_NewEx(NULL, 0,
											(void *(*)(void *))NULL,
											(void *(*)(void *))HLIB_STD_MemFree_CB,
											NULL);

		for (i = 0; i < OG_GUIDE_COL_NUM; i++)
		{
			Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)NX_APP_Calloc(sizeof(Nx_GuideChannel_t));
			HLIB_VECTOR_Add(pstContents->guideChannelsList, pstGuideChannel);
		}
	}

	pstContents->eventNameFont 	= ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_22);
	pstContents->durationFont	= ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_18);

	nx_guide_CreateImageResSurface(pstContents);
	nx_guide_draw_DrawBackground();

	// standard display time
	stRect = ONDK_Rect(OG_DISTIME_ST_X, OG_DISTIME_Y, OG_DISTIME_W, OG_DISTIME_H);
	ONDK_GWM_COM_Text_Create(NX_GUIDE_DISPLAY_STANDARD_ID, &stRect, (HCHAR*)pstContents->szStandardDisTime);
	ONDK_GWM_COM_Text_SetFont(NX_GUIDE_DISPLAY_STANDARD_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_GUIDE_DISPLAY_STANDARD_ID, ONDK_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_GUIDE_DISPLAY_STANDARD_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_GUIDE_DISPLAY_STANDARD_ID, COL(C_T_Base01), COL(C_T_Bang_100));

	stRect = ONDK_Rect(OG_DISTIME_PL_X, OG_DISTIME_Y, OG_DISTIME_W, OG_DISTIME_H);
	ONDK_GWM_COM_Text_Create(NX_GUIDE_DISPLAY_PLUS_ID, &stRect, (HCHAR*)pstContents->szPlusDisTime);
	ONDK_GWM_COM_Text_SetFont(NX_GUIDE_DISPLAY_PLUS_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_GUIDE_DISPLAY_PLUS_ID, ONDK_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_GUIDE_DISPLAY_PLUS_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_GUIDE_DISPLAY_PLUS_ID, COL(C_T_Base01), COL(C_T_Bang_100));


	// create channel inform
	nx_guide_draw_CreateChInform(pstContents);
	stRect = ONDK_Rect(OG_CH_X, OG_CH_Y, OG_CH_ITEM_W, OG_CH_INFORM_H);
	ONDK_GWM_COM_Rect_Create(NX_GUIDE_CHANNELINFORM_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_GUIDE_CHANNELINFORM_ID, nx_guide_draw_UpdateDrawChInform);
	pstContents->eCurDirCom = eNxGuideDir_NONE;
	ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);

	// create programme info
	nx_guide_draw_CreateProgramme(pstContents);
	stRect = ONDK_Rect(OG_PRO_X, OG_PRO_Y, OG_PRO_W, OG_PRO_H);
	ONDK_GWM_COM_Rect_Create(NX_GUIDE_PROGRAMMEINFORM_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_GUIDE_PROGRAMMEINFORM_ID, nx_guide_draw_UpdateDrawProgrammeInform);

	if (pstContents->unStandardStartTime < NX_VALID_UNIXTIME)
	{
		if (NX_SYSTEM_GetTime(&pstContents->unStandardStartTime) != ERR_OK)
		{
			HxLOG_Error("time is Invalid!!!\n");
			return ONDK_RET_FAIL;
		}
		pstContents->unSearchUnitTime = pstContents->unStandardStartTime;
	}

	// create 에서, reload data 전에 먼저 calcunittime 해준후에 call.
	nx_guide_CalcUnitTimePerHalfHour(pstContents);
	nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, 0);

	if (nx_guide_SearchStartAll(pstContents, eGuideSearchType_ReQuery) != ONDK_RET_OK)
	{
		NX_APP_Error("Failed!! nx_guide_SearchStartAll() !!! \n");
		return ONDK_RET_FAIL;
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unSearchUnitPerHalfHourTime, &pstContents->stSavedDateTime);
	nx_guide_MakeChangedDate(pstContents);

	// create display changed date
	stRect = ONDK_Rect(OG_DIS_CHANGED_DATE_X, OG_DIS_CHANGED_DATE_Y, OG_DIS_CHANGED_DATE_W, OG_DIS_CHANGED_DATE_H);
	ONDK_GWM_COM_Text_Create(NX_GUIDE_DISPLAY_CHANGED_DATE_ID, &stRect, (HCHAR*)pstContents->szChangedDate);
	ONDK_GWM_COM_Text_SetFont(NX_GUIDE_DISPLAY_CHANGED_DATE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_GUIDE_DISPLAY_CHANGED_DATE_ID, ONDK_FONT_SIZE_64);
	ONDK_GWM_COM_Text_SetAlign(NX_GUIDE_DISPLAY_CHANGED_DATE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_GUIDE_DISPLAY_CHANGED_DATE_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
	ONDK_GWM_Obj_SetVisible(NX_GUIDE_DISPLAY_CHANGED_DATE_ID, FALSE);

	nx_guide_ResetTimeDisplay(pstContents);

	nx_guide_SetUiCreateOptionMenu(pstContents);

	// common time display
	nx_guide_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Guide);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Guide !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	{
		ONDK_GWM_O2Ani_Settings_t stAniSetting;
		ONDK_Rect_t			stRect;
		stRect = ONDK_Rect(0, 0, 1280, 720);
		ONDK_GWM_O2ANI_GetDefaultSettings((eONDK_O2ANITYPE_Proc | eONDK_O2ANITYPE_User), &stAniSetting, &stRect);
		stAniSetting.nStartAlpha = 0;
		stAniSetting.nEndAlpha = 255;
		ONDK_GWM_O2ANI_AddItem(NX_PRISM_GUIDE_PROC_ANIMATION, &stAniSetting, DEF_O2ANI_EFFECT_IGNORE_KEYPROCESS);
	}
#endif

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_MsgGwmDestroy(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 i = 0;

	nx_guide_DestroyScheduleAll();

	if (s_hChInfoSurface)
	{
		ONDK_SURFACE_Destroy(s_hChInfoSurface);
		s_hChInfoSurface = NULL;
	}

	if (s_hTempChInfoSurface)
	{
		ONDK_SURFACE_Destroy(s_hTempChInfoSurface);
		s_hTempChInfoSurface = NULL;
	}

	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		if (s_hProgrammeSurface[i])
		{
			ONDK_SURFACE_Destroy(s_hProgrammeSurface[i]);
			s_hProgrammeSurface[i] = NULL;
		}
	}

	if (s_hImageResSurface)
	{
		ONDK_SURFACE_Destroy(s_hImageResSurface);
		s_hImageResSurface = NULL;
	}

	if (pstContents->guideChannelsList)
	{
		// free allocated data
		for (i = 0; i < OG_GUIDE_COL_NUM; i++)
		{
			Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, i);
			if (pstGuideChannel->pstGuideData != NULL)
			{
				NX_APP_Free(pstGuideChannel->pstGuideData);
				pstGuideChannel->pstGuideData = NULL;
			}
			HxSTD_MemSet(pstGuideChannel, 0, sizeof(Nx_GuideChannel_t));

		}

		HLIB_VECTOR_Delete(pstContents->guideChannelsList);
		pstContents->guideChannelsList = NULL;
	}

	if(pstContents->eventNameFont)
		ONDK_FONT_Release(pstContents->eventNameFont);
	if(pstContents->durationFont)
		ONDK_FONT_Release(pstContents->durationFont);

	nx_guide_DestroyListPopup();

	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}
	if (pstContents->szImageNItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageNItemTextList);
		pstContents->szImageNItemTextList = NULL;
	}
	if (pstContents->szImageCItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageCItemTextList);
		pstContents->szImageCItemTextList = NULL;
	}
	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_MsgGwmKeyDown(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	pstContents->eCurDirCom = eNxGuideDir_NONE;


	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

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
			if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_DCN_Proc) ==NULL)
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_DCN_Proc", APP_BACKGROUND_OSD_PRIORITY,
										NX_PRISM_DCN_Proc, (HINT32)ONDK_GWM_APP_Get(0),
										pstContents->eCurGroupType, 0, lKeyId);
			}
			return ONDK_MESSAGE_BREAK;

		case KEY_OK:
		{
			NxGuideData_t *pstCurFocusItem = NULL;
			Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, pstContents->nFocusChannelIndex);


			if (pstGuideChannel->pstGuideData != NULL)
			{
				pstCurFocusItem = &pstGuideChannel->pstGuideData[pstContents->stFocusData.nIndexArray];
				if (pstCurFocusItem && pstCurFocusItem->eventId != -1)
				{
					HxLOG_Debug("exc ProgramDetail Proc channel svcuid : %d , event id : %d\n", pstCurFocusItem->svcUid, pstCurFocusItem->eventId);
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "Nx_PRISM_ProgramDetail_Proc", APP_DEFAULT_POPUP_PRIORITY, Nx_PRISM_ProgramDetail_Proc, 0, pstCurFocusItem->svcUid, pstCurFocusItem->eventId, eNX_UCT_Guide);
				}
			}
		}
			return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWLEFT:
			nx_guide_CellPrev(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			nx_guide_CellNext(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWUP:
			nx_guide_CellUp(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWDOWN:
			nx_guide_CellDown(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_CH_PLUS:
			nx_guide_PageUp(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_CH_MINUS:
			nx_guide_PageDown(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_SPECIAL_FASTFORWARD:
			nx_guide_CellForward(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_SPECIAL_REWIND:
			nx_guide_CellBackward(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_OPT:
			// TODO:  shows opiton for guide
			HxLOG_Debug("Option key on Guide \n");
			nx_guide_CreateListPopup(pstContents);
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_guide_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}
}

static ONDK_Result_t	nx_guide_MsgGwmTimer(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if ((p1 == NX_GUIDE_QUERY_TIMEOUT_ID) && (pstContents->bIsReceivedResult == FALSE))
	{
		NX_APP_Error("\n\n==========>>>>>  REQUERY EPG  <<<<===========\n\n",__FUNCTION__,__LINE__);
		nx_guide_SearchStartAll(pstContents, eGuideSearchType_ReQuery);
		ONDK_GWM_KillTimer(NX_GUIDE_QUERY_TIMEOUT_ID);
	}

	if (p1 == NX_GUIDE_CHANGED_DATE_TIMEOUT_ID)
	{
		ONDK_GWM_Obj_SetVisible(NX_GUIDE_DISPLAY_CHANGED_DATE_ID, FALSE);
		ONDK_GWM_KillTimer(NX_GUIDE_CHANGED_DATE_TIMEOUT_ID);
	}

	if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_guide_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);

		// 표준 time display 를 통해 시간이 업데이트 되면, 데이터도 모두 업데이트 해준다.
		{
			HUINT32	ulCurrentTime = 0;

			if (NX_SYSTEM_GetTime(&ulCurrentTime) != ERR_OK)
			{
				HxLOG_Error("time is Invalid!!!\n");
				return ONDK_RET_FAIL;
			}

			if ((ulCurrentTime >= pstContents->unSearchUnitPerHalfHourTime) &&
				(ulCurrentTime <= (pstContents->unSearchUnitPerHalfHourTime + (SECONDS_PER_HOUR * 2))))
			{
				pstContents->unSearchUnitTime = ulCurrentTime;

				// create 에서, reload data 전에 먼저 calcunittime 해준후에 call.
				nx_guide_CalcUnitTimePerHalfHour(pstContents);
				nx_guide_ResetTimeDisplay(pstContents);
				nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, 0);

				if (nx_guide_SearchStartAll(pstContents, eGuideSearchType_ReQuery) != ONDK_RET_OK)
				{
					NX_APP_Error("Failed!! nx_guide_SearchStartAll() !!! \n");
					return ONDK_RET_FAIL;
				}
			}
		}

	}
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return GWM_RET_OK;
}

static void		nx_guide_UpdateCurrentTime(Nx_GuideContext_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HCHAR 			*pszDayOfWeek;
	HUINT32 		ulUnixtime = 0, ulWeekIndex = 0;
	HxDATETIME_t 	stDatetime;

	hError = NX_SYSTEM_GetTime(&ulUnixtime);
	if (hError == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixtime, &stDatetime);
		ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stDatetime.stDate);
		pszDayOfWeek = (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex));

		HxSTD_sprintf((char*)pstContents->szDate, "%02d.%02d.%04d", stDatetime.stDate.ucDay, stDatetime.stDate.ucMonth,
										stDatetime.stDate.usYear);

		HxSTD_sprintf((char*)pstContents->szTime, "%s %02d:%02d", pszDayOfWeek, stDatetime.stTime.ucHour,
												stDatetime.stTime.ucMinute);
	}
	else
	{
		pstContents->szDate[0] = '\0';
		pstContents->szTime[0] = '\0';
	}
}

static ONDK_Result_t	nx_guide_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stGuideOptContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_guide_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_guide_DisplayListPopup);
}

static void		nx_guide_UpdateUiOptionMenu(Nx_GuideContext_t *pstContents)
{
	HUINT8		stGroupStr[NX_TEXT_SIZE_128] = {'\0',};
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;


	eCurGroup = pstContents->eCurGroupType;

	/* eNxChListGroup_AlphabetAll ~ eNxChListGroup_AlphabetEtc is included in the eNxChListGroup_ALPHABET Group */
	if ((eCurGroup >= eNxChListGroup_AlphabetAll) && (eCurGroup <= eNxChListGroup_AlphabetEtc))
	{
		eCurGroup = eNxChListGroup_ALPHABET;
	}

	NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(eCurGroup, (HCHAR*)stGroupStr);
	HxSTD_sprintf(pstContents->szOptionText, "%s - %s", (HCHAR*)ONDK_GetString(RES_OPTION_01_ID), (HCHAR*)stGroupStr);
	ONDK_GWM_COM_Multi_ChangeLeftStr(NX_PRISM_GUIDE_OPTION_TEXT_ID, pstContents->szOptionText);
}

static void		nx_guide_SetUiCreateOptionMenu(Nx_GuideContext_t *pstContents)
{
	ONDK_Rect_t		stRect;
	HUINT8			stGroupStr[NX_TEXT_SIZE_128] = {'\0',};

	/* Draw OPTION BG Frame */
	stRect = ONDK_Rect(NX_PRISM_GUIDE_OPTION_FRAME_X, NX_PRISM_GUIDE_OPTION_FRAME_Y,
						NX_PRISM_GUIDE_OPTION_FRAME_W, NX_PRISM_GUIDE_OPTION_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_GUIDE_OPTION_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_GUIDE_OPTION_FRAME_ID, NX_PRISM_COMP_DrawFrameBottomDim);

	stRect = ONDK_Rect(NX_PRISM_GUIDE_OPTION_TEXT_X, NX_PRISM_GUIDE_OPTION_TEXT_Y,
						NX_PRISM_GUIDE_OPTION_TEXT_W, NX_PRISM_GUIDE_OPTION_TEXT_H);

	NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(pstContents->eCurGroupType, stGroupStr);
	HxSTD_sprintf(pstContents->szOptionText, "%s - %s", (HCHAR*)ONDK_GetString(RES_OPTION_01_ID), (HCHAR*)stGroupStr);
	ONDK_GWM_COM_Multi_Create(NX_PRISM_GUIDE_OPTION_TEXT_ID, &stRect);
	ONDK_GWM_COM_Multi_SetContents(NX_PRISM_GUIDE_OPTION_TEXT_ID, (HUINT8*)RES_201_00_OPTION_PNG,
								NULL, (HUINT8*)pstContents->szOptionText, NULL, NX_PRISM_GUIDE_OPTION_IMG_MG);
	ONDK_GWM_COM_Multi_SetFontHandle(NX_PRISM_GUIDE_OPTION_TEXT_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Multi_SetFontColor(NX_PRISM_GUIDE_OPTION_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));
	ONDK_GWM_COM_Multi_SetFocusFlag(NX_PRISM_GUIDE_OPTION_TEXT_ID, FALSE);
}

static ONDK_Result_t	nx_guide_OptionChangedItemValue(Nx_GuideContext_t *pstContents, HUINT32 ulCurIdx)
{
	eNxUiUtil_OptItem_t		eOptItem = eNxUtil_OptItem_None;

	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	eOptItem = pstContents->stOptList.aeOptItemList[ulCurIdx];

	if (pstContents->eOptCurDepth == eGuideOptDepth_First)
	{
		switch (eOptItem)
		{
			case eNxUtil_OptItem_Group:
				pstContents->eOptCurDepth = eGuideOptDepth_Second;
				pstContents->eOptCurItem = eGuideOptItem_GroupChange;
				nx_guide_OptionMakeListChangeGroup(pstContents);
				break;
			case eNxUtil_OptItem_Sort:
				pstContents->eOptCurDepth = eGuideOptDepth_Second;
				pstContents->eOptCurItem  = eGuideOptItem_Sort;
				nx_guide_OptionMakeListSort(pstContents);
				break;
			case eNxUtil_OptItem_GenreSearch:
				HxLOG_Debug("exc Genre Search Proc \n");
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_GUIDE_GENRESEARCH_Proc", APP_DEFAULT_POPUP_PRIORITY,
									NX_PRISM_GUIDE_GENRESEARCH_Proc, 0, pstContents->unStandardStartTime, pstContents->nChannelNum, 0);
				break;
			case eNxUtil_OptItem_JumpDate:
				HxLOG_Debug("exc Jump to Date selection ui. param is search unit time\n");
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_GUIDE_JUMPTODATE_Proc", APP_DEFAULT_POPUP_PRIORITY,
									NX_PRISM_GUIDE_JUMPTODATE_Proc, 0, pstContents->unSearchUnitPerHalfHourTime, pstContents->unStandardStartTime, 0);
				break;
			case eNxUtil_OptItem_KeywordSearch:
				HxLOG_Debug("exc the Keyword Search\n");
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_SMARTSEARCH_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_KEYWORDSEARCH_Proc, HANDLE_NULL, 0, 0, 0);
				break;
			default:
				NX_APP_Info("Error!!! Invalid Option item:[%d]!!\n", eOptItem);
				break;
		}
	}
	else if (pstContents->eOptCurDepth == eGuideOptDepth_Second)
	{
		if (pstContents->eOptCurItem == eGuideOptItem_GroupChange)
		{
			NX_ChListGroup_t 		eChGroupType = (NX_ChListGroup_t)eOptItem;

			if (eChGroupType == eNxChListGroup_ALPHABET)
			{
				nx_guide_OptionMakeListChangeAlphabetGroup(pstContents);
				return ONDK_RET_OK;
			}

			switch (eChGroupType)
			{
				case eNxChListGroup_ALL:
				case eNxChListGroup_TV:
				case eNxChListGroup_RADIO:
				case eNxChListGroup_HDTV:
				case eNxChListGroup_FTA:
				case eNxChListGroup_CAS:
				case eNxChListGroup_AlphabetAll:
				case eNxChListGroup_AlphabetA:
				case eNxChListGroup_AlphabetB:
				case eNxChListGroup_AlphabetC:
				case eNxChListGroup_AlphabetD:
				case eNxChListGroup_AlphabetE:
				case eNxChListGroup_AlphabetF:
				case eNxChListGroup_AlphabetG:
				case eNxChListGroup_AlphabetH:
				case eNxChListGroup_AlphabetI:
				case eNxChListGroup_AlphabetJ:
				case eNxChListGroup_AlphabetK:
				case eNxChListGroup_AlphabetL:
				case eNxChListGroup_AlphabetM:
				case eNxChListGroup_AlphabetN:
				case eNxChListGroup_AlphabetO:
				case eNxChListGroup_AlphabetP:
				case eNxChListGroup_AlphabetQ:
				case eNxChListGroup_AlphabetR:
				case eNxChListGroup_AlphabetS:
				case eNxChListGroup_AlphabetT:
				case eNxChListGroup_AlphabetU:
				case eNxChListGroup_AlphabetV:
				case eNxChListGroup_AlphabetW:
				case eNxChListGroup_AlphabetX:
				case eNxChListGroup_AlphabetY:
				case eNxChListGroup_AlphabetZ:
				case eNxChListGroup_AlphabetDigit:
				case eNxChListGroup_AlphabetEtc:
				case eNxChListGroup_Favorite1:
				case eNxChListGroup_Favorite2:
				case eNxChListGroup_Favorite3:
				case eNxChListGroup_Favorite4:
				case eNxChListGroup_Favorite5:
					break;

				default:
					NX_APP_Info("Error!!! Not Support Group Item [0x%x] !!\n", eChGroupType);
					break;
			}

			nx_guide_OptionInitData(pstContents);
			nx_guide_ChangeGroup(pstContents, eChGroupType);
		}
		else if (pstContents->eOptCurItem == eGuideOptItem_Sort)
		{
			eNxUiUtil_OptItem_t		eOptItem = eNxUtil_OptItem_None;
			if (pstContents->stOptList.ulOptItemNum == 0)
			{
				NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
				return	ONDK_MESSAGE_BREAK;
			}
			eOptItem = pstContents->stOptList.aeOptItemList[ulCurIdx];
			switch (eOptItem)
			{
				case eNxUtil_OptItem_SortNumber:
					nx_guide_OptionExcuteSort(pstContents, eNxUtil_OptItem_SortNumber);
					break;
				case eNxUtil_OptItem_SortName:
					nx_guide_OptionExcuteSort(pstContents, eNxUtil_OptItem_SortName);
					break;
				default:
					NX_APP_Info("Error!!! Not Support Sort Item [%d] !!\n", eOptItem);
					break;
			}
			NX_COMMON_NOT_USED_PARAM(pstContents);
			return ONDK_RET_OK;
		}
	}

	return	ONDK_MESSAGE_BREAK;
}

static ONDK_Result_t	nx_guide_OptionInitData(Nx_GuideContext_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}
	if (pstContents->szImageNItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageNItemTextList);
		pstContents->szImageNItemTextList = NULL;
	}
	if (pstContents->szImageCItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageCItemTextList);
		pstContents->szImageCItemTextList = NULL;
	}

	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	pstContents->szItemTextList = NULL;
	pstContents->szImageNItemTextList = NULL;
	pstContents->szImageCItemTextList = NULL;

	pstContents->eOptCurDepth = eGuideOptDepth_First;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Group);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Sort);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_GenreSearch);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_KeywordSearch);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_JumpDate);
			break;
	}

	// TODO: Add Project config if you want to do

	return	ONDK_RET_OK;
}

static void		nx_guide_CreateListPopup(Nx_GuideContext_t *pstContents)
{
	HINT32		y = 0, h = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0;
	HUINT8		**pptextList = NULL, **ppImageListOfItem = NULL;
	ONDK_Rect_t	stRect;

	nx_guide_OptionInitData(pstContents);

	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	ulIndex	= 0;
	if (pstContents->szItemTextList == NULL)
	{
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
		pptextList = pstContents->szItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option item !!\n");
			return;
		}
	}
	else
	{
		pptextList = pstContents->szItemTextList;
	}

	ulAttribute = (eListDlg_OSD_USER|eListDlg_OSD_BG|eListDlg_IMAGE_OptionLApp);
	// Set Text List
	h = (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT * ulTextListNum) + (NX_PRISM_UI_OPT_POPUP_CELL_MG * (ulTextListNum-1)) + NX_PRISM_UI_OPT_POPUP_POINTER_H;
	y = NX_PRISM_GUIDE_OPTION_LIST_Y2 - h - NX_PRISM_UI_OPT_POPUP_POINTER_H;
	stRect = ONDK_Rect(NX_PRISM_UI_OPT_POPUP_LIST_L_X, y, NX_PRISM_UI_OPT_POPUP_LIST_WIDTH, h);
	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stGuideOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT + NX_PRISM_UI_OPT_POPUP_CELL_MG),
							ulAttribute, ulTextListNum, ulTextListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stGuideOptContents, NX_PRISM_FONT_SIZE_22);

	if (ulAttribute & eListDlg_OSD_USER)
	{
		stRect = ONDK_Rect(NX_PRISM_UI_OPT_POPUP_L_X, NX_PRISM_UI_OPT_POPUP_L_Y,
							NX_PRISM_UI_OPT_POPUP_WIDTH, NX_PRISM_UI_OPT_POPUP_HEIGHT);
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stGuideOptContents, &stRect);
	}

	// Set Normal Image List
	if (pstContents->szImageNItemTextList == NULL)
	{
		pstContents->szImageNItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_IMG_N, &pstContents->stOptList);
		ppImageListOfItem = pstContents->szImageNItemTextList;
		if (ppImageListOfItem == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option Normal Image item !!\n");
			return;
		}
	}
	else
	{
		ppImageListOfItem = pstContents->szImageNItemTextList;
	}

	NX_PRISM_COMP_ListDlg_SetImageList(&s_stGuideOptContents, (HUINT32)ppImageListOfItem);

	// Set Cursor Image List
	if (pstContents->szImageCItemTextList == NULL)
	{
		pstContents->szImageCItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_IMG_C, &pstContents->stOptList);
		ppImageListOfItem = pstContents->szImageCItemTextList;
		if (ppImageListOfItem == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option Cursor Image item !!\n");
			return;
		}
	}
	else
	{
		ppImageListOfItem = pstContents->szImageCItemTextList;
	}

	NX_PRISM_COMP_ListDlg_SetImageExtList(&s_stGuideOptContents, (HUINT32)ppImageListOfItem);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_guide_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_guide_DisplayListPopup, 0, 0, 0, 0);
}

#define ___________OPT_GROUP_CHANGE__________

static void		nx_guide_OptionLoadGroupInitDataDefault(Nx_GuideContext_t *pstContents)
{
	HUINT32			ulCount = 0;

	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_ALL);

	#if defined(CONFIG_OP_MBC)
	{
		ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite1);
		if (ulCount > 0)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite1);

		ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite2);
		if (ulCount > 0)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite2);

		ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite3);
		if (ulCount > 0)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite3);

		ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite4);
		if (ulCount > 0)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite4);

		ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite5);
		if (ulCount > 0)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite5);
	}
	#endif
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_TV);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_TV);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_RADIO);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_RADIO);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_HDTV);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_HDTV);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_FTA);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_FTA);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_CAS);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_CAS);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_ALPHABET);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_ALPHABET);

	#if !defined(CONFIG_OP_MBC)
	{
		ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite1);
		if (ulCount > 0)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite1);

		ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite2);
		if (ulCount > 0)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite2);

		ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite3);
		if (ulCount > 0)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite3);

		ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite4);
		if (ulCount > 0)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite4);

		ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite5);
		if (ulCount > 0)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite5);
	}
	#endif

}

static ONDK_Result_t	nx_guide_OptionChangeGroupInitData(Nx_GuideContext_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;


	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}
	if (pstContents->szImageNItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageNItemTextList);
		pstContents->szImageNItemTextList = NULL;
	}
	if (pstContents->szImageCItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageCItemTextList);
		pstContents->szImageCItemTextList = NULL;
	}

	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add Other Change Group for Operator if you need

		default:
			nx_guide_OptionLoadGroupInitDataDefault(pstContents);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}

static void		nx_guide_CreateOptionChangeGroupListPopup(Nx_GuideContext_t *pstContents)
{
	HINT32		lLineCnt = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, i = 0, ulMaxListNum = 0;
	HUINT8		**pptextList = NULL;
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;
	ONDK_Rect_t	stRect = { 0, 0, 0, 0 }, stListRect = { 0, 0, 0, 0 };


	if (pstContents->stOptList.ulOptItemNum == 0 )
	{
		NX_APP_Error("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	if (pstContents->szItemTextList == NULL)
	{
		/* Make Option Test List */
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeChannlGroupItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
		pptextList = pstContents->szItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Error("Error!!! Please Check added Option item !!\n");
			return;
		}
	}
	else
	{
		pptextList = pstContents->szItemTextList;
	}

	/* Find Current group index for List Index */
	ulIndex	= 0;
	eCurGroup = pstContents->eCurGroupType;

	// eNxChListGroup_AlphabetAll ~ eNxChListGroup_AlphabetEtc is included in the eNxChListGroup_ALPHABET Group
	if ((eCurGroup >= eNxChListGroup_AlphabetAll) && (eCurGroup <= eNxChListGroup_AlphabetEtc))
	{
		eCurGroup = eNxChListGroup_ALPHABET;
	}

	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == (HUINT32)eCurGroup)
		{
			ulIndex = i;
			break;
		}
	}

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, ONDK_GetString(RES_MESG_5650_ID), NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_W);

	// Set Text List
	ulMaxListNum = (ulTextListNum > NX_PRISM_GUIDE_OPTION_SELECTLIST_MAX) ? NX_PRISM_GUIDE_OPTION_SELECTLIST_MAX : ulTextListNum;
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_SelectItemApp);
	stRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_List, lLineCnt, ulMaxListNum);
	if (ulAttribute & eListDlg_OSD_USER)
	{
		stListRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_OSD, lLineCnt, ulMaxListNum);
		stRect.x = stListRect.x + stRect.x;
		stRect.y = stListRect.y + stRect.y;
	}

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stGuideOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulMaxListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stGuideOptContents, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stGuideOptContents, ONDK_GetString(RES_MESG_5650_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stGuideOptContents, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_guide_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_guide_DisplayListPopup, 0, 0, 0, 0);

	//ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXIT_TIMER_ID);
}

static ONDK_Result_t	nx_guide_OptionMakeListChangeGroup(Nx_GuideContext_t *pstContents)
{
	nx_guide_OptionChangeGroupInitData(pstContents);
	nx_guide_CreateOptionChangeGroupListPopup(pstContents);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}

#define ___________OPT_ALPHABET_GROUP_CHANGE__________

static void		nx_guide_OptionLoadAlphabetGroupInitDataDefault(Nx_GuideContext_t *pstContents)
{
	HUINT32			ulCount = 0;

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetAll);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetAll);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetA);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetA);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetB);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetB);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetC);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetC);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetD);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetD);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetE);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetE);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetF);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetF);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetG);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetG);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetH);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetH);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetI);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetI);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetJ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetJ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetK);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetK);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetL);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetL);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetM);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetM);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetN);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetN);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetO);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetO);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetP);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetP);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetQ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetQ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetR);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetR);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetS);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetS);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetT);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetT);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetU);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetU);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetV);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetV);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetW);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetW);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetX);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetX);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetY);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetY);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetZ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetZ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetDigit);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetDigit);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetEtc);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetEtc);

}

static ONDK_Result_t	nx_guide_OptionChangeAlphabetGroupInitData(Nx_GuideContext_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}
	if (pstContents->szImageNItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageNItemTextList);
		pstContents->szImageNItemTextList = NULL;
	}
	if (pstContents->szImageCItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageCItemTextList);
		pstContents->szImageCItemTextList = NULL;
	}

	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add Other Change Group for Operator if you need

		default:
			nx_guide_OptionLoadAlphabetGroupInitDataDefault(pstContents);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}

static void		nx_guide_CreateOptionChangeAlphabetGroupListPopup(Nx_GuideContext_t *pstContents)
{
	HINT32		lLineCnt = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, i = 0, ulMaxListNum = 0;
	HUINT8		**pptextList = NULL;
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;
	ONDK_Rect_t	stRect = { 0, 0, 0, 0 }, stListRect = { 0, 0, 0, 0 };


	if (pstContents->stOptList.ulOptItemNum == 0 )
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	if (pstContents->szItemTextList == NULL)
	{
		/* Make Option Test List */
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeChannlGroupItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
		pptextList = pstContents->szItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option item !!\n");
			return;
		}
	}
	else
	{
		pptextList = pstContents->szItemTextList;
	}

	/* Find Current group index for List Index */
	ulIndex	= 0;
	eCurGroup = pstContents->eCurGroupType;

	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == (HUINT32)eCurGroup)
		{
			ulIndex = i;
			break;
		}
	}

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, ONDK_GetString(RES_MESG_5650_ID), NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_W);

	// Set Text List
	ulMaxListNum = (ulTextListNum > NX_PRISM_GUIDE_OPTION_SELECTLIST_MAX) ? NX_PRISM_GUIDE_OPTION_SELECTLIST_MAX : ulTextListNum;
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_SelectItemApp);
	stRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_List, lLineCnt, ulMaxListNum);
	if (ulAttribute & eListDlg_OSD_USER)
	{
		stListRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_OSD, lLineCnt, ulMaxListNum);
		stRect.x = stListRect.x + stRect.x;
		stRect.y = stListRect.y + stRect.y;
	}

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stGuideOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulMaxListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stGuideOptContents, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stGuideOptContents, ONDK_GetString(RES_MESG_5650_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stGuideOptContents, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_guide_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_guide_DisplayListPopup, 0, 0, 0, 0);

	//ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXIT_TIMER_ID);
}

static ONDK_Result_t	nx_guide_OptionMakeListChangeAlphabetGroup(Nx_GuideContext_t *pstContents)
{
	nx_guide_OptionChangeAlphabetGroupInitData(pstContents);
	nx_guide_CreateOptionChangeAlphabetGroupListPopup(pstContents);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}

#define ___________OPT_SORT_OPTION_______________
static ONDK_Result_t	nx_guide_OptionExcuteSort(Nx_GuideContext_t *pstContents, eNxUiUtil_OptItem_t eOptionItem)
{
	HINT32			i = 0, lSortChNum = 0;
	HBOOL			bFindEqualChannel = FALSE;
	HxList_t		*pSortList = NULL;
	NX_Channel_t	*pstCurChannel = NULL, *pstFindChannel = NULL,  *pstSortChLists = NULL;
	NX_Channel_t	stLastChannel;
	eNxUiUtil_OptItem_t		eOptItem = eOptionItem;

	switch (eOptItem)
	{
		case eNxUtil_OptItem_SortNumber:
			{
				if (pstContents->pstChannels == NULL)
				{
					NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
					return ONDK_RET_FAIL;
				}

				NX_CHANNEL_GROUP_SortNxChannelsByLcn(pstContents->pstChannels, pstContents->nChannelNum);

				HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
				HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurChannelIndex], sizeof(NX_Channel_t));

				bFindEqualChannel = FALSE;
				for (i = 0; i < pstContents->nChannelNum; i++)
				{
					pstFindChannel = &pstContents->pstChannels[i];

					if (pstFindChannel	&&
						(pstFindChannel->svcUid == stLastChannel.svcUid )&&
						(pstFindChannel->lcn	== stLastChannel.lcn))
					{
						bFindEqualChannel = TRUE;
						pstContents->nCurChannelIndex = i;
						break;
					}
				}

				if (bFindEqualChannel == FALSE)
					pstContents->nCurChannelIndex = 0;

				// update ui
				pstCurChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
				if (pstCurChannel != NULL)
				{
					pstContents->eChangedSort = eNxUtil_OptItem_SortNumber;
					ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);
					nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, 0);
					nx_guide_SearchStartAll(pstContents, eGuideSearchType_ReQuery);
				}
			}
			break;

		case eNxUtil_OptItem_SortName:
			{
				if (pstContents->pstChannels == NULL)
				{
					NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
					return ONDK_RET_FAIL;
				}

				pSortList = NX_CHANNEL_GROUP_SortNxChannelsByName(pstContents->pstChannels, pstContents->nChannelNum);
				if (pSortList == NULL)
				{
					break;
				}

				NX_CHANNEL_GROUP_MakeNxChannelListByList(pSortList,  &pstSortChLists, &lSortChNum);

				// sort list 제거
				HLIB_LIST_RemoveAll(pSortList);

				HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
				HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurChannelIndex], sizeof(NX_Channel_t));

				// bChangedGroup 가 true 이면, channel list mem free
				if (pstContents->bChangedGroup && pstContents->pstChannels != NULL)
				{
					// bChangedGroup 가 true 이면, pstContents->pstChannels 는 재할당 받은것이다..
					NX_APP_Free(pstContents->pstChannels);
					pstContents->pstChannels = NULL;
				}

				// change channel list in prism channel list context
				pstContents->pstChannels = pstSortChLists;
				pstContents->nChannelNum = lSortChNum;

				bFindEqualChannel = FALSE;
				for (i = 0; i < lSortChNum; i++)
				{
					pstFindChannel = &pstContents->pstChannels[i];

					if (pstFindChannel	&&
						(pstFindChannel->svcUid == stLastChannel.svcUid )&&
						(pstFindChannel->lcn	== stLastChannel.lcn))
					{
						bFindEqualChannel = TRUE;
						pstContents->nCurChannelIndex = i;
						break;
					}
				}

				if (bFindEqualChannel == FALSE)
					pstContents->nCurChannelIndex = 0;

				// update ui
				pstCurChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
				if (pstCurChannel != NULL)
				{
					pstContents->eChangedSort = eNxUtil_OptItem_SortName;
					ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);
					nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, 0);
					nx_guide_SearchStartAll(pstContents, eGuideSearchType_ReQuery);
				}
			}
			break;
		default:
			NX_APP_Info("Error!!! Not Support Sort Item [%d] !!\n", eOptItem);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}

static HUINT32		nx_guide_GetCurrentSort(Nx_GuideContext_t *pstContents)
{
	if (pstContents)
	{
		return	(HUINT32)pstContents->eChangedSort;
	}
	else
	{
		return	(HUINT32)eNxUtil_OptItem_None;
	}
}

static ONDK_Result_t	nx_guide_OptionSortInitData(Nx_GuideContext_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;


	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}
	if (pstContents->szImageNItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageNItemTextList);
		pstContents->szImageNItemTextList = NULL;
	}
	if (pstContents->szImageCItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageCItemTextList);
		pstContents->szImageCItemTextList = NULL;
	}

	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SortNumber);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SortName);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}

static void		nx_guide_CreateOptionSortListPopup(Nx_GuideContext_t *pstContents)
{
	HINT32		lLineCnt = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, ulCurSort = 0, i = 0;
	HUINT8		**pptextList = NULL;
	ONDK_Rect_t	stRect = { 0, 0, 0, 0 }, stListRect = { 0, 0, 0, 0 };


	if (pstContents->stOptList.ulOptItemNum == 0 )
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	ulIndex	= 0;
	if (pstContents->szItemTextList == NULL)
	{
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
		pptextList = pstContents->szItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option item !!\n");
			return;
		}
	}
	else
	{
		pptextList = pstContents->szItemTextList;
	}

	ulCurSort = nx_guide_GetCurrentSort(pstContents);

	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == ulCurSort)
		{
			ulIndex = i;
			break;
		}
	}

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, ONDK_GetString(RES_MESG_5651_ID), NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_W);
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_SelectItemApp);

	// Set Text List
	stRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_List, lLineCnt, ulTextListNum);
	if (ulAttribute & eListDlg_OSD_USER)
	{
		stListRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_OSD, lLineCnt, ulTextListNum);
		stRect.x = stListRect.x + stRect.x;
		stRect.y = stListRect.y + stRect.y;
	}

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stGuideOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulTextListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stGuideOptContents, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stGuideOptContents, ONDK_GetString(RES_MESG_5651_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stGuideOptContents, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_channellistOpt_DisplaySortListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_guide_DisplayListPopup, 0, 0, 0, 0);

	//ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXIT_TIMER_ID);
}


static ONDK_Result_t	nx_guide_OptionMakeListSort(Nx_GuideContext_t *pstContents)
{
	nx_guide_OptionSortInitData(pstContents);
	nx_guide_CreateOptionSortListPopup(pstContents);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


#define ___________RELATED_GUIDE_CONTROL_____________
static ONDK_Result_t nx_guide_ReloadChannelListFromLive(Nx_GuideContext_t *pstContents)
{
	HERROR hError = ERR_FAIL;

	hError = NX_PRISM_LIVE_GetCurChannelIndex(&pstContents->nCurChannelIndex);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetCurChannelIndex() fail !!\n");
		return ONDK_RET_FAIL;
	}

	hError = NX_PRISM_LIVE_GetChannelListNum(&pstContents->nChannelNum);
	if (hError != ERR_OK || pstContents->nChannelNum <= 0)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetChannelListNum() fail !!\n");
		return ONDK_RET_FAIL;
	}

	pstContents->pstChannels = NX_PRISM_LIVE_GetLiveChannels();
	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_LIVE_GetLiveChannels() is NULL!!! \n");
		return ONDK_RET_FAIL;
	}
	// save current group
	hError = NX_CHANNEL_GetLastServiceGroup(&pstContents->eCurGroupType);
	if (hError != ERR_FAIL)
	{
		pstContents->eCurGroupType = eNxChListGroup_TV;
		NX_APP_Error("Error!! NX_CHANNEL_GetLastServiceGroup() Fail !!! \n");
	}

	if (pstContents->nChannelNum <= OG_GUIDE_COL_NUM)
		pstContents->nCurChannelIndex = 0;

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_guide_ChangeGroup(Nx_GuideContext_t *pstContents, NX_ChListGroup_t eChagneGroup)
{
	HBOOL		bFindEqualChannel = FALSE;
	HUINT32		ulExtraFilter = 0;
	HINT32		lGroupNum = 0, i = 0;
	NX_Channel_t	*pstGroupList = NULL, *pstCurChannel = NULL, *pstFindChannel = NULL;
	NX_Channel_t	stLastChannel;

	if (pstContents->eCurGroupType == eChagneGroup)
	{
		HxLOG_Error("\n\n chage group is equal with old group!~!!!\n\n");
		return ONDK_RET_OK;
	}

	if(eChagneGroup == eNxChListGroup_ALL)
	{
		NX_CHANNEL_GROUP_MakeNxChannelListAll(&pstGroupList, &lGroupNum);
	}else
	{
		ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eChagneGroup);

		// make group list
		if (ERR_OK != NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstGroupList, &lGroupNum, eChagneGroup, ulExtraFilter	/* DxAlpabetGruopType_b .. etc*/))
		{
			HxLOG_Error("\nFailed Make Group list! (eChGroupType : %d)\n", eChagneGroup);
			return ONDK_RET_FAIL;
		}
	}

	if (lGroupNum <= 0)
	{
		NX_APP_Error("Error!!! eChGroupType : (0x%x)(lGroupNum <= 0) !!\n", eChagneGroup);
		if(pstGroupList)
		{
			NX_APP_Free(pstGroupList);
			pstGroupList = NULL;
		}
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
	HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurChannelIndex], sizeof(NX_Channel_t));

	if (pstContents->bChangedGroup && pstContents->pstChannels != NULL)
	{
		NX_APP_Free(pstContents->pstChannels);
		pstContents->pstChannels = NULL;
	}

	// change channel list in prism channel list context
	pstContents->pstChannels = pstGroupList;
	pstContents->nChannelNum = lGroupNum;

	// find last service in new group channel list.
	bFindEqualChannel = FALSE;
	for (i = 0; i < lGroupNum; i++)
	{
		pstFindChannel = &pstContents->pstChannels[i];

		if (pstFindChannel	&&
			(pstFindChannel->svcUid == stLastChannel.svcUid) &&
			(pstFindChannel->lcn == stLastChannel.lcn))
		{
			bFindEqualChannel = TRUE;
			pstContents->nCurChannelIndex = i;
			break;
		}
	}
	if (bFindEqualChannel == FALSE)
		pstContents->nCurChannelIndex = 0;

	pstContents->bChangedGroup = TRUE;
	pstContents->eChangedGroupType = eChagneGroup;
	pstContents->eCurGroupType = eChagneGroup;

	nx_guide_UpdateUiOptionMenu(pstContents);

	// update ui
	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	// guide list max num 보다 작은 경우 처리..
	if (pstContents->nChannelNum <= OG_GUIDE_COL_NUM)
		pstContents->nCurChannelIndex = 0;

	pstContents->nSearchStartChannelIndex = pstContents->nCurChannelIndex;

	pstCurChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
	if (pstCurChannel != NULL)
	{
		ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);
		nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, 0);

		nx_guide_SearchStartAll(pstContents, eGuideSearchType_ReQuery);
	}
	else
	{
		NX_APP_Error("Error!!! pstCurChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}


	return ONDK_RET_OK;
}

static void	nx_guide_SetJumpToDate(Nx_GuideContext_t *pstContents, HxDATETIME_t *pRcvJumpDateTime)
{
	ONDK_GWM_App_t	pGwmApp = NULL;
	HINT32	nCompareDate = 0;
	HxDATETIME_t *pstRcvDateTime = pRcvJumpDateTime;
	HxDATETIME_t standardDateTime, searchUnitDateTime;

	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unStandardStartTime, &standardDateTime);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unSearchUnitTime, &searchUnitDateTime);

	nCompareDate = HLIB_DATETIME_CompareDate(pstRcvDateTime->stDate, standardDateTime.stDate);

	if(nCompareDate <= 0)	// 선택한 date 가 standard search 보다 작거나 같으면 ..
	{
		pstContents->unSearchUnitTime = pstContents->unStandardStartTime;
	}
	else
	{
		nCompareDate = HLIB_DATETIME_CompareDate(pstRcvDateTime->stDate, searchUnitDateTime.stDate);
		// jump to date 만큼의 시간을 가감해서 unti search time 에 set 하고 research 하면 된다.
		pstContents->unSearchUnitTime += (SECONDS_PER_HOUR * 24) * nCompareDate;
	}
	nx_guide_CalcUnitTimePerHalfHour(pstContents);
	nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, 0);
	nx_guide_ResetTimeDisplay(pstContents);
	nx_guide_SearchStartAll(pstContents, eGuideSearchType_ReQuery);

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_GUIDE_JUMPTODATE_Proc);
	if(pGwmApp)
		ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_GUIDE_JUMPTODATE_Proc);
}

static ONDK_Result_t	nx_guide_CheckSvcChanged(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{

	NX_Channel_t *pstCurChangedChannel = NULL;
	NX_Channel_t *pstCurGuideChannel = NULL;
	NX_Channel_t *pstOldChannel = NULL;
	HINT32		i = 0;
	HINT32		nOldStartIndex = -1 ,nNewStartIndex = -1;

	pstCurChangedChannel = NX_PRISM_LIVE_GetMasterChannel();
	if (pstCurChangedChannel == NULL)
	{
		HxLOG_Debug("Changed Channel is NULL!\n");
		return ONDK_RET_OK;
	}
	pstCurGuideChannel = &pstContents->pstChannels[pstContents->nCurChannelIndex];
	if (pstCurGuideChannel->svcUid == pstCurChangedChannel->svcUid)
	{
		HxLOG_Debug("Not Changed Channel!\n");
		return ONDK_RET_OK;
	}

	//nx_guide_CleanFocus(pstContents);

	nOldStartIndex = pstContents->nSearchStartChannelIndex;

	nx_guide_ReloadChannelListFromLive(pstContents);

	pstOldChannel = &pstContents->stSearchStartChannel;
	for (i = 0; i < pstContents->nChannelNum; i++)
	{
		NX_Channel_t *pstTempChannel = &pstContents->pstChannels[i];

		if (pstTempChannel->svcUid == pstOldChannel->svcUid)
		{
			nNewStartIndex = i;
			break;
		}
	}

	if (nNewStartIndex != -1)
	{
		if (nOldStartIndex != nNewStartIndex)
		{
			pstContents->nSearchStartChannelIndex = nNewStartIndex;
		}
		else
		{
			HINT32 nTempMaxIndex = nOldStartIndex + OG_GUIDE_COL_NUM;

			if (nTempMaxIndex >= pstContents->nChannelNum)
			{
				nTempMaxIndex = (nTempMaxIndex - pstContents->nChannelNum);
			}

			if ((pstContents->nCurChannelIndex >= nOldStartIndex)
				&&(pstContents->nCurChannelIndex < nTempMaxIndex))
			{
				pstContents->nSearchStartChannelIndex = nOldStartIndex;
				ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);
			}
			else
			{
				pstContents->nSearchStartChannelIndex = pstContents->nCurChannelIndex;
				pstContents->nFocusChannelIndex = 0;
				ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);
				nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, 0);
				nx_guide_SearchStartAll(pstContents, eGuideSearchType_ReQuery);
			}
		}
		return ONDK_RET_OK;
	}
	else
	{
		HxLOG_Debug("Not Found Old Channel.\n");
		return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}

static void	nx_guide_SetReloadDataTimer(void)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;
	retTimer = ONDK_GWM_SetTimer(NX_GUIDE_QUERY_TIMEOUT_ID, NX_GUIDE_QUERY_TIMEOUT);

	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_GUIDE_QUERY_TIMEOUT_ID);
	}
}

static ONDK_Result_t	nx_guide_CleanFocus(Nx_GuideContext_t *pstContents)
{
	HINT32 i = 0;
	Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, pstContents->nFocusChannelIndex);

	HxSTD_MemSet(&pstContents->stFocusData, 0, sizeof(Nx_GuideFocus_t));

	for (i = 0; i < pstGuideChannel->programmeNum;i++)
	{
		if (&pstGuideChannel->pstGuideData[i] && (pstGuideChannel->pstGuideData[i].bFocus == TRUE))
		{
			pstGuideChannel->pstGuideData[i].bFocus = FALSE;
			return ONDK_RET_OK;
		}
	}


	return ONDK_RET_FAIL;
}

static HINT32	nx_guide_NextFindSetFocus(Nx_GuideContext_t *pstContents)
{
	HINT32 i = 0, nFocusIndex = 0, nTmpFocusIndex = 0;
	HBOOL	bFoundEventId = FALSE;
	Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, pstContents->nFocusChannelIndex);

	for (i = 0; i < pstGuideChannel->programmeNum; i++)
	{
		if (&pstGuideChannel->pstGuideData[i])
		{
			if (pstGuideChannel->pstGuideData[i].bFocus == TRUE)
				nTmpFocusIndex = i;

			if ((&pstGuideChannel->pstGuideData[i+1]) && (pstGuideChannel->pstGuideData[i].bFocus == TRUE)
				&& (pstGuideChannel->pstGuideData[i].eventId == pstContents->stFocusData.eventId))
			{
				nFocusIndex = i+1;
				bFoundEventId = TRUE;
				pstContents->stFocusData.bFocus = TRUE;
				pstContents->stFocusData.nIndexArray = nFocusIndex;
				pstContents->stFocusData.eventId = pstGuideChannel->pstGuideData[nFocusIndex].eventId;
				pstGuideChannel->pstGuideData[nFocusIndex].bFocus = TRUE;
				break;
			}
		}
	}

	if (bFoundEventId == FALSE)
	{
		for (i = 0; i < pstGuideChannel->programmeNum; i++)
		{
			NxGuideData_t *pstItem = &pstGuideChannel->pstGuideData[i];

			if ((pstItem->startTime < (pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM))
				&&((pstItem->startTime + pstItem->duration) > pstContents->unSearchUnitPerHalfHourTime))
			{
				nFocusIndex = i;
				pstContents->stFocusData.bFocus = TRUE;
				pstContents->stFocusData.nIndexArray = i;
				pstContents->stFocusData.eventId = pstItem->eventId;
				pstItem->bFocus = TRUE;
				break;
			}
		}
	}

	if (pstGuideChannel->pstGuideData[nTmpFocusIndex].bFocus == TRUE)
		pstGuideChannel->pstGuideData[nTmpFocusIndex].bFocus = FALSE;

	return nFocusIndex;
}

static HINT32	nx_guide_PrevFindSetFocus(Nx_GuideContext_t *pstContents)
{
	HINT32 i = 0, nFocusIndex = 0, nTmpFocusIndex = 0;
	HBOOL	bFoundEventId = FALSE;
	Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, pstContents->nFocusChannelIndex);

	for (i = pstGuideChannel->programmeNum - 1; i > 0; i--)
	{
		if (&pstGuideChannel->pstGuideData[i])
		{
			if (pstGuideChannel->pstGuideData[i].bFocus == TRUE)
				nTmpFocusIndex = i;

			if ((&pstGuideChannel->pstGuideData[i-1]) && (pstGuideChannel->pstGuideData[i].bFocus == TRUE)
				&& (pstGuideChannel->pstGuideData[i].eventId == pstContents->stFocusData.eventId))
			{
				nFocusIndex = i-1;
				bFoundEventId = TRUE;
				pstContents->stFocusData.bFocus = TRUE;
				pstContents->stFocusData.nIndexArray = nFocusIndex;
				pstContents->stFocusData.eventId = pstGuideChannel->pstGuideData[nFocusIndex].eventId;
				pstGuideChannel->pstGuideData[nFocusIndex].bFocus = TRUE;
				break;
			}
		}
	}

	if (bFoundEventId == FALSE)
	{
		for (i = pstGuideChannel->programmeNum - 1; i >= 0; i--)
		{
			NxGuideData_t *pstItem = &pstGuideChannel->pstGuideData[i];

			if (pstItem->startTime < (pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM))
			{
				nFocusIndex = i;
				pstContents->stFocusData.bFocus = TRUE;
				pstContents->stFocusData.nIndexArray = i;
				pstContents->stFocusData.eventId = pstItem->eventId;
				pstItem->bFocus = TRUE;
				break;
			}
		}
	}

	if (pstGuideChannel->pstGuideData[i].bFocus == TRUE)
		pstGuideChannel->pstGuideData[i].bFocus = FALSE;

	return nFocusIndex;
}

static HINT32		nx_guide_GetFocusIndex(Nx_GuideContext_t *pstContents)
{
	HINT32 i = 0;
	Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, pstContents->nFocusChannelIndex);

	for (i = 0; i < pstGuideChannel->programmeNum;i++)
	{
		if (&pstGuideChannel->pstGuideData[i] && (pstGuideChannel->pstGuideData[i].bFocus == TRUE))
		{
			pstGuideChannel->pstGuideData[i].bFocus = FALSE;
			return ONDK_RET_OK;
		}
	}
	return ONDK_RET_FAIL;

}

static ONDK_Result_t	nx_guide_CellBackward(Nx_GuideContext_t *pstContents)
{
	NxGuideFocusingType_e eFocusType = eGuideFocusType_NONE;
	HUINT32 tempTime = pstContents->unSearchUnitTime - (SECONDS_PER_HOUR * 2);

	if (tempTime < pstContents->unStandardStartTime)
		pstContents->unSearchUnitTime = pstContents->unStandardStartTime;
	else
	{
		pstContents->unSearchUnitTime = tempTime;
		pstContents->eLastFocusType = eGuideFocusType_LAST;
		eFocusType = eGuideFocusType_LAST;
	}

	nx_guide_CalcUnitTimePerHalfHour(pstContents);
	nx_guide_CleanFocus(pstContents);
	nx_guide_ReloadData(pstContents, (HINT32)eFocusType, 0, eNxGuideDir_LEFT);
	nx_guide_ResetTimeDisplay(pstContents);

	// reload 에서 eLastFocusType 을 none 으로 설정하므로, 다시한번 셋팅하고 결과를 기다림.
	pstContents->eLastFocusType = eFocusType;
	nx_guide_SetReloadDataTimer();

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CellForward(Nx_GuideContext_t *pstContents)
{
	pstContents->unSearchUnitTime = pstContents->unSearchUnitTime + (SECONDS_PER_HOUR * 2);	// current search display time
	nx_guide_CalcUnitTimePerHalfHour(pstContents);
	nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, eNxGuideDir_RIGHT);

	nx_guide_ResetTimeDisplay(pstContents);

	nx_guide_SetReloadDataTimer();

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CellPrev(Nx_GuideContext_t *pstContents)
{
	NxGuideFocusingType_e eFocusType = eGuideFocusType_NONE;
	HINT32 	nFocusChannelIndex = pstContents->nFocusChannelIndex;
	Nx_GuideChannel_t *pstCurGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, nFocusChannelIndex);
	HINT32	nPrevItemIndex = pstContents->stFocusData.nIndexArray - 1;

	if ((nPrevItemIndex >= 0)
		&& pstCurGuideChannel->pstGuideData != NULL
		&& (&pstCurGuideChannel->pstGuideData[nPrevItemIndex] != NULL))
	{
		NxGuideData_t *pstPrevItem = &pstCurGuideChannel->pstGuideData[nPrevItemIndex];
		if ((pstPrevItem->startTime < (pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM))
			&& ((pstPrevItem->startTime + pstPrevItem->duration) > pstContents->unSearchUnitPerHalfHourTime))
		{
			nx_guide_PrevFindSetFocus(pstContents);
			pstContents->bChangedFocus = TRUE;	// method cb 에서 data 갱신.
			pstCurGuideChannel->bFocusUpdate = TRUE;
			ONDK_GWM_APP_InvalidateObject(NX_GUIDE_PROGRAMMEINFORM_ID);
		}
		else
		{
			HUINT32 tempTime = pstContents->unSearchUnitTime - (SECONDS_PER_HOUR * 2);

			if (tempTime < pstContents->unStandardStartTime)
			{
				pstContents->unSearchUnitTime = pstContents->unStandardStartTime;
			}
			else
			{
				pstContents->unSearchUnitTime = tempTime;
				pstContents->eLastFocusType = eGuideFocusType_LAST;
				eFocusType = eGuideFocusType_LAST;
			}

			nx_guide_CalcUnitTimePerHalfHour(pstContents);
			nx_guide_CleanFocus(pstContents);
			nx_guide_ReloadData(pstContents, (HINT32)eFocusType, 0, 0);
			nx_guide_ResetTimeDisplay(pstContents);

			// reload 에서 eLastFocusType 을 none 으로 설정하므로, 다시한번 셋팅하고 결과를 기다림.
			pstContents->eLastFocusType = eFocusType;
			nx_guide_SetReloadDataTimer();
		}
	}
	else
	{
		HUINT32 tempTime = pstContents->unSearchUnitTime - (SECONDS_PER_HOUR * 2);

		if (tempTime < pstContents->unStandardStartTime)
		{
			pstContents->unSearchUnitTime = pstContents->unStandardStartTime;
		}
		else
		{
			pstContents->unSearchUnitTime = tempTime;
			pstContents->eLastFocusType = eGuideFocusType_LAST;
			eFocusType = eGuideFocusType_LAST;
		}
		nx_guide_CalcUnitTimePerHalfHour(pstContents);
		nx_guide_CleanFocus(pstContents);
		nx_guide_ReloadData(pstContents, (HINT32)eFocusType, 0, eNxGuideDir_LEFT);
		nx_guide_ResetTimeDisplay(pstContents);

		// reload 에서 eLastFocusType 을 none 으로 설정하므로, 다시한번 셋팅하고 결과를 기다림.
		pstContents->eLastFocusType = eFocusType;
		nx_guide_SetReloadDataTimer();
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CellNext(Nx_GuideContext_t *pstContents)
{

	HINT32 	nFocusChannelIndex = pstContents->nFocusChannelIndex;
	Nx_GuideChannel_t *pstCurGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, nFocusChannelIndex);
	HINT32	nNextItemIndex = pstContents->stFocusData.nIndexArray + 1;

	if (nNextItemIndex < pstCurGuideChannel->programmeNum
		&& pstCurGuideChannel->pstGuideData != NULL
		&& (&pstCurGuideChannel->pstGuideData[nNextItemIndex] != NULL))
	{
		NxGuideData_t *pstNextItem = &pstCurGuideChannel->pstGuideData[nNextItemIndex];
		if (pstNextItem->startTime < (pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM))
		{
			nx_guide_NextFindSetFocus(pstContents);
			pstContents->bChangedFocus = TRUE;	// method cb 에서 data 갱신.
			pstCurGuideChannel->bFocusUpdate = TRUE;
			ONDK_GWM_APP_InvalidateObject(NX_GUIDE_PROGRAMMEINFORM_ID);
		}
		else
		{
			pstContents->unSearchUnitTime = pstContents->unSearchUnitTime + (SECONDS_PER_HOUR * 2);	// current search display time
			nx_guide_CalcUnitTimePerHalfHour(pstContents);
			nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, 0);

			nx_guide_ResetTimeDisplay(pstContents);

			nx_guide_SetReloadDataTimer();
		}
	}
	else
	{
		pstContents->unSearchUnitTime = pstContents->unSearchUnitTime + (SECONDS_PER_HOUR * 2);	// current search display time
		nx_guide_CalcUnitTimePerHalfHour(pstContents);
		nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, eNxGuideDir_RIGHT);

		nx_guide_ResetTimeDisplay(pstContents);

		nx_guide_SetReloadDataTimer();
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CellUp(Nx_GuideContext_t *pstContents)
{
	HINT32	nFocusCurChannelIndex  = pstContents->nFocusChannelIndex;
	HINT32	nFocusUpChannelIndex = pstContents->nFocusChannelIndex - 1;
	HINT32	nMaxCollSize = 0, nGetCollSize = 0;
	HINT32	nMaxItemIndex = 0;

	if (nFocusUpChannelIndex >= 0)
	{
		HINT32	i = 0;

		Nx_GuideChannel_t *pstCurGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, nFocusCurChannelIndex);
		Nx_GuideChannel_t *pstUpGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, nFocusUpChannelIndex);

		NxGuideData_t *pstCurItem = &pstCurGuideChannel->pstGuideData[pstContents->stFocusData.nIndexArray];

		if (TRUE == nx_guide_IsFirstItem(pstContents, pstCurGuideChannel, pstCurItem))
		{
			HINT32 nFirstIndex = -1;
			// first item 이라면 이동하는 타겟도 first item 으로 셋.
			if (ERR_OK == nx_guide_GetFirstItemOfChannel(pstContents, pstUpGuideChannel, &nFirstIndex))
			{
				nMaxCollSize = 10000;
				nMaxItemIndex = nFirstIndex;
			}
		}
		else if (TRUE == nx_guide_IsLastItem(pstContents, pstCurGuideChannel, pstCurItem))
		{
			HINT32 nLastIndex = -1;
			if (ERR_OK == nx_guide_GetLastItemOfChannel(pstContents, pstUpGuideChannel, &nLastIndex))
			{
				nMaxCollSize = 10000;
				nMaxItemIndex = nLastIndex;
			}
		}
		else
		{
			for (i = 0; i < pstUpGuideChannel->programmeNum; i++)
			{
				if ( &pstUpGuideChannel->pstGuideData[i] != NULL)
				{
					NxGuideData_t *pstCompareItem = &pstUpGuideChannel->pstGuideData[i];
					nGetCollSize = nx_guide_GetCollisionSize(pstContents, pstCurItem, pstCompareItem);

					HxLOG_Debug("\n(%d):(%s) ulGetCollSize(%d),  maxSIze(%d)\n", i, pstCompareItem->pszEventName, nGetCollSize, nMaxCollSize);

					if (nGetCollSize > nMaxCollSize)
					{
						nMaxCollSize = nGetCollSize;
						nMaxItemIndex = i;
					}
				}
			}
		}

		if (nMaxCollSize > 0)
		{
			// 이전 focus data clear
			nx_guide_CleanFocus(pstContents);

			// set focus data
			pstContents->stFocusData.bFocus = TRUE;
			pstContents->stFocusData.nChannelListIndex = nFocusUpChannelIndex;
			pstContents->stFocusData.nIndexArray = nMaxItemIndex;
			pstContents->stFocusData.eventId = pstUpGuideChannel->pstGuideData[pstContents->stFocusData.nIndexArray].eventId;
			pstUpGuideChannel->pstGuideData[pstContents->stFocusData.nIndexArray].bFocus = TRUE;

			pstContents->bChangedFocus = TRUE;	// method cb 에서 data 갱신.

			pstCurGuideChannel->bFocusUpdate = TRUE;
			pstUpGuideChannel->bFocusUpdate = TRUE;

			ONDK_GWM_APP_InvalidateObject(NX_GUIDE_PROGRAMMEINFORM_ID);
		}

		pstContents->nFocusChannelIndex--;
	}
	else if (pstContents->nChannelNum > OG_GUIDE_COL_NUM)
	{
		Nx_GuideChannel_t *listFirstIndexData = NULL;
		Nx_GuideChannel_t *listSecondIndexData = NULL;

		pstContents->nSearchStartChannelIndex--;
		if (pstContents->nSearchStartChannelIndex < 0)
			pstContents->nSearchStartChannelIndex = (pstContents->nChannelNum - 1);

		pstContents->eCurDirCom = eNxGuideDir_UP;

		// clear focus
		nx_guide_CleanFocus(pstContents);
		// remove first guidechanel item
		nx_guide_RemoveGuideChannelItem(pstContents, eNxGuideDir_UP);

		// set changed focus
		listFirstIndexData = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, 0);
		listSecondIndexData = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, 1);

		listFirstIndexData->pstGuideData[0].bFocus = TRUE;

		listFirstIndexData->bFocusUpdate = TRUE;
		listSecondIndexData->bFocusUpdate = TRUE;

		// set focus data
		pstContents->stFocusData.bFocus = TRUE;
		pstContents->stFocusData.nChannelListIndex = 0;
		pstContents->stFocusData.nIndexArray = 0;	// nx_guide_RemoveGuideChannelItem 되면서 추가된 아이템은 noprogramme 1개.
		pstContents->stFocusData.eventId = listFirstIndexData->pstGuideData[0].eventId;

		pstContents->bChangedFocus = TRUE;	// method cb 에서 data 갱신.

		// redraw sync
		ONDK_GWM_APP_InvalidateObject(NX_GUIDE_PROGRAMMEINFORM_ID);
		ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);

		nx_guide_SetReloadDataTimer();
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CellDown(Nx_GuideContext_t *pstContents)
{
	HINT32	nFocusCurChannelIndex  = pstContents->nFocusChannelIndex;
	HINT32	nFocusDownChannelIndex = pstContents->nFocusChannelIndex + 1;
	HINT32 	i = 0;
	HINT32	nMaxCollSize = 0, nGetCollSize = 0;
	HINT32	nMaxItemIndex = 0;

	if(nFocusDownChannelIndex < pstContents->nChannelNum
		&& nFocusDownChannelIndex < OG_GUIDE_COL_NUM)
	{
		Nx_GuideChannel_t *pstCurGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, nFocusCurChannelIndex);
		Nx_GuideChannel_t *pstDownGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, nFocusDownChannelIndex);
		NxGuideData_t *pstCurItem = &pstCurGuideChannel->pstGuideData[pstContents->stFocusData.nIndexArray];

		if (TRUE == nx_guide_IsFirstItem(pstContents, pstCurGuideChannel, pstCurItem))
		{
			HINT32 nFirstIndex = -1;
			// first item 이라면 이동하는 타겟도 first item 으로 셋.
			if (ERR_OK == nx_guide_GetFirstItemOfChannel(pstContents, pstDownGuideChannel, &nFirstIndex))
			{
				nMaxCollSize = 10000;
				nMaxItemIndex = nFirstIndex;
			}
		}
		else if (TRUE == nx_guide_IsLastItem(pstContents, pstCurGuideChannel, pstCurItem))
		{
			HINT32 nLastIndex = -1;
			if (ERR_OK == nx_guide_GetLastItemOfChannel(pstContents, pstDownGuideChannel, &nLastIndex))
			{
				nMaxCollSize = 10000;
				nMaxItemIndex = nLastIndex;
			}
		}
		else
		{
			for (i = 0; i < pstDownGuideChannel->programmeNum; i++)
			{
				if ( &pstDownGuideChannel->pstGuideData[i] != NULL)
				{
					NxGuideData_t *pstCompareItem = &pstDownGuideChannel->pstGuideData[i];
					nGetCollSize = nx_guide_GetCollisionSize(pstContents, pstCurItem, pstCompareItem);

					HxLOG_Debug("\n(%d):(%s) ulGetCollSize(%d),  maxSIze(%d)\n", i, pstCompareItem->pszEventName, nGetCollSize, nMaxCollSize);

					if (nGetCollSize > nMaxCollSize)
					{
						nMaxCollSize = nGetCollSize;
						nMaxItemIndex = i;
					}
				}
			}
		}


		if (nMaxCollSize > 0)
		{
			// 이전 focus data clear
			nx_guide_CleanFocus(pstContents);

			// set focus data
			pstContents->stFocusData.bFocus = TRUE;
			pstContents->stFocusData.nChannelListIndex = nFocusDownChannelIndex;
			pstContents->stFocusData.nIndexArray = nMaxItemIndex;
			pstContents->stFocusData.eventId = pstDownGuideChannel->pstGuideData[pstContents->stFocusData.nIndexArray].eventId;
			pstDownGuideChannel->pstGuideData[pstContents->stFocusData.nIndexArray].bFocus = TRUE;

			pstContents->bChangedFocus = TRUE;	// method cb 에서 data 갱신.

			pstCurGuideChannel->bFocusUpdate = TRUE;
			pstDownGuideChannel->bFocusUpdate = TRUE;

			ONDK_GWM_APP_InvalidateObject(NX_GUIDE_PROGRAMMEINFORM_ID);
		}

		pstContents->nFocusChannelIndex++;
	}
	else if(pstContents->nChannelNum > OG_GUIDE_COL_NUM)
	{
		Nx_GuideChannel_t *listLastIndexData = NULL;
		Nx_GuideChannel_t *listPrevLastIndexData = NULL;

		// channel inform
		pstContents->nSearchStartChannelIndex++;
		if (pstContents->nSearchStartChannelIndex >= pstContents->nChannelNum)
			pstContents->nSearchStartChannelIndex = 0;

		pstContents->eCurDirCom = eNxGuideDir_DOWN;

		// 이전 focus data clear
		nx_guide_CleanFocus(pstContents);
		// remove first guidechanel item
		nx_guide_RemoveGuideChannelItem(pstContents, eNxGuideDir_DOWN);

		// set changed focus
		listLastIndexData = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, OG_CH_LAST_PREV_NUM);
		listPrevLastIndexData = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, (OG_CH_LAST_PREV_NUM - 1));

		listLastIndexData->pstGuideData[0].bFocus = TRUE;

		listLastIndexData->bFocusUpdate = TRUE;
		listPrevLastIndexData->bFocusUpdate = TRUE;

		// set focus data
		pstContents->stFocusData.bFocus = TRUE;
		pstContents->stFocusData.nChannelListIndex = OG_CH_LAST_PREV_NUM;
		pstContents->stFocusData.nIndexArray = 0;	// nx_guide_RemoveGuideChannelItem 되면서 추가된 아이템은 noprogramme 1개.
		pstContents->stFocusData.eventId = listLastIndexData->pstGuideData[0].eventId;

		pstContents->bChangedFocus = TRUE;	// method cb 에서 data 갱신.

		// data sync
		ONDK_GWM_APP_InvalidateObject(NX_GUIDE_PROGRAMMEINFORM_ID);
		ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);

		nx_guide_SetReloadDataTimer();
	}
	return ONDK_RET_OK;
}

static void	nx_guide_PageUp(Nx_GuideContext_t *pstContents)
{
	if (pstContents->nChannelNum > OG_GUIDE_COL_NUM)
	{
		pstContents->nSearchStartChannelIndex -= OG_GUIDE_COL_NUM;
		if (pstContents->nSearchStartChannelIndex < 0)
			pstContents->nSearchStartChannelIndex = pstContents->nChannelNum + pstContents->nSearchStartChannelIndex;

		pstContents->eCurDirCom = eNxGuideDir_PAGE_UP;
		ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);
		nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, eNxGuideDir_PAGE_UP);
		nx_guide_SetReloadDataTimer();
	}
}

static void	nx_guide_PageDown(Nx_GuideContext_t *pstContents)
{
	if (pstContents->nChannelNum > OG_GUIDE_COL_NUM)
	{
		pstContents->nSearchStartChannelIndex += OG_GUIDE_COL_NUM;
		if (pstContents->nSearchStartChannelIndex >= pstContents->nChannelNum)
		{
			pstContents->nSearchStartChannelIndex = (pstContents->nSearchStartChannelIndex - pstContents->nChannelNum);
		}
		pstContents->eCurDirCom = eNxGuideDir_PAGE_DOWN;
		ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);
		nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, eNxGuideDir_PAGE_DOWN);
		nx_guide_SetReloadDataTimer();
	}
}

static ONDK_Result_t nx_guide_MoveToDcnCh(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 	i = 0;
	HUINT32 ulChangeChSvcUid = (HUINT32)p2;
	HINT32 	curch_index = pstContents->nSearchStartChannelIndex;
	HBOOL	bIsCurList = FALSE;

	if (ulChangeChSvcUid == pstContents->pstChannels[pstContents->nCurChannelIndex].svcUid)
		return ONDK_RET_OK;

	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		NX_Channel_t *pstChannel = &pstContents->pstChannels[curch_index];

		if ( pstChannel != NULL
			&& pstChannel->svcUid == ulChangeChSvcUid)
		{
			bIsCurList = TRUE;
			break;
		}
		// check OG_GUIDE_COL_NUM 보다 적은 채널을 갖는 경우.
		if (curch_index >= (pstContents->nChannelNum - 1))
		{
			if (pstContents->nChannelNum > OG_GUIDE_COL_NUM)
				curch_index = 0;
			else if (i < (pstContents->nChannelNum - 1))
				curch_index++;
			else
				curch_index = -1;
		}
		else if (i < (pstContents->nChannelNum - 1))
			curch_index++;
		else
			curch_index = -1;
	}

	if (bIsCurList)
	{
		nx_guide_CleanFocus(pstContents);
		pstContents->nFocusChannelIndex = i;
		nx_guide_SetFocus(pstContents);
		ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);
	}
	else
	{
		HBOOL bFoundCh = FALSE;
		curch_index = 0;
		for (i = 0; i < pstContents->nChannelNum; i++)
		{
			NX_Channel_t *pstChannel = &pstContents->pstChannels[curch_index];

			if ( pstChannel != NULL
				&& pstChannel->svcUid == ulChangeChSvcUid)
			{
				bFoundCh = TRUE;
				break;
			}
			// check OG_GUIDE_COL_NUM 보다 적은 채널을 갖는 경우.
			if (curch_index >= (pstContents->nChannelNum - 1))
			{
				if (pstContents->nChannelNum > OG_GUIDE_COL_NUM)
					curch_index = 0;
				else if (i < (pstContents->nChannelNum - 1))
					curch_index++;
				else
					curch_index = -1;
			}
			else if (i < (pstContents->nChannelNum - 1))
				curch_index++;
			else
				curch_index = -1;
		}
		if (bFoundCh)
		{
			nx_guide_CleanFocus(pstContents);

			pstContents->nSearchStartChannelIndex = curch_index;
			pstContents->nFocusChannelIndex = 0;
			ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);
			nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, 0, 0);
			nx_guide_SearchStartAll(pstContents, eGuideSearchType_ReQuery);
		}
	}
	return ONDK_MESSAGE_BREAK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_GUIDE_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_GuideContext_t	*pstContents = NULL;

	pstContents = nx_guide_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_guide_MsgGwmCreate(pstContents, p1, p2, p3);
			if (eResult != ONDK_RET_OK)
			{
				HxLOG_Error("Failed Init TvGuide!!!\n\n");
				ONDK_GWM_APP_Destroy(0);
				NX_PRISM_NotAvailable_Show();
			}
			break;

		case MSG_GWM_TIMER:
			nx_guide_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_guide_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_guide_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			//eResult = nx_guide_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES:
			HxLOG_Error("\n ======>>> RECEIVED MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES \n");
			{
				pstContents->bIsReceivedResult = TRUE;
				nx_guide_ReloadData(pstContents, (HINT32)eGuideFocusType_NONE, p2, p3);
			}
			break;

		case MSG_CORE_SERVICE_EPG_JUMPTODATE_UPDATE:
			nx_guide_SetJumpToDate(pstContents, (HxDATETIME_t*)p1);
			break;

		case MSG_APP_SYSTEM_SVC_CHANGED:
			ONDK_GWM_APP_Destroy(0);
			//nx_guide_CheckSvcChanged(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_GUIDE_DONE_WATCH_LIVE_TV:
			// destroy proc when received pushed watch tv button from detail information proc.
			ONDK_GWM_APP_Destroy(0);
			break;

		case MSG_APP_DCN_INPUT:
			eResult = nx_guide_MoveToDcnCh(pstContents, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;
			//break;

		case MSG_APP_POPUP_LIST_DESTROYED:
			if (p1 == KEY_BACK)
				nx_guide_DestroyListPopup();
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED & MSG_APP_SETTING_VALUE_CHANGED\n");
			if (lMessage == MSG_APP_SETTING_VALUE_CHANGED)
			{
				NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED\n");
				nx_guide_DestroyListPopup();
				eResult = nx_guide_OptionChangedItemValue(pstContents, p1);
			}
			break;

		case MSG_CORE_SCHEDULE_NOTIFY:
			eResult = nx_guide_RecvScheduleEvent(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Guide)
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

#define _____________RELATED__SCHEDULE___________

static Nx_GuideSchedule_t *nx_guide_GetScheduleContents(void)
{
	return &s_stGuideSchedule;
}

static void 	nx_guide_ScheduleContentListDelete(void *item)
{
	if(item)
	{
		Nx_SvcSchedule_t *tempRef = (Nx_SvcSchedule_t*)item;
		if (tempRef->eventSchedList)
			HLIB_VECTOR_Delete(tempRef->eventSchedList);	// callback nx_guide_ScheduleItem_Delete
	}
}

static void		nx_guide_ScheduleItem_Delete (void *item)
{
	if(item)
	{
		NX_APP_Free(item);		item = NULL;
	}
}

static void nx_guide_InitScheduleContent(void)
{
	Nx_GuideSchedule_t *pstSchedContent = nx_guide_GetScheduleContents();

	HxSTD_MemSet(pstSchedContent, 0, sizeof(Nx_GuideSchedule_t));

	pstSchedContent->schedList = HLIB_VECTOR_NewEx(NULL, 0,
										(void *(*)(void *))NULL,
										(void *(*)(void *))nx_guide_ScheduleContentListDelete,
										NULL);
}

static void nx_guide_DestroyScheduleAll(void)
{
	Nx_GuideSchedule_t *pstSchedContent = nx_guide_GetScheduleContents();

	if (pstSchedContent->schedList)
		HLIB_VECTOR_Delete(pstSchedContent->schedList);

	HxSTD_MemSet(pstSchedContent, 0, sizeof(Nx_GuideSchedule_t));
}

static HBOOL nx_guide_FindScheduleService(void *data, void *userdata)
{
	Nx_SvcSchedule_t *pstSvcSchedule = (Nx_SvcSchedule_t *)data;
	HUINT32 findSvcUid = (HUINT32)userdata;

	if (pstSvcSchedule == NULL)
		return FALSE;

	if(pstSvcSchedule->svcUid == findSvcUid)
		return TRUE;
	else
		return FALSE;
}

static void nx_guide_LoadScheduleAll(void)
{
	Nx_GuideSchedule_t *pstSchedContent = nx_guide_GetScheduleContents();
	HxVector_t *schedList = HLIB_VECTOR_New(NULL,NULL);
	HINT32	nSchedNum = 0, i = 0;

	nx_guide_DestroyScheduleAll();
	nx_guide_InitScheduleContent();

	NX_SCHEDULER_GetScheduleList(schedList, &nSchedNum);

	for(i = 0; i < nSchedNum; i++)
	{
		Nx_SvcSchedule_t *foundSvcItem = NULL;
		Nx_GuideSchedItem_t *pAddItem = NULL;
		DxSchedule_t *pSchedItem = HLIB_VECTOR_ItemAt(schedList, i);

		foundSvcItem = HLIB_VECTOR_Find(pstSchedContent->schedList, nx_guide_FindScheduleService, (void*)pSchedItem->ulSvcUid);

		pAddItem = (Nx_GuideSchedItem_t*)NX_APP_Calloc(sizeof(Nx_GuideSchedItem_t));

		if (foundSvcItem == NULL)
		{
			// svc uid 로 schedule list 생성.
			foundSvcItem = (Nx_SvcSchedule_t*)NX_APP_Calloc(sizeof(Nx_SvcSchedule_t));
			foundSvcItem->svcUid = pSchedItem->ulSvcUid;
			foundSvcItem->eventSchedList = HLIB_VECTOR_NewEx(NULL, 0,
										(void *(*)(void *))NULL,
										(void *(*)(void *))nx_guide_ScheduleItem_Delete,
										NULL);

			HLIB_VECTOR_Add(pstSchedContent->schedList, foundSvcItem);
		}
		pAddItem->eRsvType = pSchedItem->eRsvType;
		switch(pAddItem->eRsvType)
		{
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_TBR:
			pAddItem->nEvtId = pSchedItem->uExtInfo.stWch.nEvtId;
			HxSTD_MemCopy(&pAddItem->uExtInfo.stWch, &pSchedItem->uExtInfo.stWch, sizeof(DxRSV_Watching_t));
			break;

		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
			pAddItem->nEvtId = pSchedItem->uExtInfo.stRec.nEvtId;
			HxSTD_MemCopy(&pAddItem->uExtInfo.stRec, &pSchedItem->uExtInfo.stRec, sizeof(DxRSV_Record_t));
			break;

		default:
			break;
		}

		HLIB_VECTOR_Add(foundSvcItem->eventSchedList, pAddItem);
		foundSvcItem->nEventSchedNum = HLIB_VECTOR_Length(foundSvcItem->eventSchedList);
	}

	HLIB_VECTOR_Delete(schedList);
}

static HBOOL nx_guide_FindScheduleEventItem(void *data, void *userdata)
{
	Nx_GuideSchedItem_t *pstSchedItem = (Nx_GuideSchedItem_t *)data;
	HINT32 findEvntID  = (HINT32)userdata;

	if(pstSchedItem->nEvtId == findEvntID)
		return TRUE;
	else
		return FALSE;
}

static ONDK_Result_t nx_guide_UpdateScheduleProgrammeItem(Nx_GuideChannel_t *pstGuideChannel)
{
	HINT32 i = 0;
	Nx_SvcSchedule_t *foundSvcItem = NULL;
	Nx_GuideSchedule_t *pstSchedContent = nx_guide_GetScheduleContents();

	if (pstGuideChannel == NULL)
		return ONDK_RET_FAIL;

	if (pstSchedContent->schedList)
		foundSvcItem = HLIB_VECTOR_Find(pstSchedContent->schedList, nx_guide_FindScheduleService, (void*)pstGuideChannel->svcUid);

	if (pstGuideChannel->pstGuideData != NULL)
	{
		for(i = 0; i < pstGuideChannel->programmeNum; i++)
		{
			Nx_GuideSchedItem_t *foundItem = NULL;
			if (foundSvcItem && foundSvcItem->eventSchedList)
			{
				foundItem = HLIB_VECTOR_Find(foundSvcItem->eventSchedList, nx_guide_FindScheduleEventItem, (void*)pstGuideChannel->pstGuideData[i].eventId);
			}

			if (foundItem)
				pstGuideChannel->pstGuideData[i].eRsvType = foundItem->eRsvType;
			else
				pstGuideChannel->pstGuideData[i].eRsvType = DxRSVTYPE_NOTHING;
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_guide_UpdateScheduleData(Nx_GuideContext_t *pstContents)
{
	HINT32 i = 0;
	HINT32 nIndex = pstContents->nSearchStartChannelIndex;

	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, i);
		if (pstGuideChannel == NULL)
			continue;

		nx_guide_UpdateScheduleProgrammeItem(pstGuideChannel);
		nIndex++;
		if (nIndex >= pstContents->nChannelNum)
			nIndex = 0;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_guide_RecvScheduleEvent(Nx_GuideContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(p1/*eMsgType*/)
	{
	case eAPK_METASCH_MSG_ScheduleAdded:
	case eAPK_METASCH_MSG_ScheduleRemoved:
	case eAPK_METASCH_MSG_ScheduleChanged:

	default:
		break;
	}
	nx_guide_LoadScheduleAll();
	nx_guide_UpdateScheduleData(pstContents);
	ONDK_GWM_APP_InvalidateObject(NX_GUIDE_PROGRAMMEINFORM_ID);

	return ONDK_RET_OK;
}


#define _____________RELATED___DRAW______________
static void		nx_guide_draw_DrawBackground(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	/* background start */
	stRect = ONDK_Rect(NX_PRISM_UI_BACKGROUND_BOX_X, NX_PRISM_UI_BACKGROUND_BOX_Y,
						NX_PRISM_UI_BACKGROUND_BOX_WIDTH, NX_PRISM_UI_BACKGROUND_BOX_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_GUIDE_BACKGROUND_ID, &stRect, COL(C_G_Bang_80));
}

static void	nx_guide_draw_DimLCell(Nx_GuideContext_t *pstContents, ONDKSurfaceHandle screen)
{
	ONDK_Rect_t drawRect, drawStrcRect;

	ONDK_SURFACE_Blitflag_SetBlend(screen);
	drawRect = ONDK_Rect(OG_IMG_RES_DIM_LCELL_X, OG_IMG_RES_DIM_LCELL_Y, OG_IMG_RES_DIM_LCELL_W, OG_IMG_RES_DIM_LCELL_H);

	drawStrcRect = ONDK_Rect(OG_DIM_LCELL_X, OG_DIM_LCELL_Y, OG_DIM_LCELL_W, OG_DIM_LCELL_H);
	ONDK_SURFACE_StretchBlitWithoutBlend(s_hImageResSurface, screen, drawRect, drawStrcRect);
}

static void nx_guide_draw_DimBottom(Nx_GuideContext_t *pstContents, ONDKSurfaceHandle screen)
{
	ONDK_Rect_t drawRect, drawStrcRect;

	ONDK_SURFACE_Blitflag_SetBlend(screen);
	drawRect = ONDK_Rect(OG_IMG_RES_DIM_BOTTOM_X, OG_IMG_RES_DIM_BOTTOM_Y, OG_IMG_RES_DIM_BOTTOM_W, OG_IMG_RES_DIM_BOTTOM_H);

	drawStrcRect = ONDK_Rect(OG_DIM_BOTTOM_X, OG_DIM_BOTTOM_Y, OG_DIM_BOTTOM_W, OG_DIM_BOTTOM_H);
	ONDK_SURFACE_StretchBlitWithoutBlend(s_hImageResSurface, screen, drawRect, drawStrcRect);
}

static ONDK_Result_t nx_guide_draw_CurrentVerticalLine(Nx_GuideContext_t *pstContents, ONDKSurfaceHandle screen)
{
	HUINT32	unCurTime = 0, current = 0, cellW = 0;
	ONDK_Color_t		item_color = 0, item_Shadowcolor = 0;
	ONDK_Rect_t	drawRect;

	unCurTime = pstContents->unCurrentTime;

	if (unCurTime < NX_VALID_UNIXTIME)
		return ONDK_RET_FAIL;
	else if ((unCurTime > pstContents->unSearchUnitPerHalfHourTime)
			  && (unCurTime < (pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM)))
	{
		current = abs(unCurTime - pstContents->unSearchUnitPerHalfHourTime);
		cellW = (current / 60) * OG_W_PER_MIN;

		// Draw Time Line
		item_color = COL(C_G_Yoyo02);
		drawRect = ONDK_Rect(OG_PRO_X + cellW, OG_PRO_Y, OG_TIME_LINE_SHADOW_W, OG_PRO_H);
		ONDK_DRAW_FillRect(screen, drawRect, item_color);

		// Draw Time Line shadow
		item_Shadowcolor = COL(C_G_Bang_50);
		drawRect = ONDK_Rect(drawRect.x + drawRect.w, drawRect.y, OG_TIME_LINE_SHADOW_W, drawRect.h);
		ONDK_DRAW_FillRect(screen, drawRect, item_Shadowcolor);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_draw_DrawItemChannel(Nx_GuideContext_t *pstContents, HINT32 channelIndex, ONDKSurfaceHandle surface, ONDK_Rect_t *rect)
{
	DxService_t			*tempSvc = NULL;
	ONDK_Color_t		fontColor = 0;
	ONDKFontHandle		*hChannelFont = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_20);
	NX_Channel_t		*pstChannel   = NULL;

	if (channelIndex != -1)
		pstChannel = &pstContents->pstChannels[channelIndex];

	// if current channel..
	if (pstContents->nCurChannelIndex == channelIndex)
		ONDK_DRAW_FillRectElement(surface, rect->x, rect->y, rect->w, rect->h, COL(C_G_Base01_20));
	else
		ONDK_DRAW_FillRectElement(surface, rect->x, rect->y, rect->w, rect->h, COL(C_G_Whistle_10));

	if (pstChannel == NULL)
	{
		HxLOG_Error("pstChannel is NULL! No There Channel Inform item.!\n");
		return ONDK_RET_FAIL;
	}

	tempSvc = NX_CHANNEL_FindServiceBySvcUid(pstChannel->svcUid);

	if (tempSvc)
	{
		HCHAR				szChNumber[5];
		fontColor = COL(C_T_Gong03);
		HxSTD_sprintf(szChNumber, "%04d", pstChannel->lcn);
		ONDK_FONT_DrawStringAlignAbbr(surface, hChannelFont, szChNumber, OG_CH_LCN_X,
							(rect->y + OG_CH_LCN_X), OG_CH_LCN_W, fontColor, ONDKSTF_TOPLEFT);

		ONDK_FONT_DrawStringAlignAbbr(surface, hChannelFont, tempSvc->name, OG_CH_NAME_X,
										(rect->y + OG_CH_NAME_Y), OG_CH_NAME_W, fontColor, ONDKSTF_TOPLEFT);
	}

	ONDK_FONT_Release(hChannelFont);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_draw_CreateChInform(Nx_GuideContext_t *pstContents)
{
	ONDK_Rect_t 		drawRect;
	HINT32				i = 0, yPos = 0, nIndex = 0;
	ONDKSurfaceHandle	screen = NULL;

	nIndex = pstContents->nCurChannelIndex;

	if (s_hChInfoSurface == NULL)
	{
		s_hChInfoSurface = ONDK_SURFACE_Create(OG_CH_ITEM_W, OG_CH_INFORM_H, "ARGB8888");
		ONDK_SURFACE_Clear(s_hChInfoSurface);
	}

	if (s_hTempChInfoSurface == NULL)
	{
		s_hTempChInfoSurface = ONDK_SURFACE_Create(OG_CH_ITEM_W, OG_CH_INFORM_H, "ARGB8888");
		ONDK_SURFACE_Clear(s_hTempChInfoSurface);
	}

#if !defined(CONFIG_APUI_SUPPORT_ANIMATION)
	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		drawRect = ONDK_Rect(0, yPos, OG_CH_ITEM_W, OG_CH_ITEM_H);
		nx_guide_draw_DrawItemChannel(pstContents, nIndex, s_hChInfoSurface, &drawRect);
		yPos += OG_CH_ITEM_AND_MARGIN_H;

		// check OG_GUIDE_COL_NUM 보다 적은 채널을 갖는 경우.
		if (nIndex >= (pstContents->nChannelNum - 1))
		{
			if (pstContents->nChannelNum > OG_GUIDE_COL_NUM)
				nIndex = 0;
			else if (i < (pstContents->nChannelNum - 1))
				nIndex++;
			else
				nIndex = -1;
		}
		else if (i < (pstContents->nChannelNum - 1))
			nIndex++;
		else
			nIndex = -1;
	}

	screen = ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW);
	drawRect = ONDK_Rect(0, 0, OG_CH_ITEM_W, OG_CH_INFORM_H);

	ONDK_SURFACE_Blit(s_hChInfoSurface, screen, drawRect, OG_CH_X, OG_CH_Y);

	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);
#endif
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_draw_UpdateDrawChInform(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t 	drawRect = { 0, 0, 0, 0 };
	Nx_GuideContext_t	*pstContents = nx_guide_GetContents();

	nx_guide_draw_UpdateChInform(pstContents, pstContents->eCurDirCom);

	drawRect = ONDK_Rect(0, 0, OG_CH_ITEM_W, OG_CH_INFORM_H);

	ONDK_SURFACE_Blit(s_hChInfoSurface, screen, drawRect, OG_CH_X, OG_CH_Y);

	//pstContents->eCurDirCom = eNxGuideDir_NONE;

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_draw_UpdateChInform(Nx_GuideContext_t *pstContents, Nx_GuideDir_e eDir)
{
	ONDK_Rect_t 	drawRect = { 0, 0, 0, 0 };
	HINT32			nIndex = 0;

	if (eDir == eNxGuideDir_DOWN)
	{
		ONDK_SURFACE_Clear(s_hTempChInfoSurface);
		drawRect = ONDK_Rect(0, OG_CH_ITEM_AND_MARGIN_H, OG_CH_ITEM_W, OG_CH_MOVE_BLT_H);

		ONDK_SURFACE_Blit(s_hChInfoSurface, s_hTempChInfoSurface, drawRect, 0, 0);

		ONDK_SURFACE_Clear(s_hChInfoSurface);
		drawRect = ONDK_Rect(0, 0, OG_CH_ITEM_W, OG_CH_MOVE_BLT_H);

		ONDK_SURFACE_Blit(s_hTempChInfoSurface, s_hChInfoSurface, drawRect, 0, 0);

		// 이미 search start 가 한개 증가 한후 온것
		nIndex = pstContents->nSearchStartChannelIndex + OG_CH_LAST_PREV_NUM;

		if (nIndex >= pstContents->nChannelNum)
			nIndex = nIndex - pstContents->nChannelNum;

		drawRect = ONDK_Rect(0, OG_CH_LAST_PREV_Y, OG_CH_ITEM_W, OG_CH_ITEM_H);

		nx_guide_draw_DrawItemChannel(pstContents, nIndex, s_hChInfoSurface, &drawRect);
	}
	else if (eDir == eNxGuideDir_UP)
	{
		ONDK_SURFACE_Clear(s_hTempChInfoSurface);
		drawRect = ONDK_Rect(0, 0, OG_CH_ITEM_W, OG_CH_MOVE_BLT_H);

		ONDK_SURFACE_Blit(s_hChInfoSurface, s_hTempChInfoSurface, drawRect, 0, 0);

		ONDK_SURFACE_Clear(s_hChInfoSurface);
		drawRect = ONDK_Rect(0, 0, OG_CH_ITEM_W, OG_CH_MOVE_BLT_H);

		ONDK_SURFACE_Blit(s_hTempChInfoSurface, s_hChInfoSurface, drawRect, 0, OG_CH_ITEM_AND_MARGIN_H);

		// 이미 search start 가 감소한것이므로 그냥 사용 하면 된다.
		nIndex = pstContents->nSearchStartChannelIndex;

		//if (nIndex < 0)	// 이미 처리된것이므로 0 보다 작은 값이면 코드 에러다..
		//	nIndex = pstContents->nChannelNum - 1;
		drawRect = ONDK_Rect(0, 0, OG_CH_ITEM_W, OG_CH_ITEM_H);
		nx_guide_draw_DrawItemChannel(pstContents, nIndex, s_hChInfoSurface, &drawRect);
	}
	else if (eDir == eNxGuideDir_PAGE_UP || eDir == eNxGuideDir_PAGE_DOWN || eDir == eNxGuideDir_NONE)
	{
		HINT32 i = 0, yPos = 0;
		nIndex = pstContents->nSearchStartChannelIndex;

		ONDK_SURFACE_Clear(s_hChInfoSurface);

		for (i = 0; i < OG_GUIDE_COL_NUM; i++)
		{
			drawRect = ONDK_Rect(0, yPos, OG_CH_ITEM_W, OG_CH_ITEM_H);
			nx_guide_draw_DrawItemChannel(pstContents, nIndex, s_hChInfoSurface, &drawRect);
			yPos += OG_CH_ITEM_AND_MARGIN_H;

			// check OG_GUIDE_COL_NUM 보다 적은 채널을 갖는 경우.
			if (nIndex >= (pstContents->nChannelNum - 1))
			{
				if (pstContents->nChannelNum > OG_GUIDE_COL_NUM)
					nIndex = 0;
				else if (i < (pstContents->nChannelNum - 1))
					nIndex++;
				else
					nIndex = -1;
			}
			else if (i < (pstContents->nChannelNum - 1))
				nIndex++;
			else
				nIndex = -1;
		}
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_draw_DrawCellTimeLine(Nx_GuideContext_t *pstContents, NxGuideData_t *pstGuideData, ONDKSurfaceHandle surface, ONDK_Rect_t drawRect)
{
	HUINT32	unCurTime = 0, unDiffRef = 0, ulDiff = 0, unProgress = 0;
	HINT32	nProgressW = 0;
	ONDK_Rect_t	stRect;

	unCurTime = pstContents->unCurrentTime;
	unDiffRef = pstGuideData->duration;
	ulDiff = unCurTime - pstGuideData->startTime;
	if(ulDiff > unDiffRef)
		ulDiff = unDiffRef;
	unProgress = (HUINT32)(ulDiff * 100 / unDiffRef);
	nProgressW = ((drawRect.w - 2) * unProgress) / 100;
	if (nProgressW <= 0)
		return ONDK_RET_FAIL;

	stRect = ONDK_Rect((drawRect.x + OG_PRO_CURSOR_INNER_LINE), drawRect.y, nProgressW, OG_PRO_CURSOR_DURATION_LINE);
	ONDK_DRAW_FillRect(surface, stRect, COL(C_G_Orgel01));

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_draw_DrawCellItem(Nx_GuideContext_t *pstContents, NxGuideData_t *pstGuideData, ONDKSurfaceHandle surface, HUINT32 cellW, ONDK_Rect_t drawRect)
{
	ONDK_Color_t	fontColor = 0, durationColor = 0;
	ONDK_Color_t	innerColor = COL(C_G_Bang_50);
	ONDK_Color_t	item_color = 0;
	ONDK_Rect_t		stRect;
	HINT32			nExistRsvImg = 0;

	if (pstGuideData->bFocus)
	{
		if (pstContents->bUiTop > 0)
		{
			item_color = COL(C_G_Base01_100);
			fontColor = COL(C_T_Bang_100);
			durationColor = COL(C_T_Bang_100);
		}
		else
		{
			item_color = COL(C_G_Base02_100);
			fontColor = COL(C_T_Whistle_100);
			durationColor = COL(C_T_Whistle_100);
		}
		ONDK_SURFACE_Blitflag_SetColorBlend(surface, COL_A(innerColor), COL_R(innerColor), COL_G(innerColor), COL_B(innerColor));

		// Top Line
		stRect = ONDK_Rect(drawRect.x, drawRect.y, drawRect.w, 5);
		ONDK_DRAW_FillRectWithoutColorBlend(surface, stRect);

		// Left Line
		stRect = ONDK_Rect(drawRect.x, drawRect.y, 1, drawRect.h);
		ONDK_DRAW_FillRectWithoutColorBlend(surface, stRect);

		// Right Line
		stRect = ONDK_Rect(drawRect.x, drawRect.y, (drawRect.w - 1), drawRect.h);
		ONDK_DRAW_FillRectWithoutColorBlend(surface, stRect);

		// Bottom Line
		stRect = ONDK_Rect(drawRect.x, (drawRect.y + drawRect.h - 1), drawRect.w, 1);
		ONDK_DRAW_FillRectWithoutColorBlend(surface, stRect);

		// Fill color
		ONDK_SURFACE_Blitflag_SetColorBlend(surface, COL_A(item_color), COL_R(item_color), COL_G(item_color), COL_B(item_color));
		stRect = ONDK_Rect((drawRect.x + 1), (drawRect.y + 5), (drawRect.w - 2), (drawRect.h - 6));
		ONDK_DRAW_FillRectWithoutColorBlend(surface, stRect);

		nx_guide_draw_DrawCellTimeLine(pstContents, pstGuideData, surface, drawRect);
	}
	else
	{
		fontColor = COL(C_T_Gong02);
		nx_guide_BltGridImg(surface, drawRect);

		durationColor = COL(C_T_Gong04);
	}

	if (pstGuideData->eRsvType == DxRSVTYPE_WATCHING_EBR
		|| pstGuideData->eRsvType == DxRSVTYPE_WATCHING_TBR)
	{
		ONDK_Rect_t		stRsvRect = {drawRect.x + OG_RSV_IMG_X, drawRect.y + OG_RSV_IMG_Y, OG_IMG_RES_RSV_WATCH_W, OG_IMG_RES_RSV_WATCH_H};
		nx_guide_BltRsvWatchImg(surface, stRsvRect);
		nExistRsvImg = OG_RSV_IMG_WITH_NEXT_GAP;
	}
	else if(pstGuideData->eRsvType == DxRSVTYPE_RECORD_EBR
			|| pstGuideData->eRsvType == DxRSVTYPE_RECORD_TBR)
	{
#if defined(CONFIG_MW_MM_PVR)
		ONDK_Rect_t		stRsvRect = {drawRect.x + OG_RSV_IMG_X, drawRect.y + OG_RSV_IMG_Y, OG_IMG_RES_RSV_REC_W, OG_IMG_RES_RSV_REC_H};
		nx_guide_BltRsvRecImg(surface, stRsvRect);
		nExistRsvImg = OG_RSV_IMG_WITH_NEXT_GAP;
#endif
	}

	ONDK_FONT_DrawStringAlignAbbr(surface, pstContents->eventNameFont, pstGuideData->pszEventName,
									(drawRect.x + OG_PRO_EVENT_NAME_X) + nExistRsvImg,
									(drawRect.y + OG_PRO_EVENT_NAME_Y),
									cellW - OG_PRO_EVENT_NAME_X - nExistRsvImg,
									fontColor, ONDKSTF_TOPLEFT);

	if(!pstGuideData->bNoProgramme)
	{
		ONDK_FONT_DrawStringAlignAbbr(surface, pstContents->durationFont, (HCHAR*)pstGuideData->pszDuration, (drawRect.x + OG_PRO_EVENT_TIME_X),
									(drawRect.y + OG_PRO_EVENT_TIME_Y), cellW - OG_PRO_EVENT_NAME_X, durationColor, ONDKSTF_TOPLEFT);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_draw_DrawItemProgramme(Nx_GuideContext_t *pstContents, HINT32 channelIndex, HINT32 listIndex, ONDKSurfaceHandle surface, ONDK_Rect_t *rect)
{
	HINT32				i = 0;
	NX_Channel_t		*pstChannel   = &pstContents->pstChannels[channelIndex];

	HUINT32		setStartDuration = 0, cellW = 0;
	ONDK_Rect_t	prevDrawRect, drawRect;

	Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, listIndex);

	prevDrawRect 	= ONDK_Rect(rect->x, rect->y, 0, rect->h);		// 처음 시작은. 기준시간의 위치.

	if ((pstGuideChannel->pstGuideData != NULL) &&
		(pstGuideChannel->svcUid == pstChannel->svcUid))
	{
		for(i = 0; i < pstGuideChannel->programmeNum; i++)
		{
			NxGuideData_t *guideData = &pstGuideChannel->pstGuideData[i];

			// 최대 시간 보다 길면 처리X
			if (guideData->startTime > pstContents->unDrawMaxTime)
			{
				break;;
			}
			else	// (start + duration) 이 기준 시작보다 작으면 처리 X
			if ((guideData->startTime + guideData->duration) < pstContents->unSearchUnitPerHalfHourTime)
			{
				continue;
			}
			else	// 시작시간이 기준 시간보다 작거나 같으면..
			if (guideData->startTime <= pstContents->unSearchUnitPerHalfHourTime)
			{
				//float fPosX = 0.0;
				setStartDuration = (guideData->startTime + guideData->duration) - pstContents->unSearchUnitPerHalfHourTime;
				cellW = (setStartDuration / 60 ) * OG_W_PER_MIN;
				if (cellW > 0)
				{
					drawRect = ONDK_Rect((prevDrawRect.x + prevDrawRect.w), prevDrawRect.y, cellW, prevDrawRect.h);
					nx_guide_draw_DrawCellItem(pstContents, guideData, surface, cellW, drawRect);

					prevDrawRect = drawRect;
					prevDrawRect.x += OG_PRO_CELL_MARGIN;
				}
			}
			else
			{
				if (i == 0)
				{
					setStartDuration = guideData->startTime - pstContents->unSearchUnitPerHalfHourTime;
					cellW = (setStartDuration / 60 ) * OG_W_PER_MIN;
					if (cellW > 0)
					{
						prevDrawRect.x = OG_PRO_X;
						drawRect = ONDK_Rect(prevDrawRect.x, prevDrawRect.y, cellW, prevDrawRect.h);
						nx_guide_draw_DrawCellItem(pstContents, guideData, surface, cellW, drawRect);
						prevDrawRect = drawRect;
						prevDrawRect.x += OG_PRO_CELL_MARGIN;
					}
				}
				cellW = (guideData->duration / 60 ) * OG_W_PER_MIN;
				if (cellW > 0)
				{
					drawRect = ONDK_Rect((prevDrawRect.x + prevDrawRect.w), prevDrawRect.y, cellW, prevDrawRect.h);
					nx_guide_draw_DrawCellItem(pstContents, guideData, surface, cellW, drawRect);
					prevDrawRect = drawRect;
					prevDrawRect.x += OG_PRO_CELL_MARGIN;
				}
			}
		}// end for

		// 빈공간 채우기...
		if ((prevDrawRect.x + prevDrawRect.w) < OG_PRO_W)		// 빈공간 채우기.
		{
			drawRect = ONDK_Rect((prevDrawRect.x + prevDrawRect.w), prevDrawRect.y,
						(NX_PRISM_UI_BACKGROUND_BOX_WIDTH - (prevDrawRect.x + prevDrawRect.w)), prevDrawRect.h);
			nx_guide_BltGridImg(surface, drawRect);

			ONDK_FONT_DrawStringAlignAbbr(surface, pstContents->eventNameFont, pstContents->pszNoProgram, (drawRect.x + OG_PRO_EVENT_NAME_X),
											(drawRect.y + OG_PRO_EVENT_NAME_Y), cellW - OG_PRO_EVENT_NAME_X, COL(C_T_Gong02), ONDKSTF_TOPLEFT);
		}
	}
	else
	{
		nx_guide_BltGridImg(surface, *rect);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_draw_CreateProgramme(Nx_GuideContext_t *pstContents)
{
	ONDKSurfaceHandle	screen = NULL;
	HINT32				i = 0, yPos = 0, nIndex = 0;
	ONDK_Rect_t 		drawRect = { 0, 0, 0, 0 };

	// create programme item line surface
	for(i=0; i < OG_GUIDE_COL_NUM; i++)
	{
		s_hProgrammeSurface[i] = ONDK_SURFACE_Create(OG_PRO_W, OG_PRO_ITEM_H, "ARGB8888");
		ONDK_SURFACE_Clear(s_hProgrammeSurface[i]);
	}

#if !defined(CONFIG_APUI_SUPPORT_ANIMATION)
	// fill initialize programme data
	nIndex = pstContents->nCurChannelIndex;
	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		drawRect = ONDK_Rect(0, 0, OG_PRO_W, OG_PRO_ITEM_H);
		nx_guide_draw_DrawItemProgramme(pstContents, nIndex, i, s_hProgrammeSurface[i], &drawRect);
		nIndex++;
		if (nIndex >= pstContents->nChannelNum)
			nIndex = 0;
	}
#endif

	// blt to upper window surface
	screen = ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW);
	drawRect = ONDK_Rect(0, 0, OG_PRO_W, OG_PRO_ITEM_H);


	ONDK_SURFACE_Blitflag_SetBlend(screen);

	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		ONDK_SURFACE_BlitWithoutBlend(s_hProgrammeSurface[i], screen, drawRect, OG_PRO_X, OG_PRO_Y + yPos);
		yPos += OG_PRO_ITEM_H;
	}
	// forced update
	// proc animation 으로 인해, animation 없는 경우만 create 시 강제 flush
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);


	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_draw_ReloadAllProgramme(Nx_GuideContext_t *pstContents)
{
	HINT32				i = 0, nIndex = 0;
	ONDK_Rect_t 		drawRect = { 0, 0, 0, 0 };

	// all clear programme item line surface.
	for(i=0; i < OG_GUIDE_COL_NUM; i++)
	{
		ONDK_SURFACE_Clear(s_hProgrammeSurface[i]);
	}

	// save current time
	if (NX_SYSTEM_GetTime(&pstContents->unCurrentTime) != ERR_OK)
	{
		HxLOG_Error("time is Invalid!!!\n");
		pstContents->unCurrentTime = 0;
	}

	// fill the  programme item.
	nIndex = pstContents->nSearchStartChannelIndex;
	drawRect = ONDK_Rect(0, 0, OG_PRO_W, OG_PRO_ITEM_H);
	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		nx_guide_draw_DrawItemProgramme(pstContents, nIndex, i, s_hProgrammeSurface[i], &drawRect);
		nIndex++;
		if (nIndex >= pstContents->nChannelNum)
			nIndex = 0;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_draw_UpdateDrawProgrammeInform(ONDKSurfaceHandle screen, ONDK_Region_t *area,
														ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32 			i = 0, yPos = 0, nIndex =0;
	ONDK_Rect_t 	drawRect = { 0, 0, 0, 0 };
	Nx_GuideContext_t	*pstContents = nx_guide_GetContents();

	pstContents->bUiTop = bUiTop;

	if (pstContents->eCurDirCom == eNxGuideDir_UP)
	{
		drawRect = ONDK_Rect(0, 0, OG_PRO_W, OG_PRO_ITEM_H);

		for (i = OG_CH_LAST_PREV_NUM; i > 0; i--)
		{
			if (i > 0)
			{
				ONDK_SURFACE_Clear(s_hProgrammeSurface[i]);
				ONDK_SURFACE_Blit(s_hProgrammeSurface[i-1], s_hProgrammeSurface[i], drawRect, 0, 0);
			}
		}
	}
	else if (pstContents->eCurDirCom == eNxGuideDir_DOWN)
	{
		drawRect = ONDK_Rect(0, 0, OG_PRO_W, OG_PRO_ITEM_H);

		for (i = 0; i < OG_GUIDE_COL_NUM; i++)
		{
			ONDK_SURFACE_Clear(s_hProgrammeSurface[i]);
			if (i != OG_CH_LAST_PREV_NUM)
				ONDK_SURFACE_Blit(s_hProgrammeSurface[i+1], s_hProgrammeSurface[i], drawRect, 0, 0);
		}
	}

	if (pstContents->bChangedFocus == TRUE)
	{
		nIndex = pstContents->nSearchStartChannelIndex;
		drawRect = ONDK_Rect(0, 0, OG_PRO_W, OG_PRO_ITEM_H);

		for (i = 0; i < OG_GUIDE_COL_NUM; i++)
		{
			Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, i);

			// change programme item.
			if(pstGuideChannel->bFocusUpdate)
			{
				ONDK_SURFACE_Clear(s_hProgrammeSurface[i]);
				nx_guide_draw_DrawItemProgramme(pstContents, nIndex, i, s_hProgrammeSurface[i], &drawRect);
				pstGuideChannel->bFocusUpdate = FALSE;
			}
			nIndex++;
			if (nIndex >= pstContents->nChannelNum)
				nIndex = 0;
		}
		pstContents->bChangedFocus = FALSE;	// complete focus
	}
	else		// reload all programme data and redraw all item.
	{
		// all clear programme item line surface.
		for(i=0; i < OG_GUIDE_COL_NUM; i++)
		{
			ONDK_SURFACE_Clear(s_hProgrammeSurface[i]);
		}

		// save current time
		if (NX_SYSTEM_GetTime(&pstContents->unCurrentTime) != ERR_OK)
		{
			HxLOG_Error("time is Invalid!!!\n");
			pstContents->unCurrentTime = 0;
		}

		// fill the  programme item.
		nIndex = pstContents->nSearchStartChannelIndex;
		drawRect = ONDK_Rect(0, 0, OG_PRO_W, OG_PRO_ITEM_H);
		for (i = 0; i < OG_GUIDE_COL_NUM; i++)
		{
			//HxLOG_Print("\n[%s.%d] all draw programme (%d)\n",__FUNCTION__,__LINE__, i);
			nx_guide_draw_DrawItemProgramme(pstContents, nIndex, i, s_hProgrammeSurface[i], &drawRect);
			nIndex++;
			if (nIndex >= pstContents->nChannelNum)
				nIndex = 0;
		}
	}

	drawRect = ONDK_Rect(0, 0, OG_PRO_W, OG_PRO_ITEM_H);

	ONDK_SURFACE_Blitflag_SetBlend(screen);
	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		ONDK_SURFACE_BlitWithoutBlend(s_hProgrammeSurface[i], screen, drawRect, OG_PRO_X, OG_PRO_Y + yPos);
		yPos += OG_PRO_ITEM_H;
	}

	//nx_guide_draw_DimLCell(pstContents, screen);
	//nx_guide_draw_DimBottom(pstContents, screen);

	nx_guide_draw_CurrentVerticalLine(pstContents, screen);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_ResetTimeDisplay(Nx_GuideContext_t *pstContents)
{
	HxDATETIME_t 		DateTime;

	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unSearchUnitPerHalfHourTime, &DateTime);
	HxSTD_sprintf(pstContents->szStandardDisTime, "%02d:%02d", DateTime.stTime.ucHour, DateTime.stTime.ucMinute);
	ONDK_GWM_COM_Text_SetText(NX_GUIDE_DISPLAY_STANDARD_ID, pstContents->szStandardDisTime);

	HLIB_DATETIME_ConvertUnixTimeToDateTime((pstContents->unSearchUnitPerHalfHourTime + OG_1HOUR_TO_MILLISECOND), &DateTime);
	HxSTD_sprintf(pstContents->szPlusDisTime, "%02d:%02d", DateTime.stTime.ucHour, DateTime.stTime.ucMinute);
	ONDK_GWM_COM_Text_SetText(NX_GUIDE_DISPLAY_PLUS_ID, pstContents->szPlusDisTime);

	ONDK_GWM_APP_InvalidateObject(NX_GUIDE_DISPLAY_STANDARD_ID);
	ONDK_GWM_APP_InvalidateObject(NX_GUIDE_DISPLAY_PLUS_ID);

	// check date
	if (pstContents->stSavedDateTime.stDate.ucDay != DateTime.stDate.ucDay
		|| pstContents->stSavedDateTime.stDate.usYear != DateTime.stDate.usYear
		|| pstContents->stSavedDateTime.stDate.ucMonth != DateTime.stDate.ucMonth)
	{
		HxSTD_MemCopy(&pstContents->stSavedDateTime, &DateTime, sizeof(HxDATETIME_t));
		nx_guide_MakeChangedDate(pstContents);
		ONDK_GWM_COM_Text_SetText(NX_GUIDE_DISPLAY_CHANGED_DATE_ID, pstContents->szChangedDate);
		ONDK_GWM_Obj_SetVisible(NX_GUIDE_DISPLAY_CHANGED_DATE_ID, TRUE);
		nx_guide_SetChangedDateTimer();
	}

	return ONDK_RET_OK;
}

#define ____________RELATED___DATA_______________

static void		nx_guide_NxProgrammeItem_Delete (void *item)
{
	if(item){		NX_APP_Free(item);		item = NULL;	}
}

static void	nx_guide_SetChangedDateTimer(void)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;
	retTimer = ONDK_GWM_SetTimer(NX_GUIDE_CHANGED_DATE_TIMEOUT_ID, NX_GUIDE_CHANGED_DATE_TIMEOUT);

	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_GUIDE_CHANGED_DATE_TIMEOUT_ID);
	}
}

static void	 nx_guide_MakeChangedDate(Nx_GuideContext_t *pstContents)
{
	HUINT32			ulWeekIndex = 0;
	ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(pstContents->stSavedDateTime.stDate);

	HxSTD_sprintf(pstContents->szChangedDate, "%02d.%02d %s",
		pstContents->stSavedDateTime.stDate.ucDay,
		pstContents->stSavedDateTime.stDate.ucMonth,
		ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex)));
}

static ONDK_Result_t	nx_guide_CalcUnitTimePerHalfHour(Nx_GuideContext_t *pstContents)
{
	HxDATETIME_t 		DateTime;

	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unSearchUnitTime, &DateTime);

	if (DateTime.stTime.ucMinute >= 0 && DateTime.stTime.ucMinute < 30)
	{
		DateTime.stTime.ucMinute = 0;
		DateTime.stTime.ucSecond = 0;
		DateTime.stTime.usMillisecond = 0;

		HLIB_DATETIME_ConvertDateTimeToUnixTime(&DateTime, &pstContents->unSearchUnitPerHalfHourTime);
		pstContents->unDrawMaxTime = pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM;
	}
	else if (DateTime.stTime.ucMinute >= 30)
	{
		HINT32 restTime = 60 - DateTime.stTime.ucMinute;
		DateTime.stTime.ucMinute -= (30 - restTime);			// 30min
		DateTime.stTime.ucSecond  = 0;
		DateTime.stTime.usMillisecond = 0;
		HLIB_DATETIME_ConvertDateTimeToUnixTime(&DateTime, &pstContents->unSearchUnitPerHalfHourTime);
		pstContents->unDrawMaxTime = pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM;
	}

#if defined(CONFIG_DEBUG)
{
	HxDATETIME_t	datetime;
	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unSearchUnitPerHalfHourTime, &datetime);
	HxLOG_Error("\nunSearchUnitPerHalfHourTime (%04d.%02d.%02d)(%02d:%02d:%02d.%03d)\n",
				datetime.stDate.usYear, datetime.stDate.ucMonth, datetime.stDate.ucDay,
				datetime.stTime.ucHour, datetime.stTime.ucMinute, datetime.stTime.ucSecond, datetime.stTime.usMillisecond);
}
#endif

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_ReloadData(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, Nx_GuideDir_e eDir)
{
	HINT32			i = 0, j = 0, cur_index = 0, num = 0;
	NX_Channel_t	*channel = NULL;
	HxVector_t		*result = NULL;
	DxService_t		*tempSvc = NULL;
	NxGuideFocusingType_e	eFocusType = pstContents->eLastFocusType;//(NxGuideFocusingType_e)p1;

	// data clear
	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, i);
		if (pstGuideChannel->pstGuideData != NULL)
		{
			NX_APP_Free(pstGuideChannel->pstGuideData);
			pstGuideChannel->pstGuideData = NULL;
		}
		HxSTD_MemSet(pstGuideChannel, 0, sizeof(Nx_GuideChannel_t));

	}

	cur_index = pstContents->nSearchStartChannelIndex;

	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, i);

		if (pstContents->pstChannels != NULL && cur_index != -1)
			channel = &pstContents->pstChannels[cur_index];
		else
			channel = NULL;

		// check OG_GUIDE_COL_NUM 보다 적은 채널을 갖는 경우.
		if (cur_index >= (pstContents->nChannelNum - 1))
		{
			if (pstContents->nChannelNum > OG_GUIDE_COL_NUM)
				cur_index = 0;
			else if (i < (pstContents->nChannelNum - 1))
				cur_index++;
			else
				cur_index = -1;
		}
		else if (i < (pstContents->nChannelNum - 1))
			cur_index++;
		else
			cur_index = -1;

		tempSvc = NULL;
		result = NULL;
		num = 0;

		if (channel != NULL)
			tempSvc = NX_CHANNEL_FindServiceBySvcUid(channel->svcUid);

		result = HLIB_VECTOR_NewEx(NULL, 0,
										(void *(*)(void *))NULL,
										(void *(*)(void *))nx_guide_NxProgrammeItem_Delete,
										NULL);
		if (tempSvc != NULL)
		{
			NX_METADATA_EPG_GetResultProgrammeItemBySvcUid(channel->svcUid, result, &num);
			//HxLOG_Print("\n\n[%s.%d] name (%s)\n\n",__FUNCTION__,__LINE__, tempSvc->name);
			// 기본정보 저장.
			pstGuideChannel->svcUid = channel->svcUid;
			pstGuideChannel->lcn  = channel->lcn;
			pstGuideChannel->pszChannelName = tempSvc->name;
		}

		HxLOG_Debug("(%d) : channel name: %s , NUM(%d)\n", i, pstGuideChannel->pszChannelName, num);

		if (num > 0 && (result != NULL))
		{
			HUINT32	prevItemEndTime = pstContents->unSearchUnitPerHalfHourTime;
			HUINT32 startTime = 0, duration = 0, endTime = 0;
			HINT32	listNum = num;

			Nx_GuideList_t	*list = NULL, *prevlist = NULL;

			for (j = 0; j < num; j++)
			{
				NX_ProgrammeItem_t *item = HLIB_VECTOR_ItemAt(result, j);
				Nx_GuideList_t	*addItem = (Nx_GuideList_t*)NX_APP_Calloc(sizeof(Nx_GuideList_t));
				if (addItem == NULL)
				{
					NX_APP_Error("Error!!! NX_APP_Calloc() Fail !!\n");
					continue;
				}

				startTime 	= SECOND_REMOVE(item->startTime);
				duration	= SECOND_REMOVE(item->duration);
				endTime		= startTime + duration;

				// -------- set item
				addItem->data.startTime	= startTime;
				addItem->data.duration	= duration;

				// event name
				if (item->pszEventName != NULL)
				{
					HxSTD_MemSet(addItem->data.pszEventName, 0, EPG_HUMAX_NAME_LEN);
					HLIB_STD_StrUtf8NCpy(addItem->data.pszEventName, item->pszEventName, EPG_HUMAX_NAME_LEN);
				}

				addItem->data.eventId	= item->eventId;
				addItem->data.svcUid	= item->svcUid;

				// pszduration
				HxSTD_sprintf((char*)addItem->data.pszDuration, (char*)"%s", (char*)item->pszDuration);
				addItem->data.bFocus = FALSE;
				addItem->data.bNoProgramme = FALSE;
				addItem->data.bMidNoProgramme = FALSE;

				// no programme 이 처음에 오는 경우.
				if (j == 0 && (prevItemEndTime < startTime))
				{
					Nx_GuideList_t	*templist = (Nx_GuideList_t*)NX_APP_Calloc(sizeof(Nx_GuideList_t));
					if (templist)
					{
						// set no programme
						listNum += 1;
						templist->data.startTime	= prevItemEndTime;
						templist->data.duration		= startTime - prevItemEndTime;
						//HxLOG_Print("\n\n[%s.%d] first no programme (%d)\n\n",__FUNCTION__,__LINE__, templist->data.duration);
						templist->data.eventId		= -1;
						templist->data.svcUid		= -1;
						templist->data.bNoProgramme	= TRUE;
						templist->data.bFocus		= FALSE;
						if (eDir == eNxGuideDir_NONE)
							HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);
						else
							HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)" ", EPG_HUMAX_NAME_LEN);
						templist->next = NULL;

						addItem->next = NULL;
						templist->next = addItem;

						prevItemEndTime = addItem->data.startTime + addItem->data.duration;

						list 	= templist;
						prevlist= addItem;
					}
				}
				else if (j == 0)		// 맨앞의 데이터 처리
				{
					if ( num <= 1)		// 맨앞이면서 item 이 하나인경우 처리.
					{
						Nx_GuideList_t	*templist = (Nx_GuideList_t*)NX_APP_Calloc(sizeof(Nx_GuideList_t));
						if (templist)
						{
							// set no programme
							listNum += 1;
							templist->data.startTime	= endTime;
							templist->data.duration		= (pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM) - endTime;
							//HxLOG_Print("\n\n[%s.%d] onitem in first no programme (%d)\n\n",__FUNCTION__,__LINE__, templist->data.duration);
							templist->data.eventId		= -1;
							templist->data.svcUid		= -1;
							templist->data.bNoProgramme	= TRUE;
							templist->data.bFocus		= FALSE;
							if (eDir == eNxGuideDir_NONE)
								HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);
							else
								HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)" ", EPG_HUMAX_NAME_LEN);
							templist->next = NULL;

							addItem->next = templist;
							prevlist = templist;
							list = addItem;
							prevItemEndTime = (pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM);
						}
					}
					else
					{
						list 	= addItem;
						addItem->next = NULL;
						prevlist= addItem;
						prevItemEndTime = addItem->data.startTime + addItem->data.duration;
					}
				}
				else if (prevItemEndTime != addItem->data.startTime)	// 중간에 no programme 가 오는 경우.
				{
					Nx_GuideList_t	*templist = (Nx_GuideList_t*)NX_APP_Calloc(sizeof(Nx_GuideList_t));
					if (templist)
					{
						// set no programme
						listNum += 1;
						templist->data.startTime	= prevItemEndTime;
						templist->data.duration		= startTime - prevItemEndTime;
						//HxLOG_Print("\n\n[%s.%d] middle no programme (%d)\n\n",__FUNCTION__,__LINE__, templist->data.duration);
						templist->data.eventId		= -1;
						templist->data.svcUid		= -1;
						templist->data.bNoProgramme	= TRUE;
						templist->data.bMidNoProgramme	= TRUE;
						templist->data.bFocus		= FALSE;
						if (eDir == eNxGuideDir_NONE)
							HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);
						else
							HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)" ", EPG_HUMAX_NAME_LEN);
						templist->next = NULL;

						addItem->next = NULL;
						templist->next	= addItem;
						prevlist->next	= templist;

						prevItemEndTime = addItem->data.startTime + addItem->data.duration;

						prevlist = addItem;
					}
				}
				else if (j == (num-1)	&&	// 마지막 데이터뒤에 no programme 인경우.(단, end unit time 을 넘지 않음)
						 (addItem->data.startTime + addItem->data.duration) < (pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM))
				{
					Nx_GuideList_t	*templist = (Nx_GuideList_t*)NX_APP_Calloc(sizeof(Nx_GuideList_t));
					if (templist)
					{
						// set no programme
						listNum += 1;
						templist->data.startTime	= addItem->data.startTime + addItem->data.duration;
						templist->data.duration		= (pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM) - (addItem->data.startTime + addItem->data.duration);
						//HxLOG_Print("\n\n[%s.%d] last no programme (%d)\n\n",__FUNCTION__,__LINE__, templist->data.duration);
						templist->data.eventId		= -1;
						templist->data.svcUid		= -1;
						templist->data.bNoProgramme	= TRUE;
						templist->data.bFocus		= FALSE;
						if (eDir == eNxGuideDir_NONE)
							HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);
						else
							HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)" ", EPG_HUMAX_NAME_LEN);

						templist->next = NULL;
						addItem->next = templist;
						prevlist->next = addItem;

						prevItemEndTime = templist->data.startTime + templist->data.duration;

						prevlist = templist;
					}
				}
				else	// 일반적인 처리.
				{
					prevItemEndTime = addItem->data.startTime + addItem->data.duration;
					addItem->next = NULL;
					prevlist->next = addItem;
					prevlist = addItem;
				}

			}

			{
				Nx_GuideList_t *tempList = NULL, *freeData = NULL;

				HxLOG_Debug("\n\n\n[%s.%d] listNum : %d . \n\n\n", __FUNCTION__,__LINE__, listNum);
				// copy
				pstGuideChannel->pstGuideData = (NxGuideData_t *)NX_APP_Calloc(sizeof(NxGuideData_t) * listNum);

				tempList = list;
				j = 0;
				while(tempList)
				{
					if (pstGuideChannel->pstGuideData)
					{
						HxSTD_MemCopy(&pstGuideChannel->pstGuideData[j], &tempList->data, sizeof(NxGuideData_t));
					}
					tempList = tempList->next;
					j++;
				}
				tempList = list;
				for (j = 0; j < listNum, tempList != NULL; j++)
				{
					HxLOG_Debug("\n tempList (0x%X) , list(0x%X)\n", tempList, list);
					freeData = tempList;
					tempList = tempList->next;
					if (freeData)
						NX_APP_Free(freeData);
				}
				pstGuideChannel->programmeNum = listNum;
			}

			HLIB_VECTOR_Delete(result);		// new list 만 날리고, core epg 의 data 는 날리면 안됨.
		}
		else
		{
			HxLOG_Debug("channel (%s) programmes data is null...\n", pstGuideChannel->pszChannelName);
			// fill no programme item
			pstGuideChannel->pstGuideData = (NxGuideData_t *)NX_APP_Calloc(sizeof(NxGuideData_t));
			if (pstGuideChannel->pstGuideData)
			{
				pstGuideChannel->programmeNum = 1;
				pstGuideChannel->pstGuideData[0].startTime 	= pstContents->unSearchUnitPerHalfHourTime - 200;
				pstGuideChannel->pstGuideData[0].duration	= OG_2HOUR_SECOND_ALLITEM;//SECONDS_PER_HOUR * 2;
				//HxLOG_Print("\n\n[%s.%d] i(%d) programme data is NULL  no programme (%d)\n\n",__FUNCTION__,__LINE__, i, pstGuideChannel->pstGuideData[0].duration);
				pstGuideChannel->pstGuideData[0].eventId	= -1;
				pstGuideChannel->pstGuideData[0].svcUid		= -1;
				pstGuideChannel->pstGuideData[0].bFocus 	= FALSE;
				pstGuideChannel->pstGuideData[0].bNoProgramme = TRUE;
				if (eDir == eNxGuideDir_NONE)
					HLIB_STD_StrUtf8NCpy(pstGuideChannel->pstGuideData[0].pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);
				else
					HLIB_STD_StrUtf8NCpy(pstGuideChannel->pstGuideData[0].pszEventName, (HCHAR*)" ", EPG_HUMAX_NAME_LEN);
			}
		}
	}

	pstContents->bIsReceivedResult = FALSE;

	HxSTD_MemCopy(&pstContents->stSearchStartChannel, &pstContents->pstChannels[pstContents->nSearchStartChannelIndex], sizeof(NX_Channel_t));

	nx_guide_LoadScheduleAll();
	// update schedule data
	nx_guide_UpdateScheduleData(pstContents);

	if (eFocusType == eGuideFocusType_FIRST)
	{
	}
	else
	if (eFocusType == eGuideFocusType_LAST)
		nx_guide_SetFocusForcedLast(pstContents);
	else
		nx_guide_SetFocus(pstContents);

	pstContents->eLastFocusType = eGuideFocusType_NONE;

	ONDK_GWM_APP_InvalidateObject(NX_GUIDE_PROGRAMMEINFORM_ID);
	//ONDK_GWM_APP_InvalidateObject(NX_GUIDE_CHANNELINFORM_ID);

	return ONDK_RET_OK;
}

static	ONDK_Result_t	nx_guide_SearchStartAll(Nx_GuideContext_t *pstContents, NxGuideSearchType_e eSearchType)
{
	HINT32 i = 0, cur_index = 0;
	HUINT32	unixtime = 0;
	NX_MetadataSearch_t *pstMetaSearch = NULL;

	s_ulGuideCurTick = HLIB_STD_GetSystemTick();
	if ((s_ulGuideCurTick - s_ulGuideLastTick) < NX_GUIDE_SAFETY_SEARCH_TERM_TIME)

		return ONDK_RET_FAIL;
	else
		s_ulGuideLastTick = s_ulGuideCurTick;

	if (eSearchType == eGuideSearchType_ReQuery)
	{
		unixtime = pstContents->unSearchUnitTime - (SECONDS_PER_HOUR * 2);
	}
	else if (eSearchType == eGuideSearchType_Next)
	{
		unixtime = pstContents->unSearchUnitTime - (SECONDS_PER_HOUR * 2);
	}
	else if (eSearchType == eGuideSearchType_Prev)
	{
		unixtime = pstContents->unSearchUnitTime - (SECONDS_PER_HOUR * 6);
	}

	pstContents->unQueryTime = unixtime;

	pstMetaSearch = nx_guide_MakeMetadataSearch(pstContents);

	if (pstMetaSearch == NULL)
	{
		HxLOG_Error("Can't Make NX_MetadataSearch_t. pstMetaSearch is NULL!!\n");
		return ONDK_RET_FAIL;
	}

	cur_index = pstContents->nSearchStartChannelIndex;

	for (i = 0; i < OG_GUIDE_COL_NUM; i++)
	{
		if (cur_index >= pstContents->nChannelNum)
		{
			cur_index = 0;
		}
		HxLOG_Debug("AddChannelConstraint. svcUid(%d), lcn(%d) \n",pstContents->pstChannels[cur_index].svcUid, pstContents->pstChannels[cur_index].lcn);
		NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, &pstContents->pstChannels[cur_index]);
		cur_index++;
	}
	NX_PORT_META_QUERY_SetFilter(pstMetaSearch->context, eNxQUERY_FILTER_ALL);
	NX_METADATA_EPG_SearchEPG(pstMetaSearch);

	return ONDK_RET_OK;
}

static NX_MetadataSearch_t* nx_guide_MakeMetadataSearch(Nx_GuideContext_t *pstContents)
{
	NX_MetadataSearch_t *pstMetaSearch = NULL;
	NX_QUERY_t	endQuery, startQuery, andQuery;
	HCHAR 		value[64] = {'\0',};

	HUINT32 	unixtime = pstContents->unQueryTime;

	HxSTD_sprintf(value, "%u", unixtime);
	endQuery = NX_METADATA_SEARCH_CreateQuery("endTime", ">", value);

	#if defined(CONFIG_DEBUG)
	{
		HxDATETIME_t	datetime;
		HLIB_DATETIME_ConvertUnixTimeToDateTime(unixtime, &datetime);
		HxLOG_Debug("\nMake Query StartTime (%04d.%02d.%02d)(%02d:%02d:%02d.%03d)\n",
					datetime.stDate.usYear, datetime.stDate.ucMonth, datetime.stDate.ucDay,
					datetime.stTime.ucHour, datetime.stTime.ucMinute, datetime.stTime.ucSecond, datetime.stTime.usMillisecond);
	}
	#endif

	unixtime += SECONDS_PER_HOUR * 8; // plus 4 hour
	HxSTD_sprintf(value, "%u", unixtime);
	startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);

	#if defined(CONFIG_DEBUG)
	{
		HxDATETIME_t	datetime;
		HLIB_DATETIME_ConvertUnixTimeToDateTime(unixtime, &datetime);
		HxLOG_Debug("Make Query EndTime   (%04d.%02d.%02d)(%02d:%02d:%02d.%03d)\n\n",
					datetime.stDate.usYear, datetime.stDate.ucMonth, datetime.stDate.ucDay,
					datetime.stTime.ucHour, datetime.stTime.ucMinute, datetime.stTime.ucSecond, datetime.stTime.usMillisecond);
	}
	#endif

	andQuery = NX_PORT_META_QUERY_AND(endQuery, startQuery);

	NX_PORT_META_QUERY_DeleteQuery(endQuery);
	NX_PORT_META_QUERY_DeleteQuery(startQuery);

	pstMetaSearch = NX_METADATA_SEARCH_New(eNX_META_SEARCH_TARGET_SCHEDULED);
	if (pstMetaSearch == NULL)
	{
		return NULL;
	}
	NX_PORT_META_QUERY_SetQuery(pstMetaSearch->context, andQuery);
	NX_PORT_META_QUERY_DeleteQuery(andQuery);

	return pstMetaSearch;
}

static	ONDK_Result_t nx_guide_SetFocus(Nx_GuideContext_t *pstContents)
{
	HINT32	i = 0;
	HINT32 nFocusChannelIndex = pstContents->nFocusChannelIndex;
	Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, nFocusChannelIndex);
	HINT32 programmeNum = 0;
	HBOOL	bFoundFocus = FALSE;

	if (pstGuideChannel == NULL)
	{
		return ONDK_RET_FAIL;
	}

	programmeNum = pstGuideChannel->programmeNum;

	for (i = 0; i < programmeNum; i++)
	{
		NxGuideData_t *pstItem = &pstGuideChannel->pstGuideData[i];

		if (pstItem != NULL)
		{
			HxLOG_Debug("\n\n===>>i(%d), pstItem : (%s)\n\n", i, pstItem->pszEventName);
			if ((pstItem->startTime + pstItem->duration) > pstContents->unSearchUnitPerHalfHourTime)
			{
				// 기존 focus 와 비교..  no programme 은 제외
				if ((pstItem->eventId == pstContents->stFocusData.eventId)
					&& (!pstItem->bNoProgramme || pstItem->bMidNoProgramme))
				{
					pstItem->bFocus = TRUE;
					pstContents->stFocusData.bFocus = TRUE;
					pstContents->stFocusData.eventId = pstItem->eventId;
					pstContents->stFocusData.nIndexArray = i;
					bFoundFocus = TRUE;
					break;
				}
			}
		}
	}

	if (bFoundFocus == FALSE)
	{
		Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, pstContents->nFocusChannelIndex);

		for (i = 0; i < pstGuideChannel->programmeNum; i++)
		{
			NxGuideData_t *pstItem = &pstGuideChannel->pstGuideData[i];

			if ((pstItem->startTime < (pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM))
				&&((pstItem->startTime + pstItem->duration) > pstContents->unSearchUnitPerHalfHourTime))
			{
				pstItem->bFocus = TRUE;
				pstContents->stFocusData.bFocus = TRUE;
				pstContents->stFocusData.nIndexArray = i;
				pstContents->stFocusData.eventId = pstItem->eventId;
				bFoundFocus = TRUE;
				break;
			}
		}
		if (bFoundFocus == FALSE)
		{
			NxGuideData_t *pstItem = &pstGuideChannel->pstGuideData[0];
			pstItem->bFocus = TRUE;
			pstContents->stFocusData.bFocus = TRUE;
			pstContents->stFocusData.nIndexArray = i;
			pstContents->stFocusData.eventId = pstItem->eventId;
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t 	nx_guide_SetFocusForcedLast(Nx_GuideContext_t *pstContents)
{
	HINT32	i = 0;
	HINT32 nFocusChannelIndex = pstContents->nFocusChannelIndex;
	Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, nFocusChannelIndex);
	HUINT32 ulEndSearchUnitTime = pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM;
	HINT32 programmeNum = 0;
	HBOOL	bFoundFocus = FALSE;

	if (pstGuideChannel == NULL)
	{
		return ONDK_RET_FAIL;
	}

	programmeNum = pstGuideChannel->programmeNum;

	// forced last 의 경우, 호출 함수에서 clean focus 를 하고온다.

	// 아래는, epg search all 을 다시 타면서, 이미 설정된 focus 를 다시 맨뒤로 설정하는것을 방지하기 위해.
	if (pstContents->stFocusData.bFocus == TRUE)
	{
		for (i = programmeNum-1; i >= 0; i--)		// 뒤에서부터..
		{
			NxGuideData_t *pstItem = &pstGuideChannel->pstGuideData[i];

			if (pstContents->stFocusData.eventId == pstItem->eventId)
			{
				pstItem->bFocus = TRUE;
				pstContents->stFocusData.bFocus = TRUE;
				pstContents->stFocusData.eventId = pstItem->eventId;
				pstContents->stFocusData.nIndexArray = i;
				bFoundFocus = TRUE;
				break;
			}
		}
	}
	else
	{
		// prev, 나 backward 에서 forced last 를 하는경우 처음 이곳에서 설정되어짐.
		for (i = programmeNum-1; i >= 0; i--)		// 뒤에서부터..
		{
			NxGuideData_t *pstItem = &pstGuideChannel->pstGuideData[i];

			if (pstItem != NULL)
			{
				HxLOG_Debug("\n\n===>>i(%d), pstItem : (%s)\n\n", i, pstItem->pszEventName);
				if ( ((pstItem->startTime + pstItem->duration) >= ulEndSearchUnitTime)
					&&(pstItem->startTime < ulEndSearchUnitTime))
				{
					pstItem->bFocus = TRUE;
					pstContents->stFocusData.bFocus = TRUE;
					pstContents->stFocusData.eventId = pstItem->eventId;
					pstContents->stFocusData.nIndexArray = i;
					bFoundFocus = TRUE;
					break;
				}
			}
		}
	}

	if (bFoundFocus == FALSE)
	{
		Nx_GuideChannel_t *pstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, pstContents->nFocusChannelIndex);

		// 처리가 안된경우, 일단 뒤에서부터 처음 해당되는 아이템으로 설정.
		for (i = programmeNum - 1; i >= 0; i--)
		{
			NxGuideData_t *pstItem = &pstGuideChannel->pstGuideData[i];

			if (pstItem->startTime < ulEndSearchUnitTime)
			{
				pstItem->bFocus = TRUE;
				pstContents->stFocusData.bFocus = TRUE;
				pstContents->stFocusData.nIndexArray = i;
				pstContents->stFocusData.eventId = pstItem->eventId;
				bFoundFocus = TRUE;
				break;
			}
		}

		// 그래도 못찾으면 맨뒤 아이템으로 설정.
		if (bFoundFocus == FALSE)
		{
			NxGuideData_t *pstItem = &pstGuideChannel->pstGuideData[programmeNum-1];
			pstItem->bFocus = TRUE;
			pstContents->stFocusData.bFocus = TRUE;
			pstContents->stFocusData.nIndexArray = i;
			pstContents->stFocusData.eventId = pstItem->eventId;
		}
	}

	return ERR_OK;
}

static void 	nx_guide_FillBlankGuideData(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel)
{
	// fill no programme item
	pstGuideChannel->pstGuideData = (NxGuideData_t *)NX_APP_Calloc(sizeof(NxGuideData_t));
	pstGuideChannel->programmeNum = 1;
	pstGuideChannel->pstGuideData[0].startTime 	= pstContents->unSearchUnitPerHalfHourTime;
	pstGuideChannel->pstGuideData[0].duration	= OG_2HOUR_SECOND_ALLITEM;
	pstGuideChannel->pstGuideData[0].eventId	= -1;
	pstGuideChannel->pstGuideData[0].svcUid		= -1;
	pstGuideChannel->pstGuideData[0].bFocus 	= FALSE;
	pstGuideChannel->pstGuideData[0].bNoProgramme = TRUE;
	HLIB_STD_StrUtf8NCpy(pstGuideChannel->pstGuideData[0].pszEventName, //" ", EPG_HUMAX_NAME_LEN);
					(HCHAR*)" "/*ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID)*/, EPG_HUMAX_NAME_LEN);
}

static void 	nx_guide_FillBlankGuideChannel(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel, HINT32 nChannelIndex)
{
	DxService_t		*tempSvc = NULL;
	NX_Channel_t 	*channel = NULL;

	channel = &pstContents->pstChannels[nChannelIndex];
	tempSvc = NX_CHANNEL_FindServiceBySvcUid(channel->svcUid);

	pstGuideChannel->svcUid		= channel->svcUid;
	pstGuideChannel->lcn		= channel->lcn;
	pstGuideChannel->pszChannelName = tempSvc->name;

	nx_guide_FillBlankGuideData(pstContents, pstGuideChannel);
}

static void		nx_guide_RemoveGuideChannelItem(Nx_GuideContext_t *pstContents, Nx_GuideDir_e eDir)
{
	if (eDir == eNxGuideDir_DOWN)
	{
		HINT32 nIndex = 0;

		Nx_GuideChannel_t* pstGuideChannel = (Nx_GuideChannel_t*)NX_APP_Calloc(sizeof(Nx_GuideChannel_t));

		HLIB_VECTOR_Remove (pstContents->guideChannelsList, 0);
		// 이미 search start 가 한개 증가 한후 온것
		nIndex = pstContents->nSearchStartChannelIndex + OG_CH_LAST_PREV_NUM;
		if (nIndex >= pstContents->nChannelNum)
			nIndex = nIndex - pstContents->nChannelNum;
		nx_guide_FillBlankGuideChannel(pstContents, pstGuideChannel, nIndex);

		HLIB_VECTOR_Add(pstContents->guideChannelsList, pstGuideChannel);
	}
	else if (eDir == eNxGuideDir_UP)
	{
		HINT32 nIndex = 0;
		Nx_GuideChannel_t* firstGuideChannel = NULL;
		Nx_GuideChannel_t* pstGuideChannel = (Nx_GuideChannel_t*)NX_APP_Calloc(sizeof(Nx_GuideChannel_t));
		HLIB_VECTOR_Remove (pstContents->guideChannelsList, OG_GUIDE_COL_NUM - 1);

		nIndex = pstContents->nSearchStartChannelIndex;
		nx_guide_FillBlankGuideChannel(pstContents, pstGuideChannel, nIndex);

		firstGuideChannel = (Nx_GuideChannel_t*)HLIB_VECTOR_ItemAt(pstContents->guideChannelsList, 0);
		HLIB_VECTOR_InsertBefore(pstContents->guideChannelsList, firstGuideChannel, pstGuideChannel);
	}
}

static HINT32	nx_guide_GetCollisionSize(Nx_GuideContext_t *pstContents, NxGuideData_t *pstCurItem,	NxGuideData_t *pstCompareItem)
{
	//HINT32	nCurItemLen = 0, nCompareItemLen = 0;
	HUINT32	ulCurStartTime 		= pstCurItem->startTime;
	HUINT32 ulCurDuration 		= pstCurItem->duration;
	HUINT32	ulCompareStartTime 	= pstCompareItem->startTime;
	HUINT32	ulCompareDuration 	= pstCompareItem->duration;
	HUINT32	ulHalfHourUnitTime	= pstContents->unSearchUnitPerHalfHourTime;
	HUINT32	ulHalfHourEndUnitTime = ulHalfHourUnitTime + OG_2HOUR_SECOND_ALLITEM;

	HUINT32	ulCurItemLen = 0, ulCompareItemLen = 0;

	if (ulCurStartTime < ulHalfHourUnitTime)	// 맨앞의 item 의 startTime 이 unit time 보다 작은경우 처리.
	{
		ulCurStartTime = ulHalfHourUnitTime;
		ulCurDuration = ulCurDuration - (ulHalfHourUnitTime - pstCurItem->startTime);
	}

	if (ulCompareStartTime < ulHalfHourUnitTime)
	{
		ulCompareStartTime = ulHalfHourUnitTime;
		ulCompareDuration = ulCompareDuration - (ulHalfHourUnitTime - pstCompareItem->startTime);
	}

	if ((ulCurStartTime + ulCurDuration) > ulHalfHourEndUnitTime) // end unit time 을 넘어가는 경우 처리
	{
		ulCurDuration = ulCurDuration - ((ulCurStartTime + ulCurDuration) - ulHalfHourEndUnitTime);
	}

	if ((ulCompareStartTime + ulCompareDuration) > ulHalfHourEndUnitTime)
	{
		ulCompareDuration = ulCompareDuration - ((ulCompareStartTime + ulCompareDuration) - ulHalfHourEndUnitTime);
	}

	ulCurItemLen 	= ulCurStartTime + ulCurDuration;
	ulCompareItemLen= ulCompareStartTime + ulCompareDuration;

	if (ulCurStartTime >= ulCompareStartTime)
	{
		if (ulCurItemLen < ulCompareItemLen)
			return (HINT32)ulCurDuration;
		else
			return (HINT32)(ulCompareItemLen - ulCurStartTime);
	}
	else
	{
		if (ulCurItemLen > ulCompareStartTime)
		{
			if (ulCurItemLen > ulCompareItemLen)
				return (HINT32)ulCompareDuration;
			else
				return (HINT32)(ulCurItemLen - ulCompareStartTime);
		}
		else
			return 0;
	}
}

static HBOOL nx_guide_IsFirstItem(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel, NxGuideData_t *pstCurItem)
{
	NxGuideData_t *pstGuideItem = NULL;
	HINT32 	i;
	HUINT32	ulHalfHourUnitTime	= pstContents->unSearchUnitPerHalfHourTime;

	for (i = 0; i < pstGuideChannel->programmeNum; i++)
	{
		pstGuideItem = (NxGuideData_t*)&pstGuideChannel->pstGuideData[i];

		// 최초로 보이는 아이템을 찾아서... 처리.
		if ((pstGuideItem->startTime + pstGuideItem->duration) > ulHalfHourUnitTime)
		{
			if ((pstCurItem->eventId == pstGuideItem->eventId)
				&& (pstCurItem->startTime == pstGuideItem->startTime))
			{
				return TRUE;
			}
			else
			{
				// 최초로 표시되는 아이템 비교해서 같지 않다면 first 아이템이 아니다.
				return FALSE;
			}
		}
	}
	return FALSE;
}

static HERROR nx_guide_GetFirstItemOfChannel(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel, HINT32 *pnFirstIndex)
{
	NxGuideData_t *pstGuideItem = NULL;
	HINT32 i = 0;
	HUINT32	ulHalfHourUnitTime	= pstContents->unSearchUnitPerHalfHourTime;

	for (i = 0; i < pstGuideChannel->programmeNum; i++)
	{
		pstGuideItem = (NxGuideData_t*)&pstGuideChannel->pstGuideData[i];

		// 최초로 보이는 아이템을 찾아서... 처리.
		if ((pstGuideItem->startTime + pstGuideItem->duration) > ulHalfHourUnitTime)
		{
			*pnFirstIndex 	= i;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}

static HBOOL nx_guide_IsLastItem(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel, NxGuideData_t *pstCurItem)
{
	NxGuideData_t *pstGuideItem = NULL;
	HINT32 	i;
	HUINT32 ulEndSearchUnitTime = pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM;

	for (i = pstGuideChannel->programmeNum - 1; i >= 0; i--)
	{
		pstGuideItem = &pstGuideChannel->pstGuideData[i];

		if (pstGuideItem->startTime < ulEndSearchUnitTime)
		{
			if ((pstCurItem->eventId == pstGuideItem->eventId)
				&& (pstCurItem->startTime == pstGuideItem->startTime))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	return FALSE;
}

static HERROR nx_guide_GetLastItemOfChannel(Nx_GuideContext_t *pstContents, Nx_GuideChannel_t *pstGuideChannel, HINT32 *pnLastIndex)
{
	NxGuideData_t *pstGuideItem = NULL;
	HINT32 i = 0;
	HUINT32 ulEndSearchUnitTime = pstContents->unSearchUnitPerHalfHourTime + OG_2HOUR_SECOND_ALLITEM;

	for (i = pstGuideChannel->programmeNum - 1; i >= 0; i--)
	{
		pstGuideItem = &pstGuideChannel->pstGuideData[i];

		if (pstGuideItem->startTime < ulEndSearchUnitTime)
		{
			*pnLastIndex 	= i;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}


#define ____________RELATED__IMAGE_RES___________
static	void nx_guide_CreateImageResSurface(Nx_GuideContext_t *pstContext)
{
	ONDKImageHandle hGridImg = NULL;
	ONDKImageHandle hDimImg = NULL;
	ONDKImageHandle hTempImg = NULL;

	if (s_hImageResSurface == NULL)
	{
		s_hImageResSurface = ONDK_SURFACE_Create(OG_IMAGE_SURFACE_W, OG_IMAGE_SURFACE_H, "ARGB8888");
		ONDK_SURFACE_Clear(s_hImageResSurface);
	}

	hGridImg = ONDK_IMG_CreateSelf((HUINT8*)RES_380_00_EPG_GRID_1L_PNG);
	ONDK_IMG_Blit(hGridImg, s_hImageResSurface, OG_IMG_RES_GRID_IMG_1L_X, OG_IMG_RES_GRID_IMG_Y);
	ONDK_IMG_Destroy(hGridImg);

	hGridImg = ONDK_IMG_CreateSelf((HUINT8*)RES_380_00_EPG_GRID_2C_PNG);
	ONDK_IMG_Blit(hGridImg, s_hImageResSurface, OG_IMG_RES_GRID_IMG_2C_X, OG_IMG_RES_GRID_IMG_Y);
	ONDK_IMG_Destroy(hGridImg);

	hGridImg = ONDK_IMG_CreateSelf((HUINT8*)RES_380_00_EPG_GRID_3R_PNG);
	ONDK_IMG_Blit(hGridImg, s_hImageResSurface, OG_IMG_RES_GRID_IMG_3R_X, OG_IMG_RES_GRID_IMG_Y);
	ONDK_IMG_Destroy(hGridImg);


	hDimImg = ONDK_IMG_CreateSelf((HUINT8*)RES_105_00_BG_EPG_CELL_L_PNG);
	ONDK_IMG_Blit(hDimImg, s_hImageResSurface, OG_IMG_RES_DIM_LCELL_X, OG_IMG_RES_DIM_LCELL_Y);
	ONDK_IMG_Destroy(hDimImg);

	hDimImg = ONDK_IMG_CreateSelf((HUINT8*)RES_105_00_BG_BOTTOM_90_PNG);
	ONDK_IMG_Blit(hDimImg, s_hImageResSurface, OG_IMG_RES_DIM_BOTTOM_X, OG_IMG_RES_DIM_BOTTOM_Y);
	ONDK_IMG_Destroy(hDimImg);

	hTempImg = ONDK_IMG_CreateSelf((HUINT8*)RES_205_00_RESERVATION_WATCH_PNG);
	ONDK_IMG_Blit(hTempImg, s_hImageResSurface, OG_IMG_RES_RSV_WATCH_X, OG_IMG_RES_RSV_WATCH_Y);
	ONDK_IMG_Destroy(hTempImg);

#if defined(CONFIG_MW_MM_PVR)
	hTempImg = ONDK_IMG_CreateSelf((HUINT8*)RES_205_04_REC_RESERVATION_RECORDING_PNG);
	ONDK_IMG_Blit(hTempImg, s_hImageResSurface, OG_IMG_RES_RSV_REC_X, OG_IMG_RES_RSV_REC_Y);
	ONDK_IMG_Destroy(hTempImg);
#endif
}

inline static void nx_guide_BltGridImg(ONDKSurfaceHandle dstSurface, ONDK_Rect_t dstRect)
{
	ONDK_Rect_t	drawRect, drawStrcRect;

	ONDK_SURFACE_Blitflag_SetBlend(dstSurface);
	drawRect = ONDK_Rect(OG_IMG_RES_GRID_IMG_1L_X, OG_IMG_RES_GRID_IMG_Y, OG_IMG_RES_GRID_IMG_1L_W, OG_IMG_RES_GRID_IMG_H);

	ONDK_SURFACE_BlitWithoutBlend(s_hImageResSurface, dstSurface, drawRect, dstRect.x, dstRect.y);

	drawRect = ONDK_Rect(OG_IMG_RES_GRID_IMG_2C_X, OG_IMG_RES_GRID_IMG_Y, OG_IMG_RES_GRID_IMG_2C_W, OG_IMG_RES_GRID_IMG_H);

	drawStrcRect = ONDK_Rect((dstRect.x + OG_IMG_RES_GRID_IMG_1L_W), dstRect.y, (dstRect.w - OG_IMG_RES_GRID_IMG_1L_3R_W), dstRect.h);

	if (drawStrcRect.w > 0)
		ONDK_SURFACE_StretchBlitWithoutBlend(s_hImageResSurface, dstSurface, drawRect, drawStrcRect);

	drawRect = ONDK_Rect(OG_IMG_RES_GRID_IMG_3R_X, OG_IMG_RES_GRID_IMG_Y, OG_IMG_RES_GRID_IMG_3R_W, OG_IMG_RES_GRID_IMG_H);

	ONDK_SURFACE_BlitWithoutBlend(s_hImageResSurface, dstSurface, drawRect, (dstRect.x + dstRect.w - OG_IMG_RES_GRID_IMG_3R_W), dstRect.y);

}

static void nx_guide_BltRsvWatchImg(ONDKSurfaceHandle dstSurface, ONDK_Rect_t dstRect)
{
	ONDK_Rect_t	drawRect;

	ONDK_SURFACE_Blitflag_SetBlend(dstSurface);
	drawRect = ONDK_Rect(OG_IMG_RES_RSV_WATCH_X, OG_IMG_RES_RSV_WATCH_Y, OG_IMG_RES_RSV_WATCH_W, OG_IMG_RES_RSV_WATCH_H);

	ONDK_SURFACE_BlitWithoutBlend(s_hImageResSurface, dstSurface, drawRect, dstRect.x, dstRect.y);
}

static void nx_guide_BltRsvRecImg(ONDKSurfaceHandle dstSurface, ONDK_Rect_t dstRect)
{
	ONDK_Rect_t	drawRect;

	ONDK_SURFACE_Blitflag_SetBlend(dstSurface);
	drawRect = ONDK_Rect(OG_IMG_RES_RSV_REC_X, OG_IMG_RES_RSV_REC_Y, OG_IMG_RES_RSV_REC_W, OG_IMG_RES_RSV_REC_H);

	ONDK_SURFACE_BlitWithoutBlend(s_hImageResSurface, dstSurface, drawRect, dstRect.x, dstRect.y);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
