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
 * @file	  		nx_prism_menu_search_option.c
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
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/
//#define	NX_PRISM_MENU_SEARCH_OPT_EXPIRE_TIMER_ID			(NX_PRISM_MENU_SEARCH_OPT_ITEM_ID + 1)
#define	NX_PRISM_MENU_SEARCH_OPT_EXPIRE_TIME				(30 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_MENU_SEARCH_OPT_WINDOW_X					310
#define	NX_PRISM_MENU_SEARCH_OPT_WINDOW_Y					369
#define	NX_PRISM_MENU_SEARCH_OPT_WINDOW_W					660
#define	NX_PRISM_MENU_SEARCH_OPT_WINDOW_H					351

#define	NX_PRISM_MENU_SEARCH_OPT_FRAME_X					0
#define	NX_PRISM_MENU_SEARCH_OPT_FRAME_Y					0
#define	NX_PRISM_MENU_SEARCH_OPT_FRAME_W					NX_PRISM_MENU_SEARCH_OPT_WINDOW_W
#define	NX_PRISM_MENU_SEARCH_OPT_FRAME_H					NX_PRISM_MENU_SEARCH_OPT_WINDOW_H

#define	NX_PRISM_MENU_SEARCH_OPT_TTL_TEXT_X					30
#define	NX_PRISM_MENU_SEARCH_OPT_TTL_TEXT_Y					(46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_MENU_SEARCH_OPT_TTL_TEXT_W					600
#define	NX_PRISM_MENU_SEARCH_OPT_TTL_TEXT_H					(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_MENU_SEARCH_OPT_ITEM_LS					55
#define	NX_PRISM_MENU_SEARCH_OPT_ITEM_X						30
#define	NX_PRISM_MENU_SEARCH_OPT_ITEM_Y						76
#define	NX_PRISM_MENU_SEARCH_OPT_ITEM_W						(600 + NX_PRISM_UI_OPT_MENU_ICON_MG)

#define	NX_PRISM_MENU_SEARCH_OPT_BTN_OK_X					103
#define	NX_PRISM_MENU_SEARCH_OPT_BTN_OK_Y					261
#define	NX_PRISM_MENU_SEARCH_OPT_BTN_OK_W					220
#define	NX_PRISM_MENU_SEARCH_OPT_BTN_OK_H					50

#define	NX_PRISM_MENU_SEARCH_OPT_BTN_CANCEL_X				338
#define	NX_PRISM_MENU_SEARCH_OPT_BTN_CANCEL_Y				261
#define	NX_PRISM_MENU_SEARCH_OPT_BTN_CANCEL_W				220
#define	NX_PRISM_MENU_SEARCH_OPT_BTN_CANCEL_H				50

#define	NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_MAX_LINE		5
#define	NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_CELL_HEIGHT	55
#define	NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WIDTH		290
#define	NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_HEIGHT		(NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_CELL_HEIGHT * NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_MAX_LINE)
#define	NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_X			(NX_PRISM_MENU_SEARCH_OPT_WINDOW_X + NX_PRISM_MENU_SEARCH_OPT_ITEM_X + NX_PRISM_MENU_SEARCH_OPT_ITEM_W - NX_PRISM_UI_OPT_MENU_ICON_X_TAB)
#define	NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_Y			(NX_PRISM_MENU_SEARCH_OPT_WINDOW_Y + NX_PRISM_MENU_SEARCH_OPT_ITEM_Y)

#define	NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WND_X		NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_X
#define	NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WND_Y		NX_PRISM_MENU_SEARCH_OPT_WINDOW_Y
#define	NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WND_W		(NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WIDTH)
#define	NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WND_H		NX_PRISM_MENU_SEARCH_OPT_WINDOW_H


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSearchOptItem_NetworkSearch,
	eNxSearchOptItem_ScrambledType,
	eNxSearchOptItem_ChannelType,
	eNxSearchOptItem_BTN_Ok,
	eNxSearchOptItem_BTN_Cancel,
	eNxSearchOptItem_EXPIRE_TIMER_ID,
	eNxSearchOptItem_FRAME_ID,
	eNxSearchOptItem_TTLTEXT_ID,
	eNxSearchOptItem_MAX
} NxSearchOptItem_t;


typedef struct tag_NX_SEARCHOPT_CONTENT
{
	HUINT32						ulCurFocus;
	HUINT32						ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	NX_SCANOPTIONS_t			stCurScanOpt;
} Nx_SearchOptContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_SearchOptContent_t			s_stOptContent;
static Nx_PrismListDialog_Content_t		s_stSearchPopupListOptContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_SearchOpt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SearchOpt_DestroyListPopup(void);
static void 			nx_SearchOpt_CreateListPopup(Nx_SearchOptContent_t *pstContents, HINT32 ulObjId);

static inline Nx_SearchOptContent_t* nx_SearchOpt_GetContents(void);
static HCHAR*			nx_SearchOpt_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_SearchOpt_GetMenuItemIdFormObjId(Nx_SearchOptContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_SearchOpt_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_SearchOpt_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_SearchOpt_GetItemSupport(NxSearchOptItem_t eNxMenuLanguageItem);
static HBOOL			nx_SearchOpt_GetItemiDisable(NxSearchOptItem_t eNxMenuLanguageItem);
static void				nx_SearchOpt_FindNextFocus(Nx_SearchOptContent_t *pstContents, HBOOL bUp);

static ONDK_Result_t	nx_SearchOpt_SaveChangeData(Nx_SearchOptContent_t *pstContents);
static ONDK_Result_t	nx_SearchOpt_UpdateNetworkSearch(Nx_SearchOptContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_SearchOpt_UpdateScrambledType(Nx_SearchOptContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_SearchOpt_UpdateChannelType(Nx_SearchOptContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_SearchOpt_UpdateSubItem(Nx_SearchOptContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_SearchOpt_ChangedSubItemValue(Nx_SearchOptContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_SearchOpt_AddItemNetworkSearch(Nx_SearchOptContent_t *pstContents);
static ONDK_Result_t	nx_SearchOpt_AddItemScrambledType(Nx_SearchOptContent_t *pstContents);
static ONDK_Result_t	nx_SearchOpt_AddItemChannelType(Nx_SearchOptContent_t *pstContents);
static ONDK_Result_t	nx_SearchOpt_LoadItemsValue(Nx_SearchOptContent_t *pstContents);
static ONDK_Result_t	nx_SearchOpt_InitSettingsItems(Nx_SearchOptContent_t *pstContents);

static ONDK_Result_t	nx_SearchOpt_SetUiObject(Nx_SearchOptContent_t *pstContents);

static ONDK_Result_t	nx_SearchOpt_MsgGwmCreate(Nx_SearchOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_SearchOpt_MsgGwmDestroy(Nx_SearchOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SearchOpt_MsgGwmHapiMsg(Nx_SearchOptContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SearchOpt_MsgGwmClickMsg(Nx_SearchOptContent_t *pstContents, HINT32 lMessage, HINT32 p1);
static ONDK_Result_t	nx_SearchOpt_MsgGwmKeyDown(Nx_SearchOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_SearchOpt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stSearchPopupListOptContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_SearchOpt_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_SearchOpt_DisplayListPopup);
}


static void		nx_SearchOpt_CreateListPopup(Nx_SearchOptContent_t *pstContents, HINT32 ulObjId)
{
	HUINT32			ulTextListNum = 0, ulIndex = 0, ulAttribute = 0;
	HUINT8			**pptextList = NULL;
	ONDK_Rect_t		stRect;
	Nx_SettingsLeafSubItem_t  *psubItem = NULL;

	/* Get subitem matched nObjId */
	psubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, ulObjId);
	if (psubItem == NULL)
	{
		NX_APP_Error("Invalid ulObjId Id : [0x%x]\n", ulObjId);
		return;
	}

	pptextList = psubItem->szItemTextList;
	ulTextListNum = psubItem->ulNumOfItemTextList;
	ulIndex	= psubItem->ulIdxOfTextList;

	stRect = ONDK_Rect(NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_X, NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_Y,
				NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WIDTH, NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_HEIGHT);

	ulAttribute = (eListDlg_OSD_USER|eListDlg_OSD_BG|eListDlg_SubItemApp);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stSearchPopupListOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_CELL_HEIGHT, ulAttribute,
											ulTextListNum, NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_MAX_LINE, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stSearchPopupListOptContents, NX_PRISM_FONT_SIZE_22);

	if (ulAttribute & eListDlg_OSD_USER)
	{
		stRect = ONDK_Rect(NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WND_X, NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WND_Y,
					NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WND_W, NX_PRISM_MENU_SEARCH_OPT_SUBITEM_POPUP_WND_H);
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stSearchPopupListOptContents, &stRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_SearchOpt_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_SearchOpt_DisplayListPopup, 0, 0, 0, 0);
}


static inline Nx_SearchOptContent_t* nx_SearchOpt_GetContents(void)
{
	return	&s_stOptContent;
}


static HCHAR*	nx_SearchOpt_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_SearchOpt_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSearchOptItem_NetworkSearch:
			return	RES_NETWORK_SEARCH_01_ID;

		case eNxSearchOptItem_ScrambledType:
			return	RES_SCRAMBLED_TYPE_ID;

		case eNxSearchOptItem_ChannelType:
			return	RES_CHANNEL_TYPE_ID;

		default:
			NX_APP_Error("Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_SearchOpt_GetMenuItemIdFormObjId(Nx_SearchOptContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_SearchOpt_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSearchOptItem_NetworkSearch:
			*pulMenuItemId = NX_MENUITEM_ID_SET_OFF_ON;
			break;

		case eNxSearchOptItem_ScrambledType:
			*pulMenuItemId = NX_MENUITEM_ID_SEARCH_OPTION_SCRAMBLEDTYPE;
			break;

		case eNxSearchOptItem_ChannelType:
			*pulMenuItemId = NX_MENUITEM_ID_SEARCH_OPTION_CHANNELTYPE;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static HINT32	nx_SearchOpt_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_SearchOption);

	if ((lItemId < 0) || (lItemId >= eNxSearchOptItem_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_SearchOpt_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSearchOptItem_MAX))
	{
		return 0;
	}

	return	(eNxSetting_SearchOption | lItemId);
}


static HBOOL	nx_SearchOpt_GetItemSupport(NxSearchOptItem_t eNxMenuLanguageItem)
{
	HBOOL	bSupport = FALSE;

	switch (eNxMenuLanguageItem)
	{
		case eNxSearchOptItem_NetworkSearch:
			bSupport = TRUE;
			break;
		case eNxSearchOptItem_ScrambledType:
			bSupport = TRUE;
			break;
		case eNxSearchOptItem_ChannelType:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_SearchOpt_GetItemiDisable(NxSearchOptItem_t eNxMenuLanguageItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxMenuLanguageItem)
	{
		case eNxSearchOptItem_NetworkSearch:
			bDisable = FALSE;
			break;
		case eNxSearchOptItem_ScrambledType:
			bDisable = FALSE;
			break;
		case eNxSearchOptItem_ChannelType:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_SearchOpt_FindNextFocus(Nx_SearchOptContent_t *pstContents, HBOOL bUp)
{
	HUINT32			i = 0, ulSubItemCnt = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);

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


static ONDK_Result_t	nx_SearchOpt_SaveChangeData(Nx_SearchOptContent_t *pstContents)
{
	NX_SEARCH_SetSearchOption(pstContents->stCurScanOpt);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_UpdateNetworkSearch(Nx_SearchOptContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->stCurScanOpt.enableNetworkScan != (HBOOL)pSubItem->lSubItemValue)
	{
		pstContents->stCurScanOpt.enableNetworkScan = (HBOOL)pSubItem->lSubItemValue;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_UpdateScrambledType(Nx_SearchOptContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->stCurScanOpt.casType != (DxCasType_b)pSubItem->lSubItemValue)
	{
		pstContents->stCurScanOpt.casType = (DxCasType_b)pSubItem->lSubItemValue;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_UpdateChannelType(Nx_SearchOptContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->stCurScanOpt.channelType != (DxSvcType_e)pSubItem->lSubItemValue)
	{
		pstContents->stCurScanOpt.channelType = (DxSvcType_e)pSubItem->lSubItemValue;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_UpdateSubItem(Nx_SearchOptContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_SearchOpt_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSearchOptItem_ScrambledType:
			nx_SearchOpt_UpdateScrambledType(pstContents, lObjId);
			break;

		case eNxSearchOptItem_NetworkSearch:
			nx_SearchOpt_UpdateNetworkSearch(pstContents, lObjId);
			break;

		case eNxSearchOptItem_ChannelType:
			nx_SearchOpt_UpdateChannelType(pstContents, lObjId);
			break;

		default:
			NX_APP_Error("Error!! Invalid Item Not Support lObjId:(0x%x), lItemIdx(%d)\n", lObjId, lItemIdx);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_ChangedSubItemValue(Nx_SearchOptContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_SearchOpt_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSearchOptItem_ScrambledType:
		case eNxSearchOptItem_NetworkSearch:
		case eNxSearchOptItem_ChannelType:
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
			nx_SearchOpt_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

			/* Update value */
			nx_SearchOpt_UpdateSubItem(pstContents, lObjId);
			break;

		default:
			NX_APP_Error("Error!! Invalid Item Not Support lObjId:(0x%x), lItemIdx(%d)\n", lObjId, lItemIdx);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_AddItemNetworkSearch(Nx_SearchOptContent_t *pstContents)
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
	bSupport = nx_SearchOpt_GetItemSupport(eNxSearchOptItem_NetworkSearch);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSearchOptItem_NetworkSearch(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_SearchOpt_ConvertItemIdxToObjId(eNxSearchOptItem_NetworkSearch);

	// Check Disable Item
	bDisable = nx_SearchOpt_GetItemiDisable(eNxSearchOptItem_NetworkSearch);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_SearchOpt_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_MENU_SEARCH_OPT_ITEM_X;
	stSubItem.rect.y = NX_PRISM_MENU_SEARCH_OPT_ITEM_Y + (ulSubItemCnt * NX_PRISM_MENU_SEARCH_OPT_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_MENU_SEARCH_OPT_ITEM_W;
	stSubItem.rect.h = NX_PRISM_MENU_SEARCH_OPT_ITEM_LS;

	/* Count SubItems */
	nx_SearchOpt_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!! NX_PRISM_MENU_Menuitem_CountSubitems() eResult(0x%x), ulItems:(%u)\n", eResult, ulItems);
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
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->stCurScanOpt.enableNetworkScan, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Not found menu item index(%d), stCurScanOpt.enableNetworkScan(%d)\n", ulListIdx, pstContents->stCurScanOpt.enableNetworkScan);
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->stCurScanOpt.enableNetworkScan;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_AddItemScrambledType(Nx_SearchOptContent_t *pstContents)
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
	bSupport = nx_SearchOpt_GetItemSupport(eNxSearchOptItem_ScrambledType);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSearchOptItem_ScrambledType(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_SearchOpt_ConvertItemIdxToObjId(eNxSearchOptItem_ScrambledType);

	// Check bDisable Item
	bDisable = nx_SearchOpt_GetItemiDisable(eNxSearchOptItem_ScrambledType);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_SearchOpt_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_MENU_SEARCH_OPT_ITEM_X;
	stSubItem.rect.y = NX_PRISM_MENU_SEARCH_OPT_ITEM_Y + (ulSubItemCnt * NX_PRISM_MENU_SEARCH_OPT_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_MENU_SEARCH_OPT_ITEM_W;
	stSubItem.rect.h = NX_PRISM_MENU_SEARCH_OPT_ITEM_LS;

	/* Count SubItems */
	nx_SearchOpt_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
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
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->stCurScanOpt.casType, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Not found menu item index(%d), pstContents->stCurScanOpt.casType(0x%x)\n", ulListIdx, pstContents->stCurScanOpt.casType);
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->stCurScanOpt.casType;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_AddItemChannelType(Nx_SearchOptContent_t *pstContents)
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
	bSupport = nx_SearchOpt_GetItemSupport(eNxSearchOptItem_ChannelType);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSearchOptItem_ChannelType(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_SearchOpt_ConvertItemIdxToObjId(eNxSearchOptItem_ChannelType);

	// Check bDisable Item
	bDisable = nx_SearchOpt_GetItemiDisable(eNxSearchOptItem_ChannelType);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_SearchOpt_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_MENU_SEARCH_OPT_ITEM_X;
	stSubItem.rect.y = NX_PRISM_MENU_SEARCH_OPT_ITEM_Y + (ulSubItemCnt * NX_PRISM_MENU_SEARCH_OPT_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_MENU_SEARCH_OPT_ITEM_W;
	stSubItem.rect.h = NX_PRISM_MENU_SEARCH_OPT_ITEM_LS;

	/* Count SubItems */
	nx_SearchOpt_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
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
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->stCurScanOpt.channelType, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Not found menu item index(%d), channelType(0x%x)\n", ulListIdx, pstContents->stCurScanOpt.channelType);
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->stCurScanOpt.channelType;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_LoadItemsValue(Nx_SearchOptContent_t *pstContents)
{
	NX_SCANOPTIONS_t 	stScanOption;

	HxSTD_memset(&stScanOption, 0x00, sizeof(NX_SCANOPTIONS_t));
	NX_SEARCH_GetSearchOption(&stScanOption);

	HxSTD_memcpy(&pstContents->stCurScanOpt, &stScanOption, sizeof(NX_SCANOPTIONS_t));

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_SearchOpt_InitSettingsItems(Nx_SearchOptContent_t *pstContents)
{
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

	// Load Menu Item Value
	nx_SearchOpt_LoadItemsValue(pstContents);

	nx_SearchOpt_AddItemNetworkSearch(pstContents);
	nx_SearchOpt_AddItemScrambledType(pstContents);
	nx_SearchOpt_AddItemChannelType(pstContents);

	// Default Function
	//nx_SearchOpt_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_SetUiObject(Nx_SearchOptContent_t *pstContents)
{
	HINT32				lObjId = -1;
	ONDK_Rect_t 		stRect;

	/* Draw BG Frame */
	stRect = ONDK_Rect(NX_PRISM_MENU_SEARCH_OPT_FRAME_X, NX_PRISM_MENU_SEARCH_OPT_FRAME_Y,
						NX_PRISM_MENU_SEARCH_OPT_FRAME_W, NX_PRISM_MENU_SEARCH_OPT_FRAME_H);
	lObjId = nx_SearchOpt_ConvertItemIdxToObjId(eNxSearchOptItem_FRAME_ID);
	ONDK_GWM_COM_Rect_Create(lObjId, &stRect, COL(C_G_Bang_90));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawFrameDialogueWindowBG_TopLine);

	/* Draw Title TEXT */
	stRect = ONDK_Rect(NX_PRISM_MENU_SEARCH_OPT_TTL_TEXT_X, NX_PRISM_MENU_SEARCH_OPT_TTL_TEXT_Y,
						NX_PRISM_MENU_SEARCH_OPT_TTL_TEXT_W, NX_PRISM_MENU_SEARCH_OPT_TTL_TEXT_H);
	lObjId = nx_SearchOpt_ConvertItemIdxToObjId(eNxSearchOptItem_TTLTEXT_ID);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_SEARCH_OPTION_ID));
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Draw Main Item */
	NX_PRISM_COMP_OPT_InitMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_OPT_DrawCursor_A1_Arrow);
	// Set Focus
	ONDK_GWM_Obj_SetFocus(pstContents->subItems[pstContents->ulCurFocus].ulId);

	/* Draw Button Item */
	stRect = ONDK_Rect(NX_PRISM_MENU_SEARCH_OPT_BTN_OK_X, NX_PRISM_MENU_SEARCH_OPT_BTN_OK_Y,
						NX_PRISM_MENU_SEARCH_OPT_BTN_OK_W, NX_PRISM_MENU_SEARCH_OPT_BTN_OK_H);
	lObjId = nx_SearchOpt_ConvertItemIdxToObjId(eNxSearchOptItem_BTN_Ok);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_OK_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_OPT_DrawButton_A1);

	stRect = ONDK_Rect(NX_PRISM_MENU_SEARCH_OPT_BTN_CANCEL_X, NX_PRISM_MENU_SEARCH_OPT_BTN_CANCEL_Y,
						NX_PRISM_MENU_SEARCH_OPT_BTN_CANCEL_W, NX_PRISM_MENU_SEARCH_OPT_BTN_CANCEL_H);
	lObjId = nx_SearchOpt_ConvertItemIdxToObjId(eNxSearchOptItem_BTN_Cancel);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_OPT_DrawButton_A1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_MsgGwmCreate(Nx_SearchOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_MENU_SEARCH_OPT_WINDOW_X, NX_PRISM_MENU_SEARCH_OPT_WINDOW_Y, NX_PRISM_MENU_SEARCH_OPT_WINDOW_W, NX_PRISM_MENU_SEARCH_OPT_WINDOW_H);

	// Init Menu Item
	eResult = nx_SearchOpt_InitSettingsItems(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! nx_SearchOpt_InitSettingsItems() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	// Draw Menu Item
	nx_SearchOpt_SetUiObject(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_MsgGwmDestroy(Nx_SearchOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! pstContents == NULL!! \n");
		return ONDK_RET_FAIL;
	}

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_SearchOptContent_t));
	nx_SearchOpt_DestroyListPopup();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_SearchOpt_MsgGwmHapiMsg(Nx_SearchOptContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_SearchOpt_MsgGwmClickMsg(Nx_SearchOptContent_t *pstContents, HINT32 lMessage, HINT32 p1)
{
	HINT32			lObjId = p1, lItemIdx = -1;

	lItemIdx = nx_SearchOpt_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSearchOptItem_NetworkSearch:
		case eNxSearchOptItem_ScrambledType:
		case eNxSearchOptItem_ChannelType:
			nx_SearchOpt_CreateListPopup(pstContents, p1);
			break;
		case eNxSearchOptItem_BTN_Ok:
			if (lMessage != MSG_GWM_SCROLLRIGHT)
			{
				nx_SearchOpt_SaveChangeData(pstContents);
				ONDK_GWM_APP_Destroy(0);
			}
			break;
		case eNxSearchOptItem_BTN_Cancel:
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


static ONDK_Result_t	nx_SearchOpt_MsgGwmKeyDown(Nx_SearchOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		case KEY_OPT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;
#if 0
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;
#endif

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

		case KEY_STANDBY:
		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy (0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Search_Option_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_SearchOptContent_t	*pstContents = NULL;

	pstContents = nx_SearchOpt_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_SearchOpt_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_SearchOpt_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			eResult = nx_SearchOpt_MsgGwmClickMsg(pstContents, lMessage, p1);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			//nx_SearchOpt_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			//nx_SearchOpt_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_SearchOpt_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_SearchOpt_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_SearchOpt_ChangedSubItemValue(pstContents, p1);
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

