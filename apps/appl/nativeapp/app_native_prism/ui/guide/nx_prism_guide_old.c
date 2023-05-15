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

#define	NX_VALID_UNIXTIME						(60 * 60 * 24 * 366)			// until 1971/01/02

#define	NX_GUIDE_TIMEOUT_ID						0xf000
#define	NX_GUIDE_TIMEOUT						(3*60*NX_COMMON_TIME_SECOND)	/* 3 minutes */
#define	NX_GUIDE_OBJECT_ID						(NX_PRISM_GUIDE_ITEM_ID)

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

#define	NX_GUIDE_QUERY_TIMEOUT_ID				0xf010
#define	NX_GUIDE_QUERY_TIMEOUT					200

#define	NX_PRISM_GUIDE_SHORT_STR_LEN			64
#define	NX_PRISM_GUIDE_DEF_STR_LEN				128
#define	NX_PRISM_GUIDE_LONG_STR_LEN				512

#define	NX_PRISM_GUIDE_QUERY_NUM				8

#define	NX_PRISM_GUIDE_OPTION_LIST_Y2			673


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum
{
	eGuideSearchType_Pf,
	eGuideSearchType_Prev,
	eGuideSearchType_Next,
	eGuideSearchType_Down,
	eGuideSearchType_Up,
	eGuideSearchType_PageDown,
	eGuideSearchType_PageUp,
}NxGuideSearchType_e;

typedef struct
{
	HCHAR	pszChDis[NX_PRISM_GUIDE_SHORT_STR_LEN];
	HCHAR	pszEpisodeNumber[EPG_HUMAX_NAME_LEN];
	HCHAR 	pszLongDes[EPG_HUMAX_DETAIL_LEN];
	HCHAR	pszGenres[NX_PRISM_GUIDE_DEF_STR_LEN];
}NxGuideDetailData_t;

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
}NxGuideData_t;

typedef struct tagGuideChannel {
	HUINT32	svcUid;
	HINT32	lcn;
	HINT32	programmeNum;
	HCHAR	*pszChannelName;
	NxGuideData_t *pstGuideData;

	HBOOL	bPfFocus;
} Nx_GuideChannel_t;

typedef struct _Nx_GuideList_t
{
	NxGuideData_t	data;
	struct _Nx_GuideList_t	*next;
} Nx_GuideList_t;


typedef struct tagGuideContext {
	NX_ChListGroup_t	eCurGroupType;
	HINT32	nChannelNum;
	HINT32	nCurChannelIndex;				// channel list 에서 리스트의 channel 목록 유지 및 처리를 위해.
	NX_Channel_t *pstChannels;				// live proc 에서 얻어옴..
	HUINT32	unCurrentTime;					// 현재 시간 저장.
	HUINT32	unStandardStartTime;			// 최초 시작 time 저장.
	HUINT32	unSearchUnitTime;				// create 시 search 시작 시간을 기준으로 2시간 단위로 start time 저장.
	HUINT32	unSearchUnitPerHalfHourTime;		// unSearchUnitTime 에서 시간 단위로 게산된 시간
	Nx_GuideChannel_t	pstGuideChannels[NX_PRISM_GUIDE_QUERY_NUM];
	NxGuideData_t	*pstCurFocusData;
	HBOOL	bPfSearched;
	NxGuideSearchType_e	eLastSearchType;
	HBOOL	bIsReceivedResult;

	HUINT32 ulSearchUnixtime;

	HINT32	nSearchStartChannelIndex;	// search 기준이 되는 channel index. up, down 시 가감됨.
	HINT32	nFocusChannelIndex;			// up, down 시 epg 표시 채널 갯수내에서 가감됨.
	HINT32	nFocusItemIndex;		// right, left

	// Option Item
	HCHAR		szOptionText[NX_TEXT_SIZE_64];
	HUINT8		**szItemTextList;
	HUINT8		**szImageNItemTextList;
	HUINT8		**szImageCItemTextList;
	NX_Util_OPTIONList_t	stOptList;
} Nx_GuideContext_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_GuideContext_t		s_stGuideContent;
static Nx_PrismListDialog_Content_t		s_stGuideOptContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_guide_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_guide_DestroyListPopup(void);
static void				nx_guide_CreateListPopup(Nx_GuideContext_t *pstContents);

static ONDK_Result_t	nx_guide_OptionChangedItemValue(Nx_GuideContext_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_guide_OptionInitData(Nx_GuideContext_t *pstContents);

static inline Nx_GuideContext_t	*nx_guide_GetContents(void);
static void				nx_guide_DrawBackground(void);
static void				nx_guide_SetUiCreateOptionMenu(Nx_GuideContext_t *pstContents);
static void				nx_guide_Settings_DrawTitle(HUINT8 *pTitle);
static ONDK_Result_t	nx_guide_DrawDefaultUI(Nx_GuideContext_t *pstContents);

static ONDK_Result_t	nx_guide_MsgGwmCreate(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_guide_MsgGwmDestroy(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_guide_MsgGwmHapiMsg(Nx_GuideContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_guide_MsgGwmTimer(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_guide_SearchStart(Nx_GuideContext_t *pstContents, NxGuideSearchType_e eSearchType);
static HBOOL 			nx_guide_CheckSubProc(void);
static void				nx_guide_SetJumpToDate(Nx_GuideContext_t *pstContents, HxDATETIME_t *pRcvJumpDateTime);
static HINT32			nx_guide_GetCollisionSize(Nx_GuideContext_t *pstContents, NxGuideData_t *pstCurItem,	NxGuideData_t *pstCompareItem);
static ONDK_Result_t 	nx_guide_getData(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_guide_MsgGwmKeyDown(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_guide_CellPrev(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CellNext(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CellUp(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CellDown(Nx_GuideContext_t *pstContents);
static void				nx_guide_PageUp(Nx_GuideContext_t *pstContents);
static void				nx_guide_PageDown(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_SetFocusOnChangedEvent(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CleanFocusInPf(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CleanFocusInChannel(Nx_GuideContext_t *pstContents, HINT32 nFocusChannelIndex);
static ONDK_Result_t	nx_guide_CleanFocusAll(Nx_GuideContext_t *pstContents);
static ONDK_Result_t	nx_guide_CalcUnitTimePerHalfHour(Nx_GuideContext_t *pstContents);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_guide_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stGuideOptContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_guide_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_guide_DisplayListPopup);
}


static void		nx_guide_CreateListPopup(Nx_GuideContext_t *pstContents)
{
	HINT32		y = 0, h = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0;
	HUINT8		**pptextList = NULL, **ppImageListOfItem = NULL;
	ONDK_Rect_t	stRect;


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


static ONDK_Result_t	nx_guide_OptionChangedItemValue(Nx_GuideContext_t *pstContents, HUINT32 ulCurIdx)
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
		case eNxUtil_OptItem_Group:
			break;
		case eNxUtil_OptItem_Sort:
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
		default:
			NX_APP_Info("Error!!! Invalid Option item:[%d]!!\n", eOptItem);
			break;
	}

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_guide_OptionInitData(Nx_GuideContext_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	pstContents->szItemTextList = NULL;
	pstContents->szImageNItemTextList = NULL;
	pstContents->szImageCItemTextList = NULL;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Group);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Sort);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_GenreSearch);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_JumpDate);
			break;
	}

	// TODO: Add Project config if you want to do

	return	ONDK_RET_OK;
}


static inline Nx_GuideContext_t	*nx_guide_GetContents(void)
{
	return	&s_stGuideContent;
}

static void		nx_guide_DrawBackground(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	/* background start */
	stRect = ONDK_Rect(NX_PRISM_UI_BACKGROUND_BOX_X, NX_PRISM_UI_BACKGROUND_BOX_Y,
						NX_PRISM_UI_BACKGROUND_BOX_WIDTH, NX_PRISM_UI_BACKGROUND_BOX_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_BACKGROUND_BOX_ID, &stRect, COL(C_G_Bang_80));
}

static void		nx_guide_SetUiCreateOptionMenu(Nx_GuideContext_t *pstContents)
{
	ONDK_Rect_t		stRect;

	/* Draw OPTION BG Frame */
	stRect = ONDK_Rect(NX_PRISM_GUIDE_OPTION_FRAME_X, NX_PRISM_GUIDE_OPTION_FRAME_Y,
						NX_PRISM_GUIDE_OPTION_FRAME_W, NX_PRISM_GUIDE_OPTION_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_GUIDE_OPTION_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_GUIDE_OPTION_FRAME_ID, NX_PRISM_COMP_DrawFrameBottomDim);

	stRect = ONDK_Rect(NX_PRISM_GUIDE_OPTION_TEXT_X, NX_PRISM_GUIDE_OPTION_TEXT_Y,
						NX_PRISM_GUIDE_OPTION_TEXT_W, NX_PRISM_GUIDE_OPTION_TEXT_H);
	// TODO: [JHLEE] Get Current Group
	HxSTD_sprintf(pstContents->szOptionText, "%s - %s", (HCHAR*)ONDK_GetString(RES_OPTION_01_ID), (HCHAR*)"TV");
	ONDK_GWM_COM_Multi_Create(NX_PRISM_GUIDE_OPTION_TEXT_ID, &stRect);
	ONDK_GWM_COM_Multi_SetContents(NX_PRISM_GUIDE_OPTION_TEXT_ID, (HUINT8*)RES_201_00_OPTION_PNG,
								NULL, (HUINT8*)pstContents->szOptionText, NULL, NX_PRISM_GUIDE_OPTION_IMG_MG);
	ONDK_GWM_COM_Multi_SetFontHandle(NX_PRISM_GUIDE_OPTION_TEXT_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Multi_SetFontColor(NX_PRISM_GUIDE_OPTION_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));
	ONDK_GWM_COM_Multi_SetFocusFlag(NX_PRISM_GUIDE_OPTION_TEXT_ID, FALSE);
}


static void		nx_guide_Settings_DrawTitle(HUINT8 *pTitle)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_PRISM_UI_SET_TITLE_X, NX_PRISM_UI_SET_TITLE_Y, NX_PRISM_UI_SET_TITLE_WIDTH, NX_PRISM_UI_SET_TITLE_HEIGHT);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_SET_TITLE_ID, &stRect, (HCHAR*)pTitle);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_SET_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_SET_TITLE_ID, NX_PRISM_FONT_SIZE_46);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_SET_TITLE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_SET_TITLE_ID, COL(C_T_Gong01), COL(C_T_Gong01));
}


static ONDK_Result_t	nx_guide_DrawDefaultUI(Nx_GuideContext_t *pstContents)
{
	// Draw - Background - Black_80
	nx_guide_DrawBackground();

	// Draw - Title
//	NX_PRISM_COMP_Settings_DrawTitle(ONDK_GetString(RES_GENERAL_SETTINGS_ID));
	//nx_guide_Settings_DrawTitle("-GUIDE- under construction");

	ONDK_GWM_COM_TvGuide_Create(NX_GUIDE_OBJECT_ID, 0, 0);
	ONDK_GWM_COM_TvGuide_SetGrid1LImg(NX_GUIDE_OBJECT_ID, RES_380_00_EPG_GRID_1L_PNG);
	ONDK_GWM_COM_TvGuide_SetGrid2CImg(NX_GUIDE_OBJECT_ID, RES_380_00_EPG_GRID_2C_PNG);
	ONDK_GWM_COM_TvGuide_SetGrid3RImg(NX_GUIDE_OBJECT_ID, RES_380_00_EPG_GRID_3R_PNG);
	ONDK_GWM_COM_TvGuide_SetGridDimLImg(NX_GUIDE_OBJECT_ID, RES_105_00_BG_EPG_CELL_L_PNG);
	ONDK_GWM_COM_TvGuide_SetGridDimRImg(NX_GUIDE_OBJECT_ID, RES_105_00_BG_EPG_CELL_R_PNG);
	ONDK_GWM_COM_TvGuide_SetEpgStringData(NX_GUIDE_OBJECT_ID, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID),
											(HCHAR*)ONDK_GetString(RES_ON_AIR_ID), (HCHAR*)ONDK_GetString(RES_NEXT_02_ID));
	ONDK_GWM_Obj_SetVisible(NX_GUIDE_OBJECT_ID, TRUE);
	ONDK_GWM_APP_InvalidateObject(NX_GUIDE_OBJECT_ID);

	nx_guide_SetUiCreateOptionMenu(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_guide_QuerySearchStart(Nx_GuideContext_t *pstContents)
{
	HINT32 i = 0, cur_index = 0;
	NX_MetadataSearch_t *pstMetaSearch = NULL;
	NX_QUERY_t	endQuery, startQuery, andQuery;
	HCHAR 		value[64] = {'\0',};

	HUINT32 	unixtime = pstContents->ulSearchUnixtime;

	cur_index = pstContents->nSearchStartChannelIndex;

	HxSTD_sprintf(value, "%u", unixtime);
	endQuery = NX_METADATA_SEARCH_CreateQuery("endTime", ">", value);

	unixtime += SECONDS_PER_HOUR * 4; // plus 4 hour
	HxSTD_sprintf(value, "%u", unixtime);
	startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);

	andQuery = NX_PORT_META_QUERY_AND(endQuery, startQuery);

	NX_PORT_META_QUERY_DeleteQuery(endQuery);
	NX_PORT_META_QUERY_DeleteQuery(startQuery);

	pstMetaSearch = NX_METADATA_SEARCH_New(eNX_META_SEARCH_TARGET_SCHEDULED);
	if (pstMetaSearch == NULL)
	{
		return ERR_FAIL;
	}
	NX_PORT_META_QUERY_SetQuery(pstMetaSearch->context, andQuery);
	NX_PORT_META_QUERY_DeleteQuery(andQuery);

	for (i = 0; i < NX_PRISM_GUIDE_QUERY_NUM; i++)
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

	//pstContents->bIsReceivedResult = FALSE;
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_SearchStart(Nx_GuideContext_t *pstContents, NxGuideSearchType_e eSearchType)
{
//	HINT32 i = 0;
	HINT32 cur_index = 0;
//	NX_MetadataSearch_t *pstMetaSearch = NULL;
//	NX_QUERY_t	endQuery, startQuery, andQuery;
	HUINT32 	unixtime = 0;
//	HCHAR 		value[64] = {'\0',};

	cur_index = pstContents->nSearchStartChannelIndex;

	if (pstContents->unSearchUnitTime < NX_VALID_UNIXTIME)
	{
		if (NX_SYSTEM_GetTime(&unixtime) != ERR_OK)
		{
			nx_guide_getData(pstContents, 0, 0, 0);
			HxLOG_Error("time is Invalid!!!\n");
			return ONDK_RET_FAIL;
		}
		pstContents->unSearchUnitTime = pstContents->unStandardStartTime = unixtime;
		pstContents->eLastSearchType = eGuideSearchType_Pf;
	}
	else
	{
		pstContents->unCurrentTime = unixtime;

		// 읽기 쉽게..
		if (eSearchType == eGuideSearchType_Next)
		{
			// search period  (standard - 1h) - standard - (standard + 3h)
			unixtime = pstContents->unSearchUnitTime + (SECONDS_PER_HOUR * 1);		// epg search start time
			pstContents->unSearchUnitTime = pstContents->unSearchUnitTime + (SECONDS_PER_HOUR * 2);	// current search display time
			pstContents->eLastSearchType = eGuideSearchType_Next;
		}
		else if (eSearchType == eGuideSearchType_Prev)
		{
			HUINT32 tempTime = pstContents->unSearchUnitTime - (SECONDS_PER_HOUR * 2);
			unixtime = pstContents->unSearchUnitTime - (SECONDS_PER_HOUR * 2);
			if (tempTime < pstContents->unStandardStartTime)
			{
				HxLOG_Debug("\nthis is first...\n");
			#if 0
				if (pstContents->bPfSearched)
				{
					// recorded programme 을 처리...
				}
				else
			#endif
				{
					unixtime = pstContents->unStandardStartTime;
					pstContents->bPfSearched = TRUE;
				}
			}
			else
				pstContents->unSearchUnitTime = pstContents->unSearchUnitTime - (SECONDS_PER_HOUR * 2);

			pstContents->eLastSearchType = eGuideSearchType_Prev;
		}
		else if (eSearchType == eGuideSearchType_Pf)
		{
			unixtime = pstContents->unStandardStartTime;
			pstContents->eLastSearchType = eGuideSearchType_Pf;
		}
		else if (eSearchType == eGuideSearchType_Down || eSearchType == eGuideSearchType_Up)
		{
			if (pstContents->unSearchUnitTime == pstContents->unStandardStartTime)
				unixtime = pstContents->unSearchUnitTime;
			else
				unixtime = pstContents->unSearchUnitTime - (SECONDS_PER_HOUR * 1);	// next 로 인해 2시간이 더해진 상태이므로, 2 + search buffer 1 시간을 더해 감산.
		}
	}

	pstContents->ulSearchUnixtime = unixtime;

#if 0

	HxSTD_sprintf(value, "%u", unixtime);
	endQuery = NX_METADATA_SEARCH_CreateQuery("endTime", ">", value);

	unixtime += SECONDS_PER_HOUR * 4; // plus 4 hour
	HxSTD_sprintf(value, "%u", unixtime);
	startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);

	andQuery = NX_PORT_META_QUERY_AND(endQuery, startQuery);

	NX_PORT_META_QUERY_DeleteQuery(endQuery);
	NX_PORT_META_QUERY_DeleteQuery(startQuery);

	pstMetaSearch = NX_METADATA_SEARCH_New(eNX_META_SEARCH_TARGET_SCHEDULED);
	if (pstMetaSearch == NULL)
	{
		return ERR_FAIL;
	}
	NX_PORT_META_QUERY_SetQuery(pstMetaSearch->context, andQuery);
	NX_PORT_META_QUERY_DeleteQuery(andQuery);

	for (i = 0; i < NX_PRISM_GUIDE_QUERY_NUM; i++)
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

	{
		ONDK_Result_t	retTimer = ONDK_RET_FAIL;
		//if (pstContents->eLastSearchType != eGuideSearchType_Pf)
		retTimer = ONDK_GWM_SetTimer(NX_GUIDE_QUERY_TIMEOUT_ID, NX_GUIDE_QUERY_TIMEOUT);

		if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
		{
			ONDK_GWM_ResetTimer(NX_GUIDE_QUERY_TIMEOUT_ID);
		}
	}

#endif

	pstContents->bIsReceivedResult = FALSE;

	// 일단 갱신..
	nx_guide_getData(pstContents, 0, 0, 0);

	return GWM_RET_OK;
}

static HBOOL nx_guide_CheckSubProc(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_GUIDE_JUMPTODATE_Proc);
	if(pGwmApp)
		return TRUE;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_GUIDE_GENRESEARCH_Proc);
	if (pGwmApp)
		return TRUE;

	return FALSE;
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

	if(nCompareDate <= 0)	// 선택한 date 가 standard search 보다 작거나 같으면 PF search 로.
	{
		pstContents->bPfSearched = TRUE;
		pstContents->unSearchUnitTime = pstContents->unStandardStartTime;
		nx_guide_SearchStart(pstContents, eGuideSearchType_Pf);

	}
	else
	{
		nCompareDate = HLIB_DATETIME_CompareDate(pstRcvDateTime->stDate, searchUnitDateTime.stDate);
		// jump to date 만큼의 시간을 가감해서 unti search time 에 set 하고 research 하면 된다.
		pstContents->unSearchUnitTime += (SECONDS_PER_HOUR * 24) * nCompareDate;
		nx_guide_SearchStart(pstContents, eGuideSearchType_Next);

		pstContents->bPfSearched = FALSE;
	}

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_GUIDE_JUMPTODATE_Proc);
	if(pGwmApp)
		ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_GUIDE_JUMPTODATE_Proc);
}


static ONDK_Result_t	nx_guide_CellPrev(Nx_GuideContext_t *pstContents)
{
	HINT32 nPrevItemIndex = pstContents->nFocusItemIndex - 1;

	if ( (nPrevItemIndex >= 0)	&&
		 (pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData != NULL)	&&
		 (&pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData[nPrevItemIndex] != NULL))
	{
		NxGuideData_t *pstPrevItem = &pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData[nPrevItemIndex];


		if ( (pstPrevItem->startTime < (pstContents->unSearchUnitPerHalfHourTime + (SECONDS_PER_HOUR * 2))) 	&&
			 ((pstPrevItem->startTime + pstPrevItem->duration) > pstContents->unSearchUnitPerHalfHourTime))
		{
			nx_guide_CleanFocusInChannel(pstContents, pstContents->nFocusChannelIndex);
			pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData[nPrevItemIndex].bFocus = TRUE;
			pstContents->nFocusItemIndex--;
			ONDK_GWM_APP_InvalidateObject(NX_GUIDE_OBJECT_ID);
		}
		else
			nx_guide_SearchStart(pstContents, eGuideSearchType_Prev);
	}
	else
		nx_guide_SearchStart(pstContents, eGuideSearchType_Prev);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CellNext(Nx_GuideContext_t *pstContents)
{
	HINT32 nNextItemIndex = pstContents->nFocusItemIndex + 1;

	if ( (nNextItemIndex < pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].programmeNum)	&&
		 (pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData != NULL)	&&
		 (&pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData[nNextItemIndex] != NULL))
	{
		NxGuideData_t *pstNextItem = &pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData[nNextItemIndex];

		if (pstNextItem->startTime < (pstContents->unSearchUnitPerHalfHourTime + (SECONDS_PER_HOUR * 2)))
		{
			nx_guide_CleanFocusInChannel(pstContents, pstContents->nFocusChannelIndex);
			pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData[nNextItemIndex].bFocus = TRUE;
			pstContents->nFocusItemIndex++;
			ONDK_GWM_APP_InvalidateObject(NX_GUIDE_OBJECT_ID);
		}
		else
		{
			nx_guide_SearchStart(pstContents, eGuideSearchType_Next);
		}

	}
	else
	{
		HxLOG_Debug("\n\n[%s.%d] programme NUM : %d\n\n",__FUNCTION__,__LINE__, pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].programmeNum);
		nx_guide_SearchStart(pstContents, eGuideSearchType_Next);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CellUp(Nx_GuideContext_t *pstContents)
{
	HINT32	nFocusCurChannelIndex  = pstContents->nFocusChannelIndex;
	HINT32	nFocusUpChannelIndex = pstContents->nFocusChannelIndex - 1;
	HINT32	nMaxCollSize = 0, nGetCollSize = 0;
	HINT32	nMaxItemIndex = 0;

	if (pstContents->bPfSearched == TRUE)
	{
		if (nFocusUpChannelIndex >= 0)
		{
			pstContents->nFocusChannelIndex--;
			nx_guide_CleanFocusInPf(pstContents);
			pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].bPfFocus = TRUE;
			ONDK_GWM_APP_InvalidateObject(NX_GUIDE_OBJECT_ID);
		}
		else
		{
			pstContents->nSearchStartChannelIndex--;

			if (pstContents->nSearchStartChannelIndex < 0)
				pstContents->nSearchStartChannelIndex = (pstContents->nChannelNum - 1);

			nx_guide_SearchStart(pstContents, eGuideSearchType_Pf);
		}
	}
	else
	{

		if (nFocusUpChannelIndex >= 0)
		{
			HINT32	i = 0;
			NxGuideData_t *pstCurItem = &pstContents->pstGuideChannels[nFocusCurChannelIndex].pstGuideData[pstContents->nFocusItemIndex];
			Nx_GuideChannel_t *pstUpGuideChannel = &pstContents->pstGuideChannels[nFocusUpChannelIndex];

			for (i = 0; i < pstUpGuideChannel->programmeNum; i++)
			{
				if ( &pstContents->pstGuideChannels[nFocusUpChannelIndex].pstGuideData[i] != NULL)
				{
					NxGuideData_t *pstCompareItem = &pstContents->pstGuideChannels[nFocusUpChannelIndex].pstGuideData[i];
					nGetCollSize = nx_guide_GetCollisionSize(pstContents, pstCurItem, pstCompareItem);

					HxLOG_Debug("\n(%d):(%s) ulGetCollSize(%d),  maxSIze(%d)\n",i,
						pstCompareItem->pszEventName, nGetCollSize, nMaxCollSize);

					if (nGetCollSize > nMaxCollSize)
					{
						nMaxCollSize = nGetCollSize;
						nMaxItemIndex = i;
					}
				}
				else
					continue;
			}

			if (nMaxCollSize > 0)
			{
				nx_guide_CleanFocusInChannel(pstContents, nFocusCurChannelIndex);
				pstContents->nFocusItemIndex = nMaxItemIndex;
				pstContents->nFocusChannelIndex = nFocusUpChannelIndex;
				if(pstContents->pstGuideChannels[nFocusUpChannelIndex].pstGuideData)
				{
					pstContents->pstGuideChannels[nFocusUpChannelIndex].pstGuideData[nMaxItemIndex].bFocus = TRUE;
				}
				ONDK_GWM_APP_InvalidateObject(NX_GUIDE_OBJECT_ID);
			}
			else
			{

			}
		}
		else	// over down page
		{
			pstContents->nSearchStartChannelIndex--;

			if (pstContents->nSearchStartChannelIndex < 0)
				pstContents->nSearchStartChannelIndex = (pstContents->nChannelNum - 1);

			nx_guide_SearchStart(pstContents, eGuideSearchType_Up);
		}
	} // pf else end

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CellDown(Nx_GuideContext_t *pstContents)
{
	HINT32	nFocusCurChannelIndex  = pstContents->nFocusChannelIndex;
	HINT32	nFocusDownChannelIndex = pstContents->nFocusChannelIndex + 1;
	HINT32	nMaxCollSize = 0, nGetCollSize = 0;
	HINT32	nMaxItemIndex = 0;

	if (pstContents->bPfSearched == TRUE)
	{
		if (nFocusDownChannelIndex < NX_PRISM_GUIDE_QUERY_NUM)
		{
			pstContents->nFocusChannelIndex++;
			nx_guide_CleanFocusInPf(pstContents);
			nx_guide_CleanFocusInChannel(pstContents, nFocusCurChannelIndex);
			pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].bPfFocus = TRUE;
			if(pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData)
			{
				pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData[pstContents->nFocusItemIndex].bFocus = TRUE;
			}
			ONDK_GWM_APP_InvalidateObject(NX_GUIDE_OBJECT_ID);
		}
		else
		{
			pstContents->nSearchStartChannelIndex++;

			if (pstContents->nSearchStartChannelIndex >= pstContents->nChannelNum)
				pstContents->nSearchStartChannelIndex = 0;

			nx_guide_SearchStart(pstContents, eGuideSearchType_Pf);
		}
	}
	else
	{
		if (nFocusDownChannelIndex < NX_PRISM_GUIDE_QUERY_NUM)
		{
			HINT32	i = 0;
			NxGuideData_t *pstCurItem = &pstContents->pstGuideChannels[nFocusCurChannelIndex].pstGuideData[pstContents->nFocusItemIndex];
			Nx_GuideChannel_t *pstDownGuideChannel = &pstContents->pstGuideChannels[nFocusDownChannelIndex];

			for (i = 0; i < pstDownGuideChannel->programmeNum; i++)
			{
				if ( &pstContents->pstGuideChannels[nFocusDownChannelIndex].pstGuideData[i] != NULL)
				{
					NxGuideData_t *pstCompareItem = &pstContents->pstGuideChannels[nFocusDownChannelIndex].pstGuideData[i];
					nGetCollSize = nx_guide_GetCollisionSize(pstContents, pstCurItem, pstCompareItem);

					HxLOG_Debug("\n(%d):(%s) ulGetCollSize(%d),  maxSIze(%d)\n",i,
						pstCompareItem->pszEventName, nGetCollSize, nMaxCollSize);

					if (nGetCollSize > nMaxCollSize)
					{
						nMaxCollSize = nGetCollSize;
						nMaxItemIndex = i;
					}
				}
				else
					continue;
			}

			if (nMaxCollSize > 0)
			{
				nx_guide_CleanFocusInChannel(pstContents, nFocusCurChannelIndex);
				pstContents->nFocusItemIndex = nMaxItemIndex;
				pstContents->nFocusChannelIndex = nFocusDownChannelIndex;
				pstContents->pstGuideChannels[nFocusDownChannelIndex].pstGuideData[nMaxItemIndex].bFocus = TRUE;

				ONDK_GWM_APP_InvalidateObject(NX_GUIDE_OBJECT_ID);
			}
			else
			{

			}
		}
		else	// over down page
		{
			pstContents->nSearchStartChannelIndex++;

			if (pstContents->nSearchStartChannelIndex >= pstContents->nChannelNum)
				pstContents->nSearchStartChannelIndex = 0;

			nx_guide_SearchStart(pstContents, eGuideSearchType_Down);
		}
	}

	return	ONDK_RET_OK;
}

static void	nx_guide_PageUp(Nx_GuideContext_t *pstContents)
{
	pstContents->nSearchStartChannelIndex -= NX_PRISM_GUIDE_QUERY_NUM;
	if (pstContents->nSearchStartChannelIndex < 0)
		pstContents->nSearchStartChannelIndex = pstContents->nChannelNum - NX_PRISM_GUIDE_QUERY_NUM;
	nx_guide_SearchStart(pstContents, eGuideSearchType_Up);
}

static void	nx_guide_PageDown(Nx_GuideContext_t *pstContents)
{
	pstContents->nSearchStartChannelIndex += NX_PRISM_GUIDE_QUERY_NUM;
	if (pstContents->nSearchStartChannelIndex >= pstContents->nChannelNum)
	{
		pstContents->nSearchStartChannelIndex = (pstContents->nSearchStartChannelIndex - pstContents->nChannelNum);
	}
	nx_guide_SearchStart(pstContents, eGuideSearchType_Down);
}

static ONDK_Result_t	nx_guide_SetFocusOnChangedEvent(Nx_GuideContext_t *pstContents)
{
	HINT32 i = 0;
	HINT32 nFocusChannelIndex = pstContents->nFocusChannelIndex;
	HINT32 programmeNum = pstContents->pstGuideChannels[nFocusChannelIndex].programmeNum;

	if (&pstContents->pstGuideChannels[nFocusChannelIndex].pstGuideData == NULL)
	{
		HxLOG_Debug("\n\n[%s.%d] pstGuide data is NULL\n\n\n",__FUNCTION__,__LINE__);
		return ONDK_RET_FAIL;
	}

	if (pstContents->eLastSearchType == eGuideSearchType_Next	||
		pstContents->eLastSearchType == eGuideSearchType_Pf)
	{
		for (i = 0; i < programmeNum; i++)
		{
			NxGuideData_t *pstItem = &pstContents->pstGuideChannels[nFocusChannelIndex].pstGuideData[i];

			if (pstItem != NULL)
			{
				HxLOG_Debug("\n\n===>>i(%d), pstItem : (%s)\n\n", i, pstItem->pszEventName);

				if ((pstItem->startTime + pstItem->duration) > pstContents->unSearchUnitPerHalfHourTime)
				{
					pstContents->pstGuideChannels[nFocusChannelIndex].pstGuideData[i].bFocus = TRUE;
					pstContents->nFocusItemIndex = i;
					return ONDK_RET_OK;
				}
			}
		}
	}
	else if (pstContents->eLastSearchType == eGuideSearchType_Prev)
	{
		for (i = programmeNum; i >= 0; i--)
		{
			NxGuideData_t *pstItem = &pstContents->pstGuideChannels[nFocusChannelIndex].pstGuideData[i];

			HxLOG_Debug("\n\n===>>i(%d), pstItem : (%s)\n\n", i, pstItem->pszEventName);

			{
				HxDATETIME_t datetime;
				HLIB_DATETIME_ConvertUnixTimeToDateTime(pstItem->startTime, &datetime);

				HxLOG_Debug("\n\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
				HxLOG_Debug("Prev StartTime  start time (%02d:%02d)\n\n\n\n", datetime.stTime.ucHour, datetime.stTime.ucMinute);

				HLIB_DATETIME_ConvertUnixTimeToDateTime((pstItem->startTime + pstItem->duration), &datetime);

				HxLOG_Debug("\n\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
				HxLOG_Debug("startTime + duration (%02d:%02d)\n\n\n\n", datetime.stTime.ucHour, datetime.stTime.ucMinute);

				HLIB_DATETIME_ConvertUnixTimeToDateTime((pstContents->unSearchUnitPerHalfHourTime + (SECONDS_PER_HOUR * 2)), &datetime);

				HxLOG_Debug("\n\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
				HxLOG_Debug("unittime + 2hour  start time (%02d:%02d)\n\n\n\n", datetime.stTime.ucHour, datetime.stTime.ucMinute);
			}

			if ( (pstItem->startTime < (pstContents->unSearchUnitPerHalfHourTime + (SECONDS_PER_HOUR * 2)))		&&
				 ((pstItem->startTime + pstItem->duration) > pstContents->unSearchUnitPerHalfHourTime))
			{
				pstContents->pstGuideChannels[nFocusChannelIndex].pstGuideData[i].bFocus = TRUE;
				pstContents->nFocusItemIndex = i;
				return ONDK_RET_OK;
			}
		}

		HxLOG_Debug("\n\n[%s.%d] ---->>> prev \n\n",__FUNCTION__,__LINE__);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CleanFocusInPf(Nx_GuideContext_t *pstContents)
{
	int i = 0;
	for (i = 0; i < NX_PRISM_GUIDE_QUERY_NUM; i++)
	{
		pstContents->pstGuideChannels[i].bPfFocus = FALSE;
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CleanFocusInChannel(Nx_GuideContext_t *pstContents, HINT32 nFocusChannelIndex)
{
	int i = 0;

	if (pstContents->pstGuideChannels[nFocusChannelIndex].pstGuideData == NULL)
		return ONDK_RET_FAIL;

	for (i = 0; i < pstContents->pstGuideChannels[nFocusChannelIndex].programmeNum ; i++)
	{
		if (&pstContents->pstGuideChannels[nFocusChannelIndex].pstGuideData[i] != NULL)
			pstContents->pstGuideChannels[nFocusChannelIndex].pstGuideData[i].bFocus = FALSE;
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CleanFocusAll(Nx_GuideContext_t *pstContents)
{
	int i = 0;
	for (i = 0; i < NX_PRISM_GUIDE_QUERY_NUM; i++)
	{
		nx_guide_CleanFocusInChannel(pstContents, i);
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_guide_CalcUnitTimePerHalfHour(Nx_GuideContext_t *pstContents)
{
	HxDATETIME_t 		DateTime;

	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unSearchUnitTime, &DateTime);

	if (DateTime.stTime.ucMinute >= 0 && DateTime.stTime.ucMinute < 30)
	{
		DateTime.stTime.ucMinute = 0;

		HLIB_DATETIME_ConvertDateTimeToUnixTime(&DateTime, &pstContents->unSearchUnitPerHalfHourTime);
	}
	else if (DateTime.stTime.ucMinute >= 30)
	{
		HINT32 restTime = 60 - DateTime.stTime.ucMinute;
		DateTime.stTime.ucMinute -= (30 - restTime);			// 30min
		HLIB_DATETIME_ConvertDateTimeToUnixTime(&DateTime, &pstContents->unSearchUnitPerHalfHourTime);
	}

	return ONDK_RET_OK;
}

static HINT32	nx_guide_GetCollisionSize(Nx_GuideContext_t *pstContents, NxGuideData_t *pstCurItem,	NxGuideData_t *pstCompareItem)
{
	//HINT32	nCurItemLen = 0, nCompareItemLen = 0;
	HUINT32	ulCurStartTime 		= pstCurItem->startTime;
	HUINT32 ulCurDuration 		= pstCurItem->duration;
	HUINT32	ulCompareStartTime 	= pstCompareItem->startTime;
	HUINT32	ulCompareDuration 	= pstCompareItem->duration;
	HUINT32	ulHalfHourUnitTime	= pstContents->unSearchUnitPerHalfHourTime;
	HUINT32	ulHalfHourEndUnitTime = ulHalfHourUnitTime + (SECONDS_PER_HOUR * 2);

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

#if 0
	{
		HxDATETIME_t datetime;
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurStartTime, &datetime);
		HxLOG_Debug("\n\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
		HxLOG_Debug("ulCurStartTime (%02d:%02d), duration : %u, len : %u\n", datetime.stTime.ucHour, datetime.stTime.ucMinute, ulCurDuration, ulCurItemLen);

		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCompareStartTime, &datetime);
		HxLOG_Debug("ulCompareStartTime (%02d:%02d), duration : %u, len : %u\n", datetime.stTime.ucHour, datetime.stTime.ucMinute, ulCompareDuration, ulCompareItemLen);
	}
#endif
	if (ulCurStartTime >= ulCompareStartTime)
	{
		if (ulCurItemLen < ulCompareItemLen)
		{
			HxLOG_Debug("\n[%s.%d] return : %d\n\n\n\n\n", __FUNCTION__,__LINE__, ulCurDuration);
			return (HINT32)ulCurDuration;
		}
		else
		{
			HxLOG_Debug("\n[%s.%d] return : %d\n\n\n\n\n", __FUNCTION__,__LINE__, ulCompareItemLen - ulCurStartTime);
			return (HINT32)(ulCompareItemLen - ulCurStartTime);
		}
	}
	else
	{
		if (ulCurItemLen > ulCompareStartTime)
		{
			if (ulCurItemLen > ulCompareItemLen)
			{
				HxLOG_Debug("\n[%s.%d] return : %d\n\n\n\n\n", __FUNCTION__,__LINE__, ulCompareDuration);
				return (HINT32)ulCompareDuration;
			}
			else
			{
				HxLOG_Debug("\n[%s.%d] return : %d\n\n\n\n\n", __FUNCTION__,__LINE__, ulCurItemLen - ulCompareStartTime);
				return (HINT32)(ulCurItemLen - ulCompareStartTime);
			}
		}
		else
		{
			return 0;
		}
	}
}

static void		nx_guide_NxProgrammeItem_Delete (void *item)
{
	if(item){		NX_APP_Free(item);		item = NULL;	}
}

static ONDK_Result_t 	nx_guide_getData(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			i = 0, j = 0, cur_index = 0, num = 0;
	HUINT32			ulSvcUid = 0;
	NX_Channel_t	*channel = NULL, *stCurPlayChannel = NULL;
	HxVector_t		*result = NULL;		//= HLIB_VECTOR_New(NULL,NULL);
	DxService_t		*tempSvc = NULL;


	// data clear
	for (i = 0; i < NX_PRISM_GUIDE_QUERY_NUM; i++)
	{
		if (pstContents->pstGuideChannels[i].pstGuideData != NULL)
		{
			NX_APP_Free(pstContents->pstGuideChannels[i].pstGuideData);
			pstContents->pstGuideChannels[i].pstGuideData = NULL;
		}
		HxSTD_MemSet(&pstContents->pstGuideChannels[i], 0, sizeof(Nx_GuideChannel_t));
	}
	if (pstContents->pstCurFocusData)
	{
		NX_APP_Free(pstContents->pstCurFocusData);
		pstContents->pstCurFocusData = NULL;
	}

	cur_index = pstContents->nSearchStartChannelIndex;

	// common initilizing
	nx_guide_CleanFocusAll(pstContents);
	pstContents->nFocusItemIndex = 0;
	nx_guide_CalcUnitTimePerHalfHour(pstContents);

	for (i = 0; i < NX_PRISM_GUIDE_QUERY_NUM; i++)
	{
		if (cur_index >= pstContents->nChannelNum)
		{
			cur_index = 0;
		}

		channel = &pstContents->pstChannels[cur_index];

		tempSvc = NX_CHANNEL_FindServiceBySvcUid(channel->svcUid);

		result = HLIB_VECTOR_NewEx(NULL, 0,
										(void *(*)(void *))NULL,
										(void *(*)(void *))nx_guide_NxProgrammeItem_Delete,
										NULL);

		NX_METADATA_EPG_GetResultProgrammeItemBySvcUid(channel->svcUid, result, &num);

		// 기본정보 저장.
		pstContents->pstGuideChannels[i].svcUid = channel->svcUid;
		pstContents->pstGuideChannels[i].lcn  = channel->lcn;
		pstContents->pstGuideChannels[i].pszChannelName = tempSvc->name;

		HxLOG_Debug("(%d) : channel name: %s , NUM(%d)\n", i, pstContents->pstGuideChannels[i].pszChannelName, num);

		if (num > 0 && (result != NULL))
		{
			HUINT32	prevItemEndTime = pstContents->unSearchUnitPerHalfHourTime;
			HUINT32 startTime = 0, duration = 0, endTime = 0;
			HINT32	listNum = num;

			//HCHAR eventName[EPG_HUMAX_NAME_LEN] = {'\0',};

			Nx_GuideList_t	*list = NULL, *prevlist = NULL;

			for (j = 0; j < num; j++)
			{
				//NX_Programme_t *item = HLIB_VECTOR_ItemAt(result, j);

				NX_ProgrammeItem_t *item = HLIB_VECTOR_ItemAt(result, j);
				//HxDATETIME_t datetime, durationtime;
				//HUINT32 weekIndex = 0;

				Nx_GuideList_t	*addItem = (Nx_GuideList_t*)NX_APP_Calloc(sizeof(Nx_GuideList_t));

				startTime 	= item->startTime;
				duration	= item->duration;
				endTime		= startTime + duration;

				// -------- set item
				addItem->data.startTime	= item->startTime;
				addItem->data.duration	= item->duration;

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

			#if 0
				startTime 	= NX_METADATA_PROGRAMMES_GetStartTime(item);
				duration	= NX_METADATA_PROGRAMMES_GetDuration(item);
				endTime		= startTime + duration;

				// -------- set item
				addItem->data.startTime	= NX_METADATA_PROGRAMMES_GetStartTime(item);
				addItem->data.duration	= NX_METADATA_PROGRAMMES_GetDuration(item);

				// event name
				if (NX_METADATA_PROGRAMMES_GetName(item, eventName) == ERR_OK)
				{
					HxSTD_MemSet(addItem->data.pszEventName, 0, EPG_HUMAX_NAME_LEN);
					HLIB_STD_StrUtf8NCpy(addItem->data.pszEventName, eventName, EPG_HUMAX_NAME_LEN);
				}

				addItem->data.eventId	= NX_METADATA_PROGRAMMES_GetEventID(item);
				addItem->data.svcUid	= NX_METADATA_PROGRAMMES_GetSvcUID(item);

				// pszduration
				HLIB_DATETIME_ConvertUnixTimeToDateTime(addItem->data.startTime, &datetime);
				HLIB_DATETIME_ConvertUnixTimeToDateTime(
										addItem->data.startTime +
										addItem->data.duration, &durationtime);
				weekIndex = HLIB_DATETIME_GetWeekDayIndex(datetime.stDate);
				HxSTD_sprintf((HCHAR*)addItem->data.pszDuration,
							(HCHAR*)"%02d:%02d - %02d:%02d / %s.%02d.%02d",
							datetime.stTime.ucHour, datetime.stTime.ucMinute,
							durationtime.stTime.ucHour, durationtime.stTime.ucMinute,
							ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(weekIndex)),
							datetime.stDate.ucDay, datetime.stDate.ucMonth);

				addItem->data.bFocus = FALSE;
				addItem->data.bNoProgramme = FALSE;
				// ------- end set item
			#endif

				// no programme 이 처음에 오는 경우.
				if (j == 0 && (prevItemEndTime < startTime) && pstContents->bPfSearched != TRUE)
				{
					Nx_GuideList_t	*templist = (Nx_GuideList_t*)NX_APP_Calloc(sizeof(Nx_GuideList_t));

					// set no programme
					listNum += 1;
					templist->data.startTime	= prevItemEndTime;
					templist->data.duration		= startTime - prevItemEndTime;
					templist->data.eventId		= -1;
					templist->data.svcUid		= -1;
					templist->data.bNoProgramme	= TRUE;
					templist->data.bFocus		= FALSE;
					HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);
					templist->next = NULL;

					addItem->next = NULL;
					templist->next = addItem;

					prevItemEndTime = addItem->data.startTime + addItem->data.duration;

					list 	= templist;
					prevlist= addItem;
				}
				else if (j == 0)		// 맨앞의 데이터 처리
				{
					if ( num <= 1)		// 맨앞이면서 item 이 하나인경우 처리.
					{
						Nx_GuideList_t	*templist = (Nx_GuideList_t*)NX_APP_Calloc(sizeof(Nx_GuideList_t));
						// set no programme
						listNum += 1;
						templist->data.startTime	= endTime;
						templist->data.duration		= (pstContents->unSearchUnitPerHalfHourTime + (SECONDS_PER_HOUR * 2)) - endTime;
						templist->data.eventId		= -1;
						templist->data.svcUid		= -1;
						templist->data.bNoProgramme	= TRUE;
						templist->data.bFocus		= FALSE;
						HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);
						templist->next = NULL;

						addItem->next = templist;
						prevlist = templist;
						list = addItem;
						prevItemEndTime = (pstContents->unSearchUnitPerHalfHourTime + (SECONDS_PER_HOUR * 2));
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
					// set no programme
					listNum += 1;
					templist->data.startTime	= prevItemEndTime;
					templist->data.duration		= startTime - prevItemEndTime;
					templist->data.eventId		= -1;
					templist->data.svcUid		= -1;
					templist->data.bNoProgramme	= TRUE;
					templist->data.bFocus		= FALSE;
					HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);
					templist->next = NULL;

					addItem->next = NULL;
					templist->next	= addItem;
					prevlist->next	= templist;

					prevItemEndTime = addItem->data.startTime + addItem->data.duration;

					prevlist = addItem;
				}
				else if (j == (num-1)	&&	// 마지막 데이터뒤에 no programme 인경우.(단, end unit time 을 넘지 않음)
						 (addItem->data.startTime + addItem->data.duration) < (pstContents->unSearchUnitPerHalfHourTime + (SECONDS_PER_HOUR * 2)))
				{
					Nx_GuideList_t	*templist = (Nx_GuideList_t*)NX_APP_Calloc(sizeof(Nx_GuideList_t));
					// set no programme
					listNum += 1;
					templist->data.startTime	= addItem->data.startTime + addItem->data.duration;
					templist->data.duration		= (pstContents->unSearchUnitPerHalfHourTime + (SECONDS_PER_HOUR * 2)) - (addItem->data.startTime + addItem->data.duration);
					templist->data.eventId		= -1;
					templist->data.svcUid		= -1;
					templist->data.bNoProgramme	= TRUE;
					templist->data.bFocus		= FALSE;
					HLIB_STD_StrUtf8NCpy(templist->data.pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);

					templist->next = NULL;
					addItem->next = templist;
					prevlist->next = addItem;

					prevItemEndTime = templist->data.startTime + templist->data.duration;

					prevlist = templist;
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
				pstContents->pstGuideChannels[i].pstGuideData = (NxGuideData_t *)NX_APP_Calloc(sizeof(NxGuideData_t) * listNum);

				tempList = list;
				j = 0;
				while(tempList)
				{
					HxSTD_MemCopy(&pstContents->pstGuideChannels[i].pstGuideData[j], &tempList->data, sizeof(NxGuideData_t));
					tempList = tempList->next;
					j++;
				}
				tempList = list;
				for (j = 0; j < listNum; j++)
				{
					HxLOG_Debug("\n tempList (0x%X) , list(0x%X)\n", tempList, list);
					if (tempList != NULL)
					{
						freeData = tempList;
						if (tempList->next != NULL)
						{
							tempList = tempList->next;
						}
						if (freeData != NULL)
						{
							NX_APP_Free(freeData);
							freeData = NULL;
						}
					}
				}
				pstContents->pstGuideChannels[i].programmeNum = listNum;
			}


			HLIB_VECTOR_Delete(result);		// new list 만 날리고, core epg 의 data 는 날리면 안됨.
		}
		else
		{
			HxLOG_Debug("channel (%s) programmes data is null...\n", pstContents->pstGuideChannels[i].pszChannelName);
			// fill no programme item
			if (pstContents->bPfSearched)
			{
				HINT32 nPfLoop = 0;
				pstContents->pstGuideChannels[i].pstGuideData = (NxGuideData_t *)NX_APP_Calloc(sizeof(NxGuideData_t) * 2);
				pstContents->pstGuideChannels[i].programmeNum = 2;

				for (nPfLoop = 0; nPfLoop < 2; nPfLoop++)
				{
					pstContents->pstGuideChannels[i].pstGuideData[nPfLoop].startTime 	= pstContents->unSearchUnitPerHalfHourTime + (SECONDS_PER_HOUR * nPfLoop);
					pstContents->pstGuideChannels[i].pstGuideData[nPfLoop].duration	= SECONDS_PER_HOUR * (nPfLoop + 1);
					pstContents->pstGuideChannels[i].pstGuideData[nPfLoop].eventId	= -1;
					pstContents->pstGuideChannels[i].pstGuideData[nPfLoop].svcUid		= -1;
					pstContents->pstGuideChannels[i].pstGuideData[nPfLoop].bFocus 	= FALSE;
					pstContents->pstGuideChannels[i].pstGuideData[nPfLoop].bNoProgramme = TRUE;
					HLIB_STD_StrUtf8NCpy(pstContents->pstGuideChannels[i].pstGuideData[nPfLoop].pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);
				}
			}
			else
			{
				pstContents->pstGuideChannels[i].pstGuideData = (NxGuideData_t *)NX_APP_Calloc(sizeof(NxGuideData_t));
				pstContents->pstGuideChannels[i].programmeNum = 1;

				pstContents->pstGuideChannels[i].pstGuideData[0].startTime 	= pstContents->unSearchUnitPerHalfHourTime;
				pstContents->pstGuideChannels[i].pstGuideData[0].duration		= SECONDS_PER_HOUR * 2;
				pstContents->pstGuideChannels[i].pstGuideData[0].eventId		= -1;
				pstContents->pstGuideChannels[i].pstGuideData[0].svcUid		= -1;
				pstContents->pstGuideChannels[i].pstGuideData[0].bFocus = FALSE;
				pstContents->pstGuideChannels[i].pstGuideData[0].bNoProgramme = TRUE;
				HLIB_STD_StrUtf8NCpy(pstContents->pstGuideChannels[i].pstGuideData[0].pszEventName, (HCHAR*)ONDK_GetString(RES_NO_PROGRAMME_INFORMATION_01_ID), EPG_HUMAX_NAME_LEN);
			}
		}

		cur_index++;
	}

	nx_guide_SetFocusOnChangedEvent(pstContents);

	if (pstContents->bPfSearched)
	{
		HxLOG_Debug("\n\n\n\n------------- SET PF GUIDE ----------------\n\n\n\n");
		ONDK_GWM_COM_TvGuide_SetPF(NX_GUIDE_OBJECT_ID, TRUE);
		pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].bPfFocus = TRUE;

	}
	else
	{
		HxLOG_Debug("\n\n\n\n------------- SET NOT PF GUIDE ----------------\n\n\n\n");
		ONDK_GWM_COM_TvGuide_SetPF(NX_GUIDE_OBJECT_ID, FALSE);
	}
	{
		if (NX_SYSTEM_GetTime(&pstContents->unCurrentTime) != ERR_OK)
		{
			HxLOG_Error("time is Invalid!!!\n");

		}
		else
		{
			ONDK_GWM_COM_TvGuide_SetCurrentTime(NX_GUIDE_OBJECT_ID, pstContents->unCurrentTime);
		}
	}
	ONDK_GWM_COM_TvGuide_SetStartUnitTime(NX_GUIDE_OBJECT_ID, pstContents->unSearchUnitPerHalfHourTime);


	stCurPlayChannel = NX_PRISM_LIVE_GetMasterChannel();
	if (stCurPlayChannel)
	{
		ulSvcUid = stCurPlayChannel->svcUid;
	}
	else
	{
		ulSvcUid = 0;
	}

	//if (pstContents->pstGuideChannels[0].pstGuideData != NULL)
	{
		ONDK_GWM_COM_TvGuide_SetEpgData(NX_GUIDE_OBJECT_ID, &pstContents->pstGuideChannels);
		ONDK_GWM_COM_TvGuide_SetEpgCurrentChannel(NX_GUIDE_OBJECT_ID, ulSvcUid);
		ONDK_GWM_APP_InvalidateObject(NX_GUIDE_OBJECT_ID);
	}

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_guide_MsgGwmCreate(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError = ERR_FAIL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_GuideContext_t));

	hError = NX_PRISM_LIVE_GetCurChannelIndex(&pstContents->nCurChannelIndex);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetCurChannelIndex() fail !!\n");
		return ONDK_RET_FAIL;
	}

	hError = NX_PRISM_LIVE_GetChannelListNum(&pstContents->nChannelNum);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetChannelListNum() fail !!\n");
		return ONDK_RET_FAIL;
	}

	//pstContents->nCurChannelIndex	= p1;
	//pstContents->nChannelNum		= p2;
	pstContents->pstChannels = NX_PRISM_LIVE_GetLiveChannels();

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	nx_guide_DrawDefaultUI(pstContents);

	ONDK_GWM_SetTimer(NX_GUIDE_TIMEOUT_ID, NX_GUIDE_TIMEOUT);

	pstContents->nSearchStartChannelIndex = pstContents->nCurChannelIndex;
	pstContents->nFocusChannelIndex = 0;
	pstContents->bPfSearched = TRUE;
	nx_guide_SearchStart(pstContents, eGuideSearchType_Pf);

	{
		ONDK_Result_t	retTimer = ONDK_RET_FAIL;
		//if (pstContents->eLastSearchType != eGuideSearchType_Pf)
		retTimer = ONDK_GWM_SetTimer(NX_GUIDE_QUERY_TIMEOUT_ID, NX_GUIDE_QUERY_TIMEOUT);

		if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
		{
			ONDK_GWM_ResetTimer(NX_GUIDE_QUERY_TIMEOUT_ID);
		}
	}

	// Init Option Item Data
	nx_guide_OptionInitData(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_guide_MsgGwmDestroy(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 i = 0;

	//NX_PRISM_AppNaviCntxt_FreeMainItemArray_KeepFocus(pstContents);

	for (i = 0; i < NX_PRISM_GUIDE_QUERY_NUM; i++)
	{
		if (pstContents->pstGuideChannels[i].pstGuideData != NULL)
		{
			NX_APP_Free(pstContents->pstGuideChannels[i].pstGuideData);
			pstContents->pstGuideChannels[i].pstGuideData = NULL;
		}
		HxSTD_MemSet(&pstContents->pstGuideChannels[i], 0, sizeof(Nx_GuideChannel_t));
	}

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

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_guide_MsgGwmHapiMsg(Nx_GuideContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_guide_MsgGwmTimer(Nx_GuideContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if ((p1 == NX_GUIDE_QUERY_TIMEOUT_ID) && (pstContents->bIsReceivedResult == FALSE))
	{
		NX_APP_Error("\n\n\n[%s.%d] +=+=+=+=+=+=+=+=+=+=+=+=+=+=>> Query Result Received FALSE\n\n\n\n",__FUNCTION__,__LINE__);
		//nx_guide_getData(pstContents, p1, p2, p3);
		//pstContents->bIsReceivedResult = TRUE;
		nx_guide_QuerySearchStart(pstContents);
		ONDK_GWM_KillTimer(NX_GUIDE_QUERY_TIMEOUT_ID);
	}
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);



	return GWM_RET_OK;
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

	{
		ONDK_Result_t	retTimer = ONDK_RET_FAIL;
		//if (pstContents->eLastSearchType != eGuideSearchType_Pf)
		retTimer = ONDK_GWM_SetTimer(NX_GUIDE_QUERY_TIMEOUT_ID, NX_GUIDE_QUERY_TIMEOUT);

		if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
		{
			ONDK_GWM_ResetTimer(NX_GUIDE_QUERY_TIMEOUT_ID);
		}
	}

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
										0, 0, lKeyId);
			}
			return ONDK_MESSAGE_BREAK;

		case KEY_OK:
			{
				NxGuideData_t *pstCurFocusItem = NULL;

				if (&pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData != NULL)
				{
					pstCurFocusItem = &pstContents->pstGuideChannels[pstContents->nFocusChannelIndex].pstGuideData[pstContents->nFocusItemIndex];
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
			if (pstContents->bPfSearched == TRUE)
			{
				nx_guide_SearchStart(pstContents, eGuideSearchType_Pf);
				pstContents->bPfSearched = FALSE;
			}
			else
			{
				nx_guide_CellNext(pstContents);
			}
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWUP:
			nx_guide_CellUp(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWDOWN:
			nx_guide_CellDown(pstContents);
			return	ONDK_MESSAGE_BREAK;

		case KEY_CH_PLUS:
			nx_guide_PageUp(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_CH_MINUS:
			nx_guide_PageDown(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_OPT:
			// TODO:  shows opiton for guide
			HxLOG_Debug("Option key on Guide \n");
			nx_guide_CreateListPopup(pstContents);
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_guide_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}
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
			eResult = nx_guide_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES:
			HxLOG_Print("\n\n\n ======>>> RECEIVED MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES \n\n\n");
			{
				pstContents->bIsReceivedResult = TRUE;
				nx_guide_getData(pstContents, p1, p2, p3);
			}
			break;

		case MSG_CORE_SERVICE_EPG_JUMPTODATE_UPDATE:
			{
				nx_guide_SetJumpToDate(pstContents, (HxDATETIME_t*)p1);
				#if 0
				ONDK_GWM_App_t	pGwmApp = NULL;
				HINT32	nCompareDate = 0;
				HxDATETIME_t *pstRcvDateTime = (HxDATETIME_t*)p1;
				HxDATETIME_t standardDateTime, searchUnitDateTime;

				HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unStandardStartTime, &standardDateTime);
				HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->unSearchUnitTime, &searchUnitDateTime);

				nCompareDate = HLIB_DATETIME_CompareDate(pstRcvDateTime->stDate, standardDateTime.stDate);

				if(nCompareDate <= 0)	// 선택한 date 가 standard search 보다 작거나 같으면 PF search 로.
				{
					pstContents->bPfSearched = TRUE;
					pstContents->unSearchUnitTime = pstContents->unStandardStartTime;
					nx_guide_SearchStart(pstContents, eGuideSearchType_Pf);

				}
				else
				{
					nCompareDate = HLIB_DATETIME_CompareDate(pstRcvDateTime->stDate, searchUnitDateTime.stDate);
					// jump to date 만큼의 시간을 가감해서 unti search time 에 set 하고 research 하면 된다.
					pstContents->unSearchUnitTime += (SECONDS_PER_HOUR * 24) * nCompareDate;
					nx_guide_SearchStart(pstContents, eGuideSearchType_Next);

					pstContents->bPfSearched = FALSE;
				}

				pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_GUIDE_JUMPTODATE_Proc);
				if(pGwmApp)
					ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_GUIDE_JUMPTODATE_Proc);
				#endif
			}
			break;
		case MSG_APP_POPUP_LIST_DESTROYED:
		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED & MSG_APP_SETTING_VALUE_CHANGED\n");
			if (lMessage == MSG_APP_SETTING_VALUE_CHANGED)
			{
				NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED\n");
				eResult = nx_guide_OptionChangedItemValue(pstContents, p1);
			}
			break;

		case MSG_APP_DCN_INPUT:
			HxLOG_Error("DCN Input !!!\n");
			// TODO: set current index
			eResult = ONDK_MESSAGE_BREAK;
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
