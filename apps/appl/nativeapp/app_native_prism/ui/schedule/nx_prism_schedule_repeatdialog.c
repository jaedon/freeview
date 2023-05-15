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
 * @file	  		nx_prism_schedule_repeatdialog.c
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
#define	NX_PRISM_SCHEDULE_REPEAT_ITEM_ID						eMenuFrame_Window_ScheduleRepeat

#define	NX_PRISM_SCHEDULE_REPEAT_WINDOW_X						290
#define	NX_PRISM_SCHEDULE_REPEAT_WINDOW_Y						236
#define	NX_PRISM_SCHEDULE_REPEAT_WINDOW_W						700
#define	NX_PRISM_SCHEDULE_REPEAT_WINDOW_H						248

#define	NX_PRISM_SCHEDULE_REPEAT_FRAME_ID						(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 1)
#define	NX_PRISM_SCHEDULE_REPEAT_FRAME_X						0
#define	NX_PRISM_SCHEDULE_REPEAT_FRAME_Y						0
#define	NX_PRISM_SCHEDULE_REPEAT_FRAME_W						NX_PRISM_SCHEDULE_REPEAT_WINDOW_W
#define	NX_PRISM_SCHEDULE_REPEAT_FRAME_H						NX_PRISM_SCHEDULE_REPEAT_WINDOW_H

#define	NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_ID					(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 2)
#define	NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_X						30
#define	NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_Y						29
#define	NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_W						640
#define	NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_H						(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_X					111
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_Y					123
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_W					66
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_H					(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_MG					3
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SUN_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 3)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_MON_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 4)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_TUE_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 5)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_WED_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 6)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_THU_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 7)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_FRI_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 8)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SAT_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 9)

#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_X						127
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_Y						76
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_W						34
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_H						34
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_MG					35
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SUN_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 10)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_MON_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 11)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_TUE_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 12)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_WED_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 13)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_THU_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 14)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_FRI_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 15)
#define	NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SAT_ID				(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 16)

#define	NX_PRISM_SCHEDULE_REPEAT_BTN_OK_ID						(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 17)
#define	NX_PRISM_SCHEDULE_REPEAT_BTN_OK_X						123
#define	NX_PRISM_SCHEDULE_REPEAT_BTN_OK_Y						168
#define	NX_PRISM_SCHEDULE_REPEAT_BTN_OK_W						220
#define	NX_PRISM_SCHEDULE_REPEAT_BTN_OK_H						50
#define	NX_PRISM_SCHEDULE_REPEAT_BTN_MG							15

#define	NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_ID					(NX_PRISM_SCHEDULE_REPEAT_ITEM_ID + 18)
#define	NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_X					(NX_PRISM_SCHEDULE_REPEAT_BTN_OK_X + NX_PRISM_SCHEDULE_REPEAT_BTN_OK_W + NX_PRISM_SCHEDULE_REPEAT_BTN_MG)
#define	NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_Y					168
#define	NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_W					220
#define	NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_H					50

#define	NX_PRISM_SCHEDULE_REPEAT_DAY_MAX_ITEM					7


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxUiScheduleRepeatItem_None = 0,
	eNxUiScheduleRepeatItem_Sun,
	eNxUiScheduleRepeatItem_Mon,
	eNxUiScheduleRepeatItem_Tue,
	eNxUiScheduleRepeatItem_Wed,
	eNxUiScheduleRepeatItem_Thu,
	eNxUiScheduleRepeatItem_Fri,
	eNxUiScheduleRepeatItem_Sat,
} NxUiScheduleRepeatItem_t;


typedef struct NxVolume_Context
{
	DxRsvRepeat_e			eRepeat;
	HINT32					lRepeatDate;
	Nx_ScheduleMode_t		eScheduleMode;

	HBOOL					abDayCheckList[NX_PRISM_SCHEDULE_REPEAT_DAY_MAX_ITEM];
	NX_Util_OPTIONList_t	stDayList;
} NxUiScheduleRepeatContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static NxUiScheduleRepeatContent_t		s_stScheduleRepeatContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NxUiScheduleRepeatContent_t*	nx_scheduleRepeat_GetContents (void);

static ONDK_Result_t	nx_scheduleRepeat_SetRepeatDate(NxUiScheduleRepeatContent_t *pstContents, HUINT32 ulDayItem);
static ONDK_Result_t	nx_scheduleRepeat_MakeRepeatDate(NxUiScheduleRepeatContent_t *pstContents, HINT32 *plRepeatDate, DxRsvRepeat_e *peRsvRepeat);
static ONDK_Result_t	nx_scheduleRepeat_ChangeCheckDay(NxUiScheduleRepeatContent_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_scheduleRepeat_DrawCheckButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t 	nx_scheduleRepeat_DrawDayCheckList(NxUiScheduleRepeatContent_t *pstContents);
static ONDK_Result_t 	nx_scheduleRepeat_DrawDayTextList(NxUiScheduleRepeatContent_t *pstContents);
static ONDK_Result_t 	nx_scheduleRepeat_SetUiObject(NxUiScheduleRepeatContent_t *pstContents);

static ONDK_Result_t	nx_scheduleRepeat_LoadRepeatDate(NxUiScheduleRepeatContent_t *pstContents);
static ONDK_Result_t	nx_scheduleRepeat_InitData(NxUiScheduleRepeatContent_t *pstContents);

static ONDK_Result_t 	nx_scheduleRepeat_MsgGwmCreate(NxUiScheduleRepeatContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_scheduleRepeat_MsgGwmDestroy(NxUiScheduleRepeatContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_scheduleRepeat_MsgGwmKeyDown(NxUiScheduleRepeatContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_scheduleRepeat_MsgGwmTimer(NxUiScheduleRepeatContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_scheduleRepeat_MsgGwmClickMsg(NxUiScheduleRepeatContent_t *pstContents, HINT32 p1);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxUiScheduleRepeatContent_t*	nx_scheduleRepeat_GetContents(void)
{
	return	&s_stScheduleRepeatContent;
}


static ONDK_Result_t	nx_scheduleRepeat_SetRepeatDate(NxUiScheduleRepeatContent_t *pstContents, HUINT32 ulDayItem)
{
	HUINT32 		i = 0;

	for ( i = 0; i < pstContents->stDayList.ulOptItemNum; i++)
	{
		if (i == NX_PRISM_SCHEDULE_REPEAT_DAY_MAX_ITEM)
		{
			break;
		}

		if (ulDayItem == pstContents->stDayList.aeOptItemList[i])
		{
			pstContents->abDayCheckList[i] = (pstContents->abDayCheckList[i] == TRUE) ? FALSE : TRUE ;
			break;
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleRepeat_MakeRepeatDate(NxUiScheduleRepeatContent_t *pstContents, HINT32 *plRepeatDate, DxRsvRepeat_e *peRsvRepeat)
{
	HINT32			lMakeRepeatDate = eDxSCHED_DATE_NONE, lRepeatDate = eDxSCHED_DATE_NONE;
	HUINT32 		i = 0, ulDayCount = 0;
	DxRsvRepeat_e	eRsvRepeat = DxRSVREPEAT_ONCE;

	for ( i = 0; i < pstContents->stDayList.ulOptItemNum; i++)
	{
		if (i == NX_PRISM_SCHEDULE_REPEAT_DAY_MAX_ITEM)
		{
			break;
		}

		switch (pstContents->stDayList.aeOptItemList[i])
		{
			case eNxUiScheduleRepeatItem_Sun:
				if (pstContents->abDayCheckList[i] == TRUE)
				{
					lRepeatDate = (lRepeatDate | eDxSCHED_DATE_SUN);
					ulDayCount++;
				}
				break;
			case eNxUiScheduleRepeatItem_Mon:
				if (pstContents->abDayCheckList[i] == TRUE)
				{
					lRepeatDate = (lRepeatDate | eDxSCHED_DATE_MON);
					ulDayCount++;
				}
				break;
			case eNxUiScheduleRepeatItem_Tue:
				if (pstContents->abDayCheckList[i] == TRUE)
				{
					lRepeatDate = (lRepeatDate | eDxSCHED_DATE_TUE);
					ulDayCount++;
				}
				break;
			case eNxUiScheduleRepeatItem_Wed:
				if (pstContents->abDayCheckList[i] == TRUE)
				{
					lRepeatDate = (lRepeatDate | eDxSCHED_DATE_WED);
					ulDayCount++;
				}
				break;
			case eNxUiScheduleRepeatItem_Thu:
				if (pstContents->abDayCheckList[i] == TRUE)
				{
					lRepeatDate = (lRepeatDate | eDxSCHED_DATE_THU);
					ulDayCount++;
				}
				break;
			case eNxUiScheduleRepeatItem_Fri:
				if (pstContents->abDayCheckList[i] == TRUE)
				{
					lRepeatDate = (lRepeatDate | eDxSCHED_DATE_FRI);
					ulDayCount++;
				}
				break;
			case eNxUiScheduleRepeatItem_Sat:
				if (pstContents->abDayCheckList[i] == TRUE)
				{
					lRepeatDate = (lRepeatDate | eDxSCHED_DATE_SAT);
					ulDayCount++;
				}
				break;
		}
	}

	if ((lRepeatDate & eDxSCHED_DATE_SUN) && (lRepeatDate & eDxSCHED_DATE_MON) && (lRepeatDate & eDxSCHED_DATE_TUE)
			&& (lRepeatDate & eDxSCHED_DATE_WED) && (lRepeatDate & eDxSCHED_DATE_THU) && (lRepeatDate & eDxSCHED_DATE_FRI) && (lRepeatDate & eDxSCHED_DATE_SAT))
	{
		lMakeRepeatDate = eDxSCHED_DATE_ALL;
		eRsvRepeat = DxRSVREPEAT_DAILY;
	}
	else if ((lRepeatDate & eDxSCHED_DATE_MON) && (lRepeatDate & eDxSCHED_DATE_TUE) && (lRepeatDate & eDxSCHED_DATE_WED)
				&& (lRepeatDate & eDxSCHED_DATE_THU) && (lRepeatDate & eDxSCHED_DATE_FRI) && (lRepeatDate & eDxSCHED_DATE_SAT))
	{
		lMakeRepeatDate = lRepeatDate;
		eRsvRepeat = DxRSVREPEAT_WITHOUT_SUNDAY;
	}
	else if ((lRepeatDate & eDxSCHED_DATE_MON) && (lRepeatDate & eDxSCHED_DATE_TUE) && (lRepeatDate & eDxSCHED_DATE_WED) && (lRepeatDate & eDxSCHED_DATE_THU) && (lRepeatDate & eDxSCHED_DATE_FRI))
	{
		lMakeRepeatDate = eDxSCHED_DATE_WEEKDAYS;
		eRsvRepeat = DxRSVREPEAT_WEEKDAYS;
	}
	else if ((lRepeatDate & eDxSCHED_DATE_MON) && (lRepeatDate & eDxSCHED_DATE_TUE) && (lRepeatDate & eDxSCHED_DATE_WED) && (lRepeatDate & eDxSCHED_DATE_THU) && (lRepeatDate & eDxSCHED_DATE_FRI))
	{
		lMakeRepeatDate = eDxSCHED_DATE_WEEKEND;
		eRsvRepeat = DxRSVREPEAT_WEEKEND;
	}
	else
	{
		lMakeRepeatDate = lRepeatDate;
		if (ulDayCount > 1)
		{
			eRsvRepeat = DxRSVREPEAT_FEWDAY_OF_WEEK;
		}
		else
		{
			eRsvRepeat = DxRSVREPEAT_ONCE;
		}
	}

	if (lMakeRepeatDate == eDxSCHED_DATE_NONE)
	{
		eRsvRepeat = DxRSVREPEAT_ONCE;
	}

	*plRepeatDate = lMakeRepeatDate;
	*peRsvRepeat = eRsvRepeat;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleRepeat_ChangeCheckDay(NxUiScheduleRepeatContent_t *pstContents, HINT32 lObjId)
{
	HUINT32 	ulDayItem = eNxUiScheduleRepeatItem_None;

	switch (lObjId)
	{
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SUN_ID:	ulDayItem = eNxUiScheduleRepeatItem_Sun;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_MON_ID:	ulDayItem = eNxUiScheduleRepeatItem_Mon;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_TUE_ID:	ulDayItem = eNxUiScheduleRepeatItem_Tue;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_WED_ID:	ulDayItem = eNxUiScheduleRepeatItem_Wed;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_THU_ID:	ulDayItem = eNxUiScheduleRepeatItem_Thu;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_FRI_ID:	ulDayItem = eNxUiScheduleRepeatItem_Fri;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SAT_ID:	ulDayItem = eNxUiScheduleRepeatItem_Sat;	break;
		default:
			NX_APP_Error("Error!!! Invalid Objid Id : (0x%x)!! \n", lObjId);
			return ONDK_RET_FAIL;
	}

	nx_scheduleRepeat_SetRepeatDate(pstContents, ulDayItem);
	ONDK_GWM_APP_InvalidateObject(lObjId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleRepeat_DrawCheckButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32			lObjId = -1;
	HUINT32 		ulStatus = 0, i = 0, ulDayItem = eNxUiScheduleRepeatItem_None, ulIndex = 0;
	HCHAR			*szImage = NULL;
	ONDK_Rect_t 	stImgRect;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;
	NxUiScheduleRepeatContent_t *pstContents = NULL;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	lCursorMg = 0;

	pstContents = nx_scheduleRepeat_GetContents();
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_FAIL;
	}

	lObjId = ONDK_GWM_Obj_GetObjectID(object);
	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);
	stImgRect = ONDK_Rect(x1, y1, w, h);

	switch (lObjId)
	{
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SUN_ID:	ulDayItem = eNxUiScheduleRepeatItem_Sun;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_MON_ID:	ulDayItem = eNxUiScheduleRepeatItem_Mon;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_TUE_ID:	ulDayItem = eNxUiScheduleRepeatItem_Tue;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_WED_ID:	ulDayItem = eNxUiScheduleRepeatItem_Wed;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_THU_ID:	ulDayItem = eNxUiScheduleRepeatItem_Thu;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_FRI_ID:	ulDayItem = eNxUiScheduleRepeatItem_Fri;	break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SAT_ID:	ulDayItem = eNxUiScheduleRepeatItem_Sat;	break;
		default:
			NX_APP_Error("Error!!! Invalid Objid Id : (0x%x)!! \n", lObjId);
			return ONDK_RET_FAIL;
			break;
	}

	for ( i = 0; i < pstContents->stDayList.ulOptItemNum; i++)
	{
		if (pstContents->stDayList.aeOptItemList[i] == ulDayItem)
		{
			ulIndex = i;
			break;
		}
	}

	/* active, front most */
	if ((ulStatus & GUIOBJ_PUSHED) || (ulStatus & GUIOBJ_HILIGHTED))
	{
		/* button feedback */
		/* get focused */
		szImage = (pstContents->abDayCheckList[ulIndex] == FALSE) ? RES_112_00_CHECK02_OFF_C_PNG : RES_112_00_CHECK02_ON_C_PNG ;
	}
	else
	{
		/* Normal Item cursor */
		szImage = (pstContents->abDayCheckList[ulIndex] == FALSE) ? RES_112_00_CHECK01_OFF_N_PNG : RES_112_00_CHECK01_ON_N_PNG ;
	}

	if (szImage)
	{
		ONDK_GUICOM_IMG_Blit(screen, stImgRect, szImage);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_scheduleRepeat_DrawDayCheckList(NxUiScheduleRepeatContent_t *pstContents)
{
	HUINT32			i = 0;
	ONDK_Rect_t 	stRect;


	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_X, NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_Y,
						NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_W, NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_H);
	for (i = 0; i < pstContents->stDayList.ulOptItemNum; i++)
	{
		if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Sun)
		{
			ONDK_GWM_COM_Button_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SUN_ID, &stRect, (HCHAR*)ONDK_GetString(RES_SUN_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SUN_ID, nx_scheduleRepeat_DrawCheckButton);
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Mon)
		{
			ONDK_GWM_COM_Button_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_MON_ID, &stRect, (HCHAR*)ONDK_GetString(RES_MON_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_MON_ID, nx_scheduleRepeat_DrawCheckButton);
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Tue)
		{
			ONDK_GWM_COM_Button_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_TUE_ID, &stRect, (HCHAR*)ONDK_GetString(RES_TUE_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_TUE_ID, nx_scheduleRepeat_DrawCheckButton);
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Wed)
		{
			ONDK_GWM_COM_Button_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_WED_ID, &stRect, (HCHAR*)ONDK_GetString(RES_WED_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_WED_ID, nx_scheduleRepeat_DrawCheckButton);
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Thu)
		{
			ONDK_GWM_COM_Button_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_THU_ID, &stRect, (HCHAR*)ONDK_GetString(RES_THU_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_THU_ID, nx_scheduleRepeat_DrawCheckButton);
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Fri)
		{
			ONDK_GWM_COM_Button_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_FRI_ID, &stRect, (HCHAR*)ONDK_GetString(RES_FRI_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_FRI_ID, nx_scheduleRepeat_DrawCheckButton);
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Sat)
		{
			ONDK_GWM_COM_Button_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SAT_ID, &stRect, (HCHAR*)ONDK_GetString(RES_SAT_ID));
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SAT_ID, nx_scheduleRepeat_DrawCheckButton);
		}
		else
		{
			continue;
		}

		stRect.x += (stRect.w + NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_MG);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_scheduleRepeat_DrawDayTextList(NxUiScheduleRepeatContent_t *pstContents)
{
	HUINT32			i = 0;
	ONDK_Rect_t 	stRect;


	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_X, NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_Y,
						NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_W, NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_H);

	for (i = 0; i < pstContents->stDayList.ulOptItemNum; i++)
	{
		if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Sun)
		{
			ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SUN_ID, &stRect, (HCHAR*)ONDK_GetString(RES_SUN_ID));
			ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SUN_ID, eFont_SystemBold);
			ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SUN_ID, NX_PRISM_FONT_SIZE_20);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SUN_ID, TEXT_ALIGN_CENTER);
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SUN_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Mon)
		{
			ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_MON_ID, &stRect, (HCHAR*)ONDK_GetString(RES_MON_ID));
			ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_MON_ID, eFont_SystemBold);
			ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_MON_ID, NX_PRISM_FONT_SIZE_20);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_MON_ID, TEXT_ALIGN_CENTER);
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_MON_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Tue)
		{
			ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_TUE_ID, &stRect, (HCHAR*)ONDK_GetString(RES_TUE_ID));
			ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_TUE_ID, eFont_SystemBold);
			ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_TUE_ID, NX_PRISM_FONT_SIZE_20);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_TUE_ID, TEXT_ALIGN_CENTER);
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_TUE_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Wed)
		{
			ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_WED_ID, &stRect, (HCHAR*)ONDK_GetString(RES_WED_ID));
			ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_WED_ID, eFont_SystemBold);
			ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_WED_ID, NX_PRISM_FONT_SIZE_20);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_WED_ID, TEXT_ALIGN_CENTER);
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_WED_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Thu)
		{
			ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_THU_ID, &stRect, (HCHAR*)ONDK_GetString(RES_THU_ID));
			ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_THU_ID, eFont_SystemBold);
			ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_THU_ID, NX_PRISM_FONT_SIZE_20);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_THU_ID, TEXT_ALIGN_CENTER);
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_THU_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Fri)
		{
			ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_FRI_ID, &stRect, (HCHAR*)ONDK_GetString(RES_FRI_ID));
			ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_FRI_ID, eFont_SystemBold);
			ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_FRI_ID, NX_PRISM_FONT_SIZE_20);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_FRI_ID, TEXT_ALIGN_CENTER);
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_FRI_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
		}
		else if (pstContents->stDayList.aeOptItemList[i] == eNxUiScheduleRepeatItem_Sat)
		{
			ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SAT_ID, &stRect, (HCHAR*)ONDK_GetString(RES_SAT_ID));
			ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SAT_ID, eFont_SystemBold);
			ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SAT_ID, NX_PRISM_FONT_SIZE_20);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SAT_ID, TEXT_ALIGN_CENTER);
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_SAT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
		}
		else
		{
			continue;
		}

		stRect.x += (stRect.w + NX_PRISM_SCHEDULE_REPEAT_WEEK_TEXT_MG);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_scheduleRepeat_SetUiObject(NxUiScheduleRepeatContent_t *pstContents)
{
	HINT8			*pszStringId = NULL;
	ONDK_Rect_t 	stRect;

	/* Draw BG Frame */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_REPEAT_FRAME_X, NX_PRISM_SCHEDULE_REPEAT_FRAME_Y,
						NX_PRISM_SCHEDULE_REPEAT_FRAME_W, NX_PRISM_SCHEDULE_REPEAT_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_SCHEDULE_REPEAT_FRAME_ID, &stRect, COL(C_G_Bang_95));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_REPEAT_FRAME_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG_TBLine);

	/* Draw Title TEXT */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_X, NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_Y,
						NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_W, NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_H);
#if defined(CONFIG_MW_MM_PVR)
	pszStringId = (pstContents->eScheduleMode == eNxSchedle_ModeWatching) ? STR_MESG_5993_ID : STR_MESG_5191_ID ;
#else
	pszStringId = RES_MESG_5993_ID;
#endif
	ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_ID, &stRect, (HCHAR*)ONDK_GetString(pszStringId));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_REPEAT_TTL_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Draw Check Button Item */
	nx_scheduleRepeat_DrawDayCheckList(pstContents);

	/* Draw Text Item */
	nx_scheduleRepeat_DrawDayTextList(pstContents);

	/* Draw Button Item */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_REPEAT_BTN_OK_X, NX_PRISM_SCHEDULE_REPEAT_BTN_OK_Y,
						NX_PRISM_SCHEDULE_REPEAT_BTN_OK_W, NX_PRISM_SCHEDULE_REPEAT_BTN_OK_H);
	ONDK_GWM_COM_Button_Create(NX_PRISM_SCHEDULE_REPEAT_BTN_OK_ID, &stRect, (HCHAR*)ONDK_GetString(RES_OK_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_REPEAT_BTN_OK_ID, NX_PRISM_COMP_DrawButton_A1);

	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_X, NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_Y,
						NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_W, NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_H);
	ONDK_GWM_COM_Button_Create(NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_ID, &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_ID, NX_PRISM_COMP_DrawButton_A1);

	ONDK_GWM_Obj_SetFocus(NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_ID);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleRepeat_LoadRepeatDate(NxUiScheduleRepeatContent_t *pstContents)
{

	if (pstContents->lRepeatDate & eDxSCHED_DATE_SUN)
	{
		nx_scheduleRepeat_SetRepeatDate(pstContents, eNxUiScheduleRepeatItem_Sun);
	}

	if (pstContents->lRepeatDate & eDxSCHED_DATE_MON)
	{
		nx_scheduleRepeat_SetRepeatDate(pstContents, eNxUiScheduleRepeatItem_Mon);
	}

	if (pstContents->lRepeatDate & eDxSCHED_DATE_TUE)
	{
		nx_scheduleRepeat_SetRepeatDate(pstContents, eNxUiScheduleRepeatItem_Tue);
	}

	if (pstContents->lRepeatDate & eDxSCHED_DATE_WED)
	{
		nx_scheduleRepeat_SetRepeatDate(pstContents, eNxUiScheduleRepeatItem_Wed);
	}

	if (pstContents->lRepeatDate & eDxSCHED_DATE_THU)
	{
		nx_scheduleRepeat_SetRepeatDate(pstContents, eNxUiScheduleRepeatItem_Thu);
	}

	if (pstContents->lRepeatDate & eDxSCHED_DATE_FRI)
	{
		nx_scheduleRepeat_SetRepeatDate(pstContents, eNxUiScheduleRepeatItem_Fri);
	}

	if (pstContents->lRepeatDate & eDxSCHED_DATE_SAT)
	{
		nx_scheduleRepeat_SetRepeatDate(pstContents, eNxUiScheduleRepeatItem_Sat);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleRepeat_InitData(NxUiScheduleRepeatContent_t *pstContents)
{
	HUINT32			i = 0;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	HxSTD_MemSet(&pstContents->stDayList, eNxUiScheduleRepeatItem_None, sizeof(NX_Util_OPTIONList_t));

	for (i = 0; i < NX_PRISM_SCHEDULE_REPEAT_DAY_MAX_ITEM; i++)
	{
		pstContents->abDayCheckList[i] = FALSE;
	}

	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add Other for Operator if you need

		default:
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stDayList, eNxUiScheduleRepeatItem_Sun);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stDayList, eNxUiScheduleRepeatItem_Mon);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stDayList, eNxUiScheduleRepeatItem_Tue);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stDayList, eNxUiScheduleRepeatItem_Wed);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stDayList, eNxUiScheduleRepeatItem_Thu);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stDayList, eNxUiScheduleRepeatItem_Fri);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stDayList, eNxUiScheduleRepeatItem_Sat);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleRepeat_MsgGwmCreate(NxUiScheduleRepeatContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SCHEDULE_REPEAT_WINDOW_X, NX_PRISM_SCHEDULE_REPEAT_WINDOW_Y,
							NX_PRISM_SCHEDULE_REPEAT_WINDOW_W, NX_PRISM_SCHEDULE_REPEAT_WINDOW_H);

	HxSTD_MemSet(pstContents, 0x00, sizeof(NxUiScheduleRepeatContent_t));

	eResult = nx_scheduleRepeat_InitData(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_FAIL;
	}

	pstContents->eRepeat = (DxRsvRepeat_e)p1;
	pstContents->lRepeatDate = p2;
	pstContents->eScheduleMode = (Nx_ScheduleMode_t)p3;
	nx_scheduleRepeat_LoadRepeatDate(pstContents);

	nx_scheduleRepeat_SetUiObject(pstContents);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleRepeat_MsgGwmDestroy(NxUiScheduleRepeatContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleRepeat_MsgGwmKeyDown(NxUiScheduleRepeatContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! pstContents == NULL!! \n");
		return ONDK_RET_FAIL;
	}

	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);

	switch (p1)
	{
		/* keys to exit */
		case KEY_BACK:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
		case KEY_MENU:
		case KEY_STANDBY:
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
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", p1);
			return	ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_scheduleRepeat_MsgGwmTimer(NxUiScheduleRepeatContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_scheduleRepeat_MsgGwmClickMsg(NxUiScheduleRepeatContent_t *pstContents, HINT32 p1)
{
	HINT32			lObjId = p1, lScheduleDate = eDxSCHED_DATE_NONE;
	DxRsvRepeat_e	eRsvRepeat = DxRSVREPEAT_ONCE;

	switch (lObjId)
	{
		case NX_PRISM_SCHEDULE_REPEAT_BTN_OK_ID:
			nx_scheduleRepeat_MakeRepeatDate(pstContents, &lScheduleDate, &eRsvRepeat);
			ONDK_GWM_SendMessage(NX_PRISM_SCHEDULE_Add_Proc, MSG_APP_SETTING_VALUE_CHANGED,	(HINT32)NULL, lScheduleDate, eRsvRepeat, 0);
			ONDK_GWM_APP_Destroy(0);
			break;
		case NX_PRISM_SCHEDULE_REPEAT_BTN_CANCEL_ID:
			ONDK_GWM_APP_Destroy(0);
			break;
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SUN_ID:
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_MON_ID:
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_TUE_ID:
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_WED_ID:
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_THU_ID:
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_FRI_ID:
		case NX_PRISM_SCHEDULE_REPEAT_WEEK_BTN_SAT_ID:
			nx_scheduleRepeat_ChangeCheckDay(pstContents, lObjId);
			break;
		default:
			NX_APP_Print("nx_scheduleRepeat_MsgGwmClickMsg() Invalid Obj ID : (0x%x)\n", lObjId);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_SCHEDULE_Repeat_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;
	NxUiScheduleRepeatContent_t *pstContents = nx_scheduleRepeat_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_scheduleRepeat_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			NX_APP_Message("MSG_GWM_KEYDOWN!\n");
			eResult = nx_scheduleRepeat_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_scheduleRepeat_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			break;
		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_scheduleRepeat_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_scheduleRepeat_MsgGwmClickMsg(pstContents, p1);
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return	eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

