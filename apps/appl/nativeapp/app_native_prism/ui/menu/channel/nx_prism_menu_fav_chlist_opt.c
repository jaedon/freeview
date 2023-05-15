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
 * @file	  		nx_prism_menu_fav_chlist_opt.c
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
#include <nx_port.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_PRISM_UI_MENU_SEARCH_OPT_BG_X						0
#define	NX_PRISM_UI_MENU_SEARCH_OPT_BG_Y						0
#define	NX_PRISM_UI_MENU_SEARCH_OPT_BG_W						660
#define	NX_PRISM_UI_MENU_SEARCH_OPT_BG_H						720

#define	NX_PRISM_UI_MENU_SEARCH_OPT_OPT_X						200
#define	NX_PRISM_UI_MENU_SEARCH_OPT_OPT_Y						674
#define	NX_PRISM_UI_MENU_SEARCH_OPT_OPT_W						400
#define	NX_PRISM_UI_MENU_SEARCH_OPT_OPT_H						30

#define	NX_PRISM_UI_MENU_FAV_LIST_OPT_LIST_Y2				694

#define NX_PRISM_UI_MENU_FAV_CHLIST_OPT_CHANGE_GROUP_NUM		5
/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum {
	//eNxSettingsItem_SatOpt_Hidden = 0,
	eNxSettingsItem_FavOpt_Text = 0,
	eNxSettingsItem_FavOpt_Opt,
	eNxSettingsItem_FavOpt_FrameBG,
	eNxSettingsItem_FavOpt_MAX
} NxSettingsItem_FavChListOptList_t;

typedef enum {
	eNxSettingsItem_FavOptGrpValue_AddFav = 0,
	eNxSettingsItem_FavOptGrpValue_Group,
	eNxSettingsItem_FavOptGrpValue_SelectAll,
	eNxSettingsItem_FavOptGrpValue_DeSelectAll,
	eNxSettingsItem_FavOptGrpValue_MAX
} NxSettingsItem_FavChListGrpOptValue_t;

typedef enum {
	eNxSettingsItem_FavOptFavValue_Remove = 0,
	eNxSettingsItem_FavOptFavValue_Move,
	eNxSettingsItem_FavOptFavValue_ChangeFav,
	eNxSettingsItem_FavOptFavValue_RenameFav,
	eNxSettingsItem_FavOptFavValue_SelectAll,
	eNxSettingsItem_FavOptFavValue_DeSelectAll,
	eNxSettingsItem_FavOptFavValue_MAX
} NxSettingsItem_FavChListFavOptValue_t;

typedef enum {
	eNxSettingsItem_FavOptItem_ALL = 0,
	eNxSettingsItem_FavOptItem_AddFav,
	eNxSettingsItem_FavOptItem_Group,
	eNxSettingsItem_FavOptItem_SelectAll,
	eNxSettingsItem_FavOptItem_DeSelectAll,
	eNxSettingsItem_FavOptItem_Remove,
	eNxSettingsItem_FavOptItem_Move,
	eNxSettingsItem_FavOptItem_ChangeFav,
	eNxSettingsItem_FavOptItem_RenameFav,
	eNxSettingsItem_FavOptItem_MAX
} NxSettingsItem_FavChListOptItem_t;

typedef struct
{
	NxSettingsItem_FavChListOptItem_t	eOptItem;
	eNxUiUtil_OptItem_t			eCurOptItemValue;

	// Option Item
	HUINT8					**szItemTextList;
	HUINT8					**szImageNItemTextList;
	HUINT8					**szImageCItemTextList;
	NX_Util_OPTIONList_t	stOptList;

	HINT32					nFocusViewID;
	NX_ChListGroup_t 		eCurGroup;
	NX_ChListGroup_t 		eCurFav;
} Nx_MenuFavChListOptContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuFavChListOptContent_t		s_stFavChListOptContent;
static Nx_PrismListDialog_Content_t		s_stPopupListSearchOptContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuFavChList_Opt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuFavChList_Opt_DestroyListPopup(void);
static void 			nx_menuFavChList_Opt_CreateListPopup(Nx_MenuFavChListOptContent_t *pstContents);

static void				nx_menuFavChList_Opt_CreateChangeGroupListPopup(Nx_MenuFavChListOptContent_t *pstContents, NX_ChListGroup_t eCurGroup);

static ONDK_Result_t	nx_menuFavChList_Opt_DoChangeGroup(Nx_MenuFavChListOptContent_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_menuFavChList_Opt_InitChangeGroupData(Nx_MenuFavChListOptContent_t *pstContents);
static void				nx_menuFavChList_Opt_InitChangeGroupDataDefault(Nx_MenuFavChListOptContent_t *pstContents);

static ONDK_Result_t	nx_menuFavChList_Opt_DoChangeFavourite(Nx_MenuFavChListOptContent_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_menuFavChList_Opt_InitChangeFavouriteData(Nx_MenuFavChListOptContent_t *pstContents);
static void				nx_menuFavChList_Opt_InitChangeFavouriteDataDefault(Nx_MenuFavChListOptContent_t *pstContents);

static inline Nx_MenuFavChListOptContent_t* nx_menuFavChList_Opt_GetContents(void);
static HINT32			nx_menuFavChList_Opt_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuFavChList_Opt_ConvertItemIdxToObjId(HINT32 lItemId);

static ONDK_Result_t	nx_menuFavChList_Opt_ChangedSubItemValue(Nx_MenuFavChListOptContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_menuFavChList_Opt_AddFavouriteOptionData(Nx_MenuFavChListOptContent_t *pstContents);
static ONDK_Result_t	nx_menuFavChList_Opt_InitSettingsItems(Nx_MenuFavChListOptContent_t *pstContents);
static ONDK_Result_t	nx_menuFavChList_Opt_ReCreateMenu(Nx_MenuFavChListOptContent_t *pstContents);

static ONDK_Result_t	nx_menuFavChList_Opt_DrawOption(Nx_MenuFavChListOptContent_t *pstContents);
static ONDK_Result_t	nx_menuFavChList_Opt_DrawDefaultUI(Nx_MenuFavChListOptContent_t *pstContents);

static ONDK_Result_t	nx_menuFavChList_Opt_MsgGwmCreate(Nx_MenuFavChListOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuFavChList_Opt_MsgGwmDestroy(Nx_MenuFavChListOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuFavChList_Opt_MsgGwmHapiMsg(Nx_MenuFavChListOptContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuFavChList_Opt_MsgGwmKeyDown(Nx_MenuFavChListOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_menuFavChList_Opt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListSearchOptContent, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuFavChList_Opt_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuFavChList_Opt_DisplayListPopup);
}


/******************************************************************************/
#define __Edit_Channellist_Option_Functions___________________________________________
/******************************************************************************/

static void		nx_menuFavChList_Opt_CreateListPopup(Nx_MenuFavChListOptContent_t *pstContents)
{
	HINT32		y = 0, h = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, i = 0;
	HUINT8		**pptextList = NULL, **ppImageListOfItem = NULL;
	ONDK_Rect_t		stRect;


	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Error("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
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

	ulIndex = 0;
	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == (HUINT32)pstContents->eCurOptItemValue)
		{
			ulIndex = i;
			break;
		}
	}

	// Set Text List
	if (pstContents->nFocusViewID == NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID)
	{
		h = (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT * eNxSettingsItem_FavOptGrpValue_MAX) + (NX_PRISM_UI_OPT_POPUP_CELL_MG * (eNxSettingsItem_FavOptGrpValue_MAX -1)) + NX_PRISM_UI_OPT_POPUP_POINTER_H;
	}
	else
	{
		h = (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT * eNxSettingsItem_FavOptFavValue_MAX) + (NX_PRISM_UI_OPT_POPUP_CELL_MG * (eNxSettingsItem_FavOptFavValue_MAX -1)) + NX_PRISM_UI_OPT_POPUP_POINTER_H;
	}
	y = NX_PRISM_UI_MENU_FAV_LIST_OPT_LIST_Y2 - h - NX_PRISM_UI_OPT_POPUP_POINTER_H;
	stRect = ONDK_Rect(NX_PRISM_UI_OPT_POPUP_LIST_R_X, y, NX_PRISM_UI_OPT_POPUP_LIST_WIDTH, h);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListSearchOptContent, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
										&stRect, (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT + NX_PRISM_UI_OPT_POPUP_CELL_MG),
										eListDlg_IMAGE_OptionRApp, ulTextListNum, ulTextListNum, ulIndex, (HUINT32)pptextList);

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

	NX_PRISM_COMP_ListDlg_SetImageList(&s_stPopupListSearchOptContent, (HUINT32)ppImageListOfItem);

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

	NX_PRISM_COMP_ListDlg_SetImageExtList(&s_stPopupListSearchOptContent, (HUINT32)ppImageListOfItem);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuFavChList_Opt_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuFavChList_Opt_DisplayListPopup, 0, 0, 0, 0);
}


static void				nx_menuFavChList_Opt_CreateChangeGroupListPopup(Nx_MenuFavChListOptContent_t *pstContents, NX_ChListGroup_t eCurGroup)
{
	HINT32		lLineCnt = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, i = 0, ulMaxListNum = 0;
	HUINT8		**pptextList = NULL;
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
	ulMaxListNum = (ulTextListNum > NX_PRISM_UI_MENU_FAV_CHLIST_OPT_CHANGE_GROUP_NUM) ? NX_PRISM_UI_MENU_FAV_CHLIST_OPT_CHANGE_GROUP_NUM : ulTextListNum;
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_SelectItemApp);
	stRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_List, lLineCnt, ulMaxListNum);
	if (ulAttribute & eListDlg_OSD_USER)
	{
		stListRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_OSD, lLineCnt, ulMaxListNum);
		stRect.x = stListRect.x + stRect.x;
		stRect.y = stListRect.y + stRect.y;
	}

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stPopupListSearchOptContent, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulMaxListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stPopupListSearchOptContent, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stPopupListSearchOptContent, ONDK_GetString(RES_MESG_5650_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stPopupListSearchOptContent, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuFavChList_Opt_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_menuFavChList_Opt_DisplayListPopup, 0, 0, 0, 0);
}


static ONDK_Result_t	nx_menuFavChList_Opt_DoChangeGroup(Nx_MenuFavChListOptContent_t *pstContents, HUINT32 ulCurIdx)
{
	NX_UI_CallerType_t						eDispOptType = eNX_UCT_Menu_FavChList;
	NxSettingsItem_FavChListOptItem_t		eCurOptionItem = eNxSettingsItem_FavOptItem_ALL;
	NX_ChListGroup_t 						eChGroupType = eNxChListGroup_TV;

	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	eChGroupType = (NX_ChListGroup_t)pstContents->stOptList.aeOptItemList[ulCurIdx];
	switch (eChGroupType)
	{
		case eNxChListGroup_TV:
		case eNxChListGroup_RADIO:
		case eNxChListGroup_HDTV:
		case eNxChListGroup_FTA:
		case eNxChListGroup_CAS:
			break;

		default:
			NX_APP_Info("Error!!! Not Support Group Item [0x%x] !!\n", eChGroupType);
			break;
	}

	// eDispOptType = pstContents->eDispOptType;
	eCurOptionItem = pstContents->eCurOptItemValue;
	ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eDispOptType, eCurOptionItem, eChGroupType);
	ONDK_GWM_APP_Destroy(0);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_Opt_InitChangeGroupData(Nx_MenuFavChListOptContent_t *pstContents)
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
	pstContents->eCurOptItemValue = eNxUtil_OptItem_Group;
	pstContents->eOptItem = eNxSettingsItem_FavOptItem_Group;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add Other Change Group for Operator if you need

		default:
			nx_menuFavChList_Opt_InitChangeGroupDataDefault(pstContents);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static void				nx_menuFavChList_Opt_InitChangeGroupDataDefault(Nx_MenuFavChListOptContent_t *pstContents)
{
	HUINT32			ulCount = 0;

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_TV);
	if (ulCount > 0)
	{
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_TV);
	}
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_RADIO);
	if (ulCount > 0)
	{
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_RADIO);
	}
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_HDTV);
	if (ulCount > 0)
	{
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_HDTV);
	}
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_FTA);
	if (ulCount > 0)
	{
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_FTA);
	}
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_CAS);
	if (ulCount > 0)
	{
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_CAS);
	}
}


static ONDK_Result_t	nx_menuFavChList_Opt_DoChangeFavourite(Nx_MenuFavChListOptContent_t *pstContents, HUINT32 ulCurIdx)
{
	NX_UI_CallerType_t						eDispOptType = eNX_UCT_Menu_FavChList;
	NxSettingsItem_FavChListOptItem_t		eCurOptionItem = eNxSettingsItem_FavOptItem_ALL;
	NX_ChListGroup_t 						eChGroupType = eNxChListGroup_TV;

	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	eChGroupType = (NX_ChListGroup_t)pstContents->stOptList.aeOptItemList[ulCurIdx];
	switch (eChGroupType)
	{
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

	// eDispOptType = pstContents->eDispOptType;
	eCurOptionItem = pstContents->eCurOptItemValue;
	ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eDispOptType, eCurOptionItem, eChGroupType);
	ONDK_GWM_APP_Destroy(0);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_Opt_InitChangeFavouriteData(Nx_MenuFavChListOptContent_t *pstContents)
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
	pstContents->eCurOptItemValue = eNxUtil_OptItem_ChangeFav;
	pstContents->eOptItem = eNxSettingsItem_FavOptItem_ChangeFav;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add Other Change Group for Operator if you need

		default:
			nx_menuFavChList_Opt_InitChangeFavouriteDataDefault(pstContents);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static void				nx_menuFavChList_Opt_InitChangeFavouriteDataDefault(Nx_MenuFavChListOptContent_t *pstContents)
{
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite1);
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite2);
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite3);
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite4);
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite5);
}


static inline Nx_MenuFavChListOptContent_t* nx_menuFavChList_Opt_GetContents(void)
{
	return	&s_stFavChListOptContent;
}


static HINT32	nx_menuFavChList_Opt_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_EditChListOption);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_FavOpt_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuFavChList_Opt_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_FavOpt_MAX))
	{
		return 0;
	}

	return	(eNxSetting_EditChListOption | lItemId);
}


static ONDK_Result_t	nx_menuFavChList_Opt_ChangedSubItemValue(Nx_MenuFavChListOptContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32		nObjId = -1;
	eNxUiUtil_OptItem_t		eCurOptionItem = eNxUtil_OptItem_None;

	switch (pstContents->eOptItem)
	{
		case eNxSettingsItem_FavOptItem_ALL:
			if (ulCurIdx >= pstContents->stOptList.ulOptItemNum)
			{
				NX_APP_Error("Error!!! Unknow Index - ulCurIdx(%d)!! \n", ulCurIdx);
				break;
			}
			eCurOptionItem = pstContents->stOptList.aeOptItemList[ulCurIdx];
			switch(eCurOptionItem)
			{
				case eNxUtil_OptItem_AddFav:
				case eNxUtil_OptItem_Remove:
				case eNxUtil_OptItem_Move:
				case eNxUtil_OptItem_RenameFav:
				case eNxUtil_OptItem_SelectAll:
				case eNxUtil_OptItem_DeSelectAll:
					ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, 0, eCurOptionItem, 0);
					ONDK_GWM_APP_Destroy(0);
					break;
				case eNxUtil_OptItem_ChangeFav:
				case eNxUtil_OptItem_Group:
					if (eCurOptionItem == eNxUtil_OptItem_ChangeFav)
					{
						nx_menuFavChList_Opt_InitChangeFavouriteData(pstContents);
						nx_menuFavChList_Opt_CreateChangeGroupListPopup(pstContents, pstContents->eCurFav);
					}
					else
					{
						nx_menuFavChList_Opt_InitChangeGroupData(pstContents);
						nx_menuFavChList_Opt_CreateChangeGroupListPopup(pstContents, pstContents->eCurGroup);
					}

					nObjId = nx_menuFavChList_Opt_ConvertItemIdxToObjId(eNxSettingsItem_FavOpt_FrameBG);
					if (nObjId > 0)
					{
						ONDK_GWM_Obj_Destroy(nObjId);
					}

					nObjId = nx_menuFavChList_Opt_ConvertItemIdxToObjId(eNxSettingsItem_FavOpt_Text);
					if (nObjId > 0)
					{
						ONDK_GWM_Obj_Destroy(nObjId);
					}

					nObjId = nx_menuFavChList_Opt_ConvertItemIdxToObjId(eNxSettingsItem_FavOpt_Opt);
					if (nObjId > 0)
					{
						ONDK_GWM_Obj_Destroy(nObjId);
					}
					break;
				default:
					NX_APP_Error("Error!!! Not Support option Item - (%d)!! \n", pstContents->eOptItem);
					return ONDK_RET_FAIL;
			}

			break;
		case eNxSettingsItem_FavOptItem_Group:
			nx_menuFavChList_Opt_DoChangeGroup(pstContents, ulCurIdx);
			break;
		case eNxSettingsItem_FavOptItem_ChangeFav:
			nx_menuFavChList_Opt_DoChangeFavourite(pstContents, ulCurIdx);
			break;
		default:
			NX_APP_Error("Error!!! Not Support option Item - (%d)!! \n", pstContents->eOptItem);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(ulCurIdx);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_Opt_AddFavouriteOptionData(Nx_MenuFavChListOptContent_t *pstContents)
{
	pstContents->eOptItem = eNxSettingsItem_FavOptItem_ALL;

	if (pstContents->nFocusViewID == NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID)
	{
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_AddFav);
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Group);
		pstContents->eCurOptItemValue = eNxUtil_OptItem_AddFav;
	}
	else
	{
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Remove);
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Move);
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_ChangeFav);
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_RenameFav);
		pstContents->eCurOptItemValue = eNxUtil_OptItem_Remove;
	}
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SelectAll);
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_DeSelectAll);

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_Opt_InitSettingsItems(Nx_MenuFavChListOptContent_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

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

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_Opt_ReCreateMenu(Nx_MenuFavChListOptContent_t *pstContents)
{
	// Init Menu Item
	nx_menuFavChList_Opt_InitSettingsItems(pstContents);

	// Search Channel Option
	nx_menuFavChList_Opt_AddFavouriteOptionData(pstContents);

	nx_menuFavChList_Opt_CreateListPopup(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_Opt_DrawOption(Nx_MenuFavChListOptContent_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	// Option
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SEARCH_OPT_OPT_X, NX_PRISM_UI_MENU_SEARCH_OPT_OPT_Y, NX_PRISM_UI_MENU_SEARCH_OPT_OPT_W, NX_PRISM_UI_MENU_SEARCH_OPT_OPT_H);
	lObjId = nx_menuFavChList_Opt_ConvertItemIdxToObjId(eNxSettingsItem_FavOpt_Opt);
	ONDK_GWM_COM_Rect_Create(lObjId, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawEdit_HelpR);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_Opt_DrawDefaultUI(Nx_MenuFavChListOptContent_t *pstContents)
{
	HUINT32			ulObjId = 0;
	ONDK_Rect_t		stRect;


	NX_COMMON_NOT_USED_PARAM(pstContents);

	// Draw BackGround
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SEARCH_OPT_BG_X, NX_PRISM_UI_MENU_SEARCH_OPT_BG_Y, NX_PRISM_UI_MENU_SEARCH_OPT_BG_W, NX_PRISM_UI_MENU_SEARCH_OPT_BG_H);
	ulObjId = (HUINT32)nx_menuFavChList_Opt_ConvertItemIdxToObjId(eNxSettingsItem_FavOpt_FrameBG);
	ONDK_GWM_COM_Rect_Create(ulObjId, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(ulObjId, NX_PRISM_COMP_DrawWindowOptionRight);

	// Draw Temp Object
	stRect = ONDK_Rect(0, 0, 0, 0);
	ulObjId = (HUINT32)nx_menuFavChList_Opt_ConvertItemIdxToObjId(eNxSettingsItem_FavOpt_Text);
	ONDK_GWM_COM_Button_Create(ulObjId, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(ulObjId, NX_PRISM_COMP_DrawWEmpty);
	ONDK_GWM_Obj_SetFocus(ulObjId);

	// Draw Option
	nx_menuFavChList_Opt_DrawOption(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_Opt_MsgGwmCreate(Nx_MenuFavChListOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_OPT_POPUP_R_X, NX_PRISM_UI_OPT_POPUP_R_Y, NX_PRISM_UI_OPT_POPUP_WIDTH, NX_PRISM_UI_OPT_POPUP_HEIGHT);

	// init Item Data
	pstContents->nFocusViewID = (HINT32)p1;
	pstContents->eCurGroup = (NX_ChListGroup_t)p2;
	pstContents->eCurFav = (NX_ChListGroup_t)p3;

	// Init Menu Item
	nx_menuFavChList_Opt_InitSettingsItems(pstContents);

	// Search Channel Option
	nx_menuFavChList_Opt_AddFavouriteOptionData(pstContents);

	// Draw Menu Item
	nx_menuFavChList_Opt_DrawDefaultUI(pstContents);

	nx_menuFavChList_Opt_CreateListPopup(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_Opt_MsgGwmDestroy(Nx_MenuFavChListOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	nx_menuFavChList_Opt_DestroyListPopup();

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
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuFavChListOptContent_t));

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_Opt_MsgGwmHapiMsg(Nx_MenuFavChListOptContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuFavChList_Opt_MsgGwmKeyDown(Nx_MenuFavChListOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_OPT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
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

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuFavChList_Opt_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t			NX_PRISM_MENU_FavChList_Option_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuFavChListOptContent_t	*pstContents = NULL;

	pstContents = nx_menuFavChList_Opt_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuFavChList_Opt_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuFavChList_Opt_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuFavChList_Opt_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuFavChList_Opt_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED\n");
			nx_menuFavChList_Opt_DestroyListPopup();
			eResult = nx_menuFavChList_Opt_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_APP_POPUP_LIST_DESTROYED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED!\n");
			if ((p1 == KEY_BACK) && (pstContents->eOptItem != eNxSettingsItem_FavOptItem_ALL))
			{
				nx_menuFavChList_Opt_DestroyListPopup();
				nx_menuFavChList_Opt_ReCreateMenu(pstContents);
			}
			else
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Menu)
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

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
