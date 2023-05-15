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
 * @file	  		nx_prism_dialogue_msg.c
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
#include <nx_prism_menu_contents.h>
#include <nx_prism_menu_items.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_ITEM_ID						eMenuFrame_Window_Dialogue_RsvConflict	// Statue Message 250 ~ 299

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_X						290
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_Y						413
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_W						700
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_H						307

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_Y_MG					16
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_H_MG					32

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_FRAME_ID						(NX_PRISM_UI_DLG_RSV_CONFLICT_ITEM_ID + 1)
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_FRAME_X						0
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_FRAME_Y						0
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_FRAME_W						NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_W
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_FRAME_H						NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_H

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_ID						(NX_PRISM_UI_DLG_RSV_CONFLICT_ITEM_ID + 2)
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_X						30
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_Y						(46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_W						640
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_H						(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_TEXT_LS						32
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_TEXT_MULTI_LINE			3

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_ID							(NX_PRISM_UI_DLG_RSV_CONFLICT_ITEM_ID + 4)
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_X							30
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_Y							(81 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_W							640
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_H							(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_ID						(NX_PRISM_UI_DLG_RSV_CONFLICT_ITEM_ID + 5)
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_X							30
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_Y							162
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_W							640
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_H							55
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_CELL_MG					0
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_CURSOR_MG					5

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_TEXT_OFFSET_X				18
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_TEXT_OFFSET_Y				(36 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_TEXT_WIDTH				604
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_TEXT_HEIGHT				(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_REMINDER_MAX_ITEM			2

#define	NX_PRISM_UI_DLG_RSV_CONFLICT_EXPIRE_TIMER_ID				0x00000001
#define	NX_PRISM_UI_DLG_RSV_CONFLICT_EXPIRE_TIME					(30 * NX_COMMON_TIME_SECOND)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct
{
	HUINT32				ulSchSlot;

	HINT32				lTotalConflictNum;
	HUINT8				**szItemTextList;
	NX_ScheConflict_t	*pstConflictList;

	// Cancel Slot Info
	HUINT32				ulCancelSchSlot;

} NxUiDialogRsvConflict_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxUiDialogRsvConflict_t		s_stDialogRsvConflictContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NxUiDialogRsvConflict_t*	nx_rsvConflict_GetContents(void);

static ONDK_Result_t	nx_rsvConflict_ActionReminderAddItem(NxUiDialogRsvConflict_t *pstContents, HUINT32 ulSlotId);
static ONDK_Result_t	nx_rsvConflict_ActionReminderCancelItem(NxUiDialogRsvConflict_t *pstContents, HINT32 lIndex);
static ONDK_Result_t	nx_rsvConflict_MakeReminderConflictList(NxUiDialogRsvConflict_t *pstContents);
static DxService_t		*nx_rsvConflict_FindChannelBySvcUid(HUINT32 ulSvcUid, NX_Channel_t *retChannel);

static ONDK_Result_t 	nx_rsvConflict_DrawInfoBox(NxUiDialogRsvConflict_t *pstContents, HINT32 lAddLine);
static ONDK_Result_t	nx_rsvConflict_DrawDialogList_Cursor_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t 	nx_rsvConflict_DrawBasicUi(NxUiDialogRsvConflict_t *pstContents, HINT32 lAddLine, HINT32 lh, HINT8 *pszTitle, HINT8 *pszMsg);

static ONDK_Result_t 	nx_rsvConflict_SetUiObjectReminder(NxUiDialogRsvConflict_t *pstContents);
static ONDK_Result_t	nx_rsvConflict_LoadValue(NxUiDialogRsvConflict_t *pstContents);
static ONDK_Result_t	nx_rsvConflict_Initialize(NxUiDialogRsvConflict_t *pstContents, HINT32 p1);

static ONDK_Result_t	nx_rsvConflict_MsgGwmCreate(NxUiDialogRsvConflict_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_rsvConflict_MsgGwmDestroy(NxUiDialogRsvConflict_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_rsvConflict_MsgGwmHapiMsg(NxUiDialogRsvConflict_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_rsvConflict_MsgGwmKeyDown(NxUiDialogRsvConflict_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_rsvConflict_MsgGwmTimer(NxUiDialogRsvConflict_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_rsvConflict_MsgGwmClickMsg(NxUiDialogRsvConflict_t *pstContents, HINT32 p1);
static ONDK_Result_t	nx_rsvConflict_MsgGwmListSelect(NxUiDialogRsvConflict_t *pstContents, HINT32 lIndex);
static ONDK_Result_t	nx_rsvConflict_MsgCoreScheduleNotify(NxUiDialogRsvConflict_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Confirm_Message_functions___________________________________
/******************************************************************************/

static NxUiDialogRsvConflict_t*	nx_rsvConflict_GetContents(void)
{
	return &s_stDialogRsvConflictContents;
}


static ONDK_Result_t	nx_rsvConflict_ActionReminderAddItem(NxUiDialogRsvConflict_t *pstContents, HUINT32 ulSlotId)
{
	HERROR		hError = ERR_FAIL;

	if (pstContents->pstConflictList->pstTriedSchedule)
	{
		hError = NX_SCHEDULER_AddSchedule(pstContents->pstConflictList->pstTriedSchedule);
		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!!! NX_SCHEDULER_AddSchedule() Fail!!\n");
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_RET_FAIL;
		}
	}

	ONDK_GWM_APP_Destroy(0);

	NX_COMMON_NOT_USED_PARAM(ulSlotId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_ActionReminderCancelItem(NxUiDialogRsvConflict_t *pstContents, HINT32 lIndex)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lIndexConflict = 0;
	DxSchedule_t	*pstSchedule = NULL;


	// Cancel New Schedule
	if ((pstContents->pstConflictList->pstTriedSchedule) && (lIndex == 0))
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_OK;
	}

	// Cancel Prev Schedule
	if ((pstContents->pstConflictList->pstTriedSchedule) && (lIndex > 0))
	{
		lIndexConflict = (lIndex - 1);
	}
	else
	{
		lIndexConflict = lIndex;
	}

	if (lIndexConflict >= pstContents->pstConflictList->listNum)
	{
		NX_APP_Error("ERROR!!! Invalid Select Index Total:(%d), lIndexConflict:(%d) Fail !!\n", pstContents->pstConflictList->listNum, lIndexConflict);
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	pstSchedule = NULL;
	pstSchedule = (DxSchedule_t*)HLIB_VECTOR_ItemAt(pstContents->pstConflictList->conflictList, lIndexConflict);
	if (pstSchedule == NULL)
	{
		NX_APP_Error("ERROR!!! HLIB_VECTOR_ItemAt() Fail - lIndexConflict:(%d) !!\n", lIndexConflict);
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	hError = NX_SCHEDULER_CancelSchedule(pstSchedule->ulSlot);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_SCHEDULER_CancelSchedule() Fail - ulSlot:(0x%x) !!\n", pstSchedule->ulSlot);
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	pstContents->ulCancelSchSlot = pstSchedule->ulSlot;
#if 0
	if (pstContents->pstConflictList->pstTriedSchedule)
	{
		hError = NX_SCHEDULER_AddSchedule(pstContents->pstConflictList->pstTriedSchedule);
		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!!! NX_SCHEDULER_AddSchedule() Fail!!\n");
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_RET_FAIL;
		}
	}

	ONDK_GWM_APP_Destroy(0);
#endif

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_MakeReminderConflictList(NxUiDialogRsvConflict_t *pstContents)
{
	HUINT8			szTextBuffer[NX_TEXT_SIZE_64];
	HUINT8			**ppListOfItem = NULL;
	HINT32 			lIndex = 0, i = 0, lEventNameLen = 0;
	DxSchedule_t	*pstSchedule = NULL;
	DxService_t 	*stFindService = NULL;
	NX_Channel_t	stCurChannel;


	ppListOfItem = NX_PRISM_MENU_MenuItem_AllocTextList (pstContents->lTotalConflictNum, NX_TEXT_SIZE_128);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_MENU_MenuItem_AllocTextList() Fail !!\n");
		return	ONDK_RET_FAIL;
	}
	else
	{
		// Add Tried Schedule
		if (pstContents->pstConflictList->pstTriedSchedule)
		{
			HxSTD_memset(ppListOfItem[lIndex], 0x00, NX_TEXT_SIZE_128);
			ppListOfItem[lIndex][0] = '\0';

			HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_64);
			szTextBuffer[0] = '\0';

			HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));
			stFindService = nx_rsvConflict_FindChannelBySvcUid(pstContents->pstConflictList->pstTriedSchedule->ulSvcUid, &stCurChannel);

			switch (pstContents->pstConflictList->pstTriedSchedule->eRsvType)
			{
				case DxRSVTYPE_WATCHING_TBR:
				case DxRSVTYPE_WATCHING_EBR:
				case DxRSVTYPE_WATCHING_SERIES_EBR:
					lEventNameLen = HxSTD_StrLen(pstContents->pstConflictList->pstTriedSchedule->uExtInfo.stWch.szEvtName);
					if (lEventNameLen)
					{
						HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_64, "%s", pstContents->pstConflictList->pstTriedSchedule->uExtInfo.stWch.szEvtName);
					}
					else
					{
						if (stFindService != NULL)
						{
							HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_64, "%s", stFindService->name);
						}
						else
						{
							HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_64, "%s", (HUINT8*)ONDK_GetString(RES_UNKNOWN_ID));
						}
					}
					break;
				default:
					HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_64, "%s", (HUINT8*)ONDK_GetString(RES_UNKNOWN_ID));
					break;
			}
			HxSTD_snprintf ((char*)ppListOfItem[lIndex], NX_TEXT_SIZE_128, (char*)"%s : %s", (HUINT8*)ONDK_GetString(RES_CANCEL_NEW_REMINDER_ID), szTextBuffer);
			lIndex++;
		}

		// Conflict Schedule
		for (i = 0; i < pstContents->pstConflictList->listNum; i++)
		{
			HxSTD_memset(ppListOfItem[lIndex], 0x00, NX_TEXT_SIZE_128);
			ppListOfItem[lIndex][0] = '\0';
			pstSchedule = NULL;
			pstSchedule = (DxSchedule_t*)HLIB_VECTOR_ItemAt(pstContents->pstConflictList->conflictList, i);
			if (pstSchedule == NULL)
			{
				continue;
			}

			HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_64);
			szTextBuffer[0] = '\0';

			HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));
			stFindService = nx_rsvConflict_FindChannelBySvcUid(pstContents->pstConflictList->pstTriedSchedule->ulSvcUid, &stCurChannel);

			switch (pstSchedule->eRsvType)
			{
				case DxRSVTYPE_WATCHING_TBR:
				case DxRSVTYPE_WATCHING_EBR:
				case DxRSVTYPE_WATCHING_SERIES_EBR:
					lEventNameLen = HxSTD_StrLen(pstSchedule->uExtInfo.stWch.szEvtName);
					if (lEventNameLen)
					{
						HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_64, "%s", pstSchedule->uExtInfo.stWch.szEvtName);
					}
					else
					{
						if (stFindService != NULL)
						{
							HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_64, "%s", stFindService->name);
						}
						else
						{
							HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_64, "%s", (HUINT8*)ONDK_GetString(RES_UNKNOWN_ID));
						}
					}
					break;
				default:
					HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_64, "%s", (HUINT8*)ONDK_GetString(RES_UNKNOWN_ID));
					break;
			}

			HxSTD_snprintf ((char*)ppListOfItem[lIndex], NX_TEXT_SIZE_128, (char*)"%s : %s", (HUINT8*)ONDK_GetString(RES_CANCEL_03_ID), szTextBuffer);
			lIndex++;
		}

		pstContents->szItemTextList = ppListOfItem;
	}

	return	ONDK_RET_OK;
}


static DxService_t 	*nx_rsvConflict_FindChannelBySvcUid(HUINT32 ulSvcUid, NX_Channel_t *pstRetChannel)
{
	HERROR			hError = ERR_FAIL;
	DxService_t 	*stFindService = NULL;


	// Get Service Info
	stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_TV, eNxFindSvcDir_Current, ulSvcUid);
	if (stFindService == NULL)
	{
		stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_RADIO, eNxFindSvcDir_Current, ulSvcUid);
		if (stFindService == NULL)
		{
			NX_APP_Error("ERROR!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() - Fail svcUId:(0x%x)!!\n", ulSvcUid);
			return	NULL;
		}
		else
		{
			HxSTD_memset(pstRetChannel, 0x00, sizeof(NX_Channel_t));
			hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eNxChListGroup_RADIO, ulSvcUid, pstRetChannel);
			if (hError != ERR_OK)
			{
				NX_APP_Error("ERROR!!! NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() - Fail eNxChListGroup_RADIO - svcUId:(0x%x)!!\n", ulSvcUid);
				return	NULL;
			}
		}
	}
	else
	{
		HxSTD_memset(pstRetChannel, 0x00, sizeof(NX_Channel_t));
		hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eNxChListGroup_TV, ulSvcUid, pstRetChannel);
		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!!! NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() - Fail eNxChListGroup_TV - svcUId:(0x%x)!!\n", ulSvcUid);
			return	NULL;
		}
	}

	return	stFindService;
}


static ONDK_Result_t 	nx_rsvConflict_DrawInfoBox(NxUiDialogRsvConflict_t *pstContents, HINT32 lAddLine)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(lAddLine);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_DrawDialogList_Cursor_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lText_y_off = 0;
	HUINT32		ulItem_addr = 0;
	HUINT8		*szTitle = NULL, *pDisable = NULL;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDKFontHandle		hFontHandle;
	LIST_OBJECT	obj=(LIST_OBJECT)object;

	/* make coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* set properties */
	ulItem_addr = ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if ((ulItem_addr == 0) || (lTotal == 0))
	{
		NX_APP_Error("ERROR!!! (ulItem_addr == 0) || (lTotal == 0) !!\n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("ERROR!!! ONDK_GWM_List_GetFontHandleObj() was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));

	lText_y_off = NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_TEXT_OFFSET_Y;

	if (lNumOfPage < lTotal)
	{
		lLastIdx = lNumOfPage;
	}
	else
	{
		lLastIdx = lTotal;
	}

	for (i = 0; i < lLastIdx; i++)
	{
		if (i + lStart == lCurrent)	/* focus */
		{
			title_fontColor = COL(C_T_Bang_100);
			x = x1;
			y = y1 + lCursor_y_off;

			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_CURSOR_MG, COL(C_G_Base01_100));
		}
		else
		{
			title_fontColor = COL(C_T_Whistle_100);
			if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
			{
				title_fontColor = COL(C_T_Gong08_40);
			}
		}

		szTitle = (HUINT8 *)(((HUINT8 **)ulItem_addr)[lStart+i]);
		if (szTitle == NULL)
		{
			NX_APP_Error("ERROR!!! szTitle was NULL!!\n");
			return ONDK_RET_FAIL;
		}

		NX_APP_Info("[%d][%s]\n", lStart+i, szTitle);

		x = x1 + NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off + lText_y_off;

		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, w, title_fontColor, ONDKSTF_LEFT);
		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_rsvConflict_DrawBasicUi(NxUiDialogRsvConflict_t *pstContents, HINT32 lAddLine, HINT32 lh, HINT8 *pszTitle, HINT8 *pszMsg)
{
	HINT32				lMsgLineCnt = 0;
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };


	/* Draw Background */
	stRect = ONDK_Rect(0, 0, NX_PRISM_UI_DLG_RSV_CONFLICT_FRAME_W, lh);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_RSV_CONFLICT_FRAME_ID, &stRect, COL(C_G_Bang_90));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_RSV_CONFLICT_FRAME_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG_TopLine);

	/* Draw Title TEXT */
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_X, NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_Y,
						NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_W, NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_ID, &stRect, (HCHAR*)ONDK_GetString(pszTitle));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Draw Message */
	lMsgLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22,
						(HUINT8*)ONDK_GetString(pszMsg), NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_W);
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_X, NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_Y,
						NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_W, (NX_PRISM_UI_DLG_RSV_CONFLICT_TEXT_LS * lMsgLineCnt) + NX_PRISM_FONT_VSPACE);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_ID, &stRect, (HCHAR*)ONDK_GetString(pszMsg));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_ID, (TEXT_ALIGN_CENTER | TEXT_MULTILINED | TEXT_WORDWRAP));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_rsvConflict_SetUiObjectReminder(NxUiDialogRsvConflict_t *pstContents)
{
	HINT8				*pszTitle = NULL, *pszMsg = NULL;
	HINT32				lLineCnt = 0, lTitleLineCnt = 0, lMsgLineCnt = 0, lAddLine = 0, y = 0, h = 0;
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };


	pszTitle = RES_REMINDER_CONFLICT_01_ID;
	lTitleLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemBold, NX_PRISM_FONT_SIZE_22,
					(HUINT8*)ONDK_GetString(pszTitle), NX_PRISM_UI_DLG_RSV_CONFLICT_TITLE_W);

	pszMsg = STR_MESG_097_ID;
	lMsgLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22,
						(HUINT8*)ONDK_GetString(pszMsg), NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_W);
	lLineCnt = (lTitleLineCnt + lMsgLineCnt);

	if (lLineCnt <= NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_TEXT_MULTI_LINE)
	{
		h =  NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_H;
		ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_X, NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_Y,
									NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_W, h);
		lAddLine = 0;
	}
	else
	{
		lAddLine = (lLineCnt - NX_PRISM_UI_DLG_RSV_CONFLICT_MSG_TEXT_MULTI_LINE);

		y = NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_Y - (lAddLine * NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_Y_MG);
		y = (y < 0) ? 0 : y ;
		h = NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_H + (lAddLine * NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_H_MG);

		ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_X, y, NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_W, h);
	}

	nx_rsvConflict_DrawBasicUi(pstContents, lAddLine, h, pszTitle, pszMsg);

	/* Create List */
	y = NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_Y + (lAddLine * NX_PRISM_UI_DLG_RSV_CONFLICT_WINDOW_Y_MG);
	h = (pstContents->lTotalConflictNum * (NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_H + NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_CELL_MG));
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_X, y, NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_W, h);
	ONDK_GWM_List_Create(NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_ID, &stRect, "NxConflictList",
							(NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_H + NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_CELL_MG),
							pstContents->lTotalConflictNum, NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_REMINDER_MAX_ITEM, (HUINT32)pstContents->szItemTextList);
	ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_ID, 0);
	ONDK_GWM_List_SetFontHandle(NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_ID, (GWM_ObjDraw_t)nx_rsvConflict_DrawDialogList_Cursor_A1);

	ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_ID);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_LoadValue(NxUiDialogRsvConflict_t *pstContents)
{
	HINT32		lTriedSchNum = 0;


	pstContents->pstConflictList = NX_SCHEDULER_GetConflictList(pstContents->ulSchSlot);
	if (pstContents->pstConflictList == NULL)
	{
		NX_APP_Error("Error!!! NX_SCHEDULER_GetConflictList() == NULL !!\n");
		return	ONDK_RET_FAIL;
	}

	if (pstContents->pstConflictList->listNum < 1)
	{
		NX_APP_Error("Error!!! pstContents->pstConflictList->listNum:(%d) was Zero!!\n", pstContents->pstConflictList->listNum);
		return	ONDK_RET_FAIL;
	}

	lTriedSchNum = 0;
	if (pstContents->pstConflictList->pstTriedSchedule)
	{
		lTriedSchNum = 1;
	}

	pstContents->lTotalConflictNum = (pstContents->pstConflictList->listNum + lTriedSchNum);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_Initialize(NxUiDialogRsvConflict_t *pstContents, HINT32 p1)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	ONDK_RET_INITFAIL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(NxUiDialogRsvConflict_t));
	pstContents->szItemTextList = NULL;
	pstContents->ulSchSlot = (HUINT32)p1;
	NX_APP_Info("nx_rsvConflict_Initialize() - ulSchSlot:(0x%x)\n", p1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_MsgGwmCreate(NxUiDialogRsvConflict_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_FAIL;


	eResult = nx_rsvConflict_Initialize(pstContents, p1);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! nx_rsvConflict_Initialize() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	eResult = nx_rsvConflict_LoadValue(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! nx_rsvConflict_LoadValue() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	switch (pstContents->pstConflictList->eConflictType)
	{
		case eAPK_METASCH_CONFTYPE_View:
			eResult = nx_rsvConflict_MakeReminderConflictList(pstContents);
			if (eResult != ONDK_RET_OK)
			{
				NX_APP_Error(" Error!!! nx_rsvConflict_LoadValue() Fail !!\n");
				ONDK_GWM_APP_Destroy(0);
				return	ONDK_RET_FAIL;
			}
			nx_rsvConflict_SetUiObjectReminder(pstContents);
			break;

		default:
			NX_APP_Error(" Error!!! Invalid conflict Type:(%d) Fail !!\n", pstContents->pstConflictList->eConflictType);
			break;
	}

	ONDK_GWM_SetTimer(NX_PRISM_UI_DLG_RSV_CONFLICT_EXPIRE_TIMER_ID, NX_PRISM_UI_DLG_RSV_CONFLICT_EXPIRE_TIME);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_MsgGwmDestroy(NxUiDialogRsvConflict_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_UI_DLG_RSV_CONFLICT_EXPIRE_TIMER_ID);

	if (pstContents->szItemTextList)
	{
		NX_PRISM_MENU_MenuItem_FreeTextList(pstContents->lTotalConflictNum, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(NxUiDialogRsvConflict_t));

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_MsgGwmHapiMsg(NxUiDialogRsvConflict_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_rsvConflict_MsgGwmKeyDown(NxUiDialogRsvConflict_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
		case KEY_MENU:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_OK:
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
			break;

		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", p1);
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_MsgGwmTimer(NxUiDialogRsvConflict_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lObjId = -1, lListIdx = 0;


	if (p1 == NX_PRISM_UI_DLG_RSV_CONFLICT_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_KillTimer(NX_PRISM_UI_DLG_RSV_CONFLICT_EXPIRE_TIMER_ID);

		lObjId = GWM_GetFocusedObjectID();
		if (lObjId == NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_ID)
		{
			lListIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_ID);
			switch (pstContents->pstConflictList->eConflictType)
			{
				case eAPK_METASCH_CONFTYPE_View:
					nx_rsvConflict_ActionReminderCancelItem(pstContents, lListIdx);
					break;

				default:
					ONDK_GWM_APP_Destroy(0);
					NX_APP_Error(" Error!!! Invalid conflict Type:(%d) Fail !!\n", pstContents->pstConflictList->eConflictType);
					break;
			}
		}
		else
		{
			ONDK_GWM_APP_Destroy(0);
			NX_APP_Print("nx_rsvNoti_MsgGwmTimer() Invalid Object ID : (0x%x)\n", lObjId);
		}
	}
	else
	{
		ONDK_GWM_APP_Destroy(0);
		NX_APP_Print("nx_rsvNoti_MsgGwmTimer() Timer ID : (0x%x)\n", p1);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_MsgGwmClickMsg(NxUiDialogRsvConflict_t *pstContents, HINT32 p1)
{

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_MsgGwmListSelect(NxUiDialogRsvConflict_t *pstContents, HINT32 lIndex)
{
	switch (pstContents->pstConflictList->eConflictType)
	{
		case eAPK_METASCH_CONFTYPE_View:
			nx_rsvConflict_ActionReminderCancelItem(pstContents, lIndex);
			break;

		default:
			NX_APP_Error(" Error!!! Invalid conflict Type:(%d) Fail !!\n", pstContents->pstConflictList->eConflictType);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvConflict_MsgCoreScheduleNotify(NxUiDialogRsvConflict_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
			switch (pstContents->pstConflictList->eConflictType)
			{
				case eAPK_METASCH_CONFTYPE_View:
					if (ulSlot == pstContents->ulCancelSchSlot)
					{
						pstContents->ulCancelSchSlot = 0;
						nx_rsvConflict_ActionReminderAddItem(pstContents, ulSlot);
					}
					break;

				default:
					NX_APP_Error(" Error!!! Invalid conflict Type:(%d) Fail !!\n", pstContents->pstConflictList->eConflictType);
					break;
			}
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



	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}



/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_DIALOGUE_RsvConflict_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	NxUiDialogRsvConflict_t	*pstContents = nx_rsvConflict_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE\n");
			eResult = nx_rsvConflict_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			NX_APP_Message("MSG_GWM_KEYDOWN\n");
			eResult = nx_rsvConflict_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY\n");
			eResult = nx_rsvConflict_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE\n");
			eResult = nx_rsvConflict_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER\n");
			eResult = nx_rsvConflict_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED\n");
			eResult = nx_rsvConflict_MsgGwmClickMsg(pstContents, p1);
			break;
		case MSG_GWM_LIST_SELECTED:
			NX_APP_Message("MSG_GWM_LIST_SELECTED\n");
			/* item was selected, return index to parent application */
			if (NX_PRISM_UI_DLG_RSV_CONFLICT_LIST_ID == p1)
			{
				eResult = nx_rsvConflict_MsgGwmListSelect(pstContents, p2);
				return ONDK_MESSAGE_BREAK;
			}
			break;
		case MSG_CORE_SCHEDULE_NOTIFY:
			NX_APP_Message("MSG_CORE_SCHEDULE_NOTIFY\n");
			eResult = nx_rsvConflict_MsgCoreScheduleNotify(pstContents, p1, p2, p3);
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


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
