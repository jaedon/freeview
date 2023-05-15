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
 * @file	  		nx_prism_dialogue_rsv_noti.c
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
#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>

#include <nx_core.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/
#define	NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID						eMenuFrame_Window_Dialogue_Rsv	// Statue Message 200 ~ 249

#define	NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_X						290
#define	NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_Y						197
#define	NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_W						700
#define	NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_H						294

#define	NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_Y_MG					16
#define	NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_H_MG					32

#define	NX_PRISM_UI_DLG_RSV_NOTI_FRAME_ID						(NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID + 1)
#define	NX_PRISM_UI_DLG_RSV_NOTI_FRAME_X						0
#define	NX_PRISM_UI_DLG_RSV_NOTI_FRAME_Y						0
#define	NX_PRISM_UI_DLG_RSV_NOTI_FRAME_W						NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_W
#define	NX_PRISM_UI_DLG_RSV_NOTI_FRAME_H						NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_H

#define	NX_PRISM_UI_DLG_RSV_NOTI_TITLE_ID						(NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID + 2)
#define	NX_PRISM_UI_DLG_RSV_NOTI_TITLE_X						30
#define	NX_PRISM_UI_DLG_RSV_NOTI_TITLE_Y						(46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_RSV_NOTI_TITLE_W						640
#define	NX_PRISM_UI_DLG_RSV_NOTI_TITLE_H						(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_UI_DLG_RSV_NOTI_TEXT_LS						32
#define	NX_PRISM_UI_DLG_RSV_NOTI_MSG_TEXT_MULTI_LINE			2

#define	NX_PRISM_UI_DLG_RSV_NOTI_MSG_ID							(NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID + 3)
#define	NX_PRISM_UI_DLG_RSV_NOTI_MSG_X							30
#define	NX_PRISM_UI_DLG_RSV_NOTI_MSG_Y							(81 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_RSV_NOTI_MSG_W							640
#define	NX_PRISM_UI_DLG_RSV_NOTI_MSG_H							(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_UI_DLG_RSV_NOTI_BOX_ID							(NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID + 4)
#define	NX_PRISM_UI_DLG_RSV_NOTI_BOX_X							20
#define	NX_PRISM_UI_DLG_RSV_NOTI_BOX_Y							106
#define	NX_PRISM_UI_DLG_RSV_NOTI_BOX_W							660
#define	NX_PRISM_UI_DLG_RSV_NOTI_BOX_H							88

#define	NX_PRISM_UI_DLG_RSV_NOTI_TEXT_LS_H						32

#define	NX_PRISM_UI_DLG_RSV_NOTI_EVENT_ID						(NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID + 5)
#define	NX_PRISM_UI_DLG_RSV_NOTI_EVENT_X						18
#define	NX_PRISM_UI_DLG_RSV_NOTI_EVENT_Y						(36 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_UI_DLG_RSV_NOTI_EVENT_W						610
#define	NX_PRISM_UI_DLG_RSV_NOTI_EVENT_H						(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_UI_DLG_RSV_NOTI_SVC_ID							(NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID + 6)
#define	NX_PRISM_UI_DLG_RSV_NOTI_SVC_X							18
#define	NX_PRISM_UI_DLG_RSV_NOTI_SVC_Y							(NX_PRISM_UI_DLG_RSV_NOTI_EVENT_Y + NX_PRISM_UI_DLG_RSV_NOTI_TEXT_LS_H)
#define	NX_PRISM_UI_DLG_RSV_NOTI_SVC_W							430
#define	NX_PRISM_UI_DLG_RSV_NOTI_SVC_H							(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_UI_DLG_RSV_NOTI_TIME_ID						(NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID + 7)
#define	NX_PRISM_UI_DLG_RSV_NOTI_TIME_X							(NX_PRISM_UI_DLG_RSV_NOTI_SVC_X + NX_PRISM_UI_DLG_RSV_NOTI_SVC_W)
#define	NX_PRISM_UI_DLG_RSV_NOTI_TIME_Y							NX_PRISM_UI_DLG_RSV_NOTI_SVC_Y
#define	NX_PRISM_UI_DLG_RSV_NOTI_TIME_W							180
#define	NX_PRISM_UI_DLG_RSV_NOTI_TIME_H							(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_ID						(NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID + 8)
#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_X						123
#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_Y						214
#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_W						220
#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_H						50

#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_MG							15

#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_ID						(NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID + 9)
#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_X						(NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_X + NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_W + NX_PRISM_UI_DLG_RSV_NOTI_BTN_MG)
#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_Y						NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_Y
#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_W						220
#define	NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_H						50

#define	NX_PRISM_UI_DLG_RSV_NOTI_EXPIRE_TIMER_ID				(NX_PRISM_UI_DLG_RSV_NOTI_ITEM_ID + 10)
#define	NX_PRISM_UI_DLG_RSV_NOTI_EXPIRE_TIME					(30 * NX_COMMON_TIME_SECOND)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct
{
	HUINT32			ulSchSlot;
	DxSchedule_t	*pstSchedule;
	HUINT8			szEventText[NX_TEXT_SIZE_256];
	HUINT8			szSvcNameText[NX_TEXT_SIZE_128];
	HUINT8			szTimeText[NX_TEXT_SIZE_32];
} NxUiDialogRsvNoti_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxUiDialogRsvNoti_t		s_stDialogRsvNotiContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_rsvNoti_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_rsvNoti_CreateResponseDialouge(HUINT8* pszString);

static NxUiDialogRsvNoti_t*	nx_rsvNoti_GetContents(void);

static ONDK_Result_t	nx_rsvNoti_ActionYesItem(NxUiDialogRsvNoti_t *pstContents);
static ONDK_Result_t	nx_rsvNoti_ActionNoItem(NxUiDialogRsvNoti_t *pstContents);

static ONDK_Result_t 	nx_rsvNoti_DrawInfoBox(NxUiDialogRsvNoti_t *pstContents, HINT32 lAddLine);
static ONDK_Result_t 	nx_rsvNoti_SetUiObject(NxUiDialogRsvNoti_t *pstContents);

static ONDK_Result_t	nx_rsvNoti_LoadValue(NxUiDialogRsvNoti_t *pstContents);
static ONDK_Result_t	nx_rsvNoti_Initialize(NxUiDialogRsvNoti_t *pstContents, HINT32 p1);

static ONDK_Result_t	nx_rsvNoti_MsgGwmCreate(NxUiDialogRsvNoti_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_rsvNoti_MsgGwmDestroy(NxUiDialogRsvNoti_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_rsvNoti_MsgGwmHapiMsg(NxUiDialogRsvNoti_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_rsvNoti_MsgGwmKeyDown(NxUiDialogRsvNoti_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_rsvNoti_MsgGwmTimer(NxUiDialogRsvNoti_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_rsvNoti_MsgGwmClickMsg(NxUiDialogRsvNoti_t *pstContents, HINT32 p1);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Response_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_rsvNoti_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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


static void		nx_rsvNoti_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_rsvNoti_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_rsvNoti_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


/******************************************************************************/
#define __Local_Functions_____________________________________________________
/******************************************************************************/

static NxUiDialogRsvNoti_t*	nx_rsvNoti_GetContents(void)
{
	return &s_stDialogRsvNotiContents;
}


static ONDK_Result_t	nx_rsvNoti_ActionYesItem(NxUiDialogRsvNoti_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HBOOL 			bChangable = FALSE;
	HUINT32			ulViewId = 0;
	NX_Channel_t	stCurChannel;
	NX_ChListGroup_t	eChannelGroup = eNxChListGroup_TV;


	if (pstContents->pstSchedule == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstSchedule == NULL !!\n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));

	eChannelGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
	hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eChannelGroup, pstContents->pstSchedule->ulSvcUid, &stCurChannel);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() Fail - eChannelGroup:(0x%x), ulSvcUid:(0x%x)!!\n", eChannelGroup, pstContents->pstSchedule->ulSvcUid);
		eChannelGroup = eNxChListGroup_TV;
		hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eChannelGroup, pstContents->pstSchedule->ulSvcUid, &stCurChannel);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() Fail - eChannelGroup:(0x%x), ulSvcUid:(0x%x)!!\n", eNxChListGroup_TV, pstContents->pstSchedule->ulSvcUid);
			eChannelGroup = eNxChListGroup_RADIO;
			hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eChannelGroup, pstContents->pstSchedule->ulSvcUid, &stCurChannel);
			if (hError != ERR_OK)
			{
				return	ONDK_RET_FAIL;
			}
		}
	}

	hError = NX_MEDIA_PLAY_GetMainViewId(&ulViewId);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_MEDIA_PLAY_GetMainViewId() Fail !!\n");
		return	ONDK_RET_FAIL;
	}

	hError = NX_MEDIA_PLAY_CheckPlayChangable(ulViewId, pstContents->pstSchedule->ulSvcUid, &bChangable);
	if (hError == ERR_OK)
	{
		if (bChangable == TRUE)
		{
			NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstContents->pstSchedule->ulSvcUid, eChannelGroup, 0);
		}
		else
		{
			nx_rsvNoti_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_215_ID));
		}
	}
	else
	{
		NX_APP_Error("Error!!! NX_MEDIA_PLAY_CheckPlayChangable() Fail !!\n");
		return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvNoti_ActionNoItem(NxUiDialogRsvNoti_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	hError = NX_SCHEDULER_CancelSchedule(pstContents->ulSchSlot);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SCHEDULER_CancelSchedule() Fail - Slot:(0x%x) !!\n", pstContents->ulSchSlot);
	}

	ONDK_GWM_APP_Destroy(0);
	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_rsvNoti_DrawInfoBox(NxUiDialogRsvNoti_t *pstContents, HINT32 lAddLine)
{
	HINT32			y = 0, lEventNameLen = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 }, stTextRect = { 0, 0, 0, 0 };
	DxService_t 	*stFindService = NULL;
	NX_Channel_t	stCurChannel;
	HxDATETIME_t 	stDurationDateTime;


	HxSTD_MemSet(pstContents->szEventText, 0x00, NX_TEXT_SIZE_256);
	pstContents->szEventText[0] = '\0';
	HxSTD_MemSet(pstContents->szSvcNameText, 0x00, NX_TEXT_SIZE_128);
	pstContents->szSvcNameText[0] = '\0';
	HxSTD_MemSet(pstContents->szTimeText, 0x00, NX_TEXT_SIZE_32);
	pstContents->szTimeText[0] = '\0';

	/* Info Box */
	y = NX_PRISM_UI_DLG_RSV_NOTI_BOX_Y + (lAddLine * NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_Y_MG);
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_RSV_NOTI_BOX_X, y, NX_PRISM_UI_DLG_RSV_NOTI_BOX_W, NX_PRISM_UI_DLG_RSV_NOTI_BOX_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_RSV_NOTI_BOX_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_RSV_NOTI_BOX_ID, NX_PRISM_COMP_DrawFrameBox_A);

	// Get Service Info
	HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));
	stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_TV, eNxFindSvcDir_Current, pstContents->pstSchedule->ulSvcUid);
	if (stFindService == NULL)
	{
		stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_RADIO, eNxFindSvcDir_Current, pstContents->pstSchedule->ulSvcUid);
		if (stFindService == NULL)
		{
			NX_APP_Error("ERROR!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() - Fail svcUId:(0x%x)!!\n", pstContents->pstSchedule->ulSvcUid);
		}
		else
		{
			HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));
			NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eNxChListGroup_RADIO, pstContents->pstSchedule->ulSvcUid, &stCurChannel);
		}
	}
	else
	{
		HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));
		NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eNxChListGroup_TV, pstContents->pstSchedule->ulSvcUid, &stCurChannel);
	}

	// Event Info
	switch (pstContents->pstSchedule->eRsvType)
	{
		case DxRSVTYPE_WATCHING_TBR:
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_SERIES_EBR:
			pstContents->szEventText[0] = '\0';
			lEventNameLen = HxSTD_StrLen(pstContents->pstSchedule->uExtInfo.stWch.szEvtName);
			if (lEventNameLen)
			{
				HxSTD_snprintf((char*)pstContents->szEventText, NX_TEXT_SIZE_256, "%s", pstContents->pstSchedule->uExtInfo.stWch.szEvtName);
			}
			else
			{
				HxSTD_snprintf((char*)pstContents->szEventText, NX_TEXT_SIZE_256, "%s", stFindService->name);
			}
			break;
		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
#if defined(CONFIG_MW_MM_PVR)
			pstContents->szEventText[0] = '\0';
			lEventNameLen = HxSTD_StrLen(pstContents->pstSchedule->uExtInfo.stRec.szEventName);
			if (lEventNameLen)
			{
				HxSTD_snprintf((char*)pstContents->szEventText, NX_TEXT_SIZE_256, "%s", pstContents->pstSchedule->uExtInfo.stRec.szEventName);
			}
			else
			{
				HxSTD_snprintf((char*)pstContents->szEventText, NX_TEXT_SIZE_256, "%s", stFindService->name);
			}
#endif
			break;
		default:
			break;
	}

	/* Event Name */
	stTextRect = ONDK_Rect(stRect.x + NX_PRISM_UI_DLG_RSV_NOTI_EVENT_X, stRect.y + NX_PRISM_UI_DLG_RSV_NOTI_EVENT_Y,
							NX_PRISM_UI_DLG_RSV_NOTI_EVENT_W, NX_PRISM_UI_DLG_RSV_NOTI_EVENT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_RSV_NOTI_EVENT_ID, &stTextRect, (HCHAR*)pstContents->szEventText);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_RSV_NOTI_EVENT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_RSV_NOTI_EVENT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_RSV_NOTI_EVENT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_RSV_NOTI_EVENT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Svc Info */
	HxSTD_snprintf((char*)pstContents->szSvcNameText, NX_TEXT_SIZE_128, "%04d %s", stCurChannel.lcn, stFindService->name);
	stTextRect = ONDK_Rect(stRect.x + NX_PRISM_UI_DLG_RSV_NOTI_SVC_X, stRect.y + NX_PRISM_UI_DLG_RSV_NOTI_SVC_Y,
							NX_PRISM_UI_DLG_RSV_NOTI_SVC_W, NX_PRISM_UI_DLG_RSV_NOTI_SVC_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_RSV_NOTI_SVC_ID, &stTextRect, (HCHAR*)pstContents->szSvcNameText);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_RSV_NOTI_SVC_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_RSV_NOTI_SVC_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_RSV_NOTI_SVC_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_RSV_NOTI_SVC_ID, COL(C_T_Yoyo01), COL(C_T_Bang_100));

	/* Time Info */
	NX_SCHEDULER_GetDurationWithDataTime(pstContents->pstSchedule, &stDurationDateTime);
	HxSTD_snprintf((char*)pstContents->szTimeText, NX_TEXT_SIZE_32, "%02d:%02d - %02d:%02d",
					pstContents->pstSchedule->stStartTime.stTime.ucHour, pstContents->pstSchedule->stStartTime.stTime.ucMinute,
					stDurationDateTime.stTime.ucHour, stDurationDateTime.stTime.ucMinute);
	stTextRect = ONDK_Rect(stRect.x + NX_PRISM_UI_DLG_RSV_NOTI_TIME_X, stRect.y + NX_PRISM_UI_DLG_RSV_NOTI_TIME_Y,
							NX_PRISM_UI_DLG_RSV_NOTI_TIME_W, NX_PRISM_UI_DLG_RSV_NOTI_TIME_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_RSV_NOTI_TIME_ID, &stTextRect, (HCHAR*)pstContents->szTimeText);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_RSV_NOTI_TIME_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_RSV_NOTI_TIME_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_RSV_NOTI_TIME_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_RSV_NOTI_TIME_ID, COL(C_T_Yoyo01), COL(C_T_Bang_100));

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_rsvNoti_SetUiObject(NxUiDialogRsvNoti_t *pstContents)
{
	HINT32				lLineCnt = 0, lTitleLineCnt = 0, lMsgLineCnt = 0, lAddLine = 0, y = 0, h = 0;
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };


	lTitleLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22,
					(HUINT8*)ONDK_GetString(RES_MESG_160_ID), NX_PRISM_UI_DLG_RSV_NOTI_TITLE_W);
	lMsgLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22,
						(HUINT8*)ONDK_GetString(RES_MESG_3385_ID), NX_PRISM_UI_DLG_RSV_NOTI_MSG_W);
	lLineCnt = (lTitleLineCnt + lMsgLineCnt);

	if (lLineCnt <= NX_PRISM_UI_DLG_RSV_NOTI_MSG_TEXT_MULTI_LINE)
	{
		h =  NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_H;
		ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_X, NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_Y,
									NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_W, h);
		lAddLine = 0;
	}
	else
	{
		lAddLine = (lLineCnt - NX_PRISM_UI_DLG_RSV_NOTI_MSG_TEXT_MULTI_LINE);

		y = NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_Y - (lAddLine * NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_Y_MG);
		y = (y < 0) ? 0 : y ;
		h = NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_H + (lAddLine * NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_H_MG);

		ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_X, y, NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_W, h);
	}

	/* background */
	stRect = ONDK_Rect(0, 0, NX_PRISM_UI_DLG_RSV_NOTI_FRAME_W, h);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_RSV_NOTI_FRAME_ID, &stRect, COL(C_G_Bang_90));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_RSV_NOTI_FRAME_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG_TBLine);

	/* Draw Title TEXT */
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_RSV_NOTI_TITLE_X, NX_PRISM_UI_DLG_RSV_NOTI_TITLE_Y,
						NX_PRISM_UI_DLG_RSV_NOTI_TITLE_W, (NX_PRISM_UI_DLG_RSV_NOTI_TEXT_LS * lTitleLineCnt) + NX_PRISM_FONT_VSPACE);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_RSV_NOTI_TITLE_ID, &stRect, (HCHAR*)ONDK_GetString(RES_MESG_160_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_RSV_NOTI_TITLE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_RSV_NOTI_TITLE_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_RSV_NOTI_TITLE_ID, (TEXT_ALIGN_CENTER | TEXT_MULTILINED));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_RSV_NOTI_TITLE_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Message */
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_RSV_NOTI_MSG_X, NX_PRISM_UI_DLG_RSV_NOTI_MSG_Y,
						NX_PRISM_UI_DLG_RSV_NOTI_MSG_W, (NX_PRISM_UI_DLG_RSV_NOTI_TEXT_LS * lMsgLineCnt) + NX_PRISM_FONT_VSPACE);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_RSV_NOTI_MSG_ID, &stRect, (HCHAR*)ONDK_GetString(RES_MESG_3385_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_RSV_NOTI_MSG_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_RSV_NOTI_MSG_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_RSV_NOTI_MSG_ID, (TEXT_ALIGN_CENTER | TEXT_MULTILINED));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_RSV_NOTI_MSG_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Info Box */
	nx_rsvNoti_DrawInfoBox(pstContents, lAddLine);

	/* Draw Button Item */
	y = NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_Y + (lAddLine * NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_Y_MG);
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_X, y, NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_W, NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_H);
	ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_ID, &stRect, (HCHAR*)ONDK_GetString(RES_YES_01_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_ID, NX_PRISM_COMP_DrawButton_A1);

	y = NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_Y + (lAddLine * NX_PRISM_UI_DLG_RSV_NOTI_WINDOW_Y_MG);
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_X, y, NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_W, NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_H);
	ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_ID, &stRect, (HCHAR*)ONDK_GetString(RES_NO_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_ID, NX_PRISM_COMP_DrawButton_A1);

	// Set Focus
	ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_ID);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvNoti_LoadValue(NxUiDialogRsvNoti_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	pstContents->pstSchedule = NX_APP_Calloc(sizeof(DxSchedule_t));
	if (pstContents->pstSchedule == NULL)
	{
		NX_APP_Error("ERROR!!! NX_APP_Calloc() Fail !!\n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(pstContents->pstSchedule, 0x00, sizeof(DxSchedule_t));

	hError = NX_SCHEDULER_GetScheduleBySlotID(pstContents->ulSchSlot, pstContents->pstSchedule);
	if (hError != ERR_OK)
	{
		if (pstContents->pstSchedule)
		{
			NX_APP_Free(pstContents->pstSchedule);
			pstContents->pstSchedule = NULL;
		}
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvNoti_Initialize(NxUiDialogRsvNoti_t *pstContents, HINT32 p1)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	ONDK_RET_INITFAIL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(NxUiDialogRsvNoti_t));

	pstContents->ulSchSlot = (HUINT32)p1;

	NX_APP_Info("nx_rsvNoti_Initialize() - ulSchSlot:(0x%x)\n", p1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvNoti_MsgGwmCreate(NxUiDialogRsvNoti_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	eResult = nx_rsvNoti_Initialize(pstContents, p1);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! nx_rsvNoti_Initialize() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	eResult = nx_rsvNoti_LoadValue(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! nx_rsvNoti_LoadValue() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	eResult = nx_rsvNoti_SetUiObject(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! nx_rsvNoti_SetUiObject() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_SetTimer(NX_PRISM_UI_DLG_RSV_NOTI_EXPIRE_TIMER_ID, NX_PRISM_UI_DLG_RSV_NOTI_EXPIRE_TIME);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvNoti_MsgGwmDestroy(NxUiDialogRsvNoti_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_UI_DLG_RSV_NOTI_EXPIRE_TIMER_ID);

	if (pstContents->pstSchedule)
	{
		NX_APP_Free(pstContents->pstSchedule);
		pstContents->pstSchedule = NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvNoti_MsgGwmHapiMsg(NxUiDialogRsvNoti_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_rsvNoti_MsgGwmKeyDown(NxUiDialogRsvNoti_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
		case KEY_STANDBY:
		case KEY_GUIDE:
		case KEY_MENU:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_OK:
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", p1);
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvNoti_MsgGwmTimer(NxUiDialogRsvNoti_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lObjId = -1;


	if (p1 == NX_PRISM_UI_DLG_RSV_NOTI_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_KillTimer(NX_PRISM_UI_DLG_RSV_NOTI_EXPIRE_TIMER_ID);

		lObjId = GWM_GetFocusedObjectID();
		if (lObjId == NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_ID)
		{
			nx_rsvNoti_ActionYesItem(pstContents);
			ONDK_GWM_APP_Destroy(0);
		}
		else
		{
			nx_rsvNoti_ActionNoItem(pstContents);
		}
	}
	else
	{
		NX_APP_Print("nx_rsvNoti_MsgGwmTimer() Timer ID : (0x%x)\n", p1);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_rsvNoti_MsgGwmClickMsg(NxUiDialogRsvNoti_t *pstContents, HINT32 p1)
{
	HINT32			lObjId = p1;

	switch (lObjId)
	{
		case NX_PRISM_UI_DLG_RSV_NOTI_BTN_YES_ID:
			nx_rsvNoti_ActionYesItem(pstContents);
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case NX_PRISM_UI_DLG_RSV_NOTI_BTN_NO_ID:
			nx_rsvNoti_ActionNoItem(pstContents);
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Error("Error!!! Invalid Object ID:(0x%x) \n", lObjId);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_DIALOGUE_RsvNoti_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	NxUiDialogRsvNoti_t	*pstContents = nx_rsvNoti_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE\n");
			eResult = nx_rsvNoti_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			NX_APP_Message("MSG_GWM_KEYDOWN\n");
			eResult = nx_rsvNoti_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY\n");
			eResult = nx_rsvNoti_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE\n");
			eResult = nx_rsvNoti_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER\n");
			eResult = nx_rsvNoti_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED\n");
			eResult = nx_rsvNoti_MsgGwmClickMsg(pstContents, p1);
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

