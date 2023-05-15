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
 * @file	  		nx_prism_menu_hiddne_main.c
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

#define	NX_PRISM_MENU_HIDDEN_PIN_DIGIT_MAX						5


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_HiddenPasswordReset,
	eNxSettingsItem_HiddenFusingInfo,
	eNxSettingsItem_HiddenCountryCode,
	eNxSettingsItem_Hidden_MAX
} NxSettingsItem_HiddenManagement_e;


typedef struct tag_NX_MENUHIDDENMANAGEMENT_SET_CONTENT
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	HINT32					lPrevCurIdx;
	NxUiMenuHiddenType_t	eMenuHiddenType;

	HxCountry_e				eCountryIdx;

	HUINT8					szPinCodeStr[NX_PRISM_MENU_HIDDEN_PIN_DIGIT_MAX];
	HUINT8					szMessage[NX_TEXT_SIZE_256];

	HUINT8			szDate[NX_TEXT_SIZE_32];
	HUINT8			szTime[NX_TEXT_SIZE_32];
} Nx_MenuHiddenManagementSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuHiddenManagementSetContent_t	s_stHiddenManagementContent;
static NXUI_DialogInstance_t				s_stHiddenManagement_DialogInstance;
static Nx_PrismListDialog_Content_t			s_stPopupListHiddenMgrSetConts;

static HINT32		s_lMenuhiddenKeySeq = 0;
static HINT32		s_lMenuhiddenPwdResetKeySeq = 0;
static HINT32		s_lMenuhiddenCountryCodeKeySeq = 0;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_menuHiddenManagement_GetDialogPopupContents(void);
static ONDK_Result_t	nx_menuHiddenManagement_DialogProc (HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHiddenManagement_CreateDialog(Nx_MenuHiddenManagementSetContent_t *pstContents, NxSettingsItem_HiddenManagement_e eDialogType, HUINT8 *szMessage);
static ONDK_Result_t	nx_menuHiddenManagement_DestroyDialog(void);

static ONDK_Result_t	nx_menuHiddenManagement_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHiddenManagement_DestroyListPopup(void);
static void 			nx_menuHiddenManagement_CreateListPopup(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 ulObjId);

static inline Nx_MenuHiddenManagementSetContent_t* nx_menuHiddenManagement_GetContents(void);
static HCHAR*			nx_menuHiddenManagement_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuHiddenManagement_GetMenuItemIdFormObjId(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuHiddenManagement_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuHiddenManagement_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuHiddenManagement_GetItemSupport(Nx_MenuHiddenManagementSetContent_t *pstContents, NxSettingsItem_HiddenManagement_e eNxMenuPowerManagementItem);
static HBOOL			nx_menuHiddenManagement_GetItemiDisable(Nx_MenuHiddenManagementSetContent_t *pstContents, NxSettingsItem_HiddenManagement_e eNxMenuPowerManagementItem);
static void				nx_menuHiddenManagement_FindNextFocus(Nx_MenuHiddenManagementSetContent_t *pstContents, HBOOL bUp);
static void				nx_menuHiddenManagement_UpdateCurrentTime(Nx_MenuHiddenManagementSetContent_t *pstContents);

static ONDK_Result_t	nx_menuHiddenManagement_RefreshSubItem(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lObjId);
static void				nx_menuHiddenManagement_RefreshAllSubItems(Nx_MenuHiddenManagementSetContent_t *pstContents);

static ONDK_Result_t	nx_menuHiddenManagement_UpdateCountry(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuHiddenManagement_UpdateSubItem(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuHiddenManagement_ChangedSubItemValue(Nx_MenuHiddenManagementSetContent_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_menuHiddenManagement_ActionItem(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_menuHiddenManagement_LoadSettingsItemsValue(Nx_MenuHiddenManagementSetContent_t *pstContents);
static ONDK_Result_t	nx_menuHiddenManagement_AddItemCountryCode(Nx_MenuHiddenManagementSetContent_t *pstContents);
static ONDK_Result_t	nx_menuHiddenManagement_AddItemFusingInfo(Nx_MenuHiddenManagementSetContent_t *pstContents);
static ONDK_Result_t	nx_menuHiddenManagement_AddItemPasswordReset(Nx_MenuHiddenManagementSetContent_t *pstContents);
static ONDK_Result_t	nx_menuHiddenManagement_InitSettingsItems(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p2);

static ONDK_Result_t	nx_menuHiddenManagement_DrawDefaultUI(Nx_MenuHiddenManagementSetContent_t *pstContents);

static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmCreate(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuHiddenManagement_MsgGwmDestroy(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmHapiMsg(Nx_MenuHiddenManagementSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmKeyDown(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmTimer(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmObjFocusChanged(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_menuHiddenManagement_MsgAppDlgClicked(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lClickedBtn, HINT32 lDlgWhoseId);
static ONDK_Result_t	nx_menuHiddenManagement_MsgAppDlgCanceled(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lDlgWhoseId);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Select_Dialog_Functions___________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_menuHiddenManagement_GetDialogPopupContents(void)
{
	return	&s_stHiddenManagement_DialogInstance;
}


static ONDK_Result_t	nx_menuHiddenManagement_DialogProc(HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_menuHiddenManagement_GetDialogPopupContents(), lMessage, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_menuHiddenManagement_DestroyDialog(void)
{
	ONDK_GWM_APP_Destroy(nx_menuHiddenManagement_DialogProc);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_CreateDialog(Nx_MenuHiddenManagementSetContent_t *pstContents, NxSettingsItem_HiddenManagement_e eDialogType, HUINT8 *szMessage)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuHiddenManagement_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	pstDialog->ulDlgWhoseId = eDialogType;
	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),	ulDlgAttr, NULL, (HUINT8 *)szMessage);

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuHiddenManagement_DialogProc", APP_DIALOGBOX_PRIORITY, nx_menuHiddenManagement_DialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define __PopUp_LIST_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuHiddenManagement_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListHiddenMgrSetConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuHiddenManagement_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuHiddenManagement_DisplayListPopup);
}


static void		nx_menuHiddenManagement_CreateListPopup(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 ulObjId)
{
	HUINT32			ulTextListNum = 0, ulIndex = 0;
	HUINT8			**pptextList = NULL;
	ONDK_Rect_t		stRect;
	Nx_SettingsLeafSubItem_t  *psubItem = NULL;

	/* Get subitem matched nObjId */
	psubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, ulObjId);
	if ( psubItem == NULL )
	{
		return;
	}

	pptextList = psubItem->szItemTextList;
	ulTextListNum = psubItem->ulNumOfItemTextList;
	ulIndex	= psubItem->ulIdxOfTextList;

	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SUBITEM_POPUP_X, NX_PRISM_UI_MENU_SUBITEM_POPUP_Y, NX_PRISM_UI_MENU_SUBITEM_POPUP_WIDTH, NX_PRISM_UI_MENU_SUBITEM_POPUP_HEIGHT);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListHiddenMgrSetConts,	ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
											ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuHiddenManagement_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuHiddenManagement_DisplayListPopup, 0, 0, 0, 0);
}


/******************************************************************************/
#define __Local_Functions___________________________________________
/******************************************************************************/

static inline Nx_MenuHiddenManagementSetContent_t* nx_menuHiddenManagement_GetContents(void)
{
	return	&s_stHiddenManagementContent;
}


static HCHAR*	nx_menuHiddenManagement_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuHiddenManagement_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_HiddenPasswordReset:
			return	RES_PASSWORD_RESET_ID;

		case eNxSettingsItem_HiddenFusingInfo:
			return	RES_FUSING_INFO_ID;

		case eNxSettingsItem_HiddenCountryCode:
			return	RES_SELECT_YOUR_COUNTRY_ID;

		default:
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuHiddenManagement_GetMenuItemIdFormObjId(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuHiddenManagement_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_HiddenCountryCode:
			*pulMenuItemId = NX_MENUITEM_ID_SET_COUNTRYSETTING;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static HINT32	nx_menuHiddenManagement_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_HiddenInfomation);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_Hidden_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuHiddenManagement_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_Hidden_MAX))
	{
		return 0;
	}

	return	(eNxSetting_HiddenInfomation | lItemId);
}


static HBOOL	nx_menuHiddenManagement_GetItemSupport(Nx_MenuHiddenManagementSetContent_t *pstContents, NxSettingsItem_HiddenManagement_e eNxMenuPowerManagementItem)
{
	HBOOL	bSupport = FALSE;

	switch (eNxMenuPowerManagementItem)
	{
		case eNxSettingsItem_HiddenPasswordReset:
			if ((pstContents->eMenuHiddenType & eNxUiMenuHiddenType_All) || (pstContents->eMenuHiddenType & eNxUiMenuHiddenType_PasswordReset))
			{
				bSupport = TRUE;
			}
			else
			{
				bSupport = FALSE;
			}
			break;
		case eNxSettingsItem_HiddenFusingInfo:
			if ((pstContents->eMenuHiddenType & eNxUiMenuHiddenType_All) || (pstContents->eMenuHiddenType & eNxUiMenuHiddenType_FusingInfo))
			{
				bSupport = TRUE;
			}
			else
			{
				bSupport = FALSE;
			}
			break;
		case eNxSettingsItem_HiddenCountryCode:
			if ((pstContents->eMenuHiddenType & eNxUiMenuHiddenType_All) || (pstContents->eMenuHiddenType & eNxUiMenuHiddenType_CountryCode))
			{
				bSupport = TRUE;
			}
			else
			{
				bSupport = FALSE;
			}
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_menuHiddenManagement_GetItemiDisable(Nx_MenuHiddenManagementSetContent_t *pstContents, NxSettingsItem_HiddenManagement_e eNxMenuPowerManagementItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxMenuPowerManagementItem)
	{
		case eNxSettingsItem_HiddenPasswordReset:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_HiddenFusingInfo:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_HiddenCountryCode:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuHiddenManagement_FindNextFocus(Nx_MenuHiddenManagementSetContent_t *pstContents, HBOOL bUp)
{
	HUINT32			i = 0, ulSubItems = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;

	ulSubItems = NX_PRISM_AppContents_CountSubitem((void *)pstContents);

	for (i = 0; i < ulSubItems; i++)
	{
		if ( bUp == TRUE )
		{
			pSubItem =  NX_PRISM_AppContents_GetSubitem((void *)pstContents, (ulSubItems - 1) - i);
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


static void		nx_menuHiddenManagement_UpdateCurrentTime(Nx_MenuHiddenManagementSetContent_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HCHAR 			*pszDayOfWeek;
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


static ONDK_Result_t	nx_menuHiddenManagement_RefreshSubItem(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lObjId)
{
	HUINT8		**ppListOfItem = NULL;
	HUINT32		ulMenuId, ulListIdx, ulItems;
	HINT32		lSubItem = 0, lItemIdx = -1;
	HCHAR		*pcStrId = NULL;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	 *pSubItem;


	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, (HUINT32)lObjId);
	if ( pSubItem == NULL )
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_memset(&pSubItem->szMainItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
	NX_PRISM_AppContents_ClearLeafSubItem(pSubItem);

	/* Get Main Text ID */
	pcStrId = nx_menuHiddenManagement_GetMenuStrIdFormObjId(lObjId);
	if (pcStrId != NULL)
	{
		HxSTD_sprintf((char*)pSubItem->szMainItemText, (char*)"%s", ONDK_GetString(pcStrId));
	}

	/* Convert ObjId to MenuItem Id. */
	nx_menuHiddenManagement_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuId);

	lItemIdx = nx_menuHiddenManagement_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSettingsItem_HiddenCountryCode:
			lSubItem = pstContents->eCountryIdx;
			/* Count SubItems */
			eResult = NX_PRISM_MENU_Menuitem_CountExpandSubitems (ulMenuId, &ulItems);
			if ( (eResult != ONDK_RET_OK) || (ulItems == 0) )
			{
				NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
				return	eResult;
			}

			/* List를 만든다. */
			ppListOfItem = NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId(ulMenuId, FALSE);
			if (ppListOfItem == NULL)
			{
				NX_APP_Error("Error!!! (ppListOfItem == NULL) \n");
				return	ONDK_RET_FAIL;
			}

			pSubItem->ulNumOfItemTextList = ulItems;
			pSubItem->szItemTextList = ppListOfItem;

			/* List의 Index를 얻어온다. */
			eResult = NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue(ulMenuId, lSubItem, &ulListIdx);
			if (eResult != ONDK_RET_OK)
			{
				ulListIdx = 0;
			}

			pSubItem->lSubItemValue = lSubItem;
			pSubItem->ulIdxOfTextList = ulListIdx;
			break;

		default:
			lSubItem = -1;
			/* Count SubItems */
			eResult = NX_PRISM_MENU_Menuitem_CountSubitems (ulMenuId, &ulItems);
			if ((eResult != ONDK_RET_OK) || (ulItems == 0) )
			{
				NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
				return	eResult;
			}

			/* List를 만든다. */
			ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId(ulMenuId);
			if (ppListOfItem == NULL)
			{
				NX_APP_Error("Error!!! (ppListOfItem == NULL) \n");
				return	ONDK_RET_FAIL;
			}

			pSubItem->ulNumOfItemTextList = ulItems;
			pSubItem->szItemTextList = ppListOfItem;

			/* List의 Index를 얻어온다. */
			eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue(ulMenuId, lSubItem, &ulListIdx);
			if (eResult != ONDK_RET_OK)
			{
				ulListIdx = 0;
			}

			pSubItem->lSubItemValue = lSubItem;
			pSubItem->ulIdxOfTextList = ulListIdx;
			break;
	}

	HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	return	ONDK_RET_OK;
}


static void		nx_menuHiddenManagement_RefreshAllSubItems(Nx_MenuHiddenManagementSetContent_t *pstContents)
{
	HINT32			i = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	for (i = eNxSettingsItem_HiddenPasswordReset; i < eNxSettingsItem_Hidden_MAX; i++)
	{
		eResult = nx_menuHiddenManagement_RefreshSubItem(pstContents, nx_menuHiddenManagement_ConvertItemIdxToObjId(i));
		if (eResult != ONDK_RET_OK)
		{
			NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		}
	}
}


static ONDK_Result_t	nx_menuHiddenManagement_UpdateCountry(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lObjId)
{
	HERROR		hError = ERR_FAIL;
	HINT32		lHour = 0, lGmtOffset = 0;
	HUINT32		ulMin = 0;
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eCountryIdx != (HINT32)pSubItem->lSubItemValue)
	{
		pstContents->eCountryIdx = (HINT32)pSubItem->lSubItemValue;
		hError = NX_SYSTEM_SetCountryCode(pstContents->eCountryIdx);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_SYSTEM_SetCountryCode() Fail!! \n");
		}

		hError = HLIB_DATETIME_GetTimeOffsetByCountryIndex(pstContents->eCountryIdx, &lHour, &ulMin);
		if (hError == ERR_OK)
		{
			lGmtOffset = (lHour * NX_COMMON_TIME_MINS_PER_HOUR) + (HINT32)ulMin;
			hError = NX_SYSTEM_SetGMTOffset(lGmtOffset);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_SYSTEM_SetCountryCode() Fail!! \n");
			}
		}
		else
		{
			NX_APP_Error("Error!!! HLIB_DATETIME_GetTimeOffsetByCountryIndex() - eCountryIdx:(0x%x) Fail!! \n", pstContents->eCountryIdx);
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_UpdateSubItem(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_menuHiddenManagement_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_HiddenCountryCode:
			nx_menuHiddenManagement_UpdateCountry(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_ChangedSubItemValue(Nx_MenuHiddenManagementSetContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuHiddenManagement_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_HiddenCountryCode:
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
			nx_menuHiddenManagement_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetExpandValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

			/* Update value */
			nx_menuHiddenManagement_UpdateSubItem(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_ActionItem(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lObjId)
{
	HINT32			lItemIdx = -1;
	Nx_SettingsLeafSubItem_t  *psubItem = NULL;


	/* Get subitem matched nObjId */
	psubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, (HUINT32)lObjId);
	if (psubItem == NULL)
	{
		NX_APP_Error("Error!!! (psubItem == NULL) Fail !!\n");
		return	ONDK_RET_FAIL;
	}

	lItemIdx = nx_menuHiddenManagement_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSettingsItem_HiddenPasswordReset:
			HxSTD_sprintf((char*)pstContents->szMessage, (char*)ONDK_GetString(RES_MESG_2419_ID), (char*)pstContents->szPinCodeStr);
			nx_menuHiddenManagement_CreateDialog(pstContents, eNxSettingsItem_HiddenPasswordReset, pstContents->szMessage);
			break;

		case eNxSettingsItem_HiddenFusingInfo:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_HiddenFusingInfo_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
									(ONDK_GWM_Callback_t)NX_PRISM_MENU_HiddenFusingInfo_Proc, 0, 0, 0, 0);
			break;

		case eNxSettingsItem_HiddenCountryCode:
			nx_menuHiddenManagement_CreateListPopup(pstContents, lObjId);
			break;

		default:
			NX_APP_Error("Error!!! Invalid Item index : (0x%x) !!\n", lItemIdx);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_LoadSettingsItemsValue(Nx_MenuHiddenManagementSetContent_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HxCountry_e		eCountryCode = eCountryID_NONE;

	hError = NX_SYSTEM_GetCountryCode(&eCountryCode);
	if (hError != ERR_OK)
	{
		eCountryCode = eCountryID_NONE;
		NX_APP_Error("Error!!! NX_SYSTEM_GetCountryCode() Faile! \n");
	}

	pstContents->eCountryIdx = eCountryCode;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_AddItemCountryCode(Nx_MenuHiddenManagementSetContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulItems = 0, ulListIdx = 0, ulMenuItemId = 0;
	HUINT8				**ppListOfItem = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("(pstContents == NULL)\n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuHiddenManagement_GetItemSupport(pstContents, eNxSettingsItem_HiddenCountryCode);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_HiddenCountryCode(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuHiddenManagement_ConvertItemIdxToObjId(eNxSettingsItem_HiddenCountryCode);

	// Check bDisable Item
	bDisable = nx_menuHiddenManagement_GetItemiDisable(pstContents, eNxSettingsItem_HiddenCountryCode);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuHiddenManagement_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	stSubItem.pszHelpText = NULL;

	/* Count SubItems */
	nx_menuHiddenManagement_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountExpandSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!! NX_PRISM_MENU_Menuitem_CountExpandSubitems() eResult(0x%x), ulItems:(%u)\n", eResult, ulItems);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId, TRUE);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId() ulMenuItemId:(0x%x)\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eCountryIdx), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eCountryIdx;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_AddItemFusingInfo(Nx_MenuHiddenManagementSetContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);


	if (pstContents == NULL)
	{
		NX_APP_Error("(pstContents == NULL)\n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuHiddenManagement_GetItemSupport(pstContents, eNxSettingsItem_HiddenFusingInfo);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_HiddenFusingInfo(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuHiddenManagement_ConvertItemIdxToObjId(eNxSettingsItem_HiddenFusingInfo);

	// Check bDisable Item
	bDisable = nx_menuHiddenManagement_GetItemiDisable(pstContents, eNxSettingsItem_HiddenFusingInfo);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuHiddenManagement_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	stSubItem.pszHelpText = NULL;

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_AddItemPasswordReset(Nx_MenuHiddenManagementSetContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("(pstContents == NULL)\n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuHiddenManagement_GetItemSupport(pstContents, eNxSettingsItem_HiddenPasswordReset);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_HiddenPasswordReset(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuHiddenManagement_ConvertItemIdxToObjId(eNxSettingsItem_HiddenPasswordReset);

	// Check bDisable Item
	bDisable = nx_menuHiddenManagement_GetItemiDisable(pstContents, eNxSettingsItem_HiddenPasswordReset);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuHiddenManagement_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	stSubItem.pszHelpText = NULL;

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_InitSettingsItems(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p2)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)!! \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_InitSubitemArray() Fail!! \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_HIDDEN_SETTINGS_ID), MAX_ELEMENT_TEXT_LENGTH);

	pstContents->eMenuHiddenType = (NxUiMenuHiddenType_t)p2;
	if (pstContents->eMenuHiddenType == eNxUiMenuHiddenType_None)
	{
		NX_APP_Error("Error!!! (pstContents->eMenuHiddenType == eNxUiMenuHiddenType_None) Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	eResult;
	}

	HxSTD_memset(pstContents->szPinCodeStr, '0', sizeof(HUINT8) * (NX_PRISM_MENU_HIDDEN_PIN_DIGIT_MAX));
	pstContents->szPinCodeStr[NX_PRISM_MENU_HIDDEN_PIN_DIGIT_MAX-1] = '\0';

	nx_menuHiddenManagement_AddItemFusingInfo(pstContents);
#if defined(CONFIG_APUI_SET_HIDDEN_INFO_PWD_RESET)
	nx_menuHiddenManagement_AddItemPasswordReset(pstContents);
#endif
#if defined(CONFIG_APUI_SET_HIDDEN_INFO_COUNTRY_CODE)
	nx_menuHiddenManagement_AddItemCountryCode(pstContents);
#endif

	// Default Function
	nx_menuHiddenManagement_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_DrawDefaultUI(Nx_MenuHiddenManagementSetContent_t *pstContents)
{
	Nx_SettingsLeafSubItem_t stFocusItem = pstContents->subItems[pstContents->ulCurFocus];

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuHiddenManagement_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Draw - HelpText
	NX_PRISM_COMP_Settings_DrawHelpText(stFocusItem.pszHelpText);

	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1_Arrow);
	ONDK_GWM_Obj_SetFocus(stFocusItem.ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmCreate(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Load Menu Item Value
	nx_menuHiddenManagement_LoadSettingsItemsValue(pstContents);

	// Init Menu Item
	nx_menuHiddenManagement_InitSettingsItems(pstContents, p2);
	pstContents->lPrevCurIdx = p3;

	// Draw Menu Item
	nx_menuHiddenManagement_DrawDefaultUI(pstContents);

	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Settings);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Settings !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmDestroy(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuHiddenManagementSetContent_t));

	nx_menuHiddenManagement_DestroyDialog();
	nx_menuHiddenManagement_DestroyListPopup();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmHapiMsg(Nx_MenuHiddenManagementSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmKeyDown(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
			lPrevCurIdx = pstContents->lPrevCurIdx;
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_SysInfo_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
								(ONDK_GWM_Callback_t)NX_PRISM_MENU_SysInfo_Proc, 0, 0, 0, lPrevCurIdx);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

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
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmTimer(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuHiddenManagement_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_MsgGwmObjFocusChanged(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulIndex = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	eResult = NX_PRISM_AppContents_GetSubitemIndexByObjId(pstContents, p1, &ulIndex);
	if (eResult == ONDK_RET_OK)
	{
		pstContents->ulCurFocus = ulIndex;
		NX_PRISM_COMP_Settings_UpdateHelpText(pstContents->subItems[pstContents->ulCurFocus].pszHelpText);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenManagement_MsgAppDlgClicked(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lClickedBtn, HINT32 lDlgWhoseId)
{
	HERROR		hError = ERR_FAIL;


	switch (lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:	// YES
			if (lDlgWhoseId == eNxSettingsItem_HiddenPasswordReset)
			{
				nx_menuHiddenManagement_DestroyDialog();
				hError = NX_SYSTEM_ResetPinCode((HCHAR*)pstContents->szPinCodeStr);
				if (hError != ERR_OK)
				{
					NX_APP_Error("Error!!! NX_SYSTEM_ResetPinCode() Faile!! \n");
				}
			}
			else
			{
				NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", lDlgWhoseId);
			}
			break;
		default:						// No
			if (lDlgWhoseId == eNxSettingsItem_HiddenPasswordReset)
			{
				nx_menuHiddenManagement_DestroyDialog();
			}
			else
			{
				NX_APP_Error("Error!!! Unknow lDlgWhoseId ID : (0x%x) !!\n", lDlgWhoseId);
			}
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuHiddenManagement_MsgAppDlgCanceled(Nx_MenuHiddenManagementSetContent_t *pstContents, HINT32 lDlgWhoseId)
{
	if (lDlgWhoseId == eNxSettingsItem_HiddenPasswordReset)
	{
		nx_menuHiddenManagement_DestroyDialog();
	}
	else
	{
		NX_APP_Error("Error!!! Unknow lDlgWhoseId ID : (0x%x) !!\n", lDlgWhoseId);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(lDlgWhoseId);

	return	ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_HiddenManagement_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuHiddenManagementSetContent_t	*pstContents = NULL;

	pstContents = nx_menuHiddenManagement_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuHiddenManagement_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuHiddenManagement_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			nx_menuHiddenManagement_ActionItem(pstContents, p1);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			nx_menuHiddenManagement_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			nx_menuHiddenManagement_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuHiddenManagement_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuHiddenManagement_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuHiddenManagement_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuHiddenManagement_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_FOCUS_CHANGED!\n");
			eResult = nx_menuHiddenManagement_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED! || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_menuHiddenManagement_MsgAppDlgClicked(pstContents, p1, p2);
			break;

		case MSG_APP_DLG_CANCELED:
			NX_APP_Message("MSG_APP_DLG_CLICKED!\n");
			eResult = nx_menuHiddenManagement_MsgAppDlgCanceled(pstContents, p2);
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


HBOOL	NX_PRISM_MENU_HiddenManagement_CheckHiddenKeySeq(HINT32 lKey, HINT32 *peMenuHiddenType)
{
	HBOOL	bHiddenAll = FALSE, bHiddenPwdReset = FALSE, bHiddenCountryCode = FALSE;

	*peMenuHiddenType = eNxUiMenuHiddenType_None;

	bHiddenAll = NX_PRISM_MENU_HiddenManagement_CheckHiddenAllKeySeq(lKey);
	if (bHiddenAll == TRUE)
	{
		*peMenuHiddenType = eNxUiMenuHiddenType_All;
	}

#if defined(CONFIG_APUI_SET_HIDDEN_INFO_PWD_RESET)
	bHiddenPwdReset = NX_PRISM_MENU_HiddenManagement_CheckHiddenPwdResetKeySeq(lKey);
	if (bHiddenPwdReset == TRUE)
	{
		*peMenuHiddenType = eNxUiMenuHiddenType_PasswordReset;
	}
#else
	NX_COMMON_NOT_USED_PARAM(bHiddenPwdReset);
#endif

#if defined(CONFIG_APUI_SET_HIDDEN_INFO_COUNTRY_CODE)
	bHiddenCountryCode = NX_PRISM_MENU_HiddenManagement_CheckHiddenCountryCodeKeySeq(lKey);
	if (bHiddenCountryCode == TRUE)
	{
		*peMenuHiddenType = eNxUiMenuHiddenType_CountryCode;
	}
#else
	NX_COMMON_NOT_USED_PARAM(bHiddenCountryCode);
#endif

#if defined(CONFIG_OP_AMX)
	bHiddenCountryCode = NX_PRISM_MENU_HiddenManagement_CheckHiddenBackDoorKeySeq(lKey);
	if (bHiddenCountryCode == TRUE)
	{
		*peMenuHiddenType = eNxUiMenuHiddenType_CountryCode;
	}
#else
	NX_COMMON_NOT_USED_PARAM(bHiddenCountryCode);
#endif

	if ((bHiddenAll == TRUE) || (bHiddenPwdReset == TRUE) || (bHiddenCountryCode == TRUE))
	{
		return	TRUE;
	}
	else
	{
		return	FALSE;
	}
}


void	NX_PRISM_MENU_HiddenManagement_ResetHiddenKeySeq(void)
{
	s_lMenuhiddenKeySeq = 0;
	s_lMenuhiddenPwdResetKeySeq = 0;
	s_lMenuhiddenCountryCodeKeySeq = 0;
}


HBOOL	NX_PRISM_MENU_HiddenManagement_CheckHiddenAllKeySeq(HINT32 lKey)
{
	switch(lKey)
	{
		case KEY_RED:
			if(s_lMenuhiddenKeySeq == 0)
			{
				s_lMenuhiddenKeySeq++;
			}
			else
			{
				s_lMenuhiddenKeySeq = 0;
			}
			break;

		case KEY_GREEN:
			if((s_lMenuhiddenKeySeq == 1) || (s_lMenuhiddenKeySeq == 4))
			{
				s_lMenuhiddenKeySeq++;
			}
			else
			{
				s_lMenuhiddenKeySeq = 0;
			}
			break;

		case KEY_YELLOW:
			if((s_lMenuhiddenKeySeq == 2) || (s_lMenuhiddenKeySeq == 5))
			{
				s_lMenuhiddenKeySeq++;
			}
			else
			{
				s_lMenuhiddenKeySeq = 0;
			}
			break;

		case KEY_BLUE:
			if(s_lMenuhiddenKeySeq == 3)
			{
				s_lMenuhiddenKeySeq++;
			}
			else if(s_lMenuhiddenKeySeq == 6)
			{
				return TRUE;
			}
			else
			{
				s_lMenuhiddenKeySeq = 0;
			}
			break;

		default:
			s_lMenuhiddenKeySeq = 0;
			break;
	}

	return FALSE;
}


HBOOL	NX_PRISM_MENU_HiddenManagement_CheckHiddenPwdResetKeySeq(HINT32 lKey)
{
	switch(lKey)
	{
		case KEY_7:
			if (s_lMenuhiddenPwdResetKeySeq == 0)
			{
				s_lMenuhiddenPwdResetKeySeq++;
			}
			else
			{
				s_lMenuhiddenPwdResetKeySeq = 0;
			}
			break;

		case KEY_4:
			if (s_lMenuhiddenPwdResetKeySeq == 1)
			{
				s_lMenuhiddenPwdResetKeySeq++;
			}
			else
			{
				s_lMenuhiddenPwdResetKeySeq = 0;
			}
			break;

		case KEY_1:
			if (s_lMenuhiddenPwdResetKeySeq == 2)
			{
				s_lMenuhiddenPwdResetKeySeq++;
			}
			else
			{
				s_lMenuhiddenPwdResetKeySeq = 0;
			}
			break;

		case KEY_2:
			if (s_lMenuhiddenPwdResetKeySeq == 3)
			{
				s_lMenuhiddenPwdResetKeySeq++;
			}
			else
			{
				s_lMenuhiddenPwdResetKeySeq = 0;
			}
			break;

		case KEY_3:
			if (s_lMenuhiddenPwdResetKeySeq == 4)
			{
				s_lMenuhiddenPwdResetKeySeq++;
			}
			else
			{
				s_lMenuhiddenPwdResetKeySeq = 0;
			}
			break;

		case KEY_5:
			if (s_lMenuhiddenPwdResetKeySeq == 5)
			{
				s_lMenuhiddenPwdResetKeySeq++;
			}
			else
			{
				s_lMenuhiddenPwdResetKeySeq = 0;
			}
			break;

		case KEY_9:
			if (s_lMenuhiddenPwdResetKeySeq == 6)
			{
				return TRUE;
			}
			else
			{
				s_lMenuhiddenPwdResetKeySeq = 0;
			}
			break;

		default:
			s_lMenuhiddenPwdResetKeySeq = 0;
			break;
	}

	return FALSE;
}


HBOOL	NX_PRISM_MENU_HiddenManagement_CheckHiddenCountryCodeKeySeq(HINT32 lKey)
{
	switch(lKey)
	{
		case KEY_6:
			if (s_lMenuhiddenCountryCodeKeySeq == 0)
			{
				s_lMenuhiddenCountryCodeKeySeq++;
			}
			else
			{
				s_lMenuhiddenCountryCodeKeySeq = 0;
			}
			break;

		case KEY_2:
			if (s_lMenuhiddenCountryCodeKeySeq == 1)
			{
				s_lMenuhiddenCountryCodeKeySeq++;
			}
			else
			{
				s_lMenuhiddenCountryCodeKeySeq = 0;
			}
			break;

		case KEY_4:
			if (s_lMenuhiddenCountryCodeKeySeq == 2)
			{
				s_lMenuhiddenCountryCodeKeySeq++;
			}
			else
			{
				s_lMenuhiddenCountryCodeKeySeq = 0;
			}
			break;

		case KEY_7:
			if (s_lMenuhiddenCountryCodeKeySeq == 3)
			{
				s_lMenuhiddenCountryCodeKeySeq++;
			}
			else
			{
				s_lMenuhiddenCountryCodeKeySeq = 0;
			}
			break;

		case KEY_0:
			if (s_lMenuhiddenCountryCodeKeySeq == 4)
			{
				return TRUE;
			}
			else
			{
				s_lMenuhiddenCountryCodeKeySeq = 0;
			}
			break;

		default:
			s_lMenuhiddenCountryCodeKeySeq = 0;
			break;
	}

	return FALSE;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
