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
 * @file	  		nx_prism_play_subtitle_option.c
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
#define	NX_PRISM_LIVE_OPT_ITEM_ID					eMenuFrame_Window_LiveOpt

#define	NX_PRISM_LIVE_OPT_EXPIRE_TIMER_ID			(NX_PRISM_LIVE_OPT_ITEM_ID + 1)
#define	NX_PRISM_LIVE_OPT_EXPIRE_TIME				(3 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_LIVE_OPT_WINDOW_X					310
#define	NX_PRISM_LIVE_OPT_WINDOW_Y					314
#define	NX_PRISM_LIVE_OPT_WINDOW_W					660
#define	NX_PRISM_LIVE_OPT_WINDOW_H					406

#define	NX_PRISM_LIVE_OPT_FRAME_ID					(NX_PRISM_LIVE_OPT_ITEM_ID+2)
#define	NX_PRISM_LIVE_OPT_FRAME_X					0
#define	NX_PRISM_LIVE_OPT_FRAME_Y					0
#define	NX_PRISM_LIVE_OPT_FRAME_W					NX_PRISM_LIVE_OPT_WINDOW_W
#define	NX_PRISM_LIVE_OPT_FRAME_H					NX_PRISM_LIVE_OPT_WINDOW_H

#define	NX_PRISM_LIVE_OPT_TTL_TEXT_ID				(NX_PRISM_LIVE_OPT_ITEM_ID+3)
#define	NX_PRISM_LIVE_OPT_TTL_TEXT_X				30
#define	NX_PRISM_LIVE_OPT_TTL_TEXT_Y				(46 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_LIVE_OPT_TTL_TEXT_W				600
#define	NX_PRISM_LIVE_OPT_TTL_TEXT_H				(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_LIVE_OPT_MAINITEM_ID				(NX_PRISM_LIVE_OPT_ITEM_ID|1000)

#define	NX_PRISM_LIVE_OPT_ITEM_LS					55
#define	NX_PRISM_LIVE_OPT_ITEM_X					30
#define	NX_PRISM_LIVE_OPT_ITEM_Y					76
#define	NX_PRISM_LIVE_OPT_ITEM_W					(600 + NX_PRISM_UI_OPT_MENU_ICON_MG)

#define	NX_PRISM_LIVE_OPT_BTN_OK_X					232
#define	NX_PRISM_LIVE_OPT_BTN_OK_Y					316
#define	NX_PRISM_LIVE_OPT_BTN_OK_W					220
#define	NX_PRISM_LIVE_OPT_BTN_OK_H					50

#define	NX_PRISM_LIVE_OPT_SUBITEM_POPUP_MAX_LINE	5
#define	NX_PRISM_LIVE_OPT_SUBITEM_POPUP_CELL_HEIGHT	55
#define	NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WIDTH		290
#define	NX_PRISM_LIVE_OPT_SUBITEM_POPUP_HEIGHT		(NX_PRISM_LIVE_OPT_SUBITEM_POPUP_CELL_HEIGHT * NX_PRISM_LIVE_OPT_SUBITEM_POPUP_MAX_LINE)
#define	NX_PRISM_LIVE_OPT_SUBITEM_POPUP_X			(NX_PRISM_LIVE_OPT_WINDOW_X + NX_PRISM_LIVE_OPT_ITEM_X + NX_PRISM_LIVE_OPT_ITEM_W - NX_PRISM_UI_OPT_MENU_ICON_X_TAB)
#define	NX_PRISM_LIVE_OPT_SUBITEM_POPUP_Y			(NX_PRISM_LIVE_OPT_WINDOW_Y + NX_PRISM_LIVE_OPT_ITEM_Y)

#define	NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WND_X		NX_PRISM_LIVE_OPT_SUBITEM_POPUP_X
#define	NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WND_Y		NX_PRISM_LIVE_OPT_WINDOW_Y
#define	NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WND_W		(NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WIDTH + 25)
#define	NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WND_H		NX_PRISM_LIVE_OPT_WINDOW_H

#define	NX_PRISM_LIVE_OPT_SUBTOFF_ITEM_NUM			1
#define	NX_PRISM_LIVE_OPT_SUBTOFF_ITEM_INDEX		0
#define	NX_PRISM_LIVE_OPT_SUBTOFF_ITEM_TYPE			0xFF


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxOptItem_SubtitleList,
	eNxOptItem_PositionList,
	eNxOptItem_SizeList,
	eNxOptItem_Sync,
	eNxOptItem_BTN_Close,
	eNxOptItem_MAX
} NxSubOptItem_t;


typedef struct tag_NX_SUB_OPT_CONTENT
{
	HUINT32						ulCurFocus;
	HUINT32						ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	HUINT32						ulViewId;
	OxMediaPlay_MediaType_e		eMediaType;

	// Previous Value
	HINT32						lPrevSubtitleIdx;
	HUINT32						ulPrevPosition;
	HUINT32						ulPrevFontSize;
	HUINT32						ulPrevSync;

	// Subtitle
	HBOOL							bSubtListAddOffItem;
	NX_Media_SBTList_t				stSubtList;

	HUINT32							ulPosition;
	HUINT32							ulFontSize;
	HUINT32							ulSync;
	HUINT8							SubSyncIntervalText[NX_TEXT_SIZE_16];

	///< list of Image item
	HUINT8							**szImageItemTextList;
	HUINT32							ulImageItemNum;
} Nx_SubOptContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_SubOptContent_t			s_stOptContent;
static Nx_PrismListDialog_Content_t		s_stPopupListOptContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_subOpt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_subOpt_DestroyListPopup(void);
static void 			nx_subOpt_CreateListPopup(Nx_SubOptContent_t *pstContents, HINT32 ulObjId);

static inline Nx_SubOptContent_t* nx_subOpt_GetContents(void);
static HCHAR*			nx_subOpt_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_subOpt_GetMenuItemIdFormObjId(Nx_SubOptContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_subOpt_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_subOpt_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_subOpt_GetItemSupport(NxSubOptItem_t eNxMenuLanguageItem);
static HBOOL			nx_subOpt_GetItemiDisable(NxSubOptItem_t eNxMenuLanguageItem);
static void				nx_subOpt_FindNextFocus(Nx_SubOptContent_t *pstContents, HBOOL bUp);

static ONDK_Result_t	nx_subOpt_UpdateSubItem(Nx_SubOptContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_subOpt_ChangedSubItemValue(Nx_SubOptContent_t *pstContents, HUINT32 ulCurIdx);

static void				nx_subOpt_DeleteImageTextList(HUINT32 ulItemNum, HUINT8 **ppucTextArray);
static HUINT8**			nx_subOpt_MakeImageTextList(Nx_SubOptContent_t *pstContents, HINT32 lObjId, HUINT32 ulItemNum);

static HUINT8**			nx_subOpt_MakeSubtitleTextList(Nx_SubOptContent_t *pstContents, HUINT32 *pulListIdx);
static ONDK_Result_t	nx_subOpt_LoadSubtitleItemsValue(Nx_SubOptContent_t *pstContents);

static ONDK_Result_t	nx_subOpt_LoadItemsValue(Nx_SubOptContent_t *pstContents);
static ONDK_Result_t	nx_subOpt_AddItemSubtitleList(Nx_SubOptContent_t *pstContents);
static ONDK_Result_t	nx_subOpt_AddItemPositionList(Nx_SubOptContent_t *pstContents);
static ONDK_Result_t	nx_subOpt_AddItemSizeList(Nx_SubOptContent_t *pstContents);
static ONDK_Result_t	nx_subOpt_AddItemSync(Nx_SubOptContent_t *pstContents);
static ONDK_Result_t	nx_subOpt_InitSettingsItems(Nx_SubOptContent_t *pstContents);

static ONDK_Result_t	nx_subOpt_SetUiObject(Nx_SubOptContent_t *pstContents);

static ONDK_Result_t	nx_subOpt_MsgGwmCreate(Nx_SubOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_subOpt_MsgGwmDestroy(Nx_SubOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_subOpt_MsgGwmHapiMsg(Nx_SubOptContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_subOpt_MsgGwmClickMsg(Nx_SubOptContent_t *pstContents, HINT32 lMessage, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_subOpt_MsgGwmKeyDown(Nx_SubOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_subOpt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListOptContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_subOpt_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_subOpt_DisplayListPopup);
}


static void		nx_subOpt_CreateListPopup(Nx_SubOptContent_t *pstContents, HINT32 ulObjId)
{
	HUINT32			ulTextListNum = 0, ulIndex = 0, ulAttribute = 0;
	HUINT8			**pptextList = NULL, **ppImageListOfItem = NULL;
	ONDK_Rect_t		stRect;
	Nx_SettingsLeafSubItem_t  *psubItem;

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

	stRect = ONDK_Rect(NX_PRISM_LIVE_OPT_SUBITEM_POPUP_X, NX_PRISM_LIVE_OPT_SUBITEM_POPUP_Y,
			NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WIDTH, NX_PRISM_LIVE_OPT_SUBITEM_POPUP_HEIGHT);

	ulAttribute = (eListDlg_OSD_USER|eListDlg_OSD_BG|eListDlg_ICON_L|eListDlg_SubItemApp);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_LIVE_OPT_SUBITEM_POPUP_CELL_HEIGHT, ulAttribute,
											ulTextListNum, NX_PRISM_LIVE_OPT_SUBITEM_POPUP_MAX_LINE, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stPopupListOptContents, NX_PRISM_FONT_SIZE_22);

	if (ulAttribute & eListDlg_OSD_USER)
	{
		stRect = ONDK_Rect(NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WND_X, NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WND_Y,
				NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WND_W, NX_PRISM_LIVE_OPT_SUBITEM_POPUP_WND_H);
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stPopupListOptContents, &stRect);
	}

	if (ulAttribute & eListDlg_ICON_L)
	{
		// Delete Alreay Image list
		if (pstContents->szImageItemTextList)
		{
			nx_subOpt_DeleteImageTextList(pstContents->ulImageItemNum, pstContents->szImageItemTextList);
			pstContents->szImageItemTextList = NULL;
			pstContents->ulImageItemNum = 0;
		}

		// Make New Image list
		ppImageListOfItem = nx_subOpt_MakeImageTextList(pstContents, ulObjId, ulTextListNum);
		if (ppImageListOfItem != NULL)
		{
			pstContents->szImageItemTextList = ppImageListOfItem;
			pstContents->ulImageItemNum = ulTextListNum;
			NX_PRISM_COMP_ListDlg_SetImageList(&s_stPopupListOptContents, (HUINT32)pstContents->szImageItemTextList);
		}
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_subOpt_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_subOpt_DisplayListPopup, 0, 0, 0, 0);
}


static inline Nx_SubOptContent_t* nx_subOpt_GetContents(void)
{
	return	&s_stOptContent;
}


static HCHAR*	nx_subOpt_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_subOpt_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxOptItem_SubtitleList:
			return	RES_LANGUAGE_ID;

		case eNxOptItem_PositionList:
			return	RES_POSITION_ID;

		case eNxOptItem_SizeList:
			return 	RES_SIZE_ID;

		case eNxOptItem_Sync:
			return	RES_SYNC_ID;

		default:
			NX_APP_Error("Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_subOpt_GetMenuItemIdFormObjId(Nx_SubOptContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_subOpt_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxOptItem_PositionList:
			*pulMenuItemId = NX_MENUITEM_ID_SET_TEXTPOSITION;
			break;

		case eNxOptItem_SizeList:
			*pulMenuItemId = NX_MENUITEM_ID_SET_FONTSIZE;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_subOpt_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = (lObjId & ~NX_PRISM_LIVE_OPT_MAINITEM_ID);

	if ((lItemId < 0) || (lItemId >= eNxOptItem_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_subOpt_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxOptItem_MAX))
	{
		return 0;
	}

	return	(NX_PRISM_LIVE_OPT_MAINITEM_ID | lItemId);
}


static HBOOL	nx_subOpt_GetItemSupport(NxSubOptItem_t eNxMenuLanguageItem)
{
	HBOOL	bSupport = FALSE;

	switch (eNxMenuLanguageItem)
	{
		case eNxOptItem_SubtitleList:
			bSupport = TRUE;
			break;
		case eNxOptItem_PositionList:
			bSupport = TRUE;
			break;
		case eNxOptItem_SizeList:
			bSupport = TRUE;
			break;
		case eNxOptItem_Sync:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}

static HBOOL	nx_subOpt_GetItemiDisable(NxSubOptItem_t eNxMenuLanguageItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxMenuLanguageItem)
	{
		case eNxOptItem_SubtitleList:
			bDisable = FALSE;
			break;
		case eNxOptItem_PositionList:
			bDisable = FALSE;
			break;
		case eNxOptItem_SizeList:
			bDisable = FALSE;
			break;
		case eNxOptItem_Sync:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_subOpt_FindNextFocus(Nx_SubOptContent_t *pstContents, HBOOL bUp)
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

static ONDK_Result_t	nx_subOpt_UpdateSubItem(Nx_SubOptContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;

	lItemIdx = nx_subOpt_ConvertObjIdToItemIdx(lObjId);
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);

	switch (lItemIdx)
	{
		case eNxOptItem_PositionList:
			if (pstContents->ulPosition != (HUINT32)pSubItem->lSubItemValue)
			{
				pstContents->ulPosition = (HUINT32)pSubItem->lSubItemValue;
				NX_MEDIA_PLAY_SetSubtitleTextPosition(pstContents->ulViewId, pstContents->ulPosition);
			}
			break;

		case eNxOptItem_SizeList:
			if (pstContents->ulFontSize != (HUINT32)pSubItem->lSubItemValue)
			{
				pstContents->ulFontSize = (HUINT32)pSubItem->lSubItemValue;
				NX_MEDIA_PLAY_SetSubtitleFontSize(pstContents->ulViewId, pstContents->ulFontSize);
			}
			break;

		default:
			return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_subOpt_ChangedSubItemValue(Nx_SubOptContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1, lSelectIdx = 0, lOrgIndex = 0;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_subOpt_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxOptItem_SubtitleList:
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = ulCurIdx;
			pSubItem->lSubItemValue = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			if (pstContents->lPrevSubtitleIdx != pSubItem->lSubItemValue)
			{
				lSelectIdx = pSubItem->lSubItemValue;
				if (pstContents->bSubtListAddOffItem == TRUE)
				{
					lOrgIndex = (lSelectIdx == NX_PRISM_LIVE_OPT_SUBTOFF_ITEM_INDEX) ? -1 : (lSelectIdx - NX_PRISM_LIVE_OPT_SUBTOFF_ITEM_NUM) ;
				}
				else
				{
					lOrgIndex = lSelectIdx;
				}

				if (lOrgIndex == -1)
				{
					NX_MEDIA_PLAY_SetSubtitleEnable(pstContents->ulViewId, FALSE);
					NX_SYSTEM_ShowSubtitle(FALSE);
				}
				else
				{
					NX_MEDIA_PLAY_SetSubtitleEnable(pstContents->ulViewId, TRUE);
					NX_SYSTEM_ShowSubtitle(TRUE);
				}

				NX_MEDIA_PLAY_SetComponentIndex(pstContents->ulViewId, eOxMP_COMPONENT_SUBTITLE, lOrgIndex);
			}
			else
			{
				NX_APP_Info(" No change Subtitle : lPrevSubtitleIdx [%d]\n", pstContents->lPrevSubtitleIdx);
			}
			break;

		case eNxOptItem_PositionList:
		case eNxOptItem_SizeList:
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
			nx_subOpt_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

			/* Update value */
			nx_subOpt_UpdateSubItem(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static void		nx_subOpt_DeleteImageTextList(HUINT32 ulItemNum, HUINT8 **ppucTextArray)
{
	HUINT32			ulIndex = 0, ulArraySize = 0;


	if (ppucTextArray == NULL)
	{
		NX_APP_Info(" Not Support Image list!!\n");
		return;
	}

	ulArraySize = ulItemNum;
	if (ulArraySize == 0)
	{
		NX_APP_Info(" Image list ulItemNum is Zero!!\n");
		return;
	}

	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		// Source Text
		if (ppucTextArray[ulIndex] != NULL)
		{
			NX_APP_Free(ppucTextArray[ulIndex]);
		}
	}

	if (ppucTextArray != NULL)
	{
		NX_APP_Free(ppucTextArray);
	}
}


static HUINT8 **nx_subOpt_MakeImageTextList(Nx_SubOptContent_t *pstContents, HINT32 lObjId, HUINT32 ulItemNum)
{
	HUINT32 		ulIndex = 0, ulArraySize = 0;
	HUINT32 		ulTextLen = 0;
	HUINT8			*pucSrcText = NULL, *pucDstText = NULL;
	HUINT8			**ppucTextArray = NULL;
	HINT32			lItemIdx = -1;

	lItemIdx = nx_subOpt_ConvertObjIdToItemIdx(lObjId);

	ulArraySize = ulItemNum;
	if (ulArraySize == 0)
	{
		NX_APP_Info(" Image list ulItemNum is Zero!!\n");
		return	NULL;
	}

	ppucTextArray = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * ulArraySize);
	if (ppucTextArray == NULL)
	{
		return	NULL;
	}

	HxSTD_memset(ppucTextArray, 0, sizeof(HUINT8 *) * ulArraySize);
	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		// Source Text
		switch (lItemIdx)
		{
			case eNxOptItem_SubtitleList:
				pucSrcText = (HUINT8*) NX_PRISM_UTIL_GetSubtitleIconResource(pstContents->stSubtList.pstSubtitlelInfo[ulIndex].eSubtitleType);
				if (pucSrcText == NULL)
				{
					pucSrcText = (HUINT8*)RES_NONE_PNG;
				}
				break;
			default:
				pucSrcText = NULL;
				break;
		}

		if (pucSrcText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		ulTextLen  = HxSTD_StrLen ((char*)pucSrcText);

		// Destination Text
		pucDstText = (HUINT8 *)NX_APP_Malloc (ulTextLen + 1);
		if (pucDstText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		HxSTD_memset(pucDstText, 0x0, ulTextLen + 1);
		pucDstText[0] = '\0';

		HxSTD_StrNCpy ((char*)pucDstText, (char*)pucSrcText, ulTextLen);
		ppucTextArray[ulIndex] = pucDstText;
	}

	return	ppucTextArray;
}

static HUINT8 **nx_subOpt_MakeSubtitleTextList(Nx_SubOptContent_t *pstContents, HUINT32 *pulListIdx)
{
	HERROR		hError = ERR_FAIL;
	HINT32		lSubttlTrackIndex = 0, lOffItemCnt = 0;
	HUINT32		ulIndex = 0, ulArraySize = 0, ulTextLen = 0;
	HUINT8		*pucDstText = NULL, **ppucTextArray = NULL;
	HCHAR		szStrBuf[NX_TEXT_SIZE_64];


	HxSTD_memset(szStrBuf, 0x0, NX_TEXT_SIZE_64);
	szStrBuf[0] = '\0';

	*pulListIdx = 0;

	ulArraySize = pstContents->stSubtList.ulSubtitleNum;
	if (ulArraySize == 0)
	{
		NX_APP_Info(" Subtitle list item is Zero!!\n");
		return	NULL;
	}

	ppucTextArray = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * ulArraySize);
	if (ppucTextArray == NULL)
	{
		NX_APP_Error("ERROR!!! NX_APP_Malloc() Fail!!\n");
		return	NULL;
	}

	HxSTD_memset(ppucTextArray, 0, sizeof(HUINT8 *) * ulArraySize);
	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		// Source Text
		NX_PRISM_UTIL_MakeSubtitleString(pstContents->stSubtList, ulIndex, szStrBuf);
		ulTextLen  = HxSTD_StrLen ((char*)szStrBuf);

		// Destination Text
		pucDstText = (HUINT8 *)NX_APP_Malloc (ulTextLen + 1);
		if (pucDstText == NULL)
		{
			NX_APP_Error("ERROR!!! NX_APP_Malloc() Fail!!\n");
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		HxSTD_memset(pucDstText, 0x0, ulTextLen + 1);
		pucDstText[0] = '\0';

		HxSTD_StrNCpy ((char*)pucDstText, (char*)szStrBuf, ulTextLen);
		ppucTextArray[ulIndex] = pucDstText;
	}

	lOffItemCnt = (pstContents->bSubtListAddOffItem == TRUE) ? NX_PRISM_LIVE_OPT_SUBTOFF_ITEM_NUM : 0 ;
	hError = NX_MEDIA_PLAY_GetComponentIndex(pstContents->ulViewId, eOxMP_COMPONENT_SUBTITLE, &lSubttlTrackIndex, NULL);
	if (hError != ERR_OK)
	{
		*pulListIdx = NX_PRISM_LIVE_OPT_SUBTOFF_ITEM_INDEX;
	}
	else
	{
		if ((lSubttlTrackIndex < 0) || (lSubttlTrackIndex == -1))
		{
			*pulListIdx = NX_PRISM_LIVE_OPT_SUBTOFF_ITEM_INDEX;
		}
		else
		{
			*pulListIdx = lSubttlTrackIndex + lOffItemCnt;
		}
	}

	return	ppucTextArray;
}


static ONDK_Result_t	nx_subOpt_LoadSubtitleItemsValue(Nx_SubOptContent_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				i = 0, ulCnt = 0, lOffItemCnt = 0;
	NX_Media_SBTList_t	stComponetSubtitle;
	OxMediaPlay_SubtitleComp_t	*pListInfo = NULL;

	stComponetSubtitle.pstSubtitlelInfo = NULL;
	stComponetSubtitle.pstSubtitlelInfo = (OxMediaPlay_SubtitleComp_t*)NX_APP_Calloc(sizeof(OxMediaPlay_SubtitleComp_t) * NX_CORE_SUBTTL_MAX_NUM);
	if (stComponetSubtitle.pstSubtitlelInfo == NULL)
	{
		NX_APP_Error(" Error!!! NX_APP_Calloc() Fail !!\n");
		return	ONDK_RET_NOLOCALMEMORY;
	}

	hError = NX_MEDIA_PLAY_GetComponentListForSubtitle(pstContents->ulViewId, eOxMP_COMPONENT_SUBTITLE, &stComponetSubtitle);
	if (hError != ERR_OK)
	{
		if (stComponetSubtitle.pstSubtitlelInfo)
		{
			NX_APP_Free(stComponetSubtitle.pstSubtitlelInfo);
			stComponetSubtitle.pstSubtitlelInfo = NULL;
		}
		return	ONDK_RET_INITFAIL;
	}
	else
	{
		if (stComponetSubtitle.ulSubtitleNum < 1)
		{
			if (stComponetSubtitle.pstSubtitlelInfo)
			{
				NX_APP_Free(stComponetSubtitle.pstSubtitlelInfo);
				stComponetSubtitle.pstSubtitlelInfo = NULL;
			}
			return	ONDK_RET_INVARG;
		}
		else
		{
			/* Add Off Item */
			if (pstContents->bSubtListAddOffItem == TRUE)
			{
				pListInfo = &pstContents->stSubtList.pstSubtitlelInfo[NX_PRISM_LIVE_OPT_SUBTOFF_ITEM_INDEX];
				pListInfo->eCompType = eOxMP_COMPONENT_SUBTITLE;
				pListInfo->nPid = NX_CORE_PID_NULL;
				pListInfo->eSubtitleType = eOxMP_SUBTITLETYPE_Unknown;
				lOffItemCnt = NX_PRISM_LIVE_OPT_SUBTOFF_ITEM_NUM;
			}
			else
			{
				lOffItemCnt = 0;
			}

			ulCnt = stComponetSubtitle.ulSubtitleNum;
			if (stComponetSubtitle.ulSubtitleNum >= NX_CORE_SUBTTL_MAX_NUM)
			{
				ulCnt = (NX_CORE_SUBTTL_MAX_NUM - 1);
			}

			/* Add Subtitle Item */
			for (i = 0; i < ulCnt; i++)
			{
				pListInfo = &pstContents->stSubtList.pstSubtitlelInfo[lOffItemCnt + i];
				HxSTD_memcpy(pListInfo, &stComponetSubtitle.pstSubtitlelInfo[i], sizeof(OxMediaPlay_SubtitleComp_t));
			}

			/* Add Subtitle Count + Add Off Item */
			pstContents->stSubtList.ulSubtitleNum = (ulCnt + lOffItemCnt);
		}
	}

	if (stComponetSubtitle.pstSubtitlelInfo)
	{
		NX_APP_Free(stComponetSubtitle.pstSubtitlelInfo);
		stComponetSubtitle.pstSubtitlelInfo = NULL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subOpt_LoadItemsValue(Nx_SubOptContent_t *pstContents)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulPosition = 0, ulFontSize = 0, ulSync = 0;
	HFLOAT64	flSyncTime = 0;

	hError = NX_MEDIA_PLAY_GetSubtitleTextPosition(pstContents->ulViewId, &ulPosition);
	if (hError == ERR_OK)
	{
		pstContents->ulPosition = ulPosition;
	}

	hError = NX_MEDIA_PLAY_GetSubtitleFontSize(pstContents->ulViewId, &ulFontSize);
	if (hError == ERR_OK)
	{
		pstContents->ulFontSize = ulFontSize;
	}

	hError = NX_MEDIA_PLAY_GetSubtitleSyncTime(pstContents->ulViewId, &ulSync);
	if (hError == ERR_OK)
	{
		pstContents->ulSync = ulSync;
		flSyncTime = ((HFLOAT64)((HINT32)pstContents->ulSync) / 1000);
		if (flSyncTime == 0)
		{
			HxSTD_snprintf((HCHAR *)pstContents->SubSyncIntervalText, NX_TEXT_SIZE_16, "%d %s", 0, (HCHAR*)ONDK_GetString(RES_SEC_ID));
		}
		else
		{
			HxSTD_snprintf((HCHAR *)pstContents->SubSyncIntervalText, NX_TEXT_SIZE_16, "%.1lf %s", flSyncTime, (HCHAR*)ONDK_GetString(RES_SEC_ID));
		}
	}

	/* Get Subtitle List */
	nx_subOpt_LoadSubtitleItemsValue(pstContents);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_subOpt_AddItemSubtitleList(Nx_SubOptContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulItems = 0, ulListIdx = 0, ulSubItemCnt = 0;
	HUINT8				**ppListOfItem = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_subOpt_GetItemSupport(eNxOptItem_SubtitleList);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxOptItem_SubtitleList(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_subOpt_ConvertItemIdxToObjId(eNxOptItem_SubtitleList);

	// Check bDisable Item
	bDisable = nx_subOpt_GetItemiDisable(eNxOptItem_SubtitleList);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_subOpt_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_LIVE_OPT_ITEM_X;
	stSubItem.rect.y = NX_PRISM_LIVE_OPT_ITEM_Y + (ulSubItemCnt * NX_PRISM_LIVE_OPT_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_LIVE_OPT_ITEM_W;
	stSubItem.rect.h = NX_PRISM_LIVE_OPT_ITEM_LS;

	/* Count SubItems */
	ulItems = pstContents->stSubtList.ulSubtitleNum;

	/* List를 만든다. */
	ppListOfItem = nx_subOpt_MakeSubtitleTextList(pstContents, &ulListIdx);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!! nx_subOpt_MakeSubtitleTextList() ppListOfItem is NULL \n");
		stSubItem.bDisable = TRUE;
		if (pMainStrId != NULL)
		{
			HxSTD_sprintf((char*)stSubItem.szSubItemText, (char*)"%s", ONDK_GetString(pMainStrId));
		}
	}
	else
	{
		stSubItem.ulNumOfItemTextList = ulItems;
		stSubItem.szItemTextList = ppListOfItem;

		stSubItem.lSubItemValue = (HINT32)ulListIdx;
		stSubItem.ulIdxOfTextList = ulListIdx;
		pstContents->lPrevSubtitleIdx = ulListIdx;

		HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);
	}
	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_subOpt_AddItemPositionList(Nx_SubOptContent_t *pstContents)
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
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_subOpt_GetItemSupport(eNxOptItem_PositionList);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxOptItem_PositionList(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_subOpt_ConvertItemIdxToObjId(eNxOptItem_PositionList);

	// Check bDisable Item
	bDisable = nx_subOpt_GetItemiDisable(eNxOptItem_PositionList);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_subOpt_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_LIVE_OPT_ITEM_X;
	stSubItem.rect.y = NX_PRISM_LIVE_OPT_ITEM_Y + (ulSubItemCnt * NX_PRISM_LIVE_OPT_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_LIVE_OPT_ITEM_W;
	stSubItem.rect.h = NX_PRISM_LIVE_OPT_ITEM_LS;

	/* Count SubItems */
	nx_subOpt_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
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
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->ulPosition), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Not found menu item index(%d)\n", ulListIdx);
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->ulPosition;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_subOpt_AddItemSizeList(Nx_SubOptContent_t *pstContents)
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
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_subOpt_GetItemSupport(eNxOptItem_SizeList);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxOptItem_SizeList(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_subOpt_ConvertItemIdxToObjId(eNxOptItem_SizeList);

	// Check bDisable Item
	bDisable = nx_subOpt_GetItemiDisable(eNxOptItem_SizeList);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_subOpt_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_LIVE_OPT_ITEM_X;
	stSubItem.rect.y = NX_PRISM_LIVE_OPT_ITEM_Y + (ulSubItemCnt * NX_PRISM_LIVE_OPT_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_LIVE_OPT_ITEM_W;
	stSubItem.rect.h = NX_PRISM_LIVE_OPT_ITEM_LS;

	/* Count SubItems */
	nx_subOpt_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
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
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->ulFontSize), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Not found menu item index(%d)\n", ulListIdx);
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->ulFontSize;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_subOpt_AddItemSync(Nx_SubOptContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulSubItemCnt = 0;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_subOpt_GetItemSupport(eNxOptItem_Sync);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxOptItem_SizeList(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_subOpt_ConvertItemIdxToObjId(eNxOptItem_Sync);

	// Check bDisable Item
	bDisable = nx_subOpt_GetItemiDisable(eNxOptItem_Sync);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_subOpt_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	ulSubItemCnt = NX_PRISM_AppContents_CountSubitem((void *)pstContents);
	stSubItem.rect.x = NX_PRISM_LIVE_OPT_ITEM_X;
	stSubItem.rect.y = NX_PRISM_LIVE_OPT_ITEM_Y + (ulSubItemCnt * NX_PRISM_LIVE_OPT_ITEM_LS);
	stSubItem.rect.w = NX_PRISM_LIVE_OPT_ITEM_W;
	stSubItem.rect.h = NX_PRISM_LIVE_OPT_ITEM_LS;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)pstContents->SubSyncIntervalText, MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_subOpt_InitSettingsItems(Nx_SubOptContent_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT32			ulViewId = 0;
	OxMediaPlay_MediaType_e		eMediaType = eOxMP_MEDIATYPE_NONE;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	NX_MEDIA_PLAY_GetMainViewId(&ulViewId);
	pstContents->ulViewId = ulViewId;
	NX_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);
	pstContents->eMediaType = eMediaType;

	// Add Subtitle Off Item
	pstContents->bSubtListAddOffItem = TRUE;

	// Calloc subtitle data
	pstContents->stSubtList.pstSubtitlelInfo = NULL;
	pstContents->stSubtList.pstSubtitlelInfo = (OxMediaPlay_SubtitleComp_t*)NX_APP_Calloc(sizeof(OxMediaPlay_SubtitleComp_t) * NX_CORE_SUBTTL_MAX_NUM);
	if (pstContents->stSubtList.pstSubtitlelInfo == NULL)
	{
		NX_APP_Error(" Error!!! NX_APP_Calloc() Fail !!\n");
		return	ONDK_RET_NOLOCALMEMORY;
	}

	// Load Menu Item Value
	nx_subOpt_LoadItemsValue(pstContents);

	nx_subOpt_AddItemSubtitleList(pstContents);
	nx_subOpt_AddItemPositionList(pstContents);
	nx_subOpt_AddItemSizeList(pstContents);
	nx_subOpt_AddItemSync(pstContents);
	// Default Function
	//nx_subOpt_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subOpt_SetUiObject(Nx_SubOptContent_t *pstContents)
{
	HINT32				lObjId = -1;
	ONDK_Rect_t 		stRect;

	/* Draw BG Frame */
	stRect = ONDK_Rect(NX_PRISM_LIVE_OPT_FRAME_X, NX_PRISM_LIVE_OPT_FRAME_Y, NX_PRISM_LIVE_OPT_FRAME_W, NX_PRISM_LIVE_OPT_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_LIVE_OPT_FRAME_ID, &stRect, COL(C_G_Bang_90));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_LIVE_OPT_FRAME_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG_TopLine);

	/* Draw Title TEXT */
	stRect = ONDK_Rect(NX_PRISM_LIVE_OPT_TTL_TEXT_X, NX_PRISM_LIVE_OPT_TTL_TEXT_Y,
						NX_PRISM_LIVE_OPT_TTL_TEXT_W, NX_PRISM_LIVE_OPT_TTL_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_LIVE_OPT_TTL_TEXT_ID, &stRect, (HCHAR*)ONDK_GetString(RES_MESG_3183_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_LIVE_OPT_TTL_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_LIVE_OPT_TTL_TEXT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_LIVE_OPT_TTL_TEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_LIVE_OPT_TTL_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Draw Main Item */
	NX_PRISM_COMP_OPT_InitMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_OPT_DrawCursor_A1_Arrow);
	// Set Focus
	ONDK_GWM_Obj_SetFocus(pstContents->subItems[pstContents->ulCurFocus].ulId);

	/* Draw Button Item */
	stRect = ONDK_Rect(NX_PRISM_LIVE_OPT_BTN_OK_X, NX_PRISM_LIVE_OPT_BTN_OK_Y, NX_PRISM_LIVE_OPT_BTN_OK_W, NX_PRISM_LIVE_OPT_BTN_OK_H);
	lObjId = nx_subOpt_ConvertItemIdxToObjId(eNxOptItem_BTN_Close);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_CLOSE_02_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_OPT_DrawButton_A1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subOpt_MsgGwmCreate(Nx_SubOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Init Menu Item
	nx_subOpt_InitSettingsItems(pstContents);

	ONDK_GWM_APP_SetAppArea(NX_PRISM_LIVE_OPT_WINDOW_X, NX_PRISM_LIVE_OPT_WINDOW_Y, NX_PRISM_LIVE_OPT_WINDOW_W, NX_PRISM_LIVE_OPT_WINDOW_H);

	// Draw Menu Item
	nx_subOpt_SetUiObject(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subOpt_MsgGwmDestroy(Nx_SubOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	if (pstContents->stSubtList.pstSubtitlelInfo)
	{
		NX_APP_Free(pstContents->stSubtList.pstSubtitlelInfo);
		pstContents->stSubtList.pstSubtitlelInfo = NULL;
	}

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_SubOptContent_t));
	nx_subOpt_DestroyListPopup();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subOpt_MsgGwmHapiMsg(Nx_SubOptContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_subOpt_MsgGwmClickMsg(Nx_SubOptContent_t *pstContents, HINT32 lMessage, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lObjId = p1, lItemIdx = -1;

	lItemIdx = nx_subOpt_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxOptItem_SubtitleList:
		case eNxOptItem_PositionList:
		case eNxOptItem_SizeList:
			nx_subOpt_CreateListPopup(pstContents, p1);
			break;
		case eNxOptItem_Sync:
			ONDK_GWM_APP_Destroy(0);
#if defined(CONFIG_MW_MM_MEDIA)
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_SUBTITLE_Sync_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_SUBTITLE_Sync_Proc, 0, p1, p2, p3);
#endif
			break;
		case eNxOptItem_BTN_Close:
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


static ONDK_Result_t	nx_subOpt_MsgGwmKeyDown(Nx_SubOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
		case KEY_EXIT:
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

		case KEY_MENU:
		case KEY_GUIDE:
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_TEXT:
		case KEY_WIDE:
		case KEY_AUDIO:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SUBTITLE_Option_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_SubOptContent_t	*pstContents = NULL;

	pstContents = nx_subOpt_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_subOpt_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_subOpt_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			eResult = nx_subOpt_MsgGwmClickMsg(pstContents, lMessage, p1, p2, p3);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			//nx_subOpt_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			//nx_subOpt_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_subOpt_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_subOpt_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_subOpt_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_APP_PLAYER_DESTROY:
			ONDK_GWM_APP_Destroy(0);
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

