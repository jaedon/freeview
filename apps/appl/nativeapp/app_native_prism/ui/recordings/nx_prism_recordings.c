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
 * @file	  		nx_prism_recordings.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#if defined(CONFIG_MW_MM_PVR)
/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_menu_contents.h>

#include "nx_prism_recordings.h"

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define NX_RECORDINGS_TIMEOUT_ID					0xf000
#define NX_RECORDINGS_TIMEOUT						(3*60*NX_COMMON_TIME_SECOND)	/* 3 minutes */

#define NX_RECORDINGS_LOAD_THUMBNAIL_TIMER_ID		0xac11
#define NX_RECORDINGS_LOAD_THUMBNAIL_TIMEOUT		5000

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
#define	NX_RECORDINGS_LIST_ANI_ID					0xac13
#define NX_RECORDINGS_LIST_ANI_TIMEOUT				10
#endif

#define	NX_PRISM_RECORDINGS_ITEM_ID					eMenuFrame_Window_Recordings

#define	NX_PRISM_RECORDINGS_WINDOW_X				0
#define	NX_PRISM_RECORDINGS_WINDOW_Y				0
#define	NX_PRISM_RECORDINGS_WINDOW_W				NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_RECORDINGS_WINDOW_H				NX_PRISM_OSD_VERTICAL

#define	NX_PRISM_RECORDINGS_FRAME_ID				(NX_PRISM_RECORDINGS_ITEM_ID + 1)
#define	NX_PRISM_RECORDINGS_FRAME_X					0
#define	NX_PRISM_RECORDINGS_FRAME_Y					0
#define	NX_PRISM_RECORDINGS_FRAME_W					NX_PRISM_RECORDINGS_WINDOW_W
#define	NX_PRISM_RECORDINGS_FRAME_H					NX_PRISM_RECORDINGS_WINDOW_H

#define	NX_PRISM_RECORDINGS_TTL_TEXT_ID				(NX_PRISM_RECORDINGS_ITEM_ID + 2)
#define	NX_PRISM_RECORDINGS_TTL_TEXT_X				64
#define	NX_PRISM_RECORDINGS_TTL_TEXT_Y				(40 - NX_PRISM_FONT_VSPACE)
#define	NX_PRISM_RECORDINGS_TTL_TEXT_W				1045
#define	NX_PRISM_RECORDINGS_TTL_TEXT_H				(NX_PRISM_FONT_SIZE_46 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_RECORDINGS_LINE_TOP_ID				(NX_PRISM_RECORDINGS_ITEM_ID + 4)
#define	NX_PRISM_RECORDINGS_LINE_TOP_X				60
#define	NX_PRISM_RECORDINGS_LINE_TOP_Y				87
#define	NX_PRISM_RECORDINGS_LINE_TOP_W				1160
#define	NX_PRISM_RECORDINGS_LINE_TOP_H				1

#define	NX_PRISM_RECORDINGS_OPT_ID					(NX_PRISM_RECORDINGS_ITEM_ID + 5)
#define	NX_PRISM_RECORDINGS_OPT_X					820
#define	NX_PRISM_RECORDINGS_OPT_Y					674
#define	NX_PRISM_RECORDINGS_OPT_W					400
#define	NX_PRISM_RECORDINGS_OPT_H					30

// No Recordinds
#define	NX_PRISM_RECORDINGS_NO_IMG_ID				(NX_PRISM_RECORDINGS_ITEM_ID + 6)
#define	NX_PRISM_RECORDINGS_NO_IMG_X				733
#define	NX_PRISM_RECORDINGS_NO_IMG_Y				209
#define	NX_PRISM_RECORDINGS_NO_IMG_W				136
#define	NX_PRISM_RECORDINGS_NO_IMG_H				123

#define	NX_PRISM_RECORDINGS_NO_TEXT_ID				(NX_PRISM_RECORDINGS_ITEM_ID + 7)
#define	NX_PRISM_RECORDINGS_NO_TEXT_X				445
#define	NX_PRISM_RECORDINGS_NO_TEXT_Y				(361 - NX_PRISM_FONT_SIZE_26)
#define	NX_PRISM_RECORDINGS_NO_TEXT_W				712
#define	NX_PRISM_RECORDINGS_NO_TEXT_H				(NX_PRISM_FONT_SIZE_26 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_RECORDINGS_NO_TEXT_MSG_ID			(NX_PRISM_RECORDINGS_ITEM_ID + 8)
#define	NX_PRISM_RECORDINGS_NO_TEXT_MSG_X			445
#define	NX_PRISM_RECORDINGS_NO_TEXT_MSG_Y			(483 - NX_PRISM_FONT_SIZE_24)
#define	NX_PRISM_RECORDINGS_NO_TEXT_MSG_W			712
#define	NX_PRISM_RECORDINGS_NO_TEXT_MSG_LS			30

#define	NX_PRISM_RECORDINGS_TIME_UPDATE_ID			(NX_PRISM_RECORDINGS_ITEM_ID + 9)

#define	NX_PRISM_RECORDINGS_BTN_MG_8				8
#define	NX_PRISM_RECORDINGS_BTN_MG_18				18

#define	NX_PRISM_RECORDINGS_FRAME_CONTENT_ID		(NX_PRISM_RECORDINGS_ITEM_ID + 16)
#define	NX_PRISM_RECORDINGS_FRAME_CONTENT_X			396
#define	NX_PRISM_RECORDINGS_FRAME_CONTENT_Y			103
#define	NX_PRISM_RECORDINGS_FRAME_CONTENT_W			810
#define	NX_PRISM_RECORDINGS_FRAME_CONTENT_H			617

#define NX_PRISM_RECORDINGS_CONTENTS_GRID_ID		(NX_PRISM_RECORDINGS_ITEM_ID + 17)
#define NX_PRISM_RECORDINGS_CONTENTS_GRID_X			NX_PRISM_RECORDINGS_FRAME_CONTENT_X + 5
#define NX_PRISM_RECORDINGS_CONTENTS_GRID_Y			NX_PRISM_RECORDINGS_FRAME_CONTENT_Y
#define NX_PRISM_RECORDINGS_CONTENTS_GRID_W			NX_PRISM_RECORDINGS_FRAME_CONTENT_W - 10	//(left margin + right margin)
#define NX_PRISM_RECORDINGS_CONTENTS_GRID_H			NX_PRISM_RECORDINGS_FRAME_CONTENT_H - 11
#define NX_PRISM_RECORDINGS_CONTENTS_GRID_COL		4
#define NX_PRISM_RECORDINGS_CONTENTS_GRID_ROW		3
#define NX_PRISM_RECORDINGS_CONTENTS_ITEM_W			(170 + 10 + 10) // 190
#define NX_PRISM_RECORDINGS_CONTENTS_ITEM_H			(164 + 11 + 27)	// 202

#define NX_PRISM_RECORDINGS_CONTENTS_GROUP_NUM_ID	0xb210		// 그룹 개수 표시

#define NX_PRISM_RECORDINGS_MENU_LIST_ID			(NX_PRISM_RECORDINGS_ITEM_ID + 18)
#define NX_PRISM_RECORDINGS_MENU_LIST_X				60
#define NX_PRISM_RECORDINGS_MENU_LIST_Y				103
#define NX_PRISM_RECORDINGS_MENU_LIST_W				310
#define NX_PRISM_RECORDINGS_MENU_LIST_H				398
#define NX_PRISM_RECORDINGS_MENU_LIST_ITEM_W		310
#define NX_PRISM_RECORDINGS_MENU_LIST_ITEM_H		58 + NX_PRISM_RECORDINGS_BTN_MG_8
#define NX_PRISM_RECORDINGS_MENU_LIST_ITEM_COUNT	6

#define	NX_PRISM_RECORDINGS_HDD_TEXT_ID				(NX_PRISM_RECORDINGS_ITEM_ID + 19)
#define	NX_PRISM_RECORDINGS_HDD_TEXT_X				70
#define	NX_PRISM_RECORDINGS_HDD_TEXT_Y				(588 - NX_PRISM_FONT_VSPACE)
#define	NX_PRISM_RECORDINGS_HDD_TEXT_W				290
#define	NX_PRISM_RECORDINGS_HDD_TEXT_H				(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_RECORDINGS_INFO_GRAPH_ID			(NX_PRISM_RECORDINGS_ITEM_ID + 20)
#define	NX_PRISM_RECORDINGS_INFO_GRAPH_X			70
#define	NX_PRISM_RECORDINGS_INFO_GRAPH_Y			610
#define	NX_PRISM_RECORDINGS_INFO_GRAPH_W			290
#define	NX_PRISM_RECORDINGS_INFO_GRAPH_H			21
#define	NX_PRISM_RECORDINGS_INFO_GRAPH_LINE_H		1

#define	NX_PRISM_RECORDINGS_STATUS_TEXT_ID			(NX_PRISM_RECORDINGS_ITEM_ID + 21)
#define	NX_PRISM_RECORDINGS_STATUS_TEXT_X			70
#define	NX_PRISM_RECORDINGS_STATUS_TEXT_Y			(645 - NX_PRISM_FONT_VSPACE)
#define	NX_PRISM_RECORDINGS_STATUS_TEXT_W			290
#define	NX_PRISM_RECORDINGS_STATUS_TEXT_H			(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct tagRecordingsContext {
	HINT32		ulDummy;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];

	Nx_RecordGroupType_e	eCurrentGroupType;

	// recording data
	HxVector_t	*recList;
	HINT32		nRecListNum;

	HxVector_t	*channelRecList;

	// HDD Status
	HCHAR		szHDDStatusText[NX_TEXT_SIZE_64];
} Nx_RecordingsContext_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_RecordingsContext_t		s_stRecordingsContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_RecordingsContext_t	*nx_recordings_GetContents(void);

static void				nx_recordings_UpdateCurrentTime(Nx_RecordingsContext_t *pstContents);

static ONDK_Result_t 	nx_recordings_DestroyUiNoScheduleObject(Nx_RecordingsContext_t *pstContents);
static ONDK_Result_t 	nx_recordings_SetUiNoScheduleObject(Nx_RecordingsContext_t *pstContents);
static ONDK_Result_t 	nx_recordings_SetUiMenuItemObject(Nx_RecordingsContext_t *pstContents);
static ONDK_Result_t	nx_recordings_SetUiContentsItemObject(Nx_RecordingsContext_t *pstContents);
static ONDK_Result_t	nx_recordings_DrawContentsItemFunc(HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex);
static ONDK_Result_t	nx_recordings_DrawListFunc(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_recordings_DrawStorageInfo(Nx_RecordingsContext_t *pstContents);
static ONDK_Result_t	nx_recordings_DrawPercentBar(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_recordings_SetFocusToButton(void);
static ONDK_Result_t	nx_recordings_CalcUsedAvailPercent(NX_PhysicalStorageData_t *physicalStorageData, HUINT32 *pstUsedPercent, HUINT32 *pstAvailPercent);

static ONDK_Result_t	nx_recordings_SetUiObject(Nx_RecordingsContext_t *pstContents);
static ONDK_Result_t	nx_recordings_SelectedItemList(Nx_RecordingsContext_t *pstContents);

static ONDK_Result_t	nx_recordings_MsgGwmCreate(Nx_RecordingsContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_recordings_MsgGwmDestroy(Nx_RecordingsContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_recordings_MsgGwmHapiMsg(Nx_RecordingsContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_recordings_MsgGwmKeyDown(Nx_RecordingsContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_recordings_MenuListSelect(Nx_RecordingsContext_t *pstContents, HINT32 lIndex);
static ONDK_Result_t	nx_recordings_MsgGwmListSelected(Nx_RecordingsContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_recordings_MsgGwmTimer(Nx_RecordingsContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static HCHAR*			nx_recordings_GetMenuString(HINT32 index);

//
#define	________________DATA_______________
static void		nx_recordings_RecItemDelete (void *item);
static ONDK_Result_t	nx_recordings_ClearRecList(Nx_RecordingsContext_t *pstContents);
static ONDK_Result_t	nx_recordings_MakeRecItemList(Nx_RecordingsContext_t *pstContents);
static ONDK_Result_t	nx_recordings_DrawRecItemFunc(Nx_RecordingsContext_t*pstContents, HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex);
static ONDK_Result_t	nx_recordings_ClearChannelGroupRecList(Nx_RecordingsContext_t *pstContents);
static HBOOL 			nx_recordings_FindChannelGroupData(void *data, void *userdata);
static HINT32 			nx_recordings_ChannelRecListSortByStartTime (const void *pvArg1, const void *pvArg2);
static ONDK_Result_t	nx_recordings_MakeChannelGroupRecList(Nx_RecordingsContext_t *pstContents);
static ONDK_Result_t	nx_recordings_DrawChannelGroupItemFunc(Nx_RecordingsContext_t*pstContents, HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex);

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
static ONDK_Result_t	nx_recordings_SetAnimationCtl(void);
static int				nx_recordings_SetAnimationUpdate(void);
#endif

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_RecordingsContext_t	*nx_recordings_GetContents(void)
{
	return	&s_stRecordingsContent;
}

static HCHAR*	nx_recordings_GetMenuString(HINT32 index)
{
	switch (index)
	{
		case 0:
			return (HCHAR*)ONDK_GetString(RES_PROGRAMME_01_ID);
			break;
		case 1:
			return (HCHAR*)ONDK_GetString(RES_CHANNEL_03_ID);
			break;
		case 2:
			return (HCHAR*)ONDK_GetString(RES_DAY_OF_WEEK_01_ID);
			break;
		case 3:
			return (HCHAR*)ONDK_GetString(RES_STORAGE_01_ID);
			break;
		case 4:
			return (HCHAR*)ONDK_GetString(RES_SCHEDULES_ID);
			break;
		case 5:
			return (HCHAR*)ONDK_GetString(RES_TO_BE_DELETED_ID);
			break;
		default:
			return NULL;
			break;
	}

}


static void		nx_recordings_UpdateCurrentTime(Nx_RecordingsContext_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HCHAR 			*pszDayOfWeek = NULL;
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


static ONDK_Result_t 	nx_recordings_SetUiMenuItemObject(Nx_RecordingsContext_t *pstContents)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_MENU_LIST_X, NX_PRISM_RECORDINGS_MENU_LIST_Y, NX_PRISM_RECORDINGS_MENU_LIST_W, NX_PRISM_RECORDINGS_MENU_LIST_H);
	ONDK_GWM_List_Create(NX_PRISM_RECORDINGS_MENU_LIST_ID, &stRect, "NxRecordingsList", NX_PRISM_RECORDINGS_MENU_LIST_ITEM_H,
							NX_PRISM_RECORDINGS_MENU_LIST_ITEM_COUNT, 0, 0);
	ONDK_GWM_List_SetFontHandle(NX_PRISM_RECORDINGS_MENU_LIST_ID, eFont_SystemNormal, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_List_SetCurrentIndex(NX_PRISM_RECORDINGS_MENU_LIST_ID, 0);
	ONDK_GWM_List_SetSelectedIndex(NX_PRISM_RECORDINGS_MENU_LIST_ID, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_RECORDINGS_MENU_LIST_ID, (GWM_ObjDraw_t)nx_recordings_DrawListFunc);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}

static ONDK_Result_t nx_recordings_SetUiContentsItemObject(Nx_RecordingsContext_t * pstContents)
{
	ONDK_Rect_t			stRect;
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_CONTENTS_GRID_X, NX_PRISM_RECORDINGS_CONTENTS_GRID_Y,
		NX_PRISM_RECORDINGS_CONTENTS_GRID_W, NX_PRISM_RECORDINGS_CONTENTS_ITEM_H*NX_PRISM_RECORDINGS_CONTENTS_GRID_ROW);
	ONDK_GWM_COM_GridList_Create(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID, &stRect, "Recording_contents_grid",
						NX_PRISM_RECORDINGS_CONTENTS_ITEM_W,
						NX_PRISM_RECORDINGS_CONTENTS_ITEM_H, pstContents->nRecListNum,
						NX_PRISM_RECORDINGS_CONTENTS_GRID_COL,
						NX_PRISM_RECORDINGS_CONTENTS_GRID_ROW, 0);
	ONDK_GWM_COM_GridList_SetCurrentIndex(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID, 0);
#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	ONDK_GWM_COM_GridList_SetAnimationCtl(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID, nx_recordings_SetAnimationCtl);
#endif
	ONDK_GWM_COM_GridList_SetItemDrawMethod(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID, (GRID_ItemDraw_t)nx_recordings_DrawContentsItemFunc);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_DrawContentsItemFunc(HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex)
{
	Nx_RecordingsContext_t	*pstContents = nx_recordings_GetContents();
	ONDK_Result_t	ret = ONDK_RET_FAIL;

	switch(pstContents->eCurrentGroupType)
	{
	case eNxRecGroup_PROGRAMME:
		ret = nx_recordings_DrawRecItemFunc(pstContents, id, screen, drawItem, nColIndex);
		break;

	case eNxRecGroup_CHANNEL:
		ret = nx_recordings_DrawChannelGroupItemFunc(pstContents, id, screen, drawItem, nColIndex);
		break;

	case eNxRecGroup_DOW:
		break;


	default:
		break;
	}
	return ret;
}

static ONDK_Result_t	nx_recordings_DrawListFunc(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HUINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HUINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 8;
	HUINT32		i, lCellHeight = 0, lNumOfPage = 0;
	HUINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0;
	HUINT32		lText_x_offset = 17, lText_y_offset = 17;

	HUINT32		ulStatus = 0, ulFontSize = 0, lItem = 0;
	HUINT8		*title = NULL;
	HBOOL		bFocus = FALSE;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;

	LIST_OBJECT 		obj = (LIST_OBJECT)object;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if (lTotal == 0)
	{
		NX_APP_Error("ERROR!!! (lTotal == 0) !!\n");
		return	ONDK_RET_FAIL;
	}

	ulFontSize = NX_PRISM_FONT_SIZE_22;
	lCursorMg = NX_PRISM_RECORDINGS_BTN_MG_8;

	ulStatus = ONDK_GWM_List_GetStatus(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));

	/* active, front most */
	for (i = 0; i < lTotal; i++)
	{
		lItem = i;//(HUINT32)(lStart + i);

		title = nx_recordings_GetMenuString(i);

		x = x1;
		y = y1 + lCursor_y_off;

		if (lItem == lCurrent)
		{
			if(ulStatus == GUIOBJ_HILIGHTED)
			{
				/* get focused */
				bFocus = TRUE;
				title_fontColor = COL (C_T_Bang_100);
				value_fontColor = COL (C_T_Bang_100);
				cursorColor = COL(C_G_Base01_100);
				eFontId = eFont_SystemBold;
				ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - lCursorMg, cursorColor);
			}
			else
			{
				bFocus = TRUE;
				title_fontColor = COL (C_T_Whistle_100);
				value_fontColor = COL (C_T_Whistle_100);
				cursorColor = COL(C_G_Base01_20);
				eFontId = eFont_SystemBold;
				ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - lCursorMg, cursorColor);
			}

		}
		else
		{
			/* Normal Item cursor */
			bFocus = FALSE;
			title_fontColor = COL (C_T_Gong02);
			value_fontColor = COL (C_T_Gong02);
			cursorColor = COL(C_G_Whistle_10);
			eFontId = eFont_SystemNormal;
			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - lCursorMg, cursorColor);
		}

		hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
		if (hFontHandle)
		{
			// SubItem - Title
			x = x1 + lText_x_offset;
			y = y1 + lCursor_y_off + lText_y_offset;
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, NX_PRISM_RECORDINGS_MENU_LIST_ITEM_W, title_fontColor, ONDKSTF_LEFT);
			ONDK_FONT_Release(hFontHandle);
		}

		if(i == 3)
			lCursor_y_off += (NX_PRISM_RECORDINGS_BTN_MG_18 - NX_PRISM_RECORDINGS_BTN_MG_8);
		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_DrawStorageInfo(Nx_RecordingsContext_t *pstContents)
{

	ONDK_Rect_t					stRect = { 0, 0, 0, 0 };
	HBOOL						bPaired = FALSE;
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;
	HUINT32 					ulHDFreeHrs = 0, ulSDFreeHrs = 0;
	HUINT32						ulUsedPercent = 0, ulAvailPercent = 0;

	/* Draw HDD Status Title Text */
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_HDD_TEXT_X, NX_PRISM_RECORDINGS_HDD_TEXT_Y,
						NX_PRISM_RECORDINGS_HDD_TEXT_W, NX_PRISM_RECORDINGS_HDD_TEXT_H);

	ONDK_GWM_COM_Text_Create(NX_PRISM_RECORDINGS_HDD_TEXT_ID, &stRect, (HCHAR*)ONDK_GetString(RES_HDD_STATUS_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_RECORDINGS_HDD_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_RECORDINGS_HDD_TEXT_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_RECORDINGS_HDD_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_RECORDINGS_HDD_TEXT_ID, COL(C_T_Gong01), COL(C_T_Bang_100));

	NX_FILEMGR_STORAGE_Check_HDD_Paired(&bPaired, &physicalStorageData);
	if (bPaired == TRUE && physicalStorageData != NULL)
	{
		ulHDFreeHrs = physicalStorageData->ullAvailablePvrSize / (1024 * 1024) / 4;
		ulSDFreeHrs = physicalStorageData->ullAvailablePvrSize / (1024 * 1024) / 1.5;

		nx_recordings_CalcUsedAvailPercent(physicalStorageData, &ulUsedPercent, &ulAvailPercent);
	}

	/* Draw Percent Graph */
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_INFO_GRAPH_X, NX_PRISM_RECORDINGS_INFO_GRAPH_Y,
							NX_PRISM_RECORDINGS_INFO_GRAPH_W, NX_PRISM_RECORDINGS_INFO_GRAPH_H + (NX_PRISM_RECORDINGS_INFO_GRAPH_LINE_H * 2));

	ONDK_GWM_COM_Status_Create(NX_PRISM_RECORDINGS_INFO_GRAPH_ID, &stRect, 100, 0);
	ONDK_GWM_COM_Status_SetColor(NX_PRISM_RECORDINGS_INFO_GRAPH_ID, COL(C_G_Bang_100), COL(C_G_Yoyo01));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_RECORDINGS_INFO_GRAPH_ID, nx_recordings_DrawPercentBar);
	ONDK_GWM_COM_Status_SetValue(NX_PRISM_RECORDINGS_INFO_GRAPH_ID, ulUsedPercent);

	/* Draw Remain hrs Text */
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_STATUS_TEXT_X, NX_PRISM_RECORDINGS_STATUS_TEXT_Y,
						NX_PRISM_RECORDINGS_STATUS_TEXT_W, NX_PRISM_RECORDINGS_STATUS_TEXT_H);

	HxSTD_sprintf(pstContents->szHDDStatusText, "%s %d%s / %s %d%s", (HCHAR*)ONDK_GetString(RES_HD_ID), ulHDFreeHrs, ulHDFreeHrs <= 1 ? (HCHAR*)ONDK_GetString(RES_HR_ID) : (HCHAR*)ONDK_GetString(RES_HRS_ID),
					(HCHAR*)ONDK_GetString(RES_SD_ID), ulSDFreeHrs, ulSDFreeHrs <= 1 ? (HCHAR*)ONDK_GetString(RES_HR_ID) : (HCHAR*)ONDK_GetString(RES_HRS_ID));

	ONDK_GWM_COM_Text_Create(NX_PRISM_RECORDINGS_STATUS_TEXT_ID, &stRect, pstContents->szHDDStatusText);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_RECORDINGS_STATUS_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_RECORDINGS_STATUS_TEXT_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_RECORDINGS_STATUS_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_RECORDINGS_STATUS_TEXT_ID, COL(C_T_Gong04), COL(C_T_Bang_100));

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_DrawPercentBar(ONDKSurfaceHandle screen, ONDK_Region_t *area,
															ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;
	HUINT32 	ulPrcentBar = 0, ulValue = 0, ulMax = 0;
	ONDK_Color_t	ulBackColor = 0, ulForcColor = 0;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	ONDK_GWM_COM_Status_GetProperty((STATUS_OBJECT)object, &ulMax, &ulValue, &ulBackColor, &ulForcColor);
	/* Draw Two Line */
	ONDK_DRAW_FillRectElement(screen, x1, y1, w, NX_PRISM_RECORDINGS_INFO_GRAPH_LINE_H, C_G_Whistle_100);
	ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_PRISM_RECORDINGS_INFO_GRAPH_LINE_H + NX_PRISM_RECORDINGS_INFO_GRAPH_H, w, NX_PRISM_RECORDINGS_INFO_GRAPH_LINE_H, C_G_Whistle_100);

	/* Draw BackGround */
	ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_PRISM_RECORDINGS_INFO_GRAPH_LINE_H, w, h - (NX_PRISM_RECORDINGS_INFO_GRAPH_LINE_H * 2), ulBackColor);

	/* calc bar length */
	ulPrcentBar = (ulValue * w)/ulMax;
	if (ulPrcentBar > (HUINT32)w)
	{
		ulPrcentBar = w;
	}

	if (ulPrcentBar > 0)
	{
		ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_PRISM_RECORDINGS_INFO_GRAPH_LINE_H, ulPrcentBar, h - (NX_PRISM_RECORDINGS_INFO_GRAPH_LINE_H * 2), ulForcColor);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_recordings_DestroyUiNoScheduleObject(Nx_RecordingsContext_t *pstContents)
{
	ONDK_GWM_Obj_Destroy(NX_PRISM_RECORDINGS_NO_IMG_ID);
	ONDK_GWM_Obj_Destroy(NX_PRISM_RECORDINGS_NO_TEXT_ID);
	ONDK_GWM_Obj_Destroy(NX_PRISM_RECORDINGS_NO_TEXT_MSG_ID);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}

static ONDK_Result_t 	nx_recordings_SetUiNoScheduleObject(Nx_RecordingsContext_t *pstContents)
{
	HINT32				lLineCnt = 0, h = 0;
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	/* Draw No schedule image */
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_NO_IMG_X, NX_PRISM_RECORDINGS_NO_IMG_Y,
							NX_PRISM_RECORDINGS_NO_IMG_W, NX_PRISM_RECORDINGS_NO_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_RECORDINGS_NO_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_RECORDINGS_NO_IMG_ID, RES_390_04_NOREC_PNG);

	/* Draw No schedule text */
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_NO_TEXT_X, NX_PRISM_RECORDINGS_NO_TEXT_Y,
						NX_PRISM_RECORDINGS_NO_TEXT_W, NX_PRISM_RECORDINGS_NO_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_RECORDINGS_NO_TEXT_ID, &stRect, (HCHAR*)ONDK_GetString(RES_NO_RECORDINGS_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_RECORDINGS_NO_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_RECORDINGS_NO_TEXT_ID, NX_PRISM_FONT_SIZE_26);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_RECORDINGS_NO_TEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_RECORDINGS_NO_TEXT_ID, COL(C_T_Gong03), COL(C_T_Bang_100));

	/* Draw No schedule msg */
	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_24,
										(HUINT8*)ONDK_GetString(RES_MESG_5978_ID), NX_PRISM_RECORDINGS_NO_TEXT_W);
	h = (NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_LS * lLineCnt) + NX_PRISM_FONT_VSPACE;
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_NO_TEXT_MSG_X, NX_PRISM_RECORDINGS_NO_TEXT_MSG_Y,
						NX_PRISM_RECORDINGS_NO_TEXT_MSG_W, h);
	ONDK_GWM_COM_Text_Create(NX_PRISM_RECORDINGS_NO_TEXT_MSG_ID, &stRect, (HCHAR*)ONDK_GetString(RES_MESG_3222_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_RECORDINGS_NO_TEXT_MSG_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_RECORDINGS_NO_TEXT_MSG_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_RECORDINGS_NO_TEXT_MSG_ID, (TEXT_ALIGN_CENTER | TEXT_MULTILINED));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_RECORDINGS_NO_TEXT_MSG_ID, COL(C_T_Gong05), COL(C_T_Bang_100));

	ONDK_GWM_Obj_SetHide(NX_PRISM_RECORDINGS_OPT_ID, TRUE);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_ArrowLeft(void)
{
	if(ONDK_GWM_COM_GridList_IsFirstColumn(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID) == FALSE)
	{
		//ONDK_GWM_Obj_SetFocus(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
	}
	else
	{
		ONDK_GWM_Obj_DisableObject(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
		ONDK_GWM_Obj_EnableObject(NX_PRISM_RECORDINGS_MENU_LIST_ID);
		ONDK_GWM_Obj_SetFocus(NX_PRISM_RECORDINGS_MENU_LIST_ID);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_ArrowRight(void)
{
	HINT32 objStatus;
	objStatus = ONDK_GWM_Obj_GetObjectStatus(ONDK_GWM_Obj_GetObjectByID(NX_PRISM_RECORDINGS_MENU_LIST_ID));
	if(objStatus == GUIOBJ_HILIGHTED)
	{
		ONDK_GWM_Obj_DisableObject(NX_PRISM_RECORDINGS_MENU_LIST_ID);
		ONDK_GWM_Obj_EnableObject(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
		ONDK_GWM_Obj_SetFocus(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
		return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_SetFocusToButton(void)
{
	ONDK_GWM_GuiObject_t obj;
	obj = ONDK_GWM_Obj_GetObjectByID(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
	if(ONDK_GWM_Obj_GetObjectStatus(obj)==GUIOBJ_NORMAL)
		ONDK_GWM_Obj_SetFocus(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_CalcUsedAvailPercent(NX_PhysicalStorageData_t *physicalStorageData, HUINT32 *pstUsedPercent, HUINT32 *pstAvailPercent)
{
	HUINT32 		ulTotalSize = physicalStorageData->ullTotalSize / (1024 * 1024);
	HUINT32 		ulUsedSize = (physicalStorageData->ullReservedSize + physicalStorageData->ullUsedSize) / (1024 * 1024);

	if (ulTotalSize > 0)
	{
		*pstUsedPercent = (HUINT32)((ulUsedSize * 100) / ulTotalSize);
		if (*pstUsedPercent > 100)
		{
			*pstUsedPercent = 100;
		}
		*pstAvailPercent = 100 - *pstUsedPercent;
	}
	else
	{
		*pstUsedPercent = 0;
		*pstAvailPercent = 100;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_SetUiObject(Nx_RecordingsContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_FRAME_X, NX_PRISM_RECORDINGS_FRAME_Y,
						NX_PRISM_RECORDINGS_FRAME_W, NX_PRISM_RECORDINGS_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_RECORDINGS_FRAME_ID, &stRect, COL(C_G_Bang_80));

	/* Draw Title TEXT */
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_TTL_TEXT_X, NX_PRISM_RECORDINGS_TTL_TEXT_Y,
						NX_PRISM_RECORDINGS_TTL_TEXT_W, NX_PRISM_RECORDINGS_TTL_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_RECORDINGS_TTL_TEXT_ID, &stRect, (HCHAR*)ONDK_GetString(RES_RECORDINGS_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_RECORDINGS_TTL_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_RECORDINGS_TTL_TEXT_ID, NX_PRISM_FONT_SIZE_46);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_RECORDINGS_TTL_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_RECORDINGS_TTL_TEXT_ID, COL(C_T_Gong01), COL(C_T_Bang_100));

	/* Draw TIME */
	nx_recordings_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	/* Draw TOP Line */
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_LINE_TOP_X, NX_PRISM_RECORDINGS_LINE_TOP_Y,
						NX_PRISM_RECORDINGS_LINE_TOP_W, NX_PRISM_RECORDINGS_LINE_TOP_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_RECORDINGS_LINE_TOP_ID, &stRect, COL(C_G_Whistle_30));

	/* Draw OPT Button */
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_OPT_X, NX_PRISM_RECORDINGS_OPT_Y,
						NX_PRISM_RECORDINGS_OPT_W, NX_PRISM_RECORDINGS_OPT_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_RECORDINGS_OPT_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_RECORDINGS_OPT_ID, NX_PRISM_COMP_DrawEdit_HelpR);

	/* Draw Left Menu Item */
	nx_recordings_SetUiMenuItemObject(pstContents);

	/* Draw Content Frame */
	stRect = ONDK_Rect(NX_PRISM_RECORDINGS_FRAME_CONTENT_X, NX_PRISM_RECORDINGS_FRAME_CONTENT_Y,
						NX_PRISM_RECORDINGS_FRAME_CONTENT_W, NX_PRISM_RECORDINGS_FRAME_CONTENT_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_RECORDINGS_FRAME_CONTENT_ID, &stRect, COL(C_G_Whistle_05));

	/* Draw content item (grid list) */
	nx_recordings_SetUiContentsItemObject(pstContents);

	/* Draw content Info Box */
	nx_recordings_DrawStorageInfo(pstContents);

	//nx_recordings_SetUiNoScheduleObject(pstContents);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_SelectedItemList(Nx_RecordingsContext_t *pstContents)
{
	ONDK_Result_t	ret = ONDK_RET_OK;
	Nx_RecordItem_t *pstItem = NULL;
	HINT32 nCurrentIndex = ONDK_GWM_COM_GridList_GetCurrentIndexByObjID(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);

	switch(pstContents->eCurrentGroupType)
	{
	case eNxRecGroup_PROGRAMME:
	{
		pstItem = (Nx_RecordItem_t*)HLIB_VECTOR_ItemAt(pstContents->recList, nCurrentIndex);
		if (pstItem != NULL)
		{
			NX_PRISM_PLAYMGR_PlayStart(ePlayerType_PVRPLAYBACK, (void*)pstItem->ulContentId, 0, FALSE);
		}
	}
		break;

	case eNxRecGroup_CHANNEL:
	{
		Nx_RecGroupByChannel_t *pChGroup = (Nx_RecGroupByChannel_t*)HLIB_VECTOR_ItemAt(pstContents->channelRecList, nCurrentIndex);
		if (pChGroup)
		{
			ONDK_GWM_APP_HideUI();
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_RECORDINGS_Ext_Proc",
									APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_RECORDINGS_Ext_Proc,
									HANDLE_NULL, eNxRecGroup_CHANNEL, (HUINT32)pChGroup->szChannelName, (HUINT32)pChGroup);
		}
	}
		break;

	case eNxRecGroup_DOW:
		break;

	default:
		break;
	}

	return ret;



	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_MsgGwmCreate(Nx_RecordingsContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_RecordingsContext_t));

	//NX_RECORDER_ForcedLoadRecItem();
	NX_RECORDER_ReloadRecItem();
	// 먼저 데이터부터 읽어온다..
	nx_recordings_MakeRecItemList(pstContents);
	pstContents->eCurrentGroupType = eNxRecGroup_PROGRAMME;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_RECORDINGS_WINDOW_X, NX_PRISM_RECORDINGS_WINDOW_Y,
							NX_PRISM_RECORDINGS_WINDOW_W, NX_PRISM_RECORDINGS_WINDOW_H);

	nx_recordings_SetUiObject(pstContents);

	ONDK_GWM_Obj_DisableObject(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
	ONDK_GWM_Obj_SetFocus(NX_PRISM_RECORDINGS_MENU_LIST_ID);

	ONDK_GWM_SetTimer(NX_RECORDINGS_TIMEOUT_ID, NX_RECORDINGS_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_RECORDINGS_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Recordings);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Recordings !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_recordings_MsgGwmDestroy(Nx_RecordingsContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_RECORDINGS_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_RECORDINGS_TIME_UPDATE_ID);

	NX_PANEL_ClearString();

	nx_recordings_ClearRecList(pstContents);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_recordings_MsgGwmHapiMsg(Nx_RecordingsContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_recordings_MsgGwmKeyDown(Nx_RecordingsContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (p1)
	{
		/* keys to exit */
		case KEY_BACK:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_STANDBY:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
		#if 1
			if(ONDK_GWM_Obj_GetObjectID(ONDK_GWM_Obj_GetFocusedObject()) == NX_PRISM_RECORDINGS_CONTENTS_GRID_ID)
				return nx_recordings_SelectedItemList(pstContents);
			break;
		#endif

		case KEY_ARROWLEFT:
			 nx_recordings_ArrowLeft();
			 break;

		case KEY_ARROWRIGHT:
		{
			ONDK_Result_t res;
			res = nx_recordings_ArrowRight();
			if(res == ONDK_RET_OK)
				break;
			else
				return ONDK_MESSAGE_BREAK;
		}
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		case KEY_CH_PLUS:
		case KEY_CH_MINUS:
			break;

		case KEY_OPT:
			break;

		default:
			NX_APP_Print("nx_recordings_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", p1);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_MenuListSelect(Nx_RecordingsContext_t *pstContents, HINT32 lIndex)
{
	switch(lIndex)
	{
	case 0:
		ONDK_GWM_COM_GridList_ResetItemNum(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID, pstContents->nRecListNum);
		pstContents->eCurrentGroupType = eNxRecGroup_PROGRAMME;
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
		break;

	case 1:
		HxLOG_Error("Selected List Channel\n");
		{
			if (nx_recordings_MakeChannelGroupRecList(pstContents) == ONDK_RET_OK)
			{
				ONDK_GWM_COM_GridList_ResetItemNum(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID, HLIB_VECTOR_Length(pstContents->channelRecList));
				pstContents->eCurrentGroupType = eNxRecGroup_CHANNEL;
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
			}

			#if 0
			ONDK_GWM_APP_HideUI();
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_RECORDINGS_Ext_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_RECORDINGS_Ext_Proc, HANDLE_NULL, "CH ZDF", 0, 0);
			#endif
		}
		break;

	case 2:
		break;

	case 3:
		break;

	default:
		break;
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_MsgGwmListSelected(Nx_RecordingsContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lListId = p1;
	HINT32			lListIdx = p2;

	if (lListId == NX_PRISM_RECORDINGS_MENU_LIST_ID)
	{
		nx_recordings_MenuListSelect(pstContents, lListIdx);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_MsgGwmTimer(Nx_RecordingsContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_RECORDINGS_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_RECORDINGS_LOAD_THUMBNAIL_TIMER_ID)
	{
		DxRecListData_t *pstRecItemFullData = NULL;
		HINT32 i = 0;
		HINT32 len = HLIB_VECTOR_Length(pstContents->recList);

		for(i = 0; i < len; i++)
		{
			Nx_RecordItem_t *pItem = (Nx_RecordItem_t*)HLIB_VECTOR_ItemAt(pstContents->recList, i);
			if (pItem && !pItem->bIsExistThumbnail)
			{
				pstRecItemFullData = NX_PORT_META_RECORD_GetByCID(pItem->ulContentId);

				if (pstRecItemFullData != NULL)
				{
					HCHAR	szMountPath[256] = {'\0',};
					HCHAR	szFullPath[DxRECLIST_URL] = {'\0',};
					HCHAR 	*pszThumb = NULL;

					// get thumbnail path
					HxSTD_PrintToStrN(szFullPath, DxRECLIST_URL, "HPVR:/%s%s%s",
										pstRecItemFullData->stMeta.szPath,
										pstRecItemFullData->stMeta.szFileName, DxEXT_META_FILE);
					if (HLIB_DIR_GetMountPath(pstRecItemFullData->stMeta.szPath, szMountPath, 256) == ERR_OK)
					{
						pszThumb = (HCHAR*)NX_RECORDER_GetThumbnail(szFullPath, 2, szMountPath, FALSE);
						if (pszThumb)
						{
							HxSTD_snprintf(pItem->szThumbnailPath, DxRECLIST_URL+1, "%s", pszThumb);
							NX_APP_Free(pszThumb);
						}

						HxLOG_Debug("pstItem->szThumbnailPath(%s)\n", pItem->szThumbnailPath);
					}

					// clone 된것이므로, free 해야함.
					NX_APP_Free(pstRecItemFullData);
					pstRecItemFullData = NULL;
				}
			}
		}
		ONDK_GWM_KillTimer(NX_RECORDINGS_LOAD_THUMBNAIL_TIMER_ID);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
	}
	else if (p1 == NX_PRISM_RECORDINGS_TIME_UPDATE_ID)
	{
		nx_recordings_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}
#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	else if (p1 == NX_RECORDINGS_LIST_ANI_ID)
	{
		if (nx_recordings_SetAnimationUpdate() == -1)
		{
			ONDK_GWM_KillTimer(NX_RECORDINGS_LIST_ANI_ID);
		}
	}
#endif
	else
	{
		NX_APP_Error("Error!!! Unknow Timer ID : (0x%x)\n", p1);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

#define	________________DATA_______________

#define	__________RELATED_RECORDING_ITEM__________
static void		nx_recordings_RecItemDelete (void *item)
{
	if(item){		NX_APP_Free(item);		item = NULL;	}
}

static ONDK_Result_t	nx_recordings_ClearRecList(Nx_RecordingsContext_t *pstContents)
{
	if (pstContents->recList)
		HLIB_VECTOR_Delete(pstContents->recList);
	pstContents->recList = NULL;
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_MakeRecItemList(Nx_RecordingsContext_t *pstContents)
{
	HINT32			i = 0;
	HxVector_t		*recCoreList 	= NULL;
	HINT32			nRecCoreListNum = 0;
	NxRecItem_t		*pstNxRecItem 	= NULL;
	Nx_RecordItem_t	*pstNewRecItem	= NULL;
	DxRecListData_t *pstRecItemFullData = NULL;

	// clear list
	nx_recordings_ClearRecList(pstContents);

	pstContents->recList = HLIB_VECTOR_NewEx(NULL, 0,
								(void *(*)(void *))NULL,
								(void *(*)(void *))nx_recordings_RecItemDelete,
								NULL);

	// load nx core recording list
	NX_RECORDER_GetRecItems(&recCoreList, &nRecCoreListNum);

#if 0
	for (i = 0; i < 100; i++)
	{
		pstNewRecItem = (Nx_RecordItem_t*)NX_APP_Calloc(sizeof(Nx_RecordItem_t));
		pstNewRecItem->ulContentId 	= i;
		pstNewRecItem->ulSvcUid		= i;
		pstNewRecItem->bIsExistThumbnail = FALSE;

		HxSTD_snprintf(pstNewRecItem->szName, DxRECLIST_NAME_LEN+1, "Name:[%d]", i);

		HLIB_VECTOR_Add(pstContents->recList, pstNewRecItem);
		pstContents->nRecListNum++;
	}
#endif

#if 1

	if (nRecCoreListNum <= 0)
	{
		HxLOG_Error("No Recording Item.\n");
		return ONDK_RET_OK;
	}

	for (i = 0; i < nRecCoreListNum; i++)
	{
		pstNxRecItem = (NxRecItem_t*)HLIB_VECTOR_ItemAt(recCoreList, i);

		if (pstNxRecItem == NULL)
			continue;

		pstRecItemFullData = NX_PORT_META_RECORD_GetByCID(pstNxRecItem->ulContentId);

		if (pstRecItemFullData != NULL)
		{
			HCHAR	szMountPath[256] = {'\0',};
			HCHAR	szFullPath[DxRECLIST_URL] = {'\0',};
			HCHAR 	*pszThumb = NULL;

			pstNewRecItem = (Nx_RecordItem_t*)NX_APP_Calloc(sizeof(Nx_RecordItem_t));
			pstNewRecItem->ulContentId 	= pstRecItemFullData->ulContentId;
			pstNewRecItem->ulSvcUid		= pstRecItemFullData->svcUId;
			pstNewRecItem->ulRecStartTime	 = pstRecItemFullData->ulRecStartTime;
			pstNewRecItem->bIsExistThumbnail = FALSE;

			HxSTD_snprintf(pstNewRecItem->szName, DxRECLIST_NAME_LEN+1, "%s", pstRecItemFullData->szName);

			// get thumbnail path
			HxSTD_PrintToStrN(szFullPath, DxRECLIST_URL, "HPVR:/%s%s%s",
								pstRecItemFullData->stMeta.szPath,
								pstRecItemFullData->stMeta.szFileName, DxEXT_META_FILE);
			if (HLIB_DIR_GetMountPath(pstRecItemFullData->stMeta.szPath, szMountPath, 256) == ERR_OK)
			{
				pszThumb = (HCHAR*)NX_RECORDER_GetThumbnail(szFullPath, 2, szMountPath, FALSE);
				if (pszThumb)
				{
					HxSTD_snprintf(pstNewRecItem->szThumbnailPath, DxRECLIST_URL+1, "%s", pszThumb);
					NX_APP_Free(pszThumb);
				}

				HxLOG_Debug("pstItem->szThumbnailPath(%s)\n", pstNewRecItem->szThumbnailPath);
			}

			HxLOG_Debug("\nrec item. svcuid(%u), thumbnail(%s), url(%s), path(%s), szFileName(%s)\n\n",
							pstNewRecItem->ulSvcUid, pstNewRecItem->szThumbnailPath, pstRecItemFullData->szUrl,
							pstRecItemFullData->stMeta.szPath, pstRecItemFullData->stMeta.szFileName);

			// clone 된것이므로, free 해야함.
			NX_APP_Free(pstRecItemFullData);
			pstRecItemFullData = NULL;

			HLIB_VECTOR_Add(pstContents->recList, pstNewRecItem);
			pstContents->nRecListNum++;
		}
	}

	// thumbnail 이 바로 만들어질수 없으므로, 새로운 아이템을 위해, thumbail 을 다시 만들도록 timer 를 돌림.
	// 일단은 한번만..
	ONDK_GWM_SetTimer(NX_RECORDINGS_LOAD_THUMBNAIL_TIMER_ID, NX_RECORDINGS_LOAD_THUMBNAIL_TIMEOUT);

#endif
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordings_DrawRecItemFunc(Nx_RecordingsContext_t*pstContents, HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex)
{
	ONDK_Color_t	title_fontColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	hFontHandle;
	HUINT32			ulFontSize;
	HINT32			currentIndex=0;
	// mykim
	HINT32			nSurfaceX = nColIndex * NX_PRISM_RECORDINGS_CONTENTS_ITEM_W;
	ONDK_Rect_t		rectMainImg = {nSurfaceX + 10, 11, 170, 96};
	Nx_RecordItem_t *pstItem = NULL;

	currentIndex = ONDK_GWM_COM_GridList_GetCurrentIndexByObjID(id);

	pstItem = (Nx_RecordItem_t*)HLIB_VECTOR_ItemAt(pstContents->recList, drawItem->item_index);
	// draw image
	if((pstItem != NULL) && (pstItem->bIsExistThumbnail == FALSE))
	{
		if (HLIB_FILE_Exist(pstItem->szThumbnailPath) == TRUE )
		{
			ONDK_Rect_t srcRect;
			ONDK_Size_t size;
			ONDKImageHandle hThum = NULL;

			hThum = ONDK_IMG_CreateSelf((HUINT8*)pstItem->szThumbnailPath);

			size = ONDK_IMG_GetSize(hThum);
			srcRect.x = 0;
			srcRect.y = 0;
			srcRect.w = size.w;
			srcRect.h = size.h;
			ONDK_SURFACE_StretchBlit(hThum, screen, srcRect, rectMainImg);

			pstItem->bIsExistThumbnail == TRUE;
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, rectMainImg, RES_212_04_THUMB_REC02_PNG);
		}
	}
	else
	{
		if ((pstItem != NULL) && (HLIB_FILE_Exist(pstItem->szThumbnailPath) == TRUE))
		{
			ONDK_Rect_t srcRect;
			ONDK_Size_t size;
			ONDKImageHandle hThum = NULL;

			hThum = ONDK_IMG_CreateSelf((HUINT8*)pstItem->szThumbnailPath);

			size = ONDK_IMG_GetSize(hThum);
			srcRect.x = 0;
			srcRect.y = 0;
			srcRect.w = size.w;
			srcRect.h = size.h;
			ONDK_SURFACE_StretchBlit(hThum, screen, srcRect, rectMainImg);
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, rectMainImg, RES_212_04_THUMB_REC02_PNG);
		}
	}

	ulFontSize = NX_PRISM_FONT_SIZE_20;
	eFontId = eFont_SystemBold;
	title_fontColor = COL (C_T_Whistle_100);


	if (currentIndex == drawItem->item_index)
	{
		NX_PRISM_COMP_DrawFrameCursor_D1(screen, NULL, &rectMainImg, NULL, NULL, 0);

		title_fontColor = COL (C_T_Base01);
	}


	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, rectMainImg.x + 5, (rectMainImg.y + rectMainImg.h + 5),
									rectMainImg.w - 8, 54, (char*)pstItem->szName, 2, title_fontColor, ONDKSTF_LEFT);

		ONDK_FONT_Release(hFontHandle);
	}

	return ONDK_RET_OK;
}


#define	__________RELATED_CHANNEL_GROUP_ITEM__________

static void		nx_recordings_DeleteChannelRecItem (void *item)
{
	Nx_RecGroupByChannel_t *pstDelItem = (Nx_RecGroupByChannel_t*)item;

	if(pstDelItem)
	{
		if (pstDelItem->recItemList)
		{
			// Nx_RecGroupByChannel_t 안의 recItemList 의 item 은 제거할 필요 없다.
			// recList 로부터 참조된 값이므로.
			HLIB_VECTOR_Delete(pstDelItem->recItemList);
			pstDelItem->recItemList = NULL;
		}
		NX_APP_Free(pstDelItem);
		pstDelItem = NULL;
	}
}

static ONDK_Result_t	nx_recordings_ClearChannelGroupRecList(Nx_RecordingsContext_t *pstContents)
{

	if (pstContents->channelRecList)
	{
		HLIB_VECTOR_Delete(pstContents->channelRecList);
	}
	pstContents->channelRecList = NULL;
	return ONDK_RET_OK;
}

static HBOOL nx_recordings_FindChannelGroupData(void *data, void *userdata)
{
	Nx_RecGroupByChannel_t *recGroupItem = (Nx_RecGroupByChannel_t *)data;
	HUINT32 findSvcUid = (HUINT32)userdata;

	if(recGroupItem->ulSvcUid == findSvcUid)
		return TRUE;
	else
		return FALSE;
}

static HINT32 nx_recordings_ChannelRecListSortByStartTime (const void *pvArg1, const void *pvArg2)
{
	const Nx_RecordItem_t *pstRecItem1 = *(const Nx_RecordItem_t **)pvArg1;
	const Nx_RecordItem_t *pstRecItem2 = *(const Nx_RecordItem_t **)pvArg2;

	return (pstRecItem1->ulRecStartTime - pstRecItem2->ulRecStartTime);
}

static ONDK_Result_t	nx_recordings_MakeChannelGroupRecList(Nx_RecordingsContext_t *pstContents)
{
	HINT32			i = 0, reclistLen = 0;
	Nx_RecordItem_t	*pstRecItem = NULL;

	if (pstContents == NULL || pstContents->recList == NULL)
	{
		HxLOG_Error("pstContents or pstContents->recList is NULL!\n");
		return ONDK_RET_FAIL;
	}

	// clear list
	nx_recordings_ClearChannelGroupRecList(pstContents);

	pstContents->channelRecList = HLIB_VECTOR_NewEx(NULL, 0,
								(void *(*)(void *))NULL,
								(void *(*)(void *))nx_recordings_DeleteChannelRecItem,
								NULL);


	reclistLen = HLIB_VECTOR_Length(pstContents->recList);

	if (reclistLen <= 0)
	{
		HxLOG_Error("No Recording Item!\n");
		return ONDK_RET_FAIL;
	}

	for (i = 0; i < reclistLen; i++)
	{
		Nx_RecGroupByChannel_t *pstFoundChanneItem = NULL;
		pstRecItem = HLIB_VECTOR_ItemAt(pstContents->recList, i);
		if (pstRecItem == NULL)
			continue;

		pstFoundChanneItem = (Nx_RecGroupByChannel_t*)HLIB_VECTOR_Find(pstContents->channelRecList,
									nx_recordings_FindChannelGroupData, (void*)pstRecItem->ulSvcUid);

		if (pstFoundChanneItem)
		{
			HLIB_VECTOR_Add(pstFoundChanneItem->recItemList, pstRecItem);	// 참조로 넣은것이므로 제거하면 안됨.
			HLIB_VECTOR_Sort(pstFoundChanneItem->recItemList, nx_recordings_ChannelRecListSortByStartTime);
		}
		else
		{
			DxService_t		*pstService = NULL;
			Nx_RecGroupByChannel_t *pstNewData = (Nx_RecGroupByChannel_t*)NX_APP_Calloc(sizeof(Nx_RecGroupByChannel_t));
			pstNewData->recItemList = HLIB_VECTOR_New(NULL,NULL);

			pstService = NX_CHANNEL_FindServiceBySvcUid(pstRecItem->ulSvcUid);

			pstNewData->ulSvcUid = pstRecItem->ulSvcUid;

			if (pstService != NULL)
			{
				HxSTD_snprintf(pstNewData->szChannelNo, 5, "%04d", pstService->lcn);
				HxSTD_snprintf(pstNewData->szChannelName, 64, "%s", pstService->name);
			}
			else
			{
				HxSTD_snprintf(pstNewData->szChannelNo, 5, "0000");
				HxSTD_snprintf(pstNewData->szChannelName, 64, "UNKNOWN");
			}

			HLIB_VECTOR_Add(pstNewData->recItemList, pstRecItem);	// 참조로 넣은것이므로 제거하면 안됨.

			HLIB_VECTOR_Add(pstContents->channelRecList, pstNewData);
		}
	}

#if defined(CONFIG_DEBUG)
	{
		HINT32 j = 0;
		HINT32 len = HLIB_VECTOR_Length(pstContents->channelRecList);

		for (j = 0; j < len; j++)
		{
			Nx_RecGroupByChannel_t *pstChannel = HLIB_VECTOR_ItemAt(pstContents->channelRecList, j);

			if (pstChannel)
			{
				HINT32 ii = 0;
				HxLOG_Debug("\n RecChannel svcuid(%d), no(%s), name(%s), len(%d)\n\n",
							pstChannel->ulSvcUid, pstChannel->szChannelNo, pstChannel->szChannelName,
							HLIB_VECTOR_Length(pstChannel->recItemList));
				for (ii = 0; ii < HLIB_VECTOR_Length(pstChannel->recItemList); ii++)
				{
					Nx_RecordItem_t *pstRecData = (Nx_RecordItem_t*)HLIB_VECTOR_ItemAt(pstChannel->recItemList, ii);

					HxLOG_Debug("\n\t(%d): startTime(%u), ID(%u), eventName(%s)\n", ii,
						pstRecData->ulRecStartTime, pstRecData->ulContentId, pstRecData->szName);
				}
			}
		}
	}
#endif

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_recordings_DrawChannelGroupItemFunc(Nx_RecordingsContext_t*pstContents, HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex)
{
	ONDK_Color_t	title_fontColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	hFontHandle, hFontGroupNum;
	HUINT32			ulFontSize;
	HINT32			tempWidth = 0;
	HINT32			nGroupItemLen = 0;
	HINT32			currentIndex=0;
	// mykim
	HINT32			nSurfaceX = nColIndex * NX_PRISM_RECORDINGS_CONTENTS_ITEM_W;
	ONDK_Rect_t		rectMainImg = {nSurfaceX + 10, 11, 170, 96};
	Nx_RecGroupByChannel_t *pstChannelItem = NULL;
	Nx_RecordItem_t *pstRecItem = NULL;

	currentIndex = ONDK_GWM_COM_GridList_GetCurrentIndexByObjID(id);

	if (pstContents->channelRecList)
		pstChannelItem = (Nx_RecGroupByChannel_t*)HLIB_VECTOR_ItemAt(pstContents->channelRecList, drawItem->item_index);

	if ((pstChannelItem != NULL) && (pstChannelItem->recItemList != NULL))
	{
		// 처음 item 의 정보를 가져옴.
		pstRecItem = (Nx_RecordItem_t*)HLIB_VECTOR_ItemAt(pstChannelItem->recItemList, 0);
	}
	else
		pstRecItem = NULL;

	// draw image
	if((pstRecItem != NULL) && (pstRecItem->bIsExistThumbnail == FALSE))
	{
		if (HLIB_FILE_Exist(pstRecItem->szThumbnailPath) == TRUE )
		{
			ONDK_Rect_t srcRect;
			ONDK_Size_t size;
			ONDKImageHandle hThum = NULL;

			hThum = ONDK_IMG_CreateSelf((HUINT8*)pstRecItem->szThumbnailPath);

			size = ONDK_IMG_GetSize(hThum);
			srcRect.x = 0;
			srcRect.y = 0;
			srcRect.w = size.w;
			srcRect.h = size.h;
			ONDK_SURFACE_StretchBlit(hThum, screen, srcRect, rectMainImg);

			pstRecItem->bIsExistThumbnail == TRUE;
		}
		else
			ONDK_GUICOM_IMG_Blit(screen, rectMainImg, RES_212_04_THUMB_REC02_PNG);
	}
	else	// 처음 thumbnail 을 못얻어오지만, 재시도후 얻어오는 경우.
	{
		if ((pstRecItem != NULL) && (HLIB_FILE_Exist(pstRecItem->szThumbnailPath) == TRUE))
		{
			ONDK_Rect_t srcRect;
			ONDK_Size_t size;
			ONDKImageHandle hThum = NULL;

			hThum = ONDK_IMG_CreateSelf((HUINT8*)pstRecItem->szThumbnailPath);

			size = ONDK_IMG_GetSize(hThum);
			srcRect.x = 0;
			srcRect.y = 0;
			srcRect.w = size.w;
			srcRect.h = size.h;
			ONDK_SURFACE_StretchBlit(hThum, screen, srcRect, rectMainImg);
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, rectMainImg, RES_212_04_THUMB_REC02_PNG);
		}
	}

	// draw ui
	eFontId = eFont_SystemBold;
	ulFontSize = NX_PRISM_FONT_SIZE_18;

	// draw group length
	if ((pstChannelItem != NULL) && (pstChannelItem->recItemList != NULL))
		nGroupItemLen = HLIB_VECTOR_Length(pstChannelItem->recItemList);
	tempWidth = 24;
	if (nGroupItemLen > 99)		tempWidth = 36;
	if (nGroupItemLen > 999)	tempWidth = 45;
	ONDK_DRAW_FillRectElement(screen, (drawItem->itemRect.x + NX_PRISM_RECORDINGS_CONTENTS_ITEM_W - (tempWidth + 14)),
										 76, tempWidth, 25, 0xff000000);
	hFontGroupNum = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontGroupNum)
	{
		HCHAR szChannelNum[5] = {'\0',};
		HxSTD_snprintf(szChannelNum, 5, "%d", nGroupItemLen);
		title_fontColor = COL (C_T_Gong02);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontGroupNum, (char*)szChannelNum,
						(drawItem->itemRect.x + NX_PRISM_RECORDINGS_CONTENTS_ITEM_W - (tempWidth + 14)),
						76, tempWidth, title_fontColor, ONDKSTF_TOPCENTER);
		ONDK_FONT_Release(hFontGroupNum);
	}

	// draw channel inform
	ulFontSize = NX_PRISM_FONT_SIZE_20;
	title_fontColor = COL (C_T_Whistle_100);

	if (currentIndex == drawItem->item_index)
	{
		NX_PRISM_COMP_DrawFrameCursor_D1(screen, NULL, &rectMainImg, NULL, NULL, 0);

		title_fontColor = COL (C_T_Base01);
	}

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		HCHAR szChanneInform[64] = {'\0',};
		if (pstChannelItem)
			HxSTD_snprintf(szChanneInform, 64, "%s %s", pstChannelItem->szChannelNo, pstChannelItem->szChannelName);
		ONDK_FONT_DrawMultiLineAlign(screen, hFontHandle, rectMainImg.x + 5, (rectMainImg.y + rectMainImg.h + 5),
									rectMainImg.w - 8, 54, (char*)szChanneInform, 2, title_fontColor, ONDKSTF_LEFT);

		ONDK_FONT_Release(hFontHandle);
	}

	return ONDK_RET_OK;
}


#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
static ONDK_Result_t	nx_recordings_SetAnimationCtl(void)
{
	ONDK_GWM_SetTimer(NX_RECORDINGS_LIST_ANI_ID, NX_RECORDINGS_LIST_ANI_TIMEOUT);
	return ONDK_RET_OK;
}

static int	nx_recordings_SetAnimationUpdate(void)
{
	return ONDK_GWM_COM_GridList_UpdateAnimation(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
}
#endif


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_RECORDINGS_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_RecordingsContext_t	*pstContents = NULL;

	pstContents = nx_recordings_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_recordings_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_recordings_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_recordings_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_recordings_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_LIST_SELECTED:
			NX_APP_Message("MSG_GWM_LIST_SELECTED\n");
			eResult = nx_recordings_MsgGwmListSelected(pstContents, p1, p2, p3);
			break;

		case MSG_APP_RECORDINGS_EXT_DESTROY:
			NX_APP_Message("MSG_APP_RECORDINGS_EXT_DESTROY!\n");
			ONDK_GWM_APP_ShowUI();
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_recordings_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Record)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

#endif
/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

