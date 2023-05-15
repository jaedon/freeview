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
 * @file	  		nx_prism_schedule_add.c
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
#include <nx_prism_menu_items.h>
#include <nx_prism_menu_contents.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/
#define	NX_PRISM_SCHEDULE_ADD_ITEM_ID						eMenuFrame_Window_ScheduleAdd

//#define	NX_PRISM_SCHEDULE_ADD_EXPIRE_TIMER_ID			(NX_PRISM_SCHEDULE_ADD_ITEM_ID + 1)
#define	NX_PRISM_SCHEDULE_ADD_EXPIRE_TIME					(30 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_SCHEDULE_ADD_WINDOW_X						0
#define	NX_PRISM_SCHEDULE_ADD_WINDOW_Y						0
#define	NX_PRISM_SCHEDULE_ADD_WINDOW_W						NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_SCHEDULE_ADD_WINDOW_H						NX_PRISM_OSD_VERTICAL

#define	NX_PRISM_SCHEDULE_ADD_FRAME_X						0
#define	NX_PRISM_SCHEDULE_ADD_FRAME_Y						0
#define	NX_PRISM_SCHEDULE_ADD_FRAME_W						NX_PRISM_SCHEDULE_ADD_WINDOW_W
#define	NX_PRISM_SCHEDULE_ADD_FRAME_H						NX_PRISM_SCHEDULE_ADD_WINDOW_H

#define	NX_PRISM_SCHEDULE_ADD_TTL_TEXT_X					60
#define	NX_PRISM_SCHEDULE_ADD_TTL_TEXT_Y					(74 - NX_PRISM_FONT_SIZE_36)
#define	NX_PRISM_SCHEDULE_ADD_TTL_TEXT_W					750
#define	NX_PRISM_SCHEDULE_ADD_TTL_TEXT_H					(NX_PRISM_FONT_SIZE_36 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_SCHEDULE_ADD_ITEM_LS						60
#define	NX_PRISM_SCHEDULE_ADD_ITEM_X						60
#define	NX_PRISM_SCHEDULE_ADD_ITEM_Y						103
#define	NX_PRISM_SCHEDULE_ADD_ITEM_W						(750 + NX_PRISM_UI_OPT_MENU_ICON_MG)

#define	NX_PRISM_SCHEDULE_ADD_BTN_OK_X						170
#define	NX_PRISM_SCHEDULE_ADD_BTN_OK_Y						615
#define	NX_PRISM_SCHEDULE_ADD_BTN_OK_W						260
#define	NX_PRISM_SCHEDULE_ADD_BTN_OK_H						55

#define	NX_PRISM_SCHEDULE_ADD_BTN_CANCEL_X					440
#define	NX_PRISM_SCHEDULE_ADD_BTN_CANCEL_Y					615
#define	NX_PRISM_SCHEDULE_ADD_BTN_CANCEL_W					260
#define	NX_PRISM_SCHEDULE_ADD_BTN_CANCEL_H					55

#define	NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_MAX_ITEM		9
#define	NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_CELL_HEIGHT		55
#define	NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WIDTH			290
#define	NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_HEIGHT			(NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_CELL_HEIGHT * NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_MAX_ITEM)
#define	NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_X				(NX_PRISM_SCHEDULE_ADD_WINDOW_X + NX_PRISM_SCHEDULE_ADD_ITEM_X + NX_PRISM_SCHEDULE_ADD_ITEM_W - NX_PRISM_UI_OPT_MENU_ICON_X_TAB)
#define	NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_Y				(NX_PRISM_SCHEDULE_ADD_WINDOW_Y + NX_PRISM_SCHEDULE_ADD_ITEM_Y)

#define	NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WND_X			NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_X
#define	NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WND_Y			NX_PRISM_SCHEDULE_ADD_WINDOW_Y
#define	NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WND_W			NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WIDTH
#define	NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WND_H			NX_PRISM_SCHEDULE_ADD_WINDOW_H

#define	NX_PRISM_SCHEDULE_ADD_DIALOG_INPUT_X				290
#define	NX_PRISM_SCHEDULE_ADD_DIALOG_INPUT_Y				266
#define	NX_PRISM_SCHEDULE_ADD_DIALOG_INPUT_W				700
#define	NX_PRISM_SCHEDULE_ADD_DIALOG_INPUT_H				188

#define	NX_PRISM_SCHEDULE_ADD_DEFAULT_START_TIME_MIN		10
#define	NX_PRISM_SCHEDULE_ADD_DEFAULT_DATE_COUNT			180 //	6 months
#define	NX_PRISM_SCHEDULE_ADD_DEFAULT_DURATION_INTPUT_LEN	4
#define	NX_PRISM_SCHEDULE_ADD_DEFAULT_DURATION_MAX_VALUE	1440


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxUiScheduleAddItem_Channel,
	eNxUiScheduleAddItem_Date,
	eNxUiScheduleAddItem_StartTime,
	eNxUiScheduleAddItem_Duration,
	eNxUiScheduleAddItem_Repeat,
	eNxUiScheduleAddItem_Mode,
	eNxUiScheduleAddItem_BTN_Ok,
	eNxUiScheduleAddItem_BTN_Cancel,
	eNxUiScheduleAddItem_EXPIRE_TIMER_ID,
	eNxUiScheduleAddItem_FRAME_ID,
	eNxUiScheduleAddItem_TTLTEXT_ID,
	eNxUiScheduleAddItem_MAX
} NxUiScheduleAddItem_t;

typedef enum {
	eNxUiScheduleAdd_DialogType_None = 0,
	eNxUiScheduleAdd_DialogType_SwitchSeries,
} NxUiScheduleAdd_DialogType_t;


typedef struct
{
	HUINT32						ulCurFocus;
	HUINT32						ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	HBOOL						bReSchedule;
	HUINT32						ulSlot;

	// Prev Info
	DxSchedule_t				*pstSchedule;
	Nx_ScheduleMode_t			eScheduleModePrev;

	// Currnet Schedule Info
	NX_ChListGroup_t			eChlistGroup;
	HUINT32						ulSvcUid;
	HUINT32						ulTimeStart;
	HUINT32						ulTimeDuration;
	DxRsvRepeat_e				eRepeat;
	HUINT32						ulRepeatDays;
	Nx_ScheduleMode_t			eScheduleMode;

	// Channel Group
	NX_Util_OPTIONList_t		stGroupList;
	NX_ChListGroup_t			eLoadChlistGroup;
	HBOOL						bShowChList;

	// Currnet time for make date list
	HUINT32						ulStartUnixtime;
} NxUiScheduleAddContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static NxUiScheduleAddContent_t			s_stScheduleAddContent;
static Nx_PrismListDialog_Content_t		s_stScheduleAddPopupListContents;
static NX_TimeDialogInstance_t			s_stScheduleAddTimeContents;
static Nx_PrismDialogInput_Content_t	s_stScheduleAddInputContents;
static NXUI_DialogInstance_t			s_stScheduleAddDialogInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_scheduleAdd_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_scheduleAdd_DestroyListPopup(void);
static void 			nx_scheduleAdd_CreateListPopup(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_scheduleAdd_DisplayTimeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_scheduleAdd_DestroyTimeDialog(void);
static void				nx_scheduleAdd_CreateTimeDialog(NxUiScheduleAddContent_t *pstContents, HUINT32 ulInputValue, HUINT8 *pszDlgMsg);

static ONDK_Result_t	nx_scheduleAdd_DisplayDialogInput(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_scheduleAdd_DestroyDialogInput(void);
static void				nx_scheduleAdd_CreateDialogInput(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_scheduleAdd_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_scheduleAdd_CreateResponseDialouge(HUINT8* pszString);

static ONDK_Result_t	nx_scheduleAdd_DialogProc(HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_scheduleAdd_DestroySelectDialog (void);
static ONDK_Result_t	nx_scheduleAdd_CreateSelectDialog(NxUiScheduleAddContent_t *pstContents, NxUiScheduleAdd_DialogType_t eDialogType, HUINT8 *szStrID);

static inline NxUiScheduleAddContent_t* nx_scheduleAdd_GetContents(void);
static HCHAR*			nx_scheduleAdd_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_scheduleAdd_GetMenuItemIdFormObjId(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_scheduleAdd_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_scheduleAdd_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_scheduleAdd_GetItemSupport(NxUiScheduleAddItem_t eNxMenuLanguageItem);
static HBOOL			nx_scheduleAdd_GetItemiDisable(NxUiScheduleAddItem_t eNxMenuLanguageItem);
static void				nx_scheduleAdd_FindNextFocus(NxUiScheduleAddContent_t *pstContents, HBOOL bUp);

static ONDK_Result_t	nx_scheduleAdd_SaveChangeData(NxUiScheduleAddContent_t *pstContents);

static ONDK_Result_t	nx_scheduleAdd_ActionSelectRepeat(NxUiScheduleAddContent_t *pstContents, HINT32 lRepeatDay, HINT32 lRepeat);
static ONDK_Result_t	nx_scheduleAdd_ActionSelectDateList(NxUiScheduleAddContent_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_scheduleAdd_ActionSelectChannelList(NxUiScheduleAddContent_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_scheduleAdd_ActionSelectGroupList(NxUiScheduleAddContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_scheduleAdd_MakeRepeatString(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_MakeDateTextList(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_MakeChannlListTextList(NxUiScheduleAddContent_t *pstContents, NX_ChListGroup_t eChGroupType);
static ONDK_Result_t	nx_scheduleAdd_InitChannlListTextList(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_MakeChannlGroupItemTextList(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_InitChannlGroupItemTextList(NxUiScheduleAddContent_t *pstContents);

static void				nx_scheduleAdd_LoadAlphabetGroupInitDataDefault(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_ChangeAlphabetGroupInitData(NxUiScheduleAddContent_t *pstContents);
static void				nx_scheduleAdd_LoadChannlGroupInitDataDefault(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_ChangeChannlGroupInitData(NxUiScheduleAddContent_t *pstContents);

static ONDK_Result_t	nx_scheduleAdd_UpdateStartTime(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId, HUINT32 ulUnixTime);
static ONDK_Result_t	nx_scheduleAdd_UpdateDuration(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId, HUINT32 ulUnixTime);
static ONDK_Result_t	nx_scheduleAdd_UpdateMode(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_scheduleAdd_UpdateRepeat(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_scheduleAdd_UpdateSubItem(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_scheduleAdd_ChangedSubItemValue(NxUiScheduleAddContent_t *pstContents, HINT32 p1, HINT32 p2);

static ONDK_Result_t	nx_scheduleAdd_AddItemChannel(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_AddItemDate(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_AddItemStartTime(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_AddItemDuration(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_AddItemRepeat(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_AddItemMode(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_LoadItemsValue(NxUiScheduleAddContent_t *pstContents);
static ONDK_Result_t	nx_scheduleAdd_InitSettingsItems(NxUiScheduleAddContent_t *pstContents, HINT32 p1);

static ONDK_Result_t	nx_scheduleAdd_SetUiObject(NxUiScheduleAddContent_t *pstContents);

static ONDK_Result_t	nx_scheduleAdd_MsgGwmCreate(NxUiScheduleAddContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_scheduleAdd_MsgGwmDestroy(NxUiScheduleAddContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_scheduleAdd_MsgGwmHapiMsg(NxUiScheduleAddContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_scheduleAdd_MsgGwmClickMsg(NxUiScheduleAddContent_t *pstContents, HINT32 lMessage, HINT32 p1);
static ONDK_Result_t	nx_scheduleAdd_MsgGwmKeyDown(NxUiScheduleAddContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_scheduleAdd_MsgAppInputValueChange(NxUiScheduleAddContent_t *pstContents, HUINT32 ulInputValue);
static ONDK_Result_t	nx_scheduleAdd_MsgAppDlgClicked(NxUiScheduleAddContent_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_scheduleAdd_MsgAppDlgCanceled(NxUiScheduleAddContent_t *pstContents, HINT32 ulDlgWhoseId);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

#define __Dialog_PopUpList_Functions___________________________________

static ONDK_Result_t	nx_scheduleAdd_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stScheduleAddPopupListContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_scheduleAdd_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_scheduleAdd_DisplayListPopup);
}


static void		nx_scheduleAdd_CreateListPopup(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId)
{
	HUINT32			ulTextListNum = 0, ulIndex = 0, ulAttribute = 0;
	HUINT8			**pptextList = NULL;
	ONDK_Rect_t		stRect;
	Nx_SettingsLeafSubItem_t  *psubItem;

	/* Get subitem matched nObjId */
	psubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, lObjId);
	if (psubItem == NULL)
	{
		NX_APP_Error("Error!!! Invalid ulObjId Id : [0x%x]\n", lObjId);
		return;
	}

	pptextList = psubItem->szItemTextList;
	ulTextListNum = psubItem->ulNumOfItemTextList;
	ulIndex	= psubItem->ulIdxOfTextList;

	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_X, NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_Y,
				NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WIDTH, NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_HEIGHT);

	ulAttribute = (eListDlg_OSD_USER|eListDlg_OSD_BG|eListDlg_SubItemApp);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stScheduleAddPopupListContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_CELL_HEIGHT, ulAttribute,
											ulTextListNum, NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_MAX_ITEM, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stScheduleAddPopupListContents, NX_PRISM_FONT_SIZE_22);

	if (ulAttribute & eListDlg_OSD_USER)
	{
		stRect = ONDK_Rect(NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WND_X, NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WND_Y,
					NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WND_W, NX_PRISM_SCHEDULE_ADD_SUBITEM_POPUP_WND_H);
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stScheduleAddPopupListContents, &stRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_scheduleAdd_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_scheduleAdd_DisplayListPopup, 0, 0, 0, 0);
}


#define __Dialog_Time_Functions________________________________________

static ONDK_Result_t	nx_scheduleAdd_DisplayTimeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_TimeDlalog_ProtoProc(&s_stScheduleAddTimeContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_scheduleAdd_DestroyTimeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_scheduleAdd_DisplayTimeDialog);
}


static void		nx_scheduleAdd_CreateTimeDialog(NxUiScheduleAddContent_t *pstContents, HUINT32 ulInputValue, HUINT8 *pszDlgMsg)
{
	HUINT32			ulAttribute = 0;
	ONDK_Rect_t		stRect = {0, 0, 0, 0};

	stRect = ONDK_Rect(NX_PRISM_UI_CHANGEPIN_DIALOG_X, NX_PRISM_UI_CHANGEPIN_DIALOG_Y, NX_PRISM_UI_CHANGEPIN_DIALOG_WIDTH, NX_PRISM_UI_CHANGEPIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_COMP_TimeDlalog_SetDialog(&s_stScheduleAddTimeContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), pszDlgMsg, ulInputValue, eNxTimeDataType_Schedule);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_scheduleAdd_DisplayTimeDialog", APP_DEFAULT_PRIORITY, nx_scheduleAdd_DisplayTimeDialog, 0, 0, 0, 0);
}

#define __Dialog_Input_Functions_______________________________________

static ONDK_Result_t	nx_scheduleAdd_DisplayDialogInput(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_DialogInput_ProtoProc(&s_stScheduleAddInputContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_scheduleAdd_DestroyDialogInput(void)
{
	return	ONDK_GWM_APP_Destroy(nx_scheduleAdd_DisplayDialogInput);
}


static void		nx_scheduleAdd_CreateDialogInput(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId)
{
	HUINT8			*pszMessage = NULL;
	HINT32			lItemIdx = -1;
	HUINT32			ulAttribute = 0, ulInputLen = 0, ulInputValue = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HxDATETIME_Time_t	stDurationTime;
	Nx_SettingsLeafSubItem_t  *pstSubItem = NULL;

	/* Get subitem matched nObjId */
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, lObjId);
	if ( pstSubItem == NULL )
	{
		NX_APP_Error("[%s:%d] Error!!! pstSubItem is NULL(%d)\n", __FUNCTION__, __HxLINE__);
		return;
	}

	lItemIdx = nx_scheduleAdd_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxUiScheduleAddItem_Duration:
			pszMessage = ONDK_GetString(STR_MESG_3228_ID);
			ulInputLen = NX_PRISM_SCHEDULE_ADD_DEFAULT_DURATION_INTPUT_LEN;

			HxSTD_memset(&stDurationTime, 0x00, sizeof(HxDATETIME_Time_t));
			HLIB_DATETIME_ConvertUnixTimeToTimeWithOutOffset(pstContents->ulTimeDuration, &stDurationTime);
			ulInputValue = ((stDurationTime.ucHour *NX_COMMON_TIME_MINS_PER_HOUR) + stDurationTime.ucMinute);
			break;

		default:
			NX_APP_Error("Error!!! This Object was not support input dialog. lItemIdx :[%d]\n", lItemIdx);
			return;
	}

	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_ADD_DIALOG_INPUT_X, NX_PRISM_SCHEDULE_ADD_DIALOG_INPUT_Y,
						NX_PRISM_SCHEDULE_ADD_DIALOG_INPUT_W, NX_PRISM_SCHEDULE_ADD_DIALOG_INPUT_H);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_COMP_DialogInput_SetDialog (&s_stScheduleAddInputContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, ulAttribute, COL(C_T_Whistle_100), pszMessage, ulInputValue, ulInputLen);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_scheduleAdd_DisplayDialogInput", APP_DEFAULT_PRIORITY, nx_scheduleAdd_DisplayDialogInput, 0, 0, 0, 0);
}


#define __Response_Dialog_Functions___________________________________________

static ONDK_Result_t	nx_scheduleAdd_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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


static void		nx_scheduleAdd_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_scheduleAdd_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_scheduleAdd_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


#define __Select_Dialog_Functions___________________________________________

static ONDK_Result_t	nx_scheduleAdd_DialogProc (HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (&s_stScheduleAddDialogInstance, lMessage, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_scheduleAdd_DestroySelectDialog (void)
{
	ONDK_GWM_APP_Destroy(nx_scheduleAdd_DialogProc);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_CreateSelectDialog(NxUiScheduleAddContent_t *pstContents, NxUiScheduleAdd_DialogType_t eDialogType, HUINT8 *szStrID)
{
	HUINT32 		ulDlgAttr = 0;

	HxSTD_memset (&s_stScheduleAddDialogInstance, 0, sizeof(NXUI_DialogInstance_t));

	s_stScheduleAddDialogInstance.ulDlgWhoseId = eDialogType;
	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(&s_stScheduleAddDialogInstance, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)(HINT32)ONDK_GetString((HINT8*)szStrID));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_scheduleAdd_DialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_scheduleAdd_DialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


#define _________Local_Functions_________________________________________

static inline NxUiScheduleAddContent_t* nx_scheduleAdd_GetContents(void)
{
	return	&s_stScheduleAddContent;
}


static HCHAR*	nx_scheduleAdd_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_scheduleAdd_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxUiScheduleAddItem_Channel:		return	RES_CHANNEL_ID;
		case eNxUiScheduleAddItem_Date:			return	RES_DATE_ID;
		case eNxUiScheduleAddItem_StartTime:	return	RES_START_TIME_ID;
		case eNxUiScheduleAddItem_Duration:		return	RES_DURATION_ID;
		case eNxUiScheduleAddItem_Repeat:		return	RES_REPEAT_ID;
		case eNxUiScheduleAddItem_Mode:			return	RES_MODE_ID;
		default:
			NX_APP_Error("Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_scheduleAdd_GetMenuItemIdFormObjId(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_scheduleAdd_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxUiScheduleAddItem_Repeat:		*pulMenuItemId = NX_MENUITEM_ID_SCHEDULE_REPEAT;				break;
		case eNxUiScheduleAddItem_Mode:			*pulMenuItemId = NX_MENUITEM_ID_SCHEDULE_MODE;					break;
		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static HINT32	nx_scheduleAdd_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = (lObjId & ~NX_PRISM_SCHEDULE_ADD_ITEM_ID);

	if ((lItemId < 0) || (lItemId >= eNxUiScheduleAddItem_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_scheduleAdd_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxUiScheduleAddItem_MAX))
	{
		return 0;
	}

	return	(NX_PRISM_SCHEDULE_ADD_ITEM_ID | lItemId);
}


static HBOOL	nx_scheduleAdd_GetItemSupport(NxUiScheduleAddItem_t eNxMenuLanguageItem)
{
	HBOOL	bSupport = FALSE;

	switch (eNxMenuLanguageItem)
	{
		case eNxUiScheduleAddItem_Channel:
			bSupport = TRUE;
			break;
		case eNxUiScheduleAddItem_Date:
			bSupport = TRUE;
			break;
		case eNxUiScheduleAddItem_StartTime:
			bSupport = TRUE;
			break;
		case eNxUiScheduleAddItem_Duration:
			bSupport = TRUE;
			break;
		case eNxUiScheduleAddItem_Repeat:
			bSupport = TRUE;
			break;
		case eNxUiScheduleAddItem_Mode:
#if defined(CONFIG_MW_MM_PVR)
			bSupport = TRUE;
#else
			bSupport = FALSE;
#endif
			break;

		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_scheduleAdd_GetItemiDisable(NxUiScheduleAddItem_t eNxMenuLanguageItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxMenuLanguageItem)
	{
		case eNxUiScheduleAddItem_Channel:
			bDisable = FALSE;
			break;
		case eNxUiScheduleAddItem_Date:
			bDisable = FALSE;
			break;
		case eNxUiScheduleAddItem_StartTime:
			bDisable = FALSE;
			break;
		case eNxUiScheduleAddItem_Duration:
			bDisable = FALSE;
			break;
		case eNxUiScheduleAddItem_Repeat:
			bDisable = FALSE;
			break;
		case eNxUiScheduleAddItem_Mode:
#if defined(CONFIG_MW_MM_PVR)
			bDisable = FALSE;
#else
			bDisable = TRUE;
#endif
			break;

		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_scheduleAdd_FindNextFocus(NxUiScheduleAddContent_t *pstContents, HBOOL bUp)
{
	HINT32			lFocusObjId = -1, lObjId = -1;
	HUINT32			i = 0, ulSubItemCnt = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);

	lFocusObjId = GWM_GetFocusedObjectID();
	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Channel);
	if ((lFocusObjId == lObjId) && (bUp == TRUE))
	{
		lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_BTN_Cancel);
		if (lObjId > 0)
		{
			ONDK_GWM_Obj_SetFocus(lObjId);
			return;
		}
	}

	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_BTN_Cancel);
	if ((lFocusObjId == lObjId) && (bUp == FALSE))
	{
		lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Channel);
		if (lObjId > 0)
		{
			ONDK_GWM_Obj_SetFocus(lObjId);
			return;
		}
	}

	for (i = 0; i < ulSubItemCnt; i++)
	{
		if ( bUp == TRUE )
		{
			pSubItem =  NX_PRISM_AppContents_GetSubitem((void *)pstContents, (ulSubItemCnt - 1) - i);
		}
		else
		{
			pSubItem =  NX_PRISM_AppContents_GetSubitem((void *)pstContents, i);
		}

		if ((pSubItem == NULL) || (pSubItem->bDisable == TRUE))
		{
			continue;
		}

		pstContents->ulCurFocus = i;
		ONDK_GWM_Obj_SetFocus(pSubItem->ulId);
		break;
	}
}


static ONDK_Result_t	nx_scheduleAdd_SaveChangeData(NxUiScheduleAddContent_t *pstContents)
{
	HBOOL			bChangeTBR = FALSE;
	HUINT32 		ulStartUnixtime = 0;
	DxRsvType_e		eRsvType = DxRSVTYPE_NOTHING;
	DxService_t 	*pstFindService = NULL;
	HxDATETIME_t	stStartDateTiem;
	HERROR			hErr	= ERR_FAIL;


	if (pstContents->eScheduleModePrev != pstContents->eScheduleMode)
	{
		if (pstContents->eScheduleMode == eNxSchedle_ModeRecord)
		{
			switch (pstContents->pstSchedule->eRsvType)
			{
				case DxRSVTYPE_WATCHING_EBR:
					eRsvType = DxRSVTYPE_RECORD_EBR;
					break;
				case DxRSVTYPE_WATCHING_TBR:
					eRsvType = DxRSVTYPE_RECORD_TBR;
					break;
				case DxRSVTYPE_WATCHING_SERIES_EBR:
					eRsvType = DxRSVTYPE_RECORD_SERIES_EBR;
					break;
				default:
					eRsvType = DxRSVTYPE_RECORD_TBR;
					break;
			}

			pstContents->pstSchedule->eRsvType = eRsvType;
		}
		else if (pstContents->eScheduleMode == eNxSchedle_ModeWatching)
		{
			switch (pstContents->pstSchedule->eRsvType)
			{
				case DxRSVTYPE_RECORD_EBR:
					eRsvType = DxRSVTYPE_WATCHING_EBR;
					break;
				case DxRSVTYPE_RECORD_TBR:
					eRsvType = DxRSVTYPE_WATCHING_TBR;
					break;
				case DxRSVTYPE_RECORD_SERIES_EBR:
					eRsvType = DxRSVTYPE_WATCHING_SERIES_EBR;
					break;
				case DxRSVTYPE_RECORD_SATRECORD_TBR:
					eRsvType = DxRSVTYPE_WATCHING_TBR;
					break;
				default:
					eRsvType = DxRSVTYPE_WATCHING_TBR;
					break;
			}

			pstContents->pstSchedule->eRsvType = eRsvType;
		}
		else
		{
			// Not to do..
		}
	}

	// Service Uid
	if (pstContents->pstSchedule->ulSvcUid!= pstContents->ulSvcUid)
	{
		bChangeTBR = TRUE;
		pstContents->pstSchedule->ulSvcUid = pstContents->ulSvcUid;
		pstFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(pstContents->eChlistGroup, eNxFindSvcDir_Current, pstContents->ulSvcUid);
		if (pstFindService == NULL)
		{
			NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() - eChGroupType:(0x%x), svcUid:(0x%x) !!\n", pstContents->eChlistGroup, pstContents->ulSvcUid);
			pstFindService = NX_CHANNEL_FindServiceBySvcUid(pstContents->ulSvcUid);
			if (pstFindService == NULL)
			{
				NX_APP_Error("Error!!! NX_CHANNEL_FindServiceBySvcUid() - svcUid:(0x%x) !!\n", pstContents->eLoadChlistGroup, pstContents->ulSvcUid);
				return	ONDK_RET_FAIL;
			}
		}

		pstContents->pstSchedule->stTripleId.usOnId = (HUINT16)pstFindService->onid;
		pstContents->pstSchedule->stTripleId.usTsId = (HUINT16)pstFindService->tsid;
		pstContents->pstSchedule->stTripleId.usSvcId = (HUINT16)pstFindService->svcid;

		if (pstContents->eScheduleMode == eNxSchedle_ModeWatching)
		{
			HxSTD_snprintf(pstContents->pstSchedule->uExtInfo.stWch.szEvtName, DxRSV_MAX_EVENT_NAME, "%s", pstFindService->name);
		}
		else
		{
			HxSTD_snprintf(pstContents->pstSchedule->uExtInfo.stRec.szEventName, DxRSV_MAX_EVENT_NAME, "%s", pstFindService->name);
		}
	}

	// Start Time
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstContents->pstSchedule->stStartTime, &ulStartUnixtime);
	if (ulStartUnixtime != pstContents->ulTimeStart)
	{
		bChangeTBR = TRUE;
		HxSTD_memset(&stStartDateTiem, 0x00, sizeof(HxDATETIME_t));
		HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->ulTimeStart, &(pstContents->pstSchedule->stStartTime));
	}

	// Duration
	if (pstContents->pstSchedule->ulDuration != pstContents->ulTimeDuration)
	{
		bChangeTBR = TRUE;
		pstContents->pstSchedule->ulDuration = pstContents->ulTimeDuration;
	}

	// Repeat & Repeat Days
	pstContents->pstSchedule->eRepeat = pstContents->eRepeat;
	pstContents->pstSchedule->ulRepeatDays = pstContents->ulRepeatDays;

	if (bChangeTBR == TRUE)
	{
		if (pstContents->eScheduleMode == eNxSchedle_ModeWatching)
		{
			pstContents->pstSchedule->eRsvType = DxRSVTYPE_WATCHING_TBR;
		}
		else if (pstContents->eScheduleMode == eNxSchedle_ModeRecord)
		{
			pstContents->pstSchedule->eRsvType = DxRSVTYPE_RECORD_TBR;
		}
		else
		{
			pstContents->pstSchedule->eRsvType = eRsvType;
		}
	}

	if (pstContents->bReSchedule == TRUE)
	{
		NX_SCHEDULER_UpdateSchedule(pstContents->pstSchedule);
	}
	else
	{
		hErr = NX_SCHEDULER_AddSchedule(pstContents->pstSchedule);
		if (ERR_OK != hErr)
		{
			NX_APP_Error("Error!!! APK_META_SCHEDULE_AddSchedule() Fail!!\n");
			return	ONDK_RET_FAIL;
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_ActionSelectRepeat(NxUiScheduleAddContent_t *pstContents, HINT32 lRepeatDay, HINT32 lRepeat)
{
	HINT32				lObjId = -1;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Repeat);
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() - eNxUiScheduleAddItem_Repeat !!\n");
		return	ONDK_RET_FAIL;
	}

	pSubItem->lSubItemValue = lRepeat;
	pstContents->eRepeat = (DxRsvRepeat_e)lRepeat;
	pstContents->ulRepeatDays = lRepeatDay;

	nx_scheduleAdd_MakeRepeatString(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_ActionSelectDateList(NxUiScheduleAddContent_t *pstContents, HUINT32 ulCurIdx)
{
	HCHAR				*pszDayOfWeek = NULL;
	HINT32				lObjId = -1;
	HUINT32 			ulIndex = 0, ulStartUnixtime = 0, ulWeekIndex = 0;
	HxDATETIME_Date_t	stDate;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Date);
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() - eNxUiScheduleAddItem_Date !!\n");
		return	ONDK_RET_FAIL;
	}

	if (pSubItem->ulIdxOfTextList == ulCurIdx)
	{
		NX_APP_Info("Skip same item list !!\n");
		return	ONDK_RET_OK;
	}

	ulStartUnixtime = pstContents->ulStartUnixtime;

	for (ulIndex = 0; ulIndex < ulCurIdx; ulIndex++)
	{
		ulStartUnixtime += NX_COMMON_TIME_SECONDS_PER_DAY;
	}

	HxSTD_memset(&stDate, 0x00, sizeof(HxDATETIME_Date_t));
	HLIB_DATETIME_ConvertUnixTimeToDate(ulStartUnixtime, &stDate);
	ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stDate);
	pszDayOfWeek = (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex));
	HxSTD_snprintf((char*)pSubItem->szSubItemText, MAX_ELEMENT_TEXT_LENGTH, "%s %02d.%02d", pszDayOfWeek, stDate.ucMonth, stDate.ucDay);
	pSubItem->lSubItemValue = (HINT32)ulStartUnixtime;
	pSubItem->ulIdxOfTextList = ulCurIdx;

	nx_scheduleAdd_UpdateStartTime(pstContents, lObjId, ulStartUnixtime);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_ActionSelectChannelList(NxUiScheduleAddContent_t *pstContents, HUINT32 ulCurIdx)
{
	HERROR				hError = ERR_FAIL;
	HINT32				lObjId = -1, lChListNum = 0;
	HUINT32 			ulExtraFilter = 0;
	NX_Channel_t		*pstChannelList = NULL;
	DxService_t 		*pstFindService = NULL;
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Channel);
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() - eNxUiScheduleAddItem_Channel !!\n");
		return	ONDK_RET_FAIL;
	}

	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(pstContents->eLoadChlistGroup);
	hError = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstChannelList, &lChListNum, pstContents->eLoadChlistGroup, ulExtraFilter);
	if ((hError != ERR_OK) || (lChListNum < 1))
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_MakeNxChannelListGroup() - eChGroupType:(0x%x), ulExtraFilter:(0x%x), lChListNum:(%d)!!\n", pstContents->eLoadChlistGroup, ulExtraFilter, lChListNum);
		if (pstChannelList)
		{
			NX_APP_Free(pstChannelList);
			pstChannelList = NULL;
		}
		return ONDK_RET_FAIL;
	}

	if (ulCurIdx >= (HUINT32)lChListNum)
	{
		NX_APP_Error("Error!!! Out of Index - ulCurIdx:(%d), ulChListNum:(%d) !!\n", ulCurIdx, lChListNum);
		if (pstChannelList)
		{
			NX_APP_Free(pstChannelList);
			pstChannelList = NULL;
		}
		return ONDK_RET_FAIL;
	}

	if (pstChannelList == NULL)
	{
		NX_APP_Error("Error!!! (pstChannelList == NULL) Fail !!\n");
		return ONDK_RET_FAIL;
	}

	pstFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(pstContents->eLoadChlistGroup, eNxFindSvcDir_Current, pstChannelList[ulCurIdx].svcUid);
	if (pstFindService == NULL)
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() - eChGroupType:(0x%x), svcUid:(0x%x) !!\n", pstContents->eLoadChlistGroup, pstChannelList[ulCurIdx].svcUid);
		pstFindService = NX_CHANNEL_FindServiceBySvcUid(pstChannelList[ulCurIdx].svcUid);
	}

	if (pstFindService == NULL)
	{
		NX_APP_Error("Error!!! NX_CHANNEL_FindServiceBySvcUid() Fail !!\n");
		if (pstChannelList)
		{
			NX_APP_Free(pstChannelList);
			pstChannelList = NULL;
		}
		return ONDK_RET_FAIL;
	}

	pSubItem->lSubItemValue = (HINT32)pstChannelList[ulCurIdx].svcUid;
	HxSTD_snprintf((char*)pSubItem->szSubItemText, MAX_ELEMENT_TEXT_LENGTH, "%04d %s", pstChannelList[ulCurIdx].lcn, (char*)pstFindService->name);

	pstContents->ulSvcUid = pstChannelList[ulCurIdx].svcUid;
	pstContents->eChlistGroup = pstContents->eLoadChlistGroup;
	pstContents->bShowChList = FALSE;

	if (pstChannelList)
	{
		NX_APP_Free(pstChannelList);
		pstChannelList = NULL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_ActionSelectGroupList(NxUiScheduleAddContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32					lObjId = -1;
	NX_ChListGroup_t		eChGroupType = eNxChListGroup_TV;

	if (pstContents->stGroupList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	eChGroupType = (NX_ChListGroup_t)pstContents->stGroupList.aeOptItemList[ulCurIdx];

	switch (eChGroupType)
	{
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
			lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Channel);
			nx_scheduleAdd_InitChannlGroupItemTextList(pstContents);
			nx_scheduleAdd_MakeChannlListTextList(pstContents, eChGroupType);
			nx_scheduleAdd_CreateListPopup(pstContents, lObjId);
			break;

		case eNxChListGroup_ALPHABET:
			lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Channel);
			nx_scheduleAdd_ChangeAlphabetGroupInitData(pstContents);
			nx_scheduleAdd_MakeChannlGroupItemTextList(pstContents);
			nx_scheduleAdd_CreateListPopup(pstContents, lObjId);
			break;

		default:
			NX_APP_Info("Error!!! Not Support Group Item [0x%x] !!\n", eChGroupType);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_MakeRepeatString(NxUiScheduleAddContent_t *pstContents)
{
	HBOOL		bFirstItem = TRUE;
	HINT8		*pszStringId = NULL;
	HUINT8		szItemText[MAX_ELEMENT_TEXT_LENGTH], szTextBuffer[NX_TEXT_SIZE_4], *pszText = NULL;
	HINT32		lObjId = -1;
	HUINT32		ulRemainLen = MAX_ELEMENT_TEXT_LENGTH, ulTextLen = 0, ulTotalTextLen = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Repeat);
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() - eNxUiScheduleAddItem_Repeat !!\n");
		return	ONDK_RET_FAIL;
	}

	if (pstContents->eRepeat == DxRSVREPEAT_EndOfRepeatCase)
	{
		pszStringId = RES_ONCE_ID;
	}
	else if ((pstContents->pstSchedule->eRsvType == DxRSVTYPE_WATCHING_EBR) || (pstContents->pstSchedule->eRsvType == DxRSVTYPE_RECORD_EBR))
	{
		pszStringId = RES_ONCE_ID;
	}
	else if ((pstContents->pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR) || (pstContents->pstSchedule->eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR))
	{
		pszStringId = RES_AUTO_ID;
	}
	else if (pstContents->eRepeat == DxRSVREPEAT_ONCE)
	{
		pszStringId = RES_ONCE_ID;
	}
	else if (pstContents->eRepeat == DxRSVREPEAT_DAILY)
	{
		pszStringId = RES_DAILY_ID;
	}
	else if (pstContents->eRepeat == DxRSVREPEAT_WEEKLY)
	{
		pszStringId = RES_WEEKLY_ID;
	}
	else
	{
		pszStringId = NULL;

		HxSTD_memset (szItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
		szItemText[0] = '\0';

		HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_4);
		szTextBuffer[0] = '\0';
		HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_4, (char*)", ");

		if (pstContents->ulRepeatDays & eDxSCHED_DATE_SUN)
		{
			if (bFirstItem == FALSE)
			{
				ulTotalTextLen += NX_TEXT_SIZE_4;
				if (ulRemainLen > ulTotalTextLen)
				{
					HxSTD_StrNCat((char*)szItemText, (HCHAR*)szTextBuffer, NX_TEXT_SIZE_4);
				}
			}

			pszText = ONDK_GetString(RES_SUN_ID);
			ulTextLen = HxSTD_StrLen((char*)pszText);
			ulTotalTextLen += ulTextLen;
			if (ulRemainLen > ulTotalTextLen)
			{
				HxSTD_StrNCat((char*)szItemText, (HCHAR*)pszText, ulTextLen);
			}
			bFirstItem = FALSE;
		}
		if (pstContents->ulRepeatDays & eDxSCHED_DATE_MON)
		{
			if (bFirstItem == FALSE)
			{
				ulTotalTextLen += NX_TEXT_SIZE_4;
				if (ulRemainLen > ulTotalTextLen)
				{
					HxSTD_StrNCat((char*)szItemText, (HCHAR*)szTextBuffer, NX_TEXT_SIZE_4);
				}
			}

			pszText = ONDK_GetString(RES_MON_ID);
			ulTextLen = HxSTD_StrLen((char*)pszText);
			ulTotalTextLen += ulTextLen;
			if (ulRemainLen > ulTotalTextLen)
			{
				HxSTD_StrNCat((char*)szItemText, (HCHAR*)pszText, ulTextLen);
			}
			bFirstItem = FALSE;
		}
		if (pstContents->ulRepeatDays & eDxSCHED_DATE_TUE)
		{
			if (bFirstItem == FALSE)
			{
				ulTotalTextLen += NX_TEXT_SIZE_4;
				if (ulRemainLen > ulTotalTextLen)
				{
					HxSTD_StrNCat((char*)szItemText, (HCHAR*)szTextBuffer, NX_TEXT_SIZE_4);
				}
			}

			pszText = ONDK_GetString(RES_TUE_ID);
			ulTextLen = HxSTD_StrLen((char*)pszText);
			ulTotalTextLen += ulTextLen;
			if (ulRemainLen > ulTotalTextLen)
			{
				HxSTD_StrNCat((char*)szItemText, (HCHAR*)pszText, ulTextLen);
			}
			bFirstItem = FALSE;
		}
		if (pstContents->ulRepeatDays & eDxSCHED_DATE_WED)
		{
			if (bFirstItem == FALSE)
			{
				ulTotalTextLen += NX_TEXT_SIZE_4;
				if (ulRemainLen > ulTotalTextLen)
				{
					HxSTD_StrNCat((char*)szItemText, (HCHAR*)szTextBuffer, NX_TEXT_SIZE_4);
				}
			}

			pszText = ONDK_GetString(RES_WED_ID);
			ulTextLen = HxSTD_StrLen((char*)pszText);
			ulTotalTextLen += ulTextLen;
			if (ulRemainLen > ulTotalTextLen)
			{
				HxSTD_StrNCat((char*)szItemText, (HCHAR*)pszText, ulTextLen);
			}
			bFirstItem = FALSE;
		}
		if (pstContents->ulRepeatDays & eDxSCHED_DATE_THU)
		{
			if (bFirstItem == FALSE)
			{
				ulTotalTextLen += NX_TEXT_SIZE_4;
				if (ulRemainLen > ulTotalTextLen)
				{
					HxSTD_StrNCat((char*)szItemText, (HCHAR*)szTextBuffer, NX_TEXT_SIZE_4);
				}
			}

			pszText = ONDK_GetString(RES_THU_ID);
			ulTextLen = HxSTD_StrLen((char*)pszText);
			ulTotalTextLen += ulTextLen;
			if (ulRemainLen > ulTotalTextLen)
			{
				HxSTD_StrNCat((char*)szItemText, (HCHAR*)pszText, ulTextLen);
			}
			bFirstItem = FALSE;
		}
		if (pstContents->ulRepeatDays & eDxSCHED_DATE_FRI)
		{
			if (bFirstItem == FALSE)
			{
				ulTotalTextLen += NX_TEXT_SIZE_4;
				if (ulRemainLen > ulTotalTextLen)
				{
					HxSTD_StrNCat((char*)szItemText, (HCHAR*)szTextBuffer, NX_TEXT_SIZE_4);
				}
			}

			pszText = ONDK_GetString(RES_FRI_ID);
			ulTextLen = HxSTD_StrLen((char*)pszText);
			ulTotalTextLen += ulTextLen;
			if (ulRemainLen > ulTotalTextLen)
			{
				HxSTD_StrNCat((char*)szItemText, (HCHAR*)pszText, ulTextLen);
			}
			bFirstItem = FALSE;
		}
		if (pstContents->ulRepeatDays & eDxSCHED_DATE_SAT)
		{
			if (bFirstItem == FALSE)
			{
				ulTotalTextLen += NX_TEXT_SIZE_4;
				if (ulRemainLen > ulTotalTextLen)
				{
					HxSTD_StrNCat((char*)szItemText, (HCHAR*)szTextBuffer, NX_TEXT_SIZE_4);
				}
			}

			pszText = ONDK_GetString(RES_SAT_ID);
			ulTextLen = HxSTD_StrLen((char*)pszText);
			ulTotalTextLen += ulTextLen;
			if (ulRemainLen > ulTotalTextLen)
			{
				HxSTD_StrNCat((char*)szItemText, (HCHAR*)pszText, ulTextLen);
			}

			bFirstItem = FALSE;
		}
	}

	if (pszStringId)
	{
		HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (HCHAR*)ONDK_GetString(pszStringId), MAX_ELEMENT_TEXT_LENGTH);
	}
	else
	{
		HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (HCHAR*)szItemText, MAX_ELEMENT_TEXT_LENGTH);
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_MakeDateTextList(NxUiScheduleAddContent_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HCHAR				*pszDayOfWeek = NULL;
	HUINT8				**ppListOfItem = NULL;
	HINT32				lObjId = -1;
	HUINT32 			ulIndex = 0, ulIdxOfTextList = 0, ulWeekIndex = 0,  ulStartUnixtime = 0;
	HxDATETIME_Date_t	stDate, stStartDate;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Date);
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() - eNxUiScheduleAddItem_Date !!\n");
		return	ONDK_RET_FAIL;
	}

	if (pSubItem->szItemTextList != NULL)
	{

		NX_APP_Error("Error!!! Alreay Make - eNxUiScheduleAddItem_Date !!\n");
		return	ONDK_RET_FAIL;
	}

	// Time
	hError = NX_SYSTEM_GetTime(&ulStartUnixtime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_SYSTEM_GetTime() Fail !!\n");
		return	ONDK_RET_FAIL;
	}

	// Set the selector item list
	ppListOfItem = NX_PRISM_MENU_MenuItem_AllocTextList (NX_PRISM_SCHEDULE_ADD_DEFAULT_DATE_COUNT, NX_TEXT_SIZE_32);
	if (ppListOfItem != NULL)
	{
		pstContents->ulStartUnixtime = ulStartUnixtime;

		HxSTD_memset(&stStartDate, 0x00, sizeof(HxDATETIME_Date_t));
		HLIB_DATETIME_ConvertUnixTimeToDate(pstContents->ulTimeStart, &stStartDate);

		for (ulIndex = 0; ulIndex < NX_PRISM_SCHEDULE_ADD_DEFAULT_DATE_COUNT; ulIndex++)
		{
			HxSTD_memset(&stDate, 0x00, sizeof(HxDATETIME_Date_t));
			HLIB_DATETIME_ConvertUnixTimeToDate(ulStartUnixtime, &stDate);
			ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stDate);
			pszDayOfWeek = (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex));
			HxSTD_snprintf ((char*)ppListOfItem[ulIndex], NX_TEXT_SIZE_32, (char*)"%s %02d.%02d", pszDayOfWeek, stDate.ucMonth, stDate.ucDay);
			ulStartUnixtime += NX_COMMON_TIME_SECONDS_PER_DAY;
			if ((stDate.usYear == stStartDate.usYear) && (stDate.ucMonth == stStartDate.ucMonth) && (stDate.ucDay == stStartDate.ucDay))
			{
				ulIdxOfTextList = ulIndex;
			}
		}

		pSubItem->ulNumOfItemTextList = NX_PRISM_SCHEDULE_ADD_DEFAULT_DATE_COUNT;
		pSubItem->szItemTextList = ppListOfItem;
		pSubItem->ulIdxOfTextList = ulIdxOfTextList;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_MakeChannlListTextList(NxUiScheduleAddContent_t *pstContents, NX_ChListGroup_t eChGroupType)
{
	HERROR				hError = ERR_FAIL;
	HINT32				lObjId = -1, lChListNum = 0;
	HUINT32 			ulIndex = 0, ulExtraFilter = 0, ulChListNum = 0, ulIdxOfTextList = 0;
	HUINT8				**ppListOfItem = NULL;
	NX_Channel_t		*pstChannelList = NULL;
	DxService_t 		*pstFindService = NULL;
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Channel);
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() - eNxUiScheduleAddItem_Channel !!\n");
		return	ONDK_RET_FAIL;
	}

	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eChGroupType);
	hError = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstChannelList, &lChListNum, eChGroupType, ulExtraFilter);
	if ((hError != ERR_OK) || (lChListNum < 1))
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_MakeNxChannelListGroup() - eChGroupType:(0x%x), ulExtraFilter:(0x%x), lChListNum:(%d)!!\n", eChGroupType, ulExtraFilter, lChListNum);
		if (pstChannelList)
		{
			NX_APP_Free(pstChannelList);
			pstChannelList = NULL;
		}
		return ONDK_RET_FAIL;
	}

	if (pstChannelList == NULL)
	{
		NX_APP_Error("Error!!! (pstChannelList == NULL) Fail !!\n");
		return ONDK_RET_FAIL;
	}

	ulChListNum = (HUINT32)lChListNum;
	pstContents->bShowChList = TRUE;
	pstContents->eLoadChlistGroup = eChGroupType;

	// Set the selector item list
	ppListOfItem = NX_PRISM_MENU_MenuItem_AllocTextList (ulChListNum, DxNAME_LEN);
	if (ppListOfItem != NULL)
	{
		for (ulIndex = 0; ulIndex < ulChListNum; ulIndex++)
		{
			pstFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eChGroupType, eNxFindSvcDir_Current, pstChannelList[ulIndex].svcUid);
			if (pstFindService)
			{
				if (pstChannelList[ulIndex].svcUid == pstContents->ulSvcUid)
				{
					ulIdxOfTextList = ulIndex;
				}
				HxSTD_snprintf ((char*)ppListOfItem[ulIndex], DxNAME_LEN, (char*)"%04d %s", pstChannelList[ulIndex].lcn, pstFindService->name);
			}
			else
			{
				HxSTD_StrNCpy ((char*)ppListOfItem[ulIndex], (char*)ONDK_GetString(RES_UNKNOWN_ID), DxNAME_LEN);
			}
		}

		pSubItem->ulNumOfItemTextList = ulChListNum;
		pSubItem->szItemTextList = ppListOfItem;
		pSubItem->ulIdxOfTextList = ulIdxOfTextList;
	}

	if (pstChannelList)
	{
		NX_APP_Free(pstChannelList);
		pstChannelList = NULL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_MakeChannlGroupItemTextList(NxUiScheduleAddContent_t *pstContents)
{
	HINT32			lObjId = -1;
	HUINT32			i = 0, ulIdxOfTextList = 0;
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;


	pstContents->bShowChList = FALSE;
	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Channel);
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() - eNxUiScheduleAddItem_Channel !!\n");
		return	ONDK_RET_FAIL;
	}

	pSubItem->szItemTextList = NX_PRISM_UTIL_OptionMakeChannlGroupItemTextList(eNxUtil_OptType_TEXT, &pstContents->stGroupList);
	if (pSubItem->szItemTextList == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_UTIL_OptionMakeChannlGroupItemTextList() - Fail !!\n");
		return	ONDK_RET_FAIL;
	}

	for (i = 0; i < pstContents->stGroupList.ulOptItemNum; i++)
	{
		if (pstContents->eChlistGroup == (NX_ChListGroup_t)pstContents->stGroupList.aeOptItemList[i])
		{
			ulIdxOfTextList = i;
		}

		if (((pstContents->eChlistGroup >= eNxChListGroup_AlphabetAll) && (pstContents->eChlistGroup <= eNxChListGroup_AlphabetEtc))
			&& ((NX_ChListGroup_t)pstContents->stGroupList.aeOptItemList[i] == eNxChListGroup_ALPHABET))
		{
			ulIdxOfTextList = i;
		}
	}

	pSubItem->ulNumOfItemTextList = pstContents->stGroupList.ulOptItemNum;
	pSubItem->ulIdxOfTextList = ulIdxOfTextList;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_InitChannlGroupItemTextList(NxUiScheduleAddContent_t *pstContents)
{
	HINT32			lObjId = -1;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Channel);
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() - eNxUiScheduleAddItem_Channel !!\n");
		return	ONDK_RET_FAIL;
	}

	if (pSubItem->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stGroupList, pSubItem->szItemTextList);
		pSubItem->szItemTextList = NULL;
		pSubItem->ulNumOfItemTextList = 0;
	}

	HxSTD_MemSet(&pstContents->stGroupList, 0x00, sizeof(NX_Util_OPTIONList_t));
	pstContents->bShowChList = FALSE;

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_InitChannlListTextList(NxUiScheduleAddContent_t *pstContents)
{
	HINT32			lObjId = -1;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Channel);
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() - eNxUiScheduleAddItem_Channel !!\n");
		return	ONDK_RET_FAIL;
	}

	if (pSubItem->szItemTextList)
	{
		NX_PRISM_MENU_MenuItem_FreeTextList(pSubItem->ulNumOfItemTextList, pSubItem->szItemTextList);
		pSubItem->szItemTextList = NULL;
		pSubItem->ulNumOfItemTextList = 0;
	}

	pstContents->bShowChList = FALSE;

	return ONDK_RET_OK;
}


static void		nx_scheduleAdd_LoadAlphabetGroupInitDataDefault(NxUiScheduleAddContent_t *pstContents)
{
	HUINT32			ulCount = 0;

	HxSTD_MemSet(&pstContents->stGroupList, 0x00, sizeof(NX_Util_OPTIONList_t));

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetAll);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetAll);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetA);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetA);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetB);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetB);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetC);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetC);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetD);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetD);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetE);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetE);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetF);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetF);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetG);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetG);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetH);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetH);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetI);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetI);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetJ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetJ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetK);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetK);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetL);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetL);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetM);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetM);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetN);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetN);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetO);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetO);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetP);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetP);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetQ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetQ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetR);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetR);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetS);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetS);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetT);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetT);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetU);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetU);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetV);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetV);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetW);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetW);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetX);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetX);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetY);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetY);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetZ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetZ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetDigit);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetDigit);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetEtc);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_AlphabetEtc);
}


static ONDK_Result_t	nx_scheduleAdd_ChangeAlphabetGroupInitData(NxUiScheduleAddContent_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;


	eResult = nx_scheduleAdd_InitChannlGroupItemTextList(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_scheduleAdd_InitChannlGroupItemTextList() Fail !!\n");
		return	ONDK_RET_FAIL;
	}

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add Other Change Group for Operator if you need

		default:
			nx_scheduleAdd_LoadAlphabetGroupInitDataDefault(pstContents);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static void		nx_scheduleAdd_LoadChannlGroupInitDataDefault(NxUiScheduleAddContent_t *pstContents)
{
	HUINT32			ulCount = 0;

	HxSTD_MemSet(&pstContents->stGroupList, 0x00, sizeof(NX_Util_OPTIONList_t));

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_TV);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_TV);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_RADIO);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_RADIO);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_HDTV);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_HDTV);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_FTA);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_FTA);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_CAS);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_CAS);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_ALPHABET);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_ALPHABET);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite1);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_Favorite1);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite2);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_Favorite2);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite3);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_Favorite3);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite4);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_Favorite4);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite5);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stGroupList, eNxChListGroup_Favorite5);

}


static ONDK_Result_t	nx_scheduleAdd_ChangeChannlGroupInitData(NxUiScheduleAddContent_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;


	eResult = nx_scheduleAdd_InitChannlGroupItemTextList(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_scheduleAdd_InitChannlGroupItemTextList() Fail !!\n");
		return	ONDK_RET_FAIL;
	}

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add Other Change Group for Operator if you need

		default:
			nx_scheduleAdd_LoadChannlGroupInitDataDefault(pstContents);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_UpdateStartTime(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId, HUINT32 ulUnixTime)
{
	HINT32				lItemIdx = -1;
	HUINT32 			ulStartUnixtime = 0;
	HxDATETIME_t		stStartDate;
	HxDATETIME_Time_t	stTime;
	HxDATETIME_Date_t	stDate;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() Item Id : [0x%x]\n", lObjId);
		return	ONDK_RET_FAIL;
	}

	// Update Start Schedule Date
	HxSTD_memset(&stStartDate, 0x00, sizeof(HxDATETIME_t));
	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->ulTimeStart, &stStartDate);

	lItemIdx = nx_scheduleAdd_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxUiScheduleAddItem_Date:
			HLIB_DATETIME_ConvertUnixTimeToDate(ulUnixTime, &stDate);
			stStartDate.stDate.usYear = stDate.usYear;
			stStartDate.stDate.ucMonth = stDate.ucMonth;
			stStartDate.stDate.ucDay = stDate.ucDay;
			HLIB_DATETIME_ConvertDateTimeToUnixTime(&stStartDate, &ulStartUnixtime);
			pstContents->ulTimeStart = ulStartUnixtime;
			pSubItem->lSubItemValue = (HINT32)pstContents->ulTimeStart;
			break;

		case eNxUiScheduleAddItem_StartTime:
			HLIB_DATETIME_ConvertUnixTimeToTime(ulUnixTime, &stTime);
			stStartDate.stTime.ucHour = stTime.ucHour;
			stStartDate.stTime.ucMinute = stTime.ucMinute;
			stStartDate.stTime.ucSecond = stTime.ucSecond;
			stStartDate.stTime.usMillisecond = stTime.usMillisecond;
			HLIB_DATETIME_ConvertDateTimeToUnixTime(&stStartDate, &ulStartUnixtime);
			pstContents->ulTimeStart = ulStartUnixtime;
			pSubItem->lSubItemValue = (HINT32)pstContents->ulTimeStart;
			break;

		default:
			NX_APP_Error("Error!!! nx_scheduleAdd_UpdateStartTime() Not Support Item Id : [0x%x]\n", lObjId);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_UpdateDuration(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId, HUINT32 ulUnixTime)
{
	HxDATETIME_Time_t			stTime;
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() Item Id : [0x%x]\n", lObjId);
		return	ONDK_RET_FAIL;
	}

	pSubItem->lSubItemValue = (HINT32)ulUnixTime;
	pstContents->ulTimeDuration = ulUnixTime;

	HxSTD_memset(&stTime, 0x00, sizeof(HxDATETIME_Time_t));
	HLIB_DATETIME_ConvertUnixTimeToTimeWithOutOffset(pstContents->ulTimeDuration, &stTime);

	HxSTD_snprintf((char*)pSubItem->szSubItemText, MAX_ELEMENT_TEXT_LENGTH, "%d %s",
					((stTime.ucHour *NX_COMMON_TIME_MINS_PER_HOUR) + stTime.ucMinute), ONDK_GetString(RES_MINS_ID));

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_UpdateMode(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() Item Id : [0x%x]\n", lObjId);
		return	ONDK_RET_FAIL;
	}

	if (pstContents->eScheduleMode!= (Nx_ScheduleMode_t)pSubItem->lSubItemValue)
	{
		pstContents->eScheduleMode = (Nx_ScheduleMode_t)pSubItem->lSubItemValue;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_UpdateRepeat(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pSubItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() Item Id : [0x%x]\n", lObjId);
		return	ONDK_RET_FAIL;
	}

	if (pstContents->eRepeat!= (DxRsvRepeat_e)pSubItem->lSubItemValue)
	{
		pstContents->eRepeat = (DxRsvRepeat_e)pSubItem->lSubItemValue;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_UpdateSubItem(NxUiScheduleAddContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_scheduleAdd_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxUiScheduleAddItem_Channel:
		case eNxUiScheduleAddItem_Date:
		case eNxUiScheduleAddItem_StartTime:
		case eNxUiScheduleAddItem_Duration:
			break;

		case eNxUiScheduleAddItem_Repeat:
			nx_scheduleAdd_UpdateRepeat(pstContents, lObjId);
			break;

		case eNxUiScheduleAddItem_Mode:
			nx_scheduleAdd_UpdateMode(pstContents, lObjId);
			break;

		default:
			NX_APP_Error("Error!! Invalid Item Not Support lObjId:(0x%x), lItemIdx(%d)\n", lObjId, lItemIdx);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_ChangedSubItemValue(NxUiScheduleAddContent_t *pstContents, HINT32 p1, HINT32 p2)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0, ulCurIdx = (HUINT32)p1;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_scheduleAdd_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxUiScheduleAddItem_Channel:
			if (pstContents->bShowChList == FALSE)	// Group List
			{
				nx_scheduleAdd_DestroyListPopup();
				nx_scheduleAdd_ActionSelectGroupList(pstContents, ulCurIdx);
			}
			else									// Channel list
			{
				nx_scheduleAdd_DestroyListPopup();
				nx_scheduleAdd_ActionSelectChannelList(pstContents, ulCurIdx);
				nx_scheduleAdd_InitChannlListTextList(pstContents);
			}
			break;
		case eNxUiScheduleAddItem_Date:
			nx_scheduleAdd_ActionSelectDateList(pstContents, ulCurIdx);
			break;
		case eNxUiScheduleAddItem_StartTime:
		case eNxUiScheduleAddItem_Duration:
			break;

		case eNxUiScheduleAddItem_Repeat:
			nx_scheduleAdd_ActionSelectRepeat(pstContents, p1, p2);
			break;

		case eNxUiScheduleAddItem_Mode:
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
			nx_scheduleAdd_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

			/* Update value */
			nx_scheduleAdd_UpdateSubItem(pstContents, lObjId);
			break;

		default:
			NX_APP_Error("Error!! Invalid Item Not Support lObjId:(0x%x), lItemIdx(%d)\n", lObjId, lItemIdx);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_AddItemChannel(NxUiScheduleAddContent_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HCHAR				*pMainStrId = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	HUINT32				ulSubItemCnt = 0;
	DxService_t			*pstService = NULL;
	NX_Channel_t		stChannel;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! pstContents == NULL!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_scheduleAdd_GetItemSupport(eNxUiScheduleAddItem_Channel);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUiScheduleAddItem_Channel(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Channel);

	// Check Disable Item
	bDisable = nx_scheduleAdd_GetItemiDisable(eNxUiScheduleAddItem_Channel);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_scheduleAdd_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_SCHEDULE_ADD_ITEM_X;
	stSubItem.rect.y = NX_PRISM_SCHEDULE_ADD_ITEM_Y + (ulSubItemCnt * NX_PRISM_SCHEDULE_ADD_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_SCHEDULE_ADD_ITEM_W;
	stSubItem.rect.h = NX_PRISM_SCHEDULE_ADD_ITEM_LS;

	stSubItem.ulNumOfItemTextList = 0;
	stSubItem.ulIdxOfTextList = 0;
	stSubItem.szItemTextList = NULL;
	stSubItem.lSubItemValue = (HINT32)pstContents->pstSchedule->ulSvcUid;

	pstService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(pstContents->eChlistGroup, eNxFindSvcDir_Current, pstContents->pstSchedule->ulSvcUid);
	if (pstService == NULL)
	{
		NX_APP_Error("Error!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() Fail - eChlistGroup:(0x%x), ulSvcUid:(0x%x)!! \n", pstContents->eChlistGroup, pstContents->pstSchedule->ulSvcUid);
		pstService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_TV, eNxFindSvcDir_Current, pstContents->pstSchedule->ulSvcUid);
		if (pstService == NULL)
		{
			NX_APP_Error("Error!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() Fail - eChlistGroup:eNxChListGroup_TV, ulSvcUid:(0x%x)!! \n", pstContents->pstSchedule->ulSvcUid);
			pstService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_RADIO, eNxFindSvcDir_Current, pstContents->pstSchedule->ulSvcUid);
			if (pstService == NULL)
			{
				NX_APP_Error("Error!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() Fail - eChlistGroup:eNxChListGroup_RADIO, ulSvcUid:(0x%x)!! \n", pstContents->pstSchedule->ulSvcUid);
				return	ONDK_RET_INITFAIL;
			}
			else
			{
				pstContents->eChlistGroup = eNxChListGroup_RADIO;
			}
		}
		else
		{
			 pstContents->eChlistGroup = eNxChListGroup_TV;
		}
	}

	HxSTD_MemSet(&stChannel, 0x00, sizeof(NX_Channel_t));
	hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(pstContents->eChlistGroup, pstContents->pstSchedule->ulSvcUid, &stChannel);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!! NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() eChlistGroup:(0x%x), ulSvcUid:(0x%x) Fail!! \n", pstContents->eChlistGroup, pstContents->pstSchedule->ulSvcUid);
		return	ONDK_RET_FAIL;
	}
	HxSTD_snprintf((char*)stSubItem.szSubItemText, MAX_ELEMENT_TEXT_LENGTH, "%04d %s", stChannel.lcn, (char*)pstService->name);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_AddItemDate(NxUiScheduleAddContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL, *pszDayOfWeek = NULL;
	HUINT32				ulSubItemCnt = 0, ulWeekIndex = 0;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	HxDATETIME_Date_t	stDate;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! pstContents == NULL!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_scheduleAdd_GetItemSupport(eNxUiScheduleAddItem_Date);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUiScheduleAddItem_Date(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Date);

	// Check bDisable Item
	bDisable = nx_scheduleAdd_GetItemiDisable(eNxUiScheduleAddItem_Date);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_scheduleAdd_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_SCHEDULE_ADD_ITEM_X;
	stSubItem.rect.y = NX_PRISM_SCHEDULE_ADD_ITEM_Y + (ulSubItemCnt * NX_PRISM_SCHEDULE_ADD_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_SCHEDULE_ADD_ITEM_W;
	stSubItem.rect.h = NX_PRISM_SCHEDULE_ADD_ITEM_LS;

	stSubItem.ulNumOfItemTextList = 0;
	stSubItem.ulIdxOfTextList = 0;
	stSubItem.lSubItemValue = (HINT32)pstContents->ulTimeStart;

	HxSTD_memset(&stDate, 0x00, sizeof(HxDATETIME_Date_t));
	HLIB_DATETIME_ConvertUnixTimeToDate(pstContents->ulTimeStart, &stDate);

	ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stDate);
	pszDayOfWeek = (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex));
	HxSTD_snprintf((char*)stSubItem.szSubItemText, MAX_ELEMENT_TEXT_LENGTH, "%s %02d.%02d", pszDayOfWeek, stDate.ucMonth, stDate.ucDay);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_AddItemStartTime(NxUiScheduleAddContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulSubItemCnt = 0;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	HxDATETIME_Time_t	stTime;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! pstContents == NULL!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_scheduleAdd_GetItemSupport(eNxUiScheduleAddItem_StartTime);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUiScheduleAddItem_StartTime(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_StartTime);

	// Check bDisable Item
	bDisable = nx_scheduleAdd_GetItemiDisable(eNxUiScheduleAddItem_StartTime);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_scheduleAdd_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_SCHEDULE_ADD_ITEM_X;
	stSubItem.rect.y = NX_PRISM_SCHEDULE_ADD_ITEM_Y + (ulSubItemCnt * NX_PRISM_SCHEDULE_ADD_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_SCHEDULE_ADD_ITEM_W;
	stSubItem.rect.h = NX_PRISM_SCHEDULE_ADD_ITEM_LS;

	stSubItem.ulNumOfItemTextList = 0;
	stSubItem.ulIdxOfTextList = 0;
	stSubItem.lSubItemValue = (HINT32)pstContents->ulTimeStart;

	HxSTD_memset(&stTime, 0x00, sizeof(HxDATETIME_Time_t));
	HLIB_DATETIME_ConvertUnixTimeToTime(pstContents->ulTimeStart, &stTime);

	HxSTD_snprintf((char*)stSubItem.szSubItemText, MAX_ELEMENT_TEXT_LENGTH, "%02d:%02d", stTime.ucHour, stTime.ucMinute);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_AddItemDuration(NxUiScheduleAddContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulSubItemCnt = 0;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	HxDATETIME_Time_t	stTime;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! pstContents == NULL!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_scheduleAdd_GetItemSupport(eNxUiScheduleAddItem_Duration);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support nx_scheduleAdd_AddItemDuration(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Duration);

	// Check bDisable Item
	bDisable = nx_scheduleAdd_GetItemiDisable(eNxUiScheduleAddItem_Duration);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_scheduleAdd_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_SCHEDULE_ADD_ITEM_X;
	stSubItem.rect.y = NX_PRISM_SCHEDULE_ADD_ITEM_Y + (ulSubItemCnt * NX_PRISM_SCHEDULE_ADD_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_SCHEDULE_ADD_ITEM_W;
	stSubItem.rect.h = NX_PRISM_SCHEDULE_ADD_ITEM_LS;

	stSubItem.ulNumOfItemTextList = 0;
	stSubItem.ulIdxOfTextList = 0;
	stSubItem.lSubItemValue = (HINT32)pstContents->ulTimeDuration;

	HxSTD_memset(&stTime, 0x00, sizeof(HxDATETIME_Time_t));
	HLIB_DATETIME_ConvertUnixTimeToTimeWithOutOffset(pstContents->ulTimeDuration, &stTime);

	HxSTD_snprintf((char*)stSubItem.szSubItemText, MAX_ELEMENT_TEXT_LENGTH, "%d %s",
					((stTime.ucHour *NX_COMMON_TIME_MINS_PER_HOUR) + stTime.ucMinute), ONDK_GetString(RES_MINS_ID));

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_AddItemRepeat(NxUiScheduleAddContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulListIdx = 0, ulSubItemCnt = 0;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! pstContents == NULL!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_scheduleAdd_GetItemSupport(eNxUiScheduleAddItem_Repeat);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support nx_scheduleAdd_AddItemRepeat(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Repeat);

	// Check bDisable Item
	bDisable = nx_scheduleAdd_GetItemiDisable(eNxUiScheduleAddItem_Repeat);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_scheduleAdd_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_SCHEDULE_ADD_ITEM_X;
	stSubItem.rect.y = NX_PRISM_SCHEDULE_ADD_ITEM_Y + (ulSubItemCnt * NX_PRISM_SCHEDULE_ADD_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_SCHEDULE_ADD_ITEM_W;
	stSubItem.rect.h = NX_PRISM_SCHEDULE_ADD_ITEM_LS;

#if 0
	/* Count SubItems */
	nx_scheduleAdd_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!! NX_PRISM_MENU_Menuitem_CountSubitems() eResult:(0x%x), ulItems(%u)\n", eResult, ulItems);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() ulMenuItemId:(0x%x)\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->eRepeat, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Not found menu item index(%d), pstContents->eRepeat(0x%x)\n", ulListIdx, pstContents->eRepeat);
		ulListIdx = 0;
	}
#endif

	stSubItem.lSubItemValue = (HINT32)pstContents->eRepeat;
	stSubItem.ulIdxOfTextList = ulListIdx;
	stSubItem.ulNumOfItemTextList = 0;

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	nx_scheduleAdd_MakeRepeatString(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_AddItemMode(NxUiScheduleAddContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulItems = 0, ulListIdx = 0, ulMenuItemId = 0, ulSubItemCnt = 0;
	HUINT8				**ppListOfItem = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! pstContents == NULL!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_scheduleAdd_GetItemSupport(eNxUiScheduleAddItem_Mode);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support nx_scheduleAdd_AddItemMode(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_Mode);

	// Check bDisable Item
	bDisable = nx_scheduleAdd_GetItemiDisable(eNxUiScheduleAddItem_Mode);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_scheduleAdd_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_SCHEDULE_ADD_ITEM_X;
	stSubItem.rect.y = NX_PRISM_SCHEDULE_ADD_ITEM_Y + (ulSubItemCnt * NX_PRISM_SCHEDULE_ADD_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_SCHEDULE_ADD_ITEM_W;
	stSubItem.rect.h = NX_PRISM_SCHEDULE_ADD_ITEM_LS;

	/* Count SubItems */
	nx_scheduleAdd_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!! NX_PRISM_MENU_Menuitem_CountSubitems() eResult:(0x%x), ulItems(%u)\n", eResult, ulItems);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() ulMenuItemId:(0x%x)\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->eScheduleMode, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Not found menu item index(%d), pstContents->eScheduleMode(0x%x)\n", ulListIdx, pstContents->eScheduleMode);
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eScheduleMode;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_LoadItemsValue(NxUiScheduleAddContent_t *pstContents)
{
	HERROR			hError = ERR_OK;
	HINT32			lChlistGroup = (HINT32)eNxChListGroup_TV;
	HUINT32 		ulStartUnixtime = 0, ulDuration = 0;
	DxRsvType_e 	eRsvType = DxRSVTYPE_NOTHING;
	NX_Channel_t	*pstChannel = NULL, stChannel;
	DxService_t 	*stLastService = NULL, *stFindService = NULL;
	Nx_ScheduleMode_t		eScheduleMode = eNxSchedle_ModeWatching;


	pstContents->pstSchedule = NX_APP_Calloc(sizeof(DxSchedule_t));
	if (pstContents->pstSchedule == NULL)
	{
		NX_APP_Error("ERROR!!! NX_APP_Calloc() Fail !!\n");
		return	ONDK_RET_FAIL;
	}

	if (pstContents->bReSchedule == TRUE)
	{
		hError = NX_SCHEDULER_GetScheduleBySlotID(pstContents->ulSlot, pstContents->pstSchedule);
		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!!! NX_SCHEDULER_GetScheduleBySlotID() Fail !!\n");
			return	ONDK_RET_FAIL;
		}

		HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstContents->pstSchedule->stStartTime, &ulStartUnixtime);
		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!!! HLIB_DATETIME_ConvertDateTimeToUnixTime() Fail !!\n");
		}

		ulDuration = pstContents->pstSchedule->ulDuration;
		eRsvType = pstContents->pstSchedule->eRsvType;
	}
	else
	{
#if defined(CONFIG_MW_MM_PVR)
		eRsvType = DxRSVTYPE_RECORD_TBR;
#else
		eRsvType = DxRSVTYPE_WATCHING_TBR;
#endif

		// Time
		hError = NX_SYSTEM_GetTime(&ulStartUnixtime);
		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!!! NX_SYSTEM_GetTime() Fail !!\n");
			nx_scheduleAdd_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5625_ID));
			return	ONDK_RET_FAIL;
		}

		// Start Time - Delete seconds time
		ulStartUnixtime = (ulStartUnixtime / NX_COMMON_TIME_SECONDS_PER_MIN) * NX_COMMON_TIME_SECONDS_PER_MIN;
		/* Start Tiem - Default Add Time - 10 min */
		ulStartUnixtime += (NX_COMMON_TIME_SECONDS_PER_MIN * NX_PRISM_SCHEDULE_ADD_DEFAULT_START_TIME_MIN);

		// Duration
		ulDuration = (NX_COMMON_TIME_SECONDS_PER_MIN * NX_COMMON_TIME_MINS_PER_HOUR);

		// Current Channel
		pstChannel = NX_PRISM_LIVE_GetMasterChannel();
		if (pstChannel == NULL)
		{
			hError = NX_CHANNEL_GetLastServiceGroup(&lChlistGroup);
			if (hError != ERR_OK)
			{
				lChlistGroup = (HINT32)eNxChListGroup_TV;
				NX_CHANNEL_SetLastServiceGroup(lChlistGroup);
			}

			stLastService = NX_CHANNEL_GetLastService();
			if (stLastService == NULL)
			{
				stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_TV, eNxFindSvcDir_First, 0);
				if (stFindService == NULL)
				{
					stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_RADIO, eNxFindSvcDir_First, 0);
				}
			}
			else
			{
				stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_TV, eNxFindSvcDir_Current, stLastService->uid);
				if (stFindService == NULL)
				{
					stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_RADIO, eNxFindSvcDir_Current, stLastService->uid);
				}
			}

			if (stFindService)
			{
				hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(lChlistGroup, stFindService->svcid, &stChannel);
				if (hError != ERR_OK)
				{
					return	ONDK_RET_FAIL;
				}

				NX_SCHEDULER_CreateScheduleWithTime(eRsvType, stChannel.svcUid, stChannel.lcn, ulStartUnixtime, ulDuration, 0, pstContents->pstSchedule);
			}
		}
		else
		{
			NX_SCHEDULER_CreateScheduleWithTime(eRsvType, pstChannel->svcUid, pstChannel->lcn, ulStartUnixtime, ulDuration, 0, pstContents->pstSchedule);
		}
	}

	switch (eRsvType)
	{
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_TBR:
		case DxRSVTYPE_WATCHING_SERIES_EBR:
		case DxRSVTYPE_DLNA_TBR:
		case DxRSVTYPE_DLNA_EBR:
			eScheduleMode = eNxSchedle_ModeWatching;
			break;

		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
		case DxRSVTYPE_KEYWORD_RECORD_NOTSURE:
			eScheduleMode = eNxSchedle_ModeRecord;
			break;

		default:
			eScheduleMode = eNxSchedle_ModeWatching;
			break;
	}

	pstContents->ulTimeStart = ulStartUnixtime;
	pstContents->ulTimeDuration = ulDuration;
	pstContents->eRepeat = pstContents->pstSchedule->eRepeat;
	pstContents->ulRepeatDays = pstContents->pstSchedule->ulRepeatDays;
	pstContents->eScheduleMode = pstContents->eScheduleModePrev = eScheduleMode;

	pstContents->ulSvcUid = pstContents->pstSchedule->ulSvcUid;
	pstContents->eChlistGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
	pstContents->eLoadChlistGroup = pstContents->eChlistGroup;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_InitSettingsItems(NxUiScheduleAddContent_t *pstContents, HINT32 p1)
{
	HINT32			lSlot = NX_CORE_SCHEDULE_SLOTID_NULL;
	ONDK_Result_t	eResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error(" Error!!! pstContents == NULL !!\n");
		return ONDK_RET_INITFAIL;
	}

	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! NX_PRISM_AppContents_InitSubitemArray() Fail !!\n");
		return eResult;
	}

	lSlot = p1;
	if (lSlot == NX_CORE_SCHEDULE_SLOTID_NULL)
	{
		pstContents->ulSlot = (HUINT32)NX_CORE_SCHEDULE_SLOTID_NULL;
		pstContents->bReSchedule = FALSE;
	}
	else
	{
		pstContents->ulSlot = (HUINT32)lSlot;
		pstContents->bReSchedule = TRUE;
	}

	pstContents->bShowChList = FALSE;
	HxSTD_MemSet(&pstContents->stGroupList, 0x00, sizeof(NX_Util_OPTIONList_t));

	// Load Menu Item Value
	eResult = nx_scheduleAdd_LoadItemsValue(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! nx_scheduleAdd_LoadItemsValue() Fail !!\n");
		return eResult;
	}

	nx_scheduleAdd_AddItemChannel(pstContents);
	nx_scheduleAdd_AddItemDate(pstContents);
	nx_scheduleAdd_AddItemStartTime(pstContents);
	nx_scheduleAdd_AddItemDuration(pstContents);
	nx_scheduleAdd_AddItemRepeat(pstContents);
	nx_scheduleAdd_AddItemMode(pstContents);

	// Default Function
	//nx_scheduleAdd_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_SetUiObject(NxUiScheduleAddContent_t *pstContents)
{
	HINT8			*pszTitleId = NULL;
	HINT32			lObjId = -1, lFontSize = 0, lAlign = TEXT_ALIGN_CENTER, lColorIdx = 0;
	ONDK_Rect_t 	stRect;
	ONDK_FontId_t 	eFontId = eFont_SystemNormal;

	/* Draw BG Frame */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_ADD_FRAME_X, NX_PRISM_SCHEDULE_ADD_FRAME_Y,
						NX_PRISM_SCHEDULE_ADD_FRAME_W, NX_PRISM_SCHEDULE_ADD_FRAME_H);
	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_FRAME_ID);
	ONDK_GWM_COM_Rect_Create(lObjId, &stRect, COL(C_G_Bang_95));

	/* Draw Title TEXT */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_ADD_TTL_TEXT_X, NX_PRISM_SCHEDULE_ADD_TTL_TEXT_Y,
						NX_PRISM_SCHEDULE_ADD_TTL_TEXT_W, NX_PRISM_SCHEDULE_ADD_TTL_TEXT_H);

	if (pstContents->bReSchedule == TRUE)
	{
		pszTitleId = STR_MESG_3226_ID;
		eFontId = eFont_SystemNormal;
		lFontSize = NX_PRISM_FONT_SIZE_22;
		lAlign = TEXT_ALIGN_LEFT;
		lColorIdx = C_T_Yoyo01;
	}
	else
	{
		pszTitleId = RES_MESG_3225_ID;
		eFontId = eFont_SystemBold;
		lFontSize = NX_PRISM_FONT_SIZE_36;
		lAlign = TEXT_ALIGN_CENTER;
		lColorIdx = C_T_Gong01;
	}
	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_TTLTEXT_ID);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(pszTitleId));
	ONDK_GWM_COM_Text_SetFont(lObjId, eFontId);
	ONDK_GWM_COM_Text_SetSize(lObjId, lFontSize);
	ONDK_GWM_COM_Text_SetAlign(lObjId, lAlign);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(lColorIdx), COL(C_T_Bang_100));

	/* Draw Main Item */
	NX_PRISM_COMP_OPT_InitMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_OPT_DrawCursor_A1_Arrow);
	// Set Focus
	ONDK_GWM_Obj_SetFocus(pstContents->subItems[pstContents->ulCurFocus].ulId);

	/* Draw Button Item */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_ADD_BTN_OK_X, NX_PRISM_SCHEDULE_ADD_BTN_OK_Y,
						NX_PRISM_SCHEDULE_ADD_BTN_OK_W, NX_PRISM_SCHEDULE_ADD_BTN_OK_H);
	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_BTN_Ok);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_OK_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_A1);

	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_ADD_BTN_CANCEL_X, NX_PRISM_SCHEDULE_ADD_BTN_CANCEL_Y,
						NX_PRISM_SCHEDULE_ADD_BTN_CANCEL_W, NX_PRISM_SCHEDULE_ADD_BTN_CANCEL_H);
	lObjId = nx_scheduleAdd_ConvertItemIdxToObjId(eNxUiScheduleAddItem_BTN_Cancel);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_A1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_MsgGwmCreate(NxUiScheduleAddContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SCHEDULE_ADD_WINDOW_X, NX_PRISM_SCHEDULE_ADD_WINDOW_Y, NX_PRISM_SCHEDULE_ADD_WINDOW_W, NX_PRISM_SCHEDULE_ADD_WINDOW_H);

	// Init Menu Item
	eResult = nx_scheduleAdd_InitSettingsItems(pstContents, p1);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! nx_scheduleAdd_InitSettingsItems() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	// Draw Menu Item
	nx_scheduleAdd_SetUiObject(pstContents);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_MsgGwmDestroy(NxUiScheduleAddContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! pstContents == NULL!! \n");
		return ONDK_RET_FAIL;
	}

	nx_scheduleAdd_DestroyListPopup();
	nx_scheduleAdd_DestroyTimeDialog();
	nx_scheduleAdd_DestroyDialogInput();

	if (pstContents->pstSchedule)
	{
		NX_APP_Free(pstContents->pstSchedule);
		pstContents->pstSchedule = NULL;
	}

	if (pstContents->bShowChList == FALSE)
	{
		nx_scheduleAdd_InitChannlGroupItemTextList(pstContents);
	}
	else
	{
		nx_scheduleAdd_InitChannlListTextList(pstContents);
	}

	HxSTD_MemSet(&pstContents->stGroupList, 0x00, sizeof(NX_Util_OPTIONList_t));

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(NxUiScheduleAddContent_t));

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_MsgGwmHapiMsg(NxUiScheduleAddContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(handle);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_scheduleAdd_MsgGwmClickMsg(NxUiScheduleAddContent_t *pstContents, HINT32 lMessage, HINT32 p1)
{
	HUINT8			ucStartTime[NX_PRISM_UI_TIME_DIGIT_MAX];
	HINT32			lObjId = p1, lItemIdx = -1;
	HUINT32			ulInputValue = 0;
	HxDATETIME_Time_t	stStartTime;


	lItemIdx = nx_scheduleAdd_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxUiScheduleAddItem_Channel:
			nx_scheduleAdd_ChangeChannlGroupInitData(pstContents);
			nx_scheduleAdd_MakeChannlGroupItemTextList(pstContents);
			nx_scheduleAdd_CreateListPopup(pstContents, p1);
			break;
		case eNxUiScheduleAddItem_Date:
			nx_scheduleAdd_MakeDateTextList(pstContents);
			nx_scheduleAdd_CreateListPopup(pstContents, lObjId);
			break;
		case eNxUiScheduleAddItem_StartTime:
			{
				HxSTD_memset(&stStartTime, 0x00, sizeof(HxDATETIME_Time_t));
				HLIB_DATETIME_ConvertUnixTimeToTime(pstContents->ulTimeStart, &stStartTime);

				HxSTD_memset(ucStartTime, 0x00, NX_PRISM_UI_TIME_DIGIT_MAX);
				ucStartTime[0] = '\0';
				HxSTD_snprintf((char*)ucStartTime, NX_PRISM_UI_TIME_DIGIT_MAX, "%02d%02d", stStartTime.ucHour, stStartTime.ucMinute);
				ucStartTime[NX_PRISM_UI_TIME_DIGIT_MAX-1] = '\0';
				ulInputValue = (HUINT32)HLIB_STD_StrToINT32((HCHAR*)ucStartTime);
				nx_scheduleAdd_CreateTimeDialog(pstContents, ulInputValue, ONDK_GetString(RES_MESG_3227_ID));
			}
			break;
		case eNxUiScheduleAddItem_Duration:
			nx_scheduleAdd_CreateDialogInput(pstContents, lObjId);
			break;
		case eNxUiScheduleAddItem_Repeat:
			if ((pstContents->pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR) || (pstContents->pstSchedule->eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR))
			{
				// TODO: [JHLEE] Change String ID
				nx_scheduleAdd_CreateSelectDialog(pstContents, eNxUiScheduleAdd_DialogType_SwitchSeries, (HUINT8*)RES_SCHEDULES_ID);
			}
			else
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_SCHEDULE_Repeat_Proc", (APP_DEFAULT_PRIORITY | UISTATE_SCHEDULE),
									(ONDK_GWM_Callback_t)NX_PRISM_SCHEDULE_Repeat_Proc, 0, pstContents->eRepeat, pstContents->ulRepeatDays, pstContents->eScheduleMode);
			}
			break;
		case eNxUiScheduleAddItem_Mode:
			nx_scheduleAdd_CreateListPopup(pstContents, p1);
			break;
		case eNxUiScheduleAddItem_BTN_Ok:
			if (lMessage != MSG_GWM_SCROLLRIGHT)
			{
				nx_scheduleAdd_SaveChangeData(pstContents);
				ONDK_GWM_APP_Destroy(0);
			}
			break;
		case eNxUiScheduleAddItem_BTN_Cancel:
			if (lMessage != MSG_GWM_SCROLLRIGHT)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_MsgGwmKeyDown(NxUiScheduleAddContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! pstContents == NULL!! \n");
		return ONDK_RET_FAIL;
	}

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

		case KEY_OK:
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_MsgAppInputValueChange(NxUiScheduleAddContent_t *pstContents, HUINT32 ulInputValue)
{
	HERROR				hError = ERR_OK;
	HINT32				lObjId = 0, lItemIdx = -1;
	HUINT32				ulDuration = 0, ulCurrentUnixtime = 0, ulInputStartUnixtime = 0;
	HxDATETIME_t		stStartDate;
	HxDATETIME_Time_t	stTime;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	lItemIdx = nx_scheduleAdd_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxUiScheduleAddItem_StartTime:
			HLIB_DATETIME_ConvertUnixTimeToTime(ulInputValue, &stTime);

			HxSTD_memset(&stStartDate, 0x00, sizeof(HxDATETIME_t));
			HLIB_DATETIME_ConvertUnixTimeToDateTime(pstContents->ulTimeStart, &stStartDate);
			stStartDate.stTime.ucHour = stTime.ucHour;
			stStartDate.stTime.ucMinute = stTime.ucMinute;
			stStartDate.stTime.ucSecond = stTime.ucSecond;
			stStartDate.stTime.usMillisecond = stTime.usMillisecond;
			HLIB_DATETIME_ConvertDateTimeToUnixTime(&stStartDate, &ulInputStartUnixtime);

			hError = NX_SYSTEM_GetTime(&ulCurrentUnixtime);
			if (hError != ERR_OK)
			{
				NX_APP_Error("ERROR!!! NX_SYSTEM_GetTime() Fail !!\n");
				return	ONDK_RET_FAIL;
			}
			ulCurrentUnixtime = (ulCurrentUnixtime / NX_COMMON_TIME_SECONDS_PER_MIN) * NX_COMMON_TIME_SECONDS_PER_MIN;		/* Delete Millisecond. */
			ulCurrentUnixtime = ulCurrentUnixtime + (NX_COMMON_TIME_SECONDS_PER_MIN * 2);
			if (ulInputStartUnixtime < ulCurrentUnixtime)
			{
				nx_scheduleAdd_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_092_ID));
				return	ONDK_RET_FAIL;
			}

			HxSTD_memset(pstSubItem->szSubItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
			HxSTD_sprintf((char*)pstSubItem->szSubItemText, "%02d:%02d", stTime.ucHour, stTime.ucMinute);
			nx_scheduleAdd_UpdateStartTime(pstContents, lObjId, ulInputValue);
			break;

		case eNxUiScheduleAddItem_Duration:
			if (ulInputValue > NX_PRISM_SCHEDULE_ADD_DEFAULT_DURATION_MAX_VALUE)
			{
				nx_scheduleAdd_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_4556_ID));
			}
			else
			{
				ulDuration = (ulInputValue * NX_COMMON_TIME_MINS_PER_HOUR);
				nx_scheduleAdd_UpdateDuration(pstContents, lObjId, ulDuration);
			}
			break;

		default:
			NX_APP_Error("Error!! Not Supported itme lItemIdx:[%d]\n", lItemIdx);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleAdd_MsgAppDlgClicked(NxUiScheduleAddContent_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId)
{

	switch (lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:	// YES
			if (ulDlgWhoseId == eNxUiScheduleAdd_DialogType_SwitchSeries)
			{
				nx_scheduleAdd_DestroySelectDialog();
				if (pstContents->pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR)
				{
					pstContents->pstSchedule->eRsvType = DxRSVTYPE_RECORD_TBR;
				}
				else if (pstContents->pstSchedule->eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR)
				{
					pstContents->pstSchedule->eRsvType = DxRSVTYPE_WATCHING_TBR;
				}
				else
				{
					// Not to do
				}
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_SCHEDULE_Repeat_Proc", (APP_DEFAULT_PRIORITY | UISTATE_SCHEDULE),
								(ONDK_GWM_Callback_t)NX_PRISM_SCHEDULE_Repeat_Proc, 0, pstContents->eRepeat, pstContents->ulRepeatDays, pstContents->eScheduleMode);
			}
			else
			{
				NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", ulDlgWhoseId);
			}
			break;
		default:						// No
			if (ulDlgWhoseId == eNxUiScheduleAdd_DialogType_SwitchSeries)
			{
				nx_scheduleAdd_DestroySelectDialog();
			}
			else
			{
				NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", ulDlgWhoseId);
			}
			break;
	}

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_scheduleAdd_MsgAppDlgCanceled(NxUiScheduleAddContent_t *pstContents, HINT32 ulDlgWhoseId)
{
	if (ulDlgWhoseId == eNxUiScheduleAdd_DialogType_SwitchSeries)
	{
		nx_scheduleAdd_DestroySelectDialog();
	}
	else
	{
		NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", ulDlgWhoseId);
	}

	return	ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SCHEDULE_Add_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	NxUiScheduleAddContent_t	*pstContents = NULL;

	pstContents = nx_scheduleAdd_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_scheduleAdd_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_scheduleAdd_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			eResult = nx_scheduleAdd_MsgGwmClickMsg(pstContents, lMessage, p1);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			nx_scheduleAdd_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			nx_scheduleAdd_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_scheduleAdd_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_scheduleAdd_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_POPUP_LIST_DESTROYED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED!\n");
			nx_scheduleAdd_DestroyListPopup();
			if (pstContents->bShowChList == FALSE)
			{
				nx_scheduleAdd_InitChannlGroupItemTextList(pstContents);
			}
			else
			{
				nx_scheduleAdd_InitChannlListTextList(pstContents);
			}
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			eResult = nx_scheduleAdd_ChangedSubItemValue(pstContents, p1, p2);
			break;

		case MSG_APP_SETTING_INPUT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_INPUT_VALUE_CHANGED!\n");
			eResult = nx_scheduleAdd_MsgAppInputValueChange(pstContents, p1);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED! || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_scheduleAdd_MsgAppDlgClicked(pstContents, p1, p2);
			break;

		case MSG_APP_DLG_CANCELED:
			NX_APP_Message("MSG_APP_DLG_CLICKED!\n");
			eResult = nx_scheduleAdd_MsgAppDlgCanceled(pstContents, p2);
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
