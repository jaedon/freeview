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
 * @file	  		nx_prism_channel_list.c
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
#include <nx_prism_channellist.h>

#include "nx_prism_definition.h"

#include <nx_core_metadata_pf.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define NX_PRISM_CHLIST_EVENT_DATE_TEXT_BUFFER		16
#define NX_PRISM_CHLIST_ITEM_MAX					10
#define NX_PRISM_CHLIST_ITEM_FOCUS_INDEX			2	// 3th

// channel list
#define	NX_PRISM_CHLIST_POS_X						0
#define	NX_PRISM_CHLIST_POS_Y						(62 - NX_PRISM_FONT_SIZE_26)

#define	NX_PRISM_CHLIST_ITEM_ID						eMenuFrame_Window_CHList

// timer
#define	NX_PRISM_CHLIST_PROGRAMME_TIMER_ID			(NX_PRISM_CHLIST_ITEM_ID + 1)
#define	NX_PRISM_CHLIST_PROGRAMME_EXPIRE_TIME		(1 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_CHLIST_EXPIRE_TIMER_ID				(NX_PRISM_CHLIST_ITEM_ID + 2)
#define	NX_PRISM_CHLIST_EXPIRE_TIME					(30 * NX_COMMON_TIME_SECOND)

// area
#define	NX_PRISM_CHLIST_FRAME_X						0
#define	NX_PRISM_CHLIST_FRAME_Y						0
#define	NX_PRISM_CHLIST_FRAME_W						900
#define	NX_PRISM_CHLIST_FRAME_H						NX_PRISM_OSD_VERTICAL

#define	NX_PRISM_CHLIST_ID							(NX_PRISM_CHLIST_ITEM_ID + 3)

#define	NX_PRISM_CHLIST_BG_ID						(NX_PRISM_CHLIST_ITEM_ID + 4)
#define	NX_PRISM_CHLIST_BG_X						NX_PRISM_CHLIST_FRAME_X
#define	NX_PRISM_CHLIST_BG_Y						NX_PRISM_CHLIST_FRAME_Y
#define	NX_PRISM_CHLIST_BG_W						NX_PRISM_CHLIST_FRAME_W
#define	NX_PRISM_CHLIST_BG_H						NX_PRISM_CHLIST_FRAME_H

#define	NX_PRISM_CHLIST_CHBOX_LINE_ID				(NX_PRISM_CHLIST_ITEM_ID + 5)
#define	NX_PRISM_CHLIST_CHBOX_LINE_X				(NX_PRISM_CHLIST_FRAME_X + 157)
#define	NX_PRISM_CHLIST_CHBOX_LINE_Y				(132 + 14)
#define	NX_PRISM_CHLIST_CHBOX_LINE_W				1
#define	NX_PRISM_CHLIST_CHBOX_LINE_H				42

#define	NX_PRISM_CHLIST_LIST_LINE_ID				(NX_PRISM_CHLIST_ITEM_ID + 6)
#define	NX_PRISM_CHLIST_LIST_LINE_X					(NX_PRISM_CHLIST_FRAME_X + 60)
#define	NX_PRISM_CHLIST_LIST_LINE_Y					632
#define	NX_PRISM_CHLIST_LIST_LINE_W					405
#define	NX_PRISM_CHLIST_LIST_LINE_H					1

#define	NX_PRISM_CHLIST_OPTION_TEXT_ID				(NX_PRISM_CHLIST_ITEM_ID + 7)
#define	NX_PRISM_CHLIST_OPTION_TEXT_X				60
#define	NX_PRISM_CHLIST_OPTION_TEXT_Y				653
#define	NX_PRISM_CHLIST_OPTION_TEXT_W				400
#define	NX_PRISM_CHLIST_OPTION_TEXT_H				28
#define	NX_PRISM_CHLIST_OPTION_IMG_MG				10


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct
{
	HINT32		lcn;
	HCHAR		*pszChName;
	HCHAR		pszEventName[EPG_HUMAX_NAME_LEN];
	HCHAR		pszEventDate[NX_TEXT_SIZE_64];
} NxChListData_t;


typedef struct
{
	HINT32				nChannelNum;
	HINT32				nCurItemIndex;					// channel list 에서 리스트의 channel 목록 유지 및 처리를 위해.
	HCHAR				szOptionText[NX_TEXT_SIZE_64];
	HCHAR				szRenameText[DxNAME_LEN];

	NX_Channel_t 		*pstChannels;					// live proc 에서 얻어옴..
	NxChListData_t		*pstChListData;

	HBOOL				bChangedGroup;					// User flag Set Change Group
	NX_ChListGroup_t	eChangedGroup;					// Group Item

	eNxUiUtil_OptItem_t	eChangedSort;					// Sorting Item
	eNxChlist_OptItem_t	ePinCodeActiontItem;			// Pin-Code Ation Item
} NxUiChannelListContents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static NxUiChannelListContents_t		s_stUiChListContents;
static NX_PinDialogInstance_t			s_stUiChList_PinCodeDialog;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_chlist_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlist_DestroyPinCodeDialog(void);
static void				nx_chlist_CreatePinCodeDialog(void);

static ONDK_Result_t	nx_chlist_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_chlist_CreateResponseDialouge(HUINT8* pStrinId);

/* Channel List Option */
static ONDK_Result_t	nx_chlist_OptionActionChangeGroup(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionAddFavourite(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionSort(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionDelete(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionLockUnLock(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionLock(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_OptionActionUnLock(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_OptionActionRename(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionSaveRename(NxUiChannelListContents_t *pstContents);

/* Channel List */
static NxUiChannelListContents_t* 	nx_chlist_GetContents(void);

static ONDK_Result_t	nx_chlist_ResetProgrammeTimer(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_ResetExitChListTimer(NxUiChannelListContents_t *pstContents);
static HCHAR 			*nx_chlist_GetCasTypeImagPath(NX_Channel_t *channel);
static ONDK_Result_t	nx_chlist_ReloadChannelList(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t 	nx_chlist_ActionChannelList(NxUiChannelListContents_t *pstContents, HINT32 keyId);
static ONDK_Result_t	nx_chlist_SetPfSearchCurChannel(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_SearchStart(NxUiChannelListContents_t *pstContents);
static HERROR			nx_chlist_SetChannelListData (NxUiChannelListContents_t *pstContents, HBOOL bProgramme);
static NX_Programme_t*	nx_chlist_GetPfProgramme(NX_Channel_t *pstCurChannel);

static void				nx_chlist_UpdateUiFocusItem(NxUiChannelListContents_t *pstContents);
static void				nx_chlist_UpdateUiOptionMenu(NxUiChannelListContents_t *pstContents);
static void				nx_chlist_SetUiCreateOptionMenu(NxUiChannelListContents_t *pstContents);
static HERROR 			nx_chlist_SetUiObject(NxUiChannelListContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	nx_chlist_MsgGwmCreate(NxUiChannelListContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlist_MsgGwmDestroy(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_chlist_MsgGwmKeyDown(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_chlist_MsgGwmTimer(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlist_MsgPopUpOptValueChange(NxUiChannelListContents_t *pstContents, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Pin_Code_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_chlist_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stUiChList_PinCodeDialog, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_chlist_DestroyPinCodeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_chlist_DisplayPinCodeDialog);
}


static void		nx_chlist_CreatePinCodeDialog(void)
{
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0;

	stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
	NX_PRISM_COMP_PinDlalog_SetDialog(&s_stUiChList_PinCodeDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(RES_MESG_3684_ID),
								eNxPinCodeDataType_ParentalControl);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuGeneral_DisplayPinCodeDialog", APP_DEFAULT_PRIORITY, nx_chlist_DisplayPinCodeDialog, 0, 0, 0, 0);
}


/******************************************************************************/
#define __Response_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_chlist_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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


static void		nx_chlist_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_chlist_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_chlist_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


/******************************************************************************/
#define __Option_Functions______________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_chlist_OptionActionChangeGroup(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	HBOOL		bFindEqualChannel = FALSE;
	HINT32		lGroupNum = 0, i = 0;
	HUINT32		ulExtraFilter = 0;
	NX_ChListGroup_t eChGroupType = eNxChListGroup_TV;
	NX_Channel_t	*pstGroupList = NULL, *pstCurChannel = NULL, *pstFindChannel = NULL;
	NX_Channel_t	stLastChannel;

	eChGroupType = (NX_ChListGroup_t)p3;

	if (pstContents->eChangedGroup == eChGroupType)
	{
		HxLOG_Debug("\n\neChGroupType is Equal eCurChGroupType\n\n");
		return ONDK_RET_OK;
	}

	if ((pstContents->bChangedGroup == FALSE) && (pstContents->pstChannels == NULL))
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eChGroupType);

	// make group list
	if (ERR_OK != NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstGroupList, &lGroupNum, eChGroupType, ulExtraFilter	/* DxAlpabetGruopType_b .. etc*/))
	{
		HxLOG_Debug("\nFailed Make Group list! (eChGroupType : %d)\n", eChGroupType);
		return ONDK_RET_FAIL;
	}

	if (lGroupNum <= 0)
	{
		NX_APP_Error("Error!!! eChGroupType : (0x%x)(lGroupNum <= 0) !!\n", eChGroupType);
		if(pstGroupList)
		{
			NX_APP_Free(pstGroupList);
			pstGroupList = NULL;
		}
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
	HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurItemIndex], sizeof(NX_Channel_t));

	// bChangedGroup 가 true 이면, channel list mem free
	if (pstContents->bChangedGroup && pstContents->pstChannels != NULL)
	{
		// bChangedGroup 가 true 이면, pstContents->pstChannels 는 재할당 받은것이다..
		NX_APP_Free(pstContents->pstChannels);
		pstContents->pstChannels = NULL;
	}

	// change channel list in prism channel list context
	pstContents->pstChannels = pstGroupList;
	pstContents->nChannelNum = lGroupNum;

	HxLOG_Debug("\nNew group list Num : %d \n\n", lGroupNum);
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
			pstContents->nCurItemIndex = i;
			break;
		}
	}

	if (bFindEqualChannel == FALSE)
		pstContents->nCurItemIndex = 0;

	pstContents->bChangedGroup = TRUE;
	pstContents->eChangedGroup = eChGroupType;

	// update ui
	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (pstCurChannel != NULL)
	{
		nx_chlist_SetChannelListData(pstContents, FALSE);
		nx_chlist_SetPfSearchCurChannel(pstContents);
		nx_chlist_ResetProgrammeTimer(pstContents);
		// Option Group name update
		nx_chlist_UpdateUiOptionMenu(pstContents);
	}
	else
	{
		NX_APP_Error("Error!!! pstCurChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionAddFavourite(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	HBOOL	bIsFavGroupList = FALSE;

	// Alreay Saved Action, so Just update list if in favourite list
	bIsFavGroupList = NX_CHANNEL_GROUP_IsFavouriteGroup(pstContents->eChangedGroup);
	if (bIsFavGroupList == TRUE)
	{
		nx_chlist_ReloadChannelList(pstContents);
	}
	else
	{
		// Not to do
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionSort(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	HINT32			i = 0, lSortChNum = 0;
	HBOOL			bFindEqualChannel = FALSE;
	HxList_t		*pSortList = NULL;
	NX_Channel_t	*pstCurChannel = NULL, *pstFindChannel = NULL,  *pstSortChLists = NULL;
	NX_Channel_t	stLastChannel;
	eNxUiUtil_OptItem_t		eOptItem = eNxUtil_OptItem_None;


	eOptItem = (eNxUiUtil_OptItem_t)p3;

	switch (eOptItem)
	{
		case eNxUtil_OptItem_SortNumber:
			{
				if (pstContents->pstChannels == NULL)
				{
					NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
					return ONDK_RET_FAIL;
				}

				pstContents->eChangedSort = eNxUtil_OptItem_SortNumber;
				NX_CHANNEL_GROUP_SortNxChannelsByLcn(pstContents->pstChannels, pstContents->nChannelNum);

				HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
				HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurItemIndex], sizeof(NX_Channel_t));

				bFindEqualChannel = FALSE;
				for (i = 0; i < pstContents->nChannelNum; i++)
				{
					pstFindChannel = &pstContents->pstChannels[i];

					if (pstFindChannel	&&
						(pstFindChannel->svcUid == stLastChannel.svcUid )&&
						(pstFindChannel->lcn	== stLastChannel.lcn))
					{
						bFindEqualChannel = TRUE;
						pstContents->nCurItemIndex = i;
						break;
					}
				}

				if (bFindEqualChannel == FALSE)
					pstContents->nCurItemIndex = 0;

				// update ui
				pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
				if (pstCurChannel != NULL)
				{
					nx_chlist_SetChannelListData(pstContents, FALSE);
					nx_chlist_SetPfSearchCurChannel(pstContents);
					nx_chlist_ResetProgrammeTimer(pstContents);
				}
				else
				{
					return ONDK_RET_FAIL;
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

				pstContents->eChangedSort = eNxUtil_OptItem_SortName;
				pSortList = NX_CHANNEL_GROUP_SortNxChannelsByName(pstContents->pstChannels, pstContents->nChannelNum);
				if (pSortList == NULL)
				{
					break;
				}

				NX_CHANNEL_GROUP_MakeNxChannelListByList(pSortList,  &pstSortChLists, &lSortChNum);

				// sort list 제거
				HLIB_LIST_RemoveAll(pSortList);

				HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
				HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurItemIndex], sizeof(NX_Channel_t));

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
						pstContents->nCurItemIndex = i;
						break;
					}
				}

				if (bFindEqualChannel == FALSE)
					pstContents->nCurItemIndex = 0;

				// update ui
				pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
				if (pstCurChannel != NULL)
				{
					nx_chlist_SetChannelListData(pstContents, FALSE);
					nx_chlist_SetPfSearchCurChannel(pstContents);
					nx_chlist_ResetProgrammeTimer(pstContents);
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


static ONDK_Result_t	nx_chlist_OptionActionDelete(NxUiChannelListContents_t *pstContents, HINT32 p3)
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

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
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
	nx_chlist_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5653_ID));

	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionLockUnLock(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	eNxUiUtil_OptItem_t 	eUtilOptItem = (eNxUiUtil_OptItem_t)p3;

	if (eUtilOptItem == eNxUtil_OptItem_Lock)
	{
		nx_chlist_OptionActionLock(pstContents);
	}
	else
	{
		pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Lock;
		nx_chlist_CreatePinCodeDialog();
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionLock(NxUiChannelListContents_t *pstContents)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t		*stSvcInfo = NULL;


	NX_APP_Info("Action - eNxUtil_OptItem_Lock !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
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
	nx_chlist_UpdateUiFocusItem(pstContents);

	/* Show update Info Message */
	nx_chlist_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5654_ID));

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionUnLock(NxUiChannelListContents_t *pstContents)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t		*stSvcInfo = NULL;


	NX_APP_Info("Action - eNxUtil_OptItem_UnLock !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
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
	nx_chlist_UpdateUiFocusItem(pstContents);

	/* Show update Info Message */
	nx_chlist_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5655_ID));

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionRename(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t		*stSvcInfo = NULL;


	NX_APP_Info("Action - eNxChlist_OptItem_Rename !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
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

	ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXPIRE_TIMER_ID);
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_POPUP_PRIORITY,
				NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, NX_PRISM_KEYBOARD_INPUT_NORMAL, (HINT32)stSvcInfo->name);

	NX_COMMON_NOT_USED_PARAM(p3);
	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionSaveRename(NxUiChannelListContents_t *pstContents)
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

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
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
		nx_chlist_UpdateUiFocusItem(pstContents);
	}

	/* String Buffer 초기화. */
	pszStringBuf = NX_PRISM_GetTextBuffer(NX_TEXT_SIZE_512);
	if (pszStringBuf)
	{
		HxSTD_MemSet(pszStringBuf, 0x00, NX_TEXT_SIZE_512);
		HxSTD_sprintf((char*)pszStringBuf, (char*)ONDK_GetString(RES_MESG_6113_ID), pstContents->szRenameText);

		/* Show update Info Message */
		nx_chlist_CreateResponseDialouge(pszStringBuf);
	}
	else
	{
		/* Show update Info Message */
		nx_chlist_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_6113_ID));
	}

	return ONDK_RET_OK;
}



/******************************************************************************/
#define __Channel_list_Functions______________________________________________
/******************************************************************************/

static NxUiChannelListContents_t*	nx_chlist_GetContents(void)
{
	return &s_stUiChListContents;
}


static ONDK_Result_t	nx_chlist_ResetProgrammeTimer(NxUiChannelListContents_t *pstContents)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;
	// set timer
	retTimer = ONDK_GWM_SetTimer(NX_PRISM_CHLIST_PROGRAMME_TIMER_ID, NX_PRISM_CHLIST_PROGRAMME_EXPIRE_TIME);

	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_PRISM_CHLIST_PROGRAMME_TIMER_ID);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return retTimer;
}


static ONDK_Result_t	nx_chlist_ResetExitChListTimer(NxUiChannelListContents_t *pstContents)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;

	// set timer
	retTimer = ONDK_GWM_SetTimer(NX_PRISM_CHLIST_EXPIRE_TIMER_ID, NX_PRISM_CHLIST_EXPIRE_TIME);
	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_PRISM_CHLIST_EXPIRE_TIMER_ID);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return retTimer;
}

static HCHAR 	*nx_chlist_GetCasTypeImagPath(NX_Channel_t *channel)
{
	DxService_t	*findService 	= NULL;

	if (channel == NULL)
	{
		HxLOG_Debug("channel is NULL!\n");
		return NULL;
	}

	findService = NX_CHANNEL_FindServiceBySvcUid(channel->svcUid);
	if (findService == NULL)
	{
		HxLOG_Debug("Not found DxService info!\n");
		return NULL;
	}

	return	NX_PRISM_UTIL_GetCasTypeResource(findService->casType);
}

static ONDK_Result_t	nx_chlist_SearchStart(NxUiChannelListContents_t *pstContents)
{
	NX_MetadataSearch_t *pstMetaSearch = NULL;
	NX_QUERY_t	endQuery, startQuery, andQuery;
	HUINT32 	unixtime = 0, queryTime = 0;
	HCHAR 		value[64] = {'\0',};

	NX_Channel_t	*pstCurChannel = NULL;

	if (NX_SYSTEM_GetTime(&unixtime) != ERR_OK)
	{
		HxLOG_Error("time is Invalid!!!\n");
		return ONDK_RET_FAIL;
	}

	queryTime	= unixtime - (SECONDS_PER_HOUR * 1);

	HxSTD_sprintf(value, "%u", queryTime);
	endQuery = NX_METADATA_SEARCH_CreateQuery("endTime", ">", value);

	queryTime 	= unixtime + SECONDS_PER_HOUR * 2; // plus 2 hour
	HxSTD_sprintf(value, "%u", queryTime);
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

	pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];

	NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, pstCurChannel);

	NX_PORT_META_QUERY_SetFilter(pstMetaSearch->context, eNxQUERY_FILTER_ALL);

	NX_METADATA_EPG_SearchEPG(pstMetaSearch);

	return GWM_RET_OK;
}

static ONDK_Result_t	nx_chlist_SetPfSearchCurChannel(NxUiChannelListContents_t *pstContents)
{
	NX_Channel_t	*pstCurChannel = NULL;

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (pstCurChannel != NULL)
	{
		nx_chlist_SearchStart(pstContents);
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_ReloadChannelList(NxUiChannelListContents_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bFindEqualChannel = FALSE;
	HINT32			lGroupNum = 0, i = 0;
	HUINT32 		ulExtraFilter = 0;
	NX_Channel_t	*pstGroupList = NULL, *pstCurChannel = NULL, *pstFindChannel = NULL;
	NX_Channel_t	stLastChannel;
	NX_ChListGroup_t eChGroupType = eNxChListGroup_TV;


	eChGroupType = pstContents->eChangedGroup;

	if ((pstContents->bChangedGroup == FALSE) && (pstContents->pstChannels == NULL))
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eChGroupType);

	// make group list
	hError = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstGroupList, &lGroupNum, eChGroupType, ulExtraFilter	/* DxAlpabetGruopType_b .. etc*/);
	if (ERR_OK != hError)
	{
		NX_APP_Error("\nFailed Make Group list! (eChGroupType : 0x%x)\n", eChGroupType);
		return	ONDK_RET_FAIL;
	}

	if (lGroupNum <= 0)
	{
		NX_APP_Info(" Make Group was Zero list! (eChGroupType : 0x%x)\n", eChGroupType);
		if(pstGroupList)
		{
			NX_APP_Free(pstGroupList);
			pstGroupList = NULL;
		}
		return ONDK_RET_OK;
	}

	HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
	HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurItemIndex], sizeof(NX_Channel_t));

	// bChangedGroup 가 true 이면, channel list mem free
	if (pstContents->bChangedGroup && pstContents->pstChannels != NULL)
	{
		// bChangedGroup 가 true 이면, pstContents->pstChannels 는 재할당 받은것이다..
		NX_APP_Free(pstContents->pstChannels);
		pstContents->pstChannels = NULL;
	}

	// change channel list in prism channel list context
	pstContents->pstChannels = pstGroupList;
	pstContents->nChannelNum = lGroupNum;

	HxLOG_Debug("\nNew group list Num : %d \n\n", lGroupNum);
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
			pstContents->nCurItemIndex = i;
			break;
		}
	}

	if (bFindEqualChannel == FALSE)
	{
		if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
		{
			pstContents->nCurItemIndex = 0;
		}
	}

	// update ui
	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (pstCurChannel != NULL)
	{
		nx_chlist_SetPfSearchCurChannel(pstContents);
		nx_chlist_SetChannelListData(pstContents, FALSE);
		nx_chlist_ResetProgrammeTimer(pstContents);
		// Option Group name update
		nx_chlist_UpdateUiOptionMenu(pstContents);
	}
	else
	{
		NX_APP_Error("Error!!! pstCurChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_ActionChannelList(NxUiChannelListContents_t *pstContents, HINT32 keyId)
{
	ONDK_Result_t		ret = ONDK_RET_OK;
	NX_Channel_t		*stCurPlayChannel = NULL, *stSelectChannel = NULL;
	NX_ChListGroup_t	eChListGroupType = eNxChListGroup_CURRENT, eChListPrevGroupType = eNxChListGroup_CURRENT;

	switch(keyId)
	{
		case KEY_EXIT:
		case KEY_BACK:
			// 지금은 사라지는 것만..
			{
				ONDK_GWM_APP_Destroy(0);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_OK:
			{
				// 그룹 체인지가 되고, channel 변경이 이루어 진다면,
				// main service list 를 현재 그룹으로 셋팅하고, (nx_core_channel)
				// 관련 모듈들에서 service list 를 reload 하도록 메세지를 날린다.
				if (pstContents->pstChannels == NULL)
				{
					NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
					return ONDK_RET_FAIL;
				}

				if (pstContents->bChangedGroup)
				{
					eChListGroupType = NX_PRISM_LIVE_GetCurrentChGroupType();
					eChListPrevGroupType = eChListGroupType;
					if (eChListGroupType != pstContents->eChangedGroup)
					{
						eChListGroupType = pstContents->eChangedGroup;
					}
					else
					{
						eChListGroupType = eNxChListGroup_CURRENT;
					}
				}

				stCurPlayChannel = NX_PRISM_LIVE_GetMasterChannel();
				stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
				if ((stCurPlayChannel != NULL) && (stSelectChannel != NULL) && (stSelectChannel->svcUid != stCurPlayChannel->svcUid))
				{
					NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstContents->pstChannels[pstContents->nCurItemIndex].svcUid, eChListGroupType, 0);
				}
				else if ((stCurPlayChannel != NULL) && (stSelectChannel != NULL) && ((stSelectChannel->svcUid == stCurPlayChannel->svcUid) && (pstContents->bChangedGroup)))
				{
					NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstContents->pstChannels[pstContents->nCurItemIndex].svcUid, eChListGroupType, 0);
				}

				ONDK_GWM_APP_Destroy(0);
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_ARROWUP:
			{
				pstContents->nCurItemIndex--;
				if (pstContents->nCurItemIndex < 0)
					pstContents->nCurItemIndex = pstContents->nChannelNum - 1;

				if (pstContents->nChannelNum > 1)
				{
					nx_chlist_SetPfSearchCurChannel(pstContents);
					nx_chlist_SetChannelListData(pstContents, FALSE);
				}
				ret = ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_ARROWDOWN:
			{
				pstContents->nCurItemIndex++;
				if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
					pstContents->nCurItemIndex = 0;

				if (pstContents->nChannelNum > 1)
				{
					nx_chlist_SetPfSearchCurChannel(pstContents);
					nx_chlist_SetChannelListData(pstContents, FALSE);
				}
				ret = ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_CH_PLUS:
			{
				if (pstContents->nChannelNum < NX_PRISM_CHLIST_ITEM_MAX)
				{
					ret = ONDK_MESSAGE_BREAK;
					break;
				}

				pstContents->nCurItemIndex -= NX_PRISM_CHLIST_ITEM_MAX;
				if (pstContents->nCurItemIndex < 0)
					pstContents->nCurItemIndex = pstContents->nChannelNum - NX_PRISM_CHLIST_ITEM_MAX;
				nx_chlist_SetPfSearchCurChannel(pstContents);
				nx_chlist_SetChannelListData(pstContents, FALSE);
				ret = ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_CH_MINUS:
			{
				if (pstContents->nChannelNum < NX_PRISM_CHLIST_ITEM_MAX)
				{
					ret = ONDK_MESSAGE_BREAK;
					break;
				}

				pstContents->nCurItemIndex += NX_PRISM_CHLIST_ITEM_MAX;
				if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
				{
					pstContents->nCurItemIndex = (pstContents->nCurItemIndex - pstContents->nChannelNum);
				}
				nx_chlist_SetPfSearchCurChannel(pstContents);
				nx_chlist_SetChannelListData(pstContents, FALSE);
				ret = ONDK_MESSAGE_BREAK;
			}
			break;

		default:
			break;
	}

	nx_chlist_ResetProgrammeTimer(pstContents);
	nx_chlist_ResetExitChListTimer(pstContents);

	return ret;
}


static HERROR	nx_chlist_SetChannelListData (NxUiChannelListContents_t *pstContents, HBOOL bProgramme)
{
	HINT32			i = 0, getIndex = 0, lFocusIdx = 0, lFocusItemCnt = 0;
	HUINT32			ulStartTime = 0, ulDuration = 0, ulWeekIndex = 0;
	HCHAR 			*pszCasImgPath = NULL, szEventName[EPG_HUMAX_NAME_LEN];
	HxDATETIME_t	stStartTime;
	NX_Channel_t 	*channel = NULL;
	DxService_t 	*stSvcInfo = NULL;
	NX_Programme_t	*programme = NULL;
#if defined(CONFIG_PROD_LANG_ARABIC)
	HINT32	currentLanguage = eLangID_NONE;
#endif


	if (pstContents->pstChListData == NULL)	// Make Alloc Data if it's first
	{
		pstContents->pstChListData = NX_APP_Calloc(sizeof(NxChListData_t) * NX_PRISM_CHLIST_ITEM_MAX);
	}

	lFocusItemCnt = (NX_PRISM_CHLIST_ITEM_FOCUS_INDEX + 1);

	// Service List less than 3 channel item
	if (pstContents->nChannelNum < lFocusItemCnt)
	{
		if (pstContents->nChannelNum < (lFocusItemCnt - 1))	// 1 channel Item
		{
			getIndex = 0;
		}
		else
		{
			getIndex = (pstContents->nCurItemIndex == 0) ? (pstContents->nCurItemIndex + 1) : (pstContents->nCurItemIndex - 1) ;
		}
	}
	else
	{
		// current channel 이 2번 이므로, 처음 표시될 item 부터 표시하도록 설정.
		getIndex = pstContents->nCurItemIndex - 2;
		if (getIndex < 0)
		{
			getIndex = pstContents->nChannelNum + getIndex;
		}
	}

	// clear cas img
	ONDK_GWM_COM_ChannelList_SetCasImg(NX_PRISM_CHLIST_ID, NULL);
	ONDK_GWM_COM_ChannelList_SetLockImg(NX_PRISM_CHLIST_ID, NULL);

	// clear time
	ONDK_GWM_COM_ChannelList_SetProgrammeTime(NX_PRISM_CHLIST_ID, 0, 0);

	if (pstContents->nChannelNum < lFocusItemCnt)
	{
		lFocusIdx = (pstContents->nChannelNum - 1);
		lFocusIdx = (lFocusIdx < 0) ? 0 : lFocusIdx;
	}
	else
	{
		lFocusIdx = NX_PRISM_CHLIST_ITEM_FOCUS_INDEX;
	}

	for (i = 0; i < NX_PRISM_CHLIST_ITEM_MAX; i++)
	{
		// 채널리스트가 리스트 아이템보다 작으면 처리안함.
		if (i >= pstContents->nChannelNum)
		{
			pstContents->pstChListData[i].lcn = 0;
			pstContents->pstChListData[i].pszChName = NULL;
			HxSTD_sprintf(pstContents->pstChListData[i].pszEventDate, "  ");
			HxSTD_sprintf(pstContents->pstChListData[i].pszEventName, "  ");
			break;
		}

		channel = &pstContents->pstChannels[getIndex];
		stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(channel->svcUid);
		if (stSvcInfo == NULL)
		{
			NX_APP_Error("Error!!! NX_CHANNEL_FindServiceBySvcUid() - svcUid:(0x%x) Fail !! \n", channel->svcUid);
			continue;
		}

		HxSTD_MemSet(&pstContents->pstChListData[i], 0, sizeof(NxChListData_t));
		pstContents->pstChListData[i].lcn = channel->lcn;
		pstContents->pstChListData[i].pszChName = stSvcInfo->name;

		if ((i == lFocusIdx) && (bProgramme == TRUE))
		{
			programme = nx_chlist_GetPfProgramme(channel);
			if (programme)
			{
				NX_PF_PROGRAMMES_GetNameByProgramme(programme, szEventName);
				HxSTD_MemSet(pstContents->pstChListData[i].pszEventName, 0, EPG_HUMAX_NAME_LEN);
				HLIB_STD_StrUtf8NCpy(pstContents->pstChListData[i].pszEventName, szEventName, EPG_HUMAX_NAME_LEN);

				ulStartTime = NX_PF_PROGRAMMES_GetStartTimeByProgramme(programme);
				ulDuration = NX_PF_PROGRAMMES_GetDurationByProgramme(programme);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &stStartTime);
				ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stStartTime.stDate);
				HxSTD_sprintf(pstContents->pstChListData[i].pszEventDate, "%s %02d %s",
							ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex)),
							stStartTime.stDate.ucDay, ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByMonth(stStartTime.stDate.ucMonth)));

				// set programme time
				ONDK_GWM_COM_ChannelList_SetProgrammeTime(NX_PRISM_CHLIST_ID, ulStartTime, ulDuration);
			}
			// set cas img
			pszCasImgPath = nx_chlist_GetCasTypeImagPath(channel);
			ONDK_GWM_COM_ChannelList_SetCasImg(NX_PRISM_CHLIST_ID, pszCasImgPath);

			// Set Lock Image
			if (stSvcInfo && (stSvcInfo->locked == TRUE))
			{
				ONDK_GWM_COM_ChannelList_SetLockImg(NX_PRISM_CHLIST_ID, RES_211_00_LOCK_PNG);
			}
		}

		getIndex++;
		if (getIndex >= pstContents->nChannelNum)
			getIndex = 0;
	}


#if defined(CONFIG_PROD_LANG_ARABIC)
	NX_SYSTEM_GetMenuLanuage(&currentLanguage);
	ONDK_GWM_COM_ChannelList_SetUserdata(NX_PRISM_CHLIST_ID, pstContents->pstChListData, pstContents->nChannelNum, currentLanguage);
#else
	ONDK_GWM_COM_ChannelList_SetUserdata(NX_PRISM_CHLIST_ID, pstContents->pstChListData, pstContents->nChannelNum);
#endif
	ONDK_GWM_APP_InvalidateObject(NX_PRISM_CHLIST_ID);

	return ERR_OK;
}

static NX_Programme_t*	nx_chlist_GetPfProgramme(NX_Channel_t *pstCurChannel)
{
	HINT32			num 		= 0;
	HUINT32			unixtime	= 0;
	NX_Programme_t	*programme 	= NULL;
	HxVector_t		*result 	= NULL;

	result = HLIB_VECTOR_New(NULL,NULL);

	if (NX_METADATA_EPG_GetResultProgrammeBySvcUid(pstCurChannel->svcUid, result, &num) == ERR_FAIL)
	{
		HxLOG_Debug("No There Programme Data!\n");
		goto END_FUNC;
	}

	if (NX_SYSTEM_GetTime(&unixtime) != ERR_OK)
	{
		HxLOG_Error("time is Invalid!!!\n");
		goto END_FUNC;
	}

	if (num > 0 && (result != NULL))
	{
		HINT32	j = 0;
		HUINT32 startTime = 0, duration = 0;
		for (j = 0; j < num; j++)
		{
			programme = HLIB_VECTOR_ItemAt(result, j);

			startTime	= NX_METADATA_PROGRAMMES_GetStartTime(programme);
			duration	= NX_METADATA_PROGRAMMES_GetDuration(programme);

			if ((unixtime >= startTime) &&
				(unixtime <= (startTime + duration)))
			{
				HxLOG_Debug("\nFind Programme ! (svcuid (%d))\n\n", pstCurChannel->svcUid);
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	HLIB_VECTOR_Delete(result);		// new list 만 날리고, core epg 의 data 는 날리면 안됨.
	return programme;
}

static void		nx_chlist_UpdateUiFocusItem(NxUiChannelListContents_t *pstContents)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t		*stSvcInfo = NULL;

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return;
	}

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return;
	}

	if (stSvcInfo->locked == TRUE)
	{
		ONDK_GWM_COM_ChannelList_SetLockImg(NX_PRISM_CHLIST_ID, RES_211_00_LOCK_PNG);
	}
	else
	{
		ONDK_GWM_COM_ChannelList_SetLockImg(NX_PRISM_CHLIST_ID, NULL);
	}
}


static void		nx_chlist_UpdateUiOptionMenu(NxUiChannelListContents_t *pstContents)
{
	HUINT8		stGroupStr[NX_TEXT_SIZE_128] = {'\0',};
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;


	eCurGroup = pstContents->eChangedGroup;

	/* eNxChListGroup_AlphabetAll ~ eNxChListGroup_AlphabetEtc is included in the eNxChListGroup_ALPHABET Group */
	if ((eCurGroup >= eNxChListGroup_AlphabetAll) && (eCurGroup <= eNxChListGroup_AlphabetEtc))
	{
		eCurGroup = eNxChListGroup_ALPHABET;
	}

	NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(eCurGroup, (HCHAR*)stGroupStr);
	HxSTD_sprintf(pstContents->szOptionText, "%s - %s", (HCHAR*)ONDK_GetString(RES_OPTION_01_ID), (HCHAR*)stGroupStr);
	ONDK_GWM_COM_Multi_ChangeLeftStr(NX_PRISM_CHLIST_OPTION_TEXT_ID, pstContents->szOptionText);
}


static void		nx_chlist_SetUiCreateOptionMenu(NxUiChannelListContents_t *pstContents)
{
	HUINT8				stGroupStr[NX_TEXT_SIZE_128] = {'\0',};
	ONDK_Rect_t			stRect;
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;


	stRect = ONDK_Rect(NX_PRISM_CHLIST_OPTION_TEXT_X, NX_PRISM_CHLIST_OPTION_TEXT_Y,
						NX_PRISM_CHLIST_OPTION_TEXT_W, NX_PRISM_CHLIST_OPTION_TEXT_H);

	eCurGroup = pstContents->eChangedGroup;

	/* eNxChListGroup_AlphabetAll ~ eNxChListGroup_AlphabetEtc is included in the eNxChListGroup_ALPHABET Group */
	if ((eCurGroup >= eNxChListGroup_AlphabetAll) && (eCurGroup <= eNxChListGroup_AlphabetEtc))
	{
		eCurGroup = eNxChListGroup_ALPHABET;
	}
	NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(eCurGroup, (HCHAR*)stGroupStr);

	HxSTD_sprintf(pstContents->szOptionText, "%s - %s", (HCHAR*)ONDK_GetString(RES_OPTION_01_ID), (HCHAR*)stGroupStr);
	ONDK_GWM_COM_Multi_Create(NX_PRISM_CHLIST_OPTION_TEXT_ID, &stRect);
	ONDK_GWM_COM_Multi_SetContents(NX_PRISM_CHLIST_OPTION_TEXT_ID, (HUINT8*)RES_201_00_OPTION_PNG,
								NULL, (HUINT8*)pstContents->szOptionText, NULL, NX_PRISM_CHLIST_OPTION_IMG_MG);
	ONDK_GWM_COM_Multi_SetFontHandle(NX_PRISM_CHLIST_OPTION_TEXT_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Multi_SetFontColor(NX_PRISM_CHLIST_OPTION_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));
	ONDK_GWM_COM_Multi_SetFocusFlag(NX_PRISM_CHLIST_OPTION_TEXT_ID, FALSE);
}


static HERROR	nx_chlist_SetUiObject(NxUiChannelListContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	// frame
	ONDK_GWM_APP_SetAppArea(NX_PRISM_CHLIST_FRAME_X, NX_PRISM_CHLIST_FRAME_Y, NX_PRISM_CHLIST_FRAME_W, NX_PRISM_CHLIST_FRAME_H);

	// Draw - Background Frame
	stRect = ONDK_Rect(NX_PRISM_CHLIST_BG_X, NX_PRISM_CHLIST_FRAME_Y, NX_PRISM_CHLIST_FRAME_W, NX_PRISM_CHLIST_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_CHLIST_BG_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_CHLIST_BG_ID, NX_PRISM_COMP_DrawFrameBG_SS);

	// channel list
	ONDK_GWM_COM_ChannelList_Create(NX_PRISM_CHLIST_ID, NX_PRISM_CHLIST_POS_X, NX_PRISM_CHLIST_POS_Y);
	nx_chlist_SetChannelListData(pstContents, TRUE);
	ONDK_GWM_Obj_SetVisible(NX_PRISM_CHLIST_ID, TRUE);
	ONDK_GWM_APP_InvalidateObject(NX_PRISM_CHLIST_ID);

	// Curosr Item Line
	stRect = ONDK_Rect(NX_PRISM_CHLIST_CHBOX_LINE_X, NX_PRISM_CHLIST_CHBOX_LINE_Y,
						NX_PRISM_CHLIST_CHBOX_LINE_W, NX_PRISM_CHLIST_CHBOX_LINE_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_CHLIST_CHBOX_LINE_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_CHLIST_CHBOX_LINE_ID, (HCHAR*)RES_107_00_LINE_LTV_CH_IHUB_PNG);

	// Chlist Line
	stRect = ONDK_Rect(NX_PRISM_CHLIST_LIST_LINE_X, NX_PRISM_CHLIST_LIST_LINE_Y,
						NX_PRISM_CHLIST_LIST_LINE_W, NX_PRISM_CHLIST_LIST_LINE_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_CHLIST_LIST_LINE_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_CHLIST_LIST_LINE_ID, (HCHAR*)RES_107_00_LINE_CONTEXT_L_PNG);

	// Option
	nx_chlist_SetUiCreateOptionMenu(pstContents);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(hAct);

	return ERR_OK;
}


static ONDK_Result_t	nx_chlist_MsgGwmCreate(NxUiChannelListContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8	*pszPanelStr = NULL;

	HxSTD_MemSet(pstContents, 0, sizeof(NxUiChannelListContents_t));

	pstContents->nCurItemIndex = p1;
	pstContents->nChannelNum = p2;
	pstContents->pstChannels = NX_PRISM_LIVE_GetLiveChannels();
	pstContents->bChangedGroup = FALSE;
	pstContents->eChangedGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
	pstContents->eChangedSort = eNxUtil_OptItem_SortNumber;
	pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;

	nx_chlist_SetPfSearchCurChannel(pstContents);

	if (pstContents->pstChannels == NULL)
	{
		HxLOG_Error("channel data is NULL!!!\n");
		return	ONDK_RET_FAIL;
	}

	nx_chlist_SetUiObject(pstContents, hAct, p1, p2, p3);
	nx_chlist_ResetProgrammeTimer(pstContents);
	nx_chlist_ResetExitChListTimer(pstContents);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_ChList);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_ChList !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_MsgGwmDestroy(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_CHLIST_PROGRAMME_TIMER_ID);
	ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXPIRE_TIMER_ID);

	nx_chlist_DestroyPinCodeDialog();

	ONDK_GWM_Obj_Destroy(NX_PRISM_CHLIST_ID);

	if (pstContents->pstChListData)
	{
		NX_APP_Free(pstContents->pstChListData);
		pstContents->pstChListData = NULL;
	}

	if (pstContents->bChangedGroup && pstContents->pstChannels != NULL)
	{
		// bChangedGroup 가 true 이면, pstContents->pstChannels 는 재할당 받은것이다..
		NX_APP_Free(pstContents->pstChannels);
		pstContents->pstChannels = NULL;
	}

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_MsgGwmKeyDown(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	ret = ONDK_RET_FAIL;
	HINT32			lCurItemIndex = 0, lChannelNum = 0;
	NX_Channel_t	*stSelectChannel = NULL;


	nx_chlist_ResetExitChListTimer(pstContents);
	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
	switch (p1)
	{
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_ARROWRIGHT:
		case KEY_ARROWLEFT:
			lCurItemIndex = pstContents->nCurItemIndex;
			lChannelNum = pstContents->nChannelNum;
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_INFOHUB_Proc", APP_BACKGROUND_OSD_PRIORITY, NX_PRISM_INFOHUB_Proc,
									HANDLE_NULL, lCurItemIndex, lChannelNum, 0);
			break;

		case KEY_CH_PLUS:
		case KEY_CH_MINUS:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
		case KEY_OK:
		case KEY_EXIT:
		case KEY_BACK:
			ret = nx_chlist_ActionChannelList(pstContents, p1);
			break;

		case KEY_OPT:
			ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXPIRE_TIMER_ID);
			if (pstContents->pstChannels)
			{
				if (pstContents->pstChannels == NULL)
				{
					NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
					return ONDK_RET_FAIL;

				}
				stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
				if (stSelectChannel)
				{
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_CHANNELLIST_OPT_Proc", APP_DEFAULT_PRIORITY,
								NX_PRISM_CHANNELLIST_OPT_Proc, 0, eNX_UCT_Chlist, stSelectChannel->svcUid, pstContents->eChangedGroup);
				}
				else
				{
					NX_APP_Error("Error!!! Current Channel list Data(%u) was NULL!!!\n", pstContents->nCurItemIndex);
				}
			}
			else
			{
				NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!!\n");
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ret;
}


static ONDK_Result_t	nx_chlist_MsgGwmTimer(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_PRISM_CHLIST_PROGRAMME_TIMER_ID)
	{
		// set programme info
		ONDK_GWM_KillTimer(NX_PRISM_CHLIST_PROGRAMME_TIMER_ID);
		nx_chlist_SetChannelListData(pstContents, TRUE);
	}
	else if (p1 == NX_PRISM_CHLIST_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}


static ONDK_Result_t	nx_chlist_MsgPopUpOptValueChange(NxUiChannelListContents_t *pstContents, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_FAIL;
	eNxChlist_OptItem_t 	eCurOptItem = (eNxChlist_OptItem_t)p2;


	switch (eCurOptItem)
	{
		case eNxChlist_OptItem_Group:
			eResult = nx_chlist_OptionActionChangeGroup(pstContents, p3);
			break;
		case eNxChlist_OptItem_Sort:
			eResult = nx_chlist_OptionActionSort(pstContents, p3);
			break;
		case eNxChlist_OptItem_AddFav:
			eResult = nx_chlist_OptionActionAddFavourite(pstContents, p3);
			break;
		case eNxChlist_OptItem_Delete:
			pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Delete;
			nx_chlist_CreatePinCodeDialog();
			break;
		case eNxChlist_OptItem_Lock:
			eResult = nx_chlist_OptionActionLockUnLock(pstContents, p3);
			break;
		case eNxChlist_OptItem_Rename:
			eResult = nx_chlist_OptionActionRename(pstContents, p3);
			break;
		default:
			NX_APP_Info("Not Support Option Item [%d] \n", eCurOptItem);
			break;
	}

	return ONDK_RET_OK;
}



/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_CHANNELLIST_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t				lRet = ONDK_RET_OK;
	NX_UI_CallerType_t 			eCurOptType = eNX_UCT_None;
	NX_ChListGroup_t			eCurChannelGroup = eNxChListGroup_TV;	// Group Item
	NxUiChannelListContents_t	*pstContents = nx_chlist_GetContents();

	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			HxLOG_Debug("\n[%s.%d] MSG_GWM_CREATE IN NX_UI_LIVE_Proc\n\n",__FUNCTION__,__LINE__);
			lRet = nx_chlist_MsgGwmCreate(pstContents, hHandle, p1, p2, p3);
			if (lRet != ONDK_RET_OK)
			{
				//ONDK_GWM_APP_Destroy(0);
			}
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER\n");
			nx_chlist_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_APK_MESSAGE:
			break;

		case MSG_GWM_HAPI_MESSAGE:
			lRet = ONDK_MESSAGE_PASS;
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYDOWN:
			lRet = nx_chlist_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_DESTROY:
			lRet = nx_chlist_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_APP_POPUP_OPT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_OPT_VALUE_CHANGED\n");
			eCurOptType = (NX_UI_CallerType_t)p1;
			if (eCurOptType == eNX_UCT_Chlist)
			{
				nx_chlist_ResetExitChListTimer(pstContents);
				lRet = nx_chlist_MsgPopUpOptValueChange(pstContents, p2, p3);
			}
			break;

		case MSG_CORE_SERVICE_PF_CHANGED_PROGRAMMES:
			// pf update 가 온경우, 갱신...
			NX_APP_Message("received MSG_CORE_SERVICE_PF_CHANGED_PROGRAMMES MSG.\n");
			break;

		case MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES:
			NX_APP_Message("received MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES MSG.\n");
			break;

		case MSG_APP_SYSTEM_UPDATE_SERVICE:
			NX_APP_Message("MSG_APP_SYSTEM_UPDATE_SERVICE\n");
			/* Reload Channel List */
			nx_chlist_ReloadChannelList(pstContents);
			break;

		case MSG_APP_SYSTEM_RELOAD_SERVICELIST:
			NX_APP_Message("MSG_APP_SYSTEM_UPDATE_SERVICE\n");

			eCurChannelGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
			if (eCurChannelGroup != pstContents->eChangedGroup)
			{
				pstContents->bChangedGroup = TRUE;
				pstContents->eChangedGroup = eCurChannelGroup;
				NX_APP_Info("Change Group List Live Group:(0x%x), list Group:(0x%x) \n", eCurChannelGroup, pstContents->eChangedGroup);
			}

			/* Reload Channel List */
			nx_chlist_ReloadChannelList(pstContents);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			NX_APP_Message("MSG_APP_SYSTEM_PIN_OK\n");
			lRet = ONDK_RET_OK;
			if (pstContents->ePinCodeActiontItem == eNxChlist_OptItem_Delete)
			{
				pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;
				nx_chlist_DestroyPinCodeDialog();
				nx_chlist_OptionActionDelete(pstContents, p3);
				lRet = ONDK_MESSAGE_BREAK;
			}
			else if (pstContents->ePinCodeActiontItem == eNxChlist_OptItem_Lock)
			{
				pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;
				nx_chlist_DestroyPinCodeDialog();
				nx_chlist_OptionActionUnLock(pstContents);
				lRet = ONDK_MESSAGE_BREAK;
			}
			else
			{
				NX_APP_Info("UnKnow PIN Process ePinCodeActiontItem(%d)\n", pstContents->ePinCodeActiontItem);
			}
			break;

		case MSG_APP_SYSTEM_PIN_DENIED:
			NX_APP_Message("MSG_APP_SYSTEM_PIN_DENIED\n");
			nx_chlist_DestroyPinCodeDialog();
			pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;
			lRet = ONDK_MESSAGE_BREAK;
			break;

		case MSG_APP_KEYBOARD_RESULT:
			NX_APP_Message("MSG_APP_KEYBOARD_RESULT\n");
			nx_chlist_ResetExitChListTimer(pstContents);
			nx_chlist_OptionActionSaveRename(pstContents);
			lRet = ONDK_MESSAGE_BREAK;
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Live)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

	}

	if(lRet != ONDK_RET_OK)
	{
		return lRet;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}


HUINT32		NX_PRISM_CHANNELLIST_GetCurrentSort(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;
	NxUiChannelListContents_t	*pstContents = nx_chlist_GetContents();

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_CHANNELLIST_Proc);
	if(pGwmApp == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_CHANNELLIST_Proc() was destoryed!!\n");
		return	(HUINT32)eNxUtil_OptItem_None;
	}

	if (pstContents)
	{
		return	(HUINT32)pstContents->eChangedSort;
	}
	else
	{
		return	(HUINT32)eNxUtil_OptItem_None;
	}
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/


