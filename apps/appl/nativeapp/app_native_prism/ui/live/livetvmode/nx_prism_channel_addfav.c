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
 * @file	  		nx_prism_channlist_addfav.c
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


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_PRISM_CHLIST_ADDFAV_ITEM_ID						(eMenuFrame_Window_CHList + 512)

#define	NX_PRISM_CHLIST_ADDFAV_WINDOW_X						290
#define	NX_PRISM_CHLIST_ADDFAV_WINDOW_Y						337
#define	NX_PRISM_CHLIST_ADDFAV_WINDOW_W						700
#define	NX_PRISM_CHLIST_ADDFAV_WINDOW_H						383

#define	NX_PRISM_CHLIST_ADDFAV_FRAME_ID						(NX_PRISM_CHLIST_ADDFAV_ITEM_ID + 1)
#define	NX_PRISM_CHLIST_ADDFAV_FRAME_X						0
#define	NX_PRISM_CHLIST_ADDFAV_FRAME_Y						0
#define	NX_PRISM_CHLIST_ADDFAV_FRAME_W						NX_PRISM_CHLIST_ADDFAV_WINDOW_W
#define	NX_PRISM_CHLIST_ADDFAV_FRAME_H						NX_PRISM_CHLIST_ADDFAV_WINDOW_H

#define	NX_PRISM_CHLIST_ADDFAV_LIST_BOX_ID					(NX_PRISM_CHLIST_ADDFAV_ITEM_ID + 2)
#define	NX_PRISM_CHLIST_ADDFAV_LIST_BOX_X					(NX_PRISM_CHLIST_ADDFAV_FRAME_X + 30)
#define	NX_PRISM_CHLIST_ADDFAV_LIST_BOX_Y					(NX_PRISM_CHLIST_ADDFAV_FRAME_Y + 73)
#define	NX_PRISM_CHLIST_ADDFAV_LIST_BOX_W					410
#define	NX_PRISM_CHLIST_ADDFAV_LIST_BOX_H					270
#define	NX_PRISM_CHLIST_ADDFAV_LIST_BOX_MG					10

#define	NX_PRISM_CHLIST_ADDFAV_LIST_ID						(NX_PRISM_CHLIST_ADDFAV_ITEM_ID + 3)
#define	NX_PRISM_CHLIST_ADDFAV_LIST_X						(NX_PRISM_CHLIST_ADDFAV_LIST_BOX_X + NX_PRISM_CHLIST_ADDFAV_LIST_BOX_MG)
#define	NX_PRISM_CHLIST_ADDFAV_LIST_Y						(NX_PRISM_CHLIST_ADDFAV_LIST_BOX_Y + NX_PRISM_CHLIST_ADDFAV_LIST_BOX_MG)
#define	NX_PRISM_CHLIST_ADDFAV_LIST_W						(NX_PRISM_CHLIST_ADDFAV_LIST_BOX_W - (NX_PRISM_CHLIST_ADDFAV_LIST_BOX_MG*2))
#define	NX_PRISM_CHLIST_ADDFAV_LIST_H						(NX_PRISM_CHLIST_ADDFAV_LIST_BOX_H - (NX_PRISM_CHLIST_ADDFAV_LIST_BOX_MG*2))
#define	NX_PRISM_CHLIST_ADDFAV_LIST_MAX						5
#define	NX_PRISM_CHLIST_ADDFAV_LIST_ITEM_H					50
#define	NX_PRISM_CHLIST_ADDFAV_LIST_ITEM_MG					3

#define	NX_PRISM_CHLIST_ADDFAV_TITLE_ID						(NX_PRISM_CHLIST_ADDFAV_ITEM_ID + 4)
#define	NX_PRISM_CHLIST_ADDFAV_TITLE_X						(NX_PRISM_CHLIST_ADDFAV_FRAME_X + 30)
#define	NX_PRISM_CHLIST_ADDFAV_TITLE_Y						(NX_PRISM_CHLIST_ADDFAV_FRAME_Y + 46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_CHLIST_ADDFAV_TITLE_W						640
#define	NX_PRISM_CHLIST_ADDFAV_TITLE_H						(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE);
#define	NX_PRISM_CHLIST_ADDFAV_TITLE_LS						32

#define	NX_PRISM_CHLIST_ADDFAV_LINE_TOP_ID					(NX_PRISM_CHLIST_ADDFAV_ITEM_ID + 5)
#define	NX_PRISM_CHLIST_ADDFAV_LINE_TOP_X					NX_PRISM_CHLIST_ADDFAV_FRAME_X
#define	NX_PRISM_CHLIST_ADDFAV_LINE_TOP_Y					NX_PRISM_CHLIST_ADDFAV_FRAME_Y
#define	NX_PRISM_CHLIST_ADDFAV_LINE_TOP_W					NX_PRISM_CHLIST_ADDFAV_WINDOW_W
#define	NX_PRISM_CHLIST_ADDFAV_LINE_TOP_H					1

#define	NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_ID					(NX_PRISM_CHLIST_ADDFAV_ITEM_ID + 6)
#define	NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_X					(NX_PRISM_CHLIST_ADDFAV_FRAME_X + 450)
#define	NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_Y					(NX_PRISM_CHLIST_ADDFAV_FRAME_Y + 238)
#define	NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_W					220
#define	NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_H					50

#define	NX_PRISM_CHLIST_ADDFAV_BTN_MG						5

#define	NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_ID				(NX_PRISM_CHLIST_ADDFAV_ITEM_ID + 7)
#define	NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_X					(NX_PRISM_CHLIST_ADDFAV_FRAME_X + 450)
#define	NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_Y					(NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_Y + NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_H + NX_PRISM_CHLIST_ADDFAV_BTN_MG)
#define	NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_W					220
#define	NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_H					50


#define	NX_PRISM_COMP_LIST_MULTI_SELECT_CURSOR_MG				5

#define	NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_X_MG				8
#define	NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_W				3
#define	NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_H				5

#define	NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_BAR_X			1
#define	NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_BAR_W			1

#define	NX_PRISM_COMP_LIST_MULTI_SELECT_IMG_OFFSET_X			10

#define	NX_PRISM_COMP_LIST_MULTI_SELECT_TEXT_OFFSET_X			14
#define	NX_PRISM_COMP_LIST_MULTI_SELECT_TEXT_OFFSET_Y			(32 - NX_PRISM_FONT_SIZE_22)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	HUINT32				ulSvcUid;
	NX_ChListGroup_t	eCurGroup;
	NX_Channel_t		stChannel;

	// Option Item
	HUINT8			**szItemTextList;
	HUINT8			*pbOrgSelectMark;
	HUINT8			*pbSelectMark;
	NX_Util_OPTIONList_t	stOptList;

	NX_UI_CallerType_t		eDispOptType;
} NxUiChListAddFavContents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static NxUiChListAddFavContents_t		s_UiChListAddFavContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_chlistaddfav_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_chlistaddfav_CreateResponseDialouge(HUINT8* pucStrinId);

static inline NxUiChListAddFavContents_t* nx_chlistaddfav_GetContents(void);

static HERROR			nx_chlistaddfav_CheckAccessLimited(NxUiChListAddFavContents_t *pstContents, HINT32 lSelectIdx);
static ONDK_Result_t	nx_chlistaddfav_SaveChangeData(NxUiChListAddFavContents_t *pstContents);
static ONDK_Result_t	nx_chlistaddfav_MakeFavouriteList(NxUiChListAddFavContents_t *pstContents);

static ONDK_Result_t	nx_chlistaddfav_DrawListMultiSelect_Cursor_A2(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t 	nx_chlistaddfav_SetUiObject(NxUiChListAddFavContents_t *pstContents);
static ONDK_Result_t	nx_chlistaddfav_AddFavouriteGroupItem(NxUiChListAddFavContents_t *pstContents);
static void				nx_chlistaddfav_AddFavouriteGroupItemDefault(NxUiChListAddFavContents_t *pstContents);
static ONDK_Result_t	nx_chlistaddfav_InitData(NxUiChListAddFavContents_t *pstContents);

static ONDK_Result_t	nx_chlistaddfav_MsgGwmCreate(NxUiChListAddFavContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_chlistaddfav_MsgGwmDestroy(NxUiChListAddFavContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlistaddfav_MsgGwmKeyDown(NxUiChListAddFavContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlistaddfav_MsgGwmClickMsg(NxUiChListAddFavContents_t *pstContents, HINT32 p1);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_chlistaddfav_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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
				case KEY_OK:
				case KEY_EXT_AV_PLUS:
				case KEY_EXT_AV_MINUS:
					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_BREAK;
				case KEY_EXIT:
					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_PASS;
				default:
					break;
			}
			break;

		default :
			break;
	}

	return	NX_PRISM_DIALOGUE_ResponseMessage_Proc(lMessage, hHandle, lParam1, lParam2, lParam3);
}


static void		nx_chlistaddfav_CreateResponseDialouge(HUINT8* pucStrinId)
{
	if (pucStrinId == NULL)
	{
		NX_APP_Error("Error!!! Pls check pStrinId was NULL!! \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_chlistaddfav_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_chlistaddfav_ResponseDialogProc, 0, (HINT32)ONDK_GetString((HINT8*)pucStrinId), 0, 0);
}


static inline NxUiChListAddFavContents_t* nx_chlistaddfav_GetContents(void)
{
	return	&s_UiChListAddFavContents;
}


static HERROR	nx_chlistaddfav_CheckAccessLimited(NxUiChListAddFavContents_t *pstContents, HINT32 lSelectIdx)
{
	HINT32				lFavNum = 0;
	NX_FavGroupID_e 	eFavGroupID = eNxFavGroupID_None;


	if (lSelectIdx < (HINT32)pstContents->stOptList.ulOptItemNum)
	{
		// Try Select
		if (pstContents->pbSelectMark[lSelectIdx] == FALSE)
		{
			// Check max Add count
			eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(pstContents->stOptList.aeOptItemList[lSelectIdx]);
			if (eFavGroupID != eNxFavGroupID_None)
			{
				lFavNum = NX_CHANNEL_FAV_GetFavouriteNum(eFavGroupID);
				NX_APP_Info("NX_CHANNEL_FAV_GetFavouriteNum() - Count:(%u) \n", lFavNum);
			}
			else
			{
				NX_APP_Error("Error!!! eNxFavGroupID_None - Channel Group:(0x%x)!!\n", pstContents->stOptList.aeOptItemList[lSelectIdx]);
			}
		}
	}

	if (lFavNum >= NX_CORE_CHANNEL_SUPPORTED_FAV_MAX_ITEM_NUM)
	{
		return	ERR_FAIL;
	}
	else
	{
		return	ERR_OK;
	}
}


static ONDK_Result_t	nx_chlistaddfav_SaveChangeData(NxUiChListAddFavContents_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HBOOL				bChanged = FALSE, bSelected = FALSE;
	HUINT32 			ulOptItemNum = 0, i = 0;
	NX_FavGroupID_e 	eFavGroupID = eNxFavGroupID_None;

	ulOptItemNum = pstContents->stOptList.ulOptItemNum;
	for (i = 0; i < ulOptItemNum; i++)
	{
		// Find Change Value
		if (pstContents->pbSelectMark[i] != pstContents->pbOrgSelectMark[i])
		{
			bChanged = TRUE;

			if (pstContents->pbSelectMark[i] == TRUE)
			{
				// Add Favourite Servcie
				eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(pstContents->stOptList.aeOptItemList[i]);
				if (eFavGroupID != eNxFavGroupID_None)
				{
					hError = NX_CHANNEL_FAV_AddFavouriteItem(eFavGroupID, &pstContents->stChannel);
					if (hError != ERR_OK)
					{
						NX_APP_Error("Error!!! NX_CHANNEL_FAV_AddFavouriteItem() - eFavGroupID :(%d)!!\n", eFavGroupID);
					}
				}
				else
				{
					NX_APP_Error("Error!!! eNxFavGroupID_None - Channel Group:(0x%x)!!\n", pstContents->stOptList.aeOptItemList[i]);
				}
			}
			else
			{
				// Delete Favourite Service
				eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(pstContents->stOptList.aeOptItemList[i]);
				if (eFavGroupID != eNxFavGroupID_None)
				{
					hError = NX_CHANNEL_FAV_RemoveFavouriteItem(eFavGroupID, &pstContents->stChannel);
					if (hError != ERR_OK)
					{
						NX_APP_Error("Error!!! NX_CHANNEL_FAV_RemoveFavouriteItem() - eFavGroupID :(%d)!!\n", eFavGroupID);
					}
				}
				else
				{
					NX_APP_Error("Error!!! eNxFavGroupID_None - Channel Group:(0x%x)!!\n", pstContents->stOptList.aeOptItemList[i]);
				}
			}
		}

		if (pstContents->pbSelectMark[i] == TRUE)
		{
			bSelected = TRUE;
		}
	}

	if ((bChanged == FALSE) && (bSelected == FALSE))
	{
		nx_chlistaddfav_CreateResponseDialouge((HUINT8*)RES_MESG_4562_ID);
	}
	else
	{
		if (bChanged == TRUE)
		{
			ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, pstContents->eDispOptType, eNxChlist_OptItem_AddFav, 0);
		}
		ONDK_GWM_APP_Destroy(0);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlistaddfav_MakeFavouriteList(NxUiChListAddFavContents_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bIsFavGroup = FALSE;
	HUINT32 		ulTextListNum = 0, i = 0;
	HUINT8			**pptextList = NULL;
	DxService_t		*pstSvcInfo;
	NX_FavGroupID_e 	eFavGroupID = eNxFavGroupID_None;
	NX_ChListGroup_t	eChGroup = eNxChListGroup_TV;


	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check added favourite item !!\n");
		return	ONDK_RET_FAIL;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	if (pstContents->szItemTextList == NULL)
	{
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeFavouriteItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
		pptextList = pstContents->szItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Error("Error!!! Please Check added Option item !!\n");
			return	ONDK_RET_FAIL;
		}

		if (pstContents->pbSelectMark == NULL)
		{
			pstContents->pbSelectMark = (HUINT8 *)NX_APP_Malloc(sizeof(HUINT8) * ulTextListNum);
			if (pstContents->pbSelectMark == NULL)
			{
				NX_APP_Error("Error!!! NX_APP_Malloc() Fail!!\n");
				return	ONDK_RET_FAIL;
			}
		}

		if (pstContents->pbOrgSelectMark == NULL)
		{
			pstContents->pbOrgSelectMark = (HUINT8 *)NX_APP_Malloc(sizeof(HUINT8) * ulTextListNum);
			if (pstContents->pbOrgSelectMark == NULL)
			{
				NX_APP_Error("Error!!! NX_APP_Malloc() Fail!!\n");
				return	ONDK_RET_FAIL;
			}
		}

		HxSTD_memset(&pstContents->stChannel, 0x00, sizeof(NX_Channel_t));
		hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(pstContents->eCurGroup, pstContents->ulSvcUid, &pstContents->stChannel);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() Fail!!\n");
			pstSvcInfo = NX_CHANNEL_FindServiceBySvcUid(pstContents->ulSvcUid);
			if (pstSvcInfo != NULL)
			{
				eChGroup = (pstSvcInfo->svcType == eDxSVC_TYPE_TV) ? eNxChListGroup_TV : eNxChListGroup_RADIO ;
				hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eChGroup, pstContents->ulSvcUid, &pstContents->stChannel);
				if (hError != ERR_OK)
				{
					NX_APP_Error("Error!!! Try Again NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() - eChGroup(0x%x) Fail!!\n", eChGroup);
					return	ONDK_RET_FAIL;
				}
			}
			else
			{
				NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() Fail!!\n");
				return	ONDK_RET_FAIL;
			}
		}

		for (i = 0; i < ulTextListNum; i++)
		{
			eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(pstContents->stOptList.aeOptItemList[i]);
			if (eFavGroupID == eNxFavGroupID_None)
			{
				pstContents->pbSelectMark[i] = FALSE;
				pstContents->pbOrgSelectMark[i] = FALSE;
			}
			else
			{
				bIsFavGroup = NX_CHANNEL_FAV_IsFavouriteItem(eFavGroupID, &pstContents->stChannel);
				if (bIsFavGroup == TRUE)
				{
					pstContents->pbSelectMark[i] = TRUE;
					pstContents->pbOrgSelectMark[i] = TRUE;
				}
				else
				{
					pstContents->pbSelectMark[i] = FALSE;
					pstContents->pbOrgSelectMark[i] = FALSE;
				}
			}
		}
	}
	else
	{
		pptextList = pstContents->szItemTextList;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlistaddfav_DrawListMultiSelect_Cursor_A2(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
										ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HBOOL		bNeedScrollBar = FALSE;
	HINT32		i = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0, x = 0, y = 0, w = 0, h = 0, lItem = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = 0, ulStatus = GUIOBJ_NORMAL;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lText_y_off = 0;
	HINT32		lImage_x_off = 0, lImage_y_off = 0, lScroll_w_off = 0, lScroll_height = 0;
	HUINT32		ulItem_addr = 0;
	HUINT8		*szTitle = NULL, *pDisable = NULL, *szImage = NULL;
	ONDK_Rect_t			stImgRect;
	ONDK_Size_t 		stImgSize = {0, 0};
	ONDKImageHandle 	hImgHandle = NULL;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDKFontHandle		hFontHandle;
	LIST_OBJECT			obj=(LIST_OBJECT)object;

	NxUiChListAddFavContents_t	*pstContents = NULL;

	pstContents = nx_chlistaddfav_GetContents();
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
	ulStatus = ONDK_GWM_List_GetStatus(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	lText_y_off = NX_PRISM_COMP_LIST_MULTI_SELECT_TEXT_OFFSET_Y;

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
		lScroll_w_off = (NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_X_MG + NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_W);
		lScroll_height = h;
	}
	w = w - lScroll_w_off;

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
					ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_COMP_LIST_MULTI_SELECT_CURSOR_MG, COL(C_G_Base01_100));
				}
				else
				{
					title_fontColor = COL(C_T_Base01);
				}
			}
			else
			{
				title_fontColor = COL(C_T_Base02);
			}
		}
		else
		{
			title_fontColor = COL(C_T_Whistle_100);
		}

		szTitle = (HUINT8 *)(((HUINT8 **)ulItem_addr)[lItem]);
		if (szTitle == NULL)
		{
			NX_APP_Error("ERROR!!! szTitle was NULL!!\n");
			return ONDK_RET_FAIL;
		}

		NX_APP_Info("[%d][%s]\n", lItem, szTitle);

		if (pstContents->pbSelectMark != NULL)
		{
			if (lItem == lCurrent)
			{
				if ((ulStatus & GUIOBJ_HILIGHTED) || (ulStatus & GUIOBJ_PUSHED))
				{
					szImage = (pstContents->pbSelectMark[lItem] == TRUE) ? (HUINT8*)RES_112_00_CHECK01_ON_C_PNG : (HUINT8*)RES_112_00_CHECK01_OFF_C_PNG;
				}
				else
				{
					szImage = (pstContents->pbSelectMark[lItem] == TRUE) ? (HUINT8*)RES_112_00_CHECK01_ON_N_PNG : (HUINT8*)RES_112_00_CHECK01_OFF_N_PNG;
				}
			}
			else
			{
				szImage = (pstContents->pbSelectMark[lItem] == TRUE) ? (HUINT8*)RES_112_00_CHECK01_ON_N_PNG : (HUINT8*)RES_112_00_CHECK01_OFF_N_PNG;
			}
			if (szImage != NULL)
			{
				hImgHandle = ONDK_IMG_CreateSelf(szImage);
				stImgSize = ONDK_IMG_GetSize(hImgHandle);
				x = x1 + NX_PRISM_COMP_LIST_MULTI_SELECT_IMG_OFFSET_X;
				y = y1 + lCursor_y_off;
				lImage_x_off = stImgSize.w;
				lImage_y_off = (lCellHeight - stImgSize.h)/2;
				lImage_y_off = (lImage_y_off < 0) ? 0 : lImage_y_off ;
				stImgRect = ONDK_Rect(x, y + lImage_y_off - 2, stImgSize.w, stImgSize.h);
				ONDK_GUICOM_IMG_Blit(screen, stImgRect, (HCHAR*)szImage);
				szImage = NULL;
			}
		}

		x = x1 + NX_PRISM_COMP_LIST_MULTI_SELECT_IMG_OFFSET_X + lImage_x_off + NX_PRISM_COMP_LIST_MULTI_SELECT_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off + lText_y_off;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y,
					(w - (NX_PRISM_COMP_LIST_MULTI_SELECT_IMG_OFFSET_X + lImage_x_off + NX_PRISM_COMP_LIST_MULTI_SELECT_TEXT_OFFSET_X)),
					title_fontColor, ONDKSTF_LEFT);

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
		if (lScrollBall_height < NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_H)
		{
			lScrollBall_height = NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_H;
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

		x = x1 + w + NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_X_MG;
		y = y1;

		/* Draw Scroll Bar */
		ONDK_DRAW_FillRectElement(screen, x + NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_BAR_X, y, NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_BAR_W, lScroll_height, COL(C_G_Whistle_20));

		/* Draw Scroll Bar Ball */
		stImgRect = ONDK_Rect(x, y + lScroll_y_offset, NX_PRISM_COMP_LIST_MULTI_SELECT_SCROLL_W, lScrollBall_height);
		ONDK_GUICOM_VBox(screen, stImgRect, RES_111_00_SCROLL_BAR_1U_PNG, RES_111_00_SCROLL_BAR_2M_PNG, RES_111_00_SCROLL_BAR_3D_PNG);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_chlistaddfav_SetUiObject(NxUiChListAddFavContents_t *pstContents)
{
	HUINT8			**pptextList = NULL;
	HINT32			lLineCnt = 0, lIndex = 0, lLineSpace = 0;
	HUINT32			ulTextListNum = 0, ulNumOfPerPage = 0;
	ONDK_Rect_t 	stRect;

	/* Draw Frame */
	stRect = ONDK_Rect(NX_PRISM_CHLIST_ADDFAV_FRAME_X, NX_PRISM_CHLIST_ADDFAV_FRAME_Y,
						NX_PRISM_CHLIST_ADDFAV_FRAME_W, NX_PRISM_CHLIST_ADDFAV_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_CHLIST_ADDFAV_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_CHLIST_ADDFAV_FRAME_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG);

	/* Draw Frame Top Line */
	stRect = ONDK_Rect(NX_PRISM_CHLIST_ADDFAV_LINE_TOP_X, NX_PRISM_CHLIST_ADDFAV_LINE_TOP_Y,
						NX_PRISM_CHLIST_ADDFAV_LINE_TOP_W, NX_PRISM_CHLIST_ADDFAV_LINE_TOP_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_CHLIST_ADDFAV_LINE_TOP_ID, &stRect, COL(C_G_Whistle_100));

	/* Draw Title */
	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22,
								ONDK_GetString(RES_MESG_5652_ID), NX_PRISM_CHLIST_ADDFAV_TITLE_W);
	stRect = ONDK_Rect(NX_PRISM_CHLIST_ADDFAV_TITLE_X, NX_PRISM_CHLIST_ADDFAV_TITLE_Y,
							NX_PRISM_CHLIST_ADDFAV_TITLE_W, (NX_PRISM_CHLIST_ADDFAV_TITLE_LS * lLineCnt) + NX_PRISM_FONT_VSPACE);
	ONDK_GWM_COM_Text_Create(NX_PRISM_CHLIST_ADDFAV_TITLE_ID, &stRect, (HCHAR*)ONDK_GetString(RES_MESG_5652_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_CHLIST_ADDFAV_TITLE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_CHLIST_ADDFAV_TITLE_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_CHLIST_ADDFAV_TITLE_ID, (TEXT_ALIGN_CENTER|TEXT_MULTILINED));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHLIST_ADDFAV_TITLE_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Draw List BG */
	stRect = ONDK_Rect(NX_PRISM_CHLIST_ADDFAV_LIST_BOX_X, NX_PRISM_CHLIST_ADDFAV_LIST_BOX_Y,
						NX_PRISM_CHLIST_ADDFAV_LIST_BOX_W, NX_PRISM_CHLIST_ADDFAV_LIST_BOX_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_CHLIST_ADDFAV_LIST_BOX_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_CHLIST_ADDFAV_LIST_BOX_ID, NX_PRISM_COMP_DrawFrameBox_A);

	/* Create List */
	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	ulNumOfPerPage = (ulTextListNum > NX_PRISM_CHLIST_ADDFAV_LIST_MAX) ? NX_PRISM_CHLIST_ADDFAV_LIST_MAX : ulTextListNum;
	pptextList = pstContents->szItemTextList;

	lIndex = 0;
	stRect = ONDK_Rect(NX_PRISM_CHLIST_ADDFAV_LIST_X, NX_PRISM_CHLIST_ADDFAV_LIST_Y,
						NX_PRISM_CHLIST_ADDFAV_LIST_W, NX_PRISM_CHLIST_ADDFAV_LIST_H);
	lLineSpace = NX_PRISM_CHLIST_ADDFAV_LIST_ITEM_H;
	ONDK_GWM_List_Create(NX_PRISM_CHLIST_ADDFAV_LIST_ID, &stRect, "NxAddFavList", lLineSpace,
							ulTextListNum, ulNumOfPerPage, (HUINT32)(pptextList));
	ONDK_GWM_List_SetCurrentIndex(NX_PRISM_CHLIST_ADDFAV_LIST_ID, lIndex);
	ONDK_GWM_List_SetFontHandle(NX_PRISM_CHLIST_ADDFAV_LIST_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_CHLIST_ADDFAV_LIST_ID, (GWM_ObjDraw_t)nx_chlistaddfav_DrawListMultiSelect_Cursor_A2);

	/* Draw Button Save */
	stRect = ONDK_Rect(NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_X, NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_Y,
						NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_W, NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_H);
	ONDK_GWM_COM_Button_Create(NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_ID, &stRect, (HCHAR*)ONDK_GetString(RES_SAVE_01_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_ID, NX_PRISM_COMP_DrawButton_A1);

	/* Draw Button Cancel */
	stRect = ONDK_Rect(NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_X, NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_Y,
						NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_W, NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_H);
	ONDK_GWM_COM_Button_Create(NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_ID, &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_ID, NX_PRISM_COMP_DrawButton_A1);

	ONDK_GWM_Obj_SetFocus(NX_PRISM_CHLIST_ADDFAV_LIST_ID);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlistaddfav_AddFavouriteGroupItem(NxUiChListAddFavContents_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	DxOperator_e	eOpType = eDxOPERATOR_None;
	DxService_t		*stSvcInfo = NULL;


	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(pstContents->ulSvcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! Not found DxService info - ulSvcUid(0x%x)!!\n", pstContents->ulSvcUid);
		return ONDK_RET_FAIL;
	}

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add here other Favourite list when you need to do
		default:
			nx_chlistaddfav_AddFavouriteGroupItemDefault(pstContents);
			eResult = nx_chlistaddfav_MakeFavouriteList(pstContents);
			break;
	}

	// TODO: Add Project config if you want to do

	return eResult;
}


static void		nx_chlistaddfav_AddFavouriteGroupItemDefault(NxUiChListAddFavContents_t *pstContents)
{
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite1);
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite2);
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite3);
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite4);
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite5);
}


static ONDK_Result_t	nx_chlistaddfav_InitData(NxUiChListAddFavContents_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}

	if(pstContents->pbSelectMark)
	{
		NX_APP_Free(pstContents->pbSelectMark);
		pstContents->pbSelectMark = NULL;
	}

	if(pstContents->pbOrgSelectMark)
	{
		NX_APP_Free(pstContents->pbOrgSelectMark);
		pstContents->pbOrgSelectMark = NULL;
	}

	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	NX_APP_Info("Init Info - eDispOptType:[%d]\n", pstContents->eDispOptType);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlistaddfav_MsgGwmCreate(NxUiChListAddFavContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_CHLIST_ADDFAV_WINDOW_X, NX_PRISM_CHLIST_ADDFAV_WINDOW_Y,
								NX_PRISM_CHLIST_ADDFAV_WINDOW_W, NX_PRISM_CHLIST_ADDFAV_WINDOW_H);

	HxSTD_MemSet(pstContents, 0x00, sizeof(NxUiChListAddFavContents_t));

	pstContents->szItemTextList = NULL;
	pstContents->pbSelectMark = NULL;
	pstContents->pbOrgSelectMark = NULL;

	// Init Item Data
	eResult = nx_chlistaddfav_InitData(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	pstContents->eDispOptType = (NX_UI_CallerType_t)p1;
	pstContents->ulSvcUid = (HUINT32)p2;
	pstContents->eCurGroup = (NX_ChListGroup_t)p3;

	eResult = nx_chlistaddfav_AddFavouriteGroupItem(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	nx_chlistaddfav_SetUiObject(pstContents);

	return	ONDK_RET_OK;
}



static ONDK_Result_t	nx_chlistaddfav_MsgGwmDestroy(NxUiChListAddFavContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_chlistaddfav_InitData(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlistaddfav_MsgGwmKeyDown(NxUiChListAddFavContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulSvcUid;
	NX_ChListGroup_t		eCurGroup;
	NX_UI_CallerType_t		eDispOptType;

	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
	switch (p1)
	{
		case KEY_GUIDE:
		case KEY_MENU:
		case KEY_STANDBY:
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_BACK:
			ulSvcUid = pstContents->ulSvcUid;
			eCurGroup = pstContents->eCurGroup;
			eDispOptType = pstContents->eDispOptType;

			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_CHANNELLIST_OPT_Proc", APP_DEFAULT_PRIORITY,
					NX_PRISM_CHANNELLIST_OPT_Proc, 0, eDispOptType, ulSvcUid, eCurGroup);
			return ONDK_MESSAGE_BREAK;

		/* 먹어버릴 키들 추가 */
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
		case KEY_RED:
		case KEY_GREEN:
		case KEY_YELLOW:
		case KEY_BLUE:
		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
		case KEY_OPT:
			NX_PRISM_NotAvailable_Show();
			return ONDK_MESSAGE_BREAK;

		case KEY_CH_PLUS:
			p1 = KEY_PAGEUP;
			break;

		case KEY_CH_MINUS:
			p1 = KEY_PAGEDOWN;
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlistaddfav_MsgGwmClickMsg(NxUiChListAddFavContents_t *pstContents, HINT32 p1)
{
	HINT32		lObjId = p1;

	switch (lObjId)
	{
		case NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_ID:
			NX_APP_Info("NX_PRISM_CHLIST_ADDFAV_BTN_SAVE_ID\n");
			nx_chlistaddfav_SaveChangeData(pstContents);
			break;

		case NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_ID:
			NX_APP_Info("NX_PRISM_CHLIST_ADDFAV_BTN_CANCEL_ID\n");
			ONDK_GWM_APP_Destroy(0);
			break;

		default:
			NX_APP_Error("Error!!! Unknow OBJ ID:(0x%x)\n", lObjId);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_CHANNELLIST_ADDFAV_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32				lObjID = -1, lListItemIdx = 0;
	HERROR				hError = ERR_FAIL;
	ONDK_Result_t		eResult = ONDK_RET_OK;
	NxUiChListAddFavContents_t	*pstContents = NULL;

	pstContents = nx_chlistaddfav_GetContents();
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE\n");
			eResult = nx_chlistaddfav_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_chlistaddfav_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY\n");
			eResult = nx_chlistaddfav_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_LIST_SELECTED:
			NX_APP_Message("MSG_GWM_LIST_SELECTED\n");
			lObjID = p1;
			if (lObjID == NX_PRISM_CHLIST_ADDFAV_LIST_ID)
			{
				lListItemIdx = p2;
				if (lListItemIdx < (HINT32)pstContents->stOptList.ulOptItemNum)
				{
					hError = nx_chlistaddfav_CheckAccessLimited(pstContents, lListItemIdx);
					if (hError == ERR_OK)
					{
						pstContents->pbSelectMark[lListItemIdx] = (pstContents->pbSelectMark[lListItemIdx] == TRUE) ? FALSE : TRUE ;
						ONDK_GWM_APP_InvalidateObject(NX_PRISM_CHLIST_ADDFAV_LIST_ID);
					}
					else
					{
						nx_chlistaddfav_CreateResponseDialouge((HUINT8*)RES_MESG_099_ID);
					}
				}
				return ONDK_MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED\n");
			eResult = nx_chlistaddfav_MsgGwmClickMsg(pstContents, p1);
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
	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

