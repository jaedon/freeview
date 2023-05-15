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
 * @file	  		nx_prism_schedule.c
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


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/

#define	NX_PRISM_SCHEDULE_ITEM_ID					eMenuFrame_Window_Schedule

#define	NX_PRISM_SCHEDULE_WINDOW_X					0
#define	NX_PRISM_SCHEDULE_WINDOW_Y					0
#define	NX_PRISM_SCHEDULE_WINDOW_W					NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_SCHEDULE_WINDOW_H					NX_PRISM_OSD_VERTICAL

#define	NX_PRISM_SCHEDULE_FRAME_ID					(NX_PRISM_SCHEDULE_ITEM_ID + 1)
#define	NX_PRISM_SCHEDULE_FRAME_X					0
#define	NX_PRISM_SCHEDULE_FRAME_Y					0
#define	NX_PRISM_SCHEDULE_FRAME_W					NX_PRISM_SCHEDULE_WINDOW_W
#define	NX_PRISM_SCHEDULE_FRAME_H					NX_PRISM_SCHEDULE_WINDOW_H

#define	NX_PRISM_SCHEDULE_TTL_TEXT_ID				(NX_PRISM_SCHEDULE_ITEM_ID + 2)
#define	NX_PRISM_SCHEDULE_TTL_TEXT_X				235
#define	NX_PRISM_SCHEDULE_TTL_TEXT_Y				(57 - NX_PRISM_FONT_VSPACE)
#define	NX_PRISM_SCHEDULE_TTL_TEXT_W				446
#define	NX_PRISM_SCHEDULE_TTL_TEXT_H				(NX_PRISM_FONT_SIZE_36 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_SCHEDULE_BTN_ADD_ID				(NX_PRISM_SCHEDULE_ITEM_ID + 3)
#define	NX_PRISM_SCHEDULE_BTN_ADD_X					699
#define	NX_PRISM_SCHEDULE_BTN_ADD_Y					45
#define	NX_PRISM_SCHEDULE_BTN_ADD_W					360
#define	NX_PRISM_SCHEDULE_BTN_ADD_H					45

#define	NX_PRISM_SCHEDULE_LINE_TOP_ID				(NX_PRISM_SCHEDULE_ITEM_ID + 4)
#define	NX_PRISM_SCHEDULE_LINE_TOP_X				235
#define	NX_PRISM_SCHEDULE_LINE_TOP_Y				97
#define	NX_PRISM_SCHEDULE_LINE_TOP_W				824
#define	NX_PRISM_SCHEDULE_LINE_TOP_H				1

#define	NX_PRISM_SCHEDULE_OPT_ID					(NX_PRISM_SCHEDULE_ITEM_ID + 5)
#define	NX_PRISM_SCHEDULE_OPT_X						820
#define	NX_PRISM_SCHEDULE_OPT_Y						674
#define	NX_PRISM_SCHEDULE_OPT_W						400
#define	NX_PRISM_SCHEDULE_OPT_H						30

// No Schedule
#define	NX_PRISM_SCHEDULE_NO_IMG_ID					(NX_PRISM_SCHEDULE_ITEM_ID + 6)
#define	NX_PRISM_SCHEDULE_NO_IMG_X					572
#define	NX_PRISM_SCHEDULE_NO_IMG_Y					250
#define	NX_PRISM_SCHEDULE_NO_IMG_W					136
#define	NX_PRISM_SCHEDULE_NO_IMG_H					123

#define	NX_PRISM_SCHEDULE_NO_TEXT_ID				(NX_PRISM_SCHEDULE_ITEM_ID + 7)
#define	NX_PRISM_SCHEDULE_NO_TEXT_X					284
#define	NX_PRISM_SCHEDULE_NO_TEXT_Y					(423 - NX_PRISM_FONT_SIZE_26)
#define	NX_PRISM_SCHEDULE_NO_TEXT_W					712
#define	NX_PRISM_SCHEDULE_NO_TEXT_H					(NX_PRISM_FONT_SIZE_26 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_SCHEDULE_NO_TEXT_MSG_ID			(NX_PRISM_SCHEDULE_ITEM_ID + 8)
#define	NX_PRISM_SCHEDULE_NO_TEXT_MSG_X				284
#define	NX_PRISM_SCHEDULE_NO_TEXT_MSG_Y				(483 - NX_PRISM_FONT_SIZE_24)
#define	NX_PRISM_SCHEDULE_NO_TEXT_MSG_W				712
#define	NX_PRISM_SCHEDULE_NO_TEXT_MSG_LS			30

// List Item
#define	NX_PRISM_SCHEDULE_LIST_ITEM_MAX				7

#define	NX_PRISM_SCHEDULE_LIST_TEXT_OFFSET_X		18
#define	NX_PRISM_SCHEDULE_LIST_TEXT_OFFSET_Y		(36 - NX_PRISM_FONT_SIZE_24)

#define	NX_PRISM_SCHEDULE_LIST_CELL_W				806
#define	NX_PRISM_SCHEDULE_LIST_CELL_H				81
#define	NX_PRISM_SCHEDULE_LIST_CELL_MG				4
#define	NX_PRISM_SCHEDULE_LIST_LS					(NX_PRISM_SCHEDULE_LIST_CELL_H + NX_PRISM_SCHEDULE_LIST_CELL_MG)

#define	NX_PRISM_SCHEDULE_LIST_SCROLL_MG			8
#define	NX_PRISM_SCHEDULE_LIST_SCROLL_W				3
#define	NX_PRISM_SCHEDULE_LIST_SCROLL_H				5

#define	NX_PRISM_SCHEDULE_LIST_SCROLL_BAR_X			1
#define	NX_PRISM_SCHEDULE_LIST_SCROLL_BAR_W			1

#define	NX_PRISM_SCHEDULE_LIST_ID					(NX_PRISM_SCHEDULE_ITEM_ID + 9)
#define	NX_PRISM_SCHEDULE_LIST_X					235
#define	NX_PRISM_SCHEDULE_LIST_Y					117
#define	NX_PRISM_SCHEDULE_LIST_W					(NX_PRISM_SCHEDULE_LIST_CELL_W + NX_PRISM_SCHEDULE_LIST_SCROLL_MG + NX_PRISM_SCHEDULE_LIST_SCROLL_W)
#define	NX_PRISM_SCHEDULE_LIST_H					(NX_PRISM_SCHEDULE_LIST_ITEM_MAX * NX_PRISM_SCHEDULE_LIST_CELL_H)

#define	NX_PRISM_SCHEDULE_BOTTOM_ID					(NX_PRISM_SCHEDULE_ITEM_ID + 10)
#define	NX_PRISM_SCHEDULE_BOTTOM_X					0
#define	NX_PRISM_SCHEDULE_BOTTOM_Y					630
#define	NX_PRISM_SCHEDULE_BOTTOM_W					1280
#define	NX_PRISM_SCHEDULE_BOTTOM_H					90

#define	NX_PRISM_SCHEDULE_LIST_ITEM_DATE_X			14
#define	NX_PRISM_SCHEDULE_LIST_ITEM_DATE_DAY_Y		15//17
#define	NX_PRISM_SCHEDULE_LIST_ITEM_DATE_MONTH_Y	47//49
#define	NX_PRISM_SCHEDULE_LIST_ITEM_DATE_W			94

#define	NX_PRISM_SCHEDULE_LIST_ITEM_LINE_X			120
#define	NX_PRISM_SCHEDULE_LIST_ITEM_LINE_Y			19
#define	NX_PRISM_SCHEDULE_LIST_ITEM_LINE_W			1
#define	NX_PRISM_SCHEDULE_LIST_ITEM_LINE_H			42

#define	NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_X		143
#define	NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_Y		47
#define	NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_W		28
#define	NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_H		28
#define	NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_MG		8

#define	NX_PRISM_SCHEDULE_LIST_ITEM_EVT_X			146
#define	NX_PRISM_SCHEDULE_LIST_ITEM_EVT_Y			14//16
#define	NX_PRISM_SCHEDULE_LIST_ITEM_EVT_W			642

#define	NX_PRISM_SCHEDULE_LIST_ITEM_SVC_X			143
#define	NX_PRISM_SCHEDULE_LIST_ITEM_SVC_Y			47//49
#define	NX_PRISM_SCHEDULE_LIST_ITEM_SVC_W			642

#define	NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_X		6
#define	NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_Y		21
#define	NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_W		38
#define	NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_H		38
#define	NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_MG	6

#define	NX_PRISM_SCHEDULE_LIST_SINGLE_SELECT_NUM	1


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	NxUiSchedule_DialogType_None = 0,
	NxUiSchedule_DialogType_Delete,
}  NxUiSchedule_DialogType_t;

typedef struct
{
	HBOOL		bChecked;
	HUINT32		ulSlot;
} NxUiScheduleSlotItem_t;

typedef struct
{
	HBOOL		bEditMode;

	HUINT32		ulSchduleNum;

	HUINT32		ulCurIdxOfList;
	HxVector_t	*pstScheduleSlotLists;
} NxUiScheduleContents_t;


/******************************************************************************/
#define _________Private_Value_________________________________________________
/******************************************************************************/
static NxUiScheduleContents_t	s_UiScheduleContents;
static NXUI_DialogInstance_t	s_stScheduleDialogMessageInstance;


/******************************************************************************/
#define _________Private_Prototype_____________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_schedule_GetDialogContents(void);
static ONDK_Result_t		nx_schedule_DialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t		nx_schedule_CreateSelectDialog(NxUiScheduleContents_t *pstContents, NxUiSchedule_DialogType_t eDialogType);
static ONDK_Result_t		nx_schedule_DestroySelectDialog(void);

static NxUiScheduleContents_t*	nx_schedule_GetContents(void);

static HUINT32 			nx_schedule_GetSelectedItemCount(NxUiScheduleContents_t *pstContents);

static ONDK_Result_t 	nx_schedule_ActionSelectAndDeSelect(NxUiScheduleContents_t *pstContents, eNxUiUtil_OptItem_t eCurOptItem, HINT32 lIndex);
static ONDK_Result_t 	nx_schedule_ActionDelete(NxUiScheduleContents_t *pstContents);

static ONDK_Result_t 	nx_schedule_ListItemAdd(NxUiScheduleContents_t *pstContents, HUINT32 ulSlot);
static ONDK_Result_t 	nx_schedule_ListItemDelete(NxUiScheduleContents_t *pstContents, HUINT32 ulSlot);

static ONDK_Result_t 	nx_schedule_FocusChange(NxUiScheduleContents_t *pstContents, HINT32 lkey);

static ONDK_Result_t	nx_schedule_DrawList_Cursor_C3(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
										ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

static ONDK_Result_t 	nx_schedule_DestroyUiNoScheduleObject(NxUiScheduleContents_t *pstContents);
static ONDK_Result_t 	nx_schedule_SetUiNoScheduleObject(NxUiScheduleContents_t *pstContents);
static ONDK_Result_t 	nx_schedule_SetUiObject(NxUiScheduleContents_t *pstContents);

static ONDK_Result_t	nx_schedule_LoadItems(NxUiScheduleContents_t *pstContents);
static ONDK_Result_t	nx_schedule_Initialize(NxUiScheduleContents_t *pstContents);

static ONDK_Result_t	nx_schedule_MsgGwmCreate(NxUiScheduleContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_schedule_MsgGwmDestroy(NxUiScheduleContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_schedule_MsgGwmHapiMsg(NxUiScheduleContents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_schedule_MsgGwmKeyDown(NxUiScheduleContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_schedule_MsgGwmClickMsg(NxUiScheduleContents_t *pstContents, HINT32 p1);
static ONDK_Result_t	nx_schedule_MsgGwmListSelected(NxUiScheduleContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_schedule_MsgCoreScheduleNotify(NxUiScheduleContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_schedule_MsgPopUpOptValueChange(NxUiScheduleContents_t *pstContents, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_schedule_MsgAppDlgClicked(NxUiScheduleContents_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_schedule_MsgAppDlgCanceled(NxUiScheduleContents_t *pstContents, HINT32 ulDlgWhoseId);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Select_Dialog_Functions___________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_schedule_GetDialogContents(void)
{
	return &s_stScheduleDialogMessageInstance;
}


static ONDK_Result_t	nx_schedule_DialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_schedule_GetDialogContents(), message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_schedule_CreateSelectDialog(NxUiScheduleContents_t *pstContents, NxUiSchedule_DialogType_t eDialogType)
{
	HINT8			*pszMsg = NULL;
	HUINT32 		ulDlgAttr = 0, ulSelectScheduleNum = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;


	pstDialog = nx_schedule_GetDialogContents();
	HxSTD_memset(pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	if (eDialogType == NxUiSchedule_DialogType_Delete)
	{
		ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
		ulSelectScheduleNum = nx_schedule_GetSelectedItemCount(pstContents);
		pszMsg = (ulSelectScheduleNum > NX_PRISM_SCHEDULE_LIST_SINGLE_SELECT_NUM) ? RES_MESG_4895_ID : RES_MESG_3224_ID;
	}
	else
	{
		NX_APP_Error("Error!!! Unknow Dialogue Type:[%d]\n", eDialogType);
		return	ONDK_RET_FAIL;
	}

	pstDialog->ulDlgWhoseId = eDialogType;
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)(HINT32)ONDK_GetString(pszMsg));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_schedule_DialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_schedule_DialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_DestroySelectDialog(void)
{
	ONDK_GWM_APP_Destroy(nx_schedule_DialogProc);

	return	ONDK_RET_OK;
}


#define _________Normal_Functions____________________________________________

static NxUiScheduleContents_t*	nx_schedule_GetContents(void)
{
	return &s_UiScheduleContents;
}


static HUINT32 	nx_schedule_GetSelectedItemCount(NxUiScheduleContents_t *pstContents)
{
	HUINT32		ulMaxSchedule = 0, i = 0, ulSelectScheduleNum = 0;
	NxUiScheduleSlotItem_t	*pstSchSlotItem = NULL;


	ulSelectScheduleNum = 0;
	if (pstContents->bEditMode == TRUE)
	{
		ulMaxSchedule = HLIB_VECTOR_Length(pstContents->pstScheduleSlotLists);
		for (i = 0; i < ulMaxSchedule; i++)
		{
			pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, i);
			if (pstSchSlotItem->bChecked == TRUE)
			{
				ulSelectScheduleNum++;
			}
		}

		if (ulSelectScheduleNum < NX_PRISM_SCHEDULE_LIST_SINGLE_SELECT_NUM)
		{
			ulSelectScheduleNum = NX_PRISM_SCHEDULE_LIST_SINGLE_SELECT_NUM;
		}
	}
	else
	{
		ulSelectScheduleNum = NX_PRISM_SCHEDULE_LIST_SINGLE_SELECT_NUM;
	}

	return	ulSelectScheduleNum;
}


static ONDK_Result_t 	nx_schedule_ActionSelectAndDeSelect(NxUiScheduleContents_t *pstContents, eNxUiUtil_OptItem_t eCurOptItem, HINT32 lIndex)
{
	HUINT32			i = 0, ulMaxSchedule = 0;
	NxUiScheduleSlotItem_t	*pstSchSlotItem = NULL;

	switch (eCurOptItem)
	{
		case eNxUtil_OptItem_Select:
			pstContents->bEditMode = TRUE;
			if (pstContents->pstScheduleSlotLists)
			{
				pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, lIndex);
				if (pstSchSlotItem)
				{
					pstSchSlotItem->bChecked = TRUE;
				}
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_SCHEDULE_LIST_ID);
			}
			break;

		case eNxUtil_OptItem_SelectAll:
			pstContents->bEditMode = TRUE;
			if (pstContents->pstScheduleSlotLists)
			{
				ulMaxSchedule = HLIB_VECTOR_Length(pstContents->pstScheduleSlotLists);
				for (i = 0; i < ulMaxSchedule; i++)
				{
					pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, i);
					if (pstSchSlotItem)
					{
						pstSchSlotItem->bChecked = TRUE;
					}
				}
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_SCHEDULE_LIST_ID);
			}
			break;

		case eNxUtil_OptItem_DeSelectAll:
			pstContents->bEditMode = FALSE;
			if (pstContents->pstScheduleSlotLists)
			{
				ulMaxSchedule = HLIB_VECTOR_Length(pstContents->pstScheduleSlotLists);
				for (i = 0; i < ulMaxSchedule; i++)
				{
					pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, i);
					if (pstSchSlotItem)
					{
						pstSchSlotItem->bChecked = FALSE;
					}
				}
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_SCHEDULE_LIST_ID);
			}
			break;

		default:
			NX_APP_Info("Not Support Option Item [%d] \n", eCurOptItem);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_schedule_ActionDelete(NxUiScheduleContents_t *pstContents)
{
	HERROR		hError = ERR_FAIL;
	HINT32		lListIdx = -1;
	HUINT32		ulSlotId = NX_CORE_SCHEDULE_SLOTID_NULL, ulMaxSchedule = 0, i = 0, ulDelScheduleNum = 0;
	NxUiScheduleSlotItem_t	*pstSchSlotItem = NULL, *pstDelSchSlotItem = NULL;


	if (pstContents->bEditMode == TRUE)
	{
		ulMaxSchedule = HLIB_VECTOR_Length(pstContents->pstScheduleSlotLists);
		pstDelSchSlotItem =  NX_APP_Calloc(sizeof(NxUiScheduleSlotItem_t) * ulMaxSchedule);
		if (pstDelSchSlotItem == NULL)
		{
			NX_APP_Error("Error!!! NX_APP_Calloc() Fail - ulMaxSchedule:(%u) !!\n", ulMaxSchedule);
			return	ONDK_RET_FAIL;
		}

		// Find Delete Schedule
		for (i = 0; i < ulMaxSchedule; i++)
		{
			pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, i);
			if (pstSchSlotItem->bChecked == TRUE)
			{
				pstDelSchSlotItem[ulDelScheduleNum].bChecked = FALSE;
				pstDelSchSlotItem[ulDelScheduleNum].ulSlot = NX_CORE_SCHEDULE_SLOTID_NULL;
				ulSlotId = pstSchSlotItem->ulSlot;

				// add Delete list
				pstDelSchSlotItem[ulDelScheduleNum].bChecked = TRUE;
				pstDelSchSlotItem[ulDelScheduleNum].ulSlot = ulSlotId;
				ulDelScheduleNum++;
			}
		}

		// Delete Schedule
		for (i = 0; i < ulDelScheduleNum; i++)
		{
			hError = NX_SCHEDULER_CancelSchedule(pstDelSchSlotItem[i].ulSlot);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_SCHEDULER_CancelSchedule() Fail - Slot:(0x%x) !!\n", ulSlotId);
			}
		}

		if (pstDelSchSlotItem)
		{
			NX_APP_Free(pstDelSchSlotItem);
			pstDelSchSlotItem = NULL;
		}
	}
	else
	{
		lListIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_SCHEDULE_LIST_ID);
		if (pstContents->pstScheduleSlotLists)
		{
			pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, lListIdx);
			ulSlotId = pstSchSlotItem->ulSlot;
			hError = NX_SCHEDULER_CancelSchedule(ulSlotId);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_SCHEDULER_CancelSchedule() Fail - Slot:(0x%x) !!\n", ulSlotId);
				return	ONDK_RET_FAIL;
			}
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_schedule_ListItemAdd(NxUiScheduleContents_t *pstContents, HUINT32 ulSlot)
{
	HERROR		hError = ERR_FAIL;
	DxSchedule_t *pstSchedule = NULL;
	NxUiScheduleSlotItem_t	*pstSchSlotItem = NULL;


	if (pstContents->pstScheduleSlotLists)
	{
		pstSchSlotItem = NX_APP_Calloc(sizeof(NxUiScheduleSlotItem_t));
		if (pstSchSlotItem)
		{
			pstSchedule = NX_APP_Calloc(sizeof(DxSchedule_t));
			if (pstSchedule)
			{
				hError = NX_SCHEDULER_GetScheduleBySlotID(ulSlot, pstSchedule);
				if (hError != ERR_OK)
				{
					NX_APP_Error("Error!!! NX_SCHEDULER_GetScheduleBySlotID() ulSlot:(0x%x) Fail!! \n", ulSlot);

					if (pstSchSlotItem)
					{
						NX_APP_Free(pstSchSlotItem);
						pstSchSlotItem = NULL;
					}
					if (pstSchedule)
					{
						NX_APP_Free(pstSchedule);
						pstSchedule = NULL;
					}

					return	ONDK_RET_FAIL;
				}
				else
				{
					switch (pstSchedule->eRsvType)
					{
						case DxRSVTYPE_WATCHING_TBR:
						case DxRSVTYPE_WATCHING_EBR:
						case DxRSVTYPE_WATCHING_SERIES_EBR:
						case DxRSVTYPE_RECORD_EBR:
						case DxRSVTYPE_RECORD_TBR:
						case DxRSVTYPE_RECORD_SERIES_EBR:
						case DxRSVTYPE_RECORD_SATRECORD_TBR:
							// Add Schedule List
							break;
						default:
							{
								NX_APP_Error("Error!!! Not Support Rsv type :(0x%x) Fail!! \n", pstSchedule->eRsvType);

								if (pstSchSlotItem)
								{
									NX_APP_Free(pstSchSlotItem);
									pstSchSlotItem = NULL;
								}
								if (pstSchedule)
								{
									NX_APP_Free(pstSchedule);
									pstSchedule = NULL;
								}
							}
							return	ONDK_RET_FAIL;
					}
				}
			}
			else
			{
				NX_APP_Error("Error!!! NX_APP_Calloc() Fail!! \n");
				NX_APP_Free(pstSchSlotItem);
				pstSchSlotItem = NULL;
				return	ONDK_RET_FAIL;
			}

			if (pstContents->ulSchduleNum < 1)
			{
				nx_schedule_DestroyUiNoScheduleObject(pstContents);
				ONDK_GWM_Obj_SetHide(NX_PRISM_SCHEDULE_LIST_ID, FALSE);
			}

			pstSchSlotItem->ulSlot = ulSlot;
			pstSchSlotItem->bChecked = FALSE;
			HLIB_VECTOR_Add(pstContents->pstScheduleSlotLists, (void*)pstSchSlotItem);
			pstContents->ulSchduleNum = HLIB_VECTOR_Length(pstContents->pstScheduleSlotLists);
			ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_SCHEDULE_LIST_ID), pstContents->ulSchduleNum, 0);

			if (pstContents->ulSchduleNum >= NX_PRISM_SCHEDULE_LIST_ITEM_MAX)
			{
				ONDK_GWM_Obj_SetHide(NX_PRISM_SCHEDULE_BOTTOM_ID, FALSE);
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_SCHEDULE_BOTTOM_ID);
			}
		}
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_SCHEDULE_LIST_ID);
	}


	if (pstSchedule)
	{
		NX_APP_Free(pstSchedule);
		pstSchedule = NULL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_schedule_ListItemDelete(NxUiScheduleContents_t *pstContents, HUINT32 ulSlot)
{
	HINT32			lListIdx = -1;
	HUINT32			ulFindSlot = NX_CORE_SCHEDULE_SLOTID_NULL;
	HUINT32			ulMaxSchedule = 0, i = 0;
	NxUiScheduleSlotItem_t	*pstSchSlotItem = NULL;


	if (pstContents->pstScheduleSlotLists)
	{
		pstSchSlotItem = NULL;
		lListIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_SCHEDULE_LIST_ID);
		ulMaxSchedule = HLIB_VECTOR_Length(pstContents->pstScheduleSlotLists);
		for (i = 0; i < ulMaxSchedule; i++)
		{
			pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, i);
			ulFindSlot = pstSchSlotItem->ulSlot;
			if (ulFindSlot == ulSlot)
			{
				HLIB_VECTOR_Remove(pstContents->pstScheduleSlotLists, i);
				pstContents->ulSchduleNum = HLIB_VECTOR_Length(pstContents->pstScheduleSlotLists);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_SCHEDULE_LIST_ID), pstContents->ulSchduleNum, 0);
				break;
			}
		}

		if (pstContents->ulSchduleNum < 1)
		{
			nx_schedule_SetUiNoScheduleObject(pstContents);
			ONDK_GWM_Obj_SetFocus(NX_PRISM_SCHEDULE_BTN_ADD_ID);
		}
		else
		{
			if (pstContents->ulSchduleNum < NX_PRISM_SCHEDULE_LIST_ITEM_MAX)
			{
				ONDK_GWM_Obj_SetHide(NX_PRISM_SCHEDULE_BOTTOM_ID, TRUE);
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_SCHEDULE_BOTTOM_ID);
			}

			if (lListIdx >= (HINT32)pstContents->ulSchduleNum)
			{
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_SCHEDULE_LIST_ID, (HINT32)(pstContents->ulSchduleNum - 1));
			}
			ONDK_GWM_APP_InvalidateObject(NX_PRISM_SCHEDULE_LIST_ID);
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_schedule_FocusChange(NxUiScheduleContents_t *pstContents, HINT32 lkey)
{
	HINT32		lObjId = -1, lListIdx = -1;

	lObjId = GWM_GetFocusedObjectID();
	if (lkey == KEY_ARROWUP)
	{
		if ((lObjId == NX_PRISM_SCHEDULE_LIST_ID) && (pstContents->ulSchduleNum > 0))
		{
			lListIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_SCHEDULE_LIST_ID);
			if (lListIdx == 0)
			{
				ONDK_GWM_Obj_SetHide(NX_PRISM_SCHEDULE_OPT_ID, TRUE);
				ONDK_GWM_Obj_SetFocus(NX_PRISM_SCHEDULE_BTN_ADD_ID);
				return	ONDK_MESSAGE_BREAK;
			}
		}
		else if ((lObjId == NX_PRISM_SCHEDULE_LIST_ID) && (pstContents->ulSchduleNum > 0))
		{
			return	ONDK_RET_OK;
		}
		else
		{
			return	ONDK_MESSAGE_BREAK;
		}
	}
	else if (lkey == KEY_ARROWDOWN)
	{
		if ((lObjId == NX_PRISM_SCHEDULE_BTN_ADD_ID) && (pstContents->ulSchduleNum > 0))
		{
			ONDK_GWM_Obj_SetHide(NX_PRISM_SCHEDULE_OPT_ID, FALSE);
			ONDK_GWM_APP_InvalidateObject(NX_PRISM_SCHEDULE_OPT_ID);
			ONDK_GWM_Obj_SetFocus(NX_PRISM_SCHEDULE_LIST_ID);
			return	ONDK_MESSAGE_BREAK;
		}
		else if ((lObjId == NX_PRISM_SCHEDULE_LIST_ID) && (pstContents->ulSchduleNum > 0))
		{
			return	ONDK_RET_OK;
		}
		else
		{
			return	ONDK_MESSAGE_BREAK;
		}
	}
	else
	{
		NX_APP_Error("Error!!! Invalid key:(0x%x) !!\n", lkey);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_DrawList_Cursor_C3(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
										ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HBOOL		bNeedScrollBar = FALSE;
	HERROR		hError = ERR_OK;
	HINT32		i = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0, x = 0, y = 0, w = 0, h = 0, lItem = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = NX_PRISM_SCHEDULE_LIST_ITEM_MAX;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0;
	HINT32		lImage_x_off = 0, lScroll_w_off = 0, lScroll_height = 0, lEventNameLen = 0, lCheckImage_x_off = 0;
	HUINT32		 ulSlotId = NX_CORE_SCHEDULE_SLOTID_NULL, ulStatus = GUIOBJ_NORMAL;
	HUINT8		*pDisable = NULL, szTextBuffer[NX_TEXT_SIZE_256];
	HCHAR		*pszImgStr = NULL;
	ONDK_Rect_t			stImgRect;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDK_FontId_t		eFontId = eFont_SystemBold;
	ONDKFontHandle		hFontHandle = NULL, *hOtherFontHandle = NULL;
	LIST_OBJECT			obj = (LIST_OBJECT)object;
	DxSchedule_t		*pstSchedule = NULL;
	DxService_t 		*stFindService = NULL;
	NX_Channel_t		stCurChannel;
	HxDATETIME_t 		stDurationDateTime;
	NxUiScheduleSlotItem_t	*pstSchSlotItem = NULL;
	NxUiScheduleContents_t	*pstContents = nx_schedule_GetContents();

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

	ulStatus = ONDK_GWM_List_GetStatus(obj);
	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	//lText_y_off = NX_PRISM_SCHEDULE_LIST_TEXT_OFFSET_Y;

	if (lNumOfPage < lTotal)
	{
		lLastIdx = lNumOfPage;
		bNeedScrollBar = TRUE;
	}
	else
	{
		lLastIdx = lTotal;
		bNeedScrollBar = FALSE;
	}

	lScroll_w_off = 0;
	if (bNeedScrollBar == TRUE)
	{
		lScroll_w_off = (NX_PRISM_SCHEDULE_LIST_SCROLL_MG + NX_PRISM_SCHEDULE_LIST_SCROLL_W);
		lScroll_height = h;
	}
	w = w - lScroll_w_off;

	pstSchedule = NX_APP_Calloc(sizeof(DxSchedule_t));
	if (pstSchedule == NULL)
	{
		NX_APP_Error("ERROR!!! NX_APP_Calloc() Fail !!\n");
		return	ONDK_RET_FAIL;
	}

	lCheckImage_x_off= 0;
	if (pstContents->bEditMode == TRUE)
	{
		lCheckImage_x_off = (NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_W + NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_MG);
	}

	for (i = 0; i < lLastIdx; i++)
	{
		lItem = (lStart + i);

		if (lItem >= (HINT32)pstContents->ulSchduleNum)
		{
			NX_APP_Error("ERROR!!! up to max schedule count !!\n");
			break;
		}

		pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, lItem);
		if (pstSchSlotItem == NULL)
		{
			NX_APP_Error("ERROR!!! pstSchSlotItem is NULL - lItem(%d) !!\n", lItem);
			continue;
		}

		ulSlotId = pstSchSlotItem->ulSlot;
		HxSTD_MemSet(pstSchedule, 0x00, sizeof(DxSchedule_t));
		hError = NX_SCHEDULER_GetScheduleBySlotID(ulSlotId, pstSchedule);
		if (hError != ERR_OK)
		{
			continue;
		}

		if (lItem == lCurrent)	/* focus */
		{
			x = x1;
			y = y1 + lCursor_y_off;
			eFontId = eFont_SystemBold;
			if ((ulStatus & GUIOBJ_HILIGHTED) || (ulStatus & GUIOBJ_PUSHED))
			{
				title_fontColor = COL(C_T_Bang_100);
				ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_SCHEDULE_LIST_CELL_MG, COL(C_G_Base01_100));
			}
			else
			{
				title_fontColor = COL(C_T_Whistle_100);
				ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_SCHEDULE_LIST_CELL_MG, COL(C_G_Base01_20));
			}
		}
		else
		{
			eFontId = eFont_SystemNormal;
			title_fontColor = COL(C_T_Whistle_100);
			if ((pDisable != NULL) && (pDisable[lItem] == 1))
			{
				title_fontColor = COL(C_T_Gong08_40);
			}
		}

		// Draw - Check Icon (Edit Mode)
		if (pstContents->bEditMode == TRUE)
		{
			stImgRect = ONDK_Rect(x1 + NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_X, y1 + lCursor_y_off + NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_Y,
									NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_W, NX_PRISM_SCHEDULE_LIST_ITEM_CHECK_IMG_H);
			pszImgStr = (pstSchSlotItem->bChecked == TRUE) ? RES_112_00_CHECK03_ON_PNG : RES_112_00_CHECK03_OFF_PNG ;
			ONDK_GUICOM_IMG_Blit(screen, stImgRect, pszImgStr);
		}
		pszImgStr = NULL;

		// Draw - Date - Day
		hOtherFontHandle = ONDK_FONT_CreateSystemFont(eFontId, NX_PRISM_FONT_SIZE_26);
		if (hOtherFontHandle)
		{
			x = x1 + NX_PRISM_SCHEDULE_LIST_ITEM_DATE_X + lCheckImage_x_off;
			y = y1 + lCursor_y_off + NX_PRISM_SCHEDULE_LIST_ITEM_DATE_DAY_Y;
			szTextBuffer[0] = '\0';
			HxSTD_sprintf((char*)szTextBuffer, "%02d", pstSchedule->stStartTime.stDate.ucDay);
			ONDK_FONT_DrawStringAlignAbbr(screen, hOtherFontHandle, (char*)szTextBuffer, x, y, NX_PRISM_SCHEDULE_LIST_ITEM_DATE_W, title_fontColor, ONDKSTF_CENTER);
			ONDK_FONT_Release(hOtherFontHandle);
		}

		// Draw - Devide Line
		stImgRect = ONDK_Rect(x1 + NX_PRISM_SCHEDULE_LIST_ITEM_LINE_X + lCheckImage_x_off, y1 + lCursor_y_off + NX_PRISM_SCHEDULE_LIST_ITEM_LINE_Y,
								NX_PRISM_SCHEDULE_LIST_ITEM_LINE_W, NX_PRISM_SCHEDULE_LIST_ITEM_LINE_H);
		ONDK_GUICOM_IMG_Blit(screen, stImgRect, RES_107_00_LINE_LTV_CH_IHUB_PNG);

		// Get Service Info
		stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_TV, eNxFindSvcDir_Current, pstSchedule->ulSvcUid);
		if (stFindService == NULL)
		{
			stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_RADIO, eNxFindSvcDir_Current, pstSchedule->ulSvcUid);
			if (stFindService == NULL)
			{
				NX_APP_Error("ERROR!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() - Fail svcUId:(0x%x)!!\n", pstSchedule->ulSvcUid);
				continue;
			}
			else
			{
				HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));
				NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eNxChListGroup_RADIO, pstSchedule->ulSvcUid, &stCurChannel);
			}
		}
		else
		{
			HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));
			NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eNxChListGroup_TV, pstSchedule->ulSvcUid, &stCurChannel);
		}

		// Event Info
		switch (pstSchedule->eRsvType)
		{
			case DxRSVTYPE_WATCHING_TBR:
			case DxRSVTYPE_WATCHING_EBR:
			case DxRSVTYPE_WATCHING_SERIES_EBR:
				szTextBuffer[0] = '\0';
				lEventNameLen = HxSTD_StrLen(pstSchedule->uExtInfo.stWch.szEvtName);
				if (lEventNameLen)
				{
					HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_256, "%s", pstSchedule->uExtInfo.stWch.szEvtName);
				}
				else
				{
					HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_256, "%s", stFindService->name);
				}
				pszImgStr = RES_205_00_RESERVATION_WATCH_PNG;
				break;
			case DxRSVTYPE_RECORD_EBR:
			case DxRSVTYPE_RECORD_TBR:
			case DxRSVTYPE_RECORD_SERIES_EBR:
			case DxRSVTYPE_RECORD_SATRECORD_TBR:
#if defined(CONFIG_MW_MM_PVR)
				szTextBuffer[0] = '\0';
				lEventNameLen = HxSTD_StrLen(pstSchedule->uExtInfo.stRec.szEventName);
				if (lEventNameLen)
				{
					HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_256, "%s", pstSchedule->uExtInfo.stRec.szEventName);
				}
				else
				{
					HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_256, "%s", stFindService->name);
				}
				pszImgStr = RES_205_04_REC_RESERVATION_RECORDING_PNG;
				switch (pstSchedule->eRepeat)
				{
					case DxRSVREPEAT_DAILY:
						break;
					case DxRSVREPEAT_WEEKLY:
					case DxRSVREPEAT_WEEKDAYS:
					case DxRSVREPEAT_WEEKEND:
					case DxRSVREPEAT_WITHOUT_SUNDAY:
					case DxRSVREPEAT_FEWDAY_OF_WEEK:
						pszImgStr = RES_205_04_REC_RESERVATION_WEEKLY_PNG;
						break;
					default:
						pszImgStr = RES_205_04_REC_RESERVATION_RECORDING_PNG;
						break;
				}
				if (pstSchedule->uExtInfo.stRec.ulSeriesId > 0)
				{
					pszImgStr = RES_205_04_REC_RESERVATION_SERIES_PNG;
				}
#endif
				break;

			case DxRSVTYPE_NOTHING:
			case DxRSVTYPE_POWER_ON:
			case DxRSVTYPE_POWER_OFF:
			case DxRSVTYPE_REGULAR_OTA:
			case DxRSVTYPE_EPG_GATHERING:
			case DxRSVTYPE_EMM:
			case DxRSVTYPE_UPDATE_TASK:
			case DxRSVTYPE_ASO_NOTIFY:
			case DxRSVTYPE_MYEPG_UPDATE:
			case DxRSVTYPE_NET_UPDATE:
			case DxRSVTYPE_BLURAY_TBR:
			case DxRSVTYPE_BLURAY_EBR:
			case DxRSVTYPE_RECORD_INSTANT:
			case DxRSVTYPE_RECORD_DELAYED:
			case DxRSVTYPE_KEYWORD_RECORD_NOTSURE:
			case DxRSVTYPE_CAS_POWER_CTRL:
			case DxRSVTYPE_LOGO_DOWN:
			case DxRSVTYPE_RESCAN:
			case DxRSVTYPE_CAS_TEL:
			case DxRSVTYPE_CHANNEL_MASK:
			case DxRSVTYPE_DUBBING:
			case DxRSVTYPE_OTA_BD:
			case DxRSVTYPE_IP_EPG_GATHERING:
			case DxRSVTYPE_IP_REMOTE_PROGRAMMING:
			case DxRSVTYPE_DLNA_EBR:
			case DxRSVTYPE_DLNA_TBR:
			case DxRSVTYPE_DLNA_DUBBING:
			case DxRSVTYPE_NETSTREAMING_LIVE:
			case DxRSVTYPE_NETSTREAMING_FILE:
			case DxRSVTYPE_ACTIVATION_TIME:
			case DxRSVTYPE_DESC_REC_STANDBY:
			case DxRSVTYPE_CIPLUS_OP_TUNE:
			case DxRSVTYPE_CHECK_SATRECORD_TIMER:
			case DxRSVTYPE_DYNAMIC_UPDATE:
			case DxRSVTYPE_EndOfCase:
			case DxRSVTYPE_TVTVEPG_TIMER:
				szTextBuffer[0] = '\0';
				break;
		}

		// Schedule Duration
		NX_SCHEDULER_GetDurationWithDataTime(pstSchedule, &stDurationDateTime);

		// Draw - Event Info
		hOtherFontHandle = ONDK_FONT_CreateSystemFont(eFontId, NX_PRISM_FONT_SIZE_24);
		if (hOtherFontHandle)
		{
			x = x1 + NX_PRISM_SCHEDULE_LIST_ITEM_EVT_X + lCheckImage_x_off;
			y = y1 + lCursor_y_off + NX_PRISM_SCHEDULE_LIST_ITEM_EVT_Y;
			ONDK_FONT_DrawStringAlignAbbr(screen, hOtherFontHandle, (char*)szTextBuffer, x, y, (NX_PRISM_SCHEDULE_LIST_ITEM_EVT_W - lCheckImage_x_off), title_fontColor, ONDKSTF_LEFT);
			ONDK_FONT_Release(hOtherFontHandle);
		}

		// Draw - Rsv Icon
		lImage_x_off = 0;
		if (pszImgStr)
		{
			stImgRect = ONDK_Rect((x1 + NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_X + lCheckImage_x_off), (y1 + lCursor_y_off + NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_Y),
									NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_W, NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_H);
			ONDK_GUICOM_IMG_Blit(screen, stImgRect, pszImgStr);
			lImage_x_off = (NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_W + NX_PRISM_SCHEDULE_LIST_ITEM_RSV_IMG_MG);
		}

		// Draw - Date - Month
		hOtherFontHandle = ONDK_FONT_CreateSystemFont(eFontId, NX_PRISM_FONT_SIZE_20);
		if (hOtherFontHandle)
		{
			x = x1 + NX_PRISM_SCHEDULE_LIST_ITEM_DATE_X + lCheckImage_x_off;
			y = y1 + lCursor_y_off + NX_PRISM_SCHEDULE_LIST_ITEM_DATE_MONTH_Y;
			szTextBuffer[0] = '\0';
			HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_256, "%s", ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByMonth(pstSchedule->stStartTime.stDate.ucMonth)));
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTextBuffer, x, y, NX_PRISM_SCHEDULE_LIST_ITEM_DATE_W, title_fontColor, ONDKSTF_CENTER);

			// Draw - Service Info + Schedule Time
			x = x1 + NX_PRISM_SCHEDULE_LIST_ITEM_SVC_X + lImage_x_off + lCheckImage_x_off;
			y = y1 + lCursor_y_off + NX_PRISM_SCHEDULE_LIST_ITEM_SVC_Y;
			szTextBuffer[0] = '\0';
			HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_256, "%04d %s / %02d:%02d-%02d:%02d",
							stCurChannel.lcn, stFindService->name, pstSchedule->stStartTime.stTime.ucHour, pstSchedule->stStartTime.stTime.ucMinute,
							stDurationDateTime.stTime.ucHour, stDurationDateTime.stTime.ucMinute);
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTextBuffer, x, y, (NX_PRISM_SCHEDULE_LIST_ITEM_SVC_W - lImage_x_off - lCheckImage_x_off), title_fontColor, ONDKSTF_LEFT);
			ONDK_FONT_Release(hOtherFontHandle);
		}

		lCursor_y_off += lCellHeight;
	}

	/* Scroll */
	if (bNeedScrollBar == TRUE)
	{
		HINT32	lScrollBall_step = 0, lHeight = 0;
		HINT32	lAllPage = 0, lScrollBall_height = 0, lScroll_y_offset = 0;

		lAllPage = (lTotal / lNumOfPage);
		lAllPage += (lTotal % lNumOfPage) ? 1 : 0;

		lScrollBall_height = lScroll_height / lAllPage;
		if (lScrollBall_height < NX_PRISM_SCHEDULE_LIST_SCROLL_H)
		{
			lScrollBall_height = NX_PRISM_SCHEDULE_LIST_SCROLL_H;
		}

		lHeight = (lScroll_height - lScrollBall_height);
		/* calculate vertical position */
		for (i = 1; lTotal > lHeight; i++)
		{
			lTotal /= i;
		} //while (total > (460 - lScrollBall_height));

		lScrollBall_step = lHeight / (lTotal - 1);
		lScroll_y_offset = (lCurrent / i) * lScrollBall_step;
		lScrollBall_height += (lHeight - ((lTotal - 1) * lScrollBall_step));

		x = x1 + w + NX_PRISM_SCHEDULE_LIST_SCROLL_MG;
		y = y1;

		/* Draw Scroll Bar */
		ONDK_DRAW_FillRectElement(screen, x + NX_PRISM_SCHEDULE_LIST_SCROLL_BAR_X, y, NX_PRISM_SCHEDULE_LIST_SCROLL_BAR_W, lScroll_height, COL(C_G_Whistle_20));

		/* Draw Scroll Bar Ball */
		stImgRect = ONDK_Rect(x, y + lScroll_y_offset, NX_PRISM_SCHEDULE_LIST_SCROLL_W, lScrollBall_height);
		ONDK_GUICOM_VBox(screen, stImgRect, RES_111_00_SCROLL_BAR_1U_PNG, RES_111_00_SCROLL_BAR_2M_PNG, RES_111_00_SCROLL_BAR_3D_PNG);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	if(pstSchedule)
	{
		NX_APP_Free(pstSchedule);
		pstSchedule = NULL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_schedule_DestroyUiNoScheduleObject(NxUiScheduleContents_t *pstContents)
{
	ONDK_GWM_Obj_Destroy(NX_PRISM_SCHEDULE_NO_IMG_ID);
	ONDK_GWM_Obj_Destroy(NX_PRISM_SCHEDULE_NO_TEXT_ID);
	ONDK_GWM_Obj_Destroy(NX_PRISM_SCHEDULE_NO_TEXT_MSG_ID);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_schedule_SetUiNoScheduleObject(NxUiScheduleContents_t *pstContents)
{
	HINT32				lLineCnt = 0, h = 0;
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	/* Draw No schedule image */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_NO_IMG_X, NX_PRISM_SCHEDULE_NO_IMG_Y,
							NX_PRISM_SCHEDULE_NO_IMG_W, NX_PRISM_SCHEDULE_NO_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_SCHEDULE_NO_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_SCHEDULE_NO_IMG_ID, RES_393_00_NOSCH_PNG);

	/* Draw No schedule text */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_NO_TEXT_X, NX_PRISM_SCHEDULE_NO_TEXT_Y,
						NX_PRISM_SCHEDULE_NO_TEXT_W, NX_PRISM_SCHEDULE_NO_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_NO_TEXT_ID, &stRect, (HCHAR*)ONDK_GetString(RES_NO_SCHEDULE_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_NO_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_NO_TEXT_ID, NX_PRISM_FONT_SIZE_26);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_NO_TEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_NO_TEXT_ID, COL(C_T_Gong03), COL(C_T_Bang_100));

	/* Draw No schedule msg */
	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_24,
										(HUINT8*)ONDK_GetString(RES_MESG_5978_ID), NX_PRISM_SCHEDULE_NO_TEXT_W);
	h = (NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_LS * lLineCnt) + NX_PRISM_FONT_VSPACE;
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_NO_TEXT_MSG_X, NX_PRISM_SCHEDULE_NO_TEXT_MSG_Y,
						NX_PRISM_SCHEDULE_NO_TEXT_MSG_W, h);
	ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_NO_TEXT_MSG_ID, &stRect, (HCHAR*)ONDK_GetString(RES_MESG_5978_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_NO_TEXT_MSG_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_NO_TEXT_MSG_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_NO_TEXT_MSG_ID, (TEXT_ALIGN_CENTER | TEXT_MULTILINED));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_NO_TEXT_MSG_ID, COL(C_T_Gong05), COL(C_T_Bang_100));

	ONDK_GWM_Obj_SetHide(NX_PRISM_SCHEDULE_LIST_ID, TRUE);
	ONDK_GWM_Obj_SetHide(NX_PRISM_SCHEDULE_BOTTOM_ID, TRUE);
	ONDK_GWM_Obj_SetHide(NX_PRISM_SCHEDULE_OPT_ID, TRUE);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_schedule_SetUiObject(NxUiScheduleContents_t *pstContents)
{
	HINT32				lList_h = 0;
	ONDK_Rect_t 		stRect;

	/* Draw BG Frame */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_FRAME_X, NX_PRISM_SCHEDULE_FRAME_Y,
						NX_PRISM_SCHEDULE_FRAME_W, NX_PRISM_SCHEDULE_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_SCHEDULE_FRAME_ID, &stRect, COL(C_G_Bang_80));

	/* Draw Title TEXT */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_TTL_TEXT_X, NX_PRISM_SCHEDULE_TTL_TEXT_Y,
						NX_PRISM_SCHEDULE_TTL_TEXT_W, NX_PRISM_SCHEDULE_TTL_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_SCHEDULE_TTL_TEXT_ID, &stRect, (HCHAR*)ONDK_GetString(RES_SCHEDULES_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_SCHEDULE_TTL_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_SCHEDULE_TTL_TEXT_ID, NX_PRISM_FONT_SIZE_36);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SCHEDULE_TTL_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SCHEDULE_TTL_TEXT_ID, COL(C_T_Gong01), COL(C_T_Bang_100));

	/* Draw Add a Schedule Button */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_BTN_ADD_X, NX_PRISM_SCHEDULE_BTN_ADD_Y,
						NX_PRISM_SCHEDULE_BTN_ADD_W, NX_PRISM_SCHEDULE_BTN_ADD_H);
	ONDK_GWM_COM_Button_Create(NX_PRISM_SCHEDULE_BTN_ADD_ID, &stRect, (HCHAR*)ONDK_GetString(RES_ADD_A_SCHEDULE_ID));
	ONDK_GWM_COM_Button_SetTitleSize(NX_PRISM_SCHEDULE_BTN_ADD_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_BTN_ADD_ID, NX_PRISM_COMP_DrawButton_B1);

	/* Draw TOP Line */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_LINE_TOP_X, NX_PRISM_SCHEDULE_LINE_TOP_Y,
						NX_PRISM_SCHEDULE_LINE_TOP_W, NX_PRISM_SCHEDULE_LINE_TOP_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_SCHEDULE_LINE_TOP_ID, &stRect, COL(C_G_Gong01));

	/* Create List */
	lList_h = (NX_PRISM_SCHEDULE_LIST_ITEM_MAX * NX_PRISM_SCHEDULE_LIST_CELL_H) + ((NX_PRISM_SCHEDULE_LIST_ITEM_MAX-1) * NX_PRISM_SCHEDULE_LIST_CELL_MG);
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_LIST_X, NX_PRISM_SCHEDULE_LIST_Y,
						NX_PRISM_SCHEDULE_LIST_W, lList_h);
	ONDK_GWM_List_Create(NX_PRISM_SCHEDULE_LIST_ID, &stRect, "NxSchduleList", NX_PRISM_SCHEDULE_LIST_LS,
							pstContents->ulSchduleNum, NX_PRISM_SCHEDULE_LIST_ITEM_MAX, 0);
	ONDK_GWM_List_SetFontHandle(NX_PRISM_SCHEDULE_LIST_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_List_SetCurrentIndex(NX_PRISM_SCHEDULE_LIST_ID, pstContents->ulCurIdxOfList);
	ONDK_GWM_List_SetSelectedIndex(NX_PRISM_SCHEDULE_LIST_ID, pstContents->ulCurIdxOfList);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_LIST_ID, (GWM_ObjDraw_t)nx_schedule_DrawList_Cursor_C3);

	/* Draw Bottom Image */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_BOTTOM_X, NX_PRISM_SCHEDULE_BOTTOM_Y,
						NX_PRISM_SCHEDULE_BOTTOM_W, NX_PRISM_SCHEDULE_BOTTOM_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_SCHEDULE_BOTTOM_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_BOTTOM_ID, NX_PRISM_COMP_DrawFrameBottomDim);

	/* Draw OPT Button */
	stRect = ONDK_Rect(NX_PRISM_SCHEDULE_OPT_X, NX_PRISM_SCHEDULE_OPT_Y,
						NX_PRISM_SCHEDULE_OPT_W, NX_PRISM_SCHEDULE_OPT_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_SCHEDULE_OPT_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SCHEDULE_OPT_ID, NX_PRISM_COMP_DrawEdit_HelpR);

	if (pstContents->ulSchduleNum < 1)
	{
		nx_schedule_SetUiNoScheduleObject(pstContents);
		ONDK_GWM_Obj_SetFocus(NX_PRISM_SCHEDULE_BTN_ADD_ID);
	}
	else
	{
		ONDK_GWM_Obj_SetFocus(NX_PRISM_SCHEDULE_LIST_ID);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_LoadItems(NxUiScheduleContents_t *pstContents)
{
	HINT32			i = 0, lSchNum = 0, lMaxSchNum = 0;
	DxSchedule_t 	*pstScheduleItem = NULL;
	HxVector_t		*pstScheduleList = NULL;
	NxUiScheduleSlotItem_t	*pstSchSlotItem = NULL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	ONDK_RET_INITFAIL;
	}

	pstContents->ulCurIdxOfList = 0;

	pstScheduleList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
	if (pstScheduleList == NULL)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_NewEx() Fail !!\n");
		return	ONDK_RET_INITFAIL;
	}

	NX_SCHEDULER_GetScheduleList(pstScheduleList, &lSchNum);
	lMaxSchNum = NX_CORE_SCHEDULE_SUPPORTED_SCHEDULE_NUM;
	lSchNum = (lSchNum > lMaxSchNum) ? lMaxSchNum : lSchNum ;
	for (i = 0; i < lSchNum; i++)
	{
		pstScheduleItem = (DxSchedule_t*)HLIB_VECTOR_ItemAt(pstScheduleList, i);
		if (pstScheduleItem)
		{
			if (pstContents->pstScheduleSlotLists)
			{
				pstSchSlotItem = NX_APP_Calloc(sizeof(NxUiScheduleSlotItem_t));
				if (pstSchSlotItem)
				{
					pstSchSlotItem->ulSlot = pstScheduleItem->ulSlot;
					pstSchSlotItem->bChecked = FALSE;
					HLIB_VECTOR_Add(pstContents->pstScheduleSlotLists, (void*)pstSchSlotItem);
				}
				else
				{
					NX_APP_Error("Error!!! NX_APP_Calloc() Fail!!\n");
				}
			}
			else
			{
				NX_APP_Error("Error!!! pstContents->pstScheduleSlotLists == NULL!!\n");
			}
		}
		else
		{
			NX_APP_Error("Error!!! HLIB_VECTOR_ItemAt() - index(%d) Fail !!\n", i);
		}
	}

	if (pstScheduleList)
	{
		HLIB_VECTOR_Delete(pstScheduleList);
		pstScheduleList = NULL;
	}

	if (pstContents->pstScheduleSlotLists)
	{
		pstContents->ulSchduleNum = HLIB_VECTOR_Length(pstContents->pstScheduleSlotLists);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_Initialize(NxUiScheduleContents_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	ONDK_RET_INITFAIL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(NxUiScheduleContents_t));

	pstContents->pstScheduleSlotLists = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
	if (pstContents->pstScheduleSlotLists == NULL)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_NewEx() Fail !!\n");
		return	ONDK_RET_INITFAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_MsgGwmCreate(NxUiScheduleContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SCHEDULE_WINDOW_X, NX_PRISM_SCHEDULE_WINDOW_Y,
							NX_PRISM_SCHEDULE_WINDOW_W, NX_PRISM_SCHEDULE_WINDOW_H);

	eResult = nx_schedule_Initialize(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}

	pstContents->bEditMode = FALSE;
	eResult = nx_schedule_LoadItems(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}

	nx_schedule_SetUiObject(pstContents);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_MsgGwmDestroy(NxUiScheduleContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->pstScheduleSlotLists)
	{
		HLIB_VECTOR_Delete(pstContents->pstScheduleSlotLists);
		pstContents->pstScheduleSlotLists = NULL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(NxUiScheduleContents_t));

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_MsgGwmHapiMsg(NxUiScheduleContents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_schedule_MsgGwmKeyDown(NxUiScheduleContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	ret = ONDK_RET_OK;
	HINT32			lObjId = -1;
	NX_UI_CallerType_t	eCallerType = eNX_UCT_None;

	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);

	switch (p1)
	{
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			ret = nx_schedule_FocusChange(pstContents, p1);
			return ret;

		case KEY_BACK:
			if (pstContents->bEditMode == TRUE)
			{
				nx_schedule_ActionSelectAndDeSelect(pstContents, eNxUtil_OptItem_DeSelectAll, -1);
			}
			else
			{
				ONDK_GWM_APP_Destroy(0);
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_OPT:
			lObjId = GWM_GetFocusedObjectID();
			if ((lObjId == NX_PRISM_SCHEDULE_LIST_ID) && (pstContents->ulSchduleNum > 0))
			{
				eCallerType = (pstContents->bEditMode == TRUE) ? eNX_UCT_Schedule_Edit : eNX_UCT_Schedule ;
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_SCHEDULE_Option_Proc", (APP_DEFAULT_PRIORITY | UISTATE_RECORDINGS),
									(ONDK_GWM_Callback_t)NX_PRISM_SCHEDULE_Option_Proc, 0, eCallerType, 0, 0);
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_OK:
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
			ret = ONDK_RET_OK;
			break;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_recordings_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", p1);
			ret = ONDK_RET_OK;
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return ret;
}


static ONDK_Result_t	nx_schedule_MsgGwmClickMsg(NxUiScheduleContents_t *pstContents, HINT32 p1)
{
	HINT32			lObjId = p1;

	switch (lObjId)
	{
		case NX_PRISM_SCHEDULE_BTN_ADD_ID:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_SCHEDULE_Add_Proc", (APP_DEFAULT_PRIORITY | UISTATE_SCHEDULE),
								(ONDK_GWM_Callback_t)NX_PRISM_SCHEDULE_Add_Proc, 0, NX_CORE_SCHEDULE_SLOTID_NULL, 0, 0);
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Error("Error!!! Invalid Object ID:(0x%x) \n", lObjId);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_MsgGwmListSelected(NxUiScheduleContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lListIdx = p2, lEventId = -1;
	HUINT32			ulSlotId = NX_CORE_SCHEDULE_SLOTID_NULL, ulSvcUid = 0;
	NxUiScheduleSlotItem_t	*pstSchSlotItem = NULL;

	if ((pstContents->bEditMode == TRUE) && pstContents->pstScheduleSlotLists)
	{
		pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, lListIdx);
		if (pstSchSlotItem)
		{
			pstSchSlotItem->bChecked = (pstSchSlotItem->bChecked == TRUE) ? FALSE : TRUE ;
			ONDK_GWM_APP_InvalidateObject(NX_PRISM_SCHEDULE_LIST_ID);
		}
	}
	else
	{
		pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, lListIdx);
		if (pstSchSlotItem)
		{
			ulSlotId = pstSchSlotItem->ulSlot;

			hError = NX_SCHEDULER_GetSeriveUidBySlotID(ulSlotId, &ulSvcUid);
			if (hError != ERR_OK)
			{
				NX_APP_Error("ERROR!!! NX_SCHEDULER_GetSeriveUidBySlotID() - Slot ID:(0x%x) Fail !!\n", ulSlotId);
				return	ONDK_RET_FAIL;
			}

			hError = NX_SCHEDULER_GetEventIDBySlotID(ulSlotId, &lEventId);
			if (hError != ERR_OK)
			{
				NX_APP_Error("ERROR!!! NX_SCHEDULER_GetEventIDBySlotID() - Slot ID:(0x%x) Fail !!\n", ulSlotId);
				return	ONDK_RET_FAIL;
			}

			if ((lEventId > 0) || (ulSvcUid > 0) )
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "Nx_PRISM_ProgramDetail_Proc", APP_DEFAULT_POPUP_PRIORITY, Nx_PRISM_ProgramDetail_Proc, 0, ulSlotId, lEventId, eNX_UCT_Schedule);
			}
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_MsgCoreScheduleNotify(NxUiScheduleContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			ulSlot = NX_CORE_SCHEDULE_SLOTID_NULL;
	APK_META_SCH_MsgType_e	eMsgType = eAPK_METASCH_MSG_None;


	eMsgType = (APK_META_SCH_MsgType_e)p1;
	switch(eMsgType)
	{
		case eAPK_METASCH_MSG_ScheduleAdded:
			ulSlot = (HUINT32)p2;
			if (pstContents->pstScheduleSlotLists)
			{
				nx_schedule_ListItemAdd(pstContents, ulSlot);
			}
			break;

		case eAPK_METASCH_MSG_ScheduleRemoved:
			ulSlot = (HUINT32)p2;
			if (pstContents->pstScheduleSlotLists)
			{
				nx_schedule_ListItemDelete(pstContents, ulSlot);
			}
			break;

		case eAPK_METASCH_MSG_ScheduleChanged:
			ulSlot = (HUINT32)p2;
			if (pstContents->ulSchduleNum > 0)
			{
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_SCHEDULE_LIST_ID);
			}
			break;

		case eAPK_METASCH_MSG_TimeUpStart:
		case eAPK_METASCH_MSG_TimeUpStop:
			ulSlot = (HUINT32)p3;
			if (pstContents->pstScheduleSlotLists)
			{
				nx_schedule_ListItemDelete(pstContents, ulSlot);
			}
			break;

		default:
			NX_APP_Debug("Invalid Message - eMsgType:(0x%x) \n", eMsgType);
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_MsgPopUpOptValueChange(NxUiScheduleContents_t *pstContents, HINT32 p2, HINT32 p3)
{
	HINT32			lListIdx = 0;
	eNxUiUtil_OptItem_t 	eCurOptItem = (eNxUiUtil_OptItem_t)p2;
	NxUiScheduleSlotItem_t	*pstSchSlotItem = NULL;


	switch (eCurOptItem)
	{
		case eNxUtil_OptItem_Delete:
			if (pstContents->pstScheduleSlotLists)
			{
				nx_schedule_CreateSelectDialog(pstContents, NxUiSchedule_DialogType_Delete);
			}
			break;

		case eNxUtil_OptItem_ReSchedule:
			lListIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_SCHEDULE_LIST_ID);
			if (pstContents->pstScheduleSlotLists)
			{
				pstSchSlotItem = (NxUiScheduleSlotItem_t*)HLIB_VECTOR_ItemAt(pstContents->pstScheduleSlotLists, lListIdx);
				if (pstSchSlotItem)
				{
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_SCHEDULE_Add_Proc", (APP_DEFAULT_PRIORITY | UISTATE_SCHEDULE),
										(ONDK_GWM_Callback_t)NX_PRISM_SCHEDULE_Add_Proc, 0, pstSchSlotItem->ulSlot, 0, 0);
				}
			}
			break;

		case eNxUtil_OptItem_Select:
			lListIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_SCHEDULE_LIST_ID);
			nx_schedule_ActionSelectAndDeSelect(pstContents, eCurOptItem, lListIdx);
			break;

		case eNxUtil_OptItem_SelectAll:
			if (pstContents->pstScheduleSlotLists)
			{
				lListIdx = -1;
				nx_schedule_ActionSelectAndDeSelect(pstContents, eCurOptItem, lListIdx);
			}
			break;

		case eNxUtil_OptItem_DeSelectAll:
			if (pstContents->pstScheduleSlotLists)
			{
				lListIdx = -1;
				nx_schedule_ActionSelectAndDeSelect(pstContents, eCurOptItem, lListIdx);
			}
			break;

		default:
			NX_APP_Info("Not Support Option Item [%d] \n", eCurOptItem);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_MsgAppDlgClicked(NxUiScheduleContents_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId)
{
	switch (lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:	// YES
			if (ulDlgWhoseId == NxUiSchedule_DialogType_Delete)
			{
				nx_schedule_DestroySelectDialog();
				nx_schedule_ActionDelete(pstContents);
			}
			else
			{
				NX_APP_Info("Unknow Dialogue ID:(0x%x)\n", ulDlgWhoseId);
			}
			break;

		case eNX_DLG_CLICKED_BTN_RIGHT:	// NO
			if (ulDlgWhoseId == NxUiSchedule_DialogType_Delete)
			{
				nx_schedule_DestroySelectDialog();
			}
			else
			{
				NX_APP_Info("Unknow Dialogue ID:(0x%x)\n", ulDlgWhoseId);
			}
			break;

		default:
			if (ulDlgWhoseId == NxUiSchedule_DialogType_Delete)
			{
				nx_schedule_DestroySelectDialog();
			}
			else
			{
				NX_APP_Info("Unknow Dialogue ID:(0x%x)\n", ulDlgWhoseId);
			}
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_schedule_MsgAppDlgCanceled(NxUiScheduleContents_t *pstContents, HINT32 ulDlgWhoseId)
{
	if (ulDlgWhoseId == NxUiSchedule_DialogType_Delete)
	{
		nx_schedule_DestroySelectDialog();
	}
	else
	{
		NX_APP_Info("Unknow Dialogue ID:(0x%x)\n", ulDlgWhoseId);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SCHEDULE_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	NX_UI_CallerType_t		eCurOptType = eNX_UCT_None;
	NxUiScheduleContents_t	*pstContents = nx_schedule_GetContents();

	switch (nMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE\n");
			eResult = nx_schedule_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			NX_APP_Message("MSG_GWM_KEYDOWN\n");
			eResult = nx_schedule_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY\n");
			eResult = nx_schedule_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE\n");
			eResult = nx_schedule_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_LIST_SELECTED:
			NX_APP_Message("MSG_GWM_LIST_SELECTED\n");
			eResult = nx_schedule_MsgGwmListSelected(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED\n");
			eResult = nx_schedule_MsgGwmClickMsg(pstContents, p1);
			break;
		case MSG_CORE_SCHEDULE_NOTIFY:
			NX_APP_Message("MSG_CORE_SCHEDULE_NOTIFY\n");
			eResult = nx_schedule_MsgCoreScheduleNotify(pstContents, p1, p2, p3);
			break;
		case MSG_APP_POPUP_OPT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_OPT_VALUE_CHANGED\n");
			eCurOptType = (NX_UI_CallerType_t)p1;
			if ((eCurOptType == eNX_UCT_Schedule) || (eCurOptType == eNX_UCT_Schedule_Edit))
			{
				eResult = nx_schedule_MsgPopUpOptValueChange(pstContents, p2, p3);
			}
			break;
		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_schedule_MsgAppDlgClicked(pstContents, p1, p2);
			break;
		case MSG_APP_DLG_CANCELED:
			NX_APP_Message("MSG_APP_DLG_CANCELED!\n");
			eResult = nx_schedule_MsgAppDlgCanceled(pstContents, p2);
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if ((p1 != eNX_UCT_Schedule) && (p1 != eNX_UCT_Record))
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

	return ONDK_GWM_ProcessMessageDefault(nMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

